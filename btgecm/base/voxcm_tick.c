#include <btgecm.h>

void (*btge_voxel_spawnertick_f)(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z);

BTGE_API void BTGE_Voxel_SetSpawnerTick(
	void (*fcn)(
		BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
		BTGE_VoxelData *vox, int x, int y, int z))
{
	btge_voxel_spawnertick_f=fcn;
}

BTGE_API void BTGE_Voxel_UpdateVoxel_SpawnerTick(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if(btge_voxel_spawnertick_f)
		btge_voxel_spawnertick_f(world, chk, vox, x, y, z);
}

BTGE_API bool BTGE_Voxel_CheckVoxel_FluidSourceFlowInto(
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1)
{
	if(!vox1 || !vox1->type)
		return(true);
	if((vox1->type==(vox->type+1)) &&
		((int)(vox1->aux&0x0F)<0x07))
			return(true);

	return(false);
}

BTGE_API void BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, BTGE_VoxelData *vox1, BTGE_VoxelData *vox2,
	int x, int y, int z)
{
	BTGE_VoxelData *tvox1;

	if((vox1->type==vox->type) && (vox2->type==vox->type))
	{
//		tvox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		tvox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		if(!tvox1 || !tvox1->type || (tvox1->type==(vox->type+1)))
		{
			tvox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
			tvox1->type=vox->type;
			tvox1->delay=10;
		}
	}
}

BTGE_API void BTGE_Voxel_UpdateVoxel_FluidSource(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2, *vox3, *vox4, *vox5;
	BTGE_VoxelData *tvox1, *tvox2, *tvox3, *tvox4;

//	return;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x-1, y, z);
//	vox2=BTGE_Voxel_GetRegionVoxel(chk->rgn, x+1, y, z);
//	vox3=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y-1, z);
//	vox4=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y+1, z);
//	vox5=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z-1);

	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
	vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
	vox3=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
	vox4=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
	vox5=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);


//	if(!vox1 || !vox1->type ||
//		((vox1->type==vox->type+1) &&
//		((vox1->aux&0xF0)<0x07)))
	if(BTGE_Voxel_CheckVoxel_FluidSourceFlowInto(vox, vox1))
	{
		vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x-1, y, z);
		vox1->type=vox->type+1;
		vox1->aux=(vox1->aux&0xF0)|0x07;
		vox1->delay=10;
	}

//	if(!vox2 || !vox2->type ||
//		((vox2->type==vox->type+1) &&
//		((vox2->aux&0xF0)<0x07)))
	if(BTGE_Voxel_CheckVoxel_FluidSourceFlowInto(vox, vox2))
	{
		vox2=BTGE_Voxel_GetWorldVoxelDirty(world, x+1, y, z);
		vox2->type=vox->type+1;
		vox2->aux=(vox2->aux&0xF0)|0x07;
		vox2->delay=10;
	}

//	if(!vox3 || !vox3->type ||
//		((vox3->type==vox->type+1) &&
//		((vox3->aux&0xF0)<0x07)))
	if(BTGE_Voxel_CheckVoxel_FluidSourceFlowInto(vox, vox3))
	{
		vox3=BTGE_Voxel_GetWorldVoxelDirty(world, x, y-1, z);
		vox3->type=vox->type+1;
		vox3->aux=(vox3->aux&0xF0)|0x07;
		vox3->delay=10;
	}

//	if(!vox4 || !vox4->type ||
//		((vox4->type==vox->type+1) &&
//		((vox4->aux&0xF0)<0x07)))
	if(BTGE_Voxel_CheckVoxel_FluidSourceFlowInto(vox, vox4))
	{
		vox4=BTGE_Voxel_GetWorldVoxelDirty(world, x, y+1, z);
		vox4->type=vox->type+1;
		vox4->aux=(vox4->aux&0xF0)|0x07;
		vox4->delay=10;
	}

//	if(!vox5 || !vox5->type ||
//		((vox5->type==vox->type+1) &&
//		((vox5->aux&0xF0)<0x07)))
	if(BTGE_Voxel_CheckVoxel_FluidSourceFlowInto(vox, vox5))
	{
		vox5=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z-1);
		vox5->type=vox->type+1;
		vox5->aux=(vox5->aux&0xF0)|0x07;
		vox5->delay=5;
	}

	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox1, vox3, x-1, y-1, z);
	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox1, vox4, x-1, y+1, z);
	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox2, vox3, x+1, y-1, z);
	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox2, vox4, x+1, y+1, z);

	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox1, vox5, x-1, y, z-1);
	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox2, vox5, x+1, y, z-1);
	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox3, vox5, x, y-1, z-1);
	BTGE_Voxel_CheckUpdateVoxel_FluidSourceFillInto(world, chk,
		vox, vox4, vox5, x, y+1, z-1);

