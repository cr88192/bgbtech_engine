#include <lbxgl2.h>
#include <math.h>

LBXGL_API void Skel_ClampV3F(float *iv, float *ov)
{
	V3F_COPY(iv, ov);
	if(fabs(ov[0])<0.0001)ov[0]=0;
	if(fabs(ov[1])<0.0001)ov[1]=0;
	if(fabs(ov[2])<0.0001)ov[2]=0;
}

LBXGL_API char *Skel_Num2Str(float f)
{
	char *s, *b;
	int i;

	b=gcralloc(16);
//	i=(int)round(f);
	i=floor(f+0.5);
	if(fabs(f-i)<0.001)
	{
		sprintf(b, "%d", i);
		return(b);
	}
	sprintf(b, "%.3f", f);
	s=b+strlen(b);

	s--;
	while((s>b) && (*s=='0'))s--;
	s++; *s++=0;
	return(b);
}

LBXGL_API int Skel_LoadBones(LBXGL_Skel2Model *mdl, char *name)
{
	static LBXGL_Skel2Solid *bone_solid[256];
	static char *bone_name[256];
	static float bone_org[256*3];
	static float bone_rot[256*9];
	static float bone_rorg[256*3];
	static int bone_up[256];
	int num_bones, bone_solids;

	char buf[256];
	char **a;
	VFILE *fd;
	int i, j;
	float *v;

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("can't open bones %s\n", name);
		return(-1);
	}

	for(i=0; i<256; i++)bone_name[i]=NULL;

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
				gcalloc(sizeof(LBXGL_Skel2Solid));

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
		}

		if(!strcmp(a[0], "capsule"))
		{
			bone_solid[bone_solids]=
				gcalloc(sizeof(LBXGL_Skel2Solid));

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
		}

		if(!strcmp(a[0], "sphere"))
		{
			bone_solid[bone_solids]=
				gcalloc(sizeof(LBXGL_Skel2Solid));

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
		}

		if((*a[0]<'0') || (*a[0]>'9'))
			continue;

		i=atoi(a[0]);
		if(i>=num_bones)num_bones=i+1;

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
		mdl->bone_name=gcalloc(num_bones*sizeof(char *));
		mdl->bone_up=gcalloc(num_bones*sizeof(int));
		mdl->bone_org=gcalloc(num_bones*3*sizeof(float));
		mdl->bone_rot=gcalloc(num_bones*9*sizeof(float));
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
	mdl->solid=gcalloc(bone_solids*sizeof(LBXGL_Skel2Solid *));
	for(i=0; i<bone_solids; i++)mdl->solid[i]=bone_solid[i];

	return(0);
}

LBXGL_API int Skel_SaveBones(LBXGL_Skel2Model *mdl, char *name)
{
	static float bone_rorg[256*3];
	float tv0[3], tv1[3];
	char buf[256];
	char **a;
	VFILE *fd;
	int i, j;

	fd=vffopen(name, "wt");
	if(!fd)
	{
		printf("can't write bones %s\n", name);
		return(-1);
	}

	printf("saving bones %s\n", name);

	for(i=0; i<mdl->num_bones; i++)
	{
		if(!mdl->bone_name[i])continue;

		bone_rorg[i*3+0]=mdl->bone_org[i*3+0];
		bone_rorg[i*3+1]=mdl->bone_org[i*3+1];
		bone_rorg[i*3+2]=mdl->bone_org[i*3+2];
		Skel_ClampV3F(bone_rorg+i*3, bone_rorg+i*3);

		if(mdl->bone_up[i]>=0)
		{
			j=mdl->bone_up[i];
			bone_rorg[i*3+0]+=bone_rorg[j*3+0];
			bone_rorg[i*3+1]+=bone_rorg[j*3+1];
			bone_rorg[i*3+2]+=bone_rorg[j*3+2];
		}

		vfprint(fd, "%d %d \"%s\" [ %.4g %.4g %.4g ]\n", i,
			mdl->bone_up[i], mdl->bone_name[i],
			bone_rorg[i*3+0], bone_rorg[i*3+1],
			bone_rorg[i*3+2]);
	}

	vfprint(fd, "\n\n");

	for(i=0; i<mdl->num_solids; i++)
	{
		j=mdl->solid[i]->bone;
		V3F_ADD(mdl->solid[i]->m, bone_rorg+j*3, tv0);
		V3F_ADD(mdl->solid[i]->n, bone_rorg+j*3, tv1);
		Skel_ClampV3F(tv0, tv0);
		Skel_ClampV3F(tv1, tv1);

		switch(mdl->solid[i]->type)
		{
		case BONE_SOLID_BBOX:
			vfprint(fd, "bbox %d [ %.4g %.4g %.4g ] "
				"[ %.4g %.4g %.4g ] %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				tv1[0], tv1[1], tv1[2],
				mdl->solid[i]->r0, mdl->solid[i]->w);
			break;
		case BONE_SOLID_SPHERE:
			vfprint(fd, "sphere %d [ %.4g %.4g %.4g ] "
					"%.4g %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				mdl->solid[i]->r0, mdl->solid[i]->r1,
				mdl->solid[i]->w);
			break;
		case BONE_SOLID_CAPSULE:
			vfprint(fd, "capsule %d [ %.4g %.4g %.4g ] "
				"[ %.4g %.4g %.4g ] %.4g %.4g %.4g\n",
				mdl->solid[i]->bone,
				tv0[0], tv0[1], tv0[2],
				tv1[0], tv1[1], tv1[2],
				mdl->solid[i]->r0, mdl->solid[i]->r1,
				mdl->solid[i]->w);
			break;
		default:
			break;
		}
	}

	vfclose(fd);
	return(0);
}



