#include <bgbccc.h>

int BGBCC_FrCC_IsUnaryP(BGBCC_FrCC_State *ctx, BCCX_Node *l, char *op)
{
	return(BCCX_TagAttrIsP(l, "unary", "op", op));
}

int BGBCC_FrCC_IsBinaryP(BGBCC_FrCC_State *ctx, BCCX_Node *l, char *op)
{
	return(BCCX_TagAttrIsP(l, "binary", "op", op));
}

int BGBCC_FrCC_BoolExpr(BGBCC_FrCC_State *ctx, BCCX_Node *l)
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
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));
		i=BGBCC_FrCC_BoolExpr(ctx, t);
		return(i);
	}

	if(BCCX_TagIsP(l, "boolify"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));
		i=BGBCC_FrCC_BoolExpr(ctx, t);
		return(i);
	}

	if(BCCX_TagIsP(l, "string"))
		return(1);

	return(-1);
}

BCCX_Node *BGBCC_FrCC_WrapIntSuf(s64 i, char *suf)
{
	BCCX_Node *t;
	t=BCCX_New("int");
	BCCX_SetInt(t, "value", i);
	if(suf)BCCX_Set(t, "tysuf", suf);
	return(t);
}

BCCX_Node *BGBCC_FrCC_WrapRealSuf(double f, char *suf)
{
	BCCX_Node *t;
	t=BCCX_New("real");
	BCCX_SetFloat(t, "value", f);
	if(suf)BCCX_Set(t, "tysuf", suf);
	return(t);
}

BCCX_Node *BGBCC_FrCC_WrapInt(s64 i)
{
	BCCX_Node *t;
	t=BCCX_New("int");
	BCCX_SetInt(t, "value", i);
	return(t);
}

BCCX_Node *BGBCC_FrCC_WrapReal(double f)
{
	BCCX_Node *t;
	t=BCCX_New("real");
	BCCX_SetFloat(t, "value", f);
	return(t);
}

int BGBCC_FrCC_IsIntP(BGBCC_FrCC_State *ctx, BCCX_Node *l)
{
	if(BCCX_TagIsP(l, "int"))return(1);
	return(0);
}

int BGBCC_FrCC_IsRealP(BGBCC_FrCC_State *ctx, BCCX_Node *l)
{
	if(BCCX_TagIsP(l, "int"))return(1);
	if(BCCX_TagIsP(l, "real"))return(1);
	return(0);
}

BCCX_Node *BGBCC_FrCC_Boolify(BGBCC_FrCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *t;
	if(BCCX_TagIsP(l, "boolify"))
		return(l);
	t=BCCX_New1("boolify", BCCX_New1("value", l));
	return(t);
}

//deal with number type suffixes
char *BGBCC_FrCC_BinaryTySuf(BGBCC_FrCC_State *ctx, char *op,
	BCCX_Node *ln, BCCX_Node *rn)
{
	char *s0, *s1;

//	return(NULL);

	if(!ln || !rn)
		return(NULL);

	s0=BCCX_Get(ln, "tysuf");
	s1=BCCX_Get(rn, "tysuf");

	//throw out bad suffixes early
	if(s0 && BGBCC_FrCC_IsIntP(ctx, ln))
	{
		if(	!stricmp(s0, "F") ||
			!stricmp(s0, "D") ||
			!stricmp(s0, "M"))s0=NULL;
	}
	if(s1 && BGBCC_FrCC_IsIntP(ctx, rn))
	{
		if(	!stricmp(s1, "F") ||
			!stricmp(s1, "D") ||
			!stricmp(s1, "M"))s1=NULL;
	}

	if(s0 && BGBCC_FrCC_IsRealP(ctx, ln))
	{
		if(	!stricmp(s0, "L") ||
			!stricmp(s0, "LL") ||
			!stricmp(s0, "UL") ||
			!stricmp(s0, "ULL"))s0=NULL;
	}
	if(s1 && BGBCC_FrCC_IsRealP(ctx, rn))
	{
		if(	!stricmp(s1, "L") ||
			!stricmp(s1, "LL") ||
			!stricmp(s1, "UL") ||
			!stricmp(s1, "ULL"))s1=NULL;
	}

	//int+real, throw out int suffix
	if(BGBCC_FrCC_IsIntP(ctx, ln) && BGBCC_FrCC_IsRealP(ctx, rn)) s0=NULL;
	if(BGBCC_FrCC_IsRealP(ctx, ln) && BGBCC_FrCC_IsIntP(ctx, rn)) s1=NULL;
	
	if(!s0 && !s1) return(NULL);
	if(!s0) return(s1);
	if(!s1) return(s0);
	
	if(!strcmp(s0, s1))
		return(s0);

	//throw away 'F', as it is lesser
	if(!strcmp(s0, "F"))s0=NULL;
	if(!strcmp(s1, "F"))s1=NULL;
	if(!s0 && !s1) return(NULL);
	if(!s0) return(s1);
	if(!s1) return(s0);
	
	//ok, whatever for now...
	
	return(NULL);
}

