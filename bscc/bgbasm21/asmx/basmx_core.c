#include <bgbasmx.h>

#if 0
extern char *basm_ops[];
extern int basm_opidx[];

extern int basm_opnums[];
extern char basm_opcat[];
extern char *basm_opdecl[];
#endif

#if 1
extern char **basm_ops;
extern int *basm_opidx;
extern int *basm_opnums;

extern char *basm_opcat;
extern char **basm_opdecl;

extern char **basm_regs;
extern int *basm_regidx;
#endif


BASMX_API char *BASMX_ParseOpcodeI(BASM_Context *ctx,
	char *str, BASMX_Opcode *dop)
{
	char b[64], b2[64];
	char *s, *t, *l0, *l1, *l2;
	int ty, ty2;
	int i, j, opnum;

	long long disp0, disp1, disp2;
	int br0, ir0, sc0;
	int br1, ir1, sc1;
	int br2, ir2, sc2;

	b[0]=0; b2[0]=0;

	s=BASM_Parse_EatWhite(str);
	if(!(*s))return(s);

	if(*s==';')
	{
		s++;
		s=BASM_Parse_EatWhite(str);
	}

	s=BASM_Parse_Token(s, b, &ty);
	BASM_Parse_Token(s, b2, &ty2);

	if(!b[0])return(s);

	if(!strcmp(b, "["))
	{
		dop->opnum=BASMX_OP_NOP;

		s=BASM_Parse_Token(s, b, &ty);
		t=BASM_ParseSpecial(ctx, s, b);
		if(t)
		{
			s=t;
			BASM_Parse_Token(s, b, &ty);
			if(!strcmp(b, "]"))
				s=BASM_Parse_Token(s, b, &ty);
			return(s);
		}

		//fail, seek to next ']'

		while(*s && (*s!=']'))s++;
		if(*s==']')s++;
		return(s);
	}

	if((ty==BASM_TOKEN_NAME) &&
		(!basm_stricmp(b2, ":") || !basm_stricmp(b2, "db") ||
		!basm_stricmp(b2, "dw") || !basm_stricmp(b2, "dd") ||
		!basm_stricmp(b2, "dq") || !basm_stricmp(b2, "resb") ||
		!basm_stricmp(b2, "resw") || !basm_stricmp(b2, "resd") ||
		!basm_stricmp(b2, "resq") ))
	{
		if(!basm_stricmp(b2, ":"))
			s=BASM_Parse_Token(s, b2, &ty2);

		if(b[0]=='.')
		{
			if(!ctx->label_base)
			{
				printf("BASMX_ParseOpcode: "
					"local label '%s' absent base\n", b);
				return(s);
			}

			sprintf(b2, "%s%s", ctx->label_base, b);

			dop->opnum=BASMX_OP_LABEL;
			dop->narg=1;
			dop->arg[0]->lbl=basm_strdup(b2);

			return(s);
		}else
		{
			ctx->label_base=basm_strdup(b);
		}

		dop->opnum=BASMX_OP_LABEL;
		dop->narg=1;
		dop->arg[0]->lbl=basm_strdup(b);
		return(s);
	}

#if 1
	if((ty==BASM_TOKEN_NAME) && !basm_stricmp(b2, "equ") )
	{
		s=BASM_Parse_Token(s, b2, &ty2);		//equ
		s=BASM_Parse_Token(s, b2, &ty2);		//value

		i=1;
		if(!basm_stricmp(b2, "-"))
		{
			i=-1;
			s=BASM_Parse_Token(s, b2, &ty2);		//value
		}
			

		dop->opnum=BASMX_OP_CONST;
		dop->narg=1;
		dop->arg[0]->lbl=basm_strdup(b);
		dop->arg[0]->disp=i*basm_atoi(b2);

		return(s);
	}
#endif

	if(ty!=BASM_TOKEN_NAME)
	{
		printf("BASMX_ParseOpcode: Bad Opcode Token '%s'\n", b);
		return(s);
	}

	i=BASM_LookupOpcode(ctx, b);

//	t=BASM_ParseSpecial(ctx, s, b);
//	if(t)return(t);

	dop->opnum=i;
	dop->width=0;
	dop->narg=0;
	dop->seg=BASM_Z;
	dop->flag=0;

	if(!basm_stricmp(b, "a16"))
	{
		if(!(ctx->fl&2))dop->flag|=BASM_FL_ADDRSZ;

		s=BASM_Parse_Token(s, b, &ty);
		BASM_Parse_Token(s, b2, &ty2);
		i=BASM_LookupOpcode(ctx, b);
		dop->opnum=i;
	}

	if(!basm_stricmp(b, "a32"))
	{
		if(ctx->fl&2)dop->flag|=BASM_FL_ADDRSZ;

		s=BASM_Parse_Token(s, b, &ty);
		BASM_Parse_Token(s, b2, &ty2);
		i=BASM_LookupOpcode(ctx, b);
		dop->opnum=i;
	}

	if(!basm_ops[i])
	{
		j=b[strlen(b)-1];
		if((j=='b') || (j=='w') || (j=='l'))
		{
			ctx->fl|=BASM_FL_GAS;
			if(j=='b')dop->width=8;
			if(j=='w')dop->width=16;
			if(j=='l')dop->width=32;
			b[j]=0;

			i=BASM_LookupOpcode(ctx, b);

		}else if(b[0]=='l')
		{
			ctx->fl|=BASM_FL_GAS; dop->width=32;

			i=BASM_LookupOpcode(ctx, b+1);
		}
	}

	if((i<0) || !basm_ops[i])
	{
		printf("BASMX_ParseOpcode: Bad Opcode '%s'\n", b);
		return(s);
	}

	dop->opnum=i;

	if(BASM_OpOnlySingleP(i))
	{
//		BASM_OutOpSingle(ctx, i);
		if(*s==';')s++;
		return(s);
	}

	if((*s==';') || BASM_Parse_IsLineBreak(s))
	{
//		BASM_OutOpSingle(ctx, i);
		if(*s==';')s++;
		return(s);
	}

	BASM_Parse_Token(s, b2, &ty2);


	if(!basm_stricmp(b2, "byte")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=8; }
	if(!basm_stricmp(b2, "word")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=16; }
	if(!basm_stricmp(b2, "dword")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=32; }
	if(!basm_stricmp(b2, "qword")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=64; }
	if(!basm_stricmp(b2, "oword")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=128; }
	if(!basm_stricmp(b2, "yword")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=256; }
	if(!basm_stricmp(b2, "tword")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=80; }

	if(!basm_stricmp(b2, "dqword")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=128; }

	if(!basm_stricmp(b2, "short")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=8; }
	if(!basm_stricmp(b2, "near16")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=16; }
	if(!basm_stricmp(b2, "near32")) { s=BASM_Parse_Token(s, b2, &ty2); dop->width=32; }

	BASM_Parse_Token(s, b2, &ty2);

	if(!basm_stricmp(b2, "ptr")) s=BASM_Parse_Token(s, b2, &ty2);
	if(!basm_stricmp(b2, "offset")) s=BASM_Parse_Token(s, b2, &ty2);

	BASM_Parse_Token(s, b2, &ty2);

	s=BASM_ParseArg(ctx, s, &br0, &ir0, &sc0, &disp0, &l0);
	if(l0 && (*l0=='.') && ctx->label_base)
		{ sprintf(b2, "%s%s", ctx->label_base, l0);
		  l0=basm_strdup(b2); }
	BASM_Parse_Token(s, b2, &ty2);

	if((*s==';') || BASM_Parse_IsLineBreak(s))
	{
//		BASM_OutOpGeneric1(ctx, i, dop->width, l0, br0, ir0, sc0, disp0);

		dop->arg[0]->lbl=l0;
		dop->arg[0]->breg=br0;
		dop->arg[0]->ireg=ir0;
		dop->arg[0]->sc=sc0;
		dop->arg[0]->disp=disp0;

		dop->narg=1;

		BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

	if(!strcmp(b2, ","))s=BASM_Parse_Token(s, b2, &ty2);

	s=BASM_ParseArg(ctx, s, &br1, &ir1, &sc1, &disp1, &l1);
	if(l1 && (*l1=='.') && ctx->label_base)
		{ sprintf(b2, "%s%s", ctx->label_base, l1);
		  l1=basm_strdup(b2); }
	BASM_Parse_Token(s, b2, &ty2);

	if((*s==';') || BASM_Parse_IsLineBreak(s))
	{
//		BASM_OutOpGeneric2(ctx, i, dop->width,
//			l0, br0, ir0, sc0, disp0,
//			l1, br1, ir1, sc1, disp1);

		dop->arg[0]->lbl=l0;
		dop->arg[0]->breg=br0;
		dop->arg[0]->ireg=ir0;
		dop->arg[0]->sc=sc0;
		dop->arg[0]->disp=disp0;

		dop->arg[1]->lbl=l1;
		dop->arg[1]->breg=br1;
		dop->arg[1]->ireg=ir1;
		dop->arg[1]->sc=sc1;
		dop->arg[1]->disp=disp1;

		dop->narg=2;

		BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

	BASM_Parse_Token(s, b2, &ty2);
	if(!basm_stricmp(b2, ","))s=BASM_Parse_Token(s, b2, &ty2);

	s=BASM_ParseArg(ctx, s, &br2, &ir2, &sc2, &disp2, &l2);
	if(l2 && (*l2=='.') && ctx->label_base)
		{ sprintf(b2, "%s%s", ctx->label_base, l2);
		  l2=basm_strdup(b2); }
	BASM_Parse_Token(s, b2, &ty2);

	if((*s==';') || BASM_Parse_IsLineBreak(s))
	{
//		BASM_OutOpGeneric3(ctx, i, dop->width,
//			l0, br0, ir0, sc0, disp0,
//			l1, br1, ir1, sc1, disp1,
//			l2, br2, ir2, sc2, disp2);

		dop->arg[0]->lbl=l0;
		dop->arg[0]->breg=br0;
		dop->arg[0]->ireg=ir0;
		dop->arg[0]->sc=sc0;
		dop->arg[0]->disp=disp0;

		dop->arg[1]->lbl=l1;
		dop->arg[1]->breg=br1;
		dop->arg[1]->ireg=ir1;
		dop->arg[1]->sc=sc1;
		dop->arg[1]->disp=disp1;

		dop->arg[2]->lbl=l2;
		dop->arg[2]->breg=br2;
		dop->arg[2]->ireg=ir2;
		dop->arg[2]->sc=sc2;
		dop->arg[2]->disp=disp2;

		dop->narg=3;

		BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

	printf("BASMX_ParseOpcode: Opcode Parse Error\n");

//	BASM_Parse_Token(s, b2, &ty2);
	if(*s==';')s++;
	return(s);
}

BASMX_API BASMX_OpArg *BASMX_CloneOpArgMalloc(
	BASM_Context *ctx, BASMX_OpArg *op)
{
	BASMX_OpArg *tmp;

	tmp=malloc(sizeof(BASMX_OpArg));
	memset(tmp, 0, sizeof(BASMX_OpArg));

	tmp->breg=op->breg;
	tmp->ireg=op->ireg;
	tmp->sc=op->sc;
	tmp->disp=op->disp;
	tmp->lbl=op->lbl;

	return(tmp);
}

BASMX_API BASMX_Opcode *BASMX_CloneOpcodeMalloc(
	BASM_Context *ctx, BASMX_Opcode *op)
{
	BASMX_Opcode *tmp;
	int i;

	tmp=malloc(sizeof(BASMX_Opcode));
	memset(tmp, 0, sizeof(BASMX_Opcode));

	tmp->opnum=op->opnum;
	tmp->width=op->width;
	tmp->narg=op->narg;
	tmp->seg=op->seg;
	tmp->flag=op->flag;

	tmp->arg=malloc((tmp->narg+1)*sizeof(BASMX_OpArg *));
	for(i=0; i<tmp->narg; i++)
		tmp->arg[i]=BASMX_CloneOpArgMalloc(ctx, op->arg[i]);
	tmp->arg[tmp->narg]=NULL;

	return(tmp);
}

BASMX_API BASMX_Opcode *BASMX_ParseOpcodeMalloc(
	BASM_Context *ctx, char **str)
{
	BASMX_OpArg farg0, farg1, farg2, farg3;
	BASMX_OpArg *targ[8];
	BASMX_Opcode fop;
	BASMX_Opcode *top;
	BASMX_Opcode *op;
	char *s, *t;

	targ[0]=&farg0; targ[1]=&farg1;
	targ[2]=&farg2; targ[3]=&farg3;

	memset(targ[0], 0, sizeof(BASMX_OpArg));
	memset(targ[1], 0, sizeof(BASMX_OpArg));
	memset(targ[2], 0, sizeof(BASMX_OpArg));
	memset(targ[3], 0, sizeof(BASMX_OpArg));

	top=&fop;
	memset(top, 0, sizeof(BASMX_Opcode));

	top->arg=targ;

	s=BASM_Parse_EatWhite(*str);
	if(!(*s))return(NULL);

	t=BASMX_ParseOpcodeI(ctx, s, top);
	if(t==s)return(NULL);

	*str=t;
	op=BASMX_CloneOpcodeMalloc(ctx, top);
	return(op);
}

BASMX_API BASMX_Opcode **BASMX_ParseOpcodeListMalloc(
	BASM_Context *ctx, char **str)
{
	BASMX_Opcode **tarr;
	BASMX_Opcode *dop;
	char *s;
	int i, j;

	tarr=malloc(16384*sizeof(BASMX_Opcode *));
	memset(tarr, 0, 16384*sizeof(BASMX_Opcode *));

	s=*str; i=0;
	while(1)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;

		if((*s==';') || (*s=='#'))
		{
			ctx->seg=0;
			while(*s && (*s!='\n'))s++;
			if(*s=='\n')ctx->seg=BASM_Z;
			continue;
		}

		dop=BASMX_ParseOpcodeMalloc(ctx, &s);
		if(!dop)break;
		tarr[i++]=dop;
	}
	tarr[i]=NULL;
	*str=s;

//	printf("BASMX_ParseOpcodeListMalloc: Parsed %d ops\n", i);
//	for(j=0; j<i; j++)
//	{
//		printf("%d: %d <%d>\n", j, tarr[j]->opnum, tarr[j]->width);
//	}


	tarr=realloc(tarr, (i+1)*sizeof(BASMX_Opcode *));
	return(tarr);
}

BASMX_API void BASMX_OutOpcode(BASM_Context *ctx, BASMX_Opcode *op)
{
	BASMX_OpArg *arg0, *arg1, *arg2, *arg3;

	switch(op->opnum)
	{
	case BASMX_OP_NOP:
		return;
		break;
	case BASMX_OP_LABEL:
		BASM_EmitLabel(ctx, op->arg[0]->lbl);
		return;
		break;
	case BASMX_OP_CONST:
		BASM_EmitConst(ctx, op->arg[0]->lbl, op->arg[0]->disp);
		return;
		break;
	}

	switch(op->narg)
	{
	case 0: BASM_OutOpSingle(ctx, op->opnum); break;

	case 1:
		arg0=op->arg[0];
		BASM_OutOpGeneric1(ctx, op->opnum, op->width,
		  arg0->lbl, arg0->breg, arg0->ireg, arg0->sc, arg0->disp);
		break;
	case 2:
		arg0=op->arg[0];
		arg1=op->arg[1];
		BASM_OutOpGeneric2(ctx, op->opnum, op->width,
		  arg0->lbl, arg0->breg, arg0->ireg, arg0->sc, arg0->disp,
		  arg1->lbl, arg1->breg, arg1->ireg, arg1->sc, arg1->disp);
		break;
	case 3:
		arg0=op->arg[0];
		arg1=op->arg[1];
		arg2=op->arg[2];
		BASM_OutOpGeneric3(ctx, op->opnum, op->width,
		  arg0->lbl, arg0->breg, arg0->ireg, arg0->sc, arg0->disp,
		  arg1->lbl, arg1->breg, arg1->ireg, arg1->sc, arg1->disp,
		  arg2->lbl, arg2->breg, arg2->ireg, arg2->sc, arg2->disp);
		break;
	default: break;
	}
}

BASMX_API void BASMX_OutOpcodeList(BASM_Context *ctx, BASMX_Opcode **ops)
{
	int i;

	for(i=0; ops[i]; i++)
	{
		BASMX_OutOpcode(ctx, ops[i]);
	}
}

BASMX_API void BASMX_BeginAssembly(BASM_Context *ctx, char *name)
{
	long i, j;

	BASM_Init();
	BASM_ResetCtx(ctx);

	ctx->modname=basm_strdup(name);
}

BASMX_API void *BASMX_EndAssembly(BASM_Context *ctx)
{
	byte *ip, *obj;
	int sz;

	obj=BASM_COFF_EncodeObject(ctx, &sz);
	ip=BLNK_LinkModuleObj(NULL, obj, sz);
	free(obj);

	return((void *)ip);
}

BASMX_API byte *BASMX_EndAssemblyObjBuf(BASM_Context *ctx, int *rsz)
	{ return(BASM_COFF_EncodeObject(ctx, rsz)); }

BASMX_API byte *BASMX_EndAssemblyCacheObjBuf(BASM_Context *ctx, int *rsz)
{
	byte *obj;
	int sz;

	obj=BASM_COFF_EncodeObject(ctx, &sz);
	BLNK_LinkModuleObj(NULL, obj, sz);
	return(obj);
}

BASMX_API void BASMX_EndAssemblyQueue(BASM_Context *ctx)
{
	byte *ip, *obj;
	int sz;

	obj=BASM_COFF_EncodeObject(ctx, &sz);
	BLNK_LinkModuleObjQueue(NULL, obj, sz);
	free(obj);
}
