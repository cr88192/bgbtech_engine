#include "bsde_internal.h"

BSDE_Joint *BSDE_NewJoint(int type)
{
	BSDE_Joint *tmp;

	tmp=(BSDE_Joint *)malloc(sizeof(BSDE_Joint));
	memset(tmp, 0, sizeof(BSDE_Joint));
	tmp->type=type;

	return(tmp);
}

BSDE_Joint *BSDE_NextJointLink(BSDE_Joint *joint, BSDE_Solid *obj)
{
	if(joint->obja==obj)
	{
		return(joint->linka);
	}else if(joint->objb==obj)
	{
		return(joint->linkb);
	}

	printf("joint link error\n");
	return(NULL);
}

void BSDE_SetNextJointLink(BSDE_Joint *joint, BSDE_Solid *obj,
	BSDE_Joint *nxt)
{
	if(joint->obja==obj)
	{
		joint->linka=nxt;
	}else if(joint->objb==obj)
	{
		joint->linkb=nxt;
	}

	printf("joint link error\n");
}

void BSDE_UnlinkJointSolid(BSDE_Joint *joint, BSDE_Solid *obj)
{
	BSDE_Joint *cur, *nxt, *nxt2;

	cur=obj->joint;

	if(cur==joint)
	{
		nxt=BSDE_NextJointLink(cur, obj);
		obj->joint=nxt;

		BSDE_SetNextJointLink(cur, obj, NULL);
		return;
	}

	while(cur)
	{
		nxt=BSDE_NextJointLink(cur, obj);
		if(nxt==joint)
		{
			nxt2=BSDE_NextJointLink(cur, obj);
			BSDE_SetNextJointLink(cur, obj, nxt2);

			BSDE_SetNextJointLink(nxt, obj, NULL);
			return;
		}
	}

	printf("joint unlink error: not found\n");
	return;
}

void BSDE_BindJointSolids(BSDE_Joint *joint,
	BSDE_Solid *obja, BSDE_Solid *objb)
{
	if(joint->obja)BSDE_UnlinkJointSolid(joint, joint->obja);
	if(joint->objb)BSDE_UnlinkJointSolid(joint, joint->objb);

	joint->obja=obja;
	joint->objb=objb;

	joint->linka=obja->joint;
	joint->linkb=objb->joint;

	obja->joint=joint;
	objb->joint=joint;
}

void BSDE_JointConstrainPoints(BSDE_World *world,
	BSDE_Joint *joint, bsde_real *orga, bsde_real *orgb, bsde_real dt)
{
	bsde_real pa[3], pb[3], na[3];
	bsde_real fa[3], fb[3], va[3], vb[3];
	bsde_real to[3], tv[3];
	bsde_real r, f, g, tf;

	tf=50;

	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);
	V3_NORMALIZE(va, na);

	V3_ADDSCALE(orga, va, 0.5, to);

#if 0
	BSDE_GetPointVelocity(joint->obja, orga, pa);
	BSDE_GetPointVelocity(joint->objb, orgb, pb);

	V3_COPY(joint->obja->vel, tv);
	V3_ADD(tv, joint->objb->vel, tv);
	V3_SCALE(tv, 0.5, tv);

	if(
		!(joint->obja->moveflags&
			(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC))
		&&
		!(joint->objb->moveflags&
			(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC))
		)
	{
		V3_SUB(to, orga, fa);
		V3_SUB(to, orgb, fb);
		V3_ADD(joint->obja->org, fa, joint->obja->e_org);
		V3_ADD(joint->objb->org, fb, joint->objb->e_org);

		V3_COPY(tv, joint->obja->e_vel);
		V3_COPY(tv, joint->objb->e_vel);
	}else if(!(joint->obja->moveflags&
		(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC)))
	{
		V3_ADD(joint->obja->org, va, joint->obja->e_org);
		V3_COPY(joint->objb->vel, tv);
		V3_COPY(tv, joint->obja->e_vel);
	}else
	{
		V3_ADD(joint->objb->org, vb, joint->objb->e_org);
		V3_COPY(joint->obja->vel, tv);
		V3_COPY(tv, joint->objb->e_vel);
	}

	V3_ZERO(joint->obja->tvel);
	V3_ZERO(joint->objb->tvel);

	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);
	BSDE_ApplyPointAngleForce(joint->obja, orga, fa);
	BSDE_ApplyPointAngleForce(joint->objb, orgb, fb);
