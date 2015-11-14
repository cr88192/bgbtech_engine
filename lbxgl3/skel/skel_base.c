#include <lbxgl2.h>
#include <math.h>

LBXGL_API int Skel_LoadBones(LBXGL_Skel2Model *mdl, char *name)
{
	static LBXGL_Skel2Solid *bone_solid[512];
	static char *bone_name[512];
	static float bone_org[512*3];
	static float bone_rot[512*9];
	static float bone_rorg[512*3];
	static int bone_up[512];
	int num_bones, bone_solids;

	char buf[256];
	char **a;
	VFILE *fd;
	int i, j;
	float *v;

//	fd=vffopen(name, "rt");
	fd=btcl_vffopen(name, "rt");
	if(!fd)
	{
		printf("can't open bones %s\n", name);
		return(-1);
	}

	for(i=0; i<512; i++)bone_name[i]=NULL;

	num_bones=0; bone_solids=0;
	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		if(!strcmp(a[0], "bbox"))
		{
			bone_solid[bone_solids]=
				gctalloc("lbxgl_skel_base", sizeof(LBXGL_Skel2Solid));

			bone_solid[bone_solids]->type=BONE_SOLID_BBOX;
			bone_solid[bone_solids]->bone=atoi(a[1]);

			v=bone_rorg+bone_solid[bone_solids]->bone*3;
			bone_solid[bone_solids]->m[0]=atof(a[3])-v[0];
			bone_solid[bone_solids]->m[1]=atof(a[4])-v[1];
			bone_solid[bone_solids]->m[2]=atof(a[5])-v[2];
			bone_solid[bone_solids]->n[0]=atof(a[8])-v[0];
			bone_solid[bone_solids]->n[1]=atof(a[9])-v[1];
			bone_solid[bone_solids]->n[2]=atof(a[10])-v[2];

			bone_solid[bone_solids]->r0=atof(a[12]);
			bone_solid[bone_solids]->w=atof(a[13]);

			bone_solids++;
			if(bone_solids>=512)
				{ *(int *)-1=-1; }
		}

		if(!strcmp(a[0], "capsule"))
		{
			bone_solid[bone_solids]=
				gctalloc("lbxgl_skel_base", sizeof(LBXGL_Skel2Solid));

			bone_solid[bone_solids]->type=BONE_SOLID_CAPSULE;
			bone_solid[bone_solids]->bone=atoi(a[1]);

			v=bone_rorg+bone_solid[bone_solids]->bone*3;
			bone_solid[bone_solids]->m[0]=atof(a[3])-v[0];
			bone_solid[bone_solids]->m[1]=atof(a[4])-v[1];
			bone_solid[bone_solids]->m[2]=atof(a[5])-v[2];
			bone_solid[bone_solids]->n[0]=atof(a[8])-v[0];
			bone_solid[bone_solids]->n[1]=atof(a[9])-v[1];
			bone_solid[bone_solids]->n[2]=atof(a[10])-v[2];

			bone_solid[bone_solids]->r0=atof(a[12]);
			bone_solid[bone_solids]->r1=atof(a[13]);
			bone_solid[bone_solids]->w=atof(a[14]);

			bone_solids++;
			if(bone_solids>=512)
				{ *(int *)-1=-1; }
		}

		if(!strcmp(a[0], "sphere"))
		{
			bone_solid[bone_solids]=
				gctalloc("lbxgl_skel_base", sizeof(LBXGL_Skel2Solid));

			bone_solid[bone_solids]->type=BONE_SOLID_SPHERE;
			bone_solid[bone_solids]->bone=atoi(a[1]);

			v=bone_rorg+bone_solid[bone_solids]->bone*3;
			bone_solid[bone_solids]->m[0]=atof(a[3])-v[0];
			bone_solid[bone_solids]->m[1]=atof(a[4])-v[1];
			bone_solid[bone_solids]->m[2]=atof(a[5])-v[2];

			bone_solid[bone_solids]->r0=atof(a[7]);
			bone_solid[bone_solids]->r1=atof(a[8]);
			bone_solid[bone_solids]->w=atof(a[9]);

			bone_solids++;
			if(bone_solids>=512)
				{ *(int *)-1=-1; }
		}

		if((*a[0]<'0') || (*a[0]>'9'))
			continue;

		i=atoi(a[0]);
		if(i>=num_bones)num_bones=i+1;

		if(num_bones>=512)
			{ *(int *)-1=-1; }

		j=atoi(a[1]);
		bone_up[i]=j;
		bone_name[i]=dystrdup(a[2]);

		bone_rorg[i*3+0]=atof(a[4]);
		bone_rorg[i*3+1]=atof(a[5]);
		bone_rorg[i*3+2]=atof(a[6]);

		if(j>=0)
		{
			bone_org[i*3+0]=bone_rorg[i*3+0]-bone_rorg[j*3+0];
			bone_org[i*3+1]=bone_rorg[i*3+1]-bone_rorg[j*3+1];
			bone_org[i*3+2]=bone_rorg[i*3+2]-bone_rorg[j*3+2];
		}else
		{
			bone_org[i*3+0]=bone_rorg[i*3+0];
			bone_org[i*3+1]=bone_rorg[i*3+1];
			bone_org[i*3+2]=bone_rorg[i*3+2];
		}

		for(j=0; j<9; j++)bone_rot[i*9+j]=0;
		for(j=0; j<3; j++)bone_rot[i*9+j*3+j]=1;
	}
	vfclose(fd);

	printf("skel loaded, %d bones, %d solids\n", num_bones, bone_solids);

	mdl->num_bones=num_bones;
	if(!mdl->max_bones)
	{
		//model lacks space for skeleton, so set it up
		//this should be the case for render-only models
		//or other models that may not add any new bones

		mdl->max_bones=mdl->num_bones;
		mdl->bone_name=gctalloc("lbxgl_skel_base",
			num_bones*sizeof(char *));
		mdl->bone_up=gctalloc("lbxgl_skel_base",
			num_bones*sizeof(int));
		mdl->bone_org=gctalloc("lbxgl_skel_base",
			num_bones*3*sizeof(float));
		mdl->bone_rot=gctalloc("lbxgl_skel_base",
			num_bones*9*sizeof(float));

		//hitbox
		mdl->bone_mins=gctalloc("lbxgl_skel_base",
			num_bones*3*sizeof(float));
		mdl->bone_maxs=gctalloc("lbxgl_skel_base",
			num_bones*3*sizeof(float));
	}

	for(i=0; i<num_bones; i++)
	{
		mdl->bone_name[i]=bone_name[i];
		mdl->bone_up[i]=bone_up[i];
		V3F_COPY(bone_org+i*3, mdl->bone_org+i*3);

		V3F_COPY(bone_rot+i*9+0, mdl->bone_rot+i*9+0);
		V3F_COPY(bone_rot+i*9+3, mdl->bone_rot+i*9+3);
		V3F_COPY(bone_rot+i*9+6, mdl->bone_rot+i*9+6);
	}

	mdl->num_solids=bone_solids;
	mdl->solid=gctalloc("lbxgl_skel_base",
		bone_solids*sizeof(LBXGL_Skel2Solid *));
	for(i=0; i<bone_solids; i++)mdl->solid[i]=bone_solid[i];

	return(0);
}

