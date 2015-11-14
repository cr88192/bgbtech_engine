#include <bgbsvm.h>

BGBSVM_API void BSVM_J2FrameX86_BeginFrame(bsjContext ctx)
{
	char *s;
	int i, j, k, szp, szxp, ugr, uxr;

	bsj2_printlog(ctx, "BSVM_J2FrameX86_BeginFrame: Begin, pass=%d\n",
		ctx->n_passes);

	szp=BSVM_J2Arch_SizePointer(ctx);
	szxp=16;

	ugr=ctx->mask_usedregs & BSVM_J2RegX86_GRegSaveMask(ctx);
	uxr=ctx->mask_usedxregs & BSVM_J2RegX86_XRegSaveMask(ctx);

	j=0;
	for(i=0; i<16; i++)
	{
		if(ugr&(1<<i))
			j+=szp;
	}

	j=(j+(szxp-1))&(~(szxp-1));
	for(i=0; i<16; i++)
	{
		if(uxr&(1<<i))
			j+=szxp;
	}
	
	ctx->sz_savereg=j;
	ctx->sz_tvar=ctx->n_var*szp;
	
//	szp=BSVM_J2Arch_SizePointer(ctx);
	szp=BSVM_J2Arch_SizeStackAlign(ctx);
	i=0;
	i=(i-ctx->sz_savereg)&(~(szp-1));
	ctx->offs_savereg=i;
	i=(i-ctx->sz_local)&(~(szp-1));
	ctx->offs_local=i;
	i=(i-ctx->sz_tvar)&(~(szp-1));
	ctx->offs_tvar=i;
	i=(i-ctx->sz_vstack)&(~(szp-1));
	ctx->offs_vstack=i;
	i=(i-ctx->sz_targ)&(~(szp-1));
	ctx->offs_targ=i;

	ctx->sz_frame=-i;

	if(ctx->lbl_func)
		{ bsj2_print(ctx, "%s:\n", ctx->lbl_func); }

	bsj2_print(ctx, "push ebp\n");
	bsj2_print(ctx, "mov ebp, esp\n");

	if(ctx->sz_frame>0)
		bsj2_print(ctx, "sub esp, %d\n", ctx->sz_frame);

	j=0;
	for(i=0; i<16; i++)
	{
		if(ugr&(1<<i))
		{
			k=ctx->offs_savereg+j;
			s=J2RegX86_GetBaseGRegIndexName(ctx, i);
			bsj2_print(ctx, "mov [ebp%+d], %s\n", k, s);
			j+=szp;
		}
	}

	j=(j+(szxp-1))&(~(szxp-1));
	for(i=0; i<16; i++)
	{
		if(uxr&(1<<i))
		{
			k=ctx->offs_savereg+j;
			s=J2RegX86_GetBaseXRegIndexName(ctx, i);
			bsj2_print(ctx, "movdqu [ebp%+d], %s\n", k, s);

			j+=szxp;
		}
	}

	bsj2_printlog(ctx, "BSVM_J2FrameX86_BeginFrame: End, pass=%d\n",
		ctx->n_passes);
}

BGBSVM_API void BSVM_J2FrameX86_EndFrame(bsjContext ctx)
{
	char *s;
	int i, j, k, szp, szxp, ugr, uxr;

	bsj2_printlog(ctx, "BSVM_J2FrameX86_EndFrame: Begin, pass=%d\n",
		ctx->n_passes);

	//a return label was used
	if(ctx->lbl_return)
		{ bsj2_print(ctx, "%s:\n", ctx->lbl_return); }
	
	szp=BSVM_J2Arch_SizePointer(ctx);
	szxp=16;

	ugr=ctx->mask_usedregs & BSVM_J2RegX86_GRegSaveMask(ctx);
	uxr=ctx->mask_usedxregs & BSVM_J2RegX86_XRegSaveMask(ctx);

	j=0;
	for(i=0; i<16; i++)
	{
		if(ugr&(1<<i))
		{
			k=ctx->offs_savereg+j;
			s=J2RegX86_GetBaseGRegIndexName(ctx, i);
			bsj2_print(ctx, "mov %s, [ebp%+d]\n", s, k);
			j+=szp;
		}
	}

	j=(j+(szxp-1))&(~(szxp-1));
	for(i=0; i<16; i++)
	{
		if(uxr&(1<<i))
		{
			k=ctx->offs_savereg+j;
			s=J2RegX86_GetBaseXRegIndexName(ctx, i);
			bsj2_print(ctx, "movdqu %s, [ebp%+d]\n", s, k);

			j+=szxp;
		}
	}

	bsj2_print(ctx, "mov esp, ebp\n");
	bsj2_print(ctx, "pop ebp\n");
	bsj2_print(ctx, "ret\n");

	bsj2_printlog(ctx, "BSVM_J2FrameX86_EndFrame: End, pass=%d\n",
		ctx->n_passes);
}

