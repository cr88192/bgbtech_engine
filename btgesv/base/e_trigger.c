#include <btgesv.h>

//remove phantom walls
//basically spawns the bmodel somewhere else to move it
void trigger_wall_remover(btEntity self)
{
	btEntity tmp;

	tmp=btNewEntityTriggerInit(NULL, NULL);
	tmp->classname="wall_remover";
	tmp->origin=vec3d(999999, 999999, 999999);
	tmp->angles=vec3(0,0,0);
	tmp->model=self->model;
	self->model=NULL;

	btSetThink(tmp, btDeleteEntity, 0.25);
}

void trigger_init(btEntity self)
{
	char *s0, *s1, *s2;

	self->solidtype=BT_SOLID_TRIGGER;
	BT_SetMovedir(self);

	if(self->health>0)
	{
		self->solidtype=BT_SOLID_BBOX;
		self->maxHealth=self->health;
		self->takedamage=BT_DAMAGE_YES;
	}
	
	if(self->spawnflags&BT_SPAWNFLAG_TRIGGER_ENABLE)
	{
		self->solidtype=BT_SOLID_NOT;
		self->takedamage=BT_DAMAGE_NO;
		return;
	}
	
	if(self->classname && !strcmp(self->classname, "trigger_relay_local"))
	{
		s0=BTGE_SEnt_GetStr(self->sent, "var_name");
		s1=BTGE_SEnt_GetStr(self->sent, "var_value");
		if(s0 && s1)
		{
			s2=btLocalVarGet(s0);
			if(s2 && !strcmp(s2, s1))
				{ BT_DelayUseTargets(self, self, 0.1); }
		}
	}

	if(self->classname && !strcmp(self->classname, "trigger_relay_global"))
	{
		s0=BTGE_SEnt_GetStr(self->sent, "var_name");
		s1=BTGE_SEnt_GetStr(self->sent, "var_value");
		if(s0 && s1)
		{
			s2=btGlobalVarGet(s0);
			if(s2 && !strcmp(s2, s1))
				{ BT_DelayUseTargets(self, self, 0.1); }
		}
	}
	
	trigger_wall_remover(self);
}

void trigger_deinit(btEntity self)
	{ }
void trigger_null(btEntity self)
	{ }

void trigger_blocked(btEntity self, btEntity other)
	{ }

void trigger_touch(btEntity self, btEntity other)
{
	if(!other)
		return;

	//some types may not fire triggers
	if(other->solidtype==BT_SOLID_NOT)
		return;
	if(other->solidtype==BT_SOLID_TRIGGER)
		return;
	if(other->solidtype==BT_SOLID_BSP)
		return;

	//not yet activated
	if(self->spawnflags&BT_SPAWNFLAG_TRIGGER_TRIGGERED)
		return;

	//only monsters or players work
	if(!(other->flags&(BT_FL_CLIENT|BT_FL_MONSTER)))
		return;
	
	if(self->spawnflags&BT_SPAWNFLAG_TRIGGER_NO_CLIENT)
		if(other->flags&BT_FL_CLIENT)
			return;
	if(other->flags&BT_FL_MONSTER)
		if(!(self->spawnflags&BT_SPAWNFLAG_TRIGGER_MONSTER))
			return;

	//do nothing if too soon
	if(!BT_AttackFinishedP(self))
		return;

	if(self->spawnflags&BT_SPAWNFLAG_DOOR_GOLD_KEY)
		if((other->flags&BT_FL_CLIENT) &&
			!(other->itemflags&BT_ITFL_GOLDKEY))
	{
		btCenterprint(other, "You need the red key\n");
		btSound(other, BT_CHAN_VOICE, "sound/misc/talk1",
			1.0, BT_ATTN_NORM);
		BT_AttackFinished(self, 1.0);
		return;
	}
	if(self->spawnflags&BT_SPAWNFLAG_DOOR_SILVER_KEY)
		if((other->flags&BT_FL_CLIENT) &&
			!(other->itemflags&BT_ITFL_SILVERKEY))
	{
		btCenterprint(other, "You need the blue key\n");
		btSound(other, BT_CHAN_VOICE, "sound/misc/talk1",
			1.0, BT_ATTN_NORM);
		BT_AttackFinished(self, 1.0);
		return;
	}

	trigger_use(self, other);
}

