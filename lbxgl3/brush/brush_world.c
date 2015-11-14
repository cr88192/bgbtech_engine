#include <lbxgl2.h>
#include <pdglui.h>

// #include <GL/GL.h>
// #include <GL_2/GLext.h>

int lbxgl_draw_light_xs=512;
int lbxgl_draw_light_ys=512;
float lbxgl_draw_light_delay=0;

float lbxgl_draw_gamma=4;

int lbxgl_draw_light_cxs=512;
int lbxgl_draw_light_cys=512;

int tex_screen;
int tex_screen_depth;
int tex_lighting=-1;

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

LBXGL_API LBXGL_BrushWorld *LBXGL_Brush_NewWorld()
{
	LBXGL_BrushWorld *tmp;

	LBXGL_InitCam();

	tmp=gctalloc("lbxgl_brush_world_t", sizeof(LBXGL_BrushWorld));

	tmp->shadows=48;
	tmp->default_tex="aardo/aardmbr64_1";
//	tmp->basegamma=1.25;
	tmp->basegamma=1.0;
	tmp->curgamma=tmp->basegamma;

	return(tmp);
}

LBXGL_API void LBXGL_Brush_FreeWorld(LBXGL_BrushWorld *world)
{
	LBXGL_BrushWorld_DrawFlush(world);
	LBXGL_BrushWorld_ClearSun(world);
	
	gcfree(world);
}

LBXGL_API LBXGL_BrushWorld *LBXGL_Brush_CloneWorld(LBXGL_BrushWorld *world)
{
	LBXGL_BrushWorld *tmp;
	LBXGL_SEntity *cur;
	LBXGL_Brush *bcur;

	tmp=LBXGL_Brush_NewWorld(world);

	//clone all entities (and brushes) for world
	cur=world->entity;
	while(cur)
	{
		LBXGL_SEnt_NewCloneEntity(&tmp->entity, NULL, cur);
		cur=cur->next;
	}

	//link all brushes into world brush list
	cur=tmp->entity;
	while(cur)
	{
		bcur=cur->brush;
		while(bcur)
		{
			bcur->next=tmp->brush;
			if(tmp->brush)tmp->brush->prev=bcur;
			tmp->brush=bcur;
			bcur=bcur->enext;
		}
		cur=cur->next;
	}

	return(tmp);
}

LBXGL_API LBXGL_SEntity *LBXGL_BrushWorld_FindSEnt(
	LBXGL_BrushWorld *world, LBXGL_SEntity *last, char *key, char *val)
{
	LBXGL_SEntity *cur;
	char *s;

	if(last)cur=last->next;
		else cur=world->entity;
	while(cur)
	{
		s=LBXGL_SEnt_GetStr(cur, key);

		if(!s && !val)return(cur);
		if(s && val && !strcmp(s, val))return(cur);

		cur=cur->next;
	}

	return(NULL);
}

LBXGL_API void LBXGL_BrushWorld_AddEntityBrush(
	LBXGL_BrushWorld *world, LBXGL_SEntity *ent, LBXGL_Brush *brsh)
{
	LBXGL_Brush *cur;

	brsh->se_owner=ent;

	if(ent->brush)
	{
		if(ent->brush_last)
		{
			cur=ent->brush_last;
			cur->enext=brsh;
			ent->brush_last=brsh;
		}else
		{
			cur=ent->brush;
			while(cur->enext)
			{
				if(cur==brsh)
				{
					*(int *)-1=-1;
					return;
				}
				cur=cur->enext;
			}
			cur->enext=brsh;
			ent->brush_last=brsh;
		}
	}else
	{
		ent->brush=brsh;
		ent->brush_last=brsh;
	}
}

LBXGL_API void LBXGL_BrushWorld_AddBrush(
	LBXGL_BrushWorld *world, LBXGL_Brush *brsh)
{
	LBXGL_Brush *cur;
	LBXGL_SEntity *ent;

	if(world->brush)
	{
		if(world->brush_last)
		{
			cur=world->brush_last;
			cur->next=brsh;
			world->brush_last=brsh;
		}else
		{
			cur=world->brush;
			while(cur->next)
			{
				if(cur==brsh)
				{
					*(int *)-1=-1;
					return;
				}
				cur=cur->next;
			}
			cur->next=brsh;
			world->brush_last=brsh;
		}
	}else
	{
		world->brush=brsh;
		world->brush_last=brsh;
	}

	if(world->entity_psel)
	{
		LBXGL_BrushWorld_AddEntityBrush(world,
			world->entity_psel, brsh);

#if 0
		brsh->se_owner=world->entity_psel;

		if(world->entity_psel->brush)
		{
			cur=world->entity_psel->brush;
			while(cur->enext)
			{
				if(cur==brsh)
				{
					*(int *)-1=-1;
					return;
				}
				cur=cur->enext;
			}
			cur->enext=brsh;
		}else world->entity_psel->brush=brsh;
#endif
	}else
	{
		ent=LBXGL_BrushWorld_FindSEnt(world, NULL,
			"classname", "worldspawn");
		if(ent)
		{
			LBXGL_BrushWorld_AddEntityBrush(world, ent, brsh);

#if 0
			brsh->se_owner=ent;

			if(ent->brush)
			{
				cur=ent->brush;
				while(cur->enext)cur=cur->enext;
				cur->enext=brsh;
			}else ent->brush=brsh;
#endif
		}
	}
}

LBXGL_API LBXGL_SEntity *LBXGL_BrushWorld_NewBrushEntity(
	LBXGL_BrushWorld *world, char *type)
{
	float pt[3];
	LBXGL_SEntity *tmp;
	LBXGL_Brush *brsh;

	tmp=gctalloc("lbxgl_sentity_s", sizeof(LBXGL_SEntity));
	LBXGL_SEnt_SetStr(tmp, "classname", type);
	LBXGL_SEnt_AddEntity(&world->entity, tmp);

	world->entity_psel=tmp;

	V3F_ADDSCALE(lbxgl_cam->org, lbxgl_cam->fw, 24, pt);
	brsh=LBXGL_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
		world->default_tex);
	LBXGL_BrushWorld_AddBrush(world, brsh);

	return(tmp);
}

