#include <bgbsvm.h>

dytf bsvm_toplevel;
int bsvm_opcnt;

BGBSVM_API void BSVM_AddBuiltin(char *name, char *desc,
	dytf (*fcn)(BSVM_SVMState *ctx, dytf args), int nargs)
{
	BSVM_SVMBuiltin *bfcn;
	dytf tmp, obj;
	char *s, *t;

	obj=bsvm_toplevel;

	s=name; t=s;
	while(*t)
	{
		while(*t && (*t!='.'))t++;
		if(*t=='.') { t++; s=t; }
	}

	bfcn=gctalloc("svm_builtin_t", sizeof(BSVM_SVMBuiltin));

	bfcn->name=strdup(s);
	bfcn->desc=strdup(desc);
	bfcn->fcn=fcn;
	bfcn->nargs=nargs;

	dytfBind(obj, dystrsym(s), dytfPtrF(bfcn));
}

BGBSVM_API void BSVM_AddBuiltin2(char *name, char *desc,
	dytf (*fcn)(BSVM_SVMState *ctx, dytf args), int nargs)
{
	BSVM_SVMBuiltin *bfcn;
	dytf tmp, obj;
	char *s, *t;

	obj=bsvm_toplevel;

	s=gcrstrdup(name); t=s;
	while(*t)
	{
		while(*t && (*t!='.'))t++;
		if(*t=='.')
		{
			*t++=0;

			tmp=dytfGet(obj, dystrsym(s));
//			if(tmp==NULL)
			if(dytfNullP(tmp) || dytfUndefinedP(tmp))
			{
				tmp=dytfObject();
				dytfBind(obj, dystrsym(s), tmp);
			}

			obj=tmp; s=t;
		}
	}

	bfcn=gctalloc("svm_builtin_t", sizeof(BSVM_SVMBuiltin));

	bfcn->name=strdup(s);
	bfcn->desc=strdup(desc);
	bfcn->fcn=fcn;
	bfcn->nargs=nargs;

	dytfBind(obj, dystrsym(s), dytfPtrF(bfcn));
}

BGBSVM_API void BSVM_AddBinding(char *name, dytf val)
{
	BSVM_SVMBuiltin *bfcn;
	dytf tmp, obj;
	char *s, *t;

	obj=bsvm_toplevel;

	s=name; t=s;
	while(*t)
	{
		while(*t && (*t!='.'))t++;
		if(*t=='.')
		{
			t++;
			s=t;
		}
	}

	dytfBind(obj, dystrsym(s), val);
}

BGBSVM_API void BSVM_AddBinding2(char *name, dytf val)
{
	BSVM_SVMBuiltin *bfcn;
	dytf tmp, obj;
	char *s, *t;

	obj=bsvm_toplevel;

	s=gcrstrdup(name); t=s;
	while(*t)
	{
		while(*t && (*t!='.'))t++;
		if(*t=='.')
		{
			*t++=0;

			tmp=dytfGet(obj, dystrsym(s));
//			if(tmp==NULL)
			if(dytfNullP(tmp) || dytfUndefinedP(tmp))
			{
				tmp=dytfObject();
				dytfBind(obj, dystrsym(s), tmp);
			}

			obj=tmp; s=t;
		}
	}

	dytfBind(obj, dystrsym(s), val);
}


BGBSVM_API dytf BSVM_GetTop(char *name)
{
	dytf tmp, obj;
	char *s, *t;

	obj=bsvm_toplevel;
	s=gcrstrdup(name); t=s;
	while(*t)
	{
		while(*t && (*t!='.') && (*t!='/'))t++;
		if((*t=='.') || (*t=='/'))
		{
			*t++=0;

			tmp=dytfGet(obj, dystrsym(s));
//			if(!tmp)
			if(dytfNullP(tmp) || dytfUndefinedP(tmp))
				return(DYTF_NULL);
			obj=tmp; s=t;
		}
	}

	tmp=dytfGet(obj, dystrsym(s));
	return(tmp);
}

