#include <btgesv.h>

BTGE_MoveFrame monster_wizard_stand1_mf[]=
{
{BTAI_Stand, 0, monster_wizard_idle},
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

BTGE_MoveInfo monster_wizard_stand1_mfi =
	{"stand", 0, 30, monster_wizard_stand1_mf, monster_wizard_stand};

BTGE_MoveFrame monster_wizard_walk1_mf[]=
{
{BTAI_Walk, 3, monster_wizard_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_wizard_walk1_random},

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

BTGE_MoveFrame monster_wizard_walk2_mf[]=
{
{BTAI_Walk, 3, monster_wizard_idle},
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

BTGE_MoveInfo monster_wizard_walk1_mfi =
	{"walk", 0, 20, monster_wizard_walk1_mf, monster_wizard_walk};

BTGE_MoveFrame monster_wizard_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_wizard_run_mfi =
	{"run", 2, 8, monster_wizard_run_mf, monster_wizard_run};

BTGE_MoveFrame monster_wizard_attack1_mf[]=
{
{BTAI_Charge, 0, monster_wizard_fire},
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

BTGE_MoveInfo monster_wizard_attack1_mfi =
	{"attak1", 2, 8, monster_wizard_attack1_mf, monster_wizard_run};


void monster_wizard_idle(btEntity self)
{
}


void monster_wizard_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/hover/hover.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=80;
	
	btFlymonsterStart(self);
}

void monster_wizard_deinit(btEntity self)
	{ }
void monster_wizard_null(btEntity self)
	{ }

void monster_wizard_blocked(btEntity self, btEntity other)
	{ }
void monster_wizard_touch(btEntity self, btEntity other)
	{ }
void monster_wizard_use(btEntity self, btEntity other)
	{ }
void monster_wizard_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_wizard_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

void monster_wizard_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_wizard_stand(btEntity self)
{
	btSetMove(self, &monster_wizard_stand1_mfi);
}

void monster_wizard_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_wizard_walk1_mfi);
}

void monster_wizard_walk(btEntity self)
{
	btSetMove(self, &monster_wizard_walk1_mfi);
}

void monster_wizard_run(btEntity self)
{
	btSetMove(self, &monster_wizard_run_mfi);
}

void monster_wizard_missile(btEntity self)
{
	btSetMove(self, &monster_wizard_attack1_mfi);
}

void monster_wizard_fire(btEntity self)
{
	vec3 org, dir;
	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
	dir=BT_TargetDirection(self, self->enemy);
//	BT_FireRocket(self, org, dir, 10, 600, 25);
	BT_FireBlaster(self, org, dir, 10, 600, 25);
}

void monster_wizard_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_wizard_vt =
{
monster_wizard_init,			monster_wizard_deinit,
monster_wizard_null,			monster_wizard_null,
monster_wizard_blocked,		monster_wizard_touch,
monster_wizard_use,			monster_wizard_pain,
monster_wizard_die,			monster_wizard_cmdmsg,
};

BTGE_Actor_Iface monster_wizard_avt =
{
monster_wizard_stand,
monster_wizard_walk,
monster_wizard_run,
monster_wizard_missile,
//monster_wizard_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_wizard(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_wizard_vt,
		&monster_wizard_avt);
	return(self);
}
