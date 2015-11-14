#include <bgbsvm.h>

BGBSVM_API bsjVar BSVM_J2Vars_AllocVar(bsjContext ctx)
{
	bsjVar tmp;
	tmp=gctalloc("bsvm_jit2_var_t", sizeof(BSVM_Jit2_VarInfo));
	tmp->ctx=ctx;
	return(tmp);
}

BGBSVM_API bsjMem BSVM_J2Vars_GetVarMem(bsjVar var)
{
	return(J2MemX86_GetVarMem(var));
}

BGBSVM_API void BSVM_J2Vars_FreeVar(bsjVar var)
{
	if(var->reg)
		BSVM_J2Reg_FreeReg(var->reg);
	gcfree(var);
}

BGBSVM_API bsjVar BSVM_J2Vars_CreateLocal(bsjContext ctx,
	char *name, bsjType type)
{
	bsjVar tmp;
	int sz, al;
	int i;

	tmp=BSVM_J2Vars_AllocVar(ctx);
	tmp->next=ctx->locals;
	ctx->locals=tmp;

	sz=BSVM_J2Type_GetTypeSize(type);
	al=BSVM_J2Type_GetTypeAlign(type);

	ctx->sz_local+=(al-1);
	ctx->sz_local&=~(al-1);
	i=ctx->sz_local;
	ctx->sz_local+=sz;

	tmp->name=dystrdup(name);
	tmp->type=type;
	tmp->offs=i;
	tmp->varty=BSVM_JIT2_VARTY_LOCAL;
	
	return(tmp);
}

BGBSVM_API bsjVar BSVM_J2Vars_CreateArgument(bsjContext ctx,
	char *name, bsjType type)
{
	bsjVar tmp;
	int sz, al;
	int i;

	tmp=BSVM_J2Vars_AllocVar(ctx);
	tmp->next=ctx->args;
	ctx->args=tmp;

	sz=BSVM_J2Type_GetTypeSizePtr(type);
	al=BSVM_J2Type_GetTypeAlignPtr(type);

	ctx->sz_args+=(al-1);
	ctx->sz_args&=~(al-1);
	i=ctx->sz_args;
	ctx->sz_args+=sz;

	tmp->name=dystrdup(name);
	tmp->type=type;
	tmp->offs=i;
	tmp->varty=BSVM_JIT2_VARTY_ARG;
	
	return(tmp);
}

BGBSVM_API void BSVM_J2Vars_FreeVars(bsjContext ctx)
{
	bsjVar cur, nxt;

	cur=ctx->locals;
	while(cur)
	{
		nxt=cur->next;
		BSVM_J2Vars_FreeVar(cur);
		cur=nxt;
	}

	cur=ctx->args;
	while(cur)
	{
		nxt=cur->next;
		BSVM_J2Vars_FreeVar(cur);
		cur=nxt;
	}
}


BGBSVM_API bsjVar BSVM_J2Vars_LexBind(bsjContext ctx,
	char *name, bsjType type)
{
	bsjVar tmp;
	tmp=BSVM_J2Vars_CreateLocal(ctx, name, type);
	tmp->lnext=ctx->lexenv;
	ctx->lexenv=tmp;
	return(tmp);
}

BGBSVM_API bsjVar BSVM_J2Vars_LexBindArg(bsjContext ctx,
	char *name, bsjType type)
{
	bsjVar tmp;
	
	tmp=BSVM_J2Vars_CreateArgument(ctx, name, type);
	tmp->lnext=ctx->lexenv;
	ctx->lexenv=tmp;
	return(tmp);
}

BGBSVM_API bsjVar BSVM_J2Vars_LexBindLocalSig(bsjContext ctx,
	char *name, char *sig, char *fl)
{
	bsjVar tmp;
	bsjType ty;
	
	ty=BSVM_J2Type_GetType(ctx, sig);
	tmp=BSVM_J2Vars_CreateLocal(ctx, name, ty);
	tmp->lnext=ctx->lexenv;
	ctx->lexenv=tmp;
	return(tmp);
}

BGBSVM_API bsjVar BSVM_J2Vars_LexBindArgSig(bsjContext ctx,
	char *name, char *sig, char *fl)
{
	bsjVar tmp;
	bsjType ty;
	
	ty=BSVM_J2Type_GetType(ctx, sig);
	tmp=BSVM_J2Vars_CreateArgument(ctx, name, ty);
	tmp->lnext=ctx->lexenv;
	ctx->lexenv=tmp;
	return(tmp);
}

