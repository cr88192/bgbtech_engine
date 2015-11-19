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

extern int lbxgl_shader_gloss;

int shader_nophong=0;
int shader_nolightmat=0;

int shader_phong=-1;
int shader_phong_spot=-1;
int shader_phong2=-1;
int shader_phong2_norm=-1;
int shader_phong2_box=-1;
int shader_phong2_nbox=-1;

int tex_light_defnorm=-1;
int tex_light_defgloss=-1;
int tex_light_def_falloff=-1;

extern int tex_screen;
extern int tex_lighting;

int screen_fbo;				//FBO for screen
int tex_screen_fbo;			//screen texture (FBO)
int tex_screen_depth_fbo;	//depth texture (FBO)
int tex_screen_stencil_fbo;	//stencil texture (FBO)
int tex_screen_bloom;

int tex_screen_cubemap;
int tex_gloss_cubemap;

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

int LBXGL_BrushWorld_CheckNodeVisibleFast(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int i;
	
	if(!node->mid)
		return(0);

	i=node->id;
	i=world->autovis[i>>3]&(1<<(i&7));
	if(!i)return(0);

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

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_PTLIGHT|
				LBXGL_BRFL_VISCLIP))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_GetCamAbsOrg(
	LBXGL_BrushWorld *world, float *tv)
{
	tv[0]=lbxgl_cam->org[0]+world->cam_reforg[0];
	tv[1]=lbxgl_cam->org[1]+world->cam_reforg[1];
	tv[2]=lbxgl_cam->org[2]+world->cam_reforg[2];
}

float LBXGL_BrushWorld_CameraPlaneOffset(
	LBXGL_BrushWorld *world, float *norm)
{
	float tv[3];
	float f;
	
	LBXGL_BrushWorld_GetCamAbsOrg(world, tv);
	f=V3F_NDOT(tv, norm);
//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	return(f);
}

void LBXGL_BrushWorld_DrawNodeBrushesDark(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
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

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_VISCLIP))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawFacesShadow(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesFinal(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int t0, t1;
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
	if(f<0)
	{
		t0=PDGL_TimeMS();
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidFinal(world, node);
		t1=PDGL_TimeMS();
		if((t1-t0)>15)return;
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->rnode);
	}else
	{
		t0=PDGL_TimeMS();
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidFinal(world, node);
		t1=PDGL_TimeMS();
		if((t1-t0)>15)return;
		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, node->lnode);
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesMidFlat(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_VISCLIP))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawSolidFlat(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesFlat(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
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

	if(world->lighting==3)
	{
		pdglDepthMask(GL_FALSE);

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluid(cur);
			cur=cur->chain;
		}
		LBXGL_Voxel_DrawWorldFluid(world);

		pdglDepthMask(GL_TRUE);
		return;
	}

#if 1
	pdglColorMask(0, 0, 0, 0);
	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluidFlat(cur);
		cur=cur->chain;
	}
//	LBXGL_Voxel_DrawWorldFluidFlat(world);
	pdglColorMask(1, 1, 1, 1);

	LBXGL_Voxel_DrawFluidSurfaceFogVolume(world);
#endif

	pdglDepthMask(GL_FALSE);

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluid(cur);
		cur=cur->chain;
	}
	LBXGL_Voxel_DrawWorldFluid(world);

	pdglDepthMask(GL_TRUE);

	if(shader_nophong || (world->shadows<=0) ||
		(world->lighting==3) ||
		(world->flags&BTGE_WFL_ALPHABUDGET) ||
		(world->flags&BTGE_WFL_LIGHTBUDGET))
			return;

	LBXGL_Shader_FlushState();

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluid2(cur);
		cur=cur->chain;
	}
	LBXGL_Voxel_DrawWorldFluid2(world);
}

void LBXGL_BrushWorld_DrawBrushesFluidSubmergedBasic(
	LBXGL_BrushWorld *world, LBXGL_Brush *lst, LBXGL_Brush *brsh)
{
	float pt[4];
	LBXGL_Brush *cur;
	int i, ty;

	LBXGL_Brush_DrawFluidSurfaces(world, lst);

#if 1
	ty=0; if(brsh)ty=brsh->flags&LBXGL_TXFL_FLUID_MASK;

//	pt[0]=0.02; pt[1]=0.06; pt[2]=0.10; pt[3]=1.0/384;
	pt[0]=0.02; pt[1]=0.06; pt[2]=0.10; pt[3]=1.0/256;

	if(ty==LBXGL_TXFL_FLUID_SLIME)
		{ pt[0]=0.02; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/256; }
	if(ty==LBXGL_TXFL_FLUID_SEWER)
		{ pt[0]=0.10; pt[1]=0.08; pt[2]=0.04; pt[3]=1.0/256; }
	if(ty==LBXGL_TXFL_FLUID_LAVA)
		{ pt[0]=0.5; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/64; }

	pt[3]*=100;
//	LBXGL_Brush_DrawFogVolume(world, lst, pt);
	LBXGL_BrushWorld_DrawColorOverlay(world, pt);
#endif
}

void LBXGL_BrushWorld_DrawBrushesFluidSubmerged(LBXGL_BrushWorld *world,
	LBXGL_Brush *lst, LBXGL_Brush *brsh)
{
	float pt[4];
	LBXGL_Brush *cur;
	int i, ty;

	if(shader_nophong || (world->shadows<=0) ||
		(world->flags&BTGE_WFL_ALPHABUDGET) ||
		(world->flags&BTGE_WFL_LIGHTBUDGET) ||
		(world->lighting==3))
	{
		LBXGL_BrushWorld_DrawBrushesFluidSubmergedBasic(world, lst, brsh);
		return;
	}

	LBXGL_Brush_DrawFluidSurfaces(world, lst);

#if 1
	ty=0; if(brsh)ty=brsh->flags&LBXGL_TXFL_FLUID_MASK;
	pt[0]=0.02; pt[1]=0.06; pt[2]=0.10; pt[3]=1.0/384;
	if(ty==LBXGL_TXFL_FLUID_SLIME)
		{ pt[0]=0.02; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/256; }
	if(ty==LBXGL_TXFL_FLUID_SEWER)
		{ pt[0]=0.10; pt[1]=0.08; pt[2]=0.04; pt[3]=1.0/256; }
	if(ty==LBXGL_TXFL_FLUID_LAVA)
		{ pt[0]=0.5; pt[1]=0.10; pt[2]=0.04; pt[3]=1.0/64; }
	LBXGL_Brush_DrawFogVolume(world, lst, pt);
	LBXGL_Voxel_DrawFluidFogVolume(world, pt);
#endif

	LBXGL_Shader_FlushState();
	LBXGL_BrushWorld_DrawShaderOverlay(world,
		"textures/shader/watery_overlay");
}

void LBXGL_BrushWorld_DrawBrushesFluid(LBXGL_BrushWorld *world)
{
	float pt[4];
	LBXGL_Brush *cur, *lst, *nxt, *brsh;
	int i, ty;

	world->fluidcam=0;

#if 1
//	lst=LBXGL_BrushBSP_QueryTreeNodeVisible(
//		world, world->bsp, lbxgl_cam->org, 2048);
	lst=LBXGL_BrushBSP_QueryTreeNode(
		world->bsp, lbxgl_cam->org, 2048);

	cur=lst; lst=NULL;
	while(cur)
	{
		nxt=cur->chain;

		if(cur->flags&LBXGL_BRFL_FLUID)
		{
//			if(V3F_DIST(cur->org, lbxgl_cam->org)>(512+cur->rad))
//				{ cur=nxt; continue; }

			if(cur->flags&LBXGL_BRFL_VISCLIP)
				{ cur=nxt; continue; }

			if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
				{ cur=nxt; continue; }

			if(LBXGL_Brush_CheckPointBrush(cur, lbxgl_cam->org))
				{ world->fluidcam=1; brsh=cur; }

			cur->chain=lst; lst=cur;

		}
		cur=nxt;
	}
#endif

#if 0
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
#endif

	if(!world->fluidcam && LBXGL_Voxel_CheckFluidCam(world))
		{ world->fluidcam=1; brsh=NULL; }

	if(!lst && !world->fluidcam && !world->vox_region)
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
		if(cur->flags&LBXGL_BRFL_VISCLIP)
			{ cur=cur->next; continue; }

		i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
		if(i==LBXGL_TXFL_EFFECT_FOG)
		{
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
		LBXGL_Brush_DrawFogVolume(world, fga[i], fva+i*4);
	}
}