void trigger_use(btEntity self, btEntity other)
{
	char *s0, *s1;

	//using clears triggered flag
	self->spawnflags&=~BT_SPAWNFLAG_TRIGGER_TRIGGERED;

	//triggers which are triggered to enable
	if(self->solidtype==BT_SOLID_NOT)
	{
		self->solidtype=BT_SOLID_TRIGGER;
		if(self->health>0)
		{
			self->solidtype=BT_SOLID_BBOX;
			self->takedamage=BT_DAMAGE_YES;
		}
		return;
	}

#if 0
	if(!(other->flags&(BT_FL_CLIENT|BT_FL_MONSTER)))
		return;
	
	if(self->spawnflags&BT_SPAWNFLAG_TRIGGER_NO_CLIENT)
		if(other->flags&BT_FL_CLIENT)
			return;
	if(other->flags&BT_FL_MONSTER)
		if(!(self->spawnflags&BT_SPAWNFLAG_TRIGGER_MONSTER))
			return;
#endif

	//do nothing if too soon
	if(!BT_AttackFinishedP(self))
		return;

	if(self->count>0)
	{
		self->count--;
		if(self->count>0)
		{
			if(!(self->spawnflags&BT_SPAWNFLAG_TRIGGER_SILENT))
			{
				btCenterprint(other, "%d more to go\n",
					self->count);
				btSound(other, BT_CHAN_VOICE, "sound/misc/talk1",
					1.0, BT_ATTN_NORM);
			}
			return;
		}
		if(!(self->spawnflags&BT_SPAWNFLAG_TRIGGER_SILENT))
		{
			btCenterprint(other, "sequence completed\n");
			btSound(other, BT_CHAN_VOICE, "sound/misc/talk1",
				1.0, BT_ATTN_NORM);
		}
	}

	//if we have an owner, trigger them
	if(self->owner)
	{
		//can't be triggered by owner though
		if(other==self->owner)
			return;

		//if message is set, then display the message
		if(self->message)
		{
			btCenterprint(other, self->message);
			btSound(other, BT_CHAN_VOICE, "sound/misc/talk1",
				1.0, BT_ATTN_NORM);
//			return;
		}
		else
//		if(1)
		{
			//use owner
			btEntUse(self->owner, other);
//			return;
		}
	}

	if(self->classname && !strcmp(self->classname, "trigger_relay_local"))
	{
		s0=BTGE_SEnt_GetStr(self->sent, "var_name");
		s1=BTGE_SEnt_GetStr(self->sent, "var_value");
		if(s0 && s1)
			btLocalVarSet(s0, s1);
	}

	if(self->classname && !strcmp(self->classname, "trigger_relay_global"))
	{
		s0=BTGE_SEnt_GetStr(self->sent, "var_name");
		s1=BTGE_SEnt_GetStr(self->sent, "var_value");
		if(s0 && s1)
			btGlobalVarSet(s0, s1);
	}

	BT_UseTargets(self, other);
	if(self->wait<0)
	{
		btDeleteEntity(self);
		return;
	}
	
	//avoid rapid refire
	BT_AttackFinished(self, self->wait);
}

void trigger_pain(btEntity self, btEntity other, float damage)
	{ }
void trigger_die(btEntity self, btEntity other, float damage)
	{ }
void trigger_cmdmsg(btEntity self, btEntity other, char *str)
	{ }


BTGE_Entity_Iface trigger_multiple_vt =
{
trigger_init,			trigger_deinit,
trigger_null,			trigger_null,
trigger_blocked,		trigger_touch,
trigger_use,			trigger_pain,
trigger_die,			trigger_cmdmsg,
};

