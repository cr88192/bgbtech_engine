#include <btgesv.h>

BTSV_API BTGE_BrushModel *btGetBModel(btEntity self)
{
	if(!self || !self->sent)
		return(NULL);
//	if(self->bmdl2)
//		return(self->bmdl2);
	return(self->sent->bmdl);
//	return(NULL);
}


void func_doorlike_init(btEntity self)
{
	vec3 v0, v1;
	float ang[4], tv[4];
	float f, g, h;

	self->solidtype=BT_SOLID_BSP;
	self->movetype=BT_MOVE_PUSH;

	printf("func_doorlike_init: spawnflags=%d\n", self->spawnflags);

//	self->eTrig=gcalloc(sizeof(BTGE_EntityTrigger));

//	self->snd_start="sound/doors/dr1_strt";
//	self->snd_stop="sound/doors/dr1_end";

	BT_SetMovedir(self);
	vfvec3(tv, self->movedir);

	if(!btGetBModel(self))
		return;

	BTGE_BrushModel_GetExtents(btGetBModel(self), tv, &f, &g);
	h=(g-f);
	
	if(self->height>0)h=self->height;
//		else self->height=h;
	h-=self->lip;

	if(h<0)h=0;

	self->movedir=vec3vf(tv);
	self->pos1=self->origin;
	self->pos2=v3add(self->origin, v3scale(self->movedir, h));

//	self->origin=self->pos2;
	self->angles=vec3(0,0,0);

	if(self->spawnflags&BT_SPAWNFLAG_DOOR_REVERSE)
	{
		v0=self->pos1;
		v1=self->pos2;
		self->pos1=v1;
		self->pos2=v0;
		self->origin=v1;
	}
	
	if(!self->state)
	{
		//newly spawned
		self->state=BT_STATE_TOP;
		if(self->spawnflags&BT_SPAWNFLAG_DOOR_START_OPEN)
		{
			self->origin=self->pos2;
			self->state=BT_STATE_BOTTOM;
		}
	}else
	{
		//restored from save
		if(self->state==BT_STATE_BOTTOM)
		{
			self->origin=self->pos2;
		}
	}
}

void func_door_linkDoors(btEntity self)
{
	btEntity lst, cur, nxt, head;

	//bad classname
	if(!self->classname)
		return;

	//doesn't link
	if(self->spawnflags&BT_SPAWNFLAG_DOOR_DONT_LINK)
		return;

	//already linked
	if(self->linkFirst)
		return;

	printf("func_door_linkDoors: self=%p\n", self);

	lst=BT_CheckBoxEnts(
		v3sub(self->absmin, vec3(1,1,1)),
		v3add(self->absmax, vec3(1,1,1)),
		BT_TRACE_BSPSOLID, self);

	cur=lst; head=NULL;
	while(cur)
	{
		if(!cur->classname || strcmp(cur->classname, self->classname))
			{ cur=cur->chain; continue; }
		if(cur->spawnflags&BT_SPAWNFLAG_DOOR_DONT_LINK)
			{ cur=cur->chain; continue; }
		nxt=cur->chain;
		cur->chain=head;
		head=cur;
		cur=nxt;
	}
	lst=head;

	if(!lst)return;
	self->chain=lst;
	lst=self;

	cur=lst; head=NULL;
	while(cur)
	{
		if(cur->linkFirst)
			{ head=cur->linkFirst; break; }
		cur=cur->chain;
	}

	if(!head)
	{
		cur=lst;
		while(cur)
		{
			if(cur->targetname)
				{ head=cur; break; }
			cur=cur->chain;
		}
	}

	if(!head)
		head=self;

	cur=lst;
	while(cur)
	{
		if(cur->linkFirst)
			{ cur=cur->chain; continue; }
		
		cur->linkFirst=head;
		if(cur!=head)
		{
			cur->linkNext=head->linkNext;
			head->linkNext=cur;
		}

		func_door_cfgLinkedDoor(cur);
		cur=cur->chain;
	}
}

void func_door_cfgLinkedDoor(btEntity self)
{
	printf("func_door_cfgLinkedDoor: self=%p first=%p\n",
		self, self->linkFirst);

	if(self==self->linkFirst)
	{
		return;
	}
	
	if(self->linkFirst->targetname)
//	if(1)
	{
		if(self->triggerEntity)
		{
			if(!self->triggerEntity->message)
			{
				//disable door firing
				self->triggerEntity->solidtype=BT_SOLID_NOT;
			}
		}
	}
}

void func_door_setLinkDoors(btEntity self)
{
	btEntity cur;

	func_door_linkDoors(self);

#if 0
	if(!self->linkFirst)
	{
		printf("func_door_setLinkDoors: no-op self=%p\n",
			self);
		return;
	}
		
	printf("func_door_setLinkDoors: self=%p first=%p\n",
		self, self->linkFirst);
#endif
}

