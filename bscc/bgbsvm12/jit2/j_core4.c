#include <bgbsvm.h>
#include <bgbasm.h>

#include <stdarg.h>


//#define JIT_OFFS(n)	((&BSVM_SVMState.n)-(&BSVM_SVMState))

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];
extern int bsvm_ops_nops;

// extern dyt bsvm_svm_toplevel;

BSVM_SVMState bsvm_jit_svmstate;	//example VM state
// #define JIT_OFFS(n)	(((byte *)(&(bsvm_jit_svmstate.n)))-((byte *)(&bsvm_jit_svmstate)))


void BSVM_J2Core_Init()
{
	static int init=0;
	int i, j, k, l;

	if(init)return;
	init=1;

//	bsvm_jit_ctx=malloc(sizeof(BSVM_JitContext));
//	memset(bsvm_jit_ctx, 0, sizeof(BSVM_JitContext));
}

char *BSVM_J2Core_GenSym()
{
//	char buf[64];
//	sprintf(buf, "JIT$%d", bsvm_jit_seq++);
//	return(dyllStrdup(buf));
	return(BSVMC_GenSym());
}


dyt BSVM_J2Core_ReadID(bsjContext ctx)
{
	dyt t;
	int i;

	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	t=dyarrayidx(ctx->lit, i);
	return(t);
}

int BSVM_J2Core_ReadIDX(bsjContext ctx)
{
	int i;
	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	return(i);
}

int BSVM_J2Core_ReadSIDX(bsjContext ctx)
{
	int i;
	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

s64 BSVM_J2Core_ReadVLI(bsjContext ctx)
{
	return(BS2ASM_DecVLI(&(ctx->ip)));
}

s64 BSVM_J2Core_ReadSVLI(bsjContext ctx)
{
	return(BS2ASM_DecSVLI(&(ctx->ip)));
}

int BSVM_J2Core_ReadShort(bsjContext ctx)
{
	int i;
	i=*ctx->ip++;
	i|=(*ctx->ip++)<<8;
	return((signed short)i);
}

int BSVM_J2Core_DumpOpcode(bsjContext ctx)
{
	char *bops[]={"add", "sub", "mul", "div", "mod", "and", "or",
		"xor", "shl", "shr", "eq", "eqv", "neq", "neqv", "l",
		"g", "le", "ge"};

	char *uops[]={"neg", "not", "lnot", "pos", "umul", "udiv", "uidiv",
		"umod", "uand", "uor", "uxor", "uland", "ulor", "ulxor", "inc",
		"dec", "-", "-"};

	dyt t;
	byte *ip, *ip1, *ip2;
	char *s, *s1;
	int i, j, k, l;

	ip=ctx->ip;

	i=*ip++;
	if(i>=192)
	{
		i=(i-192)*256+(*ip++);
		bsj2_printlog(ctx, "%08X %02X%02X ", ctx->ip,
			ctx->ip[0], ctx->ip[1]);
	}else
	{
		bsj2_printlog(ctx, "%08X %02X ", ctx->ip, i);
	}

#if 1
	if((i<0) || (i>=bsvm_ops_nops))
	{
		printf("BAD\n");
		return(-1);
	}
#endif

	k=0; s=bsvm_ops_args[i]; ip1=ip;
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N') || (*s=='J'))
			{ j=*ip1++; k++; if(j&128)k++; s++; continue; }
		if(*s=='A') { ip1+=2; k+=2; s++; continue; }

		if(*s=='C') { ip1++; k++; s++; continue; }
		if(*s=='B') { ip1++; k++; s++; continue; }
		if(*s=='W') { ip1+=2; k+=2; s++; continue; }
		if(*s=='K') { ip1+=2; k+=2; s++; continue; }
		if(*s=='H') { ip1+=2; k+=2; s++; continue; }

		if((*s=='U') || (*s=='V'))
		{
			ip2=ip1; BS2ASM_DecVLI(&ip2); l=ip2-ip1;
			ip1+=l; k+=l; s++; continue;
		}

		if(*s=='u') { ip1++; k++; s++; continue; }
		if(*s=='b') { ip1++; k++; s++; continue; }

		s++;
	}

	ip1=ip;
	for(j=0; j<k; j++)bsj2_printlog(ctx, "%02X", *ip1++);

	if(i<192)bsj2_printlog(ctx, "  ");
	for(; j<4; j++)bsj2_printlog(ctx, "  ");

	bsj2_printlog(ctx, " %s ", bsvm_ops_strs[i]);

	s=bsvm_ops_args[i];
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			t=dyarrayidx(ctx->lit, i);
//			BSVM_TY_Print(t);
//			s1=dyToString(t);

			s1=NULL;
			if(dysymbolp(t))s1=dysymbolv(t);
			if(dykeywordp(t))s1=dysymbolv(t);
			if(dystringp(t))s1=dystringv(t);

			if(s1)bsj2_printlog(ctx, "%s", s1);
			bsj2_printlog(ctx, " ");
			s++;
			continue;
		}
		if((*s=='N') || (*s=='J'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			bsj2_printlog(ctx, "%d ", i);
			s++;
			continue;
		}
		if(*s=='A')
		{
			i=*ip++; i|=(*ip++)<<8;
			bsj2_printlog(ctx, "%08X ", ip+i);
			s++;
			continue;
		}
		if(*s=='C')
		{
			i=(signed char)(*ip++);
			bsj2_printlog(ctx, "%d ", i);
			s++; continue;
		}
		if(*s=='B')
		{
			i=*ip++;
			bsj2_printlog(ctx, "%d ", i);
			s++; continue;
		}
		if(*s=='K')
		{
			i=*ip++; i|=(*ip++)<<8; i=(short)i;
			bsj2_printlog(ctx, "%d ", i);
			s++; continue;
		}
		if(*s=='W')
		{
			i=*ip++; i|=(*ip++)<<8;
			bsj2_printlog(ctx, "%d ", i);
			s++; continue;
		}
		if(*s=='H')
		{
			i=*ip++; i|=(*ip++)<<8;
			bsj2_printlog(ctx, "%g ", i/100.0);
			s++; continue;
		}

		if(*s=='U')
		{
			i=BS2ASM_DecVLI(&ip);
			bsj2_printlog(ctx, "%d ", i);
			s++; continue;
		}
		if(*s=='V')
		{
			i=BS2ASM_DecSVLI(&ip);
			bsj2_printlog(ctx, "%d ", i);
			s++; continue;
		}

		if(*s=='b')
		{
			i=*ip++;
			bsj2_printlog(ctx, "%s ", bops[i]);
			s++; continue;
		}
		if(*s=='u')
		{
			i=*ip++;
			bsj2_printlog(ctx, "%s ", uops[i]);
			s++; continue;
		}
		s++;
	}

	bsj2_printlog(ctx, "\n");

	return(0);
}

