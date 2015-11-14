#include <bgbsvm.h>

void BSVM_ThJitX86_LoadStack(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(stack));
}

void BSVM_ThJitX86_StackPopEDAX(BSVM_ThreadJitContext *ctx)
{
//	BSVM_ThJitX86_LoadStack(ctx, tr, op);
	dyllAsmPrint("dec ecx\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov eax, [edi+ecx*8+0]\n");
	dyllAsmPrint("mov edx, [edi+ecx*8+4]\n");
}

void BSVM_ThJitX86_StackPushEDAX(BSVM_ThreadJitContext *ctx)
{
//	BSVM_ThJitX86_LoadStack(ctx, tr, op);
	dyllAsmPrint("mov [edi+ecx*8+0], eax\n");
	dyllAsmPrint("mov [edi+ecx*8+4], edx\n");

	dyllAsmPrint("inc ecx\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));
}

void BSVM_ThJitX86_StackGetTopIndexEDAX(BSVM_ThreadJitContext *ctx, int idx)
{
//	BSVM_ThJitX86_LoadStack(ctx, tr, op);
	dyllAsmPrint("mov eax, [edi+ecx*8%+d]\n", -(idx+1)*8+0);
	dyllAsmPrint("mov edx, [edi+ecx*8%+d]\n", -(idx+1)*8+4);
}

void BSVM_ThJitX86_StackSetTopIndexEDAX(BSVM_ThreadJitContext *ctx, int idx)
{
//	BSVM_ThJitX86_LoadStack(ctx, tr, op);
	dyllAsmPrint("mov [edi+ecx*8%+d], eax\n", -(idx+1)*8+0);
	dyllAsmPrint("mov [edi+ecx*8%+d], edx\n", -(idx+1)*8+4);
}

void BSVM_ThJitX86_StackGetTopEDAX(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThJitX86_StackGetTopIndexEDAX(ctx, 0);
}

void BSVM_ThJitX86_StackSetTopEDAX(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThJitX86_StackSetTopIndexEDAX(ctx, 0);
}


void BSVM_ThJitX86_LoadVarStack(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(lestackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(levstack));
}

void BSVM_ThJitX86_VarStackGetIndexEDAX(BSVM_ThreadJitContext *ctx, int idx)
{
//	BSVM_ThJitX86_LoadVarStack(ctx, tr, op);
	dyllAsmPrint("mov eax, [edi+ecx*8%+d]\n", -(idx+1)*8+0);
	dyllAsmPrint("mov edx, [edi+ecx*8%+d]\n", -(idx+1)*8+4);
}

void BSVM_ThJitX86_VarStackSetIndexEDAX(BSVM_ThreadJitContext *ctx, int idx)
{
//	BSVM_ThJitX86_LoadVarStack(ctx, tr, op);
	dyllAsmPrint("mov [edi+ecx*8%+d], eax\n", -(idx+1)*8+0);
	dyllAsmPrint("mov [edi+ecx*8%+d], edx\n", -(idx+1)*8+4);
}

bool BSVM_ThJitX86_PUSH_XI(BSVM_ThreadJitContext *ctx)
{
	u64 l;

	l=ctx->thop->t.v;

	BSVM_ThJitX86_LoadStack(ctx);
	dyllAsmPrint("mov eax, 0x%08X\n", (u32)l);
	dyllAsmPrint("mov edx, 0x%08X\n", (u32)(l>>32));
	BSVM_ThJitX86_StackPushEDAX(ctx);

	return(true);
}

bool BSVM_ThJitX86_PUSH_XI_C(BSVM_ThreadJitContext *ctx)
{
	u64 l;

	l=((u64)((s64)ctx->thop->i)) ^ 0x1000000000000000ULL;

	BSVM_ThJitX86_LoadStack(ctx);
	dyllAsmPrint("mov eax, 0x%08X\n", (u32)l);
	dyllAsmPrint("mov edx, 0x%08X\n", (u32)(l>>32));
	BSVM_ThJitX86_StackPushEDAX(ctx);

	return(true);
}

bool BSVM_ThJitX86_ADD_XI(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThJitX86_LoadStack(ctx);

//	dyllAsmPrint("mov [esp+8], eax\n");

	BSVM_ThJitX86_StackPopEDAX(ctx);
	dyllAsmPrint("mov [esp+8], eax\n");
	BSVM_ThJitX86_StackGetTopEDAX(ctx);
	dyllAsmPrint("add eax, [esp+8]\n");
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");
	BSVM_ThJitX86_StackSetTopEDAX(ctx);

//	dyllAsmPrint("mov eax, [esp+8]\n");
//	dyllAsmPrint("mov eax, [eax+%d]\n", BSVM_THOP_OFFS(next));

	return(true);
}

bool BSVM_ThJitX86_SUB_XI(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThJitX86_LoadStack(ctx);

//	dyllAsmPrint("mov [esp+8], eax\n");

	BSVM_ThJitX86_StackPopEDAX(ctx);
	dyllAsmPrint("mov [esp+8], eax\n");
	BSVM_ThJitX86_StackGetTopEDAX(ctx);
	dyllAsmPrint("sub eax, [esp+8]\n");
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");
	BSVM_ThJitX86_StackSetTopEDAX(ctx);

//	dyllAsmPrint("mov eax, [esp+8]\n");
//	dyllAsmPrint("mov eax, [eax+%d]\n", BSVM_THOP_OFFS(next));

	return(true);
}

bool BSVM_ThJitX86_MUL_XI(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThJitX86_LoadStack(ctx);

//	dyllAsmPrint("mov [esp+8], eax\n");

	BSVM_ThJitX86_StackPopEDAX(ctx);
	dyllAsmPrint("mov [esp+8], eax\n");
	BSVM_ThJitX86_StackGetTopEDAX(ctx);
	dyllAsmPrint("imul eax, [esp+8]\n");
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");
	BSVM_ThJitX86_StackSetTopEDAX(ctx);

//	dyllAsmPrint("mov eax, [esp+8]\n");
//	dyllAsmPrint("mov eax, [eax+%d]\n", BSVM_THOP_OFFS(next));

	return(true);
}

bool BSVM_ThJitX86_ArithOpr_XI(BSVM_ThreadJitContext *ctx, char *opr)
{
	BSVM_ThJitX86_LoadStack(ctx);

//	dyllAsmPrint("mov [esp+8], eax\n");

	BSVM_ThJitX86_StackPopEDAX(ctx);
	dyllAsmPrint("mov [esp+8], eax\n");
	BSVM_ThJitX86_StackGetTopEDAX(ctx);
//	dyllAsmPrint("imul eax, [esp+8]\n");
	dyllAsmPrint("%s eax, [esp+8]\n", opr);
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");
	BSVM_ThJitX86_StackSetTopEDAX(ctx);

//	dyllAsmPrint("mov eax, [esp+8]\n");
//	dyllAsmPrint("mov eax, [eax+%d]\n", BSVM_THOP_OFFS(next));

	return(true);
}

bool BSVM_ThJitX86_AND_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI(ctx, "and")); }
bool BSVM_ThJitX86_OR_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI(ctx, "or")); }
bool BSVM_ThJitX86_XOR_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI(ctx, "xor")); }

