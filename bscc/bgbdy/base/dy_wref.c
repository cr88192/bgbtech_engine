#include <bgbgc.h>
#include <bgbdy.h>

static dyWRefCtx *dywref_root=NULL;
static dyWRefCtx *dywref_hash[4096];
static iv128_t wref_null={0,0,0,0};
static u64 wref_localseg=0;
static u32 wref_local_hi=0;
static u32 wref_local_lo=0;

void WRef_Init()
{
	static int init=0;
	if(init)return;
	init=1;

	wref_localseg=WRef_GenSeg();
	wref_local_hi=(u32)(wref_localseg>>32);
	wref_local_lo=(u32)wref_localseg;
}

u64 WRef_GenSeg()
{
	u64 seg;
	seg=bgbrng_genvalue64();
	seg=seg&0x0000FCFFFFFFFFFFLL;
	seg=seg|0x4155020000000000LL;
	return(seg);
}

dyWRefCtx *WRef_LookupSegLin(u64 seg)
{
	dyWRefCtx *cur;

	cur=dywref_root;
	while(cur)
	{
		cur=cur->next;
		if(cur->seg==seg)
			return(cur);
	}
	return(NULL);
}

dyWRefCtx *WRef_LookupSegHash(u64 seg)
{
	dyWRefCtx *cur;
	u64 cseg;
	int i;

	cseg=seg;
	for(i=0; i<16; i++)
	{
		cseg*=65521;
		cur=dywref_hash[(cseg>>48)&0xFFF];
		if(!cur)break;
		if(cur->seg==seg)
			return(cur);
	}
	return(NULL);
}

dyWRefCtx *WRef_LookupSeg(u64 seg)
{
	dyWRefCtx *cur;
	u64 cseg;
	int i, h;

	cur=WRef_LookupSegHash(seg);
	if(cur)return(cur);

	cur=WRef_LookupSegLin(seg);
	if(!cur)return(NULL);

	cseg=seg;
	for(i=0; i<16; i++)
	{
		cseg*=65521;
		h=(cseg>>48)&0xFFF;
		if(!dywref_hash[h])
		{
			dywref_hash[h]=cur;
			break;
		}
	}
	return(cur);
}

void WRef_SplitRef(iv128_t wp, dyWRefCtx **cur, u64 *seg, u64 *offs)
{
	*offs=wp.a | (((u64)wp.b)<<32);
	*seg=wp.c | (((u64)wp.d)<<32);
	*cur=WRef_LookupSeg(*seg);
}

iv128_t WRef_MergeRef(dyWRefCtx *cur, u64 offs)
{
	iv128_t wp;

	wp.a=offs&0xFFFFFFFF;
	wp.b=(offs>>32)&0xFFFFFFFF;
	wp.c=(cur->seg)&0xFFFFFFFF;
	wp.d=((cur->seg)>>32)&0xFFFFFFFF;
	return(wp);
}

void *WRef_CastPtr(iv128_t wp)
{
#ifdef X86_64
		return((void *)(wp.a|(((u64)wp.b)<<32)));
#else
#if defined(X86) || defined(ARM)
		return((void *)wp.a);
#endif
#endif
}

iv128_t WRef_LocalPtr(void *p)
{
	iv128_t wp;

#if defined(X86) || defined(ARM)
	wp.a=(u32)p; wp.b=0;
	wp.c=wref_local_lo; wp.d=wref_local_hi;
	return(wp);
#endif

#ifdef X86_64
	wp.a=(u32)(((u64)p)&0xFFFFFFFF);
	wp.b=(u32)(((u64)p)>>32);
	wp.c=wref_local_lo; wp.d=wref_local_hi;
	return(wp);
#endif
}

int WRef_LocalPtrP(iv128_t wp)
{
	if((wp.c==wref_local_lo) && (wp.d==wref_local_hi))
		return(1);
	return(0);
}

