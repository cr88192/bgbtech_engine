#ifndef BGBDYLLI_H
#define BGBDYLLI_H

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

#ifndef DYT_T
#define DYT_T
#ifdef _BGBMETA
typedef __variant dyt;
#else
#ifdef __BSCC
typedef void *dyt;
//typedef __variant dyt;
#else
typedef void *dyt;
#endif
#endif
#endif


typedef struct DYLL_Iface_s DYLL_Iface;
typedef struct DYO_Iface_s DYO_Iface;
typedef struct DY_Iface_s DY_Iface;

struct DYLL_Iface_s {
void *resv0;	//0
void *resv1;	//1
void *resv2;	//2
void *resv3;	//3

//getting and binding addresses
void *(*GetAddr)(char *sym);
char *(*GetAddrName)(void *ptr);
void (*SetAddr)(char *sym, void *ptr);

//getting and setting variables
void *(*GetPtr)(char *sym);
void (*SetPtr)(char *sym, void *v);
int (*GetInt)(char *sym);
void (*SetInt)(char *sym, int v);
s64 (*GetLong)(char *sym);
void (*SetLong)(char *sym, s64 v);
float (*GetFloat)(char *sym);
void (*SetFloat)(char *sym, float v);
double (*GetDouble)(char *sym);
void (*SetDouble)(char *sym, double v);

//get and set dynamically-typed variables
dyt (*GetDynamic)(char *sym);
void (*SetDynamic)(char *sym, dyt val);

//getting and setting env vars
dyt (*NewEnv)(dyt super);
void (*EnvBindAddr)(dyt env, char *name, void *addr);
void (*EnvBindAddrSig)(dyt env, char *name, char *sig, void *addr);
void (*EnvSetAddr)(dyt env, char *name, void *addr);
void *(*EnvGetAddr)(dyt env, char *name);
char *(*EnvGetSig)(dyt env, char *name);
void *(*EnvAllocVar)(dyt env, char *name, char *sig);
dyt (*ThunkEnv)(dyt fcn);
void (*ThunkBindAddr)(dyt fcn, char *name, void *addr);
void (*ThunkBindAddrSig)(dyt fcn, char *name, char *sig, void *addr);
void (*ThunkSetAddr)(dyt fcn, char *name, void *addr);
void *(*ThunkGetAddr)(dyt fcn, char *name);
char *(*ThunkGetSig)(dyt fcn, char *name);

//assembling stuff
dyt (*AsmThunk)(char *buf);
dyt (*AsmThunkEnv)(char *buf, dyt env);
dyt (*AsmThunkSuperEnv)(char *buf, dyt senv);
dyt (*AsmThunkInline)(char *buf);
dyt (*AsmThunkInlineEnv)(char *buf, dyt env);
dyt (*AsmThunkInlineSuperEnv)(char *buf, dyt senv);
void (*AsmBegin)();
void *(*AsmEnd)();
void (*AsmBeginThunkInline)();
void *(*AsmEndThunkInline)();
void (*AsmPuts)(char *str);
void (*AsmPrintv)(char *str, va_list lst);
void (*AsmPrint)(char *str, ...);

//register meta callback
int (*AsmRegisterLinkMeta)(char *name, void *(*fcn)(char *sym, char *name, char **args));
int (*AsmRegisterLinkNotify)(char *name, void (*fcn)(char *sym, char *name, char **args, void *ptr));

//compose meta-handler name
char *(*ComposeLinkMetaName)(char *name, char **args);
char *(*ComposeLinkNotifyName)(char *name, char **args);
char *(*ComposeLinkMetaName0)(char *name);
char *(*ComposeLinkMetaName1)(char *name, char *ar0);
char *(*ComposeLinkMetaName2)(char *name, char *ar0, char *ar1);
char *(*ComposeLinkMetaName3)(char *name, char *ar0, char *ar1, char *ar2);
char *(*ComposeLinkMetaName4)(char *name,char *ar0, char *ar1, char *ar2, char *ar3);

//get signatures
char *(*GetAddrSig)(void *fcn);
char *(*GetNameSig)(char *name);

//call functions via reflection
void *(*ApplyPtrV)(void *fcn, va_list lst);
void *(*ApplyPtrU)(void *fcn, void **args);
int (*ApplyPtrVi)(void *fcn, va_list lst);
s64 (*ApplyPtrVl)(void *fcn, va_list lst);
float (*ApplyPtrVf)(void *fcn, va_list lst);
double (*ApplyPtrVd)(void *fcn, va_list lst);
void *(*ApplyPtrVp)(void *fcn, va_list lst);
void *(*ApplyPtrObjV)(void *fcn, void *obj, va_list lst);
void *(*ApplyPtrObjU)(void *fcn, void *obj, void **args);
void *(*ApplyPtrSigV)(void *fcn, char *sig, va_list lst);
void *(*ApplyPtrSigU)(void *fcn, char *sig, void **args);
int (*ApplyPtrSigVi)(void *fcn, char *sig, va_list lst);
s64 (*ApplyPtrSigVl)(void *fcn, char *sig, va_list lst);
float (*ApplyPtrSigVf)(void *fcn, char *sig, va_list lst);
double (*ApplyPtrSigVd)(void *fcn, char *sig, va_list lst);
void *(*ApplyPtrSigVp)(void *fcn, char *sig, va_list lst);
void *(*ApplyPtrSigObjV)(void *fcn, char *sig,void *obj, va_list lst);
void *(*ApplyPtrSigObjU)(void *fcn, char *sig,void *obj, void **args);

void *(*WrapClosure)(void *fcn, void *data, char *sig);
int (*LoadLibrary)(char *name);

//meta database
void (*MetaInit)();
void (*MetaCommit)();
int (*MetaLoadDB)(char *name);
int (*MetaSaveDB)(char *name);
void (*MetaBindKey)(char *key, char *val);
char *(*MetaLookupKey)(char *key);

//get/set "objects" (AKA: interfaces)
void *(*GetObject)(char *name);
void (*SetObject)(char *name, void *value);

//some sig stuff
int (*SigQuickSize)(char *sig);
int (*SigAlignSize)(char *sig);
char *(*SigSizeAlign)(char *sig, int *rsz, int *ral);
char *(*SigNext)(char *sig);
int (*SigSizeListEsig)(char *sig, char **esig, int *algn);
int (*SigSizeList)(char *sig);

dyt (*GetVarSig)(void *var, char *sig);
void (*SetVarSig)(void *var, char *sig, dyt val);

char *(*SigRet)(char *sig);
int (*CheckCallSig)(char *fsig, char *sig);

//GenSym
char *(*GenSym)();

};



