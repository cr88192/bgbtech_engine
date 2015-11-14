#include <btgecm.h>

BTGE_API bool BTGE_VoxelPower_VoxelCanPoweredP(
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1)
{
	if(!vox)return(false);
	if(!vox->type)return(false);

	if(!vox1)return(false);
	if(!vox1->type)return(false);

	if(	((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE))
	{
		if(BTGE_Voxel_VoxelOpaqueP(vox1))
		{
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM)
				return(false);
			return(true);
		}

		if(	((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_RELAY0) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_RELAY1) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_AND) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_OR) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_XOR) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_NAND) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_NOR) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_XNOR) ||

			((vox1->type&VOX_TY_MASK)==VOX_TY_STONE_BUTTON) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_WOOD_BUTTON) ||
			((vox1->type&VOX_TY_MASK)==VOX_TY_LEVER)
			)
		{
			return(true);
		}
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_WIRE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE))
	{
		if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_WIRE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH0)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH1)
			return(true);

		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_WIRE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH0)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH1)
			return(true);

		return(false);
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_WIRE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE))
	{
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_WIRE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH0)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH1)
			return(true);

		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_WIRE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH0)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH1)
			return(true);

		return(false);
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_WIRE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE))
	{
		if((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_WIRE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH0)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH1)
			return(true);

		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_WIRE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH0)
			return(true);
		if((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH1)
			return(true);

		return(false);
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_WIRE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE))
	{
		switch(vox1->type&VOX_TY_MASK)
		{
		case VOX_TY_REDSTONE_WIRE:		case VOX_TY_GREENSTONE_WIRE:
		case VOX_TY_BLUESTONE_WIRE:		case VOX_TY_GOLDSTONE_WIRE:
		case VOX_TY_REDSTONE_PIPE:		case VOX_TY_GREENSTONE_PIPE:
		case VOX_TY_BLUESTONE_PIPE:		case VOX_TY_GOLDSTONE_PIPE:
		case VOX_TY_REDSTONE_TORCH0:	case VOX_TY_GREENSTONE_TORCH0:
		case VOX_TY_BLUESTONE_TORCH0:	case VOX_TY_GOLDSTONE_TORCH0:
		case VOX_TY_REDSTONE_TORCH1:	case VOX_TY_GREENSTONE_TORCH1:
		case VOX_TY_BLUESTONE_TORCH1:	case VOX_TY_GOLDSTONE_TORCH1:
			return(true);
			break;
		default:
			break;
		}

		return(false);
	}

	return(false);
//	return((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM);
}

BTGE_API bool BTGE_VoxelPower_CheckVoxelCanPowerP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
	return(BTGE_VoxelPower_VoxelCanPoweredP(vox, vox1));
}

BTGE_API void BTGE_Voxel_EmitBlock_RedstoneWire(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};
	float pts[4*3];
	float rgba[4], nv[4];
	BTGE_VoxelInfo *inf;
	float f, g;
	int tex, fl2;
	int i, j;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	fl2=0;
	if(BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x-1, y, z))
		fl2|=VOX_FL_NX;
	if(BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x+1, y, z))
		fl2|=VOX_FL_PX;
	if(BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x, y-1, z))
		fl2|=VOX_FL_NY;
	if(BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x, y+1, z))
		fl2|=VOX_FL_PY;

	if((fl2==VOX_FL_NX) || (fl2==VOX_FL_PX) || (fl2==(VOX_FL_NX|VOX_FL_PX)))
		tex=inf->img_side;
	if((fl2==VOX_FL_NY) || (fl2==VOX_FL_PY) || (fl2==(VOX_FL_NY|VOX_FL_PY)))
		tex=inf->img_bot;

	for(i=0; i<4; i++)
	{
		pts[i*3+0]=(i&1)?max[0]:min[0];
		pts[i*3+1]=(i&2)?max[1]:min[1];
		pts[i*3+2]=min[2]+1;
	}

	f=0.5+((vox->aux&0x0F)/15.0)*0.5;
//	f=1;

	nv[0]=0; nv[1]=0; nv[2]=1; nv[3]=min[2]+1;
	rgba[0]=f; rgba[1]=f; rgba[2]=f; rgba[3]=1; 

	BTGE_Voxel_EmitFace(chk, vox,
		pts+1*3, sta+6,
		pts+3*3, sta+4,
		pts+2*3, sta+2,
		pts+0*3, sta+0,
		nv, rgba,
		tex);
}

BTGE_API void BTGE_Voxel_EmitBlock_RedstoneRelay(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
//	static float sta[8]= { 1, 0, 1, 1, 0, 1, 0, 0};
	static float sta[8]= { 0, 0, 0, 1, 1, 1, 1, 0};
	float pts[4*3];
	float rgba[4], nv[4], min2[4], max2[4];
	BTGE_VoxelInfo *inf;
	float f, g;
	int tex, fl2, rot;
	int i, j;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	for(i=0; i<4; i++)
	{
		pts[i*3+0]=(i&1)?max[0]:min[0];
		pts[i*3+1]=(i&2)?max[1]:min[1];
		pts[i*3+2]=min[2]+16;
	}

//	f=0.5+((vox->aux&0x0F)/15.0)*0.5;
	f=1;

//	rot=vox->aux&3;
	switch(vox->aux&3)
	{
	case VOX_AXFL_RELAY_NORTH: rot=0; break;
	case VOX_AXFL_RELAY_EAST: rot=3; break;
	case VOX_AXFL_RELAY_SOUTH: rot=2; break;
	case VOX_AXFL_RELAY_WEST: rot=1; break;
	}

	min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
	max2[0]=max[0]; max2[1]=max[1]; max2[2]=min[2]+15;
	BTGE_Voxel_EmitMaterialCube(world, chk, vox,
		min2, max2, VOX_FL_ALL_CUBE);

	nv[0]=0; nv[1]=0; nv[2]=1; nv[3]=min[2]+1;
	rgba[0]=1; rgba[1]=1; rgba[2]=1; rgba[3]=1; 

	BTGE_Voxel_EmitSolidFace(chk, vox,
		pts+1*3, sta+((rot+3)&3)*2,
		pts+3*3, sta+((rot+2)&3)*2,
		pts+2*3, sta+((rot+1)&3)*2,
		pts+0*3, sta+((rot+0)&3)*2,
		nv, rgba,
		tex);
}

