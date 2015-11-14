#include <bgbgc.h>
#include <bgbdy.h>

dyt dyFunc(dyt (*fcn)(dyt *args, int nargs))
{
	BGBGC_Func *tmp;
	tmp=gctalloc("_func_t", sizeof(BGBGC_Func));
	tmp->nargs=10;
	tmp->fcn.fa=fcn;
	tmp->data=UNDEFINED;
	return((dyt)tmp);
}

dyt dyMethod(dyt (*fcn)(dyt self, dyt *args, int nargs))
{
	BGBGC_Method *tmp;

	tmp=gctalloc("_method_t", sizeof(BGBGC_Method));
	tmp->nargs=10;
	tmp->fcn.fa=fcn;
	tmp->data=UNDEFINED;
	return((dyt)tmp);
}

dyt dyFuncData(dyt data, dyt (*fcn)(dyt data, dyt *args, int nargs))
{
	BGBGC_Func *tmp;
	tmp=gctalloc("_func_t", sizeof(BGBGC_Func));
	tmp->nargs=10;
	tmp->fcn.fad=fcn;
	tmp->data=data;
	return((dyt)tmp);
}

dyt dyMethodData(dyt data, dyt (*fcn)(dyt self, dyt data, dyt *args, int nargs))
{
	BGBGC_Method *tmp;

	tmp=gctalloc("_method_t", sizeof(BGBGC_Method));
	tmp->nargs=10;
	tmp->fcn.fad=fcn;
	tmp->data=data;
	return((dyt)tmp);
}

dyt dyStubMethod(dyt data, dyt method,
	void *(*fcn)(BGBGC_StubMethod *inf, dyt self, char *sig,
		void **args, int szargs))
{
	BGBGC_StubMethod *tmp;
	tmp=gctalloc("_stubmethod_t", sizeof(BGBGC_StubMethod));
	tmp->nargs=10;
	tmp->fcn=fcn;
	tmp->method=method;
	tmp->data=data;
	return((dyt)tmp);
}

dyt dyFunc_NN(void *fcn, int n)
{
	BGBGC_Func *tmp;
	tmp=gctalloc("_func_t", sizeof(BGBGC_Func));
	tmp->nargs=n;
//	((void *)tmp->fcn.f0)=fcn;
	tmp->fcn.f0=fcn;
	tmp->data=UNDEFINED;
	return((dyt)tmp);
}

dyt dyMethod_NN(void *fcn, int n)
{
	BGBGC_Method *tmp;
	tmp=gctalloc("_method_t", sizeof(BGBGC_Method));
	tmp->nargs=n;
//	((void *)tmp->fcn.f0)=fcn;
	tmp->fcn.f0=fcn;
	tmp->data=UNDEFINED;
	return((dyt)tmp);
}

dyt dyFunc_NND(void *fcn, int n, void *data)
{
	BGBGC_Func *tmp;
	tmp=gctalloc("_func_t", sizeof(BGBGC_Func));
	tmp->nargs=n;
//	((void *)tmp->fcn.f0)=fcn;
	tmp->fcn.f0=fcn;
	tmp->data=data;
	return((dyt)tmp);
}

dyt dyMethod_NND(void *fcn, int n, void *data)
{
	BGBGC_Method *tmp;
	tmp=gctalloc("_method_t", sizeof(BGBGC_Method));
	tmp->nargs=n;
//	((void *)tmp->fcn.f0)=fcn;
	tmp->fcn.f0=fcn;
	tmp->data=data;
	return((dyt)tmp);
}

dyt dyFunc0(dyt (*fcn)())
	{ return(dyFunc_NN((void *)fcn, 0)); }
dyt dyFunc1(dyt (*fcn)(dyt))
	{ return(dyFunc_NN((void *)fcn, 1)); }
dyt dyFunc2(dyt (*fcn)(dyt, dyt))
	{ return(dyFunc_NN((void *)fcn, 2)); }
dyt dyFunc3(dyt (*fcn)(dyt, dyt, dyt))
	{ return(dyFunc_NN((void *)fcn, 3)); }
dyt dyFunc4(dyt (*fcn)(dyt, dyt, dyt, dyt))
	{ return(dyFunc_NN((void *)fcn, 4)); }
