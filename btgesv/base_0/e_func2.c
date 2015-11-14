#include <btgesv.h>

int BT_GetMaterialByName(char *name)
{
	int mat;

	mat=BTGE_TXFL_MATERIAL_SOLID;
	if(!stricmp(name, "glass"))
		mat=BTGE_TXFL_MATERIAL_GLASS;
	if(!stricmp(name, "wood"))
		mat=BTGE_TXFL_MATERIAL_WOOD;
	if(!stricmp(name, "metal"))
		mat=BTGE_TXFL_MATERIAL_METAL;
	if(!stricmp(name, "flesh"))
		mat=BTGE_TXFL_MATERIAL_FLESH;
	if(!stricmp(name, "cinder block"))
		mat=BTGE_TXFL_MATERIAL_CONCRETE;
	if(!stricmp(name, "ceiling tile"))
		mat=BTGE_TXFL_MATERIAL_FOAM;
	if(!stricmp(name, "computer"))
		mat=BTGE_TXFL_MATERIAL_COMPUTER;
	if(!stricmp(name, "unbreakable glass"))
		mat=BTGE_TXFL_MATERIAL_UBGLASS;
	if(!stricmp(name, "rocks"))
		mat=BTGE_TXFL_MATERIAL_STONE;
	return(mat);
}

int BT_GetMaterialHealth(int mat)
{
	float health;

	switch(mat&BTGE_TXFL_MATERIAL_MASK)
	{
	case BTGE_TXFL_MATERIAL_NONE:		health=50; break;
	case BTGE_TXFL_MATERIAL_SKY:		health=50; break;

	case BTGE_TXFL_MATERIAL_GLASS:		health=10; break;
	case BTGE_TXFL_MATERIAL_WOOD:		health=25; break;
	case BTGE_TXFL_MATERIAL_METAL:		health=75; break;
	case BTGE_TXFL_MATERIAL_FLESH:		health=50; break;
	case BTGE_TXFL_MATERIAL_CONCRETE:	health=50; break;
	case BTGE_TXFL_MATERIAL_FOAM:		health=10; break;
	case BTGE_TXFL_MATERIAL_COMPUTER:	health=25; break;
	case BTGE_TXFL_MATERIAL_STONE:		health=50; break;

	case BTGE_TXFL_MATERIAL_SOLID:		health=50; break;

	case BTGE_TXFL_MATERIAL_UBGLASS:	health=999999999; break;

	default:
		health=50;
		break;
	}
	
	return(health);
}

void func_breakable_init(btEntity self)
{
	float ang[4], tv[4];
	float f, g, h, health;

	self->solidtype=BT_SOLID_BSP;
	self->movetype=BT_MOVE_PUSH;

	self->pos1=self->origin;
	self->pos2=vec3(99999, 99999, 99999);
	self->angles=vec3(0,0,0);

	self->state=BT_STATE_TOP;

	health=50;

	self->sounds=btGetBModel(self)->flags&BTGE_TXFL_MATERIAL_MASK;

	if(self->material)
		{ self->sounds=BT_GetMaterialByName(self->material); }

	health=BT_GetMaterialHealth(self->sounds);

	if(self->health<=0)
		self->health=health;
	self->takedamage=BT_DAMAGE_YES;

//	V3F_ADDSCALE(org, tv, h, ent->mdl->org);
}

void func_breakable_die(btEntity self, btEntity other, float damage)
{
//	BT_RadiusDamage(self, self, 180, NULL,
//		200, BT_MOD_EXPLOSION);

	func_wall_use(self, other);

	switch(self->sounds)
	{
	case 0:
		btParticleExplosion(self->origin);
		btSound(self, BT_CHAN_VOICE, "sound/weapons/rocklx1a",
			1.0, BT_ATTN_NORM);
		break;
	default:
		btParticleExplosion(self->origin);
		btSound(self, BT_CHAN_VOICE, "sound/weapons/rocklx1a",
			1.0, BT_ATTN_NORM);
		break;
	}


//	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

BTGE_Entity_Iface func_breakable_vt =
{
func_breakable_init,	func_deinit,
func_null,				func_null,
func_blocked,			func_touch,
func_wall_use,			func_pain,
func_breakable_die,		func_cmdmsg,
};

BTSV_API btEntity BtSp_func_breakable(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &func_breakable_vt);
	return(self);
}


void func_rotating_init(btEntity self)
{
	float ang[4], tv[4];
	float f, g, h;

	self->solidtype=BT_SOLID_BSP;
	self->movetype=BT_MOVE_PUSH;

//	self->eTrig=gcalloc(sizeof(BTGE_EntityTrigger));

//	self->angles2=self->angles;

	self->pos1=self->origin;
	self->pos2=vec3(99999, 99999, 99999);
	self->angles=vec3(0,0,0);

	f=self->speed;
	if(self->spawnflags&BT_SPAWNFLAG_ROTATING_REVERSE)
		f=-f;

	if(self->spawnflags&BT_SPAWNFLAG_ROTATING_X_AXIS)
		self->movedir=vec3(f,0,0);
	else if(self->spawnflags&BT_SPAWNFLAG_ROTATING_Y_AXIS)
		self->movedir=vec3(0,f,0);
	else self->movedir=vec3(0,0,f);

	self->state=BT_STATE_OFF;

	if(self->spawnflags&BT_SPAWNFLAG_ROTATING_START_ON)
	{
		self->angvel2=self->movedir;
		self->state=BT_STATE_ON;
	}

//	V3F_ADDSCALE(org, tv, h, ent->mdl->org);
}

