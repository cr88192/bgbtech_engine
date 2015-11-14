#include <lbxgl2.h>

LBXGL_API int SkelUtil_CheckLineBox(float *m, float *n,
	float *p0, float *p1)
{
	float tv0[3], tv1[3];
	float n0[4], n1[4];
	int i;

	V3F_COPY(p0, tv0);
	V3F_COPY(p1, tv1);
	for(i=0; i<3; i++)
	{
		if((tv0[i]<=m[i]) && (tv1[i]<=m[i]))return(0);
		if((tv0[i]>=n[i]) && (tv1[i]>=n[i]))return(0);

		V3F_ZERO(n0); V3F_ZERO(n1);
		n0[i]=1; n1[i]=1;
		n0[3]=m[i]; n1[3]=n[i];

		if(tv0[i]<m[i])Vec3F_LinePlaneIntersect(tv0, tv1, n0, tv0);
		if(tv0[i]>n[i])Vec3F_LinePlaneIntersect(tv0, tv1, n1, tv0);
		if(tv1[i]<m[i])Vec3F_LinePlaneIntersect(tv0, tv1, n0, tv1);
		if(tv1[i]>n[i])Vec3F_LinePlaneIntersect(tv0, tv1, n1, tv1);
	}

	for(i=0; i<3; i++)
	{
		if((tv0[i]<=m[i]) && (tv1[i]<=m[i]))return(0);
		if((tv0[i]>=n[i]) && (tv1[i]>=n[i]))return(0);
	}

	return(1);
}

LBXGL_API int SkelUtil_CheckLineSolid(LBXGL_Skel2State *ctx, int num,
	float *org, float *dir)
{
	LBXGL_Skel2Solid *solid;
	float start[3], end[3];
	float tv[3], tv1[3];
	float *v0, *v1;
	float f, g, d;
	int i, j, k;

	solid=ctx->mdl->solid[num];
	i=solid->bone;

	V3F_SUB(org, ctx->bone_rorg+i*3, start);
	V3F_ADDSCALE(start, dir, 1000, end);

	v0=solid->m;
	v1=solid->n;
	switch(solid->type)
	{
	case BONE_SOLID_BBOX:
		i=SkelUtil_CheckLineBox(v0, v1, start, end);
		return(i);
		break;

	case BONE_SOLID_SPHERE:
		d=V3F_DOT(v0, dir)-V3F_DOT(start, dir);
		if(d<=0)return(0);
		V3F_ADDSCALE(start, dir, d, tv);
		d=V3F_DIST(tv, v0);
		if(d<=solid->r1)return(1);
		return(0);
		break;

	case BONE_SOLID_CAPSULE:
		V3F_ADDSCALE(start, dir, 1000, tv);

		d=Vec3F_NearestLineDistance(start, tv, v0, v1);
		if(d<=solid->r1)return(1);
		return(0);
		break;

	default:
		break;
	}
	return(0);
}

LBXGL_Skel2MeshFace *Skel_CloneMeshFace(LBXGL_Skel2MeshFace *face)
{
	LBXGL_Skel2MeshFace *tmp;
	int i;

	if(!face)return(NULL);

	tmp=gcalloc(sizeof(LBXGL_Skel2MeshFace));
	memset(tmp, 0, sizeof(LBXGL_Skel2MeshFace));
	
	tmp->tnext=face->tnext;
	tmp->vecs[0]=face->vecs[0];
	tmp->vecs[1]=face->vecs[1];
	tmp->vecs[2]=face->vecs[2];
	tmp->stvecs[0]=face->stvecs[0];
	tmp->stvecs[1]=face->stvecs[1];
	tmp->stvecs[2]=face->stvecs[2];

	tmp->norm[0]=face->norm[0];
	tmp->norm[1]=face->norm[1];
	tmp->norm[2]=face->norm[2];

	for(i=0; i<9; i++)
		tmp->nvecs[i]=face->nvecs[i];

	return(tmp);
}