bool BSVM_ThJitX86_SHL_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI(ctx, "shl")); }
bool BSVM_ThJitX86_SHR_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI(ctx, "sar")); }
bool BSVM_ThJitX86_SHRR_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI(ctx, "shr")); }


bool BSVM_ThJitX86_ArithOpr_XI_C(BSVM_ThreadJitContext *ctx, char *opr)
{
	BSVM_ThJitX86_LoadStack(ctx);

	BSVM_ThJitX86_StackGetTopEDAX(ctx);
	dyllAsmPrint("%s eax, %d\n", opr, ctx->thop->i);
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");
	BSVM_ThJitX86_StackSetTopEDAX(ctx);

	return(true);
}

bool BSVM_ThJitX86_ADD_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "add")); }
bool BSVM_ThJitX86_SUB_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "sub")); }
bool BSVM_ThJitX86_MUL_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "imul")); }

bool BSVM_ThJitX86_AND_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "and")); }
bool BSVM_ThJitX86_OR_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "or")); }
bool BSVM_ThJitX86_XOR_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "xor")); }

bool BSVM_ThJitX86_SHL_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "shl")); }
bool BSVM_ThJitX86_SHR_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "sar")); }
bool BSVM_ThJitX86_SHRR_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_ArithOpr_XI_C(ctx, "shr")); }

