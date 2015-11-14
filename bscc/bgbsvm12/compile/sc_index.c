#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbsvm.h>

static int bsvmc_seq=1;

BGBSVM_API int BSVMC_IndexLit(BSVM_SVMCState *ctx, dyt l)
{
	int i;

	if(!l)return(0);
	for(i=1; ctx->lit[i]; i++)
		if(dyeqp(ctx->lit[i], l))
			return(i);
	ctx->lit[i]=l;
	return(i);
}

BGBSVM_API int BSVMC_IndexLit2(BSVM_SVMCState *ctx, dyt l)
{
	int i;

	if(!l)return(0);
	for(i=1; ctx->lit[i]; i++)
		if(ctx->lit[i]==l)
			return(i);
	ctx->lit[i]=l;
	return(i);
}

BGBSVM_API void BSVMC_EmitOp(BSVM_SVMCState *ctx, int i)
{
#if 1
#if 1
#if 1
	if(i>=50331648)
	{
		*ctx->ip++=255;
		*ctx->ip++=(i>>24)&0xFF;
		*ctx->ip++=(i>>16)&0xFF;
		*ctx->ip++=(i>>8)&0xFF;
		*ctx->ip++=i&0xFF;
	}else
#endif
	if(i>=786432)
	{
		*ctx->ip++=(i>>24)+252;
		*ctx->ip++=(i>>16)&0xFF;
		*ctx->ip++=(i>>8)&0xFF;
		*ctx->ip++=i&0xFF;
	}else
#endif
	if(i>=12288)
	{
		*ctx->ip++=(i>>16)+240;
		*ctx->ip++=(i>>8)&0xFF;
		*ctx->ip++=i&0xFF;
	}else
#endif
	if(i>=192)
	{
		*ctx->ip++=(i>>8)+192;
		*ctx->ip++=i&0xFF;
	}else *ctx->ip++=i;
}

BGBSVM_API void BSVMC_EncIDX(BSVM_SVMCState *ctx, int i)
{
	if(i<128)
	{
		*ctx->ip++=i;
		return;
	}

	if(i<0x4000)
	{
		*ctx->ip++=(i>>8)|0x80;
		*ctx->ip++=i&255;
		return;
	}

	if(i<0x200000)
	{
		*ctx->ip++=(i>>16)|0xC0;
		*ctx->ip++=(i>>8)&255;
		*ctx->ip++=i&255;
		return;
	}

	if(i<0x10000000)
	{
		*ctx->ip++=(i>>24)|0xE0;	*ctx->ip++=(i>>16)&255;
		*ctx->ip++=(i>>8)&255;		*ctx->ip++=i&255;
		return;
	}

	*ctx->ip++=0xF0;			*ctx->ip++=(i>>24)&255;
	*ctx->ip++=(i>>16)&255;		*ctx->ip++=(i>>8)&255;
	*ctx->ip++=i&255;
	return;

//	*ctx->ip++=(i>>8)|128;
//	*ctx->ip++=i&255;
}

