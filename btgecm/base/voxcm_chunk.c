#include <btgecm.h>

static char *btge_voxnames[]=
{
"air",					// 0
"stone",				// 1
"grass",				// 2
"dirt",					// 3
"cobblestone",			// 4
"wood",					// 5
"sapling",				// 6
"bedrock",				// 7
"water",				// 8
"water2",				// 9
"lava",					//10
"lava2",				//11
"sand",					//12
"gravel",				//13
"goldore",				//14
"ironore",				//15
"coalore",				//16
"log",					//17
"leaves",				//18
"sponge",				//19
"glass",				//20
"lapis_ore",			//21
"lapis_block",			//22
"dispenser",			//23
"sandstone",			//24
"note_block",			//25
"bed",					//26
"powered_rail",			//27
"detector_rail",		//28
"sticky_piston",		//29
"cobweb",				//30
"tall_grass",			//31
"dead_bush",			//32
"piston",				//33
"piston_head",			//34
"cloth",				//35
"block_transit",		//36
"flower_yel",			//37
"flower_red",			//38
"shroom_brown",			//39
"shroom_red",			//40
"block_gold",			//41
"block_iron",			//42
"doubleslab",			//43
"slab",					//44
"brick",				//45
"tnt",					//46
"bookshelf",			//47
"moss_stone",			//48
"obsidian",				//49
"torch",				//50
"fire",					//51
"mobspawner",			//52
"wood_stairs",			//53
"chest",				//54
"redstone_wire",		//55
"diamond_ore",			//56
"diamond_block",		//57
"crafting_table",		//58
"seeds",				//59
"farmland",				//60
"furnace",				//61
"furnace_lit",			//62
"sign_post",			//63
"wood_door",			//64
"ladder",				//65
"rail",					//66
"cobble_stairs",		//67
"wall_sign",			//68
"lever",				//69
"stone_plate",			//70
"iron_door",			//71
"wood_plate",			//72
"redstone_ore",			//73
"redstone_ore2",		//74
"redstone_torch0",		//75
"redstone_torch1",		//76
"stone_button",			//77
"snow",					//78
"ice",					//79
"snow_block",			//80
"cactus",				//81
"clay_block",			//82
"sugar_cane",			//83
"jukebox",				//84
"fence",				//85
"pumpkin",				//86
"netherrack",			//87
"soulsand",				//88
"glowstone",			//89
"portal",				//90
"jackolantern",			//91
"cakeblock",			//92
"redstone_relay0",		//93
"redstone_relay1",		//94
// "chest_locked",		//95
"invisible_bedrock",	//95
"trapdoor",				//96
"silverfish",			//97
"stone_brick",			//98
"shroom_brown_huge",	//99
"shroom_red_huge",		//100
"iron_bars",			//101
"glass_pane",			//102
"melon",				//103
"pumpkin_stem",			//104
"melon_stem",			//105
"vines",				//106
"fence_gate",			//107
"brick_stairs",			//108
"stonebrick_stairs",	//109
"mycelium",				//110
"lilypad",				//111
"netherbrick",			//112
"netherbrick_fence",	//113
"netherbrick_stairs",	//114
"netherwart",			//115
"enchant_table",		//116
"brewstand",			//117
"cauldron",				//118
"airportal",			//119
"airportal_frame",		//120
//above: prior to Beta 1.9pre 3/4
//added, 2012-08-26
"endstone",				//121
"dragon_egg",			//122
"redstone_lamp0",		//123
"redstone_lamp1",		//124
"wood_doubleslab",		//125
"wood_slab",			//126
"cocoa_plant",			//127
"sandstone_stairs",		//128
"emerald_ore",			//129
"ender_chest",			//130
"tripwire_hook",		//131
"tripwire",				//132
"emerald_block",		//133
"spruce_stairs",		//134
"birch_stairs",			//135
"jungle_stairs",		//136
"command_block",		//137
"beacon_block",			//138
"cobblestone_wall",		//139
"flower_pot",			//140
"carrots",				//141
"potatoes",				//142
"wood_button",			//143
//added, 2014-01-12
"skull",				//144
"anvil",				//145
"trap_chest",			//146
"light_plate",			//147
"heavy_plate",			//148
"compare_off",			//149
"compare_on",			//150
"light_sensor",			//151
"redstone_block",		//152
"quartz_ore",			//153
"hopper",				//154
"quartz",				//155
"quartz_stairs",		//156
"active_rail",			//157
"dropper",				//158
"dyed_clay",			//159
"dyed_glass_pane",		//160
"leaves2",				//161
"log2",					//162
"acacia_stairs",		//163
"darkoak_stairs",		//164
"bouncy_slime",			//165
"unused_166",			//166
"unused_167",			//167
"unused_168",			//168
"unused_169",			//169
"hay",					//170
"carpet",				//171
NULL
};

static char *btge_voxnames2[]=
{
"redstone_pipe",		//256
"greenstone_pipe",		//257
"bluestone_pipe",		//258
"greenstone_torch0",	//259
"greenstone_torch1",	//260
"bluestone_torch0",		//261
"bluestone_torch1",		//262
"greenstone_wire",		//263
"bluestone_wire",		//264
"goldstone_wire",		//265
"goldstone_pipe",		//266
"goldstone_torch0",		//267
"goldstone_torch1",		//268

"comp_innards",			//269
"comp_panel",			//270
"panel_grate",			//271
"panel_hex",			//272
"panel_pipes",			//273

"redstone_gate_and",	//274
"redstone_gate_or",		//275
"redstone_gate_xor",	//276
"redstone_gate_nand",	//277
"redstone_gate_nor",	//278
"redstone_gate_xnor",	//279

"slime",				//280
"slime2",				//281
"glowslime",			//282
"glowslime2",			//283

"block_rust",			//284
"panel_pi",				//285

"grate_square",			//286
"grate_hex",			//287

"panel_video1",			//288
"panel_video2",			//289
"panel_video3",			//290
"panel_video4",			//291
"panel_video5",			//292
"panel_video6",			//293
"panel_video7",			//294
"panel_video8",			//295

"block_rust2",			//296
"block_rust3",			//297
"block_rust4",			//298
"block_rust5",			//299
"block_rust6",			//300
"block_rust7",			//301
"block_rust8",			//302
"block_rust9",			//303

NULL
};

char *btge_voxnames_cache[4096];
BTGE_VoxelInfo *btge_voxinfo[4096];
BTGE_VoxelInfo *btge_oldvoxinfo[4096];
byte btge_voxel_opaque[4096];
int btge_voxel_use_flexform=1;

void *btge_voxel_mutex;

void btge_voxel_lock()
{
	thLockFastMutex(btge_voxel_mutex);
}

void btge_voxel_unlock()
{
	thUnlockFastMutex(btge_voxel_mutex);
}


BTGE_API void BTGE_Voxel_SetInfoDefaults(BTGE_VoxelInfo *tmp)
{
	tmp->mins[0]=0;
	tmp->mins[1]=0;
	tmp->mins[2]=0;

	tmp->maxs[0]=32;
	tmp->maxs[1]=32;
	tmp->maxs[2]=32;
	
	tmp->img_scale[0]=1;
	tmp->img_scale[1]=1;
	tmp->img_scale[2]=1;
	
	tmp->blast_resist=50;
}

BTGE_API int BTGE_Voxel_LoadInfo(BTGE_VoxelInfo *tmp, char *name)
{
	VFILE *fd;
	void *p;
	char buf[256];
	char **a, *s;
	int i;

//	strcpy(buf, tmp->name);
	strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='.'))s--;
	if(*s!='.')s=buf+strlen(buf);
	strcpy(s, ".txt");

	fd=vffopen(buf, "rt");
	if(!fd)return(-1);

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "$flags"))
		{
		}

		if(!strcmp(a[0], "$vox_mins"))
		{
			tmp->mins[0]=atof(a[1]);
			tmp->mins[1]=atof(a[2]);
			tmp->mins[2]=atof(a[3]);
			continue;
		}

		if(!strcmp(a[0], "$vox_maxs"))
		{
			tmp->maxs[0]=atof(a[1]);
			tmp->maxs[1]=atof(a[2]);
			tmp->maxs[2]=atof(a[3]);
			continue;
		}

		if(!strcmp(a[0], "$vox_EmitBlock"))
			{ tmp->EmitBlock=dyllGetAddr(a[1]); }
		if(!strcmp(a[0], "$vox_BlockThink"))
			{ tmp->BlockThink=dyllGetAddr(a[1]); }
		if(!strcmp(a[0], "$vox_BlockUpdate"))
			{ tmp->BlockUpdate=dyllGetAddr(a[1]); }
		if(!strcmp(a[0], "$vox_SmallDamageEvent"))
			{ tmp->SmallDamageEvent=dyllGetAddr(a[1]); }
		if(!strcmp(a[0], "$vox_LargeDamageEvent"))
			{ tmp->LargeDamageEvent=dyllGetAddr(a[1]); }
		if(!strcmp(a[0], "$vox_UpdatePower"))
			{ tmp->UpdatePower=dyllGetAddr(a[1]); }
		if(!strcmp(a[0], "$vox_GetPower"))
			{ tmp->GetPower=dyllGetAddr(a[1]); }

		if(!strcmp(a[0], "$vox_GetWorldVoxelBBoxList"))
			{ tmp->GetWorldVoxelBBoxList=dyllGetAddr(a[1]); }

		if(!strcmp(a[0], "$vox_TouchEvent"))
			{ tmp->TouchEvent=dyllGetAddr(a[1]); }
		if(!strcmp(a[0], "$vox_RandomThink"))
			{ tmp->RandomThink=dyllGetAddr(a[1]); }

		if(!strcmp(a[0], "$vox_img_top"))
			{ tmp->img_top=BTGE_Texture_LoadImage(a[1]); }
		if(!strcmp(a[0], "$vox_img_side"))
			{ tmp->img_side=BTGE_Texture_LoadImage(a[1]); }
		if(!strcmp(a[0], "$vox_img_bot"))
			{ tmp->img_bot=BTGE_Texture_LoadImage(a[1]); }

		if(!strcmp(a[0], "$vox_img_scale"))
		{
			tmp->img_scale[0]=1.0/atof(a[1]);
			tmp->img_scale[1]=1.0/atof(a[1]);
			tmp->img_scale[2]=1.0/atof(a[1]);
		}

		if(!strcmp(a[0], "$vox_img_top_scale"))
			{ tmp->img_scale[0]=1.0/atof(a[1]); }
		if(!strcmp(a[0], "$vox_img_side_scale"))
			{ tmp->img_scale[1]=1.0/atof(a[1]); }
		if(!strcmp(a[0], "$vox_img_bot_scale"))
			{ tmp->img_scale[2]=1.0/atof(a[1]); }

		if(!strcmp(a[0], "$vox_blast_resist"))
			{ tmp->blast_resist=atof(a[1]); }

		if(!strcmp(a[0], "$vox_vlight_val"))
			{ tmp->vlight=(tmp->vlight&0x0F)|(atoi(a[1])<<4); }
		if(!strcmp(a[0], "$vox_vlight_clr"))
			{ tmp->vlight=(tmp->vlight&0xF0)|
				BTGE_Voxel_IndexVLightColor(a[1]); }
		if(!strcmp(a[0], "$vox_vlight_style"))
			{ tmp->vlight_style=BTGE_Voxel_IndexVLightStyle(a[1]); }

		if(!strcmp(a[0], "$vox_emit_sound"))
		{
			tmp->sound_name=dystrdup(a[1]);
			tmp->sound_vol=atof(a[2]);
			tmp->sound_attn=atoi(a[3]);
			tmp->flags2|=BTGE_TXFL2_EMITSSOUND;
		}
	}

	vfclose(fd);
	return(0);
}

