#include "bsde_internal.h"

void BSDE_CalcPointVelocity(bsde_real *org, bsde_real *vel, bsde_real *avel,
	bsde_real *point, bsde_real *pvel)
{
	bsde_real io[3];
	V3_SUB(point, org, io);
	V3_CROSS(avel, io, pvel);
	V3_ADD(pvel, vel, pvel);
}

void BSDE_ApplyForce(BSDE_Solid *self, bsde_real *dir)
{
	bsde_real vel[3];

	V3_SCALE(dir, 1/self->mass, vel);
	V3_ADD(self->ivel, vel, self->ivel);
}

void BSDE_ApplyAcceleration(BSDE_Solid *self, bsde_real *dir)
{
	V3_ADD(self->ivel, dir, self->ivel);
}

void BSDE_GetVelocity(BSDE_Solid *self, bsde_real *vel)
{
	V3_COPY(self->vel, vel);
}

void BSDE_GetPointAngleVelocity(BSDE_Solid *self, bsde_real *org, bsde_real *vel)
{
	bsde_real io[3];

	V3_SUB(org, self->org, io);
	V3_CROSS(self->avel, io, vel);
}

void BSDE_GetPointVelocity(BSDE_Solid *self, bsde_real *org, bsde_real *vel)
{
	bsde_real io[3];

	V3_SUB(org, self->org, io);

	V3_ZERO(vel);
	V3_CROSS(self->avel, io, vel);
	V3_ADD(vel, self->vel, vel);
}

void BSDE_GetPointForce(BSDE_Solid *self, bsde_real *org, bsde_real *vel)
{
	bsde_real io[3];

	V3_SUB(org, self->org, io);

	V3_ZERO(vel);
	V3_CROSS(self->avel, io, vel);
	V3_ADD(vel, self->vel, vel);
	V3_SCALE(vel, self->mass, vel);
}

#if 0
void BSDE_ApplyPointAcceleration(BSDE_Solid *self,
	bsde_real *org, bsde_real *vel)
{
	bsde_real io[3], nra[3];

	V3_SUB(org, self->org, io);
	V3_CROSS(io, vel, nra);
	V3_ADD(self->itvel, nra, self->itvel);
	V3_ADD(self->ivel, vel, self->ivel);
}
#endif

void BSDE_ApplyPointForce(BSDE_Solid *self,
	bsde_real *org, bsde_real *dir)
{
	bsde_real io[3], nra[3];
	bsde_real vel[3], f;
	bsde_real va[3], vb[3];

//	printf("(%f %f %f) %f\n", dir[0], dir[1], dir[2], self->mass);

//	f=V3_LEN(dir)/self->mass;
//	if(f>100)return;

	V3_SUB(org, self->org, io);
	V3_CROSS(io, dir, nra);
	V3_ADD(self->itvel, nra, self->itvel);

//	V3_ADD(self->ivel, dir, self->ivel);

//	MatN_MatMult(self->wimat, 3, 3, io, 3, 1, avel);
//	f=V3_LEN(avel);
//	if(f>1)f=1;

#if 1
	V3_NORMALIZE(dir, va);
	V3_NORMALIZE(io, vb);
	f=fabs(V3_DOT(va, vb));

	V3_SCALE(dir, f/self->mass, vel);
#else
	V3_SCALE(dir, 1/self->mass, vel);
#endif

	V3_ADD(self->ivel, vel, self->ivel);
}

void BSDE_ApplyPointAngleForce(BSDE_Solid *self,
	bsde_real *org, bsde_real *dir)
{
	bsde_real io[3], nra[3];

	V3_SUB(org, self->org, io);
	V3_CROSS(io, dir, nra);
	V3_ADD(self->itvel, nra, self->itvel);
}

void BSDE_ApplyTorque(BSDE_Solid *self, bsde_real *dir)
{
	V3_ADD(self->itvel, dir, self->itvel);
}


