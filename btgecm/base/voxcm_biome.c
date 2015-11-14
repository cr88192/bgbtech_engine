#include <btgecm.h>

static char *btge_voxel_biomename[]=
{
"default",		//0
"plains",		//1
"desert",		//2
"hills",		//3
"forest",		//4
"taiga",		//5
"swamp",		//6
"jungle",		//7
"hell",			//8
"sky",			//9
"mushroom",		//10
"acid",			//11
"metal",		//12
"tundra",		//13
NULL
};

BTGE_VoxelBiomeInfo *btge_voxel_biome_idx[64];

BTGE_VoxelBiomeInfo *btge_voxel_biome[64];
int btge_voxel_n_biome;

BTGE_API int BTGE_Voxel_SetupBiome()
{
	static int init=0;
	char tb[256];
	int i, j, k;
	
	if(init)return(0);
	init=1;
	
	for(i=0; btge_voxel_biomename[i]; i++)
	{
		sprintf(tb, "voxel/biome/biome_%s.txt",
			btge_voxel_biomename[i]);

		i=btge_voxel_n_biome++;
		btge_voxel_biome[i]=gctalloc("btge_voxelbiomeinfo_t",
			sizeof(BTGE_VoxelBiomeInfo));
		btge_voxel_biome[i]->name=dystrdup(btge_voxel_biomename[i]);

//		tmp->biomes[i]->vox_stone=tmp->vox_stone;
//		tmp->biomes[i]->vox_dirt=tmp->vox_dirt;
//		tmp->biomes[i]->vox_grass=tmp->vox_grass;
//		tmp->biomes[i]->vox_grass2=tmp->vox_grass2;
//		tmp->biomes[i]->vox_sand=tmp->vox_sand;
//		tmp->biomes[i]->vox_water=tmp->vox_water;
//		tmp->biomes[i]->vox_lava=tmp->vox_lava;

		BTGE_Voxel_LoadBiomeInfo(btge_voxel_biome[i], tb);
		
		j=btge_voxel_biome[i]->id;
		btge_voxel_biome_idx[j]=btge_voxel_biome[i];
	}
	
	return(1);
}

BTGE_API int BTGE_Voxel_IndexBiomeName(char *name)
{
	int i;

	for(i=0; btge_voxel_biomename[i]; i++)
	{
		if(!strcmp(btge_voxel_biomename[i], name))
			return(i);
	}
	return(-1);
}

BTGE_API int BTGE_Voxel_LoadBiomeInfo(BTGE_VoxelBiomeInfo *tmp, char *name)
{
	VFILE *fd;
	void *p;
	char buf[256];
	char **a, *s;
	int i;

	BTGE_Voxel_SetupBiome();

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
			for(i=1; a[i]; i++)
			{
				if(!strcmp(a[i], "trees2x"))
					{ tmp->flags|=VOX_BIFL_TREES2X; }
				if(!strcmp(a[i], "trees4x"))
					{ tmp->flags|=VOX_BIFL_TREES4X; }
				if(!strcmp(a[i], "trees8x"))
					{ tmp->flags|=VOX_BIFL_TREES8X; }
				if(!strcmp(a[i], "trees_neg"))
					{ tmp->flags|=VOX_BIFL_TREESNX; }

				if(!strcmp(a[i], "hills2x"))
					{ tmp->flags|=VOX_BIFL_HILLS2X; }
				if(!strcmp(a[i], "hills4x"))
					{ tmp->flags|=VOX_BIFL_HILLS4X; }
//				if(!strcmp(a[i], "hills8x"))
//					{ tmp->flags|=VOX_BIFL_HILLS8X; }
				if(!strcmp(a[i], "hills_neg"))
					{ tmp->flags|=VOX_BIFL_HILLSNX; }
			}
		}

		if(!strcmp(a[0], "$biome_id"))
			{ tmp->id=atoi(a[1]); }

		if(!strcmp(a[0], "$biome_val_arcane"))
			{ tmp->val_arcane=atof(a[1]); }
		if(!strcmp(a[0], "$biome_val_temperature"))
			{ tmp->val_temperature=atof(a[1]); }
		if(!strcmp(a[0], "$biome_val_humidity"))
			{ tmp->val_humidity=atof(a[1]); }

		if(!strcmp(a[0], "$biome_clr_grass"))
		{
			tmp->clr_grass[0]=atof(a[1])/255.0;
			tmp->clr_grass[1]=atof(a[2])/255.0;
			tmp->clr_grass[2]=atof(a[3])/255.0;
			continue;
		}

		if(!strcmp(a[0], "$biome_clr_water"))
		{
			tmp->clr_water[0]=atof(a[1])/255.0;
			tmp->clr_water[1]=atof(a[2])/255.0;
			tmp->clr_water[2]=atof(a[3])/255.0;
			continue;
		}

		if(!strcmp(a[0], "$biome_clr_sun"))
		{
			tmp->clr_sun[0]=atof(a[1])/255.0;
			tmp->clr_sun[1]=atof(a[2])/255.0;
			tmp->clr_sun[2]=atof(a[3])/255.0;
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
	}

	vfclose(fd);
	return(0);
}

