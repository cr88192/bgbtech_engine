#include <btgesv.h>

BTGE_BrushTempPortal *BTGE_BrushBSP_QueryPortalsNode(
	BTGE_BrushTempPortal *prt, BTGE_BrushNode *node)
{
	BTGE_BrushTempPortal *cur, *lst;

#if 0
	cur=prt; lst=NULL;
	while(cur)
	{
		if(cur->lnode==node)
			{ cur->chain=lst; lst=cur; cur=cur->next; continue; }
		if(cur->rnode==node)
			{ cur->chain=lst; lst=cur; cur=cur->next; continue; }
		cur=cur->next;
	}
#endif

	cur=node->prt; lst=NULL;
	while(cur)
	{
		if(cur->lnode==node)
			{ cur->chain=lst; lst=cur; cur=cur->lnext; continue; }
		if(cur->rnode==node)
			{ cur->chain=lst; lst=cur; cur=cur->rnext; continue; }
		break;
	}

	return(lst);
}

BTGE_BrushTempPortal *BTGE_BrushBSP_QueryPortalsNodeList(
	BTGE_BrushTempPortal *prt, BTGE_BrushNode *node,
	BTGE_BrushTempPortal *lst)
{
	BTGE_BrushTempPortal *cur, *cur1;

	cur=node->prt;
	while(cur)
	{
		if(cur->lnode==node)
		{
			cur1=lst;
			while(cur1) { if(cur==cur1)break; cur1=cur1->chain; }
			if(cur1) { cur=cur->lnext; continue; }

			cur->chain=lst; lst=cur;
			cur=cur->lnext;
			continue;
		}

		if(cur->rnode==node)
		{
			cur1=lst;
			while(cur1) { if(cur==cur1)break; cur1=cur1->chain; }
			if(cur1) { cur=cur->rnext; continue; }

			cur->chain=lst; lst=cur;
			cur=cur->rnext;
			continue;
		}

		break;
	}
	return(lst);

#if 0
	cur=prt;
	while(cur)
	{
		if((cur->lnode==node) || (cur->rnode==node))
		{
			cur1=lst;
			while(cur1) { if(cur==cur1)break; cur1=cur1->chain; }
			if(cur1) { cur=cur->next; continue; }

			cur->chain=lst; lst=cur;
			cur=cur->next;
			continue;
		}
		cur=cur->next;
	}
	return(lst);
#endif
}

int BTGE_BrushBSP_CheckPortalAOrg(BTGE_BrushTempPortal *prt,
	BTGE_BrushTempPortal *prta, float *org)
{
	float orga[3], orgb[3];
	float dir[4], rt[4], up[4], box[8];
	float f, g, d, d1;
	int i;

	V3F_ZERO(orga);
	for(i=0; i<prta->n_pts; i++) { V3F_ADD(orga, prta->pts+i*3, orga); }
	V3F_SCALE(orga, 1.0/prta->n_pts, orga);

	V3F_ZERO(orgb);
	for(i=0; i<prt->n_pts; i++) { V3F_ADD(orgb, prt->pts+i*3, orgb); }
	V3F_SCALE(orgb, 1.0/prt->n_pts, orgb);

	V3F_SUB(orga, org, dir);
	V3F_NORMALIZE(dir, dir);
	dir[3]=V3F_DOT(org, dir);

	f=V3F_NDOT(orgb, dir)-V3F_NDOT(orga, dir);
	if(f<=0)return(0);

//	return(1);

	Hull_AdjacentNormals(dir, rt, up);
	rt[3]=V3F_DOT(org, rt);
	up[3]=V3F_DOT(org, up);

	box[0]=999999; box[1]=-999999;
	box[2]=999999; box[3]=-999999;
	for(i=0; i<prta->n_pts; i++)
	{
		d=V3F_NDOT(prta->pts+i*3, dir);
		if(d<1)continue;
		d=1;

		f=V3F_NDOT(prta->pts+i*3, rt)/d;
		if(f<box[0])box[0]=f; if(f>box[1])box[1]=f;
		f=V3F_NDOT(prta->pts+i*3, up)/d;
		if(f<box[2])box[2]=f; if(f>box[3])box[3]=f;
	}

	box[4]=999999; box[5]=-999999;
	box[6]=999999; box[7]=-999999;
	for(i=0; i<prt->n_pts; i++)
	{
		d=V3F_NDOT(prt->pts+i*3, dir);
		if(d1<1)continue;
		d=1;

		f=V3F_NDOT(prt->pts+i*3, rt)/d;
		if(f<box[4])box[4]=f; if(f>box[5])box[5]=f;
		f=V3F_NDOT(prt->pts+i*3, up)/d;
		if(f<box[6])box[6]=f; if(f>box[7])box[7]=f;
	}

	//0 1 2 3 -> 0 2 1 3
	//4 5 6 7 -> 4 6 5 7

	if(box[0]>box[5])return(0);
	if(box[2]>box[7])return(0);

	if(box[1]<box[4])return(0);
	if(box[3]<box[6])return(0);

	return(1);
}

