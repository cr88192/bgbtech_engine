#include <bgbccc.h>

#ifdef WIN32
#include <windows.h>
#define CATCH_PP_SEH
#endif

char *bgbpp_ibuf;
char *bgbpp_obuf;

BGBPP_Def *bgbpp_sdef;	//static defines
// BGBPP_Def *bgbpp/_def;

int bgbpp_warn, bgbpp_err;

int bgbpp_iflvl;	//nested ifdef-level (true branches)
int bgbpp_iflvl2;	//nested ifdef-level (false branch)
int bgbpp_ifmark;	//set if 'true' branch already taken

char *bgbpp_spos;	//temp source position
char *bgbpp_lfn;	//last file name


//includes
char *bgbpp_sinc[256];	//static include paths
int bgbpp_nsinc=0;	//num static include paths

char *bgbpp_inc[256];	//include paths
int bgbpp_ninc;	//num include paths

int bgbpp_noinclude_p=0;	//_noinclude set

//char *(*bgbpp_loadfile_fp)(char *name, int *sz);

//char *bgbpp_loadfile(char *name, int *sz)
//{
//	if(bgbpp_loadfile_fp)
//		return(bgbpp_loadfile_fp(name, sz));
//	return(NULL);
//}

int BGBPP_AddIncludePathFront(char *str)
{
	int i;

	if(!str)return(-1);

	for(i=0; i<bgbpp_nsinc; i++)
		if(!strcmp(bgbpp_sinc[i], str))
			return(i);
	i=bgbpp_nsinc++;
	bgbpp_sinc[i]=strdup(str);
	return(i);
}

int BGBPP_AddIncludePathBack(char *str)
{
	int i;

	if(!str)return(-1);

	for(i=0; i<bgbpp_nsinc; i++)
		if(!strcmp(bgbpp_sinc[i], str))
			return(i);

	for(i=bgbpp_nsinc; i>0; i++)
		bgbpp_sinc[i]=bgbpp_sinc[i-1];
	bgbpp_nsinc++;
	bgbpp_sinc[0]=strdup(str);
	return(i);
}


#if 0
// int BGBPP_ParseChar(char **str)
{
	unsigned char *s;
	int i;

	s=(unsigned char *)(*str);
	i=*s++;

	if(i>=0xF0)
	{
		i=(i&7)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xE0)
	{
		i=(i&15)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xC0)
	{
		i=(i&31)<<6;
		i+=(*s++)&63;
	}else if(i>=0x80)
	{
//		continue;
	}

	*str=(char *)s;
	return(i);
}