BGBSVM_API void BSVM_SetTop(char *name, dytf val)
{
	char pn[256];
	dytf tmp, obj;
	char *s, *t;

	pn[0]=0;

	obj=bsvm_toplevel;
	s=gcrstrdup(name); t=s;
	while(*t)
	{
		while(*t && (*t!='.'))t++;
		if(*t=='.')
		{
			*t++=0;

			if(pn[0])strcat(pn, "/");
			strcat(pn, s);

			tmp=dytfGet(obj, dystrsym(s));
//			if(!tmp)
			if(dytfNullP(tmp) || dytfUndefinedP(tmp))
			{			
				tmp=dytfObject();
				dytfBind(obj, dystrsym(s), tmp);

				dytfBind(tmp, dystrsym("_pkgname_"), dytfString(pn));
			}

			obj=tmp; s=t;
		}
	}

	dytfBind(obj, dystrsym(s), val);
}

dytf BSVM_LookupCTop(BSVM_SVMState *ctx, dytf key)
{
	dytf t;

//	t=dyllGetFuncAddrObj(key);
	t=dytfWrap(dyllGetDynamic(dytfSymbolvF(key)));
	if(!dytfNullP(t) && !dytfUndefinedP(t))
		return(t);
	return(DYTF_UNDEFINED);
}

int BSVM_VarInfoP(dytf obj)
{
	return(dytftypep(obj, "svm_varinfo_t"));
}

int BSVM_LookupCheckVarInfo(BSVM_SVMState *ctx, dytf info, dytf key)
{
	BSVM_SVMVarInfo *inf;

	if(dytfEqqP(info, key))
		return(true);

	if(dytfSymbolP(info) || dytfKeywordP(info))
	{
		if(!strcmp(dytfSymbolv(key), dytfSymbolv(info)))
			return(true);
		return(false);
	}
		
	if(BSVM_VarInfoP(info))
	{
		inf=(BSVM_SVMVarInfo *)dytfPtrvF(info);
		if(!strcmp(dytfSymbolv(key), inf->name))
			return(true);
		return(false);
	}

	return(false);
}

int BSVM_LookupCheckInfoDelegate(BSVM_SVMState *ctx, dytf info)
{
	BSVM_SVMVarInfo *inf;

	if(dytfKeywordP(info))
		return(true);
	if(dytfSymbolP(info))
		return(false);

	if(BSVM_VarInfoP(info))
	{
		inf=(BSVM_SVMVarInfo *)dytfPtrvF(info);
		if(inf->flags&BGBDYC_FLAG_DELEGATE)
			return(true);
		return(false);
	}

	return(false);
}

dytf BSVM_AssignInfoCoerce(BSVM_SVMState *ctx, dytf info, dytf val)
{
	BSVM_SVMVarInfo *inf;
	dytf t;

	if(BSVM_VarInfoP(info))
	{
		inf=(BSVM_SVMVarInfo *)dytfPtrvF(info);
		t=dytfSigCastVariantSig(inf->sig, val);
		return(t);
	}

	return(val);
}

int BSVM_CheckInfoRest(BSVM_SVMState *ctx, dytf info)
{
	BSVM_SVMVarInfo *inf;

	if(BSVM_VarInfoP(info))
	{
		inf=(BSVM_SVMVarInfo *)dytfPtrvF(info);
//		if(inf->flags&BGBDYC_FLAG_DELEGATE)
//			return(true);
		if(!strcmp(inf->sig, "Cz"))
			return(true);
		if(!strcmp(inf->sig, "z"))
			return(true);
		return(false);
	}

	return(false);
}

char *BSVM_GetInfoName(BSVM_SVMState *ctx, dytf info)
{
	BSVM_SVMVarInfo *inf;

	if(dytfKeywordP(info))
		return(dytfSymbolvF(info));
	if(dytfSymbolP(info))
		return(dytfSymbolvF(info));

	if(BSVM_VarInfoP(info))
	{
		inf=(BSVM_SVMVarInfo *)dytfPtrvF(info);
		return(inf->name);
	}

	if(dytfDycSlotP(info))
		{ return(dycGetSlotName(dytfPtrvF(info))); }
	if(dytfDycMethodP(info))
		{ return(dycGetMethodName(dytfPtrvF(info))); }

	return(NULL);
}

