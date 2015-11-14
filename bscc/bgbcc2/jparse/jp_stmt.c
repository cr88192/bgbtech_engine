#include <bgbccc.h>

#ifdef WIN32
#include <windows.h>
//#define CATCH_BLOCK_SEH
#endif

BGBCP_ParseItem *bgbjp_stmts=NULL;
BGBCP_ParseItem *bgbjp_blkstmts=NULL;

char *bgbjp_curfilename=NULL;

int bgbjp_warn, bgbjp_err;

void BGBJP_Warn(char *se, char *str, ...)
{
	va_list lst;
	char *fn;
	int ln;

	va_start(lst, str);

	bgbjp_warn++;
	fn=BGBJP_GetLastFileName();
	ln=BGBJP_GetLastLineNumber();

	printf("%s:%d: warning: ", fn, ln);
	vprintf(str, lst);

	va_end(lst);
}

void BGBJP_Error(char *se, char *str, ...)
{
	va_list lst;
	char *fn;
	int ln;

	va_start(lst, str);

	bgbjp_err++;
	fn=BGBJP_GetLastFileName();
	ln=BGBJP_GetLastLineNumber();

	printf("%s:%d: error: ", fn, ln);
	vprintf(str, lst);

	va_end(lst);
	fflush(stdout);

//	*(int *)-1=-1;
	BGBCC_DieError();
}


int BGBJP_AddStatement(char *name,
	BCCX_Node *(*func)(BGBCP_ParseState *ctx, char **s))
{
	BGBCP_ParseItem *tmp;

	tmp=malloc(sizeof(BGBCP_ParseItem));
	tmp->name=strdup(name);
	tmp->func=func;

	tmp->next=bgbjp_stmts;
	bgbjp_stmts=tmp;

	return(0);
}

BCCX_Node *BGBJP_ParseStatementName(
	BGBCP_ParseState *ctx, char *name, char **s)
{
	BGBCP_ParseItem *cur;

	cur=bgbjp_stmts;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur->func(ctx, s));
		cur=cur->next;
	}
	return(NULL);
}

int BGBJP_AddBlockStatement(char *name,
	BCCX_Node *(*func)(BGBCP_ParseState *ctx, char **s))
{
	BGBCP_ParseItem *tmp;

	tmp=malloc(sizeof(BGBCP_ParseItem));
	tmp->name=strdup(name);
	tmp->func=func;

	tmp->next=bgbjp_blkstmts;
	bgbjp_blkstmts=tmp;

	return(0);
}

BCCX_Node *BGBJP_ParseBlockStatementName(
	BGBCP_ParseState *ctx, char *name, char **s)
{
	BGBCP_ParseItem *cur;

	cur=bgbjp_blkstmts;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur->func(ctx, s));
		cur=cur->next;
	}
	return(NULL);
}

char *BGBJP_EatSemicolon(char *s)
{
	s=BGBJP_EatWhite(s);
	if(*s==';')s++;
	s=BGBJP_EatWhite(s);
	return(s);
}

BCCX_Node *BGBJP_Statement(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2;

	s=*str;
	s=BGBJP_Token(s, b, &ty);
	BGBJP_Token(s, b2, &ty2);
	if(!*s)
	{
		*str=s;
		return(NULL);
	}

	if(ty!=BTK_NAME)
	{
		s=*str;
		n=BGBJP_Expression(ctx, &s);
		*str=s;
		return(n);
	}

	n=BGBJP_ParseStatementName(ctx, b, &s);
	if(n!=NULL)
	{
		*str=s;
		return(n);
	}

	if(!strcmp(b, "break"))
	{
		n=BCCX_New("break");
		*str=s;
		return(n);
	}

	if(!strcmp(b, "continue"))
	{
		n=BCCX_New("continue");
		*str=s;
		return(n);
	}

	if(!strcmp(b, "goto"))
	{
		s=BGBJP_Token(s, b2, &ty2);
//		n=BCCX_New1("goto", BCCX_NewText(b2));
		n=BCCX_New("goto");
		BCCX_Set(n, "name", b2);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "return"))
	{
		BGBJP_Token(s, b2, &ty2);
		if(b2[0]!=';')
		{
			n1=BGBJP_Expression(ctx, &s);
			n=BCCX_New1("return", BCCX_New1("value", n1));
		}else n=BCCX_New("return");

		*str=s;
		return(n);
	}

	if(!strcmp(b, "throw"))
	{
		BGBJP_Token(s, b2, &ty2);
		if(b2[0]!=';')
		{
			n1=BGBJP_Expression(ctx, &s);
			n=BCCX_New1("throw", BCCX_New1("value", n1));
		}else n=BCCX_New("throw");

		*str=s;
		return(n);
	}


	s=*str;
	s=BGBJP_EatWhite(s);
	if(!s || !*s)return(NULL);

	n=BGBJP_Expression(ctx, &s);
	if(n)
	{
		*str=s;
		return(n);
	}