BGBSVM_API void BSVM_J2FrameX86_DoReturn(bsjContext ctx)
{
	if(!ctx->lbl_return)
		ctx->lbl_return=dystrdup(BSVM_Jit2_GenSym(ctx));
	
	bsj2_print(ctx, "jmp %s\n", ctx->lbl_return);
}


void BSVM_J2FrameX86_DoJump(bsjContext ctx, char *lbl)
{
	BSVM_J2Arch_SyncState(ctx);
	bsj2_print(ctx, "jmp %s\n", lbl);
}

void BSVM_J2FrameX86_EmitLabel(bsjContext ctx, char *lbl)
{
	BSVM_J2Arch_SyncState(ctx);
	bsj2_print(ctx, "%s:\n", lbl);
}

void BSVM_J2FrameX86_ReturnReg(bsjContext ctx, bsjReg reg)
{
	char *s0, *s1;
	if(J2RegX86_XMMP(reg))
	{
		BSVM_Jit2_CaseError(ctx);
	}else
	{
		s0=J2RegX86_GetBaseGRegIndexName(ctx, 0);
		s1=J2RegX86_GetBaseGRegBaseName(reg);
		if(strcmp(s0, s1))
			bsj2_print(ctx, "mov %s, %s\n", s0, s1);
	}
	BSVM_J2FrameX86_DoReturn(ctx);
}

void BSVM_J2FrameX86_ReturnVar(bsjContext ctx, bsjVar var)
{
	bsjReg reg;

	if(var->reg)
	{
		BSVM_J2FrameX86_ReturnReg(ctx, var->reg);
		return;
	}
	reg=BSVM_J2RegX86_GetRegByIndexType(ctx, 0, var->type);
	BSVM_J2Vars_ReadRegVar(ctx, reg, var, var->type);
}


bsjReg BSVM_J2FrameX86_GetVMContext(bsjContext ctx)
{
	bsjReg reg;

	if(ctx->abitype==BSVM_JIT2_ABITY_BSVMI)
	{
		reg=BSVM_J2RegX86_GetBaseGRegByIndex(ctx, 3);
		return(reg);
	}

	//doesn't exist in ABI
	return(NULL);
}

/** Pop stack items until mark into the temp-args buffer
	as dynamically-typed values. */
int BSVM_J2FrameX86_PopArgsDyt(bsjContext ctx)
{
	bsjReg reg;
	bsjVar var;
	bsjMem mem;
	int szp;
	int i, j;
	
	szp=BSVM_J2Type_PointerSize(ctx);
	i=ctx->markstack[ctx->markstackpos-1]; j=0;
	while(i<ctx->stackpos)
	{
		var=ctx->stack[i];
//		reg=BSVM_J2Reg_ForceAllocReg(ctx, var->type);
		reg=BSVM_J2Vars_LoadVarReg(ctx, var);
		reg=BSVM_J2Cast_CastDyt(ctx, reg);
		mem=J2MemX86_GetMemTargDisp(ctx, j);
		BSVM_J2Mem_MovMemRegDyt(ctx, mem, reg);
		BSVM_J2Reg_FreeReg(reg);
		i++; j+=szp;
		if(j>ctx->sz_targ)ctx->sz_targ=j;
	}
	
	BSVM_J2Stack_PopToMark(ctx);
	
	i=j/szp;
	return(i);
}
