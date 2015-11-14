#include "bsde_internal.h"

static BSDE_World *bsde_worlds[16];
// static int bsde_world=0;

BSDE_World *BSDE_GetWorld()
{
	BSDE_ThreadState *ctx;
	ctx=BSDE_GetThreadState();
	return(bsde_worlds[ctx->world]);
}

void BSDE_SetWorld(int id)
{
	BSDE_ThreadState *ctx;
	ctx=BSDE_GetThreadState();
	ctx->world=id;
}

int BSDE_CreateWorld()
{
	int i;

	for(i=1; i<16; i++)
		if(!bsde_worlds[i])
	{
		bsde_worlds[i]=BSDE_NewWorld();
		BSDE_SetWorld(i);

//		bsdeSRand2(123456789, 987654321);
//		bsdeSRand2((rand()<<16)|rand(), (rand()<<16)|rand());

		BSDE_SRand2(bsde_worlds[i],
			(rand()<<16)|rand(), (rand()<<16)|rand());
		BSDE_Seed21(bsde_worlds[i]);

		return(i);
	}
	return(-1);
}

int BSDE_EmptyWorld(BSDE_World *world)
{
	int i;

	for(i=0; i<world->n_solids; i++)
		if(world->solids[i])
	{
		BSDE_Destroy(world->solids[i]);
		world->solids[i]=NULL;
	}

	return(0);
}

BSDE_Solid *BSDE_GetWorldSolid(int obj)
{
	BSDE_World *wrl;
	BSDE_Solid *solid;

	wrl=BSDE_GetWorld();
	if(!wrl)return(NULL);

	if(!wrl->solids)return(NULL);
	if(obj<=0)return(NULL);
	if(obj>=wrl->n_solids)return(NULL);

	solid=wrl->solids[obj];

	return(solid);
}

void BSDE_SetWorldSolid(int obj, BSDE_Solid *solid)
{
	BSDE_World *wrl;

	wrl=BSDE_GetWorld();
	if(!wrl)return;

	if(!wrl->solids)return;
	if(obj<=0)return;
	if(obj>=wrl->n_solids)return;

	wrl->solids[obj]=solid;
}


void BSDE_StepWorld(BSDE_World *world, bsde_real dt)
{
	int i;

	world->realtime+=dt;

	dt+=world->steptime;
	if(dt>world->maxframe)
	{
		world->steptime=dt-world->maxframe;
		dt=world->maxframe;
	}

	//forget about backlog
	i=0;
	while(world->steptime>world->maxframe)
	{
		world->steptime-=world->maxframe;
		i++;
	}
//	if(i)printf("dropped %d frames\n", i);

//	dt+=world->steptime;
//	if(dt>world->maxframe)dt=world->maxframe;

	while(dt>=world->maxstep)
	{
		BSDE_WorldForces(world, world->maxstep);
//		BSDE_SetupStack(world);
//		BSDE_PushInitial(world);
//		BSDE_RecurseWorld(world, world->maxstep, 0);

//		BSDE_WorldMultiStep(world, world->maxstep);
		BSDE_WorldSingleStep2(world, world->maxstep);

		dt-=world->maxstep;
		world->worldtime+=world->maxstep;
	}

	//accumulate remaining partial step
	world->steptime+=dt;
}



