#include <bgbsvm.h>

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];
extern char *bsvm_ops_sfis[];
extern int bsvm_ops_nops;

BSVM_SVMBlock *BSVM_GetCurrentBlock(BSVM_SVMState *ctx)
{
	BSVM_SVMBlock *blk;
	BSVM_SVMLambda *lmb;
	BSVM_SVMBuiltin *bfcn;
	char *ty;
	dyt f;
	
	f=dytfUnwrap(ctx->func);

	ty=dygettype(f);

	if(!strcmp(ty, "svm_blk_t"))
	{
		blk=(BSVM_SVMBlock *)f;
		return(blk);
	}

	if(!strcmp(ty, "svm_lambda_t"))
	{
		lmb=(BSVM_SVMLambda *)f;
		blk=lmb->blk;
		return(blk);
	}
	
	return(NULL);
}

BSVM_ThreadOp *BSVM_Thread_LookupTarget(BSVM_SVMState *ctx, byte *ip)
{
	BSVM_SVMBlock *blk;
	BSVM_ThreadOp *cur;

	blk=BSVM_GetCurrentBlock(ctx);
	if(!blk)return(NULL);
	
	cur=blk->thops;
	while(cur)
	{
		if(cur->ip==ip)
			return(cur);
		cur=cur->next;
	}
	
	return(NULL);
}

BSVM_ThreadOp *BSVM_Thread_GetTarget(BSVM_SVMState *ctx, byte *ip)
{
	BSVM_SVMBlock *blk;
	BSVM_ThreadOp *cur;

	cur=BSVM_Thread_LookupTarget(ctx, ip);
	if(cur)return(cur);

	blk=BSVM_GetCurrentBlock(ctx);
	if(!blk)return(NULL);
		
	cur=BSVM_Thread_DecodeBlock(ctx, blk);
	while(cur)
	{
		if(cur->ip==ip)
			return(cur);
		cur=cur->next;
	}
	
	return(NULL);
}

BSVM_ThreadOp *BSVM_Thread_GetOpJumpTarget(BSVM_SVMState *ctx,
	BSVM_ThreadOp *cur, int disp)
{
	BSVM_ThreadOp *tmp;
	byte *ip;

	ctx->ip=cur->ip;
	BSVM_SkipOpcode(ctx);
	ip=ctx->ip+disp;
	tmp=BSVM_Thread_GetTarget(ctx, ip);
	return(tmp);
}

bool BSVM_Thread_CacheOpJump(BSVM_SVMState *ctx, BSVM_ThreadOp *cur)
{
	if(cur->data)return(true);
	cur->data=(void *)BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
	if(cur->data)return(true);
	return(false);
}

int BSVM_Thread_DecodeOpcodeValue(BSVM_SVMState *ctx)
{
	int op;

	op=*ctx->ip++;
	if(op<192)
	{
		return(op);
	}
	
	if(op<240)
	{
		op=((op-192)<<8)+(*ctx->ip++);
		return(op);
	}

	if(op<252)
	{
		op=((op-240)<<8)+(*ctx->ip++);
		op=(op<<8)+(*ctx->ip++);
		return(op);
	}

	if(op<255)
	{
		op=((op-252)<<8)+(*ctx->ip++);
		op=(op<<8)+(*ctx->ip++);
		op=(op<<8)+(*ctx->ip++);
		return(op);
	}

	op=*ctx->ip++;
	op=(op<<8)+(*ctx->ip++);
	op=(op<<8)+(*ctx->ip++);
	op=(op<<8)+(*ctx->ip++);
	return(op);
	
//	if(op>=192)op=(op-192)*256+(*ctx->ip++);
}

void BSVM_Thread_DecodePrefix(BSVM_SVMState *ctx)
{
	byte *ip;
	int i, j;
	
	ip=ctx->ip;

	i=*ip++;
	if(i>=192)i=(i-192)*256+(*ip++);

	ctx->pfhint=0;
	ctx->pfsig=NULL;

	while(1)
	{
		j=0;
		switch(i)
		{
		case BSVM_SOP_PF_WIDE:
			ctx->pfhint|=BSVM_PF_WIDE; break;

		case BSVM_SOP_PF_HINT_XI:
			ctx->pfhint|=BSVM_PF_HINT_XI; break;
		case BSVM_SOP_PF_HINT_XL:
			ctx->pfhint|=BSVM_PF_HINT_XL; break;
		case BSVM_SOP_PF_HINT_XF:
			ctx->pfhint|=BSVM_PF_HINT_XF; break;
		case BSVM_SOP_PF_HINT_XD:
			ctx->pfhint|=BSVM_PF_HINT_XD; break;

		case BSVM_SOP_PF_HINT_XUBI: ctx->pfhint|=BSVM_PF_ARR_XUBI; break;
		case BSVM_SOP_PF_HINT_XSBI: ctx->pfhint|=BSVM_PF_ARR_XSBI; break;
		case BSVM_SOP_PF_HINT_XUSI: ctx->pfhint|=BSVM_PF_ARR_XUSI; break;
		case BSVM_SOP_PF_HINT_XSSI: ctx->pfhint|=BSVM_PF_ARR_XSSI; break;
		case BSVM_SOP_PF_HINT_XUDI: ctx->pfhint|=BSVM_PF_ARR_XUDI; break;
		case BSVM_SOP_PF_HINT_XSDI: ctx->pfhint|=BSVM_PF_ARR_XSDI; break;
		case BSVM_SOP_PF_HINT_XUDL: ctx->pfhint|=BSVM_PF_ARR_XUDL; break;
		case BSVM_SOP_PF_HINT_XSDL: ctx->pfhint|=BSVM_PF_ARR_XSDL; break;

		case BSVM_SOP_PF_HINT_XII: ctx->pfhint|=BSVM_PF_ARR_XII; break;
		case BSVM_SOP_PF_HINT_XLL: ctx->pfhint|=BSVM_PF_ARR_XLL; break;
		case BSVM_SOP_PF_HINT_XFF: ctx->pfhint|=BSVM_PF_ARR_XFF; break;
		case BSVM_SOP_PF_HINT_XDD: ctx->pfhint|=BSVM_PF_ARR_XDD; break;

		case BSVM_SOP_PF_HINT_XUBI_IX:
			ctx->pfhint|=BSVM_PF_ARR_XUBI|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XSBI_IX:
			ctx->pfhint|=BSVM_PF_ARR_XSBI|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XUSI_IX:
			ctx->pfhint|=BSVM_PF_ARR_XUSI|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XSSI_IX:
			ctx->pfhint|=BSVM_PF_ARR_XSSI|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XUDI_IX:
			ctx->pfhint|=BSVM_PF_ARR_XUDI|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XSDI_IX:
			ctx->pfhint|=BSVM_PF_ARR_XSDI|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XUDL_IX:
			ctx->pfhint|=BSVM_PF_ARR_XUDL|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XSDL_IX:
			ctx->pfhint|=BSVM_PF_ARR_XSDL|BSVM_PF_ARR_IX; break;

		case BSVM_SOP_PF_HINT_XII_IX:
			ctx->pfhint|=BSVM_PF_ARR_XII|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XLL_IX:
			ctx->pfhint|=BSVM_PF_ARR_XLL|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XFF_IX:
			ctx->pfhint|=BSVM_PF_ARR_XFF|BSVM_PF_ARR_IX; break;
		case BSVM_SOP_PF_HINT_XDD_IX:
			ctx->pfhint|=BSVM_PF_ARR_XDD|BSVM_PF_ARR_IX; break;

#if 1
		case BSVM_SOP_PF_HINT_PUB:
			ctx->pfhint|=BSVM_PF_ARR_XUBI|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PSB:
			ctx->pfhint|=BSVM_PF_ARR_XSBI|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PUS:
			ctx->pfhint|=BSVM_PF_ARR_XUSI|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PSS:
			ctx->pfhint|=BSVM_PF_ARR_XSSI|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PUD:
			ctx->pfhint|=BSVM_PF_ARR_XUDI|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PSD:
			ctx->pfhint|=BSVM_PF_ARR_XSDI|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PUL:
			ctx->pfhint|=BSVM_PF_ARR_XUDL|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PSL:
			ctx->pfhint|=BSVM_PF_ARR_XSDL|BSVM_PF_HINT_PTR; break;

		case BSVM_SOP_PF_HINT_PF:
			ctx->pfhint|=BSVM_PF_ARR_XFF|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PD:
			ctx->pfhint|=BSVM_PF_ARR_XDD|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PV:
			ctx->pfhint|=BSVM_PF_ARR_XUBI|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PP:
			ctx->pfhint|=BSVM_PF_ARR_XPP|BSVM_PF_HINT_PTR; break;
		case BSVM_SOP_PF_HINT_PTR:
			ctx->pfhint|=BSVM_PF_HINT_PTR; break;
#endif

		case BSVM_SOP_PF_HINT_S:
			ctx->pfsig=dysymbolv(dytfPtrvF(BSVM_ReadID(ctx)));
			break;

		default: j=1; break;
		}
		if(j)break;

		ctx->ip=ip;
		i=*ip++;
		if(i>=192)i=(i-192)*256+(*ip++);
	}
}

