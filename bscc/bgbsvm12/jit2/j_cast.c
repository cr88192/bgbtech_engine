#include <bgbsvm.h>

void BSVM_J2Cast_ConvRegInt(
	bsjContext ctx, bsjReg dreg, bsjReg sreg)
{
}

BGBSVM_API void BSVM_J2Cast_ConvReg(
	bsjContext ctx, bsjReg dreg, bsjReg sreg)
{
	if(BSVM_J2Type_VariantP(dreg->type))
	{
		BSVM_J2Dyt_ConvRegDyt(ctx, dreg, sreg);
		return;
	}

	if(BSVM_J2Type_VariantP(sreg->type))
	{
		BSVM_J2Dyt_ConvRegFromDyt(ctx, dreg, sreg);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

BGBSVM_API bsjReg BSVM_J2Cast_CastReg(
	bsjContext ctx, bsjReg reg, bsjType ty)
{
	bsjReg dreg;

	if(!ty)return(NULL);

	if(BSVM_J2Type_TypeEqualP(reg->type, ty))
		return(reg);
		
	dreg=BSVM_J2Reg_ForceAllocReg(ctx, ty);
	BSVM_J2Cast_ConvReg(ctx, dreg, reg);
	BSVM_J2Reg_FreeReg(reg);
	return(dreg);
}

BGBSVM_API bsjReg BSVM_J2Cast_CastDyt(bsjContext ctx, bsjReg reg)
{
	bsjReg dreg;

	if(BSVM_J2Type_VariantP(reg->type))
		return(reg);

	dreg=BSVM_J2Reg_ForceAllocRegDyt(ctx);
	BSVM_J2Dyt_ConvRegDyt(ctx, dreg, reg);
	BSVM_J2Reg_FreeReg(reg);
	return(dreg);
}
