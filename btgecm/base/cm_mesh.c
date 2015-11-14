#include <btgecm.h>

BTGE_API BTGE_MeshGroup *BTGE_Mesh_LoadModel(char *name)
{
	BTGE_MeshGroup *tmp;
	char *s;

	s=vfgetext(name);

	if(!strcmp(s, "ac"))
	{
		tmp=BTGE_AC3D_LoadModel(name);
		return(tmp);
	}
//	if(!strcmp(s, "acx"))
//	{
//		tmp=BTGE_ACXML_LoadModel(name);
//		return(tmp);
//	}

	return(NULL);
}

BTGE_API int BTGE_Mesh_SaveModel(char *name, BTGE_MeshGroup *grp)
{
	char *s;
	int i;

	s=vfgetext(name);

	if(!strcmp(s, "ac"))
	{
		i=BTGE_AC3D_SaveModel(grp, name);
		return(i);
	}
//	if(!strcmp(s, "acx"))
//	{
//		i=BTGE_ACXML_SaveModel(grp, name);
//		return(i);
//	}
	return(-1);
}


BTGE_API void BTGE_Mesh_DestroyPrim(BTGE_MeshPrim *prim)
{
	if(prim->xyz)gcfree(prim->xyz);
	if(prim->st)gcfree(prim->st);
	if(prim->pts)gcfree(prim->pts);
	if(prim->face)gcfree(prim->face);

	if(prim->fl_xyz)gcfree(prim->fl_xyz);
	if(prim->fl_st)gcfree(prim->fl_st);
	if(prim->fl_pts)gcfree(prim->fl_pts);
	if(prim->fl_face)gcfree(prim->fl_face);

	gcfree(prim);
}

BTGE_API void BTGE_Mesh_DestroyGroup(BTGE_MeshGroup *grp)
{
	BTGE_MeshGroup *gcur, *gnxt;
	BTGE_MeshPrim *mcur, *mnxt;

	gcur=grp->first;
	while(gcur)
	{
		gnxt=gcur->next;
		BTGE_Mesh_DestroyGroup(gcur);
		gcur=gnxt;
	}

	mcur=grp->prim;
	while(mcur)
	{
		mnxt=mcur->next;
		BTGE_Mesh_DestroyPrim(mcur);
		mcur=mnxt;
	}
}

BTGE_API BTGE_MeshPrim *BTGE_Mesh_LookupPrimGroup(
	BTGE_MeshGroup *grp, char *name)
{
	BTGE_MeshGroup *gcur, *gnxt;
	BTGE_MeshPrim *mcur, *mnxt, *mtmp;

	gcur=grp->first;
	while(gcur)
	{
		gnxt=gcur->next;
		mtmp=BTGE_Mesh_LookupPrimGroup(gcur, name);
		if(mtmp)return(mtmp);
		gcur=gnxt;
	}

	mcur=grp->prim;
	while(mcur)
	{
		mnxt=mcur->next;
		if(mcur->name && !strcmp(mcur->name, name))
			return(mcur);
//		BTGE_Mesh_DestroyPrim(mcur);
		mcur=mnxt;
	}
	
	return(NULL);
}

BTGE_API BTGE_MeshPrim *BTGE_Mesh_ClonePrim(BTGE_MeshPrim *prim)
{
	BTGE_MeshPrim *tmp;

	tmp=gctalloc("btge_meshprim_t", sizeof(BTGE_MeshPrim));

	if(prim->name)tmp->name=dystrdup(prim->name);
	if(prim->tex)tmp->tex=dystrdup(prim->tex);
	tmp->texnum=prim->texnum;

	tmp->n_xyz=prim->n_xyz;		tmp->m_xyz=prim->m_xyz;
	tmp->n_st=prim->n_st;		tmp->m_st=prim->m_st;
	tmp->n_pts=prim->n_pts;		tmp->m_pts=prim->m_pts;
	tmp->n_face=prim->n_face;	tmp->m_face=prim->m_face;

	tmp->xyz=gcatomic(tmp->m_xyz*3*sizeof(float));
	tmp->st=gcatomic(tmp->m_st*2*sizeof(float));
	tmp->pts=gcatomic(tmp->m_pts*2*sizeof(int));
	tmp->face=gcatomic(tmp->m_face*2*sizeof(int));

	memcpy(tmp->xyz, prim->xyz, tmp->m_xyz*3*sizeof(float));
	memcpy(tmp->st, prim->st, tmp->m_st*2*sizeof(float));
	memcpy(tmp->pts, prim->pts, tmp->m_pts*2*sizeof(int));
	memcpy(tmp->face, prim->face, tmp->m_face*2*sizeof(int));

	tmp->fl_xyz=gcatomic(tmp->m_xyz);
	tmp->fl_st=gcatomic(tmp->m_st);
	tmp->fl_pts=gcatomic(tmp->m_pts);
	tmp->fl_face=gcatomic(tmp->m_face);

	memcpy(tmp->fl_xyz, prim->fl_xyz, tmp->m_xyz);
	memcpy(tmp->fl_st, prim->fl_st, tmp->m_st);
	memcpy(tmp->fl_pts, prim->fl_pts, tmp->m_pts);
	memcpy(tmp->fl_face, prim->fl_face, tmp->m_face);

	tmp->fl=prim->fl;

	V3F_COPY(prim->tex_org, tmp->tex_org);
	V3F_COPY(prim->tex_sdir, tmp->tex_sdir);
	V3F_COPY(prim->tex_tdir, tmp->tex_tdir);
	tmp->tex_mode=prim->tex_mode;
	tmp->tex_fl=prim->tex_fl;

	return(tmp);
}

BTGE_API BTGE_MeshGroup *BTGE_Mesh_CloneGroup(BTGE_MeshGroup *grp)
{
	BTGE_MeshGroup *gcur, *gtmp, *gfst, *glst;
	BTGE_MeshPrim *mcur, *mtmp, *mfst, *mlst;

	gcur=grp->first; gfst=NULL; glst=NULL;
	while(gcur)
	{
		gtmp=BTGE_Mesh_CloneGroup(gcur);
		if(glst) { glst->next=gtmp; glst=gtmp; }
			else { gfst=gtmp; glst=gtmp; }
		gcur=gcur->next;
	}

	mcur=grp->prim; mfst=NULL; mlst=NULL;
	while(mcur)
	{
		mtmp=BTGE_Mesh_ClonePrim(mcur);
		if(mlst) { mlst->next=mtmp; mlst=mtmp; }
			else { mfst=mtmp; mlst=mtmp; }
		mcur=mcur->next;
	}

	gtmp=gctalloc("btge_meshgroup_t", sizeof(BTGE_MeshGroup));

	gtmp->first=gfst;
	gtmp->prim=mfst;
	if(grp->name)gtmp->name=dystrdup(grp->name);
	gtmp->fl=grp->fl;

	return(gtmp);
}

