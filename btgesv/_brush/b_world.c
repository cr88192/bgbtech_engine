#include <btgesv.h>

BTGE_API BTGE_BrushWorld *BTGE_Brush_NewWorld()
{
	BTGE_BrushWorld *tmp;

//	BTGE_InitCam();

	tmp=gctalloc("btge_brush_world_t", sizeof(BTGE_BrushWorld));

//	tmp->shadows=48;
//	tmp->default_tex="aardo/aardmbr64_1";

	return(tmp);
}

BTGE_API void BTGE_Brush_FreeWorld(BTGE_BrushWorld *world)
{
	BTGE_BrushWorld_DrawFlush(world);
	BTGE_WorldTeardown(world);
	BTGE_BrushWorld_Teardown(world);

	gcfree(world);
}

BTGE_API BTGE_SEntity *BTGE_BrushWorld_FindSEnt(
	BTGE_BrushWorld *world, BTGE_SEntity *last, char *key, char *val)
{
	BTGE_SEntity *cur;
	char *s;

	if(last)cur=last->next;
		else cur=world->entity;
	while(cur)
	{
		s=BTGE_SEnt_GetStr(cur, key);

		if(!s && !val)return(cur);
		if(s && val && !strcmp(s, val))return(cur);

		cur=cur->next;
	}

	return(NULL);
}

BTGE_API void BTGE_BrushWorld_AddBrush(
	BTGE_BrushWorld *world, BTGE_Brush *brsh)
{
	BTGE_Brush *cur;
	BTGE_SEntity *ent;

	if(world->brush)
	{
		cur=world->brush;
		while(cur->next)cur=cur->next;
		cur->next=brsh;
	}else world->brush=brsh;

	if(world->entity_psel)
	{
		brsh->se_owner=world->entity_psel;

		if(world->entity_psel->brush)
		{
			cur=world->entity_psel->brush;
			while(cur->enext)cur=cur->enext;
			cur->enext=brsh;
		}else world->entity_psel->brush=brsh;
	}else
	{
		ent=BTGE_BrushWorld_FindSEnt(world, NULL,
			"classname", "worldspawn");
		if(ent)
		{
			brsh->se_owner=ent;

			if(ent->brush)
			{
				cur=ent->brush;
				while(cur->enext)cur=cur->enext;
				cur->enext=brsh;
			}else ent->brush=brsh;
		}
	}
}

#if 0
// BTGE_API BTGE_SEntity *BTGE_BrushWorld_NewBrushEntity(
	BTGE_BrushWorld *world, char *type)
{
	float pt[3];
	BTGE_SEntity *tmp;
	BTGE_Brush *brsh;

	tmp=gctalloc("btge_sentity_s", sizeof(BTGE_SEntity));
	BTGE_SEnt_SetStr(tmp, "classname", type);
	BTGE_SEnt_AddEntity(&world->entity, tmp);

	world->entity_psel=tmp;

//	V3F_ADDSCALE(btge_cam->org, btge_cam->fw, 24, pt);
	brsh=BTGE_Brush_MakeCube(pt[0], pt[1], pt[2], 6, 6, 6,
		world->default_tex);
	BTGE_BrushWorld_AddBrush(world, brsh);

	return(tmp);
}
#endif

#if 0
// BTGE_API void BTGE_BrushWorld_UpdateModels(BTGE_BrushWorld *world)
{
	BTGE_SEntity *ecur;
	char *s;

	ecur=world->entity; world->mesh=NULL;
	while(ecur)
	{
		if(ecur->mdl)
		{
			s=BTGE_SEnt_GetStr(ecur, "classname");
//			printf("Model for %s\n", s);

			ecur->mdl->next=world->mesh;
			world->mesh=ecur->mdl;
		}

		ecur=ecur->next;
	}
}
#endif

BTGE_API void BTGE_BrushWorld_DrawBuildFast(BTGE_BrushWorld *world)
{
	BTGE_Brush *cur, *lst;
	BTGE_BrushModel *mcur, *mlst;
	BTGE_SEntity *ecur;
	int num;
	char *s;

	if(world->bsp)
		return;

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

				mcur->mdlnum=num++;
				BTGE_SEnt_SetNum(ecur, "model", mcur->mdlnum);

				ecur=ecur->next;
				continue;
			}


			cur=ecur->brush; lst=NULL;
			while(cur)
			{
				if(!cur->xyz)
					BTGE_Brush_BuildFaces(cur);
				cur->chain=lst; lst=cur;
				cur=cur->enext;
			}

			mcur=BTGE_BrushBSP_BuildBrushModelListFast(lst);
			ecur->bmdl=mcur;
			mcur->sent=ecur;

			mcur->mdlnum=num++;
			BTGE_SEnt_SetNum(ecur, "model", mcur->mdlnum);

			mcur->next=mlst;
			mlst=mcur;

			s=BTGE_SEnt_GetStr(ecur, "classname");
			if(!strcmp(s, "worldspawn"))
			{
				world->bsp=mcur->bsp;
			}else
			{
//				ecur->mdl=BTGE_Mdl_WrapModelInstance(mcur);
//				mcur->mesh=ecur->mdl;

//				BTGE_SEnt_GetVec3(ecur, "origin", mcur->org);

//				printf("BMdl @ %f %f %f\n",
//					mcur->org[0], mcur->org[1], mcur->org[2]);
			}
		}

		ecur=ecur->next;
	}

	world->bmdl=mlst;
}

