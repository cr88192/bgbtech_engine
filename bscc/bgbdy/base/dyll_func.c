#define DYLL_HASASM

#include <bgbgc.h>
#include <bgbdy.h>

#ifdef DYLL_HASASM
#include <bgbasm.h>
#endif

char *dyll_func_toString(dyt obj)
{
	char tb[1024];
	BGBGC_RawFunc *inf;
	char *s0, *s1, *sty;

	inf=(BGBGC_RawFunc *)obj;

	sty="";
	switch(inf->type)
	{
	case BGBGC_RAWFUNC_BASIC:
		sty="func"; break;
		break;
	case BGBGC_RAWFUNC_DATA:
		sty="lfunc"; break;
	case BGBGC_RAWFUNC_SELF:
		sty="vmethod"; break;
	case BGBGC_RAWFUNC_CLASS:
		sty="smethod"; break;
	case BGBGC_RAWFUNC_SELFDATA:
		sty="lvmethod"; break;
	case BGBGC_RAWFUNC_CLASSDATA:
		sty="lsmethod"; break;
	default: sty="unknown"; break;
	}

	s0=dyllGetAddrName(inf->fcn);

	sprintf(tb, "#!(raw_func(%s:%p):%s%s)",
		sty, inf->fcn, s0, inf->sig);
	return(gcrstrdup(tb));
}

void BGBGC_InitDyLL_Func()
{
	dyTypeToString("_raw_func_t", dyll_func_toString);
}

#ifdef DYLL_HASASM
BGBDY_API dyt dyRawFunc(char *sig, void *fcn)
{
	char tb[256];
	BGBGC_RawFunc *inf;
	char *s, *t;
	int ty;

	BGBGC_InitDyLL();

	if(!fcn || (fcn==UNDEFINED))
		return(fcn);

	ty=BGBGC_RAWFUNC_BASIC;

	s=sig; t=tb;
	if(*s=='(')*t++=*s++;
	if(!strncmp(s, "Cx", 2))
		{ s+=2; ty=BGBGC_RAWFUNC_SELF; }
	if(!strncmp(s, "Cy", 2))
		{ s+=2; ty=BGBGC_RAWFUNC_CLASS; }
	while(*s)*t++=*s++;
	*t++=0;

//	inf=gctalloc("_raw_func_t", sizeof(BGBGC_RawFunc));
//	inf->type=BGBGC_RAWFUNC_BASIC;
//	inf->sig=dystrdup(sig);
//	inf->sig2=inf->sig;

	inf=gctalloc("_raw_func_t", sizeof(BGBGC_RawFunc));
	inf->type=ty;
	inf->sig=dystrdup(tb);
	inf->sig2=dystrdup(sig);

#ifdef BGBDY_USEGENAPPLY
	inf->sig2id=DYLL_CalcSigID(sig);
#endif

	inf->esig=dystrdup(dyllSigRet(sig));
	inf->fcn=fcn;

	inf->thunk=BASM_MakeBufferCallStub(inf->fcn, inf->sig2);

	return((dyt)inf);
}

BGBDY_API dyt dyRawClose(char *sig, void *fcn, void *data)
{
	char tb[256];
	BGBGC_RawFunc *inf;
	char *s, *t;

	BGBGC_InitDyLL();

	if(!fcn || (fcn==UNDEFINED))
		return(fcn);

	s=sig; t=tb;
	if(*s=='(')*t++=*s++;
	*t++='r';
	while(*s)*t++=*s++;
	*t++=0;

	inf=gctalloc("_raw_func_t", sizeof(BGBGC_RawFunc));
	inf->type=BGBGC_RAWFUNC_DATA;
	inf->sig=dystrdup(sig);
	inf->sig2=dystrdup(tb);
	inf->esig=dystrdup(dyllSigRet(sig));
	inf->fcn=fcn;
	inf->data=data;

#ifdef BGBDY_USEGENAPPLY
	inf->sig2id=DYLL_CalcSigID(tb);
#endif

	inf->thunk=BASM_MakeBufferCallStub(inf->fcn, inf->sig2);

	return((dyt)inf);
}

