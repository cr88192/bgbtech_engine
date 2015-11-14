#include <bgbsvm.h>

int BSVMC_TypeFixnumP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("fixnum"))
	if(s && !strcmp(s, "fixnum"))
		return(1);
	return(0);
}

int BSVMC_TypeFlonumP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("fixnum"))
	if(s && !strcmp(s, "fixnum"))
		return(1);
	return(0);
}

int BSVMC_TypeFixrealP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
	if(s && !strcmp(s, "fixnum"))
		return(1);
	if(s && !strcmp(s, "flonum"))
		return(1);
	return(0);
}

int BSVMC_TypeSmallIntP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
	if(s && !strcmp(s, "int"))return(1);
	if(s && !strcmp(s, "ushort"))return(1);
	if(s && !strcmp(s, "short"))return(1);
	if(s && !strcmp(s, "byte"))return(1);
	if(s && !strcmp(s, "sbyte"))return(1);

//	if(ty==dykeyword("int"))return(1);
//	if(ty==dykeyword("ushort"))return(1);
//	if(ty==dykeyword("short"))return(1);
//	if(ty==dykeyword("byte"))return(1);
//	if(ty==dykeyword("sbyte"))return(1);
	return(0);
}

int BSVMC_TypeLongP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("long"))
	if(s && !strcmp(s, "long"))
		return(1);
	return(0);
}

int BSVMC_TypeSmallLongP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("long"))
	if(s && !strcmp(s, "long"))
		return(1);
	if(BSVMC_TypeSmallIntP(ctx, ty))
		return(1);
	return(0);
}

int BSVMC_TypeFloatP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("float"))
	if(s && !strcmp(s, "float"))
		return(1);
	return(0);
}

int BSVMC_TypeDoubleP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("double"))
	if(s && !strcmp(s, "double"))
		return(1);
	return(0);
}

int BSVMC_TypeSmallFloatP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("float"))
	if(s && !strcmp(s, "float"))
		return(1);
	if(BSVMC_TypeSmallIntP(ctx, ty))
		return(1);
	return(0);
}

int BSVMC_TypeSmallDoubleP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("double"))
	if(s && !strcmp(s, "double"))
		return(1);
	if(BSVMC_TypeSmallLongP(ctx, ty))
		return(1);
	return(0);
}

int BSVMC_TypeSmallFixIntP(BSVM_SVMCState *ctx, dytf ty)
{
	if(BSVMC_TypeFixnumP(ctx, ty))return(1);
	if(BSVMC_TypeSmallIntP(ctx, ty))return(1);
	return(0);
}

int BSVMC_TypeSmallFixRealP(BSVM_SVMCState *ctx, dytf ty)
{
	char *s;

	s=dytfSymbolv(ty);
//	if(ty==dykeyword("flonum"))
	if(s && !strcmp(s, "flonum"))
		return(1);
//	if(ty==dykeyword("float"))
	if(s && !strcmp(s, "float"))
		return(1);
	if(BSVMC_TypeSmallFixIntP(ctx, ty))
		return(1);
	return(0);
}

dytf BSVMC_TypeArrayBaseType(BSVM_SVMCState *ctx, dytf ty)
{
	if(dytfFormIsP(ty, "getindex"))
	{
		return(dytfCadr(ty));
	}

	if(dytfFormIsP(ty, "emptyindex"))
	{
		return(dytfCadr(ty));
	}
	
	return(DYTF_NULL);
}