BTGE_API void BTGE_Voxel_EmitBlock_Torch(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	BTGE_Voxel_EmitBlock_RedstoneTorch(world, chk, vox,
		min, max, flag, x, y, z);
}

BTGE_API void BTGE_Voxel_EmitBlock_RedstoneTorch(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static float sta[8]= { 0, 0, 0, 1, 1, 1, 1, 0};
	float pts[4*3];
	float rgba[4], nv[4], mid[4];
	BTGE_VoxelInfo *inf;
	float f, g, xc, yc;
	int tex, fl2;
	int i, j;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);

	if(inf->vlight)
	{
		i=inf->vlight|0xF0;
		j=(inf->vlight>>4)&15;
//		BTGE_Voxel_GetVLightColor(world, i, rgba);
		BTGE_Voxel_GetVLightBaseColor(world, i, rgba);
		BTGE_Voxel_EmitLightStyle(world, chk,
			mid[0], mid[1], mid[2],
			rgba[0], rgba[1], rgba[2], j*32,
			0, inf->vlight_style);
//			BTGE_LFL_NOSHADOW);
	}

//	f=0.5+((vox->aux&0x0F)/15.0)*0.5;
	f=1;

	nv[0]=0; nv[1]=0; nv[2]=1; nv[3]=min[2]+1;
	rgba[0]=f; rgba[1]=f; rgba[2]=f; rgba[3]=1; 

	switch(vox->aux&15)
	{
	case 0:		xc=  0; yc=  0; break;	//standing
	case 1:		xc=-16; yc=  0; break;	//points east (+X)
	case 2:		xc= 16; yc=  0; break;	//points west (-X)
	case 3:		xc=  0; yc= 16; break;	//points south (-Y)
	case 4:		xc=  0; yc=-16; break;	//points north (+Y)
	case 5:		xc=  0; yc=  0; break;	//standing
	default:	xc=  0; yc=  0; break;
	}

	for(i=0; i<8; i++)
	{
		for(j=0; j<4; j++)
		{
			f=(j&1)?16:-16;
			pts[j*3+0]=mid[0]+(f*cos(i*M_PI/4.0));
			pts[j*3+1]=mid[1]+(f*sin(i*M_PI/4.0));
			pts[j*3+2]=mid[2]+((j&2)?16:-16);
		}
		
		pts[0*3+0]+=xc;	pts[0*3+1]+=yc;
		pts[1*3+0]+=xc;	pts[1*3+1]+=yc;

		BTGE_Voxel_EmitFace(chk, vox,
			pts+1*3, sta+6,
			pts+3*3, sta+4,
			pts+2*3, sta+2,
			pts+0*3, sta+0,
			nv, rgba,
			tex);

		BTGE_Voxel_EmitFace(chk, vox,
			pts+0*3, sta+0,
			pts+2*3, sta+2,
			pts+3*3, sta+4,
			pts+1*3, sta+6,
			nv, rgba,
			tex);
	}
}

BTGE_API void BTGE_Voxel_EmitBlock_Lever(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static float sta[8]= { 0, 0, 0, 1, 1, 1, 1, 0};
	float pts[4*3];
	float rgba[4], nv[4], mid[4];
	BTGE_VoxelInfo *inf;
	float f, g, xc, yc;
	int tex, fl2;
	int i, j;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);

//	f=0.5+((vox->aux&0x0F)/15.0)*0.5;
	f=1;

	nv[0]=0; nv[1]=0; nv[2]=1; nv[3]=min[2]+1;
	rgba[0]=f; rgba[1]=f; rgba[2]=f; rgba[3]=1; 

#if 0
	switch(vox->aux&15)
	{
	case 0:		xc=  0; yc=  0; break;	//standing
	case 1:		xc=-16; yc=  0; break;	//points east (+X)
	case 2:		xc= 16; yc=  0; break;	//points west (-X)
	case 3:		xc=  0; yc= 16; break;	//points south (-Y)
	case 4:		xc=  0; yc=-16; break;	//points north (+Y)
	case 5:		xc=  0; yc=  0; break;	//standing
	default:	xc=  0; yc=  0; break;
	}
