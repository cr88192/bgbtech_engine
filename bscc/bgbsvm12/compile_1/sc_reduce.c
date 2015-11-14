#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbsvm.h>


int BSVMC_BoolExpr(BSVM_SVMCState *ctx, dytf l)
{
	if(dytfEqP(l, dytfInt(0)))return(0);
	if(dytfEqP(l, dytfFloat(0)))return(0);
	if(dytfEqP(l, DYTF_FALSE))return(0);

	if(dytfIntP(l))return(1);
	if(dytfFloatP(l))return(1);
	if(dytfEqP(l, DYTF_TRUE))return(1);

	return(-1);

//	return(dytruep(l));
}

int BSVMC_IsExpr(dytf l)
{
	if(dytfConsP(l))return(1);
	if(dytfSymbolP(l))return(1);
	return(0);
}

dytf BSVMC_ReduceForm(BSVM_SVMCState *ctx, dytf l)
{
	dytf c, t, x;
	int i, j;

	if(dytfFormIsP(l, "unary"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
		if(BSVMC_IsExpr(t))return(l);

		if(BSVMC_FormCdrIsP(l, "!"))
		{
			if(dytfFormIsP(t, "=="))return(dytfConss("!=", dytfCdr(t)));
			if(dytfFormIsP(t, "!="))return(dytfConss("==", dytfCdr(t)));
			if(dytfFormIsP(t, "==="))return(dytfConss("!==", dytfCdr(t)));
			if(dytfFormIsP(t, "!=="))return(dytfConss("===", dytfCdr(t)));

			if(dytfFormIsP(t, "<"))return(dytfConss(">=", dytfCdr(t)));
			if(dytfFormIsP(t, ">"))return(dytfConss("<=", dytfCdr(t)));
			if(dytfFormIsP(t, "<="))return(dytfConss(">", dytfCdr(t)));
			if(dytfFormIsP(t, ">="))return(dytfConss("<", dytfCdr(t)));
		}

		if(BSVMC_FormCdrIsP(l, "+"))i=BSVM_OPR_POS;
		if(BSVMC_FormCdrIsP(l, "-"))i=BSVM_OPR_NEG;
		if(BSVMC_FormCdrIsP(l, "~"))i=BSVM_OPR_NOT;
		if(BSVMC_FormCdrIsP(l, "!"))i=BSVM_OPR_LNOT;
		if(BSVMC_FormCdrIsP(l, "*"))i=BSVM_OPR_UMUL;
		if(BSVMC_FormCdrIsP(l, "/"))i=BSVM_OPR_UDIV;
		if(BSVMC_FormCdrIsP(l, "\\"))i=BSVM_OPR_UIDIV;
		if(BSVMC_FormCdrIsP(l, "%"))i=BSVM_OPR_UMOD;
		if(BSVMC_FormCdrIsP(l, "&"))i=BSVM_OPR_UAND;
		if(BSVMC_FormCdrIsP(l, "|"))i=BSVM_OPR_UOR;
		if(BSVMC_FormCdrIsP(l, "^"))i=BSVM_OPR_UXOR;
		if(BSVMC_FormCdrIsP(l, "&&"))i=BSVM_OPR_ULAND;
		if(BSVMC_FormCdrIsP(l, "||"))i=BSVM_OPR_ULOR;
		if(BSVMC_FormCdrIsP(l, "^^"))i=BSVM_OPR_ULXOR;

		if(i>=0)
		{
			c=BSVM_Unary(NULL, i, t);
			if(!dytfNullOrUndefinedP(c))
				return(c);
		}

//		l=BSVM_Opr_Unary(i, t);
		return(l);
	}

#if 0
	if((dytfFormIsP(l, "+")) || (dytfFormIsP(l, "-")) ||
		(dytfFormIsP(l, "*")) || (dytfFormIsP(l, "/")) ||
		(dytfFormIsP(l, "%")) || (dytfFormIsP(l, "&")) ||
		(dytfFormIsP(l, "|")) || (dytfFormIsP(l, "^")) ||
		(dytfFormIsP(l, "**")) || (dytfFormIsP(l, "\\")))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		if(BSVMC_IsExpr(t))return(l);

		if(dytfFormIsP(l, "+"))i=BSVM_OPR_ADD;
		if(dytfFormIsP(l, "-"))i=BSVM_OPR_SUB;
		if(dytfFormIsP(l, "*"))i=BSVM_OPR_MUL;
		if(dytfFormIsP(l, "/"))i=BSVM_OPR_DIV;
		if(dytfFormIsP(l, "%"))i=BSVM_OPR_MOD;
		if(dytfFormIsP(l, "&"))i=BSVM_OPR_AND;
		if(dytfFormIsP(l, "|"))i=BSVM_OPR_OR;
		if(dytfFormIsP(l, "^"))i=BSVM_OPR_XOR;
		if(dytfFormIsP(l, "\\"))i=BSVM_OPR_IDIV;
		if(dytfFormIsP(l, "**"))i=BSVM_OPR_EXP;

		x=t; c=dytfCddr(l);
		while(dytfConsP(c))
		{
			t=BSVMC_ReduceExpr(ctx, dytfCar(c));
			if(BSVMC_IsExpr(t))return(l);

			x=BSVM_Opr_Binary(i, x, t);

			c=dytfCdr(c);
		}
		return(x);
	}

	if((dytfFormIsP(l, "<<")) || (dytfFormIsP(l, ">>")) ||
		(dytfFormIsP(l, "==")) || (dytfFormIsP(l, "===")) ||
		(dytfFormIsP(l, "!=")) || (dytfFormIsP(l, "!==")) ||
		(dytfFormIsP(l, "<")) || (dytfFormIsP(l, ">")) ||
		(dytfFormIsP(l, "<=")) || (dytfFormIsP(l, ">=")))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		if(BSVMC_IsExpr(t))return(l);

		x=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
		if(BSVMC_IsExpr(x))return(l);


		if(dytfFormIsP(l, "<<"))i=BSVM_OPR_SHL;
		if(dytfFormIsP(l, ">>"))i=BSVM_OPR_SHR;
		if(dytfFormIsP(l, "=="))i=BSVM_OPR_EQV;
		if(dytfFormIsP(l, "!="))i=BSVM_OPR_NEQV;
		if(dytfFormIsP(l, "==="))i=BSVM_OPR_EQ;
		if(dytfFormIsP(l, "!=="))i=BSVM_OPR_NEQ;
		if(dytfFormIsP(l, "<"))i=BSVM_OPR_L;
		if(dytfFormIsP(l, ">"))i=BSVM_OPR_G;
		if(dytfFormIsP(l, "<="))i=BSVM_OPR_LE;
		if(dytfFormIsP(l, ">="))i=BSVM_OPR_GE;

		l=BSVM_Opr_Binary(i, t, x);
		return(l);
	}