BTGE_API BTGE_MeshPrim *BTGE_Mesh_CreatePrim(BTGE_MeshGroup *up, char *name)
{
	BTGE_MeshPrim *prim;

	prim=gctalloc("btge_meshprim_t", sizeof(BTGE_MeshPrim));
	if(name)prim->name=dystrdup(name);

	prim->m_xyz=16;
	prim->m_st=64;
	prim->m_pts=64;
	prim->m_face=16;

	prim->xyz=gcatomic(prim->m_xyz*3*sizeof(float));
	prim->st=gcatomic(prim->m_st*2*sizeof(float));
	prim->pts=gcatomic(prim->m_pts*2*sizeof(int));
	prim->face=gcatomic(prim->m_face*2*sizeof(int));

	prim->fl_xyz=gcatomic(prim->m_xyz);
	prim->fl_st=gcatomic(prim->m_st);
	prim->fl_pts=gcatomic(prim->m_pts);
	prim->fl_face=gcatomic(prim->m_face);

	if(up)
	{
		prim->next=up->prim;
		up->prim=prim;
	}

	return(prim);
}

BTGE_API BTGE_MeshGroup *BTGE_Mesh_CreateGroup(BTGE_MeshGroup *up, char *name)
{
	BTGE_MeshGroup *tmp;

	tmp=gctalloc("btge_meshgroup_t", sizeof(BTGE_MeshGroup));
	if(name)tmp->name=dystrdup(name);

	if(up)
	{
		tmp->next=up->first;
		up->first=tmp;
	}

	return(tmp);
}

BTGE_API void BTGE_Mesh_CheckExpandXYZ(BTGE_MeshPrim *prim)
{
	float *xyz;
	byte *flb;
	int i;

	if(prim->n_xyz>=prim->m_xyz)
	{
		i=prim->m_xyz;
		while(prim->n_xyz>=i)i=i+(i>>1);
		xyz=gcatomic(i*3*sizeof(float));
		flb=gcatomic(i);
		memcpy(xyz, prim->xyz, prim->m_xyz*3*sizeof(float));
		memcpy(flb, prim->fl_xyz, prim->m_xyz);
		gcfree(prim->xyz);
		gcfree(prim->fl_xyz);
		prim->xyz=xyz;
		prim->fl_xyz=flb;
		prim->m_xyz=i;
	}
}

BTGE_API void BTGE_Mesh_CheckExpandST(BTGE_MeshPrim *prim)
{
	float *ptb;
	byte *flb;
	int i;

	if(prim->n_st>=prim->m_st)
	{
		i=prim->m_st;
		while(prim->n_st>=i)i=i+(i>>1);
		ptb=gcatomic(i*2*sizeof(float));
		flb=gcatomic(i);
		memcpy(ptb, prim->st, prim->m_st*2*sizeof(float));
		memcpy(flb, prim->fl_st, prim->m_st);
		gcfree(prim->st);
		gcfree(prim->fl_st);
		prim->st=ptb;
		prim->fl_st=flb;
		prim->m_st=i;
	}
}

BTGE_API void BTGE_Mesh_CheckExpandPTS(BTGE_MeshPrim *prim, int amt)
{
	int *ptb;
	byte *flb;
	int i;

	if((prim->n_pts+amt)>=prim->m_pts)
	{
		i=prim->m_pts;
		while((prim->n_pts+amt)>=i)i=i+(i>>1);
		ptb=gcatomic(i*2*sizeof(int));
		flb=gcatomic(i);
		memcpy(ptb, prim->pts, prim->m_pts*2*sizeof(int));
		memcpy(flb, prim->fl_pts, prim->m_pts);
		gcfree(prim->pts);
		gcfree(prim->fl_pts);
		prim->pts=ptb;
		prim->fl_pts=flb;
		prim->m_pts=i;
	}
}

BTGE_API void BTGE_Mesh_CheckExpandFace(BTGE_MeshPrim *prim)
{
	int *ptb;
	byte *flb;
	int i;

	if(prim->n_face>=prim->m_face)
	{
		i=prim->m_face;
		while(prim->n_face>=i)i=i+(i>>1);
		ptb=gcatomic(i*2*sizeof(int));
		flb=gcatomic(i);
		memcpy(ptb, prim->face, prim->m_face*2*sizeof(int));
		memcpy(flb, prim->fl_face, prim->m_face);
		gcfree(prim->face);
		gcfree(prim->fl_face);
		prim->face=ptb;
		prim->fl_face=flb;
		prim->m_face=i;
	}
}

BTGE_API int BTGE_Mesh_AllocVertex(BTGE_MeshPrim *prim)
{
	int i;

	BTGE_Mesh_FlushCache(prim);

	for(i=0; i<prim->n_xyz; i++)
		if(prim->fl_xyz[i]&128)
	{
		prim->fl_xyz[i]=0;
		return(i);
	}

	BTGE_Mesh_CheckExpandXYZ(prim);
	i=prim->n_xyz++;
	prim->fl_xyz[i]=0;
	return(i);
}

BTGE_API int BTGE_Mesh_AllocST(BTGE_MeshPrim *prim)
{
	int i;

	BTGE_Mesh_FlushCache(prim);

	for(i=0; i<prim->n_st; i++)
		if(prim->fl_st[i]&128)
	{
		prim->fl_st[i]=0;
		return(i);
	}

	BTGE_Mesh_CheckExpandST(prim);
	i=prim->n_st++;
	prim->fl_st[i]=0;
	return(i);
}

BTGE_API int BTGE_Mesh_AllocPTS(BTGE_MeshPrim *prim, int num)
{
	int i, j;

	BTGE_Mesh_FlushCache(prim);

	for(i=0; i<prim->n_pts; i++)
	{
		for(j=0; ((j<num) && (i+j)<prim->n_pts); j++)
			if(!(prim->fl_pts[i+j]&128))
				break;

		if(j>=num)
		{
			for(j=0; j<num; j++)
				prim->fl_pts[i+j]=0;
			return(i);
		}
	}

	BTGE_Mesh_CheckExpandPTS(prim, num);
	i=prim->n_pts;
	prim->n_pts+=num;
	for(j=0; j<num; j++)
		prim->fl_pts[i+j]=0;
	return(i);
}

BTGE_API int BTGE_Mesh_AllocFace(BTGE_MeshPrim *prim)
{
	int i;

	BTGE_Mesh_FlushCache(prim);

	for(i=0; i<prim->n_face; i++)
		if(prim->fl_face[i]&128)
	{
		prim->fl_face[i]=0;
		return(i);
	}

	BTGE_Mesh_CheckExpandFace(prim);
	i=prim->n_face++;
	prim->fl_face[i]=0;
	return(i);
}

BTGE_API int BTGE_Mesh_CopyVertex(BTGE_MeshPrim *prim, int vn)
{
	int i;

	i=BTGE_Mesh_AllocVertex(prim);
	V3F_COPY(prim->xyz+vn*3, prim->xyz+i*3);
	return(i);
}

BTGE_API int BTGE_Mesh_CopyST(BTGE_MeshPrim *prim, int vn)
{
	int i;

	i=BTGE_Mesh_AllocST(prim);
	V3F_COPY(prim->st+vn*2, prim->st+i*2);
	return(i);
}