//	BGBJP_Error(s, "PDSCR_CParse_Statement: Unrecognizable "
//		"statement type\n");
//	*str=NULL;
	return(NULL);
}

BCCX_Node *BGBJP_BlockStatementInner(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *ab[16];
	char **a;
	char *s, *s0, *s1, *s2;
	int ty, ty2;
	BCCX_Node *n, *n1, *n2, *n3, *n4;
	int i;

	s=*str;

	s=BGBJP_Token(s, b, &ty);
	BGBJP_Token(s, b2, &ty2);
	if(!*s)
	{
		n=BCCX_New("null_statement");
		*str=s;
		return(n);
	}

	if(ty==BTK_SEPERATOR)
	{
		n=BCCX_New("null_statement");
		*str=s;
		return(n);
	}

	if((ty==BTK_NAME) && !strcmp(b2, ":"))
	{
		s=BGBJP_Token(s, b2, &ty2);	//:

		if(!strcmp(b, "default"))
		{
			n=BCCX_New("case_default");
			*str=s;
			return(n);
		}

		n=BCCX_New("label");
		BCCX_Set(n, "name", b);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "typedef") && (ctx->lang==BGBCC_LANG_C))
	{
		n=BGBJP_Definition(ctx, &s);
		BGBJP_HandleTypedef(ctx, n);

		*str=s;
		return(NULL);
	}

	if(!strcmp(b, "case"))
	{
		n1=BGBJP_Expression(ctx, &s);
		s=BGBJP_Token(s, b2, &ty2);	//:
		n=BCCX_New1("case", BCCX_New1("value", n1));

		*str=s;
		return(n);
	}

	if(!strcmp(b, "if"))
	{
		s=BGBJP_Token(s, b, &ty);	//(
		n1=BGBJP_Expression(ctx, &s);
		s=BGBJP_Token(s, b, &ty);	//)
		n2=BGBJP_BlockStatement2(ctx, &s);

		BGBJP_Token(s, b, &ty);
		if(strcmp(b, "else"))
		{
			n=BCCX_New2("if",
				BCCX_New1("cond", n1),
				BCCX_New1("then", n2));
			*str=s;
			return(n);
		}

		s=BGBJP_Token(s, b, &ty);	//else
		n3=BGBJP_BlockStatement2(ctx, &s);
		n=BCCX_New3("if",
			BCCX_New1("cond", n1),
			BCCX_New1("then", n2),
			BCCX_New1("else", n3));

		*str=s;
		return(n);
	}

	if(!strcmp(b, "while"))
	{
		s=BGBJP_Token(s, b, &ty);	//'('
		n1=BGBJP_Expression(ctx, &s);
		s=BGBJP_Token(s, b, &ty);	//')'

		n2=BGBJP_BlockStatement2(ctx, &s);
		n=BCCX_New2("while",
			BCCX_New1("cond", n1),
			BCCX_New1("body", n2));

		*str=s;
		return(n);
	}

	if(!strcmp(b, "for"))
	{
		s=BGBJP_Token(s, b, &ty);		//(
		s2=BGBJP_Token(s, b, &ty);		//
		s2=BGBJP_Token(s2, b2, &ty);	//

		n1=BGBJP_Definition(ctx, &s);
		if(!n1)n1=BGBJP_Expression2(ctx, &s);

//		n1=BGBJP_Expression2(ctx, &s);
		s=BGBJP_EatSemicolon(s);
		n2=BGBJP_Expression2(ctx, &s);
		s=BGBJP_EatSemicolon(s);
		n3=BGBJP_Expression2(ctx, &s);

		s=BGBJP_Token(s, b, &ty);	//)
		n4=BGBJP_BlockStatement2(ctx, &s);

		n=BCCX_New4("for",
			BCCX_New1("init", n1),
			BCCX_New1("cond", n2),
			BCCX_New1("step", n3),
			BCCX_New1("body", n4));

		*str=s;
		return(n);
	}

	if(!strcmp(b, "do"))
	{
		n2=BGBJP_BlockStatement2(ctx, &s);

		BGBJP_Token(s, b, &ty);	//'while'
		if(!strcmp(b, "while"))
		{
			s=BGBJP_Token(s, b, &ty);	//'while'

			s=BGBJP_Token(s, b, &ty);	//'('
			n1=BGBJP_Expression(ctx, &s);
			s=BGBJP_Token(s, b, &ty);	//')'

			s=BGBJP_EatSemicolon(s);

			n=BCCX_New2("do_while",
				BCCX_New1("cond", n1),
				BCCX_New1("body", n2));
		}else
		{
			n=n2;
		}

		*str=s;
		return(n);
	}

	if(!strcmp(b, "switch"))
	{
		s=BGBJP_Token(s, b, &ty);	//(
		n1=BGBJP_Expression(ctx, &s);
		s=BGBJP_Token(s, b, &ty);	//)

		s=BGBJP_Token(s, b, &ty); //{
		n2=BGBJP_Block(ctx, &s);

		n=BCCX_New2("switch",
			BCCX_New1("cond", n1),
			BCCX_New1("body", n2));

		*str=s;
		return(n);
	}

	if(!strcmp(b, "__asm__"))
	{
		i=0;
		while(1)
		{
			s=BGBJP_Token(s, b, &ty);
			if(!strcmp(b, "("))i++;
			if(!strcmp(b, ")"))
			{
				i--;
				if(!i)break;
			}
		}

		n=BCCX_New("gcc_asm");

		*str=s;
		return(n);
	}

	if(!strcmp(b, "__asm"))
	{
		i=0;
		while(1)
		{
			s=BGBJP_Token(s, b, &ty);
			if(!strcmp(b, "{"))i++;
			if(!strcmp(b, "}"))
			{
				i--;
				if(!i)break;
			}
		}

		n=BCCX_New("msvc_asm");

		*str=s;
		return(n);
	}

	if((ctx->lang==BGBCC_LANG_CPP) ||
		(ctx->lang==BGBCC_LANG_CS) ||
		(ctx->lang==BGBCC_LANG_CX))
	{
		if(!strcmp(b, "using"))
		{
			s=BGBJP_Token(s, b, &ty);	//'name'
			BGBJP_Token(s, b2, &ty2);

			while(!strcmp(b2, "."))
			{
				s=BGBJP_Token(s, b2, &ty2);	//.
				s=BGBJP_Token(s, b2, &ty2);
				strcat(b, "/");
				strcat(b, b2);

				BGBJP_Token(s, b2, &ty2);
			}

			s1=bgbcc_strdup(b);

			//for(i=0; ctx->cur_nsi[i]; i++);
			//ctx->cur_nsi[i++]=s1;
			//ctx->cur_nsi[i]=NULL;

			n=BCCX_New("using");
			BCCX_Set(n, "name", s1);

			s=BGBJP_EatSemicolon(s);

			*str=s;
			return(n);
		}

		if(!strcmp(b, "namespace"))
		{
			s=BGBJP_Token(s, b, &ty);	//'name'
			BGBJP_Token(s, b2, &ty2);

			while(!strcmp(b2, "."))
			{
				s=BGBJP_Token(s, b2, &ty2);	//.
				s=BGBJP_Token(s, b2, &ty2);
				strcat(b, "/");
				strcat(b, b2);

				BGBJP_Token(s, b2, &ty2);
			}

			s0=ctx->cur_ns;
			s1=bgbcc_strdup(b);
			ctx->cur_ns=s1;

			//a=ctx->cur_nsi;
			//ctx->cur_nsi=ab;
			//ctx->cur_nsi[0]=NULL;

			s=BGBJP_Token(s, b, &ty); //{
			n1=BGBJP_Block(ctx, &s);
			n=BCCX_New1("namespace", n1);
			BCCX_Set(n, "name", s1);

			ctx->cur_ns=s0;
			//ctx->cur_nsi=a;

			*str=s;
			return(n);
		}
	}

	if(ctx->lang==BGBCC_LANG_CS)
	{
		if(!strcmp(b, "get"))
		{
			n1=BGBJP_BlockStatement2(ctx, &s);
			n=BCCX_New1("get", BCCX_New1("body", n1));

			*str=s;
			return(n);
		}

		if(!strcmp(b, "set"))
		{
			n1=BGBJP_BlockStatement2(ctx, &s);
			n=BCCX_New1("set", BCCX_New1("body", n1));

			*str=s;
			return(n);
		}
	}

	if(!strcmp(b, "try"))
	{
		n1=BGBJP_BlockStatement2(ctx, &s);
		n=BCCX_New1("try_catch", BCCX_New1("body", n1));

		BGBJP_Token(s, b, &ty);
		while(!strcmp(b, "catch"))
		{
			s=BGBJP_Token(s, b, &ty);	//'catch'

			BGBJP_Token(s, b2, &ty2);
			if(!strcmp(b2, "("))
			{
				s=BGBJP_Token(s, b2, &ty2);	//'('
//				s=BGBJP_Token(s, b, &ty);	//name
				n2=BGBJP_ArgDefinition(ctx, &s);
				s=BGBJP_Token(s, b2, &ty2);	//')'
			}else
			{
				n2=NULL;
			}

			n1=BGBJP_BlockStatement2(ctx, &s);
			n1=BCCX_New1("catch", BCCX_New1("body", n1));
			if(n2)BCCX_Add(n1, BCCX_New1("args", n2));

			BCCX_Add(n, n1);

			BGBJP_Token(s, b, &ty);
		}

		if(!strcmp(b, "finally"))
		{
			s=BGBJP_Token(s, b, &ty);	//'finally'
			n2=BGBJP_BlockStatement2(ctx, &s);
			n1=BCCX_New1("finally", BCCX_New1("body", n2));
			BCCX_Add(n, n1);
		}

		*str=s;
		return(n);
	}

	if(	(ctx->lang==BGBCC_LANG_JAVA) ||
		(ctx->lang==BGBCC_LANG_CX) ||
		(ctx->lang==BGBCC_LANG_BS2))
	{
		if(!strcmp(b, "import"))
		{
			s=BGBJP_Token(s, b, &ty);	//'name'
			BGBJP_Token(s, b2, &ty2);

			while(!strcmp(b2, "."))
			{
				s=BGBJP_Token(s, b2, &ty2);	//.
				s=BGBJP_Token(s, b2, &ty2);
				strcat(b, "/");
				strcat(b, b2);

				BGBJP_Token(s, b2, &ty2);
			}

			s1=bgbcc_strdup(b);

			//for(i=0; ctx->cur_nsi[i]; i++);
			//ctx->cur_nsi[i++]=s1;
			//ctx->cur_nsi[i]=NULL;

			n=BCCX_New("import");
			BCCX_Set(n, "name", s1);

			s=BGBJP_EatSemicolon(s);

			*str=s;
			return(n);
		}

		if(!strcmp(b, "package"))
		{
			s=BGBJP_Token(s, b, &ty);	//'name'
			BGBJP_Token(s, b2, &ty2);

			while(!strcmp(b2, "."))
			{
				s=BGBJP_Token(s, b2, &ty2);	//.
				s=BGBJP_Token(s, b2, &ty2);
				strcat(b, "/");
				strcat(b, b2);

				BGBJP_Token(s, b2, &ty2);
			}

			if(ctx->cur_ns)
			{
				sprintf(b2, "%s/%s", ctx->cur_ns, b);
				strcpy(b, b2);
			}

			s0=ctx->cur_ns;
			s1=bgbcc_strdup(b);
			ctx->cur_ns=s1;

			//a=ctx->cur_nsi;
			//ctx->cur_nsi=ab;
			//ctx->cur_nsi[0]=NULL;

//			s=BGBJP_Token(s, b, &ty); //{
//			n1=BGBJP_Block(ctx, &s);

			n=BCCX_New("package");
			BCCX_Set(n, "name", s1);

			BGBJP_Token(s, b2, &ty2);
			if(!strcmp(b, ";"))
			{
				s=BGBJP_EatSemicolon(s);
				n1=NULL;
			}else
			{
				n1=BGBJP_BlockStatement2(ctx, &s);
				BCCX_Add(n, BCCX_New1("body", n1));

				ctx->cur_ns=s0;
			}

			//ctx->cur_ns=s0;
			//ctx->cur_nsi=a;

			*str=s;
			return(n);
		}
	}

	if(!strcmp(b, "{"))
	{
		s=*str;
		n=BGBJP_BlockStatement2(ctx, &s);
		*str=s;
		return(n);
	}

	n=BGBJP_ParseBlockStatementName(ctx, b, &s);
	if(n)
	{
		*str=s;
		return(n);
	}