BCCX_Node *BGBCC_FrCC_ReduceForm(BGBCC_FrCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *c, *t, *v, *x, *ln, *rn;
	char *s, *suf;
	double f, g;
	s64 i, j;
//	int i, j;

	if(!l)return(l);

	if(BCCX_TagIsP(l, "value"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "left"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "right"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "cond"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "then"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "else"))return(BCCX_Child(l));
	if(BCCX_TagIsP(l, "body"))return(BCCX_Child(l));

	if(BGBCC_FrCC_IsUnaryP(ctx, l, "!"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));

		if(BGBCC_FrCC_BoolExpr(ctx, t)==0)
			return(BGBCP_Number(NULL, "1"));
		if(BGBCC_FrCC_BoolExpr(ctx, t)==1)
			return(BGBCP_Number(NULL, "0"));

		ln=BCCX_Fetch(t, "left");
		rn=BCCX_Fetch(t, "right");

		ln=BGBCC_FrCC_ReduceExpr(ctx, ln);
		rn=BGBCC_FrCC_ReduceExpr(ctx, rn);

		x=NULL;
		if(BGBCC_FrCC_IsBinaryP(ctx, t, "=="))
			x=BGBCP_BinaryExpr("!=", ln, rn);
		if(BGBCC_FrCC_IsBinaryP(ctx, t, "!="))
			x=BGBCP_BinaryExpr("==", ln, rn);
		if(BGBCC_FrCC_IsBinaryP(ctx, t, "==="))
			x=BGBCP_BinaryExpr("!==", ln, rn);
		if(BGBCC_FrCC_IsBinaryP(ctx, t, "!=="))
			x=BGBCP_BinaryExpr("===", ln, rn);
		if(BGBCC_FrCC_IsBinaryP(ctx, t, "<"))
			x=BGBCP_BinaryExpr(">=", ln, rn);
		if(BGBCC_FrCC_IsBinaryP(ctx, t, ">"))
			x=BGBCP_BinaryExpr("<=", ln, rn);
		if(BGBCC_FrCC_IsBinaryP(ctx, t, "<="))
			x=BGBCP_BinaryExpr(">", ln, rn);
		if(BGBCC_FrCC_IsBinaryP(ctx, t, ">="))
			x=BGBCP_BinaryExpr("<", ln, rn);

		if(x)
		{
			x=BGBCC_FrCC_ReduceExpr(ctx, x);
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
				i=BGBCC_FrCC_BoolExpr(ctx, BCCX_Child(c));
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
		if(!x)return(BGBCC_FrCC_ReduceExpr(ctx, c));

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
				i=BGBCC_FrCC_BoolExpr(ctx, BCCX_Child(c));
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
		if(!x)return(BGBCC_FrCC_ReduceExpr(ctx, c));

		t=BCCX_New1("or", t);
		return(t);
	}
#endif

	if(BGBCC_FrCC_IsBinaryP(ctx, l, "&&"))
	{
		ln=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "left"));
		rn=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "right"));

		if((BGBCC_FrCC_BoolExpr(ctx, ln)==0) ||
			(BGBCC_FrCC_BoolExpr(ctx, rn)==0))
				return(BGBCP_Number(NULL, "0"));

		if(BGBCC_FrCC_BoolExpr(ctx, ln)==1)
		{
			t=BGBCC_FrCC_Boolify(ctx, rn);
			return(t);
		}

		if(BGBCC_FrCC_BoolExpr(ctx, rn)==1)
		{
			t=BGBCC_FrCC_Boolify(ctx, ln);
			return(t);
		}

		x=BGBCP_BinaryExpr("&&", ln, rn);
		return(x);
	}

	if(BGBCC_FrCC_IsBinaryP(ctx, l, "||"))
	{
		ln=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "left"));
		rn=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "right"));

		if((BGBCC_FrCC_BoolExpr(ctx, ln)==1) ||
			(BGBCC_FrCC_BoolExpr(ctx, rn)==1))
				return(BGBCP_Number(NULL, "1"));

		if(BGBCC_FrCC_BoolExpr(ctx, ln)==0)
		{
			t=BGBCC_FrCC_Boolify(ctx, rn);
			return(t);
		}

		if(BGBCC_FrCC_BoolExpr(ctx, rn)==0)
		{
			t=BGBCC_FrCC_Boolify(ctx, ln);
			return(t);
		}

		x=BGBCP_BinaryExpr("||", ln, rn);
		return(x);
	}

	if(BCCX_TagIsP(l, "unary"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "value"));
		s=BCCX_Get(l, "op");

		if(BGBCC_FrCC_IsIntP(ctx, t))
		{
			i=BCCX_GetInt(t, "value");
			suf=BCCX_Get(l, "tysuf");

			if(!strcmp(s, "+"))return(BGBCC_FrCC_WrapIntSuf(i, suf));
			if(!strcmp(s, "-"))return(BGBCC_FrCC_WrapIntSuf(-i, suf));
			if(!strcmp(s, "~"))return(BGBCC_FrCC_WrapIntSuf(~i, suf));
			if(!strcmp(s, "!"))return(BGBCC_FrCC_WrapInt(!i));
		}

		if(BGBCC_FrCC_IsRealP(ctx, t))
		{
			f=BCCX_GetFloat(t, "value");
			suf=BCCX_Get(l, "tysuf");
			if(!strcmp(s, "+"))return(BGBCC_FrCC_WrapRealSuf(f, suf));
			if(!strcmp(s, "-"))return(BGBCC_FrCC_WrapRealSuf(-f, suf));
			if(!strcmp(s, "!"))return(BGBCC_FrCC_WrapInt(f!=0));
		}

		x=BGBCP_UnaryExpr(s, t);
		return(x);
