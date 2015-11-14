/*
Written 2007-2010 by Brendan G Bohannon
This code is re-assigned into the public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bs2vm.h>


char *bs2vm_module;
char *bs2vm_lfn;
int bs2vm_lln;

int bs2vm_blockseq=1;
int bs2vm_gsseq=1;

char *BS2VM_GenSym()
{
	char tb[256];
	sprintf(tb, "B2GS%d", bs2vm_gsseq++);
	return(bs2vm_strdup(tb));
}

char *BS2VM_GetAliasTop(BS2VM_Context *ctx, char *str)
{
	int i;

	for(i=0; i<ctx->top->nalias; i++)
	{
		if(!strcmp(ctx->top->aliasn[i], str))
			return(ctx->top->aliasv[i]);
	}

	if((ctx->top->nalias+1)>=ctx->top->malias)
	{
		i=ctx->top->malias; i=i+(i>>1);
		ctx->top->aliasn=bs2vm_realloc(ctx->top->aliasn, i*sizeof(char *));
		ctx->top->aliasv=bs2vm_realloc(ctx->top->aliasv, i*sizeof(char *));
		ctx->top->malias=i;
	}

	i=ctx->top->nalias++;
	ctx->top->aliasn[i]=bs2vm_strdup(str);
	ctx->top->aliasv[i]=BS2VM_GenSym();
	return(ctx->top->aliasv[i]);
}

char *BS2VM_AliasNameTop(BS2VM_Context *ctx, char *str)
{
	int i;

	for(i=0; i<ctx->top->nalias; i++)
	{
		if(!strcmp(ctx->top->aliasn[i], str))
			return(ctx->top->aliasv[i]);
	}

	return(str);
}

void BS2VM_AddParseNewBlock(BS2VM_Context *ctx, char *buf)
{
	BS2VM_Block *blk, *cur;
	char *s;

	s=buf;
	blk=BS2VM_ParseBlock(ctx, &s);
	blk->chain=NULL;

	if(ctx->ch_blk)
	{
		cur=ctx->ch_blk;
		while(cur->chain)cur=cur->chain;
		cur->chain=blk;
	}else ctx->ch_blk=blk;
}

void BS2VM_AddParseNewInline(BS2VM_Context *ctx, char *buf)
{
	BS2VM_Block *blk;
	char *s;

	blk=BS2VM_LookupText(ctx, buf);
	if(blk)
	{
//		BS2VM_ExecBlock(ctx, blk);
		return;
	}

	s=buf;
	blk=BS2VM_ParseInlineProc(ctx, &s);
	if(!blk)return;

	blk->text=bs2vm_strdup(buf);

//	BS2VM_ExecBlock(ctx, blk);
}

BS2VM_Block *BS2VM_ParseProc(BS2VM_Context *ctx, char **str)
{
	char b[64], b2[64];

	long long li, lj, lk;
	BS2VM_Block *blk;
	byte *ip;
	char *s, *t;
	int ty, ty2;
	int n, nlbl, nsym;
	int i, j, k, l, ni, nj;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);

	if(ty!=BS2VM_TOKEN_NAME)
		return(NULL);

	blk=BS2VM_AllocBlock(ctx);
	blk->name=bs2vm_strdup(b);

	blk->lfn=bs2vm_lfn;
	blk->lln=bs2vm_lln;

	while(1)
	{
		BS2VM_Parse_Token(s, b, &ty);
		if(!strcmp(b, "END"))
		{
			s=BS2VM_Parse_Token(s, b, &ty);
			break;
		}

		s=BS2VM_ParseCommand(ctx, blk, s);
	}

	BS2VM_FinishBlock(ctx, blk);

	*str=s;
	return(blk);
}

BS2VM_Block *BS2VM_ParseInlineProc(BS2VM_Context *ctx, char **str)
{
	char b[64];
	BS2VM_Block *blk;
	char *s, *t;
	int ty;
	int i, j, k;


	s=*str;
	blk=BS2VM_AllocBlock(ctx);
	blk->name=BS2VM_GenSym();

	blk->lfn=bs2vm_lfn;
	blk->lln=bs2vm_lln;

	while(1)
	{
		s=BS2VM_Parse_EatWhite(s);
		if(!*s)break;

		BS2VM_Parse_Token(s, b, &ty);
		if(ty==BS2VM_TOKEN_NULL)break;

		if(!strcmp(b, "END"))
		{
			s=BS2VM_Parse_Token(s, b, &ty);
			break;
		}

		s=BS2VM_ParseCommand(ctx, blk, s);
	}

	BS2VM_FinishBlock(ctx, blk);

	*str=s;
	return(blk);
}


BS2VM_Block *BS2VM_ParseBlock(BS2VM_Context *ctx, char **str)
{
	char b[64], b2[64];

	long long li, lj, lk;
	BS2VM_Block *blk;
	byte *ip;
	char *s, *t;
	int ty, ty2;
	int n, nlbl, nsym;
	int i, j, k, l, ni, nj;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);

	if(ty!=BS2VM_TOKEN_NAME)
		return(NULL);

	blk=BS2VM_LookupBlock(ctx, b);

	if(blk)
	{
		if(blk->body)
		{
			printf("Duplicate Block '%s'\n", b);
			return(NULL);
		}
	}else
	{
		blk=BS2VM_AllocBlock(ctx);
		blk->name=bs2vm_strdup(b);
		blk->bty=BS2VM_BTY_PROTO;
	}

	blk->lfn=bs2vm_lfn;
	blk->lln=bs2vm_lln;

	blk->rty=BS2VM_ParseType(ctx, &s);

	BS2VM_Parse_Token(s, b, &ty);
	if(!strcmp(b, "("))
		s=BS2VM_Parse_Token(s, b, &ty);

	while(1)
	{
		BS2VM_Parse_Token(s, b, &ty);

		if(!strcmp(b, ")"))break;

		if(ty!=BS2VM_TOKEN_NAME)break;
		if(!strcmp(b, "VARS"))break;
		if(!strcmp(b, "BODY"))break;
		if(!strcmp(b, "END"))break;

		if(!strcmp(b, "..."))
		{
			s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_VARARG;
			continue;
		}

		if(!strcmp(b, "$stdcall"))
			{ s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_STDCALL; continue; }
		if(!strcmp(b, "$cdecl"))
			{ s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_CDECL; continue; }
		if(!strcmp(b, "$win64"))
			{ s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_WIN64; continue; }
		if(!strcmp(b, "$amd64"))
			{ s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_AMD64; continue; }
		if(!strcmp(b, "$xcall"))
			{ s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_XCALL; continue; }

		if(!strcmp(b, "$inline"))
			{ s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_INLINE; continue; }
		if(!strcmp(b, "$proxy"))
			{ s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_PROXY; continue; }

		if(!strcmp(b, "$static"))
		{
			BS2VM_GetAliasTop(ctx, blk->name);
			s=BS2VM_Parse_Token(s, b, &ty);
			blk->flag|=BS2VM_BFL_STATIC;
			continue;
		}

		if(b[0]=='$') { s=BS2VM_Parse_Token(s, b, &ty); continue; }

		s=BS2VM_Parse_Token(s, b, &ty);
		i=BS2VM_ParseType(ctx, &s);

//		if(BS2VM_TypeArrP(ctx, i))
//			i=BS2VM_GetTypePtr(ctx, i);

		BS2VM_AddBlockArg(blk, b, i);
	}

	BS2VM_Parse_Token(s, b, &ty);

	if(!strcmp(b, "END"))
	{
		s=BS2VM_Parse_Token(s, b, &ty);
		*str=s;
		return(blk);
	}

	blk->bty=BS2VM_BTY_FUNC;

	if(!strcmp(b, "VARS") || !strcmp(b, ")"))
	{
		s=BS2VM_Parse_Token(s, b, &ty);

		while(1)
		{
			BS2VM_Parse_Token(s, b, &ty);

			if(!strcmp(b, "{"))break;

			if(ty!=BS2VM_TOKEN_NAME)break;
			if(!strcmp(b, "VARS"))break;
			if(!strcmp(b, "BODY"))break;
			if(!strcmp(b, "END"))break;

			s=BS2VM_Parse_Token(s, b, &ty);
			BS2VM_Parse_Token(s, b2, &ty2);

			if(!strcmp(b2, "~"))
			{
				s=BS2VM_Parse_Token(s, b2, &ty2);

				sprintf(b2, "%s.%s", blk->name, b);
				t=BS2VM_GetAliasTop(ctx, b2);

				t=BS2VM_Parse_TypeSig(ctx, &s);
				i=BS2VM_IndexTopVar(ctx, b2, t);

				continue;
			}

			i=BS2VM_ParseType(ctx, &s);
			BS2VM_AddBlockVar(blk, b, i);
		}
	}

	BS2VM_Parse_Token(s, b, &ty);
	if(!strcmp(b, "BODY") || !strcmp(b, "{"))
	{
		s=BS2VM_Parse_Token(s, b, &ty);

		while(1)
		{
			BS2VM_Parse_Token(s, b, &ty);
			if(!strcmp(b, "}"))break;
			if(!strcmp(b, "END"))break;

			s=BS2VM_ParseCommand(ctx, blk, s);
		}
	}

	BS2VM_FinishBlock(ctx, blk);

	BS2VM_Parse_Token(s, b, &ty);
	if(!strcmp(b, "END") || !strcmp(b, "}"))
		s=BS2VM_Parse_Token(s, b, &ty);

	*str=s;
	return(blk);
}


BS2VM_Block *BS2VM_ParseStruct(BS2VM_Context *ctx, char **str)
{
	char b[64], b2[64];

	BS2VM_Block *tmp, *st;
	char *s, *t;
	int ty, ty2;
	int i, j, n;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);

	if(ty!=BS2VM_TOKEN_NAME)
	{
		return(NULL);
	}

//	tmp=bs2vm_talloc("bs2vm_block_t", sizeof(BS2VM_Block));
//	memset(tmp, 0, sizeof(BS2VM_Block));

	tmp=BS2VM_AllocBlock(ctx);

	tmp->name=bs2vm_strdup(b);
	tmp->rty=-1;
	tmp->bty=BS2VM_BTY_STRUCT;

	tmp->lfn=bs2vm_lfn;
	tmp->lln=bs2vm_lln;

	if(*s==':')tmp->rty=BS2VM_ParseType(ctx, &s);

	BS2VM_Parse_Token(s, b, &ty);
	if(!strcmp(b, "{"))
		s=BS2VM_Parse_Token(s, b, &ty);

	while(1)
	{
		BS2VM_Parse_Token(s, b, &ty);

		if(!strcmp(b, "}"))break;

		if(ty!=BS2VM_TOKEN_NAME)break;
		if(!strcmp(b, "END"))break;

		s=BS2VM_Parse_Token(s, b, &ty);
		j=BS2VM_ParseType(ctx, &s);

		BS2VM_AddBlockVar(tmp, b, j);
	}

	BS2VM_Parse_Token(s, b, &ty);

	if(!strcmp(b, "END") || !strcmp(b, "}"))
	{
		s=BS2VM_Parse_Token(s, b, &ty);
		*str=s;
		return(tmp);
	}

	*str=s;
	return(tmp);
}

bs2vm_lit BS2VM_IndexVariant(BS2VM_Context *ctx, dyt val)
{
	return(0);
}

bs2vm_lit BS2VM_IndexInt(BS2VM_Context *ctx, dyt val)
{
	return(0);
}

bs2vm_lit BS2VM_ParseVarValue(BS2VM_Context *ctx, char **str, int vt)
{
	return(0);

#if 0
	char b[64], b2[64];
	char *s, *t;
	dyt p, q, r, o, lst;
	int ty, ty2;
	int i, j, k;

	s=*str;

	if(BS2VM_TypeArrP(ctx, vt))
	{
		j=BS2VM_GetArrayStepCount(ctx, vt);
		k=BS2VM_GetArrayStepType(ctx, vt);

		if(!j)j=1<<30;

		lst=NULL;
		for(i=0; i<j; i++)
		{
			BS2VM_Parse_Token(s, b, &ty);
			if(!strcmp(b, "END"))
			{
//				s=BS2VM_Parse_Token(s, b, &ty);
				break;
			}

			p=BS2VM_ParseVarValue(ctx, &s, k);
			lst=dycons(p, lst);
		}

		BS2VM_Parse_Token(s, b, &ty);
		if(!strcmp(b, "END"))
		{
			s=BS2VM_Parse_Token(s, b, &ty);
//			break;
		}

		p=dynreverse(lst);
		*str=s;
		return(BS2VM_IndexVariant(ctx, p));
	}


	if(BS2VM_TypePtrP(ctx, vt))
	{
		s=BS2VM_Parse_Token(s, b, &ty);	//<value>

		if(ty==BS2VM_TOKEN_NAME)
		{
			*str=s;
			return(BS2VM_IndexVariant(ctx, dysymbol(b)));
		}

		if(ty==BS2VM_TOKEN_NUMBER)
		{
			*str=s;
			return(BS2VM_IndexVariant(ctx, dyint(bs2vm_atoi(b))));
		}

		if(ty==BS2VM_TOKEN_STRING)
		{
			*str=s;
			return(BS2VM_IndexVariant(ctx, dystring(b)));
		}

		*str=s;
		return(BS2VM_IndexVariant(ctx, NULL));
	}

	if(!BS2VM_TypeBasicP(ctx, vt))
	{
		BS2VM_Error("VAR is invalid for composite types %X\n", vt);
		*str=s;
		return(BS2VM_IndexVariant(ctx, NULL));
	}

	switch(vt&BS2VM_T_TY_MASK)
	{
		case BS2VM_T_VEC2:
			p=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			q=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			*str=s; return(BS2VM_IndexVariant(ctx, dylist2(p, q))); break;
		case BS2VM_T_VEC3:
			p=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			q=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			r=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			*str=s; return(BS2VM_IndexVariant(ctx, dylist3(p, q, r))); break;
		case BS2VM_T_VEC4:
			o=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			p=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			q=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			r=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_FLOAT);
			*str=s; return(BS2VM_IndexVariant(ctx, dylist4(o, p, q, r))); break;
		case BS2VM_T_M2F:
			p=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC2);
			q=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC2);
			*str=s; return(BS2VM_IndexVariant(ctx, dylist2(p, q))); break;
		case BS2VM_T_M3F:
			p=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC3);
			q=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC3);
			r=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC3);
			*str=s; return(BS2VM_IndexVariant(ctx, dylist3(p, q, r))); break;
		case BS2VM_T_M4F:
			o=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC4);
			p=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC4);
			q=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC4);
			r=BS2VM_ParseVarValue(ctx, &s, BS2VM_T_VEC4);
			*str=s; return(BS2VM_IndexVariant(ctx, dylist4(o, p, q, r))); break;
		default: break;
	}

	s=BS2VM_Parse_Token(s, b, &ty);	//<value>
	switch(vt&BS2VM_T_TY_MASK)
	{
	case BS2VM_T_INT:
	case BS2VM_T_UINT:
	case BS2VM_T_LONG:
	case BS2VM_T_ULONG:
	case BS2VM_T_POINTER:
		if(ty==BS2VM_TOKEN_NAME)
			return(BS2VM_IndexVariant(ctx, dysymbol(b)));
		if(ty==BS2VM_TOKEN_NUMBER)
			return(BS2VM_IndexVariant(ctx, dyint(bs2vm_atoi(b))));
		BS2VM_Error("VAR: bad initializer\n");
		break;

	case BS2VM_T_CHAR:
	case BS2VM_T_BYTE:
	case BS2VM_T_SHORT:
	case BS2VM_T_WORD:
		if(ty==BS2VM_TOKEN_NUMBER)
			return(BS2VM_IndexVariant(ctx, dyint(bs2vm_atoi(b))));
		BS2VM_Error("VAR: bad initializer\n");
		break;

	case BS2VM_T_FLOAT:
		if(ty==BS2VM_TOKEN_NUMBER)
			return(BS2VM_IndexVariant(ctx, dydouble(atof(b))));
		BS2VM_Error("VAR: bad initializer\n");
		break;
	case BS2VM_T_DOUBLE:
		if(ty==BS2VM_TOKEN_NUMBER)
			return(BS2VM_IndexVariant(ctx, dydouble(atof(b))));
		BS2VM_Error("VAR: bad initializer\n");
		break;

	default:
		BS2VM_Error("VAR: handled var type %X\n", vt);
		break;
	}

	*str=s;
	return(BS2VM_IndexVariant(ctx, NULL));
#endif
}

void BS2VM_ParseVar(BS2VM_Context *ctx, char **str)
{
	char b[64], b2[64];
	char *s, *t, *vn;
	dyt p;
	int ty, ty2, vt, al, sz;
	int i, j, fl;

	s=*str;

	vn=NULL; fl=0;
	BS2VM_Parse_Token(s, b, &ty);
	while(b[0]=='$')
	{
		s=BS2VM_Parse_Token(s, b, &ty);
		if(!strcmp(b, "$static"))
			fl|=BS2VM_BFL_STATIC;
		if(!strcmp(b, "$thread"))
			fl|=BS2VM_BFL_THREAD;
	}

	s=BS2VM_Parse_Token(s, b, &ty);
	t=BS2VM_Parse_TypeSig(ctx, &s);
	i=BS2VM_IndexTopVar(ctx, b, t);
	ctx->top->var[i]->flag=fl;

	vn=ctx->top->var[i]->name;
	vt=ctx->top->var[i]->type;

	if(!vt)
	{
		BS2VM_Error("VAR declaration problem '%s'\n", ctx->top->var[i]->name);
		return;
	}

	BS2VM_Parse_Token(s, b, &ty);
	if(!strcmp(b, "="))
	{
		s=BS2VM_Parse_Token(s, b, &ty);	//'='
		ctx->top->var[i]->lit=BS2VM_ParseVarValue(ctx, &s, vt);
	}

	*str=s;
	return;
}

void BS2VM_ParseArr(BS2VM_Context *ctx, char **str)
{
	char b[64], b2[64];
	char *s, *t, *vn;
	dyt p, lst;
	int ty, ty2, vt, al, sz;
	int i, j;

	s=*str;

	vn=NULL;
	BS2VM_Parse_Token(s, b, &ty);
	while(b[0]=='$')
	{
		s=BS2VM_Parse_Token(s, b, &ty);
		if(!strcmp(b, "$static"))vn="";
	}

	s=BS2VM_Parse_Token(s, b, &ty);
	t=BS2VM_Parse_TypeSig(ctx, &s);
	i=BS2VM_IndexTopVar(ctx, b, t);

	ctx->top->var[i]->flag=0;
	if(vn)ctx->top->var[i]->flag|=BS2VM_BFL_STATIC;

	vn=ctx->top->var[i]->name;
	vt=ctx->top->var[i]->type;

//	vt=BS2VM_GetArrayStepType(ctx, vt);
//	BS2VM_SizeType(ctx, vt, &al, &sz);

	lst=NULL;
	while(*s)
	{
		BS2VM_Parse_Token(s, b, &ty);
		if(!strcmp(b, "END"))
		{
			s=BS2VM_Parse_Token(s, b, &ty);
			break;
		}

//		p=BS2VM_ParseVarValue(ctx, &s, vt);
//		lst=dycons(p, lst);
	}
//	lst=dynreverse(lst);

	ctx->top->var[i]->lit=BS2VM_IndexVariant(ctx, lst);

	*str=s;
	return;
}

void BS2VM_ParseExtern(BS2VM_Context *ctx, char **str)
{
	char b[64];
	char *s, *t;
	int ty;
	int i, j;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);

//	j=BS2VM_ParseType(ctx, &s);
	t=BS2VM_Parse_TypeSig(ctx, &s);
	i=BS2VM_IndexTopVar(ctx, b, t);
	ctx->top->var[i]->flag|=BS2VM_VFL_EXTERN;

	*str=s;
}

void BS2VM_ParseClassExtern(BS2VM_Context *ctx, char **str)
{
	char b[64];
	BS2VM_Block *blk;
	char *s, *t;
	int ty;
	int i, j;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);
//	blk=BS2VM_ImportClass(ctx, b);
//	blk->next=ctx->top->block;
//	ctx->top->block=blk;
	*str=s;
}

void BS2VM_ParseBinOp(BS2VM_Context *ctx, char **str)
{
	char b[64], b2[64];
	BS2VM_OperatorInfo *tmp;
	char *s, *t, *vn;
	int ty, ty2, vt, vs, al, sz;
	int i, j;

	s=*str;

	tmp=bs2vm_cmalloc(ctx, sizeof(BS2VM_OperatorInfo));
	memset(tmp, 0, sizeof(BS2VM_OperatorInfo));

	s=BS2VM_Parse_Token(s, b, &ty);
	tmp->fname=bs2vm_strdup(b);

	tmp->lty=BS2VM_ParseType(ctx, &s);
	tmp->rty=BS2VM_ParseType(ctx, &s);

	s=BS2VM_Parse_Token(s, b, &ty);
	tmp->op=BS2VM_LookupOpcode2(b);

	tmp->next=ctx->top->opr;
	ctx->top->opr=tmp;

	*str=s;
}

void BS2VM_ParseUnOp(BS2VM_Context *ctx, char **str)
{
	char b[64], b2[64];
	BS2VM_OperatorInfo *tmp;
	char *s, *t, *vn;
	int ty, ty2, vt, vs, al, sz;
	int i, j;

	s=*str;

	tmp=bs2vm_cmalloc(ctx, sizeof(BS2VM_OperatorInfo));
	memset(tmp, 0, sizeof(BS2VM_OperatorInfo));

	s=BS2VM_Parse_Token(s, b, &ty);
	tmp->fname=bs2vm_strdup(b);
	tmp->lty=-1;
	tmp->rty=BS2VM_ParseType(ctx, &s);

	s=BS2VM_Parse_Token(s, b, &ty);
	tmp->op=BS2VM_LookupOpcode2(b);

	tmp->next=ctx->top->opr;
	ctx->top->opr=tmp;

	*str=s;
}

void BS2VM_ParseFile(BS2VM_Context *ctx, char **str)
{
	char b[64];
	char *s, *t;
	int ty;
	int i, j;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);
	bs2vm_lfn=bs2vm_strdup(b);
	bs2vm_lln=0;
	*str=s;
}

void BS2VM_ParseLine(BS2VM_Context *ctx, char **str)
{
	char b[64];
	char *s, *t;
	int ty;
	int i, j;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);
	bs2vm_lln=atoi(b);
	*str=s;
}

void BS2VM_ParseFLine(BS2VM_Context *ctx, char **str)
{
	char b[64];
	char *s, *t;
	int ty;
	int i, j;

	s=*str;
	s=BS2VM_Parse_Token(s, b, &ty);
	bs2vm_lfn=bs2vm_strdup(b);
	s=BS2VM_Parse_Token(s, b, &ty);
	bs2vm_lln=atoi(b);
	*str=s;
}

#if 0
BGBBS2VM_API void BS2VM_CompileOutput(BS2VM_Context *ctx, char *outfn)
{
	BS2VM_Block *blk, *lblk;
	BS2VM_Top *top;
	char *s, *t;
	int ty, ty2;
	int i, j, k;


	top=ctx->top;

	blk=top->block;
	while(blk)
	{
		if(blk->bty==BS2VM_BTY_STRUCT)
			BS2VM_SizeStruct(ctx, blk);
		if(blk->bty==BS2VM_BTY_UNION)
			BS2VM_SizeUnion(ctx, blk);

		blk=blk->next;
	}

	blk=top->block;
	while(blk)
	{
		if(blk->bty==BS2VM_BTY_FUNC)
			BS2VM_SetupFuncArgs(ctx, blk);
		if(blk->bty==BS2VM_BTY_PROTO)
			BS2VM_SetupFuncArgs(ctx, blk);

		blk=blk->next;
	}

	sxasm_print(ctx, "\n\n[section .text]\n");
	sxasm_print(ctx, "[bits %d]\n", ctx->x64p?64:32);

	blk=top->block; lblk=NULL;
	while(blk)
	{
		blk->chain=lblk;
		lblk=blk;
		blk=blk->next;
	}

	blk=lblk;
	while(blk)
	{
		ctx->ch_blk=lblk;
		BS2VM_TransBlock(ctx, blk);
		blk=blk->chain;
	}

//	BS2VM_EmitClassThunks(ctx);

	sxasm_print(ctx, "\n\n[section .data]\n");
	sxasm_print(ctx, "[bits %d]\n", ctx->x64p?64:32);

#if 0
	for(i=0; i<top->nslit; i++)
	{
		sxasm_print(ctx, "%s db %s, 0\n",
			top->slitn[i], BS2VM_Parse_CIfy(top->slitv[i]));
	}

	sxasm_print(ctx, "align 16\n");

	for(i=0; i<top->nxlit; i++)
		sxasm_print(ctx, "%s dd 0x%08X, 0x%08X, 0x%08X, 0x%08X\n",
			top->xlitn[i],
			top->xlitv[i*4+0], top->xlitv[i*4+1],
			top->xlitv[i*4+2], top->xlitv[i*4+3]);

	for(i=0; i<top->ndlit; i++)
		sxasm_print(ctx, "%s dq %f\n",
			top->dlitn[i], top->dlitv[i]);

	for(i=0; i<top->nllit; i++)
		sxasm_print(ctx, "%s dq %ld\n",
			top->llitn[i], top->llitv[i]);

	for(i=0; i<top->nflit; i++)
		sxasm_print(ctx, "%s dd %f\n",
			top->flitn[i], top->flitv[i]);

	for(i=0; i<top->nvar; i++)
	{
		if(top->var[i]->val==UNDEFINED)
			continue;
		if(top->var[i]->val==NULL)
			continue;

		sxasm_print(ctx, "%s ", top->var[i]->name);
		BS2VM_EmitVarValue(ctx, top->var[i]->type, top->var[i]->val);
	}

	sxasm_print(ctx, "\n\n[section .bss]\n");
	sxasm_print(ctx, "[bits %d]\n", ctx->x64p?64:32);

	for(i=0; i<top->nvar; i++)
	{
		if(top->var[i]->val!=NULL)
			continue;
		if(ctx->top->var[i]->flag&BS2VM_VFL_EXTERN)
			continue;

		BS2VM_SizeType(ctx, ctx->top->var[i]->type, &j, &k);

		s=BS2VM_AliasName(ctx, ctx->top->var[i]->name);

		sxasm_print(ctx, "align %d\n", j);
//		sxasm_print(ctx, "%s ", top->var[i]->name);
		sxasm_print(ctx, "%s ", s);
		sxasm_print(ctx, "resb %d\n", k);

//		BS2VM_EmitVarValue(ctx, top->var[i]->type, top->var[i]->val);
	}

#endif

//	bs2vm_checkalc();
}
#endif

// void BS2VM_ProcessBuffer(BS2VM_Top *top, char *buf, char *outfn)
BGBBS2VM_API void BS2VM_ProcessBuffer(BS2VM_Context *ctx,
	char *buf, char *outfn)
{
	char b[64], b2[64];
	BS2VM_Block *blk, *lblk;
	BS2VM_Block *st;
//	BS2VM_Context *ctx;
	BS2VM_Top *top;
	char *s, *t;
	int ty, ty2;
	int i, j, k;

//	bs2vm_init_heap();

//	ctx=BS2VM_AllocContext();
//	ctx->top=top;

	top=ctx->top;

	bs2vm_lfn="<?>";
	bs2vm_lln=0;

	s=buf;
	while(*s)
	{
		s=BS2VM_Parse_Token(s, b, &ty);

		if(!strcmp(b, "BLOCK") ||
			!strcmp(b, "FUNC"))
		{
			BS2VM_ParseBlock(ctx, &s);
			continue;
		}

		if(!strcmp(b, "PROC"))
		{
			BS2VM_ParseProc(ctx, &s);
			continue;
		}

		if(!strcmp(b, "PROTO") ||
			!strcmp(b, "S_PROTO"))
		{
			BS2VM_ParseBlock(ctx, &s);
			continue;
		}

		if(!strcmp(b, "STRUCT"))
		{
			st=BS2VM_ParseStruct(ctx, &s);
			st->bty=BS2VM_BTY_STRUCT;
			continue;
		}

		if(!strcmp(b, "UNION"))
		{
			st=BS2VM_ParseStruct(ctx, &s);
			st->bty=BS2VM_BTY_UNION;
			continue;
		}


		if(!strcmp(b, "EXTERN"))
		{
			BS2VM_ParseExtern(ctx, &s);
			continue;
		}

		if(!strcmp(b, "VAR"))
		{
			BS2VM_ParseVar(ctx, &s);
			continue;
		}

		if(!strcmp(b, "ARR"))
		{
			BS2VM_ParseArr(ctx, &s);
			continue;
		}

		if(!strcmp(b, "BINARYOP"))
		{
			BS2VM_ParseBinOp(ctx, &s);
			continue;
		}
		if(!strcmp(b, "UNARYOP"))
		{
			BS2VM_ParseUnOp(ctx, &s);
			continue;
		}

		if(!strcmp(b, "CLASSEXTERN"))
		{
			BS2VM_ParseClassExtern(ctx, &s);
			continue;
		}

		if(!strcmp(b, "FILE")) { BS2VM_ParseFile(ctx, &s); continue; }
		if(!strcmp(b, "LINE")) { BS2VM_ParseLine(ctx, &s); continue; }
		if(!strcmp(b, "FLINE")) { BS2VM_ParseFLine(ctx, &s); continue; }
	}

//	BS2VM_CompileOutput(ctx, outfn);
}