LBXGL_API void LBXGL_BrushWorld_DrawBuildFast(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur, *lst;
	LBXGL_BrushModel *mcur, *mlst;
	LBXGL_SEntity *ecur;
	int num;
	char *s;
	int i;

	if(world->bsp)
		return;
	if(!world->brush)
		return;

	world->mesh=NULL;
	world->brush_antiportal=NULL;
	world->light_sun=NULL;

	num=1;
	ecur=world->entity; mlst=NULL;
	while(ecur)
	{
		if(ecur->brush)
		{
			if(ecur->bmdl)
			{
				mcur=ecur->bmdl;
				mcur->next=mlst;
				mlst=mcur;

//				mcur->mdlnum=num++;
//				LBXGL_SEnt_SetInt(ecur, "model", mcur->mdlnum);

				mcur->mdlnum=ecur->idnum;

				if(ecur->mdl)
				{
					ecur->mdl->next=world->mesh;
					world->mesh=ecur->mdl;
				}

				ecur=ecur->next;
				continue;
			}


			cur=ecur->brush; lst=NULL;
			while(cur)
			{
				if(cur->flags&LBXGL_BRFL_DEL)
					{ cur=cur->enext; continue; }
				if(cur->se_child)
				{
					BTGE_BrushWorld_BrushSpawnEntity(world, cur);
					if(!cur->patch && !cur->mesh && !cur->n_face)
						{ cur=cur->enext; continue; }
				}
				if(!cur->xyz)
					LBXGL_Brush_BuildFaces(cur);
				cur->chain=lst; lst=cur;
				cur=cur->enext;
			}

			mcur=LBXGL_BrushBSP_BuildBrushModelListFast(lst);
			ecur->bmdl=mcur;
			mcur->sent=ecur;
			
//			mcur->mdlnum=num++;
			mcur->mdlnum=ecur->idnum;

//			LBXGL_SEnt_SetInt(ecur, "model", mcur->mdlnum);

			mcur->next=mlst;
			mlst=mcur;

			s=LBXGL_SEnt_GetStr(ecur, "classname");
			if(!strcmp(s, "worldspawn"))
			{
				world->bsp=mcur->bsp;
			}else
			{
				ecur->mdl=LBXGL_Mdl_WrapModelInstance(mcur);
				mcur->mesh=ecur->mdl;

//				LBXGL_SEnt_GetVec3(ecur, "origin", mcur->org);

				ecur->mdl->next=world->mesh;
				world->mesh=ecur->mdl;

//				printf("BMdl @ %f %f %f\n",
//					mcur->org[0], mcur->org[1], mcur->org[2]);
			}
		}

		BTGE_BrushWorld_SpawnEntity(world, ecur);
		ecur=ecur->next;
	}

	world->bmdl=mlst;

#if 1
	cur=world->brush; i=0;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_DEL)
			{ cur=cur->next; continue; }

		if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);

		if((cur->flags&LBXGL_TXFL_EFFECT_MASK)==
			LBXGL_TXFL_EFFECT_ANTIPORTAL)
		{
			cur->knext=world->brush_antiportal;
			world->brush_antiportal=cur;
			i++;
		}

		cur=cur->next;
	}
	if(i)printf("LBXGL_BrushWorld_DrawBuildFast: %d antiportals\n", i);
#endif
}

LBXGL_API void LBXGL_BrushWorld_DrawBuild(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur, *lst;

#if 0
	LBXGL_BrushWorld_DrawFlush(world);

	cur=world->brush; lst=NULL;
	while(cur)
	{
		if(!cur->xyz)LBXGL_Brush_BuildFaces(cur);
		cur->chain=lst; lst=cur; cur=cur->next;
	}

	world->bsp=LBXGL_BrushBSP_BuildTreeList(lst);
#endif
}

LBXGL_API LBXGL_Brush *LBXGL_BrushWorld_LookupBrush(
	LBXGL_BrushWorld *world, int id)
		{ return(BTGE_BrushWorld_LookupBrush(world, id)); }

LBXGL_API LBXGL_SEntity *LBXGL_BrushWorld_LookupSEntityId(
	LBXGL_BrushWorld *world, int id)
		{ return(BTGE_BrushWorld_LookupSEntityId(world, id)); }

LBXGL_API LBXGL_SEntity *LBXGL_BrushWorld_GetSEntityId(
	LBXGL_BrushWorld *world, int id)
		{ return(BTGE_BrushWorld_GetSEntityId(world, id)); }

LBXGL_API LBXGL_Brush *LBXGL_BrushWorld_CreateBrushId(
	LBXGL_BrushWorld *world, int sentid, int brushid)
		{ return(BTGE_BrushWorld_CreateBrushId(world,
			sentid, brushid)); }

// LBXGL_API void LBXGL_BrushWorld_DrawFlushBrush(
//	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
//{
//	BTGE_BrushWorld_DrawFlushBrush(world, brush);
//}

LBXGL_API void LBXGL_BrushWorld_DrawFlush(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur, *lst;
	LBXGL_BrushModel *mcur, *mlst;
	LBXGL_ModelState *mscur, *msnxt;
	LBXGL_Light *lcur, *lnxt;
	LBXGL_SEntity *ecur;
	char *s;

	if(!world->bsp)
		return;

//	LBXGL_BrushPhys_UnlinkWorld(world);
	LBXGL_BrushGround_FlushGround(world);

	//flush any model states
	mscur=world->mesh;
	while(mscur)
	{
		msnxt=mscur->next;
		LBXGL_Mdl_FreeModel(mscur);
		mscur=msnxt;
	}
	world->mesh=NULL;

	//flush any brush models
	ecur=world->entity;
	while(ecur)
	{
		if(ecur->bmdl)
		{
			LBXGL_BrushBSP_FreeBrushModel(ecur->bmdl);
			ecur->bmdl=NULL;

			if(ecur->mdl)
			{
//				LBXGL_Mdl_FreeModel(ecur->mdl);
				ecur->mdl=NULL;
			}
		}
		ecur=ecur->next;
	}

	world->bmdl=NULL;
	world->bsp=NULL;

	//flush cached brush visibility
	lcur=world->light;
	while(lcur)
	{
		lnxt=lcur->next;
		if(lcur->visbrush)
		{
			gcfree(lcur->visbrush);
			lcur->visbrush=NULL;
		}
		lcur=lnxt;
	}

	//mark any transient brushes as deleted
	cur=world->brush;
	while(cur)
	{
		if(cur->flags&LBXGL_BRFL_DEL)
			{ cur=cur->next; continue; }
		if(cur->flags&LBXGL_BRFL_TRANSIENT)
			{ cur->flags|=LBXGL_BRFL_DEL; }
		cur=cur->next;
	}
}