//		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "binary"))
	{
		ln=BCCX_Fetch(l, "left");
		rn=BCCX_Fetch(l, "right");

		ln=BGBCC_FrCC_ReduceExpr(ctx, ln);
		rn=BGBCC_FrCC_ReduceExpr(ctx, rn);

		s=BCCX_Get(l, "op");

		if(BGBCC_FrCC_IsIntP(ctx, ln) && BGBCC_FrCC_IsIntP(ctx, rn))
		{
			i=BCCX_GetInt(ln, "value");
			j=BCCX_GetInt(rn, "value");

			if(!strcmp(s, "=="))return(BGBCC_FrCC_WrapInt(i==j));
			if(!strcmp(s, "!="))return(BGBCC_FrCC_WrapInt(i!=j));
			if(!strcmp(s, "==="))return(BGBCC_FrCC_WrapInt(i==j));
			if(!strcmp(s, "!=="))return(BGBCC_FrCC_WrapInt(i!=j));
			if(!strcmp(s, "<="))return(BGBCC_FrCC_WrapInt(i<=j));
			if(!strcmp(s, ">="))return(BGBCC_FrCC_WrapInt(i>=j));
			if(!strcmp(s, "<"))return(BGBCC_FrCC_WrapInt(i<j));
			if(!strcmp(s, ">"))return(BGBCC_FrCC_WrapInt(i>j));

			suf=BGBCC_FrCC_BinaryTySuf(ctx, s, ln, rn);
			if(!strcmp(s, "+"))return(BGBCC_FrCC_WrapIntSuf(i+j, suf));
			if(!strcmp(s, "-"))return(BGBCC_FrCC_WrapIntSuf(i-j, suf));
			if(!strcmp(s, "*"))return(BGBCC_FrCC_WrapIntSuf(i*j, suf));
			if(!strcmp(s, "/"))return(BGBCC_FrCC_WrapIntSuf(i/j, suf));
			if(!strcmp(s, "%"))return(BGBCC_FrCC_WrapIntSuf(i%j, suf));

			if(!strcmp(s, "&"))return(BGBCC_FrCC_WrapIntSuf(i&j, suf));
			if(!strcmp(s, "|"))return(BGBCC_FrCC_WrapIntSuf(i|j, suf));
			if(!strcmp(s, "^"))return(BGBCC_FrCC_WrapIntSuf(i^j, suf));
			if(!strcmp(s, "\\"))return(BGBCC_FrCC_WrapIntSuf(i/j, suf));
//			if(!strcmp(s, "**"))return(BGBCC_FrCC_WrapInt(pow(i, j)));

			if(!strcmp(s, "&&"))return(BGBCC_FrCC_WrapInt(i&&j));
			if(!strcmp(s, "||"))return(BGBCC_FrCC_WrapInt(i||j));

			if(!strcmp(s, "<<"))return(BGBCC_FrCC_WrapIntSuf(i<<j, suf));
			if(!strcmp(s, ">>"))return(BGBCC_FrCC_WrapIntSuf(i>>j, suf));
		}

		if(BGBCC_FrCC_IsRealP(ctx, ln) && BGBCC_FrCC_IsRealP(ctx, rn))
		{
			f=BCCX_GetFloat(ln, "value");
			g=BCCX_GetFloat(rn, "value");

			if(!strcmp(s, "=="))return(BGBCC_FrCC_WrapInt(f==g));
			if(!strcmp(s, "!="))return(BGBCC_FrCC_WrapInt(f!=g));
			if(!strcmp(s, "==="))return(BGBCC_FrCC_WrapInt(f==g));
			if(!strcmp(s, "!=="))return(BGBCC_FrCC_WrapInt(f!=g));
			if(!strcmp(s, "<="))return(BGBCC_FrCC_WrapInt(f<=g));
			if(!strcmp(s, ">="))return(BGBCC_FrCC_WrapInt(f>=g));
			if(!strcmp(s, "<"))return(BGBCC_FrCC_WrapInt(f<g));
			if(!strcmp(s, ">"))return(BGBCC_FrCC_WrapInt(f>g));

			suf=BGBCC_FrCC_BinaryTySuf(ctx, s, ln, rn);

			if(!strcmp(s, "+"))return(BGBCC_FrCC_WrapRealSuf(f+g, suf));
			if(!strcmp(s, "-"))return(BGBCC_FrCC_WrapRealSuf(f-g, suf));
			if(!strcmp(s, "*"))return(BGBCC_FrCC_WrapRealSuf(f*g, suf));
			if(!strcmp(s, "/"))return(BGBCC_FrCC_WrapRealSuf(f/g, suf));
			if(!strcmp(s, "\\"))return(BGBCC_FrCC_WrapInt(f/g));
//			if(!strcmp(s, "**"))return(BGBCC_FrCC_WrapReal(pow(i, j)));
		}

		x=BGBCP_BinaryExpr(s, ln, rn);
		return(x);
	}

	if(BCCX_TagIsP(l, "if"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "cond"));
		i=BGBCC_FrCC_BoolExpr(ctx, t);
		if(i==1)
		{
			t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "then"));
			return(t);
		}
		if(i==0)
		{
			t=BCCX_Fetch(l, "else");
			if(t)t=BGBCC_FrCC_ReduceExpr(ctx, t);
			return(t);
		}

		ln=BCCX_Fetch(l, "then");
		rn=BCCX_Fetch(l, "else");

		ln=BGBCC_FrCC_ReduceExpr(ctx, ln);
		rn=BGBCC_FrCC_ReduceExpr(ctx, rn);
		x=BGBCP_IfExpr(t, ln, rn);
		return(x);