#endif

	if(vox->aux&8)
	{
		xc=16;
		yc=16;
	}else
	{
		xc=0; yc=0;
	}

	for(i=0; i<8; i++)
	{
		for(j=0; j<4; j++)
		{
			f=(j&1)?16:-16;
			pts[j*3+0]=mid[0]+(f*cos(i*M_PI/4.0));
			pts[j*3+1]=mid[1]+(f*sin(i*M_PI/4.0));
			pts[j*3+2]=mid[2]+((j&2)?16:-16);
		}
		
		pts[2*3+0]+=xc;	pts[2*3+1]+=yc;
		pts[3*3+0]+=xc;	pts[3*3+1]+=yc;

		BTGE_Voxel_EmitFace(chk, vox,
			pts+1*3, sta+6,
			pts+3*3, sta+4,
			pts+2*3, sta+2,
			pts+0*3, sta+0,
			nv, rgba,
			tex);

		BTGE_Voxel_EmitFace(chk, vox,
			pts+0*3, sta+0,
			pts+2*3, sta+2,
			pts+3*3, sta+4,
			pts+1*3, sta+6,
			nv, rgba,
			tex);
	}
}

BTGE_API void BTGE_Voxel_EmitBlock_PressurePlate(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	float min2[4], max2[4];
	BTGE_VoxelInfo *inf;
	float f, g;
	int tex, fl2, rot;
	int i, j;

//	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
//	if(!inf)return;
//	tex=inf->img_top;

	f=(vox->aux&1)?2:4;

	min2[0]=min[0]+2; min2[1]=min[1]+2; min2[2]=min[2];
	max2[0]=max[0]-2; max2[1]=max[1]-2; max2[2]=min[2]+f;
	BTGE_Voxel_EmitMaterialCube(world, chk, vox,
		min2, max2, VOX_FL_ALL_CUBE);
}

BTGE_API void BTGE_Voxel_EmitBlock_RedstonePipe(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};
	float pts[4*3];
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

	fl2=0;
	if(!BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x-1, y, z))
		fl2|=VOX_FL_NX;
	if(!BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x+1, y, z))
		fl2|=VOX_FL_PX;
	if(!BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x, y-1, z))
		fl2|=VOX_FL_NY;
	if(!BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x, y+1, z))
		fl2|=VOX_FL_PY;
	if(!BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x, y, z-1))
		fl2|=VOX_FL_NZ;
	if(!BTGE_VoxelPower_CheckVoxelCanPowerP(world, chk, vox, x, y, z+1))
		fl2|=VOX_FL_PZ;

//	if(!fl2)fl2|=

	f=0.5+((vox->aux&0x0F)/15.0)*0.5;

	min2[0]=mid[0]-3; min2[1]=mid[1]-3; min2[2]=mid[2]-3;
	max2[0]=mid[0]+3; max2[1]=mid[1]+3; max2[2]=mid[2]+3;
//	BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//		min2, max2, VOX_FL_ALL_CUBE);
//	BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
//		min2, max2, VOX_FL_ALL_CUBE, 1, f);
	BTGE_Voxel_EmitMaterialColorCube(world, chk, vox,
		min2, max2, VOX_FL_ALL_CUBE, f, f, f);

	if(!(fl2&VOX_FL_NX))
	{
		min2[0]=min[0];		min2[1]=mid[1]-2;	min2[2]=mid[2]-2;
		max2[0]=mid[0];		max2[1]=mid[1]+2;	max2[2]=mid[2]+2;
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
//		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE, 1, f);
		BTGE_Voxel_EmitMaterialColorCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, f, f, f);
	}

	if(!(fl2&VOX_FL_PX))
	{
		min2[0]=mid[0];		min2[1]=mid[1]-2;	min2[2]=mid[2]-2;
		max2[0]=max[0];		max2[1]=mid[1]+2;	max2[2]=mid[2]+2;
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
//		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE, 1, f);
		BTGE_Voxel_EmitMaterialColorCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, f, f, f);
	}

	if(!(fl2&VOX_FL_NY))
	{
		min2[0]=mid[0]-2;	min2[1]=min[1];		min2[2]=mid[2]-2;
		max2[0]=mid[0]+2;	max2[1]=mid[1];		max2[2]=mid[2]+2;
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
//		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE, 1, f);
		BTGE_Voxel_EmitMaterialColorCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, f, f, f);
	}

	if(!(fl2&VOX_FL_PY))
	{
		min2[0]=mid[0]-2;	min2[1]=mid[1];		min2[2]=mid[2]-2;
		max2[0]=mid[0]+2;	max2[1]=max[1];		max2[2]=mid[2]+2;
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
//		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE, 1, f);
		BTGE_Voxel_EmitMaterialColorCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, f, f, f);
	}

	if(!(fl2&VOX_FL_NZ))
	{
		min2[0]=mid[0]-2;	min2[1]=mid[1]-2;		min2[2]=min[2];
		max2[0]=mid[0]+2;	max2[1]=mid[1]+2;		max2[2]=mid[2];
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
//		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE, 1, f);
		BTGE_Voxel_EmitMaterialColorCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, f, f, f);
	}

	if(!(fl2&VOX_FL_PZ))
	{
		min2[0]=mid[0]-2;	min2[1]=mid[1]-2;		min2[2]=mid[2];
		max2[0]=mid[0]+2;	max2[1]=mid[1]+2;		max2[2]=max[2];
//		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE);
//		BTGE_Voxel_EmitMaterialAlphaCube(world, chk, vox,
//			min2, max2, VOX_FL_ALL_CUBE, 1, f);
		BTGE_Voxel_EmitMaterialColorCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, f, f, f);
	}
}

#if 0
// BTGE_API int BTGE_VoxelPower_VoxelGetPower(
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1)
{
	return(BTGE_VoxelPower_WorldVoxelGetPower(
		NULL, NULL, vox, vox1, 0,0,0, 0,0,0));
}
#endif

