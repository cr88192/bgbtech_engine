#include <btgecm.h>

BTGE_API bool BTGE_VoxelRail_VoxelCanConnectP(
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1)
{
	if(!vox)return(false);
	if(!vox->type)return(false);

	if(!vox1)return(false);
	if(!vox1->type)return(false);

	if((vox->type&VOX_TY_MASK)==VOX_TY_RAIL)
	{
		if((vox1->type&VOX_TY_MASK)==VOX_TY_RAIL)
		{
			return(true);
		}
	}

	return(false);
}

BTGE_API bool BTGE_VoxelRail_CheckVoxelCanConnectP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
	return(BTGE_VoxelRail_VoxelCanConnectP(vox, vox1));
}

BTGE_API bool BTGE_VoxelRail_VoxelRailP(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox, int x, int y, int z)
{
	if((vox->type&VOX_TY_MASK)==VOX_TY_RAIL)
		return(true);
	return(false);
}

BTGE_API void BTGE_Voxel_EmitBlock_Rail(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static float sta[8]= { 0, 0, 0, 1, 1, 1, 1, 0};
	float pts[4*3];
	float rgba[4], nv[4];
	BTGE_VoxelInfo *inf;
	float f, g;
	int tex, fl2;
	int i, j, rot, dr;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

#if 0
	fl2=0;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x-1, y, z))
		fl2|=VOX_FL_NX;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x+1, y, z))
		fl2|=VOX_FL_PX;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y-1, z))
		fl2|=VOX_FL_NY;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y+1, z))
		fl2|=VOX_FL_PY;

//	if((fl2==VOX_FL_NX) || (fl2==VOX_FL_PX) || (fl2==(VOX_FL_NX|VOX_FL_PX)))
//		tex=inf->img_side;
//	if((fl2==VOX_FL_NY) || (fl2==VOX_FL_PY) || (fl2==(VOX_FL_NY|VOX_FL_PY)))
//		tex=inf->img_bot;

	if((fl2==VOX_FL_NX) || (fl2==VOX_FL_PX) || (fl2==(VOX_FL_NX|VOX_FL_PX)))
		vox->aux=(vox->aux&0xF0)|1;
	if((fl2==VOX_FL_NY) || (fl2==VOX_FL_PY) || (fl2==(VOX_FL_NY|VOX_FL_PY)))
		vox->aux=(vox->aux&0xF0)|0;

	if(fl2==(VOX_FL_PX|VOX_FL_NY))
		vox->aux=(vox->aux&0xF0)|6;
	if(fl2==(VOX_FL_NX|VOX_FL_NY))
		vox->aux=(vox->aux&0xF0)|7;
	if(fl2==(VOX_FL_NX|VOX_FL_PY))
		vox->aux=(vox->aux&0xF0)|8;
	if(fl2==(VOX_FL_PX|VOX_FL_PY))
		vox->aux=(vox->aux&0xF0)|9;

	if((vox->aux&15)==0)
	{
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y+1, z+1))
			vox->aux=(vox->aux&0xF0)|4;
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y-1, z+1))
			vox->aux=(vox->aux&0xF0)|5;
	}

	if((vox->aux&15)==1)
	{
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x+1, y, z+1))
			vox->aux=(vox->aux&0xF0)|2;
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x-1, y, z+1))
			vox->aux=(vox->aux&0xF0)|3;
	}
#endif

	for(i=0; i<4; i++)
	{
		pts[i*3+0]=(i&1)?max[0]:min[0];
		pts[i*3+1]=(i&2)?max[1]:min[1];
		pts[i*3+2]=min[2]+1;
	}

	switch(vox->aux&15)
	{
	case 0: rot=0; dr=0; break;
	case 1: rot=1; dr=0; break;

	case 2: rot=1; dr=1;
		pts[1*3+2]=min[2]+33;
		pts[3*3+2]=min[2]+33;
		break;
	case 3: rot=1; dr=1;
		pts[0*3+2]=min[2]+33;
		pts[2*3+2]=min[2]+33;
		break;

	case 4: rot=0; dr=1;
		pts[2*3+2]=min[2]+33;
		pts[3*3+2]=min[2]+33;
		break;
	case 5: rot=0; dr=1;
		pts[0*3+2]=min[2]+33;
		pts[1*3+2]=min[2]+33;
		break;

	case 6: tex=inf->img_side; rot=0; dr=0; break;
	case 7: tex=inf->img_side; rot=3; dr=0; break;
	case 8: tex=inf->img_side; rot=2; dr=0; break;
	case 9: tex=inf->img_side; rot=1; dr=0; break;
	default: rot=0; break;
	}


