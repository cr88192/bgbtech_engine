/**
  * TVars will map a variable to a given stack address and register.
  * A tvar will exist the entire length of a function.
  * Multiple tvars may share the same stack-space and/or register,
  * provided they don't overlap in time.
  */

#include <bgbsvm.h>

int BSVM_J2Var_TVarMapFrameSlots(bsjContext ctx, int szp, int alp)
{
	int i, j;
	
	if(alp<1)alp=1;
	for(i=0; i<ctx->n_var; i++)
	{
		//misalign
		if(i&(alp-1))
			continue;

		//check run
		for(j=0; j<szp; i++)
			if(ctx->var_mask[(i+j)>>3]&(1<<((i+j)&7)))
				break;
		if(j<szp)
			{ i+=j; continue; }

		//got one
		
		if((i+szp)>ctx->n_var)
			ctx->n_var=i+szp;

		for(j=0; j<szp; i++)
			ctx->var_mask[(i+j)>>3]|=(1<<((i+j)&7));
		return(i);
	}

	i=ctx->n_var;
	while(i&(alp-1))i++;

	if((i+szp)>ctx->n_var)
		ctx->n_var=i+szp;

	for(j=0; j<szp; i++)
		ctx->var_mask[(i+j)>>3]|=(1<<((i+j)&7));
	return(i);
}

void BSVM_J2Var_TVarMapFrame(bsjContext ctx, bsjVar var)
{
	int sz, al, psz;
	int szp, alp;
	int i;
	
	sz=BSVM_J2Type_GetTypeSize(var->type);
	al=BSVM_J2Type_GetTypeAlign(var->type);
	psz=BSVM_J2Type_PointerSize(ctx);
	szp=(sz+(psz-1))/psz;
	alp=(al+(psz-1))/psz;

	i=BSVM_J2Var_TVarMapFrameSlots(ctx, szp, alp);
	var->offs=i;

	BSVM_Jit2_StubError(ctx);
}

bsjVar BSVM_J2Var_AllocTVar(bsjContext ctx, bsjType ty)
{
	BSVM_Jit2_StubError(ctx);
}

void BSVM_J2Var_FreeTVar(bsjContext ctx, bsjVar var)
{
	BSVM_Jit2_StubError(ctx);
}