char *BSVM_GetInfoSig(BSVM_SVMState *ctx, dytf info)
{
	BSVM_SVMVarInfo *inf;

	if(dytfNullOrUndefinedP(info))
		return(NULL);

	if(BSVM_VarInfoP(info))
	{
		inf=(BSVM_SVMVarInfo *)dytfPtrvF(info);
		return(inf->sig);
	}
	
	if(dytfDycSlotP(info))
		{ return(dycGetSlotSig(dytfPtrvF(info))); }
	if(dytfDycMethodP(info))
		{ return(dycGetMethodSig(dytfPtrvF(info))); }
	
	return(NULL);
}

void BSVM_LookupCallObj(BSVM_SVMState *ctx, dytf key, dytf *rhdl)
{
	dytf t, u;

	if(!dytfNullP(*rhdl))
	{
		BSVM_CallObj(ctx, ctx->self, *rhdl);
		return;
	}

	if(ctx->this_obj && ctx->this_inf)
	{
		t=dytfWrap(dycLookupMethodSig(ctx->this_obj, dytfSymbolv(key), "?"));
		if(!dytfNullP(t) && !dytfUndefinedP(t))
		{
			*rhdl=t;
			BSVM_CallObj(ctx, ctx->self, t);
			return;
		}
	}

	u=BSVM_Lookup(ctx, key);
	if(dytfNullP(u) || dytfUndefinedP(u))
	{
		dyPrintf("BSVM_LookupCallObj: Fail Lookup %s\n",
			dytfSymbolv(key));
		
		t=BSVM_PopList(ctx);
		BSVM_Push(ctx, DYTF_UNDEFINED);
		return;
	}

//	ctx->thop=cur->next;
	*rhdl=u;
	BSVM_CallObj(ctx, ctx->self, u);
//	return(ctx->thop);
}

dytf BSVM_LookupThisObject(BSVM_SVMState *ctx, dytf key)
{
	char tb[256];
	dytf ck, cv, o;
	dyt t;
	int i, j;

	if(ctx->this_obj && ctx->this_inf)
	{
		t=dycGetSlot(ctx->this_obj, dytfSymbolv(key));
		if(t!=UNDEFINED)return(dytfWrap(t));

		t=dycLookupMethodSig(ctx->this_obj, dytfSymbolv(key), "?");
		if(t && (t!=UNDEFINED))return(dytfWrap(t));

		sprintf(tb, "get_%s", (char *)dytfPtrvF(key));
		t=dycCallNameN(ctx->this_obj, tb, NULL);
		if(t!=UNDEFINED)return(dytfWrap(t));

//		return(t);
	}

	//check in 'this'
	t=dyGet(dytfUnwrap(ctx->self), dytfSymbolv(key));
	if(t!=UNDEFINED)
		return(dytfWrap(t));

	return(DYTF_UNDEFINED);
}

dytf BSVM_Lookup(BSVM_SVMState *ctx, dytf key)
{
	return(BSVM_Lookup_I(ctx, key, NULL));
}

dytf BSVM_LookupGetInfo(BSVM_SVMState *ctx, dytf key, dytf *rinfo)
{
	return(BSVM_Lookup_I(ctx, key, rinfo));
}