void BSDE_HandleFluidContact(BSDE_World *world,
	BSDE_Contact *self, bsde_real dt)
{
	BSDE_Solid *obja, *objb, *objt;
	bsde_real gn[3], ext[4], dv[3], v[3];
	bsde_real da, db, gl, d0, d1, d, f, g;

	if(self->obja->moveflags&BSDE_MOVEFL_FLUID)
	{
		obja=self->obja;
		objb=self->objb;
	}else
	{
		obja=self->objb;
		objb=self->obja;
	}

	if((objb->moveflags&BSDE_MOVEFL_FLUID) ||
		(objb->moveflags&(BSDE_MOVEFL_STATIC|
			BSDE_MOVEFL_SEMISTATIC)))
	{
		if((objb->volume>obja->volume) ||
			(objb->moveflags&(BSDE_MOVEFL_STATIC|
				BSDE_MOVEFL_SEMISTATIC)))
		{
			objt=obja;
			obja=objb;
			objb=objt;
		}
	}

	if(objb->moveflags&(BSDE_MOVEFL_STATIC|
		BSDE_MOVEFL_SEMISTATIC))
			return;

	if(obja->moveflags&BSDE_MOVEFL_NONSOLID)
		return;

	//bouyancy

	gl=V3_NORMALIZE(world->gravity, gn);

	da=obja->mass/obja->volume;
	db=objb->mass/objb->volume;

	BSDE_PlaneExtents(obja, gn, ext+0, ext+1);
	BSDE_PlaneExtents(objb, gn, ext+2, ext+3);
	d0=(ext[0]>ext[2])?ext[0]:ext[2];
	d1=(ext[1]<ext[3])?ext[1]:ext[3];
	d=d1-d0;

	g=objb->volume*(d/(ext[3]-ext[2]));
//	f=d*(da/db)*gl*objb->mass;
	f=g*da*gl*dt;
	V3_SCALE(gn, -f, dv);

	BSDE_ApplyForce(objb, dv);


	//viscosity

//	BSDE_GetPointVelocity(objb, self->org, v);
	V3_COPY(objb->vel, v);

//	g=cbrt(objb->volume);
	g=sqrt(objb->volume);
	f=(g*g)*(d/(ext[3]-ext[2]));	//approx submerged area

//	g=0.25;
//	g=0.01;
	g=obja->viscosity+(da/objb->mass);
	f=g*f*dt;

//	V3_ADDSCALE(dv, objb->vel, -f, dv);

	f=(f<0)?0.0:((f>1)?1.0:f);
	V3_SCALE(v, -f*objb->mass, dv);

	BSDE_ApplyForce(objb, dv);
//	BSDE_ApplyPointForce(objb, self->org, dv);


	//viscosity torque

	V3_COPY(objb->tvel, v);

//	g=cbrt(objb->volume);
	g=sqrt(objb->volume);
	f=(g*g)*(d/(ext[3]-ext[2]));	//approx submerged area

//	g=obja->viscosity+(da/objb->mass);
	g=obja->viscosity;
	f=g*f*dt;

	V3_SCALE(v, -f, dv);
	BSDE_ApplyTorque(objb, dv);

}

void BSDE_ComputeImpulse(BSDE_Solid *obja,
	bsde_real *org, bsde_real *normal, bsde_real *force)
{
	bsde_real pa[3];
	bsde_real v0[3], v1[3], v2[3];
	bsde_real e1, e2;

//	printf("%f\n", obj->mass);

	BSDE_GetPointVelocity(obja, org, pa);

	V3_SUB(org, obja->org, v0);
	V3_CROSS(v0, normal, v1);
	BSDE_MatN_MatMult(obja->wimat, 3, 3, v1, 3, 1, v2);
//	BSDE_MatN_MatMult(v1, 1, 3, obja->wimat, 3, 3, v2);
	V3_CROSS(v2, v0, v1);

	e1=-(1+0.5)*V3_DOT(pa, normal);
	e2=(1.0/obja->mass)+V3_DOT(v1, normal);
	V3_SCALE(normal, e1/e2, force);
}

