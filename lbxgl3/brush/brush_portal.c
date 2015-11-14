#include <lbxgl2.h>

LBXGL_BrushTempPortal *lbxgl_prt_list=NULL;

LBXGL_Brush *LBXGL_BrushBSP_GetNodeBrushListR(
	LBXGL_BrushNode *node, LBXGL_Brush *lst)
{
	LBXGL_Brush *cur;

	if(!node)return(NULL);
	cur=node->mid;
	while(cur) { cur->chain=lst; lst=cur; cur=cur->nnext; }
	lst=LBXGL_BrushBSP_GetNodeBrushListR(node->lnode, lst);
	lst=LBXGL_BrushBSP_GetNodeBrushListR(node->rnode, lst);
	return(lst);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_AllocPortal()
{
	LBXGL_BrushTempPortal *tmp;

	if(lbxgl_prt_list)
	{
		tmp=lbxgl_prt_list;
		lbxgl_prt_list=tmp->next;

		memset(tmp, 0, sizeof(LBXGL_BrushTempPortal));
		return(tmp);
	}

	tmp=gcalloc(sizeof(LBXGL_BrushTempPortal));
	return(tmp);
}

void LBXGL_BrushBSP_FreePortal(LBXGL_BrushTempPortal *prt)
{
	prt->next=lbxgl_prt_list;
	lbxgl_prt_list=prt;
}

void LBXGL_BrushBSP_PortalExtents(LBXGL_BrushTempPortal *prt,
	float *dir, float *rm, float *rn)
{
	float f, g, m, n;
	int i;

	m=999999; n=-999999;
	for(i=0; i<prt->n_pts; i++)
	{
		f=V3F_DOT(prt->pts+i*3, dir);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
}

void LBXGL_BrushBSP_PortalPlaneExtents(LBXGL_BrushTempPortal *prt,
	float *norm, float *rm, float *rn)
{
	float f, g, m, n;
	int i;

	m=999999; n=-999999;
	for(i=0; i<prt->n_pts; i++)
	{
		f=V3F_NDOT(prt->pts+i*3, norm);
		if(f<m)m=f; if(f>n)n=f;
	}
	if(rm)*rm=m; if(rn)*rn=n;
}

int LBXGL_BrushBSP_CheckPortalBrush(LBXGL_BrushTempPortal *prt,
	LBXGL_Brush *brsh)
{
	float f, g;
	int i;

	LBXGL_Brush_GetPlaneExtents(brsh, prt->norm, &f, &g);
	if(f>0)return(0);
	if(g<0)return(0);

	for(i=0; i<brsh->n_face; i++)
	{
		LBXGL_BrushBSP_PortalPlaneExtents(
			prt, brsh->norm+i*4, &f, &g);
		if(f>0)return(0);
	}
	return(1);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_ClipPortalBrush(
	LBXGL_BrushTempPortal *prt, LBXGL_BrushTempPortal *lst,
	LBXGL_Brush *brsh)
{
	LBXGL_BrushTempPortal *tmp;
	float f, g;
	int i, j;

	i=LBXGL_BrushBSP_CheckPortalBrush(prt, brsh);
	if(!i) { prt->next=lst; return(prt); }

	for(i=0; i<brsh->n_face; i++)
	{
		LBXGL_BrushBSP_PortalPlaneExtents(
			prt, brsh->norm+i*4, &f, &g);
		if(g<=0)continue;

//		j=LBXGL_BrushBSP_CheckPortalBrush(prt, brsh);
//		if(!j) { prt->next=lst; return(prt); }

		if(f>0) { prt->next=lst; return(prt); }

		lst=LBXGL_BrushBSP_RightPortalPlane(
			prt, lst, brsh->norm+i*4);
		tmp=LBXGL_BrushBSP_LeftPortalPlane(
			prt, lst, brsh->norm+i*4);
		LBXGL_BrushBSP_FreePortal(prt);
		prt=tmp;
	}

//	j=LBXGL_BrushBSP_CheckPortalBrush(prt, brsh);
//	if(!j) { prt->next=lst; return(prt); }

	//part inside brush simply discarded
	LBXGL_BrushBSP_FreePortal(prt);

	return(lst);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_ClipPortalListBrush(
	LBXGL_BrushTempPortal *prt, LBXGL_Brush *brsh)
{
	LBXGL_BrushTempPortal *cur, *nxt, *lst;

	cur=prt; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		lst=LBXGL_BrushBSP_ClipPortalBrush(cur, lst, brsh);
		cur=nxt;
	}
	return(lst);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_SplitPortalPlane(
	LBXGL_BrushTempPortal *prt, LBXGL_BrushTempPortal *lst,
	float *norm)
{
	float norm1[4];
	LBXGL_BrushTempPortal *tmp0, *tmp1;
	int i, l;

	norm1[0]=-norm[0]; norm1[1]=-norm[1];
	norm1[2]=-norm[2]; norm1[3]=-norm[3];

	tmp0=LBXGL_BrushBSP_AllocPortal();
	tmp1=LBXGL_BrushBSP_AllocPortal();

	tmp0->n_pts=Hull_ClipFace(norm, prt->pts, tmp0->pts, prt->n_pts);
	tmp1->n_pts=Hull_ClipFace(norm1, prt->pts, tmp1->pts, prt->n_pts);

	V4F_COPY(prt->norm, tmp0->norm);
	V4F_COPY(prt->norm, tmp1->norm);

	tmp0->next=tmp1;
	tmp1->next=lst;
	return(tmp0);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_LeftPortalPlane(
	LBXGL_BrushTempPortal *prt, LBXGL_BrushTempPortal *lst,
	float *norm)
{
	LBXGL_BrushTempPortal *tmp;
	int i, l;

	tmp=LBXGL_BrushBSP_AllocPortal();
	tmp->n_pts=Hull_ClipFace(norm, prt->pts, tmp->pts, prt->n_pts);
	V4F_COPY(prt->norm, tmp->norm);

	tmp->next=lst;
	return(tmp);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_RightPortalPlane(
	LBXGL_BrushTempPortal *prt, LBXGL_BrushTempPortal *lst,
	float *norm)
{
	float norm1[4];
	LBXGL_BrushTempPortal *tmp;
	int i, l;

	norm1[0]=-norm[0]; norm1[1]=-norm[1];
	norm1[2]=-norm[2]; norm1[3]=-norm[3];

	tmp=LBXGL_BrushBSP_AllocPortal();
	tmp->n_pts=Hull_ClipFace(norm1, prt->pts, tmp->pts, prt->n_pts);
	V4F_COPY(prt->norm, tmp->norm);

	tmp->next=lst;
	return(tmp);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_SplitPortalListPlane(
	LBXGL_BrushTempPortal *prt, float *norm)
{
	LBXGL_BrushTempPortal *cur, *lst, *nxt;
	float f, g;

	cur=prt; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		LBXGL_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if((f>=0) || (g<=0))
			{ cur->next=lst; lst=cur; cur=nxt; continue; }
		lst=LBXGL_BrushBSP_SplitPortalPlane(cur, lst, norm);
		LBXGL_BrushBSP_FreePortal(cur);
		cur=nxt;
	}
	return(lst);
}

void LBXGL_BrushBSP_SplitListPortalPlaneLeft(
	LBXGL_BrushTempPortal *lst, float *norm,
	LBXGL_BrushTempPortal **rllst, LBXGL_BrushTempPortal **rrlst)
{
	LBXGL_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->next;
		LBXGL_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(g<=0) { cur->next=llst; llst=cur; cur=nxt; continue; }
		if(f>=0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }
		llst=LBXGL_BrushBSP_LeftPortalPlane(cur, llst, norm);
		rlst=LBXGL_BrushBSP_RightPortalPlane(cur, rlst, norm);
		LBXGL_BrushBSP_FreePortal(cur);
		cur=nxt;
	}

	*rllst=llst;
	*rrlst=rlst;
}

void LBXGL_BrushBSP_SplitListPortalPlaneRight(
	LBXGL_BrushTempPortal *lst, float *norm,
	LBXGL_BrushTempPortal **rllst, LBXGL_BrushTempPortal **rrlst)
{
	LBXGL_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->next;
		LBXGL_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(f>=0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }
		if(g<=0) { cur->next=llst; llst=cur; cur=nxt; continue; }
		llst=LBXGL_BrushBSP_LeftPortalPlane(cur, llst, norm);
		rlst=LBXGL_BrushBSP_RightPortalPlane(cur, rlst, norm);
		LBXGL_BrushBSP_FreePortal(cur);
		cur=nxt;
	}

	*rllst=llst;
	*rrlst=rlst;
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_MergePortalList(
	LBXGL_BrushTempPortal *lsta, LBXGL_BrushTempPortal *lstb)
{
	LBXGL_BrushTempPortal *cur;

	if(!lsta)return(lstb);
	if(!lstb)return(lsta);

	cur=lsta;
	while(cur->next)cur=cur->next;
	cur->next=lstb;
	return(lsta);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_BuildPortalListNode(
	LBXGL_BrushNode *root, LBXGL_BrushNode *node,
	LBXGL_BrushTempPortal *lst)
{
	static float pts0[16*3];
	static float pts1[16*3];
	float tnorm[4];
	LBXGL_BrushTempPortal *tmp;
	LBXGL_BrushNode *ncur;
	int i, j, k, l;

	if(!node->lnode)return(lst);

	Hull_MakePlaneFace(node->norm, pts0);
	l=4;

	ncur=node;
	while(ncur->pnode)
	{
		if(ncur==ncur->pnode->lnode)
		{
			l=Hull_ClipFace(ncur->pnode->norm, pts0, pts1, l);
			for(j=0; j<(l*3); j++)pts0[j]=pts1[j];
		}else if(ncur==ncur->pnode->rnode)
		{
			V4F_SCALE(ncur->pnode->norm, -1, tnorm);
			l=Hull_ClipFace(tnorm, pts0, pts1, l);
			for(j=0; j<(l*3); j++)pts0[j]=pts1[j];
		}else l=0;
		ncur=ncur->pnode;
	}

	if(l)
	{
		tmp=LBXGL_BrushBSP_AllocPortal();
		V4F_COPY(node->norm, tmp->norm);
		for(i=0; i<(l*3); i++)tmp->pts[i]=pts0[i];
		tmp->n_pts=l; tmp->next=lst; lst=tmp;
	}

	lst=LBXGL_BrushBSP_BuildPortalListNode(root, node->lnode, lst);
	lst=LBXGL_BrushBSP_BuildPortalListNode(root, node->rnode, lst);
	return(lst);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_ClipPortalListNodeLeft(
	LBXGL_BrushNode *root, LBXGL_BrushNode *node,
	LBXGL_BrushTempPortal *ilst)
{
	LBXGL_BrushTempPortal *tmp, *llst, *rlst;
	LBXGL_Brush *bcur;

	if(!node->lnode)return(ilst);

#if 1
	bcur=node->mid;
	while(bcur)
	{
		ilst=LBXGL_BrushBSP_ClipPortalListBrush(ilst, bcur);
		bcur=bcur->nnext;
	}
#endif

	LBXGL_BrushBSP_SplitListPortalPlaneLeft(ilst, node->norm, &llst, &rlst);
	llst=LBXGL_BrushBSP_ClipPortalListNodeLeft(root, node->lnode, llst);
	rlst=LBXGL_BrushBSP_ClipPortalListNodeLeft(root, node->rnode, rlst);
	tmp=LBXGL_BrushBSP_MergePortalList(llst, rlst);
	return(tmp);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_ClipPortalListNodeRight(
	LBXGL_BrushNode *root, LBXGL_BrushNode *node,
	LBXGL_BrushTempPortal *ilst)
{
	LBXGL_BrushTempPortal *tmp, *llst, *rlst;
	LBXGL_Brush *bcur;

	if(!node->lnode)return(ilst);

#if 1
	bcur=node->mid;
	while(bcur)
	{
		ilst=LBXGL_BrushBSP_ClipPortalListBrush(ilst, bcur);
		bcur=bcur->nnext;
	}
#endif

	LBXGL_BrushBSP_SplitListPortalPlaneRight(ilst, node->norm, &llst, &rlst);
	llst=LBXGL_BrushBSP_ClipPortalListNodeRight(root, node->lnode, llst);
	rlst=LBXGL_BrushBSP_ClipPortalListNodeRight(root, node->rnode, rlst);
	tmp=LBXGL_BrushBSP_MergePortalList(llst, rlst);
	return(tmp);
}

void LBXGL_BrushBSP_SplitChainPortalPlaneLeft(
	LBXGL_BrushTempPortal *lst, float *norm,
	LBXGL_BrushTempPortal **rllst, LBXGL_BrushTempPortal **rrlst)
{
	LBXGL_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		LBXGL_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(g<=0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>=0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur=nxt;
	}
	*rllst=llst; *rrlst=rlst;
}

void LBXGL_BrushBSP_SplitChainPortalPlaneRight(
	LBXGL_BrushTempPortal *lst, float *norm,
	LBXGL_BrushTempPortal **rllst, LBXGL_BrushTempPortal **rrlst)
{
	LBXGL_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		LBXGL_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(f>=0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		if(g<=0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		cur=nxt;
	}
	*rllst=llst; *rrlst=rlst;
}

void LBXGL_BrushBSP_BindPortalListLeftNode(
	LBXGL_BrushNode *root, LBXGL_BrushNode *node,
	LBXGL_BrushTempPortal *ilst)
{
	LBXGL_BrushTempPortal *cur, *llst, *rlst, *nxt;

	node->prt=NULL;

	if(!node->lnode)
	{
		cur=ilst;
		while(cur)
		{
			cur->lnode=node;
			cur->lnext=node->prt;
			node->prt=cur;

			cur=cur->chain;
		}
		return;
	}

	LBXGL_BrushBSP_SplitChainPortalPlaneLeft(
		ilst, node->norm, &llst, &rlst);
	LBXGL_BrushBSP_BindPortalListLeftNode(root, node->lnode, llst);
	LBXGL_BrushBSP_BindPortalListLeftNode(root, node->rnode, rlst);
}

void LBXGL_BrushBSP_BindPortalListRightNode(
	LBXGL_BrushNode *root, LBXGL_BrushNode *node,
	LBXGL_BrushTempPortal *ilst)
{
	LBXGL_BrushTempPortal *cur, *llst, *rlst, *nxt;

	if(!node->lnode)
	{
		cur=ilst;
		while(cur)
		{
			cur->rnode=node;
			cur->rnext=node->prt;
			node->prt=cur;
			cur=cur->chain;
		}
		return;
	}

	LBXGL_BrushBSP_SplitChainPortalPlaneRight(
		ilst, node->norm, &llst, &rlst);
	LBXGL_BrushBSP_BindPortalListRightNode(root, node->lnode, llst);
	LBXGL_BrushBSP_BindPortalListRightNode(root, node->rnode, rlst);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_DropOutsidePortals(
	LBXGL_BrushTempPortal *ilst)
{
	LBXGL_BrushTempPortal *cur, *nxt, *lst;
	float mins[3], maxs[3];
	int i;

	cur=ilst; lst=NULL;
	while(cur)
	{
		nxt=cur->next;

		mins[0]=999999; maxs[0]=-999999;
		mins[1]=999999; maxs[1]=-999999;
		mins[2]=999999; maxs[2]=-999999;

		for(i=0; i<cur->n_pts; i++)
		{
			if(cur->pts[i*3+0]<mins[0])mins[0]=cur->pts[i*3+0];
			if(cur->pts[i*3+1]<mins[1])mins[1]=cur->pts[i*3+1];
			if(cur->pts[i*3+2]<mins[2])mins[2]=cur->pts[i*3+2];

			if(cur->pts[i*3+0]>maxs[0])maxs[0]=cur->pts[i*3+0];
			if(cur->pts[i*3+1]>maxs[1])maxs[1]=cur->pts[i*3+1];
			if(cur->pts[i*3+2]>maxs[2])maxs[2]=cur->pts[i*3+2];
		}

		if((mins[0]>(-90000)) && (mins[1]>(-90000)) && (mins[2]>(-90000)) &&
			(maxs[0]<90000) && (maxs[1]<90000) && (maxs[2]<90000))
		{
			cur->next=lst; lst=cur;
			cur=nxt; continue;
		}

		LBXGL_BrushBSP_FreePortal(cur);
		cur=nxt;
	}
	return(lst);
}

LBXGL_BrushTempPortal *LBXGL_BrushBSP_BuildPortals(LBXGL_BrushNode *root)
{
	LBXGL_BrushTempPortal *lst, *cur, *tlst;
	int i;

	printf("Build Portals\n");

	lst=LBXGL_BrushBSP_BuildPortalListNode(root, root, NULL);

#if 1
	lst=LBXGL_BrushBSP_ClipPortalListNodeLeft(root, root, lst);
	lst=LBXGL_BrushBSP_ClipPortalListNodeRight(root, root, lst);

//	lst=LBXGL_BrushBSP_DropOutsidePortals(lst);

	cur=lst; tlst=NULL;
	while(cur) { cur->chain=tlst; tlst=cur; cur=cur->next; }
	LBXGL_BrushBSP_BindPortalListLeftNode(root, root, tlst);

	cur=lst; tlst=NULL;
	while(cur) { cur->chain=tlst; tlst=cur; cur=cur->next; }
	LBXGL_BrushBSP_BindPortalListRightNode(root, root, tlst);
#endif

	cur=lst; i=0;
	while(cur) { i++; cur=cur->next; }

	printf("Build Portals: %d\n", i);

	return(lst);
}

void LBXGL_BrushBSP_DrawPortalList(LBXGL_BrushNode *root,
	LBXGL_BrushTempPortal *lst)
{
	LBXGL_BrushTempPortal *cur;
	int i, j;

//	glDisable(GL_TEXTURE_2D);
	pdglDisableTexture2D();

	cur=lst; i=0;
	while(cur)
	{
		pdglColor4f((i&1)?1:0.5, (i&2)?1:0.5, (i&4)?1:0.5, 0.25);

		pdglBegin(PDGL_POLYGON);
		for(j=0; j<cur->n_pts; j++)
			pdglVertex3fv(cur->pts+j*3);
		pdglEnd();

		i++;
		cur=cur->next;
	}
}

