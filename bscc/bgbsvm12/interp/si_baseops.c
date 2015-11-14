#include <bgbsvm.h>

#define bsSet(rf, vl)		rf=vl

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];

dytf bsvm_toplevel;
int bsvm_opcnt;

extern void *bsvm_sync_lock;

char *BSVM_MarkSig(BSVM_SVMState *ctx)
{
	return(NULL);
}

bool BSVM_CheckInstanceOf(BSVM_SVMState *ctx, dytf ty, dytf t)
{
	char tb[256];
	char *s, *s0, *t0;
	dytf u, v;
	bool ret;

	s=dytfSymbolv(ty);
	if(!s)return(false);
	
	switch(*s)
	{
	case 'i': case 'j':
		ret=dytfInt32P(t); break;
	case 'x': case 'y':
		ret=dytfLongP(t); break;
	case 'f':
		ret=dytfFloatP(t); break;
	case 'd':
		ret=dytfDoubleP(t); break;
	
	case 'C':
		switch(s[1])
		{
		case 's':
			ret=dytfStringP(t); break;
		default:
			ret=false; break;
		}
		break;
	
	case 'U':
		s0=s+1; t0=tb;
		while(*s0 && *s0!=';') *t0++=*s0++;
		*t0++=0;
		
		if(dytfDycObjectP(t))
		{
			u=BSVM_Lookup(ctx, dytfSymbol(tb));
			if(dytfDycClassP(u))
			{
				if(dycInstanceP(dytfPtrvF(t), dytfPtrvF(u)))
				{
					ret=true;
					break;
				}
			}
		}
		ret=false;
		break;
	
	default:
		v=BSVM_Cast(ctx, ty, t);
		ret=!dytfUndefinedP(v);
		break;
	}

	return(ret);
}

dytf BSVM_TryCast(BSVM_SVMState *ctx, dytf ty, dytf t)
{
	char *s, *s0;
	dytf v;

	v=BSVM_Cast(ctx, ty, t);
//	if(v && (v!=UNDEFINED))
	if(!dytfUndefinedP(v))
		return(v);

	if(dytfStringP(t))
	{
		v=dytfWrap(dysParseFromString(dytfStringv(t)));
		v=BSVM_Cast(ctx, ty, v);
		return(v);
	}

	s=dytfSymbolv(ty);
	if(s && !strcmp(s, "Cs"))
	{
		s0=malloc(65536);
		dysPrintStr(s0, dytfUnwrap(t));
		v=dytfString(s0);
		free(s0);
		return(v);
	}

	return(v);
}

dytf BSVM_Cast(BSVM_SVMState *ctx, dytf ty, dytf t)
{
#if 0
	if(ty==dykeyword("int"))
	{
		if(dyintp(t))return(t);
		if(dyflonump(t))return(dyint(dytfFloatvF(t)));
		return(NULL);
	}

	if(ty==dykeyword("float"))
	{
		if(dyintp(t))return(dyFloatF(dytfIntvF(t)));
		if(dyflonump(t))return(t);
		return(NULL);
	}

	if(ty==dykeyword("double"))
	{
		if(dyintp(t))return(dydouble(dyintv(t)));
		if(dyflonump(t))return(dydouble(dyfloatv(t)));
		return(NULL);
	}
#endif

	if(dytfSymbolP(ty))
	{
		return(dytfWrap(dyllSigCastVariantSig(
			dytfSymbolv(ty),
			dytfUnwrap(t))));
	}

	return(t);
}

dytf BSVM_OpSizeOf(BSVM_SVMState *ctx, dytf ty)
{
	if(dytfSymbolP(ty))
	{
		return(dytfInt(dyllSigQuickSize(dytfSymbolv(ty))));
	}

	return(dytfInt(0));
}

