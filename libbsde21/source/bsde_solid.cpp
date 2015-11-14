#include "bsde_internal.h"

static BSDE_Contact *bsde_contactpool=NULL;
static int bsde_contactpool_pos=0;

BSDE_Contact *BSDE_MakeContact(
	BSDE_World *world,
	BSDE_Solid *obja, BSDE_Solid *objb,
	bsde_real *org, bsde_real *norm, bsde_real depth)
{
	BSDE_Contact *tmp;

	if(!bsde_contactpool)
	{
		bsde_contactpool=(BSDE_Contact *)malloc(BSDE_MAX_CONTACTS*
			sizeof(BSDE_Contact));
		bsde_contactpool_pos=0;
	}

	if(bsde_contactpool_pos>=BSDE_MAX_CONTACTS)
		return(NULL);

	tmp=&bsde_contactpool[bsde_contactpool_pos++];

	tmp->obja=obja;
	tmp->objb=objb;
	tmp->depth=depth;
	V3_COPY(org, tmp->org);
	V3_COPY(norm, tmp->norm);
	tmp->npts=0;

	return(tmp);
}

BSDE_Contact *BSDE_MakePointsContact(
	BSDE_World *world,
	BSDE_Solid *obja, BSDE_Solid *objb,
	bsde_real *pts, bsde_real *norm, bsde_real depth, int npts)
{
	BSDE_Contact *tmp;
	bsde_real org[3];
	int i;

	if(!bsde_contactpool)
	{
		bsde_contactpool=(BSDE_Contact *)malloc(BSDE_MAX_CONTACTS*
			sizeof(BSDE_Contact));
		bsde_contactpool_pos=0;
	}

	if(bsde_contactpool_pos>=BSDE_MAX_CONTACTS)
		return(NULL);

	tmp=&bsde_contactpool[bsde_contactpool_pos++];

//	printf("Make Points Contact %d (%g %g %g) %g\n", npts,
//		norm[0], norm[1], norm[2], depth);
//	for(i=0; i<npts; i++)
//		printf("\t(%g %g %g)\n", pts[i*3+0], pts[i*3+1], pts[i*3+2]);

	V3_ZERO(org);
	for(i=0; i<npts; i++)
	{
		V3_ADD(org, pts+i*4, org);
		V4_COPY(pts+i*4, tmp->pts+i*4);
	}
	V3_SCALE(org, 1.0/npts, org);
	tmp->npts=npts;

	tmp->obja=obja;
	tmp->objb=objb;
	tmp->depth=depth;
	V3_COPY(org, tmp->org);
	V3_COPY(norm, tmp->norm);

	return(tmp);
}

void BSDE_FreeContacts()
{
	bsde_contactpool_pos=0;
}

BSDE_Solid *BSDE_NewSolidArray(BSDE_World *world,
	int type, int **arr,
	bsde_real **varr, bsde_real **sarr)
{
	BSDE_Solid *tmp;

	switch(type)
	{
	case BSDE_SOLID_BSP:
		tmp=BSDE_SolidBSP_BuildArray(world, arr, varr);
		break;
	default:
		tmp=NULL;
		break;
	}
	return(tmp);
}

void BSDE_ArraySkipUnknown(int **arr, bsde_real **varr)
{
	switch(**arr)
	{
	case BSDE_TAG_VERTEX:
		*arr=*arr+1;
		*varr=*varr+4;
		break;
	case BSDE_TAG_NORMAL:
		*arr=*arr+1;
		*varr=*varr+4;
		break;
	case BSDE_TAG_BEGIN:
		*arr=*arr+2;
		*varr=*varr+4;
		while(**arr!=BSDE_TAG_END)
			BSDE_ArraySkipUnknown(arr, varr);
		*arr=*arr+1;
		break;
	defualt:
		printf("BSDE_ArraySkipUnknown: Unknown tag %d\n", **arr);
		*arr=*arr+1;
		break;
	}
}

BSDE_Solid *BSDE_NewSolid(int type)
{
	BSDE_Solid *tmp;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=type;
	BSDE_Quat_Identity(tmp->rot);

	V3_CONST(tmp->mins, -1);
	V3_CONST(tmp->maxs, 1);
	tmp->radius=1;
	tmp->height=1;
	tmp->volume=8;
	tmp->mass=8;

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;

	return(tmp);
}

BSDE_Solid *BSDE_NewAABB(
	bsde_real mx, bsde_real my, bsde_real mz,
	bsde_real nx, bsde_real ny, bsde_real nz)
{
	BSDE_Solid *tmp;
	bsde_real f, g;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_AABB;
	BSDE_Quat_Identity(tmp->rot);

	V3_SET(tmp->mins, mx, my, mz);
	V3_SET(tmp->maxs, nx, ny, nz);
	tmp->volume=(nx-mx)*(ny-my)*(nz-mz);
	tmp->mass=tmp->volume;

	f=V3_LEN(tmp->mins);
	g=V3_LEN(tmp->maxs);
	if(g>f)f=g;
	tmp->radius=f;

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;
	BSDE_BuildInertiaTensor(tmp);

	return(tmp);
}

BSDE_Solid *BSDE_NewOBB(
	bsde_real mx, bsde_real my, bsde_real mz,
	bsde_real nx, bsde_real ny, bsde_real nz)
{
	BSDE_Solid *tmp;

	tmp=BSDE_NewAABB(mx, my, mz, nx, ny, nz);
	tmp->solidtype=BSDE_SOLID_OBB;
	return(tmp);
}

BSDE_Solid *BSDE_NewSphere(bsde_real rad)
{
	BSDE_Solid *tmp;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_SPHERE;
	BSDE_Quat_Identity(tmp->rot);

	tmp->radius=rad;
	tmp->volume=(4.0/3.0)*BSDE_M_PI*(rad*rad);
	tmp->mass=tmp->volume;

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;
	BSDE_BuildInertiaTensor(tmp);

	return(tmp);
}

BSDE_Solid *BSDE_NewCylinder(bsde_real height, bsde_real radius)
{
	BSDE_Solid *tmp;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_CYLINDER;
	BSDE_Quat_Identity(tmp->rot);

	tmp->height=height;
	tmp->radius=radius;
	tmp->volume=BSDE_M_PI*(radius*radius)*height;
	tmp->mass=tmp->volume;

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;
	BSDE_BuildInertiaTensor(tmp);

	return(tmp);
}