BGBDY_API dyt dyRawMethod(char *sig, void *fcn)
{
	char tb[256];
	BGBGC_RawFunc *inf;
	char *s, *t;

	BGBGC_InitDyLL();

	if(!fcn || (fcn==UNDEFINED))
		return(fcn);

	s=sig; t=tb;
	if(*s=='(')*t++=*s++;
	if(!strncmp(s, "Cx", 2)) { s+=2; }
	if(!strncmp(s, "Cy", 2)) { s+=2; }
	*t++='r';
	while(*s)*t++=*s++;
	*t++=0;

	inf=gctalloc("_raw_func_t", sizeof(BGBGC_RawFunc));
	inf->type=BGBGC_RAWFUNC_SELF;
	inf->sig=dystrdup(sig);
	inf->sig2=dystrdup(tb);
	inf->esig=dystrdup(dyllSigRet(sig));
	inf->fcn=fcn;

#ifdef BGBDY_USEGENAPPLY
	inf->sig2id=DYLL_CalcSigID(tb);
#endif

	inf->thunk=BASM_MakeBufferCallStub(inf->fcn, inf->sig2);

	return((dyt)inf);
}

BGBDY_API dyt dyRawMethodData(char *sig, void *fcn, void *data)
{
	char tb[64];
	BGBGC_RawFunc *inf;
	char *s, *t;

	BGBGC_InitDyLL();

	if(!fcn || (fcn==UNDEFINED))
		return(fcn);

	s=sig; t=tb;
	if(*s=='(')*t++=*s++;
	if(!strncmp(s, "Cx", 2)) { s+=2; }
	if(!strncmp(s, "Cy", 2)) { s+=2; }
	*t++='r'; *t++='r';
	while(*s)*t++=*s++;
	*t++=0;

	inf=gctalloc("_raw_func_t", sizeof(BGBGC_RawFunc));
	inf->type=BGBGC_RAWFUNC_SELFDATA;
	inf->sig=dystrdup(sig);
	inf->sig2=dystrdup(tb);
	inf->esig=dystrdup(dyllSigRet(sig));
	inf->fcn=fcn;
	inf->data=data;

#ifdef BGBDY_USEGENAPPLY
	inf->sig2id=DYLL_CalcSigID(tb);
#endif

	inf->thunk=BASM_MakeBufferCallStub(inf->fcn, inf->sig2);

	return((dyt)inf);
}

BGBDY_API dyt dyRawStaticMethod(char *sig, void *fcn)
{
	char tb[64];
	BGBGC_RawFunc *inf;
	char *s, *t;

	BGBGC_InitDyLL();

	if(!fcn || (fcn==UNDEFINED))
		return(fcn);

	s=sig; t=tb;
	if(*s=='(')*t++=*s++;
	if(!strncmp(s, "Cx", 2)) { s+=2; }
	if(!strncmp(s, "Cy", 2)) { s+=2; }
	*t++='r';
	while(*s)*t++=*s++;
	*t++=0;

	inf=gctalloc("_raw_func_t", sizeof(BGBGC_RawFunc));
	inf->type=BGBGC_RAWFUNC_CLASS;
	inf->sig=dystrdup(sig);
	inf->sig2=dystrdup(tb);
	inf->esig=dystrdup(dyllSigRet(sig));
	inf->fcn=fcn;

#ifdef BGBDY_USEGENAPPLY
	inf->sig2id=DYLL_CalcSigID(tb);
#endif

	inf->thunk=BASM_MakeBufferCallStub(inf->fcn, inf->sig2);

	return((dyt)inf);
}

