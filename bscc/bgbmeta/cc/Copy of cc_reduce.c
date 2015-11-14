#include <bgbccc.h>

int BGBCC_IsUnaryP(BCCX_Node *l, char *op)
{
	return(BCCX_TagAttrIsP(l, "unary", "op", op));
}

int BGBCC_IsBinaryP(BCCX_Node *l, char *op)
{
	return(BCCX_TagAttrIsP(l, "binary", "op", op));
}

#if 0
int BGBCC_BoolExpr(BGBCC_State *ctx, BCCX_Node *l)
{
	if(l==BS1_MM_FALSE)return(0);
	if(l==BS1_MM_NULL)return(0);
	if(l==BS1_MM_FIXNUM)return(0);
	if(l==BS1_MM_FLONUM)return(0);

	if(l==BS1_MM_TRUE)return(1);
	if(BS1_FIXNUMP(l))return(1);
	if(BS1_FLONUMP(l))return(1);
	return(-1);
}

int BGBCC_IsExpr(BCCX_Node *l)
{
	if(BS1_CONSP(l))return(1);
	if(BS1_SYMBOLP(l))return(1);
	return(0);
}
#endif

int BGBCC_BoolExpr(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *t;
	double f;
	int i;

	if(BCCX_TagIsP(l, "int"))
	{
		i=BCCX_GetInt(l, "value");
		if(!i)return(0);
		return(1);
	}
	if(BCCX_TagIsP(l, "real"))
	{
		f=BCCX_GetFloat(l, "value");
		if(f==0)return(0);
		return(1);
	}

	if(BCCX_TagIsP(l, "cast"))
	{
		t=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));
		i=BGBCC_BoolExpr(ctx, t);
		return(i);
	}

	if(BCCX_TagIsP(l, "boolify"))
	{
		t=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));
		i=BGBCC_BoolExpr(ctx, t);
		return(i);
	}

	if(BCCX_TagIsP(l, "string"))
		return(1);

	return(-1);
}

BCCX_Node *BGBCC_WrapInt(int i)
{
	BCCX_Node *t;
	t=BCCX_New("int");
	BCCX_SetInt(t, "value", i);
	return(t);
}

BCCX_Node *BGBCC_WrapReal(double f)
{
	BCCX_Node *t;
	t=BCCX_New("real");
	BCCX_SetFloat(t, "value", f);
	return(t);
}

int BGBCC_IsIntP(BGBCC_State *ctx, BCCX_Node *l)
{
	if(BCCX_TagIsP(l, "int"))return(1);
	return(0);
}

int BGBCC_IsRealP(BGBCC_State *ctx, BCCX_Node *l)
{
	if(BCCX_TagIsP(l, "int"))return(1);
	if(BCCX_TagIsP(l, "real"))return(1);
	return(0);
}

BCCX_Node *BGBCC_Boolify(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *t;
	if(BCCX_TagIsP(l, "boolify"))
		return(l);
	t=BCCX_New1("boolify", BCCX_New1("value", l));
	return(t);
}

BCCX_Node *BGBCC_ReduceForm(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *c, *t, *x, *ln, *rn;
	char *s;
	double f, g;
	int i, j;

	if(!l)return(l);

	if(BCCX_TagIsP(l, "value"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "left"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "right"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "cond"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "then"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "else"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "body"))return(BCCX_Child(l));

	if(BGBCC_IsUnaryP(l, "!"))
	{
		t=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));

		if(BGBCC_BoolExpr(ctx, t)==0)
			return(BGBCP_Number(NULL, "1"));
		if(BGBCC_BoolExpr(ctx, t)==1)
			return(BGBCP_Number(NULL, "0"));

		ln=BCCX_Fetch(t, "left");
		rn=BCCX_Fetch(t, "right");

		ln=BGBCC_ReduceExpr(ctx, ln);
		rn=BGBCC_ReduceExpr(ctx, rn);

		x=NULL;
		if(BGBCC_IsBinaryP(t, "=="))x=BGBCP_BinaryExpr("!=", ln, rn);
		if(BGBCC_IsBinaryP(t, "!="))x=BGBCP_BinaryExpr("==", ln, rn);
		if(BGBCC_IsBinaryP(t, "==="))x=BGBCP_BinaryExpr("!==", ln, rn);
		if(BGBCC_IsBinaryP(t, "!=="))x=BGBCP_BinaryExpr("===", ln, rn);
		if(BGBCC_IsBinaryP(t, "<"))x=BGBCP_BinaryExpr(">=", ln, rn);
		if(BGBCC_IsBinaryP(t, ">"))x=BGBCP_BinaryExpr("<=", ln, rn);
		if(BGBCC_IsBinaryP(t, "<="))x=BGBCP_BinaryExpr(">", ln, rn);
		if(BGBCC_IsBinaryP(t, ">="))x=BGBCP_BinaryExpr("<", ln, rn);

		if(x)
		{
			x=BGBCC_ReduceExpr(ctx, x);
			return(x);
		}
	}

