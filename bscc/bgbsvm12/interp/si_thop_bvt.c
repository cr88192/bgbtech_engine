#include <bgbsvm.h>

// s32 BSVM_BVT_GetInt(dytf a)
// {
// }

int BSVM_ThOp_DecodePushConstant(BSVM_SVMState *ctx,
	BSVM_ThreadOp *thop, int val)
{
	if(ctx->pfhint&BSVM_PF_HINT_XI)
		{ thop->i=val; thop->fcn=BSVM_ThOp_PUSH_XI_C; return(1); }
	if(ctx->pfhint&BSVM_PF_HINT_XL)
		{ thop->i=val; thop->fcn=BSVM_ThOp_PUSH_XL_C; return(1); }
	if(ctx->pfhint&BSVM_PF_HINT_XF)
		{ thop->i=val; thop->fcn=BSVM_ThOp_PUSH_XF_C; return(1); }
	if(ctx->pfhint&BSVM_PF_HINT_XD)
		{ thop->i=val; thop->fcn=BSVM_ThOp_PUSH_XD_C; return(1); }

	return(0);
}

BSVM_ThreadOp *BSVM_ThOp_PUSH_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushS32(cur->i); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushS64(cur->i); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_XF_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushF32(cur->i); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_XD_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushF64(cur->i); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_PUSH_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushS32(dytfIntvF(cur->t)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushS64(dytfLongvF(cur->t)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushF32(dytfFloatvF(cur->t)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_PUSH_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_PushF64(dytfDoublevF(cur->t)); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_Push(ctx, t);
//	BVT_PushS32(dytfIntv(t));
//	BVT_PushS32(BSVM_BVT_GetInt(t));
	return(cur->next); 
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_Push(ctx, t);
//	BVT_PushS64(dytfLongv(t));
//	BVT_PushS64(BSVM_BVT_GetLong(t));
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_Push(ctx, t);
//	BVT_PushF32(dytfFloatv(t));
//	BVT_PushF32(*(f32 *)t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	t=BSVM_LookupIdx(ctx, cur->i);
	BSVM_Push(ctx, t);
//	BVT_PushF64(dytfDoublev(t));
//	BVT_PushF64(*(f64 *)t);
	return(cur->next);
}

#ifdef BSVM_VARSTACK
BSVM_ThreadOp *BSVM_ThOp_LLOAD_XI_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	dytf t;
//	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
//	ctx->stack[ctx->stackpos++]=t;

	ctx->stack[ctx->stackpos++]=
		ctx->levstack[ctx->lestackpos-(cur->i+1)];

//	BSVM_Push(ctx, t);
//	BVT_PushS32(BSVM_BVT_GetInt(t));
	return(cur->next); 
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XL_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	dytf t;
//	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
//	ctx->stack[ctx->stackpos++]=t;

	ctx->stack[ctx->stackpos++]=
		ctx->levstack[ctx->lestackpos-(cur->i+1)];

//	BSVM_Push(ctx, t);
//	BVT_PushS64(BSVM_BVT_GetLong(t));
	return(cur->next); 
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XF_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	dytf t;
//	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
//	ctx->stack[ctx->stackpos++]=t;

	ctx->stack[ctx->stackpos++]=
		ctx->levstack[ctx->lestackpos-(cur->i+1)];

//	BSVM_Push(ctx, t);
//	BVT_PushF32(*(f32 *)t);
	return(cur->next); 
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XD_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
//	dytf t;
//	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
//	ctx->stack[ctx->stackpos++]=t;

	ctx->stack[ctx->stackpos++]=
		ctx->levstack[ctx->lestackpos-(cur->i+1)];

//	BSVM_Push(ctx, t);
//	BVT_PushF64(*(f64 *)t);
	return(cur->next); 
}
#endif

BSVM_ThreadOp *BSVM_ThOp_LSTORE_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	BVT_DefPop_Ty(s32, u);
	BSVM_AssignIdx(ctx, cur->i, dytfInt(u));
//	t=BSVM_LookupIdx(ctx, cur->i);
//	if(t)	{ BSVM_BVT_GetInt(t)=*u; BVT_FreeS32(ctx, u); }
//	else	{ BSVM_AssignIdx(ctx, cur->i, (dyt)u); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	BVT_DefPop_Ty(s64, u);
	BSVM_AssignIdx(ctx, cur->i, dytfLong(u));
//	t=BSVM_LookupIdx(ctx, cur->i);
//	if(t)	{ BSVM_BVT_GetLong(t)=*u; BVT_FreeS64(ctx, u); }
//	else	{ BSVM_AssignIdx(ctx, cur->i, (dyt)u); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	BVT_DefPop_Ty(f32, u);
	BSVM_AssignIdx(ctx, cur->i, dytfFloat(u));
//	t=BSVM_LookupIdx(ctx, cur->i);
//	if(t)	{ *(f32 *)t=*u; BVT_FreeF32(ctx, u); }
//	else	{ BSVM_AssignIdx(ctx, cur->i, (dyt)u); }
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	BVT_DefPop_Ty(f64, u);
	BSVM_AssignIdx(ctx, cur->i, dytfDouble(u));
//	t=BSVM_LookupIdx(ctx, cur->i);
//	if(t)	{ *(f64 *)t=*u; BVT_FreeF64(ctx, u); }
//	else	{ BSVM_AssignIdx(ctx, cur->i, (dyt)u); }
	return(cur->next);
}