void BSDE_ComputeImpulse2(BSDE_Solid *obja, BSDE_Solid *objb,
	bsde_real *org, bsde_real *normal, bsde_real depth, bsde_real dt,
	bsde_real *fa, bsde_real *fb)
{
	bsde_real pa[3], pb[3];
	bsde_real v0[3], v1[3], v2[3], v3[3];
	bsde_real va[3], vb[3], ua[3], ub[3];
	bsde_real da, db, dc, dd;
	bsde_real e1, e2, s, e, d;
	int i;

//	printf("%f\n", obj->mass);

	BSDE_GetPointVelocity(obja, org, pa);
	BSDE_GetPointVelocity(objb, org, pb);

	da=V3_DOT(pa, normal);
	db=V3_DOT(pb, normal);
//	s=(da-db)-depth*10;
	s=(da-db)-depth;
//	s=da-db;
//	s=db-da;
//	s=(da-db-0.1)*depth;
//	s=(da-db)*dt;

//	s-=depth;
	if(s>0)s=0;

	V3_SUB(org, obja->org, v0);
	V3_CROSS(v0, normal, v1);
	BSDE_MatN_MatMult(obja->wimat, 3, 3, v1, 3, 1, v2);
//	BSDE_MatN_MatMult(v1, 1, 3, obja->wimat, 3, 3, v2);
	V3_CROSS(v2, v0, va);

	V3_SUB(org, objb->org, v0);
	V3_CROSS(v0, normal, v1);
	BSDE_MatN_MatMult(objb->wimat, 3, 3, v1, 3, 1, v2);
//	BSDE_MatN_MatMult(v1, 1, 3, obja->wimat, 3, 3, v2);
	V3_CROSS(v2, v0, vb);

	e=obja->elastic*objb->elastic;
	e=(e<0)?0:((e>1)?1:e);	//clamp 0..1
//	e=0.5;
//	e=0.0;
	d=1;

//	e1=-e*s;
//	e2=(1.0/obja->mass)+(1.0/objb->mass);

//	e1=2*depth+0.5;
//	e2=(1.0/obja->mass)+(1.0/objb->mass);

	e1=-(1+e)*s;
//	e1=-(1+e)*s+depth;
	e2=(1.0/obja->mass)+(1.0/objb->mass)+
		V3_DOT(va, normal)+V3_DOT(vb, normal);
	V3_SCALE(normal, (e1/e2)*d, fa);
	V3_SCALE(normal, (-e1/e2)*d, fb);

#if 0
//original

//	d=0.75;
//	d=0.25;
//	d=0.10;
	d=obja->friction*objb->friction;
	d=(d<0)?0:((d>1)?1:d);	//clamp 0..1

	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);
	V3_ADDSCALE(fa, va, -d*obja->mass, fa);
	V3_ADDSCALE(fb, vb, -d*objb->mass, fb);
#endif

#if 0
	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);

	d=obja->friction*objb->friction;
	e=V3_DIST(va, vb);
	d=(e*d)/(e*e+1);

	d=(d<0)?0:((d>1)?1:d);	//clamp 0..1
	V3_ADDSCALE(fa, va, -d*obja->mass, fa);
	V3_ADDSCALE(fb, vb, -d*objb->mass, fb);
#endif

//	return;

	if(obja->moveflags&BSDE_MOVEFL_ACTOR)
	{
		if(fabs(normal[2])<0.1)return;
		if(normal[2]<0)return;
	}

	if(objb->moveflags&BSDE_MOVEFL_ACTOR)
	{
		if(fabs(normal[2])<0.1)return;
		if(normal[2]>0)return;
	}