int BSVMC_CompileBinary(BSVM_SVMCState *ctx, dytf l)
{
	dytf ln, rn, lt, rt;
	dytf t0, t1;
	int lc, rc;

	int i, j, k;

	ln=BSVMC_ReduceExpr(ctx, dytfCadr(l));
	rn=BSVMC_ReduceExpr(ctx, dytfCaddr(l));

	lt=BSVMC_InferExprClean(ctx, ln, &lc);
	rt=BSVMC_InferExprClean(ctx, rn, &rc);

#if 0
	dyPrintf("Infer L: ");
	BSVM_TY_Print(ln);
	dyPrintf(" ");
	BSVM_TY_Print(lt);
	dyPrintf(" %s\n", lc?"clean":"dirty");

	dyPrintf("Infer R: ");
	BSVM_TY_Print(rn);
	dyPrintf(" ");
	BSVM_TY_Print(rt);
	dyPrintf(" %s\n", rc?"clean":"dirty");
#endif

	if(dytfNullP(lt))return(0);
	if(dytfNullP(rt))return(0);

#ifdef BSVM_USE_BVT
	if(BSVMC_TypeSmallIntP(ctx, lt) && dytfIntP(rn))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XI_C;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_XI_C;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XI_C;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_XI_C;

		if(i>0)
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, i);
			BSVMC_EncSVLI(ctx, dytfIntv(rn));
			return(1);
		}

		if(dytfFormIsP(l, "<<"))i=BSVM_SOP_SHL_XI_C;
		if(dytfFormIsP(l, ">>"))i=BSVM_SOP_SHR_XI_C;
		if(dytfFormIsP(l, ">>>"))i=BSVM_SOP_SHR_XI_C;

		if(i>0)
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, i);
			BSVMC_EncByte(ctx, dytfIntv(rn));
			return(1);
		}
	}

	if(BSVMC_TypeLongP(ctx, lt) && dytfLongP(rn))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XL_C;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_XL_C;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XL_C;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_XL_C;

		if(i>0)
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, i);
			BSVMC_EncSVLI(ctx, dytfLongv(rn));
			return(1);
		}

		if(dytfFormIsP(l, "<<"))i=BSVM_SOP_SHL_XL_C;
		if(dytfFormIsP(l, ">>"))i=BSVM_SOP_SHR_XL_C;
		if(dytfFormIsP(l, ">>>"))i=BSVM_SOP_SHR_XL_C;

		if(i>0)
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, i);
			BSVMC_EncByte(ctx, dytfLongv(rn));
			return(1);
		}
	}

	if(dytfIntP(ln) && BSVMC_TypeSmallIntP(ctx, rt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XI_C;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XI_C;

		if(i>0)
		{
			BSVMC_CompileExpr(ctx, rn);
			BSVMC_EmitOp(ctx, i);
			BSVMC_EncSVLI(ctx, dytfIntv(ln));
			return(1);
		}
	}

	if(dytfLongP(ln) && BSVMC_TypeLongP(ctx, rt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XL_C;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XL_C;

		if(i>0)
		{
			BSVMC_CompileExpr(ctx, rn);
			BSVMC_EmitOp(ctx, i);
			BSVMC_EncSVLI(ctx, dytfLongv(ln));
			return(1);
		}
	}

//	if((lt==rt) && BSVMC_TypeSmallIntP(ctx, lt))
	if(BSVMC_TypeSmallIntP(ctx, lt) &&
		BSVMC_TypeSmallIntP(ctx, rt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XI;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_XI;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XI;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_XI;
		if(dytfFormIsP(l, "\\"))i=BSVM_SOP_DIV_XI;
		if(dytfFormIsP(l, "%"))i=BSVM_SOP_MOD_XI;
		if(dytfFormIsP(l, "&"))i=BSVM_SOP_AND_XI;
		if(dytfFormIsP(l, "|"))i=BSVM_SOP_OR_XI;
		if(dytfFormIsP(l, "^"))i=BSVM_SOP_XOR_XI;
		if(dytfFormIsP(l, "<<"))i=BSVM_SOP_SHL_XI;
		if(dytfFormIsP(l, ">>"))i=BSVM_SOP_SHR_XI;
		if(dytfFormIsP(l, ">>>"))i=BSVM_SOP_SHR_XI;
//		if(dytfFormIsP(l, "**"))i=BSVM_SOP_EXP_XI;

		if(dytfFormIsP(l, "<"))i=BSVM_SOP_CMP_LT_XI;
		if(dytfFormIsP(l, ">"))i=BSVM_SOP_CMP_GT_XI;
		if(dytfFormIsP(l, "<="))i=BSVM_SOP_CMP_LE_XI;
		if(dytfFormIsP(l, ">="))i=BSVM_SOP_CMP_GE_XI;
		if(dytfFormIsP(l, "=="))i=BSVM_SOP_CMP_EQ_XI;
		if(dytfFormIsP(l, "!="))i=BSVM_SOP_CMP_NE_XI;
		if(dytfFormIsP(l, "==="))i=BSVM_SOP_CMP_EQ_XI;
		if(dytfFormIsP(l, "!=="))i=BSVM_SOP_CMP_NE_XI;

		if(i<0)return(0);

		if(lc && rc && dytfEqP(ln, rn))
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_XI);
			BSVMC_EmitOp(ctx, i);
			return(1);
		}

		BSVMC_CompileExpr(ctx, ln);
		BSVMC_CompileExpr(ctx, rn);
		BSVMC_EmitOp(ctx, i);

		return(1);
	}

	if(BSVMC_TypeSmallLongP(ctx, lt) &&
		BSVMC_TypeSmallLongP(ctx, rt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XL;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_XL;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XL;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_XL;
		if(dytfFormIsP(l, "\\"))i=BSVM_SOP_DIV_XL;
		if(dytfFormIsP(l, "%"))i=BSVM_SOP_MOD_XL;
		if(dytfFormIsP(l, "&"))i=BSVM_SOP_AND_XL;
		if(dytfFormIsP(l, "|"))i=BSVM_SOP_OR_XL;
		if(dytfFormIsP(l, "^"))i=BSVM_SOP_XOR_XL;
		if(dytfFormIsP(l, "<<"))i=BSVM_SOP_SHL_XL;
		if(dytfFormIsP(l, ">>"))i=BSVM_SOP_SHR_XL;
		if(dytfFormIsP(l, ">>>"))i=BSVM_SOP_SHR_XL;
//		if(dytfFormIsP(l, "**"))i=BSVM_SOP_EXP_XL;

		if(dytfFormIsP(l, "<"))i=BSVM_SOP_CMP_LT_XL;
		if(dytfFormIsP(l, ">"))i=BSVM_SOP_CMP_GT_XL;
		if(dytfFormIsP(l, "<="))i=BSVM_SOP_CMP_LE_XL;
		if(dytfFormIsP(l, ">="))i=BSVM_SOP_CMP_GE_XL;
		if(dytfFormIsP(l, "=="))i=BSVM_SOP_CMP_EQ_XL;
		if(dytfFormIsP(l, "!="))i=BSVM_SOP_CMP_NE_XL;
		if(dytfFormIsP(l, "==="))i=BSVM_SOP_CMP_EQ_XL;
		if(dytfFormIsP(l, "!=="))i=BSVM_SOP_CMP_NE_XL;

		if(i<0)return(0);

		if(lc && rc && dytfEqP(ln, rn))
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_XL);
			BSVMC_EmitOp(ctx, i);
			return(1);
		}

		BSVMC_CompileExpr(ctx, ln);
		if(BSVMC_TypeSmallIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XI2XL);
		BSVMC_CompileExpr(ctx, rn);
		if(BSVMC_TypeSmallIntP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XI2XL);
		BSVMC_EmitOp(ctx, i);

		return(1);
	}

	if(BSVMC_TypeSmallFloatP(ctx, lt) &&
		BSVMC_TypeSmallFloatP(ctx, rt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XF;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_XF;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XF;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_XF;

		if(dytfFormIsP(l, "<"))i=BSVM_SOP_CMP_LT_XF;
		if(dytfFormIsP(l, ">"))i=BSVM_SOP_CMP_GT_XF;
		if(dytfFormIsP(l, "<="))i=BSVM_SOP_CMP_LE_XF;
		if(dytfFormIsP(l, ">="))i=BSVM_SOP_CMP_GE_XF;
		if(dytfFormIsP(l, "=="))i=BSVM_SOP_CMP_EQ_XF;
		if(dytfFormIsP(l, "!="))i=BSVM_SOP_CMP_NE_XF;
		if(dytfFormIsP(l, "==="))i=BSVM_SOP_CMP_EQ_XF;
		if(dytfFormIsP(l, "!=="))i=BSVM_SOP_CMP_NE_XF;

		if(i<0)return(0);

		if(lc && rc && dytfEqP(ln, rn))
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_XF);
			BSVMC_EmitOp(ctx, i);
			return(1);
		}

		BSVMC_CompileExpr(ctx, ln);
		if(BSVMC_TypeSmallIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XI2XF);
		BSVMC_CompileExpr(ctx, rn);
		if(BSVMC_TypeSmallIntP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XI2XF);
		BSVMC_EmitOp(ctx, i);

		return(1);
	}

	if(BSVMC_TypeSmallDoubleP(ctx, lt) &&
		BSVMC_TypeSmallDoubleP(ctx, rt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_XD;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_XD;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_XD;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_XD;

		if(dytfFormIsP(l, "<"))i=BSVM_SOP_CMP_LT_XD;
		if(dytfFormIsP(l, ">"))i=BSVM_SOP_CMP_GT_XD;
		if(dytfFormIsP(l, "<="))i=BSVM_SOP_CMP_LE_XD;
		if(dytfFormIsP(l, ">="))i=BSVM_SOP_CMP_GE_XD;
		if(dytfFormIsP(l, "=="))i=BSVM_SOP_CMP_EQ_XD;
		if(dytfFormIsP(l, "!="))i=BSVM_SOP_CMP_NE_XD;
		if(dytfFormIsP(l, "==="))i=BSVM_SOP_CMP_EQ_XD;
		if(dytfFormIsP(l, "!=="))i=BSVM_SOP_CMP_NE_XD;

		if(i<0)return(0);

		if(lc && rc && dytfEqP(ln, rn))
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_XD);
			BSVMC_EmitOp(ctx, i);
			return(1);
		}

		BSVMC_CompileExpr(ctx, ln);
		if(BSVMC_TypeSmallIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XI2XD);
		if(BSVMC_TypeLongP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XL2XD);
		if(BSVMC_TypeFloatP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XF2XD);
		BSVMC_CompileExpr(ctx, rn);
		if(BSVMC_TypeSmallIntP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XI2XD);
		if(BSVMC_TypeLongP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XL2XD);
		if(BSVMC_TypeFloatP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_XF2XD);
		BSVMC_EmitOp(ctx, i);

		return(1);
	}
