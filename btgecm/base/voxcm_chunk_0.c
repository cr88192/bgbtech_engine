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
"chest_locked",			//95
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

NULL
};

BTGE_VoxelInfo *btge_voxinfo[4096];
byte btge_voxel_opaque[4096];
int btge_voxel_use_flexform=1;

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
	
	tmp->blast_resist=30;
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
		}

		if(!strcmp(a[0], "$vox_maxs"))
		{
			tmp->maxs[0]=atof(a[1]);
			tmp->maxs[1]=atof(a[2]);
			tmp->maxs[2]=atof(a[3]);
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

		if(!strcmp(a[0], "$vox_img_top"))
			{ tmp->img_top=BTGE_Texture_LoadImage(a[1]); }
		if(!strcmp(a[0], "$vox_img_side"))
			{ tmp->img_side=BTGE_Texture_LoadImage(a[1]); }
		if(!strcmp(a[0], "$vox_img_bot"))
			{ tmp->img_bot=BTGE_Texture_LoadImage(a[1]); }

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
	}

	vfclose(fd);
	return(0);
}

BTGE_API int BTGE_Voxel_LoadVoxelType(int idx, char *name)
{
	char tb[256];
	BTGE_VoxelInfo *vinf;
	int i;

	if(!btge_voxinfo[idx])
	{
		vinf=gcalloc(sizeof(BTGE_VoxelInfo));
		BTGE_Voxel_SetInfoDefaults(vinf);
		btge_voxinfo[idx]=vinf;
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

		sprintf(tb, "textures/voxel/%s_side", name);
		vinf->img_side=BTGE_Texture_LoadImage(tb);
		sprintf(tb, "textures/voxel/%s_bot", name);
		vinf->img_bot=BTGE_Texture_LoadImage(tb);

		if(!vinf->img_bot)
			vinf->img_bot=vinf->img_top;
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

	for(i=0; i<4096; i++)
	{
		if(!btge_voxinfo[i])
			continue;
		if(btge_voxinfo[i]->flags&BTGE_TXFL_ALPHA)
			btge_voxel_opaque[i]=0;	//ok, not opaque
		if(btge_voxinfo[i]->flags2&BTGE_TXFL2_PARTIALBLOCK)
			btge_voxel_opaque[i]=0;	//not opaque
	}
}

BTGE_API int BTGE_Voxel_IndexVoxelName(char *name)
{
	int i;

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
	
	if(chk->face_xyz)
	{
		gcfree(chk->face_xyz);
		gcfree(chk->face_st);
		gcfree(chk->face_norm);
		gcfree(chk->face_rgba);
		gcfree(chk->face_slrgba);
		gcfree(chk->face_vlrgba);
		gcfree(chk->face_tex);
		gcfree(chk->face_vox);

		chk->face_xyz=NULL;
		chk->face_st=NULL;
		chk->face_norm=NULL;
		chk->face_rgba=NULL;
		chk->face_slrgba=NULL;
		chk->face_vlrgba=NULL;
		chk->face_tex=NULL;
		chk->face_vox=NULL;
	}

	if(chk->aface_xyz)
	{
		gcfree(chk->aface_xyz);
		gcfree(chk->aface_st);
		gcfree(chk->aface_norm);
		gcfree(chk->aface_rgba);
		gcfree(chk->aface_slrgba);
		gcfree(chk->aface_vlrgba);
		gcfree(chk->aface_tex);
		gcfree(chk->aface_vox);

		chk->aface_xyz=NULL;
		chk->aface_st=NULL;
		chk->aface_norm=NULL;
		chk->aface_rgba=NULL;
		chk->aface_slrgba=NULL;
		chk->aface_vlrgba=NULL;
		chk->aface_tex=NULL;
		chk->aface_vox=NULL;
	}

	if(chk->mesh_xyz)
	{
		if(chk->mesh_xyz)gcfree(chk->mesh_xyz);
		if(chk->mesh_norm)gcfree(chk->mesh_norm);
		if(chk->mesh_edge)gcfree(chk->mesh_edge);
		if(chk->mesh_pts)gcfree(chk->mesh_pts);
		if(chk->mesh_face)gcfree(chk->mesh_face);
		
		chk->mesh_xyz=NULL;
		chk->mesh_norm=NULL;
		chk->mesh_edge=NULL;
		chk->mesh_pts=NULL;
		chk->mesh_face=NULL;
	}
}

void BTGE_Voxel_CheckExpandFace(BTGE_VoxelChunk *chk)
{
	int i;

	if(!chk->face_xyz)
	{
//		i=64;
//		i=256;
		i=384;
		chk->face_xyz=gcatomic(i*4*3*sizeof(float));
		chk->face_st=gcatomic(i*4*2*sizeof(float));
		chk->face_norm=gcatomic(i*4*3*sizeof(float));
		chk->face_rgba=gcatomic(i*4*4*sizeof(float));
		chk->face_slrgba=gcatomic(i*4*4*sizeof(float));
		chk->face_vlrgba=gcatomic(i*4*4*sizeof(float));
		chk->face_tex=gcatomic(i*1*1*sizeof(int));
		chk->face_vox=gcatomic(i*1*1*sizeof(BTGE_VoxelData *));
		chk->n_face=0;
		chk->m_face=i;
	}
	
	if((chk->n_face+1)>=chk->m_face)
	{
		i=chk->m_face+(chk->m_face>>1);
		chk->face_xyz=gcrealloc(chk->face_xyz, i*4*3*sizeof(float));
		chk->face_st=gcrealloc(chk->face_st, i*4*2*sizeof(float));
		chk->face_norm=gcrealloc(chk->face_norm, i*4*3*sizeof(float));
		chk->face_rgba=gcrealloc(chk->face_rgba, i*4*4*sizeof(float));
		chk->face_slrgba=gcrealloc(chk->face_slrgba, i*4*4*sizeof(float));
		chk->face_vlrgba=gcrealloc(chk->face_vlrgba, i*4*4*sizeof(float));
		chk->face_tex=gcrealloc(chk->face_tex, i*1*1*sizeof(int));
		chk->face_vox=gcrealloc(chk->face_vox,
			i*1*1*sizeof(BTGE_VoxelData *));
		chk->m_face=i;
	}
}

void BTGE_Voxel_CheckExpandAlphaFace(BTGE_VoxelChunk *chk)
{
	int i;

	if(!chk->aface_xyz)
	{
		i=256;
		chk->aface_xyz=gcatomic(i*4*3*sizeof(float));
		chk->aface_st=gcatomic(i*4*2*sizeof(float));
		chk->aface_norm=gcatomic(i*4*3*sizeof(float));
		chk->aface_rgba=gcatomic(i*4*4*sizeof(float));
		chk->aface_slrgba=gcatomic(i*4*4*sizeof(float));
		chk->aface_vlrgba=gcatomic(i*4*4*sizeof(float));
		chk->aface_tex=gcatomic(i*1*1*sizeof(int));
		chk->aface_vox=gcatomic(i*1*1*sizeof(BTGE_VoxelData *));
		chk->n_aface=0;
		chk->m_aface=i;
	}
	
	if((chk->n_aface+1)>=chk->m_aface)
	{
		i=chk->m_aface+(chk->m_aface>>1);
		chk->aface_xyz=gcrealloc(chk->aface_xyz, i*4*3*sizeof(float));
		chk->aface_st=gcrealloc(chk->aface_st, i*4*2*sizeof(float));
		chk->aface_norm=gcrealloc(chk->aface_norm, i*4*3*sizeof(float));
		chk->aface_rgba=gcrealloc(chk->aface_rgba, i*4*4*sizeof(float));
		chk->aface_slrgba=gcrealloc(chk->aface_slrgba, i*4*4*sizeof(float));
		chk->aface_vlrgba=gcrealloc(chk->aface_vlrgba, i*4*4*sizeof(float));
		chk->aface_tex=gcrealloc(chk->aface_tex, i*1*1*sizeof(int));
		chk->aface_vox=gcrealloc(chk->aface_vox,
			i*1*1*sizeof(BTGE_VoxelData *));
		chk->m_aface=i;
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

//	f=(vox->aux&0xF0)/240.0;

	i=(vox->aux>>4)&15;
	f=pow(0.80, 15-i);
	return(f);
}

void BTGE_Voxel_EmitFace(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0,
	float *xyz1, float *st1,
	float *xyz2, float *st2,
	float *xyz3, float *st3,
	float *norm, float *rgba,
	int tex)
{
	int i;

	if(BTGE_Voxel_VoxelAlphaP(vox))
	{
		BTGE_Voxel_EmitAlphaFace(chk, vox,
			xyz0, st0,	xyz1, st1,
			xyz2, st2,	xyz3, st3,
			norm, rgba, tex);
		return;
	}

	BTGE_Voxel_EmitSolidFace(chk, vox,
		xyz0, st0,	xyz1, st1,
		xyz2, st2,	xyz3, st3,
		norm, rgba, tex);
}

void BTGE_Voxel_EmitSolidFace(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0,
	float *xyz1, float *st1,
	float *xyz2, float *st2,
	float *xyz3, float *st3,
	float *norm, float *rgba,
	int tex)
{
	float slrgba[4];
	float f;

//	f=(vox->aux&0xF0)/240.0;
	f=BTGE_Voxel_GetVoxelGamma(chk, vox);
	slrgba[0]=f*rgba[0];
	slrgba[1]=f*rgba[1];
	slrgba[2]=f*rgba[2];
	slrgba[3]=rgba[3];

	BTGE_Voxel_EmitSolidFace2(chk, vox,
		xyz0, st0, norm, rgba, slrgba,
		xyz1, st1, norm, rgba, slrgba,
		xyz2, st2, norm, rgba, slrgba,
		xyz3, st3, norm, rgba, slrgba,
		tex);
}

void BTGE_Voxel_EmitSolidFace2(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0, float *rgba0, float *slrgba0,
	float *xyz1, float *st1, float *norm1, float *rgba1, float *slrgba1,
	float *xyz2, float *st2, float *norm2, float *rgba2, float *slrgba2,
	float *xyz3, float *st3, float *norm3, float *rgba3, float *slrgba3,
	int tex)
{
	float tv[4];
	
	V3F_ZERO(tv);
	tv[3]=1;

	BTGE_Voxel_EmitSolidFace3(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0, tv,
		xyz1, st1, norm1, rgba1, slrgba1, tv,
		xyz2, st2, norm2, rgba2, slrgba2, tv,
		xyz3, st3, norm3, rgba3, slrgba3, tv,
		tex);
}

void BTGE_Voxel_EmitSolidFace3(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0,
		float *rgba0, float *slrgba0, float *vlrgba0,
	float *xyz1, float *st1, float *norm1,
		float *rgba1, float *slrgba1, float *vlrgba1,
	float *xyz2, float *st2, float *norm2,
		float *rgba2, float *slrgba2, float *vlrgba2,
	float *xyz3, float *st3, float *norm3,
		float *rgba3, float *slrgba3, float *vlrgba3,
	int tex)
{
	int i;

	BTGE_Voxel_CheckExpandFace(chk);
	
	i=chk->n_face++;
	
	chk->face_tex[i]=tex;
	chk->face_vox[i]=vox;
	
	V3F_COPY(xyz0, chk->face_xyz+i*4*3+0);
	V3F_COPY(xyz1, chk->face_xyz+i*4*3+3);
	V3F_COPY(xyz2, chk->face_xyz+i*4*3+6);
	V3F_COPY(xyz3, chk->face_xyz+i*4*3+9);

	V2F_COPY(st0, chk->face_st+i*4*2+0);
	V2F_COPY(st1, chk->face_st+i*4*2+2);
	V2F_COPY(st2, chk->face_st+i*4*2+4);
	V2F_COPY(st3, chk->face_st+i*4*2+6);

	V3F_COPY(norm0, chk->face_norm+i*4*3+0);
	V3F_COPY(norm1, chk->face_norm+i*4*3+3);
	V3F_COPY(norm2, chk->face_norm+i*4*3+6);
	V3F_COPY(norm3, chk->face_norm+i*4*3+9);

	V4F_COPY(rgba0, chk->face_rgba+i*4*4+ 0);
	V4F_COPY(rgba1, chk->face_rgba+i*4*4+ 4);
	V4F_COPY(rgba2, chk->face_rgba+i*4*4+ 8);
	V4F_COPY(rgba3, chk->face_rgba+i*4*4+12);

	V4F_COPY(slrgba0, chk->face_slrgba+i*4*4+ 0);
	V4F_COPY(slrgba1, chk->face_slrgba+i*4*4+ 4);
	V4F_COPY(slrgba2, chk->face_slrgba+i*4*4+ 8);
	V4F_COPY(slrgba3, chk->face_slrgba+i*4*4+12);

	V4F_COPY(vlrgba0, chk->face_vlrgba+i*4*4+ 0);
	V4F_COPY(vlrgba1, chk->face_vlrgba+i*4*4+ 4);
	V4F_COPY(vlrgba2, chk->face_vlrgba+i*4*4+ 8);
	V4F_COPY(vlrgba3, chk->face_vlrgba+i*4*4+12);
}

void BTGE_Voxel_EmitAlphaFace(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0,
	float *xyz1, float *st1,
	float *xyz2, float *st2,
	float *xyz3, float *st3,
	float *norm, float *rgba,
	int tex)
{
	float slrgba[4];
	float f;
	int i;

//	f=(vox->aux&0xF0)/240.0;

	i=(vox->aux>>4)&15;
	f=pow(0.80, 15-i);

	slrgba[0]=f*rgba[0];
	slrgba[1]=f*rgba[1];
	slrgba[2]=f*rgba[2];
	slrgba[3]=rgba[3];

	BTGE_Voxel_EmitAlphaFace2(chk, vox,
		xyz0, st0, norm, rgba, slrgba,
		xyz1, st1, norm, rgba, slrgba,
		xyz2, st2, norm, rgba, slrgba,
		xyz3, st3, norm, rgba, slrgba,
		tex);
}

void BTGE_Voxel_EmitAlphaFace2(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0, float *rgba0, float *slrgba0,
	float *xyz1, float *st1, float *norm1, float *rgba1, float *slrgba1,
	float *xyz2, float *st2, float *norm2, float *rgba2, float *slrgba2,
	float *xyz3, float *st3, float *norm3, float *rgba3, float *slrgba3,
	int tex)
{
	float tv[4];
	
	V3F_ZERO(tv);
	tv[3]=1;

	BTGE_Voxel_EmitAlphaFace3(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0, tv,
		xyz1, st1, norm1, rgba1, slrgba1, tv,
		xyz2, st2, norm2, rgba2, slrgba2, tv,
		xyz3, st3, norm3, rgba3, slrgba3, tv,
		tex);
}

void BTGE_Voxel_EmitAlphaFace3(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0,
		float *rgba0, float *slrgba0, float *vlrgba0,
	float *xyz1, float *st1, float *norm1,
		float *rgba1, float *slrgba1, float *vlrgba1,
	float *xyz2, float *st2, float *norm2,
		float *rgba2, float *slrgba2, float *vlrgba2,
	float *xyz3, float *st3, float *norm3,
		float *rgba3, float *slrgba3, float *vlrgba3,
	int tex)
{
	int i;

	BTGE_Voxel_CheckExpandAlphaFace(chk);
	
	i=chk->n_aface++;
	
	chk->aface_tex[i]=tex;
	chk->aface_vox[i]=vox;
	
	V3F_COPY(xyz0, chk->aface_xyz+i*4*3+0);
	V3F_COPY(xyz1, chk->aface_xyz+i*4*3+3);
	V3F_COPY(xyz2, chk->aface_xyz+i*4*3+6);
	V3F_COPY(xyz3, chk->aface_xyz+i*4*3+9);

	V2F_COPY(st0, chk->aface_st+i*4*2+0);
	V2F_COPY(st1, chk->aface_st+i*4*2+2);
	V2F_COPY(st2, chk->aface_st+i*4*2+4);
	V2F_COPY(st3, chk->aface_st+i*4*2+6);

	V3F_COPY(norm0, chk->aface_norm+i*4*3+0);
	V3F_COPY(norm1, chk->aface_norm+i*4*3+3);
	V3F_COPY(norm2, chk->aface_norm+i*4*3+6);
	V3F_COPY(norm3, chk->aface_norm+i*4*3+9);

	V4F_COPY(rgba0, chk->aface_rgba+i*4*4+ 0);
	V4F_COPY(rgba1, chk->aface_rgba+i*4*4+ 4);
	V4F_COPY(rgba2, chk->aface_rgba+i*4*4+ 8);
	V4F_COPY(rgba3, chk->aface_rgba+i*4*4+12);

	V4F_COPY(slrgba0, chk->aface_slrgba+i*4*4+ 0);
	V4F_COPY(slrgba1, chk->aface_slrgba+i*4*4+ 4);
	V4F_COPY(slrgba2, chk->aface_slrgba+i*4*4+ 8);
	V4F_COPY(slrgba3, chk->aface_slrgba+i*4*4+12);

	V4F_COPY(vlrgba0, chk->aface_vlrgba+i*4*4+ 0);
	V4F_COPY(vlrgba1, chk->aface_vlrgba+i*4*4+ 4);
	V4F_COPY(vlrgba2, chk->aface_vlrgba+i*4*4+ 8);
	V4F_COPY(vlrgba3, chk->aface_vlrgba+i*4*4+12);
}

void BTGE_Voxel_EmitFace2(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0, float *rgba0, float *slrgba0,
	float *xyz1, float *st1, float *norm1, float *rgba1, float *slrgba1,
	float *xyz2, float *st2, float *norm2, float *rgba2, float *slrgba2,
	float *xyz3, float *st3, float *norm3, float *rgba3, float *slrgba3,
	int tex)
{
	int i;

	if(BTGE_Voxel_VoxelAlphaP(vox))
	{
		BTGE_Voxel_EmitAlphaFace2(chk, vox,
			xyz0, st0, norm0, rgba0, slrgba0,
			xyz1, st1, norm1, rgba1, slrgba1,
			xyz2, st2, norm2, rgba2, slrgba2,
			xyz3, st3, norm3, rgba3, slrgba3,
			tex);
		return;
	}

	BTGE_Voxel_EmitSolidFace2(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0,
		xyz1, st1, norm1, rgba1, slrgba1,
		xyz2, st2, norm2, rgba2, slrgba2,
		xyz3, st3, norm3, rgba3, slrgba3,
		tex);
}

void BTGE_Voxel_EmitFace3(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0,
		float *rgba0, float *slrgba0, float *vlrgba0,
	float *xyz1, float *st1, float *norm1,
		float *rgba1, float *slrgba1, float *vlrgba1,
	float *xyz2, float *st2, float *norm2,
		float *rgba2, float *slrgba2, float *vlrgba2,
	float *xyz3, float *st3, float *norm3,
		float *rgba3, float *slrgba3, float *vlrgba3,
	int tex)
{
	int i;

	if(BTGE_Voxel_VoxelAlphaP(vox))
	{
		BTGE_Voxel_EmitAlphaFace3(chk, vox,
			xyz0, st0, norm0, rgba0, slrgba0, vlrgba0,
			xyz1, st1, norm1, rgba1, slrgba1, vlrgba1,
			xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
			xyz3, st3, norm3, rgba3, slrgba3, vlrgba3,
			tex);
		return;
	}

	BTGE_Voxel_EmitSolidFace3(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0, vlrgba0,
		xyz1, st1, norm1, rgba1, slrgba1, vlrgba1,
		xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
		xyz3, st3, norm3, rgba3, slrgba3, vlrgba3,
		tex);
}

BTGE_API void BTGE_Voxel_EmitBlockFluid(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	BTGE_VoxelData *vox1;

	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};
	static float nva[4]= { 0, 0, 1, 0};
	float vtxa[8*3];
	float rgba[4];
	int tex;
	float *pt1, *pt2, *pt3, *pt4;
	float f;
	int i, j;

	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z+1);
	if(vox1 && vox1->type)
		return;

	if(!btge_voxinfo[(vox->type)&4095])
		return;

	tex=btge_voxinfo[(vox->type)&4095]->img_top;

	f=(vox->aux&0xF0)/240.0;
	
	rgba[0]=1;
	rgba[1]=1;
	rgba[2]=1;
	rgba[3]=1;

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	pt1=vtxa+(4*3);
	pt2=vtxa+(6*3);
	pt3=vtxa+(7*3);
	pt4=vtxa+(5*3);
		
	BTGE_Voxel_EmitFace(chk, vox,
		pt4, sta+6, pt3, sta+4,
		pt2, sta+2, pt1, sta+0,
		nva, rgba, tex);
}