#if 1
LBXGL_API int Skel_LoadAnimRaw_I(LBXGL_Skel2Model *mdl,
	char *name, LBXGL_Skel2Anim *tmp)
{
	char buf[256];
	int n_frames, frame_cur;
	char **a;
	VFILE *fd;
	int i, j, k, l;

	n_frames=0;
	frame_cur=0;

//	fd=vffopen(name, "rt");
	fd=btcl_vffopen(name, "rt");
	if(!fd)
	{
		printf("can't open anim %s\n", name);
		return(-1);
	}

	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		if(!strcmp(a[0], "frame"))
		{
			frame_cur=atoi(a[1]);
			if((frame_cur+1)>n_frames)n_frames=frame_cur+1;
		}

		for(i=0; i<mdl->num_bones; i++)
			if(mdl->bone_name[i] &&
				!strcmp(mdl->bone_name[i], a[0]))
					break;
		if(i>=mdl->num_bones)continue;

		j=frame_cur*64+i;
		for(k=0; k<3; k++)tmp->frame_org[j*3+k]=atof(a[2+k]);
		for(k=0; k<9; k++)tmp->frame_rot[j*9+k]=atof(a[7+k]);
		tmp->frame_mask[j>>3]|=1<<(j&7);
	}
	vfclose(fd);

	if(n_frames>tmp->num_frames)
		tmp->num_frames=n_frames;

	return(0);
}

LBXGL_API LBXGL_Skel2Anim *Skel_LoadAnimRawMulti(
	LBXGL_Skel2Model *mdl, char **names)
{
	static float frame_org[512*128*3];
	static float frame_rot[512*128*9];
	static byte frame_mask[512*128/8];
	int n_frames, frame_cur;

	LBXGL_Skel2Anim *tmp;

	float pos[16];
	int i, j, k, l;


	memset(frame_mask, 0, 512*128/8);
	n_frames=0;
	frame_cur=0;

	tmp=gctalloc("lbxgl_skel_base", sizeof(LBXGL_Skel2Anim));

	tmp->frame_org=frame_org;
	tmp->frame_rot=frame_rot;
	tmp->frame_mask=frame_mask;
	tmp->num_frames=0;

	for(i=0; names[i]; i++)
		Skel_LoadAnimRaw_I(mdl, names[i], tmp);

	n_frames=tmp->num_frames;

//	tmp->num_frames=n_frames;
	if(n_frames<16)n_frames=16;

	tmp->frame_org=gctalloc("lbxgl_skel_base",
		n_frames*mdl->max_bones*3*sizeof(float));
	tmp->frame_rot=gctalloc("lbxgl_skel_base",
		n_frames*mdl->max_bones*9*sizeof(float));
	tmp->frame_mask=gctalloc("lbxgl_skel_base",
		(n_frames*mdl->max_bones+7)/8);

	for(i=0; i<tmp->num_frames; i++)
		for(j=0; j<mdl->num_bones; j++)
	{
		k=i*64+j;
		l=i*mdl->max_bones+j;

		V3F_COPY(frame_org+k*3, tmp->frame_org+l*3);

		V3F_COPY(frame_rot+k*9+0, tmp->frame_rot+l*9+0);
		V3F_COPY(frame_rot+k*9+3, tmp->frame_rot+l*9+3);
		V3F_COPY(frame_rot+k*9+6, tmp->frame_rot+l*9+6);

		if(frame_mask[k>>3]&(1<<(k&7)))
			tmp->frame_mask[l>>3]|=1<<(l&7);
	}

	tmp->frame_time=1.0/15.0;

	return(tmp);
}

LBXGL_API LBXGL_Skel2Anim *Skel_LoadAnimRaw(LBXGL_Skel2Model *mdl, char *name)
{
	char *ta[2];
	
	ta[0]=name; ta[1]=NULL;
	return(Skel_LoadAnimRawMulti(mdl, ta));
}

#endif


LBXGL_API float Skel_WeightVertexSolid(LBXGL_Skel2State *ctx, float *pt, int num)
{
	float tv[3], tv1[3];
	float *v0, *v1;
	float f, g, d;
	int i, j, k;

	i=ctx->mdl->solid[num]->bone;
	V3F_SUB(pt, ctx->bone_rorg+3*i, tv1);
	tv[0]=V3F_DOT(tv1, ctx->bone_rmat+9*i+0);
	tv[1]=V3F_DOT(tv1, ctx->bone_rmat+9*i+3);
	tv[2]=V3F_DOT(tv1, ctx->bone_rmat+9*i+6);	

	v0=ctx->mdl->solid[num]->m;
	v1=ctx->mdl->solid[num]->n;
	switch(ctx->mdl->solid[num]->type)
	{
	case BONE_SOLID_BBOX:
		for(i=0; i<3; i++)
		{
			if(tv[i]<v0[i])
			{
				d=v0[i]-tv[i];
				if(d>=ctx->mdl->solid[num]->r0)
					return(0);
				f=1.0-(d/ctx->mdl->solid[num]->r0);
				return(f*ctx->mdl->solid[num]->w);
			}
			if(tv[i]>v1[i])
			{
				d=tv[i]-v1[i];
				if(d>=ctx->mdl->solid[num]->r0)
					return(0);
				f=1.0-(d/ctx->mdl->solid[num]->r0);
				return(f*ctx->mdl->solid[num]->w);
			}
		}
		return(ctx->mdl->solid[num]->w);
		break;

	case BONE_SOLID_SPHERE:
		d=V3F_DIST(tv, v0);
		if(d<=ctx->mdl->solid[num]->r0)
			return(ctx->mdl->solid[num]->w);
		if(d>=ctx->mdl->solid[num]->r1)
			return(0);
		d=d-ctx->mdl->solid[num]->r0;
		f=ctx->mdl->solid[num]->r1-ctx->mdl->solid[num]->r0;
		g=1.0-(d/f);
		return(f*ctx->mdl->solid[num]->w);
		break;

	case BONE_SOLID_CAPSULE:
		d=Vec3F_DistLineSeg(tv, v0, v1);
		if(d<=ctx->mdl->solid[num]->r0)
			return(ctx->mdl->solid[num]->w);
		if(d>=ctx->mdl->solid[num]->r1)
			return(0);
		d=d-ctx->mdl->solid[num]->r0;
		f=ctx->mdl->solid[num]->r1-ctx->mdl->solid[num]->r0;
		g=1.0-(d/f);
		return(f*ctx->mdl->solid[num]->w);
		break;

	default:
		break;
	}

	return(0);
}

