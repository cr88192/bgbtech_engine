#include <bgbccc.h>

//Lit:		Literal values/builtin expressions
//PE:		(<expr>) <expr>[<expr>] <expr>.<name>
//IncDec:	++<name> --<name> + - ! ~ ... <name>++ <name>-- <expr>(<args>)       
//E:		<expr>**<expr>
//MD:		* / % \ &	*. /. %. \. &.
//AS:		+ - | ^		+. -. |. ^.
//SHLR:		<< >> <<< >>>
//RCmp:		< > <= >= == === != <<== >>== <<<== >>>==
//Lop:		&&
//Lop2:		|| ^^
//TCond:	<expr>?<expr>:<expr>
//Equals:	= += -= *= /= \= %= &= |= ^= >>= <<=

BGBCP_ParseItem *bgbjp_exprs=NULL;

int BGBJP_AddExpression(char *name, BCCX_Node *(*func)(BGBCP_ParseState *ctx, char **s))
{
	BGBCP_ParseItem *tmp;

	tmp=malloc(sizeof(BGBCP_ParseItem));
	tmp->name=strdup(name);
	tmp->func=func;

	tmp->next=bgbjp_exprs;
	bgbjp_exprs=tmp;

	return(0);
}

BCCX_Node *BGBJP_ParseExpressionName(
	BGBCP_ParseState *ctx, char *name, char **s)
{
	BGBCP_ParseItem *cur;

	cur=bgbjp_exprs;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur->func(ctx, s));
		cur=cur->next;
	}
	return(NULL);
}

BCCX_Node *BGBJP_Number(BGBCP_ParseState *ctx, char *str)
{
	BCCX_Node *n;
	char *s;
	int i;

	s=str;
	while(*s && (*s!='.') && (*s!='e') && (*s!='E')) s++;

	if(!*s)
	{
		n=BCCX_New("int");
		BCCX_Set(n, "value", str);
		return(n);
	}

	n=BCCX_New("real");
	BCCX_Set(n, "value", str);
	return(n);
}

BCCX_Node *BGBJP_ExpressionFunction(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2;
	int i;

	s=*str;

	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "[") && (ty==BTK_BRACE))
	{
		s=BGBJP_Token(s, b, &ty);	//[
		n1=BGBJP_Expression(ctx, &s);
		s=BGBJP_Token(s, b, &ty);	//]
		n=BCCX_New1("lambda_expr", BCCX_New1("value", n1));
		*str=s;
		return(n);
	}

	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "{") && (ty==BTK_BRACE))
	{
//		s=BGBJP_Token(s, b, &ty);	//[
//		n1=BGBJP_Expression(ctx, &s);
//		s=BGBJP_Token(s, b, &ty);	//]

		n1=BGBJP_BlockStatement2(ctx, &s);

		n=BCCX_New1("lambda", BCCX_New1("body", n1));
		*str=s;
		return(n);
	}

	n=BCCX_New("lambda");

	n1=BGBJP_DefType(ctx, &s);
	if(n1)BCCX_Add(n, n1);

	BGBJP_Token(s, b, &ty);
	if(ty==BTK_NAME)
	{
		s=BGBJP_Token(s, b, &ty);
		BCCX_Set(n, "name", b);
	}

	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "("))
	{
		s=BGBJP_Token(s, b, &ty); //'('
		n1=BGBJP_FunVarsList(ctx, &s);
		BCCX_Add(n, BCCX_New1("args", n1));
	}

	n1=BGBJP_BlockStatement2(ctx, &s);
	BCCX_Add(n, BCCX_New1("body", n1));

	*str=s;
	return(n);

}