#if 0
	if(BCCX_TagIsP(l, "and"))
	{
		c=BCCX_Child(l); t=NULL;
		while(c)
		{
			if(BCCX_TagIsP(c, "value"))
			{
				i=BGBCC_BoolExpr(ctx, BCCX_Child(c));
				if(i==0)return(BGBCP_Number("0"));
				if(i==-1)BCCX_AddEnd(t, BCCX_Clone(c));
				c=BCCX_Next(c);
				continue;
			}

			BCCX_AddEnd(t, BCCX_Clone(c));
			c=BCCX_Next(c);
		}

		c=t;
		while(c) { if(BCCX_TagIsP(c, "value"))break; c=BCCX_Next(c); }
		if(!c)return(BGBCP_Number("1"));

		x=BCCX_FindNextTag(c, "value");
		if(!x)return(BGBCC_ReduceExpr(ctx, c));

		t=BCCX_New1("and", t);
		return(t);
	}

	if(BCCX_TagIsP(l, "or"))
	{
		c=BCCX_Child(l); t=NULL;
		while(c)
		{
			if(BCCX_TagIsP(c, "value"))
			{
				i=BGBCC_BoolExpr(ctx, BCCX_Child(c));
				if(i==1)return(BGBCP_Number("1"));
				if(i==-1)BCCX_AddEnd(t, BCCX_Clone(c));
				c=BCCX_Next(c);
				continue;
			}

			BCCX_AddEnd(t, BCCX_Clone(c));
			c=BCCX_Next(c);
		}

		c=t;
		while(c) { if(BCCX_TagIsP(c, "value"))break; c=BCCX_Next(c); }
		if(!c)return(BGBCP_Number("0"));

		x=BCCX_FindNextTag(c, "value");
		if(!x)return(BGBCC_ReduceExpr(ctx, c));

		t=BCCX_New1("or", t);
		return(t);
	}
