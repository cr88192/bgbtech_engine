#include <bgbsvm.h>

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];

dytf bsvm_toplevel;
int bsvm_opcnt;

BSVM_SVMState *bsvm_freectx=NULL;
extern void *bsvm_vm_lock;



BGBSVM_API BSVM_SVMState *BSVM_NewState()
{
	BSVM_SVMState *ctx;

#if 1
	BSVM_LockVM();
	if(bsvm_freectx)
	{
		ctx=bsvm_freectx;
		bsvm_freectx=ctx->next;
		BSVM_UnlockVM();
		
		ctx->stackpos=0;
		ctx->markstackpos=0;
		ctx->flowstackpos=0;
		ctx->lestackpos=0;
		ctx->rs=0;

		ctx->ip=NULL;
		ctx->ipe=NULL;

		ctx->klenv=DYTF_NULL;
		ctx->vlenv=DYTF_NULL;
		ctx->kdenv=DYTF_NULL;
		ctx->vdenv=DYTF_NULL;

		ctx->this_obj=NULL;
		ctx->this_inf=NULL;
		ctx->vmflags=0;
		ctx->vmgflags=0;
		ctx->pfhint=0;
		ctx->pfsig=0;

		ctx->self=DYTF_NULL;
		ctx->top=DYTF_NULL;

		ctx->lit=DYTF_NULL;
		ctx->func=DYTF_NULL;
		ctx->pkgname=DYTF_NULL;

		ctx->thrown_obj=DYTF_NULL;
		ctx->thrown_obj2=DYTF_NULL;
		ctx->thread_join=DYTF_NULL;
		
		ctx->thop=NULL;
		ctx->thtr=NULL;
		
//		memset(ctx, 0, sizeof(BSVM_SVMState));

#if 0
		ctx->stack=gcmalloc(16384*sizeof(dytf));
		ctx->markstack=gcmalloc(4096*sizeof(int));
		ctx->flowstack=gcmalloc(4096*sizeof(BSVM_SVMFrame *));

		ctx->stacklim=16384;
		ctx->markstacklim=4096;
		ctx->flowstacklim=4096;

#ifdef BSVM_VARSTACK
		ctx->lekstack=gcmalloc(16384*sizeof(dytf));
		ctx->levstack=gcmalloc(16384*sizeof(dytf));
		ctx->lestacklim=16384;
#endif
#endif

		return(ctx);
	}
	BSVM_UnlockVM();
#endif

	ctx=gctalloc("svm_state_t", sizeof(BSVM_SVMState));

	ctx->stack=gcmalloc(16384*sizeof(dytf));
	ctx->markstack=gcmalloc(4096*sizeof(int));
	ctx->flowstack=gcmalloc(4096*sizeof(BSVM_SVMFrame *));

	ctx->stacklim=16384;
	ctx->markstacklim=4096;
	ctx->flowstacklim=4096;

#ifdef BSVM_VARSTACK
	ctx->lekstack=gcmalloc(16384*sizeof(dytf));
	ctx->levstack=gcmalloc(16384*sizeof(dytf));
	ctx->lestacklim=16384;
#endif

	return(ctx);
}

BGBSVM_API void BSVM_DestroyState(BSVM_SVMState *ctx)
{
	int i;
//	return;

	BSVM_BVT_Teardown(ctx);

#if 1
	BSVM_LockVM();
	ctx->next=bsvm_freectx;
	bsvm_freectx=ctx;
	BSVM_UnlockVM();
	return;
#endif

#if 1
#ifdef BSVM_VARSTACK
	gcfree(ctx->lekstack);
	gcfree(ctx->levstack);
#endif

	for(i=0; i<ctx->flowstacklim; i++)
	{
		if(!ctx->flowstack[i])
			break;
		gcfree(ctx->flowstack[i]);
	}

	gcfree(ctx->stack);
	gcfree(ctx->markstack);
	gcfree(ctx->flowstack);
	gcfree(ctx);
#endif
}

BGBSVM_API void BSVM_SetOpCnt(int i)
{
	bsvm_opcnt=i;
}

BGBSVM_API int BSVM_GetOpCnt()
{
	return(bsvm_opcnt);
}

void BSVM_Push(BSVM_SVMState *ctx, dytf val)
{
	if(ctx->stackpos>=16383)
	{
		dyPrintf("BSVM_Push: Stack Overflow\n");
		return;
	}

	ctx->stack[ctx->stackpos++]=val;
}

dytf BSVM_Pop(BSVM_SVMState *ctx)
{
	dytf t;

	if(ctx->stackpos<=0)
	{
		dyPrintf("BSVM_Pop: Stack Underflow @ %p\n", ctx->ip);
		return(DYTF_UNDEFINED);
	}

	ctx->stackpos--;
	t=ctx->stack[ctx->stackpos];
	return(t);
}

void BSVM_PushF(BSVM_SVMState *ctx, dytf val)
{
	if(ctx->stackpos>=16383)
	{
		dyPrintf("BSVM_PushF: Stack Overflow @ %p\n", ctx->ip);
		return;
	}

	ctx->stack[ctx->stackpos++]=val;
}

dytf BSVM_PopF(BSVM_SVMState *ctx)
{
	if(ctx->stackpos<=0)
	{
		dyPrintf("BSVM_PopF: Stack Underflow @ %p\n", ctx->ip);
		return(DYTF_UNDEFINED);
	}

	ctx->stackpos--;
	return(ctx->stack[ctx->stackpos]);
}

void BSVM_Push2(BSVM_SVMState *ctx, dytf x, dytf y)
{
	BSVM_Push(ctx, x);
	BSVM_Push(ctx, y);
}

void BSVM_Push3(BSVM_SVMState *ctx, dytf x, dytf y, dytf z)
{
	BSVM_Push(ctx, x);
	BSVM_Push(ctx, y);
	BSVM_Push(ctx, z);
}

void BSVM_Push4(BSVM_SVMState *ctx, dytf x, dytf y, dytf z, dytf w)
{
	BSVM_Push(ctx, x);
	BSVM_Push(ctx, y);
	BSVM_Push(ctx, z);
	BSVM_Push(ctx, w);
}

void BSVM_Pop2(BSVM_SVMState *ctx, dytf *rx, dytf *ry)
{
	*ry=BSVM_Pop(ctx);
	*rx=BSVM_Pop(ctx);
}

void BSVM_Pop3(BSVM_SVMState *ctx, dytf *rx, dytf *ry, dytf *rz)
{
	*rz=BSVM_Pop(ctx);
	*ry=BSVM_Pop(ctx);
	*rx=BSVM_Pop(ctx);
}

