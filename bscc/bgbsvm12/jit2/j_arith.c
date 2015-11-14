#include <bgbsvm.h>

bsjVar BSVM_J2Arith_BinaryOpVarTy(bsjContext ctx,
	bsjVar larg, bsjVar rarg, int op, bsjType ty)
{
	BSVM_Jit2_StubError(ctx);
	return(NULL);
}

bsjVar BSVM_J2Arith_AddVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_ADD, ty));
}

bsjVar BSVM_J2Arith_SubVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_SUB, ty));
}

bsjVar BSVM_J2Arith_MulVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_MUL, ty));
}

bsjVar BSVM_J2Arith_DivVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_DIV, ty));
}

bsjVar BSVM_J2Arith_ModVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_MOD, ty));
}

bsjVar BSVM_J2Arith_AndVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_AND, ty));
}

bsjVar BSVM_J2Arith_OrVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_OR, ty));
}

bsjVar BSVM_J2Arith_XorVarTy(bsjContext ctx,
	bsjVar dst, bsjVar src, bsjType ty)
{
	return(BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src,
		BSVM_JIT2_BINOP_XOR, ty));
}


bsjType BSVM_J2Arith_BinaryType(bsjContext ctx,
	bsjType lty, bsjType rty, int op)
{
	if(BSVM_J2Type_VariantP(lty) || BSVM_J2Type_VariantP(rty))
		return(BSVM_J2Type_GetType(ctx, "r"));

	BSVM_Jit2_CaseError(ctx);
	return(NULL);
}

bsjType BSVM_J2Arith_BinaryVarType(bsjContext ctx,
	bsjVar larg, bsjVar rarg, int op)
{
	return(BSVM_J2Arith_BinaryType(ctx, larg->type, rarg->type, op));
}

bsjVar BSVM_J2Arith_BinaryOpVar(bsjContext ctx,
	bsjVar dst, bsjVar src, int op)
{
	bsjType ty;
	bsjVar tmp;
	
	ty=BSVM_J2Arith_BinaryVarType(ctx, dst, src, op);
	tmp=BSVM_J2Arith_BinaryOpVarTy(ctx, dst, src, op, ty);
	return(tmp);
}

bsjVar BSVM_J2Arith_AddVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_ADD)); }
bsjVar BSVM_J2Arith_SubVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_SUB)); }
bsjVar BSVM_J2Arith_MulVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_MUL)); }
bsjVar BSVM_J2Arith_DivVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_DIV)); }
bsjVar BSVM_J2Arith_ModVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_MOD)); }
bsjVar BSVM_J2Arith_AndVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_AND)); }
bsjVar BSVM_J2Arith_OrVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_OR)); }
bsjVar BSVM_J2Arith_XorVar(bsjContext ctx, bsjVar dst, bsjVar src)
	{ return(BSVM_J2Arith_BinaryOpVar(ctx, dst, src, BSVM_JIT2_BINOP_XOR)); }