BTGE_API int BTGE_Voxel_LoadVoxelType(int idx, char *name)
{
	char tb[256];
	BTGE_VoxelInfo *vinf;
	int i, j;

	if(!btge_voxinfo[idx])
	{
		vinf=gcalloc(sizeof(BTGE_VoxelInfo));
		BTGE_Voxel_SetInfoDefaults(vinf);
		btge_voxinfo[idx]=vinf;
	}else
	{
		vinf=btge_voxinfo[idx];
		if(!strcmp(vinf->name, name))
			return(0);

		//voxel has changed types
		BTGE_Voxel_SetInfoDefaults(vinf);
	}

	btge_voxel_opaque[idx]=1;	//for now, assume opaque
	vinf->name=dystrdup(name);

	sprintf(tb, "textures/voxel/%s", name);
	vinf->img_top=BTGE_Texture_LoadImage(tb);
	vinf->img_side=vinf->img_top;
	vinf->img_bot=vinf->img_top;

	if(vinf->img_top>0)
	{
		vinf->flags=BTGE_Texture_GetImageFlags(vinf->img_top);
		vinf->flags2=BTGE_Texture_GetImageFlags2(vinf->img_top);
		vinf->flags3=BTGE_Texture_GetImageFlags3(vinf->img_top);
		vinf->flags4=BTGE_Texture_GetImageFlags4(vinf->img_top);
	}

	BTGE_Voxel_LoadInfo(vinf, tb);

//	if(btge_voxinfo[i]->img_top<=0)
	if((vinf->img_top<=0) || (!vinf->flags && !vinf->flags2))
	{
		sprintf(tb, "textures/voxel/%s_top", name);
		vinf->img_top=BTGE_Texture_LoadImage(tb);
		BTGE_Voxel_LoadInfo(vinf, tb);

		vinf->flags=BTGE_Texture_GetImageFlags(vinf->img_top);
		vinf->flags2=BTGE_Texture_GetImageFlags2(vinf->img_top);
		vinf->flags3=BTGE_Texture_GetImageFlags3(vinf->img_top);
		vinf->flags4=BTGE_Texture_GetImageFlags4(vinf->img_top);

		sprintf(tb, "textures/voxel/%s_side", name);
		vinf->img_side=BTGE_Texture_LoadImage(tb);
		sprintf(tb, "textures/voxel/%s_bot", name);
		vinf->img_bot=BTGE_Texture_LoadImage(tb);

		if(!vinf->img_bot)
			vinf->img_bot=vinf->img_top;

		if(vinf->flags2&BTGE_TXFL2_AUXVARIANT)
		{
			for(i=1; i<8; i++)
			{
				sprintf(tb, "textures/voxel/%s_top%d", name, i);
				vinf->img_auxvar_top[i]=BTGE_Texture_LoadImage(tb);
				sprintf(tb, "textures/voxel/%s_side%d", name, i);
				vinf->img_auxvar_side[i]=BTGE_Texture_LoadImage(tb);
				sprintf(tb, "textures/voxel/%s_bot%d", name, i);
				vinf->img_auxvar_bot[i]=BTGE_Texture_LoadImage(tb);
			}
		}
	}else
	{
		if(vinf->flags2&BTGE_TXFL2_AUXVARIANT)
		{
			for(i=1; i<8; i++)
			{
				sprintf(tb, "textures/voxel/%s_aux%d", name, i);
				j=BTGE_Texture_LoadImage(tb);
				vinf->img_auxvar_top[i]=j;
				vinf->img_auxvar_side[i]=j;
				vinf->img_auxvar_bot[i]=j;
			}
		}
	}

	if(vinf->img_top>0)
	{
		vinf->flags|=BTGE_Texture_GetImageFlags(vinf->img_top);
		vinf->flags2|=BTGE_Texture_GetImageFlags2(vinf->img_top);
		vinf->flags3|=BTGE_Texture_GetImageFlags3(vinf->img_top);
		vinf->flags4|=BTGE_Texture_GetImageFlags4(vinf->img_top);
	}
	if(vinf->img_side>0)
	{
		vinf->flags|=BTGE_Texture_GetImageFlags(vinf->img_side);
		vinf->flags2|=BTGE_Texture_GetImageFlags2(vinf->img_side);
		vinf->flags3|=BTGE_Texture_GetImageFlags3(vinf->img_side);
		vinf->flags4|=BTGE_Texture_GetImageFlags4(vinf->img_side);
	}
	
	return(0);
}

void BTGE_Voxel_GlobalThink(BTGE_BrushWorld *world)
{
	btge_voxel_use_flexform=
		BTGE_CvarGetNum("vox_flexform");
}

void BTGE_Voxel_Setup()
{
	static int init=0;
	char tb[256];
	BTGE_VoxelInfo *vinf;
	char *name;
	int i;
	
	if(init)return;
	init=1;
	
	btge_voxel_mutex=thFastMutex();
	
	btCvarSetDefault("vox_explosivedamage", "0");
	btCvarSetDefault("vox_savedisabled", "1");
	
	BTGE_CvarSetDefault("vox_flexform", "1");
	btge_voxel_use_flexform=
		BTGE_CvarGetNum("vox_flexform");
	
	for(i=0; i<256; i++)
	{
		btge_voxel_opaque[i]=0;	//for now, assume transparent
		if(!btge_voxnames[i])
			break;

		name=btge_voxnames[i];
		BTGE_Voxel_LoadVoxelType(i, name);
	}

	for(i=0; i<256; i++)
	{
		btge_voxel_opaque[256+i]=0;	//for now, assume transparent
		if(!btge_voxnames2[i])
			break;

		name=btge_voxnames2[i];
		BTGE_Voxel_LoadVoxelType(256+i, name);
	}

	btge_voxel_opaque[0]=0;	//air is not opaque

	btge_voxinfo[VOX_TY_WATER]->flags|=BTGE_TXFL_FLUID_WATER;
	btge_voxinfo[VOX_TY_WATER2]->flags|=BTGE_TXFL_FLUID_WATER;
	btge_voxinfo[VOX_TY_LAVA]->flags|=BTGE_TXFL_FLUID_LAVA;
	btge_voxinfo[VOX_TY_LAVA2]->flags|=BTGE_TXFL_FLUID_LAVA;

	btge_voxinfo[VOX_TY_SLIME]->flags|=BTGE_TXFL_FLUID_SLIME;
	btge_voxinfo[VOX_TY_SLIME2]->flags|=BTGE_TXFL_FLUID_SLIME;
	btge_voxinfo[VOX_TY_GLOWSLIME]->flags|=BTGE_TXFL_FLUID_SLIME;
	btge_voxinfo[VOX_TY_GLOWSLIME2]->flags|=BTGE_TXFL_FLUID_SLIME;

	for(i=0; i<4096; i++)
	{
		if(!btge_voxinfo[i])
			continue;
		if(btge_voxinfo[i]->flags&BTGE_TXFL_ALPHA)
			btge_voxel_opaque[i]=0;	//ok, not opaque
		if(btge_voxinfo[i]->flags2&BTGE_TXFL2_PARTIALBLOCK)
			btge_voxel_opaque[i]=0;	//not opaque
	}
	
	BTGE_Voxel_SetupDungeon();
}

BTGE_API int BTGE_Voxel_IndexVoxelName(char *name)
{
	int i;

	BTGE_Voxel_Setup();

	if((*name>='0') && (*name<='9'))
		return(atoi(name));
		
	for(i=0; btge_voxnames[i]; i++)
		if(!strcmp(btge_voxnames[i], name))
			return(i);

	for(i=0; i<4096; i++)
	{
		if(btge_voxinfo[i] && btge_voxinfo[i]->name)
		{
			if(!strcmp(btge_voxinfo[i]->name, name))
				return(i);
		}
	}

	return(-1);
}

BTGE_API char *BTGE_Voxel_GetNameForVoxelIndex(int idx)
{
	BTGE_Voxel_Setup();

	if(!btge_voxinfo[idx])
		return(NULL);
	return(btge_voxinfo[idx]->name);
}

#if 1
BTGE_API bool BTGE_Voxel_VoxelOpaqueP(BTGE_VoxelData *vox)
{
	if(!vox)return(false);
	if(!vox->type)
		return(0);
	return(btge_voxel_opaque[vox->type&4095]);
//	return(1);
}

BTGE_API bool BTGE_Voxel_VoxelFluidP(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);
	if(inf->flags&BTGE_TXFL_FLUID)
		return(true);
	return(false);
}

BTGE_API bool BTGE_Voxel_VoxelFluidOverlayP(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);
	if(inf->flags&BTGE_TXFL_FLUID)
	{
		if(inf->flags2&BTGE_TXFL2_NOWATEROVERLAY)
			return(false);
		return(true);
	}
	return(false);
}

BTGE_API bool BTGE_Voxel_VoxelAlphaP(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);
	if(inf->flags&BTGE_TXFL_ALPHA)
		return(true);
	return(false);
}

BTGE_API bool BTGE_Voxel_VoxelNoShadowP(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);
	if(inf->flags&BTGE_TXFL_NOSHADOW)
		return(true);
	return(false);
}

BTGE_API bool BTGE_Voxel_VoxelSolidP(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);

	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);

//	if(BTGE_Voxel_VoxelFluidP(vox))
//		return(false);
	if(inf->flags&BTGE_TXFL_FLUID)
		return(false);
	if(inf->flags&BTGE_TXFL_NONSOLID)
		return(false);
	
	return(true);

//	return(btge_voxel_opaque[vox->type]);
//	return(1);
}

BTGE_API bool BTGE_Voxel_VoxelAlphaNonMatP(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);
	if((inf->flags&BTGE_TXFL_ALPHA) && !(inf->flags&BTGE_TXFL_LIGHTMAT))
		return(true);
	return(false);
}

BTGE_API bool BTGE_Voxel_VoxelFlexformP(BTGE_VoxelData *vox)
	{ return((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM); }

BTGE_API int BTGE_Voxel_GetVoxelFlags(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags);
}

BTGE_API int BTGE_Voxel_GetVoxelFlags2(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags2);
}

BTGE_API int BTGE_Voxel_GetVoxelFlags3(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags3);
}

BTGE_API int BTGE_Voxel_GetVoxelFlags4(BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags4);
}

BTGE_API BTGE_VoxelInfo *BTGE_Voxel_GetVoxelInfo(BTGE_VoxelData *vox)
{
	if(!vox)return(NULL);
	return(btge_voxinfo[(vox->type)&4095]);
}
#endif