void LBXGL_BrushWorld_DrawBrushesDecal(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur, *nxt, *lst;
	float tv[4];
	float f, g, h, d;
	int i, j, n;


#if 1
	LBXGL_BrushWorld_GetCamAbsOrg(world, tv);
//	lst=LBXGL_BrushBSP_QueryTreeNodeVisible(
//		world, world->bsp, lbxgl_cam->org, 512);
	lst=LBXGL_BrushBSP_QueryTreeNodeVisible(
		world, world->bsp, tv, 512);
	cur=lst; lst=NULL;
	while(cur)
	{
		nxt=cur->chain;

		i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
		if((i==LBXGL_TXFL_EFFECT_DECAL) ||
			(i==LBXGL_TXFL_EFFECT_DECAL2) ||
			(i==LBXGL_TXFL_EFFECT_DECAL_ALPHA))
		{
//			if(V3F_DIST(cur->org, lbxgl_cam->org)>(512+cur->rad))
//				{ cur=nxt; continue; }

			if(cur->flags&LBXGL_BRFL_VISCLIP)
				{ cur=nxt; continue; }

			if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
				{ cur=nxt; continue; }
			
			cur->chain=lst; lst=cur;

		}
		cur=nxt;
	}
#endif

#if 0
	cur=world->brush; lst=NULL;
	while(cur)
	{
		i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
		if((i==LBXGL_TXFL_EFFECT_DECAL) ||
			(i==LBXGL_TXFL_EFFECT_DECAL2) ||
			(i==LBXGL_TXFL_EFFECT_DECAL_ALPHA))
		{
			if(V3F_DIST(cur->org, lbxgl_cam->org)>(512+cur->rad))
				{ cur=cur->next; continue; }
			
			cur->chain=lst; lst=cur;

		}
		cur=cur->next;
	}
#endif

	cur=lst; j=-1;
	while(cur)
	{
		i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
		if(i!=j)
		{
			if(i==LBXGL_TXFL_EFFECT_DECAL)
				LBXGL_Shader_BlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			if(i==LBXGL_TXFL_EFFECT_DECAL2)
				LBXGL_Shader_BlendFunc(GL_ZERO, GL_SRC_COLOR);
			if(i==LBXGL_TXFL_EFFECT_DECAL_ALPHA)
				LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			j=i;
		}

		LBXGL_Brush_DrawFaces(cur);
		cur=cur->chain;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesMid(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur, *nxt;
	int i;

	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur)
	{
//		if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
//			{ cur=cur->nnext; continue; }

		nxt=cur->nnext;

		if(cur->patch)
		{
			LBXGL_BrushPatch_DrawFaces(cur);
			cur=nxt;
			continue;
		}

		if(cur->flags&LBXGL_BRFL_VISCLIP)
			{ cur=nxt; continue; }
		if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
			{ cur=nxt; continue; }

//		if(!LBXGL_Brush_CheckBrushFrustum(cur))
//			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawFaces(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushes(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
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


void LBXGL_BrushWorld_DrawNodeBrushesFastMid(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur)
	{
//		if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
//			{ cur=cur->nnext; continue; }

//		if(!LBXGL_Brush_CheckBrushFrustum(cur))
//			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawFaces(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesFast(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesFast(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesFastMid(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesFast(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesFast(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesFastMid(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesFast(world, node->lnode);
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesFastShadowMid(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_Light *light)
{
	LBXGL_Brush *cur;
	int i;

	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur)
	{
//		if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
//			{ cur=cur->nnext; continue; }

		if(cur->flags&BTGE_BRFL_NOSHADOW)
			{ cur=cur->nnext; continue; }

		if(!LBXGL_Brush_CheckBrushFrustum(cur))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawFaces(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesFastShadow(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node,
	LBXGL_Light *light)
{
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesFastShadow(
			world, node->lnode, light);
		LBXGL_BrushWorld_DrawNodeBrushesFastShadowMid(world, node, light);
		LBXGL_BrushWorld_DrawNodeBrushesFastShadow(
			world, node->rnode, light);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesFastShadow(
			world, node->rnode, light);
		LBXGL_BrushWorld_DrawNodeBrushesFastShadowMid(world, node, light);
		LBXGL_BrushWorld_DrawNodeBrushesFastShadow(
			world, node->lnode, light);
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
		if(cur->flags&LBXGL_BRFL_ALPHA)
			{ cur=cur->nnext; continue; }

		pdglColor4fv(cur->ptrgb);
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
	int t0, t1;
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
	if(f<0)
	{
		t0=PDGL_TimeMS();
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidPointLight(world, node);
		t1=PDGL_TimeMS();
		if((t1-t0)>10)return;
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->rnode);
	}else
	{
		t0=PDGL_TimeMS();
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidPointLight(world, node);
		t1=PDGL_TimeMS();
		if((t1-t0)>10)return;
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, node->lnode);
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesMidAlpha(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

	cur=node->mid;
	while(cur)
	{
//		if(!LBXGL_Brush_CheckBrushFrustumVisible(world, cur))
//			{ cur=cur->nnext; continue; }

		if(!(cur->flags&LBXGL_BRFL_ALPHA))
			{ cur=cur->nnext; continue; }
		if(cur->flags&(LBXGL_BRFL_FLUID|
				LBXGL_BRFL_EFFECT|LBXGL_BRFL_VISCLIP))
			{ cur=cur->nnext; continue; }

		if(!LBXGL_Brush_CheckBrushFrustum(cur))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawFacesAlpha(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_DrawNodeBrushesAlpha(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
	if(f<0)
	{
		LBXGL_BrushWorld_DrawNodeBrushesAlpha(world, node->lnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidAlpha(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesAlpha(world, node->rnode);
	}else
	{
		LBXGL_BrushWorld_DrawNodeBrushesAlpha(world, node->rnode);
		LBXGL_BrushWorld_DrawNodeBrushesMidAlpha(world, node);
		LBXGL_BrushWorld_DrawNodeBrushesAlpha(world, node->lnode);
	}
}


void LBXGL_BrushWorld_UpdateNodeBrushesMidPT(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i;

	if(!LBXGL_BrushWorld_CheckNodeFrustumVisible(world, node))
		return;

	cur=node->mid;
	while(cur)
	{
		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_VISCLIP))
			{ cur=cur->nnext; continue; }

		cur->flags&=~LBXGL_BRFL_PTLIGHT;
//		if(V3F_DIST(cur->org, lbxgl_cam->org)>2048)
		if(V3F_DIST(cur->org, lbxgl_cam->org)>3072)
		{
			cur->flags|=LBXGL_BRFL_PTLIGHT;
			LBXGL_BrushWorld_InitBrushRGB(world, cur);
			cur=cur->nnext; continue;
		}
		if((cur->flags&LBXGL_BRFL_ALPHA) &&
			!(cur->flags&LBXGL_BRFL_FULLBRIGHT))
		{
			cur->flags|=LBXGL_BRFL_PTLIGHT;
			LBXGL_BrushWorld_InitBrushRGB(world, cur);
			cur=cur->nnext; continue;
		}

//		LBXGL_Brush_DrawBrushDark(cur);
		cur=cur->nnext;
	}
}

void LBXGL_BrushWorld_UpdateNodeBrushesPT(
	LBXGL_BrushWorld *world, LBXGL_BrushNode *node)
{
	int t0, t1;
	float f;

	if(!node)return;

//	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	f=LBXGL_BrushWorld_CameraPlaneOffset(world, node->norm);
	if(f<0)
	{
		t0=PDGL_TimeMS();
		LBXGL_BrushWorld_UpdateNodeBrushesPT(world, node->lnode);
		LBXGL_BrushWorld_UpdateNodeBrushesMidPT(world, node);
		t1=PDGL_TimeMS();
		if((t1-t0)>10)return;
		LBXGL_BrushWorld_UpdateNodeBrushesPT(world, node->rnode);
	}else
	{
		t0=PDGL_TimeMS();
		LBXGL_BrushWorld_UpdateNodeBrushesPT(world, node->rnode);
		LBXGL_BrushWorld_UpdateNodeBrushesMidPT(world, node);
		t1=PDGL_TimeMS();
		if((t1-t0)>10)return;
		LBXGL_BrushWorld_UpdateNodeBrushesPT(world, node->lnode);
	}
}


LBXGL_API void LBXGL_BrushWorld_PreLightWorldFast(
	LBXGL_BrushWorld *world, float *vorg)
{
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur, *lfst;

#if 1
	cur=world->brush;
	while(cur)
	{
		LBXGL_BrushWorld_InitBrushRGB(world, cur);
		cur->flags|=LBXGL_BRFL_PTLIGHT;
		cur=cur->next;
	}
	LBXGL_Voxel_ClearWorldPointLight(world);

//	lcur=LBXGL_BrushWorld_QueryLightsCam(world);
	lcur=LBXGL_BrushWorld_QueryLightsSphere(world, vorg, 2048);
	while(lcur)
	{
		lst=LBXGL_BrushWorld_QueryBrushesLightPT2(
			world, world->brush, lcur);
		LBXGL_BrushWorld_UpdateBrushesLight(world, lst, lcur);
		LBXGL_Voxel_UpdateWorldPointLight(world, lcur);
		lcur=lcur->chain;
	}
#endif
}

LBXGL_API void LBXGL_BrushWorld_DrawWorldFast(LBXGL_BrushWorld *world)
{
	LBXGL_ModelState *mcur;
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur, *lfst;
	int i, j;

	LBXGL_BrushWorld_PushWorld(world);
	LBXGL_BrushWorld_DrawBuildFast(world);
	LBXGL_BrushWorld_PopWorld();

//	LBXGL_BrushWorld_DrawNodeBrushesFlat(world, world->bsp);

	LBXGL_BrushWorld_PushWorld(world);

//	pdglCullFace(GL_BACK);
//	pdglEnable(GL_CULL_FACE);

	pdglDepthFunc(GL_LEQUAL);
	pdglColor4f(1, 1, 1, 1);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

//	pdglDisable(GL_CULL_FACE);
	LBXGL_Sky_DrawSkyBasic();
	LBXGL_BrushDraw2D_DrawWorldSky2D(world);
//	pdglEnable(GL_CULL_FACE);

	LBXGL_BrushWorld_DrawNodeBrushesFast(world, world->bsp);
//	LBXGL_BrushWorld_DrawNodeBrushes(world, world->bsp);

	LBXGL_Voxel_DrawWorld(world);
	LBXGL_BrushWorld_DrawModels(world);
//	LBXGL_BrushWorld_DrawModelsFlat(world);

//	pdglEnable(GL_CULL_FACE);

//	LBXGL_Sky_DrawSkyBasic();

	LBXGL_BrushWorld_PopWorld();
}

LBXGL_API void LBXGL_BrushWorld_DrawWorldLightViewFast(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_ModelState *mcur;
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur, *lfst;
	int i, j;

	LBXGL_BrushWorld_PushWorld(world);
	LBXGL_BrushWorld_DrawBuildFast(world);
	LBXGL_BrushWorld_PopWorld();

//	LBXGL_BrushWorld_DrawNodeBrushesFlat(world, world->bsp);

	LBXGL_BrushWorld_PushWorld(world);

//	pdglCullFace(GL_BACK);
//	pdglEnable(GL_CULL_FACE);

	pdglDepthFunc(GL_LEQUAL);
	pdglColor4f(1, 1, 1, 1);

//	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	LBXGL_Shader_Color4f(1, 1, 1, 1);

	if(light->flags&LBXGL_LFL_SUN)
	{
//		pdglDisable(GL_CULL_FACE);
		LBXGL_Sky_DrawSkyBasic();
		LBXGL_BrushDraw2D_DrawWorldSky2D(world);
//		pdglEnable(GL_CULL_FACE);
	}

	LBXGL_BrushWorld_DrawNodeBrushesFastShadow(world, world->bsp, light);
//	LBXGL_BrushWorld_DrawNodeBrushesFast(world, world->bsp);
//	LBXGL_BrushWorld_DrawNodeBrushes(world, world->bsp);

	LBXGL_Voxel_DrawWorldForLight(world, light);
//	LBXGL_BrushWorld_DrawModels(world);
//	LBXGL_BrushWorld_DrawModelsFlat(world);

//	pdglEnable(GL_CULL_FACE);

//	LBXGL_Sky_DrawSkyBasic();

	LBXGL_BrushWorld_PopWorld();
}

LBXGL_API void LBXGL_BrushWorld_DrawBrushesFlat(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst)
{
	LBXGL_ModelState *mcur;
	int i, j;

	LBXGL_BrushWorld_PushWorld(world);
	LBXGL_BrushWorld_DrawBuildFast(world);
	LBXGL_BrushWorld_PopWorld();

	LBXGL_BrushWorld_DrawNodeBrushesFlat(world, world->bsp);
}

LBXGL_API int LBXGL_BrushWorld_DrawSetupFBO(LBXGL_BrushWorld *world)
{
	int ia[16];
	int i;

#ifdef GLES
	return(0);
#endif

#ifndef GLES

#if 0
	if(lbxgl_state->lxs && lbxgl_state->lys &&
		((lbxgl_state->xs!=lbxgl_state->lxs) ||
		 (lbxgl_state->ys!=lbxgl_state->lys)))
	{
		screen_fbo=0;
		tex_screen_fbo=0;
		tex_screen_depth_fbo=0;
		tex_screen_stencil_fbo=0;
	}
#endif

	if(screen_fbo<=0)
	{
		i=pdglGenFramebuffers(1, &screen_fbo);
		if(i<0)
		{
//			usefbo=0;
//			ConCmds_CvarSetNum("r_usefbo", 0);
			return(0);
		}

		if(screen_fbo>0)
		{
			if(tex_screen_fbo<=0)
				tex_screen_fbo=Tex_ScreenshotTexture();

			if(tex_screen_depth_fbo<=0)
			{
				tex_screen_depth_fbo=Tex_ScreenshotDepthTexture();

#if 0
				pdglGenRenderbuffers(1, &tex_screen_depth_fbo);
				pdglBindRenderbuffer(GL_RENDERBUFFER_EXT, 
					tex_screen_depth_fbo);
//				pdglRenderbufferStorage(GL_RENDERBUFFER_EXT, 
//					GL_DEPTH_COMPONENT32, 1024, 1024);

//				pdglRenderbufferStorage(GL_RENDERBUFFER_EXT, 
//					GL_DEPTH_STENCIL_EXT, 1024, 1024);

				pdglRenderbufferStorage(GL_RENDERBUFFER_EXT, 
					GL_DEPTH24_STENCIL8_EXT, 1024, 1024);
#endif
   			}

#if 0
			if(tex_screen_stencil_fbo<=0)
			{
//				tex_screen_depth_fbo=Tex_ScreenshotDepthTexture();

#if 1
				pdglGenRenderbuffers(1, &tex_screen_stencil_fbo);
				pdglBindRenderbuffer(GL_RENDERBUFFER_EXT, 
					tex_screen_stencil_fbo);
				pdglRenderbufferStorage(GL_RENDERBUFFER_EXT, 
					GL_STENCIL_INDEX, 1024, 1024);
#endif
   			}
#endif

			printf("Setup FBO\n");
			pdglBindFramebuffer(GL_FRAMEBUFFER_EXT, screen_fbo);

#if 0
			pdglFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, 
				GL_DEPTH_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
				tex_screen_depth_fbo);
#endif

#if 0
#if 1
			pdglFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, 
				GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
				tex_screen_depth_fbo);
#endif

#if 1
			pdglFramebufferRenderbuffer(GL_FRAMEBUFFER_EXT, 
				GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT,
				tex_screen_depth_fbo);
#endif
#endif

#if 1
			pdglBindTexture(GL_TEXTURE_2D, tex_screen_depth_fbo);
#if 1
			pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
				GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D,
				tex_screen_depth_fbo, 0);
#endif

#if 1
			pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
				GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D,
				tex_screen_depth_fbo, 0);
#endif
#endif

#if 0
			pdglBindTexture(GL_TEXTURE_2D, tex_screen_depth_fbo);
			pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
				GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D,
				tex_screen_depth_fbo, 0);
#endif

			pdglBindTexture(GL_TEXTURE_2D, tex_screen_fbo);
			i=pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
				GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,
				tex_screen_fbo, 0);
			if(i<0) { printf("Set FBO Texture Fail\n", i); }
				
			ia[0]=GL_COLOR_ATTACHMENT0_EXT;
			ia[1]=GL_DEPTH_ATTACHMENT_EXT;
			pdglDrawBuffers(1, ia);
				
			i=pdglCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
			if(i!=GL_FRAMEBUFFER_COMPLETE_EXT)
			{
				printf("Setup FBO Fail %08X\n", i);

//				usefbo=0;
//				ConCmds_CvarSetNum("r_usefbo", 0);
				return(0);
			}

			pdglBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
		}
	}
		
	return(1);
#endif
}

LBXGL_API void LBXGL_BrushWorld_DrawBeginFBO(LBXGL_BrushWorld *world)
{
#ifndef GLES
	pdglBindFramebuffer(GL_FRAMEBUFFER_EXT, screen_fbo);
//	pdglDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);

//	pdglClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	pdglClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|
		GL_STENCIL_BUFFER_BIT);
//	camatt=1.0;

#if 0
#if 1
	pdglBindTexture(GL_TEXTURE_2D, tex_screen_depth_fbo);
	pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
		GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D,
		tex_screen_depth_fbo, 0);
#endif

	pdglBindTexture(GL_TEXTURE_2D, tex_screen_fbo);
	pdglFramebufferTexture2D(GL_FRAMEBUFFER_EXT, 
		GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,
		tex_screen_fbo, 0);
#endif

#endif	//GLES
}

LBXGL_API void LBXGL_BrushWorld_DrawEndFBO(LBXGL_BrushWorld *world)
{
	int i;

#ifndef GLES
	pdglFinish();
	//done drawing to the FBO, clean up and draw image to screen
	pdglBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
//	pdglDrawBuffer(GL_BACK);

	i=ConCmds_CvarGetNum("r_usebloom");
	if(i>0)LBXGL_BrushWorld_DrawUpdateBloom(world);

	LBXGL_BrushWorld_DrawScreenTexture(world,
		tex_screen_fbo, world->cam_expose);
	if(i>0)LBXGL_BrushWorld_DrawScreenTexture2(world,
		tex_screen_bloom, 1);

	i=ConCmds_CvarGetNum("r_cellshade");
	if(i>0)
	{
		LBXGL_Shader_FlushState();
//		LBXGL_BrushWorld_DrawShaderOverlay(world,
//			"textures/shader/cellshade_overlay");
		LBXGL_BrushWorld_DrawScreenShaderOverlay(world,
			"textures/shader/cellshade_overlay",
			tex_screen_fbo, tex_screen_depth_fbo, world->cam_expose);
	}

	i=ConCmds_CvarGetNum("r_anaglyph");
	if(i>0)
	{
		LBXGL_Shader_FlushState();
		LBXGL_BrushWorld_DrawScreenShaderOverlay(world,
			"textures/shader/anaglyph_overlay",
			tex_screen_fbo, tex_screen_depth_fbo, world->cam_expose);
	}

	i=ConCmds_CvarGetNum("r_stereo3d");
	if(i>0)
	{
		LBXGL_Shader_FlushState();
		LBXGL_BrushWorld_DrawScreenShaderOverlay(world,
			"textures/shader/stereo3d_overlay",
			tex_screen_fbo, tex_screen_depth_fbo, world->cam_expose);
	}

	i=ConCmds_CvarGetNum("r_splitstereo3d");
	if(i>0)
	{
		LBXGL_Shader_FlushState();
		LBXGL_BrushWorld_DrawScreenShaderOverlay(world,
			"textures/shader/split3d_overlay",
			tex_screen_fbo, tex_screen_depth_fbo, world->cam_expose);
	}
#endif	//GLES
}

LBXGL_API void LBXGL_BrushWorld_DrawBrushes(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst)
{
	static short *stbuf=NULL;
	static float camatt2=1.0;
	int ia[16];
	LBXGL_BrushNode *node;
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur, *lfst;
	LBXGL_ModelState *mcur;
	float f, g, h, camatt;
	int t0, t1, t2, t3, usefbo;
	int i, j;

	LBXGL_BrushWorld_PushWorld(world);

	LBXGL_BrushWorld_DrawBuildFast(world);
//	LBXGL_BrushWorld_UpdateModels(world);

	LBXGL_BrushWorld_PopWorld();

//	LBXGL_Shader_LoadIdentityMatrix();

	world->lighting=btCvarGetf("r_lighting");
	world->shadows=btCvarGetf("r_shadows");

	LBXGL_Texture_Update(lbxgl_state->dt_f);

	if(!(lbxgl_state->doshot&32))
	{
		if(lbxgl_state->time_f<world->time_turtle)
			world->lighting=3;
	}

#if 1
	if(!world->lighting)
	{
		pdglCullFace(GL_BACK);
		LBXGL_Shader_ForceDraw(1);		//force drawing nodraw faces

#if 1
		pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		pdglColor4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawNodeBrushes(world, world->bsp);
//		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, world->bsp);
//		LBXGL_Terrain_DrawFinal(world);

//		LBXGL_BrushWorld_DrawModelsFinal(world);
#endif

		LBXGL_Shader_FlushState();

		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		pdglColor4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawNodeBrushesAlpha(world, world->bsp);

		LBXGL_Shader_ForceDraw(0);

		LBXGL_Terrain_DrawTexture(world);

//		if(world->brush_sel)LBXGL_BrushWorld_DrawControlsBrush(world, world->brush_sel);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawModels(world);

		LBXGL_BrushGround_DrawGroundFaces(world);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		return;
	}
#endif

#if 1
	if(world->lighting==3)
	{
		pdglCullFace(GL_BACK);
//		LBXGL_Shader_ForceDraw(1);		//force drawing nodraw faces

#if 1
		pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
		pdglDisable(GL_STENCIL_TEST);
		pdglDisable(GL_ALPHA_TEST);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		pdglColor4f(1, 1, 1, 1);

//		LBXGL_BrushWorld_DrawNodeBrushes(world, world->bsp);
//		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, world->bsp);
//		LBXGL_Terrain_DrawFinal(world);
		LBXGL_BrushDraw2_DrawWorldBasic(world);

//		LBXGL_BrushWorld_DrawModelsFinal(world);
#endif

#if 0
			LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//			LBXGL_Shader_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			LBXGL_Shader_SetFlat(1);
			LBXGL_Shader_BindTexture(-1);
			LBXGL_Voxel_DrawWorldDark(world);
			LBXGL_Shader_SetFlat(0);
			LBXGL_Shader_BindTexture(-1);
#endif

#if 1
//		LBXGL_Shader_SetFinal(1);

//		pdglBlendFunc(GL_DST_COLOR, GL_ZERO);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

		pdglDisable(GL_STENCIL_TEST);
		pdglDisable(GL_ALPHA_TEST);

//		LBXGL_Voxel_DrawWorldFinal(world);
//		LBXGL_Voxel_DrawWorld(world);
		LBXGL_Voxel_DrawWorldStaticLight(world);

//		LBXGL_Shader_SetFinal(0);
#endif

#if 1
		LBXGL_BrushWorld_DrawModelsAlpha(world);

		LBXGL_Shader_BindTexture(-1);
		LBXGL_Shader_FlushState();

		LBXGL_BrushWorld_DrawBrushesFluid(world);
		LBXGL_BrushWorld_DrawBrushesFog(world);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		pdglEnable(GL_CULL_FACE);

		LBXGL_Voxel_DrawWorldAlpha(world);
#endif

		LBXGL_Shader_FlushState();

		LBXGL_Shader_Color4f(1, 1, 1, 1);
		pdglDisable(GL_STENCIL_TEST);
		pdglDisable(GL_ALPHA_TEST);
//		pdglColor4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawNodeBrushesAlpha(world, world->bsp);

		LBXGL_Shader_ForceDraw(0);

		LBXGL_Terrain_DrawTexture(world);

//		if(world->brush_sel)LBXGL_BrushWorld_DrawControlsBrush(world, world->brush_sel);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		LBXGL_BrushWorld_DrawModels(world);

//		LBXGL_BrushGround_DrawGroundFaces(world);

		camatt=LBXGL_Voxel_GetCameraSunGamma(world);
		world->cam_light=camatt;
		world->cam_expose=1.0;

#if 1
		t0=PDGL_TimeMS();
		LBXGL_Particle_DrawWorld(world);
		t1=PDGL_TimeMS();
		lbxgl_state->dt_render_particle=t1-t0;
#endif

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		pdglDisable(GL_CULL_FACE);

#if 1
		LBXGL_Shader_BindTexture(-1);
		LBXGL_Shader_FlushState();
		pdglDepthMask(GL_TRUE);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		pdglColorMask(1, 1, 1, 1);
		PDGL_UnbindShader();
#endif

		return;
	}
#endif

	usefbo=ConCmds_CvarGetNum("r_usefbo");
	if(usefbo)
	{
		usefbo=LBXGL_BrushWorld_DrawSetupFBO(world);
		if(!usefbo)
			{ ConCmds_CvarSetNum("r_usefbo", 0); }
	}

	if(usefbo)
	{
		camatt=1.0;
		LBXGL_BrushWorld_DrawBeginFBO(world);
	}

	LBXGL_BrushWorld_PushWorld(world);

	LBXGL_BrushWorld_UpdateSun(world);

	if((world->lighting==1) && (world->shadows>=0))
	{
		if((shader_phong<0) && !shader_nophong)
		{
			shader_phong=PDGL_LoadShader("bgb_pointlight0");

			shader_phong2=PDGL_LoadShader("bgb_pointlight1");
			shader_phong2_norm=PDGL_LoadShader("bgb_pointlight1_norm");
			shader_phong2_box=PDGL_LoadShader("bgb_boxlight1");
			shader_phong2_nbox=PDGL_LoadShader("bgb_boxlight1_norm");

//			shader_phong2=PDGL_LoadShader("bgb_pointlight2");
//			shader_phong2_norm=PDGL_LoadShader("bgb_pointlight2");
//			shader_phong2_box=PDGL_LoadShader("bgb_boxlight2");
//			shader_phong2_nbox=PDGL_LoadShader("bgb_boxlight2");

//			shader_phong_spot=PDGL_LoadShader("spotlight");
			shader_phong_spot=PDGL_LoadShader("bgb_spotlight0");

			tex_light_defnorm=Tex_LoadFile(
				"textures/effects/default_normal", NULL, NULL);
			tex_light_defgloss=Tex_LoadFile(
				"textures/effects/default_gloss", NULL, NULL);
			tex_light_def_falloff=Tex_LoadFile(
				"textures/effects/default_falloff", NULL, NULL);

			if(shader_phong<0)
				shader_nophong=1;
			if(shader_phong2<0)
				shader_nophong=1;
			if(shader_nophong || (shader_phong2_norm<0))
				shader_nolightmat=1;
		}

		LBXGL_BrushWorld_UpdateNodeBrushesPT(world, world->bsp);

#if 0
		cur=fst;
		while(cur)
		{
//			if(!LBXGL_Brush_CheckBrushMarkedVisible(world, cur))
//				{ cur=cur->next; continue; }
			
			cur->flags&=~LBXGL_BRFL_PTLIGHT;
			if(V3F_DIST(cur->org, lbxgl_cam->org)>2048)
			{
				cur->flags|=LBXGL_BRFL_PTLIGHT;
				LBXGL_BrushWorld_InitBrushRGB(world, cur);
			}
			if((cur->flags&LBXGL_BRFL_ALPHA) &&
				!(cur->flags&LBXGL_BRFL_FULLBRIGHT))
			{
				cur->flags|=LBXGL_BRFL_PTLIGHT;
				LBXGL_BrushWorld_InitBrushRGB(world, cur);
			}
			cur=cur->next;
		}
#endif

		LBXGL_Voxel_ClearWorldPointLight(world);

		pdglDepthFunc(GL_LEQUAL);

		pdglCullFace(GL_BACK);
		pdglEnable(GL_CULL_FACE);

		pdglEnable(GL_ALPHA_TEST);
		pdglAlphaFunc(GL_GREATER, 0.1);

		t0=PDGL_TimeMS();

		i=ConCmds_CvarGetNum("r_skipdark");
		if(i<=0)
		{
			LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//			LBXGL_Shader_BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			LBXGL_Shader_SetFlat(1);
			LBXGL_Shader_BindTexture(-1);
//			LBXGL_BrushWorld_DrawNodeBrushesDark(world, world->bsp);
			LBXGL_BrushDraw2_DrawWorldDark(world);
			LBXGL_BrushWorld_DrawModelsDark(world);
//			LBXGL_Terrain_DrawDark(world);
			LBXGL_Voxel_DrawWorldDark(world);
			LBXGL_Shader_SetFlat(0);
			LBXGL_Shader_BindTexture(-1);
		}
		
		t1=PDGL_TimeMS();
		lbxgl_state->dt_render_dark=t1-t0;

#if 1
		t0=PDGL_TimeMS();

		world->flags&=~BTGE_WFL_LIGHTBUDGET;	//assume all is good

//		t2=lbxgl_state->dt_render;
		t2=lbxgl_state->dt_render_avg;
		if(t2<10)world->flags&=~(BTGE_WFL_OVERBUDGET|
			BTGE_WFL_ALPHABUDGET);
		if(t2>30)world->flags|=BTGE_WFL_OVERBUDGET;

//		if(lbxgl_state->dt_render_avg>100)
		if(t2>100)
		{
			world->time_turtle=lbxgl_state->time_f+10;
		}

		t2=lbxgl_state->dt_render_alpha;
		if(t2<3)world->flags&=~BTGE_WFL_ALPHABUDGET;
		if(t2>8)world->flags|=BTGE_WFL_ALPHABUDGET;

		//enable parallax mapping
		i=ConCmds_CvarGetNum("r_parallax");
		if(i>0)world->flags|=BTGE_WFL_PARALLAX;
		else world->flags&=~BTGE_WFL_PARALLAX;
		
		//setting dictates faster lighting
		i=ConCmds_CvarGetNum("r_lightbudget");
		if(i>0)world->flags|=BTGE_WFL_LIGHTBUDGET;

		//use faster lighting if over-budget
		if(world->flags&BTGE_WFL_OVERBUDGET)
			world->flags|=
				BTGE_WFL_LIGHTBUDGET;
//		world->flags&=~BTGE_WFL_OVERBUDGET;

		world->flags|=BTGE_WFL_EARLYOUT;
		i=ConCmds_CvarGetNum("r_noearlyout");
		if(i>0)world->flags&=~BTGE_WFL_EARLYOUT;

		//use flag to disable budgeting
		if(i>0)world->flags&=~(BTGE_WFL_LIGHTBUDGET|
			BTGE_WFL_ALPHABUDGET);

		//non-real-time recording forces quality
		if(lbxgl_state->doshot&32)
		{
			world->flags&=~BTGE_WFL_OVERBUDGET;
			world->flags&=~BTGE_WFL_EARLYOUT;
			world->flags&=~(BTGE_WFL_LIGHTBUDGET|
				BTGE_WFL_ALPHABUDGET);
		}

		LBXGL_Shader_SetBudget(world->flags&BTGE_WFL_LIGHTBUDGET);
		LBXGL_Shader_SetParallax(world->flags&BTGE_WFL_PARALLAX);

		lbxgl_state->dt_render_shadow_volume=0;
		lbxgl_state->dt_render_shadow_face=0;

		lfst=NULL;

		i=ConCmds_CvarGetNum("r_skiplight");
		if(i<=0)
		{
			lfst=LBXGL_BrushWorld_QueryLightsCam(world);
			i=world->shadows;

			lcur=lfst;
			while(lcur && i)
			{
				if(lcur->flags&LBXGL_LFL_NOSHADOW)
//				if(lcur->flags&(LBXGL_LFL_NOSHADOW|LBXGL_LFL_SHADOWMAP))
					break;

				if(world->flags&BTGE_WFL_EARLYOUT)
				{
					t1=PDGL_TimeMS();
					if((t1-t0)>20)break;
//					if((t1-t0)>15)break;	//abort if taking too long

//					if((t1-t0)>8)	//try to make it faster
//						world->flags|=BTGE_WFL_LIGHTBUDGET;
				}

				LBXGL_BrushWorld_DrawForLight(world, lcur);
				i--; lcur=lcur->chain;
			}

			t1=PDGL_TimeMS();
			lbxgl_state->dt_render_fullshadow=t1-t0;

//			if((t1-t0)>10)
//				world->flags|=BTGE_WFL_LIGHTBUDGET;

			t0=PDGL_TimeMS();
			while(lcur)
			{
				if(world->flags&BTGE_WFL_EARLYOUT)
				{
					t1=PDGL_TimeMS();
//					if((t1-t0)>15)break;	//abort if taking too long
					if((t1-t0)>25)break;	//abort if taking too long
				}

				LBXGL_BrushWorld_DrawForLightNoShadow(world, lcur);

				lcur=lcur->chain;
			}

			t1=PDGL_TimeMS();
			lbxgl_state->dt_render_noshadow=t1-t0;
		}

#if 1
		lcur=lfst; camatt=0.0;
		while(lcur)
		{
			f=LBXGL_Shadow_LightGammaPoint(lcur, lbxgl_cam->org, 32);

//			if(lcur->flags&LBXGL_LFL_SUN)
//				if(f>1)f=1;

			if(lcur->flags&LBXGL_LFL_SUN)
			{
				g=LBXGL_Voxel_GetCameraSunGamma(world);
				f*=g;
			}

			if(f>camatt)camatt=f;
//			camatt+=f;
			lcur=lcur->chain;
		}
#endif

#if 1
		t0=PDGL_TimeMS();

		lcur=lfst;
		while(lcur)
		{
			t1=PDGL_TimeMS();
			if(world->flags&BTGE_WFL_EARLYOUT)
			{
				if((t1-t0)>100)break;
			}

			lst=LBXGL_BrushWorld_QueryBrushesLightPT(world, fst, lcur);
			LBXGL_BrushWorld_UpdateBrushesLight(world, lst, lcur);
			LBXGL_Voxel_UpdateWorldPointLight(world, lcur);
			lcur=lcur->chain;
		}

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		pdglColor4f(1, 1, 1, 1);
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, world->bsp);
		LBXGL_Voxel_DrawWorldPointLight(world);
		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

		t1=PDGL_TimeMS();
		lbxgl_state->dt_render_vertex=t1-t0;
#endif		

#if 1
		lfst=LBXGL_BrushWorld_QuerySceneryLightsCam(world);
		i=world->shadows; lcur=lfst;
		while(lcur && i)
		{
			if(world->flags&BTGE_WFL_EARLYOUT)
			{
				t1=PDGL_TimeMS();
				if((t1-t0)>10)break;
			}

			LBXGL_BrushWorld_DrawForSceneryLight(world, lcur);
			i--; lcur=lcur->chain;
		}

		t1=PDGL_TimeMS();
		lbxgl_state->dt_render_fullshadow+=t1-t0;
#endif

#endif
	}

	if(world->lighting==2)
	{
		pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
		LBXGL_BrushWorld_DrawNodeBrushesDark(world, world->bsp);
		LBXGL_BrushWorld_DrawModelsDark(world);
		LBXGL_Terrain_DrawDark(world);

		LBXGL_BrushWorld_DrawLightTexture(world);
	}

	if((world->lighting==1) && (world->shadows<0))
	{
		t0=PDGL_TimeMS();
#if 1
		cur=fst;
		while(cur)
		{
			LBXGL_BrushWorld_InitBrushRGB(world, cur);
			cur->flags|=LBXGL_BRFL_PTLIGHT;
			cur=cur->next;
		}
		LBXGL_Voxel_ClearWorldPointLight(world);

		lcur=LBXGL_BrushWorld_QueryLightsCam(world);
		while(lcur)
		{
			lst=LBXGL_BrushWorld_QueryBrushesLightPT(world, fst, lcur);
			LBXGL_BrushWorld_UpdateBrushesLight(world, lst, lcur);
			LBXGL_Voxel_UpdateWorldPointLight(world, lcur);
			lcur=lcur->chain;
		}
#endif

		pdglDepthFunc(GL_LEQUAL);

		pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		pdglDisableTexture2D();
//		LBXGL_BrushWorld_DrawNodeBrushesFlat(world, world->bsp);
		LBXGL_BrushWorld_DrawNodeBrushesPointLight(world, world->bsp);

		LBXGL_Shader_Color4f(1, 1, 1, 1);
//		LBXGL_BrushWorld_DrawModelsFlat(world);
		LBXGL_BrushWorld_DrawModelsPointLight(world);
		LBXGL_Terrain_DrawFlat(world);

		LBXGL_Voxel_DrawWorldPointLight(world);

		t1=PDGL_TimeMS();
		lbxgl_state->dt_render_vertex=t1-t0;
	}

	t0=PDGL_TimeMS();

	LBXGL_BrushWorld_DrawFlashlight(world);

	pdglDepthFunc(GL_LEQUAL);

	pdglCullFace(GL_BACK);
	pdglEnable(GL_CULL_FACE);

	pdglStencilFunc(GL_EQUAL, 0, 255);
	pdglDisable(GL_STENCIL_TEST);

	pdglDepthMask(GL_FALSE);	//disable drawing to Z buffer

	i=ConCmds_CvarGetNum("r_skipfinal");
	if(i<=0)
	{
		LBXGL_Shader_SetFinal(1);

//		pdglBlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

//		LBXGL_BrushWorld_DrawNodeBrushesFinal(world, world->bsp);
		LBXGL_BrushDraw2_DrawWorldFinal(world);
//		LBXGL_Terrain_DrawFinal(world);
		LBXGL_Voxel_DrawWorldFinal(world);

		LBXGL_BrushWorld_DrawModelsFinal(world);

		LBXGL_Shader_SetFinal(0);
	}

	t1=PDGL_TimeMS();
	lbxgl_state->dt_render_final=t1-t0;

#if 1
	if(0)
	{
		lfst=LBXGL_BrushWorld_QueryHiLightsCam(world);
		lcur=lfst;
		while(lcur && i)
		{
			if(world->flags&BTGE_WFL_EARLYOUT)
			{
				t1=PDGL_TimeMS();
				if((t1-t0)>10)break;
			}

			LBXGL_BrushWorld_DrawForHiLight(world, lcur);
			lcur=lcur->chain;
		}
	}
#endif

#if 1
	t0=PDGL_TimeMS();

	pdglEnable(GL_CULL_FACE);
//	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	LBXGL_Shader_BlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
	LBXGL_Shader_Color4f(1, 1, 1, 1);
	LBXGL_BrushWorld_DrawBrushesDecal(world);

	t1=PDGL_TimeMS();
	lbxgl_state->dt_render_decal=t1-t0;
#endif

	t0=PDGL_TimeMS();

	pdglDepthMask(GL_TRUE);	//enable drawing to Z buffer

	i=ConCmds_CvarGetNum("r_skipalpha");
	if(i<=0)
	{
		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		pdglDisable(GL_CULL_FACE);
		LBXGL_Sky_DrawSky();
		LBXGL_BrushDraw2D_DrawWorldSky2D(world);

		pdglEnable(GL_CULL_FACE);

		LBXGL_BrushWorld_DrawModelsAlpha(world);

		LBXGL_Shader_BindTexture(-1);
		LBXGL_Shader_FlushState();

		LBXGL_BrushWorld_DrawBrushesFluid(world);
		LBXGL_BrushWorld_DrawBrushesFog(world);

		LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		pdglEnable(GL_CULL_FACE);

		LBXGL_Voxel_DrawWorldAlpha(world);

		LBXGL_BrushWorld_DrawNodeBrushesAlpha(world, world->bsp);

		lcur=LBXGL_BrushWorld_QueryLightsCam(world);
		while(lcur)
		{
			LBXGL_BrushWorld_DrawBrushesLightVol(world, lcur);
			lcur=lcur->chain;
		}

		pdglDisable(GL_CULL_FACE);
	}

	LBXGL_Shader_BindTexture(-1);
	LBXGL_Shader_FlushState();
	pdglDepthMask(GL_TRUE);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);
	pdglColorMask(1, 1, 1, 1);
	PDGL_UnbindShader();

	t1=PDGL_TimeMS();
	lbxgl_state->dt_render_alpha=t1-t0;