BGBSVM_API void BSVMC_EncVLI(BSVM_SVMCState *ctx, s64 i)
{
	if(i<0)
	{
		//bad value, force to 0
		*ctx->ip++=0;
		return;
	}

	if(i<0x80)
	{
		*ctx->ip++=i;
		return;
	}

	if(i<0x4000)
	{
		*ctx->ip++=(i>>8)|0x80;
		*ctx->ip++=i&255;
		return;
	}

	if(i<0x200000)
	{
		*ctx->ip++=(i>>16)|0xC0;
		*ctx->ip++=(i>>8)&255;
		*ctx->ip++=i&255;
		return;
	}

	if(i<0x10000000)
	{
		*ctx->ip++=(i>>24)|0xE0;	*ctx->ip++=(i>>16)&255;
		*ctx->ip++=(i>>8)&255;		*ctx->ip++=i&255;
		return;
	}

	if(i<0x0800000000LL)
	{
		*ctx->ip++=(i>>32)|0xF0;	*ctx->ip++=(i>>24)&255;
		*ctx->ip++=(i>>16)&255;		*ctx->ip++=(i>>8)&255;
		*ctx->ip++=i&255;
		return;
	}

	if(i<0x040000000000LL)
	{
		*ctx->ip++=(i>>40)|0xF8;	*ctx->ip++=(i>>32)&255;
		*ctx->ip++=(i>>24)&255;		*ctx->ip++=(i>>16)&255;
		*ctx->ip++=(i>>8)&255;		*ctx->ip++=i&255;
		return;
	}

	if(i<0x02000000000000LL)
	{
		*ctx->ip++=(i>>48)|0xFC;	*ctx->ip++=(i>>40)&255;
		*ctx->ip++=(i>>32)&255;		*ctx->ip++=(i>>24)&255;
		*ctx->ip++=(i>>16)&255;		*ctx->ip++=(i>>8)&255;
		*ctx->ip++=i&255;
		return;
	}

	if(i<0x0100000000000000LL)
	{
		*ctx->ip++=(i>>56)|0xFE;	*ctx->ip++=(i>>48)&255;
		*ctx->ip++=(i>>40)&255;		*ctx->ip++=(i>>32)&255;
		*ctx->ip++=(i>>24)&255;		*ctx->ip++=(i>>16)&255;
		*ctx->ip++=(i>>8)&255;		*ctx->ip++=i&255;
		return;
	}

	*ctx->ip++=0xFF;
	*ctx->ip++=(i>>56)&255;		*ctx->ip++=(i>>48)&255;
	*ctx->ip++=(i>>40)&255;		*ctx->ip++=(i>>32)&255;
	*ctx->ip++=(i>>24)&255;		*ctx->ip++=(i>>16)&255;
	*ctx->ip++=(i>>8)&255;		*ctx->ip++=i&255;
	return;
}

BGBSVM_API void BSVMC_EncSVLI(BSVM_SVMCState *ctx, s64 i)
{
	BSVMC_EncVLI(ctx, (i>=0)?(i<<1):(((-i)<<1)-1));
}

BGBSVM_API void BSVMC_EncFloat(BSVM_SVMCState *ctx, double val)
{
	u64 ulv;
	s64 lm, le;
	int sg;
	
	ulv=*(u64 *)(&val);
	
	if(!ulv)
	{
		BSVMC_EncSVLI(ctx, 0);
		BSVMC_EncSVLI(ctx, 0);
		return;
	}
	
	lm=ulv&0x000FFFFFFFFFFFFFULL;		//mantissa
	lm|=0x0010000000000000ULL;			//hidden bit
	le=((ulv>>52)&0x7FF)-1023-52;		//base exponent
	sg=(ulv&0x8000000000000000ULL)?1:0;
	
	//sign bit
	if(sg)lm=-lm;
	
	//remove needless 0's
	while(!(lm&1))
		{ lm>>=1; le++; }

	BSVMC_EncSVLI(ctx, lm);
	BSVMC_EncSVLI(ctx, le);
}

BGBSVM_API void BSVMC_EncByte(BSVM_SVMCState *ctx, int i)
{
	*ctx->ip++=i&255;
}

BGBSVM_API void BSVMC_EncWord(BSVM_SVMCState *ctx, int i)
{
	*ctx->ip++=i&255;
	*ctx->ip++=(i>>8)&255;
}

BGBSVM_API void BSVMC_EncIndexLit(BSVM_SVMCState *ctx, dyt l)
{
	int i;
	i=BSVMC_IndexLit(ctx, l);
	BSVMC_EncIDX(ctx, i);
}

BGBSVM_API void BSVMC_EncIndexLit2(BSVM_SVMCState *ctx, dyt l)
{
	int i;
	i=BSVMC_IndexLit2(ctx, l);
	BSVMC_EncIDX(ctx, i);
}

BGBSVM_API void BSVMC_EmitJmpAddr(BSVM_SVMCState *ctx, char *name)
{
	BSVMC_EncWord(ctx, 0);
	BSVMC_EmitGoto(ctx, name, ctx->ip);
}

BGBSVM_API void BSVMC_EmitGoto(BSVM_SVMCState *ctx, char *name, byte *ip)
{
	ctx->goto_name[ctx->n_goto]=strdup(name);
	ctx->goto_dest[ctx->n_goto]=ip;
	ctx->n_goto++;
}

