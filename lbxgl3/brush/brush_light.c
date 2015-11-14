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

//extern int lbxgl_shader_gloss;
extern int lbxgl_shader_gloss;

extern int shader_nophong;

extern int shader_phong;
extern int shader_phong_spot;
extern int shader_phong2;
extern int shader_phong2_norm;
extern int shader_phong2_box;
extern int shader_phong2_nbox;

extern int tex_screen;
extern int tex_lighting;
extern int tex_screen_cubemap;
extern int tex_gloss_cubemap;

extern int tex_light_defnorm;
extern int tex_light_defgloss;
extern int tex_light_def_falloff;

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

LBXGL_Light *lbxgl_lightimg_light=NULL;
int lbxgl_lightimg_texnum=0;
int lbxgl_lightimg_baselayers=0;
int lbxgl_lightimg_layers=0;

void LBXGL_BrushWorld_FlushLightStatic(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	if(light->visbrush)
	{
		gcfree(light->visbrush);
		light->visbrush=NULL;
	}
	LBXGL_BrushWorld_FlushLightEnvMap(world, light);
}

LBXGL_Brush *LBXGL_BrushWorld_QueryBrushesLightStatic(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *curb, *lst, *nxt;
	float lr;
	int i, j, k, l, n;

	lst=NULL;
	if(light->flags&LBXGL_LFL_STATIC)
	{
		if(light->visbrush)
		{
			lst=NULL;
			for(i=0; light->visbrush[i]; i++)
			{
				cur=light->visbrush[i];
				cur->chain=lst;
				lst=cur;
			}
		}else
		{
//			lr=LBXGL_Shadow_LightGammaEffectRadius(light, 0.05);
//			lr=light->val[0];
//			lst=LBXGL_BrushBSP_QueryTreeNode(world->bsp, light->org, lr);
			
			if(light->flags&LBXGL_LFL_BOXCULL)
			{
				V3F_ADD(light->org, light->mins, light->amins);
				V3F_ADD(light->org, light->maxs, light->amaxs);

				lr=V3F_LEN(light->boxval);

				if((lr>1024) && !(light->flags&LBXGL_LFL_SUN))
					lr=1024;

				lst=LBXGL_BrushBSP_QueryTreeNode(
					world->bsp, light->org, lr);

				cur=lst; lst=NULL;
				while(cur)
				{
					nxt=cur->chain;
//					if(SolidAABB_BoxCollideP(
//						cur->mins, cur->maxs,
//						light->amins, light->amaxs))
					if(LBXGL_Brush_CheckBoxBrush(cur,
						light->amins, light->amaxs))
//					if(1)
							{ cur->chain=lst; lst=cur; }

					cur=nxt;
				}
			}else
			{
				lr=light->val[0];
				
				if((lr>1024) && !(light->flags&LBXGL_LFL_SUN))
					lr=1024;
				
				lst=LBXGL_BrushBSP_QueryTreeNode(
					world->bsp, light->org, lr);
			}
			
			cur=lst; n=0;
			while(cur) { n++; cur=cur->chain; }
			light->visbrush=gcalloc((n+1)*sizeof(LBXGL_Brush *));
			cur=lst; i=0; k=65536;
			while(cur && ((k--)>0))
			{
				//check for occlusion
//				j=LBXGL_Brush_CheckBrushBrushVolumeList(world,
//					lst, cur, light->org);
//				if(!j) { cur=cur->chain; continue; }

				if(n>256)
				{
					light->visbrush[i++]=cur;
					cur=cur->chain;
					continue;
				}

				curb=lst; l=65536;
				while(curb && ((l--)>0))
				{
					curb->flags&=~LBXGL_BRFL_OPSEL;
					if(curb->flags&(LBXGL_BRFL_EFFECT|
							LBXGL_BRFL_FLUID|LBXGL_BRFL_ALPHA))
						{ curb=curb->chain; continue; }
					if(cur==curb)
						{ curb=curb->chain; continue; }
					j=LBXGL_Brush_CheckBrushBrushVolume(
						cur, curb, light->org);
					if(j==1)break;
					if(j==3)
					{
//						if(!SolidAABB_BoxCollideP(
//								cur->mins, cur->maxs,
//								curb->mins, curb->maxs))
						curb->flags|=LBXGL_BRFL_OPSEL;
					}
					curb=curb->chain;
				}

				if(l<=0)
				{
					printf("LBXGL_BrushWorld_QueryBrushesLightStatic: "
						"Cycle Timeout A\n");
					break;
				}

				if(curb) { cur=cur->chain; continue; }

				curb=lst;
				while(curb)
				{
					if(curb->flags&LBXGL_BRFL_OPSEL)
						curb->flags|=LBXGL_BRFL_STATICSHADOW;
					curb=curb->chain;
				}

				light->visbrush[i++]=cur;
				cur=cur->chain;
			}
			
			if(k<=0)
			{
				printf("LBXGL_BrushWorld_QueryBrushesLightStatic: "
					"Cycle Timeout B\n");
//				break;
			}

		}
	}

	return(lst);
}

int LBXGL_BrushWorld_CheckBrushShadowVisible(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, LBXGL_Light *light)
{
	float vorg[3], sdir[3], vmins[3], vmaxs[3];
	float tv0[3];
	float f, g, h, d, lr, vr, sd;
	int i, j;

	V3F_COPY(world->autovis_mins, vmins);
	V3F_COPY(world->autovis_maxs, vmaxs);

	V3F_SCALEADDSCALE(world->autovis_mins, 0.5,
		world->autovis_maxs, 0.5, vorg);
	vr=V3F_DIST(world->autovis_maxs, vorg);

#if 1
	if(!LBXGL_Brush_CheckBrushShadowFrustum(brush, light->org))
		return(0);

	lr=LBXGL_Shadow_LightGammaEffectRadius(light, 0.05);
	if(!LBXGL_Brush_CheckBoxBrushShadow(brush,
		vmins, vmaxs, light->org, lr))
			return(0);

#if 0
	V3F_SUB(brush->org, light->org, sdir);
	sd=V3F_NORMALIZE(sdir, sdir);
	f=V3F_DOT(vorg, sdir)-V3F_DOT(light->org, sdir);

	if(f>(lr+vr))
		return(0);

	if(f<sd)
	{
		d=V3F_DIST(vorg, brush->org);
		if(d>(cur->rad+vr))
			return(0);
	}else
	{
		g=(f/sd)*brush->rad;
		V3F_ADDSCALE(light->org, sdir, f, tv0);
		d=V3F_DIST(vorg, tv0);
		if(d>(g+vr))
			return(0);
	}
#endif
#endif

	return(1);
}

LBXGL_Brush *LBXGL_BrushWorld_QueryBrushesLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	float vorg[3], sdir[3], vmins[3], vmaxs[3];
	float tv0[3];
	LBXGL_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d, lr, vr, sd;
	int i, j;

	world->shadowcam=0;

	lst=NULL;
	if(light->flags&LBXGL_LFL_STATIC)
	{
		lr=LBXGL_Shadow_LightGammaEffectRadius(light, 0.05);
		lst=LBXGL_BrushWorld_QueryBrushesLightStatic(world, light);
	}else
	{
		lr=LBXGL_Shadow_LightGammaEffectRadius(light, 0.05);
		
		if((lr>512) && !(light->flags&LBXGL_LFL_SUN))
			lr=512;
//			if(lr>512)lr=512;
		
		lst=LBXGL_BrushBSP_QueryTreeNode(world->bsp, light->org, lr);
	}

	vmins[0]=lbxgl_cam->org[0]-16;
	vmins[1]=lbxgl_cam->org[1]-16;
	vmins[2]=lbxgl_cam->org[2]-16;

	vmaxs[0]=lbxgl_cam->org[0]+16;
	vmaxs[1]=lbxgl_cam->org[1]+16;
	vmaxs[2]=lbxgl_cam->org[2]+16;

	V3F_SCALEADDSCALE(world->autovis_mins, 0.5,
		world->autovis_maxs, 0.5, vorg);
	vr=V3F_DIST(world->autovis_maxs, vorg);

#if 1
	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

		if(cur->flags&(LBXGL_BRFL_NOSHADOW|
				LBXGL_BRFL_EFFECT|LBXGL_BRFL_PTLIGHT|
				LBXGL_BRFL_ALPHA))
			{ cur=nxt; continue; }

		if(!(cur->flags&LBXGL_BRFL_STATICSHADOW))
			{ cur=nxt; continue; }

		if(!LBXGL_BrushWorld_CheckBrushShadowVisible(world, cur, light))
			{ cur=nxt; continue; }

#if 1
		cur->flags&=~LBXGL_BRFL_VSHCHK;

		if(LBXGL_Brush_CheckBoxBrushShadow(cur,
			vmins, vmaxs, light->org, lr))
		{
			world->shadowcam=1;
			cur->flags|=LBXGL_BRFL_VSHCHK;
		}
#endif

		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}
	lst=lstb;
#endif

	return(lst);
}

LBXGL_Brush *LBXGL_BrushWorld_QueryBrushesLight2(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d;
	int i, j;

	if(light->flags&LBXGL_LFL_SCENERY)
		return(NULL);

	lst=NULL;
	if(light->flags&LBXGL_LFL_STATIC)
	{
		lst=LBXGL_BrushWorld_QueryBrushesLightStatic(world, light);
	}else
	{
		d=LBXGL_Shadow_LightGammaEffectRadius(light, 0.01);
		if((d>512) && !(light->flags&LBXGL_LFL_SUN))
			d=512;
		lst=LBXGL_BrushBSP_QueryTreeNodeVisible(
			world, world->bsp, light->org, d);
	}

	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

#if 1
		if(cur->node)
		{
			i=cur->node->id;
			i=world->autovis[i>>3]&(1<<(i&7));
			if(!i) { cur=nxt; continue; }
		}
#endif

		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_PTLIGHT|
				LBXGL_BRFL_VISCLIP))
			{ cur=nxt; continue; }

		if(!LBXGL_Brush_CheckBrushFrustum(cur))
			{ cur=nxt; continue; }

		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}

	return(lstb);
}

/** query brushes for light using point lighting */
LBXGL_Brush *LBXGL_BrushWorld_QueryBrushesLightPT(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d;
	int i, j;

	if(light->flags&LBXGL_LFL_SCENERY)
		return(NULL);

	lst=NULL;
	if(light->flags&LBXGL_LFL_STATIC)
	{
		lst=LBXGL_BrushWorld_QueryBrushesLightStatic(world, light);
	}else
	{
		d=LBXGL_Shadow_LightGammaEffectRadius(light, 0.05);
//		lst=LBXGL_BrushBSP_QueryTreeNode(world->bsp, light->org, lr);
		lst=LBXGL_BrushBSP_QueryTreeNodeVisible(
			world, world->bsp, light->org, d);
	}

	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

#if 1
		if(cur->node)
		{
			i=cur->node->id;
			i=world->autovis[i>>3]&(1<<(i&7));
			if(!i) { cur=nxt; continue; }
		}
#endif

		if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_VISCLIP))
			{ cur=nxt; continue; }

		if(!(cur->flags&LBXGL_BRFL_PTLIGHT))
			{ cur=nxt; continue; }

		if(!LBXGL_Brush_CheckBrushFrustum(cur))
			{ cur=nxt; continue; }

		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}

	return(lstb);
}

