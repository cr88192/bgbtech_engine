#include <btgesv.h>

BTSV_API btEntity BtSp_ambient_drone(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);

	btSound(self, BT_CHAN_AMBIENT, "sound/world/comp_hum3",
		1.0, BT_ATTN_STATIC);

	return(self);
}

BTSV_API btEntity BtSp_ambient_comp_hum(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);

	btSound(self, BT_CHAN_AMBIENT, "sound/world/comp_hum2",
		1.0, BT_ATTN_STATIC);

	return(self);
}

BTSV_API btEntity BtSp_ambient_drip(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	btSound(self, BT_CHAN_AMBIENT, "sound/world/drip_amb",
		1.0, BT_ATTN_STATIC);
	return(self);
}

BTSV_API btEntity BtSp_ambient_swamp1(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	btSound(self, BT_CHAN_AMBIENT, "sound/world/swamp1",
		1.0, BT_ATTN_STATIC);
	return(self);
}

BTSV_API btEntity BtSp_ambient_swamp2(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	btSound(self, BT_CHAN_AMBIENT, "sound/world/swamp1",
		1.0, BT_ATTN_STATIC);
	return(self);
}

BTSV_API btEntity BtSp_ambient_suck_wind(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	btSound(self, BT_CHAN_AMBIENT, "sound/world/wind2",
		1.0, BT_ATTN_STATIC);
	return(self);
}

BTSV_API btEntity BtSp_ambient_light_buzz(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	btSound(self, BT_CHAN_AMBIENT, "sound/world/fl_hum1",
		1.0, BT_ATTN_STATIC);
	return(self);
}

BTSV_API btEntity BtSp_ambient_flouro_buzz(btSEntity args)
{
	btEntity self;
	self=btNewEntityInit(args);
	btSound(self, BT_CHAN_AMBIENT, "sound/world/fl_hum1",
		1.0, BT_ATTN_STATIC);
	return(self);
}

BTSV_API btEntity BtSp_ambient_speaker(btSEntity args)
{
	btEntity self;
	char *s;
	
	self=btNewEntityInit(args);
	if(!self->sound)
	{
		btDeleteEntity(self);
		return(NULL);
	}
	
	btSound(self, BT_CHAN_AMBIENT, self->sound,
		1.0, BT_ATTN_STATIC);
	return(self);
}


void misc_explobox_init(btEntity self)
{
	self->solidtype=BT_SOLID_BBOX;
	self->movetype=BT_MOVE_NONE;

//	self->solidtype=BT_SOLID_OBB;
//	self->movetype=BT_MOVE_RBD_PHYSICS;
	
	btSetModel(self, "models/objects/barrels/barrel.ac");
//	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=20;
	
	self->takedamage=BT_DAMAGE_YES;
	
//	btWalkmonsterStart(self);
}

void misc_explobox_die(btEntity self, btEntity other, float damage)
{
	BT_RadiusDamage(self, self, 180, NULL,
		200, BT_MOD_EXPLOSION);

	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

BTGE_Entity_Iface misc_explobox_vt =
{
misc_explobox_init,	info_deinit,
info_null,			info_null,
info_blocked,		info_touch,
info_use,			info_pain,
misc_explobox_die,	info_cmdmsg,
};

BTSV_API btEntity BtSp_misc_explobox(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_explobox_vt);	

//	btSound(self, BT_CHAN_VOICE, "sound/world/comp_hum2",
//		1.0, BT_ATTN_STATIC);

	return(self);
}

BTSV_API btEntity BtSp_misc_explobox2(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_explobox_vt);	
	return(self);
}

void misc_model_think(btEntity self)
{
	if(self->frame<self->count)
	{
		self->frame++;
		btSetThink(self, misc_model_think, self->delay);
	}
}

void misc_model_init(btEntity self)
{
//	self->solidtype=BT_SOLID_BBOX;
//	self->movetype=BT_MOVE_NONE;
	
	btSetModel(self, self->model);
//	self->snd_sight="sound/soldier/solsght1";
	
	if(!strcmp(self->classname, "misc_model_breakable"))
	{
		self->solidtype=BT_SOLID_BBOX;
		self->movetype=BT_MOVE_NONE;
		self->mins=vec3(-16, -16, -16);
		self->maxs=vec3( 16,  16,  16);
		self->health=20;
		self->takedamage=BT_DAMAGE_YES;
	}

	if(!strcmp(self->classname, "misc_model_scenery"))
		{ self->effects2|=BT_EF2_SCENERY; }
	
	if(self->scale>0)
	{
		btSetScale(self, self->scale);
	}
	
//	self->mins=vec3(-16, -16, -24);
//	self->maxs=vec3(16, 16, 40);
//	self->health=20;
	
//	self->takedamage=BT_DAMAGE_YES;
	
//	btWalkmonsterStart(self);

	if(self->count>0)
	{
		btSetThink(self, misc_model_think, self->wait);
	}
}

