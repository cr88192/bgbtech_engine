#include <bgbsvm.h>

int BSVM_dyIntvF(dytf v)
	{ return(dytfIntv(v)); }
dytf BSVM_dyIntF(int v)
	{ return(dytfInt(v)); }

double BSVM_dyFloatvF(dytf v)
	{ return(dytfFloatv(v)); }
dytf BSVM_dyFloatF(int v)
	{ return(dytfFloat(v)); }

BSVM_ThreadOp *BSVM_ThOp_NOP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BLOCK(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->rs=BSVM_RETS_BLOCK;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DBGMARK(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	ctx->dbg_fn=dystringv(dytfPtrvF(cur->t));
	ctx->dbg_fn=dytfStringvF(cur->t);
	ctx->dbg_ln=cur->i;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CGENAST(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	ctx->dbg_fn=dystringv(dytfPtrvF(cur->t));
	ctx->dbg_fn=dytfStringvF(cur->t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->dbg_ln=cur->i;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LABEL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SLEEP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->rs=BSVM_RETS_SLEEP;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SLEEP_SV(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, dytfInt(cur->i));
	ctx->rs=BSVM_RETS_SLEEP;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ASSERT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	
	t=BSVM_StackPeekTop(ctx);
	if(!dytfTrueP(t))
		ctx->rs=BSVM_RETS_ASSERT;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JOIN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_OpSetupJoin(ctx, t);
	ctx->rs=BSVM_RETS_BLOCK;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MULTIJOIN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_PopList(ctx);
	BSVM_OpSetupJoin(ctx, t);
	ctx->rs=BSVM_RETS_BLOCK;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_Push(ctx, cur->t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_POP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PopDestroy(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOAD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Lookup(ctx, cur->t);
	t=dytfCopyValue(t);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STORE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	BSVM_Assign(ctx, cur->t, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BIND(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	BSVM_Bind(ctx, cur->t, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DYNBIND(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	BSVM_DynBind(ctx, cur->t, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LEXBIND(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	BSVM_LexBind(ctx, cur->t, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADA_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupAddr(ctx, cur->t);
	BSVM_Push(ctx, t);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_LOADB_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupRef(ctx, cur->t);
	BSVM_Push(ctx, t);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	u=BSVM_Pop(ctx);
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, u);
	v=dytfCopyValue(v);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
//	v=BSVM_Pop(ctx);
//	u=BSVM_Pop(ctx);
//	t=BSVM_Pop(ctx);

	u=BSVM_Pop(ctx);
	t=BSVM_Pop(ctx);
	v=BSVM_Pop(ctx);
	BSVM_StoreIndex(ctx, t, u, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEXA(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	u=BSVM_Pop(ctx);
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndexAddr(ctx, t, u);
	v=dytfCopyValue(v);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, v;
	t=BSVM_Pop(ctx);
//	v=BSVM_LoadIndex(ctx, t, cur->t);
	v=dytfGet(t, dytfSymbolvF(cur->t));
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx);
	v=BSVM_Pop(ctx);
//	BSVM_StoreIndex(ctx, u, cur->t, v);
	dytfSet(u, dytfSymbolvF(cur->t), v);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEXA_S(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndexAddr(ctx, t, cur->t);
//	v=dytfGetRef(t, dytfSymbolvF(cur->t));
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEXB_S(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndexRef(ctx, t, cur->t);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_I(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, cur->t);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_I(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_Pop(ctx);
	BSVM_StoreIndex(ctx, t, cur->t, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEXA_I(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndexAddr(ctx, t, cur->t);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEXB_I(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndexRef(ctx, t, cur->t);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_0(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, BSVM_dyIntF(0));
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_1(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, BSVM_dyIntF(1));
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, BSVM_dyIntF(2));
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_3(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, BSVM_dyIntF(3));
	BSVM_Push(ctx, v);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_0(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_StoreIndex(ctx, t, BSVM_dyIntF(0), u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_1(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_StoreIndex(ctx, t, BSVM_dyIntF(1), u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_StoreIndex(ctx, t, BSVM_dyIntF(2), u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_3(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_StoreIndex(ctx, t, BSVM_dyIntF(3), u);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_EXCH_SELF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	u=ctx->stack[ctx->stackpos-1]; v=ctx->self;
	ctx->self=u; ctx->stack[ctx->stackpos-1]=v;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_TOP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	u=ctx->stack[ctx->stackpos-1]; v=ctx->top;
	ctx->top=u; ctx->stack[ctx->stackpos-1]=v;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_SELF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, ctx->self);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_NULL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, DYTF_NULL);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_TRUE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, DYTF_TRUE);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_FALSE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, DYTF_FALSE);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_SV(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, BSVM_dyIntF(cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_SV_F(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, dytfFloat((cur->i)/100.0));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_SV_FB(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, dytfFloat((cur->i)/10000.0));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_SV_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, dytfChar(cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_0(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(0)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_1(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(1)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(2)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_3(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(3)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_4(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(4)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_5(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(5)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_6(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(6)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_7(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_PushF(ctx, BSVM_dyIntF(7)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_CF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BSVM_Push(ctx, ctx->func); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_LLOAD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_AssignIdx(ctx, cur->i, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_F(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_PushF(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE_F(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_PopF(ctx);
	BSVM_AssignIdxF(ctx, cur->i, t);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
//	t=BSVM_LookupIdx(ctx, cur->i);
	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
//	BSVM_AssignIdx(ctx, cur->i, t);
	ctx->levstack[ctx->lestackpos-(cur->i+1)]=t;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_FVS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
//	t=BSVM_LookupIdx(ctx, cur->i);
	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
//	BSVM_PushF(ctx, t);
	ctx->stack[ctx->stackpos++]=t;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE_FVS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
//	t=BSVM_PopF(ctx);
	t=ctx->stack[--ctx->stackpos];
//	BSVM_AssignIdxF(ctx, cur->i, t);
	ctx->levstack[ctx->lestackpos-(cur->i+1)]=t;
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_LPOSTINC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_LookupIdx(ctx, cur->i);
	u=dytfInc(t);
	BSVM_AssignIdx(ctx, cur->i, u);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LPOSTDEC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_LookupIdx(ctx, cur->i);
	u=dytfDec(t);
	BSVM_AssignIdx(ctx, cur->i, u);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LPREINC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_LookupIdx(ctx, cur->i);
	u=dytfInc(t);
	BSVM_AssignIdx(ctx, cur->i, u);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LPREDEC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_LookupIdx(ctx, cur->i);
	u=dytfDec(t);
	BSVM_AssignIdx(ctx, cur->i, u);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LINC_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_AssignIdxF(ctx, cur->i, BSVM_dyIntF(BSVM_dyIntvF(t)+1));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LDEC_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_AssignIdxF(ctx, cur->i, BSVM_dyIntF(BSVM_dyIntvF(t)-1));
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_CLEARENV(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_ClearEnv(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BINDPATTERN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_LOADMINDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_STOREMINDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_TRYLOAD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Lookup(ctx, cur->t);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TRYCAST_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	u=BSVM_TryCast(ctx, cur->t, u);
	if(dytfUndefinedP(u))u=DYTF_NULL;
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CAST_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	u=BSVM_Cast(ctx, cur->t, u);
	BSVM_Push(ctx, u);
//	if(!u || (u==UNDEFINED))
	if(dytfUndefinedP(u))
		{ BSVM_OpThrowName(ctx, "CastFailException");
			ctx->rs=BSVM_RETS_THROW; }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CAST(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx);
	u=BSVM_Cast(ctx, t, u);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SIZEOF_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_OpSizeOf(ctx, cur->t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SIZEOF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=BSVM_OpSizeOf(ctx, t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_INSTANCEOF_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
//	u=BSVM_TryCast(ctx, cur->t, u);
//	if(dytfUndefinedP(u))u=DYTF_NULL;
	BSVM_PushBool(ctx, BSVM_CheckInstanceOf(ctx, cur->t, u));
	return(cur->next);
}


BSVM_ThreadOp *BSVM_ThOp_MARK(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->markstack[ctx->markstackpos++]=ctx->stackpos;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TYMARK(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->markstack[ctx->markstackpos++]=ctx->stackpos;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CALL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_CallObj(ctx, ctx->self, t);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TAILCALL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_TailCallObj(ctx, ctx->self, t);
	if(ctx->flowstackpos<0)
	{
		ctx->rs=BSVM_RETS_RETURN;
//		return(cur->next);
		return(NULL);
	}
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CALL_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	dytf t, u;
//	u=BSVM_Lookup(ctx, cur->t);
//	if(u==UNDEFINED)
//		printf("Fail Lookup %s\n", dysymbolv(cur->t));
	ctx->thop=cur->next;
	BSVM_Thread_LookupCallObj(ctx, cur, cur->t);
//	BSVM_LookupCallObj(ctx, cur->t, &(cur->v));
//	BSVM_CallObj(ctx, ctx->self, u);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TAILCALL_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Lookup(ctx, cur->t);
	ctx->thop=cur->next;
	BSVM_TailCallObj(ctx, ctx->self, t);
	if(ctx->flowstackpos<0)
	{
		ctx->rs=BSVM_RETS_RETURN;
//		return(cur->next);
		return(NULL);
	}
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_METHODCALL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_MethodCall(ctx, u, t);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_METHODTAILCALL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_MethodTailCall(ctx, u, t);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_METHODCALL_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_MethodCall(ctx, u, cur->t);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_METHODTAILCALL_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_MethodTailCall(ctx, u, cur->t);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CALL_ASYNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_CallObjAsync(ctx, ctx->self, t);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CALL_IFDEF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx);
	if(BSVM_CheckDefined(ctx, cur->t))
	{
		ctx->thop=cur->next;
		BSVM_CallObj(ctx, ctx->self, u);
		return(ctx->thop);
	}else
	{
		v=BSVM_PopList(ctx);
		BSVM_Push(ctx, DYTF_NULL);
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CALL_IFNDEF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx);
	if(!BSVM_CheckDefined(ctx, cur->t))
	{
		ctx->thop=cur->next;
		BSVM_CallObj(ctx, ctx->self, u);
		return(ctx->thop);
	}else
	{
		v=BSVM_PopList(ctx);
		BSVM_Push(ctx, DYTF_NULL);
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_SYNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginSync(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_SYNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpEndSync(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_SYNC_OBJ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_OpBeginSyncObj(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOCK_OBJ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_OpBeginSyncObj(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_SYNC_OBJ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_OpEndSyncObj(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNLOCK_OBJ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_OpEndSyncObj(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_TRY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginTry(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_TRY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpEndTry(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_CATCH(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpEndCatch(ctx);
	if(!dytfNullP(ctx->thrown_obj))
		ctx->rs=BSVM_RETS_THROW;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_CATCH_FINAL(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginCatchFinal(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_CATCH(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginCatch(ctx, cur->t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_THROW_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpThrowS(ctx, cur->t);
	ctx->rs=BSVM_RETS_THROW;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_THROW_OBJ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_OpThrowObj(ctx, t);
	ctx->rs=BSVM_RETS_THROW;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CATCH(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_NEW_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_OpNew(ctx, cur->t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_NEWARRAY_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx);
	v=BSVM_OpNewArray(ctx, cur->t, u);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DELETE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=BSVM_OpDelete(ctx, t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CLONE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfClone(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TOSTRING(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfWrap(dystring(dytfToString(t)));
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNQUOTE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
//	u=dystring(dyToString(t));
	u=dytfWrap(BSVM_EvalExprsObj(
		dylist1(dytfUnwrap(t)), dytfUnwrap(ctx->self)));
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_NEG(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfNeg(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LNOT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfLNot(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DEFER(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfUMul(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SETDEFER(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, v;
	t=BSVM_Pop(ctx); v=BSVM_Pop(ctx);
	BSVM_StoreIndex(ctx, t, BSVM_dyIntF(0), v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_NOT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfNot(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_INC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfInc(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DEC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfDec(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_INC2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfAdd(t, dytfInt(2));
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DEC2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=dytfSub(t, dytfInt(2));
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_INC_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Lookup(ctx, cur->t);
	u=dytfInc(u);
	BSVM_Assign(ctx, cur->t, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DEC_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u;
	u=BSVM_Lookup(ctx, cur->t);
	u=dytfDec(u);
	BSVM_Assign(ctx, cur->t, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_POSTINC_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Lookup(ctx, cur->t);
	v=dytfInc(u);
	BSVM_Assign(ctx, cur->t, v);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_POSTDEC_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Lookup(ctx, cur->t);
	v=dytfDec(u);
	BSVM_Assign(ctx, cur->t, v);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PREINC_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Lookup(ctx, cur->t);
	v=dytfInc(u);
	BSVM_Assign(ctx, cur->t, v);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_PREDEC_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Lookup(ctx, cur->t);
	v=dytfDec(u);
	BSVM_Assign(ctx, cur->t, v);
	BSVM_Push(ctx, v);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_TYBIND(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w;
	w=BSVM_Pop(ctx);
	BSVM_OpTyBind(ctx, cur->t, cur->u, cur->v, w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TYDYNBIND(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w;
	w=BSVM_Pop(ctx);
	BSVM_OpTyDynBind(ctx, cur->t, cur->u, cur->v, w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TYLEXBIND(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w;
	w=BSVM_Pop(ctx);
	BSVM_OpTyLexBind(ctx, cur->t, cur->u, cur->v, w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_PACKAGE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginPackage(ctx, cur->u, cur->v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_CLASS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginClass(ctx, cur->u, cur->v, cur->t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_IFACE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginIface(ctx, cur->u, cur->v, cur->t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BEGIN_STRUCT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpBeginStruct(ctx, cur->u, cur->v, cur->t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_PACKAGE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpEndPackage(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_CLASS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpEndClass(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_IFACE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpEndIface(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_END_STRUCT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpEndStruct(ctx);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CLASS_IFACE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpClassIface(ctx, cur->u, cur->v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_IMPORT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_OpImport(ctx, cur->u, cur->v);
	return(cur->next);

}

BSVM_ThreadOp *BSVM_ThOp_RET(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->thop=cur->next;
	BSVM_PopFrame(ctx);
	if(ctx->flowstackpos<0)
	{
		ctx->rs=BSVM_RETS_RETURN;
//		return(cur->next);
		return(NULL);
	}
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CALL_CF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->thop=cur->next;
	BSVM_CallObj(ctx, ctx->self, ctx->func);
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_TAILCALL_CF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	ctx->thop=cur->next;
	BSVM_TailCallObj(ctx, ctx->self, ctx->func);
	if(ctx->flowstackpos<0)
	{
		ctx->rs=BSVM_RETS_RETURN;
//		return(cur->next);
		return(NULL);
	}
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CAR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCar(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CDR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCdr(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CAAR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCaar(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CDAR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCdar(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CADR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCadr(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CDDR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCddr(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CONS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfCons(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CLOSE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	t=BSVM_Close(ctx, t);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ENVCLOSE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, l;
	t=BSVM_Pop(ctx);
	l=BSVM_PopList(ctx);
	t=BSVM_EnvClose(ctx, t, l);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DUP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_PopF(ctx);
	v=dytfCopyValue(u);
	BSVM_PushF(ctx, u);
	BSVM_PushF(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DUP_F(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, ctx->stack[ctx->stackpos-1]);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_PopF(ctx); v=BSVM_PopF(ctx);
	BSVM_PushF(ctx, u); BSVM_PushF(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_INDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=ctx->stack[ctx->stackpos-(cur->i+1)];
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_RINDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	int j;

	j=ctx->markstack[ctx->markstackpos-1];
	t=ctx->stack[cur->i+j];
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SETINDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	ctx->stack[ctx->stackpos-(cur->i+1)]=t;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SETRINDEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	int j;

	t=BSVM_Pop(ctx);
	j=ctx->markstack[ctx->markstackpos-1];
	ctx->stack[cur->i+j]=t;
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ROLL_I(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, v;
	int i, j, k;

	i=cur->i; j=cur->j;
	if(!j)return(cur->next);
	if(j>0)
	{
		while(j>0)
		{
			t=ctx->stack[ctx->stackpos-1];
			for(k=0; k<i; k++)
			{
				v=ctx->stack[ctx->stackpos-k-2];
				ctx->stack[ctx->stackpos-k-1]=v;
			}
			ctx->stack[ctx->stackpos-i]=t;
			j--;
		}
	}else
	{
		j=-j;
		while(j>0)
		{
			t=ctx->stack[ctx->stackpos-i];
			for(k=0; k<i; k++)
			{
				v=ctx->stack[ctx->stackpos-i+k+1];
				ctx->stack[ctx->stackpos-i+k]=v;
			}
			ctx->stack[ctx->stackpos-1]=t;
			j--;
		}
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_COPY_I(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, v;
	int i;

	for(i=0; i<cur->i; i++)
	{
		t=ctx->stack[ctx->stackpos-cur->i+i];
		v=dytfCopyValue(t);
		ctx->stack[ctx->stackpos+i]=v;
	}
	ctx->stackpos+=cur->i;
	return(cur->next);
}

#if 0
// BSVM_ThreadOp *BSVM_ThOp_DUP_R(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	BSVM_Push(ctx, t);
//	bsSet(ctx->reg[cur->i], t);
	ctx->reg[cur->i]=t;
	return(cur->next);
}

// BSVM_ThreadOp *BSVM_ThOp_DUP_RF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_PopF(ctx);
	BSVM_PushF(ctx, t);
	ctx->reg[cur->i]=t;
	return(cur->next);
}

// BSVM_ThreadOp *BSVM_ThOp_PUSH_R(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_Push(ctx, ctx->reg[cur->i]);
	return(cur->next);
}

// BSVM_ThreadOp *BSVM_ThOp_PUSH_RF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_PushF(ctx, ctx->reg[cur->i]);
	return(cur->next);
}

// BSVM_ThreadOp *BSVM_ThOp_POP_R(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_PopF(ctx);
	ctx->reg[cur->i]=t;
	return(cur->next);
}
#endif

BSVM_ThreadOp *BSVM_ThOp_UNARYOP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	t=BSVM_Unary(ctx, cur->i, t);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BINARYOP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	v=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx);
	t=BSVM_Binary(ctx, cur->i, u, v);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYOP_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	t=BSVM_UnaryS(ctx, dysymbolv(dytfPtrvF(cur->t)), t);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BINARYOP_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	v=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx);
	t=BSVM_BinaryS(ctx, dysymbolv(dytfPtrvF(cur->t)), u, v);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ADD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfAdd(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SUB(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfSub(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MUL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfMul(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DIV(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfDiv(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MOD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfMod(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_AND(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfAnd(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_OR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfOr(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_XOR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfXor(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfShl(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfShr(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHRR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfShr(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_E(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfEqP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_NE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfNEqP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_L(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfLtP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_G(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfGtP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_LE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfLeP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_GE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfGeP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_EQ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfEqqP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_NEQ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfNEqqP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_U(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	BSVM_Push(ctx, dytfBool(dytfUnP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	return((BSVM_ThreadOp *)(cur->data));
}

BSVM_ThreadOp *BSVM_ThOp_JMP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	if(cur->data)
//		return((BSVM_ThreadOp *)(cur->data));

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)cur->fcn=BSVM_ThOp_JMP_2;
	return((BSVM_ThreadOp *)(cur->data));

//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_TRUE_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i;
	i=BSVM_PopBool(ctx);
	if(i)return((BSVM_ThreadOp *)(cur->data));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_FALSE_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i;
	i=BSVM_PopBool(ctx);
	if(!i)return((BSVM_ThreadOp *)(cur->data));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_TRUE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
	{
		cur->fcn=BSVM_ThOp_JMP_TRUE_2;
		return(cur->fcn(ctx, cur));
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_FALSE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
	{
		cur->fcn=BSVM_ThOp_JMP_FALSE_2;
		return(cur->fcn(ctx, cur));
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_COND_U_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i;
	i=BSVM_CondUnary(ctx, cur->j, BSVM_Pop(ctx));
	if(i)return((BSVM_ThreadOp *)(cur->data));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_COND_B_2(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	int i;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	i=BSVM_CondBinary(ctx, cur->j, u, v);
	if(i)return((BSVM_ThreadOp *)(cur->data));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_COND_U(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
	{
		cur->fcn=BSVM_ThOp_JMP_COND_U_2;
		return(cur->fcn(ctx, cur));
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_COND_B(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
	{
		cur->fcn=BSVM_ThOp_JMP_COND_B_2;
		return(cur->fcn(ctx, cur));
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_SWEQ(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

//	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
//	BSVM_Thread_CacheOpJump(ctx, cur);
//	if(cur->data)
	if(BSVM_Thread_CacheOpJump(ctx, cur))
	{
//		cur->fcn=BSVM_ThOp_JMP_COND_B_2;
//		return(cur->fcn(ctx, cur));
		u=BSVM_StackPeekTop(ctx);
		if(dytfEqP(u, cur->t))
		{
			BSVM_PopDestroy(ctx);
			return((BSVM_ThreadOp *)(cur->data));
		}
		return(cur->next);
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_SWEQF(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

//	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
//	BSVM_Thread_CacheOpJump(ctx, cur);
//	if(cur->data)
	if(BSVM_Thread_CacheOpJump(ctx, cur))
	{
		u=BSVM_StackPeekTop(ctx);
		if(dytfEqqP(u, cur->t))
		{
			BSVM_PopDestroy(ctx);
			return((BSVM_ThreadOp *)(cur->data));
		}
		return(cur->next);
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_SWDFL(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

//	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
//	BSVM_Thread_CacheOpJump(ctx, cur);
//	if(cur->data)
	if(BSVM_Thread_CacheOpJump(ctx, cur))
	{
		BSVM_PopDestroy(ctx);
		return((BSVM_ThreadOp *)(cur->data));
	}
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_SWB(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	int i;

//	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
//	if(cur->data)
	if(BSVM_Thread_CacheOpJump(ctx, cur))
	{
		u=BSVM_StackPeekTop(ctx);
		i=BSVM_CondBinary(ctx, cur->j, u, cur->t);
		if(i)
		{
			BSVM_PopDestroy(ctx);
			return((BSVM_ThreadOp *)(cur->data));
		}
		return(cur->next);
	}
	return(cur->next);
}

#if 1
BSVM_ThreadOp *BSVM_ThOp_JMP_E(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfEqP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfNEqP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_L(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfLtP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfGtP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfLeP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfGeP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_EQ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfEqqP(u, v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NEQ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfNEqqP(u, v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_U(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		if(dytfUnP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}
#endif

BSVM_ThreadOp *BSVM_ThOp_LDJMP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_ThreadOp *tmp;

	if(cur->data)
	{
		tmp=(BSVM_ThreadOp *)(cur->data);
		BSVM_Push(ctx, dytfPtrF(tmp->ip));
		return(cur->next);
	}
	tmp=BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	cur->data=(void *)tmp;
	BSVM_Push(ctx, dytfPtrF(tmp->ip));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMPIA(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_ThreadOp *tmp;
	dytf t;

	t=BSVM_Pop(ctx);
	if(dytftypep(t, "svm_blk_bytecode_t"))
	{
		tmp=BSVM_Thread_GetTarget(ctx, (byte *)dytfPtrvF(t));
		if(tmp)return(tmp);
	}

	printf("BSVM_SOP_JMPIA: Invalid Jump\n");
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LIST(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_PopList2(ctx);
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DICT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dyt t, u, v;

	t=BSVM_PopSimpleList(ctx);
	u=dyObject();
	v=t;
	while(dyconsp(v))
	{
		if(dykeywordp(dyCarF(v)))
			dySetDelegate(u, (char *)dyCarF(v), dycadr(v));
		else dyBind(u, (char *)dyCarF(v), dycadr(v));
			v=dycddr(v);
	}
	BSVM_Push(ctx, dytfWrap(u));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ATTR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;

	t=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx);
	v=dytfWrap(dyattr(dytfUnwrap(t), dytfUnwrap(u)));
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_VECTOR(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_PopList(ctx);
//	u=dyListToArray(t);
	u=BSVM_ListToVector(t);
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_COMPLEX_I(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_Pop(ctx);
	t=dytfWrap(dycomplex2(0, dytfRealv(t)));
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MATRIX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_PopList(ctx);
//	u=BSVM_TYMat_List2FMat(t);
	u=DYTF_UNDEFINED;
	BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_COMPLEX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u;
	t=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx);
	t=dytfWrap(dycomplex2(dytfRealv(u), dytfRealv(t)));
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_QUAT(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v, w;

	w=BSVM_Pop(ctx); v=BSVM_Pop(ctx);
	u=BSVM_Pop(ctx); t=BSVM_Pop(ctx);
	t=dytfWrap(dyquat(
		dytfRealv(t), dytfRealv(u),
		dytfRealv(v), dytfRealv(w)));
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ARRAY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dyt t, u;

	t=BSVM_PopSimpleList(ctx);
	u=dyListToArray(t);
	BSVM_Push(ctx, dytfWrap(u));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ARRAY_S(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	u=BSVM_PopList(ctx);
	v=BSVM_OpNewArrayLst(ctx, cur->t, u);
	BSVM_Push(ctx, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YXZ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	BSVM_Pop3(ctx, &t, &u, &v);
	BSVM_Push3(ctx, u, t, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YZX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	BSVM_Pop3(ctx, &t, &u, &v);
	BSVM_Push3(ctx, u, v, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZXY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	BSVM_Pop3(ctx, &t, &u, &v);
	BSVM_Push3(ctx, v, t, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZYX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	BSVM_Pop3(ctx, &t, &u, &v);
	BSVM_Push3(ctx, v, u, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_WXYZ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, w,x,y,z);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_WXZY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, w,x,z,y);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_WYXZ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, w,y,x,z);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_WYZX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, w,y,z,x);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_WZXY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, w,z,x,y);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_WZYX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, w,z,y,x);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_XWYZ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, x,w,y,z);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_XWZY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, x,w,z,y);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_XYWZ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, x,y,w,z);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_XYZW(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, x,y,z,w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_XZWY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, x,z,w,y);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_XZYW(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, x,z,y,w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YWXZ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, y,w,x,z);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YWZX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, y,w,z,x);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YXWZ(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, y,x,w,z);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YXZW(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, y,x,z,w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YZWX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, y,z,w,x);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_YZXW(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, y,z,x,w); 
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZWXY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, z,w,x,y);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZWYX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, z,w,y,x);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZXWY(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, z,x,w,y);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZXYW(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, z,x,y,w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZYWX(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, z,y,w,x);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXCH_ZYXW(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf w, x, y, z;
	BSVM_Pop4(ctx, &w, &x, &y, &z);
	BSVM_Push4(ctx, z,y,x,w);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_Default(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	BSVM_ThreadOp *nxt;

	ctx->thop=cur->next;
	ctx->ip=cur->ip;
	ctx->rs=BSVM_Step(ctx, 0);
	
	nxt=ctx->thop;
	if(nxt && (ctx->ip!=nxt->ip))
	{
		nxt=BSVM_Thread_GetTarget(ctx, ctx->ip);
		return(nxt);
	}
	
	return(nxt);
//	return(cur->next);
}


#if 1
BSVM_ThreadOp *BSVM_ThOp_INC_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i;
	i=ctx->stackpos-1;
	ctx->stack[i]=BSVM_dyIntF(BSVM_dyIntvF(ctx->stack[i])+1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DEC_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i;
	i=ctx->stackpos-1;
	ctx->stack[i]=BSVM_dyIntF(BSVM_dyIntvF(ctx->stack[i])-1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_INC2_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i;
	i=ctx->stackpos-1;
	ctx->stack[i]=BSVM_dyIntF(BSVM_dyIntvF(ctx->stack[i])+2);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DEC2_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i;
	i=ctx->stackpos-1;
	ctx->stack[i]=BSVM_dyIntF(BSVM_dyIntvF(ctx->stack[i])-2);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ADD_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)+BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SUB_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)-BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MUL_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)*BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DIV_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=
		BSVM_dyFloatF(((float)BSVM_dyIntvF(u))/BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_IDIV_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)/BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MOD_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)%BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_AND_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)&BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_OR_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)|BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_XOR_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)^BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_L_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)<dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_G_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)>dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_LE_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)<=dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_GE_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)>=dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_E_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)==dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_NE_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)!=dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ADD_FN_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int j;
	j=ctx->stackpos-1;
	ctx->stack[j]=BSVM_dyIntF(BSVM_dyIntvF(ctx->stack[j])+(cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SUB_FN_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int j;
	j=ctx->stackpos-1;
	ctx->stack[j]=BSVM_dyIntF(BSVM_dyIntvF(ctx->stack[j])-(cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MUL_FN_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int j;
	j=ctx->stackpos-1;
	ctx->stack[j]=BSVM_dyIntF(BSVM_dyIntvF(ctx->stack[j])*(cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHL_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)<<BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHR_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(u)>>BSVM_dyIntvF(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXP_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i, j, k;
	i=BSVM_dyIntvF(ctx->stack[--ctx->stackpos]);
	j=BSVM_dyIntvF(ctx->stack[ctx->stackpos-1]);
	k=1; while(i--)k*=j;
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(k);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHL_FN_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(t)<<(cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHR_FN_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	int j;
	j=BSVM_dyIntvF(ctx->stack[ctx->stackpos-1]);
	t=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(BSVM_dyIntvF(t)>>(cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_EXP_FN_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	int i, j, k;
	i=cur->i;
	j=BSVM_dyIntvF(ctx->stack[ctx->stackpos-1]);
	k=1; while(i--)k*=j;
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(k);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CONV_FN2FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfFloat(BSVM_dyIntvF(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CONV_FL2FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=BSVM_dyIntF(((int)dytfFloatv(t)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ADD_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatv(u)+dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SUB_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatvF(u)-dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MUL_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatv(u)*dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DIV_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatv(u)/dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_L_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)<dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_G_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)>dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_LE_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)<=dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_GE_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)>=dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_E_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)==dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_NE_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	v=ctx->stack[--ctx->stackpos];
	u=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)!=dytfFloatv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_NEG_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=ctx->stack[ctx->stackpos-1];
	ctx->stack[ctx->stackpos-1]=dytfFloat(-dytfFloatv(t));
	return(cur->next);

}
#endif

#if 1
BSVM_ThreadOp *BSVM_ThOp_JMP_E_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfEqqP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfNEqqP(u, v))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_E_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfFloatv(u)==dytfFloatv(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfFloatv(u)!=dytfFloatv(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}
#endif

#if 1
BSVM_ThreadOp *BSVM_ThOp_JMP_L_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(u)<BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(u)>BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(u)<=BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(u)>=BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_L_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfFloatv(u)<dytfFloatv(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfFloatv(u)>dytfFloatv(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfFloatv(u)<=dytfFloatv(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		if(dytfFloatv(u)>=dytfFloatv(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}
#endif

#if 1
BSVM_ThreadOp *BSVM_ThOp_JMP_E_LFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		u=BSVM_LookupIdx(ctx, cur->j);
		v=BSVM_LookupIdx(ctx, cur->k);
		if(BSVM_dyIntvF(u)==BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE_LFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		u=BSVM_LookupIdx(ctx, cur->j);
		v=BSVM_LookupIdx(ctx, cur->k);
		if(BSVM_dyIntvF(u)!=BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_L_LFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		u=BSVM_LookupIdx(ctx, cur->j);
		v=BSVM_LookupIdx(ctx, cur->k);
		if(BSVM_dyIntvF(u)<BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G_LFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		u=BSVM_LookupIdx(ctx, cur->j);
		v=BSVM_LookupIdx(ctx, cur->k);
		if(BSVM_dyIntvF(u)>BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_LFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		u=BSVM_LookupIdx(ctx, cur->j);
		v=BSVM_LookupIdx(ctx, cur->k);
		if(BSVM_dyIntvF(u)<=BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_LFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;

	if(cur->data)
	{
		u=BSVM_LookupIdx(ctx, cur->j);
		v=BSVM_LookupIdx(ctx, cur->k);
		if(BSVM_dyIntvF(u)>=BSVM_dyIntvF(v))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}
#endif

#if 1
BSVM_ThreadOp *BSVM_ThOp_JMP_E_ZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(!BSVM_dyIntvF(t))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE_ZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t))
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_L_ZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)<0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G_ZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)>0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_ZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)<=0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_ZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)>=0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}
#endif

#if 1
BSVM_ThreadOp *BSVM_ThOp_JMP_E_LZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)==0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE_LZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)!=0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_L_LZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)<0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G_LZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)>0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_LZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)<=0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_LZFN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)>=0)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_E_FNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)==cur->j)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE_FNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)!=cur->j)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_L_FNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)<cur->j)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G_FNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)>cur->j)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_FNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)<=cur->j)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_FNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=ctx->stack[--ctx->stackpos];
		if(BSVM_dyIntvF(t)>=cur->j)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_E_LFNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)==cur->k)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_NE_LFNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)!=cur->k)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_L_LFNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)<cur->k)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_G_LFNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)>cur->k)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_LFNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)<=cur->k)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_LFNC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;

	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(BSVM_dyIntvF(t)>=cur->k)
			return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}

	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)
		{ return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

#endif
