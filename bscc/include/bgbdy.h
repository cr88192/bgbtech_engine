#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
// #include <stdint.h>

#include <time.h>

#include <math.h>
#include <bgbgc.h>

#ifndef BGBDY_H
#define BGBDY_H


#ifdef WIN32
#ifdef BGBDY_DLL
#define BGBDY_API __declspec(dllexport)
#else
#define BGBDY_API __declspec(dllimport)
#endif
#else
#define BGBDY_API
#endif

#ifdef _MSC_VER
#define BGBDY_THREAD __declspec(thread)
#else
#ifdef __GNUC__
#define BGBDY_THREAD __thread
#endif
#endif

#ifndef dytAlign
#if defined(_MSC_VER) || defined(_BGBMETA)
#define dytAlign(x) __declspec(align(x))
#else
#ifdef __GNUC__
// #define dytAlign(x) __attribute__((align(x)))
#define dytAlign(x)
#endif
#endif
#endif

#if defined(ARM) && defined(BGBDY_THREAD)
#undef BGBDY_THREAD
#endif

#ifndef BGBDY_THREAD
#define BGBDY_THREAD
#endif

#ifdef ARM
#define BGBDY_USEGENAPPLY
#endif

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#ifndef M_TAU
#define M_TAU 6.283185307179586476925286766559
#endif

#ifndef UNDEFINED
#define UNDEFINED		((void *)(-1))
#endif

#define BGBDY_NULL		((void *)0)
#define BGBDY_TRUE		((void *)2)
#define BGBDY_FALSE		((void *)3)
#define BGBDY_AUXNULL	((void *)4)

#define DYTF_UNDEFINED	(dytfPtrF(UNDEFINED))
#define DYTF_NULL		(dytfPtrF((void *)0))
#define DYTF_TRUE		(dytfPtrF((void *)2))
#define DYTF_FALSE		(dytfPtrF((void *)3))
#define DYTF_AUXNULL	(dytfPtrF((void *)4))

#ifndef dytname
#ifdef _BGBMETA
#define dytname(x)	__declspec(dyt_name(x))
#else
#define dytname(x)
#endif
#endif

#ifdef _BGBMETA

#ifndef transient
#define transient	__transient
#endif
#ifndef littleendian
#define littleendian	__ltlendian
#endif
#ifndef bigendian
#define bigendian	__bigendian
#endif
#ifndef dynamic
#define dynamic	__dynamic
#endif
#ifndef as_variant
#define as_variant	__dynamic
#endif

#else

#ifndef transient
#define transient
#endif
#ifndef littleendian
#define littleendian
#endif
#ifndef bigendian
#define bigendian
#endif
#ifndef dynamic
#define dynamic
#endif
#ifndef as_variant
#define as_variant
#endif

#endif


#ifndef BGBMETA_CPP_BEGIN
#if defined(__cplusplus)||defined(_BGBMETA)
#define BGBMETA_CPP_BEGIN
#define BGBMETA_CPP_END
#else
#define BGBMETA_CPP_BEGIN /##*
#define BGBMETA_CPP_END *##/
#endif
#endif


#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef SBYTE_T
#define SBYTE_T
typedef signed char sbyte;
#endif

#ifndef U8_T
#define U8_T
typedef unsigned char u8;
#endif

#ifndef S8_T
#define S8_T
typedef signed char s8;
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

#ifndef FCOMPLEX_T
#define FCOMPLEX_T
typedef struct {float r, i;} fcomplex;
#endif

#ifndef DCOMPLEX_T
#define DCOMPLEX_T
typedef struct {double r, i;} dcomplex;
#endif

#ifndef U128_T
#define U128_T
#ifdef __BSCC
typedef		 __int128 s128;
typedef unsigned __int128 u128;
typedef unsigned __float128 f128;
#else
// typedef struct { u32 v[4];} s128, u128;
typedef iv128_t s128, u128;
typedef fv128_t f128;
#endif
#endif

#ifndef F32_T
#define F32_T
typedef float f32;
#endif

#ifndef F64_T
#define F64_T
typedef double f64;
#endif

#ifndef F16_T
#define F16_T
#ifdef __BSCC
typedef __float16 float16;
#else
typedef struct { u16 v; } float16;
#endif
typedef float16 f16;
#endif

#ifndef F18_T
#define F18_T
typedef struct { u32 v; } float18;
#endif

#ifndef DYT_T
#define DYT_T
#ifdef _BGBMETA
typedef __variant dyt;
#else
#ifdef __BSCC
typedef void *dyt;
//typedef __variant dyt;
#else
//typedef void *dyt;
#ifdef __cplusplus
typedef class _dyt_s *dyt;
#else
typedef struct _dyt_s *dyt;
#endif
#endif
#endif
#endif

#ifndef DYTF_T
#define DYTF_T
#ifdef _BGBMETA
typedef __variantf dytf;
#else
// typedef u64 dytf;
typedef struct { u64 v; } dytf;
#endif
#endif

