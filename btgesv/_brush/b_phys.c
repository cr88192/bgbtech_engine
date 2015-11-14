#include <btgesv.h>

#include <libbsde.h>

BTGE_API void BTGE_BrushPhys_SendBrush(BTGE_Brush *brsh)
{
	float org[3], pt[3];
	float nv[16*4];
	float f;
	int i;

	for(i=0; i<brsh->n_face; i++)
	{
		nv[i*4+0]=brsh->norm[i*4+0];
		nv[i*4+1]=brsh->norm[i*4+1];
		nv[i*4+2]=brsh->norm[i*4+2];

		f=V3F_DOT(brsh->org, nv+i*4);
//		nv[i*4+3]=(brsh->norm[i*4+3]-f) * BTGE_METERS_PER_INCH;
		nv[i*4+3]=brsh->norm[i*4+3]-f;
	}

	i=bsdeNewHullF(nv, brsh->n_face);
//	i=bsdeNewHullF(brsh->norm, brsh->n_face);

	brsh->solid=i;

//	V3F_SCALE(brsh->org, BTGE_METERS_PER_INCH, org);
	V3F_COPY(brsh->org, org);

	bsdeSetAttrFV(i, BSDE_ORIGIN, org, 3);
//	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 0), 3);
//	bsdeSetAttrF(i, BSDE_DENSITY, 1000);
//	bsdeSetAttrF(i, BSDE_DENSITY, BTGE_DENSITY_MG_M3);

	if(brsh->model && brsh->model->ent)
	{
		bsdeSetAttrI(i, BSDE_STATIC, 0);
		bsdeSetAttrI(i, BSDE_SEMISTATIC, 1);
		bsdeSetAttrI(i, BSDE_NONSOLID, 1);
	}else
	{
		bsdeSetAttrI(i, BSDE_STATIC, 1);
	}
}

BTGE_API void BTGE_BrushPhys_SendBrushes(BTGE_Brush *brsh)
{
	BTGE_Brush *cur;

	cur=brsh;
	while(cur)
	{
		BTGE_BrushPhys_SendBrush(cur);
		cur=cur->next;
	}
}

BTGE_API void BTGE_BrushPhys_UpdateBrush(BTGE_Brush *brsh)
{
	float org[3], pt[3], rot[9];
	float *trot;
	float f;

	if(!brsh->model)
		return;		//scene is non-initialized

	if(brsh->solid<=0)
		BTGE_BrushPhys_SendBrush(brsh);
	if(brsh->solid<=0)
		return;

	if(!brsh->model->ent)
		return;	//static world geometry

//	V3F_ADD(brsh->model->mesh->org, brsh->org, org);

	V3F_SUB(brsh->org, brsh->model->mdl_org, pt);

	trot=pfvec3(brsh->model->ent->origin);
	V3F_SCALE(trot+0*3, pt[0], org);
	V3F_ADDSCALE(org, trot+1*3, pt[1], org);
	V3F_ADDSCALE(org, trot+2*3, pt[2], org);

	V3F_ADD(org, pfvec3(brsh->model->ent->origin), org);
//	V3F_ADD(brsh->model->mesh->org, brsh->org, org);

	V3F_SCALE(org, BTGE_METERS_PER_INCH, org);

	bsdeGetAttrFV(brsh->solid, BSDE_ORIGIN, pt, 3);
	bsdeGetAttrFV(brsh->solid, BSDE_ROTATION_MATRIX, rot, 9);

	f=V3F_DIST2(pt, org) +
		V3F_DIST2(rot+0, trot+0) +
		V3F_DIST2(rot+3, trot+3) +
		V3F_DIST2(rot+6, trot+6);

	if(f>0.001)	//avoid waking idle solids
	{
		bsdeSetAttrFV(brsh->solid, BSDE_ORIGIN, org, 3);
		bsdeSetAttrFV(brsh->solid, BSDE_ROTATION_MATRIX, trot, 9);
	}
}

BTGE_API void BTGE_BrushPhys_UnlinkBrush(BTGE_Brush *brsh)
{
	float org[3], pt[3];
	float *trot;

	if(brsh->solid<=0)
		return;

	bsdeDestroy(brsh->solid);
	brsh->solid=0;
}

BTGE_API void BTGE_BrushPhys_UpdateWorld(BTGE_BrushWorld *world)
{
	BTGE_Brush *cur;

//	return;

	if(world->phys_id<=0)
	{
		world->phys_id=bsdeNewWorld();

		bsdeSetWorldAttrI(BSDE_UNIT_LENGTH, BSDE_UNIT_INCH_F8);
		bsdeSetWorldAttrI(BSDE_UNIT_MASS, BSDE_UNIT_POUND);
		bsdeSetWorldAttrI(BSDE_UNIT_FORCE, BSDE_UNIT_POUND);
		bsdeSetWorldAttrI(BSDE_UNIT_VOLUME, BSDE_UNIT_FOOT);

//		bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -16*12), 3);
		bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -16*8), 3);

//		bsdeSetWorldAttrF(BSDE_MINSTEP, 0.005);
//		bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.01);
//		bsdeSetWorldAttrF(BSDE_MAXFRAME, 0.1);

//		bsdeSetWorld3f(BSDE_GRAVITY, 0, 0, -9.8);
		bsdeSetWorldf(BSDE_MINSTEP, 0.01);
//		bsdeSetWorldf(BSDE_MAXSTEP, 0.01667);
		bsdeSetWorldf(BSDE_MAXSTEP, 0.02);
//		bsdeSetWorldf(BSDE_MAXSTEP, 0.04);
		bsdeSetWorldf(BSDE_MAXFRAME, 0.1);
	}
	if(world->phys_id<=0)
		return;

	if(!world->bsp)
		return;

	bsdeBindWorld(world->phys_id);

	cur=world->brush;
	while(cur)
	{
		BTGE_BrushPhys_UpdateBrush(cur);
		cur=cur->next;
	}

	BTGE_EntPhys_UpdateWorld(world);
}

BTGE_API void BTGE_BrushPhys_UnlinkWorld(BTGE_BrushWorld *world)
{
	BTGE_Brush *cur;

	if(world->phys_id<=0)
		return;
	bsdeBindWorld(world->phys_id);

	cur=world->brush;
	while(cur)
	{
		BTGE_BrushPhys_UnlinkBrush(cur);
		cur=cur->next;
	}

	BTGE_EntPhys_UnlinkWorld(world);
}

BTGE_API void BTGE_BrushPhys_StepWorld(BTGE_BrushWorld *world, double dt)
{
	BTGE_Brush *cur;

	if(world->phys_id<=0)
		return;
	bsdeBindWorld(world->phys_id);

	bsdeStepWorld(dt);
}