float lbxgl_brush_roundsnap(float val, int snap)
{
	float f, g;

	if(snap<=0)return(val);
	f=fabs(val)/snap;
	if((f>=(0.1/snap)) && (f<0.5))
		return((val>0)?snap:-snap);
	g=floor(f+0.5)*snap;
	return((val>0)?g:-g);
}

float lbxgl_brush_roundsnap2(float val, int snap)
{
	float f, g;

	if(snap<=0)return(val);
	f=fabs(val)/snap;
	g=floor(f+0.5)*snap;
	return((val>0)?g:-g);
}

void lbxgl_brush_v3fdiv(float *a, float *b, float *c)
{
	float f;
	int i;
	
	for(i=0; i<3; i++)
	{
		f=b[i];
		if(fabs(f)<0.001)f=1;
		c[i]=a[i]/f;
	}
}

/** Determine if this normal would be valid for this brush. */
int LBXGL_BrushWorld_CheckBrushNormalValidP(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush,
	float *norm, int fn)
{
	float f, g;
	int i;

	BTGE_Brush_GetPlaneExtents(brush, norm, &f, &g);
	if(f>=0)return(0);
	
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_plane[i]==fn)
			continue;

		BTGE_Brush_GetBrushFacePlaneExtents(brush, i, norm, &f, &g);
		if(f>0)return(0);
	}
	
	return(1);
}

LBXGL_API void LBXGL_BrushWorld_DrawFlushBrush(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	BTGE_BrushNode *ncur, *pncur;
	float f, g;

	brush->flags|=BTGE_BRFL_DIRTY;
	LBXGL_Brush_DestroyFaces(brush);
//	LBXGL_BrushWorld_DrawFlush(world);

	if(!world->bsp)
		return;

	//determine if brush crosses any parent-node planes
	ncur=brush->node; pncur=NULL;
	while(ncur)
	{
		if(ncur!=brush->node)
		{
			//crosses a higher-level plane
//			LBXGL_Brush_GetPlaneExtents(brush, ncur->norm, &f, &g);
			LBXGL_Brush_GetBBoxPlaneExtents(brush, ncur->norm, &f, &g);
			if((f<0) && (g>0))
				break;

			//wrong side of plane
			if(pncur==ncur->lnode)
				if(f>0)break;
			if(pncur==ncur->rnode)
				if(g<0)break;
		}
		pncur=ncur;
		ncur=ncur->pnode;
	}
	
	//crossed plane
	if(ncur) { LBXGL_BrushWorld_DrawFlush(world); }
}

void LBXGL_BrushWorld_UpdateBrushFace(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush, int id)
{
	char buf[256];

	float tv0[3], tv1[3], tv2[3];
	float org[3], mrorg[3], tnorm[4];
	float sx, sy, f, g;
	int tn, xs, ys;
	int i, j, k, l, t;

	V3F_COPY(lbxgl_cam->morg, tv0);
	V3F_COPY(lbxgl_cam->mend, tv1);

	for(i=0; i<brush->n_face; i++)
	{
		f=V3F_NDOT(tv0, brush->norm+i*4);
		g=V3F_NDOT(tv1, brush->norm+i*4);
		if((f>0) && (g>0))break;
		if((f<=0) && (g<=0))continue;

		Hull_LinePlaneIntersect(tv0, tv1, brush->norm+i*4, tv2);

		if(f>0) { V3F_COPY(tv2, tv0); }
			else { V3F_COPY(tv2, tv1); }
	}

	V3F_COPY(brush->norg+id*3, org);
	if(brush==world->brush_sel)
	{
		pdglColor4f(0, 0, 1, 1);

		f=(V3F_DOT(org, lbxgl_cam->fw)-
			V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw))/64;
		g=Vec3F_DistLineSeg(org, lbxgl_cam->morg, lbxgl_cam->mend);
		if(g<f)pdglColor4f(1, 0, 1, 1);

		g=V3F_DOT(org, lbxgl_cam->mdir)-
			V3F_DOT(lbxgl_cam->morg, lbxgl_cam->mdir);
		V3F_ADDSCALE(lbxgl_cam->morg, lbxgl_cam->mdir, g, mrorg);

		if(id==world->brush_sel_face)
		{
			pdglColor4f(0, 1, 0, 1);

			if(PDGL_KeyDown(K_SHIFT) && (lbxgl_state->mb&1))
			{
				f=V3F_DOT(org, lbxgl_cam->fw)-
					V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);
				V3F_SCALEADDSCALE(
					lbxgl_cam->rt, f*lbxgl_state->dmx/400.0,
					lbxgl_cam->up, f*(-lbxgl_state->dmy)/400.0, tv0);

				V3F_SUB(lbxgl_cam->org, lbxgl_cam->lorg, tv1);
				V3F_ADD(tv0, tv1, tv0);

				V3F_SUB(mrorg, org, tv1);
//				lbxgl_brush_v3fdiv(tv0, tv1, tv0);
				V3F_CROSS(tv1, tv0, tv2);
				
				f=V3F_NORMALIZE(tv2, tv2)*0.001;
				Vec3F_RotatePointAxis(brush->norm+id*4,
					tv2, f, tnorm);
				V3F_NORMALIZE(tnorm, tnorm);
				tnorm[3]=V3F_DOT(org, tnorm);
				
				g=V3F_DOT(tnorm, brush->norm+id*4);
				if((g>0.90) && (g<=1) && 
					LBXGL_BrushWorld_CheckBrushNormalValidP(
						world, brush, tnorm, id))
				{
					V4F_COPY(tnorm, brush->norm+id*4);
				}
				
				printf("Org (%.3f %.3f %.3f)\n", org[0], org[1], org[2]);
				printf("Mrorg (%.3f %.3f %.3f)\n",
					mrorg[0], mrorg[1], mrorg[2]);

				printf("Norm (%.3f %.3f %.3f %.3f)\n",
					brush->norm[id*4+0],
					brush->norm[id*4+1],
					brush->norm[id*4+2],
					brush->norm[id*4+3]);

				printf("Tv0 (%.3f %.3f %.3f)\n", tv0[0], tv0[1], tv0[2]);
				printf("Tv1 (%.3f %.3f %.3f)\n", tv1[0], tv1[1], tv1[2]);
				printf("Tv2 (%.3f %.3f %.3f)\n", tv2[0], tv2[1], tv2[2]);

//				brush->flags|=BTGE_BRFL_DIRTY;
//				LBXGL_Brush_DestroyFaces(brush);
//				LBXGL_BrushWorld_DrawFlush(world);
				LBXGL_BrushWorld_DrawFlushBrush(world, brush);

//				V3F_ADD(brush->norm+id*4, tv0, brush->norm+id*4);
//				V3F_NORMALIZE(brush->norm+id*4, brush->norm+id*4);
			}else if(lbxgl_state->mb&1)
			{
				f=V3F_DOT(org, lbxgl_cam->fw)-
					V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);
				V3F_SCALEADDSCALE(
					lbxgl_cam->rt, f*lbxgl_state->dmx/400.0,
					lbxgl_cam->up, f*(-lbxgl_state->dmy)/400.0, tv0);

				V3F_SUB(lbxgl_cam->org, lbxgl_cam->lorg, tv1);
				V3F_ADD(tv0, tv1, tv0);

				g=V3F_DOT(tv0, brush->norm+id*4);
//				g=floor(g+0.5);
				g=lbxgl_brush_roundsnap(g, world->grid_snap);
//				brush->norm[id*4+3]+=g;

				V4F_COPY(brush->norm+id*4, tnorm);
				tnorm[3]=lbxgl_brush_roundsnap2(
					brush->norm[id*4+3]+g, world->grid_snap);

//				brush->norm[id*4+3]=lbxgl_brush_roundsnap2(
//					brush->norm[id*4+3]+g, world->grid_snap);

				if(LBXGL_BrushWorld_CheckBrushNormalValidP(
						world, brush, tnorm, id))
				{
					V4F_COPY(tnorm, brush->norm+id*4);
				}

//				brush->flags|=BTGE_BRFL_DIRTY;
//				LBXGL_Brush_DestroyFaces(brush);
//				LBXGL_BrushWorld_DrawFlush(world);
				LBXGL_BrushWorld_DrawFlushBrush(world, brush);
			}
		}