BGBSVM_API bsjVar BSVM_J2Vars_IndexLexical(bsjContext ctx, int idx)
{
	bsjVar cur;
	int i;
	
	cur=ctx->lexenv; i=idx;
	while(cur && (i>0))
		{ cur=cur->lnext; i--; }
	return(cur);
}

BGBSVM_API bsjVar BSVM_J2Vars_LookupLexical(bsjContext ctx, char *name)
{
	bsjVar cur;
	int i;
	
	cur=ctx->lexenv;
	while(cur)
	{
		if(!strcmp(cur->name, name))
			return(cur);
		cur=cur->lnext;
	}
	return(NULL);
}

BGBSVM_API void BSVM_J2Vars_WriteVarReg(bsjContext ctx,
	bsjVar var, bsjReg reg, bsjType ty)
{
	bsjMem mem;
	mem=BSVM_J2Vars_GetVarMem(var);
	BSVM_J2Mem_MovMemReg(ctx, mem, reg, ty);
	BSVM_J2Mem_FreeMemRef(mem);
}

BGBSVM_API void BSVM_J2Vars_ReadRegVar(bsjContext ctx,
	bsjReg reg, bsjVar var, bsjType ty)
{
	bsjMem mem;
	mem=BSVM_J2Vars_GetVarMem(var);
	BSVM_J2Mem_MovRegMem(ctx, reg, mem, ty);
	BSVM_J2Mem_FreeMemRef(mem);
}

BGBSVM_API void BSVM_J2Vars_MapVarReg(bsjContext ctx,
	bsjVar var, bsjReg reg)
{
	if(var->reg)
	{
		if(var->reg==reg)
			return;
		BSVM_J2Reg_FreeReg(var->reg);
	}
	
	if(!BSVM_J2Type_TypeEqualP(var->type, reg->type))
	{
		bsj2_printlog(ctx, "BSVM_J2Vars_MapVarReg: %p %p, %s %s\n",
			var->type, reg->type,
			BSVM_J2Type_GetTypeSig(var->type),
			BSVM_J2Type_GetTypeSig(reg->type));
		BSVM_Jit2_TypeCheckError(ctx);

		reg=BSVM_J2Cast_CastReg(ctx, reg, var->type);
	}

	var->reg=reg;
	reg->mapvar=var;
}

BGBSVM_API void BSVM_J2Vars_MovVarReg(bsjContext ctx,
	bsjVar var, bsjReg reg, bsjType ty)
{
	if(!BSVM_J2Type_TypeEqualP(var->type, reg->type))
		BSVM_Jit2_TypeCheckError(ctx);

	if(var->reg)
	{
		if(var->reg==reg)return;
		BSVM_J2Reg_MovReg(ctx, var->reg, reg, ty);
		return;
	}
	
	BSVM_J2Vars_WriteVarReg(ctx, var, reg, ty);
}

BGBSVM_API void BSVM_J2Vars_MovRegVar(bsjContext ctx,
	bsjReg reg, bsjVar var, bsjType ty)
{
	if(!BSVM_J2Type_TypeEqualP(var->type, reg->type))
		BSVM_Jit2_TypeCheckError(ctx);

	if(var->reg)
	{
		if(var->reg==reg)return;
		BSVM_J2Reg_MovReg(ctx, reg, var->reg, ty);
		return;
	}
	
	BSVM_J2Vars_ReadRegVar(ctx, reg, var, ty);
}

BGBSVM_API bsjReg BSVM_J2Vars_LoadVarReg(bsjContext ctx, bsjVar var)
{
	bsjReg reg;
	
	reg=BSVM_J2Reg_ForceAllocReg(ctx, var->type);
	BSVM_J2Vars_MovRegVar(ctx, reg, var, var->type);
	return(reg);
}

BGBSVM_API bsjReg BSVM_J2Vars_LoadLexVarRegName(bsjContext ctx, char *str)
{
	bsjVar var;
	bsjReg reg;

	var=BSVM_J2Vars_LookupLexical(ctx, str);
	if(!var)return(NULL);
	reg=BSVM_J2Vars_LoadVarReg(ctx, var);
	return(reg);
}

BGBSVM_API bsjReg BSVM_J2Vars_LoadLexVarRegIdx(bsjContext ctx, int idx)
{
	bsjVar var;
	bsjReg reg;

	var=BSVM_J2Vars_IndexLexical(ctx, idx);
	if(!var)return(NULL);
	reg=BSVM_J2Vars_LoadVarReg(ctx, var);
	return(reg);
}