LBXGL_Skel2Mesh *Skel_CloneMesh(LBXGL_Skel2Mesh *mesh)
{
	LBXGL_Skel2Mesh *tmp;
	LBXGL_Skel2MeshFace *ftmp, *flst;
	int i;

	if(!mesh)return(NULL);

	tmp=AC3D_CreateMesh();

	tmp->name=mesh->name;

	tmp->n_tex=mesh->n_tex;
	for(i=0; i<mesh->n_tex; i++)
	{
		tmp->texname[i]=mesh->texname[i];
		tmp->texnum[i]=mesh->texnum[i];
		tmp->texface[i]=mesh->texface[i];
	}
	
	tmp->n_faces=mesh->n_faces;
	tmp->face=gcalloc(mesh->n_faces*sizeof(LBXGL_Skel2MeshFace *));
	for(i=0; i<mesh->n_faces; i++)
	{
		ftmp=Skel_CloneMeshFace(mesh->face[i]);
		tmp->face[i]=ftmp;
		if(i)	{ flst->next=ftmp; flst=ftmp; }
		else	{ tmp->faces=ftmp; flst=ftmp; }
	}

	tmp->n_vecs=mesh->n_vecs;
	tmp->vecs=gcalloc(mesh->n_vecs*3*sizeof(float));
	for(i=0; i<(mesh->n_vecs*3); i++)
		tmp->vecs[i]=mesh->vecs[i];

	tmp->n_stvecs=mesh->n_stvecs;
	tmp->stvecs=gcalloc(mesh->n_stvecs*3*sizeof(float));
	for(i=0; i<(mesh->n_stvecs*3); i++)
		tmp->stvecs[i]=mesh->stvecs[i];

	Skel_RecalcMeshVFIdx(tmp);

	return(tmp);
}

LBXGL_Skel2MeshGroup *Skel_CloneMeshGroup(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	LBXGL_Skel2MeshGroup *gtmp, *gfst, *glst;
	LBXGL_Skel2Mesh *mtmp, *mfst, *mlst;

	if(!grp)return(NULL);

	gcur=grp->first; gfst=NULL; glst=NULL;
	while(gcur)
	{
		gtmp=Skel_CloneMeshGroup(gcur);
		if(glst) { glst->next=gtmp; glst=gtmp; }
			else { gfst=gtmp; glst=gtmp; }
		gcur=gcur->next;
	}

	mcur=grp->prim; mfst=NULL; mlst=NULL;
	while(mcur)
	{
		mtmp=Skel_CloneMesh(mcur);
		if(mlst) { mlst->next=mtmp; mlst=mtmp; }
			else { mfst=mtmp; mlst=mtmp; }
		mcur=mcur->next;
	}

	gtmp=AC3D_CreateMeshGroup();

	gtmp->first=gfst;
	gtmp->prim=mfst;
	if(grp->name)gtmp->name=dystring(grp->name);

	return(gtmp);
}

void Skel_RecalcMeshVFIdx(LBXGL_Skel2Mesh *prim)
{
	int tidx[65536];
	LBXGL_Skel2MeshFace *fcur;
	int i, j, k, n, n0;

	if(!prim)return;

	if(!prim->vfidx)prim->vfidx=gcalloc(prim->n_vecs*sizeof(int));
	if(!prim->vfnum)prim->vfnum=gcalloc(prim->n_vecs*sizeof(int));

	n=0;
	for(i=0; i<prim->n_vecs; i++)
	{
		n0=n;

		for(j=0; j<prim->n_faces; j++)
		{
			fcur=prim->face[j];

//			for(k=0; k<fcur->n_vecs; k++)
			for(k=0; k<3; k++)
				if(fcur->vecs[k]==i)
					break;
//			if(k<fcur->n_vecs)tidx[n++]=j;
			if(k<3)tidx[n++]=j;

			if(n>=65536)
				*(int *)-1=-1;
		}

		prim->vfidx[i]=n0;
		prim->vfnum[i]=n-n0;
	}

	if(prim->vfids)gcfree(prim->vfids);
	prim->vfids=gcalloc(n*sizeof(int));
	for(i=0; i<n; i++)prim->vfids[i]=tidx[i];
}

void Skel_RecalcGroupVFIdx(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_RecalcGroupVFIdx(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_RecalcMeshVFIdx(mcur);
		mcur=mcur->next;
	}
}

int skel_stat_xyz, skel_stat_st, skel_stat_face;