BSVM_ThreadOp *BSVM_Thread_DecodeStep(BSVM_SVMState *ctx)
{
	BSVM_ThreadOp *thop;
	byte *ip1;
	dytf t, u, v, w, x, y, z;
	s64 li, lj;
	int i, j, k, op;

//	printf("%d ", *ctx->ip);
//	BSVM_DumpOpcode(ctx);

//	BSVM_Thread_DecodePrefix(ctx);

#ifdef X86
	ip1=ctx->ip;	//save IP
	thop=BSVM_ThreadX86_DecodeStep(ctx);
	if(thop)return(thop);
	ctx->ip=ip1;	//restore IP
#endif

	thop=gctalloc("bsvm_threadop_t", sizeof(BSVM_ThreadOp));
	thop->ip=ctx->ip;

	BSVM_Thread_DecodePrefix(ctx);
	thop->pfhint=ctx->pfhint;

	op=*ctx->ip++;
	if(op>=192)op=(op-192)*256+(*ctx->ip++);

	thop->opnum=op;

//	switch(*ctx->ip++)
	switch(op)
	{
	case BSVM_SOP_NOP:
		thop->fcn=BSVM_ThOp_NOP; break;
	case BSVM_SOP_BLOCK:
		thop->fcn=BSVM_ThOp_BLOCK; break;
	case BSVM_SOP_DBGMARK:
		thop->t=BSVM_ReadID(ctx);
//		thop->u=BSVM_ReadID(ctx);
		thop->i=BSVM_ReadIDX(ctx);
		thop->u=dytfInt(thop->i);
		thop->fcn=BSVM_ThOp_DBGMARK; break;
	case BSVM_SOP_CGENAST:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_CGENAST; break;
	case BSVM_SOP_FN:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_FN; break;
	case BSVM_SOP_LN:
//		thop->t=BSVM_ReadID(ctx);
		thop->i=BSVM_ReadIDX(ctx);
		thop->t=dytfInt(thop->i);
		thop->fcn=BSVM_ThOp_LN; break;
	case BSVM_SOP_LABEL:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LABEL; break;

	case BSVM_SOP_SLEEP:
		thop->fcn=BSVM_ThOp_SLEEP; break;
	case BSVM_SOP_SLEEP_SV:
		thop->i=BSVM_ReadShort(ctx);
		thop->fcn=BSVM_ThOp_SLEEP_SV; break;
	case BSVM_SOP_ASSERT:
		thop->fcn=BSVM_ThOp_ASSERT; break;
	case BSVM_SOP_JOIN:
		thop->fcn=BSVM_ThOp_JOIN; break;
	case BSVM_SOP_MULTIJOIN:
		thop->fcn=BSVM_ThOp_MULTIJOIN; break;

	case BSVM_SOP_PUSH:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_PUSH; break;
	case BSVM_SOP_POP:
		thop->fcn=BSVM_ThOp_POP; break;
	case BSVM_SOP_LOAD:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LOAD; break;
	case BSVM_SOP_STORE:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_STORE; break;
	case BSVM_SOP_BIND:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_BIND; break;
	case BSVM_SOP_DYNBIND:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_DYNBIND; break;
	case BSVM_SOP_LEXBIND:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LEXBIND; break;
	case BSVM_SOP_LOADA_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LOADA_S; break;
	case BSVM_SOP_LOADB_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LOADB_S; break;

	case BSVM_SOP_LOADINDEX:
		BSVM_ThOp_DecodeLoadIndex(ctx, thop); break;
//		thop->fcn=BSVM_ThOp_LOADINDEX; break;
		
	case BSVM_SOP_STOREINDEX:
		BSVM_ThOp_DecodeStoreIndex(ctx, thop); break;
//		thop->fcn=BSVM_ThOp_STOREINDEX; break;
	case BSVM_SOP_LOADINDEXA:
		thop->fcn=BSVM_ThOp_LOADINDEXA; break;

	case BSVM_SOP_LOADINDEX_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LOADINDEX_S; break;
	case BSVM_SOP_STOREINDEX_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_STOREINDEX_S; break;
	case BSVM_SOP_LOADINDEXA_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LOADINDEXA_S; break;
	case BSVM_SOP_LOADINDEXB_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_LOADINDEXB_S; break;

	case BSVM_SOP_LOADINDEX_I:
		i=BSVM_ReadArrayIndex(ctx);
		BSVM_ThOp_DecodeLoadIndexI(ctx, thop, i);
		break;
//		thop->t=dyIntF(*ctx->ip++);
//		thop->i=BSVM_ReadArrayIndex(ctx);
//		thop->t=dytfInt(thop->i);
//		thop->fcn=BSVM_ThOp_LOADINDEX_I; break;
	case BSVM_SOP_STOREINDEX_I:
		i=BSVM_ReadArrayIndex(ctx);
		BSVM_ThOp_DecodeStoreIndexI(ctx, thop, i);
		break;
//		thop->t=dyIntF(*ctx->ip++);
//		thop->i=BSVM_ReadArrayIndex(ctx);
//		thop->t=dytfInt(thop->i);
//		thop->fcn=BSVM_ThOp_STOREINDEX_I; break;
	case BSVM_SOP_LOADINDEXA_I:
//		thop->t=dyIntF(*ctx->ip++);
		thop->i=BSVM_ReadArrayIndex(ctx);
		thop->t=dytfInt(thop->i);
		thop->fcn=BSVM_ThOp_LOADINDEXA_I; break;
	case BSVM_SOP_LOADINDEXB_I:
//		thop->t=dyIntF(*ctx->ip++);
		thop->i=BSVM_ReadArrayIndex(ctx);
		thop->t=dytfInt(thop->i);
		thop->fcn=BSVM_ThOp_LOADINDEXB_I; break;

	case BSVM_SOP_LOADINDEX_0:
		thop->fcn=BSVM_ThOp_LOADINDEX_0; break;
	case BSVM_SOP_LOADINDEX_1:
		thop->fcn=BSVM_ThOp_LOADINDEX_1; break;
	case BSVM_SOP_LOADINDEX_2:
		thop->fcn=BSVM_ThOp_LOADINDEX_2; break;
	case BSVM_SOP_LOADINDEX_3:
		thop->fcn=BSVM_ThOp_LOADINDEX_3; break;

	case BSVM_SOP_STOREINDEX_0:
		thop->fcn=BSVM_ThOp_STOREINDEX_0; break;
	case BSVM_SOP_STOREINDEX_1:
		thop->fcn=BSVM_ThOp_STOREINDEX_1; break;
	case BSVM_SOP_STOREINDEX_2:
		thop->fcn=BSVM_ThOp_STOREINDEX_2; break;
	case BSVM_SOP_STOREINDEX_3:
		thop->fcn=BSVM_ThOp_STOREINDEX_3; break;

	case BSVM_SOP_EXCH_SELF:
		thop->fcn=BSVM_ThOp_EXCH_SELF; break;
	case BSVM_SOP_EXCH_TOP:
		thop->fcn=BSVM_ThOp_EXCH_TOP; break;

	case BSVM_SOP_PUSH_SELF:
		thop->fcn=BSVM_ThOp_PUSH_SELF; break;
	case BSVM_SOP_PUSH_NULL:
		thop->fcn=BSVM_ThOp_PUSH_NULL; break;
	case BSVM_SOP_PUSH_TRUE:
		thop->fcn=BSVM_ThOp_PUSH_TRUE; break;
	case BSVM_SOP_PUSH_FALSE:
		thop->fcn=BSVM_ThOp_PUSH_FALSE; break;
	case BSVM_SOP_PUSH_SV:
		thop->i=BSVM_ReadShort(ctx);
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, thop->i))break;
		thop->fcn=BSVM_ThOp_PUSH_SV; break;
	case BSVM_SOP_PUSH_SV_P:
		thop->i=*ctx->ip++;
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, thop->i))break;
		thop->fcn=BSVM_ThOp_PUSH_SV; break;
	case BSVM_SOP_PUSH_SV_N:
		thop->i=-(*ctx->ip++);
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, thop->i))break;
		thop->fcn=BSVM_ThOp_PUSH_SV; break;

	case BSVM_SOP_PUSH_XI:
		li=BSVM_ReadSVLI(ctx);
		if(((int)li)==li)
			{ thop->i=li; thop->fcn=BSVM_ThOp_PUSH_XI_C; break; }
		thop->t=dytfInt(li);
		thop->fcn=BSVM_ThOp_PUSH_XI; break;
	case BSVM_SOP_PUSH_XL:
		li=BSVM_ReadSVLI(ctx);
		if(((int)li)==li)
			{ thop->i=li; thop->fcn=BSVM_ThOp_PUSH_XL_C; break; }
		thop->t=dytfLong(li);
		thop->fcn=BSVM_ThOp_PUSH_XL; break;

	case BSVM_SOP_PUSH_XF:
		li=BSVM_ReadSVLI(ctx);
		lj=BSVM_ReadSVLI(ctx);
		thop->t=dytfFloat(li*pow(2, lj));
		thop->fcn=BSVM_ThOp_PUSH_XF; break;
	case BSVM_SOP_PUSH_XD:
		li=BSVM_ReadSVLI(ctx);
		lj=BSVM_ReadSVLI(ctx);
		thop->t=dytfDouble(li*pow(2, lj));
		thop->fcn=BSVM_ThOp_PUSH_XD; break;

	case BSVM_SOP_PUSH_SV_F:
		thop->i=BSVM_ReadShort(ctx);
		thop->fcn=BSVM_ThOp_PUSH_SV_F; break;
	case BSVM_SOP_PUSH_SV_FB:
		thop->i=BSVM_ReadShort(ctx);
		thop->fcn=BSVM_ThOp_PUSH_SV_FB; break;
	case BSVM_SOP_PUSH_SV_C:
		thop->i=BSVM_ReadShort(ctx);
		thop->fcn=BSVM_ThOp_PUSH_SV_C; break;

	case BSVM_SOP_PUSH_0:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 0))break;
		thop->fcn=BSVM_ThOp_PUSH_0; break;
	case BSVM_SOP_PUSH_1:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 1))break;
		thop->fcn=BSVM_ThOp_PUSH_1; break;
	case BSVM_SOP_PUSH_2:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 2))break;
		thop->fcn=BSVM_ThOp_PUSH_2; break;
	case BSVM_SOP_PUSH_3:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 3))break;
		thop->fcn=BSVM_ThOp_PUSH_3; break;
	case BSVM_SOP_PUSH_4:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 4))break;
		thop->fcn=BSVM_ThOp_PUSH_4; break;
	case BSVM_SOP_PUSH_5:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 5))break;
		thop->fcn=BSVM_ThOp_PUSH_5; break;
	case BSVM_SOP_PUSH_6:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 6))break;
		thop->fcn=BSVM_ThOp_PUSH_6; break;
	case BSVM_SOP_PUSH_7:
		if(BSVM_ThOp_DecodePushConstant(ctx, thop, 7))break;
		thop->fcn=BSVM_ThOp_PUSH_7; break;

	case BSVM_SOP_PUSH_CF: thop->fcn=BSVM_ThOp_PUSH_CF; break;

	case BSVM_SOP_LLOAD:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
		{
			if(ctx->pfhint&BSVM_PF_HINT_XI)
				{ thop->fcn=BSVM_ThOp_LLOAD_XI_VS; break; }
			if(ctx->pfhint&BSVM_PF_HINT_XL)
				{ thop->fcn=BSVM_ThOp_LLOAD_XL_VS; break; }
			if(ctx->pfhint&BSVM_PF_HINT_XF)
				{ thop->fcn=BSVM_ThOp_LLOAD_XF_VS; break; }
			if(ctx->pfhint&BSVM_PF_HINT_XD)
				{ thop->fcn=BSVM_ThOp_LLOAD_XD_VS; break; }
		}