#endif

//	if((lt==rt) && (lt==dykeyword("int")))
//	if((lt==rt) && (lt==dykeyword("fixnum")))
	if(dytfEqP(lt, rt) && BSVMC_TypeSmallFixIntP(ctx, lt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_FN;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_FN;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_FN;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_FN;
		if(dytfFormIsP(l, "\\"))i=BSVM_SOP_IDIV_FN;
		if(dytfFormIsP(l, "%"))i=BSVM_SOP_MOD_FN;
		if(dytfFormIsP(l, "&"))i=BSVM_SOP_AND_FN;
		if(dytfFormIsP(l, "|"))i=BSVM_SOP_OR_FN;
		if(dytfFormIsP(l, "^"))i=BSVM_SOP_XOR_FN;

		if(dytfFormIsP(l, "<<"))i=BSVM_SOP_SHL_FN;
		if(dytfFormIsP(l, ">>"))i=BSVM_SOP_SHR_FN;
		if(dytfFormIsP(l, ">>>"))i=BSVM_SOP_SHR_FN;
		if(dytfFormIsP(l, "**"))i=BSVM_SOP_EXP_FN;

		if(dytfFormIsP(l, "<"))i=BSVM_SOP_CMP_L_FN;
		if(dytfFormIsP(l, ">"))i=BSVM_SOP_CMP_G_FN;
		if(dytfFormIsP(l, "<="))i=BSVM_SOP_CMP_LE_FN;
		if(dytfFormIsP(l, ">="))i=BSVM_SOP_CMP_GE_FN;
		if(dytfFormIsP(l, "=="))i=BSVM_SOP_CMP_E_FN;
		if(dytfFormIsP(l, "!="))i=BSVM_SOP_CMP_NE_FN;
		if(dytfFormIsP(l, "==="))i=BSVM_SOP_CMP_E_FN;
		if(dytfFormIsP(l, "!=="))i=BSVM_SOP_CMP_NE_FN;

		if(i<0)return(0);

		if(((i==BSVM_SOP_ADD_FN) || (i==BSVM_SOP_SUB_FN)) &&
			(dytfIntP(ln) || dytfIntP(rn)))
		{
			if(dytfIntP(ln)) { t0=rn; t1=ln; }
				else { t0=ln; t1=rn; }

			j=dytfIntv(t1);
			if(!j)
			{
				BSVMC_CompileExpr(ctx, t0);
				return(1);
			}

			k=1;
			if(i==BSVM_SOP_SUB_FN)
			{
				j=-j;

				//handle case of sides flipped
				if(dytfIntP(ln))k=0;
			}

			if(k && (j>0) && (j<256))
			{
				BSVMC_CompileExpr(ctx, t0);
				if(j<3)
				{
					if(j==1)BSVMC_EmitOp(ctx, BSVM_SOP_INC_FN);
					else BSVMC_EmitOp(ctx, BSVM_SOP_INC2_FN);
					return(1);
				}

				BSVMC_EmitOp(ctx, BSVM_SOP_ADD_FN_C);
				BSVMC_EncByte(ctx, j);
				return(1);
			}
			if(k && (j<0) && (j>-256))
			{
				BSVMC_CompileExpr(ctx, t0);
				if(j>-3)
				{
					if(j==-1)BSVMC_EmitOp(ctx, BSVM_SOP_DEC_FN);
					else BSVMC_EmitOp(ctx, BSVM_SOP_DEC2_FN);
					return(1);
				}

				BSVMC_EmitOp(ctx, BSVM_SOP_SUB_FN_C);
				BSVMC_EncByte(ctx, j);
				return(1);
			}
		}

		if((i==BSVM_SOP_MUL_FN) &&
			(dytfIntP(ln) || dytfIntP(rn)))
		{
			if(dytfIntP(ln)) { t0=rn; t1=ln; }
				else { t0=ln; t1=rn; }

			j=dytfIntv(t1);
			if(!j)
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_0);
				return(1);
			}

			k=0;
			if(j<0) { j=-j; k=1; }

			if(j<256)
			{
				BSVMC_CompileExpr(ctx, t0);
				BSVMC_EmitOp(ctx, BSVM_SOP_MUL_FN_C);
				BSVMC_EncByte(ctx, j);
				if(k)BSVMC_EmitOp(ctx, BSVM_SOP_NEG_FN);
				return(1);
			}
		}