dyt dyFunc5(dyt (*fcn)(dyt, dyt, dyt, dyt, dyt))
	{ return(dyFunc_NN((void *)fcn, 5)); }
dyt dyFunc6(dyt (*fcn)(dyt, dyt, dyt, dyt, dyt, dyt))
	{ return(dyFunc_NN((void *)fcn, 6)); }
dyt dyFunc7(dyt (*fcn)(dyt, dyt, dyt, dyt, dyt, dyt, dyt))
	{ return(dyFunc_NN((void *)fcn, 7)); }
dyt dyFunc8(dyt (*fcn)(dyt, dyt, dyt, dyt, dyt, dyt, dyt, dyt))
	{ return(dyFunc_NN((void *)fcn, 8)); }

dyt dyMethod0(dyt (*fcn)(dyt self))
	{ return(dyMethod_NN((void *)fcn, 0)); }
dyt dyMethod1(dyt (*fcn)(dyt self, dyt))
	{ return(dyMethod_NN((void *)fcn, 1)); }
dyt dyMethod2(dyt (*fcn)(dyt self, dyt, dyt))
	{ return(dyMethod_NN((void *)fcn, 2)); }
dyt dyMethod3(dyt (*fcn)(dyt self, dyt, dyt, dyt))
	{ return(dyMethod_NN((void *)fcn, 3)); }
dyt dyMethod4(dyt (*fcn)(dyt self, dyt, dyt, dyt, dyt))
	{ return(dyMethod_NN((void *)fcn, 4)); }
dyt dyMethod5(dyt (*fcn)(dyt self, dyt, dyt, dyt, dyt, dyt))
	{ return(dyMethod_NN((void *)fcn, 5)); }
dyt dyMethod6(dyt (*fcn)(dyt self, dyt, dyt, dyt, dyt, dyt, dyt))
	{ return(dyMethod_NN((void *)fcn, 6)); }
dyt dyMethod7(dyt (*fcn)(dyt self, dyt, dyt, dyt, dyt, dyt, dyt, dyt))
	{ return(dyMethod_NN((void *)fcn, 7)); }
dyt dyMethod8(dyt (*fcn)(dyt self, dyt, dyt, dyt, dyt, dyt, dyt, dyt, dyt))
	{ return(dyMethod_NN((void *)fcn, 8)); }

dyt dyMethodN(dyt (*fcn)(dyt self, dyt))
	{ return(dyMethod_NN((void *)fcn, -1)); }
dyt dyMethod1N(dyt (*fcn)(dyt self, dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -2)); }
dyt dyMethod2N(dyt (*fcn)(dyt self, dyt,dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -3)); }
dyt dyMethod3N(dyt (*fcn)(dyt self, dyt,dyt,dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -4)); }
dyt dyMethod4N(dyt (*fcn)(dyt self, dyt,dyt,dyt,dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -5)); }
dyt dyMethod5N(dyt (*fcn)(dyt self, dyt,dyt,dyt,dyt,dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -6)); }
dyt dyMethod6N(dyt (*fcn)(dyt self, dyt,dyt,dyt,dyt,dyt,dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -7)); }
dyt dyMethod7N(dyt (*fcn)(dyt self, dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -8)); }
dyt dyMethod8N(dyt (*fcn)(dyt self, dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt))
	{ return(dyMethod_NN((void *)fcn, -9)); }

BGBDY_API dyt dyApplyMethod0(dyt obj, dyt mth)
{
	dyt ar[8];
	return(dyApplyMethod(obj, mth, ar, 0));
}

BGBDY_API dyt dyApplyMethod1(dyt obj, dyt mth, dyt a)
{
	dyt ar[8]; ar[0]=a;
	return(dyApplyMethod(obj, mth, ar, 1));
}

BGBDY_API dyt dyApplyMethod2(dyt obj, dyt mth, dyt a, dyt b)
{
	dyt ar[8]; ar[0]=a; ar[1]=b;
	return(dyApplyMethod(obj, mth, ar, 2));
}

BGBDY_API dyt dyApplyMethod3(dyt obj, dyt mth, dyt a, dyt b, dyt c)
{
	dyt ar[8]; ar[0]=a; ar[1]=b; ar[2]=c;
	return(dyApplyMethod(obj, mth, ar, 3));
}