dytf BSVM_Lookup_I(BSVM_SVMState *ctx, dytf key, dytf *rinfo)
{
	char tb[256];
	dytf ck, cv, t, o;
	int i, j;

	if(dytfNullP(key) || dytfUndefinedP(key))
		return(DYTF_UNDEFINED);

#if 0
	if(ctx->klenv && !ctx->vlenv)
	{
		ck=ctx->klenv; i=0;
		while(ck)
		{
//			if(dyCarF(ck)==key)
			if(BSVM_LookupCheckVarInfo(ctx, dyCarF(ck), key))
			{
				t=ctx->stack[ctx->eargs-i-1];
				return(t);
			}
			i++; ck=dyCdrF(ck);
		}

		t=dyGet(ctx->self, (char *)key);
		if(t==UNDEFINED)
			t=dyGet(ctx->top, (char *)key);
//		if(t==UNDEFINED)
//			t=BSVM_LookupCTop(ctx, key);
		return(t);
	}
#endif

	//lookup in lexical env
	ck=ctx->klenv; cv=ctx->vlenv;
	while(!dytfNullP(ck))
	{
		if(BSVM_LookupCheckVarInfo(ctx, dytfCarF(ck), key))
		{
			if(rinfo)*rinfo=dytfCarF(ck);
			return(dytfCarF(cv));
		}
		ck=dytfCdrF(ck); cv=dytfCdrF(cv);
	}

	//lookup in lexical env via delegates (imports/...)
	ck=ctx->klenv; cv=ctx->vlenv;
	while(!dytfNullP(ck))
	{
//		if(dykeywordp(dyCarF(ck)))
		if(BSVM_LookupCheckInfoDelegate(ctx, dytfCarF(ck)))
		{
			o=dytfCarF(cv);
			if(dytfKeywordP(o))
			{
				sprintf(tb, "%s/%s",
					(char *)dytfPtrvF(o),
					(char *)dytfPtrvF(key));
				t=BSVM_GetPath(ctx, ctx->top, tb);
				if(!dytfUndefinedP(t))
					return(t);
				t=dytfWrap(dyllGetDynamic(tb));
				if(!dytfUndefinedP(t))
					return(t);
			}else
			{
				t=dytfGet(o, dytfSymbolvF(key));
				if(!dytfUndefinedP(t))
				{
					if(rinfo)*rinfo=DYTF_NULL;
					return(t);
				}
			}
		}
		ck=dytfCdrF(ck); cv=dytfCdrF(cv);
	}

	//check in 'this'
//	t=dyGet(ctx->self, (char *)key);
	t=BSVM_LookupThisObject(ctx, key);
	if(!dytfUndefinedP(t))
	{
		if(rinfo)*rinfo=DYTF_NULL;
		return(t);
	}

	//lookup in dynamic env
	ck=ctx->kdenv; cv=ctx->vdenv;
	while(!dytfNullP(ck))
	{
//		if(dyCarF(ck)==key)
		if(BSVM_LookupCheckVarInfo(ctx, dytfCar(ck), key))
		{
			if(rinfo)*rinfo=dytfCarF(ck);
			return(dytfCarF(cv));
		}
//		if(dykeywordp(dyCarF(ck)))
		if(BSVM_LookupCheckInfoDelegate(ctx, dytfCarF(ck)))
		{
			t=dytfGet(dytfCarF(cv), dytfSymbolvF(key));
			if(!dytfUndefinedP(t))
			{
				if(rinfo)*rinfo=DYTF_NULL;
				return(t);
			}
		}
		ck=dytfCdrF(ck); cv=dytfCdrF(cv);
	}

	//check top
//	if(dytfUndefinedP(t))
	t=dytfGet(ctx->top, dytfSymbolvF(key));
	if(!dytfUndefinedP(t))
	{
		if(rinfo)*rinfo=dytfTryGetInfo(
			ctx->top, dytfSymbolvF(key));
		return(t);
	}

//	if(t==UNDEFINED)
//		t=dyGet(ctx->top, (char *)BSVM_QualifyName(ctx, key));

	//check in current package
//	if(dytfUndefinedP(t) && !dytfNullP(ctx->pkgname))
	if(!dytfNullP(ctx->pkgname))
	{
//		t=BSVM_GetPath(ctx, ctx->top,
//			dytfSymbolv(BSVM_QualifyName(ctx, key)));
		t=BSVM_GetPathInfo(ctx, ctx->top,
			dytfSymbolv(BSVM_QualifyName(ctx, key)), rinfo);
		if(!dytfUndefinedP(t))
			return(t);
	}

//	if(t==UNDEFINED)
//		t=BSVM_LookupCTop(ctx, key);

#if 0
	if(t==UNDEFINED)
	{
//		t=dyllGetFuncAddrObj(key);
		t=dyllGetDynamic(key);
		if(t && t!=UNDEFINED)
			return(t);
		return(UNDEFINED);
	}
#endif

	if(rinfo)*rinfo=DYTF_NULL;
	return(t);
}