#endif
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_LLOAD_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_LLOAD_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_LLOAD_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_LLOAD_XD; break; }
		if(ctx->pfhint&BSVM_PF_HINT_PTR)
			{ thop->fcn=BSVM_ThOp_LLOAD_XP; break; }
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LLOAD_VS; break; }
#endif
		thop->fcn=BSVM_ThOp_LLOAD; break;
	case BSVM_SOP_LSTORE:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_LSTORE_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_LSTORE_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_LSTORE_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_LSTORE_XD; break; }
		if(ctx->pfhint&BSVM_PF_HINT_PTR)
			{ thop->fcn=BSVM_ThOp_LSTORE_XP; break; }
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LSTORE_VS; break; }
#endif
		thop->fcn=BSVM_ThOp_LSTORE;
		break;
	case BSVM_SOP_LLOAD_F:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LLOAD_FVS; break; }
#endif
		thop->fcn=BSVM_ThOp_LLOAD_F;
		break;
	case BSVM_SOP_LSTORE_F:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LSTORE_FVS; break; }
#endif
		thop->fcn=BSVM_ThOp_LSTORE_F;
		break;

	case BSVM_SOP_LPOSTINC:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LPOSTINC;
		break;
	case BSVM_SOP_LPOSTDEC:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LPOSTDEC;
		break;

	case BSVM_SOP_LPREINC:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LPREINC;
		break;
	case BSVM_SOP_LPREDEC:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LPREDEC;
		break;

//	case BSVM_SOP_LINC_FN:
	case BSVM_SOP_LINC:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
		{
			if(ctx->pfhint&BSVM_PF_HINT_XI)
				{ thop->fcn=BSVM_ThOp_LINC_XI_VS; break; }
			if(ctx->pfhint&BSVM_PF_HINT_XL)
				{ thop->fcn=BSVM_ThOp_LINC_XL_VS; break; }
		}
#endif
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_LINC_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_LINC_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_LINC_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_LINC_XD; break; }
		thop->fcn=BSVM_ThOp_LINC_FN;
		break;
//	case BSVM_SOP_LDEC_FN:
	case BSVM_SOP_LDEC:
//		thop->i=*ctx->ip++;
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
		{
			if(ctx->pfhint&BSVM_PF_HINT_XI)
				{ thop->fcn=BSVM_ThOp_LDEC_XI_VS; break; }
			if(ctx->pfhint&BSVM_PF_HINT_XL)
				{ thop->fcn=BSVM_ThOp_LDEC_XL_VS; break; }
		}
#endif
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_LDEC_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_LDEC_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_LDEC_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_LDEC_XD; break; }
		thop->fcn=BSVM_ThOp_LDEC_FN;
		break;

	case BSVM_SOP_CLEARENV:
		thop->fcn=BSVM_ThOp_CLEARENV;
		break;

	case BSVM_SOP_BINDPATTERN: break;
	case BSVM_SOP_LOADMINDEX: break;
	case BSVM_SOP_STOREMINDEX: break;

	case BSVM_SOP_TRYLOAD:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_TRYLOAD;
		break;

	case BSVM_SOP_TRYCAST_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_TRYCAST_S;
		break;
	case BSVM_SOP_CAST_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_CAST_S;
		break;
	case BSVM_SOP_CAST:
		thop->fcn=BSVM_ThOp_CAST;
		break;
	case BSVM_SOP_SIZEOF_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_SIZEOF_S;
		break;
	case BSVM_SOP_SIZEOF:
		thop->fcn=BSVM_ThOp_SIZEOF;
		break;
	case BSVM_SOP_INSTANCEOF_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_INSTANCEOF_S;
		break;

	case BSVM_SOP_MARK:
		thop->fcn=BSVM_ThOp_MARK;
		break;
	case BSVM_SOP_TYMARK:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_TYMARK;
		break;

	case BSVM_SOP_CALL:
		thop->fcn=BSVM_ThOp_CALL;
		break;

	case BSVM_SOP_TAILCALL:
		thop->fcn=BSVM_ThOp_TAILCALL;
		break;
	case BSVM_SOP_CALL_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_CALL_S;
		break;
	case BSVM_SOP_TAILCALL_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_TAILCALL_S;
		break;

	case BSVM_SOP_METHODCALL:
		thop->fcn=BSVM_ThOp_METHODCALL;
		break;
	case BSVM_SOP_METHODTAILCALL:
		thop->fcn=BSVM_ThOp_METHODTAILCALL;
		break;
	case BSVM_SOP_METHODCALL_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_METHODCALL_S;
		break;
	case BSVM_SOP_METHODTAILCALL_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_METHODTAILCALL_S;
		break;

	case BSVM_SOP_CALL_THIS:
		thop->fcn=BSVM_ThOp_CALL_THIS;
		break;
	case BSVM_SOP_CALL_SUPER:
		thop->fcn=BSVM_ThOp_CALL_SUPER;
		break;

	case BSVM_SOP_CALL_ASYNC:
		thop->fcn=BSVM_ThOp_CALL_ASYNC;
		break;
	case BSVM_SOP_CALL_IFDEF:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_CALL_IFDEF;
		break;
	case BSVM_SOP_CALL_IFNDEF:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_CALL_IFNDEF;
		break;

	case BSVM_SOP_BEGIN_SYNC:
		thop->fcn=BSVM_ThOp_BEGIN_SYNC;
		break;
	case BSVM_SOP_END_SYNC:
		thop->fcn=BSVM_ThOp_END_SYNC;
		break;
	case BSVM_SOP_BEGIN_SYNC_OBJ:
	case BSVM_SOP_LOCK_OBJ:
		thop->fcn=BSVM_ThOp_LOCK_OBJ;
		break;
	case BSVM_SOP_END_SYNC_OBJ:
	case BSVM_SOP_UNLOCK_OBJ:
		thop->fcn=BSVM_ThOp_UNLOCK_OBJ;
		break;

	case BSVM_SOP_BEGIN_TRY:
		thop->fcn=BSVM_ThOp_BEGIN_TRY; break;
	case BSVM_SOP_END_TRY:
		thop->fcn=BSVM_ThOp_END_TRY; break;
	case BSVM_SOP_END_CATCH:
		thop->fcn=BSVM_ThOp_END_CATCH; break;

	case BSVM_SOP_BEGIN_CATCH_FINAL:
		thop->fcn=BSVM_ThOp_BEGIN_CATCH_FINAL;
		break;
	case BSVM_SOP_BEGIN_CATCH:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_BEGIN_CATCH;
		break;

	case BSVM_SOP_THROW_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_THROW_S;
		break;
	case BSVM_SOP_THROW_OBJ:
		thop->fcn=BSVM_ThOp_THROW_OBJ;
		break;

	case BSVM_SOP_CATCH:
		thop->t=BSVM_ReadID(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_CATCH;
		break;

	case BSVM_SOP_NEW_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_NEW_S;
		break;
	case BSVM_SOP_NEWARRAY_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_NEWARRAY_S;
		break;
	case BSVM_SOP_DELETE:
		thop->fcn=BSVM_ThOp_DELETE;
		break;
	case BSVM_SOP_CLONE:
		thop->fcn=BSVM_ThOp_CLONE;
		break;
	case BSVM_SOP_TOSTRING:
		thop->fcn=BSVM_ThOp_TOSTRING;
		break;
	case BSVM_SOP_UNQUOTE:
		thop->fcn=BSVM_ThOp_UNQUOTE;
		break;

	case BSVM_SOP_NEG:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_NEG_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_NEG_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_NEG_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_NEG_XD; break; }
		thop->fcn=BSVM_ThOp_NEG; break;
	case BSVM_SOP_LNOT:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_LNOT_XI; break; }
		thop->fcn=BSVM_ThOp_LNOT; break;
	case BSVM_SOP_DEFER:
		BSVM_ThOp_DecodeDefer(ctx, thop); break;
//		thop->fcn=BSVM_ThOp_DEFER; break;
	case BSVM_SOP_SETDEFER:
		BSVM_ThOp_DecodeSetDefer(ctx, thop); break;
