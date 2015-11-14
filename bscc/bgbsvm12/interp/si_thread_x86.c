#include <bgbsvm.h>

int BSVM_ThreadX86_Init()
{
	bsvmLoadModuleASM("interp/asm_thread_x86.asm");
	return(0);
}

int BSVM_Thread_LoadEmitPrefixASM()
{
#ifdef X86
	BSVM_ThreadX86_LoadEmitPrefixASM();
#endif
	return(0);
}

#if 0
//#ifndef offsetof
#define offsetof(st, m) \
     ((size_t) ( (char *)&((st *)0)->m - (char *)0 ))
#endif

#define BSVM_THOP_OFFS(k)	offsetof(BSVM_ThreadOp, k)
#define BSVM_CTX_OFFS(k)	offsetof(BSVM_SVMState, k)
#define BSVM_THTR_OFFS(k)	offsetof(BSVM_ThreadTrace, k)

#define BSVM_ThreadPrintASM_CTX_OFFS(k)	\
	dyllAsmPrint("#define bs_ctx_offs_%s %d\n", \
		#k, BSVM_CTX_OFFS(k))

#define BSVM_ThreadPrintASM_THTR_OFFS(k)	\
	dyllAsmPrint("#define bs_thtr_offs_%s %d\n", \
		#k, BSVM_THTR_OFFS(k))

int BSVM_ThreadX86_LoadEmitPrefixASM()
{
	dyllAsmPrint("#define bs_thop_offs_next %d\n", BSVM_THOP_OFFS(next));
	dyllAsmPrint("#define bs_thop_offs_ip %d\n", BSVM_THOP_OFFS(ip));
	dyllAsmPrint("#define bs_thop_offs_fcn %d\n", BSVM_THOP_OFFS(fcn));
	dyllAsmPrint("#define bs_thop_offs_data %d\n", BSVM_THOP_OFFS(data));
	dyllAsmPrint("#define bs_thop_offs_t %d\n", BSVM_THOP_OFFS(t));
	dyllAsmPrint("#define bs_thop_offs_u %d\n", BSVM_THOP_OFFS(u));
	dyllAsmPrint("#define bs_thop_offs_v %d\n", BSVM_THOP_OFFS(v));
	dyllAsmPrint("#define bs_thop_offs_i %d\n", BSVM_THOP_OFFS(i));
	dyllAsmPrint("#define bs_thop_offs_j %d\n", BSVM_THOP_OFFS(j));
	dyllAsmPrint("#define bs_thop_offs_k %d\n", BSVM_THOP_OFFS(k));

	BSVM_ThreadPrintASM_THTR_OFFS(next);
	BSVM_ThreadPrintASM_THTR_OFFS(jmpnext);
	BSVM_ThreadPrintASM_THTR_OFFS(thop);
	BSVM_ThreadPrintASM_THTR_OFFS(n_thop);
	BSVM_ThreadPrintASM_THTR_OFFS(run);
	BSVM_ThreadPrintASM_THTR_OFFS(end);

	BSVM_ThreadPrintASM_CTX_OFFS(stack);
	BSVM_ThreadPrintASM_CTX_OFFS(stackpos);
	BSVM_ThreadPrintASM_CTX_OFFS(stacklim);
	BSVM_ThreadPrintASM_CTX_OFFS(markstack);
	BSVM_ThreadPrintASM_CTX_OFFS(markstackpos);
	BSVM_ThreadPrintASM_CTX_OFFS(markstacklim);

#ifdef BSVM_VARSTACK
	BSVM_ThreadPrintASM_CTX_OFFS(lekstack);
	BSVM_ThreadPrintASM_CTX_OFFS(levstack);
	BSVM_ThreadPrintASM_CTX_OFFS(lestackpos);
	BSVM_ThreadPrintASM_CTX_OFFS(lestacklim);
#endif

	BSVM_ThreadPrintASM_CTX_OFFS(flowstack);
	BSVM_ThreadPrintASM_CTX_OFFS(flowstackpos);
	BSVM_ThreadPrintASM_CTX_OFFS(flowstacklim);

	BSVM_ThreadPrintASM_CTX_OFFS(ip);
	BSVM_ThreadPrintASM_CTX_OFFS(ipe);

	BSVM_ThreadPrintASM_CTX_OFFS(self);
	BSVM_ThreadPrintASM_CTX_OFFS(top);
	BSVM_ThreadPrintASM_CTX_OFFS(lit);
	BSVM_ThreadPrintASM_CTX_OFFS(func);
	BSVM_ThreadPrintASM_CTX_OFFS(pkgname);

	BSVM_ThreadPrintASM_CTX_OFFS(thop);
	BSVM_ThreadPrintASM_CTX_OFFS(rs);

	BSVM_ThreadPrintASM_CTX_OFFS(fbvt_int);
	BSVM_ThreadPrintASM_CTX_OFFS(fbvt_long);
	BSVM_ThreadPrintASM_CTX_OFFS(fbvt_float);
	BSVM_ThreadPrintASM_CTX_OFFS(fbvt_double);

	BSVM_ThreadPrintASM_CTX_OFFS(this_obj);
	BSVM_ThreadPrintASM_CTX_OFFS(this_inf);

	return(0);
}

BSVM_ThreadOp *BSVM_ThreadX86_DecodeStep(BSVM_SVMState *ctx)
{
	BSVM_ThreadOp *thop;
	byte *ip1;
	dyt t, u, v, w, x, y, z;
	s64 li, lj;
	int i, j, k, op;

	return(NULL);

	thop=gctalloc("bsvm_threadop_t", sizeof(BSVM_ThreadOp));
	thop->ip=ctx->ip;

	BSVM_Thread_DecodePrefix(ctx);

//	op=*ctx->ip++;
//	if(op>=192)op=(op-192)*256+(*ctx->ip++);
	op=BSVM_Thread_DecodeOpcodeValue(ctx);

//	switch(*ctx->ip++)
	switch(op)
	{
	default:
		gcfree(thop);
		thop=NULL;
		break;
	}

	return(thop);
}

bool BSVM_ThreadTraceX86_TryJitOpcode(
	BSVM_ThreadJitContext *ctx, BSVM_ThreadOp *op)
{
	bool ret;
	
	ctx->thop=op;
	
	switch(op->opnum)
	{
	case BSVM_SOP_ADD_XI:		ret=BSVM_ThJitX86_ADD_XI(ctx);		break;
	case BSVM_SOP_SUB_XI:		ret=BSVM_ThJitX86_SUB_XI(ctx);		break;
	case BSVM_SOP_MUL_XI:		ret=BSVM_ThJitX86_MUL_XI(ctx);		break;

	case BSVM_SOP_AND_XI:		ret=BSVM_ThJitX86_AND_XI(ctx);		break;
	case BSVM_SOP_OR_XI:		ret=BSVM_ThJitX86_OR_XI(ctx);		break;
	case BSVM_SOP_XOR_XI:		ret=BSVM_ThJitX86_XOR_XI(ctx);		break;

	case BSVM_SOP_SHL_XI:		ret=BSVM_ThJitX86_SHL_XI(ctx);		break;
	case BSVM_SOP_SHR_XI:		ret=BSVM_ThJitX86_SHR_XI(ctx);		break;
	case BSVM_SOP_SHRR_XI:		ret=BSVM_ThJitX86_SHRR_XI(ctx);		break;

	case BSVM_SOP_ADD_XI_C:		ret=BSVM_ThJitX86_ADD_XI_C(ctx); break;
	case BSVM_SOP_SUB_XI_C:		ret=BSVM_ThJitX86_SUB_XI_C(ctx); break;
	case BSVM_SOP_MUL_XI_C:		ret=BSVM_ThJitX86_MUL_XI_C(ctx); break;

//	case BSVM_SOP_AND_XI_C:		ret=BSVM_ThJitX86_AND_XI_C(ctx); break;
//	case BSVM_SOP_OR_XI_C:		ret=BSVM_ThJitX86_OR_XI_C(ctx); break;
//	case BSVM_SOP_XOR_XI_C:		ret=BSVM_ThJitX86_XOR_XI_C(ctx); break;

	case BSVM_SOP_SHL_XI_C:		ret=BSVM_ThJitX86_SHL_XI_C(ctx); break;
	case BSVM_SOP_SHR_XI_C:		ret=BSVM_ThJitX86_SHR_XI_C(ctx); break;
	case BSVM_SOP_SHRR_XI_C:	ret=BSVM_ThJitX86_SHRR_XI_C(ctx); break;

#if 1
	case BSVM_SOP_LLOAD:
		if(ctx->vmctx->vmflags&BSVM_VMFL_VARSTACK)
		{
			if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
				{ ret=BSVM_ThJitX86_LLOAD_XI_VS(ctx); break; }
			ret=BSVM_ThJitX86_LLOAD_VS(ctx);
			break;
		}
		ret=false;
		break;
	case BSVM_SOP_LLOAD_XI:
		if(ctx->vmctx->vmflags&BSVM_VMFL_VARSTACK)
			{ ret=BSVM_ThJitX86_LLOAD_XI_VS(ctx); break; }
		ret=false;
		break;
#endif

#if 1
	case BSVM_SOP_LSTORE:
		if(ctx->vmctx->vmflags&BSVM_VMFL_VARSTACK)
		{
			if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
				{ ret=BSVM_ThJitX86_LSTORE_XI_VS(ctx); break; }
			ret=BSVM_ThJitX86_LSTORE_VS(ctx);
			break;
		}
		ret=false;
		break;
	case BSVM_SOP_LSTORE_XI:
		if(ctx->vmctx->vmflags&BSVM_VMFL_VARSTACK)
			{ ret=BSVM_ThJitX86_LSTORE_XI_VS(ctx); break; }
		ret=false;
		break;
#endif

#if 1
	case BSVM_SOP_LINC:
		if(ctx->vmctx->vmflags&BSVM_VMFL_VARSTACK)
		{
			if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
				{ ret=BSVM_ThJitX86_LINC_XI_VS(ctx); break; }
		}
		ret=false;
		break;

	case BSVM_SOP_LDEC:
		if(ctx->vmctx->vmflags&BSVM_VMFL_VARSTACK)
		{
			if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
				{ ret=BSVM_ThJitX86_LDEC_XI_VS(ctx); break; }
		}
		ret=false;
		break;
#endif

	case BSVM_SOP_LOADINDEX:
//		if(1) { *(int *)-1=-1; }
		ret=BSVM_ThJitX86_LoadIndex(ctx);
		break;

	case BSVM_SOP_STOREINDEX:
//		if(1) { *(int *)-1=-1; }
		ret=BSVM_ThJitX86_StoreIndex(ctx);
		break;

	case BSVM_SOP_DBGMARK:	ret=BSVM_ThJitX86_DBGMARK(ctx);	break;
	case BSVM_SOP_FN:		ret=BSVM_ThJitX86_FN(ctx);		break;
	case BSVM_SOP_LN:		ret=BSVM_ThJitX86_LN(ctx);		break;

#if 0
	case BSVM_SOP_JMP_GE:
		if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
			{ ret=BSVM_ThJitX86_JMP_GE_XI(ctx); break; }
//		if(ctx->pfhint&BSVM_PF_HINT_XL)
//			{ thop->fcn=BSVM_ThOp_JMP_GE_XL; break; }
//		if(ctx->pfhint&BSVM_PF_HINT_XF)
//			{ thop->fcn=BSVM_ThOp_JMP_GE_XF; break; }
//		if(ctx->pfhint&BSVM_PF_HINT_XD)
//			{ thop->fcn=BSVM_ThOp_JMP_GE_XD; break; }
//		thop->fcn=BSVM_ThOp_JMP_GE;
		ret=false;
		break;
#endif

	default:
		ret=false;
		break;
	}
	return(ret);
}

bool BSVM_ThreadTraceX86_TryJitTailOpcode(
	BSVM_ThreadJitContext *ctx, BSVM_ThreadOp *op)
{
	bool ret;
	
	ctx->thop=op;
	
	switch(op->opnum)
	{
	case BSVM_SOP_RET:
		ret=false;		//Not Handled Here
		break;

	case BSVM_SOP_JMP:
		ret=BSVM_ThJitX86_JMP(ctx);
		break;

#if 1
	case BSVM_SOP_JMP_EQ:
		if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
			{ ret=BSVM_ThJitX86_JMP_EQ_XI(ctx); break; }
		ret=false;
		break;
	case BSVM_SOP_JMP_NE:
		if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
			{ ret=BSVM_ThJitX86_JMP_NE_XI(ctx); break; }
		ret=false;
		break;
	case BSVM_SOP_JMP_LT:
		if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
			{ ret=BSVM_ThJitX86_JMP_LT_XI(ctx); break; }
		ret=false;
		break;
	case BSVM_SOP_JMP_GT:
		if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
			{ ret=BSVM_ThJitX86_JMP_GT_XI(ctx); break; }
		ret=false;
		break;
	case BSVM_SOP_JMP_LE:
		if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
			{ ret=BSVM_ThJitX86_JMP_LE_XI(ctx); break; }
		ret=false;
		break;
	case BSVM_SOP_JMP_GE:
		if(ctx->thop->pfhint&BSVM_PF_HINT_XI)
			{ ret=BSVM_ThJitX86_JMP_GE_XI(ctx); break; }
//		if(ctx->pfhint&BSVM_PF_HINT_XL)
//			{ thop->fcn=BSVM_ThOp_JMP_GE_XL; break; }
//		if(ctx->pfhint&BSVM_PF_HINT_XF)
//			{ thop->fcn=BSVM_ThOp_JMP_GE_XF; break; }
//		if(ctx->pfhint&BSVM_PF_HINT_XD)
//			{ thop->fcn=BSVM_ThOp_JMP_GE_XD; break; }
//		thop->fcn=BSVM_ThOp_JMP_GE;
		ret=false;
		break;
#endif

	default:
		if(BSVM_ThreadTraceX86_TryJitOpcode(ctx, op))
		{
			dyllAsmPrint("mov eax, 0x%08X\n", op->next);
			ret=true;
			break;
		}
		ret=false;
		break;
	}
	return(ret);
}

bool BSVM_ThreadTraceX86_CompileTrace_TryEndDefault(
	BSVM_ThreadJitContext *ctx)
{
//	ctx->flags|=BSVM_THJIT_CTXFL_THOPEAX;
	if(!BSVM_ThreadTraceX86_TryJitTailOpcode(ctx, ctx->thop))
		return(false);

	dyllAsmPrint("mov eax, [eax+%d]\n", BSVM_THOP_OFFS(trace));

//	dyllAsmPrint("and eax, eax\n");
//	dyllAsmPrint("jz %s\n", s0);

	return(true);
}

void BSVM_ThreadTraceX86_CompileTrace_Default(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThreadOp *op;
	bool last_thop;
	int i;

	dyllAsmPrint("push ebp\nmov ebp, esp\n");
//	dyllAsmPrint("push esi\n");
//	dyllAsmPrint("push edi\n");

	op=ctx->trace->thop; i=ctx->trace->n_thop-1;

	dyllAsmPrint("sub esp, 24\n", op);

	dyllAsmPrint("mov [ebp-4], esi\n");
	dyllAsmPrint("mov [ebp-8], edi\n");

//	dyllAsmPrint("mov eax, [ebp+8]\n", op);
//	dyllAsmPrint("mov [esp+0], eax\n");
//	dyllAsmPrint("mov eax, 0x%08X\n", op);

	last_thop=false;

	while(i--)
	{
		if(!BSVM_ThreadTraceX86_TryJitOpcode(ctx, op))
		{
			if(last_thop)
			{
				dyllAsmPrint("mov [esp+4], eax\n");
//				dyllAsmPrint("call 0x%08X\n", op->fcn);
//				dyllAsmPrint("mov ecx, 0x%08X\n", op->fcn);
//				dyllAsmPrint("call ecx\n", op->fcn);
				dyllAsmPrint("call @0x%08X\n", op->fcn);
			}else
			{
				dyllAsmPrint("mov eax, [ebp+8]\n", op);
				dyllAsmPrint("mov [esp+0], eax\n");
				dyllAsmPrint("mov dword [esp+4], 0x%08X\n", op);
				dyllAsmPrint("call @0x%08X\n", op->fcn);
			}
			last_thop=true;
			op=op->next;
		}else
		{
			last_thop=false;
			op=op->next;
//			dyllAsmPrint("mov eax, 0x%08X\n", op);
		}

//		op=op->next;
//		op=op->fcn(ctx, op);
	}

	ctx->thop=op;
	if(last_thop)
		{ ctx->flags|=BSVM_THJIT_CTXFL_LASTTHOP; }
	else
		{ ctx->flags&=~BSVM_THJIT_CTXFL_LASTTHOP; }

	if(!BSVM_ThreadTraceX86_CompileTrace_TryEndDefault(ctx))
	{
		if(last_thop)
		{
			dyllAsmPrint("mov [esp+8], eax\n");
		}else
		{
			dyllAsmPrint("mov eax, [ebp+8]\n");
			dyllAsmPrint("mov [esp+0], eax\n");
			dyllAsmPrint("mov dword [esp+8], 0x%08X\n", op);
		}

		dyllAsmPrint("mov eax, [ebp+12]\n");
		dyllAsmPrint("mov [esp+4], eax\n");
//		dyllAsmPrint("mov ecx, [eax+%d]\n", BSVM_THTR_OFFS(end));
//		dyllAsmPrint("call ecx\n");
		dyllAsmPrint("call @0x%08X\n", ctx->trace->end);
	}

//	return(tr->end(ctx, tr, op));

	dyllAsmPrint("mov esi, [ebp-4]\n");
	dyllAsmPrint("mov edi, [ebp-8]\n");

//	dyllAsmPrint("lea esp, [ebp-8]");
//	dyllAsmPrint("pop edi\n");
//	dyllAsmPrint("pop esi\n");
//	dyllAsmPrint("pop ebp\nret\n");

	dyllAsmPrint("mov esp, ebp\npop ebp\nret\n");
}

void BSVM_ThreadTraceX86_CompileTrace_Basic(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThreadOp *op;
	bool last_thop;
	int i;

	dyllAsmPrint("push ebp\nmov ebp, esp\n");

//	dyllAsmPrint("push esi\n");
//	dyllAsmPrint("push edi\n");

	op=ctx->trace->thop; i=ctx->trace->n_thop;

	dyllAsmPrint("sub esp, 24\n", op);
	
	dyllAsmPrint("mov [ebp-4], esi\n");
	dyllAsmPrint("mov [ebp-8], edi\n");
	
//	dyllAsmPrint("mov eax, [ebp+8]\n", op);
//	dyllAsmPrint("mov [esp+0], eax\n");
//	dyllAsmPrint("mov eax, 0x%08X\n", op);

	last_thop=false;

	while(i--)
	{

		if(!BSVM_ThreadTraceX86_TryJitOpcode(ctx, op))
		{
			if(last_thop)
			{
				dyllAsmPrint("mov [esp+4], eax\n");
//				dyllAsmPrint("call 0x%08X\n", op->fcn);
//				dyllAsmPrint("mov ecx, 0x%08X\n", op->fcn);
//				dyllAsmPrint("call ecx\n", op->fcn);
				dyllAsmPrint("call @0x%08X\n", op->fcn);
			}else
			{
				dyllAsmPrint("mov eax, [ebp+8]\n", op);
				dyllAsmPrint("mov [esp+0], eax\n");
				dyllAsmPrint("mov dword [esp+4], 0x%08X\n", op);
				dyllAsmPrint("call @0x%08X\n", op->fcn);
			}
			last_thop=true;
			op=op->next;
		}else
		{
			last_thop=false;
			op=op->next;
//			dyllAsmPrint("mov eax, 0x%08X\n", op);
		}

//		op=op->next;
//		op=op->fcn(ctx, op);
	}
//	return(tr->end(ctx, tr, op));

//	dyllAsmPrint("lea esp, [ebp-8]");
//	dyllAsmPrint("pop edi\n");
//	dyllAsmPrint("pop esi\n");
//	dyllAsmPrint("pop ebp\nret\n");

	dyllAsmPrint("mov eax, 0x%08X\n", ctx->trace->next);

	dyllAsmPrint("mov esp, ebp\npop ebp\nret\n");
}

BSVM_ThreadJitContext *BSVM_ThreadTraceX86_AllocContext(
	BSVM_SVMState *ctx, BSVM_ThreadTrace *tr)
{
	BSVM_ThreadJitContext *tmp;
	
	tmp=gctalloc("BSVM_ThreadJitContext_t", sizeof(BSVM_ThreadJitContext));
	tmp->vmctx=ctx;
	tmp->trace=tr;
	return(tmp);
}

void BSVM_ThreadTraceX86_FreeContext(BSVM_ThreadJitContext *ctx)
{
	gcfree(ctx);
}

void BSVM_ThreadTraceX86_CompileTrace(BSVM_SVMState *ctx,
	BSVM_ThreadTrace *tr)
{
	BSVM_ThreadJitContext *jctx;
	BSVM_ThreadOp *thop;
	void *pv;
	int i;

	jctx=BSVM_ThreadTraceX86_AllocContext(ctx, tr);

	thop=tr->thop; i=tr->n_thop-1;
	while(i--) { thop=thop->next; }

	if(thop->pfhint&(BSVM_PFHINT_TRACEBREAK|BSVM_PFHINT_JMPADDR))
	{
//		dyllAsmBeginThunkInline();
		dyllAsmBegin();

		dyllAsmPrint("[section .text]\n");
		dyllAsmPrint("[bits 32]\n");

		BSVM_ThreadTraceX86_CompileTrace_Default(jctx);

//		pv=dyllAsmEndThunkInline();
//		pv=dyllAsmEnd();
		pv=dyllAsmEndDebug();
		
		if(pv)tr->run=pv;

		BSVM_ThreadTraceX86_FreeContext(jctx);
		return;
	}

	if(thop->next && (thop->next->pfhint&BSVM_PFHINT_TRACESTART))
	{
//		dyllAsmBeginThunkInline();
		dyllAsmBegin();

		dyllAsmPrint("[section .text]\n");
		dyllAsmPrint("[bits 32]\n");

		BSVM_ThreadTraceX86_CompileTrace_Basic(jctx);

//		pv=dyllAsmEndThunkInline();
//		pv=dyllAsmEnd();
		pv=dyllAsmEndDebug();

		if(pv)tr->run=pv;

		BSVM_ThreadTraceX86_FreeContext(jctx);
		return;
	}

//	if(thop->pfhint&(BSVM_PFHINT_TRACEBREAK|BSVM_PFHINT_JMPADDR))
	if(1)
	{
//		dyllAsmBeginThunkInline();
		dyllAsmBegin();

		dyllAsmPrint("[section .text]\n");
		dyllAsmPrint("[bits 32]\n");

		BSVM_ThreadTraceX86_CompileTrace_Default(jctx);

//		pv=dyllAsmEndThunkInline();
//		pv=dyllAsmEnd();
		pv=dyllAsmEndDebug();
		
		if(pv)tr->run=pv;

		BSVM_ThreadTraceX86_FreeContext(jctx);
		return;
	}
}