BTGE_API void BTGE_BrushWorld_DrawBuild(BTGE_BrushWorld *world)
{
	BTGE_Brush *cur, *lst;

#if 0
	BTGE_BrushWorld_DrawFlush(world);

	cur=world->brush; lst=NULL;
	while(cur)
	{
		if(!cur->xyz)BTGE_Brush_BuildFaces(cur);
		cur->chain=lst; lst=cur; cur=cur->next;
	}

	world->bsp=BTGE_BrushBSP_BuildTreeList(lst);
#endif
}

BTGE_API void BTGE_BrushWorld_DrawFlush(BTGE_BrushWorld *world)
{
	BTGE_Brush *cur, *lst;
	BTGE_BrushModel *mcur, *mlst;
	BTGE_SEntity *ecur;
	char *s;

	if(!world->bsp)
		return;

//	BTGE_BrushPhys_UnlinkWorld(world);
//	BTGE_BrushGround_FlushGround(world);

	ecur=world->entity;
	while(ecur)
	{
		if(ecur->bmdl)
		{
			BTGE_BrushBSP_FreeBrushModel(ecur->bmdl);
			ecur->bmdl=NULL;

//			if(ecur->mdl)
//			{
//				BTGE_Mdl_FreeModel(ecur->mdl);
//				ecur->mdl=NULL;
//			}
		}
		ecur=ecur->next;
	}

	world->portal=NULL;
	world->bmdl=NULL;
	world->bsp=NULL;

#if 0
	if(world->bsp)
	{
		BTGE_BrushBSP_FreeTreeNode(world->bsp);
		world->bsp=NULL;
	}
#endif
}

BTGE_API void BTGE_BrushWorld_Teardown(BTGE_BrushWorld *world)
{
	BTGE_Brush *cur, *lst, *nxt;
	BTGE_BrushModel *mcur, *mlst;
	BTGE_SEntity *ecur, *enxt;

	BTGE_BrushWorld_DrawFlush(world);
	
	cur=world->brush;
	while(cur)
	{
		nxt=cur->next;
		BTGE_Brush_DestroyBrush(cur);
		cur=nxt;
	}
	world->brush=NULL;
	
	ecur=world->entity;
	while(ecur)
	{
		enxt=ecur->next;
		BTGE_SEnt_DestroyEntity(ecur);
		ecur=enxt;
	}
	world->entity=NULL;

	world->light=NULL;

	if(world->light_mask)
		gcfree(world->light_mask);
	world->light_mask=NULL;
}

#if 0
// void BTGE_BrushWorld_UpdateBrushFace(
	BTGE_BrushWorld *world, BTGE_Brush *brush, int id)
{
	char buf[256];

	float tv0[3], tv1[3], tv2[3];
	float org[3];
	float sx, sy, f, g;
	int tn, xs, ys;
	int i, j, k, l, t;

	V3F_COPY(btge_cam->morg, tv0);
	V3F_COPY(btge_cam->mend, tv1);

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
		glColor4f(0, 0, 1, 1);

		f=(V3F_DOT(org, btge_cam->fw)-
			V3F_DOT(btge_cam->org, btge_cam->fw))/64;
		g=Vec3F_DistLineSeg(org, btge_cam->morg, btge_cam->mend);
		if(g<f)glColor4f(1, 0, 1, 1);

		if(id==world->brush_sel_face)
		{
			glColor4f(0, 1, 0, 1);

			if(btge_state->mb&1)
			{
				f=V3F_DOT(org, btge_cam->fw)-
					V3F_DOT(btge_cam->org, btge_cam->fw);
				V3F_SCALEADDSCALE(
					btge_cam->rt, f*btge_state->dmx/400.0,
					btge_cam->up, f*(-btge_state->dmy)/400.0, tv0);

				V3F_SUB(btge_cam->org, btge_cam->lorg, tv1);
				V3F_ADD(tv0, tv1, tv0);

				g=V3F_DOT(tv0, brush->norm+id*4);
				brush->norm[id*4+3]+=g;

				BTGE_Brush_DestroyFaces(brush);
				BTGE_BrushWorld_DrawFlush(world);
			}
		}

#if 1
//		if(V3F_NDOT(btge_cam->org, brush->norm+id*4)>=0)
		if(1)
		{
			f=(V3F_DOT(org, btge_cam->fw)-
				V3F_DOT(btge_cam->org, btge_cam->fw))/64;

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_STENCIL_TEST);

			glBegin(GL_POLYGON);

			V3F_ADDSCALEADDSCALE(org, btge_cam->rt, -f,
				btge_cam->up, -f, tv0);
			glVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, btge_cam->rt, f,
				btge_cam->up, -f, tv0);
			glVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, btge_cam->rt, f,
				btge_cam->up, f, tv0);
			glVertex3fv(tv0);

			V3F_ADDSCALEADDSCALE(org, btge_cam->rt, -f,
				btge_cam->up, f, tv0);
			glVertex3fv(tv0);

			glEnd();

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_TEXTURE_2D);
		}
#endif
	}
}
#endif

BTGE_API BTGE_Brush *BTGE_BrushWorld_TraceLineBrush(BTGE_BrushWorld *world,
	float *s, float *e, BTGE_Brush *ignore)
{
	BTGE_Brush *bcur;

	bcur=world->brush;
	while(bcur)
	{
		if(bcur==ignore)
		{
			bcur=bcur->next;
			continue;
		}

		if(BTGE_Brush_CheckLineBrush(bcur, s, e))
			return(bcur);
		bcur=bcur->next;
	}

	return(NULL);
}