#if 1
	t0=PDGL_TimeMS();
	LBXGL_Particle_DrawWorld(world);
	t1=PDGL_TimeMS();
	lbxgl_state->dt_render_particle=t1-t0;
#endif

#if 1
	for(i=0; i<16; i++)
	{
		pdglActiveTexture(i);
		pdglDisable(GL_TEXTURE_2D);
		pdglDisable(GL_TEXTURE_CUBE_MAP);
	}
	pdglActiveTexture(0);

	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_TEXTURE_COORD_ARRAY);
	pdglDisableClientState(GL_NORMAL_ARRAY);
	pdglDisableClientState(GL_COLOR_ARRAY);

	LBXGL_Shader_BindTexture(-1);
	LBXGL_Shader_FlushState();
	pdglDepthMask(GL_TRUE);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);
	pdglColorMask(1, 1, 1, 1);
	PDGL_UnbindShader();
#endif

	if(usefbo)
	{
		if(camatt>0.01)
		{
//			f=sqrt(1.0/camatt);
//			f=1.0/camatt;
//			f=sqrt(1.0/camatt);
			f=pow(1.0/camatt, 0.25);
//			f=pow(1.0/camatt, 0.1);
		}else
		{
			f=10.0;
		}

		h=ConCmds_CvarGetNum("r_exposure_scale");
		if(h!=0)f*=h;

		if(f>10)f=10;
		if(f<0.25)f=0.25;

		h=ConCmds_CvarGetNum("r_exposure_force");
		if(h!=0)f=h;


//		printf("Exposure=%f, %f\n", f, camatt2);

		g=0.5*lbxgl_state->dt_f;
		if(g>0.25)g=0.25;
		camatt2=camatt2*(1.0-g)+(f*g);
		f=camatt2;

		world->cam_light=camatt;
		world->cam_expose=camatt2;


		LBXGL_BrushWorld_DrawEndFBO(world);
	}else
	{
		world->cam_light=camatt;
		world->cam_expose=1.0;

//		LBXGL_BrushWorld_DrawScreenTexture(world, tex_screen);
	}

	LBXGL_BrushWorld_PopWorld();

	return;
}