struct DYO_Iface_s {
void *resv0;	//0
void *resv1;	//1
void *resv2;	//2
void *resv3;	//3

//func

dyt (*RawFunc)(char *sig, void *fcn);
dyt (*RawClose)(char *sig, void *fcn, void *data);
dyt (*RawMethod)(char *sig, void *fcn);
dyt (*RawMethodData)(char *sig, void *fcn, void *data);
dyt (*RawStaticMethod)(char *sig, void *fcn);
dyt (*RawStaticMethodData)(char *sig, void *fcn, void *data);

int (*RawFuncP)(dyt obj);
char *(*GenSym)();
char *(*ToString)(dyt obj);

void *(*ConvObjCFunc)(dyt obj, char *sig);

//method

dyt (*ApplyMethod)(dyt obj, dyt mth, dyt *args, int nargs);
dyt (*ApplyMethodLst)(dyt obj, dyt mth, dyt args);
void *(*ApplyMethodU)(dyt obj, dyt mth,char *sig, void **args, int szargs);
void *(*ApplyMethodV)(dyt obj, dyt mth,char *sig, va_list args);

//basic OO
void (*TypeGetSlot)(char *ty, dyt (*fcn)(dyt, dyt));
void (*TypeSetSlot)(char *ty, dyt (*fcn)(dyt, dyt, dyt));
void (*TypeNextSlot)(char *ty, dyt (*fcn)(dyt, dyt));
void (*TypeTrySetSlot)(char *ty, dyt (*fcn)(dyt, dyt, dyt));
void (*TypeGetDelegate)(char *ty, dyt (*fcn)(dyt, dyt));
void (*TypeSetDelegate)(char *ty, dyt (*fcn)(dyt, dyt, dyt));
void (*TypeNextDelegate)(char *ty, dyt (*fcn)(dyt, dyt));
void (*TypeCallMethod)(char *ty, dyt (*fcn)(dyt, dyt, dyt *, int));
void (*TypeApply)(char *ty, dyt (*fcn)(dyt, dyt *, int));
void (*TypeApplyObj)(char *ty, dyt (*fcn)(dyt, dyt, dyt *, int));
void (*TypeToString)(char *ty, char *(*fcn)(dyt));

dyt (*Object)();
dyt (*ObjectParent)(dyt parent);
int (*Objectp)(dyt obj);
int (*ObjectP)(dyt obj);
dyt (*Bind)(dyt obj, char *var, dyt val);
dyt (*TrySet)(dyt obj, char *var, dyt val);
dyt (*Get)(dyt obj, char *sym);
int (*Geti)(dyt obj, char *sym);
s64 (*Getl)(dyt obj, char *sym);
double (*Getf)(dyt obj, char *sym);
double (*Getd)(dyt obj, char *sym);
void (*Get2fv)(dyt obj, char *sym, float *v);
void (*Get3fv)(dyt obj, char *sym, float *v);
void (*Get4fv)(dyt obj, char *sym, float *v);
void (*Get2dv)(dyt obj, char *sym, double *v);
void (*Get3dv)(dyt obj, char *sym, double *v);
void (*Get4dv)(dyt obj, char *sym, double *v);
char *(*Gets)(dyt obj, char *sym);
dyt (*Set)(dyt obj, char *sym, dyt val);
void (*Seti)(dyt obj, char *sym, int val);
void (*Setl)(dyt obj, char *sym, s64 val);
void (*Setf)(dyt obj, char *sym, double val);
void (*Setd)(dyt obj, char *sym, double val);
void (*Set2f)(dyt obj, char *sym, double x, double y);
void (*Set3f)(dyt obj, char *sym, double x, double y, double z);
void (*Set4f)(dyt obj, char *sym, double x, double y, double z, double w);
void (*Set2fv)(dyt obj, char *sym, float *v);
void (*Set3fv)(dyt obj, char *sym, float *v);
void (*Set4fv)(dyt obj, char *sym, float *v);
void (*Set2dv)(dyt obj, char *sym, double *v);
void (*Set3dv)(dyt obj, char *sym, double *v);
void (*Set4dv)(dyt obj, char *sym, double *v);
void (*Sets)(dyt obj, char *sym, char *val);
dyt (*GetIdx)(dyt obj, int idx);
dyt (*SetIdx)(dyt obj, int idx, dyt val);
dyt (*GetKey)(dyt obj, dyt key);
dyt (*SetKey)(dyt obj, dyt key, dyt val);
dyt (*GetDelegate)(dyt obj, char *sym);
void (*SetDelegate)(dyt obj, char *sym, dyt val);
dyt (*GetParent)(dyt obj);
void (*SetParent)(dyt obj, dyt val);
void (*BindMethod)(dyt obj, char *sym, dyt mth);
void (*DefMethod)(dyt obj, char *sym,dyt (*fcn)(dyt self, dyt *args, int nargs));
void (*DefMethodN)(dyt obj, char *sym, dyt (*fcn)(dyt self,dyt rest));
dyt (*Call)(dyt obj, char *sym, dyt *args, int nargs);
dyt (*CallN)(dyt obj, char *sym, dyt args);
dyt (*CallSig)(dyt obj, char *sym, char *sig, ...);
dyt (*LookupMethodSig)(dyt obj, char *name, char *sig);
};