BTGE_API void BTGE_Voxel_EmitBlock_FluidFlow(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	int tex[3];
	BTGE_VoxelData *vox1;
	float *pt1, *pt2, *pt3, *pt4;
	float f, g, h, g0, g1, g2, g3;
	float h0, h1, h2, h3;
	int i, j, ty;
	
	//skip air
	if(!vox->type)
		return;

	if(!btge_voxinfo[(vox->type)&4095])
		return;

	//check bad block type
//	ty=vox->type|((vox->type2&15)<<8);
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

	tex[0]=btge_voxinfo[ty]->img_top;
	tex[1]=btge_voxinfo[ty]->img_side;
	tex[2]=btge_voxinfo[ty]->img_bot;

	f=(vox->aux&0xF0)/240.0;
	g=1;
	h=(vox->aux&0x0F)/7.0;
	
	if(btge_voxinfo[ty]->flags&BTGE_TXFL_FLUID)
	{
		g=0.75;
		
//		if((vox->type==VOX_TY_WATER2) || (vox->type==VOX_TY_LAVA2))
//		{
//			max[2]=min[2]+(max[2]-min[2])*h;
//		}
	}

#if 1
//	g0=1; g1=1; g2=1; g3=1;
	g0=0.875; g1=0.875; g2=0.875; g3=0.875;
//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x-1, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
	if(vox1 && (vox1->type==vox->type))
		g0=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g0=0;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x+1, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
	if(vox1 && (vox1->type==vox->type))
		g1=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g1=0;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y-1, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
	if(vox1 && (vox1->type==vox->type))
		g2=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g2=0;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y+1, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
	if(vox1 && (vox1->type==vox->type))
		g3=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g3=0;

	h0=(g0+g2)/2;
	h1=(g1+g2)/2;
	h2=(g0+g3)/2;
	h3=(g1+g3)/2;
	h0=(h0+h)/2;
	h1=(h1+h)/2;
	h2=(h2+h)/2;
	h3=(h3+h)/2;
#endif

//	f=(flag&0xF0)/240.0;
//	f=f*f;
//	f=0.05;
	
	f=1;
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	if(flag&32)
	{
		vtxa[4*3+2]=min[2]+(32*h0);
		vtxa[5*3+2]=min[2]+(32*h1);
		vtxa[6*3+2]=min[2]+(32*h2);
		vtxa[7*3+2]=min[2]+(32*h3);
	}

//	if((vox->type==VOX_TY_GRASS) && (flag&32))
	if(0)
//	if(((vox->type==VOX_TY_GRASS)||(vox->type==VOX_TY_DIRT)) && (flag&32))
//	if(flag&32)
	{
#if 0
		if((!(flag&1) && !(flag&64)) && (!(flag&4) && !(flag&256)))
			vtxa[4*3+2]+=32;
		if((!(flag&2) && !(flag&128)) && (!(flag&4) && !(flag&256)))
			vtxa[5*3+2]+=32;
		if((!(flag&1) && !(flag&64)) && (!(flag&8) && !(flag&512)))
			vtxa[6*3+2]+=32;
		if((!(flag&2) && !(flag&128)) && (!(flag&8) && !(flag&512)))
			vtxa[7*3+2]+=32;
#endif

#if 0
		if((flag&VOX_FL_NX) && (flag&VOX_FL_NY))
			vtxa[4*3+2]-=32;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_NY))
			vtxa[5*3+2]-=32;
		if((flag&VOX_FL_NX) && (flag&VOX_FL_PY))
			vtxa[6*3+2]-=32;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_PY))
			vtxa[7*3+2]-=32;
