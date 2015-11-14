#include <bgbgc.h>

#ifdef MSVC
#include <windows.h>
#endif

void *bgbgc_stackbase=NULL;

void (*bgbgc_markhdlr[1024])();
int bgbgc_nmarkhdlr=0;

void *bgbgc_alloc_mutex=NULL;

volatile int bgbgc_used_cells;
volatile int bgbgc_free_cells;
volatile int bgbgc_freed_cells;
volatile int bgbgc_manual_cells;
volatile int bgbgc_total_cells;

volatile int bgbgc_used_cons;
volatile int bgbgc_freed_cons;
volatile int bgbgc_total_cons;

volatile int bgbgc_last_gc=-999999;
volatile int bgbgc_est_used_cells, bgbgc_est_free_cells;
volatile int bgbgc_num_lobjs, bgbgc_size_lobjs;
volatile int bgbgc_freed_lobjs, bgbgc_freed_size_lobjs, bgbgc_size_lim_lobjs;

volatile int bgbgc_lastgc;
volatile int bgbgc_delaymode;

volatile int bgbgc_rungc=0;	//GC cycle is running
volatile int bgbgc_reqgc=0;	//GC cycle requested
volatile int bgbgc_spawngc=0;	//GC thread spawned
volatile int bgbgc_markok=0;	//GC is still marking stuff

volatile int bgbgc_reqpause=0;	//GC pause requested
volatile int bgbgc_pauseok=0;	//GC pause accepted

volatile void **bgbgc_markstack=NULL;
volatile int bgbgc_markstackpos;
volatile int bgbgc_markstacklim;

volatile void **bgbgc_pmarkstack=NULL;
volatile int bgbgc_pmarkstackpos;
volatile int bgbgc_pmarkstacklim;

#ifdef BGBGC_USEPRECISE
byte *bgbgc_precise_base=NULL;
byte *bgbgc_precise_end=NULL;

extern byte *bgbgc_precise_cbase;
extern byte *bgbgc_precise_cend;
#endif

extern volatile BGBGC_Chunk *bgbgc_chunktab[1024];
extern volatile int bgbgc_nchunk;

#ifdef BGBGC_USEPRECISE
extern void **bgbgc_gcp_root[4096];
extern int bgbgc_gcp_nroot;
#endif

#ifdef BGBGC_TINYLIST
volatile void **bgbgc_tinylist=NULL;
#endif

int BGBGC_ProbeDelayGC(int mode)
{
	bgbgc_delaymode=mode;
	if(!mode)return(0);

	if((mode==1) || (mode==2))
		bgbgc_lastgc=clock();
	if(mode==3)
		BGBGC_Collect();
	return(0);
}

int BGBGC_CheckNoGC()
{
	int t;

	if(bgbgc_delaymode==1)
		return(1);
	if(bgbgc_delaymode==5)
		return(1);
	t=clock();
	if((t-bgbgc_lastgc)<(30*CLOCKS_PER_SEC))
		return(1);
	return(0);
}

int BGBGC_CheckLogAlloc()
{
	if(bgbgc_delaymode==4)
		return(1);
	return(0);
}

#ifdef BGBGC_TINYLIST
int BGBGC_TryFreeTiny(void *p, int szi)
{
	int i, j;
	if(bgbgc_rungc)
		return(0);

	if((szi<0) || (szi>=BGBGC_TINYLIST))
		return(0);

	if(((void **)p)[1]==(void *)(0xF5EEDEAD))
	{
		*(int *)-1=-1;
	}

//	memset(p, 0, szi<<BGBGC_CELLSHIFT);
//	j=BGBGC_CELLSHIFT/sizeof(void *);
	j=BGBGC_CELLSIZE/sizeof(void *);
	for(i=0; i<(szi*j); i++)
	{
#ifdef X86_64
		((volatile void **)p)[i]=(void *)(0xF5EEDF5EED000000ULL+i);
#else
		((volatile void **)p)[i]=(void *)(0xF5EED000UL+i);
#endif
	}

	*(volatile void **)p=bgbgc_tinylist[szi];
	((volatile void **)p)[1]=(void *)(0xF5EEDEAD);
	bgbgc_tinylist[szi]=p;
	return(1);
}

int BGBGC_FlushTiny()
{
	int i;

	if(!bgbgc_tinylist)
		bgbgc_tinylist=malloc(BGBGC_TINYLIST*sizeof(void *));

	for(i=0; i<BGBGC_TINYLIST; i++)
		bgbgc_tinylist[i]=NULL;
	return(0);
}

int BGBGC_LockGC()
{
	thLockFastMutex(bgbgc_alloc_mutex);
	return(0);
}

int BGBGC_UnlockGC()
{
	thUnlockFastMutex(bgbgc_alloc_mutex);
	return(0);
}

int BGBGC_LockGC2()
{
	thLockFastMutex(bgbgc_alloc_mutex);
	return(0);
}

int BGBGC_UnlockGC2()
{
	thUnlockFastMutex(bgbgc_alloc_mutex);
	return(0);
}

void *BGBGC_AllocTiny(int szi)
{
	void *p;
	int i;

	if((szi<0) || (szi>=BGBGC_TINYLIST))
		return(NULL);
	if(bgbgc_tinylist[szi])
	{
		p=(void *)(bgbgc_tinylist[szi]);
		bgbgc_tinylist[szi]=*(void **)p;
		memset(p, 0, szi<<BGBGC_CELLSHIFT);
		return(p);
	}
	return(NULL);
}
#endif