#if 0
	if((vox1->type==vox->type) && (vox3->type==vox->type))
	{
		tvox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x-1, y-1, z);
		if(!tvox1 || !tvox1->type || (tvox1->type==(vox->type+1)))
		{
			tvox1=BTGE_Voxel_GetWorldVoxelDirty(world, x-1, y-1, z);
			tvox1->type=vox->type;
			tvox1->delay=10;
		}
	}

	if((vox1->type==vox->type) && (vox4->type==vox->type))
	{
		tvox2=BTGE_Voxel_GetRegionVoxel(chk->rgn, x-1, y+1, z);
		if(!tvox2 || !tvox2->type || (tvox2->type==(vox->type+1)))
		{
			tvox2=BTGE_Voxel_GetWorldVoxelDirty(world, x-1, y+1, z);
			tvox2->type=vox->type;
			tvox2->delay=10;
		}
	}

	if((vox2->type==vox->type) && (vox3->type==vox->type))
	{
		tvox3=BTGE_Voxel_GetRegionVoxel(chk->rgn, x+1, y-1, z);
		if(!tvox3 || !tvox3->type || (tvox3->type==(vox->type+1)))
		{
			tvox3=BTGE_Voxel_GetWorldVoxelDirty(world, x+1, y-1, z);
			tvox3->type=vox->type;
			tvox3->delay=10;
		}
	}

	if((vox2->type==vox->type) && (vox4->type==vox->type))
	{
		tvox4=BTGE_Voxel_GetRegionVoxel(chk->rgn, x+1, y+1, z);
		if(!tvox4 || !tvox4->type || (tvox4->type==(vox->type+1)))
		{
			tvox4=BTGE_Voxel_GetWorldVoxelDirty(world, x+1, y+1, z);
			tvox4->type=vox->type;
			tvox4->delay=10;
		}
	}


	if((vox1->type==vox->type) && (vox5->type==vox->type))
	{
		tvox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x-1, y, z-1);
		if(!tvox1 || !tvox1->type || (tvox1->type==(vox->type+1)))
		{
			tvox1=BTGE_Voxel_GetWorldVoxelDirty(world, x-1, y, z-1);
			tvox1->type=vox->type;
			tvox1->delay=10;
		}
	}

	if((vox2->type==vox->type) && (vox5->type==vox->type))
	{
		tvox2=BTGE_Voxel_GetRegionVoxel(chk->rgn, x+1, y, z-1);
		if(!tvox2 || !tvox2->type || (tvox2->type==(vox->type+1)))
		{
			tvox2=BTGE_Voxel_GetWorldVoxelDirty(world, x+1, y, z-1);
			tvox2->type=vox->type;
			tvox2->delay=10;
		}
	}

	if((vox3->type==vox->type) && (vox5->type==vox->type))
	{
		tvox3=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y-1, z-1);
		if(!tvox3 || !tvox3->type || (tvox3->type==(vox->type+1)))
		{
			tvox3=BTGE_Voxel_GetWorldVoxelDirty(world, x, y-1, z-1);
			tvox3->type=vox->type;
			tvox3->delay=10;
		}
	}

	if((vox4->type==vox->type) && (vox5->type==vox->type))
	{
		tvox4=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y+1, z-1);
		if(!tvox4 || !tvox4->type || (tvox4->type==(vox->type+1)))
		{
			tvox4=BTGE_Voxel_GetWorldVoxelDirty(world, x, y+1, z-1);
			tvox4->type=vox->type;
			tvox4->delay=10;
		}
	}
#endif

}

BTGE_API bool BTGE_Voxel_CheckUpdateVoxel_FluidFlow(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1;

	if((vox->aux&0x0F)<1)
		return(false);

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
	if(!vox1 || !vox1->type ||
		((vox1->type==vox->type) &&
		((vox1->aux&0x0F)<((int)(vox->aux&0x0F)-1))))
	{
		BTGE_Voxel_UpdateVoxel_TouchAdjacent(world, chk, vox, x, y, z);
		vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		vox1->type=vox->type;
		vox1->aux=(vox1->aux&0xF0)|((vox->aux&0x0F)-1);
		vox1->delay=10;
		return(true);
	}
	
	if(vox->type==VOX_TY_WATER2)
	{
		if(vox1->type==VOX_TY_LAVA)
		{
			BTGE_Voxel_UpdateVoxel_TouchAdjacent(world, chk, vox, x, y, z);
			vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
			vox1->type=VOX_TY_OBSIDIAN;
			vox1->aux=vox1->aux&0xF0;
			return(true);
		}
		if(vox1->type==VOX_TY_LAVA2)
		{
			BTGE_Voxel_UpdateVoxel_TouchAdjacent(world, chk, vox, x, y, z);
			vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
			vox1->type=VOX_TY_COBBLESTONE;
			vox1->aux=vox1->aux&0xF0;
			return(true);
		}
	}

	if(vox->type==VOX_TY_LAVA2)
	{
		if(vox1->type==VOX_TY_WATER)
		{
			BTGE_Voxel_UpdateVoxel_TouchAdjacent(world, chk, vox, x, y, z);
			vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
			vox1->type=VOX_TY_STONE;
			vox1->aux=vox1->aux&0xF0;
			return(true);
		}
		if(vox1->type==VOX_TY_WATER2)
		{
			BTGE_Voxel_UpdateVoxel_TouchAdjacent(world, chk, vox, x, y, z);
			vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
			vox1->type=VOX_TY_STONE;
			vox1->aux=vox1->aux&0xF0;
			return(true);
		}
	}
	
	return(false);
}

