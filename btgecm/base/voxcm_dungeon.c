/*

Dungeon Generation:
Work in terms of chunks (16x16x16 voxels), which may have special types (as dungeon chunks). Dungeon chunks may connect onto certain other types of dungeon chunks along each edge, which may radiate outward.

When a chunk becomes a dungeon chunk, any voxels with a designated value within the chunk-type will be replaced by that value.

Certain chunk-types may be qualified to only exist in certian areas, for example, Z coordinate or biome.

*/

#include <btgecm.h>
#include <bgbbtj.h>

char *btge_vox_dungeon_name[256];
BTGE_VoxelDungeonInfo *btge_vox_dungeon_info[256];

int BTGE_Voxel_LoadDungeonInfo(
	BTGE_VoxelDungeonInfo *inf, char *name)
{
	VFILE *fd;
	void *p;
	char buf[256];
	char **a, *s;
	int i, j, k;

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

		if(!strcmp(a[0], "$dung_vox"))
		{
			j=BTGE_Voxel_IndexVoxelName(a[1]);
			if(j<0)
			{
				printf("BTGE_Voxel_LoadDungeonInfo: "
					"%s Bad Voxel Type %s\n", name, a[1]);
				continue;
			}

			i=inf->n_idx++;
			inf->idx_vox[i].type=j;

			inf->idx_rgba[i*4+0]=atoi(a[2]);
			inf->idx_rgba[i*4+1]=atoi(a[3]);
			inf->idx_rgba[i*4+2]=atoi(a[4]);
			inf->idx_rgba[i*4+3]=255;
			
			for(j=5; a[j]; j++)
			{
				if(!strcmp(a[j], "unbreakable"))
					{ inf->idx_vox[i].type|=VOX_TYM_UNBREAKABLE; }
				if(!strcmp(a[j], "flexform"))
					{ inf->idx_vox[i].type|=VOX_TYM_FLEXFORM; }
				if(!strncmp(a[j], "aux=", 4))
					{ inf->idx_vox[i].aux=atoi(a[j]+4); }
				if(!strncmp(a[j], "aux2=", 5))
					{ inf->idx_vox[i].aux2=atoi(a[j]+5); }
				if(!strncmp(a[j], "vlight=", 7))
					{ inf->idx_vox[i].vlight=atoi(a[j]+7); }
				if(!strncmp(a[j], "slight=", 7))
					{ inf->idx_vox[i].slight=atoi(a[j]+7); }
				if(!strncmp(a[j], "delay=", 6))
					{ inf->idx_vox[i].delay=atoi(a[j]+6); }
			}
		}
		
		if(!strcmp(a[0], "$dung_allow_nx"))
		{
			i=inf->allow_n_edge[0]++;
			inf->allow_edge[0][i]=BTGE_Voxel_IndexDungeonName(a[1]);
		}
		if(!strcmp(a[0], "$dung_allow_px"))
		{
			i=inf->allow_n_edge[1]++;
			inf->allow_edge[1][i]=BTGE_Voxel_IndexDungeonName(a[1]);
		}
		if(!strcmp(a[0], "$dung_allow_ny"))
		{
			i=inf->allow_n_edge[2]++;
			inf->allow_edge[2][i]=BTGE_Voxel_IndexDungeonName(a[1]);
		}
		if(!strcmp(a[0], "$dung_allow_py"))
		{
			i=inf->allow_n_edge[3]++;
			inf->allow_edge[3][i]=BTGE_Voxel_IndexDungeonName(a[1]);
		}
		if(!strcmp(a[0], "$dung_allow_nz"))
		{
			i=inf->allow_n_edge[4]++;
			inf->allow_edge[4][i]=BTGE_Voxel_IndexDungeonName(a[1]);
		}
		if(!strcmp(a[0], "$dung_allow_pz"))
		{
			i=inf->allow_n_edge[5]++;
			inf->allow_edge[5][i]=BTGE_Voxel_IndexDungeonName(a[1]);
		}

		if(!strcmp(a[0], "$dung_allow_biome"))
		{
			i=inf->allow_n_biome++;
			inf->allow_biome[i]=BTGE_Voxel_IndexBiomeName(a[1]);
		}

		if(!strcmp(a[0], "$dung_deny_biome"))
		{
			i=inf->deny_n_biome++;
			inf->deny_biome[i]=BTGE_Voxel_IndexBiomeName(a[1]);
		}

		if(!strcmp(a[0], "$dung_min_z"))
		{
			inf->min_z=atoi(a[1]);
		}

		if(!strcmp(a[0], "$dung_max_z"))
		{
			inf->max_z=atoi(a[1]);
		}
	}

	vfclose(fd);
	return(0);
}