BGBDY_API dyt dyRawStaticMethodData(char *sig, void *fcn, void *data)
{
	char tb[64];
	BGBGC_RawFunc *inf;
	char *s, *t;

	BGBGC_InitDyLL();

	if(!fcn || (fcn==UNDEFINED))
		return(fcn);

	s=sig; t=tb;
	if(*s=='(')*t++=*s++;
	if(!strncmp(s, "Cx", 2)) { s+=2; }
	if(!strncmp(s, "Cy", 2)) { s+=2; }
	*t++='r'; *t++='r';
	while(*s)*t++=*s++;
	*t++=0;

	inf=gctalloc("_raw_func_t", sizeof(BGBGC_RawFunc));
	inf->type=BGBGC_RAWFUNC_CLASSDATA;
	inf->sig=dystrdup(sig);
	inf->sig2=dystrdup(tb);
	inf->esig=dystrdup(dyllSigRet(sig));
	inf->fcn=fcn;
	inf->data=data;

#ifdef BGBDY_USEGENAPPLY
	inf->sig2id=DYLL_CalcSigID(tb);
#endif

	inf->thunk=BASM_MakeBufferCallStub(inf->fcn, inf->sig2);

	return((dyt)inf);
}

BGBDY_API char *dyllGenSym()
{
	BGBGC_InitDyLL();
	return((char *)dysymbol(BASM_GenSym()));
}

#else

BGBDY_API dyt dyRawFunc(char *sig, void *fcn)
	{ return(NULL); }
BGBDY_API dyt dyRawClose(char *sig, void *fcn, void *data)
	{ return(NULL); }
BGBDY_API dyt dyRawMethod(char *sig, void *fcn)
	{ return(NULL); }
BGBDY_API dyt dyRawMethodData(char *sig, void *fcn, void *data)
	{ return(NULL); }

#endif


#ifdef DYLL_HASASM

BGBDY_API int dyRawFuncP(dyt obj)
{
	char *ty;

	ty=dygettype(obj);
	if(!ty)return(0);

	if(!strcmp(ty, "_raw_func_t"))return(1);
//	if(!strcmp(ty, "_raw_close_t"))return(1);
//	if(!strcmp(ty, "_raw_method_t"))return(1);

	return(0);
}

BGBDY_API int dyRawFuncBasicP(dyt obj)
{
	BGBGC_RawFunc *inf;
	char *ty;

	ty=dygettype(obj);
	if(!ty)return(0);

	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=(BGBGC_RawFunc *)obj;
		if(inf->type==BGBGC_RAWFUNC_BASIC)
			return(1);
		return(0);
	}

//	if(!strcmp(ty, "_raw_close_t"))return(1);
//	if(!strcmp(ty, "_raw_method_t"))return(1);

	return(0);
}

BGBDY_API char *dyRawFuncGetSig(dyt obj)
{
	BGBGC_RawFunc *inf;
	
	inf=(BGBGC_RawFunc *)obj;
	return(inf->sig);
}

BGBDY_API void *dyRawFuncGetFuncPtr(dyt obj)
{
	BGBGC_RawFunc *inf;
	
	inf=(BGBGC_RawFunc *)obj;
	return(inf->fcn);
}

BGBDY_API void *dyllApplyCallSig(BGBGC_RawFunc *inf, void *buf)
{
	if(inf->thunk)
	{
		((void(*)(void*))inf->thunk)(buf);
		return(buf);
	}

#ifdef BGBDY_USEGENAPPLY
	DYLL_ApplyCallSig2(inf, buf);
	return(buf);
#endif

#ifndef BGBDY_USEGENAPPLY
	BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
	return(buf);
#endif
}

