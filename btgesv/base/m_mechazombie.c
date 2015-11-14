#include <btgesv.h>

BTGE_MoveFrame monster_mechazombie_stand1_mf[]=
{
{BTAI_Stand, 0, monster_mechazombie_idle},
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

BTGE_MoveInfo monster_mechazombie_stand1_mfi =
	{"stand", 0, 30, monster_mechazombie_stand1_mf, monster_mechazombie_stand};

BTGE_MoveFrame monster_mechazombie_walk1_mf[]=
{
{BTAI_Walk, 3, monster_mechazombie_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_mechazombie_walk1_random},

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

BTGE_MoveFrame monster_mechazombie_walk2_mf[]=
{
{BTAI_Walk, 3, monster_mechazombie_idle},
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

BTGE_MoveInfo monster_mechazombie_walk1_mfi =
	{"walk", 0, 20, monster_mechazombie_walk1_mf, monster_mechazombie_walk};

BTGE_MoveFrame monster_mechazombie_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_mechazombie_run_mfi =
	{"run", 2, 8, monster_mechazombie_run_mf, monster_mechazombie_run};

BTGE_MoveFrame monster_mechazombie_attack1_mf[]=
{
{BTAI_Charge, 0, monster_mechazombie_fire},
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

BTGE_MoveInfo monster_mechazombie_attack1_mfi =
	{"attak1", 2, 8, monster_mechazombie_attack1_mf, monster_mechazombie_run};


void monster_mechazombie_idle(btEntity self)
{
}


void monster_mechazombie_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/mech_zombie/insane.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=100;

	if(self->spawnflags&1)
	{
		self->flags|=BT_FL_AI_PASSIVE;
	}

	//attach constant engine noise
	btSound(self, BT_CHAN_AMBIENT, "sound/mechazombie/engine3",
		1.0, BT_ATTN_STATIC);
	
	btWalkmonsterStart(self);
}

void monster_mechazombie_deinit(btEntity self)
	{ }
void monster_mechazombie_null(btEntity self)
	{ }

void monster_mechazombie_blocked(btEntity self, btEntity other)
	{ }
void monster_mechazombie_touch(btEntity self, btEntity other)
	{ }
void monster_mechazombie_use(btEntity self, btEntity other)
	{ }
void monster_mechazombie_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_mechazombie_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
	BT_EntityRobotExplode(self);
}

void monster_mechazombie_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_mechazombie_stand(btEntity self)
{
	btSetMove(self, &monster_mechazombie_stand1_mfi);
}

void monster_mechazombie_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_mechazombie_walk1_mfi);
}

void monster_mechazombie_walk(btEntity self)
{
	btSetMove(self, &monster_mechazombie_walk1_mfi);
}

void monster_mechazombie_run(btEntity self)
{
	btSetMove(self, &monster_mechazombie_run_mfi);
}

void monster_mechazombie_missile(btEntity self)
{
	btSetMove(self, &monster_mechazombie_attack1_mfi);
}

void monster_mechazombie_fire(btEntity self)
{
	vec3d org;
	vec3 dir;
//	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
//	dir=BT_TargetDirection(self, self->enemy);
	org=BT_AimOrigin(self);
	dir=BT_AimDirection(self, self->enemy, 600);
//	BT_FireRocket(self, org, dir, 10, 600, 25);
	BT_FireBlaster(self, org, dir, 10, 600, 25);
}

void monster_mechazombie_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_mechazombie_vt =
{
monster_mechazombie_init,			monster_mechazombie_deinit,
monster_mechazombie_null,			monster_mechazombie_null,
monster_mechazombie_blocked,		monster_mechazombie_touch,
monster_mechazombie_use,			monster_mechazombie_pain,
monster_mechazombie_die,			monster_mechazombie_cmdmsg,
};

BTGE_Actor_Iface monster_mechazombie_avt =
{
monster_mechazombie_stand,
monster_mechazombie_walk,
monster_mechazombie_run,
monster_mechazombie_missile,
//monster_mechazombie_melee,
NULL,
};


BTSV_API btEntity BtSp_monster_zombie_mecha(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_mechazombie_vt,
		&monster_mechazombie_avt);
	return(self);
}