BTGE_API int BTGE_Voxel_LoadDungeonType(int idx, char *name)
{
	char tb[256];
	BTGE_VoxelDungeonInfo *inf;
	byte *buf, *pix, *pix2;
	int xs, ys;
	int x0, y0, z0, x1, y1, x, y;
	int i, j, k;

	printf("BTGE_Voxel_LoadDungeonType: %d %s\n", idx, name);

	if(!btge_vox_dungeon_info[idx])
	{
		inf=gcalloc(sizeof(BTGE_VoxelDungeonInfo));
//		BTGE_Voxel_SetInfoDefaults(vinf);
		btge_vox_dungeon_info[idx]=inf;
	}else
	{
		inf=btge_vox_dungeon_info[idx];
		if(!strcmp(inf->name, name))
			return(0);

		//voxel has changed types
//		BTGE_Voxel_SetInfoDefaults(vinf);
	}

	inf->name=dystrdup(name);
	inf->type=idx;
	
	inf->min_z=16;
	inf->max_z=112;

	//index 0 is special, and means the block is unchanged.
	i=inf->n_idx++;
	inf->idx_vox[i].type=VOX_TYS_ANYSOLIDBLOCK;
	inf->idx_rgba[i*4+0]=255;
	inf->idx_rgba[i*4+1]=255;
	inf->idx_rgba[i*4+2]=255;
	inf->idx_rgba[i*4+3]=255;

	sprintf(tb, "voxel/dungeon/dungeon_%s", name);
	BTGE_Voxel_LoadDungeonInfo(inf, tb);

	for(i=0; i<16; i++)
		for(j=0; j<16; j++)
			for(k=0; k<16; k++)
	{
		inf->vox_idx[i*256+j*16+k]=0;
	}

	sprintf(tb, "voxel/dungeon/dungeon_%s.png", name);
	buf=BGBBTJ_PNG_LoadRaw(tb, &xs, &ys);
	if(!buf)
		{ return(-1); }

	if((xs!=64) || (ys!=64))
	{
		printf("BTGE_Voxel_LoadDungeonType: %d %s: Fail PNG\n", idx, name);
		BGBBTJ_PNG_Free(buf);
		return(-1);
	}

	for(z0=0; z0<16; z0++)
	{
		y0=z0/4; x0=z0%4;
		for(y1=0; y1<16; y1++)
			for(x1=0; x1<16; x1++)
		{
			x=x0*16+x1;
			y=y0*16+(15-y1);
			pix=buf+(((63-y)*64+x)*4);
			
			for(i=0; i<inf->n_idx; i++)
			{
				pix2=inf->idx_rgba+(i*4);
				if((pix[0]==pix2[0]) &&
					(pix[1]==pix2[1]) &&
					(pix[2]==pix2[2]))
						break;
			}
			if(i<inf->n_idx)
				{ inf->vox_idx[z0*256+y1*16+x1]=i; }
		}
	}

	BGBBTJ_PNG_Free(buf);

	printf("BTGE_Voxel_LoadDungeonType: %d %s: OK\n", idx, name);

	return(0);
}