BGBDY_API dyt dyApplyMethod4(dyt obj, dyt mth, dyt a, dyt b, dyt c, dyt d)
{
	dyt ar[8]; ar[0]=a; ar[1]=b; ar[2]=c; ar[3]=d;
	return(dyApplyMethod(obj, mth, ar, 4));
}


BGBDY_API dytf dyApplyMethodB(dyt obj, dyt mth, dytf *args, int nargs)
{
	dyt arr[16];
	BGBGC_RawFunc *rinf;
	char *ty;
	void *pv;
	dyt p;
	int i;

	ty=dygettype(mth);
	if(!ty)return(DYTF_UNDEFINED);

	if(!strcmp(ty, "_raw_func_t"))
	{
		rinf=(BGBGC_RawFunc *)mth;
		pv=dyllApplyRawFuncB(rinf, obj, args, nargs);
		if(!pv || (pv==UNDEFINED))
			return(DYTF_UNDEFINED);
		p=dyllExtractVariantBufferSig(rinf->esig, pv);
		return(dytfWrap(p));
	}

	for(i=0; i<nargs; i++)
		arr[i]=dytfUnwrap(args[i]);
	return(dytfWrap(dyApplyMethod(obj, mth, arr, nargs)));
}

BGBDY_API dyt dyApplyMethod(dyt obj, dyt mth, dyt *args, int nargs)
{
	BGBGC_RawFunc *rinf;
	BGBGC_Func *finf;
	BGBGC_Method *minf;
	BGBGC_StubMethod *sinf;
	char *ty;
	void *pv;
	dyt p;
	int i, j;

	ty=dygettype(mth);
	if(!ty)return(UNDEFINED);

	if(!strcmp(ty, "_raw_func_t"))
	{
		rinf=(BGBGC_RawFunc *)mth;
		pv=dyllApplyRawFuncD(rinf, obj, args, nargs);
		if(!pv || (pv==UNDEFINED))
			return(UNDEFINED);
		p=dyllExtractVariantBufferSig(rinf->esig, pv);
		return(p);
	}

	if(!strcmp(ty, "_func_t"))
	{
		finf=(BGBGC_Func *)mth;

		if(finf->data!=UNDEFINED)
		{
			if(finf->nargs==10)
				return(finf->fcn.fad(finf->data, args, nargs));

			for(i=nargs; i>0; i++)
				args[i]=args[i-1];
			args[0]=finf->data;
			nargs++;
		}

		if(finf->nargs==10)
		{
			dyoPushThis2(obj, &j);
			p=finf->fcn.fa(args, nargs);
			dyoPopThis2(j);
			return(p);
		}
		if(finf->nargs>=0)
		{
			if(finf->nargs!=nargs)
				return(UNDEFINED);

			dyoPushThis2(obj, &j);
			switch(nargs)
			{
			case 0: p=finf->fcn.f0(); break;
			case 1: p=finf->fcn.f1(args[0]); break;
			case 2: p=finf->fcn.f2(args[0], args[1]); break;
			case 3: p=finf->fcn.f3(args[0], args[1], args[2]);
				break;
			case 4: p=finf->fcn.f4(args[0], args[1], args[2],
				args[3]); break;
			case 5: p=finf->fcn.f5(args[0], args[1], args[2],
				args[3], args[4]); break;
			case 6: p=finf->fcn.f6(args[0], args[1], args[2],
				args[3], args[4], args[5]); break;
			case 7: p=finf->fcn.f7(args[0], args[1], args[2],
				args[3], args[4], args[5], args[6]); break;
			case 8: p=finf->fcn.f8(args[0], args[1], args[2],
				args[3], args[4], args[5], args[6], args[7]);
				break;
			default: p=UNDEFINED;
			}
			dyoPopThis2(j);
			return(p);
		}

		i=(-finf->nargs)-1;
		if(nargs<i)
			return(UNDEFINED);

		p=dyWrapArray(args+i, nargs-i);

		dyoPushThis2(obj, &j);
		switch(finf->nargs)
		{
		case -1: p=finf->fcn.f1(p); break;
		case -2: p=finf->fcn.f2(args[0], p); break;
		case -3: p=finf->fcn.f3(args[0], args[1], p); break;
		case -4: p=finf->fcn.f4(args[0], args[1], args[2], p);
			break;
		case -5: p=finf->fcn.f5(args[0], args[1], args[2],
			args[3], p); break;
		case -6: p=finf->fcn.f6(args[0], args[1], args[2],
			args[3], args[4], p); break;
		case -7: p=finf->fcn.f7(args[0], args[1], args[2],
			args[3], args[4], args[5], p); break;
		case -8: p=finf->fcn.f8(args[0], args[1], args[2],
			args[3], args[4], args[5], args[6], p); break;
		default: p=UNDEFINED;
		}
		dyoPopThis2(j);

		return(p);
	}

	if(!strcmp(ty, "_method_t"))
	{
		minf=(BGBGC_Method *)mth;

		if(minf->data!=UNDEFINED)
		{
			if(minf->nargs==10)
				return(minf->fcn.fad(obj, minf->data,
					args, nargs));

			for(i=nargs; i>0; i++)
				args[i]=args[i-1];
			args[0]=minf->data;
			nargs++;
		}

		if(minf->nargs==10)
			return(minf->fcn.fa(obj, args, nargs));
		if(minf->nargs>=0)
		{
			if(minf->nargs!=nargs)
				return(UNDEFINED);
			switch(nargs)
			{
			case 0: p=minf->fcn.f0(obj); break;
			case 1: p=minf->fcn.f1(obj, args[0]); break;
			case 2: p=minf->fcn.f2(obj, args[0], args[1]); break;
			case 3: p=minf->fcn.f3(obj, args[0], args[1], args[2]);
				break;
			case 4: p=minf->fcn.f4(obj, args[0], args[1], args[2],
				args[3]); break;
			case 5: p=minf->fcn.f5(obj, args[0], args[1], args[2],
				args[3], args[4]); break;
			case 6: p=minf->fcn.f6(obj, args[0], args[1], args[2],
				args[3], args[4], args[5]); break;
			case 7: p=minf->fcn.f7(obj, args[0], args[1], args[2],
				args[3], args[4], args[5], args[6]); break;
			case 8: p=minf->fcn.f8(obj, args[0], args[1], args[2],
				args[3], args[4], args[5], args[6], args[7]);
				break;
			default: p=UNDEFINED;
			}
			return(p);
		}

		i=(-minf->nargs)-1;
		if(nargs<i)
			return(UNDEFINED);

		p=dyWrapArray(args+i, nargs-i);

		switch(minf->nargs)
		{
		case -1: p=minf->fcn.f1(obj, p); break;
		case -2: p=minf->fcn.f2(obj, args[0], p); break;
		case -3: p=minf->fcn.f3(obj, args[0], args[1], p); break;
		case -4: p=minf->fcn.f4(obj, args[0], args[1], args[2], p);
			break;
		case -5: p=minf->fcn.f5(obj, args[0], args[1], args[2],
			args[3], p); break;
		case -6: p=minf->fcn.f6(obj, args[0], args[1], args[2],
			args[3], args[4], p); break;
		case -7: p=minf->fcn.f7(obj, args[0], args[1], args[2],
			args[3], args[4], args[5], p); break;
		case -8: p=minf->fcn.f8(obj, args[0], args[1], args[2],
			args[3], args[4], args[5], args[6], p); break;
		default: p=UNDEFINED;
		}
		return(p);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		p=sinf->fcn(sinf, obj, NULL, (void **)args, nargs);
		return(p);
	}

//	if(dyRawFuncP(mth))
//	{
//		dyoPushThis2(obj, &j);
//		p=dyApplyRawFunc(obj, mth, args, nargs);
//		dyoPopThis2(j);
//		return(p);
//	}

	if(BGBDY_CanApplyObjP(mth))
	{
		p=BGBDY_ApplyObj(mth, obj, (void **)args, nargs);
		return(p);
	}

	if(BGBDY_CanApplyP(mth))
	{
		dyoPushThis2(obj, &j);
		p=BGBDY_Apply(mth, (void **)args, nargs);
		dyoPopThis2(j);
		return(p);
	}

	dySetStatus("badMethodType");
	return(UNDEFINED);
}