#endif

	if(BSVMC_FormIs(l, "binary"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
		if(BSVMC_IsExpr(t))return(l);

		x=BSVMC_ReduceExpr(ctx, dytfCadddr(l));
		if(BSVMC_IsExpr(x))return(l);

		i=-1;
		if(BSVMC_FormCdrIsP(l, "+"))i=BSVM_OPR_ADD;
		if(BSVMC_FormCdrIsP(l, "-"))i=BSVM_OPR_SUB;
		if(BSVMC_FormCdrIsP(l, "*"))i=BSVM_OPR_MUL;
		if(BSVMC_FormCdrIsP(l, "/"))i=BSVM_OPR_DIV;
		if(BSVMC_FormCdrIsP(l, "%"))i=BSVM_OPR_MOD;
		if(BSVMC_FormCdrIsP(l, "&"))i=BSVM_OPR_AND;
		if(BSVMC_FormCdrIsP(l, "|"))i=BSVM_OPR_OR;
		if(BSVMC_FormCdrIsP(l, "^"))i=BSVM_OPR_XOR;
		if(BSVMC_FormCdrIsP(l, "\\"))i=BSVM_OPR_IDIV;
		if(BSVMC_FormCdrIsP(l, "**"))i=BSVM_OPR_EXP;

		if(BSVMC_FormCdrIsP(l, "<<"))i=BSVM_OPR_SHL;
		if(BSVMC_FormCdrIsP(l, ">>"))i=BSVM_OPR_SHR;
		if(BSVMC_FormCdrIsP(l, "=="))i=BSVM_OPR_EQV;
		if(BSVMC_FormCdrIsP(l, "!="))i=BSVM_OPR_NEQV;
		if(BSVMC_FormCdrIsP(l, "==="))i=BSVM_OPR_EQ;
		if(BSVMC_FormCdrIsP(l, "!=="))i=BSVM_OPR_NEQ;
		if(BSVMC_FormCdrIsP(l, "<"))i=BSVM_OPR_L;
		if(BSVMC_FormCdrIsP(l, ">"))i=BSVM_OPR_G;
		if(BSVMC_FormCdrIsP(l, "<="))i=BSVM_OPR_LE;
		if(BSVMC_FormCdrIsP(l, ">="))i=BSVM_OPR_GE;


//		if((BSVMC_FormCdrIsP(l, "/")) && dyfixintp(t) && dyfixintp(x))
//			return(BSVM_RATIONAL(TOINT(t), TOINT(x)));

		if((BSVMC_FormCdrIsP(l, "/")) && dytfFixintP(t) && dytfFixintP(x))
			return(dytfFloat(((double)dytfIntv(t))/dytfIntv(x)));

//		if((i>0) && dySmallQuatp(t) && dySmallQuatp(x))
		if(i>=0)
		{
			c=BSVM_Binary(NULL, i, t, x);
			if(!dytfNullOrUndefinedP(c))
				return(c);
		}

//		l=BSVM_Opr_Binary(i, t, x);
		return(l);
	}

	if(dytfFormIsP(l, "if"))
	{
#if 1
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			l=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
			return(l);
		}
		if(i==0)
		{
			t=DYTF_NULL;
			if(dytfConsP(dytfCdddr(l)))
				t=BSVMC_ReduceExpr(ctx, dytfCadddr(l));
			return(t);
		}
#endif
		return(l);
	}

	if(dytfFormIsP(l, "begin"))
	{
//		if((dytfCddr(l)==BGBDY_NULL) &&
//			(BSVMC_FormIs(dytfCadr(l), "unary") ||
//			BSVMC_FormIs(dytfCadr(l), "binary")))

		if(dytfNullP(dytfCdr(l)))
			return(DYTF_NULL);

		if(dytfNullP(dytfCddr(l)))
		{
			t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
			return(t);
		}

		return(l);
	}

	if(dytfFormIsP(l, "sizeof"))
	{
		i=BSVMC_InferTypeBaseSize(dytfCadr(l));
		if(i>=0)return(dytfInt(i));
		return(l);
	}

	return(l);
}

