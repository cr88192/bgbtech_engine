#include <bgbsvm.h>

BGBSVM_API int BSVM_J2Arch_IsJIT(bsjContext ctx)
{
	return(!ctx->top->nojit);
}

BGBSVM_API int BSVM_J2Arch_IsX86(bsjContext ctx)
{
	return(ctx->top->arch==BSVM_JIT2_ARCH_X86);
//	return(1);
}

BGBSVM_API int BSVM_J2Arch_IsX64(bsjContext ctx)
{
	return(ctx->top->arch==BSVM_JIT2_ARCH_X64);
//	return(0);
}

BGBSVM_API int BSVM_J2Arch_IsARM(bsjContext ctx)
{
	return(ctx->top->arch==BSVM_JIT2_ARCH_ARM);
//	return(0);
}

BGBSVM_API int BSVM_J2Arch_IsJitX86(bsjContext ctx)
	{ return(BSVM_J2Arch_IsJIT(ctx) && BSVM_J2Arch_IsX86(ctx)); }
BGBSVM_API int BSVM_J2Arch_IsJitX64(bsjContext ctx)
	{ return(BSVM_J2Arch_IsJIT(ctx) && BSVM_J2Arch_IsX64(ctx)); }
BGBSVM_API int BSVM_J2Arch_IsJitARM(bsjContext ctx)
	{ return(BSVM_J2Arch_IsJIT(ctx) && BSVM_J2Arch_IsARM(ctx)); }

BGBSVM_API int BSVM_J2Arch_SizePointer(bsjContext ctx)
{
	if(BSVM_J2Arch_IsX86(ctx))
		return(4);
	if(BSVM_J2Arch_IsX64(ctx))
		return(8);
	if(BSVM_J2Arch_IsARM(ctx))
		return(4);
	return(0);
}

BGBSVM_API int BSVM_J2Arch_SizeStackAlign(bsjContext ctx)
{
	if(BSVM_J2Arch_IsX86(ctx))
		return(4);
	if(BSVM_J2Arch_IsX64(ctx))
		return(16);
	if(BSVM_J2Arch_IsARM(ctx))
		return(4);
	return(0);
}


BGBSVM_API void BSVM_J2Arch_DoJump(bsjContext ctx, char *lbl)
{
	BSVM_J2FrameX86_DoJump(ctx, lbl);
}

BGBSVM_API void BSVM_J2Arch_EmitLabel(bsjContext ctx, char *lbl)
{
	BSVM_J2FrameX86_EmitLabel(ctx, lbl);
}

/** Synchronize state.
  * Write registers to memory.
  * Ensure stack address is correct.
  * ...
  */
BGBSVM_API void BSVM_J2Arch_SyncState(bsjContext ctx)
{
	BSVM_J2Reg_SyncRegs(ctx);
}
