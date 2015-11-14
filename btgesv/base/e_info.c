#include <btgesv.h>

void info_init(btEntity self)
	{ }
void info_deinit(btEntity self)
	{ }
void info_null(btEntity self)
	{ }

void info_blocked(btEntity self, btEntity other)
	{ }
void info_touch(btEntity self, btEntity other)
	{ }
void info_use(btEntity self, btEntity other)
	{ }
void info_pain(btEntity self, btEntity other, float damage)
	{ }
void info_die(btEntity self, btEntity other, float damage)
	{ }
void info_cmdmsg(btEntity self, btEntity other, char *str)
	{ }

BTGE_Entity_Iface info_vt =
{
info_init,			info_deinit,
info_null,			info_null,
info_blocked,		info_touch,
info_use,			info_pain,
info_die,			info_cmdmsg,
};

void info_light_init(btEntity self)
{
	if(!self->state)
	{
		self->state=BT_STATE_ON;
		if(self->spawnflags&BT_SPAWNFLAG_LIGHT_START_OFF)
		{
			self->state=BT_STATE_OFF;
			self->light->flags|=BTGE_LFL_DISABLE|BTGE_LFL_CHANGED;
		}
	}else
	{
		if(self->state==BT_STATE_OFF)
			{ self->light->flags|=BTGE_LFL_DISABLE|BTGE_LFL_CHANGED; }
	}

	self->light->flags|=BTGE_LFL_CHANGED;
	self->light->flags|=BTGE_LFL_STATIC;

	if(!strcmp(self->classname, "light_envmap"))
		{ self->light->flags|=BTGE_LFL_ENVGLOSS|BTGE_LFL_ENVRENDER; }
	if(!strcmp(self->classname, "light_scenery"))
		{ self->light->flags|=BTGE_LFL_SCENERY; }
	if(!strcmp(self->classname, "light_sun"))
		{ self->light->flags|=BTGE_LFL_SUN; }
	if(!strcmp(self->classname, "light_sunrel"))
		{ self->light->flags|=BTGE_LFL_SUN|BTGE_LFL_CAMREL; }
	
	if(!strcmp(self->classname, "light_flourospark"))
	{
		if(!self->light->style)
			self->light->style=10;
		btSound(self, BT_CHAN_AMBIENT, "world/electro",
			0.5, BT_ATTN_STATIC);
	}

	if(!strcmp(self->classname, "light_flourospark"))
	{
		btSound(self, BT_CHAN_AMBIENT, "world/fl_hum1",
			0.5, BT_ATTN_STATIC);
	}

	if(!strcmp(self->classname, "light_torch_small_walltorch") ||
		!strcmp(self->classname, "light_flame_large_yellow") ||
		!strcmp(self->classname, "light_flame_small_yellow") ||
		!strcmp(self->classname, "light_flame_small_white"))
	{
		btSound(self, BT_CHAN_AMBIENT, "world/fire1",
			0.5, BT_ATTN_STATIC);
	}

	if(!self->sound)
	{
		self->sound=BTGE_SEnt_GetStr(self->sent, "s_shader");
	}
	
	if(self->sound)
	{
		btSound(self, BT_CHAN_AMBIENT, self->sound,
			1.0, BT_ATTN_STATIC);
	}
}

void info_light_use(btEntity self, btEntity other)
{
	printf("info_light_use\n");
	self->light->flags^=BTGE_LFL_DISABLE;
	self->light->flags|=BTGE_LFL_CHANGED;
	self->state=(self->light->flags&BTGE_LFL_DISABLE)?
		BT_STATE_OFF:BT_STATE_ON;
}

BTGE_Entity_Iface info_light_vt =
{
info_light_init,	info_deinit,
info_null,			info_null,
info_blocked,		info_touch,
info_light_use,		info_pain,
info_die,			info_cmdmsg,
};


BTSV_API btEntity BtSp_info_player_start(btSEntity args)
	{ return(btNewEntityInit(args)); }
BTSV_API btEntity BtSp_info_player_deathmatch(btSEntity args)
	{ return(btNewEntityInit(args)); }
BTSV_API btEntity BtSp_info_player_coop(btSEntity args)
	{ return(btNewEntityInit(args)); }
BTSV_API btEntity BtSp_info_player_start2d(btSEntity args)
	{ return(btNewEntityInit(args)); }
	
BTSV_API btEntity BtSp_info_player_team1(btSEntity args)
	{ return(btNewEntityInit(args)); }