BCCX_Node *BGBJP_ExpressionLit(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2;
	int i;

	s=*str;

	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BTK_BRACE))
	{
		s=BGBJP_Token(s, b, &ty);	//(
		n=BGBJP_Expression(ctx, &s);
		s=BGBJP_Token(s, b, &ty);	//)

		if(strcmp(b, ")"))printf("parser, token '%s'\n", b);

//		n=BGBJP_FunArgs(ctx, &s);
//		if(CDR(n))n=CONS(SYM("begin"), n);

		*str=s;
		return(n);
	}

	s=*str;

	s=BGBJP_Token(s, b, &ty);
	if(!s)return(NULL);

	if((ctx->lang!=BGBCC_LANG_BS) &&
		(ctx->lang!=BGBCC_LANG_BS2))
	{
		if(!strcmp(b, "{") && (ty==BTK_BRACE))
		{
			n1=BGBJP_FunArgs(ctx, &s);
			n=BCCX_New1("list", n1);
			*str=s;
			return(n);
		}
	}

	if((ctx->lang==BGBCC_LANG_BS) ||
		(ctx->lang==BGBCC_LANG_BS2))
	{
		if(!strcmp(b, "#") && (ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty);
			BGBJP_Token(s, b2, &ty2);

			if(!strcmp(b, "{") && (ty==BTK_BRACE))
			{
				n1=BGBJP_FunArgs(ctx, &s);
				n=BCCX_New1("list", n1);
				*str=s;
				return(n);
			}

			if(!strcmp(b, "[") && (ty==BTK_BRACE))
			{
				n1=BGBJP_FunArgs(ctx, &s);
				n=BCCX_New1("vector", n1);
				*str=s;
				return(n);
			}

			if(!strcmp(b, ":") && (ty==BTK_SEPERATOR) && (ty2==BTK_NAME))
			{
				s=BGBJP_Token(s, b2, &ty2);
				n=BCCX_New("keyword");
				BCCX_Set(n, "name", b2);
				*str=s;
				return(n);
			}

			if(ty==BTK_NAME)
			{
				n=BCCX_New("symbol");
				BCCX_Set(n, "name", b);
				*str=s;
				return(n);
			}

			if(ty==BTK_CHARSTRING)
			{
				n=BCCX_New("ref");
				BCCX_Set(n, "name", b);
				*str=s;
				return(n);
			}

			if(ty==BTK_STRING)
			{
				n=BCCX_New("symbol");
				BCCX_Set(n, "name", b);
				*str=s;
				return(n);
			}

			if(ty==BTK_OPERATOR)
			{
				n=BCCX_New("symbol");
				BCCX_Set(n, "name", b);
				*str=s;
				return(n);
			}

		}

		if(!strcmp(b, "{") && (ty==BTK_BRACE))
		{
			n1=BGBJP_FunArgs2(ctx, &s);
			n=BCCX_New1("dict", n1);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "[") && (ty==BTK_BRACE))
		{
			n1=BGBJP_FunArgs(ctx, &s);
			n=BCCX_New1("array", n1);
			*str=s;
			return(n);
		}
	}

	if(ty==BTK_NAME)
	{
		BGBJP_Token(s, b2, &ty2);

		if(!strcmp(b, "new"))
		{
			n=BCCX_New("new");

			n1=BGBJP_DefType(ctx, &s);

			if(!n1)
			{
				BGBJP_Error(s, "invalid type for new\n");
				*str=NULL;
				return(NULL);
			}

			BCCX_Add(n, n1);

			BGBJP_Token(s, b, &ty);
			if(!strcmp(b, "("))
			{
				s=BGBJP_Token(s, b, &ty); //'('
				n1=BGBJP_FunArgs(ctx, &s);
				BCCX_Add(n, BCCX_New1("args", n1));
			}

			*str=s;
			return(n);
		}

		if(ctx->lang==BGBCC_LANG_BS2)
		{
			if(!strcmp(b, "ref"))
			{
				n1=BGBJP_Expression(ctx, &s);
				n=BCCX_New1("byref", BCCX_New1("value", n1));
				*str=s;
				return(n);
			}

			if(!strcmp(b, "out"))
			{
				n1=BGBJP_Expression(ctx, &s);
				n=BCCX_New1("byref_out", BCCX_New1("value", n1));
				*str=s;
				return(n);
			}

			if(!strcmp(b, "delete"))
			{
				n1=BGBJP_Expression(ctx, &s);
				n=BCCX_New1("delete", BCCX_New1("value", n1));
				*str=s;
				return(n);
			}

			if(!strcmp(b, "typeof"))
			{
				n1=BGBJP_Expression(ctx, &s);
				n=BCCX_New1("typeof", BCCX_New1("value", n1));
				*str=s;
				return(n);
			}
			
			
			if(!strcmp(b, "fun"))
			{
				n=BGBJP_ExpressionFunction(ctx, &s);
				*str=s;
				return(n);
			}
		}

		n=BGBJP_ParseExpressionName(ctx, b, &s);
		if(n)
		{
			*str=s;
			return(n);
		}

		n=BCCX_New("ref");
		BCCX_Set(n, "name", b);
		*str=s;
		return(n);
	}

	BGBJP_Token(s, b2, &ty2);

	if(ty==BTK_NUMBER)
	{
		BGBJP_Token(s, b2, &ty2);
		if((!strcmp(b2, "i") || !strcmp(b2, "iF") ||
			!strcmp(b2, "if")) && (ty2==BTK_NAME))
		{
			s=BGBJP_Token(s, b2, &ty2);
			if(!strcmp(b2, "i"))
			{
				n=BCCX_New("complex");
				BCCX_Set(n, "imag", b);
			}
			*str=s;
			return(n);

		}

		if(!strcmp(b2, "F") && (ty2==BTK_NAME))
		{
			s=BGBJP_Token(s, b2, &ty2);
			*str=s;
			return(BGBJP_Number(ctx, b));
		}

		if(!strcmp(b2, "D") && (ty2==BTK_NAME))
		{
			s=BGBJP_Token(s, b2, &ty2);
			*str=s;
			return(BGBJP_Number(ctx, b));
		}

		if(!strcmp(b2, "M") && (ty2==BTK_NAME))
		{
			s=BGBJP_Token(s, b2, &ty2);
			*str=s;
			return(BGBJP_Number(ctx, b));
		}

		if(!strcmp(b2, "L") && (ty2==BTK_NAME))
		{
			s=BGBJP_Token(s, b2, &ty2);
			*str=s;
			return(BGBJP_Number(ctx, b));
		}

		if(!strcmp(b2, "LL") && (ty2==BTK_NAME))
		{
			s=BGBJP_Token(s, b2, &ty2);
			*str=s;
			return(BGBJP_Number(ctx, b));
		}

		n=BGBJP_Number(ctx, b);
		*str=s;
		return(n);
	}

	if(ty==BTK_STRING)
	{
		BGBJP_Token(s, b2, &ty2);
		while(ty2==BTK_STRING)
		{
			s=BGBJP_Token(s, b2, &ty2);
			strcat(b, b2);
			BGBJP_Token(s, b2, &ty2);
		}

		n=BCCX_New("string");
		BCCX_Set(n, "value", b);

		*str=s;
		return(n);
	}

	if(ty==BTK_CHARSTRING)
	{
		BGBJP_Token(s, b2, &ty2);
		while(ty2==BTK_CHARSTRING)
		{
			s=BGBJP_Token(s, b2, &ty2);
			strcat(b, b2);
			BGBJP_Token(s, b2, &ty2);
		}

		n=BCCX_New("charstring");
		BCCX_Set(n, "value", b);

		*str=s;
		return(n);
	}

	return(NULL);
}

