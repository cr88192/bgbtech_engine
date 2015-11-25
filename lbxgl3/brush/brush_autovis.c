#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

int LBXGL_BrushVis_CheckNodeMarkVisible(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int i;

	i=node->id;
	i=world->autovis[i>>3]&(1<<(i&7));
	if(!i)return(0);

	return(1);
}

int LBXGL_BrushVis_CheckNodeMarkVisible2(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int i;

	i=node->id;
	i=world->autovis_shadow[i>>3]&(1<<(i&7));
	if(!i)return(0);

	return(1);
}

void LBXGL_BrushVis_MarkNodeInvisible(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int i;
	i=node->id;
	world->autovis[i>>3]&=~(1<<(i&7));
}

void LBXGL_BrushVis_MarkNodeInvisible2(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int i;
	i=node->id;
	world->autovis_shadow[i>>3]&=~(1<<(i&7));
}

void LBXGL_BrushVis_OccludeNodeBrushesMidBrush(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_Brush *brush, float *vorg)
{
	LBXGL_Brush *cur;
	int i;

	if(!LBXGL_BrushVis_CheckNodeMarkVisible(world, node))
		return;

	i=LBXGL_Brush_CheckBBoxBrushVolume(brush,
		node->mins, node->maxs, vorg);
	if(i==1)
		{ LBXGL_BrushVis_MarkNodeInvisible(world, node); return; }

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_ALPHA))
			{ cur=cur->nnext; continue; }

		i=LBXGL_Brush_CheckBBoxBrushVolume(brush,
			cur->mins, cur->maxs, vorg);
		if(i!=1)break;

//		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->nnext;
	}

	if(!cur)
		{ LBXGL_BrushVis_MarkNodeInvisible(world, node); return; }
}

void LBXGL_BrushWorld_OccludeNodeBrushesBrush(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_Brush *brush, float *vorg)
{
	float f;

	if(!node)return;

	if(!LBXGL_BrushVis_CheckNodeMarkVisible2(world, node))
		return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_OccludeNodeBrushesBrush(
			world, node->lnode, brush, vorg);
		LBXGL_BrushVis_OccludeNodeBrushesMidBrush(
			world, node, brush, vorg);
		LBXGL_BrushWorld_OccludeNodeBrushesBrush(
			world, node->rnode, brush, vorg);
	}else
	{
		LBXGL_BrushWorld_OccludeNodeBrushesBrush(
			world, node->rnode, brush, vorg);
		LBXGL_BrushVis_OccludeNodeBrushesMidBrush(
			world, node, brush, vorg);
		LBXGL_BrushWorld_OccludeNodeBrushesBrush(
			world, node->lnode, brush, vorg);
	}
}

void LBXGL_BrushVis_OccludeNodeBrushesMid(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node, float *vorg)
{
	LBXGL_Brush *cur;
	int i;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		{ LBXGL_BrushVis_MarkNodeInvisible(world, node); return; }

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_ALPHA))
			{ cur=cur->nnext; continue; }

		LBXGL_BrushWorld_OccludeNodeBrushesBrush(
			world, world->bsp, cur, vorg);

//		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_OccludeNodeBrushes(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node, float *vorg)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_OccludeNodeBrushes(
			world, node->lnode, vorg);
		LBXGL_BrushVis_OccludeNodeBrushesMid(
			world, node, vorg);
		LBXGL_BrushWorld_OccludeNodeBrushes(
			world, node->rnode, vorg);
	}else
	{
		LBXGL_BrushWorld_OccludeNodeBrushes(
			world, node->rnode, vorg);
		LBXGL_BrushVis_OccludeNodeBrushesMid(
			world, node, vorg);
		LBXGL_BrushWorld_OccludeNodeBrushes(
			world, node->lnode, vorg);
	}
	
	LBXGL_BrushVis_MarkNodeInvisible2(world, node);
}