void BSVM_J2Core_SkipOpcodeArgs(bsjContext ctx, int op)
{
	dyt t;
	byte *ip, *ip1, *ip2;
	char *s;
	int i, j, k, l;

	ip=ctx->ip;

	k=0; s=bsvm_ops_args[op]; ip1=ip;
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N') || (*s=='J'))
			{ j=*ip1++; k++; if(j&128) { ip1++; k++;} s++; continue; }
		if(*s=='A') { ip1+=2; k+=2; s++; continue; }

		if(*s=='C') { ip1++; k++; s++; continue; }
		if(*s=='B') { ip1++; k++; s++; continue; }
		if(*s=='W') { ip1+=2; k+=2; s++; continue; }
		if(*s=='K') { ip1+=2; k+=2; s++; continue; }
		if(*s=='H') { ip1+=2; k+=2; s++; continue; }

		if((*s=='U') || (*s=='V'))
		{
			ip2=ip1; BS2ASM_DecVLI(&ip2); l=ip2-ip1;
			ip1+=l; k+=l; s++; continue;
		}

		if(*s=='u') { ip1++; k++; s++; continue; }
		if(*s=='b') { ip1++; k++; s++; continue; }

		s++;
	}

	ctx->ip=ip1;
}

void BSVM_J2Core_SkipOpcode(bsjContext ctx)
{
	dyt t;
	byte *ip, *ip1;
	char *s;
	int i, j, k;

	ip=ctx->ip;
	i=*ip++;
	if(i>=192)
		{ i=(i-192)*256+(*ip++); }
	ctx->ip=ip;
	BSVM_J2Core_SkipOpcodeArgs(ctx, i);
}

void BSVM_J2Core_PreStep(bsjContext ctx)
{
	char buf[64];
	byte *ip, *ip1;
	char *s;
	int i, j;

	ip=ctx->ip;
	i=*ip++;
	if(i>=192)i=(i-192)*256+(*ip++);
	switch(i)
	{
	case BSVM_SOP_JMP:
	case BSVM_SOP_JMP_TRUE:		case BSVM_SOP_JMP_FALSE:

	case BSVM_SOP_JMP_E_FN:		case BSVM_SOP_JMP_NE_FN:
	case BSVM_SOP_JMP_E_FL:		case BSVM_SOP_JMP_NE_FL:
	case BSVM_SOP_JMP_L_FN:		case BSVM_SOP_JMP_G_FN:
	case BSVM_SOP_JMP_LE_FN:		case BSVM_SOP_JMP_GE_FN:
	case BSVM_SOP_JMP_L_FL:		case BSVM_SOP_JMP_G_FL:
	case BSVM_SOP_JMP_LE_FL:		case BSVM_SOP_JMP_GE_FL:

	case BSVM_SOP_JMP_E_ZFN:		case BSVM_SOP_JMP_NE_ZFN:
	case BSVM_SOP_JMP_L_ZFN:		case BSVM_SOP_JMP_G_ZFN:
	case BSVM_SOP_JMP_LE_ZFN:	case BSVM_SOP_JMP_GE_ZFN:
		i=*ip++; i|=(*ip++)<<8; i=((signed short)i); ip1=ip+i;
		for(j=0; j<ctx->n_lbl; j++)
			if(ctx->lbl[j]==ip1)
				break;
		if(j>=ctx->n_lbl)
		{
			j=ctx->n_lbl++;
			ctx->lbl[j]=ip1;
			ctx->lbln[j]=BSVM_J2Core_GenSym();
//			printf("LBL %08X %s\n", (int)ip1, ctx->lbln[j]);
		}
		break;

	case BSVM_SOP_JMP_COND_U:	case BSVM_SOP_JMP_COND_B:

	case BSVM_SOP_JMP_E_LZFN:	case BSVM_SOP_JMP_NE_LZFN:
	case BSVM_SOP_JMP_L_LZFN:	case BSVM_SOP_JMP_G_LZFN:
	case BSVM_SOP_JMP_LE_LZFN:	case BSVM_SOP_JMP_GE_LZFN:

	case BSVM_SOP_JMP_E_FNC:		case BSVM_SOP_JMP_NE_FNC:
	case BSVM_SOP_JMP_L_FNC:		case BSVM_SOP_JMP_G_FNC:
	case BSVM_SOP_JMP_LE_FNC:	case BSVM_SOP_JMP_GE_FNC:
		ip++;
		i=*ip++; i|=(*ip++)<<8; i=((signed short)i); ip1=ip+i;
		for(j=0; j<ctx->n_lbl; j++)
			if(ctx->lbl[j]==ip1)
				break;
		if(j>=ctx->n_lbl)
		{
			j=ctx->n_lbl++;
			ctx->lbl[j]=ip1;
			ctx->lbln[j]=BSVM_J2Core_GenSym();
//			printf("LBL %08X %s\n", (int)ip1, ctx->lbln[j]);
		}
		break;

	case BSVM_SOP_JMP_E_LFN:		case BSVM_SOP_JMP_NE_LFN:
	case BSVM_SOP_JMP_L_LFN:		case BSVM_SOP_JMP_G_LFN:
	case BSVM_SOP_JMP_LE_LFN:	case BSVM_SOP_JMP_GE_LFN:

	case BSVM_SOP_JMP_E_LFNC:	case BSVM_SOP_JMP_NE_LFNC:
	case BSVM_SOP_JMP_L_LFNC:	case BSVM_SOP_JMP_G_LFNC:
	case BSVM_SOP_JMP_LE_LFNC:	case BSVM_SOP_JMP_GE_LFNC:
		ip+=2;
		i=*ip++; i|=(*ip++)<<8; i=((signed short)i); ip1=ip+i;
		for(j=0; j<ctx->n_lbl; j++)
			if(ctx->lbl[j]==ip1)
				break;
		if(j>=ctx->n_lbl)
		{
			j=ctx->n_lbl++;
			ctx->lbl[j]=ip1;
			ctx->lbln[j]=BSVM_J2Core_GenSym();
//			printf("LBL %08X %s\n", (int)ip1, ctx->lbln[j]);
		}
		break;

	default:
		s=bsvm_ops_args[i];
		while(*s)
		{
			if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N') ||
					(*s=='J'))
				{ i=*ip++; if(i&128)ip++; }
			if(*s=='A')ip+=2;
			if(*s=='C')ip++;
			if(*s=='B')ip++;
			if(*s=='W')ip+=2;
			if(*s=='K')ip+=2;
			if(*s=='H')ip+=2;
			if(*s=='G')ip+=2;
			if(*s=='u')ip++;
			if(*s=='b')ip++;
			if(*s=='U')BS2ASM_DecVLI(&ip);
			if(*s=='V')BS2ASM_DecVLI(&ip);
			s++;
		}
		break;
	}

	ctx->ip=ip;
}