//LBXGL_API int Skel_WeightVertex(LBXGL_Skel2State *ctx,
//	float *pt, float *txyz, int *tidx, float *tval, int n)
LBXGL_API int Skel_WeightVertexNV(LBXGL_Skel2State *ctx,
	float *pt, float *nv,
	float *txyz, float *tnv,
	int *tidx, float *tval, int n)
{
	float tv[3], tv1[3], bv[3], bnv[3];
	LBXGL_Skel2Model *mdl;
	int i, j, k, bi, n0;
	float d, bd;

	n0=n;
	mdl=ctx->mdl;

	for(i=0; i<ctx->mdl->num_solids; i++)
	{
		d=Skel_WeightVertexSolid(ctx, pt, i);

		if(d>0)
		{
			j=ctx->mdl->solid[i]->bone;

			//add to any existing weights
			for(k=n0; k<n; k++)
				if(tidx[k]==j)break;
			if(k<n)
			{
				tval[k]+=d;
				continue;
			}

			//add new weight
			V3F_SUB(pt, ctx->bone_rorg+3*j, tv1);
			tv[0]=V3F_DOT(tv1, ctx->bone_rmat+9*j+0);
			tv[1]=V3F_DOT(tv1, ctx->bone_rmat+9*j+3);
			tv[2]=V3F_DOT(tv1, ctx->bone_rmat+9*j+6);

			tv1[0]=V3F_DOT(nv, ctx->bone_rmat+9*j+0);
			tv1[1]=V3F_DOT(nv, ctx->bone_rmat+9*j+3);
			tv1[2]=V3F_DOT(nv, ctx->bone_rmat+9*j+6);

			V3F_COPY(tv, txyz+n*3);
			V3F_COPY(tv1, tnv+n*3);
			tidx[n]=j;
			tval[n]=d;
			n++;
		}
	}

	if(n==n0)
	{
		V3F_ZERO(bv);
		bi=-1;
		bd=-999999;

		//bones to parent bones
		for(j=0; j<mdl->num_bones; j++)
		{
			k=mdl->bone_up[j];
			if(k<0)continue;

			d=Vec3F_DistLineSeg(pt,
				ctx->bone_rorg+3*j,
				ctx->bone_rorg+3*k);
			if(d>1)d=1.0/d; else d=1;
			if(d<0.0001)continue;

			V3F_SUB(pt, ctx->bone_rorg+3*k, tv1);
			tv[0]=V3F_DOT(tv1, ctx->bone_rmat+9*k+0);
			tv[1]=V3F_DOT(tv1, ctx->bone_rmat+9*k+3);
			tv[2]=V3F_DOT(tv1, ctx->bone_rmat+9*k+6);

			tv1[0]=V3F_DOT(nv, ctx->bone_rmat+9*k+0);
			tv1[1]=V3F_DOT(nv, ctx->bone_rmat+9*k+3);
			tv1[2]=V3F_DOT(nv, ctx->bone_rmat+9*k+6);

			if(d>bd)
			{
				V3F_COPY(tv, bv);
				V3F_COPY(tv1, bnv);
				bi=k; bd=d;
			}
		}

		if(bi<0)
		{
			//worst case, attack to skeleton root
			V3F_COPY(pt, txyz+n*3);
			V3F_COPY(nv, tnv+n*3);
			tidx[n]=0;
			tval[n]=1;
			n++;
		}else
		{
			V3F_COPY(bv, txyz+n*3);
			V3F_COPY(bnv, tnv+n*3);
			tidx[n]=bi;
			tval[n]=bd;
			n++;
		}
	}

	return(n);
}

LBXGL_API void Skel_TempCalcMeshVNorm(
	LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *mesh,
	float *txyz, float *tnorm, float *tvnorm)
{
	LBXGL_Skel2MeshFace *cur, *cur2;
	float im[16];
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *v0, *v1, *v2, *imv;
	int i, j, k, l, l1;
	float f, g;

	if(!mesh)return;
	if(!mesh->vidx)return;

	//calculate face normals
	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];
		v0=txyz+cur->vecs[0]*3;
		v1=txyz+cur->vecs[1]*3;
		v2=txyz+cur->vecs[2]*3;
		V3F_SUB(v1, v0, tv0);
		V3F_SUB(v2, v1, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_FRNORMALIZE(tv2, tnorm+i*3);
	}

	//calculate vertex normals
	for(i=0; i<mesh->n_vecs; i++)
	{
		V3F_ZERO(tv);
			
		for(j=0; j<mesh->vfnum[i]; j++)
		{
			k=mesh->vfids[mesh->vfidx[i]+j];
			v1=tnorm+k*4;
			V3F_ADD(tv, v1, tv);
		}

		V3F_FRNORMALIZE(tv, tvnorm+i*3);
	}
}

LBXGL_API void Skel_BindMesh(LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *mesh)
{
	static float txyz[65536*3];
	static float tvnv[65536*3];
	static int tidx[65536];
	static float tval[65536];

	static float tfnorm[65536*3];
	static float tvnorm[65536*3];

	LBXGL_Skel2MeshFace *cur;
	float tv[3], tv1[3], bv[3];
	int i, j, k, n, bi;
	float d, bd;

	if(!mesh->edge)
		Skel_BuildMeshEdges(mesh);

	if(mesh->n_vecs>=65536)
		{ *(int *)-1=-1; }

	Skel_TempCalcMeshVNorm(ctx, mesh,
		mesh->vecs, tfnorm, tvnorm);

	mesh->vidx=gctalloc("lbxgl_skel_base", mesh->n_vecs*sizeof(int));
	mesh->vnum=gctalloc("lbxgl_skel_base", mesh->n_vecs*sizeof(int));

	n=0;
	for(i=0; i<mesh->n_vecs; i++)
	{
		d=V3F_LEN(mesh->vecs+i*3);
		if(d>ctx->mdl->radius)
			ctx->mdl->radius=d;
	
		mesh->vidx[i]=n;

//		n=Skel_WeightVertex(ctx, mesh->vecs+i*3, txyz, tidx, tval, n);
		n=Skel_WeightVertexNV(ctx,
			mesh->vecs+i*3, tvnorm+i*3,
			txyz, tvnv,
			tidx, tval, n);

		mesh->vnum[i]=n-mesh->vidx[i];
	}

	mesh->wxyz=gctalloc("lbxgl_skel_base", 3*n*sizeof(float));
	mesh->wvnv=gctalloc("lbxgl_skel_base", 3*n*sizeof(float));
	mesh->widx=gctalloc("lbxgl_skel_base", n*sizeof(int));
	mesh->wval=gctalloc("lbxgl_skel_base", n*sizeof(float));

	for(i=0; i<n; i++)
	{
		V3F_COPY(txyz+i*3, mesh->wxyz+i*3);
		V3F_COPY(tvnv+i*3, mesh->wvnv+i*3);
		mesh->widx[i]=tidx[i];
		mesh->wval[i]=tval[i];
	}

	mesh->n_weights=n;
}