void Skel_RecalcMeshFaceNorm(LBXGL_Skel2Mesh *mesh,
	LBXGL_Skel2MeshFace *face)
{
	float tv0[3], tv1[3], tv2[3];
	float *v0, *v1, *v2;
	int i;

//	tmp=gcalloc(sizeof(LBXGL_Skel2MeshFace));
//	memset(tmp, 0, sizeof(LBXGL_Skel2MeshFace));
	
	v0=mesh->vecs+face->vecs[0]*3;
	v1=mesh->vecs+face->vecs[1]*3;
	v2=mesh->vecs+face->vecs[2]*3;

	V3F_SUB(v1, v0, tv0);
	V3F_SUB(v2, v1, tv1);
	V3F_CROSS(tv0, tv1, tv2);
	V3F_NORMALIZE(tv2, face->norm);

//	for(i=0; i<9; i++)
//		tmp->nvecs[i]=face->nvecs[i];

//	return(tmp);
}

void Skel_RecalcMeshFaceVNorm(LBXGL_Skel2Mesh *mesh,
	LBXGL_Skel2MeshFace *face)
{
	float tv0[3], tv1[3], tv2[3];
	LBXGL_Skel2MeshFace *face2;
	float *v0, *v1, *v2;
	float f, g;
	int i, j, k, l;

#if 0
	for(i=0; i<3; i++)
	{
		V3F_ZERO(tv0);
		for(j=0; j<mesh->n_face; j++)
			if(	(mesh->face[j]->vecs[0]==face->vecs[i]) ||
				(mesh->face[j]->vecs[1]==face->vecs[i]) ||
				(mesh->face[j]->vecs[2]==face->vecs[i]))
		{
//			if(mesh->face[j]==face)continue;
			v0=mesh->face[j]->norm;
			f=V3F_DOT(v0, face->norm);

			if(f>0)
			{
				if(f>1)f=1;
				V3F_ADDSCALE(tv0, v0, f, tv0);
			}
		}
		V3F_NORMALIZE(tv0, face->nvecs+i*3);
	}
#endif

#if 1
	for(i=0; i<3; i++)
	{
		V3F_ZERO(face->nvecs+i*3);

		j=face->vecs[i];
		for(k=0; k<mesh->vfnum[j]; k++)
		{
			l=mesh->vfids[mesh->vfidx[j]+k];
			face2=mesh->face[l];

			f=V3F_DOT(face->norm, face2->norm);
			if(f>=0.75)
			{
				V3F_ADDSCALE(face->nvecs+i*3,
					face2->norm, f,
					face->nvecs+i*3);
			}
		}

		V3F_NORMALIZE(face->nvecs+i*3, face->nvecs+i*3);
	}
#endif
}

void Skel_RecalcMeshNorm(LBXGL_Skel2Mesh *mesh)
{
	int i;
	for(i=0; i<mesh->n_faces; i++)
		Skel_RecalcMeshFaceNorm(mesh, mesh->face[i]);

//	ri.Con_Printf(PRINT_DEVELOPER, "RecalcMeshNorm: %s: "
//			"%d xyz, %d st, %d face\n",
//		mesh->name,
//		mesh->n_vecs, mesh->n_stvecs, mesh->n_faces);
	skel_stat_xyz+=mesh->n_vecs;
	skel_stat_st+=mesh->n_stvecs;
	skel_stat_face+=mesh->n_faces;
}

void Skel_RecalcMeshVNorm(LBXGL_Skel2Mesh *mesh)
{
	int i;
	for(i=0; i<mesh->n_faces; i++)
		Skel_RecalcMeshFaceVNorm(mesh, mesh->face[i]);
}

void Skel_RecalcGroupNorm(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_RecalcGroupNorm(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_RecalcMeshNorm(mcur);
		mcur=mcur->next;
	}
}

void Skel_RecalcGroupVNorm(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_RecalcGroupVNorm(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_RecalcMeshVNorm(mcur);
		mcur=mcur->next;
	}
}

//calculate average normal for a given vertex
void Skel_CalcMeshVecANorm(LBXGL_Skel2Mesh *mesh, int vec, float *norm)
{
	float tv[3];
	LBXGL_Skel2MeshFace *face;
	float *v0, *v1, *v2;
	float f, g;
	int i, j, k;

	V3F_ZERO(tv);
	for(i=0; i<mesh->vfnum[vec]; i++)
	{
		j=mesh->vfids[mesh->vfidx[vec]+i];
		face=mesh->face[j];
		V3F_ADD(tv, face->norm, tv);
	}
	
	V3F_NORMALIZE(tv, norm);
}

