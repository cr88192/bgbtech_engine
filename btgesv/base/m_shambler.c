#include <btgesv.h>

BTGE_MoveFrame monster_shambler_stand1_mf[]=
{
{BTAI_Stand, 0, monster_shambler_idle},
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

BTGE_MoveInfo monster_shambler_stand1_mfi =
	{"stand", 0, 30, monster_shambler_stand1_mf, monster_shambler_stand};

BTGE_MoveFrame monster_shambler_walk1_mf[]=
{
{BTAI_Walk, 3, monster_shambler_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_shambler_walk1_random},

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

BTGE_MoveFrame monster_shambler_walk2_mf[]=
{
{BTAI_Walk, 3, monster_shambler_idle},
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

BTGE_MoveInfo monster_shambler_walk1_mfi =
	{"walk", 0, 20, monster_shambler_walk1_mf, monster_shambler_walk};

BTGE_MoveFrame monster_shambler_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_shambler_run_mfi =
	{"run", 2, 8, monster_shambler_run_mf, monster_shambler_run};

BTGE_MoveFrame monster_shambler_attack1_mf[]=
{
{BTAI_Charge, 0, monster_shambler_fire},
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

BTGE_MoveInfo monster_shambler_attack1_mfi =
	{"attak1", 2, 8, monster_shambler_attack1_mf, monster_shambler_run};


void monster_shambler_idle(btEntity self)
{
}


void monster_shambler_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/tank/tank.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-32, -32, -24);
	self->maxs=vec3(32, 32, 64);
	self->health=600;
	
	btWalkmonsterStart(self);
}

void monster_shambler_deinit(btEntity self)
	{ }
void monster_shambler_null(btEntity self)
	{ }

void monster_shambler_blocked(btEntity self, btEntity other)
	{ }
void monster_shambler_touch(btEntity self, btEntity other)
	{ }
void monster_shambler_use(btEntity self, btEntity other)
	{ }
void monster_shambler_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_shambler_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
	BT_EntityRobotExplode(self);
}

void monster_shambler_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_shambler_stand(btEntity self)
{
	btSetMove(self, &monster_shambler_stand1_mfi);
}

void monster_shambler_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_shambler_walk1_mfi);
}

void monster_shambler_walk(btEntity self)
{
	btSetMove(self, &monster_shambler_walk1_mfi);
}

void monster_shambler_run(btEntity self)
{
	btSetMove(self, &monster_shambler_run_mfi);
}

void monster_shambler_missile(btEntity self)
{
	btSetMove(self, &monster_shambler_attack1_mfi);
}

void monster_shambler_fire(btEntity self)
{
	vec3d org;
	vec3 dir;

//	org=self->origin;
//	org=btCalcCenter(self);
//	dir=btYawVector(btCurrentYaw(self));
//	dir=BT_TargetDirection(self, self->enemy);
	org=BT_AimOrigin(self);
	dir=BT_AimDirection(self, self->enemy, 400);

//	BT_FireRocket(self, org, dir, 10, 600, 25);
//	BT_FireBlaster(self, org, dir, 10, 600, 25);
	BT_FireBFG(self, org, dir, 100, 400, 280);
}

void monster_shambler_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_shambler_vt =
{
monster_shambler_init,			monster_shambler_deinit,
monster_shambler_null,			monster_shambler_null,
monster_shambler_blocked,		monster_shambler_touch,
monster_shambler_use,			monster_shambler_pain,
monster_shambler_die,			monster_shambler_cmdmsg,
};

BTGE_Actor_Iface monster_shambler_avt =
{
monster_shambler_stand,
monster_shambler_walk,
monster_shambler_run,
monster_shambler_missile,
//monster_shambler_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_shambler(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_shambler_vt,
		&monster_shambler_avt);
	return(self);
}

BTSV_API void BtMt_monster_shambler(btItem item)
{
	item->dropWeight=1;
}

BTSV_API void BtMt_monster_enemyhead(btItem item)
{
	item->dropWeight=5;
}
