#include <btgesv.h>

BTGE_MoveFrame monster_enforcer_stand1_mf[]=
{
{BTAI_Stand, 0, monster_enforcer_idle},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},

{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},

{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},
{BTAI_Stand, 0, NULL},

{BTAI_Stand, 0, NULL},
};

BTGE_MoveInfo monster_enforcer_stand1_mfi =
	{"stand", 0, 30, monster_enforcer_stand1_mf, monster_enforcer_stand};

BTGE_MoveFrame monster_enforcer_walk1_mf[]=
{
{BTAI_Walk, 3, monster_enforcer_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_enforcer_walk1_random},

{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},

{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},

{BTAI_Walk, 0, NULL},
{BTAI_Walk, 0, NULL},
};

BTGE_MoveFrame monster_enforcer_walk2_mf[]=
{
{BTAI_Walk, 3, monster_enforcer_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, NULL},
};

BTGE_MoveInfo monster_enforcer_walk1_mfi =
	{"walk", 0, 20, monster_enforcer_walk1_mf, monster_enforcer_walk};

BTGE_MoveFrame monster_enforcer_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_enforcer_run_mfi =
	{"run", 2, 8, monster_enforcer_run_mf, monster_enforcer_run};

BTGE_MoveFrame monster_enforcer_attack1_mf[]=
{
{BTAI_Charge, 0, monster_enforcer_fire},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
{BTAI_Charge, 0, NULL},
};

BTGE_MoveInfo monster_enforcer_attack1_mfi =
	{"attak1", 2, 8, monster_enforcer_attack1_mf, monster_enforcer_run};


void monster_enforcer_idle(btEntity self)
{
}


void monster_enforcer_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/enforcer/enforcer.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=80;
	
	btWalkmonsterStart(self);
}

void monster_enforcer_deinit(btEntity self)
	{ }
void monster_enforcer_null(btEntity self)
	{ }

void monster_enforcer_blocked(btEntity self, btEntity other)
	{ }
void monster_enforcer_touch(btEntity self, btEntity other)
	{ }
void monster_enforcer_use(btEntity self, btEntity other)
	{ }
void monster_enforcer_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_enforcer_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
	BT_EntityRobotExplode(self);
}

void monster_enforcer_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_enforcer_stand(btEntity self)
{
	btSetMove(self, &monster_enforcer_stand1_mfi);
}

void monster_enforcer_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_enforcer_walk1_mfi);
}

void monster_enforcer_walk(btEntity self)
{
	btSetMove(self, &monster_enforcer_walk1_mfi);
}

void monster_enforcer_run(btEntity self)
{
	btSetMove(self, &monster_enforcer_run_mfi);
}

void monster_enforcer_missile(btEntity self)
{
	btSetMove(self, &monster_enforcer_attack1_mfi);
}

void monster_enforcer_fire(btEntity self)
{
	vec3d org;
	vec3 dir;
//	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
//	org=btCalcCenter(self);
//	dir=BT_TargetDirection(self, self->enemy);

	org=BT_AimOrigin(self);
	dir=BT_AimDirection(self, self->enemy, 600);

//	BT_FireRocket(self, org, dir, 10, 600, 25);
//	BT_FireBlaster(self, org, dir, 10, 600, 25);
	BT_FirePlasma(self, org, dir, 45, 800, 60, 1);
}

void monster_enforcer_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_enforcer_vt =
{
monster_enforcer_init,			monster_enforcer_deinit,
monster_enforcer_null,			monster_enforcer_null,
monster_enforcer_blocked,		monster_enforcer_touch,
monster_enforcer_use,			monster_enforcer_pain,
monster_enforcer_die,			monster_enforcer_cmdmsg,
};

BTGE_Actor_Iface monster_enforcer_avt =
{
monster_enforcer_stand,
monster_enforcer_walk,
monster_enforcer_run,
monster_enforcer_missile,
//monster_enforcer_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_enforcer(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_enforcer_vt,
		&monster_enforcer_avt);
	return(self);
}

BTSV_API void BtMt_monster_enforcer(btItem item)
{
	item->dropWeight=20;
}