BGBDY_API void *dyllApplyRawFuncD(BGBGC_RawFunc *inf, dyt obj, dyt *args, int nargs)
{
	byte *buf;
	char *esig;
	int i;

	buf=gcralloc(256);
	switch(inf->type)
	{
	case BGBGC_RAWFUNC_BASIC:
		i=dyllRepackBufSigArray(inf->sig,
			&esig, buf, args, nargs);
		if(i<0)return(UNDEFINED);
//		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_DATA:
		i=dyllRepackBufSigArray(inf->sig, &esig,
			buf+sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELF:
	case BGBGC_RAWFUNC_CLASS:
		i=dyllRepackBufSigArray(inf->sig, &esig,
			buf+sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELFDATA:
	case BGBGC_RAWFUNC_CLASSDATA:
		i=dyllRepackBufSigArray(inf->sig, &esig,
			buf+2*sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	}
	return(buf);
}

BGBDY_API void *dyllApplyRawFuncB(BGBGC_RawFunc *inf, dyt obj,
	dytf *args, int nargs)
{
	byte *buf;
	char *esig;
	int i;

	buf=gcralloc(256);
	switch(inf->type)
	{
	case BGBGC_RAWFUNC_BASIC:
		i=dyllRepackBufSigArrayB(inf->sig,
			&esig, buf, args, nargs);
		if(i<0)return(UNDEFINED);
//		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_DATA:
		i=dyllRepackBufSigArrayB(inf->sig, &esig,
			buf+sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELF:
	case BGBGC_RAWFUNC_CLASS:
		i=dyllRepackBufSigArrayB(inf->sig, &esig,
			buf+sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELFDATA:
	case BGBGC_RAWFUNC_CLASSDATA:
		i=dyllRepackBufSigArrayB(inf->sig, &esig,
			buf+2*sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	}
	return(buf);
}

int dyllRepackBufRawFuncV(BGBGC_RawFunc *inf,
	char *sig, byte *buf, va_list lst)
{
	int i;
	i=dyllRepackBufSigV(sig, buf, lst);
	return(i);
}

BGBDY_API void *dyllApplyRawFuncV(BGBGC_RawFunc *inf, dyt obj, char *sig, va_list lst)
{
	byte *buf;
	int i;

	buf=gcralloc(256);
	switch(inf->type)
	{
	case BGBGC_RAWFUNC_BASIC:
		i=dyllRepackBufSigV(sig, buf, lst);
		if(i<0)return(UNDEFINED);
//		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_DATA:
		i=dyllRepackBufSigV(sig, buf+sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELF:
	case BGBGC_RAWFUNC_CLASS:
		i=dyllRepackBufSigV(sig, buf+sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELFDATA:
	case BGBGC_RAWFUNC_CLASSDATA:
		i=dyllRepackBufSigV(sig, buf+2*sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	}
	return(buf);
}

BGBDY_API void *dyllApplyRawFuncU(BGBGC_RawFunc *inf, dyt obj, char *sig, void **args)
{
	byte *buf;
	int i;

	buf=gcralloc(256);
	switch(inf->type)
	{
	case BGBGC_RAWFUNC_BASIC:
		i=dyllRepackBufSigU(sig, buf, args);
		if(i<0)return(UNDEFINED);
//		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_DATA:
		i=dyllRepackBufSigU(sig, buf+sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELF:
	case BGBGC_RAWFUNC_CLASS:
		i=dyllRepackBufSigU(sig, buf+sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELFDATA:
	case BGBGC_RAWFUNC_CLASSDATA:
		i=dyllRepackBufSigU(sig, buf+2*sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	}
	return(buf);
}

BGBDY_API void *dyllApplyObjRawFuncV(BGBGC_RawFunc *inf,
	dycObject obj, char *sig, va_list lst)
{
	byte *buf;
	int i;

	buf=gcralloc(256);
	switch(inf->type)
	{
	case BGBGC_RAWFUNC_BASIC:
		i=dyllRepackBufSigV(sig, buf, lst);
		if(i<0)return(UNDEFINED);
//		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_DATA:
		i=dyllRepackBufSigV(sig, buf+sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELF:
		i=dyllRepackBufSigV(sig, buf+sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELFDATA:
		i=dyllRepackBufSigV(sig, buf+2*sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_CLASS:
		i=dyllRepackBufSigV(sig, buf+sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj->info;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_CLASSDATA:
		i=dyllRepackBufSigV(sig, buf+2*sizeof(void *), lst);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj->info;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	}
	return(buf);
}

BGBDY_API void *dyllApplyObjRawFuncU(BGBGC_RawFunc *inf,
	dycObject obj, char *sig, void **args)
{
	byte *buf;
	int i;

	buf=gcralloc(256);
	switch(inf->type)
	{
	case BGBGC_RAWFUNC_BASIC:
		i=dyllRepackBufSigU(sig, buf, args);
		if(i<0)return(UNDEFINED);
//		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_DATA:
		i=dyllRepackBufSigU(sig, buf+sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELF:
		i=dyllRepackBufSigU(sig, buf+sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_SELFDATA:
		i=dyllRepackBufSigU(sig, buf+2*sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	case BGBGC_RAWFUNC_CLASS:
		i=dyllRepackBufSigU(sig, buf+sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj->info;
		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		break;
	case BGBGC_RAWFUNC_CLASSDATA:
		i=dyllRepackBufSigU(sig, buf+2*sizeof(void *), args);
		if(i<0)return(UNDEFINED);
		((void **)buf)[0]=obj->info;
		((void **)buf)[1]=inf->data;
//		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		dyllApplyCallSig(inf, buf);
		break;
	}
	return(buf);
}

void *bgbdyll_rawfunc_ApplyV_self(void *obj,
	dyt mth, char *sig, va_list args)
{
	BGBGC_RawFunc *inf;
	byte *buf;
	int i;

	inf=(BGBGC_RawFunc *)mth;
	buf=gcralloc(256);
	i=dyllRepackBufSigV(sig, buf+sizeof(void *), args);
	if(i<0)return(UNDEFINED);
	((void **)buf)[0]=obj;
//	dyllApplyCallSig(inf, buf);
//	return(buf);

	if(inf->thunk) { ((void(*)(void*))inf->thunk)(buf); return(buf); }
	BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
	return(buf);
}

void *bgbdyll_rawfunc_ApplyU(void *obj,
	dyt mth, char *sig, void **args, int szargs)
	{ return(dyllApplyObjRawFuncU((BGBGC_RawFunc *)mth, obj, sig, args)); }
void *bgbdyll_rawfunc_ApplyV(void *obj,
	dyt mth, char *sig, va_list args)
	{ return(dyllApplyObjRawFuncV((BGBGC_RawFunc *)mth, obj, sig, args)); }

BGBDYC_ApplyU_t dyllGetRawFuncApplyU(BGBGC_RawFunc *inf)
{
	return(bgbdyll_rawfunc_ApplyU);
}

BGBDYC_ApplyV_t dyllGetRawFuncApplyV(BGBGC_RawFunc *inf)
{
	if(inf->type==BGBGC_RAWFUNC_SELF)
		return(bgbdyll_rawfunc_ApplyV_self);
	return(bgbdyll_rawfunc_ApplyV);
}

#if 0
// dyt dyApplyRawFunc(dyt obj, dyt fcn, dyt *args, int nargs)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig;
	dyt p;
	int i;

	buf=gcralloc(256);

	if(dytypep(fcn, "_raw_func_t"))
	{
		inf=fcn;
		switch(inf->type)
		{
		case BGBGC_RAWFUNC_BASIC:
			i=dyllRepackBufSigArray(inf->sig,
				&esig, buf, args, nargs);
			if(i<0)return(UNDEFINED);
			BASM_CallSig(inf->fcn, inf->sig, buf, buf);
			break;
		case BGBGC_RAWFUNC_DATA:
			i=dyllRepackBufSigArray(inf->sig, &esig,
				buf+sizeof(void *), args, nargs);
			if(i<0)return(UNDEFINED);
			*(void **)buf=inf->data;
			BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
			break;
		case BGBGC_RAWFUNC_SELF:
			i=dyllRepackBufSigArray(inf->sig, &esig,
				buf+sizeof(void *), args, nargs);
			if(i<0)return(UNDEFINED);
			((void **)buf)[0]=obj;
			BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
			break;
		case BGBGC_RAWFUNC_SELFDATA:
			i=dyllRepackBufSigArray(inf->sig, &esig,
				buf+2*sizeof(void *), args, nargs);
			if(i<0)return(UNDEFINED);
			((void **)buf)[0]=obj;
			((void **)buf)[1]=inf->data;
			BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
			break;
		}
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

	if(dytypep(fcn, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArray(inf->sig, &esig,
			buf+sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

	if(dytypep(fcn, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArray(inf->sig, &esig,
			buf+2*sizeof(void *), args, nargs);
		if(i<0)return(UNDEFINED);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

	return(UNDEFINED);
}

// dyt dyApplyRawFuncSig(dyt obj, dyt fcn, char *sig, va_list lst)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	int i;

	ty=dygettype(fcn);
	buf=gcralloc(256);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(UNDEFINED);
	}

//	if(dytypep(fcn, "_raw_func_t"))
	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig, buf, sig, lst);
		if(i<0)return(UNDEFINED);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

//	if(dytypep(fcn, "_raw_close_t"))
	if(!strcmp(ty, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, lst);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

//	if(dytypep(fcn, "_raw_method_t"))
	if(!strcmp(ty, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, lst);
		if(i<0)return(UNDEFINED);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackArgsSigArray(sig, lst, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		return(p);
	}

	dySetStatus("badMethodType");
	return(UNDEFINED);
}

// s64 dyApplyRawFuncSigLong(dyt obj, dyt fcn, char *sig, va_list lst)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	s64 v;
	int i;

	ty=dygettype(fcn);
	buf=gcralloc(256);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(0);
	}

//	if(dytypep(fcn, "_raw_func_t"))
	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig, buf, sig, lst);
		if(i<0)return(0);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		v=dyllExtractLongBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_close_t"))
	if(!strcmp(ty, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, lst);
		if(i<0)return(0);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractLongBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_method_t"))
	if(!strcmp(ty, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, lst);
		if(i<0)return(0);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractLongBufferSig(esig, buf);
		return(v);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackArgsSigArray(sig, lst, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		return(dylongv(p));
	}

	dySetStatus("badMethodType");
	return(0);
}

// double dyApplyRawFuncSigReal(dyt obj, dyt fcn, char *sig, va_list lst)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	double v;
	int i;

	ty=dygettype(fcn);
	buf=gcralloc(256);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(0);
	}

//	if(dytypep(fcn, "_raw_func_t"))
	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig, buf, sig, lst);
		if(i<0)return(0);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		v=dyllExtractRealBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_close_t"))
	if(!strcmp(ty, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, lst);
		if(i<0)return(0);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractRealBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_method_t"))
	if(!strcmp(ty, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, lst);
		if(i<0)return(0);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractRealBufferSig(esig, buf);
		return(v);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackArgsSigArray(sig, lst, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		return(dydoublev(p));
	}

	dySetStatus("badMethodType");
	return(0);
}

// void *dyApplyRawFuncSigRPtr(dyt obj, dyt fcn, char *sig, va_list lst)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	double v;
	int i;

	ty=dygettype(fcn);
	buf=gcralloc(256);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(0);
	}

	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig, buf, sig, lst);
		if(i<0)return(0);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		return(buf);
	}

	if(!strcmp(ty, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, lst);
		if(i<0)return(0);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		return(buf);
	}

	if(!strcmp(ty, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, lst);
		if(i<0)return(0);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		return(buf);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackArgsSigArray(sig, lst, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		*(dyt *)buf=p;
		return(buf);
	}

	dySetStatus("badMethodType");
	return(NULL);
}

// dyt dyApplyRawFuncUSig(dyt obj, dyt fcn, char *sig, void **args)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	int i;

	ty=dygettype(fcn);
	buf=gcralloc(256);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(UNDEFINED);
	}

//	if(dytypep(fcn, "_raw_func_t"))
	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig, buf, sig, args);
		if(i<0)return(UNDEFINED);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

//	if(dytypep(fcn, "_raw_close_t"))
	if(!strcmp(ty, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, args);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

//	if(dytypep(fcn, "_raw_method_t"))
	if(!strcmp(ty, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, args);
		if(i<0)return(UNDEFINED);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		p=dyllExtractVariantBufferSig(esig, buf);
		return(p);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackUArgsSigArray(sig, args, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		return(p);
	}

	dySetStatus("badMethodType");
	return(UNDEFINED);
}

// s64 dyApplyRawFuncUSigLong(dyt obj, dyt fcn, char *sig, void **args)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	s64 v;
	int i;

	ty=dygettype(fcn);
	buf=gcralloc(256);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(0);
	}

//	if(dytypep(fcn, "_raw_func_t"))
	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig, buf, sig, args);
		if(i<0)return(0);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		v=dyllExtractLongBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_close_t"))
	if(!strcmp(ty, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, args);
		if(i<0)return(0);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractLongBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_method_t"))
	if(!strcmp(ty, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, args);
		if(i<0)return(0);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractLongBufferSig(esig, buf);
		return(v);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackUArgsSigArray(sig, args, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		return(dylongv(p));
	}

	dySetStatus("badMethodType");
	return(0);
}

// double dyApplyRawFuncUSigReal(dyt obj, dyt fcn, char *sig, void **args)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	double v;
	int i;

	ty=dygettype(fcn);
	buf=gcralloc(256);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(0);
	}

//	if(dytypep(fcn, "_raw_func_t"))
	if(!strcmp(ty, "_raw_func_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig, buf, sig, args);
		if(i<0)return(0);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		v=dyllExtractRealBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_close_t"))
	if(!strcmp(ty, "_raw_close_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, args);
		if(i<0)return(0);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractRealBufferSig(esig, buf);
		return(v);
	}

//	if(dytypep(fcn, "_raw_method_t"))
	if(!strcmp(ty, "_raw_method_t"))
	{
		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, args);
		if(i<0)return(0);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		v=dyllExtractRealBufferSig(esig, buf);
		return(v);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackUArgsSigArray(sig, args, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		return(dydoublev(p));
	}

	dySetStatus("badMethodType");
	return(0);
}

// void *dyApplyRawFuncUSigRPtr(dyt obj, dyt fcn, char *sig, void **args)
{
	byte *buf;
	BGBGC_RawFunc *inf;
	char *esig, *ty;
	dyt p;
	int i;

	ty=dygettype(fcn);

	if(!ty)
	{
		dySetStatus("badMethodType");
		return(UNDEFINED);
	}

//	if(dytypep(fcn, "_raw_func_t"))
	if(!strcmp(ty, "_raw_func_t"))
	{
		buf=gcralloc(256);

		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig, buf, sig, args);
		if(i<0)return(UNDEFINED);
		BASM_CallSig(inf->fcn, inf->sig, buf, buf);
		return(buf);
	}

//	if(dytypep(fcn, "_raw_close_t"))
	if(!strcmp(ty, "_raw_close_t"))
	{
		buf=gcralloc(256);

		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+sizeof(void *), sig, args);
		if(i<0)return(UNDEFINED);
		*(void **)buf=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		return(buf);
	}

//	if(dytypep(fcn, "_raw_method_t"))
	if(!strcmp(ty, "_raw_method_t"))
	{
		buf=gcralloc(256);

		inf=fcn;
		i=dyllRepackBufSigUArgs(inf->sig, &esig,
			buf+2*sizeof(void *), sig, args);
		if(i<0)return(UNDEFINED);

		((void **)buf)[0]=obj;
		((void **)buf)[1]=inf->data;

		BASM_CallSig(inf->fcn, inf->sig2, buf, buf);
		return(buf);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		i=dyllUnpackUArgsSigArray(sig, args, (dyt *)buf);
		p=dyApplyMethod(obj, fcn, (dyt *)buf, i);
		return(p);
	}

	dySetStatus("badMethodType");
	return(UNDEFINED);
}

#endif

#else

// dyt dyApplyRawFunc(dyt obj, dyt fcn, dyt *args, int nargs)
//	{ return(UNDEFINED); }
// dyt dyApplyRawFuncSig(dyt obj, dyt fcn, char *sig, va_list lst)
//	{ return(UNDEFINED); }

// s64 dyApplyRawFuncSigLong(dyt obj, dyt fcn, char *sig, va_list lst)
//	{ return(0); }
// double dyApplyRawFuncSigReal(dyt obj, dyt fcn, char *sig, va_list lst)
//	{ return(0); }

#endif
