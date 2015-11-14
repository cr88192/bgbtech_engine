#include <bgbsvm.h>

char *j2regx86_regs_dword[16]=
{
 "eax",  "ecx",  "edx",  "ebx",
 "esp",  "ebp",  "esi",  "edi",
 "r8d",  "r9d", "r10d", "r11d",
"r12d", "r13d", "r14d", "r15d"
};

char *j2regx86_regs_word[16]=
{
  "ax",   "cx",   "dx",   "bx",
  "sp",   "bp",   "si",   "di",
 "r8w",  "r9w", "r10w", "r11w",
"r12w", "r13w", "r14w", "r15w"
};

char *j2regx86_regs_byte[16]=
{
  "al",   "cl",   "dl",   "bl",
 "spl",  "bpl",  "sil",  "dil",
 "r8b",  "r9b", "r10b", "r11b",
"r12b", "r13b", "r14b", "r15b"
};

char *j2regx86_regs_qword[16]=
{
"rax", "rcx", "rdx", "rbx",
"rsp", "rbp", "rsi", "rdi",
 "r8",  "r9", "r10", "r11",
"r12", "r13", "r14", "r15"
};

char *j2regx86_regs_xmm[16]=
{
 "xmm0",  "xmm1",  "xmm2",  "xmm3",
 "xmm4",  "xmm5",  "xmm6",  "xmm7",
 "xmm8",  "xmm9", "xmm10", "xmm11",
"xmm12", "xmm13", "xmm14", "xmm15"
};

char *J2RegX86_GetRegName(bsjReg reg)
{
	if(!reg)
		return(NULL);

	if((reg->regidx>>4)==BSVM_JIT2_REGTY_BYTE)
		return(j2regx86_regs_byte[reg->regidx&15]);
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_WORD)
		return(j2regx86_regs_word[reg->regidx&15]);
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_DWORD)
		return(j2regx86_regs_dword[reg->regidx&15]);
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_QWORD)
		return(j2regx86_regs_qword[reg->regidx&15]);
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM)
		return(j2regx86_regs_xmm[reg->regidx&15]);
	return(NULL);
}

char *J2RegX86_GetBaseGRegIndexName(bsjContext ctx, int idx)
	{ return(j2regx86_regs_dword[idx&15]); }
char *J2RegX86_GetBaseXRegIndexName(bsjContext ctx, int idx)
	{ return(j2regx86_regs_dword[idx&15]); }

char *J2RegX86_GetBaseGRegBaseName(bsjReg reg)
	{ return(j2regx86_regs_dword[reg->regidx&15]); }

bool J2RegX86_DWordP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_DWORD)
		return(true);
	return(false);
}

bool J2RegX86_WordP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_WORD)
		return(true);
	return(false);
}

bool J2RegX86_QWordP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_QWORD)
		return(true);
	return(false);
}

bool J2RegX86_XMMP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM)
		return(true);

	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_SS)
		return(true);
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_SD)
		return(true);
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_PS)
		return(true);
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_PD)
		return(true);

	return(false);
}

bool J2RegX86_XMMSSP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_SS)
		return(true);
	return(false);
}

bool J2RegX86_XMMSDP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_SD)
		return(true);
	return(false);
}

bool J2RegX86_XMMPSP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_PS)
		return(true);
	return(false);
}

bool J2RegX86_XMMPDP(bsjReg reg)
{
	if((reg->regidx>>4)==BSVM_JIT2_REGTY_XMM_PD)
		return(true);
	return(false);
}


bsjReg BSVM_J2RegX86_GetXRegByIndex(bsjContext ctx, int idx, int rt)
{
	bsjReg tmp;

	if(!ctx->xreg[idx])
	{
		tmp=BSVM_J2Vars_AllocVar(ctx);
		ctx->xreg[idx]=tmp;
//		tmp->type=ty;
		tmp->regidx=idx|(rt<<4);
		return(tmp);
	}
	
	return(ctx->xreg[idx]);
}