void BSVM_Assign(BSVM_SVMState *ctx, dytf key, dytf val)
{
	dytf ck, cv, t;
	int i;

#if 0
	if(ctx->klenv && !ctx->vlenv)
	{
		ck=ctx->klenv; i=0;
		while(ck)
		{
//			if(dyCarF(ck)==key)
			if(BSVM_LookupCheckVarInfo(ctx, dyCarF(ck), key))
			{
				bsSet(ctx->stack[ctx->eargs-i-1], val);
				return;
			}
			i++; ck=dyCdrF(ck);
		}

		t=dyTrySet(ctx->self, (char *)key, val);
		if(t!=UNDEFINED)return;
		t=dyTrySet(ctx->top, (char *)key, val);
		if(t!=UNDEFINED)return;
		if(ctx->self)
			{ dyBind(ctx->self, (char *)key, val); return; }
//		dyBind(ctx->top, (char *)key, val);
		BSVM_Bind(ctx, key, val);
		return;
	}
#endif

	ck=ctx->klenv;
	cv=ctx->vlenv;
	while(!dytfNullP(ck))
	{
//		if(dyCarF(ck)==key)
		if(BSVM_LookupCheckVarInfo(ctx, dytfCar(ck), key))
		{
			dytfDropValue(dytfCar(cv));
			t=BSVM_AssignInfoCoerce(ctx, dytfCar(ck), val);
			dytfSetCarF(cv, t);
//			dySetCarF(cv, val);
			return;
		}
		ck=dytfCdr(ck); cv=dytfCdr(cv);
	}

	//assign in lexical env via delegates (imports/...)
	ck=ctx->klenv; cv=ctx->vlenv;
	while(!dytfNullP(ck))
	{
//		if(dykeywordp(dyCarF(ck)))
		if(BSVM_LookupCheckInfoDelegate(ctx, dytfCar(ck)))
		{
			t=dytfTrySet(dytfCar(cv), dytfSymbolvF(key), val);
			if(!dytfUndefinedP(t))
				return;
		}
		ck=dytfCdr(ck); cv=dytfCdr(cv);
	}

	t=dytfTrySet(ctx->self, dytfSymbolv(key), val);
	if(!dytfUndefinedP(t))return;

	//assign in dynamic env
	ck=ctx->kdenv; cv=ctx->vdenv;
	while(!dytfNullP(ck))
	{
//		if(dyCarF(ck)==key)
		if(BSVM_LookupCheckVarInfo(ctx, dytfCar(ck), key))
		{
			dytfDropValue(dytfCar(cv));
			t=BSVM_AssignInfoCoerce(ctx, dytfCar(ck), val);
			dytfSetCar(cv, t);
//			dySetCarF(cv, val);
			return;
		}
//		if(dykeywordp(dyCarF(ck)))
		if(BSVM_LookupCheckInfoDelegate(ctx, dytfCar(ck)))
		{
			t=dytfTrySet(dytfCar(cv), dytfSymbolv(key), val);
			if(!dytfUndefinedP(t))
				return;
		}
		ck=dytfCdr(ck); cv=dytfCdr(cv);
	}

	if(!dytfNullP(ctx->self))
	{
		t=dytfTrySet(ctx->self, dytfSymbolvF(key), val);
		if(!dytfUndefinedP(t))
			return;
	}

	t=dytfTrySet(ctx->top, dytfSymbolv(key), val);
	if(!dytfUndefinedP(t))
		return;

	if(!dytfNullP(ctx->self))
	{
		dytfBind(ctx->self, dytfSymbolv(key), val);
		return;
	}
//	dyBind(ctx->top, (char *)key, val);
	BSVM_Bind(ctx, key, val);
	return;
}

dytf BSVM_LookupAddr(BSVM_SVMState *ctx, dytf key)
{
	return(BSVM_LookupRef(ctx, key));
}