BSDE_Solid *BSDE_NewCappedCylinder(bsde_real height, bsde_real radius)
{
	BSDE_Solid *tmp;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_CCYLINDER;
	BSDE_Quat_Identity(tmp->rot);

	tmp->height=height;
	tmp->radius=radius;

	tmp->volume=((4.0/3.0)+height)*BSDE_M_PI*(radius*radius);
	tmp->mass=tmp->volume;
	BSDE_BuildInertiaTensor(tmp);

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;

	return(tmp);
}

BSDE_Solid *BSDE_NewPlane(bsde_real x, bsde_real y, bsde_real z, bsde_real d)
{
	BSDE_Solid *tmp;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_PLANE;
	BSDE_Quat_Identity(tmp->rot);

	tmp->dir[0]=x;
	tmp->dir[1]=y;
	tmp->dir[2]=z;
	tmp->dir[3]=d;

	V3_NORMALIZE(tmp->dir, tmp->dir);

	//so there is something sane here...
	tmp->volume=999999999999.0;
	tmp->mass=999999999999.0;

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;

	return(tmp);
}

BSDE_Solid *BSDE_NewHull(bsde_real *norms, int n)
{
	BSDE_Solid *tmp;
	int i;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_HULL;
	BSDE_Quat_Identity(tmp->rot);

	tmp->norms=(bsde_real *)malloc(4*n*sizeof(bsde_real));
	tmp->n_faces=n;
	for(i=0; i<n; i++)
	{
		V3_NORMALIZE(norms+i*4, tmp->norms+i*4);
		tmp->norms[i*4+3]=norms[i*4+3];
	}

	BSDE_SolidHull_MakeHullFaces(tmp);
	tmp->mass=tmp->volume;

	BSDE_BuildInertiaTensor(tmp);

#if 0
	tmp->mins[0]=-1;
	tmp->mins[1]=-1;
	tmp->mins[2]=-1;
	tmp->maxs[0]=1;
	tmp->maxs[1]=1;
	tmp->maxs[2]=1;
	BSDE_BuildBoxInertiaTensor(tmp);
#endif

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;

	return(tmp);
}

BSDE_Solid *BSDE_NewTriMesh(bsde_real *vecs, int *tris, int nv, int nt)
{
	BSDE_Solid *tmp;
	bsde_real tv[3], tv0[3], tv1[3], tv2[3];
	bsde_real *v0, *v1, *v2;
	int i;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_TRIMESH;
	BSDE_Quat_Identity(tmp->rot);

	tmp->vecs=(bsde_real *)malloc(3*nv*sizeof(bsde_real));
	tmp->faces=(int *)malloc(3*nt*sizeof(int));
	tmp->norms=(bsde_real *)malloc(4*nt*sizeof(bsde_real));
	tmp->evecs=(bsde_real *)malloc(3*4*nt*sizeof(bsde_real));
	tmp->n_vecs=nv;
	tmp->n_faces=nt;

	V3_INITBOX(tmp->mins, tmp->maxs);
	for(i=0; i<nv; i++)
	{
		V3_COPY(vecs+i*3, tmp->vecs+i*3);

		V3_COPY(vecs+i*3, tv);
		V3_MIN(tmp->mins, tv, tmp->mins);
		V3_MAX(tmp->maxs, tv, tmp->maxs);
	}

	for(i=0; i<nt; i++)
	{
		tmp->faces[i*3+0]=tris[i*3+0];
		tmp->faces[i*3+1]=tris[i*3+1];
		tmp->faces[i*3+2]=tris[i*3+2];

		v0=vecs+tris[i*3+0]*3;
		v1=vecs+tris[i*3+1]*3;
		v2=vecs+tris[i*3+2]*3;

		V3_SUB(v1, v0, tv0);
		V3_SUB(v2, v0, tv1);
//		V3_CROSS(tv2, tv1, tv);
		V3_CROSS(tv1, tv0, tv);

		V3_NORMALIZE(tv, tmp->norms+i*4);
		tmp->norms[i*4+3]=V3_DOT(v0, tmp->norms+i*4);

		V3_SUB(v1, v0, tv0);
		V3_SUB(v2, v1, tv1);
		V3_SUB(v0, v2, tv2);

		V3_CROSS(tv0, tmp->evecs+i*12+0*4, tv);
		tmp->norms[i*12+0*4+3]=V3_DOT(v0, tmp->evecs+i*12+0*4);

		V3_CROSS(tv1, tmp->evecs+i*12+1*4, tv);
		tmp->norms[i*12+1*4+3]=V3_DOT(v1, tmp->evecs+i*12+1*4);

		V3_CROSS(tv2, tmp->evecs+i*12+2*4, tv);
		tmp->norms[i*12+2*4+3]=V3_DOT(v2, tmp->evecs+i*12+2*4);
	}


//	v0=malloc(3*nv*sizeof(bsde_real));

	tmp->volume=BSDE_SolidBRep_ApproxBrepVolume(vecs, tris, nv, nt);
	tmp->mass=tmp->volume;

	BSDE_BuildInertiaTensor(tmp);

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;

	return(tmp);
}

BSDE_Solid *BSDE_NewVoxelChunk(int xs, int ys, int zs, bsde_real vsz)
{
	BSDE_Solid *tmp;

	tmp=(BSDE_Solid *)malloc(sizeof(BSDE_Solid));
	memset(tmp, 0, sizeof(BSDE_Solid));
	tmp->solidtype=BSDE_SOLID_VOXELS;
	BSDE_Quat_Identity(tmp->rot);

	tmp->vox_xs=xs;
	tmp->vox_ys=ys;
	tmp->vox_zs=zs;
	tmp->vox_vsz=vsz;

	V3_SET(tmp->mins, -xs*vsz*0.5, -ys*vsz*0.5, -zs*vsz*0.5);
	V3_SET(tmp->maxs,  xs*vsz*0.5,  ys*vsz*0.5,  zs*vsz*0.5);

	tmp->radius=vsz*sqrt((double)(xs*xs+ys*ys+zs*zs))*0.5;
	tmp->volume=xs*ys*zs*(vsz*vsz*vsz);
	tmp->mass=tmp->volume;

	tmp->elastic=BSDE_M_DEF_ELASTIC;
	tmp->friction=BSDE_M_DEF_FRICTION;
	BSDE_BuildInertiaTensor(tmp);

	return(tmp);
}