void *BGBGC_Alloc(int size)
{
	BGBGC_Chunk *chk;
	void *p;
	int t, r;

#ifdef BGBGC_TINYLIST
#ifdef MSVC
	__try {
#endif

	t=(size+BGBGC_CELLPAD)>>BGBGC_CELLSHIFT;
	p=BGBGC_AllocTiny(t);
	if(p)return(p);

#ifdef MSVC
	} __except(EXCEPTION_EXECUTE_HANDLER)
		{ BGBGC_FlushTiny(); }
#endif

#endif

//	thLockFastMutex(bgbgc_alloc_mutex);

	//if GC, try to pause (delay if not possible)
	BGBGC_CollectPause();

//	if(0)
	if(size>=6144)
	{
		bgbgc_size_lobjs+=size;

		if(bgbgc_size_lobjs>=bgbgc_size_lim_lobjs)
		{
//			t=clock();
			if(!BGBGC_CheckNoGC())
				//last gc too recent, don't bother
			{
				BGBGC_CollectUnpause();
//				BGBGC_Collect();
				BGBGC_CollectPause();

				bgbgc_lastgc=clock();
				bgbgc_size_lobjs+=size;
			}else
			{
				bgbgc_size_lim_lobjs+=
					bgbgc_size_lim_lobjs>>2;
				if(bgbgc_size_lim_lobjs>(1LL<<30))
					bgbgc_size_lim_lobjs=(1LL<<30);
			}

		}

		p=BGBGC_AllocLObj(size);

		BGBGC_CollectUnpause();
//		thUnlockFastMutex(bgbgc_alloc_mutex);
		BGBGC_TryMarkSafe(p);
		return(p);
	}

	//PASS 1: try normally
	chk=bgbgc_curchunk;
//	chk=bgbgc_initchain;
	while(chk)
	{
		p=BGBGC_AllocChk(chk, size);
		if(p)
		{
			bgbgc_curchunk=chk;
//			thUnlockFastMutex(bgbgc_alloc_mutex);
			BGBGC_TryMarkSafe(p);
			return(p);
		}
		chk=chk->next;

		//avoid stepping in executable memory
		while(chk && (chk->magic==BGBGC_XMAGIC))
			chk=chk->next;
	}

#ifdef BGBGC_TINYLIST
	BGBGC_FlushTiny();
#endif

	//PASS 2: reset and scan entire heap
	chk=bgbgc_initchain;
	while(chk)
	{
		chk->rover=0;
		p=BGBGC_AllocChk(chk, size);
		if(p)
		{
			bgbgc_curchunk=chk;
//			thUnlockFastMutex(bgbgc_alloc_mutex);
			BGBGC_TryMarkSafe(p);
			return(p);
		}
		chk=chk->next;
		//at this point, exec memory is ok
	}

	BGBGC_CollectUnpauseDelay();

	//PASS 3: collect and try again
//	t=clock();
//	if((t-bgbgc_lastgc)>(30*CLOCKS_PER_SEC))
	if(!BGBGC_CheckNoGC() && !BGBGC_CheckHeapBelowBaseLimit())
	{
		//last gc too recent, don't bother

		BGBGC_Collect();
		bgbgc_lastgc=clock();

		r=(100LL*bgbgc_used_cells)/bgbgc_total_cells;

		//heap mostly full, expand
		if(r>75)
		{
			chk=BGBGC_NewBlock(size, 0);
			p=BGBGC_AllocChk(chk, size);
			bgbgc_curchunk=chk;
//			thUnlockFastMutex(bgbgc_alloc_mutex);
			BGBGC_TryMarkSafe(p);
			return(p);
		}

		chk=bgbgc_initchain;
		while(chk)
		{
			p=BGBGC_AllocChk(chk, size);
			if(p)
			{
				bgbgc_curchunk=chk;
//				thUnlockFastMutex(bgbgc_alloc_mutex);
				BGBGC_TryMarkSafe(p);
				return(p);
			}
			chk=chk->next;
		}
	}

	//PASS 4: increase heap
	chk=BGBGC_NewBlock(size, 0);
	p=BGBGC_AllocChk(chk, size);
	bgbgc_curchunk=chk;

//	thUnlockFastMutex(bgbgc_alloc_mutex);
	BGBGC_TryMarkSafe(p);
	return(p);
}

void *BGBGC_AllocExec(int size)
{
	BGBGC_Chunk *chk;
	void *p;
	int t, r;

	//if GC, delay until done
	BGBGC_CollectDelay();

//	thLockFastMutex(bgbgc_alloc_mutex);

	//PASS 1: try normally
	chk=bgbgc_curxchunk;
	while(chk)
	{
		p=BGBGC_AllocChk(chk, size);
		if(p)
		{
			bgbgc_curxchunk=chk;
//			thUnlockFastMutex(bgbgc_alloc_mutex);
			BGBGC_TryMarkSafe(p);
			return(p);
		}
		chk=chk->next;
		while(chk && (chk->magic!=BGBGC_XMAGIC))
			chk=chk->next;
	}

	//PASS 2: reset and scan entire heap
	chk=bgbgc_initchain;
	while(chk && (chk->magic!=BGBGC_XMAGIC))
		chk=chk->next;
	while(chk)
	{
		chk->rover=0;
		p=BGBGC_AllocChk(chk, size);
		if(p)
		{
			bgbgc_curxchunk=chk;
//			thUnlockFastMutex(bgbgc_alloc_mutex);
			BGBGC_TryMarkSafe(p);
			return(p);
		}
		chk=chk->next;
		while(chk && (chk->magic!=BGBGC_XMAGIC))
			chk=chk->next;
	}

	//PASS 3: collect and try again
//	t=clock();
//	if((t-bgbgc_lastgc)>CLOCKS_PER_SEC)
		//last gc too recent, don't bother
	if(!BGBGC_CheckNoGC())
	{
		BGBGC_Collect();
		bgbgc_lastgc=clock();

		r=(100LL*bgbgc_used_cells)/bgbgc_total_cells;

		//heap mostly full, expand
		if(r>75)
		{
			chk=BGBGC_NewBlock(size, 1);
			p=BGBGC_AllocChk(chk, size);
			bgbgc_curxchunk=chk;
//			thUnlockFastMutex(bgbgc_alloc_mutex);
			BGBGC_TryMarkSafe(p);
			return(p);
		}

		chk=bgbgc_initchain;
		while(chk && (chk->magic!=BGBGC_XMAGIC))
			chk=chk->next;
		while(chk)
		{
			p=BGBGC_AllocChk(chk, size);
			if(p)
			{
				bgbgc_curxchunk=chk;
//				thUnlockFastMutex(bgbgc_alloc_mutex);
				BGBGC_TryMarkSafe(p);
				return(p);
			}
			chk=chk->next;
			while(chk && (chk->magic!=BGBGC_XMAGIC))
				chk=chk->next;
		}
	}

	//PASS 4: increase heap
	chk=BGBGC_NewBlock(size, 1);
	p=BGBGC_AllocChk(chk, size);
	bgbgc_curxchunk=chk;

//	thUnlockFastMutex(bgbgc_alloc_mutex);
	BGBGC_TryMarkSafe(p);
	return(p);
}

void BGBGC_PrintBacktrack(BGBGC_Chunk *chk, int idx)
{
	static int rec=0;
	BGBGC_SourceInfo tsrcinf;
	void *p;
	char *s, *s1;
	int i, j, k;
	
	i=idx-1;
	for(; i>0; i--)
	{
		j=BGBGC_ChunkGetEnt(chk, i);
		if((j&3)==1)break;
//		if((j&3)!=2)break;
	}
	if(i<=0)
	{
		printf("\tChunk Start @%p\n", chk->data);
		return;
	}

	if(!rec)
		printf("\t");

	p=(chk->data)+(i<<BGBGC_CELLSHIFT);
//	j=BGBGC_HandleMark(p);
	s=BGBGC_HandleGetTypeName(p);
//	if(j==-2)
	if(s==UNDEFINED)
	{
		rec++;
		if(rec>=16)
		{
//			printf("\tRecursion Limit @%p\n", p);
			printf("LIM\n");
			rec--;
			return;
		}
//		printf("\tMore Damage @%p\n", p);
		printf(".");
		BGBGC_PrintBacktrack(chk, i);
		rec--;
		return;
	}
	
	BGBGC_HandleGetObjSourceInfo(p, &tsrcinf);
	s1=tsrcinf.lfn;
	if(!s1)s1="?";
	j=tsrcinf.lln;
	k=tsrcinf.access;
	
	printf("Obj @%p Ty=%s, LLn=%s:%d, Acc=%08X\n", p, s, s1, j, k);
}