#if 0
// void LBXGL_BrushWorld_DrawTestShadowvis(LBXGL_BrushWorld *world)
{
	static byte *stbuf=NULL;
	float tv0[4], tv1[4];
	LBXGL_Light *lcur;
	LBXGL_ModelState *mcur;
	int i, j, k, l, xs, ys;
	float f, g, d;

	xs=160; ys=120;

	if(!stbuf)stbuf=malloc(xs*ys*4);

	pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);

	UI_Camera_SetupView3D(0, 0, xs, ys);

	LBXGL_Brush_DrawBSPNodeBlack(world->bsp);
	pdglDepthMask(0);

	lcur=LBXGL_BrushWorld_QueryLightsCam(world); i=world->shadows;
	while(lcur && i)
	{
		if(lcur->flags&LBXGL_LFL_NOSHADOW)
			break;


//		f=0.15;
//		if(lcur->flags&LBXGL_LFL_LINEAR)
//			d=(6.0*lcur->val[0])/f;
//		else	d=sqrt((6.0*lcur->val[0])/f);
		d=0;

		LBXGL_Shadow_LightGammaEffectRadius(lcur, 0.15);

		pdglClear(GL_COLOR_BUFFER_BIT);

//		pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		LBXGL_Brush_DrawBSPShadowNodeColors(world->bsp, lcur->org, d);

		pdglFinish();
		pdglReadPixels(0, 0, xs, ys, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
//		pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for(j=0; j<(xs*ys); j++)
		{
			k=(stbuf[j*3+0]>>2) | ((stbuf[j*3+1]>>2)<<6) |
				((stbuf[j*3+2]>>2)<<12);
			world->autovis_shadow[k>>3]|=1<<(k&7);
		}

		i--; lcur=lcur->next;
	}

	pdglDepthMask(1);

	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
#endif

#if 0
// LBXGL_API void LBXGL_BrushWorld_BBoxVisible(LBXGL_BrushWorld *world,
	float *mins, float *maxs)
{
	LBXGL_Brush *cur;
	float f, g, h, d;
	int i;

	mins[0]=999999;		mins[1]=999999;		mins[2]=999999;
	maxs[0]=-999999;	maxs[1]=-999999;	maxs[2]=-999999;

	cur=world->brush;
	while(cur)
	{
		if(cur->node)
		{
			i=cur->node->id;
			i=world->autovis[i>>3]&(1<<(i&7));
			if(!i) { cur=cur->next; continue; }
		}

		if(!LBXGL_Brush_CheckBrushFrustum(cur))
			{ cur=cur->next; continue; }

		if(cur->mins[0]<mins[0])mins[0]=cur->mins[0];
		if(cur->mins[1]<mins[1])mins[1]=cur->mins[1];
		if(cur->mins[2]<mins[2])mins[2]=cur->mins[2];

		if(cur->maxs[0]>maxs[0])maxs[0]=cur->maxs[0];
		if(cur->maxs[1]>maxs[1])maxs[1]=cur->maxs[1];
		if(cur->maxs[2]>maxs[2])maxs[2]=cur->maxs[2];

		cur=cur->next;
	}
}
#endif

