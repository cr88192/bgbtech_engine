#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbsvm.h>

// #define BSVM_USE_BVT

int BSVMC_CompileSpecial(BSVM_SVMCState *ctx, dyt l)
{
	dyt c, t, u, v;
	char *s0, *s1, *s2, *s3;
	int i, j, na;

	na=dyListLength(dycdr(l));

	if((na==1) && dyFormIsP(l, "infer"))
	{
		t=BSVMC_InferExpr(ctx, dycadr(l));
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		return(1);
	}

#if 1
	if((na==2) && dyFormIsP(l, "atan2"))
	{
		u=BSVMC_ReduceExpr(ctx, dycadr(l));
		v=BSVMC_ReduceExpr(ctx, dycaddr(l));
		BSVMC_CompileExpr(ctx, u);
		BSVMC_CompileExpr(ctx, v);

//		BSVMC_CompileExpr(ctx, dycadr(l));
//		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_CompileInferBinaryTypePrefix(ctx, u, v);
		BSVMC_EmitOp(ctx, BSVM_SOP_BINARYOP);
		BSVMC_EncIDX(ctx, BSVM_OPR_ATAN2);
		return(1);
	}

	i=-1;
	if((na==2) && dyFormIsP(l, "pow"))i=1;
	if((na==2) && dyFormIsP(l, "spow"))i=1;
	if(i>0)
	{
		u=BSVMC_ReduceExpr(ctx, dycadr(l));
		v=BSVMC_ReduceExpr(ctx, dycaddr(l));
		BSVMC_CompileExpr(ctx, u);
		BSVMC_CompileExpr(ctx, v);
		BSVMC_CompileInferBinaryTypePrefix(ctx, u, v);
		BSVMC_EmitOp(ctx, BSVM_SOP_BINARYINTRIN_S);
		BSVMC_EncIndexLit(ctx, dycar(l));
		return(1);
	}

	i=-1;
	if((na==1) && dyFormIsP(l, "sin"))i=BSVM_OPR_SIN;
	if((na==1) && dyFormIsP(l, "cos"))i=BSVM_OPR_COS;
	if((na==1) && dyFormIsP(l, "tan"))i=BSVM_OPR_TAN;
	if((na==1) && dyFormIsP(l, "asin"))i=BSVM_OPR_ASIN;
	if((na==1) && dyFormIsP(l, "acos"))i=BSVM_OPR_ACOS;
	if((na==1) && dyFormIsP(l, "atan"))i=BSVM_OPR_ATAN;
	if((na==1) && dyFormIsP(l, "sqr"))i=BSVM_OPR_SQR;
	if((na==1) && dyFormIsP(l, "sqrt"))i=BSVM_OPR_SQRT;
	if(i>0)
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		BSVMC_CompileExpr(ctx, t);

//		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_CompileInferUnaryTypePrefix(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
		BSVMC_EncIDX(ctx, i);
		return(1);
	}

	i=-1;
	if((na==1) && dyFormIsP(l, "ssqr"))i=1;
	if((na==1) && dyFormIsP(l, "ssqrt"))i=1;
	if(i>0)
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_CompileInferUnaryTypePrefix(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYINTRIN_S);
		BSVMC_EncIndexLit(ctx, dycar(l));
		return(1);
	}

