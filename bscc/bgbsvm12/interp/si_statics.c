#include <bgbsvm.h>

BSVM_ThreadCallInfo *bsvm_statics_callinfo=NULL;

void BSVM_Statics_CallObj_Default(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj)
{
	BSVM_CallObj(ctx, obj, cinf->hdl);
}

void BSVM_Statics_CallObj_Dynamic(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj)
{
	dytf t, u;

	u=BSVM_Lookup(ctx, cinf->key);
	if(dytfNullP(u) || dytfUndefinedP(u))
	{
		dyPrintf("BSVM_LookupCallObj: Fail Lookup %s\n",
			dytfSymbolv(cinf->key));
		
		t=BSVM_PopList(ctx);
		BSVM_Push(ctx, DYTF_UNDEFINED);
		return;
	}

//	ctx->thop=cur->next;
//	cinf=BSVM_Thread_CreateCallInfo(ctx, thop, u);
//	cinf->CallObj(ctx, cinf, ctx->self);
	BSVM_CallObj(ctx, ctx->self, u);
}

void BSVM_Statics_CallObj_SvmBlock(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj)
{
	BSVM_CallObj_SvmBlock(ctx, obj, cinf->hdl);
}

void BSVM_Statics_CallObj_SvmLambda(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj)
{
	BSVM_CallObj_SvmLambda(ctx, obj, cinf->hdl);
}

void BSVM_Statics_CallObj_RawFunc(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj)
{
	BGBGC_RawFunc *rinf;
	dytf *args;
	void *pv;
	dyt p;
	dytf t;
	int nargs;

	args=BSVM_PopMarkArgs(ctx, &nargs);

	rinf=(BGBGC_RawFunc *)dytfPtrvF(cinf->hdl);
	pv=dyllApplyRawFuncB(rinf, dytfUnwrap(obj), args, nargs);
	if(!pv || (pv==UNDEFINED))
	{
		BSVM_Push(ctx, DYTF_UNDEFINED);
		return;
	}
//	p=dyllExtractVariantBufferSig(rinf->esig, pv);
//	t=dytfWrap(p);
	t=dyllExtractVariantBufferSigB(rinf->esig, pv);

	BSVM_Push(ctx, t);

//	BSVM_CallObj_SvmBlock(ctx, obj, cinf->hdl);
}


void BSVM_Statics_TailCallObj_Default(BSVM_SVMState *ctx,
	BSVM_ThreadCallInfo *cinf, dytf obj)
{
	BSVM_TailCallObj(ctx, obj, cinf->hdl);
}


BSVM_ThreadCallInfo *BSVM_Thread_CreateCallInfo(BSVM_SVMState *ctx,
	BSVM_ThreadOp *thop, dytf mth, dytf key, dytf vinf)
{
	BSVM_ThreadCallInfo *cinf;
	char *ty, *sig;

	cinf=gctalloc("bsvm_threadcallinfo_t", sizeof(BSVM_ThreadCallInfo));
	cinf->thop=thop;
	cinf->hdl=mth;
	cinf->key=key;

	cinf->CallObj=BSVM_Statics_CallObj_Default;
	cinf->TailCallObj=BSVM_Statics_TailCallObj_Default;
	thop->data=cinf;

	sig=BSVM_GetInfoSig(ctx, vinf);

	if(sig && (*sig!='('))
//	if(1)
	{
		cinf->CallObj=BSVM_Statics_CallObj_Dynamic;
	}else
	{
		ty=dytfgettype(mth);

		if(!strcmp(ty, "svm_blk_t"))
		{
			cinf->CallObj=BSVM_Statics_CallObj_SvmBlock;
//			BSVM_CallObj_SvmBlock(ctx, obj, f);
//			return;
		}

		if(!strcmp(ty, "svm_lambda_t"))
		{
			cinf->CallObj=BSVM_Statics_CallObj_SvmLambda;
//			cinf->CallObj=BSVM_Statics_CallObj_Dynamic;
		}

		if(!strcmp(ty, "_raw_func_t"))
		{
			cinf->CallObj=BSVM_Statics_CallObj_RawFunc;
		}
	}

	cinf->next=bsvm_statics_callinfo;
	bsvm_statics_callinfo=cinf;
	return(cinf);
}

void BSVM_Thread_LookupCallObj(BSVM_SVMState *ctx,
	BSVM_ThreadOp *thop, dytf key)
{
	BSVM_ThreadCallInfo *cinf;
	dytf t, u, vinf;

	if(thop->data)
	{
		cinf=(BSVM_ThreadCallInfo *)thop->data;
		cinf->CallObj(ctx, cinf, ctx->self);
//		BSVM_CallObj(ctx, ctx->self, cinf->hdl);
		return;
	}

	if(ctx->this_obj && ctx->this_inf)
	{
		t=dytfWrap(dycLookupMethodSig(ctx->this_obj, dytfSymbolvF(key), "?"));
		if(!dytfNullP(t) && !dytfUndefinedP(t))
		{
			cinf=BSVM_Thread_CreateCallInfo(ctx, thop, t, key, DYTF_NULL);
			cinf->CallObj(ctx, cinf, ctx->self);
//			BSVM_CallObj(ctx, ctx->self, t);
			return;
		}
	}

//	u=BSVM_Lookup(ctx, key);
	u=BSVM_LookupGetInfo(ctx, key, &vinf);
	if(dytfNullP(u) || dytfUndefinedP(u))
	{
		dyPrintf("BSVM_LookupCallObj: Fail Lookup %s\n",
			dytfSymbolv(key));
		
//		t=BSVM_PopList(ctx);
		BSVM_PopMark(ctx);
		BSVM_Push(ctx, DYTF_UNDEFINED);
		return;
	}

//	ctx->thop=cur->next;
	cinf=BSVM_Thread_CreateCallInfo(ctx, thop, u, key, vinf);
	cinf->CallObj(ctx, cinf, ctx->self);
//	BSVM_CallObj(ctx, ctx->self, u);
//	return(ctx->thop);
}