#endif

	if(BGBCC_IsBinaryP(l, "&&"))
	{
		ln=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "left"));
		rn=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "right"));

		if((BGBCC_BoolExpr(ctx, ln)==0) ||
			(BGBCC_BoolExpr(ctx, rn)==0))
				return(BGBCP_Number(NULL, "0"));

		if(BGBCC_BoolExpr(ctx, ln)==1)
		{
			t=BGBCC_Boolify(ctx, rn);
			return(t);
		}

		if(BGBCC_BoolExpr(ctx, rn)==1)
		{
			t=BGBCC_Boolify(ctx, ln);
			return(t);
		}

		x=BGBCP_BinaryExpr("&&", ln, rn);
		return(x);
	}

	if(BGBCC_IsBinaryP(l, "||"))
	{
		ln=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "left"));
		rn=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "right"));

		if((BGBCC_BoolExpr(ctx, ln)==1) ||
			(BGBCC_BoolExpr(ctx, rn)==1))
				return(BGBCP_Number(NULL, "1"));

		if(BGBCC_BoolExpr(ctx, ln)==0)
		{
			t=BGBCC_Boolify(ctx, rn);
			return(t);
		}

		if(BGBCC_BoolExpr(ctx, rn)==0)
		{
			t=BGBCC_Boolify(ctx, ln);
			return(t);
		}

		x=BGBCP_BinaryExpr("||", ln, rn);
		return(x);
	}

	if(BCCX_TagIsP(l, "unary"))
	{
		t=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));
		s=BCCX_Get(l, "op");

		if(BGBCC_IsIntP(ctx, t))
		{
			i=BCCX_GetInt(t, "value");

			if(!strcmp(s, "+"))return(BGBCC_WrapInt(i));
			if(!strcmp(s, "-"))return(BGBCC_WrapInt(-i));
			if(!strcmp(s, "~"))return(BGBCC_WrapInt(~i));
			if(!strcmp(s, "!"))return(BGBCC_WrapInt(!i));
		}

		if(BGBCC_IsRealP(ctx, t))
		{
			f=BCCX_GetFloat(t, "value");
			if(!strcmp(s, "+"))return(BGBCC_WrapReal(f));
			if(!strcmp(s, "-"))return(BGBCC_WrapReal(-f));
			if(!strcmp(s, "!"))return(BGBCC_WrapInt(f!=0));
		}

		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "binary"))
	{
		ln=BCCX_Fetch(l, "left");
		rn=BCCX_Fetch(l, "right");

		ln=BGBCC_ReduceExpr(ctx, ln);
		rn=BGBCC_ReduceExpr(ctx, rn);

		s=BCCX_Get(l, "op");

		if(BGBCC_IsIntP(ctx, ln) && BGBCC_IsIntP(ctx, rn))
		{
			i=BCCX_GetInt(ln, "value");
			j=BCCX_GetInt(rn, "value");

			if(!strcmp(s, "=="))return(BGBCC_WrapInt(i==j));
			if(!strcmp(s, "!="))return(BGBCC_WrapInt(i!=j));
			if(!strcmp(s, "==="))return(BGBCC_WrapInt(i==j));
			if(!strcmp(s, "!=="))return(BGBCC_WrapInt(i!=j));
			if(!strcmp(s, "<="))return(BGBCC_WrapInt(i<=j));
			if(!strcmp(s, ">="))return(BGBCC_WrapInt(i>=j));
			if(!strcmp(s, "<"))return(BGBCC_WrapInt(i<j));
			if(!strcmp(s, ">"))return(BGBCC_WrapInt(i>j));

			if(!strcmp(s, "+"))return(BGBCC_WrapInt(i+j));
			if(!strcmp(s, "-"))return(BGBCC_WrapInt(i-j));
			if(!strcmp(s, "*"))return(BGBCC_WrapInt(i*j));
			if(!strcmp(s, "/"))return(BGBCC_WrapInt(i/j));
			if(!strcmp(s, "%"))return(BGBCC_WrapInt(i%j));

			if(!strcmp(s, "&"))return(BGBCC_WrapInt(i&j));
			if(!strcmp(s, "|"))return(BGBCC_WrapInt(i|j));
			if(!strcmp(s, "^"))return(BGBCC_WrapInt(i^j));
			if(!strcmp(s, "\\"))return(BGBCC_WrapInt(i/j));
//			if(!strcmp(s, "**"))return(BGBCC_WrapInt(pow(i, j)));

			if(!strcmp(s, "&&"))return(BGBCC_WrapInt(i&&j));
			if(!strcmp(s, "||"))return(BGBCC_WrapInt(i||j));

			if(!strcmp(s, "<<"))return(BGBCC_WrapInt(i<<j));
			if(!strcmp(s, ">>"))return(BGBCC_WrapInt(i>>j));
		}

		if(BGBCC_IsRealP(ctx, ln) && BGBCC_IsRealP(ctx, rn))
		{
			f=BCCX_GetFloat(ln, "value");
			g=BCCX_GetFloat(rn, "value");

			if(!strcmp(s, "=="))return(BGBCC_WrapInt(f==g));
			if(!strcmp(s, "!="))return(BGBCC_WrapInt(f!=g));
			if(!strcmp(s, "==="))return(BGBCC_WrapInt(f==g));
			if(!strcmp(s, "!=="))return(BGBCC_WrapInt(f!=g));
			if(!strcmp(s, "<="))return(BGBCC_WrapInt(f<=g));
			if(!strcmp(s, ">="))return(BGBCC_WrapInt(f>=g));
			if(!strcmp(s, "<"))return(BGBCC_WrapInt(f<g));
			if(!strcmp(s, ">"))return(BGBCC_WrapInt(f>g));

			if(!strcmp(s, "+"))return(BGBCC_WrapReal(f+g));
			if(!strcmp(s, "-"))return(BGBCC_WrapReal(f-g));
			if(!strcmp(s, "*"))return(BGBCC_WrapReal(f*g));
			if(!strcmp(s, "/"))return(BGBCC_WrapReal(f/g));
			if(!strcmp(s, "\\"))return(BGBCC_WrapInt(f/g));
//			if(!strcmp(s, "**"))return(BGBCC_WrapReal(pow(i, j)));
		}

		x=BGBCP_BinaryExpr(s, ln, rn);
		return(x);
	}