#endif

#if 0
	BSDE_GetPointVelocity(joint->obja, orga, pa);
	BSDE_GetPointVelocity(joint->objb, orgb, pb);

	f=V3_DOT(pa, na);
	g=V3_DOT(pb, na);
	r=f-g;
	V3_ADDSCALE(va, na, -r, va);
	V3_ADDSCALE(vb, na, r, vb);

//	V3_SCALE(na, -r, va);
//	V3_SCALE(na, r, vb);

	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyPointForce(joint->obja, orga, fa);
	BSDE_ApplyPointForce(joint->objb, orgb, fb);
#endif

#if 1
	BSDE_GetVelocity(joint->obja, pa);
	BSDE_GetVelocity(joint->objb, pb);

	f=V3_DOT(pa, na);
	g=V3_DOT(pb, na);
//	r=(f-g)*0.25;
	r=(f-g)*0.5;
	V3_ADDSCALE(va, na, -r, va);
	V3_ADDSCALE(vb, na, r, vb);

	BSDE_ApplyAcceleration(joint->obja, va);
	BSDE_ApplyAcceleration(joint->objb, vb);


	BSDE_GetPointAngleVelocity(joint->obja, orga, pa);
	BSDE_GetPointAngleVelocity(joint->objb, orgb, pb);

	f=V3_DOT(pa, na);
	g=V3_DOT(pb, na);
//	r=(f-g)*0.25;
	r=(f-g)*0.5;
	V3_ADDSCALE(va, na, -r, va);
	V3_ADDSCALE(vb, na, r, vb);

	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyPointAngleForce(joint->obja, orga, fa);
	BSDE_ApplyPointAngleForce(joint->objb, orgb, fb);
#endif
}

void BSDE_JointConstrainPoints2(BSDE_World *world,
	BSDE_Joint *joint, bsde_real *orga, bsde_real *orgb, bsde_real dt)
{
	bsde_real pa[3], pb[3], na[3];
	bsde_real fa[3], fb[3], va[3], vb[3];
	bsde_real r, f, g;

	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);
	V3_NORMALIZE(va, na);

	BSDE_GetPointAngleVelocity(joint->obja, orga, pa);
	BSDE_GetPointAngleVelocity(joint->objb, orgb, pb);

	f=V3_DOT(pa, na);
	g=V3_DOT(pb, na);
	r=(f-g)*0.25;
//	r=(f-g)*0.5;
	V3_ADDSCALE(va, na, -r, va);
	V3_ADDSCALE(vb, na, r, vb);

	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyPointAngleForce(joint->obja, orga, fa);
	BSDE_ApplyPointAngleForce(joint->objb, orgb, fb);
}

void BSDE_JointConstrainPointsDirect(BSDE_World *world,
	BSDE_Joint *joint, bsde_real *orga, bsde_real *orgb, bsde_real dt)
{
	bsde_real pa[3], pb[3], na[3];
	bsde_real fa[3], fb[3], va[3], vb[3];
	bsde_real to[3], tv[3];
	bsde_real r, f, g, tf;

	tf=50;

	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);
	V3_NORMALIZE(va, na);

	V3_ADDSCALE(orga, va, 0.5, to);

	BSDE_GetPointVelocity(joint->obja, orga, pa);
	BSDE_GetPointVelocity(joint->objb, orgb, pb);

	V3_COPY(joint->obja->vel, tv);
	V3_ADD(tv, joint->objb->vel, tv);
	V3_SCALE(tv, 0.5, tv);

	if(
		!(joint->obja->moveflags&
			(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC))
		&&
		!(joint->objb->moveflags&
			(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC))
		)
	{
		V3_SUB(to, orga, fa);
		V3_SUB(to, orgb, fb);
		V3_ADD(joint->obja->org, fa, joint->obja->org);
		V3_ADD(joint->objb->org, fb, joint->objb->org);

		V3_ADDSCALE(joint->obja->vel, fa, 1.0/dt, joint->obja->vel);
		V3_ADDSCALE(joint->objb->vel, fb, 1.0/dt, joint->objb->vel);
	}else if(!(joint->obja->moveflags&
		(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC)))
	{
		V3_ADD(joint->obja->org, va, joint->obja->org);
		V3_ADDSCALE(joint->obja->vel, va, 1.0/dt, joint->obja->vel);
	}else
	{
		V3_ADD(joint->objb->org, vb, joint->objb->org);
		V3_ADDSCALE(joint->objb->vel, vb, 1.0/dt, joint->objb->vel);
	}

	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);
	BSDE_ApplyPointAngleForce(joint->obja, orga, fa);
	BSDE_ApplyPointAngleForce(joint->objb, orgb, fb);
}

