#include <bgbgc.h>
#include <bgbdy.h>

BGBDY_API dytf dytfCopyValue(dytf a)
{
	if(!dytfPtrP(a))return(a);
	return(dytfWrap(dyCopyValue(dytfUnwrap(a))));
}

BGBDY_API bool dytfDropValue(dytf p)
{
	if(!dytfPtrP(p))return(false);
	return(dyDropValue(dytfUnwrap(p)));
}

BGBDY_API dytf dytfClone(dytf a)
	{ return(dytfWrap(dyClone(dytfUnwrap(a)))); }
BGBDY_API char *dytfToString(dytf a)
	{ return(dyToString(dytfUnwrap(a))); }
BGBDY_API bool dytfDelete(dytf a)
	{ return(dyDelete(dytfUnwrap(a))); }

BGBDY_API dytf dytfObject()
{
	return(dytfWrap(dyObject()));
}

BGBDY_API bool dytfObjectP(dytf obj)
{
	if(!dytfPtrP(obj))return(false);
	return(dytftypep(obj, "_object_t"));
}

BGBDY_API dytf dytfBind(dytf obj, char *var, dytf val)
{
	return(dyBindB(dytfUnwrap(obj), var, val));
//	return(dytfWrap(dyBind(dytfUnwrap(obj), var, dytfUnwrap(val))));
}

BGBDY_API dytf dytfBindInfo(dytf obj, dycSlot var, dytf val)
{
	return(dytfWrap(dyBindInfo(dytfUnwrap(obj), var, dytfUnwrap(val))));
}

BGBDY_API dytf dytfTrySet(dytf obj, char *var, dytf val)
{
	return(dytfWrap(dyTrySet(dytfUnwrap(obj), var, dytfUnwrap(val))));
}

BGBDY_API dytf dytfGet(dytf obj, char *var)
{
	if(dytftypep(obj, "_object_t"))
		{ return(dyGet_I2(dytfPtrvF(obj), var)); }

	return(dytfWrap(dyGet(dytfUnwrap(obj), var)));
}

BGBDY_API dytf dytfSet(dytf obj, char *var, dytf val)
{
	dyt obj1;
	dytf t;

	if(dytfObjectP(obj))
	{
		obj1=dytfPtrvF(obj);
		t=dySetR_I2(obj1, var, val);
		if(!dytfUndefinedP(t))
			return(t);
		t=dyBind_I(obj1, var, val);
		return(t);
	}

	return(dySetB(dytfUnwrap(obj), var, val));
//	return(dytfWrap(dySet(dytfUnwrap(obj), var, dytfUnwrap(val))));
}

BGBDY_API dytf dytfTryGetRef(dytf obj, char *var)
{
	return(dytfWrap(dyTryGetRef(dytfUnwrap(obj), var)));
}

BGBDY_API dytf dytfTryGetInfo(dytf obj, char *var)
{
	return(dytfPtrF(dyTryGetInfo(dytfUnwrap(obj), var)));
}

BGBDY_API dytf dytfGetIdx(dytf obj, int idx)
{
	return(dytfWrap(dyGetIdx(dytfUnwrap(obj), idx)));
}

BGBDY_API dytf dytfSetIdx(dytf obj, int idx, dytf val)
{
	return(dytfWrap(dySetIdx(dytfUnwrap(obj), idx, dytfUnwrap(val))));
}

BGBDY_API dytf dytfGetIdxRef(dytf obj, int idx)
{
	return(dytfWrap(dyGetIdxRef(dytfUnwrap(obj), idx)));
}


BGBDY_API dytf dytfGetKey(dytf obj, dytf key)
{
	return(dytfWrap(dyGetKey(dytfUnwrap(obj), dytfUnwrap(key))));
}

BGBDY_API dytf dytfSetKey(dytf obj, dytf key, dytf val)
{
	return(dytfWrap(dySetKey(dytfUnwrap(obj),
		dytfUnwrap(key), dytfUnwrap(val))));
}

BGBDY_API dytf dytfGetKeyRef(dytf obj, dytf key)
{
	return(dytfWrap(dyGetKeyRef(dytfUnwrap(obj), dytfUnwrap(key))));
}

