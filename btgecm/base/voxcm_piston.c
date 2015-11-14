#include <btgecm.h>

BTGE_API void BTGE_VoxelPiston_EmitBlock(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
//		4, 6, 7, 5,	//+Z
		6, 7, 5, 4, //+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};
	static int sti[6*2]= { 1, 2, 1, 2, 0, 2, 0, 2, 0, 1, 0, 1};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float stb[8];
	float vtxa[8*3];
	float vgam[6];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4];
	int tex[6], pos[3];
	BTGE_VoxelInfo *inf;
	BTGE_VoxelData *vox1;
	float *pt1, *pt2, *pt3, *pt4;
	float cr, cg, cb;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	if((vox->type&VOX_TY_MASK)==VOX_TY_PISTON_HEAD)
	{
		BTGE_VoxelPiston_EmitBlock_PistonHead(world, chk, vox,
			min, max, flag, x, y, z);
		return;
	}
	
	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;

	tex[0]=inf->img_side;
	tex[1]=inf->img_side;
	tex[2]=inf->img_side;
	tex[3]=inf->img_side;
	tex[4]=inf->img_side;
	tex[5]=inf->img_side;

//	tex[4]=inf->img_top;
//	tex[5]=inf->img_bot;

	if(!(vox->aux&8))
		switch(vox->aux&7)
	{
	case VOX_AXFL_PISTON_DOWN:		tex[4]=inf->img_top; break;
	case VOX_AXFL_PISTON_UP:		tex[5]=inf->img_top; break;
	case VOX_AXFL_PISTON_NORTH:		tex[3]=inf->img_top; break;
	case VOX_AXFL_PISTON_SOUTH:		tex[2]=inf->img_top; break;
	case VOX_AXFL_PISTON_WEST:		tex[0]=inf->img_top; break;
	case VOX_AXFL_PISTON_EAST:		tex[1]=inf->img_top; break;
	default: break;
	}

	pos[0]=x; pos[1]=y; pos[2]=z;

	f=BTGE_Voxel_GetVoxelGamma(chk, vox);
	for(i=0; i<6; i++)
	{
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn,
			x+nva[i*4+0], y+nva[i*4+1], z+nva[i*4+2]);
		f=BTGE_Voxel_GetVoxelGamma(chk, vox1);

		vgam[i]=f;
	}

//	f=(vox->aux&0xF0)/240.0;
	f=1;
	g=1;

	cr=1; cg=1; cb=1;

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
		V4F_COPY(sta+0, stb+0);
		V4F_COPY(sta+4, stb+4);

		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=1;

//		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		f=vgam[i];
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		V3F_SUB(pt3, pt4, tv0);
		V3F_SUB(pt2, pt3, tv1);
		V3F_CROSS(tv0, tv1, tv2);

		V3F_SUB(pt2, pt3, tv0);
		V3F_SUB(pt1, pt2, tv1);
		V3F_CROSS(tv0, tv1, tv3);
		
		V3F_ADD(tv2, tv3, tv0);
		V3F_NORMALIZE(tv0, nv);

		BTGE_Voxel_EmitFace2(chk, vox,
			pt4, stb+6, nv, brgba, slrgba,
			pt3, stb+4, nv, brgba, slrgba,
			pt2, stb+2, nv, brgba, slrgba,
			pt1, stb+0, nv, brgba, slrgba,
			tex[i]);
	}
}

BTGE_API void BTGE_VoxelPiston_EmitBlock_PistonHead(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	float mid[3], min2[3], max2[3];
	float rgba[4], nv[4];
	BTGE_VoxelInfo *inf;
	float f, g, h;
	int tex1, tex2, fl2;
	int i, j;

	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	
	h=(mid[2]+max[2])*0.5;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;
	tex1=inf->img_side;
	tex2=inf->img_top;

	if(vox->aux&8)
		tex2=inf->img_bot;

	switch(vox->aux&7)
	{
	case VOX_AXFL_PISTON_EAST:
		min2[0]=min[0]; min2[1]=mid[1]-8; min2[2]=mid[2]-8;
		max2[0]=mid[0]; max2[1]=mid[1]+8; max2[2]=mid[2]+8;
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex1);
		min2[0]=mid[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex2);
		break;
	case VOX_AXFL_PISTON_WEST:
		min2[0]=mid[0]; min2[1]=mid[1]-8; min2[2]=mid[2]-8;
		max2[0]=max[0]; max2[1]=mid[1]+8; max2[2]=mid[2]+8;
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex1);
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=mid[0]; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex2);
		break;
	case VOX_AXFL_PISTON_SOUTH:
		min2[0]=mid[0]-8; min2[1]=mid[1]; min2[2]=mid[2]-8;
		max2[0]=mid[0]+8; max2[1]=max[1]; max2[2]=mid[2]+8;
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex1);
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=mid[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex2);
		break;
	case VOX_AXFL_PISTON_NORTH:
		min2[0]=mid[0]-8; min2[1]=min[1]; min2[2]=mid[2]-8;
		max2[0]=mid[0]+8; max2[1]=mid[1]; max2[2]=mid[2]+8;
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex1);
		min2[0]=min[0]; min2[1]=mid[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex2);
		break;

	case VOX_AXFL_PISTON_DOWN:
		min2[0]=mid[0]-8; min2[1]=mid[1]-8; min2[2]=mid[2];
		max2[0]=mid[0]+8; max2[1]=mid[1]+8; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex1);
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=min[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=mid[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex2);
		break;
	case VOX_AXFL_PISTON_UP:
		min2[0]=mid[0]-8; min2[1]=mid[1]-8; min2[2]=min[2];
		max2[0]=mid[0]+8; max2[1]=mid[1]+8; max2[2]=mid[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex1);
		min2[0]=min[0]; min2[1]=min[1]; min2[2]=mid[2];
		max2[0]=max[0]; max2[1]=max[1]; max2[2]=max[2];
		BTGE_Voxel_EmitMaterialTextureCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE, tex2);
		break;
	default:
		break;
	}
}

