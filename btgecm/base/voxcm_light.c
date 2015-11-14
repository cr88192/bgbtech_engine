#include <btgecm.h>

static float *btge_voxel_vlc=NULL;
static float *btge_voxel_slc=NULL;
static byte *btge_voxel_vlt=NULL;

//color names
static char *btge_voxel_clrs[16]={
	"white",		//0
	"red",			//1
	"green",		//2
	"yellow",		//3
	"blue",			//4
	"violet",		//5
	"cyan",			//6
	"orange",		//7
	"greenish",		//8
	"pale_red",		//9
	"pale_green",	//10
	"pale_yellow",	//11
	"pale_blue",	//12
	"pale_violet",	//13
	"pale_cyan",	//14
	"sky"			//15
	};

#if 1
//color table (basic)
static float btge_voxel_clrt[16*3]=
{
1.0, 1.0, 1.0,	// 0, white
1.0, 0.0, 0.0,	// 1, red
0.0, 1.0, 0.0,	// 2, green
1.0, 1.0, 0.0,	// 3, yellow
0.0, 0.0, 1.0,	// 4, blue
1.0, 0.0, 1.0,	// 5, violet
0.0, 1.0, 1.0,	// 6, cyan
1.0, 0.6, 0.3,	// 7, orange
0.6, 1.0, 0.3,	// 8, greenish
1.0, 0.5, 0.5,	// 9, pale-red
0.5, 1.0, 0.5,	//10, pale-green
1.0, 1.0, 0.5,	//11, pale-yellow
0.5, 0.5, 1.0,	//12, pale-blue
1.0, 0.5, 1.0,	//13, pale-violet
0.5, 1.0, 1.0,	//14, pale-cyan
0.3, 0.6, 1.0	//15, sky
};
#endif

#if 0
//color table (basic)
static float btge_voxel_clrt[16*3]=
{
1.0, 1.0, 1.0,	// 0, white
1.0, 0.5, 0.5,	// 1, red
0.5, 1.0, 0.5,	// 2, green
1.0, 1.0, 0.5,	// 3, yellow
0.5, 0.5, 1.0,	// 4, blue
1.0, 0.5, 1.0,	// 5, violet
0.5, 1.0, 1.0,	// 6, cyan
1.0, 0.6, 0.3,	// 7, orange
0.6, 1.0, 0.3,	// 8, greenish
1.0, 0.8, 0.8,	// 9, pale-red
0.8, 1.0, 0.8,	//10, pale-green
1.0, 1.0, 0.8,	//11, pale-yellow
0.8, 0.8, 1.0,	//12, pale-blue
1.0, 0.8, 1.0,	//13, pale-violet
0.8, 1.0, 1.0,	//14, pale-cyan
0.3, 0.6, 1.0	//15, sky
};
#endif

//normalized color table
static float btge_voxel_clrtn[16*3];

//colors:
//black(0), red(1), green(2), yellow(3), blue(4), violet(5), cyan(6), white(7)

int BTGE_Voxel_IndexVLightColor(char *name)
{
	int i;
	
	BTGE_Voxel_InitVLightColors();
	
	for(i=0; i<16; i++)
		if(!strcmp(btge_voxel_clrs[i], name))
			return(i);

	return(atoi(name));
}

int BTGE_Voxel_IndexVLightStyle(char *name)
{
	static char *lightstyle[]=
	{
		"normal",	
		"flicker", 
		"slow_strong_pulse",
		"candle",
		"fast_strobe",
		"gentle_pulse",
		"alternate_flicker",
		"candle2",
		"candle3",
		"slow_strobe",
		"flourescent_flicker",
		"slow_pulse_absent_black",
		NULL
	};

	int i;
	
	BTGE_Voxel_InitVLightColors();
	
	for(i=0; lightstyle[i]; i++)
	{
		if(!strcmp(lightstyle[i], name))
			return(i);
	}

	return(atoi(name));
}