void BSDE_WorldForces(BSDE_World *world, bsde_real dt)
{
	bsde_real dv[3];
	BSDE_Solid *cur;
	int i;

	for(i=0; i<world->n_solids; i++)
	{
		if(!world->solids[i])continue;
		cur=world->solids[i];
		if(cur->moveflags&BSDE_MOVEFL_HIDDEN)continue;

		if(cur->moveflags&BSDE_MOVEFL_BLOCKED)
		{
//			cur->vel[0]+=BSDE_RandG()*0.1;
//			cur->vel[1]+=BSDE_RandG()*0.1;
//			cur->vel[2]+=BSDE_RandG()*0.1;
			continue;
		}

		if(cur->moveflags&BSDE_MOVEFL_IDLE)continue;

		if(!(cur->moveflags&BSDE_MOVEFL_NOGRAVITY))
		{
			V3_ADDSCALE(cur->vel, world->gravity,
				dt, cur->vel);
		}
		V3_ADDSCALE(cur->vel, cur->u_acc, dt, cur->vel);
		BSDE_Plane_TransformVector(cur->u_racc, cur->xform, dv);
		V3_ADDSCALE(cur->vel, dv, dt, cur->vel);

		if(cur->moveflags&BSDE_MOVEFL_WANTORG)
		{
			V3_SUB(cur->u_wantorg, cur->org, dv);
			V3_ADDSCALE(cur->vel, dv, dt, cur->vel);
		}

		if(cur->moveflags&BSDE_MOVEFL_WANTVEL)
		{
			V3_SUB(cur->u_wantvel, cur->vel, dv);
//			V3_ADDSCALE(cur->vel, dv, dt, cur->vel);

			dv[2]=cur->u_wantvel[2];

//			V3_ADDSCALE(cur->vel, dv, dt*10, cur->vel);
			V3_ADD(cur->vel, dv, cur->vel);
		}

		if(cur->moveflags&BSDE_MOVEFL_WANTROT)
		{
//			V3_SUB(cur->u_wantrot, cur->rot, dv);
//			V3_ADDSCALE(cur->avel, dv, dt, cur->avel);
		}
	}
}


int bsde_finite(bsde_real val)
{
#ifdef BSDE_REAL64
	u64 l;

	l=*(u64 *)(&val);
	if((l&0x7FF0000000000000LL)==0x7FF0000000000000LL)
		return(0);
	return(1);
#endif
#ifdef BSDE_REAL32
	u32 l;

	l=*(u32 *)(&val);
	if((l&0x7F000000LL)==0x7F000000LL)
		return(0);
	return(1);
#endif
	return(1);
}

void BSDE_UpdateVel(BSDE_World *world, BSDE_Solid *cur, bsde_real dt)
{
	bsde_real f;

#if 1
	if(!bsde_finite(V3_DOT(cur->ivel, cur->ivel)))
		{ V3_ZERO(cur->ivel); }
	if(!bsde_finite(V3_DOT(cur->itvel, cur->itvel)))
		{ V3_ZERO(cur->itvel); }
#endif

#if 0
	if(V3_LEN(cur->ivel)>10)
	{
		V3_SCALE(cur->ivel, 10/V3_LEN(cur->ivel), cur->ivel);
	}
	if((V3_LEN(cur->itvel)/cur->mass)>10)
	{
		V3_SCALE(cur->itvel, 10/(V3_LEN(cur->itvel)/cur->mass),
			cur->itvel);
	}
#endif


	V3_ADD(cur->vel, cur->ivel, cur->vel);
	V3_ADD(cur->tvel, cur->itvel, cur->tvel);
	V3_ZERO(cur->ivel);
	V3_ZERO(cur->itvel);

	if(V3_LEN(cur->vel)<0.01) { V3_ZERO(cur->vel); }
	if((V3_LEN(cur->tvel)/cur->mass)<0.01) { V3_ZERO(cur->tvel); }

#if 1
	if(!bsde_finite(V3_DOT(cur->vel, cur->vel)))
		{ V3_ZERO(cur->vel); }
	if(!bsde_finite(V3_DOT(cur->tvel, cur->tvel)))
		{ V3_ZERO(cur->tvel); }
#endif

#if 0
	if(V3_LEN(cur->vel)>100)
	{
		V3_SCALE(cur->vel, 100/V3_LEN(cur->vel), cur->vel);
	}

	if((V3_LEN(cur->tvel)/cur->mass)>10)
	{
		V3_SCALE(cur->tvel, 10/(V3_LEN(cur->vel)/cur->mass),
			cur->tvel);
	}
#endif

	if((cur->solidtype==BSDE_SOLID_AABB) ||
		(cur->moveflags&BSDE_MOVEFL_NOSPIN))
	{
		BSDE_Quat_Identity(cur->rot);
		V3_ZERO(cur->tvel);
	}

	if(cur->moveflags&BSDE_MOVEFL_NOMOVE)
	{
		V3_ZERO(cur->vel);
	}


//	f=V3_DIST(cur->e_org, cur->org)+V4_DIST(cur->e_rot, cur->rot);
	f=V3_LEN(cur->vel)+V3_LEN(cur->avel);
	if(f>=(0.25*dt))
	{
		cur->idletime=0;
		cur->moveflags&=~BSDE_MOVEFL_IDLE;
	}
}