BTGE_API int BTGE_Mesh_IndexXYZ(BTGE_MeshPrim *prim, float *v)
{
	int i;

	for(i=0; i<prim->n_xyz; i++)
		if(V3F_DIST(prim->xyz+i*3, v)<0.0001)
			return(i);

	i=BTGE_Mesh_AllocVertex(prim);
	V3F_COPY(v, prim->xyz+i*3);
	return(i);
}

BTGE_API int BTGE_Mesh_IndexST(BTGE_MeshPrim *prim, float *v)
{
	int i;

	for(i=0; i<prim->n_st; i++)
		if(V2F_DIST(prim->st+i*2, v)<0.0001)
			return(i);

	i=BTGE_Mesh_AllocST(prim);
	V2F_COPY(v, prim->st+i*2);
	return(i);
}

BTGE_API int BTGE_Mesh_IndexST2(BTGE_MeshPrim *prim, float *v)
{
	int i;

//	for(i=0; i<prim->n_st; i++)
//		if(V2F_DIST(prim->st+i*2, v)<0.0001)
//			return(i);

	i=BTGE_Mesh_AllocST(prim);
	V2F_COPY(v, prim->st+i*2);
	return(i);
}

BTGE_API int BTGE_Mesh_AddTriangle(BTGE_MeshPrim *prim,
	int i0, int j0, int i1, int j1, int i2, int j2)
{
	int i, p;

	BTGE_Mesh_FlushCache(prim);

	p=BTGE_Mesh_AllocPTS(prim, 3);
	prim->pts[(p+0)*2+0]=i0;
	prim->pts[(p+0)*2+1]=j0;
	prim->pts[(p+1)*2+0]=i1;
	prim->pts[(p+1)*2+1]=j1;
	prim->pts[(p+2)*2+0]=i2;
	prim->pts[(p+2)*2+1]=j2;

	i=BTGE_Mesh_AllocFace(prim);
	prim->face[i*2+0]=p;
	prim->face[i*2+1]=3;
	return(i);
}

BTGE_API int BTGE_Mesh_AddQuad(BTGE_MeshPrim *prim,
	int i0, int j0, int i1, int j1, int i2, int j2, int i3, int j3)
{
	int i, p;

	BTGE_Mesh_FlushCache(prim);

	p=BTGE_Mesh_AllocPTS(prim, 4);
	prim->pts[(p+0)*2+0]=i0;
	prim->pts[(p+0)*2+1]=j0;
	prim->pts[(p+1)*2+0]=i1;
	prim->pts[(p+1)*2+1]=j1;
	prim->pts[(p+2)*2+0]=i2;
	prim->pts[(p+2)*2+1]=j2;
	prim->pts[(p+3)*2+0]=i3;
	prim->pts[(p+3)*2+1]=j3;

	i=BTGE_Mesh_AllocFace(prim);
	prim->face[i*2+0]=p;
	prim->face[i*2+1]=4;
	return(i);
}

BTGE_API int BTGE_Mesh_AddPolygon(BTGE_MeshPrim *prim,
	int *vidx, int *stidx, int n)
{
	int i, p;

	BTGE_Mesh_FlushCache(prim);

	p=BTGE_Mesh_AllocPTS(prim, n);
	for(i=0; i<n; i++)
	{
		prim->pts[(p+i)*2+0]=vidx[i];
		prim->pts[(p+i)*2+1]=stidx[i];
	}

	i=BTGE_Mesh_AllocFace(prim);
	prim->face[i*2+0]=p;
	prim->face[i*2+1]=n;
	return(i);
}



BTGE_API void BTGE_Mesh_RotatePointAxisAngle(float *n, float th, float *pt)
{
	float io[3], vx[3], vy[3];
	float f, g;

	f=V3F_DOT(pt, n);
	V3F_ADDSCALE(pt, n, -f, io);
	g=V3F_NORMALIZE(io, vx);
	V3F_CROSS(n, vx, vy);

	io[0]=V3F_DOT(pt, vx);
	io[1]=V3F_DOT(pt, vy);
	io[2]=V3F_DOT(pt, n);

	f=atan2(io[1], io[0])+th;
	V3F_SCALE(n, io[2], pt);
	V3F_ADDSCALE(pt, vx, g*cos(f), pt);
	V3F_ADDSCALE(pt, vy, g*sin(f), pt);
}

BTGE_API void BTGE_Mesh_RotatePointOrgAxisAngle(
	float *o, float *n, float th, float *pt)
{
	V3F_SUB(pt, o, pt);
	BTGE_Mesh_RotatePointAxisAngle(n, th, pt);
	V3F_ADD(pt, o, pt);
}

BTGE_API void BTGE_Mesh_CalcCenter(BTGE_MeshPrim *prim, float *org)
{
	int i;

	V3F_ZERO(org);
	for(i=0; i<prim->n_xyz; i++)
	{
		if(prim->fl_xyz[i]&128)continue;
		V3F_ADD(org, prim->xyz+i*3, org);
	}
	V3F_SCALE(org, 1.0/prim->n_xyz, org);
}

BTGE_API void BTGE_Mesh_CalcFaceCenter(BTGE_MeshPrim *prim, int fn, float *org)
{
	int i, p, n;

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];

	V3F_ZERO(org);
	for(i=0; i<n; i++)
		{ V3F_ADD(org, prim->xyz+prim->pts[(p+i)*2+0]*3, org); }
	V3F_SCALE(org, 1.0/n, org);
}

BTGE_API void BTGE_Mesh_CalcFaceNormal(BTGE_MeshPrim *prim, int fn, float *norm)
{
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	int i0, i1, i2;

	int i, p, n;

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];

	i0=prim->pts[(p+0)*2+0];
	i1=prim->pts[(p+1)*2+0];
	i2=prim->pts[(p+2)*2+0];

	V3F_COPY(prim->xyz+i0*3, v0);
	V3F_COPY(prim->xyz+i1*3, v1);
	V3F_COPY(prim->xyz+i2*3, v2);

	V3F_SUB(v0, v1, dv0);
	V3F_SUB(v2, v1, dv1);
	V3F_CROSS(dv1, dv0, dv2);
	V3F_NORMALIZE(dv2, dv2);

	V3F_COPY(dv2, norm);
	norm[3]=V3F_DOT(v0, dv2);
}

BTGE_API void BTGE_Mesh_TranslateFace(BTGE_MeshPrim *prim, int fn, float *offs)
{
	float *fa;
	int i, p, n;

	BTGE_Mesh_FlushCache(prim);

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
	{
		fa=prim->xyz+prim->pts[(p+i)*2+0]*3;
		V3F_ADD(fa, offs, fa);
	}
}

BTGE_API void BTGE_Mesh_RotateFace(BTGE_MeshPrim *prim, int fn,
	float *org, float *norm, float th)
{
	float io[3];
	float *fa;
	int i, p, n;

	BTGE_Mesh_FlushCache(prim);

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
	{
		fa=prim->xyz+prim->pts[(p+i)*2+0]*3;
		BTGE_Mesh_RotatePointOrgAxisAngle(
			org, norm, th, fa);
	}
}