LBXGL_Brush *LBXGL_BrushWorld_QueryBrushesLightPT2(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *nxt, *lst, *lstb;
	float f, g, h, d;
	int i, j;

	if(light->flags&LBXGL_LFL_SCENERY)
		return(NULL);

	lst=NULL;
	if(light->flags&LBXGL_LFL_STATIC)
	{
		lst=LBXGL_BrushWorld_QueryBrushesLightStatic(world, light);
	}else
	{
		d=LBXGL_Shadow_LightGammaEffectRadius(light, 0.05);
		lst=LBXGL_BrushBSP_QueryTreeNode(world->bsp, light->org, d);
//		lst=LBXGL_BrushBSP_QueryTreeNodeVisible(
//			world, world->bsp, light->org, d);
	}

	cur=lst; lstb=NULL;
	while(cur)
	{
		nxt=cur->chain;

#if 1
		if(cur->node)
		{
			i=cur->node->id;
			i=world->autovis[i>>3]&(1<<(i&7));
			if(!i) { cur=nxt; continue; }
		}
#endif

		if(cur->flags&(LBXGL_BRFL_EFFECT))
			{ cur=nxt; continue; }
		if(!(cur->flags&LBXGL_BRFL_PTLIGHT))
			{ cur=nxt; continue; }

//		if(!LBXGL_Brush_CheckBrushFrustum(cur))
//			{ cur=nxt; continue; }

		cur->chain=lstb; lstb=cur;
		cur=nxt;
	}

	return(lstb);
}

LBXGL_Brush *LBXGL_BrushWorld_QueryBrushesLightB(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	float vmins[3], vmaxs[3];
	LBXGL_Brush *cur, *lst;
	float lr;
	int i;

	if(light->shadbrush)
	{
		V3F_CONSTSUB(lbxgl_cam->org, 16, vmins);
		V3F_CONSTADD(lbxgl_cam->org, 16, vmaxs);
		lr=LBXGL_Shadow_LightGammaEffectRadius(light, 0.05);

		world->shadowcam=0;

		lst=NULL;
		for(i=0; light->shadbrush[i]; i++)
		{
			cur=light->shadbrush[i];

			if(cur->flags&(LBXGL_BRFL_NOSHADOW|
					LBXGL_BRFL_EFFECT|LBXGL_BRFL_PTLIGHT|
					LBXGL_BRFL_ALPHA))
				continue;

			if(!LBXGL_BrushWorld_CheckBrushShadowVisible(
					world, cur, light))
				continue;

			cur->flags&=~LBXGL_BRFL_VSHCHK;
			if(LBXGL_Brush_CheckBoxBrushShadow(cur,
					vmins, vmaxs, light->org, lr))
				{ world->shadowcam=1; cur->flags|=LBXGL_BRFL_VSHCHK; }

			cur->chain=lst;
			lst=cur;
		}

		return(lst);
	}

	lst=LBXGL_BrushWorld_QueryBrushesLight(world, world->brush, light);
	return(lst);
}

LBXGL_Brush *LBXGL_BrushWorld_QueryBrushesLight2B(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *lst;
	int i, j;

	if(light->litbrush)
	{
		lst=NULL;
		for(i=0; light->litbrush[i]; i++)
		{
			cur=light->litbrush[i];

#if 1
			if(cur->node)
			{
				j=cur->node->id;
				j=world->autovis[j>>3]&(1<<(j&7));
				if(!j) continue;
			}
#endif

			if(cur->flags&(LBXGL_BRFL_EFFECT|LBXGL_BRFL_PTLIGHT|
					LBXGL_BRFL_VISCLIP))
				continue;
			if(!LBXGL_Brush_CheckBrushFrustum(cur))
				continue;

			cur->chain=lst;
			lst=cur;
		}
		return(lst);
	}

	lst=LBXGL_BrushWorld_QueryBrushesLight2(world, world->brush, light);
	return(lst);
}

void LBXGL_BrushWorld_UpdateLightBrushes(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_Brush *arra[256];
	LBXGL_Brush *arrb[256];
	LBXGL_Brush *arrc[256];

	LBXGL_Brush *lsta, *lstb;
	LBXGL_Brush *cura, *curb;
	LBXGL_Brush *nxta, *nxtb;
	float d;
	int i, j, na, nb, nc;

	return;

//	if(light->litbrush &&
//		(light->flags&LBXGL_LFL_STATIC) &&
//		!(light->flags&LBXGL_LFL_CHANGED))
//			return;

#if 0
	if(!light->litbrush)
	{
		i=16;
		light->litbrush=gcalloc((i+1)*sizeof(LBXGL_Brush *));
		light->shadbrush=gcalloc((i+1)*sizeof(LBXGL_Brush *));
//		light->litmdl=gcalloc((i+1)*sizeof(LBXGL_ModelState *));
//		light->shadmdl=gcalloc((i+1)*sizeof(LBXGL_ModelState *));
		light->sz_litbrush=i;
		light->sz_shadbrush=i;
//		light->sz_litmdl=i;
//		light->sz_shadmdl=i;
	}
#endif

	lsta=LBXGL_BrushWorld_QueryBrushesLight2(
		world, world->brush, light);

//	lsta=LBXGL_BrushWorld_QueryBrushesLight(
//		world, world->brush, light);

#if 0
	lsta=NULL;
	if(light->flags&LBXGL_LFL_STATIC)
	{
		lsta=LBXGL_BrushWorld_QueryBrushesLightStatic(world, light);
	}else
	{
		d=LBXGL_Shadow_LightGammaEffectRadius(light, 0.01);
		lsta=LBXGL_BrushBSP_QueryTreeNode(world->bsp, light->org, d);
	}
#endif

	if(!lsta)
	{
		if(light->litbrush)
			light->litbrush[0]=NULL;
		if(light->shadbrush)
			light->shadbrush[0]=NULL;
//		light->litmdl[0]=NULL;
//		light->shadmdl[0]=NULL;
		return;
	}

	lstb=LBXGL_BrushWorld_QueryBrushesLight(
		world, world->brush, light);
//	lstb=lsta;

	cura=lsta; na=0; nc=0;
	while(cura)
	{
		nxta=cura->chain;

		if(cura->flags&LBXGL_BRFL_EFFECT)
			{ cura=nxta; continue; }

		curb=lstb; nb=0;
		while(curb)
		{
			nxtb=curb->chain;

			if(curb->flags&(LBXGL_BRFL_NOSHADOW|
					LBXGL_BRFL_EFFECT|LBXGL_BRFL_ALPHA))
				{ curb=nxtb; continue; }

			if(curb==cura)
				{ curb=nxtb; continue; }

			//skip intersecting brushes
			if(SolidAABB_BoxCollideP(
					cura->mins, cura->maxs,
					curb->mins, curb->maxs))
				{ curb=nxtb; continue; }

//			i=LBXGL_Brush_CheckBBoxBrushVolume(curb,
//				cura->mins, cura->maxs, light->org);
			i=LBXGL_Brush_CheckBrushBrushVolume(
				cura, curb, light->org);
			
			//break if completely inside
			if(i==1)break;
			
			//partial occlusion
			if(i==3)
				{ arrb[nb++]=curb; }
			curb=nxtb;
		}

		//complete occlusion
		if(curb)
			{ cura=nxta; continue; }

		//add brush to first array
		arra[na++]=cura;
		
		//add any occluders
		for(i=0; i<nb; i++)
		{
			for(j=0; j<nc; j++)
				if(arrc[j]==arrb[i])
					break;
			if(j>=nc)
				{ arrc[nc++]=arrb[i]; }
		}

		cura=nxta;
	}
	
	if(!light->litbrush || (na>light->sz_litbrush))
	{
		light->litbrush=gcrealloc(
			light->litbrush, (na+1)*sizeof(LBXGL_Brush *));
		light->sz_litbrush=na;
	}

	if(!light->shadbrush || (nc>light->sz_shadbrush))
	{
		light->shadbrush=gcrealloc(
			light->shadbrush, (nc+1)*sizeof(LBXGL_Brush *));
		light->sz_shadbrush=nc;
	}
	
	for(i=0; i<na; i++)
		light->litbrush[i]=arra[i];
	for(i=0; i<nc; i++)
		light->shadbrush[i]=arrc[i];
	light->litbrush[na]=NULL;
	light->shadbrush[nc]=NULL;
}

void LBXGL_BrushWorld_DrawBrushesLightVol(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_Brush *cur, *nxt, *lst;
	float tv[3];
	float f, g, h, d;
	int i;

	if(!(light->flags&LBXGL_LFL_LIGHTVOL))
		return;

	LBXGL_BrushWorld_DrawSetupLightScale(world, light, 0.25, 0);

//	glDisable(GL_CULL_FACE);
	glDisable(GL_STENCIL_TEST);

	pdglDisableTexture2D();
	LBXGL_Shader_Color4f(0, 1, 1, 0.1);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);

	glDepthMask(0);
//	glColorMask(0, 0, 0, 0);

	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

//	d=LBXGL_Shadow_LightGammaEffectRadius(light, 0.01);

	d=light->curval*8;
	lst=LBXGL_BrushBSP_QueryTreeNode(world->bsp, light->org, d);

	cur=lst;
	while(cur)
	{
		i=cur->flags&LBXGL_TXFL_EFFECT_MASK;
		if(i!=LBXGL_TXFL_EFFECT_LVOL) { cur=cur->chain; continue; }

		if(!LBXGL_Brush_CheckBrushShadowFrustum(cur, light->org))
			{ cur=cur->chain; continue; }

		V3F_SUB(light->org, cur->org, tv);
		V3F_NORMALIZE(tv, tv);
		pdglNormal3fv(tv);

		f=LBXGL_Texture_GetImageAlpha(cur->pf_tex[0]);
//		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, light->curval*f);
		PDGL_Light0_ConstantAttenuation(light->curval*f);
		LBXGL_Brush_DrawShadowVolume(cur, light);

		cur=cur->chain;
	}

//	LBXGL_BrushWorld_DrawModelsLight(world, light);

	glDepthMask(1);

	PDGL_UnbindShader();
//	glDisable(GL_LIGHTING);
	pdglDisableLighting();
}