void BSDE_Destroy(BSDE_Solid *self)
{
	if(!self)return;

	//FIXME: better handle bsp's, hulls, ...
	if(self->vecs)free(self->vecs);
	if(self->norms)free(self->norms);
	if(self->faces)free(self->faces);
	if(self->facevecs)free(self->facevecs);

	if((self->solidtype==BSDE_SOLID_BSP) && self->data)
		BSDE_SolidBSP_DestroyNode((BSDE_BSPNode *)self->data);

	free(self);
}

void BSDE_Box(BSDE_Solid *self, bsde_real *mins, bsde_real *maxs)
{
	if(!self)return;

	switch(self->solidtype)
	{
	case BSDE_SOLID_AABB:
		V3_ADD(self->org, self->mins, mins);
		V3_ADD(self->org, self->maxs, maxs);
		break;
	case BSDE_SOLID_OBB:
		BSDE_SolidOBB_Box(self->mins, self->maxs,
			self->xform, mins, maxs);
		break;
	case BSDE_SOLID_SPHERE:
		V3_SUBCONST(self->org, self->radius, mins);
		V3_ADDCONST(self->org, self->radius, maxs);
		break;
	case BSDE_SOLID_CYLINDER:
		BSDE_SolidCylinder_MakeBox(self->height, self->radius, 0,
			self->xform, mins, maxs);
		break;
	case BSDE_SOLID_CCYLINDER:
		BSDE_SolidCylinder_MakeBox(self->height, self->radius, 1,
			self->xform, mins, maxs);
		break;

	case BSDE_SOLID_PLANE:
		V3_CONST(mins, -999999999);
		V3_CONST(maxs, 999999999);
		break;

	case BSDE_SOLID_TRIMESH:	//inaccurate
		BSDE_SolidOBB_Box(self->mins, self->maxs,
			self->xform, mins, maxs);
		break;
	case BSDE_SOLID_HULL:
		BSDE_SolidHull_Box(self, self->xform, mins, maxs);
		break;
	case BSDE_SOLID_BSP:
		V3_ADD(self->org, self->mins, mins);
		V3_ADD(self->org, self->maxs, maxs);
		break;

	default:
		break;
	}
}

void BSDE_BoxMove(BSDE_Solid *self, bsde_real *mins, bsde_real *maxs)
{
	bsde_real f, g;
	int i;

	switch(self->solidtype)
	{
	case BSDE_SOLID_AABB:
		for(i=0; i<3; i++)
		{
			f=self->mins[i]+self->org[i];
			g=self->mins[i]+self->e_org[i];
			mins[i]=(f<g)?f:g;

			f=self->maxs[i]+self->org[i];
			g=self->maxs[i]+self->e_org[i];
			maxs[i]=(f>g)?f:g;
		}
		break;
	case BSDE_SOLID_OBB:
		BSDE_SolidOBB_BoxMove(self->mins, self->maxs,
			self->xform, self->e_xform, mins, maxs);
		break;
	case BSDE_SOLID_SPHERE:
		for(i=0; i<3; i++)
		{
			f=self->org[i]-self->radius;
			g=self->e_org[i]-self->radius;
			mins[i]=(f<g)?f:g;

			f=self->org[i]+self->radius;
			g=self->e_org[i]+self->radius;
			maxs[i]=(f>g)?f:g;
		}
		break;
	case BSDE_SOLID_CYLINDER:
		BSDE_SolidCylinder_BoxMove(self->height, self->radius, 0,
			self->xform, self->e_xform, mins, maxs);
		break;
	case BSDE_SOLID_CCYLINDER:
		BSDE_SolidCylinder_BoxMove(self->height, self->radius, 1,
			self->xform, self->e_xform, mins, maxs);
		break;
	case BSDE_SOLID_PLANE:
		V3_CONST(mins, -999999999);
		V3_CONST(maxs, 999999999);
		break;

	case BSDE_SOLID_TRIMESH:
		BSDE_SolidOBB_BoxMove(self->mins, self->maxs,
			self->xform, self->e_xform, mins, maxs);
		break;
	case BSDE_SOLID_HULL:
		BSDE_SolidHull_BoxMove(self, self->xform, self->e_xform,
			mins, maxs);
		break;
	case BSDE_SOLID_BSP:
		V3_ADD(self->org, self->mins, mins);
		V3_ADD(self->org, self->maxs, maxs);
		break;
	default:
		break;
	}
}

void BSDE_BoxMovePos(BSDE_Solid *self,
	bsde_real *sxform, bsde_real *exform,
	bsde_real *sorg, bsde_real *eorg,
	bsde_real *mins, bsde_real *maxs)
{
	bsde_real f, g;
	int i;

	switch(self->solidtype)
	{
	case BSDE_SOLID_AABB:
		for(i=0; i<3; i++)
		{
			f=self->mins[i]+sorg[i];
			g=self->mins[i]+eorg[i];
			mins[i]=(f<g)?f:g;

			f=self->maxs[i]+sorg[i];
			g=self->maxs[i]+eorg[i];
			maxs[i]=(f>g)?f:g;
		}
		break;
	case BSDE_SOLID_OBB:
		BSDE_SolidOBB_BoxMove(self->mins, self->maxs,
			sxform, exform, mins, maxs);
		break;
	case BSDE_SOLID_SPHERE:
		for(i=0; i<3; i++)
		{
			f=sorg[i]-self->radius;
			g=eorg[i]-self->radius;
			mins[i]=(f<g)?f:g;

			f=sorg[i]+self->radius;
			g=eorg[i]+self->radius;
			maxs[i]=(f>g)?f:g;
		}
		break;
	case BSDE_SOLID_CYLINDER:
		BSDE_SolidCylinder_BoxMove(self->height, self->radius, 0,
			sxform, exform, mins, maxs);
		break;
	case BSDE_SOLID_CCYLINDER:
		BSDE_SolidCylinder_BoxMove(self->height, self->radius, 1,
			sxform, exform, mins, maxs);
		break;
	case BSDE_SOLID_PLANE:
		V3_CONST(mins, -999999999);
		V3_CONST(maxs, 999999999);
		break;

	case BSDE_SOLID_TRIMESH:
		BSDE_SolidOBB_BoxMove(self->mins, self->maxs,
			sxform, exform, mins, maxs);
		break;
	case BSDE_SOLID_HULL:
		BSDE_SolidHull_BoxMove(self, sxform, exform, mins, maxs);
		break;
	case BSDE_SOLID_BSP:
		for(i=0; i<3; i++)
		{
			f=self->mins[i]+sorg[i];
			g=self->mins[i]+eorg[i];
			mins[i]=(f<g)?f:g;

			f=self->maxs[i]+sorg[i];
			g=self->maxs[i]+eorg[i];
			maxs[i]=(f>g)?f:g;
		}
		break;
	default:
		break;
	}
}