void func_door_init(btEntity self)
{
	btEntity trg;
	int i;

	if(self->speed==0)self->speed=100;
	if(self->wait==0)self->wait=3;
	if(self->lip==0)self->lip=8;

	if(self->damage==0)
		self->damage=10;

	self->snd_start="sound/doors/dr1_strt";
	self->snd_stop="sound/doors/dr1_end";

	self->vieworg=self->origin;

	if(self->spawnflags&BT_SPAWNFLAG_DOOR_START_OPEN)
	{
		self->spawnflags&=~BT_SPAWNFLAG_DOOR_START_OPEN;
		self->spawnflags|=BT_SPAWNFLAG_DOOR_REVERSE;
	}

	func_doorlike_init(self);

//	func_door_linkDoors(self);

	if(self->health>0)
	{
		self->maxHealth=self->health;
		self->takedamage=BT_DAMAGE_YES;
	}

//	if(self->linkFirst && (self->linkFirst!=self))
//	{
//	}else
	if(!self->targetname && (self->health<=0))
	{
		trg=BT_NewBoxTrigger(self,
			v3sub(self->mins, vec3(32, 32, 0)),
			v3add(self->maxs, vec3(32, 32, 0)));
		self->triggerEntity=trg;

		i=self->spawnflags&
			(BT_SPAWNFLAG_DOOR_GOLD_KEY|
			 BT_SPAWNFLAG_DOOR_SILVER_KEY);
		if(!i)trg->spawnflags|=BT_SPAWNFLAG_TRIGGER_MONSTER;
		trg->spawnflags|=i;
	}else
//	if(self->message && (self->targetname || (self->health>0)))
	if(self->message)
	{
		trg=BT_NewBoxTriggerMsg(self,
			v3sub(self->mins, vec3(32, 32, 0)),
			v3add(self->maxs, vec3(32, 32, 0)),
			self->message);
		self->message=NULL;
		self->triggerEntity=trg;

//		trg->spawnflags|=BT_SPAWNFLAG_TRIGGER_MONSTER;
	}

//	V3F_ADDSCALE(org, tv, h, ent->mdl->org);

	btSetThink(self, func_door_setLinkDoors, 0.1);
}

void func_secret_init(btEntity self)
{
	if(self->spawnflags&BT_SPAWNFLAG_SECRET_OPEN_ONCE)
	{
		self->wait=-1;
	}

	if(self->spawnflags&BT_SPAWNFLAG_SECRET_ALWAYS_SHOOT)
	{
		if(self->health<=0)
			self->health=1;
//		self->health=1;
	}
	
	if(!self->targetname)
	{
		if(self->health<=0)
			self->health=1;
	}

	
	self->spawnflags&=~
		(BT_SPAWNFLAG_SECRET_OPEN_ONCE|
		BT_SPAWNFLAG_SECRET_FIRST_LEFT|
		BT_SPAWNFLAG_SECRET_FIRST_DOWN|
		BT_SPAWNFLAG_SECRET_NOT_SHOOT|
		BT_SPAWNFLAG_SECRET_ALWAYS_SHOOT);
		
	func_door_init(self);
}

void func_button_init(btEntity self)
{
	if(self->speed==0)self->speed=40;
	if(self->wait==0)self->wait=1;
	if(self->lip==0)self->lip=4;
	
	self->snd_start="sound/switches/butn2";
//	self->snd_stop="sound/doors/dr1_end";

//	if(self->targetname)
//		self->targetname=0;

	self->vieworg=self->origin;

	func_doorlike_init(self);

	if(self->health>0)
	{
		self->maxHealth=self->health;
		self->takedamage=BT_DAMAGE_YES;
	}

	if(self->health<=0)
	{
		BT_NewBoxTrigger(self,
			v3sub(self->mins, vec3(16, 16, 16)),
			v3add(self->maxs, vec3(16, 16, 16)));
	}
}