#if 1
BTGE_API bool BTGE_Voxel_VoxelDataOpaqueP(BTGE_VoxelData vox)
{
	if(!vox.type)
		return(0);
	return(btge_voxel_opaque[vox.type&4095]);
//	return(1);
}
#endif

BTGE_API bool BTGE_Voxel_VoxelTypeNoFlexformP(
	BTGE_BrushWorld *world, int type)
{
	if(btge_voxinfo[type&4095] &&
		btge_voxinfo[type&4095]->flags2&BTGE_TXFL2_NOFLEXFORM)
			return(true);
	return(false);
}

BTGE_API BTGE_VoxelInfo *BTGE_Voxel_GetWorldVoxelInfo(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z)
{
	if(!vox)return(NULL);
	return(btge_voxinfo[(vox->type)&4095]);
}

BTGE_API BTGE_VoxelInfo *BTGE_Voxel_GetWorldChunkVoxelInfo(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if(!vox)return(NULL);
	return(btge_voxinfo[(vox->type)&4095]);
}

#if 1
BTGE_API bool BTGE_Voxel_WorldChunkVoxelOpaqueP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if(!vox)return(false);
	if(!vox->type)
		return(0);
	return(btge_voxel_opaque[vox->type&4095]);
//	return(1);
}

BTGE_API bool BTGE_Voxel_WorldChunkVoxelFluidP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)

{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);
	if(inf->flags&BTGE_TXFL_FLUID)
		return(true);
	return(false);
}

BTGE_API bool BTGE_Voxel_WorldChunkVoxelAlphaP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);
	if(inf->flags&BTGE_TXFL_ALPHA)
		return(true);
	return(false);
}

BTGE_API bool BTGE_Voxel_WorldChunkVoxelSolidP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(false);
	if(!vox->type)return(false);

	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(false);

//	if(BTGE_Voxel_VoxelFluidP(vox))
//		return(false);
	if(inf->flags&BTGE_TXFL_FLUID)
		return(false);
	if(inf->flags&BTGE_TXFL_NONSOLID)
		return(false);
	
	return(true);

//	return(btge_voxel_opaque[vox->type]);
//	return(1);
}

BTGE_API bool BTGE_Voxel_WorldChunkVoxelFlexformP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	return((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM);
}

BTGE_API int BTGE_Voxel_GetWorldChunkVoxelFlags(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags);
}

BTGE_API int BTGE_Voxel_GetWorldChunkVoxelFlags2(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags2);
}

BTGE_API int BTGE_Voxel_GetWorldChunkVoxelFlags3(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags3);
}

BTGE_API int BTGE_Voxel_GetWorldChunkVoxelFlags4(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelInfo *inf;
	if(!vox)return(0);
	if(!vox->type)return(0);
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)return(0);
	return(inf->flags4);
}

#if 0
// BTGE_API BTGE_VoxelInfo *BTGE_Voxel_GetWorldChunkVoxelInfo(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if(!vox)return(NULL);
	return(btge_voxinfo[(vox->type)&4095]);
}
#endif

#endif


BTGE_API void BTGE_Voxel_FlushChunk(BTGE_VoxelChunk *chk)
{
	chk->clean=0;
}

BTGE_API void BTGE_Voxel_FlushReleaseChunk(BTGE_VoxelChunk *chk)
{
	chk->clean=0;
	
	BTGE_VoxelESN2_FlushChunk(NULL, chk);
	BTGE_VoxelProps_FlushReleaseChunk(chk);
	
	if(chk->mesh && chk->mesh->face_xyz)
	{
		gcfree(chk->mesh->face_xyz);
		gcfree(chk->mesh->face_st);
		gcfree(chk->mesh->face_norm);
		gcfree(chk->mesh->face_norm_x);
		gcfree(chk->mesh->face_norm_y);
		gcfree(chk->mesh->face_rgba);
		gcfree(chk->mesh->face_slrgba);
		gcfree(chk->mesh->face_vlrgba);
		gcfree(chk->mesh->face_tex);
		gcfree(chk->mesh->face_vox);

		chk->mesh->face_xyz=NULL;
		chk->mesh->face_st=NULL;
		chk->mesh->face_norm=NULL;
		chk->mesh->face_norm_x=NULL;
		chk->mesh->face_norm_y=NULL;
		chk->mesh->face_rgba=NULL;
		chk->mesh->face_slrgba=NULL;
		chk->mesh->face_vlrgba=NULL;
		chk->mesh->face_tex=NULL;
		chk->mesh->face_vox=NULL;
	}

	if(chk->mesh && chk->mesh->aface_xyz)
	{
		gcfree(chk->mesh->aface_xyz);
		gcfree(chk->mesh->aface_st);
		gcfree(chk->mesh->aface_norm);
		gcfree(chk->mesh->aface_rgba);
		gcfree(chk->mesh->aface_slrgba);
		gcfree(chk->mesh->aface_vlrgba);
		gcfree(chk->mesh->aface_tex);
		gcfree(chk->mesh->aface_vox);

		chk->mesh->aface_xyz=NULL;
		chk->mesh->aface_st=NULL;
		chk->mesh->aface_norm=NULL;
		chk->mesh->aface_rgba=NULL;
		chk->mesh->aface_slrgba=NULL;
		chk->mesh->aface_vlrgba=NULL;
		chk->mesh->aface_tex=NULL;
		chk->mesh->aface_vox=NULL;
	}

	if(chk->mesh && chk->mesh->mesh_xyz)
	{
		if(chk->mesh->mesh_xyz)gcfree(chk->mesh->mesh_xyz);
		if(chk->mesh->mesh_norm)gcfree(chk->mesh->mesh_norm);
		if(chk->mesh->mesh_edge)gcfree(chk->mesh->mesh_edge);
		if(chk->mesh->mesh_pts)gcfree(chk->mesh->mesh_pts);
		if(chk->mesh->mesh_face)gcfree(chk->mesh->mesh_face);
		
		chk->mesh->mesh_xyz=NULL;
		chk->mesh->mesh_norm=NULL;
		chk->mesh->mesh_edge=NULL;
		chk->mesh->mesh_pts=NULL;
		chk->mesh->mesh_face=NULL;
	}

	if(chk->mesh && chk->mesh->face_tlrgba)
	{
		gcfree(chk->mesh->face_tlrgba);
		chk->mesh->face_tlrgba=NULL;
	}

	if(chk->mesh && chk->mesh->aface_tlrgba)
	{
		gcfree(chk->mesh->aface_tlrgba);
		chk->mesh->aface_tlrgba=NULL;
	}
	
	if(chk->mesh)
	{
		gcfree(chk->mesh);
		chk->mesh=NULL;
	}
}

void BTGE_Voxel_CheckExpandFace(BTGE_VoxelChunk *chk)
{
	int i, sz;

	sz=3;

	if(!chk->mesh)
	{
		chk->mesh=gctalloc("btge_voxelchunkmesh_t",
			sizeof(BTGE_VoxelChunkMesh));
	}

	if(!chk->mesh->face_xyz)
	{
//		i=64;
//		i=256;
		i=384;
		chk->mesh->face_xyz=gctatomic("btge_chunkface_xyz",
			i*sz*3*sizeof(float));
		chk->mesh->face_st=gctatomic("btge_chunkface_st",
			i*sz*2*sizeof(float));
#ifndef BTGE_VOXEL_BYTERGBA
		chk->mesh->face_norm=gctatomic("btge_chunkface_norm",
			i*sz*3*sizeof(float));
		chk->mesh->face_rgba=gctatomic("btge_chunkface_rgba",
			i*sz*4*sizeof(float));
		chk->mesh->face_slrgba=gctatomic("btge_chunkface_slrgba",
			i*sz*4*sizeof(float));
		chk->mesh->face_vlrgba=gctatomic("btge_chunkface_vlrgba",
			i*sz*4*sizeof(float));
#else
		chk->mesh->face_norm=gctatomic("btge_chunkface_norm",
			i*sz*3*sizeof(sbyte));
		chk->mesh->face_norm_x=gctatomic("btge_chunkface_normx",
			i*sz*3*sizeof(sbyte));
		chk->mesh->face_norm_y=gctatomic("btge_chunkface_normy",
			i*sz*3*sizeof(sbyte));
		chk->mesh->face_rgba=gctatomic("btge_chunkface_rgba",
			i*sz*4*sizeof(byte));
		chk->mesh->face_slrgba=gctatomic("btge_chunkface_slrgba",
			i*sz*4*sizeof(byte));
		chk->mesh->face_vlrgba=gctatomic("btge_chunkface_vlrgba",
			i*sz*4*sizeof(byte));
#endif
		chk->mesh->face_tex=gctatomic("btge_chunkface_tex",
			i*1*1*sizeof(int));
//		chk->mesh->face_vox=gcatomic(i*1*1*sizeof(BTGE_VoxelData *));
		chk->mesh->face_vox=gctatomic("btge_chunkface_vox",
			i*1*1*sizeof(BTGE_VoxelData));
		chk->mesh->n_face=0;
		chk->mesh->m_face=i;
	}
	
	if((chk->mesh->n_face+2)>=chk->mesh->m_face)
	{
		i=chk->mesh->m_face+(chk->mesh->m_face>>1);
		chk->mesh->face_xyz=gcrealloc(chk->mesh->face_xyz,
			i*sz*3*sizeof(float));
		chk->mesh->face_st=gcrealloc(chk->mesh->face_st,
			i*sz*2*sizeof(float));
#ifndef BTGE_VOXEL_BYTERGBA
		chk->mesh->face_norm=gcrealloc(chk->mesh->face_norm,
			i*sz*3*sizeof(float));
		chk->mesh->face_rgba=gcrealloc(chk->mesh->face_rgba,
			i*sz*4*sizeof(float));
		chk->mesh->face_slrgba=gcrealloc(chk->mesh->face_slrgba,
			i*sz*4*sizeof(float));
		chk->mesh->face_vlrgba=gcrealloc(chk->mesh->face_vlrgba,
			i*sz*4*sizeof(float));

		if(chk->mesh->face_tlrgba)
		{
			chk->mesh->face_tlrgba=gcrealloc(chk->mesh->face_tlrgba,
				i*sz*4*sizeof(float));
		}
#else
		chk->mesh->face_norm=gcrealloc(chk->mesh->face_norm,
			i*sz*3*sizeof(sbyte));
		chk->mesh->face_norm_x=gcrealloc(chk->mesh->face_norm_x,
			i*sz*3*sizeof(sbyte));
		chk->mesh->face_norm_y=gcrealloc(chk->mesh->face_norm_y,
			i*sz*3*sizeof(sbyte));
		chk->mesh->face_rgba=gcrealloc(chk->mesh->face_rgba,
			i*sz*4*sizeof(byte));
		chk->mesh->face_slrgba=gcrealloc(chk->mesh->face_slrgba,
			i*sz*4*sizeof(byte));
		chk->mesh->face_vlrgba=gcrealloc(chk->mesh->face_vlrgba,
			i*sz*4*sizeof(byte));

		if(chk->mesh->face_tlrgba)
		{
			chk->mesh->face_tlrgba=gcrealloc(chk->mesh->face_tlrgba,
				i*sz*4*sizeof(byte));
		}
#endif
		chk->mesh->face_tex=gcrealloc(chk->mesh->face_tex,
			i*1*1*sizeof(int));
//		chk->mesh->face_vox=gcrealloc(chk->mesh->face_vox,
//			i*1*1*sizeof(BTGE_VoxelData *));
		chk->mesh->face_vox=gcrealloc(chk->mesh->face_vox,
			i*1*1*sizeof(BTGE_VoxelData));
		chk->mesh->m_face=i;
	}
}