//		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "begin"))
	{
		if(!BCCX_Child(l))
		{
			return(NULL);
		}

		if(!BCCX_Next(BCCX_Child(l)))
		{
			t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Child(l));
			return(t);
		}
		
		x=BCCX_New("begin");
		c=BCCX_Child(l);
		while(c)
		{
			t=BGBCC_FrCC_ReduceExpr(ctx, c);
			BCCX_Add(x, t);
			c=BCCX_Next(c);
		}
		return(x);

//		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "ref"))
	{
		if(!ctx)	//preprocessor
		{
			t=BGBCC_FrCC_WrapInt(0);
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
			i=BGBCC_FrCC_TryGetSizeofType(ctx, t);
			if(i>0)return(BGBCC_FrCC_WrapInt(i));
		}
	}

	return(BCCX_Clone(l));
}

int BGBCC_FrCC_SetLine(BGBCC_FrCC_State *ctx, BCCX_Node *l,
	char *fn, int ln)
{
	if(fn)BCCX_Set(l, "fn", fn);
	if(ln>0)BCCX_SetInt(l, "ln", ln);
	return(0);
}

BCCX_Node *BGBCC_FrCC_ReduceStatementForm(BGBCC_FrCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *c, *t, *v, *x, *ln, *rn;
	char *s, *fnam;
	double f, g;
	int i, j, lnum;

	if(!l)return(l);

	fnam=BCCX_Get(l, "fn");
	lnum=BCCX_GetInt(l, "ln");

	if(BCCX_TagIsP(l, "if"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "cond"));
		i=BGBCC_FrCC_BoolExpr(ctx, t);
		if(i==1)
		{
			t=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "then"));
			return(t);
		}
		if(i==0)
		{
			t=BCCX_Fetch(l, "else");
			if(t)t=BGBCC_FrCC_ReduceStatement(ctx, t);
			return(t);
		}

		ln=BCCX_Fetch(l, "then");
		rn=BCCX_Fetch(l, "else");

		ln=BGBCC_FrCC_ReduceStatement(ctx, ln);
		rn=BGBCC_FrCC_ReduceStatement(ctx, rn);
		x=BGBCP_IfExpr(t, ln, rn);
		BGBCC_FrCC_SetLine(ctx, x, fnam, lnum);
		return(x);

