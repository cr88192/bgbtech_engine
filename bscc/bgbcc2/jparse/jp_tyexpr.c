#include <bgbccc.h>

BCCX_Node *BGBJP_TypeArgExpression(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	BCCX_Node *n, *n1, *lst;
	int i;

	s=*str;

	BGBJP_Token(s, b, &ty);

	if((ty==BTK_OPERATOR) && !strcmp(b, "?"))
	{
		s=BGBJP_Token(s, b, &ty);
		n=BCCX_New("anytype");
		
//		*str=s;
//		return(n);
	}else
	{
		n=BGBJP_ExpressionAS(ctx, &s);
		if(!n)return(NULL);
	}

	while(1)
	{
		BGBJP_Token(s, b, &ty);

		if(	!strcmp(b, "extends") ||
			!strcmp(b, "implements") ||
			!strcmp(b, "super"))
		{
			s=BGBJP_Token(s, b, &ty);
			n1=BGBJP_DefType(ctx, &s);
			n=BCCX_New2(b,
				BCCX_New1("value", n), n1);
			continue;
		}

#if 0
		if(!strcmp(b, "extends"))
		{
			s=BGBJP_Token(s, b, &ty);
			n1=BGBJP_TypeExpression(ctx, &s);
			n=BCCX_New2("extends",
				BCCX_New1("value", n),
				BCCX_New1("tyexpr", n1));
			continue;
		}

		if(!strcmp(b, "implements"))
		{
			s=BGBJP_Token(s, b, &ty);
			n1=BGBJP_TypeExpression(ctx, &s);
			n=BCCX_New2("extends",
				BCCX_New1("value", n),
				BCCX_New1("tyexpr", n1));
			continue;
		}

		if(!strcmp(b, "super"))
		{
			s=BGBJP_Token(s, b, &ty);
			n1=BGBJP_TypeExpression(ctx, &s);
			n=BCCX_New2("super",
				BCCX_New1("value", n),
				BCCX_New1("tyexpr", n1));
			continue;
		}
#endif

		if(!strcmp(b, "[") && (ty==BTK_BRACE))
		{
			s=BGBJP_Token(s, b, &ty); //[
			BGBJP_Token(s, b2, &ty2); //]
			i=1;
			while(!strcmp(b2, ","))
				{ i++; s=BGBJP_Token(s, b2, &ty2); }
			if(!strcmp(b2, "]"))
			{
				s=BGBJP_Token(s, b2, &ty2); //]
				n=BCCX_New1("emptyindex", BCCX_New1("array", n));
				if(i>1)BCCX_SetInt(n, "ord", i);
				continue;
			}
			
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

#if 0
		if(!strcmp(b, "(") && (ty==BTK_BRACE))
		{
			s=BGBJP_Token(s, b, &ty); //'('
			n1=BGBJP_FunArgs(ctx, &s);
			if(!s)return(NULL);
			
			n=BCCX_New2("funcall",
				BCCX_New1("value", n),
				BCCX_New1("args", n1));
			continue;
		}
#endif

		if(!strcmp(b, "<") && (ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty); //'<'
			n1=BGBJP_TypeArgs(ctx, &s);
			if(!s)return(NULL);

			n=BCCX_New2("sharpcall",
				BCCX_New1("value", n),
				BCCX_New1("args", n1));
			continue;
		}

		if((!strcmp(b, "^") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty);
			n=BCCX_New1("unary", BCCX_New1("value", n));
			BCCX_Set(n, "op", b);
			continue;
		}

		if(!strcmp(b, "...") && (ty==BTK_MISC))
		{
			s=BGBJP_Token(s, b, &ty);
			n=BCCX_New1("unary", BCCX_New1("value", n));
			BCCX_Set(n, "op", b);
			continue;
		}

		break;
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_TypeArgs(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	BCCX_Node *n, *n1, *lst;

	s=*str;
	lst=NULL;

	while(1)
	{
		b[0]=0;
		BGBJP_Token(s, b, &ty);
		if(!*b)break;
		if((ty==BTK_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		if((ty==BTK_OPERATOR) && (b[0]=='>'))
		{
			s=BGBJP_EatWhite(s);
			s++;	//'>'
			break;
		}

#if 0
		if((ty==BTK_OPERATOR) && !strcmp(b, "?"))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}
#endif

		n=BGBJP_TypeArgExpression(ctx, &s);
//		n=BGBJP_ExpressionAS(ctx, &s);
		if(!n)
		{
			*str=NULL;
			return(NULL);
		}

		lst=BCCX_AddEnd(lst, n);

		BGBJP_Token(s, b, &ty);
		if(!strcmp(b, ","))
		{
			s=BGBJP_Token(s, b, &ty);
			continue;
		}

		if((ty==BTK_OPERATOR) && (b[0]=='>'))
		{
			s=BGBJP_EatWhite(s);
			s++;	//'>'
			break;
		}

		*str=NULL;
		return(NULL);
	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBJP_TypeExpressionLit(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2;
	int i, j;

	s=*str;

	BGBJP_Token(s, b, &ty);

	if((ty==BTK_SEPERATOR) && !strcmp(b, ";"))
		return(NULL);
	if((ty==BTK_SEPERATOR) && !strcmp(b, ","))
		return(NULL);

	s=BGBJP_Token(s, b, &ty);
	if(!s)return(NULL);

	if(ty==BTK_NAME)
	{
		i=BGBCC_GetOrdinalLang(b, ctx->lang);
		if(i>=0)
		{
			//reject non-type keywords
			if(!BGBCC_CheckOrdinalFlag(i, 't'))
				return(NULL);
		}
	
		n=BCCX_New("ref");
		BCCX_Set(n, "name", b);
		*str=s;
		return(n);
	}

	return(NULL);
}

BCCX_Node *BGBJP_TypeExpressionPE(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;
	int i;

	s=*str;
	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BTK_BRACE))
	{
//		n=BGBJP_FunArgs(ctx, &s);
		n=BGBJP_TypeArgs(ctx, &s);
		if(!n) return(NULL);

		n=BCCX_New1("begin", n);

		*str=s;
		return(n);
	}

	n=BGBJP_TypeExpressionLit(ctx, &s);
	if(!n) return(NULL);

	while(1)
	{
		if(!BGBJP_IsWhiteOnly(s))break;
		BGBJP_Token(s, b, &ty);

		if(!strcmp(b, "[") && (ty==BTK_BRACE))
		{
			s=BGBJP_Token(s, b, &ty); //[
			BGBJP_Token(s, b2, &ty2); //]
			if(!strcmp(b2, "]"))
			{
				s=BGBJP_Token(s, b2, &ty2); //]
				n=BCCX_New1("emptyindex", BCCX_New1("array", n));
				continue;
			}
			
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

#if 1
#if 0
		if(!strcmp(b, "(") && (ty==BTK_BRACE))
		{
			s=BGBJP_Token(s, b, &ty); //'('
			n1=BGBJP_FunArgs(ctx, &s);
			if(!s)return(NULL);
			
			n=BCCX_New2("funcall",
				BCCX_New1("value", n),
				BCCX_New1("args", n1));
			continue;
		}
#endif

		if(!strcmp(b, "<") && (ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty); //'<'
			n1=BGBJP_TypeArgs(ctx, &s);
			if(!s)return(NULL);

			n=BCCX_New2("sharpcall",
				BCCX_New1("value", n),
				BCCX_New1("args", n1));
			continue;
		}

		if((!strcmp(b, "^") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BTK_OPERATOR))
		{
			s=BGBJP_Token(s, b, &ty);
			n=BCCX_New1("unary", BCCX_New1("value", n));
			BCCX_Set(n, "op", b);
			continue;
		}

		if(!strcmp(b, "...") && (ty==BTK_MISC))
		{
			s=BGBJP_Token(s, b, &ty);
			n=BCCX_New1("unary", BCCX_New1("value", n));
			BCCX_Set(n, "op", b);
			continue;
		}
#endif

		break;
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_TypeExpressionUnary(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	BGBJP_Token(s, b, &ty);

	if((!strcmp(b, "^") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BTK_OPERATOR))
	{
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_TypeExpressionUnary(ctx, &s);
		if(!n1)return(NULL);
		n=BCCX_New1("unary", BCCX_New1("value", n1));
		BCCX_Set(n, "op", b);

		*str=s;
		return(n);
	}

	s=*str;
	n=BGBJP_TypeExpressionPE(ctx, &s);
	if(!n) return(NULL);

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_TypeExpressionBinary(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;

	s=*str;
	
	n=BGBJP_TypeExpressionUnary(ctx, &s);
	if(!n)return(NULL);
	
	BGBJP_Token(s, b, &ty);
	if((ty==BTK_OPERATOR) && (b[0]!='>'))
		return(NULL);

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_TypeExpression(BGBCP_ParseState *ctx, char **str)
{
	return(BGBJP_TypeExpressionBinary(ctx, str));
}

BCCX_Node *BGBJP_CastTypeExpression(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3;
	int i;

	s=*str;
	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BTK_BRACE))
	{
		s=BGBJP_Token(s, b, &ty);	//(
		n=BGBJP_TypeExpression(ctx, &s);
		if(!n)return(NULL);
		s=BGBJP_Token(s, b, &ty);	//)
		if(strcmp(b, ")") || (ty!=BTK_BRACE))
			return(NULL);

//		n=BCCX_New1("casttype", n);
		n=BCCX_New1("type", BCCX_New1("expr", n));

		*str=s;
		return(n);
	}

	return(NULL);
}