//	f=0.5+((vox->aux&0x0F)/15.0)*0.5;
	f=1;

	nv[0]=0; nv[1]=0; nv[2]=1; nv[3]=min[2]+1;
	rgba[0]=f; rgba[1]=f; rgba[2]=f; rgba[3]=1; 

	BTGE_Voxel_EmitFace(chk, vox,
		pts+1*3, sta+((rot+3)&3)*2,
		pts+3*3, sta+((rot+2)&3)*2,
		pts+2*3, sta+((rot+1)&3)*2,
		pts+0*3, sta+((rot+0)&3)*2,
		nv, rgba, tex);
		
	if(dr)
	{
		BTGE_Voxel_EmitFace(chk, vox,
			pts+0*3, sta+((rot+0)&3)*2,
			pts+2*3, sta+((rot+1)&3)*2,
			pts+3*3, sta+((rot+2)&3)*2,
			pts+1*3, sta+((rot+3)&3)*2,
			nv, rgba, tex);
	}
}

BTGE_API void BTGE_VoxelRail_UpdateVoxel_Rail(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelInfo *inf;
	float f, g;
	int tex, fl2;
	int i, j, rot;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex=inf->img_top;

	fl2=0;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x-1, y, z))
		fl2|=VOX_FL_NX;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x+1, y, z))
		fl2|=VOX_FL_PX;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y-1, z))
		fl2|=VOX_FL_NY;
	if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y+1, z))
		fl2|=VOX_FL_PY;

	if((fl2==VOX_FL_NX) || (fl2==VOX_FL_PX) || (fl2==(VOX_FL_NX|VOX_FL_PX)))
		vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_EAST_WEST;
	if((fl2==VOX_FL_NY) || (fl2==VOX_FL_PY) || (fl2==(VOX_FL_NY|VOX_FL_PY)))
		vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_NORTH_SOUTH;

	if(fl2==(VOX_FL_PX|VOX_FL_NY))
		vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_EAST_SOUTH;
	if(fl2==(VOX_FL_NX|VOX_FL_NY))
		vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_WEST_SOUTH;
	if(fl2==(VOX_FL_NX|VOX_FL_PY))
		vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_WEST_NORTH;
	if(fl2==(VOX_FL_PX|VOX_FL_PY))
		vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_EAST_NORTH;

	if((vox->aux&15)==VOX_AXFL_RAIL_NORTH_SOUTH)
	{
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y+1, z+1))
			vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_ASCEND_NORTH;
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x, y-1, z+1))
			vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_ASCEND_SOUTH;
	}

	if((vox->aux&15)==VOX_AXFL_RAIL_EAST_WEST)
	{
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x+1, y, z+1))
			vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_ASCEND_EAST;
		if(BTGE_VoxelRail_CheckVoxelCanConnectP(world, chk, vox, x-1, y, z+1))
			vox->aux=(vox->aux&0xF0)|VOX_AXFL_RAIL_ASCEND_WEST;
	}
}

BTGE_API void BTGE_VoxelRail_UpdateVoxel(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if((vox->type&VOX_TY_MASK)==VOX_TY_RAIL)
	{
		BTGE_VoxelRail_UpdateVoxel_Rail(world, chk, vox, x, y, z);
		return;
	}
}

int BTGE_Voxel_GetPointVoxelCoord(BTGE_BrushWorld *world,
	double *org, int *rxyz)
{
	rxyz[0]=floor(org[0]/32);
	rxyz[1]=floor(org[1]/32);
	rxyz[2]=floor(org[2]/32);
	return(0);
}

/** Attempt to align a point such that it moves along the center of
  * a rail block. */
