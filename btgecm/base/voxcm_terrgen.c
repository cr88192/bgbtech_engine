#include <btgecm.h>
#include <bgbbtj.h>

// #define VOX_TERR_BORDER 312
// #define VOX_TERR_BORDER 384
#define VOX_TERR_BORDER 448

#if 1
BTGE_API int BTGE_Voxel_GetWorldRegionXSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(32*16); }
BTGE_API int BTGE_Voxel_GetWorldRegionYSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(32*16); }
BTGE_API int BTGE_Voxel_GetWorldRegionZSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(8*16); }
#endif

#if 0
BTGE_API int BTGE_Voxel_GetWorldRegionXSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(16*16); }
BTGE_API int BTGE_Voxel_GetWorldRegionYSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(16*16); }
BTGE_API int BTGE_Voxel_GetWorldRegionZSizeVoxels(
		BTGE_BrushWorld *world)
	{ return(16*16); }
#endif

#if 0
// BTGE_API void BTGE_Voxel_FreeRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	int i, n;
	
	n=rgn->xs*rgn->ys*rgn->zs;
	for(i=0; i<n; i++)
	{
		if(rgn->chk[i])
		{
			BTGE_Voxel_FreeChunk(rgn->chk[i]);
			rgn->chk[i]=NULL;
		}
	}
	
	gcfree(rgn->chk);

	if(rgn->chk_offs)
	{
		gcfree(rgn->chk_offs);
		gcfree(rgn->chk_sz);
	}
}
#endif

BTGE_API BTGE_VoxelRegion *BTGE_Voxel_AllocRegion(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *rgn;
	int n;

	BTGE_Voxel_Setup();

	rgn=gctalloc("btge_voxelregion_t", sizeof(BTGE_VoxelRegion));
	rgn->next=world->vox_region;
	world->vox_region=rgn;
	
	rgn->ox=-256;
	rgn->oy=-256;
	rgn->oz=0;

#if 1
	rgn->xs=32;
	rgn->ys=32;
	rgn->zs=8;
#endif

#if 0
	rgn->xs=16;
	rgn->ys=16;
	rgn->zs=16;
#endif

	rgn->xst=16;
	rgn->yst=16;
	rgn->zst=16;

	rgn->xss=4;
	rgn->yss=4;
	rgn->zss=4;

	rgn->voxsz=32;

	rgn->xsv=rgn->xs*rgn->xst;
	rgn->ysv=rgn->ys*rgn->yst;
	rgn->zsv=rgn->zs*rgn->zst;
	
	rgn->chk=gcalloc(rgn->xs*rgn->ys*rgn->zs*sizeof(BTGE_VoxelChunk *));
	
	n=rgn->xs*rgn->ys*rgn->zs;
	if(!rgn->chk_offs)
	{
		rgn->chk_offs=gcatomic(n*sizeof(int));
		rgn->chk_sz=gcatomic(n*sizeof(int));
	}

	return(rgn);
}

BTGE_API BTGE_VoxelRegion *BTGE_Voxel_AllocRegionForPoint(
	BTGE_BrushWorld *world, int x, int y, int z)
{
	BTGE_VoxelRegion *rgn;
	int bx, by, bz;
	
#if 1
	bx=x/512;
	by=y/512;
	bz=z/128;
	if(x<(bx*512))bx--;
	if(y<(by*512))by--;
	if(z<(bz*128))bz--;
#endif

#if 0
	bx=x/256;
	by=y/256;
	bz=z/256;
	if(x<(bx*256))bx--;
	if(y<(by*256))by--;
	if(z<(bz*256))bz--;
#endif

	rgn=BTGE_Voxel_AllocRegion(world);

#if 1
	rgn->ox=bx*512;
	rgn->oy=by*512;
	rgn->oz=bz*128;
#endif

#if 0
	rgn->ox=bx*256;
	rgn->oy=by*256;
	rgn->oz=bz*256;
#endif

	printf("BTGE_Voxel_AllocRegionForPoint: "
		"Pt=(%d,%d,%d) -> Rgn=(%d,%d,%d) Org=(%d,%d,%d)\n",
		x,y,z, bx,by,bz, rgn->ox,rgn->oy,rgn->oz);
	
	return(rgn);
}

BTGE_API void BTGE_Voxel_GenerateTree(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y, int z)
{
	BTGE_VoxelData *vox;
	float f, g, h;
	int x1, y1, z1, aty, ta;

	aty=rand()%6;
	ta=(rand()&3)+4;

	for(x1=x-4; x1<(x+5); x1++)
		for(y1=y-4; y1<(y+5); y1++)
			for(z1=(z+ta-2); z1<(z+ta+6); z1++)
	{
		f=x1-x; g=y1-y; h=z1-(z+ta);
		f=sqrt(f*f+g*g+h*h);

		if(f>3)continue;

		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z1);
		if(!vox)continue;
		vox->type=VOX_TY_LEAVES|VOX_TYM_FLEXFORM;
		vox->aux=aty;
//		vox->aux=0xF0;
	}

	for(z1=z; z1<(z+ta); z1++)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z1);
		if(!vox)continue;
		vox->type=VOX_TY_LOG|VOX_TYM_FLEXFORM;
		vox->aux=aty;
//		vox->aux=0xF0;
	}
}

BTGE_API void BTGE_Voxel_GenerateGiantShroom(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y, int z)
{
	BTGE_VoxelData *vox;
	float f, g, h;
	int x1, y1, z1, ty, ta;
	
	ty=VOX_TY_SHROOM_BROWN_HUGE;
	f=BTGE_Noise_Random(terr->noise3);
	if(f<0.33)ty=VOX_TY_SHROOM_RED_HUGE;

	ta=(rand()&3)+4;

	for(x1=x-5; x1<(x+6); x1++)
		for(y1=y-5; y1<(y+6); y1++)
			for(z1=(z+ta-2); z1<(z+ta+6); z1++)
	{
		f=x1-x; g=y1-y; h=z1-(z+ta);
		f=sqrt(f*f+g*g+h*h);

		if(f>4)continue;
		if(f<3)continue;
//		if(h<(-1))continue;
		if(h<0)continue;

		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z1);
		if(!vox)continue;
		vox->type=ty|VOX_TYM_FLEXFORM;
//		vox->aux=0xF0;
	}

	for(z1=z; z1<(z+ta+4); z1++)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z1);
		if(!vox)continue;
		vox->type=ty|VOX_TYM_FLEXFORM;
//		vox->aux=0xF0;
	}
}

BTGE_API void BTGE_Voxel_GenerateMetalTree(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y, int z)
{
	BTGE_VoxelData *vox;
	float f, g, h;
	int x1, y1, z1, ta, ty;

	ta=(rand()&3)+4;
	ty=rand()&3;

	for(x1=x-4; x1<(x+5); x1++)
		for(y1=y-4; y1<(y+5); y1++)
			for(z1=(z+4); z1<(z+ta+6); z1++)
	{
		f=x1-x; g=y1-y; h=z1-(z+6);
		f=sqrt(f*f+g*g+h*h);

		if(f>3)continue;

		f=BTGE_Noise_Random(terr->noise3);
		if(f<0.5)continue;

		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z1);
		if(!vox)continue;
//		vox->type=VOX_TY_LEAVES|VOX_TYM_FLEXFORM;
		vox->type=VOX_TY_BLOCK_IRON;
//		vox->aux=0xF0;

		switch(ty)
		{
		case 0: case 1:
			vox->type=VOX_TY_BLOCK_IRON;
			break;
		case 2:
			vox->type=VOX_TY_BLOCK_GOLD;
			break;
		case 3:
			vox->type=VOX_TY_LAPIS_BLOCK;
			break;
		}
	}

	for(z1=z; z1<(z+ta); z1++)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z1);
		if(!vox)continue;
//		vox->type=VOX_TY_LOG|VOX_TYM_FLEXFORM;
		vox->type=VOX_TY_BLOCK_RUST;
//		vox->aux=0xF0;
	}
}

BTGE_API void BTGE_Voxel_GenerateMegaTree(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y, int z)
{
	BTGE_VoxelData *vox;
	float f, g, h;
	int x1, y1, z1, tz;

	tz=16+(rand()&15);

	for(x1=x-12; x1<(x+13); x1++)
		for(y1=y-12; y1<(y+13); y1++)
			for(z1=(z+tz-6); z1<(z+tz+6); z1++)
	{
		f=x1-x; g=y1-y; h=z1-(z+tz);
		f=sqrt(f*f+g*g+h*h);

		if(f>9)continue;

		if(!(rand()&31))
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z1);
			if(!vox)continue;
			vox->type=VOX_TY_LOG|VOX_TYM_FLEXFORM;
			vox->aux=0x04;
			continue;
		}

		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z1);
		if(!vox)continue;
		vox->type=VOX_TY_LEAVES|VOX_TYM_FLEXFORM;
		vox->aux=0x04;