#if 1
//		if(V3F_NDOT(lbxgl_cam->org, brush->norm+id*4)>=0)
		if(1)
		{
			f=(V3F_DOT(org, lbxgl_cam->fw)-
				V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw))/64;

			LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			pdglDisableTexture2D();
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);

			pdglBegin(PDGL_POLYGON);

			V3F_ADDSCALEADDSCALE(org, lbxgl_cam->rt, -f,
				lbxgl_cam->up, -f, tv0);
			pdglVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, lbxgl_cam->rt, f,
				lbxgl_cam->up, -f, tv0);
			pdglVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, lbxgl_cam->rt, f,
				lbxgl_cam->up, f, tv0);
			pdglVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, lbxgl_cam->rt, -f,
				lbxgl_cam->up, f, tv0);
			pdglVertex3fv(tv0);

			pdglEnd();

			glEnable(GL_DEPTH_TEST);
			pdglEnableTexture2D();
		}
#endif
	}
}


LBXGL_API LBXGL_Brush *LBXGL_BrushWorld_TraceLineBrush(LBXGL_BrushWorld *world,
	float *s, float *e, LBXGL_Brush *ignore)
{
	LBXGL_Brush *bcur;

	if(!world)return(NULL);

	bcur=world->brush;
	while(bcur)
	{
		if(bcur==ignore)
		{
			bcur=bcur->next;
			continue;
		}

		if(LBXGL_Brush_CheckLineBrush(bcur, s, e))
			return(bcur);
		bcur=bcur->next;
	}

	return(NULL);
}

