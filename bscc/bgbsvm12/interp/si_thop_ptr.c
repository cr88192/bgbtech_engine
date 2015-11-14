#include <bgbsvm.h>

BSVM_ThreadOp *BSVM_ThOp_ADD_XP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	DYLL_BoxPtr *bptr;
	s32 pi;

	BVT_PopU_Ty(s32, pi);
	bptr=(DYLL_BoxPtr *)dytfPtrvF(ctx->stack[ctx->stackpos-1]);
	bptr->ptr=((char *)bptr->ptr)+((pi)*bptr->step);
	BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_SUB_XP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	DYLL_BoxPtr *bptr;
	s32 pi;

	BVT_PopU_Ty(s32, pi);
	bptr=(DYLL_BoxPtr *)dytfPtrvF(ctx->stack[ctx->stackpos-1]);
	bptr->ptr=((char *)bptr->ptr)-((pi)*bptr->step);
	BVT_FreeS32(ctx, pi);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_ADD_XP_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	DYLL_BoxPtr *bptr;
	bptr=(DYLL_BoxPtr *)dytfPtrvF(ctx->stack[ctx->stackpos-1]);
	bptr->ptr=((char *)bptr->ptr)+(cur->i*bptr->step);
	return(cur->next);
}
BSVM_ThreadOp *BSVM_ThOp_SUB_XP_C(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	DYLL_BoxPtr *bptr;
	bptr=(DYLL_BoxPtr *)dytfPtrvF(ctx->stack[ctx->stackpos-1]);
	bptr->ptr=((char *)bptr->ptr)-(cur->i*bptr->step);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_DUP_XP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	DYLL_BoxPtr *bptr, *bptr2;

	bptr=(DYLL_BoxPtr *)dytfPtrvF(ctx->stack[ctx->stackpos-1]);
	bptr2=DYLL_TypeBox_AllocBoxPtr();
	(*bptr2)=(*bptr);
	ctx->stack[ctx->stackpos++]=dytfPtrF(bptr2);

//	bptr->ptr=((char *)bptr->ptr)-(cur->i*bptr->step);
//	ctx->stack[ctx->stackpos++]=
//		dyllTypeboxPtr(bptr->sig, bptr->ptr);
	return(cur->next);
}

BSVM_ThreadOp *BSVM_ThOp_LLOAD_XP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	DYLL_BoxPtr *bptr, *bptr2;
	dytf t;

	t=BSVM_LookupIdx(ctx, cur->i);
	bptr=(DYLL_BoxPtr *)dytfPtrvF(t);
	bptr2=DYLL_TypeBox_AllocBoxPtr();
	(*bptr2)=(*bptr);
	ctx->stack[ctx->stackpos++]=dytfPtrF(bptr2);

	return(cur->next); 
}

BSVM_ThreadOp *BSVM_ThOp_LSTORE_XP(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	DYLL_BoxPtr *bptr, *bptr2;
	dytf t;

	bptr=(DYLL_BoxPtr *)dytfPtrvF(ctx->stack[--ctx->stackpos]);
	t=BSVM_LookupIdx(ctx, cur->i);
	if(!dytfNullP(t))
	{
		bptr2=(DYLL_BoxPtr *)dytfPtrvF(t);
		(*bptr2)=(*bptr);
		DYLL_TypeBox_FreeBoxPtr(bptr);
	}else
	{
		BSVM_AssignIdx(ctx, cur->i, dytfPtrF(bptr));
	}
//	ctx->stack[ctx->stackpos++]=(dyt)bptr2;

	return(cur->next); 
}