char *BSVMC_LookupPPKey(BSVM_SVMCState *ctx, char *name)
{
	char tb[256];
	char *s;

	sprintf(tb, "CPreProc/%s", name);
	s=dyllMetaLookupKey(tb);
	return(s);
}

dytf BSVMC_ReduceExpr(BSVM_SVMCState *ctx, dytf l)
{
	char tb[256];
	dytf t, cn, cv, ct;
	char *s, *name;

	if(dytfConsP(l))
	{
		t=BSVMC_ReduceForm(ctx, l);
		return(t);
	}

	if(dytfSymbolP(l))
	{
		if(BSVMC_SymbolIsP(l, "I"))return(dytfComplex2(0, 1));
		if(BSVMC_SymbolIsP(l, "PI"))return(dytfFloat(3.14159265358979323846));
		if(BSVMC_SymbolIsP(l, "TAU"))return(dytfFloat(2*3.14159265358979323846));
		if(BSVMC_SymbolIsP(l, "E"))return(dyfloat(2.7182818284590452354));
		if(BSVMC_SymbolIsP(l, "NULL"))return(dytfSymbol("null"));

		if(BSVMC_SymbolIsP(l, "this"))return(l);
		if(BSVMC_SymbolIsP(l, "true"))return(l);
		if(BSVMC_SymbolIsP(l, "false"))return(l);
		if(BSVMC_SymbolIsP(l, "null"))return(l);
//		if(BSVMC_SymbolIsP(l, "undefined"))return(l);

		cn=dytfWrap(ctx->lenv);
		ct=dytfWrap(ctx->tenv);
		cv=dytfWrap(ctx->venv);

		if(ctx->cap)return(l);

		while(dytfConsP(cn) && (cn!=ctx->lenv))
		{
			if((dytfCar(cn)==l) && !dytfNullP(dytfCar(cv)))
				return(dytfCar(cv));

			cn=dytfCdr(cn);
			ct=dytfCdr(ct);
			cv=dytfCdr(cv);
		}

		name=dytfSymbolv(l);

		s=BSVMC_LookupPPKey(ctx, name);
		if(s && !strcmp(s, "define"))
		{
			sprintf(tb, "%s:value", name);
			s=BSVMC_LookupPPKey(ctx, tb);
			if(!s)s="";

			BSVMP_SetLinenum("<ppexpr>", s, 1);
			t=BSVMP_Expression(&s);
			t=BSVMC_ReduceExpr(ctx, t);
			return(t);

//			BGBPP_AddDefine(ctx, name, s);
//			cur=BGBPP_LookupDefine(ctx, name);
//			return(cur);
		}
	}
	return(l);
}


dytf BSVMC_InferTypeBase(dytf l)
{
	char b[64];
	char *s, *t;

	if(!dytfKeywordP(l) && !dytfSymbolP(l))
		return(l);
//		return(BGBDY_NULL);

	s=dytfSymbolv(l); t=b;
	while(*s && (*s!='$'))*t++=*s++;
	*t++=0;

	return(dytfKeyword(b));
}

char *BSVMC_FlattenQNameBaseSig(dytf l)
{
	char tb[256];
	char *s;
	dytf t;
	int i;

	if(dytfKeywordP(l) || dytfSymbolP(l))
	{
		s=dytfSymbolv(l);
		return(s);
	}

	if(BSVMC_FormIs(l, "objref"))
	{
		s=BSVMC_FlattenQNameBaseSig(dytfCadr(l));
		if(!s)return(NULL);
		t=dytfCaddr(l);
		if(dytfSymbolP(t))
			return(gcrsprint("%s/%s", s, dytfSymbolv(t)));
		return(NULL);
	}
	return(NULL);
}

