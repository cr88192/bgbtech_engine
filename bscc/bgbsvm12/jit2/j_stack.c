#include <bgbsvm.h>

BGBSVM_API int BSVM_J2Stack_PushAdjustForType(bsjContext ctx, bsjType ty)
{
	int sz, al;
	int i;
	
//	sz=BSVM_J2Type_GetTypeSizePtr(ty);
//	al=BSVM_J2Type_GetTypeAlignPtr(ty);

	sz=BSVM_J2Type_GetTypeSize(ty);
	al=BSVM_J2Type_GetTypeAlign(ty);

	ctx->vstack_offs+=(al-1);
	ctx->vstack_offs&=~(al-1);
	i=ctx->vstack_offs;
	ctx->vstack_offs+=sz;

//	ctx->vstack_offs+=sz+(al-1);
//	ctx->vstack_offs&=~(al-1);

	if(ctx->vstack_offs>=ctx->sz_vstack)
		ctx->sz_vstack=ctx->vstack_offs;

	return(i);

//	ctx->nstack_offs-=sz;
//	ctx->nstack_offs&=~(al-1);
//	return(ctx->nstack_offs);
}

BGBSVM_API int BSVM_J2Stack_PopAdjustForLast(bsjContext ctx)
{
	ctx->stackpos--;
	ctx->vstack_offs=ctx->stack[ctx->stackpos]->offs;
	ctx->stack[ctx->stackpos]=NULL;
	return(ctx->vstack_offs);
}

BGBSVM_API bsjVar BSVM_J2Stack_PushType(bsjContext ctx, bsjType ty)
{
	bsjVar tmp;
	int i, j;

	tmp=BSVM_J2Vars_AllocVar(ctx);
	i=BSVM_J2Stack_PushAdjustForType(ctx, ty);
	tmp->type=ty;
	tmp->offs=i;
	tmp->varty=BSVM_JIT2_VARTY_STACK;
	tmp->value=NULL;
	ctx->stack[ctx->stackpos++]=tmp;
	return(tmp);
}

BGBSVM_API bsjVar BSVM_J2Stack_PopVar(bsjContext ctx)
{
	bsjVar tmp;
	
	if(ctx->stackpos<=0)
	{
		BSVM_Jit2_SanityCheckError(ctx, "Stack Underflow");
		return(NULL);
	}
	tmp=ctx->stack[ctx->stackpos-1];
	BSVM_J2Stack_PopAdjustForLast(ctx);
	return(tmp);
}

BGBSVM_API bsjVar BSVM_J2Stack_PeekVar(bsjContext ctx)
{
	bsjVar tmp;
	
	if(ctx->stackpos<=0)
	{
		BSVM_Jit2_SanityCheckError(ctx, "Stack Underflow");
		return(NULL);
	}
	tmp=ctx->stack[ctx->stackpos-1];
	return(tmp);
}

BGBSVM_API void BSVM_J2Stack_PopDestroy(bsjContext ctx)
{
	bsjVar tmp;
	tmp=BSVM_J2Stack_PopVar(ctx);
	if(!tmp)return;
	if(tmp->reg)BSVM_J2Reg_FreeReg(tmp->reg);
	BSVM_J2Vars_FreeVar(tmp);
}

/** Push a register to the stack. If it is mapped a copy will be pushed. */
BGBSVM_API bsjVar BSVM_J2Stack_PushReg(bsjContext ctx, bsjReg reg)
{
	bsjVar tmp;
	bsjMem tmem;

	tmp=BSVM_J2Stack_PushType(ctx, reg->type);
	if(tmp->mapvar)
	{
		tmem=BSVM_J2Vars_GetVarMem(tmp);
		BSVM_J2Mem_MovMemReg(ctx, tmem, reg, reg->type);
		BSVM_J2Mem_FreeMemRef(tmem);
	}else
	{
		BSVM_J2Reg_UnlockReg(reg);
		tmp->reg=reg;
		reg->mapvar=tmp;
	}
	return(tmp);
}

/** Pop a register from the stack. Will allocate a register if needed. */
BGBSVM_API bsjReg BSVM_J2Stack_PopReg(bsjContext ctx)
{
	bsjVar tmp;
	bsjMem tmem;
	bsjReg reg;
	dyt val;
	
	tmp=BSVM_J2Stack_PopVar(ctx);
	if(!tmp)
		return(NULL);
	if(tmp->reg)
	{
		reg=tmp->reg;
		BSVM_J2Reg_LockReg(reg);
		return(reg);
	}
	
	reg=BSVM_J2Reg_ForceAllocReg(ctx, tmp->type);
	if(!reg)return(NULL);

	if(tmp->value)
	{
		val=tmp->value;
		reg->value=tmp->value;
		if(val==BGBDY_AUXNULL)val=NULL;
		BSVM_J2Dyt_LoadReg(ctx, reg, val);
		return(reg);
	}

	tmem=BSVM_J2Vars_GetVarMem(tmp);
	BSVM_J2Mem_MovRegMem(ctx, reg, tmem, tmp->type);
	BSVM_J2Mem_FreeMemRef(tmem);
	BSVM_J2Vars_FreeVar(tmp);
	
	return(reg);
}

/** Free up memory used for stack */
BGBSVM_API void BSVM_J2Stack_FreeStack(bsjContext ctx)
{
	int i;
	
	for(i=0; i<256; i++)
	{
		if(ctx->stack[i])
		{
			BSVM_J2Vars_FreeVar(ctx->stack[i]);
			ctx->stack[i]=NULL;
		}
	}
}

/** Push a mark value onto the stack */
BGBSVM_API void BSVM_J2Stack_PushMark(bsjContext ctx)
{
	int i;

	i=ctx->markstackpos++;
	ctx->markstack[i]=ctx->stackpos;
}

BGBSVM_API void BSVM_J2Stack_PopToMark(bsjContext ctx)
{
	int i, j;
	
	i=ctx->markstack[ctx->markstackpos-1];
	while(ctx->stackpos>i)
		BSVM_J2Stack_PopDestroy(ctx);
	ctx->markstackpos--;
}

/** Push dynamically-typed value to stack */
BGBSVM_API void BSVM_J2Stack_PushDyt(bsjContext ctx, dyt v)
{
#if 0
	bsjVar tmp;
//	bsjMem tmem;
	bsjType ty;	

	if(!v)v=BGBDY_AUXNULL;

	ty=BSVM_J2Type_GetTypeDyt(ctx);
	tmp=BSVM_J2Stack_PushType(ctx, ty);
	tmp->value=v;
#endif

#if 1
	bsjReg tmp;
	bsjType ty;	

	ty=BSVM_J2Type_GetTypeDyt(ctx);
	tmp=BSVM_J2Reg_ForceAllocReg(ctx, ty);
	BSVM_J2Dyt_LoadReg(ctx, tmp, v);
	BSVM_J2Stack_PushReg(ctx, tmp);
#endif
}

