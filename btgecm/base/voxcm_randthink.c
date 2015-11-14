#include <btgecm.h>

BTGE_API void BTGE_Voxel_RandomThink_Grass(
	BTGE_BrushWorld *world, BTGE_VoxelData vox,
	int x, int y, int z, int fl)
{
	BTGE_VoxelData vox1, vox2;
	char *s;
	int x1, y1, z1;
	int i, j;

//	printf("BTGE_Voxel_RandomThink_Grass: Grass At %d %d %d\n",
//		x, y, z);
	
	i=rand()&63;
	x1=x; y1=y; z1=z;
	if(i&1)x1++;
	if(i&2)x1--;
	if(i&4)y1++;
	if(i&8)y1--;
	if(i&16)z1++;
	if(i&32)z1--;
	
	if((x1==x) && (y1==y) && (z1==z))
	{
//		printf("BTGE_Voxel_RandomThink_Grass: Same Spot\n");
		vox1=BTGE_Voxel_GetWorldVoxelData(world, x, y, z+1);
		if(BTGE_Voxel_VoxelDataOpaqueP(vox1))
		{
			printf("BTGE_Voxel_RandomThink_Grass: Convert To Dirt\n");
			vox.type=(vox.type&(~VOX_TY_MASK))|VOX_TY_DIRT;
			BTGE_Voxel_SetWorldVoxelData(world, x, y, z, vox);
			return;
		}

		if((vox1.type&VOX_TY_MASK)==VOX_TY_AIR)
		{
//			switch(rand()&255)
			switch(rand()&4093)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_TALL_GRASS;
				BTGE_Voxel_SetWorldVoxelData(world, x, y, z+1, vox1);
				printf("BTGE_Voxel_RandomThink_Grass: "
						"Tall Grass %d %d %d\n",
					x1, y1, z1);
				break;
			case 4:
				vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_FLOWER_YEL;
				BTGE_Voxel_SetWorldVoxelData(world, x, y, z+1, vox1);
				printf("BTGE_Voxel_RandomThink_Grass: "
						"Yellow Flower %d %d %d\n",
					x1, y1, z1);
				break;
			case 5:
				vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_FLOWER_RED;
				BTGE_Voxel_SetWorldVoxelData(world, x, y, z+1, vox1);
				printf("BTGE_Voxel_RandomThink_Grass: "
						"Red Flower %d %d %d\n",
					x1, y1, z1);
				break;
			default:
				break;
			}
		}

		return;
	}
	
	vox1=BTGE_Voxel_GetWorldVoxelData(world, x1, y1, z1);
	if((vox1.type&VOX_TY_MASK)==VOX_TY_AIR)
	{
		z1--;
		vox1=BTGE_Voxel_GetWorldVoxelData(world, x1, y1, z1);
//		printf("BTGE_Voxel_RandomThink_Grass: Step Down\n");
	}

	if((vox1.type&VOX_TY_MASK)==VOX_TY_DIRT)
	{
		vox2=BTGE_Voxel_GetWorldVoxelData(world, x1, y1, z1+1);
//		if((vox2.type&VOX_TY_MASK)==VOX_TY_AIR)
		if(!BTGE_Voxel_VoxelDataOpaqueP(vox2))
		{
			vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_GRASS;
			BTGE_Voxel_SetWorldVoxelData(world, x1, y1, z1, vox1);
			
			printf("BTGE_Voxel_RandomThink_Grass: Grassify %d %d %d\n",
				x1, y1, z1);
		}else
		{
			s=BTGE_Voxel_GetNameForVoxelIndex(vox2.type&VOX_TY_MASK);
			if(!s)s="(invalid)";
//			printf("BTGE_Voxel_RandomThink_Grass: "
//				"Target Dirt Covered By %s\n", s);
		}
	}else
	{
//		s=BTGE_Voxel_GetNameForVoxelIndex(vox1.type&VOX_TY_MASK);
//		if(!s)s="(invalid)";
//		printf("BTGE_Voxel_RandomThink_Grass: Hit %s\n", s);
	}
}

