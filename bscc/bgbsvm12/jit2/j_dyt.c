#include <bgbsvm.h>

int BSVM_J2Dyt_CalcMetaRegsMask(bsjContext ctx)
{
	int i, j, gvm;

	if(BSVM_J2Arch_IsX86(ctx))
	{
		gvm=BSVM_J2RegX86_GRegValidMask(ctx, BSVM_JIT2_REGTY_DWORD);

		//clear regs we need to save but haven't used
		i=BSVM_J2RegX86_GRegSaveMask(ctx)&(~(ctx->mask_usedregs));
		gvm=gvm&(~i);

		j=0;
		for(i=0; i<8; i++)
			if(!ctx->reg[i] && (gvm&(1<<i)))
				j|=1<<i;

		for(i=0; i<8; i++)
			if(!ctx->xreg[i])
				j|=1<<(i+8);
				
		return(j);
	}
	return(0);
}

void BSVM_J2Dyt_LoadRegInt(bsjContext ctx, bsjReg reg, int val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dyint(val);

	if(BSVM_J2Arch_IsJitX86(ctx) && dyfixintp(v))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadInt, %s, %d, regs=%d\n", s, val, rm);
}

void BSVM_J2Dyt_LoadRegLong(bsjContext ctx, bsjReg reg, s64 val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dyint(val);

	if(BSVM_J2Arch_IsJitX86(ctx) && dyfixintp(v))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadLong, %s, %d, regs=%d\n", s, val, rm);
}

void BSVM_J2Dyt_LoadRegFlonum(bsjContext ctx, bsjReg reg, float val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dyflonum(val);

	if(BSVM_J2Arch_IsJitX86(ctx) && dyflonump(v))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadFlonum, %s, %f, regs=%d\n", s, val, rm);
}

void BSVM_J2Dyt_LoadRegFloat(bsjContext ctx, bsjReg reg, float val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dyfloat(val);

	if(BSVM_J2Arch_IsJitX86(ctx) && dyfloatp(v))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadFloat, %s, %f, regs=%d\n", s, val, rm);
}

void BSVM_J2Dyt_LoadRegDouble(bsjContext ctx, bsjReg reg, float val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dydouble(val);

	if(BSVM_J2Arch_IsJitX86(ctx) && dydoublep(v))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadDouble, %s, %f, regs=%d\n", s, val, rm);
}

void BSVM_J2Dyt_LoadRegSymbol(bsjContext ctx, bsjReg reg, char *val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dysymbol(val);

	if(BSVM_J2Arch_IsJitX86(ctx))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadSymbol, %s, \"%s\", regs=%d\n", s, val, rm);
}

void BSVM_J2Dyt_LoadRegKeyword(bsjContext ctx, bsjReg reg, char *val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dykeyword(val);

	if(BSVM_J2Arch_IsJitX86(ctx))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadKeyword, %s, \"%s\", regs=%d\n",
		s, val, rm);
}

void BSVM_J2Dyt_LoadRegString(bsjContext ctx, bsjReg reg, char *val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
	v=dystring(val);

	if(BSVM_J2Arch_IsJitX86(ctx))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadString, %s, \"%s\", regs=%d\n",
		s, val, rm);
}

void BSVM_J2Dyt_LoadRegFixedValue(bsjContext ctx, bsjReg reg, dyt val)
{
	char *s;
	dyt v;
	int rm;

	s=BSVM_J2Reg_GetRegName(reg);
//	v=dystring(val);

	if(!val)
	{
		bsj2_print(ctx, "xor %s, %s\n", s, s);
		return;
	}

	if(BSVM_J2Arch_IsX86(ctx))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)val);
		return;
	}

#if 0
	if(BSVM_J2Arch_IsX86(ctx))
	{
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)v);
		return;
	}
#endif

	BSVM_Jit2_CaseError(ctx);

//	bsj2_print(ctx, "xmeta DyLoadString, %s, \"%s\"\n", s, val);
}

