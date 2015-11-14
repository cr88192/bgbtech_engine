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


struct DYCNI_s {
//base/dyc_api.c
	int (*ObjectP)(dyt obj);
	int (*ClassP)(dyt obj);
	int (*SlotP)(dyt obj);
	int (*MethodP)(dyt obj);
	void (*BeginClass)(char *name, char *super);
	dycClass (*EndClass();
	void (*BeginClassAnon)(dycClass super);
	dycClass (*EndClassAnon();
	void (*ClassSuper)(char *name);
	void (*SlotBasic)(char *name, char *sig);
	void (*SlotStatic)(char *name, char *sig);
	void (*SlotInit)(char *name, char *sig);
	void (*SlotStatici)(char *name, char *sig, int val);
	void (*SlotStaticl)(char *name, char *sig, s64 val);
	void (*SlotStaticf)(char *name, char *sig, double val);
	void (*SlotStaticd)(char *name, char *sig, double val);
	void (*SlotStaticU)(char *name, char *sig, void *val);
	void (*SlotIniti)(char *name, char *sig, int val);
	void (*SlotInitl)(char *name, char *sig, s64 val);
	void (*SlotInitf)(char *name, char *sig, double val);
	void (*SlotInitd)(char *name, char *sig, double val);
	void (*SlotInitU)(char *name, char *sig, void *val);
	void (*SlotDelegate)(char *name, char *cname);
	void (*SlotStaticDelegate)(char *name, char *cname);
	void (*MethodBasic)(char *name, char *sig);
	void (*MethodStatic)(char *name, char *sig);
	void (*MethodFunc)(char *name, char *sig, dyt fcn);
	void (*MethodStaticFunc)(char *name, char *sig, dyt fcn);
	void (*MethodCFunc)(char *name, char *sig, void *fcn);
	void (*MethodCFuncData)(char *name, char *sig, void *fcn, void *data);
	void (*MethodCSelf)(char *name, char *sig, void *fcn);
	void (*MethodCSelfData)(char *name, char *sig, void *fcn, void *data);
	void (*MethodStaticCFunc)(char *name, char *sig, void *fcn);
	void (*MethodStaticCFuncData)(char *name, char *sig, void *fcn, void *data);
	void (*MethodStaticCSelf)(char *name, char *sig, void *fcn);
	void (*MethodStaticCSelfData)(char *name, char *sig, void *fcn, void *data);
	void (*SlotMethod)(char *name, char *sig);
	void (*StaticSlotMethod)(char *name, char *sig);
	void (*ClassInterface)(char *name);
	void (*BeginIface)(char *name, char *super);
	dycClass (*EndIface)();
	void (*IfaceSuper)(char *name);
	void (*IfaceMethod)(char *name, char *sig);
	void (*IfaceSlot)(char *name, char *sig);
	void (*BeginStruct)(char *name);
	dycClass (*EndStruct)();
	dycClass (*GetClass)(char *name);
	dycObject (*Alloc)(dycClass inf);
	int (*SubclassP)(dycClass info, dycClass super);
	int (*InstanceP)(dycObject obj, dycClass inf);
	dycClass (*GetObjClass)(dycObject obj);
	dycClass (*GetClassSuper)(dycClass inf);
	char *(*GetClassName)(dycClass inf);
	char *(*GetObjClassName)(dycObject obj);
	dycClassV (*GetClassVersion)(dycClass inf);
	int (*GetStructSize)(dycClassV vinf);
	dycClass (*GetVersionClass)(dycClassV vinf);
	char *(*GetSlotName)(dycSlot idx);
	char *(*GetSlotSig)(dycSlot idx);
	dycClass (*GetSlotClass)(dycSlot idx);
	char *(*GetMethodName)(dycMethod idx);
	char *(*GetMethodSig)(dycMethod idx);
	dycClass (*GetMethodClass)(dycMethod idx);
	dycSlot (*IdxSlot)(dycClass info, char *name);
	dycSlot (*IdxObjSlot)(dycObject obj, char *name);
	dycMethod (*IdxMethod)(dycClass inf, char *name, char *sig);
	dycMethod (*IdxObjMethod)(dycObject obj, char *name, char *sig);
	void *(*GetSlotPtr)(dycObject obj, dycSlot idx);
	void *(*GetInitPtr)(dycClass inf, dycSlot idx);
	void *(*GetStaticPtr)(dycClass inf, dycSlot idx);
	void *(*GetStructPtr)(dycClassV vinf, void *obj, dycSlot idx);
	void *(*PtrSlot)(dycObject obj, char *name);
	void *(*PtrStaticSlot)(dycClass info, char *name);
	void *(*PtrInitSlot)(dycClass info, char *name);
	void *(*PtrStructSlot)(dycClassV vinf, void *obj, char *name);
	int (*InstanceOfP)(dycObject obj, char *name);
	int (*SubclassOfP)(char *name, char *super);
	dyt (*LookupMethodSig)(dycObject obj, char *name, char *sig);
	dyt (*GetMethodFunc)(dycClass inf, dycMethod idx);
	dyt (*GetObjMethodFunc)(dycObject obj, dycMethod idx);
	void (*SetMethodFunc)(dycClass inf, dycMethod idx, dyt fcn);
	void (*SetObjMethodFunc)(dycObject obj, dycMethod idx, dyt fcn);
	dycSlot (*AddObjSlot)(dycObject obj, char *name, char *sig);
	dycMethod (*AddObjMethod)(dycObject obj, char *name, char *sig);
	dycObject (*CloneObject)(dycObject obj);
	dycObject (*GetObjDelegate)(dycObject obj, char *name);
	void (*SetObjDelegate)(dycObject obj, char *name, dycObject obj1);
	dycSlot *(*dycEnumSlotArray)(dycClass inf);
	dycMethod *(*dycEnumMethodArray)(dycClass inf);
	dycSlot *(*dycEnumClassSlotArray)(char *name);
	dycMethod *(*dycEnumClassMethodArray)(char *name);
	dycObject (*AllocClass)(char *name);
	void *(*PtrClassSlot)(char *cname, char *sname);
	dycSlot (*IdxClassSlot)(char *cname, char *sname);
	dycMethod (*IdxClassMethod)(char *cname, char *name, char *sig);
	dyt (*GetSlot)(dycObject obj, char *name);
	dyt (*GetSlotr)(dycObject obj, char *name);
	void *(*GetSlotp)(dycObject obj, char *name);
	int (*GetSlotsb)(dycObject obj, char *name);
	int (*GetSlotub)(dycObject obj, char *name);
	int (*GetSlotss)(dycObject obj, char *name);
	int (*GetSlotus)(dycObject obj, char *name);
	int (*GetSloti)(dycObject obj, char *name);
	s64 (*GetSlotl)(dycObject obj, char *name);
	float (*GetSlotf)(dycObject obj, char *name);
	double (*GetSlotd)(dycObject obj, char *name);
	int (*SetSlot)(dycObject obj, char *name, dyt val);
	int (*SetSlotr)(dycObject obj, char *name, dyt val);
	int (*SetSlotp)(dycObject obj, char *name, void *val);
	int (*SetSlotsb)(dycObject obj, char *name, int val);
	int (*SetSlotub)(dycObject obj, char *name, int val);
	int (*SetSlotss)(dycObject obj, char *name, int val);
	int (*SetSlotus)(dycObject obj, char *name, int val);
	int (*SetSloti)(dycObject obj, char *name, int val);
	int (*SetSlotl)(dycObject obj, char *name, s64 val);
	int (*SetSlotf)(dycObject obj, char *name, float val);
	int (*SetSlotd)(dycObject obj, char *name, double val);
	int (*Getsb)(dycObject obj, dycSlot idx);
	int (*Getub)(dycObject obj, dycSlot idx);
	int (*Getss)(dycObject obj, dycSlot idx);
	int (*Getus)(dycObject obj, dycSlot idx);
	int (*Geti)(dycObject obj, dycSlot idx);
	s64 (*Getl)(dycObject obj, dycSlot idx);
	float (*Getf)(dycObject obj, dycSlot idx);
	double (*Getd)(dycObject obj, dycSlot idx);
	dyt (*Getr)(dycObject obj, dycSlot idx);
	void *(*Getp)(dycObject obj, dycSlot idx);
	void (*Setsb)(dycObject obj, dycSlot idx, int val);
	void (*Setub)(dycObject obj, dycSlot idx, int val);
	void (*Setss)(dycObject obj, dycSlot idx, int val);
	void (*Setus)(dycObject obj, dycSlot idx, int val);
	void (*Seti)(dycObject obj, dycSlot idx, int val);
	void (*Setl)(dycObject obj, dycSlot idx, s64 val);
	void (*Setf)(dycObject obj, dycSlot idx, float val);
	void (*Setd)(dycObject obj, dycSlot idx, double val);
	void (*Setr)(dycObject obj, dycSlot idx, dyt val);
	void (*Setp)(dycObject obj, dycSlot idx, void *val);
	int (*GetStaticsb)(dycClass inf, dycSlot idx);
	int (*GetStaticub)(dycClass inf, dycSlot idx);
	int (*GetStaticss)(dycClass inf, dycSlot idx);
	int (*GetStaticus)(dycClass inf, dycSlot idx);
	int (*GetStatici)(dycClass inf, dycSlot idx);
	s64 (*GetStaticl)(dycClass inf, dycSlot idx);
	float (*GetStaticf)(dycClass inf, dycSlot idx);
	double (*GetStaticd)(dycClass inf, dycSlot idx);
	dyt (*GetStaticr)(dycClass inf, dycSlot idx);
	void *(*GetStaticp)(dycClass inf, dycSlot idx);
	void (*SetStaticsb)(dycClass inf, dycSlot idx, int val);
	void (*SetStaticub)(dycClass inf, dycSlot idx, int val);
	void (*SetStaticss)(dycClass inf, dycSlot idx, int val);
	void (*SetStaticus)(dycClass inf, dycSlot idx, int val);
	void (*SetStatici)(dycClass inf, dycSlot idx, int val);
	void (*SetStaticl)(dycClass inf, dycSlot idx, s64 val);
	void (*SetStaticf)(dycClass inf, dycSlot idx, float val);
	void (*SetStaticd)(dycClass inf, dycSlot idx, double val);
	void (*SetStaticr)(dycClass inf, dycSlot idx, dyt val);
	void (*SetStaticp)(dycClass inf, dycSlot idx, void *val);
	int (*GetInitsb)(dycClass inf, dycSlot idx);
	int (*GetInitub)(dycClass inf, dycSlot idx);
	int (*GetInitss)(dycClass inf, dycSlot idx);
	int (*GetInitus)(dycClass inf, dycSlot idx);
	int (*GetIniti)(dycClass inf, dycSlot idx);
	s64 (*GetInitl)(dycClass inf, dycSlot idx);
	float (*GetInitf)(dycClass inf, dycSlot idx);
	double (*GetInitd)(dycClass inf, dycSlot idx);
	dyt (*GetInitr)(dycClass inf, dycSlot idx);
	void *(*GetInitp)(dycClass inf, dycSlot idx);
	void (*SetInitsb)(dycClass inf, dycSlot idx, int val);
	void (*SetInitub)(dycClass inf, dycSlot idx, int val);
	void (*SetInitss)(dycClass inf, dycSlot idx, int val);
	void (*SetInitus)(dycClass inf, dycSlot idx, int val);
	void (*SetIniti)(dycClass inf, dycSlot idx, int val);
	void (*SetInitl)(dycClass inf, dycSlot idx, s64 val);
	void (*SetInitf)(dycClass inf, dycSlot idx, float val);
	void (*SetInitd)(dycClass inf, dycSlot idx, double val);
	void (*SetInitr)(dycClass inf, dycSlot idx, dyt val);
	void (*SetInitp)(dycClass inf, dycSlot idx, void *val);
	int (*GetStructsb)(dycClassV vinf, void *obj, dycSlot idx);
	int (*GetStructub)(dycClassV vinf, void *obj, dycSlot idx);
	int (*GetStructss)(dycClassV vinf, void *obj, dycSlot idx);
	int (*GetStructus)(dycClassV vinf, void *obj, dycSlot idx);
	int (*GetStructi)(dycClassV vinf, void *obj, dycSlot idx);
	s64 (*GetStructl)(dycClassV vinf, void *obj, dycSlot idx);
	float (*GetStructf)(dycClassV vinf, void *obj, dycSlot idx);
	double (*GetStructd)(dycClassV vinf, void *obj, dycSlot idx);
	dyt (*GetStructr)(dycClassV vinf, void *obj, dycSlot idx);
	void *(*GetStructp)(dycClassV vinf, void *obj, dycSlot idx);
	void (*SetStructsb)(dycClassV vinf, void *obj, dycSlot idx, int val);
	void (*SetStructub)(dycClassV vinf, void *obj, dycSlot idx, int val);
	void (*SetStructss)(dycClassV vinf, void *obj, dycSlot idx, int val);
	void (*SetStructus)(dycClassV vinf, void *obj, dycSlot idx, int val);
	void (*SetStructi)(dycClassV vinf, void *obj, dycSlot idx, int val);
	void (*SetStructl)(dycClassV vinf, void *obj, dycSlot idx, s64 val);
	void (*SetStructf)(dycClassV vinf, void *obj, dycSlot idx, float val);
	void (*SetStructd)(dycClassV vinf, void *obj, dycSlot idx, double val);
	void (*SetStructr)(dycClassV vinf, void *obj, dycSlot idx, dyt val);
	void (*SetStructp)(dycClassV vinf, void *obj, dycSlot idx, void *val);
	dycObject (*NewClass)(char *name);
	dycObject (*NewInitClassV)(char *name, char *sig, va_list args);
	dycObject (*NewInitClassU)(char *name, char *sig, void **args);
	dycObject (*New)(dycClass inf);
	dycObject (*NewInitV)(dycClass inf, char *sig, va_list args);
	dycObject (*NewInitU)(dycClass inf, char *sig, void **args);
	void *(*CallSigU)(dycObject obj, char *name, char *sig, void **args);
	void *(*CallSigV)(dycObject obj, char *name, char *sig, va_list args);
	dyt (*CallSigVr)(dycObject obj, char *name, char *sig, va_list lst);
	int (*CallSigVi)(dycObject obj, char *name, char *sig, va_list lst);
	s64 (*CallSigVl)(dycObject obj, char *name, char *sig, va_list lst);
	float (*CallSigVf)(dycObject obj, char *name, char *sig, va_list lst);
	double (*CallSigVd)(dycObject obj, char *name, char *sig, va_list lst);
	dyt (*CallSigUr)(dycObject obj, char *name, char *sig, void **args);
	int (*CallSigUi)(dycObject obj, char *name, char *sig, void **args);
	s64 (*CallSigUl)(dycObject obj, char *name, char *sig, void **args);
	float (*CallSigUf)(dycObject obj, char *name, char *sig, void **args);
	double (*CallSigUd)(dycObject obj, char *name, char *sig, void **args);
	void *(*CallU)(dycObject obj, dycMethod mth, void **args);
	void *(*CallV)(dycObject obj, dycMethod mth, va_list args);
	int (*CallUi)(dycObject obj, dycMethod mth, void **args);
	s64 (*CallUl)(dycObject obj, dycMethod mth, void **args);
	float (*CallUf)(dycObject obj, dycMethod mth, void **args);
	double (*CallUd)(dycObject obj, dycMethod mth, void **args);
	dyt (*CallUr)(dycObject obj, dycMethod mth, void **args);
	void *(*CallUp)(dycObject obj, dycMethod mth, void **args);
	void (*CallUv)(dycObject obj, dycMethod mth, void **args);
	int (*CallVi)(dycObject obj, dycMethod mth, va_list args);
	s64 (*CallVl)(dycObject obj, dycMethod mth, va_list args);
	float (*CallVf)(dycObject obj, dycMethod mth, va_list args);
	double (*CallVd)(dycObject obj, dycMethod mth, va_list args);
	dyt (*CallVr)(dycObject obj, dycMethod mth, va_list args);
	void *(*CallVp)(dycObject obj, dycMethod mth, va_list args);
	void (*CallVv)(dycObject obj, dycMethod mth, va_list args);
	void *(*CallStaticU)(dycClass inf, dycMethod mth, void **args);
	void *(*CallStaticV)(dycClass inf, dycMethod mth, va_list args);
	int (*CallStaticUi)(dycClass inf, dycMethod mth, void **args);
	s64 (*CallStaticUl)(dycClass inf, dycMethod mth, void **args);
	float (*CallStaticUf)(dycClass inf, dycMethod mth, void **args);
	double (*CallStaticUd)(dycClass inf, dycMethod mth, void **args);
	dyt (*CallStaticUr)(dycClass inf, dycMethod mth, void **args);
	void *(*CallStaticUp)(dycClass inf, dycMethod mth, void **args);
	void (*CallStaticUv)(dycClass inf, dycMethod mth, void **args);
	int (*CallStaticVi)(dycClass inf, dycMethod mth, va_list args);
	s64 (*CallStaticVl)(dycClass inf, dycMethod mth, va_list args);
	float (*CallStaticVf)(dycClass inf, dycMethod mth, va_list args);
	double (*CallStaticVd)(dycClass inf, dycMethod mth, va_list args);
	dyt (*CallStaticVr)(dycClass inf, dycMethod mth, va_list args);
	void *(*CallStaticVp)(dycClass inf, dycMethod mth, va_list args);
	void (*CallStaticVv)(dycClass inf, dycMethod mth, va_list args);
	void *(*CallAsU)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	void *(*CallAsV)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	int (*CallAsUi)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	s64 (*CallAsUl)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	float (*CallAsUf)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	double (*CallAsUd)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	dyt (*CallAsUr)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	void *(*CallAsUp)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	void (*CallAsUv)(dycObject obj, dycClass inf, dycMethod mth, void **args);
	int (*CallAsVi)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	s64 (*CallAsVl)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	float (*CallAsVf)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	double (*CallAsVd)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	dyt (*CallAsVr)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	void *(*CallAsVp)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	void (*CallAsVv)(dycObject obj, dycClass inf, dycMethod mth, va_list args);
	void *(*CallStructU)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	void *(*CallStructV)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
	int (*CallStructUi)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	s64 (*CallStructUl)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	float (*CallStructUf)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	double (*CallStructUd)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	dyt (*CallStructUr)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	void *(*CallStructUp)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	void (*CallStructUv)(void *obj, dycClassV vinf, dycMethod mth, void **args);
	int (*CallStructVi)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
	s64 (*CallStructVl)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
	float (*CallStructVf)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
	double (*CallStructVd)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
	dyt (*CallStructVr)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
	void *(*CallStructVp)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
	void (*CallStructVv)(void *obj, dycClassV vinf, dycMethod mth, va_list args);
//base/dyc_array.c
	dycArray (*NewArray)(char *sig, int cnt);
	int (*ArrayP)(dyt p);
	int (*ArraySigP)(dyt p, char *sig);
	void *(*ArrayV)(dycArray arr);
	int (*ArraySz)(dycArray arr);
	char *(*ArraySig)(dycArray arr);
	int (*ArrayStep)(dycArray arr);
	void *(*ArrayIdxPtr)(dycArray arr, int idx);
	int (*GetArraysb)(dycArray arr, int idx);
	int (*GetArrayub)(dycArray arr, int idx);
	int (*GetArrayss)(dycArray arr, int idx);
	int (*GetArrayus)(dycArray arr, int idx);
	int (*GetArrayi)(dycArray arr, int idx);
	s64 (*GetArrayl)(dycArray arr, int idx);
	float (*GetArrayf)(dycArray arr, int idx);
	double (*GetArrayd)(dycArray arr, int idx);
	dyt (*GetArrayr)(dycArray arr, int idx);
	void *(*GetArrayp)(dycArray arr, int idx);
	void (*SetArraysb)(dycArray arr, int idx, int val);
	void (*SetArrayub)(dycArray arr, int idx, int val);
	void (*SetArrayss)(dycArray arr, int idx, int val);
	void (*SetArrayus)(dycArray arr, int idx, int val);
	void (*SetArrayi)(dycArray arr, int idx, int val);
	void (*SetArrayl)(dycArray arr, int idx, s64 val);
	void (*SetArrayf)(dycArray arr, int idx, float val);
	void (*SetArrayd)(dycArray arr, int idx, double val);
	void (*SetArrayr)(dycArray arr, int idx, dyt val);
	void (*SetArrayp)(dycArray arr, int idx, void *val);
	void *(*ArrayIdxPtrBC)(dycArray arr, int idx);
	int (*GetArraySsb)(dycArray arr, int idx);
	int (*GetArraySub)(dycArray arr, int idx);
	int (*GetArraySss)(dycArray arr, int idx);
	int (*GetArraySus)(dycArray arr, int idx);
	int (*GetArraySi)(dycArray arr, int idx);
	s64 (*GetArraySl)(dycArray arr, int idx);
	float (*GetArraySf)(dycArray arr, int idx);
	double (*GetArraySd)(dycArray arr, int idx);
	dyt (*GetArraySr)(dycArray arr, int idx);
	void *(*GetArraySp)(dycArray arr, int idx);
	void (*SetArraySsb)(dycArray arr, int idx, int val);
	void (*SetArraySub)(dycArray arr, int idx, int val);
	void (*SetArraySss)(dycArray arr, int idx, int val);
	void (*SetArraySus)(dycArray arr, int idx, int val);
	void (*SetArraySi)(dycArray arr, int idx, int val);
	void (*SetArraySl)(dycArray arr, int idx, s64 val);
	void (*SetArraySf)(dycArray arr, int idx, float val);
	void (*SetArraySd)(dycArray arr, int idx, double val);
	void (*SetArraySr)(dycArray arr, int idx, dyt val);
	void (*SetArraySp)(dycArray arr, int idx, void *val);
	dycArray (*NewArrayM)(char *sig, int ord, int *sz);
	dycArray (*NewArrayM2)(char *sig, int sz0, int sz1);
	dycArray (*NewArrayM3)(char *sig, int sz0, int sz1, int sz2);
	dycArray (*NewArrayM4)(char *sig, int sz0, int sz1, int sz2, int sz3);
	dycArray (*NewArrayM5)(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4);
	dycArray (*NewArrayM6)(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5);
	dycArray (*NewArrayM7)(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6);
	dycArray (*NewArrayM8)(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6, int sz7);
	dycArray (*NewArrayM9)(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6, int sz7, int sz8);
	dycArray (*NewArrayM10)(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6, int sz7, int sz8, int sz9);
	dycArray (*NewArrayM11)(char *sig,int sz0, int sz1, int sz2 , int sz3, int sz4, int sz5, int sz6 , int sz7, int sz8, int sz9, int sz10);
	dycArray (*NewArrayM12)(char *sig,int sz0, int sz1, int sz2 , int sz3, int sz4, int sz5, int sz6 , int sz7, int sz8, int sz9, int sz10, int sz11);
	dycArrayN (*NewArrayN)(char *sig, int ord, int *sz);
	int (*ArrayNP)(dyt p);
	int (*ArrayNSigP)(dyt p, char *sig);
	int (*ArrayNSigOrdP)(dyt p, char *sig, int ord);
	void *(*ArrayNV)(dycArrayN arr);
	int *(*dycArrayNSz)(dycArrayN arr);
	int (*ArrayNCnt)(dycArrayN arr);
	int (*ArrayNOrd)(dycArrayN arr);
	char *(*ArrayNSig)(dycArrayN arr);
	int (*ArrayNStep)(dycArrayN arr);
	void *(*ArrayNIdxPtr)(dycArrayN arr, int *idx);
	void *(*ArrayNIdxPtrBC)(dycArrayN arr, int *idx);
	int (*GetArrayNsb)(dycArrayN arr, int *idx);
	int (*GetArrayNub)(dycArrayN arr, int *idx);
	int (*GetArrayNss)(dycArrayN arr, int *idx);
	int (*GetArrayNus)(dycArrayN arr, int *idx);
	int (*GetArrayNi)(dycArrayN arr, int *idx);
	s64 (*GetArrayNl)(dycArrayN arr, int *idx);
	float (*GetArrayNf)(dycArrayN arr, int *idx);
	double (*GetArrayNd)(dycArrayN arr, int *idx);
	dyt (*GetArrayNr)(dycArrayN arr, int *idx);
	void *(*GetArrayNp)(dycArrayN arr, int *idx);
	void (*SetArrayNsb)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNub)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNss)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNus)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNi)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNl)(dycArrayN arr, int *idx, s64 val);
	void (*SetArrayNf)(dycArrayN arr, int *idx, float val);
	void (*SetArrayNd)(dycArrayN arr, int *idx, double val);
	void (*SetArrayNr)(dycArrayN arr, int *idx, dyt val);
	void (*SetArrayNp)(dycArrayN arr, int *idx, void *val);
	int (*GetArrayNSsb)(dycArrayN arr, int *idx);
	int (*GetArrayNSub)(dycArrayN arr, int *idx);
	int (*GetArrayNSss)(dycArrayN arr, int *idx);
	int (*GetArrayNSus)(dycArrayN arr, int *idx);
	int (*GetArrayNSi)(dycArrayN arr, int *idx);
	s64 (*GetArrayNSl)(dycArrayN arr, int *idx);
	float (*GetArrayNSf)(dycArrayN arr, int *idx);
	double (*GetArrayNSd)(dycArrayN arr, int *idx);
	dyt (*GetArrayNSr)(dycArrayN arr, int *idx);
	void *(*GetArrayNSp)(dycArrayN arr, int *idx);
	void (*SetArrayNSsb)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNSub)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNSss)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNSus)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNSi)(dycArrayN arr, int *idx, int val);
	void (*SetArrayNSl)(dycArrayN arr, int *idx, s64 val);
	void (*SetArrayNSf)(dycArrayN arr, int *idx, float val);
	void (*SetArrayNSd)(dycArrayN arr, int *idx, double val);
	void (*SetArrayNSr)(dycArrayN arr, int *idx, dyt val);
	void (*SetArrayNSp)(dycArrayN arr, int *idx, void *val);
	void *(*Array2IdxPtr)(dycArrayN arr, int y, int x);
	void *(*Array2IdxPtrBC)(dycArrayN arr, int y, int x);
	int (*GetArray2sb)(dycArrayN arr, int y, int x);
	int (*GetArray2ub)(dycArrayN arr, int y, int x);
	int (*GetArray2ss)(dycArrayN arr, int y, int x);
	int (*GetArray2us)(dycArrayN arr, int y, int x);
	int (*GetArray2i)(dycArrayN arr, int y, int x);
	s64 (*GetArray2l)(dycArrayN arr, int y, int x);
	float (*GetArray2f)(dycArrayN arr, int y, int x);
	double (*GetArray2d)(dycArrayN arr, int y, int x);
	dyt (*GetArray2r)(dycArrayN arr, int y, int x);
	void *(*GetArray2p)(dycArrayN arr, int y, int x);
	void (*SetArray2sb)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2ub)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2ss)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2us)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2i)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2l)(dycArrayN arr, int y, int x, s64 val);
	void (*SetArray2f)(dycArrayN arr, int y, int x, float val);
	void (*SetArray2d)(dycArrayN arr, int y, int x, double val);
	void (*SetArray2r)(dycArrayN arr, int y, int x, dyt val);
	void (*SetArray2p)(dycArrayN arr, int y, int x, void *val);
	int (*GetArray2Ssb)(dycArrayN arr, int y, int x);
	int (*GetArray2Sub)(dycArrayN arr, int y, int x);
	int (*GetArray2Sss)(dycArrayN arr, int y, int x);
	int (*GetArray2Sus)(dycArrayN arr, int y, int x);
	int (*GetArray2Si)(dycArrayN arr, int y, int x);
	s64 (*GetArray2Sl)(dycArrayN arr, int y, int x);
	float (*GetArray2Sf)(dycArrayN arr, int y, int x);
	double (*GetArray2Sd)(dycArrayN arr, int y, int x);
	dyt (*GetArray2Sr)(dycArrayN arr, int y, int x);
	void *(*GetArray2Sp)(dycArrayN arr, int y, int x);
	void (*SetArray2Ssb)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2Sub)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2Sss)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2Sus)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2Si)(dycArrayN arr, int y, int x, int val);
	void (*SetArray2Sl)(dycArrayN arr, int y, int x, s64 val);
	void (*SetArray2Sf)(dycArrayN arr, int y, int x, float val);
	void (*SetArray2Sd)(dycArrayN arr, int y, int x, double val);
	void (*SetArray2Sr)(dycArrayN arr, int y, int x, dyt val);
	void (*SetArray2Sp)(dycArrayN arr, int y, int x, void *val);
	void *(*Array3IdxPtr)(dycArrayN arr, int z, int y, int x);
	void *(*Array3IdxPtrBC)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3sb)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3ub)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3ss)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3us)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3i)(dycArrayN arr, int z, int y, int x);
	s64 (*GetArray3l)(dycArrayN arr, int z, int y, int x);
	float (*GetArray3f)(dycArrayN arr, int z, int y, int x);
	double (*GetArray3d)(dycArrayN arr, int z, int y, int x);
	dyt (*GetArray3r)(dycArrayN arr, int z, int y, int x);
	void *(*GetArray3p)(dycArrayN arr, int z, int y, int x);
	void (*SetArray3sb)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3ub)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3ss)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3us)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3i)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3l)(dycArrayN arr, int z, int y, int x, s64 val);
	void (*SetArray3f)(dycArrayN arr, int z, int y, int x, float val);
	void (*SetArray3d)(dycArrayN arr, int z, int y, int x, double val);
	void (*SetArray3r)(dycArrayN arr, int z, int y, int x, dyt val);
	void (*SetArray3p)(dycArrayN arr, int z, int y, int x, void *val);
	int (*GetArray3Ssb)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3Sub)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3Sss)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3Sus)(dycArrayN arr, int z, int y, int x);
	int (*GetArray3Si)(dycArrayN arr, int z, int y, int x);
	s64 (*GetArray3Sl)(dycArrayN arr, int z, int y, int x);
	float (*GetArray3Sf)(dycArrayN arr, int z, int y, int x);
	double (*GetArray3Sd)(dycArrayN arr, int z, int y, int x);
	dyt (*GetArray3Sr)(dycArrayN arr, int z, int y, int x);
	void *(*GetArray3Sp)(dycArrayN arr, int z, int y, int x);
	void (*SetArray3Ssb)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3Sub)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3Sss)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3Sus)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3Si)(dycArrayN arr, int z, int y, int x, int val);
	void (*SetArray3Sl)(dycArrayN arr, int z, int y, int x, s64 val);
	void (*SetArray3Sf)(dycArrayN arr, int z, int y, int x, float val);
	void (*SetArray3Sd)(dycArrayN arr, int z, int y, int x, double val);
	void (*SetArray3Sr)(dycArrayN arr, int z, int y, int x, dyt val);
	void (*SetArray3Sp)(dycArrayN arr, int z, int y, int x, void *val);
};

