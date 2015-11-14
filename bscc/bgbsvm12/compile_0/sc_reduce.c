#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbsvm.h>


int BSVMC_BoolExpr(BSVM_SVMCState *ctx, dyt l)
{
	if(l==dyint(0))return(0);
	if(l==dyfloat(0))return(0);
	if(l==BGBDY_FALSE)return(0);

	if(dyintp(l))return(1);
	if(dyfloatp(l))return(1);
	if(l==BGBDY_TRUE)return(1);

	return(-1);

//	return(dytruep(l));
}

int BSVMC_IsExpr(dyt l)
{
	if(dyconsp(l))return(1);
	if(dysymbolp(l))return(1);
	return(0);
}

dyt BSVMC_ReduceForm(BSVM_SVMCState *ctx, dyt l)
{
	dyt c, t, x;
	int i, j;

	if(dyFormIsP(l, "unary"))
	{
		t=BSVMC_ReduceExpr(ctx, dycaddr(l));
		if(BSVMC_IsExpr(t))return(l);

		if(dycadr(l)==dysymbol("!"))
		{
			if(dycar(t)==dysymbol("=="))return(dyconss("!=", dycdr(t)));
			if(dycar(t)==dysymbol("!="))return(dyconss("==", dycdr(t)));
			if(dycar(t)==dysymbol("==="))return(dyconss("!==", dycdr(t)));
			if(dycar(t)==dysymbol("!=="))return(dyconss("===", dycdr(t)));

			if(dycar(t)==dysymbol("<"))return(dyconss(">=", dycdr(t)));
			if(dycar(t)==dysymbol(">"))return(dyconss("<=", dycdr(t)));
			if(dycar(t)==dysymbol("<="))return(dyconss(">", dycdr(t)));
			if(dycar(t)==dysymbol(">="))return(dyconss("<", dycdr(t)));
		}

		if(dycadr(l)==dysymbol("+"))i=BSVM_OPR_POS;
		if(dycadr(l)==dysymbol("-"))i=BSVM_OPR_NEG;
		if(dycadr(l)==dysymbol("~"))i=BSVM_OPR_NOT;
		if(dycadr(l)==dysymbol("!"))i=BSVM_OPR_LNOT;
		if(dycadr(l)==dysymbol("*"))i=BSVM_OPR_UMUL;
		if(dycadr(l)==dysymbol("/"))i=BSVM_OPR_UDIV;
		if(dycadr(l)==dysymbol("\\"))i=BSVM_OPR_UIDIV;
		if(dycadr(l)==dysymbol("%"))i=BSVM_OPR_UMOD;
		if(dycadr(l)==dysymbol("&"))i=BSVM_OPR_UAND;
		if(dycadr(l)==dysymbol("|"))i=BSVM_OPR_UOR;
		if(dycadr(l)==dysymbol("^"))i=BSVM_OPR_UXOR;
		if(dycadr(l)==dysymbol("&&"))i=BSVM_OPR_ULAND;
		if(dycadr(l)==dysymbol("||"))i=BSVM_OPR_ULOR;
		if(dycadr(l)==dysymbol("^^"))i=BSVM_OPR_ULXOR;

		if(i>=0)
		{
			c=dytfUnwrap(BSVM_Unary(NULL, i, dytfWrap(t)));
			if(c && (c!=UNDEFINED))
				return(c);
		}

//		l=BSVM_Opr_Unary(i, t);
		return(l);
	}

#if 0
	if((dyFormIsP(l, "+")) || (dyFormIsP(l, "-")) ||
		(dyFormIsP(l, "*")) || (dyFormIsP(l, "/")) ||
		(dyFormIsP(l, "%")) || (dyFormIsP(l, "&")) ||
		(dyFormIsP(l, "|")) || (dyFormIsP(l, "^")) ||
		(dyFormIsP(l, "**")) || (dyFormIsP(l, "\\")))
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		if(BSVMC_IsExpr(t))return(l);

		if(dyFormIsP(l, "+"))i=BSVM_OPR_ADD;
		if(dyFormIsP(l, "-"))i=BSVM_OPR_SUB;
		if(dyFormIsP(l, "*"))i=BSVM_OPR_MUL;
		if(dyFormIsP(l, "/"))i=BSVM_OPR_DIV;
		if(dyFormIsP(l, "%"))i=BSVM_OPR_MOD;
		if(dyFormIsP(l, "&"))i=BSVM_OPR_AND;
		if(dyFormIsP(l, "|"))i=BSVM_OPR_OR;
		if(dyFormIsP(l, "^"))i=BSVM_OPR_XOR;
		if(dyFormIsP(l, "\\"))i=BSVM_OPR_IDIV;
		if(dyFormIsP(l, "**"))i=BSVM_OPR_EXP;

		x=t; c=dycddr(l);
		while(dyconsp(c))
		{
			t=BSVMC_ReduceExpr(ctx, dycar(c));
			if(BSVMC_IsExpr(t))return(l);

			x=BSVM_Opr_Binary(i, x, t);

			c=dycdr(c);
		}
		return(x);
	}

	if((dyFormIsP(l, "<<")) || (dyFormIsP(l, ">>")) ||
		(dyFormIsP(l, "==")) || (dyFormIsP(l, "===")) ||
		(dyFormIsP(l, "!=")) || (dyFormIsP(l, "!==")) ||
		(dyFormIsP(l, "<")) || (dyFormIsP(l, ">")) ||
		(dyFormIsP(l, "<=")) || (dyFormIsP(l, ">=")))
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		if(BSVMC_IsExpr(t))return(l);

		x=BSVMC_ReduceExpr(ctx, dycaddr(l));
		if(BSVMC_IsExpr(x))return(l);


		if(dyFormIsP(l, "<<"))i=BSVM_OPR_SHL;
		if(dyFormIsP(l, ">>"))i=BSVM_OPR_SHR;
		if(dyFormIsP(l, "=="))i=BSVM_OPR_EQV;
		if(dyFormIsP(l, "!="))i=BSVM_OPR_NEQV;
		if(dyFormIsP(l, "==="))i=BSVM_OPR_EQ;
		if(dyFormIsP(l, "!=="))i=BSVM_OPR_NEQ;
		if(dyFormIsP(l, "<"))i=BSVM_OPR_L;
		if(dyFormIsP(l, ">"))i=BSVM_OPR_G;
		if(dyFormIsP(l, "<="))i=BSVM_OPR_LE;
		if(dyFormIsP(l, ">="))i=BSVM_OPR_GE;

		l=BSVM_Opr_Binary(i, t, x);
		return(l);
	}