void BSVM_J2Dyt_LoadReg(bsjContext ctx, bsjReg reg, dyt val)
{
	char *s;

	if(!val)
		{ BSVM_J2Dyt_LoadRegFixedValue(ctx, reg, val); return; }
	if((val==UNDEFINED) || (val==BGBDY_TRUE) || (val==BGBDY_FALSE))
		{ BSVM_J2Dyt_LoadRegFixedValue(ctx, reg, val); return; }

	if(dyfixintp(val) || dyintp(val))
		{ BSVM_J2Dyt_LoadRegInt(ctx, reg, dyintv(val)); return; }
	if(dylongp(val))
		{ BSVM_J2Dyt_LoadRegLong(ctx, reg, dylongv(val)); return; }
	if(dyflonump(val))
		{ BSVM_J2Dyt_LoadRegFlonum(ctx, reg, dyfloatv(val)); return; }
	if(dyfloatp(val))
		{ BSVM_J2Dyt_LoadRegFloat(ctx, reg, dyfloatv(val)); return; }
	if(dydoublep(val))
		{ BSVM_J2Dyt_LoadRegDouble(ctx, reg, dydoublev(val)); return; }

	if(dysymbolp(val))
		{ BSVM_J2Dyt_LoadRegSymbol(ctx, reg, dysymbolv(val)); return; }
	if(dykeywordp(val))
		{ BSVM_J2Dyt_LoadRegKeyword(ctx, reg, dysymbolv(val)); return; }
	if(dystringp(val))
		{ BSVM_J2Dyt_LoadRegString(ctx, reg, dystringv(val)); return; }

	if(BSVM_J2Arch_IsJitX86(ctx))
	{
		s=BSVM_J2Reg_GetRegName(reg);
		bsj2_print(ctx, "mov %s, 0x%08X\n", s, (u32)val);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

void BSVM_J2Dyt_UnaryOpReg(bsjContext ctx, int op,
	bsjReg dst, bsjReg arg)
{
	char *s0, *s1, *s2;
	char *fn;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(arg);

	fn=NULL;
	if(op==BSVM_JIT2_UNOP_NEG)fn="DyNeg";
	if(op==BSVM_JIT2_UNOP_NOT)fn="DyNot";
	if(op==BSVM_JIT2_UNOP_LNOT)fn="DyLNot";
	if(op==BSVM_JIT2_UNOP_DEFER)fn="DyDefer";
	if(op==BSVM_JIT2_UNOP_CAR)fn="DyCar";
	if(op==BSVM_JIT2_UNOP_CDR)fn="DyCdr";
	if(op==BSVM_JIT2_UNOP_COPYVALUE)fn="DyCopyValue";
	if(op==BSVM_JIT2_UNOP_DROPVALUE)fn="DyDropValue";
	if(op==BSVM_JIT2_UNOP_DELETE)fn="DyDelete";
	if(op==BSVM_JIT2_UNOP_CLONE)fn="DyClone";

	if(!fn)
	{
		BSVM_Jit2_CaseError(ctx);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta %s, %s, %s, regs=%d\n", fn, s0, s1, rm);
}

void BSVM_J2Dyt_BinaryOpReg(bsjContext ctx, int op,
	bsjReg dst, bsjReg larg, bsjReg rarg)
{
	char *s0, *s1, *s2;
	char *fn;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(larg);
	s2=BSVM_J2Reg_GetRegName(rarg);

	fn=NULL;
	if(op==BSVM_JIT2_BINOP_ADD)fn="DyAdd";
	if(op==BSVM_JIT2_BINOP_SUB)fn="DySub";
	if(op==BSVM_JIT2_BINOP_MUL)fn="DyMul";
	if(op==BSVM_JIT2_BINOP_DIV)fn="DyDiv";
	if(op==BSVM_JIT2_BINOP_MOD)fn="DyMod";
	if(op==BSVM_JIT2_BINOP_AND)fn="DyAnd";
	if(op==BSVM_JIT2_BINOP_OR)fn="DyOr";
	if(op==BSVM_JIT2_BINOP_XOR)fn="DyXor";

	if(op==BSVM_JIT2_BINOP_EXP)fn="DyExp";
	if(op==BSVM_JIT2_BINOP_SHL)fn="DyShl";
	if(op==BSVM_JIT2_BINOP_SHR)fn="DyShr";
	if(op==BSVM_JIT2_BINOP_SHRR)fn="DyShrr";
	if(op==BSVM_JIT2_BINOP_CONS)fn="DyCons";

	if(op==BSVM_JIT2_BINOP_EQV)fn="DyEqv";
	if(op==BSVM_JIT2_BINOP_NEQV)fn="DyNeqv";
	if(op==BSVM_JIT2_BINOP_L)fn="DyLt";
	if(op==BSVM_JIT2_BINOP_G)fn="DyGt";
	if(op==BSVM_JIT2_BINOP_LE)fn="DyLe";
	if(op==BSVM_JIT2_BINOP_GE)fn="DyGe";
	if(op==BSVM_JIT2_BINOP_EQ)fn="DyEq";
	if(op==BSVM_JIT2_BINOP_NEQ)fn="DyNeq";

	if(!fn)
	{
		BSVM_Jit2_CaseError(ctx);
		return;
	}

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta %s, %s, %s, %s, regs=%d\n", fn, s0, s1, s2, rm);
}

void BSVM_J2Dyt_ConvRegDyt(bsjContext ctx, bsjReg dst, bsjReg src)
{
	char *s0, *s1, *s2;
	char *fn;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(src);

	if(BSVM_J2Type_SmallInt32P(src->type))
	{
		bsj2_print(ctx, "xmeta DyConvFromInt, %s, %s\n", s0, s1);
		return;
	}

	if(BSVM_J2Type_SmallInt64P(src->type))
	{
		bsj2_print(ctx, "xmeta DyConvFromLong, %s, %s\n", s0, s1);
		return;
	}

	if(BSVM_J2Type_SmallFloat32P(src->type))
	{
		bsj2_print(ctx, "xmeta DyConvFromFloat, %s, %s\n", s0, s1);
		return;
	}

	if(BSVM_J2Type_SmallFloat64P(src->type))
	{
		bsj2_print(ctx, "xmeta DyConvFromDouble, %s, %s\n", s0, s1);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

void BSVM_J2Dyt_ConvRegFromDyt(bsjContext ctx, bsjReg dst, bsjReg src)
{
	char *s0, *s1, *s2;
	char *fn;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(src);

	if(BSVM_J2Type_SmallInt32P(dst->type))
	{
		bsj2_print(ctx, "xmeta DyConvToInt, %s, %s\n", s0, s1);
		return;
	}

	if(BSVM_J2Type_SmallInt64P(dst->type))
	{
		bsj2_print(ctx, "xmeta DyConvToLong, %s, %s\n", s0, s1);
		return;
	}

	if(BSVM_J2Type_SmallFloat32P(dst->type))
	{
		bsj2_print(ctx, "xmeta DyConvToFloat, %s, %s\n", s0, s1);
		return;
	}

	if(BSVM_J2Type_SmallFloat64P(dst->type))
	{
		bsj2_print(ctx, "xmeta DyConvToDouble, %s, %s\n", s0, s1);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

void BSVM_J2Dyt_ConvRegFn2Fl(bsjContext ctx, bsjReg dst, bsjReg src)
{
	char *s0, *s1;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(src);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyConvFn2Fl, %s, %s, regs=%d\n", s0, s1, rm);
}

void BSVM_J2Dyt_ConvRegFl2Fn(bsjContext ctx, bsjReg dst, bsjReg src)
{
	char *s0, *s1;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(src);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyConvFl2Fn, %s, %s, regs=%d\n", s0, s1, rm);
}

void BSVM_J2Dyt_GetSelf(bsjContext ctx, bsjReg dst)
{
	BSVM_SVMState st;
	char *s0, *s1;
	int rm;
	int i;
	
	if(BSVM_J2Arch_IsJitX86(ctx))
	{
		i=((byte *)(&st.self))-((byte *)(&st));
		s0=BSVM_J2Reg_GetRegName(dst);
		bsj2_print(ctx, "mov %s, [ebx+%d]\n", s0, i);
		return;
	}

	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmGetSelf, %s\n", s0);
}

void BSVM_J2Dyt_GetTop(bsjContext ctx, bsjReg dst)
{
	BSVM_SVMState st;
	char *s0, *s1;
	int i;
	
	if(BSVM_J2Arch_IsJitX86(ctx))
	{
		i=((byte *)(&st.top))-((byte *)(&st));
		s0=BSVM_J2Reg_GetRegName(dst);
		bsj2_print(ctx, "mov %s, [ebx+%d]\n", s0, i);
		return;
	}

	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmGetTop, %s\n", s0);
}

void BSVM_J2Dyt_GetCF(bsjContext ctx, bsjReg dst)
{
	BSVM_SVMState st;
	char *s0, *s1;
	int i;
	
	if(BSVM_J2Arch_IsJitX86(ctx))
	{
		i=((byte *)(&st.func))-((byte *)(&st));
		s0=BSVM_J2Reg_GetRegName(dst);
		bsj2_print(ctx, "mov %s, [ebx+%d]\n", s0, i);
		return;
	}

	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmGetCF, %s\n", s0);
}

void BSVM_J2Dyt_LexLoadIdx(bsjContext ctx, bsjReg dst, int idx)
{
	char *s0, *s1;
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmLexLoadIdx, %s, %d\n", s0, idx);
}

void BSVM_J2Dyt_LexStoreIdx(bsjContext ctx, int idx, bsjReg dst)
{
	char *s0, *s1;
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmLexStoreIdx, %d, %s\n", idx, s0);
}

void BSVM_J2Dyt_LoadName(bsjContext ctx, bsjReg dst, char *str)
{
	char *s0, *s1;
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmLoadName, %s, %s\n", s0, str);
}

void BSVM_J2Dyt_StoreName(bsjContext ctx, char *str, bsjReg dst)
{
	char *s0, *s1;
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmStoreName, %s, %s\n", str, s0);
}

void BSVM_J2Dyt_BindName(bsjContext ctx, char *str, bsjReg dst)
{
	char *s0, *s1;
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmBindName, %s, %s\n", str, s0);
}

void BSVM_J2Dyt_DynBindName(bsjContext ctx, char *str, bsjReg dst)
{
	char *s0, *s1;
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmDynBindName, %s, %s\n", str, s0);
}

void BSVM_J2Dyt_TyBindName(bsjContext ctx,
	char *name, char *sig, char *flags, bsjReg dst)
{
	char *s0, *s1;
	if(!sig)sig="r";
	if(!flags)flags="";
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmTyBindName, %s, \"%s\", \"%s\", %s\n",
		name, sig, flags, s0);
}

void BSVM_J2Dyt_TyDynBindName(bsjContext ctx,
	char *name, char *sig, char *flags, bsjReg dst)
{
	char *s0, *s1;
	if(!sig)sig="r";
	if(!flags)flags="";
	s0=BSVM_J2Reg_GetRegName(dst);
	bsj2_print(ctx, "xmeta BsVmTyDynBindName, %s, \"%s\", \"%s\", %s\n",
		name, sig, flags, s0);
}

void BSVM_J2Dyt_LoadIndex(bsjContext ctx,
	bsjReg dst, bsjReg arr, bsjReg idx)
{
	char *s0, *s1, *s2;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(arr);
	s2=BSVM_J2Reg_GetRegName(idx);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyGetKey, %s, %s, %s, regs=%d\n",
		s0, s1, s2, rm);
}

void BSVM_J2Dyt_StoreIndex(bsjContext ctx,
	bsjReg arr, bsjReg idx, bsjReg val)
{
	char *s0, *s1, *s2, *s3;
	int rm;

	s0=BSVM_J2Reg_GetRegName(arr);
	s1=BSVM_J2Reg_GetRegName(idx);
	s2=BSVM_J2Reg_GetRegName(val);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DySetKey, %s, %s, %s, regs=%d\n",
		s0, s1, s2, rm);
}

void BSVM_J2Dyt_LoadSlot(bsjContext ctx,
	bsjReg dst, bsjReg arr, char *idx)
{
	char *s0, *s1, *s2;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(arr);
//	s2=BSVM_J2Reg_GetRegName(idx);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyGetSlot, %s, %s, %s, regs=%d\n",
		s0, s1, idx, rm);
}

void BSVM_J2Dyt_StoreSlot(bsjContext ctx,
	bsjReg arr, char *idx, bsjReg val)
{
	char *s0, *s1, *s2, *s3;
	int rm;

	s0=BSVM_J2Reg_GetRegName(arr);
//	s1=BSVM_J2Reg_GetRegName(idx);
	s2=BSVM_J2Reg_GetRegName(val);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DySetSlot, %s, %s, %s, %s, regs=%d\n",
		s2, s0, idx, s2, rm);
}

void BSVM_J2Dyt_BindSlot(bsjContext ctx,
	bsjReg arr, char *idx, bsjReg val)
{
	char *s0, *s1, *s2, *s3;
	int rm;

	s0=BSVM_J2Reg_GetRegName(arr);
//	s1=BSVM_J2Reg_GetRegName(idx);
	s2=BSVM_J2Reg_GetRegName(val);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyBindSlot, %s, %s, %s, %s, regs=%d\n",
		s2, s0, idx, s2, rm);
}

void BSVM_J2Dyt_LoadIndexInt(bsjContext ctx,
	bsjReg dst, bsjReg arr, int idx)
{
	char *s0, *s1, *s2;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(arr);
//	s2=BSVM_J2Reg_GetRegName(idx);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyLoadIndexInt, %s, %s, %d, regs=%d\n",
		s0, s1, idx, rm);
}

