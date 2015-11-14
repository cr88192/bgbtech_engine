#include <lbxgl2.h>
#include <pdglui.h>

#include <libbsde.h>

LBXGL_API void LBXGL_EntPhys_SendEnt(lbxWorld *world, lbxEntity *ent)
{
	float tv[9], v0[3], v1[3];
	lbxEntity *cur;
	char *s, *t, *st, *mt;
	float f, g;
	int i, j;

	s=lbxEntGetPropertyString(ent, "solidtype");

	st=s;

	if(s)
	{
		if(!strcmp(s, "solid_not"))
		{
		}else if(!strcmp(s, "solid_prim"))
		{
//			bmdl=lbxEntGetProperty(ent, "mdl");
//			i=bmdl->solid;
//			bsdeUnhide(i);
//			lbxEntSetPropertyFloat(ent, "_solid", i);
		}else if(!strcmp(s, "solid_bbox"))
		{
			lbxEntGet3FV(ent, "mins", v0);
			lbxEntGet3FV(ent, "maxs", v1);
			V3F_SCALE(v0, LBXGL_METERS_PER_INCH, v0);
			V3F_SCALE(v1, LBXGL_METERS_PER_INCH, v1);

			i=bsdeNewAABB(v0[0], v0[1], v0[2],
				v1[0], v1[1], v1[2]);
			lbxEntSetInt(ent, "_solid", i);
			bsdeSetAttrF(i, BSDE_DENSITY,
				LBXGL_DENSITY_MG_M3);
		}else if(!strcmp(s, "solid_trigger"))
		{
			lbxEntGet3FV(ent, "mins", v0);
			lbxEntGet3FV(ent, "maxs", v1);
			V3F_SCALE(v0, LBXGL_METERS_PER_INCH, v0);
			V3F_SCALE(v1, LBXGL_METERS_PER_INCH, v1);

			i=bsdeNewAABB(v0[0], v0[1], v0[2],
				v1[0], v1[1], v1[2]);
			lbxEntSetInt(ent, "_solid", i);

			bsdeSetAttrI(i, BSDE_NONSOLID, 1);
			bsdeSetAttrI(i, BSDE_TRIGGER, 1);
			bsdeSetAttrF(i, BSDE_DENSITY,
				LBXGL_DENSITY_MG_M3);
		}else if(!strcmp(s, "solid_obb"))
		{
			lbxEntGet3FV(ent, "mins", v0);
			lbxEntGet3FV(ent, "maxs", v1);
			V3F_SCALE(v0, LBXGL_METERS_PER_INCH, v0);
			V3F_SCALE(v1, LBXGL_METERS_PER_INCH, v1);

			i=bsdeNewOBB(v0[0], v0[1], v0[2],
				v1[0], v1[1], v1[2]);
			lbxEntSetInt(ent, "_solid", i);
			bsdeSetAttrF(i, BSDE_DENSITY,
				LBXGL_DENSITY_MG_M3);
			bsdeSetAttrI(i, BSDE_NONSOLID, 0);
		}else if(!strcmp(s, "solid_sphere"))
		{
			f=lbxEntGetFloat(ent, "radius");
			f*=LBXGL_METERS_PER_INCH;

			i=bsdeNewSphere(f);
			lbxEntSetInt(ent, "_solid", i);
			bsdeSetAttrF(i, BSDE_DENSITY,
				LBXGL_DENSITY_MG_M3);
			bsdeSetAttrI(i, BSDE_NONSOLID, 0);
		}else if(!strcmp(s, "solid_cylinder"))
		{
			f=lbxEntGetFloat(ent, "height");
			g=lbxEntGetFloat(ent, "radius");
			f*=LBXGL_METERS_PER_INCH;
			g*=LBXGL_METERS_PER_INCH;

			i=bsdeNewCylinder(f, g);
			lbxEntSetInt(ent, "_solid", i);
			bsdeSetAttrF(i, BSDE_DENSITY,
				LBXGL_DENSITY_MG_M3);
			bsdeSetAttrI(i, BSDE_NONSOLID, 0);
		}else if(!strcmp(s, "solid_capped_cylinder"))
		{
			f=lbxEntGetFloat(ent, "height");
			g=lbxEntGetFloat(ent, "radius");
			f*=LBXGL_METERS_PER_INCH;
			g*=LBXGL_METERS_PER_INCH;

			i=bsdeNewCappedCylinder(f, g);
			lbxEntSetInt(ent, "_solid", i);
			bsdeSetAttrF(i, BSDE_DENSITY,
				LBXGL_DENSITY_MG_M3);
			bsdeSetAttrI(i, BSDE_NONSOLID, 0);
		}
	}

	i=lbxEntGetInt(ent, "_solid");
	if(i<=0)return;

	t=lbxEntGetPropertyString(ent, "movetype");
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


//	s=lbxEntGetString(ent, "touch");
//	bsdeSetAttrI(i, BSDE_TRIGGER, s?BSDE_TRUE:BSDE_FALSE);

	LBXGL_EntPhys_SendEntState(world, ent);
}