//		vox->aux=0xF0;
	}

	for(z1=z; z1<(z+tz); z1++)
	{
		for(x1=x; x1<(x+2); x1++)
			for(y1=y; y1<(y+2); y1++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z1);
			if(!vox)continue;
			vox->type=VOX_TY_LOG|VOX_TYM_FLEXFORM;
			vox->aux=0x04;
//			vox->aux=0xF0;
		}
	}
}

BTGE_API void BTGE_Voxel_GenerateSmallHouse(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y, int z)
{
	BTGE_VoxelData *vox;
	float f, g, h;
	int x1, y1, z1, z2, xs, ys;

	xs=4+2*BTGE_Noise_SRandom(terr->noise2);
	ys=4+2*BTGE_Noise_SRandom(terr->noise2);

	for(x1=x-xs; x1<(x+xs+1); x1++)
	{
		for(y1=y-ys; y1<(y+ys+1); y1++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z);
			if(!vox)continue;
			vox->type=VOX_TY_BRICK;
			vox->aux=0xF0;
			
			z2=(z>=24)?(z-8):16;
			for(z1=z-1; z1>z2; z1--)
			{
				vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z1);
				if(!vox)break;
				if(vox->type)break;

				vox->type=VOX_TY_GRAVEL;
				vox->aux=0xF0;
			}
		}
	}

	for(x1=x-xs; x1<(x+xs+1); x1++)
	{
		for(y1=y-ys; y1<(y+ys+1); y1++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y1, z+4);
			if(!vox)continue;
			vox->type=VOX_TY_BRICK;
			vox->aux=0xF0;
		}
	}

	for(z1=z+1; z1<z+4; z1++)
	{
		for(x1=x-xs; x1<(x+xs+1); x1++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y-ys, z1);
			if(!vox)continue;
			vox->type=VOX_TY_BRICK;
			vox->aux=0xF0;
		}

		for(x1=x-xs; x1<(x+xs+1); x1++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x1, y+ys, z1);
			if(!vox)continue;
			vox->type=VOX_TY_BRICK;
			vox->aux=0xF0;
		}

		for(y1=y-ys; y1<(y+ys+1); y1++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x-xs, y1, z1);
			if(!vox)continue;
			vox->type=VOX_TY_BRICK;
			vox->aux=0xF0;
		}

		for(y1=y-ys; y1<(y+ys+1); y1++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x+xs, y1, z1);
			if(!vox)continue;
			vox->type=VOX_TY_BRICK;
			vox->aux=0xF0;
		}
	}
}


BTGE_API void BTGE_Voxel_GenerateDirtPanel(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, BTGE_VoxelData *vox, int x, int y, int z)
{
	float f, g;

	if(!(terr->flags&VOX_TGFL_PANELS))
		return;

#if 1
	g=BTGE_Noise_Random(terr->noise2);
	if(g<0.01)
	{
//		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
//		vox->type=VOX_TY_SAPLING;
//		vox->aux=0xF0;

		switch(rand()%5)
		{
		case 0: vox->type=VOX_TY_COMP_INNARDS; break;
		case 1: vox->type=VOX_TY_COMP_PANEL; break;
		case 2: vox->type=VOX_TY_PANEL_GRATE; break;
		case 3: vox->type=VOX_TY_PANEL_HEX; break;
		case 4: vox->type=VOX_TY_PANEL_PIPES; break;
		}
	}
#endif
}

BTGE_API void BTGE_Voxel_GenerateSpawner(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, BTGE_VoxelData *vox, int x, int y, int z)
{
	float f, g;
	int j;

	if(!(terr->flags&VOX_TGFL_SPAWNERS))
		return;

//	if((abs(x)<VOX_TERR_BORDER) && (abs(y)<VOX_TERR_BORDER))
	if((abs(x)<VOX_TERR_BORDER) && (abs(y)<VOX_TERR_BORDER) && (z>64))
	{
		g=BTGE_Noise_Random(terr->noise2);
//		if(g<0.001)
		if(g<0.00075)
//		if(g<0.00025)
		{
//			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
			j=((int)(g*10000))&15;

			vox->type=VOX_TY_MOBSPAWNER;
			vox->aux=0xF0|j;
		}
	}

}