BTGE_API int BTGE_VoxelPower_WorldVoxelGetPower(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1,
	int x0, int y0, int z0, int x1, int y1, int z1)
{
	BTGE_VoxelInfo *inf;
	int val;

	if(!vox1 || !vox->type)
		return(0);

#if 0
	if(((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED2) ||
		((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2))
	{
		return(16);
	}
#endif

	//color matching...
	switch(vox->type&VOX_TY_MASK)
	{
	case VOX_TY_REDSTONE_WIRE:
	case VOX_TY_REDSTONE_PIPE:
		switch(vox1->type&VOX_TY_MASK)
		{
		case VOX_TY_REDSTONE_WIRE:
		case VOX_TY_REDSTONE_PIPE:
		case VOX_TY_GOLDSTONE_WIRE:
		case VOX_TY_GOLDSTONE_PIPE:
			val=vox1->aux&15; break;
		case VOX_TY_REDSTONE_TORCH1:
		case VOX_TY_GOLDSTONE_TORCH1:
			val=16; break;
		case VOX_TY_GREENSTONE_WIRE:
		case VOX_TY_GREENSTONE_TORCH1:
		case VOX_TY_GREENSTONE_PIPE:
			val=-1; break;
		case VOX_TY_BLUESTONE_WIRE:
		case VOX_TY_BLUESTONE_TORCH1:
		case VOX_TY_BLUESTONE_PIPE:
			val=-1; break;
		default: val=0; break;
		}
		break;

	case VOX_TY_GREENSTONE_WIRE:
	case VOX_TY_GREENSTONE_PIPE:
		switch(vox1->type&VOX_TY_MASK)
		{
		case VOX_TY_GREENSTONE_WIRE:
		case VOX_TY_GREENSTONE_PIPE:
		case VOX_TY_GOLDSTONE_WIRE:
		case VOX_TY_GOLDSTONE_PIPE:
			val=vox1->aux&15; break;
		case VOX_TY_GREENSTONE_TORCH1:
		case VOX_TY_GOLDSTONE_TORCH1:
			val=16; break;
		case VOX_TY_REDSTONE_WIRE:
		case VOX_TY_REDSTONE_TORCH1:
		case VOX_TY_REDSTONE_PIPE:
			val=-1; break;
		case VOX_TY_BLUESTONE_WIRE:
		case VOX_TY_BLUESTONE_TORCH1:
		case VOX_TY_BLUESTONE_PIPE:
			val=-1; break;
		default: val=0; break;
		}
		break;

	case VOX_TY_BLUESTONE_WIRE:
	case VOX_TY_BLUESTONE_PIPE:
		switch(vox1->type&VOX_TY_MASK)
		{
		case VOX_TY_BLUESTONE_WIRE:
		case VOX_TY_BLUESTONE_PIPE:
		case VOX_TY_GOLDSTONE_WIRE:
		case VOX_TY_GOLDSTONE_PIPE:
			val=vox1->aux&15; break;
		case VOX_TY_BLUESTONE_TORCH1:
		case VOX_TY_GOLDSTONE_TORCH1:
			val=16; break;
		case VOX_TY_REDSTONE_WIRE:
		case VOX_TY_REDSTONE_TORCH1:
		case VOX_TY_REDSTONE_PIPE:
			val=-1; break;
		case VOX_TY_GREENSTONE_WIRE:
		case VOX_TY_GREENSTONE_TORCH1:
		case VOX_TY_GREENSTONE_PIPE:
			val=-1; break;
		default: val=0; break;
		}
		break;

	default:
		val=0; break;
	}
	
	if(val<0)return(0);
	if(val>0)return(val);

	if(	((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_WIRE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_WIRE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_WIRE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_WIRE))
	{
		return(vox1->aux&15);
	}

	if(	((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE))
	{
		return(vox1->aux&15);
	}

	if(	((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH1) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH1) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH1) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH1))
	{
		return(16);
	}

#if 0
//	if(vox1->type==VOX_TY_REDSTONE_WIRE)
//	{
//		return(vox1->aux&15);
//	}

//	if(vox1->type==VOX_TY_REDSTONE_TORCH1)
//	{
//		return(16);
//	}
#endif

	if((vox1->type==VOX_TY_LEVER) ||
		(vox1->type==VOX_TY_STONE_BUTTON) ||
		(vox1->type==VOX_TY_WOOD_BUTTON))
	{
		if(vox1->aux&8)
			return(16);
//		return(0);
	}

	if((vox1->type==VOX_TY_STONE_PLATE) ||
		(vox1->type==VOX_TY_WOOD_PLATE))
	{
		if(vox1->aux&1)
			return(16);
//		return(0);
	}

#if 1
	if(((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED2) ||
		((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2))
	{
		return(16);
	}
#endif

	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox1, x1, y1, z1);
	if(inf && inf->GetPower)
	{
		return(inf->GetPower(world, chk,
			vox1, vox, x1, y1, z1, x0, y0, z0));
	}

	return(0);
}

#if 0
// BTGE_API int BTGE_VoxelPower_VoxelGetPower2(
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1)
{
	return(BTGE_VoxelPower_WorldVoxelGetPower2(
		NULL, NULL, vox, vox1,
		0,0,0, 0,0,0));
}
#endif

BTGE_API int BTGE_VoxelPower_WorldVoxelGetPower2(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1,
	int x0, int y0, int z0, int x1, int y1, int z1)
{
	if(!vox1 || !vox->type)
		return(0);

	if(((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED) ||
		((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED))
	{
		return(16);
	}

#if 0
	if(	((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH1))
	{
//		return(16);
	}
#endif

//	return(BTGE_VoxelPower_VoxelGetPower(vox, vox1));
	return(BTGE_VoxelPower_WorldVoxelGetPower(
		world, chk, vox, vox1, x0,y0,z0, x1,y1,z1));
}

BTGE_API void BTGE_VoxelPower_TouchAdjacent(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	int x, int y, int z)
{
	BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x-1, y, z);
	BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x+1, y, z);
	BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y-1, z);
	BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y+1, z);
	BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z-1);
	BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z+1);
}