#endif

#if 1
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_NY)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXNY)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_NXNY)))
				{ vtxa[4*3+2]-=32*(1-h); vtxa[0*3+2]-=32*(1-h); }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_NY)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXNY)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_PXNY)))
				{ vtxa[5*3+2]-=32*(1-h); vtxa[1*3+2]-=32*(1-h); }
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_PY)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXPY)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_NXPY)))
				{ vtxa[6*3+2]-=32*(1-h); vtxa[2*3+2]-=32*(1-h); }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_PY)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXPY)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_PXPY)))
				{ vtxa[7*3+2]-=32*(1-h); vtxa[3*3+2]-=32*(1-h); }
#endif

//		if((flag&15)!=0)
//			flag|=15;

//		flag|=15;
	}

//	if((flag&15)!=0)
//		flag|=15;

	for(i=0; i<6; i++)
	{
//		if(flag&(65536<<i))
//			continue;

		if(!(flag&(1<<i)))
		{
			if((i==0) && (g0==1.0))continue;
			if((i==1) && (g1==1.0))continue;
			if((i==2) && (g2==1.0))continue;
			if((i==3) && (g3==1.0))continue;
			if((i==4)||(i==5))continue;
//			continue;
		}

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
//		BTGE_Voxel_EmitFace(chk, vox,
//			pt1, sta+0,
//			pt2, sta+2,
//			pt3, sta+4,
//			pt4, sta+6,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		BTGE_Voxel_EmitFace(chk, vox,
			pt4, sta+6,
			pt3, sta+4,
			pt2, sta+2,
			pt1, sta+0,
			nva+(i*4), rgba+(i*4),
			tex[vtxt[i]]);
	}
}