LBXGL_API LBXGL_Skel2Anim *Skel_LoadAnimRaw(LBXGL_Skel2Model *mdl, char *name)
{
	static float frame_org[256*64*3];
	static float frame_rot[256*64*9];
	static byte frame_mask[256*64/8];
	int n_frames, frame_cur;

	LBXGL_Skel2Anim *tmp;

	char buf[256];
	float pos[16];
	char **a;
	VFILE *fd;
	int i, j, k, l;

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("can't open anim %s\n", name);
		return(NULL);
	}

	memset(frame_mask, 0, 256*64/8);
	n_frames=0;
	frame_cur=0;

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
		for(k=0; k<3; k++)frame_org[j*3+k]=atof(a[2+k]);
		for(k=0; k<9; k++)frame_rot[j*9+k]=atof(a[7+k]);
		frame_mask[j>>3]|=1<<(j&7);
	}
	vfclose(fd);

	tmp=gcalloc(sizeof(LBXGL_Skel2Anim));

	tmp->num_frames=n_frames;
	if(n_frames<64)n_frames=64;

	tmp->frame_org=gcalloc(n_frames*mdl->max_bones*3*sizeof(float));
	tmp->frame_rot=gcalloc(n_frames*mdl->max_bones*9*sizeof(float));
	tmp->frame_mask=gcalloc((n_frames*mdl->max_bones+7)/8);

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

LBXGL_API int Skel_SaveAnim(LBXGL_Skel2Model *mdl, LBXGL_Skel2Anim *anim, char *name)
{
	char buf[256];
	float pos[16];
	char **a;
	VFILE *fd;
	float f;
	int i, j, k, l;

	fd=vffopen(name, "wt");
	if(!fd)
	{
		printf("can't write anim %s\n", name);
		return(-1);
	}

	printf("saving anim %s\n", name);

	for(i=0; i<anim->num_frames; i++)
	{
		l=-1;
		vfprint(fd, "frame %d\n", i);
		for(j=0; j<mdl->num_bones; j++)
		{
			if(!mdl->bone_name[j])continue;

			k=i*mdl->max_bones+j;
			if(anim->frame_mask[k>>3]&(1<<(k&7)))
			{
				vfprint(fd, "%s [ ", mdl->bone_name[j]);
				for(l=0; l<3; l++)
				{
					f=anim->frame_org[k*3+l];
//					if(fabs(f)<0.0001)f=0;
//					vfprint(fd, "%.4g ", f);

					vfprint(fd, "%s ", Skel_Num2Str(f));
				}
				vfprint(fd, "] [ ");
				for(l=0; l<9; l++)
				{
					f=anim->frame_rot[k*9+l];
//					if(fabs(f)<0.0001)f=0;
//					vfprint(fd, "%.4g ", f);
					vfprint(fd, "%s ", Skel_Num2Str(f));
				}
				vfprint(fd, "]\n");
			}
		}
		if(l>=0)vfprint(fd, "\n");
	}
	vfclose(fd);
	return(0);
}


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