int BGBGC_Mark(void *p)
{
	BGBGC_Chunk *chk;
	void *mark1;
	int i;

	if(!p)return(-1);	//NULL
	if(p==UNDEFINED)
		return(-1);	//NULL

	if(BGBGC_CheckSpaceP(p))
		return(-1);

//	if((bgbgc_markstackpos+1)>=bgbgc_markstacklim)
	if(!bgbgc_markstack || ((bgbgc_markstackpos+1)>=bgbgc_markstacklim))
	{
//		fprintf(stderr, "BGBGC_Mark: depth limit exceeded\n");
//		printf("BGBGC_Mark: depth limit exceeded\n");
//		return(-1);

		i=bgbgc_markstacklim; i=i+(i>>1);

		if(i<bgbgc_markstacklim)
		{
			printf("BGBGC_Mark: stack size overflow, %d -> %d\n",
				bgbgc_markstacklim, i);
			return(-1);
		}

//		fprintf(stderr, "BGBGC_Mark: depth limit %d->%d\n",
//			bgbgc_markstacklim, i);
//		printf("BGBGC_Mark: depth limit %d->%d\n",
//			bgbgc_markstacklim, i);

//		bgbgc_markstack=realloc((void *)bgbgc_markstack, i*sizeof(void *));
//		bgbgc_markstacklim=i;

		mark1=malloc(i*sizeof(void *));
		if(mark1)
		{
			printf("BGBGC_Mark: depth limit %d->%d\n",
				bgbgc_markstacklim, i);

			memcpy(mark1, (void *)bgbgc_markstack,
				bgbgc_markstacklim*sizeof(void *));
			free((void *)bgbgc_markstack);
			bgbgc_markstack=mark1;
			bgbgc_markstacklim=i;
		}else
		{
//			printf("BGBGC_Mark: depth limit %d (expand failed)\n",
//				bgbgc_markstacklim);
			return(-1);
		}
	}

#ifdef BGBGC_USECONS
	if(BGBGC_CheckCons(p)>=0)
	{
		bgbgc_markstack[bgbgc_markstackpos++]=p;
		return(0);
	}
#endif

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
		i=BGBGC_LookupLObjS(p);
		if(i<0)return(-1);	//invalid
		if(bgbgc_lobj_flag[i]&1)return(1);	//black
	}

	bgbgc_markstack[bgbgc_markstackpos++]=p;
	return(0);
}

int BGBGC_MarkObj(void *p)
{
	BGBGC_Chunk *chk;
	int i, j, k;

	if(!p)return(-1);	//NULL
	if(p==UNDEFINED)
		return(-1);	//NULL

#ifdef BGBGC_USECONS
	i=BGBGC_CheckCons(p);
	if(i>=0)
	{
		BGBGC_MarkCons(p, i);
		return(0);
	}
#endif

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
		i=BGBGC_LookupLObjS(p);
		if(i>=0)
		{
			if(bgbgc_lobj_flag[i]&1)
				return(1);

			bgbgc_lobj_flag[i]|=1;

			if(!(bgbgc_lobj_flag[i]&16))
			{
				p=bgbgc_lobj[i];
				j=bgbgc_lobj_size[i]>>BGBGC_WORDSHIFT;
				k=BGBGC_HandleMark(p);
				if(k==-2)printf("\tIs LOBJ\n");
				if(k<0)BGBGC_ScanRange((void **)p, j);
			}

			return(0);
		}

		return(-1);	//invalid
	}

//	printf("\t\tValid1 %p\n", p);

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;

	for(; i>0; i--)
	{
		j=BGBGC_ChunkGetEnt(chk, i);
//		if((j&3)==1)break;
		if((j&BGBGC_MAP_TYPE_MASK)!=BGBGC_MAP_TYPE_BODY)
			break;
	}
	if(!i)return(-1);	//invalid
	if((j&BGBGC_MAP_TYPE_MASK)!=BGBGC_MAP_TYPE_HEAD)
		return(-1);	//invalid

	if((j&BGBGC_MAP_MARK_MASK)==BGBGC_MAP_MARK_BLACK)
		return(1);	//already marked

//	printf("\t\tMark %p\n", p);

	if((j&BGBGC_MAP_MARK_MASK)!=BGBGC_MAP_MARK_LOCK)
	{
		for(j=0;; j++)
		{
			k=BGBGC_ChunkGetEnt(chk, i+j);
			if(j && ((k&BGBGC_MAP_TYPE_MASK)!=BGBGC_MAP_TYPE_BODY))
				break;
			BGBGC_ChunkSetEnt(chk, i+j,
				(k&(~BGBGC_MAP_MARK_MASK)) |
				BGBGC_MAP_MARK_BLACK | BGBGC_MAP_CONSV_MARK);
		}
	}else
	{
		for(j=0;; j++)
		{
			k=BGBGC_ChunkGetEnt(chk, i+j);
			if(j && ((k&BGBGC_MAP_TYPE_MASK)!=BGBGC_MAP_TYPE_BODY))
				break;
		}
	}

#if 0
	k=BGBGC_ChunkGetEnt(chk, i);
	if(!(k&16))
	{
		p=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
		k=BGBGC_HandleMark(p);
		if(k<0)BGBGC_ScanRange((void **)p,
			j<<(BGBGC_CELLSHIFT-BGBGC_WORDSHIFT));
	}
#endif

	k=BGBGC_ChunkGetEnt(chk, i);
	switch((k>>4)&7)
	{
	case 0:
//		p=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
		p=(chk->data)+(i<<BGBGC_CELLSHIFT);
		k=BGBGC_HandleMark(p);
		if(k<0)BGBGC_ScanRange((void **)p,
			j<<(BGBGC_CELLSHIFT-BGBGC_WORDSHIFT));
		if(k==-2)BGBGC_PrintBacktrack(chk, i);
		break;
	case 1: break;
	case 2: case 3: case 4: case 5: case 6: case 7:
//		p=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
		p=(chk->data)+(i<<BGBGC_CELLSHIFT);
		k=BGBGC_HandleMark(p);
		if(k==-2)BGBGC_PrintBacktrack(chk, i);
//		if(k<0)BGBGC_ScanRange((void **)p,
//			j<<(BGBGC_CELLSHIFT-BGBGC_WORDSHIFT));
		break;

	}
	return(0);
}


#ifdef BGBGC_USEPRECISE
int BGBGC_MarkPrecise(void *p)
{
	int i;

	if(!p)return(-1);	//NULL

	if(bgbgc_pmarkstackpos>bgbgc_pmarkstacklim)
	{
		fprintf(stderr, "BGBGC_MarkPrecise: depth limit exceeded\n");
//		return(-1);

		i=bgbgc_pmarkstacklim; i=i+(i>>1);
		fprintf(stderr, "BGBGC_Mark: depth limit %d->%d\n",
			bgbgc_pmarkstacklim, i);

		bgbgc_pmarkstack=realloc((void *)bgbgc_pmarkstack, i*sizeof(void *));
		bgbgc_pmarkstacklim=i;
	}

	bgbgc_pmarkstack[bgbgc_pmarkstackpos++]=p;
	return(0);
}