BTGE_API void BTGE_Voxel_GenerateTallGrass(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1;
	float f, g;
	int j, k;

//	if(!(terr->flags&VOX_TGFL_SPAWNERS))
//		return;

//	if((abs(x)<VOX_TERR_BORDER) && (abs(y)<VOX_TERR_BORDER))
	if((vox->type&VOX_TY_MASK)==VOX_TY_GRASS)
	{
		g=BTGE_Noise_Random(terr->noise2);
		if(g<0.01)
//		if(g<0.001)
//		if(g<0.00075)
//		if(g<0.00025)
		{
			vox1=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
			j=(int)(g*100000);
			k=j&15;

			switch(k)
			{
			case 0:
				vox1->type=VOX_TY_FLOWER_YEL;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			case 1:
				vox1->type=VOX_TY_FLOWER_RED;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			case 2:
				vox1->type=VOX_TY_PUMPKIN;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			default:
				vox1->type=VOX_TY_TALL_GRASS;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			}
		}
	}

	if((vox->type&VOX_TY_MASK)==VOX_TY_MYCELIUM)
	{
		g=BTGE_Noise_Random(terr->noise2);
		if(g<0.005)
//		if(g<0.001)
//		if(g<0.00075)
//		if(g<0.00025)
		{
			vox1=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
			j=(int)(g*100000);
			k=j&15;

			switch(k)
			{
			case 0:
				vox1->type=VOX_TY_SHROOM_BROWN;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			case 1:
				vox1->type=VOX_TY_SHROOM_RED;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
//			case 2:
//				vox1->type=VOX_TY_PUMPKIN;
//				vox1->aux=0;
//				vox1->aux=0xF0|j;
//				break;
			default:
//				vox1->type=VOX_TY_TALL_GRASS;
//				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			}
		}
	}

	if((vox->type&VOX_TY_MASK)==VOX_TY_SAND)
	{
		g=BTGE_Noise_Random(terr->noise2);
//		if(g<0.005)
		if(g<0.001)
//		if(g<0.00075)
//		if(g<0.00025)
		{
			vox1=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
			j=(int)(g*100000);
			k=j&15;

			switch(k)
			{
			case 0:
				vox1->type=VOX_TY_DEAD_BUSH;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			case 1:
				vox1->type=VOX_TY_CACTUS;
				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
//			case 2:
//				vox1->type=VOX_TY_PUMPKIN;
//				vox1->aux=0;
//				vox1->aux=0xF0|j;
//				break;
			default:
//				vox1->type=VOX_TY_TALL_GRASS;
//				vox1->aux=0;
//				vox1->aux=0xF0|j;
				break;
			}
		}
	}
}

BTGE_API float BTGE_Voxel_CalcSpanBaseHeightI(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	float f, g, h, h2;

//	f=BTGE_Noise_GenValue2(terr->noise, x/32.0, y/32.0);
//	f=BTGE_Noise_GenValue2(terr->noise, x/256.0, y/256.0);
	f=BTGE_Noise_GenValue2(terr->noise, x/128.0, y/128.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, x/32.0, y/32.0);
	g=BTGE_Noise_GenValue2(terr->noise2, x/16.0, y/16.0);
	h=BTGE_Noise_GenValue2(terr->noise3, y/1024.0, x/1024.0);
//	h2=BTGE_Noise_GenValue2(terr->noise4, -y/4096.0, -x/4096.0);
//	h2=BTGE_Noise_GenValue2(terr->noise4, x/4096.0, y/4096.0);
//	f=64+f*32;
//	f=64+f*32+g*8;
//	f=80+f*32+g*8;
//	f=72+f*32+g*8;
//	f=68+f*32+g*8;
//	f=68+f*32+g*8+h*32+h2*32;
	f=68+f*32+g*8+h*32;
	
	return(f);
}

BTGE_API float BTGE_Voxel_CalcSpanBaseHeight(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	int x2, y2;
	float t0, t1;
	float f, g, h;
	float h0, h1, h2, h3;

	if((x<(terr->world_minx+256)) || (y<(terr->world_miny+256)) ||
		(x>(terr->world_maxx-256)) || (y>(terr->world_maxy-256)))
	{
		x2=x; y2=y;

		if(x<(terr->world_minx+256))
		{
			x2+=terr->world_xs;
			t0=(x-terr->world_minx)/256.0;
		}
		if(y<(terr->world_miny+256))
		{
			y2+=terr->world_ys;
			t1=(y-terr->world_miny)/256.0;
		}
		if(x>(terr->world_maxx-256))
		{
			x2-=terr->world_xs;
			t0=(terr->world_maxx-x)/256.0;
		}
		if(y>(terr->world_maxy-256))
		{
			y2-=terr->world_ys;
			t1=(terr->world_maxy-y)/256.0;
		}
		
		if(t0<0)t0=0;
		if(t1<0)t1=0;

		h0=BTGE_Voxel_CalcSpanBaseHeightI(world, terr, x , y );
		h1=BTGE_Voxel_CalcSpanBaseHeightI(world, terr, x2, y );
		h2=BTGE_Voxel_CalcSpanBaseHeightI(world, terr, x , y2);
		h3=BTGE_Voxel_CalcSpanBaseHeightI(world, terr, x2, y2);
		
		f=btge_lerp(h0, h1, 1.0-(0.5*t0));
		g=btge_lerp(h2, h3, 1.0-(0.5*t0));
		h=btge_lerp(f, g, 1.0-(0.5*t1));
		return(h);
	}

	f=BTGE_Voxel_CalcSpanBaseHeightI(world, terr, x, y);
	return(f);
}

BTGE_API void BTGE_Voxel_GenerateSpanBase(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	BTGE_VoxelBiomeInfo *biome;
	BTGE_VoxelChunk *chk;
	BTGE_VoxelData *vox;
	float height;
	float f, g, h, h2;
	int grass, dirt, stone, sand, water;
	int z;
	int i, j;

	biome=BTGE_Voxel_GenerateFindBiome(world, terr, x, y);

#if 0
//	f=BTGE_Noise_GenValue2(terr->noise, x/32.0, y/32.0);
//	f=BTGE_Noise_GenValue2(terr->noise, x/256.0, y/256.0);
	f=BTGE_Noise_GenValue2(terr->noise, x/128.0, y/128.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, x/32.0, y/32.0);
	g=BTGE_Noise_GenValue2(terr->noise2, x/16.0, y/16.0);
	h=BTGE_Noise_GenValue2(terr->noise3, y/1024.0, x/1024.0);
//	h2=BTGE_Noise_GenValue2(terr->noise4, -y/4096.0, -x/4096.0);
//	h2=BTGE_Noise_GenValue2(terr->noise4, x/4096.0, y/4096.0);
//	f=64+f*32;
//	f=64+f*32+g*8;
//	f=80+f*32+g*8;
//	f=72+f*32+g*8;
//	f=68+f*32+g*8;
//	f=68+f*32+g*8+h*32+h2*32;
	f=68+f*32+g*8+h*32;
#endif

	f=BTGE_Voxel_CalcSpanBaseHeight(world, terr, x, y);

#if 1
	h=1.0;
	if(biome->flags&VOX_BIFL_HILLS2X)h*=2;
	if(biome->flags&VOX_BIFL_HILLS4X)h*=4;
//	if(biome->flags&VOX_BIFL_HILLS8X)h*=2;
	if(biome->flags&VOX_BIFL_HILLSNX)h=1.0/h;
	if(f>68) { f=(f-68)*h+68; }
#endif

	height=f;

	if(!((x+8)&15) && !((y+8)&15))
	{
		for(z=8; z<128; z+=16)
		{
			chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 2);
			if(chk)
				{ chk->biome=biome->id; }
		}
	}

	grass=biome->vox_grass|VOX_TYM_FLEXFORM;
	dirt=biome->vox_dirt|VOX_TYM_FLEXFORM;
	stone=biome->vox_stone|VOX_TYM_FLEXFORM;
	sand=biome->vox_sand|VOX_TYM_FLEXFORM;
	water=biome->vox_water|VOX_TYM_FLEXFORM;

	if((grass&VOX_TY_MASK)==VOX_TY_BLOCK_RUST)
		grass=grass&VOX_TY_MASK; 

	if(BTGE_Voxel_VoxelTypeNoFlexformP(world, grass))
		{ grass=grass&VOX_TY_MASK; }
	if(BTGE_Voxel_VoxelTypeNoFlexformP(world, dirt))
		{ dirt=dirt&VOX_TY_MASK; }
	if(BTGE_Voxel_VoxelTypeNoFlexformP(world, stone))
		{ stone=stone&VOX_TY_MASK; }
	if(BTGE_Voxel_VoxelTypeNoFlexformP(world, sand))
		{ sand=sand&VOX_TY_MASK; }
	if(BTGE_Voxel_VoxelTypeNoFlexformP(world, water))
		{ water=water&VOX_TY_MASK; }

	if(grass<0)
		{ grass=VOX_TY_GRASS|VOX_TYM_FLEXFORM; }
	if(dirt<0)
		{ dirt=VOX_TY_DIRT|VOX_TYM_FLEXFORM; }
	if(stone<0)
		{ stone=VOX_TY_STONE|VOX_TYM_FLEXFORM; }
	if(sand<0)
		{ sand=VOX_TY_SAND|VOX_TYM_FLEXFORM; }
	if(water<0)
		{ water=VOX_TY_WATER|VOX_TYM_FLEXFORM; }

	for(z=0; z<128; z++)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);

		if(z>=f)
		{
			if(z>(64+2))
			{
//				vox->type=VOX_TY_GRASS|VOX_TYM_FLEXFORM;
//				vox->type=biome->vox_grass|VOX_TYM_FLEXFORM;
				vox->type=grass;
//				vox->type=VOX_TY_STONE;
//				vox->aux=0xF0;

				BTGE_Voxel_GenerateTallGrass(world, terr, vox, x, y, z);
				BTGE_Voxel_GenerateDirtPanel(world, terr, vox, x, y, z);
				BTGE_Voxel_GenerateSpawner(world, terr, vox, x, y, z);
				break;
			}

//			vox->type=VOX_TY_SAND|VOX_TYM_FLEXFORM;
//			vox->type=biome->vox_sand|VOX_TYM_FLEXFORM;
			vox->type=sand;
//			vox->type=VOX_TY_STONE;
//			vox->aux=0xF0;

			BTGE_Voxel_GenerateSpawner(world, terr, vox, x, y, z);

			for(z=z+1; z<64; z++)
			{
				vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
//				vox->type=VOX_TY_WATER;
//				vox->type=biome->vox_water;
				vox->type=water;
//				vox->aux=0xF0;
			}

			break;
		}
			
		if(z>=(f-8))
		{
//			vox->type=VOX_TY_DIRT|VOX_TYM_FLEXFORM;
//			vox->type=biome->vox_dirt|VOX_TYM_FLEXFORM;
			vox->type=dirt;
//			vox->aux=0xF0;

//			if((z>=(64-4)) && (z<(64+2)))
			if((z>=(64-4)) && (z<(64+2)) && (f<68))
			{
//				vox->type=VOX_TY_SAND|VOX_TYM_FLEXFORM;
//				vox->type=biome->vox_sand|VOX_TYM_FLEXFORM;
				vox->type=sand;
			}

			BTGE_Voxel_GenerateDirtPanel(world, terr, vox, x, y, z);

			continue;
		}

		if(z<4)
		{
			vox->aux=0xF0;
			g=BTGE_Noise_Random(terr->noise2);
			if(!z || (g<0.75))
				{ vox->type=VOX_TY_BEDROCK|VOX_TYM_FLEXFORM; continue; }
		}

//		vox->type=VOX_TY_STONE|VOX_TYM_FLEXFORM;
//		vox->type=terr->vox_stone|VOX_TYM_FLEXFORM;
//		vox->type=biome->vox_stone|VOX_TYM_FLEXFORM;
		vox->type=stone;
		BTGE_Voxel_GenerateVoxelOre(world, terr, biome, vox, x, y, z);
	}

	BTGE_Voxel_GenerateSpanCaves(world, terr, x, y, height);
	BTGE_Voxel_GenerateSpanFillLava(world, terr, x, y, 12);
	BTGE_Voxel_GenerateSpanBorder(world, terr, x, y);
}

