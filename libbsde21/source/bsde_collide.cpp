#include "bsde_internal.h"

int bsde_checkcollide_obb(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	bsde_real norm[4];
	int i, j;

	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		if(V3_DIST(axform+12, bxform+12)>=(aobj->radius+bobj->radius))
			return(0);

		i=BSDE_SolidOBB_CheckCollide(
			aobj->mins, aobj->maxs, axform,
			bobj->mins, bobj->maxs, bxform);
		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidSphere_CollideObbContact(
			bobj->radius, borg,
			aobj->mins, aobj->maxs, axform,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		j=bobj->solidtype==BSDE_SOLID_CCYLINDER;
		i=BSDE_SolidCylinder_CollideObbContact(
			bobj->height, bobj->radius, j, bxform,
			aobj->mins, aobj->maxs, axform,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		j=BSDE_SolidOBB_CheckObbPlane(
			aobj->mins, aobj->maxs, axform, norm, NULL);
		i=(j&2)!=0;

//		if(i)printf("checkcollide plane box\n");
		break;

	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckObbCollide(bobj, aobj,
			bxform, axform);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidBRep_CheckCollide(bobj, aobj,
			bxform, axform);
	default:
		break;
	}

	return(i);
}

int bsde_checkcollide_sphere(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	bsde_real norm[4], f;
	int i, j;

	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		i=BSDE_SolidSphere_CollideObbContact(
			aobj->radius, aorg,
			bobj->mins, bobj->maxs, bxform,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidSphere_CollideContact(
			aobj->radius, aorg, bobj->radius, borg,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		j=bobj->solidtype==BSDE_SOLID_CCYLINDER;
		i=BSDE_SolidCylinder_CollideContact(
			0, aobj->radius, 1, axform,
			bobj->height, bobj->radius, j, bxform,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		f=V3_NDOT(aorg, norm);
		i=(f-aobj->radius)<0;
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckSphereCollide(bobj, aobj,
			bxform, axform, borg, aorg);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidBRep_CheckCollide(bobj, aobj,
			bxform, axform);

	default:
		break;
	}

	return(i);
}

int bsde_checkcollide_cylinder(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg, int t)
{
	bsde_real norm[4], f, g;
	int i, j;

	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		i=BSDE_SolidCylinder_CollideObbContact(
			aobj->height, aobj->radius, t, axform,
			bobj->mins, bobj->maxs, bxform,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidCylinder_CollideContact(
			aobj->height, aobj->radius, t, axform,
			0.0, bobj->radius, 1, bxform,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		j=bobj->solidtype==BSDE_SOLID_CCYLINDER;
		i=BSDE_SolidCylinder_CollideContact(
			aobj->height, aobj->radius, t, axform,
			bobj->height, bobj->radius, j, bxform,
			NULL, NULL, NULL);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		f=V3_NDOT(aorg, norm);
		g=BSDE_SolidCylinder_AxisRadius(
			aobj->height, aobj->radius, t, axform, norm);
		i=(f-g)<0;
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckCylinderCollide(bobj, aobj,
			bxform, axform, borg, aorg);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidBRep_CheckCollide(bobj, aobj,
			bxform, axform);

	default:
		break;
	}

	return(i);
}

int bsde_checkcollide_plane(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	bsde_real norm[4], f, g;
	int i, j;

	norm[0]=(aobj->dir[0]*axform[0])+(aobj->dir[1]*axform[4])+
		(aobj->dir[2]*axform[8]);
	norm[1]=(aobj->dir[0]*axform[1])+(aobj->dir[1]*axform[5])+
		(aobj->dir[2]*axform[9]);
	norm[2]=(aobj->dir[0]*axform[2])+(aobj->dir[1]*axform[6])+
		(aobj->dir[2]*axform[10]);
	norm[3]=aobj->dir[3]+V3_DOT(aorg, norm);

//	printf("checkcollide plane (%f %f %f %f)\n",
//		norm[0], norm[1], norm[2], norm[3]);

	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		j=BSDE_SolidOBB_CheckObbPlane(
			bobj->mins, bobj->maxs, bxform, norm, NULL);
		i=(j&2)!=0;
//		printf("checkcollide plane box %d %d\n", i, j);
		break;

	case BSDE_SOLID_SPHERE:
		f=V3_NDOT(borg, norm);
		i=(f-bobj->radius)<0;
//		if(i)printf("collide sphere\n");
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		j=bobj->solidtype==BSDE_SOLID_CCYLINDER;
		f=V3_NDOT(borg, norm);
		g=BSDE_SolidCylinder_AxisRadius(
			bobj->height, bobj->radius, j, bxform, norm);
		i=(f-g)<0;
		break;

	case BSDE_SOLID_PLANE:	//planes don't collide
		i=0;
		break;

	case BSDE_SOLID_HULL:
		BSDE_SolidHull_ProjectLine(bobj, bxform, norm, &f, &g);
		i=(f-norm[3])<0;
		break;

	default:
		break;
	}

	return(i);
}

int bsde_checkcollide_hull(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	bsde_real norm[4], f, g;
	int i, j;

//	i=BSDE_SolidHull_CheckCollide(aobj, bobj);

	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		i=BSDE_SolidHull_CheckObbCollide(aobj, bobj,
			axform, bxform);
		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidHull_CheckSphereCollide(aobj, bobj,
			axform, bxform, aorg, borg);
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidHull_CheckCylinderCollide(aobj, bobj,
			axform, bxform, aorg, borg);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		BSDE_SolidHull_ProjectLine(aobj, axform, norm, &f, &g);
		i=(f-norm[3])<0;
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckCollide(aobj, bobj,
			axform, bxform);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidHull_CheckTriMeshCollide(
			aobj, bobj, axform, bxform);
		break;

	default:
		break;
	}

	return(i);
}

int bsde_checkcollide(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	bsde_real ma[3], na[3];
	bsde_real mb[3], nb[3];
	int i;

	if((aobj->solidtype==BSDE_SOLID_AABB) && (bobj->solidtype==BSDE_SOLID_AABB))
	{
		V3_ADD(aorg, aobj->mins, ma);
		V3_ADD(aorg, aobj->maxs, na);
		V3_ADD(borg, bobj->mins, mb);
		V3_ADD(borg, bobj->maxs, nb);
		i=BSDE_SolidAABB_BoxCollideP(ma, na, mb, nb);
		return(i);
	}
	if((aobj->solidtype==BSDE_SOLID_BSP) ||
		(bobj->solidtype==BSDE_SOLID_BSP))
	{
		i=BSDE_SolidBSP_CheckCollide(aobj, axform, bobj, bxform);
		return(i);
	}

	i=-1;
	switch(aobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		i=bsde_checkcollide_obb(aobj, bobj,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_SPHERE:
		i=bsde_checkcollide_sphere(aobj, bobj,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_CYLINDER:
		i=bsde_checkcollide_cylinder(aobj, bobj,
			axform, bxform, aorg, borg, 0);
		break;
	case BSDE_SOLID_CCYLINDER:
		i=bsde_checkcollide_cylinder(aobj, bobj,
			axform, bxform, aorg, borg, 1);
		break;
	case BSDE_SOLID_PLANE:
		i=bsde_checkcollide_plane(aobj, bobj,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_HULL:
		i=bsde_checkcollide_hull(aobj, bobj,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidBRep_CheckCollide(aobj, bobj,
			axform, bxform);
	default:
		break;
	}

	return(i);
}

int BSDE_CheckCollide(BSDE_Solid *aobj, BSDE_Solid *bobj)
{
	int i;

	i=BSDE_SolidAABB_BoxCollideP(aobj->mmins, aobj->mmaxs,
		bobj->mmins, bobj->mmaxs);
	if(!i)return(0);

	i=bsde_checkcollide(aobj, bobj,
		aobj->e_xform, bobj->e_xform, aobj->e_org, bobj->e_org);
	return(i);
}

int BSDE_CheckInitialCollide(BSDE_Solid *aobj, BSDE_Solid *bobj)
{
	int i;

	i=bsde_checkcollide(aobj, bobj,
		aobj->xform, bobj->xform, aobj->org, bobj->org);
	return(i);
}

int BSDE_CheckCollide2(BSDE_Solid *aobj, BSDE_Solid *bobj, int m)
{
	int i;

	i=BSDE_SolidAABB_BoxCollideP(aobj->mmins, aobj->mmaxs,
		bobj->mmins, bobj->mmaxs);
	if(!i)return(0);

	if(m)
	{
		i=bsde_checkcollide(aobj, bobj,
			aobj->e_xform, bobj->e_xform,
			aobj->e_org, bobj->e_org);
	}else
	{
		i=bsde_checkcollide(aobj, bobj,
			aobj->xform, bobj->xform, aobj->org, bobj->org);
	}
	return(i);
}

bsde_real BSDE_CollideTimeR(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real bt, bsde_real dt, bsde_real mdt)
{
	bsde_real aorg[3], arot[4], axform[16];
	bsde_real borg[3], brot[4], bxform[16];
	bsde_real f, hdt;
	int i;

	if(dt<=mdt)return(0);

	hdt=dt*0.5;
	BSDE_PredictPosTime(aobj, bt+hdt, aorg, arot, axform);
	BSDE_PredictPosTime(bobj, bt+hdt, borg, brot, bxform);
	i=bsde_checkcollide(aobj, bobj, axform, bxform, aorg, borg);

	if(i)
	{
		f=BSDE_CollideTimeR(aobj, bobj, bt, hdt, mdt);
		return(f);
	}else
	{
		f=BSDE_CollideTimeR(aobj, bobj, bt+hdt, hdt, mdt);
		return(hdt+f);
	}
}

bsde_real BSDE_CollideTime(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real dt, bsde_real mdt)
{
	bsde_real asorg[3], asrot[4], asxform[16];
	bsde_real aeorg[3], aerot[4], aexform[16];
	bsde_real bsorg[3], bsrot[4], bsxform[16];
	bsde_real beorg[3], berot[4], bexform[16];
	bsde_real amins[3], amaxs[3];
	bsde_real bmins[3], bmaxs[3];
	bsde_real f;
	int i, j;

	BSDE_PredictPosTime(aobj, 0, asorg, asrot, asxform);
	BSDE_PredictPosTime(bobj, 0, bsorg, bsrot, bsxform);
	BSDE_PredictPosTime(aobj, dt, aeorg, aerot, aexform);
	BSDE_PredictPosTime(bobj, dt, beorg, berot, bexform);

	BSDE_BoxMovePos(aobj, asxform, aexform, asorg, aeorg, amins, amaxs);
	BSDE_BoxMovePos(bobj, bsxform, bexform, bsorg, beorg, bmins, bmaxs);

	i=BSDE_SolidAABB_BoxCollideP(amins, amaxs, bmins, bmaxs);
	if(!i)return(dt);

	i=bsde_checkcollide(aobj, bobj, asxform, bsxform, asorg, bsorg);
	j=bsde_checkcollide(aobj, bobj, aexform, bexform, aeorg, beorg);
	if(i)return(0);
	if(!j)return(dt);

	f=BSDE_CollideTimeR(aobj, bobj, 0, dt, mdt);
	return(f);
}


int bsde_collide_aabb(BSDE_World *world, BSDE_Solid *aobj, BSDE_Solid *bobj,
	BSDE_Contact **lst, bsde_real *axform, bsde_real *bxform,
	bsde_real *aorg, bsde_real *borg)
{
	bsde_real pts[16*3];
	bsde_real norm[4], f, g;
	bsde_real ma[4], na[4];
	bsde_real mb[4], nb[4];
	bsde_real io[4], iv[4], id;
	bsde_real ext[2];
	int i, n;

	n=0;
	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
		i=BSDE_SolidAABB_BoxCollideContact(
			aobj->mmins, aobj->mmaxs, bobj->mmins, bobj->mmaxs,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_OBB:
		i=BSDE_SolidAABB_BoxCollideP(
			aobj->mmins, aobj->mmaxs, bobj->mmins, bobj->mmaxs);
		if(!i)break;

//		i=BSDE_SolidOBB3_CollideContact(
//			aobj->mins, aobj->maxs, axform,
//			bobj->mins, bobj->maxs, bxform,
//			io, iv, &id);

		i=BSDE_SolidOBB3_CollideAABBContact(
			aobj->mins, aobj->maxs, axform,
			bobj->mins, bobj->maxs, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidSphere_CollideObbContact(
			bobj->radius, borg,
			aobj->mins, aobj->maxs, axform,
			io, iv, &id);
		V3_SCALE(iv, -1, iv);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CYLINDER:
		i=BSDE_SolidCylinder_CollideObbContact(
			bobj->height, bobj->radius, 0, bxform,
			aobj->mins, aobj->maxs, axform,
			io, iv, &id);
		V3_SCALE(iv, -1, iv);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidCylinder_CollideObbContact(
			bobj->height, bobj->radius, 1, bxform,
			aobj->mins, aobj->maxs, axform,
			io, iv, &id);
		V3_SCALE(iv, -1, iv);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		BSDE_SolidOBB_ObbPlaneExtents(
			aobj->mins, aobj->maxs, axform, norm, ext);
		BSDE_SolidOBB_ObbPlaneIntersect(
			aobj->mins, aobj->maxs, axform, norm, io);
		id=norm[3]-ext[0];
		i=id>0;

//		if(i)printf("collide box plane %f\n", id);

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, norm, id);
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckObbCollide(bobj, aobj,
			bxform, axform);
		if(i)
		{
//			i=BSDE_SolidHull_ContactGeneric(bobj, aobj,
//				bxform, axform, borg, aorg, io, iv, &id);
//			if(i)lst[n++]=BSDE_MakeContact(world, bobj, aobj,
//				io, iv, id);

			i=BSDE_SolidHull_ContactGeneric(bobj, aobj,
				bxform, axform, borg, aorg, pts, iv, &id);
			if(i)lst[n++]=BSDE_MakePointsContact(world, bobj, aobj,
				pts, iv, id, i);
		}
		break;
	case BSDE_SOLID_TRIMESH:
		n=BSDE_SolidBRep_MultiContact(world, bobj, aobj,
			bxform, axform, lst, n);
		break;

	default:
		break;
	}

	return(n);
}

int bsde_collide_obb(BSDE_World *world, BSDE_Solid *aobj, BSDE_Solid *bobj,
	BSDE_Contact **lst, bsde_real *axform, bsde_real *bxform,
	bsde_real *aorg, bsde_real *borg)
{
	bsde_real pts[16*3];
	bsde_real norm[4], f, g;
	bsde_real ma[4], na[4];
	bsde_real mb[4], nb[4];
	bsde_real io[4], iv[4], id;
	bsde_real ext[2];
	int i, n;

#if 0
	n=0;
	if((aobj->solidtype==BSDE_SOLID_AABB) && (bobj->solidtype==BSDE_SOLID_AABB))
	{
		BSDE_SolidAABB_BoxMove(aobj->mins, aobj->maxs,
			aobj->org, aobj->e_org, ma, na);
		BSDE_SolidAABB_BoxMove(bobj->mins, bobj->maxs,
			bobj->org, bobj->e_org, mb, nb);
		i=BSDE_SolidAABB_BoxCollideContact(ma, na, mb, nb,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		return(i);
	}
#endif

	n=0;
	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
#if 0
		i=BSDE_SolidAABB_BoxCollideP(
			aobj->mmins, aobj->mmaxs, bobj->mmins, bobj->mmaxs);
		if(!i)break;

		i=BSDE_SolidOBB3_CollideAABBContact(
			bobj->mins, bobj->maxs, bxform,
			aobj->mins, aobj->maxs, axform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, bobj, aobj, io, iv, id);
#endif

#if 1
		i=BSDE_SolidHull2_CollideOBB(
			aobj->mins, aobj->maxs,
			bobj->mins, bobj->maxs,
			axform, bxform, pts, iv, &id);

		if(i)lst[n++]=BSDE_MakePointsContact(world, aobj, bobj,
			pts, iv, id, i);
#endif
		break;

	case BSDE_SOLID_OBB:
		i=BSDE_SolidAABB_BoxCollideP(
			aobj->mmins, aobj->mmaxs, bobj->mmins, bobj->mmaxs);
		if(!i)break;

#if 0
		i=BSDE_SolidOBB3_CollideContact(
			aobj->mins, aobj->maxs, axform,
			bobj->mins, bobj->maxs, bxform,
			io, iv, &id);

//		i=BSDE_SolidOBB3_CollideOBBContact2(
//			aobj->mins, aobj->maxs, axform,
//			bobj->mins, bobj->maxs, bxform,
//			io, iv, &id);

//		i=BSDE_SolidOBB3_CollideOBBContact3(
//			aobj, axform, bobj, bxform, io, iv, &id);

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
#endif

#if 1
		i=BSDE_SolidHull2_CollideOBB(
			aobj->mins, aobj->maxs,
			bobj->mins, bobj->maxs,
			axform, bxform, pts, iv, &id);

		if(i)lst[n++]=BSDE_MakePointsContact(world, aobj, bobj,
			pts, iv, id, i);
#endif

		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidSphere_CollideObbContact(
			bobj->radius, borg,
			aobj->mins, aobj->maxs, axform,
			io, iv, &id);
		V3_SCALE(iv, -1, iv);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CYLINDER:
		i=BSDE_SolidCylinder_CollideObbContact(
			bobj->height, bobj->radius, 0, bxform,
			aobj->mins, aobj->maxs, axform,
			io, iv, &id);
		V3_SCALE(iv, -1, iv);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidCylinder_CollideObbContact(
			bobj->height, bobj->radius, 1, bxform,
			aobj->mins, aobj->maxs, axform,
			io, iv, &id);
		V3_SCALE(iv, -1, iv);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		BSDE_SolidOBB_ObbPlaneExtents(
			aobj->mins, aobj->maxs, axform, norm, ext);
		BSDE_SolidOBB_ObbPlaneIntersect(
			aobj->mins, aobj->maxs, axform, norm, io);
		id=norm[3]-ext[0];
		i=id>0;

//		if(i)printf("collide box plane %f\n", id);

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, norm, id);
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckObbCollide(bobj, aobj,
			bxform, axform);
		if(i)
		{
//			i=BSDE_SolidHull_ContactGeneric(bobj, aobj,
//				bxform, axform, borg, aorg, io, iv, &id);
//			if(i)lst[n++]=BSDE_MakeContact(world, bobj, aobj,
//				io, iv, id);

			i=BSDE_SolidHull_ContactGeneric(bobj, aobj,
				bxform, axform, borg, aorg, pts, iv, &id);
			if(i)lst[n++]=BSDE_MakePointsContact(world, bobj, aobj,
				pts, iv, id, i);
		}
		break;
	case BSDE_SOLID_TRIMESH:
		n=BSDE_SolidBRep_MultiContact(world, bobj, aobj,
			bxform, axform, lst, n);
		break;

	default:
		break;
	}

	return(n);
}

int bsde_collide_sphere(BSDE_World *world,
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	BSDE_Contact **lst, bsde_real *axform, bsde_real *bxform,
	bsde_real *aorg, bsde_real *borg)
{
	bsde_real pts[16*3];
	bsde_real norm[4], f, g;
	bsde_real io[4], iv[4], id;
	int i, n;

	n=0;
	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
//		i=BSDE_SolidAABB_BoxCollideP(
//			aobj->mmins, aobj->mmaxs, bobj->mmins, bobj->mmaxs);
//		if(!i)break;

		i=BSDE_SolidSphere_CollideObbContact(
			aobj->radius, aorg,
			bobj->mins, bobj->maxs, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_SPHERE:
//		i=BSDE_SolidAABB_BoxCollideP(
//			aobj->mmins, aobj->mmaxs, bobj->mmins, bobj->mmaxs);
//		if(!i)break;

		i=BSDE_SolidSphere_CollideContact(
			aobj->radius, aorg, bobj->radius, borg, io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CYLINDER:
		i=BSDE_SolidCylinder_CollideContact(
			0, aobj->radius, 1, axform,
			bobj->height, bobj->radius, 0, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidCylinder_CollideContact(
			0, aobj->radius, 1, axform,
			bobj->height, bobj->radius, 1, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		f=V3_NDOT(aobj->org, norm);
		V3_ADDSCALE(aobj->org, norm, -f, io);
		id=aobj->radius-f;
		i=id>0;
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, norm, id);
//		if(i)printf("sphere collide plane %f\n", id);
		break;
	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckSphereCollide(bobj, aobj,
			bxform, axform, borg, aorg);
		if(i)
		{
			i=BSDE_SolidHull_ContactGeneric(bobj, aobj,
				bxform, axform, borg, aorg, pts, iv, &id);
			if(i)lst[n++]=BSDE_MakePointsContact(world, bobj, aobj,
				pts, iv, id, i);
		}
		break;
	case BSDE_SOLID_TRIMESH:
		n=BSDE_SolidBRep_MultiContact(world, bobj, aobj,
			bxform, axform, lst, n);
		break;

	default:
		break;
	}

	return(n);
}

int bsde_collide_cylinder(BSDE_World *world,
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	BSDE_Contact **lst, bsde_real *axform, bsde_real *bxform,
	bsde_real *aorg, bsde_real *borg, int t)
{
	bsde_real pts[16*3];
	bsde_real norm[4], f, g;
	bsde_real io[4], iv[4], id;
	int i, n;

	n=0;
	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		i=BSDE_SolidCylinder_CollideObbContact(
			aobj->height, aobj->radius, t, axform,
			bobj->mins, bobj->maxs, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidCylinder_CollideContact(
			aobj->height, aobj->radius, t, axform,
			0.0, bobj->radius, 1, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CYLINDER:
		i=BSDE_SolidCylinder_CollideContact(
			aobj->height, aobj->radius, t, axform,
			bobj->height, bobj->radius, 0, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidCylinder_CollideContact(
			aobj->height, aobj->radius, t, axform,
			bobj->height, bobj->radius, 1, bxform,
			io, iv, &id);
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);

		f=V3_NDOT(aorg, norm);
		g=BSDE_SolidCylinder_AxisRadius(
			aobj->height, aobj->radius, t, axform, norm);
		id=g-f;
		i=(f-g)<0;

//		V3_ADDSCALE(aobj->org, norm, -f, io);
		f=V3_NDOT(aorg, norm);
		g=V3_DOT(axform+2*3, norm);
		V3_ADDSCALE(aobj->org, axform+2*3, -f*g, io);

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, norm, id);
		break;

	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckCylinderCollide(bobj, aobj,
			bxform, axform, borg, aorg);

		if(i)
		{
			i=BSDE_SolidHull_ContactGeneric(bobj, aobj,
				bxform, axform, borg, aorg, pts, iv, &id);
			if(i)lst[n++]=BSDE_MakePointsContact(world, bobj, aobj,
				pts, iv, id, i);
		}
		break;
	case BSDE_SOLID_TRIMESH:
		n=BSDE_SolidBRep_MultiContact(world, bobj, aobj,
			bxform, axform, lst, n);
		break;

	default:
		break;
	}

	return(n);
}

int bsde_collide_plane(BSDE_World *world, 
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	BSDE_Contact **lst, bsde_real *axform, bsde_real *bxform,
	bsde_real *aorg, bsde_real *borg)
{
	bsde_real norm[4], ext[2], f, g;
	bsde_real io[4], iv[4], id;
	int i, j, n;

	norm[0]=(aobj->dir[0]*axform[0])+(aobj->dir[1]*axform[4])+
		(aobj->dir[2]*axform[8]);
	norm[1]=(aobj->dir[0]*axform[1])+(aobj->dir[1]*axform[5])+
		(aobj->dir[2]*axform[9]);
	norm[2]=(aobj->dir[0]*axform[2])+(aobj->dir[1]*axform[6])+
		(aobj->dir[2]*axform[10]);
	norm[3]=aobj->dir[3]+V3_DOT(aorg, norm);

	V3_SCALE(norm, -1, iv);

//	printf("collide plane (%f %f %f %f)\n",
//		norm[0], norm[1], norm[2], norm[3]);

	n=0;
	i=-1;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		BSDE_SolidOBB_ObbPlaneExtents(
			bobj->mins, bobj->maxs, bxform, norm, ext);
		BSDE_SolidOBB_ObbPlaneIntersect(
			bobj->mins, bobj->maxs, bxform, norm, io);
		id=norm[3]-ext[0];
		i=id>0;

#if 0
		if(i)
		{
			printf("collide plane (%f %f %f %f)\n",
				norm[0], norm[1], norm[2], norm[3]);
			printf("  (%f %f %f %f)\n",
				io[0], io[1], io[2], io[3]);
			printf("  (%f %f %f %f)\n",
				iv[0], iv[1], iv[2], iv[3]);
			printf("collide plane box %f\n", id);
		}
#endif

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_SPHERE:
		f=V3_NDOT(borg, norm);
		V3_ADDSCALE(borg, norm, -f, io);

		id=bobj->radius-f;
		i=id>0;

//		if(i)printf("collide sphere %f\n", id);

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		j=bobj->solidtype==BSDE_SOLID_CCYLINDER;
		f=V3_NDOT(borg, norm);
		g=BSDE_SolidCylinder_AxisRadius(
			bobj->height, bobj->radius, j, bxform, norm);

		V3_ADDSCALE(borg, norm, -f, io);
		id=g-f;
		i=id>0;

		f=V3_NDOT(borg, norm);
		g=V3_DOT(bxform+2*3, norm);
		if(fabs(g)>0.05)
		{
			V3_ADDSCALE(borg, bxform+2*3, -f*(1.0/g), io);
		}

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	case BSDE_SOLID_PLANE:	//planes don't collide
		i=0;
		break;

	case BSDE_SOLID_HULL:
		f=V3_NDOT(borg, norm);
		V3_ADDSCALE(borg, norm, -f, io);

		BSDE_SolidHull_ProjectLine(bobj, bxform, norm, &f, &g);
		id=norm[3]-f;
		i=id>0;

		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		break;

	default:
		break;
	}
	return(n);
}

int bsde_collide_hull(BSDE_World *world,
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	BSDE_Contact **lst,  bsde_real *axform, bsde_real *bxform,
	bsde_real *aorg, bsde_real *borg)
{
	bsde_real pts[64*3];
	bsde_real norm[4], f, g;
	bsde_real io[4], iv[4], id;
	int i, j, n;

//	i=BSDE_SolidHull_CheckCollide(aobj, bobj);

	n=0;
	i=0;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		i=BSDE_SolidHull_CheckObbCollide(aobj, bobj,
			axform, bxform);
		if(i)
		{
			BSDE_SolidHull_ContactGeneric(aobj, bobj,
				axform, bxform, aorg, borg, io, iv, &id);
			lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		}
		break;

	case BSDE_SOLID_SPHERE:
		i=BSDE_SolidHull_CheckSphereCollide(aobj, bobj,
			axform, bxform, aorg, borg);
		if(i)
		{
			BSDE_SolidHull_ContactGeneric(aobj, bobj,
				axform, bxform, aorg, borg, io, iv, &id);
			lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		}
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		i=BSDE_SolidHull_CheckCylinderCollide(aobj, bobj,
			axform, bxform, aorg, borg);
		if(i)
		{
			BSDE_SolidHull_ContactGeneric(aobj, bobj,
				axform, bxform, aorg, borg, io, iv, &id);
			lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		}
		break;

	case BSDE_SOLID_PLANE:
		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		BSDE_SolidHull_ProjectLine(aobj, axform, norm, &f, &g);
		i=(f-norm[3])<0;

		BSDE_Plane_TransformNormal(bobj->dir, bxform, norm);
		f=V3_NDOT(aorg, norm);
		V3_ADDSCALE(aobj->org, norm, -f, io);

		BSDE_SolidHull_ProjectLine(aobj, axform, norm, &f, &g);
		id=norm[3]-f;
		i=id>0;
		if(i)lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, norm, id);
		break;

	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckCollide(aobj, bobj,
			axform, bxform);
		if(i)
		{
//			i=BSDE_SolidHull_ContactHull(aobj, bobj,
//				axform, bxform, aorg, borg, io, iv, &id);
//			lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);

			i=BSDE_SolidHull_ContactHull(aobj, bobj,
				axform, bxform, aorg, borg, pts, iv, &id);
			if(i)lst[n++]=BSDE_MakePointsContact(world, aobj, bobj,
				pts, iv, id, i);
		}

		break;

	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidHull_CheckTriMeshCollide(aobj, bobj,
			axform, bxform);
		if(i)
		{
			BSDE_SolidHull_ContactGeneric(aobj, bobj,
				axform, bxform, aorg, borg, io, iv, &id);
			lst[n++]=BSDE_MakeContact(world, aobj, bobj, io, iv, id);
		}
		break;

	default:
		break;
	}

	return(n);
}

int bsde_collide(BSDE_World *world,
	BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst,
	bsde_real *axform, bsde_real *bxform, bsde_real *aorg, bsde_real *borg)
{
	int i;

	if((aobj->solidtype==BSDE_SOLID_BSP) ||
		(bobj->solidtype==BSDE_SOLID_BSP))
	{
//		printf("collide bsp\n");
		i=BSDE_SolidBSP_CollideMultiContact(
			world, aobj, axform, bobj, bxform, lst);
		return(i);
	}

	i=-1;
	switch(aobj->solidtype)
	{
	case BSDE_SOLID_AABB:
		i=bsde_collide_aabb(world, aobj, bobj, lst,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_OBB:
		i=bsde_collide_obb(world, aobj, bobj, lst,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_SPHERE:
		i=bsde_collide_sphere(world, aobj, bobj, lst,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_CYLINDER:
		i=bsde_collide_cylinder(world, aobj, bobj, lst,
			axform, bxform, aorg, borg, 0);
		break;
	case BSDE_SOLID_CCYLINDER:
		i=bsde_collide_cylinder(world, aobj, bobj, lst,
			axform, bxform, aorg, borg, 1);
		break;
	case BSDE_SOLID_PLANE:
		i=bsde_collide_plane(world, aobj, bobj, lst,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_HULL:
		i=bsde_collide_hull(world, aobj, bobj, lst,
			axform, bxform, aorg, borg);
		break;
	case BSDE_SOLID_TRIMESH:
		i=BSDE_SolidBRep_MultiContact(world, aobj, bobj,
			axform, bxform, lst, 0);
		break;
	default:
		break;
	}
	return(i);
}

int BSDE_Collide(BSDE_World *world, BSDE_Solid *aobj, BSDE_Solid *bobj,
	BSDE_Contact **lst)
{
	int i;

	i=bsde_collide(world, aobj, bobj, lst,
		aobj->e_xform, bobj->e_xform, aobj->e_org, bobj->e_org);
	return(i);
}

int BSDE_InitialCollide(BSDE_World *world,
	BSDE_Solid *aobj, BSDE_Solid *bobj, BSDE_Contact **lst)
{
	int i;

	i=bsde_collide(world, aobj, bobj, lst,
		aobj->xform, bobj->xform, aobj->org, bobj->org);
	return(i);
}