BGBDY_API dyt dyApplyMethodLst(dyt obj, dyt mth, dyt args)
{
	dyt ar[16];
	dyt *par;
	dyt c;
	int i, j, k;

	if(!args)
	{
		return(dyApplyMethod(obj, mth, ar, 0));
	}

	if(dyarrayp(args))
	{
		par=dyarrayv(args);
		i=dyarraysz(args);
		return(dyApplyMethod(obj, mth, par, i));
	}

	if(dyconsp(args))
	{
		i=0; c=args;
		while(dyconsp(c))
		{
			ar[i++]=dycar(c);
			c=dycdr(c);
		}

		if(c && dyarrayp(c))
		{
			par=dyarrayv(c);
			k=dyarraysz(c);
			for(j=0; j<k; j++)
				ar[i++]=par[j];
		}

		return(dyApplyMethod(obj, mth, ar, i));
	}

	return(UNDEFINED);
}

BGBDY_API void *dyApplyMethodU(dyt obj, dyt mth,
	char *sig, void **args, int szargs)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyRawFuncU((BGBGC_RawFunc *)mth, obj, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		pv=sinf->fcn(sinf, obj, sig, args, szargs);
		return(pv);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackUArgsSigArray(sig, args, abuf);
		p=dyApplyMethod(obj, mth, abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

//	if(dyRawFuncP(mth))
//	{
//		dyoPushThis2(obj, &j);
//		pv=dyApplyRawFuncUSigRPtr(obj, mth, sig, args);
//		dyoPopThis2(j);
//		return(pv);
//	}

	if(BGBDY_CanApplyObjP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackUArgsSigArray(sig, args, abuf);
		p=BGBDY_ApplyObj(mth, obj, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	if(BGBDY_CanApplyP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackUArgsSigArray(sig, args, abuf);
		p=BGBDY_Apply(mth, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	dySetStatus("badMethodType");
	return(UNDEFINED);
}

BGBDY_API void *dyApplyMethodV(dyt obj, dyt mth,
	char *sig, va_list args)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyRawFuncV((BGBGC_RawFunc *)mth, obj, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigUArgs(sig, args, (void **)abuf);
		pv=sinf->fcn(sinf, obj, sig, (void **)abuf, i);
		gcfree(abuf);
		return(pv);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigArray(sig, args, abuf);
		p=dyApplyMethod(obj, mth, abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

//	if(dyRawFuncP(mth))
//	{
//		pv=dyApplyRawFuncSigRPtr(obj, mth, sig, args);
//		return(pv);
//	}

	if(BGBDY_CanApplyObjP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigArray(sig, args, abuf);
		p=BGBDY_ApplyObj(mth, obj, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	if(BGBDY_CanApplyP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigArray(sig, args, abuf);
		p=BGBDY_Apply(mth, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	printf("bad method type %s\n", ty);

	*(int *)-1=-1;

	dySetStatus("badMethodType");
	return(UNDEFINED);
}

BGBDY_API void *dyApplyMethodSig(dyt obj, dyt mth,
	char *sig, ...)
{
	va_list lst;
	void *p;

	va_start(lst, sig);
	p=dyApplyMethodV(obj, mth, sig, lst);
	va_end(lst);
	return(p);
}


BGBDY_API void *dycApplyMethodU(dycObject obj, dyt mth,
	char *sig, void **args, int szargs)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyObjRawFuncU((BGBGC_RawFunc *)mth, obj, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		pv=sinf->fcn(sinf, (dyt)obj, sig, args, szargs);
		return(pv);
	}

	return(dyApplyMethodU((dyt)obj, mth, sig, args, szargs));
}

BGBDY_API void *dycApplyMethodV(dycObject obj, dyt mth,
	char *sig, va_list args)
{
	BGBGC_StubMethod *sinf;
	BGBGC_RawFunc *rinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyObjRawFuncV((BGBGC_RawFunc *)mth, obj, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigUArgs(sig, args, (void **)abuf);
		pv=sinf->fcn(sinf, (dyt)obj, sig, (void **)abuf, i);
		gcfree(abuf);
		return(pv);
	}

	return(dyApplyMethodV((dyt)obj, mth, sig, args));
}

BGBDY_API void *dycApplyStaticU(dycClass inf, dyt mth,
	char *sig, void **args, int szargs)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyRawFuncU((BGBGC_RawFunc *)mth, (dyt)inf, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		pv=sinf->fcn(sinf, (dyt)inf, sig, args, szargs);
		return(pv);
	}

	return(dyApplyMethodU((dyt)inf, mth, sig, args, szargs));
}

BGBDY_API void *dycApplyStaticV(dycClass inf, dyt mth,
	char *sig, va_list args)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyRawFuncV((BGBGC_RawFunc *)mth, (dyt)inf, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigUArgs(sig, args, (void **)abuf);
		pv=sinf->fcn(sinf, (dyt)inf, sig, (void **)abuf, i);
		gcfree(abuf);
		return(pv);
	}

	return(dyApplyMethodV((dyt)inf, mth, sig, args));
}


BGBDY_API void *dycApplyStructMethodU(void *obj, dyt mth,
	char *sig, void **args, int szargs)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyRawFuncU((BGBGC_RawFunc *)mth, obj, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		pv=sinf->fcn(sinf, obj, sig, args, szargs);
		return(pv);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackUArgsSigArray(sig, args, abuf);
		p=dyApplyMethod(obj, mth, abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

//	if(dyRawFuncP(mth))
//	{
//		dyoPushThis2(obj, &j);
//		pv=dyApplyRawFuncUSigRPtr(obj, mth, sig, args);
//		dyoPopThis2(j);
//		return(pv);
//	}

	if(BGBDY_CanApplyObjP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackUArgsSigArray(sig, args, abuf);
		p=BGBDY_ApplyObj(mth, obj, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	if(BGBDY_CanApplyP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackUArgsSigArray(sig, args, abuf);
		p=BGBDY_Apply(mth, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	dySetStatus("badMethodType");
	return(UNDEFINED);
}

BGBDY_API void *dycApplyStructMethodV(void *obj, dyt mth,
	char *sig, va_list args)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	ty=dygettype(mth);

	if(!strcmp(ty, "_raw_func_t"))
	{
		pv=dyllApplyRawFuncV((BGBGC_RawFunc *)mth, obj, sig, args);
		return(pv);
	}

	if(!strcmp(ty, "_stubmethod_t"))
	{
		sinf=(BGBGC_StubMethod *)mth;
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigUArgs(sig, args, (void **)abuf);
		pv=sinf->fcn(sinf, obj, sig, (void **)abuf, i);
		gcfree(abuf);
		return(pv);
	}

	if(!strcmp(ty, "_func_t") || !strcmp(ty, "_method_t"))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigArray(sig, args, abuf);
		p=dyApplyMethod(obj, mth, abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

//	if(dyRawFuncP(mth))
//	{
//		pv=dyApplyRawFuncSigRPtr(obj, mth, sig, args);
//		return(pv);
//	}

	if(BGBDY_CanApplyObjP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigArray(sig, args, abuf);
		p=BGBDY_ApplyObj(mth, obj, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	if(BGBDY_CanApplyP(mth))
	{
		abuf=gcalloc(32*sizeof(dyt));
		i=dyllUnpackArgsSigArray(sig, args, abuf);
		p=BGBDY_Apply(mth, (void **)abuf, i);
		gcfree(abuf);

		pv=gcralloc(16);
		s=dyllSigRet(sig);
		dyllSetVarSig(pv, s, p);
		return(pv);
	}

	printf("bad method type %s\n", ty);

	*(int *)-1=-1;

	dySetStatus("badMethodType");
	return(UNDEFINED);
}


void *bgbdyc_rawfunc_ApplyU(void *obj,
	dyt mth, char *sig, void **args, int szargs)
{
	return(dyllApplyObjRawFuncU((BGBGC_RawFunc *)mth, obj, sig, args));
}

void *bgbdyc_stubmethod_ApplyU(void *obj,
	dyt mth, char *sig, void **args, int szargs)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	sinf=(BGBGC_StubMethod *)mth;
	pv=sinf->fcn(sinf, (dyt)obj, sig, args, szargs);
	return(pv);
}

void *bgbdyc_rawfunc_ApplyV(void *obj,
	dyt mth, char *sig, va_list args)
{
	return(dyllApplyObjRawFuncV((BGBGC_RawFunc *)mth, obj, sig, args));
}

void *bgbdyc_stubmethod_ApplyV(void *obj,
	dyt mth, char *sig, va_list args)
{
	BGBGC_StubMethod *sinf;
	void *pv;
	dyt *abuf;
	int i;

	sinf=(BGBGC_StubMethod *)mth;
	abuf=gcalloc(32*sizeof(dyt));
	i=dyllUnpackArgsSigUArgs(sig, args, (void **)abuf);
	pv=sinf->fcn(sinf, (dyt)obj, sig, (void **)abuf, i);
	gcfree(abuf);
	return(pv);
}

BGBDYC_ApplyU_t dycGetMethodApplyU(dyt mth)
{
	char *ty;

	ty=dygettype(mth);
	if(!strcmp(ty, "_raw_func_t"))
	{
		return(dyllGetRawFuncApplyU((BGBGC_RawFunc *)mth));
//		return(bgbdyc_rawfunc_ApplyU);
	}
	if(!strcmp(ty, "_stubmethod_t"))
		return(bgbdyc_stubmethod_ApplyU);
	return((BGBDYC_ApplyU_t)dycApplyMethodU);
}

BGBDYC_ApplyV_t dycGetMethodApplyV(dyt mth)
{
	char *ty;

	ty=dygettype(mth);
	if(!strcmp(ty, "_raw_func_t"))
	{
		return(dyllGetRawFuncApplyV((BGBGC_RawFunc *)mth));
//		return(bgbdyc_rawfunc_ApplyV);
	}
	if(!strcmp(ty, "_stubmethod_t"))
		return(bgbdyc_stubmethod_ApplyV);
	return((BGBDYC_ApplyV_t)dycApplyMethodV);
}



typedef struct {
dyt obj, mth;
char *sig;
}DYLL_ConvObjCFunc_data;

void *DYLL_ConvObjCFunc_Ip(void *data, ...)
{
	DYLL_ConvObjCFunc_data *inf;
	va_list lst;
	void *p;

	inf=(DYLL_ConvObjCFunc_data *)data;
	va_start(lst, data);
	p=dyApplyMethodV(inf->obj, inf->mth, inf->sig, lst);
	va_end(lst);
	return(*(void **)p);
}

s64 DYLL_ConvObjCFunc_Il(void *data, ...)
{
	DYLL_ConvObjCFunc_data *inf;
	va_list lst;
	void *p;

	inf=(DYLL_ConvObjCFunc_data *)data;
	va_start(lst, data);
	p=dyApplyMethodV(inf->obj, inf->mth, inf->sig, lst);
	va_end(lst);
	return(*(s64 *)p);
}

double DYLL_ConvObjCFunc_Id(void *data, ...)
{
	DYLL_ConvObjCFunc_data *inf;
	va_list lst;
	void *p;

	inf=(DYLL_ConvObjCFunc_data *)data;
	va_start(lst, data);
	p=dyApplyMethodV(inf->obj, inf->mth, inf->sig, lst);
	va_end(lst);
	return(*(double *)p);
}

BGBDY_API void *dyllConvObjCFunc(dyt obj, char *sig)
{
	DYLL_ConvObjCFunc_data *inf;
	BGBGC_RawFunc *rinf;
	BGBGC_StubMethod *sinf;

	void *pv;
	dyt *abuf;
	dyt p;
	char *s, *ty;
	int i, j;

	BGBGC_InitDyLL();

	ty=dygettype(obj);

	if(!strcmp(ty, "_raw_func_t"))
	{
		rinf=(BGBGC_RawFunc *)obj;
		return(rinf->fcn);
	}

#if 1
	s=dyllSigRet(sig);
	inf=gcalloc(sizeof(DYLL_ConvObjCFunc_data));
	inf->obj=NULL;
	inf->mth=obj;
	inf->sig=dystrdup(sig);
	pv=DYLL_ConvObjCFunc_Ip;
	if(!strcmp(s, "i") || !strcmp(s, "j") ||
		!strcmp(s, "l") || !strcmp(s, "m") ||
		!strcmp(s, "x") || !strcmp(s, "y"))
			pv=DYLL_ConvObjCFunc_Il;
	if(!strcmp(s, "f") || !strcmp(s, "d"))
			pv=DYLL_ConvObjCFunc_Id;
	p=dyllWrapClosure(pv, inf, sig);
#endif

	return(NULL);
}
