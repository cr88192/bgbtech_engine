#include <btgesv.h>

#include <libbsde.h>


void BTGE_EntPhys_UnlinkEntity(btEntity ent)
{
	float org[3], pt[3];
	float *trot;

	if(!ent)return;

	if(ent->solidnum<=0)
		return;

	bsdeDestroy(ent->solidnum);
	ent->solidnum=0;
}

void BTGE_EntPhys_SendEntity(btEntity ent)
{
	float org[3], rot[9], vel[3];
	float v0[3], v1[3];
	int solid;
	int i;
	
//	printf("BTGE_EntPhys_SendEntity: %p\n", ent);
	
	solid=ent->solidnum;
	if(solid>0)
	{
		bsdeHide(solid);
//		bsdeDestroy(i);
//		LBXGL_Entity_SetPropertyFloat(cur, "_solid", -1);
		ent->solidnum=-1;
	}

	if(ent->solidtype==BT_SOLID_NOT)
	{
		if(solid>0)bsdeDestroy(solid);
		return;
	}

	if(ent->solidtype==BT_SOLID_BSP)
	{
//		i=ent->bmdl->solid;
//		bsdeUnhide(i);
//		ent->solidnum=i;

		if(solid>0)bsdeDestroy(solid);
		return;
	}

	if(ent->solidtype==BT_SOLID_TRIGGER)
	{
		if(solid>0)bsdeDestroy(solid);
		return;
	}


	printf("BTGE_EntPhys_SendEntity: %p\n", ent);

	if(	(ent->solidtype==BT_SOLID_BBOX) ||
		(ent->solidtype==BT_SOLID_SLIDEBOX))
	{
		if(solid>0)bsdeDestroy(solid);

		vfvec3(v0, ent->mins);
		vfvec3(v1, ent->maxs);
		
		i=bsdeNewAABB(v0[0], v0[1], v0[2],
			v1[0], v1[1], v1[2]);

		ent->solidnum=i;
		solid=i;

//		j=!strcmp(s, "solid_trigger");
//		bsdeSetAttrI(i, BSDE_NONSOLID, j);
//		bsdeSetAttrF(i, BSDE_DENSITY,
//			LBXGL_DENSITY_LB_IN3);

		bsdeSetAttrI(i, BSDE_NONSOLID, 0);
//		bsdeSetAttrF(i, BSDE_DENSITY, BTGE_DENSITY_MG_M3);
	}

	if(	(ent->solidtype==BT_SOLID_OBB) ||
		(ent->solidtype==BT_SOLID_OBB_YAW))
	{
		if(solid>0)bsdeDestroy(solid);

		vfvec3(v0, ent->mins);
		vfvec3(v1, ent->maxs);
		
		i=bsdeNewOBB(v0[0], v0[1], v0[2],
			v1[0], v1[1], v1[2]);

		ent->solidnum=i;
		solid=i;

		bsdeSetAttrI(i, BSDE_NONSOLID, 0);
//		bsdeSetAttrF(i, BSDE_DENSITY, BTGE_DENSITY_MG_M3);
	}

	if(ent->solidtype==BT_SOLID_SPHERE)
	{
		if(solid>0)bsdeDestroy(solid);
		
		i=bsdeNewSphere(ent->radius);

		ent->solidnum=i;
		solid=i;

		bsdeSetAttrI(i, BSDE_NONSOLID, 0);
//		bsdeSetAttrF(i, BSDE_DENSITY, BTGE_DENSITY_MG_M3);
	}

	if(ent->solidtype==BT_SOLID_CYLINDER)
	{
		if(solid>0)bsdeDestroy(solid);
		
		i=bsdeNewCylinder(ent->height, ent->radius);

		ent->solidnum=i;
		solid=i;

		bsdeSetAttrI(i, BSDE_NONSOLID, 0);
//		bsdeSetAttrF(i, BSDE_DENSITY, BTGE_DENSITY_MG_M3);
	}

	if(ent->solidtype==BT_SOLID_CAPSULE)
	{
		if(solid>0)bsdeDestroy(solid);
		
		i=bsdeNewCappedCylinder(ent->height, ent->radius);

		ent->solidnum=i;
		solid=i;

		bsdeSetAttrI(i, BSDE_NONSOLID, 0);
//		bsdeSetAttrF(i, BSDE_DENSITY, BTGE_DENSITY_MG_M3);
	}

	if(ent->movetype==BT_MOVE_NONE)
	{
		//non-moving
		bsdeSetAttrI(solid, BSDE_STATIC, 1);
	}

	if((ent->movetype==BT_MOVE_WALK) ||
		(ent->movetype==BT_MOVE_STEP) ||
		(ent->movetype==BT_MOVE_FLY) ||
		(ent->movetype==BT_MOVE_TOSS) ||
		(ent->movetype==BT_MOVE_FLYMISSILE) ||
		(ent->movetype==BT_MOVE_BOUNCE) ||
		(ent->movetype==BT_MOVE_BOUNCEMISSILE))
	{
		//controlled by program
		bsdeSetAttrI(solid, BSDE_STATIC, 0);
		bsdeSetAttrI(solid, BSDE_SEMISTATIC, 1);
//		bsdeSetAttrI(solid, BSDE_SEMISTATIC, 0);
	}

	if(ent->movetype==BT_MOVE_NOCLIP)
	{
		//controlled by program, non-solid
		bsdeSetAttrI(solid, BSDE_STATIC, 0);
		bsdeSetAttrI(solid, BSDE_SEMISTATIC, 1);
		bsdeSetAttrI(i, BSDE_NONSOLID, 1);
	}

	if(ent->movetype==BT_MOVE_RBD_PHYSICS)
	{
		//controlled by rigid-body physics
		bsdeSetAttrI(solid, BSDE_STATIC, 0);
		bsdeSetAttrI(solid, BSDE_SEMISTATIC, 0);
	}

	vfvec3(org, ent->origin);
	vfvec3(vel, ent->velocity);
	QuatF_To3Matrix(pfquat(ent->rot), rot);

	bsdeSetAttrFV(ent->solidnum, BSDE_ORIGIN, org, 3);
	bsdeSetAttrFV(ent->solidnum, BSDE_VELOCITY, vel, 3);
	bsdeSetAttrFV(ent->solidnum, BSDE_ROTATION_MATRIX, rot, 9);
}

