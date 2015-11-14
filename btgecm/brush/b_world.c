#include <btgecm.h>

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
//	BTGE_WorldTeardown(world);
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

#if 0
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
#endif

#if 1
BTGE_API void BTGE_BrushWorld_AddEntityBrush(
	BTGE_BrushWorld *world, BTGE_SEntity *ent, BTGE_Brush *brsh)
{
	BTGE_Brush *cur;

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

BTGE_API void BTGE_BrushWorld_AddBrush(
	BTGE_BrushWorld *world, BTGE_Brush *brsh)
{
	BTGE_Brush *cur;
	BTGE_SEntity *ent;

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
		BTGE_BrushWorld_AddEntityBrush(world,
			world->entity_psel, brsh);
	}else
	{
		ent=BTGE_BrushWorld_FindSEnt(world, NULL,
			"classname", "worldspawn");
		if(ent)
		{
			BTGE_BrushWorld_AddEntityBrush(world, ent, brsh);
		}
	}
}
#endif

BTGE_API void BTGE_BrushWorld_AddBrushChainList(
	BTGE_BrushWorld *world, BTGE_Brush *brsh)
{
	if(!brsh)return;
	BTGE_BrushWorld_AddBrush(world, brsh);
	BTGE_BrushWorld_AddBrushChainList(world, brsh->chain);
}

BTGE_API void BTGE_BrushWorld_AddBrushChainListTransient(
	BTGE_BrushWorld *world, BTGE_Brush *brsh)
{
	if(!brsh)return;
	brsh->flags|=BTGE_BRFL_TRANSIENT;
	BTGE_BrushWorld_AddBrush(world, brsh);
	BTGE_BrushWorld_AddBrushChainListTransient(world, brsh->chain);
}

BTGE_API void BTGE_BrushWorld_BrushSpawnEntity(
	BTGE_BrushWorld *world, BTGE_Brush *brsh)
{
	char tb[256];
	BTGE_SEntity *ent, *ent2;
	void *ptr;
	char *s;

	ent2=world->entity_psel;
	world->entity_psel=brsh->se_owner;

	ent=brsh->se_child;
	s=BTGE_SEnt_GetStr(ent, "classname");
	if(!s)return;

	sprintf(tb, "BtSb_%s", s);
	ptr=dyllGetAddr(tb);

	if(ptr)
	{
		((void (*)(btWorld, BTGE_Brush *))ptr)(world, brsh);
		world->entity_psel=ent2;
		return;
	}

	sprintf(tb, "BtSg_%s", s);
	ptr=dyllGetAddr(tb);

	if(ptr)
	{
		((void (*)(btWorld, btSEntity))ptr)(world, ent);
		world->entity_psel=ent2;
		return;
	}
	
	world->entity_psel=ent2;
}

BTGE_API void BTGE_BrushWorld_SpawnEntity(
	BTGE_BrushWorld *world, BTGE_SEntity *ent)
{
	char tb[256];
	BTGE_SEntity *ent2;
	void *ptr;
	char *s;

	ent2=world->entity_psel;
	world->entity_psel=ent;

	s=BTGE_SEnt_GetStr(ent, "classname");
	if(!s)return;

	sprintf(tb, "BtSg_%s", s);
	ptr=dyllGetAddr(tb);

	if(ptr)
	{
		((void (*)(btWorld, btSEntity))ptr)(world, ent);
		world->entity_psel=ent2;
		return;
	}
	
	world->entity_psel=ent2;
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

//				mcur->mdlnum=num++;
				mcur->mdlnum=ecur->idnum;
				BTGE_SEnt_SetNum(ecur, "model", mcur->mdlnum);

				ecur=ecur->next;
				continue;
			}


			cur=ecur->brush; lst=NULL;
			while(cur)
			{
				if(cur->flags&BTGE_BRFL_DEL)
					{ cur=cur->enext; continue; }
				if(cur->se_child)
				{
					BTGE_BrushWorld_BrushSpawnEntity(world, cur);
					if(!cur->patch && !cur->mesh && !cur->n_face)
						{ cur=cur->enext; continue; }
				}
				if(!cur->xyz)
					BTGE_Brush_BuildFaces(cur);
				cur->chain=lst; lst=cur;
				cur=cur->enext;
			}

			mcur=BTGE_BrushBSP_BuildBrushModelListFast(lst);
			ecur->bmdl=mcur;
			mcur->sent=ecur;

