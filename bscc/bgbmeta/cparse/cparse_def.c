#include <bgbccc.h>

// extern BCCX_Node *bgbcp_structs;
// extern BCCX_Node *bgbcp_types;

BCCX_Node *BGBCP_VarsList(BGBCP_ParseState *ctx, char **str, BCCX_Node *tn)
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
		BGBCP_Token(s, b, &ty);
		if(!strcmp(b, ";"))
		{
			s=BGBCP_Token(s, b, &ty);
			break;
		}

		n=BGBCP_VarDefinition(ctx, &s, tn);

		BGBCP_Token(s, b, &ty);
		if(!strcmp(b, "="))
		{
			s=BGBCP_Token(s, b, &ty);
			n1=BGBCP_Expression(ctx, &s);
			BCCX_Add(n, BCCX_New1("value", n1));

			BGBCP_Token(s, b, &ty);
		}

//		lst=BCCX_AddEnd(lst, n);
		lst=BCCX_AddEnd2(lst, &lste, n);

		s=BGBCP_Token(s, b, &ty);
		if(!strcmp(b, ";"))break;

		if(strcmp(b, ","))
		{
			BGBCP_Error(s, "PDSCR_CParse_VarsList: "
				"Invalid token '%s' in vars list\n", b);

//			printf("%p %s\n", s, s);
//			*(int *)-1=-1;

			*str=NULL;
			return(NULL);
		}
	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBCP_FunVarsList(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *lst, *lste;

	s=*str;
	lst=NULL; lste=NULL;

	while(1)
	{
		s1=BGBCP_Token(s, b, &ty);
		BGBCP_Token(s1, b2, &ty2);
		if(!strcmp(b, ")"))
		{
			s=BGBCP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, "..."))
		{
			s=BGBCP_Token(s, b, &ty);

			n=BCCX_New("rest");
//			lst=BCCX_AddEnd(lst, n);
			lst=BCCX_AddEnd2(lst, &lste, n);
			continue;
		}

		if(!strcmp(b, "void") && !strcmp(b2, ")"))
		{
			s=BGBCP_Token(s, b, &ty);
			s=BGBCP_Token(s, b, &ty);
			break;
		}


		n=BGBCP_ArgDefinition(ctx, &s);
//		lst=BCCX_AddEnd(lst, n);
		lst=BCCX_AddEnd2(lst, &lste, n);

		BGBCP_Token(s, b, &ty);

		if(!strcmp(b, ")"))
		{
			s=BGBCP_Token(s, b, &ty);
			break;
		}

		if(strcmp(b, ","))
		{
			BGBCP_Error(s, "PDSCR_CParse_FunVarsList: "
				"Invalid token '%s' "
				"in vars list\n", b);
//			*(int *)-1=-1;

			*str=NULL;
			return(NULL);
		}
		s=BGBCP_Token(s, b, &ty);
	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBCP_DefName(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	char *s;
	int i, ty, fl, ind;
	BCCX_Node *n, *n1, *nl, *anl, *nty;

	s=*str;

	BGBCP_Token(s, b, &ty);

	if(!strcmp(b, ";") || !strcmp(b, ","))
		return(NULL);

	fl=0; ind=0;

	anl=NULL;
	while(1)
	{
//		BGBCP_Token(s, b, &ty);

		if(!strcmp(b, "*"))
		{
			s=BGBCP_Token(s, b, &ty);
			BGBCP_Token(s, b, &ty);
			ind++;
			continue;
		}

		i=BGBCP_DefTypeFlag(ctx, b);
		if(i)
		{
			s=BGBCP_Token(s, b, &ty);
			BGBCP_Token(s, b, &ty);
			fl|=i;
			continue;
		}

		n1=BGBCP_DeclAttributeC(ctx, &s);
		if(n1)
		{
			BGBCP_Token(s, b, &ty);
			anl=BCCX_AddEnd(anl, n1);
			continue;
		}
		
		break;
	}

	BGBCP_Token(s, b, &ty);

	if(!strcmp(b, "("))
	{
		s=BGBCP_Token(s, b, &ty);	//(
		n1=BGBCP_DefName2(ctx, &s);
		s=BGBCP_Token(s, b, &ty);	//)

		n=BCCX_New1("fproto", n1);
	}else if(!strcmp(b, ";") || !strcmp(b, ",") || !strcmp(b, ")") ||
		!strcmp(b, "[") || !strcmp(b, ":"))
	{
		n=BCCX_New("var");
	}else if(ty==BTK_NAME)
	{
		s=BGBCP_Token(s, b, &ty);
		n=BCCX_New("var");
		BCCX_Set(n, "name", b);
	}else
	{
		BGBCP_Error(s, "BGBCP_DefName: "
			"Invalid Token '%s'\n", b);
//		*(int *)(-1)=-1;
	}

	nty=BCCX_New("type");
	BCCX_Add(n, nty);

	nl=NULL;
	while(1)
	{
		BGBCP_Token(s, b, &ty);
		if(!strcmp(b, "["))
		{
			s=BGBCP_Token(s, b, &ty);
			BGBCP_Token(s, b, &ty);
			if(!strcmp(b, "]"))
			{
				s=BGBCP_Token(s, b, &ty);
				n1=BGBCP_Number(ctx, "0");
				nl=BCCX_AddEnd(nl, n1);
				continue;
			}

			n1=BGBCP_Expression(ctx, &s);
			s=BGBCP_Token(s, b, &ty);

			nl=BCCX_AddEnd(nl, n1);
			continue;
		}

		if(!strcmp(b, ":"))
		{
			s=BGBCP_Token(s, b, &ty);	//':'
			n1=BGBCP_Expression(ctx, &s);
//			s=BGBCP_Token(s, b, &ty);

			n1=BCCX_New1("bits", n1);
			BCCX_Add(nty, n1);
			continue;
		}

		n1=BGBCP_DeclAttributeC(ctx, &s);
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

BCCX_Node *BGBCP_DefName2(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	char *s;
	int i, ty;
	BCCX_Node *n, *n1;

	n=BGBCP_DefName(ctx, str);
	if(n==NULL)return(n);

	s=*str;

	BGBCP_Token(s, b, &ty);
	if(!strcmp(b, ";") || !strcmp(b, ","))
		return(n);

	if(!strcmp(b, "("))
	{
		s=BGBCP_Token(s, b, &ty);	//(
		n1=BGBCP_FunVarsList(ctx, &s);

		BCCX_SetTag(n, "iproto");
		BCCX_Add(n, BCCX_New1("args", n1));

		while(1)
		{
			n1=BGBCP_DeclAttributeC(ctx, &s);
			if(!n1)break;
			BCCX_Add(n, n1);
		}

		*str=s;
		return(n);
	}

	*str=s;
	return(n);
}

BCCX_Node *BGBCP_VarDefinition(BGBCP_ParseState *ctx, char **str, BCCX_Node *tn)
{
	char b[256], b2[256];
	char *s, *s2;
	int ty, ty2, fl, ind;
	BCCX_Node *n, *n1, *n2, *n3, *n4;
	BCCX_Node *nl;
	int i, j;

	s=*str;

	fl=BCCX_GetInt(tn, "flags");
	ind=BCCX_GetInt(tn, "ind");

	BGBCP_Token(s, b, &ty);
	if(!strcmp(b, "__stdcall"))
		{ s=BGBCP_Token(s, b, &ty); fl|=BGBCC_TYFL_STDCALL; }

	n1=BGBCP_DefName(ctx, &s);

	if(!n1)
	{
		n=BCCX_New("var");

		n1=BCCX_Clone(tn);
		BCCX_Add(n, n1);

		BCCX_SetInt(n1, "flags", fl);
		BCCX_SetInt(n1, "ind", ind);

		*str=s;
		return(n);
	}

	BGBCP_Token(s, b, &ty);
	if(!strcmp(b, "__stdcall"))
		{ s=BGBCP_Token(s, b, &ty); fl|=BGBCC_TYFL_STDCALL; }

	BGBCP_Token(s, b, &ty);

	if(!strcmp(b, "(") && (ty==BTK_BRACE))
	{
		s=BGBCP_Token(s, b, &ty);	//(
		n2=BGBCP_FunVarsList(ctx, &s);

		if(BCCX_TagIsP(n1, "fproto"))
		{
//			s2=BGBCC_GenSym2();

//			n3=BCCX_FindTag(n1, "type");
			s2=BGBCC_GenProtoSig(NULL, tn, n2);
			i=0; while(*s2)i=(i*127)+(*s2++);
			sprintf(b2, "HSG%08X", i);
			s2=bgbcc_strdup(b2);

			n=BCCX_New("func");
			BCCX_Set(n, "name", s2);
			BCCX_SetInt(n, "flags", fl|BCCX_GetInt(n, "flags"));
			BCCX_SetInt(n, "ind", ind+BCCX_GetInt(n, "ind"));

			BCCX_Add(n, BCCX_Clone(tn));
			BCCX_Add(n, BCCX_New1("args", n2));
//			ctx->structs=BCCX_AddEnd(ctx->structs, n);
			ctx->structs=BCCX_AddEnd2(ctx->structs, &ctx->e_structs, n);


			n=BCCX_Child(n1);

			n1=BCCX_FindTag(n, "type");
			if(!n1)
			{
				n1=BCCX_New("type");
				BCCX_Add(n, n1);
			}
			BCCX_Set(n1, "name", s2);

			while(1)
			{
				n2=BGBCP_DeclAttributeC(ctx, &s);
				if(!n2)break;
				BCCX_Add(n1, n2);
			}

			*str=s;
			return(n);
		}

		n=n1;
		BCCX_SetTag(n, "proto");

//		n2=BCCX_Clone(n2);

		BCCX_Add(n, BCCX_New1("args", n2));

		n2=BCCX_FindTag(n, "type");
		BCCX_Set(n2, "name", BCCX_Get(tn, "name"));
		BCCX_SetInt(n2, "flags", fl|BCCX_GetInt(n2, "flags"));
		BCCX_SetInt(n2, "ind", ind+BCCX_GetInt(n2, "ind"));

		while(1)
		{
			n1=BGBCP_DeclAttributeC(ctx, &s);
			if(!n1)break;
			BCCX_Add(n2, n1);
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
	BCCX_SetInt(n4, "ind", ind+BCCX_GetInt(n4, "ind"));

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

BCCX_Node *BGBCP_ArgDefinition(BGBCP_ParseState *ctx, char **str)
{
	char *s, *s2;
	BCCX_Node *n;

	s=*str;
	n=BGBCP_DefType(ctx, &s);
	if(!n)return(NULL);

	n=BGBCP_VarDefinition(ctx, &s, n);

	*str=s;
	return(n);
}

BCCX_Node *BGBCP_Definition(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	char *s, *s2;
	BCCX_Node *n, *n1, *n2;
	int i, ty;

	if(	(ctx->lang==BGBCC_LANG_JAVA) ||
		(ctx->lang==BGBCC_LANG_CS) ||
		(ctx->lang==BGBCC_LANG_BS) ||
		(ctx->lang==BGBCC_LANG_MINIC) ||
		(ctx->lang==BGBCC_LANG_BS2))
			return(BGBJP_Definition(ctx, str));

	s=*str;

#if 1
	s=BGBCP_EatWhiteOnly(s);
	if(!strncmp(s, "/*AHPRX:*/", 10))
	{
		s+=10;
		n=BGBCP_Definition(ctx, &s);
		if(n)
		{
			n1=BCCX_FindTag(n, "type");
			if(n1)
			{
				i=BCCX_GetInt(n1, "flags");
				i|=BGBCC_TYFL_VIRTUAL|BGBCC_TYFL_PROXY;
				BCCX_SetInt(n1, "flags", i);
			}
			*str=s;
			return(n);
		}
		return(NULL);
	}
	
	n1=BGBCP_ParseDocComment(ctx, &s);
	n2=BGBCP_ParseComment(ctx, &s);
	if(n1 && !n2)
	{
		n=BGBCP_Definition(ctx, &s);
		if(n)
		{
			BCCX_Add(n, n1);
			*str=s;
			return(n);
		}
		return(NULL);
	}
#endif

//	printf("\n");
//	printf("DEF %p %s\n", s, s);

	n=BGBCP_DefType(ctx, &s);
	if(n)
	{
//		BGBCP_Warn(s, "BGBCP_Definition: Note: definition\n");

#if 0
		if(BCCX_TagIsP(n, "classdef") ||
			BCCX_TagIsP(n, "enumdef"))
		{
			*str=s2;
			return(n);
		}
#endif


		s2=s;

		n1=BGBCP_VarDefinition(ctx, &s2, n);
		if(BCCX_TagIsP(n1, "proto"))
		{
			BGBCP_Token(s2, b, &ty);
			if(!strcmp(b, "{"))
			{
				n=BCCX_Clone(n1);
				n1=BGBCP_BlockStatement2(ctx, &s2);

				BCCX_SetTag(n, "defun");
				BCCX_Add(n, BCCX_New1("body", n1));

				*str=s2;
				return(n);
			}
		}

		n1=BGBCP_VarsList(ctx, &s, n);
//		n1=BCCX_Clone(n1);
		n=BCCX_New1("vars", n1);

		*str=s;
		return(n);
	}

	return(NULL);
}