// int BGBPP_EmitChar(char **str, int j)
{
	unsigned char *t;
	int i;

	t=(unsigned char *)(*str);

	if(j>0x3FFFFFF)
	{
		*t++=0xFC+(j>>30);
		*t++=0x80+((j>>24)&0x3F);
		*t++=0x80+((j>>18)&0x3F);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0x1FFFFF)
	{
		*t++=0xF8+(j>>24);
		*t++=0x80+((j>>18)&0x3F);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0xFFFF)
	{
		*t++=0xF0+(j>>18);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0x7FF)
	{
		*t++=0xE0+(j>>12);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if((j>0x7F) || !j)
	{
		*t++=0xC0+(j>>6);
		*t++=0x80+(j&0x3F);
	}else
	{
		if(!j) //Modified-UTF8
			{ *t++=0xC0; *t++=0x80; }
			else *t++=j;
	}

	*str=(char *)t;
	return(0);
}
#endif

char *BGBPP_EmitString(char *t, char *s)
{
	int i;

	if((*(t-1)=='\"') || (*(t-1)=='\''))
		*t++=' ';

	*t++='\"';
	while(*s)
	{
		i=BGBCP_ParseChar(&s);

		if((i<' ') || (i>='~'))
		{
			if(i>0xFF)
			{
				sprintf(t, "\\u%04X", i); t+=6;
				continue;
			}

			switch(i)
			{
			case '\r': *t++='\\'; *t++='r'; break;
			case '\n': *t++='\\'; *t++='n'; break;
			case '\t': *t++='\\'; *t++='t'; break;
			case '\x1B': *t++='\\'; *t++='e'; break;
			default: sprintf(t, "\\x%02X", i); t+=4; break;
			}
			continue;
		}

		if(i=='\"')*t++='\\';
		if(i=='\'')*t++='\\';
		if(i=='\\')*t++='\\';
		BGBCP_EmitChar(&t, i);
//		*t++=i;
	}

	*t++='\"';
	*t=0;

	return(t);
}

char *BGBPP_EmitCharString(char *t, char *s)
{
	int i;

	if((*(t-1)=='\"') || (*(t-1)=='\''))
		*t++=' ';

	*t++='\'';
	while(*s)
	{
		i=BGBCP_ParseChar(&s);

		if((i<' ') || (i>='~'))
		{
			if(i>0xFF)
			{
				sprintf(t, "\\u%04X", i); t+=6;
				continue;
			}

			switch(i)
			{
			case '\r': *t++='\\'; *t++='r'; break;
			case '\n': *t++='\\'; *t++='n'; break;
			case '\t': *t++='\\'; *t++='t'; break;
			case '\x1B': *t++='\\'; *t++='e'; break;
			default: sprintf(t, "\\x%02X", i); t+=4; break;
			}
			continue;
		}

		if(i=='\"')*t++='\\';
		if(i=='\'')*t++='\\';
		if(i=='\\')*t++='\\';
		BGBCP_EmitChar(&t, i);
//		*t++=i;
	}

	*t++='\'';
	*t=0;

	return(t);
}

void BGBPP_Trigraphs(BGBCP_ParseState *ctx, char *str)
{
	char *s, *t;

	//limit trigraphs to C/C++
	if((ctx->lang!=BGBCC_LANG_C) && (ctx->lang!=BGBCC_LANG_CPP))
		return;

	s=str; t=str;
	while(*s)
	{
		if((s[0]=='?') && (s[1]=='?'))
		{
			if(s[2]=='=') { *t++='#'; s+=3; continue; }
			if(s[2]==')') { *t++=']'; s+=3; continue; }
			if(s[2]=='!') { *t++='|'; s+=3; continue; }
			if(s[2]=='(') { *t++='['; s+=3; continue; }
			if(s[2]=='\'') { *t++='^'; s+=3; continue; }
			if(s[2]=='>') { *t++='}'; s+=3; continue; }
			if(s[2]=='/') { *t++='\\'; s+=3; continue; }
			if(s[2]=='<') { *t++='{'; s+=3; continue; }
			if(s[2]=='-') { *t++='~'; s+=3; continue; }
		}
		*t++=*s++;
		continue;
	}
	*t++=0;
}

void BGBPP_UniEscape(BGBCP_ParseState *ctx, char *str)
{
	char *s, *t;
	int i, j, k;

//	return;

	s=str; t=str;
	while(*s)
	{
		if((s[0]=='\\') && ((s[1]=='u') || (s[1]=='U')))
		{
			k=2;
			if(s[1]=='u')k=4;
			if(s[1]=='U')k=8;

			s+=2; j=0;
			while(k--)
			{
				j<<=4;
				if((*s>='0') && (*s<='9'))j+=*s-'0';
				if((*s>='A') && (*s<='F'))j+=*s-'A'+10;
				if((*s>='a') && (*s<='f'))j+=*s-'a'+10;
				s++;
			}
			BGBCP_EmitChar(&t, j);
			continue;
		}
		
		*t++=*s++;
		continue;
	}
	*t++=0;
}

int BGBPP_Error(BGBCP_ParseState *ctx, char *str, ...)
{
	char tb[1024];
	va_list lst;
	int l;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	
	l=BGBCP_CalcLinenum(bgbpp_spos);
	printf("pp-error: %s:%d: %s\n", bgbpp_lfn, l, tb);
	bgbpp_err++;

	return(0);
}

BGBPP_Def *BGBPP_LookupDefine(BGBCP_ParseState *ctx, char *name)
{
	char *args[64];
	char buf[256];
	BGBPP_Def *cur;
	char *s;
	int i;

	s=name; i=0; while(*s)i=(i*251)+(*s++);
	i&=255;

	cur=ctx->ppdef[i];
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	cur=bgbpp_sdef;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->next;
	}

	if(!bgbpp_noinclude_p)
		return(NULL);

	s=BGBCP_LookupPPKey(ctx, name);
	if(s && !strcmp(s, "define"))
	{
		sprintf(buf, "%s:value", name);
		s=BGBCP_LookupPPKey(ctx, buf);
		if(!s)s="";
		BGBPP_AddDefine(ctx, name, s);

		cur=BGBPP_LookupDefine(ctx, name);
		return(cur);
	}

	if(s && !strcmp(s, "macro"))
	{
		sprintf(buf, "%s:value", name);
		s=BGBCP_LookupPPKey(ctx, buf);
		if(!s)s="";

		for(i=0; i<64; i++)
		{
			sprintf(buf, "%s:arg.%d", name, i);
			s=BGBCP_LookupPPKey(ctx, buf);
			if(!s)break;
			args[i]=bgbcc_strdup(s);
		}
		args[i]=NULL;

		BGBPP_AddDefineArgs(ctx, name, args, s);

		cur=BGBPP_LookupDefine(ctx, name);
		return(cur);
	}

	return(NULL);
}

void BGBPP_DeleteDefine(BGBCP_ParseState *ctx, char *name)
{
	BGBPP_Def *cur, *prv;
	char *s;
	int i;

	s=name; i=0; while(*s)i=(i*251)+(*s++);
	i&=255;

	cur=ctx->ppdef[i]; prv=NULL;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(prv)
			{
				prv->next=cur->next;
			}else
			{
				ctx->ppdef[i]=cur->next;
			}
			if(cur->args)bgbcc_free(cur->args);
			bgbcc_free(cur->value);
			bgbcc_free(cur);
			return;
		}
		prv=cur; cur=cur->next;
	}
}