void BSDE_Update(BSDE_World *world, BSDE_Solid *cur, bsde_real dt)
{
	bsde_real f;
	int i;

	f=V3_DIST(cur->e_org, cur->org)+V4_DIST(cur->e_rot, cur->rot);

//	if(f<(0.10*dt))
	if(f<(0.25*dt))
	{
		cur->idletime+=dt;
		if(cur->idletime>=1)
			cur->moveflags|=BSDE_MOVEFL_IDLE;
	}else
	{
		cur->idletime=0;
//		cur->moveflags&=~BSDE_MOVEFL_IDLE;
		cur->moveflags&=~BSDE_MOVEFL_IDLE;
		cur->stateflags&=~BSDE_STATEFL_MOVECACHE;
	}

	V3_COPY(cur->e_org, cur->org);
	V4_COPY(cur->e_rot, cur->rot);

	if(!bsde_finite(V3_DOT(cur->org, cur->org)))
		{ V3_ZERO(cur->org); }
	if(!bsde_finite(V4_DOT(cur->rot, cur->rot)))
		{ V4_ZERO(cur->rot); cur->rot[3]=1; }

	if(cur->moveflags&BSDE_MOVEFL_IDLE)
	{
		V3_ZERO(cur->tvel);
		V3_ZERO(cur->vel);
//		V3_ZERO(cur->avel);
	}
}

void BSDE_Predict(BSDE_World *world, BSDE_Solid *cur, bsde_real dt)
{
	bsde_real qa[4], qb[4], n[3], l;
	bsde_real ma[9], mb[9], mc[9];
	int i;

	if(cur->stateflags&BSDE_STATEFL_MOVECACHE)
		return;

//	if(cur->moveflags&BSDE_MOVEFL_IDLE)
//		return;

	//predict origin and rotation

	V3_ADDSCALE(cur->org, cur->vel, dt, cur->e_org);


	l=V3_NORMALIZE(cur->avel, n);

//	printf("spin (%f %f %f) %f\n", n[0], n[1], n[2], l);

	BSDE_Quat_FromAxis(n, l*dt, qa);
	BSDE_Quat_Multiply(qa, cur->rot, qb);
//	BSDE_Quat_Multiply(cur->rot, qa, qb);
	BSDE_Quat_Normalize(qb, cur->e_rot);

	V3_COPY(cur->vel, cur->e_vel);
	V3_COPY(cur->tvel, cur->e_tvel);


	//calculate current inertia tensor

	BSDE_WorldInertiaTensor(cur);

	//calculate current transform
	BSDE_Quat_ToMatrix(cur->rot, cur->xform);
	for(i=0; i<3; i++)cur->xform[3*4+i]+=cur->org[i];

	//calculate predicted transform
	BSDE_Quat_ToMatrix(cur->e_rot, cur->e_xform);
	for(i=0; i<3; i++)cur->e_xform[3*4+i]+=cur->e_org[i];


	//calculate move volume
	BSDE_BoxMove(cur, cur->mmins, cur->mmaxs);

	//clear prediction flag (used for expensive internal predictions)
	cur->moveflags&=~BSDE_MOVEFL_PREDICT;

	if(cur->moveflags&(BSDE_MOVEFL_STATIC|BSDE_MOVEFL_IDLE))
		cur->stateflags|=BSDE_STATEFL_MOVECACHE;
}

void BSDE_Awaken(BSDE_Solid *cur)
{
	cur->idletime=0;
	cur->moveflags&=~BSDE_MOVEFL_IDLE;
	cur->stateflags&=~BSDE_STATEFL_MOVECACHE;
}


void BSDE_PredictPosTime(BSDE_Solid *cur, bsde_real dt,
	bsde_real *org, bsde_real *rot, bsde_real *xform)
{
	bsde_real qa[4], qb[4], n[3], l;
	int i;

	if(dt>0)
	{
		V3_ADDSCALE(cur->org, cur->vel, dt, org);

		l=V3_NORMALIZE(cur->avel, n);
		BSDE_Quat_FromAxis(n, l*dt, qa);
		BSDE_Quat_Multiply(qa, cur->rot, qb);
		BSDE_Quat_Normalize(qb, rot);
	}else
	{
		V3_COPY(cur->org, org);
		V3_COPY(cur->rot, rot);
	}

	BSDE_Quat_ToMatrix(rot, xform);
	for(i=0; i<3; i++)xform[3*4+i]+=org[i];
}

