#include <btgecm.h>

BTGE_API bool BTGE_Voxel_CheckRepackChunkVoxelIndex(
	BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData tarr[256];
	byte ridx[256];
	int i, j, n, n2;
	
	return(false);
	
	n=0;
	for(i=0; i<chk->pidx_n_vox; i++)
	{
		for(j=0; j<n; j++)
		{
			if(!memcmp(chk->pidx_vox+i, tarr+j, sizeof(BTGE_VoxelData)))
				break;
		}
		if(j<n)
			{ ridx[i]=j; continue; }
		j=n++;
		tarr[j]=chk->pidx_vox[i];
		ridx[i]=j;
	}
	
	for(i=0; i<n; i++)
		{ chk->pidx_vox[i]=tarr[i]; }
	chk->pidx_n_vox=n;

	n2=chk->xs*chk->ys*chk->zs;
	for(i=0; i<n2; i++)
	{
		chk->pidx_idx[i]=ridx[chk->pidx_idx[i]];
	}
	
	return(true);
}

BTGE_API bool BTGE_Voxel_CheckRegionChunkExpandVoxelIndex(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	BTGE_VoxelChunk *chk)
{
	int i;

	if((chk->pidx_n_vox+1)<chk->pidx_m_vox)
		return(true);
	if(chk->pidx_m_vox>=252)
	{
		BTGE_Voxel_CheckRepackChunkVoxelIndex(chk);
		if((chk->pidx_n_vox+1)<chk->pidx_m_vox)
			return(true);
		return(false);
	}

	if(chk->pidx_m_vox>=64)
	{
		BTGE_Voxel_CheckRepackChunkVoxelIndex(chk);
		if((chk->pidx_n_vox+1)<chk->pidx_m_vox)
			return(true);
	}
		
	i=chk->pidx_m_vox+(chk->pidx_m_vox>>1);
	if(i>=255)i=255;
	chk->pidx_vox=gcrealloc(chk->pidx_vox,
		i*sizeof(BTGE_VoxelData));
	if(!chk->pidx_vox) { *(int *)-1=-1; }
	chk->pidx_m_vox=i;
	return(true);
}

BTGE_API bool BTGE_Voxel_CheckChunkExpandVoxelIndex(
	BTGE_VoxelChunk *chk)
{
	return(BTGE_Voxel_CheckRegionChunkExpandVoxelIndex(
		NULL, NULL, chk));
}

BTGE_API bool BTGE_Voxel_CheckSetupChunkVoxelIndex(
	BTGE_VoxelChunk *chk, int szidx)
{
	int i, j;

	if(!chk->pidx_vox)
	{
		j=szidx; i=8;
		while(i<=j)i=i+(i>>1);
		if(i>=255)i=255;
		chk->pidx_vox=gctatomic("btge_voxeldata_t",
			i*sizeof(BTGE_VoxelData));
//		chk->pidx_n_vox=j;
		chk->pidx_m_vox=i;
	}

	if(szidx>=chk->pidx_m_vox)
	{
		j=szidx; i=8;
		while(i<=j)i=i+(i>>1);
		if(i>=255)i=255;
		chk->pidx_vox=gcrealloc(chk->pidx_vox,
			i*sizeof(BTGE_VoxelData));
//		chk->pidx_n_vox=j;
		chk->pidx_m_vox=i;
	}

	if(!chk->pidx_idx)
	{
		i=chk->xs*chk->ys*chk->zs;
		chk->pidx_idx=gctatomic("btge_voxelindex_t",
			i*sizeof(byte));
	}
	
	return(true);
}

BTGE_API int BTGE_Voxel_CheckChunkIndexVoxel(
	BTGE_VoxelChunk *chk, BTGE_VoxelData vox)
{
	int i;

	for(i=0; i<chk->pidx_n_vox; i++)
	{
		if(!memcmp(chk->pidx_vox+i, &vox, sizeof(BTGE_VoxelData)))
			return(i);
	}

	if(BTGE_Voxel_CheckChunkExpandVoxelIndex(chk))
	{
		i=chk->pidx_n_vox++;
		chk->pidx_vox[i]=vox;
		return(i);
	}

	return(-1);
}

BTGE_API bool BTGE_Voxel_PackVoxelIndexChunk(
	BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData tarr[256];
	byte pidx[16*16*16];
	BTGE_VoxelData vox;
	int i, j, k, n, nv;

	if(!chk->vox)return(false);
	if(chk->pidx_idx || chk->pidx_vox)
		return(false);
	if(chk->flags&VOX_CHFL_NOPACKINDEX)
		return(false);

	n=chk->xs*chk->ys*chk->zs; nv=0;
	for(i=0; i<n; i++)
	{
		vox=chk->vox[i];
		for(j=0; j<nv; j++)
		{
			if(!memcmp(&vox, tarr+j, sizeof(BTGE_VoxelData)))
				break;
		}
		if(j<nv)
		{
			pidx[i]=j;
			continue;
		}

//		if(nv<252)
		if(nv<64)
		{
			j=nv++;
			tarr[j]=vox;
			pidx[i]=j;
			continue;
		}else
		{
			break;
		}

//		j=BTGE_Voxel_CheckChunkIndexVoxel(chk, chk->vox[i]);
//		if(j<0)break;
//		chk->pidx_idx[i]=j;
	}

	if(i<n)
	{
		chk->flags|=VOX_CHFL_NOPACKINDEX;
		return(false);
	}

	BTGE_Voxel_CheckSetupChunkVoxelIndex(chk, nv);

	for(i=0; i<nv; i++)
		{ chk->pidx_vox[i]=tarr[i]; }
	for(i=0; i<n; i++)
		{ chk->pidx_idx[i]=pidx[i]; }
	chk->pidx_n_vox=nv;

	gcfree(chk->vox);
	chk->vox=NULL;

	chk->flags|=VOX_CHFL_PACKINDEX;
	return(true);
}

BTGE_API bool BTGE_Voxel_UnpackVoxelIndexChunk(
	BTGE_VoxelChunk *chk)
{
	int i, j, k, n;

	if(chk->vox)return(false);

	if(!chk->pidx_idx || !chk->pidx_vox)
		return(false);

	n=chk->xs*chk->ys*chk->zs;
	chk->vox=gctatomic("btge_voxeldata_t",
		n*sizeof(BTGE_VoxelData));
		
	n=chk->xs*chk->ys*chk->zs;
	for(i=0; i<n; i++)
	{
		j=chk->pidx_idx[i];
		chk->vox[i]=chk->pidx_vox[j];
	}
	
	gcfree(chk->pidx_idx);
	gcfree(chk->pidx_vox);
	chk->pidx_idx=NULL;
	chk->pidx_vox=NULL;
	
	chk->flags&=~VOX_CHFL_PACKINDEX;
	return(true);
}