void LBXGL_BrushWorld_DrawModelsLight(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
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
		if(!LBXGL_Mdl_CheckModelLight(mcur, light))
			{ mcur=mcur->next; i++; continue; }
		if(mcur->tmp_drawface)
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelLight(mcur, light);
		mcur=mcur->next; i++;
	}
}

void LBXGL_BrushWorld_DrawModelsShadow(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	float d;
	int i, j;

//	return;

	d=LBXGL_Shadow_LightGammaRadius(light);

	mcur=world->mesh; i=1;
	while(mcur)
	{
//		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
//			{ mcur=mcur->next; i++; continue; }
		if(!LBXGL_Mdl_CheckModelLight(mcur, light))
			{ mcur=mcur->next; i++; continue; }
		if(mcur->flags&
				(LBXGL_TXFL_ALPHA|LBXGL_TXFL_NODRAW|
				 LBXGL_TXFL_EFFECT|LBXGL_TXFL_NOSHADOW))
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
		j=LBXGL_Brush_CheckBBoxShadowFrustumVisible(
			world, tv0, tv1, light->org, d);
		if(!j)
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelShadow(mcur, light, light->org, d);
		mcur=mcur->next; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelsDark(LBXGL_BrushWorld *world)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }

		if(mcur->tmp_drawface)
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelDark(mcur);
		mcur=mcur->next; i++;
	}

//	Con_Printf("Draw %d meshes\n", i);
}

LBXGL_API void LBXGL_BrushWorld_DrawModelsFinal(LBXGL_BrushWorld *world)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int t0, t1;
	int i;

//	t0=PDGL_TimeMS();

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }

		if(mcur->tmp_drawface)
			{ mcur=mcur->next; i++; continue; }

		LBXGL_Mdl_DrawModelFinal(mcur);
		mcur=mcur->next; i++;

//		t1=PDGL_TimeMS();
//		if((t1-t0)>15)break;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelsAlpha(LBXGL_BrushWorld *world)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	LBXGL_Shader_Color4f(1, 1, 1, 1);

	mcur=world->mesh; i=1;
	while(mcur)
	{
#if 1
		if(!(mcur->effects&BT_EFF_BEAM))
		{
			LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
//			if(!LBXGL_Brush_CheckBBoxFrustum(tv0, tv1))
//				{ mcur=mcur->next; i++; continue; }
			if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
				{ mcur=mcur->next; i++; continue; }
		}
#endif

		LBXGL_Mdl_DrawModelAlpha(mcur);
		mcur=mcur->next; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelsFlat(LBXGL_BrushWorld *world)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }
		LBXGL_Mdl_DrawModelFlat(mcur);
		mcur=mcur->next; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelsPointLight(
	LBXGL_BrushWorld *world)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	LBXGL_Light *lcur;
	float f, g;
	int i, j;

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }

//		f=LBXGL_Shadow_LightGammaPoint(cur, mcur->org, 32);

		lcur=LBXGL_BrushWorld_QueryLightsSphere(world, mcur->org, 16);
		g=0; j=16;
		while(lcur && j>0)
		{
//			f=LBXGL_Shadow_LightGammaPoint(lcur, mcur->org, 16);
			f=lcur->tval;
//			if(f<0.01)break;
			g+=f;
			lcur=lcur->chain;
		}

		LBXGL_Shader_Color4f(g, g, g, 1);

		LBXGL_Mdl_DrawModelFlat(mcur);
		mcur=mcur->next; i++;
	}
	LBXGL_Shader_Color4f(1, 1, 1, 1);
}

LBXGL_API void LBXGL_BrushWorld_DrawModels(LBXGL_BrushWorld *world)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=world->mesh; i=1;
	while(mcur)
	{
		if(!(world->mdlvis[i>>3]&(1<<(i&7))))
			{ mcur=mcur->next; i++; continue; }
		LBXGL_Mdl_DrawModel(mcur);
		mcur=mcur->next; i++;
	}
}

#if 1
void LBXGL_BrushWorld_DrawModelListLight(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh, LBXGL_Light *light)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i, j;

	mcur=mesh; i=1;
	while(mcur)
	{
		if(!LBXGL_Mdl_CheckModelLight(mcur, light))
			{ mcur=mcur->chain; i++; continue; }
		if(mcur->tmp_drawface)
			{ mcur=mcur->chain; i++; continue; }

		LBXGL_Mdl_DrawModelLight(mcur, light);
		mcur=mcur->chain; i++;
	}
}