void BSDE_HandleJointFixed(BSDE_World *world, BSDE_Joint *joint, bsde_real dt)
{
	bsde_real orga[3], orgb[3];
	bsde_real org1a[3], org1b[3];
	bsde_real org2a[3], org2b[3];
	bsde_real org3a[3], org3b[3];

	if(!joint->obja)return;
	if(!joint->objb)return;

	BSDE_Plane_TransformPoint(joint->orga, joint->obja->e_xform, orga);
	BSDE_Plane_TransformPoint(joint->orgb, joint->objb->e_xform, orgb);
	BSDE_Plane_TransformPoint(joint->org1a, joint->obja->e_xform, org1a);
	BSDE_Plane_TransformPoint(joint->org1b, joint->objb->e_xform, org1b);
	BSDE_Plane_TransformPoint(joint->org2a, joint->obja->e_xform, org2a);
	BSDE_Plane_TransformPoint(joint->org2b, joint->objb->e_xform, org2b);
	BSDE_Plane_TransformPoint(joint->org3a, joint->obja->e_xform, org3a);
	BSDE_Plane_TransformPoint(joint->org3b, joint->objb->e_xform, org3b);

	BSDE_JointConstrainPoints(world, joint, orga, orgb, dt);
	BSDE_JointConstrainPoints2(world, joint, org1a, org1b, dt);
	BSDE_JointConstrainPoints2(world, joint, org2a, org2b, dt);
	BSDE_JointConstrainPoints2(world, joint, org3a, org3b, dt);
}

void BSDE_HandleJointBall(BSDE_World *world, BSDE_Joint *joint, bsde_real dt)
{
	bsde_real orga[3], orgb[3], org[3];
	bsde_real pa[3], pb[3], na[3], nb[3];
	bsde_real fa[3], fb[3], va[3], vb[3];
	bsde_real r, f, g;

	if(!joint->obja)return;
	if(!joint->objb)return;
//	printf("handle joint ball\n");

	BSDE_Plane_TransformPoint(
		joint->orga, joint->obja->e_xform, orga);
	BSDE_Plane_TransformPoint(
		joint->orgb, joint->objb->e_xform, orgb);

//	printf("orga %f %f %f\n", orga[0], orga[1], orga[2]);
//	printf("orgb %f %f %f\n", orgb[0], orgb[1], orgb[2]);

	BSDE_GetVelocity(joint->obja, pa);
	BSDE_GetVelocity(joint->objb, pb);

	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);

//	V3_SCALE(va, 0.5, va);
//	V3_SCALE(vb, 0.5, vb);


	V3_NORMALIZE(va, na);
//	V3_NORMALIZE(vb, nb);
	f=V3_DOT(pa, na);
	g=V3_DOT(pb, na);
	r=(f-g)*0.5;
//	r=f-g;
	V3_ADDSCALE(va, na, -r, va);
	V3_ADDSCALE(vb, na, r, vb);

//	printf("fa %f %f %f\n", fa[0], fa[1], fa[2]);
//	printf("fb %f %f %f\n", fb[0], fb[1], fb[2]);

	BSDE_ApplyAcceleration(joint->obja, va);
	BSDE_ApplyAcceleration(joint->objb, vb);


	BSDE_GetPointAngleVelocity(joint->obja, orga, pa);
	BSDE_GetPointAngleVelocity(joint->objb, orgb, pb);

	f=V3_DOT(pa, na);
	g=V3_DOT(pb, na);
	r=(f-g)*0.5;