BTGE_API void BTGE_Voxel_EmitMaterialCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag)
{
	BTGE_Voxel_EmitMaterialColorCube(world,
		chk, vox, min, max, flag, 1, 1, 1);
}

BTGE_API void BTGE_Voxel_EmitMaterialColorCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	float cr, float cg, float cb)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4];
	int tex[3];
	float *pt1, *pt2, *pt3, *pt4, *pnv, *prgba;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	//check bad block type
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

	tex[0]=btge_voxinfo[ty]->img_top;
	tex[1]=btge_voxinfo[ty]->img_side;
	tex[2]=btge_voxinfo[ty]->img_bot;

//	f=(vox->aux&0xF0)/240.0;
	f=1;
	g=1;
	
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=1;

		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
//		BTGE_Voxel_EmitSolidFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		pnv=nva+(i*4);
//		prgba=rgba+(i*4);
		BTGE_Voxel_EmitSolidFace2(chk, vox,
			pt4, sta+6, pnv, brgba, slrgba,
			pt3, sta+4, pnv, brgba, slrgba,
			pt2, sta+2, pnv, brgba, slrgba,
			pt1, sta+0, pnv, brgba, slrgba,
			tex[vtxt[i]]);
	}
}

BTGE_API void BTGE_Voxel_EmitMaterialAlphaCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	float alpha, float gamma)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4];
	int tex[3];
	float *pt1, *pt2, *pt3, *pt4, *pnv, *prgba;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	//check bad block type
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

	tex[0]=btge_voxinfo[ty]->img_top;
	tex[1]=btge_voxinfo[ty]->img_side;
	tex[2]=btge_voxinfo[ty]->img_bot;