#endif

	if((na==2) && dyFormIsP(l, "cons"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CONS);
		return(1);
	}

	if((na==1) && dyFormIsP(l, "clone"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CLONE);
		return(1);
	}

	if((na==1) && dyFormIsP(l, "assert"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_ASSERT);
		return(1);
	}

	if(dyFormIsP(l, "join"))
	{
		if(dycddr(l))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
			c=dycdr(l);
			while(dyconsp(c))
			{
				BSVMC_CompileExpr(ctx, dycar(c));
				c=dycdr(c);
			}
			BSVMC_EmitOp(ctx, BSVM_SOP_MULTIJOIN);
			return(1);
		}
	
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_JOIN);
		return(1);
	}

	if((na==1) && dyFormIsP(l, "sleep"))
	{
		v=dycadr(l);
		if(dyintp(v))
		{
			i=dyintv(dycadr(l));
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

	if(dyFormIsP(l, "this"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycdr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_THIS);
		return(1);
	}

	if(dyFormIsP(l, "super"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycdr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_SUPER);
		return(1);
	}

	s0=dysymbolv(dycar(l));
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

	BSVMC_CompileExpr(ctx, dycadr(l));

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

int BSVMC_CompileSpecialMethod(BSVM_SVMCState *ctx, dyt l)
{
	if(dycadr(l)==dysymbol("toString"))
	{
		BSVMC_CompileExpr(ctx, dycar(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_TOSTRING);
		return(1);
	}

	if(dycadr(l)==dysymbol("clone"))
	{
		BSVMC_CompileExpr(ctx, dycar(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CLONE);
		return(1);
	}

	return(0);
}

int BSVMC_CheckLexical(BSVM_SVMCState *ctx, dyt l)
{
	dyt c, t;
	int i;

	if(!dysymbolp(l))return(-1);

	c=ctx->lenv; i=0;
	while(dyconsp(c))
	{
		if(dycar(c)==l)break;
		c=dycdr(c);
		i++;
	}

	if(dyconsp(c))return(i);
	return(-1);
}

void BSVMC_CompileExprCast(BSVM_SVMCState *ctx, dyt l, dyt ty)
{
	BSVMC_CompileExprCastFl(ctx, l, ty, 0);
}

void BSVMC_CompileExprCastFl(BSVM_SVMCState *ctx,
	dyt l, dyt ty, int flags)
{
	dyt n, t, u, c, ty2;
	char *s;
	int i, j;

	n=BSVMC_ReduceExpr(ctx, l);
	ty2=BSVMC_InferExpr(ctx, n);

	if(ty2==ty)
	{
		BSVMC_CompileExpr(ctx, n);
		return;
	}

	if(!n)
	{
		if(BSVMC_FormIs(ty, "emptyindex"))
		{
			BSVMC_CompileExpr(ctx, n);
			return;
		}

		if(BSVMC_FormIs(ty, "getindex"))
		{
			BSVMC_CompileExpr(ctx, dycaddr(ty));

			s=BSVMC_InferTypeBaseSig(dycadr(ty));
			t=dysymbol(s);

			i=BSVMC_IndexLit(ctx, t);
			BSVMC_EmitOp(ctx, BSVM_SOP_NEWARRAY_S);
			BSVMC_EncIDX(ctx, i);
			return;
		}
	}

#if 1
	if(dyFormIsP(l, "funcall") && dysymbolp(dycadr(l)))
	{
		if(dysymbolp(dycadr(l)))
		{
			i=BSVMC_CompileSpecial(ctx, dycdr(l));
			if(i>0)return;
		}

		s=BSVMC_InferCallArgsBaseSig(ctx, dycdr(l), ty);
		BSVMC_EmitOp(ctx, BSVM_SOP_TYMARK);
		BSVMC_EncIndexLit(ctx, dysymbol(s));

		c=dycddr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}

		if(dycadr(l)==ctx->cf_n)
		{
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_CF);
				else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_CF);
			return;
		}

		if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_S);
			else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_S);
		BSVMC_EncIndexLit(ctx, dycadr(l));
		return;
	}
#endif

#ifdef BSVM_USE_BVT
	if(BSVMC_TypeSmallIntP(ctx, ty))
	{
		if(dyintp(n) || !n)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_XI);
			BSVMC_EncSVLI(ctx, dyintv(n));
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
		if(dylongp(n) || !n)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_XL);
			BSVMC_EncSVLI(ctx, dylongv(n));
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