bool BSVM_ThJitX86_LLOAD_VS(BSVM_ThreadJitContext *ctx)
{
	// mov esi, [ebp+...]    //VM context
	// mov edi, [esi+...]    //value-stack
	// mov edx, [esi+...]    //local-stack
	// mov eax, [esi+...]    //local-stack pos
	// mov ecx, [esi+...]    //value-stack pos
	// movq xmm0, [edx+eax*8-...]
	// movq [edi+ecx*8], xmm0
	// inc ecx
	// mov [esi+...], ecx

//	dyllAsmPrint("int 3\n");

	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(stack));
	dyllAsmPrint("mov eax, [esi+%d]\n", BSVM_CTX_OFFS(lestackpos));
	dyllAsmPrint("mov edx, [esi+%d]\n", BSVM_CTX_OFFS(levstack));

	dyllAsmPrint("movq xmm0, [edx+eax*8%+d]\n", -(ctx->thop->i+1)*8+0);
	dyllAsmPrint("movq [edi+ecx*8+0], xmm0\n");

	dyllAsmPrint("inc ecx\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));

//	BSVM_ThJitX86_LoadVarStack(ctx);
//	BSVM_ThJitX86_VarStackGetIndexEDAX(ctx, ctx->thop->i);

//	BSVM_ThJitX86_LoadStack(ctx);
//	BSVM_ThJitX86_StackPushEDAX(ctx);

//	BSVM_ThJitX86_StackPopEDAX(ctx);
//	dyllAsmPrint("mov [esp+8], eax\n");
//	BSVM_ThJitX86_StackGetTopEDAX(ctx);
//	dyllAsmPrint("imul eax, [esp+8]\n");
//	dyllAsmPrint("cdq\n");
//	dyllAsmPrint("xor edx, 0x10000000\n");
//	BSVM_ThJitX86_StackSetTopEDAX(ctx);

	return(true);
}

bool BSVM_ThJitX86_LLOAD_XI_VS(BSVM_ThreadJitContext *ctx)
{
	return(BSVM_ThJitX86_LLOAD_VS(ctx));
}

bool BSVM_ThJitX86_LSTORE_VS(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(stack));
	dyllAsmPrint("mov eax, [esi+%d]\n", BSVM_CTX_OFFS(lestackpos));
	dyllAsmPrint("mov edx, [esi+%d]\n", BSVM_CTX_OFFS(levstack));

	dyllAsmPrint("dec ecx\n");

	dyllAsmPrint("movq xmm0, [edi+ecx*8+0]\n");
	dyllAsmPrint("movq [edx+eax*8%+d], xmm0\n", -(ctx->thop->i+1)*8+0);

//	dyllAsmPrint("inc ecx\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));

//	BSVM_ThJitX86_LoadStack(ctx);
//	BSVM_ThJitX86_StackPopEDAX(ctx);

//	BSVM_ThJitX86_LoadVarStack(ctx);
//	BSVM_ThJitX86_VarStackSetIndexEDAX(ctx, ctx->thop->i);

	return(true);
}

bool BSVM_ThJitX86_LSTORE_XI_VS(BSVM_ThreadJitContext *ctx)
{
	return(BSVM_ThJitX86_LSTORE_VS(ctx));
}


bool BSVM_ThJitX86_LINC_XI_VS(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(lestackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(levstack));

	dyllAsmPrint("mov eax, [edi+ecx*8%+d]\n", -(ctx->thop->i+1)*8+0);
	dyllAsmPrint("inc eax\n");
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");
	dyllAsmPrint("mov [edi+ecx*8%+d], eax\n", -(ctx->thop->i+1)*8+0);
	dyllAsmPrint("mov [edi+ecx*8%+d], edx\n", -(ctx->thop->i+1)*8+4);

	return(true);
}

