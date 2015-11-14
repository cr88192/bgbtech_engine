/*
Written 2007-2010 by Brendan G Bohannon
This code is re-assigned into the public domain.
*/

#include <bs2asm.h>

extern int bsvm_ops_nops;
extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];
extern char *bsvm_ops_sfis[];

int bs2vm_ophash[4096];

BGBSVM_API int BS2ASM_LookupOpcode(char *name)
{
	char *s;
	int i, hi;

	s=name; hi=0;
	while(*s)hi=(hi*4093)+(*s++);
	hi*=4093;
	hi=(hi>>12)&4095;

	i=bs2vm_ophash[hi];
	if((i>=0) && (i<bsvm_ops_nops))
		if(!stricmp(bsvm_ops_strs[i], name))
			return(i);

	for(i=0; i<bsvm_ops_nops; i++)
		if(!stricmp(bsvm_ops_strs[i], name))
	{
		bs2vm_ophash[hi]=i;
		return(i);
	}
	return(-1);
}

BGBSVM_API int BS2ASM_LookupOpcode2(char *name)
{
	int i;

	i=-1;
	if(!strcmp(name, "+"))i=BSVM_SOP_ADD;
	if(!strcmp(name, "-"))i=BSVM_SOP_SUB;
	if(!strcmp(name, "*"))i=BSVM_SOP_MUL;
	if(!strcmp(name, "/"))i=BSVM_SOP_DIV;
	if(!strcmp(name, "%"))i=BSVM_SOP_MOD;
	if(!strcmp(name, "&"))i=BSVM_SOP_AND;
	if(!strcmp(name, "|"))i=BSVM_SOP_OR;
	if(!strcmp(name, "^"))i=BSVM_SOP_XOR;

	if(!strcmp(name, "<<"))i=BSVM_SOP_SHL;
	if(!strcmp(name, ">>"))i=BSVM_SOP_SHR;

	if(!strcmp(name, "++"))i=BSVM_SOP_INC;
	if(!strcmp(name, "--"))i=BSVM_SOP_DEC;
	if(!strcmp(name, "+++"))i=BSVM_SOP_INC2;
	if(!strcmp(name, "---"))i=BSVM_SOP_DEC2;

	if(!strcmp(name, "=="))i=BSVM_SOP_CMP_EQ;
	if(!strcmp(name, "!="))i=BSVM_SOP_CMP_NE;
	if(!strcmp(name, "<"))i=BSVM_SOP_CMP_LT;
	if(!strcmp(name, ">"))i=BSVM_SOP_CMP_GT;
	if(!strcmp(name, "<="))i=BSVM_SOP_CMP_LE;
	if(!strcmp(name, ">="))i=BSVM_SOP_CMP_GE;

	if(i>=0)return(i);

	i=BS2ASM_LookupOpcode(name);
	if(i>=0)return(i);

	return(-1);
}

BGBSVM_API int BS2ASM_LookupOpcodeArgs(char *name, int *at, int na)
{
	char *s, *s1;
	int i, j;

	j=BS2ASM_LookupOpcode(name);
	for(i=(j+1); i<bsvm_ops_nops; i++)
	{
		s=bsvm_ops_strs[i];
		j=strlen(name);
		if(strnicmp(s, name, j))
			break;
		if(s[j]!='.')break;

		s1=bsvm_ops_args[i]; j=0;
		while(*s1)
		{
			if(j>=na)break;

			if((*s1=='S') && (at[j]!=1))break;
			if((*s1=='K') && (at[j]!=2))break;
			if((*s1=='T') && (at[j]!=3))break;
			if((*s1=='I') && (at[j]!=4))break;
			if((*s1=='F') && (at[j]!=5))break;
			s1++; j++;
		}

		if((*s1) || (j<na))continue;
		return(i);
	}
	return(-1);
}

