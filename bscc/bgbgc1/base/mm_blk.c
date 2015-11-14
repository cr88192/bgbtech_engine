/*

block-and-freelist based allocator.
this will handle larger objects (leaving the cell allocator primarily to
smaller objects).

this will also serve as a write-barrier protected alternative to malloc.

free list sizes:
free list sizes will go up by a factor of 25% each time.
list 0 will have a size of 256 bytes.

thus, the max size of each free list is thus:
256*1.25^i

 */

BGBGC_BlockInfo *mblk_info;	//array of info structs
int mblk_num_info;		//size of info array

int mblk_info_free;		//first free info
int mblk_info_used;		//first used info
int mblk_free[64];

void MBLK_Init()
{
	static int init=0;
	int i;

	if(init)return;
	init=1;

	mblk_info_free=-1;
	mblk_info_used=-1;
	for(i=0; i<64; i++)mblk_free[i]=-1;
	mblk_info=NULL;
	mblk_num_info=0;
}

int MBLK_IndexSize(int v)
{
	int i, j;

	i=0; j=256;
	while(v>j) { i++; j+=j>>2; }
	return(i);
}

int MBLK_AllocInfo()
{
	int i;

	if(mblk_info_free>=0)
	{
		i=mblk_info_free;
		mblk_info_free=mblk_info[i].next;

		mblk_info[i].rnext=-1;
		mblk_info[i].rprev=-1;
		mblk_info[i].ptr=NULL;
		mblk_info[i].size=0;
		mblk_info[i].flags=0;

		return(i);
	}

	i=mblk_num_info;
	if(!i)mblk_num_info=1<<14;
		else mblk_num_info+=i>>1;
	mblk_info=realloc(mblk_info, mblk_num_info*sizeof(BGBGC_BlockInfo));

	for(; i<mblk_num_info; i++)
	{
		mblk_info[i].next=mblk_info_free;
		mblk_info_free=i;
	}

	i=MBLK_AllocInfo();
	return(i);
}

int MBLK_AllocMemBlock(int sz)
{
	int sz1;
	int i;

	sz1=1<<20;
	while(sz1<sz)sz1<<=1;

	i=MBLK_AllocInfo();
	mblk_info[i].ptr=VirtualAlloc(NULL, sz1,
		MEM_RESERVE|MEM_COMMIT|MEM_WRITE_WATCH, PAGE_READWRITE);
	mblk_info[i].size=sz1;
	return(i);
}

int MBLK_FindFreeBlockSize(int sz)
{
	int i, j;

	i=MBLK_IndexSize(sz);
	while(i<64)
	{
		j=mblk_free[i];
		while(j>=0)
		{
			if(mblk_info[j].size>=sz)
				return(j);
			j=mblk_info[j].next;
		}
	}
	return(-1);
}

int MBLK_AllocFreeBlockSize(int sz)
{
	int i, j, k;

	i=MBLK_IndexSize(sz);
	while(i<64)
	{
		j=mblk_free[i]; k=-1;
		while(j>=0)
		{
			if(mblk_info[j].size>=sz)
			{
				if(k>=0)mblk_info[k].next=
					mblk_info[j].next;
				else mblk_free[i]=mblk_info[j].next;
				mblk_info[j].flags&=~1;
				return(j);
			}
			k=j; j=mblk_info[j].next;
		}
	}
	return(-1);
}

void MBLK_AddBlockFreeList(int i)
{
	int j;

	j=MBLK_IndexSize(mblk_info[i].size);
	mblk_info[i].next=mblk_free[j];
	mblk_free[j]=i;

	mblk_info[i].flags|=1;
}

int MBLK_FindUsedBlockPtr(void *p)
{
	void *q, *r;
	int i;

	i=mblk_used;
	while(i>=0)
	{
		q=mblk_info[i].ptr;
		r=(void *)(((byte *)q)+mblk_info[i].size);
		if((p<q) || (p>=r))
			{ i=mblk_info[i].next; continue; }
		return(i);
	}
	return(-1);
}

void MBLK_Split(int i, int sz)
{
	void *p;
	int j, k, sz1;

	sz1=mblk_info[i].size-sz;
	if(sz1<64)return;

	j=MBLK_AllocInfo();
	k=mblk_info[i].rnext;

	mblk_info[i].rnext=j;
	mblk_info[j].rprev=i;
	mblk_info[j].rnext=k;
	mblk_info[k].rprev=j;

	p=mblk_info[i].ptr;
	mblk_info[j].ptr=(void *)(((byte *)p)+sz);
	mblk_info[j].size=sz1;

	MBLK_AddBlockFreeList(j);
}

void MBLK_UnlinkFreeInfo()
{
	int i, j, k, l;

	for(i=0; i<64; i++)
	{
		j=mblk_free[i]; k=-1;
		while(j>=0)
		{
			if(mblk_info[j].flags&2)
			{
				if(k>=0)mblk_info[k].next=
					mblk_info[j].next;
				else mblk_free[i]=mblk_info[j].next;

				l=mblk_info[j].next;
				mblk_info[j].next=mblk_info_free;
				mblk_info_free=j;

				k=j; j=l;
				continue;
			}
			k=j; j=mblk_info[j].next;
		}
	}
}

void *MBLK_AllocSize(int sz)
{
	int i;

	sz=(sz+15)&(~15);
	i=MBLK_AllocFreeBlockSize(sz);
	if(i<0)i=MBLK_AllocMemBlock(sz);
	MBLK_Split(i, sz);
	return(mblk_info[i].ptr);
}

void MBLK_FreePtr(void *ptr)
{
	int i, j, k, i0;

	i=MBLK_FindUsedBlockPtr(ptr);
	if(i<0)return;

	MBLK_AddBlockFreeList(i);


	j=mblk_info[i].rprev;
	while((j>=0) && (mblk_info[j].flags&1))
		{ i=j; j=mblk_info[i].rprev; }

	i0=i;
	k=mblk_info[i].size;
	j=mblk_info[i].rnext;
	while((j>=0) && (mblk_info[j].flags&1))
	{
		i=j;
		mblk_info[i].flags|=1;
		k+=mblk_info[i].size;
		j=mblk_info[i].rnext;
	}

	MBLK_UnlinkFreeInfo();
}