void BSDE_SetWorldGravity(BSDE_World *world, bsde_real x, bsde_real y, bsde_real z)
{
	world->gravity[0]=x*world->sc_len;
	world->gravity[1]=y*world->sc_len;
	world->gravity[2]=z*world->sc_len;
}

void BSDE_GetWorldGravity(BSDE_World *world, bsde_real *v)
{
	V3_SCALE(world->gravity, 1.0/world->sc_len, v);
}

bsde_real BSDE_GetWorldAttrF(BSDE_World *world, int var)
{
	bsde_real f;

	f=0;
	switch(var)
	{
	case BSDE_MINSTEP:
		f=world->minstep;
		break;
	case BSDE_MAXSTEP:
		f=world->maxstep;
		break;
	case BSDE_MAXFRAME:
		f=world->maxframe;
		break;
	case BSDE_REALTIME:
		f=world->realtime;
		break;
	case BSDE_WORLDTIME:
		f=world->worldtime;
		break;
	default:
		break;
	}
	return(f);
}

void BSDE_SetWorldAttrF(BSDE_World *world, int var, bsde_real val)
{
	switch(var)
	{
	case BSDE_MINSTEP:
		world->minstep=val;
		break;
	case BSDE_MAXSTEP:
		world->maxstep=val;
		break;
	case BSDE_MAXFRAME:
		world->maxframe=val;
		break;

	case BSDE_UNIT_LENGTH:
		world->sc_len=val; break;
	case BSDE_UNIT_VOLUME:
		world->sc_vol=val; break;
	case BSDE_UNIT_MASS:
		world->sc_mass=val; break;
	case BSDE_UNIT_FORCE:
		world->sc_force=val; break;

	default:
		break;
	}
}

int BSDE_GetWorldAttrI(BSDE_World *world, int var)
{
	int i;

	i=0;
	switch(var)
	{
	case BSDE_COLLIDE_STRICT:
		i=(world->flags&BSDE_WORLDFL_STRICT)!=0;
		break;
	case BSDE_FRAME_PASSES:
		i=world->passes;
		break;
	default:
		break;
	}
	return(i);
}

void BSDE_SetWorldAttrI(BSDE_World *world, int var, int val)
{
	switch(var)
	{
	case BSDE_COLLIDE_STRICT:
		if(val)world->flags|=BSDE_WORLDFL_STRICT;
			else world->flags&=~BSDE_WORLDFL_STRICT;
		break;
	case BSDE_FRAME_PASSES:
		world->passes=val;
		break;

	case BSDE_UNIT_LENGTH:
		switch(val)
		{
		case BSDE_UNIT_METER: world->sc_len=1.0; break;
		case BSDE_UNIT_CENTIMETER: world->sc_len=0.01; break;
		case BSDE_UNIT_KILOMETER: world->sc_len=1000.0; break;
		case BSDE_UNIT_FOOT: world->sc_len=0.305; break;
		case BSDE_UNIT_INCH: world->sc_len=0.0254; break;
		case BSDE_UNIT_YARD: world->sc_len=0.915; break;
		case BSDE_UNIT_MILE: world->sc_len=1610.0; break;
		case BSDE_UNIT_INCH_F8: world->sc_len=0.0254/8; break;
		case BSDE_UNIT_INCH_F10: world->sc_len=0.0254/10; break;
		case BSDE_UNIT_INCH_F16: world->sc_len=0.0254/16; break;
		default: break;
		}
		break;
	case BSDE_UNIT_VOLUME:
		switch(val)
		{
		case BSDE_UNIT_METER: world->sc_vol=1.0; break;
		case BSDE_UNIT_CENTIMETER: world->sc_vol=0.000001; break;
		case BSDE_UNIT_KILOMETER: world->sc_vol=1000000000.0; break;
		case BSDE_UNIT_FOOT: world->sc_vol=0.028372625; break;
		case BSDE_UNIT_INCH: world->sc_vol=0.000016387064; break;
		case BSDE_UNIT_YARD: world->sc_vol=0.766060875; break;
		case BSDE_UNIT_MILE: world->sc_vol=4173281000.0; break;
		case BSDE_UNIT_GALLON: world->sc_vol=0.00379; break;
		case BSDE_UNIT_LITER: world->sc_vol=0.001; break;
		default: break;
		}
		break;
	case BSDE_UNIT_MASS:
		switch(val)
		{
		case BSDE_UNIT_POUND: world->sc_mass=0.0004525; break;
		case BSDE_UNIT_POUND_M: world->sc_mass=0.0005; break;
		case BSDE_UNIT_GRAM: world->sc_mass=0.000001; break;
		case BSDE_UNIT_KILOGRAM: world->sc_mass=0.001; break;
		case BSDE_UNIT_MEGAGRAM: world->sc_mass=1.0; break;
		case BSDE_UNIT_MILLIGRAM: world->sc_mass=0.000000001; break;
		default: break;
		}
		break;
	case BSDE_UNIT_FORCE:
		switch(val)
		{
		case BSDE_UNIT_POUND: world->sc_force=4.45; break;
		case BSDE_UNIT_NEWTON: world->sc_force=1.0; break;
		default: break;
		}
		break;

	default:
		break;
	}
}