bsjReg BSVM_J2RegX86_GetGRegByIndex(bsjContext ctx, int idx, int rt)
{
	bsjReg tmp;

	if(!ctx->reg[idx])
	{
		tmp=BSVM_J2Vars_AllocVar(ctx);
		ctx->reg[idx]=tmp;
//		tmp->type=ty;
		tmp->regidx=idx|(rt<<4);
		return(tmp);
	}

	tmp=ctx->reg[idx];
	tmp->regidx=idx|(rt<<4);
	return(tmp);
}

bsjReg BSVM_J2RegX86_GetBaseGRegByIndex(bsjContext ctx, int idx)
{
	return(BSVM_J2RegX86_GetGRegByIndex(ctx, idx, BSVM_JIT2_REGTY_DWORD));
}

bsjReg BSVM_J2RegX86_GetGRegByIndexType(bsjContext ctx, int idx, bsjType ty)
{
	bsjReg tmp;
	int rt;
	
	rt=BSVM_JIT2_REGTY_DWORD;
	if(BSVM_J2Type_SmallInt64P(ty))rt=BSVM_JIT2_REGTY_QWORD;
	if(BSVM_J2Type_SmallInt32P(ty))rt=BSVM_JIT2_REGTY_DWORD;
	if(BSVM_J2Type_SmallInt16P(ty))rt=BSVM_JIT2_REGTY_WORD;
	if(BSVM_J2Type_SmallInt8P(ty))rt=BSVM_JIT2_REGTY_BYTE;

	tmp=BSVM_J2RegX86_GetGRegByIndex(ctx, idx, rt);
	tmp->type=ty;
	return(tmp);
}

bsjReg BSVM_J2RegX86_GetXRegByIndexType(bsjContext ctx, int idx, bsjType ty)
{
	bsjReg tmp;
	int rt;
	
	rt=BSVM_JIT2_REGTY_XMM;
	tmp=BSVM_J2RegX86_GetGRegByIndex(ctx, idx, rt);
	tmp->type=ty;
	return(tmp);
}

bsjReg BSVM_J2RegX86_GetRegByIndexType(bsjContext ctx, int idx, bsjType ty)
{
	if(BSVM_J2Reg_TypeXRegP(ty))
		return(BSVM_J2RegX86_GetXRegByIndexType(ctx, idx, ty));
	return(BSVM_J2RegX86_GetGRegByIndexType(ctx, idx, ty));
}

bsjReg BSVM_J2RegX86_AllocXReg(bsjContext ctx, bsjType ty)
{
	bsjReg tmp;
	int i;
	
	for(i=0; i<8; i++)
	{
		if(!ctx->xreg[i])
		{
			tmp=BSVM_J2Vars_AllocVar(ctx);
			ctx->xreg[i]=tmp;
			tmp->type=ty;
			tmp->regidx=i|(BSVM_JIT2_REGTY_XMM<<4);
			ctx->mask_usedxregs|=1<<i;
			return(tmp);
		}
	}
	return(NULL);
}

bsjReg BSVM_J2RegX86_AllocGReg(bsjContext ctx, bsjType ty)
{
	bsjReg tmp;
	int i, rt, vm;
	
	rt=BSVM_JIT2_REGTY_DWORD;
	if(BSVM_J2Type_SmallInt64P(ty))rt=BSVM_JIT2_REGTY_QWORD;
	if(BSVM_J2Type_SmallInt32P(ty))rt=BSVM_JIT2_REGTY_DWORD;
	if(BSVM_J2Type_SmallInt16P(ty))rt=BSVM_JIT2_REGTY_WORD;
	if(BSVM_J2Type_SmallInt8P(ty))rt=BSVM_JIT2_REGTY_BYTE;
	
	vm=BSVM_J2RegX86_GRegValidMask(ctx, rt);
	
	for(i=0; i<16; i++)
	{
		if(!ctx->reg[i] && (vm&(1<<i)))
		{
			tmp=BSVM_J2Vars_AllocVar(ctx);
			ctx->reg[i]=tmp;
			tmp->type=ty;
			tmp->regidx=i|(rt<<4);
			ctx->mask_usedregs|=1<<i;
			return(tmp);
		}
	}
	return(NULL);
}