#if 0
LBXGL_API void LBXGL_BrushWorld_DrawTestAutovis(LBXGL_BrushWorld *world)
{
	static byte *stbuf=NULL;
	float tv0[4], tv1[4];
	LBXGL_Light *lcur;
	LBXGL_ModelState *mcur;
	int i, j, k, l;
	float f, g, d;

	if(!stbuf)stbuf=malloc(320*240*4);

	if(!world->lighting)
	{
		memset(world->autovis, 255, 4096);
		memset(world->autovis_shadow, 255, 4096);
		memset(world->mdlvis, 255, 4096);

		return;
	}

	if(0)
	{
		memset(world->autovis, 255, 4096);
		memset(world->autovis_shadow, 255, 4096);
		memset(world->mdlvis, 255, 4096);

		LBXGL_BrushWorld_OccludeNodeBrushes(
			world, world->bsp, lbxgl_cam->org);

		return;
	}

	LBXGL_BrushWorld_DrawBuildFast(world);

	UI_Camera_SetupView3D(0, 0, 160, 120);
	pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
	pdglDepthFunc(GL_LEQUAL);

#if 1
	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	LBXGL_Brush_DrawBSPNodeColors(world->bsp);

	pdglFinish();
	pdglReadPixels(0, 0, 160, 120, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
//	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pdglClear(GL_COLOR_BUFFER_BIT);

	memset(world->autovis, 0, 4096);
	memset(world->autovis_shadow, 0, 4096);
	for(i=0; i<(160*120); i++)
	{
		j=(stbuf[i*3+0]>>2) | ((stbuf[i*3+1]>>2)<<6) |
			((stbuf[i*3+2]>>2)<<12);
		world->autovis[j>>3]|=1<<(j&7);
		world->autovis_shadow[j>>3]|=1<<(j&7);
	}
#endif

//	LBXGL_BrushWorld_BBoxVisible(world,
//		world->autovis_mins, world->autovis_maxs);
	LBXGL_BrushBSP_BBoxVis(world->bsp, world->autovis,
		world->autovis_mins, world->autovis_maxs);

#if 1
	tv0[0]=lbxgl_cam->org[0]-16;
	tv0[1]=lbxgl_cam->org[1]-16;
	tv0[2]=lbxgl_cam->org[2]-16;
	tv1[0]=lbxgl_cam->org[0]+16;
	tv1[1]=lbxgl_cam->org[1]+16;
	tv1[2]=lbxgl_cam->org[2]+16;

	if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
	{
//		printf("PVS Sanity\n");

		//camera PVS is not visible to itself
		//partly disable box checks as a result
		V3F_SET(world->autovis_mins, -9999, -9999, -9999);
		V3F_SET(world->autovis_maxs,  9999,  9999,  9999);
	}
#endif

//	printf("Visible ")

#if 1
//	LBXGL_Brush_DrawBSPNodeBlack(world->bsp);

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(strcmp(mcur->mdltype, "lbxgl_brushmodel_t"))
//		if(!dytypep(mcur->mdl, "lbxgl_brushmodel_t"))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//		if(!LBXGL_Brush_CheckBBoxFrustum(tv0, tv1))
//			{ mcur=mcur->next; i++; continue; }

		if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
			{ mcur=mcur->next; i++; continue; }

//		i=LBXGL_BrushBSP_GetPVSLeafID(world->bsp, mcur->org);
		j=i&63; k=(i>>6)&63; l=(i>>12)&63;
		pdglColor4ub((j<<2)+2, (k<<2)+2, (l<<2)+2, 255);

		LBXGL_Mdl_DrawModelFlat(mcur);
//		LBXGL_Mdl_DrawModelFlatApprox(mcur);

//		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//		PD3D_DrawFlatCube2(
//			tv0[0], tv0[1], tv0[2], tv1[0], tv1[1], tv1[2]);

		mcur=mcur->next; i++;
	}

	pdglFinish();
	pdglReadPixels(0, 0, 160, 120, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
//	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	pdglClear(GL_COLOR_BUFFER_BIT);

	memset(world->mdlvis, 0, 4096);
	for(i=0; i<(160*120); i++)
	{
		j=(stbuf[i*3+0]>>2) | ((stbuf[i*3+1]>>2)<<6) |
			((stbuf[i*3+2]>>2)<<12);
		world->mdlvis[j>>3]|=1<<(j&7);
	}
#endif

#if 1
//	LBXGL_Brush_DrawBSPNodeBlack(world->bsp);

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!strcmp(mcur->mdltype, "lbxgl_brushmodel_t"))
//		if(dytypep(mcur->mdl, "lbxgl_brushmodel_t"))
			{ mcur=mcur->next; i++; continue; }

		//alpha models are special, and are drawn later
		if(LBXGL_Mdl_AlphaModelP(mcur))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//		if(!LBXGL_Brush_CheckBBoxFrustum(tv0, tv1))
//			{ mcur=mcur->next; i++; continue; }
		if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
			{ mcur=mcur->next; i++; continue; }

//		i=LBXGL_BrushBSP_GetPVSLeafID(world->bsp, mcur->org);
		j=i&63; k=(i>>6)&63; l=(i>>12)&63;
		pdglColor4ub((j<<2)+2, (k<<2)+2, (l<<2)+2, 255);

		LBXGL_Mdl_DrawModelFlat(mcur);
//		LBXGL_Mdl_DrawModelFlatApprox(mcur);

//		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//		PD3D_DrawFlatCube2(
//			tv0[0], tv0[1], tv0[2], tv1[0], tv1[1], tv1[2]);

		mcur=mcur->next; i++;
	}

	pdglFinish();
	pdglReadPixels(0, 0, 160, 120, GL_RGB, GL_UNSIGNED_BYTE, stbuf);
	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	memset(world->mdlvis, 0, 4096);
	for(i=0; i<(160*120); i++)
	{
		j=(stbuf[i*3+0]>>2) | ((stbuf[i*3+1]>>2)<<6) |
			((stbuf[i*3+2]>>2)<<12);
		world->mdlvis[j>>3]|=1<<(j&7);
	}
#endif

