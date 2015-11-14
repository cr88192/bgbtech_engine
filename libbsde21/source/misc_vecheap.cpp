/*

Vector Heap

basically, this will be for special allocatord intended for allocating vectors.
Each vector will be 4-elements, either integer or bsde_real.

Small arrays may also be allocated via this mechanism.

A byte-map will be used for managing the allocation state of each vector.
ent&7: 0=free, 1=single, 2=array-start, 3=array-body

*/

#include "bsde_internal.h"

typedef unsigned char byte;

static int	**bsde_vecheap_idata;
static bsde_real	**bsde_vecheap_fdata;
static byte	**bsde_vecheap_imask;
static byte	**bsde_vecheap_fmask;

static int	bsde_vecheap_niblk;
static int	bsde_vecheap_nfblk;

static int	bsde_vecheap_irovi, bsde_vecheap_irovj;
static int	bsde_vecheap_frovi, bsde_vecheap_frovj;


void BSDE_VecHeap_Init()
{
	static int init=0;

	if(init)return;

	bsde_vecheap_idata=(int **)malloc(4096*sizeof(int *));
	bsde_vecheap_fdata=(bsde_real **)malloc(4096*sizeof(bsde_real *));

	bsde_vecheap_imask=(byte **)malloc(4096*sizeof(byte *));
	bsde_vecheap_fmask=(byte **)malloc(4096*sizeof(byte *));

	bsde_vecheap_niblk=0;
	bsde_vecheap_nfblk=0;
}

void BSDE_VecHeap_NewIBlock()
{
	int i;

	i=bsde_vecheap_niblk++;
	bsde_vecheap_idata[i]=(int *)malloc(16384*4*sizeof(int));
	bsde_vecheap_imask[i]=(byte *)malloc(16384);
	memset(bsde_vecheap_imask[i], 0, 16384);
}

void BSDE_VecHeap_NewFBlock()
{
	int i;

	i=bsde_vecheap_nfblk++;
	bsde_vecheap_fdata[i]=(bsde_real *)malloc(16384*4*sizeof(bsde_real));
	bsde_vecheap_fmask[i]=(byte *)malloc(16384);
	memset(bsde_vecheap_fmask[i], 0, 16384);
}

int BSDE_VecHeap_AllocICellBlk(int blk)
{
	int i;

	for(i=bsde_vecheap_irovj; i<16384; i++)
		if(!bsde_vecheap_imask[blk][i])
	{
		bsde_vecheap_imask[blk][i]=1;
		bsde_vecheap_irovj=i+1;
		return(i);
	}
	return(-1);
}

int BSDE_VecHeap_AllocFCellBlk(int blk)
{
	int i;

	for(i=bsde_vecheap_frovj; i<16384; i++)
		if(!bsde_vecheap_fmask[blk][i])
	{
		bsde_vecheap_fmask[blk][i]=1;
		bsde_vecheap_frovj=i+1;
		return(i);
	}
	return(-1);
}

int BSDE_VecHeap_AllocICellsBlk(int blk, int sz)
{
	int i, j;

	for(i=bsde_vecheap_irovj; i<16384; i++)if(!bsde_vecheap_imask[blk][i])
	{
		for(j=1; j<sz; j++)if(bsde_vecheap_imask[blk][i+j])break;
		if(j<sz) { i+=j-1; continue; }
		for(j=1; j<sz; j++)bsde_vecheap_imask[blk][i+j]=3;
		bsde_vecheap_imask[blk][i]=2;
		bsde_vecheap_irovj=i+sz;
		return(i);
	}
	return(-1);
}

int BSDE_VecHeap_AllocFCellsBlk(int blk, int sz)
{
	int i, j;

	for(i=bsde_vecheap_frovj; i<16384; i++)if(!bsde_vecheap_fmask[blk][i])
	{
		for(j=1; j<sz; j++)if(bsde_vecheap_fmask[blk][i+j])break;
		if(j<sz) { i+=j-1; continue; }
		for(j=1; j<sz; j++)bsde_vecheap_fmask[blk][i+j]=3;
		bsde_vecheap_fmask[blk][i]=2;
		bsde_vecheap_frovj=i+1;
		return(i);
	}
	return(-1);
}