BTGE_API void BTGE_VoxelPower_VoxelUpdatePower(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1,
	int xo, int yo, int zo,
	int x, int y, int z,
	int pwr)
{
	BTGE_VoxelInfo *inf;
	int i;

	if(!vox1)
	{
		vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
			chk->rgn, x, y, z);
//		return;
	}

	if(((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_WIRE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_WIRE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_WIRE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_WIRE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE))
	{
//		vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
//			chk->rgn, x, y, z);
//		BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);

		if(pwr>=16)
		{
			if((vox1->aux&15)==15)
				return;

			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
			vox1->aux=(vox->aux&0xF0)|15;

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_NORMAL)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_POWERED2; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UB_POWERED2; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_POWERED2; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UB_POWERED2; }
			return;
		}

		if(pwr==-2)
		{
			if((vox1->aux&15)==0)
				return;

			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
			vox1->aux=(vox->aux&0xF0)|0;

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_NORMAL; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED2)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_NORMAL; }

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UNBREAKABLE; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UNBREAKABLE; }
			return;
		}

#if 0
		if((pwr-1)>(vox1->aux&15))
		{
			i=pwr-1;
			if(i<0)i=0;
			if(i>15)i=15;
			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			vox1->aux=(vox->aux&0xF0)|i;
		}
		
		if(pwr<0)
		{
			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			vox1->aux=(vox->aux&0xF0)|0;
		}
#endif
		return;
	}

#if 1
	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox1, x, y, z);
	if(inf && inf->UpdatePower)
	{
		inf->UpdatePower(world, chk,
			vox1, vox, x, y, z, xo, yo, zo,
			pwr);
		return;
	}
#endif

	if(BTGE_Voxel_VoxelOpaqueP(vox1) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_AIR))
	{
		if(pwr>=16)
		{
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED2)
				return;
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2)
				return;

			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);

			if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP0)
				{ vox1->type=(vox1->type&VOX_TYM_MASK)|
					VOX_TY_REDSTONE_LAMP1; }

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_NORMAL)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_POWERED2; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UB_POWERED2; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_POWERED2; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UB_POWERED2; }
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
			return;
		}

		if(pwr>0)
		{
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED)
				return;
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED)
				return;
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED2)
				return;
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2)
				return;

			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);

			if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP0)
				{ vox1->type=(vox1->type&VOX_TYM_MASK)|
					VOX_TY_REDSTONE_LAMP1; }

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_NORMAL)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_POWERED; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UB_POWERED; }
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
			return;
		}
		
		if(pwr==-1)
		{
			if(	((vox1->type&VOX_TYM_MASK)==VOX_TYM_NORMAL)||
				((vox1->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE))
			{
				if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP1)
				{
					vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
						chk->rgn, x, y, z);
					BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
					vox1->type=(vox1->type&VOX_TYM_MASK)|
						VOX_TY_REDSTONE_LAMP0;
				}
				return;
			}

//			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_NORMAL)
//				return;
//			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE)
//				return;

			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);

			if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP1)
				{ vox1->type=(vox1->type&VOX_TYM_MASK)|
					VOX_TY_REDSTONE_LAMP0; }

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_NORMAL; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UNBREAKABLE; }
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
			return;
		}

		if(pwr==-2)
		{
			if(	((vox1->type&VOX_TYM_MASK)==VOX_TYM_NORMAL)||
				((vox1->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE))
			{
				if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP1)
				{
					vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
						chk->rgn, x, y, z);
					BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
					vox1->type=(vox1->type&VOX_TYM_MASK)|
						VOX_TY_REDSTONE_LAMP0;
				}
				return;
			}

//			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_NORMAL)
//				return;
//			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE)
//				return;

			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);

			if((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP1)
				{ vox1->type=(vox1->type&VOX_TYM_MASK)|
					VOX_TY_REDSTONE_LAMP0; }

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_NORMAL; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_POWERED2)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_NORMAL; }

			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UNBREAKABLE; }
			if((vox1->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2)
				{ vox1->type=(vox1->type&VOX_TY_MASK)|VOX_TYM_UNBREAKABLE; }
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);
			return;
		}
		
		return;
	}

#if 0
	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox1, x, y, z);
	if(inf && inf->UpdatePower)
	{
		inf->UpdatePower(world, chk,
			vox1, vox, x, y, z, xo, yo, zo,
			pwr);
		return;
	}