void LBXGL_BrushWorld_DrawModelListShadow(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh, LBXGL_Light *light)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	float d;
	int i, j;

	d=LBXGL_Shadow_LightGammaRadius(light);

	mcur=mesh; i=1;
	while(mcur)
	{
		if(!LBXGL_Mdl_CheckModelLight(mcur, light))
			{ mcur=mcur->chain; i++; continue; }
		if(mcur->flags&
				(LBXGL_TXFL_ALPHA|LBXGL_TXFL_NODRAW|
				 LBXGL_TXFL_EFFECT|LBXGL_TXFL_NOSHADOW))
			{ mcur=mcur->chain; i++; continue; }

		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
		j=LBXGL_Brush_CheckBBoxShadowFrustumVisible(
			world, tv0, tv1, light->org, d);
		if(!j)
			{ mcur=mcur->chain; i++; continue; }

		LBXGL_Mdl_DrawModelShadow(mcur, light, light->org, d);
		mcur=mcur->chain; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelListDark(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=mesh; i=1;
	while(mcur)
	{
		if(mcur->tmp_drawface)
			{ mcur=mcur->chain; i++; continue; }

		LBXGL_Mdl_DrawModelDark(mcur);
		mcur=mcur->chain; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelListFinal(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int t0, t1;
	int i;

	mcur=mesh; i=1;
	while(mcur)
	{
		if(mcur->tmp_drawface)
			{ mcur=mcur->chain; i++; continue; }

		LBXGL_Mdl_DrawModelFinal(mcur);
		mcur=mcur->chain; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelListAlpha(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=mesh; i=1;
	while(mcur)
	{
#if 1
		LBXGL_Mdl_BoxModel(mcur, tv0, tv1);
		if(!LBXGL_Brush_CheckBBoxFrustumVisible(world, tv0, tv1))
			{ mcur=mcur->chain; i++; continue; }
#endif

		LBXGL_Mdl_DrawModelAlpha(mcur);
		mcur=mcur->chain; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelListFlat(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=mesh; i=1;
	while(mcur)
	{
		LBXGL_Mdl_DrawModelFlat(mcur);
		mcur=mcur->chain; i++;
	}
}

LBXGL_API void LBXGL_BrushWorld_DrawModelListPointLight(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	LBXGL_Light *lcur;
	float f, g;
	int i, j;

	mcur=mesh; i=1;
	while(mcur)
	{
		lcur=LBXGL_BrushWorld_QueryLightsSphere(world, mcur->org, 16);
		g=0; j=16;
		while(lcur && j>0)
		{
//			f=LBXGL_Shadow_LightGammaPoint(lcur, mcur->org, 16);
			f=lcur->tval;
//			if(f<0.01)break;
			g+=f;
			lcur=lcur->chain;
		}

		LBXGL_Shader_Color4f(g, g, g, 1);

		LBXGL_Mdl_DrawModelFlat(mcur);
		mcur=mcur->chain; i++;
	}
	LBXGL_Shader_Color4f(1, 1, 1, 1);
}

LBXGL_API void LBXGL_BrushWorld_DrawModelList(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mesh)
{
	float tv0[3], tv1[3];
	LBXGL_ModelState *mcur;
	int i;

	mcur=mesh; i=1;
	while(mcur)
	{
		LBXGL_Mdl_DrawModel(mcur);
		mcur=mcur->chain; i++;
	}
}
#endif

#if 0
// void LBXGL_BrushWorld_DrawShadowsLightDF(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst,
	LBXGL_Light *light, int flags)
{
	LBXGL_Brush *cur;
	float f, g, d;

	glDisable(GL_TEXTURE_2D);
	LBXGL_Shader_Color4f(0, 1, 1, 0.1);

//	LBXGL_BrushWorld_DrawModelsShadow(world, light);

	glDepthFunc(GL_LESS);

	glDepthMask(0);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
	glStencilFunc(GL_ALWAYS, 0, 255);


	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_INCR_WRAP, GL_KEEP);

	cur=fst;
	while(cur)
	{
		LBXGL_Brush_DrawShadowVolume(cur, light);
		cur=cur->chain;
	}

	LBXGL_BrushWorld_DrawModelsShadow(world, light);

	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_DECR, GL_KEEP);

	cur=fst;
	while(cur)
	{
		LBXGL_Brush_DrawShadowVolume(cur, light);
		cur=cur->chain;
	}

	LBXGL_BrushWorld_DrawModelsShadow(world, light);

	glDisable(GL_CULL_FACE);

	glDepthMask(1);
	glColorMask(1, 1, 1, 1);
}
#endif

void LBXGL_BrushWorld_DrawShadowsLightDP(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst,
	LBXGL_Light *light, int flags)
{
	LBXGL_Brush *cur;
	float f, g, d;

	pdglDisableTexture2D();
	LBXGL_Shader_Color4f(0, 1, 1, 0.5);

#if 0
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
//	glDisable(GL_CULL_FACE);

	glDepthMask(0);

	glCullFace(GL_BACK);
	LBXGL_Shader_Color4f(0, 1, 1, 0.5);

	cur=fst;
	while(cur)
	{
		LBXGL_Shader_Color4f(0, 1, 1, 0.5);
		if(cur->flags&LBXGL_BRFL_VSHCHK)
			LBXGL_Shader_Color4f(1, 0, 1, 0.5);

		LBXGL_Brush_DrawShadowVolumeDP(cur, light);
		cur=cur->chain;
	}
	
	glDepthMask(1);

	return;
#endif

	glDepthFunc(GL_LESS);

	glDepthMask(0);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
	glStencilFunc(GL_ALWAYS, 0, 255);


	glCullFace(GL_BACK);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	if(!(light->flags&LBXGL_LFL_SHADOWMAP))
	{
		cur=fst;
		while(cur)
		{
			if(cur->flags&LBXGL_BRFL_VSHCHK)
			{
				LBXGL_Brush_DrawShadowVolumeDPClip(cur, light);
			}else
			{
				LBXGL_Brush_DrawShadowVolumeDP3(
					cur, light, LBXGL_SFL_NOENDCAPS);
			}

			cur=cur->chain;
		}
	}
	
	LBXGL_Voxel_DrawWorldShadows(world, light);
	LBXGL_BrushWorld_DrawModelsShadow(world, light);

	glCullFace(GL_FRONT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

	cur=fst;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_VSHCHK)
		{
			LBXGL_Brush_DrawShadowVolumeDPClip(cur, light);
		}else
		{
			LBXGL_Brush_DrawShadowVolumeDP3(
				cur, light, LBXGL_SFL_NOENDCAPS);
		}

		cur=cur->chain;
	}

	LBXGL_Voxel_DrawWorldShadows(world, light);
	LBXGL_BrushWorld_DrawModelsShadow(world, light);

	glDisable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glDepthMask(1);
	glColorMask(1, 1, 1, 1);
}

void LBXGL_BrushWorld_DrawShadowsLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst,
	LBXGL_Light *light, int flags)
{
	if(world->shadowcam==2)
		LBXGL_BrushWorld_DrawShadowsLightDP(world, fst, light, flags);
	if(world->shadowcam==1)
//		LBXGL_BrushWorld_DrawShadowsLightDF(world, fst, light, flags);
		LBXGL_BrushWorld_DrawShadowsLightDP(world, fst, light, flags);
	if(world->shadowcam==0)
		LBXGL_BrushWorld_DrawShadowsLightDP(world, fst, light, flags);
}

float LBXGL_BrushWorld_SetupLightFrame(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	static char *lightstyle[]=
	{
	// 0, normal
		"m",	
	// 1, flicker
		"mmnmmommommnonmmonqnmmo", 
	// 2, slow strong pulse
		"abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba",
	// 3, candle 1
		"mmmmmaaaaammmmmaaaaaabcdefgabcdefg",
	// 4, fast strobe
		"mamamamamama",
	// 5, gentle pulse
		"jklmnopqrstuvwxyzyxwvutsrqponmlkj",
	// 6, alternate flicker
		"nmonqnmomnmomomno",
	// 7, candle 2
		"mmmaaaabcdefgmmmmaaaammmaamm",
	// 8, candle 3
		"mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa",
	// 9, slow strobe
		"aaaaaaaazzzzzzzz",
	// 10, flourescent flicker
		"mmamammmmammamamaaamammma",
	// 11, slow pulse absent black
		"abcdefghijklmnopqrrqponmlkjihgfedcba"
	};

	float tv[4];
	float f, g, h, d, gam, ang;
	char *s;
	int i, j;

	gam=light->val[0]; f=lbxgl_state->time_f;

	if((light->style>0) && (light->style<=11))
	{
		s=lightstyle[light->style];
		i=((int)(lbxgl_state->time_f*10))%strlen(s);
		gam*=(s[i]-'a')*(1.0/('m'-'a'));
	}

	if(light->flags&LBXGL_LFL_SUN)
	{
		ang=2*M_PI*world->time_daynight*(1.0/(24*60));
	
		f=cos(ang);
//		f=cos(2*M_PI*world->time_daynight*(1.0/(24*60)));
//		f=cos(2*M_PI*lbxgl_state->time_f*(1.0/(24*60)));
//		f=cos(2*M_PI*lbxgl_state->time_f*(1.0/(12*60)));

		g=btCvarGetf("r_forcetime");
		if(g!=0.0)
		{
			f=g;
			ang=acos(g);
		}

		LBXGL_SkyCoulds_SetEnabled(true);
		LBXGL_SkyCoulds_SetSunAngle(ang);

		f=f+0.25;
		if(f>1)f=1;
		if(f<0)f=0;
//		g=f*0.75+0.25;
		g=f*0.65+0.35;
//		gam=gam*g;
//		light->cur_mcsc=f;
		light->cur_mcsc=g;
		
		tv[0]=f; tv[1]=1.0-f;
		tv[2]=1.0-(2.0*fabs(f-0.5));
		tv[3]=0;
		LBXGL_Sky_SetAlphas(tv, 4);
		
		LBXGL_Voxel_GetPointSunLightColor(world,
			lbxgl_cam->org, tv);
		
//		V3F_SCALE(tv, g, tv);
		
		g=1.25-fabs(0.75-f); g=g*0.5+0.5;
		h=0.5*(0.5-f);
		if(h<0)h=0;
		tv[0]*=g;
		tv[1]*=f*0.65+0.35;
		tv[2]*=1.0+h;
		tv[3]=1.0;
//		V3F_NORMALIZE(tv, tv);
		
//		LBXGL_SkyCoulds_SetColor(tv);
		
		V3F_COPY(tv, light->clr);
//		light->clr[0]=tv[0]*light->clr[0];
//		light->clr[1]=tv[1]*light->clr[1];
//		light->clr[2]=tv[2]*light->clr[2];

		g=f*0.85+0.15;
//		V3F_SCALE(tv, g*g, tv);
		V3F_SCALE(tv, g*g*g*g, tv);
		LBXGL_SkyCoulds_SetColor(tv);
	}else
	{
		light->cur_mcsc=1;
	}

	if(!(light->flags&LBXGL_LFL_BOXCULL))
	{
		V3F_CONST(light->mins, -gam);
		V3F_CONST(light->maxs,  gam);
	}

	if(light->flags&LBXGL_LFL_CAMREL)
	{
//		V3F_ADD(light->rel_org, lbxgl_cam->org, light->org);

		V3F_SCALE(lbxgl_cam->rt, light->rel_dir[0], light->dir);
		V3F_ADDSCALE(light->dir,
			lbxgl_cam->fw, light->rel_dir[1], light->dir);
		V3F_ADDSCALE(light->dir,
			lbxgl_cam->up, light->rel_dir[2], light->dir);
	}
	
	V3F_ADD(light->org, light->mins, light->amins);
	V3F_ADD(light->org, light->maxs, light->amaxs);

//	light->curval=gam;
	light->curval=gam*world->curgamma;
	return(0);
}

float LBXGL_BrushWorld_DrawSetupLight(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	return(LBXGL_BrushWorld_DrawSetupLightScale(
		world, light, 1.0, 0));
}

float LBXGL_BrushWorld_DrawSetupHiLight(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	if((shader_phong<0) || shader_nophong)
		return(0);

	return(LBXGL_BrushWorld_DrawSetupLightScale(
		world, light, 1.0, LBXGL_LFL_DETAIL));
}

float LBXGL_BrushWorld_DrawSetupLightScale(
	LBXGL_BrushWorld *world, LBXGL_Light *light,
	float gscale, int flags)
{
	float pt[4];
	float f, g, h, d, gam, gam2;
	int i;

	i=glGetError();
	while(i!=GL_NO_ERROR)
	{
		printf("LBXGL_BrushWorld_DrawSetupLightScale(A): Error 0x%08X\n", i);
		i=glGetError();
	}

	if((light->flags&LBXGL_LFL_STATIC) &&
		(light->flags&LBXGL_LFL_CHANGED) &&
		!(world->flags&BTGE_WFL_OVERBUDGET))

//	if((light->flags&LBXGL_LFL_STATIC) &&
//		(light->flags&LBXGL_LFL_CHANGED))
	{
		LBXGL_BrushWorld_FlushLightStatic(world, light);
		light->flags&=~LBXGL_LFL_CHANGED;
	}

	LBXGL_Shader_SetCurrentLight(light);
	if((shader_phong<0) || shader_nophong)
	{
		f=LBXGL_BrushWorld_DrawSetupLightScale_NoPhong(
			world, light, gscale);
		return(f);
	}

//	gam=light->curval*gscale*world->curgamma;
	gam=light->curval*gscale;
	gam2=light->val[1]*gscale;

	if(gam<0)gam=0;
	if(gam2<0)gam2=0;

#ifndef GLES
	glDisable(GL_COLOR_MATERIAL);
#endif

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pt);
	PDGL_ColorMaterial_Ambient(pt);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);
	PDGL_ColorMaterial_Diffuse(pt);

	pt[0]=0.2; pt[1]=0.2; pt[2]=0.2; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);
	PDGL_ColorMaterial_Specular(pt);

	pt[0]=0; pt[1]=0; pt[2]=0; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pt);
	PDGL_ColorMaterial_Emission(pt);

	pt[0]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, pt);
	PDGL_ColorMaterial_Shininess(pt);


//	glEnable(GL_LIGHTING);
	pdglEnableLighting();

//	glEnable(GL_NORMALIZE);
//	glDisable(GL_NORMALIZE);

#ifndef GLES
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glShadeModel(GL_SMOOTH);
#endif

	if(flags&LBXGL_LFL_DETAIL)
	{
		gam=gam2;

		V3F_SCALE(light->clr, light->coeff[0], pt); pt[3]=1;
//		glLightfv(GL_LIGHT0, GL_AMBIENT, pt);
		PDGL_Light0_Ambient(pt);

		V3F_SCALE(light->clr, light->coeff[1], pt); pt[3]=1;
//		glLightfv(GL_LIGHT0, GL_DIFFUSE, pt);
		PDGL_Light0_Diffuse(pt);

		V3F_SCALE(light->clr, light->coeff[2], pt); pt[3]=1;
//		glLightfv(GL_LIGHT0, GL_SPECULAR, pt);
		PDGL_Light0_Specular(pt);
	}else
	{
		f=light->cur_mcsc;
	
		V3F_SCALE(light->clr, light->coeff[0]*f, pt); pt[3]=1;
//		glLightfv(GL_LIGHT0, GL_AMBIENT, pt);
		PDGL_Light0_Ambient(pt);

		V3F_SCALE(light->clr, light->coeff[1]*f, pt); pt[3]=1;
//		glLightfv(GL_LIGHT0, GL_DIFFUSE, pt);
		PDGL_Light0_Diffuse(pt);

		V3F_SCALE(light->clr, light->coeff[2]*f, pt); pt[3]=1;
//		glLightfv(GL_LIGHT0, GL_SPECULAR, pt);
		PDGL_Light0_Specular(pt);
	}

#if 0
//	V3F_SCALE(light->clr, 0.50/0.58, pt); pt[3]=1;
	V3F_SCALE(light->clr, light->coeff[0], pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_AMBIENT, pt);

//	V3F_SCALE(light->clr, 0.50/0.58, pt); pt[3]=1;
	V3F_SCALE(light->clr, light->coeff[1], pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, pt);

//	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	V3F_SCALE(light->clr, light->coeff[2], pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_SPECULAR, pt);
#endif

	V3F_COPY(light->org, pt); pt[3]=1;