#if 1
//friction
	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);

	V3_SCALE(va, obja->mass, v0);
	V3_SCALE(vb, objb->mass, v1);
	V3_ADD(v0, v1, v2);
	V3_SCALE(v2, 1.0/(obja->mass+objb->mass), v2);

	d=obja->friction*objb->friction;

	e=V3_DIST(va, v2);
	e1=(e*d)/(e*e+1);
	e=V3_DIST(vb, v2);
	e2=(e*d)/(e*e+1);

	e1=(e1<0)?0:((e1>1)?1:e1);	//clamp 0..1
	e2=(e2<0)?0:((e2>1)?1:e2);	//clamp 0..1
	V3_ADDSCALE(fa, va, -e1*obja->mass, fa);
	V3_ADDSCALE(fb, vb, -e2*objb->mass, fb);
#endif

#if 1
//contact velocity (walking, conveyors, ...)
	da=V3_DOT(obja->u_wvel, normal);
	db=V3_DOT(obja->u_wvel, normal);
	V3_ADDSCALE(obja->u_wvel, normal, -da, v0);
	V3_ADDSCALE(objb->u_wvel, normal, -db, v1);

	d=obja->friction*objb->friction;
	e1=obja->mass/(obja->mass+objb->mass);
	e2=objb->mass/(obja->mass+objb->mass);

	e=V3_NORMALIZE(v0, v2);
	if(e>V3_DOT(va, v2))
	{
		V3_SUB(v0, va, v3);
		V3_ADDSCALE(fa, v3, d*e2*obja->u_wpow, fa);
	}
	if(e>V3_DOT(vb, v2))
	{
		V3_SUB(v0, vb, v3);
		V3_ADDSCALE(fb, v3, -d*e1*obja->u_wpow, fb);
	}

	e=V3_NORMALIZE(v1, v2);
	if(e>V3_DOT(vb, v2))
	{
		V3_SUB(v1, vb, v3);
		V3_ADDSCALE(fb, v3, d*e1*objb->u_wpow, fb);
	}
	if(e>V3_DOT(va, v2))
	{
		V3_SUB(v1, va, v3);
		V3_ADDSCALE(fa, v3, -d*e2*objb->u_wpow, fa);
	}

#if 0
	V3_SUB(v0, va, v2);
	V3_SUB(v1, vb, v3);
	V3_ADDSCALE(fa, v2, d*obja->mass, fa);
	V3_ADDSCALE(fb, v3, d*objb->mass, fb);

	V3_SUB(v0, vb, v2);
	V3_SUB(v1, va, v3);
	V3_ADDSCALE(fa, v3, d*e2*obja->mass, fa);
	V3_ADDSCALE(fb, v2, d*e1*objb->mass, fb);
#endif
#endif

#if 0
//friction+contact

	//velocity to plane
	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);

	//conveyor velocity to plane
	dc=V3_DOT(obja->u_wvel, normal);
	dd=V3_DOT(obja->u_wvel, normal);
	V3_ADDSCALE(obja->u_wvel, normal, -dc, ua);
	V3_ADDSCALE(objb->u_wvel, normal, -dd, ub);

	//friction and mass ratio
	d=obja->friction*objb->friction;
	s=obja->mass/(obja->mass+objb->mass);

	//contact point velocity
	V3_SCALE(va, obja->mass, v0);
	V3_SCALE(vb, objb->mass, v1);
	V3_ADD(v0, v1, v2);
	V3_SCALE(v2, 1.0/(obja->mass+objb->mass), v2);

	//weight speeds
	for(i=0; i<3; i++)
	{
		v3[i]=(v2[i]-ua[i]*(1-s)+ub[i]*s)-vb[i];
		v2[i]=(v2[i]+ua[i]*(1-s)-ub[i]*s)-va[i];
	}

//	V3_ADDSCALE(v2, ua, 1-s, v0);
//	V3_ADDSCALE(v2, ub, s, v1);
//	V3_SUB(v0, va, v2);
//	V3_SUB(v1, vb, v3);

	e=V3_LEN(v2);
	e1=(e*d)/(e*e+1);
	e=V3_LEN(v3);
	e2=(e*d)/(e*e+1);

	V3_ADDSCALE(fa, v2, e1*obja->mass, fa);
	V3_ADDSCALE(fb, v3, e2*objb->mass, fb);
