#include <btgesv.h>

BTGE_MoveFrame monster_shalrath_stand1_mf[]=
{
{BTAI_Stand, 0, monster_shalrath_idle},
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

BTGE_MoveInfo monster_shalrath_stand1_mfi =
	{"stand", 0, 30, monster_shalrath_stand1_mf, monster_shalrath_stand};

BTGE_MoveFrame monster_shalrath_walk1_mf[]=
{
{BTAI_Walk, 3, monster_shalrath_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_shalrath_walk1_random},

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

BTGE_MoveFrame monster_shalrath_walk2_mf[]=
{
{BTAI_Walk, 3, monster_shalrath_idle},
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

BTGE_MoveInfo monster_shalrath_walk1_mfi =
	{"walk", 0, 20, monster_shalrath_walk1_mf, monster_shalrath_walk};

BTGE_MoveFrame monster_shalrath_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_shalrath_run_mfi =
	{"run", 2, 8, monster_shalrath_run_mf, monster_shalrath_run};

BTGE_MoveFrame monster_shalrath_attack1_mf[]=
{
{BTAI_Charge, 0, monster_shalrath_fire},
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

BTGE_MoveInfo monster_shalrath_attack1_mfi =
	{"attak1", 2, 8, monster_shalrath_attack1_mf, monster_shalrath_run};


void monster_shalrath_idle(btEntity self)
{
	if(btRandom()<0.1)
	{
		btSound(self, BT_CHAN_VOICE, self->snd_idle,
			1.0, BT_ATTN_NORM);
	}
}


void monster_shalrath_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/bitch/bitch.model");
	self->snd_sight="sound/chick/chksght1";
	self->snd_idle="sound/chick/chkidle1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=400;
	
	btWalkmonsterStart(self);
}

void monster_shalrath_deinit(btEntity self)
	{ }
void monster_shalrath_null(btEntity self)
	{ }

void monster_shalrath_blocked(btEntity self, btEntity other)
	{ }
void monster_shalrath_touch(btEntity self, btEntity other)
	{ }
void monster_shalrath_use(btEntity self, btEntity other)
	{ }
void monster_shalrath_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_shalrath_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

void monster_shalrath_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_shalrath_stand(btEntity self)
{
	btSetMove(self, &monster_shalrath_stand1_mfi);
}

void monster_shalrath_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_shalrath_walk1_mfi);
}

void monster_shalrath_walk(btEntity self)
{
	btSetMove(self, &monster_shalrath_walk1_mfi);
}

void monster_shalrath_run(btEntity self)
{
	btSetMove(self, &monster_shalrath_run_mfi);
}

void monster_shalrath_missile(btEntity self)
{
	btSetMove(self, &monster_shalrath_attack1_mfi);
}

void monster_shalrath_fire(btEntity self)
{
	vec3 org, dir;

	btSound(self, BT_CHAN_VOICE, "sound/chick/chkatck1",
		1.0, BT_ATTN_NORM);

	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
	dir=BT_TargetDirection(self, self->enemy);
	BT_FireRocket(self, org, dir, 10, 600, 25);
//	BT_FireBlaster(self, org, dir, 10, 600, 25);
}

void monster_shalrath_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_shalrath_vt =
{
monster_shalrath_init,			monster_shalrath_deinit,
monster_shalrath_null,			monster_shalrath_null,
monster_shalrath_blocked,		monster_shalrath_touch,
monster_shalrath_use,			monster_shalrath_pain,
monster_shalrath_die,			monster_shalrath_cmdmsg,
};

BTGE_Actor_Iface monster_shalrath_avt =
{
monster_shalrath_stand,
monster_shalrath_walk,
monster_shalrath_run,
monster_shalrath_missile,
//monster_shalrath_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_shalrath(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_shalrath_vt,
		&monster_shalrath_avt);
	return(self);
}