BTGE_API void BTGE_Mesh_ScaleFace(BTGE_MeshPrim *prim, int fn, float *org, float f)
{
	float io[3];
	float *fa;
	int i, p, n;

	BTGE_Mesh_FlushCache(prim);

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
	{
		fa=prim->xyz+prim->pts[(p+i)*2+0]*3;
		V3F_SUB(fa, org, io);
		V3F_ADDSCALE(org, io, f, fa);
	}
}

BTGE_API void BTGE_Mesh_BoxFace(BTGE_MeshPrim *prim, int fn,
	float *mins, float *maxs)
{
	float io[3];
	float *fa;
	int i, p, n;

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
	{
		fa=prim->xyz+prim->pts[(p+i)*2+0]*3;
		V3F_MIN(mins, fa, mins);
		V3F_MAX(maxs, fa, maxs);
	}
}

BTGE_API void BTGE_Mesh_DeleteFace(BTGE_MeshPrim *prim, int fn)
{
	int i, p, n;

	BTGE_Mesh_FlushCache(prim);

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
		prim->fl_pts[p+i]|=128;
	prim->fl_face[fn]|=128;
}

BTGE_API int BTGE_Mesh_CheckFaceVertex(BTGE_MeshPrim *prim, int fn, int vn)
{
	int i, p, n;

	if(prim->fl_face[fn]&128)return(0);

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
		if(prim->pts[(p+i)*2+0]==vn)
			return(1);
	return(0);
}

BTGE_API void BTGE_Mesh_DeleteVertex(BTGE_MeshPrim *prim, int vn)
{
	int i, j, p, n;

	BTGE_Mesh_FlushCache(prim);

	for(i=0; i<prim->n_face; i++)
	{
		j=BTGE_Mesh_CheckFaceVertex(prim, i, vn);
		if(j)BTGE_Mesh_DeleteFace(prim, i);
	}
	prim->fl_xyz[vn]|=128;
}

BTGE_API int BTGE_Mesh_CheckLineFace(
	BTGE_MeshPrim *prim, int fn, float *src, float *dst)
{
	return(BTGE_Mesh_CheckLineFaceClip(prim, fn, src, dst, NULL, NULL));
}

BTGE_API int BTGE_Mesh_CheckLineFaceClip(
	BTGE_MeshPrim *prim, int fn,
	float *src, float *dst,
	float *csv, float *cev)
{
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	float io[3], pt[4], norm[4];
	float f, g;
	int n, p;
	int i0, i1, i2;
	int i, j, k;

	if(csv) { V3F_COPY(src, csv); }
	if(cev) { V3F_COPY(dst, cev); }

	if(prim->fl_face[fn]&128)return(0);

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];

	i0=prim->pts[(p+0)*2+0];
	i1=prim->pts[(p+1)*2+0];
	i2=prim->pts[(p+2)*2+0];

	V3F_COPY(prim->xyz+i0*3, v0);
	V3F_COPY(prim->xyz+i1*3, v1);
	V3F_COPY(prim->xyz+i2*3, v2);

	V3F_SUB(v0, v1, dv0);
	V3F_SUB(v2, v1, dv1);
	V3F_CROSS(dv1, dv0, norm);
	V3F_NORMALIZE(norm, norm);
	norm[3]=V3F_DOT(v0, norm);

//	printf("P0\n");
//	return(1);

	if((V3F_NDOT(src, norm)<=0) && (V3F_NDOT(dst, norm)<=0))
		return(0);
	if((V3F_NDOT(src, norm)>=0) && (V3F_NDOT(dst, norm)>=0))
		return(0);

//	printf("P1\n");

	Vec3F_LinePlaneIntersect(src, dst, norm, io);

	for(j=0; j<n; j++)
	{
		V3F_COPY(prim->xyz+prim->pts[(p+j)*2+0]*3, v0);
		V3F_COPY(prim->xyz+prim->pts[(p+(j+1)%n)*2+0]*3, v1);
		V3F_SUB(v1, v0, dv0);
		V3F_CROSS(dv0, norm, dv2);
		V3F_NORMALIZE(dv2, dv2);
		if(V3F_DOT(io, dv2)>=V3F_DOT(v0, dv2))
			return(0);
	}
	
	if(cev) { V3F_COPY(io, cev); }
	return(1);
}

BTGE_API int BTGE_Mesh_CheckLinePrim(
	BTGE_MeshPrim *prim, float *src, float *dst)
{
	int i, j;

	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;
		j=BTGE_Mesh_CheckLineFace(prim, i, src, dst);
		if(j)return(1);
	}
	return(0);
}

BTGE_API int BTGE_Mesh_CheckLinePrimClip(
	BTGE_MeshPrim *prim,
	float *src, float *dst,
	float *csv, float *cev)
{
	float pt[3];
	float f, g, bd;
	int i, j, rt;

	bd=999999999; rt=0;
	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;
		j=BTGE_Mesh_CheckLineFaceClip(prim, i, src, dst, csv, pt);
		if(j)
		{
			if(!cev)return(1);
		
			f=V3F_DIST2(src, pt);
			if(f<bd)
			{
				V3F_COPY(pt, cev);
				bd=f;
			}
			rt=1;
		}
	}
	return(rt);
}

BTGE_API void BTGE_Mesh_BoxPrim(BTGE_MeshPrim *prim, float *mins, float *maxs)
{
	float *fa;
	int i;

	for(i=0; i<prim->n_xyz; i++)
	{
		fa=prim->xyz+i*3;
		V3F_MIN(mins, fa, mins);
		V3F_MAX(maxs, fa, maxs);
	}
}

BTGE_API void BTGE_Mesh_BoxGroup(BTGE_MeshGroup *grp, float *mins, float *maxs)
{
	BTGE_MeshGroup *gcur;
	BTGE_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		BTGE_Mesh_BoxGroup(gcur, mins, maxs);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		BTGE_Mesh_BoxPrim(mcur, mins, maxs);
		mcur=mcur->next;
	}
}



BTGE_API void BTGE_Mesh_CleanFaceEdges(BTGE_MeshPrim *prim)
{
	int p, n;
	int i, j, k;

	BTGE_Mesh_FlushCache(prim);

	for(i=0; i<prim->n_face; i++)
	{
		//collapse edges with shared vertices
		p=prim->face[i*2+0]; n=prim->face[i*2+1];
		for(j=0; (j<n) && (n>=3);)
		{
			if(prim->pts[(p+j)*2+0]==prim->pts[(p+(j+1)%n)*2+0])
			{
				for(k=j; k<(n-1); k++)
				{
					prim->pts[(p+k)*2+0]=
						prim->pts[(p+k+1)*2+0];
					prim->pts[(p+k)*2+1]=
						prim->pts[(p+k+1)*2+1];
				}
				prim->fl_pts[p+n-1]|=128;
				n--;
				continue;
			}
			j++;
		}
		prim->face[i*2+1]=n;

		//face has ceased being a valid polygon
		if(n<3)
		{
			prim->fl_pts[p+0]|=128; prim->fl_pts[p+1]|=128;
			prim->fl_pts[p+2]|=128; prim->fl_face[i]|=128;
		}
	}
}

