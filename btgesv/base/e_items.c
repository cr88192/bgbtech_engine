#include <btgesv.h>

BTGE_ItemInfo *btge_items=NULL;
BTGE_ItemInfo *btge_monsters=NULL;

static char *btge_items_preload[]={
"item_health", 
"item_health1", 
"item_health25", 
"item_health_small", 
"item_health_medium", 
"item_health_large", 
"item_key2", 
"item_key1", 
"item_flag_team1", 
"item_flag_team2", 
"item_shells", 
"item_rockets", 
"item_spikes", 
"item_cells", 
"item_bullets", 
"ammo_bullets", 
"ammo_shells", 
"ammo_grenades", 
"ammo_rockets", 
"ammo_slugs", 
"ammo_cells", 
"ammo_bfg", 
"weapon_supershotgun", 
"weapon_grenadelauncher", 
"weapon_rocketlauncher", 
"weapon_rocketshovel", 
"weapon_nailgun", 
"weapon_supernailgun", 
"weapon_lightning", 
"item_armor1", 
"item_armor2", 
"item_armor_small", 
"item_armor_large", 
"item_artifact_super_damage", 
"item_artifact_invulnerability", 
"item_strength", 
"item_invincible", 
"item_artifact_envirosuit",
NULL};

static char *btge_monsters_preload[]={
"monster_army",
"monster_demon1",
"monster_dog",
"monster_enforcer",
"monster_ogre",
"monster_knight",
"monster_shalrath",
"monster_shambler",
"monster_enemyhead",
"monster_wizard",
"monster_zombie",
NULL};

int BT_InitItems()
{
	static int init=0;
	int i;
	
	if(init)return(0);
	init=1;
	
	for(i=0; btge_items_preload[i]; i++)
		btGetItem(btge_items_preload[i]);

	for(i=0; btge_monsters_preload[i]; i++)
		btGetMonsterItem(btge_monsters_preload[i]);

	return(1);
}

