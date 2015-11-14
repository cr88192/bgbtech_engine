#include <lbxgl2.h>
#include <pdglui.h>

//#include <GL/GL.h>
//#include <GL_2/GLext.h>


extern int lbxgl_draw_light_xs;
extern int lbxgl_draw_light_ys;
extern float lbxgl_draw_light_delay;

extern float lbxgl_draw_gamma;

extern int lbxgl_draw_light_cxs;
extern int lbxgl_draw_light_cys;

extern int shader_nophong;

extern int shader_phong;
extern int shader_phong_spot;
extern int shader_phong2;

extern int tex_screen;
extern int tex_lighting;

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
LBXGL_BrushWorld *lbxgl_world;

LBXGL_BrushWorld *lbxgl_worldstack[64];
int lbxgl_worldstack_pos;


void LBXGL_BrushWorld_PushWorld(LBXGL_BrushWorld *world)
{
	int i;
	
	i=lbxgl_worldstack_pos++;
	lbxgl_worldstack[i]=world;

	lbxgl_world=world;
}

void LBXGL_BrushWorld_PopWorld(void)
{
	int i;
	
	i=--lbxgl_worldstack_pos;
	lbxgl_world=lbxgl_worldstack[i];

//	lbxgl_world=NULL;
}

int LBXGL_BrushWorld_CheckNodeFrustum(LBXGL_BrushNode *node)
{
	if(!node->mid)
		return(0);

	if(!LBXGL_Brush_CheckBBoxFrustum(node->mins, node->maxs))
		return(0);
		
	return(1);
}

int LBXGL_BrushWorld_CheckNodeFrustumVisible(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int i;
	
	if(!node->mid)
		return(0);

	i=node->id;
	i=world->autovis[i>>3]&(1<<(i&7));
	if(!i)return(0);

	if(!LBXGL_Brush_CheckBBoxFrustum(node->mins, node->maxs))
		return(0);

	if(!LBXGL_Brush_CheckBBoxAntiportal(
		world, node->mins, node->maxs, lbxgl_cam->org))
			return(0);

	return(1);
}