BTGE_API int BTGE_Mesh_SubdivideMesh(BTGE_MeshPrim *prim)
{
	int eia[16], eib[16];
	float v0[3], v1[3], v2[3];

	float *op, *opst;	//original points
	float *fp, *fpst;	//face points
	float *ep, *epst;	//edge points

	float *fa, *fb;

	int *oe;	//original edges (2 org points)
	int *fe;	//face point to edge point

	float *ofp, *ofpst;	//original+face points
	float *oep, *oepst;	//original+edge points
	byte *ofpn, *oepn;	//original+edge point counts

	int nfp, nop;
	int noe, nfe;
	int i0, i1, i2;
	int i, j, k, l;
	int p, n;

	BTGE_Mesh_FlushCache(prim);

	prim->fl&=~1;

	op=malloc(prim->m_xyz*3*sizeof(float));
	fp=malloc(prim->m_face*3*sizeof(float));
	ep=malloc(prim->m_pts*3*sizeof(float));
	opst=malloc(prim->m_xyz*2*sizeof(float));
	fpst=malloc(prim->m_face*2*sizeof(float));
	epst=malloc(prim->m_pts*2*sizeof(float));

	oe=malloc(prim->m_pts*2*sizeof(int));
	fe=malloc(prim->m_pts*2*sizeof(int));
	oe=malloc(2*prim->m_pts*2*sizeof(int));

	ofp=malloc(prim->m_xyz*3*sizeof(float));
	oep=malloc(prim->m_xyz*3*sizeof(float));
	ofpst=malloc(prim->m_xyz*2*sizeof(float));
	oepst=malloc(prim->m_xyz*2*sizeof(float));
	ofpn=malloc(prim->m_xyz);
	oepn=malloc(prim->m_xyz);

	nfp=0; nop=0;
	nfe=0; noe=0;

	for(i=0; i<prim->n_face; i++)
		if(!(prim->fl_face[i]&128))
	{
		V3F_ZERO(fp+nfp*3);
		V2F_ZERO(fpst+nfp*2);

		//Calculate Face Points and Index Vertices
		p=prim->face[i*2+0];
		n=prim->face[i*2+1];
		for(j=0; j<n; j++)
		{
			fa=prim->xyz+prim->pts[(p+j)*2+0]*3;
			fb=prim->st+prim->pts[(p+j)*2+1]*2;
			V3F_ADD(fp+nfp*3, fa, fp+nfp*3);
			V2F_ADD(fpst+nfp*2, fb, fpst+nfp*2);

			for(k=0; k<nop; k++)
				if(V3F_DIST(fa, op+k*3)<0.001)
					break;
			if(k>=nop)
			{
				k=nop++;
				V3F_COPY(fa, op+k*3);
				V2F_COPY(fb, opst+k*2);

				V3F_ZERO(ofp+k*3);
				V3F_ZERO(oep+k*3);
				V2F_ZERO(ofpst+k*2);
				V2F_ZERO(oepst+k*2);
				ofpn[k]=0;
				oepn[k]=0;
			}

			eia[j]=k;
		}
		eia[j]=eia[0];
		V3F_SCALE(fp+nfp*3, 1.0/n, fp+nfp*3);
		V2F_SCALE(fpst+nfp*2, 1.0/n, fpst+nfp*2);

		//Build Edges Org->Org, and Face->Edge
		//l gives an edge sign flip
		for(j=0; j<n; j++)
		{
			k=eia[j];
			V3F_ADD(ofp+k*3, fp+nfp*3, ofp+k*3);
			V2F_ADD(ofpst+k*2, fpst+nfp*2, ofpst+k*2);
			ofpn[k]++;

			l=0;
			for(k=0; k<noe; k++)
			{
				if((oe[k*2+0]==eia[j]) &&
					(oe[k*2+1]==eia[j+1]))
						break;
				if((oe[k*2+0]==eia[j+1]) &&
					(oe[k*2+1]==eia[j]))
						{ l=1; break; }
			}
			if(k>=noe)
			{
				k=noe++;
				oe[k*2+0]=eia[j];
				oe[k*2+1]=eia[j+1];

				V3F_SCALEADDSCALE(
					op+eia[j]*3, 0.5,
					op+eia[j+1]*3, 0.5,
					ep+k*3);
				V2F_SCALEADDSCALE(
					opst+eia[j]*2, 0.5,
					opst+eia[j+1]*2, 0.5,
					epst+k*2);
			}

			fe[nfe*2+0]=nfp;
			fe[nfe*2+1]=l?(-(k+1)):k;
			nfe++;
		}
		nfp++;
	}

#if 1
	//Accumulate Face and Edge points for each original point
	for(i=0; i<noe; i++)
	{
		V3F_SCALEADDSCALE(
			op+oe[i*2+0]*3, 0.5, op+oe[i*2+1]*3, 0.5, v0);
		V2F_SCALEADDSCALE(
			opst+oe[i*2+0]*2, 0.5, opst+oe[i*2+1]*2, 0.5, v1);

		k=oe[i*2+0];
		V3F_ADD(oep+k*3, v0, oep+k*3);
		V2F_ADD(oepst+k*2, v1, oepst+k*2);
		oepn[k]++;

		k=oe[i*2+1];
		V3F_ADD(oep+k*3, v0, oep+k*3);
		V2F_ADD(oepst+k*2, v1, oepst+k*2);
		oepn[k]++;
	}
#endif

#if 0
	//Make each edge point the average of the connected org points and
	//any connected face points
	for(i=0; i<noe; i++)
	{
		V3F_ZERO(ep+i*3);
		V2F_ZERO(epst+i*2);

		l=0;

		V3F_ADD(ep+i*3, op+oe[i*2+0]*3, ep+i*3);
		V3F_ADD(ep+i*3, op+oe[i*2+1]*3, ep+i*3);
		V2F_ADD(epst+i*2, opst+oe[i*2+0]*2, epst+i*2);
		V2F_ADD(epst+i*2, opst+oe[i*2+1]*2, epst+i*2);
		l+=2;

		for(j=0; j<nfe; j++)
			if((fe[j*2+1]==i) || (fe[j*2+1]==((-i)-1)))
		{
			V3F_ADD(ep+i*3, fp+fe[j*2+0]*3, ep+i*3);
			V2F_ADD(epst+i*2, fpst+fe[j*2+0]*2, epst+i*2);
			l++;
		}

		V3F_SCALE(ep+i*3, 1.0/l, ep+i*3);
		V2F_SCALE(epst+i*2, 1.0/l, epst+i*2);
	}
#endif

#if 1
	//Make each edge point the average of the connected org points and
	//any connected face points
	for(i=0; i<noe; i++)
	{
		V3F_ZERO(v0);
		V2F_ZERO(v1);

		l=0;

		V3F_ADD(v0, op+oe[i*2+0]*3, v0);
		V3F_ADD(v0, op+oe[i*2+1]*3, v0);
		V2F_ADD(v1, opst+oe[i*2+0]*2, v1);
		V2F_ADD(v1, opst+oe[i*2+1]*2, v1);
		l+=2;

		for(j=0; j<nfe; j++)
			if((fe[j*2+1]==i) || (fe[j*2+1]==((-i)-1)))
		{
			V3F_ADD(v0, fp+fe[j*2+0]*3, v0);
			V2F_ADD(v1, fpst+fe[j*2+0]*2, v1);
			l++;
		}

		if(l<4)continue;

		V3F_SCALE(v0, 1.0/l, ep+i*3);
		V2F_SCALE(v1, 1.0/l, epst+i*2);
	}
#endif

#if 1
	//Recalculate Original Points
	for(i=0; i<nop; i++)
	{
		if(ofpn[i]<3)continue;
		if(oepn[i]<3)continue;

		V3F_SCALE(ofp+i*3, 1.0/ofpn[i], v0);
		V3F_ADDSCALE(v0, oep+i*3, 2.0/oepn[i], v0);
		V3F_ADDSCALE(v0, op+i*3, ofpn[i]-3, v0);
		V3F_SCALE(v0, 1.0/ofpn[i], op+i*3);

		V2F_SCALE(ofpst+i*2, 1.0/ofpn[i], v0);
		V2F_ADDSCALE(v0, oepst+i*2, 2.0/oepn[i], v0);
		V2F_ADDSCALE(v0, opst+i*2, ofpn[i]-3, v0);
		V2F_SCALE(v0, 1.0/ofpn[i], opst+i*2);
	}
#endif

//	printf("%d OP, %d FP, %d FE, %d OE\n", nop, nfp, nfe, noe);

	//Rebuild Mesh
	prim->n_xyz=0;	prim->n_st=0;
	prim->n_pts=0;	prim->n_face=0;

	for(i=0; i<nfp; i++)
		for(j=0; j<nfe; j++)
			if(fe[j*2+0]==i)
	{
		eia[0]=BTGE_Mesh_IndexXYZ(prim, fp+i*3);
		eib[0]=BTGE_Mesh_IndexST(prim, fpst+i*2);
		k=fe[j*2+1];
		if(k<0)k=(-k)-1;

		eia[1]=BTGE_Mesh_IndexXYZ(prim, ep+k*3);
		eib[1]=BTGE_Mesh_IndexST(prim, epst+k*2);

		i0=oe[k*2+((fe[j*2+1]<0)?0:1)];
		eia[2]=BTGE_Mesh_IndexXYZ(prim, op+i0*3);
		eib[2]=BTGE_Mesh_IndexST(prim, opst+i0*2);

		for(l=0; l<noe; l++)
		{
			if(l==k)continue;
			if((oe[l*2+0]!=i0) && (oe[l*2+1]!=i0))
				continue;
			for(i1=0; i1<nfe; i1++)
				if(fe[i1*2+0]==i)
			{
				if(fe[i1*2+1]==l)break;
				if(fe[i1*2+1]==(-(l+1)))break;
			}
			if(i1<nfe)break;
		}

		if(l>=noe)
		{
			BTGE_Mesh_AddTriangle(prim, eia[0], eib[0],
				eia[1], eib[1], eia[2], eib[2]);
			continue;
		}

		eia[3]=BTGE_Mesh_IndexXYZ(prim, ep+l*3);
		eib[3]=BTGE_Mesh_IndexST(prim, epst+l*2);

//		BTGE_Mesh_AddTriangle(prim, eia[0], eib[0],
//			eia[1], eib[1], eia[2], eib[2]);
//		BTGE_Mesh_AddTriangle(prim, eia[0], eib[0],
//			eia[2], eib[2], eia[3], eib[3]);

		BTGE_Mesh_AddQuad(prim, eia[0], eib[0], eia[1], eib[1],
			eia[2], eib[2], eia[3], eib[3]);
	}

	free(op);	free(fp);	free(ep);
	free(opst);	free(fpst);	free(epst);
	free(oe);	free(fe);
	free(ofp);	free(oep);	free(ofpn);
	free(ofpst);	free(oepst);	free(oepn);

	return(0);
}