BGBSVM_API void BSVMC_EmitLabel(BSVM_SVMCState *ctx, char *name, byte *ip)
{
	ctx->lbl_name[ctx->n_lbl]=strdup(name);
	ctx->lbl_dest[ctx->n_lbl]=ip;
	ctx->n_lbl++;
}

BGBSVM_API void BSVMC_EmitJmp(BSVM_SVMCState *ctx, char *name)
{
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
	BSVMC_EncWord(ctx, 0);
	BSVMC_EmitGoto(ctx, name, ctx->ip);
}

BGBSVM_API char *BSVMC_GenSym()
{
	char buf[32];

	sprintf(buf, "gs$%d", bsvmc_seq++);
	return(gcrstrdup(buf));
}

int BSVMC_RegIdx(dyt t)
{
	char *s;

	if(!dysymbolp(t))return(-1);
	s=dysymbolv(t);
	if(s[0]!='$')return(-1);
	if((s[1]<'0') || (s[1]>'9'))return(-1);
	return(atoi(s+1));
}

BGBSVM_API void BSVMC_CompileBreak(BSVM_SVMCState *ctx)
{
	char *s;

	s=ctx->breakstack[ctx->breakstackpos-1];
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileContinue(BSVM_SVMCState *ctx)
{
	char *s;

	s=ctx->contstack[ctx->contstackpos-1];
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileBreakFalse(BSVM_SVMCState *ctx)
{
	char *s;

	s=ctx->breakstack[ctx->breakstackpos-1];
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_FALSE);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileBreakTrue(BSVM_SVMCState *ctx)
{
	char *s;

	s=ctx->breakstack[ctx->breakstackpos-1];
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_TRUE);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileContinueFalse(BSVM_SVMCState *ctx)
{
	char *s;

	s=ctx->contstack[ctx->contstackpos-1];
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_FALSE);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileContinueTrue(BSVM_SVMCState *ctx)
{
	char *s;

	s=ctx->contstack[ctx->contstackpos-1];
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_TRUE);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}


BGBSVM_API void BSVMC_CompileJmp(BSVM_SVMCState *ctx, char *s)
{
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileJmpFalse(BSVM_SVMCState *ctx, char *s)
{
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_FALSE);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileJmpTrue(BSVM_SVMCState *ctx, char *s)
{
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_TRUE);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileJmpCond(BSVM_SVMCState *ctx, int i, char *s)
{
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_COND_B);
	*ctx->ip++=i;
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileJmpSwitchEqual(
	BSVM_SVMCState *ctx, dyt v, char *s)
{
	if(dyfixintp(v))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_JMP_SWEQF); }
	else
		{ BSVMC_EmitOp(ctx, BSVM_SOP_JMP_SWEQ); }

	BSVMC_EncIndexLit(ctx, v);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileJmpSwitchDefault(
	BSVM_SVMCState *ctx, char *s)
{
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_SWDFL);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileJmpSwitchCond(BSVM_SVMCState *ctx,
	int i, dyt v, char *s)
{
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_SWB);
	*ctx->ip++=i;
	BSVMC_EncIndexLit(ctx, v);
	*ctx->ip++=0;
	*ctx->ip++=0;
	BSVMC_EmitGoto(ctx, s, ctx->ip);
}