char *BSVM_J2Core_GetLabel(bsjContext ctx, byte *ip)
{
	int i;
	for(i=0; i<ctx->n_lbl; i++)
		if(ctx->lbl[i]==ip)
			return(ctx->lbln[i]);
	return(NULL);
}

int BSVM_J2Core_Step(bsjContext ctx)
{
	dyt t, u, v;
	int i, j, k, rs;
	char *s, *s1, *s2;

	for(i=0; i<ctx->n_lbl; i++)
		if(ctx->lbl[i]==ctx->ip)
	{
//		BSVM_J2Core_FlattenStack(ctx);
//		basm_print("%s:\n", ctx->lbln[i]);
		BSVM_J2Arch_EmitLabel(ctx, ctx->lbln[i]);
	}

	bsj2_printlog(ctx, "; ");
	BSVM_J2Core_DumpOpcode(ctx);

	i=*ctx->ip++;
	if(i>=192)i=(i-192)*256+(*ctx->ip++);

//	bsj2_printlog(ctx, "OP: %s\n", bsvm_ops_strs[i]);

	switch(i)
	{
	case BSVM_SOP_NOP:
		break;
	case BSVM_SOP_BLOCK:
		break;
	case BSVM_SOP_DBGMARK:
		t=BSVM_J2Core_ReadID(ctx);
		t=BSVM_J2Core_ReadID(ctx);
		break;
	case BSVM_SOP_CGENAST:
		t=BSVM_J2Core_ReadID(ctx);
		break;
	case BSVM_SOP_FN:
		t=BSVM_J2Core_ReadID(ctx);
		break;
	case BSVM_SOP_LN:
		t=BSVM_J2Core_ReadID(ctx);
		break;
	case BSVM_SOP_LABEL:
		t=BSVM_J2Core_ReadID(ctx);
		break;

	case BSVM_SOP_SLEEP:
		break;
	case BSVM_SOP_SLEEP_SV:
		BSVM_J2Core_ReadShort(ctx);
		break;

	case BSVM_SOP_PUSH:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2Stack_PushDyt(ctx, t);
		break;
	case BSVM_SOP_POP:
		BSVM_J2Stack_PopDestroy(ctx);
		break;
	case BSVM_SOP_LOAD:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_LoadName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_STORE:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_StoreName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_BIND:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BindName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_DYNBIND:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_DynBindName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_LEXBIND:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_LexBindName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_LOADA_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_LoadAddrName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_LOADB_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_LoadRefName(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_LOADINDEX:
		BSVM_J2StackOp_LoadIndex(ctx);
		break;
	case BSVM_SOP_STOREINDEX:
		BSVM_J2StackOp_StoreIndex(ctx);
		break;
	case BSVM_SOP_LOADINDEXA:
		BSVM_J2StackOp_LoadIndexAddr(ctx);
		break;

	case BSVM_SOP_LOADINDEX_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_LoadIndexName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_STOREINDEX_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_StoreIndexName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_LOADINDEXA_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_LoadIndexAddrName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_LOADINDEXB_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_LoadIndexRefName(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_LOADINDEX_I:
		BSVM_J2StackOp_LoadIndexInt(ctx, *ctx->ip++);
		break;
	case BSVM_SOP_STOREINDEX_I:
		BSVM_J2StackOp_StoreIndexInt(ctx, *ctx->ip++);
		break;
	case BSVM_SOP_LOADINDEXA_I:
		BSVM_J2StackOp_LoadIndexAddrInt(ctx, *ctx->ip++);
		break;
	case BSVM_SOP_LOADINDEXB_I:
		BSVM_J2StackOp_LoadIndexRefInt(ctx, *ctx->ip++);
		break;

	case BSVM_SOP_LOADINDEX_0:
		BSVM_J2StackOp_LoadIndexInt(ctx, 0); break;
	case BSVM_SOP_LOADINDEX_1:
		BSVM_J2StackOp_LoadIndexInt(ctx, 1); break;
	case BSVM_SOP_LOADINDEX_2:
		BSVM_J2StackOp_LoadIndexInt(ctx, 2); break;
	case BSVM_SOP_LOADINDEX_3:
		BSVM_J2StackOp_LoadIndexInt(ctx, 3); break;

	case BSVM_SOP_STOREINDEX_0:
		BSVM_J2StackOp_StoreIndexInt(ctx, 0); break;
	case BSVM_SOP_STOREINDEX_1:
		BSVM_J2StackOp_StoreIndexInt(ctx, 1); break;
	case BSVM_SOP_STOREINDEX_2:
		BSVM_J2StackOp_StoreIndexInt(ctx, 2); break;
	case BSVM_SOP_STOREINDEX_3:
		BSVM_J2StackOp_StoreIndexInt(ctx, 3); break;

	case BSVM_SOP_EXCH_SELF:
		BSVM_J2StackOp_ExchSelf(ctx); break;
	case BSVM_SOP_EXCH_TOP:
		BSVM_J2StackOp_ExchTop(ctx); break;

	case BSVM_SOP_PUSH_SELF:
		BSVM_J2StackOp_PushSelf(ctx); break;
	case BSVM_SOP_PUSH_NULL:
		BSVM_J2Stack_PushDyt(ctx, NULL); break;
	case BSVM_SOP_PUSH_TRUE:
		BSVM_J2Stack_PushDyt(ctx, BGBDY_TRUE); break;
	case BSVM_SOP_PUSH_FALSE:
		BSVM_J2Stack_PushDyt(ctx, BGBDY_FALSE); break;
	case BSVM_SOP_PUSH_SV:
		i=BSVM_J2Core_ReadShort(ctx);
		BSVM_J2Stack_PushDyt(ctx, dyint(i));
		break;
	case BSVM_SOP_PUSH_SV_P:
		i=*ctx->ip++;
		BSVM_J2Stack_PushDyt(ctx, dyint(i));
		break;
	case BSVM_SOP_PUSH_SV_N:
		i=*ctx->ip++;
		BSVM_J2Stack_PushDyt(ctx, dyint(-i));
		break;

	case BSVM_SOP_PUSH_XI:
		t=dyint(BSVM_J2Core_ReadSVLI(ctx));
		BSVM_J2Stack_PushDyt(ctx, t);
		break;
	case BSVM_SOP_PUSH_XL:
		t=dylong(BSVM_J2Core_ReadSVLI(ctx));
		BSVM_J2Stack_PushDyt(ctx, t);
		break;

	case BSVM_SOP_PUSH_SV_F:
		i=BSVM_J2Core_ReadShort(ctx);
		BSVM_J2Stack_PushDyt(ctx, dyflonum(i/100.0));
		break;
	case BSVM_SOP_PUSH_SV_FB:
		i=BSVM_J2Core_ReadShort(ctx);
		BSVM_J2Stack_PushDyt(ctx, dyflonum(i/10000.0));
		break;
	case BSVM_SOP_PUSH_SV_C:
		i=BSVM_J2Core_ReadShort(ctx);
		BSVM_J2Stack_PushDyt(ctx, dychar(i));
		break;

	case BSVM_SOP_PUSH_0: BSVM_J2Stack_PushDyt(ctx, dyint(0)); break;
	case BSVM_SOP_PUSH_1: BSVM_J2Stack_PushDyt(ctx, dyint(1)); break;
	case BSVM_SOP_PUSH_2: BSVM_J2Stack_PushDyt(ctx, dyint(2)); break;
	case BSVM_SOP_PUSH_3: BSVM_J2Stack_PushDyt(ctx, dyint(3)); break;
	case BSVM_SOP_PUSH_4: BSVM_J2Stack_PushDyt(ctx, dyint(4)); break;
	case BSVM_SOP_PUSH_5: BSVM_J2Stack_PushDyt(ctx, dyint(5)); break;
	case BSVM_SOP_PUSH_6: BSVM_J2Stack_PushDyt(ctx, dyint(6)); break;
	case BSVM_SOP_PUSH_7: BSVM_J2Stack_PushDyt(ctx, dyint(7)); break;
	case BSVM_SOP_PUSH_CF: BSVM_J2StackOp_PushCurrentFunc(ctx); break;

	case BSVM_SOP_LLOAD:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexLoadIdx(ctx, i); break;
	case BSVM_SOP_LSTORE:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexStoreIdx(ctx, i); break;
	case BSVM_SOP_LLOAD_F:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexLoadIdx(ctx, i); break;
	case BSVM_SOP_LSTORE_F:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexStoreIdx(ctx, i); break;

	case BSVM_SOP_LPOSTINC:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexPostIncrementIdx(ctx, i);
		break;
	case BSVM_SOP_LPOSTDEC:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexPostDecrementIdx(ctx, i);
		break;

	case BSVM_SOP_LPREINC:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexPreIncrementIdx(ctx, i);
		break;
	case BSVM_SOP_LPREDEC:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexPreDecrementIdx(ctx, i);
		break;

//	case BSVM_SOP_LINC_FN:
	case BSVM_SOP_LINC:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexIncrementIdx(ctx, i); break;
//	case BSVM_SOP_LDEC_FN:
	case BSVM_SOP_LDEC:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_LexDecrementIdx(ctx, i); break;

	case BSVM_SOP_CLEARENV: break;
	case BSVM_SOP_BINDPATTERN: break;
	case BSVM_SOP_LOADMINDEX: break;
	case BSVM_SOP_STOREMINDEX: break;

	case BSVM_SOP_TRYLOAD:
		t=BSVM_J2Core_ReadID(ctx);
		break;

	case BSVM_SOP_CAST_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_CastName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_CAST:
		BSVM_J2StackOp_Cast(ctx);
		break;
	case BSVM_SOP_SIZEOF_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_SizeofName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_SIZEOF:
		BSVM_J2StackOp_Sizeof(ctx);
		break;

	case BSVM_SOP_MARK:
		BSVM_J2Stack_PushMark(ctx);
		break;

	case BSVM_SOP_CALL:
		BSVM_J2StackOp_Call(ctx);
		break;
	case BSVM_SOP_TAILCALL:
		BSVM_J2StackOp_TailCall(ctx);
		break;
	case BSVM_SOP_CALL_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_CallName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_TAILCALL_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_TailCallName(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_METHODCALL:
		BSVM_J2StackOp_MethodCall(ctx); break;
	case BSVM_SOP_METHODTAILCALL:
		BSVM_J2StackOp_MethodTailCall(ctx); break;
	case BSVM_SOP_METHODCALL_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_MethodCallName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_METHODTAILCALL_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_MethodTailCallName(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_CALL_ASYNC:
		break;
	case BSVM_SOP_CALL_IFDEF:
		t=BSVM_J2Core_ReadID(ctx);
		break;
	case BSVM_SOP_CALL_IFNDEF:
		t=BSVM_J2Core_ReadID(ctx);
		break;

	case BSVM_SOP_BEGIN_SYNC:
		BSVM_J2StackOp_BeginSync(ctx);
		break;
	case BSVM_SOP_END_SYNC:
		BSVM_J2StackOp_EndSync(ctx);
		break;
	case BSVM_SOP_BEGIN_SYNC_OBJ:
	case BSVM_SOP_LOCK_OBJ:
		BSVM_J2StackOp_LockObj(ctx);
		break;
	case BSVM_SOP_END_SYNC_OBJ:
	case BSVM_SOP_UNLOCK_OBJ:
		BSVM_J2StackOp_UnlockObj(ctx);
		break;

	case BSVM_SOP_BEGIN_TRY:
		BSVM_J2StackOp_BeginTry(ctx);
		break;
	case BSVM_SOP_END_TRY:
		BSVM_J2StackOp_EndTry(ctx);
		break;
	case BSVM_SOP_END_CATCH:
		BSVM_J2StackOp_EndCatch(ctx);
		break;

	case BSVM_SOP_BEGIN_CATCH_FINAL:
		BSVM_J2StackOp_BeginCatchFinal(ctx);
		break;
	case BSVM_SOP_BEGIN_CATCH:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BeginCatch(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_THROW_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_ThrowName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_THROW_OBJ:
		BSVM_J2StackOp_ThrowObj(ctx);
		break;

	case BSVM_SOP_CATCH:
		t=BSVM_J2Core_ReadID(ctx);
		i=BSVM_J2Core_ReadShort(ctx);
		break;

	case BSVM_SOP_NEW_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_NewName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_NEWARRAY_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_NewArrayName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_DELETE:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DELETE);
		break;
	case BSVM_SOP_CLONE:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CLONE);
		break;
	case BSVM_SOP_TOSTRING:
		BSVM_J2StackOp_ToString(ctx);
		break;
	case BSVM_SOP_UNQUOTE:
		BSVM_J2StackOp_Unquote(ctx);
		break;

	case BSVM_SOP_NEG:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_NEG);
		break;
	case BSVM_SOP_LNOT:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_LNOT);
		break;
	case BSVM_SOP_DEFER:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEFER);
		break;
	case BSVM_SOP_SETDEFER:
		BSVM_J2StackOp_SetDefer(ctx);
		break;
	case BSVM_SOP_NOT:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_NOT);
		break;
	case BSVM_SOP_INC:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC);
		break;
	case BSVM_SOP_DEC:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC);
		break;
	case BSVM_SOP_INC2:
