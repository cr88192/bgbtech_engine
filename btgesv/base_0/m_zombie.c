#include <btgesv.h>

BTGE_MoveFrame monster_zombie_stand1_mf[]=
{
{BTAI_Stand, 0, monster_zombie_idle},
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

BTGE_MoveInfo monster_zombie_stand1_mfi =
	{"stand", 0, 30, monster_zombie_stand1_mf, monster_zombie_stand};

BTGE_MoveFrame monster_zombie_walk1_mf[]=
{
{BTAI_Walk, 3, monster_zombie_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_zombie_walk1_random},

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

BTGE_MoveFrame monster_zombie_walk2_mf[]=
{
{BTAI_Walk, 3, monster_zombie_idle},
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

BTGE_MoveInfo monster_zombie_walk1_mfi =
	{"walk", 0, 20, monster_zombie_walk1_mf, monster_zombie_walk};

BTGE_MoveFrame monster_zombie_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_zombie_run_mfi =
	{"run", 2, 8, monster_zombie_run_mf, monster_zombie_run};

BTGE_MoveFrame monster_zombie_attack1_mf[]=
{
{BTAI_Charge, 0, monster_zombie_fire},
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

BTGE_MoveInfo monster_zombie_attack1_mfi =
	{"attak1", 2, 8, monster_zombie_attack1_mf, monster_zombie_run};


void monster_zombie_idle(btEntity self)
{
	if(btRandom()<0.1)
	{
		btSound(self, BT_CHAN_VOICE, self->snd_idle,
			1.0, BT_ATTN_NORM);
	}
}


void monster_zombie_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/insane/insane.model");
//	self->snd_sight="sound/soldier/solsght1";
	self->snd_idle="sound/zombie/idle1";

	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=50;

	if(self->spawnflags&1)
	{
		self->flags|=BT_FL_AI_PASSIVE;
	}
	
	btWalkmonsterStart(self);
}

void monster_zombie_deinit(btEntity self)
	{ }
void monster_zombie_null(btEntity self)
	{ }

void monster_zombie_blocked(btEntity self, btEntity other)
	{ }
void monster_zombie_touch(btEntity self, btEntity other)
	{ }
void monster_zombie_use(btEntity self, btEntity other)
	{ }
void monster_zombie_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_zombie_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

void monster_zombie_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_zombie_stand(btEntity self)
{
	btSetMove(self, &monster_zombie_stand1_mfi);
}

void monster_zombie_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_zombie_walk1_mfi);
}

void monster_zombie_walk(btEntity self)
{
	btSetMove(self, &monster_zombie_walk1_mfi);
}

void monster_zombie_run(btEntity self)
{
	btSetMove(self, &monster_zombie_run_mfi);
}

void monster_zombie_missile(btEntity self)
{
	btSetMove(self, &monster_zombie_attack1_mfi);
}

void monster_zombie_fire(btEntity self)
{
	vec3 org, dir;
	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
	dir=BT_TargetDirection(self, self->enemy);
//	BT_FireRocket(self, org, dir, 10, 600, 25);
	BT_FireBlaster(self, org, dir, 10, 600, 25);
}

void monster_zombie_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_zombie_vt =
{
monster_zombie_init,		monster_zombie_deinit,
monster_zombie_null,		monster_zombie_null,
monster_zombie_blocked,		monster_zombie_touch,
monster_zombie_use,			monster_zombie_pain,
monster_zombie_die,			monster_zombie_cmdmsg,
};

BTGE_Actor_Iface monster_zombie_avt =
{
monster_zombie_stand,
monster_zombie_walk,
monster_zombie_run,
monster_zombie_missile,
//monster_zombie_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_zombie(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_zombie_vt,
		&monster_zombie_avt);
	return(self);
}