void BGBPP_CleanupDefines(BGBCP_ParseState *ctx)
{
#if 0
	BGBPP_Def *cur, *nxt;

	cur=ctx->ppdef;
	ctx->ppdef=NULL;

	while(cur)
	{
		nxt=cur->next;
		if(cur->args)bgbcc_free(cur->args);
		bgbcc_free(cur->value);
		bgbcc_free(cur);
		cur=nxt; 
	}
#endif
}

void BGBPP_AddDefine(BGBCP_ParseState *ctx, char *name, char *str)
{
	BGBPP_Def *cur;
	char *s;
	int i;

	s=name; i=0;
	while(*s)i=(i*251)+(*s++);
	i&=255;

	cur=bgbcc_malloc(sizeof(BGBPP_Def));
	memset(cur, 0, sizeof(BGBPP_Def));

	cur->name=bgbcc_strdup(name);
	cur->value=bgbcc_strdup(str);

	cur->next=ctx->ppdef[i];
	ctx->ppdef[i]=cur;
}

void BGBPP_AddDefineArgs(BGBCP_ParseState *ctx,
	char *name, char **args, char *str)
{
	BGBPP_Def *cur;
	char *s;
	int i, hi;

	s=name; hi=0; while(*s)hi=(hi*251)+(*s++);
	hi&=255;

	cur=bgbcc_malloc(sizeof(BGBPP_Def));
	memset(cur, 0, sizeof(BGBPP_Def));

	cur->name=bgbcc_strdup(name);
	cur->value=bgbcc_strdup(str);

	for(i=0; args[i]; i++);
	cur->args=bgbcc_malloc((i+1)*sizeof(char *));
	for(i=0; args[i]; i++)cur->args[i]=bgbcc_strdup(args[i]);
	cur->args[i]=NULL;

	cur->next=ctx->ppdef[hi];
	ctx->ppdef[hi]=cur;
}


void BGBPP_AddStaticDefine(BGBCP_ParseState *ctx,
	char *name, char *str)
{
	BGBPP_Def *cur;

	cur=bgbcc_malloc2(sizeof(BGBPP_Def));
//	cur=malloc(sizeof(BGBPP_Def));
	memset(cur, 0, sizeof(BGBPP_Def));

	cur->name=bgbcc_strdup(name);
	cur->value=bgbcc_strdup(str);

	cur->next=bgbpp_sdef;
	bgbpp_sdef=cur;
}

void BGBPP_AddStaticDefineArgs(BGBCP_ParseState *ctx,
	char *name, char **args, char *str)
{
	BGBPP_Def *cur;
	int i;

	cur=bgbcc_malloc2(sizeof(BGBPP_Def));
//	cur=malloc(sizeof(BGBPP_Def));
	memset(cur, 0, sizeof(BGBPP_Def));

	cur->name=bgbcc_strdup(name);
	cur->value=bgbcc_strdup(str);

	for(i=0; args[i]; i++);
	cur->args=bgbcc_malloc2((i+1)*sizeof(char *));
//	cur->args=malloc((i+1)*sizeof(char *));
	for(i=0; args[i]; i++)
		cur->args[i]=bgbcc_strdup(args[i]);
	cur->args[i]=NULL;

	cur->next=bgbpp_sdef;
	bgbpp_sdef=cur;
}

void BGBPP_SendDefines(BGBCP_ParseState *ctx)
{
	char buf[256];
	BGBPP_Def *cur;
	char *s;
	int i, j;

	for(i=0; i<256; i++)
	{
		cur=ctx->ppdef[i];
		while(cur)
		{
//			if(!strcmp(cur->name, name))
//				return(cur);

			if(cur->args)
			{
				BGBCC_BindPPKey(ctx, cur->name, "macro");
				sprintf(buf, "%s:value", cur->name);
				BGBCC_BindPPKey(ctx, buf, cur->value);

				for(j=0; cur->args[j]; j++)
				{
					sprintf(buf, "%s:arg.%d",
						cur->name, j);
					BGBCC_BindPPKey(ctx,
						buf, cur->args[j]);
				}

				sprintf(buf, "%s:arg.%d", cur->name, j);
				BGBCC_BindPPKey(ctx, buf, NULL);
			}else
			{
				BGBCC_BindPPKey(ctx, cur->name, "define");
				sprintf(buf, "%s:value", cur->name);
				BGBCC_BindPPKey(ctx, buf, cur->value);
			}

			cur=cur->next;
		}
	}
}

char *BGBPP_LoadInclude(BGBCP_ParseState *ctx, char *name, int *rsz)
{
	char b[256];
	char *buf;
	int i, sz;

	buf=NULL;
	for(i=(bgbpp_ninc-1); i>=0; i--)
	{
		sprintf(b, "%s/%s", bgbpp_inc[i], name);
//		buf=bgbpp_loadfile(b, &sz);
		buf=bgbcc_loadfile(b, &sz);
		if(buf) { if(rsz)*rsz=sz; return(buf); }
	}

	for(i=(bgbpp_nsinc-1); i>=0; i--)
	{
		sprintf(b, "%s/%s", bgbpp_sinc[i], name);
		buf=bgbcc_loadfile(b, &sz);
		if(buf) { if(rsz)*rsz=sz; return(buf); }
	}

	return(NULL);
}