void misc_model_die(btEntity self, btEntity other, float damage)
{
	BT_RadiusDamage(self, self, 180, NULL,
		200, BT_MOD_EXPLOSION);

	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

BTGE_Entity_Iface misc_model_vt =
{
misc_model_init,	info_deinit,
info_null,			info_null,
info_blocked,		info_touch,
info_use,			info_pain,
misc_model_die,		info_cmdmsg,
};

BTSV_API btEntity BtSp_misc_model(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_model_vt);	
	return(self);
}

BTSV_API btEntity BtSp_misc_gamemodel(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_model_vt);	
	return(self);
}

BTSV_API btEntity BtSp_misc_model_breakable(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_model_vt);	
	return(self);
}

BTSV_API btEntity BtSp_misc_model_scenery(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_model_vt);	
	return(self);
}


void misc_squidship_think(btEntity self)
{
	btEntity tgt;
	vec3 dir;

	if(self->state==BT_STATE_USER_A)
	{
		btSound(self, BT_CHAN_VOICE, "sound/voice/SquidAttackWorlds0_1",
			1.0, BT_ATTN_NORM);
		btSetThink(self, misc_squidship_think, 3);
		self->state=BT_STATE_USER_B;
		return;
	}

	tgt=btFindTargets(self->target);
	if(tgt)
	{
		if(self->frame==0)
		{
			btSound(self, BT_CHAN_VOICE, "sound/voice/FiringLazah0_2",
				1.0, BT_ATTN_NORM);
		}
		
		dir=BT_TargetDirection(self, tgt);
		BT_FirePlasma(self, self->origin, dir, 15, 50, 32, 4);

		self->frame=1;
		btSetThink(self, misc_squidship_think, 0.09);
	}
}

void misc_squidship_init(btEntity self)
{
//	self->solidtype=BT_SOLID_BBOX;
//	self->movetype=BT_MOVE_NONE;
	
	btSetModel(self, self->model);
//	self->snd_sight="sound/soldier/solsght1";
	
	self->state=BT_STATE_USER_A;
	
	if(!strcmp(self->classname, "misc_model_breakable"))
	{
		self->solidtype=BT_SOLID_BBOX;
		self->movetype=BT_MOVE_NONE;
		self->mins=vec3(-8, -8, -8);
		self->maxs=vec3( 8,  8,  8);
		self->health=20;
		self->takedamage=BT_DAMAGE_YES;
	}
	
//	self->mins=vec3(-16, -16, -24);
//	self->maxs=vec3(16, 16, 40);
//	self->health=20;
	
//	self->takedamage=BT_DAMAGE_YES;

	btSetThink(self, misc_squidship_think, 1);

//	btWalkmonsterStart(self);
}

BTGE_Entity_Iface misc_squidship_vt =
{
misc_squidship_init,	info_deinit,
info_null,				info_null,
info_blocked,			info_touch,
info_use,				info_pain,
misc_model_die,			info_cmdmsg,
};

BTSV_API btEntity BtSp_misc_squidship(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_squidship_vt);	
	return(self);
}

/** entity spawned for non-spawned entities */
BTSV_API btEntity BtSp_misc_nospawn(btSEntity args)
{
	btEntity self;
	
	self=btNewEntityTriggerInit(args, NULL);
	BT_UseTargets(self, self);
	btSetThink(self, btDeleteEntity, 0.25);
	return(self);
}

void trap_spikeshooter_init(btEntity self)
{
	BT_SetMovedir(self);
}

void trap_spikeshooter_use(btEntity self, btEntity other)
{
	btSound(self, BT_CHAN_VOICE, "sound/weapons/blastf1a",
		1.0, BT_ATTN_NORM);
	BT_FireBlaster(self, self->origin, self->movedir, 10, 800, 32);
}

