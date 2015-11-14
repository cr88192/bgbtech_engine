#include <btgesv.h>

BTGE_ItemInfo *btge_items=NULL;

void item_init(btEntity self)
{
	char *mdl;

	if(!self->item)
		return;

	self->mins=vec3(-16, -16, -16);
	self->maxs=vec3( 16,  16,  16);
	self->origin=v3add(self->origin, vec3(0, 0, 24));

	self->solidtype=BT_SOLID_TRIGGER;
	self->movetype=BT_MOVE_TOSS;
	
	self->effects=self->item->effects;
	
	mdl=self->item->worldModel;
	if(!mdl)mdl="objects/monitor31";
	btSetModel(self, mdl);
}

int item_generic_canPickup(btEntity self, btEntity item)
{
	if(!self || !item)
		return(false);
		
	if(!(self->flags&BT_FL_CLIENT))
		return(false);
	return(true);
}

void item_deinit(btEntity self)
	{ }
void item_null(btEntity self)
	{ }
void item_blocked(btEntity self, btEntity other)
	{ }

void item_respawn(btEntity self)
{
	char *mdl;

	self->solidtype=BT_SOLID_TRIGGER;
	self->movetype=BT_MOVE_TOSS;
	
	self->effects=self->item->effects;
	
	mdl=self->item->worldModel;
	if(!mdl)mdl="objects/monitor31";
	btSetModel(self, mdl);

	BT_EntityRespawnEffect(self);
}

void item_touch(btEntity self, btEntity other)
{
	dyt v;

	if(self->item->dy_this)
	{
		v=dyCall2(self->item->dy_this, "canPickup",
			(dyt)self, (dyt)self->item);
		if(dyintp(v) || dyboolp(v))
		{
			if(!dytruep(v))
				return;
		}
	}

	if(self->item->canPickup)
	{
		if(!self->item->canPickup(other, self))
			return;
	}else
	{
		if(!item_generic_canPickup(other, self))
			return;
	}

	btSound(other, BT_CHAN_ITEM, self->item->touchSound,
		1.0, BT_ATTN_NORM);

	if(self->item->touchThink)
		self->item->touchThink(other, self->item);

	if(self->item->dy_this)
		dyCall2(self->item->dy_this, "touchThink",
			(dyt)self, (dyt)self->item);

	BT_UseTargets(self, other);

	if(btGameIsDeathmatch() || btGameIsCoop())
	{
		self->solidtype=BT_SOLID_NOT;
		self->movetype=BT_MOVE_NONE;
		btSetModel(self, NULL);

		btSetThink(self, item_respawn, 30);
	}else
	{
		btDeleteEntity(self);
	}
}

void item_use(btEntity self, btEntity other)
{
}

void item_pain(btEntity self, btEntity other, float damage)
	{ }
void item_die(btEntity self, btEntity other, float damage)
	{ }
void item_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

BTGE_Entity_Iface item_vt =
{
item_init,			item_deinit,
item_null,			item_null,
item_blocked,		item_touch,
item_use,			item_pain,
item_die,			item_cmdmsg,
};

