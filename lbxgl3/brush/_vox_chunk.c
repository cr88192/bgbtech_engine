#include <lbxgl2.h>

static char *bgb_voxnames[]=
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
"bricks",				//45
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
"netherwark",			//115
"enchant_table",		//116
"brewstand",			//117
"cauldron",				//118
"airportal",			//119
"crystal",				//120

NULL
};

LBXGL_VoxelInfo bgb_voxinfo[256];
byte bgb_voxel_opaque[256];

void BGB_Voxel_Setup()
{
	static int init=0;
	char tb[256];
	int i;
	
	if(init)return;
	init=1;
	
	for(i=0; i<256; i++)
	{
		bgb_voxel_opaque[i]=0;	//for now, assume transparent
		if(!bgb_voxnames[i])
			break;

		bgb_voxel_opaque[i]=1;	//for now, assume opaque
		bgb_voxinfo[i].name=bgb_voxnames[i];

		sprintf(tb, "textures/voxel/%s", bgb_voxnames[i]);
		bgb_voxinfo[i].img_top=LBXGL_Texture_LoadImage(tb);
		bgb_voxinfo[i].img_side=bgb_voxinfo[i].img_top;
		bgb_voxinfo[i].img_bot=bgb_voxinfo[i].img_top;

		if(bgb_voxinfo[i].img_top<=0)
		{
			sprintf(tb, "textures/voxel/%s_top", bgb_voxnames[i]);
			bgb_voxinfo[i].img_top=LBXGL_Texture_LoadImage(tb);
			sprintf(tb, "textures/voxel/%s_side", bgb_voxnames[i]);
			bgb_voxinfo[i].img_side=LBXGL_Texture_LoadImage(tb);
			sprintf(tb, "textures/voxel/%s_bot", bgb_voxnames[i]);
			bgb_voxinfo[i].img_bot=LBXGL_Texture_LoadImage(tb);

			if(!bgb_voxinfo[i].img_bot)
				bgb_voxinfo[i].img_bot=
					bgb_voxinfo[i].img_top;
		}
		
		if(bgb_voxinfo[i].img_top>0)
		{
			bgb_voxinfo[i].flags=
				LBXGL_Texture_GetImageFlags(bgb_voxinfo[i].img_top);
		}
		if(bgb_voxinfo[i].img_side>0)
		{
			bgb_voxinfo[i].flags|=
				LBXGL_Texture_GetImageFlags(bgb_voxinfo[i].img_side);
		}
	}

	bgb_voxel_opaque[0]=0;	//air is not opaque

	bgb_voxinfo[VOX_TY_WATER].flags|=LBXGL_TXFL_FLUID_WATER;
	bgb_voxinfo[VOX_TY_WATER2].flags|=LBXGL_TXFL_FLUID_WATER;
	bgb_voxinfo[VOX_TY_LAVA].flags|=LBXGL_TXFL_FLUID_LAVA;
	bgb_voxinfo[VOX_TY_LAVA2].flags|=LBXGL_TXFL_FLUID_LAVA;

	for(i=0; i<256; i++)
	{
		if(bgb_voxinfo[i].flags&LBXGL_TXFL_ALPHA)
			bgb_voxel_opaque[i]=0;	//ok, not opaque
	}
}

bool LBXGL_Voxel_VoxelOpaqueP(LBXGL_VoxelData *vox)
{
	if(!vox)return(false);
	if(!vox->type)
		return(0);
	return(bgb_voxel_opaque[vox->type]);
//	return(1);
}

void LBXGL_Voxel_FlushChunk(LBXGL_VoxelChunk *chk)
{
	chk->clean=0;
}

void BGB_Voxel_CheckExpandFace(LBXGL_VoxelChunk *chk)
{
	int i;

	if(!chk->face_xyz)
	{
		chk->face_xyz=gcatomic(64*4*3*sizeof(float));
		chk->face_st=gcatomic(64*4*2*sizeof(float));
		chk->face_norm=gcatomic(64*4*3*sizeof(float));
		chk->face_rgba=gcatomic(64*4*4*sizeof(float));
		chk->face_tex=gcatomic(64*1*1*sizeof(int));
		chk->n_face=0;
		chk->m_face=64;
	}
	
	if((chk->n_face+1)>=chk->m_face)
	{
		i=chk->m_face+(chk->m_face>>1);
		chk->face_xyz=gcrealloc(chk->face_xyz, i*4*3*sizeof(float));
		chk->face_st=gcrealloc(chk->face_st, i*4*2*sizeof(float));
		chk->face_norm=gcrealloc(chk->face_norm, i*4*3*sizeof(float));
		chk->face_rgba=gcrealloc(chk->face_rgba, i*4*4*sizeof(float));
		chk->face_tex=gcrealloc(chk->face_tex, i*1*1*sizeof(int));
		chk->m_face=i;
	}
}

