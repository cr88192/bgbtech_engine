#include <bgbgc.h>

#include <math.h>
// #include <complex.h>

FILE *bgbgc_log;

void (*bgbgc_loghook_f[1024])(char *str);
int bgbgc_nloghook=0;

void (*bgbgc_dbgloghook_f[1024])(int lvl, char *str);
int bgbgc_ndbgloghook=0;

char *(*bgbgc_typehook_f[1024])(void *p);
int bgbgc_ntypehook=0;

volatile int bgbgc_spawngc;	//GC thread spawned

static int init=0;
static int used_refcount=0;	//set if ref-counting has been used

void *bgbgc_ralloc_mutex;

BGBGC_API void gc_init(void *base)
{
	if(init)return;
	init=1;

	BGBGC_Init();
	BGBGC_InitTY();
	BGBGC_StackBase(base);

	BGBGC_ProbeDelayGC(2);		//assume app starting up

	bgbgc_ralloc_mutex=thFastMutex();

//	BGBGC_InitDyLL();
//	BGBGC_InitTypes();
//	dyopr_init();
}

BGBGC_API void gc_init2(int (*fcn)(void *ptr), void *ptr)
{
	void *p;

	if(init)return;
	init=1;

	bgbgc_spawngc=1;

	BGBGC_Init();
	BGBGC_InitTY();
	BGBGC_StackBase(&p);

	BGBGC_ProbeDelayGC(2);		//assume app starting up

//	BGBGC_InitDyLL();
//	BGBGC_InitTypes();
//	dyopr_init();

	bgbgc_ralloc_mutex=thFastMutex();

	thThread(fcn, ptr);

	bgbgc_spawngc=0;

	BGBGC_CollectLoop(NULL);
}

BGBGC_API void gc_init_nogc(void)
{
	if(init)return;
	init=1;

	BGBGC_Init();
	BGBGC_InitTY();
//	BGBGC_StackBase(base);

	BGBGC_ProbeDelayGC(5);		//assume app starting up

	bgbgc_ralloc_mutex=thFastMutex();
}

BGBGC_API void gc_deinit()
{
	if(bgbgc_log)
	{
		fflush(bgbgc_log);
//		fclose(bgbgc_log);
//		bgbgc_log=NULL;
	}

	BGBGC_DeinitThread();
}

BGBGC_API void gcAddTypeHook(char *(*fcn)(void *p))
{
	int i;
	
	for(i=0; i<bgbgc_ntypehook; i++)
		if(bgbgc_typehook_f[i]==fcn)
			return;
	
	i=bgbgc_ntypehook++;
	if(i>=1024) { *(int *)-1=-1; }
	bgbgc_typehook_f[i]=fcn;
}

BGBGC_API void gcAddLogHook(void (*fcn)(char *str))
{
	int i;

	for(i=0; i<bgbgc_nloghook; i++)
		if(bgbgc_loghook_f[i]==fcn)
			return;

	i=bgbgc_nloghook++;
	if(i>=1024) { *(int *)-1=-1; }
	bgbgc_loghook_f[i]=fcn;
}

BGBGC_API void gcAddDbgLogHook(void (*fcn)(int lvl, char *str))
{
	int i;

	for(i=0; i<bgbgc_ndbgloghook; i++)
		if(bgbgc_dbgloghook_f[i]==fcn)
			return;

	i=bgbgc_ndbgloghook++;
	if(i>=1024) { *(int *)-1=-1; }
	bgbgc_dbgloghook_f[i]=fcn;
}

BGBGC_API void gc_puts(char *str)
{
	int i;

	if(!bgbgc_log && (bgbgc_spawngc>=0))
		bgbgc_log=fopen("bgbgc_log.txt", "wt");

	if(bgbgc_log)
	{
		fputs(str, bgbgc_log);
		fflush(bgbgc_log);
	}

	for(i=0; i<bgbgc_nloghook; i++)
		bgbgc_loghook_f[i](str);
	for(i=0; i<bgbgc_ndbgloghook; i++)
		bgbgc_dbgloghook_f[i](0, str);

	fputs(str, stdout);
}

BGBGC_API void gc_printf(char *str, ...)
{
	char buf[65536];
	va_list lst;
	int i;

//	if(!bgbgc_log)
//		bgbgc_log=fopen("bgbgc_log.txt", "wt");

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	gc_puts(buf);
}