LBXGL_API int Skel_WeightVertex(LBXGL_Skel2State *ctx,
	float *pt, float *txyz, int *tidx, float *tval, int n)
{
	float tv[3], tv1[3], bv[3];
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

			V3F_COPY(tv, txyz+n*3);
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
			if(d<0.01)continue;

			V3F_SUB(pt, ctx->bone_rorg+3*k, tv);
			tv1[0]=V3F_DOT(tv, ctx->bone_rmat+9*k+0);
			tv1[1]=V3F_DOT(tv, ctx->bone_rmat+9*k+3);
			tv1[2]=V3F_DOT(tv, ctx->bone_rmat+9*k+6);

			if(d>bd)
			{
				V3F_COPY(tv1, bv);
				bi=k; bd=d;
			}
		}

		V3F_COPY(bv, txyz+n*3);
		tidx[n]=bi;
		tval[n]=bd;
		n++;
	}

	return(n);
}

LBXGL_API void Skel_BindMesh(LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *mesh)
{
	static float txyz[65536*3];
	static int tidx[65536];
	static float tval[65536];

	LBXGL_Skel2MeshFace *cur;
	float tv[3], tv1[3], bv[3];
	int i, j, k, n, bi;
	float d, bd;

	mesh->vidx=gcalloc(mesh->n_vecs*sizeof(int));
	mesh->vnum=gcalloc(mesh->n_vecs*sizeof(int));

	n=0;
	for(i=0; i<mesh->n_vecs; i++)
	{
		mesh->vidx[i]=n;

		n=Skel_WeightVertex(ctx, mesh->vecs+i*3, txyz, tidx, tval, n);

		mesh->vnum[i]=n-mesh->vidx[i];
	}

	mesh->wxyz=gcalloc(3*n*sizeof(float));
	mesh->widx=gcalloc(n*sizeof(int));
	mesh->wval=gcalloc(n*sizeof(float));

	for(i=0; i<n; i++)
	{
		V3F_COPY(txyz+i*3, mesh->wxyz+i*3);
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

LBXGL_API void Skel_UnbindMesh(LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *mesh)
{
	mesh->n_weights=0;

	if(mesh->vidx)gcfree(mesh->vidx);
	if(mesh->vnum)gcfree(mesh->vnum);
	if(mesh->wxyz)gcfree(mesh->wxyz);
	if(mesh->widx)gcfree(mesh->widx);
	if(mesh->wval)gcfree(mesh->wval);

	mesh->vidx=NULL;
	mesh->vnum=NULL;
	mesh->wxyz=NULL;
	mesh->widx=NULL;
	mesh->wval=NULL;
}

LBXGL_API void Skel_UnbindMeshGroup(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshGroup *grp)
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

LBXGL_API int Skel_LoadMeshAC3D(LBXGL_Skel2Model *mdl, char *name)
{
	static float org[64*3];
	static float rot[64*9];
	LBXGL_Skel2State ctx;
	int i, j;

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

	mdl->mesh=AC3D_LoadModel(name);
	Skel_BindMeshGroup(&ctx, mdl->mesh);

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

LBXGL_API LBXGL_Skel2Model *Skel_LoadModel(char *name)
{
	char buf[256];
	float pos[16];
	LBXGL_Skel2Model *tmp;
	LBXGL_Skel2Anim *atmp;
	char **a;
	char *base, *s, *t;
	VFILE *fd;
	int i, j;

	strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='/') && (*s!='\\'))s--;
	*s=0;
	base=strdup(buf);

	fd=vffopen(name, "rt");
	if(!fd)
	{
		printf("can't open model %s\n", name);
		return(NULL);
	}

//	tmp=gcalloc(sizeof(LBXGL_Skel2Model));
	tmp=gctalloc("lbxgl_skel2model_t", sizeof(LBXGL_Skel2Model));

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

			atmp->next=tmp->anim;
			tmp->anim=atmp;

			continue;
		}

		if(!strcmp(a[0], "origin"))
		{
			tmp->mdlorg[0]=atof(a[1]);
			tmp->mdlorg[1]=atof(a[2]);
			tmp->mdlorg[2]=atof(a[3]);
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

	tmp->bone_rorg=gcalloc(mdl->max_bones*3*sizeof(float));
	tmp->bone_rmat=gcalloc(mdl->max_bones*9*sizeof(float));

	return(tmp);
}


LBXGL_API void Skel_LerpRot(LBXGL_Skel2State *ctx, int num, float *rot)
{
	float f;
	int i, j, k, t0, t1;

	//no frames, use base rot
	if(!ctx->anim->num_frames ||
		(ctx->frame_cur>=ctx->anim->num_frames))
	{
		for(i=0; i<9; i++)rot[i]=ctx->mdl->bone_rot[num*9+i];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}

	//keyframe
	i=ctx->frame_cur*ctx->mdl->max_bones+num;
	if(ctx->anim->frame_mask[i>>3]&(1<<(i&7)))
	{
		for(j=0; j<9; j++)rot[j]=ctx->anim->frame_rot[i*9+j];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}

	i=ctx->frame_cur;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(ctx->anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i--;
		if(i<0)i+=ctx->anim->num_frames;
		if(i==ctx->frame_cur)break;
	}
	t0=i;

	i=ctx->frame_cur;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(ctx->anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i++;
		if(i>=ctx->anim->num_frames)i-=ctx->anim->num_frames;
		if(i==ctx->frame_cur)break;
	}
	t1=i;

	//no keyframes, use base rot
	if((t0==ctx->frame_cur) || (t1==ctx->frame_cur))
	{
		for(i=0; i<9; i++)rot[i]=ctx->mdl->bone_rot[num*9+i];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}

	//same keyframe, no need to lerp
	if(t0==t1)
	{
		i=t0*ctx->mdl->max_bones+num;
		for(j=0; j<9; j++)rot[j]=ctx->anim->frame_rot[i*9+j];
		Mat3F_Filter3Matrix(rot, rot);
		return;
	}


	i=ctx->frame_cur; j=t1;
	if(i<t0)i+=ctx->anim->num_frames;
	if(j<t0)j+=ctx->anim->num_frames;
	f=((float)(i-t0))/(j-t0);

	i=t0*ctx->mdl->max_bones+num;
	j=t1*ctx->mdl->max_bones+num;

	Mat3F_Interpolate(ctx->anim->frame_rot+i*9,
		ctx->anim->frame_rot+j*9, rot, f);
	Mat3F_Filter3Matrix(rot, rot);
}

LBXGL_API void Skel_LerpOrg(LBXGL_Skel2State *ctx, int num, float *org)
{
	float f;
	int i, j, k, t0, t1;

	//no frames, use base rot
	if(!ctx->anim->num_frames ||
		(ctx->frame_cur>=ctx->anim->num_frames))
	{
		org[0]=ctx->mdl->bone_org[num*3+0];
		org[1]=ctx->mdl->bone_org[num*3+1];
		org[2]=ctx->mdl->bone_org[num*3+2];
		return;
	}

	//keyframe
	i=ctx->frame_cur*ctx->mdl->max_bones+num;
	if(ctx->anim->frame_mask[i>>3]&(1<<(i&7)))
	{
		org[0]=ctx->anim->frame_org[i*3+0];
		org[1]=ctx->anim->frame_org[i*3+1];
		org[2]=ctx->anim->frame_org[i*3+2];
		return;
	}

	i=ctx->frame_cur;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(ctx->anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i--;
		if(i<0)i+=ctx->anim->num_frames;
		if(i==ctx->frame_cur)break;
	}
	t0=i;

	i=ctx->frame_cur;
	while(1)
	{
		j=i*ctx->mdl->max_bones+num;
		if(ctx->anim->frame_mask[j>>3]&(1<<(j&7)))
			break;
		i++;
		if(i>=ctx->anim->num_frames)i-=ctx->anim->num_frames;
		if(i==ctx->frame_cur)break;
	}
	t1=i;

	//no keyframes, use base rot
	if((t0==ctx->frame_cur) || (t1==ctx->frame_cur))
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
		org[0]=ctx->anim->frame_org[i*3+0];
		org[1]=ctx->anim->frame_org[i*3+1];
		org[2]=ctx->anim->frame_org[i*3+2];
		return;
	}


	i=ctx->frame_cur; j=t1;
	if(i<t0)i+=ctx->anim->num_frames;
	if(j<t0)j+=ctx->anim->num_frames;
	f=((float)(i-t0))/(j-t0);

	i=t0*ctx->mdl->max_bones+num;
	j=t1*ctx->mdl->max_bones+num;
	org[0]=ctx->anim->frame_org[i*3+0]*(1-f) +
		ctx->anim->frame_org[j*3+0]*f;
	org[1]=ctx->anim->frame_org[i*3+1]*(1-f) +
		ctx->anim->frame_org[j*3+1]*f;
	org[2]=ctx->anim->frame_org[i*3+2]*(1-f) +
		ctx->anim->frame_org[j*3+2]*f;
}


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

		V3F_SUB(txyz+i*3, ctx->mdl->mdlorg, txyz+i*3);
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