//calculate average value:
//IOW, the average of how similar the average normal is
//to each face normal
float Skel_CalcMeshVecAVal(LBXGL_Skel2Mesh *mesh, int vec)
{
	float tv[3];
	LBXGL_Skel2MeshFace *face;
	float *v0, *v1, *v2;
	float f, g;
	int i, j, k, n;

	n=mesh->vfnum[vec];
	if(n<=0)return(-1);
	
	V3F_ZERO(tv);
	for(i=0; i<n; i++)
	{
		j=mesh->vfids[mesh->vfidx[vec]+i];
		face=mesh->face[j];
		V3F_ADD(tv, face->norm, tv);
	}
	V3F_NORMALIZE(tv, tv);

	f=0;
	for(i=0; i<n; i++)
	{
		j=mesh->vfids[mesh->vfidx[vec]+i];
		face=mesh->face[j];
		f+=V3F_DOT(tv, face->norm);
	}

	g=f/n;
	return(g);
}

//most "similar" adjacent vector
int Skel_CalcMeshVecAVecI(LBXGL_Skel2Mesh *mesh, int vec, int nl)
{
	float tv[3];
	LBXGL_Skel2MeshFace *face;
	float *v0, *v1, *v2;
	float f, g, bd;
	int i, j, k, n, bi;

	n=mesh->vfnum[vec];
	if(n<=0)return(-1);

	v0=mesh->vecs+vec*3;

	bd=999999999.0; bi=-1;
	for(i=0; i<n; i++)
	{
		j=mesh->vfids[mesh->vfidx[vec]+i];
		face=mesh->face[j];
		for(j=0; j<3; j++)
		{
			k=face->vecs[j];
			if(k==vec)continue;
			if(nl && (k<vec))continue;
			v1=mesh->vecs+k*3;
			f=V3F_DIST(v0, v1);
			if(f<bd) { bi=k; bd=f; }
		}
	}
	return(bi);
}

int Skel_CalcMeshVecAVec(LBXGL_Skel2Mesh *mesh, int vec)
	{ return(Skel_CalcMeshVecAVecI(mesh, vec, 0)); }

int Skel_CalcMeshVecAVecNl(LBXGL_Skel2Mesh *mesh, int vec)
	{ return(Skel_CalcMeshVecAVecI(mesh, vec, 1)); }

LBXGL_Skel2Mesh *skel_tmpst_mesh[65536];
LBXGL_Skel2Mesh *skel_tmpxyz_mesh[65536];
float skel_tmpst[65536*2];
float skel_tmpxyz[65536*3];
int skel_ntmpst;
int skel_ntmpxyz;

int Skel_TempIndexST(LBXGL_Skel2Mesh *prim, float *st)
{
	int i;

	for(i=0; i<prim->n_stvecs; i++)
		if((prim->stvecs[i*2+0]==st[0]) && (prim->stvecs[i*2+1]==st[1]))
			return(i);

	for(i=0; i<skel_ntmpst; i++)
		if((skel_tmpst_mesh[i]==prim) &&
			(skel_tmpst[i*2+0]==st[0]) && (skel_tmpst[i*2+1]==st[1]))
				return(-(i+1));
	
	i=skel_ntmpst++;
	skel_tmpst_mesh[i]=prim;
	skel_tmpst[i*2+0]=st[0];
	skel_tmpst[i*2+1]=st[1];
	return(-(i+1));
}

int Skel_TempIndexXYZ(LBXGL_Skel2Mesh *prim, float *xyz)
{
	int i;

	for(i=0; i<prim->n_vecs; i++)
		if((prim->vecs[i*3+0]==xyz[0]) && (prim->vecs[i*3+1]==xyz[1]) &&
			(prim->vecs[i*3+2]==xyz[2]))
				return(i);

	for(i=0; i<skel_ntmpxyz; i++)
		if((skel_tmpxyz_mesh[i]==prim) &&
			(skel_tmpxyz[i*3+0]==xyz[0]) && (skel_tmpxyz[i*3+1]==xyz[1]) &&
			(skel_tmpxyz[i*3+2]==xyz[2]))
				return(-(i+1));
	
	i=skel_ntmpxyz++;
	skel_tmpxyz_mesh[i]=prim;
	skel_tmpxyz[i*3+0]=xyz[0];
	skel_tmpxyz[i*3+1]=xyz[1];
	skel_tmpxyz[i*3+2]=xyz[2];
	return(-(i+1));
}