BTGE_API void BTGE_Voxel_GenerateVoxelOre(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, BTGE_VoxelBiomeInfo *biome,
	BTGE_VoxelData *vox, int x, int y, int z)
{
#if 0
	static struct { int ty; float wf; } oretab[]=
	{
//	{VOX_TY_STONE,			0.500},
	{VOX_TY_GRAVEL,			0.350},
	{VOX_TY_DIRT,			0.300},
	{VOX_TY_COAL,			0.200},
	{VOX_TY_SAND,			0.150},
	{VOX_TY_IRONORE,		0.100},
	{VOX_TY_CLAY_BLOCK,		0.075},
	{VOX_TY_GOLDORE,		0.050},
	{VOX_TY_LAPIS_ORE,		0.025},
	{VOX_TY_DIAMOND_ORE,	0.010},
	{0, 0}};
#endif

	float f, g, h, wt, ws;
	int i, j, noflex;

	if(!(terr->flags&VOX_TGFL_ORES))
		return;

	f=BTGE_Noise_GenValue3(terr->noise2, -x/16.0, -y/16.0, -z/16.0);
	g=BTGE_Noise_GenValue3(terr->noise2, z/4.0, x/4.0, y/4.0);
	f=f*1.5+g*0.5;

//	vox->type=VOX_TY_STONE|VOX_TYM_FLEXFORM;
//	vox->type=terr->vox_stone|VOX_TYM_FLEXFORM;
	if(biome)
		{ vox->type=biome->vox_stone|VOX_TYM_FLEXFORM; }
	else
		{ vox->type=terr->vox_stone|VOX_TYM_FLEXFORM; }

	noflex=0;
	if(BTGE_Voxel_VoxelTypeNoFlexformP(world, vox->type))
		{ vox->type=vox->type&VOX_TY_MASK; noflex=1; }

	if(f<0.5)
		return;

	g=BTGE_Noise_GenValue3(terr->noise2, x/16.0, y/16.0, z/16.0);
	g*=2;

	h=2.0-(z/64.0);
	if(h<1)h=1;
	g*=h;

	wt=0;
//	for(i=0; oretab[i].ty; i++)
//		{ wt+=oretab[i].wf; }
	for(i=0; i<terr->n_ores; i++)
		{ wt+=terr->ores[i].wf; }
	ws=1.0/wt;
	
//	for(i=0; oretab[i].ty; i++)
	for(i=0; i<terr->n_ores; i++)
	{
//		if(g<(1-(oretab[i].wf*ws)))
		if(g<(1-(terr->ores[i].wf*ws)))
		{
			j=terr->ores[i].ty;
			if((j<0) || (j==0xFFFF))
			{
				printf("BTGE_Voxel_GenerateVoxelOre: Bad Ore %d\n", i);
				continue;
			}
			
//			vox->type=oretab[i].ty|VOX_TYM_FLEXFORM;
			vox->type=j|VOX_TYM_FLEXFORM;
			if(noflex)
				{ vox->type=vox->type&VOX_TY_MASK; }
//			vox->aux=0xF0;
			break;
		}
	}

#if 0
	f=BTGE_Noise_GenValue3(terr->noise2, -x/16.0, -y/16.0, -z/16.0);
	f*=2;

	g=BTGE_Noise_GenValue3(terr->noise2, x/16.0, y/16.0, z/16.0);
	g*=2;

	if(f<(1-0.5))
	{
		if(g<(1-0.5))
		{
			vox->type=VOX_TY_STONE|VOX_TYM_FLEXFORM;
			vox->aux=0xF0;
		}else if(g<(1-0.25))
		{
			vox->type=VOX_TY_GRAVEL|VOX_TYM_FLEXFORM;
			vox->aux=0xF0;
		}else if(g<(1-0.15))
		{
			vox->type=VOX_TY_DIRT|VOX_TYM_FLEXFORM;
			vox->aux=0xF0;
		}else if(g<(1-0.10))
		{
			vox->type=VOX_TY_SAND|VOX_TYM_FLEXFORM;
			vox->aux=0xF0;
		}else
		{
			vox->type=VOX_TY_CLAY_BLOCK|VOX_TYM_FLEXFORM;
			vox->aux=0xF0;
		}

		return;
	}

	//higher value ore
	if(g<(1-0.5))
	{
		vox->type=VOX_TY_STONE|VOX_TYM_FLEXFORM;
		vox->aux=0xF0;
	}else if(g<(1-0.25))
	{
		vox->type=VOX_TY_DIRT|VOX_TYM_FLEXFORM;
		vox->aux=0xF0;
	}else if(g<(1-0.15))
	{
		vox->type=VOX_TY_COAL|VOX_TYM_FLEXFORM;
		vox->aux=0xF0;
	}else if(g<(1-0.10))
	{
		vox->type=VOX_TY_IRONORE|VOX_TYM_FLEXFORM;
		vox->aux=0xF0;
	}else if(g<(1-0.075))
	{
		vox->type=VOX_TY_GOLDORE|VOX_TYM_FLEXFORM;
		vox->aux=0xF0;
	}else if(g<(1-0.05))
	{
		vox->type=VOX_TY_LAPIS_ORE|VOX_TYM_FLEXFORM;
		vox->aux=0xF0;
	}else
	{
		vox->type=VOX_TY_DIAMOND_ORE|VOX_TYM_FLEXFORM;
		vox->aux=0xF0;
	}
#endif
}

BTGE_API void BTGE_Voxel_GenerateSpanCaves(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y, float height)
{
	BTGE_VoxelData *vox;
	float f, g;
	int z, zl;
	int i, j;

	if(!(terr->flags&VOX_TGFL_CAVES))
		return;

	zl=height+16;
	if(zl>120)zl=120;
	for(z=0; z<zl; z++)
	{
		f=BTGE_Noise_GenValue3(terr->noise2, x/64.0, y/64.0, z/16.0);
//		f=0.25+abs(f); f=f*f;
		if(f<0.20)continue;
		
		g=BTGE_Noise_GenValue3(terr->noise2, x/8.0, y/8.0, z/4.0);
//		g=0.25+abs(g); g=g*g;
		f=(height/64.0) * (f+g);
		f=0.2+f*f;

		f=btge_scurve(f);

//		g=1.0-abs(z-32)/64.0;
//		g=0.5+g*g;
//		f=f*g;

//		if(f<0.25)
		if(f>(1-0.25))
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
			if(!vox)continue;

			if((vox->type&VOX_TY_MASK)==VOX_TY_BEDROCK)
				continue;
			if(((vox->type&VOX_TY_MASK)==VOX_TY_WATER) ||
				((vox->type&VOX_TY_MASK)==VOX_TY_ICE))
				continue;

			vox->type=VOX_TY_AIR;
//			vox->aux=0xF0;
#if 0
			if(z<12)
			{
//				vox->type=VOX_TY_LAVA;
				vox->type=terr->vox_lava;
			}
#endif

		}
	}
}

BTGE_API void BTGE_Voxel_GenerateSpanFillLava(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y, int height)
{
	BTGE_VoxelData *vox;
	int z;

	for(z=0; z<height; z++)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
		if(!vox)continue;
		
		if(!vox->type)
		{
			vox->type=terr->vox_lava;
		}
	}
}

BTGE_API void BTGE_Voxel_GenerateSpanBorder(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	BTGE_VoxelData *vox;
//	float height;
	float f, g;
	int z;
	int i, j;

	if(!(terr->flags&VOX_TGFL_BORDERS))
		return;

#ifdef VOX_TERR_BORDER
//	if((abs(x)==VOX_TERR_BORDER) || (abs(y)==VOX_TERR_BORDER))
	if(((abs(x)==VOX_TERR_BORDER) || (abs(y)==VOX_TERR_BORDER)) &&
		((abs(x)<=VOX_TERR_BORDER) && (abs(y)<=VOX_TERR_BORDER)))
	{
//		for(z=0; z<96; z++)
		for(z=0; z<112; z++)
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
			if(!vox)continue;
			
//			if(!vox->type)
			if((vox->type&VOX_TY_MASK)==VOX_TY_AIR)
			{
				vox->type=VOX_TY_GLASS|VOX_TYM_UNBREAKABLE;
//				vox->aux=0xFF;
				continue;
			}

//			if((vox->type&VOX_TY_MASK)==VOX_TY_WATER)
			if(((vox->type&VOX_TY_MASK)==VOX_TY_WATER) ||
				((vox->type&VOX_TY_MASK)==VOX_TY_SLIME) ||
				((vox->type&VOX_TY_MASK)==VOX_TY_GLOWSLIME))
			{
//				vox->type=VOX_TY_ICE|VOX_TYM_UNBREAKABLE;
				vox->type=VOX_TY_GLASS|VOX_TYM_UNBREAKABLE;
//				vox->aux=0xFF;
				continue;
			}

#if 0
			if(vox->type==VOX_TY_STONE)
			{
				vox->type=VOX_TY_OBSIDIAN;
//				vox->aux=0xFF;
				continue;
			}
#endif

//			if(((vox->type&VOX_TY_MASK)==VOX_TY_DIRT) ||
//				((vox->type&VOX_TY_MASK)==VOX_TY_GRASS) ||
//				((vox->type&VOX_TY_MASK)==VOX_TY_STONE) ||
//				((vox->type&VOX_TY_MASK)==VOX_TY_SAND))
			if(1)
			{
//				vox->type=VOX_TY_OBSIDIAN;
//				vox->aux=0xFF;
				vox->type=(vox->type&VOX_TY_MASK)|VOX_TYM_UNBREAKABLE;
				continue;
			}
		}
	}