void BTGE_Voxel_InitVLightColors()
{
	static int init=0;
	float tv0[3], tv1[3], tv2[3];
	float tv[3];
	float f, g, h;
	int i, j, k, l, l0;

	if(init)return;
	init=1;

	btge_voxel_vlc=malloc(256*3*sizeof(float));
	btge_voxel_slc=malloc(256*3*sizeof(float));

	for(i=0; i<256; i++)
	{
//		tv[0]=(i&1)?1.0:((i&8)?0.5:0.1);
//		tv[1]=(i&2)?1.0:((i&8)?0.5:0.1);
//		tv[2]=(i&4)?1.0:((i&8)?0.5:0.1);

		tv0[0]=btge_voxel_clrt[(i&15)*3+0];
		tv0[1]=btge_voxel_clrt[(i&15)*3+1];
		tv0[2]=btge_voxel_clrt[(i&15)*3+2];

		V3F_NORMALIZE(tv0, tv);
		V3F_COPY(tv, btge_voxel_clrtn+((i&15)*3));
		
		j=(i>>4)&15;
		f=pow(0.9, 15-j);
		V3F_SCALE(tv, f, btge_voxel_vlc+(i*3));
		
		dyPrintf("%02X: %.4f %.4f %.4f\n", i,
			btge_voxel_vlc[i*3+0],
			btge_voxel_vlc[i*3+1],
			btge_voxel_vlc[i*3+2]);

		j=(i>>4)&15;
		f=pow(0.80, 15-j);
		V3F_SCALE(tv0, f, btge_voxel_slc+(i*3));
	}

	btge_voxel_vlt=malloc(65536);
	for(i=0; i<256; i++)
	{
		for(j=0; j<256; j++)
		{
			BTGE_Voxel_GetVLightColor(NULL, i, tv0);
			BTGE_Voxel_GetVLightColor(NULL, j, tv1);
			f=V3F_LEN(tv0); g=V3F_LEN(tv1);
			h=f+g; if(h<=0)h=1;
			f=f/h; g=g/h;
			V3F_SCALEADDSCALE(tv0, f, tv1, g, tv2);

			l=0; h=999999;
			V3F_NORMALIZE(tv2, tv);
			for(k=0; k<16; k++)
			{
				f=V3F_DIST(tv, btge_voxel_clrtn+(k*3));
				if(f<h) { l=k; h=f; }
			}
			l0=l;
		
			l=0; h=999999;
			for(k=0; k<256; k++)
			{
				f=V3F_DIST(tv2, btge_voxel_vlc+(k*3));
				if(f<h) { l=k; h=f; }
			}
			
			l=(l&0xF0)|l;
			
			btge_voxel_vlt[i*256+j]=l;
//			dyPrintf("%02X ", l);
		}
//		dyPrintf("\n");
	}
}

void BTGE_Voxel_GetVLightBaseColor(
	BTGE_BrushWorld *world, int vl, float *rgb)
{
	V3F_COPY(btge_voxel_clrt+((vl&15)*3), rgb);
}

void BTGE_Voxel_GetVLightColor(
	BTGE_BrushWorld *world, int vl, float *rgb)
{
	V3F_COPY(btge_voxel_vlc+((vl&255)*3), rgb);
}

void BTGE_Voxel_GetSLightColor(
	BTGE_BrushWorld *world, int vl, float *rgb)
{
	V3F_COPY(btge_voxel_slc+((vl&255)*3), rgb);
}

int BTGE_Voxel_AddBlendVLight(
	BTGE_BrushWorld *world, int vl0, int vl1)
{
	return(btge_voxel_vlt[((vl1<<8)|vl0)&0xFFFF]);
}

int BTGE_Voxel_GetVoxelTransmitLight(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox)
{
	int l;
	
	if(!vox)return(15);
	if(!vox->type) { return((vox->slight>>4)&15); }

	if(BTGE_Voxel_VoxelOpaqueP(vox))
			return(0);
	l=vox?((vox->slight>>4)&15):15;
	return(l);
}

int BTGE_Voxel_GetVoxelTransmitVLight(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox)
{
	BTGE_VoxelInfo *inf;
	int l, ty;
	
//	return((15<<4)|2);
	
	if(!vox)return(0);
	if(!vox->type) { return(vox->vlight); }
	
	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(BTGE_Voxel_VoxelOpaqueP(vox))
	{
		if(!inf)return(0);
		return(inf->vlight);
	}
	l=vox->vlight;
	if(inf && inf->vlight)
	{
		l=BTGE_Voxel_AddBlendVLight(NULL, l, inf->vlight);
	}

	ty=vox->type&VOX_TY_MASK;
	if(ty==VOX_TY_GLASS)
	{
		if(vox->aux&15)
			{ l=(l&0xF0)|(vox->aux&15); }
	}

	return(l);
}