BTGE_API int BTGE_VoxelRail_AlignPointRail(
	BTGE_BrushWorld *world, 
	double *org, float *mins, float *maxs,
	double *org1)
{
	double mid[3], min[3], max[3];
	int apt[3];
	BTGE_VoxelData *vox, *vox1;
	float f, g, h;
	int i, j, k, n;

	if(!world->vox_region)
	{
		V3F_COPY(org, org1);
		return(-1);
	}

	BTGE_Voxel_GetPointVoxelCoord(world, org, apt);

	vox=BTGE_Voxel_GetWorldVoxel(world,
		apt[0], apt[1], apt[2]);

	if(!vox || !BTGE_VoxelRail_VoxelRailP(
		world, vox, apt[0], apt[1], apt[2]))
	{
		vox1=BTGE_Voxel_GetWorldVoxel(world,
			apt[0], apt[1], apt[2]-1);
		if(vox1 && BTGE_VoxelRail_VoxelRailP(world, vox1,
			apt[0], apt[1], apt[2]-1))
		{
			vox=vox1;
			apt[2]=apt[2]-1;
		}
	}

	if(!vox)
	{
		//no voxel, move freely
		V3F_COPY(org, org1);
		return(0);
	}
	
	if(!BTGE_VoxelRail_VoxelRailP(world, vox, apt[0], apt[1], apt[2]))
	{
		//voxel isn't rail, move freely
		V3F_COPY(org, org1);
		return(0);
	}

	mid[0]=apt[0]*32+16;
	mid[1]=apt[1]*32+16;
	mid[2]=apt[2]*32+16;

	min[0]=apt[0]*32;
	min[1]=apt[1]*32;
	min[2]=apt[2]*32;

	max[0]=apt[0]*32+32;
	max[1]=apt[1]*32+32;
	max[2]=apt[2]*32+32;

	V3F_COPY(org, org1);
	switch(vox->aux&15)
	{
	case VOX_AXFL_RAIL_NORTH_SOUTH:		//north/south
		org1[0]=mid[0];
		org1[2]=mid[2];
		break;
	case VOX_AXFL_RAIL_EAST_WEST:		//east/west
		org1[1]=mid[1];
		org1[2]=mid[2];
		break;

	case VOX_AXFL_RAIL_ASCEND_EAST:		//ascend east
		f=org[0]-mid[0]+16;
		org1[1]=mid[1];
		org1[2]=mid[2]+f;
		break;
	case VOX_AXFL_RAIL_ASCEND_WEST:		//ascend west
		f=mid[0]-org[0]+16;
		org1[1]=mid[1];
		org1[2]=mid[2]+f;
		break;
	case VOX_AXFL_RAIL_ASCEND_NORTH:		//ascend north
		f=org[1]-mid[1]+16;
		org1[0]=mid[0];
		org1[2]=mid[2]+f;
		break;
	case VOX_AXFL_RAIL_ASCEND_SOUTH:		//ascend south
		f=mid[1]-org[1]+16;
		org1[0]=mid[0];
		org1[2]=mid[2]+f;
		break;

	case VOX_AXFL_RAIL_EAST_SOUTH:
		h=atan2(org[1]-min[1], max[0]-org[0]);
//		org1[0]=max[0]-16*sin(h);
//		org1[1]=min[1]+16*cos(h);
		org1[0]=max[0]-16*cos(h);
		org1[1]=min[1]+16*sin(h);
		org1[2]=mid[2];
		break;
	case VOX_AXFL_RAIL_WEST_SOUTH:
		h=atan2(org[1]-min[1], org[0]-min[0]);
//		org1[0]=min[0]+16*sin(h);
//		org1[1]=min[1]+16*cos(h);
		org1[0]=min[0]+16*cos(h);
		org1[1]=min[1]+16*sin(h);
		org1[2]=mid[2];
		break;
	case VOX_AXFL_RAIL_WEST_NORTH:
		h=atan2(max[1]-org[1], org[0]-min[0]);
//		org1[0]=min[0]+16*sin(h);
//		org1[1]=max[1]-16*cos(h);
		org1[0]=min[0]+16*cos(h);
		org1[1]=max[1]-16*sin(h);
		org1[2]=mid[2];
		break;
	case VOX_AXFL_RAIL_EAST_NORTH:
		h=atan2(max[1]-org[1], max[0]-org[0]);
//		org1[0]=max[0]-16*sin(h);
//		org1[1]=max[1]-16*cos(h);
		org1[0]=max[0]-16*cos(h);
		org1[1]=max[1]-16*sin(h);
		org1[2]=mid[2];
		break;

#if 0
	case VOX_AXFL_RAIL_EAST_SOUTH:
		f=org[0]-mid[0]+16;
//		g=org[1]-mid[1]+16;
		g=mid[1]-org[1]+16;
		h=atan2(g, f);
//		org1[0]=mid[0]+16*sin(h);	org1[1]=mid[1]-16*cos(h);
		org1[0]=mid[0]+16*sin(h);	org1[1]=mid[1]-16*cos(h);
		org1[2]=mid[2];
		break;
	case VOX_AXFL_RAIL_WEST_SOUTH:
		f=mid[0]-org[0]+16;
//		g=org[1]-mid[1]+16;
		g=mid[1]-org[1]+16;
		h=atan2(g, f);
		org1[0]=mid[0]-16*sin(h);	org1[1]=mid[1]-16*cos(h);
		org1[2]=mid[2];
		break;
	case VOX_AXFL_RAIL_WEST_NORTH:
		f=mid[0]-org[0]+16;
//		g=mid[1]-org[1]+16;
		g=org[1]-mid[1]+16;
		h=atan2(g, f);
		org1[0]=mid[0]-16*sin(h);	org1[1]=mid[1]+16*cos(h);
		org1[2]=mid[2];
		break;
	case VOX_AXFL_RAIL_EAST_NORTH:
		f=org[0]-mid[0]+16;
//		g=mid[1]-org[1]+16;
		g=org[1]-mid[1]+16;
		h=atan2(g, f);
		org1[0]=mid[0]+16*sin(h);	org1[1]=mid[1]+16*cos(h);
		org1[2]=mid[2];
		break;
#endif

	default:
		org1[2]=mid[2];
		break;
	}

	return(1);

//	if(vox && BTGE_Voxel_VoxelSolidP(vox))
//	{
//	}
}

BTGE_API vec3d BTGE_VoxelRail_AlignPointRail2(
	BTGE_BrushWorld *world, 
	vec3d org, vec3 mins, vec3 maxs)
{
	double org0[3], org1[3];
	float min[3], max[3];
	
	vdvec3d(org0, org);
	vfvec3(min, mins);
	vfvec3(max, maxs);
	BTGE_VoxelRail_AlignPointRail(world, org0, min, max, org1);
	return(vec3dvd(org1));
}
