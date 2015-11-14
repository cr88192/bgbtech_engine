#include <btgesv.h>

BTGE_MoveFrame monster_ogre_stand1_mf[]=
{
{BTAI_Stand, 0, monster_ogre_idle},
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

BTGE_MoveInfo monster_ogre_stand1_mfi =
	{"stand", 0, 30, monster_ogre_stand1_mf, monster_ogre_stand};

BTGE_MoveFrame monster_ogre_walk1_mf[]=
{
{BTAI_Walk, 3, monster_ogre_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_ogre_walk1_random},

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

BTGE_MoveFrame monster_ogre_walk2_mf[]=
{
{BTAI_Walk, 3, monster_ogre_idle},
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

BTGE_MoveInfo monster_ogre_walk1_mfi =
	{"walk", 0, 20, monster_ogre_walk1_mf, monster_ogre_walk};

BTGE_MoveFrame monster_ogre_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_ogre_run_mfi =
	{"run", 2, 8, monster_ogre_run_mf, monster_ogre_run};

BTGE_MoveFrame monster_ogre_attack1_mf[]=
{
{BTAI_Charge, 0, monster_ogre_fire},
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

BTGE_MoveInfo monster_ogre_attack1_mfi =
	{"attak1", 2, 8, monster_ogre_attack1_mf, monster_ogre_run};


void monster_ogre_idle(btEntity self)
{
}


void monster_ogre_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/infantry/infantry.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=200;
	
	btWalkmonsterStart(self);
}

void monster_ogre_deinit(btEntity self)
	{ }
void monster_ogre_null(btEntity self)
	{ }

void monster_ogre_blocked(btEntity self, btEntity other)
	{ }
void monster_ogre_touch(btEntity self, btEntity other)
	{ }
void monster_ogre_use(btEntity self, btEntity other)
	{ }
void monster_ogre_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_ogre_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

void monster_ogre_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_ogre_stand(btEntity self)
{
	btSetMove(self, &monster_ogre_stand1_mfi);
}

void monster_ogre_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_ogre_walk1_mfi);
}

void monster_ogre_walk(btEntity self)
{
	btSetMove(self, &monster_ogre_walk1_mfi);
}

void monster_ogre_run(btEntity self)
{
	btSetMove(self, &monster_ogre_run_mfi);
}

void monster_ogre_missile(btEntity self)
{
	btSetMove(self, &monster_ogre_attack1_mfi);
}

void monster_ogre_fire(btEntity self)
{
	vec3 org, dir;
	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
	dir=BT_TargetDirection(self, self->enemy);
//	BT_FireRocket(self, org, dir, 10, 600, 25);
//	BT_FireBlaster(self, org, dir, 10, 600, 25);
	BT_FirePlasma(self, org, dir, 25, 800, 32, 2);
}

void monster_ogre_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_ogre_vt =
{
monster_ogre_init,			monster_ogre_deinit,
monster_ogre_null,			monster_ogre_null,
monster_ogre_blocked,		monster_ogre_touch,
monster_ogre_use,			monster_ogre_pain,
monster_ogre_die,			monster_ogre_cmdmsg,
};

BTGE_Actor_Iface monster_ogre_avt =
{
monster_ogre_stand,
monster_ogre_walk,
monster_ogre_run,
monster_ogre_missile,
//monster_ogre_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_ogre(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_ogre_vt,
		&monster_ogre_avt);
	return(self);
}