BTGE_API int BTGE_Mesh_OptimizePrim(BTGE_MeshPrim *prim)
{
	float *xyz, *st;
	int *pts, *face;
	byte *fl_face;
	float *fa, *fb;
	int n_xyz, n_st, n_pts, n_face;
	int p, n, p1;
	int i, j, k;

	BTGE_Mesh_FlushCache(prim);

	xyz=prim->xyz;
	st=prim->st;
	pts=prim->pts;
	face=prim->face;

	n_xyz=prim->n_xyz;
	n_st=prim->n_st;
	n_pts=prim->n_pts;
	n_face=prim->n_face;
	fl_face=prim->fl_face;

	prim->n_xyz=0;	prim->n_st=0;
	prim->n_pts=0;	prim->n_face=0;

	prim->xyz=gcatomic(8*3*sizeof(float));
	prim->st=gcatomic(24*2*sizeof(float));
	prim->pts=gcatomic(24*2*sizeof(int));
	prim->face=gcatomic(8*2*sizeof(int));

	prim->fl_xyz=gcatomic(8);
	prim->fl_st=gcatomic(24);
	prim->fl_pts=gcatomic(24);
	prim->fl_face=gcatomic(8);

	prim->m_xyz=8;
	prim->m_st=24;
	prim->m_pts=24;
	prim->m_face=8;

	for(i=0; i<n_face; i++)
	{
		if(fl_face[i]&128)continue;
		p=face[i*2+0];
		n=face[i*2+1];

		p1=BTGE_Mesh_AllocPTS(prim, n);
		for(j=0; j<n; j++)
		{
			fa=xyz+pts[(p+j)*2+0]*3;
			fb=st+pts[(p+j)*2+1]*2;
			prim->pts[(p1+j)*2+0]=BTGE_Mesh_IndexXYZ(prim, fa);
			prim->pts[(p1+j)*2+1]=BTGE_Mesh_IndexST(prim, fb);
		}

		j=BTGE_Mesh_AllocFace(prim);
		prim->face[j*2+0]=p1;
		prim->face[j*2+1]=n;
	}

	gcfree(xyz);
	gcfree(st);
	gcfree(pts);
	gcfree(face);
	gcfree(fl_face);

	return(0);
}


