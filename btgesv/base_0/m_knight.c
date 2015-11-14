#include <btgesv.h>

BTGE_MoveFrame monster_knight_stand1_mf[]=
{
{BTAI_Stand, 0, monster_knight_idle},
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

BTGE_MoveInfo monster_knight_stand1_mfi =
	{"stand", 0, 30, monster_knight_stand1_mf, monster_knight_stand};

BTGE_MoveFrame monster_knight_walk1_mf[]=
{
{BTAI_Walk, 3, monster_knight_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_knight_walk1_random},

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

BTGE_MoveFrame monster_knight_walk2_mf[]=
{
{BTAI_Walk, 3, monster_knight_idle},
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

BTGE_MoveInfo monster_knight_walk1_mfi =
	{"walk", 0, 20, monster_knight_walk1_mf, monster_knight_walk};

BTGE_MoveFrame monster_knight_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_knight_run_mfi =
	{"run", 2, 8, monster_knight_run_mf, monster_knight_run};

BTGE_MoveFrame monster_knight_attack1_mf[]=
{
{BTAI_Charge, 0, monster_knight_fire},
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

BTGE_MoveInfo monster_knight_attack1_mfi =
	{"attak1", 2, 8, monster_knight_attack1_mf, monster_knight_run};


void monster_knight_idle(btEntity self)
{
}


void monster_knight_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/berserk/berserk.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=50;
	
	btWalkmonsterStart(self);
}

void monster_knight_deinit(btEntity self)
	{ }
void monster_knight_null(btEntity self)
	{ }

void monster_knight_blocked(btEntity self, btEntity other)
	{ }
void monster_knight_touch(btEntity self, btEntity other)
	{ }
void monster_knight_use(btEntity self, btEntity other)
	{ }
void monster_knight_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_knight_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

void monster_knight_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_knight_stand(btEntity self)
{
	btSetMove(self, &monster_knight_stand1_mfi);
}

void monster_knight_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_knight_walk1_mfi);
}

void monster_knight_walk(btEntity self)
{
	btSetMove(self, &monster_knight_walk1_mfi);
}

void monster_knight_run(btEntity self)
{
	btSetMove(self, &monster_knight_run_mfi);
}

void monster_knight_missile(btEntity self)
{
	btSetMove(self, &monster_knight_attack1_mfi);
}

void monster_knight_fire(btEntity self)
{
	vec3 org, dir;
	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
	dir=BT_TargetDirection(self, self->enemy);
//	BT_FireRocket(self, org, dir, 10, 600, 25);
	BT_FireBlaster(self, org, dir, 10, 600, 25);
}

void monster_knight_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_knight_vt =
{
monster_knight_init,			monster_knight_deinit,
monster_knight_null,			monster_knight_null,
monster_knight_blocked,		monster_knight_touch,
monster_knight_use,			monster_knight_pain,
monster_knight_die,			monster_knight_cmdmsg,
};

BTGE_Actor_Iface monster_knight_avt =
{
monster_knight_stand,
monster_knight_walk,
monster_knight_run,
monster_knight_missile,
//monster_knight_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_knight(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_knight_vt,
		&monster_knight_avt);
	return(self);
}