void BSDE_GetWorldAttrFV(BSDE_World *world, int var, float *val, int n)
{
	switch(var)
	{
	case BSDE_GRAVITY:
		val[0]=world->gravity[0]/world->sc_len;
		val[1]=world->gravity[1]/world->sc_len;
		val[2]=world->gravity[2]/world->sc_len;
		break;
	default:
		break;
	}
}

void BSDE_SetWorldAttrFV(BSDE_World *world, int var, float *val, int n)
{
	switch(var)
	{
	case BSDE_GRAVITY:
		world->gravity[0]=val[0]*world->sc_len;
		world->gravity[1]=val[1]*world->sc_len;
		world->gravity[2]=val[2]*world->sc_len;
		break;
	default:
		break;
	}
}

void BSDE_GetWorldAttrDV(BSDE_World *world, int var, double *val, int n)
{
	switch(var)
	{
	case BSDE_GRAVITY:
		val[0]=world->gravity[0]/world->sc_len;
		val[1]=world->gravity[1]/world->sc_len;
		val[2]=world->gravity[2]/world->sc_len;
		break;
	default:
		break;
	}
}

void BSDE_SetWorldAttrDV(BSDE_World *world, int var, double *val, int n)
{
	switch(var)
	{
	case BSDE_GRAVITY:
		world->gravity[0]=val[0]*world->sc_len;
		world->gravity[1]=val[1]*world->sc_len;
		world->gravity[2]=val[2]*world->sc_len;
		break;
	default:
		break;
	}
}

void *BSDE_GetBuffer(BSDE_World *world, int var)
{
	return(NULL);
}

void BSDE_SetBuffer(BSDE_World *world,
	int var, int type, int num, int stride, void *val)
{
	switch(var)
	{
	case BSDE_CONTACT_SELF:
		world->t_arr_hs=val;
		world->t_str_hs=stride;
		world->t_ty_hs=type;
		world->t_num=num;
		break;
	case BSDE_CONTACT_OTHER:
		world->t_arr_ho=val;
		world->t_str_ho=stride;
		world->t_ty_ho=type;
		world->t_num=num;
		break;
	case BSDE_CONTACT_ORIGIN:
		world->t_arr_o=val;
		world->t_str_o=stride;
		world->t_ty_o=type;
		world->t_num=num;
		break;
	case BSDE_CONTACT_NORMAL:
		world->t_arr_n=val;
		world->t_str_n=stride;
		world->t_ty_n=type;
		world->t_num=num;
		break;
	default:
		break;
	}
}