int BSVMC_InferTypeBaseSize(dytf l)
{
	char tb[256];
	char *s;
	dytf t;
	int i;

	if(dytfKeywordP(l) || dytfSymbolP(l))
	{
		s=dytfSymbolv(l);
		if(!strcmp(s, "void"))return(0);
		if(!strcmp(s, "byte"))return(1);
		if(!strcmp(s, "sbyte"))return(1);
		if(!strcmp(s, "bool"))return(1);
		if(!strcmp(s, "boolean"))return(1);
		if(!strcmp(s, "char8"))return(1);
		if(!strcmp(s, "char"))return(2);
		if(!strcmp(s, "char16"))return(2);
		if(!strcmp(s, "short"))return(2);
		if(!strcmp(s, "ushort"))return(2);
		if(!strcmp(s, "int"))return(4);
		if(!strcmp(s, "uint"))return(4);
		if(!strcmp(s, "long"))return(8);
		if(!strcmp(s, "ulong"))return(8);
		if(!strcmp(s, "int128"))return(16);
		if(!strcmp(s, "uint128"))return(16);
		if(!strcmp(s, "float"))return(4);
		if(!strcmp(s, "double"))return(8);
		if(!strcmp(s, "float128"))return(16);

//		if(!strcmp(s, "var"))return("r");
//		if(!strcmp(s, "variant"))return("r");
//		if(!strcmp(s, "object"))return("r");
//		if(!strcmp(s, "string"))return("r");

//		if(!strcmp(s, "vec2"))return("Ca");
//		if(!strcmp(s, "vec3"))return("Cb");
//		if(!strcmp(s, "vec4"))return("Cc");
//		if(!strcmp(s, "quat"))return("Cq");

//		return(gcrsprint("U%s;", s));

		return(-1);
	}

	return(-1);
}

char *BSVMC_InferTypeBaseSig(dytf l)
{
	char tb[256];
	char *s;
	dytf t;
	int i;

	if(dytfKeywordP(l) || dytfSymbolP(l))
	{
		s=dytfSymbolv(l);
		if(!strcmp(s, "void"))return("v");
		if(!strcmp(s, "byte"))return("h");
		if(!strcmp(s, "sbyte"))return("a");
		if(!strcmp(s, "bool"))return("b");
		if(!strcmp(s, "boolean"))return("b");
		if(!strcmp(s, "char8"))return("c");
		if(!strcmp(s, "char"))return("w");
		if(!strcmp(s, "char16"))return("w");
		if(!strcmp(s, "short"))return("s");
		if(!strcmp(s, "ushort"))return("t");
		if(!strcmp(s, "int"))return("i");
		if(!strcmp(s, "uint"))return("j");
		if(!strcmp(s, "long"))return("x");
		if(!strcmp(s, "ulong"))return("y");
		if(!strcmp(s, "int128"))return("n");
		if(!strcmp(s, "uint128"))return("o");
		if(!strcmp(s, "float"))return("f");
		if(!strcmp(s, "double"))return("d");
		if(!strcmp(s, "float128"))return("g");

		if(!strcmp(s, "var"))return("r");
		if(!strcmp(s, "variant"))return("r");
//		if(!strcmp(s, "object"))return("r");
//		if(!strcmp(s, "string"))return("r");

		if(!strcmp(s, "object"))return("Co");
		if(!strcmp(s, "string"))return("Cs");

		if(!strcmp(s, "fixnum"))return("r");
		if(!strcmp(s, "flonum"))return("r");

		if(!strcmp(s, "vec2"))return("Ca");
		if(!strcmp(s, "vec3"))return("Cb");
		if(!strcmp(s, "vec4"))return("Cc");
		if(!strcmp(s, "quat"))return("Cq");

		if(!strcmp(s, "vec2f"))return("Ca");
		if(!strcmp(s, "vec3f"))return("Cb");
		if(!strcmp(s, "vec4f"))return("Cc");
		if(!strcmp(s, "quatf"))return("Cq");

		if(!strcmp(s, "vec2d"))return("Da");
		if(!strcmp(s, "vec3d"))return("Db");
		if(!strcmp(s, "vec4d"))return("Dc");
		if(!strcmp(s, "quatd"))return("Dq");

		return(gcrsprint("U%s;", s));
	}

	if(BSVMC_FormIsUnary(l, "*"))
	{
		s=BSVMC_InferTypeBaseSig(dytfCaddr(l));
		return(gcrsprint("P%s", s));
	}

	if(BSVMC_FormIsUnary(l, "&"))
	{
		s=BSVMC_InferTypeBaseSig(dytfCaddr(l));
		return(gcrsprint("R%s", s));
	}

	if(BSVMC_FormIs(l, "emptyindex"))
	{
		s=BSVMC_InferTypeBaseSig(dytfCadr(l));
		return(gcrsprint("Q%s", s));
	}

	if(BSVMC_FormIs(l, "getindex"))
	{
		t=dytfCaddr(l);

		if(dytfSymbolP(t))
		{
			s=BSVMC_FlattenQNameBaseSig(l);
			if(s)return(gcrsprint("U%s;", s));
			return(DYTF_NULL);
		}
	
		if(dytfIntP(t))
		{
			s=BSVMC_InferTypeBaseSig(dytfCadr(l));
			i=dyintv(t);
			return(gcrsprint("A%d;%s", i, s));
		}
		return(DYTF_NULL);
	}

	s=BSVMC_FlattenQNameBaseSig(l);
	if(s)return(gcrsprint("U%s;", s));

	return(DYTF_NULL);
}