void BTGE_EntPhys_UpdateEntity(btEntity ent)
{
	float org[3], rot[9], vel[3];
	float torg[3], trot[9], tvel[3];
	float f;

//	if(!brsh->model)
//		return;		//scene is non-initialized

	if(!ent)return;

	if((ent->solidnum<=0) || (ent->flags&BT_FL_SET_SOLID))
		BTGE_EntPhys_SendEntity(ent);
	if(ent->solidnum<=0)
		return;

	if((ent->movetype!=BT_MOVE_RBD_PHYSICS) ||
		(ent->flags&BT_FL_SET_POSITION))
	{
		vfvec3(org, ent->origin);
		vfvec3(vel, ent->velocity);
		QuatF_To3Matrix(pfquat(ent->rot), rot);

//		V3F_SCALE(org, BTGE_METERS_PER_INCH, org);

		bsdeGetAttrFV(ent->solidnum, BSDE_ORIGIN, torg, 3);
		bsdeGetAttrFV(ent->solidnum, BSDE_VELOCITY, tvel, 3);
		bsdeGetAttrFV(ent->solidnum, BSDE_ROTATION_MATRIX, trot, 9);

		f=	V3F_DIST2(org, torg) +
			V3F_DIST2(vel, tvel) +
			V3F_DIST2(rot+0, trot+0) +
			V3F_DIST2(rot+3, trot+3) +
			V3F_DIST2(rot+6, trot+6);

		if(f>0.01)	//avoid waking idle solids
		{
			bsdeSetAttrFV(ent->solidnum, BSDE_ORIGIN, org, 3);
			bsdeSetAttrFV(ent->solidnum, BSDE_VELOCITY, vel, 3);
			bsdeSetAttrFV(ent->solidnum, BSDE_ROTATION_MATRIX, rot, 9);
		}
	}

	if(ent->movetype==BT_MOVE_RBD_PHYSICS)
	{
		bsdeGetAttrFV(ent->solidnum, BSDE_ORIGIN, org, 3);
		bsdeGetAttrFV(ent->solidnum, BSDE_VELOCITY, vel, 3);
		bsdeGetAttrFV(ent->solidnum, BSDE_ROTATION_MATRIX, rot, 9);
		
		QuatF_From3Matrix(rot, trot);
		
		ent->origin=vec3vf(org);
		ent->velocity=vec3vf(vel);
		ent->rot=quatvf(trot);
		
//		printf("BTGE_EntPhys_UpdateEntity: "
//			"(%.2f %.2f %.2f) (%.2f %.2f %.2f)\n",
//			org[0], org[1], org[2],
//			vel[0], vel[1], vel[2]);
	}
}

void BTGE_EntPhys_UpdateWorld(BTGE_BrushWorld *world)
{
#if 1
	btEntity ent;
	int i;
	
	for(i=1; i<world->n_ents; i++)
	{
		ent=world->ents[i];
		if(!ent)continue;
		BTGE_EntPhys_UpdateEntity(ent);
		ent->flags&=~(BT_FL_SET_SOLID|BT_FL_SET_POSITION);
	}
#endif
}

void BTGE_EntPhys_UnlinkWorld(BTGE_BrushWorld *world)
{
	btEntity ent;
	int i;
	
	for(i=1; i<world->n_ents; i++)
	{
		ent=world->ents[i];
		if(!ent)continue;
		BTGE_EntPhys_UnlinkEntity(ent);
	}
}
