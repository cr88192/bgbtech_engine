#ifdef _MSC_VER
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <math.h>

#ifndef BGBGC_H
#define BGBGC_H

#include <bgbgc_conf.h>

#ifndef UNDEFINED
#define UNDEFINED	((void *)(-1))
#endif

#define BGBGC_NULLBASE	((void *)0)
#define BGBGC_NULLEND	((void *)4095)

// #define BGBDY_NULL	((void *)0)
// #define BGBDY_TRUE	((void *)2)
// #define BGBDY_FALSE	((void *)3)
// #define BGBDY_AUXNULL	((void *)4)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef PDLIB_INT_BITS_T
#define PDLIB_INT_BITS_T
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef signed long long s64;
#endif

// typedef float _Complex fcomplex;
// typedef double _Complex dcomplex;

typedef struct { u32 a,b,c,d; } iv128_t;
typedef struct { u32 a,b,c,d,e,f,g,h; } iv256_t;

// typedef struct { u32 a,b,c,d; } f128_t;
typedef iv128_t fv128_t;

typedef struct gcp_ *gcp;

// write barrier:
#define gcSet(pp, p)	gcset((void **)(&(pp)), p);
#define gcSet64(pp, p)	gcset64((s64 *)(&(pp)), *(s64 *)(&(p)));

#define gcSetsb(p, v)	gcsetbyte(&(p), v);
#define gcSetub(p, v)	gcsetbyte(&(p), v);
#define gcSetsw(p, v)	gcsetword(&(p), v);
#define gcSetuw(p, v)	gcsetword(&(p), v);
#define gcSetb(p, v)	gcsetbyte(&(p), v);
#define gcSetw(p, v)	gcsetword(&(p), v);
#define gcSeti(p, v)	gcsetint(&(p), v);
#define gcSetl(p, v)	gcsetlong(&(p), v);
#define gcSetf(p, v)	gcsetfloat(&(p), v);
#define gcSetd(p, v)	gcsetdouble(&(p), v);

#define gcSetCopy(p, v, sz)	gcMemCpy(p, v, sz);

#ifdef WIN32
#define printf gc_printf
#endif

#ifdef linux
#define printf gc_printf
#endif

#ifdef NATIVECLIENT
#define printf gc_printf
#endif

#define dbgprintf gc_dbgprintf

//#ifdef linux
#if defined(linux) || defined(NATIVECLIENT)
#ifndef stricmp
#define stricmp strcasecmp
#endif
#ifndef strnicmp
#define strnicmp strncasecmp
#endif
#endif

#define BDBG_PRINT_DEFAULT	0
#define BDBG_PRINT_DEBUG	1

#include <bgbgc_mm.h>
#include <bgbgc_objtype.h>
#include <bgbgc_thread.h>

//#include <bgbgc_dyoo.h>
//#include <bgbgc_dyxml.h>

//#include <bgbgc_vfile2.h>
//#include <bgbgc_vfzip.h>
//#include <bgbgc_wref.h>

//#include <bgbgal.h>

#include <bgbgc_auto.h>

//remap functions
#ifndef BGBGC_DLL
#define gcalloc(sz)			gcallocLLn(sz, __FILE__, __LINE__)
#define gctalloc(ty, sz)	gctallocLLn(ty, sz, __FILE__, __LINE__)
#define gcatomic(sz)		gcatomicLLn(sz, __FILE__, __LINE__)
#define gctatomic(ty, sz)	gctatomicLLn(ty, sz, __FILE__, __LINE__)
#endif


#ifdef __cplusplus
}
#endif

#endif
