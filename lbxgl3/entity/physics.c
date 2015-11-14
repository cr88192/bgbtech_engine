/*
Do world physics via use of BSDE.

The entities will see physics in terms of settable properies, movetype, and
solidtype. Altering physics related properties will change the state of
attached solids, and solids may be created or destroyed in some cases.

For many solidtypes, AABBs will be the default primitive type.
*/

#include <lbxgl2.h>

int LBXGL_Physics_SpawnWorld(LBXGL_World *wrl)
{
	LBXGL_Entity *ecur;
	LBXGL_Model *mcur;
	float fv[3];
	char *s, *t, *s2, *t2;
	int i;

	wrl->id=bsdeNewWorld();

	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -9.8), 3);
//	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -2), 3);
	bsdeSetWorldAttrF(BSDE_MINSTEP, 0.005);
	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.01);
//	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.03);
	bsdeSetWorldAttrF(BSDE_MAXFRAME, 0.1);

//	bsdeSetWorldAttrI(BSDE_COLLIDE_STRICT, 1);

	mcur=world->mdls;
	while(mcur)
	{
		bsdeBegin(BSDE_SOLID_BSP);
		BSP_SendNode(mcur->root);
		i=bsdeEnd();
		mcur->solid=i;
		bsdeSetAttrF(i, BSDE_DENSITY, 1000);
		bsdeSetAttrI(i, BSDE_SEMISTATIC, 1);
		bsdeSetAttrI(i, BSDE_NONSOLID, 1);

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
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);
			bsdeSetAttrI(i, BSDE_SEMISTATIC, 0);
			bsdeSetAttrI(i, BSDE_NONSOLID, 0);
			bsdeSetAttrI(i, BSDE_STATIC, 1);
		}

		LBXGL_EntClass_SpawnEnt(ecur);
		ecur=ecur->next;
	}

	return(0);
}

int LBXGL_Physics_StepWorld(LBXGL_World *wrl, float dt)
{
	bsdeBindWorld(wrl->id);
	bsdeStepWorld(dt);

	BSP_RelinkEnts(wrl);
	return(0);
}
