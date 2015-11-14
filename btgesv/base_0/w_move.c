#include <btgesv.h>

BTGE_BrushWorld *btge_world;

float btge_time;		//world time
float btge_steptime;	//world step time
float btge_framedt;		//frame delta time
float btge_voxsteptime;	//voxel step time
// btEntity btge_client;	//player

bool BT_CheckEntityTeleportSpot(btEntity self, vec3 org)
{
	int i;

	i=btPointBoxContents(org, self->mins, self->maxs, self);
	if(i&BT_CONTENTS_MONSTERSOLID)
		return(false);
	return(true);
}

vec3 BT_FindEntityTeleportSpot(btEntity self, vec3 org)
{
	vec3 org1, org2, dir;
	int i;

	//see if target is good as-is
	if(BT_CheckEntityTeleportSpot(self, org))
		return(org);

	//try to move upwards
	org1=v3sub(org, vec3(0,0,v3z(self->mins)));
	if(BT_CheckEntityTeleportSpot(self, org1))
		return(org1);
	
	//try to move downwards
	org1=v3sub(org, vec3(0,0,v3z(self->maxs)));
	if(BT_CheckEntityTeleportSpot(self, org1))
		return(org1);

	//look for a spot
	for(i=0; i<16; i++)
	{
		dir=vec3(
			(i&1)?v3x(self->maxs):v3x(self->mins),
			(i&2)?v3y(self->maxs):v3y(self->mins),
			(i&8)?((i&4)?v3z(self->maxs):v3z(self->mins)):0);
		org1=v3sub(org, dir);
		if(BT_CheckEntityTeleportSpot(self, org1))
			return(org1);
	}
	
	return(self->origin);
}

void BT_StepEntity_AddGravity(btEntity self)
{
	vec3 gv;
	float f;
	int i;

//	if(self->flags&BT_FL_CLIENT)
//		self->flags&=~BT_FL_ONGROUND;
//	if(self->flags&BT_FL_ONGROUND)
//		return;

	if(self->flags&BT_FL_ONGROUND)
	{
		if(!(self->flags&BT_FL_CLIENT))
			return;
	}

	if(self->flags&BT_FL_NOCLIP)
		return;
		
	i=btPointContents(self->origin, self);

//	if(i&BT_CONTENTS_PLAYERSOLID)
//		return;

//	gv=vec3(0,0,-800);
//	f=v3len(self->velocity);
//	f=(f*f)/(1000*1000);
//	f=(f<0)?0:((f>2)?2:f);
//	gv=v3scale(gv, 1.0-f);

	if(i&BT_CONTENTS_FLUID)
	{
		gv=vec3(0,0,-800*0.10);
		f=v3len(self->velocity);
		f=(f*f)/(200*200);
		f=(f<0)?0:((f>2)?2:f);
		gv=v3scale(gv, 1.0-f);

		if(self->flags&BT_FL_SWIM)return;
		if(self->flags&BT_FL_FLY)return;
//		self->velocity=v3addscale(self->velocity,
//			gv, btge_framedt*0.25);
		self->velocity=v3addscale(self->velocity,
			gv, btge_framedt);
	}else
	{
		gv=vec3(0,0,-800);
		f=v3len(self->velocity);
		f=(f*f)/(1000*1000);
		f=(f<0)?0:((f>2)?2:f);
		gv=v3scale(gv, 1.0-f);

		if(self->flags&BT_FL_FLY)return;
		self->velocity=v3addscale(self->velocity,
			gv, btge_framedt);
	}
}

void BT_StepEntity_UpdateAngles(btEntity self)
{
	self->angles=v3addscale(self->angles,
		self->angvel, btge_framedt);
	self->angles2=v3addscale(self->angles2,
		self->angvel2, btge_framedt);
		
	self->angles=btAngleMod3(self->angles);
	self->angles2=btAngleMod3(self->angles2);
}

void BT_StepEntity_Walk(btEntity self)
{
	BT_StepEntity_AddGravity(self);
	BT_CheckFireBoxTrigger(self);
}