bsde_real BSDE_GetAttrF(BSDE_Solid *self, int var)
{
	bsde_real f;

	if(!self)return(0);

	f=0;
	switch(var)
	{
	case BSDE_HEIGHT:
		f=self->height/self->world->sc_len;
		break;
	case BSDE_RADIUS:
		f=self->radius/self->world->sc_len;
		break;

	case BSDE_MASS:
		f=self->mass/self->world->sc_mass;
		break;
	case BSDE_VOLUME:
		f=self->volume/self->world->sc_vol;
		break;
	case BSDE_DENSITY:
		f=(self->mass/self->world->sc_mass)/
			(self->volume/self->world->sc_vol);
		break;
	case BSDE_ELASTIC:
		f=self->elastic;
		break;
	case BSDE_FRICTION:
		f=self->friction;
		break;
	case BSDE_VISCOSITY:
		f=self->friction;
		break;
	case BSDE_CONTACT_POWER:
		f=self->u_wpow;
		break;
	case BSDE_INERTIA_SCALAR:
		f=self->moment;
		break;

	case BSDE_VOXEL_SIZE:
		f=self->vox_vsz;
		break;

	default:
		break;
	}
	return(f);
}

void BSDE_SetAttrF(BSDE_Solid *self, int var, bsde_real val)
{
	if(!self)return;

	switch(var)
	{
	case BSDE_HEIGHT:
		self->height=val*self->world->sc_len;
		break;
	case BSDE_RADIUS:
		self->radius=val*self->world->sc_len;
		break;

	case BSDE_MASS:
		self->mass=val*self->world->sc_mass;
		break;
	case BSDE_VOLUME:
		self->volume=val*self->world->sc_vol;
		break;
	case BSDE_DENSITY:
//		self->mass=val*self->volume;
		self->mass=(val*self->world->sc_mass)*
			(self->volume/self->world->sc_vol);
		break;
	case BSDE_ELASTIC:
		self->elastic=val;
		break;
	case BSDE_FRICTION:
		self->friction=val;
		break;
	case BSDE_VISCOSITY:
		self->viscosity=val;
		break;
	case BSDE_CONTACT_POWER:
		self->u_wpow=val;
		break;
	case BSDE_INERTIA_SCALAR:
		self->moment=val;
		break;

	case BSDE_VOXEL_SIZE:
		self->vox_vsz=val;
		break;

	default:
		break;
	}

	BSDE_Awaken(self);
}

int BSDE_GetAttrI(BSDE_Solid *self, int var)
{
	int i;

	if(!self)return(-1);

	i=0;
	switch(var)
	{
	case BSDE_SOLIDTYPE:
		i=self->solidtype;
		break;
	case BSDE_NOSPIN:
		i=(self->moveflags&BSDE_MOVEFL_NOSPIN)!=0;
		break;
	case BSDE_NOMOVE:
		i=(self->moveflags&BSDE_MOVEFL_NOMOVE)!=0;
		break;
	case BSDE_NOGRAVITY:
		i=(self->moveflags&BSDE_MOVEFL_NOGRAVITY)!=0;
		break;
	case BSDE_STATIC:
		i=(self->moveflags&BSDE_MOVEFL_STATIC)!=0;
		break;
	case BSDE_SEMISTATIC:
		i=(self->moveflags&BSDE_MOVEFL_SEMISTATIC)!=0;
		break;
	case BSDE_FLUID:
		i=(self->moveflags&BSDE_MOVEFL_FLUID)!=0;
		break;
	case BSDE_NONSOLID:
		i=(self->moveflags&BSDE_MOVEFL_NONSOLID)!=0;
		break;
	case BSDE_TRIGGER:
		i=(self->moveflags&BSDE_MOVEFL_TRIGGER)!=0;
		break;

	case BSDE_ACTOR:
		i=(self->moveflags&BSDE_MOVEFL_ACTOR)!=0;
		break;
	case BSDE_WANTED_ORIGIN:
		i=(self->moveflags&BSDE_MOVEFL_WANTORG)!=0;
		break;
	case BSDE_WANTED_VELOCITY:
		i=(self->moveflags&BSDE_MOVEFL_WANTVEL)!=0;
		break;
	case BSDE_WANTED_ROTATION:
		i=(self->moveflags&BSDE_MOVEFL_WANTROT)!=0;
		break;

	case BSDE_NUM_FACES:
		i=self->n_faces;
		break;
	case BSDE_NUM_VERTICES:
		i=self->n_vecs;
		break;

	case BSDE_CHUNK_XS:
		i=self->vox_xs;
		break;
	case BSDE_CHUNK_YS:
		i=self->vox_ys;
		break;
	case BSDE_CHUNK_ZS:
		i=self->vox_zs;
		break;

	default:
		break;
	}
	return(i);
}