void BGBPP_Include(BGBCP_ParseState *ctx, char *str)
{
	char b[256];
	char *buf;
	int i;

	buf=NULL;
	for(i=(bgbpp_ninc-1); i>=0; i--)
	{
		sprintf(b, "%s/%s", bgbpp_inc[i], str);
		buf=bgbcc_loadfile(b, NULL);
		if(buf)break;
	}

	if(!buf)
	{
		printf("BGBPP_Directive: fail open include"
			" '%s'\n", str);
		bgbpp_err++;
		return;
	}

//	printf("Include: '%s' %d bytes\n", str, strlen(buf));

	BGBCP_PushLinenum();
	BGBCP_SetLinenum(str, buf, 1);

	BGBPP_Buffer(ctx, buf);
//	free(buf);

	BGBCP_PopLinenum();
}

void BGBPP_Directive2(BGBCP_ParseState *ctx, char *str)
{
	char b[256], b2[4096];
	int ty, ty2;
	char *s, *t, *s1;
	BCCX_Node *n0;
	int i;

	s=str;
	s=BGBCP_EatWhiteOnly(s);
	s++;

	s=BGBCP_TokenCtx(ctx, s, b, &ty);
	BGBCP_TokenCtx(ctx, s, b2, &ty2);

	if(!strcmp(b, "if"))		{ bgbpp_iflvl2++; return; }
	if(!strcmp(b, "ifdef"))		{ bgbpp_iflvl2++; return; }
	if(!strcmp(b, "ifndef"))	{ bgbpp_iflvl2++; return; }
	if(!strcmp(b, "endif"))		{ bgbpp_iflvl2--; return; }

	if(!strcmp(b, "else"))
	{
//		if(bgbpp_iflvl2==1)
		if((bgbpp_iflvl2==1) && !bgbpp_ifmark)
		{
			bgbpp_iflvl2--;
			bgbpp_iflvl++;
			return;
		}
		return;
	}

	if(!strcmp(b, "elif"))
	{
//		if(bgbpp_iflvl2==1)
		if((bgbpp_iflvl2==1) && !bgbpp_ifmark)
		{
			strcpy(b2, s);
			i=1; while(i)i=BGBPP_Line(ctx, b2);

			s1=b2;
			n0=BGBCP_Expression(ctx, &s1);
			n0=BGBCC_ReduceExpr(NULL, n0);
			i=BGBCC_BoolExpr(NULL, n0);

			if(i>0)
			{
				bgbpp_iflvl2--;
				bgbpp_iflvl++;
				return;
			}else if(i<0)
			{
				BGBPP_Error(ctx, "#elif pp-expr couldn't be evaluated\n");
				//error
			}

			return;
		}

		return;
	}
}