void BT_StepEntity_Step(btEntity self)
{
	vec3 org1;
	int i;

	if(v3len(self->velocity)>1000)
	{
		printf("BT_StepEntity_Step: %d Fast Moving Org=(%s) Vel=(%s)\n",
			self->entnum, btVec3ToStr(self->origin),
				btVec3ToStr(self->velocity));
	}

	if(!(self->flags&BT_FL_ONGROUND) &&
		!(self->flags&(BT_FL_SWIM|BT_FL_FLY)))
	{
		org1=v3addscale(self->origin, self->velocity, btge_framedt);

		i=btPointBoxContents(org1, self->mins, self->maxs, self);
		if(i&BT_CONTENTS_MONSTERSOLID)
		{
			self->velocity=vec3(0,0,0);
			self->flags|=BT_FL_ONGROUND;
			return;
		}
		self->origin=org1;
		self->absmin=v3add(self->origin, self->mins);
		self->absmax=v3add(self->origin, self->maxs);
	}

	BT_StepEntity_AddGravity(self);
	BT_CheckFireBoxTrigger(self);
}

void BT_StepEntity_Fly(btEntity self)
{
	vec3 org1;
	int i;

	BT_StepEntity_UpdateAngles(self);

	org1=v3addscale(self->origin, self->velocity, btge_framedt);

	i=btPointBoxContents(org1, self->mins, self->maxs, self);
	if(i&BT_CONTENTS_MONSTERSOLID)
	{
		self->velocity=vec3(0,0,0);
		return;
	}
	self->origin=org1;
	BT_CheckFireBoxTrigger(self);
}

void BT_StepEntity_Toss(btEntity self)
{
	vec3 org1;
	int i;

	if(self->flags&BT_FL_ONGROUND)
		return;

	if(btge_framedt>=0.25)
	{
		self->flags|=BT_FL_ONGROUND;
		self->velocity=vec3(0,0,0);
		return;
	}

	BT_StepEntity_AddGravity(self);
	BT_StepEntity_UpdateAngles(self);

	org1=v3addscale(self->origin, self->velocity, btge_framedt);

	i=btPointBoxContents(org1, self->mins, self->maxs, self);
	if(i&BT_CONTENTS_ITEMSOLID)
	{
		self->flags|=BT_FL_ONGROUND;
		self->velocity=vec3(0,0,0);
		return;
	}
	self->origin=org1;

//	BT_CheckFireBoxTrigger(self);
}

void BT_PushEntity(btEntity self, btEntity ent, vec3 vel)
{
	vec3 org0, org1;
	btEntity cur;
	int i;
	
	cur=ent;
//	cur->origin=btBoxStepMove(cur->origin, cur->mins, cur->maxs,
//		vel, btge_framedt, cur);

//	cur->velocity=v3add(cur->velocity, vel);

	if(cur->solidtype==BT_SOLID_TRIGGER)
	{
		cur->flags&=~BT_FL_ONGROUND;
		
		//ignore non-moving and pushmove objects
		if(cur->movetype==BT_MOVE_NONE)return;
		if(cur->movetype==BT_MOVE_PUSH)return;
	}


	for(i=10; i<25; i++)
	{
		org1=v3add(cur->origin,
			v3scale(vel, btge_framedt*(i*0.1)));
		if(BT_CheckEntityTeleportSpot(cur, org1))
		{
			cur->origin=org1;
			break;
		}
	}

	cur->flags&=~BT_FL_ONGROUND;

	cur->absmin=v3add(cur->origin, cur->mins);
	cur->absmax=v3add(cur->origin, cur->maxs);
}