bool BSVM_ThJitX86_LDEC_XI_VS(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(lestackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(levstack));

	dyllAsmPrint("mov eax, [edi+ecx*8%+d]\n", -(ctx->thop->i+1)*8+0);
	dyllAsmPrint("dec eax\n");
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");
	dyllAsmPrint("mov [edi+ecx*8%+d], eax\n", -(ctx->thop->i+1)*8+0);
	dyllAsmPrint("mov [edi+ecx*8%+d], edx\n", -(ctx->thop->i+1)*8+4);

	return(true);
}

bool BSVM_ThJitX86_FN(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov [esi+%d], 0x%08X\n",
		BSVM_CTX_OFFS(dbg_fn), dytfStringvF(ctx->thop->t));
	return(true);
}

bool BSVM_ThJitX86_LN(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov [esi+%d], %d\n",
		BSVM_CTX_OFFS(dbg_ln), ctx->thop->i);
	return(true);
}

bool BSVM_ThJitX86_DBGMARK(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov [esi+%d], 0x%08X\n",
		BSVM_CTX_OFFS(dbg_fn), dytfStringvF(ctx->thop->t));
	dyllAsmPrint("mov [esi+%d], %d\n",
		BSVM_CTX_OFFS(dbg_ln), ctx->thop->i);
	return(true);
}

bool BSVM_ThJitX86_LOADINDEX_XUBI_IX(BSVM_ThreadJitContext *ctx, int flags)
	{ return(BSVM_ThJitX86_LOADINDEX_MovI_IX(ctx, "movzxb", 1, flags)); }
bool BSVM_ThJitX86_LOADINDEX_XSBI_IX(BSVM_ThreadJitContext *ctx, int flags)
	{ return(BSVM_ThJitX86_LOADINDEX_MovI_IX(ctx, "movsxb", 1, flags)); }
bool BSVM_ThJitX86_LOADINDEX_XUSI_IX(BSVM_ThreadJitContext *ctx, int flags)
	{ return(BSVM_ThJitX86_LOADINDEX_MovI_IX(ctx, "movzxw", 2, flags)); }
bool BSVM_ThJitX86_LOADINDEX_XSSI_IX(BSVM_ThreadJitContext *ctx, int flags)
	{ return(BSVM_ThJitX86_LOADINDEX_MovI_IX(ctx, "movsxw", 2, flags)); }

bool BSVM_ThJitX86_LOADINDEX_XUDI_IX(BSVM_ThreadJitContext *ctx, int flags)
	{ return(BSVM_ThJitX86_LOADINDEX_MovI_IX(ctx, "mov", 4, flags)); }
bool BSVM_ThJitX86_LOADINDEX_XSDI_IX(BSVM_ThreadJitContext *ctx, int flags)
	{ return(BSVM_ThJitX86_LOADINDEX_MovI_IX(ctx, "mov", 4, flags)); }

bool BSVM_ThJitX86_LOADINDEX_MovI_IX(BSVM_ThreadJitContext *ctx,
	char *mov, int step, int flags)
{
	char *s0, *s1, *s2;

	if(flags&BSVM_THJIT_OPFL_SAFETYCHECK)
	{
		s0=dyllGenSym();
		s1=dyllGenSym();
		s2=dyllGenSym();
	}

	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(stack));

	dyllAsmPrint("mov edx, [edi+ecx*8-16]\n");	//array
	dyllAsmPrint("mov eax, [edi+ecx*8-8]\n");	//index

	if(flags&BSVM_THJIT_OPFL_SAFETYCHECK)
	{
		dyllAsmPrint("and edx, edx\n");
		dyllAsmPrint("jz %s\n", s2);
		dyllAsmPrint("add eax, [edx+%d]\n", BSVM_ARRAY_OFFS(offs));
		dyllAsmPrint("cmp eax, [edx+%d]\n", BSVM_ARRAY_OFFS(cnt));
		dyllAsmPrint("jnbe %s\n", s0);
	}else
	{
		dyllAsmPrint("add eax, [edx+%d]\n", BSVM_ARRAY_OFFS(offs));
	}

	dyllAsmPrint("mov edx, [edx+%d]\n", BSVM_ARRAY_OFFS(data));