#endif
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel_RedstoneLamp(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2, *vox3, *vox4, *vox5, *vox6;
	int pwr1, pwr2, pwr3, pwr4, pwr5, pwr6;
	int pwr;
	int i, j, k;

	if((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP1)
	{
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
		vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
		vox3=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
		vox4=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
		vox5=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
		vox6=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z+1);

		pwr1=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox2, x, y, z, x+1, y, z);
		pwr3=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox3, x, y, z, x, y-1, z);
		pwr4=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox4, x, y, z, x, y+1, z);
		pwr5=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox5, x, y, z, x, y, z-1);
		pwr6=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox6, x, y, z, x, y, z+1);
	
		pwr=MAX(MAX(MAX(pwr1, pwr2), MAX(pwr3, pwr4)), MAX(pwr5, pwr6));
		
		if(pwr<=0)
		{
			vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);

			vox->type=(vox->type&VOX_TYM_MASK)|
				VOX_TY_REDSTONE_LAMP0;
		}
	}

	if((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_LAMP0)
	{
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
		vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
		vox3=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
		vox4=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
		vox5=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
		vox6=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z+1);

		pwr1=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox2, x, y, z, x+1, y, z);
		pwr3=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox3, x, y, z, x, y-1, z);
		pwr4=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox4, x, y, z, x, y+1, z);
		pwr5=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox5, x, y, z, x, y, z-1);
		pwr6=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox6, x, y, z, x, y, z+1);
	
		pwr=MAX(MAX(MAX(pwr1, pwr2), MAX(pwr3, pwr4)), MAX(pwr5, pwr6));
		
		if(pwr>0)
		{
			vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world,
				chk->rgn, x, y, z);
			BTGE_VoxelPower_TouchAdjacent(world, chk, x, y, z);

			vox->type=(vox->type&VOX_TYM_MASK)|
				VOX_TY_REDSTONE_LAMP1;
		}
	}
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel_Redstone(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2, *vox3, *vox4, *vox5, *vox6;
	int pwr1, pwr2, pwr3, pwr4, pwr5, pwr6;
	int pwr;
	int i, j, k;

	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
	vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
	vox3=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
	vox4=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
	vox5=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
	vox6=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z+1);
	
	if(((vox->type&VOX_TYM_MASK)==VOX_TYM_POWERED) ||
		((vox->type&VOX_TYM_MASK)==VOX_TYM_POWERED2) ||
		((vox->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED) ||
		((vox->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2))
	{
		//direct-powered
		pwr=16;
	}else
	{
		//indirect power
		pwr1=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox2, x, y, z, x+1, y, z);
		pwr3=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox3, x, y, z, x, y-1, z);
		pwr4=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox4, x, y, z, x, y+1, z);
//		pwr5=BTGE_VoxelPower_VoxelGetPower2(vox, vox5);
//		pwr6=BTGE_VoxelPower_VoxelGetPower2(vox, vox6);
		pwr5=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox5, x, y, z, x, y, z-1);
		pwr6=BTGE_VoxelPower_WorldVoxelGetPower(world, chk,
			vox, vox6, x, y, z, x, y, z+1);
	
		pwr=MAX(MAX(MAX(pwr1, pwr2), MAX(pwr3, pwr4)), MAX(pwr5, pwr6));
	}

	if((pwr-1)>(vox->aux&15))
	{
//		printf("BTGE_VoxelPower_UpdateVoxel_Redstone(%d,%d,%d): "
//			"Boost Power %d\n", x, y, z, pwr);

		i=pwr-1;
		if(i<0)i=0;
		if(i>15)i=15;
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux=(vox->aux&0xF0)|i;
		
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox1, x, y, z, x-1, y, z, i);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox2, x, y, z, x+1, y, z, i);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox3, x, y, z, x, y-1, z, i);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox4, x, y, z, x, y+1, z, i);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox5, x, y, z, x, y, z-1, i);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox5, x, y, z, x, y, z+1, i);
	}

	if((vox->aux&15) && ((pwr-1)<(vox->aux&15)))
	{
//		printf("BTGE_VoxelPower_UpdateVoxel_Redstone(%d,%d,%d): "
//			"Reset Power %d\n", x, y, z, pwr);

		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux=(vox->aux&0xF0)|0;
		
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox1, x, y, z, x-1, y, z, -1);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox2, x, y, z, x+1, y, z, -1);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox3, x, y, z, x, y-1, z, -1);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox4, x, y, z, x, y+1, z, -1);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox5, x, y, z, x, y, z-1, -1);
		BTGE_VoxelPower_VoxelUpdatePower(world, chk,
			vox, vox5, x, y, z, x, y, z+1, -1);
	}
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel_RedstoneTorch0(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2;
	int pwr, flip;

	switch(vox->aux&15)
	{
	case 0:
	case 5:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y, z-1);
		break;
	case 1:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		break;
	case 2:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x+1, y, z);
		break;
	case 3:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y-1, z);
		break;
	case 4:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y+1, z);
		break;
	default:
		vox1=NULL;
		pwr=0;
		break;
	}
	
	if(!vox1)return;
	
	if(((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE))
	{
		flip=(pwr>0);
	}else
	{
		flip=(pwr<1);
	}
	
//	if(pwr<1)
	if(flip)
	{
//		printf("BTGE_VoxelPower_UpdateVoxel_RedstoneTorch0"
//			"(%d,%d,%d): Enable\n", x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
//		vox->type=VOX_TY_REDSTONE_TORCH1;
		vox->delay=1;

		switch(vox->type&VOX_TY_MASK)
		{
		case VOX_TY_REDSTONE_TORCH0:
			vox->type=VOX_TY_REDSTONE_TORCH1;
			break;
		case VOX_TY_GREENSTONE_TORCH0:
			vox->type=VOX_TY_GREENSTONE_TORCH1;
			break;
		case VOX_TY_BLUESTONE_TORCH0:
			vox->type=VOX_TY_BLUESTONE_TORCH1;
			break;
		case VOX_TY_GOLDSTONE_TORCH0:
			vox->type=VOX_TY_GOLDSTONE_TORCH1;
			break;
		default:
			break;
		}

		//upright: send signal to block above
		if(((vox->aux&15)==0) || ((vox->aux&15)==5))
		{
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z+1);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x, y, z+1, 16);
		}
	}
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel_RedstoneTorch1(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2;
	int pwr, flip;

	switch(vox->aux&15)
	{
	case 0:
	case 5:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y, z-1);
		break;
	case 1:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		break;
	case 2:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x+1, y, z);
		break;
	case 3:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y-1, z);
		break;
	case 4:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y+1, z);
		break;
	default:
		vox1=NULL;
		pwr=0;
		break;
	}
	
	if(!vox1)return;
	
	if(((vox1->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE) ||
		((vox1->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE))
	{
		flip=(pwr<1);
	}else
	{
		flip=(pwr>0);
	}
	
//	if(pwr<1)
	if(flip)
	{
//		printf("BTGE_VoxelPower_UpdateVoxel_RedstoneTorch1: Disable\n");
//		printf("BTGE_VoxelPower_UpdateVoxel_RedstoneTorch0"
//			"(%d,%d,%d): Disable\n", x, y, z);

		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
//		vox->type=VOX_TY_REDSTONE_TORCH0;
		vox->delay=1;

		switch(vox->type&VOX_TY_MASK)
		{
		case VOX_TY_REDSTONE_TORCH1:
			vox->type=VOX_TY_REDSTONE_TORCH0;
			break;
		case VOX_TY_GREENSTONE_TORCH1:
			vox->type=VOX_TY_GREENSTONE_TORCH0;
			break;
		case VOX_TY_BLUESTONE_TORCH1:
			vox->type=VOX_TY_BLUESTONE_TORCH0;
			break;
		case VOX_TY_GOLDSTONE_TORCH1:
			vox->type=VOX_TY_GOLDSTONE_TORCH0;
			break;
		default:
			break;
		}

		//upright: send signal to block above
		if(((vox->aux&15)==0) || ((vox->aux&15)==5))
		{
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z+1);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x, y, z+1, -2);
		}
	}
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel_RedstoneRelay(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2;
	int pwr, flip;
	int i, j;

	switch(vox->aux&3)
	{
	case VOX_AXFL_RELAY_EAST:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		break;
	case VOX_AXFL_RELAY_WEST:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x+1, y, z);
		break;
	case VOX_AXFL_RELAY_SOUTH:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y+1, z);
		break;
	case VOX_AXFL_RELAY_NORTH:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
		pwr=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y-1, z);
		break;
	default:
		pwr=0;
		vox1=NULL;
		break;
	}
	
	switch(vox->type&VOX_TY_MASK)
	{
	case VOX_TY_REDSTONE_RELAY0:
		flip=pwr>0; break;
	case VOX_TY_REDSTONE_RELAY1:
		flip=pwr<1; break;
	default:
		flip=0; break;
	}
	