void BGBPP_Directive(BGBCP_ParseState *ctx, char *str)
{
	char b[256], b2[4096];
	char *ab[64];
	BGBPP_Def *def;
	BCCX_Node *exp;
	char *s, *t, *s1, *s2, *tn;
	int ty, ty2;
	int i, j, n;

#if 0
	if(strlen(str)>=2048)
	{
		printf("PP ?: %s\n", str);
		return;
	}
#endif

	if(bgbpp_iflvl2)
	{
		BGBPP_Directive2(ctx, str);
		return;
	}

	s=str;
//	while(*s && (*s<=' '))s++;
	s=BGBCP_EatWhiteOnly(s);

	s++;

	s=BGBCP_TokenCtx(ctx, s, b, &ty);
	s1=BGBCP_TokenCtx(ctx, s, b2, &ty2);

	if(!strcmp(b, "pragma"))
	{
		return;
	}

	if(!strcmp(b, "error"))
	{
		s=BGBCP_EatWhiteOnly(s);
		printf("BGBPP_Directive: %s:%d: #error: %s\n",
			BGBCP_GetFilename(),
			BGBCP_CalcLinenum(bgbpp_spos), s);
		bgbpp_err++;

		return;
	}

	if(!strcmp(b, "warning"))
	{
		s=BGBCP_EatWhiteOnly(s);
		printf("BGBPP_Directive: %s:%d: #warning: \"%s\"\n",
			BGBCP_GetFilename(),
			BGBCP_CalcLinenum(bgbpp_spos), s);
		bgbpp_warn++;

		return;
	}

	if(!strcmp(b, "include"))
	{
		i=1; strcpy(b, s);
		while(i)i=BGBPP_Line(ctx, b);

		s1=BGBCP_TokenCtx(ctx, b, b2, &ty2);

		if(!strcmp(b2, "<"))
		{
//			BGBCP_TokenCtx(ctx, s1, b2, &ty2);

			t=b2;
			while(*s1 && (*s1!='>'))*t++=*s1++;
			*t++=0;

			BGBPP_Include(ctx, b2);
			return;
		}

		t=b2; t+=strlen(t);
		while(*s1)
		{
			s1=BGBCP_TokenCtx(ctx, s1, t, &ty2);
			t+=strlen(t);
		}
		*t=0;

		BGBPP_Include(ctx, b2);
		return;
	}

	if(!strcmp(b, "define"))
	{
		if(*s1!='(')
		{
			if(!strcmp(b2, "_noinclude"))
				{ bgbpp_noinclude_p=1; return; }

			while(*s1 && (*s1<=' '))s1++;
			BGBPP_AddDefine(ctx, b2, s1);
			return;
		}

		tn=bgbcc_strdup(b2);

		s1++;
		n=0;
		while(1)
		{
			s1=BGBCP_TokenCtx(ctx, s1, b, &ty);
			if(!strcmp(b, ")"))break;
			if(!strcmp(b, ","))continue;

			ab[n++]=bgbcc_strdup(b);
		}
		ab[n]=NULL;

		while(*s1 && (*s1<=' '))s1++;

		BGBPP_AddDefineArgs(ctx, tn, ab, s1);
		return;
	}

	if(!strcmp(b, "endif"))
	{
		bgbpp_iflvl--;
		return;
	}

	if(!strcmp(b, "else"))
	{
		bgbpp_iflvl--;
		bgbpp_iflvl2++;
		return;
	}

	if(!strcmp(b, "elif"))
	{
#if 0
		strcpy(b2, s);
		i=1; while(i)i=BGBPP_Line(ctx, b2);

		s1=b2;
		exp=BGBCP_Expression(ctx, &s1);
		exp=BGBCC_ReduceExpr(NULL, exp);
		i=BGBCC_BoolExpr(NULL, exp);

		if(i==0)
		{
			bgbpp_iflvl--;
			bgbpp_iflvl2++;
			return;
		}
#endif

		bgbpp_iflvl--;
		bgbpp_iflvl2++;
		return;
	}

	if(!strcmp(b, "ifdef"))
	{
		bgbpp_ifmark=0;
		def=BGBPP_LookupDefine(ctx, b2);
		if(!def)bgbpp_iflvl2++;
			else bgbpp_iflvl++;
		if(!bgbpp_iflvl2)bgbpp_ifmark=1;
		return;
	}
	if(!strcmp(b, "ifndef"))
	{
		bgbpp_ifmark=0;
		def=BGBPP_LookupDefine(ctx, b2);
		if(!def)bgbpp_iflvl++;
			else bgbpp_iflvl2++;
		if(!bgbpp_iflvl2)bgbpp_ifmark=1;
		return;
	}

	if(!strcmp(b, "undef"))
	{
		BGBPP_DeleteDefine(ctx, b2);
		return;
	}

	if(!strcmp(b, "if"))
	{
		i=1;
		strcpy(b2, s);
		while(i)i=BGBPP_Line(ctx, b2);

		s2=b2;
		exp=BGBCP_Expression(ctx, &s2);
		exp=BGBCC_ReduceExpr(NULL, exp);
		i=BGBCC_BoolExpr(NULL, exp);

		bgbpp_ifmark=0;
		if(i>0) { bgbpp_iflvl++; }
			else { bgbpp_iflvl2++; }
		if(!bgbpp_iflvl2)bgbpp_ifmark=1;

		if(i<0)
		{
			BGBPP_Error(ctx, "#if pp-expr couldn't be evaluated\n");
			BCCX_Print(exp);
		}

#if 0
		else if(!i) { bgbpp_iflvl2++; }
		else if(i<0)
		{
			BGBPP_Error(ctx, "#if pp-expr couldn't be evaluated\n");
		}
#endif
		return;
	}

	printf("BGBPP_Directive: %s:%d: Unsupported preproc directive %s\n",
		BGBCP_GetFilename(),
		BGBCP_CalcLinenum(bgbpp_spos), b);
	bgbpp_err++;
}


char *BGBPP_ParseArg(BGBCP_ParseState *ctx, char *s, char *b)
{
	char b2[4096];
	int i, ty2;

	i=0;
	while(*s)
	{
		if((*s=='\"') || (*s=='\''))
		{
			s=BGBCP_TokenCtx(ctx, s, b2, &ty2);
			b=BGBPP_PrintToken(b, b2, ty2);
			continue;
		}

		if(*s=='(') { i++; *b++=*s++; continue; }
		if(*s==')') { if(!i)break; i--; *b++=*s++; continue; }
		if(*s==',') { if(!i)break; *b++=*s++; continue; }
		*b++=*s++;
	}

	*b++=0;
	return(s);
}

char *BGBPP_PrintName(char *t, char *s)
{
	int i;

	i=*(t-1);
	if((i=='_') || ((i>='0') && (i<='9')) ||
		((i>='A') && (i<='Z')) || ((i>='a') && (i<='z')))
			*t++=' ';
	strcpy(t, s);
	t+=strlen(s);
	return(t);
}