int BTGE_Voxel_LoadDungeonList(char *name)
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

		if(!strcmp(a[0], "$dungeon"))
		{
			btge_vox_dungeon_name[atoi(a[1])]=dystrdup(a[2]);
//			BTGE_Voxel_LoadDungeonType(
//				atoi(a[1]), dyllStrdup(a[2]));
		}
	}

	for(i=0; i<256; i++)
	{
		if(!btge_vox_dungeon_name[i])
			continue;
		BTGE_Voxel_LoadDungeonType(
			i, btge_vox_dungeon_name[i]);
	}

	vfclose(fd);
	return(0);
}

BTGE_API int BTGE_Voxel_IndexDungeonName(char *name)
{
	int i;
	
	for(i=0; i<256; i++)
	{
		if(btge_vox_dungeon_name[i] &&
			!strcmp(btge_vox_dungeon_name[i], name))
				return(i);
	}
	return(-1);
}

void BTGE_Voxel_SetupDungeon()
{
	BTGE_Voxel_LoadDungeonList("voxel/dungeon_list");
}

BTGE_API bool BTGE_VoxelDungeon_SpawnWorldDungeon(
	BTGE_BrushWorld *world, BTGE_VoxelDungeonInfo *dung,
	int xo, int yo, int zo, int fl)
{
	BTGE_VoxelChunk *chk;
	BTGE_VoxelData vox;
	int px, py, pz;
	int x, y, z;
	int i, j, k;

	chk=BTGE_Voxel_GetWorldChunkFlags(world, xo, yo, zo, fl);
	if(!chk)
	{
		printf("BTGE_VoxelDungeon_SpawnWorldDungeon: No Chunk\n");
		return(false);
	}

	chk->dungeon_type=dung->type;
	chk->dungeon_attr=0;

	for(pz=0; pz<16; pz++)
		for(py=0; py<16; py++)
			for(px=0; px<16; px++)
	{
		x=xo+px;	y=yo+py;	z=zo+pz;
		
		i=pz*256+py*16+px;
		j=dung->vox_idx[i];
		if(!j)continue;

		vox=BTGE_Voxel_GetWorldVoxelData(world, x, y, z);
		if((vox.type&VOX_TY_MASK)==VOX_TY_BEDROCK)
			continue;
		if((vox.type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE)
			continue;
		
		BTGE_Voxel_SetWorldVoxelDataFlags(world,
			x, y, z, fl, dung->idx_vox[j]);
	}

	return(true);
}

BTGE_API int BTGE_VoxelDungeon_PickChunkEdgeDungeonRandom(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	int edge)
{
	BTGE_VoxelDungeonInfo *dung;
	BTGE_VoxelTerrGen *terr;
	int n;
	int i, j, k;
	
	if(!chk)
		return(VOX_DTY_UNASSIGNED);
	
	if((chk->dungeon_type==VOX_DTY_UNASSIGNED) ||
		(chk->dungeon_type==VOX_DTY_NONE))
			return(VOX_DTY_UNASSIGNED);
	
	dung=btge_vox_dungeon_info[chk->dungeon_type];
	if(!dung)
		return(VOX_DTY_UNASSIGNED);

	terr=world->vox_terr;
		
	n=dung->allow_n_edge[edge];
	if(!n) { return(VOX_DTY_UNASSIGNED); }
	
	i=n*BTGE_Noise_SqRandom(terr->noise3);
	j=dung->allow_edge[edge][i];
	return(j);
}

BTGE_API int BTGE_VoxelDungeon_PickChunkEdgeDungeonCommon(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk0, BTGE_VoxelChunk *chk1,
	int edge0, int edge1)
{
	int na[16];
	BTGE_VoxelDungeonInfo *dung0, *dung1;
	BTGE_VoxelTerrGen *terr;
	int i, j, k, n;

	if(!chk0 || !chk1)
		return(VOX_DTY_UNASSIGNED);
	
	if((chk0->dungeon_type==VOX_DTY_UNASSIGNED) ||
		(chk0->dungeon_type==VOX_DTY_NONE) ||
		(chk1->dungeon_type==VOX_DTY_UNASSIGNED) ||
		(chk1->dungeon_type==VOX_DTY_NONE))
			return(VOX_DTY_UNASSIGNED);

	terr=world->vox_terr;

	dung0=btge_vox_dungeon_info[chk0->dungeon_type];
	dung1=btge_vox_dungeon_info[chk1->dungeon_type];

	n=0;
	for(i=0; i<dung0->allow_n_edge[edge0]; i++)
		for(j=0; j<dung1->allow_n_edge[edge1]; j++)
	{
		if(dung0->allow_edge[edge0][i]==dung1->allow_edge[edge1][j])
		{
			na[n++]=dung1->allow_edge[edge1][j];
//			return(dung1->allow_edge[edge1][j]);
		}
	}
	if(n>0)
	{
		if(n==1)return(na[0]);
		i=BTGE_Noise_Rand(terr->noise3)%n;
		return(na[i]);
	}

	return(VOX_DTY_UNASSIGNED);
}

BTGE_API int BTGE_VoxelDungeon_PickChunkBecomeDungeon(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelChunk *chk_nx, *chk_px;
	BTGE_VoxelChunk *chk_ny, *chk_py;
	BTGE_VoxelChunk *chk_nz, *chk_pz;
	int x, y, z;
	int p0, p1, p2, p3;

	x=chk->ox;
	y=chk->oy;
	z=chk->oz;

	chk_nx=BTGE_Voxel_GetWorldChunkFlags(world, x-16, y, z, 0);
	chk_px=BTGE_Voxel_GetWorldChunkFlags(world, x+16, y, z, 0);
	chk_ny=BTGE_Voxel_GetWorldChunkFlags(world, x, y-16, z, 0);
	chk_py=BTGE_Voxel_GetWorldChunkFlags(world, x, y+16, z, 0);
	chk_nz=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z-16, 0);
	chk_pz=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z+16, 0);

	//vertical gets priority
	p0=BTGE_VoxelDungeon_PickChunkEdgeDungeonRandom(
		world, chk_nz, VOX_DEDGE_PZ);
	if(p0)return(p0);
	p1=BTGE_VoxelDungeon_PickChunkEdgeDungeonRandom(
		world, chk_pz, VOX_DEDGE_NZ);
	if(p1)return(p1);
	
	//look for a commonly agreed on corner
	p0=BTGE_VoxelDungeon_PickChunkEdgeDungeonCommon(world,
		chk_nx, chk_ny, VOX_DEDGE_PX, VOX_DEDGE_PY);
	p1=BTGE_VoxelDungeon_PickChunkEdgeDungeonCommon(world,
		chk_nx, chk_py, VOX_DEDGE_PX, VOX_DEDGE_NY);
	p2=BTGE_VoxelDungeon_PickChunkEdgeDungeonCommon(world,
		chk_px, chk_ny, VOX_DEDGE_NX, VOX_DEDGE_PY);
	p3=BTGE_VoxelDungeon_PickChunkEdgeDungeonCommon(world,
		chk_px, chk_py, VOX_DEDGE_NX, VOX_DEDGE_NY);

	//all agree
	if(p0 && (p0==p1) && (p1==p2) && (p2==p3))
		return(p0);

	//majority vote
	if(p0 &&
		(((p0==p1) && ((p0==p2) || (p0==p3))) ||
		((p0==p2) && ((p0==p1) || (p0==p3))) ||
		((p0==p3) && ((p0==p1) || (p0==p2)))))
			return(p0);
	if(p1 &&
		(((p1==p0) && ((p1==p2) || (p1==p3))) ||
		((p1==p2) && ((p1==p0) || (p1==p3))) ||
		((p1==p3) && ((p1==p0) || (p1==p2)))))
			return(p1);
	if(p2 &&
		(((p2==p1) && ((p2==p0) || (p2==p3))) ||
		((p2==p0) && ((p2==p1) || (p2==p3))) ||
		((p2==p3) && ((p2==p1) || (p2==p0)))))
			return(p2);
	if(p3 &&
		(((p3==p1) && ((p3==p2) || (p3==p0))) ||
		((p3==p2) && ((p3==p1) || (p3==p0))) ||
		((p3==p0) && ((p3==p1) || (p3==p2)))))
			return(p3);
	
	//pair vote
	if(p0 && ((p0==p1) || (p0==p2) || (p0==p3)))
		return(p0);
	if(p1 && ((p1==p0) || (p1==p2) || (p1==p3)))
		return(p1);
	if(p2 && ((p2==p1) || (p2==p0) || (p2==p3)))
		return(p2);
	if(p3 && ((p3==p1) || (p3==p2) || (p3==p0)))
		return(p3);

	//any vote
	if(p0)	{ return(p0); }
	if(p1)	{ return(p1); }
	if(p2)	{ return(p2); }
	if(p3)	{ return(p3); }

	//pick randomly
	p0=BTGE_VoxelDungeon_PickChunkEdgeDungeonRandom(
		world, chk_nx, VOX_DEDGE_PX);
	p1=BTGE_VoxelDungeon_PickChunkEdgeDungeonRandom(
		world, chk_px, VOX_DEDGE_NX);
	p2=BTGE_VoxelDungeon_PickChunkEdgeDungeonRandom(
		world, chk_ny, VOX_DEDGE_PY);
	p3=BTGE_VoxelDungeon_PickChunkEdgeDungeonRandom(
		world, chk_py, VOX_DEDGE_NY);

	//all agree
	if(p0 && (p0==p1) && (p1==p2) && (p2==p3))
		return(p0);

	//majority vote
	if(p0 &&
		(((p0==p1) && ((p0==p2) || (p0==p3))) ||
		((p0==p2) && ((p0==p1) || (p0==p3))) ||
		((p0==p3) && ((p0==p1) || (p0==p2)))))
			return(p0);
	if(p1 &&
		(((p1==p0) && ((p1==p2) || (p1==p3))) ||
		((p1==p2) && ((p1==p0) || (p1==p3))) ||
		((p1==p3) && ((p1==p0) || (p1==p2)))))
			return(p1);
	if(p2 &&
		(((p2==p1) && ((p2==p0) || (p2==p3))) ||
		((p2==p0) && ((p2==p1) || (p2==p3))) ||
		((p2==p3) && ((p2==p1) || (p2==p0)))))
			return(p2);
	if(p3 &&
		(((p3==p1) && ((p3==p2) || (p3==p0))) ||
		((p3==p2) && ((p3==p1) || (p3==p0))) ||
		((p3==p0) && ((p3==p1) || (p3==p2)))))
			return(p3);
	
	//pair vote
	if(p0 && ((p0==p1) || (p0==p2) || (p0==p3)))
		return(p0);
	if(p1 && ((p1==p0) || (p1==p2) || (p1==p3)))
		return(p1);
	if(p2 && ((p2==p1) || (p2==p0) || (p2==p3)))
		return(p2);
	if(p3 && ((p3==p1) || (p3==p2) || (p3==p0)))
		return(p3);

	//any vote
	if(p0)	{ return(p0); }
	if(p1)	{ return(p1); }
	if(p2)	{ return(p2); }
	if(p3)	{ return(p3); }
	
	//guess nothing then
	return(VOX_DTY_UNASSIGNED);
}