//	if(pwr<1)
	if(flip)
	{
//		printf("BTGE_VoxelPower_UpdateVoxel_RedstoneTorch1: Disable\n");
//		printf("BTGE_VoxelPower_UpdateVoxel_RedstoneTorch0"
//			"(%d,%d,%d): Disable\n", x, y, z);

		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
//		vox->type=VOX_TY_REDSTONE_TORCH0;
		vox->delay=1;

		switch(vox->type&VOX_TY_MASK)
		{
		case VOX_TY_REDSTONE_RELAY0:
			vox->type=VOX_TY_REDSTONE_RELAY1;
			break;
		case VOX_TY_REDSTONE_RELAY1:
			vox->type=VOX_TY_REDSTONE_RELAY0;
			break;
		default:
			break;
		}

		i=((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_RELAY1)?16:-2;

		//update block powering
		switch(vox->aux&3)
		{
		case VOX_AXFL_RELAY_EAST:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x+1, y, z, i);
			break;
		case VOX_AXFL_RELAY_WEST:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x-1, y, z, i);
			break;
		case VOX_AXFL_RELAY_SOUTH:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x, y-1, z, i);
			break;
		case VOX_AXFL_RELAY_NORTH:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x, y+1, z, i);
			break;
		default:
			break;
		}
	}
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel_RedstoneGate(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2;
	int pwr, pwr1, pwr2, flip;
	int i, j;

	switch(vox->aux&3)
	{
	case VOX_AXFL_RELAY_EAST:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
		vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
//		pwr1=BTGE_VoxelPower_VoxelGetPower2(vox, vox1);
//		pwr2=BTGE_VoxelPower_VoxelGetPower2(vox, vox2);
		pwr1=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y-1, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox2, x, y, z, x, y+1, z);
		break;
	case VOX_AXFL_RELAY_WEST:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
		vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
//		pwr1=BTGE_VoxelPower_VoxelGetPower2(vox, vox1);
//		pwr2=BTGE_VoxelPower_VoxelGetPower2(vox, vox2);
		pwr1=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x, y+1, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox2, x, y, z, x, y-1, z);
		break;
	case VOX_AXFL_RELAY_SOUTH:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
		vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
//		pwr1=BTGE_VoxelPower_VoxelGetPower2(vox, vox1);
//		pwr2=BTGE_VoxelPower_VoxelGetPower2(vox, vox2);
		pwr1=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x+1, y, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox2, x, y, z, x-1, y, z);
		break;
	case VOX_AXFL_RELAY_NORTH:
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
		vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