#if 0
	n=BGBJP_Definition(ctx, &s);
	if(n!=NULL)
	{
		BCCX_Set(n, "fn", fn);
		BCCX_SetInt(n, "ln", ln);

		s=BGBJP_EatSemicolon(s);
		*str=s;
		return(n);
	}
#endif

	s=*str;

	n=BGBJP_Statement(ctx, &s);
	s=BGBJP_EatSemicolon(s);
	*str=s;
	return(n);
}

BCCX_Node *BGBJP_BlockStatement(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	int ty, ln;
	BCCX_Node *n, *n1;
	char *s, *s1, *fn;
	char **a;
	int i;

	s=*str;

#if 1
	BGBJP_Token(s, b, &ty);
	while(!strcmp(b, "#"))
	{
		s=BGBJP_Token(s, b, &ty);
		while(*s && (*s!='\n'))s++;

		b[0]=0;
		BGBJP_Token(s, b, &ty);
	}
#endif

	s=BGBJP_EatWhite(s);
	fn=BGBJP_GetLastFileName();
	ln=BGBJP_GetLastLineNumber();

//	printf("@ %s:%d\n", fn, ln);

#if 1
	n=BGBJP_Definition(ctx, &s);
	if(n!=NULL)
	{
		BCCX_Set(n, "fn", fn);
		BCCX_SetInt(n, "ln", ln);

		s=BGBJP_EatSemicolon(s);
		*str=s;
		return(n);
	}
#endif

	n=BGBJP_DefClass(ctx, &s);
	if(n)
	{
		BCCX_Set(n, "fn", fn);
		BCCX_SetInt(n, "ln", ln);

		*str=s;
		return(n);
	}
	n=BGBJP_FlagsBlock(ctx, &s);
	if(n)
	{
		BCCX_Set(n, "fn", fn);
		BCCX_SetInt(n, "ln", ln);

		*str=s;
		return(n);
	}

	s1=ctx->cur_ns; a=ctx->cur_nsi;
	ctx->cur_ns=NULL; ctx->cur_nsi=NULL;

	n=BGBJP_BlockStatementInner(ctx, &s);

	ctx->cur_ns=s1; ctx->cur_nsi=a;

	if(n)
	{
		BCCX_Set(n, "fn", fn);
		BCCX_SetInt(n, "ln", ln);

		*str=s;
		return(n);
	}

	return(NULL);
}