#if 0
	if(FORM_IS(l, "unary"))
	{
		t=BGBCC_ReduceExpr(ctx, CADDR(l));
		if(BGBCC_IsExpr(t))return(BCCX_Clone(l));

		i=-1;
		if(CADR(l)==SYM("+"))i=BS1_OPR_POS;
		if(CADR(l)==SYM("-"))i=BS1_OPR_NEG;
		if(CADR(l)==SYM("~"))i=BS1_OPR_NOT;
		if(CADR(l)==SYM("!"))i=BS1_OPR_LNOT;

		if(i<0)return(BCCX_Clone(l));

		x=BS1_Opr_Unary(i, t);
		return(x);
	}

	if(FORM_IS(l, "binary"))
	{
		t=BGBCC_ReduceExpr(ctx, CADDR(l));
		if(BGBCC_IsExpr(t))return(BCCX_Clone(l));

		x=BGBCC_ReduceExpr(ctx, CADDDR(l));
		if(BGBCC_IsExpr(x))return(BCCX_Clone(l));

		i=-1;
		if(CADR(l)==SYM("+"))i=BS1_OPR_ADD;
		if(CADR(l)==SYM("-"))i=BS1_OPR_SUB;
		if(CADR(l)==SYM("*"))i=BS1_OPR_MUL;
		if(CADR(l)==SYM("/"))i=BS1_OPR_DIV;
		if(CADR(l)==SYM("%"))i=BS1_OPR_MOD;
		if(CADR(l)==SYM("&"))i=BS1_OPR_AND;
		if(CADR(l)==SYM("|"))i=BS1_OPR_OR;
		if(CADR(l)==SYM("^"))i=BS1_OPR_XOR;
		if(CADR(l)==SYM("\\"))i=BS1_OPR_IDIV;
		if(CADR(l)==SYM("**"))i=BS1_OPR_EXP;

		if(CADR(l)==SYM("<<"))i=BS1_OPR_SHL;
		if(CADR(l)==SYM(">>"))i=BS1_OPR_SHR;
		if(CADR(l)==SYM("=="))i=BS1_OPR_EQV;
		if(CADR(l)==SYM("!="))i=BS1_OPR_NEQV;
		if(CADR(l)==SYM("==="))i=BS1_OPR_EQ;
		if(CADR(l)==SYM("!=="))i=BS1_OPR_NEQ;
		if(CADR(l)==SYM("<"))i=BS1_OPR_L;
		if(CADR(l)==SYM(">"))i=BS1_OPR_G;
		if(CADR(l)==SYM("<="))i=BS1_OPR_LE;
		if(CADR(l)==SYM(">="))i=BS1_OPR_GE;

		if((CADR(l)==SYM("/")) && BS1_FIXNUMP(t) && BS1_FIXNUMP(x))
			return(BS1_TY_Rational(TOINT(t), TOINT(x)));

		if(i<0)return(BCCX_Clone(l));

		x=BS1_Opr_Binary(i, t, x);
		return(x);
	}
#endif

	if(BCCX_TagIsP(l, "if"))
	{
		t=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "cond"));
		i=BGBCC_BoolExpr(ctx, t);
		if(i==1)
		{
			t=BGBCC_ReduceExpr(ctx, BCCX_Fetch(l, "then"));
			return(t);
		}
		if(i==0)
		{
			t=BCCX_Fetch(l, "else");
			if(t)t=BGBCC_ReduceExpr(ctx, t);
			return(t);
		}
		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "begin"))
	{
		if(!BCCX_Next(BCCX_Child(l)))
		{
			t=BGBCC_ReduceExpr(ctx, BCCX_Child(l));
			return(t);
		}

		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "ref"))
	{
		if(!ctx)	//preprocessor
		{
			t=BGBCC_WrapInt(0);
			return(t);
		}
	}

	if(BCCX_TagIsP(l, "sizeof"))
	{
		t=BCCX_FindTag(l, "type");
		if(!t)
		{
			t=BCCX_Fetch(l, "tyexpr");
			if(t)t=BCCX_FindTag(t, "type");
		}

		if(ctx && t)
		{
			i=BGBCC_TryGetSizeofType(ctx, t);
			if(i>0)return(BGBCC_WrapInt(i));
		}
	}

	return(BCCX_Clone(l));
}

