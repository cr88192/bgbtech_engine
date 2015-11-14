#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbsvm.h>

// #define BSVM_USE_BVT

int BSVMC_CompileSpecial(BSVM_SVMCState *ctx, dytf l)
{
	dytf c, t, u, v;
	char *s0, *s1, *s2, *s3;
	int i, j, na;

	na=dytfListLength(dytfCdr(l));

	if((na==1) && dytfFormIsP(l, "infer"))
	{
		t=BSVMC_InferExpr(ctx, dytfCadr(l));
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		return(1);
	}

#if 1
	if((na==2) && dytfFormIsP(l, "atan2"))
	{
		u=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		v=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
		BSVMC_CompileExpr(ctx, u);
		BSVMC_CompileExpr(ctx, v);

//		BSVMC_CompileExpr(ctx, dytfCadr(l));
//		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_CompileInferBinaryTypePrefix(ctx, u, v);
		BSVMC_EmitOp(ctx, BSVM_SOP_BINARYOP);
		BSVMC_EncIDX(ctx, BSVM_OPR_ATAN2);
		return(1);
	}

	i=-1;
	if((na==2) && dytfFormIsP(l, "pow"))i=1;
	if((na==2) && dytfFormIsP(l, "spow"))i=1;
	if(i>0)
	{
		u=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		v=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
		BSVMC_CompileExpr(ctx, u);
		BSVMC_CompileExpr(ctx, v);
		BSVMC_CompileInferBinaryTypePrefix(ctx, u, v);
		BSVMC_EmitOp(ctx, BSVM_SOP_BINARYINTRIN_S);
		BSVMC_EncIndexLit(ctx, dytfCar(l));
		return(1);
	}

	i=-1;
	if((na==1) && dytfFormIsP(l, "sin"))i=BSVM_OPR_SIN;
	if((na==1) && dytfFormIsP(l, "cos"))i=BSVM_OPR_COS;
	if((na==1) && dytfFormIsP(l, "tan"))i=BSVM_OPR_TAN;
	if((na==1) && dytfFormIsP(l, "asin"))i=BSVM_OPR_ASIN;
	if((na==1) && dytfFormIsP(l, "acos"))i=BSVM_OPR_ACOS;
	if((na==1) && dytfFormIsP(l, "atan"))i=BSVM_OPR_ATAN;
	if((na==1) && dytfFormIsP(l, "sqr"))i=BSVM_OPR_SQR;
	if((na==1) && dytfFormIsP(l, "sqrt"))i=BSVM_OPR_SQRT;
	if(i>0)
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		BSVMC_CompileExpr(ctx, t);

//		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_CompileInferUnaryTypePrefix(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
		BSVMC_EncIDX(ctx, i);
		return(1);
	}

	i=-1;
	if((na==1) && dytfFormIsP(l, "ssqr"))i=1;
	if((na==1) && dytfFormIsP(l, "ssqrt"))i=1;
	if(i>0)
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_CompileInferUnaryTypePrefix(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYINTRIN_S);
		BSVMC_EncIndexLit(ctx, dytfCar(l));
		return(1);
	}

#endif

	if((na==2) && dytfFormIsP(l, "cons"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CONS);
		return(1);
	}

	if((na==1) && dytfFormIsP(l, "clone"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CLONE);
		return(1);
	}

	if((na==1) && dytfFormIsP(l, "assert"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_ASSERT);
		return(1);
	}

	if(dytfFormIsP(l, "join"))
	{
		if(!dytfNullP(dytfCddr(l)))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
			c=dytfCdr(l);
			while(dytfConsP(c))
			{
				BSVMC_CompileExpr(ctx, dytfCar(c));
				c=dytfCdr(c);
			}
			BSVMC_EmitOp(ctx, BSVM_SOP_MULTIJOIN);
			return(1);
		}
	
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_JOIN);
		return(1);
	}

	if((na==1) && dytfFormIsP(l, "sleep"))
	{
		v=dytfCadr(l);
		if(dytfIntP(v))
		{
			i=dytfIntv(dytfCadr(l));
			if((i>=0) && (i<65536))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_SLEEP_SV);
				BSVMC_EncWord(ctx, i);
				return(1);
			}
		}
		BSVMC_CompileExpr(ctx, v);
		BSVMC_EmitOp(ctx, BSVM_SOP_SLEEP);
		return(1);
	}

	if(dytfFormIsP(l, "this"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCdr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_THIS);
		return(1);
	}

	if(dytfFormIsP(l, "super"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCdr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_SUPER);
		return(1);
	}

	s0=dytfSymbolv(dytfCar(l));
	s1=s0+(strlen(s0)-1);

	i=0;
	if((*s0=='c') && (*s1=='r'))
	{
		s2=s0+1;
		while((s2<s1) && ((*s2=='a') || (*s2=='d')))
			s2++;
		if(s2==s1)i=1;
	}

	if(!i)return(0);

	BSVMC_CompileExpr(ctx, dytfCadr(l));

	s0++; s2=s1-1;
	while((s2-1)>=s0)
	{
		if(!strncmp(s2-1, "aa", 2))BSVMC_EmitOp(ctx, BSVM_SOP_CAAR);
		if(!strncmp(s2-1, "da", 2))BSVMC_EmitOp(ctx, BSVM_SOP_CDAR);
		if(!strncmp(s2-1, "ad", 2))BSVMC_EmitOp(ctx, BSVM_SOP_CADR);
		if(!strncmp(s2-1, "dd", 2))BSVMC_EmitOp(ctx, BSVM_SOP_CDDR);
		s2-=2;
	}
	while(s2>=s0)
	{
		if(*s2=='a')BSVMC_EmitOp(ctx, BSVM_SOP_CAR);
			else BSVMC_EmitOp(ctx, BSVM_SOP_CDR);
		s2--;
	}

	return(1);
}