struct DY_Iface_s {
void *resv0;	//0
void *resv1;	//1
void *resv2;	//2
void *resv3;	//3

//base/dy_opr.c
dyt (*Add)(dyt a, dyt b);
dyt (*Sub)(dyt a, dyt b);
dyt (*Mul)(dyt a, dyt b);
dyt (*Div)(dyt a, dyt b);
dyt (*Mod)(dyt a, dyt b);
dyt (*And)(dyt a, dyt b);
dyt (*Or)(dyt a, dyt b);
dyt (*Xor)(dyt a, dyt b);
dyt (*Shl)(dyt a, dyt b);
dyt (*Shr)(dyt a, dyt b);
dyt (*Exp)(dyt a, dyt b);
dyt (*Neg)(dyt a);
dyt (*Not)(dyt a);
dyt (*LNot)(dyt a);
int (*Eqp)(dyt a, dyt b);
int (*NEqp)(dyt a, dyt b);
int (*Ltp)(dyt a, dyt b);
int (*Gtp)(dyt a, dyt b);
int (*Lep)(dyt a, dyt b);
int (*Gep)(dyt a, dyt b);
int (*Truep)(dyt a);
int (*Falsep)(dyt a);
dyt (*Sqr)(dyt a);
dyt (*Sqrt)(dyt a);
//base/(*_cons.c
dyt (*cons)(dyt a, dyt b);
int (*consp)(dyt p);
int (*listp)(dyt lst);
int (*listlen)(dyt lst);
void (*FreeCons)(dyt p);
void (*FreeList)(dyt p);
int (*FormIsP)(dyt lst, char *str);
dyt (*car)(dyt p);
dyt (*cdr)(dyt p);
void (*setcar)(dyt p, dyt v);
void (*setcdr)(dyt p, dyt v);
dyt (*reverse)(dyt lst);
dyt (*nreverse)(dyt lst);
dyt (*nappend)(dyt la, dyt lb);
dyt (*ListToArray)(dyt lst);
dyt (*listcopy)(dyt lst);
dyt (*append)(dyt la, dyt lb);
//base/(*_array.c
dyt (*array)(int cnt);
int (*arrayp)(dyt p);
dyt (*arrayidx)(dyt p, int idx);
void (*arraysetidx)(dyt p, int idx, dyt q);
dyt *(*arrayv)(dyt p);
int (*arraysz)(dyt p);
dyt (*WrapArray)(dyt *args, int cnt);
//base/(*_print.c
void (*PrintBuf)(dyt n, int ind,void (*pb)(void *p, char *b), void *ob);
void (*DumpFD)(FILE *fd, dyt n);
void (*PrintFD)(FILE *fd, dyt n);
void (*DumpGCP)(dyt n);
void (*PrintGCP)(dyt n);
char *(*DumpStr)(char *buf, dyt n);
char *(*PrintStr)(char *buf, dyt n);
void (*Print)(dyt n);
//base/(*s_parse.c
dyt (*sParseFromString)(char *s);
dyt (*sParseMultiFromString)(char *s);
//base/(*s_print.c
void (*sDumpFD)(FILE *fd, dyt n);
void (*sPrintFD)(FILE *fd, dyt n);
char *(*sDumpStr)(char *buf, dyt n);
char *(*sPrintStr)(char *buf, dyt n);
void (*sPrint)(dyt n);
void (*sPrintln)(dyt n);
//base/bgb(*_api.c
void (*dy_init)(void *base);
void (*dy_init2)(int (*fcn)());
void (*AddLogHook)(void (*fcn)(char *str));
void (*AddTypeHook)(char *(*fcn)(void *p));
char *(*gettype)(dyt p);
int (*typep)(dyt p, char *ty);
dyt (*dybool)(int v);
dyt (*dyint)(int v);
dyt (*dylong)(s64 v);
dyt (*dylonglong)(s64 v);
dyt (*dyflonum)(float v);
dyt (*dyfloat)(float v);
dyt (*dydouble)(double v);
dyt (*dycomplex2)(double r, double i);
dyt (*string)(char *s);
dyt (*symbol)(char *s);
dyt (*keyword)(char *s);
dyt (*sym)(char *s);
dyt (*ksym)(char *s);
dyt (*wstring)(u16 *s);
dyt (*wstring2)(char *str);
dyt (*ustring)(char *s);
dyt (*usym)(char *s);
int (*boolp)(dyt p);
int (*fixintp)(dyt p);
int (*intp)(dyt p);
int (*longp)(dyt p);
int (*longlongp)(dyt p);
int (*flonump)(dyt p);
int (*floatp)(dyt p);
int (*doublep)(dyt p);
int (*realp)(dyt p);
int (*fcomplexp)(dyt p);
int (*dcomplexp)(dyt p);
int (*complexp)(dyt p);
int (*stringp)(dyt p);
int (*symbolp)(dyt p);
int (*keywordp)(dyt p);
int (*wstringp)(dyt p);
int (*ustringp)(dyt p);
int (*usymp)(dyt p);
char *(*stringv)(dyt p);
char *(*symbolv)(dyt p);
u16 *(*wstringv)(dyt p);
int (*intv)(dyt p);
s64 (*longv)(dyt p);
s64 (*longlongv)(dyt p);
float (*floatv)(dyt p);
double (*doublev)(dyt p);
double (*realv)(dyt p);
double (*creal)(dyt p);
double (*cimag)(dyt p);
dyt (*dyvec2)(float x, float y);
dyt (*dyvec3)(float x, float y, float z);
dyt (*dyvec4)(float x, float y, float z, float w);
dyt (*dyvec2fv)(float *v);
dyt (*dyvec3fv)(float *v);
dyt (*dyvec4fv)(float *v);
dyt (*dyvec2dv)(double *v);
dyt (*dyvec3dv)(double *v);
dyt (*dyvec4dv)(double *v);
int (*dyvec2p)(dyt p);
int (*dyvec3p)(dyt p);
int (*dyvec4p)(dyt p);
void (*dyvecv2fv)(dyt p, float *v);
void (*dyvecv3fv)(dyt p, float *v);
void (*dyvecv4fv)(dyt p, float *v);
void (*dyvecv2dv)(dyt p, double *v);
void (*dyvecv3dv)(dyt p, double *v);
void (*dyvecv4dv)(dyt p, double *v);
dyt (*hash)(int cnt);
int (*hashp)(dyt p);
dyt (*hashget)(dyt p, char *str);
dyt (*hashset)(dyt obj, char *var, dyt val);

};

#endif
