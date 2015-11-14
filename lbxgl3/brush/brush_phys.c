#include <lbxgl2.h>
#include <pdglui.h>

#include <libbsde.h>

LBXGL_API void LBXGL_BrushPhys_SendBrush(LBXGL_Brush *brsh)
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
		nv[i*4+3]=(brsh->norm[i*4+3]-f) * LBXGL_METERS_PER_INCH;
	}

	i=bsdeNewHullF(nv, brsh->n_face);
//	i=bsdeNewHullF(brsh->norm, brsh->n_face);

	brsh->solid=i;

	V3F_SCALE(brsh->org, LBXGL_METERS_PER_INCH, org);
	bsdeSetAttrFV(i, BSDE_ORIGIN, org, 3);
//	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 0), 3);
//	bsdeSetAttrF(i, BSDE_DENSITY, 1000);
	bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_MG_M3);

	if(brsh->model && brsh->model->mesh)
	{
		bsdeSetAttrI(i, BSDE_STATIC, 0);
		bsdeSetAttrI(i, BSDE_SEMISTATIC, 1);
		bsdeSetAttrI(i, BSDE_NONSOLID, 1);
	}else
	{
		bsdeSetAttrI(i, BSDE_STATIC, 1);
	}
}

LBXGL_API void LBXGL_BrushPhys_SendBrushes(LBXGL_Brush *brsh)
{
	LBXGL_Brush *cur;

	cur=brsh;
	while(cur)
	{
		LBXGL_BrushPhys_SendBrush(cur);
		cur=cur->next;
	}
}

LBXGL_API void LBXGL_BrushPhys_UpdateBrush(LBXGL_Brush *brsh)
{
	float org[3], pt[3], rot[9];
	float *trot;
	float f;

	if(!brsh->model)
		return;		//scene is non-initialized

	if(brsh->solid<=0)
		LBXGL_BrushPhys_SendBrush(brsh);
	if(brsh->solid<=0)
		return;

	if(!brsh->model->mesh)
		return;	//static world geometry

//	V3F_ADD(brsh->model->mesh->org, brsh->org, org);

	V3F_SUB(brsh->org, brsh->model->mdl_org, pt);

	trot=brsh->model->mesh->org;
	V3F_SCALE(trot+0*3, pt[0], org);
	V3F_ADDSCALE(org, trot+1*3, pt[1], org);
	V3F_ADDSCALE(org, trot+2*3, pt[2], org);

	V3F_ADD(org, brsh->model->mesh->org, org);
//	V3F_ADD(brsh->model->mesh->org, brsh->org, org);

	V3F_SCALE(org, LBXGL_METERS_PER_INCH, org);

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

LBXGL_API void LBXGL_BrushPhys_UnlinkBrush(LBXGL_Brush *brsh)
{
	float org[3], pt[3];
	float *trot;

	if(brsh->solid<=0)
		return;

	bsdeDestroy(brsh->solid);
	brsh->solid=0;
}

LBXGL_API void LBXGL_BrushPhys_UpdateWorld(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur;

	return;

	if(world->phys_id<=0)
	{
		world->phys_id=bsdeNewWorld();

		bsdeSetWorld3f(BSDE_GRAVITY, 0, 0, -9.8);
//		bsdeSetWorldf(BSDE_MINSTEP, 0.005);
//		bsdeSetWorldf(BSDE_MINSTEP, 0.001);
		bsdeSetWorldf(BSDE_MINSTEP, 0.01);
//		bsdeSetWorldf(BSDE_MAXSTEP, 0.01);
//		bsdeSetWorldf(BSDE_MAXSTEP, 0.03);
		bsdeSetWorldf(BSDE_MAXSTEP, 0.01667);
		bsdeSetWorldf(BSDE_MAXFRAME, 0.1);

//		bsdeSetWorldf(BSDE_MINSTEP, 0.0005);
//		bsdeSetWorldf(BSDE_MAXSTEP, 0.005);

//		bsdeSetWorldAttrI(BSDE_COLLIDE_STRICT, 1);
	}
	if(world->phys_id<=0)
		return;

	if(!world->bsp)
		return;

	bsdeBindWorld(world->phys_id);

	cur=world->brush;
	while(cur)
	{
		LBXGL_BrushPhys_UpdateBrush(cur);
		cur=cur->next;
	}

	LBXGL_EntPhys_UpdateWorld(world);
}

LBXGL_API void LBXGL_BrushPhys_UnlinkWorld(LBXGL_BrushWorld *world)
{
	LBXGL_Brush *cur;

	if(world->phys_id<=0)
		return;
	bsdeBindWorld(world->phys_id);

	cur=world->brush;
	while(cur)
	{
		LBXGL_BrushPhys_UnlinkBrush(cur);
		cur=cur->next;
	}

	LBXGL_EntPhys_UnlinkWorld(world);
}

LBXGL_API void LBXGL_BrushPhys_StepWorld(LBXGL_BrushWorld *world, double dt)
{
	LBXGL_Brush *cur;

	if(world->phys_id<=0)
		return;
	bsdeBindWorld(world->phys_id);

	bsdeStepWorld(dt);
}