#if 0
		if((i==BSVM_SOP_EXP_FN) && dytfIntP(rn))
		{
			j=dyintv(rn);
			if(!j)
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_1);
				return(1);
			}

			if((j>0) && (j<=8))
			{
				BSVMC_CompileExpr(ctx, ln);
				for(k=1; k<j; k++)
					BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
				for(k=1; k<j; k++)
					BSVMC_EmitOp(ctx, BSVM_SOP_MUL_FN);
				return(1);
			}
		}
#endif

		if(((i==BSVM_SOP_SHL_FN) || (i==BSVM_SOP_SHR_FN) ||
			(i==BSVM_SOP_EXP_FN)) && dytfIntP(rn))
		{
			j=dytfIntv(rn);
			if(!j && ((i==BSVM_SOP_SHL_FN) || (i==BSVM_SOP_SHR_FN)))
			{
				BSVMC_CompileExpr(ctx, ln);
				return(1);
			}
			if(!j && (i==BSVM_SOP_EXP_FN))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_1);
				return(1);
			}

			if((j>0) && (j<256))
			{
				if(i==BSVM_SOP_SHL_FN)i=BSVM_SOP_SHL_FN_C;
				if(i==BSVM_SOP_SHR_FN)i=BSVM_SOP_SHR_FN_C;
				if(i==BSVM_SOP_EXP_FN)i=BSVM_SOP_EXP_FN_C;

				BSVMC_CompileExpr(ctx, ln);
				BSVMC_EmitOp(ctx, i);
				BSVMC_EncByte(ctx, j);
				return(1);
			}
		}

		if(lc && rc && dytfEqP(ln, rn))
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
			BSVMC_EmitOp(ctx, i);
			return(1);
		}

		BSVMC_CompileExpr(ctx, ln);
		BSVMC_CompileExpr(ctx, rn);
		BSVMC_EmitOp(ctx, i);

		return(1);
	}

	i=0; j=0;
//	if(lt==dykeyword("int"))i=1;
//	if(lt==dykeyword("float"))i=1;
//	if(rt==dykeyword("int"))j=1;
//	if(rt==dykeyword("float"))j=1;

//	if(i && j)
	if(BSVMC_TypeSmallFixRealP(ctx, lt) &&
		BSVMC_TypeSmallFixRealP(ctx, rt))
	{
		i=-1;

		if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD_FL;
		if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB_FL;
		if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL_FL;
		if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV_FL;

		if(dytfFormIsP(l, "<"))i=BSVM_SOP_CMP_L_FL;
		if(dytfFormIsP(l, ">"))i=BSVM_SOP_CMP_G_FL;
		if(dytfFormIsP(l, "<="))i=BSVM_SOP_CMP_LE_FL;
		if(dytfFormIsP(l, ">="))i=BSVM_SOP_CMP_GE_FL;
		if(dytfFormIsP(l, "=="))i=BSVM_SOP_CMP_E_FL;
		if(dytfFormIsP(l, "!="))i=BSVM_SOP_CMP_NE_FL;

		if(i<0)return(0);

		if(lc && rc && dytfEqP(ln, rn))
		{
			BSVMC_CompileExpr(ctx, ln);
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP_F);
			BSVMC_EmitOp(ctx, i);
		}


		BSVMC_CompileExpr(ctx, ln);
