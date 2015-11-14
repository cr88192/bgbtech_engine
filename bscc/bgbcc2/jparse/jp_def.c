#include <bgbccc.h>

BCCX_Node *BGBJP_VarsList(BGBCP_ParseState *ctx, char **str, BCCX_Node *tn)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	BCCX_Node *n, *n1, *lst, *lste;

	s=*str;
	lst=NULL; lste=NULL;

//	printf("VL %p %s\n", s, s);

	while(1)
	{
		BGBJP_Token(s, b, &ty);
		if(!strcmp(b, ";"))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		n=BGBJP_VarDefinition(ctx, &s, tn, 0);

		BGBJP_Token(s, b, &ty);
		if(!strcmp(b, "="))
		{
			s=BGBJP_Token(s, b, &ty);
			n1=BGBJP_Expression(ctx, &s);
			BCCX_Add(n, BCCX_New1("value", n1));

			BGBJP_Token(s, b, &ty);
		}

//		lst=BCCX_AddEnd(lst, n);
		lst=BCCX_AddEnd2(lst, &lste, n);

		s=BGBJP_Token(s, b, &ty);
		if(!strcmp(b, ";"))break;

		if(strcmp(b, ","))
		{
//			BGBJP_Error(s, "PDSCR_CParse_VarsList: "
//				"Invalid token '%s' in vars list\n", b);

			*str=NULL;
			return(NULL);
		}
	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBJP_FunVarsList(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *lst, *lste;

	s=*str;
	lst=NULL; lste=NULL;

	while(1)
	{
		s1=BGBJP_Token(s, b, &ty);
		BGBJP_Token(s1, b2, &ty2);
		if(!strcmp(b, ")"))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, "..."))
		{
			s=BGBJP_Token(s, b, &ty);
			BGBJP_Token(s, b2, &ty2);

			n=BCCX_New("rest");
			if(ty2==BTK_NAME)
			{
				s=BGBJP_Token(s, b2, &ty2);
				BCCX_Set(n, "name", b2);
			}

//			lst=BCCX_AddEnd(lst, n);
			lst=BCCX_AddEnd2(lst, &lste, n);
			continue;
		}

		if(!strcmp(b, "void") && !strcmp(b2, ")"))
		{
			s=BGBJP_Token(s, b, &ty);
			s=BGBJP_Token(s, b, &ty);
			break;
		}


		n=BGBJP_ArgDefinition(ctx, &s);
//		lst=BCCX_AddEnd(lst, n);
		lst=BCCX_AddEnd2(lst, &lste, n);

		BGBJP_Token(s, b, &ty);

		if(!strcmp(b, ")"))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		if(strcmp(b, ","))
		{
			*str=NULL;
			return(NULL);
		}
		s=BGBJP_Token(s, b, &ty);
	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBJP_DefName(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	char *s;
	int i, ty, fl, ind;
	BCCX_Node *n, *n1, *nl, *anl, *nty;

	s=*str;

	BGBJP_Token(s, b, &ty);

	if(!strcmp(b, ";") || !strcmp(b, ","))
		return(NULL);

	fl=0; ind=0;

	anl=NULL;
	while(1)
	{
		if(!strcmp(b, "*"))
		{
			s=BGBJP_Token(s, b, &ty);
			BGBJP_Token(s, b, &ty);
			ind++;
			continue;
		}

		i=BGBJP_DefTypeFlag(ctx, b);
		if(i)
		{
			s=BGBJP_Token(s, b, &ty);
			BGBJP_Token(s, b, &ty);
			fl|=i;
			continue;
		}

		n1=BGBJP_DeclAttribute(ctx, &s);
		if(n1)
		{
			anl=BCCX_AddEnd(anl, n1);
			continue;
		}
		
		break;
	}

	if(!strcmp(b, "("))
	{
		s=BGBJP_Token(s, b, &ty);	//(
		n1=BGBJP_DefName2(ctx, &s);
		s=BGBJP_Token(s, b, &ty);	//)

		n=BCCX_New1("fproto", n1);
	}else if(!strcmp(b, ";") || !strcmp(b, ",") || !strcmp(b, ")") ||
		!strcmp(b, "[") || !strcmp(b, ":"))
	{
		n=BCCX_New("var");
	}else if(ty==BTK_NAME)
	{
		s=BGBJP_Token(s, b, &ty);
		n=BCCX_New("var");
		BCCX_Set(n, "name", b);
	}else
	{
//		BGBJP_Error(s, "BGBJP_DefName: "
//			"Invalid Token '%s'\n", b);
		return(NULL);
	}

	nty=BCCX_New("type");
	BCCX_Add(n, nty);

	nl=NULL;
	while(1)
	{
		BGBJP_Token(s, b, &ty);
		if(!strcmp(b, "["))
		{
			s=BGBJP_Token(s, b, &ty);
			BGBJP_Token(s, b, &ty);
			if(!strcmp(b, "]"))
			{
				s=BGBJP_Token(s, b, &ty);
				n1=BGBJP_Number(ctx, "0");
				nl=BCCX_AddEnd(nl, n1);
				continue;
			}

			n1=BGBJP_Expression(ctx, &s);
			s=BGBJP_Token(s, b, &ty);

			nl=BCCX_AddEnd(nl, n1);
			continue;
		}

		if(!strcmp(b, ":"))
		{
			s=BGBJP_Token(s, b, &ty);	//':'
			n1=BGBJP_Expression(ctx, &s);
//			s=BGBJP_Token(s, b, &ty);

			n1=BCCX_New1("bits", n1);
			BCCX_Add(nty, n1);
			continue;
		}

#if 0
		if(!strcmp(b, "..."))
		{
			s=BGBJP_Token(s, b, &ty);	//'...'
			n1=BCCX_New1("bits", n1);
			BCCX_Add(nty, n1);
			continue;
		}
#endif

		n1=BGBJP_DeclAttribute(ctx, &s);
		if(n1)
		{
			anl=BCCX_AddEnd(anl, n1);
			continue;
		}

		break;
	}

	if(nl)BCCX_Add(nty, BCCX_New1("size", nl));
	if(anl)BCCX_Add(nty, anl);
	BCCX_SetInt(nty, "flags", fl);
	BCCX_SetInt(nty, "ind", ind);

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_DefName2(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	char *s;
	int i, ty;
	BCCX_Node *n, *n1;

	n=BGBJP_DefName(ctx, str);
	if(!n)return(NULL);

	s=*str;

	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, ";") || !strcmp(b, ","))
		return(n);

	if(!strcmp(b, "("))
	{
		s=BGBJP_Token(s, b, &ty);	//(
		n1=BGBJP_FunVarsList(ctx, &s);
		if(!s)return(NULL);

		BCCX_SetTag(n, "iproto");
		BCCX_Add(n, BCCX_New1("args", n1));

		*str=s;
		return(n);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_VarDefinition(BGBCP_ParseState *ctx, char **str, BCCX_Node *tn, int mfl)
{
	char b[256], b2[256];
	char *s, *s2;
	int ty, ty2, fl, ind;
	BCCX_Node *n, *n1, *n2, *n3, *n4;
	BCCX_Node *nl;
	int i, j;

	s=*str;

	fl=BCCX_GetInt(tn, "flags");
//	ind=BCCX_GetInt(tn, "ind");

	BGBJP_Token(s, b, &ty);

	if((ctx->lang==BGBCC_LANG_BS2) &&
		(!strcmp(b, "get") || !strcmp(b, "set")) &&
		(ty==BTK_NAME))
	{
		s=BGBJP_Token(s, b, &ty);
		n1=BGBJP_DefName(ctx, &s);
		if(!n1)return(NULL);
		
		if(!strcmp(b, "get"))
			fl|=BGBCC_TYFL_GETTER;
		if(!strcmp(b, "set"))
			fl|=BGBCC_TYFL_SETTER;

//		if(!strcmp(b, "get"))
//			BCCX_SetTag(n1, "getter");
//		if(!strcmp(b, "set"))
//			BCCX_SetTag(n1, "setter");
	}else
	{
		n1=BGBJP_DefName(ctx, &s);
	}

	if(!n1)
	{
		if(mfl&1)return(NULL);
	
		n=BCCX_New("var");

		n1=BCCX_Clone(tn);
		BCCX_Add(n, n1);

		BCCX_SetInt(n1, "flags", fl);
		//BCCX_SetInt(n1, "ind", ind);

		*str=s;
		return(n);
	}

	BGBJP_Token(s, b, &ty);

	if(!strcmp(b, "(") && (ty==BTK_BRACE))
	{
		s=BGBJP_Token(s, b, &ty);	//(
		n2=BGBJP_FunVarsList(ctx, &s);
		if(!s)return(NULL);

		n=n1;
		BCCX_SetTag(n, "proto");

//		n2=BCCX_Clone(n2);

		BCCX_Add(n, BCCX_New1("args", n2));

		n2=BCCX_FindTag(n, "type");
		BCCX_Set(n2, "name", BCCX_Get(tn, "name"));
		BCCX_SetInt(n2, "flags", fl|BCCX_GetInt(n2, "flags"));
//		BCCX_SetInt(n2, "ind", ind+BCCX_GetInt(n2, "ind"));

		n3=BCCX_FindTag(tn, "expr");
		if(n3)BCCX_Add(n2, BCCX_Clone(n3));

		while(1)
		{
			n1=BGBJP_DeclAttribute(ctx, &s);
			if(n1)
			{
				BCCX_Add(n2, n1);
				continue;
			}
			break;
		}

#if 1
		n1=BCCX_Child(tn);
		while(n1)
		{
			if(BCCX_TagIsP(n1, "declspec") ||
				BCCX_TagIsP(n1, "attribute"))
					BCCX_Add(n2, BCCX_Clone(n1));
			n1=BCCX_Next(n1);
		}
#endif

		*str=s;
		return(n);
	}

	n=n1;
//	BCCX_SetInt(n, "flags", fl|BCCX_GetInt(n, "flags"));
//	BCCX_SetInt(n, "ind", ind+BCCX_GetInt(n, "ind"));
//	BCCX_Add(n, BCCX_Clone(tn));

	n4=BCCX_FindTag(n, "type");
	BCCX_Set(n4, "name", BCCX_Get(tn, "name"));
	BCCX_SetInt(n4, "flags", fl|BCCX_GetInt(n4, "flags"));
//	BCCX_SetInt(n4, "ind", ind+BCCX_GetInt(n4, "ind"));

	n3=BCCX_FindTag(tn, "expr");
	if(n3)BCCX_Add(n4, BCCX_Clone(n3));

	n1=BCCX_FindTag(n4, "size");
	n2=BCCX_FindTag(tn, "size");
	if(n1 && n2)
	{
		nl=NULL;

		n2=BCCX_Child(n2);
		while(n2)
		{
			nl=BCCX_AddEnd(nl, BCCX_Clone(n2));
			n2=BCCX_Next(n2);
		}

		n2=BCCX_Child(n1);
		while(n2)
		{
			nl=BCCX_AddEnd(nl, BCCX_Clone(n2));
			n2=BCCX_Next(n2);
		}

		BCCX_Unlink(n1);
		BCCX_Add(n4, BCCX_New1("size", nl));
	}else if(n2)
	{
		BCCX_Add(n4, BCCX_Clone(n2));
	}

#if 1
	n1=BCCX_Child(tn);
	while(n1)
	{
		if(BCCX_TagIsP(n1, "declspec") ||
			BCCX_TagIsP(n1, "attribute"))
				BCCX_Add(n4, BCCX_Clone(n1));
		n1=BCCX_Next(n1);
	}
#endif

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_ArgDefinition(BGBCP_ParseState *ctx, char **str)
{
	char *s, *s2;
	BCCX_Node *n;

	s=*str;
	n=BGBJP_DefType(ctx, &s);
	if(!n)return(NULL);

	n=BGBJP_VarDefinition(ctx, &s, n, 0);

	*str=s;
	return(n);
}

BCCX_Node *BGBJP_Definition(BGBCP_ParseState *ctx, char **str)
{
	char b[64];
	char *s, *s2;
	BCCX_Node *n, *n1;
	int ty;

	if(	(ctx->lang==BGBCC_LANG_C) ||
		(ctx->lang==BGBCC_LANG_CPP) ||
		(ctx->lang==BGBCC_LANG_CX))
			return(BGBCP_Definition(ctx, str));

	s=*str;

	n=BGBJP_DefType(ctx, &s);
	if(n)
	{
		s2=s;

		n1=BGBJP_VarDefinition(ctx, &s2, n, 1);
		if(!n1)return(NULL);
		
		if(BCCX_TagIsP(n1, "proto"))
		{
			BGBJP_Token(s2, b, &ty);
			if(!strcmp(b, "{"))
			{
				n=BCCX_Clone(n1);
				n1=BGBJP_BlockStatement2(ctx, &s2);

				BCCX_SetTag(n, "defun");
				BCCX_Add(n, BCCX_New1("body", n1));

				*str=s2;
				return(n);
			}
		}

		if(BCCX_TagIsP(n1, "var"))
		{
			BGBJP_Token(s2, b, &ty);
			if(!strcmp(b, "{"))
			{
				s2=BGBJP_Token(s2, b, &ty);	//'{'

				n=BCCX_Clone(n1);

//				n1=BGBJP_BlockStatement2(ctx, &s2);
				n1=BGBJP_Block(ctx, &s2);

				BCCX_SetTag(n, "property");
				BCCX_Add(n, n1);

				*str=s2;
				return(n);
			}
		}

		n1=BGBJP_VarsList(ctx, &s, n);
		if(!s)return(NULL);
		n=BCCX_New1("vars", n1);

		*str=s;
		return(n);
	}

	return(NULL);
}