int WRef_ReadMem(void *buf, int sz, iv128_t wp, int disp)
{
	dyWRefCtx *cur;
	u64 seg, offs;

	WRef_Init();

	if(WRef_LocalPtrP(wp))
	{
		memcpy(buf, WRef_CastPtr(wp), sz);
		return(0);
	}

	WRef_SplitRef(wp, &cur, &seg, &offs);
	if(cur && cur->read_mem)
		return(cur->read_mem(buf, sz, cur, offs+disp));
	return(-1);
}

int WRef_WriteMem(void *buf, int sz, iv128_t wp, int disp)
{
	dyWRefCtx *cur;
	u64 seg, offs;

	WRef_Init();

	if(WRef_LocalPtrP(wp))
	{
		memcpy(WRef_CastPtr(wp), buf, sz);
		return(0);
	}

	WRef_SplitRef(wp, &cur, &seg, &offs);
	if(cur && cur->write_mem)
		return(cur->write_mem(buf, sz, cur, offs+disp));
	return(-1);
}

int WRef_CopyMem(iv128_t dst, iv128_t src, u64 sz)
{
	dyWRefCtx *dcur, *scur;
	u64 dseg, doffs, sseg, soffs;
	void *buf, *buf1;

	WRef_Init();

	if(WRef_LocalPtrP(dst))
	{
		if(WRef_LocalPtrP(src))
		{
			memcpy(WRef_CastPtr(dst), WRef_CastPtr(src), sz);
			return(0);
		}
		WRef_ReadMem(WRef_CastPtr(dst), sz, src, 0);
		return(0);
	}
	if(WRef_LocalPtrP(src))
	{
		WRef_WriteMem(WRef_CastPtr(src), sz, dst, 0);
		return(0);
	}

	WRef_SplitRef(dst, &dcur, &dseg, &doffs);
	WRef_SplitRef(src, &scur, &sseg, &soffs);
	if(!dcur || !scur)return(-1);

	if(dcur && (scur==dcur) && dcur->copy_mem)
		return(dcur->copy_mem(dcur, doffs, soffs, sz));

	if(!dcur->write_mem || !scur->read_mem)return(-1);

	if(sz<=(1<<20))
	{
		buf=malloc(sz);
		scur->read_mem(buf, sz, scur, soffs);
		dcur->write_mem(buf, sz, dcur, doffs);
		free(buf);
	}

	buf=malloc(1<<20);

	while(sz>(1<<20))
	{
		scur->read_mem(buf, 1<<20, scur, soffs);
		dcur->write_mem(buf, 1<<20, dcur, doffs);
		soffs+=1<<20; doffs+=1<<20;
	}

	scur->read_mem(buf, sz, scur, soffs);
	dcur->write_mem(buf, sz, dcur, doffs);

	free(buf);

	return(-1);
}

iv128_t WRef_AllocMem(u64 seg, u64 sz)
{
	dyWRefCtx *cur;
	u64 offs;

	WRef_Init();

	cur=WRef_LookupSeg(seg);
	if(!cur)return(wref_null);

	if(cur->alloc_mem)
	{
		offs=cur->alloc_mem(cur, sz);
		if(!offs)return(wref_null);
		return(WRef_MergeRef(cur, offs));
	}
	return(wref_null);
}

int WRef_FreeMem(iv128_t wp)
{
	dyWRefCtx *cur;
	u64 seg, offs;

	WRef_Init();

	if(WRef_LocalPtrP(wp))
		{ gcfree(WRef_CastPtr(wp)); return(0); }

	WRef_SplitRef(wp, &cur, &seg, &offs);
	if(cur && cur->free_mem)return(cur->free_mem(cur, offs));
	return(-1);
}