dytf BSVM_LookupRef(BSVM_SVMState *ctx, dytf key)
{
	char tb[256];
	dytf ck, cv, t, o;
	int i, j;

	//lookup in lexical env
	ck=ctx->klenv; cv=ctx->vlenv;
	while(!dytfNullP(ck))
	{
//		if(dyCarF(ck)==key)
		if(BSVM_LookupCheckVarInfo(ctx, dytfCar(ck), key))
		{
			t=dytfWrap(dyllTypeboxPtr("Cr", (void *)dytfPtrvF(cv)));
			return(t);
		}
		ck=dytfCdr(ck); cv=dytfCdr(cv);
	}

#if 0
	//lookup in lexical env via delegates (imports/...)
	ck=ctx->klenv; cv=ctx->vlenv;
	while(!dytfNullP(ck))
	{
		if(dytfKeywordP(dyCarF(ck)))
		{
			o=dyCarF(cv);
			if(dykeywordp(o))
			{
				sprintf(tb, "%s/%s", (char *)o, (char *)key);
				t=BSVM_GetPath(ctx, ctx->top, tb);
				if(t!=UNDEFINED)
					return(t);
				t=dyllGetDynamic(tb);
				if(t!=UNDEFINED)
					return(t);
			}else
			{
				t=dyGet(o, (char *)key);
				if(t!=UNDEFINED)
					return(t);
			}
		}
		ck=dyCdrF(ck); cv=dyCdrF(cv);
	}
#endif

#if 1
	//check in 'this'
	t=dytfTryGetRef(ctx->self, dytfSymbolvF(key));
	if(!dytfUndefinedP(t))
		return(t);
#endif

	//lookup in dynamic env
	ck=ctx->kdenv; cv=ctx->vdenv;
	while(!dytfNullP(ck))
	{
//		if(dyCarF(ck)==key)
		if(BSVM_LookupCheckVarInfo(ctx, dytfCar(ck), key))
		{
			t=dytfWrap(dyllTypeboxPtr("Cr", (void *)dytfPtrvF(cv)));
			return(t);
		}

//		if(dykeywordp(dyCarF(ck)))
		if(BSVM_LookupCheckInfoDelegate(ctx, dytfCar(ck)))
		{
			t=dytfTryGetRef(dytfCar(cv), dytfSymbolvF(key));
			if(!dytfUndefinedP(t))
				return(t);
		}
		ck=dytfCdr(ck); cv=dytfCdr(cv);
	}

	t=DYTF_UNDEFINED;

	//check top
	if(dytfUndefinedP(t))
	{
		t=dytfTryGetRef(ctx->top, dytfSymbolv(key));
	}

#if 0
//	if(t==UNDEFINED)
//		t=dyGet(ctx->top, (char *)BSVM_QualifyName(ctx, key));

	//check in current package
	if((t==UNDEFINED) && ctx->pkgname)
		t=BSVM_GetPath(ctx, ctx->top, (char *)BSVM_QualifyName(ctx, key));

//	if(t==UNDEFINED)
//		t=BSVM_LookupCTop(ctx, key);
#endif

	return(t);
}

dytf BSVM_LookupIdx(BSVM_SVMState *ctx, int idx)
{
	dytf c, t;
	int i;

#ifdef BSVM_VARSTACK
	if(ctx->vmflags&BSVM_VMFL_VARSTACK)
	{
		t=ctx->levstack[ctx->lestackpos-(idx+1)];
		return(t);
	}
#endif

	c=ctx->vlenv; i=idx;
	while(i--)c=dytfCdrF(c);
	return(dytfCarF(c));
}

void BSVM_AssignIdx(BSVM_SVMState *ctx, int idx, dytf val)
{
	dytf c, t;
	int i;

#ifdef BSVM_VARSTACK
	if(ctx->vmflags&BSVM_VMFL_VARSTACK)
	{
		ctx->levstack[ctx->lestackpos-(idx+1)]=val;
		return;
	}
#endif

	c=ctx->vlenv; i=idx;
	while(i--)c=dytfCdrF(c);
	dytfSetCarF(c, val);
}

void BSVM_AssignIdxF(BSVM_SVMState *ctx, int idx, dytf val)
{
	dytf c, t;
	int i;

#ifdef BSVM_VARSTACK
	if(ctx->vmflags&BSVM_VMFL_VARSTACK)
	{
		ctx->levstack[ctx->lestackpos-(idx+1)]=val;
		return;
	}
#endif

	c=ctx->vlenv; i=idx;
	while(i--)c=dytfCdrF(c);
	dytfSetCarF(c, val);
}

s32 BSVM_LookupIdxS32(BSVM_SVMState *ctx, int idx)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, idx);
	return(dytfIntv(t));
}

s64 BSVM_LookupIdxS64(BSVM_SVMState *ctx, int idx)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, idx);
	return(dytfLongv(t));
}

f32 BSVM_LookupIdxF32(BSVM_SVMState *ctx, int idx)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, idx);
	return(dytfFloatv(t));
}

f64 BSVM_LookupIdxF64(BSVM_SVMState *ctx, int idx)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, idx);
	return(dytfDoublev(t));
}