BTSV_API btEntity BtSp_trigger_once(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
	self->wait=-1;
	return(self);
}

BTSV_API btEntity BtSp_trigger_multiple(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
	if(self->wait==0)
		self->wait=1;
	return(self);
}

BTSV_API btEntity BtSp_trigger_secret(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
	self->wait=-1;
	if(!self->message)
		self->message="You found a secret";
	return(self);
}


BTSV_API btEntity BT_NewBoxTrigger(btEntity owner, vec3 mins, vec3 maxs)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(NULL, &trigger_multiple_vt);
	self->wait=1;
	self->owner=owner;
	self->origin=v3f2d(owner->vieworg);
	self->mins=mins;
	self->maxs=maxs;
	return(self);
}

BTSV_API btEntity BT_NewBoxTriggerMsg(
	btEntity owner, vec3 mins, vec3 maxs, char *msg)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(NULL, &trigger_multiple_vt);
	self->wait=1;
	self->owner=owner;
	self->origin=v3f2d(owner->vieworg);
	self->mins=mins;
	self->maxs=maxs;
	self->message=msg;
	return(self);
}



BTSV_API btEntity BtSp_trigger_relay(btSEntity args)
{
	btEntity self;	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
	return(self);
}

BTSV_API btEntity BtSp_trigger_relay_local(btSEntity args)
{
	btEntity self;	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
	return(self);
}

BTSV_API btEntity BtSp_trigger_relay_global(btSEntity args)
{
	btEntity self;	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
	return(self);
}

BTSV_API btEntity BtSp_trigger_counter(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
//	if(self->wait==0)
//		self->wait=1;
	self->wait=-1;
	if(self->count==0)
		self->count=2;
	return(self);
}

BTSV_API btEntity BtSp_trigger_always(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_multiple_vt);
	if(self->delay==0)
		self->delay=0.2;
	BT_UseTargets(self, self);
	return(self);
}

BTSV_API btEntity BtSp_trigger_onlyregistered(btSEntity args)
{
	btEntity self;

	self=BtSp_trigger_multiple(args);
	self->message=NULL;
	return(self);
}

void changelevel_touch(btEntity self, btEntity other)
{
	if(other->solidtype!=BT_SOLID_SLIDEBOX)
		return;

	if(btGameIsDeathmatch())
	{
		BT_Damage(other, self, 9999, BT_MOD_CHANGELEVEL);
		return;
	}

	changelevel_use(self, other);
}

void changelevel_use(btEntity self, btEntity other)
{
	btEntity ent;
	int i, n;

	ent=NULL;
	if(self->landmark)
	{
		ent=btFindTargets(self->landmark);
		if(!ent)
		{
			printf("changelevel_use: no landmark %s\n",
				self->landmark);
		}
	}

	if(ent)
	{
		other->landmark=self->landmark;
		other->movedir=v3d2f(v3dsub(other->origin, ent->origin));
	}

	other->itemflags&=~(BT_ITFL_GOLDKEY|BT_ITFL_SILVERKEY);

	BT_PreserveClientState(other);

	btDPrint("changelevel_use: %s\n", self->map);
	btConsoleCmd("map \"%s\"\n", self->map);
}

BTGE_Entity_Iface trigger_changelevel_vt =
{
trigger_init,			trigger_deinit,
trigger_null,			trigger_null,
trigger_blocked,		changelevel_touch,
changelevel_use,		trigger_pain,
trigger_die,			trigger_cmdmsg,
};

BTSV_API btEntity BtSp_trigger_changelevel(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_changelevel_vt);
	self->solidtype=BT_SOLID_TRIGGER;
	return(self);
}

BTSV_API btEntity BtSp_target_changelevel(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_changelevel_vt);
//	self->solidtype=BT_SOLID_TRIGGER;
	return(self);
}