void BSVM_Pop4(BSVM_SVMState *ctx, dytf *rx, dytf *ry, dytf *rz, dytf *rw)
{
	*rw=BSVM_Pop(ctx);
	*rz=BSVM_Pop(ctx);
	*ry=BSVM_Pop(ctx);
	*rx=BSVM_Pop(ctx);
}

void BSVM_PopDestroy(BSVM_SVMState *ctx)
{
	dytf t;

	if(ctx->stackpos<=0)
	{
		dyPrintf("BSVM_PopDestroy: Stack Underflow\n");
		return;
	}

	ctx->stackpos--;
	t=ctx->stack[ctx->stackpos];
	ctx->stack[ctx->stackpos]=DYTF_NULL;
	dytfDropValue(t);
}

int BSVM_PopBool(BSVM_SVMState *ctx)
{
	dytf t;
	int i;

	ctx->stackpos--;
	t=ctx->stack[ctx->stackpos];
	return(dytfTrueP(t));
}

void BSVM_PushBool(BSVM_SVMState *ctx, bool val)
{
	BSVM_PushF(ctx, dytfBool(val));
}

void BSVM_PushS32(BSVM_SVMState *ctx, s32 val)
	{ BSVM_Push(ctx, dytfInt(val)); }
void BSVM_PushS64(BSVM_SVMState *ctx, s64 val)
	{ BSVM_Push(ctx, dytfLong(val)); }
void BSVM_PushF32(BSVM_SVMState *ctx, f32 val)
	{ BSVM_Push(ctx, dytfFloat(val)); }
void BSVM_PushF64(BSVM_SVMState *ctx, f64 val)
	{ BSVM_Push(ctx, dytfDouble(val)); }

void BSVM_Push2S32(BSVM_SVMState *ctx, s32 x, s32 y)
	{ BSVM_PushS32(ctx, x); BSVM_PushS32(ctx, y); }
void BSVM_Push2S64(BSVM_SVMState *ctx, s64 x, s64 y)
	{ BSVM_PushS64(ctx, x); BSVM_PushS64(ctx, y); }
void BSVM_Push2F32(BSVM_SVMState *ctx, f32 x, f32 y)
	{ BSVM_PushF32(ctx, x); BSVM_PushF32(ctx, y); }
void BSVM_Push2F64(BSVM_SVMState *ctx, f64 x, f64 y)
	{ BSVM_PushF64(ctx, x); BSVM_PushF64(ctx, y); }

s32 BSVM_PopS32(BSVM_SVMState *ctx)
{
	dytf t;
	t=BSVM_Pop(ctx);
//	return(dytfIntv(t));
	return(dytfInt32vF(t));
}

s64 BSVM_PopS64(BSVM_SVMState *ctx)
{
	dytf t;
	t=BSVM_Pop(ctx);
//	return(dytfLongv(t));
	return(dytfLongvF(t));
}

f32 BSVM_PopF32(BSVM_SVMState *ctx)
{
	dytf t;
	t=BSVM_Pop(ctx);
//	return(dytfFloatv(t));
	return(dytfFloatvF(t));
}

f64 BSVM_PopF64(BSVM_SVMState *ctx)
{
	dytf t;
	t=BSVM_Pop(ctx);
//	return(dytfDoublev(t));
	return(dytfDoublevF(t));
}

dytf BSVM_StackPeekTop(BSVM_SVMState *ctx)
	{ return(ctx->stack[ctx->stackpos-1]); }

void BSVM_PushList(BSVM_SVMState *ctx, dytf l)
{
	dytf c;

	ctx->markstack[ctx->markstackpos++]=ctx->stackpos;

	c=l;
	while(dytfConsP(l))
	{
		BSVM_Push(ctx, dytfCar(l));
		l=dytfCdr(l);
	}
}

void BSVM_PushSimpleList(BSVM_SVMState *ctx, dyt l)
{
	dyt c;

	ctx->markstack[ctx->markstackpos++]=ctx->stackpos;

	c=l;
	while(dyconsp(l))
	{
		BSVM_Push(ctx, dytfWrap(dycar(l)));
		l=dycdr(l);
	}
}

dytf BSVM_PopList(BSVM_SVMState *ctx)
{
	dytf t, l;
	int i;

	i=ctx->markstack[ctx->markstackpos-1];
	l=DYTF_NULL;
	while(ctx->stackpos>i)
	{
		t=BSVM_Pop(ctx);
		bsSet(l, dytfCons(t, l));
	}

	ctx->markstackpos--;
	return(l);
}

dytf BSVM_PopList2(BSVM_SVMState *ctx)
{
	dytf t, l;
	int i;

	i=ctx->markstack[ctx->markstackpos-1];
	l=BSVM_Pop(ctx);
	while(ctx->stackpos>i)
	{
		t=BSVM_Pop(ctx);
		l=dytfCons(t, l);
	}

	ctx->markstackpos--;
	return(l);
}

dyt BSVM_PopSimpleList(BSVM_SVMState *ctx)
{
	dyt t, l;
	int i;

	i=ctx->markstack[ctx->markstackpos-1];
	l=NULL;
	while(ctx->stackpos>i)
	{
		t=dytfUnwrap(BSVM_Pop(ctx));
		bsSet(l, dycons(t, l));
	}

	ctx->markstackpos--;
	return(l);
}

dytf BSVM_StackPeekList(BSVM_SVMState *ctx)
{
	dytf t, l;
	int i, j;

	i=ctx->markstack[ctx->markstackpos-1];
	j=ctx->stackpos; l=DYTF_NULL;
	while(j>i)
	{
		j--;
		t=ctx->stack[j];
		bsSet(l, dytfCons(t, l));
	}
	return(l);
}

void BSVM_PopMark(BSVM_SVMState *ctx)
{
	int i;

	ctx->markstackpos--;
	i=ctx->markstack[ctx->markstackpos];
	while(ctx->stackpos>i)
		BSVM_PopDestroy(ctx);
}

dytf *BSVM_PopMarkArgs(BSVM_SVMState *ctx, int *rnargs)
{
	int i;

	ctx->markstackpos--;
	i=ctx->markstack[ctx->markstackpos];
	*rnargs=ctx->stackpos-i;
	ctx->stackpos=i;
	return(ctx->stack+i);
}

