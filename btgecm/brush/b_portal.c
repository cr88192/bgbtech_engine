#include <btgecm.h>

BTGE_BrushTempPortal *lbxgl_prt_list=NULL;

BTGE_Brush *BTGE_BrushBSP_GetNodeBrushListR(
	BTGE_BrushNode *node, BTGE_Brush *lst)
{
	BTGE_Brush *cur;

	if(!node)return(NULL);
	cur=node->mid;
	while(cur) { cur->chain=lst; lst=cur; cur=cur->nnext; }
	lst=BTGE_BrushBSP_GetNodeBrushListR(node->lnode, lst);
	lst=BTGE_BrushBSP_GetNodeBrushListR(node->rnode, lst);
	return(lst);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_AllocPortal()
{
	BTGE_BrushTempPortal *tmp;

	if(lbxgl_prt_list)
	{
		tmp=lbxgl_prt_list;
		lbxgl_prt_list=tmp->next;

		memset(tmp, 0, sizeof(BTGE_BrushTempPortal));
		return(tmp);
	}

	tmp=gcalloc(sizeof(BTGE_BrushTempPortal));
	return(tmp);
}

void BTGE_BrushBSP_FreePortal(BTGE_BrushTempPortal *prt)
{
	prt->next=lbxgl_prt_list;
	lbxgl_prt_list=prt;
}

void BTGE_BrushBSP_PortalExtents(BTGE_BrushTempPortal *prt,
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

void BTGE_BrushBSP_PortalPlaneExtents(BTGE_BrushTempPortal *prt,
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

int BTGE_BrushBSP_CheckPortalBrush(BTGE_BrushTempPortal *prt,
	BTGE_Brush *brsh)
{
	float f, g;
	int i;

	BTGE_Brush_GetPlaneExtents(brsh, prt->norm, &f, &g);
	if(f>0)return(0);
	if(g<0)return(0);

	for(i=0; i<brsh->n_face; i++)
	{
		BTGE_BrushBSP_PortalPlaneExtents(
			prt, brsh->norm+i*4, &f, &g);
		if(f>0)return(0);
	}
	return(1);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_ClipPortalBrush(
	BTGE_BrushTempPortal *prt, BTGE_BrushTempPortal *lst,
	BTGE_Brush *brsh)
{
	BTGE_BrushTempPortal *tmp;
	float f, g;
	int i, j;

	i=BTGE_BrushBSP_CheckPortalBrush(prt, brsh);
	if(!i) { prt->next=lst; return(prt); }

	for(i=0; i<brsh->n_face; i++)
	{
		BTGE_BrushBSP_PortalPlaneExtents(
			prt, brsh->norm+i*4, &f, &g);
		if(g<=0)continue;

//		j=BTGE_BrushBSP_CheckPortalBrush(prt, brsh);
//		if(!j) { prt->next=lst; return(prt); }

		if(f>0) { prt->next=lst; return(prt); }

		lst=BTGE_BrushBSP_RightPortalPlane(
			prt, lst, brsh->norm+i*4);
		tmp=BTGE_BrushBSP_LeftPortalPlane(
			prt, lst, brsh->norm+i*4);
		BTGE_BrushBSP_FreePortal(prt);
		prt=tmp;
	}

//	j=BTGE_BrushBSP_CheckPortalBrush(prt, brsh);
//	if(!j) { prt->next=lst; return(prt); }

	//part inside brush simply discarded
	BTGE_BrushBSP_FreePortal(prt);

	return(lst);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_ClipPortalListBrush(
	BTGE_BrushTempPortal *prt, BTGE_Brush *brsh)
{
	BTGE_BrushTempPortal *cur, *nxt, *lst;

	cur=prt; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		lst=BTGE_BrushBSP_ClipPortalBrush(cur, lst, brsh);
		cur=nxt;
	}
	return(lst);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_SplitPortalPlane(
	BTGE_BrushTempPortal *prt, BTGE_BrushTempPortal *lst,
	float *norm)
{
	float norm1[4];
	BTGE_BrushTempPortal *tmp0, *tmp1;
	int i, l;

	norm1[0]=-norm[0]; norm1[1]=-norm[1];
	norm1[2]=-norm[2]; norm1[3]=-norm[3];

	tmp0=BTGE_BrushBSP_AllocPortal();
	tmp1=BTGE_BrushBSP_AllocPortal();

	tmp0->n_pts=Hull_ClipFace(norm, prt->pts, tmp0->pts, prt->n_pts);
	tmp1->n_pts=Hull_ClipFace(norm1, prt->pts, tmp1->pts, prt->n_pts);

	V4F_COPY(prt->norm, tmp0->norm);
	V4F_COPY(prt->norm, tmp1->norm);

	tmp0->next=tmp1;
	tmp1->next=lst;
	return(tmp0);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_LeftPortalPlane(
	BTGE_BrushTempPortal *prt, BTGE_BrushTempPortal *lst,
	float *norm)
{
	BTGE_BrushTempPortal *tmp;
	int i, l;

	tmp=BTGE_BrushBSP_AllocPortal();
	tmp->n_pts=Hull_ClipFace(norm, prt->pts, tmp->pts, prt->n_pts);
	V4F_COPY(prt->norm, tmp->norm);

	tmp->next=lst;
	return(tmp);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_RightPortalPlane(
	BTGE_BrushTempPortal *prt, BTGE_BrushTempPortal *lst,
	float *norm)
{
	float norm1[4];
	BTGE_BrushTempPortal *tmp;
	int i, l;

	norm1[0]=-norm[0]; norm1[1]=-norm[1];
	norm1[2]=-norm[2]; norm1[3]=-norm[3];

	tmp=BTGE_BrushBSP_AllocPortal();
	tmp->n_pts=Hull_ClipFace(norm1, prt->pts, tmp->pts, prt->n_pts);
	V4F_COPY(prt->norm, tmp->norm);

	tmp->next=lst;
	return(tmp);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_SplitPortalListPlane(
	BTGE_BrushTempPortal *prt, float *norm)
{
	BTGE_BrushTempPortal *cur, *lst, *nxt;
	float f, g;

	cur=prt; lst=NULL;
	while(cur)
	{
		nxt=cur->next;
		BTGE_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if((f>=0) || (g<=0))
			{ cur->next=lst; lst=cur; cur=nxt; continue; }
		lst=BTGE_BrushBSP_SplitPortalPlane(cur, lst, norm);
		BTGE_BrushBSP_FreePortal(cur);
		cur=nxt;
	}
	return(lst);
}

void BTGE_BrushBSP_SplitListPortalPlaneLeft(
	BTGE_BrushTempPortal *lst, float *norm,
	BTGE_BrushTempPortal **rllst, BTGE_BrushTempPortal **rrlst)
{
	BTGE_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->next;
		BTGE_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(g<=0) { cur->next=llst; llst=cur; cur=nxt; continue; }
		if(f>=0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }
		llst=BTGE_BrushBSP_LeftPortalPlane(cur, llst, norm);
		rlst=BTGE_BrushBSP_RightPortalPlane(cur, rlst, norm);
		BTGE_BrushBSP_FreePortal(cur);
		cur=nxt;
	}

	*rllst=llst;
	*rrlst=rlst;
}

void BTGE_BrushBSP_SplitListPortalPlaneRight(
	BTGE_BrushTempPortal *lst, float *norm,
	BTGE_BrushTempPortal **rllst, BTGE_BrushTempPortal **rrlst)
{
	BTGE_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->next;
		BTGE_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(f>=0) { cur->next=rlst; rlst=cur; cur=nxt; continue; }
		if(g<=0) { cur->next=llst; llst=cur; cur=nxt; continue; }
		llst=BTGE_BrushBSP_LeftPortalPlane(cur, llst, norm);
		rlst=BTGE_BrushBSP_RightPortalPlane(cur, rlst, norm);
		BTGE_BrushBSP_FreePortal(cur);
		cur=nxt;
	}

	*rllst=llst;
	*rrlst=rlst;
}

BTGE_BrushTempPortal *BTGE_BrushBSP_MergePortalList(
	BTGE_BrushTempPortal *lsta, BTGE_BrushTempPortal *lstb)
{
	BTGE_BrushTempPortal *cur;

	if(!lsta)return(lstb);
	if(!lstb)return(lsta);

	cur=lsta;
	while(cur->next)cur=cur->next;
	cur->next=lstb;
	return(lsta);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_BuildPortalListNode(
	BTGE_BrushNode *root, BTGE_BrushNode *node,
	BTGE_BrushTempPortal *lst)
{
	static float pts0[16*3];
	static float pts1[16*3];
	float tnorm[4];
	BTGE_BrushTempPortal *tmp;
	BTGE_BrushNode *ncur;
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
		tmp=BTGE_BrushBSP_AllocPortal();
		V4F_COPY(node->norm, tmp->norm);
		for(i=0; i<(l*3); i++)tmp->pts[i]=pts0[i];
		tmp->n_pts=l; tmp->next=lst; lst=tmp;
	}

	lst=BTGE_BrushBSP_BuildPortalListNode(root, node->lnode, lst);
	lst=BTGE_BrushBSP_BuildPortalListNode(root, node->rnode, lst);
	return(lst);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_ClipPortalListNodeLeft(
	BTGE_BrushNode *root, BTGE_BrushNode *node,
	BTGE_BrushTempPortal *ilst)
{
	BTGE_BrushTempPortal *tmp, *llst, *rlst;
	BTGE_Brush *bcur;

	if(!node->lnode)return(ilst);

#if 1
	bcur=node->mid;
	while(bcur)
	{
		ilst=BTGE_BrushBSP_ClipPortalListBrush(ilst, bcur);
		bcur=bcur->nnext;
	}
#endif

	BTGE_BrushBSP_SplitListPortalPlaneLeft(ilst, node->norm, &llst, &rlst);
	llst=BTGE_BrushBSP_ClipPortalListNodeLeft(root, node->lnode, llst);
	rlst=BTGE_BrushBSP_ClipPortalListNodeLeft(root, node->rnode, rlst);
	tmp=BTGE_BrushBSP_MergePortalList(llst, rlst);
	return(tmp);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_ClipPortalListNodeRight(
	BTGE_BrushNode *root, BTGE_BrushNode *node,
	BTGE_BrushTempPortal *ilst)
{
	BTGE_BrushTempPortal *tmp, *llst, *rlst;
	BTGE_Brush *bcur;

	if(!node->lnode)return(ilst);

#if 1
	bcur=node->mid;
	while(bcur)
	{
		ilst=BTGE_BrushBSP_ClipPortalListBrush(ilst, bcur);
		bcur=bcur->nnext;
	}
#endif

	BTGE_BrushBSP_SplitListPortalPlaneRight(ilst, node->norm, &llst, &rlst);
	llst=BTGE_BrushBSP_ClipPortalListNodeRight(root, node->lnode, llst);
	rlst=BTGE_BrushBSP_ClipPortalListNodeRight(root, node->rnode, rlst);
	tmp=BTGE_BrushBSP_MergePortalList(llst, rlst);
	return(tmp);
}

void BTGE_BrushBSP_SplitChainPortalPlaneLeft(
	BTGE_BrushTempPortal *lst, float *norm,
	BTGE_BrushTempPortal **rllst, BTGE_BrushTempPortal **rrlst)
{
	BTGE_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		BTGE_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(g<=0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>=0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur=nxt;
	}
	*rllst=llst; *rrlst=rlst;
}

void BTGE_BrushBSP_SplitChainPortalPlaneRight(
	BTGE_BrushTempPortal *lst, float *norm,
	BTGE_BrushTempPortal **rllst, BTGE_BrushTempPortal **rrlst)
{
	BTGE_BrushTempPortal *cur, *llst, *rlst, *nxt;
	float f, g;

	cur=lst; llst=NULL; rlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		BTGE_BrushBSP_PortalPlaneExtents(cur, norm, &f, &g);
		if(f>=0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		if(g<=0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		cur=nxt;
	}
	*rllst=llst; *rrlst=rlst;
}

void BTGE_BrushBSP_BindPortalListLeftNode(
	BTGE_BrushNode *root, BTGE_BrushNode *node,
	BTGE_BrushTempPortal *ilst)
{
	BTGE_BrushTempPortal *cur, *llst, *rlst, *nxt;

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

	BTGE_BrushBSP_SplitChainPortalPlaneLeft(
		ilst, node->norm, &llst, &rlst);
	BTGE_BrushBSP_BindPortalListLeftNode(root, node->lnode, llst);
	BTGE_BrushBSP_BindPortalListLeftNode(root, node->rnode, rlst);
}

void BTGE_BrushBSP_BindPortalListRightNode(
	BTGE_BrushNode *root, BTGE_BrushNode *node,
	BTGE_BrushTempPortal *ilst)
{
	BTGE_BrushTempPortal *cur, *llst, *rlst, *nxt;

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

	BTGE_BrushBSP_SplitChainPortalPlaneRight(
		ilst, node->norm, &llst, &rlst);
	BTGE_BrushBSP_BindPortalListRightNode(root, node->lnode, llst);
	BTGE_BrushBSP_BindPortalListRightNode(root, node->rnode, rlst);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_DropOutsidePortals(
	BTGE_BrushTempPortal *ilst)
{
	BTGE_BrushTempPortal *cur, *nxt, *lst;
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

		BTGE_BrushBSP_FreePortal(cur);
		cur=nxt;
	}
	return(lst);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_BuildPortals(BTGE_BrushNode *root)
{
	BTGE_BrushTempPortal *lst, *cur, *tlst;
	int i;

	printf("Build Portals\n");

	lst=BTGE_BrushBSP_BuildPortalListNode(root, root, NULL);

#if 1
	lst=BTGE_BrushBSP_ClipPortalListNodeLeft(root, root, lst);
	lst=BTGE_BrushBSP_ClipPortalListNodeRight(root, root, lst);

//	lst=BTGE_BrushBSP_DropOutsidePortals(lst);

	cur=lst; tlst=NULL;
	while(cur) { cur->chain=tlst; tlst=cur; cur=cur->next; }
	BTGE_BrushBSP_BindPortalListLeftNode(root, root, tlst);

	cur=lst; tlst=NULL;
	while(cur) { cur->chain=tlst; tlst=cur; cur=cur->next; }
	BTGE_BrushBSP_BindPortalListRightNode(root, root, tlst);
#endif

	cur=lst; i=0;
	while(cur) { i++; cur=cur->next; }

	printf("Build Portals: %d\n", i);

	return(lst);
}