void teleport_init(btEntity self)
{
	if(!strcmp(self->classname, "target_teleport"))
	{
		return;
	}

	self->solidtype=BT_SOLID_TRIGGER;
	BT_SetMovedir(self);

	if(self->health>0)
	{
		self->solidtype=BT_SOLID_BBOX;
		self->maxHealth=self->health;
		self->takedamage=BT_DAMAGE_YES;
	}
	
//	if(self->spawnflags&BT_SPAWNFLAG_TRIGGER_ENABLE)
	if(self->targetname)
	{
		self->solidtype=BT_SOLID_NOT;
		self->takedamage=BT_DAMAGE_NO;
		return;
	}
}

void teleport_touch(btEntity self, btEntity other)
{
	if(!other)
		return;
	if(other->solidtype!=BT_SOLID_SLIDEBOX)
		return;
	if(self->spawnflags&BT_SPAWNFLAG_TELEPORT_ONLYCLIENT)
		if(!(other->flags&BT_FL_CLIENT))
			return;
//	teleport_use(self, other);

	teleport_useGo(self, other);
}

//invoked when we want to teleport self to other
void BT_TeleportToTarget(btEntity self, btEntity other, int flags)
{
	vec3d sorg, eorg, dorg, mid;

	if(!self || !other)
		return;

	dorg=BT_FindEntityTeleportSpot(self, other->origin);

	if(!(flags&BT_SPAWNFLAG_TELEPORT_SILENT))
	{
		mid=v3f2d(v3scale(v3add(self->mins, self->maxs), 0.5));
		sorg=v3dadd(self->origin, mid);
		eorg=v3dadd(dorg, mid);

		btParticleTeleportExplosion(sorg);
		btParticleTeleportExplosion(eorg);

		btSound(other, BT_CHAN_VOICE, "sound/misc/tele1",
			1.0, BT_ATTN_NORM);
	}

	self->origin=dorg;
	self->angles=other->angles;
	self->flags&=~BT_FL_ONGROUND;
	if(self->flags&BT_FL_CLIENT)
		self->pauseTime=btDelayTime(0.1);
}

//invoked when we want to teleport other
void teleport_useGo(btEntity self, btEntity other)
{
	vec3d sorg, eorg, dorg, mid;
	btEntity lst, cur;
	int i, n;

	if(!other)return;

	lst=btFindTargets(self->target);
	if(!lst)return;
	
	cur=lst; n=0;
	while(cur) { n++; cur=cur->chain; }
	i=btRandom()*n;
	cur=lst;
	while(cur->chain && ((i--)>0))
		cur=cur->chain;

	if(!cur)return;

	dorg=BT_FindEntityTeleportSpot(other, cur->origin);

	if(!(self->spawnflags&BT_SPAWNFLAG_TELEPORT_SILENT))
	{
//		btParticleTeleportExplosion(other->origin);
//		btParticleTeleportExplosion(cur->origin);

		mid=v3f2d(v3scale(v3add(other->mins, other->maxs), 0.5));
		sorg=v3dadd(other->origin, mid);
//		eorg=v3dadd(cur->origin, mid);
		eorg=v3dadd(dorg, mid);

		btParticleTeleportExplosion(sorg);
		btParticleTeleportExplosion(eorg);

		btSound(cur, BT_CHAN_VOICE, "sound/misc/tele1",
			1.0, BT_ATTN_NORM);
	}

//	other->origin=v3add(cur->origin, vec3(0,0,48));

//	mid=v3scale(v3add(other->mins, other->maxs), 0.5);
//	eorg=v3sub(cur->origin, mid);
//	eorg=v3sub(cur->origin, vec3(0,0,v2z(other->mins)));
		
//	other->origin=cur->origin;
	other->origin=dorg;
	other->angles=cur->angles;

	other->flags&=~BT_FL_ONGROUND;

	if(other->flags&BT_FL_CLIENT)
	{
		//try to force player to look the correct direction
		other->effects2|=BT_EF2_CAM_FORCEANGLE;
		other->pauseTime=btDelayTime(0.5);
	}
}