BTGE_Entity_Iface trap_spikeshooter_vt =
{
trap_spikeshooter_init,	info_deinit,
info_null,				info_null,
info_blocked,			info_touch,
trap_spikeshooter_use,	info_pain,
info_die,				info_cmdmsg,
};

BTSV_API btEntity BtSp_trap_spikeshooter(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &trap_spikeshooter_vt);	
	return(self);
}

void target_eval_use(btEntity self, btEntity other)
{
//	btSound(self, BT_CHAN_VOICE, "sound/weapons/blastf1a",
//		1.0, BT_ATTN_NORM);
//	BT_FireBlaster(self, self->origin, self->movedir, 10, 800, 32);
//	btConsoleCmd(";%s", self->message);
	btScriptEval(self->message);
}

BTGE_Entity_Iface target_eval_vt =
{
info_init,				info_deinit,
info_null,				info_null,
info_blocked,			info_touch,
target_eval_use,		info_pain,
info_die,				info_cmdmsg,
};

BTSV_API btEntity BtSp_target_eval(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &target_eval_vt);	
	return(self);
}


void target_speaker_init(btEntity self)
{
	if(self->spawnflags&BT_SPAWNFLAG_SPEAKER_LOOP_ON)
	{
		btSound(self, BT_CHAN_AMB_VOICE, self->sound, 1.0, BT_ATTN_STATIC);
		self->state=BT_STATE_ON;
		return;
	}

	if(self->spawnflags&BT_SPAWNFLAG_SPEAKER_LOOP_OFF)
	{
		self->state=BT_STATE_OFF;
		return;
	}

	if(!self->targetname)
	{
		if(self->wait>0)
		{
			btSetThink(self, target_speaker_think, self->wait);
			return;
		}
		
		btSound(self, BT_CHAN_AMB_VOICE, self->sound, 1.0, BT_ATTN_STATIC);
		self->state=BT_STATE_ON;
		return;
	}

//	BT_SetMovedir(self);
}

void target_speaker_think(btEntity self)
{
	btSound(self, BT_CHAN_VOICE, self->sound, 1.0, BT_ATTN_NORM);
}

void target_speaker_use(btEntity self, btEntity other)
{
	if(!self->sound)
		return;

	printf("target_speaker_use: %s, %s, %d\n",
		self->targetname, self->sound, self->state);

	if(self->state==BT_STATE_OFF)
	{
		//turn on
		btSound(self, BT_CHAN_AMB_VOICE, self->sound, 1.0, BT_ATTN_STATIC);
		self->state=BT_STATE_ON;
		return;
	}

	if(self->state==BT_STATE_ON)
	{
		//turn off
		btSound(self, BT_CHAN_VOICE, "", 1.0, BT_ATTN_STATIC);
		self->state=BT_STATE_OFF;
		return;
	}

	btSound(self, BT_CHAN_VOICE, self->sound, 1.0, BT_ATTN_NORM);

//	BT_FireBlaster(self, self->origin, self->movedir, 10, 800, 32);
//	btConsoleCmd(";%s", self->message);
//	btScriptEval(self->message);
}

BTGE_Entity_Iface target_speaker_vt =
{
target_speaker_init,	info_deinit,
info_null,				info_null,
info_blocked,			info_touch,
target_speaker_use,		info_pain,
info_die,				info_cmdmsg,
};

BTSV_API btEntity BtSp_target_speaker(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &target_speaker_vt);	
	return(self);
}

BTSV_API btEntity BtSp_speaker(btSEntity args)
{
	btEntity self;
	int i, j;

	self=btNewEntityTriggerInit(args, &target_speaker_vt);

	self->targetname=BTGE_SEnt_GetStr(args, "name");
	self->sound=BTGE_SEnt_GetStr(args, "s_shader");	
	i=BTGE_SEnt_GetNum(args, "s_looping");
	j=BTGE_SEnt_GetNum(args, "s_waitfortrigger");

	if(i==1)
	{
		self->spawnflags|=j?BT_SPAWNFLAG_SPEAKER_LOOP_OFF:
			BT_SPAWNFLAG_SPEAKER_LOOP_ON;
	}

	target_speaker_init(self);

	return(self);
}

BTSV_API btEntity BtSp_target_position(btSEntity args)
{
	btEntity self;
//	self=btNewEntityTriggerInit(args, &target_speaker_vt);	
	self=btNewEntityTriggerInit(args, NULL);	
	return(self);
}


