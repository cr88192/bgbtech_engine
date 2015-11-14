/*
stuff to make dynamic types accessible from DYC.
 */

#include <bgbgc.h>
#include <bgbdy.h>


BGBDY_API int dycDObjectP(dyt obj)
{
	if(dytypep(obj, "_classobj_t"))
		return(1);
	if(dytypep(obj, "_object_t"))
		return(1);
	return(0);
}


BGBDY_API void *dycDCallSigU(dyt obj, char *name, char *sig, void **args)
{
	dyt abuf[32];
	dyt fcn;
	void *p;
	int i;

	if(dycObjectP(obj))
		return(dycCallSigU((dycObject)obj, name, sig, args));

	if(dyObjectP(obj))
	{
		fcn=dyGet(obj, name);
		p=dyApplyMethodU(obj, fcn, sig, args, -1);
		return(p);
	}

	i=dyllUnpackUArgsSigArray(sig, args, abuf);
	return(dyCall_I(obj, name, abuf, i));

//	return(UNDEFINED);
}

BGBDY_API void *dycDCallSigV(dyt obj, char *name, char *sig, va_list args)
{
	dyt abuf[32];
	dyt fcn;
	void *p;
	int i;

	if(dycObjectP(obj))
		return(dycCallSigV((dycObject)obj, name, sig, args));

	if(dyObjectP(obj))
	{
		fcn=dyGet(obj, name);
		p=dyApplyMethodV(obj, fcn, sig, args);
		return(p);
	}

	i=dyllUnpackArgsSigArray(sig, args, abuf);
	return(dyCall_I(obj, name, abuf, i));

//	return(UNDEFINED);
}

BGBDY_API void *dycDCallSig(dyt obj, char *name, char *sig, ...)
{
	va_list args;
	void *p;

	va_start(args, sig);
	p=dycDCallSigV(obj, name, sig, args);
	va_end(args);
	return(p);
}

BGBDY_API dyt dycDCallSigVr(dyt obj, char *name, char *sig, va_list lst)
	{ return(*(dyt *)dycDCallSigV(obj, name, sig, lst)); }
BGBDY_API int dycDCallSigVi(dyt obj, char *name, char *sig, va_list lst)
	{ return(*(int *)dycDCallSigV(obj, name, sig, lst)); }
BGBDY_API s64 dycDCallSigVl(dyt obj, char *name, char *sig, va_list lst)
	{ return(*(s64 *)dycDCallSigV(obj, name, sig, lst)); }
BGBDY_API float dycDCallSigVf(dyt obj, char *name, char *sig, va_list lst)
	{ return(*(float *)dycDCallSigV(obj, name, sig, lst)); }

BGBDY_API double dycDCallSigVd(dyt obj, char *name, char *sig, va_list lst)
{
	void *p;
	p=dycDCallSigV(obj, name, sig, lst);
	return(*(double *)p);
}

BGBDY_API dyt dycDCallSigUr(dyt obj, char *name, char *sig, void **args)
	{ return(*(dyt *)dycDCallSigU(obj, name, sig, args)); }
BGBDY_API int dycDCallSigUi(dyt obj, char *name, char *sig, void **args)
	{ return(*(int *)dycDCallSigU(obj, name, sig, args)); }
BGBDY_API s64 dycDCallSigUl(dyt obj, char *name, char *sig, void **args)
	{ return(*(s64 *)dycDCallSigU(obj, name, sig, args)); }
BGBDY_API float dycDCallSigUf(dyt obj, char *name, char *sig, void **args)
	{ return(*(float *)dycDCallSigU(obj, name, sig, args)); }
BGBDY_API double dycDCallSigUd(dyt obj, char *name, char *sig, void **args)
	{ return(*(double *)dycDCallSigU(obj, name, sig, args)); }

