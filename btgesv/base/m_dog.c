#include <btgesv.h>

BTGE_MoveFrame monster_dog_stand1_mf[]=
{
{BTAI_Stand, 0, monster_dog_idle},
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

BTGE_MoveInfo monster_dog_stand1_mfi =
	{"stand1", 0, 30, monster_dog_stand1_mf, monster_dog_stand};

BTGE_MoveFrame monster_dog_walk1_mf[]=
{
{BTAI_Walk, 3, monster_dog_idle},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 3, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 5, NULL},
{BTAI_Walk, 2, NULL},
{BTAI_Walk, 0, monster_dog_walk1_random},

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

BTGE_MoveFrame monster_dog_walk2_mf[]=
{
{BTAI_Walk, 3, monster_dog_idle},
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

BTGE_MoveInfo monster_dog_walk1_mfi =
	{"walk1", 0, 32, monster_dog_walk1_mf, monster_dog_walk};
BTGE_MoveInfo monster_dog_walk2_mfi =
	{"walk2", 0, 10, monster_dog_walk2_mf, monster_dog_walk};

BTGE_MoveFrame monster_dog_run_mf[]=
{
{BTAI_Run, 10, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 11, NULL},
{BTAI_Run, 15, NULL},
{BTAI_Run, 15, NULL},
};

BTGE_MoveInfo monster_dog_run_mfi =
	{"run", 2, 8, monster_dog_run_mf, monster_dog_run};

BTGE_MoveFrame monster_dog_attack1_mf[]=
{
{BTAI_Charge, 0, monster_dog_fire},
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

BTGE_MoveInfo monster_dog_attack1_mfi =
	{"attack1", 2, 8, monster_dog_attack1_mf, monster_dog_run};


void monster_dog_idle(btEntity self)
{
	if(btRandom()<0.1)
	{
		btSound(self, BT_CHAN_VOICE, self->snd_idle,
			1.0, BT_ATTN_NORM);
	}
}


void monster_dog_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/dog/dog.model");
//	self->snd_sight="sound/soldier/solsght1";
	self->snd_sight="sound/dog/dsight";
	self->snd_idle="sound/dog/idle";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 32);
	self->health=30;
	self->damage=10;
	
	btWalkmonsterStart(self);
}

void monster_demon_init(btEntity self)
{
	self->solidtype=BT_SOLID_SLIDEBOX;
	self->movetype=BT_MOVE_STEP;
	
	btSetModel(self, "models/monsters/demon/demon.model");
	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 32);
	self->health=300;
	self->damage=25;
	
	btWalkmonsterStart(self);
}

void monster_dog_deinit(btEntity self)
	{ }
void monster_dog_null(btEntity self)
	{ }

void monster_dog_blocked(btEntity self, btEntity other)
	{ }
void monster_dog_touch(btEntity self, btEntity other)
	{ }
void monster_dog_use(btEntity self, btEntity other)
	{ }
void monster_dog_pain(btEntity self, btEntity other, float damage)
	{ }
void monster_dog_die(btEntity self, btEntity other, float damage)
{
//	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
	BT_EntityRobotExplode(self);
}

void monster_dog_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

void monster_dog_stand(btEntity self)
{
	btSetMove(self, &monster_dog_stand1_mfi);
}

void monster_dog_walk1_random(btEntity self)
{
	if(btRandom()>0.1)
		btSetMove(self, &monster_dog_walk1_mfi);
}

void monster_dog_walk(btEntity self)
{
	if(btRandom()>0.5)
		btSetMove(self, &monster_dog_walk1_mfi);
	else
		btSetMove(self, &monster_dog_walk2_mfi);
}

void monster_dog_run(btEntity self)
{
	btSetMove(self, &monster_dog_run_mfi);
}

void monster_dog_missile(btEntity self)
{
//	btSetMove(self, &monster_dog_attack1_mfi);
}

void monster_dog_fire(btEntity self)
{
	vec3d org;
	vec3 dir;
	float dist, ft;

	if(!(self->flags&BT_FL_ONGROUND))
		return;

	org=self->origin;
//	dir=btYawVector(btCurrentYaw(self));
	dir=BT_TargetDirection(self, self->enemy);
//	BT_FireRocket(self, org, dir, 10, 600, 25);
//	BT_FireBlaster(self, org, dir, 10, 600, 25);

	dist=BT_TargetDistance(self, self->enemy);
	ft=dist/300;	//flight time

	if(dist<80)return;
	if(dist>160)return;

	self->velocity=v3add(v3scale(dir, 300), vec3(0,0,200));
	self->flags&=~BT_FL_ONGROUND;
}

void monster_dog_melee(btEntity self)
{
	BT_Damage(self->enemy, self, self->damage, BT_MOD_DOGBITE);
//	BT_RadiusDamage(self, self, self->damage, self,
//		120, BT_MOD_MELEE);
	BT_AttackFinished(self, 2.0);
}


BTGE_Entity_Iface monster_dog_vt =
{
monster_dog_init,			monster_dog_deinit,
monster_dog_null,			monster_dog_null,
monster_dog_blocked,		monster_dog_touch,
monster_dog_use,			monster_dog_pain,
monster_dog_die,			monster_dog_cmdmsg,
};

BTGE_Actor_Iface monster_dog_avt =
{
monster_dog_stand,
monster_dog_walk,
monster_dog_run,
monster_dog_missile,
monster_dog_melee,
//NULL,
};

BTGE_Entity_Iface monster_demon_vt =
{
monster_demon_init,			monster_dog_deinit,
monster_dog_null,			monster_dog_null,
monster_dog_blocked,		monster_dog_touch,
monster_dog_use,			monster_dog_pain,
monster_dog_die,			monster_dog_cmdmsg,
};


BTSV_API btEntity BtSp_monster_dog(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_dog_vt,
		&monster_dog_avt);
	return(self);
}

BTSV_API btEntity BtSp_monster_demon1(btSEntity args)
{
	btEntity self;
	
//	return(NULL);
	self=btNewEntityActorInit(args,
		&monster_demon_vt,
		&monster_dog_avt);
	return(self);
}

BTSV_API void BtMt_monster_dog(btItem item)
{
	item->dropWeight=4*35;
}

BTSV_API void BtMt_monster_demon1(btItem item)
{
	item->dropWeight=8;
}