int Skel_IndexTempST(LBXGL_Skel2Mesh *prim, float *st)
{
	int i;

	for(i=0; i<skel_ntmpst; i++)
		if((skel_tmpst_mesh[i]==prim) &&
			(skel_tmpst[i*2+0]==st[0]) && (skel_tmpst[i*2+1]==st[1]))
			return(i);
	i=skel_ntmpst++;
	skel_tmpst_mesh[i]=prim;
	skel_tmpst[i*2+0]=st[0];
	skel_tmpst[i*2+1]=st[1];
	return(i);
}

int Skel_IndexTempXYZ(LBXGL_Skel2Mesh *prim, float *xyz)
{
	int i;

	for(i=0; i<skel_ntmpxyz; i++)
		if((skel_tmpxyz_mesh[i]==prim) &&
			(skel_tmpxyz[i*3+0]==xyz[0]) && (skel_tmpxyz[i*3+1]==xyz[1]) &&
			(skel_tmpxyz[i*3+2]==xyz[2]))
				return(i);
	
	i=skel_ntmpxyz++;
	skel_tmpxyz_mesh[i]=prim;
	skel_tmpxyz[i*3+0]=xyz[0];
	skel_tmpxyz[i*3+1]=xyz[1];
	skel_tmpxyz[i*3+2]=xyz[2];
	return(i);
}

float *Skel_GetTempXYZ(LBXGL_Skel2Mesh *mesh, int vec)
{
	if(vec<0)return(skel_tmpxyz+((-vec)-1)*3);
	return(mesh->vecs+vec*3);
}

float *Skel_GetTempST(LBXGL_Skel2Mesh *mesh, int vec)
{
	if(vec<0)return(skel_tmpst+((-vec)-1)*2);
	return(mesh->stvecs+vec*2);
}

void Skel_MeshMergeVecs_I(LBXGL_Skel2Mesh *mesh, int dv, int sv)
{
	float tv[3], ddv[3], dsv[3];
	float dv0[3], dv1[3];
	float dst0[2], dst1[2], dst2[2];
	float *v0, *v1, *v2, *st0, *st1, *st2, *vdv, *vsv;
	float d0, d1, f, g;
	int dtv;
	int i, j, k;
	
	vdv=Skel_GetTempXYZ(mesh, dv);
	vsv=Skel_GetTempXYZ(mesh, sv);

	tv[0]=(vdv[0]+vsv[0])*0.5;
	tv[1]=(vdv[1]+vsv[1])*0.5;
	tv[2]=(vdv[2]+vsv[2])*0.5;
	V3F_SUB(tv, vdv, ddv);
	V3F_SUB(tv, vsv, dsv);
//	dtv=Skel_TempIndexXYZ(mesh, tv);
	dtv=dv;
	
	for(i=0; i<mesh->n_faces; i++)
		for(j=0; j<3; j++)
	{
		if(	(mesh->face[i]->vecs[j]==dv) ||
			(mesh->face[i]->vecs[j]==sv))
		{
			v0=Skel_GetTempXYZ(mesh, mesh->face[i]->vecs[j]);
			v1=Skel_GetTempXYZ(mesh, mesh->face[i]->vecs[(j+1)%3]);
			v2=Skel_GetTempXYZ(mesh, mesh->face[i]->vecs[(j+2)%3]);

			st0=Skel_GetTempST(mesh, mesh->face[i]->stvecs[j]);
			st1=Skel_GetTempST(mesh, mesh->face[i]->stvecs[(j+1)%3]);
			st2=Skel_GetTempST(mesh, mesh->face[i]->stvecs[(j+2)%3]);

			V3F_SUB(v0, v1, dv0);
			V3F_SUB(v0, v2, dv1);
			V2F_SUB(st0, st1, dst0);
			V2F_SUB(st0, st2, dst1);
			
			d0=V3F_NORMALIZE(dv0, dv0);
			d1=V3F_NORMALIZE(dv1, dv1);
			f=(d0==0)?0:(1.0/d0);
			g=(d1==0)?0:(1.0/d1);
			V2F_SCALE(dst0, f, dst0);
			V2F_SCALE(dst1, g, dst1);

			f=V3F_DOT(ddv, dv0);
			g=V3F_DOT(ddv, dv1);
			V2F_ADDSCALE(st0, dst0, f, dst2);
			V2F_ADDSCALE(dst2, dst1, g, dst2);

			k=Skel_TempIndexST(mesh, dst2);
			mesh->face[i]->stvecs[j]=k;
			mesh->face[i]->vecs[j]=dtv;
		}
	}
	
	V3F_COPY(tv, vdv);
}