btItem btLookupItem(char *classname)
{
	btItem cur;
	
	cur=btge_items;
	while(cur)
	{
		if(!strcmp(cur->classname, classname))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

btItem btGetItem(char *classname)
{
	btItem tmp;
	int i;

	tmp=btLookupItem(classname);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btge_iteminfo_t", sizeof(BTGE_ItemInfo));
	tmp->classname=dystrdup(classname);
	i=BT_InitItem(tmp);
	if(i<0)
	{
		gcfree(tmp);
		return(NULL);
	}
	
	tmp->next=btge_items;
	btge_items=tmp;
	return(tmp);
}

btEntity btSpawnItem(btSEntity args)
{
	btItem item;
	btEntity ent;
	char *s;

	s=BTGE_SEnt_GetStr(args, "classname");
	if(!s)return(NULL);

	item=btGetItem(s);
	if(!item)return(NULL);
	ent=btNewEntityTriggerInit(args, &item_vt);
	ent->item=item;
	
	ent->vt->init(ent);	
	return(ent);
}

int BT_InitItem(btItem item)
{
	char tb[256];
	void *ptr;
	char *s;
	dyt v;

	if(!item)return(-1);
	if(!item->classname)return(-1);
	
//	return(-1);
	
	sprintf(tb, "BtIt_%s", item->classname);
	ptr=dyllGetAddr(tb);

	if(ptr)
	{
		((void (*)(btItem))ptr)(item);
		return(0);
	}

	v=bsvmCallName1(tb, (dyt)item);
	if(v!=UNDEFINED)
		return(0);

//	btDPrint("BT_InitItem: No itemfunc for '%s'\n", s);
	return(-1);
}

BTSV_API void btEntityUseItem(btEntity self, btItem item)
{
	if(!self || !item)
		return;
	if(item->use)
		item->use(self, item);
	if(item->dy_this)
		dyCall2(item->dy_this, "use", (dyt)self, (dyt)item);
}

int item_health_canPickup(btEntity self, btEntity item)
{
	if(!self || !item)
		return(false);
		
	if(!(self->flags&BT_FL_CLIENT))
		return(false);
	if(self->health>=self->maxHealth)
		return(false);
	return(true);
}

int item_ammo_canPickup(btEntity self, btEntity item)
{
	if(!self || !item)
		return(false);
		
	if(!(self->flags&BT_FL_CLIENT))
		return(false);
	if(self->curMP>=self->maxMP)
		return(false);
	return(true);
}

void health_touchThink(btEntity self, btItem item)
{
	self->health+=50;
	self->itemflags|=item->flags;
}

void ammo_touchThink(btEntity self, btItem item)
{
	self->curMP+=50;
	self->itemflags|=item->flags;
}

void item_touchThink(btEntity self, btItem item)
{
//	self->curMP+=50;
	self->itemflags|=item->flags;
}

int item_weapon_canPickup(btEntity self, btEntity item)
{
	if(!self || !item)
		return(false);
		
	if(!(self->flags&BT_FL_CLIENT))
		return(false);

	if(item->flags)
	{
		//if item gives something which self lacks, allow pickup
		if((item->flags&self->itemflags)!=item->flags)
			return(true);
	}

	if(self->curMP>=self->maxMP)
		return(false);
	return(true);
}

void weapon_touchThink(btEntity self, btItem item)
{
	int i;
	
	for(i=0; i<10; i++)
		if((item->flags&(1<<i)) && !(self->itemflags&(1<<i)))
	{
		self->itemflags|=item->flags;
		BT_PlayerWeaponChange(self, i+1);
	}

	self->itemflags|=item->flags;
	self->curMP+=50;
}

int item_armor_canPickup(btEntity self, btEntity item)
{
	if(!self || !item)
		return(false);
		
	if(!(self->flags&BT_FL_CLIENT))
		return(false);
	if(self->curMP>=self->maxMP)
		return(false);
	return(true);
}

void armor_touchThink(btEntity self, btItem item)
{
	self->curMP+=50;
}


BTSV_API void BtIt_item_health(btItem item)
{
	item->worldModel="items/healing/large/large0";
	item->touchSound="sound/items/l_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
}

BTSV_API void BtIt_item_health1(btItem item)
{
	item->worldModel="items/healing/stimpack/stimpack0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
}

BTSV_API void BtIt_item_health25(btItem item)
{
	item->worldModel="items/healing/medium/medium0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
}

BTSV_API void BtIt_item_health_small(btItem item)
{
	item->worldModel="items/healing/stimpack/stimpack0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
}

BTSV_API void BtIt_item_health_medium(btItem item)
{
	item->worldModel="items/healing/medium/medium0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
}

BTSV_API void BtIt_item_health_large(btItem item)
{
	item->worldModel="items/healing/large/large0";
	item->touchSound="sound/items/l_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
}

BTSV_API void BtIt_item_key2(btItem item)
{
	item->worldModel="items/keys/red_key/red_key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY1;
}

BTSV_API void BtIt_item_key1(btItem item)
{
	item->worldModel="items/keys/key/key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY2;
}

BTSV_API void BtIt_item_flag_team1(btItem item)
{
	item->worldModel="items/keys/red_key/red_key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY3;
}

BTSV_API void BtIt_item_flag_team2(btItem item)
{
	item->worldModel="items/keys/key/key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY4;
}

BTSV_API void BtIt_item_shells(btItem item)
{
	item->worldModel="items/ammo/shells/shells0";
	item->touchSound="sound/misc/am_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
}

BTSV_API void BtIt_item_rockets(btItem item)
{
	item->worldModel="items/ammo/rockets/rockets0";
	item->touchSound="sound/misc/am_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
}

BTSV_API void BtIt_item_spikes(btItem item)
{
	item->worldModel="items/ammo/bullets/bullets0";
	item->touchSound="sound/misc/am_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
}

BTSV_API void BtIt_item_cells(btItem item)
{
	item->worldModel="items/ammo/bullets/bullets0";
	item->touchSound="sound/misc/am_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
}

BTSV_API void BtIt_item_bullets(btItem item)
{
	item->worldModel="items/ammo/bullets/bullets0";
	item->touchSound="sound/misc/am_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
}

BTSV_API void BtIt_ammo_bullets(btItem item)
	{ BtIt_item_bullets(item); }
BTSV_API void BtIt_ammo_shells(btItem item)
	{ BtIt_item_bullets(item); }
BTSV_API void BtIt_ammo_grenades(btItem item)
	{ BtIt_item_rockets(item); }
BTSV_API void BtIt_ammo_rockets(btItem item)
	{ BtIt_item_rockets(item); }
BTSV_API void BtIt_ammo_slugs(btItem item)
	{ BtIt_item_bullets(item); }
BTSV_API void BtIt_ammo_cells(btItem item)
	{ BtIt_item_cells(item); }
BTSV_API void BtIt_ammo_bfg(btItem item)
	{ BtIt_item_cells(item); }


BTSV_API void BtIt_weapon_supershotgun(btItem item)
{
	item->worldModel="weapons/g_shotg/g_shotg0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_SHOTGUN;
}

BTSV_API void BtIt_weapon_grenadelauncher(btItem item)
{
	item->worldModel="weapons/g_launch/g_launch0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_LASERGUN;
}

BTSV_API void BtIt_weapon_rocketlauncher(btItem item)
{
	item->worldModel="weapons/g_rocket/g_rocket0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_ROCKETLAUNCHER;
}

BTSV_API void BtIt_weapon_nailgun(btItem item)
{
	item->worldModel="weapons/g_chain/g_chain1";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_CHAINGUN;
}

BTSV_API void BtIt_weapon_supernailgun(btItem item)
{
	item->worldModel="weapons/g_machn/g_machn0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_PLASMAGUN;
}

BTSV_API void BtIt_weapon_lightning(btItem item)
{
	item->worldModel="weapons/g_hyperb/g_hyperb1";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_PLASMACANNON;
}

BTSV_API void BtIt_weapon_uzi(btItem item)
{
	item->worldModel="weapons/g_machn/g_machn0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_CHAINGUN;
}

BTSV_API void BtIt_weapon_electro(btItem item)
{
	item->worldModel="weapons/g_hyperb/g_hyperb1";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_LASERGUN;
}

BTSV_API void BtIt_item_armor1(btItem item)
{
	item->worldModel="items/armor/body/body0";
	item->touchSound="sound/misc/ar1_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_armor2(btItem item)
{
	item->worldModel="items/armor/combat/combat2";
	item->touchSound="sound/misc/ar1_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_armor_small(btItem item)
{
	item->worldModel="items/armor/shard/shard0";
	item->touchSound="sound/misc/ar1_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_armor_large(btItem item)
{
	item->worldModel="items/armor/body/body0";
	item->touchSound="sound/misc/ar1_pkup";
	item->touchThink=ammo_touchThink;
	item->canPickup=item_ammo_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_artifact_super_damage(btItem item)
{
	item->worldModel="items/quaddama/quaddama0";
	item->touchSound="sound/items/damage";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_generic_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_artifact_invulnerability(btItem item)
{
	item->worldModel="items/invulner/invulner1";
	item->touchSound="sound/items/protect";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_strength(btItem item)
{
	item->worldModel="items/quaddama/quaddama0";
	item->touchSound="sound/items/damage";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_invincible(btItem item)
{
	item->worldModel="items/invulner/invulner1";
	item->touchSound="sound/items/protect";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_artifact_envirosuit(btItem item)
{
	item->worldModel="items/enviro/enviro0";
	item->touchSound="sound/items/s_health";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
}