#endif
}

BTGE_API void BTGE_Voxel_GenerateSpanBldg(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	BTGE_VoxelData *vox;
	BTGE_VoxelChunk *chk;
	BTGE_VoxelBiomeInfo *biome;
	float height;
	float f, g, h;
	float trees;
	int z, bid;
	int i, j;


//	g=BTGE_Noise_Random(terr->noise2);
//	if(g>0.01)
//		return;
	
	for(z=128; z>0; z--)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);		
		if(vox && vox->type)
			break;
	}

	if(vox)
	{
		if((vox->type&VOX_TY_MASK)==VOX_TY_WATER)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_LAVA)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_SLIME)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_LEAVES)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_GLASS)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_BRICK)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_WOOD)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_LOG)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_SHROOM_BROWN_HUGE)return;
		if((vox->type&VOX_TY_MASK)==VOX_TY_SHROOM_RED_HUGE)return;
	}
	
	if(!vox)return;

	chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 0);
	if(chk)
	{
		biome=BTGE_Voxel_GenerateFindBiomeID(world, terr, chk->biome);
		bid=chk->biome;
		
		trees=1;
		if(biome->flags&VOX_BIFL_TREES2X)
			trees*=2;
		if(biome->flags&VOX_BIFL_TREES4X)
			trees*=4;
		if(biome->flags&VOX_BIFL_TREES8X)
			trees*=8;

		if(biome->flags&VOX_BIFL_TREESNX)
		{
			if(trees==1.0)
			{
				trees=0.000000001;
			}else
			{
				trees=1.0/trees;
			}
		}
	}else
	{
		biome=NULL;
		trees=1;
		bid=0;
	}

#if 0
	g=BTGE_Noise_Random(terr->noise2);
	if(g<0.001)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
		vox->type=VOX_TY_SAPLING;
//		vox->aux=0xF0;
//		BTGE_Voxel_GenerateTree(world, terr, x, y, z);
	}
#endif

#if 1
	g=BTGE_Noise_Random(terr->noise2);
	g/=trees;
	if(g<0.001)
	{
//		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
//		vox->type=VOX_TY_SAPLING;
//		vox->aux=0xF0;
		if((vox->type&VOX_TY_MASK)==VOX_TY_GRASS)
		{
			if(bid==VOX_BIOME_JUNGLE)
			{
				h=BTGE_Noise_Random(terr->noise3);
				if(h<0.25)
					{ BTGE_Voxel_GenerateMegaTree(world, terr, x, y, z+1); }
				else
					{ BTGE_Voxel_GenerateTree(world, terr, x, y, z+1); }
			}else
			{
				BTGE_Voxel_GenerateTree(world, terr, x, y, z+1);
			}
		}

		if((vox->type&VOX_TY_MASK)==VOX_TY_MYCELIUM)
		{
			BTGE_Voxel_GenerateGiantShroom(world, terr, x, y, z+1);
		}

		if( ((vox->type&VOX_TY_MASK)==VOX_TY_BLOCK_RUST) ||
//			((vox->type&VOX_TY_MASK)==VOX_TY_BLOCK_RUST2) ||
//			((vox->type&VOX_TY_MASK)==VOX_TY_BLOCK_RUST3) ||
//			((vox->type&VOX_TY_MASK)==VOX_TY_BLOCK_RUST4) ||
			((vox->type&VOX_TY_MASK)==VOX_TY_BLOCK_RUST5))
		{
			BTGE_Voxel_GenerateMetalTree(world, terr, x, y, z+1);
		}
	}
#endif

#if 1
	g=BTGE_Noise_Random(terr->noise2);
	if(g<0.00025)
	{
//		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z+1);
//		vox->type=VOX_TY_SAPLING;
//		vox->aux=0xF0;
		BTGE_Voxel_GenerateSmallHouse(world, terr, x, y, z+1);
	}
#endif
}

BTGE_API void BTGE_Voxel_GenerateLightSpan(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	BTGE_VoxelData *vox;
	float height;
	float f, g;
	int z, l;
	int i, j;


//	g=BTGE_Noise_Random(terr->noise2);
//	if(g>0.01)
//		return;
	
	l=15;
	for(z=128; z>0; z--)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
		if(!vox)continue;
		
//		if((vox->aux&0xF0)!=(l<<4))
//		{
//			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
//			vox->aux=(vox->aux&0x0F)|(l<<4);
//		}

		if(vox->slight!=(l<<4))
		{
			vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
			vox->slight=(l<<4);
		}

		if(terr->flags&VOX_TGFL_SUN_AMBIENT)
		{
			if(l<15)l++;
		}

		if(l<=0) { continue; }
		
		if(vox && (vox->type&VOX_TY_MASK))
		{
			if((vox->type&VOX_TY_MASK)==VOX_TY_GLASS)
				continue;
		
			if(BTGE_Voxel_VoxelOpaqueP(vox))
				{ l=0; continue; }
//				break;
			l--;
			if(l<0)l=0;
//			if(l<=0)break;
		}
	}
}

BTGE_API void BTGE_Voxel_GenerateSpanBaseAcid(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	BTGE_VoxelData *vox;
	float height;
	float f, g;
	int z;
	int i, j;

//	f=BTGE_Noise_GenValue2(terr->noise, x/32.0, y/32.0);
//	f=BTGE_Noise_GenValue2(terr->noise, x/256.0, y/256.0);
	f=BTGE_Noise_GenValue2(terr->noise, x/128.0, y/128.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, x/32.0, y/32.0);
	g=BTGE_Noise_GenValue2(terr->noise2, x/16.0, y/16.0);
//	f=64+f*32;
//	f=64+f*32+g*8;
//	f=80+f*32+g*8;
//	f=72+f*32+g*8;
	f=68+f*32+g*8;
	height=f;

	for(z=0; z<128; z++)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);

		if(z>=f)
		{
			if(z>(64+2))
			{
//				vox->type=VOX_TY_GRASS|VOX_TYM_FLEXFORM;
//				vox->type=VOX_TY_DIRT|VOX_TYM_FLEXFORM;
				vox->type=terr->vox_grass|VOX_TYM_FLEXFORM;

//				vox->type=VOX_TY_STONE;
//				vox->aux=0xF0;

				g=BTGE_Noise_GenValue2(terr->noise2, y/12.0, x/12.0);
				if(g<0.33)
				{
//					vox->type=VOX_TY_SAND|VOX_TYM_FLEXFORM;
					vox->type=terr->vox_grass2|VOX_TYM_FLEXFORM;
				}

				BTGE_Voxel_GenerateDirtPanel(world, terr, vox, x, y, z);
				BTGE_Voxel_GenerateSpawner(world, terr, vox, x, y, z);
				break;
			}

//			vox->type=VOX_TY_SAND|VOX_TYM_FLEXFORM;
			vox->type=terr->vox_sand|VOX_TYM_FLEXFORM;
//			vox->type=VOX_TY_STONE;
//			vox->aux=0xF0;

			BTGE_Voxel_GenerateSpawner(world, terr, vox, x, y, z);

			for(z=z+1; z<64; z++)
			{
				vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
//				vox->type=VOX_TY_SLIME;
//				vox->type=terr->vox_water|VOX_TYM_FLEXFORM;
				vox->type=terr->vox_water;
//				vox->aux=0xF0;
			}

			break;
		}
			
		if(z>=(f-8))
		{
//			vox->type=VOX_TY_DIRT|VOX_TYM_FLEXFORM;
			vox->type=terr->vox_dirt|VOX_TYM_FLEXFORM;
//			vox->aux=0xF0;

			if((z>=(64-4)) && (z<(64+2)))
			{
//				vox->type=VOX_TY_SAND|VOX_TYM_FLEXFORM;
				vox->type=terr->vox_sand|VOX_TYM_FLEXFORM;
			}

			BTGE_Voxel_GenerateDirtPanel(world, terr, vox, x, y, z);

			continue;
		}

		if(z<4)
		{
//			vox->aux=0xF0;
			g=BTGE_Noise_Random(terr->noise2);
			if(!z || (g<0.75))
				{ vox->type=VOX_TY_BEDROCK|VOX_TYM_FLEXFORM; continue; }
		}

		BTGE_Voxel_GenerateVoxelOre(world, terr, NULL, vox, x, y, z);
	}

	BTGE_Voxel_GenerateSpanCaves(world, terr, x, y, height);
	BTGE_Voxel_GenerateSpanFillLava(world, terr, x, y, 12);
	BTGE_Voxel_GenerateSpanBorder(world, terr, x, y);
}