bsjReg BSVM_J2RegX86_AllocReg(bsjContext ctx, bsjType ty)
{
	if(BSVM_J2Reg_TypeXRegP(ty))
		return(BSVM_J2RegX86_AllocXReg(ctx, ty));
	return(BSVM_J2RegX86_AllocGReg(ctx, ty));
}

bsjReg BSVM_J2RegX86_MapVarReg(bsjContext ctx, bsjVar var)
{
	bsjReg reg;
	
	if(var->reg)
		return(var->reg);
	
	reg=BSVM_J2RegX86_AllocReg(ctx, var->type);
	var->reg=reg;
	reg->mapvar=var;
	return(reg);
}

void BSVM_J2RegX86_FreeReg(bsjReg reg)
{
	if(reg->mapvar)
	{
		reg->mapvar->reg=NULL;
	}

	if(J2RegX86_XMMP(reg))
	{
		reg->ctx->xreg[reg->regidx&15]=NULL;
		gcfree(reg);
		return;
	}

	reg->ctx->reg[reg->regidx&15]=NULL;
	gcfree(reg);
	return;
}

bsjReg BSVM_J2RegX86_GetNamedReg(bsjContext ctx, char *name)
{
	bsjReg reg;
	int i;
	
	for(i=0; i<16; i++)
		if(!strcmp(j2regx86_regs_dword[i], name))
	{
		reg=BSVM_J2RegX86_GetGRegByIndex(ctx, i, BSVM_JIT2_REGTY_DWORD);
		return(reg);
	}

	for(i=0; i<16; i++)
		if(!strcmp(j2regx86_regs_qword[i], name))
	{
		reg=BSVM_J2RegX86_GetGRegByIndex(ctx, i, BSVM_JIT2_REGTY_QWORD);
		return(reg);
	}

	for(i=0; i<16; i++)
		if(!strcmp(j2regx86_regs_xmm[i], name))
	{
		reg=BSVM_J2RegX86_GetGRegByIndex(ctx, i, BSVM_JIT2_REGTY_XMM);
		return(reg);
	}

	for(i=0; i<16; i++)
		if(!strcmp(j2regx86_regs_word[i], name))
	{
		reg=BSVM_J2RegX86_GetGRegByIndex(ctx, i, BSVM_JIT2_REGTY_WORD);
		return(reg);
	}

	for(i=0; i<16; i++)
		if(!strcmp(j2regx86_regs_byte[i], name))
	{
		reg=BSVM_J2RegX86_GetGRegByIndex(ctx, i, BSVM_JIT2_REGTY_BYTE);
		return(reg);
	}
	
	return(NULL);
}

/** Returns mask of which regs may be validly allocated. */
BGBSVM_API int BSVM_J2RegX86_GRegValidMask(bsjContext ctx, int rt)
{
	if(rt==BSVM_JIT2_REGTY_BYTE)
		return(0x0F);

	if(ctx->abitype==BSVM_JIT2_ABITY_BSVMI)
	{
		//EAX ECX EDX EBX ESP EBP ESI EDI
		//	1	1	1	0	0	0	1	1
		return(0xC7);
	}

	//EAX ECX EDX EBX ESP EBP ESI EDI
	//	1	1	1	1	0	0	1	1
	return(0xCF);
}

/** Returns a mask of which registers may need to be saved. */
BGBSVM_API int BSVM_J2RegX86_GRegSaveMask(bsjContext ctx)
{
	//EAX ECX EDX EBX ESP EBP ESI EDI
	//	0	0	0	1	0	0	1	1
	return(0xC8);
}

BGBSVM_API int BSVM_J2RegX86_XRegSaveMask(bsjContext ctx)
{
	return(0);
}