BTGE_API void BTGE_Voxel_RandomThink_Mycelium(
	BTGE_BrushWorld *world, BTGE_VoxelData vox,
	int x, int y, int z, int fl)
{
	BTGE_VoxelData vox1, vox2;
	char *s;
	int x1, y1, z1;
	int i, j;

//	printf("BTGE_Voxel_RandomThink_Mycelium: Grass At %d %d %d\n",
//		x, y, z);
	
	i=rand()&63;
	x1=x; y1=y; z1=z;
	if(i&1)x1++;
	if(i&2)x1--;
	if(i&4)y1++;
	if(i&8)y1--;
	if(i&16)z1++;
	if(i&32)z1--;
	
	if((x1==x) && (y1==y) && (z1==z))
	{
//		printf("BTGE_Voxel_RandomThink_Mycelium: Same Spot\n");
		vox1=BTGE_Voxel_GetWorldVoxelData(world, x, y, z+1);
		if(BTGE_Voxel_VoxelDataOpaqueP(vox1))
		{
			printf("BTGE_Voxel_RandomThink_Mycelium: Convert To Dirt\n");
			vox.type=(vox.type&(~VOX_TY_MASK))|VOX_TY_DIRT;
			BTGE_Voxel_SetWorldVoxelData(world, x, y, z, vox);
			return;
		}

		if((vox1.type&VOX_TY_MASK)==VOX_TY_AIR)
		{
//			switch(rand()&255)
			switch(rand()&4093)
			{
			case 0:
				vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_SHROOM_BROWN;
				BTGE_Voxel_SetWorldVoxelData(world, x, y, z+1, vox1);
				printf("BTGE_Voxel_RandomThink_Mycelium: "
						"Brown Mushroom %d %d %d\n",
					x1, y1, z1);
				break;
			case 1:
				vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_SHROOM_RED;
				BTGE_Voxel_SetWorldVoxelData(world, x, y, z+1, vox1);
				printf("BTGE_Voxel_RandomThink_Mycelium: "
						"Red Mushroom %d %d %d\n",
					x1, y1, z1);
				break;
			default:
				break;
			}
		}

		return;
	}
	
	vox1=BTGE_Voxel_GetWorldVoxelData(world, x1, y1, z1);
	if((vox1.type&VOX_TY_MASK)==VOX_TY_AIR)
	{
		z1--;
		vox1=BTGE_Voxel_GetWorldVoxelData(world, x1, y1, z1);
//		printf("BTGE_Voxel_RandomThink_Mycelium: Step Down\n");
	}

//	if((vox1.type&VOX_TY_MASK)==VOX_TY_DIRT)
	if(((vox1.type&VOX_TY_MASK)==VOX_TY_DIRT) ||
		((vox1.type&VOX_TY_MASK)==VOX_TY_GRASS))
	{
		vox2=BTGE_Voxel_GetWorldVoxelData(world, x1, y1, z1+1);
//		if((vox2.type&VOX_TY_MASK)==VOX_TY_AIR)
		if(!BTGE_Voxel_VoxelDataOpaqueP(vox2))
		{
			vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_MYCELIUM;
			BTGE_Voxel_SetWorldVoxelData(world, x1, y1, z1, vox1);
			
			printf("BTGE_Voxel_RandomThink_Mycelium: Fungify %d %d %d\n",
				x1, y1, z1);
		}else
		{
//			s=BTGE_Voxel_GetNameForVoxelIndex(vox2.type&VOX_TY_MASK);
//			if(!s)s="(invalid)";
//			printf("BTGE_Voxel_RandomThink_Mycelium: "
//				"Target Dirt Covered By %s\n", s);
		}
	}else
	{
//		s=BTGE_Voxel_GetNameForVoxelIndex(vox1.type&VOX_TY_MASK);
//		if(!s)s="(invalid)";
//		printf("BTGE_Voxel_RandomThink_Mycelium: Hit %s\n", s);
	}
}

BTGE_API void BTGE_Voxel_RandomThink_Cactus(
	BTGE_BrushWorld *world, BTGE_VoxelData vox,
	int x, int y, int z, int fl)
{
	BTGE_VoxelData vox1, vox2, vox3;

	vox1=BTGE_Voxel_GetWorldVoxelData(world, x, y, z+1);
	if((vox1.type&VOX_TY_MASK)==VOX_TY_AIR)
	{
		vox2=BTGE_Voxel_GetWorldVoxelData(world, x, y, z-1);
		vox3=BTGE_Voxel_GetWorldVoxelData(world, x, y, z-2);

		if(((vox2.type&VOX_TY_MASK)!=VOX_TY_CACTUS) ||
			((vox3.type&VOX_TY_MASK)!=VOX_TY_CACTUS))
		{
			vox1.type=(vox1.type&(~VOX_TY_MASK))|VOX_TY_CACTUS;
			BTGE_Voxel_SetWorldVoxelData(world, x, y, z+1, vox1);
			printf("BTGE_Voxel_RandomThink_Cactus: Grow %d %d %d\n",
				x, y, z+1);
		}
	}
}