BGBDY_API dytf dytfGetDelegate(dytf obj, char *var)
{
	return(dytfWrap(dyGetDelegate(dytfUnwrap(obj), var)));
}

BGBDY_API void dytfSetDelegate(dytf obj, char *var, dytf val)
{
	dySetDelegate(dytfUnwrap(obj), var, dytfUnwrap(val));
}

BGBDY_API dytf dytfCall(dytf obj, char *sym, dytf *args, int nargs)
{
	dyt ar[16];
	int i;

	for(i=0; i<nargs; i++)
		ar[i]=dytfUnwrap(args[i]);
	return(dytfWrap(dyCall_I(dytfUnwrap(obj), sym, ar, nargs)));
}

BGBDY_API dytf dytfCallN(dytf obj, char *sym, dytf args)
{	
	return(dytfWrap(dyCallN(dytfUnwrap(obj), sym, dytfUnwrap(args))));
}

BGBDY_API dytf dytfLookupMethodSig(dytf obj, char *name, char *sig)
{
	return(dytfWrap(dyLookupMethodSig(dytfUnwrap(obj), name, sig)));
}

BGBDY_API dytf dytfNewN(char *name, dytf args)
{
	return(dytfWrap(dyNewN(name, dytfUnwrap(args))));
}

BGBDY_API dytf dytfNewA(char *name, dytf *args, int nargs)
{
	dyt ar[16];
	int i;

	for(i=0; i<nargs; i++)
		ar[i]=dytfUnwrap(args[i]);
	return(dytfWrap(dyNewA(name, ar, nargs)));
}

BGBDY_API dytf dytfSigCastVariantSig(char *sig, dytf val)
{
	return(dytfWrap(dyllSigCastVariantSig(sig, dytfUnwrap(val))));
}

BGBDY_API dycArray dytfDycArrayvF(dytf a)
	{ return((dycArray)dytfPtrvF(a)); }
BGBDY_API dycObject dytfDycObjectvF(dytf a)
	{ return((dycObject)dytfPtrvF(a)); }
BGBDY_API dycClass dytfDycClassvF(dytf a)
	{ return((dycClass)dytfPtrvF(a)); }

BGBDY_API bool dytfDycArrayP(dytf a)
	{ return(dycArrayP(dytfUnwrap(a))); }
BGBDY_API bool dytfDycObjectP(dytf a)
	{ return(dycObjectP(dytfUnwrap(a))); }
BGBDY_API bool dytfDycClassP(dytf a)
	{ return(dycClassP(dytfUnwrap(a))); }

BGBDY_API bool dytfDycSlotP(dytf a)
	{ return(dycSlotP(dytfUnwrap(a))); }
BGBDY_API bool dytfDycMethodP(dytf a)
	{ return(dycMethodP(dytfUnwrap(a))); }


BGBDY_API dytf dytfDycArrayGetIdx(dytf obj, int idx)
{
	return(dytfWrap(dycGetArrayVar(dytfDycArrayvF(obj), idx)));
}

BGBDY_API void dytfDycArraySetIdx(dytf obj, int idx, dytf val)
{
	dycSetArrayVar(dytfDycArrayvF(obj), idx, dytfUnwrap(val));
}

BGBDY_API dytf dytfApplyMethod(dytf obj, dytf mth,
	dytf *args, int nargs)
{
//	dyt arr[16];
//	int i;

	return(dyApplyMethodB(
		dytfUnwrap(obj), dytfUnwrap(mth), args, nargs));

//	for(i=0; i<16; i++)
//		arr[i]=dytfUnwrap(args[i]);

//	return(dytfWrap(dyApplyMethod(
//		dytfUnwrap(obj), dytfUnwrap(mth), arr, nargs)));
}

BGBDY_API dytf dytfApplyMethodLst(dytf obj, dytf mth, dytf args)
{
	return(dytfWrap(dyApplyMethodLst(
		dytfUnwrap(obj), dytfUnwrap(mth), dytfUnwrap(args))));
}
