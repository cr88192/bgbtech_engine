/*
Stack Frame Layout:
	Caller Arguments
	Return EIP
	Saved EBP
	<- EBP
	Locals
	TVARs
	Value Stack
	Temp Args
	<- ESP (On Call)
	Scratch
	<- ESP (Intermediate)
*/

#include <bgbsvm.h>

char *J2MemX86_GetMemStr(bsjMem mem)
{
	char tbuf[256];
	char *s, *t;
	
	if(!mem)
	{
//		BSVM_Jit2_CaseError(ctx);
		return("[BAD_ADDR]");
	}
	
	t=tbuf;
	*t++='[';
	if(mem->name)
	{
		strcpy(t, mem->name);
		t+=strlen(t);
		if(mem->breg)
		{
			*t++='+';
			s=BSVM_J2Reg_GetRegName(mem->breg);
			strcpy(t, s);
			t+=strlen(t);
		}
	}else if(mem->breg)
	{
		s=BSVM_J2Reg_GetRegName(mem->breg);
		strcpy(t, s);
		t+=strlen(t);
	}

	if(mem->ireg)
	{
		s=BSVM_J2Reg_GetRegName(mem->ireg);
		sprintf(t, "+%s*%d", s, mem->scale);
		t+=strlen(t);
	}
	
	if(mem->disp)
	{
		sprintf(t, "%+d", mem->disp);
		t+=strlen(t);
	}
	
	*t++=']';
	*t++=0;
	
	return(gcrstrdup(tbuf));
}

void J2MemX86_MovRegMem(bsjContext ctx, bsjReg dst, bsjMem src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Mem_GetMemStr(src);
	
	if(J2RegX86_DWordP(dst))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst))
		{ bsj2_print(ctx, "movss %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst))
		{ bsj2_print(ctx, "movsd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst))
		{ bsj2_print(ctx, "movps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst))
		{ bsj2_print(ctx, "movpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2MemX86_MovMemReg(bsjContext ctx, bsjMem dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Mem_GetMemStr(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(src))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "movss %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "movsd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "movps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "movpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2MemX86_MovMemMem(bsjContext ctx, bsjMem dst, bsjMem src, bsjType ty)
{
	bsjReg tmp;
	
	tmp=BSVM_J2Reg_ForceAllocReg(ctx, ty);
	BSVM_J2Mem_MovRegMem(ctx, tmp, src, ty);
	BSVM_J2Mem_MovMemReg(ctx, dst, tmp, ty);
	BSVM_J2Reg_FreeReg(tmp);
}

bsjMem J2MemX86_GetVarMem(bsjVar var)
{
	bsjMem mem;
	
	mem=BSVM_J2Mem_AllocMemRef(var->ctx);

	if(var->varty==BSVM_JIT2_VARTY_GLOBAL)
	{
		mem->name=var->name;
		return(mem);
	}

	if(var->varty==BSVM_JIT2_VARTY_LOCAL)
	{
//		mem->name=var->name;
		mem->breg=BSVM_J2Reg_GetNamedReg(var->ctx, "ebp");
		mem->disp=var->ctx->offs_local+var->offs;
		return(mem);
	}

	if(var->varty==BSVM_JIT2_VARTY_ARG)
	{
//		mem->name=var->name;
		mem->breg=BSVM_J2Reg_GetNamedReg(var->ctx, "ebp");
		mem->disp=8+var->offs;
		return(mem);
	}

	if(var->varty==BSVM_JIT2_VARTY_TVAR)
	{
//		mem->name=var->name;
		mem->breg=BSVM_J2Reg_GetNamedReg(var->ctx, "ebp");
		mem->disp=var->ctx->offs_tvar+var->offs;
		return(mem);
	}

	if(var->varty==BSVM_JIT2_VARTY_STACK)
	{
//		mem->name=var->name;
		mem->breg=BSVM_J2Reg_GetNamedReg(var->ctx, "ebp");
		mem->disp=var->ctx->offs_vstack+var->offs;
		return(mem);
	}

	if(var->varty==BSVM_JIT2_VARTY_TARG)
	{
//		mem->name=var->name;
		mem->breg=BSVM_J2Reg_GetNamedReg(var->ctx, "ebp");
		mem->disp=var->ctx->offs_targ+var->offs;
		return(mem);
	}

	BSVM_Jit2_CaseError(var->ctx);
	return(NULL);
}

bsjMem J2MemX86_GetMemTargDisp(bsjContext ctx, int disp)
{
	bsjMem mem;
	
	mem=BSVM_J2Mem_AllocMemRef(ctx);
	mem->breg=BSVM_J2Reg_GetNamedReg(ctx, "ebp");
	mem->disp=ctx->offs_targ+disp;
	return(mem);
}

bsjMem J2MemX86_GetMemRegDisp(bsjContext ctx, bsjReg reg, int disp)
{
	bsjMem mem;
	
	mem=BSVM_J2Mem_AllocMemRef(ctx);
	mem->breg=reg;
	mem->disp=disp;
	return(mem);
}

void J2MemX86_LeaRegMem(bsjContext ctx,
	bsjReg dst, bsjMem src)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Mem_GetMemStr(src);
	
	bsj2_print(ctx, "lea %s, %s\n", dname, sname);
	return;
}

void J2MemX86_LeaRegTargDisp(bsjContext ctx, bsjReg dst, int disp)
{
	char *s;
	int i;

	s=BSVM_J2Reg_GetRegName(dst);
	i=ctx->offs_targ+disp;
	bsj2_print(ctx, "lea %s, [ebp%+d]\n", s, i);
}

bsjReg J2MemX86_GetRegTargDisp(bsjContext ctx, int disp)
{
	bsjReg reg;
	reg=BSVM_J2Reg_ForceAllocRegVoidPtr(ctx);
	J2MemX86_LeaRegTargDisp(ctx, reg, disp);
	return(reg);
}

bsjReg J2MemX86_GetRegTargDispDyt(bsjContext ctx, int disp)
{
	bsjReg reg;
	reg=BSVM_J2Reg_ForceAllocRegDyt(ctx);
	J2MemX86_LeaRegTargDisp(ctx, reg, disp);
	return(reg);
}