void BSVM_ShiftMark(BSVM_SVMState *ctx)
{
	int i, j, k;

	ctx->markstackpos--;
	k=ctx->markstack[ctx->markstackpos];

	i=k;
	j=ctx->markstack[ctx->markstackpos-1];
	while(i<ctx->stackpos)
	{
		bsSet(ctx->stack[j++], ctx->stack[i++]);
	}
	ctx->stackpos=j;
}

dytf BSVM_LoadIndex(BSVM_SVMState *ctx, dytf obj, dytf key)
{
	char *s;
	dytf c, t;
	dyt c1;
	int i;

#if 0
	if(dyarrayp(obj))
	{
		i=dyintv(key);
		t=dyarrayidx(obj, i);
		return(t);
	}
#endif

#if 0
	if(dytfDycArrayP(obj))
	{
		i=dytfIntv(key);
		t=dytfArrayGetIdx(obj, i);
		return(t);
	}

	if(dycArrayRefP(dytfUnwrap(obj)))
	{
		c1=(dyt)dycArrayRefArray(dytfUnwrap(obj));
		i=dycArrayRefIndex(dytfUnwrap(obj))+dyIntv(key);
		t=dytfWrap(dycGetArrayVar((dycArray)c1, i));
		return(t);
	}

	if(dytfObjectP(obj))
	{
		t=dytfGet(obj, dytfSymbolvF(key));
		if(dytfUndefinedP(t))
			t=dytfGetDelegate(obj, dytfSymbolvF(key));
		return(t);
	}

	if(dytfConsP(obj))
	{
		c=obj; i=dyIntvF(key);
		while(dytfConsP(c) && i)
			{ i--; c=dytfCdr(c); }
		if(dytfConsP(c))return(dytfCar(c));
		return(DYTF_UNDEFINED);
	}

	if(dytfStringP(obj) && dytfIntP(key))
	{
//		s=(char *)obj; i=dyintv(key);
//		t=dychar(((char *)obj)[i]);
		t=dytfGetIdx(obj, dytfIntv(key));
		return(t);
	}
#endif

//	t=BSVM_TY_LoadIndex(obj, key);
	t=dytfGetKey(obj, key);
	return(t);
}

void BSVM_StoreIndex(BSVM_SVMState *ctx, dytf obj, dytf key, dytf val)
{
	void *p;
	char *s;
	dytf c;
	int i;

#if 0
	if(dyarrayp(obj))
	{
		i=dyintv(key);
		dyarraysetidx(obj, i, val);
		return;
	}
#endif

#if 0
	if(dycArrayP(obj))
	{
		i=dyintv(key);
		dycSetArrayVar((dycArray)obj, i, val);
		return;
	}

	if(dycArrayRefP(obj))
	{
		c=(dyt)dycArrayRefArray(obj);
		i=dycArrayRefIndex(obj)+dyintv(key);
		dycSetArrayVar((dycArray)c, i, val);
		return;
	}

	if(dyObjectp(obj))
	{
		dySet(obj, (char *)key, val);
		return;
	}
	
	if(dyllTypeboxPtrP(obj))
	{
		s=dyllTypeboxPtrSig(obj);
		p=dyllTypeboxPtrData(obj);
		dyllSetVarSig(p, s, val);
		return;
	}

	if(dyconsp(obj))
	{
		c=obj; i=dyIntvF(key);
		while(i && dyconsp(dyCdrF(c))) { i--; c=dyCdrF(c); }
		while(i) { i--; dySetCdrF(c, dylist1(NULL)); c=dyCdrF(c); }

		if(dyconsp(c))dysetcar(c, val);

		return;
	}
#endif

	dytfSetKey(obj, key, val);
}

dytf BSVM_LoadIndexAddr(BSVM_SVMState *ctx, dytf obj, dytf key)
{
	dytf t;

#if 0
	if(dyllTypeboxPtrP(obj))
	{
		s=dyllTypeboxPtrSig(obj);
		p=dyllTypeboxPtrData(obj);
//		dyllSetVarSig(p, s, val);
		return(p);
	}
#endif

	t=dytfGetKeyRef(obj, key);
	return(t);
//	return(UNDEFINED);
}

dytf BSVM_LoadIndexRef(BSVM_SVMState *ctx, dytf obj, dytf key)
{
	dytf t;

	t=dytfGetKeyRef(obj, key);
	return(t);
}

dytf BSVM_Unary(BSVM_SVMState *ctx, int i, dytf obj)
{
	char *uops[]={"neg", "not", "lnot", "pos", "umul", "udiv", "uidiv",
		"umod", "uand", "uor", "uxor", "uland", "ulor", "ulxor", "inc",
		"dec", "defer", "car", "cdr",
		"copyvalue", "dropvalue", "delete", "clone",
		"sin", "cos", "tan", "asin", "acos", "atan", "sqr", "sqrt"};
	dytf t;

	switch(i)
	{
	case BSVM_OPR_NEG: t=dytfNeg(obj); break;
	case BSVM_OPR_NOT: t=dytfNot(obj); break;
//	case BSVM_OPR_LNOT: t=dyBoolF(dyfalsep(obj)); break;
	case BSVM_OPR_LNOT: t=dytfLNot(obj); break;
	case BSVM_OPR_UMUL: t=dytfUMul(obj); break;
	case BSVM_OPR_UDIV: t=dytfUDiv(obj); break;
	case BSVM_OPR_UMOD: t=dytfUMod(obj); break;
	case BSVM_OPR_UAND: t=dytfUAnd(obj); break;
	case BSVM_OPR_UOR: t=dytfUOr(obj); break;
	case BSVM_OPR_UXOR: t=dytfUXor(obj); break;

	case BSVM_OPR_INC: t=dytfInc(obj); break;
	case BSVM_OPR_DEC: t=dytfDec(obj); break;

	case BSVM_OPR_SIN: t=dytfSin(obj); break;
	case BSVM_OPR_COS: t=dytfCos(obj); break;
	case BSVM_OPR_TAN: t=dytfTan(obj); break;
	case BSVM_OPR_ASIN: t=dytfASin(obj); break;
	case BSVM_OPR_ACOS: t=dytfACos(obj); break;
	case BSVM_OPR_ATAN: t=dytfATan(obj); break;

	case BSVM_OPR_SQR: t=dytfSqr(obj); break;
	case BSVM_OPR_SQRT: t=dytfSqrt(obj); break;

//	case BSVM_OPR_INC: t=dyadd(obj, dyint(1)); break;
//	case BSVM_OPR_DEC: t=dyadd(obj, dyint(-1)); break;
	default: t=DYTF_UNDEFINED; break;
	}

	if(dytfUndefinedP(t))
	{
		t=BSVM_UnaryS(ctx, uops[i], obj);
		return(t);
	}

	return(t);
}