int BSVMC_CompileSpecialMethod(BSVM_SVMCState *ctx, dytf l)
{
	if(dytfEqqP(dytfCadr(l), dytfSymbol("toString")))
	{
		BSVMC_CompileExpr(ctx, dytfCar(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_TOSTRING);
		return(1);
	}

	if(dytfEqqP(dytfCadr(l), dytfSymbol("clone")))
	{
		BSVMC_CompileExpr(ctx, dytfCar(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CLONE);
		return(1);
	}

	return(0);
}

int BSVMC_CheckLexical(BSVM_SVMCState *ctx, dytf l)
{
	dytf c, t;
	int i;

	if(!dytfSymbolP(l))return(-1);

	c=dytfWrap(ctx->lenv); i=0;
	while(dytfConsP(c))
	{
		if(dytfEqP(dytfCar(c), l))break;
		c=dytfCdr(c);
		i++;
	}

	if(dytfConsP(c))return(i);
	return(-1);
}

void BSVMC_CompileExprCast(BSVM_SVMCState *ctx, dytf l, dytf ty)
{
	BSVMC_CompileExprCastFl(ctx, l, ty, 0);
}

void BSVMC_CompileExprCastFl(BSVM_SVMCState *ctx,
	dytf l, dytf ty, int flags)
{
	dytf n, t, u, c, ty2;
	char *s;
	int i, j;

	n=BSVMC_ReduceExpr(ctx, l);
	ty2=BSVMC_InferExpr(ctx, n);

	if(dytfEqP(ty2, ty))
	{
		BSVMC_CompileExpr(ctx, n);
		return;
	}

	if(dytfNullP(n))
	{
		if(BSVMC_FormIs(ty, "emptyindex"))
		{
			BSVMC_CompileExpr(ctx, n);
			return;
		}

		if(BSVMC_FormIs(ty, "getindex"))
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(ty));

			s=BSVMC_InferTypeBaseSig(dytfCadr(ty));
			t=dytfSymbol(s);

			i=BSVMC_IndexLit(ctx, t);
			BSVMC_EmitOp(ctx, BSVM_SOP_NEWARRAY_S);
			BSVMC_EncIDX(ctx, i);
			return;
		}
	}

#if 1
	if(dytfFormIsP(l, "funcall") && dytfSymbolP(dytfCadr(l)))
	{
		if(dytfSymbolP(dytfCadr(l)))
		{
			i=BSVMC_CompileSpecial(ctx, dytfCdr(l));
			if(i>0)return;
		}

		s=BSVMC_InferCallArgsBaseSig(ctx, dytfCdr(l), ty);
		BSVMC_EmitOp(ctx, BSVM_SOP_TYMARK);
		BSVMC_EncIndexLit(ctx, dytfSymbol(s));

		c=dytfCddr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}

		if(dytfEqP(dytfCadr(l), dytfWrap(ctx->cf_n)))
		{
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_CF);
				else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_CF);
			return;
		}

		if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_S);
			else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_S);
		BSVMC_EncIndexLit(ctx, dytfCadr(l));
		return;
	}
#endif

#ifdef BSVM_USE_BVT
	if(BSVMC_TypeSmallIntP(ctx, ty))
	{
		if(dytfIntP(n) || dytfNullP(n))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_XI);
			BSVMC_EncSVLI(ctx, dytfIntv(n));
			return;
		}

		BSVMC_CompileExpr(ctx, n);
		if(!BSVMC_TypeSmallIntP(ctx, ty2))
		{
			if(!(flags&1))BSVMC_Warning(ctx,
				"unsafe implicit conversion");
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FN2XI);
		}
		return;
	}

	if(BSVMC_TypeLongP(ctx, ty))
	{
		if(dytfLongP(n) || dytfNullP(n))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_XL);
			BSVMC_EncSVLI(ctx, dytfLongv(n));
			return;
		}

		BSVMC_CompileExpr(ctx, n);
		if(!BSVMC_TypeLongP(ctx, ty2))
		{
			if(BSVMC_TypeSmallIntP(ctx, ty2))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XI2XL);
			}else if(BSVMC_TypeFloatP(ctx, ty2))
			{
				if(!(flags&1))BSVMC_Warning(ctx,
					"conversion may lose precision");
				BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XF2XL);
			}else if(BSVMC_TypeDoubleP(ctx, ty2))
			{
				if(!(flags&1))BSVMC_Warning(ctx,
					"conversion may lose precision");
				BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XD2XL);
			}else
			{
				if(!(flags&1))BSVMC_Warning(ctx,
					"unsafe implicit conversion");
				BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FN2XL);
			}
		}
		return;
	}

	if(BSVMC_TypeFloatP(ctx, ty))
	{
		BSVMC_CompileExpr(ctx, n);
		if(!BSVMC_TypeFloatP(ctx, ty2))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FL2XF);
		return;
	}

	if(BSVMC_TypeDoubleP(ctx, ty))
	{
		BSVMC_CompileExpr(ctx, n);
		if(!BSVMC_TypeDoubleP(ctx, ty2))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FL2XD);
		return;
	}
#endif

	BSVMC_CompileExpr(ctx, n);

	u=BSVMC_InferArgsBaseSig2(ty);
	i=BSVMC_IndexLit(ctx, u);
	BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
	BSVMC_EncIDX(ctx, i);

	return;
}

void BSVMC_CompileAssign(BSVM_SVMCState *ctx, dytf l)
{
	dytf c, ct, cv, t, u;
	int i, j;

	if(dytfFormIsP(l, "objref"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_S);
		BSVMC_EncIndexLit(ctx, dytfCaddr(l));
		return;
	}

	if(dytfFormIsP(l, "getindex"))
	{
		if(dytfIntP(dytfCaddr(l)))
		{
			i=dytfIntv(dytfCaddr(l));
			if((i>=0) && (i<4))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_0+i);
				return;
			}
			if((i>=0) && (i<255))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_I);
				BSVMC_EncByte(ctx, i);
				return;
			}
		}

		BSVMC_CompileExpr(ctx, dytfCadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
		return;
	}

	if(BSVMC_FormIsUnary(l, "*"))
	{
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_SETDEFER);
		return;
	}

	if(BSVMC_FormIsUnary(l, "&"))
	{
		BSVMC_Error(ctx,
			"BSVMC_CompileAssign: Attempted assign to a reference");
//		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
//		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dytfSymbolP(l))
	{
#if 0
		i=BSVMC_RegIdx(l);
		if(i>=0)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_POP_R);
			BSVMC_EncByte(ctx, i);
			return;
		}
