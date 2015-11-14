#ifdef BGBSVM_DLL

// #define BVT_FreeS32(ctx, v)		BSVM_FreeInt(ctx, v)
// #define BVT_FreeS64(ctx, v)		BSVM_FreeLong(ctx, v)
// #define BVT_FreeF32(ctx, v)		BSVM_FreeFloat(ctx, v)
// #define BVT_FreeF64(ctx, v)		BSVM_FreeDouble(ctx, v)

#define BVT_FreeS32(ctx, v)
#define BVT_FreeS64(ctx, v)
#define BVT_FreeF32(ctx, v)
#define BVT_FreeF64(ctx, v)

#define BVT_Free2S32(ctx, u, v)		\
	BVT_FreeS32(ctx, u); BVT_FreeS32(ctx, v)
#define BVT_Free2S64(ctx, u, v)		\
	BVT_FreeS64(ctx, u); BVT_FreeS64(ctx, v)
#define BVT_Free2F32(ctx, u, v)		\
	BVT_FreeF32(ctx, u); BVT_FreeF32(ctx, v)
#define BVT_Free2F64(ctx, u, v)		\
	BVT_FreeF64(ctx, u); BVT_FreeF64(ctx, v)

#define BVT_Free_s32(v)		BVT_FreeS32(ctx, v)
#define BVT_Free_s64(v)		BVT_FreeS64(ctx, v)
#define BVT_Free_f32(v)		BVT_FreeF32(ctx, v)
#define BVT_Free_f64(v)		BVT_FreeF64(ctx, v)

// #define BVT_Unwrap_s32(v)		dytfIntv(v)
// #define BVT_Unwrap_s64(v)		dytfLongv(v)
// #define BVT_Unwrap_f32(v)		dytfFloatv(v)
// #define BVT_Unwrap_f64(v)		dytfDoublev(v)

#define BVT_Unwrap_s32(v)		dytfInt32vF(v)
#define BVT_Unwrap_s64(v)		dytfLongvF(v)
#define BVT_Unwrap_f32(v)		dytfFloatvF(v)
#define BVT_Unwrap_f64(v)		dytfDoublevF(v)

#define BVT_Lookup_s32(v, i)		v=BSVM_LookupIdxS32(ctx, i)
#define BVT_Lookup_s64(v, i)		v=BSVM_LookupIdxS64(ctx, i)
#define BVT_Lookup_f32(v, i)		v=BSVM_LookupIdxF32(ctx, i)
#define BVT_Lookup_f64(v, i)		v=BSVM_LookupIdxF64(ctx, i)

#define BVT_UnwrapU_Ty(ty, u)		BVT_Unwrap_##ty(u)
#define BVT_LookupU_Ty(ty, u, i)	BVT_Lookup_##ty(u, i)

#define BVT_FreeU_Ty(ty, u)		BVT_Free_##ty(u)
#define BVT_FreeUV_Ty(ty, u, v)		\
	BVT_FreeU_Ty(ty, u); BVT_FreeU_Ty(ty, v)

#define BVT_PopU_Ty(ty, u)		\
	u=BVT_UnwrapU_Ty(ty, ctx->stack[--ctx->stackpos])
#define BVT_GetU_Ty(ty, u)		\
	u=BVT_UnwrapU_Ty(ty, ctx->stack[ctx->stackpos-1])

#define BVT_PopUV_Ty(ty, u, v)		\
	v=BVT_UnwrapU_Ty(ty, ctx->stack[--ctx->stackpos]);		\
	u=BVT_UnwrapU_Ty(ty, ctx->stack[ctx->stackpos-1])

#define BVT_PopXY_Ty(ty, u, v)		\
	v=BVT_UnwrapU_Ty(ty, ctx->stack[--ctx->stackpos]);		\
	u=BVT_UnwrapU_Ty(ty, ctx->stack[--ctx->stackpos])