BCCX_Node *BGBCC_ReduceExpr(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *t;
	t=BGBCC_ReduceForm(ctx, l);
	return(t);
}


#if 0

BCCX_Node *BGBCC_InferTypeBase(BCCX_Node *l)
{
	char b[64];
	char *s, *t;
	BCCX_Node *n;
	int i;

	if(BS1_CONSP(l))
	{
		n=CADR(l);

		if(BS1_KEYWORDP(n))
		{
			s=TOSYM(n); t=b;
			while(*s && (*s!='$'))*t++=*s++;
			*t++=0;

			i=TOINT(CADDR(l));
			if(*s=='$')i|=atoi(s+1);
			if((i&1) && (b[0]!='u'))
			{
				BS1_StrCpy(b+1, b);
				b[0]='u';
			}

			n=KEYSYM(b);
		}

		return(n);
	}

	if(BS1_KEYWORDP(l))
	{
		s=TOSYM(l); t=b;
		while(*s && (*s!='$'))*t++=*s++;
		*t++=0;

		return(KEYSYM(b));
	}
}

int BGBCC_InferIsIntP(BCCX_Node *t)
{
	if(t==KEYSYM("char"))return(1);
	if(t==KEYSYM("uchar"))return(1);

	if(t==KEYSYM("byte"))return(1);
	if(t==KEYSYM("sbyte"))return(1);
	if(t==KEYSYM("short"))return(1);
	if(t==KEYSYM("ushort"))return(1);
	if(t==KEYSYM("int"))return(1);
	if(t==KEYSYM("uint"))return(1);
	return(0);
}

int BGBCC_InferIsLongP(BCCX_Node *t)
{
	if(BGBCC_InferIsIntP(t))return(1);
	if(t==KEYSYM("long"))return(1);
	if(t==KEYSYM("ulong"))return(1);
	return(0);
}

int BGBCC_InferIsFloatP(BCCX_Node *t)
{
	if(BGBCC_InferIsIntP(t))return(1);
	if(t==KEYSYM("float"))return(1);
	return(0);
}

int BGBCC_InferIsDoubleP(BCCX_Node *t)
{
	if(BGBCC_InferIsIntP(t))return(1);
	if(t==KEYSYM("float"))return(1);
	if(t==KEYSYM("double"))return(1);
	return(0);
}

int BGBCC_InferIsNumP(BCCX_Node *t)
{
	if(BGBCC_InferIsIntP(t))return(1);
	if(BGBCC_InferIsLongP(t))return(1);
	if(BGBCC_InferIsFloatP(t))return(1);
	if(BGBCC_InferIsDoubleP(t))return(1);
	return(0);
}

BCCX_Node *BGBCC_InferIsInt(BCCX_Node *t)
{
	if(BGBCC_InferIsIntP(t))
		return(KEYSYM("int"));
	return(BS1_MM_NULL);
}

BCCX_Node *BGBCC_InferIsLong(BCCX_Node *t)
{
	if(BGBCC_InferIsIntP(t))
		return(KEYSYM("int"));
	if(BGBCC_InferIsLongP(t))
		return(KEYSYM("long"));
	return(BS1_MM_NULL);
}

BCCX_Node *BGBCC_InferIsNum(BCCX_Node *t)
{
	if(BGBCC_InferIsIntP(t))
		return(KEYSYM("int"));
	if(BGBCC_InferIsLongP(t))
		return(KEYSYM("long"));
	if(BGBCC_InferIsFloatP(t))
		return(KEYSYM("float"));
	if(BGBCC_InferIsDoubleP(t))
		return(KEYSYM("double"));
	return(BS1_MM_NULL);
}