BGBDY_API int dycDCallSigi(dyt obj, char *name, char *sig, ...)
{
	va_list args;
	int v;

	va_start(args, sig);
	v=dycDCallSigVi(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API s64 dycDCallSigl(dyt obj, char *name, char *sig, ...)
{
	va_list args;
	s64 v;

	va_start(args, sig);
	v=dycDCallSigVl(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API float dycDCallSigf(dyt obj, char *name, char *sig, ...)
{
	va_list args;
	float v;

	va_start(args, sig);
	v=dycDCallSigVf(obj, name, sig, args);
	va_end(args);
	return(v);
}

BGBDY_API double dycDCallSigd(dyt obj, char *name, char *sig, ...)
{
	va_list args;
	double v;

	va_start(args, sig);
	v=dycDCallSigVd(obj, name, sig, args);
	va_end(args);
	return(v);
}


BGBDY_API void *dycDCallU(dyt obj, dycMethod mth, void **args)
{
	dyt fcn;
	void *p;

	if(dycObjectP(obj))
		return(dycCallU((dycObject)obj, mth, args));

	if(dyObjectP(obj))
	{
		fcn=dyGet(obj, mth->name);
		p=dyApplyMethodU(obj, fcn, mth->sig, args, -1);
		return(p);
	}

	p=dycDCallSigU(obj, mth->name, mth->sig, args);
	return(p);

//	fcn=BGBDYC_GetMinfFunc(obj, mth);
//	p=dycApplyMethodU(obj, fcn, mth->sig, args, -1);
//	return(p);
}

BGBDY_API void *dycDCallV(dyt obj, dycMethod mth, va_list args)
{
	dyt fcn;
	void *p;
	int i;

	if(dycObjectP(obj))
		return(dycCallV((dycObject)obj, mth, args));

	if(dyObjectP(obj))
	{
		fcn=dyGet(obj, mth->name);
		p=dyApplyMethodV(obj, fcn, mth->sig, args);
		return(p);
	}

	p=dycDCallSigV(obj, mth->name, mth->sig, args);
	return(p);

//	return(NULL);

//	fcn=BGBDYC_GetMinfFunc(obj, mth);
//	p=dycApplyMethodV(obj, fcn, mth->sig, args);
//	return(p);
}

BGBDY_API void *dycDCall(dyt obj, dycMethod mth, ...)
{
	va_list args;
	dyt fcn;
	void *p;
	int i;

	if(dycObjectP(obj))
	{
		va_start(args, mth);
		p=dycCallV((dycObject)obj, mth, args);
		va_end(args);
		return(p);
	}

	if(dyObjectP(obj))
	{
		va_start(args, mth);
		fcn=dyGet(obj, mth->name);
		p=dyApplyMethodV(obj, fcn, mth->sig, args);
		va_end(args);
		return(p);
	}

	va_start(args, mth);
	p=dycDCallSigV(obj, mth->name, mth->sig, args);
	va_end(args);
	return(p);
}

BGBDY_API int dycDCallUi(dyt obj, dycMethod mth, void **args)
	{ return(*(int *)dycDCallU(obj, mth, args)); }
BGBDY_API s64 dycDCallUl(dyt obj, dycMethod mth, void **args)
	{ return(*(s64 *)dycDCallU(obj, mth, args)); }
BGBDY_API float dycDCallUf(dyt obj, dycMethod mth, void **args)
	{ return(*(float *)dycDCallU(obj, mth, args)); }
BGBDY_API double dycDCallUd(dyt obj, dycMethod mth, void **args)
	{ return(*(double *)dycDCallU(obj, mth, args)); }
BGBDY_API dyt dycDCallUr(dyt obj, dycMethod mth, void **args)
	{ return(*(dyt *)dycDCallU(obj, mth, args)); }
BGBDY_API void *dycDCallUp(dyt obj, dycMethod mth, void **args)
	{ return(*(void **)dycDCallU(obj, mth, args)); }
BGBDY_API void dycDCallUv(dyt obj, dycMethod mth, void **args)
	{ dycDCallU(obj, mth, args); }

BGBDY_API int dycDCallVi(dyt obj, dycMethod mth, va_list args)
	{ return(*(int *)dycDCallV(obj, mth, args)); }
BGBDY_API s64 dycDCallVl(dyt obj, dycMethod mth, va_list args)
	{ return(*(s64 *)dycDCallV(obj, mth, args)); }
BGBDY_API float dycDCallVf(dyt obj, dycMethod mth, va_list args)
	{ return(*(float *)dycDCallV(obj, mth, args)); }
BGBDY_API double dycDCallVd(dyt obj, dycMethod mth, va_list args)
	{ return(*(double *)dycDCallV(obj, mth, args)); }
BGBDY_API dyt dycDCallVr(dyt obj, dycMethod mth, va_list args)
	{ return(*(dyt *)dycDCallV(obj, mth, args)); }
BGBDY_API void *dycDCallVp(dyt obj, dycMethod mth, va_list args)
	{ return(*(void **)dycDCallV(obj, mth, args)); }
BGBDY_API void dycDCallVv(dyt obj, dycMethod mth, va_list args)
	{ dycDCallV(obj, mth, args); }

BGBDY_API int dycDCalli(dyt obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycDCallV(obj, mth, args); va_end(args);
	return(*(int *)p);
}

BGBDY_API s64 dycDCalll(dyt obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycDCallV(obj, mth, args); va_end(args);
	return(*(s64 *)p);
}

BGBDY_API float dycDCallf(dyt obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycDCallV(obj, mth, args); va_end(args);
	return(*(float *)p);
}

BGBDY_API double dycDCalld(dyt obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycDCallV(obj, mth, args); va_end(args);
	return(*(double *)p);
}

BGBDY_API dyt dycDCallr(dyt obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycDCallV(obj, mth, args); va_end(args);
	return(*(dyt *)p);
}

BGBDY_API void *dycDCallp(dyt obj, dycMethod mth, ...)
{
	va_list args; void *p;
	va_start(args, mth); p=dycDCallV(obj, mth, args); va_end(args);
	return(*(void **)p);
}

BGBDY_API void dycDCallv(dyt obj, dycMethod mth, ...)
{
	va_list args;
	va_start(args, mth); dycDCallV(obj, mth, args); va_end(args);
}



BGBDY_API dyt dycDGet(dyt obj, dycSlot idx)
{
	void *p;
//	thLockFastMutex(bgbdyc_mutex);
	p=BGBDYC_GetDynSlotIdxPtrTH(obj, idx);
//	thUnlockFastMutex(bgbdyc_mutex);
	if(!p) { dySetStatus("badSlot"); return(NULL); }
	return(dyllGetVarSig(p, idx->sig));
}

BGBDY_API int dycDSet(dyt obj, dycSlot slot, dyt val)
{
	void *p;
//	p=dycGetSlotPtr(obj, slot);
	p=BGBDYC_GetDynSlotIdxPtrTH(obj, slot);
	if(!p) { dySetStatus("badSlot"); return(-1); }
	dyllSetVarSig(p, slot->sig, val);
	return(0);
}

BGBDY_API dyt dycDGetr(dyt obj, dycSlot idx)
	{ return(dycDGet(obj, idx)); }
BGBDY_API void dycDSetr(dyt obj, dycSlot idx, dyt val)
	{ dycDSet(obj, idx, val); }

BGBDY_API int dycDGetsb(dyt obj, dycSlot idx)
	{ return(*(char *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycDGetub(dyt obj, dycSlot idx)
	{ return(*(byte *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycDGetss(dyt obj, dycSlot idx)
	{ return(*(short *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycDGetus(dyt obj, dycSlot idx)
	{ return(*(unsigned short *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API int dycDGeti(dyt obj, dycSlot idx)
	{ return(*(int *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API s64 dycDGetl(dyt obj, dycSlot idx)
	{ return(*(s64 *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API float dycDGetf(dyt obj, dycSlot idx)
	{ return(*(float *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API double dycDGetd(dyt obj, dycSlot idx)
	{ return(*(double *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
//BGBDY_API dyt dycDGetr(dyt obj, dycSlot idx)
//	{ return(*(dyt *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }
BGBDY_API void *dycDGetp(dyt obj, dycSlot idx)
	{ return(*(void **)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)); }

BGBDY_API void dycDSetsb(dyt obj, dycSlot idx, int val)
	{ *(char *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSetub(dyt obj, dycSlot idx, int val)
	{ *(byte *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSetss(dyt obj, dycSlot idx, int val)
	{ *(short *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSetus(dyt obj, dycSlot idx, int val)
	{ *(unsigned short *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSeti(dyt obj, dycSlot idx, int val)
	{ *(int *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSetl(dyt obj, dycSlot idx, s64 val)
	{ *(s64 *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSetf(dyt obj, dycSlot idx, float val)
	{ *(float *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSetd(dyt obj, dycSlot idx, double val)
	{ *(double *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
//BGBDY_API void dycDSetr(dyt obj, dycSlot idx, dyt val)
//	{ *(dyt *)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }
BGBDY_API void dycDSetp(dyt obj, dycSlot idx, void *val)
	{ *(void **)BGBDYC_GetDynSlotIdxPtrTH(obj, idx)=val; }

BGBDY_API dycMethod dycIdxMethodArgsN(dycClass inf, char *name, dyt args)
{
	dycMethod *ma;
	dycMethod bm;
	char *s;
	int i, j, bn;

	name=dycGetClassSpecialName(inf, name);

	bm=NULL; bn=999999;
	ma=dycEnumMethodArray(inf);
	for(i=0; ma[i]; i++)
	{
		s=dycGetMethodName(ma[i]);
		if(!s || strcmp(s, name))
			continue;
		s=dycGetMethodSig(ma[i]);
		if(!s)continue;
		j=dyllCheckCallSigList(s, args);
		if(j<0)continue;
		
		if(j<bn)
		{
			bm=ma[i];
			bn=j;
		}
	}
	gcfree(ma);
	
	return(bm);
}

BGBDY_API dycMethod dycIdxMethodArray(dycClass inf, char *name,
	dyt *args, int nargs)
{
	dycMethod *ma;
	dycMethod bm;
	char *s;
	int i, j, bn;

	name=dycGetClassSpecialName(inf, name);

	bm=NULL; bn=999999;
	ma=dycEnumMethodArray(inf);
	for(i=0; ma[i]; i++)
	{
		s=dycGetMethodName(ma[i]);
		if(!s || strcmp(s, name))
			continue;
		s=dycGetMethodSig(ma[i]);
		if(!s)continue;
		j=dyllCheckCallSigArray(s, args, nargs);
		if(j<0)continue;
		
		if(j<bn)
		{
			bm=ma[i];
			bn=j;
		}
	}
	gcfree(ma);
	
	return(bm);
}

BGBDY_API dyt dycCallN(dycObject obj, dycMethod minf, dyt args)
{
	void *ar[64];
	void *pv;
	char *s0, *s1;
	dyt v;

//	if(!minf)return(UNDEFINED);
	s0=dycGetMethodSig(minf);
	dyllUnpackListSigUArgs(s0, args, ar);
	pv=dycCallU(obj, minf, ar);
	s1=dyllSigEsig(s0);
	v=dyllExtractVariantBufferSig(s1, pv);
	return(v);
}

BGBDY_API dyt dycCallArray(dycObject obj, dycMethod minf,
	dyt *args, int nargs)
{
	void *ar[64];
	void *pv;
	char *s0, *s1;
	dyt v;

//	if(!minf)return(UNDEFINED);
	s0=dycGetMethodSig(minf);
	dyllUnpackArraySigUArgs(s0, args, ar);
	pv=dycCallU(obj, minf, ar);
	s1=dyllSigEsig(s0);
	v=dyllExtractVariantBufferSig(s1, pv);
	return(v);
}



BGBDY_API dyt dycCallStaticN(dycClass inf, dycMethod minf, dyt args)
{
	void *ar[64];
	void *pv;
	char *s0, *s1;
	dyt v;

//	if(!minf)return(UNDEFINED);
	s0=dycGetMethodSig(minf);
	dyllUnpackListSigUArgs(s0, args, ar);
	pv=dycCallStaticU(inf, minf, ar);
	s1=dyllSigEsig(s0);
	v=dyllExtractVariantBufferSig(s1, pv);
	return(v);
}

BGBDY_API dyt dycCallStaticArray(dycClass inf, dycMethod minf,
	dyt *args, int nargs)
{
	void *ar[64];
	void *pv;
	char *s0, *s1;
	dyt v;

//	if(!minf)return(UNDEFINED);
	s0=dycGetMethodSig(minf);
	dyllUnpackArraySigUArgs(s0, args, ar);
	pv=dycCallStaticU(inf, minf, ar);
	s1=dyllSigEsig(s0);
	v=dyllExtractVariantBufferSig(s1, pv);
	return(v);
}

BGBDY_API dyt dycLookupMethodArgsN(dycObject obj, char *name, dyt args)
{
	dycClass inf;
	dycMethod minf;
	dyt fcn;

	inf=dycGetObjClass(obj);
	minf=dycIdxMethodArgsN(inf, name, args);
	if(!minf)return(UNDEFINED);
	fcn=dycGetObjMethodFunc(obj, minf);
	return(fcn);
}

BGBDY_API dyt dycLookupStaticMethodArgsN(dycClass inf, char *name, dyt args)
{
	dycMethod minf;
	dyt fcn;

	minf=dycIdxMethodArgsN(inf, name, args);
	if(!minf)return(UNDEFINED);
	fcn=dycGetMethodFunc(inf, minf);
	return(fcn);
}


BGBDY_API dyt dycCallNameN(dycObject obj, char *name, dyt args)
{
	dycClass inf;
	dycMethod minf;
	dyt p;

	inf=dycGetObjClass(obj);
	minf=dycIdxMethodArgsN(inf, name, args);
	if(!minf)
	{
//		printf("dycCallNameN: fail match method %s\n", name);
		return(UNDEFINED);
	}
	p=dycCallN(obj, minf, args);
//	printf("dycCallNameN: success %s()->%p\n", name, p);
	return(p);
}

BGBDY_API dyt dycCallNameArray(dycObject obj, char *name,
	dyt *args, int nargs)
{
	dycClass inf;
	dycMethod minf;

	inf=dycGetObjClass(obj);
	minf=dycIdxMethodArray(inf, name, args, nargs);
	if(!minf)return(UNDEFINED);
	return(dycCallArray(obj, minf, args, nargs));
}

BGBDY_API dyt dycCallStaticNameN(dycClass inf, char *name, dyt args)
{
	dycMethod minf;

	minf=dycIdxMethodArgsN(inf, name, args);
	if(!minf)return(UNDEFINED);
	return(dycCallStaticN(inf, minf, args));
}

BGBDY_API dyt dycCallStaticNameArray(dycClass inf, char *name,
	dyt *args, int nargs)
{
	dycMethod minf;

	minf=dycIdxMethodArray(inf, name, args, nargs);
	if(!minf)return(UNDEFINED);
	return(dycCallStaticArray(inf, minf, args, nargs));
}

BGBDY_API dycObject dycNewInitN(dycClass inf, dyt args)
{
	dycObject obj;

	if(!inf)
		return(NULL);

	if(!args)
	{
//		printf("dycNewInitN: no args\n");
		obj=dycNew(inf);
		return(obj);
	}

	obj=dycAlloc(inf);
	dycCallNameN(obj, "<init>", args);
	return(obj);
}

BGBDY_API dycObject dycNewInitArray(dycClass inf, dyt *args, int nargs)
{
	dycObject obj;

	if(!inf)
		return(NULL);

	if(!args)
	{
		obj=dycNew(inf);
		return(obj);
	}

	obj=dycAlloc(inf);
	dycCallNameArray(obj, "<init>", args, nargs);
	return(obj);
}

BGBDY_API dycObject dycNewInitClassN(char *name, dyt args)
{
	char tb[256];
	dycClass inf;
	dycObject obj;
	char *s;

	strcpy(tb, name); s=tb;
	while(*s) { if(*s=='.')*s='/'; s++; }

	inf=dycGetClass(tb);
	if(!inf)return(NULL);
	obj=dycNewInitN(inf, args);
	return(obj);
}

BGBDY_API dycObject dycNewInitClassArray(char *name, dyt *args, int nargs)
{
	char tb[256];
	dycClass inf;
	dycObject obj;
	char *s;

	strcpy(tb, name); s=tb;
	while(*s) { if(*s=='.')*s='/'; s++; }

	inf=dycGetClass(tb);
	if(!inf)return(NULL);
	obj=dycNewInitArray(inf, args, nargs);
	return(obj);
}


BGBDY_API dyt dycCallAliasNameArray(dyt obj, char *name,
	dyt *args, int nargs)
{
	BGBDY_ObjVTab *ty;
	dycClass inf;
	dycObject tobj;
	dycMethod minf;
	dyt t;

	ty=BGBDY_GetObjVTabFast(obj);
	if(!ty)return(UNDEFINED);

	if(!(ty->classAlias))
		return(UNDEFINED);

	inf=dycGetClass(ty->classAlias);
	if(!inf)return(UNDEFINED);

//	inf=dycGetObjClass(obj);

	minf=dycIdxMethodArray(inf, name, args, nargs);
	if(!minf)return(UNDEFINED);

	tobj=dycNewInitN(inf, dylist1(obj));
	if(!tobj)return(UNDEFINED);

	t=dycCallArray(tobj, minf, args, nargs);
	dycDeleteObject(tobj);

	return(t);
}

BGBDY_API dyt dycCallAliasNameN(dyt obj, char *name, dyt args)
{
	BGBDY_ObjVTab *ty;
	dycClass inf;
	dycObject tobj;
	dycMethod minf;
	dyt t;

	ty=BGBDY_GetObjVTabFast(obj);
	if(!ty)return(UNDEFINED);

	if(!(ty->classAlias))
		return(UNDEFINED);

	inf=dycGetClass(ty->classAlias);
	if(!inf)return(UNDEFINED);

//	inf=dycGetObjClass(obj);

	minf=dycIdxMethodArgsN(inf, name, args);
	if(!minf)return(UNDEFINED);

	tobj=dycNewInitN(inf, dylist1(obj));
	if(!tobj)return(UNDEFINED);

	t=dycCallN(tobj, minf, args);
	dycDeleteObject(tobj);

	return(t);
}

BGBDY_API int dycGetObjectDelegateArray(dycObject obj, dyt *arr, int max)
{
	dycSlot slots[256];
	void *ar[256];
	void *pv;
	char *s0, *s1;
	dyt v;
	int i, n, n1;

	n=BGBDYC_EnumClassDelegateSlots(obj->info, slots, 0, 256);
	
	n1=0;
	for(i=0; i<n; i++)
	{
		if(n1>=(max-1))break;
		v=dycGet(obj, slots[i]);
		if(v && (v!=UNDEFINED))
			arr[n1++]=v;
	}
	arr[n1]=NULL;

	return(n1);
}

BGBDY_API dycSlot dycGetProtoSlotFlags(
	char *name, char *sig, char *flags)
{
	s64 li;
	li=dyllDecodeFlagSig(flags);
	return(BGBDYC_GetProtoSlotFl(name, sig, li,
		dyGetAccessModeDefault()));
}

BGBDY_API dycMethod dycGetProtoMethodFlags(
	char *name, char *sig, char *flags)
{
	s64 li;
	li=dyllDecodeFlagSig(flags);
	return(BGBDYC_GetProtoMinfFl(name, sig, li,
		dyGetAccessModeDefault()));
}
