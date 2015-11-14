#include <btgecm.h>

BTGE_API void BTGE_Voxel_EmitBlock_Stairs(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	float mid[3], min2[3], max2[3];
	float rgba[4], nv[4];
	BTGE_VoxelInfo *inf;
	float f, g, h;
	int tex, fl2;
	int i, j;

	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	
	h=(mid[2]+max[2])*0.5;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	switch(vox->aux&3)
	{
	case VOX_AXFL_STAIRS_EAST:
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=mid[0]; max2[1]=max[1]; max2[2]=mid[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		min2[0]=mid[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		break;
	case VOX_AXFL_STAIRS_WEST:
		min2[0]=mid[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=mid[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=mid[0]; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		break;
	case VOX_AXFL_STAIRS_SOUTH:
		min2[0]=min[0]; min2[1]=mid[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=mid[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=mid[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		break;
	case VOX_AXFL_STAIRS_NORTH:
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=mid[1]; max2[2]=mid[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		min2[0]=min[0]; min2[1]=mid[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
		break;
	default:
		break;
	}
}

BTGE_API int BTGE_VoxelStairs_GetWorldVoxelBBoxList(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl,
	float *mins, float *maxs, int max)
{
//	float mid[3], min2[3], max2[3];
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	float f, g, h;
	int i, j;

//	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	
	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
	if(!vox || !vox->type)
		return(0);
	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
	if(!inf)return(0);

	mins[0]=x*32+0;
	mins[1]=y*32+0;
	mins[2]=z*32+0;
	maxs[0]=x*32+32;
	maxs[1]=y*32+32;
	maxs[2]=z*32+16;

	mins[3]=x*32+0;
	mins[4]=y*32+0;
	mins[5]=z*32+0;
	maxs[3]=x*32+32;
	maxs[4]=y*32+32;
	maxs[5]=z*32+32;

	switch(vox->aux&3)
	{
	case VOX_AXFL_STAIRS_EAST:
		mins[3]=x*32+16;
		break;
	case VOX_AXFL_STAIRS_WEST:
		maxs[3]=x*32+16;
		break;
	case VOX_AXFL_STAIRS_SOUTH:
		maxs[4]=y*32+16;
		break;
	case VOX_AXFL_STAIRS_NORTH:
		mins[4]=y*32+16;
		break;
	}

	return(2);
}


BTGE_API void BTGE_Voxel_EmitBlock_Ladder(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	float mid[3], min2[3], max2[3];
	float rgba[4], nv[4];
	BTGE_VoxelInfo *inf;
	float f, g, h;
	int tex, fl2;
	int i, j;

	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	
	h=(mid[2]+max[2])*0.5;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	switch(vox->aux&15)
	{
	case VOX_AXFL_LADDER_EAST:
		min2[0]=min[0]+0; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=min[0]+4; max2[1]=max[1]; max2[2]=max[2];
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	case VOX_AXFL_LADDER_WEST:
		min2[0]=max[0]-4; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]-0; max2[1]=max[1]; max2[2]=max[2];
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	case VOX_AXFL_LADDER_DOWN:
	case VOX_AXFL_LADDER_UP:
	case VOX_AXFL_LADDER_SOUTH:
		min2[0]=min[0]; min2[1]=max[1]-4; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]-0; max2[2]=max[2];
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	case VOX_AXFL_LADDER_NORTH:
		min2[0]=min[0]; min2[1]=min[1]+0; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=min[1]+4; max2[2]=max[2];
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	default:
		break;
	}
}

BTGE_API int BTGE_VoxelLadder_GetWorldVoxelBBoxList(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl,
	float *mins, float *maxs, int max)
{
//	float mid[3], min2[3], max2[3];
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	float f, g, h;
	int i, j;

//	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	
	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
	if(!vox || !vox->type)
		return(0);
	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
	if(!inf)return(0);

	mins[0]=x*32+0;
	mins[1]=y*32+0;
	mins[2]=z*32+0;
	maxs[0]=x*32+32;
	maxs[1]=y*32+32;
	maxs[2]=z*32+32;

	switch(vox->aux&15)
	{
	case VOX_AXFL_LADDER_EAST:
		maxs[0]=x*32+8;
		break;
	case VOX_AXFL_LADDER_WEST:
		mins[0]=x*32+24;
		break;
	case VOX_AXFL_LADDER_DOWN:
	case VOX_AXFL_LADDER_UP:
	case VOX_AXFL_LADDER_SOUTH:
		mins[1]=y*32+24;
		break;
	case VOX_AXFL_LADDER_NORTH:
		maxs[1]=y*32+8;
		break;
	}

	return(1);
}

BTGE_API void BTGE_VoxelDoor_EmitBlock(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	float mid[3], min2[3], max2[3];
	float rgba[4], nv[4];
	BTGE_VoxelInfo *inf;
	float f, g, h;
	int tex, fl2;
	int i, j;

	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	
	h=(mid[2]+max[2])*0.5;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	i=vox->aux&3;
	if(vox->aux&4)
	{
		switch(i)
		{
		case VOX_AXFL_DOOR_EAST: i=VOX_AXFL_DOOR_SOUTH; break;
		case VOX_AXFL_DOOR_WEST: i=VOX_AXFL_DOOR_NORTH; break;
		case VOX_AXFL_DOOR_SOUTH: i=VOX_AXFL_DOOR_EAST; break;
		case VOX_AXFL_DOOR_NORTH: i=VOX_AXFL_DOOR_WEST; break;
		}
	}

	switch(i)
	{
	case VOX_AXFL_DOOR_EAST:
		min2[0]=min[0]+0; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=min[0]+4; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	case VOX_AXFL_DOOR_WEST:
		min2[0]=max[0]-4; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]-0; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	case VOX_AXFL_DOOR_SOUTH:
		min2[0]=min[0]; min2[1]=max[1]-4; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]-0; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	case VOX_AXFL_DOOR_NORTH:
		min2[0]=min[0]; min2[1]=min[1]+0; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=min[1]+4; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, 1.0, 1.0);
		break;
	default:
		break;
	}
}

BTGE_API int BTGE_VoxelDoor_GetWorldVoxelBBoxList(
	BTGE_BrushWorld *world,
	int x, int y, int z, int fl,
	float *mins, float *maxs, int max)
{
//	float mid[3], min2[3], max2[3];
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	float f, g, h;
	int i, j;

//	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	
	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
	if(!vox || !vox->type)
		return(0);
	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
	if(!inf)return(0);

	mins[0]=x*32+4;
	mins[1]=y*32+4;
	mins[2]=z*32+4;
	maxs[0]=x*32+28;
	maxs[1]=y*32+28;
	maxs[2]=z*32+28;

	i=vox->aux&3;
	if(vox->aux&4)
	{
		switch(i)
		{
		case VOX_AXFL_DOOR_EAST: i=VOX_AXFL_DOOR_SOUTH; break;
		case VOX_AXFL_DOOR_WEST: i=VOX_AXFL_DOOR_NORTH; break;
		case VOX_AXFL_DOOR_SOUTH: i=VOX_AXFL_DOOR_EAST; break;
		case VOX_AXFL_DOOR_NORTH: i=VOX_AXFL_DOOR_WEST; break;
		}
	}

	switch(i)
	{
	case VOX_AXFL_DOOR_EAST:
		maxs[0]=x*32+8;
		break;
	case VOX_AXFL_DOOR_WEST:
		mins[0]=x*32+24;
		break;
	case VOX_AXFL_DOOR_SOUTH:
		mins[1]=y*32+24;
		break;
	case VOX_AXFL_DOOR_NORTH:
		maxs[1]=y*32+8;
		break;
	}

	return(1);
}

BTGE_API int BTGE_VoxelDoor_SmallDamageEvent(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z, double *org,
	float radius, float damage, float dmg)
{
	BTGE_VoxelData *vox1;

	if(vox->aux&16)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		return(0);
	}

	vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
	vox->aux^=4;
	vox->aux|=16;
	vox->delay=1;

	vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z+((vox->aux&8)?(-1):1));
	if(vox1->type==vox->type)
	{
		vox1->aux=(vox1->aux&(~4))|(vox->aux&4);
		vox1->aux|=16;
		vox1->delay=1;
	}
	
	return(0);
}

BTGE_API void BTGE_VoxelDoor_UpdateVoxel(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if(vox->aux&16)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		vox->aux&=~16;
	}
}