dytf BSVM_OpNew(BSVM_SVMState *ctx, dytf ty)
{
	char *s;
	dytf obj;
	dytf t, u, l;

	l=BSVM_PopList(ctx);
	obj=dytfNewN(dytfStringv(ty), l);
	if(!dytfNullP(obj))return(obj);

	t=BSVM_Lookup(ctx, ty);
	if(dytfNullP(t) || dytfUndefinedP(t))
		return(DYTF_NULL);

	if(dytfObjectP(t))
	{
		BSVM_PushList(ctx, l);
		dytfFreeList(l);

		u=dytfClone(t);
		BSVM_MethodCall(ctx, u, ty);
		return(u);
	}

	if(dytfDycClassP(t))
	{
//		s=dycGetClassName((dycClass)t);
		obj=dytfPtrF(dycNewInitN(dytfDycClassvF(t), dytfUnwrap(l)));
		if(!dytfNullP(obj))return(obj);
	}

	BSVM_PushList(ctx, l);
	dytfFreeList(l);

	u=dytfObject();
	BSVM_CallObj(ctx, u, t);
	return(u);
}

dytf BSVM_OpNewArray(BSVM_SVMState *ctx, dytf ty, dytf n)
{
	dycArray arr;

	arr=dycNewArray(dytfSymbolv(ty), dytfIntv(n));
	return(dytfPtrF(arr));
}

dytf BSVM_OpNewArrayLst(BSVM_SVMState *ctx, dytf ty, dytf l)
{
	dycArray arr;
	dytf c;
	int i, n;

	n=dytfListLength(l);
	arr=dycNewArray(dytfSymbolv(ty), n);
	
	c=l; i=0;
	while(dytfConsP(c))
	{
		dycSetArrayVar(arr, i, dytfUnwrap(dytfCar(c)));
		c=dytfCdr(c); i++;
	}
	
	return(dytfPtrF(arr));
}

dytf BSVM_OpDelete(BSVM_SVMState *ctx, dytf t)
{
	int i;
	i=dytfDelete(t);
	return(dytfBool(!i));
}

void BSVM_OpTyBind(BSVM_SVMState *ctx,
	dytf name, dytf type, dytf flags, dytf val)
{
	dyAccessMode acc;
	s64 fl;
	char *ty, *vn, *s0, *s1;

	if(dytfDycClassP(ctx->self))
	{
		vn=dytfSymbolv(name);
		ty=dytfSymbolv(type);
		fl=dyllDecodeFlagSig(dytfSymbolv(flags));

		//typeAlias is basically a magic field
		if(vn && !strcmp(vn, "typeAlias"))
		{
			s0=dytfStringv(val);
			s1=dycGetClassName(dytfDycClassvF(ctx->self));
			dyTypeSetClassAlias(s0, s1);
		}

		if(!ty)
		{
			if(fl&BGBDYC_FLAG_DELEGATE)
			{
				if(fl&BGBDYC_FLAG_STATIC)
						dycSlotStaticDelegater(vn, "r", dytfUnwrap(val));
				else	dycSlotInitDelegater(vn, "r", dytfUnwrap(val));
				dycAddSlotFlags(vn, fl);
				return;
			}

			if(fl&BGBDYC_FLAG_STATIC)
					dycSlotStaticr(vn, "r", dytfUnwrap(val));
			else	dycSlotInitr(vn, "r", dytfUnwrap(val));
			if(fl)dycAddSlotFlags(vn, fl);
			return;
		}
	
		if(*ty=='(')
		{
			if(fl&BGBDYC_FLAG_STATIC)
					dycMethodStaticFunc(vn, ty, dytfUnwrap(val));
			else	dycMethodFunc(vn, ty, dytfUnwrap(val));
			if(fl)dycAddMethodFlags(vn, ty, fl);
		}else if(fl&BGBDYC_FLAG_DELEGATE)
		{
			if(fl&BGBDYC_FLAG_STATIC)
					dycSlotStaticDelegater(vn, ty, dytfUnwrap(val));
			else	dycSlotInitDelegater(vn, ty, dytfUnwrap(val));
			dycAddSlotFlags(vn, fl);
		}else
		{
			if(fl&BGBDYC_FLAG_STATIC)
					dycSlotStaticr(vn, ty, dytfUnwrap(val));
			else	dycSlotInitr(vn, ty, dytfUnwrap(val));
			if(fl)dycAddSlotFlags(vn, fl);
		}
		
		acc=ctx->access;
		acc|=BGBDY_ACCESS_DEFAULT;
		dycAddSlotAccess(vn, acc);
		return;
	}

	fl=dyllDecodeFlagSig(dytfSymbolv(flags));

	if(fl&BGBDYC_FLAG_DELEGATE)
		name=dytfKeyword(dytfSymbolv(name));

	if(fl&BGBDYC_FLAG_DYNAMIC)
		{ BSVM_DynBind(ctx, name, val); }
	else
		{ BSVM_BindType(ctx, name, type, flags, val); }
}