BGBSVM_API void BSVMC_CompileJCO(
	BSVM_SVMCState *ctx, int op, dyt ln, dyt rn)
{
	dyt lt, rt;
	dyt t0, t1;
	int i, j, k;

	ln=BSVMC_ReduceExpr(ctx, ln);
	rn=BSVMC_ReduceExpr(ctx, rn);
	lt=BSVMC_InferExpr(ctx, ln);
	rt=BSVMC_InferExpr(ctx, rn);


	if(dyintp(ln) || dyflonump(ln))
	{
		switch(op)
		{
			case BSVM_OPR_L: op=BSVM_OPR_G; break;
			case BSVM_OPR_G: op=BSVM_OPR_L; break;
			case BSVM_OPR_LE: op=BSVM_OPR_GE; break;
			case BSVM_OPR_GE: op=BSVM_OPR_LE; break;
			default: break;
		}

		t0=ln; t1=lt;
		ln=rn; lt=rt;
		rn=t0; rt=t1;
	}

#if 0
	j=BSVMC_CheckLexical(ctx, ln);
	k=BSVMC_CheckLexical(ctx, rn);

	if((lt==rt) && BSVMC_TypeFixnumP(ctx, lt) && (j>=0) && (k>=0) &&
		!ctx->i_cap)
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_E_LFN; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_LFN; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_E_LFN; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_LFN; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_L_LFN; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_G_LFN; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_LFN; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_LFN; break;
			default: break;
		}

		BSVMC_EmitOp(ctx, i);
		*ctx->ip++=j;
		*ctx->ip++=k;
		*ctx->ip++=0;
		*ctx->ip++=0;
		return;
	}

	if(BSVMC_TypeFixnumP(ctx, lt) && (j>=0) && (rn==dyIntF(0)) &&
		!ctx->i_cap)
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_E_LZFN; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_LZFN; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_E_LZFN; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_LZFN; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_L_LZFN; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_G_LZFN; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_LZFN; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_LZFN; break;
			default: break;
		}

		BSVMC_EmitOp(ctx, i);
		*ctx->ip++=j;
		*ctx->ip++=0;
		*ctx->ip++=0;
		return;
	}

	if(BSVMC_TypeFixnumP(ctx, lt) && (j>=0) && dyfixintp(rn) &&
		(dyintv(rn)>-128) && (dyintv(rn)<128) &&
		!ctx->i_cap)
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_E_LFNC; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_LFNC; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_E_LFNC; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_LFNC; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_L_LFNC; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_G_LFNC; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_LFNC; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_LFNC; break;
			default: break;
		}

		BSVMC_EmitOp(ctx, i);
		*ctx->ip++=j;
		*ctx->ip++=dyintv(rn);
		*ctx->ip++=0;
		*ctx->ip++=0;
		return;
	}
#endif

#ifdef BSVM_USE_BVT
//#if 0
//	if(BSVMC_TypeSmallIntP(ctx, lt) && dyintp(rn))
	if(BSVMC_TypeSmallDoubleP(ctx, lt) && dyintp(rn))
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_EQ_C; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_C; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_EQ_C; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_C; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_LT_C; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_GT_C; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_C; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_C; break;
			default: break;
		}

		BSVMC_CompileExpr(ctx, ln);

		if(BSVMC_TypeSmallIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
		if(BSVMC_TypeLongP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XL);
		if(BSVMC_TypeFloatP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XF);
		if(BSVMC_TypeDoubleP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XD);

		BSVMC_EmitOp(ctx, i);
//		*ctx->ip++=dyintv(rn);
		BSVMC_EncSVLI(ctx, dyintv(rn));
		*ctx->ip++=0; *ctx->ip++=0;
		return;
	}

//	if(BSVMC_TypeSmallIntP(ctx, lt) && BSVMC_TypeSmallIntP(ctx, rt))
	if(BSVMC_TypeSmallDoubleP(ctx, lt) && (lt==rt))
	{
		switch(op)
		{
//			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_EQ_XI; break;
//			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_XI; break;
//			case BSVM_OPR_L: i=BSVM_SOP_JMP_LT_XI; break;
//			case BSVM_OPR_G: i=BSVM_SOP_JMP_GT_XI; break;
//			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_XI; break;
//			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_XI; break;

			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_EQ; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_LT; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_GT; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE; break;
			default: break;
		}

		BSVMC_CompileExpr(ctx, ln);
		BSVMC_CompileExpr(ctx, rn);

		if(BSVMC_TypeSmallIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
		if(BSVMC_TypeLongP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XL);
		if(BSVMC_TypeFloatP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XF);
		if(BSVMC_TypeDoubleP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XD);

		BSVMC_EmitOp(ctx, i);
//		*ctx->ip++=dyintv(rn);
//		BSVMC_EncSVLI(ctx, dyintv(rn));
		*ctx->ip++=0; *ctx->ip++=0;
		return;
	}
#endif

//	if(BSVMC_TypeFixnumP(ctx, lt) && (rn==dyIntF(0)))
	if(BSVMC_TypeSmallFixIntP(ctx, lt) && (rn==dyIntF(0)))
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_E_ZFN; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_ZFN; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_E_ZFN; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_ZFN; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_L_ZFN; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_G_ZFN; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_ZFN; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_ZFN; break;
			default: break;
		}

		BSVMC_CompileExpr(ctx, ln);

