#include <lbxgl2.h>

LBXGL_BrushNode *lbxgl_bbsp_nodefree;
int lbxgl_bbsp_tseq;

LBXGL_API LBXGL_BrushNode *LBXGL_BrushBSP_AllocNode()
{
	LBXGL_BrushNode *tmp;
	if(lbxgl_bbsp_nodefree)
	{
		tmp=lbxgl_bbsp_nodefree;
		lbxgl_bbsp_nodefree=tmp->lnode;

		memset(tmp, 0, sizeof(LBXGL_BrushNode));

		tmp->lnode=NULL;
		tmp->rnode=NULL;
		tmp->mid=NULL;
		return(tmp);
	}

	tmp=gcalloc(sizeof(LBXGL_BrushNode));
	return(tmp);
}

LBXGL_API void LBXGL_BrushBSP_FreeNode(LBXGL_BrushNode *tmp)
{
	if(tmp->vis)gcfree(tmp->vis);
	tmp->vis=NULL;

	tmp->lnode=NULL;
	tmp->rnode=NULL;
	tmp->mid=NULL;
	tmp->lnode=lbxgl_bbsp_nodefree;
	lbxgl_bbsp_nodefree=tmp;
}

LBXGL_API void LBXGL_BrushBSP_FreeTreeNode(LBXGL_BrushNode *tmp)
{
	if(!tmp)return;
	LBXGL_BrushBSP_FreeTreeNode(tmp->lnode);
	LBXGL_BrushBSP_FreeTreeNode(tmp->rnode);
	LBXGL_BrushBSP_FreeNode(tmp);
}