void BSVM_OpTyDynBind(BSVM_SVMState *ctx,
	dytf name, dytf type, dytf flags, dytf val)
{
	s64 fl;

	fl=dyllDecodeFlagSig(dytfSymbolv(flags));

	if(fl&BGBDYC_FLAG_DELEGATE)
		name=dytfKeyword(dytfSymbolv(name));

	BSVM_DynBind(ctx, name, val);
}

void BSVM_OpTyLexBind(BSVM_SVMState *ctx,
	dytf name, dytf type, dytf flags, dytf val)
{
	BSVM_SVMVarInfo *inf;
	s64 fl;

	fl=dyllDecodeFlagSig(dytfSymbolv(flags));

	if(!strcmp(dytfSymbolvF(type), "r"))
	{
		if(fl&BGBDYC_FLAG_DELEGATE)
			name=dytfKeyword(dytfSymbolv(name));

		if(fl&BGBDYC_FLAG_DYNAMIC)
			{ BSVM_DynBind(ctx, name, val); }
		else
			{ BSVM_LexBind(ctx, name, val); }

		return;
	}

	inf=gctalloc("svm_varinfo_t", sizeof(BSVM_SVMVarInfo));
	inf->name=dyllString(dytfSymbolv(name));
	inf->sig=dyllString(dytfSymbolv(type));
	inf->flags=fl;

	if(fl&BGBDYC_FLAG_DYNAMIC)
		{ BSVM_DynBind(ctx, dytfPtrF(inf), val); }
	else
		{ BSVM_LexBind(ctx, dytfPtrF(inf), val); }
}

void BSVM_OpBeginPackage(BSVM_SVMState *ctx, dytf name, dytf flags)
{
	char tb[256];
	char *s;
	dytf t, n;

	BSVM_Push(ctx, ctx->pkgname);

	n=name;
	s=dytfSymbolv(n);

	//HACK: native package C is toplevel
	if(!strcmp(s, "C"))
	{
		ctx->pkgname=DYTF_NULL;
		return;
	}

	//HACK: fudge native package names
	if(!strncmp(s, "C/", 2))
		n=dytfSymbol(s+2);
	if(!strncmp(s, "CPP/", 4))
		n=dytfSymbol(s+4);
	
	t=BSVM_QualifyName(ctx, n);
	ctx->pkgname=t;
}

void BSVM_OpEndPackage(BSVM_SVMState *ctx)
{
	dytf t;
	t=ctx->pkgname;
	ctx->pkgname=BSVM_Pop(ctx);
	BSVM_Push(ctx, t);
}