BTGE_API bool BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox,
	int sx, int sy, int sz,
	int x, int y, int z)
{
	BTGE_VoxelData *vox1;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
	if(!vox1 || !vox1->type)
	{
		return(false);
	}
	
	if((vox1->type==(vox->type-1)) ||
		((vox1->type==vox->type) &&
		(((vox1->aux&0x0F)>((int)(vox->aux&0x0F))) || (z>sz))))
	{
//		vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
//		vox1->type=vox->type;
//		vox1->aux=(vox1->aux&0xF0)|((vox->aux&0x0F)-1);
//		vox1->delay=10;
		return(true);
	}
	return(false);	
}

BTGE_API void BTGE_Voxel_UpdateVoxel_TouchAdjacent(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_Voxel_GetWorldVoxelDirty(world, x+1, y, z);
	BTGE_Voxel_GetWorldVoxelDirty(world, x-1, y, z);
	BTGE_Voxel_GetWorldVoxelDirty(world, x, y+1, z);
	BTGE_Voxel_GetWorldVoxelDirty(world, x, y-1, z);
	BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z+1);
	BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z-1);
}

BTGE_API void BTGE_Voxel_UpdateVoxel_FluidFlow(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2, *vox3, *vox4;
//	BTGE_VoxelData *tvox1, *tvox2, *tvox3, *tvox4;
	BTGE_VoxelData *vox5;
	int i;

	if(!(vox->aux&0x0F))
	{
//		vox5=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z-1);
//		vox5=BTGE_Voxel_GetWorldVoxel(world, x, y, z-1);
		vox5=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
		if(!vox5 || !vox5->type ||
			((vox5->type==vox->type) &&
			((vox5->aux&0x0F)<0x07)))
		{
			vox5=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z-1);
			vox5->type=vox->type;
			vox5->aux=(vox5->aux&0xF0)|0x07;
			vox5->delay=5;
		}

		if(	!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
				world, chk, vox, x, y, z, x-1, y, z) &&
			!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
				world, chk, vox, x, y, z, x+1, y, z) &&
			!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
				world, chk, vox, x, y, z, x, y-1, z) &&
			!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
				world, chk, vox, x, y, z, x, y+1, z) &&
			!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
				world, chk, vox, x, y, z, x, y, z+1))
		{
			BTGE_Voxel_UpdateVoxel_TouchAdjacent(world, chk, vox, x, y, z);

			vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
			vox->type=VOX_TY_AIR;
			vox->aux=(vox->aux&0xF0)|0;
			vox->delay=10;
		}

		return;
	}

	vox5=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
	if(!vox5 || !vox5->type ||
		((vox5->type==vox->type)))
	{
		if((vox5->aux&0x0F)<0x07)
		{
			vox5=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z-1);
			vox5->type=vox->type;
			vox5->aux=(vox5->aux&0xF0)|0x07;
			vox5->delay=5;
		}
		return;
	}

	BTGE_Voxel_CheckUpdateVoxel_FluidFlow(world, chk, vox, x-1, y, z);
	BTGE_Voxel_CheckUpdateVoxel_FluidFlow(world, chk, vox, x+1, y, z);
	BTGE_Voxel_CheckUpdateVoxel_FluidFlow(world, chk, vox, x, y-1, z);
	BTGE_Voxel_CheckUpdateVoxel_FluidFlow(world, chk, vox, x, y+1, z);
	BTGE_Voxel_CheckUpdateVoxel_FluidFlow(world, chk, vox, x, y, z-1);

	if(	!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
			world, chk, vox, x, y, z, x-1, y, z) &&
		!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
			world, chk, vox, x, y, z, x+1, y, z) &&
		!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
			world, chk, vox, x, y, z, x, y-1, z) &&
		!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
			world, chk, vox, x, y, z, x, y+1, z) &&
		!BTGE_Voxel_CheckUpdateVoxel_FluidFlowFrom(
			world, chk, vox, x, y, z, x, y, z+1))
	{
		BTGE_Voxel_UpdateVoxel_TouchAdjacent(world, chk, vox, x, y, z);

		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		i=((int)(vox->aux&15))-1;
		if(i<0)
		{
			vox->type=VOX_TY_GLASS;
			vox->aux=(vox->aux&0xF0)|0;
			vox->delay=10;
		}else
		{
			vox->aux=(vox->aux&0xF0)|((vox->aux&0x0F)-1);
			vox->delay=10;
		}
	}