//		thop->fcn=BSVM_ThOp_SETDEFER; break;
	case BSVM_SOP_NOT:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_NOT_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_NOT_XL; break; }
		thop->fcn=BSVM_ThOp_NOT; break;
	case BSVM_SOP_INC:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->i=1; thop->fcn=BSVM_ThOp_ADD_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->i=1; thop->fcn=BSVM_ThOp_ADD_XL_C; break; }
		thop->fcn=BSVM_ThOp_INC; break;
	case BSVM_SOP_DEC:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->i=1; thop->fcn=BSVM_ThOp_SUB_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->i=1; thop->fcn=BSVM_ThOp_SUB_XL_C; break; }
		thop->fcn=BSVM_ThOp_DEC; break;
	case BSVM_SOP_INC2:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->i=2; thop->fcn=BSVM_ThOp_ADD_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->i=2; thop->fcn=BSVM_ThOp_ADD_XL_C; break; }
		thop->fcn=BSVM_ThOp_INC2; break;
	case BSVM_SOP_DEC2:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->i=2; thop->fcn=BSVM_ThOp_SUB_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->i=2; thop->fcn=BSVM_ThOp_SUB_XL_C; break; }
		thop->fcn=BSVM_ThOp_DEC2; break;

	case BSVM_SOP_INC_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_INC_S;
		break;
	case BSVM_SOP_DEC_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_DEC_S;
		break;
	case BSVM_SOP_POSTINC_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_POSTINC_S;
		break;
	case BSVM_SOP_POSTDEC_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_PREDEC_S;
		break;
	case BSVM_SOP_PREINC_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_PREINC_S;
		break;
	case BSVM_SOP_PREDEC_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_PREDEC_S;
		break;

	case BSVM_SOP_TYBIND:
		thop->t=BSVM_ReadID(ctx);
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_TYBIND;
		break;
	case BSVM_SOP_TYDYNBIND:
		thop->t=BSVM_ReadID(ctx);
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_TYDYNBIND;
		break;
	case BSVM_SOP_TYLEXBIND:
		thop->t=BSVM_ReadID(ctx);
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_TYLEXBIND;
		break;

	case BSVM_SOP_BEGIN_PACKAGE:
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_BEGIN_PACKAGE;
		break;
	case BSVM_SOP_BEGIN_CLASS:
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_BEGIN_CLASS;
		break;
	case BSVM_SOP_BEGIN_IFACE:
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_BEGIN_IFACE;
		break;
	case BSVM_SOP_BEGIN_STRUCT:
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_BEGIN_STRUCT;
		break;
	case BSVM_SOP_END_PACKAGE:
		thop->fcn=BSVM_ThOp_END_PACKAGE; break;
	case BSVM_SOP_END_CLASS:
		thop->fcn=BSVM_ThOp_END_CLASS; break;
	case BSVM_SOP_END_IFACE:
		thop->fcn=BSVM_ThOp_END_IFACE; break;
	case BSVM_SOP_END_STRUCT:
		thop->fcn=BSVM_ThOp_END_STRUCT; break;
	case BSVM_SOP_CLASS_IFACE:
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_CLASS_IFACE; break;
	case BSVM_SOP_IMPORT:
		thop->u=BSVM_ReadID(ctx);
		thop->v=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_IMPORT; break;

	case BSVM_SOP_RET:
		thop->fcn=BSVM_ThOp_RET;
		break;

	case BSVM_SOP_CALL_CF:
		thop->fcn=BSVM_ThOp_CALL_CF;
		break;
	case BSVM_SOP_TAILCALL_CF:
		thop->fcn=BSVM_ThOp_TAILCALL_CF;
		break;

	case BSVM_SOP_CAR:
		thop->fcn=BSVM_ThOp_CAR; break;
	case BSVM_SOP_CDR:
		thop->fcn=BSVM_ThOp_CDR; break;
	case BSVM_SOP_CAAR:
		thop->fcn=BSVM_ThOp_CAAR; break;
	case BSVM_SOP_CDAR:
		thop->fcn=BSVM_ThOp_CDAR; break;
	case BSVM_SOP_CADR:
		thop->fcn=BSVM_ThOp_CADR; break;
	case BSVM_SOP_CDDR:
		thop->fcn=BSVM_ThOp_CDDR; break;
	case BSVM_SOP_CONS:
		thop->fcn=BSVM_ThOp_CONS; break;
	case BSVM_SOP_CLOSE:
		thop->fcn=BSVM_ThOp_CLOSE; break;
	case BSVM_SOP_ENVCLOSE:
		thop->fcn=BSVM_ThOp_ENVCLOSE; break;

	case BSVM_SOP_DUP:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_DUP_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_DUP_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_DUP_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_DUP_XD; break; }
		if(ctx->pfhint&BSVM_PF_HINT_PTR)
			{ thop->fcn=BSVM_ThOp_DUP_XP; break; }
		thop->fcn=BSVM_ThOp_DUP; break;

	case BSVM_SOP_DUP_F:
		thop->fcn=BSVM_ThOp_DUP_F;
		break;
	case BSVM_SOP_EXCH:
		thop->fcn=BSVM_ThOp_EXCH;
		break;
	case BSVM_SOP_INDEX:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_INDEX;
		break;
	case BSVM_SOP_RINDEX:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_RINDEX;
		break;
	case BSVM_SOP_SETINDEX:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_SETINDEX;
		break;
	case BSVM_SOP_SETRINDEX:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_SETRINDEX;
		break;
	case BSVM_SOP_ROLL_I:
		thop->i=BSVM_ReadIDX(ctx);
		thop->j=BSVM_ReadSIDX(ctx);
		thop->fcn=BSVM_ThOp_ROLL_I;
		break;
	case BSVM_SOP_COPY_I:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_COPY_I;
		break;

#if 0
	case BSVM_SOP_DUP_R:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_DUP_R;
		break;
	case BSVM_SOP_DUP_RF:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_DUP_RF;
		break;
	case BSVM_SOP_PUSH_R:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_PUSH_R;
		break;
	case BSVM_SOP_PUSH_RF:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_PUSH_RF;
		break;
	case BSVM_SOP_POP_R:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_POP_R;
		break;
#endif

	case BSVM_SOP_UNARYOP:
		i=*ctx->ip++;
		BSVM_ThOp_DecodeUnaryOpI(ctx, thop, i);
//		thop->i=*ctx->ip++;
//		thop->fcn=BSVM_ThOp_UNARYOP;
		break;
	case BSVM_SOP_BINARYOP:
		i=*ctx->ip++;
		BSVM_ThOp_DecodeBinaryOpI(ctx, thop, i);
//		thop->i=*ctx->ip++;
//		thop->fcn=BSVM_ThOp_BINARYOP;
		break;
	case BSVM_SOP_UNARYOP_S:
		t=BSVM_ReadID(ctx);
		BSVM_ThOp_DecodeUnaryOpS(ctx, thop, t);
//		thop->t=BSVM_ReadID(ctx);
//		thop->fcn=BSVM_ThOp_UNARYOP_S;
		break;
	case BSVM_SOP_BINARYOP_S:
		t=BSVM_ReadID(ctx);
		BSVM_ThOp_DecodeBinaryOpS(ctx, thop, t);