BTGE_API void BTGE_Voxel_GenerateSpanBaseCave(BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	BTGE_VoxelData *vox;
	float height;
	float f, g;
	int z;
	int i, j;

#if 0
//	f=BTGE_Noise_GenValue2(terr->noise, x/32.0, y/32.0);
//	f=BTGE_Noise_GenValue2(terr->noise, x/256.0, y/256.0);
	f=BTGE_Noise_GenValue2(terr->noise, x/128.0, y/128.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, x/32.0, y/32.0);
	g=BTGE_Noise_GenValue2(terr->noise2, x/16.0, y/16.0);
//	f=64+f*32;
//	f=64+f*32+g*8;
//	f=80+f*32+g*8;
//	f=72+f*32+g*8;
	f=68+f*32+g*8;
	height=f;
#endif

	for(z=0; z<128; z++)
	{
		f=BTGE_Noise_GenValue3(terr->noise2, x/128.0, y/128.0, z/128.0);
		g=BTGE_Noise_GenValue3(terr->noise3, x/16.0, y/16.0, z/16.0);
//		f=64+f*32;
//		f=64+f*32+g*8;
//		f=80+f*32+g*8;
//		f=72+f*32+g*8;
		f=68+f*32+g*8;

//		g=1.75-fabs((z-64)/64.0);
//		g=1+pow(sin(M_PI*(z-64)/64.0), 2);
		g=0.75+pow(sin(M_PI*(z-64)/64.0), 2);
//		g=pow(g, 1.0/4.0);
		g=pow(g, 1.0/8.0);
		f*=g;
		
		height=f;

//		if((height>72) && (z>6) && (z<122))
//		if((height>80) && (z>8) && (z<112))
		if((height>72) && (z>8) && (z<112))
			continue;

		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);

		if(z<4)
		{
//			vox->aux=0xF0;
			g=BTGE_Noise_Random(terr->noise2);
			if(!z || (g<0.75))
				{ vox->type=VOX_TY_BEDROCK|VOX_TYM_FLEXFORM; continue; }
		}

		if(z>124)
		{
//			vox->aux=0xF0;
			g=BTGE_Noise_Random(terr->noise2);
			if(!z || (g<0.75) || (z==127))
				{ vox->type=VOX_TY_BEDROCK|VOX_TYM_FLEXFORM; continue; }
		}

		BTGE_Voxel_GenerateVoxelOre(world, terr, NULL, vox, x, y, z);
	}

	BTGE_Voxel_GenerateSpanCaves(world, terr, x, y, 128);
	BTGE_Voxel_GenerateSpanFillLava(world, terr, x, y, 16);
	BTGE_Voxel_GenerateSpanBorder(world, terr, x, y);
}

BTGE_API int BTGE_Voxel_SetTerrainDefaults(BTGE_VoxelTerrGen *tmp)
{
	tmp->vox_stone=VOX_TY_STONE;
	tmp->vox_dirt=VOX_TY_DIRT;
	tmp->vox_grass=VOX_TY_GRASS;
	tmp->vox_grass2=VOX_TY_GRASS;
	tmp->vox_sand=VOX_TY_SAND;
	tmp->vox_water=VOX_TY_WATER;
	tmp->vox_lava=VOX_TY_LAVA;
	
	tmp->biomes=gcalloc(64*sizeof(BTGE_VoxelBiomeInfo *));
	tmp->ores=gcalloc(64*sizeof(BTGE_VoxelOreInfo));
	return(0);
}

BTGE_API int BTGE_Voxel_LoadTerrainInfo(
	BTGE_VoxelTerrGen *tmp, char *name)
{
	VFILE *fd;
	void *p;
	char buf[256];
	char **a, *s;
	int i, j;

	BTGE_Voxel_Setup();

	if(!name || !(*name))
		return(-1);

//	strcpy(buf, tmp->name);
	strcpy(buf, name);
	s=buf+strlen(buf);
	while((s>buf) && (*s!='.'))s--;
	if(*s!='.')s=buf+strlen(buf);
	strcpy(s, ".txt");

//	seed=rand()*rand()*rand()*rand();

	fd=vffopen(buf, "rt");
	if(!fd)return(-1);

	while(!vfeof(fd))
	{
		vfgets(buf, 255, fd);
		a=gcrsplit(buf);
		if(!a[0])continue;

		if(!strcmp(a[0], "$flags"))
		{
			for(i=1; a[i]; i++)
			{
				if(!strcmp(a[i], "borders"))
					{ tmp->flags|=VOX_TGFL_BORDERS; continue; }
				if(!strcmp(a[i], "ores"))
					{ tmp->flags|=VOX_TGFL_ORES; continue; }
				if(!strcmp(a[i], "caves"))
					{ tmp->flags|=VOX_TGFL_CAVES; continue; }
				if(!strcmp(a[i], "panels"))
					{ tmp->flags|=VOX_TGFL_PANELS; continue; }
				if(!strcmp(a[i], "spawners"))
					{ tmp->flags|=VOX_TGFL_SPAWNERS; continue; }

				if(!strcmp(a[i], "sun_ambient"))
					{ tmp->flags|=VOX_TGFL_SUN_AMBIENT; continue; }
			}
		}

		if(!strcmp(a[0], "$terr_seed"))
			{ tmp->seed=gcratoi(a[1]); continue; }

		if(!strcmp(a[0], "$terr_SpanGenerator"))
		{
			i=tmp->n_GenerateSpan++;
			tmp->GenerateSpan[i]=dyllGetAddr(a[1]);
			tmp->GenerateSpan_name[i]=dystrdup(a[1]);
			continue;
		}

		if(!strcmp(a[0], "$terr_vox_stone"))
		{
			i=BTGE_Voxel_IndexVoxelName(a[1]);
			if(i<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}
			tmp->vox_stone=i;
			continue;
		}

		if(!strcmp(a[0], "$terr_vox_dirt"))
		{
			i=BTGE_Voxel_IndexVoxelName(a[1]);
			if(i<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}
			tmp->vox_dirt=i;
			continue;
		}

		if(!strcmp(a[0], "$terr_vox_grass"))
		{
			i=BTGE_Voxel_IndexVoxelName(a[1]);
			if(i<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}
			tmp->vox_grass=i;
			continue;
		}

		if(!strcmp(a[0], "$terr_vox_grass2"))
		{
			i=BTGE_Voxel_IndexVoxelName(a[1]);
			if(i<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}
			tmp->vox_grass2=i;
			continue;
		}

		if(!strcmp(a[0], "$terr_vox_sand"))
		{
			i=BTGE_Voxel_IndexVoxelName(a[1]);
			if(i<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}
			tmp->vox_sand=i;
			continue;
		}

		if(!strcmp(a[0], "$terr_vox_water"))
		{
			i=BTGE_Voxel_IndexVoxelName(a[1]);
			if(i<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}
			tmp->vox_water=i;
			continue;
		}

		if(!strcmp(a[0], "$terr_vox_lava"))
		{
			i=BTGE_Voxel_IndexVoxelName(a[1]);
			if(i<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}
			tmp->vox_lava=i;
			continue;
		}

		if(!strcmp(a[0], "$terr_ore"))
		{
			j=BTGE_Voxel_IndexVoxelName(a[1]);
			if(j<0)
			{
				printf("BTGE_Voxel_LoadTerrainInfo: "
					"Fail Voxel Type %s\n", a[1]);
				continue;
			}

			i=tmp->n_ores++;
			tmp->ores[i].ty=j;
			tmp->ores[i].wf=atof(a[2]);
			tmp->ores[i].fl=0;
			continue;
		}

		if(!strcmp(a[0], "$terr_biome"))
		{
			sprintf(buf, "voxel/biome/biome_%s.txt", a[1]);

			i=tmp->n_biomes++;
			tmp->biomes[i]=gctalloc("btge_voxelbiomeinfo_t",
				sizeof(BTGE_VoxelBiomeInfo));
			tmp->biomes[i]->name=dystrdup(a[1]);

			tmp->biomes[i]->vox_stone=tmp->vox_stone;
			tmp->biomes[i]->vox_dirt=tmp->vox_dirt;
			tmp->biomes[i]->vox_grass=tmp->vox_grass;
			tmp->biomes[i]->vox_grass2=tmp->vox_grass2;
			tmp->biomes[i]->vox_sand=tmp->vox_sand;
			tmp->biomes[i]->vox_water=tmp->vox_water;
			tmp->biomes[i]->vox_lava=tmp->vox_lava;

			BTGE_Voxel_LoadBiomeInfo(tmp->biomes[i], buf);
			continue;
		}

		if(!strcmp(a[0], "$terr_map"))
		{
			tmp->portal_map[atoi(a[1])]=dystrdup(a[2]);
			continue;
		}
	}

	vfclose(fd);
	return(0);
}