BTGE_API void BTGE_Mesh_MergePrim(BTGE_MeshPrim *prim, BTGE_MeshPrim *prim2)
{
	float *fa, *fb;
	int p, n, p1;
	int i, j, k;

	BTGE_Mesh_FlushCache(prim);
	BTGE_Mesh_FlushCache(prim2);

	for(i=0; i<prim2->n_face; i++)
	{
		if(prim2->fl_face[i]&128)continue;
		p=prim2->face[i*2+0];
		n=prim2->face[i*2+1];

		p1=BTGE_Mesh_AllocPTS(prim, n);
		for(j=0; j<n; j++)
		{
			fa=prim2->xyz+prim2->pts[(p+j)*2+0]*3;
			fb=prim2->st+prim2->pts[(p+j)*2+1]*2;
			prim->pts[(p1+j)*2+0]=BTGE_Mesh_IndexXYZ(prim, fa);
			prim->pts[(p1+j)*2+1]=BTGE_Mesh_IndexST(prim, fb);
		}

		j=BTGE_Mesh_AllocFace(prim);
		prim->face[j*2+0]=p1;
		prim->face[j*2+1]=n;
	}
}

BTGE_API void BTGE_Mesh_ProjectPointST(BTGE_MeshPrim *prim,
	float *pt, float *st, float *nv,
	float *sdir, float *tdir, float *udir,
	float sv, float tv, int w, int h)
{
	float io[3], torg[3];
	float *fa;

	if(prim->tex_mode==BTGE_TEX_PLANAR)
	{
		V3F_ADDSCALE(prim->tex_org, sdir, -sv*0.5, torg);
		V3F_ADDSCALE(torg, tdir, -tv*0.5, torg);

		V3F_SUB(pt, torg, io);

		st[0]=V3F_DOT(io, sdir)/sv;
		st[1]=V3F_DOT(io, tdir)/tv;

		//backside
		if(w>=(h*2))
		{
			st[0]*=0.5;
			if(st[0]<0)st[0]=0;
			if(st[0]>0.5)st[0]=0.5;
			if(V3F_DOT(nv, udir)<0)st[0]=-st[0];
		}

		return;
	}

	if(prim->tex_mode==BTGE_TEX_BOX)
	{
		if(fabs(V3F_DOT(nv, sdir))>fabs(V3F_DOT(nv, udir)))
		{
			fa=sdir; sdir=udir; udir=fa;
			sv=-sv;
		}
		if(fabs(V3F_DOT(nv, tdir))>fabs(V3F_DOT(nv, udir)))
		{
			fa=tdir; tdir=udir; udir=fa;
			tv=-tv;
		}
		if(V3F_DOT(nv, udir)<0)sv=-sv;

		V3F_ADDSCALE(prim->tex_org, sdir, -sv*0.5, torg);
		V3F_ADDSCALE(torg, tdir, -tv*0.5, torg);

		V3F_SUB(pt, torg, io);

		st[0]=V3F_DOT(io, sdir)/sv;
		st[1]=V3F_DOT(io, tdir)/tv;

		return;
	}

	if(prim->tex_mode==BTGE_TEX_SPHERICAL)
	{
		V3F_SUB(pt, prim->tex_org, io);
		V3F_NORMALIZE(io, io);

		torg[0]=V3F_DOT(io, sdir);
		torg[1]=-V3F_DOT(io, udir);
		torg[2]=V3F_DOT(io, tdir);

		st[0]=(atan2(torg[1], torg[0])+M_PI)/(2*M_PI);
		st[1]=-(acos(torg[2])/M_PI);
		return;
	}

	if(prim->tex_mode==BTGE_TEX_CYLINDRICAL)
	{
		V3F_ADDSCALE(prim->tex_org, tdir, -tv*0.5, torg);

		V3F_SUB(pt, torg, io);
		V3F_NORMALIZE(io, io);

		torg[0]=V3F_DOT(io, sdir);
		torg[1]=-V3F_DOT(io, udir);
		torg[2]=V3F_DOT(io, tdir);

		st[0]=(atan2(torg[1], torg[0])+M_PI)/(2*M_PI);
		st[1]=torg[2]/tv;
		return;
	}

	//default: planar
	V3F_ADDSCALE(prim->tex_org, sdir, -sv*0.5, torg);
	V3F_ADDSCALE(torg, tdir, -tv*0.5, torg);
	V3F_SUB(pt, torg, io);
	st[0]=V3F_DOT(io, sdir)/sv;
	st[1]=V3F_DOT(io, tdir)/tv;
}

BTGE_API void BTGE_Mesh_ReprojectPrim(BTGE_MeshPrim *prim)
{
	float sdir[4], tdir[4], udir[4], torg[3];
	float io[3], nv[4];
	float st[2], sv, tv;
	float *fa, *fb;
	int p, n, w, h;
	int i, j, k;

	w=-1; h=-1;
//	i=Tex_LoadFile(prim->tex, &w, &h);
	i=BTGE_Texture_LoadImageSize(prim->tex, &w, &h);
	if((i<0) || (w<=0) || (h<=0))return;


//	printf("Remap Tex Coords: %d F %d V\n", prim->n_face, prim->n_xyz);

	sv=V3F_NORMALIZE(prim->tex_sdir, sdir);
	tv=V3F_NORMALIZE(prim->tex_tdir, tdir);
	V3F_CROSS(sdir, tdir, udir);
	sdir[3]=V3F_DOT(prim->tex_org, sdir)-sv*0.5;
	tdir[3]=V3F_DOT(prim->tex_org, tdir)-tv*0.5;
	udir[3]=V3F_DOT(prim->tex_org, udir);

	prim->n_st=0;
	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;
		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		BTGE_Mesh_CalcFaceNormal(prim, i, nv);

		for(j=0; j<n; j++)
		{
			fa=prim->xyz+prim->pts[(p+j)*2+0]*3;

			BTGE_Mesh_ProjectPointST(prim, fa, st, nv,
				sdir, tdir, udir, sv, tv, w, h);

			prim->pts[(p+j)*2+1]=BTGE_Mesh_IndexST2(prim, st);
		}
	}
}

BTGE_API void BTGE_Mesh_ReprojectPrimXY(BTGE_MeshPrim *prim)
{
	float mst[2], nst[2], io[3];
	float *fa;
	int i;

	mst[0]=1000000;		mst[1]=1000000;
	nst[0]=-1000000;	nst[1]=-1000000;
	V3F_ZERO(io);
	for(i=0; i<prim->n_xyz; i++)
	{
		fa=prim->xyz+i*3;
		if(fa[0]<mst[0])mst[0]=fa[0];
		if(fa[1]<mst[1])mst[1]=fa[1];

		if(fa[0]>nst[0])nst[0]=fa[0];
		if(fa[1]>nst[1])nst[1]=fa[1];

		V3F_ADD(io, fa, io);
	}
	V3F_SCALE(io, 1.0/i, io);

	prim->tex_org[0]=io[0];
	prim->tex_org[1]=io[1];
	prim->tex_org[2]=io[2];

	prim->tex_sdir[0]=nst[0]-mst[0];
	prim->tex_sdir[1]=0;
	prim->tex_sdir[2]=0;

	prim->tex_tdir[0]=0;
	prim->tex_tdir[1]=nst[1]-mst[1];
	prim->tex_tdir[2]=0;

	BTGE_Mesh_ReprojectPrim(prim);
}