void BSVM_J2Dyt_StoreIndexInt(bsjContext ctx,
	bsjReg arr, int idx, bsjReg val)
{
	char *s0, *s1, *s2, *s3;
	int rm;

	s0=BSVM_J2Reg_GetRegName(arr);
//	s1=BSVM_J2Reg_GetRegName(idx);
	s2=BSVM_J2Reg_GetRegName(val);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyStoreIndexInt, %s, %d, %s, regs=%d\n",
		s0, idx, s2, rm);
}


void BSVM_J2Dyt_CallName(bsjContext ctx,
	bsjReg dst, char *str, bsjReg args, int cnt)
{
	char *s0, *s1;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(args);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyCall, %s, %s, %s, %d, regs=%d\n",
		s0, str, s1, cnt, rm);
}

void BSVM_J2Dyt_TailCallName(bsjContext ctx,
	bsjReg dst, char *str, bsjReg args, int cnt)
{
	char *s0, *s1;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(args);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyCall, %s, %s, %s, %d, regs=%d\n",
		s0, str, s1, cnt, rm);
}

void BSVM_J2Dyt_CallFunc(bsjContext ctx,
	bsjReg dst, bsjReg fcn, bsjReg args, int cnt)
{
	char *s0, *s1, *s2;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(fcn);
	s2=BSVM_J2Reg_GetRegName(args);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyApply, %s, %s, %s, %d, regs=%d\n",
		s0, s1, s2, cnt, rm);
}