BGBSVM_API char *BS2ASM_ParseOpArgs(BS2ASM_Context *ctx, BS2ASM_Block *blk,
	char *str, char *opname)
{
	char b[64], b2[64];
	char *argn[16];
	int argt[16];

	char *s, *t, *s1;
	int narg;
	int ty, ty2;
	int i, j, k;


	s=str;

	i=BS2ASM_LookupOpcode(opname);
	if(!(*bsvm_ops_args[i]))
	{
		BS2ASM_EmitBlockOpcode(blk, i);
		if(*s==';')s++;
		return(s);
	}

	narg=0;
	while(*s)
	{
		BS2ASM_Parse_Token(s, b, &ty);
		if(!strcmp(b, ";"))
		{
			s=BS2ASM_Parse_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, ","))
		{
			s=BS2ASM_Parse_Token(s, b, &ty);
			continue;
		}

		j=0;
		if(!strcmp(b, "%"))
		{
			s=BS2ASM_Parse_Token(s, b, &ty);
			BS2ASM_Parse_Token(s, b, &ty);
			j=1;
		}

		if(!strcmp(b, "~"))
		{
			s=BS2ASM_Parse_Token(s, b, &ty);
			BS2ASM_Parse_Token(s, b, &ty);
			j=2;
		}

		if(!j && (ty==BTK_NAME))j=1;
		if(!j && (ty==BTK_STRING))j=3;

		if(!j && (ty==BTK_NUMBER))
		{
			j=4; s1=b;
			while(*s1)
			{
				if(*s1=='.')j=5;
				if(*s1=='e')j=5;
				s1++;
			}
		}

		s=BS2ASM_Parse_Token(s, b, &ty);
		argn[narg]=bs2asm_strdup(b);
		argt[narg]=j;
		narg++;
	}

	s1=bsvm_ops_args[i];
	if(!strcmp(s1, "?"))
		i=BS2ASM_LookupOpcodeArgs(opname, argt, narg);

	if(i<0)
	{
		BS2ASM_Error(ctx, "Bad Opcode Args for %s\n", opname);
		return(s);
	}

	BS2ASM_EmitBlockOpcode(blk, i);

	s1=bsvm_ops_args[i]; j=0;
	while(*s1)
	{
		switch(*s1)
		{
		case 'S':
			k=BS2ASM_LitIndexSym(ctx, argn[j]);
			BS2ASM_EmitBlockVLI(blk, k);
			break;
//		case 'K':
//			k=BS2ASM_LitIndexSym(ctx, argn[j]);
//			BS2ASM_EmitBlockVLI(blk, k);
//			break;
		case 'T':
			k=BS2ASM_LitIndexString(ctx, argn[j]);
			BS2ASM_EmitBlockVLI(blk, k);
			break;
//		case 'I':
//			BS2ASM_EmitBlockSVLI(blk, bs2asm_atoi(argn[j]));
//			break;

		case 'U':
			BS2ASM_EmitBlockVLI(blk, bs2asm_atoi(argn[j]));
			break;
		case 'V':
			BS2ASM_EmitBlockSVLI(blk, bs2asm_atoi(argn[j]));
			break;

//		case 'F':
//			k=BS2ASM_LitIndexDouble(ctx, atof(argn[j]));
//			BS2ASM_EmitBlockVLI(blk, k);
//			break;
		default:
			BS2ASM_Error(ctx, "OpTab: Bad Type %s %c\n", opname, *s1);
			break;
		}
		s1++; j++;
	}
	return(s);
}

BGBSVM_API char *BS2ASM_ParseCommand(BS2ASM_Context *ctx,
	BS2ASM_Block *blk, char *str)
{
	char b[64], b2[64];

	BS2ASM_Block *blk1;

	long long li, lj, lk;
	double f;

	char *s, *t, *s1;
	int ty, ty2;
	int i, j, k;


	s=BS2ASM_Parse_EatWhite(str);
	if(!*s)return(s);

	if(*s=='$')
	{
		s++;
		s=BS2ASM_Parse_Token(s, b, &ty);

		i=BS2ASM_LitIndexSym(ctx, b);
		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH);
		BS2ASM_EmitBlockVLI(blk, i);

		return(s);
	}


	if((*s=='%') || (*s=='=') || (*s=='&') || (*s==')') ||
		(*s=='?') || (*s=='!') || (*s=='*'))
//		if(s[1]>' ')
		if((s[1]=='.') || (s[1]=='_') ||
			(s[1]>='A' && s[1]<='Z') ||
			(s[1]>='a' && s[1]<='z'))
	{
		t=s++; s=BS2ASM_Parse_Token(s, b, &ty);
		BS2ASM_Parse_Token(s, b2, &ty2);

		if((*t=='=') && (*b=='.'))
		{
			i=BS2ASM_LitIndexSym(ctx, b+1);
//			BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RS);
//			BS2ASM_EmitBlockVLI(blk, i);
//			BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_STORESLOT);

			BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_STOREINDEX_S);
			BS2ASM_EmitBlockVLI(blk, i);
			return(s);
		}

//		i=BS2ASM_IndexSymbol(ctx, b);
//		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RS);
//		BS2ASM_EmitBlockVLI(blk, i);

		i=-1;

		if(*t=='%')i=BSVM_SOP_LOAD;
		if(*t=='=')i=BSVM_SOP_STORE;
		if(*t=='&')i=BSVM_SOP_LOADA_S;
		if(*t==')')i=BSVM_SOP_CALL_S;

		if(i>=0)
		{
			j=BS2ASM_LitIndexSym(ctx, b);
			BS2ASM_EmitBlockOpcode(blk, i);
			BS2ASM_EmitBlockVLI(blk, j);
			return(s);
		}

		if(*t=='?')i=BSVM_SOP_JMP_TRUE;
		if(*t=='!')i=BSVM_SOP_JMP_FALSE;
		if(*t=='*')i=BSVM_SOP_JMP;

		if(i>=0)
		{
			BS2ASM_EmitBlockOpcode(blk, i);
			BS2ASM_EmitBlockJmpAddr(blk, b);
			return(s);
		}