int BTGE_Voxel_GetVoxelTransmitSLight(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox)
{
//	BTGE_VoxelInfo *inf;
	int l, ty;
	
	if(!vox)return(0xF0);
	if(!vox->type)
	{
		l=vox->slight;

#if 0
		if(world->vox_terr &&
			(world->vox_terr->flags&VOX_TGFL_SUN_AMBIENT))
		{
//			if(l<224)l=l+32;
		}
#endif

		return(l);
//		return(vox->slight);
	}

//	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(BTGE_Voxel_VoxelOpaqueP(vox))
		return(0);

	l=vox->slight;
	ty=vox->type&VOX_TY_MASK;
	if(ty==VOX_TY_GLASS)
	{
		if(vox->aux&15)
			{ l=(l&0xF0)|(vox->aux&15); }
	}

	if(BTGE_Voxel_VoxelFluidP(vox))
	{
		l-=16;
		l=(l>0)?l:0;
	}

	return(l);
}

void BTGE_Voxel_UpdateChunkLightVert(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox, *vox1;
	int x, y, z, sa;
	int i, j, k, l;

	if(!chk)return;

	if(!(world->vox_terr->flags&VOX_TGFL_SUN_AMBIENT))
	{
		chk->flags|=VOX_CHFL_SKIP_SUN; sa=0;
	}else
	{
		chk->flags&=~VOX_CHFL_SKIP_SUN; sa=1;
	}

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
	{
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+chk->zs;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		l=BTGE_Voxel_GetVoxelTransmitLight(world, vox);
		if(l>1) { chk->flags&=~VOX_CHFL_SKIP_SUN; }

		if((l==0) && (!sa))
		{
			for(k=0; k<chk->zs; k++)
			{
				x=chk->ox+i; y=chk->oy+j; z=chk->oz+k;
//				vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
				if(!vox)continue;

//				if(vox->slight&0xF0)
//				if(vox->slight)
				if(vox->slight>=224)
				{
//					vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
					vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
						world, chk->rgn, x, y, z);
//					vox->slight=(vox->slight&0x0F)|0;
					vox->slight=0;
				}
			}
			continue;
		}

		for(k=0; k<chk->zs; k++)
		{
			x=chk->ox+i;
			y=chk->oy+j;
			z=chk->oz+(chk->zs-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

//			l=vox1?((vox1->slight>>4)&15):15;
//			if(vox1 && BTGE_Voxel_VoxelOpaqueP(vox1))
//				l=0;
#if 0
			l=BTGE_Voxel_GetVoxelTransmitLight(world, vox1);

			if((vox->slight&0xF0)!=(l<<4))
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->slight=(vox->slight&0x0F)|(l<<4);
			}
#endif

#if 1
			l=BTGE_Voxel_GetVoxelTransmitSLight(world, vox1);
			if(sa && (l<240))l+=16;
			//if(vox->slight!=l)
			if(vox->slight<l)
			{
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->slight=l;
			}
#endif
		}
	}
}

