/**
 * Fast Indexed Allocator.
 * This uses indexes giving the type and size of an object in order
 * to speed up allocation of the given type of object.
 * Suitible objects are to be small and fixed-size.
 * This may in-turn allow the use of slabs and dedicated free-lists.
 *
 * Note that this allocator does not require that the returned size
 * of allocated objects match that given on handle creation, nor
 * is it required to initialize the objects.
 */

#include <bgbgc.h>

static BGBGC_FastIndexInfo *bgbgc_fastidx_info[4096];
static void *bgbgc_fastidx_lock=NULL;

/**
 * Allocate an index which may be used to allocate objects of a
 * given type and size.
 * Returns -1 on failure (such as invalid parameters).
 * Valid index values will be greater than 0.
 */
BGBGC_API int gcFiAllocIndex(char *type, int size, int flags)
{
	BGBGC_FastIndexInfo *inf;
	char *s;
	int i, j, k, h;
	
	
	if(size>4096)
		return(-1);
	if(size<sizeof(void *))
		size=sizeof(void *);
	//pad up to a pointer-sized amount
	size=size+(sizeof(void *)-1);
	size&=~(sizeof(void *)-1);

	if(!bgbgc_fastidx_lock)
		bgbgc_fastidx_lock=thFastMutex();
	thLockFastMutex(bgbgc_fastidx_lock);
	
	s=type; h=0;
	while(*s)h=(h*251)+(*s++);
	h=(h*251)+size;
	h=(h*251)+flags;
	
	i=h*251; k=16;
	while((k--)>0)
	{
		j=(i>>8)&4095;
		inf=bgbgc_fastidx_info[j];
		if(!inf)
		{
			if(!j) { i=i*251; continue; }
			inf=malloc(sizeof(BGBGC_FastIndexInfo));
			memset(inf, 0, sizeof(BGBGC_FastIndexInfo));
			inf->name=gcInternType(type);
			inf->size=size;
			inf->flags=flags;
			inf->idx=j;
			inf->lock=thFastMutex();
			bgbgc_fastidx_info[j]=inf;
			thUnlockFastMutex(bgbgc_fastidx_lock);
			return(inf->idx);
		}
		if((inf->size==size) && !strcmp(inf->name, type))
		{
			thUnlockFastMutex(bgbgc_fastidx_lock);
			return(inf->idx);
		}
		i=i*251;
	}
	thUnlockFastMutex(bgbgc_fastidx_lock);
	return(-1);
}

/** Allocate an object using the given index. */
BGBGC_API void *gcFiIndexAlloc(int idx)
{
	BGBGC_FastIndexInfo *inf;
	int i, n;
	void *p, *q;
	byte *bp;

	inf=bgbgc_fastidx_info[idx];

	thLockFastMutex(inf->lock);
	if(inf->freeobjs)
	{
		p=inf->freeobjs;
		inf->freeobjs=*(void **)p;
		thUnlockFastMutex(inf->lock);
		return(p);
	}
	
	if(inf->flags&BGBGC_FIFL_SINGLEWRAP)
	{
		if(inf->flags&BGBGC_FIFL_EXEC)
			{ p=BGBGC_NewExec(inf->name, inf->size); }
		else
			{ p=BGBGC_New(inf->name, inf->size); }

		if(inf->flags&BGBGC_FIFL_ATOMIC)
			{ BGBGC_MarkAtomic(p); }

		thUnlockFastMutex(inf->lock);
		return(p);
	}
	
	n=65536/inf->size;
	if(inf->flags&BGBGC_FIFL_EXEC)
	{
		bp=BGBGC_NewExec(inf->name, 65536);
	}else
	{
		bp=BGBGC_New(inf->name, 65536);
	}
	if(inf->flags&BGBGC_FIFL_ATOMIC)
		{ BGBGC_MarkAtomic(bp); }

	for(i=0; i<n; i++)
	{
		q=bp+(i*inf->size);
		*(void **)q=inf->freeobjs;
		inf->freeobjs=q;
	}

//	p=BGBGC_New(inf->name, inf->size);
	p=inf->freeobjs;
	inf->freeobjs=*(void **)p;
	thUnlockFastMutex(inf->lock);
	return(p);
}

/** Free an object using the given index. */
BGBGC_API void gcFiIndexFree(int idx, void *obj)
{
	BGBGC_FastIndexInfo *inf;
	inf=bgbgc_fastidx_info[idx];
	thLockFastMutex(inf->lock);
	*(void **)obj=inf->freeobjs;
	inf->freeobjs=obj;
	thUnlockFastMutex(inf->lock);
}