#ifdef BSVM_USE_BVT
		if(BSVMC_TypeSmallIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
#endif

		BSVMC_EmitOp(ctx, i);
		*ctx->ip++=0; *ctx->ip++=0;
		return;
	}

//	if(BSVMC_TypeFixnumP(ctx, lt) && dyfixintp(rn) &&
//		(dyintv(rn)>-128) && (dyintv(rn)<128))
	if(BSVMC_TypeSmallFixIntP(ctx, lt) && dyfixintp(rn) &&
		(dyintv(rn)>-128) && (dyintv(rn)<128))
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_E_FNC; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_FNC; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_E_FNC; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_FNC; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_L_FNC; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_G_FNC; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_FNC; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_FNC; break;
			default: break;
		}

		BSVMC_CompileExpr(ctx, ln);

#ifdef BSVM_USE_BVT
		if(BSVMC_TypeSmallIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
#endif

		BSVMC_EmitOp(ctx, i);
		*ctx->ip++=dyintv(rn);
		*ctx->ip++=0; *ctx->ip++=0;
		return;
	}

	if(BSVMC_TypeSmallFixIntP(ctx, lt) && 
			BSVMC_TypeSmallFixIntP(ctx, rt))
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_E_FN; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_FN; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_E_FN; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_FN; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_L_FN; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_G_FN; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_FN; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_FN; break;
			default: break;
		}

		BSVMC_CompileExpr(ctx, ln);
		BSVMC_CompileExpr(ctx, rn);

#ifdef BSVM_USE_BVT
		if(BSVMC_TypeSmallIntP(ctx, lt) &&
				BSVMC_TypeSmallIntP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
#endif

		BSVMC_EmitOp(ctx, i);
		*ctx->ip++=0; *ctx->ip++=0;
		return;
	}

	if(BSVMC_TypeFixrealP(ctx, lt) &&
		BSVMC_TypeFixrealP(ctx, rt))
	{
		switch(op)
		{
			case BSVM_OPR_EQV: i=BSVM_SOP_JMP_E_FL; break;
			case BSVM_OPR_NEQV: i=BSVM_SOP_JMP_NE_FL; break;
			case BSVM_OPR_EQ: i=BSVM_SOP_JMP_E_FL; break;
			case BSVM_OPR_NEQ: i=BSVM_SOP_JMP_NE_FL; break;
			case BSVM_OPR_L: i=BSVM_SOP_JMP_L_FL; break;
			case BSVM_OPR_G: i=BSVM_SOP_JMP_G_FL; break;
			case BSVM_OPR_LE: i=BSVM_SOP_JMP_LE_FL; break;
			case BSVM_OPR_GE: i=BSVM_SOP_JMP_GE_FL; break;
			default: break;
		}

		BSVMC_CompileExpr(ctx, ln);
		if(BSVMC_TypeFixnumP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FN2FL);
		BSVMC_CompileExpr(ctx, rn);
		if(BSVMC_TypeFixnumP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FN2FL);

		BSVMC_EmitOp(ctx, i);
		*ctx->ip++=0; *ctx->ip++=0;
		return;
	}

	BSVMC_CompileExpr(ctx, ln);
	BSVMC_CompileExpr(ctx, rn);
	BSVMC_EmitOp(ctx, BSVM_SOP_JMP_COND_B);
	*ctx->ip++=op;
	*ctx->ip++=0; *ctx->ip++=0;
}