#endif

		c=dytfWrap(ctx->lenv);
		ct=dytfWrap(ctx->tenv);
		cv=dytfWrap(ctx->venv);
		i=0; j=0;
		while(dytfConsP(c))
		{
			if(dytfUnwrap(c)==ctx->llenv)i=1;
			if(dytfEqP(dytfCar(c), l))break;
			c=dytfCdr(c); ct=dytfCdr(ct); cv=dytfCdr(cv);
			j++;
		}
		if(dytfConsP(c) && i)ctx->cap++;

		if(dytfConsP(c))
		{
			t=dytfCar(ct);
			dytfSetCar(cv, DYTF_NULL);

			if(!dytfNullP(t))
			{
				u=BSVMC_InferArgsBaseSig2(t);
				i=BSVMC_IndexLit(ctx, u);
				BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
				BSVMC_EncIDX(ctx, i);
			}

#ifdef BSVM_USE_BVT
			i=-1;
			if(BSVMC_TypeSmallIntP(ctx, t))
				i=BSVM_SOP_LSTORE_XI;
			if(BSVMC_TypeLongP(ctx, t))
				i=BSVM_SOP_LSTORE_XL;
			if(BSVMC_TypeFloatP(ctx, t))
				i=BSVM_SOP_LSTORE_XF;
			if(BSVMC_TypeDoubleP(ctx, t))
				i=BSVM_SOP_LSTORE_XD;

			if(i>=0)
			{
				BSVMC_EmitOp(ctx, i);
				BSVMC_EncByte(ctx, j);
				return;
			}
#endif

//			if((t==dytfKeyword("int")) || (t==dytfKeyword("float")))
			if(BSVMC_TypeSmallFixRealP(ctx, t))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LSTORE_F);
				BSVMC_EncByte(ctx, j);
				return;
			}

			BSVMC_EmitOp(ctx, BSVM_SOP_LSTORE);
			BSVMC_EncByte(ctx, j);
			return;
		}

		if(ctx->pcap)ctx->cap++;		//capture stores if package

		i=BSVMC_IndexLit(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_STORE);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	BSVMC_Error(ctx,
		"BSVMC_CompileAssign: Attempted assign to an invalid expression type");
	BSVMC_EmitOp(ctx, BSVM_SOP_POP);
	return;
}

void BSVMC_CompileNew(BSVM_SVMCState *ctx, dytf l)
{
	byte *ips[64];
	byte *ip0, *ip1, *ip2;
	char *s;
	dytf c, d, t, u, v, n;
	int i, j;
	
	if(dytfFormIsP(l, "funcall"))
	{
#if 0
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		i=BSVMC_IndexLit(ctx, dytfKeyword("parent"));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF);
		BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
		BSVMC_CompileStatement(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
#endif

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCddr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}

		s=BSVMC_FlattenQNameBaseSig(dytfCadr(l));
		if(s)
		{
			t=dytfSymbol(s);
			i=BSVMC_IndexLit(ctx, t);
			BSVMC_EmitOp(ctx, BSVM_SOP_NEW_S);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_NEW);
		return;
	}

	if(dytfFormIsP(l, "getindex"))
	{
		BSVMC_CompileExpr(ctx, dytfCaddr(l));

		s=BSVMC_InferTypeBaseSig(dytfCadr(l));
		t=dytfSymbol(s);

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_NEWARRAY_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}
	
	s=BSVMC_FlattenQNameBaseSig(l);
	if(s)
	{
		t=dytfSymbol(s);
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_NEW_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dytfSymbolP(l))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		i=BSVMC_IndexLit(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_NEW_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}
	
	BSVMC_Error(ctx, "BSVMC_CompileNew: New with invalid expression");
	BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_ERROR);
	return;
}

void BSVMC_CompileInstanceof(BSVM_SVMCState *ctx, dytf l)
{
	char *s;
	dytf t;
	int i;

	BSVMC_CompileExpr(ctx, dytfCadr(l));

	s=BSVMC_InferTypeBaseSig(dytfCaddr(l));
	if(s)
	{
		t=dytfSymbol(s);
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_INSTANCEOF_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	BSVMC_CompileExpr(ctx, dytfCaddr(l));
	BSVMC_EmitOp(ctx, BSVM_SOP_INSTANCEOF);
	return;
}

void BSVMC_CompileForm(BSVM_SVMCState *ctx, dytf l)
{
	char tb[256];
	byte *ips[64];
	byte *ip0, *ip1, *ip2;
	dytf c, d, t, u, v, n;
	char *s;
	s64 li;
	int i, j, k;

	if(dytfKeywordP(dytfCar(l)))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		i=BSVMC_IndexLit(ctx, dytfCar(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	l=BSVMC_ProcessFlagsExpr(ctx, l, tb);
	if(tb[0])ctx->cs_fl=dysymbol(tb);
		else ctx->cs_fl=NULL;

	if(dytfFormIsP(l, "goto"))
	{
		if(!ctx->tail)
		{
			dyPrintf("invalid use of goto\n");
			return;
		}
		
		BSVMC_CompileGoto(ctx, l);
		return;

#if 0
		BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
		BSVMC_EncWord(ctx, 0);
		BSVMC_EmitGoto(ctx,
			dytfSymbolv(dytfCadr(l)), ctx->ip);
		return;
#endif
	}

	if(dytfFormIsP(l, "label"))
	{
		if(!ctx->tail)
		{
			dyPrintf("invalid use of label\n");
			return;
		}

		BSVMC_EmitLabel(ctx, dytfSymbolv(dytfCadr(l)), ctx->ip);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
		return;
	}

	if(	(dytfFormIsP(l, "for")) || (dytfFormIsP(l, "while")) ||
		(dytfFormIsP(l, "for_in")) || (dytfFormIsP(l, "do_while")))
	{
		BSVMC_CompileStatement(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
		return;
	}


	if(dytfFormIsP(l, "import"))
	{
		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		j=BSVMC_IndexLit(ctx, dytfWrap(ctx->cs_fl));
		BSVMC_EmitOp(ctx, BSVM_SOP_IMPORT);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		ctx->pcap++;

		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);

		return;
	}

	if(dytfFormIsP(l, "quote"))
	{
		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dytfFormIsP(l, "unquote"))
	{
//		i=BSVMC_IndexLit(ctx, dytfCadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//		BSVMC_EncIDX(ctx, i);

		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_UNQUOTE);
		return;
	}

	if(dytfFormIsP(l, "return"))
	{
//		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_CompileExprCast(ctx, dytfCadr(l), dytfWrap(ctx->cf_ty));
		if(!ctx->tail)
		{
			if(!(ctx->i_cap))
				BSVMC_EmitOp(ctx, BSVM_SOP_CLEARENV);
			BSVMC_EmitOp(ctx, BSVM_SOP_RET);
		}
		return;
	}

	if(dytfFormIsP(l, "cast"))
	{
//		t=dytfSymbol(s);

		BSVMC_CompileExpr(ctx, dytfCaddr(l));

		t=BSVMC_InferArgsBaseSig2(dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_TRYCAST_S);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	if(dytfFormIsP(l, "cast_force"))
	{
//		t=dytfSymbol(s);

		BSVMC_CompileExpr(ctx, dytfCaddr(l));

		t=BSVMC_InferArgsBaseSig2(dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	if(dytfFormIsP(l, "sizeof"))
	{
		t=BSVMC_InferArgsBaseSig2(dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_SIZEOF_S);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	if(dytfFormIsP(l, "set!") || dytfFormIsP(l, "setexpr!"))
	{
		BSVMC_CompileExpr(ctx, dytfCaddr(l));

#if 0
		i=BSVMC_RegIdx(dytfCadr(l));
		if(i>=0)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_R);
			BSVMC_EncByte(ctx, i);
			return;
		}
#endif

		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		BSVMC_CompileAssign(ctx, dytfCadr(l));
		return;
	}

	if((dytfFormIsP(l, "setindex!")) || (dytfFormIsP(l, "vector-set!")))
	{
		t=dytfCadddr(l);
		u=BSVMC_InferExpr(ctx, t);
		BSVMC_CompileExpr(ctx, t);
//		if((u==dytfKeyword("int")) || (u==dytfKeyword("float")))
		if(BSVMC_TypeSmallFixRealP(ctx, u))
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
			else BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		if(dytfIntP(dytfCaddr(l)))
		{
			i=dytfIntv(dytfCaddr(l));
			if((i>=0) && (i<4))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_0+i);
				return;
			}
			if((i>=0) && (i<255))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_I);
				BSVMC_EncByte(ctx, i);
				return;
			}
		}

//		BSVMC_CompileExpr(ctx, dytfCadddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		BSVMC_CompileExpr(ctx, dytfCadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
		return;
	}

	if((dytfFormIsP(l, "getindex")) || (dytfFormIsP(l, "vector-ref")))
	{
		BSVMC_CompileGetIndex(ctx, l);
		return;
		
#if 0
		if(dytfIntP(dytfCaddr(l)))
		{
			i=dytfIntv(dytfCaddr(l));
			if((i>=0) && (i<4))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_0+i);
				return;
			}
			if((i>=0) && (i<255))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_I);
				BSVMC_EncByte(ctx, i);
				return;
			}
		}

		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX);
		return;