BTSV_API btEntity BtSp_info_player_team2(btSEntity args)
	{ return(btNewEntityInit(args)); }
BTSV_API btEntity BtSp_info_player_saved(btSEntity args)
	{ return(btNewEntityInit(args)); }

BTSV_API btEntity BtSp_info_player_fixed(btSEntity args)
	{ return(btNewEntityInit(args)); }
BTSV_API btEntity BtSp_info_player_fixed2d(btSEntity args)
	{ return(btNewEntityInit(args)); }

BTSV_API btEntity BtSp_info_intermission(btSEntity args)
	{ return(btNewEntityInit(args)); }

BTSV_API btEntity BtSp_info_teleport_destination(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	return(self);
}

BTSV_API btEntity BtSp_info_landmark(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	return(self);
}

BTSV_API btEntity BtSp_light(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	return(self);
}

BTSV_API btEntity BtSp_light_fluoro(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	return(self);
}

BTSV_API btEntity BtSp_light_fluorospark(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	return(self);
}

BTSV_API btEntity BtSp_light_torch_small_walltorch(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	btSetModel(self, "objects/torch");
	return(self);
}

BTSV_API btEntity BtSp_light_flame_large_yellow(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	btSetModel(self, "objects/flame");
	return(self);
}

BTSV_API btEntity BtSp_light_flame_small_yellow(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	btSetModel(self, "objects/flame");
	return(self);
}

BTSV_API btEntity BtSp_light_flame_small_white(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	btSetModel(self, "objects/flame");
	return(self);
}

BTSV_API btEntity BtSp_light_envmap(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	return(self);
}

BTSV_API btEntity BtSp_light_scenery(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	return(self);
}

BTSV_API btEntity BtSp_light_sun(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	return(self);
}

BTSV_API btEntity BtSp_light_sunrel(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &info_light_vt);	
	return(self);
}

BTSV_API btEntity BtSp_worldspawn(btSEntity args)
{
	btEntity self;
	char *str;
	
	self=btNewEntityInit(args);
	self->map=btCvarGet("mapname");

	str=BTGE_SEnt_GetStr(args, "sky");
	if(str)
	{
		btBroadcastCmd("sky3 bkgr\n", str);
		btBroadcastCmd("sky2 stars\n", str);

		btBroadcastCmd("sky %s\n", str);
//		btBroadcastCmd("sky %s\nsky2 stars\nsky3 bkgr\n", str);
	}

	str=BTGE_SEnt_GetStr(args, "music");
	if(str)
	{
		btSound(self, BT_CHAN_VOICE, str, 1.0, BT_ATTN_NONE);
	}

	return(self);
}

BTSV_API btEntity BtSp_info_null(btSEntity args)
{
	return(NULL);
}

BTSV_API btEntity BtSp_info_notnull(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	return(self);
}


void path_corner_init(btEntity self)
{
	self->solidtype=BT_SOLID_TRIGGER;
	self->mins=vec3(-24, -24, -24);
	self->maxs=vec3( 24,  24,  24);
}

void path_corner_touch(btEntity self, btEntity other)
{
	char *s0;

	if(other->goalEntity==self)
	{
//		printf("path_corner_touch: %s Touched by %d(%s)\n",
//			self->targetname, other->entnum, other->classname);
	
		other->goalEntity=btFindTargets(self->target);
		
		if(self->target && !other->goalEntity)
		{
			s0=self->targetname?self->targetname:"-";
//			printf("path_corner_touch: '%s' no target '%s'\n",
//				s0, self->target);
		}else
		{
//			printf("path_corner_touch: %s Rediret %d(%s) To %s(%s)\n",
//				self->targetname, other->entnum, other->classname,
//				other->goalEntity->targetname,
//				other->goalEntity->classname);
		}
	}
}

BTGE_Entity_Iface path_corner_vt =
{
path_corner_init,	info_deinit,
info_null,			info_null,
info_blocked,		path_corner_touch,
info_use,			info_pain,
info_die,			info_cmdmsg,
};

BTSV_API btEntity BtSp_path_corner(btSEntity args)
{
	btEntity self;
//	self=btNewEntityInit(args);
	self=btNewEntityTriggerInit(args, &path_corner_vt);
	return(self);
}

BTSV_API btEntity BtSp_path_corner_roam(btSEntity args)
{
	btEntity self;
//	self=btNewEntityInit(args);
	self=btNewEntityTriggerInit(args, &path_corner_vt);
	return(self);
}