//	glLightfv(GL_LIGHT0, GL_POSITION, pt);
	PDGL_Light0_Position(pt);

	if(0 && (light->flags&LBXGL_LFL_BOXCULL))
	{
		PDGL_BindShader(shader_phong2_box);
		PDGL_Uniform3fv("light_mins", 1, light->amins);
		PDGL_Uniform3fv("light_maxs", 1, light->amaxs);
		PDGL_Uniform3fv("light_value", 1, light->boxval);

		pt[0]=0; pt[1]=0; pt[2]=-1; pt[3]=0;
//		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);
		PDGL_Light0_SpotDirection(pt);
//		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
//		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);
		PDGL_Light0_SpotCutoff(180);
		PDGL_Light0_SpotExponent(0);

		PDGL_Uniform1i("texIsNorm", 0);
		PDGL_Uniform1i("lightIsBox", 1);
	}else if(light->flags&LBXGL_LFL_SPOTLIGHT)
	{
		PDGL_BindShader(shader_phong_spot);

		V3F_COPY(light->dir, pt); pt[3]=0;
//		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);
		PDGL_Light0_SpotDirection(pt);
//		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);
		PDGL_Light0_SpotExponent(0);

		f=light->angle;
		if(f==0)f=15;
//		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, f);
		PDGL_Light0_SpotCutoff(f);
	}else
	{
		if(light->flags&LBXGL_LFL_LINEAR)
			PDGL_BindShader(shader_phong);
			else PDGL_BindShader(shader_phong2);

		pt[0]=0; pt[1]=0; pt[2]=-1; pt[3]=0;
//		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);
//		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
//		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);

		PDGL_Light0_SpotDirection(pt);
		PDGL_Light0_SpotCutoff(180);
		PDGL_Light0_SpotExponent(0);

//		PDGL_Uniform1i("envShadowMap", 0);
//		PDGL_Uniform3fv("envOrigin", 1, light->org);
		PDGL_Uniform3fv("envOrigin", 1, light->env_org);

//		PDGL_Uniform1i("texIsNorm", 0);
//		PDGL_Uniform1i("lightIsBox", 0);
	}


	if(light->flags&LBXGL_LFL_LINEAR)
	{
//		g=36*6.0/255.0;
		g=36*12.0/255.0;
		f=g*gam;
//		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0/f);
//		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
//		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);

		PDGL_Light0_LinearAttenuation(1.0/f);
		PDGL_Light0_QuadraticAttenuation(0.0);
		PDGL_Light0_ConstantAttenuation(0.0);
	}else
	{
#if 0
//		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, gam/0.58);
//		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, gam/(256*0.58));
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, gam);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0);
//		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, gam2);
#endif

		PDGL_Light0_LinearAttenuation(0);
		PDGL_Light0_QuadraticAttenuation(gam2);
		PDGL_Light0_ConstantAttenuation(gam);
	}
//	glEnable(GL_LIGHT0);
	PDGL_Enable_Light0();

	i=glGetError();
	while(i!=GL_NO_ERROR)
	{
		printf("LBXGL_BrushWorld_DrawSetupLightScale(B): Error 0x%08X\n", i);
		i=glGetError();
	}

	LBXGL_BrushLight_DrawSetupLightImage(light, NULL);

	i=glGetError();
	while(i!=GL_NO_ERROR)
	{
		printf("LBXGL_BrushWorld_DrawSetupLightScale(C): Error 0x%08X\n", i);
		i=glGetError();
	}

	return(gam);
}

float LBXGL_BrushWorld_DrawSetupLightScale_NoPhong(
	LBXGL_BrushWorld *world, LBXGL_Light *light, float gscale)
{
	float pt[4];
	float f, g, h, d, gam;

#ifndef GLES

//	gam=light->curval*gscale*world->curgamma;
	gam=light->curval*gscale;

	glDisable(GL_COLOR_MATERIAL);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pt);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);

	pt[0]=0.2; pt[1]=0.2; pt[2]=0.2; pt[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);

	pt[0]=0; pt[1]=0; pt[2]=0; pt[3]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pt);

	pt[0]=1;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, pt);


	glEnable(GL_LIGHTING);
//	glEnable(GL_NORMALIZE);
	glDisable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glShadeModel(GL_SMOOTH);

	V3F_SCALE(light->clr, 0.50/0.58, pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_AMBIENT, pt);

	V3F_SCALE(light->clr, 0.50/0.58, pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_DIFFUSE, pt);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
	glLightfv(GL_LIGHT0, GL_SPECULAR, pt);

	V3F_COPY(light->org, pt); pt[3]=1;
	glLightfv(GL_LIGHT0, GL_POSITION, pt);

	if(light->flags&LBXGL_LFL_SPOTLIGHT)
	{
		V3F_COPY(light->dir, pt); pt[3]=0;
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);

		f=light->angle;
		if(f==0)f=15;
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, f);
	}else
	{
		pt[0]=0; pt[1]=0; pt[2]=-1; pt[3]=0;
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);
		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
		glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0);
	}


	if(light->flags&LBXGL_LFL_LINEAR)
	{
//		g=36*6.0/255.0;
		g=36*12.0/255.0;
		f=g*gam;
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1.0/f);
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
	}else
	{
#if 1
		// -0.042378 0.045295 0.083003 -0.021884
		f=1.0/gam;
		glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.083*f);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0453*f);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, -0.0424*f);
#endif
	}
	glEnable(GL_LIGHT0);

	return(gam);
#endif //GLES
}

/** Reset lighting to defaults after drawing an "advanced" material */
void LBXGL_BrushLight_DrawPrimLight_Reset(LBXGL_Light *light)
{
	float pt[4];

	if((lbxgl_lightimg_texnum<=0) && (lbxgl_lightimg_light==light))
		return;

//	if((lbxgl_lightimg_layers>0) || (lbxgl_lightimg_light!=light))
	if(lbxgl_lightimg_layers>0)
	{
		LBXGL_BrushLight_DrawTeardownLightImage(light, NULL);
//		lbxgl_lightimg_texnum=0;
//		lbxgl_lightimg_layers=0;
		lbxgl_lightimg_layers=lbxgl_lightimg_baselayers;
	}
	
	//check if unnecessary
	if(lbxgl_lightimg_texnum<=0)
		return;
	lbxgl_lightimg_texnum=0;

//	glDisable(GL_COLOR_MATERIAL);

	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, pt);
	PDGL_ColorMaterial_Ambient(pt);
	pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);
	PDGL_ColorMaterial_Diffuse(pt);
	pt[0]=0.2; pt[1]=0.2; pt[2]=0.2; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);
	PDGL_ColorMaterial_Specular(pt);
	pt[0]=0; pt[1]=0; pt[2]=0; pt[3]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, pt);
	PDGL_ColorMaterial_Emission(pt);
	pt[0]=1;
//	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, pt);
	PDGL_ColorMaterial_Shininess(pt);

	if(!(shader_phong<=0) && !shader_nophong)
	{
		//reset to default light shader
		if(0 && (light->flags&LBXGL_LFL_BOXCULL))
		{
			PDGL_BindShader(shader_phong2_box);
			PDGL_Uniform3fv("light_mins", 1, light->amins);
			PDGL_Uniform3fv("light_maxs", 1, light->amaxs);
			PDGL_Uniform3fv("light_value", 1, light->boxval);

//			PDGL_Uniform1i("envShadowMap", 0);
//			PDGL_Uniform1i("lightIsBox", 1);
//			PDGL_Uniform1i("texIsNorm", 0);
//			PDGL_Uniform3fv("envOrigin", 1, light->org);

		}else if(light->flags&LBXGL_LFL_SPOTLIGHT)
		{
			PDGL_BindShader(shader_phong_spot);
//			PDGL_Uniform1i("envShadowMap", 0);
//			PDGL_Uniform1i("lightIsBox", 0);
//			PDGL_Uniform1i("texIsNorm", 0);
		}else
		{
			if(light->flags&LBXGL_LFL_LINEAR)
				PDGL_BindShader(shader_phong);
				else PDGL_BindShader(shader_phong2);

//			PDGL_Uniform1i("envShadowMap", 0);
//			PDGL_Uniform3fv("envOrigin", 1, light->org);
//			PDGL_Uniform1i("lightIsBox", 0);
//			PDGL_Uniform1i("texIsNorm", 0);
		}
	}
	
	LBXGL_BrushLight_DrawSetupLightImage(light, NULL);
}

