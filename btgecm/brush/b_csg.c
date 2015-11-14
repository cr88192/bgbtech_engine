#include <btgecm.h>

BTGE_BrushTempFace *lbxgl_face_list=NULL;

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_AllocFace()
{
	BTGE_BrushTempFace *tmp;

	if(lbxgl_face_list)
	{
		tmp=lbxgl_face_list;
		lbxgl_face_list=tmp->next;

		memset(tmp, 0, sizeof(BTGE_BrushTempFace));
		return(tmp);
	}

	tmp=gcalloc(sizeof(BTGE_BrushTempFace));
	return(tmp);
}

BTGE_API void BTGE_BrushCSG_FreeFace(BTGE_BrushTempFace *face)
{
	if(!face)return;

	if(face->xyz)gcfree(face->xyz);
	if(face->st)gcfree(face->st);
	if(face->evec)gcfree(face->evec);

	face->xyz=NULL;
	face->st=NULL;
	face->evec=NULL;
	face->vec=NULL;

//	gcfree(face);

	face->next=lbxgl_face_list;
	lbxgl_face_list=face;
}

BTGE_API void BTGE_BrushCSG_FreeFaceList(BTGE_BrushTempFace *face)
{
	BTGE_BrushTempFace *cur, *nxt;

	cur=face;
	while(cur)
	{
		nxt=cur->next;
		BTGE_BrushCSG_FreeFace(cur);
		cur=nxt;
	}
}


BTGE_API void BTGE_BrushCSG_FreeDrawFace(BTGE_BrushDrawFace *face)
{
	if(!face)return;

//	if(face->xyz)gcfree(face->xyz);
//	if(face->st)gcfree(face->st);
//	if(face->evec)gcfree(face->evec);

	face->xyz=NULL;
	face->st=NULL;
	face->evec=NULL;
	face->vec=NULL;

//	gcfree(face);

	face->next=lbxgl_face_list;
	lbxgl_face_list=face;
}

BTGE_API void BTGE_BrushCSG_DestroyBrushFaces(BTGE_Brush *brush)
{
	BTGE_BrushTempFace *cur, *nxt;
	
	if(!brush->face)return;
	if(brush->face==BGBDY_AUXNULL)
		return;

	cur=brush->face;
	while(cur)
	{
		nxt=cur->bnext;
		BTGE_BrushCSG_FreeDrawFace(cur);
		cur=nxt;
	}
	brush->face=NULL;
}

BTGE_API BTGE_BrushDrawFace *BTGE_BrushCSG_GetBrushFaces(
	BTGE_Brush *brush, BTGE_BrushDrawFace *lst)
{
	BTGE_BrushDrawFace *cur, *nxt;

//	cur=lst;
//	while(cur)
//		cur=cur->next;

	if(!brush->face)
	{
		if(brush->patch && brush->mesh)
		{
			return(lst);
		}

//		cur=BTGE_BrushCSG_MakeBrushDrawFaces(brush, NULL);

		cur=BTGE_BrushCSG_MakeBrushDrawFacesClip(brush);
		cur=BTGE_BrushCSG_SubdivideFaceList(cur, 256);
		
//		cur=BTGE_BrushCSG_MakeBrushFaces(brush, NULL);
		while(cur)
		{
#if 0
			if(BTGE_BrushCSG_InferFaceOutside(cur))
			{
				nxt=cur->next;
				BTGE_BrushCSG_FreeDrawFace(cur);
				cur=nxt;
				continue;
			}
#endif

			cur->bnext=brush->face;
			brush->face=cur;
			cur=cur->next;
		}
		
		if(!brush->face)
			brush->face=BGBDY_AUXNULL;
	}

	if(brush->face==BGBDY_AUXNULL)
		return(lst);

#if 0
	cur=lst;
	while(cur)
	{
		if(cur->brush==brush)
			{ *(int *)-1=-1; }
		cur=cur->next;
	}
#endif

	cur=brush->face;
	while(cur)
	{
		if(cur->brush!=brush)
			{ *(int *)-1=-1; }
		cur->next=lst;
		lst=cur;
		cur=cur->bnext;
	}

//	cur=lst;
//	while(cur)
//		cur=cur->next;

	return(lst);
}

// BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_BuildBrushFacesClip(
// 	BTGE_Brush *brush, BTGE_BrushNode *root)