void BTGE_Voxel_CheckExpandAlphaFace(BTGE_VoxelChunk *chk)
{
	int i, sz;

	sz=3;

	if(!chk->mesh)
	{
		chk->mesh=gctalloc("btge_voxelchunkmesh_t",
			sizeof(BTGE_VoxelChunkMesh));
	}

	if(!chk->mesh->aface_xyz)
	{
		i=256;
		chk->mesh->aface_xyz=gctatomic("btge_chunkaface_xyz",
			i*sz*3*sizeof(float));
		chk->mesh->aface_st=gctatomic("btge_chunkaface_st",
			i*sz*2*sizeof(float));
#ifndef BTGE_VOXEL_BYTERGBA
		chk->mesh->aface_norm=gctatomic("btge_chunkaface_norm",
			i*sz*3*sizeof(float));
		chk->mesh->aface_rgba=gctatomic("btge_chunkaface_rgba",
			i*sz*4*sizeof(float));
		chk->mesh->aface_slrgba=gctatomic("btge_chunkaface_slrgba",
			i*sz*4*sizeof(float));
		chk->mesh->aface_vlrgba=gctatomic("btge_chunkaface_vlrgba",
			i*sz*4*sizeof(float));
#else
		chk->mesh->aface_norm=gctatomic("btge_chunkaface_norm",
			i*sz*3*sizeof(sbyte));
		chk->mesh->aface_rgba=gctatomic("btge_chunkaface_rgba",
			i*sz*4*sizeof(byte));
		chk->mesh->aface_slrgba=gctatomic("btge_chunkaface_slrgba",
			i*sz*4*sizeof(byte));
		chk->mesh->aface_vlrgba=gctatomic("btge_chunkaface_vlrgba",
			i*sz*4*sizeof(byte));
#endif
		chk->mesh->aface_tex=gctatomic("btge_chunkaface_tex",
			i*1*1*sizeof(int));
//		chk->mesh->aface_vox=gcatomic(i*1*1*sizeof(BTGE_VoxelData *));
		chk->mesh->aface_vox=gctatomic("btge_chunkaface_vox",
			i*1*1*sizeof(BTGE_VoxelData));
		chk->mesh->n_aface=0;
		chk->mesh->m_aface=i;
	}
	
	if((chk->mesh->n_aface+2)>=chk->mesh->m_aface)
	{
		i=chk->mesh->m_aface+(chk->mesh->m_aface>>1);
		chk->mesh->aface_xyz=gcrealloc(chk->mesh->aface_xyz,
			i*sz*3*sizeof(float));
		chk->mesh->aface_st=gcrealloc(chk->mesh->aface_st,
			i*sz*2*sizeof(float));
#ifndef BTGE_VOXEL_BYTERGBA
		chk->mesh->aface_norm=gcrealloc(chk->mesh->aface_norm,
			i*sz*3*sizeof(float));
		chk->mesh->aface_rgba=gcrealloc(chk->mesh->aface_rgba,
			i*sz*4*sizeof(float));
		chk->mesh->aface_slrgba=gcrealloc(chk->mesh->aface_slrgba,
			i*sz*4*sizeof(float));
		chk->mesh->aface_vlrgba=gcrealloc(chk->mesh->aface_vlrgba,
			i*sz*4*sizeof(float));

		if(chk->mesh->aface_tlrgba)
		{
			chk->mesh->aface_tlrgba=gcrealloc(chk->mesh->aface_tlrgba,
				i*sz*4*sizeof(float));
		}
#else
		chk->mesh->aface_norm=gcrealloc(chk->mesh->aface_norm,
			i*sz*3*sizeof(sbyte));
		chk->mesh->aface_rgba=gcrealloc(chk->mesh->aface_rgba,
			i*sz*4*sizeof(byte));
		chk->mesh->aface_slrgba=gcrealloc(chk->mesh->aface_slrgba,
			i*sz*4*sizeof(byte));
		chk->mesh->aface_vlrgba=gcrealloc(chk->mesh->aface_vlrgba,
			i*sz*4*sizeof(byte));

		if(chk->mesh->aface_tlrgba)
		{
			chk->mesh->aface_tlrgba=gcrealloc(chk->mesh->aface_tlrgba,
				i*sz*4*sizeof(byte));
		}
#endif
		chk->mesh->aface_tex=gcrealloc(chk->mesh->aface_tex,
			i*1*1*sizeof(int));
//		chk->mesh->aface_vox=gcrealloc(chk->mesh->aface_vox,
//			i*1*1*sizeof(BTGE_VoxelData *));
		chk->mesh->aface_vox=gcrealloc(chk->mesh->aface_vox,
			i*1*1*sizeof(BTGE_VoxelData));
		chk->mesh->m_aface=i;
	}
}

BTGE_API float BTGE_Voxel_GetVoxelGamma(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox)
{
	float f;
	int i;

	if(!vox)return(1.0);
//	if(!vox->type)return(1.0);
//	return(btge_voxinfo[vox->type&4095]->flags4);

//	f=(vox->slight&0xF0)/240.0;

	i=(vox->slight>>4)&15;
	f=pow(0.80, 15-i);
	return(f);
}

BTGE_API void BTGE_Voxel_RebuildChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox;
	BTGE_Light *lcur, *lnxt;
	float org[3], mins[3], maxs[3];
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l, n, nvox, ntr;

	BTGE_Voxel_Setup();

	if(chk->flags&VOX_CHFL_NOVISBLOCKS)
		return;

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

//	chk->tickdirty=1;	//wake chunk up
	chk->tickthink=1;	//wake chunk up

	if(chk->mesh)
	{
		chk->mesh->n_face=0;
		chk->mesh->n_aface=0;
		nvox=0;
	
		if(chk->mesh->light)
		{
			lcur=chk->mesh->light;
			while(lcur)
			{
				lnxt=lcur->next;
				gcfree(lcur);
				lcur=lnxt;
			}
			chk->mesh->light=NULL;
		}
	}

#if 1
	if(chk->vox)
	{
		n=chk->xs*chk->ys*chk->zs;
		for(i=0; i<n; i++)
		{
			vox=chk->vox+i;
			if(!(vox->type&VOX_TY_MASK))
				break;
			if(!BTGE_Voxel_VoxelOpaqueP(vox))
				break;
		}
		
		ntr=(i>=n)?0:1;
		
#if 0
		if(i>=n)
		{
			chk->flags|=VOX_CHFL_NOVISBLOCKS;
			chk->tick_inhibit_voxrle=0;

			if(chk->voxrle)
			{
				if(chk->vox)
					gcfree(chk->vox);
				chk->vox=NULL;
			}
//			btge_voxel_unlock();
			return;
		}
#endif
	}else if(chk->pidx_vox)
	{
		n=chk->pidx_n_vox;
		for(i=0; i<n; i++)
		{
			vox=chk->pidx_vox+i;
			if(!(vox->type&VOX_TY_MASK))
				break;
			if(!BTGE_Voxel_VoxelOpaqueP(vox))
				break;
		}
		
		ntr=(i>=n)?0:1;
	}	
#endif

#if 0
	ntr=0;
	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
			for(k=0; k<chk->zs; k++)
	{
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+k;

//		vox=BTGE_Voxel_GetChunkVoxelFast(chk, i, j, k);
		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		if(!vox)break;
		if(!(vox->type&VOX_TY_MASK))
			{ ntr++; continue; }
		if(!BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
			ntr++;
	}
#endif

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
			for(k=0; k<chk->zs; k++)
	{
		l=0;
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+k;

#if 1
		if(!ntr)
		{
//			if((i>0) && ((i+1)<chk->xs))
//				continue;
//			if((j>0) && ((j+1)<chk->ys))
//				continue;
//			if((k>0) && ((k+1)<chk->zs))
//				continue;
			if(	((i>0) && ((i+1)<chk->xs)) &&
				((j>0) && ((j+1)<chk->ys)) &&
				((k>0) && ((k+1)<chk->zs)) )
					continue;
		}
#endif

		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		if(!vox)continue;
		if(!(vox->type&VOX_TY_MASK))
			continue;

#if 1
		if((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM)
		{
			if(BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
				{ continue; }
		}
#endif

//		if(BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
		if(0)
		{
#if 0
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y, z))l|=1;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y, z))l|=2;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y-1, z))l|=4;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y+1, z))l|=8;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y, z-1))l|=16;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y, z+1))l|=32;
#endif
#if 1
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y, z))l|=VOX_FL_NX;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y, z))l|=VOX_FL_PX;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y-1, z))l|=VOX_FL_NY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y+1, z))l|=VOX_FL_PY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y, z-1))l|=VOX_FL_NZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y, z+1))l|=VOX_FL_PZ;
#endif

			if(!z && ((vox->type&VOX_TY_MASK)==VOX_TY_BEDROCK))
				l&=~16;

			if(!l)continue;
		}else
		{
//			BTGE_Voxel_RegionVoxelSkipFaceP()

#if 0
			if(!BTGE_Voxel_RegionVoxelSkipFaceP(chk->rgn,
				vox, x-1, y, z))l|=1;
			if(!BTGE_Voxel_RegionVoxelSkipFaceP(chk->rgn,
				vox, x+1, y, z))l|=2;
			if(!BTGE_Voxel_RegionVoxelSkipFaceP(chk->rgn,
				vox, x, y-1, z))l|=4;
			if(!BTGE_Voxel_RegionVoxelSkipFaceP(chk->rgn,
				vox, x, y+1, z))l|=8;
			if(!BTGE_Voxel_RegionVoxelSkipFaceP(chk->rgn,
				vox, x, y, z-1))l|=16;
			if(!BTGE_Voxel_RegionVoxelSkipFaceP(chk->rgn,
				vox, x, y, z+1))l|=32;
#endif
#if 1
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x-1, y, z))l|=VOX_FL_NX;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x+1, y, z))l|=VOX_FL_PX;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y-1, z))l|=VOX_FL_NY;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y+1, z))l|=VOX_FL_PY;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y, z-1))l|=VOX_FL_NZ;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y, z+1))l|=VOX_FL_PZ;
#endif

			if(BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
			{
				if(!z && ((vox->type&VOX_TY_MASK)==VOX_TY_BEDROCK))
					l&=~16;
				if(!l)continue;
			}

//			if(!l)continue;
		}

#if 0
//		if((vox->type==VOX_TY_GRASS) || (vox->type==VOX_TY_DIRT))
//		if(0)
		if((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM)
		{
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y, z+1))l|=VOX_FL_NXPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y, z+1))l|=VOX_FL_PXPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y-1, z+1))l|=VOX_FL_NYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y+1, z+1))l|=VOX_FL_PYPZ;

			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y+1, z))l|=VOX_FL_NXPY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y-1, z))l|=VOX_FL_PXNY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y+1, z))l|=VOX_FL_PXPY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y-1, z))l|=VOX_FL_NXNY;

			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y+1, z+1))l|=VOX_FL_NXPYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y-1, z+1))l|=VOX_FL_PXNYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y+1, z+1))l|=VOX_FL_PXPYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y-1, z+1))l|=VOX_FL_NXNYPZ;
		}