LBXGL_API void LBXGL_BrushWorld_DrawUpdateBloom(LBXGL_BrushWorld *world)
{
#if 1
	if(tex_screen_bloom<=0)
		{ tex_screen_bloom=Tex_EmptyTexture(128, 128); }
	LBXGL_BrushWorld_DrawTestSmallScreen(world,
		tex_screen_fbo, tex_screen_bloom, 0.5, 128, 128);
#endif
}

LBXGL_API void LBXGL_BrushWorld_DrawPreRender(LBXGL_BrushWorld *world)
{
	LBXGL_BrushWorld_DrawPreRenderSky(world);
	LBXGL_BrushWorld_DrawSetupModels(world);
}

//LBXGL_BrushWorld_DrawTestSmallScreen(LBXGL_BrushWorld *world,
//	int stex, int dtex, float gam, int xs, int ys);

LBXGL_API void LBXGL_BrushWorld_DrawSetupModels(
	LBXGL_BrushWorld *world)
{
	LBXGL_ModelState *mcur;
	LBXGL_BrushDrawFace *fcur;
	int i, j;

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!(world->mdlvis[i>>3]))
		{
			do { mcur=mcur->next; i++; } while(mcur && (i&7));
			continue;
		}
		
		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }

		fcur=LBXGL_Mdl_QueryDrawFaces(mcur);
		mcur->tmp_drawface=NULL;
		while(fcur)
		{
			fcur->bnext=mcur->tmp_drawface;
			mcur->tmp_drawface=fcur;
			fcur=fcur->next;
		}

		mcur=mcur->next; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawPreRenderSky(LBXGL_BrushWorld *world)
{
	static float tick=0;
	float tv0[3];
	LBXGL_Light *lcur;
	int *rsky;
	int i, j, n;

	if(tex_gloss_cubemap<=0)
	{
		tex_gloss_cubemap=Tex_AllocTexnum();
		LBXGL_BrushWorld_DrawGlossCubeMap(world, tex_gloss_cubemap);
	}
	
//	lbxgl_shader_gloss
	if(tex_screen_cubemap<=0)
	{
		tex_screen_cubemap=Tex_AllocTexnum();

#if 0
		pdglBindTexture(GL_TEXTURE_CUBE_MAP, tex_screen_cubemap);
		for(i=0; i<6; i++)
		{
			j=Tex_AllocTexnum();
			pdglBindTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, j);
		}
#endif

		LBXGL_BrushWorld_DrawRenderToCubeMap(world, lbxgl_cam->org,
			tex_screen_cubemap, 0, 0, NULL);
	}

	lcur=LBXGL_BrushWorld_QueryGlossMapCam(world);
	if(lcur)
	{
		if(lcur->flags&LBXGL_LFL_ENVRENDER)
		{
			if(lcur->envtex<=0)
			{
				lcur->envtex=Tex_AllocTexnum();
				lcur->envdepthtex=Tex_AllocTexnum();
				LBXGL_BrushWorld_DrawRenderToCubeMap(
					world, lcur->org,
					lcur->envtex, lcur->envdepthtex,
					lcur->flags|LBXGL_LFL_CHANGED, lcur);
			}
		}
		if(lcur->envtex>0)
			LBXGL_Shader_SetGlossEnv(lcur->envtex);
	}else
	{
//		LBXGL_Shader_SetGlossEnv(tex_gloss_cubemap);
		LBXGL_Shader_SetGlossEnv(0);
	}