LBXGL_API void LBXGL_BrushBSP_BBoxList(LBXGL_Brush *lst, float *mins, float *maxs)
{
	LBXGL_Brush *cur;

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

LBXGL_API void LBXGL_BrushBSP_SphereList(LBXGL_Brush *lst,
	float *org, float *rad)
{
	LBXGL_Brush *cur;
	float f, g;
	int i;

	V3F_ZERO(org);
	*rad=0;

	cur=lst; i=0;
	while(cur)
	{
		V3F_ADD(org, cur->org, org);
		i++;
		cur=cur->chain;
	}
	if(i<=0)return;
	V3F_SCALE(org, 1.0/i, org);

	cur=lst; g=0;
	while(cur)
	{
		f=V3F_DIST(org, cur->org)+cur->rad;
		if(f>g)g=f;
		cur=cur->chain;
	}
	*rad=g;
}

#if 1
LBXGL_API float LBXGL_BrushBSP_WeightPlaneBrushList(LBXGL_Brush *lst, float *norm)
{
	LBXGL_Brush *cur;
	int nl, nr, nm;
	float f, g;

	cur=lst; nl=0; nr=0; nm=0;
	while(cur)
	{
		LBXGL_Brush_GetPlaneExtents(cur, norm, &f, &g);
		if(g<=0)nl++; if(f>=0)nr++;
		if((f<0) && (g>0))nm++;
		cur=cur->chain;
	}

	if(!nl || !nr)return(999999.0);

	f=fabs(nl-nr)+(nm*0.25);
	return(f);
}

LBXGL_API int LBXGL_BrushBSP_BestPlaneBrushList(LBXGL_Brush *lst, float *norm)
{
	float bn[4];
	LBXGL_Brush *cur;
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
			f=LBXGL_BrushBSP_WeightPlaneBrushList(
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

LBXGL_API LBXGL_BrushNode *LBXGL_BrushBSP_BuildNodeList(LBXGL_Brush *lst)
{
	float org[3], dir[4];
	LBXGL_BrushNode *tmp;
	LBXGL_Brush *cur, *nxt, *llst, *rlst, *mlst;
	float f, g;
	int i, j, k;

	tmp=LBXGL_BrushBSP_AllocNode();
	tmp->id=lbxgl_bbsp_tseq++;

	i=LBXGL_BrushBSP_BestPlaneBrushList(lst, dir);

	if(i<0)
	{
		LBXGL_BrushBSP_BBoxList(lst, tmp->mins, tmp->maxs);

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
		LBXGL_Brush_GetPlaneExtents(cur, dir, &f, &g);
		if(g<=0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>=0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	tmp->lnode=LBXGL_BrushBSP_BuildNodeList(llst);
	tmp->rnode=LBXGL_BrushBSP_BuildNodeList(rlst);
	tmp->lnode->pnode=tmp;
	tmp->rnode->pnode=tmp;

	LBXGL_BrushBSP_BBoxList(mlst, tmp->mins, tmp->maxs);
	LBXGL_BrushBSP_SphereList(mlst, tmp->org, &(tmp->rad));

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
LBXGL_API LBXGL_BrushNode *LBXGL_BrushBSP_BuildNodeListFast(LBXGL_Brush *lst)
{
	static int rcnt=0;
	float org[3], dir[4], dx[3], dy[3], dz[3];
	LBXGL_BrushNode *tmp;
	LBXGL_Brush *cur, *nxt, *llst, *rlst, *mlst;
	float f, g, h;
	int i, j, k;

	tmp=LBXGL_BrushBSP_AllocNode();
	tmp->id=lbxgl_bbsp_tseq++;

	if(!lst || !lst->chain || (rcnt>=64))
	{
		LBXGL_BrushBSP_BBoxList(lst, tmp->mins, tmp->maxs);
		LBXGL_BrushBSP_SphereList(lst, tmp->org, &(tmp->rad));

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
		LBXGL_Brush_GetPlaneExtents(cur, dir, &f, &g);
		if(g<0)i++; if(f>0)j++;
		if(i && j) break;
		cur=cur->chain;
	}

	if(!i || !j)
	{
		LBXGL_BrushBSP_BBoxList(lst, tmp->mins, tmp->maxs);
		LBXGL_BrushBSP_SphereList(lst, tmp->org, &(tmp->rad));

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
		LBXGL_Brush_GetPlaneExtents(cur, dir, &f, &g);
		if(g<0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	rcnt++;
	tmp->lnode=LBXGL_BrushBSP_BuildNodeListFast(llst);
	tmp->rnode=LBXGL_BrushBSP_BuildNodeListFast(rlst);
	tmp->lnode->pnode=tmp;
	tmp->rnode->pnode=tmp;
	rcnt--;

	LBXGL_BrushBSP_BBoxList(mlst, tmp->mins, tmp->maxs);
	LBXGL_BrushBSP_SphereList(mlst, tmp->org, &(tmp->rad));

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

LBXGL_API int LBXGL_BrushBSP_CheckPointNode(
	LBXGL_BrushNode *node, float *pt)
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

LBXGL_API int LBXGL_BrushBSP_LastNodeID(LBXGL_BrushNode *node)
{
	if(node->rnode)
		return(LBXGL_BrushBSP_LastNodeID(node->rnode));
	return(node->id);
}

LBXGL_API LBXGL_BrushNode *LBXGL_BrushBSP_BuildTreeListFast(LBXGL_Brush *lst)
{
	LBXGL_BrushNode *tmp;
	LBXGL_BrushTempPortal *prt;

//	printf("Build BSP\n");

	lbxgl_bbsp_tseq=1;
	tmp=LBXGL_BrushBSP_BuildNodeListFast(lst);

//	printf("Build BSP: OK, %d nodes\n", lbxgl_bbsp_tseq);

	return(tmp);
}


#if 1
LBXGL_API float LBXGL_BrushBSP_WeightPlaneFaceList(LBXGL_BrushTempFace *lst, float *norm)
{
	LBXGL_BrushTempFace *cur;
	int nl, nr, nm;
	float f, g;

	cur=lst; nl=0; nr=0; nm=0;
	while(cur)
	{
		LBXGL_BrushCSG_FacePlaneExtents(cur, norm, &f, &g);
		if(g<=0)nl++; if(f>=0)nr++;
		if((f<0) && (g>0))nm++;
		cur=cur->next;
	}

	if(!nl || !nr)return(999999.0);

	f=fabs(nl-nr)+(nm*0.25);
	return(f);
}

LBXGL_API int LBXGL_BrushBSP_BestPlaneFaceList(LBXGL_BrushTempFace *lst, float *norm)
{
	float bn[4];
	LBXGL_BrushTempFace *cur;
	float f, bf;
	int i;

	if(!lst || !lst->next)
		return(-1);

	V4F_ZERO(bn); bf=999999.0;

	cur=lst;
	while(cur)
	{
		if(cur->flags&LBXGL_TXFL_NOPLANE)
			{ cur=cur->next; continue; }

		f=LBXGL_BrushBSP_WeightPlaneFaceList(lst, cur->norm);
		if(f<bf) { V4F_COPY(cur->norm, bn); bf=f; }
		cur=cur->next;
	}
	if(bf>=900000.0)
		return(-1);

	V4F_COPY(bn, norm);
	return(0);
}

LBXGL_API LBXGL_BrushNode *LBXGL_BrushBSP_BuildNodeFaceList(LBXGL_BrushTempFace *lst)
{
	float org[3], dir[4];
	LBXGL_BrushNode *tmp;
	LBXGL_BrushTempFace *cur, *nxt, *llst, *rlst, *mlst;
	float f, g;
	int i, j, k;

	tmp=LBXGL_BrushBSP_AllocNode();
	tmp->id=lbxgl_bbsp_tseq++;

	i=LBXGL_BrushBSP_BestPlaneFaceList(lst, dir);


	if(i<0)
//	if(1)
	{
//		printf("Leaf\n");

		LBXGL_BrushCSG_BBoxFaceList(lst, tmp->mins, tmp->maxs);

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

	LBXGL_BrushCSG_SplitListFacePlane(lst, dir, &llst, &rlst);

	if(!llst || !rlst)
	{
//		printf("Node: Split Fail %p %p\n", llst, rlst);
//		return(NULL);
	}

	tmp->lnode=LBXGL_BrushBSP_BuildNodeFaceList(llst);
	tmp->rnode=LBXGL_BrushBSP_BuildNodeFaceList(rlst);
	tmp->lnode->pnode=tmp;
	tmp->rnode->pnode=tmp;

//	printf("Node OK\n");

	return(tmp);
}
#endif

LBXGL_API void LBXGL_BrushBSP_BindNodeBrushList(LBXGL_BrushNode *node, LBXGL_Brush *lst)
{
	LBXGL_Brush *cur, *nxt, *llst, *rlst, *mlst;
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
		LBXGL_Brush_GetPlaneExtents(cur, node->norm, &f, &g);
		if(g<0) { cur->chain=llst; llst=cur; cur=nxt; continue; }
		if(f>0) { cur->chain=rlst; rlst=cur; cur=nxt; continue; }
		cur->chain=mlst; mlst=cur; cur=nxt; continue;
	}

	LBXGL_BrushBSP_BindNodeBrushList(node->lnode, llst);
	LBXGL_BrushBSP_BindNodeBrushList(node->rnode, rlst);

	cur=mlst;
	while(cur)
	{
		cur->node=node;
		cur->nnext=node->mid;
		node->mid=cur;
		cur=cur->chain;
	}
}

LBXGL_API LBXGL_BrushNode *LBXGL_BrushBSP_BuildTreeList(LBXGL_Brush *lst)
{
	LBXGL_BrushNode *tmp;
	LBXGL_BrushTempPortal *prt;
	LBXGL_BrushTempFace *flst, *fcur;
	int i;

	printf("Build BSP\n");

	lbxgl_bbsp_tseq=1;
	tmp=LBXGL_BrushBSP_BuildNodeList(lst);
	LBXGL_BrushCSG_BuildFaces(tmp);

//	LBXGL_BrushBSP_BuildVis(tmp);
//	prt=LBXGL_BrushBSP_BuildPortals(tmp);
//	LBXGL_BrushBSP_BuildVis(tmp, prt);

#if 0
	printf("Build CSG Faces\n");

	flst=LBXGL_BrushCSG_MakeFacesBrushList(lst);
//	flst=LBXGL_BrushCSG_ClipFaceListBrushList(flst, lst);

	fcur=flst; i=0;
	while(fcur) { i++; fcur=fcur->next; }
	printf("Build CSG Faces: OK, %d faces\n", i);

	tmp=LBXGL_BrushBSP_BuildNodeFaceList(flst);
	LBXGL_BrushBSP_BindNodeBrushList(tmp, lst);
#endif

	printf("Build BSP: OK, %d nodes\n", lbxgl_bbsp_tseq);

	return(tmp);
}

LBXGL_Brush *LBXGL_BrushBSP_QueryTreeNodeR(LBXGL_BrushNode *node,
	LBXGL_Brush *lst, float *org, float rad)
{
	float dir[4];
	LBXGL_Brush *cur;
	float f, g;

	if(!node)return(lst);

	cur=node->mid;
	if(cur)
	{
		if(!LBXGL_Hull_CheckSphereBBox(org, rad, node->mins, node->maxs))
		{
			cur=NULL;
		}else
		{
			f=V3F_DIST(org, node->org);
			if(f>=(rad+node->rad))
				cur=NULL;
		}
	}

//	cur=node->mid;
	while(cur)
	{
#if 0
		V3F_SUB(cur->org, org, dir);
//		f=V3F_NORMALIZE(dir, dir);
		g=V3F_FRNORMALIZE(dir, dir);
		f=(g>0)?(1.0/g):0;
		if(f>(cur->rad+rad)) { cur=cur->nnext; continue; }

#if 1
		dir[3]=V3F_DOT(org, dir);
		LBXGL_Brush_GetPlaneExtents(cur, dir, &f, &g);
//		LBXGL_Brush_GetBBoxPlaneExtents(cur, dir, &f, &g);
		if(f>=rad) { cur=cur->nnext; continue; }
#endif
#endif

#if 0
		V3F_SUB(cur->org, org, dir);
		f=V3F_LEN(dir);
		if(f>=(cur->rad+rad)) { cur=cur->nnext; continue; }
#endif

#if 1
		if(!LBXGL_Brush_CheckSphereBrush(cur, org, rad))
			{ cur=cur->nnext; continue; }
#endif

		cur->chain=lst; lst=cur;
		cur=cur->nnext;
	}

	if(!node->lnode)
		return(lst);

	f=V3F_NDOT(org, node->norm);
	if(fabs(f)<rad)
	{
		lst=LBXGL_BrushBSP_QueryTreeNodeR(node->lnode, lst, org, rad);
		lst=LBXGL_BrushBSP_QueryTreeNodeR(node->rnode, lst, org, rad);
		return(lst);
	}

	if(f<0)
	{
		lst=LBXGL_BrushBSP_QueryTreeNodeR(node->lnode, lst, org, rad);
		return(lst);
	}

	if(f>0)
	{
		lst=LBXGL_BrushBSP_QueryTreeNodeR(node->rnode, lst, org, rad);
		return(lst);
	}

	return(lst);
}

LBXGL_API LBXGL_Brush *LBXGL_BrushBSP_QueryTreeNode(LBXGL_BrushNode *node,
	float *org, float rad)
{
	LBXGL_Brush *lst;
	lst=LBXGL_BrushBSP_QueryTreeNodeR(node, NULL, org, rad);
	return(lst);
}


LBXGL_API LBXGL_Brush *LBXGL_BrushBSP_TracelineFirst(LBXGL_BrushNode *node,
	float *sv, float *ev)
{
	float pt[4];
	LBXGL_Brush *cur;
	float f, g;

	if(!node)return(NULL);

	cur=node->mid;
	while(cur)
	{
		if(LBXGL_Brush_CheckLineBrush(cur, sv, ev))
			return(cur);
		cur=cur->nnext;
	}

	if(!node->lnode)return(NULL);

	f=V3F_NDOT(sv, node->norm);
	g=V3F_NDOT(ev, node->norm);

	if((f<0) && (g>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		cur=LBXGL_BrushBSP_TracelineFirst(node->lnode, sv, pt);
		if(cur)return(cur);
		cur=LBXGL_BrushBSP_TracelineFirst(node->rnode, pt, ev);
		return(cur);
	}

	if((g<0) && (f>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		cur=LBXGL_BrushBSP_TracelineFirst(node->rnode, sv, pt);
		if(cur)return(cur);
		cur=LBXGL_BrushBSP_TracelineFirst(node->lnode, pt, ev);
		return(cur);
	}

	if((f==0) && (g==0))return(NULL);

	if((f<=0) && (g<=0))
	{
		cur=LBXGL_BrushBSP_TracelineFirst(node->lnode, sv, ev);
		return(cur);
	}

	if((f>=0) && (g>=0))
	{
		cur=LBXGL_BrushBSP_TracelineFirst(node->rnode, sv, ev);
		return(cur);
	}

	return(NULL);
}

LBXGL_API LBXGL_Brush *LBXGL_BrushBSP_TracelineFirstSkip(LBXGL_BrushNode *node,
	float *sv, float *ev, LBXGL_Brush *skip)
{
	float pt[4];
	LBXGL_Brush *cur, *cur1;
	float f, g;

	if(!node)return(NULL);

	cur=node->mid;
	while(cur)
	{
		if(LBXGL_Brush_CheckLineBrush(cur, sv, ev))
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
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->lnode,
			sv, pt, skip);
		if(cur)return(cur);
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->rnode,
			pt, ev, skip);
		return(cur);
	}

	if((g<0) && (f>0))
	{
		Hull_LinePlaneIntersect(sv, ev, node->norm, pt);
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->rnode,
			sv, pt, skip);
		if(cur)return(cur);
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->lnode,
			pt, ev, skip);
		return(cur);
	}

	if((f==0) && (g==0))
	{
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->lnode,
			sv, ev, skip);
		if(cur)return(cur);
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->rnode,
			sv, ev, skip);
		return(cur);

//		return(NULL);
	}

	if((f<=0) && (g<=0))
	{
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->lnode,
			sv, ev, skip);
		return(cur);
	}

	if((f>=0) && (g>=0))
	{
		cur=LBXGL_BrushBSP_TracelineFirstSkip(node->rnode,
			sv, ev, skip);
		return(cur);
	}

	return(NULL);
}

LBXGL_Brush *LBXGL_BrushBSP_GetBrushListR(
	LBXGL_BrushNode *node, LBXGL_Brush *lst)
{
	LBXGL_Brush *cur;

	if(!node)return(lst);

	cur=node->mid;
	while(cur)
	{
		cur->chain=lst; lst=cur;
		cur=cur->nnext;
	}

	lst=LBXGL_BrushBSP_GetBrushListR(node->lnode, lst);
	lst=LBXGL_BrushBSP_GetBrushListR(node->rnode, lst);

	return(lst);
}

LBXGL_API LBXGL_Brush *LBXGL_BrushBSP_GetBrushList(LBXGL_BrushNode *root)
	{ return(LBXGL_BrushBSP_GetBrushListR(root, NULL)); }


void LBXGL_BrushBSP_BBoxVisR(LBXGL_BrushNode *node, byte *vis,
	float *mins, float *maxs)
{
	LBXGL_Brush *cur;
	int i;

	if(!node)return;

	i=node->id;
	if(vis[i>>3]&(1<<(i&7)))
	{
		cur=node->mid;
		while(cur)
		{
			if(!LBXGL_Brush_CheckBrushFrustum(cur))
				{ cur=cur->nnext; continue; }
			if(cur->flags&LBXGL_BRFL_VISCLIP)
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

	LBXGL_BrushBSP_BBoxVisR(node->lnode, vis, mins, maxs);
	LBXGL_BrushBSP_BBoxVisR(node->rnode, vis, mins, maxs);
}

LBXGL_API void LBXGL_BrushBSP_BBoxVis(LBXGL_BrushNode *root, byte *vis,
	float *mins, float *maxs)
{
	mins[0]=999999;		mins[1]=999999;		mins[2]=999999;
	maxs[0]=-999999;	maxs[1]=-999999;	maxs[2]=-999999;

	LBXGL_BrushBSP_BBoxVisR(root, vis, mins, maxs);
}

LBXGL_Brush *LBXGL_BrushBSP_QueryTreeNodeVisibleMid(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_Brush *lst, float *org, float rad)
{
	float dir[4];

	LBXGL_Brush *cur;
	float f, g;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return(lst);

	if(!LBXGL_Hull_CheckSphereBBox(org, rad, node->mins, node->maxs))
		return(lst);

	f=V3F_DIST(org, node->org);
	if(f>=(rad+node->rad))
		return(lst);

	cur=node->mid;
	while(cur)
	{
//		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_VISCLIP))
		if(cur->flags&LBXGL_BRFL_VISCLIP)
			{ cur=cur->nnext; continue; }

#if 0
		V3F_SUB(cur->org, org, dir);
//		f=V3F_NORMALIZE(dir, dir);
		g=V3F_FRNORMALIZE(dir, dir);
		f=(g>0)?(1.0/g):0;
		if(f>(cur->rad+rad)) { cur=cur->nnext; continue; }

#if 1
		dir[3]=V3F_DOT(org, dir);
		LBXGL_Brush_GetPlaneExtents(cur, dir, &f, &g);
//		LBXGL_Brush_GetBBoxPlaneExtents(cur, dir, &f, &g);
		if(f>=rad) { cur=cur->nnext; continue; }
#endif
#endif

#if 0
		V3F_SUB(cur->org, org, dir);
		f=V3F_LEN(dir);
		if(f>=(cur->rad+rad)) { cur=cur->nnext; continue; }
#endif

#if 1
		if(!LBXGL_Brush_CheckSphereBrush(cur, org, rad))
			{ cur=cur->nnext; continue; }
#endif

		cur->chain=lst; lst=cur;
		cur=cur->nnext;
	}

	return(lst);
}

LBXGL_Brush *LBXGL_BrushBSP_QueryTreeNodeVisibleR(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_Brush *lst, float *org, float rad)
{
	float f, g;

	if(!node)return(lst);

	lst=LBXGL_BrushBSP_QueryTreeNodeVisibleMid(
		world, node, lst, org, rad);

	if(!node->lnode)
		return(lst);

	f=V3F_NDOT(org, node->norm);
//	if(isnan(f))f=0;
	if(!(f<=0) && !(f>=0))f=0;

	if(fabs(f)<rad)
	{
		lst=LBXGL_BrushBSP_QueryTreeNodeVisibleR(
			world, node->lnode, lst, org, rad);
		lst=LBXGL_BrushBSP_QueryTreeNodeVisibleR(
			world, node->rnode, lst, org, rad);
		return(lst);
	}

	if(f<0)
	{
		lst=LBXGL_BrushBSP_QueryTreeNodeVisibleR(
			world, node->lnode, lst, org, rad);
		return(lst);
	}

	if(f>0)
	{
		lst=LBXGL_BrushBSP_QueryTreeNodeVisibleR(
			world, node->rnode, lst, org, rad);
		return(lst);
	}

	return(lst);
}

LBXGL_API LBXGL_Brush *LBXGL_BrushBSP_QueryTreeNodeVisible(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	float *org, float rad)
{
	LBXGL_Brush *lst;
	lst=LBXGL_BrushBSP_QueryTreeNodeVisibleR(world, node, NULL, org, rad);
	return(lst);
}
