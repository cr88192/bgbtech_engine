#include <lbxgl2.h>

LBXGL_Entity *BSP_AddSolid(LBXGL_World *ctx, int id)
{
	LBXGL_Entity *tmp;
	LBXGL_Entity *cur;
	int i;

	tmp=LBXGL_Entity_New();

//	tmp->base->solid=id;
	LBXGL_Entity_SetPropertyFloat(tmp, "solid", id);

	tmp->next=ctx->ents;
	ctx->ents=tmp;

	cur=tmp;
	for(i=0; cur; i++)cur=cur->next;

	return(tmp);
}

void BSP_UnlinkEntsNode(BSP_Node *node)
{
	node->ents=NULL;
	if(node->lnode)BSP_UnlinkEntsNode(node->lnode);
	if(node->rnode)BSP_UnlinkEntsNode(node->rnode);
}

void BSP_LinkEntNode(BSP_Node *node, LBXGL_Entity *ent)
{
	float org[3];
	double d;

	if(!node->lnode)
	{
		ent->lnext=node->ents;
		node->ents=ent;
		return;
	}

	LBXGL_Entity_GetPropertyFVector(ent, "origin", org, 3);
	d=Vec3F_DotProduct(org, node->norm)-node->norm[3];

	if(d<0)
	{
		BSP_LinkEntNode(node->lnode, ent);
	}else
	{
		BSP_LinkEntNode(node->rnode, ent);
	}
}

float BSP_GammaAtPoint(LBXGL_World *ctx, float *pt)
{
	LBXGL_Light *cur;
	float f, g, lv, d;
	int i;

	return(1);

	g=0;
	cur=ctx->lights;
	while(cur)
	{
		lv=cur->val[0];
		d=V3F_DIST(cur->org, pt);
		f=(6.0*cur->val[0])/(d*d+1);
		if(f<4)
		{
			cur=cur->next;
			continue;
		}

		i=BSP_TraceNode(ctx->bsp, cur->org, pt);
		if(i)
		{
			cur=cur->next;
			continue;
		}

		g+=f/255.0;
		cur=cur->next;
	}

	if(g>1)g=1;
	return(g);
}

void BSP_AddLightPoint(LBXGL_World *ctx, LBXGL_Light *cur,
	float *pt, float *gv)
{
	float f, g, lv, d;
	int i;

	lv=cur->val[0];
	d=V3F_DIST(cur->org, pt)/36.0;
	f=(6.0*cur->val[0])/(d*d+1);
	if(f<4)return;

	i=BSP_TraceNode(ctx->bsp, cur->org, pt);
	if(i)return;

	gv[0]+=cur->clr[0]*(f/255.0);
	gv[1]+=cur->clr[1]*(f/255.0);
	gv[2]+=cur->clr[2]*(f/255.0);
}

void BSP_GammaAtPoint2(LBXGL_World *ctx, float *pt, float *gv)
{
	LBXGL_Light *cur;

	V3F_ZERO(gv);

	cur=ctx->lights;
	while(cur)
	{
		BSP_AddLightPoint(ctx, cur, pt, gv);
		cur=cur->next;
	}

	cur=ctx->dlights;
	while(cur)
	{
		BSP_AddLightPoint(ctx, cur, pt, gv);
		cur=cur->next;
	}
}

void BSP_RelinkEnts(LBXGL_World *ctx)
{
	LBXGL_Entity *cur;
	float org[3], rgb[4];
	float d, f, lv, g;
	int i, j;

	BSP_UnlinkEntsNode(ctx->bsp);

	cur=ctx->ents;
	while(cur)
	{
//		bsdeGetAttrFV(cur->base->solid, BSDE_ORIGIN,
//			cur->base->origin, 3);
//		bsdeGetAttrFV(cur->base->solid, BSDE_ROTATION_MATRIX, 
//			cur->base->rot, 9);

//		cur->base->gamma=BSP_GammaAtPoint(ctx, cur->base->origin);

		LBXGL_Entity_GetPropertyFVector(cur, "origin", org, 3);
		BSP_GammaAtPoint2(ctx, org, rgb);
		LBXGL_Entity_SetPropertyFVector(cur, "gamma", rgb, 3);
		BSP_LinkEntNode(ctx->bsp, cur);
		cur=cur->next;
	}

}


void BSP_DrawEnts(LBXGL_World *wrl, LBXGL_Entity *first)
{
	LBXGL_Entity *cur;

	cur=first;
	while(cur)
	{
		LBXGL_Entity_Render(wrl, cur);
		cur=cur->lnext;
	}
}


int BSP_PointLeafNode(BSP_Node *node, float *org)
{
	double d;
	int i;

	if(!node->lnode)
		return(node->num);

	d=Vec3F_DotProduct(org, node->norm)-node->norm[3];

	if(d<0)i=BSP_PointLeafNode(node->lnode, org);
		else i=BSP_PointLeafNode(node->rnode, org);
	return(i);
}
