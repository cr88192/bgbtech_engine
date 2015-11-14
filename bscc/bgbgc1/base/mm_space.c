/*
Purpose:
Support low-level memory and address space allocation.

Address space allocation will differ from memory allocation in that address
space will be assumed to be non-accessable (and thus allocated outside of the
area usable for memory).

 */

#include <bgbgc.h>

#ifdef linux
#include <sys/mman.h>
#endif

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#endif

s64 bgbgc_space_pos;
s64 bgbgc_space_min;
s64 bgbgc_space_lim;

char *bgbgc_space_type[256];
void *bgbgc_space_base[256];
void *bgbgc_space_end[256];
int bgbgc_space_bits[256];
int bgbgc_space_cnt=0;


void BGBGC_InitSpace()
{
	static int init=0;
	if(init)return;
	init=1;

#ifdef X86_64
	bgbgc_space_min=(s64)0x7100000000000000LL;
	bgbgc_space_lim=(s64)0x7200000000000000LL;
	bgbgc_space_pos=bgbgc_space_min;
#endif

#ifdef __i386__
#if defined(linux) || defined(WIN32)
	bgbgc_space_min=(s64)0x0C0000000LL;
//	bgbgc_space_lim=(s64)0x100000000LL;
	bgbgc_space_lim=(s64)0x0FFFFFFFFLL;
	bgbgc_space_pos=bgbgc_space_min;
#endif
#endif

#ifdef __arm__
#if defined(linux)
	/* unsafe and ugly, but AFAICT Linux on ARM seems to use the same
	   default address-space as x86 */
	bgbgc_space_min=(s64)0x0C0000000LL;
//	bgbgc_space_lim=(s64)0x100000000LL;
	bgbgc_space_lim=(s64)0x0FFFFFFFFLL;
	bgbgc_space_pos=bgbgc_space_min;
#endif
#endif
}

int BGBGC_SpaceAllocSafeP(int bits)
{
	BGBGC_InitSpace();

#ifdef X86_64
	if(bits>48)return(0);

	if((bgbgc_space_pos+(((s64)1)<<bits))<=bgbgc_space_lim)
		return(1);
	return(0);
#endif

	if(bits>30)return(0);

	if((bgbgc_space_pos+(((s64)1)<<bits))<=bgbgc_space_lim)
		return(1);

	return(0);
}

