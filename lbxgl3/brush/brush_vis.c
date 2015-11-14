#include <lbxgl2.h>

#define PVS_STEP	8
#define PVS_OVER	4

int LBXGL_BrushBSP_CheckVisNodePoint(LBXGL_BrushNode *root,
	LBXGL_BrushNode *node, float *org, LBXGL_Brush *lst)
{
	float step[3], pt[3];
	float *mins;
	LBXGL_Brush *brsh;
	int i, j, k, l;

	for(i=0; i<3; i++)step[i]=(node->maxs[i]-node->mins[i])/PVS_STEP;

#if 1
	mins=node->mins;
	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*i;
		pt[1]=mins[1]+step[1]*i;
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(node, pt))continue;
		brsh=LBXGL_BrushBSP_TracelineFirstSkip(root, pt, org, lst);
		if(!brsh)return(1);
	}

	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*(PVS_STEP-i-1);
		pt[1]=mins[1]+step[1]*i;
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(node, pt))continue;
		brsh=LBXGL_BrushBSP_TracelineFirstSkip(root, pt, org, lst);
		if(!brsh)return(1);
	}

	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*i;
		pt[1]=mins[1]+step[1]*(PVS_STEP-i-1);
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(node, pt))continue;
		brsh=LBXGL_BrushBSP_TracelineFirstSkip(root, pt, org, lst);
		if(!brsh)return(1);
	}

	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*(PVS_STEP-i-1);
		pt[1]=mins[1]+step[1]*(PVS_STEP-i-1);
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(node, pt))continue;
		brsh=LBXGL_BrushBSP_TracelineFirstSkip(root, pt, org, lst);
		if(!brsh)return(1);
	}
#endif

#if 0
	for(i=0; i<16; i++)for(j=0; j<16; j++)for(k=0; k<16; k++)
	{
		pt[0]=node->mins[0]+step[0]*i;
		pt[1]=node->mins[1]+step[1]*j;
		pt[2]=node->mins[2]+step[2]*k;

		l=LBXGL_BrushBSP_CheckPointNode(node, pt);
		if(!l)continue;

		brsh=LBXGL_BrushBSP_TracelineFirstSkip(root, pt, org, lst);
		if(!brsh)return(1);
	}
#endif

	return(0);
}

int LBXGL_BrushBSP_CheckVisNodeNode(LBXGL_BrushNode *root,
	LBXGL_BrushNode *anode, LBXGL_BrushNode *bnode)
{
	float step[3], pt[3];
	float *mins;
	LBXGL_Brush *cur, *lst;
	int i, j, k, l;

//	return(1);
//	return(0);

//	return(rand()&1);

	if(!anode->mid)return(1);
	if(!bnode->mid)return(1);

	lst=NULL;

#if 0
	cur=anode->mid;
	while(cur) { cur->chain=lst; lst=cur; cur=cur->nnext; }
	cur=bnode->mid;
	while(cur) { cur->chain=lst; lst=cur; cur=cur->nnext; }
#endif

	for(i=0; i<3; i++)step[i]=(anode->maxs[i]-anode->mins[i])/8;
	mins=anode->mins;

#if 1
	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*i;
		pt[1]=mins[1]+step[1]*i;
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(anode, pt))continue;
		l=LBXGL_BrushBSP_CheckVisNodePoint(root, bnode, pt, lst);
		if(l)return(1);
	}

	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*(PVS_STEP-i-1);
		pt[1]=mins[1]+step[1]*i;
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(anode, pt))continue;
		l=LBXGL_BrushBSP_CheckVisNodePoint(root, bnode, pt, lst);
		if(l)return(1);
	}

	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*i;
		pt[1]=mins[1]+step[1]*(PVS_STEP-i-1);
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(anode, pt))continue;
		l=LBXGL_BrushBSP_CheckVisNodePoint(root, bnode, pt, lst);
		if(l)return(1);
	}

	for(i=(-PVS_OVER); i<(PVS_STEP+PVS_OVER); i++)
	{
		pt[0]=mins[0]+step[0]*(PVS_STEP-i-1);
		pt[1]=mins[1]+step[1]*(PVS_STEP-i-1);
		pt[2]=mins[2]+step[2]*i;
		if(!LBXGL_BrushBSP_CheckPointNode(anode, pt))continue;
		l=LBXGL_BrushBSP_CheckVisNodePoint(root, bnode, pt, lst);
		if(l)return(1);
	}
#endif

#if 0
	for(i=0; i<16; i++)for(j=0; j<16; j++)for(k=0; k<16; k++)
	{
		pt[0]=anode->mins[0]+step[0]*i;
		pt[1]=anode->mins[1]+step[1]*j;
		pt[2]=anode->mins[2]+step[2]*k;

		l=LBXGL_BrushBSP_CheckPointNode(anode, pt);
		if(!l)continue;

		l=LBXGL_BrushBSP_CheckVisNodePoint(root, bnode, pt, lst);
		if(l)return(1);
	}