void misc_waterfall_init(btEntity self)
{
	BT_SetMovedir(self);
	btSetThink(self, misc_waterfall_think, 0.1);
}

void misc_waterfall_think(btEntity self)
{
	btParticles("waterglob_fall", 6, BT_RGB_WHITE,
		self->origin, v3scale(self->movedir, self->speed), vec3(25,25,25));
	btSetThink(self, misc_waterfall_think, 0.1);
}

BTGE_Entity_Iface misc_waterfall_vt =
{
misc_waterfall_init,	info_deinit,
info_null,				info_null,
info_blocked,			info_touch,
info_use,				info_pain,
info_die,				info_cmdmsg,
};

BTSV_API btEntity BtSp_misc_waterfall(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &misc_waterfall_vt);	
	return(self);
}


void target_npcevent_useTarget(btEntity self,
	btEntity other, btEntity target)
{
	if(other->flags&BT_FL_MONSTER)
	{
		BTAI_ScriptedSetNode(other, target);
	}
}

void target_npcevent_use(btEntity self, btEntity other)
{
	btEntity lst, cur, tgt;
	int i, n;

	if(!other)return;

	tgt=btFindTargets(self->landmark);

	lst=btFindTargets(self->target);
	if(!lst)return;
	
	cur=lst;
	while(cur)
	{
		target_npcevent_useTarget(self, cur, tgt);
		cur=cur->chain;
	}
}

BTGE_Entity_Iface target_npcevent_vt =
{
info_init,				info_deinit,
info_null,				info_null,
info_blocked,			info_touch,
target_npcevent_use,	info_pain,
info_die,				info_cmdmsg,
};

BTSV_API btEntity BtSp_target_npcevent(btSEntity args)
{
	btEntity self;
	self=btNewEntityTriggerInit(args, &target_npcevent_vt);	
	return(self);
}


BTSV_API btEntity BtSp_random_prefab(btSEntity args)
{
	char tb[256];
	float tv[3];
	btEntity self;
	vec3 org;
	char **a;
	char *map, *name, *str;
	int i, n;

//	map=BTGE_SEnt_GetStr(args, "map");
//	if(!map)return(NULL);
	BTGE_SEnt_GetVec3(args, "origin", tv);

	str=
		"prefab_empty "
		"prefab_enemy "
		"prefab_wall1a "
		"prefab_wall1b "
		"prefab_wall1c "
		"prefab_wall2a "
		"prefab_wall2b "
		"prefab_wall2c ";

	a=gcrsplit(str);
	for(n=0; a[n]; n++);
	map=a[btRangeRandomInt(0, n-1)];

	sprintf(tb, "maps/%s.map", map);
	org=vec3vf(tv);
	btSpawnPrefab(tb, org);
	return(NULL);
}