BTGE_API int BTGE_VoxelDungeon_PickChunkBecomeDungeonChance(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelDungeonInfo *dung;
	BTGE_VoxelTerrGen *terr;
	float f, g;
	int p, h;
	int i;

	terr=world->vox_terr;

	//try to grow existing dungeon
	p=BTGE_VoxelDungeon_PickChunkBecomeDungeon(world, chk);
	if(p)return(p);

	if(chk->oz>48)
		return(VOX_DTY_UNASSIGNED);

	g=1.0-(fabs(chk->oz-32)/64.0);
	if(g<0.0)g=0.0;
//	g=1.0/(g*g);
	g=g*g;

	f=BTGE_Noise_Random(terr->noise3);
//	f=BTGE_Noise_Random(terr->noise3)*g;
//	if(f<0.001)
	if(f<(0.001*g))
	{
		h=BTGE_Noise_Rand(terr->noise3);
		for(i=0; i<16; i++)
		{
			p=(h>>16)&255;
			dung=btge_vox_dungeon_info[p];
			if(dung)break;
			h=h*65521;
		}
		if(!dung) { return(VOX_DTY_UNASSIGNED); }

//		p=256*BTGE_Noise_Random(terr->noise3);
//		dung=btge_vox_dungeon_info[p];
//		if(!dung) { return(VOX_DTY_UNASSIGNED); }

		if(dung->allow_n_biome)
		{
			for(i=0; i<dung->allow_n_biome; i++)
				if(chk->biome==dung->allow_biome[i])
					break;
			if(i>=dung->allow_n_biome)
				{ return(VOX_DTY_UNASSIGNED); }
		}
		if(dung->deny_n_biome)
		{
			for(i=0; i<dung->deny_n_biome; i++)
				if(chk->biome==dung->deny_biome[i])
					break;
			if(i<dung->deny_n_biome)
				{ return(VOX_DTY_UNASSIGNED); }
		}

		return(p);
	}
	
	return(VOX_DTY_UNASSIGNED);
}