int BGBGC_MarkObjPrecise(void *p)
{
	BGBGC_Chunk *chk;
	int i, j, k;

	if((((byte *)p)>=bgbgc_precise_base) &&
		(((byte *)p)<=bgbgc_precise_end))
	{
		i=((byte *)p)-bgbgc_precise_base;

		chk=bgbgc_chunktab[i>>BGBGC_BLKSHIFT];
		i&=((1<<BGBGC_BLKSHIFT)-1);

		j=BGBGC_ChunkGetEnt(chk, i);

		if(!i)return(-1);	//invalid
		if((j&3)!=1)return(-1);	//invalid
		if((j&12)==4)return(1);	//allready marked

		if((j&12)!=12)
		{
			for(j=0;; j++)
			{
				k=BGBGC_ChunkGetEnt(chk, i+j);
				if(j && ((k&3)!=2))break;
				BGBGC_ChunkSetEnt(chk, i+j, (k&(~12))|4);
			}
		}

#if 0
		k=BGBGC_ChunkGetEnt(chk, i);
		if(!(k&16))
		{
			p=((void *)chk)+(i<<BGBGC_CELLSHIFT);
			k=BGBGC_HandleMark(p);
			if(k<0)BGBGC_ScanRange((void **)p,
				j<<(BGBGC_CELLSHIFT-BGBGC_WORDSHIFT));
		}
#endif

		k=BGBGC_ChunkGetEnt(chk, i);
		switch((k>>4)&7)
		{
		case 0:
//			p=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
			p=(chk->data)+(i<<BGBGC_CELLSHIFT);
			k=BGBGC_HandleMark(p);
			if(k<0)BGBGC_ScanRange((void **)p,
				j<<(BGBGC_CELLSHIFT-BGBGC_WORDSHIFT));
			break;
		case 1: break;
		case 2: case 3: case 4: case 5: case 6: case 7:
//			p=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
			p=(chk->data)+(i<<BGBGC_CELLSHIFT);
			k=BGBGC_HandleMark(p);
//			if(k<0)BGBGC_ScanRange((void **)p,
//				j<<(BGBGC_CELLSHIFT-BGBGC_WORDSHIFT));
			break;
		}

		return(0);
	}

#ifdef BGBGC_USECONS
	if((((byte *)p)>=bgbgc_precise_cbase) &&
		(((byte *)p)<=bgbgc_precise_cend))
	{
		i=((byte *)p)-bgbgc_precise_cbase;
		return(BGBGC_MarkConsPrecise(i));
	}
#endif

	return(BGBGC_MarkObj(p));
}
#endif

int BGBGC_CheckObjPtr(void *p)
{
	BGBGC_Chunk *chk;
	int i, j, k;

	if(!p)return(-1);	//NULL

#ifdef BGBGC_USECONS
	i=BGBGC_CheckCons(p);
	if(i>=0)
	{
		BGBGC_MarkCons(p, i);
		return(0);
	}
#endif

	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);		//2012-10-24
		i=BGBGC_LookupLObjS2(p);
		if(i>=0)return(0);
		return(-1);
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;
	j=0;

	for(; i>0; i--)
	{
		j=BGBGC_ChunkGetEnt(chk, i);
		if((j&3)!=2)break;
	}

	if((j&3)!=1)return(-1);	//invalid
	if(!i)return(-1);	//invalid
	return(0);
}

int BGBGC_SetLockObj(void *p, int l)
{
	BGBGC_Chunk *chk;
	int i, j, k;

	if(!p)return(-1);	//NULL
	chk=BGBGC_GetChunk(p);
	if(!chk)
	{
//		i=BGBGC_LookupLObj(p);		//2012-10-24
		i=BGBGC_LookupLObjS2(p);
		if(i>=0)
		{
			if(l)bgbgc_lobj_flag[i]|=32;
			if(!l)bgbgc_lobj_flag[i]&=~32;
			return(0);
		}

		return(-1);	//invalid
	}

//	i=((int)(((byte *)p)-((byte *)chk)))>>BGBGC_CELLSHIFT;
	i=((int)(((byte *)p)-chk->data))>>BGBGC_CELLSHIFT;
	for(; i>0; i--)
	{
		j=BGBGC_ChunkGetEnt(chk, i);
		if((j&3)!=2)break;
	}
	if(!i)return(-1);	//invalid
	if((j&3)!=1)return(-1);	//invalid

	if(l && ((j&12)!=12))
		for(j=0;; j++)
	{
		k=BGBGC_ChunkGetEnt(chk, i+j);
		if(j && ((k&3)!=2))break;
		BGBGC_ChunkSetEnt(chk, i+j, (k&(~12))|12);
	}

	if(!l && ((j&12)==12))
		for(j=0;; j++)
	{
		k=BGBGC_ChunkGetEnt(chk, i+j);
		if(j && ((k&3)!=2))break;
		BGBGC_ChunkSetEnt(chk, i+j, (k&(~12))|8);
	}

	return(0);
}

#ifdef BGBGC_USEPRECISE
int BGBGC_IncRef(void *p)
{
	BGBGC_Chunk *chk;
	int i, j, k;

	if(!p)return(-1);

	if((((byte *)p)>=bgbgc_precise_base) &&
		(((byte *)p)<=bgbgc_precise_end))
	{
		i=((byte *)p)-bgbgc_precise_base;

		chk=bgbgc_chunktab[i>>BGBGC_BLKSHIFT];
		i&=((1<<BGBGC_BLKSHIFT)-1);

		k=BGBGC_ChunkGetEnt(chk, i);
		switch((k>>4)&7)
		{
		case 0: break;	//conservative
		case 1: break;	//atomic (conservative)
		case 2: break;	//defiled
		case 3: case 4: case 5: case 6:
			BGBGC_ChunkSetEnt(chk, i, k+16); break;
		case 7: break;	//many
		}

		return(0);
	}

#ifdef BGBGC_USECONS
	if((((byte *)p)>=bgbgc_precise_cbase) &&
		(((byte *)p)<=bgbgc_precise_cend))
			return(BGBGC_IncCons(p));
#endif

	return(0);
}

int BGBGC_DecRef(void *p)
{
	BGBGC_Chunk *chk;
	int i, j, k;

	if(!p)return(-1);

	if((((byte *)p)>=bgbgc_precise_base) &&
		(((byte *)p)<=bgbgc_precise_end))
	{
		i=((byte *)p)-bgbgc_precise_base;

		chk=bgbgc_chunktab[i>>BGBGC_BLKSHIFT];
		i&=((1<<BGBGC_BLKSHIFT)-1);

		k=BGBGC_ChunkGetEnt(chk, i);
		switch((k>>4)&7)
		{
		case 0: break;	//conservative
		case 1: break;	//atomic (conservative)
		case 2: break;	//defiled
		case 3: case 4:
//			BGBGC_Free(p);
//			BGBGC_Free(((byte *)chk)+(i<<BGBGC_CELLSHIFT));
			BGBGC_Free((chk->data)+(i<<BGBGC_CELLSHIFT));
			break;
		case 5: case 6:
			BGBGC_ChunkSetEnt(chk, i, k-16); break;
		case 7: break;	//many
		}

		return(0);
	}

#ifdef BGBGC_USECONS
	if((((byte *)p)>=bgbgc_precise_cbase) &&
		(((byte *)p)<=bgbgc_precise_cend))
			return(BGBGC_DecCons(p));
#endif

	return(0);
}