LBXGL_API void Skel_SetupFrame(LBXGL_Skel2State *ctx)
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

		ctx->frame_calc=1;
		return;
	}

	for(i=0; i<ctx->mdl->num_bones; i++)
	{
		j=ctx->mdl->bone_up[i];
		if(j<0)
		{
			Skel_LerpOrg(ctx, i, ctx->bone_rorg+i*3);
			Skel_LerpRot(ctx, i, ctx->bone_rmat+i*9);
		}else
		{
			Skel_LerpOrg(ctx, i, pt);
			Skel_LerpRot(ctx, i, im);

			imv=ctx->bone_rmat+j*9;
			Mat3F_Mat3Mult(im, imv, ctx->bone_rmat+i*9);
			io[0]=pt[0]*imv[0]+pt[1]*imv[3]+pt[2]*imv[6];
			io[1]=pt[0]*imv[1]+pt[1]*imv[4]+pt[2]*imv[7];
			io[2]=pt[0]*imv[2]+pt[1]*imv[5]+pt[2]*imv[8];
			V3F_ADD(io, ctx->bone_rorg+j*3, io);
			V3F_COPY(io, ctx->bone_rorg+i*3);
		}
	}

	if(ctx->mesh)Skel_SetupMeshStateGroup(ctx, ctx->mesh);
	ctx->frame_calc=1;
}