void func_plat_init(btEntity self)
{
	vec3 v0, v1, v2, v3;
	float h;
	
	if(self->speed==0)self->speed=150;
	if(self->wait==0)self->wait=1;
	if(self->lip==0)self->lip=8;

	if(self->damage==0)
		self->damage=10;

	self->angles=vec3(0,0,-2);
	
	self->snd_start="sound/plats/pt1_strt";
	self->snd_stop="sound/plats/pt1_end";

	h=self->height;
	if(h<0)h=0;

	self->vieworg=self->origin;

	func_doorlike_init(self);

	v0=self->pos1;
	v1=self->pos2;
	self->pos2=v0;
	self->pos1=v1;

	if(self->targetname)
	{
		self->state=BT_STATE_BOTTOM;
		self->origin=v0;
	}else
	{
		self->state=BT_STATE_TOP;
		self->origin=v1;
	}

#if 0
//	v2=v3scaleadd2(self->mins, 0.5, self->maxs, 0.5);
//	v2=v3add(v1, v2);
//	v3=vec3(v3x(v2), v3y(v2), v3z(v1)+v3z(self->maxs));
//	h=v3z(v0)-v3z(v1);

//	BT_NewBoxTrigger(self,
//		v3add(v3, vec3(-16, -16, 0)),
//		v3add(v3, vec3(16, 16, h+32)));
#endif

#if 0
	BT_NewBoxTrigger(self,
		v3add(self->mins, vec3(16, 16, 0)),
		v3add(self->maxs, vec3(-16, -16, 32)));
#endif

#if 1
	BT_NewBoxTrigger(self,
		v3add(self->mins, vec3(16, 16, 0-h)),
		v3add(self->maxs, vec3(-16, -16, 32)));
#endif
}

void func_wall_init(btEntity self)
{
	float ang[4], tv[4];
	float f, g, h;

	self->solidtype=BT_SOLID_BSP;
	self->movetype=BT_MOVE_PUSH;

//	self->eTrig=gcalloc(sizeof(BTGE_EntityTrigger));

	self->pos1=self->origin;
	self->pos2=vec3(99999, 99999, 99999);
	self->angles=vec3(0,0,0);

	self->state=BT_STATE_TOP;

//	V3F_ADDSCALE(org, tv, h, ent->mdl->org);
}

void func_wall_use(btEntity self, btEntity other)
{
	vec3 t;

	if(self->state==BT_STATE_TOP)
	{
		self->origin=self->pos2;
		self->state=BT_STATE_BOTTOM;
		return;
	}

	if(self->state==BT_STATE_BOTTOM)
	{
		self->origin=self->pos1;
		self->state=BT_STATE_TOP;
		return;
	}
}

void func_door_movethink(btEntity self)
{
//	printf("func_door_movethink\n");

	if(self->state==BT_STATE_DOWN)
	{
		self->state=BT_STATE_BOTTOM;
//		if(!(self->spawnflags&BT_SPAWNFLAG_DOOR_START_OPEN))
		if(!(self->spawnflags&BT_SPAWNFLAG_DOOR_TOGGLE) &&
				(self->wait>=0))
			btSetThink(self, func_door_waitthink, self->wait);

		btSound(self, BT_CHAN_VOICE, self->snd_stop,
			1.0, BT_ATTN_NORM);
		return;
	}

	if(self->state==BT_STATE_UP)
	{
		self->state=BT_STATE_TOP;
		btSound(self, BT_CHAN_VOICE, self->snd_stop,
			1.0, BT_ATTN_NORM);
		return;
	}
}

void func_door_waitthink(btEntity self)
{
//	printf("func_door_waitthink\n");
	if(self->state==BT_STATE_BOTTOM)
	{
//		self->origin=self->pos1;
//		self->state=BT_STATE_TOP;
		BT_CalcMove(self, self->pos1, self->speed,
			func_door_movethink);
		self->state=BT_STATE_UP;
		self->health=self->maxHealth;
		btSound(self, BT_CHAN_VOICE, self->snd_start,
			1.0, BT_ATTN_NORM);
		return;
	}
}

void func_door_use(btEntity self, btEntity other)
{
	btEntity cur;
	vec3 t;
	int i;

	//avoid rapid refire
	if(!BT_AttackFinishedP(self))
		return;
	BT_AttackFinished(self, 0.1);

	if(!(self->spawnflags&BT_SPAWNFLAG_DOOR_GOLD_KEY))
	{
	}

	if(self->triggerEntity && self->triggerEntity->message)
	{
		//in-use now, disable message trigger
		self->triggerEntity->solidtype=BT_SOLID_NOT;
	}

	if(self->linkFirst)
	{
		
		cur=self->linkFirst; i=16;
		while(cur && ((i--)>0))
		{
			if(cur!=self)
				btEntUse(cur, other);
			cur=self->linkNext;
			if(cur==self->linkFirst)
				break;
		}
		if(i<=0)
			printf("func_door_use: cyclic door link protect\n");
	}

	if((self->state==BT_STATE_TOP) ||
		(self->state==BT_STATE_UP))
	{
		BT_UseTargets(self, other);

//		self->origin=self->pos2;
//		self->state=BT_STATE_BOTTOM;
		BT_CalcMove(self, self->pos2, self->speed,
			func_door_movethink);
		self->state=BT_STATE_DOWN;
		btSound(self, BT_CHAN_VOICE, self->snd_start,
			1.0, BT_ATTN_NORM);
		return;
	}

	if(self->state==BT_STATE_BOTTOM)
	{
		if(self->spawnflags&BT_SPAWNFLAG_DOOR_TOGGLE)
		{
			//set door in motion
			func_door_waitthink(self);
			return;
		}

//		self->origin=self->pos1;
//		self->state=BT_STATE_TOP;

//		if(self->wait>=0)
		if(self->spawnflags&BT_SPAWNFLAG_DOOR_START_OPEN)
		{
			//trigger immediately
			self->spawnflags&=~BT_SPAWNFLAG_DOOR_START_OPEN;
			btSetThink(self, func_door_waitthink, 0.1);
		}else if(self->wait>=0)
		{
			//keep door open
			btSetThink(self, func_door_waitthink, self->wait);
		}

		return;
	}
}