//	r=f-g;
	V3_ADDSCALE(va, na, -r, va);
	V3_ADDSCALE(vb, na, r, vb);


	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyPointAngleForce(joint->obja, orga, fa);
	BSDE_ApplyPointAngleForce(joint->objb, orgb, fb);
}

void BSDE_HandleJointHinge(BSDE_World *world, BSDE_Joint *joint, bsde_real dt)
{
	bsde_real orga[3], orgb[3], org[3];
	bsde_real dira[3], dirb[3], dir[3];
	bsde_real org2a[3], org2b[3];
	bsde_real lpa[3], lpb[3];
	bsde_real apa[3], apb[3];
	bsde_real ap2a[3], ap2b[3];
	bsde_real fa[3], fb[3], va[3], vb[3];
	bsde_real r;

	if(!joint->obja)return;
	if(!joint->objb)return;
//	printf("handle joint hinge\n");

	BSDE_Plane_TransformPoint(
		joint->orga, joint->obja->e_xform, orga);
	BSDE_Plane_TransformPoint(
		joint->orgb, joint->objb->e_xform, orgb);
	BSDE_Plane_TransformVector(
		joint->dira, joint->obja->e_xform, dira);
	BSDE_Plane_TransformVector(
		joint->dirb, joint->objb->e_xform, dirb);

//	V3_ADDSCALE(orga, dira, 100, org2a);
//	V3_ADDSCALE(orgb, dirb, 100, org2b);

//	V3_ADDSCALE(orga, dira, 0.75, org2a);
//	V3_ADDSCALE(orgb, dirb, 0.75, org2b);

	V3_ADDSCALE(orga, dira, 1/dt, org2a);
	V3_ADDSCALE(orgb, dirb, 1/dt, org2b);

	BSDE_JointConstrainPoints(world, joint, orga, orgb, dt);
//	BSDE_JointConstrainPoints2(world, joint, org2a, org2b, dt);

#if 1
	BSDE_GetVelocity(joint->obja, lpa);
	BSDE_GetVelocity(joint->objb, lpb);
	BSDE_GetPointAngleVelocity(joint->obja, orga, apa);
	BSDE_GetPointAngleVelocity(joint->objb, orgb, apb);
	BSDE_GetPointAngleVelocity(joint->obja, org2a, ap2a);
	BSDE_GetPointAngleVelocity(joint->objb, org2b, ap2b);
#endif

#if 0
	//linear force at orgs
	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);
	V3_SCALE(va, 1, va);
	V3_SCALE(vb, 1, vb);

	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyForce(joint->obja, fa);
	BSDE_ApplyForce(joint->objb, fb);

	//angular force at orgs
	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);
	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyPointAngleForce(joint->obja, orga, fa);
	BSDE_ApplyPointAngleForce(joint->objb, orgb, fb);
#endif

#if 1
	//angular force at org2
	V3_SUB(org2b, org2a, va);
	V3_SUB(org2a, org2b, vb);
//	V3_SUB(org2a, org2b, va);
//	V3_SUB(org2b, org2a, vb);
	V3_SCALE(va, joint->obja->mass*dt, fa);
	V3_SCALE(vb, joint->objb->mass*dt, fb);

	BSDE_ApplyPointAngleForce(joint->obja, org2a, fa);
	BSDE_ApplyPointAngleForce(joint->objb, org2b, fb);
#endif
}