//			mcur->mdlnum=num++;
			mcur->mdlnum=ecur->idnum;

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

		BTGE_BrushWorld_SpawnEntity(world, ecur);
		ecur=ecur->next;
	}

	world->bmdl=mlst;
}

BTGE_API void BTGE_BrushWorld_DrawBuild(BTGE_BrushWorld *world)
{
	BTGE_Brush *cur, *lst;

	BTGE_BrushWorld_DrawBuildFast(world);

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

BTGE_API void BTGE_BrushWorld_DrawFlushBrush(
	BTGE_BrushWorld *world, BTGE_Brush *brush)
{
	BTGE_BrushNode *ncur, *pncur;
	float f, g;

	brush->flags|=BTGE_BRFL_DIRTY;
	BTGE_Brush_DestroyFaces(brush);
//	BTGE_BrushWorld_DrawFlush(world);

	//determine if brush crosses any parent-node planes
	ncur=brush->node; pncur=NULL;
	while(ncur)
	{
		if(ncur!=brush->node)
		{
			//crosses a higher-level plane
//			BTGE_Brush_GetPlaneExtents(brush, ncur->norm, &f, &g);
			BTGE_Brush_GetBBoxPlaneExtents(brush, ncur->norm, &f, &g);
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
	if(ncur) { BTGE_BrushWorld_DrawFlush(world); }
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

#if 1
	//mark any transient brushes as deleted
	cur=world->brush;
	while(cur)
	{
		if(cur->flags&BTGE_BRFL_DEL)
			{ cur=cur->next; continue; }
		if(cur->flags&BTGE_BRFL_TRANSIENT)
			{ cur->flags|=BTGE_BRFL_DEL|BTGE_BRFL_DIRTY; }
		cur=cur->next;
	}
#endif

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

BTGE_API BTGE_Brush *BTGE_BrushWorld_LookupBrush(
	BTGE_BrushWorld *world, int id)
{
	BTGE_Brush *bcur;

	bcur=world->brush;
	while(bcur)
	{
		if(bcur->idnum==id)
			return(bcur);
		bcur=bcur->next;
	}

	return(NULL);
}

BTGE_API BTGE_SEntity *BTGE_BrushWorld_LookupSEntityId(
	BTGE_BrushWorld *world, int id)
{
	BTGE_SEntity *scur;

	scur=world->entity;
	while(scur)
	{
		if(scur->idnum==id)
			return(scur);
		scur=scur->next;
	}

	return(NULL);
}

BTGE_API BTGE_SEntity *BTGE_BrushWorld_GetSEntityId(
	BTGE_BrushWorld *world, int id)
{
	BTGE_SEntity *scur;

	scur=BTGE_BrushWorld_LookupSEntityId(world, id);
	if(scur)return(scur);

	scur=gctalloc("lbxgl_sentity_s", sizeof(BTGE_SEntity));
	BTGE_SEnt_AddEntity(&world->entity, scur);
	scur->wrl=world;
	scur->idnum=id;

	return(scur);
}

BTGE_API BTGE_Brush *BTGE_BrushWorld_CreateBrushId(
	BTGE_BrushWorld *world, int sentid, int brushid)
{
	BTGE_SEntity *scur;
	BTGE_Brush *bcur;
	
	BTGE_BrushWorld_DrawFlush(world);
	
//	scur=BTGE_BrushWorld_LookupSEntityId(world, sentid);
	scur=BTGE_BrushWorld_GetSEntityId(world, sentid);
	
	if(!scur)
	{
		printf("BTGE_BrushWorld_CreateBrushId: No Entity %d\n", sentid);
		return(NULL);
	}
	
	bcur=BTGE_Brush_New(8);
	bcur->idnum=brushid;
	bcur->n_face=0;
	
	world->entity_psel=scur;
	BTGE_BrushWorld_AddBrush(world, bcur);
//	BTGE_BrushWorld_AddEntityBrush(world, scur, bcur);
	
	return(bcur);
}
