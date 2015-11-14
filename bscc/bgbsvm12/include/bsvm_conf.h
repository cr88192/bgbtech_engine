#if defined(__x86_64__) || defined(_M_X64)
#ifndef X86_64
#define X86_64
#endif
#ifndef __x86_64__
#define __x86_64__
#endif
#endif

#if defined(__i386__) || defined(_M_IX86)
#ifndef X86
#define X86
#endif
#ifndef __i386__
#define __i386__
#endif
#endif


#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

#ifdef _WIN64
#ifndef WIN64
#define WIN64
#endif
#endif

#ifdef _MSC_VER
#ifndef MSVC
#define MSVC
#endif
#endif

#ifdef WIN32
#ifdef BGBSVM_DLL
#define BGBSVM_API __declspec(dllexport)
#else
#define BGBSVM_API __declspec(dllimport)
#endif
#else
#define BGBSVM_API
#endif

#ifndef BYTE_T
#define BYTE_T
typedef unsigned char byte;
#endif

#ifndef SBYTE_T
#define SBYTE_T
typedef signed char sbyte;
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