int BTGE_BrushBSP_CheckPortalAB(BTGE_BrushTempPortal *prt,
	BTGE_BrushTempPortal *prta, BTGE_BrushTempPortal *prtb)
{
	float org[3];
	int i;

//	return(1);

	V3F_ZERO(org);
	for(i=0; i<prta->n_pts; i++) { V3F_ADD(org, prta->pts+i*3, org); }
	V3F_SCALE(org, 1.0/prta->n_pts, org);

	i=BTGE_BrushBSP_CheckPortalAOrg(prt, prtb, org);
	return(i);
}

#if 0
int BTGE_BrushBSP_CheckPortalAB(BTGE_BrushTempPortal *prt,
	BTGE_BrushTempPortal *prta, BTGE_BrushTempPortal *prtb)
{
	float orga[3], orgb[3], orgc[3];
	float dir[3], rt[3], up[3], box[8];
	float d, d1;
	int i;

	V3F_ZERO(orga);
	for(i=0; i<prta->n_pts; i++) { V3F_ADD(orga, prta->pts+i*3, orga); }
	V3F_SCALE(orga, 1.0/prta->n_pts, orga);

	V3F_ZERO(orgb);
	for(i=0; i<prtb->n_pts; i++) { V3F_ADD(orgb, prtb->pts+i*3, orgb); }
	V3F_SCALE(orgb, 1.0/prtb->n_pts, orgb);

	V3F_ZERO(orgc);
	for(i=0; i<prt->n_pts; i++) { V3F_ADD(orgc, prt->pts+i*3, orgc); }
	V3F_SCALE(orgc, 1.0/prt->n_pts, orgc);

	V3F_SUB(orgb, orga, dir);
	d=V3F_NORMALIZE(dir, dir);

	d1=V3F_DOT(orgc, dir)-V3F_DOT(orga, dir);

	Hull_AdjacentNormals(dir, rt, up);

	BTGE_BrushBSP_PortalExtents(prtb, rt, &box[0], &box[2]);
	BTGE_BrushBSP_PortalExtents(prtb, up, &box[1], &box[3]);

	BTGE_BrushBSP_PortalExtents(prt, rt, &box[4], &box[6]);
	BTGE_BrushBSP_PortalExtents(prt, up, &box[5], &box[7]);

	box[0]/=d; box[1]/=d; box[2]/=d; box[3]/=d;
	box[4]/=d1; box[5]/=d1; box[6]/=d1; box[7]/=d1;

	if(box[0]>box[6])return(0);
	if(box[1]>box[7])return(0);

	if(box[2]<box[4])return(0);
	if(box[3]<box[5])return(0);


	return(1);
}
#endif

