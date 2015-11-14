#include <lbxgl2.h>

//get a list of panels sorted as to how much each is in the
//direction of the target point from the current panel
int LBXGL_BrushPath_ClassifyPanelsForPoint(
	LBXGL_GroundPanel *panel,
	LBXGL_GroundPanel **lst, int mlst,
	float *point)
{
	LBXGL_GroundPanel *tlst[1024];
	float tweight[1024];
	float org[3], dv[3];
	float torg[3], tv[3];
	LBXGL_GroundPanel *ptmp;
	float f, g;
	int i, j, k, n;

	LBXGL_BrushGround_GetPanelOrigin(panel, org);
	V3F_SUB(point, org, dv);
	Vec3F_Normalize(dv, dv);

	n=panel->n_link;
	for(i=0; i<n; i++)
	{
		tlst[i]=panel->link[i];
		LBXGL_BrushGround_GetPanelOrigin(tlst[i], torg);
		V3F_SUB(torg, org, tv);
		Vec3F_Normalize(tv, tv);
		tweight[i]=V3F_DOT(tv, dv);
	}

	for(i=0; i<n; i++)
		for(j=i+1; j<n; j++)
			if(tweight[j]>tweight[i])
	{
		ptmp=tlst[i]; tlst[i]=tlst[j]; tlst[j]=ptmp;
		f=tweight[i]; tweight[i]=tweight[j]; tweight[j]=f;
	}

	if(n>mlst)n=mlst;
	for(i=0; i<n; i++)
		lst[i]=tlst[i];
	return(n);
}

//try to find a path from the source to the dest
//returns: -1 on failure, 0 if same panel, or >0 for a panel list
int LBXGL_BrushPath_FindPathPanelList(
	LBXGL_BrushWorld *world, LBXGL_GroundPanel **lst,
	float *src, float *dst)
{
	LBXGL_GroundPanel *tlst[1024];
	LBXGL_GroundPanel *tblst[1024];

	LBXGL_BrushPath_FindPathPanelList_State tctx;
	LBXGL_BrushPath_FindPathPanelList_State *ctx;
	LBXGL_GroundPanel *spanel, *dpanel;

	int i, n;

	spanel=LBXGL_BrushGround_GetPanelForPoint(world, src, NULL);
	dpanel=LBXGL_BrushGround_FindGoodPanelForPoint(world, dst);

	if(!spanel || !dpanel)
		return(-1);
	if(spanel==dpanel)
		return(0);

	ctx=&tctx;
	memset(ctx, 0, sizeof(LBXGL_BrushPath_FindPathPanelList_State));

	ctx->world=world;
	ctx->blst=tblst;
	ctx->lst=tlst;
	ctx->blpos=-1;
	ctx->lpos=0;

	ctx->sorg=src;
	ctx->dorg=dst;

	ctx->spanel=spanel;
	ctx->dpanel=dpanel;

	LBXGL_BrushPath_FindPathPanelList_R(ctx, spanel);

	n=ctx->blpos;
	for(i=0; i<n; i++)
		lst[i]=tblst[i];
	return(n);
}

int LBXGL_BrushPath_FindPathPanelList_R(
	LBXGL_BrushPath_FindPathPanelList_State *ctx,
	LBXGL_GroundPanel *panel)
{
	LBXGL_GroundPanel *tlst[64];
	int i, j, k, n;

	//skip longer paths
	if((ctx->blpos>0) && (ctx->lpos+1)>=ctx->blpos)
		return(-1);

	//avoid returning to prior panels
	for(i=ctx->lpos-1; i>=0; i--)
		if(ctx->lst[i]==panel)
			return(-1);

	if(panel==ctx->dpanel)
	{
		//hit target panel
		ctx->lst[ctx->lpos++]=panel;
		if((ctx->blpos<0) || (ctx->lpos<ctx->blpos))
		{
			//found better path, so use this one
			for(i=0; i<ctx->lpos; i++)
				ctx->blst[i]=ctx->lst[i];
			ctx->blpos=ctx->lpos;
		}
		ctx->lpos--;
		return(ctx->lpos);
	}

	//avoid overflow
	if(ctx->lpos>=1020)
		return(-1);

	//check linked-to panels
	ctx->lst[ctx->lpos++]=panel;
	n=LBXGL_BrushPath_ClassifyPanelsForPoint(panel, tlst, 64, ctx->dorg);
	for(i=0; i<n; i++)
	{
		LBXGL_BrushPath_FindPathPanelList_R(ctx, tlst[i]);
	}

	ctx->lpos--;
	return(ctx->lpos);
}
