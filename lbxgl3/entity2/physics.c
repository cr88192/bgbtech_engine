/*
Do world physics via use of BSDE.

The entities will see physics in terms of settable properies, movetype, and
solidtype. Altering physics related properties will change the state of
attached solids, and solids may be created or destroyed in some cases.

For many solidtypes, AABBs will be the default primitive type.
*/

#include <lbxgl2.h>

int LBXGL_Physics_SpawnEntity(LBXGL_World *wrl, LBXGL_Entity *ent)
{
	char *s;
	void *p;
	int i;

//	s=LBXGL_Entity_GetPropertyString(ent, "classname");
	s=EntGetString(ent, "classname");
	if(!s)return(-1);

	p=PDGL_GetPtr(s);
	if(!p)
	{
		printf("Failed Spawn Entity '%s'\n", s);
		return(-1);
	}

//	printf("Call %p\n", p);

	i=((int (*)(LBXGL_Entity *ent))p)(ent);
	return(i);
}

int LBXGL_Physics_SpawnWorld(LBXGL_World *wrl)
{
	LBXGL_Entity *ecur;
	BSP_Model *mcur;
	float fv[3];
	int *ta;
	char *s, *t, *s2, *t2;
	int i;

	EntInit();	//forces linker inclusion of some things

	wrl->id=bsdeNewWorld();

	ta=malloc(4096*2*sizeof(int));
	bsdeSetTriggerArray(ta, 4096);

	bsdeSetWorldAttrI(BSDE_UNIT_LENGTH, BSDE_UNIT_INCH);
	bsdeSetWorldAttrI(BSDE_UNIT_MASS, BSDE_UNIT_POUND_M);
	bsdeSetWorldAttrI(BSDE_UNIT_FORCE, BSDE_UNIT_POUND);
	bsdeSetWorldAttrI(BSDE_UNIT_VOLUME, BSDE_UNIT_FOOT);

//	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -9.8), 3);
//	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -2), 3);
	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -16*12), 3);
	bsdeSetWorldAttrF(BSDE_MINSTEP, 0.005);
	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.01);
//	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.03);
	bsdeSetWorldAttrF(BSDE_MAXFRAME, 0.1);

//	bsdeSetWorldAttrI(BSDE_COLLIDE_STRICT, 1);

	mcur=wrl->mdls;
	while(mcur)
	{
		bsdeBegin(BSDE_SOLID_BSP);
		BSP_SendNode(mcur->root);
		i=bsdeEnd();
		mcur->solid=i;

		if(mcur->mdlnum)
		{
			//generic bmodel
//			bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_IN3);
			bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_FT3);
			bsdeSetAttrI(i, BSDE_SEMISTATIC, 1);
			bsdeSetAttrI(i, BSDE_NONSOLID, 1);
		}else
		{
			//world model
//			bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_IN3);
			bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_FT3);
			bsdeSetAttrI(i, BSDE_STATIC, 1);
			bsdeSetAttrI(i, BSDE_NONSOLID, 0);
		}

		mcur=mcur->next;
	}

	ecur=wrl->ents;
	while(ecur)
	{
		t=LBXGL_Entity_GetPropertyString(ecur, "classname");
		if(!strcmp(t, "worldspawn"))
		{
			LBXGL_Entity_SetPropertyString(ecur, "solidtype",
				"solid_prim");
			LBXGL_Entity_SetPropertyString(ecur, "movetype",
				"move_push");

			LBXGL_Entity_GetPropertyFVector(ecur, "absmin", fv, 3);
			LBXGL_Entity_SetPropertyFVector(ecur, "mins", fv, 3);

			LBXGL_Entity_GetPropertyFVector(ecur, "absmax", fv, 3);
			LBXGL_Entity_SetPropertyFVector(ecur, "maxs", fv, 3);

			s2=LBXGL_Entity_GetPropertyString(ecur, "skybox");
			if(s2)LBXGL_SkyBox_LoadSky(s2);

			s2=LBXGL_Entity_GetPropertyString(ecur, "skybox2");
			if(s2)LBXGL_SkyBox_LoadSkyCS(s2);

			s2=LBXGL_Entity_GetPropertyString(ecur, "skydome");
			if(s2)LBXGL_SkyDome_LoadSky(s2);

			i=LBXGL_Entity_GetPropertyFVector(
				ecur, "gravity", fv, 3);
			if(i>=0)bsdeSetWorldAttrFV(BSDE_GRAVITY, fv, 3);

			//world is always present and solid
			i=ecur->bmdl->solid;
			bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_IN3);
			bsdeSetAttrI(i, BSDE_SEMISTATIC, 0);
			bsdeSetAttrI(i, BSDE_NONSOLID, 0);
			bsdeSetAttrI(i, BSDE_STATIC, 1);
		}