int BGBGC_SafeDecRef(void *p)
{
	BGBGC_Chunk *chk;
	int i, j, k;

	if(!p)return(-1);

	if((((byte *)p)>=bgbgc_precise_base) &&
		(((byte *)p)<=bgbgc_precise_end))
	{
		i=((byte *)p)-bgbgc_precise_base;

		chk=bgbgc_chunktab[i>>BGBGC_BLKSHIFT];
		i&=((1<<BGBGC_BLKSHIFT)-1);

		k=BGBGC_ChunkGetEnt(chk, i);
		switch((k>>4)&7)
		{
		case 0: break;	//conservative
		case 1: break;	//atomic (conservative)
		case 2: break;	//defiled
		case 3: break;	//count=0
		case 4: case 5: case 6:
			BGBGC_ChunkSetEnt(chk, i, k-16); break;
		case 7: break;	//many
		}

		return(0);
	}

#ifdef BGBGC_USECONS
	if((((byte *)p)>=bgbgc_precise_cbase) &&
		(((byte *)p)<=bgbgc_precise_cend))
			return(BGBGC_SafeDecCons(p));
#endif

	return(0);
}

void *BGBGC_Defile(void *p)
{
	BGBGC_Chunk *chk;
	void *q;
	int i, j, k;

	if(!p)return(NULL);

	if((((byte *)p)>=bgbgc_precise_base) &&
		(((byte *)p)<=bgbgc_precise_end))
	{
		i=((byte *)p)-bgbgc_precise_base;

		chk=bgbgc_chunktab[i>>BGBGC_BLKSHIFT];
		i&=((1<<BGBGC_BLKSHIFT)-1);

		k=BGBGC_ChunkGetEnt(chk, i);
		switch((k>>4)&7)
		{
		case 0: break;	//conservative
		case 1: break;	//atomic (conservative)
		case 2: break;	//defiled
		case 3: case 4: case 5: case 6: case 7:
			BGBGC_ChunkSetEnt(chk, i, (k&0x8F)|0x20);
			break;
		}

//		q=((byte *)chk)+(i<<BGBGC_CELLSHIFT);
		q=(chk->data)+(i<<BGBGC_CELLSHIFT);
		return(q);
	}

#ifdef BGBGC_USECONS
	if((((byte *)p)>=bgbgc_precise_cbase) &&
		(((byte *)p)<=bgbgc_precise_cend))
			return(BGBGC_DefileCons(p));
#endif

	return(p);
}

void *BGBGC_UnDefile(void *p)
{
	void *q;

	if(!p)return(NULL);

	if((((byte *)p)>=bgbgc_precise_base) &&
		(((byte *)p)<=bgbgc_precise_end))
			return(p);

	if((((byte *)p)>=bgbgc_precise_cbase) &&
		(((byte *)p)<=bgbgc_precise_cend))
			return(p);

	q=BGBGC_TryGetPrecise(p);
	if(q)return(q);

#ifdef BGBGC_USECONS
	q=BGBGC_UnDefileCons(p);
	if(q)return(q);
#endif

	return(p);
}

void *BGBGC_PreciseGetPtr(void *p)
{
	BGBGC_Chunk *chk;
	void *q;
	int i, j, k;

	if(!p)return(NULL);

	if((((byte *)p)>=bgbgc_precise_base) &&
		(((byte *)p)<=bgbgc_precise_end))
	{
		i=((byte *)p)-bgbgc_precise_base;
		chk=bgbgc_chunktab[i>>BGBGC_BLKSHIFT];
		i&=((1<<BGBGC_BLKSHIFT)-1);
		q=(chk->data)+(i<<BGBGC_CELLSHIFT);
		return(q);
	}

#ifdef BGBGC_USECONS
	if((((byte *)p)>=bgbgc_precise_cbase) &&
		(((byte *)p)<=bgbgc_precise_cend))
			return(BGBGC_PreciseGetConsPtr(p));
#endif

	return(p);
}

void *BGBGC_AllocPrecise(int size)
{
	void *p;

	p=BGBGC_Alloc(size);
	if(!p)return(NULL);

	p=BGBGC_MakePrecise(p);
	return(p);
}

#endif

/** Scan Range Sequentially */
BGBGC_API int BGBGC_ScanRange0(void **p, int cnt)
{
	int i;

#ifdef MSVC
//	__try {
#endif
	for(i=0; i<cnt; i++)
		BGBGC_Mark(p[i]);
#ifdef MSVC
//	} __except(EXCEPTION_EXECUTE_HANDLER)
//		{ return(-1); }
#endif
	return(0);
}

/** Scan Range Sequentially, in reverse */
BGBGC_API int BGBGC_ScanRange2(void **p, int cnt)
{
	int i;

#ifdef MSVC
	__try {
#endif

//	for(i=0; i<cnt; i++)
//		BGBGC_Mark(p[i]);
	for(i=cnt-1; i>=0; i--)
		BGBGC_Mark(p[i]);

#ifdef MSVC
	}
//	__except(GetExceptionCode() == EXCEPTION_INT_DIVIDE_BY_ZERO ? 
//		EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
	__except(EXCEPTION_EXECUTE_HANDLER)
		{ return(-1); }

#endif
	return(0);
}

/** Scan Range Sequentially, Try to skip errors */
BGBGC_API int BGBGC_ScanRange3(void **p, int cnt)
{
	int i;

	for(i=0; i<cnt;)
	{
#ifdef MSVC
		__try {
#endif
		for(; i<cnt; i++)
			BGBGC_Mark(p[i]);
#ifdef MSVC
		} __except(EXCEPTION_EXECUTE_HANDLER)
		{
			i+=4096/sizeof(void *);
			// i-=1;
			continue;
		}
#endif
	}
	return(0);
}

BGBGC_API int BGBGC_ScanRange(void **p, int cnt)
{
	return(BGBGC_ScanRange3(p, cnt));
}

int BGBGC_PreSweepChk(BGBGC_Chunk *chk) //AH:ignore
{
	int i, j, k, cells;
	int csize, mapsize;
	void *p;

	cells=chk->cells;
	for(i=0; i<cells; i++)
	{
		j=chk->map[i]; k=j&15;

		if(k==BGBGC_MAP_TYPE_HEAD)	//unmarked object
		{
			p=(chk->data)+(i<<BGBGC_CELLSHIFT);
			k=BGBGC_HandleDestroy(p);
			if(k>0)BGBGC_Mark(p);
			if(k==-2)BGBGC_PrintBacktrack(chk, i);
			continue;
		}

		if(k==BGBGC_MAP_LOCK_HEAD)	//locked object
		{
			p=(chk->data)+(i<<BGBGC_CELLSHIFT);
			BGBGC_Mark(p);
			continue;
		}
	}
	return(0);
}

int BGBGC_PreSweep()
{
	BGBGC_Chunk *chk;
	int i, j;

	chk=bgbgc_initchain;
	while(chk)
	{
		BGBGC_PreSweepChk(chk);
		chk=chk->next;
	}

	for(i=0; i<bgbgc_lobj_num; i++)
		if(bgbgc_lobj_flag[i]&32)
			BGBGC_Mark(bgbgc_lobj[i]);

	for(i=0; i<bgbgc_lobj_num; i++)
		if(!(bgbgc_lobj_flag[i]&1))
	{
		j=BGBGC_HandleDestroy(bgbgc_lobj[i]);
		if(j==-2)printf("\tIs LObj\n");
	}

	return(0);
}