void BGB_Voxel_EmitFace(
	LBXGL_VoxelChunk *chk, LBXGL_VoxelData *vox,
	float *xyz0, float *st0,
	float *xyz1, float *st1,
	float *xyz2, float *st2,
	float *xyz3, float *st3,
	float *norm, float *rgba,
	int tex)
{
	int i;

	BGB_Voxel_CheckExpandFace(chk);
	
	i=chk->n_face++;
	
	chk->face_tex[i]=tex;
	
	V3F_COPY(xyz0, chk->face_xyz+i*4*3+0);
	V3F_COPY(xyz1, chk->face_xyz+i*4*3+3);
	V3F_COPY(xyz2, chk->face_xyz+i*4*3+6);
	V3F_COPY(xyz3, chk->face_xyz+i*4*3+9);

	V2F_COPY(st0, chk->face_st+i*4*2+0);
	V2F_COPY(st1, chk->face_st+i*4*2+2);
	V2F_COPY(st2, chk->face_st+i*4*2+4);
	V2F_COPY(st3, chk->face_st+i*4*2+6);

	V3F_COPY(norm, chk->face_norm+i*4*3+0);
	V3F_COPY(norm, chk->face_norm+i*4*3+3);
	V3F_COPY(norm, chk->face_norm+i*4*3+6);
	V3F_COPY(norm, chk->face_norm+i*4*3+9);

	V4F_COPY(rgba, chk->face_rgba+i*4*4+ 0);
	V4F_COPY(rgba, chk->face_rgba+i*4*4+ 4);
	V4F_COPY(rgba, chk->face_rgba+i*4*4+ 8);
	V4F_COPY(rgba, chk->face_rgba+i*4*4+12);
}

void BGB_Voxel_EmitBlock(
	LBXGL_VoxelChunk *chk, LBXGL_VoxelData *vox,
	float *min, float *max, int flag)
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
	float *pt1, *pt2, *pt3, *pt4;
	float f;
	int i, j;
	
	//skip air
	if(!vox->type)
		return;
	
	//check bad block type
	if(bgb_voxinfo[vox->type].img_top<=0)
		return;

//	if(bgb_voxinfo[type].mt_contents&(CONTENTS_WATER|CONTENTS_LAVA))
//	{
//		BGB_Voxel_DrawFluid(min, max, type, flag);
//		return;
//	}

	tex[0]=bgb_voxinfo[vox->type].img_top;
	tex[1]=bgb_voxinfo[vox->type].img_side;
	tex[2]=bgb_voxinfo[vox->type].img_bot;

	f=(vox->aux&0xF0)/240.0;
	
//	f=(flag&0xF0)/240.0;
//	f=f*f;
//	f=0.05;
	
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=1;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

//	if((vox->type==VOX_TY_GRASS) && (flag&32))
	if(((vox->type==VOX_TY_GRASS)||(vox->type==VOX_TY_DIRT)) && (flag&32))
	{
#if 1
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
				{ vtxa[4*3+2]-=32; vtxa[0*3+2]-=32; }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_NY)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXNY)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_PXNY)))
				{ vtxa[5*3+2]-=32; vtxa[1*3+2]-=32; }
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_PY)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXPY)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_NXPY)))
				{ vtxa[6*3+2]-=32; vtxa[2*3+2]-=32; }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_PY)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXPY)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_PXPY)))
				{ vtxa[7*3+2]-=32; vtxa[3*3+2]-=32; }
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
			continue;

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
//		BGB_Voxel_EmitFace(chk, vox,
//			pt1, sta+0,
//			pt2, sta+2,
//			pt3, sta+4,
//			pt4, sta+6,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		BGB_Voxel_EmitFace(chk, vox,
			pt4, sta+6,
			pt3, sta+4,
			pt2, sta+2,
			pt1, sta+0,
			nva+(i*4), rgba+(i*4),
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