void BT_StepEntity_Push(btEntity self)
{
	vec3 org0, org1, amins, amaxs;
	btEntity lst, cur, nxt, blst;

	if(v3len(self->velocity)<0.001)
	{
		//don't bother moving if stationary
		BT_StepEntity_UpdateAngles(self);
		BT_CheckFireBoxTrigger(self);
		return;
	}

	//clear onground for objects directly above
	amins=v3sub(self->absmin, vec3(1,1,1));
	amaxs=v3add(self->absmax, vec3(1,1,64));
	lst=BT_CheckBoxEnts(amins, amaxs,
		BT_TRACE_MONSTERS|BT_TRACE_TRIGGER, self);

	cur=lst; blst=NULL;
	while(cur)
	{
		cur->flags&=~BT_FL_ONGROUND;
		cur=cur->chain;
	}

	//step forward
	org0=self->origin;
	self->origin=v3addscale(self->origin, self->velocity, btge_framedt);
	self->absmin=v3add(self->origin, self->mins);
	self->absmax=v3add(self->origin, self->maxs);

	//check if pushing anything
//	lst=BT_CheckBoxEnts(self->absmin, self->absmax,
//		BT_TRACE_MONSTERS|BT_TRACE_TRIGGER, self);

	//check if pushing anything
	//box expanded so that items above are found
	amins=v3sub(self->absmin, vec3(1,1,1));
	amaxs=v3add(self->absmax, vec3(1,1,12));
	lst=BT_CheckBoxEnts(amins, amaxs,
		BT_TRACE_MONSTERS|BT_TRACE_TRIGGER, self);

	cur=lst; blst=NULL;
	while(cur)
	{
		//skip if it doesn't hit current bmodel
//		if(!btEntityCheckBox(self, cur->absmin, cur->absmax))
//			{ cur=cur->chain; continue; }

		//ignore noclip entities
		if(cur->flags&BT_FL_NOCLIP)
			{ cur=cur->chain; continue; }

		//expand box some
		amins=v3sub(cur->absmin, vec3(1,1,12));
		amaxs=v3add(cur->absmax, vec3(1,1,1));
		if(!btEntityCheckBox(self, amins, amaxs))
			{ cur=cur->chain; continue; }

//		if(cur->solidtype!=BT_SOLID_SLIDEBOX)
//			{ cur=cur->chain; continue; }
//		if((cur->movetype!=BT_MOVE_WALK) &&
//				(cur->movetype!=BT_MOVE_STEP))
//			{ cur=cur->chain; continue; }

		//try to push other
		nxt=cur->chain;
//		cur->velocity=v3add(cur->velocity, self->velocity);
//		cur->origin=btBoxStepMove(cur->origin, cur->mins, cur->maxs,
//			self->velocity, btge_framedt*1.1, cur);

#if 0
		org1=v3add(cur->origin,
			v3scale(self->velocity, btge_framedt*1.1));
		if(BT_CheckEntityTeleportSpot(cur, org1))
			cur->origin=org1;
		
		cur->absmin=v3add(cur->origin, cur->mins);
		cur->absmax=v3add(cur->origin, cur->maxs);
#endif

		BT_PushEntity(self, cur, self->velocity);

		//ignore further checks with triggers
		if(cur->solidtype==BT_SOLID_TRIGGER)
			{ cur=nxt; continue; }

		//still collide? add to blocked-list
		if(btEntityCheckBox(self, cur->absmin, cur->absmax))
			{ cur->chain=blst; blst=cur; }

		cur=nxt;
	}

	if(blst)
	{
		//allow CalcMove to do its thing
		BT_CalcMoveBlocked(self);

		cur=blst;
		while(cur)
		{
			nxt=cur->chain;
			btEntBlocked(self, cur);
			cur=nxt;
		}
		
		self->origin=org0;
		self->absmin=v3add(self->origin, self->mins);
		self->absmax=v3add(self->origin, self->maxs);
	}

	BT_StepEntity_UpdateAngles(self);

//	self->origin=v3addscale(self->origin, self->velocity, btge_framedt);
	BT_CheckFireBoxTrigger(self);
}

void BT_StepEntity_Noclip(btEntity self)
{
	self->origin=v3addscale(self->origin, self->velocity, btge_framedt);
}