BCCX_Node *BGBCC_InferForm(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *c, t, x, n;
	int i;

	l=BGBCC_StripForm(l);

	if(FORM_IS(l, "cast"))
	{
		t=BGBCC_InferExpr(ctx, CADDR(l));
		return(CADR(l));
	}

	if((FORM_IS(l, "unary")) && (CADR(l)==SYM("!")))
	{
		t=BGBCC_InferExpr(ctx, CADDR(l));
		return(KEYSYM("int"));
	}

	if((FORM_IS(l, "unary")) && (CADR(l)==SYM("~")))
	{
		t=BGBCC_InferExpr(ctx, CADR(l));
		return(BGBCC_InferIsLong(t));
	}

//	if((FORM_IS(l, "-")) && !BS1_CONSP(CDDR(l)))
//	{
//		t=BGBCC_InferExpr(ctx, CADR(l));
//		return(BGBCC_InferIsNum(t));
//	}

	if(FORM_IS(l, "binary") && CADR(l)==SYM("/"))
	{
		t=BGBCC_InferExpr(ctx, CADDR(l));
		x=BGBCC_InferExpr(ctx, CADDDR(l));

		i=BGBCC_InferIsNumP(t) &&
			BGBCC_InferIsNumP(x);
		if(!i)return(BS1_MM_NULL);

		n=KEYSYM("float");
		if(t==KEYSYM("double"))n=t;
		if(x==KEYSYM("double"))n=x;
		return(n);
	}

	if(FORM_IS(l, "binary") && (CADR(l)==SYM("\\")))
	{
		t=BGBCC_InferExpr(ctx, CADDR(l));
		x=BGBCC_InferExpr(ctx, CADDDR(l));

		return(KEYSYM("int"));
	}

//	if((FORM_IS(l, "+")) || (FORM_IS(l, "-")) ||
//		(FORM_IS(l, "*")) ||
//		(FORM_IS(l, "%")) || (FORM_IS(l, "&")) ||
//		(FORM_IS(l, "|")) || (FORM_IS(l, "^")))

	if(FORM_IS(l, "binary") && (
		(CADR(l)==SYM("+")) || (CADR(l)==SYM("-")) ||
		(CADR(l)==SYM("*")) || (CADR(l)==SYM("%")) ||
		(CADR(l)==SYM("&")) || (CADR(l)==SYM("|")) ||
		(CADR(l)==SYM("^"))))
	{
		t=BGBCC_InferExpr(ctx, CADDR(l));

		x=t; c=CDDDR(l);
		while(BS1_CONSP(c))
		{
			t=BGBCC_InferExpr(ctx, CAR(c));
			if(t!=x)return(BS1_MM_NULL);
			c=CDR(c);
		}
		return(x);
	}

	if(FORM_IS(l, "binary") &&
		((CADR(l)==SYM("<<")) || (CADR(l)==SYM(">>"))))
	{
		t=BGBCC_InferExpr(ctx, CADDR(l));
		x=BGBCC_InferExpr(ctx, CADDDR(l));

		i=BGBCC_InferIsIntP(t) &&
			BGBCC_InferIsIntP(x);
		if(i)return(t);
		return(BS1_MM_NULL);
	}

//	if((FORM_IS(l, "==")) || (FORM_IS(l, "===")) ||
//		(FORM_IS(l, "!=")) || (FORM_IS(l, "!==")) ||
//		(FORM_IS(l, "<")) || (FORM_IS(l, ">")) ||
//		(FORM_IS(l, "<=")) || (FORM_IS(l, ">=")))

	if(FORM_IS(l, "binary") && (
		(CADR(l)==SYM("==")) || (CADR(l)==SYM("===")) ||
		(CADR(l)==SYM("!=")) || (CADR(l)==SYM("!==")) ||
		(CADR(l)==SYM("<"))  || (CADR(l)==SYM(">")) ||
		(CADR(l)==SYM("<=")) || (CADR(l)==SYM(">="))))
	{
		return(KEYSYM("int"));
//		return(KEYSYM("bool"));
	}

	if(FORM_IS(l, "if"))
	{
		t=BGBCC_ReduceExpr(ctx, CADR(l));
		i=BGBCC_BoolExpr(ctx, t);
		if(i==1)
		{
			t=BGBCC_InferExpr(ctx, CADDR(l));
			return(t);
		}
		if(i==0)
		{
			t=BS1_MM_NULL;
			if(BS1_CONSP(CDDDR(l)))
				t=BGBCC_InferExpr(ctx, CADDDR(l));
			return(t);
		}

		x=BS1_MM_NULL;
		t=BGBCC_InferExpr(ctx, CADDR(l));
		if(BS1_CONSP(CDDDR(l)))
			x=BGBCC_InferExpr(ctx, CADDDR(l));
		if(t!=x)return(BS1_MM_NULL);
		return(t);
	}

	if(FORM_IS(l, "funcall"))
	{
#if 0
		printf("Infer Func: ");
		BS1_TY_Print(CADR(l));
		printf("=");
		BS1_TY_Print(ctx->cf_n);
		printf("\n");
#endif

		ctx->i_cx_p=0;
		if(CADR(l)==ctx->cf_n)
		{
			t=BGBCC_InferTypeBase(ctx->cf_ty);
			return(t);
//			return(ctx->cf_ty);
		}
		return(BS1_MM_NULL);
	}

	ctx->i_cx_p=0;
	return(BS1_MM_NULL);
}