void item_init(btEntity self)
{
	char *mdl;

	if(!self->item)
		return;

	self->mins=vec3(-16, -16, -16);
	self->maxs=vec3( 16,  16,  16);
	self->origin=v3dadd(self->origin, vec3d(0, 0, 24));

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
	tmp->dropWeight=100;
	tmp->dropChance=100;
	
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

btItem btGetRandomItem()
{
	btItem cur, cur1, lst, prv;
	btItem tmp;
	int i, n;

	BT_InitItems();

	cur=btge_items; n=0; lst=NULL;
	while(cur)
	{
		if(!cur->dropWeight)
		{
			cur=cur->next;
			continue;
		}
	
		cur1=lst; prv=NULL;
		while(cur1)
		{
			if(cur1->dropWeight<=cur->dropWeight)
				break;
			prv=cur1;
			cur1=cur1->chain;
		}
		if(prv)
		{
			cur->chain=prv->chain;
			prv->chain=cur;
		}else
		{
			cur->chain=lst;
			lst=cur;
		}
		n+=cur->dropWeight;
		//n++;
		cur=cur->next;
	}

	i=btRandom()*n;
	//cur=btge_items;
	cur=lst;
	while(cur && cur->chain)
	{
		i-=cur->dropWeight;
		if(i<=0)return(cur);
		cur=cur->chain;
		//i--; cur=cur->next;
	}
	return(cur);
}

btItem btLookupMonsterItem(char *classname)
{
	btItem cur;
	
	cur=btge_monsters;
	while(cur)
	{
		if(!strcmp(cur->classname, classname))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

btItem btGetMonsterItem(char *classname)
{
	btItem tmp;
	int i;

	tmp=btLookupMonsterItem(classname);
	if(tmp)return(tmp);
	
	tmp=gctalloc("btge_iteminfo_t", sizeof(BTGE_ItemInfo));
	tmp->classname=dystrdup(classname);
	tmp->dropWeight=100;
	tmp->dropChance=100;
	
	i=BT_InitMonsterItem(tmp);
	if(i<0)
	{
		gcfree(tmp);
		return(NULL);
	}
	
	tmp->next=btge_monsters;
	btge_monsters=tmp;
	return(tmp);
}

btItem btGetRandomMonsterItem()
{
	btItem cur, cur1, lst, prv;
	btItem tmp;
	int i, n;

	BT_InitItems();

	cur=btge_monsters; n=0; lst=NULL;
	while(cur)
	{
		if(!cur->dropWeight)
		{
			cur=cur->next;
			continue;
		}
	
		cur1=lst; prv=NULL;
		while(cur1)
		{
			if(cur1->dropWeight<=cur->dropWeight)
				break;
			prv=cur1;
			cur1=cur1->chain;
		}
		if(prv)
		{
			cur->chain=prv->chain;
			prv->chain=cur;
		}else
		{
			cur->chain=lst;
			lst=cur;
		}
		n+=cur->dropWeight;
		//n++;
		cur=cur->next;
	}

	i=btRandom()*n;
	//cur=btge_items;
	cur=lst;
	while(cur && cur->chain)
	{
		i-=cur->dropWeight;
		if(i<=0)return(cur);
		cur=cur->chain;
		//i--; cur=cur->next;
	}
	return(cur);
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

btEntity btSpawnRandomItem(vec3d origin)
{
	btItem item;
	btEntity ent;
	float f;

	item=btGetRandomItem();
	if(!item)return(NULL);
	
	f=btRandom()*100;
	if(f>=item->dropChance)
		return(NULL);
	
	ent=btNewEntityTriggerInit(NULL, &item_vt);
	ent->origin=origin;
	ent->item=item;

	btSetThink(ent, btDeleteEntity, 5*60);

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

int BT_InitMonsterItem(btItem item)
{
	char tb[256];
	void *ptr;
	char *s;
	dyt v;

	if(!item)return(-1);
	if(!item->classname)return(-1);
	
//	return(-1);
	
	sprintf(tb, "BtMt_%s", item->classname);
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
//	if(self->health>=self->maxHealth)
	if(self->health>self->maxHealth)
	{
//		if((item->usageValue<=0.0) || (item->usageValue>10.0))
		if(!(item->item) ||
				(item->item->usageValue<=0.0) ||
				(item->item->usageValue>10.0))
			return(false);
	}
	return(true);
}

int item_ammo_canPickup(btEntity self, btEntity item)
{
	if(!self || !item)
		return(false);
		
	if(!(self->flags&BT_FL_CLIENT))
		return(false);
//	if(self->curMP>=self->maxMP)
	if(self->curMP>=(5*self->maxMP))
		return(false);
	return(true);
}

// int item_armor_canPickup(btEntity self, btEntity item)
// {
//	if(!self || !item)
//		return(false);
//		
//	if(!(self->flags&BT_FL_CLIENT))
//		return(false);
//	if(self->curArmor>=self->maxArmor)
//		return(false);
//	return(true);
// }

void health_touchThink(btEntity self, btItem item)
{
	if(item->usageValue!=0)
		{ self->health+=item->usageValue; }
	else
		{ self->health+=50; }
	self->itemflags|=item->flags;
}

void ammo_touchThink(btEntity self, btItem item)
{
	if(item->usageValue!=0)
		{ self->curMP+=item->usageValue; }
	else
		{ self->curMP+=50; }
	self->itemflags|=item->flags;
}

//void armor_touchThink(btEntity self, btItem item)
//{
//	self->curArmor+=50;
//	self->itemflags|=item->flags;
//}

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

	if(item->item->flags)
	{
		//if item gives something which self lacks, allow pickup
//		if((item->flags&self->itemflags)!=item->flags)
		if((item->item->flags&self->itemflags)!=item->item->flags)
			return(true);
	}

//	if(self->curMP>=self->maxMP)
	if(self->curMP>=(5*self->maxMP))
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
//	if(self->curArmor>=self->maxArmor)
	if(self->curArmor>self->maxArmor)
	{
//		if((item->usageValue<=0.0) || (item->usageValue>10.0))
		if(!(item->item) ||
				(item->item->usageValue<=0.0) ||
				(item->item->usageValue>10.0))
			return(false);
	}
	return(true);
}

void armor_touchThink(btEntity self, btItem item)
{
	if(item->usageValue!=0)
		{ self->curArmor+=item->usageValue; }
	else
		{ self->curArmor+=50; }
}

void item_invul_touchThink(btEntity self, btItem item)
{
	if(!BT_InvulFinishedP(self))
	{
		self->invincibleFinished+=30;
		return;
	}

//	self->curMP+=50;
	self->invincibleFinished=btDelayTime(30);
}

void item_invis_touchThink(btEntity self, btItem item)
{
	if(!BT_InvisFinishedP(self))
	{
		self->invisibleFinished+=30;
		return;
	}

//	self->curMP+=50;
	self->invisibleFinished=btDelayTime(30);
}

void item_quad_touchThink(btEntity self, btItem item)
{
//	self->curMP+=50;
	if(!BT_QuadFinishedP(self))
	{
//		self->quadFinished+=30;
//		self->quadFinished+=60;
		self->quadFinished+=45;
		return;
	}

//	self->quadFinished=btDelayTime(30);
	self->quadFinished=btDelayTime(60);
}

void item_enviro_touchThink(btEntity self, btItem item)
{
	if(!BT_EnviroFinishedP(self))
	{
		self->suitFinished+=30;
		return;
	}

//	self->curMP+=50;
	self->suitFinished=btDelayTime(30);
}


BTSV_API void BtIt_item_health(btItem item)
{
	item->worldModel="items/healing/large/large0";
	item->touchSound="sound/items/l_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
	item->usageValue=50;
}

BTSV_API void BtIt_item_health1(btItem item)
{
	item->worldModel="items/healing/stimpack/stimpack0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
	item->usageValue=10;
}

BTSV_API void BtIt_item_health25(btItem item)
{
	item->worldModel="items/healing/medium/medium0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
	item->usageValue=25;
}

BTSV_API void BtIt_item_health_small(btItem item)
{
	item->worldModel="items/healing/stimpack/stimpack0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
	item->usageValue=10;
}

BTSV_API void BtIt_item_health_medium(btItem item)
{
	item->worldModel="items/healing/medium/medium0";
	item->touchSound="sound/items/n_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
	item->usageValue=25;
}

BTSV_API void BtIt_item_health_large(btItem item)
{
	item->worldModel="items/healing/large/large0";
	item->touchSound="sound/items/l_health";
	item->touchThink=health_touchThink;
	item->canPickup=item_health_canPickup;
	item->usageValue=50;
}

BTSV_API void BtIt_item_key2(btItem item)
{
	item->worldModel="items/keys/red_key/red_key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY1;
	item->dropWeight=0;
}

BTSV_API void BtIt_item_key1(btItem item)
{
	item->worldModel="items/keys/key/key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY2;
	item->dropWeight=0;
}

BTSV_API void BtIt_item_flag_team1(btItem item)
{
	item->worldModel="items/keys/red_key/red_key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY3;
	item->dropWeight=0;
}

BTSV_API void BtIt_item_flag_team2(btItem item)
{
	item->worldModel="items/keys/key/key0";
	item->touchSound="sound/items/pkup";
	item->touchThink=item_touchThink;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_KEY4;
	item->dropWeight=0;
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
	item->dropWeight=150;
}

BTSV_API void BtIt_weapon_grenadelauncher(btItem item)
{
	item->worldModel="weapons/g_launch/g_launch0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_LASERGUN;
	item->dropWeight=75;
}

BTSV_API void BtIt_weapon_rocketlauncher(btItem item)
{
	item->worldModel="weapons/g_rocket/g_rocket0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_ROCKETLAUNCHER;
	item->dropWeight=25;
}

BTSV_API void BtIt_weapon_rocketshovel(btItem item)
{
	item->worldModel="weapons/g_rockets/g_rocket0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_ROCKETSHOVEL;
	item->dropWeight=25;
}

BTSV_API void BtIt_weapon_nailgun(btItem item)
{
	item->worldModel="weapons/g_chain/g_chain1";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_CHAINGUN;
	item->dropWeight=100;
}

BTSV_API void BtIt_weapon_supernailgun(btItem item)
{
	item->worldModel="weapons/g_machn/g_machn0";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_PLASMAGUN;
	item->dropWeight=50;
}

BTSV_API void BtIt_weapon_lightning(btItem item)
{
	item->worldModel="weapons/g_hyperb/g_hyperb1";
	item->touchSound="sound/misc/w_pkup";
	item->touchThink=weapon_touchThink;
	item->canPickup=item_weapon_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->flags=BT_ITFL_PLASMACANNON;
	item->dropWeight=5;
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
	item->touchThink=armor_touchThink;
	item->canPickup=item_armor_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->usageValue=75;
}

BTSV_API void BtIt_item_armor2(btItem item)
{
	item->worldModel="items/armor/combat/combat2";
	item->touchSound="sound/misc/ar1_pkup";
	item->touchThink=armor_touchThink;
	item->canPickup=item_armor_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->usageValue=50;
}

BTSV_API void BtIt_item_armor_small(btItem item)
{
	item->worldModel="items/armor/shard/shard0";
	item->touchSound="sound/misc/ar1_pkup";
	item->touchThink=armor_touchThink;
	item->canPickup=item_armor_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->usageValue=25;
}

BTSV_API void BtIt_item_armor_large(btItem item)
{
	item->worldModel="items/armor/body/body0";
	item->touchSound="sound/misc/ar1_pkup";
	item->touchThink=armor_touchThink;
	item->canPickup=item_armor_canPickup;
	item->effects=BT_EFF_ROTATE;
	item->usageValue=75;
}

BTSV_API void BtIt_item_artifact_super_damage(btItem item)
{
	item->worldModel="items/quaddama/quaddama0";
	item->touchSound="sound/items/damage";
	item->touchThink=item_quad_touchThink;
	item->canPickup=item_generic_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_artifact_invulnerability(btItem item)
{
	item->worldModel="items/invulner/invulner1";
	item->touchSound="sound/items/protect";
	item->touchThink=item_invul_touchThink;
//	item->canPickup=item_weapon_canPickup;
	item->canPickup=item_generic_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_strength(btItem item)
{
	item->worldModel="items/quaddama/quaddama0";
	item->touchSound="sound/items/damage";
//	item->touchThink=weapon_touchThink;
	item->touchThink=item_quad_touchThink;
//	item->canPickup=item_weapon_canPickup;
	item->canPickup=item_generic_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_invincible(btItem item)
{
	item->worldModel="items/invulner/invulner1";
	item->touchSound="sound/items/protect";
//	item->touchThink=weapon_touchThink;
	item->touchThink=item_invul_touchThink;
//	item->canPickup=item_weapon_canPickup;
	item->canPickup=item_generic_canPickup;
	item->effects=BT_EFF_ROTATE;
}

BTSV_API void BtIt_item_artifact_envirosuit(btItem item)
{
	item->worldModel="items/enviro/enviro0";
	item->touchSound="sound/items/s_health";
//	item->touchThink=weapon_touchThink;
	item->touchThink=item_enviro_touchThink;
//	item->canPickup=item_weapon_canPickup;
	item->canPickup=item_generic_canPickup;
	item->effects=BT_EFF_ROTATE;
}