int BGBGC_SweepChk(BGBGC_Chunk *chk) //AH:ignore
{
	int i, j, k, cells;
	int csize, mapsize;
	void *p;


	cells=chk->cells;
	for(i=0; i<cells; i++)
	{
//		j=BGBGC_ChunkGetEnt(chk, i);
		j=chk->map[i];
		if((j&3)==BGBGC_MAP_TYPE_RESV)continue;

		if(j)
		{
//			if(!(j&12))
			if(((j&BGBGC_MAP_MARK_MASK)==BGBGC_MAP_MARK_WHITE) &&
				!(j&BGBGC_MAP_CONSV_MARK))
			{
				p=(chk->data)+(i<<BGBGC_CELLSHIFT);

#ifdef X86
				((void **)p)[0]=(void *)0xDEAD0047;
				((void **)p)[1]=(void *)0xDEAD1047;
				((void **)p)[2]=(void *)0xDEAD2047;
				((void **)p)[3]=(void *)0xDEAD3047;
#endif

#ifdef X86_64
				((void **)p)[0]=(void *)0xDEADF00DBAAD0047ULL;
				((void **)p)[1]=(void *)0xDEADF00DBAAD1047ULL;
#endif

//				memset(p, 0xff, 1<<BGBGC_CELLSHIFT);
				j=0;
				bgbgc_freed_cells++;
			}else
			{
				bgbgc_used_cells++;
			}

//			chk->map[i]=j&(~12);
			if((j&BGBGC_MAP_MARK_MASK)==BGBGC_MAP_MARK_WHITE)
			{
				chk->map[i]=j&(~0x8C);
			}else
			{
				chk->map[i]=(j&(~0x0C))|BGBGC_MAP_CONSV_MARK;
			}

//			chk->map[i]=j&(~0x8C);
		}else bgbgc_free_cells++;
		bgbgc_total_cells++;
	}

	chk->rover=0;

	return(0);
}

int BGBGC_Sweep()
{
	BGBGC_Chunk *chk;
	int i;

	chk=bgbgc_initchain;
	while(chk)
	{
		BGBGC_SweepChk(chk);
		chk=chk->next;
	}

	for(i=0; i<bgbgc_lobj_num; i++)
	{
		if(!(bgbgc_lobj_flag[i]&1))
		{
			BGBGC_FreeLObjIdx(i);
		}else
		{
			bgbgc_lobj_flag[i]&=~1;
		}
	}

	BGBGC_SweepCons();

	return(0);
}

int BGBGC_PostSweepChk(BGBGC_Chunk *chk) //AH:ignore
{
	int i, j, k, cells;
	int csize, mapsize;
	void *p;

	cells=chk->cells;
	for(i=0; i<cells; i++)
	{
		j=chk->map[i];
		if((j&3)==1)
		{
			p=(chk->data)+(i<<BGBGC_CELLSHIFT);
			k=BGBGC_HandlePreserve(p);
			if(k==-2)BGBGC_PrintBacktrack(chk, i);
		}
	}
	return(0);
}

int BGBGC_PostSweep()
{
	BGBGC_Chunk *chk;
	int i, k;

	chk=bgbgc_initchain;
	while(chk)
	{
		BGBGC_PostSweepChk(chk);
		chk=chk->next;
	}

	for(i=0; i<bgbgc_lobj_num; i++)
		if(bgbgc_lobj[i])
	{
		k=BGBGC_HandlePreserve(bgbgc_lobj[i]);
		if(k==-2)printf("\tIs LObj\n");
	}

	return(0);
}

int BGBGC_StackBase(void *p)
{
	int i;
	bgbgc_stackbase=p;
	return(0);
}

int BGBGC_ScanStack()
{
	void **sp;

	printf("StackBase %p\n", bgbgc_stackbase);

	if(!bgbgc_stackbase)return(-1);
	sp=(void **)bgbgc_stackbase;
	sp-=1<<16;	//reasonable guess

	BGBGC_ScanRange2(sp, 1<<16);

#if 0
	sp=(void **)&sp;
	while(((void *)sp)<bgbgc_stackbase)
	{
		BGBGC_Mark(*sp);
		sp++;
	}
#endif
	return(0);
}

int BGBGC_Collect()
{
	BGBGC_SpawnThread();

	if(bgbgc_spawngc<0)
	{
		BGBGC_RunCollect();
		return(0);
	}

	if(bgbgc_rungc)
	{
		while(bgbgc_rungc)thSleep(0);
		return(0);
	}

	bgbgc_reqgc=1;

	while(bgbgc_reqgc)thSleep(0);
	while(bgbgc_rungc)thSleep(0);
	return(0);
}

void BGBGC_CollectHandlePause()
{
	return;

	while(bgbgc_reqpause)
	{
		bgbgc_pauseok=1;
		while(bgbgc_reqpause)
			thSleep(1);
		bgbgc_pauseok=0;
	}
}

#ifdef linux
extern void *__data_start;
extern void *__bss_start;
extern void *_edata;
extern void *_end;
#endif