//		if(lt==dykeyword("int"))
		if(BSVMC_TypeSmallFixIntP(ctx, lt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FN2FL);

		BSVMC_CompileExpr(ctx, rn);
//		if(rt==dykeyword("int"))
		if(BSVMC_TypeSmallFixIntP(ctx, rt))
			BSVMC_EmitOp(ctx, BSVM_SOP_CONV_FN2FL);

		BSVMC_EmitOp(ctx, i);

		return(1);
	}

	i=-1;
	if(dytfFormIsP(l, "+"))i=BSVM_SOP_ADD;
	if(dytfFormIsP(l, "-"))i=BSVM_SOP_SUB;
	if(dytfFormIsP(l, "*"))i=BSVM_SOP_MUL;
	if(dytfFormIsP(l, "/"))i=BSVM_SOP_DIV;
	if(dytfFormIsP(l, "%"))i=BSVM_SOP_MOD;
	if(dytfFormIsP(l, "&"))i=BSVM_SOP_AND;
	if(dytfFormIsP(l, "|"))i=BSVM_SOP_OR;
	if(dytfFormIsP(l, "^"))i=BSVM_SOP_XOR;

	if(i>0)
	{
		BSVMC_CompileExpr(ctx, ln);
		BSVMC_CompileExpr(ctx, rn);
		BSVMC_EmitOp(ctx, i);
		return(1);
	}
	
	return(0);
}

int BSVMC_CompileBinaryExpr(BSVM_SVMCState *ctx, dytf l)
{
	int i;

	i=BSVMC_CompileBinary(ctx, dytfCdr(l));
	if(i)return(i);

	BSVMC_CompileExpr(ctx, dytfCaddr(l));
	BSVMC_CompileExpr(ctx, dytfCadddr(l));

	i=-1;
	if(dytfEqP(dytfCadr(l), dytfSymbol("+")))i=BSVM_OPR_ADD;
	if(dytfEqP(dytfCadr(l), dytfSymbol("-")))i=BSVM_OPR_SUB;
	if(dytfEqP(dytfCadr(l), dytfSymbol("*")))i=BSVM_OPR_MUL;
	if(dytfEqP(dytfCadr(l), dytfSymbol("/")))i=BSVM_OPR_DIV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("\\")))i=BSVM_OPR_IDIV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("%")))i=BSVM_OPR_MOD;
	if(dytfEqP(dytfCadr(l), dytfSymbol("&")))i=BSVM_OPR_AND;
	if(dytfEqP(dytfCadr(l), dytfSymbol("|")))i=BSVM_OPR_OR;
	if(dytfEqP(dytfCadr(l), dytfSymbol("^")))i=BSVM_OPR_XOR;
	if(dytfEqP(dytfCadr(l), dytfSymbol("**")))i=BSVM_OPR_EXP;

	if(dytfEqP(dytfCadr(l), dytfSymbol("+.")))i=BSVM_OPR_DADD;
	if(dytfEqP(dytfCadr(l), dytfSymbol("-.")))i=BSVM_OPR_DSUB;
	if(dytfEqP(dytfCadr(l), dytfSymbol("*.")))i=BSVM_OPR_DMUL;
	if(dytfEqP(dytfCadr(l), dytfSymbol("/.")))i=BSVM_OPR_DDIV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("\\.")))i=BSVM_OPR_DIDIV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("%.")))i=BSVM_OPR_DMOD;
	if(dytfEqP(dytfCadr(l), dytfSymbol("&.")))i=BSVM_OPR_DAND;
	if(dytfEqP(dytfCadr(l), dytfSymbol("|.")))i=BSVM_OPR_DOR;
	if(dytfEqP(dytfCadr(l), dytfSymbol("^.")))i=BSVM_OPR_DXOR;

	if(dytfEqP(dytfCadr(l), dytfSymbol("<<")))i=BSVM_OPR_SHL;
	if(dytfEqP(dytfCadr(l), dytfSymbol(">>")))i=BSVM_OPR_SHR;
	if(dytfEqP(dytfCadr(l), dytfSymbol(">>>")))i=BSVM_OPR_SHR;
	if(dytfEqP(dytfCadr(l), dytfSymbol("==")))i=BSVM_OPR_EQV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("!=")))i=BSVM_OPR_NEQV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("===")))i=BSVM_OPR_EQ;
	if(dytfEqP(dytfCadr(l), dytfSymbol("!==")))i=BSVM_OPR_NEQ;
	if(dytfEqP(dytfCadr(l), dytfSymbol("<")))i=BSVM_OPR_L;
	if(dytfEqP(dytfCadr(l), dytfSymbol(">")))i=BSVM_OPR_G;
	if(dytfEqP(dytfCadr(l), dytfSymbol("<=")))i=BSVM_OPR_LE;
	if(dytfEqP(dytfCadr(l), dytfSymbol(">=")))i=BSVM_OPR_GE;

	if(i<0)
	{
		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_BINARYOP_S);
		BSVMC_EncIDX(ctx, i);
		return(1);
	}

	switch(i)
	{
	case BSVM_OPR_ADD: BSVMC_EmitOp(ctx, BSVM_SOP_ADD); break;
	case BSVM_OPR_SUB: BSVMC_EmitOp(ctx, BSVM_SOP_SUB); break;
	case BSVM_OPR_MUL: BSVMC_EmitOp(ctx, BSVM_SOP_MUL); break;
	case BSVM_OPR_DIV: BSVMC_EmitOp(ctx, BSVM_SOP_DIV); break;
	case BSVM_OPR_MOD: BSVMC_EmitOp(ctx, BSVM_SOP_MOD); break;
	case BSVM_OPR_AND: BSVMC_EmitOp(ctx, BSVM_SOP_AND); break;
	case BSVM_OPR_OR:  BSVMC_EmitOp(ctx, BSVM_SOP_OR); break;
	case BSVM_OPR_XOR: BSVMC_EmitOp(ctx, BSVM_SOP_XOR); break;
	case BSVM_OPR_SHL: BSVMC_EmitOp(ctx, BSVM_SOP_SHL); break;
	case BSVM_OPR_SHR: BSVMC_EmitOp(ctx, BSVM_SOP_SHR); break;

	default:
		BSVMC_EmitOp(ctx, BSVM_SOP_BINARYOP);
		BSVMC_EncByte(ctx, i);
		break;
	}
	return(1);
}