//		thop->t=BSVM_ReadID(ctx);
//		thop->fcn=BSVM_ThOp_BINARYOP_S;
		break;

	case BSVM_SOP_UNARYINTRIN_S:
		t=BSVM_ReadID(ctx);
		BSVM_ThOp_DecodeUnaryIntrinS(ctx, thop, t);
		break;
	case BSVM_SOP_BINARYINTRIN_S:
		t=BSVM_ReadID(ctx);
		BSVM_ThOp_DecodeBinaryIntrinS(ctx, thop, t);
		break;
	case BSVM_SOP_TRINARYINTRIN_S:
		t=BSVM_ReadID(ctx);
		BSVM_ThOp_DecodeTrinaryIntrinS(ctx, thop, t);
		break;

	case BSVM_SOP_ADD:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_ADD_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_ADD_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_ADD_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_ADD_XD; break; }
		if(ctx->pfhint&BSVM_PF_HINT_PTR)
			{ thop->fcn=BSVM_ThOp_ADD_XP; break; }
		thop->fcn=BSVM_ThOp_ADD; break;

	case BSVM_SOP_SUB:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SUB_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SUB_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_SUB_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_SUB_XD; break; }
		if(ctx->pfhint&BSVM_PF_HINT_PTR)
			{ thop->fcn=BSVM_ThOp_SUB_XP; break; }
		thop->fcn=BSVM_ThOp_SUB; break;

	case BSVM_SOP_MUL:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_MUL_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_MUL_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_MUL_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_MUL_XD; break; }
		thop->fcn=BSVM_ThOp_MUL; break;

	case BSVM_SOP_DIV:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_DIV_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_DIV_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_DIV_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_DIV_XD; break; }
		thop->fcn=BSVM_ThOp_DIV; break;

	case BSVM_SOP_MOD:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_MOD_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_MOD_XL; break; }
		thop->fcn=BSVM_ThOp_MOD; break;
	case BSVM_SOP_AND:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_AND_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_AND_XL; break; }
		thop->fcn=BSVM_ThOp_AND; break;
	case BSVM_SOP_OR:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_OR_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_OR_XL; break; }
		thop->fcn=BSVM_ThOp_OR; break;
	case BSVM_SOP_XOR:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_XOR_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_XOR_XL; break; }
		thop->fcn=BSVM_ThOp_XOR; break;
	case BSVM_SOP_SHL:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SHL_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SHL_XL; break; }
		thop->fcn=BSVM_ThOp_SHL; break;
	case BSVM_SOP_SHR:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SHR_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SHR_XL; break; }
		thop->fcn=BSVM_ThOp_SHR; break;
	case BSVM_SOP_SHRR:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SHRR_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SHRR_XL; break; }
		thop->fcn=BSVM_ThOp_SHRR; break;

	case BSVM_SOP_CMP_EQ:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_EQ_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_EQ_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_CMP_EQ_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_CMP_EQ_XD; break; }
		thop->fcn=BSVM_ThOp_CMP_E; break;

	case BSVM_SOP_CMP_NE:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_NE_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_NE_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_CMP_NE_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_CMP_NE_XD; break; }
		thop->fcn=BSVM_ThOp_CMP_NE; break;

	case BSVM_SOP_CMP_LT:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_LT_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_LT_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_CMP_LT_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_CMP_LT_XD; break; }
		thop->fcn=BSVM_ThOp_CMP_L; break;

	case BSVM_SOP_CMP_GT:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_GT_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_GT_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_CMP_GT_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_CMP_GT_XD; break; }
		thop->fcn=BSVM_ThOp_CMP_G; break;

	case BSVM_SOP_CMP_LE:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_LE_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_LE_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_CMP_LE_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_CMP_LE_XD; break; }
		thop->fcn=BSVM_ThOp_CMP_LE; break;

	case BSVM_SOP_CMP_GE:
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_GE_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_GE_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_CMP_GE_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_CMP_GE_XD; break; }
		thop->fcn=BSVM_ThOp_CMP_GE; break;

	case BSVM_SOP_CMP_EQQ:
		thop->fcn=BSVM_ThOp_CMP_EQ; break;
	case BSVM_SOP_CMP_NEQ:
		thop->fcn=BSVM_ThOp_CMP_NEQ; break;
	case BSVM_SOP_CMP_U:
		thop->fcn=BSVM_ThOp_CMP_U; break;

	case BSVM_SOP_ADD_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_ADD_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_ADD_XL_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_PTR)
			{ thop->fcn=BSVM_ThOp_ADD_XP_C; break; }
		thop->fcn=BSVM_ThOp_ADD_C; break;
	case BSVM_SOP_SUB_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SUB_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SUB_XL_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_PTR)
			{ thop->fcn=BSVM_ThOp_SUB_XP_C; break; }
		thop->fcn=BSVM_ThOp_SUB_C; break;

	case BSVM_SOP_MUL_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_MUL_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_MUL_XL_C; break; }
		thop->fcn=BSVM_ThOp_MUL_C; break;
	case BSVM_SOP_DIV_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_DIV_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_DIV_XL_C; break; }
		thop->fcn=BSVM_ThOp_DIV_C; break;
	case BSVM_SOP_MOD_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_MOD_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_MOD_XL_C; break; }
		thop->fcn=BSVM_ThOp_MOD_C; break;
	case BSVM_SOP_AND_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_AND_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_AND_XL_C; break; }
		thop->fcn=BSVM_ThOp_AND_C; break;
	case BSVM_SOP_OR_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_OR_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_OR_XL_C; break; }
		thop->fcn=BSVM_ThOp_OR_C; break;
	case BSVM_SOP_XOR_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_XOR_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_XOR_XL_C; break; }
		thop->fcn=BSVM_ThOp_XOR_C; break;
	case BSVM_SOP_SHL_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SHL_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SHL_XL_C; break; }
		thop->fcn=BSVM_ThOp_SHL_C; break;
	case BSVM_SOP_SHR_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SHR_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SHR_XL_C; break; }
		thop->fcn=BSVM_ThOp_SHR_C; break;
	case BSVM_SOP_SHRR_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_SHRR_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_SHRR_XL_C; break; }
		thop->fcn=BSVM_ThOp_SHRR_C; break;

	case BSVM_SOP_CMP_EQ_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_EQ_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_EQ_XL_C; break; }
		thop->fcn=BSVM_ThOp_CMP_EQ_C; break;
	case BSVM_SOP_CMP_NE_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_NE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_NE_XL_C; break; }
		thop->fcn=BSVM_ThOp_CMP_NE_C; break;
	case BSVM_SOP_CMP_LT_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_LT_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_LT_XL_C; break; }
		thop->fcn=BSVM_ThOp_CMP_LT_C; break;
	case BSVM_SOP_CMP_GT_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_GT_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_GT_XL_C; break; }
		thop->fcn=BSVM_ThOp_CMP_GT_C; break;
	case BSVM_SOP_CMP_LE_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_LE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_LE_XL_C; break; }
		thop->fcn=BSVM_ThOp_CMP_LE_C; break;
	case BSVM_SOP_CMP_GE_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->t=dytfInt(thop->i);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_CMP_GE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_CMP_GE_XL_C; break; }
		thop->fcn=BSVM_ThOp_CMP_GE_C; break;

	case BSVM_SOP_JMP_EQ_C:
		thop->j=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->j);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_EQ_C; break;
	case BSVM_SOP_JMP_NE_C:
		thop->j=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->j);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_NE_C; break;
	case BSVM_SOP_JMP_LT_C:
		thop->j=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->j);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_LT_C; break;
	case BSVM_SOP_JMP_GT_C:
		thop->j=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->j);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_GT_C; break;
	case BSVM_SOP_JMP_LE_C:
		thop->j=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->j);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_LE_C; break;
	case BSVM_SOP_JMP_GE_C:
		thop->j=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->j);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_GE_C; break;

	case BSVM_SOP_JMP_EQ_LC:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->k);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XI_LC; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XL_LC; break; }
		thop->fcn=BSVM_ThOp_JMP_EQ_LC; break;
	case BSVM_SOP_JMP_NE_LC:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->k);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XI_LC; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XL_LC; break; }
		thop->fcn=BSVM_ThOp_JMP_NE_LC; break;
	case BSVM_SOP_JMP_LT_LC:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->k);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XI_LC; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XL_LC; break; }
		thop->fcn=BSVM_ThOp_JMP_LT_LC; break;
	case BSVM_SOP_JMP_GT_LC:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->k);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XI_LC; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XL_LC; break; }
		thop->fcn=BSVM_ThOp_JMP_GT_LC; break;
	case BSVM_SOP_JMP_LE_LC:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->k);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XI_LC; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XL_LC; break; }
		thop->fcn=BSVM_ThOp_JMP_LE_LC; break;
	case BSVM_SOP_JMP_GE_LC:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadSVLI(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->t=dytfInt(thop->k);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XI_LC; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XL_LC; break; }
		thop->fcn=BSVM_ThOp_JMP_GE_LC; break;

	case BSVM_SOP_JMP:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP;
		break;
	case BSVM_SOP_JMP_TRUE:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_TRUE;
		break;
	case BSVM_SOP_JMP_FALSE:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_FALSE;
		break;
	case BSVM_SOP_JMP_COND_U:
		thop->j=*ctx->ip++;
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_COND_U; break;

	case BSVM_SOP_JMP_COND_B:
		thop->j=*ctx->ip++;
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_COND_B; break;

	case BSVM_SOP_JMP_SWEQ:
		thop->t=BSVM_ReadID(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_SWEQ;
		break;
	case BSVM_SOP_JMP_SWEQF:
		thop->t=BSVM_ReadID(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_SWEQF;
		break;
	case BSVM_SOP_JMP_SWDFL:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_SWDFL;
		break;
	case BSVM_SOP_JMP_SWB:
		thop->j=*ctx->ip++;
		thop->t=BSVM_ReadID(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_SWB; break;

#if 1
	case BSVM_SOP_JMP_EQ:
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XD; break; }
		thop->fcn=BSVM_ThOp_JMP_E;
		break;

	case BSVM_SOP_JMP_NE:
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XD; break; }
		thop->fcn=BSVM_ThOp_JMP_NE;
		break;

	case BSVM_SOP_JMP_LT:
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XD; break; }
		thop->fcn=BSVM_ThOp_JMP_L;
		break;

	case BSVM_SOP_JMP_GT:
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XD; break; }
		thop->fcn=BSVM_ThOp_JMP_G;
		break;

	case BSVM_SOP_JMP_LE:
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XD; break; }
		thop->fcn=BSVM_ThOp_JMP_LE;
		break;

	case BSVM_SOP_JMP_GE:
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XI; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XL; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XF)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XF; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XD)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XD; break; }
		thop->fcn=BSVM_ThOp_JMP_GE;
		break;

	case BSVM_SOP_JMP_EQQ:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_EQ;
		break;
	case BSVM_SOP_JMP_NEQ:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_NEQ;
		break;
	case BSVM_SOP_JMP_U:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_U;
		break;
#endif

	case BSVM_SOP_LDJMP:
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		thop->i=((signed short)i);
		thop->fcn=BSVM_ThOp_LDJMP; break;
	case BSVM_SOP_JMPIA:
		thop->fcn=BSVM_ThOp_JMPIA; break;

	case BSVM_SOP_LIST:
		thop->fcn=BSVM_ThOp_LIST; break;
	case BSVM_SOP_DICT:
		thop->fcn=BSVM_ThOp_DICT; break;
	case BSVM_SOP_ATTR:
		thop->fcn=BSVM_ThOp_ATTR; break;
	case BSVM_SOP_VECTOR:
		thop->fcn=BSVM_ThOp_VECTOR; break;
	case BSVM_SOP_COMPLEX_I:
		thop->fcn=BSVM_ThOp_COMPLEX_I; break;
	case BSVM_SOP_MATRIX:
		thop->fcn=BSVM_ThOp_MATRIX; break;
	case BSVM_SOP_COMPLEX:
		thop->fcn=BSVM_ThOp_COMPLEX; break;
	case BSVM_SOP_QUAT:
		thop->fcn=BSVM_ThOp_QUAT; break;

	case BSVM_SOP_ARRAY:
		thop->fcn=BSVM_ThOp_ARRAY; break;

	case BSVM_SOP_ARRAY_S:
		thop->t=BSVM_ReadID(ctx);
		thop->fcn=BSVM_ThOp_ARRAY_S; break;

	case BSVM_SOP_EXCH_YXZ:
		thop->fcn=BSVM_ThOp_EXCH_YXZ; break;
	case BSVM_SOP_EXCH_YZX:
		thop->fcn=BSVM_ThOp_EXCH_YZX; break;
	case BSVM_SOP_EXCH_ZXY:
		thop->fcn=BSVM_ThOp_EXCH_ZXY; break;
	case BSVM_SOP_EXCH_ZYX:
		thop->fcn=BSVM_ThOp_EXCH_ZYX; break;

	case BSVM_SOP_EXCH_WXYZ:
		thop->fcn=BSVM_ThOp_EXCH_WXYZ; break;
	case BSVM_SOP_EXCH_WXZY:
		thop->fcn=BSVM_ThOp_EXCH_WXZY; break;
	case BSVM_SOP_EXCH_WYXZ:
		thop->fcn=BSVM_ThOp_EXCH_WYXZ; break;
	case BSVM_SOP_EXCH_WYZX:
		thop->fcn=BSVM_ThOp_EXCH_WYZX; break;
	case BSVM_SOP_EXCH_WZXY:
		thop->fcn=BSVM_ThOp_EXCH_WZXY; break;
	case BSVM_SOP_EXCH_WZYX:
		thop->fcn=BSVM_ThOp_EXCH_WZYX; break;

	case BSVM_SOP_EXCH_XWYZ:
		thop->fcn=BSVM_ThOp_EXCH_XWYZ; break;
	case BSVM_SOP_EXCH_XWZY:
		thop->fcn=BSVM_ThOp_EXCH_XWZY; break;
	case BSVM_SOP_EXCH_XYWZ:
		thop->fcn=BSVM_ThOp_EXCH_XYWZ; break;
	case BSVM_SOP_EXCH_XYZW:
		thop->fcn=BSVM_ThOp_EXCH_XYZW; break;
	case BSVM_SOP_EXCH_XZWY:
		thop->fcn=BSVM_ThOp_EXCH_XZWY; break;
	case BSVM_SOP_EXCH_XZYW:
		thop->fcn=BSVM_ThOp_EXCH_XZYW; break;

	case BSVM_SOP_EXCH_YWXZ:
		thop->fcn=BSVM_ThOp_EXCH_YWXZ; break;
	case BSVM_SOP_EXCH_YWZX:
		thop->fcn=BSVM_ThOp_EXCH_YWZX; break;
	case BSVM_SOP_EXCH_YXWZ:
		thop->fcn=BSVM_ThOp_EXCH_YXWZ; break;
	case BSVM_SOP_EXCH_YXZW:
		thop->fcn=BSVM_ThOp_EXCH_YXZW; break;
	case BSVM_SOP_EXCH_YZWX:
		thop->fcn=BSVM_ThOp_EXCH_YZWX; break;
	case BSVM_SOP_EXCH_YZXW:
		thop->fcn=BSVM_ThOp_EXCH_YZXW; break;

	case BSVM_SOP_EXCH_ZWXY:
		thop->fcn=BSVM_ThOp_EXCH_ZWXY; break;
	case BSVM_SOP_EXCH_ZWYX:
		thop->fcn=BSVM_ThOp_EXCH_ZWYX; break;
	case BSVM_SOP_EXCH_ZXWY:
		thop->fcn=BSVM_ThOp_EXCH_ZXWY; break;
	case BSVM_SOP_EXCH_ZXYW:
		thop->fcn=BSVM_ThOp_EXCH_ZXYW; break;
	case BSVM_SOP_EXCH_ZYWX:
		thop->fcn=BSVM_ThOp_EXCH_ZYWX; break;
	case BSVM_SOP_EXCH_ZYXW:
		thop->fcn=BSVM_ThOp_EXCH_ZYXW; break;