void LBXGL_Voxel_RebuildSwapFaces(LBXGL_VoxelChunk *chk, int a, int b)
{
	float f, g;
	int i, j;
	
	lbxgl_swapvec(chk->face_xyz +a*12, chk->face_xyz +b*12, 12);
	lbxgl_swapvec(chk->face_st  +a* 8, chk->face_st  +b* 8,  8);
	lbxgl_swapvec(chk->face_norm+a*12, chk->face_norm+b*12, 12);
	lbxgl_swapvec(chk->face_rgba+a*16, chk->face_rgba+b*16, 16);

	i=chk->face_tex[a];
	j=chk->face_tex[b];
	chk->face_tex[a]=j;
	chk->face_tex[b]=i;
}

void LBXGL_Voxel_RebuildSortFaces(LBXGL_VoxelChunk *chk)
{
	int i, j;
	
	for(i=0; i<chk->n_face; i++)
		for(j=i+1; j<chk->n_face; j++)
	{
		if(chk->face_tex[j]<chk->face_tex[i])
			LBXGL_Voxel_RebuildSwapFaces(chk, i, j);
	}
}

void LBXGL_Voxel_RebuildChunk(LBXGL_VoxelChunk *chk)
{
	LBXGL_VoxelData *vox;
	float org[3], mins[3], maxs[3];
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l, nvox;

	BGB_Voxel_Setup();

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

	chk->n_face=0; nvox=0;

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
			for(k=0; k<chk->zs; k++)
	{
		l=0;
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+k;

		vox=LBXGL_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		if(!vox)continue;

		if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y, z))l|=1;
		if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y, z))l|=2;
		if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y-1, z))l|=4;
		if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y+1, z))l|=8;
		if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y, z-1))l|=16;
		if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y, z+1))l|=32;
		if(!l)continue;

		if(vox->type==VOX_TY_GRASS)
		{
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y, z+1))l|=64;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y, z+1))l|=128;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y-1, z+1))l|=256;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y+1, z+1))l|=512;

			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y+1, z))l|=1024;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y-1, z))l|=2048;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y+1, z))l|=4096;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y-1, z))l|=8192;

			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y+1, z+1))l|=16384;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y-1, z+1))l|=32768;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y+1, z+1))l|=65536;
			if(!LBXGL_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y-1, z+1))l|=131072;
		}
		
		org[0]=x*vsz; org[1]=y*vsz; org[2]=z*vsz;
		mins[0]=org[0]-vsz2; mins[1]=org[1]-vsz2; mins[2]=org[2]-vsz2;
		maxs[0]=org[0]+vsz2; maxs[1]=org[1]+vsz2; maxs[2]=org[2]+vsz2;

		BGB_Voxel_EmitBlock(chk, vox, mins, maxs, l);
		
		nvox++;
	}
	
	LBXGL_Voxel_RebuildSortFaces(chk);
	
	printf("LBXGL_Voxel_RebuildChunk: %d vox, %d face\n",
		nvox, chk->n_face);
}

void LBXGL_Voxel_CheckRebuildChunk(LBXGL_VoxelChunk *chk)
{
	if(chk->clean)return;
	LBXGL_Voxel_RebuildChunk(chk);
	chk->clean=1;
}