LBXGL_API void LBXGL_BrushWorld_DrawControlsBrush(
	LBXGL_BrushWorld *world, LBXGL_Brush *brush)
{
	float tv0[3], tv1[3], tv2[3];
	float x, y, z;
	float f, g;
	int i;

	brush->flags&=~(LBXGL_BRFL_SEL|LBXGL_BRFL_OPSEL);
	if(world->brush_sel==brush)
		brush->flags|=LBXGL_BRFL_SEL;
	if(LBXGL_Brush_CheckLineBrush(brush, lbxgl_cam->morg, lbxgl_cam->mend))
		brush->flags|=LBXGL_BRFL_OPSEL;

//	LBXGL_Brush_DrawFaces(brush);
//	LBXGL_Brush_DrawFacesShadow(brush);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for(i=0; i<brush->n_face; i++)
	{
		LBXGL_BrushWorld_UpdateBrushFace(world, brush, i);
	}

	if((lbxgl_state->mb&1) && (lbxgl_state->mb&2) && (lbxgl_state->lmb&2) &&
		(world->brush_sel==brush))
	{
		f=V3F_DOT(brush->org, lbxgl_cam->lmdir)-
			V3F_DOT(lbxgl_cam->lmorg, lbxgl_cam->lmdir);
		V3F_ADDSCALE(lbxgl_cam->morg, lbxgl_cam->mdir, f, tv0);
		V3F_SUB(tv0, brush->org, tv0);

		for(i=0; i<brush->n_face; i++)
		{
//			brush->norm[i*4+3]+=V3F_DOT(tv0, brush->norm+i*4);
//			brush->sdir[i*4+3]+=V3F_DOT(tv0, brush->sdir+i*4);
//			brush->tdir[i*4+3]+=V3F_DOT(tv0, brush->tdir+i*4);

			x=V3F_DOT(tv0, brush->norm+i*4);
			y=V3F_DOT(tv0, brush->sdir+i*4);
			z=V3F_DOT(tv0, brush->tdir+i*4);

			x=lbxgl_brush_roundsnap(x, world->grid_snap);
			y=lbxgl_brush_roundsnap(y, world->grid_snap);
			z=lbxgl_brush_roundsnap(z, world->grid_snap);

//			brush->norm[i*4+3]+=x;
//			brush->sdir[i*4+3]+=y;
//			brush->tdir[i*4+3]+=z;

			brush->norm[i*4+3]=lbxgl_brush_roundsnap2(
				brush->norm[i*4+3]+x, world->grid_snap);
			brush->sdir[i*4+3]=lbxgl_brush_roundsnap2(
				brush->sdir[i*4+3]+y, world->grid_snap);
			brush->tdir[i*4+3]=lbxgl_brush_roundsnap2(
				brush->tdir[i*4+3]+z, world->grid_snap);

//			g=lbxgl_brush_roundsnap(g, 1);
		}

//		brush->flags|=BTGE_BRFL_DIRTY;
//		LBXGL_Brush_DestroyFaces(brush);
//		LBXGL_BrushWorld_DrawFlush(world);
		LBXGL_BrushWorld_DrawFlushBrush(world, brush);

		return;
	}

	if((lbxgl_state->mb&1) && (world->brush_sel==brush) && (world->brush_sel_face==-1))
	{

		f=V3F_DOT(brush->org, lbxgl_cam->fw)-
			V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);
		V3F_SCALEADDSCALE(
			lbxgl_cam->rt, f*lbxgl_state->dmx/400.0,
			lbxgl_cam->up, f*(-lbxgl_state->dmy)/400.0, tv0);
		V3F_SUB(lbxgl_cam->org, lbxgl_cam->lorg, tv1);
		V3F_ADD(tv0, tv1, tv0);

		for(i=0; i<brush->n_face; i++)
		{
//			brush->norm[i*4+3]+=V3F_DOT(tv0, brush->norm+i*4);
//			brush->sdir[i*4+3]+=V3F_DOT(tv0, brush->sdir+i*4);
//			brush->tdir[i*4+3]+=V3F_DOT(tv0, brush->tdir+i*4);

			x=V3F_DOT(tv0, brush->norm+i*4);
			y=V3F_DOT(tv0, brush->sdir+i*4);
			z=V3F_DOT(tv0, brush->tdir+i*4);

			x=lbxgl_brush_roundsnap(x, world->grid_snap);
			y=lbxgl_brush_roundsnap(y, world->grid_snap);
			z=lbxgl_brush_roundsnap(z, world->grid_snap);

//			brush->norm[i*4+3]+=x;
//			brush->sdir[i*4+3]+=y;
//			brush->tdir[i*4+3]+=z;

			brush->norm[i*4+3]=lbxgl_brush_roundsnap2(
				brush->norm[i*4+3]+x, world->grid_snap);
			brush->sdir[i*4+3]=lbxgl_brush_roundsnap2(
				brush->sdir[i*4+3]+y, world->grid_snap);
			brush->tdir[i*4+3]=lbxgl_brush_roundsnap2(
				brush->tdir[i*4+3]+z, world->grid_snap);
		}

//		brush->flags|=BTGE_BRFL_DIRTY;
//		LBXGL_Brush_DestroyFaces(brush);
//		LBXGL_BrushWorld_DrawFlush(world);
		LBXGL_BrushWorld_DrawFlushBrush(world, brush);
	}
}

void LBXGL_BrushWorld_DrawFlashlight(LBXGL_BrushWorld *world)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j, tn;
	float f;

#if 0
	if(world->flashlight)
	{
		lcur=gcalloc(sizeof(LBXGL_Light));

		f=0.57735;
		V3F_SET(lcur->clr, f, f, f);

		V3F_ADDSCALE(lbxgl_cam->org, lbxgl_cam->up, -12, lcur->org);

		lcur->val[0]=1000;
		lcur->val[1]=0;

		V3F_COPY(lbxgl_cam->fw, lcur->dir);

		lcur->flags|=LBXGL_LFL_SPOTLIGHT;
		lcur->angle=10;
		

		glClear(GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		lst=LBXGL_BrushWorld_QueryBrushesLight(world, fst, lcur);
		LBXGL_BrushWorld_DrawShadowsLight(world, lst, lcur);
		lst=LBXGL_BrushWorld_QueryBrushesLight2(world, fst, lcur);
		LBXGL_BrushWorld_DrawBrushesLight(world, lst, lcur);
		glDisable(GL_STENCIL_TEST);
	}
#endif

	if(!world->flashlight)
		return;

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -6, lbxgl_cam->up, -6, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  6, lbxgl_cam->up, -6, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  6, lbxgl_cam->up,  6, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -6, lbxgl_cam->up,  6, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 12, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 12, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 12, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 12, pt3);

	V3F_ADD(pt0, lbxgl_cam->org, pt0);
	V3F_ADD(pt1, lbxgl_cam->org, pt1);
	V3F_ADD(pt2, lbxgl_cam->org, pt2);
	V3F_ADD(pt3, lbxgl_cam->org, pt3);

	glDepthMask(0);
	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
	pdglColor4f(1, 1, 1, 1);

	glDisable(GL_TEXTURE_CUBE_MAP);

	tn=Tex_LoadFile("textures/effects/flash0", NULL, NULL);
	glBindTexture(GL_TEXTURE_2D, tn);
	pdglEnableTexture2D();

	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(1, 0);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(1, 1);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(0, 1);
	pdglVertex3fv(pt3);
	pdglEnd();

	glDepthMask(1);
}

void LBXGL_BrushWorld_DrawLightTexture(LBXGL_BrushWorld *world)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j, tn;
	float f, g, h;


	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);


//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(-1, 1, -1/aspect, 1000/aspect, -99999, 99999);

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->v_org, pt0);
	V3F_ADD(pt1, lbxgl_cam->v_org, pt1);
	V3F_ADD(pt2, lbxgl_cam->v_org, pt2);
	V3F_ADD(pt3, lbxgl_cam->v_org, pt3);

	glDepthMask(0);