#if 1
BSVM_ThreadOp *BSVM_ThOp_LINC_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	s32 v;
	v=BSVM_LookupIdxS32(ctx, cur->i);
	BSVM_AssignIdxS32(ctx, cur->i, v+1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LINC_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	s64 v;
	v=BSVM_LookupIdxS64(ctx, cur->i);
	BSVM_AssignIdxS64(ctx, cur->i, v+1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LINC_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	f32 v;
	v=BSVM_LookupIdxF32(ctx, cur->i);
	BSVM_AssignIdxF32(ctx, cur->i, v+1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LINC_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	f64 v;
	v=BSVM_LookupIdxF64(ctx, cur->i);
	BSVM_AssignIdxF64(ctx, cur->i, v+1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LDEC_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	s32 v;
	v=BSVM_LookupIdxS32(ctx, cur->i);
	BSVM_AssignIdxS32(ctx, cur->i, v-1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LDEC_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	s64 v;
	v=BSVM_LookupIdxS64(ctx, cur->i);
	BSVM_AssignIdxS64(ctx, cur->i, v-1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LDEC_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	f32 v;
	v=BSVM_LookupIdxF32(ctx, cur->i);
	BSVM_AssignIdxF32(ctx, cur->i, v-1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LDEC_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	f64 v;
	v=BSVM_LookupIdxF64(ctx, cur->i);
	BSVM_AssignIdxF64(ctx, cur->i, v-1);
	return(cur->next);
}

#endif

#if 1
BSVM_ThreadOp *BSVM_ThOp_LINC_XI_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	s32 v;

	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
	t=dytfInt32F(dytfInt32vF(t)+1);
	ctx->levstack[ctx->lestackpos-(cur->i+1)]=t;

//	v=BSVM_LookupIdxS32(ctx, cur->i);
//	BSVM_AssignIdxS32(ctx, cur->i, v+1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LINC_XL_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	s64 v;

	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
	t=dytfLong(dytfLongvF(t)+1);
	ctx->levstack[ctx->lestackpos-(cur->i+1)]=t;

//	v=BSVM_LookupIdxS64(ctx, cur->i);
//	BSVM_AssignIdxS64(ctx, cur->i, v+1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LDEC_XI_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	s32 v;

	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
	t=dytfInt32F(dytfInt32vF(t)-1);
	ctx->levstack[ctx->lestackpos-(cur->i+1)]=t;

//	v=BSVM_LookupIdxS32(ctx, cur->i);
//	BSVM_AssignIdxS32(ctx, cur->i, v-1);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LDEC_XL_VS(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t;
	s64 v;

	t=ctx->levstack[ctx->lestackpos-(cur->i+1)];
	t=dytfLong(dytfLongvF(t)-1);
	ctx->levstack[ctx->lestackpos-(cur->i+1)]=t;

//	v=BSVM_LookupIdxS64(ctx, cur->i);
//	BSVM_AssignIdxS64(ctx, cur->i, v-1);
	return(cur->next);
}
#endif

BSVM_ThreadOp *BSVM_ThOp_DUP_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	s32 v;
	v=BSVM_PopS32(ctx);
	BSVM_Push2S32(ctx, v, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DUP_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	s64 v;
	v=BSVM_PopS64(ctx);
	BSVM_Push2S64(ctx, v, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DUP_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	f32 v;
	v=BSVM_PopF32(ctx);
	BSVM_Push2F32(ctx, v, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DUP_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	f64 v;
	v=BSVM_PopF64(ctx);
	BSVM_Push2F64(ctx, v, v);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ADD_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_AddUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_ADD_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_AddUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_ADD_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_AddUV(u, v); BVT_FreeF32(ctx, v);
	BVT_PushF32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_ADD_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_AddUV(u, v); BVT_FreeF64(ctx, v);
	BVT_PushF64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SUB_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_SubUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SUB_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_SubUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SUB_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_SubUV(u, v); BVT_FreeF32(ctx, v);
	BVT_PushF32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SUB_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_SubUV(u, v); BVT_FreeF64(ctx, v);
	BVT_PushF64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MUL_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_MulUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MUL_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_MulUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MUL_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_MulUV(u, v); BVT_FreeF32(ctx, v);
	BVT_PushF32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MUL_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_MulUV(u, v); BVT_FreeF64(ctx, v);
	BVT_PushF64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_DIV_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_DivUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_DIV_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_DivUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_DIV_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_DivUV(u, v); BVT_FreeF32(ctx, v);
	BVT_PushF32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_DIV_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_DivUV(u, v); BVT_FreeF64(ctx, v);
	BVT_PushF64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MOD_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_ModUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MOD_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_ModUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_AND_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_AndUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_AND_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_AndUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_OR_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_OrUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_OR_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_OrUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_XOR_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_XorUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_XOR_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_XorUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_SHL_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_ShlUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHL_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_ShlUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHR_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_ShrUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHR_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_ShrUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHRR_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_ShrrUV(u, v); BVT_FreeS32(ctx, v);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHRR_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_ShrrUV(u, v); BVT_FreeS64(ctx, v);
	BVT_PushS64(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_ADD_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_AddUV2(u, cur->i);
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_ADD_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_AddUV2(u, cur->i);
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SUB_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_SubUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SUB_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_SubUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MUL_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_MulUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MUL_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_MulUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_DIV_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_DivUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_DIV_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_DivUV2(u, cur->i);
	BVT_PushS64(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_MOD_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_ModUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_MOD_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_ModUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_AND_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_AndUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_AND_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_AndUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_OR_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_OrUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_OR_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_OrUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_XOR_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_XorUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_XOR_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_XorUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_SHL_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_ShlUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHL_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_ShlUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHR_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_ShrUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHR_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_ShrUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHRR_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); BVT_ShrrUV2(u, cur->i); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_SHRR_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); BVT_ShrrUV2(u, cur->i); 
	BVT_PushS64(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_NEG_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); u=-(u); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_NEG_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); u=-(u); 
	BVT_PushS64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_NOT_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); u=~(u); 
	BVT_PushS32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_NOT_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s64, u); u=~(u); 
	BVT_PushS64(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_NEG_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(f32, u); u=-(u); 
	BVT_PushF32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_NEG_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(f64, u); u=-(u); 
	BVT_PushF64(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_NOT_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(f32, u); u=~(s64)(u); 
	BVT_PushF32(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_NOT_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(f64, u); u=~(s64)(u); 
	BVT_PushF64(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_LNOT_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefPop_Ty(s32, u); u=!(u); 
	BVT_PushS32(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CMP_EQ_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_PushEqUV(u, v);
	BVT_Free2S32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_EQ_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_PushEqUV(u, v);
	BVT_Free2S64(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_EQ_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_PushEqUV(u, v);
	BVT_Free2F32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_EQ_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_PushEqUV(u, v);
	BVT_Free2F64(ctx, u, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CMP_NE_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_PushNeUV(u, v);
	BVT_Free2S32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_NE_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_PushNeUV(u, v);
	BVT_Free2S64(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_NE_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_PushNeUV(u, v);
	BVT_Free2F32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_NE_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_PushNeUV(u, v);
	BVT_Free2F64(ctx, u, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CMP_LT_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_PushLtUV(u, v);
	BVT_Free2S32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LT_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_PushLtUV(u, v);
	BVT_Free2S64(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LT_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_PushLtUV(u, v);
	BVT_Free2F32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LT_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_PushLtUV(u, v);
	BVT_Free2F64(ctx, u, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CMP_GT_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_PushGtUV(u, v);
	BVT_Free2S32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GT_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_PushGtUV(u, v);
	BVT_Free2S64(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GT_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_PushGtUV(u, v);
	BVT_Free2F32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GT_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_PushGtUV(u, v);
	BVT_Free2F64(ctx, u, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CMP_LE_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_PushLeUV(u, v);
	BVT_Free2S32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LE_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_PushLeUV(u, v);
	BVT_Free2S64(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LE_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_PushLeUV(u, v);
	BVT_Free2F32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LE_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_PushLeUV(u, v);
	BVT_Free2F64(ctx, u, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CMP_GE_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s32, u, v); BVT_PushGeUV(u, v);
	BVT_Free2S32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GE_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(s64, u, v); BVT_PushGeUV(u, v);
	BVT_Free2S64(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GE_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f32, u, v); BVT_PushGeUV(u, v);
	BVT_Free2F32(ctx, u, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GE_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopXY_Ty(f64, u, v); BVT_PushGeUV(u, v);
	BVT_Free2F64(ctx, u, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CMP_EQ_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushEqUC(u, cur->i);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_EQ_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushEqUC(u, cur->i);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_NE_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushNeUC(u, cur->i);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_NE_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushNeUC(u, cur->i);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LT_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushLtUC(u, cur->i);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LT_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushLtUC(u, cur->i);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GT_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushGtUC(u, cur->i);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GT_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushGtUC(u, cur->i);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LE_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushLeUC(u, cur->i);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_LE_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushLeUC(u, cur->i);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GE_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushGeUC(u, cur->i);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CMP_GE_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushGeUC(u, cur->i);
	BVT_FreeS64(ctx, u); return(cur->next); }


BSVM_ThreadOp *BSVM_ThOp_CONV_FN2XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty2(dytf, u); BVT_PushS32(dytfIntv(u)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_FN2XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty2(dytf, u); BVT_PushS64(dytfLongv(u)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_FL2XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty2(dytf, u); BVT_PushF32(dytfFloatv(u)); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_FL2XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty2(dytf, u); BVT_PushF64(dytfDoublev(u)); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CONV_XI2XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushS64(u);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XI2XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushF32(u);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XI2XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushF64(u);
	BVT_FreeS32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XI2FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s32, u); BVT_PushDyt(dytfInt(u));
	BVT_FreeS32(ctx, u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CONV_XL2XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushS32(u);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XL2XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushF32(u);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XL2XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushF64(u);
	BVT_FreeS64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XL2FN(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(s64, u); BVT_PushDyt(dytfLong(u));
	BVT_FreeS64(ctx, u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CONV_XF2XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f32, u); BVT_PushS32(u);
	BVT_FreeF32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XF2XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f32, u); BVT_PushS64(u);
	BVT_FreeF32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XF2XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f32, u); BVT_PushF64(u);
	BVT_FreeF32(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XF2FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f32, u); BVT_PushDyt(dytfFloat(u));
	BVT_FreeF32(ctx, u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_CONV_XD2XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f64, u); BVT_PushS32(u);
	BVT_FreeF64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XD2XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f64, u); BVT_PushS64(u);
	BVT_FreeF64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XD2XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f64, u); BVT_PushF32(u);
	BVT_FreeF64(ctx, u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_CONV_XD2FL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPop_Ty(f64, u); BVT_PushDyt(dytfDouble(u));
	BVT_FreeF64(ctx, u); return(cur->next); }
	
	
#define BVT_JMPCC_BODY(ty, cc)	\
	ty u, v;	\
	if(cur->data)	\
	{	\
		BVT_PopXY_Ty(ty, u, v);	\
		if((u)cc(v))	\
		{ \
			BVT_FreeUV_Ty(ty, u, v);	\
			return((BSVM_ThreadOp *)(cur->data));	\
		} \
		BVT_FreeUV_Ty(ty, u, v);	\
		return(cur->next);	\
	}	\
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);	\
	if(cur->data) { return(cur->fcn(ctx, cur)); }	\
	else { *(int *)-1=-1; } \
	return(cur->next)

BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s32, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s32, !=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s64, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s64, !=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f32, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f32, !=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f64, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f64, !=); }

BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s32, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s32, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s32, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XI(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s32, >=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s64, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s64, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s64, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XL(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(s64, >=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f32, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f32, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f32, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XF(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f32, >=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f64, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f64, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f64, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XD(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY(f64, >=); }


#define BVT_JMPCC_BODY2(ty, cc)	\
	ty u, v;	\
	if(cur->data)	\
	{	\
		BVT_PopU_Ty(ty, u);	\
		if((u)cc(cur->j))	\
		{	\
			BVT_FreeU_Ty(ty, u);	\
			return((BSVM_ThreadOp *)(cur->data));	\
		}	\
		BVT_FreeU_Ty(ty, u);	\
		return(cur->next);	\
	}	\
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);	\
	if(cur->data) { return(cur->fcn(ctx, cur)); }	\
	else { *(int *)-1=-1; } \
	return(cur->next)

BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s32, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s64, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s32, !=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s64, !=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s32, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s64, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s32, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s64, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s32, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s64, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XI_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s32, >=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XL_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY2(s64, >=); }


#define BVT_JMPCC_BODY3(ty, cc)	\
	ty u, v;	\
	if(cur->data)	\
	{	\
		BVT_LookupU_Ty(ty, u, cur->j); \
		if((u)cc(cur->k))	\
		{	\
			return((BSVM_ThreadOp *)(cur->data));	\
		}	\
		return(cur->next);	\
	}	\
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);	\
	if(cur->data) { return(cur->fcn(ctx, cur)); }	\
	else { *(int *)-1=-1; } \
	return(cur->next)

BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XI_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s32, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_EQ_XL_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s64, ==); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XI_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s32, !=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_NE_XL_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s64, !=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XI_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s32, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LT_XL_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s64, <); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XI_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s32, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GT_XL_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s64, >); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XI_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s32, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_LE_XL_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s64, <=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XI_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s32, >=); }
BSVM_ThreadOp *BSVM_ThOp_JMP_GE_XL_LC(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_JMPCC_BODY3(s64, >=); }