float skel_vtmp_aval[65536];
int skel_vtmp_idx[65536];
int skel_vtmp_idx1[65536];
LBXGL_Skel2Mesh *skel_vtmp_mesh[65536];
int skel_vtmp_num;

void SkelLod_MeshAddVecs(LBXGL_Skel2Mesh *mesh)
{
	float f;
	int i, j, k;

	for(i=0; i<mesh->n_vecs; i++)
	{
		j=skel_vtmp_num;
		skel_vtmp_mesh[j]=mesh;
		skel_vtmp_idx[j]=i;
		skel_vtmp_aval[j]=
			Skel_CalcMeshVecAVal(mesh, i);
		k=Skel_CalcMeshVecAVecNl(mesh, i);
		if(k<0)continue;
		skel_vtmp_idx1[j]=k;

#if 0
		//see if other vec is better
		f=Skel_CalcMeshVecAVal(mesh, k);
		if(f>skel_vtmp_aval[j])
		{
			skel_vtmp_idx[j]=k;
			skel_vtmp_idx1[j]=i;
			skel_vtmp_aval[j]=f;
		}
#endif

		skel_vtmp_num++;
	}
}

void SkelLod_GroupAddVecs(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		SkelLod_GroupAddVecs(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		SkelLod_MeshAddVecs(mcur);
		mcur=mcur->next;
	}
}

void SkelLod_GroupSwapVecs(int i, int j)
{
	LBXGL_Skel2Mesh *mtmp;
	float f;
	int k;

	mtmp=skel_vtmp_mesh[i];
	skel_vtmp_mesh[i]=skel_vtmp_mesh[j];
	skel_vtmp_mesh[j]=mtmp;
		
	k=skel_vtmp_idx[i];
	skel_vtmp_idx[i]=skel_vtmp_idx[j];
	skel_vtmp_idx[j]=k;

	f=skel_vtmp_aval[i];
	skel_vtmp_aval[i]=skel_vtmp_aval[j];
	skel_vtmp_aval[j]=f;

	k=skel_vtmp_idx1[i];
	skel_vtmp_idx1[i]=skel_vtmp_idx1[j];
	skel_vtmp_idx1[j]=k;
}

void SkelLod_GroupRankVecs(LBXGL_Skel2MeshGroup *grp)
{
	int i, j, k;

	skel_vtmp_num=0;
	SkelLod_GroupAddVecs(grp);
	
	for(i=0; i<skel_vtmp_num; i++)
		for(j=i+1; j<skel_vtmp_num; j++)
			if(skel_vtmp_aval[j]>skel_vtmp_aval[i])
				SkelLod_GroupSwapVecs(i, j);
}

LBXGL_API int AC3D_IndexST(LBXGL_Skel2Mesh *prim, float s, float t);

LBXGL_API int AC3D_IndexXYZ(LBXGL_Skel2Mesh *prim,
	float x, float y, float z);

extern float skel_a_xyz[65536*4];
extern float skel_a_norm[65536*4];
extern float skel_a_st[65536*2];
extern float skel_a_rgba[65536*4];