#if 1
	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	pdglColor4f(1, 1, 1, 1);
//	pdglColor4f(0.5, 0.5, 0.5, 1);

	glBindTexture(GL_TEXTURE_2D, tex_lighting);
	pdglEnableTexture2D();

	f=((float)lbxgl_draw_light_cxs)/lbxgl_draw_light_xs;
	g=((float)lbxgl_draw_light_cys)/lbxgl_draw_light_ys;

	h=lbxgl_draw_gamma;
	while(h>1.0)
	{
		pdglColor4f(1, 1, 1, 1);

		pdglBegin(PDGL_POLYGON);
		pdglTexCoord2f(0, 0);
		pdglVertex3fv(pt0);
		pdglTexCoord2f(f, 0);
		pdglVertex3fv(pt1);
		pdglTexCoord2f(f, g);
		pdglVertex3fv(pt2);
		pdglTexCoord2f(0, g);
		pdglVertex3fv(pt3);
		pdglEnd();

		h-=1.0;
	}

	pdglColor4f(h, h, h, 1);

	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(f, 0);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(f, g);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(0, g);
	pdglVertex3fv(pt3);
	pdglEnd();
#endif


	glDepthMask(1);
}

#if 0
void LBXGL_BrushWorld_DrawScreenTexture(
	LBXGL_BrushWorld *world, int tex, float gam)
{
//	float pt0[3], pt1[3], pt2[3], pt3[3];
	double pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j, tn;
	float f, g, h;


	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);
//	glDisable(GL_DEPTH_TEST);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->v_org, pt0);
	V3F_ADD(pt1, lbxgl_cam->v_org, pt1);
	V3F_ADD(pt2, lbxgl_cam->v_org, pt2);
	V3F_ADD(pt3, lbxgl_cam->v_org, pt3);

	f=((float)800)/1024;
	g=((float)600)/1024;
	h=gam;

#if 0
	glDepthMask(0);

	LBXGL_Shader_BindTexture(tex);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_BeginPoly();
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(pt0);
	LBXGL_Shader_TexCoord2f(f, 0);
	LBXGL_Shader_Vertex3fv(pt1);
	LBXGL_Shader_TexCoord2f(f, g);
	LBXGL_Shader_Vertex3fv(pt2);
	LBXGL_Shader_TexCoord2f(0, g);
	LBXGL_Shader_Vertex3fv(pt3);
	LBXGL_Shader_EndPoly();

	glDepthMask(1);
#endif

	LBXGL_Shader_BindTexture(0);

#if 1
//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	pdglColor4f(1, 1, 1, 1);
//	pdglColor4f(0.5, 0.5, 0.5, 1);

	LBXGL_Shader_BlendFunc(GL_ONE, GL_ZERO);

	pdglEnableTexture2D();
	glBindTexture(GL_TEXTURE_2D, tex);

	f=((float)800)/1024;
	g=((float)600)/1024;
	h=gam;

	glDepthMask(0);

#if 1
//	h=lbxgl_draw_gamma;
	while(h>1.0)
	{
		pdglColor4f(1, 1, 1, 1);

#if 0
		pdglBegin(PDGL_POLYGON);
		pdglTexCoord2f(0, 0);
		pdglVertex3fv(pt0);
		pdglTexCoord2f(f, 0);
		pdglVertex3fv(pt1);
		pdglTexCoord2f(f, g);
		pdglVertex3fv(pt2);
		pdglTexCoord2f(0, g);
		pdglVertex3fv(pt3);
		pdglEnd();
#endif

#if 1
		pdglBegin(PDGL_POLYGON);
		pdglTexCoord2f(0, 0);
		pdglVertex3dv(pt0);
		pdglTexCoord2f(f, 0);
		pdglVertex3dv(pt1);
		pdglTexCoord2f(f, g);
		pdglVertex3dv(pt2);
		pdglTexCoord2f(0, g);
		pdglVertex3dv(pt3);
		pdglEnd();
#endif

		h-=1.0;

		LBXGL_Shader_BlendFunc(GL_ONE, GL_ONE);
	}
#endif

	pdglColor4f(h, h, h, 1);

#if 0
	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(f, 0);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(f, g);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(0, g);
	pdglVertex3fv(pt3);
	pdglEnd();
#endif

#if 1
	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(0, 0);
	pdglVertex3dv(pt0);
	pdglTexCoord2f(f, 0);
	pdglVertex3dv(pt1);
	pdglTexCoord2f(f, g);
	pdglVertex3dv(pt2);
	pdglTexCoord2f(0, g);
	pdglVertex3dv(pt3);
	pdglEnd();
#endif

	glDepthMask(1);
#endif

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glEnable(GL_DEPTH_TEST);
//	glDepthMask(1);
}
#endif

#if 1
void LBXGL_BrushWorld_DrawScreenTexture(
	LBXGL_BrushWorld *world, int tex, float gam)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
//	double pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j, tn, xs, ys, xo, yo, txs, tys;
	float ms, ns, mt, nt;
	float f, g, h;

//	GfxDrv_GetWindowSize(&xs, &ys);
//	GfxDrv_GetWindowTexSize(&txs, &tys);

	LBXGL_Shader_GetWindowOrigin(&xo, &yo);
	LBXGL_Shader_GetWindowSize(&xs, &ys);
//	LBXGL_Shader_GetWindowTexSize(&txs, &tys);
	LBXGL_Shader_GetWindowMaxTexSize(&txs, &tys);


#ifndef GLES
	glColorMask(1,1,1,1);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif


//	glMatrixMode(GL_PROJECTION);
	pdglProjectionMatrix();
	pdglPushMatrix();

	pdglLoadIdentity();
//	pdglOrtho(0, 800, 0, 600, -99999, 99999);
	pdglOrtho(0, xs, 0, ys, -99999, 99999);