BTGE_API void BTGE_Voxel_CalcBiomeParms(
	BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y,
	float *tv)
{
	float f, g, h;
	float f1, g1, h1;
	float f2, g2, h2;
	float d, bd;
	int bi;
	int i, j;

//	f=BTGE_Noise_GenValue2(terr->noise, x/512.0, y/512.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, x/512.0, y/512.0);
//	h=BTGE_Noise_GenValue2(terr->noise3, x/512.0, y/512.0);

//	f=BTGE_Noise_GenValue2(terr->noise, -x/256.0, -y/256.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, -x/256.0, -y/256.0);
//	h=BTGE_Noise_GenValue2(terr->noise3, -x/256.0, -y/256.0);

//	f=BTGE_Noise_GenValue2(terr->noise, -x/128.0, -y/128.0);
//	g=BTGE_Noise_GenValue2(terr->noise2, -x/128.0, -y/128.0);
//	h=BTGE_Noise_GenValue2(terr->noise3, -x/128.0, -y/128.0);

	f=BTGE_Noise_GenValue2(terr->noise, -x/1024.0, -y/1024.0);
	g=BTGE_Noise_GenValue2(terr->noise2, -x/1024.0, -y/1024.0);
	h=BTGE_Noise_GenValue2(terr->noise3, -x/1024.0, -y/1024.0);

//	f1=BTGE_Noise_GenValue2(terr->noise, -x/256.0, -y/256.0);
//	g1=BTGE_Noise_GenValue2(terr->noise2, -x/256.0, -y/256.0);
//	h1=BTGE_Noise_GenValue2(terr->noise3, -x/256.0, -y/256.0);

	f1=BTGE_Noise_GenValue2(terr->noise, -y/256.0, -x/256.0);
	g1=BTGE_Noise_GenValue2(terr->noise2, -y/256.0, -x/256.0);
	h1=BTGE_Noise_GenValue2(terr->noise3, -y/256.0, -x/256.0);

//	f2=BTGE_Noise_GenValue2(terr->noise, y/4096.0, x/4096.0);
//	g2=BTGE_Noise_GenValue2(terr->noise2, y/4096.0, x/4096.0);
//	h2=BTGE_Noise_GenValue2(terr->noise3, y/4096.0, x/4096.0);

	f=f+h1*0.5;
	g=g+f1*0.5;
	h=h+g1*0.5;
	
//	f=f+h1*0.5+h2*0.25;
//	g=g+f1*0.5+f2*0.25;
//	h=h+g1*0.5+g2*0.25;

#if 1
	if(terr->world_ys)
	{
		f1=((float)y*(1.0/16))/(terr->world_ys/2);
//		f1=((float)y*(1.0/16))/terr->world_ys;
		f1=f1*f1;
//		g+=1.25-f1;
		g+=0.25-f1;
	}
#endif

//	f*=2; g*=2; h*=2;
	f*=3; g*=3; h*=3;
	
	tv[0]=f;
	tv[1]=g;
	tv[2]=h;
}