iv128_t WRef_ReallocMem(iv128_t wp, u64 sz)
{
	iv128_t wp1;
	dyWRefCtx *cur;
	u64 seg, offs, offs1;

	WRef_Init();

	if(WRef_LocalPtrP(wp))
		return(WRef_LocalPtr(gcrealloc(WRef_CastPtr(wp), sz)));

	WRef_SplitRef(wp, &cur, &seg, &offs);
	if(!cur)return(wref_null);

	if(cur->realloc_mem)
	{
		offs1=cur->realloc_mem(cur, offs, sz);
		if(!offs1)return(wref_null);
		return(WRef_MergeRef(cur, offs1));
	}

	offs1=cur->alloc_mem(cur, sz);
	if(!offs1)return(wref_null);
	wp1=WRef_MergeRef(cur, offs1);
	WRef_CopyMem(wp1, wp, sz);
	cur->free_mem(cur, offs);
	return(wp1);
}

char *WRef_GetType(iv128_t wp)
{
	dyWRefCtx *cur;
	u64 seg, offs;

	WRef_Init();

	if(WRef_LocalPtrP(wp))
		return(dygettype(WRef_CastPtr(wp)));

	WRef_SplitRef(wp, &cur, &seg, &offs);
	if(cur && cur->gettype_mem)return(cur->gettype_mem(cur, offs));
	return(NULL);
}


BGBDY_API dyt dwWrapWRef(iv128_t ptr)
{
	dyt p;

	if(!ptr.a && !ptr.b && !ptr.c && !ptr.d)
		return(NULL);

	if(WRef_LocalPtrP(ptr))
		return(WRef_CastPtr(ptr));

	p=gctalloc("_wref_t", sizeof(iv128_t));
	*(iv128_t *)p=ptr;
	return(p);
}

BGBDY_API iv128_t dwUnwrapWRef(dyt p)
{
	iv128_t t;

	if(!p)return(wref_null);
	if(!dwrefp(p))
	{
		t=WRef_LocalPtr(p);
		return(t);
	}

	t=*(iv128_t *)p;
	return(t);
}

BGBDY_API u64 dwGetWRefSeg(dyt p)
{
	iv128_t t;

	t=dwUnwrapWRef(p);
	return((((u64)t.d)<<32)|t.c);
}

BGBDY_API int dwrefp(dyt p)
{
	return(dytypep(p, "_wref_t"));
}

BGBDY_API char *dwgettype(dyt p)
{
	if(!dwrefp(p))return(NULL);
	return(WRef_GetType(*(iv128_t *)p));
}

BGBDY_API int dwtypep(dyt p, char *ty)
{
	char *ty1;

	if(!dwrefp(p))return(0);

	ty1=WRef_GetType(*(iv128_t *)p);
	if(!ty && !ty1)return(1);
	if(!ty || !ty1)return(0);

	if(!strcmp(ty1, ty))return(1);
	return(0);
}

BGBDY_API dyt dwtalloc(u64 seg, char *ty, u64 sz)
{
	iv128_t t;
	t=WRef_AllocMem(seg, sz);
	return(dwWrapWRef(t));
}

BGBDY_API int dwfree(dyt ptr)
{
	WRef_FreeMem(dwUnwrapWRef(ptr));
	return(0);
}

BGBDY_API int dwmemcpy(dyt dst, dyt src, int sz)
{
	if(!dwrefp(dst) && !dwrefp(src))
	{
		memcpy(dst, src, sz);
		return(0);
	}

	if(!dwrefp(dst))
	{
		WRef_ReadMem(dst, sz, dwUnwrapWRef(src), 0);
		return(0);
	}

	if(!dwrefp(src))
	{
		WRef_WriteMem(src, sz, dwUnwrapWRef(dst), 0);
		return(0);
	}

	WRef_CopyMem(dwUnwrapWRef(dst), dwUnwrapWRef(src), sz);
	return(0);
}

BGBDY_API int dwreadmem(void *buf, int sz, dyt ptr, int disp)
{
	WRef_ReadMem(buf, sz, dwUnwrapWRef(ptr), disp);
	return(0);
}

BGBDY_API int dwwritemem(void *buf, int sz, dyt ptr, int disp)
{
	WRef_WriteMem(buf, sz, dwUnwrapWRef(ptr), disp);
	return(0);
}