void BSDE_SetAttrI(BSDE_Solid *self, int var, int val)
{
	int i;

	if(!self)return;

	switch(var)
	{
	case BSDE_NOSPIN:
		if(val)self->moveflags|=BSDE_MOVEFL_NOSPIN;
			else self->moveflags&=~BSDE_MOVEFL_NOSPIN;
		break;
	case BSDE_NOMOVE:
		if(val)self->moveflags|=BSDE_MOVEFL_NOMOVE;
			else self->moveflags&=~BSDE_MOVEFL_NOMOVE;
		break;
	case BSDE_NOGRAVITY:
		if(val)self->moveflags|=BSDE_MOVEFL_NOGRAVITY;
			else self->moveflags&=~BSDE_MOVEFL_NOGRAVITY;
		break;
	case BSDE_STATIC:
		i=BSDE_MOVEFL_NOSPIN|BSDE_MOVEFL_NOMOVE|
			BSDE_MOVEFL_NOGRAVITY|BSDE_MOVEFL_STATIC;
		if(val)self->moveflags|=i;
			else self->moveflags&=~i;
		break;
	case BSDE_SEMISTATIC:
		i=BSDE_MOVEFL_NOSPIN|BSDE_MOVEFL_NOMOVE|
			BSDE_MOVEFL_NOGRAVITY|BSDE_MOVEFL_SEMISTATIC;
		if(val)self->moveflags|=i;
			else self->moveflags&=~i;
		break;

	case BSDE_FLUID:
		if(val)self->moveflags|=BSDE_MOVEFL_FLUID;
			else self->moveflags&=~BSDE_MOVEFL_FLUID;
		break;
	case BSDE_NONSOLID:
		if(val)self->moveflags|=BSDE_MOVEFL_NONSOLID;
			else self->moveflags&=~BSDE_MOVEFL_NONSOLID;
		break;
	case BSDE_TRIGGER:
		if(val)self->moveflags|=BSDE_MOVEFL_TRIGGER;
			else self->moveflags&=~BSDE_MOVEFL_TRIGGER;
		break;

	case BSDE_ACTOR:
		if(val)self->moveflags|=BSDE_MOVEFL_ACTOR;
			else self->moveflags&=~BSDE_MOVEFL_ACTOR;
		break;
	case BSDE_WANTED_ORIGIN:
		if(val)self->moveflags|=BSDE_MOVEFL_WANTORG;
			else self->moveflags&=~BSDE_MOVEFL_WANTORG;
		break;
	case BSDE_WANTED_VELOCITY:
		if(val)self->moveflags|=BSDE_MOVEFL_WANTVEL;
			else self->moveflags&=~BSDE_MOVEFL_WANTVEL;
		break;
	case BSDE_WANTED_ROTATION:
		if(val)self->moveflags|=BSDE_MOVEFL_WANTROT;
			else self->moveflags&=~BSDE_MOVEFL_WANTROT;
		break;

	case BSDE_CHUNK_XS:
		self->vox_xs=val;
		break;
	case BSDE_CHUNK_YS:
		self->vox_ys=val;
		break;
	case BSDE_CHUNK_ZS:
		self->vox_zs=val;
		break;

	default:
		break;
	}

	BSDE_Awaken(self);
}

void BSDE_GetAttrFV(BSDE_Solid *self, int var, float *val, int n)
{
	float tv[4];
	int i, j;

	if(!self)return;

	switch(var)
	{
	case BSDE_ORIGIN:
		V3_SCALE(self->org, 1.0/self->world->sc_len, val);
		break;
	case BSDE_VELOCITY:
		V3_SCALE(self->vel, 1.0/self->world->sc_len, val);
		break;
	case BSDE_TORQUE:
		V3_COPY(self->tvel, val);
		break;
	case BSDE_ANGULAR_VELOCITY:
		V3_COPY(self->avel, val);
		break;
	case BSDE_ROTATION:
		V4_COPY(self->rot, val);
		break;

	case BSDE_MINS:
		V3_SCALE(self->mins, 1.0/self->world->sc_len, val);
		break;
	case BSDE_MAXS:
		V3_SCALE(self->maxs, 1.0/self->world->sc_len, val);
		break;

	case BSDE_MASS_OFFSET:
		V3_SCALE(self->mass_offs, 1.0/self->world->sc_len, val);
		break;
	case BSDE_INERTIA_TENSOR:
		for(i=0; i<9; i++)val[i]=self->mass_imat[i];
//		BSDE_Mat32F_Copy(self->mass_imat, val);
		break;

	case BSDE_DIRECTION:
		V3_COPY(self->dir, val);
		val[3]=self->dir[3]/self->world->sc_len;
		break;
	case BSDE_ROTATION_MATRIX:
		for(i=0; i<3; i++)for(j=0; j<3; j++)
			val[i*3+j]=self->xform[i*4+j];
		break;
	case BSDE_VERTICES:
		for(i=0; i<n; i++)val[i]=self->vecs[i]/self->world->sc_len;
		break;

	case BSDE_CONTACT_VELOCITY:
		V3_SCALE(self->u_wvel, 1.0/self->world->sc_len, val);
		break;
	case BSDE_ACCELERATION:
		V3_SCALE(self->u_acc, 1.0/self->world->sc_len, val);
		break;
	case BSDE_LOCAL_THRUST:
		V3_SCALE(self->u_racc, 1.0/self->world->sc_len, val);
		break;

	case BSDE_WANTED_ORIGIN:
		V3_SCALE(self->u_wantorg, 1.0/self->world->sc_len, val);
		break;
	case BSDE_WANTED_VELOCITY:
		V3_SCALE(self->u_wantvel, 1.0/self->world->sc_len, val);
		break;
	case BSDE_WANTED_ROTATION:
		V4_COPY(self->u_wantorg, val);
		break;

	case BSDE_MMINS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SCALE(self->mmins, 1.0/self->world->sc_len, val);
		break;
	case BSDE_MMAXS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SCALE(self->mmaxs, 1.0/self->world->sc_len, val);
		break;
	case BSDE_RMINS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SUB(self->mmins, self->org, tv);
		V3_SCALE(tv, 1.0/self->world->sc_len, val);
		break;
	case BSDE_RMAXS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SUB(self->mmaxs, self->org, tv);
		V3_SCALE(tv, 1.0/self->world->sc_len, val);
		break;

	default:
		break;
	}
}