//		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "begin"))
	{
		if(!BCCX_Child(l))
		{
			return(NULL);
		}

		if(!BCCX_Next(BCCX_Child(l)))
		{
			t=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Child(l));
			return(t);
		}
		
		x=BCCX_New("begin");
		c=BCCX_Child(l);
		while(c)
		{
			t=BGBCC_FrCC_ReduceStatement(ctx, c);
			BCCX_Add(x, t);
			c=BCCX_Next(c);
		}

		BGBCC_FrCC_SetLine(ctx, x, fnam, lnum);
		return(x);

//		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "while"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "cond"));
		i=BGBCC_FrCC_BoolExpr(ctx, t);
		if(i==0)
		{
			return(NULL);
		}

		v=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "body"));
		if(!v)return(NULL);

		x=BCCX_New2("while",
			BCCX_New1("cond", t),
			BCCX_New1("body", v));

		BGBCC_FrCC_SetLine(ctx, x, fnam, lnum);
		return(x);

//		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "do_while"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "cond"));
		i=BGBCC_FrCC_BoolExpr(ctx, t);
		if(i==0)
		{
			v=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "body"));
			return(v);
		}

		v=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "body"));
		if(!v)return(NULL);

		x=BCCX_New2("do_while",
			BCCX_New1("cond", t),
			BCCX_New1("body", v));
		BGBCC_FrCC_SetLine(ctx, x, fnam, lnum);

		return(x);

//		return(BCCX_Clone(l));
	}

	if(BCCX_TagIsP(l, "for"))
	{
		t=BGBCC_FrCC_ReduceExpr(ctx, BCCX_Fetch(l, "cond"));
		i=BGBCC_FrCC_BoolExpr(ctx, t);
		if(i==0)
		{
			ln=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "init"));
			return(ln);
//			v=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "body"));
//			t=BCCX_New2("begin", ln, v);
//			x=BGBCC_FrCC_ReduceStatement(ctx, t);
//			return(x);
		}

		ln=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "init"));
		rn=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "step"));

		v=BGBCC_FrCC_ReduceStatement(ctx, BCCX_Fetch(l, "body"));
		//if(!v)return(NULL);

		x=BCCX_New4("for",
			BCCX_New1("init", ln),
			BCCX_New1("cond", t),
			BCCX_New1("step", rn),
			BCCX_New1("body", v));
		BGBCC_FrCC_SetLine(ctx, x, fnam, lnum);

		return(x);

//		return(BCCX_Clone(l));
	}

	t=BGBCC_FrCC_ReduceForm(ctx, l);
	return(t);
}

BCCX_Node *BGBCC_FrCC_ReduceExpr(BGBCC_FrCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *t;
	if(!l)return(NULL);
	t=BGBCC_FrCC_ReduceForm(ctx, l);
	return(t);
}

BCCX_Node *BGBCC_FrCC_ReduceStatement(BGBCC_FrCC_State *ctx, BCCX_Node *l)
{
	BCCX_Node *t;
	if(!l)return(NULL);
	t=BGBCC_FrCC_ReduceStatementForm(ctx, l);
	return(t);
}