//		LBXGL_EntClass_SpawnEnt(ecur);
//		LBXGL_BS1IF_SpawnEnt(ecur);
		LBXGL_Physics_SpawnEntity(wrl, ecur);
		ecur=ecur->next;
	}

	return(0);
}

void LBXGL_Physics_SendEnts(LBXGL_World *ctx)
{
	float tv[9], v0[3], v1[3];
	LBXGL_Entity *cur;
	BSP_Model *bmdl;
	char *s, *t, *st, *mt;
	float f, g;
	int i, j;

	cur=ctx->ents;
	while(cur)
	{
//		s=cur->base->solidtype;
//		t=cur->lastbase->solidtype;

		s=LBXGL_Entity_GetPropertyString(cur, "solidtype");
		i=LBXGL_Entity_GetPropertyFlag(cur, "solidtype");
		LBXGL_Entity_SetPropertyFlag(cur, "solidtype", i&(~1));

		st=s;

		if(s && (i&1))	//solidtype changed
		{
			t=LBXGL_Entity_GetPropertyString(cur, "oldsolidtype");

			i=LBXGL_Entity_GetPropertyFloat(cur, "_solid");
			if(i>0)
			{
				bsdeHide(i);
//				bsdeDestroy(i);
				LBXGL_Entity_SetPropertyFloat(cur, "_solid", -1);
			}

			if(t && !strcmp(s, t))
			{
				bsdeUnhide(i);
			}else if(!strcmp(s, "solid_not"))
			{
			}else if(!strcmp(s, "solid_prim"))
			{
				bmdl=LBXGL_Entity_GetProperty(cur, "mdl");
				i=bmdl->solid;
				bsdeUnhide(i);
				LBXGL_Entity_SetPropertyFloat(cur, "_solid", i);
			}else if(!strcmp(s, "solid_bbox") ||
				!strcmp(s, "solid_trigger"))
			{
				if(i>0)bsdeDestroy(i);

				EntGet3FV(cur, "mins", v0);
				EntGet3FV(cur, "maxs", v1);
				i=bsdeNewAABB(v0[0], v0[1], v0[2],
					v1[0], v1[1], v1[2]);
				LBXGL_Entity_SetPropertyFloat(cur, "_solid", i);

				j=!strcmp(s, "solid_trigger");
				bsdeSetAttrI(i, BSDE_NONSOLID, j);
//				bsdeSetAttrF(i, BSDE_DENSITY,
//					LBXGL_DENSITY_LB_IN3);
				bsdeSetAttrF(i, BSDE_DENSITY,
					LBXGL_DENSITY_LB_FT3);
			}else if(!strcmp(s, "solid_obb"))
			{
				if(i>0)bsdeDestroy(i);

				EntGet3FV(cur, "mins", v0);
				EntGet3FV(cur, "maxs", v1);
				i=bsdeNewOBB(v0[0], v0[1], v0[2],
					v1[0], v1[1], v1[2]);
				LBXGL_Entity_SetPropertyFloat(cur, "_solid", i);
//				bsdeSetAttrF(i, BSDE_DENSITY,
//					LBXGL_DENSITY_LB_IN3);
				bsdeSetAttrF(i, BSDE_DENSITY,
					LBXGL_DENSITY_LB_FT3);
				bsdeSetAttrI(i, BSDE_NONSOLID, 0);
			}else if(!strcmp(s, "solid_sphere"))
			{
				if(i>0)bsdeDestroy(i);

				f=EntGetFloat(cur, "radius");
				i=bsdeNewSphere(f);
				LBXGL_Entity_SetPropertyFloat(cur, "_solid", i);
				bsdeSetAttrF(i, BSDE_DENSITY,
					LBXGL_DENSITY_LB_FT3);
				bsdeSetAttrI(i, BSDE_NONSOLID, 0);
			}else if(!strcmp(s, "solid_cylinder"))
			{
				if(i>0)bsdeDestroy(i);

				f=EntGetFloat(cur, "height");
				g=EntGetFloat(cur, "radius");
				i=bsdeNewCylinder(f, g);
				LBXGL_Entity_SetPropertyFloat(cur, "_solid", i);
				bsdeSetAttrF(i, BSDE_DENSITY,
					LBXGL_DENSITY_LB_FT3);
				bsdeSetAttrI(i, BSDE_NONSOLID, 0);
			}else if(!strcmp(s, "solid_capped_cylinder"))
			{
				if(i>0)bsdeDestroy(i);

				f=EntGetFloat(cur, "height");
				g=EntGetFloat(cur, "radius");
				i=bsdeNewCappedCylinder(f, g);
				LBXGL_Entity_SetPropertyFloat(cur, "_solid", i);
				bsdeSetAttrF(i, BSDE_DENSITY,
					LBXGL_DENSITY_LB_FT3);
				bsdeSetAttrI(i, BSDE_NONSOLID, 0);
			}
		}

		i=LBXGL_Entity_GetPropertyFloat(cur, "_solid");
		if(i<=0)
		{
			cur=cur->next;
			continue;
		}

		t=LBXGL_Entity_GetPropertyString(cur, "movetype");
		mt=t;

		if(t)
		{
			if(!strcmp(t, "move_none"))
			{
				bsdeSetAttrI(i, BSDE_STATIC, 1);
			}

			if(!strcmp(t, "move_walk"))
			{
				bsdeSetAttrI(i, BSDE_STATIC, 0);
			}

			if(!strcmp(t, "move_toss"))
			{
				bsdeSetAttrI(i, BSDE_STATIC, 0);
			}
			if(!strcmp(t, "move_bounce"))
			{
				bsdeSetAttrI(i, BSDE_STATIC, 0);
				bsdeSetAttrF(i, BSDE_ELASTIC, 5);
			}

			if(!strcmp(t, "move_fly"))
			{
				bsdeSetAttrI(i, BSDE_STATIC, 0);
				bsdeSetAttrI(i, BSDE_NOGRAVITY, 1);
			}
		}


		s=LBXGL_Entity_GetProperty(cur, "touch");
		bsdeSetAttrI(i, BSDE_TRIGGER, s?BSDE_TRUE:BSDE_FALSE);

		if(EntIsDirty(cur, "origin"))
		{
			LBXGL_Entity_GetPropertyFVector(cur, "origin", tv, 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, tv, 3);
		}

		if(EntIsDirty(cur, "velocity"))
		{
			LBXGL_Entity_GetPropertyFVector(cur, "velocity", tv, 3);
			bsdeSetAttrFV(i, BSDE_VELOCITY, tv, 3);
		}

		if(EntIsDirty(cur, "rot"))
		{
			LBXGL_Entity_GetPropertyFVector(cur, "rot", tv, 9);
			bsdeSetAttrFV(i, BSDE_ROTATION_MATRIX, tv, 9);
		}

		cur=cur->next;
	}
}