#if 0
	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x-1, y, z);
	vox2=BTGE_Voxel_GetRegionVoxel(chk->rgn, x+1, y, z);
	vox3=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y-1, z);
	vox4=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y+1, z);
	vox5=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z-1);

	if(!vox1 || !vox1->type ||
		((vox1->type==vox->type) &&
		((vox1->aux&0xF0)<((vox->aux&0x0F)-1))))
	{
		vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x-1, y, z);
		vox1->type=vox->type;
		vox1->aux=(vox1->aux&0xF0)|((vox->aux&0x0F)-1);
		vox1->delay=10;
	}

	if(!vox2 || !vox2->type ||
		((vox2->type==vox->type) &&
		((vox2->aux&0xF0)<((vox->aux&0x0F)-1))))
	{
		vox2=BTGE_Voxel_GetWorldVoxelDirty(world, x+1, y, z);
		vox2->type=vox->type;
		vox2->aux=(vox2->aux&0xF0)|((vox->aux&0x0F)-1);
		vox2->delay=10;
	}

	if(!vox3 || !vox3->type ||
		((vox3->type==vox->type) &&
		((vox3->aux&0xF0)<((vox->aux&0x0F)-1))))
	{
		vox3=BTGE_Voxel_GetWorldVoxelDirty(world, x, y-1, z);
		vox3->type=vox->type;
		vox3->aux=(vox3->aux&0xF0)|((vox->aux&0x0F)-1);
		vox3->delay=10;
	}

	if(!vox4 || !vox4->type ||
		((vox4->type==vox->type) &&
		((vox4->aux&0xF0)<((vox->aux&0x0F)-1))))
	{
		vox4=BTGE_Voxel_GetWorldVoxelDirty(world, x, y+1, z);
		vox4->type=vox->type;
		vox4->aux=(vox4->aux&0xF0)|((vox->aux&0x0F)-1);
		vox4->delay=10;
	}

	if(!vox5 || !vox5->type ||
		((vox5->type==vox->type) &&
		((vox5->aux&0xF0)<0x07)))
	{
		vox5=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z-1);
		vox5->type=vox->type;
		vox5->aux=(vox5->aux&0xF0)|0x07;
		vox5->delay=5;
	}
#endif
}

BTGE_API void BTGE_Voxel_UpdateVoxel_Falling(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData tvox1, tvox2;
	BTGE_VoxelData *vox1;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z-1);
	if(!vox1 || !vox1->type)
	{
//		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
//		vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z-1);
//		if(!vox || !vox1)return;
		
//		vox1->type=vox->type;
//		vox1->aux=(vox1->aux&0xF0)|(vox->aux&0x0F);
//		vox1->delay=2;
		
//		vox->type=0;
//		vox->aux=0;

		memset(&tvox1, 0, sizeof(BTGE_VoxelData));
		memset(&tvox2, 0, sizeof(BTGE_VoxelData));

		tvox1.type=vox->type;
		tvox1.aux=vox->aux;
		tvox1.aux2=vox->aux2;
		tvox1.delay=2;

		tvox2.type=0;
		tvox2.aux=0;
		tvox2.aux2=0;

		BTGE_Voxel_SetWorldVoxelData(world, x, y, z-1, tvox1);
		BTGE_Voxel_SetWorldVoxelData(world, x, y, z, tvox2);

//		vox1=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);

		return;
	}
}

