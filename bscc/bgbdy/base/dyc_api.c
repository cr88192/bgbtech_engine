#include <bgbgc.h>
#include <bgbdy.h>

extern void *bgbdyc_mutex;
BGBDY_THREAD BGBGC_ClassInfo *dyc_curclass;

BGBDY_API int dycObjectP(dyt obj)	{ return(dytypep(obj, "_classobj_t")); }
BGBDY_API int dycClassP(dyt obj)	{ return(dytypep(obj, "_classinfo_t")); }
BGBDY_API int dycSlotP(dyt obj)		{ return(dytypep(obj, "_class_slot_t")); }
BGBDY_API int dycMethodP(dyt obj)	{ return(dytypep(obj, "_class_minf_t")); }

BGBDY_API int dycObjectInstanceOfP(dyt obj, char *name)
{
	if(!dytypep(obj, "_classobj_t"))return(0);
	return(dycInstanceOfP((dycObject)obj, name));
}

BGBDY_API void dycRegisterClassResolver(dycClass (*fn)(char *name))
	{ BGBDYC_RegisterClassResolver(fn); }

BGBDY_API dycClass dycBeginClass(char *name, char *super)
{
	BGBDYC_Init();
	if(super)dycGetClass(super);
	thLockFastMutex(bgbdyc_mutex);
	if(name)dyc_curclass=BGBDYC_CreateClass(name, super);
		else dyc_curclass=BGBDYC_CreateClassAnon2(super);
	thUnlockFastMutex(bgbdyc_mutex);
	return(dyc_curclass);
}

BGBDY_API dycClass dycEndClass()
{
	dycClass tmp;
	tmp=dyc_curclass;
	dyc_curclass=NULL;
//	thUnockFastMutex(bgbdyc_mutex);

	return(tmp);
}

