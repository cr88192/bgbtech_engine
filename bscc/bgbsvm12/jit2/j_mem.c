#include <bgbsvm.h>

BGBSVM_API bsjMem BSVM_J2Mem_AllocMemRef(bsjContext ctx)
{
	bsjMem tmp;
	tmp=gctalloc("bsvm_jit2_mem_t", sizeof(BSVM_Jit2_MemInfo));
	tmp->ctx=ctx;
	return(tmp);
}

BGBSVM_API void BSVM_J2Mem_FreeMemRef(bsjMem mem)
{
	gcfree(mem);
}

BGBSVM_API char *BSVM_J2Mem_GetMemStr(bsjMem mem)
{
	return(J2MemX86_GetMemStr(mem));
}

BGBSVM_API void BSVM_J2Mem_MovRegMem(bsjContext ctx,
	bsjReg dst, bsjMem src, bsjType ty)
{
	J2MemX86_MovRegMem(ctx, dst, src, ty);
}

BGBSVM_API void BSVM_J2Mem_MovMemReg(bsjContext ctx,
	bsjMem dst, bsjReg src, bsjType ty)
{
	J2MemX86_MovMemReg(ctx, dst, src, ty);
}

BGBSVM_API void BSVM_J2Mem_MovMemMem(bsjContext ctx,
	bsjMem dst, bsjMem src, bsjType ty)
{
	J2MemX86_MovMemMem(ctx, dst, src, ty);
}


BGBSVM_API void BSVM_J2Mem_MovRegMemDyt(bsjContext ctx,
	bsjReg dst, bsjMem src)
{
	bsjType ty;
	ty=BSVM_J2Type_GetTypeDyt(ctx);
	J2MemX86_MovRegMem(ctx, dst, src, ty);
}

BGBSVM_API void BSVM_J2Mem_MovMemRegDyt(bsjContext ctx,
	bsjMem dst, bsjReg src)
{
	bsjType ty;
	ty=BSVM_J2Type_GetTypeDyt(ctx);
	J2MemX86_MovMemReg(ctx, dst, src, ty);
}

BGBSVM_API void BSVM_J2Mem_MovMemMemDyt(bsjContext ctx,
	bsjMem dst, bsjMem src)
{
	bsjType ty;
	ty=BSVM_J2Type_GetTypeDyt(ctx);
	J2MemX86_MovMemMem(ctx, dst, src, ty);
}


BGBSVM_API void BSVM_J2Mem_LoadRegRegDisp(bsjContext ctx,
	bsjReg dst, bsjReg src, int disp, bsjType ty)
{
	bsjMem mem;
	
	mem=J2MemX86_GetMemRegDisp(ctx, src, disp);
	J2MemX86_MovRegMem(ctx, dst, mem, ty);
	BSVM_J2Mem_FreeMemRef(mem);
}

BGBSVM_API void BSVM_J2Mem_LoadRegRegDispDyt(bsjContext ctx,
	bsjReg dst, bsjReg src, int disp)
{
	BSVM_J2Mem_LoadRegRegDisp(ctx, dst, src, disp,
		BSVM_J2Type_GetTypeDyt(ctx));
}