//	dyllAsmPrint("mov eax, [edx+eax*4+0]\n");	//array element
	dyllAsmPrint("%s eax, [edx+eax*%d+0]\n",
		mov, step);	//array element

//	dyllAsmPrint("dec eax\n");
	dyllAsmPrint("cdq\n");
	dyllAsmPrint("xor edx, 0x10000000\n");

	dyllAsmPrint("dec ecx\n");

	dyllAsmPrint("mov [edi+ecx*8-8], eax\n");
	dyllAsmPrint("mov [edi+ecx*8-4], edx\n");

//	dyllAsmPrint("movq xmm0, [edx+eax*8%+d]\n", -(ctx->thop->i+1)*8+0);
//	dyllAsmPrint("movq [edi+ecx*8+0], xmm0\n");

//	dyllAsmPrint("inc ecx\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));

	if(flags&BSVM_THJIT_OPFL_SAFETYCHECK)
	{
		dyllAsmPrint("jmp %s\n", s1);
		dyllAsmPrint("%s:\n", s0);
		dyllAsmPrint("mov [esi+%d], %d\n",
			BSVM_CTX_OFFS(rs), BSVM_RETS_BOUNDSCHK);
		dyllAsmPrint("jmp %s\n", s1);

		dyllAsmPrint("%s:\n", s2);
		dyllAsmPrint("mov [esi+%d], %d\n",
			BSVM_CTX_OFFS(rs), BSVM_RETS_NULLCHK);
		dyllAsmPrint("jmp %s\n", s1);

		dyllAsmPrint("%s:\n", s1);
	}

	return(true);
}


bool BSVM_ThJitX86_LoadIndex(
	BSVM_ThreadJitContext *ctx)
{
	bool ret;

	ret=false;
	if(ctx->thop->pfhint&BSVM_PF_ARR_IX)
	{
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUBI)
			{ ret=BSVM_ThJitX86_LOADINDEX_XUBI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSBI)
			{ ret=BSVM_ThJitX86_LOADINDEX_XSBI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUSI)
			{ ret=BSVM_ThJitX86_LOADINDEX_XUSI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSSI)
			{ ret=BSVM_ThJitX86_LOADINDEX_XSSI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDI)
			{ ret=BSVM_ThJitX86_LOADINDEX_XUDI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDI)
			{ ret=BSVM_ThJitX86_LOADINDEX_XSDI_IX(ctx, 0); return(ret); }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDL)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XUDL_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDL)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSDL_IX; return; }

//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XII)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSDI_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XLL)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XLL_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XFF)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XFF_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XDD)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XDD_IX; return; }
	}
#if 0
	else
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_ARR_XUBI: case BSVM_PF_ARR_XSBI:
		case BSVM_PF_ARR_XUSI: case BSVM_PF_ARR_XSSI:
		case BSVM_PF_ARR_XUDI: case BSVM_PF_ARR_XSDI:
		case BSVM_PF_ARR_XII: case BSVM_PF_HINT_XI:
			thop->fcn=BSVM_ThOp_LOADINDEX_XI;
			break;
		case BSVM_PF_ARR_XUDL: case BSVM_PF_ARR_XSDL:
		case BSVM_PF_ARR_XLL: case BSVM_PF_HINT_XL:
			thop->fcn=BSVM_ThOp_LOADINDEX_XL;
			break;
		case BSVM_PF_ARR_XFF: case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_LOADINDEX_XF;
			break;
		case BSVM_PF_ARR_XDD: case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_LOADINDEX_XD;
			break;
		default:
			thop->fcn=BSVM_ThOp_LOADINDEX;
			return;
		}
		return;
	}

	thop->fcn=BSVM_ThOp_LOADINDEX;
	return;