BCCX_Node *BGBJP_ExpressionPostfix(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;
	int i;

	s=*str;

	n=BGBJP_ExpressionLit(ctx, &s);
	if(!n) { return(n); }

	while(1)
	{
		BGBJP_Token(s, b, &ty);

		if(!strcmp(b, "[") && (ty==BTK_BRACE))
		{
			s=BGBJP_Token(s, b, &ty); //[
			n1=BGBJP_Expression(ctx, &s);
			s=BGBJP_Token(s, b, &ty); //]
			n=BCCX_New2("getindex",
				BCCX_New1("array", n),
				BCCX_New1("index", n1));
			continue;
		}

		if(!strcmp(b, ".") && (ty==BTK_SEPERATOR))
		{
			s=BGBJP_Token(s, b, &ty); //.
			s=BGBJP_Token(s, b, &ty); //name
			n=BCCX_New1("objref", BCCX_New1("value", n));
			BCCX_Set(n, "name", b);
			continue;
		}

		if(!strcmp(b, "->") && (ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty); //.
			s=BGBJP_Token(s, b, &ty); //name
			n=BCCX_New1("objref", BCCX_New1("value", n));
			BCCX_Set(n, "name", b);
			continue;
		}

#if 1
		if(!strcmp(b, "(") && (ty==BTK_BRACE))
		{
			s=BGBJP_Token(s, b, &ty); //'('
			n1=BGBJP_FunArgs(ctx, &s);
			if(!s)
			{
				*str=NULL;
				return(NULL);
			}

			if(BCCX_TagIsP(n, "objref"))
			{
				BCCX_SetTag(n, "methodcall");
				BCCX_Add(n, BCCX_New1("args", n1));
				continue;
			}

			if(BCCX_TagIsP(n, "ref"))
			{
				BCCX_SetTag(n, "funcall");
				BCCX_Add(n, BCCX_New1("args", n1));
				continue;
			}

			n=BCCX_New2("funcall",
				BCCX_New1("value", n),
				BCCX_New1("args", n1));
			continue;
		}

		if(!strcmp(b, "++") && (ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty);
			n=BCCX_New1("postinc", BCCX_New1("expr", n));
			continue;
		}
		if(!strcmp(b, "--") && (ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty);
			n=BCCX_New1("postdec", BCCX_New1("expr", n));
			continue;
		}
#endif

		break;
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionUnary(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	t=BGBJP_Token(s, b, &ty);
	BGBJP_Token(t, b2, &ty2);

	if(!strcmp(b, "++") && (ty==BTK_OPERATOR))
	{
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionUnary(ctx, &s);
		if(!n1)return(NULL);
		n=BCCX_New1("preinc", BCCX_New1("expr", n1));
		*str=s;
		return(n);
	}
	if(!strcmp(b, "--") && (ty==BTK_OPERATOR))
	{
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionUnary(ctx, &s);
		if(!n1)return(NULL);
		n=BCCX_New1("predec", BCCX_New1("expr", n1));
		*str=s;
		return(n);
	}

	if((!strcmp(b, "+") || !strcmp(b, "-") || !strcmp(b, "!") ||
		!strcmp(b, "~") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BTK_OPERATOR))
	{
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionCast(ctx, &s);
		if(!n1)return(NULL);
		n1=BCCX_New1("value", n1);
		n=BCCX_New1("unary", n1);
		BCCX_Set(n, "op", b);
		*str=s;
		return(n);
	}

#if 1
	if(!strcmp(b, "sizeof"))
	{
		s=BGBJP_Token(s, b, &ty);

		if(!strcmp(b2, "(") && (ty2==BTK_BRACE))
		{
			s=BGBJP_Token(s, b2, &ty2);

			n=BGBJP_ArgDefinition(ctx, &s);
			if(n)
			{
				n=BCCX_New1("sizeof", BCCX_New1("tyexpr", n));
			}else
			{
				n1=BGBJP_Expression(ctx, &s);
				n=BCCX_New1("sizeof_expr", BCCX_New1("value", n1));
			}

			s=BGBJP_Token(s, b2, &ty2);
			if(strcmp(b2, ")"))
				BGBJP_Error(s,
					"sizeof token '%s'\n", b2);

			*str=s;
			return(n);
		}

		n=BGBJP_ArgDefinition(ctx, &s);
		if(n)
		{
			n=BCCX_New1("sizeof", BCCX_New1("tyexpr", n));
		}else
		{
			n1=BGBJP_Expression(ctx, &s);
			n=BCCX_New1("sizeof_expr", BCCX_New1("value", n1));
		}

		*str=s;
		return(n);
	}
#endif

	s=*str;
	n=BGBJP_ExpressionPostfix(ctx, &s);
	if(!n)return(NULL);

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionCast(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t, *s1;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2;

	s=*str;
	n1=BGBJP_CastTypeExpression(ctx, &s);
	if(n1 && BCCX_TagIsP(n1, "type"))
	{
		n2=BGBJP_ExpressionCast(ctx, &s);
		n=BCCX_New2("cast", n1, BCCX_New1("value", n2));
		*str=s;
		return(n);
	}

	n=BGBJP_ExpressionUnary(ctx, &s);
	*str=s;
	return(n);

#if 0
	s=*str;
	s1=BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BTK_BRACE))
	{
		n=BGBJP_ArgDefinition(ctx, &s1);

		s1=BGBJP_Token(s1, b, &ty);
		if(!strcmp(b, ")"))
		{
			n1=BGBJP_ExpressionCast(ctx, &s1);
		}else n1=NULL;

		if(n && n1)
		{
			s=s1;

			n=BCCX_FindTag(n, "type");
			n=BCCX_Clone(n);

			n=BCCX_New2("cast", n, BCCX_New1("value", n1));

			*str=s;
			return(n);
		}
	}

	n=BGBJP_ExpressionUnary(ctx, str);
	return(n);
#endif
}

BCCX_Node *BGBJP_BinaryExpr(char *op, BCCX_Node *l, BCCX_Node *r)
{
	BCCX_Node *n, *n1, *n2;

	n1=BCCX_New1("left", l);
	n2=BCCX_New1("right", r);
	n=BCCX_New2("binary", n1, n2);
	BCCX_Set(n, "op", op);

	return(n);
}

BCCX_Node *BGBJP_ExpressionExp(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionCast(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		t=BGBJP_Token(s, b, &ty);
		if(ty!=BTK_OPERATOR)
			break;

		if(strcmp(b, "`") && strcmp(b, "``") &&
			strcmp(b, "&") && strcmp(b, "&`") && strcmp(b, "`&") &&
			strcmp(b, "|") && strcmp(b, "|`") && strcmp(b, "`|") &&
			strcmp(b, "^") && strcmp(b, "^`") && strcmp(b, "`^")
			)
			break;

		s=BGBJP_Token(s, b, &ty);
		if(!strcmp(b, "`"))strcpy(b, "**");

		n1=BGBJP_ExpressionCast(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionMD(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionExp(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		t=BGBJP_Token(s, b, &ty);
		if(ty!=BTK_OPERATOR)
			break;

		if(	strcmp(b, "*") && strcmp(b, "/") && strcmp(b, "%") &&
			strcmp(b, "*`") && strcmp(b, "/`") && strcmp(b, "%`") &&
			strcmp(b, "`*") && strcmp(b, "`/") && strcmp(b, "`%") &&
			strcmp(b, "~") && strcmp(b, "~`") && strcmp(b, "`~"))
			break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionExp(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionAS(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionMD(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		t=BGBJP_Token(s, b, &ty);
		if(ty!=BTK_OPERATOR)
			break;

		if(	strcmp(b, "+") && strcmp(b, "-") &&
			strcmp(b, "+`") && strcmp(b, "-`") &&
			strcmp(b, "`+") && strcmp(b, "`-"))
			break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionMD(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionSHLR(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionAS(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		t=BGBJP_Token(s, b, &ty);
		if(ty!=BTK_OPERATOR)
			break;
		if(strcmp(b, "<<") && strcmp(b, ">>") &&
			strcmp(b, "<<<") && strcmp(b, ">>>"))
			break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionAS(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionRCmp(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionSHLR(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		BGBJP_Token(s, b, &ty);

		if((ctx->lang==BGBCC_LANG_JAVA) ||
			(ctx->lang==BGBCC_LANG_BS) ||
			(ctx->lang==BGBCC_LANG_BS2))
		{
			if(!strcmp(b, "instanceof"))
			{
				s=BGBJP_Token(s, b, &ty);
//				n1=BGBJP_TypeExpression(ctx, &s);
//				n=BCCX_New2("instanceof",
//					BCCX_New1("value", n),
//					BCCX_New1("tyexpr", n1));

				n1=BGBJP_DefType(ctx, &s);
				n=BCCX_New2("instanceof",
					BCCX_New1("value", n), n1);
				continue;
			}
		}
		
		if((ctx->lang==BGBCC_LANG_CS) ||
			(ctx->lang==BGBCC_LANG_BS2))
		{
			if(!strcmp(b, "is"))
			{
				s=BGBJP_Token(s, b, &ty);
				n1=BGBJP_DefType(ctx, &s);
				n=BCCX_New2("instanceof",
					BCCX_New1("value", n), n1);
				continue;
			}
		}

		if(ctx->lang==BGBCC_LANG_BS2)
		{
			if(!strcmp(b, "as"))
			{
				s=BGBJP_Token(s, b, &ty);
				n1=BGBJP_DefType(ctx, &s);
//				n=BCCX_New2("instanceof",
//					BCCX_New1("value", n), n1);
				n=BCCX_New2("cast", n1, BCCX_New1("value", n));
				continue;
			}
		}


		if(ty!=BTK_OPERATOR)
			break;

		if(strcmp(b, "<") && strcmp(b, ">") &&
			strcmp(b, "<=") && strcmp(b, ">="))
			break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionSHLR(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionECmp(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionRCmp(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		BGBJP_Token(s, b, &ty);

		if(ty!=BTK_OPERATOR)
			break;
		if(strcmp(b, "==") && strcmp(b, "!="))
			break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionRCmp(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionBitAnd(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionECmp(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		t=BGBJP_Token(s, b, &ty);
		if(ty!=BTK_OPERATOR)break;
		if(strcmp(b, "&"))break;
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionECmp(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}
	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionBitXor(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionBitAnd(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		t=BGBJP_Token(s, b, &ty);
		if(ty!=BTK_OPERATOR)break;
		if(strcmp(b, "^"))break;
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionBitAnd(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}
	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionBitOr(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionBitXor(ctx, &s);
	if(!n)return(NULL);
	while(1)
	{
		t=BGBJP_Token(s, b, &ty);
		if(ty!=BTK_OPERATOR)break;
		if(strcmp(b, "|"))break;
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionBitXor(ctx, &s);
		if(!n1)return(NULL);
		n=BGBJP_BinaryExpr(b, n, n1);
	}
	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionLop(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionBitOr(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		BGBJP_Token(s, b, &ty);

		if(ty!=BTK_OPERATOR)break;
		if(strcmp(b, "&&"))break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionBitOr(ctx, &s);
		if(!n1)return(NULL);

		n=BGBJP_BinaryExpr("&&", n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionLop2(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionLop(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		BGBJP_Token(s, b, &ty);

		if(ty!=BTK_OPERATOR)
			break;
		if(strcmp(b, "||"))
			break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionLop(ctx, &s);
		if(!n1)return(NULL);

		n=BGBJP_BinaryExpr("||", n, n1);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionTCond(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n1=BGBJP_ExpressionLop2(ctx, &s);
	if(!n1)return(NULL);
	BGBJP_Token(s, b, &ty);
	if(strcmp(b, "?") || (ty!=BTK_OPERATOR))
	{
		*str=s;
		return(n1);
	}

	s=BGBJP_Token(s, b, &ty); //?
	n2=BGBJP_ExpressionLop2(ctx, &s);
	if(!n2)return(NULL);
	s=BGBJP_Token(s, b, &ty); //:
	n3=BGBJP_ExpressionTCond(ctx, &s);
	if(!n3)return(NULL);

	n=BCCX_New3("if",
		BCCX_New1("cond", n1),
		BCCX_New1("then", n2),
		BCCX_New1("else", n3));

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionEquals(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionTCond(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		BGBJP_Token(s, b, &ty);

		if(ty!=BTK_OPERATOR)
			break;

		if(strcmp(b, "=") && strcmp(b, "+=") && strcmp(b, "-=") &&
		strcmp(b, "*=") && strcmp(b, "/=")  && strcmp(b, "%=")  &&
		strcmp(b, "&=") && strcmp(b, "|=") &&
		strcmp(b, "^=") && strcmp(b, "<<=") && strcmp(b, ">>=") &&
		strcmp(b, "<<<=") && strcmp(b, ">>>="))
			break;

		t=NULL;
		if(!strcmp(b, "+="))t="+";
		if(!strcmp(b, "-="))t="-";
		if(!strcmp(b, "*="))t="*";
		if(!strcmp(b, "/="))t="/";
		if(!strcmp(b, "%="))t="%";
		if(!strcmp(b, "&="))t="&";
		if(!strcmp(b, "|="))t="|";
		if(!strcmp(b, "^="))t="^";
		if(!strcmp(b, "<<="))t="<<";
		if(!strcmp(b, ">>="))t=">>";
		if(!strcmp(b, "<<<="))t="<<<";
		if(!strcmp(b, ">>>="))t=">>>";

		s=BGBJP_Token(s, b, &ty);

		n1=BGBJP_ExpressionEquals(ctx, &s);
		if(!n1)return(NULL);

		n=BCCX_New2("assign",
			BCCX_New1("left", n),
			BCCX_New1("right", n1));
		if(t)BCCX_Set(n, "op", t);

		continue;
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ExpressionComma(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	n=BGBJP_ExpressionEquals(ctx, &s);
	if(!n)return(NULL);

	while(1)
	{
		BGBJP_Token(s, b, &ty);
		if(ty!=BTK_SEPERATOR)break;
		if(strcmp(b, ","))break;

		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_ExpressionEquals(ctx, &s);
		if(!n1)return(NULL);

		n=BCCX_New2("comma",
			BCCX_New1("left", n),
			BCCX_New1("right", n1));
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_Expression(BGBCP_ParseState *ctx, char **str)
{
	return(BGBJP_ExpressionEquals(ctx, str));
}

BCCX_Node *BGBJP_Expression2(BGBCP_ParseState *ctx, char **str)
{
	return(BGBJP_ExpressionComma(ctx, str));
}