dytf BSVM_Binary(BSVM_SVMState *ctx, int i, dytf tl, dytf tr)
{
	char *bops[]={"add", "sub", "mul", "div", "mod", "and", "or",
		"xor", "shl", "shr", "eq", "eqv", "neq", "neqv", "l",
		"g", "le", "ge", "idiv", "dadd", "dsub", "dmul", "ddiv",
		"dmod", "dand", "dor", "dxor", "didiv", "exp", "shrr",
		"cons", "atan2"};
	dytf t;

	switch(i)
	{
	case BSVM_OPR_ADD: t=dytfAdd(tl, tr); break;
	case BSVM_OPR_SUB: t=dytfSub(tl, tr); break;
	case BSVM_OPR_MUL: t=dytfMul(tl, tr); break;
	case BSVM_OPR_DIV: t=dytfDiv(tl, tr); break;
	case BSVM_OPR_MOD: t=dytfMod(tl, tr); break;
	case BSVM_OPR_AND: t=dytfAnd(tl, tr); break;
	case BSVM_OPR_OR: t=dytfOr(tl, tr); break;
	case BSVM_OPR_XOR: t=dytfXor(tl, tr); break;
	case BSVM_OPR_SHL: t=dytfShl(tl, tr); break;
	case BSVM_OPR_SHR: t=dytfShr(tl, tr); break;
	case BSVM_OPR_EXP: t=dytfExp(tl, tr); break;

	case BSVM_OPR_ATAN2: t=dytfATan2(tl, tr); break;

	case BSVM_OPR_EQ: t=dytfBool(dytfEqqP(tl, tr)); break;
	case BSVM_OPR_EQV: t=dytfBool(dytfEqP(tl, tr)); break;
	case BSVM_OPR_NEQ: t=dytfBool(dytfNEqqP(tl, tr)); break;
	case BSVM_OPR_NEQV: t=dytfBool(dytfNEqP(tl, tr)); break;
	case BSVM_OPR_L: t=dytfBool(dytfLtP(tl, tr)); break;
	case BSVM_OPR_G: t=dytfBool(dytfGtP(tl, tr)); break;
	case BSVM_OPR_LE: t=dytfBool(dytfLeP(tl, tr)); break;
	case BSVM_OPR_GE: t=dytfBool(dytfGeP(tl, tr)); break;

	default: t=DYTF_UNDEFINED; break;
	}

	if(dytfUndefinedP(t))
	{
		t=BSVM_BinaryS(ctx, bops[i], tl, tr);
		return(t);
	}

	return(t);
}

dytf BSVM_UnaryS(BSVM_SVMState *ctx, char *sym, dytf obj)
{
	char tb[256];
	dytf t;

	t=dytfUnaryS(sym, obj);
	if(!dytfUndefinedP(t))
		return(t);

	sprintf(tb, "operator_%s", sym);
	t=dytfWrap(BSVM_CallFuncObjCtx(ctx, NULL, dysymbol(tb),
		dylist1(dytfUnwrap(obj))));
	if(!dytfUndefinedP(t))
		return(t);

	return(DYTF_UNDEFINED);
}

dytf BSVM_BinaryS(BSVM_SVMState *ctx, char *sym, dytf tl, dytf tr)
{
	char tb[256];
	dytf t;
	
	t=dytfBinaryS(sym, tl, tr);
	if(!dytfUndefinedP(t))return(t);

	sprintf(tb, "operator_%s_", sym);
	t=dytfWrap(BSVM_CallFuncObjCtx(ctx, NULL, dysymbol(tb),
		dylist2(dytfUnwrap(tl), dytfUnwrap(tr))));
	if(!dytfUndefinedP(t))return(t);

	return(DYTF_UNDEFINED);
}

int BSVM_CondUnary(BSVM_SVMState *ctx, int i, dytf obj)
{
	dytf t;
	t=BSVM_Unary(ctx, i, obj);
	return(dytfTrueP(t));
}

int BSVM_CondBinary(BSVM_SVMState *ctx, int i, dytf tl, dytf tr)
{
	dytf t;
	t=BSVM_Binary(ctx, i, tl, tr);
	return(dytfTrueP(t));
}

BGBSVM_API dytf BSVM_ListToVector(dytf lst)
{
	dytf t;
	int i;
	
	i=dytfListLength(lst);
	switch(i)
	{
	case 2:
		t=dytfWrap(dyvec2(
			dytfRealv(dytfCar(lst)),
			dytfRealv(dytfCadr(lst))));
		break;
	case 3:
		t=dytfWrap(dyvec3(
			dytfRealv(dytfCar(lst)),
			dytfRealv(dytfCadr(lst)),
			dytfRealv(dytfCaddr(lst))));
		break;
	case 4:
		t=dytfWrap(dyvec4(
			dytfRealv(dytfCar(lst)),
			dytfRealv(dytfCadr(lst)),
			dytfRealv(dytfCaddr(lst)),
			dytfRealv(dytfCadddr(lst))));
		break;
	default:
		t=dytfListToArray(lst);
		break;
	}
	return(t);
}