void LBXGL_Physics_RecieveEnts(LBXGL_World *ctx)
{
	float org[3], tv[9], v0[3];
	LBXGL_Entity *cur;
	float d, f, lv, g;
	char *s;
	int i, j;

	cur=ctx->ents;
	while(cur)
	{
		i=LBXGL_Entity_GetPropertyFloat(cur, "_solid");
		if(i<=0)
		{
			cur=cur->next;
			continue;
		}

		bsdeGetAttrFV(i, BSDE_ORIGIN, org, 3);
		LBXGL_Entity_SetPropertyFVector2(cur, "origin", org, 3);

		bsdeGetAttrFV(i, BSDE_VELOCITY, tv, 3);
		LBXGL_Entity_SetPropertyFVector2(cur, "velocity", tv, 3);

		bsdeGetAttrFV(i, BSDE_ROTATION_MATRIX, tv, 9);
		LBXGL_Entity_SetPropertyFVector2(cur, "rot", tv, 9);

		s=LBXGL_Entity_GetPropertyString(cur, "solidtype");
		if(strcmp(s, "solid_bbox") &&
			strcmp(s, "solid_trigger"))
		{
			Vec3F_Mat3ToEuler(tv, v0);
			LBXGL_Entity_SetPropertyFVector2(cur, "angles", v0, 3);
		}

		cur=cur->next;
	}
}