#endif

//		org[0]=x*vsz; org[1]=y*vsz; org[2]=z*vsz;
//		mins[0]=org[0]-vsz2; mins[1]=org[1]-vsz2; mins[2]=org[2]-vsz2;
//		maxs[0]=org[0]+vsz2; maxs[1]=org[1]+vsz2; maxs[2]=org[2]+vsz2;

//		org[0]=x*vsz; org[1]=y*vsz; org[2]=z*vsz;

		org[0]=(x-chk->rgn->ox)*vsz;
		org[1]=(y-chk->rgn->oy)*vsz;
		org[2]=(z-chk->rgn->oz)*vsz;

		mins[0]=org[0]; mins[1]=org[1]; mins[2]=org[2];
		maxs[0]=org[0]+vsz; maxs[1]=org[1]+vsz; maxs[2]=org[2]+vsz;

		BTGE_Voxel_EmitBlock(world, chk, vox, mins, maxs, l, x, y, z);
		
		nvox++;
	}
	
//	BTGE_VoxelESN_RebuildChunk(world, chk);
	BTGE_VoxelESN2_RebuildChunk(world, chk);
	
	BTGE_Voxel_RebuildSortFaces(chk);
	
	btge_voxel_lock();
	if(!chk->mesh)
	{
		chk->flags|=VOX_CHFL_NOVISBLOCKS;
		chk->tick_inhibit_voxrle=0;

		if(chk->voxrle)
		{
			if(chk->vox)
				{ gcfree(chk->vox); chk->vox=NULL; }
			if(chk->pidx_vox)
				{ gcfree(chk->pidx_vox); chk->pidx_vox=NULL; }
			if(chk->pidx_idx)
				{ gcfree(chk->pidx_idx); chk->pidx_idx=NULL; }
		}
	}
	btge_voxel_unlock();
	
//	printf("BTGE_Voxel_RebuildChunk: %d vox, %d face %d aface\n",
//		nvox, chk->mesh->n_face, chk->mesh->n_aface);
}

BTGE_API void BTGE_Voxel_CheckRebuildChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	if(chk->clean)return;
	BTGE_Voxel_RebuildChunk(world, chk);
	chk->clean=1;
}

BTGE_API void BTGE_Voxel_BoxChunk(BTGE_VoxelChunk *chk,
	float *mins, float *maxs)
{
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l;

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

	mins[0]=chk->ox*vsz-vsz2;
	mins[1]=chk->oy*vsz-vsz2;
	mins[2]=chk->oz*vsz-vsz2;
	maxs[0]=mins[0]+chk->xs*vsz;
	maxs[1]=mins[1]+chk->ys*vsz;
	maxs[2]=mins[2]+chk->zs*vsz;
}

BTGE_API void BTGE_Voxel_BoxRegion(BTGE_VoxelRegion *rgn,
	float *mins, float *maxs)
{
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l;

	vsz=rgn->voxsz;
	vsz2=vsz*0.5;

	mins[0]=rgn->ox*vsz-vsz2;
	mins[1]=rgn->oy*vsz-vsz2;
	mins[2]=rgn->oz*vsz-vsz2;
	maxs[0]=mins[0]+(rgn->xs*rgn->xst)*vsz;
	maxs[1]=mins[1]+(rgn->ys*rgn->yst)*vsz;
	maxs[2]=mins[2]+(rgn->zs*rgn->zst)*vsz;
}

BTGE_API bool BTGE_Voxel_CheckChunkFrustum(BTGE_VoxelChunk *chk)
{
	float mins[3], maxs[3];
	BTGE_Voxel_BoxChunk(chk, mins, maxs);
//	if(!BTGE_Brush_CheckBBoxFrustum(mins, maxs))
//		return(false);
	return(true);
}

BTGE_API bool BTGE_Voxel_CheckRegionFrustum(BTGE_VoxelRegion *rgn)
{
	float mins[3], maxs[3];
	BTGE_Voxel_BoxRegion(rgn, mins, maxs);
//	if(!BTGE_Brush_CheckBBoxFrustum(mins, maxs))
//		return(false);
	return(true);
}

BTGE_API bool BTGE_Voxel_CheckChunkLight(
	BTGE_VoxelChunk *chk, BTGE_Light *light)
{
	float mins[3], maxs[3];
	BTGE_Voxel_BoxChunk(chk, mins, maxs);
//	if(!BTGE_Brush_CheckBBoxFrustum(mins, maxs))
//		return(false);
	if(!SolidAABB_BoxCollideP(mins, maxs, light->amins, light->amaxs))
		return(false);
	return(true);
}

BTGE_API bool BTGE_Voxel_CheckRegionLight(
	BTGE_VoxelRegion *rgn, BTGE_Light *light)
{
	float mins[3], maxs[3];
	BTGE_Voxel_BoxRegion(rgn, mins, maxs);
//	if(!BTGE_Brush_CheckBBoxFrustum(mins, maxs))
//		return(false);
	if(!SolidAABB_BoxCollideP(mins, maxs, light->amins, light->amaxs))
		return(false);
	return(true);
}

BTGE_API bool BTGE_Voxel_RegionVoxelOpaqueP(BTGE_VoxelRegion *rgn,
	int x, int y, int z)
{
	BTGE_VoxelData *vox;
	vox=BTGE_Voxel_GetRegionVoxel(rgn, x, y, z);
	if(!vox)return(false);
	return(BTGE_Voxel_VoxelOpaqueP(vox));
}

BTGE_API bool BTGE_Voxel_RegionVoxelSkipFaceP(BTGE_VoxelRegion *rgn,
	BTGE_VoxelData *vox,
	int x, int y, int z)
{
	BTGE_VoxelData *vox2;

//	vox2=BTGE_Voxel_GetRegionVoxel(rgn, x, y, z);
	vox2=BTGE_Voxel_GetRegionVoxel(rgn, x, y, z);

	if(!vox2)return(false);
	if(BTGE_Voxel_VoxelOpaqueP(vox2))
		return(true);
	if(BTGE_Voxel_VoxelOpaqueP(vox))
		return(false);

	if(BTGE_Voxel_VoxelFluidP(vox))
	{
		if(BTGE_Voxel_VoxelFluidP(vox2))
			return(true);
	}

	if(BTGE_Voxel_VoxelAlphaP(vox))
	{
		if(BTGE_Voxel_VoxelAlphaP(vox2))
			return(true);
	}
	
	if(vox->type==vox2->type)
		return(true);

	return(false);
}

BTGE_API bool BTGE_Voxel_WorldRegionVoxelOpaqueP(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, int z)
{
	BTGE_VoxelData *vox;
//	vox=BTGE_Voxel_GetRegionVoxel(rgn, x, y, z);
	vox=BTGE_Voxel_GetWorldRegionVoxel(world, rgn, x, y, z);
	if(!vox)return(false);
	return(BTGE_Voxel_VoxelOpaqueP(vox));
}

BTGE_API bool BTGE_Voxel_WorldRegionVoxelSkipFaceP(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	BTGE_VoxelData *vox,
	int x, int y, int z)
{
	BTGE_VoxelData *vox2, *vox3;
//	vox2=BTGE_Voxel_GetRegionVoxel(rgn, x, y, z);
	vox2=BTGE_Voxel_GetWorldRegionVoxel(world, rgn, x, y, z);
	if(!vox2)return(false);
	if(BTGE_Voxel_VoxelOpaqueP(vox2))
	{
		if(BTGE_Voxel_VoxelFlexformP(vox2))
		{
			if(BTGE_Voxel_VoxelFluidP(vox))
				return(true);

			//if(!BTGE_Voxel_VoxelFlexformP(vox))
				//return(false);

			if(!BTGE_Voxel_VoxelFlexformP(vox))
			{
				vox3=BTGE_Voxel_GetWorldRegionVoxel(world, rgn, x, y, z+1);
				if(!BTGE_Voxel_VoxelOpaqueP(vox3))
					return(false);
				vox3=BTGE_Voxel_GetWorldRegionVoxel(world, rgn, x, y, z-1);
				if(!BTGE_Voxel_VoxelOpaqueP(vox3))
					return(false);
			}
		}
		return(true);
	}
	if(BTGE_Voxel_VoxelOpaqueP(vox))
		return(false);

	if(BTGE_Voxel_VoxelFluidP(vox))
	{
		if(BTGE_Voxel_VoxelFluidP(vox2))
			return(true);
	}

	if(BTGE_Voxel_VoxelAlphaP(vox))
	{
//		if(BTGE_Voxel_VoxelAlphaP(vox2))
//			return(true);

		if(vox->type==vox2->type)
			return(true);
	}
	
//	if(vox->type==vox2->type)
//		return(true);

	return(false);
}


BTGE_API bool BTGE_Voxel_CheckPointRegion(BTGE_VoxelRegion *rgn,
	int x, int y, int z)
{
	int rx, ry, rz;
	int tx, ty, tz;
	int i;

	rx=x-rgn->ox;
	ry=y-rgn->oy;
	rz=z-rgn->oz;

	if(rx<0)return(false);
	if(ry<0)return(false);
	if(rz<0)return(false);

	tx=rx/rgn->xst;
	ty=ry/rgn->yst;
	tz=rz/rgn->zst;

	if(tx>=rgn->xs)return(false);
	if(ty>=rgn->ys)return(false);
	if(tz>=rgn->zs)return(false);
	
	return(true);
}

BTGE_API BTGE_VoxelData BTGE_Voxel_GetRegionVoxelDataFlags(
	BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl)
{
	BTGE_VoxelData tv;
	BTGE_VoxelData *tmp;
	tmp=BTGE_Voxel_GetRegionVoxelFlags2(NULL, rgn, x, y, z, fl);
	if(!tmp)
	{
		memset(&tv, 0, sizeof(BTGE_VoxelData));
		return(tv);
	}
	return(*tmp);
}

BTGE_API BTGE_VoxelData BTGE_Voxel_GetWorldRegionVoxelDataFlags(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl)
{
	BTGE_VoxelData tv;
	BTGE_VoxelData *tmp;
	tmp=BTGE_Voxel_GetRegionVoxelFlags2(world, rgn, x, y, z, fl);
	if(!tmp)
	{
		memset(&tv, 0, sizeof(BTGE_VoxelData));
		return(tv);
	}
	return(*tmp);
}

BTGE_API bool BTGE_Voxel_SetRegionVoxelDataFlags(
	BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl, BTGE_VoxelData val)
{
	return(BTGE_Voxel_SetRegionVoxelDataFlags2(
		NULL, rgn, x, y, z, fl, val));

//	BTGE_VoxelData *tmp;
//	tmp=BTGE_Voxel_GetRegionVoxelFlags2(NULL, rgn, x, y, z, fl);
//	if(tmp)
//	{
//		*tmp=val;
//		return(true);
//	}
//	return(false);
}