void BSVM_PushFrame(BSVM_SVMState *ctx)
{
	BSVM_SVMFrame *frm;
	int i;

	i=ctx->flowstackpos++;
	if(i<0)
	{
//		dyPrintf("BSVM_PushFrame: Stack Underflow\n");
		return;
	}
	if(i>=ctx->flowstacklim)
	{
		dyPrintf("BSVM_PushFrame: Stack Overflow\n");
		return;
	}

	frm=ctx->flowstack[i];
	if(!frm)
	{
		frm=gctalloc("bsvm_svmframe_t", sizeof(BSVM_SVMFrame));
		ctx->flowstack[i]=frm;
	}

	frm->self=ctx->self;
	frm->top=ctx->top;
	frm->lit=ctx->lit;
	frm->func=ctx->func;
	frm->klenv=ctx->klenv;
	frm->vlenv=ctx->vlenv;
	frm->kdenv=ctx->kdenv;
	frm->vdenv=ctx->vdenv;
	frm->oklenv=ctx->oklenv;
	frm->ovlenv=ctx->ovlenv;
	frm->okdenv=ctx->okdenv;
	frm->ovdenv=ctx->ovdenv;

	ctx->self=DYTF_NULL;
//	ctx->top=DYTF_NULL;
	ctx->lit=DYTF_NULL;
	ctx->func=DYTF_NULL;
	ctx->klenv=DYTF_NULL;
	ctx->vlenv=DYTF_NULL;
	ctx->oklenv=DYTF_NULL;
	ctx->ovlenv=DYTF_NULL;
	ctx->okdenv=ctx->kdenv;
	ctx->ovdenv=ctx->vdenv;

	frm->ip=ctx->ip;
	frm->ipe=ctx->ipe;

	frm->iv[0]=ctx->args;
	frm->iv[1]=ctx->eargs;

	frm->stackpos=ctx->stackpos;
	frm->markstackpos=ctx->markstackpos;
//	frm->lestackpos=ctx->lestackpos;


#ifdef BSVM_VARSTACK
	frm->lekstack=ctx->lekstack;
	frm->levstack=ctx->levstack;
	frm->lestackpos=ctx->lestackpos;
	frm->olestackpos=ctx->olestackpos;

	ctx->olestackpos=ctx->lestackpos;
#endif

	frm->access=ctx->access;
	frm->vmflags=ctx->vmflags;
//	frm->blkflags=ctx->blkflags;

	frm->dbg_fn=ctx->dbg_fn;
	frm->dbg_ln=ctx->dbg_ln;

	frm->this_obj=ctx->this_obj;
	frm->this_inf=ctx->this_inf;

#ifdef BSVM_JIT
	frm->thunk=ctx->thunk;
#endif

#ifdef BSVM_THREADCODE
	frm->thop=ctx->thop;
	frm->thtr=ctx->thtr;
#endif
}

void BSVM_PopFrame(BSVM_SVMState *ctx)
{
	BSVM_SVMFrame *frm;
	int i;

	BSVM_ClearDEnv(ctx);

	ctx->flowstackpos--;
	i=ctx->flowstackpos;

	if(i<0)return;

	frm=ctx->flowstack[i];
	ctx->self=frm->self;
	ctx->top=frm->top;
	ctx->lit=frm->lit;
	ctx->func=frm->func;
	ctx->klenv=frm->klenv;
	ctx->vlenv=frm->vlenv;
	ctx->oklenv=frm->oklenv;
	ctx->ovlenv=frm->ovlenv;

	ctx->kdenv=frm->kdenv;
	ctx->vdenv=frm->vdenv;
	ctx->okdenv=frm->okdenv;
	ctx->ovdenv=frm->ovdenv;

	ctx->ip=frm->ip;
	ctx->ipe=frm->ipe;
	ctx->args=frm->iv[0];
	ctx->eargs=frm->iv[1];

//	ctx->lestackpos=frm->lestackpos;

	ctx->vmflags=frm->vmflags;
//	ctx->blkflags=frm->blkflags;

	ctx->dbg_fn=frm->dbg_fn;
	ctx->dbg_ln=frm->dbg_ln;

#ifdef BSVM_VARSTACK
	ctx->lekstack=frm->lekstack;
	ctx->levstack=frm->levstack;
	ctx->lestackpos=frm->lestackpos;
	ctx->olestackpos=frm->olestackpos;
#endif

	ctx->this_obj=frm->this_obj;
	ctx->this_inf=frm->this_inf;

	if(ctx->access!=frm->access)
	{
		ctx->access=frm->access;
		dySetAccessMode(ctx->access);
	}

#ifdef BSVM_JIT
	ctx->thunk=frm->thunk;
#endif

#ifdef BSVM_THREADCODE
	ctx->thop=frm->thop;
	ctx->thtr=frm->thtr;
#endif
}

void BSVM_BindArgs(BSVM_SVMState *ctx, dytf kl, dytf vl)
{
	dytf n, t, v;
	dytf kc, vc;

	if(dytfNullP(kl) && dytfNullP(vl))
		return;

	kc=kl; vc=vl;
	while(dytfConsP(kc) && dytfConsP(vc))
	{
#if 1
		if(dytfConsP(dytfCarF(kc)))
		{
//			bsSet(ctx->klenv, dycons(dycadar(kc), ctx->klenv));
//			bsSet(ctx->vlenv, dycons(dycar(vc), ctx->vlenv));
			BSVM_LexBind(ctx, dytfCadar(kc), dytfCarF(vc));

			kc=dytfCdrF(kc); vc=dytfCdrF(vc);
			continue;
		}
#endif

		if(!dytfSymbolP(dytfCar(kc)))
		{
			kc=dytfCdrF(kc); vc=dytfCdrF(vc);
			continue;
		}

//		bsSet(ctx->klenv, dycons(dycar(kc), ctx->klenv));
//		bsSet(ctx->vlenv, dycons(dycar(vc), ctx->vlenv));
		BSVM_LexBind(ctx, dytfCarF(kc), dytfCarF(vc));

		kc=dytfCdrF(kc); vc=dytfCdrF(vc);
	}

	if(dytfSymbolP(kc))
	{
		vc=dytfListCopy(vc);
//		bsSet(ctx->klenv, dycons(kc, ctx->klenv));
//		bsSet(ctx->vlenv, dycons(vc, ctx->vlenv));
		BSVM_LexBind(ctx, kc, vc);
	}
}

void BSVM_BindArgs2(BSVM_SVMState *ctx, dytf kl, dytf vl)
{
	dytf kc, vc;
	dytf kv, vv;
	char *sig;

	if(dytfNullP(kl) && dytfNullP(vl))
		return;

	kc=kl; vc=vl;
	while(dytfConsP(kc) && !dytfNullP(vc))
	{
		kv=dytfCarF(kc);
		vv=dytfCarF(vc);
	
		if(BSVM_CheckInfoRest(ctx, kv))
		{
			vc=dytfListCopy(vc);
//			bsSet(ctx->klenv, dycons(dycar(kc), ctx->klenv));
//			bsSet(ctx->vlenv, dycons(vc, ctx->vlenv));
			BSVM_LexBind(ctx, kv, vc);
			return;
		}

#ifdef BSVM_USE_BVT
		sig=BSVM_GetInfoSig(ctx, kv);
		if(sig)
		{
			switch(*sig)
			{
			case 'i':
				vv=dytfInt(dytfIntv(vv));
//				if(!BSVM_BoxedIntP(ctx, vv))
//					vv=dytfPtrF(BSVM_WrapAllocInt(ctx, dytfIntv(vv)));
				break;
			case 'x':
				vv=dytfLong(dytfLongv(vv));
//				if(!BSVM_BoxedLongP(ctx, vv))
//					vv=dytfPtrF(BSVM_WrapAllocLong(ctx, dytfLongv(vv)));
				break;
			case 'f':
				vv=dytfFloat(dytfFloatv(vv));
//				if(!BSVM_BoxedFloatP(ctx, vv))
//					vv=dytfPtrF(BSVM_WrapAllocFloat(ctx, dytfFloatv(vv)));
				break;
			case 'd':
				vv=dytfDouble(dytfDoublev(vv));
//				if(!BSVM_BoxedDoubleP(ctx, vv))
//					vv=dytfPtrF(BSVM_WrapAllocDouble(ctx, dytfDoublev(vv)));
				break;
			default: break;
			}
		}
#endif

//		bsSet(ctx->klenv, dycons(dycar(kc), ctx->klenv));
//		bsSet(ctx->vlenv, dycons(dycar(vc), ctx->vlenv));
		BSVM_LexBind(ctx, kv, vv);
		kc=dytfCdrF(kc); vc=dytfCdrF(vc);
	}

	if(dytfSymbolP(kc))
	{
		vc=dytfListCopy(vc);
//		bsSet(ctx->klenv, dycons(kc, ctx->klenv));
//		bsSet(ctx->vlenv, dycons(vc, ctx->vlenv));
		BSVM_LexBind(ctx, kc, vc);
	}
}