void BSDE_HandleJointUniversal(BSDE_World *world,
	BSDE_Joint *joint, bsde_real dt)
{
	bsde_real orga[3], orgb[3], org[3];
	bsde_real dira[3], dirb[3], dir[3];
	bsde_real dir2a[3], dir2b[3];
	bsde_real org2a[3], org2b[3];
	bsde_real org3a[3], org3b[3];
	bsde_real lpa[3], lpb[3];
	bsde_real apa[3], apb[3];
	bsde_real ap2a[3], ap2b[3];
	bsde_real fa[3], fb[3], va[3], vb[3];
	bsde_real r;

	if(!joint->obja)return;
	if(!joint->objb)return;
//	printf("handle joint universal\n");

	BSDE_Plane_TransformPoint(
		joint->orga, joint->obja->e_xform, orga);
	BSDE_Plane_TransformPoint(
		joint->orgb, joint->objb->e_xform, orgb);
	BSDE_Plane_TransformVector(
		joint->dira, joint->obja->e_xform, dira);
	BSDE_Plane_TransformVector(
		joint->dirb, joint->objb->e_xform, dirb);

	V3_CROSS(dira, dirb, dir);
	V3_CROSS(dir, dira, dir2b);
	V3_CROSS(dirb, dir, dir2a);

	V3_NORMALIZE(dir2a, dir2a);
	V3_NORMALIZE(dir2b, dir2b);

//	printf("A (%f %f %f) (%f %f %f)\n", dira[0], dira[1], dira[2],
//		dir2a[0], dir2a[1], dir2a[2]);
//	printf("B (%f %f %f) (%f %f %f)\n", dirb[0], dirb[1], dirb[2],
//		dir2b[0], dir2b[1], dir2b[2]);

	V3_ADDSCALE(orga, dira, 100, org2a);
	V3_ADDSCALE(orgb, dirb, 100, org2b);

	V3_ADDSCALE(orga, dir2a, 100, org3a);
	V3_ADDSCALE(orgb, dir2b, 100, org3b);


	BSDE_GetVelocity(joint->obja, lpa);
	BSDE_GetVelocity(joint->objb, lpb);
	BSDE_GetPointAngleVelocity(joint->obja, orga, apa);
	BSDE_GetPointAngleVelocity(joint->objb, orgb, apb);
	BSDE_GetPointAngleVelocity(joint->obja, org2a, ap2a);
	BSDE_GetPointAngleVelocity(joint->objb, org2b, ap2b);

	//linear force at orgs
	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);
	V3_SCALE(va, 1, va);
	V3_SCALE(vb, 1, vb);

	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyForce(joint->obja, fa);
	BSDE_ApplyForce(joint->objb, fb);

	//angular force at orgs
	V3_SUB(orgb, orga, va);
	V3_SUB(orga, orgb, vb);
	V3_SCALE(va, joint->obja->mass, fa);
	V3_SCALE(vb, joint->objb->mass, fb);

	BSDE_ApplyPointAngleForce(joint->obja, orga, fa);
	BSDE_ApplyPointAngleForce(joint->objb, orgb, fb);

	//angular force at org2
	V3_SUB(org3a, org2a, va);
	V3_SUB(org3b, org2b, vb);
	V3_SCALE(va, joint->obja->mass*dt, fa);
	V3_SCALE(vb, joint->objb->mass*dt, fb);

	BSDE_ApplyPointAngleForce(joint->obja, org2a, fa);
	BSDE_ApplyPointAngleForce(joint->objb, org2b, fb);
}

void BSDE_HandleJointElastic(BSDE_World *world,
	BSDE_Joint *joint, bsde_real dt)
{
	bsde_real orga[3], orgb[3], org[3];
	bsde_real pa[3], pb[3], na[3], nb[3];
	bsde_real fa[3], fb[3], va[3], vb[3];
	bsde_real r, f, g;

	if(!joint->obja)return;
	if(!joint->objb)return;
//	printf("handle joint ball\n");

	BSDE_Plane_TransformPoint(
		joint->orga, joint->obja->e_xform, orga);
	BSDE_Plane_TransformPoint(
		joint->orgb, joint->objb->e_xform, orgb);

//	r=V3_DIST(orga, orgb)-joint->radius;
//	if(r<=0)return;

	r=V3_DIST(orga, orgb);
	if(r<=joint->radius)return;

	V3_SUB(orgb, orga, va);
	V3_NORMALIZE(va, na);

	f=((joint->attn[0]*r+joint->attn[1])*r+joint->attn[2])*r+
		joint->attn[3];
//	if(joint->str[0]!=0)f*=joint->str[0];

	V3_SCALE(na, f, fa);
	V3_SCALE(na, -f, fb);

	BSDE_ApplyPointForce(joint->obja, orga, fa);
	BSDE_ApplyPointForce(joint->objb, orgb, fb);
}

