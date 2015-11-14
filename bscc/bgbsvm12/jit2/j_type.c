#include <bgbsvm.h>

BGBSVM_API BSVM_Jit2_TypeInfo *BSVM_Jit2_LookupTypeTop(
	BSVM_Jit2_TopContext *top, char *sig)
{
	BSVM_Jit2_TypeInfo *cur;

	cur=top->type;
	while(cur)
	{
		if(!strcmp(cur->sig, sig))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BGBSVM_API BSVM_Jit2_TypeInfo *BSVM_Jit2_GetTypeTop(
	BSVM_Jit2_TopContext *top, char *sig)
{
	BSVM_Jit2_TypeInfo *tmp;

	tmp=BSVM_Jit2_LookupTypeTop(top, sig);
	if(tmp)return(tmp);
	
	tmp=gctalloc("bsvm_jit2_typeinfo_t", sizeof(BSVM_Jit2_TypeInfo));
	tmp->top=top;
	tmp->sig=dystrdup(sig);
	
	tmp->next=top->type;
	top->type=tmp;
	return(tmp);
}

BGBSVM_API bsjType BSVM_J2Type_LookupType(bsjContext ctx, char *sig)
	{ return(BSVM_Jit2_LookupTypeTop(ctx->top, sig)); }

BGBSVM_API bsjType BSVM_J2Type_GetType(bsjContext ctx, char *sig)
	{ return(BSVM_Jit2_GetTypeTop(ctx->top, sig)); }

BGBSVM_API bsjType BSVM_J2Type_GetTypeDyt(bsjContext ctx)
{
	bsjType ty;	
	ty=BSVM_J2Type_GetType(ctx, "r");
	return(ty);
}

BGBSVM_API char *BSVM_J2Type_GetTypeSig(bsjType ty)
{
	if(!ty)return(NULL);
	return(ty->sig);
}

BGBSVM_API bool BSVM_J2Type_PointerP(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='P')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_VariantP(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='r')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_UnsignedP(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='o')
		return(true);
	if(*ty->sig=='y')
		return(true);
	if(*ty->sig=='j')
		return(true);
	if(*ty->sig=='t')
		return(true);
	if(*ty->sig=='h')
		return(true);
	if(*ty->sig=='w')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Int32P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='i')
		return(true);
	if(*ty->sig=='j')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Int16P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='s')
		return(true);
	if(*ty->sig=='t')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Int8P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='a')
		return(true);
	if(*ty->sig=='h')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallInt32P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Int32P(ty))
		return(true);
	if(BSVM_J2Type_Int16P(ty))
		return(true);
	if(BSVM_J2Type_Int8P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallInt16P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Int16P(ty))
		return(true);
	if(BSVM_J2Type_Int8P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallInt8P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Int8P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Int64P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='x')
		return(true);
	if(*ty->sig=='y')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Int128P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='n')
		return(true);
	if(*ty->sig=='o')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallInt64P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Int64P(ty))
		return(true);
	if(BSVM_J2Type_Int32P(ty))
		return(true);
	if(BSVM_J2Type_Int16P(ty))
		return(true);
	if(BSVM_J2Type_Int8P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallInt128P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Int128P(ty))
		return(true);
	if(BSVM_J2Type_Int64P(ty))
		return(true);
	if(BSVM_J2Type_Int32P(ty))
		return(true);
	if(BSVM_J2Type_Int16P(ty))
		return(true);
	if(BSVM_J2Type_Int8P(ty))
		return(true);
	return(false);
}


BGBSVM_API bool BSVM_J2Type_Float16P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='k')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Float32P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='f')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Float64P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='d')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_Float128P(bsjType ty)
{
	if(!ty)return(false);
	if(*ty->sig=='g')
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallFloat32P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Float32P(ty))
		return(true);
	if(BSVM_J2Type_Float16P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallFloat64P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Float64P(ty))
		return(true);
	if(BSVM_J2Type_Float32P(ty))
		return(true);
	if(BSVM_J2Type_Float16P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallFloat128P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_Float128P(ty))
		return(true);
	if(BSVM_J2Type_Float64P(ty))
		return(true);
	if(BSVM_J2Type_Float32P(ty))
		return(true);
	if(BSVM_J2Type_Float16P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallReal32P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_SmallFloat32P(ty))
		return(true);
	if(BSVM_J2Type_SmallInt32P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallReal64P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_SmallFloat64P(ty))
		return(true);
	if(BSVM_J2Type_SmallInt64P(ty))
		return(true);
	return(false);
}

BGBSVM_API bool BSVM_J2Type_SmallReal128P(bsjType ty)
{
	if(!ty)return(false);
	if(BSVM_J2Type_SmallFloat128P(ty))
		return(true);
	if(BSVM_J2Type_SmallInt128P(ty))
		return(true);
	return(false);
}

BGBSVM_API int BSVM_J2Type_GetTypeSize(bsjType ty)
{
	if(!ty)return(-1);
	return(dyllSigQuickSize(ty->sig));
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API int BSVM_J2Type_GetTypeAlign(bsjType ty)
{
	if(!ty)return(-1);
	return(dyllSigAlign(ty->sig));
//	BSVM_Jit2_StubError(ctx);
}

BGBSVM_API int BSVM_J2Type_PointerSizeTop(BSVM_Jit2_TopContext *ctx)
{
	bsjType ty;
	int i;
	
	ty=BSVM_Jit2_GetTypeTop(ctx, "Pv");
	i=BSVM_J2Type_GetTypeSize(ty);
	return(i);
}

BGBSVM_API int BSVM_J2Type_PointerSize(bsjContext ctx)
{
	bsjType ty;
	int i;
	
	ty=BSVM_J2Type_GetType(ctx, "Pv");
	i=BSVM_J2Type_GetTypeSize(ty);
	return(i);
}

BGBSVM_API int BSVM_J2Type_GetTypeSizePtr(bsjType ty)
{
	int i, j, k;
	i=BSVM_J2Type_GetTypeSize(ty);
	j=BSVM_J2Type_PointerSizeTop(ty->top);
	k=(i+(j-1))/j;
	return(k);
}

BGBSVM_API int BSVM_J2Type_GetTypeAlignPtr(bsjType ty)
{
	int i, j, k;
	i=BSVM_J2Type_GetTypeAlign(ty);
	j=BSVM_J2Type_PointerSizeTop(ty->top);
	k=(i+(j-1))/j;
	return(k);
}

BGBSVM_API bool BSVM_J2Type_TypeEqualP(bsjType lty, bsjType rty)
{
	if(!lty || !rty)
		return(false);
	if(lty==rty)
		return(true);
	if(!strcmp(lty->sig, rty->sig))
		return(true);
	return(false);
}