int *BSDE_VecHeap_TryAllocInt(int cnt)
{
	int *v;
	int i, j, n;

	n=(cnt+3)/4;
	for(i=bsde_vecheap_irovi; i<bsde_vecheap_niblk; i++)
	{
		j=BSDE_VecHeap_AllocICellsBlk(i, n);
		if(j>=0)
		{
			v=bsde_vecheap_idata[i]+j*4;
			memset(v, 0, n*4*sizeof(int));
			bsde_vecheap_irovi=i;
			bsde_vecheap_irovj=j+n;
			return(v);
		}
		bsde_vecheap_irovj=0;
	}
	bsde_vecheap_irovi=0;
	bsde_vecheap_irovj=0;
	return((int *)NULL);
}

bsde_real *BSDE_VecHeap_TryAllocFloat(int cnt)
{
	bsde_real *v;
	int i, j, n;

	n=(cnt+3)/4;
	for(i=bsde_vecheap_frovi; i<bsde_vecheap_nfblk; i++)
	{
		j=BSDE_VecHeap_AllocFCellsBlk(i, n);
		if(j>=0)
		{
			v=bsde_vecheap_fdata[i]+j*4;
			memset(v, 0, n*4*sizeof(bsde_real));
			bsde_vecheap_frovi=i;
			bsde_vecheap_frovj=j+n;
			return(v);
		}
		bsde_vecheap_frovj=0;
	}
	bsde_vecheap_frovi=0;
	bsde_vecheap_frovj=0;
	return((bsde_real *)NULL);
}

int *BSDE_VecHeap_AllocInt(int cnt)
{
	int *v;
	BSDE_VecHeap_Init();
	v=BSDE_VecHeap_TryAllocInt(cnt); if(v)return(v);
	v=BSDE_VecHeap_TryAllocInt(cnt); if(v)return(v);
	BSDE_VecHeap_NewIBlock();
	v=BSDE_VecHeap_TryAllocInt(cnt);
	return(v);
}

bsde_real *BSDE_VecHeap_AllocFloat(int cnt)
{
	bsde_real *v;
	BSDE_VecHeap_Init();
	v=BSDE_VecHeap_TryAllocFloat(cnt); if(v)return(v);
	v=BSDE_VecHeap_TryAllocFloat(cnt); if(v)return(v);
	BSDE_VecHeap_NewFBlock();
	v=BSDE_VecHeap_TryAllocFloat(cnt);
	return(v);
}

void BSDE_VecHeap_FreeICellsBlk(int blk, int idx)
{
	if(!(bsde_vecheap_imask[blk][idx]&7))return;
	if((bsde_vecheap_imask[blk][idx]&7)==1)
		{ bsde_vecheap_imask[blk][idx]=0; return; }
	while((idx>0) && ((bsde_vecheap_imask[blk][idx]&7)==3))idx--;
	if((bsde_vecheap_imask[blk][idx]&7)==2)
	{
		bsde_vecheap_imask[blk][idx]=0; idx++;
		while((idx<16384) && ((bsde_vecheap_imask[blk][idx]&7)==3))
			{ bsde_vecheap_imask[blk][idx]=0; idx++; }
		return;
	}
}

void BSDE_VecHeap_FreeFCellsBlk(int blk, int idx)
{
	if(!(bsde_vecheap_fmask[blk][idx]&7))return;
	if((bsde_vecheap_fmask[blk][idx]&7)==1)
		{ bsde_vecheap_fmask[blk][idx]=0; return; }
	while((idx>0) && ((bsde_vecheap_fmask[blk][idx]&7)==3))idx--;
	if((bsde_vecheap_fmask[blk][idx]&7)==2)
	{
		bsde_vecheap_fmask[blk][idx]=0; idx++;
		while((idx<16384) && ((bsde_vecheap_fmask[blk][idx]&7)==3))
			{ bsde_vecheap_fmask[blk][idx]=0; idx++; }
		return;
	}
}

void BSDE_VecHeap_Free(void *v)
{
	int i, j;

	for(i=0; i<bsde_vecheap_nfblk; i++)
	{
		j=(((bsde_real *)v)-bsde_vecheap_fdata[i])/4;
		if((j>=0) && (j<16384))
		{
			BSDE_VecHeap_FreeFCellsBlk(i, j);
			return;
		}
	}

	for(i=0; i<bsde_vecheap_niblk; i++)
	{
		j=(((int *)v)-bsde_vecheap_idata[i])/4;
		if((j>=0) && (j<16384))
		{
			BSDE_VecHeap_FreeICellsBlk(i, j);
			return;
		}
	}
}