#endif

	return(ret);
}

bool BSVM_ThJitX86_STOREINDEX_XUBI_IX(
	BSVM_ThreadJitContext *ctx, int flags)
{
	return(BSVM_ThJitX86_STOREINDEX_StepI_IX(ctx, 1, flags));
}

bool BSVM_ThJitX86_STOREINDEX_XSBI_IX(
	BSVM_ThreadJitContext *ctx, int flags)
{
	return(BSVM_ThJitX86_STOREINDEX_StepI_IX(ctx, 1, flags));
}

bool BSVM_ThJitX86_STOREINDEX_XUSI_IX(
	BSVM_ThreadJitContext *ctx, int flags)
{
	return(BSVM_ThJitX86_STOREINDEX_StepI_IX(ctx, 2, flags));
}

bool BSVM_ThJitX86_STOREINDEX_XSSI_IX(
	BSVM_ThreadJitContext *ctx, int flags)
{
	return(BSVM_ThJitX86_STOREINDEX_StepI_IX(ctx, 2, flags));
}

bool BSVM_ThJitX86_STOREINDEX_XUDI_IX(
	BSVM_ThreadJitContext *ctx, int flags)
{
	return(BSVM_ThJitX86_STOREINDEX_StepI_IX(ctx, 4, flags));
}

bool BSVM_ThJitX86_STOREINDEX_XSDI_IX(
	BSVM_ThreadJitContext *ctx, int flags)
{
	return(BSVM_ThJitX86_STOREINDEX_StepI_IX(ctx, 4, flags));
}

bool BSVM_ThJitX86_STOREINDEX_StepI_IX(
	BSVM_ThreadJitContext *ctx, int step, int flags)
{
	char *s0, *s1, *s2;

	if(flags&BSVM_THJIT_OPFL_SAFETYCHECK)
	{
		s0=dyllGenSym();
		s1=dyllGenSym();
		s2=dyllGenSym();
	}

	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(stack));

	dyllAsmPrint("mov edx, [edi+ecx*8-16]\n");	//array
	dyllAsmPrint("mov eax, [edi+ecx*8-8]\n");	//index

	if(flags&BSVM_THJIT_OPFL_SAFETYCHECK)
	{
		dyllAsmPrint("and edx, edx\n");
		dyllAsmPrint("jz %s\n", s2);
		dyllAsmPrint("add eax, [edx+%d]\n", BSVM_ARRAY_OFFS(offs));
		dyllAsmPrint("cmp eax, [edx+%d]\n", BSVM_ARRAY_OFFS(cnt));
		dyllAsmPrint("jnbe %s\n", s0);
	}else
	{
		dyllAsmPrint("add eax, [edx+%d]\n", BSVM_ARRAY_OFFS(offs));
	}

	dyllAsmPrint("mov edx, [edx+%d]\n", BSVM_ARRAY_OFFS(data));

//	dyllAsmPrint("mov eax, [edx+eax*4+0]\n");			//array element
//	dyllAsmPrint("lea edx, [edx+eax*4+0]\n");			//element addr
	dyllAsmPrint("lea edx, [edx+eax*%d+0]\n", step);	//element addr

	dyllAsmPrint("mov eax, [edi+ecx*8-24]\n");	//value
//	dyllAsmPrint("mov [edx], eax\n");			//store value

	switch(step)
	{
	case 1:
		dyllAsmPrint("mov [edx], al\n");			//store value
		break;
	case 2:
		dyllAsmPrint("mov [edx], ax\n");			//store value
	case 4:
		dyllAsmPrint("mov [edx], eax\n");			//store value
	default:
		break;
	}

	dyllAsmPrint("sub ecx, 3\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));

	if(flags&BSVM_THJIT_OPFL_SAFETYCHECK)
	{
		dyllAsmPrint("jmp %s\n", s1);
		dyllAsmPrint("%s:\n", s0);
		dyllAsmPrint("mov [esi+%d], %d\n",
			BSVM_CTX_OFFS(rs), BSVM_RETS_BOUNDSCHK);
		dyllAsmPrint("jmp %s\n", s1);

		dyllAsmPrint("%s:\n", s2);
		dyllAsmPrint("mov [esi+%d], %d\n",
			BSVM_CTX_OFFS(rs), BSVM_RETS_NULLCHK);
		dyllAsmPrint("jmp %s\n", s1);

		dyllAsmPrint("%s:\n", s1);
	}

	return(true);
}