BTGE_API bool BTGE_Voxel_SetWorldRegionVoxelDataFlags(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl, BTGE_VoxelData val)
{
	return(BTGE_Voxel_SetRegionVoxelDataFlags2(
		world, rgn, x, y, z, fl, val));

//	BTGE_VoxelData *tmp;
//	tmp=BTGE_Voxel_GetRegionVoxelFlags2(world, rgn, x, y, z, fl);
//	if(tmp)
//	{
//		*tmp=val;
//		return(true);
//	}
//	return(false);
}

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetRegionVoxelFlags(
	BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl)
{
	return(BTGE_Voxel_GetRegionVoxelFlags2(NULL, rgn, x, y, z, fl));
}

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetRegionVoxelFlags2(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl)
{
	BTGE_VoxelChunk *chk;
	BTGE_VoxelData *vox;
	int rx, ry, rz;
	int tx, ty, tz;
	int cn, vn;
	int i, j;

	rx=x-rgn->ox;
	ry=y-rgn->oy;
	rz=z-rgn->oz;

	if(rx<0)return(NULL);
	if(ry<0)return(NULL);
	if(rz<0)return(NULL);

	if(rx>=rgn->xsv)return(NULL);
	if(ry>=rgn->ysv)return(NULL);
	if(rz>=rgn->zsv)return(NULL);

//	tx=rx/rgn->xst;
//	ty=ry/rgn->yst;
//	tz=rz/rgn->zst;

	tx=rx>>rgn->xss;
	ty=ry>>rgn->yss;
	tz=rz>>rgn->zss;

//	if(tx>=rgn->xs)return(NULL);
//	if(ty>=rgn->ys)return(NULL);
//	if(tz>=rgn->zs)return(NULL);
	
	cn=(tx*rgn->ys+ty)*rgn->zs+tz;
	chk=rgn->chk[cn];

	//no chunk... try to load chunk if not already loading chunk...
	if(!chk && !(fl&(VOX_GFL_DELTA_LOAD|VOX_GFL_SKIP_LOAD)))
	{
		BTGE_VoxelSave_LoadRegionChunk(world, rgn, cn);
		chk=rgn->chk[cn];
	}

	//no chunk...
	if(!chk)
	{
		//if not modifying chunks, return NULL
		if(!(fl&VOX_GFL_DIRTY) && !(fl&VOX_GFL_REBUILD))
			return(NULL);
		
		if(!(fl&VOX_GFL_DELTA_LOAD))
		{
			printf("BTGE_Voxel_GetRegionVoxelFlags2: "
				"New Empty Chunk %d %d\n", cn, fl);
		}
		
		//ok, create a new chunk
//		chk=gcalloc(sizeof(BTGE_VoxelChunk));
		chk=gctalloc("btge_voxelchunk_t",
			sizeof(BTGE_VoxelChunk));
//		rgn->chk[(tx*rgn->ys+ty)*rgn->zs+tz]=chk;
		rgn->chk[cn]=chk;
		chk->rgn=rgn;
		
		chk->ox=rgn->ox+tx*(rgn->xst);
		chk->oy=rgn->oy+ty*(rgn->yst);
		chk->oz=rgn->oz+tz*(rgn->zst);
		chk->xs=rgn->xst;
		chk->ys=rgn->yst;
		chk->zs=rgn->zst;
		
		i=chk->xs*chk->ys*chk->zs;
//		chk->vox=gcatomic(i*sizeof(BTGE_VoxelData));
		chk->vox=gctatomic("btge_voxeldata_t",
			i*sizeof(BTGE_VoxelData));

		rgn->flags&=~VOX_CHFL_SAVE_OK;
	}

//	tx=rx%rgn->xst;
//	ty=ry%rgn->yst;
//	tz=rz%rgn->zst;

	tx=rx&(rgn->xst-1);
	ty=ry&(rgn->yst-1);
	tz=rz&(rgn->zst-1);
	vn=(tz*chk->ys+ty)*chk->xs+tx;

	//mark chunk as dirty (tick and rebuild)
	if(fl&VOX_GFL_DIRTY)
	{
		chk->clean=0;
		chk->tickdirty=1;
		chk->flags|=VOX_CHFL_DELTADIRTY;
		if(!(fl&VOX_GFL_DELTA_LOAD))
		{
			chk->flags&=~VOX_CHFL_SAVE_OK;
			chk->flags|=VOX_CHFL_LIGHT_DIRTY;
			chk->flags&=~VOX_CHFL_NOVISBLOCKS;
			rgn->flags&=~VOX_CHFL_SAVE_OK;
		}
	}

	//mark chunk as dirty (rebuild only)
	if(fl&VOX_GFL_REBUILD)
	{
		chk->clean=0;
//		chk->tickdirty=1;
		chk->flags|=VOX_CHFL_DELTADIRTY;
		if(!(fl&VOX_GFL_DELTA_LOAD))
		{
			chk->flags&=~VOX_CHFL_SAVE_OK;
			chk->flags|=VOX_CHFL_LIGHT_DIRTY;
			chk->flags&=~VOX_CHFL_NOVISBLOCKS;
			rgn->flags&=~VOX_CHFL_SAVE_OK;
		}
	}

	if(fl&VOX_GFL_ENTITY_MOVE)
	{
//		chk->flags&=~VOX_CHFL_SAVE_OK;
	}

	BTGE_Voxel_CheckDecodeChunkVoxelRLE(world, chk);
	
	if(chk->flags&VOX_CHFL_PACKINDEX)
	{
		if((fl&VOX_GFL_DIRTY) || (fl&VOX_GFL_REBUILD))
		{
			if(BTGE_Voxel_CheckRegionChunkExpandVoxelIndex(
				world, rgn, chk))
			{
				if(!chk->pidx_vox)
//				if(1)
				{
					*(int *)-1=-1;
				}
			
				i=chk->pidx_n_vox++;
				j=chk->pidx_idx[vn];
				chk->pidx_vox[i]=chk->pidx_vox[j];

				chk->pidx_idx[vn]=i;
				vox=&(chk->pidx_vox[i]);
//				*vox=chk->pidx_vox[j];
				return(vox);
			}else
			{
				BTGE_Voxel_UnpackVoxelIndexChunk(chk);
				chk->flags|=VOX_CHFL_NOPACKINDEX;
			}
//			return(NULL);
		}else
		{
			i=chk->pidx_idx[vn];
			vox=&(chk->pidx_vox[i]);
			return(vox);
		}
	}
	
	if(!chk->vox)
		return(NULL);

//	vox=&(chk->vox[(tx*chk->ys+ty)*chk->zs+tz]);
//	vox=&(chk->vox[(tz*chk->ys+ty)*chk->xs+tx]);
	vox=&(chk->vox[vn]);

	return(vox);
}

BTGE_API bool BTGE_Voxel_SetRegionVoxelDataFlags2(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl, BTGE_VoxelData vox)
{
	BTGE_VoxelChunk *chk;
	int rx, ry, rz;
	int tx, ty, tz;
	int cn, vn;
	int i;

	if(vox.type==0xFFFF)
	{
		*(int *)-1=-1;
	}

	rx=x-rgn->ox;
	ry=y-rgn->oy;
	rz=z-rgn->oz;

	if(rx<0)return(false);
	if(ry<0)return(false);
	if(rz<0)return(false);

	if(rx>=rgn->xsv)return(false);
	if(ry>=rgn->ysv)return(false);
	if(rz>=rgn->zsv)return(false);

	tx=rx>>rgn->xss;
	ty=ry>>rgn->yss;
	tz=rz>>rgn->zss;

	chk=BTGE_Voxel_GetRegionChunkFlags2(world, rgn, x, y, z, fl);
	if(!chk)return(false);
	
	tx=rx&(rgn->xst-1);
	ty=ry&(rgn->yst-1);
	tz=rz&(rgn->zst-1);
	vn=(tz*chk->ys+ty)*chk->xs+tx;

	BTGE_Voxel_CheckDecodeChunkVoxelRLE(world, chk);
	
	if(chk->flags&VOX_CHFL_PACKINDEX)
	{
		i=BTGE_Voxel_CheckChunkIndexVoxel(chk, vox);
		if(i>=0)
		{
			chk->pidx_idx[vn]=i;
			return(true);
		}
		BTGE_Voxel_UnpackVoxelIndexChunk(chk);
		chk->flags|=VOX_CHFL_NOPACKINDEX;
	}
	
	if(!chk->vox)
		return(false);
	chk->vox[vn]=vox;
	return(true);
}

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetRegionVoxel(
	BTGE_VoxelRegion *rgn,
	int x, int y, int z)
	{ return(BTGE_Voxel_GetRegionVoxelFlags(rgn, x, y, z, 0)); }

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetRegionVoxelDirty(
	BTGE_VoxelRegion *rgn,
	int x, int y, int z)
	{ return(BTGE_Voxel_GetRegionVoxelFlags(rgn, x, y, z, 1)); }

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetRegionVoxelDirty2(
	BTGE_VoxelRegion *rgn,
	int x, int y, int z)
	{ return(BTGE_Voxel_GetRegionVoxelFlags(rgn, x, y, z, 2)); }


BTGE_API BTGE_VoxelChunk *BTGE_Voxel_GetRegionChunkFlags2(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl)
{
	BTGE_VoxelChunk *chk;
	int rx, ry, rz;
	int tx, ty, tz;
	int cn;
	int i;

	rx=x-rgn->ox;
	ry=y-rgn->oy;
	rz=z-rgn->oz;

	if(rx<0)return(NULL);
	if(ry<0)return(NULL);
	if(rz<0)return(NULL);

	if(rx>=rgn->xsv)return(NULL);
	if(ry>=rgn->ysv)return(NULL);
	if(rz>=rgn->zsv)return(NULL);

	tx=rx>>rgn->xss;
	ty=ry>>rgn->yss;
	tz=rz>>rgn->zss;

	cn=(tx*rgn->ys+ty)*rgn->zs+tz;
	chk=rgn->chk[cn];

	//no chunk... try to load chunk if not already loading chunk...
//	if(!chk && !(fl&VOX_GFL_DELTA_LOAD))
	if(!chk && !(fl&(VOX_GFL_DELTA_LOAD|VOX_GFL_SKIP_LOAD)))
	{
		BTGE_VoxelSave_LoadRegionChunk(world, rgn, cn);
		chk=rgn->chk[cn];
	}

	//no chunk...
	if(!chk)
	{
		//if not modifying chunks, return NULL
		if(!(fl&VOX_GFL_DIRTY) && !(fl&VOX_GFL_REBUILD))
			return(NULL);
		
		if(!(fl&VOX_GFL_DELTA_LOAD))
		{
			printf("BTGE_Voxel_GetRegionVoxelFlags2: "
				"New Empty Chunk %d %d\n", cn, fl);
		}
		
		//ok, create a new chunk
//		chk=gcalloc(sizeof(BTGE_VoxelChunk));
		chk=gctalloc("btge_voxelchunk_t",
			sizeof(BTGE_VoxelChunk));
		rgn->chk[(tx*rgn->ys+ty)*rgn->zs+tz]=chk;
		chk->rgn=rgn;
		
		chk->ox=rgn->ox+tx*(rgn->xst);
		chk->oy=rgn->oy+ty*(rgn->yst);
		chk->oz=rgn->oz+tz*(rgn->zst);
		chk->xs=rgn->xst;
		chk->ys=rgn->yst;
		chk->zs=rgn->zst;
		
		i=chk->xs*chk->ys*chk->zs;
//		chk->vox=gcatomic(i*sizeof(BTGE_VoxelData));
//		chk->vox=gctatomic("btge_voxeldata_t", i*sizeof(BTGE_VoxelData));
		rgn->flags&=~VOX_CHFL_SAVE_OK;
	}

	//mark chunk as dirty (tick and rebuild)
	if(fl&VOX_GFL_DIRTY)
	{
		chk->clean=0;
		chk->tickdirty=1;
		chk->flags|=VOX_CHFL_DELTADIRTY;
		if(!(fl&VOX_GFL_DELTA_LOAD))
		{
			chk->flags&=~VOX_CHFL_SAVE_OK;
			chk->flags|=VOX_CHFL_LIGHT_DIRTY;
			rgn->flags&=~VOX_CHFL_SAVE_OK;
		}
	}

	//mark chunk as dirty (rebuild only)
	if(fl&VOX_GFL_REBUILD)
	{
		chk->clean=0;
//		chk->tickdirty=1;
		chk->flags|=VOX_CHFL_DELTADIRTY;
		if(!(fl&VOX_GFL_DELTA_LOAD))
		{
			chk->flags&=~VOX_CHFL_SAVE_OK;
			chk->flags|=VOX_CHFL_LIGHT_DIRTY;
			rgn->flags&=~VOX_CHFL_SAVE_OK;
		}
	}

	if(fl&VOX_GFL_ENTITY_MOVE)
	{
//		chk->flags&=~VOX_CHFL_SAVE_OK;
	}

	if(fl&VOX_GFL_DELTA_LOAD)
	{
		chk->clean=0;
	}

	return(chk);
}