LBXGL_API void Skel_BindMeshGroup(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_BindMeshGroup(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_BindMesh(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_UnbindMesh(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh)
{
	mesh->n_weights=0;

	if(mesh->vidx)gcfree(mesh->vidx);
	if(mesh->vnum)gcfree(mesh->vnum);
	if(mesh->wxyz)gcfree(mesh->wxyz);
	if(mesh->wvnv)gcfree(mesh->wvnv);
	if(mesh->widx)gcfree(mesh->widx);
	if(mesh->wval)gcfree(mesh->wval);

	mesh->vidx=NULL;
	mesh->vnum=NULL;
	mesh->wxyz=NULL;
	mesh->wvnv=NULL;
	mesh->widx=NULL;
	mesh->wval=NULL;
}

LBXGL_API void Skel_UnbindMeshGroup(LBXGL_Skel2State *ctx, 
	LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_UnbindMeshGroup(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_UnbindMesh(ctx, mcur);
		mcur=mcur->next;
	}
}


LBXGL_API void Skel_CalcHitboxMesh(LBXGL_Skel2Model *mdl,
	LBXGL_Skel2Mesh *mesh,
	float *borg, float *brad, float *bwght)
{
	float tv0[3], tv1[3], tv2[3];
	float f, g;
	int i, bn;
	
	for(i=0; i<mesh->n_weights; i++)
	{
		bn=mesh->widx[i];
		V3F_COPY(mesh->wxyz+(i*3), tv0);
		f=mesh->wval[i];
		
		if(!brad)
		{
			V3F_ADDSCALE(borg+(bn*3), tv0, f, borg+(bn*3));
			bwght[bn]+=f;
		}
		
		if(brad)
		{
			//calculate bone origin
			g=bwght[bn]; g=(g>0)?(1.0/g):1;
			V3F_SCALE(borg+(bn*3), g, tv1);
			//calculate difference from origin
			V3F_SUB(tv0, tv1, tv2);

//			tv2[0]=fabs(tv2[0]);
//			tv2[1]=fabs(tv2[1]);
//			tv2[2]=fabs(tv2[2]);

			//calculate squared vector

			V3F_SCALE(tv2, f, tv2);

			tv2[0]=tv2[0]*tv2[0];
			tv2[1]=tv2[1]*tv2[1];
			tv2[2]=tv2[2]*tv2[2];

			V3F_ADD(brad+(bn*3), tv2, brad+(bn*3));

//			V3F_ADDSCALE(brad+(bn*3), tv2, f, brad+(bn*3));
		}
	}
}

LBXGL_API void Skel_CalcHitboxGroup(LBXGL_Skel2Model *mdl,
	LBXGL_Skel2MeshGroup *grp,
	float *borg, float *brad, float *bwght)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_CalcHitboxGroup(mdl, gcur, borg, brad, bwght);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_CalcHitboxMesh(mdl, mcur, borg, brad, bwght);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_BindCalculateHitbox(LBXGL_Skel2Model *mdl)
{
	float borg[256*3], brad[256*3], bwght[256];
	float tv0[3], tv1[3];
	float f, g;
	int i, j;
	
	for(i=0; i<mdl->num_bones; i++)
	{
		V3F_ZERO(borg+i*3);
		V3F_ZERO(brad+i*3);
		bwght[i]=0;
	}
	
	Skel_CalcHitboxGroup(mdl, mdl->mesh, borg, NULL, bwght);
	Skel_CalcHitboxGroup(mdl, mdl->mesh, borg, brad, bwght);

	for(i=0; i<mdl->num_bones; i++)
	{
		//calculate bone origin
		f=bwght[i];
		if(f>0)f=1.0/f;
		V3F_SCALE(borg+i*3, f, tv0);
//		V3F_SCALE(brad+i*3, f, tv1);

		//calculate bone radius
		g=bwght[i];
		if(g>1)g=1.414*(1.0/(g-1));
			else g=1;

		V3F_COPY(brad+i*3, tv1);
		tv1[0]=sqrt(tv1[0])*g;
		tv1[1]=sqrt(tv1[1])*g;
		tv1[2]=sqrt(tv1[2])*g;

//		f=sqrt(tv1[0]+tv1[1]+tv1[2]);
//		f=(f>0)?(1.0/f):1;
//		tv1[0]=tv1[0]*f*g;
//		tv1[1]=tv1[1]*f*g;
//		tv1[2]=tv1[2]*f*g;
		
		printf("Hitbox: %d %.3f "
				"Org=(%.3f %.3f %.3f) Rad=(%.3f %.3f %.3f)\n",
			i, bwght[i],
				tv0[0], tv0[1], tv0[2],
				tv1[0], tv1[1], tv1[2]);
		
//		V3F_ZERO(tv0);
//		V3F_SET(tv1, 16, 16, 16);
		
		V3F_SUB(tv0, tv1, mdl->bone_mins+(i*3));
		V3F_ADD(tv0, tv1, mdl->bone_maxs+(i*3));
	}
}


LBXGL_Skel2Mesh *Skel_CloneMesh(LBXGL_Skel2Mesh *mesh);
LBXGL_Skel2MeshGroup *Skel_CloneMeshGroup(LBXGL_Skel2MeshGroup *grp);

LBXGL_API int Skel_LoadMeshAC3D(LBXGL_Skel2Model *mdl, char *name)
{
	static float org[512*3];
	static float rot[512*9];
	LBXGL_Skel2State ctx;
	int i0, i1, i2;
	int i, j, k;

	for(i=0; i<mdl->num_bones; i++)
	{
		j=mdl->bone_up[i];

		if(j>=0)
		{
			V3F_ADD(mdl->bone_org+i*3, org+j*3, org+i*3);
			Mat3F_Mat3Mult(mdl->bone_rot+i*9, rot+j*9, rot+i*9);
		}else
		{
			V3F_COPY(mdl->bone_org+i*3, org+i*3);
			Mat3F_Identity(rot+i*9);
		}
	}

	ctx.mdl=mdl;
	ctx.bone_rorg=org;
	ctx.bone_rmat=rot;

	mdl->radius=16;

	mdl->mesh=AC3D_LoadModel(name);
	
	i0=0; i1=0; i2=0;
	AC3D_AssignVecsMeshGroup(mdl->mesh, &i0, &i1, &i2);

#if 0
	while(i2>4000)
	{
		//model has too many faces, so LOD it
		SkelLod_GroupMergeVecs(mdl->mesh);
		i0=0; i1=0; i2=0;
		AC3D_AssignVecsMeshGroup(mdl->mesh, &i0, &i1, &i2);
	}
#endif
	
	mdl->n_xyz=i0;
	mdl->n_st=i1;
	mdl->n_face=i2;

//	mdl->mesh=Skel_CloneMeshGroup(mdl->mesh);
//	SkelLod_GroupMergeVecs(mdl->mesh);

#if 1
#if 1
	mdl->mesh_lod[0]=Skel_CloneMeshGroup(mdl->mesh);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[0]);
	mdl->mesh_lod[1]=Skel_CloneMeshGroup(mdl->mesh_lod[0]);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[1]);
	mdl->mesh_lod[2]=Skel_CloneMeshGroup(mdl->mesh_lod[1]);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[2]);
	mdl->mesh_lod[3]=Skel_CloneMeshGroup(mdl->mesh_lod[2]);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[3]);
#endif

#if 0
	mdl->mesh_lod[4]=Skel_CloneMeshGroup(mdl->mesh_lod[3]);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[4]);
	mdl->mesh_lod[5]=Skel_CloneMeshGroup(mdl->mesh_lod[4]);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[5]);
	mdl->mesh_lod[6]=Skel_CloneMeshGroup(mdl->mesh_lod[5]);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[6]);
	mdl->mesh_lod[7]=Skel_CloneMeshGroup(mdl->mesh_lod[6]);
	SkelLod_GroupMergeVecs(mdl->mesh_lod[7]);
#endif
#endif

#if 0
	mdl->mesh_lod[0]=Skel_CloneMeshGroup(mdl->mesh);
	SkelLod_GroupMergeVecsFrac(mdl->mesh_lod[0], 33);
	mdl->mesh_lod[1]=Skel_CloneMeshGroup(mdl->mesh);
	SkelLod_GroupMergeVecsFrac(mdl->mesh_lod[1], 50);
	mdl->mesh_lod[2]=Skel_CloneMeshGroup(mdl->mesh);
	SkelLod_GroupMergeVecsFrac(mdl->mesh_lod[2], 75);
	mdl->mesh_lod[3]=Skel_CloneMeshGroup(mdl->mesh);
	SkelLod_GroupMergeVecsFrac(mdl->mesh_lod[3], 85);
#endif

	//assign indices to LOD-levels
	for(i=0; i<8; i++)
	{
		i0=0; i1=0; i2=0;
		AC3D_AssignVecsMeshGroup(mdl->mesh_lod[i], &i0, &i1, &i2);
		//however, there is no need to save results
		//as the arrays will be reused from the base mesh
	}

	Skel_BindMeshGroup(&ctx, mdl->mesh);
	Skel_BindCalculateHitbox(mdl);