void *BGBGC_SpaceAlloc(int bits)
{
	void *p;

	BGBGC_InitSpace();

	if(bits>48)return(NULL);
	if((bgbgc_space_pos+(((s64)1)<<bits))>bgbgc_space_lim)
	{
#ifdef X86
		if(bits>28)return(NULL);
#endif
#ifdef X86_64
		if(bits>32)return(NULL);
#endif

#ifdef linux
		p=mmap(NULL, ((s64)1)<<bits, PROT_NONE,
			MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		if(p)return(p);
#endif

#ifdef WIN32
		p=VirtualAlloc(NULL, ((s64)1)<<bits,
			MEM_RESERVE|MEM_TOP_DOWN, PAGE_NOACCESS);
		if(p)return(p);
#endif
		return(NULL);
	}

	p=(void *)bgbgc_space_pos;
	bgbgc_space_pos+=((s64)1)<<bits;
	return(p);
}

int BGBGC_CheckSpaceP(void *ptr)
{
#if 1
	if(ptr<((void *)bgbgc_space_min))return(0);
	if(ptr>=((void *)bgbgc_space_lim))return(0);
	return(1);
#endif
}

int BGBGC_IndexSpace(void *ptr)
{
	byte *pa, *pb, *pc;
	int i;

#if 1
	if(ptr<((void *)bgbgc_space_min))return(-1);
	if(ptr>=((void *)bgbgc_space_lim))return(-1);
#endif

	pa=ptr;
	for(i=0; i<bgbgc_space_cnt; i++)
	{
		pb=(byte *)bgbgc_space_base[i];
		if(pa<pb)continue;

		pc=pb+(((nlint)1)<<bgbgc_space_bits[i]);
		if(pa>=pc)continue;

		return(i);
	}

	return(-1);
}

BGBGC_API int gcllSpaceAllocSafeP(int bits)
	{ return(BGBGC_SpaceAllocSafeP(bits)); }

BGBGC_API void *gcllSpaceNew(char *type, int bits)
{
	void *p;
	int i;

	p=gcllSpaceLookup(type);
	if(p)return(p);

	p=BGBGC_SpaceAlloc(bits);
	if(!p)return(NULL);

	i=bgbgc_space_cnt++;
	if(i>=256) { *(int *)-1=-1; }
	
//	bgbgc_space_type[i]=gcstrdup(type);
	bgbgc_space_type[i]=gcInternType(type);
	bgbgc_space_base[i]=p;
	bgbgc_space_end[i]=((byte *)p)+(((nlint)1)<<bits);
	bgbgc_space_bits[i]=bits;

	printf("SpaceNew(%d): %s %p..%p\n", i, type,
		bgbgc_space_base[i], bgbgc_space_end[i]);

	return(p);
}

BGBGC_API void *gcllSpaceLookup(char *type)
{
	int i;

	for(i=0; i<bgbgc_space_cnt; i++)
	{
		if(!strcmp(bgbgc_space_type[i], type))
			return(bgbgc_space_base[i]);
	}
	return(NULL);
}

BGBGC_API char *gcllSpaceType(void *ptr)
{
#if 0
	int i;

	i=BGBGC_IndexSpace(ptr);
	if(i<0)return(NULL);
	return(bgbgc_space_type[i]);
#endif

#if 1
	int i;

	if(ptr<((void *)bgbgc_space_min))return(NULL);
	if(ptr>=((void *)bgbgc_space_lim))return(NULL);
	for(i=0; i<bgbgc_space_cnt; i++)
	{
		if(ptr<bgbgc_space_base[i])continue;
		if(ptr>=bgbgc_space_end[i])continue;
		return(bgbgc_space_type[i]);
	}
	return(NULL);
#endif
}

BGBGC_API int gcllSpaceBits(void *ptr)
{
	int i;

	i=BGBGC_IndexSpace(ptr);
	if(i<0)return(-1);
	return(bgbgc_space_bits[i]);
}

BGBGC_API void *gcllSpaceBase(void *ptr)
{
	int i;

	i=BGBGC_IndexSpace(ptr);
	if(i<0)return(NULL);
	return(bgbgc_space_base[i]);
}

BGBGC_API void *gcllSpaceEnd(void *ptr)
{
	int i;

	i=BGBGC_IndexSpace(ptr);
	if(i<0)return(NULL);
	return(bgbgc_space_end[i]);
}

BGBGC_API s64 gcllSpaceOffset(void *ptr)
{
	byte *pa, *pb, *pc;
	int i;

	pa=ptr;
	for(i=0; i<bgbgc_space_cnt; i++)
	{
		pb=(byte *)bgbgc_space_base[i];
		if(pa<pb)continue;

		pc=pb+(((s64)1)<<bgbgc_space_bits[i]);
		if(pa>=pc)continue;

		return(pa-pb);
	}

	return(0);
}

BGBGC_API void *gcllSpaceWrap(char *type, s64 offs)
{
	byte *p;	
	p=(byte *)gcllSpaceLookup(type);
	return((void *)(p+offs));
}

BGBGC_API int gcllSpaceP(void *ptr)
{
	int i;

	i=BGBGC_IndexSpace(ptr);
	if(i<0)return(0);
	return(1);
}

BGBGC_API void *gcllSpaceAbsoluteBase(void *ptr)
{
	return((byte *)bgbgc_space_pos);
}

BGBGC_API s64 gcllSpaceAbsoluteOffset(void *ptr)
{
	byte *pa, *pb, *pc;
	int i;

	pa=ptr;
	pb=(byte *)bgbgc_space_pos;
	return(pa-pb);
}

BGBGC_API void *gcllSpaceWrapAbsolute(s64 offs)
{
	byte *p;	
	p=(byte *)bgbgc_space_pos;
	return((void *)(p+offs));
}