/** Setup for drawing a particular light with a particular texture */
int LBXGL_BrushLight_DrawSetupLightImage(
	LBXGL_Light *light, LBXGL_TexImage *img)
{
	float pt[4], sdir[4], tdir[4];
	float znear, zfar;
	int i, l, tn;

	i=glGetError();
	while(i!=GL_NO_ERROR)
	{
//		printf("LBXGL_BrushLight_DrawSetupLightImage(A): Error 0x%08X\n", i);
		i=glGetError();
	}

//	znear=1.0/tan(90*(M_PI/360.0));
//	zfar=100000;
	UI_Camera_GetZNearFar(&znear, &zfar);

	if(lbxgl_lightimg_light!=light)
	{
		LBXGL_BrushLight_DrawTeardownLightImage(NULL, NULL);
	}

	l=lbxgl_lightimg_baselayers;
	if(img)
	{
		if(lbxgl_lightimg_layers)
		{
			if((img->num==lbxgl_lightimg_texnum) &&
				(lbxgl_lightimg_light==light))
			{
				l=lbxgl_lightimg_layers;
				return(l);
			}
			LBXGL_BrushLight_DrawTeardownLightImage(light, img);
		}
	
//		glEnable(GL_COLOR_MATERIAL);

//		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, img->mat_ambient);
//		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, img->mat_diffuse);
//		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, img->mat_specular);
//		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, img->mat_emission);
//		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, img->mat_shininess);

		PDGL_ColorMaterial_Ambient(img->mat_ambient);
		PDGL_ColorMaterial_Diffuse(img->mat_diffuse);
		PDGL_ColorMaterial_Specular(img->mat_specular);
		PDGL_ColorMaterial_Emission(img->mat_emission);
		PDGL_ColorMaterial_Shininess(img->mat_shininess);

		if(!shader_nophong && (shader_phong2_norm>0) &&
			(img->tex_norm || img->tex_gloss || (light->tex_falloff>0)) &&
			(img->flags&LBXGL_TXFL_LIGHTMAT) &&
			!(light->flags&(LBXGL_LFL_SPOTLIGHT|LBXGL_LFL_LINEAR)) &&
			!(lbxgl_world->flags&BTGE_WFL_LIGHTBUDGET))
		{
			if(0 && (light->flags&LBXGL_LFL_BOXCULL))
			{
				PDGL_BindShader(shader_phong2_nbox);
				PDGL_Uniform3fv("light_mins", 1, light->amins);
				PDGL_Uniform3fv("light_maxs", 1, light->amaxs);
				PDGL_Uniform3fv("light_value", 1, light->boxval);
			}else if(light->flags&LBXGL_LFL_SPOTLIGHT)
			{
//				PDGL_BindShader(shader_phong_spot);
				PDGL_BindShader(shader_phong2_norm);
				PDGL_Uniform1i("texIsNorm", 1);
			}else
			{
//				if(light->flags&LBXGL_LFL_LINEAR)
//					PDGL_BindShader(shader_phong);
//					else
				PDGL_BindShader(shader_phong2_norm);
				PDGL_Uniform1i("texIsNorm", 1);
			}

//			PDGL_BindShader(shader_phong2_norm);

			i=l++;
			tn=img->tex_alt;
			if(tn<=0)tn=img->num;
			pdglActiveTexture(i);
			glBindTexture(GL_TEXTURE_2D, tn);
			glEnable(GL_TEXTURE_2D);
			PDGL_Uniform1i("texBase", i);

			i=l++;
			tn=img->tex_norm;
			if(tn<=0)tn=tex_light_defnorm;
			pdglActiveTexture(i);
			glBindTexture(GL_TEXTURE_2D, tn);
			glEnable(GL_TEXTURE_2D);
			PDGL_Uniform1i("texNorm", i);

			i=l++;
			tn=img->tex_gloss;
			if(tn<=0)tn=tex_light_defgloss;
			pdglActiveTexture(i);
			glBindTexture(GL_TEXTURE_2D, tn);
			glEnable(GL_TEXTURE_2D);
			PDGL_Uniform1i("texGloss", i);

			if(0 && (light->flags&LBXGL_LFL_BOXCULL))
			{
				i=l++;
				tn=light->tex_falloff;
				if(tn<=0)tn=tex_light_def_falloff;
				pdglActiveTexture(i);
				glBindTexture(GL_TEXTURE_2D, tn);
				glEnable(GL_TEXTURE_2D);
				PDGL_Uniform1i("texFalloffXZ", i);

				i=l++;
				tn=light->tex_falloff;
				if(tn<=0)tn=tex_light_def_falloff;
				pdglActiveTexture(i);
				glBindTexture(GL_TEXTURE_2D, tn);
				glEnable(GL_TEXTURE_2D);
				PDGL_Uniform1i("texFalloffYZ", i);
			}

			pdglActiveTexture(0);

			V3F_ZERO(sdir);
			V3F_ZERO(tdir);
//			Hull_AdjacentNormals23(norm, sdir, tdir);
//			PDGL_Uniform3fv("vecSdir", 1, sdir);
//			PDGL_Uniform3fv("vecTdir", 1, tdir);

			PDGL_Uniform1i("texIsUVAY",
				(img->flags&LBXGL_TXFL_COLOR_UVAY)?1:0);
		}else
		{

#if 0
			if(light->flags&LBXGL_LFL_SPOTLIGHT)
			{
//				PDGL_BindShader(shader_phong_spot);
				PDGL_BindShader(shader_phong2);
				PDGL_Uniform1i("texIsNorm", 0);
			}else
			{
//				if(light->flags&LBXGL_LFL_LINEAR)
//					PDGL_BindShader(shader_phong);
//					else
				PDGL_BindShader(shader_phong2);
				PDGL_Uniform1i("texIsNorm", 0);
			}
#endif

//			PDGL_Uniform1i("texIsNorm", 0);
		}
	}else
	{
		if(lbxgl_lightimg_layers)
		{
			if((lbxgl_lightimg_texnum==0) &&
				(lbxgl_lightimg_light==light))
			{
				l=lbxgl_lightimg_layers;
				return(l);
			}
			LBXGL_BrushLight_DrawTeardownLightImage(light, img);
		}

		PDGL_Uniform1i("texIsNorm", 0);

//		LBXGL_BrushLight_DrawPrimLight_Reset(light);
	}

#if 1
	if((light->flags&LBXGL_LFL_SHADOWMAP) &&
		(light->envdepthtex>0))
	{
		i=l++;
		tn=light->envtex;
		if(tn<=0)tn=tex_gloss_cubemap;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tn);
		glEnable(GL_TEXTURE_CUBE_MAP);
		PDGL_Uniform1i("texEnvMap", i);

		i=l++;
		tn=light->envdepthtex;
		if(tn<=0)tn=tex_gloss_cubemap;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tn);
		glEnable(GL_TEXTURE_CUBE_MAP);
		PDGL_Uniform1i("texEnvDepthMap", i);

//		PDGL_Uniform1i("envShadowMap",
//			((light->flags&LBXGL_LFL_ENVRENDER) &&
//			!(light->flags&LBXGL_LFL_NOSHADOW))?1:0);

		PDGL_Uniform1i("envShadowMap",
			((light->flags&LBXGL_LFL_SHADOWMAP) &&
			(light->envdepthtex>0))?1:0);

//		PDGL_Uniform1i("envShadowMap", 0);

		PDGL_Uniform3fv("envOrigin", 1, light->env_org);

//		PDGL_Uniform1f("zNear", 0.1);
//		PDGL_Uniform1f("zFar", 100000.0);
//		PDGL_Uniform1f("zFar", 100000000.0);

		PDGL_Uniform1f("zNear", znear);
		PDGL_Uniform1f("zFar", zfar);

		pdglActiveTexture(0);
	}else
	{
		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_gloss_cubemap);
		glEnable(GL_TEXTURE_CUBE_MAP);
		PDGL_Uniform1i("texEnvMap", i);

		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex_gloss_cubemap);
		glEnable(GL_TEXTURE_CUBE_MAP);
		PDGL_Uniform1i("texEnvDepthMap", i);

		PDGL_Uniform1i("envShadowMap", 0);
		PDGL_Uniform3fv("envOrigin", 1, light->env_org);
		PDGL_Uniform1f("zNear", znear);
		PDGL_Uniform1f("zFar", zfar);

		pdglActiveTexture(0);
	}
#endif

	i=glGetError();
	while(i!=GL_NO_ERROR)
	{
		printf("LBXGL_BrushLight_DrawSetupLightImage(B): Error 0x%08X\n", i);
		i=glGetError();
	}
	
	lbxgl_lightimg_light=light;
	lbxgl_lightimg_texnum=img?img->num:0;
	lbxgl_lightimg_layers=l;
	return(l);
}

/** Tear down lighting texture, but does not reset lighting */
void LBXGL_BrushLight_DrawTeardownLightImage(
	LBXGL_Light *light, LBXGL_TexImage *img)
{
	int i;

	if(!light || (lbxgl_lightimg_light!=light))
//	if(1)
	{
#if 1
		for(i=0; i<16; i++)
		{
			pdglActiveTexture(i);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_CUBE_MAP);
		}
		pdglActiveTexture(0);
#endif

		lbxgl_lightimg_light=light;
		lbxgl_lightimg_texnum=0;
		lbxgl_lightimg_baselayers=0;
		lbxgl_lightimg_layers=0;
		return;
	}

#if 0
	for(i=0; i<16; i++)
	{
		pdglActiveTexture(i);
		glDisable(GL_TEXTURE_2D);
	}
	pdglActiveTexture(0);
#endif

	if(lbxgl_lightimg_layers<=0)
		return;

//	if(!shader_nophong && (shader_phong2_norm>0))
	if(1)
	{
		for(i=lbxgl_lightimg_baselayers; i<lbxgl_lightimg_layers; i++)
		{
			pdglActiveTexture(i);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_CUBE_MAP);
		}
		pdglActiveTexture(0);
		
		lbxgl_lightimg_layers=lbxgl_lightimg_baselayers;
	}
}

void LBXGL_BrushLight_DrawPrimLight(LBXGL_Light *light,
	int prim, float *xyz, float *st, float *norm, int nxyz, int tex)
{
	LBXGL_TexImage *img;
	float pt[4], sdir[4], tdir[4];
	int i, l, tn;

//	l=0;
	img=LBXGL_Texture_LookupNum(tex);
	l=LBXGL_BrushLight_DrawSetupLightImage(light, img);

//	LBXGL_Texture_SetImageMaterial(tex);

	PDGL_DrawPrim_DrawArraysNormalTex(prim, 0, nxyz,
		3, GL_FLOAT, 0, xyz,
		2, GL_FLOAT, 0, st,
		3, GL_FLOAT, 4*sizeof(float), norm);
}

void LBXGL_BrushLight_DrawPrimLightImage2(
	LBXGL_TexImage *img, LBXGL_Light *light,
	int prim, float *xyz, float *st, float *norm,
	float *xnorm, float *ynorm, int nxyz)
{
	LBXGL_BrushLight_DrawPrimLightImage3(
		img, light, prim,
		xyz, st, norm, NULL,
		xnorm, ynorm, nxyz);
}

void LBXGL_BrushLight_DrawPrimLightImage3(
	LBXGL_TexImage *img, LBXGL_Light *light,
	int prim, float *xyz, float *st, float *norm,
	float *rgba,
	float *xnorm, float *ynorm, int nxyz)
{
	float pt[4], sdir[4], tdir[4];
	int i, l, tn, txn, tyn;

#ifndef GLES
	l=LBXGL_BrushLight_DrawSetupLightImage(light, img);

	if(l>0)
	{
		txn=PDGL_GetAttribLocation("vecSdir");
		tyn=PDGL_GetAttribLocation("vecTdir");
	}else { txn=-1; tyn=-1; }

//	LBXGL_Texture_SetImageMaterial(tex);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if(rgba)
		glEnableClientState(GL_COLOR_ARRAY);
	pdglEnableVertexAttribArray(txn);
	pdglEnableVertexAttribArray(tyn);

	i=4*sizeof(float);
	glVertexPointer(3, GL_FLOAT, i, xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, st);
	if(rgba)
		glColorPointer(4, GL_FLOAT, 0, rgba);
	glNormalPointer(GL_FLOAT, 4*sizeof(float), norm);
	pdglVertexAttribPointer(txn, 4, GL_FLOAT, 0, i, xnorm);
	pdglVertexAttribPointer(tyn, 4, GL_FLOAT, 0, i, ynorm);

	glDrawArrays(prim, 0, nxyz);

	pdglDisableVertexAttribArray(txn);
	pdglDisableVertexAttribArray(tyn);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	if(rgba)
		glDisableClientState(GL_COLOR_ARRAY);

#endif	//GLES
}

void LBXGL_BrushWorld_DrawBrushesLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_BrushWorld_DrawBrushesLightFlags(world, fst, light, 0);
}

void LBXGL_BrushWorld_DrawBrushesLightFlags(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst,
	LBXGL_Light *light, int flags)
{
	LBXGL_Brush *cur;
	float f, g, d;

	pdglDisableTexture2D();
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	if(light->flags&LBXGL_LFL_SUN)
	{
//		printf("LBXGL_BrushWorld_DrawBrushesLightFlags: Sun\n");
	}

	LBXGL_Shader_SetCurrentLight(light);
	LBXGL_BrushWorld_DrawSetupLight(world, light);

	pdglDisableTexture2D();
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_EQUAL, 0, 255);
	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_ONE, GL_ONE);

	glDepthMask(GL_FALSE);
//	glDepthRange(0, 0.99);

	glEnable(GL_CULL_FACE);
//	glDisable(GL_CULL_FACE);

	LBXGL_BrushDraw2_DrawBrushesLight(world, fst, light);
	LBXGL_Voxel_DrawWorldLight(world, light);

	LBXGL_BrushWorld_DrawModelsLight(world, light);

//	glDepthRange(0, 1);
	glDepthMask(GL_TRUE);

	PDGL_UnbindShader();
