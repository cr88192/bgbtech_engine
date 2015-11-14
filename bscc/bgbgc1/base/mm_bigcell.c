/*
Big Cell Allocator
*/

#include <bgbgc.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef linux
#include <sys/mman.h>
#endif


void *bgbgc_alloc_mutex;

BGBGC_Chunk *bgbgc_bigcell_initchain=NULL;
BGBGC_Chunk *bgbgc_bigcell_curchunk=NULL;

BGBGC_Chunk *bgbgc_bigcell_chunktab[1024];
int bgbgc_bigcell_nchunk=0;

size_t bgbgc_size_lim_bigbase;

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


BGBGC_Chunk *BGBGC_BigCell_NewBlock(int size, int exec)
{
	int size2;
	BGBGC_Chunk *head;
	void *p;
	int i;

	size2=size+(sizeof(BGBGC_Chunk)*2)+4095+(size>>BGBGC_BIGMAPSHIFT);
	size2=1<<log2up(size2);

	if(size2<(1<<BGBGC_BIGCHKSIZE))size2=1<<BGBGC_BIGCHKSIZE;

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

	head->next=bgbgc_bigcell_initchain;
	head->size=size2;
	bgbgc_bigcell_initchain=head;

	head->magic=BGBGC_BIGMAGIC;
	if(exec)head->magic=BGBGC_BIGXMAGIC;

	i=size2>>BGBGC_BIGMAPSHIFT;
	head->map=((byte *)head)+(size2-i);

	head->data=(byte *)head;

	i=size2>>BGBGC_BIGCELLSHIFT;
	head->cells=i-(i>>BGBGC_BIGMAPSHIFT);


	for(i=0; i<((sizeof(BGBGC_Chunk)+BGBGC_BIGCELLPAD)>>BGBGC_BIGCELLSHIFT); i++)
		BGBGC_ChunkSetEnt(head, i, 3);

//	printf("BGBGC_NewBlock(%d): %p\n", size2, head);

	bgbgc_total_cells+=head->cells;

	i=bgbgc_bigcell_nchunk++;
	if(i>=1024) { *(int *)-1=-1; }
	bgbgc_bigcell_chunktab[i]=head;

	return(head);
}

BGBGC_Chunk *BGBGC_BigCell_GetChunk(void *p)
{
	BGBGC_Chunk *cur;
	void *s, *e;

	cur=bgbgc_bigcell_initchain;
	while(cur)
	{
#if 0
		if((cur->magic!=BGBGC_BIGMAGIC) && (cur->magic!=BGBGC_BIGXMAGIC))
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
	if(cur && (cur->magic!=BGBGC_BIGMAGIC) && (cur->magic!=BGBGC_BIGXMAGIC))
	{
		printf("BGBGC_GetChunk: Magic Failure %p\n", cur);
		*(int *)-1=-1;
	}
	return(cur);
#endif

//	return(NULL);
}

int BGBGC_BigCell_ChunkGetEnt(BGBGC_Chunk *chk, int ent)
{
	return(chk->map[ent]);
}

int BGBGC_BigCell_ChunkSetEnt(BGBGC_Chunk *chk, int ent, int val)
{
	chk->map[ent]=val;
	return(0);
}

int BGBGC_BigCell_CountFreeCells(BGBGC_Chunk *chk, int rov, int cnt)
{
	int i, j;

	for(i=rov; i<cnt; i++)
	{
		j=BGBGC_BigCell_ChunkGetEnt(chk, i);
		if(j)break;
	}
	return(i-rov);
}

int BGBGC_BigCell_CountUsedCells(BGBGC_Chunk *chk, int rov, int cnt)
{
	int i, j;

	for(i=rov; i<cnt; i++)
	{
		j=BGBGC_BigCell_ChunkGetEnt(chk, i);
		if(!j)break;
	}
	return(i-rov);
}

void *BGBGC_BigCell_AllocChk(BGBGC_Chunk *chk, int size)
{
	int i, j, cells, req;
	byte *map;
	int csize, mapsize;

	if(!chk)return(NULL);

	map=chk->map;
	cells=chk->cells;
	req=(size+BGBGC_BIGCELLPAD)>>BGBGC_BIGCELLSHIFT;

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
			bgbgc_est_used_bigcells+=req;
			bgbgc_est_free_bigcells-=req;

//			return(((byte *)chk)+(i<<BGBGC_BIGCELLSHIFT));
			return(chk->data+(i<<BGBGC_BIGCELLSHIFT));
		}
		i+=j;

		for(; i<cells; i++)
			if(!(map[i]))
				break;
	}
	return(NULL);
}

