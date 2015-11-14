#include <btgesv.h>

BTGE_BrushTempFace *lbxgl_face_list=NULL;

BTGE_BrushTempFace *BTGE_BrushCSG_AllocFace()
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

void BTGE_BrushCSG_FreeFace(BTGE_BrushTempFace *face)
{
	if(!face)return;

	if(face->xyz)gcfree(face->xyz);
	if(face->st)gcfree(face->st);
	if(face->evec)gcfree(face->evec);

	face->xyz=NULL;
	face->st=NULL;
	face->evec=NULL;

//	gcfree(face);

	face->next=lbxgl_face_list;
	lbxgl_face_list=face;
}

void BTGE_BrushCSG_FreeFaceList(BTGE_BrushTempFace *face)
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

BTGE_BrushTempFace *BTGE_BrushCSG_MakeBrushFaces(
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

		fv=brsh->norm+brsh->pf_plane[i]*4;
		V4F_COPY(fv, tmp->norm);

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

BTGE_BrushTempFace *BTGE_BrushCSG_MakeFacesBrushList(BTGE_Brush *fst)
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

BTGE_BrushTempFace *BTGE_BrushCSG_LeftFacePlane(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	float *norm)
{
	float xyz[64*3], st[64*2];
	BTGE_BrushTempFace *tmp;
	int i, l;

	l=Hull_ClipFaceST(norm, face->xyz, face->st, xyz, st, face->n_vec);
//	l=Hull_ClipFace(norm, face->xyz, xyz, face->n_vec);

	if(l<3)return(NULL);

	tmp=BTGE_BrushCSG_AllocFace();

	tmp->xyz=gcatomic(l*3*sizeof(float));
	tmp->st=gcatomic(l*2*sizeof(float));
	for(i=0; i<(3*l); i++)tmp->xyz[i]=xyz[i];
	for(i=0; i<(2*l); i++)tmp->st[i]=st[i];
	tmp->n_vec=l;

	V4F_COPY(face->norm, tmp->norm);
	tmp->tex=face->tex;
	tmp->flags=face->flags;
	tmp->brush=face->brush;

	tmp->next=lst;
	return(tmp);
}

BTGE_BrushTempFace *BTGE_BrushCSG_RightFacePlane(
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

BTGE_BrushTempFace *BTGE_BrushCSG_ChainLeftFacePlane(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	float *norm)
{
	BTGE_BrushTempFace *tmp;

	tmp=BTGE_BrushCSG_LeftFacePlane(face, NULL, norm);
	tmp->chain=lst;
	return(tmp);
}

BTGE_BrushTempFace *BTGE_BrushCSG_ChainRightFacePlane(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	float *norm)
{
	BTGE_BrushTempFace *tmp;

	tmp=BTGE_BrushCSG_RightFacePlane(face, NULL, norm);
	tmp->chain=lst;
	return(tmp);
}


void BTGE_BrushCSG_FaceExtents(BTGE_BrushTempFace *face,
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

void BTGE_BrushCSG_FacePlaneExtents(BTGE_BrushTempFace *face,
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

void BTGE_BrushCSG_BBoxFace(BTGE_BrushTempFace *face, float *min, float *max)
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

void BTGE_BrushCSG_BBoxFaceList(BTGE_BrushTempFace *lst,
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

void BTGE_BrushCSG_GetFaceOrigin(BTGE_BrushTempFace *face, float *org)
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

int BTGE_BrushCSG_CheckFaceBrush(BTGE_BrushTempFace *face,
	BTGE_Brush *brsh)
{
	float f, g;
	int i;

	BTGE_Brush_GetPlaneExtents(brsh, face->norm, &f, &g);
	if(f>0)return(0);
	if(g<0)return(0);

	for(i=0; i<brsh->n_face; i++)
	{
		BTGE_BrushCSG_FacePlaneExtents(
			face, brsh->norm+i*4, &f, &g);
		if(f>0)return(0);
	}
	return(1);
}

BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceBrush(
	BTGE_BrushTempFace *face, BTGE_BrushTempFace *lst,
	BTGE_Brush *brsh)
{
	BTGE_BrushTempFace *tmp;
	float f, g;
	int i, j;

	if(face->brush==brsh) { face->next=lst; return(face); }

	i=BTGE_BrushCSG_CheckFaceBrush(face, brsh);
	if(!i) { face->next=lst; return(face); }

	for(i=0; i<brsh->n_face; i++)
	{
		BTGE_BrushCSG_FacePlaneExtents(face, brsh->norm+i*4, &f, &g);
		if(g<=0)continue;
		if(f>0) { face->next=lst; return(face); }
		lst=BTGE_BrushCSG_RightFacePlane(face, lst, brsh->norm+i*4);
		tmp=BTGE_BrushCSG_LeftFacePlane(face, lst, brsh->norm+i*4);
		BTGE_BrushCSG_FreeFace(face);
		face=tmp;
		if(!face)break;
	}

	//part inside brush simply discarded
	BTGE_BrushCSG_FreeFace(face);

	return(lst);
}

BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrush(
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

BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrushList(
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


//expand CSG:
//this expands brushes by a given amount prior to clipping

float BTGE_BrushCSG_ExpandRadius(float *norm, float *rad)
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

int BTGE_BrushCSG_CheckFaceBrushExpand(BTGE_BrushTempFace *face,
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

BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceBrushExpand(
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

BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrushExpand(
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

BTGE_BrushTempFace *BTGE_BrushCSG_ClipFaceListBrushListExpand(
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


void BTGE_BrushCSG_SplitListFacePlane(
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

BTGE_BrushTempFace *BTGE_BrushCSG_MergeFaceList(
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


BTGE_BrushTempFace *BTGE_BrushCSG_BindFaceListNode(
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


BTGE_BrushTempFace *BTGE_BrushCSG_BuildFaces(BTGE_BrushNode *root)
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