//	glDisable(GL_LIGHTING);
	pdglDisableLighting();

//	LBXGL_Terrain_DrawLight(world, light);

	// disable current light
	LBXGL_Shader_SetCurrentLight(NULL);
}

void LBXGL_BrushWorld_DrawBrushesHiLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	LBXGL_Brush *cur;
	float f, g, d;

	pdglDisableTexture2D();
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_SetCurrentLight(light);
	LBXGL_BrushWorld_DrawSetupHiLight(world, light);

	glDepthFunc(GL_LEQUAL);
	glStencilFunc(GL_EQUAL, 0, 255);
	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

	glDepthMask(GL_FALSE);
//	glDepthRange(0, 0.99);

//	glDisable(GL_CULL_FACE);

	LBXGL_BrushDraw2_DrawBrushesLight(world, fst, light);

	LBXGL_BrushWorld_DrawModelsLight(world, light);

//	glDepthRange(0, 1);
	glDepthMask(GL_TRUE);

	PDGL_UnbindShader();
//	glDisable(GL_LIGHTING);
	pdglDisableLighting();

	LBXGL_Terrain_DrawLight(world, light);

	// disable current light
	LBXGL_Shader_SetCurrentLight(NULL);
}

void LBXGL_BrushWorld_InitBrushRGB(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	int i;

	if(brush->patch)
	{
		LBXGL_BrushPatch_InitPatchRGB(world, brush);
		return;
	}

	if(brush->mesh)
	{
		LBXGL_BrushMesh_InitMeshRGB(world, brush);
		return;
	}

	V4F_SET(brush->ptrgb, 0, 0, 0, 1);
//	V4F_SET(brush->ptrgb, 1, 1, 1, 1);

	if(!brush->rgb)
		brush->rgb=gcatomic(brush->n_vec*3*sizeof(float));

	for(i=0; i<brush->n_vec; i++)
	{
		brush->rgb[i*3+0]=0;
		brush->rgb[i*3+1]=0;
		brush->rgb[i*3+2]=0;
	}
}

void LBXGL_BrushWorld_UpdateBrushesLight(
	LBXGL_BrushWorld *world, LBXGL_Brush *fst, LBXGL_Light *light)
{
	float dir[3], ldir[3];
	LBXGL_Brush *cur;
	float *v0, *rgb, *nv;
	float f, g, d;
	float tr, tg, tb;
	int i, j, k, l;

//	glDisable(GL_TEXTURE_2D);
//	glColor4f(1, 1, 1, 1);

	cur=fst;
	while(cur)
	{
		if(cur->patch)
		{
			LBXGL_BrushPatch_UpdatePatchLight(world, cur, light);
			cur=cur->chain;
			continue;
		}

		if(cur->mesh)
		{
			LBXGL_BrushMesh_UpdateMeshLight(world, cur, light);
			cur=cur->chain;
			continue;
		}

//		if(!cur->rgb)
//			cur->rgb=gcatomic(cur->n_vec*3);

		V3F_SUB(light->org, cur->org, ldir);
		V3F_NORMALIZE(ldir, ldir);
	
//		glEnable(GL_CULL_FACE);
//		LBXGL_Brush_DrawBrushLight3(cur, light);
		d=V3F_DIST(cur->org, light->org);
//		f=light->val[0]-d;
		f=light->curval-d;
		if(f<0)f=0;
		V3F_ADDSCALE(cur->ptrgb, light->clr, f/256.0, cur->ptrgb);

		if(cur->rgb)
		{
			for(i=0; i<cur->n_poly; i++)
			{
				nv=cur->norm+cur->pf_plane[i]*4;
				f=V3F_NDOT(light->org, nv);
				if(f<=0)continue;
				
				k=cur->pf_ivec[i];
				l=cur->pf_nvec[i];
				for(j=0; j<l; j++)
				{
					v0=cur->xyz+cur->vec[(k+j)*4+0]*3;
					rgb=cur->rgb+(k+j)*3;

					V3F_SUB(light->org, v0, ldir);
					V3F_NORMALIZE(ldir, ldir);
					V3F_COPY(nv, dir);

//					V3F_SUB(v0, cur->org, dir);
//					V3F_NORMALIZE(dir, dir);
//					V3F_ADD(dir, nv, dir);
//					V3F_NORMALIZE(dir, dir);

					d=V3F_DIST(v0, light->org);
					f=(light->curval-d)/256.0;
					if(f<=0)continue;

					g=V3F_DOT(dir, ldir);
//					f=f*((1.0+0.5*g)*(1.0/1.5));
					f=f*(1+g);
//					if(f<=0)continue;
					if(f<0)f=0;
					if(f>1)f=1;
				
					tr=rgb[0]+light->clr[0]*f;
					tg=rgb[1]+light->clr[1]*f;
					tb=rgb[2]+light->clr[2]*f;
//					tr=(tr<256)?tr:255;
//					tg=(tg<256)?tg:255;
//					tb=(tb<256)?tb:255;
					rgb[0]=tr;
					rgb[1]=tg;
					rgb[2]=tb;
				}
			}
		}

		cur=cur->chain;
	}

//	LBXGL_BrushWorld_DrawModelsLight(world, light);
}

void LBXGL_BrushWorld_DrawForLight(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_BrushWorld_DrawForLightFlags(world, light, 0);
}

void LBXGL_BrushWorld_DrawForSceneryLight(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_BrushWorld_DrawForLightFlags(
		world, light, BT_EF2_SCENERY);
}

void LBXGL_BrushWorld_DrawForLightFlags(
	LBXGL_BrushWorld *world, LBXGL_Light *light, int flags)
{
	LBXGL_Brush *lst;
//	LBXGL_Brush *lsta, *lstb;
	int t2, t3;

#if 1
	LBXGL_BrushWorld_UpdateLightBrushes(world, light);

	t2=PDGL_TimeMS();

	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

//	lst=LBXGL_BrushWorld_QueryBrushesLight(world, world->brush, light);
	lst=LBXGL_BrushWorld_QueryBrushesLightB(world, light);
	LBXGL_BrushWorld_DrawShadowsLight(world, lst, light, flags);
//	LBXGL_Voxel_DrawWorldShadows(world, light);

	t3=PDGL_TimeMS();
	lbxgl_state->dt_render_shadow_volume+=t3-t2;

//	lst=LBXGL_BrushWorld_QueryBrushesLight2(world, world->brush, light);
	lst=LBXGL_BrushWorld_QueryBrushesLight2B(world, light);
	LBXGL_BrushWorld_DrawBrushesLightFlags(world, lst, light, flags);
//	LBXGL_Voxel_DrawWorldLight(world, light);
	glDisable(GL_STENCIL_TEST);

	t2=PDGL_TimeMS();
	lbxgl_state->dt_render_shadow_face+=t2-t3;
#endif
}

void LBXGL_BrushWorld_DrawForLightNoShadow(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_Brush *lst;
	int t2, t3;

#if 1
//	lst=LBXGL_BrushWorld_QueryBrushesLight2(world, world->brush, light);
	lst=LBXGL_BrushWorld_QueryBrushesLight2B(world, light);
	LBXGL_BrushWorld_DrawBrushesLight(world, lst, light);
//	LBXGL_Voxel_DrawWorldLight(world, light);
#endif
}

void LBXGL_BrushWorld_DrawForHiLight(
	LBXGL_BrushWorld *world, LBXGL_Light *light)
{
	LBXGL_Brush *lst;
	int t2, t3;

#if 1
//	lst=LBXGL_BrushWorld_QueryBrushesLight2(world, world->brush, light);
	lst=LBXGL_BrushWorld_QueryBrushesLight2B(world, light);
	LBXGL_BrushWorld_DrawBrushesHiLight(world, lst, light);
//	LBXGL_Voxel_DrawWorldLight(world, light);
#endif
}

LBXGL_Light *LBXGL_BrushWorld_QueryLightsCam(
	LBXGL_BrushWorld *world)
{
	return(LBXGL_BrushWorld_QueryLightsCamFlags(
		world, 0));
}

LBXGL_Light *LBXGL_BrushWorld_QueryHiLightsCam(
	LBXGL_BrushWorld *world)
{
	return(LBXGL_BrushWorld_QueryLightsCamFlags(
		world, LBXGL_LFL_DETAIL));
}

LBXGL_Light *LBXGL_BrushWorld_QuerySceneryLightsCam(
	LBXGL_BrushWorld *world)
{
	return(LBXGL_BrushWorld_QueryLightsCamFlags(
		world, BT_EF2_SCENERY));
}

LBXGL_Light *LBXGL_BrushWorld_QueryGlossMapCam(
	LBXGL_BrushWorld *world)
{
//	return(LBXGL_BrushWorld_QueryLightsCamFlags(
//		world, LBXGL_LFL_ENVGLOSS));

	LBXGL_Light *cur, *lst;
	float f, g, vr;
	int i;

	cur=world->light; lst=NULL;
	while(cur)
	{
		if(!(cur->flags&LBXGL_LFL_ENVGLOSS))
			{ cur=cur->next; continue; }

		g=V3F_DIST(cur->org, lbxgl_cam->org);
		if(lst && (g>=vr)) { cur=cur->next; continue; }
		lst=cur; vr=g;
		cur=cur->next;
	}

	return(lst);
}

LBXGL_Light *LBXGL_BrushWorld_QueryAddLightList(
	LBXGL_BrushWorld *world, LBXGL_Light *lcur,
	LBXGL_Light *lst, int flags)
{
	LBXGL_Light *cur, *prv;
	float f, g, vr;

//	if(cur->flags&LBXGL_LFL_SUN)
//		printf("LBXGL_BrushWorld_QueryLightsCam: Sun: OK\n");

	f=LBXGL_Shadow_LightGammaPoint(lcur, lbxgl_cam->org, 32);
	lcur->tval=f;

	if(lcur->flags&LBXGL_LFL_SHADOWMAP)
	{
		cur=lst; prv=NULL;

		while(cur)
		{
			if(cur->flags&LBXGL_LFL_SHADOWMAP)break;
			if(cur->flags&LBXGL_LFL_NOSHADOW)break;
			prv=cur; cur=cur->chain;
		}

		while(cur)
		{
			if(cur->flags&LBXGL_LFL_NOSHADOW)break;
			if(f>cur->tval)break;
			prv=cur; cur=cur->chain;
		}
	}else if(lcur->flags&LBXGL_LFL_NOSHADOW)
	{
		cur=lst; prv=NULL;

		while(cur)
		{
			if(cur->flags&LBXGL_LFL_NOSHADOW)
				break;
			prv=cur; cur=cur->chain;
		}

		while(cur)
		{
			if(f>cur->tval)break;
			prv=cur; cur=cur->chain;
		}
	}else
	{
		cur=lst; prv=NULL;
		while(cur)
		{
			if(cur->flags&LBXGL_LFL_NOSHADOW)break;
			if(f>cur->tval)break;
			prv=cur; cur=cur->chain;
		}
	}

	if(prv) { lcur->chain=prv->chain; prv->chain=lcur; }
		else { lcur->chain=lst; lst=lcur; }

	return(lst);
}