void BSVM_AssignIdxS32(BSVM_SVMState *ctx, int idx, s32 val)
	{ BSVM_AssignIdx(ctx, idx, dytfInt(val)); }
void BSVM_AssignIdxS64(BSVM_SVMState *ctx, int idx, s64 val)
	{ BSVM_AssignIdx(ctx, idx, dytfLong(val)); }
void BSVM_AssignIdxF32(BSVM_SVMState *ctx, int idx, f32 val)
	{ BSVM_AssignIdx(ctx, idx, dytfFloat(val)); }
void BSVM_AssignIdxF64(BSVM_SVMState *ctx, int idx, f64 val)
	{ BSVM_AssignIdx(ctx, idx, dytfDouble(val)); }

void BSVM_Bind(BSVM_SVMState *ctx, dytf key, dytf val)
{
	dytf t;

//	dyPrintf("bind %s %08X\n", dysymbolv(key), val);

	if(!dytfNullP(ctx->self))
	{
		dytfBind(ctx->self, dytfSymbolvF(key), val);
		return;
	}

//	dyBind(ctx->top, (char *)key, val);

	t=BSVM_QualifyName(ctx, key);
	BSVM_BindPath(ctx, ctx->top, dytfSymbolv(t), val);
}

void BSVM_BindType(BSVM_SVMState *ctx, dytf key,
	dytf type, dytf flags, dytf val)
{
	dycSlot inf;
	char *ty, *vn, *s0, *s1;
	s64 fl;
	dytf t;

//	dyPrintf("bind %s %08X\n", dysymbolv(key), val);

	if(!dytfNullP(ctx->self))
	{
		vn=dytfSymbolv(key);
		ty=dytfSymbolv(type);
		fl=dyllDecodeFlagSig(dytfSymbolv(flags));

		inf=dycGetProtoSlotInfoFl(vn, ty, fl);
		dytfBindInfo(ctx->self, inf, val);
//		dytfBind(ctx->self, dytfSymbolvF(key), val);
		return;
	}

//	dyBind(ctx->top, (char *)key, val);

	t=BSVM_QualifyName(ctx, key);
//	BSVM_BindPath(ctx, ctx->top, dytfSymbolv(t), val);
	BSVM_BindPathType(ctx, ctx->top, dytfSymbolv(t),
		type, flags, val);
}

void BSVM_LexBind(BSVM_SVMState *ctx, dytf key, dytf val)
{
	int i;

#ifdef BSVM_VARSTACK
	if(ctx->vmflags&BSVM_VMFL_VARSTACK)
	{
		i=ctx->lestackpos++;
		ctx->lekstack[i]=key;
		ctx->levstack[i]=val;
		return;
	}
#endif

	bsSet(ctx->klenv, dytfCons(key, ctx->klenv));
	bsSet(ctx->vlenv, dytfCons(val, ctx->vlenv));
}

void BSVM_DynBind(BSVM_SVMState *ctx, dytf key, dytf val)
{
	bsSet(ctx->kdenv, dytfCons(key, ctx->kdenv));
	bsSet(ctx->vdenv, dytfCons(val, ctx->vdenv));
}

BGBSVM_API void BSVM_BindPath(BSVM_SVMState *ctx,
	dytf top, char *name, dytf val)
{
	char pn[256];
	dytf tmp, obj;
	char *s, *t;

	pn[0]=0;
	obj=top;
	s=gcrstrdup(name); t=s;
	while(*t)
	{
		while(*t && (*t!='.') && (*t!='/'))t++;
		if((*t=='.') || (*t=='/'))
		{
			*t++=0;

			if(pn[0])strcat(pn, "/");
			strcat(pn, s);

			tmp=dytfGet(obj, dystrsym(s));
			if(dytfNullP(tmp) || dytfUndefinedP(tmp))
			{
				tmp=dytfObject();
				dytfBind(obj, dystrsym(s), tmp);
				
				dytfBind(tmp, dystrsym("_pkgname_"), dytfString(pn));
			}

			obj=tmp; s=t;
		}
	}

	dytfBind(obj, dystrsym(s), val);
}

