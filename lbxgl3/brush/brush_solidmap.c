#include <lbxgl2.h>

int LBXGL_BrushSolid_CheckBBoxBrush(LBXGL_Brush *brush,
	float *mins, float *maxs)
{
	float f, g;
	int i;

	if(brush->maxs[0]<=mins[0])return(0);
	if(brush->maxs[1]<=mins[1])return(0);
	if(brush->maxs[2]<=mins[2])return(0);
	if(brush->mins[0]>=maxs[0])return(0);
	if(brush->mins[1]>=maxs[1])return(0);
	if(brush->mins[2]>=maxs[2])return(0);

	for(i=0; i<brush->n_face; i++)
	{
		PlaneF_BoxPlaneExtents(mins, maxs, brush->norm+i*4, &f, &g);
		if(f>=0)break;
	}

	if(i<brush->n_face)return(0);

	return(1);
}

int LBXGL_BrushSolid_CheckBBoxBrushList(LBXGL_Brush *lst,
	float *mins, float *maxs)
{
	LBXGL_Brush *cur;
	int i;

	if(!lst)
	{
		mins[0]=0; mins[1]=0; mins[2]=0;
		maxs[0]=0; maxs[1]=0; maxs[2]=0;
		return(0);
	}

	mins[0]=999999;		mins[1]=999999;		mins[2]=999999;
	maxs[0]=-999999;	maxs[1]=-999999;	maxs[2]=-999999;

	cur=lst;
	while(cur)
	{
		i=LBXGL_BrushSolid_CheckBBoxBrush(cur, mins, maxs);
		if(i)return(1);
		cur=cur->chain;
	}

	return(0);
}

LBXGL_Brush *LBXGL_BrushBSP_QueryBoxNodeR(LBXGL_BrushNode *node,
	LBXGL_Brush *lst, float *mins, float *maxs)
{
	float dir[4];
	LBXGL_Brush *cur;
	float f, g;
	int i;

	if(!node)return(lst);

	cur=node->mid;
	while(cur)
	{
		i=LBXGL_BrushSolid_CheckBBoxBrush(cur, mins, maxs);
		if(!i) { cur=cur->nnext; continue; }

		cur->chain=lst; lst=cur;
		cur=cur->nnext;
	}

	if(!node->lnode)
		return(lst);

	PlaneF_BoxPlaneExtents(mins, maxs, node->norm, &f, &g);

	if(g<=0)
	{
		lst=LBXGL_BrushBSP_QueryBoxNodeR(node->lnode, lst, mins, maxs);
		return(lst);
	}

	if(f>=0)
	{
		lst=LBXGL_BrushBSP_QueryBoxNodeR(node->rnode, lst, mins, maxs);
		return(lst);
	}

	lst=LBXGL_BrushBSP_QueryBoxNodeR(node->lnode, lst, mins, maxs);
	lst=LBXGL_BrushBSP_QueryBoxNodeR(node->rnode, lst, mins, maxs);
	return(lst);
}

LBXGL_Brush *LBXGL_BrushBSP_QueryBoxNode(LBXGL_BrushNode *node,
	float *mins, float *maxs)
{
	LBXGL_Brush *lst;
	lst=LBXGL_BrushBSP_QueryBoxNodeR(node, NULL, mins, maxs);
	return(lst);
}


int LBXGL_BrushSolid_BuildBoxMapBits(LBXGL_BrushNode *root,
	byte *bits, int xs, int ys, int zs, float *org, float *box)
{
	LBXGL_Brush *lst;
	float tv0[3], tv1[3];
	int i, j, k, l;

	i=xs*ys*zs;
	memset(bits, 0, (i+7)/8);

	for(i=0; i<xs; i++)
		for(j=0; j<ys; j++)
			for(k=0; k<zs; k++)
	{
		tv0[0]=org[0]+i*box[0];
		tv0[1]=org[1]+j*box[1];
		tv0[2]=org[2]+k*box[2];
		tv1[0]=org[0]+box[0];
		tv1[1]=org[1]+box[1];
		tv1[2]=org[2]+box[2];

		lst=LBXGL_BrushBSP_QueryBoxNode(root, tv0, tv1);

		if(lst)
		{
			l=(k*ys+j)*xs+i;
			bits[l>>3]|=1<<(l&7);
		}
	}

	return(0);
}