int BSDE_AddSolidWorld(BSDE_World *world, BSDE_Solid *solid)
{
	BSDE_Solid **lst;
	int i, j;

	if(!world->solids)
	{
		world->solids=(BSDE_Solid **)
			malloc(4096*sizeof(BSDE_Solid *));
		memset(world->solids, 0, 4096*sizeof(BSDE_Solid *));
		world->n_solids=4096;
	}

	for(i=1; i<world->n_solids; i++)
		if(world->solids[i]==solid)
			return(i);
	for(i=1; i<world->n_solids; i++)
		if(!world->solids[i])
	{
		world->solids[i]=solid;
		solid->handle=i;
		solid->world=world;
		return(i);
	}

//	i=world->n_solids+256;
	i=world->n_solids+(world->n_solids>>1);
	world->solids=(BSDE_Solid **)realloc(
		world->solids, i*sizeof(BSDE_Solid *));

	for(j=world->n_solids; j<i; j++)
		world->solids[j]=0;
	world->n_solids=i;

//	lst=(BSDE_Solid **)malloc(i*sizeof(BSDE_Solid *));
//	memset(lst, 0, i*sizeof(BSDE_Solid *));

//	if(world->n_solids)
//	{
//		memcpy(lst, world->solids,
//			world->n_solids*sizeof(BSDE_Solid *));
//		free(world->solids);
//	}
//	world->solids=lst;
//	world->n_solids=i;

	i=BSDE_AddSolidWorld(world, solid);
	return(i);
}

int BSDE_AddJointWorld(BSDE_World *world, BSDE_Joint *joint)
{
	BSDE_Joint **lst;
	int i;

	for(i=1; i<world->n_joints; i++)
		if(world->joints[i]==joint)
			return(i);
	for(i=1; i<world->n_joints; i++)
		if(!world->joints[i])
	{
		world->joints[i]=joint;
		return(i);
	}

	i=world->n_joints+256;
	lst=(BSDE_Joint **)malloc(i*sizeof(BSDE_Joint *));
	memset(lst, 0, i*sizeof(BSDE_Joint *));

	if(world->n_joints)
	{
		memcpy(lst, world->joints,
			world->n_joints*sizeof(BSDE_Joint *));
		free(world->joints);
	}
	world->joints=lst;
	world->n_joints=i;

	i=BSDE_AddJointWorld(world, joint);
	return(i);
}

BSDE_World *BSDE_NewWorld()
{
	BSDE_World *tmp;

	tmp=(BSDE_World *)malloc(sizeof(BSDE_World));
	memset(tmp, 0, sizeof(BSDE_World));

	tmp->maxstep=0.01;
	tmp->minstep=0.0001;
	tmp->maxframe=0.5;

	tmp->sc_len=1;
	tmp->sc_vol=1;
	tmp->sc_mass=1;
	tmp->sc_force=1;

	tmp->contact=(BSDE_Contact **)
		malloc(BSDE_MAX_CONTACTS*sizeof(BSDE_Contact *));

	return(tmp);
}


int *BSDE_GetTriggerArray(BSDE_World *world)
	{return(world->tr_arr);}
int BSDE_GetTriggerArrayPos(BSDE_World *world)
	{return(world->tr_cur);}
void BSDE_ResetTriggerArray(BSDE_World *world)
	{world->tr_cur=0;}

void BSDE_SetTriggerArray(BSDE_World *world, int *arr, int num)
{
	world->tr_arr=arr;
	world->tr_num=num;
	world->tr_cur=0;
}

void BSDE_WorldAddTriggerContact(BSDE_World *world,
	BSDE_Solid *obja, BSDE_Solid *objb)
{
	int ia, ib;
	int i;

	if(!world->tr_arr)return;	//no trigger array

	for(i=1; i<world->n_solids; i++)
		if(world->solids[i]==obja)
			break;
	if(i>=world->n_solids)return;	//obja is not in the world
	ia=i;

	for(i=1; i<world->n_solids; i++)
		if(world->solids[i]==objb)
			break;
	if(i>=world->n_solids)return;	//objb is not in the world
	ib=i;


	for(i=0; i<world->tr_cur; i++)
		if((world->tr_arr[i*2+0]==ia) &&
			(world->tr_arr[i*2+1]==ib))
				break;
	if(i<world->tr_cur)return;	//nothing needs to be done

	world->tr_arr[i*2+0]=ia;
	world->tr_arr[i*2+1]=ib;
	world->tr_cur++;
}