BTGE_BrushTempPortal *BTGE_BrushBSP_QueryPortalsNodeListClipAB(
	BTGE_BrushTempPortal *prt, BTGE_BrushNode *node,
	BTGE_BrushTempPortal *lst,
	BTGE_BrushTempPortal *prta, BTGE_BrushTempPortal *prtb)
{
	BTGE_BrushTempPortal *cur, *cur1;

	cur=node->prt;
	while(cur)
	{
		if(cur->lnode==node)
		{
			if(!BTGE_BrushBSP_CheckPortalAB(cur, prta, prtb))
				{ cur=cur->lnext; continue; }

			cur1=lst;
			while(cur1) { if(cur==cur1)break; cur1=cur1->chain; }
			if(cur1) { cur=cur->lnext; continue; }

			cur->chain=lst; lst=cur;
			cur=cur->lnext;
			continue;
		}

		if(cur->rnode==node)
		{
			if(!BTGE_BrushBSP_CheckPortalAB(cur, prta, prtb))
				{ cur=cur->rnext; continue; }

			cur1=lst;
			while(cur1) { if(cur==cur1)break; cur1=cur1->chain; }
			if(cur1) { cur=cur->rnext; continue; }

			cur->chain=lst; lst=cur;
			cur=cur->rnext;
			continue;
		}

		break;
	}
	return(lst);

#if 0
	cur=prt;
	while(cur)
	{
		if((cur->lnode==node) || (cur->rnode==node))
		{
			if(!BTGE_BrushBSP_CheckPortalAB(cur, prta, prtb))
				{ cur=cur->next; continue; }

			cur1=lst;
			while(cur1) { if(cur==cur1)break; cur1=cur1->chain; }
			if(cur1) { cur=cur->next; continue; }

			cur->chain=lst; lst=cur;
			cur=cur->next;
			continue;
		}
		cur=cur->next;
	}
	return(lst);
#endif
}

int BTGE_BrushBSP_GetVis(byte *vis, int id)
{
	if(vis[id>>3]&(1<<(id&7)))
		return(1);
	return(0);
}

void BTGE_BrushBSP_SetVis(byte *vis, int id)
{
	vis[id>>3]|=1<<(id&7);
}

void BTGE_BrushBSP_BuildVisNodePrtB(BTGE_BrushNode *root,
	BTGE_BrushNode *node, BTGE_BrushTempPortal *prt, byte *vis,
	BTGE_BrushTempPortal *plst,
	BTGE_BrushTempPortal *pcura, BTGE_BrushTempPortal *pcurb)
{
	static int r_d=0;
	BTGE_BrushTempPortal *lst, *cur;
	BTGE_BrushNode *tnode;

	tnode=(node==pcurb->lnode)?pcurb->rnode:pcurb->lnode;
	if(!tnode)return;

	if(r_d>16)return;
	r_d++;

	BTGE_BrushBSP_SetVis(vis, tnode->id);
	lst=BTGE_BrushBSP_QueryPortalsNodeListClipAB(
		prt, tnode, plst, pcura, pcurb);

	cur=lst;
	while(cur && (cur!=plst))
	{
		BTGE_BrushBSP_BuildVisNodePrtB(
			root, tnode, prt, vis, lst, pcura, cur);
		cur=cur->chain;
	}

	r_d--;
}

void BTGE_BrushBSP_BuildVisNodePrt(BTGE_BrushNode *root,
	BTGE_BrushNode *node, BTGE_BrushTempPortal *prt, byte *vis,
	BTGE_BrushTempPortal *plst, BTGE_BrushTempPortal *pcur)
{
	BTGE_BrushTempPortal *lst, *cur;
	BTGE_BrushNode *tnode;

	tnode=(node==pcur->lnode)?pcur->rnode:pcur->lnode;
	if(!tnode)return;

	BTGE_BrushBSP_SetVis(vis, tnode->id);
	lst=BTGE_BrushBSP_QueryPortalsNodeList(prt, tnode, plst);

	cur=lst;
	while(cur && (cur!=plst))
	{
		BTGE_BrushBSP_BuildVisNodePrtB(
			root, tnode, prt, vis, lst, pcur, cur);
		cur=cur->chain;
	}
}

void BTGE_BrushBSP_BuildVisNode(BTGE_BrushNode *root,
	BTGE_BrushNode *node,
	BTGE_BrushTempPortal *prt)
{
	byte tvis[4096];
	BTGE_BrushTempPortal *lst, *cur;
	int i, sz;

	i=BTGE_BrushBSP_LastNodeID(root);
	sz=(i+7)/8;

	printf("Node %d/%d\r", node->id, i);

	memset(tvis, 0, 4096);
//	BTGE_BrushBSP_BuildVisNodeR(root, node, root, tvis);

	BTGE_BrushBSP_SetVis(tvis, node->id);

	lst=BTGE_BrushBSP_QueryPortalsNode(prt, node);
	cur=lst;
	while(cur)
	{
		BTGE_BrushBSP_BuildVisNodePrt(root, node, prt, tvis, lst, cur);
		cur=cur->chain;
	}

	node->vis=gcatomic(sz);
	memcpy(node->vis, tvis, sz);
}