BTGE_API void BTGE_Voxel_UpdateVoxel(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox2;
	BTGE_VoxelInfo *inf;
	int dotick;

	if(!vox->type)
		return;
	
	dotick=0;
	if(vox->delay)
	{	
//		chk->tickdirty=1;
		chk->tickthink=1;
		vox->delay--;

		if(!vox->delay)
			dotick=1;

		if(!vox->delay && BTGE_Voxel_VoxelFluidP(vox))
			chk->flags|=VOX_CHFL_OLDTICKDIRTY;
//			chk->tickdirty=1;

//		if(chk->flags&VOX_CHFL_OLDTICKDIRTY)
//			chk->tickdirty=1;

		if(vox->delay)
			return;
	}

	inf=BTGE_Voxel_GetVoxelInfo(vox);
	if(!inf)
	{
		printf("BTGE_Voxel_UpdateVoxel: "
			"No Info Type=%04X XYZ=%d,%d,%d\n",
				vox->type, x, y, z);

		//get rid of it...
		vox=BTGE_Voxel_GetWorldVoxelDirty2(world, x, y, z);
		vox->type=VOX_TY_AIR;
		return;
	}

	switch(vox->type&VOX_TY_MASK)
	{
	case VOX_TY_MOBSPAWNER:
		BTGE_Voxel_UpdateVoxel_SpawnerTick(world, chk, vox, x, y, z);
		break;
//	case VOX_TY_WATER:
//	case VOX_TY_LAVA:
//		if(chk->flags&VOX_CHFL_OLDTICKDIRTY)
//			chk->tickdirty=1;
		break;
	default:
		if(inf->BlockThink)
			inf->BlockThink(world, chk, vox, x, y, z);
		break;
	}
	
//	if(chk->tickdirty)
//	if(chk->tickdirty || (chk->flags&VOX_CHFL_OLDTICKDIRTY))

//	if(!chk->tickdirty && !(chk->flags&VOX_CHFL_OLDTICKDIRTY))
	if(!chk->tickdirty && !(chk->flags&VOX_CHFL_OLDTICKDIRTY) && !dotick)
		return;

	switch(vox->type&VOX_TY_MASK)
	{
	case VOX_TY_WATER:
		BTGE_Voxel_UpdateVoxel_FluidSource(world, chk, vox, x, y, z);
		break;
	case VOX_TY_WATER2:
		BTGE_Voxel_UpdateVoxel_FluidFlow(world, chk, vox, x, y, z);
		break;
	case VOX_TY_LAVA:
		BTGE_Voxel_UpdateVoxel_FluidSource(world, chk, vox, x, y, z);
		break;
	case VOX_TY_LAVA2:
		BTGE_Voxel_UpdateVoxel_FluidFlow(world, chk, vox, x, y, z);
		break;
	case VOX_TY_SLIME:
		BTGE_Voxel_UpdateVoxel_FluidSource(world, chk, vox, x, y, z);
		break;
	case VOX_TY_SLIME2:
		BTGE_Voxel_UpdateVoxel_FluidFlow(world, chk, vox, x, y, z);
		break;
	case VOX_TY_GLOWSLIME:
		BTGE_Voxel_UpdateVoxel_FluidSource(world, chk, vox, x, y, z);
		break;
	case VOX_TY_GLOWSLIME2:
		BTGE_Voxel_UpdateVoxel_FluidFlow(world, chk, vox, x, y, z);
		break;
//	case VOX_TY_MOBSPAWNER:
//		BTGE_Voxel_UpdateVoxel_SpawnerTick(world, chk, vox, x, y, z);
//		break;

	case VOX_TY_SAND:
		BTGE_Voxel_UpdateVoxel_Falling(world, chk, vox, x, y, z);
		break;
	case VOX_TY_GRAVEL:
		BTGE_Voxel_UpdateVoxel_Falling(world, chk, vox, x, y, z);
		break;

	case VOX_TY_REDSTONE_WIRE:
	case VOX_TY_REDSTONE_TORCH0:
	case VOX_TY_REDSTONE_TORCH1:
		BTGE_VoxelPower_UpdateVoxel(world, chk, vox, x, y, z);
		break;

	case VOX_TY_REDSTONE_LAMP0:
	case VOX_TY_REDSTONE_LAMP1:
		BTGE_VoxelPower_UpdateVoxel_RedstoneLamp(world, chk, vox, x, y, z);
		break;

	default:
		if(inf->BlockUpdate)
			inf->BlockUpdate(world, chk, vox, x, y, z);
		break;
	}

	BTGE_VoxelPower_UpdatePoweredVoxel(world, chk, vox, x, y, z);
}

BTGE_API bool BTGE_Voxel_ChunkActiveP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	//HACK: should determine if anyone is near chunk
	if(!chk->mesh)
		return(false);
	if(!chk->mesh->face_xyz && !chk->mesh->aface_xyz)
		return(false);
	return(true);
}

BTGE_API void BTGE_Voxel_UpdateChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox;
	float org[3], mins[3], maxs[3];
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l, nvox;

//	BTGE_Voxel_Setup();

//	if(!BTGE_Voxel_ChunkActiveP(world, chk))
//		return;

	if(!chk->tickactive)
		return;

	if(!chk->tickdirty && !chk->tickthink)
		return;
	
	if(chk->tickdirty)
			chk->flags|=VOX_CHFL_OLDTICKDIRTY;
	else	chk->flags&=~VOX_CHFL_OLDTICKDIRTY;
	
	chk->tickdirty=0;
	chk->tickthink=0;

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

//	chk->n_face=0;
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
		if(!vox)continue;

		BTGE_Voxel_UpdateVoxel(world, chk, vox, x, y, z);

#if 0
		if(BTGE_Voxel_VoxelOpaqueP(vox))
		{
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x-1, y, z))l|=1;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x+1, y, z))l|=2;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y-1, z))l|=4;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y+1, z))l|=8;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y, z-1))l|=16;
			if(!BTGE_Voxel_RegionVoxelOpaqueP(chk->rgn, x, y, z+1))l|=32;
			if(!l)continue;
		}else
		{
//			BTGE_Voxel_RegionVoxelSkipFaceP()

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
			if(!l)continue;
		}
#endif

//		org[0]=x*vsz; org[1]=y*vsz; org[2]=z*vsz;
//		mins[0]=org[0]; mins[1]=org[1]; mins[2]=org[2];
//		maxs[0]=org[0]+vsz; maxs[1]=org[1]+vsz; maxs[2]=org[2]+vsz;

//		BTGE_Voxel_EmitBlock(chk, vox, mins, maxs, l, x, y, z);
		
		nvox++;
	}
	
//	BTGE_Voxel_RebuildSortFaces(chk);
	
//	printf("BTGE_Voxel_RebuildChunk: %d vox, %d face %d aface\n",
//		nvox, chk->n_face, chk->n_aface);
}

BTGE_API void BTGE_Voxel_UpdateRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *chk;
	int i, j, k, l, n, m, lu;

	if(!BTGE_Voxel_CheckRegionFrustum(rgn))
		return;