char *BGBPP_PrintToken(char *t, char *s, int ty)
{
	if(ty==BTK_NAME)
	{
		t=BGBPP_PrintName(t, s);
		return(t);
	}

	if(ty==BTK_NUMBER)
	{
		t=BGBPP_PrintName(t, s);
		return(t);
	}

	if(ty==BTK_STRING)
	{
		t=BGBPP_EmitString(t, s);
		return(t);
	}

	if(ty==BTK_CHARSTRING)
	{
		t=BGBPP_EmitCharString(t, s);
		return(t);
	}

	if(ty==BTK_OPERATOR)
	{
		if(BGBCP_OpChar(*(t-1)))
			*t++=' ';
		sprintf(t, "%s", s);
		t+=strlen(t);
		return(t);
	}

	sprintf(t, "%s", s);
	t+=strlen(t);
	return(t);
}

int BGBPP_Expand(BGBCP_ParseState *ctx, char **src, char **dst, char *op)
{
	char b[256], b2[256];
	char *ab[64];
	BGBPP_Def *def;
	char *s, *s1, *t;
	int i, j, ty, ty2;

	s=*src; t=*dst;

	if(!strcmp(op, "defined"))
	{
		s=BGBCP_TokenCtx(ctx, s, b, &ty);	//<name> | '('
		if(!strcmp(b, "("))
		{
			s=BGBCP_TokenCtx(ctx, s, b, &ty); //<name>
			def=BGBPP_LookupDefine(ctx, b);
			s=BGBCP_TokenCtx(ctx, s, b, &ty); //')'

			sprintf(t, "%d ", def?1:0);
			t+=strlen(t);

			*src=s; *dst=t;
			return(1);
		}

		def=BGBPP_LookupDefine(ctx, b);
		sprintf(t, "%d ", def!=NULL);
		t+=strlen(t);

		*src=s; *dst=t;
		return(1);
	}

	def=BGBPP_LookupDefine(ctx, op);
	if(!def)return(0);

	if(!def->args)
	{
		t=BGBPP_PrintName(t, def->value);
		*src=s; *dst=t;
		return(1);
	}

	s=BGBCP_EatWhiteOnly(s);

	if(*s!='(')return(0);

	s++; j=0;
	while(def->args[j])
	{
		while(*s && (*s<=' '))s++;

		if(*s==',')
		{
			ab[j++]=bgbcc_strdup("");
			s++; continue;
		}
		if(*s==')') break;
		while(*s && (*s<=' '))s++;

		s=BGBPP_ParseArg(ctx, s, b2);
		ab[j++]=bgbcc_strdup(b2);

		if(*s==',')
		{
			s++;
			while(*s && (*s<=' '))s++;
			if(*s==')')
			{
				ab[j++]=bgbcc_strdup("");
				break;
			}
			continue;
		}
	}
	ab[j]=NULL;

	if(def->args[j] || (*s!=')'))
	{
		printf("BGBPP_Expand: macro parse problem (%s)\n", op);
		bgbpp_err++;
		return(0);
	}
	s++;

	*src=s;

	s=def->value;
	while(*s)
	{
		s=BGBCP_TokenCtx(ctx, s, b, &ty);

		if(ty==BTK_NAME)
		{
			for(i=0; def->args[i]; i++)
				if(!strcmp(def->args[i], b))
					break;

			if(def->args[i])
			{
				t=BGBPP_PrintName(t, ab[i]);
				continue;
			}

			t=BGBPP_PrintName(t, b);
			continue;
		}

		if(ty==BTK_NUMBER)
		{
			t=BGBPP_PrintName(t, b);
			continue;
		}

		if(ty==BTK_STRING)
		{
			t=BGBPP_EmitString(t, b);
			continue;
		}

		if(ty==BTK_CHARSTRING)
		{
			t=BGBPP_EmitCharString(t, b);
			continue;
		}

		if(ty==BTK_OPERATOR)
		{
			if(BGBCP_OpChar(*(t-1)))
				*t++=' ';
			sprintf(t, "%s", b);
			t+=strlen(t);
			continue;
		}

		sprintf(t, "%s", b);
		t+=strlen(t);
	}

	*t=0;

	*dst=t;
	return(1);
}