extern struct DYCNI_s *DYCNI_InitGet();

static struct DYCNI_s *dycniGet()
{
	static struct DYCNI_s *tmp=NULL;
	if(tmp)return(tmp);
	tmp=DYCNI_InitGet();
	return(tmp);
}

#define dycni (dycniGet())

//base/dyc_api.c
static int dycObjectP(dyt obj)
	{ return(dycni->ObjectP(obj)); }
static int dycClassP(dyt obj)
	{ return(dycni->ClassP(obj)); }
static int dycSlotP(dyt obj)
	{ return(dycni->SlotP(obj)); }
static int dycMethodP(dyt obj)
	{ return(dycni->MethodP(obj)); }
static void dycBeginClass(char *name, char *super)
	{ dycni->BeginClass(name, super); }
static dycClass dycEndClass()
	{ return(dycni->EndClass()); }
static void dycBeginClassAnon(dycClass super)
	{ dycni->BeginClassAnon(super); }
static dycClass dycEndClassAnon()
	{ return(dycni->EndClassAnon()); }
static void dycClassSuper(char *name)
	{ dycni->ClassSuper(name); }

static void dycSlotBasic(char *name, char *sig)
	{ dycni->SlotBasic(name, sig); }
static void dycSlotStatic(char *name, char *sig)
	{ dycni->SlotStatic(name, sig); }