BGBGC_API void gc_dbgputs(int lvl, char *str)
{
	int i;

	if(lvl==BDBG_PRINT_DEFAULT)
		{ gc_puts(str); return; }

	if(bgbgc_log)
	{
		fprintf(bgbgc_log, "DBG%d: ", lvl);
		fputs(str, bgbgc_log);
		fflush(bgbgc_log);
	}

	for(i=0; i<bgbgc_ndbgloghook; i++)
		bgbgc_dbgloghook_f[i](lvl, str);

//	fputs(str, stdout);
}

BGBGC_API void gc_dbgprintf(int lvl, char *str, ...)
{
	char buf[65536];
	va_list lst;
	int i;

	if(!bgbgc_log && (bgbgc_spawngc>=0))
		bgbgc_log=fopen("bgbgc_log.txt", "wt");

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	gc_dbgputs(lvl, buf);
}

BGBGC_API void gc_logputs(char *str)
{
	int i;

	if(!bgbgc_log && (bgbgc_spawngc>=0))
		bgbgc_log=fopen("bgbgc_log.txt", "wt");

	if(bgbgc_log)
	{
		fputs(str, bgbgc_log);
		fflush(bgbgc_log);
	}

//	for(i=0; i<bgbgc_nloghook; i++)
//		bgbgc_loghook_f[i](str);
//	for(i=0; i<bgbgc_ndbgloghook; i++)
//		bgbgc_dbgloghook_f[i](0, str);

//	fputs(str, stdout);
}

/** printf which only goes to log */
BGBGC_API void gc_logprintf(char *str, ...)
{
	char buf[65536];
	va_list lst;
	int i;

//	if(!bgbgc_log)
//		bgbgc_log=fopen("bgbgc_log.txt", "wt");

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	gc_logputs(buf);
}

BGBGC_API int gcDelayGC(int mode)
{
	//stat(theoretically):
	//0: resume normal behavior
	//1: temporarily lock GC (temporarily disable)
	//2: temporarily delay GC
	//3: request GC
	//4: log memory allocations
	//5: disable GC (permanently disable)

	return(BGBGC_ProbeDelayGC(mode));
}

BGBGC_API int gcForceGC(int req)
{
	//0: do nothing
	//1: force GC collection
	if(req==1)
		BGBGC_Collect();
	return(0);
}

/** Reserve at least this much from OS.
  * Will not reduce heap size or prior limits.
  */
BGBGC_API int gcBaseHeap(size_t heap)
{
	BGBGC_BaseHeap(heap);
	return(0);
}

/** Allocate up to this limit before collecting.
  * This will not reduce a prior limit.
  */
BGBGC_API int gcBaseHeapLimit(size_t heap)
{
	BGBGC_BaseHeapLimit(heap);
	return(0);
}

/** How much memory to allow for large objects before GC.
  * This will not reduce a prior limit.
  */
BGBGC_API int gcBaseHeapLObj(size_t heap)
{
	BGBGC_BaseHeapLObj(heap);
	return(0);
}

/** How much memory to allow for cons cells before GC.
  * This will not reduce a prior limit.
  */
BGBGC_API int gcBaseHeapCons(size_t heap)
{
	BGBGC_BaseHeapCons(heap);
	return(0);
}

BGBGC_API int gcCheckMemoryLow(void)
{
//	BGBGC_BaseHeapCons(heap);
	return(BGBGC_GetMemoryUseStatus());
}