LBXGL_API void Skel_SetAnim(LBXGL_Skel2State *ctx, char *name)
{
	char base[16];
	LBXGL_Skel2Anim *cur;
	char *s, *t;
	int fr;

	if(!name)
	{
		ctx->frame_cur=0;
		ctx->frame_calc=0;
		return;
	}

	s=name;
	t=base;
	while(*s && (*s!=':'))*t++=*s++;
	*t++=0;

	if(*s==':')fr=atoi(s+1);
		else fr=0;

	cur=ctx->mdl->anim;
	while(cur)
	{
		if(!strcmp(cur->name, base))break;
		cur=cur->next;
	}

	if(cur)
	{
		ctx->anim=cur;
		ctx->frame_cur=fr;
		return;
	}

	ctx->frame_cur=0;
	ctx->frame_calc=0;
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

#if 1
LBXGL_API void Skel_DrawMesh(LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *mesh)
{
	static float txyz[65536*3];
	static float tnorm[16384*3];
	LBXGL_Skel2MeshFace *cur, *cur2;
	float im[16];
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *v0, *v1, *imv;
	int i, j, k, l, l1, tn;
	float f, g;

	if(!mesh->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh, im);
		return;
	}

	//calculate vertex poses
	for(i=0; i<mesh->n_vecs; i++)
	{
		V3F_ZERO(txyz+i*3);
		g=0;

		j=mesh->vidx[i];
		k=mesh->vnum[i];
		while(k--)
		{
			v0=mesh->wxyz+j*3;
			v1=ctx->bone_rorg+mesh->widx[j]*3;
			imv=ctx->bone_rmat+mesh->widx[j]*9;

			tv[0]=v0[0]*imv[0]+v0[1]*imv[3]+v0[2]*imv[6]+v1[0];
			tv[1]=v0[0]*imv[1]+v0[1]*imv[4]+v0[2]*imv[7]+v1[1];
			tv[2]=v0[0]*imv[2]+v0[1]*imv[5]+v0[2]*imv[8]+v1[2];

			f=mesh->wval[j];
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
	}

	//calc face normals
	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];
		V3F_SUB(txyz+cur->vecs[1]*3, txyz+cur->vecs[0]*3, tv0);
		V3F_SUB(txyz+cur->vecs[2]*3, txyz+cur->vecs[1]*3, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_NORMALIZE(tv2, tnorm+i*3);
	}

	for(tn=0; tn<mesh->n_tex; tn++)
	{
		glDisable(GL_TEXTURE_2D);
		if(mesh->texnum[tn]>=0)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mesh->texnum[tn]);
		}

		glBegin(GL_TRIANGLES);
		i=mesh->texface[tn];
		while(1)
		{
			cur=mesh->face[i];

			glNormal3fv(tnorm+i*3);

			for(j=0; j<3; j++)
			{
				V3F_ZERO(tv);
				k=cur->vecs[j];
				for(l=0; l<mesh->vfnum[k]; l++)
				{
					l1=mesh->vfids[mesh->vfidx[k]+l];
					f=V3F_DOT(tnorm+i*3, tnorm+l1*3);

					if(f>1)f=1;
					if(f>0)
					{
						V3F_ADDSCALE(tv, tnorm+l1*3,
							f, tv);
					}
				}
				glNormal3fv(tv);

				glTexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
				V3F_COPY(txyz+(3*cur->vecs[j]), tv);

				glVertex3fv(tv);
			}

			i=cur->tnext;
			if(i<=0)break;
		}
		glEnd();
	}
}