BTGE_API bool BTGE_Voxel_GenerateFillDungeonRange(
	BTGE_BrushWorld *world, int ox, int oy, int xs, int ys)
{
	BTGE_VoxelDungeonInfo *dung;
	BTGE_VoxelTerrGen *terr;
	BTGE_VoxelChunk *chk, *chk1;
	int x, y, z;
	int i, j, k;

	terr=world->vox_terr;
	
	for(x=ox; x<(ox+xs); x+=16)
		for(y=oy; y<(oy+ys); y+=16)
	{
		for(z=16; z<112; z+=16)
		{
			chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 0);
			if(!chk)continue;
			if(chk->dungeon_type)
				continue;

//			i=BTGE_VoxelDungeon_PickChunkBecomeDungeon(world, chk);
			i=BTGE_VoxelDungeon_PickChunkBecomeDungeonChance(world, chk);
			if(i)
			{
				dung=btge_vox_dungeon_info[i];
				if(z<dung->min_z)
					continue;
				if(z>dung->max_z)
					continue;
				
				printf("BTGE_Voxel_GenerateFillDungeonRange: %d %d %d %s\n",
					x, y, z, dung->name);
				
				BTGE_VoxelDungeon_SpawnWorldDungeon(world, dung,
					chk->ox, chk->oy, chk->oz, VOX_GFL_REBUILD);
			}
		}

		for(z=96; z>=16; z-=16)
		{
			chk=BTGE_Voxel_GetWorldChunkFlags(world, x, y, z, 0);
			if(!chk)continue;
			if(chk->dungeon_type)
				continue;

//			i=BTGE_VoxelDungeon_PickChunkBecomeDungeon(world, chk);
			i=BTGE_VoxelDungeon_PickChunkBecomeDungeonChance(world, chk);
//			if(i)
			if(i>0)
			{
				dung=btge_vox_dungeon_info[i];
				if(!dung)
					continue;
				
				if(z<dung->min_z)
					continue;
				if(z>dung->max_z)
					continue;
				
				printf("BTGE_Voxel_GenerateFillDungeonRange: %d %d %d %s\n",
					x, y, z, dung->name);
				
				BTGE_VoxelDungeon_SpawnWorldDungeon(world, dung,
					chk->ox, chk->oy, chk->oz, VOX_GFL_REBUILD);
			}
		}
	}
	return(false);
}