void BSDE_SetAttrFV(BSDE_Solid *self, int var, float *val, int n)
{
	bsde_real tv[9];
	int i;

	if(!self)return;

	switch(var)
	{
	case BSDE_ORIGIN:
		V3_SCALE(val, self->world->sc_len, self->org);
		break;
	case BSDE_VELOCITY:
		V3_SCALE(val, self->world->sc_len, self->vel);
		break;
	case BSDE_TORQUE:
		V3_COPY(val, self->tvel);
		break;
	case BSDE_ANGULAR_VELOCITY:
		BSDE_WorldInertiaTensor(self);
		V3_COPY(val, self->avel);
		BSDE_MatN_MatMult(self->wtmat, 3, 3, self->avel, 3, 1,
			self->tvel);
		break;
	case BSDE_ROTATION:
		V4_COPY(val, self->rot);
		break;
	case BSDE_ROTATION_MATRIX:
		for(i=0; i<9; i++)tv[i]=val[i];
		BSDE_Quat_From3Matrix(tv, self->rot);
		break;

	case BSDE_MINS:
		V3_SCALE(val, self->world->sc_len, self->mins);
		break;
	case BSDE_MAXS:
		V3_SCALE(val, self->world->sc_len, self->maxs);
		break;
	case BSDE_MASS_OFFSET:
		V3_SCALE(val, self->world->sc_len, self->mass_offs);
		break;
	case BSDE_INERTIA_TENSOR:
//		BSDE_Mat3F2D_Copy(val, self->mass_imat);
		for(i=0; i<9; i++)self->mass_imat[i]=val[i];

		BSDE_MatN_Inverse(self->mass_imat, self->mass_tmat, 3);
		break;

	case BSDE_DIRECTION:
		V3_COPY(val, self->dir);
		self->dir[3]=val[3]*self->world->sc_len;
		break;

	case BSDE_CONTACT_VELOCITY:
		V3_SCALE(val, self->world->sc_len, self->u_wvel);
		break;
	case BSDE_ACCELERATION:
		V3_SCALE(val, self->world->sc_len, self->u_acc);
		break;
	case BSDE_LOCAL_THRUST:
		V3_SCALE(val, self->world->sc_len, self->u_racc);
		break;

	case BSDE_WANTED_ORIGIN:
		V3_SCALE(val, self->world->sc_len, self->u_wantorg);
		break;
	case BSDE_WANTED_VELOCITY:
		V3_SCALE(val, self->world->sc_len, self->u_wantvel);
		break;
	case BSDE_WANTED_ROTATION:
		V4_COPY(val, self->u_wantrot);
		break;

	default:
		break;
	}

	BSDE_Awaken(self);
}

void BSDE_GetAttrDV(BSDE_Solid *self, int var, double *val, int n)
{
	bsde_real tv[9];
	int i, j;

	if(!self)return;

	switch(var)
	{
	case BSDE_ORIGIN:
		V3_SCALE(self->org, 1.0/self->world->sc_len, val);
		break;
	case BSDE_VELOCITY:
		V3_SCALE(self->vel, 1.0/self->world->sc_len, val);
		break;
	case BSDE_TORQUE:
		V3_COPY(self->tvel, val);
		break;
	case BSDE_ANGULAR_VELOCITY:
		V3_COPY(self->avel, val);
		break;
	case BSDE_ROTATION:
		V4_COPY(self->rot, val);
		break;

	case BSDE_MINS:
		V3_SCALE(self->mins, 1.0/self->world->sc_len, val);
		break;
	case BSDE_MAXS:
		V3_SCALE(self->maxs, 1.0/self->world->sc_len, val);
		break;

	case BSDE_MASS_OFFSET:
		V3_SCALE(self->mass_offs, 1.0/self->world->sc_len, val);
		break;
	case BSDE_INERTIA_TENSOR:
//		BSDE_Mat3_Copy(self->mass_imat, val);
		for(i=0; i<9; i++)
			val[i]=self->mass_imat[i];
		break;

	case BSDE_DIRECTION:
		V3_COPY(self->dir, val);
		val[3]=self->dir[3]/self->world->sc_len;
		break;
	case BSDE_ROTATION_MATRIX:
		for(i=0; i<3; i++)for(j=0; j<3; j++)
			val[i*3+j]=self->xform[i*4+j];
		break;
	case BSDE_VERTICES:
		for(i=0; i<n; i++)val[i]=self->vecs[i]/self->world->sc_len;
		break;

	case BSDE_CONTACT_VELOCITY:
		V3_SCALE(self->u_wvel, 1.0/self->world->sc_len, val);
		break;
	case BSDE_ACCELERATION:
		V3_SCALE(self->u_acc, 1.0/self->world->sc_len, val);
		break;
	case BSDE_LOCAL_THRUST:
		V3_SCALE(self->u_racc, 1.0/self->world->sc_len, val);
		break;

	case BSDE_WANTED_ORIGIN:
		V3_SCALE(self->u_wantorg, 1.0/self->world->sc_len, val);
		break;
	case BSDE_WANTED_VELOCITY:
		V3_SCALE(self->u_wantvel, 1.0/self->world->sc_len, val);
		break;
	case BSDE_WANTED_ROTATION:
		V4_COPY(self->u_wantorg, val);
		break;

	case BSDE_MMINS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SCALE(self->mmins, 1.0/self->world->sc_len, val);
		break;
	case BSDE_MMAXS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SCALE(self->mmaxs, 1.0/self->world->sc_len, val);
		break;
	case BSDE_RMINS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SUB(self->mmins, self->org, tv);
		V3_SCALE(tv, 1.0/self->world->sc_len, val);
		break;
	case BSDE_RMAXS:
		BSDE_Predict(self->world, self, 0.0001);
		V3_SUB(self->mmaxs, self->org, tv);
		V3_SCALE(tv, 1.0/self->world->sc_len, val);
		break;

	default:
		break;
	}
}

void BSDE_SetAttrDV(BSDE_Solid *self, int var, double *val, int n)
{
	bsde_real tv[9];
	int i;

	if(!self)return;

	switch(var)
	{
	case BSDE_ORIGIN:
		V3_SCALE(val, self->world->sc_len, self->org);
		break;
	case BSDE_VELOCITY:
		V3_SCALE(val, self->world->sc_len, self->vel);
		break;
	case BSDE_TORQUE:
		V3_COPY(val, self->tvel);
		break;
	case BSDE_ANGULAR_VELOCITY:
		BSDE_WorldInertiaTensor(self);
		V3_COPY(val, self->avel);
		BSDE_MatN_MatMult(self->wtmat, 3, 3, self->avel, 3, 1,
			self->tvel);
		break;
	case BSDE_ROTATION:
		V4_COPY(val, self->rot);
		break;
	case BSDE_ROTATION_MATRIX:
		for(i=0; i<9; i++)tv[i]=val[i];
		BSDE_Quat_From3Matrix(tv, self->rot);
		break;

	case BSDE_MINS:
		V3_SCALE(val, self->world->sc_len, self->mins);
		break;
	case BSDE_MAXS:
		V3_SCALE(val, self->world->sc_len, self->maxs);
		break;
	case BSDE_MASS_OFFSET:
		V3_SCALE(val, self->world->sc_len, self->mass_offs);
		break;
	case BSDE_INERTIA_TENSOR:
//		BSDE_Mat3_Copy(val, self->mass_imat);

		for(i=0; i<9; i++)
			self->mass_imat[i]=val[i];

		BSDE_MatN_Inverse(self->mass_imat, self->mass_tmat, 3);
		break;

	case BSDE_DIRECTION:
		V3_COPY(val, self->dir);
		self->dir[3]=val[3]*self->world->sc_len;
		break;

	case BSDE_CONTACT_VELOCITY:
		V3_SCALE(val, self->world->sc_len, self->u_wvel);
		break;
	case BSDE_ACCELERATION:
		V3_SCALE(val, self->world->sc_len, self->u_acc);
		break;
	case BSDE_LOCAL_THRUST:
		V3_SCALE(val, self->world->sc_len, self->u_racc);
		break;

	case BSDE_WANTED_ORIGIN:
		V3_SCALE(val, self->world->sc_len, self->u_wantorg);
		break;
	case BSDE_WANTED_VELOCITY:
		V3_SCALE(val, self->world->sc_len, self->u_wantvel);
		break;
	case BSDE_WANTED_ROTATION:
		V4_COPY(val, self->u_wantrot);
		break;

	default:
		break;
	}
	BSDE_Awaken(self);
}