void BSVM_J2Dyt_TailCallFunc(bsjContext ctx,
	bsjReg dst, bsjReg fcn, bsjReg args, int cnt)
{
	char *s0, *s1, *s2;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(fcn);
	s2=BSVM_J2Reg_GetRegName(args);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyApply, %s, %s, %s, %d, regs=%d\n",
		s0, s1, s2, cnt, rm);
}


void BSVM_J2Dyt_MethodCallName(bsjContext ctx,
	bsjReg dst, bsjReg obj, char *str, bsjReg args, int cnt)
{
	char *s0, *s1, *s2;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(obj);
	s2=BSVM_J2Reg_GetRegName(args);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyMethodCall, %s, %s, %s, %s, %d, regs=%d\n",
		s0, s1, str, s2, cnt, rm);
}

void BSVM_J2Dyt_MethodTailCallName(bsjContext ctx,
	bsjReg dst, bsjReg obj, char *str, bsjReg args, int cnt)
{
	char *s0, *s1, *s2;
	int rm;

	s0=BSVM_J2Reg_GetRegName(dst);
	s1=BSVM_J2Reg_GetRegName(obj);
	s2=BSVM_J2Reg_GetRegName(args);

	rm=BSVM_J2Dyt_CalcMetaRegsMask(ctx);
	bsj2_print(ctx, "xmeta DyMethodCall, %s, %s, %s, %s, %d, regs=%d\n",
		s0, s1, str, s2, cnt, rm);
}