BTSV_API btEntity BtSp_random_dungeon(btSEntity args)
{
	char tb[256];
	char *ta[64];
	char *ta_a[64];
	char *ta_b[64];
	char *ta_c[64];
	char *ta_z[64];
	float tv[3], tv1[3], szv[3], stv[3];
	btEntity self;
	vec3 org;
	byte *maze;
	char **a;
	char *map, *name, *str, *s;
	int xs, ys, zs;
	int na, nb, nc, nz;
	int i, j, k, l, n;

//	map=BTGE_SEnt_GetStr(args, "map");
//	if(!map)return(NULL);
	BTGE_SEnt_GetVec3(args, "origin", tv);

	BTGE_SEnt_GetVec3(args, "boxsize", szv);
	xs=szv[0]; ys=szv[1]; zs=szv[2];

	if(xs<=0)xs=16;
	if(ys<=0)ys=16;
	if(zs<=0)zs=1;

	if(xs>64)xs=64;
	if(ys>64)ys=64;

	BTGE_SEnt_GetVec3(args, "boxstep", stv);
	if(stv[0]<=0)stv[0]=256;
	if(stv[1]<=0)stv[1]=256;
	if(stv[2]<=0)stv[2]=256;

	printf("random_dungeon: ( %d %d %d ) ( %f %f %f )\n",
		xs, ys, zs,
		stv[0], stv[1], stv[2]);

	maze=malloc(xs*ys*zs);
	BTGE_GenerateMaze3D(maze, xs, ys, zs, rand(), 0);

	str=
		"prefab_empty "
		"prefab_enemy "
		"prefab_wall1a "
		"prefab_wall1b "
		"prefab_wall1c "
		"prefab_wall2a "
		"prefab_wall2b "
		"prefab_wall2c ";

	a=gcrsplit(str);
//	for(n=0; a[n]; n++)
//		ta[n]=dyllStrdup(a[n]);

	na=0; nb=0; nc=0; nz=0;
	for(n=0; a[n]; n++)
	{
		s=dyllStrdup(a[n]);
		ta[n]=s;
		switch(s[strlen(s)-1])
		{
		case 'a': ta_a[na++]=s; break;
		case 'b': ta_b[nb++]=s; break;
		case 'c': ta_c[nc++]=s; break;
		default: ta_z[nz++]=s; break;
		}
	}

#if 1
	for(i=0; i<zs; i++)
		for(j=0; j<ys; j++)
			for(k=0; k<xs; k++)
	{
//		map=ta[btRangeRandomInt(0, n-1)];
		switch(maze[i*xs*ys+j*xs+k])
		{
		case 0: case 1:
			map=ta_z[btRangeRandomInt(0, nz-1)]; break;
		case 2:
			l=0;
			if(BTGE_CheckWallPointMaze(maze, xs, ys, k-1, j))l|=1;
			if(BTGE_CheckWallPointMaze(maze, xs, ys, k+1, j))l|=2;
			if(BTGE_CheckWallPointMaze(maze, xs, ys, k, j-1))l|=4;
			if(BTGE_CheckWallPointMaze(maze, xs, ys, k, j+1))l|=8;
			switch(l)
			{
			case 1: case 2: case 3:
				map=ta_a[btRangeRandomInt(0, na-1)]; break;
			case 4: case 8: case 12:
				map=ta_b[btRangeRandomInt(0, nb-1)]; break;
			default:
				map=ta_c[btRangeRandomInt(0, nc-1)]; break;
			}
			break;

		case 3:		//start
			map=ta_z[btRangeRandomInt(0, nz-1)]; break;
		case 4:		//end
			map=ta_z[btRangeRandomInt(0, nz-1)]; break;

		case 5: map=ta_a[btRangeRandomInt(0, na-1)]; break;
		case 6: map=ta_b[btRangeRandomInt(0, nb-1)]; break;
		case 7: map=ta_c[btRangeRandomInt(0, nc-1)]; break;
		default:
			map=ta_z[btRangeRandomInt(0, nz-1)];
			break;
		}

		tv1[0]=tv[0]+k*stv[0];
		tv1[1]=tv[1]+j*stv[1];
		tv1[2]=tv[2]+i*stv[2];

		printf("random_dungeon: at=%d,%d,%d map=%s\n",
			k,j,i, map);

		sprintf(tb, "maps/%s.map", map);
		org=vec3vf(tv1);
		btSpawnPrefab(tb, org);
	}
#endif

#if 0
	for(i=0; i<zs; i++)
		for(j=0; j<ys; j++)
			for(k=0; k<xs; k++)
	{
		map=ta[btRangeRandomInt(0, n-1)];

		tv1[0]=tv[0]+k*stv[0];
		tv1[1]=tv[1]+j*stv[1];
		tv1[2]=tv[2]+i*stv[2];

		printf("random_dungeon: at=%d,%d,%d map=%s\n",
			k,j,i, map);

		sprintf(tb, "maps/%s.map", map);
		org=vec3vf(tv1);
		btSpawnPrefab(tb, org);
	}
#endif

	return(NULL);
}

BTSV_API btEntity BtSp_random_terrain(btSEntity args)
{
	BTGE_Voxel_GenerateChunks(btCurrentWorld());
	return(NULL);
}



void misc_fractree_init(btEntity self)
{
	self->solidtype=BT_SOLID_BBOX;
	self->movetype=BT_MOVE_NONE;

//	self->solidtype=BT_SOLID_OBB;
//	self->movetype=BT_MOVE_RBD_PHYSICS;
	
	btSetModel(self, "fractal_tree");
//	self->snd_sight="sound/soldier/solsght1";
	
	self->mins=vec3(-16, -16, -24);
	self->maxs=vec3(16, 16, 40);
	self->health=20;
	
	self->takedamage=BT_DAMAGE_YES;
	
//	btWalkmonsterStart(self);
}