int BGBGC_RunCollect()
{
	int t, t2;
	int i, j;

	if(bgbgc_rungc)
	{
		while(bgbgc_rungc)thSleep(0);
		return(0);
	}

	if(!bgbgc_markstack)
	{
		i=BGBGC_MARKSIZE;
		bgbgc_markstack=malloc(i*sizeof(void *));
		bgbgc_markstacklim=i;
		bgbgc_pmarkstack=malloc(i*sizeof(void *));
		bgbgc_pmarkstacklim=i;
	}

	BGBGC_CollectHandlePause();

//	BGBGC_SuspendThreads();
	gcFlushTypeCache();

	bgbgc_reqpause=0; //make sure this is not set (deadlock)


	bgbgc_markstackpos=0;
	bgbgc_pmarkstackpos=0;

//	bgbgc_reqpause=0;
	bgbgc_pauseok=0;

	bgbgc_markok=1;
	bgbgc_rungc=1;
	bgbgc_reqgc=0;

	bgbgc_used_cells=0;
	bgbgc_free_cells=0;
	bgbgc_freed_cells=0;
	bgbgc_total_cells=0;

	printf("BGBGC_Collect: Begin\n");
	t=clock();

	BGBGC_LockGC2();

#ifdef BGBGC_TINYLIST
	BGBGC_FlushTiny();
#endif

	BGBGC_CollectHandlePause();

	BGBGC_ShiftLObj();

	bgbgc_num_lobjs=bgbgc_lobj_num;
	bgbgc_size_lobjs=0;
	for(i=0; i<bgbgc_num_lobjs; i++)
		bgbgc_size_lobjs+=bgbgc_lobj_size[i];

	BGBGC_CollectHandlePause();

	BGBGC_ScanStack();

#ifdef MSVC

//errm... yeah...

#else
#ifdef WIN32
	i=(&_data_end__)-(&_data_start__);
	j=(&_bss_end__)-(&_bss_start__);
//	BGBGC_ScanRange((void **)&_data_start__, i/sizeof(void *));
//	BGBGC_ScanRange((void **)&_bss_start__, j/sizeof(void *));
	BGBGC_ScanRange((void **)&_data_start__, i);
	BGBGC_ScanRange((void **)&_bss_start__, j);
#endif

#ifdef linux
	//need figure out...
//	i=(&__data_end)-(&__data_start);
//	j=(&__bss_end)-(&__bss_start);
//	BGBGC_ScanRange((void **)&__data_start, i/sizeof(void *));
//	BGBGC_ScanRange((void **)&__bss_start, j/sizeof(void *));

	i=(&_edata)-(&__data_start);
	j=(&_end)-(&__bss_start);
	BGBGC_ScanRange((void **)&__data_start, i);
	BGBGC_ScanRange((void **)&__bss_start, j);
#endif
#endif

	for(i=0; i<bgbgc_nmarkhdlr; i++)
		bgbgc_markhdlr[i]();

#ifdef BGBGC_USEPRECISE
	for(i=0; i<bgbgc_gcp_nroot; i++)
		BGBGC_MarkPrecise(bgbgc_gcp_root[i][0]);
#endif

	BGBGC_CollectHandlePause();

	if(bgbgc_markstackpos)
		printf("GC: MarkStackPos=%d (N)\n", bgbgc_markstackpos);

#if 0
	while(bgbgc_markstackpos>0)
	{
		bgbgc_markstackpos--;
		BGBGC_MarkObj((void *)bgbgc_markstack[bgbgc_markstackpos]);
		BGBGC_CollectHandlePause();
	}
#endif

	while(1)
	{
		BGBGC_CollectHandlePause();

#ifdef BGBGC_USEPRECISE
		if(bgbgc_pmarkstackpos>0)
		{
			bgbgc_pmarkstackpos--;
			BGBGC_MarkObjPrecise((void *)
				bgbgc_pmarkstack[bgbgc_pmarkstackpos]);
			continue;
		}
#endif

		if(bgbgc_markstackpos>0)
		{
			bgbgc_markstackpos--;
			BGBGC_MarkObj((void *)
				bgbgc_markstack[bgbgc_markstackpos]);
			continue;
		}

		break;
	}

	if(bgbgc_markstackpos)
		printf("GC: MarkStackPos=%d (A)\n", bgbgc_markstackpos);


	BGBGC_PreSweep();

#if 0
	while(bgbgc_markstackpos>0)
	{
		bgbgc_markstackpos--;
		BGBGC_MarkObj((void *)bgbgc_markstack[bgbgc_markstackpos]);
		BGBGC_CollectHandlePause();

		if(!bgbgc_markstackpos)
		{
			bgbgc_markok=0;
			thSleep(0);
			if(bgbgc_markstackpos)
				bgbgc_markok=1;
			continue;
		}
	}
#endif

	while(1)
	{
		BGBGC_CollectHandlePause();

#ifdef BGBGC_USEPRECISE
		if(bgbgc_pmarkstackpos>0)
		{
			bgbgc_pmarkstackpos--;
			BGBGC_MarkObjPrecise((void *)
				bgbgc_pmarkstack[bgbgc_pmarkstackpos]);
			continue;
		}
#endif

		if(bgbgc_markstackpos>0)
		{
			bgbgc_markstackpos--;
			BGBGC_MarkObj((void *)
				bgbgc_markstack[bgbgc_markstackpos]);
			continue;
		}

		bgbgc_markok=0;
		thSleep(0);
		if(bgbgc_markstackpos || bgbgc_pmarkstackpos)
		{
			bgbgc_markok=1;
			continue;
		}

		break;
	}

	if(bgbgc_markstackpos)
		printf("GC: MarkStackPos=%d (B)\n", bgbgc_markstackpos);

	bgbgc_markok=0;

	BGBGC_Sweep();
	BGBGC_PostSweep();
	BGBGC_ShiftLObj2();

	bgbgc_freed_size_lobjs=0;
	for(i=0; i<bgbgc_lobj_num; i++)
		bgbgc_freed_size_lobjs+=bgbgc_lobj_size[i];

	BGBGC_UnlockGC2();

	printf("%d cells (%dKiB) used, %d cells (%dKiB) free, "
		"%d cells (%dKiB) total\n",
		bgbgc_used_cells, BGBGC_CELLKIB(bgbgc_used_cells),
		bgbgc_free_cells, BGBGC_CELLKIB(bgbgc_free_cells),
		bgbgc_total_cells, BGBGC_CELLKIB(bgbgc_total_cells));
	printf("%d cells (%dKiB) collected, "
		"%d cells (%dKiB) manually freed\n",
		bgbgc_freed_cells, BGBGC_CELLKIB(bgbgc_freed_cells),
		bgbgc_manual_cells, BGBGC_CELLKIB(bgbgc_manual_cells));

	printf("%d lobjs, %d lobjs freed, "
			"%dKiB used, %dKiB freed (%dKiB lim)\n",
		bgbgc_lobj_num,
		bgbgc_num_lobjs-bgbgc_lobj_num,
		(bgbgc_freed_size_lobjs+1023)/1024,
		(bgbgc_size_lobjs-bgbgc_freed_size_lobjs+1023)/1024,
		(bgbgc_size_lim_lobjs+1023)/1024);

	printf("%d cons (%dKiB) used, "
			"%d cons (%dKiB) collected, "
			"%d cons (%dKiB) total\n",
		bgbgc_used_cons, BGBGC_CONSKIB(bgbgc_used_cons),
		bgbgc_freed_cons, BGBGC_CONSKIB(bgbgc_freed_cons),
		bgbgc_total_cons, BGBGC_CONSKIB(bgbgc_total_cons));

	bgbgc_manual_cells=0;
	bgbgc_size_lobjs=bgbgc_freed_size_lobjs;

	bgbgc_est_used_cells=bgbgc_used_cells;
	bgbgc_est_free_cells=bgbgc_free_cells;

	t2=clock();
	printf("BGBGC_Collect: End (took %.3fs)\n",
		((float)(t2-t))/CLOCKS_PER_SEC);

	printf("\n");

	bgbgc_lastgc=clock();
	bgbgc_rungc=0;

	gcFlushTypeCache();

	BGBGC_ResumeThreads();

	return(0);
}

void BGBGC_CollectAssign(void **pp, void *p)
{
	if(!bgbgc_rungc)
	{
		*pp=p;
		return;
	}

	//if still in mark pass, submit pointers
	if(bgbgc_markok)
	{
		BGBGC_Mark(*pp);
		BGBGC_Mark(p);
		BGBGC_Mark(pp);

		*pp=p;
		return;
	}

	//sweep pass, delay until GC is done
	while(bgbgc_rungc)
		thSleep(1);
	*pp=p;
}

void BGBGC_WaitIfGC()
{
	while(bgbgc_rungc)
		thSleep(1);
}

void BGBGC_TryMarkSafe(void *p)
{
	if(!bgbgc_rungc)
		return;

	//if still in mark pass, submit pointers
	if(bgbgc_markok)
		{ BGBGC_Mark(p); return; }
}


#ifdef BGBGC_USEPRECISE
void BGBGC_CollectAssignPrecise(void **pp, void *p)
{
	if(!bgbgc_rungc)
	{
		BGBGC_DecRef(*pp);
		BGBGC_IncRef(p);
		*pp=p;
		return;
	}

	//if still in mark pass, submit pointers
	if(bgbgc_markok)
	{
		BGBGC_MarkPrecise(*pp);
		BGBGC_MarkPrecise(p);
		BGBGC_MarkPrecise(pp);

		BGBGC_DecRef(*pp);
		BGBGC_IncRef(p);

		*pp=p;
		return;
	}

	//sweep pass, delay until GC is done
	while(bgbgc_rungc)
		thSleep(1);

	BGBGC_DecRef(*pp);
	BGBGC_IncRef(p);
	*pp=p;
}
#endif

void BGBGC_CollectDelay()
{
	if(!bgbgc_rungc)return;

	BGBGC_UnlockGC();
	while(bgbgc_rungc)
		thSleep(1);
	BGBGC_LockGC();
}