static void dycSlotInit(char *name, char *sig)
	{ dycni->SlotInit(name, sig); }

static void dycSlotStatici(char *name, char *sig, int val)
	{ dycni->SlotStatici(name, sig, val); }
static void dycSlotStaticl(char *name, char *sig, s64 val)
	{ dycni->SlotStaticl(name, sig, val); }
static void dycSlotStaticf(char *name, char *sig, double val)
	{ dycni->SlotStaticf(name, sig, val); }
static void dycSlotStaticd(char *name, char *sig, double val)
	{ dycni->SlotStaticd(name, sig, val); }
static void dycSlotStaticU(char *name, char *sig, void *val)
	{ dycni->SlotStaticU(name, sig, val); }
static void dycSlotIniti(char *name, char *sig, int val)
	{ dycni->SlotIniti(name, sig, val); }
static void dycSlotInitl(char *name, char *sig, s64 val)
	{ dycni->SlotInitl(name, sig, val); }
static void dycSlotInitf(char *name, char *sig, double val)
	{ dycni->SlotInitf(name, sig, val); }
static void dycSlotInitd(char *name, char *sig, double val)
	{ dycni->SlotInitd(name, sig, val); }
static void dycSlotInitU(char *name, char *sig, void *val)
	{ dycni->SlotInitU(name, sig, val); }