#endif
	}

	if(dytfFormIsP(l, "objref"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
//		i=BSVMC_IndexLit(ctx, dytfCaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//		BSVMC_EncIDX(ctx, i);
//		BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX);

		i=BSVMC_IndexLit(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dytfFormIsP(l, "objset!"))
	{
//		BSVMC_CompileExpr(ctx, dytfCadddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
//
//		BSVMC_CompileExpr(ctx, dytfCadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
//
//		i=BSVMC_IndexLit(ctx, dytfCaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//		BSVMC_EncIDX(ctx, i);
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
//		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);

		t=dytfCadddr(l);
		u=BSVMC_InferExpr(ctx, t);
		BSVMC_CompileExpr(ctx, t);
//		if((u==dytfKeyword("int")) || (u==dytfKeyword("float")))
		if(BSVMC_TypeSmallFixRealP(ctx, u))
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
			else BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		BSVMC_CompileExpr(ctx, dytfCadr(l));
		i=BSVMC_IndexLit(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(BSVMC_FormIs(l, "preinc!") || BSVMC_FormIs(l, "predec!"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		u=BSVMC_InferExpr(ctx, t);
		if(BSVMC_FormIs(l, "preinc!"))i=BSVM_OPR_INC;
			else i=BSVM_OPR_DEC;

		if(dytfSymbolP(t))
		{
//			if(u==dytfKeyword("int"))
			if(BSVMC_TypeSmallFixIntP(ctx, u))
			{
				BSVMC_CompileExpr(ctx, t);
				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC_FN);
					else BSVMC_EmitOp(ctx, BSVM_SOP_DEC_FN);
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
				BSVMC_CompileAssign(ctx, t);
			}else
			{
//				BSVMC_CompileExpr(ctx, t);
//				BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
//				BSVMC_EmitOp(ctx, i);
//				BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
//				BSVMC_CompileAssign(ctx, t);

				j=BSVMC_CheckLexical(ctx, t);
				if((j>=0) && (j<256))
				{
					if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_LPREINC);
						else BSVMC_EmitOp(ctx, BSVM_SOP_LPREDEC);
					BSVMC_EncByte(ctx, j);
					return;
				}

				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_PREINC_S);
					else BSVMC_EmitOp(ctx, BSVM_SOP_PREDEC_S);
				BSVMC_EncIndexLit(ctx, t);
				return;
			}
			return;
		}

		BSVMC_CompileExpr(ctx, t);
//		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
//		BSVMC_EmitOp(ctx, i);
		if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC);
			else BSVMC_EmitOp(ctx, BSVM_SOP_DEC);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		BSVMC_CompileAssign(ctx, t);

		return;
	}

	if(BSVMC_FormIs(l, "postinc!") || BSVMC_FormIs(l, "postdec!"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		u=BSVMC_InferExpr(ctx, t);
		if(BSVMC_FormIs(l, "postinc!"))i=BSVM_OPR_INC;
			else i=BSVM_OPR_DEC;

		if(dytfSymbolP(t))
		{
//			if(u==dytfKeyword("int"))
			if(BSVMC_TypeSmallFixIntP(ctx, u))
			{
				BSVMC_CompileExpr(ctx, t);
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC_FN);
					else BSVMC_EmitOp(ctx, BSVM_SOP_DEC_FN);
				BSVMC_CompileAssign(ctx, t);
			}else
			{
//				BSVMC_CompileExpr(ctx, t);
//				BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
//				BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
//				BSVMC_EmitOp(ctx, i);
//				BSVMC_CompileAssign(ctx, t);

				j=BSVMC_CheckLexical(ctx, t);
				if((j>=0) && (j<256))
				{
					if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_LPOSTINC);
						else BSVMC_EmitOp(ctx, BSVM_SOP_LPOSTDEC);
					BSVMC_EncByte(ctx, j);
					return;
				}

				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_POSTINC_S);
					else BSVMC_EmitOp(ctx, BSVM_SOP_POSTDEC_S);
				BSVMC_EncIndexLit(ctx, t);
				return;
			}
			return;
		}

		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
//		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
//		BSVMC_EmitOp(ctx, i);
		if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC);
			else BSVMC_EmitOp(ctx, BSVM_SOP_DEC);
		BSVMC_CompileAssign(ctx, t);

		return;
	}


	if(dytfFormIsP(l, "if"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));

#if 0
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(l));
			return;
		}
		if(i==0)
		{
			if(dytfConsP(dytfCdddr(l)))
				BSVMC_CompileExpr(ctx, dytfCadddr(l));
			return;
		}
#endif

#if 0
		if(BSVMC_FormIs(t, "unary") && (dytfCadr(t)==dytfSymbol("!")))
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_JMP_TRUE);
			BSVMC_EncWord(ctx, 0);
			ip0=ctx->ip;
		}else
		{
			BSVMC_CompileExpr(ctx, t);
			BSVMC_EmitOp(ctx, BSVM_SOP_JMP_FALSE);
			BSVMC_EncWord(ctx, 0);
			ip0=ctx->ip;
		}
#endif

		BSVMC_CompileJCF(ctx, t);
		ip0=ctx->ip;