void BTGE_Voxel_UpdateChunkLightHoriz(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox, *vox1;
	int x, y, z;
	int i, j, k, l, l1;

	if(!chk)return;

	if(chk->flags&VOX_CHFL_SKIP_SUN)
		return;

//	chk->flags|=VOX_CHFL_SKIP_SUN;

	for(i=0; i<chk->ys; i++)
		for(j=0; j<chk->zs; j++)
	{
		y=chk->oy+i;
		z=chk->oz+j;
		x=chk->ox-1;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->xs+1; k++)
		for(k=0; k<chk->xs; k++)
		{
			x=chk->ox+k;

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

//			l=BTGE_Voxel_GetVoxelTransmitLight(world, vox1);
//			l=(l>0)?(l-1):0;

			l=BTGE_Voxel_GetVoxelTransmitSLight(world, vox1);
			l=(l>16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitSLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if((vox->slight&0xF0)<(l<<4))
			if(vox->slight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
//				vox->slight=(vox->slight&0x0F)|(l<<4);
				vox->slight=l;
			}
		}

		x=chk->ox+chk->xs;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->xs+1; k++)
		for(k=0; k<chk->xs; k++)
		{
			x=chk->ox+(chk->xs-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

//			l=BTGE_Voxel_GetVoxelTransmitLight(world, vox1);
//			l=(l>0)?(l-1):0;

			l=BTGE_Voxel_GetVoxelTransmitSLight(world, vox1);
			l=(l>16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitSLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if((vox->slight&0xF0)<(l<<4))
			if(vox->slight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
//				vox->slight=(vox->slight&0x0F)|(l<<4);
				vox->slight=l;
			}
		}
	}

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->zs; j++)
	{
		x=chk->ox+i;
		y=chk->oy-1;
		z=chk->oz+j;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->ys+1; k++)
		for(k=0; k<chk->ys; k++)
		{
			y=chk->oy+k;

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

//			l=BTGE_Voxel_GetVoxelTransmitLight(world, vox1);
//			l=(l>0)?(l-1):0;

			l=BTGE_Voxel_GetVoxelTransmitSLight(world, vox1);
			l=(l>16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitSLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if((vox->slight&0xF0)<(l<<4))
			if(vox->slight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
//				vox->slight=(vox->slight&0x0F)|(l<<4);
				vox->slight=l;
			}
		}

		y=chk->oy+chk->ys;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->ys+1; k++)
		for(k=0; k<chk->ys; k++)
		{
			y=chk->oy+(chk->ys-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

//			l=BTGE_Voxel_GetVoxelTransmitLight(world, vox1);
//			l=(l>0)?(l-1):0;

			l=BTGE_Voxel_GetVoxelTransmitSLight(world, vox1);
			l=(l>16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitSLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if((vox->slight&0xF0)<(l<<4))
			if(vox->slight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
//				vox->slight=(vox->slight&0x0F)|(l<<4);
				vox->slight=l;
			}
		}
	}
}

bool BTGE_Voxel_UpdateChunkLight(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	if(!chk)return(false);
	if(!(chk->flags&VOX_CHFL_LIGHT_DIRTY))
		return(false);

	BTGE_Voxel_UpdateChunkLightVert(world, chk);
	BTGE_Voxel_UpdateChunkLightHoriz(world, chk);
	BTGE_Voxel_UpdateChunkLightHoriz(world, chk);

	BTGE_Voxel_InitVLightColors();

	chk->flags|=VOX_CHFL_SKIP_VLIGHT;
#if 0
	BTGE_Voxel_UpdateChunkVLightClear(world, chk);
	BTGE_Voxel_UpdateChunkVLightVert(world, chk);
	BTGE_Voxel_UpdateChunkVLightHoriz(world, chk);
	BTGE_Voxel_UpdateChunkVLightHoriz(world, chk);
	BTGE_Voxel_UpdateChunkVLightVert(world, chk);
//	BTGE_Voxel_UpdateChunkVLightHoriz(world, chk);
//	BTGE_Voxel_UpdateChunkVLightHoriz(world, chk);
#endif

	chk->flags&=~VOX_CHFL_LIGHT_DIRTY;
	return(true);
}

void BTGE_Voxel_UpdateRegionLight(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *chk;
	int i, j, k, l, n, m;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
	{
		for(k=0; k<rgn->zs; k++)
		{
			l=rgn->zs-k-1;
			chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+l];
			if(!chk)continue;
			BTGE_Voxel_UpdateChunkLightVert(world, chk);
		}
	}
}


void BTGE_Voxel_UpdateChunkVLightClear(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox, *vox1;
	int x, y, z;
	int i, j, k, l, l1;

	if(!chk)return;

//	chk->flags|=VOX_CHFL_SKIP_VLIGHT;

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
			for(k=0; k<chk->zs; k++)
	{
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+k;
		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);

		if(vox->vlight)
		{
//			vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
				world, chk->rgn, x, y, z);
			vox->vlight=0;
		}
	}
}

void BTGE_Voxel_UpdateChunkVLightVert(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox, *vox1;
	int x, y, z;
	int i, j, k, l, l1;

	if(!chk)return;

//	chk->flags|=VOX_CHFL_SKIP_VLIGHT;

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
	{
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+chk->zs;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);

//		for(k=0; k<chk->zs+1; k++)
		for(k=0; k<chk->zs; k++)
		{
			x=chk->ox+i;
			y=chk->oy+j;
			z=chk->oz+(chk->zs-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);
			if(l>0) { chk->flags&=~VOX_CHFL_SKIP_VLIGHT; }

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}
	}

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
	{
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz-1;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);