BGBGC_API void *gcalloc(size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New("_raw_t", sz);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gctalloc(char *ty, size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New(ty, sz);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gcallocLLn(size_t sz, char *fn, int ln)
{
	return(gctallocLLn("_raw_t", sz, fn, ln));
}

BGBGC_API void *gctallocLLn(char *ty, size_t sz, char *fn, int ln)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New(ty, sz);
//	BGBGC_SetObjLLn(p, fn, ln);
	BGBGC_SetObjLLnAccess(p, fn, ln, 0x00000F00);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gctallocLLnAcc(char *ty, size_t sz,
	char *fn, int ln, u32 access)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New(ty, sz);
	BGBGC_SetObjLLnAccess(p, fn, ln, access);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gcatomic(size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New("_atomic_raw_t", sz);
	BGBGC_MarkAtomic(p);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gctatomic(char *ty, size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New(ty, sz);
	BGBGC_MarkAtomic(p);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gcatomicLLn(size_t sz, char *fn, int ln)
{
	return(gctatomicLLn("_atomic_raw_t", sz, fn, ln));
}

BGBGC_API void *gctatomicLLn(char *ty, size_t sz, char *fn, int ln)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New(ty, sz);
	BGBGC_MarkAtomic(p);
//	BGBGC_SetObjLLn(p, fn, ln);
	BGBGC_SetObjLLnAccess(p, fn, ln, 0x00000F00);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gctatomicLLnAcc(char *ty, size_t sz,
	char *fn, int ln, u32 access)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New(ty, sz);
	BGBGC_MarkAtomic(p);
	BGBGC_SetObjLLnAccess(p, fn, ln, access);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gcweak(size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New("_weak_t", sz);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gcexec(size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_NewExec("_exec_raw_t", sz);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gctexec(char *ty, size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_NewExec(ty, sz);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gcallocrc(size_t sz)
	{ used_refcount=1; return(BGBGC_NewRC("_raw_rc_t", sz, 0)); }
BGBGC_API void *gcexecrc(size_t sz)
	{ used_refcount=1; return(BGBGC_NewRC("_exec_raw_rc_t", sz, 1)); }
BGBGC_API void *gctallocrc(char *ty, size_t sz)
	{ used_refcount=1; return(BGBGC_NewRC(ty, sz, 0)); }
BGBGC_API void *gctexecrc(char *ty, size_t sz)
	{ used_refcount=1; return(BGBGC_NewRC(ty, sz, 1)); }


BGBGC_API void *gcrealloc(void *p, size_t sz)
{
	BGBGC_SourceInfo tsrcinf;
	void *q;
	char *ty;
	int i, j;

	if(!p) return(gcalloc(sz));

	BGBGC_WaitIfGC();

	BGBGC_LockGC();

	ty=BGBGC_GetTypeName(p);
	i=BGBGC_GetSize(p);
	j=BGBGC_MarkAtomicP(p);
	BGBGC_GetObjSourceInfo(p, &tsrcinf);

	q=BGBGC_New(ty, sz);
	if(sz<i)i=sz;
	memcpy(q, p, i);
	if(j)BGBGC_MarkAtomic(q);
	BGBGC_SetObjSourceInfo(q, &tsrcinf);
	BGBGC_Free(p);

//	BGBGC_TryMarkSafe(q);
	BGBGC_UnlockGC();

	return(q);
}

BGBGC_API void *gcmemdup(void *p, size_t sz)
{
	BGBGC_SourceInfo tsrcinf;
	void *q;
	char *ty;
	int i, j;

	if(!p) return(gcalloc(sz));

	BGBGC_WaitIfGC();

	BGBGC_LockGC();

	ty=BGBGC_GetTypeName(p);
	i=BGBGC_GetSize(p);
	j=BGBGC_MarkAtomicP(p);
	BGBGC_GetObjSourceInfo(p, &tsrcinf);

	if(!ty)ty="_raw_t";
	if(!i)i=sz;

	q=BGBGC_New(ty, sz);
	if(sz<i)i=sz;
	memcpy(q, p, i);
	if(j)BGBGC_MarkAtomic(q);
	BGBGC_SetObjSourceInfo(q, &tsrcinf);
//	BGBGC_Free(p);

//	BGBGC_TryMarkSafe(q);
	BGBGC_UnlockGC();

	return(q);
}

//static void *bgbgc_gettype_lastptr;
//static void *bgbgc_gettype_lasttype;

static void *bgbgc_gettype_lastptr[256];
static void *bgbgc_gettype_lasttype[256];
static int bgbgc_gettype_laststat=0;

BGBGC_API void gcFlushTypeCache()
{
	int i;

	BGBGC_ObjType_FlushTypeCache();

	if(!bgbgc_gettype_laststat)
		return;

	for(i=0; i<256; i++)
	{
		bgbgc_gettype_lastptr[i]=NULL;
		bgbgc_gettype_lasttype[i]=NULL;
	}
	bgbgc_gettype_laststat=0;
}

BGBGC_API char *gcCheckPtrTypeCache(void *p)
{
	int i;

	i=(nlint)p;
	i=((i*65521)>>16)&0xFF;
	if(p && (p==bgbgc_gettype_lastptr[i]))
		return(bgbgc_gettype_lasttype[i]);
	return(NULL);
}

BGBGC_API void gcfree(void *p)
{
//	bgbgc_gettype_lastptr=NULL;
	BGBGC_LockGC();
	gcFlushTypeCache();
	BGBGC_Free(p);
	BGBGC_UnlockGC();
}

BGBGC_API char *gcgettype(void *p)
{
	char *s;
	int i;

//	if(p && (p==bgbgc_gettype_lastptr))
//		return(bgbgc_gettype_lasttype);
//	s=gcCheckPtrTypeCache(p);
//	if(s)return(s);

	BGBGC_LockGC();

	i=(nlint)p;
	i=((i*65521)>>16)&0xFF;
	if(p && (p==bgbgc_gettype_lastptr[i]))
	{
		s=bgbgc_gettype_lasttype[i];
		BGBGC_UnlockGC();
		return(s);
	}

//	BGBGC_LockGC();

	s=BGBGC_GetTypeName(p);
//	bgbgc_gettype_lastptr=p;
//	bgbgc_gettype_lasttype=s;

	bgbgc_gettype_lastptr[i]=p;
	bgbgc_gettype_lasttype[i]=s;
	bgbgc_gettype_laststat=1;

	BGBGC_UnlockGC();

	return(s);

//	return(BGBGC_GetTypeName(p));
}

BGBGC_API int gcgetsize(void *p)
{
	int i;
	BGBGC_LockGC();
	i=BGBGC_GetSize(p);
	BGBGC_UnlockGC();
	return(i);
}

BGBGC_API void *gcgetbase(void *p)
{
	void *q;
	BGBGC_LockGC();
	q=BGBGC_GetObjBase(p);
	BGBGC_UnlockGC();
	return(q);
}


BGBGC_API int gcIndexType(char *name)
{
	int i;
	BGBGC_LockGC();
	i=BGBGC_IndexType(name);
	BGBGC_UnlockGC();
	return(i);
}

BGBGC_API char *gcInternType(char *name)
{
	char *s;
	BGBGC_LockGC();
	s=BGBGC_InternType(name);
	BGBGC_UnlockGC();
	return(s);
}

BGBGC_API char *gcGetType(void *p)
{
	return(gcgettype(p));

#if 0
	char *s;
	BGBGC_LockGC();
	s=BGBGC_GetTypeName(p);
	BGBGC_UnlockGC();
	return(s);
#endif
}

BGBGC_API int gcGetSize(void *p)
{
	int i;
	BGBGC_LockGC();
	i=BGBGC_GetSize(p);
	BGBGC_UnlockGC();
	return(i);
}

BGBGC_API char *gcGetTypeFast(void *p)
{
	char *s;
	BGBGC_LockGC();
	s=BGBGC_GetTypeNameFast(p);
	BGBGC_UnlockGC();
	return(s);
}

BGBGC_API int gcGetSizeFast(void *p)
{
	int i;
	BGBGC_LockGC();
	i=BGBGC_GetSizeFast(p);
	BGBGC_UnlockGC();
	return(i);
}

BGBGC_API int gcGetTypeIdx(void *p)
{
	int i;
	BGBGC_LockGC();
	i=BGBGC_GetTypeIdx(p);
	BGBGC_UnlockGC();
	return(i);
}

BGBGC_API int gcGetTypeIdxFast(void *p)
{
	int i;
	BGBGC_LockGC();
	i=BGBGC_GetTypeIdxFast(p);
	BGBGC_UnlockGC();
	return(i);
}

BGBGC_API int gcGetSourceFileLine(void *p, char **rfn, int *rln)
{
	BGBGC_SourceInfo tsrcinf;
	int i;
	BGBGC_LockGC();
	i=BGBGC_GetObjSourceInfo(p, &tsrcinf);
//	i=BGBGC_GetSize(p);
	if(rfn)*rfn=tsrcinf.lfn;
	if(rln)*rln=tsrcinf.lln;
	BGBGC_UnlockGC();
	return(i);
}

BGBGC_API u32 gcGetAccess(void *p)
{
	BGBGC_SourceInfo tsrcinf;
	u32 t;
	int i;
	BGBGC_LockGC();
	i=BGBGC_GetObjSourceInfo(p, &tsrcinf);
//	i=BGBGC_GetSize(p);
	t=tsrcinf.access;
	BGBGC_UnlockGC();
	return(t);
}

BGBGC_API int gctypep(void *p, char *ty)
{
	char *ty1;

//	BGBGC_LockGC();
//	ty1=BGBGC_GetTypeName(p);
//	BGBGC_UnlockGC();

	ty1=gcgettype(p);

	if(!ty && !ty1)return(1);
	if(!ty || !ty1)return(0);

	if(!strcmp(ty1, ty))return(1);
	return(0);
}

BGBGC_API int gcatomicp(void *p)
{
	int i;
	BGBGC_LockGC();
	i=BGBGC_MarkAtomicP(p);
	BGBGC_UnlockGC();
	return(i);
}


BGBGC_API int gcsetmark(char *ty, int (*fcn)(void *p))
{
	BGBGC_LockGC();
	BGBGC_SetMark(ty, fcn);
	BGBGC_UnlockGC();
	return(0);
}

BGBGC_API int gcsetfinal(char *ty, int (*fcn)(void *p))
{
	BGBGC_LockGC();
	BGBGC_SetDestroy(ty, fcn);
	BGBGC_UnlockGC();
	return(0);
}

BGBGC_API int gcsetpreserve(char *ty, int (*fcn)(void *p))
{
	BGBGC_LockGC();
	BGBGC_SetPreserve(ty, fcn);
	BGBGC_UnlockGC();
	return(0);
}



BGBGC_API void *gctallocu(char *ty, int sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New(ty, sz);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void *gcmalloc(size_t sz)
{
	void *p;
	BGBGC_LockGC();
	p=BGBGC_New("_raw_t", sz);
	BGBGC_SetLockObj(p, 1);
	BGBGC_UnlockGC();
	return(p);
}

BGBGC_API void gclock(void *p)
{
	BGBGC_SetLockObj(p, 1);
}

BGBGC_API void gcunlock(void *p)
{
	BGBGC_SetLockObj(p, 0);
}

BGBGC_API void gcinc(void *p)
{
	if(!used_refcount)return;
	BGBGC_IncPtr(p);
}

BGBGC_API void gcdec(void *p)
{
	if(!used_refcount)return;
	BGBGC_DecPtr(p);
}

BGBGC_API void gcsafedec(void *p)
{
	if(!used_refcount)return;
	BGBGC_SafeDecPtr(p);
}

BGBGC_API void gcset(void **r, void *p)
{
	void *q; q=*r;
	BGBGC_CollectAssign(r, p);
	if(used_refcount)	//only bother if ref-counting was used
		{ BGBGC_IncPtr(p); BGBGC_DecPtr(q); }
}

BGBGC_API void gcset64(s64 *r, s64 p)
{
//	void *q;
	
	*r=p;
//	BGBGC_CollectAssign((void **)r, ()p);
//	if(used_refcount)	//only bother if ref-counting was used
//		{ q=*(void **)r; BGBGC_IncPtr(p); BGBGC_DecPtr(q); }
}

BGBGC_API void gcsetbyte(void *p, int v)
{
	BGBGC_TryMarkSafe(p);
	*(byte *)p=v;
}

BGBGC_API void gcsetword(void *p, int v)
{
	BGBGC_TryMarkSafe(p);
	*(short *)p=v;
}

BGBGC_API void gcsetint(void *p, int v)
{
	BGBGC_TryMarkSafe(p);
	*(int *)p=v;
}

BGBGC_API void gcsetlong(void *p, s64 v)
{
	BGBGC_TryMarkSafe(p);
	*(s64 *)p=v;
}

BGBGC_API void gcsetfloat(void *p, float v)
{
	BGBGC_TryMarkSafe(p);
	*(float *)p=v;
}

BGBGC_API void gcsetdouble(void *p, double v)
{
	BGBGC_TryMarkSafe(p);
	*(double *)p=v;
}

/** (Write Barrier) Write byte value into pointed-to address. */
BGBGC_API void gcSetByte(void *p, int v)
{
	BGBGC_TryMarkSafe(p);
	*(byte *)p=v;
}

/** (Write Barrier) Write word value into pointed-to address. */
BGBGC_API void gcSetWord(void *p, int v)
{
	BGBGC_TryMarkSafe(p);
	*(short *)p=v;
}

/** (Write Barrier) Write int/dword value into pointed-to address. */
BGBGC_API void gcSetInt(void *p, int v)
{
	BGBGC_TryMarkSafe(p);
	*(int *)p=v;
}

/** (Write Barrier) Write long/qword value into pointed-to address. */
BGBGC_API void gcSetLong(void *p, s64 v)
{
	BGBGC_TryMarkSafe(p);
	*(s64 *)p=v;
}

/** (Write Barrier) Write float value into pointed-to address. */
BGBGC_API void gcSetFloat(void *p, float v)
{
	BGBGC_TryMarkSafe(p);
	*(float *)p=v;
}

/** (Write Barrier) Write double value into pointed-to address. */
BGBGC_API void gcSetDouble(void *p, double v)
{
	BGBGC_TryMarkSafe(p);
	*(double *)p=v;
}

/** (Write Barrier) Copy memory contents */
BGBGC_API void gcMemCpy(void *p, void *v, int sz)
{
	BGBGC_TryMarkSafe(p);
	memcpy(p, v, sz);
}

/** Map executable object to a writable buffer */
BGBGC_API void *gcBeginMapExec(void *obj)
{
	BGBGC_TryMarkSafe(obj);
	return(obj);
}

/** Write contents to exeutable object and unmap, If Applicable */
BGBGC_API void gcEndMapExec(void *p, void *v)
{
}

BGBGC_API void gcclear(void **r)
{
	void *q; q=*r;
	BGBGC_CollectAssign(r, NULL);
	BGBGC_DecPtr(q);
}

BGBGC_API void gcDumpHeapStats(int fl)
{
	BGBGC_Stat_StatHeap(fl);
}


BGBGC_API char *gcstrdup(char *str)
{
	char *t;
	t=gctalloc("_string_t", strlen(str)+1);
	strcpy(t, str);
	return(t);
}

BGBGC_API void *gcralloc(int sz)
{
	static char *buf=NULL;
	static int pos=0;
	char *s;

	thLockFastMutex(bgbgc_ralloc_mutex);

	if(!buf)buf=malloc(1<<20);
	if((pos+sz)>=(1<<20))pos=0;

	s=buf+pos;
	pos=(pos+sz+7)&(~7);

	thUnlockFastMutex(bgbgc_ralloc_mutex);
	return(s);
}

BGBGC_API char *gcrstrdup(char *str)
{
	char *t;
	t=gcralloc(strlen(str)+1);
	strcpy(t, str);
	return(t);
}

BGBGC_API char *gcrsprint(char *str, ...)
{
	char buf[4096];
	va_list lst;
	char *t;

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	t=gcrstrdup(buf);
	return(t);
}

BGBGC_API char *gcrsprints(char *buf, char *str, ...)
{
//	char buf[4096];
	va_list lst;
	char *t;

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	t=gcrstrdup(buf);
	return(t);
}

BGBGC_API char **gcrsplit(char *s)
{
	char tb[256];
	char **a, *t;
	int i, j;

	a=gcralloc(256*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
//		t=gcralloc(256);
//		a[i++]=t;

		t=tb;

		if(*s=='"')
		{
			s++;

			while(*s && (*s!='"'))
			{
				if(*s=='\\')
				{
					*t++=*s++;

					if(*s=='x')
					{
						*t++=*s++;
						*t++=*s++; *t++=*s++;
						continue;
					}
					if(*s=='u')
					{
						*t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						continue;
					}
					if(*s=='U')
					{
						*t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						continue;
					}

					*t++=*s++;
					continue;
				}

				*t++=*s++;
			}

			if(*s=='"')s++;

			*t++=0;
			*t++='\"';
			*t++=0;

			j=t-tb;
			t=gcralloc(j);
			memcpy(t, tb, j);
			a[i++]=t;
			continue;
		}

		while(*s && (*s>' ') && (*s!='\"'))*t++=*s++;
		*t++=0;
		*t++=0;
		*t++=0;

		j=t-tb;
		t=gcralloc(j);
		memcpy(t, tb, j);
		a[i++]=t;
	}
	a[i]=NULL;

	return(a);
}

BGBGC_API char *gcrsgets(char *buf, int lim, char *str)
{
	char *s, *t, *te;

	s=str; t=buf; te=t+lim-1;
	while(*s && (*s!='\n') && (*s!='\r') && (t<te))*t++=*s++;
	*t++=0;

	if(*s=='\r')s++;
	if(*s=='\n')s++;
	return(s);
}

BGBGC_API char *gcrgets(char **str)
{
	char *s, *s1, *b, *t;

	s=*str; s1=s;
	while(*s1 && (*s1!='\n') && (*s1!='\r'))s1++;

	b=gcralloc((s1-s)+1);
	t=b;
	while(s<s1)*t++=*s++;
	*t++=0;

	if(*s=='\r')s++;
	if(*s=='\n')s++;
	*str=s;
	
	return(b);
}

BGBGC_API char *gcrgettoken(char **str)
{
	char *s, *s0, *s1, *b, *t;

	s=*str; s0=s;
	while(*s0 && (*s0<=' '))s0++;
	if(*s0=='\"')
	{
		s1=s0+1;
		while(*s1 && (*s1!='\"'))
		{
			if(*s1=='\\')
			{
				s1++;

				if(*s1=='x')
				{
					s1++; s1++; s1++;
					continue;
				}
				if(*s=='u')
				{
					s1++; s1++; s1++; s1++;
					continue;
				}
				if(*s=='U')
				{
					s1++; s1++; s1++; s1++;
					s1++; s1++; s1++; s1++; s1++;
					continue;
				}
				s1++;
				continue;
			}
			s1++;
		}
	}else
	{
		s1=s0;
		while(*s1 && (*s1>' '))s1++;
	}

	b=gcralloc((s1-s0)+3);
	s=s0; t=b;
	while(s<s1)*t++=*s++;
	*t++=0;
	if(*s=='\"')
		{ *t++=*s++; }
	*str=s;
	return(b);
}

BGBGC_API char *gcrlltoa(s64 v)
{
	char *t;
	t=gcralloc(64);
#ifdef _MSC_VER
	sprintf(t, "%I64d", v);
#else
	sprintf(t, "%lld", v);
#endif
	return(t);
}

BGBGC_API char *gcrdtoa(double v)
{
	char *t;
	if(((s64)v)==v)
		return(gcrlltoa((s64)v));
	t=gcralloc(64);
	sprintf(t, "%f", v);
	return(t);
}

BGBGC_API char *gcrdtoa1p(double v)
{
	s64 i;
	i=(s64)((v>=0)?(v+0.5):(v-0.5));
	if(fabs(v-i)<0.1)return(gcrlltoa(i));
	return(gcrsprint("%.1f", v));
}

BGBGC_API char *gcrdtoa2p(double v)
{
	s64 i;
	i=(s64)((v>=0)?(v+0.5):(v-0.5));
	if(fabs(v-i)<0.01)return(gcrlltoa(i));
	return(gcrsprint("%.2f", v));
}

BGBGC_API char *gcrdtoa3p(double v)
{
	s64 i;
	i=(s64)((v>=0)?(v+0.5):(v-0.5));
	if(fabs(v-i)<0.001)return(gcrlltoa(i));
	return(gcrsprint("%.3f", v));
}

BGBGC_API char *gcrdtoa4p(double v)
{
	s64 i;
	i=(s64)((v>=0)?(v+0.5):(v-0.5));
	if(fabs(v-i)<0.0001)return(gcrlltoa(i));
	return(gcrsprint("%.4f", v));
}

BGBGC_API s64 gcratoi(char *str)
{
	s64 l;
	char *s, *t;
	int i, sg;

	t=str;
	if((t[0]=='0') && (t[1]=='x'))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=16;
			if((*t>='0') && (*t<='9'))l+=*t-'0';
			if((*t>='A') && (*t<='F'))l+=*t-'A'+10;
			if((*t>='a') && (*t<='f'))l+=*t-'a'+10;
			t++;
		}

		return(l);
	}

	if(t[0]=='0')
	{
		l=0; t++;
		while(*t)
		{
			l*=8;
			if((*t>='0') && (*t<='7'))l+=*t-'0';
				else break;
			t++;
		}

		return(l);
	}

	t=str; l=0; sg=0;
	if(*t=='-') { sg=1; t++; }
	while(*t)
	{
		l*=10;
		if((*t>='0') && (*t<='9'))l+=*t-'0';
			else break;
		t++;
	}
	if(sg)l=-l;

	return(l);
}


#ifdef NATIVECLIENT
BGBGC_API void _mcount()
{
}
#endif