BCCX_Node *BGBJP_Block(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	char *s, *s1;
	int ty, ty2, i;
	BCCX_Node *n, *lst, *lste;

	s=*str; s1=s;
	lst=NULL; lste=NULL;
	while(1)
	{
		BGBJP_Token(s, b, &ty);
		if(!*s || (*b=='}'))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}
		s1=BGBJP_EatWhite(s);
		if(!*s1) { s=s1; break; }

#ifdef CATCH_BLOCK_SEH
		__try {
#endif

		s1=s;
		n=BGBJP_BlockStatement(ctx, &s);
		if((n==NULL) || !s || (s==s1))
		{
			if(!s || s==s1)
			{
				BGBJP_Error(s, "PDSCR_CParse_Block: "
					"Unknown Parse Error\n");
				break;
			}
			continue;
		}
//		lst=BCCX_AddEnd(lst, n);
		lst=BCCX_AddEnd2(lst, &lste, n);

#ifdef CATCH_BLOCK_SEH
		} __except(EXCEPTION_EXECUTE_HANDLER)
		{
			printf("RECAUGHT\n");
			i=1;
			while(s && *s)
			{
				if(*s=='(') { i++; s++; continue; }
				if(*s==')') { i--; s++; continue; }
				if(*s=='[') { i++; s++; continue; }
				if(*s==']') { i--; s++; continue; }

				if(*s=='{') { i++; s++; continue; }
				if(*s=='}')
				{
					i--; s++;
					if(!i)break;
					continue;
				}
				if((i==1) && (*s==';'))
					{ s++; break; }
				s++;
			}
		}
#endif

	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBJP_BlockStatement2(BGBCP_ParseState *ctx, char **str)
{
	char b[256];
	char *s, *fn;
	int ty, ln;
	BCCX_Node *n;

	if(	(ctx->lang==BGBCC_LANG_C) ||
		(ctx->lang==BGBCC_LANG_CPP) ||
		(ctx->lang==BGBCC_LANG_CX) ||
		(ctx->lang==BGBCC_LANG_MINIC))
			return(BGBCP_BlockStatement2(ctx, str));

	s=*str;

	s=BGBJP_EatWhite(s);
	fn=BGBJP_GetLastFileName();
	ln=BGBJP_GetLastLineNumber();

	BGBJP_Token(s, b, &ty);
	if(!strcmp(b, "{"))
	{
		s=BGBJP_Token(s, b, &ty);
		n=BGBJP_Block(ctx, &s);
		n=BCCX_New1("begin", n);

		BCCX_Set(n, "fn", fn);
		BCCX_SetInt(n, "ln", ln);

		*str=s;
		return(n);
	}

	n=BGBJP_BlockStatement(ctx, str);
	return(n);
}