//	LBXGL_BrushBSP_BBoxVis(world->bsp, world->autovis,
//		world->autovis_mins, world->autovis_maxs);

//	LBXGL_BrushWorld_BBoxVisible(world,
//		world->autovis_mins, world->autovis_maxs);

	LBXGL_BrushWorld_DrawTestSmallLight(world);
	LBXGL_BrushWorld_DrawPreRender(world);

}
#endif


void LBXGL_BrushVis_DrawNodeBrushesMidQuery(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

	if(!node->mid)return;

#ifndef GLES
//	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
//		return;
	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

	pdglBeginQuery(GL_SAMPLES_PASSED, node->id);

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_ALPHA))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->nnext;
	}

	pdglEndQuery(GL_SAMPLES_PASSED);
//	pdglGetQueryObjectuiv(1, GL_QUERY_RESULT, &i);
	
//	if(i>0)
//	{
//		world->autovis[i>>3]|=1<<(i&7);
//		world->autovis_shadow[i>>3]|=1<<(i&7);
//	}
#endif
}

void LBXGL_BrushWorld_DrawNodeBrushesQuery(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesQuery(world, node->lnode);
		LBXGL_BrushVis_DrawNodeBrushesMidQuery(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesQuery(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesQuery(world, node->rnode);
		LBXGL_BrushVis_DrawNodeBrushesMidQuery(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesQuery(world, node->lnode);
	}
}

void LBXGL_BrushVis_FetchNodeBrushesMidQuery(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i, j;

	if(!node->mid)return;

//	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
//		return;
	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

#ifndef GLES
	pdglGetQueryObjectuiv(node->id, GL_QUERY_RESULT, &i);
#else
	i=1;	//force visible for now
#endif
	
	if(i>0)
	{
		j=node->id;
		world->autovis[j>>3]|=1<<(j&7);
		world->autovis_shadow[j>>3]|=1<<(j&7);
	}

#if 1
	if(i>0)
	{
		//if we can see node, cull brushes by frustum
		cur=node->mid;
		while(cur)
		{
			if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_ALPHA))
				{ cur=cur->nnext; continue; }

#if 1
			cur->flags&=~LBXGL_BRFL_VISCLIP;
			if(!LBXGL_Brush_CheckBrushFrustum(cur))
			{
				cur->flags|=LBXGL_BRFL_VISCLIP;
				cur=cur->nnext;
				continue;
			}
#endif

//			LBXGL_Brush_DrawBrushDark(cur);
			cur=cur->nnext;
		}
	}
#endif
}

void LBXGL_BrushWorld_FetchNodeBrushesQuery(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_FetchNodeBrushesQuery(world, node->lnode);
		LBXGL_BrushVis_FetchNodeBrushesMidQuery(world, node);
		LBXGL_BrushWorld_FetchNodeBrushesQuery(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_FetchNodeBrushesQuery(world, node->rnode);
		LBXGL_BrushVis_FetchNodeBrushesMidQuery(world, node);
		LBXGL_BrushWorld_FetchNodeBrushesQuery(world, node->lnode);
	}
}

int LBXGL_BrushVis_AlphaBrushP(
	LBXGL_BrushWorld *world, LBXGL_Brush *cur)
{
	int i;

	if(cur->flags&(LBXGL_BRFL_FLUID|LBXGL_BRFL_ALPHA))
		return(1);

	i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
	if((i==LBXGL_TXFL_EFFECT_DECAL) ||
		(i==LBXGL_TXFL_EFFECT_DECAL2) ||
		(i==LBXGL_TXFL_EFFECT_DECAL_ALPHA))
			return(1);

	if(i==LBXGL_TXFL_EFFECT_FOG)
		return(1);

	return(0);
}

