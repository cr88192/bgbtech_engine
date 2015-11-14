#include <bgbsvm.h>

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUBI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS32(dycGetArrayub(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSBI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS32(dycGetArraysb(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUSI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS32(dycGetArrayus(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSSI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS32(dycGetArrayss(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUDI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS32(dycGetArrayi(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSDI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS32(dycGetArrayi(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUDL_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS64((u32)dycGetArrayi(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSDL_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS64(dycGetArrayi(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XLL_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushS64(dycGetArrayl(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XFF_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushF32(dycGetArrayf(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XDD_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; BVT_DefPop_Ty(s32, u); t=BSVM_Pop(ctx);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, u))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	BVT_PushF64(dycGetArrayd(arr, u));
	BVT_FreeS32(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XI(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v; u=BSVM_Pop(ctx); t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, u); BVT_PushS32(dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XL(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v; u=BSVM_Pop(ctx); t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, u); BVT_PushS64(dytfIntv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XF(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v; u=BSVM_Pop(ctx); t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, u); BVT_PushF32(dytfRealv(v));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XD(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v; u=BSVM_Pop(ctx); t=BSVM_Pop(ctx);
	v=BSVM_LoadIndex(ctx, t, u); BVT_PushF64(dytfRealv(v));
	return(cur->next);
}

void BSVM_ThOp_DecodeLoadIndex(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop)
{
	if(ctx->pfhint&BSVM_PF_ARR_IX)
	{
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUBI)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XUBI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSBI)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSBI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUSI)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XUSI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSSI)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSSI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDI)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XUDI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDI)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSDI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDL)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XUDL_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDL)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSDL_IX; return; }

		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XII)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSDI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XLL)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XLL_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XFF)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XFF_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XDD)
			{ thop->fcn=BSVM_ThOp_LOADINDEX_XDD_IX; return; }
	}else
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_ARR_XUBI: case BSVM_PF_ARR_XSBI:
		case BSVM_PF_ARR_XUSI: case BSVM_PF_ARR_XSSI:
		case BSVM_PF_ARR_XUDI: case BSVM_PF_ARR_XSDI:
		case BSVM_PF_ARR_XII: case BSVM_PF_HINT_XI:
			thop->fcn=BSVM_ThOp_LOADINDEX_XI;
			break;
		case BSVM_PF_ARR_XUDL: case BSVM_PF_ARR_XSDL:
		case BSVM_PF_ARR_XLL: case BSVM_PF_HINT_XL:
			thop->fcn=BSVM_ThOp_LOADINDEX_XL;
			break;
		case BSVM_PF_ARR_XFF: case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_LOADINDEX_XF;
			break;
		case BSVM_PF_ARR_XDD: case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_LOADINDEX_XD;
			break;
		default:
			thop->fcn=BSVM_ThOp_LOADINDEX;
			return;
		}
		return;
	}

	thop->fcn=BSVM_ThOp_LOADINDEX;
	return;
}


BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XUBI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; s32 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(s32, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArrayub(arr, pi, u);
	BVT_FreeS32(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XSBI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; s32 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(s32, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArraysb(arr, pi, u);
	BVT_FreeS32(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XUSI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; s32 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(s32, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArrayus(arr, pi, u);
	BVT_FreeS32(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XSSI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; s32 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(s32, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArrayss(arr, pi, u);
	BVT_FreeS32(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XSDI_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; s32 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(s32, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArrayi(arr, pi, u);
	BVT_FreeS32(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XLL_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; s64 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(s64, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArrayl(arr, pi, u);
	BVT_FreeS64(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XFF_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; f32 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(f32, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArrayf(arr, pi, u);
	BVT_FreeF32(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_STOREINDEX_XDD_IX(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dycArray arr;
	dytf t; f64 u; s32 pi;
	BVT_PopU_Ty(s32, pi); t=BSVM_Pop(ctx); BVT_PopU_Ty(f64, u);
	arr=(dycArray)dytfPtrvF(t);
	if(!dycArrayIdxCheck(arr, pi))
		{ ctx->rs=BSVM_RETS_BOUNDSCHK; return(cur->next); }
	dycSetArrayd(arr, pi, u);
	BVT_FreeF64(ctx, u); BVT_FreeS32(ctx, pi);
	return(cur->next);
}

void BSVM_ThOp_DecodeStoreIndex(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop)
{
	if(ctx->pfhint&BSVM_PF_ARR_IX)
	{
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUBI)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XUBI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSBI)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XSBI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUSI)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XUSI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSSI)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XSSI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDI)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XSDI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDI)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XSDI_IX; return; }

		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XII)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XSDI_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XLL)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XLL_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XFF)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XFF_IX; return; }
		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XDD)
			{ thop->fcn=BSVM_ThOp_STOREINDEX_XDD_IX; return; }
	}

	thop->fcn=BSVM_ThOp_STOREINDEX;
	return;
}

void BSVM_ThOp_DecodeDefer(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop)
{
	thop->fcn=BSVM_ThOp_DEFER;
}

void BSVM_ThOp_DecodeSetDefer(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop)
{
	thop->fcn=BSVM_ThOp_SETDEFER;
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUBI_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS32(dycGetArrayub((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSBI_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS32(dycGetArraysb((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUSI_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS32(dycGetArrayus((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSSI_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS32(dycGetArrayss((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUDI_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS32(dycGetArrayi((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSDI_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS32(dycGetArrayi((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XUDL_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS64((u32)dycGetArrayi((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XSDL_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS64(dycGetArrayi((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XLL_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushS64(dycGetArrayl((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XFF_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushF32(dycGetArrayf((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LOADINDEX_XDD_C(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t; t=BSVM_Pop(ctx);
	BVT_PushF64(dycGetArrayd((dycArray)dytfPtrvF(t), cur->i));
	return(cur->next);
}

void BSVM_ThOp_DecodeLoadIndexI(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, int idx)
{
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUBI)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XUBI_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSBI)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XSBI_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUSI)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XUSI_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSSI)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XSSI_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDI)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XUDI_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDI)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XSDI_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDL)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XUDL_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDL)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XSDL_C; return; }

	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XII)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XSDI_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XLL)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XLL_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XFF)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XFF_C; return; }
	if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XDD)
		{ thop->i=idx; thop->fcn=BSVM_ThOp_LOADINDEX_XDD_C; return; }

	thop->t=dytfInt(idx);
	thop->fcn=BSVM_ThOp_LOADINDEX_I;
}

void BSVM_ThOp_DecodeStoreIndexI(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, int idx)
{
	thop->t=dytfInt(idx);
	thop->fcn=BSVM_ThOp_STOREINDEX_I;
}