void BSDE_HandleJoint(BSDE_World *world, BSDE_Joint *joint, bsde_real dt)
{
//	printf("handle joint\n");

	switch(joint->type)
	{
	case BSDE_JOINT_BALL:
		BSDE_HandleJointBall(world, joint, dt);
		break;
	case BSDE_JOINT_HINGE:
		BSDE_HandleJointHinge(world, joint, dt);
		break;
	case BSDE_JOINT_UNIVERSAL:
		BSDE_HandleJointUniversal(world, joint, dt);
		break;
	case BSDE_JOINT_FIXED:
		BSDE_HandleJointFixed(world, joint, dt);
		break;
	case BSDE_JOINT_ELASTIC:
		BSDE_HandleJointElastic(world, joint, dt);
		break;
	default:
		break;
	}

	if(joint->obja)BSDE_UpdateVel(world, joint->obja, dt);
	if(joint->objb)BSDE_UpdateVel(world, joint->objb, dt);
}

bsde_real BSDE_GetJointAttrF(BSDE_Joint *self, int var)
{
	bsde_real f;

	f=0;
	switch(var)
	{
	case BSDE_STRENGTH_0:
		f=self->str[0];
		break;
	case BSDE_STRENGTH_1:
		f=self->str[1];
		break;
	case BSDE_STRENGTH_2:
		f=self->str[2];
		break;
	case BSDE_STRENGTH_3:
		f=self->str[3];
		break;
	case BSDE_CONSTANT_ATTENUATION:
		f=self->attn[3];
		break;
	case BSDE_LINEAR_ATTENUATION:
		f=self->attn[2];
		break;
	case BSDE_QUADRATIC_ATTENUATION:
		f=self->attn[1];
		break;
	case BSDE_CUBIC_ATTENUATION:
		f=self->attn[0];
		break;

	case BSDE_RADIUS:
		f=self->radius;
		break;
	default:
		break;
	}
	return(f);
}

void BSDE_SetJointAttrF(BSDE_Joint *self, int var, bsde_real val)
{
	switch(var)
	{
	case BSDE_STRENGTH_0:
		self->str[0]=val;
		break;
	case BSDE_STRENGTH_1:
		self->str[1]=val;
		break;
	case BSDE_STRENGTH_2:
		self->str[2]=val;
		break;
	case BSDE_STRENGTH_3:
		self->str[3]=val;
		break;
	case BSDE_CONSTANT_ATTENUATION:
		self->attn[3]=val;
		break;
	case BSDE_LINEAR_ATTENUATION:
		self->attn[2]=val;
		break;
	case BSDE_QUADRATIC_ATTENUATION:
		self->attn[1]=val;
		break;
	case BSDE_CUBIC_ATTENUATION:
		self->attn[0]=val;
		break;

	case BSDE_RADIUS:
		self->radius=val;
		break;
	default:
		break;
	}
}

int BSDE_GetJointAttrI(BSDE_Joint *self, int var)
{
	int i;

	i=0;
	switch(var)
	{
	default:
		break;
	}
	return(i);
}

void BSDE_SetJointAttrI(BSDE_Joint *self, int var, int val)
{
	int i;
	switch(var)
	{
	default:
		break;
	}
}

void BSDE_GetJointAttrFV(BSDE_Joint *self, int var, float *val, int n)
{
	double vec[64];
	int i;

	BSDE_GetJointAttrDV(self, var, vec, n);
	for(i=0; i<n; i++)val[i]=vec[i];
}

void BSDE_SetJointAttrFV(BSDE_Joint *self, int var, float *val, int n)
{
	double vec[64];
	int i;

	for(i=0; i<n; i++)vec[i]=val[i];
	BSDE_SetJointAttrDV(self, var, vec, n);
}

void BSDE_GetJointAttrDV(BSDE_Joint *self, int var, double *val, int n)
{
	switch(var)
	{
	default:
		break;
	}
}