void teleport_use(btEntity self, btEntity other)
{
	vec3 sorg, eorg, mid;
	btEntity lst, cur;
	int i, n;

	//triggers which are triggered to enable
	if(!strcmp(self->classname, "trigger_teleport"))
	{
		if(self->solidtype==BT_SOLID_NOT)
		{
			self->solidtype=BT_SOLID_TRIGGER;
			if(self->health>0)
			{
				self->solidtype=BT_SOLID_BBOX;
				self->takedamage=BT_DAMAGE_YES;
			}
			return;
		}
	}

	if(!strcmp(self->classname, "target_teleport"))
	{
		teleport_useGo(self, other);
	}

//	teleport_useGo(self, other);
}

BTGE_Entity_Iface trigger_teleport_vt =
{
teleport_init,			trigger_deinit,
trigger_null,			trigger_null,
trigger_blocked,		teleport_touch,
teleport_use,			trigger_pain,
trigger_die,			trigger_cmdmsg,
};

BTSV_API btEntity BtSp_trigger_teleport(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_teleport_vt);
	return(self);
}

BTSV_API btEntity BtSp_target_teleport(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_teleport_vt);
	return(self);
}


void push_init(btEntity self)
{
	self->solidtype=BT_SOLID_TRIGGER;
	BT_SetMovedir(self);

	if(self->health>0)
	{
		self->solidtype=BT_SOLID_BBOX;
		self->maxHealth=self->health;
		self->takedamage=BT_DAMAGE_YES;
	}
	
	if(self->targetname)
	{
		self->solidtype=BT_SOLID_NOT;
		self->takedamage=BT_DAMAGE_NO;
		return;
	}
}

void push_touch(btEntity self, btEntity other)
{
	if(!other)
		return;
	if(other->solidtype!=BT_SOLID_SLIDEBOX)
		return;
//	if(self->spawnflags&BT_SPAWNFLAG_TELEPORT_ONLYCLIENT)
//		if(!(other->flags&BT_FL_CLIENT))
//			return;
//	teleport_use(self, other);

	if(other->health>0)
	{
//		BT_PushEntity(other, v3scale(self->movedir, self->speed*10));
		other->velocity=v3scale(self->movedir, self->speed*10);

		if(self->spawnflags&BT_SPAWNFLAG_PUSH_ONCE)
			btDeleteEntity(self);
	}
}

void push_use(btEntity self, btEntity other)
{
	if(self->solidtype==BT_SOLID_NOT)
	{
		//activate trigger
		self->solidtype=BT_SOLID_TRIGGER;
//		if(self->health>0)
//			self->solidtype=BT_SOLID_BBOX;
	}
}

BTGE_Entity_Iface trigger_push_vt =
{
push_init,				trigger_deinit,
trigger_null,			trigger_null,
trigger_blocked,		push_touch,
push_use,				trigger_pain,
trigger_die,			trigger_cmdmsg,
};

BTSV_API btEntity BtSp_trigger_push(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_push_vt);
	return(self);
}

void hurt_init(btEntity self)
{
	if(self->damage==0)
		self->damage=10;

	self->solidtype=BT_SOLID_TRIGGER;
	BT_SetMovedir(self);
	if(self->wait==0)
		self->wait=1;	//default refire=1s

	if(self->targetname)
	{
		self->solidtype=BT_SOLID_NOT;
		self->takedamage=BT_DAMAGE_NO;
		return;
	}
}

void heal_init(btEntity self)
{
	if(self->damage==0)
		self->damage=10;
	self->damage=-self->damage;
	hurt_init(self);
}