//	f=(vox->aux&0xF0)/240.0;
	f=1;
	g=1;
	
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		brgba[0]=gamma;
		brgba[1]=gamma;
		brgba[2]=gamma;
		brgba[3]=alpha;

//		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		f=1;
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
//		BTGE_Voxel_EmitSolidFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		pnv=nva+(i*4);
//		prgba=rgba+(i*4);
//		BTGE_Voxel_EmitSolidFace2(chk, vox,
		BTGE_Voxel_EmitAlphaFace2(chk, vox,
			pt4, sta+6, pnv, brgba, slrgba,
			pt3, sta+4, pnv, brgba, slrgba,
			pt2, sta+2, pnv, brgba, slrgba,
			pt1, sta+0, pnv, brgba, slrgba,
			tex[vtxt[i]]);
	}
}

BTGE_API void BTGE_Voxel_EmitMaterialTextureCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag, int tex)
{
	BTGE_Voxel_EmitMaterialTextureColorCube(world,
		chk, vox, min, max, flag, tex, 1, 1, 1);
}

BTGE_API void BTGE_Voxel_EmitMaterialTextureColorCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag, int tex,
	float cr, float cg, float cb)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4];
	float *pt1, *pt2, *pt3, *pt4, *pnv, *prgba;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	//check bad block type
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

//	f=(vox->aux&0xF0)/240.0;
	f=1;
	g=1;
	
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=1;

		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		pnv=nva+(i*4);
//		prgba=rgba+(i*4);
		BTGE_Voxel_EmitSolidFace2(chk, vox,
			pt4, sta+6, pnv, brgba, slrgba,
			pt3, sta+4, pnv, brgba, slrgba,
			pt2, sta+2, pnv, brgba, slrgba,
			pt1, sta+0, pnv, brgba, slrgba,
			tex);
	}
}

BTGE_API void BTGE_Voxel_EmitBlock_Fence(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	float mid[3], min2[3], max2[3];
	float h;
	
	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);

	min2[0]=mid[0]-4; min2[1]=mid[1]-4; min2[2]=min[2];
	max2[0]=mid[0]+4; max2[1]=mid[1]+4; max2[2]=max[2]+8;
	BTGE_Voxel_EmitMaterialCube(world, chk, vox,
		min2, max2, VOX_FL_ALL_CUBE);
	
	h=(mid[2]+max[2])*0.5;
	
	if(!(flag&VOX_FL_NX))
	{
		min2[0]=min[0];		min2[1]=mid[1]-2;	min2[2]=h-4;
		max2[0]=mid[0];		max2[1]=mid[1]+2;	max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}

	if(!(flag&VOX_FL_PX))
	{
		min2[0]=mid[0];		min2[1]=mid[1]-2;	min2[2]=h-4;
		max2[0]=max[0];		max2[1]=mid[1]+2;	max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}

	if(!(flag&VOX_FL_NY))
	{
		min2[0]=mid[0]-2;	min2[1]=min[1];		min2[2]=h-4;
		max2[0]=mid[0]+2;	max2[1]=mid[1];		max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}

	if(!(flag&VOX_FL_PY))
	{
		min2[0]=mid[0]-2;	min2[1]=mid[1];		min2[2]=h-4;
		max2[0]=mid[0]+2;	max2[1]=max[1];		max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}
}

BTGE_API void BTGE_Voxel_EmitBlock(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static const int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
//		4, 6, 7, 5,	//+Z
		6, 7, 5, 4, //+Z
	};
	static const int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static const float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static const float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};
	static const int sti[6*2]= { 1, 2, 1, 2, 0, 2, 0, 2, 0, 1, 0, 1};

	static const float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float stb[8];
	float vtxa[8*3];
	float rgba[6*4];
	float vgam[6];
	float vlgam[6*3];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4], vlrgba[4];
	int tex[3], pos[3];
	BTGE_VoxelData *vox1;
	float *pt1, *pt2, *pt3, *pt4;
	float cr, cg, cb;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	//skip air
	if(!vox->type)
		return;
	if(!(vox->type&VOX_TY_MASK))
		return;

	if(!btge_voxinfo[(vox->type)&4095])
		return;

//	ty=vox->type|((vox->type2&15)<<8);
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->EmitBlock)
	{
		btge_voxinfo[ty]->EmitBlock(
			world, chk, vox,
			min, max, flag, x, y, z);
		return;
	}
	
	//check bad block type
	if(btge_voxinfo[ty]->img_top<=0)
		return;

#if 0
	if(btge_voxinfo[ty]->flags&BTGE_TXFL_FLUID)
	{
		BTGE_Voxel_EmitBlockFluid(world, chk, vox, min, max, flag, x, y, z);
		return;
	}