BGBDY_API void dycBeginClassAnon(dycClass super)
{
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	dyc_curclass=BGBDYC_CreateClassAnon(super);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API dycClass dycEndClassAnon()
{
	dycClass tmp;
	tmp=dyc_curclass;
	dyc_curclass=NULL;
	return(tmp);
}

BGBDY_API void dycClassSuper(char *name)
{
	BGBGC_ClassInfo *inf;
	int i;

	thLockFastMutex(bgbdyc_mutex);
	inf=BGBDYC_FindClassQName(name);
	if(!inf)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return;
	}

	BGBDYC_InheritMulti(dyc_curclass, inf);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycAddClassFlags(s64 flags)
{
	dyc_curclass->flags|=flags;
}

BGBDY_API void dycSlotBasic(char *name, char *sig)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_AddSlot(dyc_curclass, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStatic(char *name, char *sig)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_AddSlotStatic(dyc_curclass, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotInit(char *name, char *sig)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_AddSlotInit(dyc_curclass, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStatici(char *name, char *sig, int val)
{
	BGBGC_ClassSlot *sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotStatic(dyc_curclass, name, sig);
	dyllSigSetPtrS64(sig, sinf->val, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStaticl(char *name, char *sig, s64 val)
{
	BGBGC_ClassSlot *sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotStatic(dyc_curclass, name, sig);
	dyllSigSetPtrS64(sig, sinf->val, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStaticf(char *name, char *sig, double val)
{
	BGBGC_ClassSlot *sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotStatic(dyc_curclass, name, sig);
	dyllSigSetPtrDouble(sig, sinf->val, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStaticd(char *name, char *sig, double val)
{
	BGBGC_ClassSlot *sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotStatic(dyc_curclass, name, sig);
	dyllSigSetPtrDouble(sig, sinf->val, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStaticU(char *name, char *sig, void *val)
{
	BGBGC_ClassSlot *sinf;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotStatic(dyc_curclass, name, sig);
	memcpy(sinf->val, val, dyllSigQuickSize(sig));
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStaticr(char *name, char *sig, dyt val)
{
	BGBGC_ClassSlot *sinf;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotStatic(dyc_curclass, name, sig);
	dyllSetVarSig(sinf->val, sig, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotIniti(char *name, char *sig, int val)
	{ dycSlotInitl(name, sig, val); }
BGBDY_API void dycSlotInitl(char *name, char *sig, s64 val)
{
	BGBGC_ClassSlot *sinf;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotInit(dyc_curclass, name, sig);
	p=BGBDYC_GetInitIdxPtr(dyc_curclass, sinf);
	dyllSigSetPtrS64(sig, p, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotInitf(char *name, char *sig, double val)
	{ dycSlotInitd(name, sig, val); }
BGBDY_API void dycSlotInitd(char *name, char *sig, double val)
{
	BGBGC_ClassSlot *sinf;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotInit(dyc_curclass, name, sig);
	p=BGBDYC_GetInitIdxPtr(dyc_curclass, sinf);
	dyllSigSetPtrDouble(sig, p, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotInitU(char *name, char *sig, void *val)
{
	BGBGC_ClassSlot *sinf;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotInit(dyc_curclass, name, sig);
	p=BGBDYC_GetInitIdxPtr(dyc_curclass, sinf);
	memcpy(p, val, dyllSigQuickSize(sig));
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotInitr(char *name, char *sig, dyt val)
{
	BGBGC_ClassSlot *sinf;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotInit(dyc_curclass, name, sig);
	p=BGBDYC_GetInitIdxPtr(dyc_curclass, sinf);
	dyllSetVarSig(p, sig, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotDelegate(char *name, char *cname)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_AddSlotDelegate(dyc_curclass, name, cname);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStaticDelegate(char *name, char *cname)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_AddSlotStaticDelegate(dyc_curclass, name, cname);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotInitDelegater(char *name, char *sig, dyt val)
{
	BGBGC_ClassSlot *sinf;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotDelegate(dyc_curclass, name, sig);
	dyllSetVarSig(sinf->val, sig, val);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSlotStaticDelegater(char *name, char *sig, dyt val)
{
	BGBGC_ClassSlot *sinf;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_AddSlotStaticDelegate(dyc_curclass, name, sig);
	p=BGBDYC_GetInitIdxPtr(dyc_curclass, sinf);
	dyllSetVarSig(p, sig, val);
	thUnlockFastMutex(bgbdyc_mutex);
}


BGBDY_API void dycMethodBasic(char *name, char *sig)
{
	BGBGC_ClassMinf *minf;

	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_GetMethodSig(dyc_curclass, name, sig);
	BGBDYC_TouchMinfMethod(dyc_curclass, minf);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycMethodStatic(char *name, char *sig)
{
	BGBGC_ClassMinf *minf;

	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_GetMethodSig(dyc_curclass, name, sig);
	minf->flags|=BGBDYC_FLAG_STATIC;
	BGBDYC_TouchMinfMethod(dyc_curclass, minf);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycMethodFunc(char *name, char *sig, dyt fcn)
{
	BGBGC_ClassMinf *minf;

	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_GetMethodSig(dyc_curclass, name, sig);
	BGBDYC_TouchMinfMethod(dyc_curclass, minf);
	BGBDYC_SetStaticMinfFunc(dyc_curclass, minf, fcn);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycMethodStaticFunc(char *name, char *sig, dyt fcn)
{
	BGBGC_ClassMinf *minf;

	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_GetMethodSig(dyc_curclass, name, sig);
	minf->flags|=BGBDYC_FLAG_STATIC;
	BGBDYC_TouchMinfMethod(dyc_curclass, minf);
	BGBDYC_SetStaticMinfFunc(dyc_curclass, minf, fcn);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycMethodCFunc(char *name, char *sig, void *fcn)
	{ dycMethodFunc(name, sig, dyRawFunc(sig, fcn)); }
BGBDY_API void dycMethodCFuncData(char *name, char *sig, void *fcn, void *data)
	{ dycMethodFunc(name, sig, dyRawClose(sig, fcn, data)); }
BGBDY_API void dycMethodCSelf(char *name, char *sig, void *fcn)
	{ dycMethodFunc(name, sig, dyRawMethod(sig, fcn)); }
BGBDY_API void dycMethodCSelfData(char *name, char *sig, void *fcn, void *data)
	{ dycMethodFunc(name, sig, dyRawMethodData(sig, fcn, data)); }

BGBDY_API void dycMethodStaticCFunc(char *name, char *sig, void *fcn)
	{ dycMethodStaticFunc(name, sig, dyRawFunc(sig, fcn)); }
BGBDY_API void dycMethodStaticCFuncData(char *name, char *sig, void *fcn, void *data)
	{ dycMethodStaticFunc(name, sig, dyRawClose(sig, fcn, data)); }
BGBDY_API void dycMethodStaticCSelf(char *name, char *sig, void *fcn)
	{ dycMethodStaticFunc(name, sig, dyRawStaticMethod(sig, fcn)); }
BGBDY_API void dycMethodStaticCSelfData(char *name, char *sig, void *fcn, void *data)
	{ dycMethodStaticFunc(name, sig, dyRawStaticMethodData(sig, fcn, data)); }

BGBDY_API void dycSlotMethod(char *name, char *sig)
{
	BGBGC_ClassMinf *minf;
	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_AddSlotMethodSig(dyc_curclass, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycStaticSlotMethod(char *name, char *sig)
{
	BGBGC_ClassMinf *minf;
	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_AddStaticSlotMethodSig(dyc_curclass, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycAddMethodFlags(char *name, char *sig, s64 flags)
{
	BGBGC_ClassMinf *minf;
	thLockFastMutex(bgbdyc_mutex);
//	minf=BGBDYC_AddStaticSlotMethodSig(dyc_curclass, name, sig);
	minf=BGBDYC_LookupMethodSig(dyc_curclass, name, sig);
	if(minf)minf->flags|=flags;
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycAddMethodAccess(char *name, char *sig, dyAccessMode acc)
{
	BGBGC_ClassMinf *minf;
	thLockFastMutex(bgbdyc_mutex);
//	minf=BGBDYC_AddStaticSlotMethodSig(dyc_curclass, name, sig);
	minf=BGBDYC_LookupMethodSig(dyc_curclass, name, sig);
	if(minf)minf->access=acc;
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycAddSlotFlags(char *name, s64 flags)
{
	dycSlot sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_LookupSlot(dyc_curclass, name);
	if(sinf)sinf->flags|=flags;
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycAddSlotAccess(char *name, dyAccessMode acc)
{
	dycSlot sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_LookupSlot(dyc_curclass, name);
	if(sinf)sinf->access=acc;
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycClassInterface(char *name)
{
	dycClass inf;

	inf=BGBDYC_FindClassQName(name);
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_AddClassInterface(dyc_curclass, inf);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API dycClass dycBeginIface(char *name, char *super)
{
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	dyc_curclass=BGBDYC_CreateInterface(name, super);
	thUnlockFastMutex(bgbdyc_mutex);
	return(dyc_curclass);
}

BGBDY_API dycClass dycEndIface()
{
	dycClass tmp;
	tmp=dyc_curclass;
	dyc_curclass=NULL;
	return(tmp);
}

BGBDY_API void dycIfaceSuper(char *name)
{
	BGBGC_ClassInfo *inf;
	int i;

	thLockFastMutex(bgbdyc_mutex);
	inf=BGBDYC_FindClassQName(name);
	if(!inf)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return;
	}

	if(!dyc_curclass->misup)
		dyc_curclass->misup=gcalloc(16*sizeof(BGBGC_ClassInfo *));
	i=dyc_curclass->n_misup++;
	dyc_curclass->misup[i]=inf;
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycIfaceMethod(char *name, char *sig)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_GetIfaceMethodSig(dyc_curclass, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycIfaceSlot(char *name, char *sig)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_AddSlotIface(dyc_curclass, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API dycClass dycBeginStruct(char *name)
{
	BGBDYC_Init();
	thLockFastMutex(bgbdyc_mutex);
	dyc_curclass=BGBDYC_CreateStruct(name);
	thUnlockFastMutex(bgbdyc_mutex);
	return(dyc_curclass);
}

BGBDY_API dycClass dycEndStruct()
{
	dycClass tmp;
	tmp=dyc_curclass;
	dyc_curclass=NULL;
	return(tmp);
}

BGBDY_API dycClass dycLookupClass(char *name)
{
	dycClass inf;
//	thLockFastMutex(bgbdyc_mutex);
	inf=BGBDYC_FindClassQNameFl(name, 0);
//	thUnlockFastMutex(bgbdyc_mutex);
	return(inf);
}

BGBDY_API dycClass dycGetClass(char *name)
{
	dycClass inf;
//	thLockFastMutex(bgbdyc_mutex);
	inf=BGBDYC_FindClassQName(name);
//	thUnlockFastMutex(bgbdyc_mutex);
	return(inf);
}

BGBDY_API dycObject dycAlloc(dycClass inf)
{
	dycObject obj;
	if(!inf)return(NULL);
	thLockFastMutex(bgbdyc_mutex);
	obj=(dycObject)BGBDYC_CreateInstance(inf);
	thUnlockFastMutex(bgbdyc_mutex);
	return(obj);
}

BGBDY_API void dycFreeObject(dycObject obj)
{
	BGBDYC_FreeInstance(obj);
}

BGBDY_API void dycDeleteObject(dycObject obj)
{
	dycCallSig(obj, "<deinit>", "()v");
	BGBDYC_FreeInstance(obj);
}

BGBDY_API void dycSetObjectObject(dycObject dst, dycObject src)
{
	BGBDYC_SetInstance(dst, src);
}

BGBDY_API int dycSubclassP(dycClass info, dycClass super)
	{ return(BGBDYC_SubclassP(info, super)); }
BGBDY_API int dycInstanceP(dycObject obj, dycClass inf)
	{ return(BGBDYC_InstanceP(obj->info, inf)); }
//	{ return(BGBDYC_SubclassP(obj->info, inf)); }
//BGBDY_API int dycInstanceOfP(dycObject obj, char *name)
//	{ return(BGBDYC_InstanceP(obj->info, dycGetClass(name))); }
//	{ return(BGBDYC_SubclassP(obj->info, dycGetClass(name))); }

BGBDY_API int dycClassDynamicP(dycClass info)
	{ return(info->flags&BGBDYC_FLAG_DYNAMIC); }
BGBDY_API int dycObjectDynamicP(dycObject obj)
	{ return(obj->info->flags&BGBDYC_FLAG_DYNAMIC); }

BGBDY_API dycClass dycGetObjClass(dycObject obj)
	{ return(obj->info); }
BGBDY_API dycClass dycGetClassSuper(dycClass inf)
	{ return(inf->super); }
BGBDY_API char *dycGetClassName(dycClass inf)
	{ return(inf->qname); }
BGBDY_API char *dycGetObjClassName(dycObject obj)
	{ return(obj->info->qname); }
BGBDY_API s64 dycGetClassFlags(dycClass inf)
	{ return(inf->flags); }
BGBDY_API char *dycGetClassShortName(dycClass inf)
	{ return(inf->sname); }
BGBDY_API char *dycGetClassSpecialName(dycClass inf, char *name)
	{ return(BGBDYC_RemapNameSpecial(inf, name, NULL)); }

BGBDY_API char *dycGetObjClassShortName(dycObject obj)
	{ return(obj->info->sname); }

BGBDY_API dycClassV dycGetClassVersion(dycClass inf)
	{ return(inf->vinf[inf->ver]); }
BGBDY_API int dycGetStructSize(dycClassV vinf)
	{ return(vinf->length); }
BGBDY_API dycClass dycGetVersionClass(dycClassV vinf)
	{ return(vinf->info); }
BGBDY_API int dycGetStructSlotOffset(dycClassV vinf, dycSlot idx)
	{ return(BGBDYC_GetStructSlotOffset(vinf, idx)); }

BGBDY_API char *dycGetSlotName(dycSlot idx)
	{ return(idx->name); }
BGBDY_API char *dycGetSlotSig(dycSlot idx)
	{ return(idx->sig); }
BGBDY_API dycClass dycGetSlotClass(dycSlot idx)
	{ return(idx->info); }
BGBDY_API s64 dycGetSlotFlags(dycSlot idx)
	{ return(idx->flags); }

BGBDY_API char *dycGetMethodName(dycMethod idx)
	{ return(idx->name); }
BGBDY_API char *dycGetMethodSig(dycMethod idx)
	{ return(idx->sig); }
BGBDY_API dycClass dycGetMethodClass(dycMethod idx)
	{ return(idx->info); }
BGBDY_API s64 dycGetMethodFlags(dycMethod idx)
	{ return(idx->flags); }

BGBDY_API dycSlot dycIdxSlot(dycClass info, char *name)
{
	dycSlot sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_LookupSlot(info, name);
	thUnlockFastMutex(bgbdyc_mutex);
	return(sinf);
}

BGBDY_API dycSlot dycIdxObjSlot(dycObject obj, char *name)
{
	dycSlot sinf;
	thLockFastMutex(bgbdyc_mutex);
	sinf=BGBDYC_LookupObjectSlot(obj, name);
	thUnlockFastMutex(bgbdyc_mutex);
	return(sinf);
}

BGBDY_API dycMethod dycIdxMethod(dycClass inf, char *name, char *sig)
{
	dycMethod mth;
	thLockFastMutex(bgbdyc_mutex);
	mth=BGBDYC_LookupMethodSig(inf, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
	return(mth);
}

BGBDY_API dycMethod dycIdxObjMethod(dycObject obj, char *name, char *sig)
{
	dycMethod mth;
	thLockFastMutex(bgbdyc_mutex);
	mth=BGBDYC_LookupObjectMethodSig(obj, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
	return(mth);
}


BGBDY_API void *dycGetSlotPtr(dycObject obj, dycSlot idx)
	{ return(BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API void *dycGetInitPtr(dycClass inf, dycSlot idx)
	{ return(BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API void *dycGetStaticPtr(dycClass inf, dycSlot idx)
	{ return(idx->val); }

BGBDY_API void *dycGetStructPtr(dycClassV vinf, void *obj, dycSlot idx)
	{ return(BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }

BGBDY_API void *dycPtrSlot(dycObject obj, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupObjectSlot(obj, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return(NULL);
	}
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	return(p);
}

BGBDY_API void *dycPtrStaticSlot(dycClass info, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupSlot(info, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return(NULL);
	}
	p=idx->val;
	thUnlockFastMutex(bgbdyc_mutex);
	return(p);
}

BGBDY_API void *dycPtrInitSlot(dycClass info, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupSlot(info, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return(NULL);
	}
	p=BGBDYC_GetInitIdxPtr(info, idx);
	thUnlockFastMutex(bgbdyc_mutex);

	return(p);
}

BGBDY_API void *dycPtrStructSlot(dycClassV vinf, void *obj, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupSlot(vinf->info, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return(NULL);
	}
	p=BGBDYC_GetStructSlotIdxPtr(vinf, obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	return(p);
}

BGBDY_API int dycInstanceOfP(dycObject obj, char *name)
{
	dycClass inf; inf=dycGetClass(name); if(!inf)return(0);
	return(BGBDYC_InstanceP(obj->info, inf));
}

BGBDY_API int dycSubclassOfP(char *name, char *super)
{
	dycClass infa, infb;

	infa=dycGetClass(name); if(!infa)return(0);
	infb=dycGetClass(super); if(!infb)return(0);
	return(BGBDYC_InstanceP(infa, infb));
}

BGBDY_API dyt dycLookupMethodSig(dycObject obj, char *name, char *sig)
{
	BGBGC_ClassMinf *minf;
	dyt fcn;
	dyt p;

	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_LookupObjectMethodSig(obj, name, sig);
	if(!minf)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return(NULL);
	}

	fcn=BGBDYC_GetMinfFunc(obj, minf);
	thUnlockFastMutex(bgbdyc_mutex);
	return(fcn);
}

BGBDY_API dyt dycLookupStaticMethodSig(dycClass inf, char *name, char *sig)
{
	BGBGC_ClassMinf *minf;
	dyt fcn;

	thLockFastMutex(bgbdyc_mutex);
	minf=BGBDYC_LookupMethodSig(inf, name, sig);
	if(!minf)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		return(NULL);
	}

	fcn=BGBDYC_GetStaticMinfFunc(inf, minf);
	thUnlockFastMutex(bgbdyc_mutex);
	return(fcn);
}

BGBDY_API dyt dycGetMethodFunc(dycClass inf, dycMethod idx)
{
	dyt p;
	thLockFastMutex(bgbdyc_mutex);
	p=BGBDYC_GetStaticMinfFunc(inf, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	return(p);
}

BGBDY_API dyt dycGetObjMethodFunc(dycObject obj, dycMethod idx)
{
	dyt p;
	thLockFastMutex(bgbdyc_mutex);
	p=BGBDYC_GetMinfFunc(obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	return(p);
}

BGBDY_API void dycSetMethodFunc(dycClass inf, dycMethod idx, dyt fcn)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_TouchMinfMethod(inf, idx);
	BGBDYC_SetStaticMinfFunc(inf, idx, fcn);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API void dycSetObjMethodFunc(dycObject obj, dycMethod idx, dyt fcn)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_TouchMinfMethod(obj->info, idx);
	BGBDYC_SetMinfFunc(obj, idx, fcn);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API dycSlot dycAddObjSlot(dycObject obj, char *name, char *sig)
{
	dycSlot idx;
	thLockFastMutex(bgbdyc_mutex);
	if(!obj->proto)obj->proto=BGBDYC_AllocProtoObj();
	idx=BGBDYC_AddObjProtoSlot(obj->proto, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
	return(idx);
}

BGBDY_API dycMethod dycAddObjMethod(dycObject obj, char *name, char *sig)
{
	dycMethod idx;
	thLockFastMutex(bgbdyc_mutex);
	if(!obj->proto)obj->proto=BGBDYC_AllocProtoObj();
	idx=BGBDYC_AddObjProtoMethodSig(obj->proto, name, sig);
	thUnlockFastMutex(bgbdyc_mutex);
	return(idx);
}

BGBDY_API dycObject dycCloneObject(dycObject obj)
{
	dycObject tmp;

	thLockFastMutex(bgbdyc_mutex);
	tmp=BGBDYC_CloneInstance(obj);
	thUnlockFastMutex(bgbdyc_mutex);
	return(tmp);
}

BGBDY_API dycObject dycCopyObject(dycObject obj)
{
	dycObject tmp;
	dycMethod minf;

	minf=dycIdxMethodArray(obj->info, "<init>", (dyt *)(&obj), 1);
	if(minf)
	{
		//copy constructor
		tmp=dycAlloc(obj->info);
		dycCallArray(tmp, minf, (dyt *)(&obj), 1);
		return(tmp);
	}else
	{
		thLockFastMutex(bgbdyc_mutex);
		tmp=BGBDYC_CloneInstance(obj);
		thUnlockFastMutex(bgbdyc_mutex);
		return(tmp);
	}
}

BGBDY_API dycObject dycGetObjDelegate(dycObject obj, char *name)
{
	dycObject p;
	thLockFastMutex(bgbdyc_mutex);
	p=BGBDYC_GetObjectDelegate(obj, name);
	thUnlockFastMutex(bgbdyc_mutex);
	return(p);
}

BGBDY_API void dycSetObjDelegate(dycObject obj, char *name, dycObject obj1)
{
	thLockFastMutex(bgbdyc_mutex);
	BGBDYC_SetObjectDelegate(obj, name, obj1);
	thUnlockFastMutex(bgbdyc_mutex);
}

BGBDY_API dycSlot *dycEnumSlotArray(dycClass inf)
	{ return(BGBDYC_GetClassSlotArray(inf)); }
BGBDY_API dycMethod *dycEnumMethodArray(dycClass inf)
	{ return(BGBDYC_GetClassMinfArray(inf)); }
BGBDY_API dycSlot *dycEnumDelegateSlotArray(dycClass inf)
	{ return(BGBDYC_GetClassDelegateSlotArray(inf)); }

BGBDY_API dycSlot *dycEnumClassSlotArray(char *name)
	{ return(BGBDYC_GetClassSlotArray(dycGetClass(name))); }
BGBDY_API dycMethod *dycEnumClassMethodArray(char *name)
	{ return(BGBDYC_GetClassMinfArray(dycGetClass(name))); }
BGBDY_API dycSlot *dycEnumClassDelegateSlotArray(char *name)
	{ return(BGBDYC_GetClassDelegateSlotArray(dycGetClass(name))); }

BGBDY_API dycObject dycAllocClass(char *name)
	{ return(dycAlloc(dycGetClass(name))); }
BGBDY_API void *dycPtrClassSlot(char *cname, char *sname)
	{ return(dycPtrStaticSlot(dycGetClass(cname), sname)); }
BGBDY_API dycSlot dycIdxClassSlot(char *cname, char *sname)
	{ return(dycIdxSlot(dycGetClass(cname), sname)); }
BGBDY_API dycMethod dycIdxClassMethod(char *cname, char *name, char *sig)
	{ return(dycIdxMethod(dycGetClass(cname), name, sig)); }

BGBDY_API dyt dycGetSlot(dycObject obj, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupObjectSlot(obj, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		dySetStatus("badSlot");
		return(UNDEFINED);
	}
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	if(!p) { dySetStatus("badSlot"); return(UNDEFINED); }
	return(dyllGetVarSig(p, idx->sig));
}

BGBDY_API dyt dycGetSlotr(dycObject obj, char *name)
	{ return(dycGetSlot(obj, name)); }

BGBDY_API void *dycGetSlotp(dycObject obj, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupObjectSlot(obj, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		dySetStatus("badSlot");
		return(NULL);
	}
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	if(!p) { dySetStatus("badSlot"); return(NULL); }
	return(*(void **)p);
}

BGBDY_API int dycGetSlotsb(dycObject obj, char *name)
	{ return(dycGetSlotl(obj, name)); }
BGBDY_API int dycGetSlotub(dycObject obj, char *name)
	{ return(dycGetSlotl(obj, name)); }
BGBDY_API int dycGetSlotss(dycObject obj, char *name)
	{ return(dycGetSlotl(obj, name)); }
BGBDY_API int dycGetSlotus(dycObject obj, char *name)
	{ return(dycGetSlotl(obj, name)); }

BGBDY_API int dycGetSloti(dycObject obj, char *name)
	{ return(dycGetSlotl(obj, name)); }
BGBDY_API s64 dycGetSlotl(dycObject obj, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupObjectSlot(obj, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		dySetStatus("badSlot");
		return(0);
	}
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	if(!p) { dySetStatus("badSlot"); return(0); }
	return(dyllSigGetPtrS64(idx->sig, p));
}

BGBDY_API float dycGetSlotf(dycObject obj, char *name)
	{ return(dycGetSlotd(obj, name)); }
BGBDY_API double dycGetSlotd(dycObject obj, char *name)
{
	dycSlot idx;
	void *p;

	thLockFastMutex(bgbdyc_mutex);
	idx=BGBDYC_LookupObjectSlot(obj, name);
	if(!idx)
	{
		thUnlockFastMutex(bgbdyc_mutex);
		dySetStatus("badSlot");
		return(0);
	}
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	if(!p) { dySetStatus("badSlot"); return(0); }
	return(dyllSigGetPtrDouble(idx->sig, p));
}

BGBDY_API int dycSetSlot(dycObject obj, char *name, dyt val)
{
	dycSlot slot;
	void *p;
	slot=dycIdxObjSlot(obj, name);
	if(!slot) { dySetStatus("badSlot"); return(-1); }
	p=dycGetSlotPtr(obj, slot);
	if(!p) { dySetStatus("badSlot"); return(-1); }
	dyllSetVarSig(p, slot->sig, val);
	return(0);
}

BGBDY_API int dycSetSlotr(dycObject obj, char *name, dyt val)
	{ return(dycSetSlot(obj, name, val)); }

BGBDY_API int dycSetSlotp(dycObject obj, char *name, void *val)
{
	dycSlot slot;
	void *p;
	slot=dycIdxObjSlot(obj, name);
	if(!slot) { dySetStatus("badSlot"); return(-1); }
	p=dycGetSlotPtr(obj, slot);
	if(!p) { dySetStatus("badSlot"); return(-1); }
	*(void **)p=val;
	return(0);
}

BGBDY_API int dycSetSlotsb(dycObject obj, char *name, int val)
	{ return(dycSetSlotl(obj, name, val)); }
BGBDY_API int dycSetSlotub(dycObject obj, char *name, int val)
	{ return(dycSetSlotl(obj, name, val)); }
BGBDY_API int dycSetSlotss(dycObject obj, char *name, int val)
	{ return(dycSetSlotl(obj, name, val)); }
BGBDY_API int dycSetSlotus(dycObject obj, char *name, int val)
	{ return(dycSetSlotl(obj, name, val)); }

BGBDY_API int dycSetSloti(dycObject obj, char *name, int val)
	{ return(dycSetSlotl(obj, name, val)); }
BGBDY_API int dycSetSlotl(dycObject obj, char *name, s64 val)
{
	dycSlot slot;
	void *p;
	slot=dycIdxObjSlot(obj, name);
	if(!slot) { dySetStatus("badSlot"); return(-1); }
	p=dycGetSlotPtr(obj, slot);
	if(!p) { dySetStatus("badSlot"); return(-1); }
	dyllSigSetPtrS64(slot->sig, p, val);
	return(0);
}

BGBDY_API int dycSetSlotf(dycObject obj, char *name, float val)
	{ return(dycSetSlotd(obj, name, val)); }
BGBDY_API int dycSetSlotd(dycObject obj, char *name, double val)
{
	dycSlot slot;
	void *p;
	slot=dycIdxObjSlot(obj, name);
	if(!slot) { dySetStatus("badSlot"); return(-1); }
	p=dycGetSlotPtr(obj, slot);
	if(!p) { dySetStatus("badSlot"); return(-1); }
	dyllSigSetPtrDouble(slot->sig, p, val);
	return(0);
}

BGBDY_API dyt dycGet(dycObject obj, dycSlot idx)
{
	void *p;
	thLockFastMutex(bgbdyc_mutex);
	p=BGBDYC_GetObjectSlotIdxPtr(obj, idx);
	thUnlockFastMutex(bgbdyc_mutex);
	if(!p) { dySetStatus("badSlot"); return(NULL); }
	return(dyllGetVarSig(p, idx->sig));
}

BGBDY_API int dycSet(dycObject obj, dycSlot slot, dyt val)
{
	void *p;
//	p=dycGetSlotPtr(obj, slot);
	p=BGBDYC_GetObjectSlotIdxPtr(obj, slot);
	if(!p) { dySetStatus("badSlot"); return(-1); }
	dyllSetVarSig(p, slot->sig, val);
	return(0);
}

BGBDY_API dyt dycGetr(dycObject obj, dycSlot idx)
	{ return(dycGet(obj, idx)); }
BGBDY_API void dycSetr(dycObject obj, dycSlot idx, dyt val)
	{ dycSet(obj, idx, val); }

BGBDY_API int dycGetsb(dycObject obj, dycSlot idx)
	{ return(*(char *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycGetub(dycObject obj, dycSlot idx)
	{ return(*(byte *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycGetss(dycObject obj, dycSlot idx)
	{ return(*(short *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycGetus(dycObject obj, dycSlot idx)
	{ return(*(unsigned short *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycGeti(dycObject obj, dycSlot idx)
	{ return(*(int *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API s64 dycGetl(dycObject obj, dycSlot idx)
	{ return(*(s64 *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API float dycGetf(dycObject obj, dycSlot idx)
	{ return(*(float *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API double dycGetd(dycObject obj, dycSlot idx)
	{ return(*(double *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
//BGBDY_API dyt dycGetr(dycObject obj, dycSlot idx)
//	{ return(*(dyt *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }
BGBDY_API void *dycGetp(dycObject obj, dycSlot idx)
	{ return(*(void **)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)); }

BGBDY_API void dycSetsb(dycObject obj, dycSlot idx, int val)
	{ *(char *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSetub(dycObject obj, dycSlot idx, int val)
	{ *(byte *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSetss(dycObject obj, dycSlot idx, int val)
	{ *(short *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSetus(dycObject obj, dycSlot idx, int val)
	{ *(unsigned short *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSeti(dycObject obj, dycSlot idx, int val)
	{ *(int *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSetl(dycObject obj, dycSlot idx, s64 val)
	{ *(s64 *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSetf(dycObject obj, dycSlot idx, float val)
	{ *(float *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSetd(dycObject obj, dycSlot idx, double val)
	{ *(double *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
//BGBDY_API void dycSetr(dycObject obj, dycSlot idx, dyt val)
//	{ *(dyt *)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycSetp(dycObject obj, dycSlot idx, void *val)
	{ *(void **)BGBDYC_GetObjectSlotIdxPtrTH(obj, idx)=val; }

BGBDY_API dyt dycGetStaticSlot(dycClass inf, char *name)
{
	dycSlot slot;
	dyt t;
	void *p;
	slot=dycIdxSlot(inf, name);
	if(!slot) { dySetStatus("badSlot"); return(UNDEFINED); }
	p=dycGetStaticPtr(inf, slot);
	if(!p) { dySetStatus("badSlot"); return(UNDEFINED); }
	t=dyllGetVarSig(p, slot->sig);
	return(t);
}

BGBDY_API int dycSetStaticSlot(dycClass inf, char *name, dyt val)
{
	dycSlot slot;
	void *p;
	slot=dycIdxSlot(inf, name);
	if(!slot) { dySetStatus("badSlot"); return(-1); }
	p=dycGetStaticPtr(inf, slot);
	if(!p) { dySetStatus("badSlot"); return(-1); }
	dyllSetVarSig(p, slot->sig, val);
	return(0);
}

BGBDY_API int dycGetStaticsb(dycClass inf, dycSlot idx)
	{ return(*(char *)dycGetStaticPtr(inf, idx)); }
BGBDY_API int dycGetStaticub(dycClass inf, dycSlot idx)
	{ return(*(byte *)dycGetStaticPtr(inf, idx)); }
BGBDY_API int dycGetStaticss(dycClass inf, dycSlot idx)
	{ return(*(short *)dycGetStaticPtr(inf, idx)); }
BGBDY_API int dycGetStaticus(dycClass inf, dycSlot idx)
	{ return(*(unsigned short *)dycGetStaticPtr(inf, idx)); }
BGBDY_API int dycGetStatici(dycClass inf, dycSlot idx)
	{ return(*(int *)dycGetStaticPtr(inf, idx)); }
BGBDY_API s64 dycGetStaticl(dycClass inf, dycSlot idx)
	{ return(*(s64 *)dycGetStaticPtr(inf, idx)); }
BGBDY_API float dycGetStaticf(dycClass inf, dycSlot idx)
	{ return(*(float *)dycGetStaticPtr(inf, idx)); }
BGBDY_API double dycGetStaticd(dycClass inf, dycSlot idx)
	{ return(*(double *)dycGetStaticPtr(inf, idx)); }
BGBDY_API dyt dycGetStaticr(dycClass inf, dycSlot idx)
	{ return(*(dyt *)dycGetStaticPtr(inf, idx)); }
BGBDY_API void *dycGetStaticp(dycClass inf, dycSlot idx)
	{ return(*(void **)dycGetStaticPtr(inf, idx)); }

BGBDY_API void dycSetStaticsb(dycClass inf, dycSlot idx, int val)
	{ *(char *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticub(dycClass inf, dycSlot idx, int val)
	{ *(byte *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticss(dycClass inf, dycSlot idx, int val)
	{ *(short *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticus(dycClass inf, dycSlot idx, int val)
	{ *(unsigned short *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStatici(dycClass inf, dycSlot idx, int val)
	{ *(int *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticl(dycClass inf, dycSlot idx, s64 val)
	{ *(s64 *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticf(dycClass inf, dycSlot idx, float val)
	{ *(float *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticd(dycClass inf, dycSlot idx, double val)
	{ *(double *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticr(dycClass inf, dycSlot idx, dyt val)
	{ *(dyt *)dycGetStaticPtr(inf, idx)=val; }
BGBDY_API void dycSetStaticp(dycClass inf, dycSlot idx, void *val)
	{ *(void **)dycGetStaticPtr(inf, idx)=val; }

BGBDY_API int dycGetInitsb(dycClass inf, dycSlot idx)
	{ return(*(char *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API int dycGetInitub(dycClass inf, dycSlot idx)
	{ return(*(byte *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API int dycGetInitss(dycClass inf, dycSlot idx)
	{ return(*(short *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API int dycGetInitus(dycClass inf, dycSlot idx)
	{ return(*(unsigned short *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API int dycGetIniti(dycClass inf, dycSlot idx)
	{ return(*(int *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API s64 dycGetInitl(dycClass inf, dycSlot idx)
	{ return(*(s64 *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API float dycGetInitf(dycClass inf, dycSlot idx)
	{ return(*(float *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API double dycGetInitd(dycClass inf, dycSlot idx)
	{ return(*(double *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API dyt dycGetInitr(dycClass inf, dycSlot idx)
	{ return(*(dyt *)BGBDYC_GetInitIdxPtrTH(inf, idx)); }
BGBDY_API void *dycGetInitp(dycClass inf, dycSlot idx)
	{ return(*(void **)BGBDYC_GetInitIdxPtrTH(inf, idx)); }

BGBDY_API void dycSetInitsb(dycClass inf, dycSlot idx, int val)
	{ *(char *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitub(dycClass inf, dycSlot idx, int val)
	{ *(byte *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitss(dycClass inf, dycSlot idx, int val)
	{ *(short *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitus(dycClass inf, dycSlot idx, int val)
	{ *(unsigned short *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetIniti(dycClass inf, dycSlot idx, int val)
	{ *(int *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitl(dycClass inf, dycSlot idx, s64 val)
	{ *(s64 *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitf(dycClass inf, dycSlot idx, float val)
	{ *(float *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitd(dycClass inf, dycSlot idx, double val)
	{ *(double *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitr(dycClass inf, dycSlot idx, dyt val)
	{ *(dyt *)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }
BGBDY_API void dycSetInitp(dycClass inf, dycSlot idx, void *val)
	{ *(void **)BGBDYC_GetInitIdxPtrTH(inf, idx)=val; }

BGBDY_API int dycGetStructsb(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(char *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API int dycGetStructub(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(byte *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API int dycGetStructss(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(short *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API int dycGetStructus(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(u16 *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API int dycGetStructi(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(int *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API s64 dycGetStructl(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(s64 *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API float dycGetStructf(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(float *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API double dycGetStructd(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(double *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API dyt dycGetStructr(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(dyt *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }
BGBDY_API void *dycGetStructp(dycClassV vinf, void *obj, dycSlot idx)
	{ return(*(void **)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)); }

BGBDY_API void dycSetStructsb(dycClassV vinf, void *obj, dycSlot idx, int val)
	{ *(char *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructub(dycClassV vinf, void *obj, dycSlot idx, int val)
	{ *(byte *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructss(dycClassV vinf, void *obj, dycSlot idx, int val)
	{ *(short *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructus(dycClassV vinf, void *obj, dycSlot idx, int val)
	{ *(u16 *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructi(dycClassV vinf, void *obj, dycSlot idx, int val)
	{ *(int *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructl(dycClassV vinf, void *obj, dycSlot idx, s64 val)
	{ *(s64 *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructf(dycClassV vinf, void *obj, dycSlot idx, float val)
	{ *(float *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructd(dycClassV vinf, void *obj, dycSlot idx, double val)
	{ *(double *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructr(dycClassV vinf, void *obj, dycSlot idx, dyt val)
	{ *(dyt *)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }
BGBDY_API void dycSetStructp(dycClassV vinf, void *obj, dycSlot idx, void *val)
	{ *(void **)BGBDYC_GetStructSlotIdxPtrTH(vinf, obj, idx)=val; }


BGBDY_API dycObject dycNewClass(char *name)
{
	return(dycNewInitClass(name, "()v"));
}

BGBDY_API dycObject dycNewInitClass(char *name, char *sig, ...)
{
	va_list args;
	dycObject tmp;

	tmp=dycAllocClass(name);
	if(!tmp)return(tmp);
	va_start(args, sig);
	dycCallSigV(tmp, "<init>", sig, args);
	va_end(args);
	return(tmp);
}

BGBDY_API dycObject dycNewInitClassV(char *name, char *sig, va_list args)
{
	dycObject tmp;

	tmp=dycAllocClass(name);
	if(!tmp)return(tmp);
	dycCallSigV(tmp, "<init>", sig, args);
	return(tmp);
}

BGBDY_API dycObject dycNewInitClassU(char *name, char *sig, void **args)
{
	dycObject tmp;

	tmp=dycAllocClass(name);
	if(!tmp)return(tmp);
	dycCallSigU(tmp, "<init>", sig, args);
	return(tmp);
}

BGBDY_API dycObject dycNew(dycClass inf)
{
	return(dycNewInit(inf, "()v"));
}

BGBDY_API dycObject dycNewInit(dycClass inf, char *sig, ...)
{
	va_list args;
	dycObject tmp;

	tmp=dycAlloc(inf);
	if(!tmp)return(tmp);
	va_start(args, sig);
	dycCallSigV(tmp, "<init>", sig, args);
	va_end(args);
	return(tmp);
}

BGBDY_API dycObject dycNewInitV(dycClass inf, char *sig, va_list args)
{
	dycObject tmp;

	tmp=dycAlloc(inf);
	if(!tmp)return(tmp);
	dycCallSigV(tmp, "<init>", sig, args);
	return(tmp);
}

BGBDY_API dycObject dycNewInitU(dycClass inf, char *sig, void **args)
{
	dycObject tmp;

	tmp=dycAlloc(inf);
	if(!tmp)return(tmp);
	dycCallSigU(tmp, "<init>", sig, args);
	return(tmp);
}

BGBDY_API void *dycCallSigU(dycObject obj, char *name, char *sig, void **args)
{
	BGBGC_ClassMinf *minf;
	BGBDYC_ApplyU_t apply;
	dyt fcn;
	void *p;

	minf=dycIdxObjMethod(obj, name, sig);
	if(!minf) { dySetStatus("badMethod"); return(NULL); }

//	fcn=BGBDYC_GetMinfFunc(obj, minf);
	fcn=BGBDYC_GetMinfFunc3U(obj, minf, &apply);
	if(apply)return(apply(obj, fcn, sig, args, -1));
	p=dycApplyMethodU(obj, fcn, sig, args, -1);
	return(p);
}

BGBDY_API void *dycCallSigV(dycObject obj, char *name, char *sig, va_list args)
{
	BGBGC_ClassMinf *minf;
	BGBDYC_ApplyV_t apply;
	dyt fcn;
	void *p;
	int i;

	minf=dycIdxObjMethod(obj, name, sig);
	if(!minf) { dySetStatus("badMethod"); return(NULL); }
//	fcn=BGBDYC_GetMinfFunc(obj, minf);
	fcn=BGBDYC_GetMinfFunc3V(obj, minf, &apply);
	if(apply)return(apply(obj, fcn, sig, args));
	p=dycApplyMethodV(obj, fcn, sig, args);
	return(p);
}

BGBDY_API void *dycCallSig(dycObject obj, char *name, char *sig, ...)
{
	va_list args;
	void *p;

	va_start(args, sig);
	p=dycCallSigV(obj, name, sig, args);
	va_end(args);
	return(p);
}

BGBDY_API dyt dycCallSigVr(dycObject obj, char *name, char *sig, va_list lst)
{
	void *pv;
	char *s;
	dyt v;

	pv=dycCallSigV(obj, name, sig, lst);
	s=dyllSigEsig(sig);
	v=dyllExtractVariantBufferSig(s, pv);
	return(v);
}
BGBDY_API dyt dycCallSigUr(dycObject obj, char *name, char *sig, void **args)
{
	void *pv;
	char *s;
	dyt v;

	pv=dycCallSigU(obj, name, sig, args);
	s=dyllSigEsig(sig);
	v=dyllExtractVariantBufferSig(s, pv);
	return(v);
}

BGBDY_API void *dycCallStaticSigV(dycClass inf, char *name, char *sig, va_list args)
{
	BGBGC_ClassMinf *minf;
//	BGBDYC_ApplyV_t apply;
	dyt fcn;
	void *p;
	int i;

	minf=dycIdxMethod(inf, name, sig);
	if(!minf) { dySetStatus("badMethod"); return(NULL); }
	fcn=BGBDYC_GetStaticMinfFunc(inf, minf);
//	fcn=BGBDYC_GetStaticMinfFunc3V(inf, minf, &apply);
//	if(apply)return(apply(obj, fcn, sig, args));
	p=dycApplyStaticV(inf, fcn, sig, args);
	return(p);
}

BGBDY_API void *dycCallStaticSig(dycClass inf, char *name, char *sig, ...)
{
	va_list args;
	void *p;

	va_start(args, sig);
	p=dycCallStaticSigV(inf, name, sig, args);
	va_end(args);
	return(p);
}

BGBDY_API dyt dycCallUr(dycObject obj, dycMethod mth, void **args)
{
	void *pv;
	char *s;
	dyt v;

	pv=dycCallU(obj, mth, args);
	s=dyllSigEsig(mth->sig);
	v=dyllExtractVariantBufferSig(s, pv);
	return(v);
}

BGBDY_API dyt dycCallVr(dycObject obj, dycMethod mth, va_list args)
{
	void *pv;
	char *s;
	dyt v;

	pv=dycCallV(obj, mth, args);
	s=dyllSigEsig(mth->sig);
	v=dyllExtractVariantBufferSig(s, pv);
	return(v);
}

BGBDY_API dyt dycCallStaticUr(dycClass inf, dycMethod mth, void **args)
{
	void *pv;
	char *s;
	dyt v;

	pv=dycCallStaticU(inf, mth, args);
	s=dyllSigEsig(mth->sig);
	v=dyllExtractVariantBufferSig(s, pv);
	return(v);
}

BGBDY_API dyt dycCallStaticVr(dycClass inf, dycMethod mth, va_list args)
{
	void *pv;
	char *s;
	dyt v;

	pv=dycCallStaticV(inf, mth, args);
	s=dyllSigEsig(mth->sig);
	v=dyllExtractVariantBufferSig(s, pv);
	return(v);
}

BGBDY_API int dycCallSigVi(dycObject obj, char *name, char *sig, va_list lst)
	{ return(*(int *)dycCallSigV(obj, name, sig, lst)); }
BGBDY_API s64 dycCallSigVl(dycObject obj, char *name, char *sig, va_list lst)
	{ return(*(s64 *)dycCallSigV(obj, name, sig, lst)); }
BGBDY_API float dycCallSigVf(dycObject obj, char *name, char *sig, va_list lst)
	{ return(*(float *)dycCallSigV(obj, name, sig, lst)); }

BGBDY_API double dycCallSigVd(dycObject obj, char *name, char *sig, va_list lst)
{
	void *p;
	p=dycCallSigV(obj, name, sig, lst);
	return(*(double *)p);
}

BGBDY_API int dycCallSigUi(dycObject obj, char *name, char *sig, void **args)
	{ return(*(int *)dycCallSigU(obj, name, sig, args)); }
BGBDY_API s64 dycCallSigUl(dycObject obj, char *name, char *sig, void **args)
	{ return(*(s64 *)dycCallSigU(obj, name, sig, args)); }
BGBDY_API float dycCallSigUf(dycObject obj, char *name, char *sig, void **args)
	{ return(*(float *)dycCallSigU(obj, name, sig, args)); }
BGBDY_API double dycCallSigUd(dycObject obj, char *name, char *sig, void **args)
	{ return(*(double *)dycCallSigU(obj, name, sig, args)); }

BGBDY_API int dycCallSigi(dycObject obj, char *name, char *sig, ...)
{
	va_list args;
	int v;

	va_start(args, sig);
	v=dycCallSigVi(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API s64 dycCallSigl(dycObject obj, char *name, char *sig, ...)
{
	va_list args;
	s64 v;

	va_start(args, sig);
	v=dycCallSigVl(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API float dycCallSigf(dycObject obj, char *name, char *sig, ...)
{
	va_list args;
	float v;

	va_start(args, sig);
	v=dycCallSigVf(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API double dycCallSigd(dycObject obj, char *name, char *sig, ...)
{
	va_list args;
	double v;

	va_start(args, sig);
	v=dycCallSigVd(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API dyt dycCallSigr(dycObject obj, char *name, char *sig, ...)
{
	va_list args;
	dyt v;

	va_start(args, sig);
	v=dycCallSigVr(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API void *dycCallU(dycObject obj, dycMethod mth, void **args)
{
	BGBDYC_ApplyU_t apply;
	dyt fcn;
	void *p;

//	fcn=BGBDYC_GetMinfFunc(obj, mth);
	fcn=BGBDYC_GetMinfFunc3U(obj, mth, &apply);
	if(apply)return(apply(obj, fcn, mth->sig, args, -1));
	p=dycApplyMethodU(obj, fcn, mth->sig, args, -1);
	return(p);

}

BGBDY_API void *dycCallV(dycObject obj, dycMethod mth, va_list args)
{
	BGBDYC_ApplyV_t apply;
	dyt fcn;
	void *p;
	int i;

//	fcn=BGBDYC_GetMinfFunc(obj, mth);
	fcn=BGBDYC_GetMinfFunc3V(obj, mth, &apply);
	if(apply)return(apply(obj, fcn, mth->sig, args));
	p=dycApplyMethodV(obj, fcn, mth->sig, args);
	return(p);
}

BGBDY_API void *dycCall(dycObject obj, dycMethod mth, ...)
{
	va_list args;
	BGBDYC_ApplyV_t apply;
	dyt fcn;
	void *p;
	int i;

	va_start(args, mth);
//	fcn=BGBDYC_GetMinfFunc(obj, mth);
	fcn=BGBDYC_GetMinfFunc3V(obj, mth, &apply);
	if(apply)return(apply(obj, fcn, mth->sig, args));
	p=dycApplyMethodV(obj, fcn, mth->sig, args);
	va_end(args);
	return(p);
}

BGBDY_API int dycCallUi(dycObject obj, dycMethod mth, void **args)
	{ return(*(int *)dycCallU(obj, mth, args)); }
BGBDY_API s64 dycCallUl(dycObject obj, dycMethod mth, void **args)
	{ return(*(s64 *)dycCallU(obj, mth, args)); }
BGBDY_API float dycCallUf(dycObject obj, dycMethod mth, void **args)
	{ return(*(float *)dycCallU(obj, mth, args)); }
BGBDY_API double dycCallUd(dycObject obj, dycMethod mth, void **args)
	{ return(*(double *)dycCallU(obj, mth, args)); }
BGBDY_API void *dycCallUp(dycObject obj, dycMethod mth, void **args)
	{ return(*(void **)dycCallU(obj, mth, args)); }
BGBDY_API void dycCallUv(dycObject obj, dycMethod mth, void **args)
	{ dycCallU(obj, mth, args); }

BGBDY_API int dycCallVi(dycObject obj, dycMethod mth, va_list args)
	{ return(*(int *)dycCallV(obj, mth, args)); }
BGBDY_API s64 dycCallVl(dycObject obj, dycMethod mth, va_list args)
	{ return(*(s64 *)dycCallV(obj, mth, args)); }
BGBDY_API float dycCallVf(dycObject obj, dycMethod mth, va_list args)
	{ return(*(float *)dycCallV(obj, mth, args)); }
BGBDY_API double dycCallVd(dycObject obj, dycMethod mth, va_list args)
	{ return(*(double *)dycCallV(obj, mth, args)); }
BGBDY_API void *dycCallVp(dycObject obj, dycMethod mth, va_list args)
	{ return(*(void **)dycCallV(obj, mth, args)); }
BGBDY_API void dycCallVv(dycObject obj, dycMethod mth, va_list args)
	{ dycCallV(obj, mth, args); }

BGBDY_API int dycCalli(dycObject obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallV(obj, mth, args); va_end(args);
	return(*(int *)p);
}

BGBDY_API s64 dycCalll(dycObject obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallV(obj, mth, args); va_end(args);
	return(*(s64 *)p);
}

BGBDY_API float dycCallf(dycObject obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallV(obj, mth, args); va_end(args);
	return(*(float *)p);
}

BGBDY_API double dycCalld(dycObject obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallV(obj, mth, args); va_end(args);
	return(*(double *)p);
}

BGBDY_API void *dycCallp(dycObject obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallV(obj, mth, args); va_end(args);
	return(*(void **)p);
}

BGBDY_API void dycCallv(dycObject obj, dycMethod mth, ...)
{
	va_list args;
	va_start(args, mth); dycCallV(obj, mth, args); va_end(args);
}

BGBDY_API dyt dycCallr(dycObject obj, dycMethod mth, ...)
{
	va_list args; dyt v;
	va_start(args, mth); v=dycCallVr(obj, mth, args); va_end(args);
	return(v);
}


BGBDY_API void *dycCallStaticU(dycClass inf, dycMethod mth, void **args)
{
	dyt fcn;
	void *p;

	fcn=BGBDYC_GetStaticMinfFunc(inf, mth);
	p=dycApplyStaticU(inf, fcn, mth->sig, args, -1);
	return(p);
}

BGBDY_API void *dycCallStaticV(dycClass inf, dycMethod mth, va_list args)
{
	dyt fcn;
	void *p;
	int i;

	fcn=BGBDYC_GetStaticMinfFunc(inf, mth);
	p=dycApplyStaticV(inf, fcn, mth->sig, args);
	return(p);
}

BGBDY_API void *dycCallStatic(dycClass inf, dycMethod mth, ...)
{
	va_list args;
	dyt fcn;
	void *p;
	int i;

	va_start(args, mth);
	fcn=BGBDYC_GetStaticMinfFunc(inf, mth);
	p=dycApplyStaticV(inf, fcn, mth->sig, args);
	va_end(args);
	return(p);
}

BGBDY_API int dycCallStaticUi(dycClass inf, dycMethod mth, void **args)
	{ return(*(int *)dycCallStaticU(inf, mth, args)); }
BGBDY_API s64 dycCallStaticUl(dycClass inf, dycMethod mth, void **args)
	{ return(*(s64 *)dycCallStaticU(inf, mth, args)); }
BGBDY_API float dycCallStaticUf(dycClass inf, dycMethod mth, void **args)
	{ return(*(float *)dycCallStaticU(inf, mth, args)); }
BGBDY_API double dycCallStaticUd(dycClass inf, dycMethod mth, void **args)
	{ return(*(double *)dycCallStaticU(inf, mth, args)); }
BGBDY_API void *dycCallStaticUp(dycClass inf, dycMethod mth, void **args)
	{ return(*(void **)dycCallStaticU(inf, mth, args)); }
BGBDY_API void dycCallStaticUv(dycClass inf, dycMethod mth, void **args)
	{ dycCallStaticU(inf, mth, args); }

BGBDY_API int dycCallStaticVi(dycClass inf, dycMethod mth, va_list args)
	{ return(*(int *)dycCallStaticV(inf, mth, args)); }
BGBDY_API s64 dycCallStaticVl(dycClass inf, dycMethod mth, va_list args)
	{ return(*(s64 *)dycCallStaticV(inf, mth, args)); }
BGBDY_API float dycCallStaticVf(dycClass inf, dycMethod mth, va_list args)
	{ return(*(float *)dycCallStaticV(inf, mth, args)); }
BGBDY_API double dycCallStaticVd(dycClass inf, dycMethod mth, va_list args)
	{ return(*(double *)dycCallStaticV(inf, mth, args)); }
BGBDY_API void *dycCallStaticVp(dycClass inf, dycMethod mth, va_list args)
	{ return(*(void **)dycCallStaticV(inf, mth, args)); }
BGBDY_API void dycCallStaticVv(dycClass inf, dycMethod mth, va_list args)
	{ dycCallStaticV(inf, mth, args); }

BGBDY_API int dycCallStatici(dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStaticV(inf, mth, args); va_end(args);
	return(*(int *)p);
}

BGBDY_API s64 dycCallStaticl(dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStaticV(inf, mth, args); va_end(args);
	return(*(s64 *)p);
}

BGBDY_API float dycCallStaticf(dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStaticV(inf, mth, args); va_end(args);
	return(*(float *)p);
}

BGBDY_API double dycCallStaticd(dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStaticV(inf, mth, args); va_end(args);
	return(*(double *)p);
}

BGBDY_API void *dycCallStaticp(dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStaticV(inf, mth, args); va_end(args);
	return(*(void **)p);
}

BGBDY_API void dycCallStaticv(dycClass inf, dycMethod mth, ...)
{
	va_list args;
	va_start(args, mth); dycCallStaticV(inf, mth, args); va_end(args);
}

BGBDY_API dyt dycCallStaticr(dycClass inf, dycMethod mth, ...)
{
	va_list args; dyt v;
	va_start(args, mth); v=dycCallStaticVr(inf, mth, args); va_end(args);
	return(v);
}

BGBDY_API void *dycCallAsU(dycObject obj, dycClass inf, dycMethod mth, void **args)
{
	dyt fcn;
	void *p;

	fcn=BGBDYC_GetStaticMinfFunc(inf, mth);
	p=dycApplyMethodU(obj, fcn, mth->sig, args, -1);
	return(p);
}

BGBDY_API void *dycCallAsV(dycObject obj, dycClass inf, dycMethod mth, va_list args)
{
	dyt fcn;
	void *p;
	int i;

	fcn=BGBDYC_GetStaticMinfFunc(inf, mth);
	p=dycApplyMethodV(obj, fcn, mth->sig, args);
	return(p);
}

BGBDY_API void *dycCallAs(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args;
	dyt fcn;
	void *p;
	int i;

	va_start(args, mth);
	fcn=BGBDYC_GetStaticMinfFunc(inf, mth);
	p=dycApplyMethodV(obj, fcn, mth->sig, args);
	va_end(args);
	return(p);
}

BGBDY_API int dycCallAsUi(dycObject obj, dycClass inf, dycMethod mth, void **args)
	{ return(*(int *)dycCallAsU(obj, inf, mth, args)); }
BGBDY_API s64 dycCallAsUl(dycObject obj, dycClass inf, dycMethod mth, void **args)
	{ return(*(s64 *)dycCallAsU(obj, inf, mth, args)); }
BGBDY_API float dycCallAsUf(dycObject obj, dycClass inf, dycMethod mth, void **args)
	{ return(*(float *)dycCallAsU(obj, inf, mth, args)); }
BGBDY_API double dycCallAsUd(dycObject obj, dycClass inf, dycMethod mth, void **args)
	{ return(*(double *)dycCallAsU(obj, inf, mth, args)); }
BGBDY_API dyt dycCallAsUr(dycObject obj, dycClass inf, dycMethod mth, void **args)
	{ return(*(dyt *)dycCallAsU(obj, inf, mth, args)); }
BGBDY_API void *dycCallAsUp(dycObject obj, dycClass inf, dycMethod mth, void **args)
	{ return(*(void **)dycCallAsU(obj, inf, mth, args)); }
BGBDY_API void dycCallAsUv(dycObject obj, dycClass inf, dycMethod mth, void **args)
	{ dycCallAsU(obj, inf, mth, args); }

BGBDY_API int dycCallAsVi(dycObject obj, dycClass inf, dycMethod mth, va_list args)
	{ return(*(int *)dycCallAsV(obj, inf, mth, args)); }
BGBDY_API s64 dycCallAsVl(dycObject obj, dycClass inf, dycMethod mth, va_list args)
	{ return(*(s64 *)dycCallAsV(obj, inf, mth, args)); }
BGBDY_API float dycCallAsVf(dycObject obj, dycClass inf, dycMethod mth, va_list args)
	{ return(*(float *)dycCallAsV(obj, inf, mth, args)); }
BGBDY_API double dycCallAsVd(dycObject obj, dycClass inf, dycMethod mth, va_list args)
	{ return(*(double *)dycCallAsV(obj, inf, mth, args)); }
BGBDY_API dyt dycCallAsVr(dycObject obj, dycClass inf, dycMethod mth, va_list args)
	{ return(*(dyt *)dycCallAsV(obj, inf, mth, args)); }
BGBDY_API void *dycCallAsVp(dycObject obj, dycClass inf, dycMethod mth, va_list args)
	{ return(*(void **)dycCallAsV(obj, inf, mth, args)); }
BGBDY_API void dycCallAsVv(dycObject obj, dycClass inf, dycMethod mth, va_list args)
	{ dycCallAsV(obj, inf, mth, args); }

BGBDY_API int dycCallAsi(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallAsV(obj, inf, mth, args); va_end(args);
	return(*(int *)p);
}

BGBDY_API s64 dycCallAsl(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallAsV(obj, inf, mth, args); va_end(args);
	return(*(s64 *)p);
}

BGBDY_API float dycCallAsf(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallAsV(obj, inf, mth, args); va_end(args);
	return(*(float *)p);
}

BGBDY_API double dycCallAsd(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallAsV(obj, inf, mth, args); va_end(args);
	return(*(double *)p);
}

BGBDY_API dyt dycCallAsr(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallAsV(obj, inf, mth, args); va_end(args);
	return(*(dyt *)p);
}

BGBDY_API void *dycCallAsp(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallAsV(obj, inf, mth, args); va_end(args);
	return(*(void **)p);
}

BGBDY_API void dycCallAsv(dycObject obj, dycClass inf, dycMethod mth, ...)
{
	va_list args;
	va_start(args, mth); dycCallAsV(obj, inf, mth, args); va_end(args);
}

BGBDY_API void *dycCallStructU(void *obj, dycClassV vinf, dycMethod mth, void **args)
{
	dyt fcn;
	void *p;

	fcn=BGBDYC_GetStructMinfFunc(vinf, mth);
	p=dycApplyStructMethodU(obj, fcn, mth->sig, args, -1);
	return(p);
}

BGBDY_API void *dycCallStructV(void *obj, dycClassV vinf, dycMethod mth, va_list args)
{
	dyt fcn;
	void *p;
	int i;

	fcn=BGBDYC_GetStructMinfFunc(vinf, mth);
	p=dycApplyStructMethodV(obj, fcn, mth->sig, args);
	return(p);
}

BGBDY_API void *dycCallStruct(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args;
	dyt fcn;
	void *p;
	int i;

	va_start(args, mth);
	fcn=BGBDYC_GetStructMinfFunc(vinf, mth);
	p=dycApplyStructMethodV(obj, fcn, mth->sig, args);
	va_end(args);
	return(p);
}

BGBDY_API int dycCallStructUi(void *obj, dycClassV vinf, dycMethod mth, void **args)
	{ return(*(int *)dycCallStructU(obj, vinf, mth, args)); }
BGBDY_API s64 dycCallStructUl(void *obj, dycClassV vinf, dycMethod mth, void **args)
	{ return(*(s64 *)dycCallStructU(obj, vinf, mth, args)); }
BGBDY_API float dycCallStructUf(void *obj, dycClassV vinf, dycMethod mth, void **args)
	{ return(*(float *)dycCallStructU(obj, vinf, mth, args)); }
BGBDY_API double dycCallStructUd(void *obj, dycClassV vinf, dycMethod mth, void **args)
	{ return(*(double *)dycCallStructU(obj, vinf, mth, args)); }
BGBDY_API dyt dycCallStructUr(void *obj, dycClassV vinf, dycMethod mth, void **args)
	{ return(*(dyt *)dycCallStructU(obj, vinf, mth, args)); }
BGBDY_API void *dycCallStructUp(void *obj, dycClassV vinf, dycMethod mth, void **args)
	{ return(*(void **)dycCallStructU(obj, vinf, mth, args)); }
BGBDY_API void dycCallStructUv(void *obj, dycClassV vinf, dycMethod mth, void **args)
	{ dycCallStructU(obj, vinf, mth, args); }

BGBDY_API int dycCallStructVi(void *obj, dycClassV vinf, dycMethod mth, va_list args)
	{ return(*(int *)dycCallStructV(obj, vinf, mth, args)); }
BGBDY_API s64 dycCallStructVl(void *obj, dycClassV vinf, dycMethod mth, va_list args)
	{ return(*(s64 *)dycCallStructV(obj, vinf, mth, args)); }
BGBDY_API float dycCallStructVf(void *obj, dycClassV vinf, dycMethod mth, va_list args)
	{ return(*(float *)dycCallStructV(obj, vinf, mth, args)); }
BGBDY_API double dycCallStructVd(void *obj, dycClassV vinf, dycMethod mth, va_list args)
	{ return(*(double *)dycCallStructV(obj, vinf, mth, args)); }
BGBDY_API dyt dycCallStructVr(void *obj, dycClassV vinf, dycMethod mth, va_list args)
	{ return(*(dyt *)dycCallStructV(obj, vinf, mth, args)); }
BGBDY_API void *dycCallStructVp(void *obj, dycClassV vinf, dycMethod mth, va_list args)
	{ return(*(void **)dycCallStructV(obj, vinf, mth, args)); }
BGBDY_API void dycCallStructVv(void *obj, dycClassV vinf, dycMethod mth, va_list args)
	{ dycCallStructV(obj, vinf, mth, args); }

BGBDY_API int dycCallStructi(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStructV(obj, vinf, mth, args); va_end(args);
	return(*(int *)p);
}

BGBDY_API s64 dycCallStructl(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStructV(obj, vinf, mth, args); va_end(args);
	return(*(s64 *)p);
}

BGBDY_API float dycCallStructf(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStructV(obj, vinf, mth, args); va_end(args);
	return(*(float *)p);
}

BGBDY_API double dycCallStructd(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStructV(obj, vinf, mth, args); va_end(args);
	return(*(double *)p);
}

BGBDY_API dyt dycCallStructr(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStructV(obj, vinf, mth, args); va_end(args);
	return(*(dyt *)p);
}

BGBDY_API void *dycCallStructp(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycCallStructV(obj, vinf, mth, args); va_end(args);
	return(*(void **)p);
}

BGBDY_API void dycCallStructv(void *obj, dycClassV vinf, dycMethod mth, ...)
{
	va_list args;
	va_start(args, mth); dycCallStructV(obj, vinf, mth, args); va_end(args);
}