BTGE_API BTGE_VoxelBiomeInfo *BTGE_Voxel_GenerateFindBiome(
	BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int x, int y)
{
	float tv0[4], tv1[4];
	float f, g, h;
	float f1, g1, h1;
	float f2, g2, h2;
	float d, bd;
	int bi;
	int i, j;

	if(terr->n_biomes<=0)
		return(NULL);

	BTGE_Voxel_CalcBiomeParms(world, terr, x, y, tv0);
	f=tv0[0];
	g=tv0[1];
	h=tv0[2];
	
	bi=0; bd=999999.0;
	for(i=0; i<terr->n_biomes; i++)
	{
		tv0[0]=f;
		tv0[1]=g;
		tv0[2]=h;
		
		tv1[0]=terr->biomes[i]->val_arcane;
		tv1[1]=terr->biomes[i]->val_temperature;
		tv1[2]=terr->biomes[i]->val_humidity;
		
		d=V3F_DIST(tv0, tv1);
		
		if(d<bd)
			{ bi=i; bd=d; }
	}
	
	return(terr->biomes[bi]);
}

BTGE_API BTGE_VoxelBiomeInfo *BTGE_Voxel_GenerateFindBiomeID(
	BTGE_BrushWorld *world,
	BTGE_VoxelTerrGen *terr, int id)
{
	int i;
	
	for(i=0; i<terr->n_biomes; i++)
	{
		if(terr->biomes[i]->id==id)
			return(terr->biomes[i]);
	}
	return(NULL);
}


BTGE_API BTGE_VoxelBiomeInfo *BTGE_Voxel_FetchBiomeForPoint(
	BTGE_BrushWorld *world, int x, int y, int z)
{
	BTGE_VoxelChunk *chk;
	int i;

	BTGE_Voxel_SetupBiome();

	chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 0);
	if(!chk)return(NULL);

	return(btge_voxel_biome_idx[chk->biome]);

#if 0
	for(i=0; i<btge_voxel_n_biome; i++)
	{
		if(btge_voxel_biome[i]->id==id)
			return(btge_voxel_biome[i]);
	}
	return(NULL);
#endif
}

BTGE_API BTGE_VoxelBiomeInfo *BTGE_Voxel_FetchBiomeForID(
	BTGE_BrushWorld *world, int id)
{
	int i;

	BTGE_Voxel_SetupBiome();

	return(btge_voxel_biome_idx[id]);

#if 0
	for(i=0; i<btge_voxel_n_biome; i++)
	{
		if(btge_voxel_biome[i]->id==id)
			return(btge_voxel_biome[i]);
	}
	return(NULL);
#endif
}


BTGE_API bool BTGE_Voxel_CalculateGrassColorForPoint(
	BTGE_BrushWorld *world, int x, int y, int z,
	float *rcr, float *rcg, float *rcb)
{
	float tvx0[3], tvx1[3], tvx2[3];
	float tvy0[3], tvy1[3], tvy2[3];
	float tv[3];
	BTGE_VoxelChunk *chk, *chknx, *chkpx, *chkny, *chkpy;
	BTGE_VoxelBiomeInfo *bixy, *binx, *bipx, *biny, *bipy;
	float tx, ty;
	int i;

	BTGE_Voxel_SetupBiome();

	chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 0);
	if(!chk)
	{
		*rcr=1.0; *rcg=1.0; *rcb=1.0;
		return(false);
	}

	chknx=BTGE_Voxel_GetWorldChunkFlags(world, x-16, y, z, 0);
	chkpx=BTGE_Voxel_GetWorldChunkFlags(world, x+16, y, z, 0);
	chkny=BTGE_Voxel_GetWorldChunkFlags(world, x, y-16, z, 0);
	chkpy=BTGE_Voxel_GetWorldChunkFlags(world, x, y+16, z, 0);

	if(!chknx)chknx=chk;
	if(!chkpx)chkpx=chk;
	if(!chkny)chkny=chk;
	if(!chkpy)chkpy=chk;