void SkelLod_MeshReIndex(LBXGL_Skel2Mesh *mesh)
{
	LBXGL_Skel2MeshFace *tface[16384];
	LBXGL_Skel2MeshFace *face;
	float *oldst, *oldxyz, *tv, *tst;
	int nf, onxyz, onst;
	int i, j, k;

	oldst=mesh->stvecs;
	oldxyz=mesh->vecs;
	onst=mesh->n_stvecs;
	onxyz=mesh->n_vecs;
	
	mesh->stvecs=skel_a_st;
	mesh->vecs=skel_a_xyz;
	mesh->n_stvecs=0;
	mesh->n_vecs=0;

	nf=0;
	for(i=0; i<mesh->n_faces; i++)
	{
		face=mesh->face[i];
		if((face->vecs[0]==face->vecs[1]) ||
			(face->vecs[1]==face->vecs[2]) ||
			(face->vecs[2]==face->vecs[0]))
				continue;
		tface[nf++]=face;
		face->tnext=nf;
		
		for(j=0; j<3; j++)
		{
			if(face->vecs[j]<0)
				tv=skel_tmpxyz+((-face->vecs[j])-1)*3;
			else tv=oldxyz+face->vecs[j]*3;

			if(face->stvecs[j]<0)
				tst=skel_tmpst+((-face->stvecs[j])-1)*2;
			else tst=oldst+face->stvecs[j]*2;

			k=AC3D_IndexXYZ(mesh, tv[0], tv[1], tv[2]);
			face->vecs[j]=k;
			k=AC3D_IndexST(mesh, tst[0], tst[1]);
			face->stvecs[j]=k;
		}
	}

	if(nf>0)tface[nf-1]->tnext=-1;
	mesh->texface[0]=0;

	for(i=0; i<nf; i++)
		mesh->face[i]=tface[i];
	mesh->n_faces=nf;
	
	if(mesh->n_vecs>onxyz)
		oldxyz=gcalloc(mesh->n_vecs*3*sizeof(float));
	if(mesh->n_stvecs>onst)
		oldst=gcalloc(mesh->n_stvecs*2*sizeof(float));
	
	for(i=0; i<(mesh->n_vecs*3); i++)
		oldxyz[i]=mesh->vecs[i];
	for(i=0; i<(mesh->n_stvecs*2); i++)
		oldst[i]=mesh->stvecs[i];

	mesh->stvecs=oldst;
	mesh->vecs=oldxyz;
	
	mesh->edge=NULL;
}

void SkelLod_GroupReIndex(LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	gcur=grp->first;
	while(gcur)
	{
		SkelLod_GroupReIndex(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		SkelLod_MeshReIndex(mcur);
		mcur=mcur->next;
	}
}


int SkelLod_GroupMergeVecsFrac(LBXGL_Skel2MeshGroup *grp, int frac)
{
	int i, ne;

	if(frac>50)
	{
		SkelLod_GroupMergeVecsFrac(grp, 50);
		SkelLod_GroupMergeVecsFrac(grp, frac-50);
		return(0);
	}

	skel_stat_xyz=0;
	skel_stat_st=0;
	skel_stat_face=0;

	Skel_RecalcGroupNorm(grp);
	SkelLod_GroupRankVecs(grp);	

	ri.Con_Printf(PRINT_DEVELOPER, "GroupMergeVecsFrac 1: "
			"%d xyz, %d st, %d face\n",
		skel_stat_xyz, skel_stat_st, skel_stat_face);

	if(skel_vtmp_num<25)return(-1);

	ne=skel_vtmp_num*(frac/100.0);
	
	skel_ntmpst=0;
	skel_ntmpxyz=0;
	
	for(i=0; i<ne; i++)
	{
		Skel_MeshMergeVecs_I(skel_vtmp_mesh[i],
			skel_vtmp_idx1[i], skel_vtmp_idx[i]);
	}

	skel_stat_xyz=0;
	skel_stat_st=0;
	skel_stat_face=0;
	
	SkelLod_GroupReIndex(grp);
	Skel_RecalcGroupNorm(grp);
	Skel_RecalcGroupVFIdx(grp);
	Skel_RecalcGroupVNorm(grp);
	Skel_BuildGroupEdges(grp);

	ri.Con_Printf(PRINT_DEVELOPER, "GroupMergeVecsFrac 2: "
			"%d xyz, %d st, %d face\n",
		skel_stat_xyz, skel_stat_st, skel_stat_face);

	return(0);
}

int SkelLod_GroupMergeVecs(LBXGL_Skel2MeshGroup *grp)
{
	SkelLod_GroupMergeVecsFrac(grp, 33);
//	SkelLod_GroupMergeVecsFrac(grp, 18);
//	SkelLod_GroupMergeVecsFrac(grp, 18);

	return(0);
}
