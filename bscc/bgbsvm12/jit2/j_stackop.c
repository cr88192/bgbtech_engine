#include <bgbsvm.h>

BGBSVM_API bsjReg BSVM_J2StackOp_PopBuildList(bsjContext ctx)
{
	bsjReg rt, rl;
	int i, j;

	rl=BSVM_J2Reg_ForceAllocRegDyt(ctx);
	BSVM_J2Dyt_LoadRegFixedValue(ctx, rl, NULL);

	i=ctx->markstack[ctx->markstackpos-1];
	while(ctx->stackpos>i)
	{
		rt=BSVM_J2Stack_PopReg(ctx);
		BSVM_J2Dyt_BinaryOpReg(ctx, BSVM_JIT2_BINOP_CONS, rl, rt, rl);
		BSVM_J2Reg_FreeReg(rt);
	}
	
	ctx->markstackpos--;
	return(rl);
}

/** Perform the given binary operation */
BGBSVM_API void BSVM_J2Stack_BinaryOp(bsjContext ctx, int op)
{
	bsjReg larg, rarg;
	bsjType ty;
	
	rarg=BSVM_J2Stack_PopReg(ctx);
	larg=BSVM_J2Stack_PopReg(ctx);
	if(!larg || !rarg)
		return;

	ty=BSVM_J2Arith_BinaryType(ctx, larg->type, rarg->type, op);
	if(!ty)
	{
		BSVM_Jit2_SanityCheckError(ctx, "BSVM_J2Stack_BinaryOp: "
			"NULL Target Type");
		return;
	}

	larg=BSVM_J2Cast_CastReg(ctx, larg, ty);
	rarg=BSVM_J2Cast_CastReg(ctx, rarg, ty);
	
	if(BSVM_J2Type_VariantP(ty))
	{
		BSVM_J2Dyt_BinaryOpReg(ctx, op, larg, larg, rarg);
		BSVM_J2Stack_PushReg(ctx, larg);
		BSVM_J2Reg_FreeReg(rarg);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

BGBSVM_API void BSVM_J2Stack_UnaryOp(bsjContext ctx, int op)
{
	bsjReg reg;
	bsjType ty;
	
	reg=BSVM_J2Stack_PopReg(ctx);
	if(!reg)return;

	if(BSVM_J2Type_VariantP(reg->type))
	{
		BSVM_J2Dyt_UnaryOpReg(ctx, op, reg, reg);
		BSVM_J2Stack_PushReg(ctx, reg);
//		BSVM_J2Reg_FreeReg(rarg);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LoadName(bsjContext ctx, char *str)
{
	bsjReg reg;

	reg=BSVM_J2Vars_LoadLexVarRegName(ctx, str);
	if(reg)
	{
		BSVM_J2Stack_PushReg(ctx, reg);
		return;
	}

	if(1)
	{
		reg=BSVM_J2Reg_ForceAllocRegDyt(ctx);
		BSVM_J2Dyt_LoadName(ctx, reg, str);
		BSVM_J2Stack_PushReg(ctx, reg);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_StoreName(bsjContext ctx, char *str)
{
	bsjVar var;
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	if(!reg)return;

	var=BSVM_J2Vars_LookupLexical(ctx, str);
	if(var)
	{
		BSVM_J2Vars_MovVarReg(ctx, var, reg, var->type);
		BSVM_J2Reg_FreeReg(reg);
		return;
	}

	if(1)
	{
		BSVM_J2Dyt_StoreName(ctx, str, reg);
		BSVM_J2Reg_FreeReg(reg);
		return;
	}
}

BGBSVM_API void BSVM_J2StackOp_LoadAddrName(bsjContext ctx, char *str)
{
	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LoadRefName(bsjContext ctx, char *str)
{
	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexLoadIdx(bsjContext ctx, int idx)
{
	bsjReg reg;

	reg=BSVM_J2Vars_LoadLexVarRegIdx(ctx, idx);
	if(reg)
	{
		BSVM_J2Stack_PushReg(ctx, reg);
		return;
	}

	if(1)
	{
		reg=BSVM_J2Reg_ForceAllocRegDyt(ctx);
		BSVM_J2Dyt_LexLoadIdx(ctx, reg, idx);
		BSVM_J2Stack_PushReg(ctx, reg);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexStoreIdx(bsjContext ctx, int idx)
{
	bsjVar var;
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	if(!reg)return;

	var=BSVM_J2Vars_IndexLexical(ctx, idx);
	if(var)
	{
		BSVM_J2Vars_MovVarReg(ctx, var, reg, var->type);
		BSVM_J2Reg_FreeReg(reg);
		return;
	}

	if(1)
	{
		BSVM_J2Dyt_LexStoreIdx(ctx, idx, reg);
		BSVM_J2Reg_FreeReg(reg);
		return;
	}
}

BGBSVM_API void BSVM_J2StackOp_LexIncrementIdx(bsjContext ctx, int idx)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, idx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC);
	BSVM_J2StackOp_LexStoreIdx(ctx, idx);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexDecrementIdx(bsjContext ctx, int idx)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, idx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC);
	BSVM_J2StackOp_LexStoreIdx(ctx, idx);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexPreIncrementIdx(bsjContext ctx, int idx)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, idx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2StackOp_LexStoreIdx(ctx, idx);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexPreDecrementIdx(bsjContext ctx, int idx)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, idx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2StackOp_LexStoreIdx(ctx, idx);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexPostIncrementIdx(bsjContext ctx, int idx)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, idx);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC);
	BSVM_J2StackOp_LexStoreIdx(ctx, idx);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexPostDecrementIdx(bsjContext ctx, int idx)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, idx);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC);
	BSVM_J2StackOp_LexStoreIdx(ctx, idx);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_IncrementName(bsjContext ctx, char *str)
{
	BSVM_J2StackOp_LoadName(ctx, str);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC);
	BSVM_J2StackOp_StoreName(ctx, str);
}

BGBSVM_API void BSVM_J2StackOp_DecrementName(bsjContext ctx, char *str)
{
	BSVM_J2StackOp_LoadName(ctx, str);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC);
	BSVM_J2StackOp_StoreName(ctx, str);
}

BGBSVM_API void BSVM_J2StackOp_PreIncrementName(bsjContext ctx, char *str)
{
	BSVM_J2StackOp_LoadName(ctx, str);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2StackOp_StoreName(ctx, str);
}

BGBSVM_API void BSVM_J2StackOp_PreDecrementName(bsjContext ctx, char *str)
{
	BSVM_J2StackOp_LoadName(ctx, str);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2StackOp_StoreName(ctx, str);
}

BGBSVM_API void BSVM_J2StackOp_PostIncrementName(bsjContext ctx, char *str)
{
	BSVM_J2StackOp_LoadName(ctx, str);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_INC);
	BSVM_J2StackOp_StoreName(ctx, str);
}

BGBSVM_API void BSVM_J2StackOp_PostDecrementName(bsjContext ctx, char *str)
{
	BSVM_J2StackOp_LoadName(ctx, str);
	BSVM_J2StackOp_Dup(ctx);
	BSVM_J2Stack_UnaryOp(ctx, BSVM_JIT2_UNOP_DEC);
	BSVM_J2StackOp_StoreName(ctx, str);
}



BGBSVM_API void BSVM_J2StackOp_Sizeof(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_SizeofName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_BeginTry(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_EndTry(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BeginCatch(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BeginCatchFinal(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_EndCatch(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_BeginSync(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_EndSync(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_LockObj(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_UnlockObj(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_ThrowObj(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_ThrowName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_NewName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_NewArrayName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_Unquote(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_SetDefer(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_ToString(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_Import(bsjContext ctx, char *name, char *fl)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_Iface(bsjContext ctx, char *name, char *fl)
	{ BSVM_Jit2_StubError(ctx); }


BGBSVM_API void BSVM_J2StackOp_EndClass(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_EndStruct(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_EndIface(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_EndPackage(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_BeginClass(bsjContext ctx,
	char *name, char *super, char *flags)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BeginStruct(bsjContext ctx,
	char *name, char *super, char *flags)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BeginIface(bsjContext ctx,
	char *name, char *super, char *flags)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BeginPackage(bsjContext ctx,
	char *name, char *flags)
	{ BSVM_Jit2_StubError(ctx); }


BGBSVM_API void BSVM_J2StackOp_BindName(bsjContext ctx, char *str)
{
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Dyt_BindName(ctx, str, reg);
	BSVM_J2Reg_FreeReg(reg);

//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DynBindName(bsjContext ctx, char *str)
{
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Dyt_DynBindName(ctx, str, reg);
	BSVM_J2Reg_FreeReg(reg);

//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LexBindName(bsjContext ctx, char *name)
{
	bsjVar tmp;
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	tmp=BSVM_J2Vars_LexBindLocalSig(ctx, name, "r", NULL);
	BSVM_J2Vars_MapVarReg(ctx, tmp, reg);
}

BGBSVM_API void BSVM_J2StackOp_TyBind(bsjContext ctx,
	char *name, char *sig, char *flags)
{
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Dyt_TyBindName(ctx, name, sig, flags, reg);
	BSVM_J2Reg_FreeReg(reg);

//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_TyLexBind(bsjContext ctx,
	char *name, char *sig, char *flags)
{
	bsjVar tmp, tmp1;
	bsjReg reg;

	tmp=BSVM_J2Vars_LexBindLocalSig(ctx, name, sig, flags);
	
	tmp1=BSVM_J2Stack_PeekVar(ctx);
	if(tmp1 && (tmp1->value!=BGBDY_AUXNULL))
	{
		reg=BSVM_J2Stack_PopReg(ctx);
		BSVM_J2Vars_MapVarReg(ctx, tmp, reg);
	}else
	{
		BSVM_J2Stack_PopDestroy(ctx);
	}
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_TyDynBind(bsjContext ctx,
	char *name, char *sig, char *flags)
{
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Dyt_TyDynBindName(ctx, name, sig, flags, reg);
	BSVM_J2Reg_FreeReg(reg);

//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_PushSelf(bsjContext ctx)
{
	BSVM_SVMState st;
	bsjReg reg, reg2;
	int i;

#if 0
	i=((byte *)(&st.self))-((byte *)(&st));

	reg=BSVM_J2Reg_GetVMContext(ctx);
	if(reg)
	{
		reg2=BSVM_J2Reg_ForceAllocRegDyt(ctx);
		BSVM_J2Mem_LoadRegRegDispDyt(ctx, reg2, reg, i);
		BSVM_J2Stack_PushReg(ctx, reg2);
		BSVM_J2Reg_FreeReg(reg);
		return;
	}
#endif

	if(1)
	{
		reg=BSVM_J2Reg_ForceAllocRegDyt(ctx);
		BSVM_J2Dyt_GetSelf(ctx, reg);
		BSVM_J2Stack_PushReg(ctx, reg);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_PushCurrentFunc(bsjContext ctx)
{
	BSVM_SVMState st;
	bsjReg reg, reg2;
	int i;

#if 0
	i=((byte *)(&st.func))-((byte *)(&st));

	reg=BSVM_J2Reg_GetVMContext(ctx);
	if(reg)
	{
		reg2=BSVM_J2Reg_ForceAllocRegDyt(ctx);
		BSVM_J2Mem_LoadRegRegDispDyt(ctx, reg2, reg, i);
		BSVM_J2Stack_PushReg(ctx, reg2);
		BSVM_J2Reg_FreeReg(reg);
		return;
	}
#endif

	if(1)
	{
		reg=BSVM_J2Reg_ForceAllocRegDyt(ctx);
		BSVM_J2Dyt_GetCF(ctx, reg);
		BSVM_J2Stack_PushReg(ctx, reg);
		return;
	}

	BSVM_Jit2_CaseError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_LoadIndex(bsjContext ctx)
{
	bsjReg larg, rarg;
	bsjType ty;
	
	rarg=BSVM_J2Stack_PopReg(ctx);
	larg=BSVM_J2Stack_PopReg(ctx);

	if(!larg || !rarg)
	{
		BSVM_Jit2_CaseError(ctx);
		return;
	}
	
	BSVM_J2Dyt_LoadIndex(ctx, larg, larg, rarg);
	BSVM_J2Stack_PushReg(ctx, larg);
	BSVM_J2Reg_FreeReg(rarg);

//	ty=BSVM_J2Arith_BinaryType(ctx, larg->type, rarg->type, op);
//	larg=BSVM_J2Cast_CastReg(ctx, larg, ty);
//	rarg=BSVM_J2Cast_CastReg(ctx, rarg, ty);
}

BGBSVM_API void BSVM_J2StackOp_StoreIndex(bsjContext ctx)
{
	bsjReg larg, rarg, varg;

	rarg=BSVM_J2Stack_PopReg(ctx);
	larg=BSVM_J2Stack_PopReg(ctx);
	varg=BSVM_J2Stack_PopReg(ctx);
	
	BSVM_J2Dyt_StoreIndex(ctx, larg, rarg, varg);
	BSVM_J2Reg_FreeReg(larg);
	BSVM_J2Reg_FreeReg(rarg);
	BSVM_J2Reg_FreeReg(varg);
}

BGBSVM_API void BSVM_J2StackOp_LoadIndexAddr(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_LoadIndexName(bsjContext ctx, char *str)
{
	bsjReg larg, rarg;
	bsjType ty;
	
//	rarg=BSVM_J2Stack_PopReg(ctx);
	larg=BSVM_J2Stack_PopReg(ctx);
	
	BSVM_J2Dyt_LoadSlot(ctx, larg, larg, str);
	BSVM_J2Stack_PushReg(ctx, larg);
//	BSVM_J2Reg_FreeReg(rarg);
}

BGBSVM_API void BSVM_J2StackOp_StoreIndexName(bsjContext ctx, char *str)
{
	bsjReg larg, rarg, varg;

//	rarg=BSVM_J2Stack_PopReg(ctx);
	larg=BSVM_J2Stack_PopReg(ctx);
	varg=BSVM_J2Stack_PopReg(ctx);
	
	BSVM_J2Dyt_StoreSlot(ctx, larg, str, varg);
	BSVM_J2Reg_FreeReg(larg);
//	BSVM_J2Reg_FreeReg(rarg);
	BSVM_J2Reg_FreeReg(varg);
}

BGBSVM_API void BSVM_J2StackOp_LoadIndexAddrName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_LoadIndexRefName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_LoadIndexInt(bsjContext ctx, int idx)
{
	bsjReg larg, rarg;
	bsjType ty;
	
//	rarg=BSVM_J2Stack_PopReg(ctx);
	larg=BSVM_J2Stack_PopReg(ctx);
	
	BSVM_J2Dyt_LoadIndexInt(ctx, larg, larg, idx);
	BSVM_J2Stack_PushReg(ctx, larg);
//	BSVM_J2Reg_FreeReg(rarg);
}

BGBSVM_API void BSVM_J2StackOp_StoreIndexInt(bsjContext ctx, int idx)
{
	bsjReg larg, rarg, varg;

//	rarg=BSVM_J2Stack_PopReg(ctx);
	larg=BSVM_J2Stack_PopReg(ctx);
	varg=BSVM_J2Stack_PopReg(ctx);
	
	BSVM_J2Dyt_StoreIndexInt(ctx, larg, idx, varg);
	BSVM_J2Reg_FreeReg(larg);
//	BSVM_J2Reg_FreeReg(rarg);
	BSVM_J2Reg_FreeReg(varg);
}

BGBSVM_API void BSVM_J2StackOp_LoadIndexAddrInt(bsjContext ctx, int idx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_LoadIndexRefInt(bsjContext ctx, int idx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_Close(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_Cast(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_CastName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_Dup(bsjContext ctx)
{
	bsjReg reg;

	reg=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Stack_PushReg(ctx, reg);
	BSVM_J2Stack_PushReg(ctx, reg);
}

BGBSVM_API void BSVM_J2StackOp_Exch(bsjContext ctx)
{
	bsjReg rega, regb;

	rega=BSVM_J2Stack_PopReg(ctx);
	regb=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Stack_PushReg(ctx, rega);
	BSVM_J2Stack_PushReg(ctx, regb);
}

BGBSVM_API void BSVM_J2StackOp_ExchSelf(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_ExchTop(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_Pop(bsjContext ctx)
{
	BSVM_J2Stack_PopDestroy(ctx);
}

BGBSVM_API void BSVM_J2StackOp_IndexStack(bsjContext ctx, int idx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_RIndexStack(bsjContext ctx, int idx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_SetIndexStack(bsjContext ctx, int idx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_SetRIndexStack(bsjContext ctx, int idx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_DoJump(bsjContext ctx, char *str)
{
	BSVM_J2FrameX86_DoJump(ctx, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpTrue(bsjContext ctx, char *str)
{
	bsjReg reg;
	reg=BSVM_J2Stack_PopReg(ctx);
	if(!reg)return;
	BSVM_J2Dyt_DoJumpRegNotFalse(ctx, reg, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpFalse(bsjContext ctx, char *str)
{
	bsjReg reg;
	reg=BSVM_J2Stack_PopReg(ctx);
	if(!reg)return;
	BSVM_J2Dyt_DoJumpRegFalse(ctx, reg, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpCondUnary(bsjContext ctx,
		int op, char *str)
{
	BSVM_J2Stack_UnaryOp(ctx, op);
	BSVM_J2StackOp_DoJumpTrue(ctx, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpCondBinary(bsjContext ctx,
		int op, char *str)
{
	BSVM_J2Stack_BinaryOp(ctx, op);
	BSVM_J2StackOp_DoJumpTrue(ctx, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpCondBinaryFn(bsjContext ctx,
		int op, char *str)
{
	BSVM_J2StackOp_DoJumpCondBinary(ctx, op, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpCondBinaryFl(bsjContext ctx,
		int op, char *str)
{
	BSVM_J2StackOp_DoJumpCondBinary(ctx, op, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpCondBinaryLexFn(bsjContext ctx,
		int op, char *str, int li, int ri)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, li);
	BSVM_J2StackOp_LexLoadIdx(ctx, ri);
	BSVM_J2StackOp_DoJumpCondBinaryFn(ctx, op, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpCondBinaryFnC(bsjContext ctx,
		int op, char *str, int c)
{
	BSVM_J2Stack_PushDyt(ctx, dyint(c));
	BSVM_J2StackOp_DoJumpCondBinaryFn(ctx, op, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_DoJumpCondBinaryLexFnC(bsjContext ctx,
		int op, char *str, int li, int c)
{
	BSVM_J2StackOp_LexLoadIdx(ctx, li);
	BSVM_J2Stack_PushDyt(ctx, dyint(c));
	BSVM_J2StackOp_DoJumpCondBinaryFn(ctx, op, str);
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API void BSVM_J2StackOp_CompoundExch(bsjContext ctx, int op)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_JumpIndirectAddr(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_LoadIndirectAddr(bsjContext ctx, char *name)
	{ BSVM_Jit2_StubError(ctx); }


BGBSVM_API void BSVM_J2StackOp_BuildList(bsjContext ctx)
{
	bsjReg ra;
	ra=BSVM_J2StackOp_PopBuildList(ctx);
	BSVM_J2Stack_PushReg(ctx, ra);
}

BGBSVM_API void BSVM_J2StackOp_BuildDict(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildAttr(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildVector(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildComplexImag(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildComplex(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildMatrix(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildQuat(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildArray(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_BuildArrayName(bsjContext ctx, char *str)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_IncrementFn(bsjContext ctx, int v)
	{ BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_ADD, v); }
BGBSVM_API void BSVM_J2StackOp_DecrementFn(bsjContext ctx, int v)
	{ BSVM_J2StackOp_BinaryOpFnC(ctx, BSVM_JIT2_BINOP_SUB, v); }

BGBSVM_API void BSVM_J2StackOp_ConvFn2Fl(bsjContext ctx)
{
	bsjReg reg;
	reg=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Dyt_ConvRegFn2Fl(ctx, reg, reg);
	BSVM_J2Stack_PushReg(ctx, reg);
}

BGBSVM_API void BSVM_J2StackOp_ConvFl2Fn(bsjContext ctx)
{
	bsjReg reg;
	reg=BSVM_J2Stack_PopReg(ctx);
	BSVM_J2Dyt_ConvRegFl2Fn(ctx, reg, reg);
	BSVM_J2Stack_PushReg(ctx, reg);
}

BGBSVM_API void BSVM_J2StackOp_BinaryOp(bsjContext ctx, int op)
{
	BSVM_J2Stack_BinaryOp(ctx, op);
}

BGBSVM_API void BSVM_J2StackOp_UnaryOp(bsjContext ctx, int op)
{
	BSVM_J2Stack_UnaryOp(ctx, op);
}

BGBSVM_API void BSVM_J2StackOp_BinaryOpFn(bsjContext ctx, int op)
{
	BSVM_J2Stack_BinaryOp(ctx, op);
}

BGBSVM_API void BSVM_J2StackOp_UnaryOpFn(bsjContext ctx, int op)
{
	BSVM_J2Stack_UnaryOp(ctx, op);
}

BGBSVM_API void BSVM_J2StackOp_BinaryOpFnC(bsjContext ctx, int op, int c)
{
	BSVM_J2Stack_PushDyt(ctx, dyint(c));
	BSVM_J2Stack_BinaryOp(ctx, op);
}

BGBSVM_API void BSVM_J2StackOp_BinaryOpFl(bsjContext ctx, int op)
{
	BSVM_J2Stack_BinaryOp(ctx, op);
}

BGBSVM_API void BSVM_J2StackOp_UnaryOpFl(bsjContext ctx, int op)
{
	BSVM_J2Stack_UnaryOp(ctx, op);
}

BGBSVM_API void BSVM_J2StackOp_BinaryOpName(bsjContext ctx, char *op)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_UnaryOpName(bsjContext ctx, char *op)
	{ BSVM_Jit2_StubError(ctx); }


BGBSVM_API void BSVM_J2StackOp_Call(bsjContext ctx)
{
	bsjReg ra, rb;
	int n;

	rb=BSVM_J2Stack_PopReg(ctx);
	
	n=BSVM_J2FrameX86_PopArgsDyt(ctx);
	ra=J2MemX86_GetRegTargDispDyt(ctx, 0);
//	ra=BSVM_J2StackOp_PopBuildList(ctx);
	BSVM_J2Dyt_CallFunc(ctx, ra, rb, ra, n);
	BSVM_J2Reg_FreeReg(rb);
	BSVM_J2Stack_PushReg(ctx, ra);
}

BGBSVM_API void BSVM_J2StackOp_TailCall(bsjContext ctx)
{
	bsjReg ra, rb;
	int n;

	rb=BSVM_J2Stack_PopReg(ctx);
	n=BSVM_J2FrameX86_PopArgsDyt(ctx);
	ra=J2MemX86_GetRegTargDispDyt(ctx, 0);
//	ra=BSVM_J2StackOp_PopBuildList(ctx);
	BSVM_J2Dyt_TailCallFunc(ctx, ra, rb, ra, n);
	BSVM_J2Reg_FreeReg(rb);
	BSVM_J2Stack_PushReg(ctx, ra);
}

BGBSVM_API void BSVM_J2StackOp_CallName(bsjContext ctx, char *str)
{
	bsjReg ra;
	int n;

	n=BSVM_J2FrameX86_PopArgsDyt(ctx);
	ra=J2MemX86_GetRegTargDispDyt(ctx, 0);
//	ra=BSVM_J2StackOp_PopBuildList(ctx);
	BSVM_J2Dyt_CallName(ctx, ra, str, ra, n);
	BSVM_J2Stack_PushReg(ctx, ra);
}

BGBSVM_API void BSVM_J2StackOp_TailCallName(bsjContext ctx, char *str)
{
	bsjReg ra;
	int n;

	n=BSVM_J2FrameX86_PopArgsDyt(ctx);
	ra=J2MemX86_GetRegTargDispDyt(ctx, 0);
//	ra=BSVM_J2StackOp_PopBuildList(ctx);
	BSVM_J2Dyt_TailCallName(ctx, ra, str, ra, n);
	BSVM_J2Stack_PushReg(ctx, ra);
}

BGBSVM_API void BSVM_J2StackOp_MethodCall(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }
BGBSVM_API void BSVM_J2StackOp_MethodTailCall(bsjContext ctx)
	{ BSVM_Jit2_StubError(ctx); }

BGBSVM_API void BSVM_J2StackOp_MethodCallName(bsjContext ctx, char *str)
{
	bsjReg ra, rb;
	int n;

	rb=BSVM_J2Stack_PopReg(ctx);
	n=BSVM_J2FrameX86_PopArgsDyt(ctx);
	ra=J2MemX86_GetRegTargDispDyt(ctx, 0);
//	ra=BSVM_J2StackOp_PopBuildList(ctx);
	BSVM_J2Dyt_MethodCallName(ctx, ra, rb, str, ra, n);
	BSVM_J2Reg_FreeReg(rb);
	BSVM_J2Stack_PushReg(ctx, ra);
}

BGBSVM_API void BSVM_J2StackOp_MethodTailCallName(bsjContext ctx, char *str)
{
	bsjReg ra, rb;
	int n;

	rb=BSVM_J2Stack_PopReg(ctx);
	n=BSVM_J2FrameX86_PopArgsDyt(ctx);
	ra=J2MemX86_GetRegTargDispDyt(ctx, 0);
//	ra=BSVM_J2StackOp_PopBuildList(ctx);
	BSVM_J2Dyt_MethodTailCallName(ctx, ra, rb, str, ra, n);
	BSVM_J2Reg_FreeReg(rb);
	BSVM_J2Stack_PushReg(ctx, ra);
}

BGBSVM_API void BSVM_J2StackOp_Return(bsjContext ctx)
{
	bsjVar tmp;
	tmp=BSVM_J2Stack_PopVar(ctx);
	if(!tmp)
	{
		BSVM_Jit2_CaseError(ctx);
		return;
	}
	BSVM_J2FrameX86_ReturnVar(ctx, tmp);
	BSVM_J2Vars_FreeVar(tmp);
}

BGBSVM_API void BSVM_J2StackOp_CallCF(bsjContext ctx)
{
	BSVM_J2StackOp_PushCurrentFunc(ctx);
	BSVM_J2StackOp_Call(ctx);
}

BGBSVM_API void BSVM_J2StackOp_TailCallCF(bsjContext ctx)
{
	BSVM_J2StackOp_PushCurrentFunc(ctx);
	BSVM_J2StackOp_TailCall(ctx);
}