//	if(!chknx || !chkpx || !chkny || !chkpy)
//	{
//		*rcr=1.0; *rcg=1.0; *rcb=1.0;
//		return(NULL);
//	}

	bixy=btge_voxel_biome_idx[chk->biome];
	binx=btge_voxel_biome_idx[chknx->biome];
	bipx=btge_voxel_biome_idx[chkpx->biome];
	biny=btge_voxel_biome_idx[chkny->biome];
	bipy=btge_voxel_biome_idx[chkpy->biome];

	if(!bixy || !binx || !bipx || !biny || !bipy)
	{
		*rcr=1.0; *rcg=1.0; *rcb=1.0;
		return(false);
	}

	tx=(x&15)/15.0;
	ty=(y&15)/15.0;
//	tx=tx*0.5+0.25;
//	ty=ty*0.5+0.25;
	
	V3F_SCALEADDSCALE(binx->clr_grass, (1.0-tx), bixy->clr_grass, tx, tvx0);
	V3F_SCALEADDSCALE(bixy->clr_grass, (1.0-tx), bipx->clr_grass, tx, tvx1);
	V3F_SCALEADDSCALE(tvx0, (1.0-tx), tvx1, tx, tvx2);

	V3F_SCALEADDSCALE(biny->clr_grass, (1.0-ty), bixy->clr_grass, ty, tvy0);
	V3F_SCALEADDSCALE(bixy->clr_grass, (1.0-ty), bipy->clr_grass, ty, tvy1);
	V3F_SCALEADDSCALE(tvy0, (1.0-ty), tvy1, ty, tvy2);

	V3F_SCALEADDSCALE(tvx2, 0.5, tvy2, 0.5, tv);

	*rcr=tv[0];
	*rcg=tv[1];
	*rcb=tv[2];
	
	return(true);

//	return(btge_voxel_biome_idx[chk->biome]);
}

BTGE_API bool BTGE_Voxel_CalculateLeavesColorForPoint(
	BTGE_BrushWorld *world, int x, int y, int z,
	float *rcr, float *rcg, float *rcb)
{
	return(BTGE_Voxel_CalculateGrassColorForPoint(
		world, x, y, z, rcr, rcg, rcb));
}

BTGE_API bool BTGE_Voxel_CalculateWaterColorForPoint(
	BTGE_BrushWorld *world, int x, int y, int z,
	float *rcr, float *rcg, float *rcb)
{
	float tvx0[3], tvx1[3], tvx2[3];
	float tvy0[3], tvy1[3], tvy2[3];
	float tv[3];
	BTGE_VoxelChunk *chk, *chknx, *chkpx, *chkny, *chkpy;
	BTGE_VoxelBiomeInfo *bixy, *binx, *bipx, *biny, *bipy;
	float tx, ty;
	int i;

	BTGE_Voxel_SetupBiome();

	chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 0);
	if(!chk)
		{ *rcr=1.0; *rcg=1.0; *rcb=1.0; return(false); }

	chknx=BTGE_Voxel_GetWorldChunkFlags(world, x-16, y, z, 0);
	chkpx=BTGE_Voxel_GetWorldChunkFlags(world, x+16, y, z, 0);
	chkny=BTGE_Voxel_GetWorldChunkFlags(world, x, y-16, z, 0);
	chkpy=BTGE_Voxel_GetWorldChunkFlags(world, x, y+16, z, 0);

	if(!chknx)chknx=chk;
	if(!chkpx)chkpx=chk;
	if(!chkny)chkny=chk;
	if(!chkpy)chkpy=chk;

	bixy=btge_voxel_biome_idx[chk->biome];
	binx=btge_voxel_biome_idx[chknx->biome];
	bipx=btge_voxel_biome_idx[chkpx->biome];
	biny=btge_voxel_biome_idx[chkny->biome];
	bipy=btge_voxel_biome_idx[chkpy->biome];

	if(!bixy || !binx || !bipx || !biny || !bipy)
		{ *rcr=1.0; *rcg=1.0; *rcb=1.0; return(false); }

	tx=(x&15)/15.0; ty=(y&15)/15.0;
	V3F_SCALEADDSCALE(binx->clr_water, (1.0-tx), bixy->clr_water, tx, tvx0);
	V3F_SCALEADDSCALE(bixy->clr_water, (1.0-tx), bipx->clr_water, tx, tvx1);
	V3F_SCALEADDSCALE(tvx0, (1.0-tx), tvx1, tx, tvx2);
	V3F_SCALEADDSCALE(biny->clr_water, (1.0-ty), bixy->clr_water, ty, tvy0);
	V3F_SCALEADDSCALE(bixy->clr_water, (1.0-ty), bipy->clr_water, ty, tvy1);
	V3F_SCALEADDSCALE(tvy0, (1.0-ty), tvy1, ty, tvy2);
	V3F_SCALEADDSCALE(tvx2, 0.5, tvy2, 0.5, tv);

	*rcr=tv[0]; *rcg=tv[1]; *rcb=tv[2];
	return(true);
}