bool BSVM_ThJitX86_StoreIndex(
	BSVM_ThreadJitContext *ctx)
{
	bool ret;

	ret=false;
	if(ctx->thop->pfhint&BSVM_PF_ARR_IX)
	{
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUBI)
			{ ret=BSVM_ThJitX86_STOREINDEX_XUBI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSBI)
			{ ret=BSVM_ThJitX86_STOREINDEX_XSBI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUSI)
			{ ret=BSVM_ThJitX86_STOREINDEX_XUSI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSSI)
			{ ret=BSVM_ThJitX86_STOREINDEX_XSSI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDI)
			{ ret=BSVM_ThJitX86_STOREINDEX_XUDI_IX(ctx, 0); return(ret); }
		if((ctx->thop->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDI)
			{ ret=BSVM_ThJitX86_STOREINDEX_XSDI_IX(ctx, 0); return(ret); }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XUDL)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XUDL_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XSDL)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSDL_IX; return; }

//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XII)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XSDI_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XLL)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XLL_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XFF)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XFF_IX; return; }
//		if((ctx->pfhint&BSVM_PF_ARR_MASK)==BSVM_PF_ARR_XDD)
//			{ thop->fcn=BSVM_ThOp_LOADINDEX_XDD_IX; return; }
	}
#if 0
	else
	{
		switch(ctx->pfhint&BSVM_PF_ARR_MASK)
		{
		case BSVM_PF_ARR_XUBI: case BSVM_PF_ARR_XSBI:
		case BSVM_PF_ARR_XUSI: case BSVM_PF_ARR_XSSI:
		case BSVM_PF_ARR_XUDI: case BSVM_PF_ARR_XSDI:
		case BSVM_PF_ARR_XII: case BSVM_PF_HINT_XI:
			thop->fcn=BSVM_ThOp_LOADINDEX_XI;
			break;
		case BSVM_PF_ARR_XUDL: case BSVM_PF_ARR_XSDL:
		case BSVM_PF_ARR_XLL: case BSVM_PF_HINT_XL:
			thop->fcn=BSVM_ThOp_LOADINDEX_XL;
			break;
		case BSVM_PF_ARR_XFF: case BSVM_PF_HINT_XF:
			thop->fcn=BSVM_ThOp_LOADINDEX_XF;
			break;
		case BSVM_PF_ARR_XDD: case BSVM_PF_HINT_XD:
			thop->fcn=BSVM_ThOp_LOADINDEX_XD;
			break;
		default:
			thop->fcn=BSVM_ThOp_LOADINDEX;
			return;
		}
		return;
	}

	thop->fcn=BSVM_ThOp_LOADINDEX;
	return;
#endif

	return(ret);
}


bool BSVM_ThJitX86_JMP_Jcc_XI(BSVM_ThreadJitContext *ctx, char *opr)
{
	char *s0, *s1;

	if(!ctx->thop->data)
	{
		ctx->thop->data=(void *)BSVM_Thread_GetOpJumpTarget(
			ctx->vmctx, ctx->thop, ctx->thop->i);
		if(!ctx->thop->data)
			return(false);
	}

	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(stack));

	dyllAsmPrint("mov eax, [edi+ecx*8-16]\n");	//left
	dyllAsmPrint("mov edx, [edi+ecx*8-8]\n");	//right

	dyllAsmPrint("sub ecx, 2\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));

	s0=dyllGenSym();
	s1=dyllGenSym();
	dyllAsmPrint("cmp eax, edx\n");
//	dyllAsmPrint("jl %s\n", s0);
	dyllAsmPrint("%s %s\n", opr, s0);
	dyllAsmPrint("mov eax, 0x%08X\n", ctx->thop->data);
	dyllAsmPrint("jmp %s\n", s1);
	dyllAsmPrint("%s:\n", s0);
	dyllAsmPrint("mov eax, 0x%08X\n", ctx->thop->next);
	dyllAsmPrint("%s:\n", s1);

	return(true);
}