//	rgn->vischk=NULL;
	n=0; m=0; lu=0;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
	{
		for(k=0; (k<rgn->zs) && (lu<2); k++)
		{
			l=rgn->zs-k-1;
			chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+l];
			if(!chk)continue;
//			BTGE_Voxel_UpdateChunkLight(world, chk);

			if(BTGE_Voxel_UpdateChunkLight(world, chk))
				{ lu++; break; }
		}

		for(k=0; k<rgn->zs; k++)
		{
			chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+k];
			if(!chk)continue;
			if(!BTGE_Voxel_CheckChunkFrustum(chk))
				continue;
			BTGE_Voxel_UpdateChunk(world, chk);
		
			m++;
//			if(!chk->n_face)
//				continue;
		
//			chk->vnext=rgn->vischk;
//			rgn->vischk=chk;
			n++;
		}
	}
	
//	printf("Visible Chunks: %d/%d\n", n, m);
}

BTGE_API void BTGE_Voxel_UpdateRegionList(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *lst)
{
	BTGE_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		BTGE_Voxel_UpdateRegion(world, cur);
		cur=cur->next;
	}
}

BTGE_API void BTGE_Voxel_ClearActiveRegionTick(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *chk;
	int i, j, k, l, n, m;

	rgn->flags&=~VOX_CHFL_DELTA_ACTIVE;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
			for(k=0; k<rgn->zs; k++)
	{
		chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+k];
		if(!chk)continue;

		if(chk->tickactive)
		{
			chk->flags|=VOX_CHFL_OLDTICKACTIVE;
			chk->tickactive=0;
			chk->flags&=~VOX_CHFL_DELTA_ACTIVE;
		}
		else
		{
			chk->flags&=~(VOX_CHFL_OLDTICKACTIVE|VOX_CHFL_DELTA_ACTIVE);
		}

//		if(chk->tickactive)
//				{ chk->flags|=VOX_CHFL_OLDTICKACTIVE; }
//		else	{ chk->flags&=~VOX_CHFL_OLDTICKACTIVE; }
//		chk->tickactive=0;
//		chk->flags&=~VOX_CHFL_DELTA_ACTIVE;
	}
}

BTGE_API void BTGE_Voxel_SetActiveRegionTick(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	float *org)
{
	float tv[3];
	BTGE_VoxelChunk *chk;
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	float cd, vsc;
	int x, y, z;
	int i, j, k, l, cn, n, m;

	vsc=1;
	if(gcCheckMemoryLow())
		vsc=vsc*0.33;

	tv[0]=(rgn->ox+((rgn->xs/2)*rgn->xst)+(rgn->xst/2))*32;
	tv[1]=(rgn->oy+((rgn->ys/2)*rgn->yst)+(rgn->yst/2))*32;
	tv[2]=org[2];

	cd=V3F_DIST(org, tv);
	if(cd>(24576*vsc))
	{
		rgn->rgn_tick_inactive++;
		return;
	}

	rgn->flags|=VOX_CHFL_DELTA_ACTIVE;
	rgn->rgn_tick_inactive=0;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
	{
#if 1
		tv[0]=(rgn->ox+(i*rgn->xst)+(rgn->xst/2))*32;
		tv[1]=(rgn->oy+(j*rgn->yst)+(rgn->yst/2))*32;
		tv[2]=org[2];
		
		cd=V3F_DIST(org, tv);
		if(cd>(8192*vsc))
		{
			cn=(i*rgn->ys+j)*rgn->zs+0;
			for(k=0; k<rgn->zs; k++)
			{
//				cn=(i*rgn->ys+j)*rgn->zs+k;
				chk=rgn->chk[cn];
				if(chk)
				{
					BTGE_Voxel_FreeChunk(world, chk);
					rgn->chk[cn]=NULL;
				}
				cn++;
			}
			continue;
		}
#endif

		for(k=0; k<rgn->zs; k++)
		{
			cn=(i*rgn->ys+j)*rgn->zs+k;
			chk=rgn->chk[cn];

#if 1
			tv[0]=(rgn->ox+(i*rgn->xst)+(rgn->xst/2))*32;
			tv[1]=(rgn->oy+(j*rgn->yst)+(rgn->yst/2))*32;
			tv[2]=(rgn->oz+(k*rgn->zst)+(rgn->zst/2))*32;
		
			cd=V3F_DIST(org, tv);
//			if(cd>12288)
			if(cd>(8192*vsc))
			{
				if(!chk)continue;
		
				if(chk)
				{
					BTGE_Voxel_FreeChunk(world, chk);
					rgn->chk[cn]=NULL;
				}
			
				continue;
			}
#endif

			if(!chk && (cd>(6144*vsc)))
			{
				continue;
			}

			if(!chk && rgn->chk_offs && rgn->chk_offs[cn])
			{
				BTGE_VoxelSave_LoadRegionChunk(world, rgn, cn);
				chk=rgn->chk[cn];
			}

			if(!chk)
			{			
				if(k)continue;
				if(!k && !rgn->oz)
				{
#if 0
					//determine if too far away
					tv[0]=(rgn->ox+i*rgn->xst)*32;
					tv[1]=(rgn->oy+j*rgn->yst)*32;
//					tv[2]=(rgn->oz+k*rgn->zst)*32;
					tv[2]=0;
					if(V3F_DIST(org, tv)>8192)
						continue;

					//try to generate spans
					BTGE_Voxel_GenerateFillRange(world,
						rgn->ox+i*rgn->xst, rgn->oy+j*rgn->yst,
						rgn->xst, rgn->yst);
#endif

					if(rgn->chk_offs[cn])
						continue;

					BTGE_Voxel_CheckGenerateFillChucks(
						world, rgn, i, j, org, 0);

					chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+k];
					if(!chk)continue;
				}
//				continue;
				if(!chk)continue;
			}
//			chk->tickactive=0;

			tv[0]=(chk->ox+(chk->xs/2))*32;
			tv[1]=(chk->oy+(chk->ys/2))*32;
			tv[2]=(chk->oz+(chk->zs/2))*32;

			cd=V3F_DIST(org, tv);
//			if(cd<1600)
			if(cd<(2048*vsc))
			{
				if(!(chk->flags&VOX_CHFL_OLDTICKACTIVE))
				{
//					chk->tickdirty=1;
					chk->tickthink=1;
				}
				chk->tickactive=1;

#if 1
//				if(!(rand()&255))
//				if(!(rand()&63))
//				if(!(rand()&15))
				if(!(rand()&3))
				{
					x=chk->ox+(rand()&(chk->xs-1));
					y=chk->oy+(rand()&(chk->ys-1));
					z=chk->oz+(rand()&(chk->zs-1));
//					vox=BTGE_Voxel_GetRegionVoxel(rgn, x, y, z);
					vox=BTGE_Voxel_GetWorldRegionVoxel(world, rgn, x, y, z);
					inf=BTGE_Voxel_GetVoxelInfo(vox);
					
					if(inf && inf->RandomThink)
					{
						inf->RandomThink(world, *vox, x, y, z, 0);
					}
				}
#endif
			}
			if(cd<(6144*vsc))
			{
				chk->flags|=VOX_CHFL_DELTA_ACTIVE;
			}
		}
	}
}