BTGE_API void BTGE_Voxel_GenerateFillGlobeMapPixel(
	BTGE_BrushWorld *world, BTGE_VoxelTerrGen *terr,
	int mx, int my, byte *map, byte *rgb)
{
	BTGE_VoxelBiomeInfo *biome;
	float f, g, h, h2;
	float vx, vy;
	
//	vx=(mx-(terr->world_xs/2))*(32*16);
//	vy=(my-(terr->world_ys/2))*(32*16);

//	vx=mx-(terr->world_xs/2);
//	vy=my-(terr->world_ys/2);

//	vx=(mx-(terr->world_xs/2))*32;
//	vy=(my-(terr->world_ys/2))*32;

	vx=(mx-(terr->world_xs/2))*16;
	vy=(my-(terr->world_ys/2))*16;


	f=BTGE_Noise_GenValue2(terr->noise, vx/128.0, vy/128.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, vx/32.0, vy/32.0);
	g=BTGE_Noise_GenValue2(terr->noise2, vx/16.0, vy/16.0);
	h=BTGE_Noise_GenValue2(terr->noise2, vy/1024.0, vx/1024.0);
	h2=BTGE_Noise_GenValue2(terr->noise3, -vy/4096.0, -vx/4096.0);
//	f=64+f*32;
//	f=64+f*32+g*8;
//	f=80+f*32+g*8;
//	f=72+f*32+g*8;
//	f=68+f*32+g*8;
	f=68+f*32+g*8+h*32+h2*32;
//	height=f;

	biome=BTGE_Voxel_GenerateFindBiome(world, terr, vx, vy);
	
	map[0]=btge_clampi((f-64)*2+128, 0, 255);
//	map[1]=biome->id;
//	map[2]=0;
	map[1]=btge_clampi(biome->val_temperature*64+128, 0, 255);
	map[2]=btge_clampi(biome->val_humidity*64+128, 0, 255);

	map[3]=255;
	
	if(biome->val_temperature<=-1)
	{
		if(f<=64)
		{
			rgb[0]=btge_clampi(biome->clr_water[0]* 224, 0, 255);
			rgb[1]=btge_clampi(biome->clr_water[1]* 224, 0, 255);
			rgb[2]=btge_clampi(biome->clr_water[2]* 255, 0, 255);
			rgb[3]=255;
		}else
		{
			rgb[0]=btge_clampi(biome->clr_grass[0]*110, 0, 255);
			rgb[1]=btge_clampi(biome->clr_grass[1]*160, 0, 255);
			rgb[2]=btge_clampi(biome->clr_grass[2]*200, 0, 255);
			rgb[3]=255;
		}
	}else
	{
		if(f<=64)
		{
			rgb[0]=btge_clampi(biome->clr_water[0]* 50, 0, 255);
			rgb[1]=btge_clampi(biome->clr_water[1]* 50, 0, 255);
			rgb[2]=btge_clampi(biome->clr_water[2]* 140, 0, 255);
			rgb[3]=255;
		}else
		{
			rgb[0]=btge_clampi(biome->clr_grass[0]*100, 0, 255);
			rgb[1]=btge_clampi(biome->clr_grass[1]*110, 0, 255);
			rgb[2]=btge_clampi(biome->clr_grass[2]* 80, 0, 255);
			rgb[3]=255;
		}
	}
}

#if 1
BTGE_API void BTGE_Voxel_GenerateChunks(BTGE_BrushWorld *world)
{
	char tb[256];
	char tb2[256];
	BTGE_VoxelRegion *rgn;
	BTGE_VoxelData *vox;
	BTGE_NoiseContext *noise;
	BTGE_NoiseContext *noise2;
	BTGE_NoiseContext *noise3;
	BTGE_NoiseContext *noise4;
	BTGE_VoxelTerrGen *terr;
	byte *buf, *buf2;
	char *s;
	float tv[3];
	float f, g;
	s64 seed, sv1;
	int xs, ys;
	int i, j, k;

	//for now...
	if(world->vox_region)
		return;

	terr=gctalloc("btge_voxelterrgen_t", sizeof(BTGE_VoxelTerrGen));

	seed=rand()*rand()*rand()*rand();
//	seed=(s64)bgbrng_genvalue64();

	s=world->map_name;
	while(*s)seed=seed*4294967291LL+1+(*s++);

	terr->seed=seed;

	BTGE_Voxel_SetTerrainDefaults(terr);
	
	s=BTGE_VoxelSave_GetTerrainName(world);
	i=BTGE_Voxel_LoadTerrainInfo(terr, s);
	
	if(i<0)
	{
//		BTGE_Voxel_LoadTerrainInfo(terr, "region/terrain_default.txt");

		s=btCvarGet("sv_terrain_type");
		if(!s)s="default";
		sprintf(tb, "voxel/terrain_%s.txt", s);
		BTGE_Voxel_LoadTerrainInfo(terr, tb);
	}
	seed=terr->seed;

	sv1=seed;
	noise=BTGE_Noise_CreateContext(2, 256, sv1);
	sv1=sv1*4294967291LL+1;
	noise2=BTGE_Noise_CreateContext(3, 256, sv1);
	sv1=sv1*4294967291LL+1;
	noise3=BTGE_Noise_CreateContext(3, 256, sv1);
	sv1=sv1*4294967291LL+1;
	noise4=BTGE_Noise_CreateContext(3, 256, sv1);

//	noise=BTGE_Noise_CreateContext(2, 256, rand());
//	noise2=BTGE_Noise_CreateContext(3, 256, rand());
//	noise3=BTGE_Noise_CreateContext(3, 256, rand());
//	noise4=BTGE_Noise_CreateContext(3, 256, rand());

	terr->noise=noise;
	terr->noise2=noise2;
	terr->noise3=noise3;
	terr->noise4=noise4;
//	terr->seed=seed;

#if 0
	i=terr->n_GenerateSpan++;
	terr->GenerateSpan[i]=BTGE_Voxel_GenerateSpanBase;

	i=terr->n_GenerateSpan++;
	terr->GenerateSpan[i]=BTGE_Voxel_GenerateSpanBldg;

//	i=terr->n_GenerateSpan++;
//	terr->GenerateSpan[i]=BTGE_Voxel_GenerateLightSpan;
#endif

	world->vox_terr=terr;

	BTGE_Voxel_CheckGenerateRegionForPoint(world, -1, -1, 0, 1);
	BTGE_Voxel_CheckGenerateRegionForPoint(world,  1, -1, 0, 1);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, -1,  1, 0, 1);
	BTGE_Voxel_CheckGenerateRegionForPoint(world,  1,  1, 0, 1);
	
	if(!terr->world_map)
	{
		s=BTGE_VoxelSave_GetMapBase(world);
		sprintf(tb, "region/%s_map.png", s);
		sprintf(tb2, "region/%s_map_rgb.png", s);

		buf=BGBBTJ_PNG_LoadRaw(tb, &xs, &ys);
		buf2=BGBBTJ_PNG_LoadRaw(tb2, &xs, &ys);

//		if(buf && buf2)
		if(0)
		{
			terr->world_map=gcatomic(xs*ys*4);
			terr->world_xs=xs;
			terr->world_ys=ys;
			memcpy(terr->world_map, buf, xs*ys*4);
			BGBBTJ_PNG_Free(buf);

			terr->world_maprgb=gcatomic(xs*ys*4);
			memcpy(terr->world_maprgb, buf2, xs*ys*4);
			BGBBTJ_PNG_Free(buf2);
		}else
		{
//			xs=1024; ys=512;
//			xs=4096; ys=2048;
			xs=1024; ys=1024;
			buf=gcatomic(xs*ys*4);
			buf2=gcatomic(xs*ys*4);
			terr->world_map=buf;
			terr->world_maprgb=buf2;
			terr->world_xs=xs;
			terr->world_ys=ys;
			
			for(i=0; i<ys; i++)
				for(j=0; j<xs; j++)
			{
				BTGE_Voxel_GenerateFillGlobeMapPixel(
					world, terr, j, i,
					buf+(i*xs+j)*4,
					buf2+(i*xs+j)*4);
			}
			
			BGBBTJ_PNG_SaveRaw(tb, buf, xs, ys);
			BGBBTJ_PNG_SaveRaw(tb2, buf2, xs, ys);
		}
		
		terr->world_minx=-(terr->world_xs/2)*16;
		terr->world_miny=-(terr->world_ys/2)*16;
		terr->world_maxx= (terr->world_xs/2)*16;
		terr->world_maxy= (terr->world_ys/2)*16;
	}
}
#endif