BCCX_Node *BGBCC_InferExpr(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *t, cn, cv, ct;

	l=BGBCC_ReduceExpr(ctx, l);

//	printf("Infer Reduce: ");
//	BS1_TY_PrintLN(l);

	if(BS1_CONSP(l))
	{
		t=BGBCC_InferForm(ctx, l);
		return(t);
	}

	if(BS1_SYMBOLP(l))
	{
		if(l==SYM("I"))return(KEYSYM("complex"));
		if(l==SYM("PI"))return(KEYSYM("float"));
		if(l==SYM("E"))return(KEYSYM("float"));

		cn=ctx->lenv;
		ct=ctx->tenv;
		cv=ctx->venv;

		while(BS1_CONSP(cn))
		{
			if(CAR(cn)==l)
			{
				t=BGBCC_InferTypeBase(CAR(ct));
				return(t);
			}
			cn=CDR(cn);
			ct=CDR(ct);
			cv=CDR(cv);
		}

		return(BS1_MM_NULL);
	}

	if(l==BS1_MM_NULL)return(KEYSYM("null"));
	if(l==BS1_MM_TRUE)return(KEYSYM("bool"));
	if(l==BS1_MM_FALSE)return(KEYSYM("bool"));

	if(BS1_FIXNUMP(l))return(KEYSYM("int"));
	if(BS1_FLONUMP(l))return(KEYSYM("float"));
	if(BS1_CHARP(l))return(KEYSYM("char"));

	if(BS1_STRINGP(l))return(KEYSYM("string"));
	if(BS1_KEYWORDP(l))return(KEYSYM("keyword"));

	if(BS1_ARRAYP(l))return(KEYSYM("array"));
	if(BS1_FVECTORP(l))return(KEYSYM("vector"));

	return(BS1_MM_NULL);
}

BCCX_Node *BGBCC_InferExprClean(BGBCC_State *ctx, BCCX_Node *l, int *cp)
{
	BCCX_Node *t;

	ctx->i_cx_p=1;
	t=BGBCC_InferExpr(ctx, l);
	*cp=ctx->i_cx_p;

	return(t);
}


int BGBCC_GuessCloseForm(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *c;
	int i;

	l=BGBCC_StripForm(l);

	if(FORM_IS(l, "lambda"))return(1);
	if(FORM_IS(l, "rlambda"))return(1);
	if(FORM_IS(l, "defun"))return(1);
	if(FORM_IS(l, "define") && BS1_CONSP(CADR(l)))
		return(1);

	c=l; i=0;
	while(BS1_CONSP(c))
	{
		i=i||BGBCC_GuessCloseExpr(ctx, CAR(c));
		c=CDR(c);
	}
	return(i);
}

int BGBCC_GuessCloseExpr(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *c;
	int i;

	if(BS1_CONSP(l))
		return(BGBCC_GuessCloseForm(ctx, l));

#if 0
	if(BS1_SYMBOLP(l))
	{
		c=ctx->lenv; i=0;
		while(BS1_CONSP(c))
		{
			if(c==ctx->llenv)i=1;
			if(CAR(c)==l)break;
			c=CDR(c);
		}

		if(BS1_CONSP(c))return(i);
		return(0);
	}
#endif

	return(0);
}

int BGBCC_GuessCloseBlock(BGBCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *c;
	int i;

	c=l; i=0;
	while(BS1_CONSP(c))
	{
		i=i||BGBCC_GuessCloseExpr(ctx, CAR(c));
		c=CDR(c);
	}
	return(i);
}

#endif