void BSVMC_CompileUnaryTypePrefix(BSVM_SVMCState *ctx, dytf ty)
{
	if(BSVMC_TypeSmallIntP(ctx, ty))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI); return; }
	if(BSVMC_TypeLongP(ctx, ty))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XL); return; }
	if(BSVMC_TypeFloatP(ctx, ty))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XF); return; }
	if(BSVMC_TypeDoubleP(ctx, ty))
		{ BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XD); return; }
		
}

void BSVMC_CompileInferUnaryTypePrefix(BSVM_SVMCState *ctx, dytf l)
{
	dytf ty;

	ty=BSVMC_InferExpr(ctx, l);
	BSVMC_CompileUnaryTypePrefix(ctx, ty);
}

void BSVMC_CompileInferBinaryTypePrefix(BSVM_SVMCState *ctx,
	dytf ln, dytf rn)
{
	dytf lt, rt;

	lt=BSVMC_InferExpr(ctx, ln);
	rt=BSVMC_InferExpr(ctx, rn);
	if(dytfEqP(lt, rt))BSVMC_CompileUnaryTypePrefix(ctx, lt);
}

int BSVMC_CompileUnaryExpr(BSVM_SVMCState *ctx, dytf l)
{
	dytf t, n, ty;
	int i, lc;

	if(dytfEqP(dytfCadr(l), dytfSymbol("&")))
	{
		t=dytfCaddr(l);
		if(dytfSymbolp(t))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_LOADA_S);
			BSVMC_EncIndexLit(ctx, t);
			return(1);
		}
		if(dytfFormIsP(t, "getindex"))
		{
			BSVMC_CompileExpr(ctx, dytfCadr(t));
			BSVMC_CompileExpr(ctx, dytfCaddr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEXA);
			return(1);
		}
		if(dytfFormIsP(t, "objref"))
		{
			BSVMC_CompileExpr(ctx, dytfCadr(t));
//			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//			BSVMC_EncIndexLit(ctx, dytfCaddr(t));
//			BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEXA);

			BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEXA_S);
			BSVMC_EncIndexLit(ctx, dytfCaddr(t));
			return(1);
		}
	}

	if(dytfEqP(dytfCadr(l), dytfSymbol("&&")))
	{
		t=dytfCaddr(l);
		if(dytfSymbolp(t))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_LDJMP);
			BSVMC_EmitJmpAddr(ctx, dytfSymbolv(t));
			return(1);
		}
	}

	n=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
	ty=BSVMC_InferExprClean(ctx, n, &lc);
	BSVMC_CompileExpr(ctx, n);

//	BSVMC_CompileExpr(ctx, dytfCaddr(l));

	i=-1;
	if(dytfEqP(dytfCadr(l), dytfSymbol("+")))i=BSVM_OPR_POS;
	if(dytfEqP(dytfCadr(l), dytfSymbol("-")))i=BSVM_OPR_NEG;
	if(dytfEqP(dytfCadr(l), dytfSymbol("~")))i=BSVM_OPR_NOT;
	if(dytfEqP(dytfCadr(l), dytfSymbol("!")))i=BSVM_OPR_LNOT;
	if(dytfEqP(dytfCadr(l), dytfSymbol("*")))i=BSVM_OPR_UMUL;
	if(dytfEqP(dytfCadr(l), dytfSymbol("/")))i=BSVM_OPR_UDIV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("\\")))i=BSVM_OPR_UIDIV;
	if(dytfEqP(dytfCadr(l), dytfSymbol("%")))i=BSVM_OPR_UMOD;
	if(dytfEqP(dytfCadr(l), dytfSymbol("&")))i=BSVM_OPR_UAND;
	if(dytfEqP(dytfCadr(l), dytfSymbol("|")))i=BSVM_OPR_UOR;
	if(dytfEqP(dytfCadr(l), dytfSymbol("^")))i=BSVM_OPR_UXOR;
	if(dytfEqP(dytfCadr(l), dytfSymbol("&&")))i=BSVM_OPR_ULAND;
	if(dytfEqP(dytfCadr(l), dytfSymbol("||")))i=BSVM_OPR_ULOR;
	if(dytfEqP(dytfCadr(l), dytfSymbol("^^")))i=BSVM_OPR_ULXOR;

	if(i<0)
	{
		BSVMC_CompileUnaryTypePrefix(ctx, ty);
		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP_S);
		BSVMC_EncIDX(ctx, i);
		return(1);
	}

	switch(i)
	{
	case BSVM_OPR_NEG:
		BSVMC_CompileUnaryTypePrefix(ctx, ty);
		BSVMC_EmitOp(ctx, BSVM_SOP_NEG);
		break;
	case BSVM_OPR_NOT:
		BSVMC_CompileUnaryTypePrefix(ctx, ty);
		BSVMC_EmitOp(ctx, BSVM_SOP_NOT);
		break;
	case BSVM_OPR_LNOT:
		BSVMC_CompileUnaryTypePrefix(ctx, ty);
		BSVMC_EmitOp(ctx, BSVM_SOP_LNOT);
		break;
	case BSVM_OPR_UMUL:
		BSVMC_CompileUnaryTypePrefix(ctx, ty);
		BSVMC_EmitOp(ctx, BSVM_SOP_DEFER);
		break;
//	case BSVM_OPR_INC: BSVMC_EmitOp(ctx, BSVM_SOP_INC); break;
//	case BSVM_OPR_DEC: BSVMC_EmitOp(ctx, BSVM_SOP_DEC); break;
	default:
		BSVMC_CompileUnaryTypePrefix(ctx, ty);
		BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
		BSVMC_EncByte(ctx, i);
		break;
	}