void hurt_touch(btEntity self, btEntity other)
{
	if(!BT_AttackFinishedP(self))
		return;

	if(!other)
		return;
	if(other->solidtype!=BT_SOLID_SLIDEBOX)
		return;
//	if(self->spawnflags&BT_SPAWNFLAG_TELEPORT_ONLYCLIENT)
//		if(!(other->flags&BT_FL_CLIENT))
//			return;
//	teleport_use(self, other);

	if(other->health>0)
	{
		BT_Damage(other, self, self->damage, BT_MOD_HURT);

//		if(self->spawnflags&BT_SPAWNFLAG_PUSH_ONCE)
//			btDeleteEntity(self);
	
		BT_AttackFinished(self, self->wait);
	}
}

void hurt_use(btEntity self, btEntity other)
{
	if(self->solidtype==BT_SOLID_NOT)
	{
		//activate trigger
		self->solidtype=BT_SOLID_TRIGGER;
	}
}

BTGE_Entity_Iface trigger_hurt_vt =
{
hurt_init,				trigger_deinit,
trigger_null,			trigger_null,
trigger_blocked,		hurt_touch,
hurt_use,				trigger_pain,
trigger_die,			trigger_cmdmsg,
};

BTGE_Entity_Iface trigger_heal_vt =
{
heal_init,				trigger_deinit,
trigger_null,			trigger_null,
trigger_blocked,		hurt_touch,
hurt_use,				trigger_pain,
trigger_die,			trigger_cmdmsg,
};

BTSV_API btEntity BtSp_trigger_hurt(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_hurt_vt);
	return(self);
}

BTSV_API btEntity BtSp_trigger_heal(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_heal_vt);
	return(self);
}



void impulse_init(btEntity self)
{
	self->solidtype=BT_SOLID_TRIGGER;
	BT_SetMovedir(self);

	if(self->health>0)
	{
		self->solidtype=BT_SOLID_BBOX;
		self->maxHealth=self->health;
		self->takedamage=BT_DAMAGE_YES;
	}
	
	if(self->targetname)
	{
		self->solidtype=BT_SOLID_NOT;
		self->takedamage=BT_DAMAGE_NO;
		return;
	}
}

vec3 impulse_touch_dir(btEntity self, btEntity other)
{
	vec3 dir;
	btEntity lst, cur;
	int i, n;

	if(!self->target)
		return(self->movedir);

	lst=btFindTargets(self->target);
	if(!lst)return(self->movedir);
	
	cur=lst; n=0;
	while(cur) { n++; cur=cur->chain; }
	i=btRandom()*n;
	cur=lst;
	while(cur->chain && ((i--)>0))
		cur=cur->chain;

	if(!cur)return(self->movedir);
	
	dir=v3d2f(v3dsub(cur->origin, other->origin));
	return(v3norm(dir));
}

void impulse_touch(btEntity self, btEntity other)
{
	vec3 dir;
	btEntity lst, cur;
	int i, n;

	if(!other)
		return;
	if(other->solidtype!=BT_SOLID_SLIDEBOX)
		return;

	if(other->health<=0)
		return;

//	if(self->spawnflags&BT_SPAWNFLAG_TELEPORT_ONLYCLIENT)
//		if(!(other->flags&BT_FL_CLIENT))
//			return;
//	teleport_use(self, other);

	if(self->target)
	{
		dir=impulse_touch_dir(self, other);
		other->velocity=v3add(other->velocity,
			v3scale(dir, self->strength*btStepTime()));
	}else
	{
		other->velocity=v3scale(other->velocity, self->strength);
	}
}

void impulse_use(btEntity self, btEntity other)
{
	if(self->solidtype==BT_SOLID_NOT)
	{
		//activate trigger
		self->solidtype=BT_SOLID_TRIGGER;
//		if(self->health>0)
//			self->solidtype=BT_SOLID_BBOX;
	}
}

BTGE_Entity_Iface trigger_impulse_vt =
{
impulse_init,			trigger_deinit,
trigger_null,			trigger_null,
trigger_blocked,		impulse_touch,
impulse_use,			trigger_pain,
trigger_die,			trigger_cmdmsg,
};

BTSV_API btEntity BtSp_trigger_impulse(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &trigger_impulse_vt);
	return(self);
}