//		BSVMC_CompileExprT(ctx, dytfCaddr(l));

		if(ctx->tail)
		{
			if(!BSVMC_FormIs(dytfCaddr(l), "funcall"))
			{
				BSVMC_CompileExprCast(ctx, dytfCaddr(l),
					dytfWrap(ctx->cf_ty));
				if(!(ctx->i_cap))
					BSVMC_EmitOp(ctx, BSVM_SOP_CLEARENV);
				BSVMC_EmitOp(ctx, BSVM_SOP_RET);
			}else
			{
				BSVMC_CompileExprT(ctx, dytfCaddr(l));
			}
		}else
		{
			BSVMC_CompileExprT(ctx, dytfCaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
			BSVMC_EncWord(ctx, 0);
			ip1=ctx->ip;
		}

		i=ctx->ip-ip0;
		*(ip0-2)=i&0xFF;
		*(ip0-1)=(i>>8)&0xFF;

		if(dytfConsP(dytfCdddr(l)))
			BSVMC_CompileExprT(ctx, dytfCadddr(l));
			else BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);

		if(!ctx->tail)
		{
			i=ctx->ip-ip1;
			*(ip1-2)=i&0xFF;
			*(ip1-1)=(i>>8)&0xFF;
		}

		return;
	}

	if(dytfFormIsP(l, "begin"))
	{
		BSVMC_CompileBeginExpr(ctx, l);
		return;

#if 0
		c=dytfCdr(l);
		while(1)
		{
			if(dytfConsP(dytfCdr(c)))
			{
				BSVMC_CompileStatement(ctx, dytfCar(c));
				c=dytfCdr(c);
				continue;
			}

			BSVMC_CompileExprT(ctx, dytfCar(c));
			break;
		}
		return;
#endif
	}

	if(dytfFormIsP(l, "throw"))
	{
		t=dytfCadr(l);
		
		if(dytfSymbolP(t))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);

			BSVMC_EmitOp(ctx, BSVM_SOP_THROW_S);
			BSVMC_EncIndexLit(ctx, t);
			return;
		}
		
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		BSVMC_EmitOp(ctx, BSVM_SOP_THROW_OBJ);
		return;
	}

	if(dytfFormIsP(l, "ifdef") ||
		dytfFormIsP(l, "ifndef"))
	{
		BSVMC_CompileIfdef(ctx, l);
		return;
	}

	if((dytfFormIsP(l, "and")) || (dytfFormIsP(l, "or")))
	{
		if(dytfFormIsP(l, "and"))j=BSVM_SOP_JMP_FALSE;
		if(dytfFormIsP(l, "or"))j=BSVM_SOP_JMP_TRUE;

		i=0; c=dytfCdr(l);
		while(1)
		{
			if(dytfConsP(dytfCdr(c)))
			{
				BSVMC_CompileExpr(ctx, dytfCar(c));
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

				BSVMC_EmitOp(ctx, j);
				BSVMC_EncWord(ctx, 0);
				ips[i++]=ctx->ip;

				BSVMC_EmitOp(ctx, BSVM_SOP_POP);

				c=dytfCdr(c);
				continue;
			}

			BSVMC_CompileExprT(ctx, dytfCar(c));

			while(i>0)
			{
				i--;
				j=ctx->ip-ips[i];
				*(ips[i]-2)=j&0xFF;
				*(ips[i]-1)=(j>>8)&0xFF;
			}

			break;
		}

		return;
	}

	if(dytfFormIsP(l, "define"))
	{
		if(!dytfConsP(dytfCadr(l)))
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
			i=BSVMC_IndexLit(ctx, dytfCadr(l));
			if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
				else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		t=dytfCons(dytfSymbol("lambda"), dytfCons(dytfCdadr(l), dytfCddr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		n=dytfCaadr(l);
		if(dytfFormIsP(n, "cast"))
			n=dytfCaddr(n);
#if 0
		i=BSVMC_IndexLit(ctx, n);
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIDX(ctx, i);
#endif

#if 1
		s=BSVMC_InferArgsBaseSig(dytfCadr(l));
		u=dytfSymbol(s);

		if(dytfNullP(u) && !ctx->cs_fl)
		{
			if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
				else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIndexLit(ctx, n);
			return;
		}

		i=BSVMC_IndexLit(ctx, n);
		j=BSVMC_IndexLit(ctx, u);
		k=BSVMC_IndexLit(ctx, dytfWrap(ctx->cs_fl));
		if(ctx->lvl)
			{ BSVMC_EmitOp(ctx, BSVM_SOP_TYLEXBIND); }
		else
			{ BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND); }
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		BSVMC_EncIDX(ctx, k);
#endif

		return;
	}

#if 0
	if(dytfFormIsP(l, "defun"))
	{
//		t=CONS2S("lambda", dytfCadddr(l), dytfCddddr(l));
		t=dytfCons3s("rlambda", dytfList2(dytfCadr(l), dytfCaddr(l)),
			dytfCadddr(l), dytfCddddr(l));

//		BSVM_TY_PrintLN(t);

		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		i=BSVMC_IndexLit(ctx, dytfCaddr(l));
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dytfFormIsP(l, "defvar"))
	{
		if(!dytfConsP(dytfCadr(l)))
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
			i=BSVMC_IndexLit(ctx, dytfCadr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		t=dytfCons(dytfSymbol("lambda"), dytfCons(dytfCdadr(l), dytfCddr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		i=BSVMC_IndexLit(ctx, dytfCaadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}
#endif

	if(dytfFormIsP(l, "vars"))
	{
//		c=dytfCaddr(l);

		c=dytfCadr(l);
		while(dytfConsP(c))
		{
			n=dytfCar(c);
			v=DYTF_NULL; t=DYTF_NULL;

			BSVMC_CompileEmitVar(ctx, n);

#if 0
			if(dytfFormIsP(n, "set!"))
			{
				v=dytfCaddr(n);
				n=dytfCadr(n);				
			}

			if(dytfFormIsP(n, "cast"))
			{
				t=dytfCadr(n);
				n=dytfCaddr(n);				
			}

//			dyPrintf("vars: %p %p %p\n", n, t, v);

#if 0
			BSVMC_CompileExpr(ctx, v);
			i=BSVMC_IndexLit(ctx, n);
			if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
				else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIDX(ctx, i);
#endif

#if 1
			BSVMC_CompileExpr(ctx, v);

			s=BSVMC_InferTypeBaseSig(t);
			u=dytfSymbol(s);

			if(!u && !ctx->cs_fl)
			{
				if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
					else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
				BSVMC_EncIndexLit(ctx, n);

				c=dytfCdr(c);
				continue;
			}

			li=dyllDecodeFlagSig(dytfSymbolv(ctx->cs_fl));

//			i=BSVMC_IndexLit(ctx, n);
//			j=BSVMC_IndexLit(ctx, u);
//			k=BSVMC_IndexLit(ctx, ctx->cs_fl);
			if(li&BGBDYC_FLAG_DYNAMIC)
				{ BSVMC_EmitOp(ctx, BSVM_SOP_TYDYNBIND); }
			else if(ctx->lvl)
				{ BSVMC_EmitOp(ctx, BSVM_SOP_TYLEXBIND);}
			else
				{ BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND); }
//			BSVMC_EncIDX(ctx, i);
//			BSVMC_EncIDX(ctx, j);
//			BSVMC_EncIDX(ctx, k);
			BSVMC_EncIndexLit(ctx, n);
			BSVMC_EncIndexLit(ctx, u);
			BSVMC_EncIndexLit(ctx, ctx->cs_fl);
#endif

#if 0
			if(dytfConsP(dytfCar(c)))
			{
				BSVMC_CompileExpr(ctx, dytfCadar(c));
				i=BSVMC_IndexLit(ctx, dytfCaar(c));
				if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
					else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
				BSVMC_EncIDX(ctx, i);
			}else
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
				i=BSVMC_IndexLit(ctx, dytfCar(c));
				if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
					else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
				BSVMC_EncIDX(ctx, i);
			}
#endif
#endif

			c=dytfCdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
		return;
	}

	if(dytfFormIsP(l, "lambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		n=DYTF_NULL;
		v=dytfCddr(l);

		if(dytfFormIsP(v, "cast"))
		{
			n=dytfList3s("cast", dytfCadr(v), n);
			v=dytfCaddr(v);
		}

		t=BSVMC_CompileBlock(ctx, n, dytfCadr(l), v);

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		if(ctx->cap)BSVMC_EmitOp(ctx, BSVM_SOP_CLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dytfFormIsP(l, "rlambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		n=dytfCadr(l);
		v=dytfCdddr(l);
		
		if(dytfFormIsP(v, "cast"))
		{
			n=dytfList3s("cast", dytfCadr(v), n);
			v=dytfCaddr(v);
		}

		t=BSVMC_CompileBlock(ctx, n, dytfCaddr(l), v);

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		if(ctx->cap)BSVMC_EmitOp(ctx, BSVM_SOP_CLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dytfFormIsP(l, "elambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		u=dytfCadr(l);
		n=DYTF_NULL;
		v=dytfCdddr(l);

		if(dytfFormIsP(v, "cast"))
		{
			n=dytfList3s("cast", dytfCadr(v), n);
			v=dytfCaddr(v);
		}

		t=BSVMC_CompileBlock(ctx, n, dytfCaddr(l), v);


		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		c=u;
		while(dytfConsP(c))
		{
			v=dytfCar(c);
			if(dytfFormIsP(v, "byref"))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, dytfCadr(v));
			}

			if(dytfFormIsP(v, "byval"))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(v));
			}
			
			if(dytfSymbolP(v))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, v);
//				BSVMC_CompileExpr(ctx, v);
			}
			
			c=dytfCdr(c);
		}

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		BSVMC_EmitOp(ctx, BSVM_SOP_ENVCLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dytfFormIsP(l, "erlambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		u=dytfCadr(l);
		n=dytfCaddr(l);
		v=dytfCddddr(l);
		
		if(dytfFormIsP(v, "cast"))
		{
			n=dytfList3s("cast", dytfCadr(v), n);
			v=dytfCaddr(v);
		}

		t=BSVMC_CompileBlockEnv(ctx, u, n, dytfCadddr(l), v);

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		c=u;
		while(dytfConsP(c))
		{
			v=dytfCar(c);
			if(dytfFormIsP(v, "byref"))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, dytfCadr(v));
			}

			if(dytfFormIsP(v, "byval"))
			{
				BSVMC_CompileExpr(ctx, dytfCadr(v));
			}
			
			if(dytfSymbolP(v))
			{
//				BSVMC_CompileExpr(ctx, v);
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, v);
			}
			
			c=dytfCdr(c);
		}

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		BSVMC_EmitOp(ctx, BSVM_SOP_ENVCLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dytfFormIsP(l, "attr"))
	{
		t=dytfCadr(l);
		if(dytfFormIsP(t, "quote"))
			t=dytfKeyword(dytfSymbolv(dytfCadr(t)));

		BSVMC_CompileExpr(ctx, dytfCaddr(l));
//		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_ATTR);
		
		return;
	}

	if(dytfFormIsP(l, "dict"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		i=BSVMC_IndexLit(ctx, dytfKeyword("parent"));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF);

		j=ctx->lvl;
		ctx->lvl=0;
		ctx->olvl++;

		c=dytfCdr(l);
		while(dytfConsP(c))
		{
			t=dytfCar(c);

			if(dytfFormIsP(t, "attr") || dytfFormIsP(t, "set!"))
			{
				v=dytfCadr(t);
				if(dytfFormIsP(v, "quote"))
					v=dytfKeyword(dytfSymbolv(dytfCadr(v)));
				i=BSVMC_IndexLit(ctx, v);
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
				BSVMC_EncIDX(ctx, i);
				BSVMC_CompileExpr(ctx, dytfCaddr(t));
			}

			if(dytfFormIsP(t, "define"))
			{
				i=BSVMC_IndexLit(ctx, dytfCaadr(t));
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
				BSVMC_EncIDX(ctx, i);
				u=dytfCons2s("lambda", dytfCdadr(t), dytfCddr(t));
				BSVMC_CompileExpr(ctx, u);
			}

			c=dytfCdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

		ctx->olvl--;
		ctx->lvl=j;
		return;
	}

#if 0
	if(dytfFormIsP(l, "new"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		i=BSVMC_IndexLit(ctx, dytfKeyword("parent"));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF);
		BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
		BSVMC_CompileStatement(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);

		return;
	}
#endif

#if 1
	if(dytfFormIsP(l, "new"))
	{
		BSVMC_CompileNew(ctx, dytfCadr(l));
		return;
	}
#endif

	if(dytfFormIsP(l, "delete"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DELETE);
		return;
	}

	if(dytfFormIsP(l, "instanceof"))
	{
		BSVMC_CompileInstanceof(ctx, l);
		return;
	}

	if(dytfFormIsP(l, "with"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);

		return;
	}

	if(dytfFormIsP(l, "with_top"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_TOP);
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_TOP);

		return;
	}

	if(dytfFormIsP(l, "class") ||
		dytfFormIsP(l, "interface") ||
		dytfFormIsP(l, "struct"))
//	if(dytfFormIsP(l, "class"))
	{
		BSVMC_CompileClass(ctx, l);
		return;
	}

	if(dytfFormIsP(l, "package"))
	{
		BSVMC_CompilePackage(ctx, l);
		return;
	}

	if((dytfFormIsP(l, "list")) || (dytfFormIsP(l, "array")) ||
		(dytfFormIsP(l, "vector")))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCdr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}
		if(dytfFormIsP(l, "list"))
		{
			BSVMC_CompileExpr(ctx, c);
			BSVMC_EmitOp(ctx, BSVM_SOP_LIST);
		}
		if(dytfFormIsP(l, "array"))BSVMC_EmitOp(ctx, BSVM_SOP_ARRAY);
		if(dytfFormIsP(l, "vector"))BSVMC_EmitOp(ctx, BSVM_SOP_VECTOR);
		return;
	}

	if(dytfFormIsP(l, "quat"))
	{
		c=dytfCdr(l); i=4;
		while(dytfConsP(c) && i)
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c); i--;
		}
		while(i--)
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_0);
		BSVMC_EmitOp(ctx, BSVM_SOP_QUAT);
		return;
	}

	if(dytfFormIsP(l, "array_ty"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCddr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}

		s=BSVMC_InferTypeBaseSig(dytfCadr(l));
		t=dytfSymbol(s);
		i=BSVMC_IndexLit(ctx, t);

		BSVMC_EmitOp(ctx, BSVM_SOP_ARRAY_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dytfFormIsP(l, "matrix"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCdr(l);
		while(dytfConsP(c))
		{
			if(dytfConsP(dytfCar(c)))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
				d=dytfCar(c);
				while(dytfConsP(d))
				{
					BSVMC_CompileExpr(ctx, dytfCar(d));
					d=dytfCdr(d);
				}
				BSVMC_EmitOp(ctx, BSVM_SOP_VECTOR);
			}else BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}

		BSVMC_EmitOp(ctx, BSVM_SOP_MATRIX);
		return;
	}

	if(dytfFormIsP(l, "unary"))
	{
		BSVMC_CompileUnaryExpr(ctx, l);
		return;
	}

	if(BSVMC_FormIs(l, "binary"))
	{
		BSVMC_CompileBinaryExpr(ctx, l);
		return;
	}

	if(dytfFormIsP(l, "funcall"))
	{
		if(dytfSymbolP(dytfCadr(l)))
		{
			i=BSVMC_CompileSpecial(ctx, dytfCdr(l));
			if(i>0)return;
		}

		s=BSVMC_InferCallArgsBaseSig(ctx, dytfCdr(l), DYTF_NULL);
		BSVMC_EmitOp(ctx, BSVM_SOP_TYMARK);
		BSVMC_EncIndexLit(ctx, dytfSymbol(s));

//		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCddr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}

		if(dytfEqP(dytfCadr(l), dytfWrap(ctx->cf_n)))
		{
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_CF);
				else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_CF);
			return;
		}

		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