#if 1
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[0]);
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[1]);
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[2]);
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[3]);
#if 0
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[4]);
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[5]);
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[6]);
	Skel_BindMeshGroup(&ctx, mdl->mesh_lod[7]);
#endif
#endif

#if 1
	SkelVBO_ModelRebuildLodVBO(mdl, -1);
	SkelVBO_ModelRebuildLodVBO(mdl, 0);
	SkelVBO_ModelRebuildLodVBO(mdl, 1);
	SkelVBO_ModelRebuildLodVBO(mdl, 2);
	SkelVBO_ModelRebuildLodVBO(mdl, 3);
#endif

	mdl->radius*=1.414;

	return(0);
}

LBXGL_API int Skel_LoadMesh(LBXGL_Skel2Model *mdl, char *name)
{
	char *s;

	s=vfgetext(name);
	if(!strcmp(s, "ac"))return(Skel_LoadMeshAC3D(mdl, name));
	if(!strcmp(s, "smd"))return(LBXGL_SkelSmd_LoadReference(mdl, name));
	return(-1);
}

LBXGL_API LBXGL_Skel2Anim *Skel_LoadAnim(LBXGL_Skel2Model *mdl, char *name)
{
	char *s;

	s=vfgetext(name);
	if(!strcmp(s, "anim"))return(Skel_LoadAnimRaw(mdl, name));
	if(!strcmp(s, "smd"))return(LBXGL_SkelSmd_LoadAnim(mdl, name));
	return(NULL);
}

LBXGL_API LBXGL_Skel2Anim *Skel_LoadAnimMulti(LBXGL_Skel2Model *mdl,
	char **names)
{
	char *s;

	s=vfgetext(names[0]);
	if(!strcmp(s, "anim"))return(Skel_LoadAnimRawMulti(mdl, names));
//	if(!strcmp(s, "smd"))return(LBXGL_SkelSmd_LoadAnim(mdl, name));
	return(NULL);
}

LBXGL_API LBXGL_Skel2Model *Skel_LoadModel(char *name)
{
	char tba[16][256];
	char *ta[64];
	char buf[256];
	float pos[16];
	LBXGL_Skel2Model *tmp;
	LBXGL_Skel2Anim *atmp;
	char **a;
	char *base, *s, *t;
	VFILE *fd;
	int i, j, nframe;

	strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='/') && (*s!='\\'))s--;
	*s=0;
	base=strdup(buf);

//	fd=vffopen(name, "rt");
	fd=btcl_vffopen(name, "rt");
	if(!fd)
	{
		printf("can't open model %s\n", name);
		return(NULL);
	}

//	tmp=gctalloc("lbxgl_skel_base", sizeof(LBXGL_Skel2Model));
	tmp=gctalloc("lbxgl_skel2model_t", sizeof(LBXGL_Skel2Model));

	tmp->mdlscale[0]=1;
	tmp->mdlscale[1]=1;
	tmp->mdlscale[2]=1;

	tmp->radius = 96;

	nframe=0;
	while(!vfeof(fd))
	{
		memset(buf, 0, 256);
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);

		if(!a[0])continue;
		if(!strncmp(a[0], "//", 2))continue;

		if(!strcmp(a[0], "bones"))
		{
			sprintf(buf, "%s/%s", base, a[1]);
			Skel_LoadBones(tmp, buf);
			continue;
		}

		if(!strcmp(a[0], "vmesh"))
		{
			sprintf(buf, "%s/%s", base, a[1]);
			Skel_LoadMesh(tmp, buf);

			printf("mesh %p\n", tmp->mesh);

			continue;
		}

		if(!strcmp(a[0], "anim"))
		{
			s=dystrdup(a[1]);
			t=dystrdup(a[3]);

			sprintf(buf, "%s/%s", base, a[2]);
			atmp=Skel_LoadAnim(tmp, buf);

			atmp->name=s;
			atmp->next_anim=t;

			atmp->frame_base=nframe;
			nframe+=atmp->num_frames;

			atmp->next=tmp->anim;
			tmp->anim=atmp;

			printf("anim %s: %d, %d frames\n",
				atmp->name, atmp->frame_base, atmp->num_frames);

			continue;
		}

		if(!strcmp(a[0], "anim_m"))
		{
			s=dystrdup(a[1]);
			t=dystrdup(a[3]);

			for(i=0; a[i+2]; i++)
			{
				sprintf(tba[i], "%s/%s", base, a[2]);
				ta[i]=tba[i];
			}
			ta[i]=0;
			
			atmp=Skel_LoadAnimMulti(tmp, ta);

			atmp->name=s;
			atmp->next_anim=t;

			atmp->frame_base=nframe;
			nframe+=atmp->num_frames;

			atmp->next=tmp->anim;
			tmp->anim=atmp;

			printf("anim_m %s: %d, %d frames\n",
				atmp->name, atmp->frame_base, atmp->num_frames);

			continue;
		}

		if(!strcmp(a[0], "origin"))
		{
			tmp->mdlorg[0]=atof(a[1]);
			tmp->mdlorg[1]=atof(a[2]);
			tmp->mdlorg[2]=atof(a[3]);
		}

		if(!strcmp(a[0], "xlat"))
		{
			tmp->mdlxlat[0]=atof(a[1]);
			tmp->mdlxlat[1]=atof(a[2]);
			tmp->mdlxlat[2]=atof(a[3]);
		}

		if(!strcmp(a[0], "scale"))
		{
			tmp->mdlscale[0]=atof(a[1]);
			tmp->mdlscale[1]=atof(a[2]);
			tmp->mdlscale[2]=atof(a[3]);
		}

		if(!strcmp(a[0], "pose"))
		{
			Skel_SetPoseAnimFrame(tmp, atoi(a[1]),
				a[2], atoi(a[3]));
		}
	}
	vfclose(fd);

	return(tmp);
}

LBXGL_API LBXGL_Skel2State *Skel_NewInstance(LBXGL_Skel2Model *mdl)
{
	LBXGL_Skel2State *tmp;

	tmp=gctalloc("lbxgl_skel2state_t", sizeof(LBXGL_Skel2State));
	tmp->mdl=mdl;

	tmp->bone_rorg=gctalloc("lbxgl_skel_base", mdl->max_bones*3*sizeof(float));
	tmp->bone_rmat=gctalloc("lbxgl_skel_base", mdl->max_bones*9*sizeof(float));
	tmp->bone_flag=gctalloc("lbxgl_skel_base", mdl->max_bones*sizeof(int));

	tmp->cur_xyz=gctalloc("lbxgl_skel_base", mdl->n_xyz*3*sizeof(float));
	tmp->cur_norm=gctalloc("lbxgl_skel_base", mdl->n_face*4*sizeof(float));
	tmp->cur_vnorm=gctalloc("lbxgl_skel_base", mdl->n_face*3*4*sizeof(float));

	return(tmp);
}

LBXGL_API LBXGL_Skel2State *Skel_NewTempInstance(LBXGL_Skel2Model *mdl)
{
	LBXGL_Skel2State *tmp;

	tmp=gcralloc(sizeof(LBXGL_Skel2State));
	memset(tmp, 0, sizeof(LBXGL_Skel2State));
	tmp->mdl=mdl;

	tmp->bone_rorg=gcralloc(mdl->max_bones*3*sizeof(float));
	tmp->bone_rmat=gcralloc(mdl->max_bones*9*sizeof(float));
	tmp->bone_flag=gcralloc(mdl->max_bones*sizeof(int));

	return(tmp);
}