#endif

	if(BSVMC_FormIs(l, "binary"))
	{
		t=BSVMC_ReduceExpr(ctx, dycaddr(l));
		if(BSVMC_IsExpr(t))return(l);

		x=BSVMC_ReduceExpr(ctx, dycadddr(l));
		if(BSVMC_IsExpr(x))return(l);

		i=-1;
		if(dycadr(l)==dysymbol("+"))i=BSVM_OPR_ADD;
		if(dycadr(l)==dysymbol("-"))i=BSVM_OPR_SUB;
		if(dycadr(l)==dysymbol("*"))i=BSVM_OPR_MUL;
		if(dycadr(l)==dysymbol("/"))i=BSVM_OPR_DIV;
		if(dycadr(l)==dysymbol("%"))i=BSVM_OPR_MOD;
		if(dycadr(l)==dysymbol("&"))i=BSVM_OPR_AND;
		if(dycadr(l)==dysymbol("|"))i=BSVM_OPR_OR;
		if(dycadr(l)==dysymbol("^"))i=BSVM_OPR_XOR;
		if(dycadr(l)==dysymbol("\\"))i=BSVM_OPR_IDIV;
		if(dycadr(l)==dysymbol("**"))i=BSVM_OPR_EXP;

		if(dycadr(l)==dysymbol("<<"))i=BSVM_OPR_SHL;
		if(dycadr(l)==dysymbol(">>"))i=BSVM_OPR_SHR;
		if(dycadr(l)==dysymbol("=="))i=BSVM_OPR_EQV;
		if(dycadr(l)==dysymbol("!="))i=BSVM_OPR_NEQV;
		if(dycadr(l)==dysymbol("==="))i=BSVM_OPR_EQ;
		if(dycadr(l)==dysymbol("!=="))i=BSVM_OPR_NEQ;
		if(dycadr(l)==dysymbol("<"))i=BSVM_OPR_L;
		if(dycadr(l)==dysymbol(">"))i=BSVM_OPR_G;
		if(dycadr(l)==dysymbol("<="))i=BSVM_OPR_LE;
		if(dycadr(l)==dysymbol(">="))i=BSVM_OPR_GE;


//		if((dycadr(l)==dysymbol("/")) && dyfixintp(t) && dyfixintp(x))
//			return(BSVM_RATIONAL(TOINT(t), TOINT(x)));

		if((dycadr(l)==dysymbol("/")) && dyfixintp(t) && dyfixintp(x))
			return(dyfloat(((double)dyintv(t))/dyintv(x)));

//		if((i>0) && dySmallQuatp(t) && dySmallQuatp(x))
		if(i>=0)
		{
			c=dytfUnwrap(BSVM_Binary(NULL, i, dytfWrap(t), dytfWrap(x)));
			if(c && (c!=UNDEFINED))
				return(c);
		}

//		l=BSVM_Opr_Binary(i, t, x);
		return(l);
	}

	if(dyFormIsP(l, "if"))
	{
#if 1
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			l=BSVMC_ReduceExpr(ctx, dycaddr(l));
			return(l);
		}
		if(i==0)
		{
			t=BGBDY_NULL;
			if(dyconsp(dycdddr(l)))
				t=BSVMC_ReduceExpr(ctx, dycadddr(l));
			return(t);
		}