int BGBPP_Line(BGBCP_ParseState *ctx, char *str)
{
	char lbuf[16384];
	char b[256], b2[256];
	char *s, *t, *t1;
	int i, ty, ty2, ni;

	BGBCP_FlushToken(str);

	ni=0;
	s=str; t=lbuf;
	while(*s)
	{
		s=BGBCP_TokenCtx(ctx, s, b, &ty);
		BGBCP_TokenCtx(ctx, s, b2, &ty2);

		if((ty2==BTK_OPERATOR) &&
			(!strcmp(b2, "##") || !strcmp(b, "%:%:")))
		{
			if(ty==BTK_NAME)
			{
				t1=t+1;
				i=BGBPP_Expand(ctx, &s, &t1, b);
				if(i>0) { strcpy(b, t+1); ni++; }
			}

			s=BGBCP_TokenCtx(ctx, s, b2, &ty2);	//'##'

			s=BGBCP_TokenCtx(ctx, s, b2, &ty2);

			if(ty2==BTK_NAME)
			{
				t1=t+1;
				i=BGBPP_Expand(ctx, &s, &t1, b2);
				if(i>0) { strcpy(b2, t+1); ni++; }
			}

//			printf("## operator: '%s' ## '%s' ('%s')\n", b, b2, str);

			*t=0;
			strcat(b, b2);
			
			if((ty==BTK_STRING)||(ty2==BTK_STRING))
			{
				t=BGBPP_EmitString(t, b);
			}else if((ty==BTK_CHARSTRING)||(ty2==BTK_CHARSTRING))
			{
				t=BGBPP_EmitCharString(t, b);
			}else
			{
				t=BGBPP_PrintName(t, b);
			}
			
//			printf("## operator: '%s'\n", b);
			
			ni++;
			continue;
		}

		if(ty==BTK_NAME)
		{
			i=BGBPP_Expand(ctx, &s, &t, b);
			if(i>0)
			{
				ni++;
				continue;
			}

			t=BGBPP_PrintName(t, b);
			continue;
		}

		if((ty==BTK_OPERATOR) &&
			(!strcmp(b, "#") || !strcmp(b, "%:")))
		{
			if(ty2==BTK_NAME)
			{
				s=BGBCP_TokenCtx(ctx, s, b2, &ty2);

				t1=t;
				i=BGBPP_Expand(ctx, &s, &t1, b2);
				if(i>0) { strcpy(b2, t); ni++; }

				t=BGBPP_PrintToken(t, b2, BTK_STRING);
				ni++;
				continue;
			}

			s=BGBCP_TokenCtx(ctx, s, b2, &ty2);
			t=BGBPP_PrintToken(t, b2, BTK_STRING);
			ni++;
			continue;
		}

		if(ty==BTK_NUMBER)
		{
			t=BGBPP_PrintName(t, b);
			continue;
		}

		if(ty==BTK_STRING)
		{
			t=BGBPP_EmitString(t, b);
			continue;
		}

		if(ty==BTK_CHARSTRING)
		{
			t=BGBPP_EmitCharString(t, b);
			continue;
		}

		if(ty==BTK_OPERATOR)
		{
			if(BGBCP_OpChar(*(t-1)))
				*t++=' ';
			sprintf(t, "%s", b);
			t+=strlen(t);
			continue;
		}

		sprintf(t, "%s", b);
		t+=strlen(t);
	}
	*t=0;


	BGBCP_FlushToken(str);

	memset(str, 0, 256);
	strcpy(str, lbuf);
	return(ni);
}

int BGBPP_LineDigraph(BGBCP_ParseState *ctx, char *str)
{
	char lbuf[16384], b[256];
	char *s, *t;
	int i, ty;

	//limit digraphs to C/C++
	if((ctx->lang!=BGBCC_LANG_C) && (ctx->lang!=BGBCC_LANG_CPP))
		return(0);

	s=str; t=lbuf;
	while(*s)
	{
		s=BGBCP_TokenCtx(ctx, s, b, &ty);

		if((ty==BTK_BRACE) || (ty==BTK_OPERATOR))
		{
			if(!strcmp(b, "<:"))strcpy(b, "[");
			if(!strcmp(b, ":>"))strcpy(b, "]");
			if(!strcmp(b, "<%"))strcpy(b, "{");
			if(!strcmp(b, "%>"))strcpy(b, "}");
			if(!strcmp(b, "%:"))strcpy(b, "#");
			if(!strcmp(b, "%:%:"))strcpy(b, "##");
		}

		t=BGBPP_PrintToken(t, b, ty);
	}
	*t=0;

	strcpy(str, lbuf);
	return(0);
}

int BGBPP_LinePostFilter(BGBCP_ParseState *ctx, char *str)
{
	BGBPP_LineDigraph(ctx, str);
	return(0);
}


char *BGBPP_ParseLine(BGBCP_ParseState *ctx, char *s, char *b)
{
	static int is=0;
	char *t;

	t=b;
	while(*s)
	{
		if(*s=='\"')
		{
			is=!is;
			*t++=*s++;
			continue;
		}

		if(is && (s[0]=='\\') && (s[1]=='\"'))
			{ *t++=*s++; *t++=*s++; continue; }
		if(is && (s[0]=='\\') && (s[1]=='\\'))
			{ *t++=*s++; *t++=*s++; continue; }

		if(is) { *t++=*s++; continue; }
		if((*s=='\n') || (*s=='\r')) break;

		if((s[0]=='/') && (s[1]=='/'))
		{
//			*t++=' ';
//			while(*s && (*s!='\n') && (*s!='\r'))
//				s++;
			break;
		}

		if((s[0]=='/') && (s[1]=='*'))
		{
			s+=2;
//			while(*s && (*s!='\n') && (*s!='\r'))
			while(*s)
			{
				if((s[0]=='*') && (s[1]=='/'))
				{
					s+=2;
					break;
				}
				s++;
			}
			*t++=' ';
			continue;
		}

		if((*s=='\\') && ((s[1]=='\r') || (s[1]=='\n')))
		{
			s++;
			if(*s=='\r')s++;
			if(*s=='\n')s++;
			while(*s && (*s<=' ') && (*s!='\r') && (*s!='\n'))
				s++;
			*t++=' ';
			continue;
		}
		*t++=*s++;
	}
	*t++=0;
	if(*s=='\r')s++;
	if(*s=='\n')s++;

	return(s);
}