BTGE_API BTGE_BrushDrawFace *BTGE_BrushCSG_MakeBrushDrawFacesClip(
	BTGE_Brush *brush)
{
	BTGE_BrushTempFace *tlst;
	BTGE_BrushDrawFace *lst;
	BTGE_BrushNode *node;
	
	node=brush->node;
	while(node && node->pnode)
		node=node->pnode;
		
	tlst=BTGE_BrushCSG_BuildBrushFacesClip(brush, node);
	
	return(tlst);
}

BTGE_API BTGE_BrushDrawFace *BTGE_BrushCSG_MakeBrushDrawFaces(
	BTGE_Brush *brsh, BTGE_BrushDrawFace *lst)
{
	BTGE_BrushDrawFace *tmp;
	float *fv;
	int i, j, k, l;

	for(i=0; i<brsh->n_poly; i++)
	{
		tmp=BTGE_BrushCSG_AllocFace();
		tmp->brush=brsh;

		l=brsh->pf_nvec[i];
//		tmp->xyz=gcatomic(l*3*sizeof(float));
//		tmp->st=gcatomic(l*2*sizeof(float));

		tmp->xyz=brsh->xyz;
		tmp->st=brsh->st;
//		tmp->evec=brush->evec;
		
		tmp->vec=brsh->vec + brsh->pf_ivec[i]*4;
		tmp->n_vec=l;

		tmp->flags=brsh->pf_flag[i];
		tmp->tex=brsh->pf_tex[i];
		tmp->prim=BTGE_POLYGON;

		fv=brsh->norm+brsh->pf_plane[i]*4;
		V4F_COPY(fv, tmp->norm);
		fv=brsh->sdir+brsh->pf_plane[i]*4;
		V4F_COPY(fv, tmp->sdir);
		fv=brsh->tdir+brsh->pf_plane[i]*4;
		V4F_COPY(fv, tmp->tdir);

#if 0
		k=brsh->pf_ivec[i];
		for(j=0; j<l; j++)
		{
			fv=brsh->xyz+brsh->vec[(k+j)*4+0]*3;
			V3F_COPY(fv, tmp->xyz+j*3);
			fv=brsh->st+brsh->vec[(k+j)*4+1]*2;
			V2F_COPY(fv, tmp->st+j*2);
		}
#endif

		tmp->next=lst;
		lst=tmp;
	}

	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_MakeBrushFaces(
	BTGE_Brush *brsh, BTGE_BrushTempFace *lst)
{
	BTGE_BrushTempFace *tmp;
	float *fv;
	int i, j, k, l;

	for(i=0; i<brsh->n_poly; i++)
	{
		tmp=BTGE_BrushCSG_AllocFace();
		tmp->brush=brsh;

		l=brsh->pf_nvec[i];
		tmp->xyz=gcatomic(l*3*sizeof(float));
		tmp->st=gcatomic(l*2*sizeof(float));
		tmp->n_vec=l;

		tmp->flags=brsh->pf_flag[i];
		tmp->tex=brsh->pf_tex[i];
		tmp->prim=BTGE_POLYGON;

		fv=brsh->norm+brsh->pf_plane[i]*4;
		V4F_COPY(fv, tmp->norm);
		fv=brsh->sdir+brsh->pf_plane[i]*4;
		V4F_COPY(fv, tmp->sdir);
		fv=brsh->tdir+brsh->pf_plane[i]*4;
		V4F_COPY(fv, tmp->tdir);

		k=brsh->pf_ivec[i];
		for(j=0; j<l; j++)
		{
			fv=brsh->xyz+brsh->vec[(k+j)*4+0]*3;
			V3F_COPY(fv, tmp->xyz+j*3);
			fv=brsh->st+brsh->vec[(k+j)*4+1]*2;
			V2F_COPY(fv, tmp->st+j*2);
		}

		tmp->next=lst;
		lst=tmp;
	}

	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_MakeFacesBrushList(
	BTGE_Brush *fst)
{
	BTGE_BrushTempFace *lst;
	BTGE_Brush *cur;

	cur=fst; lst=NULL;
	while(cur)
	{
		lst=BTGE_BrushCSG_MakeBrushFaces(cur, lst);
		cur=cur->chain;
	}

	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_CopyFace(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst)
{
	BTGE_BrushTempFace *tmp;
	int i, l;

	tmp=BTGE_BrushCSG_AllocFace();

	l=face->n_vec;
	tmp->xyz=gcatomic(l*3*sizeof(float));
	tmp->st=gcatomic(l*2*sizeof(float));
	for(i=0; i<(3*l); i++)tmp->xyz[i]=face->xyz[i];
	for(i=0; i<(2*l); i++)tmp->st[i]=face->st[i];
	tmp->n_vec=l;

	V4F_COPY(face->norm, tmp->norm);
	V4F_COPY(face->sdir, tmp->sdir);
	V4F_COPY(face->tdir, tmp->tdir);
	tmp->tex=face->tex;
	tmp->flags=face->flags;
	tmp->brush=face->brush;
	tmp->prim=face->prim;

	tmp->next=lst;
	return(tmp);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_LeftFacePlane(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	float *norm)
{
	float xyz[64*3], st[64*2];
	BTGE_BrushTempFace *tmp;
	float f, g;
	int i, l;

	f=V3F_LEN(norm);
	if((f<0.9999) || (f>1.0001))
	{
		printf("BTGE_BrushCSG_LeftFacePlane: Normal Check "
				"%f (%f %f %f %f)\n", f,
			norm[0], norm[1], norm[2], norm[3]);
	}

	l=Hull_ClipFaceST(norm, face->xyz, face->st, xyz, st, face->n_vec);
//	l=Hull_ClipFace(norm, face->xyz, xyz, face->n_vec);

	if(l<3)
	{
		printf("BTGE_BrushCSG_LeftFacePlane: Clipped Away %d\n", l);
		printf("\tFace=(%.3f %.3f %.3f %.3f)\n",
			face->norm[0], face->norm[1], face->norm[2], face->norm[3]);
		printf("\tPlane=(%.3f %.3f %.3f %.3f)\n",
			norm[0], norm[1], norm[2], norm[3]);
		return(BTGE_BrushCSG_CopyFace(face, lst));
//		return(NULL);
	}
	if(l>=48)
	{
		printf("BTGE_BrushCSG_LeftFacePlane: Sanity %d\n", l);
		return(lst);
	}

	for(i=0; i<l; i++)
	{
		f=V3F_NDOT(xyz+(i*3), face->norm);
		V3F_ADDSCALE(xyz+(i*3), face->norm, -f, xyz+(i*3));
#if 0
		if((f<0) || (f>0))
			{ printf("BTGE_BrushCSG_LeftFacePlane: "
				"Vertex Off-Plane %f\n", f); }
#endif		
	}

	tmp=BTGE_BrushCSG_AllocFace();

	tmp->xyz=gcatomic(l*3*sizeof(float));
	tmp->st=gcatomic(l*2*sizeof(float));
	for(i=0; i<(3*l); i++)tmp->xyz[i]=xyz[i];
	for(i=0; i<(2*l); i++)tmp->st[i]=st[i];
	tmp->n_vec=l;

	V4F_COPY(face->norm, tmp->norm);
	V4F_COPY(face->sdir, tmp->sdir);
	V4F_COPY(face->tdir, tmp->tdir);
	tmp->tex=face->tex;
	tmp->flags=face->flags;
	tmp->brush=face->brush;
	tmp->prim=face->prim;

	tmp->next=lst;
	return(tmp);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_RightFacePlane(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	float *norm)
{
	float norm1[4];
	BTGE_BrushTempFace *tmp;

	norm1[0]=-norm[0]; norm1[1]=-norm[1];
	norm1[2]=-norm[2]; norm1[3]=-norm[3];

	tmp=BTGE_BrushCSG_LeftFacePlane(face, lst, norm1);
	return(tmp);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ChainLeftFacePlane(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	float *norm)
{
	BTGE_BrushTempFace *tmp;

	tmp=BTGE_BrushCSG_LeftFacePlane(face, NULL, norm);
	tmp->chain=lst;
	return(tmp);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ChainRightFacePlane(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	float *norm)
{
	BTGE_BrushTempFace *tmp;

	tmp=BTGE_BrushCSG_RightFacePlane(face, NULL, norm);
	tmp->chain=lst;
	return(tmp);
}


BTGE_API void BTGE_BrushCSG_FaceExtents(BTGE_BrushTempFace *face,
	float *dir, float *rm, float *rn)
{
	float f, g, m, n;
	int i;

	m=999999; n=-999999;
	for(i=0; i<face->n_vec; i++)
	{
		f=V3F_DOT(face->xyz+i*3, dir);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
}

BTGE_API void BTGE_BrushCSG_FacePlaneExtents(BTGE_BrushTempFace *face,
	float *norm, float *rm, float *rn)
{
	float f, g, m, n;
	int i;

	m=999999; n=-999999;
	for(i=0; i<face->n_vec; i++)
	{
		f=V3F_NDOT(face->xyz+i*3, norm);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
}

BTGE_API void BTGE_BrushCSG_BBoxFace(
	BTGE_BrushTempFace *face, float *min, float *max)
{
	float f;
	int i;

	for(i=0; i<face->n_vec; i++)
	{
		f=face->xyz[i*3+0]; if(f<min[0])min[0]=f; if(f>max[0])max[0]=f;
		f=face->xyz[i*3+1]; if(f<min[1])min[1]=f; if(f>max[1])max[1]=f;
		f=face->xyz[i*3+2]; if(f<min[2])min[2]=f; if(f>max[2])max[2]=f;
	}
}

BTGE_API void BTGE_BrushCSG_BBoxFaceList(BTGE_BrushTempFace *lst,
	float *min, float *max)
{
	BTGE_BrushTempFace *cur;

	min[0]= 999999; min[1]= 999999; min[2]= 999999;
	max[0]=-999999; max[1]=-999999; max[2]=-999999;

	cur=lst;
	while(cur)
	{
		BTGE_BrushCSG_BBoxFace(cur, min, max);
		cur=cur->next;
	}
}

BTGE_API void BTGE_BrushCSG_GetFaceOrigin(
	BTGE_BrushTempFace *face, float *org)
{
	float f;
	int i;

	org[0]=0; org[1]=0; org[2]=0;
	for(i=0; i<face->n_vec; i++)
	{
		org[0]+=face->xyz[i*3+0];
		org[1]+=face->xyz[i*3+1];
		org[2]+=face->xyz[i*3+2];
	}

	org[0]/=face->n_vec;
	org[1]/=face->n_vec;
	org[2]/=face->n_vec;
}

/** Clip polygons larger than max on any axis into smaller polygons */
BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_SubdivideFace(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst, float max)
{
	float bm[3], bn[3], tv[4];
	BTGE_BrushTempFace *lface, *rface;
	int i;

	bm[0]= 999999; bm[1]= 999999; bm[2]= 999999;
	bn[0]=-999999; bn[1]=-999999; bn[2]=-999999;
	BTGE_BrushCSG_BBoxFace(face, bm, bn);
	
	for(i=0; i<3; i++)
	{
		if((bn[i]-bm[i])>max)
		{
			tv[0]=0; tv[1]=0; tv[2]=0;
			tv[i]=1; tv[3]=(bn[i]+bm[i])*0.5;
	
			lface=BTGE_BrushCSG_LeftFacePlane(face, NULL, tv);
			rface=BTGE_BrushCSG_RightFacePlane(face, NULL, tv);
			BTGE_BrushCSG_FreeFace(face);
		
			lst=BTGE_BrushCSG_SubdivideFace(rface, lst, max);
			lst=BTGE_BrushCSG_SubdivideFace(lface, lst, max);
			return(lst);
		}
	}

	face->next=lst;
	lst=face;
	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_SubdivideFaceList(
	BTGE_BrushTempFace *lst, float max)
{
	BTGE_BrushTempFace *cur, *nxt;

	cur=lst; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		lst=BTGE_BrushCSG_SubdivideFace(cur, lst, max);
		cur=nxt;
	}
	
	return(lst);
}

BTGE_API int BTGE_BrushCSG_InferFaceOutside(
	BTGE_BrushTempFace *face)
{
	float org[3], tv0[3];
	BTGE_BrushWorld *world;
	BTGE_SEntity *sent;
	BTGE_Brush *tlst;
	int i;

	BTGE_BrushCSG_GetFaceOrigin(face, org);

//	V3F_ADDSCALE(org, face->norm, 99999, tv0);

	if(!face->brush)return(0);
	sent=face->brush->se_owner;
	if(!sent)return(0);
	
	//hack: determine if worldspawn
	if(sent->idnum!=1)return(0);
	
	world=sent->wrl;
	if(!world)return(0);

	V3F_ADDSCALE(org, face->norm, 99999, tv0);
	tlst=BTGE_BrushWorld_TraceLineBrush(
		world, org, tv0, face->brush);
	if(!tlst)return(1);

#if 1
	for(i=0; i<6; i++)
	{
		V3F_ADDSCALE(org, face->norm, 99999, tv0);
		tv0[i>>1]+=(i&1)?9999:-9999;
		tlst=BTGE_BrushWorld_TraceLineBrush(
			world, org, tv0, face->brush);
		if(!tlst)return(1);
	}
#endif

	return(0);
}

BTGE_API int BTGE_BrushCSG_CheckFaceBrush(
	BTGE_BrushTempFace *face, BTGE_Brush *brsh)
{
	float f, g;
	int i;

	if(brsh->patch || brsh->mesh)
		return(0);

	BTGE_Brush_GetPlaneExtents(brsh, face->norm, &f, &g);
	if(f>0)return(0);
	if(g<0)return(0);

	for(i=0; i<brsh->n_face; i++)
	{
		BTGE_BrushCSG_FacePlaneExtents(
			face, brsh->norm+i*4, &f, &g);
//		if(g<0)return(0);
		if(f>0)return(0);
	}
	return(1);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceBrush(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	BTGE_Brush *brsh)
{
	BTGE_BrushTempFace *tmp;
	float f, g;
	int i, j, xm;

	//don't clip against self
	if(face->brush==brsh) { face->next=lst; return(face); }

	//don't clip against patches, meshes, or brushes lacking faces
	if(brsh->patch || brsh->mesh || !brsh->n_face)
		{ face->next=lst; return(face); }

	//determine other types of brushes to not clip against
	xm=BTGE_BRFL_DEL|BTGE_BRFL_HIDDEN|BTGE_BRFL_ALPHA|
		BTGE_BRFL_NODRAW|BTGE_BRFL_FLUID|BTGE_BRFL_EFFECT;

	if(brsh->flags&xm)
	{
		i=brsh->flags&face->brush->flags;
		j=BTGE_BRFL_FLUID;

		if(!(i&j))
		{
			//brush does not clip face
			face->next=lst;
			return(face);
		}
	}

	i=BTGE_BrushCSG_CheckFaceBrush(face, brsh);
	if(!i) { face->next=lst; return(face); }

	for(i=0; i<brsh->n_face; i++)
	{	
		BTGE_BrushCSG_FacePlaneExtents(face, brsh->norm+i*4, &f, &g);
		if(g<=0)continue;
		if(f>=0)
		{
			face->next=lst; return(face);
		}

#if 1
		//check for and skip similar planes
		f=V3F_DOT(face->norm, brsh->norm+i*4);
		g=(f>=0)?(face->norm[3]-brsh->norm[i*4+3]):
			(face->norm[3]+brsh->norm[i*4+3]);
//		if((f>=0.99) || (f<=-0.99))
		if((fabs(f)>=0.999) && (fabs(g)<1))
		{
//			printf("BTGE_BrushCSG_ClipFaceBrush: Skip Similar\n");
			continue;
		}
#endif

		lst=BTGE_BrushCSG_RightFacePlane(face, lst, brsh->norm+i*4);
		tmp=BTGE_BrushCSG_LeftFacePlane(face, NULL, brsh->norm+i*4);
		BTGE_BrushCSG_FreeFace(face);
		face=tmp;
		if(!face)break;
	}

	//part inside brush simply discarded
//	BTGE_BrushCSG_FreeFace(face);

	if(face)
	{
		for(i=0; i<brsh->n_face; i++)
		{
			if(V3F_DOT(face->norm, brsh->norm+i*4)<0)
				continue;
		
			BTGE_BrushCSG_FacePlaneExtents(face, brsh->norm+i*4, &f, &g);
//			if((f+0.1)>=0)break;
			if(f>=0)break;
		}
		if(i<brsh->n_face)
//		if(1)
		{
			face->next=lst;
			lst=face;
		}else
		{
			BTGE_BrushCSG_FreeFace(face);
		}
	}

#if 0
	if(face)
	{
		face->next=lst;
		lst=face;
	}
#endif

	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrush(
	BTGE_BrushTempFace *ilst, BTGE_Brush *brsh)
{
	BTGE_BrushTempFace *cur, *nxt, *lst;

	cur=ilst; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		lst=BTGE_BrushCSG_ClipFaceBrush(cur, lst, brsh);
		cur=nxt;
	}
	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrushList(
	BTGE_BrushTempFace *ilst, BTGE_Brush *blst)
{
	BTGE_Brush *cur;

	cur=blst;
	while(cur)
	{
		ilst=BTGE_BrushCSG_ClipFaceListBrush(ilst, cur);
		cur=cur->chain;
	}

	return(ilst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_BuildBrushFacesClip(
	BTGE_Brush *brush, BTGE_BrushNode *root)
{
	float mins[3], maxs[3];
	BTGE_BrushTempFace *flst;
	BTGE_Brush *blst;
	
	mins[0]=brush->mins[0]-1;
	mins[1]=brush->mins[1]-1;
	mins[2]=brush->mins[2]-1;

	maxs[0]=brush->maxs[0]+1;
	maxs[1]=brush->maxs[1]+1;
	maxs[2]=brush->maxs[2]+1;

	blst=BTGE_BrushBSP_BoxQueryTreeNode(root, mins, maxs);
//	blst=BTGE_BrushBSP_BoxQueryTreeNode(root, brush->mins, brush->maxs);
	
	flst=BTGE_BrushCSG_MakeBrushFaces(brush, NULL);
	flst=BTGE_BrushCSG_ClipFaceListBrushList(flst, blst);
	return(flst);
}

//expand CSG:
//this expands brushes by a given amount prior to clipping

BTGE_API float BTGE_BrushCSG_ExpandRadius(float *norm, float *rad)
{
	float f, r;

	if(!rad)return(0);

#if 0
	if(rad[2]<0)
	{
		//special case: -Z means no upward expansion

		f=fabs(norm[2]*rad[2]);
		if(f<0)f=0;

		r=	fabs(norm[0]*rad[0]) +
			fabs(norm[1]*rad[1]) +
			f;
		return(r);
	}
#endif

	r=	fabs(norm[0]*rad[0]) +
		fabs(norm[1]*rad[1]) +
		fabs(norm[2]*rad[2]);
	return(r);
}

BTGE_API int BTGE_BrushCSG_CheckFaceBrushExpand(BTGE_BrushTempFace *face,
	BTGE_Brush *brsh, float *rad)
{
	float f, g, r;
	int i;

	r=BTGE_BrushCSG_ExpandRadius(face->norm, rad);
	BTGE_Brush_GetPlaneExtents(brsh, face->norm, &f, &g);

	if(rad && (rad[2]<0))
	{
		if((f-r)>=0)return(0);
		if(g<=0)return(0);
	}else
	{
		if((f-r)>=0)return(0);
		if((g+r)<=0)return(0);
	}

	for(i=0; i<brsh->n_face; i++)
	{
		r=BTGE_BrushCSG_ExpandRadius(brsh->norm+i*4, rad);
		BTGE_BrushCSG_FacePlaneExtents(
			face, brsh->norm+i*4, &f, &g);

		if(rad && (rad[2]<0) && (brsh->norm[i*4+2]>0.66))
			if(f>=0)return(0);

		if((f-r)>=0)return(0);
	}
	return(1);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceBrushExpand(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	BTGE_Brush *brsh, float *rad)
{
	float norm[4];
	BTGE_BrushTempFace *tmp;
	float f, g, r;
	int i, j;

	if(face->brush==brsh) { face->next=lst; return(face); }

	i=BTGE_BrushCSG_CheckFaceBrushExpand(face, brsh, rad);
	if(!i) { face->next=lst; return(face); }

	for(i=0; i<brsh->n_face; i++)
	{
		norm[0]=brsh->norm[i*4+0];
		norm[1]=brsh->norm[i*4+1];
		norm[2]=brsh->norm[i*4+2];
		norm[3]=brsh->norm[i*4+3];

		r=BTGE_BrushCSG_ExpandRadius(brsh->norm+i*4, rad);
		if(rad && ((rad[2]>=0) || (brsh->norm[i*4+2]<0.66)))
			norm[3]+=r;

		BTGE_BrushCSG_FacePlaneExtents(face, norm, &f, &g);
		if(g<=0)continue;
		if(f>=0) { face->next=lst; return(face); }
		lst=BTGE_BrushCSG_RightFacePlane(face, lst, norm);
		tmp=BTGE_BrushCSG_LeftFacePlane(face, lst, norm);
		BTGE_BrushCSG_FreeFace(face);
		face=tmp;
		if(!face)break;
	}

	//part inside brush simply discarded
	BTGE_BrushCSG_FreeFace(face);

	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrushExpand(
	BTGE_BrushTempFace *ilst, BTGE_Brush *brsh, float *rad)
{
	BTGE_BrushTempFace *cur, *nxt, *lst;

	cur=ilst; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		lst=BTGE_BrushCSG_ClipFaceBrushExpand(cur, lst, brsh, rad);
		cur=nxt;
	}
	return(lst);
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrushListExpand(
	BTGE_BrushTempFace *ilst, BTGE_Brush *blst, float *rad)
{
	BTGE_Brush *cur;

	cur=blst;
	while(cur)
	{
		ilst=BTGE_BrushCSG_ClipFaceListBrushExpand(ilst, cur, rad);
		cur=cur->chain;
	}

	return(ilst);
}


//


BTGE_API void BTGE_BrushCSG_SplitListFacePlane(
	BTGE_BrushTempFace *lst, float *norm,
	BTGE_BrushTempFace **rllst, BTGE_BrushTempFace **rrlst)
{
	BTGE_BrushTempFace *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->next;
		BTGE_BrushCSG_FacePlaneExtents(cur, norm, &f, &g);
		if(g<0) { cur->next=llst; llst=cur; cur=nxt; continue; }
		if(f>0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }

		if((f==0) && (g==0))
		{
			cur->flags|=BTGE_TXFL_NOPLANE;

			f=V3F_DOT(cur->norm, norm);
			if(f<0) { cur->next=llst; llst=cur; cur=nxt; continue; }
			if(f>0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }

			BTGE_BrushCSG_FreeFace(cur);
			cur=nxt;
			continue;
		}

		llst=BTGE_BrushCSG_LeftFacePlane(cur, llst, norm);
		rlst=BTGE_BrushCSG_RightFacePlane(cur, rlst, norm);
		BTGE_BrushCSG_FreeFace(cur);
		cur=nxt;
	}

	*rllst=llst;
	*rrlst=rlst;
}

BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_MergeFaceList(
	BTGE_BrushTempFace *lsta, BTGE_BrushTempFace *lstb)
{
	BTGE_BrushTempFace *cur;

	if(!lsta)return(lstb);
	if(!lstb)return(lsta);

	cur=lsta;
	while(cur->next)cur=cur->next;
	cur->next=lstb;
	return(lsta);
}


BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_BindFaceListNode(
	BTGE_BrushNode *root, BTGE_BrushNode *node,
	BTGE_BrushTempFace *ilst)
{
	BTGE_BrushTempFace *cur, *llst, *rlst, *nxt;

	node->face=NULL;
	if(!node->lnode)
	{
		cur=ilst;
		while(cur)
		{
			cur->nnext=node->face;
			node->face=cur;
			cur=cur->next;
		}
		return(ilst);
	}

	BTGE_BrushCSG_SplitListFacePlane(ilst, node->norm, &llst, &rlst);
	llst=BTGE_BrushCSG_BindFaceListNode(root, node->lnode, llst);
	rlst=BTGE_BrushCSG_BindFaceListNode(root, node->rnode, rlst);
	cur=BTGE_BrushCSG_MergeFaceList(llst, rlst);
	return(cur);
}


BTGE_API BTGE_BrushTempFace *BTGE_BrushCSG_BuildFaces(BTGE_BrushNode *root)
{
	BTGE_BrushTempFace *lst, *cur, *tlst;
	BTGE_Brush *blst;
	int i;

	printf("Build CSG Faces\n");

	blst=BTGE_BrushBSP_GetBrushList(root);
	lst=BTGE_BrushCSG_MakeFacesBrushList(blst);
//	lst=BTGE_BrushCSG_ClipFaceListBrushList(lst, blst);

	lst=BTGE_BrushCSG_BindFaceListNode(root, root, lst);

	cur=lst; i=0;
	while(cur) { i++; cur=cur->next; }

	printf("Build CSG Faces: OK, %d faces\n", i);

	return(lst);
}