char *BSVMC_InferVarBaseSig(dytf n)
{
	char *s;
	dytf u, v;

	u=NULL; v=NULL;

	if(dytfFormIsP(n, "set!"))
	{
		v=dytfCaddr(n);
		n=dytfCadr(n);				
	}
	
	if(dytfFormIsP(n, "cast"))
	{
		u=dytfCadr(n);
		n=dytfCaddr(n);				
	}
		
	s=BSVMC_InferTypeBaseSig(u);
	return(s);
}

char *BSVMC_InferVarsListBaseSig(dytf l)
{
	char tb[256];
	char *s, *t;
	dytf c, n, u, v;
	int i;

	c=l; t=tb;
	while(dytfConsP(c))
	{
		n=dytfCar(c);
		c=dytfCdr(c);

		s=BSVMC_InferVarBaseSig(n);
		if(!s)s="r";
		
		strcpy(t, s);
		t+=strlen(t);
	}
	
	*t++=0;
	
	return(gcrstrdup(tb));
}

char *BSVMC_InferArgsBaseSig(dytf l)
{
	char tb[256];
	char *s, *t;
	dytf c, n, u, v;
	
	t=tb;
	*t++='(';
	s=BSVMC_InferVarsListBaseSig(dytfCdr(l));
	strcpy(t, s);
	t+=strlen(t);
	*t++=')';

	n=dytfCar(l);
	s=BSVMC_InferVarBaseSig(n);
	if(!s)s="r";
		
	strcpy(t, s);
	t+=strlen(t);

	*t++=0;

	return(gcrstrdup(tb));
}

dytf BSVMC_InferArgsBaseSig2(dytf l)
{
	char *s;

	s=BSVMC_InferTypeBaseSig(l);
	if(!s)return(dytfSymbol("r"));
	return(dytfSymbol(s));
}


char *BSVMC_InferExprListBaseSig(BSVM_SVMCState *ctx, dytf l)
{
	char tb[256];
	char *s, *t;
	dytf c, n, u, v;
	int i;

	c=l; t=tb;
	while(dytfConsP(c))
	{
		n=dytfCar(c);
		c=dytfCdr(c);

		v=BSVMC_ReduceExpr(ctx, n);
		u=BSVMC_InferExpr(ctx, v);

		s=BSVMC_InferTypeBaseSig(u);
//		s=BSVMC_InferVarBaseSig(n);
		if(!s)s="r";
		
		strcpy(t, s);
		t+=strlen(t);
	}
	
	*t++=0;
	
	return(gcrstrdup(tb));
}

char *BSVMC_InferCallArgsBaseSig(BSVM_SVMCState *ctx, dytf l, dytf rt)
{
	char tb[256];
	char *s, *t;
	dytf c, n, u, v;
	
	t=tb;
	*t++='(';
	s=BSVMC_InferExprListBaseSig(ctx, dytfCdr(l));
	strcpy(t, s);
	t+=strlen(t);
	*t++=')';

//	n=dytfCar(l);
//	s=BSVMC_InferVarBaseSig(n);
	s=NULL;
	if(rt) { s=BSVMC_InferTypeBaseSig(rt); }
	if(!s)s="r";
		
	strcpy(t, s);
	t+=strlen(t);

	*t++=0;

	return(gcrstrdup(tb));
}

char *BSVMC_InferMethodCallArgsBaseSig(BSVM_SVMCState *ctx, dytf l, dytf rt)
{
	char tb[256];
	char *s, *t;
	dytf c, n, u, v;
	
	t=tb;
	*t++='(';
	s=BSVMC_InferExprListBaseSig(ctx, dytfCddr(l));
	strcpy(t, s);
	t+=strlen(t);
	*t++=')';

//	n=dytfCar(l);
//	s=BSVMC_InferVarBaseSig(n);
	s=NULL;
	if(rt) { s=BSVMC_InferTypeBaseSig(rt); }
	if(!s)s="r";
		
	strcpy(t, s);
	t+=strlen(t);

	*t++=0;

	return(gcrstrdup(tb));
}