void BSVM_CallObj_SvmBlock(BSVM_SVMState *ctx, dytf obj, dytf f)
{
//	dytf targ[64];
	BSVM_SVMBlock *blk;
//	BSVM_SVMLambda *lmb;
//	BSVM_SVMBuiltin *bfcn;
//	char *ty;
//	dytf *ap;
	dytf t, u, l, x;
//	int narg;

	blk=(BSVM_SVMBlock *)dytfPtrvF(f);

#if 1
	if(ctx->flowstackpos>=0)
	{
		if(blk->flags&BGBDYC_FLAG_ASYNC)
		{
			BSVM_CallObjAsync(ctx, obj, f);
			return;
		}
	}
#endif

	l=BSVM_PopList(ctx);
	BSVM_PushFrame(ctx);

	bsSet(ctx->self, obj);
	bsSet(ctx->lit, dytfWrap(blk->lit));
	bsSet(ctx->func, f);
//	bsSet(ctx->klenv, NULL);
//	bsSet(ctx->vlenv, NULL);
//	bsSet(ctx->oklenv, NULL);
//	bsSet(ctx->ovlenv, NULL);
//	bsSet(ctx->okdenv, ctx->kdenv);
//	bsSet(ctx->ovdenv, ctx->vdenv);

	if(dytfDycObjectP(obj))
	{
		ctx->this_obj=dytfDycObjectvF(obj);
		ctx->this_inf=dycGetObjClass(ctx->this_obj);
	}else
	{
		ctx->this_obj=NULL;
		ctx->this_inf=NULL;
	}

#ifdef BSVM_JIT
	ctx->thunk=blk->thunk;
#endif
#ifdef BSVM_THREADCODE
	ctx->thop=blk->thops;
#endif

	ctx->ip=blk->body;
	ctx->ipe=blk->body+blk->szbody;
	ctx->vmflags=blk->vmflags;

//	BSVM_BindArgs2(ctx, blk->args, l);
//	BSVM_BindArgs(ctx, blk->args, l);

	if(blk->fargs)
		BSVM_BindArgs2(ctx, dytfWrap(blk->fargs), l);
	else BSVM_BindArgs(ctx, dytfWrap(blk->args), l);

	dytfFreeListF(l);

#if 1
	if(blk->flags&(BGBDYC_FLAG_SETUID|BGBDYC_FLAG_SETGID))
	{
		if(blk->flags&BGBDYC_FLAG_SETUID)
		{
			ctx->access&=~BGBDY_ACCESS_UIDMASK;
			ctx->access|=blk->access&BGBDY_ACCESS_UIDMASK;
		}
		if(blk->flags&BGBDYC_FLAG_SETGID)
		{
			ctx->access&=~BGBDY_ACCESS_GIDMASK;
			ctx->access|=blk->access&BGBDY_ACCESS_GIDMASK;
		}
		dySetAccessMode(ctx->access);
	}
#endif
}

void BSVM_CallObj_SvmLambda(BSVM_SVMState *ctx, dytf obj, dytf f)
{
//	dytf targ[64];
	BSVM_SVMBlock *blk;
	BSVM_SVMLambda *lmb;
//	BSVM_SVMBuiltin *bfcn;
//	char *ty;
//	dytf *ap;
	dytf t, u, l, x;
//	int narg;

	lmb=(BSVM_SVMLambda *)dytfPtrvF(f);
	blk=lmb->blk;

#if 1
	if(ctx->flowstackpos>=0)
	{
		if(blk->flags&BGBDYC_FLAG_ASYNC)
		{
			BSVM_CallObjAsync(ctx, obj, f);
			return;
		}
	}
#endif

	l=BSVM_PopList(ctx);
	BSVM_PushFrame(ctx);

	bsSet(ctx->self, obj);
	bsSet(ctx->top, lmb->top);
	bsSet(ctx->pkgname, lmb->pkgname);
	bsSet(ctx->lit, dytfWrap(lmb->blk->lit));
	bsSet(ctx->func, f);
	bsSet(ctx->klenv, lmb->lkey);
	bsSet(ctx->vlenv, lmb->lval);
	bsSet(ctx->oklenv, lmb->lkey);
	bsSet(ctx->ovlenv, lmb->lval);

	bsSet(ctx->okdenv, ctx->kdenv);
	bsSet(ctx->ovdenv, ctx->vdenv);

	if(dytfDycObjectP(obj))
	{
		ctx->this_obj=dytfDycObjectvF(obj);
		ctx->this_inf=dycGetObjClass(ctx->this_obj);
	}else
	{
		ctx->this_obj=NULL;
		ctx->this_inf=NULL;
	}

#ifdef BSVM_JIT
	ctx->thunk=lmb->blk->thunk;
#endif
#ifdef BSVM_THREADCODE
	ctx->thop=blk->thops;
#endif

	ctx->ip=lmb->blk->body;
	ctx->ipe=blk->body+blk->szbody;
	ctx->vmflags=blk->vmflags;

//	BSVM_BindArgs(ctx, lmb->blk->args, l);
	if(blk->fargs)
		BSVM_BindArgs2(ctx, dytfWrap(blk->fargs), l);
	else BSVM_BindArgs(ctx, dytfWrap(blk->args), l);
	dytfFreeList(l);

#if 1
	if(blk->flags&(BGBDYC_FLAG_SETUID|BGBDYC_FLAG_SETGID))
	{
		if(blk->flags&BGBDYC_FLAG_SETUID)
		{
			ctx->access&=~BGBDY_ACCESS_UIDMASK;
			ctx->access|=blk->access&BGBDY_ACCESS_UIDMASK;
		}
		if(blk->flags&BGBDYC_FLAG_SETGID)
		{
			ctx->access&=~BGBDY_ACCESS_GIDMASK;
			ctx->access|=blk->access&BGBDY_ACCESS_GIDMASK;
		}
		dySetAccessMode(ctx->access);
	}
#endif
}