BGBDY_API int dwgetc(dyt ptr, int disp)
	{ char v; dwreadmem(&v, sizeof(char), ptr, disp); return(v); }
BGBDY_API int dwgetb(dyt ptr, int disp)
	{ byte v; dwreadmem(&v, sizeof(byte), ptr, disp); return(v); }
BGBDY_API int dwgetw(dyt ptr, int disp)
	{ u16 v; dwreadmem(&v, sizeof(u16), ptr, disp); return(v); }
BGBDY_API int dwgeth(dyt ptr, int disp)
	{ s16 v; dwreadmem(&v, sizeof(s16), ptr, disp); return(v); }
BGBDY_API u32 dwgetdw(dyt ptr, int disp)
	{ u32 v; dwreadmem(&v, sizeof(u32), ptr, disp); return(v); }
BGBDY_API s32 dwgeti(dyt ptr, int disp)
	{ s32 v; dwreadmem(&v, sizeof(s32), ptr, disp); return(v); }
BGBDY_API u64 dwgetq(dyt ptr, int disp)
	{ u64 v; dwreadmem(&v, sizeof(u64), ptr, disp); return(v); }
BGBDY_API s64 dwgetl(dyt ptr, int disp)
	{ s64 v; dwreadmem(&v, sizeof(s64), ptr, disp); return(v); }

BGBDY_API float dwgetf(dyt ptr, int disp)
	{ float v; dwreadmem(&v, sizeof(float), ptr, disp); return(v); }
BGBDY_API double dwgetd(dyt ptr, int disp)
	{ double v; dwreadmem(&v, sizeof(double), ptr, disp); return(v); }
BGBDY_API iv128_t dwgetdq(dyt ptr, int disp)
	{ iv128_t v; dwreadmem(&v, sizeof(iv128_t), ptr, disp); return(v); }

BGBDY_API int dwputc(dyt ptr, int disp, char v)
	{ return(dwwritemem(&v, sizeof(char), ptr, disp)); }
BGBDY_API int dwputb(dyt ptr, int disp, byte v)
	{ return(dwwritemem(&v, sizeof(byte), ptr, disp)); }
BGBDY_API int dwputw(dyt ptr, int disp, u16 v)
	{ return(dwwritemem(&v, sizeof(u16), ptr, disp)); }
BGBDY_API int dwputh(dyt ptr, int disp, s16 v)
	{ return(dwwritemem(&v, sizeof(s16), ptr, disp)); }
BGBDY_API int dwputdw(dyt ptr, int disp, u32 v)
	{ return(dwwritemem(&v, sizeof(u32), ptr, disp)); }
BGBDY_API int dwputi(dyt ptr, int disp, s32 v)
	{ return(dwwritemem(&v, sizeof(s32), ptr, disp)); }
BGBDY_API int dwputq(dyt ptr, int disp, u64 v)
	{ return(dwwritemem(&v, sizeof(u64), ptr, disp)); }
BGBDY_API int dwputl(dyt ptr, int disp, s64 v)
	{ return(dwwritemem(&v, sizeof(s64), ptr, disp)); }

BGBDY_API int dwputf(dyt ptr, int disp, float v)
	{ return(dwwritemem(&v, sizeof(float), ptr, disp)); }
BGBDY_API int dwputd(dyt ptr, int disp, double v)
	{ return(dwwritemem(&v, sizeof(double), ptr, disp)); }
BGBDY_API int dwputdq(dyt ptr, int disp, iv128_t v)
	{ return(dwwritemem(&v, sizeof(iv128_t), ptr, disp)); }

BGBDY_API dyt dwgetp(dyt ptr, int disp)
{
	iv128_t v;
	dwreadmem(&v, sizeof(iv128_t), ptr, disp);
	return(dwWrapWRef(v));
}

BGBDY_API int dwputp(dyt ptr, int disp, dyt p)
{
	iv128_t v;
	v=dwUnwrapWRef(p);
	return(dwwritemem(&v, sizeof(iv128_t), ptr, disp));
}