#endif

	if((ty==VOX_TY_WATER2) || (ty==VOX_TY_LAVA2))
	{
		BTGE_Voxel_EmitBlock_FluidFlow(
			world, chk, vox, min, max, flag, x, y, z);
		return;
	}

	if((ty==VOX_TY_FENCE) || (ty==VOX_TY_NETHERBRICK_FENCE))
	{
		BTGE_Voxel_EmitBlock_Fence(
			world, chk, vox, min, max, flag, x, y, z);
		return;
	}

	tex[0]=btge_voxinfo[ty]->img_top;
	tex[1]=btge_voxinfo[ty]->img_side;
	tex[2]=btge_voxinfo[ty]->img_bot;
	pos[0]=x; pos[1]=y; pos[2]=z;

	f=BTGE_Voxel_GetVoxelGamma(chk, vox);
	for(i=0; i<6; i++)
	{
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn,
			x+nva[i*4+0], y+nva[i*4+1], z+nva[i*4+2]);
		f=BTGE_Voxel_GetVoxelGamma(chk, vox1);

		vgam[i]=f;
		
		if(vox1)
		{
//			j=vox1->vlight;
			j=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
//			j=(15<<4)|1;
			BTGE_Voxel_GetVLightColor(world, j, vlgam+i*3);
		}else
		{
			V3F_ZERO(vlgam+i*3);
//			vlgam[i*3+0]=1;
		}
		
//		vlgam[i*3+0]=1;
	}

//	f=(vox->aux&0xF0)/240.0;
	f=1;
	g=1;
	
	if(btge_voxinfo[ty]->flags&BTGE_TXFL_FLUID)
	{
		g=0.75;
		
		if((ty==VOX_TY_WATER2) || (ty==VOX_TY_LAVA2))
		{
			h=(vox->aux&0x0F)/7.0;
//			max[2]=min[2]+(max[2]-min[2])*h;
		}

		if((ty==VOX_TY_WATER) || (ty==VOX_TY_LAVA))
		{
			h=7.0/8.0;
//			max[2]=min[2]+(max[2]-min[2])*h;
		}
	}

//	f=(flag&0xF0)/240.0;
//	f=f*f;
//	f=0.05;

	switch(ty)
	{
	case VOX_TY_CLOTH:
	case VOX_TY_REDSTONE_LAMP0:
	case VOX_TY_REDSTONE_LAMP1:
		cr=(vox->aux&1)?0.5:1;
		cg=(vox->aux&2)?0.5:1;
		cb=(vox->aux&4)?0.5:1;
		if(vox->aux&8)
			{ cr*=0.25; cg*=0.25; cb*=0.25; }
		break;
	default: cr=1; cg=1; cb=1; break;
	}

	if(btge_voxinfo[ty]->flags&BTGE_TXFL_ALPHA)
		f=2;

	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=cr*f*gam[i];
		rgba[i*4+1]=cg*f*gam[i];
		rgba[i*4+2]=cb*f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	if(ty==VOX_TY_SLAB)
	{
		vtxa[4*3+2]-=16;
		vtxa[5*3+2]-=16;
		vtxa[6*3+2]-=16;
		vtxa[7*3+2]-=16;
	}

//	if((vox->type==VOX_TY_GRASS) && (flag&32))
//	if(0)
//	if(((vox->type==VOX_TY_GRASS)||(vox->type==VOX_TY_DIRT)) && (flag&32))
//	if(flag&32)
	if(((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM) &&
		(flag&VOX_FL_PZ) && btge_voxel_use_flexform)
	{
		dk=32;
//		dk=16;
		fl2=0;
	
#if 1
		if((!(flag&VOX_FL_NX) && !(flag&VOX_FL_NXPZ)) &&
			(!(flag&VOX_FL_NY) && !(flag&VOX_FL_NYPZ)) &&
			(!(flag&VOX_FL_NXNY) && !(flag&VOX_FL_NXNYPZ)))
				{ fl2|=1; }
		if((!(flag&VOX_FL_PX) && !(flag&VOX_FL_PXPZ)) &&
			(!(flag&VOX_FL_NY) && !(flag&VOX_FL_NYPZ)) &&
			(!(flag&VOX_FL_PXNY) && !(flag&VOX_FL_PXNYPZ)))
				{ fl2|=2; }
		if((!(flag&VOX_FL_NX) && !(flag&VOX_FL_NXPZ)) &&
			(!(flag&VOX_FL_PY) && !(flag&VOX_FL_PYPZ)) &&
			(!(flag&VOX_FL_NXPY) && !(flag&VOX_FL_NXPYPZ)))
				{ fl2|=4; }
		if((!(flag&VOX_FL_PX) && !(flag&VOX_FL_PXPZ)) &&
			(!(flag&VOX_FL_PY) && !(flag&VOX_FL_PYPZ)) &&
			(!(flag&VOX_FL_PXPY) && !(flag&VOX_FL_PXPYPZ)))
				{ fl2|=8; }
#endif

#if 0
		if((flag&VOX_FL_NX) && (flag&VOX_FL_NY))
			vtxa[4*3+2]-=dk;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_NY))
			vtxa[5*3+2]-=dk;
		if((flag&VOX_FL_NX) && (flag&VOX_FL_PY))
			vtxa[6*3+2]-=dk;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_PY))
			vtxa[7*3+2]-=dk;
#endif

#if 1
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_NY) &&
				(flag&VOX_FL_NXNY) && (flag&VOX_FL_NXNYPZ)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXNY) &&
				(flag&VOX_FL_NYPZ)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_NXNY) &&
				(flag&VOX_FL_NXPZ)))
			{ fl2|=16; }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_NY) &&
				(flag&VOX_FL_PXNY) && (flag&VOX_FL_PXNYPZ)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXNY) &&
				(flag&VOX_FL_NYPZ)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_PXNY) &&
				(flag&VOX_FL_PXPZ)))
			{ fl2|=32; }
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_PY) &&
				(flag&VOX_FL_NXPY) && (flag&VOX_FL_NXPYPZ)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXPY) &&
				(flag&VOX_FL_PYPZ)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_NXPY) &&
				(flag&VOX_FL_NXPZ)))
			{ fl2|=64; }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_PY) &&
				(flag&VOX_FL_PXPY) && (flag&VOX_FL_PXPYPZ)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXPY) &&
				(flag&VOX_FL_PYPZ)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_PXPY) &&
				(flag&VOX_FL_PXPZ)))
			{ fl2|=128; }
#endif

