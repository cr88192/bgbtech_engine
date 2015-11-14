#include <btgesv.h>

BTGE_MoveFrame monster_generic_stand1_mf[]=
{
{BTAI_Stand, 0, monster_generic_idle},
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

BTGE_MoveInfo monster_generic_stand1_mfi =
	{"stand1", 0, 30, monster_generic_stand1_mf, monster_generic_stand};

BTGE_MoveFrame monster_generic_walk1_mf[]=
{
{BTAI_Walk, 3, monster_generic_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_generic_walk1_random},

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

BTGE_MoveFrame monster_generic_walk2_mf[]=
{
{BTAI_Walk, 3, monster_generic_idle},
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

BTGE_MoveInfo monster_generic_walk1_mfi =
	{"walk1", 0, 32, monster_generic_walk1_mf, monster_generic_walk};
BTGE_MoveInfo monster_generic_walk2_mfi =
	{"walk2", 0, 10, monster_generic_walk2_mf, monster_generic_walk};

BTGE_MoveFrame monster_generic_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_generic_run_mfi =
	{"run", 2, 8, monster_generic_run_mf, monster_generic_run};

BTGE_MoveFrame monster_generic_attack1_mf[]=
{
{BTAI_Charge, 0, monster_generic_fire},
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

BTGE_MoveInfo monster_generic_attack1_mfi =
	{"attack1", 2, 8, monster_generic_attack1_mf, monster_generic_run};


void monster_generic_idle(btEntity self)
{
}


void monster_generic_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/soldier/soldier.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=30;
	
	btWalkmonsterStart(self);
}

void monster_generic_deinit(btEntity self)
	{ }
void monster_generic_null(btEntity self)
	{ }

void monster_generic_blocked(btEntity self, btEntity other)
	{ }
void monster_generic_touch(btEntity self, btEntity other)
	{ }
void monster_generic_use(btEntity self, btEntity other)
	{ }
void monster_generic_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_generic_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
	BT_EntityRobotExplode(self);
}

void monster_generic_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_generic_stand(btEntity self)
{
	btSetMove(self, &monster_generic_stand1_mfi);
}

void monster_generic_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_generic_walk1_mfi);
}

void monster_generic_walk(btEntity self)
{
	if(btRandom()>0.5)
		btSetMove(self, &monster_generic_walk1_mfi);
	else
		btSetMove(self, &monster_generic_walk2_mfi);
}

void monster_generic_run(btEntity self)
{
	btSetMove(self, &monster_generic_run_mfi);
}

void monster_generic_missile(btEntity self)
{
	btSetMove(self, &monster_generic_attack1_mfi);
}

void monster_generic_fire(btEntity self)
{
	vec3d org;
	vec3 dir;
	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
	dir=BT_TargetDirection(self, self->enemy);
//	BT_FireRocket(self, org, dir, 10, 600, 25);
	BT_FireBlaster(self, org, dir, 10, 600, 25);
}

void monster_generic_melee(btEntity self)
{
}


BTGE_Entity_Iface monster_generic_vt =
{
monster_generic_init,			monster_generic_deinit,
monster_generic_null,			monster_generic_null,
monster_generic_blocked,		monster_generic_touch,
monster_generic_use,			monster_generic_pain,
monster_generic_die,			monster_generic_cmdmsg,
};

BTGE_Actor_Iface monster_generic_avt =
{
monster_generic_stand,
monster_generic_walk,
monster_generic_run,
monster_generic_missile,
//monster_generic_melee,
NULL,
};

#if 0
// BTSV_API btEntity BtSp_monster_army(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_generic_vt,
		&monster_generic_avt);
	return(self);
}
#endif

BTSV_API void BtMt_monster_army(btItem item)
{
	item->dropWeight=4*100;
}