LBXGL_API void Skel_FreeInstance(LBXGL_Skel2State *ctx)
{
	if(!ctx)return;

	SkelVBO_DestroyVBO(ctx);

	if(ctx->bone_rorg)gcfree(ctx->bone_rorg);
	if(ctx->bone_rmat)gcfree(ctx->bone_rmat);

	if(ctx->cur_xyz)gcfree(ctx->cur_xyz);
	if(ctx->cur_norm)gcfree(ctx->cur_norm);
	if(ctx->cur_vnorm)gcfree(ctx->cur_vnorm);
	
	gcfree(ctx);
}

LBXGL_API void Skel_FlushInstance(LBXGL_Skel2State *ctx)
{
	if(!ctx)return;

	SkelVBO_DestroyVBO(ctx);

//	if(ctx->bone_rorg)gcfree(ctx->bone_rorg);
//	if(ctx->bone_rmat)gcfree(ctx->bone_rmat);

	if(ctx->cur_xyz)gcfree(ctx->cur_xyz);
	if(ctx->cur_norm)gcfree(ctx->cur_norm);
	if(ctx->cur_vnorm)gcfree(ctx->cur_vnorm);
	
	ctx->cur_xyz=NULL;
	ctx->cur_norm=NULL;
	ctx->cur_vnorm=NULL;
	
//	gcfree(ctx);
}

#if 1
LBXGL_API void Skel_LerpRotFrame(LBXGL_Skel2State *ctx,
	int num, float *rot, LBXGL_Skel2Anim *anim, int frame)
{
	float f;
	int i, j, k, t0, t1;

	//no frames, use base rot
	if(!anim->num_frames ||
		(frame>=anim->num_frames))
	{
		for(i=0; i<9; i++)rot[i]=ctx->mdl->bone_rot[num*9+i];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}

	//keyframe
	i=frame*ctx->mdl->max_bones+num;
	if(anim->frame_mask[i>>3]&(1<<(i&7)))
	{
		for(j=0; j<9; j++)rot[j]=anim->frame_rot[i*9+j];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}

	i=frame;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i--;
		if(i<0)i+=anim->num_frames;
		if(i==frame)break;
	}
	t0=i;

	i=frame;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i++;
		if(i>=anim->num_frames)i-=anim->num_frames;
		if(i==frame)break;
	}
	t1=i;

	//no keyframes, use base rot
	if((t0==frame) || (t1==frame))
	{
		for(i=0; i<9; i++)rot[i]=ctx->mdl->bone_rot[num*9+i];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}

	//same keyframe, no need to lerp
	if(t0==t1)
	{
		i=t0*ctx->mdl->max_bones+num;
		for(j=0; j<9; j++)rot[j]=anim->frame_rot[i*9+j];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}


	i=frame; j=t1;
	if(i<t0)i+=anim->num_frames;
	if(j<t0)j+=anim->num_frames;
	f=((float)(i-t0))/(j-t0);

	i=t0*ctx->mdl->max_bones+num;
	j=t1*ctx->mdl->max_bones+num;

	Mat3F_Interpolate(anim->frame_rot+i*9,
		anim->frame_rot+j*9, rot, f);
	Mat3F_Filter3Matrix(rot, rot);
}

LBXGL_API void Skel_LerpOrgFrame(LBXGL_Skel2State *ctx,
	int num, float *org, LBXGL_Skel2Anim *anim, int frame)
{
	float f;
	int i, j, k, t0, t1;

	//no frames, use base rot
	if(!anim->num_frames ||
		(frame>=anim->num_frames))
	{
		org[0]=ctx->mdl->bone_org[num*3+0];
		org[1]=ctx->mdl->bone_org[num*3+1];
		org[2]=ctx->mdl->bone_org[num*3+2];
		return;
	}

	//keyframe
	i=frame*ctx->mdl->max_bones+num;
	if(anim->frame_mask[i>>3]&(1<<(i&7)))
	{
		org[0]=anim->frame_org[i*3+0];
		org[1]=anim->frame_org[i*3+1];
		org[2]=anim->frame_org[i*3+2];
		return;
	}

	i=frame;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i--;
		if(i<0)i+=anim->num_frames;
		if(i==frame)break;
	}
	t0=i;

	i=frame;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i++;
		if(i>=anim->num_frames)i-=anim->num_frames;
		if(i==frame)break;
	}
	t1=i;

	//no keyframes, use base rot
	if((t0==frame) || (t1==frame))
	{
		org[0]=ctx->mdl->bone_org[num*3+0];
		org[1]=ctx->mdl->bone_org[num*3+1];
		org[2]=ctx->mdl->bone_org[num*3+2];
		return;
	}

	//same keyframe, no need to lerp
	if(t0==t1)
	{
		i=t0*ctx->mdl->max_bones+num;
		org[0]=anim->frame_org[i*3+0];
		org[1]=anim->frame_org[i*3+1];
		org[2]=anim->frame_org[i*3+2];
		return;
	}

	i=frame; j=t1;
	if(i<t0)i+=anim->num_frames;
	if(j<t0)j+=anim->num_frames;
	f=((float)(i-t0))/(j-t0);

	i=t0*ctx->mdl->max_bones+num;
	j=t1*ctx->mdl->max_bones+num;
	org[0]=anim->frame_org[i*3+0]*(1-f) +
		anim->frame_org[j*3+0]*f;
	org[1]=anim->frame_org[i*3+1]*(1-f) +
		anim->frame_org[j*3+1]*f;
	org[2]=anim->frame_org[i*3+2]*(1-f) +
		anim->frame_org[j*3+2]*f;
}

LBXGL_API void Skel_LerpRot(LBXGL_Skel2State *ctx, int num, float *rot)
{
	float rot0[9], rot1[9];
	float f;
	
	if(!ctx->last_anim)
	{
		Skel_LerpRotFrame(ctx, num, rot, ctx->anim, ctx->frame_cur);
		return;
	}

	Skel_LerpRotFrame(ctx, num, rot0, ctx->last_anim, ctx->last_frame);
	Skel_LerpRotFrame(ctx, num, rot1, ctx->anim, ctx->frame_cur);

	f=1.0-ctx->backlerp;
	if(f<0)f=0; if(f>1)f=1;

	Mat3F_Interpolate(rot0, rot1, rot, f);
	Mat3F_Filter3Matrix(rot, rot);
}

LBXGL_API void Skel_LerpOrg(LBXGL_Skel2State *ctx, int num, float *org)
{
	float org0[9], org1[9];
	float f;

	if(!ctx->last_anim)
	{
		Skel_LerpOrgFrame(ctx, num, org, ctx->anim, ctx->frame_cur);
		return;
	}

	Skel_LerpOrgFrame(ctx, num, org0, ctx->last_anim, ctx->last_frame);
	Skel_LerpOrgFrame(ctx, num, org1, ctx->anim, ctx->frame_cur);

	f=1.0-ctx->backlerp;
	if(f<0)f=0; if(f>1)f=1;

	org[0]=org0[0]*(1-f) + org1[0]*f;
	org[1]=org0[1]*(1-f) + org1[1]*f;
	org[2]=org0[2]*(1-f) + org1[2]*f;
}

#endif


LBXGL_API LBXGL_Skel2MeshState *Skel_NewMeshInstance(LBXGL_Skel2Mesh *prim)
{
	LBXGL_Skel2MeshState *tmp;

	tmp=gctalloc("lbxgl_skel2meshstate_t", sizeof(LBXGL_Skel2MeshState));
	tmp->prim=prim;

	return(tmp);
}