dytf BSVMC_BuildVarInfo(dytf n)
{
	BSVM_SVMVarInfo *inf;
	char *s;
	dytf u, v;
	int rst, ref;

	u=NULL; v=NULL; rst=0; ref=0;

	if(dytfFormIsP(n, "byval"))
	{
		n=dytfCadr(n);
	}

	if(dytfFormIsP(n, "byref"))
	{
		n=dytfCadr(n);
		ref=1;				
	}

	if(dytfFormIsP(n, "rest"))
	{
//		v=dytfCaddr(n);
		n=dytfCadr(n);
		rst=1;				
	}

	if(dytfFormIsP(n, "set!"))
	{
		v=dytfCaddr(n);
		n=dytfCadr(n);				
	}
	
	if(dytfFormIsP(n, "cast"))
	{
		u=dytfCadr(n);
		n=dytfCaddr(n);
	}

	if(!u && !v && !rst)return(n);

	s=BSVMC_InferTypeBaseSig(u);
	
	if(rst)s="Cz";
	
	inf=gctalloc("svm_varinfo_t", sizeof(BSVM_SVMVarInfo));
	inf->name=dyllString(dytfSymbolv(n));
	inf->sig=dyllString(s);
//	inf->flags=fl;
	inf->value=dytfWrap(v);
	
	if(ref)
	{
		inf->flags|=BGBDYC_FLAG_BYREF;
	}
	
	return((dyt)inf);
}



int BSVMC_InferIsIntP(dytf t)
{
	if(t==dykeyword("byte"))return(1);
	if(t==dykeyword("sbyte"))return(1);
	if(t==dykeyword("short"))return(1);
	if(t==dykeyword("ushort"))return(1);
	if(t==dykeyword("int"))return(1);
	if(t==dykeyword("uint"))return(1);
	return(0);
}

int BSVMC_InferIsLongP(dytf t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(t==dykeyword("long"))return(1);
	if(t==dykeyword("ulong"))return(1);
	return(0);
}

int BSVMC_InferIsFloatP(dytf t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(t==dykeyword("float"))return(1);
	return(0);
}

int BSVMC_InferIsDoubleP(dytf t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(t==dykeyword("float"))return(1);
	if(t==dykeyword("double"))return(1);
	return(0);
}

int BSVMC_InferIsNumP(dytf t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(BSVMC_InferIsLongP(t))return(1);
	if(BSVMC_InferIsFloatP(t))return(1);
	if(BSVMC_InferIsDoubleP(t))return(1);
	return(0);
}

dytf BSVMC_InferIsInt(dytf t)
{
	if(BSVMC_InferIsIntP(t))
		return(dykeyword("int"));
	return(BGBDY_NULL);
}

dytf BSVMC_InferIsLong(dytf t)
{
	if(BSVMC_InferIsIntP(t))
		return(dykeyword("int"));
	if(BSVMC_InferIsLongP(t))
		return(dykeyword("long"));
	return(BGBDY_NULL);
}

dytf BSVMC_InferIsNum(dytf t)
{
	if(BSVMC_InferIsIntP(t))
		return(dykeyword("int"));
	if(BSVMC_InferIsLongP(t))
		return(dykeyword("long"));
	if(BSVMC_InferIsFloatP(t))
		return(dykeyword("float"));
	if(BSVMC_InferIsDoubleP(t))
		return(dykeyword("double"));
	return(BGBDY_NULL);
}