void LBXGL_BrushWorld_DrawNodeBrushesMidDark(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

//	i=node->id;
//	i=world->autovis[i>>3]&(1<<(i&7));
//	if(!i)return;

//	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
//		return;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return;

	cur=node->mid;
	while(cur)
	{
//		if(cur->flags&LBXGL_BRFL_EFFECT)
//			{ cur=cur->nnext; continue; }

		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_PTLIGHT))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesDark(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesDark(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidDark(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesDark(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesDark(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidDark(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesDark(world, node->lnode);
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesMidFinal(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

//	i=node->id;
//	i=world->autovis[i>>3]&(1<<(i&7));
//	if(!i)return;

//	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
//		return;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_EFFECT)
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawFacesShadow(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesFinal(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidFinal(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidFinal(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->lnode);
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesMidFlat(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

//	i=node->id; i=world->autovis[i>>3]&(1<<(i&7)); if(!i)return;

//	if(!cur->mid)return;
//	if(!LBXGL_Brush_CheckBBoxFrustum(node->mins, node->maxs))
//		return;

//	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
//		return;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return;

	cur=node->mid;
	while(cur) { LBXGL_Brush_DrawSolidFlat(cur); cur=cur->nnext; }
}

void LBXGL_BrushWorld_DrawNodeBrushesFlat(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesFlat(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidFlat(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesFlat(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesFlat(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidFlat(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesFlat(world, node->lnode);
	}
}

void LBXGL_BrushWorld_DrawBrushesFluidSurface(LBXGL_BrushWorld *world,
	LBXGL_Brush *lst)
{
	float pt[4];
	LBXGL_Brush *cur, *brsh;
	int i, ty;

	glColorMask(0, 0, 0, 0);
	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluidFlat(cur);
		cur=cur->chain;
	}
	glColorMask(1, 1, 1, 1);

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluid(cur);
		cur=cur->chain;
	}

	if(world->shadows<0)
		return;

	LBXGL_Shader_FlushState();

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluid2(cur);
		cur=cur->chain;
	}
}

void LBXGL_BrushWorld_DrawBrushesFluidSubmergedBasic(
	LBXGL_BrushWorld *world, LBXGL_Brush *lst, LBXGL_Brush *brsh)
{
	float pt[4];
	LBXGL_Brush *cur;
	int i, ty;

#if 0
		glDisable(GL_TEXTURE_2D);

		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);

		glDepthMask(0);
		glColorMask(0, 0, 0, 0);

		glDepthFunc(GL_GEQUAL);

		glEnable(GL_CULL_FACE);
		glStencilFunc(GL_ALWAYS, 0, 255);

		glCullFace(GL_FRONT);
//		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluidFlat(cur);
			cur=cur->chain;
		}

		glCullFace(GL_BACK);
//		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluidFlat(cur);
			cur=cur->chain;
		}

		glColorMask(1, 1, 1, 1);
		glDepthMask(1);

		glDepthFunc(GL_LEQUAL);

		glCullFace(GL_BACK);
//		glStencilFunc(GL_EQUAL, 0, 255);
		glStencilFunc(GL_NOTEQUAL, 0, 255);

#if 0
		glFogi(GL_FOG_MODE, GL_LINEAR);
//		glFogi(GL_FOG_MODE, GL_EXP);
		glFogi(GL_FOG_COORD_SRC, GL_FRAGMENT_DEPTH);
//		glFogf(GL_FOG_DENSITY, 1.0);
		glFogf(GL_FOG_DENSITY, 0.1);
		glFogf(GL_FOG_START, 16.0);
		glFogf(GL_FOG_END, 256.0);

		pt[0]=0.0; pt[1]=0.2; pt[2]=0.4; pt[3]=1;
		glFogfv(GL_FOG_COLOR, pt);

		glEnable(GL_FOG);

		glBlendFunc(GL_ONE_MINUS_SRC_COLOR, GL_SRC_COLOR);
		glColor4f(1, 1, 1, 0.75);
		LBXGL_BrushWorld_DrawNodeBrushesFlat(world, world->bsp);

		glDisable(GL_FOG);
#endif

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		ty=0; if(brsh)ty=brsh->flags&LBXGL_TXFL_FLUID_MASK;

		pt[0]=0.02; pt[1]=0.06; pt[2]=0.10; pt[3]=1.0/384;

		if(ty==LBXGL_TXFL_FLUID_SLIME)
			{ pt[0]=0.02; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/256; }
		if(ty==LBXGL_TXFL_FLUID_SEWER)
			{ pt[0]=0.10; pt[1]=0.08; pt[2]=0.04; pt[3]=1.0/256; }
		if(ty==LBXGL_TXFL_FLUID_LAVA)
			{ pt[0]=0.5; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/64; }

		LBXGL_Brush_DrawLayerFog(pt, 16, 12, 64);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//		LBXGL_BrushWorld_DrawNodeBrushesDark(world, world->bsp);
//		LBXGL_BrushWorld_DrawModelsDark(world);

//		glDisable(GL_STENCIL_TEST);

#if 0

#if 0
		glDisable(GL_CULL_FACE);
		glColorMask(0, 0, 0, 0);
		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluidFlat(cur);
			cur=cur->chain;
		}
		glColorMask(1, 1, 1, 1);

#endif

		glDepthMask(0);

		glDisable(GL_CULL_FACE);
//		glStencilFunc(GL_NOTEQUAL, 0, 255);
		glStencilFunc(GL_EQUAL, 0, 255);

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluid(cur);
			cur=cur->chain;
		}

		LBXGL_Shader_FlushState();

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluid2(cur);
			cur=cur->chain;
		}

		glDepthMask(1);
#endif

		glDisable(GL_STENCIL_TEST);

#endif

		LBXGL_Brush_DrawFluidSurfaces(lst);

#if 1
		ty=0; if(brsh)ty=brsh->flags&LBXGL_TXFL_FLUID_MASK;

		pt[0]=0.02; pt[1]=0.06; pt[2]=0.10; pt[3]=1.0/384;

		if(ty==LBXGL_TXFL_FLUID_SLIME)
			{ pt[0]=0.02; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/256; }
		if(ty==LBXGL_TXFL_FLUID_SEWER)
			{ pt[0]=0.10; pt[1]=0.08; pt[2]=0.04; pt[3]=1.0/256; }
		if(ty==LBXGL_TXFL_FLUID_LAVA)
			{ pt[0]=0.5; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/64; }

		pt[3]*=100;
//		LBXGL_Brush_DrawFogVolume(lst, pt);
		LBXGL_BrushWorld_DrawColorOverlay(world, pt);
#endif
}

void LBXGL_BrushWorld_DrawBrushesFluidSubmerged(LBXGL_BrushWorld *world,
	LBXGL_Brush *lst, LBXGL_Brush *brsh)
{
	float pt[4];
	LBXGL_Brush *cur;
	int i, ty;

	if(world->shadows<0)
	{
		LBXGL_BrushWorld_DrawBrushesFluidSubmergedBasic(world, lst, brsh);
		return;
	}

	LBXGL_Brush_DrawFluidSurfaces(lst);

#if 1
	ty=0; if(brsh)ty=brsh->flags&LBXGL_TXFL_FLUID_MASK;
	pt[0]=0.02; pt[1]=0.06; pt[2]=0.10; pt[3]=1.0/384;
	if(ty==LBXGL_TXFL_FLUID_SLIME)
		{ pt[0]=0.02; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/256; }
	if(ty==LBXGL_TXFL_FLUID_SEWER)
		{ pt[0]=0.10; pt[1]=0.08; pt[2]=0.04; pt[3]=1.0/256; }
	if(ty==LBXGL_TXFL_FLUID_LAVA)
		{ pt[0]=0.5; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/64; }
	LBXGL_Brush_DrawFogVolume(lst, pt);
#endif

	LBXGL_Shader_FlushState();
	LBXGL_BrushWorld_DrawShaderOverlay(world,
		"textures/shader/watery_overlay");
}

void LBXGL_BrushWorld_DrawBrushesFluid(LBXGL_BrushWorld *world)
{
	float pt[4];
	LBXGL_Brush *cur, *lst, *brsh;
	int i, ty;

	world->fluidcam=0;

	cur=world->brush; lst=NULL; brsh=NULL;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_FLUID)
		{
//			if(!LBXGL_Brush_CheckBrushFrustum(cur))
//				{ cur=cur->next; continue; }
			if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
				{ cur=cur->next; continue; }

			if(LBXGL_Brush_CheckPointBrush(cur, lbxgl_cam->org))
				{ world->fluidcam=1; brsh=cur; }
			cur->chain=lst; lst=cur;
		}
		cur=cur->next;
	}

	if(!lst)
		return;

	if(!world->fluidcam)
	{
		LBXGL_BrushWorld_DrawBrushesFluidSurface(world, lst);
		return;
	}

	if(world->fluidcam)
	{
		LBXGL_BrushWorld_DrawBrushesFluidSubmerged(world, lst, brsh);
		return;
	}
}

void LBXGL_BrushWorld_DrawBrushesFog(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *fga[16];
	float fva[4*16];

	LBXGL_Brush *cur, *nxt;
	float tv[4];
	float f, g, h, d;
	int i, n;

	n=0;
	cur=world->brush;
	while(cur)
	{
		i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
		if(i==LBXGL_TXFL_EFFECT_FOG)
		{
//			if(LBXGL_Brush_CheckPointBrush(cur, lbxgl_cam->org))
//				{ world->fluidcam=1; brsh=cur; }
//			cur->chain=lst; lst=cur;

			LBXGL_Texture_GetImageVParm(cur->pf_tex[0], 0, tv);

			for(i=0; i<n; i++)
				if(V4F_DIST(tv, fva+i*4)<0.0001)
					break;
			if(i>=n)
			{
				i=n++;
				V4F_COPY(tv, fva+i*4);
				fga[i]=NULL;
			}

			cur->chain=fga[i]; fga[i]=cur;

		}
		cur=cur->next;
	}

	for(i=0; i<n; i++)
	{
//		printf("Fog %f %f %f  %f\n",
//			fva[i*4+0], fva[i*4+1], fva[i*4+2], fva[i*4+3]);
//		LBXGL_Brush_DrawFaces(cur);
		LBXGL_Brush_DrawFogVolume(fga[i], fva+i*4);
	}

#if 0
	cur=lst;
	while(cur)
	{
		LBXGL_Texture_GetImageVParm(cur->pf_tex[0], 0, tv);
		cur=cur->chain;
	}
#endif
}

void LBXGL_BrushWorld_DrawBrushesDecal(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur, *nxt, *lst;
	float tv[4];
	float f, g, h, d;
	int i, n;

	cur=world->brush; lst=NULL;
	while(cur)
	{
		i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
		if(i==LBXGL_TXFL_EFFECT_DECAL)
		{
			if(V3F_DIST(cur->org, lbxgl_cam->org)>(1024+cur->rad))
				{ cur=cur->next; continue; }
			
			cur->chain=lst; lst=cur;

		}
		cur=cur->next;
	}

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFaces(cur);
		cur=cur->chain;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesMid(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

//	i=node->id; i=world->autovis[i>>3]&(1<<(i&7)); if(!i)return;
	cur=node->mid;
	while(cur) { LBXGL_Brush_DrawFaces(cur); cur=cur->nnext; }
}

void LBXGL_BrushWorld_DrawNodeBrushes(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushes(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMid(world, node);
		LBXGL_BrushWorld_DrawNodeBrushes(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushes(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMid(world, node);
		LBXGL_BrushWorld_DrawNodeBrushes(world, node->lnode);
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesMidPointLight(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

	i=node->id; i=world->autovis[i>>3]&(1<<(i&7)); if(!i)return;
	cur=node->mid;
	while(cur)
	{
		if(!(cur->flags&LBXGL_BRFL_PTLIGHT))
			{ cur=cur->nnext; continue; }

		glColor4fv(cur->ptrgb);
		if(cur->rgb)
			{ LBXGL_Brush_DrawSolidFlatRGB(cur); }
		else
			{ LBXGL_Brush_DrawSolidFlat(cur); }
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesPointLight(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidPointLight(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidPointLight(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->lnode);
	}
}


LBXGL_API void LBXGL_BrushWorld_DrawBrushes(LBXGL_BrushWorld *world, LBXGL_Brush *fst)
{
	static short *stbuf=NULL;
	LBXGL_BrushNode *node;
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur, *lfst;
	LBXGL_ModelState *mcur;
	int i, j;

	LBXGL_BrushWorld_PushWorld(world);

	LBXGL_BrushWorld_DrawBuildFast(world);
//	LBXGL_BrushWorld_UpdateModels(world);

	LBXGL_BrushWorld_PopWorld();

#if 0
	if(!world->lighting)
	{
		glCullFace(GL_BACK);

		cur=fst;
		while(cur)
		{
			glEnable(GL_CULL_FACE);
			if(!(cur->flags&LBXGL_BRFL_ALPHA))
				LBXGL_Brush_DrawFaces(cur);
			cur=cur->next;
		}

		LBXGL_Shader_FlushState();

		cur=fst;
		while(cur)
		{
			glEnable(GL_CULL_FACE);
			if(cur->flags&LBXGL_BRFL_ALPHA)
				LBXGL_Brush_DrawFaces(cur);
			cur=cur->next;
		}

		LBXGL_Terrain_DrawTexture(world);

//		if(world->brush_sel)LBXGL_BrushWorld_DrawControlsBrush(world, world->brush_sel);

#if 0
		cur=fst;
		while(cur)
		{
			LBXGL_BrushWorld_DrawControlsBrush(world, cur);
			cur=cur->next;
		}
#endif

		mcur=world->mesh;
		while(mcur)
		{
			LBXGL_Mdl_DrawModel(mcur);
			mcur=mcur->next;
		}

		LBXGL_BrushGround_DrawGroundFaces(world);

		return;
	}
#endif

#if 1
	if(!world->lighting)
	{
		glCullFace(GL_BACK);
		LBXGL_Shader_ForceDraw(1);		//force drawing nodraw faces

#if 0
		cur=fst;
		while(cur)
		{
			glEnable(GL_CULL_FACE);
			if(!(cur->flags&LBXGL_BRFL_ALPHA))
				LBXGL_Brush_DrawFaces(cur);
			cur=cur->next;
		}
#endif

#if 1
		glCullFace(GL_BACK); glEnable(GL_CULL_FACE);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		glColor4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawNodeBrushes(world, world->bsp);
//		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, world->bsp);
		LBXGL_Terrain_DrawFinal(world);

//		LBXGL_BrushWorld_DrawModelsFinal(world);
#endif

#if 0
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		glDisable(GL_CULL_FACE);
		LBXGL_Sky_DrawSky();

		LBXGL_BrushWorld_DrawModelsAlpha(world);

		LBXGL_Shader_FlushState();

		LBXGL_BrushWorld_DrawBrushesFluid(world);
		LBXGL_BrushWorld_DrawBrushesFog(world);
#endif

		LBXGL_Shader_FlushState();

		LBXGL_Shader_Color4f(1, 1, 1, 1);
		glColor4f(1, 1, 1, 1);

		cur=fst;
		while(cur)
		{
			//clear colors
			V4F_SET(cur->ptrgb, 1, 1, 1, 1);
			if(cur->rgb)
				{ gcfree(cur->rgb); cur->rgb=NULL; }

			//draw alpha
			glEnable(GL_CULL_FACE);
			if(cur->flags&LBXGL_BRFL_ALPHA)
				LBXGL_Brush_DrawFaces(cur);
			cur=cur->next;
		}

		LBXGL_Shader_ForceDraw(0);

		LBXGL_Terrain_DrawTexture(world);

//		if(world->brush_sel)LBXGL_BrushWorld_DrawControlsBrush(world, world->brush_sel);

#if 0
		cur=fst;
		while(cur)
		{
			LBXGL_BrushWorld_DrawControlsBrush(world, cur);
			cur=cur->next;
		}
#endif

#if 1
		mcur=world->mesh;
		while(mcur)
		{
			LBXGL_Mdl_DrawModel(mcur);
			mcur=mcur->next;
		}
#endif

		LBXGL_BrushGround_DrawGroundFaces(world);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		return;
	}
#endif

#if 0
	if(!world->bsp)
	{
		cur=fst; lst=NULL;
		while(cur)
		{
			if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);
			cur->chain=lst; lst=cur; cur=cur->next;
		}

//		LBXGL_BrushBSP_FreeTreeNode(world->bsp);
//		world->bsp=LBXGL_BrushBSP_BuildNodeList(lst);
//		world->bsp=LBXGL_BrushBSP_BuildTreeList(lst);
//		world->portal=LBXGL_BrushBSP_BuildPortals(world->bsp);

		world->bsp=LBXGL_BrushBSP_BuildTreeListFast(lst);
	}
#endif

	LBXGL_BrushWorld_PushWorld(world);

//	if(world->light_sun)
	LBXGL_BrushWorld_UpdateSun(world);

//	if(world->lighting==1)
//	if((world->lighting==1) && (world->shadows>0))
	if((world->lighting==1) && (world->shadows>=0))
	{
		if((shader_phong<0) && !shader_nophong)
		{
			shader_phong=PDGL_LoadShader("bgb_pointlight0");
			shader_phong2=PDGL_LoadShader("bgb_pointlight1");

			shader_phong_spot=PDGL_LoadShader("spotlight");

			if(shader_phong<0)
				shader_nophong=1;
		}

#if 1
		cur=fst;
		while(cur)
		{
			cur->flags&=~LBXGL_BRFL_PTLIGHT;
			if(V3F_DIST(cur->org, lbxgl_cam->org)>2048)
			{
				cur->flags|=LBXGL_BRFL_PTLIGHT;
				LBXGL_BrushWorld_InitBrushRGB(world, cur);
			}
			cur=cur->next;
		}
#endif

		glDepthFunc(GL_LEQUAL);

		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);

//		glDepthRange(0.0001, 1.0);
//		glDepthRange(0.0, 0.99);

//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix();
//		glTranslatef(
//			lbxgl_cam->fw[0],
//			lbxgl_cam->fw[1],
//			lbxgl_cam->fw[2]);

		LBXGL_BrushWorld_DrawNodeBrushesDark(world, world->bsp);
		LBXGL_BrushWorld_DrawModelsDark(world);
		LBXGL_Terrain_DrawDark(world);

//		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, world->bsp);

//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPopMatrix();

//		glDepthRange(0, 1);

#if 1
		lfst=LBXGL_BrushWorld_QueryLightsCam(world); i=world->shadows;
		lcur=lfst;
		while(lcur && i)
		{
			if(lcur->flags&LBXGL_LFL_NOSHADOW)
				break;

			glClear(GL_STENCIL_BUFFER_BIT);
			glEnable(GL_STENCIL_TEST);
			lst=LBXGL_BrushWorld_QueryBrushesLight(world, fst, lcur);
			LBXGL_BrushWorld_DrawShadowsLight(world, lst, lcur);
			lst=LBXGL_BrushWorld_QueryBrushesLight2(world, fst, lcur);
			LBXGL_BrushWorld_DrawBrushesLight(world, lst, lcur);
			glDisable(GL_STENCIL_TEST);
			i--; lcur=lcur->chain;
		}

		while(lcur)
		{
			lst=LBXGL_BrushWorld_QueryBrushesLight2(world, fst, lcur);
			LBXGL_BrushWorld_DrawBrushesLight(world, lst, lcur);
			lcur=lcur->chain;
		}

#if 1
		lcur=lfst;
		while(lcur)
		{
			lst=LBXGL_BrushWorld_QueryBrushesLightPT(world, fst, lcur);
			LBXGL_BrushWorld_UpdateBrushesLight(world, lst, lcur);
			lcur=lcur->chain;
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		glColor4f(1, 1, 1, 1);
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, world->bsp);
		glBlendFunc(GL_SRC_COLOR, GL_ONE);
#endif		

#endif
	}

	if(world->lighting==2)
	{
		glCullFace(GL_BACK); glEnable(GL_CULL_FACE);
		LBXGL_BrushWorld_DrawNodeBrushesDark(world, world->bsp);
		LBXGL_BrushWorld_DrawModelsDark(world);
		LBXGL_Terrain_DrawDark(world);

		LBXGL_BrushWorld_DrawLightTexture(world);
	}

	if((world->lighting==1) && (world->shadows<0))
	{
#if 1
		cur=fst;
		while(cur)
		{
			LBXGL_BrushWorld_InitBrushRGB(world, cur);
			cur->flags|=LBXGL_BRFL_PTLIGHT;
			cur=cur->next;
		}

		lcur=LBXGL_BrushWorld_QueryLightsCam(world);
		while(lcur)
		{
			lst=LBXGL_BrushWorld_QueryBrushesLightPT(world, fst, lcur);
			LBXGL_BrushWorld_UpdateBrushesLight(world, lst, lcur);
			lcur=lcur->chain;
		}
#endif

		glCullFace(GL_BACK); glEnable(GL_CULL_FACE);
		glColor4f(1, 1, 1, 1);
		glDisable(GL_TEXTURE_2D);
//		LBXGL_BrushWorld_DrawNodeBrushesFlat(world, world->bsp);
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, world->bsp);

		glColor4f(1, 1, 1, 1);
//		LBXGL_BrushWorld_DrawModelsFlat(world);
		LBXGL_BrushWorld_DrawModelsPointLight(world);
		LBXGL_Terrain_DrawFlat(world);

	}

	LBXGL_BrushWorld_DrawFlashlight(world);

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);

	glDepthMask(GL_FALSE);	//disable drawing to Z buffer

	LBXGL_BrushWorld_DrawNodeBrushesFinal(world, world->bsp);
	LBXGL_Terrain_DrawFinal(world);

	LBXGL_BrushWorld_DrawModelsFinal(world);

#if 1
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);
	LBXGL_BrushWorld_DrawBrushesDecal(world);
#endif

	glDepthMask(GL_TRUE);	//enable drawing to Z buffer

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	glDisable(GL_CULL_FACE);
	LBXGL_Sky_DrawSky();

	LBXGL_BrushWorld_DrawModelsAlpha(world);

	LBXGL_Shader_FlushState();

	LBXGL_BrushWorld_DrawBrushesFluid(world);
	LBXGL_BrushWorld_DrawBrushesFog(world);

	cur=fst;
	while(cur)
	{
//		if(cur->flags&LBXGL_BRFL_FLUID)
//			{ cur=cur->next; continue; }
//		if(cur->flags&LBXGL_BRFL_EFFECT)
//			{ cur=cur->next; continue; }

		if(cur->flags&(LBXGL_BRFL_FLUID|LBXGL_BRFL_EFFECT))
			{ cur=cur->next; continue; }

		if(cur->flags&LBXGL_BRFL_ALPHA)
			LBXGL_Brush_DrawFaces(cur);
		cur=cur->next;
	}

//	if(world->brush_sel)LBXGL_BrushWorld_DrawControlsBrush(world, world->brush_sel);

	lcur=LBXGL_BrushWorld_QueryLightsCam(world);
	while(lcur)
	{
		LBXGL_BrushWorld_DrawBrushesLightVol(world, lcur);
		lcur=lcur->chain;
	}

#if 0
	//draw mapper controls, surprisingly expensive
	cur=fst;
	while(cur)
	{
		LBXGL_BrushWorld_DrawControlsBrush(world, cur);
		cur=cur->next;
	}
#endif

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	glDisable(GL_CULL_FACE);

//	LBXGL_BrushBSP_DrawPortalList(world->bsp, world->portal);

	LBXGL_Shader_FlushState();

	LBXGL_BrushWorld_PopWorld();

	return;
}

void LBXGL_BrushWorld_DrawTestSmallLight(LBXGL_BrushWorld *world)
{
	static int ql=0;
	static float qlt=0;
	float tv0[4], tv1[4];
	LBXGL_Brush *cur, *lst, *fst;
	LBXGL_Light *lcur;
	int i, j, k, l, xs, ys, ns;
	float f, g, d;

	if(!world->lighting)return;
	if(world->lighting!=2)return;

//	if(lbxgl_state->adt_f>=(1.0/5))return;

#if 1
	if(lbxgl_state->adt_f>=(1.0/5))
	{
		xs=lbxgl_draw_light_cxs; ys=lbxgl_draw_light_cys;

		UI_Camera_SetupView3D(0, 0, xs, ys);
		glCullFace(GL_BACK); glEnable(GL_CULL_FACE);

		glClearColor(0.25, 0.25, 0.25, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, tex_lighting);
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0,
			lbxgl_draw_light_xs, lbxgl_draw_light_ys, 0);

		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}
#endif

//	LBXGL_BrushWorld_DrawTestShadowvis(world);

	if(tex_lighting<=0)tex_lighting=
		Tex_EmptyTextureRGB(lbxgl_draw_light_xs, lbxgl_draw_light_xs);

	lbxgl_draw_light_cxs=lbxgl_draw_light_xs;
	lbxgl_draw_light_cys=lbxgl_draw_light_ys;

	if((V3F_DIST(lbxgl_cam->org, lbxgl_cam->lorg)>1) ||
		(V3F_DIST(lbxgl_cam->ang, lbxgl_cam->lang)>=5))
	{
		lbxgl_draw_light_cxs>>=1;
		lbxgl_draw_light_cys>>=1;
	}

	ns=world->shadows;

//	if(lbxgl_state->adt2_f>=(1.0/15))
//		{ lbxgl_draw_light_cxs>>=1; lbxgl_draw_light_cys>>=1; ns>>=2; }
//	if(lbxgl_state->adt2_f>=(1.0/10))
//		{ lbxgl_draw_light_cxs>>=1; lbxgl_draw_light_cys>>=1; ns=0; }

#if 0
	f=lbxgl_state->time_f-qlt;
	if((f<0) || (f>0.25))
	{
		if(lbxgl_state->adt_f<=(1.0/25))ql--;
		if(lbxgl_state->adt_f>=(1.0/10))ql++;
		if(ql<=0)ql=0; if(ql>=4)ql=4;
		qlt=lbxgl_state->time_f;
	}
#endif

	lbxgl_draw_light_cxs>>=ql; lbxgl_draw_light_cys>>=ql; ns>>=ql;
	if(ql==4)ns=0;

	xs=lbxgl_draw_light_cxs; ys=lbxgl_draw_light_cys;



	LBXGL_BrushWorld_DrawBuildFast(world);

	UI_Camera_SetupView3D(0, 0, xs, ys);
	glCullFace(GL_BACK); glEnable(GL_CULL_FACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


#if 1
	if((shader_phong<0) && !shader_nophong)
	{
//		shader_phong=PDGL_LoadShader("pointlight");
		shader_phong_spot=PDGL_LoadShader("spotlight");

		shader_phong=PDGL_LoadShader("bgb_pointlight0");
		shader_phong2=PDGL_LoadShader("bgb_pointlight1");

		if(shader_phong<0)
			shader_nophong=1;
	}
#endif

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	fst=world->brush;

//	cur=world->brush; fst=NULL;
//	while(cur) { cur->chain=fst; fst=cur; cur=cur->next; }

	LBXGL_BrushWorld_DrawNodeBrushesDark(world, world->bsp);
	LBXGL_BrushWorld_DrawModelsDark(world);

	lcur=LBXGL_BrushWorld_QueryLightsCam(world); i=ns;
	while(lcur && i)
	{
		if(lcur->flags&LBXGL_LFL_NOSHADOW)
			break;

		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		lst=LBXGL_BrushWorld_QueryBrushesLight(world, fst, lcur);
		LBXGL_BrushWorld_DrawShadowsLight(world, lst, lcur);
		lst=LBXGL_BrushWorld_QueryBrushesLight2(world, fst, lcur);
		LBXGL_BrushWorld_DrawBrushesLight(world, lst, lcur);
//		LBXGL_BrushWorld_DrawBrushesLightVol(world, lcur);
		glDisable(GL_STENCIL_TEST);
		i--; lcur=lcur->chain;
	}

	while(lcur)
	{
		lst=LBXGL_BrushWorld_QueryBrushesLight2(world, fst, lcur);
		LBXGL_BrushWorld_DrawBrushesLight(world, lst, lcur);
		lcur=lcur->chain;
	}

	glBindTexture(GL_TEXTURE_2D, tex_lighting);
//	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0,
//		lbxgl_draw_light_xs, lbxgl_draw_light_ys, 0);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0,
		lbxgl_draw_light_xs, lbxgl_draw_light_ys, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