void BGBPP_BufferLine(BGBCP_ParseState *ctx, char *b)
{
	char *s, *t, *s1;
	int i;

	t=b; while(*t && (*t<=' '))t++;
	if(!*t)
	{
//		if(!bgbpp_iflvl2)
//			sprintf(bgbpp_obuf, "\n");
		*b=0;
		return;
	}

	BGBPP_Trigraphs(ctx, b);
	BGBPP_UniEscape(ctx, b);

	s1=b; while(*s1 && (*s1<=' '))s1++;
	if(*s1=='#')
	{
		BGBCP_FlushToken(b);
		BGBPP_Directive(ctx, b);
		return;
	}

	if(bgbpp_iflvl2)return;

	i=1;
	while(i)
	{
		BGBCP_FlushToken(b);
		i=BGBPP_Line(ctx, b);
	}

	BGBCP_FlushToken(b);
	BGBPP_LinePostFilter(ctx, b);

	s1=BGBCP_GetFilename();
	if(s1!=bgbpp_lfn)
	{
		sprintf(bgbpp_obuf, "/*\"%s\"%d*/ %s\n",
			s1, BGBCP_CalcLinenum(bgbpp_spos), b);
		bgbpp_lfn=s1;
	}else
	{
		sprintf(bgbpp_obuf, "/*%d*/ %s\n",
			BGBCP_CalcLinenum(bgbpp_spos), b);
	}
}

void BGBPP_Buffer(BGBCP_ParseState *ctx, char *ibuf)
{
	char b[16384];
	char *s, *t, *s1;
	int i;

	s=ibuf;
	while(*s)
	{
		bgbpp_spos=s;
		
#if 1
		s1=s;
		while(*s1 && (*s1<=' '))s1++;
		if((s1[0]=='/') && (s1[1]=='/'))
		{
			s=s1; t=bgbpp_obuf;
			while(*s && (*s!='\r') && (*s!='\n')) *t++=*s++;
			if(*s=='\r')s++; if(*s=='\n')*t++=*s++;
//			while(*s && (*s<=' '))*t++=*s++;
			bgbpp_obuf=t; *t=0;
			continue;
		}

		if((s1[0]=='/') && (s1[1]=='*') && (s1[2]=='*'))
		{
			s=s1; t=bgbpp_obuf;
			*t++=*s++; *t++=*s++; *t++=*s++;
			while(*s)
			{
				if((s[0]=='*') && (s[1]=='/'))
					{ *t++=*s++; *t++=*s++; break; }
				if(*s=='\r') { s++; continue; }
				*t++=*s++;
			}
//			*t++=' ';
			while(*s && ((*s==' ')||(*s=='\t'))) *t++=*s++;
			if(*s=='\r')s++; if(*s=='\n')*t++=*s++;
//			while(*s && (*s<=' '))*t++=*s++;
			bgbpp_obuf=t; *t=0;
			continue;
		}

		if((s1[0]=='/') && (s1[1]=='*'))
		{
			s=s1; t=bgbpp_obuf;
			s+=2;
			while(*s)
			{
				if((s[0]=='*') && (s[1]=='/'))
					{ s+=2; break; }
				s++;
			}
			while(*s && (*s<=' '))
				{ *s++; }
			bgbpp_obuf=t; *t=0;
			continue;
		}
#endif
		
		s=BGBPP_ParseLine(ctx, s, b);

#ifdef CATCH_PP_SEH
		__try {
#endif

		BGBPP_BufferLine(ctx, b);

#ifdef CATCH_PP_SEH
		} __except(EXCEPTION_EXECUTE_HANDLER)
		{
			printf("RECAUGHT(PP)\n");
			continue;
		}
#endif

//		sprintf(bgbpp_obuf, "%s\n", b);
		bgbpp_obuf+=strlen(bgbpp_obuf);
	}
}


int BGBPP_Filter(BGBCP_ParseState *ctx, char *ibuf, char *obuf)
{
	char tb[256];
	char *s, *t;
	int i;


	for(i=0; i<bgbpp_nsinc; i++)
		bgbpp_inc[i]=bgbpp_sinc[i];
	bgbpp_ninc=bgbpp_nsinc;

	s=BGBCP_GetFilename();
	if(*s)
	{
		strcpy(tb, s);
		s=tb+strlen(tb);
		while((s>tb) && (*s!='/'))s--;

		if(*s=='/')
		{
			*s=0;

			i=bgbpp_ninc++;
			bgbpp_inc[i]=bgbcc_strdup(tb);
		}
	}

	bgbpp_ibuf=ibuf;
	bgbpp_obuf=obuf;
	bgbpp_noinclude_p=0;

	for(i=0; i<256; i++)
		ctx->ppdef[i]=NULL;

	bgbpp_warn=0;
	bgbpp_err=0;

//	BGBPP_Include(ctx, "_bscc_conf.h");

	BGBCP_FlushToken(ibuf);
	BGBPP_Buffer(ctx, ibuf);

	BGBPP_SendDefines(ctx);
	BGBCP_FlushToken(obuf);
	
	return(0);
}