void BSVM_OpBeginClassI(BSVM_SVMState *ctx, dytf name, dytf super, s64 fl)
{
	dycClass inf, sinf;
	char *sname;
	dytf t, name2;

	name2=BSVM_QualifyName(ctx, name);
	
	sname=dytfSymbolv(super);
	
	t=BSVM_Lookup(ctx, super);
	if(dytfDycClassP(t))
	{
		sinf=dytfDycClassvF(t);
		sname=dycGetClassName(sinf);
	}

	if(fl&BGBDYC_FLAG_INTERFACE)
	{
		inf=dycBeginIface(dytfSymbolv(name2), sname);
		dycAddClassFlags(fl);
	}else
	if(fl&BGBDYC_FLAG_STRUCT)
	{
		inf=dycBeginStruct(dytfSymbolv(name2));
		dycAddClassFlags(fl);
	}else
	{
		inf=dycBeginClass(dytfSymbolv(name2), sname);
		dycAddClassFlags(fl);
	}
	
//	BSVM_Bind(ctx, name, (dyt)inf);
//	BSVM_AddBinding2(dytfSymbolv(name), (dyt)inf);

//	t=BSVM_QualifyName(ctx, name);
	BSVM_BindPath(ctx, ctx->top, dytfSymbolv(name2), dytfPtrF(inf));

	BSVM_Push(ctx, ctx->self);
	ctx->self=dytfPtrF(inf);

//	BSVM_Bind(ctx, name, ctx->self);
}

void BSVM_OpEndClass(BSVM_SVMState *ctx)
{
	dycClass inf;
	inf=dycEndClass();
	ctx->self=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfPtrF(inf));
}

void BSVM_OpEndIface(BSVM_SVMState *ctx)
{
	dycClass inf;
	inf=dycEndIface();
	ctx->self=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfPtrF(inf));
}

void BSVM_OpEndStruct(BSVM_SVMState *ctx)
{
	dycClass inf;
	inf=dycEndStruct();
	ctx->self=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfPtrF(inf));
}

void BSVM_OpBeginClass(BSVM_SVMState *ctx, dytf name, dytf super, dytf flags)
{
	s64 fl;
	fl=dyllDecodeFlagSig(dytfSymbolv(flags));
	BSVM_OpBeginClassI(ctx, name, super, fl);
}

void BSVM_OpBeginIface(BSVM_SVMState *ctx, dytf name, dytf super, dytf flags)
{
	s64 fl;
	fl=dyllDecodeFlagSig(dytfSymbolv(flags));
	fl|=BGBDYC_FLAG_INTERFACE;
	BSVM_OpBeginClassI(ctx, name, super, fl);
}

void BSVM_OpBeginStruct(BSVM_SVMState *ctx, dytf name, dytf super, dytf flags)
{
	s64 fl;
	fl=dyllDecodeFlagSig(dytfSymbolv(flags));
	fl|=BGBDYC_FLAG_STRUCT;
	BSVM_OpBeginClassI(ctx, name, super, fl);
}

void BSVM_OpClassIface(BSVM_SVMState *ctx, dytf name, dytf flags)
{
	dycClassInterface(dytfSymbolv(name));
}

void BSVM_OpImport(BSVM_SVMState *ctx, dytf name, dytf flags)
{
	char *s;
	dytf t, k;
	s64 fl;

	fl=dyllDecodeFlagSig(dytfSymbolv(flags));
	s=dytfSymbolv(name);
	k=dytfKeyword(s);

	if(fl&BGBDYC_FLAG_NATIVE)
	{
		if(!strncmp(s, "C/", 2))
			{ dyllLoadLibrary(s+2); }
	}

//	if(fl&BGBDYC_FLAG_EXTERN)
	if(1)
	{
		BSVM_LoadQNameScriptCtx(ctx, s, NULL);
	}

	if(fl&BGBDYC_FLAG_ABSTRACT)
	{
		/* abstract import does not import */
	}else if(fl&BGBDYC_FLAG_DELEGATE)
	{
		t=BSVM_GetPath(ctx, ctx->top, dytfSymbolv(ctx->pkgname));
		dytfSetDelegate(t, s, k);
	}else
	{
		k=dytfKeyword(dytfSymbolv(name));
		BSVM_LexBind(ctx, k, k);
	}

// ctx->pkgname
//	t=BSVM_GetPath(ctx, ctx->top, (char *)name);
//	if(t==UNDEFINED)t=NULL;
//	BSVM_LexBind(ctx, k, t);
}

void BSVM_OpBeginSync(BSVM_SVMState *ctx)
{
	if(ctx->sync_lock)
	{
		ctx->sync_lock++;
		return;
	}
	thLockMutex(bsvm_sync_lock);
	ctx->sync_lock=1;
}