//		if(dytfSymbolP(dytfCadr(l)))
		if(dytfSymbolP(t))
		{
			i=BSVMC_IndexLit(ctx, t);
//			i=BSVMC_IndexLit(ctx, dytfCadr(l));
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_S);
				else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_S);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		BSVMC_CompileExpr(ctx, t);
//		BSVMC_CompileExpr(ctx, dytfCadr(l));
		if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL);
			else BSVMC_EmitOp(ctx, BSVM_SOP_CALL);
		return;
	}

	if(dytfFormIsP(l, "methodcall"))
	{
		i=BSVMC_CompileSpecialMethod(ctx, dytfCdr(l));
		if(i>0)return;

		s=BSVMC_InferMethodCallArgsBaseSig(ctx, dytfCdr(l), DYTF_NULL);
		BSVMC_EmitOp(ctx, BSVM_SOP_TYMARK);
		BSVMC_EncIndexLit(ctx, dytfSymbol(s));

//		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dytfCdddr(l);
		while(dytfConsP(c))
		{
			BSVMC_CompileExpr(ctx, dytfCar(c));
			c=dytfCdr(c);
		}

		BSVMC_CompileExpr(ctx, dytfCadr(l));

//		t=dytfCaddr(l);
		t=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
		if(dytfSymbolP(t))
		{
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_METHODTAILCALL_S);
				else BSVMC_EmitOp(ctx, BSVM_SOP_METHODCALL_S);
			BSVMC_EncIndexLit(ctx, t);
			return;
		}