#if 0
		if((flag&VOX_FL_NX) && (flag&VOX_FL_NY) && (flag&VOX_FL_NXNY))
				{ fl2|=16; }
		if((flag&VOX_FL_PX) && (flag&VOX_FL_NY) && (flag&VOX_FL_PXNY))
				{ fl2|=32; }
		if((flag&VOX_FL_NX) && (flag&VOX_FL_PY) && (flag&VOX_FL_NXPY))
				{ fl2|=64; }
		if((flag&VOX_FL_PX) && (flag&VOX_FL_PY) && (flag&VOX_FL_PXPY))
				{ fl2|=128; }
#endif

		if(fl2==0xF0)
		{
			vtxa[4*3+2]+=dk/2; vtxa[0*3+2]+=dk/2;
			vtxa[5*3+2]+=dk/2; vtxa[1*3+2]+=dk/2;
			vtxa[6*3+2]+=dk/2; vtxa[2*3+2]+=dk/2;
			vtxa[7*3+2]+=dk/2; vtxa[3*3+2]+=dk/2;
		}

		if(fl2==0x0F)
		{
			vtxa[4*3+2]-=dk/2;
			vtxa[5*3+2]-=dk/2;
			vtxa[6*3+2]-=dk/2;
			vtxa[7*3+2]-=dk/2;
		}

		if(fl2&1)		{ vtxa[4*3+2]+=dk; }
		if(fl2&2)		{ vtxa[5*3+2]+=dk; }
		if(fl2&4)		{ vtxa[6*3+2]+=dk; }
		if(fl2&8)		{ vtxa[7*3+2]+=dk; }
		if(fl2&16)		{ vtxa[4*3+2]-=dk; vtxa[0*3+2]-=dk; }
		if(fl2&32)		{ vtxa[5*3+2]-=dk; vtxa[1*3+2]-=dk; }
		if(fl2&64)		{ vtxa[6*3+2]-=dk; vtxa[2*3+2]-=dk; }
		if(fl2&128)		{ vtxa[7*3+2]-=dk; vtxa[3*3+2]-=dk; }

//		if((flag&15)!=0)
//			flag|=15;

//		flag|=15;
	}

//	if((flag&15)!=0)
//		flag|=15;

	for(i=0; i<6; i++)
	{
//		if(flag&(65536<<i))
//			continue;

		if(!(flag&(1<<i)))
			continue;

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
		if(btge_voxinfo[ty]->img_scale[vtxt[i]]!=1.0)
		{
			f=btge_voxinfo[ty]->img_scale[vtxt[i]];
			for(j=0; j<4; j++)
			{
				stb[j*2+0]=(pos[sti[i*2+0]]+sta[j*2+0])*f;
				stb[j*2+1]=(pos[sti[i*2+1]]+sta[j*2+1])*f;
			}
		}else
		{
			V4F_COPY(sta+0, stb+0);
			V4F_COPY(sta+4, stb+4);
		}
		
//		BTGE_Voxel_EmitFace(chk, vox,
//			pt1, sta+0,
//			pt2, sta+2,
//			pt3, sta+4,
//			pt4, sta+6,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

//		BTGE_Voxel_EmitFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=1;

//		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		f=vgam[i];
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		vlrgba[0]=vlgam[i*3+0]*brgba[0]*gam[i];
		vlrgba[1]=vlgam[i*3+1]*brgba[1]*gam[i];
		vlrgba[2]=vlgam[i*3+2]*brgba[2]*gam[i];
		vlrgba[3]=brgba[3];

		V3F_SUB(pt3, pt4, tv0);
		V3F_SUB(pt2, pt3, tv1);
		V3F_CROSS(tv0, tv1, tv2);

		V3F_SUB(pt2, pt3, tv0);
		V3F_SUB(pt1, pt2, tv1);
		V3F_CROSS(tv0, tv1, tv3);
		
		V3F_ADD(tv2, tv3, tv0);
		V3F_NORMALIZE(tv0, nv);

//		BTGE_Voxel_EmitFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nv, rgba+(i*4),
//			tex[vtxt[i]]);

//		BTGE_Voxel_EmitSolidFace2(chk, vox,
		BTGE_Voxel_EmitFace3(chk, vox,
			pt4, stb+6, nv, brgba, slrgba, vlrgba,
			pt3, stb+4, nv, brgba, slrgba, vlrgba,
			pt2, stb+2, nv, brgba, slrgba, vlrgba,
			pt1, stb+0, nv, brgba, slrgba, vlrgba,
			tex[vtxt[i]]);
	}
}

static void lbxgl_swapvec(float *a, float *b, int n)
{
	float f;
	int i;
	
	for(i=0; i<n; i++)
		{ f=a[i]; a[i]=b[i]; b[i]=f; }
}

BTGE_API void BTGE_Voxel_RebuildSwapFaces(
	BTGE_VoxelChunk *chk, int a, int b)
{
	float f, g;
	int i, j;
	
	if(a==b)return;
	
	lbxgl_swapvec(chk->face_xyz +a*12, chk->face_xyz +b*12, 12);
	lbxgl_swapvec(chk->face_st  +a* 8, chk->face_st  +b* 8,  8);
	lbxgl_swapvec(chk->face_norm+a*12, chk->face_norm+b*12, 12);
	lbxgl_swapvec(chk->face_rgba+a*16, chk->face_rgba+b*16, 16);
	lbxgl_swapvec(chk->face_slrgba+a*16, chk->face_slrgba+b*16, 16);
	lbxgl_swapvec(chk->face_vlrgba+a*16, chk->face_vlrgba+b*16, 16);

	i=chk->face_tex[a];
	j=chk->face_tex[b];
	chk->face_tex[a]=j;
	chk->face_tex[b]=i;
}

BTGE_API void BTGE_Voxel_RebuildSwapAlphaFaces(
	BTGE_VoxelChunk *chk, int a, int b)
{
	float f, g;
	int i, j;
	
	if(a==b)return;
	
	lbxgl_swapvec(chk->aface_xyz +a*12, chk->aface_xyz +b*12, 12);
	lbxgl_swapvec(chk->aface_st  +a* 8, chk->aface_st  +b* 8,  8);
	lbxgl_swapvec(chk->aface_norm+a*12, chk->aface_norm+b*12, 12);
	lbxgl_swapvec(chk->aface_rgba+a*16, chk->aface_rgba+b*16, 16);
	lbxgl_swapvec(chk->aface_slrgba+a*16, chk->aface_slrgba+b*16, 16);
	lbxgl_swapvec(chk->aface_vlrgba+a*16, chk->aface_vlrgba+b*16, 16);

	i=chk->aface_tex[a];
	j=chk->aface_tex[b];
	chk->aface_tex[a]=j;
	chk->aface_tex[b]=i;
}