void misc_fractree_die(btEntity self, btEntity other, float damage)
{
//	BT_RadiusDamage(self, self, 180, NULL,
//		200, BT_MOD_EXPLOSION);

	BT_EntityExplode(self);
//	BT_EntityBloodExplode(self);
//	BT_EntityDisintegration(self);
}

BTGE_Entity_Iface misc_fractree_vt =
{
misc_fractree_init,	info_deinit,
info_null,			info_null,
info_blocked,		info_touch,
info_use,			info_pain,
misc_fractree_die,	info_cmdmsg,
};

BTSV_API btEntity BtSp_misc_fractal_tree(btSEntity args)
{
	btEntity self;

	self=btNewEntityTriggerInit(args, &misc_fractree_vt);	
	return(self);
}

BTSV_API btEntity BtSp_misc_spawn_tiles(btSEntity args)
{
	char tb[256];
	char *spawns[256];
	byte pal[768];
	float org[3], tv[3], tvm[3], tvn[3];
	byte *buf, *buftx;
	char *img, *tex, *str;
	float step;
	int xs, ys, xs1, ys1;
	int i, j, k, l, np, nsp;

//	*(int *)-1=-1;

	BTGE_SEnt_GetVec3(args, "origin", org);

	np=BTGECM_ColorKey_LoadPaletteDefault(pal, 256);
	str=BTGE_SEnt_GetStr(args, "palette");
	if(str)
	{
		i=BTGECM_ColorKey_LoadPalette(str, pal, 256);
		if(i>0)np=i;
	}

#if 1
	for(i=0; i<256; i++)
		spawns[i]=NULL;
	if(1)
	{
		spawns[1]="monster_army";
		spawns[2]="monster_dog";
		spawns[3]="monster_orge";
		spawns[4]="monster_zombie";
		spawns[5]="monster_demon1";
		spawns[6]="monster_wizard";
		spawns[7]="monster_knight";
		spawns[8]="monster_shambler";
		spawns[9]="monster_shalrath";
		spawns[10]="monster_enforcer";
		spawns[11]="monster_hell_knight";
		spawns[17]="item_shells";
		spawns[18]="item_rockets";
		spawns[19]="item_spikes";
		spawns[20]="item_cells";
		spawns[21]="item_bullets";
	}
#endif

	str=BTGE_SEnt_GetStr(args, "spawntab");
	if(str)
	{
		i=BTGECM_ColorKey_LoadNames(str, "spawn", spawns, 256);
		if(i>0)nsp=i;
	}

	img=BTGE_SEnt_GetStr(args, "image");
	if(!img)
	{
		printf("BtSp_misc_spawn_tiles: no image\n");
		return(NULL);
	}

	sprintf(tb, "%s_spawn.png", img);
	buf=BTGECM_PNG_LoadName(tb, &xs, &ys);
	if(!buf)
	{
		printf("BtSp_misc_spawn_tiles: failed load image %s\n", img);
		return(NULL);
	}

	step=BTGE_SEnt_GetNum(args, "step");
	if(step<=0)step=36;

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
//		k=BTGECM_Misc_GetColorIndexPDN(buf+((i*xs)+j)*4);
		k=BTGECM_ColorKey_GetColorPaletteIndex(
			buf+((i*xs)+j)*4, pal, np);
		if(!k)continue;
		if(k<=0)continue;

#if 0
		str=NULL;
		switch(k)
		{
		case 1: str="monster_army"; break;
		case 2: str="monster_dog"; break;
		case 3: str="monster_orge"; break;
		case 4: str="monster_zombie"; break;
		case 5: str="monster_demon1"; break;
		case 6: str="monster_wizard"; break;
		case 7: str="monster_knight"; break;
		case 8: str="monster_shambler"; break;
		case 9: str="monster_shalrath"; break;
		case 10: str="monster_enforcer"; break;
		case 11: str="monster_hell_knight"; break;

//		case 17: str="item_shells"; break;
		case 17: str="item_shells"; break;
		case 18: str="item_rockets"; break;
		case 19: str="item_spikes"; break;
		case 20: str="item_cells"; break;
		case 21: str="item_bullets"; break;
		
		default: break;
		}
#endif

		str=spawns[k];

		if(!str)continue;

		tv[0]=org[0]+(j-(xs/2))*step;
		tv[1]=org[1]+(i-(ys/2))*step;
		tv[2]=org[2]+32;

		btSpawnEntityBasic(str, vec3vf(tv), 0);

	}
	
	return(NULL);
}