int BT_StepEntity_FlyMissileR(btEntity self, float dt)
{
	btEntity ents, cur, nxt;
	vec3 org1, vel1, mins1, maxs1, amins1, amaxs1;
	float d, r;
	int i, st, cont;

	if(dt>(1.0/100)) //clamp to a certain rate
	{
		//check for subdivision of faster movements
		d=v3len(self->velocity)*dt;
		r=btCalcRadius(self);
		if(r<4)r=4;
//		if(d>(1.717*r))		//object is moving quickly
		if(d>(1.717*r) || (d>8))		//object is moving quickly
		{
			//so, divide movement in half
			i=BT_StepEntity_FlyMissileR(self, dt*0.5);
			if(i)return(i);
			i=BT_StepEntity_FlyMissileR(self, dt*0.5);
			return(i);
		}
	}

//	printf("BT_StepEntity_FlyMissileR dt=%f d=%f r=%f\n", dt, d, r);

	//predict ending position
	org1=v3addscale(self->origin, self->velocity, dt);

	st=0;

	if(self->flags&BT_FL_FLY)
	{
		//setting flag disables bbox expand
		mins1=self->mins;
		maxs1=self->maxs;
	}else
	{
		//pad out bbox by movement vector
		vel1=vec3(	fabs(v3x(self->velocity)),
					fabs(v3y(self->velocity)),
					fabs(v3z(self->velocity)));
		mins1=v3addscale(self->mins, vel1, -dt);
		maxs1=v3addscale(self->maxs, vel1, dt);
	}

	amins1=v3add(org1, mins1);
	amaxs1=v3add(org1, maxs1);

#if 1
	ents=BT_CheckBoxEnts(v3add(org1, mins1), v3add(org1, maxs1),
		BT_TRACE_BSPSOLID|BT_TRACE_MONSTERS, self);

//	ents=BT_CheckBoxEnts(v3add(org1, self->mins), v3add(org1, self->maxs),
//		BT_TRACE_BSPSOLID|BT_TRACE_MONSTERS, self);
	cur=ents;
	while(cur)
	{
		cont=btEntityBoxContents(cur, amins1, amaxs1);
		if(!(cont&BT_CONTENTS_MONSTERSOLID))
			{ cur=cur->chain; continue; }
		if(cont&BT_CONTENTS_SKY)
			{ cur=cur->chain; continue; }

		nxt=cur->chain;
		self->velocity=vec3(0,0,0);
		btEntTouch(self, cur);
		cur=nxt;
		st=1;
	}
#endif

//	i=btPointBoxContents(org1, self->mins, self->maxs, self);
	i=btPointBoxContents(org1, mins1, maxs1, self);
	if((i&BT_CONTENTS_MONSTERSOLID) && !(i&BT_CONTENTS_SKY))
	{
		self->velocity=vec3(0,0,0);
		btEntTouch(self, NULL);
		st=1;
	}

	if(st)return(st);

	self->origin=org1;
	BT_CheckFirePadBoxTrigger(self);
	return(0);
}

void BT_StepEntity_FlyMissile(btEntity self)
{
	BT_StepEntity_UpdateAngles(self);

	BT_StepEntity_FlyMissileR(self, btge_framedt);
}


int BT_StepEntity_BounceR(btEntity self, float dt)
{
	btEntity ents, cur, nxt;
	vec3 org1, vel1, mins1, maxs1, amins1, amaxs1;
	float f, g, d, r;
	int i, st, cont;

	if(dt>(1.0/100)) //clamp to a certain rate
	{
		//check for subdivision of faster movements
		d=v3len(self->velocity)*dt;
		r=btCalcRadius(self);
		if(r<4)r=4;
		if(d>(1.717*r))		//object is moving quickly
		{
			//so, divide movement in half
			i=BT_StepEntity_BounceR(self, dt*0.5);
			if(i)return(i);
			i=BT_StepEntity_BounceR(self, dt*0.5);
			return(i);
		}
	}

//	printf("BT_StepEntity_FlyMissileR dt=%f d=%f r=%f\n", dt, d, r);

	//predict ending position
	org1=v3addscale(self->origin, self->velocity, dt);

	st=0;

	//pad out bbox by movement vector
	vel1=vec3(	fabs(v3x(self->velocity)),
				fabs(v3y(self->velocity)),
				fabs(v3z(self->velocity)));
	mins1=v3addscale(self->mins, vel1, -dt);
	maxs1=v3addscale(self->maxs, vel1, dt);

	amins1=v3add(org1, mins1);
	amaxs1=v3add(org1, maxs1);

#if 1
	ents=BT_CheckBoxEnts(v3add(org1, mins1), v3add(org1, maxs1),
		BT_TRACE_BSPSOLID|BT_TRACE_MONSTERS, self);

//	ents=BT_CheckBoxEnts(v3add(org1, self->mins), v3add(org1, self->maxs),
//		BT_TRACE_BSPSOLID|BT_TRACE_MONSTERS, self);
	cur=ents;
	while(cur)
	{
		cont=btEntityBoxContents(cur, amins1, amaxs1);
		if(!(cont&BT_CONTENTS_MONSTERSOLID))
			{ cur=cur->chain; continue; }
		if(cont&BT_CONTENTS_SKY)
			{ cur=cur->chain; continue; }

		nxt=cur->chain;
//		self->velocity=vec3(0,0,0);
		btEntTouch(self, cur);
		cur=nxt;
		st=1;
	}
#endif

//	i=btPointBoxContents(org1, self->mins, self->maxs, self);
	i=btPointBoxContents(org1, mins1, maxs1, self);
	if((i&BT_CONTENTS_MONSTERSOLID) && !(i&BT_CONTENTS_SKY))
	{
//		self->velocity=vec3(0,0,0);
		btEntTouch(self, NULL);
		st=1;
	}

	if(st)
	{
		vel1=v3mul(self->velocity,
			vec3(btSRandom(),btSRandom(),btSRandom()));

//		vel1=v3add(self->velocity,
//			vec3(btSRandom(),btSRandom(),btSRandom()));

//		vel1=v3add(v3scale(self->velocity, -0.75),
//			vec3(btSRandom(),btSRandom(),btSRandom()));

		vel1=v3add(vel1,
			vec3(btSRandom(),btSRandom(),btSRandom()));

//		vel1=vec3(btSRandom(),btSRandom(),btSRandom());

		f=v3len(self->velocity)*0.75;
		g=v3len(vel1);
		vel1=v3scale(vel1, (f+1)/(g+1));
		self->velocity=vel1;

		return(st);
	}

	self->origin=org1;
	BT_CheckFirePadBoxTrigger(self);
	return(0);
}