int BGBGC_BigCell_FreeChk(BGBGC_Chunk *chk, int idx)
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
		p=chk->data+(i<<BGBGC_BIGCELLSHIFT);
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

	bgbgc_est_used_bigcells-=j;
	bgbgc_est_free_bigcells+=j;

	return(0);
}

int BGBGC_BigCell_Free(void *p)
{
	BGBGC_Chunk *chk;
	int i;

	chk=BGBGC_BigCell_GetChunk(p);
	if(!chk)
	{
		return(-1);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_BIGCELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_BIGCELLSHIFT;
	BGBGC_BigCell_FreeChk(chk, i);

	return(0);
}

int BGBGC_BigCell_MarkAtomic(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_BigCell_GetChunk(p);
	if(!chk)
	{
		return(-1);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_BIGCELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_BIGCELLSHIFT;

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

int BGBGC_BigCell_MarkAtomicP(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_BigCell_GetChunk(p);
	if(!chk)
	{
		return(0);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_BIGCELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_BIGCELLSHIFT;

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

void *BGBGC_BigCell_GetBase(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_BigCell_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);
		i=BGBGC_LookupLObjS2(p);		//2012-10-24
		if(i>=0)return(bgbgc_lobj[i]);

		printf("BGBGC_GetBase: No Chunk for %p\n", p);
		*(int *)-1=-1;
		return(NULL);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_BIGCELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_BIGCELLSHIFT;

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

//	p=((byte *)chk)+(i<<BGBGC_BIGCELLSHIFT);
	p=chk->data+(i<<BGBGC_BIGCELLSHIFT);

	return(p);
}

void *BGBGC_BigCell_TryGetBase(void *p)
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=BGBGC_BigCell_GetChunk(p);
	if(!chk)
	{
		return(NULL);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_BIGCELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_BIGCELLSHIFT;

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

//	p=((byte *)chk)+(i<<BGBGC_BIGCELLSHIFT);
	p=chk->data+(i<<BGBGC_BIGCELLSHIFT);

	return(p);
}

int BGBGC_BigCell_Init()
{
	static int init;
	int i;

	if(init)return(0);
	init=1;

//	printf("BGBGC_Init\n");

	if(!bgbgc_size_lim_bigbase)
		bgbgc_size_lim_bigbase=BGBGC_BIGHEAPLIM;

//	BGBGC_InitLow();
//	BGBGC_InitCons();
//	BGBGC_Init();

	BGBGC_BigCell_NewBlock(1<<(BGBGC_BIGCHKSIZE-1), 1);
	for(i=1; i<BGBGC_BIGINITHEAP; i++)
		BGBGC_NewBlock(1<<(BGBGC_BIGCHKSIZE-1), 0);

	return(0);
}

int BGBGC_BigCell_BaseHeap(size_t heap)
{
	BGBGC_Chunk *chk;

	BGBGC_BigCell_Init();

	chk=bgbgc_bigcell_initchain;
	while(chk)
	{
		heap-=chk->size;
		chk=chk->next;
	}
	if(heap<=0)return(0);	//allready have enough heap

	while(heap>0)
	{
		BGBGC_BigCell_NewBlock(1<<(BGBGC_BIGCHKSIZE-1), 0);
		heap-=1<<(BGBGC_BIGCHKSIZE-1);
	}

	return(0);
}

int BGBGC_BigCell_BaseHeapLimit(size_t heap)
{
	if(!bgbgc_size_lim_bigbase)
		bgbgc_size_lim_bigbase=BGBGC_BIGHEAPLIM;

	while(heap>bgbgc_size_lim_bigbase)
		bgbgc_size_lim_bigbase+=
			bgbgc_size_lim_bigbase>>2;
	return(0);
}

int BGBGC_BigCell_CheckHeapBelowBaseLimit(void)
{
	BGBGC_Chunk *chk;
	int sz;
	
	sz=bgbgc_size_lim_bigbase;

	chk=bgbgc_bigcell_initchain;
	while(chk)
	{
		sz-=chk->size;
		chk=chk->next;
	}
	if(sz<=0)return(0);	//allready past limit
	return(1);
}