dytf BSVMC_InferForm(BSVM_SVMCState *ctx, dytf l)
{
	dytf c, t, u, x, n, lt, rt, ln, rn;
	int i;

	if(dytfFormIsP(l, "cast"))
	{
//		t=BSVMC_InferExpr(ctx, dytfCaddr(l));
		t=dytfCadr(l);
		t=BSVMC_InferTypeBase(t);
		return(t);
	}

	if((dytfFormIsP(l, "unary")) && (BSVMC_FormCdrIsP(l, "!")))
	{
		t=BSVMC_InferExpr(ctx, dytfCaddr(l));
		return(dykeyword("bool"));
	}

	if((dytfFormIsP(l, "unary")) && (BSVMC_FormCdrIsP(l, "~")))
	{
		t=BSVMC_InferExpr(ctx, dytfCadr(l));
		return(BSVMC_InferIsLong(t));
	}

//	if((dytfFormIsP(l, "-")) && !dytfConsP(dytfCddr(l)))
//	{
//		t=BSVMC_InferExpr(ctx, dytfCadr(l));
//		return(BSVMC_InferIsNum(t));
//	}

	if(BSVMC_FormIs(l, "binary") && BSVMC_FormCdrIsP(l, "/"))
	{
		t=BSVMC_InferExpr(ctx, dytfCaddr(l));
		x=BSVMC_InferExpr(ctx, dytfCadddr(l));

		i=BSVMC_InferIsNumP(t) &&
			BSVMC_InferIsNumP(x);
		if(!i)return(BGBDY_NULL);

		n=dykeyword("float");
		if(t==dykeyword("double"))n=t;
		if(x==dykeyword("double"))n=x;
		return(n);
	}

	if(BSVMC_FormIs(l, "binary") && (BSVMC_FormCdrIsP(l, "\\")))
	{
		t=BSVMC_InferExpr(ctx, dytfCaddr(l));
		x=BSVMC_InferExpr(ctx, dytfCadddr(l));

		return(dykeyword("int"));
	}

//	if((dytfFormIsP(l, "+")) || (dytfFormIsP(l, "-")) ||
//		(dytfFormIsP(l, "*")) ||
//		(dytfFormIsP(l, "%")) || (dytfFormIsP(l, "&")) ||
//		(dytfFormIsP(l, "|")) || (dytfFormIsP(l, "^")))

	if(BSVMC_FormIs(l, "binary") && (
		(BSVMC_FormCdrIsP(l, "+")) || (BSVMC_FormCdrIsP(l, "-")) ||
		(BSVMC_FormCdrIsP(l, "*")) || (BSVMC_FormCdrIsP(l, "%")) ||
		(BSVMC_FormCdrIsP(l, "&")) || (BSVMC_FormCdrIsP(l, "|")) ||
		(BSVMC_FormCdrIsP(l, "^"))))
	{
//		ln=dytfCaddr(l);
//		rn=dytfCadddr(l);

		ln=BSVMC_ReduceExpr(ctx, dytfCaddr(l));
		rn=BSVMC_ReduceExpr(ctx, dytfCadddr(l));
		lt=BSVMC_InferExpr(ctx, ln);
		rt=BSVMC_InferExpr(ctx, rn);

		if(BSVMC_TypeSmallIntP(ctx, lt) && dyintp(rn))
			return(lt);
		if(BSVMC_TypeLongP(ctx, lt) && dylongp(rn))
			return(lt);
		if(BSVMC_TypeFloatP(ctx, lt) && dyrealp(rn))
			return(lt);
		if(BSVMC_TypeDoubleP(ctx, lt) && dyrealp(rn))
			return(lt);

		if(BSVMC_TypeSmallIntP(ctx, lt) && BSVMC_TypeSmallIntP(ctx, rt))
		{
			if(lt==rt)return(lt);
			return(dytfSymbol("int"));
		}

		if(lt==rt)return(lt);
		return(NULL);

#if 0
		t=BSVMC_InferExpr(ctx, dytfCaddr(l));

		x=t; c=dytfCdddr(l);
		while(dytfConsP(c))
		{
			t=BSVMC_InferExpr(ctx, dytfCar(c));
			if(t!=x)return(BGBDY_NULL);
			c=dytfCdr(c);
		}
		return(x);
#endif
	}

	if(BSVMC_FormIs(l, "binary") &&
		((BSVMC_FormCdrIsP(l, "<<")) || (BSVMC_FormCdrIsP(l, ">>")) ||
			(BSVMC_FormCdrIsP(l, ">>>"))))
	{
		t=BSVMC_InferExpr(ctx, dytfCaddr(l));
		x=BSVMC_InferExpr(ctx, dytfCadddr(l));

		i=BSVMC_InferIsIntP(t) &&
			BSVMC_InferIsIntP(x);
		if(i)return(t);
		return(BGBDY_NULL);
	}

//	if((dytfFormIsP(l, "==")) || (dytfFormIsP(l, "===")) ||
//		(dytfFormIsP(l, "!=")) || (dytfFormIsP(l, "!==")) ||
//		(dytfFormIsP(l, "<")) || (dytfFormIsP(l, ">")) ||
//		(dytfFormIsP(l, "<=")) || (dytfFormIsP(l, ">=")))

	if(BSVMC_FormIs(l, "binary") && (
		(BSVMC_FormCdrIsP(l, "==")) || (BSVMC_FormCdrIsP(l, "===")) ||
		(BSVMC_FormCdrIsP(l, "!=")) || (BSVMC_FormCdrIsP(l, "!==")) ||
		(BSVMC_FormCdrIsP(l, "<"))  || (BSVMC_FormCdrIsP(l, ">")) ||
		(BSVMC_FormCdrIsP(l, "<=")) || (BSVMC_FormCdrIsP(l, ">="))))
	{
		return(dykeyword("bool"));
	}

	if(dytfFormIsP(l, "if"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			t=BSVMC_InferExpr(ctx, dytfCaddr(l));
			return(t);
		}
		if(i==0)
		{
			t=BGBDY_NULL;
			if(dytfConsP(dytfCdddr(l)))
				t=BSVMC_InferExpr(ctx, dytfCadddr(l));
			return(t);
		}

		x=BGBDY_NULL;
		t=BSVMC_InferExpr(ctx, dytfCaddr(l));
		if(dytfConsP(dytfCdddr(l)))
			x=BSVMC_InferExpr(ctx, dytfCadddr(l));
		if(t!=x)return(BGBDY_NULL);
		return(t);
	}

	if(dytfFormIsP(l, "funcall"))
	{
#if 0
		dyPrintf("Infer Func: ");
		BSVM_TY_Print(dytfCadr(l));
		dyPrintf("=");
		BSVM_TY_Print(ctx->cf_n);
		dyPrintf("\n");
#endif

		ctx->i_cx_p=0;
		if(dytfCadr(l)==ctx->cf_n)
		{
//			*(int *)-1=-1;
			
			t=BSVMC_InferTypeBase(ctx->cf_ty);
			return(t);
//			return(ctx->cf_ty);
		}
		return(BGBDY_NULL);
	}

	if((dytfFormIsP(l, "setindex!")) || (dytfFormIsP(l, "vector-set!")))
	{
		t=BSVMC_InferExpr(ctx, dytfCadddr(l));
		return(t);
	}

	if((dytfFormIsP(l, "getindex")) || (dytfFormIsP(l, "vector-ref")))
	{
		t=BSVMC_InferExpr(ctx, dytfCadr(l));
		if(!t)
			return(NULL);
		u=BSVMC_TypeArrayBaseType(ctx, t);
		return(u);
	}

	ctx->i_cx_p=0;
	return(BGBDY_NULL);
}