static void dycSlotDelegate(char *name, char *cname)
	{ dycni->SlotDelegate(name, cname); }
static void dycSlotStaticDelegate(char *name, char *cname)
	{ dycni->SlotStaticDelegate(name, cname); }
static void dycMethodBasic(char *name, char *sig)
	{ dycni->MethodBasic(name, sig); }
static void dycMethodStatic(char *name, char *sig)
	{ dycni->MethodStatic(name, sig); }
static void dycMethodFunc(char *name, char *sig, dyt fcn)
	{ dycni->MethodFunc(name, sig, fcn); }
static void dycMethodStaticFunc(char *name, char *sig, dyt fcn)
	{ dycni->MethodStaticFunc(name, sig, fcn); }

static void dycMethodCFunc(char *name, char *sig, void *fcn)
	{ dycni->MethodCFunc(name, sig, fcn); }
static void dycMethodCFuncData(char *name, char *sig, void *fcn, void *data)
	{ dycni->MethodCFuncData(name, sig, fcn, data); }
static void dycMethodCSelf(char *name, char *sig, void *fcn);
	{ dycni->MethodCSelf(name, sig, fcn); }
static void dycMethodCSelfData(char *name, char *sig, void *fcn, void *data);
	{ dycni->MethodCSelfData(name, sig, fcn, data); }

static void dycMethodStaticCFunc(char *name, char *sig, void *fcn)
	{ dycni->MethodStaticCFunc(name, sig, fcn); }
static void dycMethodStaticCFuncData(char *name, char *sig, void *fcn, void *data)
	{ dycni->MethodStaticCFuncData(name, sig, fcn, data); }
static void dycMethodStaticCSelf(char *name, char *sig, void *fcn)
	{ dycni->MethodStaticCSelf(name, sig, fcn); }
static void dycMethodStaticCSelfData(char *name, char *sig, void *fcn, void *data)
	{ dycni->MethodStaticCSelfData(name, sig, fcn, data); }

static void dycSlotMethod(char *name, char *sig)
	{ dycni->SlotMethod(name, sig); }