//	glMatrixMode(GL_MODELVIEW);
	pdglModelviewMatrix();
	pdglPushMatrix();

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);
//	glDisable(GL_DEPTH_TEST);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if 0
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->org, pt0);
	V3F_ADD(pt1, lbxgl_cam->org, pt1);
	V3F_ADD(pt2, lbxgl_cam->org, pt2);
	V3F_ADD(pt3, lbxgl_cam->org, pt3);
#endif

//	pt0[0]=0;		pt0[1]=0;		pt0[2]=0;
//	pt1[0]=800;		pt1[1]=0;		pt1[2]=0;
//	pt2[0]=800;		pt2[1]=600;		pt2[2]=0;
//	pt3[0]=0;		pt3[1]=600;		pt3[2]=0;

//	f=((float)800)/1024;
//	g=((float)600)/1024;
//	h=gam;

	pt0[0]=0;		pt0[1]=0;		pt0[2]=0;
	pt1[0]=xs;		pt1[1]=0;		pt1[2]=0;
	pt2[0]=xs;		pt2[1]=ys;		pt2[2]=0;
	pt3[0]=0;		pt3[1]=ys;		pt3[2]=0;

//	f=((float)xs)/txs;
//	g=((float)ys)/tys;

	ms=((float)xo)/txs;
	mt=((float)yo)/tys;
	ns=ms+((float)xs)/txs;
	nt=mt+((float)ys)/tys;
	h=gam;

//	LBXGL_Shader_BindTexture(0);
	LBXGL_Shader_BindTexture(-1);

#if 1
//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	pdglColor4f(1, 1, 1, 1);
//	pdglColor4f(0.5, 0.5, 0.5, 1);

	LBXGL_Shader_BlendFunc(GL_ONE, GL_ZERO);

	pdglEnableTexture2D();
	glBindTexture(GL_TEXTURE_2D, tex);

//	pdglDisableTexture2D();

//	f=((float)800)/1024;
//	g=((float)600)/1024;
	f=((float)xs)/txs;
	g=((float)ys)/tys;
	h=gam;

	glDepthMask(0);

#if 1
//	pdglBegin(PDGL_QUADS);
//	h=lbxgl_draw_gamma;
	while(h>1.0)
	{
		pdglColor4f(1, 1, 1, 1);

#if 1
//		pdglBegin(PDGL_POLYGON);
		pdglBegin(PDGL_QUADS);
		pdglTexCoord2f(ms, mt);
		pdglVertex3fv(pt0);
		pdglTexCoord2f(ns, mt);
		pdglVertex3fv(pt1);
		pdglTexCoord2f(ns, nt);
		pdglVertex3fv(pt2);
		pdglTexCoord2f(ms, nt);
		pdglVertex3fv(pt3);
		pdglEnd();
#endif

		h-=1.0;

		LBXGL_Shader_BlendFunc(GL_ONE, GL_ONE);
	}
//	pdglEnd();
#endif

	pdglColor4f(h, h, h, 1);

#if 1
//	pdglBegin(PDGL_POLYGON);
	pdglBegin(PDGL_QUADS);
	pdglTexCoord2f(ms, mt);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(ns, mt);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(ns, nt);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(ms, nt);
	pdglVertex3fv(pt3);
	pdglEnd();
#endif

	glDepthMask(1);
#endif

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//	glMatrixMode(GL_MODELVIEW);
	pdglModelviewMatrix();
	pdglPopMatrix();

//	glMatrixMode(GL_PROJECTION);
	pdglProjectionMatrix();
	pdglPopMatrix();
//	pdglPushMatrix();

//	glMatrixMode(GL_MODELVIEW);
	pdglModelviewMatrix();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);
}
#endif

void LBXGL_BrushWorld_DrawShaderOverlay(LBXGL_BrushWorld *world, char *name)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	int i, j, tn;

	tn=LBXGL_Texture_LoadImage(name);
	if(tn<=0)return;

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);

//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up, -1.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up, -1.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up,  1.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up,  1.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->v_org, pt0);
	V3F_ADD(pt1, lbxgl_cam->v_org, pt1);
	V3F_ADD(pt2, lbxgl_cam->v_org, pt2);
	V3F_ADD(pt3, lbxgl_cam->v_org, pt3);

	glDepthMask(0);

	LBXGL_Shader_BindTexture(tn);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Shader_BeginPoly();
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(pt0);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex3fv(pt1);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex3fv(pt2);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex3fv(pt3);
	LBXGL_Shader_EndPoly();

	glDepthMask(1);
}

void LBXGL_BrushWorld_DrawScreenShaderOverlay(
	LBXGL_BrushWorld *world, char *name,
	int tex_screen, int tex_depth, float gam)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	int i, j, tn;

	tn=LBXGL_Texture_LoadImage(name);
	if(tn<=0)return;

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);

//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up, -1.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up, -1.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up,  1.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up,  1.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->v_org, pt0);
	V3F_ADD(pt1, lbxgl_cam->v_org, pt1);
	V3F_ADD(pt2, lbxgl_cam->v_org, pt2);
	V3F_ADD(pt3, lbxgl_cam->v_org, pt3);

	glDepthMask(0);

	LBXGL_Shader_BindTexture(tn);
	LBXGL_Shader_SetUseScreen(tex_screen);
	LBXGL_Shader_SetUseDepth(tex_depth);
	
//	LBXGL_Shader_Color4f(1, 1, 1, 1);
	LBXGL_Shader_Color4f(1, 0.5, 1, sqrt(gam)-0.5);

	LBXGL_Shader_BeginPoly();
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(pt0);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex3fv(pt1);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex3fv(pt2);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex3fv(pt3);
	LBXGL_Shader_EndPoly();

	LBXGL_Shader_SetUseScreen(0);
	LBXGL_Shader_SetUseDepth(0);

	glDepthMask(1);
}


void LBXGL_BrushWorld_DrawColorOverlay(LBXGL_BrushWorld *world,
	float *rgba)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	int i, j, tn;

//	return;