LBXGL_API void Skel_DrawMeshGroup(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshGroup(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMesh(ctx, mcur);
		mcur=mcur->next;
	}
}
#endif

#if 1
LBXGL_API void Skel_DrawMeshState(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshState *mesh)
{
	float im[16];
	LBXGL_Skel2MeshFace *cur;
	int i, j, k, l, l1, tn;

	if(!mesh->prim->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh->prim, im);
		return;
	}

	for(tn=0; tn<mesh->prim->n_tex; tn++)
	{
		glDisable(GL_TEXTURE_2D);
		if(mesh->prim->texnum[tn]>=0)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mesh->prim->texnum[tn]);
		}

		glBegin(GL_TRIANGLES);
		i=mesh->prim->texface[tn];
		while(1)
		{
			cur=mesh->prim->face[i];
			glNormal3fv(mesh->curnorms+i*4);
			for(j=0; j<3; j++)
			{
				glNormal3fv(mesh->curvnorms+i*12+j*4);
				glTexCoord2fv(mesh->prim->stvecs+(2*cur->stvecs[j]));
				glVertex3fv(mesh->curvecs+(3*cur->vecs[j]));
			}

			i=cur->tnext;
			if(i<=0)break;
		}
		glEnd();
	}
}
#endif

LBXGL_API void Skel_DrawMeshStateGroup(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroup(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshState(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_DrawModel(LBXGL_Skel2State *ctx)
{
	Skel_SetupFrame(ctx);
	if(ctx->mesh)Skel_DrawMeshStateGroup(ctx, ctx->mesh);
		else Skel_DrawMeshGroup(ctx, ctx->mdl->mesh);
}


LBXGL_API void Skel_DrawMeshStateFlat(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshState *mesh)
{
	float im[16];
	LBXGL_Skel2MeshFace *cur;
	int i, j, k, l, tn;

	if(!mesh->prim->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh->prim, im);
		return;
	}

	glBegin(GL_TRIANGLES);
	for(i=0; i<mesh->prim->n_faces; i++)
	{
		cur=mesh->prim->face[i];
		glVertex3fv(mesh->curvecs+(3*cur->vecs[0]));
		glVertex3fv(mesh->curvecs+(3*cur->vecs[1]));
		glVertex3fv(mesh->curvecs+(3*cur->vecs[2]));
	}
	glEnd();
}

LBXGL_API void Skel_DrawMeshStateFlat2(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshState *mesh)
{
	LBXGL_Skel2MeshFace *cur;
	float im[16];
	int i, j, k, l, tn;

	if(!mesh->prim->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh->prim, im);
		return;
	}

	glBegin(GL_TRIANGLES);
	for(i=0; i<mesh->prim->n_faces; i++)
	{
		cur=mesh->prim->face[i];
		for(j=0; j<3; j++)
		{
			glNormal3fv(mesh->curvnorms+i*12+j*4);
			glVertex3fv(mesh->curvecs+(3*cur->vecs[j]));
		}
	}
	glEnd();
}

LBXGL_API void Skel_DrawMeshStateGroupFlat(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroupFlat(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshStateFlat(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_DrawMeshStateGroupFlat2(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroupFlat2(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshStateFlat2(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_DrawModelDark(LBXGL_Skel2State *ctx)
{
	if(!ctx->mesh)ctx->mesh=Skel_NewGroupInstance(ctx->mdl->mesh);
	Skel_SetupFrame(ctx);

	glDisable(GL_TEXTURE_2D);
	glColor4f(0, 0, 0, 1);

	Skel_DrawMeshStateGroupFlat(ctx, ctx->mesh);
}

LBXGL_API void Skel_DrawModelLight(LBXGL_Skel2State *ctx)
{
	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);
	Skel_DrawMeshStateGroupFlat2(ctx, ctx->mesh);
}

LBXGL_API void Skel_DrawModelFinal(LBXGL_Skel2State *ctx)
{
	glEnable(GL_TEXTURE_2D);
	glColor4f(1, 1, 1, 1);

	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	Skel_DrawMeshStateGroup(ctx, ctx->mesh);
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

LBXGL_API void Skel_DrawMeshStateShadow(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshState *mesh, float *org, float rad)
{
	static float *xyza=NULL;
	static float *xyzb=NULL;
	static int nxyz;
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	LBXGL_Skel2Mesh *prim;
	LBXGL_Skel2MeshFace *cur;
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1, p, n;

	prim=mesh->prim;

	if(!prim->edge)Skel_BuildMeshEdges(prim);

	if(!xyza)
	{
		nxyz=256;
		while(nxyz<=prim->n_vecs)nxyz+=nxyz>>1;
		xyza=malloc(nxyz*3*sizeof(float));
		xyzb=malloc(nxyz*3*sizeof(float));
	}

	if(nxyz<prim->n_vecs)
	{
		free(xyza); free(xyzb);
		while(nxyz<=prim->n_vecs)nxyz+=nxyz>>1;
		xyza=malloc(nxyz*3*sizeof(float));
		xyzb=malloc(nxyz*3*sizeof(float));
	}


	xyz=mesh->curvecs;
	for(i=0; i<prim->n_vecs; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SUB(tv0, org, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 1, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*rad, xyzb+i*3);
	}

	glBegin(GL_QUADS);
	for(i=0; i<prim->n_edge; i++)
	{
		i0=prim->edge[i*4+2]; i1=prim->edge[i*4+3];
		nv0=mesh->curnorms+i0*4;
		nv1=mesh->curnorms+i1*4;

		f=V3F_NDOT(org, nv0);
		g=V3F_NDOT(org, nv1);

		if((f>0) && (g<=0))
		{
			i0=prim->edge[i*4+0]; i1=prim->edge[i*4+1];
			glVertex3fv(xyza+i1*3); glVertex3fv(xyza+i0*3);
			glVertex3fv(xyzb+i0*3); glVertex3fv(xyzb+i1*3);
		}

		if((g>0) && (f<=0))
		{
			i0=prim->edge[i*4+0]; i1=prim->edge[i*4+1];
			glVertex3fv(xyza+i0*3); glVertex3fv(xyza+i1*3);
			glVertex3fv(xyzb+i1*3); glVertex3fv(xyzb+i0*3);
		}
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	for(i=0; i<prim->n_faces; i++)
	{
		nv=mesh->curnorms+i*4;
		f=V3F_NDOT(org, nv);
		if(f>0)xyz=xyza; else xyz=xyzb;

		cur=prim->face[i];
		glVertex3fv(xyz+(3*cur->vecs[0]));
		glVertex3fv(xyz+(3*cur->vecs[1]));
		glVertex3fv(xyz+(3*cur->vecs[2]));
	}
	glEnd();
}

LBXGL_API void Skel_DrawMeshStateGroupShadow(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp, float *org, float rad)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroupShadow(ctx, gcur, org, rad);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshStateShadow(ctx, mcur, org, rad);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_DrawModelShadow(LBXGL_Skel2State *ctx, float *org, float rad)
{
	Skel_DrawMeshStateGroupShadow(ctx, ctx->mesh, org, rad);
}