static void dycStaticSlotMethod(char *name, char *sig)
	{ dycni->StaticSlotMethod(name, sig); }

static void dycClassInterface(char *name);
	{ dycni->ClassInterface(name); }
static void dycBeginIface(char *name, char *super);
	{ dycni->BeginIface(name, super); }

static dycClass dycEndIface();
	{ return(dycni->EndIface()); }
static void dycIfaceSuper(char *name)
	{ dycni->IfaceSuper(name); }
static void dycIfaceMethod(char *name, char *sig)
	{ dycni->IfaceMethod(name, sig); }
static void dycIfaceSlot(char *name, char *sig)
	{ dycni->IfaceSlot(name, sig); }

static void dycBeginStruct(char *name)
	{ dycni->BeginStruct(name); }
static dycClass dycEndStruct()
	{ return(dycni->EndStruct()); }

static dycClass dycGetClass(char *name)
	{ return(dycni->GetClass(name)); }
static dycObject dycAlloc(dycClass inf)
	{ return(dycni->Alloc(inf)); }

static int dycSubclassP(dycClass info, dycClass super)
	{ return(dycni->SubclassP(info, super)); }
static int dycInstanceP(dycObject obj, dycClass inf)
	{ return(dycni->InstanceP(obj, inf)); }

static dycClass dycGetObjClass(dycObject obj)
	{ return(dycni->GetObjClass(obj)); }
static dycClass dycGetClassSuper(dycClass inf)
	{ return(dycni->GetClassSuper(inf)); }
static char *dycGetClassName(dycClass inf)
	{ return(dycni->GetClassName(inf)); }
static char *dycGetObjClassName(dycObject obj)
	{ return(dycni->GetObjClassName(obj)); }
static dycClassV dycGetClassVersion(dycClass inf)
	{ return(dycni->GetClassVersion(inf)); }
static int dycGetStructSize(dycClassV vinf)
	{ return(dycni->GetStructSize(vinf)); }
static dycClass dycGetVersionClass(dycClassV vinf)
	{ return(dycni->GetVersionClass(vinf)); }

static char *dycGetSlotName(dycSlot idx)
	{ return(dycni->GetSlotName(idx)); }
static char *dycGetSlotSig(dycSlot idx)
	{ return(dycni->GetSlotSig(idx)); }
static dycClass dycGetSlotClass(dycSlot idx)
	{ return(dycni->GetSlotClass(idx)); }
static char *dycGetMethodName(dycMethod idx)
	{ return(dycni->GetMethodName(idx)); }
static char *dycGetMethodSig(dycMethod idx)
	{ return(dycni->GetMethodSig(idx)); }
static dycClass dycGetMethodClass(dycMethod idx)
	{ return(dycni->GetMethodClass(idx)); }

static dycSlot dycIdxSlot(dycClass info, char *name)
	{ return(dycni->IdxSlot(info, name)); }
static dycSlot dycIdxObjSlot(dycObject obj, char *name)
	{ return(dycni->IdxObjSlot(obj, name)); }
static dycMethod dycIdxMethod(dycClass inf, char *name, char *sig)
	{ return(dycni->IdxMethod(inf, name, sig)); }
static dycMethod dycIdxObjMethod(dycObject obj, char *name, char *sig)
	{ return(dycni->IdxObjMethod(obj, name, sig)); }

static void *dycGetSlotPtr(dycObject obj, dycSlot idx)
	{ return(dycni->GetSlotPtr(obj, idx)); }
static void *dycGetInitPtr(dycClass inf, dycSlot idx)
	{ return(dycni->GetInitPtr(inf, idx)); }
static void *dycGetStaticPtr(dycClass inf, dycSlot idx)
	{ return(dycni->GetStaticPtr(inf, idx)); }
static void *dycGetStructPtr(dycClassV vinf, void *obj, dycSlot idx);
	{ return(dycni->GetStructPtr(vinf, obj, idx)); }

static void *dycPtrSlot(dycObject obj, char *name)
	{ return(dycni->PtrSlot(obj, name)); }
static void *dycPtrStaticSlot(dycClass info, char *name)
	{ return(dycni->PtrStaticSlot(info, name)); }
static void *dycPtrInitSlot(dycClass info, char *name)
	{ return(dycni->PtrInitSlot(info, name)); }
static void *dycPtrStructSlot(dycClassV vinf, void *obj, char *name)
	{ return(dycni->PtrStructSlot(vinf, obj, name)); }

static int dycInstanceOfP(dycObject obj, char *name)
	{ return(dycni->PtrInstanceOf(obj, name)); }
static int dycSubclassOfP(char *name, char *super)
	{ return(dycni->SubclassOf(name, super)); }