void BSVMC_CompileAssign(BSVM_SVMCState *ctx, dyt l)
{
	dyt c, ct, cv, t, u;
	int i, j;

	if(dyFormIsP(l, "objref"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_S);
		BSVMC_EncIndexLit(ctx, dycaddr(l));
		return;
	}

	if(dyFormIsP(l, "getindex"))
	{
		if(dyintp(dycaddr(l)))
		{
			i=dyintv(dycaddr(l));
			if((i>=0) && (i<4))
			{
				BSVMC_CompileExpr(ctx, dycadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_0+i);
				return;
			}
			if((i>=0) && (i<255))
			{
				BSVMC_CompileExpr(ctx, dycadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_I);
				BSVMC_EncByte(ctx, i);
				return;
			}
		}

		BSVMC_CompileExpr(ctx, dycadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_CompileExpr(ctx, dycaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
		return;
	}

	if(BSVMC_FormIsUnary(l, "*"))
	{
		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_SETDEFER);
		return;
	}

	if(BSVMC_FormIsUnary(l, "&"))
	{
		BSVMC_Error(ctx,
			"BSVMC_CompileAssign: Attempted assign to a reference");
//		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
//		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dysymbolp(l))
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

		c=ctx->lenv; ct=ctx->tenv; cv=ctx->venv;
		i=0; j=0;
		while(dyconsp(c))
		{
			if(c==ctx->llenv)i=1;
			if(dycar(c)==l)break;
			c=dycdr(c); ct=dycdr(ct); cv=dycdr(cv);
			j++;
		}
		if(dyconsp(c) && i)ctx->cap++;

		if(dyconsp(c))
		{
			t=dycar(ct);
			dysetcar(cv, NULL);

			if(t!=NULL)
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

//			if((t==dykeyword("int")) || (t==dykeyword("float")))
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

void BSVMC_CompileNew(BSVM_SVMCState *ctx, dyt l)
{
	byte *ips[64];
	byte *ip0, *ip1, *ip2;
	char *s;
	dyt c, d, t, u, v, n;
	int i, j;
	
	if(dyFormIsP(l, "funcall"))
	{
#if 0
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		i=BSVMC_IndexLit(ctx, dykeyword("parent"));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF);
		BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
		BSVMC_CompileStatement(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
#endif

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycddr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}

		s=BSVMC_FlattenQNameBaseSig(dycadr(l));
		if(s)
		{
			t=dysymbol(s);
			i=BSVMC_IndexLit(ctx, t);
			BSVMC_EmitOp(ctx, BSVM_SOP_NEW_S);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_NEW);
		return;
	}

	if(dyFormIsP(l, "getindex"))
	{
		BSVMC_CompileExpr(ctx, dycaddr(l));

		s=BSVMC_InferTypeBaseSig(dycadr(l));
		t=dysymbol(s);

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_NEWARRAY_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}
	
	s=BSVMC_FlattenQNameBaseSig(l);
	if(s)
	{
		t=dysymbol(s);
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_NEW_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dysymbolp(l))
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

void BSVMC_CompileInstanceof(BSVM_SVMCState *ctx, dyt l)
{
	char *s;
	dyt t;
	int i;

	BSVMC_CompileExpr(ctx, dycadr(l));

	s=BSVMC_InferTypeBaseSig(dycaddr(l));
	if(s)
	{
		t=dysymbol(s);
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_INSTANCEOF_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	BSVMC_CompileExpr(ctx, dycaddr(l));
	BSVMC_EmitOp(ctx, BSVM_SOP_INSTANCEOF);
	return;
}

void BSVMC_CompileForm(BSVM_SVMCState *ctx, dyt l)
{
	char tb[256];
	byte *ips[64];
	byte *ip0, *ip1, *ip2;
	dyt c, d, t, u, v, n;
	char *s;
	s64 li;
	int i, j, k;

	if(dykeywordp(dycar(l)))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		i=BSVMC_IndexLit(ctx, dycar(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	l=BSVMC_ProcessFlagsExpr(ctx, l, tb);
	if(tb[0])ctx->cs_fl=dysymbol(tb);
		else ctx->cs_fl=NULL;

	if(dyFormIsP(l, "goto"))
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
			dysymbolv(dycadr(l)), ctx->ip);
		return;
#endif
	}

	if(dyFormIsP(l, "label"))
	{
		if(!ctx->tail)
		{
			dyPrintf("invalid use of label\n");
			return;
		}

		BSVMC_EmitLabel(ctx, dysymbolv(dycadr(l)), ctx->ip);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
		return;
	}

	if(	(dyFormIsP(l, "for")) || (dyFormIsP(l, "while")) ||
		(dyFormIsP(l, "for_in")) || (dyFormIsP(l, "do_while")))
	{
		BSVMC_CompileStatement(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
		return;
	}


	if(dyFormIsP(l, "import"))
	{
		i=BSVMC_IndexLit(ctx, dycadr(l));
		j=BSVMC_IndexLit(ctx, ctx->cs_fl);
		BSVMC_EmitOp(ctx, BSVM_SOP_IMPORT);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		ctx->pcap++;

		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);

		return;
	}

	if(dyFormIsP(l, "quote"))
	{
		i=BSVMC_IndexLit(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dyFormIsP(l, "unquote"))
	{
//		i=BSVMC_IndexLit(ctx, dycadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//		BSVMC_EncIDX(ctx, i);

		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_UNQUOTE);
		return;
	}

	if(dyFormIsP(l, "return"))
	{
//		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_CompileExprCast(ctx, dycadr(l), ctx->cf_ty);
		if(!ctx->tail)
		{
			if(!(ctx->i_cap))
				BSVMC_EmitOp(ctx, BSVM_SOP_CLEARENV);
			BSVMC_EmitOp(ctx, BSVM_SOP_RET);
		}
		return;
	}

	if(dyFormIsP(l, "cast"))
	{
//		t=dysymbol(s);

		BSVMC_CompileExpr(ctx, dycaddr(l));

		t=BSVMC_InferArgsBaseSig2(dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_TRYCAST_S);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	if(dyFormIsP(l, "cast_force"))
	{
//		t=dysymbol(s);

		BSVMC_CompileExpr(ctx, dycaddr(l));

		t=BSVMC_InferArgsBaseSig2(dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	if(dyFormIsP(l, "sizeof"))
	{
		t=BSVMC_InferArgsBaseSig2(dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_SIZEOF_S);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	if(dyFormIsP(l, "set!") || dyFormIsP(l, "setexpr!"))
	{
		BSVMC_CompileExpr(ctx, dycaddr(l));

#if 0
		i=BSVMC_RegIdx(dycadr(l));
		if(i>=0)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_R);
			BSVMC_EncByte(ctx, i);
			return;
		}
#endif

		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		BSVMC_CompileAssign(ctx, dycadr(l));
		return;
	}

	if((dyFormIsP(l, "setindex!")) || (dyFormIsP(l, "vector-set!")))
	{
		t=dycadddr(l);
		u=BSVMC_InferExpr(ctx, t);
		BSVMC_CompileExpr(ctx, t);
//		if((u==dykeyword("int")) || (u==dykeyword("float")))
		if(BSVMC_TypeSmallFixRealP(ctx, u))
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
			else BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		if(dyintp(dycaddr(l)))
		{
			i=dyintv(dycaddr(l));
			if((i>=0) && (i<4))
			{
				BSVMC_CompileExpr(ctx, dycadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_0+i);
				return;
			}
			if((i>=0) && (i<255))
			{
				BSVMC_CompileExpr(ctx, dycadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_I);
				BSVMC_EncByte(ctx, i);
				return;
			}
		}

//		BSVMC_CompileExpr(ctx, dycadddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		BSVMC_CompileExpr(ctx, dycadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_CompileExpr(ctx, dycaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
		return;
	}

	if((dyFormIsP(l, "getindex")) || (dyFormIsP(l, "vector-ref")))
	{
		BSVMC_CompileGetIndex(ctx, l);
		return;
		
#if 0
		if(dyintp(dycaddr(l)))
		{
			i=dyintv(dycaddr(l));
			if((i>=0) && (i<4))
			{
				BSVMC_CompileExpr(ctx, dycadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_0+i);
				return;
			}
			if((i>=0) && (i<255))
			{
				BSVMC_CompileExpr(ctx, dycadr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_I);
				BSVMC_EncByte(ctx, i);
				return;
			}
		}

		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX);
		return;
#endif
	}

	if(dyFormIsP(l, "objref"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
//		i=BSVMC_IndexLit(ctx, dycaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//		BSVMC_EncIDX(ctx, i);
//		BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX);

		i=BSVMC_IndexLit(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dyFormIsP(l, "objset!"))
	{
//		BSVMC_CompileExpr(ctx, dycadddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
//
//		BSVMC_CompileExpr(ctx, dycadr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
//
//		i=BSVMC_IndexLit(ctx, dycaddr(l));
//		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//		BSVMC_EncIDX(ctx, i);
//		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
//		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);

		t=dycadddr(l);
		u=BSVMC_InferExpr(ctx, t);
		BSVMC_CompileExpr(ctx, t);
//		if((u==dykeyword("int")) || (u==dykeyword("float")))
		if(BSVMC_TypeSmallFixRealP(ctx, u))
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
			else BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		BSVMC_CompileExpr(ctx, dycadr(l));
		i=BSVMC_IndexLit(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(BSVMC_FormIs(l, "preinc!") || BSVMC_FormIs(l, "predec!"))
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		u=BSVMC_InferExpr(ctx, t);
		if(BSVMC_FormIs(l, "preinc!"))i=BSVM_OPR_INC;
			else i=BSVM_OPR_DEC;

		if(dysymbolp(t))
		{
//			if(u==dykeyword("int"))
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
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		u=BSVMC_InferExpr(ctx, t);
		if(BSVMC_FormIs(l, "postinc!"))i=BSVM_OPR_INC;
			else i=BSVM_OPR_DEC;

		if(dysymbolp(t))
		{
//			if(u==dykeyword("int"))
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


	if(dyFormIsP(l, "if"))
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));

#if 0
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			BSVMC_CompileExpr(ctx, dycaddr(l));
			return;
		}
		if(i==0)
		{
			if(dyconsp(dycdddr(l)))
				BSVMC_CompileExpr(ctx, dycadddr(l));
			return;
		}
#endif

#if 0
		if(BSVMC_FormIs(t, "unary") && (dycadr(t)==dysymbol("!")))
		{
			BSVMC_CompileExpr(ctx, dycaddr(t));
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

//		BSVMC_CompileExprT(ctx, dycaddr(l));

		if(ctx->tail)
		{
			if(!BSVMC_FormIs(dycaddr(l), "funcall"))
			{
				BSVMC_CompileExprCast(ctx, dycaddr(l), ctx->cf_ty);
				if(!(ctx->i_cap))
					BSVMC_EmitOp(ctx, BSVM_SOP_CLEARENV);
				BSVMC_EmitOp(ctx, BSVM_SOP_RET);
			}else
			{
				BSVMC_CompileExprT(ctx, dycaddr(l));
			}
		}else
		{
			BSVMC_CompileExprT(ctx, dycaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
			BSVMC_EncWord(ctx, 0);
			ip1=ctx->ip;
		}

		i=ctx->ip-ip0;
		*(ip0-2)=i&0xFF;
		*(ip0-1)=(i>>8)&0xFF;

		if(dyconsp(dycdddr(l)))
			BSVMC_CompileExprT(ctx, dycadddr(l));
			else BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);

		if(!ctx->tail)
		{
			i=ctx->ip-ip1;
			*(ip1-2)=i&0xFF;
			*(ip1-1)=(i>>8)&0xFF;
		}

		return;
	}

	if(dyFormIsP(l, "begin"))
	{
		BSVMC_CompileBeginExpr(ctx, l);
		return;

#if 0
		c=dycdr(l);
		while(1)
		{
			if(dyconsp(dycdr(c)))
			{
				BSVMC_CompileStatement(ctx, dycar(c));
				c=dycdr(c);
				continue;
			}

			BSVMC_CompileExprT(ctx, dycar(c));
			break;
		}
		return;
#endif
	}

	if(dyFormIsP(l, "throw"))
	{
		t=dycadr(l);
		
		if(dysymbolp(t))
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

	if(dyFormIsP(l, "ifdef") ||
		dyFormIsP(l, "ifndef"))
	{
		BSVMC_CompileIfdef(ctx, l);
		return;
	}

	if((dyFormIsP(l, "and")) || (dyFormIsP(l, "or")))
	{
		if(dyFormIsP(l, "and"))j=BSVM_SOP_JMP_FALSE;
		if(dyFormIsP(l, "or"))j=BSVM_SOP_JMP_TRUE;

		i=0; c=dycdr(l);
		while(1)
		{
			if(dyconsp(dycdr(c)))
			{
				BSVMC_CompileExpr(ctx, dycar(c));
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

				BSVMC_EmitOp(ctx, j);
				BSVMC_EncWord(ctx, 0);
				ips[i++]=ctx->ip;

				BSVMC_EmitOp(ctx, BSVM_SOP_POP);

				c=dycdr(c);
				continue;
			}

			BSVMC_CompileExprT(ctx, dycar(c));

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

	if(dyFormIsP(l, "define"))
	{
		if(!dyconsp(dycadr(l)))
		{
			BSVMC_CompileExpr(ctx, dycaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
			i=BSVMC_IndexLit(ctx, dycadr(l));
			if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
				else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		t=dycons(dysymbol("lambda"), dycons(dycdadr(l), dycddr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		n=dycaadr(l);
		if(dyFormIsP(n, "cast"))
			n=dycaddr(n);
#if 0
		i=BSVMC_IndexLit(ctx, n);
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIDX(ctx, i);
#endif

#if 1
		s=BSVMC_InferArgsBaseSig(dycadr(l));
		u=dysymbol(s);

		if(!u && !ctx->cs_fl)
		{
			if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
				else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIndexLit(ctx, n);
			return;
		}

		i=BSVMC_IndexLit(ctx, n);
		j=BSVMC_IndexLit(ctx, u);
		k=BSVMC_IndexLit(ctx, ctx->cs_fl);
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
	if(dyFormIsP(l, "defun"))
	{
//		t=CONS2S("lambda", dycadddr(l), dycddddr(l));
		t=dycons3s("rlambda", dylist2(dycadr(l), dycaddr(l)),
			dycadddr(l), dycddddr(l));

//		BSVM_TY_PrintLN(t);

		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		i=BSVMC_IndexLit(ctx, dycaddr(l));
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dyFormIsP(l, "defvar"))
	{
		if(!dyconsp(dycadr(l)))
		{
			BSVMC_CompileExpr(ctx, dycaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
			i=BSVMC_IndexLit(ctx, dycadr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		t=dycons(dysymbol("lambda"), dycons(dycdadr(l), dycddr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
		i=BSVMC_IndexLit(ctx, dycaadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}
#endif

	if(dyFormIsP(l, "vars"))
	{
//		c=dycaddr(l);

		c=dycadr(l);
		while(dyconsp(c))
		{
			n=dycar(c);
			v=NULL; t=NULL;

			BSVMC_CompileEmitVar(ctx, n);

#if 0
			if(dyFormIsP(n, "set!"))
			{
				v=dycaddr(n);
				n=dycadr(n);				
			}

			if(dyFormIsP(n, "cast"))
			{
				t=dycadr(n);
				n=dycaddr(n);				
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
			u=dysymbol(s);

			if(!u && !ctx->cs_fl)
			{
				if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
					else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
				BSVMC_EncIndexLit(ctx, n);

				c=dycdr(c);
				continue;
			}

			li=dyllDecodeFlagSig(dysymbolv(ctx->cs_fl));

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
			if(dyconsp(dycar(c)))
			{
				BSVMC_CompileExpr(ctx, dycadar(c));
				i=BSVMC_IndexLit(ctx, dycaar(c));
				if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
					else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
				BSVMC_EncIDX(ctx, i);
			}else
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
				i=BSVMC_IndexLit(ctx, dycar(c));
				if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
					else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
				BSVMC_EncIDX(ctx, i);
			}
#endif
#endif

			c=dycdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
		return;
	}

	if(dyFormIsP(l, "lambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		n=NULL;
		v=dycddr(l);

		if(dyFormIsP(v, "cast"))
		{
			n=dylist3s("cast", dycadr(v), n);
			v=dycaddr(v);
		}

		t=BSVMC_CompileBlock(ctx, n, dycadr(l), v);

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		if(ctx->cap)BSVMC_EmitOp(ctx, BSVM_SOP_CLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dyFormIsP(l, "rlambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		n=dycadr(l);
		v=dycdddr(l);
		
		if(dyFormIsP(v, "cast"))
		{
			n=dylist3s("cast", dycadr(v), n);
			v=dycaddr(v);
		}

		t=BSVMC_CompileBlock(ctx, n, dycaddr(l), v);

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		if(ctx->cap)BSVMC_EmitOp(ctx, BSVM_SOP_CLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dyFormIsP(l, "elambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		u=dycadr(l);
		n=NULL;
		v=dycdddr(l);

		if(dyFormIsP(v, "cast"))
		{
			n=dylist3s("cast", dycadr(v), n);
			v=dycaddr(v);
		}

		t=BSVMC_CompileBlock(ctx, n, dycaddr(l), v);


		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		c=u;
		while(dyconsp(c))
		{
			v=dycar(c);
			if(dyFormIsP(v, "byref"))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, dycadr(v));
			}

			if(dyFormIsP(v, "byval"))
			{
				BSVMC_CompileExpr(ctx, dycadr(v));
			}
			
			if(dysymbolp(v))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, v);
//				BSVMC_CompileExpr(ctx, v);
			}
			
			c=dycdr(c);
		}

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		BSVMC_EmitOp(ctx, BSVM_SOP_ENVCLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dyFormIsP(l, "erlambda"))
	{
		j=ctx->cap;
		ctx->cap=0;

		u=dycadr(l);
		n=dycaddr(l);
		v=dycddddr(l);
		
		if(dyFormIsP(v, "cast"))
		{
			n=dylist3s("cast", dycadr(v), n);
			v=dycaddr(v);
		}

		t=BSVMC_CompileBlockEnv(ctx, u, n, dycadddr(l), v);

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		c=u;
		while(dyconsp(c))
		{
			v=dycar(c);
			if(dyFormIsP(v, "byref"))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, dycadr(v));
			}

			if(dyFormIsP(v, "byval"))
			{
				BSVMC_CompileExpr(ctx, dycadr(v));
			}
			
			if(dysymbolp(v))
			{
//				BSVMC_CompileExpr(ctx, v);
				BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
				BSVMC_EncIndexLit(ctx, v);
			}
			
			c=dycdr(c);
		}

		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);

		BSVMC_EmitOp(ctx, BSVM_SOP_ENVCLOSE);
		ctx->cap=ctx->cap||j;

		return;
	}

	if(dyFormIsP(l, "attr"))
	{
		t=dycadr(l);
		if(dyFormIsP(t, "quote"))
			t=dykeyword(dysymbolv(dycadr(t)));

		BSVMC_CompileExpr(ctx, dycaddr(l));
//		i=BSVMC_IndexLit(ctx, dycadr(l));
		i=BSVMC_IndexLit(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_ATTR);
		
		return;
	}

	if(dyFormIsP(l, "dict"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		i=BSVMC_IndexLit(ctx, dykeyword("parent"));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF);

		j=ctx->lvl;
		ctx->lvl=0;
		ctx->olvl++;

		c=dycdr(l);
		while(dyconsp(c))
		{
			t=dycar(c);

			if(dyFormIsP(t, "attr") || dyFormIsP(t, "set!"))
			{
				v=dycadr(t);
				if(dyFormIsP(v, "quote"))
					v=dykeyword(dysymbolv(dycadr(v)));
				i=BSVMC_IndexLit(ctx, v);
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
				BSVMC_EncIDX(ctx, i);
				BSVMC_CompileExpr(ctx, dycaddr(t));
			}

			if(dyFormIsP(t, "define"))
			{
				i=BSVMC_IndexLit(ctx, dycaadr(t));
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
				BSVMC_EncIDX(ctx, i);
				u=dycons2s("lambda", dycdadr(t), dycddr(t));
				BSVMC_CompileExpr(ctx, u);
			}

			c=dycdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

		ctx->olvl--;
		ctx->lvl=j;
		return;
	}

#if 0
	if(dyFormIsP(l, "new"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

		i=BSVMC_IndexLit(ctx, dykeyword("parent"));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF);
		BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
		BSVMC_CompileStatement(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);

		return;
	}
#endif

#if 1
	if(dyFormIsP(l, "new"))
	{
		BSVMC_CompileNew(ctx, dycadr(l));
		return;
	}
#endif

	if(dyFormIsP(l, "delete"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DELETE);
		return;
	}

	if(dyFormIsP(l, "instanceof"))
	{
		BSVMC_CompileInstanceof(ctx, l);
		return;
	}

	if(dyFormIsP(l, "with"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);

		return;
	}

	if(dyFormIsP(l, "with_top"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_TOP);
		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
		BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_TOP);

		return;
	}

	if(dyFormIsP(l, "class") ||
		dyFormIsP(l, "interface") ||
		dyFormIsP(l, "struct"))
//	if(dyFormIsP(l, "class"))
	{
		BSVMC_CompileClass(ctx, l);
		return;
	}

	if(dyFormIsP(l, "package"))
	{
		BSVMC_CompilePackage(ctx, l);
		return;
	}

	if((dyFormIsP(l, "list")) || (dyFormIsP(l, "array")) ||
		(dyFormIsP(l, "vector")))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycdr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}
		if(dyFormIsP(l, "list"))
		{
			BSVMC_CompileExpr(ctx, c);
			BSVMC_EmitOp(ctx, BSVM_SOP_LIST);
		}
		if(dyFormIsP(l, "array"))BSVMC_EmitOp(ctx, BSVM_SOP_ARRAY);
		if(dyFormIsP(l, "vector"))BSVMC_EmitOp(ctx, BSVM_SOP_VECTOR);
		return;
	}

	if(dyFormIsP(l, "quat"))
	{
		c=dycdr(l); i=4;
		while(dyconsp(c) && i)
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c); i--;
		}
		while(i--)
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_0);
		BSVMC_EmitOp(ctx, BSVM_SOP_QUAT);
		return;
	}

	if(dyFormIsP(l, "array_ty"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycddr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}

		s=BSVMC_InferTypeBaseSig(dycadr(l));
		t=dysymbol(s);
		i=BSVMC_IndexLit(ctx, t);

		BSVMC_EmitOp(ctx, BSVM_SOP_ARRAY_S);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dyFormIsP(l, "matrix"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycdr(l);
		while(dyconsp(c))
		{
			if(dyconsp(dycar(c)))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
				d=dycar(c);
				while(dyconsp(d))
				{
					BSVMC_CompileExpr(ctx, dycar(d));
					d=dycdr(d);
				}
				BSVMC_EmitOp(ctx, BSVM_SOP_VECTOR);
			}else BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}

		BSVMC_EmitOp(ctx, BSVM_SOP_MATRIX);
		return;
	}

	if(dyFormIsP(l, "unary"))
	{
		BSVMC_CompileUnaryExpr(ctx, l);
		return;
	}

	if(BSVMC_FormIs(l, "binary"))
	{
		BSVMC_CompileBinaryExpr(ctx, l);
		return;
	}

	if(dyFormIsP(l, "funcall"))
	{
		if(dysymbolp(dycadr(l)))
		{
			i=BSVMC_CompileSpecial(ctx, dycdr(l));
			if(i>0)return;
		}

		s=BSVMC_InferCallArgsBaseSig(ctx, dycdr(l), NULL);
		BSVMC_EmitOp(ctx, BSVM_SOP_TYMARK);
		BSVMC_EncIndexLit(ctx, dysymbol(s));

//		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycddr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}

		if(dycadr(l)==ctx->cf_n)
		{
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_CF);
				else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_CF);
			return;
		}

		t=BSVMC_ReduceExpr(ctx, dycadr(l));
//		if(dysymbolp(dycadr(l)))
		if(dysymbolp(t))
		{
			i=BSVMC_IndexLit(ctx, t);
//			i=BSVMC_IndexLit(ctx, dycadr(l));
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL_S);
				else BSVMC_EmitOp(ctx, BSVM_SOP_CALL_S);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		BSVMC_CompileExpr(ctx, t);
//		BSVMC_CompileExpr(ctx, dycadr(l));
		if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL);
			else BSVMC_EmitOp(ctx, BSVM_SOP_CALL);
		return;
	}

	if(dyFormIsP(l, "methodcall"))
	{
		i=BSVMC_CompileSpecialMethod(ctx, dycdr(l));
		if(i>0)return;

		s=BSVMC_InferMethodCallArgsBaseSig(ctx, dycdr(l), NULL);
		BSVMC_EmitOp(ctx, BSVM_SOP_TYMARK);
		BSVMC_EncIndexLit(ctx, dysymbol(s));

//		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		c=dycdddr(l);
		while(dyconsp(c))
		{
			BSVMC_CompileExpr(ctx, dycar(c));
			c=dycdr(c);
		}

		BSVMC_CompileExpr(ctx, dycadr(l));

//		t=dycaddr(l);
		t=BSVMC_ReduceExpr(ctx, dycaddr(l));
		if(dysymbolp(t))
		{
			if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_METHODTAILCALL_S);
				else BSVMC_EmitOp(ctx, BSVM_SOP_METHODCALL_S);
			BSVMC_EncIndexLit(ctx, t);
			return;
		}

//		i=BSVMC_IndexLit(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIndexLit(ctx, t);
//		BSVMC_EncIDX(ctx, i);

		if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_METHODTAILCALL);
			else BSVMC_EmitOp(ctx, BSVM_SOP_METHODCALL);
		return;
	}

	if(dyFormIsP(l, "complex"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX_I);
		return;
	}

	if(dyFormIsP(l, "inline_c"))
	{
		t=dycadr(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_CGENAST);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}

	dyPrintf("unhandled expr %s\n", dysymbolv(dycar(l)));
	BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);

#if 0
	BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
	c=dycdr(l);
	while(dyconsp(c))
	{
		BSVMC_CompileExpr(ctx, dycar(c));
		c=dycdr(c);
	}
	BSVMC_CompileExpr(ctx, dycar(l));
	if(ctx->tail)BSVMC_EmitOp(ctx, BSVM_SOP_TAILCALL);
		else BSVMC_EmitOp(ctx, BSVM_SOP_CALL);
#endif
}

void BSVMC_CompileExprT(BSVM_SVMCState *ctx, dyt l)
{
	float f, g;
	dyt t, c, ct, cv;
	int i, j;

	t=l;
	l=BSVMC_ReduceExpr(ctx, l);

	if(BSVMC_DebugP(ctx))
	{
		dyPrintf("expr: ");
		dyPrint(t);
		dyPrintf(" -> ");
		dyPrint(l);
		dyPrintf("\n");
	}

	if(dyconsp(l))
	{
		BSVMC_CompileForm(ctx, l);
		return;
	}

	if(dysymbolp(l))
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
		c=ctx->lenv; ct=ctx->tenv; cv=ctx->venv; i=0; j=0;
		while(dyconsp(c))
		{
			if(c==ctx->llenv)i=1;
			if(dycar(c)==l)break;
			c=dycdr(c); ct=dycdr(ct); cv=dycdr(cv);
			j++;
		}
		if(dyconsp(c) && i)ctx->cap++;

		if(!dyconsp(c))
		{
//			if((l==dysymbol("self")) || (l==dysymbol("this")))
			if(l==dysymbol("this"))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SELF); return; }
			if(l==dysymbol("true"))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_TRUE); return; }
			if(l==dysymbol("false"))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_FALSE); return; }
			if(l==dysymbol("null"))
				{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL); return; }
		}

		if((l==ctx->cf_n) && (!dyconsp(c) || i))
			{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_CF); return; }

#if 0
		if(dyconsp(c) && i)dyPrintf("CCAP: %s\n",
			BSVM_TY_GetSymbol(l));
		if(dyconsp(c) && !i)dyPrintf("LOC: %s\n",
			BSVM_TY_GetSymbol(l));
		if(!dyconsp(c))dyPrintf("OBJ: %s\n",
			BSVM_TY_GetSymbol(l));
#endif

		if(dyconsp(c))
		{
			t=dycar(ct);

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

//			if((t==dykeyword("int")) || (t==dykeyword("float")))
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

	if(l==NULL)
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL); }
	else if(l==BGBDY_TRUE)
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_TRUE); }
	else if(l==BGBDY_FALSE)
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_FALSE); }
	else if(l==dyint(0)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_0); }
	else if(l==dyint(1)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_1); }
	else if(l==dyint(2)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_2); }
	else if(l==dyint(3)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_3); }
	else if(l==dyint(4)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_4); }
	else if(l==dyint(5)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_5); }
	else if(l==dyint(6)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_6); }
	else if(l==dyint(7)) { BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_7); }
	else if(dycharp(l))
	{
		i=dyintv(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_SV_C);
		BSVMC_EncWord(ctx, i);
	}
	else if(dyintp(l))
	{
		i=dyintv(l);

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
	}else if(dyfloatp(l))
	{
		f=dyfloatv(l);
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
	}else if(dycomplexp(l))
	{
		f=dycreal(l);
		g=dycimag(l);
		
		if(f==0)
		{
			if(g==((int)g))
			{
				BSVMC_CompileExpr(ctx, dyint(g));
				BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX_I);
				return;
			}

			BSVMC_CompileExpr(ctx, dyfloat(g));
			BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX_I);
			return;
		}

		BSVMC_CompileExpr(ctx, (f==((int)f))?dyint(f):dyfloat(f));
		BSVMC_CompileExpr(ctx, (g==((int)g))?dyint(g):dyfloat(g));
		BSVMC_EmitOp(ctx, BSVM_SOP_COMPLEX);
		return;
	}else if(dyquatp(l))
	{
		f=qw(dyquatv(l));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dyint(f):dyfloat(f));
		f=qx(dyquatv(l));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dyint(f):dyfloat(f));
		f=qy(dyquatv(l));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dyint(f):dyfloat(f));
		f=qz(dyquatv(l));
		BSVMC_CompileExpr(ctx, (f==((int)f))?dyint(f):dyfloat(f));
		BSVMC_EmitOp(ctx, BSVM_SOP_QUAT);
		return;
	}else
	{
		i=BSVMC_IndexLit(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
		BSVMC_EncIDX(ctx, i);
	}
}

void BSVMC_CompileExpr(BSVM_SVMCState *ctx, dyt l)
{
	int i;

	i=ctx->tail;
	ctx->tail=0;
	BSVMC_CompileExprT(ctx, l);
	ctx->tail=i;
}

void BSVMC_CompileExprTail(BSVM_SVMCState *ctx, dyt l)
{
	int i;

	i=ctx->tail;
	ctx->tail=1;
	BSVMC_CompileExprT(ctx, l);
	ctx->tail=i;
}