void LBXGL_BrushVis_DrawNodeBrushesMidQueryAlpha(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i, n;

	if(!node->mid)return;

#ifndef GLES

//	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
//		return;
	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

	cur=node->mid; n=0;
	while(cur)
	{
//		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_ALPHA))
//			{ cur=cur->nnext; continue; }
//		if(!(cur->flags&(LBXGL_BRFL_FLUID|LBXGL_BRFL_ALPHA)))
		if(!LBXGL_BrushVis_AlphaBrushP(world, cur))
			{ cur=cur->nnext; continue; }

		n++;
		pdglBeginQuery(GL_SAMPLES_PASSED, n);
//		LBXGL_Brush_DrawBrushDark(cur);
//		LBXGL_Brush_DrawFluidFlat(cur);
		LBXGL_Brush_DrawFlat(cur);
		pdglEndQuery(GL_SAMPLES_PASSED);
		cur=cur->nnext;
	}

	if(!n)return;

	cur=node->mid; n=0;
	while(cur)
	{
//		if(!(cur->flags&(LBXGL_BRFL_FLUID|LBXGL_BRFL_ALPHA)))
		if(!LBXGL_BrushVis_AlphaBrushP(world, cur))
			{ cur=cur->nnext; continue; }

		n++;
		pdglGetQueryObjectuiv(n, GL_QUERY_RESULT, &i);

		cur->flags&=~LBXGL_BRFL_VISCLIP;
		if(i<=0)cur->flags|=LBXGL_BRFL_VISCLIP;

		cur=cur->nnext;
	}

//	pdglGetQueryObjectuiv(node->id, GL_QUERY_RESULT, &i);
	
//	if(i>0)
//	{
//		world->autovis[i>>3]|=1<<(i&7);
//		world->autovis_shadow[i>>3]|=1<<(i&7);
//	}
#endif
}

void LBXGL_BrushWorld_DrawNodeBrushesQueryAlpha(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesQueryAlpha(world, node->lnode);
		LBXGL_BrushVis_DrawNodeBrushesMidQueryAlpha(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesQueryAlpha(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesQueryAlpha(world, node->rnode);
		LBXGL_BrushVis_DrawNodeBrushesMidQueryAlpha(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesQueryAlpha(world, node->lnode);
	}
}


#if 1
LBXGL_API void LBXGL_BrushWorld_DrawTestAutovis(LBXGL_BrushWorld *world)
{
//	static byte *stbuf=NULL;
	float tv0[4], tv1[4], tv2[4], tv3[4];
	LBXGL_Light *lcur;
	LBXGL_ModelState *mcur;
	int i, j, k, l;
	float f, g, d, drawdist;

	f=btCvarGetf("r_drawdist");
	if(f<=0.0)f=4096;
	drawdist=f;
	
//	if(!stbuf)stbuf=malloc(320*240*4);

	if(!world->lighting || (world->lighting==3) ||
		btCvarGeti("r_noautovis"))
	{
		memset(world->autovis, 255, 4096);
		memset(world->autovis_shadow, 255, 4096);
		memset(world->mdlvis, 255, 4096);
		LBXGL_Voxel_FakeWorldQuery(world);

		if(world->lighting==3)
		{
			LBXGL_SkyCoulds_SetEnabled(0);
		}

		pdglDepthFunc(GL_LEQUAL);
		pdglDepthMask(GL_TRUE);
		return;
	}

	if(lbxgl_state->time_f<world->time_autovis)
		return;
	world->time_autovis=lbxgl_state->time_f+0.16;

	LBXGL_BrushWorld_DrawBuildFast(world);

	LBXGL_BrushWorld_DrawPreRender(world);

//	UI_Camera_SetupView3D(0, 0, 160, 120);
//	UI_Camera_SetupView3D(0, 0, 320, 240);
	UI_Camera_SetupView3D(0, 0, 400, 300);
	pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
	pdglDepthFunc(GL_LEQUAL);
	pdglDepthMask(GL_TRUE);

//	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
		GL_STENCIL_BUFFER_BIT);
	pdglColorMask(0,0,0,0);

#ifndef GLES
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglDisableClientState(GL_NORMAL_ARRAY);
	pdglDisableClientState(GL_COLOR_ARRAY);
#endif

	memset(world->autovis, 0, 4096);
	memset(world->autovis_shadow, 0, 4096);
	LBXGL_BrushWorld_DrawNodeBrushesQuery(world, world->bsp);
	LBXGL_BrushWorld_FetchNodeBrushesQuery(world, world->bsp);

	LBXGL_Voxel_DrawWorldQuery(world);

//	pdglFinish();

//	LBXGL_BrushWorld_BBoxVisible(world,
//		world->autovis_mins, world->autovis_maxs);
	LBXGL_BrushBSP_BBoxVis(world->bsp, world->autovis,
		world->autovis_mins, world->autovis_maxs);

#if 1
	tv0[0]=lbxgl_cam->org[0]-16;
	tv0[1]=lbxgl_cam->org[1]-16;
	tv0[2]=lbxgl_cam->org[2]-16;
	tv1[0]=lbxgl_cam->org[0]+16;
	tv1[1]=lbxgl_cam->org[1]+16;
	tv1[2]=lbxgl_cam->org[2]+16;

	if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
	{
//		printf("PVS Sanity\n");

		//camera PVS is not visible to itself
		//partly disable box checks as a result
//		V3F_SET(world->autovis_mins, -9999, -9999, -9999);
//		V3F_SET(world->autovis_maxs,  9999,  9999,  9999);

		V3F_SET(tv2, -9999, -9999, -9999);
		V3F_SET(tv3,  9999,  9999,  9999);
		V3F_ADD(lbxgl_cam->org, tv2, world->autovis_mins);
		V3F_ADD(lbxgl_cam->org, tv3, world->autovis_maxs);
	}
#endif

//	printf("Visible ")

#if 1
//	LBXGL_Brush_DrawBSPNodeBlack(world->bsp);

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(strcmp(mcur->mdltype, "lbxgl_brushmodel_t"))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//		if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
//			{ mcur=mcur->next; i++; continue; }
		if(!LBXGL_Brush_CheckBBoxFrustumDistanceVisible(
				world, tv0, tv1, 4096))
			{ mcur=mcur->next; i++; continue; }

#ifndef GLES
		pdglBeginQuery(GL_SAMPLES_PASSED, i);
		LBXGL_Mdl_DrawModelFlat(mcur);
		pdglEndQuery(GL_SAMPLES_PASSED);
#endif
		mcur=mcur->next; i++;
	}
#endif


#if 1
	pdglDepthMask(GL_FALSE);
	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!strcmp(mcur->mdltype, "lbxgl_brushmodel_t"))
			{ mcur=mcur->next; i++; continue; }

		//alpha models are special, and are drawn later
		if(LBXGL_Mdl_AlphaModelP(mcur))
			{ mcur=mcur->next; i++; continue; }

		//viewmodels are always visible
		if(LBXGL_Mdl_ViewModelP(mcur))
		{
//			world->mdlvis[i>>3]|=1<<(i&7);
			mcur=mcur->next; i++;
			continue;
		}

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//		if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
//			{ mcur=mcur->next; i++; continue; }
//		if(!LBXGL_Brush_CheckBBoxFrustumDistanceVisible(
//				world, tv0, tv1, 4096))
		if(!LBXGL_Brush_CheckBBoxFrustumDistanceVisible(
				world, tv0, tv1, drawdist))
			{ mcur=mcur->next; i++; continue; }