//		if((*t=='%') && !strcmp(b2, ",") &&
//			(ty2!=BTK_STRING))
//		{
//			s=BS2ASM_Parse_Token(s, b2, &ty2);
//			i=BSVM_SOP_LARG;
//		}

//		BS2ASM_EmitBlockOpcode(blk, i);
//		BS2ASM_EmitBlockVLI(blk, j);

		return(s);
	}

#if 0
	if((s[0]=='<') && (s[1]=='>'))
	{
		s+=2;
		s=BS2ASM_Parse_Token(s, b, &ty);
		s++;
		s=BS2ASM_Parse_Token(s, b2, &ty2);

//		i=BS2ASM_IndexSymbol(ctx, b);
//		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RS);
//		BS2ASM_EmitBlockVLI(blk, i);

//		i=BS2ASM_IndexSymbol(ctx, b2);
//		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RS);
//		BS2ASM_EmitBlockVLI(blk, i);

		i=BS2ASM_LitIndexSym(ctx, b);
		j=BS2ASM_LitIndexSym(ctx, b2);
		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_JMP_LG);
		BS2ASM_EmitBlockVLI(blk, i);
		BS2ASM_EmitBlockVLI(blk, j);

		return(s);
	}
#endif

	if((*s=='-') && (s[1]>='0') && (s[1]<='9'))
	{
		b[0]=*s++;
		s=BS2ASM_Parse_Token(s, b+1, &ty);

		if(bs2asm_strisreal(b))
		{
			j=BS2ASM_LitIndexDouble(ctx, atof(b));
//			i=BSVM_SOP_PUSH_F;
			i=BSVM_SOP_PUSH;
			BS2ASM_EmitBlockOpcode(blk, i);
			BS2ASM_EmitBlockVLI(blk, j);
			return(s);
		}

		li=bs2asm_atoi(b);
		i=BSVM_SOP_PUSH_XI;

		if(*s=='L') { i=BSVM_SOP_PUSH_XL; s++; }
		if(li>=(1LL<<31)) i=BSVM_SOP_PUSH_XL;
		if(li<=(-(1LL<<31))) i=BSVM_SOP_PUSH_XL;

		BS2ASM_EmitBlockOpcode(blk, i);
		BS2ASM_EmitBlockSVLI(blk, li);

		return(s);
	}

	s=BS2ASM_Parse_Token(s, b, &ty);
	BS2ASM_Parse_Token(s, b2, &ty2);

	if(ty==BTK_NUMBER)
	{
		if(bs2asm_strisreal(b))
		{
			j=BS2ASM_LitIndexDouble(ctx, atof(b));
//			i=BSVM_SOP_PUSH_F;
			i=BSVM_SOP_PUSH;
			BS2ASM_EmitBlockOpcode(blk, i);
			BS2ASM_EmitBlockVLI(blk, j);
			return(s);
		}

		li=bs2asm_atoi(b);
		i=BSVM_SOP_PUSH_XI;

		if(*s=='L') { i=BSVM_SOP_PUSH_XL; s++; }
		if(li>=(1LL<<31)) i=BSVM_SOP_PUSH_XL;
		if(li<=(-(1LL<<31))) i=BSVM_SOP_PUSH_XL;

		BS2ASM_EmitBlockOpcode(blk, i);
		BS2ASM_EmitBlockSVLI(blk, li);

		return(s);
	}

	if(ty==BTK_STRING)
	{
//		s++;
//		s=BS2ASM_Parse_Token(s, b, &ty);

//		i=BS2ASM_IndexSymbol(ctx, b);
		i=BS2ASM_LitIndexString(ctx, b);

//		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RT);
		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH);
		BS2ASM_EmitBlockVLI(blk, i);

		return(s);
	}

	i=-1;
	if(!strcmp(b, "("))i=BSVM_SOP_MARK;
	if(!strcmp(b, ")"))i=BSVM_SOP_CALL_S;
	if(!strcmp(b, "*)"))i=BSVM_SOP_CALL;

	if(!strcmp(b, "+"))i=BSVM_SOP_ADD;
	if(!strcmp(b, "-"))i=BSVM_SOP_SUB;
	if(!strcmp(b, "*"))i=BSVM_SOP_MUL;
	if(!strcmp(b, "/"))i=BSVM_SOP_DIV;
	if(!strcmp(b, "%"))i=BSVM_SOP_MOD;
	if(!strcmp(b, "&"))i=BSVM_SOP_AND;
	if(!strcmp(b, "|"))i=BSVM_SOP_OR;
	if(!strcmp(b, "^"))i=BSVM_SOP_XOR;

	if(!strcmp(b, "<<"))i=BSVM_SOP_SHL;
	if(!strcmp(b, ">>"))i=BSVM_SOP_SHR;

	if(!strcmp(b, "++"))i=BSVM_SOP_INC;
	if(!strcmp(b, "--"))i=BSVM_SOP_DEC;
	if(!strcmp(b, "+++"))i=BSVM_SOP_INC2;
	if(!strcmp(b, "---"))i=BSVM_SOP_DEC2;