static dyt dycLookupMethodSig(dycObject obj, char *name, char *sig);
static dyt dycGetMethodFunc(dycClass inf, dycMethod idx);
static dyt dycGetObjMethodFunc(dycObject obj, dycMethod idx);
static void dycSetMethodFunc(dycClass inf, dycMethod idx, dyt fcn);
static void dycSetObjMethodFunc(dycObject obj, dycMethod idx, dyt fcn);
static dycSlot dycAddObjSlot(dycObject obj, char *name, char *sig);
static dycMethod dycAddObjMethod(dycObject obj, char *name, char *sig);
static dycObject dycCloneObject(dycObject obj);
static dycObject dycGetObjDelegate(dycObject obj, char *name);
static void dycSetObjDelegate(dycObject obj, char *name, dycObject obj1);
static dycSlot *dycEnumSlotArray(dycClass inf);
static dycMethod *dycEnumMethodArray(dycClass inf);
static dycSlot *dycEnumClassSlotArray(char *name);
static dycMethod *dycEnumClassMethodArray(char *name);
static dycObject dycAllocClass(char *name);
static void *dycPtrClassSlot(char *cname, char *sname);
static dycSlot dycIdxClassSlot(char *cname, char *sname);
static dycMethod dycIdxClassMethod(char *cname, char *name, char *sig);
static dyt dycGetSlot(dycObject obj, char *name);
static dyt dycGetSlotr(dycObject obj, char *name);
static void *dycGetSlotp(dycObject obj, char *name);
static int dycGetSlotsb(dycObject obj, char *name);
static int dycGetSlotub(dycObject obj, char *name);
static int dycGetSlotss(dycObject obj, char *name);
static int dycGetSlotus(dycObject obj, char *name);
static int dycGetSloti(dycObject obj, char *name);
static s64 dycGetSlotl(dycObject obj, char *name);
static float dycGetSlotf(dycObject obj, char *name);
static double dycGetSlotd(dycObject obj, char *name);
static int dycSetSlot(dycObject obj, char *name, dyt val);
static int dycSetSlotr(dycObject obj, char *name, dyt val);
static int dycSetSlotp(dycObject obj, char *name, void *val);
static int dycSetSlotsb(dycObject obj, char *name, int val);
static int dycSetSlotub(dycObject obj, char *name, int val);
static int dycSetSlotss(dycObject obj, char *name, int val);
static int dycSetSlotus(dycObject obj, char *name, int val);
static int dycSetSloti(dycObject obj, char *name, int val);
static int dycSetSlotl(dycObject obj, char *name, s64 val);
static int dycSetSlotf(dycObject obj, char *name, float val);
static int dycSetSlotd(dycObject obj, char *name, double val);
static int dycGetsb(dycObject obj, dycSlot idx);
static int dycGetub(dycObject obj, dycSlot idx);
static int dycGetss(dycObject obj, dycSlot idx);
static int dycGetus(dycObject obj, dycSlot idx);
static int dycGeti(dycObject obj, dycSlot idx);
static s64 dycGetl(dycObject obj, dycSlot idx);
static float dycGetf(dycObject obj, dycSlot idx);
static double dycGetd(dycObject obj, dycSlot idx);
static dyt dycGetr(dycObject obj, dycSlot idx);
static void *dycGetp(dycObject obj, dycSlot idx);
static void dycSetsb(dycObject obj, dycSlot idx, int val);
static void dycSetub(dycObject obj, dycSlot idx, int val);
static void dycSetss(dycObject obj, dycSlot idx, int val);
static void dycSetus(dycObject obj, dycSlot idx, int val);
static void dycSeti(dycObject obj, dycSlot idx, int val);
static void dycSetl(dycObject obj, dycSlot idx, s64 val);
static void dycSetf(dycObject obj, dycSlot idx, float val);
static void dycSetd(dycObject obj, dycSlot idx, double val);
static void dycSetr(dycObject obj, dycSlot idx, dyt val);
static void dycSetp(dycObject obj, dycSlot idx, void *val);
static int dycGetStaticsb(dycClass inf, dycSlot idx);
static int dycGetStaticub(dycClass inf, dycSlot idx);
static int dycGetStaticss(dycClass inf, dycSlot idx);
static int dycGetStaticus(dycClass inf, dycSlot idx);
static int dycGetStatici(dycClass inf, dycSlot idx);
static s64 dycGetStaticl(dycClass inf, dycSlot idx);
static float dycGetStaticf(dycClass inf, dycSlot idx);
static double dycGetStaticd(dycClass inf, dycSlot idx);
static dyt dycGetStaticr(dycClass inf, dycSlot idx);
static void *dycGetStaticp(dycClass inf, dycSlot idx);
static void dycSetStaticsb(dycClass inf, dycSlot idx, int val);
static void dycSetStaticub(dycClass inf, dycSlot idx, int val);
static void dycSetStaticss(dycClass inf, dycSlot idx, int val);
static void dycSetStaticus(dycClass inf, dycSlot idx, int val);
static void dycSetStatici(dycClass inf, dycSlot idx, int val);
static void dycSetStaticl(dycClass inf, dycSlot idx, s64 val);
static void dycSetStaticf(dycClass inf, dycSlot idx, float val);
static void dycSetStaticd(dycClass inf, dycSlot idx, double val);
static void dycSetStaticr(dycClass inf, dycSlot idx, dyt val);
static void dycSetStaticp(dycClass inf, dycSlot idx, void *val);
static int dycGetInitsb(dycClass inf, dycSlot idx);
static int dycGetInitub(dycClass inf, dycSlot idx);
static int dycGetInitss(dycClass inf, dycSlot idx);
static int dycGetInitus(dycClass inf, dycSlot idx);
static int dycGetIniti(dycClass inf, dycSlot idx);
static s64 dycGetInitl(dycClass inf, dycSlot idx);
static float dycGetInitf(dycClass inf, dycSlot idx);
static double dycGetInitd(dycClass inf, dycSlot idx);
static dyt dycGetInitr(dycClass inf, dycSlot idx);
static void *dycGetInitp(dycClass inf, dycSlot idx);
static void dycSetInitsb(dycClass inf, dycSlot idx, int val);
static void dycSetInitub(dycClass inf, dycSlot idx, int val);
static void dycSetInitss(dycClass inf, dycSlot idx, int val);
static void dycSetInitus(dycClass inf, dycSlot idx, int val);
static void dycSetIniti(dycClass inf, dycSlot idx, int val);
static void dycSetInitl(dycClass inf, dycSlot idx, s64 val);
static void dycSetInitf(dycClass inf, dycSlot idx, float val);
static void dycSetInitd(dycClass inf, dycSlot idx, double val);
static void dycSetInitr(dycClass inf, dycSlot idx, dyt val);
static void dycSetInitp(dycClass inf, dycSlot idx, void *val);
static int dycGetStructsb(dycClassV vinf, void *obj, dycSlot idx);
static int dycGetStructub(dycClassV vinf, void *obj, dycSlot idx);
static int dycGetStructss(dycClassV vinf, void *obj, dycSlot idx);
static int dycGetStructus(dycClassV vinf, void *obj, dycSlot idx);
static int dycGetStructi(dycClassV vinf, void *obj, dycSlot idx);
static s64 dycGetStructl(dycClassV vinf, void *obj, dycSlot idx);
static float dycGetStructf(dycClassV vinf, void *obj, dycSlot idx);
static double dycGetStructd(dycClassV vinf, void *obj, dycSlot idx);
static dyt dycGetStructr(dycClassV vinf, void *obj, dycSlot idx);
static void *dycGetStructp(dycClassV vinf, void *obj, dycSlot idx);
static void dycSetStructsb(dycClassV vinf, void *obj, dycSlot idx, int val);
static void dycSetStructub(dycClassV vinf, void *obj, dycSlot idx, int val);
static void dycSetStructss(dycClassV vinf, void *obj, dycSlot idx, int val);
static void dycSetStructus(dycClassV vinf, void *obj, dycSlot idx, int val);
static void dycSetStructi(dycClassV vinf, void *obj, dycSlot idx, int val);
static void dycSetStructl(dycClassV vinf, void *obj, dycSlot idx, s64 val);
static void dycSetStructf(dycClassV vinf, void *obj, dycSlot idx, float val);
static void dycSetStructd(dycClassV vinf, void *obj, dycSlot idx, double val);
static void dycSetStructr(dycClassV vinf, void *obj, dycSlot idx, dyt val);
static void dycSetStructp(dycClassV vinf, void *obj, dycSlot idx, void *val);
static dycObject dycNewClass(char *name);
static dycObject dycNewInitClass(char *name, char *sig, ...);
static dycObject dycNewInitClassV(char *name, char *sig, va_list args);
static dycObject dycNewInitClassU(char *name, char *sig, void **args);
static dycObject dycNew(dycClass inf);
static dycObject dycNewInit(dycClass inf, char *sig, ...);
static dycObject dycNewInitV(dycClass inf, char *sig, va_list args);
static dycObject dycNewInitU(dycClass inf, char *sig, void **args);
static void *dycCallSigU(dycObject obj, char *name, char *sig, void **args);
static void *dycCallSigV(dycObject obj, char *name, char *sig, va_list args);
static void *dycCallSig(dycObject obj, char *name, char *sig, ...);
static dyt dycCallSigVr(dycObject obj, char *name, char *sig, va_list lst);
static int dycCallSigVi(dycObject obj, char *name, char *sig, va_list lst);
static s64 dycCallSigVl(dycObject obj, char *name, char *sig, va_list lst);
static float dycCallSigVf(dycObject obj, char *name, char *sig, va_list lst);
static double dycCallSigVd(dycObject obj, char *name, char *sig, va_list lst);
static dyt dycCallSigUr(dycObject obj, char *name, char *sig, void **args);
static int dycCallSigUi(dycObject obj, char *name, char *sig, void **args);
static s64 dycCallSigUl(dycObject obj, char *name, char *sig, void **args);
static float dycCallSigUf(dycObject obj, char *name, char *sig, void **args);
static double dycCallSigUd(dycObject obj, char *name, char *sig, void **args);
static int dycCallSigi(dycObject obj, char *name, char *sig, ...);
static s64 dycCallSigl(dycObject obj, char *name, char *sig, ...);
static float dycCallSigf(dycObject obj, char *name, char *sig, ...);
static double dycCallSigd(dycObject obj, char *name, char *sig, ...);
static void *dycCallU(dycObject obj, dycMethod mth, void **args);
static void *dycCallV(dycObject obj, dycMethod mth, va_list args);
static void *dycCall(dycObject obj, dycMethod mth, ...);
static int dycCallUi(dycObject obj, dycMethod mth, void **args);
static s64 dycCallUl(dycObject obj, dycMethod mth, void **args);
static float dycCallUf(dycObject obj, dycMethod mth, void **args);
static double dycCallUd(dycObject obj, dycMethod mth, void **args);
static dyt dycCallUr(dycObject obj, dycMethod mth, void **args);
static void *dycCallUp(dycObject obj, dycMethod mth, void **args);
static void dycCallUv(dycObject obj, dycMethod mth, void **args);
static int dycCallVi(dycObject obj, dycMethod mth, va_list args);
static s64 dycCallVl(dycObject obj, dycMethod mth, va_list args);
static float dycCallVf(dycObject obj, dycMethod mth, va_list args);
static double dycCallVd(dycObject obj, dycMethod mth, va_list args);
static dyt dycCallVr(dycObject obj, dycMethod mth, va_list args);
static void *dycCallVp(dycObject obj, dycMethod mth, va_list args);
static void dycCallVv(dycObject obj, dycMethod mth, va_list args);
static int dycCalli(dycObject obj, dycMethod mth, ...);
static s64 dycCalll(dycObject obj, dycMethod mth, ...);
static float dycCallf(dycObject obj, dycMethod mth, ...);
static double dycCalld(dycObject obj, dycMethod mth, ...);
static dyt dycCallr(dycObject obj, dycMethod mth, ...);
static void *dycCallp(dycObject obj, dycMethod mth, ...);
static void dycCallv(dycObject obj, dycMethod mth, ...);
static void *dycCallStaticU(dycClass inf, dycMethod mth, void **args);
static void *dycCallStaticV(dycClass inf, dycMethod mth, va_list args);
static void *dycCallStatic(dycClass inf, dycMethod mth, ...);
static int dycCallStaticUi(dycClass inf, dycMethod mth, void **args);
static s64 dycCallStaticUl(dycClass inf, dycMethod mth, void **args);
static float dycCallStaticUf(dycClass inf, dycMethod mth, void **args);
static double dycCallStaticUd(dycClass inf, dycMethod mth, void **args);
static dyt dycCallStaticUr(dycClass inf, dycMethod mth, void **args);
static void *dycCallStaticUp(dycClass inf, dycMethod mth, void **args);
static void dycCallStaticUv(dycClass inf, dycMethod mth, void **args);
static int dycCallStaticVi(dycClass inf, dycMethod mth, va_list args);
static s64 dycCallStaticVl(dycClass inf, dycMethod mth, va_list args);
static float dycCallStaticVf(dycClass inf, dycMethod mth, va_list args);
static double dycCallStaticVd(dycClass inf, dycMethod mth, va_list args);
static dyt dycCallStaticVr(dycClass inf, dycMethod mth, va_list args);
static void *dycCallStaticVp(dycClass inf, dycMethod mth, va_list args);
static void dycCallStaticVv(dycClass inf, dycMethod mth, va_list args);
static int dycCallStatici(dycClass inf, dycMethod mth, ...);
static s64 dycCallStaticl(dycClass inf, dycMethod mth, ...);
static float dycCallStaticf(dycClass inf, dycMethod mth, ...);
static double dycCallStaticd(dycClass inf, dycMethod mth, ...);
static dyt dycCallStaticr(dycClass inf, dycMethod mth, ...);
static void *dycCallStaticp(dycClass inf, dycMethod mth, ...);
static void dycCallStaticv(dycClass inf, dycMethod mth, ...);
static void *dycCallAsU(dycObject obj, dycClass inf, dycMethod mth, void **args);
static void *dycCallAsV(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static void *dycCallAs(dycObject obj, dycClass inf, dycMethod mth, ...);
static int dycCallAsUi(dycObject obj, dycClass inf, dycMethod mth, void **args);
static s64 dycCallAsUl(dycObject obj, dycClass inf, dycMethod mth, void **args);
static float dycCallAsUf(dycObject obj, dycClass inf, dycMethod mth, void **args);
static double dycCallAsUd(dycObject obj, dycClass inf, dycMethod mth, void **args);
static dyt dycCallAsUr(dycObject obj, dycClass inf, dycMethod mth, void **args);
static void *dycCallAsUp(dycObject obj, dycClass inf, dycMethod mth, void **args);
static void dycCallAsUv(dycObject obj, dycClass inf, dycMethod mth, void **args);
static int dycCallAsVi(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static s64 dycCallAsVl(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static float dycCallAsVf(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static double dycCallAsVd(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static dyt dycCallAsVr(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static void *dycCallAsVp(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static void dycCallAsVv(dycObject obj, dycClass inf, dycMethod mth, va_list args);
static int dycCallAsi(dycObject obj, dycClass inf, dycMethod mth, ...);
static s64 dycCallAsl(dycObject obj, dycClass inf, dycMethod mth, ...);
static float dycCallAsf(dycObject obj, dycClass inf, dycMethod mth, ...);
static double dycCallAsd(dycObject obj, dycClass inf, dycMethod mth, ...);
static dyt dycCallAsr(dycObject obj, dycClass inf, dycMethod mth, ...);
static void *dycCallAsp(dycObject obj, dycClass inf, dycMethod mth, ...);
static void dycCallAsv(dycObject obj, dycClass inf, dycMethod mth, ...);
static void *dycCallStructU(void *obj, dycClassV vinf, dycMethod mth, void **args);
static void *dycCallStructV(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static void *dycCallStruct(void *obj, dycClassV vinf, dycMethod mth, ...);
static int dycCallStructUi(void *obj, dycClassV vinf, dycMethod mth, void **args);
static s64 dycCallStructUl(void *obj, dycClassV vinf, dycMethod mth, void **args);
static float dycCallStructUf(void *obj, dycClassV vinf, dycMethod mth, void **args);
static double dycCallStructUd(void *obj, dycClassV vinf, dycMethod mth, void **args);
static dyt dycCallStructUr(void *obj, dycClassV vinf, dycMethod mth, void **args);
static void *dycCallStructUp(void *obj, dycClassV vinf, dycMethod mth, void **args);
static void dycCallStructUv(void *obj, dycClassV vinf, dycMethod mth, void **args);
static int dycCallStructVi(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static s64 dycCallStructVl(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static float dycCallStructVf(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static double dycCallStructVd(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static dyt dycCallStructVr(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static void *dycCallStructVp(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static void dycCallStructVv(void *obj, dycClassV vinf, dycMethod mth, va_list args);
static int dycCallStructi(void *obj, dycClassV vinf, dycMethod mth, ...);
static s64 dycCallStructl(void *obj, dycClassV vinf, dycMethod mth, ...);
static float dycCallStructf(void *obj, dycClassV vinf, dycMethod mth, ...);
static double dycCallStructd(void *obj, dycClassV vinf, dycMethod mth, ...);
static dyt dycCallStructr(void *obj, dycClassV vinf, dycMethod mth, ...);
static void *dycCallStructp(void *obj, dycClassV vinf, dycMethod mth, ...);
static void dycCallStructv(void *obj, dycClassV vinf, dycMethod mth, ...);
//base/dyc_array.c
static dycArray dycNewArray(char *sig, int cnt);
static int dycArrayP(dyt p);
static int dycArraySigP(dyt p, char *sig);
static void *dycArrayV(dycArray arr);
static int dycArraySz(dycArray arr);
static char *dycArraySig(dycArray arr);
static int dycArrayStep(dycArray arr);
static void *dycArrayIdxPtr(dycArray arr, int idx);
static int dycGetArraysb(dycArray arr, int idx);
static int dycGetArrayub(dycArray arr, int idx);
static int dycGetArrayss(dycArray arr, int idx);
static int dycGetArrayus(dycArray arr, int idx);
static int dycGetArrayi(dycArray arr, int idx);
static s64 dycGetArrayl(dycArray arr, int idx);
static float dycGetArrayf(dycArray arr, int idx);
static double dycGetArrayd(dycArray arr, int idx);
static dyt dycGetArrayr(dycArray arr, int idx);
static void *dycGetArrayp(dycArray arr, int idx);
static void dycSetArraysb(dycArray arr, int idx, int val);
static void dycSetArrayub(dycArray arr, int idx, int val);
static void dycSetArrayss(dycArray arr, int idx, int val);
static void dycSetArrayus(dycArray arr, int idx, int val);
static void dycSetArrayi(dycArray arr, int idx, int val);
static void dycSetArrayl(dycArray arr, int idx, s64 val);
static void dycSetArrayf(dycArray arr, int idx, float val);
static void dycSetArrayd(dycArray arr, int idx, double val);
static void dycSetArrayr(dycArray arr, int idx, dyt val);
static void dycSetArrayp(dycArray arr, int idx, void *val);
static void *dycArrayIdxPtrBC(dycArray arr, int idx);
static int dycGetArraySsb(dycArray arr, int idx);
static int dycGetArraySub(dycArray arr, int idx);
static int dycGetArraySss(dycArray arr, int idx);
static int dycGetArraySus(dycArray arr, int idx);
static int dycGetArraySi(dycArray arr, int idx);
static s64 dycGetArraySl(dycArray arr, int idx);
static float dycGetArraySf(dycArray arr, int idx);
static double dycGetArraySd(dycArray arr, int idx);
static dyt dycGetArraySr(dycArray arr, int idx);
static void *dycGetArraySp(dycArray arr, int idx);
static void dycSetArraySsb(dycArray arr, int idx, int val);
static void dycSetArraySub(dycArray arr, int idx, int val);
static void dycSetArraySss(dycArray arr, int idx, int val);
static void dycSetArraySus(dycArray arr, int idx, int val);
static void dycSetArraySi(dycArray arr, int idx, int val);
static void dycSetArraySl(dycArray arr, int idx, s64 val);
static void dycSetArraySf(dycArray arr, int idx, float val);
static void dycSetArraySd(dycArray arr, int idx, double val);
static void dycSetArraySr(dycArray arr, int idx, dyt val);
static void dycSetArraySp(dycArray arr, int idx, void *val);
static dycArray DYC_NewArrayM_R(char *sig, int ord, int *sz);
static dycArray dycNewArrayM(char *sig, int ord, int *sz);
static dycArray dycNewArrayM2(char *sig, int sz0, int sz1);
static dycArray dycNewArrayM3(char *sig, int sz0, int sz1, int sz2);
static dycArray dycNewArrayM4(char *sig, int sz0, int sz1, int sz2, int sz3);
static dycArray dycNewArrayM5(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4);
static dycArray dycNewArrayM6(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5);
static dycArray dycNewArrayM7(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6);
static dycArray dycNewArrayM8(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6, int sz7);
static dycArray dycNewArrayM9(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6, int sz7, int sz8);
static dycArray dycNewArrayM10(char *sig,int sz0, int sz1, int sz2, int sz3, int sz4, int sz5, int sz6, int sz7, int sz8, int sz9);
static dycArray dycNewArrayM11(char *sig,int sz0, int sz1, int sz2 , int sz3, int sz4, int sz5, int sz6 , int sz7, int sz8, int sz9, int sz10);
static dycArray dycNewArrayM12(char *sig,int sz0, int sz1, int sz2 , int sz3, int sz4, int sz5, int sz6 , int sz7, int sz8, int sz9, int sz10, int sz11);
static dycArrayN dycNewArrayN(char *sig, int ord, int *sz);
static int dycArrayNP(dyt p);
static int dycArrayNSigP(dyt p, char *sig);
static int dycArrayNSigOrdP(dyt p, char *sig, int ord);
static void *dycArrayNV(dycArrayN arr);
static int *dycArrayNSz(dycArrayN arr);
static int dycArrayNCnt(dycArrayN arr);
static int dycArrayNOrd(dycArrayN arr);
static char *dycArrayNSig(dycArrayN arr);
static int dycArrayNStep(dycArrayN arr);
static void *dycArrayNIdxPtr(dycArrayN arr, int *idx);
static void *dycArrayNIdxPtrBC(dycArrayN arr, int *idx);
static int dycGetArrayNsb(dycArrayN arr, int *idx);
static int dycGetArrayNub(dycArrayN arr, int *idx);
static int dycGetArrayNss(dycArrayN arr, int *idx);
static int dycGetArrayNus(dycArrayN arr, int *idx);
static int dycGetArrayNi(dycArrayN arr, int *idx);
static s64 dycGetArrayNl(dycArrayN arr, int *idx);
static float dycGetArrayNf(dycArrayN arr, int *idx);
static double dycGetArrayNd(dycArrayN arr, int *idx);
static dyt dycGetArrayNr(dycArrayN arr, int *idx);
static void *dycGetArrayNp(dycArrayN arr, int *idx);
static void dycSetArrayNsb(dycArrayN arr, int *idx, int val);
static void dycSetArrayNub(dycArrayN arr, int *idx, int val);
static void dycSetArrayNss(dycArrayN arr, int *idx, int val);
static void dycSetArrayNus(dycArrayN arr, int *idx, int val);
static void dycSetArrayNi(dycArrayN arr, int *idx, int val);
static void dycSetArrayNl(dycArrayN arr, int *idx, s64 val);
static void dycSetArrayNf(dycArrayN arr, int *idx, float val);
static void dycSetArrayNd(dycArrayN arr, int *idx, double val);
static void dycSetArrayNr(dycArrayN arr, int *idx, dyt val);
static void dycSetArrayNp(dycArrayN arr, int *idx, void *val);
static int dycGetArrayNSsb(dycArrayN arr, int *idx);
static int dycGetArrayNSub(dycArrayN arr, int *idx);
static int dycGetArrayNSss(dycArrayN arr, int *idx);
static int dycGetArrayNSus(dycArrayN arr, int *idx);
static int dycGetArrayNSi(dycArrayN arr, int *idx);
static s64 dycGetArrayNSl(dycArrayN arr, int *idx);
static float dycGetArrayNSf(dycArrayN arr, int *idx);
static double dycGetArrayNSd(dycArrayN arr, int *idx);
static dyt dycGetArrayNSr(dycArrayN arr, int *idx);
static void *dycGetArrayNSp(dycArrayN arr, int *idx);
static void dycSetArrayNSsb(dycArrayN arr, int *idx, int val);
static void dycSetArrayNSub(dycArrayN arr, int *idx, int val);
static void dycSetArrayNSss(dycArrayN arr, int *idx, int val);
static void dycSetArrayNSus(dycArrayN arr, int *idx, int val);
static void dycSetArrayNSi(dycArrayN arr, int *idx, int val);
static void dycSetArrayNSl(dycArrayN arr, int *idx, s64 val);
static void dycSetArrayNSf(dycArrayN arr, int *idx, float val);
static void dycSetArrayNSd(dycArrayN arr, int *idx, double val);
static void dycSetArrayNSr(dycArrayN arr, int *idx, dyt val);
static void dycSetArrayNSp(dycArrayN arr, int *idx, void *val);
static void *dycArray2IdxPtr(dycArrayN arr, int y, int x);
static void *dycArray2IdxPtrBC(dycArrayN arr, int y, int x);
static int dycGetArray2sb(dycArrayN arr, int y, int x);
static int dycGetArray2ub(dycArrayN arr, int y, int x);
static int dycGetArray2ss(dycArrayN arr, int y, int x);
static int dycGetArray2us(dycArrayN arr, int y, int x);
static int dycGetArray2i(dycArrayN arr, int y, int x);
static s64 dycGetArray2l(dycArrayN arr, int y, int x);
static float dycGetArray2f(dycArrayN arr, int y, int x);
static double dycGetArray2d(dycArrayN arr, int y, int x);
static dyt dycGetArray2r(dycArrayN arr, int y, int x);
static void *dycGetArray2p(dycArrayN arr, int y, int x);
static void dycSetArray2sb(dycArrayN arr, int y, int x, int val);
static void dycSetArray2ub(dycArrayN arr, int y, int x, int val);
static void dycSetArray2ss(dycArrayN arr, int y, int x, int val);
static void dycSetArray2us(dycArrayN arr, int y, int x, int val);
static void dycSetArray2i(dycArrayN arr, int y, int x, int val);
static void dycSetArray2l(dycArrayN arr, int y, int x, s64 val);
static void dycSetArray2f(dycArrayN arr, int y, int x, float val);
static void dycSetArray2d(dycArrayN arr, int y, int x, double val);
static void dycSetArray2r(dycArrayN arr, int y, int x, dyt val);
static void dycSetArray2p(dycArrayN arr, int y, int x, void *val);
static int dycGetArray2Ssb(dycArrayN arr, int y, int x);
static int dycGetArray2Sub(dycArrayN arr, int y, int x);
static int dycGetArray2Sss(dycArrayN arr, int y, int x);
static int dycGetArray2Sus(dycArrayN arr, int y, int x);
static int dycGetArray2Si(dycArrayN arr, int y, int x);
static s64 dycGetArray2Sl(dycArrayN arr, int y, int x);
static float dycGetArray2Sf(dycArrayN arr, int y, int x);
static double dycGetArray2Sd(dycArrayN arr, int y, int x);
static dyt dycGetArray2Sr(dycArrayN arr, int y, int x);
static void *dycGetArray2Sp(dycArrayN arr, int y, int x);
static void dycSetArray2Ssb(dycArrayN arr, int y, int x, int val);
static void dycSetArray2Sub(dycArrayN arr, int y, int x, int val);
static void dycSetArray2Sss(dycArrayN arr, int y, int x, int val);
static void dycSetArray2Sus(dycArrayN arr, int y, int x, int val);
static void dycSetArray2Si(dycArrayN arr, int y, int x, int val);
static void dycSetArray2Sl(dycArrayN arr, int y, int x, s64 val);
static void dycSetArray2Sf(dycArrayN arr, int y, int x, float val);
static void dycSetArray2Sd(dycArrayN arr, int y, int x, double val);
static void dycSetArray2Sr(dycArrayN arr, int y, int x, dyt val);
static void dycSetArray2Sp(dycArrayN arr, int y, int x, void *val);
static void *dycArray3IdxPtr(dycArrayN arr, int z, int y, int x);
static void *dycArray3IdxPtrBC(dycArrayN arr, int z, int y, int x);
static int dycGetArray3sb(dycArrayN arr, int z, int y, int x);
static int dycGetArray3ub(dycArrayN arr, int z, int y, int x);
static int dycGetArray3ss(dycArrayN arr, int z, int y, int x);
static int dycGetArray3us(dycArrayN arr, int z, int y, int x);
static int dycGetArray3i(dycArrayN arr, int z, int y, int x);
static s64 dycGetArray3l(dycArrayN arr, int z, int y, int x);
static float dycGetArray3f(dycArrayN arr, int z, int y, int x);
static double dycGetArray3d(dycArrayN arr, int z, int y, int x);
static dyt dycGetArray3r(dycArrayN arr, int z, int y, int x);
static void *dycGetArray3p(dycArrayN arr, int z, int y, int x);
static void dycSetArray3sb(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3ub(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3ss(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3us(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3i(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3l(dycArrayN arr, int z, int y, int x, s64 val);
static void dycSetArray3f(dycArrayN arr, int z, int y, int x, float val);
static void dycSetArray3d(dycArrayN arr, int z, int y, int x, double val);
static void dycSetArray3r(dycArrayN arr, int z, int y, int x, dyt val);
static void dycSetArray3p(dycArrayN arr, int z, int y, int x, void *val);
static int dycGetArray3Ssb(dycArrayN arr, int z, int y, int x);
static int dycGetArray3Sub(dycArrayN arr, int z, int y, int x);
static int dycGetArray3Sss(dycArrayN arr, int z, int y, int x);
static int dycGetArray3Sus(dycArrayN arr, int z, int y, int x);
static int dycGetArray3Si(dycArrayN arr, int z, int y, int x);
static s64 dycGetArray3Sl(dycArrayN arr, int z, int y, int x);
static float dycGetArray3Sf(dycArrayN arr, int z, int y, int x);
static double dycGetArray3Sd(dycArrayN arr, int z, int y, int x);
static dyt dycGetArray3Sr(dycArrayN arr, int z, int y, int x);
static void *dycGetArray3Sp(dycArrayN arr, int z, int y, int x);
static void dycSetArray3Ssb(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3Sub(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3Sss(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3Sus(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3Si(dycArrayN arr, int z, int y, int x, int val);
static void dycSetArray3Sl(dycArrayN arr, int z, int y, int x, s64 val);
static void dycSetArray3Sf(dycArrayN arr, int z, int y, int x, float val);
static void dycSetArray3Sd(dycArrayN arr, int z, int y, int x, double val);
static void dycSetArray3Sr(dycArrayN arr, int z, int y, int x, dyt val);
static void dycSetArray3Sp(dycArrayN arr, int z, int y, int x, void *val);

#endif