//		i=BSVMC_IndexLit(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIndexLit(ctx, t);
//		BSVMC_EncIDX(ctx, i);

		if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_METHODTAILCALL);
			else BSVMC_EmitOp(ctx, BSVM_SOP_METHODCALL);
		return;
	}

	if(dytfFormIsP(l, "complex"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX_I);
		return;
	}

	if(dytfFormIsP(l, "inline_c"))
	{
		t=dytfCadr(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_CGENAST);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	dyPrintf("unhandled expr %s\n", dytfSymbolv(dytfCar(l)));
	BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);

#if 0
	BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
	c=dytfCdr(l);
	while(dytfConsP(c))
	{
		BSVMC_CompileExpr(ctx, dytfCar(c));
		c=dytfCdr(c);
	}
	BSVMC_CompileExpr(ctx, dytfCar(l));
	if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL);
		else BSVMC_EmitOp(ctx, BSVM_SOP_CALL);
#endif
}

void BSVMC_CompileExprT(BSVM_SVMCState *ctx, dytf l)
{
	float f, g;
	dytf t, c, ct, cv;
	int i, j;

	t=l;
	l=BSVMC_ReduceExpr(ctx, l);

	if(BSVMC_DebugP(ctx))
	{
		dyPrintf("expr: ");
		dyPrint(dytfUnwrap(t));
		dyPrintf(" -> ");
		dyPrint(dytfUnwrap(l));
		dyPrintf("\n");
	}

	if(dytfConsP(l))
	{
		BSVMC_CompileForm(ctx, l);
		return;
	}

	if(dytfSymbolP(l))
	{
#if 0
		i=BSVMC_RegIdx(l);
		if(i>=0)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_R);
			BSVMC_EncByte(ctx, i);
			return;
		}
#endif

		//take note of any captured variables
		c=dytfWrap(ctx->lenv);
		ct=dytfWrap(ctx->tenv);
		cv=dytfWrap(ctx->venv); i=0; j=0;
		while(dytfConsP(c))
		{
			if(dytfEqP(c, dytfWrap(ctx->llenv)))i=1;
			if(dytfEqP(dytfCar(c), l))break;
			c=dytfCdr(c); ct=dytfCdr(ct); cv=dytfCdr(cv);
			j++;
		}
		if(dytfConsP(c) && i)ctx->cap++;

		if(!dytfConsP(c))
		{
//			if((l==dytfSymbol("self")) || (l==dytfSymbol("this")))
			if(dytfEqP(l, dytfSymbol("this")))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF); return; }
			if(dytfEqP(l, dytfSymbol("true")))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_TRUE); return; }
			if(dytfEqP(l, dytfSymbol("false")))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_FALSE); return; }
			if(dytfEqP(l, dytfSymbol("null")))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL); return; }
		}

		if((dytfEqP(l, dytfWrap(ctx->cf_n))) && (!dytfConsP(c) || i))
			{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_CF); return; }

