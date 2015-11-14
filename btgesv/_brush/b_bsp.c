#include <btgesv.h>

BTGE_BrushNode *lbxgl_bbsp_nodefree;
int lbxgl_bbsp_tseq;

BTGE_API BTGE_BrushNode *BTGE_BrushBSP_AllocNode()
{
	BTGE_BrushNode *tmp;
	if(lbxgl_bbsp_nodefree)
	{
		tmp=lbxgl_bbsp_nodefree;
		lbxgl_bbsp_nodefree=tmp->lnode;

		memset(tmp, 0, sizeof(BTGE_BrushNode));

		tmp->lnode=NULL;
		tmp->rnode=NULL;
		tmp->mid=NULL;
		return(tmp);
	}

	tmp=gcalloc(sizeof(BTGE_BrushNode));
	return(tmp);
}

BTGE_API void BTGE_BrushBSP_FreeNode(BTGE_BrushNode *tmp)
{
	if(tmp->vis)gcfree(tmp->vis);
	tmp->vis=NULL;

	tmp->lnode=NULL;
	tmp->rnode=NULL;
	tmp->mid=NULL;
	tmp->lnode=lbxgl_bbsp_nodefree;
	lbxgl_bbsp_nodefree=tmp;
}

BTGE_API void BTGE_BrushBSP_FreeTreeNode(BTGE_BrushNode *tmp)
{
	if(!tmp)return;
	BTGE_BrushBSP_FreeTreeNode(tmp->lnode);
	BTGE_BrushBSP_FreeTreeNode(tmp->rnode);
	BTGE_BrushBSP_FreeNode(tmp);
}

BTGE_API void BTGE_BrushBSP_BBoxList(BTGE_Brush *lst, float *mins, float *maxs)
{
	BTGE_Brush *cur;

	if(!lst)
	{
		mins[0]=0; mins[1]=0; mins[2]=0;
		maxs[0]=0; maxs[1]=0; maxs[2]=0;
		return;
	}

	mins[0]=999999;		mins[1]=999999;		mins[2]=999999;
	maxs[0]=-999999;	maxs[1]=-999999;	maxs[2]=-999999;

	cur=lst;
	while(cur)
	{
		if(cur->mins[0]<mins[0])mins[0]=cur->mins[0];
		if(cur->mins[1]<mins[1])mins[1]=cur->mins[1];
		if(cur->mins[2]<mins[2])mins[2]=cur->mins[2];

		if(cur->maxs[0]>maxs[0])maxs[0]=cur->maxs[0];
		if(cur->maxs[1]>maxs[1])maxs[1]=cur->maxs[1];
		if(cur->maxs[2]>maxs[2])maxs[2]=cur->maxs[2];

		cur=cur->chain;
	}
}

#if 1
BTGE_API float BTGE_BrushBSP_WeightPlaneBrushList(BTGE_Brush *lst, float *norm)
{
	BTGE_Brush *cur;
	int nl, nr, nm;
	float f, g;

	cur=lst; nl=0; nr=0; nm=0;
	while(cur)
	{
		BTGE_Brush_GetPlaneExtents(cur, norm, &f, &g);
		if(g<=0)nl++; if(f>=0)nr++;
		if((f<0) && (g>0))nm++;
		cur=cur->chain;
	}

	if(!nl || !nr)return(999999.0);

	f=fabs(nl-nr)+(nm*0.25);
	return(f);
}

BTGE_API int BTGE_BrushBSP_BestPlaneBrushList(BTGE_Brush *lst, float *norm)
{
	float bn[4];
	BTGE_Brush *cur;
	float f, bf;
	int i;

	if(!lst || !lst->chain)
		return(-1);

	V4F_ZERO(bn); bf=999999.0;

	cur=lst;
	while(cur)
	{
		for(i=0; i<cur->n_face; i++)
		{
			f=BTGE_BrushBSP_WeightPlaneBrushList(
				lst, cur->norm+i*4);
			if(f<bf)
			{
				V4F_COPY(cur->norm+i*4, bn);
				bf=f;
			}
		}
		cur=cur->chain;
	}
	if(bf>=900000.0)
		return(-1);

	V4F_COPY(bn, norm);
	return(0);
}