void func_rotating_use(btEntity self, btEntity other)
{
	vec3 t;

	if(self->state==BT_STATE_ON)
	{
		self->angvel2=vec3(0,0,0);
		self->state=BT_STATE_OFF;
		return;
	}

	if(self->state==BT_STATE_OFF)
	{
		self->angvel2=self->movedir;
		self->state=BT_STATE_ON;
		return;
	}
}

BTGE_Entity_Iface func_rotating_vt =
{
func_rotating_init,	func_deinit,
func_null,			func_null,
func_blocked,		func_touch,
func_rotating_use,	func_pain,
func_die,			func_cmdmsg,
};

BTSV_API btEntity BtSp_func_rotating(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &func_rotating_vt);
	return(self);
}


void func_door_rotating_init(btEntity self)
{
	float ang[4], tv[4];
	float f, g, h;
	vec3 v0, v1;

//	self->solidtype=BT_SOLID_BSP;
	self->solidtype=BT_SOLID_NOT;
	self->movetype=BT_MOVE_PUSH;

	if(self->speed==0)self->speed=100;
	if(self->wait==0)self->wait=3;
	if(self->lip==0)self->lip=8;

	if(self->damage==0)
		self->damage=10;

	self->snd_start="sound/doors/dr1_strt";
	self->snd_stop="sound/doors/dr1_end";


//	self->pos1=self->origin;
//	self->pos2=vec3(99999, 99999, 99999);
//	self->angles=vec3(0,0,0);

	f=self->speed;

	if(self->spawnflags&BT_SPAWNFLAG_DOOR_ROTATING_X_AXIS)
	{
		self->movedir=vec3(self->speed,0,0);
		self->pos1=vec3(0,0,0);
		self->pos2=vec3(v3z(self->angles),0,0);
	}else if(self->spawnflags&BT_SPAWNFLAG_DOOR_ROTATING_Y_AXIS)
	{
		self->movedir=vec3(0,self->speed,0);
		self->pos1=vec3(0,0,0);
		self->pos2=vec3(0,v3z(self->angles),0);
	}else
	{
		self->movedir=vec3(0,0,self->speed);
		self->pos1=vec3(0,0,0);
		self->pos2=vec3(0,0,v3z(self->angles));
	}

	if(self->spawnflags&BT_SPAWNFLAG_DOOR_ROTATING_REVERSE)
	{
		v0=self->pos1;
		v1=self->pos2;
		self->pos1=v1;
		self->pos2=v0;
		self->angles2=v1;
	}


	self->state=BT_STATE_OFF;
}

void func_door_rotating_movethink(btEntity self)
{
	printf("func_door_rotating_movethink\n");

	if(self->state==BT_STATE_OFF)
	{
		self->angles2=self->pos2;
		self->state=BT_STATE_ON;
		btSetThink(self, func_door_rotating_waitthink, self->wait);
		return;
	}

	if(self->state==BT_STATE_ON)
	{
		btSetThink(self, func_door_rotating_waitthink, self->wait);
	}
}

void func_door_rotating_waitthink(btEntity self)
{
	printf("func_door_rotating_waitthink\n");

	if(self->state==BT_STATE_ON)
	{
		self->angles2=self->pos1;
		self->state=BT_STATE_OFF;
//		btSetThink(self, func_door_waitthink, self->wait);
		return;
	}
}

void func_door_rotating_use(btEntity self, btEntity other)
{
	vec3 t;

	//avoid rapid refire
	if(!BT_AttackFinishedP(self))
		return;
	BT_AttackFinished(self, 0.1);

	printf("func_door_rotating_use\n");

//	func_door_rotating_movethink(self, other);

	if(self->state==BT_STATE_OFF)
	{
		self->angles2=self->pos2;
		self->state=BT_STATE_ON;
		btSetThink(self, func_door_rotating_waitthink, self->wait);
		return;
	}

	if(self->state==BT_STATE_ON)
	{
		btSetThink(self, func_door_rotating_waitthink, self->wait);
	}
}

BTGE_Entity_Iface func_door_rotating_vt =
{
func_door_rotating_init,	func_deinit,
func_null,					func_null,
func_blocked,				func_touch,
func_door_rotating_use,		func_pain,
func_die,					func_cmdmsg,
};

BTSV_API btEntity BtSp_func_door_rotating(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &func_door_rotating_vt);
	return(self);
}

BTSV_API btEntity BtSp_func_secret(btSEntity args)
	{ return(BtSp_func_door_secret(args)); }

BTSV_API btEntity BtSp_func_mover(btSEntity args)
	{ return(BtSp_func_wall(args)); }
BTSV_API btEntity BtSp_func_fx(btSEntity args)
	{ return(BtSp_func_wall(args)); }

BTSV_API btEntity BtSp_func_instance(btSEntity args)
{
	char tb[256];
	float tv[3];
	btEntity self;
	vec3 org;
	char *map;

	map=BTGE_SEnt_GetStr(args, "map");
	if(!map)return(NULL);
	BTGE_SEnt_GetVec3(args, "origin", tv);

	sprintf(tb, "maps/%s.map", map);
	org=vec3vf(tv);
	btSpawnPrefab(tb, org);
	return(NULL);
}

BTSV_API btEntity BtSp_prefab_worldspawn(btSEntity args)
{
	return(BtSp_func_static(args));
}