void BSDE_SetJointAttrDV(BSDE_Joint *self, int var, double *v, int n)
{
	bsde_real val[16];
	bsde_real org[3];
	int i;

	for(i=0; i<n; i++)val[i]=v[i];

	switch(var)
	{
	case BSDE_ORIGIN:
		BSDE_TransformPointLocal(self->obja, val, self->orga);
		BSDE_TransformPointLocal(self->objb, val, self->orgb);
		break;
	case BSDE_ORIGIN_1:
		BSDE_TransformPointLocal(self->obja, val, self->org1a);
		BSDE_TransformPointLocal(self->objb, val, self->org1b);
		break;
	case BSDE_ORIGIN_2:
		BSDE_TransformPointLocal(self->obja, val, self->org2a);
		BSDE_TransformPointLocal(self->objb, val, self->org2b);
		break;
	case BSDE_ORIGIN_3:
		BSDE_TransformPointLocal(self->obja, val, self->org3a);
		BSDE_TransformPointLocal(self->objb, val, self->org3b);
		break;
	case BSDE_ORIGIN_123:
		BSDE_TransformPointLocal(self->obja, val, self->orga);
		BSDE_TransformPointLocal(self->objb, val, self->orgb);
		V3_COPY(val, org); org[0]+=1;
		BSDE_TransformPointLocal(self->obja, org, self->org1a);
		BSDE_TransformPointLocal(self->objb, org, self->org1b);
		V3_COPY(val, org); org[1]+=1;
		BSDE_TransformPointLocal(self->obja, org, self->org2a);
		BSDE_TransformPointLocal(self->objb, org, self->org2b);
		V3_COPY(val, org); org[2]+=1;
		BSDE_TransformPointLocal(self->obja, org, self->org3a);
		BSDE_TransformPointLocal(self->objb, org, self->org3b);
		break;

	case BSDE_ORIGIN_A:
		BSDE_TransformPointLocal(self->obja, val, self->orga);
		break;
	case BSDE_ORIGIN_B:
		BSDE_TransformPointLocal(self->objb, val, self->orgb);
		break;
	case BSDE_ORIGIN_RA:
		V3_COPY(val, self->orga);
		break;
	case BSDE_ORIGIN_RB:
		V3_COPY(val, self->orgb);
		break;

	case BSDE_DIRECTION:
		BSDE_TransformVectorLocal(self->obja, val, self->dira);
		BSDE_TransformVectorLocal(self->objb, val, self->dirb);
		break;
	case BSDE_DIRECTION_1:
		BSDE_TransformVectorLocal(self->obja, val, self->dir1a);
		BSDE_TransformVectorLocal(self->objb, val, self->dir1b);
		break;

	case BSDE_DIRECTION_A:
		BSDE_TransformVectorLocal(self->obja, val, self->dira);
		break;
	case BSDE_DIRECTION_B:
		BSDE_TransformVectorLocal(self->objb, val, self->dirb);
		break;
	case BSDE_DIRECTION_RA:
		V3_COPY(val, self->orga);
		break;
	case BSDE_DIRECTION_RB:
		V3_COPY(val, self->orgb);
		break;

	default:
		break;
	}
}

void BSDE_SetJointCubic(BSDE_Joint *self,
	bsde_real vx, bsde_real vy, bsde_real sx, bsde_real sy,
	bsde_real fa, bsde_real fb, bsde_real fc, bsde_real fd)
{
	bsde_real a, b, c, d, sxy, vx2, vx3;
	//Sy*(Sx*a(x-Vx)^3+Sx*b(x-Vx)^2+Sx*c(x-Vx)+d)+Vy

	//a(x-y)^3=a(x-y)(x-y)(x-y)=a(x-y)(x^2-2xy+y^2)=
	//ax^3 - 3ax^2y + 3axy^2 - ay^3
	//b(x-y)^2 = bx^2 - 2bxy + by^2
	//c(x-y) = cx-cy

	//ax^3 - 3ax^2y + 3axy^2 - ay^3 + bx^2 -
	//	2bxy + by^2 + cx - cy

	vx2=vx*vx;
	vx3=vx2*vx;

	sxy=sx*sy;

	a=fa*sxy;
	b=-3*fa*vx*sxy + fb*sxy;
	c=3*fa*vx2*sxy - 2*fb*vx*sxy + fc*sxy;
	d=-fa*vx3*sxy + fb*vx2*sxy - fc*vx*sxy + fd*sy + vy;

	self->attn[0]=a;
	self->attn[1]=b;
	self->attn[2]=c;
	self->attn[3]=d;

//	printf("attn %gx^3 + %gx^2 + %gx + %g\n", a, b, c, d);
}
