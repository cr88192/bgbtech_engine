#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

LBXGL_BrushTempFace *lbxgl_face_list=NULL;

LBXGL_BrushTempFace *LBXGL_BrushCSG_AllocFace()
{
	LBXGL_BrushTempFace *tmp;

	if(lbxgl_face_list)
	{
		tmp=lbxgl_face_list;
		lbxgl_face_list=tmp->next;

		memset(tmp, 0, sizeof(LBXGL_BrushTempFace));
		return(tmp);
	}

	tmp=gcalloc(sizeof(LBXGL_BrushTempFace));
	return(tmp);
}

void LBXGL_BrushCSG_FreeFace(LBXGL_BrushTempFace *face)
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

void LBXGL_BrushCSG_FreeFaceList(LBXGL_BrushTempFace *face)
{
	LBXGL_BrushTempFace *cur, *nxt;

	cur=face;
	while(cur)
	{
		nxt=cur->next;
		LBXGL_BrushCSG_FreeFace(cur);
		cur=nxt;
	}
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_MakeBrushFaces(
	LBXGL_Brush *brsh, LBXGL_BrushTempFace *lst)
{
	LBXGL_BrushTempFace *tmp;
	float *fv;
	int i, j, k, l;

	for(i=0; i<brsh->n_poly; i++)
	{
		tmp=LBXGL_BrushCSG_AllocFace();
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

LBXGL_BrushTempFace *LBXGL_BrushCSG_MakeFacesBrushList(LBXGL_Brush *fst)
{
	LBXGL_BrushTempFace *lst;
	LBXGL_Brush *cur;

	cur=fst; lst=NULL;
	while(cur)
	{
		lst=LBXGL_BrushCSG_MakeBrushFaces(cur, lst);
		cur=cur->chain;
	}

	return(lst);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_LeftFacePlane(
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *lst,
	float *norm)
{
	float xyz[64*3], st[64*2];
	LBXGL_BrushTempFace *tmp;
	int i, l;

	l=Hull_ClipFaceST(norm, face->xyz, face->st, xyz, st, face->n_vec);
//	l=Hull_ClipFace(norm, face->xyz, xyz, face->n_vec);

	if(l<3)return(NULL);

	tmp=LBXGL_BrushCSG_AllocFace();

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

LBXGL_BrushTempFace *LBXGL_BrushCSG_RightFacePlane(
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *lst,
	float *norm)
{
	float norm1[4];
	LBXGL_BrushTempFace *tmp;

	norm1[0]=-norm[0]; norm1[1]=-norm[1];
	norm1[2]=-norm[2]; norm1[3]=-norm[3];

	tmp=LBXGL_BrushCSG_LeftFacePlane(face, lst, norm1);
	return(tmp);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ChainLeftFacePlane(
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *lst,
	float *norm)
{
	LBXGL_BrushTempFace *tmp;

	tmp=LBXGL_BrushCSG_LeftFacePlane(face, NULL, norm);
	tmp->chain=lst;
	return(tmp);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ChainRightFacePlane(
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *lst,
	float *norm)
{
	LBXGL_BrushTempFace *tmp;

	tmp=LBXGL_BrushCSG_RightFacePlane(face, NULL, norm);
	tmp->chain=lst;
	return(tmp);
}


void LBXGL_BrushCSG_FaceExtents(LBXGL_BrushTempFace *face,
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

void LBXGL_BrushCSG_FacePlaneExtents(LBXGL_BrushTempFace *face,
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

void LBXGL_BrushCSG_BBoxFace(LBXGL_BrushTempFace *face, float *min, float *max)
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

void LBXGL_BrushCSG_BBoxFaceList(LBXGL_BrushTempFace *lst,
	float *min, float *max)
{
	LBXGL_BrushTempFace *cur;

	min[0]= 999999; min[1]= 999999; min[2]= 999999;
	max[0]=-999999; max[1]=-999999; max[2]=-999999;

	cur=lst;
	while(cur)
	{
		LBXGL_BrushCSG_BBoxFace(cur, min, max);
		cur=cur->next;
	}
}

void LBXGL_BrushCSG_GetFaceOrigin(LBXGL_BrushTempFace *face, float *org)
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

int LBXGL_BrushCSG_CheckFaceBrush(LBXGL_BrushTempFace *face,
	LBXGL_Brush *brsh)
{
	float f, g;
	int i;

	LBXGL_Brush_GetPlaneExtents(brsh, face->norm, &f, &g);
	if(f>0)return(0);
	if(g<0)return(0);

	for(i=0; i<brsh->n_face; i++)
	{
		LBXGL_BrushCSG_FacePlaneExtents(
			face, brsh->norm+i*4, &f, &g);
		if(f>0)return(0);
	}
	return(1);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ClipFaceBrush(
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *lst,
	LBXGL_Brush *brsh)
{
	LBXGL_BrushTempFace *tmp;
	float f, g;
	int i, j;

	if(face->brush==brsh) { face->next=lst; return(face); }

	i=LBXGL_BrushCSG_CheckFaceBrush(face, brsh);
	if(!i) { face->next=lst; return(face); }

	for(i=0; i<brsh->n_face; i++)
	{
		LBXGL_BrushCSG_FacePlaneExtents(face, brsh->norm+i*4, &f, &g);
		if(g<=0)continue;
		if(f>0) { face->next=lst; return(face); }
		lst=LBXGL_BrushCSG_RightFacePlane(face, lst, brsh->norm+i*4);
		tmp=LBXGL_BrushCSG_LeftFacePlane(face, lst, brsh->norm+i*4);
		LBXGL_BrushCSG_FreeFace(face);
		face=tmp;
		if(!face)break;
	}

	//part inside brush simply discarded
	LBXGL_BrushCSG_FreeFace(face);

	return(lst);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ClipFaceListBrush(
	LBXGL_BrushTempFace *ilst, LBXGL_Brush *brsh)
{
	LBXGL_BrushTempFace *cur, *nxt, *lst;

	cur=ilst; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		lst=LBXGL_BrushCSG_ClipFaceBrush(cur, lst, brsh);
		cur=nxt;
	}
	return(lst);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ClipFaceListBrushList(
	LBXGL_BrushTempFace *ilst, LBXGL_Brush *blst)
{
	LBXGL_Brush *cur;

	cur=blst;
	while(cur)
	{
		ilst=LBXGL_BrushCSG_ClipFaceListBrush(ilst, cur);
		cur=cur->chain;
	}

	return(ilst);
}


//expand CSG:
//this expands brushes by a given amount prior to clipping

float LBXGL_BrushCSG_ExpandRadius(float *norm, float *rad)
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

int LBXGL_BrushCSG_CheckFaceBrushExpand(LBXGL_BrushTempFace *face,
	LBXGL_Brush *brsh, float *rad)
{
	float f, g, r;
	int i;

	r=LBXGL_BrushCSG_ExpandRadius(face->norm, rad);
	LBXGL_Brush_GetPlaneExtents(brsh, face->norm, &f, &g);

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
		r=LBXGL_BrushCSG_ExpandRadius(brsh->norm+i*4, rad);
		LBXGL_BrushCSG_FacePlaneExtents(
			face, brsh->norm+i*4, &f, &g);

		if(rad && (rad[2]<0) && (brsh->norm[i*4+2]>0.66))
			if(f>=0)return(0);

		if((f-r)>=0)return(0);
	}
	return(1);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ClipFaceBrushExpand(
	LBXGL_BrushTempFace *face, LBXGL_BrushTempFace *lst,
	LBXGL_Brush *brsh, float *rad)
{
	float norm[4];
	LBXGL_BrushTempFace *tmp;
	float f, g, r;
	int i, j;

	if(face->brush==brsh) { face->next=lst; return(face); }

	i=LBXGL_BrushCSG_CheckFaceBrushExpand(face, brsh, rad);
	if(!i) { face->next=lst; return(face); }

	for(i=0; i<brsh->n_face; i++)
	{
		norm[0]=brsh->norm[i*4+0];
		norm[1]=brsh->norm[i*4+1];
		norm[2]=brsh->norm[i*4+2];
		norm[3]=brsh->norm[i*4+3];

		r=LBXGL_BrushCSG_ExpandRadius(brsh->norm+i*4, rad);
		if(rad && ((rad[2]>=0) || (brsh->norm[i*4+2]<0.66)))
			norm[3]+=r;

		LBXGL_BrushCSG_FacePlaneExtents(face, norm, &f, &g);
		if(g<=0)continue;
		if(f>=0) { face->next=lst; return(face); }
		lst=LBXGL_BrushCSG_RightFacePlane(face, lst, norm);
		tmp=LBXGL_BrushCSG_LeftFacePlane(face, lst, norm);
		LBXGL_BrushCSG_FreeFace(face);
		face=tmp;
		if(!face)break;
	}

	//part inside brush simply discarded
	LBXGL_BrushCSG_FreeFace(face);

	return(lst);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ClipFaceListBrushExpand(
	LBXGL_BrushTempFace *ilst, LBXGL_Brush *brsh, float *rad)
{
	LBXGL_BrushTempFace *cur, *nxt, *lst;

	cur=ilst; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		lst=LBXGL_BrushCSG_ClipFaceBrushExpand(cur, lst, brsh, rad);
		cur=nxt;
	}
	return(lst);
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_ClipFaceListBrushListExpand(
	LBXGL_BrushTempFace *ilst, LBXGL_Brush *blst, float *rad)
{
	LBXGL_Brush *cur;

	cur=blst;
	while(cur)
	{
		ilst=LBXGL_BrushCSG_ClipFaceListBrushExpand(ilst, cur, rad);
		cur=cur->chain;
	}

	return(ilst);
}


//


void LBXGL_BrushCSG_SplitListFacePlane(
	LBXGL_BrushTempFace *lst, float *norm,
	LBXGL_BrushTempFace **rllst, LBXGL_BrushTempFace **rrlst)
{
	LBXGL_BrushTempFace *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->next;
		LBXGL_BrushCSG_FacePlaneExtents(cur, norm, &f, &g);
		if(g<0) { cur->next=llst; llst=cur; cur=nxt; continue; }
		if(f>0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }

		if((f==0) && (g==0))
		{
			cur->flags|=LBXGL_TXFL_NOPLANE;

			f=V3F_DOT(cur->norm, norm);
			if(f<0) { cur->next=llst; llst=cur; cur=nxt; continue; }
			if(f>0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }

			LBXGL_BrushCSG_FreeFace(cur);
			cur=nxt;
			continue;
		}

		llst=LBXGL_BrushCSG_LeftFacePlane(cur, llst, norm);
		rlst=LBXGL_BrushCSG_RightFacePlane(cur, rlst, norm);
		LBXGL_BrushCSG_FreeFace(cur);
		cur=nxt;
	}

	*rllst=llst;
	*rrlst=rlst;
}

LBXGL_BrushTempFace *LBXGL_BrushCSG_MergeFaceList(
	LBXGL_BrushTempFace *lsta, LBXGL_BrushTempFace *lstb)
{
	LBXGL_BrushTempFace *cur;

	if(!lsta)return(lstb);
	if(!lstb)return(lsta);

	cur=lsta;
	while(cur->next)cur=cur->next;
	cur->next=lstb;
	return(lsta);
}


LBXGL_BrushTempFace *LBXGL_BrushCSG_BindFaceListNode(
	LBXGL_BrushNode *root, LBXGL_BrushNode *node,
	LBXGL_BrushTempFace *ilst)
{
	LBXGL_BrushTempFace *cur, *llst, *rlst, *nxt;

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

	LBXGL_BrushCSG_SplitListFacePlane(ilst, node->norm, &llst, &rlst);
	llst=LBXGL_BrushCSG_BindFaceListNode(root, node->lnode, llst);
	rlst=LBXGL_BrushCSG_BindFaceListNode(root, node->rnode, rlst);
	cur=LBXGL_BrushCSG_MergeFaceList(llst, rlst);
	return(cur);
}


LBXGL_BrushTempFace *LBXGL_BrushCSG_BuildFaces(LBXGL_BrushNode *root)
{
	LBXGL_BrushTempFace *lst, *cur, *tlst;
	LBXGL_Brush *blst;
	int i;

	printf("Build CSG Faces\n");

	blst=LBXGL_BrushBSP_GetBrushList(root);
	lst=LBXGL_BrushCSG_MakeFacesBrushList(blst);
//	lst=LBXGL_BrushCSG_ClipFaceListBrushList(lst, blst);

	lst=LBXGL_BrushCSG_BindFaceListNode(root, root, lst);

	cur=lst; i=0;
	while(cur) { i++; cur=cur->next; }

	printf("Build CSG Faces: OK, %d faces\n", i);

	return(lst);
}


void LBXGL_BrushBSP_DrawFacesNodeFlat(LBXGL_BrushNode *node)
{
	LBXGL_BrushTempFace *cur;
	float f;
	int i, j;

	if(node->lnode)
	{
		f=V3F_NDOT(lbxgl_cam->org, node->norm);
		if(f<=0)
		{
			LBXGL_BrushBSP_DrawFacesNodeFlat(node->lnode);
			LBXGL_BrushBSP_DrawFacesNodeFlat(node->rnode);
		}else
		{
			LBXGL_BrushBSP_DrawFacesNodeFlat(node->rnode);
			LBXGL_BrushBSP_DrawFacesNodeFlat(node->lnode);
		}
		return;
	}

	cur=node->face; i=0;
	while(cur)
	{
		pdglNormal3fv(cur->norm);
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<cur->n_vec; j++)
			pdglVertex3fv(cur->xyz+j*3);
		pdglEnd();

		i++;
		cur=cur->nnext;
	}
}

void LBXGL_BrushBSP_DrawFacesNodeFlatSphere(LBXGL_BrushNode *node,
	float *org, float rad)
{
	LBXGL_BrushTempFace *cur;
	float f;
	int i, j;

	if(node->lnode)
	{
		f=V3F_NDOT(org, node->norm);

		if(f<=(-rad))
		{
			LBXGL_BrushBSP_DrawFacesNodeFlatSphere(
				node->lnode, org, rad);
			return;
		}
		if(f>=rad)
		{
			LBXGL_BrushBSP_DrawFacesNodeFlatSphere(
				node->rnode, org, rad);
			return;
		}

		if(f<=0)
		{
			LBXGL_BrushBSP_DrawFacesNodeFlatSphere(
				node->lnode, org, rad);
			LBXGL_BrushBSP_DrawFacesNodeFlatSphere(
				node->rnode, org, rad);
		}else
		{
			LBXGL_BrushBSP_DrawFacesNodeFlatSphere(
				node->rnode, org, rad);
			LBXGL_BrushBSP_DrawFacesNodeFlatSphere(
				node->lnode, org, rad);
		}
		return;
	}

	cur=node->face; i=0;
	while(cur)
	{
		pdglNormal3fv(cur->norm);
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<cur->n_vec; j++)
			pdglVertex3fv(cur->xyz+j*3);
		pdglEnd();

		i++;
		cur=cur->nnext;
	}
}

void LBXGL_BrushBSP_DrawNodeFacesShadow(LBXGL_BrushNode *node)
{
	LBXGL_BrushTempFace *cur;
	float f;
	int i, j;

	if(node->lnode)
	{
		f=V3F_NDOT(lbxgl_cam->org, node->norm);
		if(f<=0)
		{
			LBXGL_BrushBSP_DrawNodeFacesShadow(node->lnode);
			LBXGL_BrushBSP_DrawNodeFacesShadow(node->rnode);
		}else
		{
			LBXGL_BrushBSP_DrawNodeFacesShadow(node->rnode);
			LBXGL_BrushBSP_DrawNodeFacesShadow(node->lnode);
		}
		return;
	}

	pdglDisableTexture2D();
	pdglColor4f(0, 0, 0, 1);

	cur=node->face; i=0;
	while(cur)
	{
//		pdglBegin(PDGL_POLYGON);
//		for(j=0; j<cur->n_vec; j++)
//			pdglVertex3fv(cur->xyz+j*3);
//		pdglEnd();

		pdglBlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_BindTexture(cur->tex);
		LBXGL_Shader_Normal3fv(cur->norm);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		LBXGL_Shader_BeginPoly();
		for(j=0; j<cur->n_vec; j++)
		{
			LBXGL_Shader_TexCoord2fv(cur->st+j*2);
			LBXGL_Shader_Vertex3fv(cur->xyz+j*3);
		}
		LBXGL_Shader_EndPoly();

		i++;
		cur=cur->nnext;
	}
}


void LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(LBXGL_BrushNode *node,
	float *org, float rad)
{
	static float tva[16*3];
	static float tvb[16*3];
	float tv[3];

	LBXGL_BrushTempFace *cur;
	float f;
	int i, j, k, l;

	if(node->lnode)
	{
		f=V3F_NDOT(org, node->norm);

		if(f<=(-rad))
		{
			LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(
				node->lnode, org, rad);
			return;
		}
		if(f>=rad)
		{
			LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(
				node->rnode, org, rad);
			return;
		}

		if(f<=0)
		{
			LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(
				node->lnode, org, rad);
			LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(
				node->rnode, org, rad);
		}else
		{
			LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(
				node->rnode, org, rad);
			LBXGL_BrushBSP_DrawVolFacesNodeFlatSphere(
				node->lnode, org, rad);
		}
		return;
	}

	cur=node->face; i=0;
	while(cur)
	{
		l=cur->n_vec;

		f=V3F_NDOT(org, cur->norm);
		if(f==0)continue;

		if(f>0)
		{
			for(j=0; j<l; j++)
			{
				V3F_COPY(cur->xyz+j*3, tva+j*3);
				V3F_SUB(tva+j*3, org, tv);
				V3F_ADDSCALE(tva+j*3, tv, 1000, tvb+j*3);
			}
			V3F_COPY(tva, tva+j*3);
			V3F_COPY(tvb, tvb+j*3);
		}else
		{
			for(j=0; j<l; j++)
			{
				V3F_COPY(cur->xyz+(l-j-1)*3, tva+j*3);
				V3F_SUB(tva+j*3, org, tv);
				V3F_ADDSCALE(tva+j*3, tv, 1000, tvb+j*3);
			}
			V3F_COPY(tva, tva+j*3);
			V3F_COPY(tvb, tvb+j*3);
		}


		pdglBegin(PDGL_POLYGON);
		for(j=0; j<l; j++)
			pdglVertex3fv(tva+j*3);
		pdglEnd();

		pdglBegin(PDGL_QUADS);
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(tva+(j+1)*3);
			pdglVertex3fv(tva+j*3);
			pdglVertex3fv(tvb+j*3);
			pdglVertex3fv(tvb+(j+1)*3);
		}
		pdglEnd();

		pdglBegin(PDGL_POLYGON);
		for(j=l-1; j>=0; j--)
			pdglVertex3fv(tvb+j*3);
		pdglEnd();

		i++;
		cur=cur->nnext;
	}
}