BTGE_API void BTGE_Mesh_ReprojectPrimXZ(BTGE_MeshPrim *prim)
{
	float mst[3], nst[3];
	float *fa;
	int i;

	mst[0]=1000000;		mst[1]=1000000;		mst[2]=1000000;
	nst[0]=-1000000;	nst[1]=-1000000;	nst[2]=-1000000;

	for(i=0; i<prim->n_xyz; i++)
	{
		fa=prim->xyz+i*3;
		if(fa[0]<mst[0])mst[0]=fa[0];
		if(fa[1]<mst[1])mst[1]=fa[1];
		if(fa[2]<mst[2])mst[2]=fa[2];

		if(fa[0]>nst[0])nst[0]=fa[0];
		if(fa[1]>nst[1])nst[1]=fa[1];
		if(fa[2]>nst[2])nst[2]=fa[2];
	}

	prim->tex_org[0]=(mst[0]+nst[0])*0.5;
	prim->tex_org[1]=(mst[1]+nst[1])*0.5;
	prim->tex_org[2]=(mst[2]+nst[2])*0.5;

	prim->tex_sdir[0]=nst[0]-mst[0];
	prim->tex_sdir[1]=0;
	prim->tex_sdir[2]=0;

	prim->tex_tdir[0]=0;
	prim->tex_tdir[1]=0;
	prim->tex_tdir[2]=nst[2]-mst[2];

	BTGE_Mesh_ReprojectPrim(prim);
}

BTGE_API void BTGE_Mesh_FlushCache(BTGE_MeshPrim *prim)
{
	if(prim->norm_face) { gcfree(prim->norm_face); prim->norm_face=NULL; }
	if(prim->norm_pts) { gcfree(prim->norm_pts); prim->norm_pts=NULL; }
	if(prim->norm_xyz) { gcfree(prim->norm_xyz); prim->norm_xyz=NULL; }

	if(prim->edge) { gcfree(prim->edge); prim->edge=NULL; }
}

BTGE_API void BTGE_Mesh_BuildCache(BTGE_MeshPrim *prim)
{
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	float *nxyz, *nface, *npts;
	int *edge;
	float f, g;
	int i0, i1, i2, p, n, ne;
	int i, j, k;

	if(!prim->norm_face)
	{
		prim->norm_face=gctatomic("mesh_norm_face", prim->n_face*4*sizeof(float));
		prim->norm_pts=gctatomic("mesh_norm_pts", prim->n_pts*4*sizeof(float));
		prim->norm_xyz=gctatomic("mesh_norm_xyz", prim->n_xyz*4*sizeof(float));
		nface=prim->norm_face;
		npts=prim->norm_pts;
		nxyz=prim->norm_xyz;

		for(i=0; i<prim->n_face; i++)
		{
			p=prim->face[i*2+0];
			n=prim->face[i*2+1];

			i0=prim->pts[(p+0)*2+0];
			i1=prim->pts[(p+1)*2+0];
			i2=prim->pts[(p+2)*2+0];

			V3F_COPY(prim->xyz+i0*3, v0);
			V3F_COPY(prim->xyz+i1*3, v1);
			V3F_COPY(prim->xyz+i2*3, v2);

			V3F_SUB(v0, v1, dv0);
			V3F_SUB(v2, v1, dv1);
			V3F_CROSS(dv1, dv0, dv2);
			V3F_NORMALIZE(dv2, dv2);

			V3F_COPY(dv2, nface+i*4);
			nface[i*4+3]=V3F_DOT(v0, dv2);

			for(j=0; j<n; j++)
			{
				k=prim->pts[(p+j)*2+0];
				V3F_ADD(nxyz+k*4, dv2, nxyz+k*4);
			}
		}

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_NORMALIZE(nxyz+i*4, nxyz+i*4);
			prim->norm_xyz[i*4+3]=V3F_DOT(prim->xyz+i*3, nxyz+i*4);
		}


		for(i=0; i<prim->n_face; i++)
		{
			p=prim->face[i*2+0];
			n=prim->face[i*2+1];

			for(j=0; j<n; j++)
			{
				if((p+j)>=prim->n_pts)
				{
					printf("BTGE_Mesh_BuildCache: overflow trap\n");
					continue;
				}
			
				k=prim->pts[(p+j)*2+0];
				f=V3F_DOT(nxyz+k*4, nface+i*4);
				if(f<0)f=0; if(f>1)f=1;
				V3F_SCALEADDSCALE(
					nface+i*4, 1.0-f, nxyz+k*4, f, dv0);
				V3F_NORMALIZE(dv0, dv0);
				V3F_COPY(dv0, npts+(p+j)*4);
				npts[(p+j)*4+3]=V3F_DOT(prim->xyz+k*4, dv0);
			}
		}
	}

	if(!prim->edge)
	{
		edge=malloc(2*prim->n_pts*4*sizeof(int)); ne=0;
		for(i=0; i<prim->n_face; i++)
		{
			p=prim->face[i*2+0]; n=prim->face[i*2+1];
			for(j=0; j<(n-1); j++)
			{
				i0=prim->pts[(p+j)*2+0];
				i1=prim->pts[(p+j+1)*2+0];

				for(k=0; k<ne; k++)
					if((edge[k*4+0]==i1) && (edge[k*4+1]==i0))
						break;
				if(k<ne) { edge[k*4+3]=i; continue; }

				edge[ne*4+0]=i0; edge[ne*4+1]=i1;
				edge[ne*4+2]=i; edge[ne*4+3]=i;
				ne++;
			}

			i0=prim->pts[(p+n-1)*2+0]; i1=prim->pts[(p+0)*2+0];
			for(k=0; k<ne; k++)if((edge[k*4+0]==i1) && (edge[k*4+1]==i0))
				break;
			if(k<ne) { edge[k*4+3]=i; continue; }

			edge[ne*4+0]=i0; edge[ne*4+1]=i1;
			edge[ne*4+2]=i; edge[ne*4+3]=i;
			ne++;
		}

		prim->n_edge=ne;
		prim->edge=gctatomic("mesh_prim_edge", prim->n_edge*4*sizeof(int));
		for(i=0; i<(prim->n_edge*4); i++)prim->edge[i]=edge[i];
		free(edge);

		//information about bounding volume
		V3F_CONST(prim->com_org, 0);
		V3F_CONST(prim->com_mins,  999999);
		V3F_CONST(prim->com_maxs, -999999);

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_MIN(prim->com_mins, prim->xyz+i*3, prim->com_mins);
			V3F_MAX(prim->com_maxs, prim->xyz+i*3, prim->com_maxs);
			V3F_ADD(prim->com_org, prim->xyz+i*3, prim->com_org);
//			prim->norm_xyz[i*4+3]=V3F_DOT(prim->xyz+i*3, nxyz+i*4);
		}
		V3F_SCALE(prim->com_org, 1.0/prim->n_xyz, prim->com_org);
		prim->com_rad=V3F_DIST(prim->com_maxs, prim->com_org);
	}
}