//		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC2);
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_ADD, 2);
		break;
	case BSVM_SOP_DEC2:
//		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC2);
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_SUB, 2);
		break;

	case BSVM_SOP_INC_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_IncrementName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_DEC_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_DecrementName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_POSTINC_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_PostIncrementName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_POSTDEC_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_PostDecrementName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_PREINC_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_PreIncrementName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_PREDEC_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_PreDecrementName(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_TYBIND:
		t=BSVM_J2Core_ReadID(ctx);
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_TyBind(ctx,
			dysymbolv(t), dysymbolv(u), dysymbolv(v));
		break;
	case BSVM_SOP_TYDYNBIND:
		t=BSVM_J2Core_ReadID(ctx);
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_TyDynBind(ctx,
			dysymbolv(t), dysymbolv(u), dysymbolv(v));
		break;
	case BSVM_SOP_TYLEXBIND:
		t=BSVM_J2Core_ReadID(ctx);
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_TyLexBind(ctx,
			dysymbolv(t), dysymbolv(u), dysymbolv(v));
		break;

	case BSVM_SOP_BEGIN_PACKAGE:
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BeginPackage(ctx,
			dysymbolv(u), dysymbolv(v));
		break;
	case BSVM_SOP_BEGIN_CLASS:
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BeginClass(ctx,
			dysymbolv(u), dysymbolv(v), dysymbolv(t));
		break;
	case BSVM_SOP_BEGIN_IFACE:
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BeginIface(ctx,
			dysymbolv(u), dysymbolv(v), dysymbolv(t));
		break;
	case BSVM_SOP_BEGIN_STRUCT:
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BeginStruct(ctx,
			dysymbolv(u), dysymbolv(v), dysymbolv(t));
		break;
	case BSVM_SOP_END_PACKAGE:
		BSVM_J2StackOp_EndPackage(ctx); break;
	case BSVM_SOP_END_CLASS:
		BSVM_J2StackOp_EndClass(ctx); break;
	case BSVM_SOP_END_IFACE:
		BSVM_J2StackOp_EndIface(ctx); break;
	case BSVM_SOP_END_STRUCT:
		BSVM_J2StackOp_EndStruct(ctx); break;
	case BSVM_SOP_CLASS_IFACE:
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_Iface(ctx,
			dysymbolv(u), dysymbolv(v));
		break;
	case BSVM_SOP_IMPORT:
		u=BSVM_J2Core_ReadID(ctx);
		v=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_Import(ctx,
			dysymbolv(u), dysymbolv(v));
		break;

	case BSVM_SOP_RET:
		BSVM_J2StackOp_Return(ctx);
		break;

	case BSVM_SOP_CALL_CF:
		BSVM_J2StackOp_CallCF(ctx);
		break;
	case BSVM_SOP_TAILCALL_CF:
		BSVM_J2StackOp_TailCallCF(ctx);
		break;


	case BSVM_SOP_CAR:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CAR);
		break;
	case BSVM_SOP_CDR:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CDR);
		break;
	case BSVM_SOP_CAAR:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CAR);
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CAR);
		break;
	case BSVM_SOP_CDAR:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CDR);
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CAR);
		break;
	case BSVM_SOP_CADR:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CAR);
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CDR);
		break;
	case BSVM_SOP_CDDR:
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CDR);
		BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_CDR);
		break;

	case BSVM_SOP_CLOSE:
		BSVM_J2StackOp_Close(ctx);
		break;

	case BSVM_SOP_DUP:
		BSVM_J2StackOp_Dup(ctx);
		break;
	case BSVM_SOP_DUP_F:
		BSVM_J2StackOp_Dup(ctx);
		break;
	case BSVM_SOP_EXCH:
		BSVM_J2StackOp_Exch(ctx);
		break;
	case BSVM_SOP_INDEX:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_IndexStack(ctx, i);
		break;
	case BSVM_SOP_RINDEX:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_RIndexStack(ctx, i);
		break;
	case BSVM_SOP_SETINDEX:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_SetIndexStack(ctx, i);
		break;
	case BSVM_SOP_SETRINDEX:
		i=BSVM_J2Core_ReadIDX(ctx);
		BSVM_J2StackOp_SetRIndexStack(ctx, i);
		break;
	case BSVM_SOP_ROLL_I:
		i=BSVM_J2Core_ReadIDX(ctx);
		j=BSVM_J2Core_ReadSIDX(ctx);
		break;
	case BSVM_SOP_COPY_I:
		i=BSVM_J2Core_ReadIDX(ctx);
		break;