void LBXGL_Physics_EntThinkFrame(LBXGL_World *ctx, float dt)
{
	float tv[9];
	LBXGL_Entity *cur;
	LBXGL_Light *dl;
	void *p;
	float d, f, lv, g;
	int i, j;

	cur=ctx->ents;
	while(cur)
	{
		p=EntSendMessage0(cur, "frame_think");
		if(p==UNDEFINED)
		{
			p=LBXGL_Entity_GetProperty(cur, "frame_think");
			if(p)	((int (*)(LBXGL_Entity *self, float dt))p)(cur, dt);
		}

		f=LBXGL_Entity_GetPropertyFloat(cur, "thinktime");
		if(f>=0)
		{
			f-=dt;
			if(f<=0)
			{
				LBXGL_Entity_SetPropertyFloat(cur, "thinktime", f);
				p=EntSendMessage0(cur, "think");
				if(p==UNDEFINED)
				{
					p=LBXGL_Entity_GetProperty(cur, "think");
					if(p)	((int (*)(LBXGL_Entity *self))p)
							(cur);
				}
				f=-1;
			}else
			{
				LBXGL_Entity_SetPropertyFloat(cur,
					"thinktime", f);
			}
		}

//		LBXGL_Entity_SetPropertyFVector(cur, "origin", tv, 3);
//		LBXGL_Entity_SetPropertyFVector(cur, "rot", tv, 9);

		dl=LBXGL_Entity_GetProperty(cur, "_dlight");
		if(dl)
		{
			LBXGL_Entity_GetPropertyFVector(cur, "origin", tv, 3);
			if(V3F_DIST(tv, dl->org)>8)
			{
				dl->flags|=LBXGL_LFL_CHANGED;
				V3F_COPY(tv, dl->org);
			}
			dl->leaf=BSP_PointLeafNode(ctx->mdls->root, dl->org);
		}

		cur=cur->next;
	}
}


LBXGL_Entity *LBXGL_Physics_GetEntitySolid(LBXGL_World *ctx, int hdl)
{
	LBXGL_Entity *cur;
	int i;

	cur=ctx->ents;
	while(cur)
	{
		i=LBXGL_Entity_GetPropertyFloat(cur, "_solid");
		if(i==hdl)return(cur);

		cur=cur->next;
	}
	return(NULL);
}

void LBXGL_Physics_EntTriggerFrame(LBXGL_World *ctx)
{
	LBXGL_Entity *e0, *e1;
	void *p;
	int *ta;
	int i, j, n;

	ta=bsdeGetTriggerArray();
	n=bsdeGetTriggerArrayPos();

	if(!ta)return;
	if(!n)return;

	for(i=0; i<n; i++)
	{
		e0=LBXGL_Physics_GetEntitySolid(ctx, ta[i*2+0]);
		e1=LBXGL_Physics_GetEntitySolid(ctx, ta[i*2+1]);

		p=EntSendMessage1(e0, "touch", e1);
		if(p==UNDEFINED)
		{
			p=LBXGL_Entity_GetProperty(e0, "touch");
			if(p)j=((int (*)(LBXGL_Entity *self, LBXGL_Entity *other))p)
				(e0, e1);
		}
	}

	bsdeResetTriggerArray();
}

int LBXGL_Physics_StepWorld(LBXGL_World *wrl, float dt)
{
	bsdeBindWorld(wrl->id);

	LBXGL_Physics_SendEnts(wrl);
	bsdeStepWorld(dt);
	LBXGL_Physics_RecieveEnts(wrl);
	BSP_RelinkEnts(wrl);

	wrl->time+=dt;
	LBXGL_Physics_EntThinkFrame(wrl, dt);
	LBXGL_Physics_EntTriggerFrame(wrl);

	return(0);
}

int LBXGL_Physics_SetSolidType(LBXGL_Entity *ent, char *str)
{
	char *s;

	if(!strcmp(str, "solid_not"))
	{
		s=EntGetString(ent, "solidtype");
		EntSetString(ent, "oldsolidtype", s);
	}else EntSetString(ent, "oldsolidtype", NULL);

	LBXGL_Entity_SetPropertyString(ent, "solidtype", str);
}

int LBXGL_Physics_SetMoveType(LBXGL_Entity *ent, char *str)
{
	LBXGL_Entity_SetPropertyString(ent, "movetype", str);
}