//	tn=LBXGL_Texture_LoadImage(name);
//	if(tn<=0)return;

	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up, -1.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up, -1.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up,  1.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up,  1.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->v_org, pt0);
	V3F_ADD(pt1, lbxgl_cam->v_org, pt1);
	V3F_ADD(pt2, lbxgl_cam->v_org, pt2);
	V3F_ADD(pt3, lbxgl_cam->v_org, pt3);

	glDepthMask(0);
	pdglDisableTexture2D();

	pdglColor4fv(rgba);
	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(1, 0);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(1, 1);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(0, 1);
	pdglVertex3fv(pt3);
	pdglEnd();

	pdglEnableTexture2D();
	glDepthMask(1);
}

#if 0
void LBXGL_BrushWorld_DrawScreenTexture2(
	LBXGL_BrushWorld *world, int tex, float gam)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j, tn;
	float f, g, h;


	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);
//	glDisable(GL_DEPTH_TEST);

//	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->v_org, pt0);
	V3F_ADD(pt1, lbxgl_cam->v_org, pt1);
	V3F_ADD(pt2, lbxgl_cam->v_org, pt2);
	V3F_ADD(pt3, lbxgl_cam->v_org, pt3);

	LBXGL_Shader_BindTexture(0);

#if 1
	pdglEnableTexture2D();
	glBindTexture(GL_TEXTURE_2D, tex);

	glDepthMask(0);

	pdglColor4f(gam, gam, gam, 1);

	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(0, 0);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(1, 0);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(1, 1);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(0, 1);
	pdglVertex3fv(pt3);
	pdglEnd();

	glDepthMask(1);
#endif

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
#endif

#if 1
void LBXGL_BrushWorld_DrawScreenTexture2(
	LBXGL_BrushWorld *world, int tex, float gam)
{
	double pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur, *lst;
	LBXGL_Light *lcur;
	int i, j, tn;
	float f, g, h;


	glDepthFunc(GL_LEQUAL);

	glCullFace(GL_BACK);
//	glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	glStencilFunc(GL_EQUAL, 0, 255);
	glDisable(GL_STENCIL_TEST);
//	glDisable(GL_DEPTH_TEST);

//	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up, -0.75, pt0);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up, -0.75, pt1);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt,  1, lbxgl_cam->up,  0.75, pt2);
//	V3F_SCALEADDSCALE(lbxgl_cam->rt, -1, lbxgl_cam->up,  0.75, pt3);

	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up, -1.75, pt0);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up, -1.75, pt1);
	V3F_SCALEADDSCALE(lbxgl_cam->rt,  2, lbxgl_cam->up,  1.75, pt2);
	V3F_SCALEADDSCALE(lbxgl_cam->rt, -2, lbxgl_cam->up,  1.75, pt3);

	V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1, pt0);
	V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1, pt1);
	V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1, pt2);
	V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1, pt3);

	V3F_ADD(pt0, lbxgl_cam->v_org, pt0);
	V3F_ADD(pt1, lbxgl_cam->v_org, pt1);
	V3F_ADD(pt2, lbxgl_cam->v_org, pt2);
	V3F_ADD(pt3, lbxgl_cam->v_org, pt3);

	LBXGL_Shader_BindTexture(0);

#if 1
	pdglEnableTexture2D();
	glBindTexture(GL_TEXTURE_2D, tex);

	glDepthMask(0);

	pdglColor4f(gam, gam, gam, 1);

	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(0, 0);
	pdglVertex3dv(pt0);
	pdglTexCoord2f(1, 0);
	pdglVertex3dv(pt1);
	pdglTexCoord2f(1, 1);
	pdglVertex3dv(pt2);
	pdglTexCoord2f(0, 1);
	pdglVertex3dv(pt3);
	pdglEnd();

	glDepthMask(1);
#endif

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
#endif

LBXGL_API LBXGL_ModelState *LBXGL_BrushWorld_LookupModelStateEntNum(
	LBXGL_BrushWorld *world, int entnum)
{
	LBXGL_ModelState *mcur;
	
	mcur=world->mesh;
	while(mcur)
	{
		if(mcur->entnum==entnum)
			return(mcur);
		mcur=mcur->next;
	}
	return(NULL);
}

LBXGL_API LBXGL_ModelState *LBXGL_BrushWorld_LookupModelStateMdlNum(
	LBXGL_BrushWorld *world, int mdlnum)
{
	LBXGL_ModelState *mcur;
	LBXGL_BrushModel *bmcur;
	
	mcur=world->mesh;
	while(mcur)
	{
//		if(dytypep(mcur->mdl, "lbxgl_brushmodel_t"))
		if(mcur->mdltype && !strcmp(mcur->mdltype, "lbxgl_brushmodel_t"))
		{
			bmcur=mcur->mdl;
			if(bmcur->mdlnum==mdlnum)
				return(mcur);
		}
		mcur=mcur->next;
	}
	return(NULL);
}

LBXGL_API LBXGL_ModelState *LBXGL_BrushWorld_GetModelStateEntNum(
	LBXGL_BrushWorld *world, int entnum)
{
	LBXGL_ModelState *mcur;
	
	mcur=LBXGL_BrushWorld_LookupModelStateEntNum(world, entnum);
	if(mcur)return(mcur);
	
	mcur=LBXGL_Mdl_WrapModelInstance(NULL);
	mcur->next=world->mesh;
	world->mesh=mcur;
	mcur->entnum=entnum;
	
	return(mcur);
}

LBXGL_API void LBXGL_BrushWorld_AddModelStateEntNum(
	LBXGL_BrushWorld *world, LBXGL_ModelState *mdl, int entnum)
{
	if(!mdl)return;

	mdl->entnum=entnum;
	mdl->next=world->mesh;
	world->mesh=mdl;
}

LBXGL_API int LBXGL_BrushWorld_DeleteModelStateEntNum(
	LBXGL_BrushWorld *world, int entnum)
{
	LBXGL_ModelState *mcur, *mlst, *mnxt;
	
	mcur=world->mesh; mlst=NULL;
	while(mcur)
	{
		if(mcur->entnum==entnum)
		{
			if(mlst)
				{ mlst->next=mcur->next; }
			else world->mesh=mcur->next;
			mnxt=mcur->next;
			LBXGL_Mdl_FreeModel(mcur);
			mcur=mnxt;
			continue;
		}
		mlst=mcur;
		mcur=mcur->next;
	}
	return(0);
}