BGBSVM_API void BSVM_BindPathType(BSVM_SVMState *ctx,
	dytf top, char *name,
	dytf type, dytf flags, dytf val)
{
	char pn[256];
	dytf tmp, obj;
	dycSlot inf;
	char *ty, *vn, *s0, *s1;
	s64 fl;
	char *s, *t;

	pn[0]=0;
	obj=top;
	s=gcrstrdup(name); t=s;
	while(*t)
	{
		while(*t && (*t!='.') && (*t!='/'))t++;
		if((*t=='.') || (*t=='/'))
		{
			*t++=0;

			if(pn[0])strcat(pn, "/");
			strcat(pn, s);

			tmp=dytfGet(obj, dystrsym(s));
			if(dytfNullP(tmp) || dytfUndefinedP(tmp))
			{
				tmp=dytfObject();
				dytfBind(obj, dystrsym(s), tmp);
				
				dytfBind(tmp, dystrsym("_pkgname_"), dytfString(pn));
			}

			obj=tmp; s=t;
		}
	}

	vn=dystrsym(s);
	ty=dytfSymbolv(type);
	fl=dyllDecodeFlagSig(dytfSymbolv(flags));

	inf=dycGetProtoSlotInfoFl(vn, ty, fl);
	dytfBindInfo(obj, inf, val);
//	dytfBind(obj, dystrsym(s), val);
}

BGBSVM_API dytf BSVM_GetPath(BSVM_SVMState *ctx, dytf top, char *name)
{
	return(BSVM_GetPathInfo(ctx, top, name, NULL));
}

dytf BSVM_GetPathInfo(BSVM_SVMState *ctx,
	dytf top, char *name, dytf *rinfo)
{
	dytf tmp, obj;
	char *s, *t, *s1;

	obj=top;
	s=gcrstrdup(name); t=s;
	while(*t)
	{
		while(*t && (*t!='.') && (*t!='/'))t++;
		if((*t=='.') || (*t=='/'))
		{
			*t++=0;

			tmp=dytfGet(obj, dystrsym(s));
			if(dytfNullP(tmp) || dytfUndefinedP(tmp))
				return(DYTF_UNDEFINED);
			obj=tmp; s=t;
		}
	}

	s1=dystrsym(s);
	tmp=dytfGet(obj, s1);
	if(rinfo && !dytfUndefinedP(tmp))
		{ *rinfo=dytfTryGetInfo(obj, s1); }
	return(tmp);
}

void BSVM_ClearEnv(BSVM_SVMState *ctx)
{
	dytf kc, vc, okc, ovc, kn, vn;
	int i;

#ifdef BSVM_VARSTACK
	while(ctx->lestackpos > ctx->olestackpos)
	{
		i=--ctx->lestackpos;
		dytfDropValue(ctx->levstack[i]);
	}
#endif

	kc=ctx->klenv;
	vc=ctx->vlenv;
	okc=ctx->oklenv;
	ovc=ctx->ovlenv;
	while(dytfNEqqP(kc, okc))
	{
		//drop values leaving scope
		dytfDropValue(dytfCar(vc));

		kn=dytfCdr(kc);
		vn=dytfCdr(vc);
		dytfFreeCons(kc);
		dytfFreeCons(vc);
		kc=kn; vc=vn;
	}
	ctx->klenv=okc;
	ctx->vlenv=ovc;
}

void BSVM_ClearDEnv(BSVM_SVMState *ctx)
{
	dytf kc, vc, okc, ovc, kn, vn;

	if(dytfEqqP(ctx->kdenv, ctx->okdenv))
		return;

	kc=ctx->kdenv;
	vc=ctx->vdenv;
	okc=ctx->okdenv;
	ovc=ctx->ovdenv;
	while(dytfNEqqP(kc, okc))
	{
		kn=dytfCdr(kc);
		vn=dytfCdr(vc);
		dytfFreeCons(kc);
		dytfFreeCons(vc);
		kc=kn; vc=vn;
	}
	ctx->kdenv=okc;
	ctx->vdenv=ovc;
}

dytf BSVM_QualifyName(BSVM_SVMState *ctx, dytf name)
{
	char tb[256];
	dytf t;

	if(!dytfNullP(ctx->pkgname))
	{
		sprintf(tb, "%s/%s",
			dytfSymbolv(ctx->pkgname),
			dytfSymbolv(name));
		t=dytfSymbol(tb);
	}else
	{
		t=name;
	}
	return(t);
}