#if 1
	case BSVM_SOP_INC_FN:
		thop->fcn=BSVM_ThOp_INC_FN; break;
	case BSVM_SOP_DEC_FN:
		thop->fcn=BSVM_ThOp_DEC_FN; break;
	case BSVM_SOP_INC2_FN:
		thop->fcn=BSVM_ThOp_INC2_FN; break;
	case BSVM_SOP_DEC2_FN:
		thop->fcn=BSVM_ThOp_DEC2_FN; break;

	case BSVM_SOP_ADD_FN:
		thop->fcn=BSVM_ThOp_ADD_FN; break;
	case BSVM_SOP_SUB_FN:
		thop->fcn=BSVM_ThOp_SUB_FN; break;
	case BSVM_SOP_MUL_FN:
		thop->fcn=BSVM_ThOp_MUL_FN; break;
	case BSVM_SOP_DIV_FN:
		thop->fcn=BSVM_ThOp_DIV_FN; break;
	case BSVM_SOP_IDIV_FN:
		thop->fcn=BSVM_ThOp_IDIV_FN; break;
	case BSVM_SOP_MOD_FN:
		thop->fcn=BSVM_ThOp_MOD_FN; break;
	case BSVM_SOP_AND_FN:
		thop->fcn=BSVM_ThOp_AND_FN; break;
	case BSVM_SOP_OR_FN:
		thop->fcn=BSVM_ThOp_OR_FN; break;
	case BSVM_SOP_XOR_FN:
		thop->fcn=BSVM_ThOp_XOR_FN; break;

	case BSVM_SOP_CMP_L_FN:
		thop->fcn=BSVM_ThOp_CMP_L_FN; break;
	case BSVM_SOP_CMP_G_FN:
		thop->fcn=BSVM_ThOp_CMP_G_FN; break;
	case BSVM_SOP_CMP_LE_FN:
		thop->fcn=BSVM_ThOp_CMP_LE_FN; break;
	case BSVM_SOP_CMP_GE_FN:
		thop->fcn=BSVM_ThOp_CMP_GE_FN; break;
	case BSVM_SOP_CMP_E_FN:
		thop->fcn=BSVM_ThOp_CMP_E_FN; break;
	case BSVM_SOP_CMP_NE_FN:
		thop->fcn=BSVM_ThOp_CMP_NE_FN; break;

	case BSVM_SOP_ADD_FN_C:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_ADD_FN_C;
		break;
	case BSVM_SOP_SUB_FN_C:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_SUB_FN_C;
		break;
	case BSVM_SOP_MUL_FN_C:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_MUL_FN_C;
		break;

	case BSVM_SOP_SHL_FN:
		thop->fcn=BSVM_ThOp_SHL_FN; break;
	case BSVM_SOP_SHR_FN:
		thop->fcn=BSVM_ThOp_SHR_FN; break;
	case BSVM_SOP_EXP_FN:
		thop->fcn=BSVM_ThOp_EXP_FN; break;

	case BSVM_SOP_SHL_FN_C:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_SHL_FN_C; break;
	case BSVM_SOP_SHR_FN_C:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_SHR_FN_C; break;
	case BSVM_SOP_EXP_FN_C:
		thop->i=*ctx->ip++;
		thop->fcn=BSVM_ThOp_EXP_FN_C; break;

	case BSVM_SOP_CONV_FN2FL:
		thop->fcn=BSVM_ThOp_CONV_FN2FL; break;
	case BSVM_SOP_CONV_FL2FN:
		thop->fcn=BSVM_ThOp_CONV_FL2FN; break;

	case BSVM_SOP_ADD_FL:
		thop->fcn=BSVM_ThOp_ADD_FL; break;
	case BSVM_SOP_SUB_FL:
		thop->fcn=BSVM_ThOp_SUB_FL; break;
	case BSVM_SOP_MUL_FL:
		thop->fcn=BSVM_ThOp_MUL_FL; break;
	case BSVM_SOP_DIV_FL:
		thop->fcn=BSVM_ThOp_DIV_FL; break;
	case BSVM_SOP_CMP_L_FL:
		thop->fcn=BSVM_ThOp_CMP_L_FL; break;
	case BSVM_SOP_CMP_G_FL:
		thop->fcn=BSVM_ThOp_CMP_G_FL; break;
	case BSVM_SOP_CMP_LE_FL:
		thop->fcn=BSVM_ThOp_CMP_LE_FL; break;
	case BSVM_SOP_CMP_GE_FL:
		thop->fcn=BSVM_ThOp_CMP_GE_FL; break;
	case BSVM_SOP_CMP_E_FL:
		thop->fcn=BSVM_ThOp_CMP_E_FL; break;
	case BSVM_SOP_CMP_NE_FL:
		thop->fcn=BSVM_ThOp_CMP_NE_FL; break;
	case BSVM_SOP_NEG_FL:
		thop->fcn=BSVM_ThOp_NEG_FL; break;
#endif

#if 1
	case BSVM_SOP_JMP_E_FN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_E_FN;
		break;
	case BSVM_SOP_JMP_NE_FN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_NE_FN;
		break;
	case BSVM_SOP_JMP_E_FL:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_E_FL;
		break;
	case BSVM_SOP_JMP_NE_FL:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_NE_FL;
		break;
#endif

#if 1
	case BSVM_SOP_JMP_L_FN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_L_FN;
		break;
	case BSVM_SOP_JMP_G_FN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_G_FN;
		break;
	case BSVM_SOP_JMP_LE_FN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_LE_FN;
		break;
	case BSVM_SOP_JMP_GE_FN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_GE_FN;
		break;

	case BSVM_SOP_JMP_L_FL:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_L_FL;
		break;
	case BSVM_SOP_JMP_G_FL:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_G_FL;
		break;
	case BSVM_SOP_JMP_LE_FL:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_LE_FL;
		break;
	case BSVM_SOP_JMP_GE_FL:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_GE_FL;
		break;
#endif

#if 1
	case BSVM_SOP_JMP_E_LFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_E_LFN;
		break;
	case BSVM_SOP_JMP_NE_LFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_NE_LFN;
		break;
	case BSVM_SOP_JMP_L_LFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_L_LFN;
		break;
	case BSVM_SOP_JMP_G_LFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_G_LFN;
		break;
	case BSVM_SOP_JMP_LE_LFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_LE_LFN;
		break;
	case BSVM_SOP_JMP_GE_LFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_G_LFN;
		break;
#endif

#if 1
	case BSVM_SOP_JMP_E_ZFN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_E_ZFN;
		break;
	case BSVM_SOP_JMP_NE_ZFN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_NE_ZFN;
		break;
	case BSVM_SOP_JMP_L_ZFN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_L_ZFN;
		break;
	case BSVM_SOP_JMP_G_ZFN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_G_ZFN;
		break;
	case BSVM_SOP_JMP_LE_ZFN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_LE_ZFN;
		break;
	case BSVM_SOP_JMP_GE_ZFN:
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_GE_ZFN;
		break;
#endif

#if 1
	case BSVM_SOP_JMP_E_LZFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_E_LZFN;
		break;
	case BSVM_SOP_JMP_NE_LZFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_NE_LZFN;
		break;
	case BSVM_SOP_JMP_L_LZFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_L_LZFN;
		break;
	case BSVM_SOP_JMP_G_LZFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_G_LZFN;
		break;
	case BSVM_SOP_JMP_LE_LZFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_LE_LZFN;
		break;
	case BSVM_SOP_JMP_GE_LZFN:
		thop->j=BSVM_ReadIDX(ctx);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_GE_LZFN;
		break;

	case BSVM_SOP_JMP_E_FNC:
		thop->j=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_EQ_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_E_FNC;
		break;
	case BSVM_SOP_JMP_NE_FNC:
		thop->j=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_NE_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_NE_FNC;
		break;
	case BSVM_SOP_JMP_L_FNC:
		thop->j=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LT_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_L_FNC;
		break;
	case BSVM_SOP_JMP_G_FNC:
		thop->j=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GT_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_G_FNC;
		break;
	case BSVM_SOP_JMP_LE_FNC:
		thop->j=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_LE_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_LE_FNC;
		break;
	case BSVM_SOP_JMP_GE_FNC:
		thop->j=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		if(ctx->pfhint&BSVM_PF_HINT_XI)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XI_C; break; }
		if(ctx->pfhint&BSVM_PF_HINT_XL)
			{ thop->fcn=BSVM_ThOp_JMP_GE_XL_C; break; }
		thop->fcn=BSVM_ThOp_JMP_GE_FNC;
		break;

	case BSVM_SOP_JMP_E_LFNC:
//		thop->j=*ctx->ip++;
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_E_LFNC;
		break;
	case BSVM_SOP_JMP_NE_LFNC:
//		thop->j=*ctx->ip++;
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_NE_LFNC;
		break;
	case BSVM_SOP_JMP_L_LFNC:
//		thop->j=*ctx->ip++;
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_L_LFNC;
		break;
	case BSVM_SOP_JMP_G_LFNC:
//		thop->j=*ctx->ip++;
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_G_LFNC;
		break;
	case BSVM_SOP_JMP_LE_LFNC:
//		thop->j=*ctx->ip++;
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_LE_LFNC;
		break;
	case BSVM_SOP_JMP_GE_LFNC:
//		thop->j=*ctx->ip++;
		thop->j=BSVM_ReadIDX(ctx);
		thop->k=(s8)(*ctx->ip++);
		thop->i=BSVM_ReadRelAddr(ctx);
		thop->fcn=BSVM_ThOp_JMP_GE_LFNC;
		break;
#endif

#if 1
	case BSVM_SOP_LLOAD_XI:
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LLOAD_XI_VS; break; }
#endif
		thop->fcn=BSVM_ThOp_LLOAD_XI; break;
	case BSVM_SOP_LSTORE_XI:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LSTORE_XI; break;
	case BSVM_SOP_LLOAD_XL:
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LLOAD_XL_VS; break; }
#endif
		thop->fcn=BSVM_ThOp_LLOAD_XL; break;
	case BSVM_SOP_LSTORE_XL:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LSTORE_XL; break;
	case BSVM_SOP_LLOAD_XF:
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LLOAD_XF_VS; break; }
#endif
		thop->fcn=BSVM_ThOp_LLOAD_XF; break;
	case BSVM_SOP_LSTORE_XF:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LSTORE_XF; break;
	case BSVM_SOP_LLOAD_XD:
		thop->i=BSVM_ReadIDX(ctx);
#ifdef BSVM_VARSTACK
		if(ctx->vmflags&BSVM_VMFL_VARSTACK)
			{ thop->fcn=BSVM_ThOp_LLOAD_XD_VS; break; }
#endif
		thop->fcn=BSVM_ThOp_LLOAD_XD; break;
	case BSVM_SOP_LSTORE_XD:
		thop->i=BSVM_ReadIDX(ctx);
		thop->fcn=BSVM_ThOp_LSTORE_XD; break;

	case BSVM_SOP_DUP_XI: thop->fcn=BSVM_ThOp_DUP_XI; break;
	case BSVM_SOP_DUP_XL: thop->fcn=BSVM_ThOp_DUP_XL; break;
	case BSVM_SOP_DUP_XF: thop->fcn=BSVM_ThOp_DUP_XF; break;
	case BSVM_SOP_DUP_XD: thop->fcn=BSVM_ThOp_DUP_XD; break;

	case BSVM_SOP_ADD_XI: thop->fcn=BSVM_ThOp_ADD_XI; break;
	case BSVM_SOP_SUB_XI: thop->fcn=BSVM_ThOp_SUB_XI; break;
	case BSVM_SOP_MUL_XI: thop->fcn=BSVM_ThOp_MUL_XI; break;
	case BSVM_SOP_DIV_XI: thop->fcn=BSVM_ThOp_DIV_XI; break;
	case BSVM_SOP_ADD_XL: thop->fcn=BSVM_ThOp_ADD_XL; break;
	case BSVM_SOP_SUB_XL: thop->fcn=BSVM_ThOp_SUB_XL; break;
	case BSVM_SOP_MUL_XL: thop->fcn=BSVM_ThOp_MUL_XL; break;
	case BSVM_SOP_DIV_XL: thop->fcn=BSVM_ThOp_DIV_XL; break;
	case BSVM_SOP_ADD_XF: thop->fcn=BSVM_ThOp_ADD_XF; break;
	case BSVM_SOP_SUB_XF: thop->fcn=BSVM_ThOp_SUB_XF; break;
	case BSVM_SOP_MUL_XF: thop->fcn=BSVM_ThOp_MUL_XF; break;
	case BSVM_SOP_DIV_XF: thop->fcn=BSVM_ThOp_DIV_XF; break;
	case BSVM_SOP_ADD_XD: thop->fcn=BSVM_ThOp_ADD_XD; break;
	case BSVM_SOP_SUB_XD: thop->fcn=BSVM_ThOp_SUB_XD; break;
	case BSVM_SOP_MUL_XD: thop->fcn=BSVM_ThOp_MUL_XD; break;
	case BSVM_SOP_DIV_XD: thop->fcn=BSVM_ThOp_DIV_XD; break;

	case BSVM_SOP_MOD_XI:  thop->fcn=BSVM_ThOp_MOD_XI; break;
	case BSVM_SOP_AND_XI:  thop->fcn=BSVM_ThOp_AND_XI; break;
	case BSVM_SOP_OR_XI:   thop->fcn=BSVM_ThOp_OR_XI; break;
	case BSVM_SOP_XOR_XI:  thop->fcn=BSVM_ThOp_XOR_XI; break;
	case BSVM_SOP_SHL_XI:  thop->fcn=BSVM_ThOp_SHL_XI; break;
	case BSVM_SOP_SHR_XI:  thop->fcn=BSVM_ThOp_SHR_XI; break;
	case BSVM_SOP_SHRR_XI: thop->fcn=BSVM_ThOp_SHRR_XI; break;
	case BSVM_SOP_MOD_XL:  thop->fcn=BSVM_ThOp_MOD_XL; break;
	case BSVM_SOP_AND_XL:  thop->fcn=BSVM_ThOp_AND_XL; break;
	case BSVM_SOP_OR_XL:   thop->fcn=BSVM_ThOp_OR_XL; break;
	case BSVM_SOP_XOR_XL:  thop->fcn=BSVM_ThOp_XOR_XL; break;
	case BSVM_SOP_SHL_XL:  thop->fcn=BSVM_ThOp_SHL_XL; break;
	case BSVM_SOP_SHR_XL:  thop->fcn=BSVM_ThOp_SHR_XL; break;
	case BSVM_SOP_SHRR_XL: thop->fcn=BSVM_ThOp_SHRR_XL; break;

	case BSVM_SOP_ADD_XI_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_ADD_XI_C;
		break;
	case BSVM_SOP_SUB_XI_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_SUB_XI_C;
		break;
	case BSVM_SOP_MUL_XI_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_MUL_XI_C;
		break;
	case BSVM_SOP_DIV_XI_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_DIV_XI_C;
		break;
	case BSVM_SOP_SHL_XI_C:
		thop->i=(s8)(*ctx->ip++);
		thop->fcn=BSVM_ThOp_SHL_XI_C;
		break;
	case BSVM_SOP_SHR_XI_C:
		thop->i=(s8)(*ctx->ip++);
		thop->fcn=BSVM_ThOp_SHR_XI_C;
		break;
	case BSVM_SOP_SHRR_XI_C:
		thop->i=(s8)(*ctx->ip++);
		thop->fcn=BSVM_ThOp_SHRR_XI_C;
		break;

	case BSVM_SOP_ADD_XL_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_ADD_XL_C;
		break;
	case BSVM_SOP_SUB_XL_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_SUB_XL_C;
		break;
	case BSVM_SOP_MUL_XL_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_MUL_XL_C;
		break;
	case BSVM_SOP_DIV_XL_C:
		thop->i=BSVM_ReadSVLI(ctx);
		thop->fcn=BSVM_ThOp_DIV_XL_C;
		break;
	case BSVM_SOP_SHL_XL_C:
		thop->i=(s8)(*ctx->ip++);
		thop->fcn=BSVM_ThOp_SHL_XL_C;
		break;
	case BSVM_SOP_SHR_XL_C:
		thop->i=(s8)(*ctx->ip++);
		thop->fcn=BSVM_ThOp_SHR_XL_C;
		break;
	case BSVM_SOP_SHRR_XL_C:
		thop->i=(s8)(*ctx->ip++);
		thop->fcn=BSVM_ThOp_SHRR_XL_C;
		break;

	case BSVM_SOP_CMP_EQ_XI: thop->fcn=BSVM_ThOp_CMP_EQ_XI; break;
	case BSVM_SOP_CMP_NE_XI: thop->fcn=BSVM_ThOp_CMP_NE_XI; break;
	case BSVM_SOP_CMP_LT_XI: thop->fcn=BSVM_ThOp_CMP_LT_XI; break;
	case BSVM_SOP_CMP_GT_XI: thop->fcn=BSVM_ThOp_CMP_GT_XI; break;
	case BSVM_SOP_CMP_LE_XI: thop->fcn=BSVM_ThOp_CMP_LE_XI; break;
	case BSVM_SOP_CMP_GE_XI: thop->fcn=BSVM_ThOp_CMP_GE_XI; break;
	case BSVM_SOP_CMP_EQ_XL: thop->fcn=BSVM_ThOp_CMP_EQ_XL; break;
	case BSVM_SOP_CMP_NE_XL: thop->fcn=BSVM_ThOp_CMP_NE_XL; break;
	case BSVM_SOP_CMP_LT_XL: thop->fcn=BSVM_ThOp_CMP_LT_XL; break;
	case BSVM_SOP_CMP_GT_XL: thop->fcn=BSVM_ThOp_CMP_GT_XL; break;
	case BSVM_SOP_CMP_LE_XL: thop->fcn=BSVM_ThOp_CMP_LE_XL; break;
	case BSVM_SOP_CMP_GE_XL: thop->fcn=BSVM_ThOp_CMP_GE_XL; break;
	case BSVM_SOP_CMP_EQ_XF: thop->fcn=BSVM_ThOp_CMP_EQ_XF; break;
	case BSVM_SOP_CMP_NE_XF: thop->fcn=BSVM_ThOp_CMP_NE_XF; break;
	case BSVM_SOP_CMP_LT_XF: thop->fcn=BSVM_ThOp_CMP_LT_XF; break;
	case BSVM_SOP_CMP_GT_XF: thop->fcn=BSVM_ThOp_CMP_GT_XF; break;
	case BSVM_SOP_CMP_LE_XF: thop->fcn=BSVM_ThOp_CMP_LE_XF; break;
	case BSVM_SOP_CMP_GE_XF: thop->fcn=BSVM_ThOp_CMP_GE_XF; break;
	case BSVM_SOP_CMP_EQ_XD: thop->fcn=BSVM_ThOp_CMP_EQ_XD; break;
	case BSVM_SOP_CMP_NE_XD: thop->fcn=BSVM_ThOp_CMP_NE_XD; break;
	case BSVM_SOP_CMP_LT_XD: thop->fcn=BSVM_ThOp_CMP_LT_XD; break;
	case BSVM_SOP_CMP_GT_XD: thop->fcn=BSVM_ThOp_CMP_GT_XD; break;
	case BSVM_SOP_CMP_LE_XD: thop->fcn=BSVM_ThOp_CMP_LE_XD; break;
	case BSVM_SOP_CMP_GE_XD: thop->fcn=BSVM_ThOp_CMP_GE_XD; break;

	case BSVM_SOP_CONV_FN2XI: thop->fcn=BSVM_ThOp_CONV_FN2XI; break;
	case BSVM_SOP_CONV_FN2XL: thop->fcn=BSVM_ThOp_CONV_FN2XL; break;
	case BSVM_SOP_CONV_FL2XF: thop->fcn=BSVM_ThOp_CONV_FL2XF; break;
	case BSVM_SOP_CONV_FL2XD: thop->fcn=BSVM_ThOp_CONV_FL2XD; break;
	case BSVM_SOP_CONV_XI2XL: thop->fcn=BSVM_ThOp_CONV_XI2XL; break;
	case BSVM_SOP_CONV_XI2XF: thop->fcn=BSVM_ThOp_CONV_XI2XF; break;
	case BSVM_SOP_CONV_XI2XD: thop->fcn=BSVM_ThOp_CONV_XI2XD; break;
	case BSVM_SOP_CONV_XI2FN: thop->fcn=BSVM_ThOp_CONV_XI2FN; break;
	case BSVM_SOP_CONV_XL2XI: thop->fcn=BSVM_ThOp_CONV_XL2XI; break;
	case BSVM_SOP_CONV_XL2XF: thop->fcn=BSVM_ThOp_CONV_XL2XF; break;
	case BSVM_SOP_CONV_XL2XD: thop->fcn=BSVM_ThOp_CONV_XL2XD; break;
	case BSVM_SOP_CONV_XL2FN: thop->fcn=BSVM_ThOp_CONV_XL2FN; break;
	case BSVM_SOP_CONV_XF2XI: thop->fcn=BSVM_ThOp_CONV_XF2XI; break;
	case BSVM_SOP_CONV_XF2XL: thop->fcn=BSVM_ThOp_CONV_XF2XL; break;
	case BSVM_SOP_CONV_XF2XD: thop->fcn=BSVM_ThOp_CONV_XF2XD; break;
	case BSVM_SOP_CONV_XF2FL: thop->fcn=BSVM_ThOp_CONV_XF2FL; break;
	case BSVM_SOP_CONV_XD2XI: thop->fcn=BSVM_ThOp_CONV_XD2XI; break;
	case BSVM_SOP_CONV_XD2XL: thop->fcn=BSVM_ThOp_CONV_XD2XL; break;
	case BSVM_SOP_CONV_XD2XF: thop->fcn=BSVM_ThOp_CONV_XD2XF; break;
	case BSVM_SOP_CONV_XD2FL: thop->fcn=BSVM_ThOp_CONV_XD2FL; break;

