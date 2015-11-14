#ifndef _DYOBJ_H_
#define _DYOBJ_H_

#include <stdarg.h>
#include <stdint.h>

typedef struct BGBGC_ClassInfo_s *dycClass;
typedef struct BGBGC_ClassSlot_s *dycSlot;
typedef struct BGBGC_ClassMinf_s *dycMethod;
typedef struct BGBGC_ClassBase_s *dycObject;

typedef struct BGBGC_ObjProto_s *dycObjProto;
typedef struct BGBGC_ClassVinf_s *dycClassV;

typedef struct BGBDYC_Array_s *dycArray;
typedef struct BGBDYC_ArrayN_s *dycArrayN;

#ifndef DYT_T
#define DYT_T
#ifdef __BSCC
typedef void *dyt;
//typedef __variant dyt;
#else
typedef void *dyt;
#endif
#endif


#include <dyc_auto.h>

#endif