BTGE_API void BTGE_Voxel_UpdateWorldTick(
	BTGE_BrushWorld *world, float dt)
{
	BTGE_Voxel_UpdateRegionList(world, world->vox_region);

//	BTGE_Voxel_FlattenWorld(world);
	BTGE_Voxel_SaveWorld(world);
}

BTGE_API void BTGE_Voxel_ClearActiveWorldTick(
	BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur;

	cur=world->vox_region;
	while(cur)
	{
		BTGE_Voxel_ClearActiveRegionTick(world, cur);
		cur=cur->next;
	}
	
	BTGE_Voxel_CheckUnloadRegions(world);
}

BTGE_API void BTGE_Voxel_SetActiveWorldTick(
	BTGE_BrushWorld *world, vec3 org)
{
	BTGE_VoxelRegion *cur;
	float tv[3];
	int x, y, z;

	vfvec3(tv, org);

	x=tv[0]/32; y=tv[1]/32; z=tv[2]/32;
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x-64, y, z, 0);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x+64, y, z, 0);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y-64, z, 0);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x, y+64, z, 0);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x-64, y-64, z, 0);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x+64, y-64, z, 0);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x-64, y+64, z, 0);
	BTGE_Voxel_CheckGenerateRegionForPoint(world, x+64, y+64, z, 0);

	cur=world->vox_region;
	while(cur)
	{
		BTGE_Voxel_SetActiveRegionTick(world, cur, tv);
		cur=cur->next;
	}
}

BTGE_API void BTGE_Voxel_WorldEntityMoveTick(
	BTGE_BrushWorld *world, vec3d oldorg, vec3d neworg)
{
	int x0, y0, z0;
	int x1, y1, z1;

	x0=floor(v3dx(oldorg)*(1.0/(16*32)));
	y0=floor(v3dy(oldorg)*(1.0/(16*32)));
	z0=floor(v3dz(oldorg)*(1.0/(16*32)));

	x1=floor(v3dx(neworg)*(1.0/(16*32)));
	y1=floor(v3dy(neworg)*(1.0/(16*32)));
	z1=floor(v3dz(neworg)*(1.0/(16*32)));
		
	if((x0!=x1) || (y0!=y1) || (z0!=z1))
	{
		BTGE_Voxel_GetWorldVoxelFlags(world,
			x0*16, y0*16, z0*16, VOX_GFL_ENTITY_MOVE);
		BTGE_Voxel_GetWorldVoxelFlags(world,
			x1*16, y1*16, z1*16, VOX_GFL_ENTITY_MOVE);
	}
}