#if 1
	lcur=LBXGL_BrushWorld_QueryLightsCam(world); n=16;
	while(lcur)
	{
		if(lcur->flags&LBXGL_LFL_ENVRENDER)
		{
			if(n<0)
			{
//				LBXGL_BrushWorld_FlushLightEnvMap(world, lcur);
//				lcur->env_time=lbxgl_state->time_f+4.0+(bgbrng_randg()*0.1);
				LBXGL_BrushWorld_UpdateLightEnvMap_Delay(world, lcur);
				lcur=lcur->chain;
				continue;
			}
			n--;
			LBXGL_BrushWorld_UpdateLightEnvMap(world, lcur);
		}
		lcur=lcur->chain;
	}
#endif

	if(lbxgl_state->time_f < tick)
		return;

//	LBXGL_BrushWorld_DrawRenderToCubeMap(world, lbxgl_cam->org,
//		tex_screen_cubemap);
//	LBXGL_Shader_SetGlossEnv(tex_screen_cubemap);

//	LBXGL_Shader_SetGlossEnv(tex_gloss_cubemap);

//	LBXGL_Shader_SetNoGloss(1);

	if(world->skycam)
	{
		LBXGL_Sky_GetAltOrigin(tv0);
		if(V3F_DIST(tv0, lbxgl_cam->org)>512)
		{
			rsky=LBXGL_Sky_RefSkyAlt();
			V3F_ADDSCALE(world->skycam_org, lbxgl_cam->org, 1.0/16, tv0);
			LBXGL_BrushWorld_DrawRenderToCubeFaces(world, tv0, rsky);
			LBXGL_Sky_SetHasAlt(1);
			LBXGL_Sky_SetAltOrigin(lbxgl_cam->org);
		}
	}else
	{
		LBXGL_Sky_SetHasAlt(0);
	}

	tick=lbxgl_state->time_f + 1;
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
		pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);

		pdglClearColor(0.25, 0.25, 0.25, 1);
		pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		pdglBindTexture(GL_TEXTURE_2D, tex_lighting);
		pdglCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0,
			lbxgl_draw_light_xs, lbxgl_draw_light_ys, 0);

		pdglClearColor(0, 0, 0, 1);
		pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	lbxgl_draw_light_cxs>>=ql; lbxgl_draw_light_cys>>=ql; ns>>=ql;
	if(ql==4)ns=0;

	xs=lbxgl_draw_light_cxs; ys=lbxgl_draw_light_cys;



	LBXGL_BrushWorld_DrawBuildFast(world);

	UI_Camera_SetupView3D(0, 0, xs, ys);
	pdglCullFace(GL_BACK);	pdglEnable(GL_CULL_FACE);