void BSDE_GetAttrIV(BSDE_Solid *self, int var, int *val, int n)
{
	int i;

	if(!self)return;

	switch(var)
	{
	case BSDE_FACE_VERTICES:
		for(i=0; i<n; i++)val[i]=self->faces[i];
		break;
	case BSDE_FACE_COUNTS:
		for(i=0; i<n; i++)val[i]=self->facevecs[i];
		break;
	default:
		break;
	}
}

void BSDE_SetAttrIV(BSDE_Solid *self, int var, int *val, int num)
{
}

void BSDE_SetAttribPointer(BSDE_Solid *self, int var,
	int size, int type, int stride, void *val, int num)
{
	int i;

	if(!self)return;

	switch(var)
	{
	case BSDE_VOXEL_TYPE:
		self->vox_type_size=size;
		self->vox_type_type=type;
		self->vox_type_step=stride;
		self->vox_type_data=val;
		self->vox_type_num=num;
		break;
	case BSDE_VOXEL_META:
		self->vox_meta_size=size;
		self->vox_meta_type=type;
		self->vox_meta_step=stride;
		self->vox_meta_data=val;
		self->vox_meta_num=num;
		break;
	default:
		break;
	}
}

int BSDE_PointCollide(BSDE_Solid *self, bsde_real *point)
{
	bsde_real d;
	int i, j;

	i=0;
	switch(self->solidtype)
	{
	case BSDE_SOLID_AABB:
		i=1;
		for(j=0; j<3; j++)
		{
			d=point[j]-self->org[j];
			if((d<self->mins[j]) || (d>self->maxs[j]))i=0;
		}
		break;
	case BSDE_SOLID_OBB:
		i=BSDE_SolidOBB_ObbPointCollide(
			self->mins, self->maxs, self->xform, point);
		break;
	case BSDE_SOLID_SPHERE:
		i=1;
		d=V3_DIST(self->org, point);
		if(d>self->radius)i=0;
		break;
	case BSDE_SOLID_CYLINDER:
		i=BSDE_SolidCylinder_PointCollide(
			self->height, self->radius, 0, self->xform,
			point);
		break;
	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidCylinder_PointCollide(
			self->height, self->radius, 1, self->xform,
			point);
		break;
	case BSDE_SOLID_PLANE:
		i=BSDE_SolidPlane_PointCollide(
			self->dir, self->xform, point);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidOBB_ObbPointCollide(
			self->mins, self->maxs, self->xform, point);
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_PointCollide(self, self->xform, point);
		break;
	default:
		break;
	}
	return(i);
}

int BSDE_LineCollide(BSDE_Solid *self, bsde_real *isp, bsde_real *iep)
{
	bsde_real sp[4], ep[4];
	bsde_real d;
	int i, j;

	i=0;
	switch(self->solidtype)
	{
	case BSDE_SOLID_AABB:
		V3_SUB(isp, self->org, sp);
		V3_SUB(iep, self->org, ep);
		i=BSDE_SolidAABB_BoxLineCollideP(
			self->mins, self->maxs, sp, ep);
		break;
	case BSDE_SOLID_OBB:
		BSDE_Plane_TransformPointLocal(isp, self->xform, sp);
		BSDE_Plane_TransformPointLocal(iep, self->xform, ep);
		i=BSDE_SolidAABB_BoxLineCollideP(
			self->mins, self->maxs, sp, ep);
		break;
	case BSDE_SOLID_SPHERE:
		d=BSDE_Vec3_DistLineSeg(self->org, isp, iep);
		i=(d>self->radius)?0:1;
		break;
	case BSDE_SOLID_CYLINDER:
		i=BSDE_SolidCylinder_LineCollide(
			self->height, self->radius, 0, self->xform,
			isp, iep);
		break;
	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidCylinder_LineCollide(
			self->height, self->radius, 1, self->xform,
			isp, iep);
		break;
	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformPointLocal(isp, self->xform, sp);
		BSDE_Plane_TransformPointLocal(iep, self->xform, ep);
		if(V3_NDOT(sp, self->dir)<0)i=1;
		if(V3_NDOT(ep, self->dir)<0)i=1;
		break;
	case BSDE_SOLID_TRIMESH:
//		i=BSDE_SolidOBB_ObbPointCollide(
//			self->mins, self->maxs, self->xform, point);
		BSDE_Plane_TransformPointLocal(isp, self->xform, sp);
		BSDE_Plane_TransformPointLocal(iep, self->xform, ep);
		i=BSDE_SolidAABB_BoxLineCollideP(
			self->mins, self->maxs, sp, ep);
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_LineCollide(self, self->xform, isp, iep);
		break;
	case BSDE_SOLID_BSP:
		i=BSDE_SolidBSP_LineCollide(self, self->xform, isp, iep);
		break;
	default:
		break;
	}
	return(i);
}