BTGE_API int BTGE_Voxel_SmallDamageEvent(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z, double *org,
	float radius, float damage, float dmg)
{
	BTGE_VoxelInfo *inf;

	if(((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH0) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_REDSTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GREENSTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_BLUESTONE_TORCH1) ||
		((vox->type&VOX_TY_MASK)==VOX_TY_GOLDSTONE_TORCH1))
	{
//		BTGE_VoxelPower_UpdateVoxel_RedstoneTorch1(world, chk, vox, x, y, z);

		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		switch(vox->type&VOX_TY_MASK)
		{
		case VOX_TY_REDSTONE_TORCH0:
			vox->type=VOX_TY_REDSTONE_TORCH1; break;
		case VOX_TY_GREENSTONE_TORCH0:
			vox->type=VOX_TY_GREENSTONE_TORCH1; break;
		case VOX_TY_BLUESTONE_TORCH0:
			vox->type=VOX_TY_BLUESTONE_TORCH1; break;
		case VOX_TY_GOLDSTONE_TORCH0:
			vox->type=VOX_TY_GOLDSTONE_TORCH1; break;

		case VOX_TY_REDSTONE_TORCH1:
			vox->type=VOX_TY_REDSTONE_TORCH0; break;
		case VOX_TY_GREENSTONE_TORCH1:
			vox->type=VOX_TY_GREENSTONE_TORCH0; break;
		case VOX_TY_BLUESTONE_TORCH1:
			vox->type=VOX_TY_BLUESTONE_TORCH0; break;
		case VOX_TY_GOLDSTONE_TORCH1:
			vox->type=VOX_TY_GOLDSTONE_TORCH0; break;
		}

		return(0);
	}

	if((vox->type&VOX_TY_MASK)==VOX_TY_LEVER)
	{
		vox=BTGE_Voxel_GetWorldVoxelDirty(world, x, y, z);
		vox->aux^=8;
		return(0);
	}

	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
	if(inf && inf->SmallDamageEvent)
	{
		return(inf->SmallDamageEvent(world, vox, x, y, z,
			org, radius, damage, dmg));
	}
	
	return(-1);
}

BTGE_API int BTGE_Voxel_LargeDamageEvent(
	BTGE_BrushWorld *world, BTGE_VoxelData *vox,
	int x, int y, int z, double *org,
	float radius, float damage, float dmg)
{
	BTGE_VoxelInfo *inf;

	inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
	if(inf && inf->LargeDamageEvent)
	{
		return(inf->LargeDamageEvent(world, vox, x, y, z,
			org, radius, damage, dmg));
	}
	
	return(-1);
}


BTGE_API bool BTGE_Voxel_FindNearestBlockType(
	BTGE_BrushWorld *world,
	int sx, int sy, int sz, int *rx, int *ry, int *rz,
	int voxty, int dist)
{
	BTGE_VoxelData *vox;
	int bx, by, bz, bd;
	int x, y, z;
	int i, j, k;

	if(dist>8)
	{
		i=BTGE_Voxel_FindNearestBlockType(world,
				sx, sy, sz, rx, ry, rz, voxty, dist>>1);
		if(i)return(i);
	}

	bd=3*(dist*dist);
	for(z=sz-dist; z<=(sz+dist); z++)
		for(y=sy-dist; y<=(sy+dist); y++)
			for(x=sx-dist; x<=(sx+dist); x++)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
		if((!voxty && !vox) ||
			(vox && ((vox->type&VOX_TY_MASK)==voxty)) ||
			((voxty==VOX_TYS_ANYSOLIDBLOCK) &&
				(BTGE_Voxel_VoxelOpaqueP(vox)))
			)
		{
			i=(sx-x); k=i*i;
			i=(sy-y); k+=i*i;
			i=(sz-z); k+=i*i;
			if(k<bd)
			{ bx=x; by=y; bz=z; bd=k; }
		}
	}
	
	if(bd>=(dist*dist))
		return(false);
		
	if(rx)*rx=bx;
	if(ry)*ry=by;
	if(rz)*rz=bz;
	return(true);
}

BTGE_API bool BTGE_Voxel_FindNearestSpawnBlockType(
	BTGE_BrushWorld *world,
	int sx, int sy, int sz, int *rx, int *ry, int *rz,
	int voxty, int dist)
{
	BTGE_VoxelData *vox;
	int x, y, z;
	int i;
	
	if(!world->vox_region)
		return(false);

	if(rx)*rx=sx;
	if(ry)*ry=sy;
	if(rz)*rz=sz;
	
	x=sx; y=sy; z=sz;
	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
	if(vox)
	{
		if((vox->type&VOX_TY_MASK)==voxty)
		{
			//convinient
			if(rx)*rx=x;
			if(ry)*ry=y;
			if(rz)*rz=z;
			return(true);
		}
		
		//try to find open air
		if((vox->type&VOX_TY_MASK) &&
			!BTGE_Voxel_FindNearestBlockType(world,
				x, y, z, &x, &y, &z, 0, dist))
		{
			return(false);
		}
	}
	
	//fall and search for a solid block
	while(z>0)
	{
		vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
		if(vox && (vox->type&VOX_TY_MASK))
			break;
		z--;
	}
	
	if(voxty==VOX_TYS_ANYSOLIDBLOCK)
	{
		if(BTGE_Voxel_VoxelOpaqueP(vox))
		{
			if(rx)*rx=x;
			if(ry)*ry=y;
			if(rz)*rz=z;
			return(true);
		}
	}
	
	//search for the block
	i=BTGE_Voxel_FindNearestBlockType(world,
		x, y, z, rx, ry, rz, voxty, dist);
	return(i);
}