dytf BSVMC_InferExpr(BSVM_SVMCState *ctx, dytf l)
{
	dytf t, cn, cv, ct;

	l=BSVMC_ReduceExpr(ctx, l);

//	dyPrintf("Infer Reduce: ");
//	BSVM_TY_PrintLN(l);

	if(dytfConsP(l))
	{
		t=BSVMC_InferForm(ctx, l);
		return(t);
	}

	if(dytfSymbolP(l))
	{
		if(BSVMC_SymbolIsP(l, "I"))
			return(dytfKeyword("complex"));
		if(BSVMC_SymbolIsP(l, "PI"))
			return(dytfKeyword("float"));
		if(BSVMC_SymbolIsP(l, "E"))
			return(dytfKeyword("float"));

		cn=ctx->lenv;
		ct=ctx->tenv;
		cv=ctx->venv;

		while(dytfConsP(cn))
		{
			if(dytfCar(cn)==l)
			{
				t=BSVMC_InferTypeBase(dytfCar(ct));
				return(t);
			}
			cn=dytfCdr(cn);
			ct=dytfCdr(ct);
			cv=dytfCdr(cv);
		}

		return(BGBDY_NULL);
	}

	if(l==BGBDY_NULL)return(dykeyword("null"));
	if(l==BGBDY_TRUE)return(dykeyword("bool"));
	if(l==BGBDY_FALSE)return(dykeyword("bool"));

	if(dyfixintp(l))return(dykeyword("fixnum"));
	if(dyflonump(l))return(dykeyword("flonum"));

//	if(dyfixintp(l))return(dykeyword("int"));
//	if(dyflonump(l))return(dykeyword("float"));
//	if(BSVM_CHARP(l))return(dykeyword("char"));

	if(dystringp(l))return(dykeyword("string"));
	if(dykeywordp(l))return(dykeyword("keyword"));

	if(dyarrayp(l))return(dykeyword("array"));
//	if(BSVM_FVECTORP(l))return(dykeyword("vector"));

	return(BGBDY_NULL);
}

dytf BSVMC_InferExprClean(BSVM_SVMCState *ctx, dytf l, int *cp)
{
	dytf t;

	ctx->i_cx_p=1;
	t=BSVMC_InferExpr(ctx, l);
	*cp=ctx->i_cx_p;

	return(t);
}


int BSVMC_GuessCloseForm(BSVM_SVMCState *ctx, dytf l)
{
	dytf c;
	int i;

	if(BSVMC_FormIs(l, "lambda"))
		return(1);
	if(BSVMC_FormIs(l, "rlambda"))
		return(1);
	if(BSVMC_FormIs(l, "defun"))
		return(1);
	if(BSVMC_FormIs(l, "define") && dytfConsP(dytfCadr(l)))
		return(1);

	c=l; i=0;
	while(dytfConsP(c))
	{
		i=i||BSVMC_GuessCloseExpr(ctx, dytfCar(c));
		c=dytfCdr(c);
	}
	return(i);
}

int BSVMC_GuessCloseExpr(BSVM_SVMCState *ctx, dytf l)
{
	dytf c;
	int i;

	if(dytfConsP(l))
		return(BSVMC_GuessCloseForm(ctx, l));

	if(dysymbolp(l))
	{
		if(l==ctx->cf_n)
			return(0);
	
		c=ctx->lenv; i=0;
		while(dytfConsP(c))
		{
			if(c==ctx->llenv)
				i=1;
			if(dytfCar(c)==l)
				break;
			c=dytfCdr(c);
		}

		if(dytfConsP(c))
			return(i);
		return(0);
	}

	return(0);
}

int BSVMC_GuessCloseBlock(BSVM_SVMCState *ctx, dytf l)
{
	dytf c;
	int i;

	c=l; i=0;
	while(dytfConsP(c))
	{
		i=i||BSVMC_GuessCloseExpr(ctx, dytfCar(c));
		c=dytfCdr(c);
	}
	return(i);
}