#ifndef GLES
		pdglBeginQuery(GL_SAMPLES_PASSED, i);
//		LBXGL_Mdl_DrawModelFlat(mcur);
		LBXGL_Mdl_DrawModelFlatApprox(mcur);
		pdglEndQuery(GL_SAMPLES_PASSED);
#endif

		mcur=mcur->next; i++;
	}
	pdglDepthMask(GL_TRUE);
#endif

#if 1
	memset(world->mdlvis, 0, 4096);

	mcur=world->mesh; i=1;
	while(mcur && (i<32768))
	{
		//alpha models are special, and are drawn later
		if(LBXGL_Mdl_AlphaModelP(mcur))
			{ mcur=mcur->next; i++; continue; }

		//viewmodels are always visible
		if(LBXGL_Mdl_ViewModelP(mcur))
		{
			world->mdlvis[i>>3]|=1<<(i&7);
			mcur=mcur->next; i++;
			continue;
		}

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//		if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
//			{ mcur=mcur->next; i++; continue; }
//		if(!LBXGL_Brush_CheckBBoxFrustumDistanceVisible(
//				world, tv0, tv1, 4096))
		if(!LBXGL_Brush_CheckBBoxFrustumDistanceVisible(
				world, tv0, tv1, drawdist))
			{ mcur=mcur->next; i++; continue; }

#ifndef GLES
		pdglGetQueryObjectuiv(i, GL_QUERY_RESULT, &j);
#else
		j=1;
#endif
	
		if(j>0)
			{ world->mdlvis[i>>3]|=1<<(i&7); }

		mcur=mcur->next; i++;
	}
#endif

//	LBXGL_BrushWorld_DrawNodeBrushesQueryAlpha(world, world->bsp);

	pdglColorMask(1,1,1,1);
	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	LBXGL_BrushWorld_DrawTestSmallLight(world);
//	LBXGL_BrushWorld_DrawPreRender(world);

}
#endif