#if 0
	case BSVM_SOP_DUP_R: 
		i=*ctx->ip++;
		BSVM_Jit2_StubError(ctx);
 		break;
	case BSVM_SOP_DUP_RF:
		i=*ctx->ip++;
		BSVM_Jit2_StubError(ctx);
		break;
	case BSVM_SOP_PUSH_R:
		i=*ctx->ip++;
		BSVM_Jit2_StubError(ctx);
		break;
	case BSVM_SOP_PUSH_RF:
		i=*ctx->ip++;
		BSVM_Jit2_StubError(ctx);
		break;
	case BSVM_SOP_POP_R:
		i=*ctx->ip++;
		BSVM_Jit2_StubError(ctx);
		break;
#endif

	case BSVM_SOP_UNARYOP:
		i=*ctx->ip++;
		BSVM_J2StackOp_UnaryOp(ctx, i|256);
		break;
	case BSVM_SOP_BINARYOP:
		i=*ctx->ip++;
		BSVM_J2StackOp_BinaryOp(ctx, i);
		break;
	case BSVM_SOP_UNARYOP_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_UnaryOpName(ctx, dysymbolv(t));
		break;
	case BSVM_SOP_BINARYOP_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BinaryOpName(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_ADD:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_ADD); break;
	case BSVM_SOP_SUB:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_SUB); break;
	case BSVM_SOP_MUL:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_MUL); break;
	case BSVM_SOP_DIV:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_DIV); break;
	case BSVM_SOP_MOD:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_MOD); break;
	case BSVM_SOP_AND:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_AND); break;
	case BSVM_SOP_OR:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_OR); break;
	case BSVM_SOP_XOR:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_XOR); break;
	case BSVM_SOP_SHL:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_SHL); break;
	case BSVM_SOP_SHR:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_SHR); break;

	case BSVM_SOP_CMP_EQ:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_EQV); break;
	case BSVM_SOP_CMP_NE:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_NEQV); break;
	case BSVM_SOP_CMP_LT:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_L); break;
	case BSVM_SOP_CMP_GT:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_G); break;
	case BSVM_SOP_CMP_LE:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_LE); break;
	case BSVM_SOP_CMP_GE:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_GE); break;
	case BSVM_SOP_CMP_EQQ:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_EQ); break;
	case BSVM_SOP_CMP_NEQ:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_NEQ); break;
	case BSVM_SOP_CMP_U:
		BSVM_J2StackOp_BinaryOp(ctx, BSVM_JIT2_BINOP_UNORD); break;

	case BSVM_SOP_JMP:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJump(ctx, s);
		break;
	case BSVM_SOP_JMP_TRUE:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpTrue(ctx, s);
		break;
	case BSVM_SOP_JMP_FALSE:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpFalse(ctx, s);
		break;

	case BSVM_SOP_JMP_COND_U:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondUnary(ctx, j|256, s);
		break;
	case BSVM_SOP_JMP_COND_B:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, j, s);
		break;

	case BSVM_SOP_JMP_SWEQ:
		t=BSVM_J2Core_ReadID(ctx);
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_Dup(ctx);
		BSVM_J2Stack_PushDyt(ctx, t);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_EQV, s);
		break;
	case BSVM_SOP_JMP_SWEQF:
		t=BSVM_J2Core_ReadID(ctx);
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_Dup(ctx);
		BSVM_J2Stack_PushDyt(ctx, t);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_EQV, s);
		break;
	case BSVM_SOP_JMP_SWDFL:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_Pop(ctx);
		BSVM_J2StackOp_DoJump(ctx, s);
		break;
	case BSVM_SOP_JMP_SWB:
		j=*ctx->ip++;
		t=BSVM_J2Core_ReadID(ctx);
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_Dup(ctx);
		BSVM_J2Stack_PushDyt(ctx, t);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, j, s);
		break;

	case BSVM_SOP_JMP_EQ:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_EQV, s);
		break;
	case BSVM_SOP_JMP_NE:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_NEQV, s);
		break;
	case BSVM_SOP_JMP_LT:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_L, s);
		break;
	case BSVM_SOP_JMP_GT:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_G, s);
		break;
	case BSVM_SOP_JMP_LE:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_LE, s);
		break;
	case BSVM_SOP_JMP_GE:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_GE, s);
		break;
	case BSVM_SOP_JMP_EQQ:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_EQ, s);
		break;
	case BSVM_SOP_JMP_NEQ:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_NEQ, s);
		break;
	case BSVM_SOP_JMP_U:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinary(ctx, BSVM_JIT2_BINOP_UNORD, s);
		break;

	case BSVM_SOP_LDJMP:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_LoadIndirectAddr(ctx, s);
		break;
	case BSVM_SOP_JMPIA:
		BSVM_J2StackOp_JumpIndirectAddr(ctx);
		break;

	case BSVM_SOP_LIST:
		BSVM_J2StackOp_BuildList(ctx); break;
	case BSVM_SOP_DICT:
		BSVM_J2StackOp_BuildDict(ctx); break;
	case BSVM_SOP_ATTR:
		BSVM_J2StackOp_BuildAttr(ctx); break;
	case BSVM_SOP_VECTOR:
		BSVM_J2StackOp_BuildVector(ctx); break;
	case BSVM_SOP_COMPLEX_I:
		BSVM_J2StackOp_BuildComplexImag(ctx); break;
	case BSVM_SOP_MATRIX:
		BSVM_J2StackOp_BuildMatrix(ctx); break;
	case BSVM_SOP_COMPLEX:
		BSVM_J2StackOp_BuildComplex(ctx); break;
	case BSVM_SOP_QUAT:
		BSVM_J2StackOp_BuildQuat(ctx); break;
	case BSVM_SOP_ARRAY:
		BSVM_J2StackOp_BuildArray(ctx); break;

	case BSVM_SOP_ARRAY_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2StackOp_BuildArrayName(ctx, dysymbolv(t));
		break;

	case BSVM_SOP_EXCH_YXZ: case BSVM_SOP_EXCH_YZX:
	case BSVM_SOP_EXCH_ZXY: case BSVM_SOP_EXCH_ZYX:
		BSVM_J2StackOp_CompoundExch(ctx, i);
		break;

	case BSVM_SOP_EXCH_WXYZ: case BSVM_SOP_EXCH_WXZY:
	case BSVM_SOP_EXCH_WYXZ: case BSVM_SOP_EXCH_WYZX:
	case BSVM_SOP_EXCH_WZXY: case BSVM_SOP_EXCH_WZYX:
	case BSVM_SOP_EXCH_XWYZ: case BSVM_SOP_EXCH_XWZY:
	case BSVM_SOP_EXCH_XYWZ: case BSVM_SOP_EXCH_XYZW:
	case BSVM_SOP_EXCH_XZWY: case BSVM_SOP_EXCH_XZYW:
	case BSVM_SOP_EXCH_YWXZ: case BSVM_SOP_EXCH_YWZX:
	case BSVM_SOP_EXCH_YXWZ: case BSVM_SOP_EXCH_YXZW:
	case BSVM_SOP_EXCH_YZWX: case BSVM_SOP_EXCH_YZXW:
	case BSVM_SOP_EXCH_ZWXY: case BSVM_SOP_EXCH_ZWYX:
	case BSVM_SOP_EXCH_ZXWY: case BSVM_SOP_EXCH_ZXYW:
	case BSVM_SOP_EXCH_ZYWX: case BSVM_SOP_EXCH_ZYXW:
		BSVM_J2StackOp_CompoundExch(ctx, i);
		break;

	case BSVM_SOP_INC_FN:
		BSVM_J2StackOp_IncrementFn(ctx, 1); break;
	case BSVM_SOP_DEC_FN:
		BSVM_J2StackOp_DecrementFn(ctx, 1); break;
	case BSVM_SOP_INC2_FN:
		BSVM_J2StackOp_IncrementFn(ctx, 2); break;
	case BSVM_SOP_DEC2_FN:
		BSVM_J2StackOp_DecrementFn(ctx, 2); break;

	case BSVM_SOP_ADD_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_ADD); break;
	case BSVM_SOP_SUB_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_SUB); break;
	case BSVM_SOP_MUL_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_MUL); break;
	case BSVM_SOP_DIV_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_DIV); break;
	case BSVM_SOP_IDIV_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_IDIV); break;
	case BSVM_SOP_MOD_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_MOD); break;
	case BSVM_SOP_AND_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_AND); break;
	case BSVM_SOP_OR_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_OR); break;
	case BSVM_SOP_XOR_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_XOR); break;

	case BSVM_SOP_CMP_L_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_L); break;
	case BSVM_SOP_CMP_G_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_G); break;
	case BSVM_SOP_CMP_LE_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_LE); break;
	case BSVM_SOP_CMP_GE_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_GE); break;
	case BSVM_SOP_CMP_E_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_EQV); break;
	case BSVM_SOP_CMP_NE_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_NEQV); break;

	case BSVM_SOP_ADD_FN_C:
		i=*ctx->ip++;
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_ADD, i);
		break;
	case BSVM_SOP_SUB_FN_C:
		i=*ctx->ip++;
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_SUB, i);
		break;
	case BSVM_SOP_MUL_FN_C:
		i=*ctx->ip++;
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_MUL, i);
		break;

	case BSVM_SOP_SHL_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_SHL); break;
	case BSVM_SOP_SHR_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_SHR); break;
	case BSVM_SOP_EXP_FN:
		BSVM_J2StackOp_BinaryOpFn(ctx, BSVM_JIT2_BINOP_EXP); break;

	case BSVM_SOP_SHL_FN_C:
		i=*ctx->ip++;
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_SHL, i);
		break;
	case BSVM_SOP_SHR_FN_C:
		i=*ctx->ip++;
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_SHR, i);
		break;
	case BSVM_SOP_EXP_FN_C:
		i=*ctx->ip++;
		BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_EXP, i);
		break;


	case BSVM_SOP_CONV_FN2FL: BSVM_J2StackOp_ConvFn2Fl(ctx); break;
	case BSVM_SOP_CONV_FL2FN: BSVM_J2StackOp_ConvFl2Fn(ctx); break;

	case BSVM_SOP_ADD_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_ADD); break;
	case BSVM_SOP_SUB_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_SUB); break;
	case BSVM_SOP_MUL_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_MUL); break;
	case BSVM_SOP_DIV_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_DIV); break;
	case BSVM_SOP_CMP_L_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_L); break;
	case BSVM_SOP_CMP_G_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_G); break;
	case BSVM_SOP_CMP_LE_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_LE); break;
	case BSVM_SOP_CMP_GE_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_GE); break;
	case BSVM_SOP_CMP_E_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_EQV); break;
	case BSVM_SOP_CMP_NE_FL:
		BSVM_J2StackOp_BinaryOpFl(ctx, BSVM_JIT2_BINOP_NEQV); break;
	case BSVM_SOP_NEG_FL:
		BSVM_J2StackOp_UnaryOpFl(ctx, BSVM_JIT2_UNOP_NEG); break;

	case BSVM_SOP_JMP_E_FN:
		i=BSVM_J2Core_ReadShort(ctx);
		break;
	case BSVM_SOP_JMP_NE_FN:
		i=BSVM_J2Core_ReadShort(ctx);
		break;
	case BSVM_SOP_JMP_E_FL:
		i=BSVM_J2Core_ReadShort(ctx);
		break;
	case BSVM_SOP_JMP_NE_FL:
		i=BSVM_J2Core_ReadShort(ctx);
		break;

	case BSVM_SOP_JMP_L_FN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFn(ctx, BSVM_JIT2_BINOP_L, s);
		break;

	case BSVM_SOP_JMP_G_FN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFn(ctx, BSVM_JIT2_BINOP_G, s);
		break;
	case BSVM_SOP_JMP_LE_FN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFn(ctx, BSVM_JIT2_BINOP_LE, s);
		break;
	case BSVM_SOP_JMP_GE_FN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFn(ctx, BSVM_JIT2_BINOP_GE, s);
		break;

	case BSVM_SOP_JMP_L_FL:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFl(ctx, BSVM_JIT2_BINOP_L, s);
		break;
	case BSVM_SOP_JMP_G_FL:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFl(ctx, BSVM_JIT2_BINOP_G, s);
		break;
	case BSVM_SOP_JMP_LE_FL:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFl(ctx, BSVM_JIT2_BINOP_LE, s);
		break;
	case BSVM_SOP_JMP_GE_FL:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFl(ctx, BSVM_JIT2_BINOP_GE, s);
		break;

	case BSVM_SOP_JMP_E_LFN:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFn(
			ctx, BSVM_JIT2_BINOP_EQV, s, j, k);
		break;
	case BSVM_SOP_JMP_NE_LFN:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFn(
			ctx, BSVM_JIT2_BINOP_NEQV, s, j, k);
		break;

	case BSVM_SOP_JMP_L_LFN:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFn(
			ctx, BSVM_JIT2_BINOP_L, s, j, k);
		break;
	case BSVM_SOP_JMP_G_LFN:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFn(
			ctx, BSVM_JIT2_BINOP_G, s, j, k);
		break;
	case BSVM_SOP_JMP_LE_LFN:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFn(
			ctx, BSVM_JIT2_BINOP_LE, s, j, k);
		break;
	case BSVM_SOP_JMP_GE_LFN:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFn(
			ctx, BSVM_JIT2_BINOP_GE, s, j, k);
		break;

	case BSVM_SOP_JMP_E_ZFN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_EQV, s, 0);
		break;
	case BSVM_SOP_JMP_NE_ZFN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_NEQV, s, 0);
		break;
	case BSVM_SOP_JMP_L_ZFN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_L, s, 0);
		break;
	case BSVM_SOP_JMP_G_ZFN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_G, s, 0);
		break;
	case BSVM_SOP_JMP_LE_ZFN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_LE, s, 0);
		break;
	case BSVM_SOP_JMP_GE_ZFN:
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_GE, s, 0);
		break;

	case BSVM_SOP_JMP_E_LZFN:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_EQV, s, j, 0);
		break;
	case BSVM_SOP_JMP_NE_LZFN:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_NEQV, s, j, 0);
		break;
	case BSVM_SOP_JMP_L_LZFN:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_L, s, j, 0);
		break;
	case BSVM_SOP_JMP_G_LZFN:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_G, s, j, 0);
		break;
	case BSVM_SOP_JMP_LE_LZFN:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_LE, s, j, 0);
		break;
	case BSVM_SOP_JMP_GE_LZFN:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_GE, s, j, 0);
		break;

	case BSVM_SOP_JMP_E_FNC:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_EQV, s, j);
		break;
	case BSVM_SOP_JMP_NE_FNC:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_NEQV, s, j);
		break;
	case BSVM_SOP_JMP_L_FNC:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_L, s, j);
		break;
	case BSVM_SOP_JMP_G_FNC:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_G, s, j);
		break;
	case BSVM_SOP_JMP_LE_FNC:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_LE, s, j);
		break;
	case BSVM_SOP_JMP_GE_FNC:
		j=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryFnC(
			ctx, BSVM_JIT2_BINOP_GE, s, j);
		break;

	case BSVM_SOP_JMP_E_LFNC:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_EQV, s, j, k);
		break;
	case BSVM_SOP_JMP_NE_LFNC:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_NEQV, s, j, k);
		break;
	case BSVM_SOP_JMP_L_LFNC:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_L, s, j, k);
		break;
	case BSVM_SOP_JMP_G_LFNC:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_G, s, j, k);
		break;
	case BSVM_SOP_JMP_LE_LFNC:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_LE, s, j, k);
		break;
	case BSVM_SOP_JMP_GE_LFNC:
		j=*ctx->ip++; k=*ctx->ip++;
		i=BSVM_J2Core_ReadShort(ctx);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2StackOp_DoJumpCondBinaryLexFnC(
			ctx, BSVM_JIT2_BINOP_GE, s, j, k);
		break;

	default:
		BSVM_Jit2_StubError(ctx);
		break;
	}

	return(0);
}