BTGE_API bool BTGE_Voxel_CalculateSunColorForPoint(
	BTGE_BrushWorld *world, int x, int y, int z,
	float *rcr, float *rcg, float *rcb)
{
	float tvx0[3], tvx1[3], tvx2[3];
	float tvy0[3], tvy1[3], tvy2[3];
	float tv[3];
	BTGE_VoxelChunk *chk, *chknx, *chkpx, *chkny, *chkpy;
	BTGE_VoxelBiomeInfo *bixy, *binx, *bipx, *biny, *bipy;
	float tx, ty;
	int i;

	BTGE_Voxel_SetupBiome();

	chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 0);
	if(!chk)
		{ *rcr=1.0; *rcg=1.0; *rcb=1.0; return(false); }

	chknx=BTGE_Voxel_GetWorldChunkFlags(world, x-16, y, z, 0);
	chkpx=BTGE_Voxel_GetWorldChunkFlags(world, x+16, y, z, 0);
	chkny=BTGE_Voxel_GetWorldChunkFlags(world, x, y-16, z, 0);
	chkpy=BTGE_Voxel_GetWorldChunkFlags(world, x, y+16, z, 0);

	if(!chknx)chknx=chk;
	if(!chkpx)chkpx=chk;
	if(!chkny)chkny=chk;
	if(!chkpy)chkpy=chk;

	bixy=btge_voxel_biome_idx[chk->biome];
	binx=btge_voxel_biome_idx[chknx->biome];
	bipx=btge_voxel_biome_idx[chkpx->biome];
	biny=btge_voxel_biome_idx[chkny->biome];
	bipy=btge_voxel_biome_idx[chkpy->biome];

	if(!bixy || !binx || !bipx || !biny || !bipy)
		{ *rcr=1.0; *rcg=1.0; *rcb=1.0; return(false); }

	tx=(x&15)/15.0; ty=(y&15)/15.0;
	V3F_SCALEADDSCALE(binx->clr_sun, (1.0-tx), bixy->clr_sun, tx, tvx0);
	V3F_SCALEADDSCALE(bixy->clr_sun, (1.0-tx), bipx->clr_sun, tx, tvx1);
	V3F_SCALEADDSCALE(tvx0, (1.0-tx), tvx1, tx, tvx2);
	V3F_SCALEADDSCALE(biny->clr_sun, (1.0-ty), bixy->clr_sun, ty, tvy0);
	V3F_SCALEADDSCALE(bixy->clr_sun, (1.0-ty), bipy->clr_sun, ty, tvy1);
	V3F_SCALEADDSCALE(tvy0, (1.0-ty), tvy1, ty, tvy2);
	V3F_SCALEADDSCALE(tvx2, 0.5, tvy2, 0.5, tv);

	*rcr=tv[0]; *rcg=tv[1]; *rcb=tv[2];
	return(true);
}
