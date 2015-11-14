#include <bgbsvm.h>

BGBSVM_API bool BSVM_J2Reg_TypeGRegP(bsjType ty)
{
	if(BSVM_J2Type_SmallInt32P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Reg_TypeXRegP(bsjType ty)
{
	if(BSVM_J2Type_SmallFloat128P(ty))
		return(true);
	if(BSVM_J2Type_Int128P(ty))
		return(true);
	return(false);
}

/** Try to allocate a free register, returning NULL if none are available */
BGBSVM_API bsjReg BSVM_J2Reg_AllocReg(bsjContext ctx, bsjType ty)
{
	return(BSVM_J2RegX86_AllocReg(ctx, ty));
}

/** Try to allocate a free register, or try to flush a register.
  * Returns NULL if no registers could be freed up.
  * The newly allocated register will be locked.
  */
BGBSVM_API bsjReg BSVM_J2Reg_ForceAllocReg(bsjContext ctx, bsjType ty)
{
	bsjReg tmp;

	if(!ty)
	{
		BSVM_Jit2_SanityCheckError(ctx, "BSVM_J2Reg_ForceAllocReg: "
			"NULL Type\n");
		return(NULL);
	}

	tmp=BSVM_J2Reg_AllocReg(ctx, ty);
	if(tmp)return(tmp);

	//write out regs and try again
	BSVM_J2Reg_SoftSyncRegs(ctx);

	tmp=BSVM_J2Reg_AllocReg(ctx, ty);
	if(tmp)return(tmp);

	BSVM_Jit2_SanityCheckError(ctx, "BSVM_J2Reg_ForceAllocReg Failed\n");

	return(NULL);
}

BGBSVM_API bsjReg BSVM_J2Reg_MapVarReg(bsjContext ctx, bsjVar var)
{
	if(!var)
		return(NULL);
	return(BSVM_J2RegX86_MapVarReg(ctx, var));
}

BGBSVM_API char *BSVM_J2Reg_GetRegName(bsjReg reg)
{
	if(!reg)return(NULL);
	return(J2RegX86_GetRegName(reg));
}

BGBSVM_API char *BSVM_J2Reg_GetVarRegName(bsjVar var)
{
	if(!var)
		return(NULL);
	if(!var->reg)return(NULL);
	return(BSVM_J2Reg_GetRegName(var->reg));
}

BGBSVM_API char *BSVM_J2Reg_GetMapRegName(bsjContext ctx, bsjVar var)
{
	bsjReg reg;

	if(!var)
		return(NULL);
	if(!var->reg)
	{
		reg=BSVM_J2Reg_MapVarReg(ctx, var);
		return(BSVM_J2Reg_GetRegName(reg));
	}
	return(BSVM_J2Reg_GetRegName(var->reg));
}

BGBSVM_API void BSVM_J2Reg_FreeReg(bsjReg reg)
{
	if(!reg)return;
	BSVM_J2RegX86_FreeReg(reg);
}

BGBSVM_API void BSVM_J2Reg_LockReg(bsjReg reg)
{
}

BGBSVM_API void BSVM_J2Reg_UnlockReg(bsjReg reg)
{
}

BGBSVM_API void BSVM_J2Reg_SyncRegs(bsjContext ctx)
{
	int i;

	for(i=0; i<16; i++)
	{
		if(ctx->reg[i])
		{
			if(ctx->reg[i]->mapvar)
			{
				BSVM_J2Vars_WriteVarReg(ctx,
					ctx->reg[i]->mapvar, ctx->reg[i],
					ctx->reg[i]->type);
			}
			BSVM_J2Reg_FreeReg(ctx->reg[i]);
			ctx->reg[i]=NULL;
		}
		if(ctx->xreg[i])
		{
			if(ctx->xreg[i]->mapvar)
			{
				BSVM_J2Vars_WriteVarReg(ctx,
					ctx->xreg[i]->mapvar, ctx->xreg[i],
					ctx->xreg[i]->type);
			}
			BSVM_J2Reg_FreeReg(ctx->xreg[i]);
			ctx->xreg[i]=NULL;
		}
	}
}

BGBSVM_API void BSVM_J2Reg_SoftSyncRegs(bsjContext ctx)
{
	int i;

	for(i=0; i<16; i++)
	{
		if(ctx->reg[i])
		{
			if(ctx->reg[i]->mapvar)
			{
				BSVM_J2Vars_WriteVarReg(ctx,
					ctx->reg[i]->mapvar, ctx->reg[i],
					ctx->reg[i]->type);
				BSVM_J2Reg_FreeReg(ctx->reg[i]);
				ctx->reg[i]=NULL;
			}
		}
		if(ctx->xreg[i])
		{
			if(ctx->xreg[i]->mapvar)
			{
				BSVM_J2Vars_WriteVarReg(ctx,
					ctx->xreg[i]->mapvar, ctx->xreg[i],
					ctx->xreg[i]->type);
				BSVM_J2Reg_FreeReg(ctx->xreg[i]);
				ctx->xreg[i]=NULL;
			}
		}
	}
}

BGBSVM_API bsjReg BSVM_J2Reg_GetNamedReg(bsjContext ctx, char *reg)
{
	return(BSVM_J2RegX86_GetNamedReg(ctx, reg));
}

/** Free all registers allocated within context. */
BGBSVM_API void BSVM_J2Reg_FreeContextRegs(bsjContext ctx)
{
	int i;

	for(i=0; i<16; i++)
	{
		if(ctx->reg[i])
		{
			BSVM_J2Reg_FreeReg(ctx->reg[i]);
			ctx->reg[i]=NULL;
		}
		if(ctx->xreg[i])
		{
			BSVM_J2Reg_FreeReg(ctx->xreg[i]);
			ctx->xreg[i]=NULL;
		}
	}
}

BGBSVM_API void BSVM_J2Reg_MovReg(bsjContext ctx,
	bsjReg dst, bsjReg src, bsjType ty)
{
	J2ArithX86_MovReg(ctx, dst, src, ty);
}

/** Get VM Context */
BGBSVM_API bsjReg BSVM_J2Reg_GetVMContext(bsjContext ctx)
{
	return(BSVM_J2FrameX86_GetVMContext(ctx));
}

BGBSVM_API bsjReg BSVM_J2Reg_ForceAllocRegDyt(bsjContext ctx)
{
	return(BSVM_J2Reg_ForceAllocReg(ctx, BSVM_J2Type_GetTypeDyt(ctx)));
}

BGBSVM_API bsjReg BSVM_J2Reg_ForceAllocRegVoidPtr(bsjContext ctx)
{
	return(BSVM_J2Reg_ForceAllocReg(ctx, BSVM_J2Type_GetTypeDyt(ctx)));
}