void path_corner_event_touch(btEntity self, btEntity other)
{
	char *s0;
	if(other->goalEntity==self)
	{
#if 0
		other->target=self->target;
		other->killtarget=self->killtarget;

		other->goalEntity=btFindTargets(self->target);
		other->enemy=btFindTargets(self->killtarget);
		
		if(self->target && !other->goalEntity)
		{
			s0=self->targetname?self->targetname:"-";
			printf("path_corner_event_touch: '%s' no target '%s'\n",
				s0, self->target);
		}

		if(!other->goalEntity)
		{
			//no target, disengage other from script
			other->flags&=~BT_FL_SCRIPTED;
			return;
		}
#endif

		if(other->flags&BT_FL_MONSTER)
		{
			BTAI_ScriptedTouchNode(other, self);
			BTAI_ScriptedThink(other);
		}
	}
}

BTGE_Entity_Iface path_corner_event_vt =
{
path_corner_init,	info_deinit,
info_null,			info_null,
info_blocked,		path_corner_event_touch,
info_use,			info_pain,
info_die,			info_cmdmsg,
};

BTSV_API btEntity BtSp_path_corner_event(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &path_corner_event_vt);
	return(self);
}


/** Note: random, non-useful function, good at being slow */
BTSV_API int btFib(int x)
{
	if(x>2)return(btFib(x-2)+btFib(x-1));
		else return(1);
}

BTSV_API void btSelSort(int *a, int n)
{
	int i, j, k;
	
	for(i=0; i<n; i++)
		for(j=i+1; j<n; j++)
			if(a[j]<a[i])
	{
		k=a[i];
		a[i]=a[j];
		a[j]=k;
	}
}

BTSV_API void btSelSortTst(int n)
{
	int *arr;
	int i;

	arr=malloc(n*sizeof(int));
	for(i=0; i<n; i++)
		arr[i]=rand();
	btSelSort(arr, n);
}

void target_laser_init(btEntity self)
{
	self->effects=BT_EFF_BEAM_LASER|BT_EFF_GLOW_RED|BT_EFF_ALPHA_50;

//	target_laser_think(self);
	btSetThink(self, target_laser_think, 0.1);
}

void target_laser_think(btEntity self)
{
	vec3 v0;
	btEntity tgt;

	tgt=btFindTargets(self->target);
	
	if(tgt)
	{
		self->velocity=v3d2f(v3dsub(tgt->origin, self->origin));
	}else
	{
		BT_SetMovedir(self);
		self->velocity=v3scale(self->movedir, 1000);
	}

	btSetThink(self, target_laser_think, 0.1);
}

void target_laser_touch(btEntity self, btEntity other)
{
//	if(other->goalEntity==self)
//	{
//		other->goalEntity=btFindTargets(self->target);
//	}
}

BTGE_Entity_Iface target_laser_vt =
{
target_laser_init,	info_deinit,
info_null,			info_null,
info_blocked,		target_laser_touch,
info_use,			info_pain,
info_die,			info_cmdmsg,
};

BTSV_API btEntity BtSp_target_laser(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &target_laser_vt);
	return(self);
}

/*
info_camera_event:
These are events which will grab active clients and cause them to do something (move somewhere or look at something).
 */

void info_camera_event_init(btEntity self)
{
//	self->effects=BT_EFF_BEAM_LASER|BT_EFF_GLOW_RED|BT_EFF_ALPHA_50;
//	target_laser_think(self);
//	btSetThink(self, target_laser_think, 0.1);
}

void info_camera_event_use(btEntity self, btEntity other)
{
}

BTGE_Entity_Iface info_camera_event_vt =
{
info_camera_event_init,	info_deinit,
info_null,				info_null,
info_blocked,			path_corner_event_touch,
info_camera_event_use,	info_pain,
info_die,				info_cmdmsg,
};

BTSV_API btEntity BtSp_info_camera_lookat(btSEntity args)
	{ return(btNewEntityTriggerInit(args, &info_camera_event_vt)); }
BTSV_API btEntity BtSp_info_camera_moveto(btSEntity args)
	{ return(btNewEntityTriggerInit(args, &info_camera_event_vt)); }
BTSV_API btEntity BtSp_info_camera_movelook(btSEntity args)
	{ return(btNewEntityTriggerInit(args, &info_camera_event_vt)); }