void BT_StepEntity_Bounce(btEntity self)
{
	BT_StepEntity_AddGravity(self);
	BT_StepEntity_UpdateAngles(self);

	BT_StepEntity_BounceR(self, btge_framedt);

#if 0
	vec3 org1, vel1;
	float f, g;
	int i;

	BT_StepEntity_AddGravity(self);
	BT_StepEntity_UpdateAngles(self);

	org1=v3addscale(self->origin, self->velocity, btge_framedt);

	i=btPointBoxContents(org1, self->mins, self->maxs, self);
	if(!(i&BT_CONTENTS_ITEMSOLID))
	{
//		self->velocity=vec3(0,0,0);

		vel1=v3mul(self->velocity,
			vec3(btRandom(),btRandom(),btRandom()));
		f=v3len(self->velocity);
		g=v3len(vel1);
		vel1=v3scale(vel1, (f+1)/(g+1));
		self->velocity=vel1;
		return;
	}
	self->origin=org1;
	BT_CheckFireBoxTrigger(self);
#endif
}

void BT_StepEntity_BounceMissile(btEntity self)
{
	vec3 org1, vel1;
	float f, g;
	int i;

	BT_StepEntity_AddGravity(self);
	BT_StepEntity_UpdateAngles(self);

	org1=v3addscale(self->origin, self->velocity, btge_framedt);

	i=btPointBoxContents(org1, self->mins, self->maxs, self);
	if(!(i&BT_CONTENTS_ITEMSOLID))
	{
//		self->velocity=vec3(0,0,0);

		vel1=v3mul(self->velocity,
			vec3(btRandom(),btRandom(),btRandom()));
		f=v3len(self->velocity);
		g=v3len(vel1);
		vel1=v3scale(vel1, (f+1)/(g+1));
		self->velocity=vel1;
		return;
	}
	self->origin=org1;
	BT_CheckFirePadBoxTrigger(self);
}

void BT_StepEntity_PhysicsRBD(btEntity self)
{
	btge_world->phys_cnt++;

	if(v3len(self->origin)>=999999)
	{
		//entity has flown away
		printf("BT_StepEntity_PhysicsRBD: Entity %d has flown away\n",
			self->entnum);
		btDeleteEntity(self);
		return;
	}

//	BTGE_EntPhys_UpdateEntity(self);
//	self->flags&=~(BT_FL_SET_SOLID|BT_FL_SET_POSITION);
}

void BT_PreStepEntity(btEntity self)
{
	self->absmin=v3add(self->origin, self->mins);
	self->absmax=v3add(self->origin, self->maxs);

//	if(self->vt && self->vt->prethink)
//		self->vt->prethink(self);
}