#define BVT_AddUV(u, v)		u=(u)+(v)
#define BVT_SubUV(u, v)		u=(u)-(v)
#define BVT_MulUV(u, v)		u=(u)*(v)
#define BVT_DivUV(u, v)		u=(u)/(v)
#define BVT_ModUV(u, v)		u=(u)%(v)
#define BVT_AndUV(u, v)		u=(u)&(v)
#define BVT_OrUV(u, v)		u=(u)|(v)
#define BVT_XorUV(u, v)		u=(u)^(v)
#define BVT_ShlUV(u, v)		u=(u)<<(v)
#define BVT_ShrUV(u, v)		u=(u)>>(v)
#define BVT_ShrrUV(u, v)	u=((u64)(u))>>(v)

#define BVT_AddUV2(u, v)	u=(u)+(v)
#define BVT_SubUV2(u, v)	u=(u)-(v)
#define BVT_MulUV2(u, v)	u=(u)*(v)
#define BVT_DivUV2(u, v)	u=(u)/(v)
#define BVT_ModUV2(u, v)	u=(u)%(v)
#define BVT_AndUV2(u, v)	u=(u)&(v)
#define BVT_OrUV2(u, v)		u=(u)|(v)
#define BVT_XorUV2(u, v)	u=(u)^(v)

#define BVT_ShlUV2(u, v)	u=(u)<<(v)
#define BVT_ShrUV2(u, v)	u=(u)>>(v)
#define BVT_ShrrUV2(u, v)	u=((u64)(u))>>(v)


#define BVT_DefPopUV_Ty(ty, u, v)		\
	ty u, v; BVT_PopUV_Ty(ty, u, v)

#define BVT_DefPopXY_Ty(ty, u, v)		\
	ty u, v; BVT_PopXY_Ty(ty, u, v)

#define BVT_DefGet_Ty(ty, u)	ty u; BVT_GetU_Ty(ty, u)
#define BVT_DefPop_Ty(ty, u)	ty u; BVT_PopU_Ty(ty, u)

#define BVT_DefPop_Ty2(ty, u)	\
	ty u; u=(ctx->stack[--ctx->stackpos])

#define BVT_PushBool(b)		\
	ctx->stack[ctx->stackpos++]=(b)?DYTF_TRUE:DYTF_FALSE

#define BVT_PushS32(v)		\
	ctx->stack[ctx->stackpos++]=dytfInt(v)
#define BVT_PushS64(v)		\
	ctx->stack[ctx->stackpos++]=dytfLong(v)
#define BVT_PushF32(v)		\
	ctx->stack[ctx->stackpos++]=dytfFloat(v)
#define BVT_PushF64(v)		\
	ctx->stack[ctx->stackpos++]=dytfDouble(v)
#define BVT_PushDyt(v)		\
	ctx->stack[ctx->stackpos++]=v

#define BVT_PushEqUV(u, v)	BVT_PushBool((u)==(v))
#define BVT_PushNeUV(u, v)	BVT_PushBool((u)!=(v))
#define BVT_PushLtUV(u, v)	BVT_PushBool((u)<(v))
#define BVT_PushGtUV(u, v)	BVT_PushBool((u)>(v))
#define BVT_PushLeUV(u, v)	BVT_PushBool((u)<=(v))
#define BVT_PushGeUV(u, v)	BVT_PushBool((u)>=(v))

#define BVT_PushEqUC(u, v)	BVT_PushBool((u)==(v))
#define BVT_PushNeUC(u, v)	BVT_PushBool((u)!=(v))
#define BVT_PushLtUC(u, v)	BVT_PushBool((u)<(v))
#define BVT_PushGtUC(u, v)	BVT_PushBool((u)>(v))
#define BVT_PushLeUC(u, v)	BVT_PushBool((u)<=(v))
#define BVT_PushGeUC(u, v)	BVT_PushBool((u)>=(v))


#endif