LBXGL_API void LBXGL_EntPhys_SendEntState(lbxWorld *world, lbxEntity *ent)
{
	float tv[9];
	int i;

	i=lbxEntGetInt(ent, "_solid");
	if(i<=0)return;

	lbxEntGet3FV(ent, "origin", tv);
	V3F_SCALE(tv, LBXGL_METERS_PER_INCH, tv);
	bsdeSetAttrFV(i, BSDE_ORIGIN, tv, 3);

	lbxEntGet3FV(ent, "velocity", tv);
	V3F_SCALE(tv, LBXGL_METERS_PER_INCH, tv);
	bsdeSetAttrFV(i, BSDE_VELOCITY, tv, 3);

	lbxEntGet9FV(ent, "rot", tv);
	bsdeSetAttrFV(i, BSDE_ROTATION_MATRIX, tv, 9);
}

LBXGL_API void LBXGL_EntPhys_RecvEntState(lbxWorld *world, lbxEntity *ent)
{
	float tv[9];
	int i;

	i=lbxEntGetInt(ent, "_solid");
	if(i<=0)return;

	bsdeGetAttrFV(i, BSDE_ORIGIN, tv, 3);
	V3F_SCALE(tv, LBXGL_INCHES_PER_METER, tv);
	lbxEntSet3FV(ent, "origin", tv);

	bsdeGetAttrFV(i, BSDE_VELOCITY, tv, 3);
	V3F_SCALE(tv, LBXGL_INCHES_PER_METER, tv);
	lbxEntSet3FV(ent, "velocity", tv);

	bsdeGetAttrFV(i, BSDE_ROTATION_MATRIX, tv, 9);
	lbxEntSet9FV(ent, "rot", tv);
}

LBXGL_API void LBXGL_EntPhys_UpdateEnt(lbxWorld *world, lbxEntity *ent)
{
	int i;

	if(lbxEntIsDirty(ent, "solidtype") || lbxEntIsDirty(ent, "movetype"))
		LBXGL_EntPhys_UnlinkEnt(world, ent);

	i=lbxEntGetInt(ent, "_solid");
	if(i<=0)
	{
		LBXGL_EntPhys_SendEnt(world, ent);
		return;
	}

	LBXGL_EntPhys_RecvEntState(world, ent);

//	bsdeDestroy(i);
//	lbxEntSetInt(ent, "_solid", 0);
}

LBXGL_API void LBXGL_EntPhys_UnlinkEnt(lbxWorld *world, lbxEntity *ent)
{
	int i;

	i=lbxEntGetInt(ent, "_solid");
	if(i<=0)return;
	bsdeDestroy(i);
	lbxEntSetInt(ent, "_solid", 0);
}

LBXGL_API void LBXGL_EntPhys_UpdateWorld(lbxWorld *world)
{
	float tv[3];
	LBXGL_Light *dl;
	lbxEntity *cur;
	int i;

	if(world->phys_id<=0)
		return;

//	if(!world->bsp)
//		return;

	bsdeBindWorld(world->phys_id);

	cur=world->ents;
	while(cur)
	{
		LBXGL_EntPhys_UpdateEnt(world, cur);

		i=lbxEntGetInt(cur, "_lightnum");
		if(i>0)
		{
			lbxEntGet3FV(cur, "origin", tv);
			dl=lbxWorldFetchLight(world, i);
			V3F_COPY(tv, dl->org);
		}

		cur=cur->next;
	}
}

LBXGL_API void LBXGL_EntPhys_UnlinkWorld(lbxWorld *world)
{
	lbxEntity *cur;

	if(world->phys_id<=0)
		return;
	bsdeBindWorld(world->phys_id);

	cur=world->ents;
	while(cur)
	{
		LBXGL_EntPhys_UnlinkEnt(world, cur);
		cur=cur->next;
	}
}