void BT_StepEntity(btEntity self)
{
	self->absmin=v3add(self->origin, self->mins);
	self->absmax=v3add(self->origin, self->maxs);

	if(self->vt && self->vt->prethink)
	{
//		self->vt->prethink(self);
		btEntPreThink(self);
		
		//in case of teleport
		self->absmin=v3add(self->origin, self->mins);
		self->absmax=v3add(self->origin, self->maxs);
	}
	
	if((self->nextthink>0) && (self->nextthink<=btge_time))
	{
		self->nextthink=0;
		if(self->dy_think)
		{
			dyApplyMethod1(self->dy_this, self->dy_think, (dyt)self);
		}else if(self->f_think)
		{
			self->f_think(self);
		}else if(self->vt && self->vt->think)
		{
			self->vt->think(self);
		}
	}

	if(self->flags&BT_FL_CLIENT)
	{
		BT_ClientThink(self);
	}

	if(self->flags&BT_FL_SCRIPTED)
	{
		BTAI_ScriptedThink(self);
	}

	switch(self->movetype)
	{
	case BT_MOVE_NONE: break;
	case BT_MOVE_WALK:			BT_StepEntity_Walk(self); break;
	case BT_MOVE_STEP:			BT_StepEntity_Step(self); break;
	case BT_MOVE_FLY:			BT_StepEntity_Fly(self); break;
	case BT_MOVE_TOSS:			BT_StepEntity_Toss(self); break;
	case BT_MOVE_PUSH:			BT_StepEntity_Push(self); break;
	case BT_MOVE_NOCLIP:		BT_StepEntity_Noclip(self); break;
	case BT_MOVE_FLYMISSILE:	BT_StepEntity_FlyMissile(self); break;
	case BT_MOVE_BOUNCE:		BT_StepEntity_Bounce(self); break;
	case BT_MOVE_BOUNCEMISSILE: BT_StepEntity_BounceMissile(self); break;
	case BT_MOVE_RBD_PHYSICS:	BT_StepEntity_PhysicsRBD(self); break;
	default: break;
	}
}

void BT_StepWorld2(float dt)
{
	btEntity cur;
	int i, j;

	BTGE_EntBSP_BuildEntityBSP(btge_world);
//	BTGE_Voxel_UpdateWorldTick(btge_world, dt);

	btge_framedt=dt;
	btge_time+=dt;
	btge_world->phys_cnt=0;

	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		BT_PreStepEntity(cur);
	}
	
	for(i=1; i<btge_world->n_ents; i++)
	{
		cur=btge_world->ents[i];
		if(!cur)continue;
		BT_StepEntity(cur);
	}

	if(btge_world->phys_cnt>0)
	{
		//only do rigid-body physics if entities exist which use it.
		BTGE_BrushPhys_UpdateWorld(btge_world);
		BTGE_BrushPhys_StepWorld(btge_world, dt);
	}
}

/** Advance world by a certain amount of time */
BTSV_API bool btsvStepWorld(float dt)
{
//	static float tick=1.0/10;
	static float tick=1.0/16;
	static float voxtick=1.0/10;
	static float atick=0;
	btEntity cl;
	bool b;

	if(dt>0.33)
		return(false);

	BTGE_BrushWorld_DrawBuildFast(btge_world);

#if 0	
	if(dt>0.25)
	{
		atick+=dt;
		return;
	}
	if(atick>dt)
	{
		atick-=dt;
		dt=tick;
	}
#endif
	
#if 1
	cl=btGetCurrentClientList();
	if(cl)
	{
		BTGE_Voxel_ClearActiveWorldTick(btge_world);
		while(cl)
		{
			BTGE_Voxel_SetActiveWorldTick(btge_world, cl->origin);
			cl=cl->chain;
		}
	}
	btge_voxsteptime+=dt;
	while(btge_voxsteptime>voxtick)
	{
		BTGE_Voxel_UpdateWorldTick(btge_world, voxtick);
		btge_voxsteptime-=voxtick;
	}
#endif

	btge_steptime+=dt; b=false;
	while(btge_steptime>tick)
	{
		BT_StepWorld2(tick);
		btge_steptime-=tick;
		b=true;
	}
	
	return(b);
}