void BSDE_PlaneExtents(BSDE_Solid *self, bsde_real *normal,
	bsde_real *min, bsde_real *max)
{
	bsde_real mins[3], maxs[3];
	bsde_real ext[2];
	bsde_real d, r;

	switch(self->solidtype)
	{
	case BSDE_SOLID_AABB:
		V3_ADD(self->org, self->mins, mins);
		V3_ADD(self->org, self->maxs, maxs);
		BSDE_SolidAABB_BoxPlaneExtents(mins, maxs, normal, min, max);
		break;
	case BSDE_SOLID_OBB:
		BSDE_SolidOBB_ObbPlaneExtents(self->mins, self->maxs,
			self->xform, normal, ext);
		*min=ext[0];
		*max=ext[1];
		break;
	case BSDE_SOLID_SPHERE:
		d=V3_DOT(self->org, normal);
		*min=d-self->radius;
		*max=d+self->radius;
		break;
	case BSDE_SOLID_CYLINDER:
		r=BSDE_SolidCylinder_AxisRadius(
			self->height, self->radius, 0, self->xform, normal);
		d=V3_DOT(self->org, normal);
		*min=d-r;
		*max=d+r;
		break;
	case BSDE_SOLID_CCYLINDER:
		r=BSDE_SolidCylinder_AxisRadius(
			self->height, self->radius, 1, self->xform, normal);
		d=V3_DOT(self->org, normal);
		*min=d-r;
		*max=d+r;
		break;
	case BSDE_SOLID_PLANE:
		*min=-999999999;
		*max=999999999;
		break;

	case BSDE_SOLID_TRIMESH:
		BSDE_SolidBRep_ProjectLine(self->vecs, self->faces,
			self->n_vecs, self->n_faces, self->xform,
			normal, min, max);
		break;
	case BSDE_SOLID_HULL:
		BSDE_SolidHull_ProjectLine(self, self->xform,
			normal, min, max);
		break;
	case BSDE_SOLID_BSP:	//just be cheap...
		V3_ADD(self->org, self->mins, mins);
		V3_ADD(self->org, self->maxs, maxs);
		BSDE_SolidAABB_BoxPlaneExtents(mins, maxs, normal, min, max);
		break;

	default:
		break;
	}
}

void BSDE_PlaneExtentsPos(BSDE_Solid *self, bsde_real *pos, bsde_real *normal,
	bsde_real *min, bsde_real *max)
{
	bsde_real mins[3], maxs[3];
	bsde_real ext[2];
	bsde_real d, r;

	switch(self->solidtype)
	{
	case BSDE_SOLID_AABB:
		V3_ADD(pos+3*4, self->mins, mins);
		V3_ADD(pos+3*4, self->maxs, maxs);
		BSDE_SolidAABB_BoxPlaneExtents(mins, maxs, normal, min, max);
		break;
	case BSDE_SOLID_OBB:
		BSDE_SolidOBB_ObbPlaneExtents(self->mins, self->maxs,
			pos, normal, ext);
		*min=ext[0];
		*max=ext[1];
		break;
	case BSDE_SOLID_SPHERE:
		d=V3_DOT(pos+3*4, normal);
		*min=d-self->radius;
		*max=d+self->radius;
		break;
	case BSDE_SOLID_CYLINDER:
//		r=BSDE_SolidCylinder_AxisRadius(
//			self->height, self->radius, 0, pos, normal);
		r=BSDE_SolidCylinder_AxisRadius(
			self->height, self->radius, 1, pos, normal);
		d=V3_DOT(pos+3*4, normal);
		*min=d-r;
		*max=d+r;
		break;
	case BSDE_SOLID_CCYLINDER:
		r=BSDE_SolidCylinder_AxisRadius(
			self->height, self->radius, 1, pos, normal);
		d=V3_DOT(pos+3*4, normal);
		*min=d-r;
		*max=d+r;
		break;
	case BSDE_SOLID_PLANE:
		*min=-999999999;
		*max=999999999;
		break;

	case BSDE_SOLID_TRIMESH:
		BSDE_SolidBRep_ProjectLine(self->vecs, self->faces,
			self->n_vecs, self->n_faces, pos,
			normal, min, max);
		break;
	case BSDE_SOLID_HULL:
		BSDE_SolidHull_ProjectLine(self, pos, normal, min, max);
		break;
	case BSDE_SOLID_BSP:	//just be cheap...
		V3_ADD(pos+3*4, self->mins, mins);
		V3_ADD(pos+3*4, self->maxs, maxs);
		BSDE_SolidAABB_BoxPlaneExtents(mins, maxs, normal, min, max);
		break;

	default:
		break;
	}
}

void BSDE_PlaneMoveExtents(BSDE_Solid *self, bsde_real *normal,
	bsde_real *min, bsde_real *max)
{
	bsde_real m, n, o, p;
	BSDE_PlaneExtentsPos(self, self->xform, normal, &m, &n);
	BSDE_PlaneExtentsPos(self, self->e_xform, normal, &o, &p);

	*min=(m<o)?m:o;
	*max=(n>p)?n:p;
}

void BSDE_PlaneFastMoveExtents(BSDE_Solid *self, bsde_real *normal,
	bsde_real *min, bsde_real *max)
{
	BSDE_SolidAABB_BoxPlaneExtents(self->mmins, self->mmaxs,
		normal, min, max);
}

int BSDE_BoxCollide(BSDE_Solid *self, bsde_real *mins, bsde_real *maxs)
{
	bsde_real lmin[3], lmax[3], im[16];
	int i;

	if(self->solidtype==BSDE_SOLID_BSP)
	{
		BSDE_Mat4_Identity(im);
		i=BSDE_SolidBSP2_BoxCollide(self, self->xform, mins, maxs, im);
		return(i);
	}

	BSDE_Box(self, lmin, lmax);
	i=BSDE_SolidAABB_BoxCollideP(lmin, lmax, mins, maxs);

	return(i);
}



int BSDE_HullCollide(BSDE_Solid *self, bsde_real *nv, int nnv)
{
	static bsde_real vecbuf[16*16*3];
	bsde_real mins[4], maxs[4], im[16];

	bsde_real m, n;
	int i, nvec;

	if(self->solidtype==BSDE_SOLID_BSP)
	{
		BSDE_SolidHull_MakeHullGeom(nv, nnv,
			vecbuf, NULL, NULL, &nvec, NULL, NULL);
		BSDE_SolidHull_FitBoxPoints(vecbuf, nvec, mins, maxs, im);
		i=BSDE_SolidBSP2_BoxCollide(self, self->xform, mins, maxs, im);
		return(i);
	}

	for(i=0; i<nnv; i++)
	{
		BSDE_PlaneExtents(self, nv+i*4, &m, &n);
		if(m>nv[i*4+3])return(0);
	}

	return(1);
}