#endif

	default:
		*(int *)-1=-1;
		BSVM_SkipOpcodeArgs(ctx, op);
		thop->fcn=BSVM_ThOp_Default;
		break;
	}

	BSVM_Thread_DecodeHandleSFIS(ctx, thop, op);
	return(thop);
}

void BSVM_Thread_DecodeHandleSFIS(BSVM_SVMState *ctx,
	BSVM_ThreadOp *thop, int op)
{
	char *s;
	
	s=bsvm_ops_args[op];
	while(*s)
	{
		if(*s=='A')
			thop->pfhint|=BSVM_PFHINT_JMPADDR;
		s++;
	}

	s=bsvm_ops_sfis[op];
	while(*s)
	{
		if(*s=='!')
			thop->pfhint|=BSVM_PFHINT_TRACEBREAK;
		if(*s=='@')
			thop->pfhint|=BSVM_PFHINT_TRACESTART;
		if(*s=='?')
			thop->pfhint|=BSVM_PFHINT_MAYTHROW;
		s++;
	}
}

BSVM_ThreadOp *BSVM_Thread_DecodeBlock(BSVM_SVMState *ctx,
	BSVM_SVMBlock *blk)
{
	BSVM_ThreadOp *cur, *prv, *fst;
	byte *oldip;

	if(blk->thops)
		return(blk->thops);

	oldip=ctx->ip;
	
	ctx->ip=blk->body;
	prv=NULL; fst=NULL;
	while(ctx->ip<ctx->ipe)
	{
		cur=BSVM_Thread_DecodeStep(ctx);
		if(prv)
			{ prv->next=cur; }
		else { fst=cur; }
		while(cur->next)
			cur=cur->next;
		prv=cur;
	}
	blk->thops=fst;

	BSVM_ThreadTrace_BuildTraces(ctx, blk);

	ctx->ip=oldip;
	return(fst);
}


void BSVM_Thread_PrintThopDebug(BSVM_SVMState *ctx, BSVM_ThreadOp *thop)
{
	byte *ips, *ipe;
	char *s0;
	dytf vk, vv;
	int i, j, k;

	dyPrintf("%s:%d\n", ctx->dbg_fn, ctx->dbg_ln);

	i=ctx->lestackpos;
	j=ctx->olestackpos;
	while(i>j)
	{
		k=ctx->lestackpos-i;
		i--;
		vk=ctx->lekstack[i];
		vv=ctx->levstack[i];
		
		s0=dytfToString(vv);
		if(strlen(s0)>64)
			strcpy(s0+60, "...");
		
		dyPrintf("%d: %s=%s\n", k,
		BSVM_GetInfoName(ctx, vk), s0);
	}

	i=ctx->ostackpos;
	j=ctx->stackpos;
	if(i<j)
	{
		dyPrintf("%d: ", i);
		while(i<j)
		{
			vv=ctx->stack[i];

			s0=dytfToString(vv);
			if(strlen(s0)>16)
				strcpy(s0+12, "...");

			dyPrintf("%s ", s0);
			i++;
		}
		dyPrintf("\n");
	}

	if(thop->next)
	{
		ips=thop->ip;
		ipe=thop->next->ip;
		ctx->ip=ips;
		
		BSVMC_DumpOpcodeList(ips, ipe, dytfPtrvF(ctx->lit));
#if 0
		while(ctx->ip<ipe)
		{
//			i=BSVM_DumpOpcode(ctx);
			if(i<=0)break;
			ctx->ip+=i;
		}
#endif
	}else
	{
		ips=thop->ip;
		ctx->ip=ips;
		BSVM_DumpOpcode(ctx);
	}
	dyPrintf("\n");
}

int BSVM_Thread_Step(BSVM_SVMState *ctx)
{
	BSVM_ThreadTrace *tr;
	BSVM_ThreadOp *cur;

	cur=ctx->thop;
	if(!cur)
	{
		cur=BSVM_Thread_GetTarget(ctx, ctx->ip);
		if(!cur)
		{
			ctx->rs=BSVM_Step(ctx, 0);
			return(ctx->rs);
		}
	}

#if 1
	if(ctx->vmgflags&BSVM_VMGFL_DEBUGSTEP)
	{
		while(cur && !ctx->rs)
		{
			BSVM_Thread_PrintThopDebug(ctx, cur);
			cur=cur->fcn(ctx, cur);
		}
		ctx->thop=cur;
		if(cur)ctx->ip=cur->ip;
		return(ctx->rs);
	}
#endif

#if 0
//	cur=cur->fcn(ctx, cur);
	while(cur && !ctx->rs)
	{
		cur=cur->fcn(ctx, cur);
//		if(cur)ctx->ip=cur->ip;
	}
	ctx->thop=cur;
	if(cur)ctx->ip=cur->ip;
	return(ctx->rs);
#endif

#if 1
//	cur=cur->fcn(ctx, cur);
	tr=cur->trace;
	while(tr && !ctx->rs)
		{ tr=tr->run(ctx, tr); }
	ctx->thtr=tr;
	if(tr)
	{
		cur=tr->thop;
		ctx->thop=cur;
		if(cur)ctx->ip=cur->ip;
	}
	else
	{
		ctx->thop=NULL;
	}
	return(ctx->rs);
#endif

#if 0
	cur=ctx->thop;
	if(cur)
	{
//		cur=cur->fcn(ctx, cur);
		while(cur && !ctx->rs)
			{ cur=cur->fcn(ctx, cur); }
		ctx->thop=cur;
		if(cur)ctx->ip=cur->ip;
		return(ctx->rs);
	}

	cur=BSVM_Thread_GetTarget(ctx, ctx->ip);
	if(cur)
	{
//		cur=cur->fcn(ctx, cur);
		while(cur && !ctx->rs)
			{ cur=cur->fcn(ctx, cur); }
		ctx->thop=cur;
		if(cur)ctx->ip=cur->ip;
		return(ctx->rs);
	}

	ctx->rs=BSVM_Step(ctx, 0);
	return(ctx->rs);
#endif
}