#if 0
BTGE_API void BTGE_Voxel_GenerateChunks(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *rgn;
	BTGE_VoxelData *vox;
	BTGE_NoiseContext *noise;
	BTGE_NoiseContext *noise2;
	BTGE_NoiseContext *noise3;
	BTGE_NoiseContext *noise4;
	BTGE_VoxelTerrGen *terr;
	float tv[3];
	float f, g;
	s64 seed, sv1;
	int i, j, k;

	//for now...
	if(world->vox_region)
		return;

	BTGE_Voxel_Setup();

	seed=rand()*rand()*rand()*rand();
	
	sv1=seed;
	noise=BTGE_Noise_CreateContext(2, 256, sv1);
	sv1=sv1*4294967291LL+1;
	noise2=BTGE_Noise_CreateContext(3, 256, sv1);
	sv1=sv1*4294967291LL+1;
	noise3=BTGE_Noise_CreateContext(3, 256, sv1);
	sv1=sv1*4294967291LL+1;
	noise4=BTGE_Noise_CreateContext(3, 256, sv1);

//	noise=BTGE_Noise_CreateContext(2, 256, rand());
//	noise2=BTGE_Noise_CreateContext(3, 256, rand());
//	noise3=BTGE_Noise_CreateContext(3, 256, rand());
//	noise4=BTGE_Noise_CreateContext(3, 256, rand());

	terr=gctalloc("btge_voxelterrgen_t", sizeof(BTGE_VoxelTerrGen));
	terr->noise=noise;
	terr->noise2=noise2;
	terr->noise3=noise3;
	terr->noise4=noise4;
	terr->seed=seed;

	i=terr->n_GenerateSpan++;
	terr->GenerateSpan[i]=BTGE_Voxel_GenerateSpanBase;

	i=terr->n_GenerateSpan++;
	terr->GenerateSpan[i]=BTGE_Voxel_GenerateSpanBldg;

	i=terr->n_GenerateSpan++;
	terr->GenerateSpan[i]=BTGE_Voxel_GenerateLightSpan;

	world->vox_terr=terr;

	BTGE_Voxel_CheckGenerateRegionForPoint(world, -1, -1, 0, 1);
	BTGE_Voxel_CheckGenerateRegionForPoint(world,  1, -1, 0, 1);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, -1,  1, 0, 1);
	BTGE_Voxel_CheckGenerateRegionForPoint(world,  1,  1, 0, 1);
}
#endif

BTGE_API void BTGE_Voxel_BoxFloodFill(
	BTGE_BrushWorld *world,
	int mx, int my, int mz,
	int nx, int ny, int nz,
	int type)
{
	BTGE_VoxelData *vox;
	int i, j, k;

	for(i=mx; i<=nx; i++)
		for(j=my; j<=ny; j++)
			for(k=mz; k<=nz; k++)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world, i, j, k);
		if(vox)
		{
			vox->type=type;
//			vox->aux=0xF0;
		}
	}
}

BTGE_API bool BTGE_Voxel_GenerateFillRegion(
	BTGE_BrushWorld *world, BTGE_VoxelTerrGen *terr, BTGE_VoxelRegion *rgn)
{
	int xs, ys;
	int x, y;
	int i, j, k;

	if(rgn->oz)
		return(false);

//	if(!world->vox_terr)
//		return(false);
	if(!terr)
		return(false);

	xs=rgn->xs*rgn->xst;
	ys=rgn->ys*rgn->yst;
	for(k=0; k<terr->n_GenerateSpan; k++)
	{
		for(i=0; i<xs; i++)
			for(j=0; j<ys; j++)
		{
			x=rgn->ox+i; y=rgn->oy+j;
//			BTGE_Voxel_GenerateSpan(world, terr, x, y);
			terr->GenerateSpan[k](world, terr, x, y);
		}
	}

	return(true);
}

BTGE_API bool BTGE_Voxel_GenerateFillRange(
	BTGE_BrushWorld *world, int ox, int oy, int xs, int ys)
{
	BTGE_VoxelTerrGen *terr;
	int x, y;
	int i, j, k;

	terr=world->vox_terr;
	for(k=0; k<terr->n_GenerateSpan; k++)
	{
		for(i=0; i<xs; i++)
			for(j=0; j<ys; j++)
		{
			x=ox+i; y=oy+j;
			terr->GenerateSpan[k](world, terr, x, y);
		}
	}
	
	return(true);
}

BTGE_API BTGE_VoxelRegion *BTGE_Voxel_CheckGenerateRegionForPoint(
	BTGE_BrushWorld *world, int x, int y, int z, int fl)
{
	BTGE_VoxelRegion *rgn;

	rgn=BTGE_Voxel_GetWorldRegionFlags(world, x, y, z, 0);
	if(rgn)return(rgn);

	if(!(fl&2))
	{
		if((z<0) || (z>=128))
			return(NULL);
	}

	rgn=BTGE_Voxel_AllocRegionForPoint(world, x, y, z);
//	BTGE_Voxel_GenerateFillRegion(world, world->vox_terr, rgn);

	BTGE_VoxelSave_LoadRegion(world, rgn);

	return(rgn);
}

BTGE_API bool BTGE_Voxel_CheckGenerateFillChucks(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	int x, int y, float *org, int fl)
{
	float tv[3];

	if(!world->vox_terr)
		return(false);

#if 1
	if(org)
	{
		//determine if too far away
		tv[0]=(rgn->ox+x*rgn->xst)*32;
		tv[1]=(rgn->oy+y*rgn->yst)*32;
//		tv[2]=(rgn->oz+z*rgn->zst)*32;
		tv[2]=0;
		if(V3F_DIST(org, tv)>8192)
			return(false);
	}

	//try to generate spans
	BTGE_Voxel_GenerateFillRange(world,
		rgn->ox + x*rgn->xst,
		rgn->oy + y*rgn->yst,
		rgn->xst, rgn->yst);

	//try to generate dungeons
	BTGE_Voxel_GenerateFillDungeonRange(world,
		rgn->ox + x*rgn->xst,
		rgn->oy + y*rgn->yst,
		rgn->xst, rgn->yst);

	return(true);
#endif
}

BTGE_API void BTGE_Voxel_UnlinkRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelRegion *cur, *prv;

	cur=world->vox_region; prv=NULL;
	while(cur)
	{
		if(cur==rgn)
		{
			if(prv)
				{ prv->next=cur->next; }
			else
				{ world->vox_region=cur->next; }
			break;
		}
		prv=cur; cur=cur->next;
	}
}

BTGE_API void BTGE_Voxel_UnloadRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelSave_SaveRegion(world, rgn);
	BTGE_Voxel_UnlinkRegion(world, rgn);
	BTGE_Voxel_FreeRegion(world, rgn);
}

BTGE_API void BTGE_Voxel_CheckUnloadRegions(
	BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur, *nxt, *cur2, *prv2, *lst;
	int n;

	cur=world->vox_region; lst=NULL;
	while(cur)
	{
		cur2=lst; prv2=NULL;
		while(cur2 && ((cur2->rgn_tick_inactive)<cur->rgn_tick_inactive))
			{ prv2=cur2; cur2=cur2->chain; }
		if(prv2)
			{ cur->chain=prv2->chain; prv2->chain=cur; }
		else
			{ cur->chain=lst; lst=cur; }
		cur=cur->next;
	}

	cur=lst; n=0;
	while(cur && ((n<8) || (cur->rgn_tick_inactive<256)))
		{ cur=cur->chain; n++; }
		
	while(cur)
	{
		nxt=cur->chain;
		BTGE_Voxel_UnloadRegion(world, cur);
		cur=nxt;
	}
}