//	if(!strcmp(b, ","))i=BSVM_SOP_ARG;

	if(i>=0)
	{
		BS2ASM_EmitBlockOpcode(blk, i);
		return(s);
	}

	if(!strcmp(b, "=="))i=BSVM_SOP_CMP_EQ;
	if(!strcmp(b, "!="))i=BSVM_SOP_CMP_NE;
	if(!strcmp(b, "<"))i=BSVM_SOP_CMP_LT;
	if(!strcmp(b, ">"))i=BSVM_SOP_CMP_GT;
	if(!strcmp(b, "<="))i=BSVM_SOP_CMP_LE;
	if(!strcmp(b, ">="))i=BSVM_SOP_CMP_GE;

	if(i>=0)
	{
		if(*s>' ')
		{
			if(*s=='?')s++;
			s=BS2ASM_Parse_Token(s, b2, &ty2);

			if(!strcmp(b, "=="))i=BSVM_SOP_JMP_EQ;
			if(!strcmp(b, "!="))i=BSVM_SOP_JMP_NE;
			if(!strcmp(b, "<"))i=BSVM_SOP_JMP_LT;
			if(!strcmp(b, ">"))i=BSVM_SOP_JMP_GT;
			if(!strcmp(b, "<="))i=BSVM_SOP_JMP_LE;
			if(!strcmp(b, ">="))i=BSVM_SOP_JMP_GE;

			BS2ASM_EmitBlockOpcode(blk, i);
			BS2ASM_EmitBlockJmpAddr(blk, b2);

//			j=BS2ASM_LitIndexSym(ctx, b2);
//			BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RS);
//			BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH);
//			BS2ASM_EmitBlockVLI(blk, j);
//			BS2ASM_EmitBlockVLI(blk, i);

			return(s);
		}

		BS2ASM_EmitBlockOpcode(blk, i);
		return(s);
	}

#if 1
	if(!strcmp(b, "{"))
	{
//		s=BS2ASM_Parse_Token(s, b, &ty);

		blk1=BS2ASM_AllocBlock(ctx);
		while(1)
		{
			BS2ASM_Parse_Token(s, b, &ty);
			if(!strcmp(b, "}"))
			{
				s=BS2ASM_Parse_Token(s, b, &ty);
				break;
			}

			s=BS2ASM_ParseCommand(ctx, blk1, s);
		}

		BS2ASM_FinishBlock(ctx, blk1);

//		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RB);
//		BS2ASM_EmitBlockVLI(blk, blk1->litidx);

		i=BS2ASM_LitIndexBlock(ctx, blk1);
		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH);
		BS2ASM_EmitBlockVLI(blk, i);
	}
#endif

	if(ty!=BTK_NAME)
	{
		BS2ASM_Error(ctx, "BS2ASM_ParseBlock: Bad Token %s\n", b);

		return(s);
	}

	if(*s==':')
	{
		BS2ASM_AddBlockLabel(blk, b);

		s++;
		return(s);
	}

#if 0
	if(*s=='(')
	{
		i=BS2ASM_LitIndexSym(ctx, b);
		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RS);
		BS2ASM_EmitBlockVLI(blk, i);

		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_MARK_S);

		s++;
		return(s);
	}
#endif

	if(*b=='.')
	{
		i=BS2ASM_LitIndexSym(ctx, b+1);
//		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_RS);
//		BS2ASM_EmitBlockVLI(blk, i);

		BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_LOADINDEX_S);
		BS2ASM_EmitBlockVLI(blk, i);

		s++;
		return(s);
	}

	i=BS2ASM_LookupOpcode(b);
	if(i>=0)
	{
		s=BS2ASM_ParseOpArgs(ctx, blk, s, b);
		return(s);
	}

//	i=BS2ASM_LitIndexSym(ctx, b);
//	BS2ASM_EmitBlockOpcode(blk, BSVM_SOP_PUSH_DI);
//	BS2ASM_EmitBlockVLI(blk, i);

	return(s);
}