BTGE_API BTGE_VoxelChunk *BTGE_Voxel_GetWorldChunkFlags(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl)
{
	BTGE_VoxelRegion *cur;
	BTGE_VoxelChunk *chk;

	cur=world->vox_region;
	while(cur)
	{
		chk=BTGE_Voxel_GetRegionChunkFlags2(world, cur, x, y, z, fl);
		if(chk)return(chk);
		cur=cur->next;
	}

	if(fl&VOX_GFL_DIRTY)
	{
		//if marking dirty, create a new region if-needed
		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl|2);
		if(!cur)return(NULL);
		chk=BTGE_Voxel_GetRegionChunkFlags2(world, cur, x, y, z, fl);
		if(chk)return(chk);
		return(NULL);
	}

	if(fl&VOX_GFL_DELTA_LOAD)
	{
		if(!(fl&(VOX_GFL_REBUILD|VOX_GFL_DIRTY)))
			return(NULL);
	
		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
		if(!cur)return(NULL);
		chk=BTGE_Voxel_GetRegionChunkFlags2(world, cur, x, y, z, fl);
		if(chk)return(chk);
		return(NULL);
	}

	return(NULL);
}

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldVoxelFlags(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl)
{
	BTGE_VoxelRegion *cur;
	BTGE_VoxelData *vox;

	cur=world->vox_region;
	while(cur)
	{
		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
		if(vox)return(vox);
		cur=cur->next;
	}

	if(fl&VOX_GFL_DIRTY)
	{
		//if marking dirty, create a new region if-needed
//		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl|2);
		if(!cur)return(NULL);
		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
		if(vox)return(vox);
		return(NULL);
	}

	if(fl&VOX_GFL_DELTA_LOAD)
	{
		if(!(fl&(VOX_GFL_REBUILD|VOX_GFL_DIRTY)))
			return(NULL);

		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
//		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl|2);
		if(!cur)return(NULL);
		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
		if(vox)return(vox);
		return(NULL);
	}

	return(NULL);
}

BTGE_API bool BTGE_Voxel_SetWorldVoxelDataFlags(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl, BTGE_VoxelData val)
{
	BTGE_VoxelRegion *cur;
	BTGE_VoxelData *vox;
	bool b;

	cur=world->vox_region;
	while(cur)
	{
		b=BTGE_Voxel_SetRegionVoxelDataFlags2(world, cur, x, y, z, fl, val);
		if(b)return(b);
//		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
//		if(vox)return(vox);
		cur=cur->next;
	}

	if(fl&VOX_GFL_DIRTY)
	{
		//if marking dirty, create a new region if-needed
//		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl|2);
		if(!cur)return(false);
//		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
//		if(vox)return(vox);
//		return(NULL);
		b=BTGE_Voxel_SetRegionVoxelDataFlags2(world, cur, x, y, z, fl, val);
		return(b);
	}

	if(fl&VOX_GFL_DELTA_LOAD)
	{
		if(!(fl&(VOX_GFL_REBUILD|VOX_GFL_DIRTY)))
			return(false);

		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
//		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl|2);
		if(!cur)return(false);
//		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
//		if(vox)return(vox);
//		return(NULL);
		b=BTGE_Voxel_SetRegionVoxelDataFlags2(world, cur, x, y, z, fl, val);
		return(b);
	}

	return(false);
}

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldRegionVoxelFlags(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, int z, int fl)
{
	BTGE_VoxelData *vox;

	vox=BTGE_Voxel_GetRegionVoxelFlags2(world, rgn, x, y, z, fl);
	if(vox)return(vox);
	
	vox=BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, fl);
	return(vox);
}

BTGE_API BTGE_VoxelRegion *BTGE_Voxel_GetWorldRegionFlags(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl)
{
	BTGE_VoxelRegion *cur;
	bool b;

	cur=world->vox_region;
	while(cur)
	{
		b=BTGE_Voxel_CheckPointRegion(cur, x, y, z);
		if(b)return(cur);
		cur=cur->next;
	}

	if(fl&VOX_GFL_DIRTY)
	{
		//if marking dirty, create a new region if-needed
//		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl);
		return(cur);
	}

	return(NULL);
}

#if 0
// BTGE_API int BTGE_Voxel_GetWorldRegionXSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(32*16); }
// BTGE_API int BTGE_Voxel_GetWorldRegionYSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(32*16); }
// BTGE_API int BTGE_Voxel_GetWorldRegionZSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(8*16); }
#endif

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldVoxel(
		BTGE_BrushWorld *world, int x, int y, int z)
	{ return(BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, 0)); }

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldVoxelDirty(
		BTGE_BrushWorld *world, int x, int y, int z)
	{ return(BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, 1)); }

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldVoxelDirty2(
		BTGE_BrushWorld *world, int x, int y, int z)
	{ return(BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, 2)); }

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldVoxelDirty3(
		BTGE_BrushWorld *world, int x, int y, int z)
	{ return(BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, 2|4)); }

BTGE_API BTGE_VoxelData BTGE_Voxel_GetWorldVoxelData(
		BTGE_BrushWorld *world, int x, int y, int z)
{
	BTGE_VoxelData tmp;
	BTGE_VoxelData *pvox;
	pvox=BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, 0);
	if(!pvox)
	{
		memset(&tmp, 0, sizeof(BTGE_VoxelData));
		return(tmp);
	}
	return(*pvox);
}

BTGE_API bool BTGE_Voxel_SetWorldVoxelData(
		BTGE_BrushWorld *world, int x, int y, int z, BTGE_VoxelData vox)
{
	return(BTGE_Voxel_SetWorldVoxelDataFlags(world, x, y, z, 1, vox));

//	BTGE_VoxelData *pvox;
//	pvox=BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, 1);
//	if(!pvox)return(false);
//	*pvox=vox;
//	return(true);
}

BTGE_API BTGE_VoxelData BTGE_Voxel_GetWorldVoxelDataFlags(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl)
{
	BTGE_VoxelData tv;
	BTGE_VoxelData *tmp;
	tmp=BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, fl);
	if(!tmp)
	{
		memset(&tv, 0, sizeof(BTGE_VoxelData));
		return(tv);
	}
	return(*tmp);
}

#if 0
// BTGE_API bool BTGE_Voxel_SetWorldVoxelDataFlags(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl, BTGE_VoxelData val)
{
	BTGE_VoxelData *tmp;
	tmp=BTGE_Voxel_GetWorldVoxelFlags(world, x, y, z, fl);
	if(tmp)
	{
		*tmp=val;
		return(true);
	}
	return(false);
}
#endif

BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldRegionVoxel(
		BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
		int x, int y, int z)
	{ return(BTGE_Voxel_GetWorldRegionVoxelFlags(world, rgn, x, y, z, 0)); }
BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldRegionVoxelDirty(
		BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
		int x, int y, int z)
	{ return(BTGE_Voxel_GetWorldRegionVoxelFlags(world, rgn, x, y, z, 1)); }
BTGE_API BTGE_VoxelData *BTGE_Voxel_GetWorldRegionVoxelDirty2(
		BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
		int x, int y, int z)
	{ return(BTGE_Voxel_GetWorldRegionVoxelFlags(world, rgn, x, y, z, 2)); }

BTGE_API void BTGE_Voxel_RebuildRegionVisible(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *chk;
	int i, j, k, l, n, m;

	if(!BTGE_Voxel_CheckRegionFrustum(rgn))
		return;

	rgn->vischk=NULL; n=0; m=0;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
			for(k=0; k<rgn->zs; k++)
	{
		chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+k];
		if(!chk)continue;
		if(!BTGE_Voxel_CheckChunkFrustum(chk))
			continue;
		BTGE_Voxel_CheckRebuildChunk(world, chk);
		
		m++;
		if(!chk->mesh || !chk->mesh->n_face)
			continue;
		
		chk->mesh->vnext=rgn->vischk;
		rgn->vischk=chk;
		n++;
	}
	
//	printf("Visible Chunks: %d/%d\n", n, m);
}

BTGE_API void BTGE_Voxel_RebuildRegionListVisible(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *lst)
{
	BTGE_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		BTGE_Voxel_RebuildRegionVisible(world, cur);
		cur=cur->next;
	}
}

// void BTGE_Voxel_ClearWorldPointLight(BTGE_BrushWorld *world)
// {
// }

// void BTGE_Voxel_UpdateWorldPointLight(BTGE_BrushWorld *world,
//	BTGE_Light *light)
// {
//	BTGE_Voxel_DrawRegionListFlat(world->vox_region);
// }

BTGE_API bool BTGE_Voxel_GetWorldVoxelBBox(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl,
	float *mins, float *maxs)
{
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	int i, j;

	mins[0]=x*32;
	mins[1]=y*32;
	mins[2]=z*32;

	maxs[0]=x*32+32;
	maxs[1]=y*32+32;
	maxs[2]=z*32+32;

	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
	if(!vox || !vox->type)
		return(false);
	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
	if(!inf)return(false);
	
	mins[0]=x*32+inf->mins[0];
	mins[1]=y*32+inf->mins[1];
	mins[2]=z*32+inf->mins[2];

	maxs[0]=x*32+inf->maxs[0];
	maxs[1]=y*32+inf->maxs[1];
	maxs[2]=z*32+inf->maxs[2];
	return(true);
}

