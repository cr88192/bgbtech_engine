#include <bgbsvm.h>

BSVM_ThreadTrace *BSVM_ThTr_RunDefault(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	int i;
	op=tr->thop; i=tr->n_thop-1;
	while(i--)
		{ op=op->fcn(ctx, op); }
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	int i;
	op=tr->thop; i=tr->n_thop;
	while(i--)
		{ op=op->fcn(ctx, op); }
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_EndDefault(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr, BSVM_ThreadOp *op)
{
	BSVM_ThreadOp *op1;
	op1=op->fcn(ctx, op);
	if(!op1)return(NULL);
	return(op1->trace);
//	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault1(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, tr->thop));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault2(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault3(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault4(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault5(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault6(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault7(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault8(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault9(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault10(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault11(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault12(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault13(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault14(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault15(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunDefault16(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->end(ctx, tr, op));
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic1(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic2(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic3(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic4(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic5(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic6(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic7(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic8(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic9(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic10(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic11(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic12(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic13(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic14(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic15(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

BSVM_ThreadTrace *BSVM_ThTr_RunBasic16(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadOp *op;
	op=tr->thop;
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	op=op->fcn(ctx, op);	op=op->fcn(ctx, op);
	BSVM_ThreadTrace_UpdateCheckJit(ctx, tr);
	return(tr->next);
}

void BSVM_ThreadTrace_UpdateCheckJit(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	if(tr->cnt_jit>0)
	{
		tr->cnt_jit--;
		if(tr->cnt_jit<=0)
		{
#ifdef X86
			BSVM_ThreadTraceX86_CompileTrace(ctx, tr);
#endif
		}
	}
}

void BSVM_ThreadTrace_FinishTrace(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr, BSVM_ThreadOp *thop)
{
	tr->end=BSVM_ThTr_EndDefault;
	tr->cnt_jit=16;

	switch(tr->n_thop)
	{
	case 1:		tr->run=BSVM_ThTr_RunDefault1; break;
	case 2:		tr->run=BSVM_ThTr_RunDefault2; break;
	case 3:		tr->run=BSVM_ThTr_RunDefault3; break;
	case 4:		tr->run=BSVM_ThTr_RunDefault4; break;
	case 5:		tr->run=BSVM_ThTr_RunDefault5; break;
	case 6:		tr->run=BSVM_ThTr_RunDefault6; break;
	case 7:		tr->run=BSVM_ThTr_RunDefault7; break;
	case 8:		tr->run=BSVM_ThTr_RunDefault8; break;
	case 9:		tr->run=BSVM_ThTr_RunDefault9; break;
	case 10:	tr->run=BSVM_ThTr_RunDefault10; break;
	case 11:	tr->run=BSVM_ThTr_RunDefault11; break;
	case 12:	tr->run=BSVM_ThTr_RunDefault12; break;
	case 13:	tr->run=BSVM_ThTr_RunDefault13; break;
	case 14:	tr->run=BSVM_ThTr_RunDefault14; break;
	case 15:	tr->run=BSVM_ThTr_RunDefault15; break;
	case 16:	tr->run=BSVM_ThTr_RunDefault16; break;
	default:	tr->run=BSVM_ThTr_RunDefault; break;
	}

	if(thop->pfhint&(BSVM_PFHINT_TRACEBREAK|BSVM_PFHINT_JMPADDR))
	{
#ifdef X86
//		BSVM_ThreadTraceX86_CompileTrace(ctx, tr);
#endif
		return;
	}

	if(thop->next && (thop->next->pfhint&BSVM_PFHINT_TRACESTART))
	{
		switch(tr->n_thop)
		{
		case 1:		tr->run=BSVM_ThTr_RunBasic1; break;
		case 2:		tr->run=BSVM_ThTr_RunBasic2; break;
		case 3:		tr->run=BSVM_ThTr_RunBasic3; break;
		case 4:		tr->run=BSVM_ThTr_RunBasic4; break;
		case 5:		tr->run=BSVM_ThTr_RunBasic5; break;
		case 6:		tr->run=BSVM_ThTr_RunBasic6; break;
		case 7:		tr->run=BSVM_ThTr_RunBasic7; break;
		case 8:		tr->run=BSVM_ThTr_RunBasic8; break;
		case 9:		tr->run=BSVM_ThTr_RunBasic9; break;
		case 10:	tr->run=BSVM_ThTr_RunBasic10; break;
		case 11:	tr->run=BSVM_ThTr_RunBasic11; break;
		case 12:	tr->run=BSVM_ThTr_RunBasic12; break;
		case 13:	tr->run=BSVM_ThTr_RunBasic13; break;
		case 14:	tr->run=BSVM_ThTr_RunBasic14; break;
		case 15:	tr->run=BSVM_ThTr_RunBasic15; break;
		case 16:	tr->run=BSVM_ThTr_RunBasic16; break;
		default:	tr->run=BSVM_ThTr_RunBasic; break;
		}
	}

#ifdef X86
//	BSVM_ThreadTraceX86_CompileTrace(ctx, tr);
#endif
}

BSVM_ThreadTrace *BSVM_ThreadTrace_BuildTraces(BSVM_SVMState *ctx,
	BSVM_SVMBlock *blk)
{
	BSVM_ThreadTrace *tr, *trfst, *trlst;
	BSVM_ThreadOp *cur, *tgt;
	int i;

	//first pass: mark trace starts
	cur=blk->thops;
	while(cur)
	{
		if(cur->pfhint&BSVM_PFHINT_JMPADDR)
		{
			tgt=BSVM_Thread_GetOpJumpTarget(ctx, cur, cur->i);
			if(tgt)
				{ tgt->pfhint|=BSVM_PFHINT_TRACESTART; }
		}
		cur=cur->next;
	}
	
	//second pass: break into traces
	cur=blk->thops; tr=NULL; trfst=NULL; trlst=NULL;
	while(cur)
	{
		if(!tr)
		{
			tr=gctalloc("bsvm_threadtrace_t", sizeof(BSVM_ThreadTrace));
			tr->thop=cur;
			if(trlst)
				{ trlst->next=tr; trlst=tr; }
			else
				{ trfst=tr; trlst=tr; }
		}
		
		cur->trace=tr;
		tr->n_thop++;
		
		if(cur->pfhint&(BSVM_PFHINT_TRACEBREAK|BSVM_PFHINT_JMPADDR))
		{
//			BSVM_ThreadTrace_FinishTrace(ctx, tr, cur);
			tr=NULL; cur=cur->next; continue;
		}

		if(!cur->next || (cur->next->pfhint&BSVM_PFHINT_TRACESTART))
		{
//			BSVM_ThreadTrace_FinishTrace(ctx, tr, cur);
			tr=NULL; cur=cur->next; continue;
		}

		cur=cur->next;
	}
	
#if 1
	tr=trfst;
	while(tr)
	{
		cur=tr->thop; i=tr->n_thop-1;
		while(i--) { cur=cur->next; }
		BSVM_ThreadTrace_FinishTrace(ctx, tr, cur);

		tr=tr->next;
	}
#endif

	blk->thtrace=trfst;
	return(trfst);
}

