#include <bgbsvm.h>

BSVM_ThreadOp *BSVM_ThOp_CALL_THIS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycObject obj;
	dycClass inf;
	dycMethod minf;
	dyt t, l;
	dytf p;

	l=BSVM_PopSimpleList(ctx);

//	obj=(dycObject)ctx->self;
//	inf=dycGetObjClass(obj);
	obj=ctx->this_obj;
	inf=ctx->this_inf;
	minf=dycIdxMethodArgsN(inf, "<init>", l);

	t=dycGetMethodFunc(inf, minf);

	if(!t || (t==UNDEFINED) || (t==dytfPtrvF(ctx->func)))
	{
		printf("BSVM_ThOp_CALL_THIS: Bad Ctor %p\n", t);
		BSVM_Push(ctx, DYTF_UNDEFINED);
		return(cur->next);
	}

	BSVM_PushSimpleList(ctx, l);

//	t=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_CallObj(ctx, ctx->self, dytfPtrF(t));
	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CALL_SUPER(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycObject obj;
	dycClass inf;
	dycMethod minf;
	dyt t, l;
	dytf p;

	l=BSVM_PopSimpleList(ctx);

//	obj=(dycObject)ctx->self;
//	inf=dycGetObjClass(obj);
	obj=ctx->this_obj;
	inf=ctx->this_inf;
	inf=dycGetClassSuper(inf);
	minf=dycIdxMethodArgsN(inf, "<init>", l);

	t=dycGetMethodFunc(inf, minf);

	if(!t || (t==UNDEFINED) || (t==dytfPtrvF(ctx->func)))
	{
		printf("BSVM_ThOp_CALL_SUPER: Bad Ctor %p\n", t);
		BSVM_Push(ctx, DYTF_UNDEFINED);
		return(cur->next);
	}

	BSVM_PushSimpleList(ctx, l);

//	t=BSVM_Pop(ctx);
	ctx->thop=cur->next;
	BSVM_CallObj(ctx, ctx->self, dytfPtrF(t));
	ctx->this_inf=inf;

	return(ctx->thop);
//	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ADD_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfAdd(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SUB_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfSub(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MUL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfMul(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DIV_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfDiv(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_MOD_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfMod(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_AND_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfAnd(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_OR_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfOr(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_XOR_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfXor(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfShl(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHR_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfShr(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SHRR_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfShr(u, v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_EQ_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfBool(dytfEqP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_NE_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfBool(dytfNEqP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_LT_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfBool(dytfLtP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_GT_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfBool(dytfGtP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_LE_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfBool(dytfLeP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_CMP_GE_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u, v;
	u=BSVM_Pop(ctx); v=cur->t;
	BSVM_Push(ctx, dytfBool(dytfGeP(u, v)));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_Pop(ctx);
		if(dytfEqP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}


BSVM_ThreadOp *BSVM_ThOp_JMP_NE_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_Pop(ctx);
		if(dytfNEqqP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LT_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_Pop(ctx);
		if(dytfLtP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GT_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_Pop(ctx);
		if(dytfGtP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_Pop(ctx);
		if(dytfLeP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_Pop(ctx);
		if(dytfGeP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}




BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(dytfEqP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}


BSVM_ThreadOp *BSVM_ThOp_JMP_NE_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(dytfNEqP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LT_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(dytfLtP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GT_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(dytfGtP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_LE_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(dytfLeP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_JMP_GE_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	if(cur->data)
	{
		t=BSVM_LookupIdx(ctx, cur->j);
		if(dytfGeP(t, cur->t))return((BSVM_ThreadOp *)(cur->data));
		return(cur->next);
	}
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data) { return(cur->fcn(ctx, cur)); }
	return(cur->next);
}