void BSVM_OpEndSync(BSVM_SVMState *ctx)
{
	if(ctx->sync_lock>1)
	{
		ctx->sync_lock--;
		return;
	}
	thUnlockMutex(bsvm_sync_lock);
	ctx->sync_lock=0;
}

void BSVM_OpBeginSyncObj(BSVM_SVMState *ctx, dytf obj)
{
	if(ctx->sync_lock)
	{
		ctx->sync_lock++;
		return;
	}
	thLockMutex(bsvm_sync_lock);
	ctx->sync_lock=1;
}

void BSVM_OpEndSyncObj(BSVM_SVMState *ctx, dytf obj)
{
	if(ctx->sync_lock>1)
	{
		ctx->sync_lock--;
		return;
	}
	thUnlockMutex(bsvm_sync_lock);
	ctx->sync_lock=0;
}

void BSVM_OpThrowObj(BSVM_SVMState *ctx, dytf obj)
{
	printf("BSVM_OpThrowObj: %s\n", dytfToString(obj));

	ctx->thrown_obj=obj;
	ctx->thrown_obj2=DYTF_NULL;
	ctx->rs=BSVM_RETS_THROW;
}

void BSVM_OpThrowS(BSVM_SVMState *ctx, dytf obj)
{
	dytf t;

//	ctx->thrown_obj=obj;

	t=BSVM_Lookup(ctx, obj);
	if(!dytfNullP(t) && !dytfUndefinedP(t))
	{
		BSVM_OpThrowObj(ctx, t);
		return;
	}

	BSVM_OpThrowName(ctx, dytfSymbolv(obj));
}

void BSVM_OpThrowName(BSVM_SVMState *ctx, char *name)
{
	char tb[256];
	dycObject tmp;
	
	sprintf(tb, "bsvm/init/%s", name);
	tmp=dycNewClass(tb);
	if(tmp) { BSVM_OpThrowObj(ctx, dytfPtrF(tmp)); return; }
	
	BSVM_OpThrowObj(ctx, dytfKeyword(name));
}


void BSVM_OpBeginTry(BSVM_SVMState *ctx)
{
}

void BSVM_OpEndTry(BSVM_SVMState *ctx)
{
}

void BSVM_OpBeginCatch(BSVM_SVMState *ctx, dytf ty)
{
	int i;

	//if we hit a begin_catch as a normal instruction
	//then it should not be executed
	//so, seek to the next end_catch instruction (to skip catch block)
	while(1)
	{
		i=*ctx->ip++;
		if(i>=192)
			{ i=(i-192)*256+(*ctx->ip++); }
		if(i==BSVM_SOP_END_CATCH)
			break;
		BSVM_SkipOpcodeArgs(ctx, i);
	}
}

void BSVM_OpEndCatch(BSVM_SVMState *ctx)
{
	if(dytfNullP(ctx->thrown_obj))
		ctx->thrown_obj=ctx->thrown_obj2;
}

void BSVM_OpBeginCatchFinal(BSVM_SVMState *ctx)
{
}

void BSVM_OpSetupJoin(BSVM_SVMState *ctx, dytf obj)
{
	BSVM_SVMState *ctx2;
	dyt t, c;
	
	if(dytfConsP(obj))
	{
		c=dytfUnwrap(obj);
		while(dyconsp(c))
		{
			t=dycar(c);
			if(dytypep(t, "svm_state_t"))
			{
				ctx2=(BSVM_SVMState *)t;
				ctx2->vmgflags|=BSVM_VMGFL_JOINED;
			}
			c=dycddr(c);
		}
		ctx->thread_join=obj;
		return;
	}
	
	if(dytftypep(obj, "svm_state_t"))
	{
		ctx2=(BSVM_SVMState *)dytfPtrvF(obj);
		ctx2->vmgflags|=BSVM_VMGFL_JOINED;
		ctx->thread_join=obj;
		return;
	}

	ctx->thread_join=obj;
	return;
}