//	BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
//	BSVMC_EncByte(ctx, i);
	return(1);
}

void BSVMC_CompileGetIndexTypePrefix(BSVM_SVMCState *ctx, dytf l)
{
	dytf t, u, t2;
	char *s;
	int i;

	t=BSVMC_InferExpr(ctx, dytfCadr(l));
	if(dytfNullP(t))return;
	u=BSVMC_TypeArrayBaseType(ctx, t);
	if(dtrfNullP(u))return;

	t2=BSVMC_InferExpr(ctx, dytfCaddr(l));

#ifdef BSVM_USE_BVT
	if(BSVMC_TypeSmallIntP(ctx, u))
	{
		i=BSVM_SOP_PF_HINT_XI;

		s=dytfSymbolv(u);

		if(BSVMC_TypeSmallIntP(ctx, t2))
		{
			if(s && !strcmp(s, "byte"))		i=BSVM_SOP_PF_HINT_XUBI_IX;
			if(s && !strcmp(s, "sbyte"))	i=BSVM_SOP_PF_HINT_XSBI_IX;
			if(s && !strcmp(s, "ushort"))	i=BSVM_SOP_PF_HINT_XUSI_IX;
			if(s && !strcmp(s, "short"))	i=BSVM_SOP_PF_HINT_XSSI_IX;
			if(s && !strcmp(s, "uint"))		i=BSVM_SOP_PF_HINT_XUDI_IX;
			if(s && !strcmp(s, "int"))		i=BSVM_SOP_PF_HINT_XSDI_IX;
		}else
		{
			if(s && !strcmp(s, "byte"))		i=BSVM_SOP_PF_HINT_XUBI;
			if(s && !strcmp(s, "sbyte"))	i=BSVM_SOP_PF_HINT_XSBI;
			if(s && !strcmp(s, "ushort"))	i=BSVM_SOP_PF_HINT_XUSI;
			if(s && !strcmp(s, "short"))	i=BSVM_SOP_PF_HINT_XSSI;
			if(s && !strcmp(s, "uint"))		i=BSVM_SOP_PF_HINT_XUDI;
			if(s && !strcmp(s, "int"))		i=BSVM_SOP_PF_HINT_XSDI;
		}

		BSVMC_EmitOp(ctx, i);
		return;
	}

	if(BSVMC_TypeLongP(ctx, u))
	{
		i=BSVM_SOP_PF_HINT_XLL;
		if(BSVMC_TypeSmallIntP(ctx, t2))
			i=BSVM_SOP_PF_HINT_XLL_IX;
		BSVMC_EmitOp(ctx, i);
		return;
	}

	if(BSVMC_TypeFloatP(ctx, u))
	{
		i=BSVM_SOP_PF_HINT_XFF;
		if(BSVMC_TypeSmallIntP(ctx, t2))
			i=BSVM_SOP_PF_HINT_XFF_IX;
		BSVMC_EmitOp(ctx, i);
		return;
	}

	if(BSVMC_TypeDoubleP(ctx, u))
	{
		i=BSVM_SOP_PF_HINT_XDD;
		if(BSVMC_TypeSmallIntP(ctx, t2))
			i=BSVM_SOP_PF_HINT_XDD_IX;
		BSVMC_EmitOp(ctx, i);
		return;
	}
#endif
}

void BSVMC_CompileGetIndex(BSVM_SVMCState *ctx, dytf l)
{
	dytf t;
	int i;

	if(dytfIntP(dytfCaddr(l)))
	{
		i=dytfIntv(dytfCaddr(l));
		if((i>=0) && (i<4))
		{
			BSVMC_CompileExpr(ctx, dytfCadr(l));
			BSVMC_CompileGetIndexTypePrefix(ctx, l);
			BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_0+i);
			return;
		}
		if((i>=0) && (i<255))
		{
			BSVMC_CompileExpr(ctx, dytfCadr(l));
			BSVMC_CompileGetIndexTypePrefix(ctx, l);
			BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX_I);
			BSVMC_EncByte(ctx, i);
			return;
		}
	}

	BSVMC_CompileExpr(ctx, dytfCadr(l));
	BSVMC_CompileExpr(ctx, dytfCaddr(l));
	BSVMC_CompileGetIndexTypePrefix(ctx, l);
	BSVMC_EmitOp(ctx, BSVM_SOP_LOADINDEX);
	return;
}