void BSVM_J2Core_CompileBlock(bsjContext ctx, BSVM_SVMBlock *fcn)
{
	dyt t, c;
	byte *ip;
	char *buf, *log;

	BSVM_J2Core_Init();

	//don't bother with top-level expressions or anonymous closures
//	if(!dysymbolp(fcn->name))
//		return;

//	gcset(fcn->th_name, dysym(BSVM_J2Core_GenSym()));

	ctx->lit=fcn->lit;
//	ctx->args=fcn->args;
//	ctx->local=NULL;
	ctx->func=(dyt)fcn;
	ctx->blk=fcn;	//counter-intuitive...
	ctx->n_lbl=0;

	c=fcn->args;
	while(c)
	{
		t=dycar(c);
		if(dysymbolp(t))
		{
			BSVM_J2Vars_LexBindArgSig(ctx, dysymbolv(t), "r", NULL);
			c=dycdr(c);
			continue;
		}
		c=dycdr(c);
	}

	ctx->ips=fcn->body;
	ctx->ipe=fcn->body+fcn->szbody;
	ctx->ip=fcn->body;
	while(ctx->ip<ctx->ipe)
		BSVM_J2Core_PreStep(ctx);

//	BASM_BeginAssembly(BSVM_TOSYM(fcn->th_name));
//	ip=BASM_GetIP();

//	basm_print("; Func '%s'->'%s'\n",
//		BSVM_TOSYM(fcn->name), BSVM_TOSYM(fcn->th_name));
//	basm_print("%s:\n", BSVM_TOSYM(fcn->th_name));
//	BSVM_J2Core_Entry(ctx);
//	basm_print("\n");

	while(!BSVM_Jit2_ContextOutputStable(ctx))
	{
		BSVM_Jit2_BeginFrame(ctx);
//		ctx->ip=fcn->body;
//		ctx->ipe=fcn->body+fcn->szbody;
		ctx->ip=ctx->ips;
		while(ctx->ip<ctx->ipe)
			BSVM_J2Core_Step(ctx);
		BSVM_Jit2_EndFrame(ctx);
	}

	buf=BSVM_Jit2_GetOutputAsm(ctx);
	log=BSVM_Jit2_GetOutputLog(ctx);

	printf("BSVM_J2Core_CompileBlock ASM:\n%s\n", buf);
	printf("BSVM_J2Core_CompileBlock Log:\n%s\n", log);

//	ip=BASM_EndAssembly();
//	fcn->thunk=ip;
}