LBXGL_API LBXGL_Skel2MeshStateGroup *Skel_NewGroupInstance(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	LBXGL_Skel2MeshStateGroup *gtmp, *gfst, *glst;
	LBXGL_Skel2MeshState *mtmp, *mfst, *mlst;

	gcur=grp->first; gfst=NULL; glst=NULL;
	while(gcur)
	{
		gtmp=Skel_NewGroupInstance(gcur);
		if(glst) { glst->next=gtmp; glst=gtmp; }
			else { gfst=gtmp; glst=gtmp; }
		gcur=gcur->next;
	}

	mcur=grp->prim; mfst=NULL; mlst=NULL;
	while(mcur)
	{
		mtmp=Skel_NewMeshInstance(mcur);
		if(mlst) { mlst->next=mtmp; mlst=mtmp; }
			else { mfst=mtmp; mlst=mtmp; }
		mcur=mcur->next;
	}

	gtmp=gctalloc("lbxgl_skel2meshstategroup_t",
		sizeof(LBXGL_Skel2MeshStateGroup));

	gtmp->first=gfst;
	gtmp->prim=mfst;
	if(grp->name)gtmp->name=dystrdup(grp->name);

	return(gtmp);
}

LBXGL_API void Skel_SetupMeshState(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshState *mesh)
{
	static float txyz[65536*3];
	static float tnorm[16384*4];
	LBXGL_Skel2MeshFace *cur, *cur2;
	float im[16];
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *v0, *v1, *imv;
	int i, j, k, l, l1, tn;
	float f, g;

	if(!mesh->prim->vidx)return;

	if(mesh->prim->n_vecs>=65536)
		{ *(int *)-1=-1; }
	if(mesh->prim->n_faces>=16384)
		{ *(int *)-1=-1; }

	if(!mesh->curvecs)
	{
		mesh->curvecs=gcatomic(mesh->prim->n_vecs*3*sizeof(float));
		mesh->curnorms=gcatomic(mesh->prim->n_faces*4*sizeof(float));
		mesh->curvnorms=gcatomic(mesh->prim->n_faces*3*4*sizeof(float));
	}

	//calculate vertex poses
	for(i=0; i<mesh->prim->n_vecs; i++)
	{
		V3F_ZERO(txyz+i*3);
		g=0;

		j=mesh->prim->vidx[i];
		k=mesh->prim->vnum[i];
		while(k--)
		{
			v0=mesh->prim->wxyz+j*3;
			v1=ctx->bone_rorg+mesh->prim->widx[j]*3;
			imv=ctx->bone_rmat+mesh->prim->widx[j]*9;

			tv[0]=v0[0]*imv[0]+v0[1]*imv[3]+v0[2]*imv[6]+v1[0];
			tv[1]=v0[0]*imv[1]+v0[1]*imv[4]+v0[2]*imv[7]+v1[1];
			tv[2]=v0[0]*imv[2]+v0[1]*imv[5]+v0[2]*imv[8]+v1[2];

			f=mesh->prim->wval[j];
			txyz[i*3+0]+=tv[0]*f;
			txyz[i*3+1]+=tv[1]*f;
			txyz[i*3+2]+=tv[2]*f;
			g+=f;

			j++;
		}

		if(g<0.01)g=1;
		txyz[i*3+0]/=g;
		txyz[i*3+1]/=g;
		txyz[i*3+2]/=g;

//		V3F_SUB(txyz+i*3, ctx->mdl->mdlorg, txyz+i*3);

		txyz[i*3+0]=((txyz[i*3+0]-ctx->mdl->mdlorg[0])*
			ctx->mdl->mdlscale[0])+ctx->mdl->mdlxlat[0];
		txyz[i*3+1]=((txyz[i*3+1]-ctx->mdl->mdlorg[1])*
			ctx->mdl->mdlscale[1])+ctx->mdl->mdlxlat[1];
		txyz[i*3+2]=((txyz[i*3+2]-ctx->mdl->mdlorg[2])*
			ctx->mdl->mdlscale[2])+ctx->mdl->mdlxlat[2];
	}

	for(i=0; i<mesh->prim->n_vecs; i++)
		{ V3F_COPY(txyz+i*3, mesh->curvecs+i*3); }

	//calc face normals
	for(i=0; i<mesh->prim->n_faces; i++)
	{
		cur=mesh->prim->face[i];
		V3F_SUB(txyz+cur->vecs[1]*3, txyz+cur->vecs[0]*3, tv0);
		V3F_SUB(txyz+cur->vecs[2]*3, txyz+cur->vecs[1]*3, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tnorm+i*4);

		V3F_COPY(tnorm+i*4, mesh->curnorms+i*4);
		mesh->curnorms[i*4+3]=V3F_DOT(txyz+cur->vecs[0]*3, tnorm+i*4);
	}

	for(i=0; i<mesh->prim->n_faces; i++)
	{
		cur=mesh->prim->face[i];

		for(j=0; j<3; j++)
		{
			V3F_ZERO(tv);
			k=cur->vecs[j];
			for(l=0; l<mesh->prim->vfnum[k]; l++)
			{
				l1=mesh->prim->vfids[mesh->prim->vfidx[k]+l];
				f=V3F_DOT(tnorm+i*4, tnorm+l1*4);

				if(f>1)f=1;
				if(f>0)
				{
					V3F_ADDSCALE(tv, tnorm+l1*4,
						f, tv);
				}
			}

			V3F_NORMALIZE(tv, tv);
			V3F_COPY(tv, mesh->curvnorms+i*12+j*4);
			mesh->curvnorms[i*12+j*4+3]=
				V3F_DOT(txyz+cur->vecs[0]*3, tv);
		}
	}
}

