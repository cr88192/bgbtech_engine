#include <bgbsvm.h>

void BSVM_ThOp_DecodeUnaryOpS(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, dytf name)
{
	thop->t=name;
	thop->fcn=BSVM_ThOp_UNARYOP_S;
}

void BSVM_ThOp_DecodeBinaryOpS(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, dytf name)
{
	thop->t=name;
	thop->fcn=BSVM_ThOp_BINARYOP_S;
}


BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SQR(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfSqr(u); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SQRT(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfSqrt(u); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SSQR(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfSSqr(u); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SSQRT(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfSSqrt(u); BSVM_Push(ctx, u);
	return(cur->next);
}

static double ssqr(double a)
	{ return((a>=0)?(a*a):(-(a*a))); }
static double ssqrt(double a)
	{ return((a>=0)?sqrt(a):(-sqrt(-a))); }
static double spow(double a, double b)
	{ return((a>=0)?pow(a, b):(-pow(-a, b))); }


BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SIN(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfWrap(dySin(dytfUnwrap(u))); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_COS(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfWrap(dyCos(dytfUnwrap(u))); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_TAN(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfWrap(dyTan(dytfUnwrap(u))); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ASIN(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfWrap(dyASin(dytfUnwrap(u))); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ACOS(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfWrap(dyACos(dytfUnwrap(u))); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ATAN(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf u; u=BSVM_Pop(ctx);
	u=dytfWrap(dyATan(dytfUnwrap(u))); BSVM_Push(ctx, u);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SQR_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=(u)*(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SQR_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=(u)*(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SQRT_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=sqrt(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SQRT_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=sqrt(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SSQR_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=ssqr(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SSQR_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=ssqr(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SSQRT_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=ssqrt(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SSQRT_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=ssqrt(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SIN_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=sin(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_SIN_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=sin(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_COS_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=cos(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_COS_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=cos(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_TAN_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=tan(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_TAN_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=tan(u); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ASIN_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=asin(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ASIN_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=asin(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ACOS_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=acos(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ACOS_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=acos(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ATAN_XF(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f32, u); u=atan(u); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_UNARYINT_ATAN_XD(
		BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
	{ BVT_DefGet_Ty(f64, u); u=atan(u); return(cur->next); }

void BSVM_ThOp_DecodeUnaryIntrinS(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, dytf name)
{
	char *s;

	s=dytfSymbolv(name);

	if(!strcmp(s, "sqr"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_UNARYINT_SQR_XF; break;
		case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_UNARYINT_SQR_XD; break;
		default:
			thop->fcn=BSVM_ThOp_UNARYINT_SQR; break;			
		}
		return;
	}

	if(!strcmp(s, "sqrt"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_UNARYINT_SQRT_XF; break;
		case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_UNARYINT_SQRT_XD; break;
		default:
			thop->fcn=BSVM_ThOp_UNARYINT_SQRT; break;			
		}
		return;
	}

	if(!strcmp(s, "ssqr"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_UNARYINT_SSQR_XF; break;
		case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_UNARYINT_SSQR_XD; break;
		default:
			thop->fcn=BSVM_ThOp_UNARYINT_SSQR; break;			
		}
		return;
	}

	if(!strcmp(s, "ssqrt"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_UNARYINT_SSQRT_XF; break;
		case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_UNARYINT_SSQRT_XD; break;
		default:
			thop->fcn=BSVM_ThOp_UNARYINT_SSQRT; break;			
		}
		return;
	}

	if(!strcmp(s, "sin"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_SIN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_SIN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_SIN; break;			
		}
		return;
	}
	if(!strcmp(s, "cos"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_COS_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_COS_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_COS; break;			
		}
		return;
	}
	if(!strcmp(s, "tan"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_TAN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_TAN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_TAN; break;			
		}
		return;
	}

	if(!strcmp(s, "asin"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_ASIN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_ASIN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_ASIN; break;			
		}
		return;
	}
	if(!strcmp(s, "acos"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_ACOS_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_ACOS_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_ACOS; break;			
		}
		return;
	}
	if(!strcmp(s, "atan"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_ATAN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_ATAN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_ATAN; break;			
		}
		return;
	}

	thop->t=name;
	thop->fcn=BSVM_ThOp_UNARYOP_S;
}

void BSVM_ThOp_DecodeUnaryOpI(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, int op)
{
	if(op==BSVM_OPR_SQR)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_UNARYINT_SQR_XF; break;
		case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_UNARYINT_SQR_XD; break;
		default:
			thop->fcn=BSVM_ThOp_UNARYINT_SQR; break;			
		}
		return;
	}

	if(op==BSVM_OPR_SQRT)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_UNARYINT_SQRT_XF; break;
		case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_UNARYINT_SQRT_XD; break;
		default:
			thop->fcn=BSVM_ThOp_UNARYINT_SQRT; break;			
		}
		return;
	}

	if(op==BSVM_OPR_SIN)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_SIN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_SIN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_SIN; break;			
		}
		return;
	}
	if(op==BSVM_OPR_COS)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_COS_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_COS_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_COS; break;			
		}
		return;
	}
	if(op==BSVM_OPR_TAN)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_TAN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_TAN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_TAN; break;			
		}
		return;
	}

	if(op==BSVM_OPR_ASIN)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_ASIN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_ASIN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_ASIN; break;			
		}
		return;
	}
	if(op==BSVM_OPR_ACOS)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_ACOS_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_ACOS_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_ACOS; break;			
		}
		return;
	}
	if(op==BSVM_OPR_ATAN)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_UNARYINT_ATAN_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_UNARYINT_ATAN_XD; break;
		default: thop->fcn=BSVM_ThOp_UNARYINT_ATAN; break;			
		}
		return;
	}

	thop->i=op;
	thop->fcn=BSVM_ThOp_UNARYOP;
}

BSVM_ThreadOp *BSVM_ThOp_BINARYINT_ATAN2(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	t=dytfWrap(dyATan2(dytfUnwrap(u), dytfUnwrap(v)));
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BINARYINT_POW(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	t=dytfWrap(dyexp(dytfUnwrap(u), dytfUnwrap(v)));
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BINARYINT_SPOW(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	dytf t, u, v;
	v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
	t=dytfWrap(dySexp(dytfUnwrap(u), dytfUnwrap(v)));
	BSVM_Push(ctx, t);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_BINARYINT_ATAN2_XF(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopUV_Ty(f32, u, v); u=atan2(u, v);
	BVT_FreeF32(ctx, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_BINARYINT_ATAN2_XD(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopUV_Ty(f64, u, v); u=atan2(u, v);
	BVT_FreeF64(ctx, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_BINARYINT_POW_XF(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopUV_Ty(f32, u, v); u=pow(u, v);
	BVT_FreeF32(ctx, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_BINARYINT_POW_XD(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopUV_Ty(f64, u, v); u=pow(u, v);
	BVT_FreeF64(ctx, v); return(cur->next); }

BSVM_ThreadOp *BSVM_ThOp_BINARYINT_SPOW_XF(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopUV_Ty(f32, u, v); u=spow(u, v);
	BVT_FreeF32(ctx, v); return(cur->next); }
BSVM_ThreadOp *BSVM_ThOp_BINARYINT_SPOW_XD(
	BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{	BVT_DefPopUV_Ty(f64, u, v); u=spow(u, v);
	BVT_FreeF64(ctx, v); return(cur->next); }

void BSVM_ThOp_DecodeBinaryIntrinS(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, dytf name)
{
	char *s;

	s=dytfSymbolv(name);

	if(!strcmp(s, "atan2"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_BINARYINT_ATAN2_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_BINARYINT_ATAN2_XD; break;
		default: thop->fcn=BSVM_ThOp_BINARYINT_ATAN2; break;			
		}
		return;
	}

	if(!strcmp(s, "pow"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_BINARYINT_POW_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_BINARYINT_POW_XD; break;
		default: thop->fcn=BSVM_ThOp_BINARYINT_POW; break;			
		}
		return;
	}

	if(!strcmp(s, "spow"))
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_BINARYINT_SPOW_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_BINARYINT_SPOW_XD; break;
		default: thop->fcn=BSVM_ThOp_BINARYINT_SPOW; break;			
		}
		return;
	}

	thop->t=name;
	thop->fcn=BSVM_ThOp_BINARYOP_S;
}

void BSVM_ThOp_DecodeBinaryOpI(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, int op)
{
	if(op==BSVM_OPR_ATAN2)
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_HINT_XF: thop->fcn=BSVM_ThOp_BINARYINT_ATAN2_XF; break;
		case BSVM_PF_HINT_XD: thop->fcn=BSVM_ThOp_BINARYINT_ATAN2_XD; break;
		default: thop->fcn=BSVM_ThOp_BINARYINT_ATAN2; break;			
		}
		return;
	}

	thop->i=op;
	thop->fcn=BSVM_ThOp_BINARYOP;
}

void BSVM_ThOp_DecodeTrinaryIntrinS(
	BSVM_SVMState *ctx, BSVM_ThreadOp *thop, dytf name)
{
	thop->t=name;
//	thop->fcn=BSVM_ThOp_BINARYOP_S;
	thop->fcn=BSVM_ThOp_NOP;
}