BGBSVM_API void BSVMC_CompileJCT(BSVM_SVMCState *ctx, dyt l)
{
	dyt t;
	int i;

	l=BSVMC_ReduceExpr(ctx, l);
	t=BSVMC_InferExpr(ctx, l);

	if(BSVMC_FormIs(l, "unary") && (dycadr(l)==dysymbol("!")))
	{
		BSVMC_CompileJCF(ctx, dycaddr(l));
		return;
	}

	i=-1;
	if(BSVMC_FormIsBinary(l, "=="))i=BSVM_OPR_EQV;
	if(BSVMC_FormIsBinary(l, "!="))i=BSVM_OPR_NEQV;
	if(BSVMC_FormIsBinary(l, "==="))i=BSVM_OPR_EQ;
	if(BSVMC_FormIsBinary(l, "!=="))i=BSVM_OPR_NEQ;
	if(BSVMC_FormIsBinary(l, "<"))i=BSVM_OPR_L;
	if(BSVMC_FormIsBinary(l, ">"))i=BSVM_OPR_G;
	if(BSVMC_FormIsBinary(l, "<="))i=BSVM_OPR_LE;
	if(BSVMC_FormIsBinary(l, ">="))i=BSVM_OPR_GE;

	if(i<0)
	{
#if 1
		i=BSVMC_CheckLexical(ctx, l);
		if(BSVMC_TypeSmallFixIntP(ctx, t) && (i>=0) && !ctx->i_cap)
		{
#ifdef BSVM_USE_BVT
			if(BSVMC_TypeSmallIntP(ctx, t))
				BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
#endif

			BSVMC_EmitOp(ctx, BSVM_SOP_JMP_NE_LZFN);
			*ctx->ip++=i;
			*ctx->ip++=0; *ctx->ip++=0;
			return;
		}

//		if(t==dykeyword("int"))
		if(BSVMC_TypeSmallFixIntP(ctx, t))
		{
			BSVMC_CompileExpr(ctx, l);

#ifdef BSVM_USE_BVT
			if(BSVMC_TypeSmallIntP(ctx, t))
				BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
#endif

			BSVMC_EmitOp(ctx, BSVM_SOP_JMP_NE_ZFN);
			*ctx->ip++=0; *ctx->ip++=0;
			return;
		}
#endif

		BSVMC_CompileExpr(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_JMP_TRUE);
		*ctx->ip++=0;
		*ctx->ip++=0;
		return;
	}

	BSVMC_CompileJCO(ctx, i, dycaddr(l), dycadddr(l));
}

BGBSVM_API void BSVMC_CompileJCF(BSVM_SVMCState *ctx, dyt l)
{
	dyt t;
	int i;

	l=BSVMC_ReduceExpr(ctx, l);
	t=BSVMC_InferExpr(ctx, l);

	if(BSVMC_FormIs(l, "unary") && (dycadr(l)==dysymbol("!")))
	{
		BSVMC_CompileJCT(ctx, dycaddr(l));
		return;
	}

	i=-1;
	if(BSVMC_FormIsBinary(l, "=="))i=BSVM_OPR_NEQV;
	if(BSVMC_FormIsBinary(l, "!="))i=BSVM_OPR_EQV;
	if(BSVMC_FormIsBinary(l, "==="))i=BSVM_OPR_NEQ;
	if(BSVMC_FormIsBinary(l, "!=="))i=BSVM_OPR_EQ;
	if(BSVMC_FormIsBinary(l, "<"))i=BSVM_OPR_GE;
	if(BSVMC_FormIsBinary(l, ">"))i=BSVM_OPR_LE;
	if(BSVMC_FormIsBinary(l, "<="))i=BSVM_OPR_G;
	if(BSVMC_FormIsBinary(l, ">="))i=BSVM_OPR_L;

	if(i<0)
	{
		i=BSVMC_CheckLexical(ctx, l);
		if(BSVMC_TypeSmallFixIntP(ctx, t) && (i>=0) && !ctx->i_cap)
		{
#ifdef BSVM_USE_BVT
			if(BSVMC_TypeSmallIntP(ctx, t))
				BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
#endif

			BSVMC_EmitOp(ctx, BSVM_SOP_JMP_E_LZFN);
			*ctx->ip++=i;
			*ctx->ip++=0; *ctx->ip++=0;
			return;
		}

		if(BSVMC_TypeSmallFixIntP(ctx, t))
		{
			BSVMC_CompileExpr(ctx, l);

#ifdef BSVM_USE_BVT
			if(BSVMC_TypeSmallIntP(ctx, t))
				BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
#endif

			BSVMC_EmitOp(ctx, BSVM_SOP_JMP_E_ZFN);
			*ctx->ip++=0; *ctx->ip++=0;
			return;
		}

		BSVMC_CompileExpr(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_JMP_FALSE);
		*ctx->ip++=0;
		*ctx->ip++=0;
		return;
	}

	BSVMC_CompileJCO(ctx, i, dycaddr(l), dycadddr(l));
}