void BGBGC_CollectPause()
{
	if(!bgbgc_rungc)return;

	BGBGC_UnlockGC();
	bgbgc_reqpause++;
	while(bgbgc_rungc && !bgbgc_pauseok)
		thSleep(1);
	BGBGC_LockGC();
}

void BGBGC_CollectUnpause()
{
	if(!bgbgc_rungc)return;
	bgbgc_reqpause--;
}

void BGBGC_CollectUnpauseDelay()
{
	if(!bgbgc_rungc)return;

	BGBGC_UnlockGC();
	bgbgc_reqpause--;
	while(bgbgc_rungc)
		thSleep(1);
	BGBGC_LockGC();
}

int BGBGC_GetMemoryUseStatus(void)
{
	s64 i;
	int fl;
	
	fl=0;
	if(!BGBGC_CheckHeapBelowBaseLimit())
		fl|=1;

	i=768*(1LL<<20);
//	i=512*(1LL<<20);
	if(bgbgc_size_lobjs>=i)
		fl|=2;
	
	return(fl);
}

int BGBGC_CollectLoop(void *p)
{
	s64 i, j;

	if(bgbgc_spawngc)
	{
		printf("GC: Collect Loop Bad Spawn State %d\n",
			bgbgc_spawngc);
		return(-1);
	}

	if(!bgbgc_markstack)
	{
		i=BGBGC_MARKSIZE;
		bgbgc_markstack=malloc(i*sizeof(void *));
		bgbgc_markstacklim=i;
		bgbgc_pmarkstack=malloc(i*sizeof(void *));
		bgbgc_pmarkstacklim=i;
	}

	bgbgc_spawngc=1;

	while(1)
	{
		i=clock()-bgbgc_lastgc;
//		if(bgbgc_rungc || (i>=0) && (i<CLOCKS_PER_SEC))
//		{
//			thSleep(10);
//			continue;
//		}

		if(bgbgc_spawngc<0)
		{
			printf("GC: Termination Request (bgbgc_spawngc<0)\n");
			break;
		}

		if(bgbgc_rungc)
		{
			thSleep(10);
			continue;
		}

		if(bgbgc_reqgc)
		{
			printf("GC: A (Requested GC)\n");
			BGBGC_RunCollect();

			i=(bgbgc_total_cells*2LL)/4;
			if(bgbgc_est_used_cells>i)
				BGBGC_NewBlock(0, 0);

			i=(bgbgc_size_lim_lobjs*2LL)/4;
			if(bgbgc_size_lobjs>=i)
				bgbgc_size_lim_lobjs+=i;

//			if(bgbgc_size_lim_lobjs>(1LL<<30))
//				bgbgc_size_lim_lobjs=(1LL<<30);

			if((bgbgc_size_lim_lobjs<0) ||
					(bgbgc_size_lim_lobjs>(1LL<<30)))
				bgbgc_size_lim_lobjs=(1LL<<30);

			continue;
		}

#if 1
		i=(bgbgc_total_cells*3LL)/4;
		if(i<0)
		{
			printf("GC: Heap Cells Limit Overflow\n");
			break;
		}
		if(bgbgc_est_used_cells>i)
		{
			j=clock()-bgbgc_lastgc;
			if((j>=0) && (j<(30*CLOCKS_PER_SEC)))
			{
				BGBGC_NewBlock(0, 0);
				thSleep(10);
				continue;
			}
			
			if(BGBGC_CheckNoGC() || BGBGC_CheckHeapBelowBaseLimit())
			{
				BGBGC_NewBlock(0, 0);
				thSleep(10);
				continue;
			}

			printf("GC: B (Cell Usage > 75%) %d %d\n",
				bgbgc_est_used_cells, i);
			BGBGC_RunCollect();

			i=(bgbgc_total_cells*2LL)/4;
			if(i<0)break;
			if(bgbgc_est_used_cells>i)
				BGBGC_NewBlock(0, 0);

			continue;
		}

		i=(bgbgc_size_lim_lobjs*3LL)/4;
//		if(i<0)break;
		if(i<0)
		{
			printf("GC: Heap LObj Limit Overflow\n");
			break;
		}
		if(bgbgc_size_lobjs>=i)
		{
		
			bgbgc_size_lobjs=BGBGC_SizeLObjs();
			if(bgbgc_size_lobjs<i)
			{
				printf("GC: C, False Hit\n");
				thSleep(10);
				continue;
			}
		
			j=clock()-bgbgc_lastgc;
			if((j>=0) && (j<(30*CLOCKS_PER_SEC)))
			{
				bgbgc_size_lim_lobjs+=bgbgc_size_lim_lobjs>>1;
//				if(bgbgc_size_lim_lobjs<0)break;

				if((bgbgc_size_lim_lobjs<0) ||
						(bgbgc_size_lim_lobjs>(1LL<<30)))
					bgbgc_size_lim_lobjs=(1LL<<30);

				thSleep(10);
				continue;
			}

			if(BGBGC_CheckNoGC())
			{
				bgbgc_size_lim_lobjs+=bgbgc_size_lim_lobjs>>1;
//				if(bgbgc_size_lim_lobjs<0)break;

				if((bgbgc_size_lim_lobjs<0) ||
						(bgbgc_size_lim_lobjs>(1LL<<30)))
					bgbgc_size_lim_lobjs=(1LL<<30);

				thSleep(10);
				continue;
			}

			printf("GC: C (LObj Usage, LObjs %d/%d)\n",
				(int)bgbgc_size_lobjs,
				(int)bgbgc_size_lim_lobjs);

			BGBGC_RunCollect();

			i=(bgbgc_size_lim_lobjs*2LL)/4;
			if(bgbgc_size_lobjs>=i)
				bgbgc_size_lim_lobjs+=i;
//			if(bgbgc_size_lim_lobjs<0)break;

			if((bgbgc_size_lim_lobjs<0) ||
					(bgbgc_size_lim_lobjs>(1LL<<30)))
				bgbgc_size_lim_lobjs=(1LL<<30);

			continue;
		}
#endif

		BGBGC_DispatchWork();
		thSleep(10);
	}

	printf("GC: Collect Loop Exited\n");
	return(0);
}

int BGBGC_StartCollectLoop()
{
	thThread(BGBGC_CollectLoop, NULL);
	return(0);
}

int BGBGC_GetUsedCells()
{
	return(bgbgc_est_used_cells);
}

int BGBGC_GetFreeCells()
{
	return(bgbgc_est_free_cells);
}

BGBGC_API void BGBGC_AddMarkHandler(void (*fcn)())
{
	int i;
	i=bgbgc_nmarkhdlr++;
	if(i>=1024) { *(int *)-1=-1; }
	bgbgc_markhdlr[i]=fcn;
}

int BGBGC_SpawnThread()
{
	static int init=0;
	void *p;

	if(init)return(0);
	init=1;

	if(bgbgc_spawngc)
		return(0);

	p=thThread(BGBGC_CollectLoop, NULL);
	if(!p)
	{
		printf("BGBGC_SpawnThread: DeInit\n");
		bgbgc_spawngc=-1;
	}

	return(0);
}

int BGBGC_DeinitThread()
{
//	*(int *)-1=-1;
	printf("BGBGC_DeinitThread\n");
	bgbgc_spawngc=-1;
	return(0);
}