void BTGE_BrushBSP_BuildVisR(BTGE_BrushNode *root,
	BTGE_BrushNode *node,
	BTGE_BrushTempPortal *prt)
{
	if(!node)return;

	if(!node->lnode)
	{
		BTGE_BrushBSP_BuildVisNode(root, node, prt);
		return;
	}

	BTGE_BrushBSP_BuildVisR(root, node->lnode, prt);
	BTGE_BrushBSP_BuildVisR(root, node->rnode, prt);
}

void BTGE_BrushBSP_BuildVis(BTGE_BrushNode *root,
	BTGE_BrushTempPortal *prt)
{
	printf("Build Vis-2\n");
	BTGE_BrushBSP_BuildVisR(root, root, prt);
	printf("Build Vis: OK\n");
}


BTGE_BrushNode *BTGE_BrushBSP_LookupLeafPoint(
	BTGE_BrushNode *node, float *org)
{
	float f;

	if(!node->lnode)return(node);

	f=V3F_NDOT(org, node->norm);
	if(f<0)return(BTGE_BrushBSP_LookupLeafPoint(node->lnode, org));
	if(f>=0)return(BTGE_BrushBSP_LookupLeafPoint(node->rnode, org));
	return(node);
}

int BTGE_BrushBSP_GetPVSLeafID(BTGE_BrushNode *root, float *org)
{
	BTGE_BrushNode *node;
	node=BTGE_BrushBSP_LookupLeafPoint(root, org);
	return(node->id);
}

int BTGE_BrushBSP_CheckLeafVisBBox(
	BTGE_BrushNode *node, float *mins, float *maxs, byte *vis)
{
	float f, g;
	int i;

	if(!node->lnode)
	{
		i=BTGE_BrushBSP_GetVis(vis, node->id);
		return(i);
	}

	Hull_BoxPlaneExtents(mins, maxs, node->norm, &f, &g);

	if(g<0)
	{
		i=BTGE_BrushBSP_CheckLeafVisBBox(
			node->lnode, mins, maxs, vis);
		return(i);
	}
	if(f>0)
	{
		i=BTGE_BrushBSP_CheckLeafVisBBox(
			node->rnode, mins, maxs, vis);
		return(i);
	}

	i=BTGE_BrushBSP_CheckLeafVisBBox(node->lnode, mins, maxs, vis);
	if(i)return(i);
	i=BTGE_BrushBSP_CheckLeafVisBBox(node->rnode, mins, maxs, vis);
	return(i);
}


int BTGE_BrushBSP_CheckBrushPVS(BTGE_BrushNode *root,
	BTGE_Brush *brush, float *org)
{
	BTGE_BrushNode *tnode;
	int i, j;

//	if(!brush->node)return(1);
//	if(!brush->node->vis)return(1);

	tnode=BTGE_BrushBSP_LookupLeafPoint(root, org);
	if(!tnode->vis)return(1);

	i=BTGE_BrushBSP_CheckLeafVisBBox(root,
		brush->mins, brush->maxs, tnode->vis);
	return(i);

//	i=brush->node->id;
//	j=tnode->vis[i>>3]&(1<<(i&7));
//	if(j)return(1);
//	return(0);
}

int BTGE_BrushBSP_CheckPointPVS(BTGE_BrushNode *root,
	float *pt, float *org)
{
	BTGE_BrushNode *anode, *bnode;
	int i, j;

	anode=BTGE_BrushBSP_LookupLeafPoint(root, pt);
	if(!anode->vis)return(1);

	bnode=BTGE_BrushBSP_LookupLeafPoint(root, org);
	if(!bnode->vis)return(1);

	i=anode->id;
	j=bnode->vis[i>>3]&(1<<(i&7));
	if(j)return(1);

	i=bnode->id;
	j=anode->vis[i>>3]&(1<<(i&7));
	if(j)return(1);

	return(0);
}