BTGE_API void BTGE_Voxel_RebuildSortFaces(BTGE_VoxelChunk *chk)
{
	int i, j;
	
	for(i=0; i<chk->n_face; i++)
		for(j=i+1; j<chk->n_face; j++)
	{
		if(chk->face_tex[j]<chk->face_tex[i])
			BTGE_Voxel_RebuildSwapFaces(chk, i, j);
	}

	for(i=0; i<chk->n_aface; i++)
		for(j=i+1; j<chk->n_aface; j++)
	{
		if(chk->aface_tex[j]<chk->aface_tex[i])
			BTGE_Voxel_RebuildSwapAlphaFaces(chk, i, j);
	}
}

BTGE_API void BTGE_Voxel_RebuildChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox;
	float org[3], mins[3], maxs[3];
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l, nvox;

	BTGE_Voxel_Setup();

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

//	chk->tickdirty=1;	//wake chunk up
	chk->tickthink=1;	//wake chunk up

	chk->n_face=0;
	chk->n_aface=0;
	nvox=0;

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
			for(k=0; k<chk->zs; k++)
	{
		l=0;
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+k;

		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		if(!vox)continue;

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

#if 1
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

		org[0]=x*vsz; org[1]=y*vsz; org[2]=z*vsz;
		mins[0]=org[0]; mins[1]=org[1]; mins[2]=org[2];
		maxs[0]=org[0]+vsz; maxs[1]=org[1]+vsz; maxs[2]=org[2]+vsz;

		BTGE_Voxel_EmitBlock(world, chk, vox, mins, maxs, l, x, y, z);
		
		nvox++;
	}
	
	BTGE_Voxel_RebuildSortFaces(chk);
	
//	printf("BTGE_Voxel_RebuildChunk: %d vox, %d face %d aface\n",
//		nvox, chk->n_face, chk->n_aface);
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
	BTGE_VoxelData *vox2;
//	vox2=BTGE_Voxel_GetRegionVoxel(rgn, x, y, z);
	vox2=BTGE_Voxel_GetWorldRegionVoxel(world, rgn, x, y, z);
	if(!vox2)return(false);
	if(BTGE_Voxel_VoxelOpaqueP(vox2))
	{
		if(BTGE_Voxel_VoxelFlexformP(vox2))
		{
			if(BTGE_Voxel_VoxelFluidP(vox))
				return(true);

			if(!BTGE_Voxel_VoxelFlexformP(vox))
				return(false);
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
	if(!chk && !(fl&VOX_GFL_DELTA_LOAD))
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
		chk=gcalloc(sizeof(BTGE_VoxelChunk));
		rgn->chk[(tx*rgn->ys+ty)*rgn->zs+tz]=chk;
		chk->rgn=rgn;
		
		chk->ox=rgn->ox+tx*(rgn->xst);
		chk->oy=rgn->oy+ty*(rgn->yst);
		chk->oz=rgn->oz+tz*(rgn->zst);
		chk->xs=rgn->xst;
		chk->ys=rgn->yst;
		chk->zs=rgn->zst;
		
		i=chk->xs*chk->ys*chk->zs;
		chk->vox=gcatomic(i*sizeof(BTGE_VoxelData));
	}

//	tx=rx%rgn->xst;
//	ty=ry%rgn->yst;
//	tz=rz%rgn->zst;

	tx=rx&(rgn->xst-1);
	ty=ry&(rgn->yst-1);
	tz=rz&(rgn->zst-1);

	//mark chunk as dirty (tick and rebuild)
	if(fl&VOX_GFL_DIRTY)
	{
		chk->clean=0;
		chk->tickdirty=1;
		chk->flags|=VOX_CHFL_DELTADIRTY;
		chk->flags|=VOX_CHFL_LIGHT_DIRTY;
		if(!(fl&VOX_GFL_DELTA_LOAD))
			{ chk->flags&=~VOX_CHFL_SAVE_OK; }
	}

	//mark chunk as dirty (rebuild only)
	if(fl&VOX_GFL_REBUILD)
	{
		chk->clean=0;
//		chk->tickdirty=1;
		chk->flags|=VOX_CHFL_DELTADIRTY;
		chk->flags|=VOX_CHFL_LIGHT_DIRTY;
		if(!(fl&VOX_GFL_DELTA_LOAD))
			{ chk->flags&=~VOX_CHFL_SAVE_OK; }
	}

	if(fl&VOX_GFL_ENTITY_MOVE)
	{
		chk->flags&=~VOX_CHFL_SAVE_OK;
	}

//	vox=&(chk->vox[(tx*chk->ys+ty)*chk->zs+tz]);
	vox=&(chk->vox[(tz*chk->ys+ty)*chk->xs+tx]);

	return(vox);
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

	if(fl&1)
	{
		//if marking dirty, create a new region if-needed
//		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl|2);
		if(!cur)return(NULL);
		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
		if(vox)return(vox);
		return(NULL);
	}

	if(fl&4)
	{
		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
//		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl|2);
		if(!cur)return(NULL);
		vox=BTGE_Voxel_GetRegionVoxelFlags2(world, cur, x, y, z, fl);
		if(vox)return(vox);
		return(NULL);
	}

	return(NULL);
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

	if(fl&1)
	{
		//if marking dirty, create a new region if-needed
//		cur=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
		cur=BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y, z, fl);
		return(cur);
	}

	return(NULL);
}

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
		if(!chk->n_face)
			continue;
		
		chk->vnext=rgn->vischk;
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
	double org[3], org1[3];
	double am[4], an[4];
	int ami[4], ani[4];
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	float f, g, br;
	int i, j, k, l0, l1, ty;
	
	vdvec3d(org, origin);

	am[0]=org[0]-radius;
	am[1]=org[1]-radius;
	am[2]=org[2]-radius;

	an[0]=org[0]+radius;
	an[1]=org[1]+radius;
	an[2]=org[2]+radius;

	ami[0]=am[0]/32-1; ami[1]=am[1]/32-1; ami[2]=am[2]/32-1;
	ani[0]=an[0]/32+1; ani[1]=an[1]/32+1; ani[2]=an[2]/32+1;

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
			
			f=f-16; if(f<0)f=0;
			
			//out of range
			if(f>=radius)continue;
			
			//no damage
			g=damage-f;
			if(g<=0)continue;

			inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, i, j, k);
			if(inf)
			{
				br=inf->blast_resist;
			}else
			{
				br=30;
			}

			//below threshold
			if(g<=br)
			{
				vox=BTGE_Voxel_GetWorldVoxelDirty(world, i, j, k);
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