#endif

	return(0);
}

void LBXGL_BrushBSP_BuildVisNodeR(LBXGL_BrushNode *root,
	LBXGL_BrushNode *node, LBXGL_BrushNode *bnode, byte *vis)
{
	int i, j;

	if(!bnode)return;

	if(node==bnode)
	{
		i=node->id;
		vis[i>>3]|=1<<(i&7);

		LBXGL_BrushBSP_BuildVisNodeR(root, node, bnode->lnode, vis);
		LBXGL_BrushBSP_BuildVisNodeR(root, node, bnode->rnode, vis);
		return;
	}

	if(bnode->vis)
	{
		i=node->id;
		j=bnode->vis[i>>3]&(1<<(i&7));
		
		i=bnode->id;
		if(j)vis[i>>3]|=1<<(i&7);
			else vis[i>>3]&=~(1<<(i&7));
	}else
	{
		i=bnode->id;
		j=LBXGL_BrushBSP_CheckVisNodeNode(root, node, bnode);
		if(j)vis[i>>3]|=1<<(i&7);
			else vis[i>>3]&=~(1<<(i&7));
	}

	LBXGL_BrushBSP_BuildVisNodeR(root, node, bnode->lnode, vis);
	LBXGL_BrushBSP_BuildVisNodeR(root, node, bnode->rnode, vis);
}

void LBXGL_BrushBSP_BuildVisNode(LBXGL_BrushNode *root,
	LBXGL_BrushNode *node)
{
	byte tvis[4096];
	int i, sz;

	i=LBXGL_BrushBSP_LastNodeID(root);
	sz=(i+7)/8;

	printf("Node %d/%d\r", node->id, i);

	memset(tvis, 0, 4096);
	LBXGL_BrushBSP_BuildVisNodeR(root, node, root, tvis);


	node->vis=gcatomic(sz);
	memcpy(node->vis, tvis, sz);
}

void LBXGL_BrushBSP_BuildVisR(LBXGL_BrushNode *root,
	LBXGL_BrushNode *node)
{
	if(!node)return;
	LBXGL_BrushBSP_BuildVisNode(root, node);
	LBXGL_BrushBSP_BuildVisR(root, node->lnode);
	LBXGL_BrushBSP_BuildVisR(root, node->rnode);
}

void LBXGL_BrushBSP_BuildVis(LBXGL_BrushNode *root)
{
	printf("Build Vis\n");
	LBXGL_BrushBSP_BuildVisR(root, root);
	printf("Build Vis: OK\n");
}


LBXGL_BrushNode *LBXGL_BrushBSP_LookupLeafPoint(
	LBXGL_BrushNode *node, float *org)
{
	float f;

	if(!node->lnode)return(node);

	f=V3F_NDOT(org, node->norm);
	if(f<0)return(LBXGL_BrushBSP_LookupLeafPoint(node->lnode, org));
	if(f>=0)return(LBXGL_BrushBSP_LookupLeafPoint(node->rnode, org));
	return(node);
}

int LBXGL_BrushBSP_GetPVSLeafID(LBXGL_BrushNode *root, float *org)
{
	LBXGL_BrushNode *node;
	node=LBXGL_BrushBSP_LookupLeafPoint(root, org);
	return(node->id);
}

int LBXGL_BrushBSP_CheckBrushPVS(LBXGL_BrushNode *root,
	LBXGL_Brush *brush, float *org)
{
	LBXGL_BrushNode *tnode;
	int i, j;

	if(!brush->node)return(1);
	if(!brush->node->vis)return(1);

	tnode=LBXGL_BrushBSP_LookupLeafPoint(root, org);
	if(!tnode->vis)return(1);

	i=brush->node->id;
	j=tnode->vis[i>>3]&(1<<(i&7));
	if(j)return(1);

	return(0);
}

int LBXGL_BrushBSP_CheckPointPVS(LBXGL_BrushNode *root,
	float *pt, float *org)
{
	LBXGL_BrushNode *anode, *bnode;
	int i, j;

	anode=LBXGL_BrushBSP_LookupLeafPoint(root, pt);
	if(!anode->vis)return(1);

	bnode=LBXGL_BrushBSP_LookupLeafPoint(root, org);
	if(!bnode->vis)return(1);

	i=anode->id;
	j=bnode->vis[i>>3]&(1<<(i&7));
	if(j)return(1);

	i=bnode->id;
	j=anode->vis[i>>3]&(1<<(i&7));
	if(j)return(1);

	return(0);
}