#endif

#if 0
	d=obja->friction*objb->friction;
	d=(d<0)?0:((d>1)?1:d);	//clamp 0..1
	s=obja->mass/(obja->mass+objb->mass);

	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);

	dc=V3_DOT(obja->u_wvel, normal);
	dd=V3_DOT(obja->u_wvel, normal);
	V3_ADDSCALE(obja->u_wvel, normal, -dc, ua);
	V3_ADDSCALE(objb->u_wvel, normal, -dd, ub);

	V3_SUB(ua, va, va);
	V3_SUB(ub, vb, vb);

	V3_ADDSCALE(fa, va, -d*obja->mass, fa);
	V3_ADDSCALE(fb, vb, -d*objb->mass, fb);
#endif
}


void BSDE_ComputeImpulse3(BSDE_Solid *obja, BSDE_Solid *objb,
	bsde_real *org, bsde_real *normal, bsde_real depth, bsde_real dt,
	bsde_real *fa, bsde_real *fb)
{
	bsde_real pa[3], pb[3];
	bsde_real v0[3], v1[3], v2[3], v3[3];
	bsde_real va[3], vb[3], ua[3], ub[3];
	bsde_real da, db, dc, dd;
	bsde_real e1, e2, s, e, d;
	bsde_real f;
	int i;

//	printf("%f\n", obj->mass);

	BSDE_GetPointVelocity(obja, org, pa);
	BSDE_GetPointVelocity(objb, org, pb);

	da=V3_DOT(pa, normal);
	db=V3_DOT(pb, normal);
	s=(da-db)-depth;
	if(s>0)s=0;

	V3_SUB(org, obja->org, v0);
	V3_CROSS(v0, normal, v1);
	BSDE_MatN_MatMult(obja->wimat, 3, 3, v1, 3, 1, v2);
	V3_CROSS(v2, v0, va);

	V3_SUB(org, objb->org, v0);
	V3_CROSS(v0, normal, v1);
	BSDE_MatN_MatMult(objb->wimat, 3, 3, v1, 3, 1, v2);
	V3_CROSS(v2, v0, vb);

	e=obja->elastic*objb->elastic;
//	e=0;
	e=(e<0)?0:((e>1)?1:e);	//clamp 0..1

	d=1;

	e1=-(1+e)*s;
	e2=(1.0/obja->mass)+(1.0/objb->mass)+
		V3_DOT(va, normal)+V3_DOT(vb, normal);
	V3_SCALE(normal, (e1/e2)*d, fa);
	V3_SCALE(normal, (-e1/e2)*d, fb);

#if 0
//original

//	d=0.75;
//	d=0.25;
//	d=0.10;
	d=obja->friction*objb->friction;
	d=(d<0)?0:((d>1)?1:d);	//clamp 0..1

	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);
	V3_ADDSCALE(fa, va, -d*obja->mass, fa);
	V3_ADDSCALE(fb, vb, -d*objb->mass, fb);
#endif

#if 0
	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);

	d=obja->friction*objb->friction;
	e=V3_DIST(va, vb);
	d=(e*d)/(e*e+1);

	d=(d<0)?0:((d>1)?1:d);	//clamp 0..1
	V3_ADDSCALE(fa, va, -d*obja->mass, fa);
	V3_ADDSCALE(fb, vb, -d*objb->mass, fb);
#endif