int BSVMC_CompileSetIndexType(BSVM_SVMCState *ctx, dytf l, int fl)
{
	dytf t, u, ta, ti, tv, na, ni, nv;
	char *s;
	int i, j;

//	na=dytfCadr(l);
//	ni=dytfCaddr(l);
//	nv=dytfCadddr(l);

	na=BSVMC_ReduceExpr(ctx, dytfCadr(l));
	ni=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
	nv=BSVMC_ReduceExpr(ctx, dytfCadddr(l));

	t=BSVMC_InferExpr(ctx, na);
	if(dytfNullP(t))
		return(0);
	ta=BSVMC_TypeArrayBaseType(ctx, t);
//	if(!ta)return(0);

	ti=BSVMC_InferExpr(ctx, ni);
	tv=BSVMC_InferExpr(ctx, nv);

#ifdef BSVM_USE_BVT
	if(!dytfNullP(ta) && dytfIntP(ti))
	{
		i=-1;

		if(BSVMC_TypeSmallIntP(ctx, ta))
			i=BSVM_SOP_PF_HINT_XII;
		if(BSVMC_TypeLongP(ctx, ta))
			i=BSVM_SOP_PF_HINT_XLL;
		if(BSVMC_TypeFloatP(ctx, ta))
			i=BSVM_SOP_PF_HINT_XFF;
		if(BSVMC_TypeDoubleP(ctx, ta))
			i=BSVM_SOP_PF_HINT_XDD;

		s=dytfSymbolv(ta);
		if(s && !strcmp(s, "byte"))		i=BSVM_SOP_PF_HINT_XUBI;
		if(s && !strcmp(s, "sbyte"))	i=BSVM_SOP_PF_HINT_XSBI;
		if(s && !strcmp(s, "ushort"))	i=BSVM_SOP_PF_HINT_XUSI;
		if(s && !strcmp(s, "short"))	i=BSVM_SOP_PF_HINT_XSSI;
		if(s && !strcmp(s, "uint"))		i=BSVM_SOP_PF_HINT_XUDI;
		if(s && !strcmp(s, "int"))		i=BSVM_SOP_PF_HINT_XSDI;

		j=dytfIntv(ti);
		if((j>=0) && (j<4))
		{
			BSVMC_CompileExprCast(ctx, nv, ta);
			BSVMC_CompileExpr(ctx, na);

			if(i>0)BSVMC_EmitOp(ctx, i);
			BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_0+j);
			return(1);
		}
		if((j>=0) && (j<255))
		{
			BSVMC_CompileExprCast(ctx, nv, ta);
			BSVMC_CompileExpr(ctx, na);

			if(i>0)BSVMC_EmitOp(ctx, i);
			BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX_I);
			BSVMC_EncByte(ctx, j);
			return(1);
		}
	}

	if(!dytfNullP(ta) && BSVMC_TypeSmallIntP(ctx, ta))
	{
		i=BSVM_SOP_PF_HINT_XI;

		s=dytfSymbolv(ta);
		if(BSVMC_TypeSmallIntP(ctx, ti))
		{
			if(s && !strcmp(s, "byte"))		i=BSVM_SOP_PF_HINT_XUBI_IX;
			if(s && !strcmp(s, "sbyte"))	i=BSVM_SOP_PF_HINT_XSBI_IX;
			if(s && !strcmp(s, "ushort"))	i=BSVM_SOP_PF_HINT_XUSI_IX;
			if(s && !strcmp(s, "short"))	i=BSVM_SOP_PF_HINT_XSSI_IX;
			if(s && !strcmp(s, "uint"))		i=BSVM_SOP_PF_HINT_XUDI_IX;
			if(s && !strcmp(s, "int"))		i=BSVM_SOP_PF_HINT_XSDI_IX;
		}else
		{
			if(s && !strcmp(s, "byte"))		i=BSVM_SOP_PF_HINT_XUBI;
			if(s && !strcmp(s, "sbyte"))	i=BSVM_SOP_PF_HINT_XSBI;
			if(s && !strcmp(s, "ushort"))	i=BSVM_SOP_PF_HINT_XUSI;
			if(s && !strcmp(s, "short"))	i=BSVM_SOP_PF_HINT_XSSI;
			if(s && !strcmp(s, "uint"))		i=BSVM_SOP_PF_HINT_XUDI;
			if(s && !strcmp(s, "int"))		i=BSVM_SOP_PF_HINT_XSDI;
		}

		BSVMC_CompileExprCast(ctx, nv, ta);
		BSVMC_CompileExpr(ctx, na);
		BSVMC_CompileExpr(ctx, ni);
		BSVMC_EmitOp(ctx, i);
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
		return(1);
	}
#endif

	return(0);
}

void BSVMC_CompileSetIndexStmt(BSVM_SVMCState *ctx, dytf l)
{
	if(BSVMC_CompileSetIndexType(ctx, l, 0))
		return;

//	*(int *)-1=-1;

	BSVMC_CompileExpr(ctx, dytfCadddr(l));
	BSVMC_CompileExpr(ctx, dytfCadr(l));
	BSVMC_CompileExpr(ctx, dytfCaddr(l));
	BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
	return;
}