bool LBXGL_BrushWorld_QueryLightsCam_CheckIgnoreFlags(
	LBXGL_BrushWorld *world, LBXGL_Light *cur, int flags)
{
	float vorg[3];
//	LBXGL_Light *cur, *cur1, *lst, *prv;
	LBXGL_Brush *bcur;
	float f, g, vr;
	int i, j, k;

	if(cur->flags&LBXGL_LFL_SUN)
	{
//		printf("LBXGL_BrushWorld_QueryLightsCam: Sun\n");
//		*(int *)-1=-1;
	}
		
//		if(cur->val[0]<=100) { return(true); }

	if(flags&LBXGL_LFL_ENVGLOSS)
	{
		if(!(cur->flags&LBXGL_LFL_ENVGLOSS))
			{ return(true); }
	}else
	{
		if(cur->flags&LBXGL_LFL_DISABLE)
			{ return(true); }
	}

	if(cur->flags&LBXGL_LFL_DETAIL)
	{
		if(world->flags&BTGE_WFL_LIGHTBUDGET)
			{ return(true); }
	}

	if(flags&LBXGL_LFL_DETAIL)
	{
		if(cur->val[1]<=0)
			{ return(true); }
	}

	if(flags&BT_EF2_SCENERY)
	{
		if(!(cur->flags&LBXGL_LFL_SCENERY))
			{ return(true); }
	}else
	{
		if(cur->flags&LBXGL_LFL_SCENERY)
			{ return(true); }
	}

	if(cur->flags&LBXGL_LFL_CAMREL)
	{
		V3F_ADD(cur->rel_org, lbxgl_cam->org, cur->org);
	}

#if 1
//	f=LBXGL_Shadow_LightGammaRadius(cur);
//	f=LBXGL_Shadow_LightGammaEffectRadius(cur, 0.01);
	f=cur->val[0];
//	f=cur->curval;
//	i=LBXGL_Brush_CheckSphereFrustum(cur->org, f);
	i=LBXGL_Brush_CheckSphereFrustumVisible(world, cur->org, f);
	if(!i) { return(true); }
#endif

#if 1
	//check if line of sight hits antiportal
	if(!LBXGL_Brush_CheckLineAntiportal(
		world, lbxgl_cam->org, cur->org))
			{ return(true); }
#endif

#if 1
	if(cur->visbrush && !(cur->flags&LBXGL_LFL_SUN))
	{
		k=0;
		for(i=0; cur->visbrush[i]; i++)
		{
			bcur=cur->visbrush[i];
			if(!LBXGL_BrushWorld_CheckNodeVisibleFast(
					world, bcur->node))
				continue;
			if(bcur->flags&LBXGL_BRFL_VISCLIP)
				continue;
			k++;
		}
		if(!k)
			{ return(true); }
	}
#endif

//	if(cur->flags&LBXGL_LFL_SUN)
//		printf("LBXGL_BrushWorld_QueryLightsCam: Sun A %f\n", f);

#if 1
	V3F_SCALEADDSCALE(world->autovis_mins, 0.5,
		world->autovis_maxs, 0.5, vorg);
	vr=V3F_DIST(world->autovis_maxs, vorg);
#endif

#if 1
	g=V3F_DIST(cur->org, vorg);
	if(g>=(f+vr)) { return(true); }
#endif

	return(false);
}

LBXGL_Light *LBXGL_BrushWorld_QueryLightsCamFlags(
	LBXGL_BrushWorld *world, int flags)
{
	float vorg[3];
	LBXGL_Light *cur, *cur1, *lst, *prv, *nxt;
	LBXGL_Brush *bcur;
	float f, g, vr;
	int i, j, k;

//	V3F_SCALEADDSCALE(world->autovis_mins, 0.5,
//		world->autovis_maxs, 0.5, vorg);
//	vr=V3F_DIST(world->autovis_maxs, vorg);

	if(flags&LBXGL_LFL_DETAIL)
	{
		if(world->flags&BTGE_WFL_LIGHTBUDGET)
			return(NULL);
	}

	cur=world->light; lst=NULL;
	while(cur)
	{
		if(LBXGL_BrushWorld_QueryLightsCam_CheckIgnoreFlags(
				world, cur, flags))
			{ cur=cur->next; continue; }

		lst=LBXGL_BrushWorld_QueryAddLightList(world, cur, lst, flags);
		cur=cur->next;
	}

	cur=LBXGL_Voxel_QueryLightsCamWorld(world);
	while(cur)
	{
		if(LBXGL_BrushWorld_QueryLightsCam_CheckIgnoreFlags(
				world, cur, flags))
			{ cur=cur->chain; continue; }

		nxt=cur->chain;
		lst=LBXGL_BrushWorld_QueryAddLightList(world, cur, lst, flags);
		cur=nxt;
	}

	cur=lst;
	while(cur)
	{
		LBXGL_BrushWorld_SetupLightFrame(world, cur);
		cur=cur->chain;
	}

	return(lst);
}

LBXGL_Light *LBXGL_BrushWorld_QueryLightsSphere(
	LBXGL_BrushWorld *world, float *org, float rad)
{
	LBXGL_Light *cur, *cur1, *lst, *prv;
	float f, g;
	int i;

	cur=world->light; lst=NULL;
	while(cur)
	{
		if(cur->flags&LBXGL_LFL_DISABLE)
			{ cur=cur->next; continue; }

		f=LBXGL_Shadow_LightGammaRadius(cur);

#if 1
		i=LBXGL_Brush_CheckSphereFrustum(cur->org, f);
		if(!i) { cur=cur->next; continue; }
#endif

#if 1
		g=V3F_DIST(cur->org, org);
		if(g>=(f+rad)) { cur=cur->next; continue; }
#endif

		f=LBXGL_Shadow_LightGammaPoint(cur, org, 32);
		cur->tval=f;

		cur1=lst; prv=NULL;
		while(cur1)
		{
			if(cur1->flags&LBXGL_LFL_NOSHADOW)break;
			if(f>cur1->tval)break;
			prv=cur1; cur1=cur1->chain;
		}

		if(prv) { cur->chain=prv->chain; prv->chain=cur; }
			else { cur->chain=lst; lst=cur; }

		cur=cur->next;
	}

	cur=lst;
	while(cur)
	{
		LBXGL_BrushWorld_SetupLightFrame(world, cur);
		cur=cur->chain;
	}

	return(lst);
}

void LBXGL_BrushWorld_ClearSun(LBXGL_BrushWorld *world)
{
	world->light_sun=NULL;
	LBXGL_Sky_SetSunLight(0);
}

void LBXGL_BrushWorld_SetupSun(LBXGL_BrushWorld *world,
	float *org, float *clr, float val)
{
	if(!world->light_sun)
	{
		world->light_sun=lbxWorldNewLight(world);
		world->light_sun->flags=LBXGL_LFL_CHANGED;
	}
		
//	LBXGL_Sky_GetSunOrigin(world->light_sun->org);
//	LBXGL_Sky_GetSunColor(world->light_sun->clr);

//	V3F_ADD(org, lbxgl_cam->org, world->light_sun->org);
	V3F_COPY(org, world->light_sun->rel_org);
	V3F_COPY(org, world->light_sun->org);
	V3F_COPY(clr, world->light_sun->clr);

	world->light_sun->val[0]=val;
	world->light_sun->val[1]=0;
	world->light_sun->curval=val;
//	world->light_sun->flags|=LBXGL_LFL_STATIC|LBXGL_LFL_SUN;
//	world->light_sun->flags|=LBXGL_LFL_SUN;
	world->light_sun->flags|=LBXGL_LFL_SUN|LBXGL_LFL_CAMREL;

	if(btCvarGetf("r_shadowmap")>0.0)
	{
		world->light_sun->flags|=
			LBXGL_LFL_ENVRENDER|LBXGL_LFL_SHADOWMAP;
	}

	V3F_SET(world->light_sun->mins, -val, -val, -val);
	V3F_SET(world->light_sun->maxs, val, val, val);

	V3F_ADD(world->light_sun->org,
		world->light_sun->mins,
		world->light_sun->amins);
	V3F_ADD(world->light_sun->org,
		world->light_sun->maxs,
		world->light_sun->amaxs);

	world->light_sun->coeff[0]=0.50/0.58;
	world->light_sun->coeff[1]=0.50/0.58;
	world->light_sun->coeff[2]=1;

	world->light_sun->flags&=~LBXGL_LFL_DISABLE;

#if 0
	printf("Sun Origin=( %f %f %f )\n",
		world->light_sun->org[0],
		world->light_sun->org[1],
		world->light_sun->org[2]);
	printf("Sun Color=( %f %f %f)\n",
		world->light_sun->clr[0],
		world->light_sun->clr[1],
		world->light_sun->clr[2]);
	printf("Sun Light=%f\n", world->light_sun->val[0]);
#endif
}

void LBXGL_BrushWorld_UpdateSun(LBXGL_BrushWorld *world)
{
	float org[3], clr[3];
	float f;
	
	f=LBXGL_Sky_GetSunLight();
	if((f>0) || world->light_sun)
	{
		if(!world->light_sun)
		{
			LBXGL_Sky_GetSunOrigin(org);
			LBXGL_Sky_GetSunColor(clr);
			LBXGL_BrushWorld_SetupSun(world, org, clr, f);
		}
	}
	
	if(0)
	{
//		if(!world->light_sun)
		if(1)
		{
			V3F_SET(org, 0, 0, 1024);
//			V3F_SET(org, 0, 0, 0);
			V3F_SET(clr, 1, 1, 1);
			f=10000;
//			f=1000;
			LBXGL_BrushWorld_SetupSun(world, org, clr, f);
		}
	}
}


LBXGL_API LBXGL_Light *lbxWorldNewLight(lbxWorld *wrl)
{
	LBXGL_Light *dl;

	dl=gctalloc("lbxgl_light_t", sizeof(LBXGL_Light));
	dl->next=wrl->light;
	wrl->light=dl;
	return(dl);
}

LBXGL_API LBXGL_Light *lbxWorldFetchLight(lbxWorld *wrl, int id)
{
	LBXGL_Light *cur;
	int i;

	cur=wrl->light;
	while(cur)
	{
		if(cur->num==id)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_API LBXGL_Light *lbxWorldGetLight(lbxWorld *wrl, int id)
{
	LBXGL_Light *cur;

	cur=lbxWorldFetchLight(wrl, id);
	if(cur)return(cur);
	
	cur=lbxWorldNewLight(wrl);
	cur->num=id;
	return(cur);
}
