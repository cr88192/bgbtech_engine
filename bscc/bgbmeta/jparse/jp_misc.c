#include <bgbccc.h>
#include <time.h>

extern int bgbjpp_warn, bgbjpp_err;
extern int bgbjp_warn, bgbjp_err;


BCCX_Node *BGBJP_FunArgs(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	BCCX_Node *n, *n1, *lst, *lste;

	s=*str; lst=NULL; lste=NULL;
	while(1)
	{
		b[0]=0;
		BGBJP_Token(s, b, &ty);
		if(ty==BTK_NULL)break;
		if((ty==BTK_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		n=BGBJP_Expression(ctx, &s);
//		lst=BCCX_AddEnd(lst, n);
		lst=BCCX_AddEnd2(lst, &lste, n);

		BGBJP_Token(s, b, &ty);
		if((ty==BTK_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		if(strcmp(b, ","))
		{
//			BGBJP_Error(s, "BGBJP_FunArgs: "
//				"Invalid Token '%s'\n", b);
//			*(int *)(-1)=-1;
			return(NULL);
		}

		s=BGBJP_Token(s, b, &ty);	//','
	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBJP_FunArgs2(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	BCCX_Node *n, *n1, *lst, *lste;

	s=*str; lst=NULL; lste=NULL;
	while(1)
	{
		b[0]=0;
		BGBJP_Token(s, b, &ty);
		if(ty==BTK_NULL)break;
		if((ty==BTK_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		n=BGBJP_Expression(ctx, &s);
//		lst=BCCX_AddEnd(lst, n);
		lst=BCCX_AddEnd2(lst, &lste, n);

		BGBJP_Token(s, b, &ty);
		if((ty==BTK_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, ","))
			s=BGBJP_Token(s, b, &ty);	//','
	}

	*str=s;
	return(lst);
}

BCCX_Node *BGBJP_ModuleBuffer(char *name, char *modname, char *buf)
{
	char b[256];
//	VFILE *fd;
	BGBCP_ParseState *ctx;
	char *tbuf, *s;
	BCCX_Node *n, *n1, *n2;
	int t0, t1, dt;

	tbuf=malloc(1<<24);
	memset(tbuf, 0, 1<<24);

	ctx=bgbcc_malloc(sizeof(BGBCP_ParseState));
	memset(ctx, 0, sizeof(BGBCP_ParseState));

	ctx->lang=BGBCP_LangForName(name);
	ctx->arch=BGBCC_GetArch();

	t0=clock();

	BGBJP_PushLinenum();
	BGBJP_SetLinenum(name, buf, 1);

//	BGBJPP_Filter(ctx, buf, tbuf);
	BGBPP_Filter(ctx, buf, tbuf);

	BGBJP_PopLinenum();

	t1=clock();
	dt=(1000.0*(t1-t0))/CLOCKS_PER_SEC;
	printf("PreProc took %dms\n", dt);

//	printf("PP\n%s\n\n", tbuf);

	sprintf(b, "dump/%s_pp.txt", modname);
	BGBCC_StoreTextFile(b, tbuf);

	t0=clock();

	bgbjp_warn=0;
	bgbjp_err=0;

	ctx->structs=NULL;
	ctx->types=NULL;

	BGBJP_SetLinenum(name, tbuf, 1);
	s=tbuf;
	n1=BGBJP_Block(ctx, &s);

	t1=clock();
	dt=(1000.0*(t1-t0))/CLOCKS_PER_SEC;
	printf("Parse took %dms\n", dt);

	n=BCCX_New3("module",
		BCCX_New1("types", ctx->types),
		BCCX_New1("structs", ctx->structs),
		BCCX_New1("body", n1));

	s=BGBCP_NameForLang(ctx->lang);
	BCCX_Set(n, "lang", s);

	s=BGBCP_NameForArch(ctx->arch);
	BCCX_Set(n, "arch", s);

	free(tbuf);

	return(n);
}