void func_door_blocked(btEntity self, btEntity other)
{
	BT_Damage(other, self, self->damage, BT_MOD_CRUSH);

	if(!(self->spawnflags&BT_SPAWNFLAG_DOOR_CRUSHER))
		func_door_use(self, other);
}

void func_door_die(btEntity self, btEntity other, float damage)
{
	func_door_use(self, other);
}

void func_deinit(btEntity self)
	{ }
void func_null(btEntity self)
	{ }

void func_blocked(btEntity self, btEntity other)
	{ }
void func_touch(btEntity self, btEntity other)
	{ }
void func_use(btEntity self, btEntity other)
	{ }
void func_pain(btEntity self, btEntity other, float damage)
	{ }
void func_die(btEntity self, btEntity other, float damage)
	{ }
void func_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

BTGE_Entity_Iface func_wall_vt =
{
func_wall_init,		func_deinit,
func_null,			func_null,
func_blocked,		func_touch,
func_wall_use,		func_pain,
func_die,			func_cmdmsg,
};

BTGE_Entity_Iface func_door_vt =
{
func_door_init,		func_deinit,
func_null,			func_null,
func_door_blocked,	func_touch,
func_door_use,		func_pain,
func_door_die,		func_cmdmsg,
};

BTGE_Entity_Iface func_secret_vt =
{
func_secret_init,	func_deinit,
func_null,			func_null,
func_blocked,		func_touch,
func_door_use,		func_pain,
func_door_die,		func_cmdmsg,
};

BTGE_Entity_Iface func_button_vt =
{
func_button_init,	func_deinit,
func_null,			func_null,
func_blocked,		func_touch,
func_door_use,		func_pain,
func_door_die,		func_cmdmsg,
};

BTGE_Entity_Iface func_plat_vt =
{
func_plat_init,		func_deinit,
func_null,			func_null,
func_blocked,		func_touch,
func_door_use,		func_pain,
func_die,			func_cmdmsg,
};

BTSV_API btEntity BtSp_func_wall(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_wall_vt);
	return(self);
}

BTSV_API btEntity BtSp_func_nowall(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_wall_vt);
	func_wall_use(self, NULL);
	self->vt->use=func_use;

//	BT_UseTargets(self, self);

//	btSetThink(self, btDeleteEntity, 0.25);

	return(self);
}

BTSV_API btEntity BtSp_func_illusionary(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, NULL);
	return(self);
}

BTSV_API btEntity BtSp_func_bossgate(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_wall_vt);
	return(self);
}

BTSV_API btEntity BtSp_func_episodegate(btSEntity args)
{
	btEntity self;
	
//	self=btNewEntityTriggerInit(args, &func_wall_vt);
	self=BtSp_func_nowall(args);
	return(self);
}

BTSV_API btEntity BtSp_func_onlyregistered(btSEntity args)
{
	btEntity self;
	
//	self=btNewEntityTriggerInit(args, &func_wall_vt);
//	self->origin=self->pos2;
	self=BtSp_func_nowall(args);

	return(self);
}

BTSV_API btEntity BtSp_func_group(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_wall_vt);
	return(self);
}

BTSV_API btEntity BtSp_func_static(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_wall_vt);
	return(self);
}

BTSV_API btEntity BtSp_func_door(btSEntity args)
{
	btEntity self;

	if(!args->bmdl)
		return(NULL);
	
	self=btNewEntityTriggerInit(args, &func_door_vt);
//	self->vt->use(self, NULL);

	return(self);
}

BTSV_API btEntity BtSp_func_door_secret(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_secret_vt);
//	self->vt->use(self, NULL);

	return(self);
}

BTSV_API btEntity BtSp_func_button(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_button_vt);
	return(self);
}

BTSV_API btEntity BtSp_func_plat(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, &func_plat_vt);
	return(self);
}