#if 0
		if(dytfConsP(c) && i)dyPrintf("CCAP: %s\n",
			BSVM_TY_GetSymbol(l));
		if(dytfConsP(c) && !i)dyPrintf("LOC: %s\n",
			BSVM_TY_GetSymbol(l));
		if(!dytfConsP(c))dyPrintf("OBJ: %s\n",
			BSVM_TY_GetSymbol(l));
#endif

		if(dytfConsP(c))
		{
			t=dytfCar(ct);

//			*(int *)-1=-1;

#ifdef BSVM_USE_BVT
			i=-1;
			if(BSVMC_TypeSmallIntP(ctx, t))
				i=BSVM_SOP_LLOAD_XI;
			if(BSVMC_TypeLongP(ctx, t))
				i=BSVM_SOP_LLOAD_XL;
			if(BSVMC_TypeFloatP(ctx, t))
				i=BSVM_SOP_LLOAD_XF;
			if(BSVMC_TypeDoubleP(ctx, t))
				i=BSVM_SOP_LLOAD_XD;

			if(i>=0)
			{
				BSVMC_EmitOp(ctx, i);
				BSVMC_EncByte(ctx, j);
				return;
			}
#endif

//			if((t==dytfKeyword("int")) || (t==dytfKeyword("float")))
			if(BSVMC_TypeSmallFixRealP(ctx, t))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LLOAD_F);
				BSVMC_EncByte(ctx, j);
				return;
			}

			BSVMC_EmitOp(ctx, BSVM_SOP_LLOAD);
			BSVMC_EncByte(ctx, j);
			return;
		}

		if(ctx->pcap)ctx->cap++;		//capture loads if package

		i=BSVMC_IndexLit(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_LOAD);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dytfEqP(l, DYTF_NULL))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL); }
	else if(dytfEqP(l, DYTF_TRUE))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_TRUE); }
	else if(dytfEqP(l, DYTF_FALSE))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_FALSE); }
	else if(dytfEqP(l, dytfInt(0)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_0); }
	else if(dytfEqP(l, dytfInt(1)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_1); }
	else if(dytfEqP(l, dytfInt(2)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_2); }
	else if(dytfEqP(l, dytfInt(3)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_3); }
	else if(dytfEqP(l, dytfInt(4)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_4); }
	else if(dytfEqP(l, dytfInt(5)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_5); }
	else if(dytfEqP(l, dytfInt(6)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_6); }
	else if(dytfEqP(l, dytfInt(7)))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_7); }
	else if(dytfCharP(l))
	{
		i=dytfIntv(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SV_C);
		BSVMC_EncWord(ctx, i);
	}
	else if(dytfIntP(l))
	{
		i=dytfIntv(l);

		if((i>=0) && (i<=255))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SV_P);
			BSVMC_EncByte(ctx, i);
			return;
		}
		if((i<=0) && (i>=-255))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SV_N);
			BSVMC_EncByte(ctx, -i);
			return;
		}
		if((i>=-32768) && (i<=32767))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SV);
			BSVMC_EncWord(ctx, i);
			return;
		}

		i=BSVMC_IndexLit2(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		return;
	}else if(dytfFloatP(l))
	{
		f=dytfFloatv(l);
//		i=(int)(floor(f*100+0.5));
//		i=(int)(f*100);
//		i=(int)((f>=0)?floor(f*100+0.5):floor(f*100-0.5));

		i=(int)(floor(fabs(f)*100+0.5));
		if(f<0)i=-i;

//		dyPrintf("%f %f %f\n", f, i/100.0, f-(i/100.0));
//		if((-32768<=i) && (i<=32767) && ( (f==(i/100.0)) ||
//			((fabs(f-(i/100.0))<=0.0001) && ((i>100) || (i<-100))) ))
		if((-32768<=i) && (i<=32767) && (fabs(f-(i/100.0))<=fabs(f*0.0000001)) )
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SV_F);
			BSVMC_EncWord(ctx, i);
			return;
		}

#if 1
		i=(int)(floor(fabs(f)*10000+0.5));
		if(f<0)i=-i;
		if((-32768<=i) && (i<=32767) &&
			(fabs(f-(i/10000.0))<=fabs(f*0.0000001)) )
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SV_FB);
			BSVMC_EncWord(ctx, i);
			return;
		}
#endif

		i=BSVMC_IndexLit2(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		return;
	}else if(dycomplexp(dytfUnwrap(l)))
	{
		f=dycreal(dytfUnwrap(l));
		g=dycimag(dytfUnwrap(l));
		
		if(f==0)
		{
			if(g==((int)g))
			{
				BSVMC_CompileExpr(ctx, dytfInt(g));
				BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX_I);
				return;
			}

			BSVMC_CompileExpr(ctx, dytfFloat(g));
			BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX_I);
			return;
		}

		BSVMC_CompileExpr(ctx, (f==((int)f))?dytfInt(f):dytfFloat(f));
		BSVMC_CompileExpr(ctx, (g==((int)g))?dytfInt(g):dytfFloat(g));
		BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX);
		return;
	}else if(dyquatp(dytfUnwrap(l)))
	{
		f=qw(dyquatv(dytfUnwrap(l)));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dytfInt(f):dytfFloat(f));
		f=qx(dyquatv(dytfUnwrap(l)));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dytfInt(f):dytfFloat(f));
		f=qy(dyquatv(dytfUnwrap(l)));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dytfInt(f):dytfFloat(f));
		f=qz(dyquatv(dytfUnwrap(l)));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dytfInt(f):dytfFloat(f));
		BSVMC_EmitOp(ctx, BSVM_SOP_QUAT);
		return;
	}else
	{
		i=BSVMC_IndexLit(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
	}
}

void BSVMC_CompileExpr(BSVM_SVMCState *ctx, dytf l)
{
	int i;

	i=ctx->tail;
	ctx->tail=0;
	BSVMC_CompileExprT(ctx, l);
	ctx->tail=i;
}

void BSVMC_CompileExprTail(BSVM_SVMCState *ctx, dytf l)
{
	int i;

	i=ctx->tail;
	ctx->tail=1;
	BSVMC_CompileExprT(ctx, l);
	ctx->tail=i;
}