void BSVM_CallObj_SvmBuiltin(BSVM_SVMState *ctx, dytf obj, dytf f)
{
//	dytf targ[64];
//	BSVM_SVMBlock *blk;
//	BSVM_SVMLambda *lmb;
	BSVM_SVMBuiltin *bfcn;
//	char *ty;
//	dytf *ap;
	dytf t, u, l, x;
//	int narg;

	l=BSVM_PopList(ctx);
	bfcn=(BSVM_SVMBuiltin *)dytfPtrvF(f);

//	dyPrintf("Builtin %s\n", bfcn->name);

	u=DYTF_NULL; x=DYTF_NULL;
	bsSet(u, ctx->self);
	bsSet(x, ctx->func);
	bsSet(ctx->self, obj);
	bsSet(ctx->func, f);

	t=bfcn->fcn(ctx, l);
	bsSet(ctx->self, u);
	bsSet(ctx->func, x);

	BSVM_Push(ctx, t);
	dytfFreeList(l);
}

void BSVM_CallObj_Default(BSVM_SVMState *ctx, dytf obj, dytf f)
{
//	dytf targ[64];
//	BSVM_SVMBlock *blk;
//	BSVM_SVMLambda *lmb;
//	BSVM_SVMBuiltin *bfcn;
//	char *ty;
	dytf *ap;
	dytf t, u, l, x;
	int narg;

	ap=BSVM_PopMarkArgs(ctx, &narg);
	t=dytfApplyMethod(obj, f, ap, narg);
	BSVM_Push(ctx, t);

//	l=BSVM_PopList(ctx);
//	t=dytfApplyMethodLst(obj, f, l);
//	BSVM_Push(ctx, t);
//	dytfFreeList(l);
}

void BSVM_CallObj(BSVM_SVMState *ctx, dytf obj, dytf f)
{
	dytf targ[64];
	BSVM_SVMBlock *blk;
	BSVM_SVMLambda *lmb;
	BSVM_SVMBuiltin *bfcn;
	char *ty;
	dytf *ap;
	dytf t, u, l, x;
	int narg;

	if(dytfNullP(f) || dytfUndefinedP(f))
	{
		return;
	}

//	dyPrintf("call %s\n", BSVM_TY_GetTypeName(f));

	ty=dytfgettype(f);

	if(!strcmp(ty, "svm_blk_t"))
	{
		BSVM_CallObj_SvmBlock(ctx, obj, f);
		return;
	}

	if(!strcmp(ty, "svm_lambda_t"))
	{
		BSVM_CallObj_SvmLambda(ctx, obj, f);
		return;
	}

	if(!strcmp(ty, "svm_builtin_t"))
	{
		BSVM_CallObj_SvmBuiltin(ctx, obj, f);
		return;
	}

	if(dytfSymbolP(f))
	{
		l=BSVM_PopList(ctx);
		t=dytfCallN(obj, dytfSymbolvF(f), l);
		BSVM_Push(ctx, t);
		dytfFreeList(l);
		return;
	}

	BSVM_CallObj_Default(ctx, obj, f);
	return;
}

void BSVM_TailCallObj(BSVM_SVMState *ctx, dytf obj, dytf f)
{
	BSVM_SVMBlock *blk;
	BSVM_SVMLambda *lmb;
	BSVM_SVMBuiltin *bfcn;
	char *ty;
	dytf t, l, x;

	ty=dytfgettype(f);

	if(!strcmp(ty, "svm_blk_t"))
	{
//		if(ctx->klenv && !ctx->vlenv)
//			BSVM_ShiftMark(ctx);

		blk=(BSVM_SVMBlock *)dytfPtrvF(f);

		l=BSVM_PopList(ctx);

		ctx->self=obj;
		ctx->lit=dytfWrap(blk->lit);
		ctx->func=f;
		ctx->klenv=DYTF_NULL;
		ctx->vlenv=DYTF_NULL;
		ctx->oklenv=DYTF_NULL;
		ctx->ovlenv=DYTF_NULL;

		ctx->okdenv=ctx->kdenv;
		ctx->ovdenv=ctx->vdenv;

		ctx->ip=blk->body;
		ctx->ipe=blk->body+blk->szbody;
		ctx->vmflags=blk->vmflags;

#ifdef BSVM_JIT
		ctx->thunk=blk->thunk;
#endif
#ifdef BSVM_THREADCODE
		ctx->thop=blk->thops;
#endif
//		BSVM_BindArgs(ctx, blk->args, l);
		if(blk->fargs)
			BSVM_BindArgs2(ctx, dytfWrap(blk->fargs), l);
		else BSVM_BindArgs(ctx, dytfWrap(blk->args), l);
//		BSVM_GC_CheckRef(l);
		dytfFreeList(l);

		return;
	}

	if(!strcmp(ty, "svm_lambda_t"))
	{
//		if(ctx->klenv && !ctx->vlenv)
//			BSVM_ShiftMark(ctx);

		l=BSVM_PopList(ctx);
		lmb=(BSVM_SVMLambda *)dytfPtrvF(f);
		blk=lmb->blk;

		ctx->self=obj;
		ctx->top=lmb->top;
		ctx->lit=dytfWrap(lmb->blk->lit);
		ctx->func=f;
		ctx->klenv=lmb->lkey;
		ctx->vlenv=lmb->lval;
		ctx->oklenv=lmb->lkey;
		ctx->ovlenv=lmb->lval;

		ctx->okdenv=ctx->kdenv;
		ctx->ovdenv=ctx->vdenv;

		ctx->ip=lmb->blk->body;
		ctx->ipe=blk->body+blk->szbody;
		ctx->vmflags=blk->vmflags;

#ifdef BSVM_JIT
		ctx->thunk=lmb->blk->thunk;
#endif
#ifdef BSVM_THREADCODE
		ctx->thop=blk->thops;
#endif
//		BSVM_BindArgs(ctx, lmb->blk->args, l);
		if(blk->fargs)
			BSVM_BindArgs2(ctx, dytfWrap(blk->fargs), l);
		else BSVM_BindArgs(ctx, dytfWrap(blk->args), l);
//		BSVM_GC_CheckRef(l);
		dytfFreeList(l);

		return;
	}

	if(!strcmp(ty, "svm_builtin_t"))
	{
//		if(ctx->klenv && !ctx->vlenv)
//			BSVM_ShiftMark(ctx);

		l=BSVM_PopList(ctx);
		bfcn=(BSVM_SVMBuiltin *)dytfPtrvF(f);

		x=DYTF_NULL;
		bsSet(x, ctx->func);

		bsSet(ctx->self, obj);
		bsSet(ctx->func, f);
		t=bfcn->fcn(ctx, l);
		bsSet(ctx->func, x);

		BSVM_PopFrame(ctx);
		BSVM_Push(ctx, t);
		dytfFreeList(l);
		return;
	}

	if(dytfSymbolP(f))
	{
//		if(ctx->klenv && !ctx->vlenv)
//			BSVM_ShiftMark(ctx);
		l=BSVM_PopList(ctx);
		t=dytfCallN(obj, dytfSymbolvF(f), l);
		BSVM_PopFrame(ctx);
		BSVM_Push(ctx, t);
		dytfFreeList(l);
		return;
	}

//	if(ctx->klenv && !ctx->vlenv)
//		BSVM_ShiftMark(ctx);
	l=BSVM_PopList(ctx);
	t=dytfApplyMethodLst(obj, f, l);
	BSVM_PopFrame(ctx);
	BSVM_Push(ctx, t);
	dytfFreeList(l);
	return;
}