#if 1
//friction
	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);

	V3_SCALE(va, obja->mass, v0);
	V3_SCALE(vb, objb->mass, v1);
	V3_ADD(v0, v1, v2);
	V3_SCALE(v2, 1.0/(obja->mass+objb->mass), v2);

	d=obja->friction*objb->friction;

	e=V3_DIST(va, v2);
	e1=(e*d)/(e*e+1);
	e=V3_DIST(vb, v2);
	e2=(e*d)/(e*e+1);

	e1=(e1<0)?0:((e1>1)?1:e1);	//clamp 0..1
	e2=(e2<0)?0:((e2>1)?1:e2);	//clamp 0..1
	V3_ADDSCALE(fa, va, -e1*obja->mass, fa);
	V3_ADDSCALE(fb, vb, -e2*objb->mass, fb);
#endif

#if 0
//friction 2
	V3_ADDSCALE(pa, normal, -da, va);
	V3_ADDSCALE(pb, normal, -db, vb);


	V3_SCALE(va, obja->mass, v0);
	V3_SCALE(vb, objb->mass, v1);
	V3_ADD(v0, v1, v2);

	V3_ZERO(v2);

	d=obja->friction*objb->friction;
	e=V3_DIST(v0, v1);
	f=d/(e*e+1);

	V3_ADDSCALE(v2, va, -obja->mass, v3);
	V3_ADDSCALE(fa, v3, f, fa);

	V3_ADDSCALE(v2, vb, -objb->mass, v3);
	V3_ADDSCALE(fb, v3, f, fb);
#endif
}

void BSDE_HandleContact(BSDE_World *world, BSDE_Contact *self, bsde_real dt)
{
//	bsde_real v0[3], v1[3], v2[3];
	bsde_real pa[3], pb[3];
	bsde_real fa[3], fb[3];
	bsde_real s, t, e;
	bsde_real sa, sb, ca, cb, da, db;
	bsde_real va[3], vb[3];
	int i, fla, flo;

#if 1
	fla=self->obja->moveflags&self->objb->moveflags;
	flo=self->obja->moveflags|self->objb->moveflags;

	if(fla&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC|
		BSDE_MOVEFL_NONSOLID))
			return;
	i=BSDE_MOVEFL_STATIC|BSDE_MOVEFL_SEMISTATIC;
	if((self->obja->moveflags&i) &&
		(self->objb->moveflags&i))
			return;

	if(flo&BSDE_MOVEFL_FLUID)
	{
		BSDE_HandleFluidContact(world, self, dt);
		return;
	}
#endif

//	printf("contact %04X %04X (%f %f %f) (%f %f %f) %f\n",
//		self->obja->solidtype, self->objb->solidtype,
//		self->org[0], self->org[1], self->org[2],
//		self->norm[0], self->norm[1], self->norm[2], self->depth);

	if((self->npts>1) &&
		!(self->obja->moveflags&BSDE_MOVEFL_NONSOLID) &&
		!(self->objb->moveflags&BSDE_MOVEFL_NONSOLID))
	{
		for(i=0; i<self->npts; i++)
		{
			BSDE_ComputeImpulse3(self->obja, self->objb,
				self->pts+i*4, self->norm, self->depth,
				dt, fa, fb);

//			BSDE_ComputeImpulse3(self->obja, self->objb,
//				self->pts+i*4, self->norm, self->pts[i*4+3],
//				dt, fa, fb);

			V3_SCALE(fa, 1.0/self->npts, fa);
			V3_SCALE(fb, 1.0/self->npts, fb);

			BSDE_ApplyPointForce(self->obja, self->pts+i*4, fa);
			BSDE_ApplyPointForce(self->objb, self->pts+i*4, fb);
		}

		return;
	}

	BSDE_ComputeImpulse2(self->obja, self->objb,
		self->org, self->norm, self->depth, dt, fa, fb);

//	BSDE_ComputeImpulse3(self->obja, self->objb,
//		self->org, self->norm, self->depth, dt, fa, fb);

	if(!(self->objb->moveflags&BSDE_MOVEFL_NONSOLID))
		BSDE_ApplyPointForce(self->obja, self->org, fa);
	if(!(self->obja->moveflags&BSDE_MOVEFL_NONSOLID))
		BSDE_ApplyPointForce(self->objb, self->org, fb);
}