void LBXGL_Voxel_BoxChunk(LBXGL_VoxelChunk *chk,
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

void LBXGL_Voxel_BoxRegion(LBXGL_VoxelRegion *rgn,
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

bool LBXGL_Voxel_CheckChunkFrustum(LBXGL_VoxelChunk *chk)
{
	float mins[3], maxs[3];
	LBXGL_Voxel_BoxChunk(chk, mins, maxs);
	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(false);
	return(true);
}

bool LBXGL_Voxel_CheckRegionFrustum(LBXGL_VoxelRegion *rgn)
{
	float mins[3], maxs[3];
	LBXGL_Voxel_BoxRegion(rgn, mins, maxs);
	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(false);
	return(true);
}

bool LBXGL_Voxel_RegionVoxelOpaqueP(LBXGL_VoxelRegion *rgn,
	int x, int y, int z)
{
	LBXGL_VoxelData *vox;
	vox=LBXGL_Voxel_GetRegionVoxel(rgn, x, y, z);
	if(!vox)return(false);
	return(LBXGL_Voxel_VoxelOpaqueP(vox));
}

LBXGL_VoxelData *LBXGL_Voxel_GetRegionVoxelFlags(LBXGL_VoxelRegion *rgn,
	int x, int y, int z, int fl)
{
	LBXGL_VoxelChunk *chk;
	LBXGL_VoxelData *vox;
	int rx, ry, rz;
	int tx, ty, tz;
	int i;

	rx=x-rgn->ox;
	ry=y-rgn->oy;
	rz=z-rgn->oz;

	if(rx<0)return(NULL);
	if(ry<0)return(NULL);
	if(rz<0)return(NULL);

	tx=rx/rgn->xst;
	ty=ry/rgn->yst;
	tz=rz/rgn->zst;

	if(tx>=rgn->xs)return(NULL);
	if(ty>=rgn->ys)return(NULL);
	if(tz>=rgn->zs)return(NULL);
	
	chk=rgn->chk[(tx*rgn->ys+ty)*rgn->zs+tz];

	//no chunk...
	if(!chk)
	{
		//if not modifying chunks, return NULL
		if(!(fl&1))return(NULL);
		
		//ok, create a new chunk
		chk=gcalloc(sizeof(LBXGL_VoxelChunk));
		rgn->chk[(tx*rgn->ys+ty)*rgn->zs+tz]=chk;
		chk->rgn=rgn;
		
		chk->ox=rgn->ox+tx*(rgn->xst);
		chk->oy=rgn->oy+ty*(rgn->yst);
		chk->oz=rgn->oz+tz*(rgn->zst);
		chk->xs=rgn->xst;
		chk->ys=rgn->yst;
		chk->zs=rgn->zst;
		
		i=chk->xs*chk->ys*chk->zs;
		chk->vox=gcatomic(i*sizeof(LBXGL_VoxelData));
	}

	tx=rx%rgn->xst;
	ty=ry%rgn->yst;
	tz=rz%rgn->zst;

	//mark chunk as dirty
	if(fl&1) { chk->clean=0; }

	vox=&(chk->vox[(tx*chk->ys+ty)*chk->zs+tz]);
	return(vox);
}

LBXGL_VoxelData *LBXGL_Voxel_GetRegionVoxel(LBXGL_VoxelRegion *rgn,
	int x, int y, int z)
	{ return(LBXGL_Voxel_GetRegionVoxelFlags(rgn, x, y, z, 0)); }

LBXGL_VoxelData *LBXGL_Voxel_GetRegionVoxelDirty(LBXGL_VoxelRegion *rgn,
	int x, int y, int z)
	{ return(LBXGL_Voxel_GetRegionVoxelFlags(rgn, x, y, z, 1)); }

LBXGL_VoxelData *LBXGL_Voxel_GetWorldVoxelFlags(LBXGL_BrushWorld *world,
	int x, int y, int z, int fl)
{
	LBXGL_VoxelRegion *cur;
	LBXGL_VoxelData *vox;

	cur=world->vox_region;
	while(cur)
	{
		vox=LBXGL_Voxel_GetRegionVoxelFlags(cur, x, y, z, fl);
		if(vox)return(vox);
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_VoxelData *LBXGL_Voxel_GetWorldVoxel(LBXGL_BrushWorld *world,
	int x, int y, int z)
	{ return(LBXGL_Voxel_GetWorldVoxelFlags(world, x, y, z, 0)); }

LBXGL_VoxelData *LBXGL_Voxel_GetWorldVoxelDirty(LBXGL_BrushWorld *world,
	int x, int y, int z)
	{ return(LBXGL_Voxel_GetWorldVoxelFlags(world, x, y, z, 1)); }

void LBXGL_Voxel_RebuildRegionVisible(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *chk;
	int i, j, k, l, n, m;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	rgn->vischk=NULL; n=0; m=0;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
			for(k=0; k<rgn->zs; k++)
	{
		chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+k];
		if(!chk)continue;
		if(!LBXGL_Voxel_CheckChunkFrustum(chk))
			continue;
		LBXGL_Voxel_CheckRebuildChunk(chk);
		
		m++;
		if(!chk->n_face)
			continue;
		
		chk->vnext=rgn->vischk;
		rgn->vischk=chk;
		n++;
	}
	
//	printf("Visible Chunks: %d/%d\n", n, m);
}

void LBXGL_Voxel_RebuildRegionListVisible(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_RebuildRegionVisible(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_ClearWorldPointLight(LBXGL_BrushWorld *world)
{
}

void LBXGL_Voxel_UpdateWorldPointLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
}