BTGE_API int BTGE_Voxel_GetWorldVoxelBBoxList(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl,
	float *mins, float *maxs, int max)
{
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	int i;

	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
	if(!vox || !vox->type)
		return(0);
	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
	if(!inf)return(0);

	if(inf->GetWorldVoxelBBoxList)
	{
		i=inf->GetWorldVoxelBBoxList(world, x, y, z, fl,
			mins, maxs, max);
		return(i);
	}

	mins[0]=x*32+inf->mins[0];
	mins[1]=y*32+inf->mins[1];
	mins[2]=z*32+inf->mins[2];
	maxs[0]=x*32+inf->maxs[0];
	maxs[1]=y*32+inf->maxs[1];
	maxs[2]=z*32+inf->maxs[2];
	return(1);

//	if(BTGE_Voxel_GetWorldVoxelBBox(world, x, y, z, fl, mins, maxs))
//		return(1);
//	return(0);
}

BTGE_API void BTGE_Voxel_UpdateBlastDamage(
	BTGE_BrushWorld *world, vec3d origin, float radius, float damage)
{
	BTGE_Voxel_UpdateBlastDamage2(world, origin, radius, damage, 0);
}

BTGE_API void BTGE_Voxel_UpdateBlastDamage2(
	BTGE_BrushWorld *world, vec3d origin,
	float radius, float damage, int flags)
{
	double org[3], org1[3];
	double am[4], an[4];
	int ami[4], ani[4];
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	float f, g, br;
	int i, j, k, l0, l1, ty, ved;
	
	vdvec3d(org, origin);

	ved=btCvarGeti("vox_explosivedamage");

	if(ved==2)
	{
		ved=0;
		if(flags&1)ved=1;
	}

	if(world->flags&BTGE_WFL_NOEXPLOSIVEDAMAGE)
		ved=0;

#if 1
	org[0]=floor(org[0]/32)*32+16;
	org[1]=floor(org[1]/32)*32+16;
	org[2]=floor(org[2]/32)*32+16;
#endif

	am[0]=org[0]-radius;
	am[1]=org[1]-radius;
	am[2]=org[2]-radius;

	an[0]=org[0]+radius;
	an[1]=org[1]+radius;
	an[2]=org[2]+radius;

//	ami[0]=am[0]/32-1; ami[1]=am[1]/32-1; ami[2]=am[2]/32-1;
//	ani[0]=an[0]/32+1; ani[1]=an[1]/32+1; ani[2]=an[2]/32+1;

	ami[0]=floor(am[0]/32)-1;
	ami[1]=floor(am[1]/32)-1;
	ami[2]=floor(am[2]/32)-1;

	ani[0]=ceil(an[0]/32)+1;
	ani[1]=ceil(an[1]/32)+1;
	ani[2]=ceil(an[2]/32)+1;

	for(i=ami[0]; i<=ani[0]; i++)
		for(j=ami[1]; j<=ani[1]; j++)
			for(k=ami[2]; k<=ani[2]; k++)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world, i, j, k);
		if(vox && vox->type)
		{
			ty=vox->type&VOX_TY_MASK;
			if(ty==VOX_TY_BEDROCK)
				continue;
			if((vox->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE)
				continue;

#if 0
			//unbreakable varieties
			if((ty==VOX_TY_GLASS) ||
				(ty==VOX_TY_ICE) ||
				(ty==VOX_TY_STONE) ||
				(ty==VOX_TY_COBBLESTONE) ||
				(ty==VOX_TY_DIRT) ||
				(ty==VOX_TY_GRASS) ||
				(ty==VOX_TY_SAND) ||
				(ty==VOX_TY_OBSIDIAN))
			{
				if((vox->aux&0x0F)==15)
					continue;
			}
#endif

			org1[0]=i*32+16;
			org1[1]=j*32+16;
			org1[2]=k*32+16;
			f=V3F_DIST(org, org1);
			
			//f=f-16;
			if(f<0)f=0;

			if(vox->type==VOX_TY_WOOD_DOOR)
			{
				f=f-16;
				if(f<0)f=0;
			}
			
			//out of range
			if(f>=radius)continue;
//			if(f>radius)continue;
			
			//no damage
			g=damage-f;
			if(g<=0)continue;

#if 0
			if(g<=0)
			{
				//vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j, k);
				BTGE_Voxel_SmallDamageEvent(world,
					vox, i, j, k, org, radius, damage, g);
				continue;
			}
#endif

//			if(f>=radius)continue;

			inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, i, j, k);
			if(inf)
			{
				br=inf->blast_resist;
			}else
			{
				br=50;
			}

			//below threshold
			if((g<=br) || !ved)
			{
				//vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j, k);
				BTGE_Voxel_SmallDamageEvent(world,
					vox, i, j, k, org, radius, damage, g);
				continue;
			}

			//trigger any actions before blowing up.
			BTGE_Voxel_LargeDamageEvent(world,
				vox, i, j, k, org, radius, damage, g);

			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j, k);
			if(!vox || !(vox->type&VOX_TY_MASK))
			{
				//there was a block here, but it's gone now...
				continue;
			}

			l0=BTGE_Voxel_GetVoxelFlags(vox);
			l1=BTGE_Voxel_GetVoxelFlags2(vox);

			//can't blow up fluids
			if(l0&BTGE_TXFL_FLUID)
				continue;
			if(l1&BTGE_TXFL2_UNBREAKABLE)
				continue;

			vox->type=0;	//destroy voxel

			//touch nearby voxels as well
			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i-1, j, k);
			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i+1, j, k);
			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j-1, k);
			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j+1, k);
			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j, k-1);
			vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j, k+1);
		}
	}
}

/** Walk from start to end position, finding matching voxels */
BTGE_API BTGE_VoxelData *BTGE_Voxel_TraceLineVoxel(
	BTGE_BrushWorld *world, vec3 spos, vec3 epos, int flags,
	float *rpt)
{
	float am[4], an[4];
	int ami[3], ani[3];
	int adx[3], asx[3];
	int apx[3], err[3];
	int idx[3], jdx[3];
	BTGE_VoxelData *vox;
	int i, j, k, n;

	if(!world->vox_region)
		return(NULL);

	vfvec3(am, spos);
	vfvec3(an, epos);

	ami[0]=floor(am[0]/32);
	ami[1]=floor(am[1]/32);
	ami[2]=floor(am[2]/32);

	ani[0]=floor(an[0]/32);
	ani[1]=floor(an[1]/32);
	ani[2]=floor(an[2]/32);

	//see if starting position is solid
	vox=BTGE_Voxel_GetWorldVoxel(world, ami[0], ami[1], ami[2]);
	if(BTGE_Voxel_VoxelSolidP(vox))
	{
		rpt[0]=ami[0]*32;
		rpt[1]=ami[1]*32;
		rpt[2]=ami[2]*32;
		return(vox);
	}

	adx[0]=ani[0]-ami[0];
	adx[1]=ani[1]-ami[1];
	adx[2]=ani[2]-ami[2];

	if(adx[0]<0) { adx[0]=-adx[0]; asx[0]=-1; } else {asx[0]=1; }
	if(adx[1]<0) { adx[1]=-adx[1]; asx[1]=-1; } else {asx[1]=1; }
	if(adx[2]<0) { adx[2]=-adx[2]; asx[2]=-1; } else {asx[2]=1; }
	
	idx[0]=0; idx[1]=1; idx[2]=2;
	for(i=0; i<3; i++)
	{
		for(j=i+1; j<3; j++)
		{
			if(adx[j]>adx[i])
			{
				k=adx[i]; adx[i]=adx[j]; adx[j]=k;
				k=asx[i]; asx[i]=asx[j]; asx[j]=k;
				k=idx[i]; idx[i]=idx[j]; idx[j]=k;
			}
		}
	}
	
	jdx[idx[0]]=0;
	jdx[idx[1]]=1;
	jdx[idx[2]]=2;
	
	apx[0]=ami[idx[0]]; apx[1]=ami[idx[1]]; apx[2]=ami[idx[2]];
	err[0]=0; err[1]=0; err[2]=0;

	n=512;
	while((n--)>0)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world,
			apx[jdx[0]], apx[jdx[1]], apx[jdx[2]]);
		if(vox && BTGE_Voxel_VoxelSolidP(vox))
		{
			rpt[0]=apx[jdx[0]]*32+16;
			rpt[1]=apx[jdx[1]]*32+16;
			rpt[2]=apx[jdx[2]]*32+16;
			return(vox);
		}

		if(	(apx[0]==ani[idx[0]]) &&
			(apx[1]==ani[idx[1]]) &&
			(apx[2]==ani[idx[2]]))
				break;

		err[1]+=adx[2];
		if(err[1]>adx[1])
		{
			apx[2]+=asx[2];
			err[1]-=adx[1];
		}

		err[0]+=adx[1];
		if(err[0]>adx[0])
		{
			apx[1]+=asx[1];
			err[0]-=adx[0];
		}
		apx[0]+=asx[0];
	}

	if(n<=0)
	{
		printf("BTGE_Voxel_TraceLineVoxel: Traceline Fail Terminate\n");
		return(NULL);
	}

	return(NULL);
}

BTGE_API BTGE_SEntity *BTGE_VoxelRegion_LookupSEntityId(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int id)
{
	BTGE_SEntity *scur;

	scur=rgn->sentity;
	while(scur)
	{
		if(scur->idnum==id)
			return(scur);
		scur=scur->next;
	}

	return(NULL);
}

BTGE_API BTGE_SEntity *BTGE_VoxelRegion_GetSEntityId(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int id)
{
	BTGE_SEntity *scur;

	scur=BTGE_VoxelRegion_LookupSEntityId(world, rgn, id);
	if(scur)return(scur);

	scur=gctalloc("lbxgl_sentity_s", sizeof(BTGE_SEntity));
	BTGE_SEnt_AddEntity(&rgn->sentity, scur);
	scur->wrl=world;
	scur->idnum=id;

	return(scur);
}

BTGE_API BTGE_Brush *BTGE_VoxelRegion_LookupBrush(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int id)
{
	BTGE_Brush *bcur;

	bcur=rgn->brush;
	while(bcur)
	{
		if(bcur->idnum==id)
			return(bcur);
		bcur=bcur->next;
	}

	return(NULL);
}

BTGE_API BTGE_Brush *BTGE_VoxelRegion_CreateBrushId(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int sentid, int brushid)
{
	BTGE_SEntity *scur;
	BTGE_Brush *bcur;
	
//	BTGE_BrushWorld_DrawFlush(world);
	
//	scur=BTGE_BrushWorld_LookupSEntityId(world, sentid);
	scur=BTGE_VoxelRegion_GetSEntityId(world, rgn, sentid);
	
	if(!scur)
	{
		printf("BTGE_BrushWorld_CreateBrushId: No Entity %d\n", sentid);
		return(NULL);
	}
	
	bcur=BTGE_Brush_New(8);
	bcur->idnum=brushid;
	bcur->n_face=0;
	
//	world->entity_psel=scur;
//	BTGE_BrushWorld_AddBrush(world, bcur);
//	BTGE_BrushWorld_AddEntityBrush(world, scur, bcur);
	
	return(bcur);
}