BTGE_API void BTGE_VoxelPiston_UpdateVoxel_Piston(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	BTGE_VoxelData *vox1, *vox2;
	int pos[3], dir[3];
	int pwr;
	int i, j, k;

	pwr=BTGE_VoxelPower_GetBlockAmbientPower(world, chk, vox, x, y, z);

	pos[0]=x; pos[1]=y; pos[2]=z;
	dir[0]=0; dir[1]=0; dir[2]=0;
	switch(vox->aux&7)
	{
	case VOX_AXFL_PISTON_DOWN:		pos[2]--; dir[2]=-1; break;
	case VOX_AXFL_PISTON_UP:		pos[2]++; dir[2]= 1; break;
	case VOX_AXFL_PISTON_NORTH:		pos[1]++; dir[1]= 1; break;
	case VOX_AXFL_PISTON_SOUTH:		pos[1]--; dir[1]=-1; break;
	case VOX_AXFL_PISTON_WEST:		pos[0]--; dir[0]=-1; break;
	case VOX_AXFL_PISTON_EAST:		pos[0]++; dir[0]= 1; break;
	default: break;
	}

	if((pwr>0) && !(vox->aux&8))
	{
		for(i=0; i<12; i++)
		{
			vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn,
				pos[0]+dir[0]*i, pos[1]+dir[1]*i, pos[2]+dir[2]*i);
			if(!(vox1->type&VOX_TY_MASK))
				break;
		}
		
		if(i>=12)return;

		for(j=i; j>0; j--)
		{
			k=j-1;
			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn,
				pos[0]+dir[0]*j, pos[1]+dir[1]*j, pos[2]+dir[2]*j);
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn,
				pos[0]+dir[0]*k, pos[1]+dir[1]*k, pos[2]+dir[2]*k);
			(*vox1)=(*vox2);
		}
	
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux|=8;
		
		vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn,
			pos[0], pos[1], pos[2]);
		vox1->type=VOX_TY_PISTON_HEAD;
		vox1->aux=vox->aux&0xF7;
		vox1->aux2=0;
		
		if((vox->type&VOX_TY_MASK)==VOX_TY_STICKY_PISTON)
			vox1->aux|=8;

		return;
	}

	if((pwr<=0) && (vox->aux&8))
	{
		vox=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn, x, y, z);
		vox->aux&=~8;

		if((vox->type&VOX_TY_MASK)==VOX_TY_STICKY_PISTON)
		{
			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn,
				pos[0], pos[1], pos[2]);
			vox2=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn,
				pos[0]+dir[0], pos[1]+dir[1], pos[2]+dir[2]);
			(*vox1)=(*vox2);
			vox2->type=0;
			vox2->aux=vox2->aux&0xF0;
			vox2->aux2=0;
		}else
		{
			vox1=BTGE_Voxel_GetWorldRegionVoxelDirty(world, chk->rgn,
				pos[0], pos[1], pos[2]);
			vox1->type=0;
			vox1->aux=vox1->aux&0xF0;
			vox1->aux2=0;
		}
	}
}

BTGE_API void BTGE_VoxelPiston_UpdateVoxel(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z)
{
	if((vox->type&VOX_TY_MASK)==VOX_TY_PISTON)
	{
		BTGE_VoxelPiston_UpdateVoxel_Piston(world, chk, vox, x, y, z);
		return;
	}

	if((vox->type&VOX_TY_MASK)==VOX_TY_STICKY_PISTON)
	{
		BTGE_VoxelPiston_UpdateVoxel_Piston(world, chk, vox, x, y, z);
		return;
	}
}
