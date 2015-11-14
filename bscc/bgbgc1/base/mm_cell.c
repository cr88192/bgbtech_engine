#include <bgbgc.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef linux
#include <sys/mman.h>
#endif


void *bgbgc_alloc_mutex;

BGBGC_Chunk *bgbgc_initchain=NULL, *bgbgc_curchunk=NULL,
	*bgbgc_curxchunk=NULL;

BGBGC_Chunk *bgbgc_chunktab[1024];
int bgbgc_nchunk=0;

size_t bgbgc_size_lim_base;

#ifdef BGBGC_USEPRECISE
extern byte *bgbgc_precise_base;
extern byte *bgbgc_precise_end;
extern byte *bgbgc_precise_cbase;
extern byte *bgbgc_precise_cend;
#endif

static int log2up(int v)
{
	int c;
	c=0;
	while(v>1)
	{
		c++;
		if(v>1 && v&1)v++;
		v>>=1;
	}
	return(c);
}


BGBGC_Chunk *BGBGC_NewBlock(int size, int exec)
{
	int size2;
	BGBGC_Chunk *head;
	void *p;
	int i;

	size2=size+(sizeof(BGBGC_Chunk)*2)+4095+(size>>BGBGC_MAPSHIFT);
	size2=1<<log2up(size2);

	if(size2<(1<<BGBGC_CHKSIZE))size2=1<<BGBGC_CHKSIZE;

#ifdef linux
	if(exec)
	{
#ifdef __x86_64__
		head=(BGBGC_Chunk *)mmap(NULL, size2,
			PROT_READ|PROT_WRITE|PROT_EXEC,
			MAP_PRIVATE|MAP_ANONYMOUS|MAP_32BIT, -1, 0);
#else
		head=(BGBGC_Chunk *)mmap(NULL, size2,
			PROT_READ|PROT_WRITE|PROT_EXEC,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif
	}else
	{
		head=(BGBGC_Chunk *)mmap(NULL, size2,
			PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	}
#else
#ifdef WIN32
	if(exec)
	{
		head=VirtualAlloc(NULL, size2, MEM_RESERVE|MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);
	}else
	{
		head=VirtualAlloc(NULL, size2, MEM_RESERVE|MEM_COMMIT,
			PAGE_READWRITE);
	}
	if(!head)return(NULL);
	memset(head, 0, size2);
#else
//	head=sbrk(size2);
	head=malloc(size2);
	if(!head)return(NULL);
	memset(head, 0, size2);
#endif
#endif

	if(!head)return(NULL);

	head->next=bgbgc_initchain;
	head->size=size2;
	bgbgc_initchain=head;

	head->magic=BGBGC_MAGIC;
	if(exec)head->magic=BGBGC_XMAGIC;

	i=size2>>BGBGC_MAPSHIFT;
	head->map=((byte *)head)+(size2-i);

	head->data=(byte *)head;

	i=size2>>BGBGC_CELLSHIFT;
	head->cells=i-(i>>BGBGC_MAPSHIFT);


	for(i=0; i<((sizeof(BGBGC_Chunk)+BGBGC_CELLPAD)>>BGBGC_CELLSHIFT); i++)
		BGBGC_ChunkSetEnt(head, i, 3);

//	printf("BGBGC_NewBlock(%d): %p\n", size2, head);

	bgbgc_total_cells+=head->cells;

	i=bgbgc_nchunk++;
	if(i>=1024) { *(int *)-1=-1; }
	bgbgc_chunktab[i]=head;

	return(head);
}

BGBGC_Chunk *BGBGC_GetChunk(void *p)
{
	BGBGC_Chunk *cur;
	void *s, *e;

	cur=bgbgc_initchain;
	while(cur)
	{
#if 0
		if((cur->magic!=BGBGC_MAGIC) && (cur->magic!=BGBGC_XMAGIC))
		{
			printf("BGBGC_GetChunk: Magic Failure %p\n", cur);
			*(int *)-1=-1;
		}
#endif

		s=cur;
		e=((byte *)s)+cur->size;
//		if((p>=s) && (p<e))return(cur);
		if((p>=s) && (p<e))break;

//		if(	(p>=((void *)cur)) &&
//			(p<(void *)(((byte *)cur)+cur->size)))
//				break;

		cur=cur->next;
	}

#if 1
	if(cur && (cur->magic!=BGBGC_MAGIC) && (cur->magic!=BGBGC_XMAGIC))
	{
		printf("BGBGC_GetChunk: Magic Failure %p\n", cur);
		*(int *)-1=-1;
	}
	return(cur);
#endif

//	return(NULL);
}

int BGBGC_ChunkGetEnt(BGBGC_Chunk *chk, int ent)
{
	return(chk->map[ent]);
}

int BGBGC_ChunkSetEnt(BGBGC_Chunk *chk, int ent, int val)
{
	chk->map[ent]=val;
	return(0);
}

int BGBGC_CountFreeCells(BGBGC_Chunk *chk, int rov, int cnt)
{
	int i, j;

	for(i=rov; i<cnt; i++)
	{
		j=BGBGC_ChunkGetEnt(chk, i);
		if(j)break;
	}
	return(i-rov);
}

int BGBGC_CountUsedCells(BGBGC_Chunk *chk, int rov, int cnt)
{
	int i, j;

	for(i=rov; i<cnt; i++)
	{
		j=BGBGC_ChunkGetEnt(chk, i);
		if(!j)break;
	}
	return(i-rov);
}

void *BGBGC_AllocChk(BGBGC_Chunk *chk, int size)
{
	int i, j, cells, req;
	byte *map;
	int csize, mapsize;

	if(!chk)return(NULL);

	map=chk->map;
	cells=chk->cells;
	req=(size+BGBGC_CELLPAD)>>BGBGC_CELLSHIFT;

	i=chk->rover;
	while(i<cells)
	{
//		for(j=0; (j<req) && (j<cells); j++)
//			if(map[i+j] || ((i+j)>=cells))break;

		for(j=0; (j<req) && ((i+j)<cells); j++)
			if(map[i+j])break;

		if(j>=req)
//		if(j>req)
		{
			map[i]=BGBGC_MAP_NEW_HEAD;
			for(j=1; j<req; j++)map[i+j]=BGBGC_MAP_NEW_BODY;
			chk->rover=i+j;
			bgbgc_est_used_cells+=req;
			bgbgc_est_free_cells-=req;

//			return(((byte *)chk)+(i<<BGBGC_CELLSHIFT));
			return(chk->data+(i<<BGBGC_CELLSHIFT));
		}
		i+=j;

		for(; i<cells; i++)
			if(!(map[i]))
				break;
	}
	return(NULL);
}

int BGBGC_FreeChk(BGBGC_Chunk *chk, int idx)
{
#ifdef BGBGC_TINYLIST
	void *p;
#endif
	int i, j, k;

	for(i=idx; i>0; i--)
	{
		j=chk->map[i];
//		if((j&3)==1)break;
//		if((j&3)!=2)break;
		if((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_BODY)
			break;
	}
	if(!i || ((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_HEAD))
		return(-1);
//	if(!i)return(-1);

#ifdef BGBGC_TINYLIST
	for(j=1; j<BGBGC_TINYLIST; j++)
	{
		k=chk->map[i+j];
		if((k&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_BODY)
			break;
	}
	if(j<BGBGC_TINYLIST)
	{
		p=chk->data+(i<<BGBGC_CELLSHIFT);
		if(BGBGC_TryFreeTiny(p, j))
			return(0);
	}
#endif

	chk->map[i]=0;

	for(j=1;; j++)
	{
		k=chk->map[i+j];
		if((k&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_BODY)
			break;
		chk->map[i+j]=0;
	}
	bgbgc_manual_cells+=j;

	bgbgc_est_used_cells-=j;
	bgbgc_est_free_cells+=j;

	return(0);
}

int BGBGC_Free(void *p)
{
	BGBGC_Chunk *chk;
	int i;

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
		if(BGBGC_ConsP(p))
		{
			BGBGC_FreeCons(p);
			return(0);
		}

		i=BGBGC_FreeLObj(p);
		if(i>0)
		{
			bgbgc_size_lobjs-=i;
			if(bgbgc_size_lobjs<0)
				bgbgc_size_lobjs=0;
		}
		
		return(-1);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;
	BGBGC_FreeChk(chk, i);

	return(0);
}

int BGBGC_MarkAtomic(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);
		i=BGBGC_LookupLObjS2(p);		//2012-10-24
		if(i>=0)bgbgc_lobj_flag[i]|=16;
		return(-1);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;

	for(; i>0; i--)
	{
		j=chk->map[i];
//		if((j&BGBGC_MAP_TYPEMASK)==BGBGC_MAP_TYPE_HEAD)
//			break;
		if((j&BGBGC_MAP_TYPE_MASK)!=BGBGC_MAP_TYPE_BODY)
			break;
	}

	if(!i)return(-1);
	if((j&BGBGC_MAP_TYPE_MASK)!=BGBGC_MAP_TYPE_HEAD)
		return(-1);	//invalid

	chk->map[i]&=BGBGC_MAP_BASEMASK;
	chk->map[i]|=BGBGC_MAP_TRC_ATOMIC;
	chk->map[i]|=BGBGC_MAP_CONSV_MARK;

	return(0);
}

int BGBGC_MarkAtomicP(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);
		i=BGBGC_LookupLObjS2(p);		//2012-10-24
		if(i>=0)if(bgbgc_lobj_flag[i]&16)
			return(1);
		return(0);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;

	for(; i>0; i--)
	{
		j=chk->map[i];
		if((j&BGBGC_MAP_TYPEMASK)==BGBGC_MAP_TYPE_HEAD)
			break;
	}
	if(!i)return(-1);

//	if(chk->map[i]&16)return(1);
	if((chk->map[i]&BGBGC_MAP_TRC_MASK)==BGBGC_MAP_TRC_ATOMIC)
		return(1);
	return(0);
}

void *BGBGC_GetBase(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);
		i=BGBGC_LookupLObjS2(p);		//2012-10-24
		if(i>=0)return(bgbgc_lobj[i]);

		printf("BGBGC_GetBase: No Chunk for %p\n", p);
		*(int *)-1=-1;
		return(NULL);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;

	if(i>=chk->cells)
	{
		printf("BGBGC_GetBase: Bad Range for %p\n", p);
		*(int *)-1=-1;
	}

	for(; i>0; i--)
	{
		j=chk->map[i];
//		if((j&BGBGC_MAP_TYPEMASK)==1)break;
		if((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_BODY)
			break;
	}
	if(!i || ((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_HEAD))
	{
		printf("BGBGC_GetBase: No Base for %p\n", p);
		return(NULL);
	}

//	p=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
	p=chk->data+(i<<BGBGC_CELLSHIFT);

	return(p);
}

void *BGBGC_TryGetBase(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);
		i=BGBGC_LookupLObjS2(p);		//2012-10-24
		if(i>=0)return(bgbgc_lobj[i]);
		return(NULL);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;

	if(i>=chk->cells)
		return(NULL);

	for(; i>0; i--)
	{
		j=chk->map[i];
//		if((j&BGBGC_MAP_TYPEMASK)==BGBGC_MAP_TYPE_HEAD)break;
		if((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_BODY)
			break;
	}
//	if(!i)
	if(!i || ((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_HEAD))
		return(NULL);

//	p=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
	p=chk->data+(i<<BGBGC_CELLSHIFT);

	return(p);
}

#ifdef BGBGC_USEPRECISE
void *BGBGC_TryGetPrecise(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);
		i=BGBGC_LookupLObjS2(p);		//2012-10-24
		if(i>=0)return(bgbgc_lobj[i]);
		return(NULL);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;

	if(i>=chk->cells)
		return(NULL);

	for(; i>0; i--)
	{
		j=chk->map[i];
		if((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_BODY)break;
	}
	if(!i)return(NULL);
	if((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_HEAD)return(NULL);

//	p=((void *)chk)+(i<<BGBGC_CELLSHIFT);

	for(j=0; j<bgbgc_nchunk; j++)
		if(chk==bgbgc_chunktab[j])
			break;

	p=(void *)(bgbgc_precise_base+(j<<BGBGC_BLKSHIFT)+i);

	return(p);
}

void *BGBGC_MakePrecise(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);
		i=BGBGC_LookupLObjS2(p);		//2012-10-24
		if(i>=0)return(bgbgc_lobj[i]);
		return(NULL);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;

	if(i>=chk->cells)
		return(NULL);

	for(; i>0; i--)
	{
		j=chk->map[i];
		if((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_BODY)
			break;
	}
	if(!i)return(NULL);
	if((j&BGBGC_MAP_TYPEMASK)!=BGBGC_MAP_TYPE_HEAD)
		return(NULL);

//	p=((void *)chk)+(i<<BGBGC_CELLSHIFT);

	for(j=0; j<bgbgc_nchunk; j++)
		if(chk==bgbgc_chunktab[j])
			break;

	chk->map[i]|=BGBGC_MAP_TRC_PRECISE0;

	p=(void *)(bgbgc_precise_base+(j<<BGBGC_BLKSHIFT)+i);
	return(p);
}
#endif

int BGBGC_Init()
{
	static int init;
	int i;

	if(init)return(0);
	init=1;

//	printf("BGBGC_Init\n");

	if(!bgbgc_size_lim_base)
		bgbgc_size_lim_base=BGBGC_HEAPLIM;

	BGBGC_InitLow();
	BGBGC_InitCons();
//	BGBGC_Init();

#ifdef BGBGC_USEPRECISE
	//BGB: because 'precise' is dead, no need to waste space...
//	bgbgc_precise_base=gcllSpaceNew("_precise_vt", 24);
//	bgbgc_precise_end=bgbgc_precise_base+(1<<16);
//	bgbgc_precise_cbase=gcllSpaceNew("_cons_vt", 24);
//	bgbgc_precise_cend=bgbgc_precise_cbase+(1<<16);
#endif

	BGBGC_NewBlock(1<<(BGBGC_CHKSIZE-1), 1);
	for(i=1; i<BGBGC_INITHEAP; i++)
		BGBGC_NewBlock(1<<(BGBGC_CHKSIZE-1), 0);

	bgbgc_alloc_mutex=thFastMutex();
	BGBGC_SpawnThread();
	return(0);
}

int BGBGC_BaseHeap(size_t heap)
{
	BGBGC_Chunk *chk;

	BGBGC_Init();

	chk=bgbgc_initchain;
	while(chk)
	{
		heap-=chk->size;
		chk=chk->next;
	}
	if(heap<=0)return(0);	//allready have enough heap

	while(heap>0)
	{
		BGBGC_NewBlock(1<<(BGBGC_CHKSIZE-1), 0);
		heap-=1<<(BGBGC_CHKSIZE-1);
	}

	return(0);
}

int BGBGC_BaseHeapLimit(size_t heap)
{
	if(!bgbgc_size_lim_base)
		bgbgc_size_lim_base=BGBGC_HEAPLIM;

	while(heap>bgbgc_size_lim_base)
		bgbgc_size_lim_base+=
			bgbgc_size_lim_base>>2;
	return(0);
}

int BGBGC_CheckHeapBelowBaseLimit(void)
{
	BGBGC_Chunk *chk;
	int sz;
	
	sz=bgbgc_size_lim_base;

	chk=bgbgc_initchain;
	while(chk)
	{
		sz-=chk->size;
		chk=chk->next;
	}
	if(sz<=0)return(0);	//allready past limit
	return(1);
}