void BSVM_MethodCall(BSVM_SVMState *ctx, dytf obj, dytf key)
{
	dytf f, t, l;

#if 1
//	if(ctx->this_obj && ctx->this_inf)
	if(dytfDycObjectP(obj))
	{
		l=BSVM_StackPeekList(ctx);
		t=dytfWrap(dycLookupMethodArgsN(
			dytfDycObjectvF(obj), dytfSymbolvF(key), dytfUnwrap(l)));
		dytfFreeList(l);			

//		t=dycLookupMethodSig(obj, (char *)key, "?");
		if(!dytfNullP(t) && !dytfUndefinedP(t))
		{
			BSVM_CallObj(ctx, obj, t);
			return;
		}
	}

	if(dytfDycClassP(obj))
	{
		l=BSVM_StackPeekList(ctx);
		t=dytfWrap(dycLookupStaticMethodArgsN(
			dytfDycClassvF(obj), dytfSymbolvF(key), dytfUnwrap(l)));
		dytfFreeList(l);			

//		t=dycLookupMethodSig(obj, (char *)key, "?");
		if(!dytfNullP(t) && !dytfUndefinedP(t))
		{
			BSVM_CallObj(ctx, obj, t);
			return;
		}
	}
#endif

	f=BSVM_LoadIndex(ctx, obj, key);
	if(!dytfNullP(f) && !dytfUndefinedP(f))
	{
		BSVM_CallObj(ctx, obj, f);
		return;
	}

	l=BSVM_PopList(ctx);
	t=dytfCallN(obj, dytfSymbolvF(key), l);
	BSVM_Push(ctx, t);
	dytfFreeList(l);
	return;
}

void BSVM_MethodTailCall(BSVM_SVMState *ctx, dytf obj, dytf key)
{
	dytf f, t, l;

	f=BSVM_LoadIndex(ctx, obj, key);
	if(!dytfNullP(f) && !dytfUndefinedP(f))
	{
		BSVM_TailCallObj(ctx, obj, f);
		return;
	}

	l=BSVM_PopList(ctx);
	t=dytfCallN(obj, dytfSymbolvF(key), l);
	BSVM_PopFrame(ctx);
	BSVM_Push(ctx, t);
	dytfFreeList(l);
	return;
}

void BSVM_CallObjAsync(BSVM_SVMState *ctx, dytf obj, dytf f)
{
	BSVM_SVMState *ctx2;
	dytf l;

	l=BSVM_PopList(ctx);
	
	ctx2=BSVM_NewState();
	ctx2->flowstackpos=-1;
	ctx2->top=ctx->top;
	ctx2->access=ctx->access;

	BSVM_PushList(ctx2, l);
	BSVM_CallObj(ctx2, obj, f);
	BSVM_RunThread(ctx2);

//	dyFreeList(l);

	BSVM_Push(ctx, dytfPtrF(ctx2));
}

int BSVM_CheckDefined(BSVM_SVMState *ctx, dytf sym)
{
	char *s;
	dytf t;
	
	s=BSVMC_LookupPPKey(NULL, dytfSymbolv(sym));
	if(s)return(1);
	
	t=BSVM_Lookup(ctx, sym);
	if(!dytfUndefinedP(t))return(1);
	
	return(0);
}

dytf BSVM_Close(BSVM_SVMState *ctx, dytf f)
{
	BSVM_SVMBlock *blk;
	BSVM_SVMLambda *lmb;
	dytf t;

	if(dytftypep(f, "svm_blk_t"))
	{
		blk=(BSVM_SVMBlock *)dytfPtrvF(f);

		lmb=gctalloc("svm_lambda_t", sizeof(BSVM_SVMLambda));
		bsSet(lmb->lkey, ctx->klenv);
		bsSet(lmb->lval, ctx->vlenv);
		bsSet(lmb->top, ctx->top);
		bsSet(lmb->pkgname, ctx->pkgname);
		lmb->blk=blk;
		return(dytfPtrF(lmb));
	}

	return(f);
}

dytf BSVM_EnvClose(BSVM_SVMState *ctx, dytf f, dytf l)
{
	BSVM_SVMBlock *blk;
	BSVM_SVMLambda *lmb;
	dytf t;

	if(dytftypep(f, "svm_blk_t"))
	{
		blk=(BSVM_SVMBlock *)dytfPtrvF(f);

		lmb=gctalloc("svm_lambda_t", sizeof(BSVM_SVMLambda));
//		bsSet(lmb->lkey, ctx->klenv);
//		bsSet(lmb->lval, ctx->vlenv);
		bsSet(lmb->lkey, dytfWrap(blk->lenv));
		bsSet(lmb->lval, l);
		bsSet(lmb->top, ctx->top);
		bsSet(lmb->pkgname, ctx->pkgname);
		lmb->blk=blk;
		return(dytfPtrF(lmb));
	}

	return(f);
}