BSVM_SVMBlock *BSVM_J2Core_GetBlockListR(
	BSVM_SVMBlock *fcn, BSVM_SVMBlock *lst)
{
	BSVM_SVMBlock *cur;
	dyt t;
	int i, n;
	
	//check if this block is already in list
	cur=lst;
	while(cur)
	{
		if(cur==fcn)break;
		cur=cur->chain;
	}
	if(cur)return(lst);
	
	//ok, add it...
	fcn->chain=lst;
	lst=fcn;
	
	if(dyarrayp(fcn->lit))
	{
		n=dyarraysz(fcn->lit);
		for(i=0; i<n; i++)
		{
			t=dyarrayidx(fcn->lit, i);
			if(dytypep(t, "svm_blk_t"))
			{
				cur=(BSVM_SVMBlock *)t;
				lst=BSVM_J2Core_GetBlockListR(cur, lst);
			}
		}
	}
	
	return(lst);
}

BSVM_SVMBlock *BSVM_J2Core_GetBlockList(BSVM_SVMBlock *fcn)
{
	return(BSVM_J2Core_GetBlockListR(fcn, NULL));
}

void BSVM_J2Core_TestJitBlock(BSVM_SVMBlock *fcn)
{
	bsjTopContext tctx;
	bsjContext ctx;
	BSVM_SVMBlock *blklst;
	BSVM_SVMBlock *cur;
	
	blklst=BSVM_J2Core_GetBlockList(fcn);
	
	tctx=BSVM_Jit2_AllocTopContext();
	tctx->arch=BSVM_JIT2_ARCH_X86;

//	ctx=BSVM_Jit2_AllocContext(tctx);
//	ctx->abitype=BSVM_JIT2_ABITY_BSVMI;
//	BSVM_J2Core_CompileBlock(ctx, fcn);
	
	cur=blklst;
	while(cur)
	{
		ctx=BSVM_Jit2_AllocContext(tctx);
		ctx->abitype=BSVM_JIT2_ABITY_BSVMI;
		BSVM_J2Core_CompileBlock(ctx, cur);
		cur=cur->chain;
	}
}