//		pwr1=BTGE_VoxelPower_VoxelGetPower2(vox, vox1);
//		pwr2=BTGE_VoxelPower_VoxelGetPower2(vox, vox2);
		pwr1=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox2, x, y, z, x+1, y, z);
		break;
	default:
		pwr=0;
		break;
	}
	
	switch(vox->type&VOX_TY_MASK)
	{
	case VOX_TY_REDSTONE_GATE_AND:
		pwr=(pwr1>0)&(pwr2>0); break;
	case VOX_TY_REDSTONE_GATE_OR:
		pwr=(pwr1>0)|(pwr2>0); break;
	case VOX_TY_REDSTONE_GATE_XOR:
		pwr=(pwr1>0)^(pwr2>0); break;
	case VOX_TY_REDSTONE_GATE_NAND:
		pwr=!((pwr1>0)&(pwr2>0)); break;
	case VOX_TY_REDSTONE_GATE_NOR:
		pwr=!((pwr1>0)|(pwr2>0)); break;
	case VOX_TY_REDSTONE_GATE_XNOR:
		pwr=!((pwr1>0)^(pwr2>0)); break;
	default:
		pwr=0; break;
	}
	
	flip=(vox->aux&8)?(pwr<1):(pwr>0);
	
//	if(pwr<1)
	if(flip)
	{
//		printf("BTGE_VoxelPower_UpdateVoxel_RedstoneTorch1: Disable\n");
//		printf("BTGE_VoxelPower_UpdateVoxel_RedstoneTorch0"
//			"(%d,%d,%d): Disable\n", x, y, z);

		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
//		vox->type=VOX_TY_REDSTONE_TORCH0;
		vox->delay=1;

		vox->aux^=8;
		i=(vox->aux&8)?16:-2;

		//update block powering (output)
		switch(vox->aux&3)
		{
		case VOX_AXFL_RELAY_EAST:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x+1, y, z, i);
			break;
		case VOX_AXFL_RELAY_WEST:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x-1, y, z, i);
			break;
		case VOX_AXFL_RELAY_SOUTH:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x, y-1, z, i);
			break;
		case VOX_AXFL_RELAY_NORTH:
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
			BTGE_VoxelPower_VoxelUpdatePower(world, chk,
				vox, vox2, x, y, z, x, y+1, z, i);
			break;
		default:
			break;
		}
	}
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel_PressurePlate(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if(vox->aux&1)
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux&=~1;
//		vox->type=VOX_TY_REDSTONE_TORCH0;
//		vox->delay=1;
	}
}

BTGE_API void BTGE_VoxelPower_UpdateVoxel(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_WIRE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_WIRE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_WIRE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_WIRE))
	{
		BTGE_VoxelPower_UpdateVoxel_Redstone(world, chk, vox, x, y, z);
		return;
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH0))
	{
		BTGE_VoxelPower_UpdateVoxel_RedstoneTorch0(world, chk, vox, x, y, z);
		return;
	}
	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH1))
	{
		BTGE_VoxelPower_UpdateVoxel_RedstoneTorch1(world, chk, vox, x, y, z);
		return;
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_PIPE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_PIPE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_PIPE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_PIPE))
	{
		BTGE_VoxelPower_UpdateVoxel_Redstone(world, chk, vox, x, y, z);
		return;
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_RELAY0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_RELAY1))
	{
		BTGE_VoxelPower_UpdateVoxel_RedstoneRelay(world, chk, vox, x, y, z);
		return;
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_AND) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_OR) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_XOR) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_NAND) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_NOR) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_GATE_XNOR))
	{
		BTGE_VoxelPower_UpdateVoxel_RedstoneGate(world, chk, vox, x, y, z);
		return;
	}

	if(((vox->type&VOX_TY_MASK)==VOX_TY_STONE_PLATE) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_WOOD_PLATE))
	{
		BTGE_VoxelPower_UpdateVoxel_PressurePlate(world, chk, vox, x, y, z);
		return;
	}
}

BTGE_API void BTGE_VoxelPower_UpdatePoweredVoxel(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
}

BTGE_API int BTGE_VoxelPower_GetBlockAmbientPower(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2, *vox3, *vox4, *vox5, *vox6;
	int pwr1, pwr2, pwr3, pwr4, pwr5, pwr6;
	int pwr;
	int i, j, k;

	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
	vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
	vox3=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
	vox4=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
	vox5=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
	vox6=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z+1);
	
	if(((vox->type&VOX_TYM_MASK)==VOX_TYM_POWERED) ||
		((vox->type&VOX_TYM_MASK)==VOX_TYM_POWERED2) ||
		((vox->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED) ||
		((vox->type&VOX_TYM_MASK)==VOX_TYM_UB_POWERED2))
	{
		//direct-powered
		pwr=16;
	}else
	{
		//indirect power
		pwr1=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox1, x, y, z, x-1, y, z);
		pwr2=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox2, x, y, z, x+1, y, z);
		pwr3=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox3, x, y, z, x, y-1, z);
		pwr4=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox4, x, y, z, x, y+1, z);
//		pwr5=BTGE_VoxelPower_VoxelGetPower2(vox, vox5);
//		pwr6=BTGE_VoxelPower_VoxelGetPower2(vox, vox6);
		pwr5=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox5, x, y, z, x, y, z-1);
		pwr6=BTGE_VoxelPower_WorldVoxelGetPower2(world, chk,
			vox, vox6, x, y, z, x, y, z+1);
	
		pwr=MAX(MAX(MAX(pwr1, pwr2), MAX(pwr3, pwr4)), MAX(pwr5, pwr6));
	}
	
	return(pwr);
}