void BSVM_J2Dyt_DoJumpRegNotFalse(bsjContext ctx, bsjReg reg, char *lbl)
{
	char *lbl2, *s0;

	s0=BSVM_J2Reg_GetRegName(reg);

	BSVM_J2Reg_FreeReg(reg);
	BSVM_J2Arch_SyncState(ctx);
	
	lbl2=BSVM_Jit2_GenSym(ctx);
	if(BSVM_J2Arch_IsX86(ctx))
	{
		bsj2_print(ctx, "and %s, %s\njz %s\n",
			s0, s0, lbl2);
		bsj2_print(ctx, "cmp %s, 0x%08X\njz %s\n",
			s0, (u32)BGBDY_FALSE, lbl2);
		bsj2_print(ctx, "jmp %s\n", lbl);
		BSVM_J2FrameX86_EmitLabel(ctx, lbl2);
		return;
	}
	
	BSVM_Jit2_CaseError(ctx);
}

void BSVM_J2Dyt_DoJumpRegFalse(bsjContext ctx, bsjReg reg, char *lbl)
{
	char *lbl2, *s0;

	s0=BSVM_J2Reg_GetRegName(reg);

	BSVM_J2Reg_FreeReg(reg);
	BSVM_J2Arch_SyncState(ctx);
	
//	lbl2=BSVM_Jit2_GenSym(ctx);
	if(BSVM_J2Arch_IsX86(ctx))
	{
		bsj2_print(ctx, "and %s, %s\njz %s\n",
			s0, s0, lbl);
		bsj2_print(ctx, "cmp %s, 0x%08X\njz %s\n",
			s0, (u32)BGBDY_FALSE, lbl);
//		bsj2_print(ctx, "jmp %s\n", lbl);
//		BSVM_J2FrameX86_EmitLabel(ctx, lbl2);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}