#endif
		return(l);
	}

	if(dyFormIsP(l, "begin"))
	{
//		if((dycddr(l)==BGBDY_NULL) &&
//			(BSVMC_FormIs(dycadr(l), "unary") ||
//			BSVMC_FormIs(dycadr(l), "binary")))

		if(dycdr(l)==BGBDY_NULL)
			return(BGBDY_NULL);

		if(dycddr(l)==BGBDY_NULL)
		{
			t=BSVMC_ReduceExpr(ctx, dycadr(l));
			return(t);
		}

		return(l);
	}

	if(dyFormIsP(l, "sizeof"))
	{
		i=BSVMC_InferTypeBaseSize(dycadr(l));
		if(i>=0)return(dyint(i));
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

dyt BSVMC_ReduceExpr(BSVM_SVMCState *ctx, dyt l)
{
	char tb[256];
	dyt t, cn, cv, ct;
	char *s, *name;

	if(dyconsp(l))
	{
		t=BSVMC_ReduceForm(ctx, l);
		return(t);
	}

	if(dysymbolp(l))
	{
		if(l==dysymbol("I"))return(dycomplex2(0, 1));
		if(l==dysymbol("PI"))return(dyfloat(3.14159265358979323846));
		if(l==dysymbol("TAU"))return(dyfloat(2*3.14159265358979323846));
		if(l==dysymbol("E"))return(dyfloat(2.7182818284590452354));
		if(l==dysymbol("NULL"))return(dysymbol("null"));

		if(l==dysymbol("this"))return(l);
		if(l==dysymbol("true"))return(l);
		if(l==dysymbol("false"))return(l);
		if(l==dysymbol("null"))return(l);
//		if(l==dysymbol("undefined"))return(l);

		cn=ctx->lenv;
		ct=ctx->tenv;
		cv=ctx->venv;

		if(ctx->cap)return(l);

		while(dyconsp(cn) && (cn!=ctx->lenv))
		{
			if((dycar(cn)==l) && (dycar(cv)!=BGBDY_NULL))
				return(dycar(cv));

			cn=dycdr(cn);
			ct=dycdr(ct);
			cv=dycdr(cv);
		}

		name=dysymbolv(l);

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


dyt BSVMC_InferTypeBase(dyt l)
{
	char b[64];
	char *s, *t;

	if(!dykeywordp(l) && !dysymbolp(l))
		return(l);
//		return(BGBDY_NULL);

	s=dysymbolv(l); t=b;
	while(*s && (*s!='$'))*t++=*s++;
	*t++=0;

	return(dykeyword(b));
}

char *BSVMC_FlattenQNameBaseSig(dyt l)
{
	char tb[256];
	char *s;
	dyt t;
	int i;

	if(dykeywordp(l) || dysymbolp(l))
	{
		s=dysymbolv(l);
		return(s);
	}

	if(BSVMC_FormIs(l, "objref"))
	{
		s=BSVMC_FlattenQNameBaseSig(dycadr(l));
		if(!s)return(NULL);
		t=dycaddr(l);
		if(dysymbolp(t))
			return(gcrsprint("%s/%s", s, dysymbolv(t)));
		return(NULL);
	}
	return(NULL);
}

int BSVMC_InferTypeBaseSize(dyt l)
{
	char tb[256];
	char *s;
	dyt t;
	int i;

	if(dykeywordp(l) || dysymbolp(l))
	{
		s=dysymbolv(l);
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

char *BSVMC_InferTypeBaseSig(dyt l)
{
	char tb[256];
	char *s;
	dyt t;
	int i;

	if(dykeywordp(l) || dysymbolp(l))
	{
		s=dysymbolv(l);
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
		s=BSVMC_InferTypeBaseSig(dycaddr(l));
		return(gcrsprint("P%s", s));
	}

	if(BSVMC_FormIsUnary(l, "&"))
	{
		s=BSVMC_InferTypeBaseSig(dycaddr(l));
		return(gcrsprint("R%s", s));
	}

	if(BSVMC_FormIs(l, "emptyindex"))
	{
		s=BSVMC_InferTypeBaseSig(dycadr(l));
		return(gcrsprint("Q%s", s));
	}

	if(BSVMC_FormIs(l, "getindex"))
	{
		t=dycaddr(l);

		if(dysymbolp(t))
		{
			s=BSVMC_FlattenQNameBaseSig(l);
			if(s)return(gcrsprint("U%s;", s));
			return(NULL);
		}
	
		if(dyintp(t))
		{
			s=BSVMC_InferTypeBaseSig(dycadr(l));
			i=dyintv(t);
			return(gcrsprint("A%d;%s", i, s));
		}
		return(NULL);
	}

	s=BSVMC_FlattenQNameBaseSig(l);
	if(s)return(gcrsprint("U%s;", s));

	return(NULL);
}

char *BSVMC_InferVarBaseSig(dyt n)
{
	char *s;
	dyt u, v;

	u=NULL; v=NULL;

	if(dyFormIsP(n, "set!"))
	{
		v=dycaddr(n);
		n=dycadr(n);				
	}
	
	if(dyFormIsP(n, "cast"))
	{
		u=dycadr(n);
		n=dycaddr(n);				
	}
		
	s=BSVMC_InferTypeBaseSig(u);
	return(s);
}

char *BSVMC_InferVarsListBaseSig(dyt l)
{
	char tb[256];
	char *s, *t;
	dyt c, n, u, v;
	int i;

	c=l; t=tb;
	while(dyconsp(c))
	{
		n=dycar(c);
		c=dycdr(c);

		s=BSVMC_InferVarBaseSig(n);
		if(!s)s="r";
		
		strcpy(t, s);
		t+=strlen(t);
	}
	
	*t++=0;
	
	return(gcrstrdup(tb));
}

char *BSVMC_InferArgsBaseSig(dyt l)
{
	char tb[256];
	char *s, *t;
	dyt c, n, u, v;
	
	t=tb;
	*t++='(';
	s=BSVMC_InferVarsListBaseSig(dycdr(l));
	strcpy(t, s);
	t+=strlen(t);
	*t++=')';

	n=dycar(l);
	s=BSVMC_InferVarBaseSig(n);
	if(!s)s="r";
		
	strcpy(t, s);
	t+=strlen(t);

	*t++=0;

	return(gcrstrdup(tb));
}

dyt BSVMC_InferArgsBaseSig2(dyt l)
{
	char *s;

	s=BSVMC_InferTypeBaseSig(l);
	if(!s)return(dysymbol("r"));
	return(dysymbol(s));
}


char *BSVMC_InferExprListBaseSig(BSVM_SVMCState *ctx, dyt l)
{
	char tb[256];
	char *s, *t;
	dyt c, n, u, v;
	int i;

	c=l; t=tb;
	while(dyconsp(c))
	{
		n=dycar(c);
		c=dycdr(c);

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

char *BSVMC_InferCallArgsBaseSig(BSVM_SVMCState *ctx, dyt l, dyt rt)
{
	char tb[256];
	char *s, *t;
	dyt c, n, u, v;
	
	t=tb;
	*t++='(';
	s=BSVMC_InferExprListBaseSig(ctx, dycdr(l));
	strcpy(t, s);
	t+=strlen(t);
	*t++=')';

//	n=dycar(l);
//	s=BSVMC_InferVarBaseSig(n);
	s=NULL;
	if(rt) { s=BSVMC_InferTypeBaseSig(rt); }
	if(!s)s="r";
		
	strcpy(t, s);
	t+=strlen(t);

	*t++=0;

	return(gcrstrdup(tb));
}

char *BSVMC_InferMethodCallArgsBaseSig(BSVM_SVMCState *ctx, dyt l, dyt rt)
{
	char tb[256];
	char *s, *t;
	dyt c, n, u, v;
	
	t=tb;
	*t++='(';
	s=BSVMC_InferExprListBaseSig(ctx, dycddr(l));
	strcpy(t, s);
	t+=strlen(t);
	*t++=')';

//	n=dycar(l);
//	s=BSVMC_InferVarBaseSig(n);
	s=NULL;
	if(rt) { s=BSVMC_InferTypeBaseSig(rt); }
	if(!s)s="r";
		
	strcpy(t, s);
	t+=strlen(t);

	*t++=0;

	return(gcrstrdup(tb));
}


dyt BSVMC_BuildVarInfo(dyt n)
{
	BSVM_SVMVarInfo *inf;
	char *s;
	dyt u, v;
	int rst, ref;

	u=NULL; v=NULL; rst=0; ref=0;

	if(dyFormIsP(n, "byval"))
	{
		n=dycadr(n);
	}

	if(dyFormIsP(n, "byref"))
	{
		n=dycadr(n);
		ref=1;				
	}

	if(dyFormIsP(n, "rest"))
	{
//		v=dycaddr(n);
		n=dycadr(n);
		rst=1;				
	}

	if(dyFormIsP(n, "set!"))
	{
		v=dycaddr(n);
		n=dycadr(n);				
	}
	
	if(dyFormIsP(n, "cast"))
	{
		u=dycadr(n);
		n=dycaddr(n);
	}

	if(!u && !v && !rst)return(n);

	s=BSVMC_InferTypeBaseSig(u);
	
	if(rst)s="Cz";
	
	inf=gctalloc("svm_varinfo_t", sizeof(BSVM_SVMVarInfo));
	inf->name=dyllString(dysymbolv(n));
	inf->sig=dyllString(s);
//	inf->flags=fl;
	inf->value=dytfWrap(v);
	
	if(ref)
	{
		inf->flags|=BGBDYC_FLAG_BYREF;
	}
	
	return((dyt)inf);
}



int BSVMC_InferIsIntP(dyt t)
{
	if(t==dykeyword("byte"))return(1);
	if(t==dykeyword("sbyte"))return(1);
	if(t==dykeyword("short"))return(1);
	if(t==dykeyword("ushort"))return(1);
	if(t==dykeyword("int"))return(1);
	if(t==dykeyword("uint"))return(1);
	return(0);
}

int BSVMC_InferIsLongP(dyt t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(t==dykeyword("long"))return(1);
	if(t==dykeyword("ulong"))return(1);
	return(0);
}

int BSVMC_InferIsFloatP(dyt t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(t==dykeyword("float"))return(1);
	return(0);
}

int BSVMC_InferIsDoubleP(dyt t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(t==dykeyword("float"))return(1);
	if(t==dykeyword("double"))return(1);
	return(0);
}

int BSVMC_InferIsNumP(dyt t)
{
	if(BSVMC_InferIsIntP(t))return(1);
	if(BSVMC_InferIsLongP(t))return(1);
	if(BSVMC_InferIsFloatP(t))return(1);
	if(BSVMC_InferIsDoubleP(t))return(1);
	return(0);
}

dyt BSVMC_InferIsInt(dyt t)
{
	if(BSVMC_InferIsIntP(t))
		return(dykeyword("int"));
	return(BGBDY_NULL);
}

dyt BSVMC_InferIsLong(dyt t)
{
	if(BSVMC_InferIsIntP(t))
		return(dykeyword("int"));
	if(BSVMC_InferIsLongP(t))
		return(dykeyword("long"));
	return(BGBDY_NULL);
}

dyt BSVMC_InferIsNum(dyt t)
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

dyt BSVMC_InferForm(BSVM_SVMCState *ctx, dyt l)
{
	dyt c, t, u, x, n, lt, rt, ln, rn;
	int i;

	if(dyFormIsP(l, "cast"))
	{
//		t=BSVMC_InferExpr(ctx, dycaddr(l));
		t=dycadr(l);
		t=BSVMC_InferTypeBase(t);
		return(t);
	}

	if((dyFormIsP(l, "unary")) && (dycadr(l)==dysymbol("!")))
	{
		t=BSVMC_InferExpr(ctx, dycaddr(l));
		return(dykeyword("bool"));
	}

	if((dyFormIsP(l, "unary")) && (dycadr(l)==dysymbol("~")))
	{
		t=BSVMC_InferExpr(ctx, dycadr(l));
		return(BSVMC_InferIsLong(t));
	}

//	if((dyFormIsP(l, "-")) && !dyconsp(dycddr(l)))
//	{
//		t=BSVMC_InferExpr(ctx, dycadr(l));
//		return(BSVMC_InferIsNum(t));
//	}

	if(BSVMC_FormIs(l, "binary") && dycadr(l)==dysymbol("/"))
	{
		t=BSVMC_InferExpr(ctx, dycaddr(l));
		x=BSVMC_InferExpr(ctx, dycadddr(l));

		i=BSVMC_InferIsNumP(t) &&
			BSVMC_InferIsNumP(x);
		if(!i)return(BGBDY_NULL);

		n=dykeyword("float");
		if(t==dykeyword("double"))n=t;
		if(x==dykeyword("double"))n=x;
		return(n);
	}

	if(BSVMC_FormIs(l, "binary") && (dycadr(l)==dysymbol("\\")))
	{
		t=BSVMC_InferExpr(ctx, dycaddr(l));
		x=BSVMC_InferExpr(ctx, dycadddr(l));

		return(dykeyword("int"));
	}

//	if((dyFormIsP(l, "+")) || (dyFormIsP(l, "-")) ||
//		(dyFormIsP(l, "*")) ||
//		(dyFormIsP(l, "%")) || (dyFormIsP(l, "&")) ||
//		(dyFormIsP(l, "|")) || (dyFormIsP(l, "^")))

	if(BSVMC_FormIs(l, "binary") && (
		(dycadr(l)==dysymbol("+")) || (dycadr(l)==dysymbol("-")) ||
		(dycadr(l)==dysymbol("*")) || (dycadr(l)==dysymbol("%")) ||
		(dycadr(l)==dysymbol("&")) || (dycadr(l)==dysymbol("|")) ||
		(dycadr(l)==dysymbol("^"))))
	{
//		ln=dycaddr(l);
//		rn=dycadddr(l);

		ln=BSVMC_ReduceExpr(ctx, dycaddr(l));
		rn=BSVMC_ReduceExpr(ctx, dycadddr(l));
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
			return(dysymbol("int"));
		}

		if(lt==rt)return(lt);
		return(NULL);

#if 0
		t=BSVMC_InferExpr(ctx, dycaddr(l));

		x=t; c=dycdddr(l);
		while(dyconsp(c))
		{
			t=BSVMC_InferExpr(ctx, dycar(c));
			if(t!=x)return(BGBDY_NULL);
			c=dycdr(c);
		}
		return(x);
#endif
	}

	if(BSVMC_FormIs(l, "binary") &&
		((dycadr(l)==dysymbol("<<")) || (dycadr(l)==dysymbol(">>")) ||
			(dycadr(l)==dysymbol(">>>"))))
	{
		t=BSVMC_InferExpr(ctx, dycaddr(l));
		x=BSVMC_InferExpr(ctx, dycadddr(l));

		i=BSVMC_InferIsIntP(t) &&
			BSVMC_InferIsIntP(x);
		if(i)return(t);
		return(BGBDY_NULL);
	}

//	if((dyFormIsP(l, "==")) || (dyFormIsP(l, "===")) ||
//		(dyFormIsP(l, "!=")) || (dyFormIsP(l, "!==")) ||
//		(dyFormIsP(l, "<")) || (dyFormIsP(l, ">")) ||
//		(dyFormIsP(l, "<=")) || (dyFormIsP(l, ">=")))

	if(BSVMC_FormIs(l, "binary") && (
		(dycadr(l)==dysymbol("==")) || (dycadr(l)==dysymbol("===")) ||
		(dycadr(l)==dysymbol("!=")) || (dycadr(l)==dysymbol("!==")) ||
		(dycadr(l)==dysymbol("<"))  || (dycadr(l)==dysymbol(">")) ||
		(dycadr(l)==dysymbol("<=")) || (dycadr(l)==dysymbol(">="))))
	{
		return(dykeyword("bool"));
	}

	if(dyFormIsP(l, "if"))
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			t=BSVMC_InferExpr(ctx, dycaddr(l));
			return(t);
		}
		if(i==0)
		{
			t=BGBDY_NULL;
			if(dyconsp(dycdddr(l)))
				t=BSVMC_InferExpr(ctx, dycadddr(l));
			return(t);
		}

		x=BGBDY_NULL;
		t=BSVMC_InferExpr(ctx, dycaddr(l));
		if(dyconsp(dycdddr(l)))
			x=BSVMC_InferExpr(ctx, dycadddr(l));
		if(t!=x)return(BGBDY_NULL);
		return(t);
	}

	if(dyFormIsP(l, "funcall"))
	{
#if 0
		dyPrintf("Infer Func: ");
		BSVM_TY_Print(dycadr(l));
		dyPrintf("=");
		BSVM_TY_Print(ctx->cf_n);
		dyPrintf("\n");
#endif

		ctx->i_cx_p=0;
		if(dycadr(l)==ctx->cf_n)
		{
//			*(int *)-1=-1;
			
			t=BSVMC_InferTypeBase(ctx->cf_ty);
			return(t);
//			return(ctx->cf_ty);
		}
		return(BGBDY_NULL);
	}

	if((dyFormIsP(l, "setindex!")) || (dyFormIsP(l, "vector-set!")))
	{
		t=BSVMC_InferExpr(ctx, dycadddr(l));
		return(t);
	}

	if((dyFormIsP(l, "getindex")) || (dyFormIsP(l, "vector-ref")))
	{
		t=BSVMC_InferExpr(ctx, dycadr(l));
		if(!t)
			return(NULL);
		u=BSVMC_TypeArrayBaseType(ctx, t);
		return(u);
	}

	ctx->i_cx_p=0;
	return(BGBDY_NULL);
}

dyt BSVMC_InferExpr(BSVM_SVMCState *ctx, dyt l)
{
	dyt t, cn, cv, ct;

	l=BSVMC_ReduceExpr(ctx, l);

//	dyPrintf("Infer Reduce: ");
//	BSVM_TY_PrintLN(l);

	if(dyconsp(l))
	{
		t=BSVMC_InferForm(ctx, l);
		return(t);
	}

	if(dysymbolp(l))
	{
		if(l==dysymbol("I"))return(dykeyword("complex"));
		if(l==dysymbol("PI"))return(dykeyword("float"));
		if(l==dysymbol("E"))return(dykeyword("float"));

		cn=ctx->lenv;
		ct=ctx->tenv;
		cv=ctx->venv;

		while(dyconsp(cn))
		{
			if(dycar(cn)==l)
			{
				t=BSVMC_InferTypeBase(dycar(ct));
				return(t);
			}
			cn=dycdr(cn);
			ct=dycdr(ct);
			cv=dycdr(cv);
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

dyt BSVMC_InferExprClean(BSVM_SVMCState *ctx, dyt l, int *cp)
{
	dyt t;

	ctx->i_cx_p=1;
	t=BSVMC_InferExpr(ctx, l);
	*cp=ctx->i_cx_p;

	return(t);
}


int BSVMC_GuessCloseForm(BSVM_SVMCState *ctx, dyt l)
{
	dyt c;
	int i;

	if(BSVMC_FormIs(l, "lambda"))
		return(1);
	if(BSVMC_FormIs(l, "rlambda"))
		return(1);
	if(BSVMC_FormIs(l, "defun"))
		return(1);
	if(BSVMC_FormIs(l, "define") && dyconsp(dycadr(l)))
		return(1);

	c=l; i=0;
	while(dyconsp(c))
	{
		i=i||BSVMC_GuessCloseExpr(ctx, dycar(c));
		c=dycdr(c);
	}
	return(i);
}

int BSVMC_GuessCloseExpr(BSVM_SVMCState *ctx, dyt l)
{
	dyt c;
	int i;

	if(dyconsp(l))
		return(BSVMC_GuessCloseForm(ctx, l));

	if(dysymbolp(l))
	{
		if(l==ctx->cf_n)
			return(0);
	
		c=ctx->lenv; i=0;
		while(dyconsp(c))
		{
			if(c==ctx->llenv)
				i=1;
			if(dycar(c)==l)
				break;
			c=dycdr(c);
		}

		if(dyconsp(c))
			return(i);
		return(0);
	}

	return(0);
}

int BSVMC_GuessCloseBlock(BSVM_SVMCState *ctx, dyt l)
{
	dyt c;
	int i;

	c=l; i=0;
	while(dyconsp(c))
	{
		i=i||BSVMC_GuessCloseExpr(ctx, dycar(c));
		c=dycdr(c);
	}
	return(i);
}