//		for(k=0; k<chk->zs+1; k++)
		for(k=0; k<chk->zs; k++)
		{
			x=chk->ox+i;
			y=chk->oy+j;
			z=chk->oz+k;

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);
			if(l>0) { chk->flags&=~VOX_CHFL_SKIP_VLIGHT; }

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}
	}
}

void BTGE_Voxel_UpdateChunkVLightHoriz(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox, *vox1;
	int x, y, z;
	int i, j, k, l, l1;

	if(!chk)return;

//	if(chk->flags&VOX_CHFL_SKIP_VLIGHT)
//		return;

	for(i=0; i<chk->ys; i++)
		for(j=0; j<chk->zs; j++)
	{
		y=chk->oy+i;
		z=chk->oz+j;
		x=chk->ox-1;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->xs+1; k++)
		for(k=0; k<chk->xs; k++)
		{
			x=chk->ox+k;

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}

		x=chk->ox+chk->xs;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->xs+1; k++)
		for(k=0; k<chk->xs; k++)
		{
			x=chk->ox+(chk->xs-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}
	}

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->zs; j++)
	{
		x=chk->ox+i;
		y=chk->oy-1;
		z=chk->oz+j;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->ys+1; k++)
		for(k=0; k<chk->ys; k++)
		{
			y=chk->oy+k;

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}

		y=chk->oy+chk->ys;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//		for(k=0; k<chk->ys+1; k++)
		for(k=0; k<chk->ys; k++)
		{
			y=chk->oy+(chk->ys-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
			if(!vox)continue;

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}
	}
}


#if 0
void BTGE_Voxel_UpdateChunkVLightHoriz2(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox, *vox1;
	int x, y, z, tx, ty, tz;
	int i, j, k, l, l1;

	if(!chk)return;

//	if(chk->flags&VOX_CHFL_SKIP_VLIGHT)
//		return;

	for(i=0; i<chk->ys; i++)
		for(j=0; j<chk->zs; j++)
	{
		y=chk->oy+i;
		z=chk->oz+j;
		x=chk->ox-1;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		for(k=0; k<chk->xs; k++)
		{
			x=chk->ox+k;

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//			if(!vox)continue;

			tx=x-chx->ox; ty=x-chx->oy; tz=x-chx->oz;
			vox=&(chk->vox[(tz*chk->ys+ty)*chk->xs+tx]);

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
//				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
//					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}

		x=chk->ox+chk->xs;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		for(k=0; k<chk->xs; k++)
		{
			x=chk->ox+(chk->xs-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//			if(!vox)continue;

			tx=x-chx->ox; ty=x-chx->oy; tz=x-chx->oz;
			vox=&(chk->vox[(tz*chk->ys+ty)*chk->xs+tx]);

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
//				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
//					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}
	}

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->zs; j++)
	{
		x=chk->ox+i;
		y=chk->oy-1;
		z=chk->oz+j;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		for(k=0; k<chk->ys; k++)
		{
			y=chk->oy+k;

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//			if(!vox)continue;

			tx=x-chx->ox; ty=x-chx->oy; tz=x-chx->oz;
			vox=&(chk->vox[(tz*chk->ys+ty)*chk->xs+tx]);

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}

		y=chk->oy+chk->ys;
//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		for(k=0; k<chk->ys; k++)
		{
			y=chk->oy+(chk->ys-k-1);

			vox1=vox;
//			vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//			vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
//			if(!vox)continue;

			tx=x-chx->ox; ty=x-chx->oy; tz=x-chx->oz;
			vox=&(chk->vox[(tz*chk->ys+ty)*chk->xs+tx]);

			l=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
			l=(l>=16)?(l-16):0;
			l1=BTGE_Voxel_GetVoxelTransmitVLight(world, vox);
			l=BTGE_Voxel_AddBlendVLight(world, l, l1);

//			if(vox->vlight!=l)
			if(vox->vlight<l)
			{
//				vox=BTGE_Voxel_GetRegionVoxelDirty2(chk->rgn, x, y, z);
				vox=BTGE_Voxel_GetWorldRegionVoxelDirty2(
					world, chk->rgn, x, y, z);
				vox->vlight=l;
			}
		}
	}
}
#endif