bool BSVM_ThJitX86_JMP_EQ_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI(ctx, "jne")); }
bool BSVM_ThJitX86_JMP_NE_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI(ctx, "je")); }
bool BSVM_ThJitX86_JMP_LT_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI(ctx, "jge")); }
bool BSVM_ThJitX86_JMP_GT_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI(ctx, "jle")); }
bool BSVM_ThJitX86_JMP_LE_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI(ctx, "jg")); }
bool BSVM_ThJitX86_JMP_GE_XI(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI(ctx, "jl")); }

bool BSVM_ThJitX86_JMP_Jcc_XI_C(BSVM_ThreadJitContext *ctx, char *opr)
{
	char *s0, *s1;

	if(!ctx->thop->data)
	{
		ctx->thop->data=(void *)BSVM_Thread_GetOpJumpTarget(
			ctx->vmctx, ctx->thop, ctx->thop->i);
		if(!ctx->thop->data)
			return(false);
	}

	dyllAsmPrint("mov esi, [ebp+8]\n");
	dyllAsmPrint("mov ecx, [esi+%d]\n", BSVM_CTX_OFFS(stackpos));
	dyllAsmPrint("mov edi, [esi+%d]\n", BSVM_CTX_OFFS(stack));

//	dyllAsmPrint("mov eax, [edi+ecx*8-16]\n");	//left
//	dyllAsmPrint("mov edx, [edi+ecx*8-8]\n");	//right

	dyllAsmPrint("mov eax, [edi+ecx*8-8]\n");	//left

//	dyllAsmPrint("sub ecx, 2\n");
	dyllAsmPrint("dec ecx\n");
	dyllAsmPrint("mov [esi+%d], ecx\n", BSVM_CTX_OFFS(stackpos));

	s0=dyllGenSym();
	s1=dyllGenSym();
//	dyllAsmPrint("cmp eax, edx\n");
	dyllAsmPrint("cmp eax, %d\n", ctx->thop->j);
//	dyllAsmPrint("jl %s\n", s0);
	dyllAsmPrint("%s %s\n", opr, s0);
	dyllAsmPrint("mov eax, 0x%08X\n", ctx->thop->data);
	dyllAsmPrint("jmp %s\n", s1);
	dyllAsmPrint("%s:\n", s0);
	dyllAsmPrint("mov eax, 0x%08X\n", ctx->thop->next);
	dyllAsmPrint("%s:\n", s1);

	return(true);
}

bool BSVM_ThJitX86_JMP_EQ_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI_C(ctx, "jne")); }
bool BSVM_ThJitX86_JMP_NE_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI_C(ctx, "je")); }
bool BSVM_ThJitX86_JMP_LT_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI_C(ctx, "jge")); }
bool BSVM_ThJitX86_JMP_GT_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI_C(ctx, "jle")); }
bool BSVM_ThJitX86_JMP_LE_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI_C(ctx, "jg")); }
bool BSVM_ThJitX86_JMP_GE_XI_C(BSVM_ThreadJitContext *ctx)
	{ return(BSVM_ThJitX86_JMP_Jcc_XI_C(ctx, "jl")); }

bool BSVM_ThJitX86_JMP(BSVM_ThreadJitContext *ctx)
{
	char *s0, *s1;

	if(!ctx->thop->data)
	{
		ctx->thop->data=(void *)BSVM_Thread_GetOpJumpTarget(
			ctx->vmctx, ctx->thop, ctx->thop->i);
		if(!ctx->thop->data)
			return(false);
	}

	dyllAsmPrint("mov eax, 0x%08X\n", ctx->thop->data);
	return(true);
}