#ifndef DYTVA_T
#define DYTVA_T
#ifdef _BGBMETA
typedef __variant_va dytva;
typedef __variant_this dythis;
typedef __variant_cls dytcls;
#else
typedef dyt dytva;
typedef dyt dythis;
typedef dyt dytcls;
#endif
#endif

#ifndef __cplusplus
#ifndef BOOL_T
#define BOOL_T
typedef char bool;
#endif
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif
#endif

#if 1
typedef as_variant dytname("_classinfo_t")
	struct BGBGC_ClassInfo_s *dycClass;
typedef as_variant dytname("_class_slot_t")
	struct BGBGC_ClassSlot_s *dycSlot;
typedef as_variant dytname("_class_minf_t")
	struct BGBGC_ClassMinf_s *dycMethod;
typedef as_variant dytname("_classobj_t")
	struct BGBGC_ClassBase_s *dycObject;
typedef as_variant dytname("_array_t")
	struct BGBDYC_Array_s *dycArray;
typedef as_variant dytname("_sqrarray_t")
	struct BGBDYC_ArrayN_s *dycArrayN;
#endif

#ifndef DYCTHIS_T
#define DYCTHIS_T
#ifdef _BGBMETA
typedef __variant_va dycVA;
typedef __variant_this dycThis;
typedef __variant_cls dycThisClass;
#else
typedef dycArray dycVA;
typedef dycObject dycThis;
typedef dycClass dycThisClass;
#endif
#endif

#ifndef DYTVALUE_T
#define DYTVALUE_T
typedef union
{
s32 i;						//int
u32 ui;						//uint
s64 l;						//long
u64 ul;						//ulong
f32 f;						//float
f64 d;						//double
nlint a;					//address
void *pv;					//raw pointer
dyt r;						//variant reference
dytf cr;					//variant reference (double)
dycClass cls;				//class
dycObject obj;				//object
dycArray arr;				//array
struct { int x, y; }k;		//lexical env index
}dycValue;
#endif


// typedef struct { u32 a,b,c,d; } iv128_t;
// typedef struct { u32 a,b,c,d,e,f,g,h; } iv256_t;

// #define gcSet(pp, p)	BGBGC_CollectAssign((void **)(&(pp)), p);

#define dyIntF(a)	BGBDY_WrapIntFast(a)
#define dyIntvF(a)	BGBDY_UnwrapIntFast(a)

#define dyFloatF(a)	BGBDY_WrapMiniFloat(a)
#define dyFloatvF(a)	BGBDY_UnwrapMiniFloat(a)

#if defined(BGBGC_USECONS64) && !defined(PTRBITS_64)
#define dyCarF(a)			(BGBDY_CarF(a))
#define dyCdrF(a)			(BGBDY_CdrF(a))
#define dySetCarF(a, b)		(BGBDY_SetCarF(a, b))
#define dySetCdrF(a, b)		(BGBDY_SetCdrF(a, b))
#else
#define dyCarF(a)			(((dyt *)(a))[0])
#define dyCdrF(a)			(((dyt *)(a))[1])
#define dySetCarF(a, b)		((((dyt *)(a))[0])=(b))
#define dySetCdrF(a, b)		((((dyt *)(a))[1])=(b))
#endif

#define dyBoolF(a)	((a)?BGBDY_TRUE:BGBDY_FALSE)

#include <vecnf.h>
#include <matnf.h>
#include <dyvfsi.h>
#include <bgbdylli.h>

#include <bgbdy_int128.h>

#include <bgbdy_oo.h>
#include <bgbdy_xml.h>
#include <bgbdy_chan.h>

#include <bgbdy_vfile2.h>
#include <bgbdy_vfzip.h>

#include <bgbdy_dytf.h>

#include <bgbgc_wref.h>

#include <bgbdy_exwad.h>
#include <bgbdy_dyll.h>

#include <bgbgal.h>

#include <bgbdy_auto.h>

#ifdef BGBDY_DLL
#include <bgbdy_autoi.h>
#endif

#ifdef __cplusplus
}
#endif

//#ifdef __cplusplus
#if 0
static dyt operator+(dyt a, dyt b)
	{ return(dyadd(a, b)); }
static dyt operator-(dyt a, dyt b)
	{ return(dysub(a, b)); }
static dyt operator*(dyt a, dyt b)
	{ return(dymul(a, b)); }
static dyt operator/(dyt a, dyt b)
	{ return(dydiv(a, b)); }
static dyt operator%(dyt a, dyt b)
	{ return(dymod(a, b)); }
static dyt operator&(dyt a, dyt b)
	{ return(dyand(a, b)); }
static dyt operator|(dyt a, dyt b)
	{ return(dyor(a, b)); }
static dyt operator^(dyt a, dyt b)
	{ return(dyxor(a, b)); }

static dyt operator~(dyt a)
	{ return(dynot(a)); }
static dyt operator!(dyt a)
	{ return(dylnot(a)); }
static dyt operator-(dyt a)
	{ return(dyneg(a)); }
#endif

#endif