//	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | 
		GL_STENCIL_BUFFER_BIT);


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

	pdglDepthFunc(GL_LEQUAL);

	pdglCullFace(GL_BACK);
	pdglEnable(GL_CULL_FACE);

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

		pdglClear(GL_STENCIL_BUFFER_BIT);
		pdglEnable(GL_STENCIL_TEST);
		lst=LBXGL_BrushWorld_QueryBrushesLight(world, fst, lcur);
		LBXGL_BrushWorld_DrawShadowsLight(world, lst, lcur, 0);
		lst=LBXGL_BrushWorld_QueryBrushesLight2(world, fst, lcur);
		LBXGL_BrushWorld_DrawBrushesLight(world, lst, lcur);
//		LBXGL_BrushWorld_DrawBrushesLightVol(world, lcur);
		pdglDisable(GL_STENCIL_TEST);
		i--; lcur=lcur->chain;
	}

	while(lcur)
	{
		lst=LBXGL_BrushWorld_QueryBrushesLight2(world, fst, lcur);
		LBXGL_BrushWorld_DrawBrushesLight(world, lst, lcur);
		lcur=lcur->chain;
	}

	pdglBindTexture(GL_TEXTURE_2D, tex_lighting);
//	pdglCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0,
//		lbxgl_draw_light_xs, lbxgl_draw_light_ys, 0);
	pdglCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0,
		lbxgl_draw_light_xs, lbxgl_draw_light_ys, 0);

	pdglClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void LBXGL_BrushWorld_DrawTestSmallScreen(LBXGL_BrushWorld *world,
	int stex, int dtex, float gam, int xs, int ys)
{
	static byte *stbuf=NULL;
	int ixa, ixb, ixc, ixd;
	int i, j, k, l, m;

#ifndef GLES
	if(!stbuf)stbuf=malloc((xs+1)*(ys+1)*4);

	pdglMatrixMode(GL_PROJECTION_MATRIX);
	pdglPushMatrix();
	
	pdglPushAttrib(GL_VIEWPORT_BIT);

	UI_Camera_SetupView3D(0, 0, xs, ys);
	LBXGL_BrushWorld_DrawScreenTexture(world, stex, gam);

//	i=ConCmds_CvarGetNum("r_usehdr");
//	i=(i==1)?GL_RGBA16F_ARB:GL_RGBA;

	pdglFinish();
	pdglReadPixels(0, 0, xs, ys, GL_RGBA, GL_UNSIGNED_BYTE, stbuf);

	for(m=0; m<4; m++)
	{
		for(i=0; i<ys; i++)
			for(j=0; j<xs; j++)
		{
			ixa=((i+0)*xs+j+0)*4;
			ixb=((i+0)*xs+j+1)*4;
			ixc=((i+1)*xs+j+0)*4;
			ixd=((i+1)*xs+j+1)*4;
			for(k=0; k<3; k++)
			{
				l=(stbuf[ixa+k]+stbuf[ixb+k]+
					stbuf[ixc+k]+stbuf[ixd+k])>>2;
				stbuf[ixa+k]=(stbuf[ixa+k]+l)>>1;
				stbuf[ixb+k]=(stbuf[ixb+k]+l)>>1;
				stbuf[ixc+k]=(stbuf[ixc+k]+l)>>1;
				stbuf[ixd+k]=(stbuf[ixd+k]+l)>>1;
			}
		}
	}

//	pdglBindTexture(GL_TEXTURE_2D, dtex);
//	pdglCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, xs, ys, 0);
//	pdglCopyTexImage2D(GL_TEXTURE_2D, 0, i, 0, 0, xs, ys, 0);

	pdglBindTexture(GL_TEXTURE_2D, dtex);
	pdglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, xs, ys, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, stbuf);

	pdglPopAttrib();
	pdglPopMatrix();

	pdglMatrixMode(GL_MODELVIEW_MATRIX);
#endif
}