BTGE_API BTGE_BrushNode *BTGE_BrushBSP_BuildNodeList(BTGE_Brush *lst)
{
	float org[3], dir[4];
	BTGE_BrushNode *tmp;
	BTGE_Brush *cur, *nxt, *llst, *rlst, *mlst;
	float f, g;
	int i, j, k;

	tmp=BTGE_BrushBSP_AllocNode();
	tmp->id=lbxgl_bbsp_tseq++;

	i=BTGE_BrushBSP_BestPlaneBrushList(lst, dir);

	if(i<0)
	{
		BTGE_BrushBSP_BBoxList(lst, tmp->mins, tmp->maxs);

		cur=lst;
		while(cur)
		{
			cur->node=tmp;
			cur->nnext=tmp->mid;
			tmp->mid=cur;
			cur=cur->chain;
		}
		return(tmp);
	}


	V4F_COPY(dir, tmp->norm);

	cur=lst; llst=NULL; rlst=NULL; mlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		BTGE_Brush_GetPlaneExtents(cur, dir, &f, &g);
		if(g<=0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>=0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	tmp->lnode=BTGE_BrushBSP_BuildNodeList(llst);
	tmp->rnode=BTGE_BrushBSP_BuildNodeList(rlst);
	tmp->lnode->pnode=tmp;
	tmp->rnode->pnode=tmp;

	BTGE_BrushBSP_BBoxList(mlst, tmp->mins, tmp->maxs);

	cur=mlst;
	while(cur)
	{
		cur->node=tmp;
		cur->nnext=tmp->mid;
		tmp->mid=cur;
		cur=cur->chain;
	}
	return(tmp);
}
#endif

#if 1
BTGE_API BTGE_BrushNode *BTGE_BrushBSP_BuildNodeListFast(BTGE_Brush *lst)
{
	static int rcnt=0;
	float org[3], dir[4], dx[3], dy[3], dz[3];
	BTGE_BrushNode *tmp;
	BTGE_Brush *cur, *nxt, *llst, *rlst, *mlst;
	float f, g, h;
	int i, j, k;

	tmp=BTGE_BrushBSP_AllocNode();
	tmp->id=lbxgl_bbsp_tseq++;

	if(!lst || !lst->chain || (rcnt>=64))
	{
		BTGE_BrushBSP_BBoxList(lst, tmp->mins, tmp->maxs);

		cur=lst;
		while(cur)
		{
			cur->node=tmp;
			cur->nnext=tmp->mid;
			tmp->mid=cur;
			cur=cur->chain;
		}
		return(tmp);
	}

	cur=lst; V3F_ZERO(org); i=0;
	while(cur)
	{
		org[0]+=cur->org[0];
		org[1]+=cur->org[1];
		org[2]+=cur->org[2];
		i++; cur=cur->chain;
	}
	org[0]/=i; org[1]/=i; org[2]/=i;

#if 0
	cur=lst; V3F_ZERO(dir);
	while(cur)
	{
		dir[0]+=fabs(cur->org[0]-org[0]);
		dir[1]+=fabs(cur->org[1]-org[1]);
		dir[2]+=fabs(cur->org[2]-org[2]);
		cur=cur->chain;
	}

	V3F_NORMALIZE(dir, dir); dir[3]=V3F_DOT(org, dir);
#endif

#if 1
	cur=lst; V3F_ZERO(dir);
	V3F_ZERO(dx); V3F_ZERO(dy); V3F_ZERO(dz);
	while(cur)
	{
		if(cur->org[0]>=org[0])
		{
			dx[0]+=cur->org[0]-org[0];
			dx[1]+=cur->org[1]-org[1];
			dx[2]+=cur->org[2]-org[2];
		}else
		{
			dx[0]-=cur->org[0]-org[0];
			dx[1]-=cur->org[1]-org[1];
			dx[2]-=cur->org[2]-org[2];
		}

		if(cur->org[1]>=org[1])
		{
			dy[0]+=cur->org[0]-org[0];
			dy[1]+=cur->org[1]-org[1];
			dy[2]+=cur->org[2]-org[2];
		}else
		{
			dy[0]-=cur->org[0]-org[0];
			dy[1]-=cur->org[1]-org[1];
			dy[2]-=cur->org[2]-org[2];
		}

		if(cur->org[1]>=org[1])
		{
			dz[0]+=cur->org[0]-org[0];
			dz[1]+=cur->org[1]-org[1];
			dz[2]+=cur->org[2]-org[2];
		}else
		{
			dz[0]-=cur->org[0]-org[0];
			dz[1]-=cur->org[1]-org[1];
			dz[2]-=cur->org[2]-org[2];
		}

		cur=cur->chain;
	}

	f=V3F_NORMALIZE(dx, dx);
	g=V3F_NORMALIZE(dy, dy);
	h=V3F_NORMALIZE(dz, dz);

	if((f>=g) && (f>=h)){ V3F_COPY(dx, dir); }
		else if((g>=f) && (g>=h)) { V3F_COPY(dy, dir); }
		else { V3F_COPY(dz, dir); }

	f=V3F_NORMALIZE(dir, dir);
	if(f<0.001) { dir[0]=0; dir[1]=0; dir[2]=1; }

	dir[3]=V3F_DOT(org, dir);

#endif


	V4F_COPY(dir, tmp->norm);

	cur=lst; i=0; j=0;
	while(cur)
	{
		BTGE_Brush_GetPlaneExtents(cur, dir, &f, &g);
		if(g<=0)i++; if(f>=0)j++;
		if(i && j) break;
		cur=cur->chain;
	}

	if(!i || !j)
	{
		BTGE_BrushBSP_BBoxList(lst, tmp->mins, tmp->maxs);

		cur=lst;
		while(cur)
		{
			cur->node=tmp;
			cur->nnext=tmp->mid;
			tmp->mid=cur;
			cur=cur->chain;
		}
		return(tmp);
	}

	cur=lst; llst=NULL; rlst=NULL; mlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		BTGE_Brush_GetPlaneExtents(cur, dir, &f, &g);
		if(g<=0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>=0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	rcnt++;
	tmp->lnode=BTGE_BrushBSP_BuildNodeListFast(llst);
	tmp->rnode=BTGE_BrushBSP_BuildNodeListFast(rlst);
	tmp->lnode->pnode=tmp;
	tmp->rnode->pnode=tmp;
	rcnt--;

	BTGE_BrushBSP_BBoxList(mlst, tmp->mins, tmp->maxs);

	cur=mlst;
	while(cur)
	{
		cur->node=tmp;
		cur->nnext=tmp->mid;
		tmp->mid=cur;
		cur=cur->chain;
	}
	return(tmp);
}
#endif

BTGE_API int BTGE_BrushBSP_CheckPointNode(BTGE_BrushNode *node, float *pt)
{
	float f;
	int i;

	if(!node->pnode)return(1);

	while(node->pnode)
	{
		f=V3F_NDOT(pt, node->pnode->norm);
		if((node==node->pnode->lnode) && (f>0))return(0);
		if((node==node->pnode->rnode) && (f<0))return(0);
		node=node->pnode;
	}

	return(1);
}

BTGE_API int BTGE_BrushBSP_LastNodeID(BTGE_BrushNode *node)
{
	if(node->rnode)
		return(BTGE_BrushBSP_LastNodeID(node->rnode));
	return(node->id);
}

BTGE_API BTGE_BrushNode *BTGE_BrushBSP_BuildTreeListFast(BTGE_Brush *lst)
{
	BTGE_BrushNode *tmp;
	BTGE_BrushTempPortal *prt;

//	printf("Build BSP\n");

	lbxgl_bbsp_tseq=1;
	tmp=BTGE_BrushBSP_BuildNodeListFast(lst);

//	printf("Build BSP: OK, %d nodes\n", lbxgl_bbsp_tseq);

	return(tmp);
}


#if 1
BTGE_API float BTGE_BrushBSP_WeightPlaneFaceList(BTGE_BrushTempFace *lst, float *norm)
{
	BTGE_BrushTempFace *cur;
	int nl, nr, nm;
	float f, g;

	cur=lst; nl=0; nr=0; nm=0;
	while(cur)
	{
		BTGE_BrushCSG_FacePlaneExtents(cur, norm, &f, &g);
		if(g<=0)nl++; if(f>=0)nr++;
		if((f<0) && (g>0))nm++;
		cur=cur->next;
	}

	if(!nl || !nr)return(999999.0);

	f=fabs(nl-nr)+(nm*0.25);
	return(f);
}

BTGE_API int BTGE_BrushBSP_BestPlaneFaceList(BTGE_BrushTempFace *lst, float *norm)
{
	float bn[4];
	BTGE_BrushTempFace *cur;
	float f, bf;
	int i;

	if(!lst || !lst->next)
		return(-1);

	V4F_ZERO(bn); bf=999999.0;

	cur=lst;
	while(cur)
	{
		if(cur->flags&BTGE_TXFL_NOPLANE)
			{ cur=cur->next; continue; }

		f=BTGE_BrushBSP_WeightPlaneFaceList(lst, cur->norm);
		if(f<bf) { V4F_COPY(cur->norm, bn); bf=f; }
		cur=cur->next;
	}
	if(bf>=900000.0)
		return(-1);

	V4F_COPY(bn, norm);
	return(0);
}

BTGE_API BTGE_BrushNode *BTGE_BrushBSP_BuildNodeFaceList(BTGE_BrushTempFace *lst)
{
	float org[3], dir[4];
	BTGE_BrushNode *tmp;
	BTGE_BrushTempFace *cur, *nxt, *llst, *rlst, *mlst;
	float f, g;
	int i, j, k;

	tmp=BTGE_BrushBSP_AllocNode();
	tmp->id=lbxgl_bbsp_tseq++;

	i=BTGE_BrushBSP_BestPlaneFaceList(lst, dir);


	if(i<0)
//	if(1)
	{
//		printf("Leaf\n");

		BTGE_BrushCSG_BBoxFaceList(lst, tmp->mins, tmp->maxs);

		cur=lst;
		while(cur)
		{
//			cur->node=tmp;
			cur->nnext=tmp->face;
			tmp->face=cur;
			cur=cur->next;
		}

//		printf("Leaf OK\n");
		return(tmp);
	}


//	printf("Node (%g %g %g %g)\n", dir[0], dir[1], dir[2], dir[3]);

	V4F_COPY(dir, tmp->norm);

	BTGE_BrushCSG_SplitListFacePlane(lst, dir, &llst, &rlst);

	if(!llst || !rlst)
	{
//		printf("Node: Split Fail %p %p\n", llst, rlst);
//		return(NULL);
	}

	tmp->lnode=BTGE_BrushBSP_BuildNodeFaceList(llst);
	tmp->rnode=BTGE_BrushBSP_BuildNodeFaceList(rlst);
	tmp->lnode->pnode=tmp;
	tmp->rnode->pnode=tmp;

//	printf("Node OK\n");

	return(tmp);
}
#endif

BTGE_API void BTGE_BrushBSP_BindNodeBrushList(BTGE_BrushNode *node, BTGE_Brush *lst)
{
	BTGE_Brush *cur, *nxt, *llst, *rlst, *mlst;
	float f, g;

	if(!lst)return;

	if(!node->lnode)
	{
		cur=lst;
		while(cur)
		{
			cur->node=node;
			cur->nnext=node->mid;
			node->mid=cur;
			cur=cur->chain;
		}

		return;
	}

	cur=lst; llst=NULL; rlst=NULL; mlst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		BTGE_Brush_GetPlaneExtents(cur, node->norm, &f, &g);
		if(g<0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	BTGE_BrushBSP_BindNodeBrushList(node->lnode, llst);
	BTGE_BrushBSP_BindNodeBrushList(node->rnode, rlst);

	cur=mlst;
	while(cur)
	{
		cur->node=node;
		cur->nnext=node->mid;
		node->mid=cur;
		cur=cur->chain;
	}
}

BTGE_API BTGE_BrushNode *BTGE_BrushBSP_BuildTreeList(BTGE_Brush *lst)
{
	BTGE_BrushNode *tmp;
	BTGE_BrushTempPortal *prt;
	BTGE_BrushTempFace *flst, *fcur;
	int i;

	printf("Build BSP\n");

	lbxgl_bbsp_tseq=1;
	tmp=BTGE_BrushBSP_BuildNodeList(lst);
	BTGE_BrushCSG_BuildFaces(tmp);

//	BTGE_BrushBSP_BuildVis(tmp);
//	prt=BTGE_BrushBSP_BuildPortals(tmp);
//	BTGE_BrushBSP_BuildVis(tmp, prt);

#if 0
	printf("Build CSG Faces\n");

	flst=BTGE_BrushCSG_MakeFacesBrushList(lst);
//	flst=BTGE_BrushCSG_ClipFaceListBrushList(flst, lst);

	fcur=flst; i=0;
	while(fcur) { i++; fcur=fcur->next; }
	printf("Build CSG Faces: OK, %d faces\n", i);

	tmp=BTGE_BrushBSP_BuildNodeFaceList(flst);
	BTGE_BrushBSP_BindNodeBrushList(tmp, lst);
#endif

	printf("Build BSP: OK, %d nodes\n", lbxgl_bbsp_tseq);

	return(tmp);
}

BTGE_Brush *BTGE_BrushBSP_QueryTreeNodeR(BTGE_BrushNode *node,
	BTGE_Brush *lst, float *org, float rad)
{
	float dir[4];
	BTGE_Brush *cur;
	float f, g;

	if(!node)return(lst);

	cur=node->mid;
	while(cur)
	{
#if 0
		V3F_SUB(cur->org, org, dir);
		f=V3F_NORMALIZE(dir, dir);
		if(f>(cur->rad+rad)) { cur=cur->nnext; continue; }

		dir[3]=V3F_DOT(org, dir);
		BTGE_Brush_GetPlaneExtents(cur, dir, &f, &g);
//		BTGE_Brush_GetBBoxPlaneExtents(cur, dir, &f, &g);
		if(f>=rad) { cur=cur->nnext; continue; }
#endif

		if(!BTGE_Brush_CheckSphereBrush(cur, org, rad))
			{ cur=cur->nnext; continue; }

		cur->chain=lst; lst=cur;
		cur=cur->nnext;
	}

	if(!node->lnode)
		return(lst);

	f=V3F_NDOT(org, node->norm);
	if(fabs(f)<rad)
	{
		lst=BTGE_BrushBSP_QueryTreeNodeR(node->lnode, lst, org, rad);
		lst=BTGE_BrushBSP_QueryTreeNodeR(node->rnode, lst, org, rad);
		return(lst);
	}

	if(f<0)
	{
		lst=BTGE_BrushBSP_QueryTreeNodeR(node->lnode, lst, org, rad);
		return(lst);
	}

	if(f>0)
	{
		lst=BTGE_BrushBSP_QueryTreeNodeR(node->rnode, lst, org, rad);
		return(lst);
	}

	return(lst);
}

BTGE_API BTGE_Brush *BTGE_BrushBSP_QueryTreeNode(BTGE_BrushNode *node,
	float *org, float rad)
{
	BTGE_Brush *lst;
	lst=BTGE_BrushBSP_QueryTreeNodeR(node, NULL, org, rad);
	return(lst);
}

BTGE_API BTGE_Brush *BTGE_BrushBSP_TracelineMid(
	BTGE_BrushNode *node, BTGE_Brush *lst,
	float *sv, float *ev)
{
	BTGE_Brush *cur;

	cur=node->mid;
	while(cur)
	{
		if(BTGE_Brush_CheckLineBrush(cur, sv, ev))
			{ cur->chain=lst; lst=cur; }
		cur=cur->nnext;
	}
	return(lst);
}

/** Recursive world trace.
  * Returns a list of all brushes which intersect line.
  * Will try to order brushes by increasing distance from start.
  */
BTGE_API BTGE_Brush *BTGE_BrushBSP_Traceline(
	BTGE_BrushNode *node, BTGE_Brush *lst,
	float *sv, float *ev)
{
	float pt[4];
	BTGE_Brush *cur;
	float f, g;

	if(!node)return(lst);

	if(!node->lnode)
	{
		cur=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
		return(cur);
	}

	f=V3F_NDOT(sv, node->norm);
	g=V3F_NDOT(ev, node->norm);

	if((f<0) && (g>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		lst=BTGE_BrushBSP_Traceline(node->rnode, lst, pt, ev);
		lst=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
		lst=BTGE_BrushBSP_Traceline(node->lnode, lst, sv, pt);
		return(lst);
	}

	if((g<0) && (f>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		lst=BTGE_BrushBSP_Traceline(node->lnode, lst, pt, ev);
		lst=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
		lst=BTGE_BrushBSP_Traceline(node->rnode, lst, sv, pt);
		return(lst);
	}

	if((f==0) && (g==0))
	{
		lst=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
		return(lst);
	}

	if((f<=0) && (g<=0))
	{
		if(f<g)
		{
			lst=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
			lst=BTGE_BrushBSP_Traceline(node->lnode, lst, sv, ev);
		}else
		{
			lst=BTGE_BrushBSP_Traceline(node->lnode, lst, sv, ev);
			lst=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
		}
		return(lst);
	}

	if((f>=0) && (g>=0))
	{
		if(f>g)
		{
			lst=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
			lst=BTGE_BrushBSP_Traceline(node->rnode, lst, sv, ev);
		}else
		{
			lst=BTGE_BrushBSP_Traceline(node->rnode, lst, sv, ev);
			lst=BTGE_BrushBSP_TracelineMid(node, lst, sv, ev);
		}
		return(lst);
	}

	return(lst);
}

BTGE_API BTGE_Brush *BTGE_BrushBSP_TracelineFirstMid(BTGE_BrushNode *node,
	float *sv, float *ev)
{
	float pt[4];
	BTGE_Brush *cur;
	float f, g;
	
	cur=node->mid;
	while(cur)
	{
		if(BTGE_Brush_CheckLineBrush(cur, sv, ev))
			return(cur);
		cur=cur->nnext;
	}

	return(NULL);
}

BTGE_API BTGE_Brush *BTGE_BrushBSP_TracelineFirst(BTGE_BrushNode *node,
	float *sv, float *ev)
{
	float pt[4];
	BTGE_Brush *cur;
	float f, g;

	if(!node)return(NULL);

	if(!node->lnode)
	{
		cur=BTGE_BrushBSP_TracelineFirstMid(node, sv, ev);
		return(cur);
	}

	f=V3F_NDOT(sv, node->norm);
	g=V3F_NDOT(ev, node->norm);

	if((f<0) && (g>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		cur=BTGE_BrushBSP_TracelineFirst(node->lnode, sv, pt);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirstMid(node, sv, ev);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirst(node->rnode, pt, ev);
		return(cur);
	}

	if((g<0) && (f>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		cur=BTGE_BrushBSP_TracelineFirst(node->rnode, sv, pt);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirstMid(node, sv, ev);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirst(node->lnode, pt, ev);
		return(cur);
	}

	if((f==0) && (g==0))
	{
		cur=BTGE_BrushBSP_TracelineFirstMid(node, sv, ev);
		if(cur)return(cur);
		return(NULL);
	}

	if((f<=0) && (g<=0))
	{
		cur=BTGE_BrushBSP_TracelineFirstMid(node, sv, ev);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirst(node->lnode, sv, ev);
		return(cur);
	}

	if((f>=0) && (g>=0))
	{
		cur=BTGE_BrushBSP_TracelineFirstMid(node, sv, ev);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirst(node->rnode, sv, ev);
		return(cur);
	}

	return(NULL);
}

BTGE_API BTGE_Brush *BTGE_BrushBSP_TracelineFirstSkip(BTGE_BrushNode *node,
	float *sv, float *ev, BTGE_Brush *skip)
{
	float pt[4];
	BTGE_Brush *cur, *cur1;
	float f, g;

	if(!node)return(NULL);

	cur=node->mid;
	while(cur)
	{
		if(BTGE_Brush_CheckLineBrush(cur, sv, ev))
		{
			cur1=skip;
			while(cur1) { if(cur==cur1)break; cur1=cur1->chain; }
			if(!cur1)return(cur);
		}
		cur=cur->nnext;
	}

	if(!node->lnode)return(NULL);

	f=V3F_NDOT(sv, node->norm);
	g=V3F_NDOT(ev, node->norm);

	if((f<0) && (g>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->lnode,
			sv, pt, skip);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->rnode,
			pt, ev, skip);
		return(cur);
	}

	if((g<0) && (f>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->rnode,
			sv, pt, skip);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->lnode,
			pt, ev, skip);
		return(cur);
	}

	if((f==0) && (g==0))
	{
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->lnode,
			sv, ev, skip);
		if(cur)return(cur);
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->rnode,
			sv, ev, skip);
		return(cur);

//		return(NULL);
	}

	if((f<=0) && (g<=0))
	{
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->lnode,
			sv, ev, skip);
		return(cur);
	}

	if((f>=0) && (g>=0))
	{
		cur=BTGE_BrushBSP_TracelineFirstSkip(node->rnode,
			sv, ev, skip);
		return(cur);
	}

	return(NULL);
}

BTGE_Brush *BTGE_BrushBSP_GetBrushListR(
	BTGE_BrushNode *node, BTGE_Brush *lst)
{
	BTGE_Brush *cur;

	if(!node)return(lst);

	cur=node->mid;
	while(cur)
	{
		cur->chain=lst; lst=cur;
		cur=cur->nnext;
	}

	lst=BTGE_BrushBSP_GetBrushListR(node->lnode, lst);
	lst=BTGE_BrushBSP_GetBrushListR(node->rnode, lst);

	return(lst);
}

BTGE_API BTGE_Brush *BTGE_BrushBSP_GetBrushList(BTGE_BrushNode *root)
	{ return(BTGE_BrushBSP_GetBrushListR(root, NULL)); }


#if 0
// void BTGE_BrushBSP_BBoxVisR(BTGE_BrushNode *node, byte *vis,
	float *mins, float *maxs)
{
	BTGE_Brush *cur;
	int i;

	if(!node)return;

	i=node->id;
	if(vis[i>>3]&(1<<(i&7)))
	{
		cur=node->mid;
		while(cur)
		{
			if(!BTGE_Brush_CheckBrushFrustum(cur))
				{ cur=cur->nnext; continue; }

			if(cur->mins[0]<mins[0])mins[0]=cur->mins[0];
			if(cur->mins[1]<mins[1])mins[1]=cur->mins[1];
			if(cur->mins[2]<mins[2])mins[2]=cur->mins[2];

			if(cur->maxs[0]>maxs[0])maxs[0]=cur->maxs[0];
			if(cur->maxs[1]>maxs[1])maxs[1]=cur->maxs[1];
			if(cur->maxs[2]>maxs[2])maxs[2]=cur->maxs[2];

			cur=cur->nnext;
		}
	}

	BTGE_BrushBSP_BBoxVisR(node->lnode, vis, mins, maxs);
	BTGE_BrushBSP_BBoxVisR(node->rnode, vis, mins, maxs);
}

// BTGE_API void BTGE_BrushBSP_BBoxVis(BTGE_BrushNode *root, byte *vis,
	float *mins, float *maxs)
{
	mins[0]=999999;		mins[1]=999999;		mins[2]=999999;
	maxs[0]=-999999;	maxs[1]=-999999;	maxs[2]=-999999;

	BTGE_BrushBSP_BBoxVisR(root, vis, mins, maxs);
}
#endif


BTGE_API BTGE_Brush *BTGE_BrushBSP_BoxQueryTreeNode(BTGE_BrushNode *node,
	float *mins, float *maxs)
{
	float org[3], sz[3];
	BTGE_Brush *lst, *nxt, *cur;
	float rad;

	org[0]=(mins[0]+maxs[0])*0.5;
	org[1]=(mins[1]+maxs[1])*0.5;
	org[2]=(mins[2]+maxs[2])*0.5;
	sz[0]=maxs[0]-org[0];
	sz[1]=maxs[1]-org[1];
	sz[2]=maxs[2]-org[2];
	rad=V3F_LEN(sz);

	cur=BTGE_BrushBSP_QueryTreeNodeR(node, NULL, org, rad);
	
	lst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		if(BTGE_Brush_CheckBoxBrush(cur, mins, maxs))
		{
			cur->chain=lst;
			lst=cur;
		}
		cur=nxt;
	}
	return(lst);
}