LBXGL_API void Skel_SetupMeshStateGroup(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_SetupMeshStateGroup(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_SetupMeshState(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_SetupFrameBasic(LBXGL_Skel2State *ctx)
{
	LBXGL_Skel2Model *mdl;
	float pt[3], io[3], im[9], *imv;
	int i, j;

	mdl=ctx->mdl;

	if(ctx->frame_calc)return;

	if(!ctx->anim)
	{
		for(i=0; i<mdl->num_bones; i++)
		{
			if(ctx->bone_flag[i]&1)
				continue;
	
			j=mdl->bone_up[i];

			if(j>=0)
			{
				V3F_ADD(mdl->bone_org+i*3,
					ctx->bone_rorg+j*3,
					ctx->bone_rorg+i*3);
				Mat3F_Mat3Mult(mdl->bone_rot+i*9,
					ctx->bone_rmat+j*9,
					ctx->bone_rmat+i*9);
			}else
			{
				V3F_COPY(mdl->bone_org+i*3,
					ctx->bone_rorg+i*3);
				Mat3F_Identity(ctx->bone_rmat+i*9);
			}
		}

//		Skel_SetupMeshTempPos(ctx);
//		ctx->frame_calc=1;
		return;
	}

	for(i=0; i<ctx->mdl->num_bones; i++)
	{
		j=ctx->mdl->bone_up[i];
		if(j<0)
		{
			Skel_LerpOrg(ctx, i, ctx->bone_rorg+i*3);
			Skel_LerpRot(ctx, i, ctx->bone_rmat+i*9);

			if(ctx->bone_flag[i]&1)
				{ Vec3F_Copy(ctx->bone_uorg+i*3, ctx->bone_rorg+i*3); }
			if(ctx->bone_flag[i]&2)
				{ Mat3F_Copy(ctx->bone_umat+i*9, ctx->bone_rmat+i*9); }
		}else
		{
			if(ctx->bone_flag[i]&1)
				{ Vec3F_Copy(ctx->bone_uorg+i*3, pt); }
			else
				{ Skel_LerpOrg(ctx, i, pt); }

			if(ctx->bone_flag[i]&2)
				{ Mat3F_Copy(ctx->bone_umat+i*9, im); }
			else
				{ Skel_LerpRot(ctx, i, im); }

			imv=ctx->bone_rmat+j*9;
			Mat3F_Mat3Mult(im, imv, ctx->bone_rmat+i*9);
			io[0]=pt[0]*imv[0]+pt[1]*imv[3]+pt[2]*imv[6];
			io[1]=pt[0]*imv[1]+pt[1]*imv[4]+pt[2]*imv[7];
			io[2]=pt[0]*imv[2]+pt[1]*imv[5]+pt[2]*imv[8];
			V3F_ADD(io, ctx->bone_rorg+j*3, io);
			V3F_COPY(io, ctx->bone_rorg+i*3);
		}
	}

#if 0
	if(!ctx->cur_xyz)
	{
		ctx->cur_xyz=gctalloc("lbxgl_skel_base",
			ctx->mdl->n_xyz*3*sizeof(float));
		ctx->cur_norm=gctalloc("lbxgl_skel_base",
			ctx->mdl->n_face*4*sizeof(float));
		ctx->cur_vnorm=gctalloc("lbxgl_skel_base",
			ctx->mdl->n_face*3*4*sizeof(float));
	}
#endif

//	Skel_SetupMeshTempPos(ctx);
//	if(ctx->mesh)Skel_SetupMeshStateGroup(ctx, ctx->mesh);
//	ctx->frame_calc=1;
}

LBXGL_API void Skel_SetupFrame(LBXGL_Skel2State *ctx)
{
	if(ctx->frame_calc)
		return;

	Skel_SetupFrameBasic(ctx);

#if 1
	if(!ctx->cur_xyz)
	{
		ctx->cur_xyz=gctalloc("lbxgl_skel_base",
			ctx->mdl->n_xyz*3*sizeof(float));
		ctx->cur_norm=gctalloc("lbxgl_skel_base",
			ctx->mdl->n_face*4*sizeof(float));
		ctx->cur_vnorm=gctalloc("lbxgl_skel_base",
			ctx->mdl->n_face*3*4*sizeof(float));
	}
#endif

	Skel_SetupMeshTempPos(ctx);
	if(ctx->mesh)Skel_SetupMeshStateGroup(ctx, ctx->mesh);
	
	SkelVBO_RebuildVBO(ctx);
	
	ctx->frame_calc=1;
}

LBXGL_API void Skel_SetFrameBone(LBXGL_Skel2State *ctx,
	int bone, float *org, float *qrot, int flag)
{
	ctx->bone_flag[bone]=flag;
	
	if((flag&1) && !ctx->bone_uorg)
		ctx->bone_uorg=gcalloc(ctx->mdl->max_bones*3*sizeof(float));
	if((flag&2) && !ctx->bone_umat)
		ctx->bone_umat=gcalloc(ctx->mdl->max_bones*9*sizeof(float));
	
	if(org)
		{ Vec3F_Copy(org, ctx->bone_uorg+bone*3); }
	if(qrot)
		{ QuatF_To3Matrix(qrot, ctx->bone_umat+bone*9); }
}

LBXGL_API void Skel_SetAnimFrame(LBXGL_Skel2State *ctx,
	char *name, int frame)
{
	char base[16];
	LBXGL_Skel2Anim *cur;
	char *s, *t;
	int fr;

	if(!name)
	{
		if(ctx->anim || ctx->frame_cur)
			ctx->frame_calc=0;

		ctx->anim=NULL;
		ctx->frame_cur=0;
//		ctx->frame_calc=0;
		return;
	}

	s=name;
	t=base;
	while(*s && (*s!=':'))*t++=*s++;
	*t++=0;

	if(*s==':')fr=atoi(s+1);
		else fr=frame;

	cur=ctx->mdl->anim;
	while(cur)
	{
		if(!strcmp(cur->name, base))break;
		cur=cur->next;
	}

	if(cur)
	{
		if((ctx->anim!=cur) || (ctx->frame_cur!=fr))
			ctx->frame_calc=0;

		ctx->anim=cur;
		ctx->frame_cur=fr;
		return;
	}

	if(ctx->anim || ctx->frame_cur)
		ctx->frame_calc=0;
	ctx->anim=NULL;
	ctx->frame_cur=0;
//	ctx->frame_calc=0;
}

LBXGL_API void Skel_SetAnim(LBXGL_Skel2State *ctx, char *name)
{
	Skel_SetAnimFrame(ctx, name, 0);
}

LBXGL_API void Skel_SetFrame(LBXGL_Skel2State *ctx, int frame)
{
	LBXGL_Skel2Anim *cur;
	int i, fr;
	
	cur=ctx->mdl->anim;
	while(cur)
	{
		i=frame-cur->frame_base;
		if(i>=0 && i<cur->num_frames)
			break;
		cur=cur->next;
	}

//	ctx->anim=cur;
//	if(cur)ctx->frame_cur=frame-cur->frame_base;
//	ctx->frame_calc=0;

	if(cur)
	{
		fr=frame-cur->frame_base;

		if((ctx->anim!=cur) || (ctx->frame_cur!=fr))
			ctx->frame_calc=0;

		ctx->anim=cur;
		ctx->frame_cur=fr;
	}else
	{
		if(ctx->anim || ctx->frame_cur)
			ctx->frame_calc=0;

		ctx->anim=NULL;
		ctx->frame_cur=0;
//		ctx->frame_calc=0;
	}
	return;
}

LBXGL_API void Skel_StepTime(LBXGL_Skel2State *ctx, float dt)
{
	ctx->frame_time+=dt;

	if(ctx->frame_time>=ctx->anim->frame_time)
	{
		ctx->frame_calc=0;

		ctx->frame_time-=ctx->anim->frame_time;

		ctx->frame_cur++;
		if(ctx->frame_cur>=ctx->anim->num_frames)
		{
			Skel_SetAnim(ctx, ctx->anim->next_anim);
		}
	}
}

LBXGL_API void Skel_BuildMeshEdges(LBXGL_Skel2Mesh *prim)
{
	LBXGL_Skel2MeshFace *cur;
	int *edge;

	int i0, i1, ne;
	int i, j, k;

	if(!prim->edge)
	{
		edge=malloc(2*prim->n_faces*3*4*sizeof(int)); ne=0;
		for(i=0; i<prim->n_faces; i++)
		{
			cur=prim->face[i];
			for(j=0; j<3; j++)
			{
				i0=cur->vecs[j]; i1=cur->vecs[(j+1)%3];
				for(k=0; k<ne; k++)
					if((edge[k*4+0]==i1) && (edge[k*4+1]==i0))
						break;
				if(k<ne) { edge[k*4+3]=i; continue; }
				edge[ne*4+0]=i0; edge[ne*4+1]=i1;
				edge[ne*4+2]=i; edge[ne*4+3]=i;
				ne++;
			}
		}

		prim->n_edge=ne;
		prim->edge=gcatomic(prim->n_edge*4*sizeof(int));
		for(i=0; i<(prim->n_edge*4); i++)prim->edge[i]=edge[i];
		free(edge);
	}
}

void Skel_BuildGroupEdges(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_BuildGroupEdges(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_BuildMeshEdges(mcur);
		mcur=mcur->next;
	}
}
