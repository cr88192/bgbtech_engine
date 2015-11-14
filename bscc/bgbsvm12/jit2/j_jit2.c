/**
  * Toplevel contexts hold general shared state (shared between blocks).
  *
  * Contexts exist on a per-function basis.
  *
  * General Process:
  * 	Allocate Context
  * 	Create Variables
  *		while(!ContextOutputStable())
  *		{
  *			BeginFrame
  *			Emit Operations
  *			EndFrame
  *		}
  *		GetOutputAsm
  *		GetOutputLog
  *		FreeContext
  *
  *	ABI
  *		Arguments will be passed directly on stack.
  *		VM context will be passed in EBX.
  */

#include <bgbsvm.h>

BGBSVM_API bsjTopContext BSVM_Jit2_AllocTopContext()
{
	bsjTopContext tmp;
	tmp=gctalloc("bsvm_jit2_topcontext_t", sizeof(BSVM_Jit2_TopContext));
	return(tmp);
}

BGBSVM_API bsjContext BSVM_Jit2_AllocContext(bsjTopContext top)
{
	bsjContext tmp;
	tmp=gctalloc("bsvm_jit2_context_t", sizeof(BSVM_Jit2_Context));
	tmp->top=top;
	return(tmp);
}

/** Teardown JIT state, returning context to initial conditions. */
BGBSVM_API void BSVM_Jit2_TeardownContext(bsjContext ctx)
{
	int i;

	BSVM_J2Stack_FreeStack(ctx);
	BSVM_J2Vars_FreeVars(ctx);
	BSVM_J2Reg_FreeContextRegs(ctx);

	ctx->n_passes=0;
	ctx->mask_usedregs=0;
	ctx->mask_usedxregs=0;
	ctx->stackpos=0;
	ctx->markstackpos=0;

	if(ctx->asm_cts)
	{
		gcfree(ctx->asm_cts);
		ctx->asm_cts=NULL;
		ctx->asm_cte=NULL;
		ctx->asm_ct=NULL;
	}

	if(ctx->log_cts)
	{
		gcfree(ctx->log_cts);
		ctx->log_cts=NULL;
		ctx->log_cte=NULL;
		ctx->log_ct=NULL;
	}
}

/** Free JIT context */
BGBSVM_API void BSVM_Jit2_FreeContext(bsjContext ctx)
{
	BSVM_Jit2_TeardownContext(ctx);
	gcfree(ctx);
}

/** Returns a bool indicating whether the context output is stable. */
BGBSVM_API bool BSVM_Jit2_ContextOutputStable(bsjContext ctx)
{
	if(ctx->n_passes>=2)
		return(true);
	return(false);
}

/** Get ASM output from JIT pass */
BGBSVM_API char *BSVM_Jit2_GetOutputAsm(bsjContext ctx)
{
	char *buf;
	
	BSVM_Jit2_AsmFlatten(ctx);
	
	buf=gcatomic(strlen(ctx->asm_cts));
	strcpy(buf, ctx->asm_cts);
	return(buf);
}

/** Get log output from JIT pass */
BGBSVM_API char *BSVM_Jit2_GetOutputLog(bsjContext ctx)
{
	char *buf;
	buf=gcatomic(strlen(ctx->log_cts));
	strcpy(buf, ctx->log_cts);
	return(buf);
}

/** Generate a unique symbol */
BGBSVM_API char *BSVM_Jit2_GenSym(bsjContext ctx)
{
	return(BSVMC_GenSym());
}

/** Begin JIT-compiling function */
BGBSVM_API void BSVM_Jit2_BeginFrame(bsjContext ctx)
{
	BSVM_Jit2_FrameReset(ctx);
	BSVM_J2FrameX86_BeginFrame(ctx);
}

/** End JIT-compiling function */
BGBSVM_API void BSVM_Jit2_EndFrame(bsjContext ctx)
{
	BSVM_J2FrameX86_EndFrame(ctx);
	ctx->n_passes++;
}

/** Reset JIT state between frames.
  * Will discard prior ASM output, but will retain the prior log.
  */
BGBSVM_API void BSVM_Jit2_FrameReset(bsjContext ctx)
{
	BSVM_Jit2_AsmReset(ctx);
}

BGBSVM_API void BSVM_Jit2_ErrorLLn(bsjContext ctx,
	char *file, int line, char *str, ...)
{
	char tbuf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(tbuf, str, lst);
//	BSVM_Jit2_AsmPuts(ctx, tbuf);
//	BSVM_Jit2_TextPuts(ctx, tbuf);
//	BSVM_Jit2_LogPuts(ctx, tbuf);
	va_end(lst);
}

BGBSVM_API void BSVM_Jit2_WarnLLn(bsjContext ctx,
	char *file, int line, char *str, ...)
{
}

BGBSVM_API void BSVM_Jit2_StubErrorLLn(bsjContext ctx,
	char *file, int line)
{
	char tbuf[4096];
	sprintf(tbuf, "BSVM_Jit2_StubError: %s:%d\n", file, line);
	BSVM_Jit2_LogPuts(ctx, tbuf);
}

BGBSVM_API void BSVM_Jit2_CaseErrorLLn(bsjContext ctx,
	char *file, int line)
{
	char tbuf[4096];
	sprintf(tbuf, "BSVM_Jit2_CaseError: %s:%d\n", file, line);
	BSVM_Jit2_LogPuts(ctx, tbuf);
}

BGBSVM_API void BSVM_Jit2_TypeCheckErrorLLn(bsjContext ctx,
	char *file, int line)
{
	char tbuf[4096];
	sprintf(tbuf, "BSVM_Jit2_TypeCheckError: %s:%d\n", file, line);
	BSVM_Jit2_LogPuts(ctx, tbuf);
}

BGBSVM_API void BSVM_Jit2_SanityCheckErrorLLn(bsjContext ctx,
	char *file, int line, char *msg)
{
	char tbuf[4096];
	sprintf(tbuf, "BSVM_Jit2_SanityCheckError: %s:%d %s\n",
		file, line, msg);
	BSVM_Jit2_LogPuts(ctx, tbuf);
}

/** Discard current ASM output */
BGBSVM_API void BSVM_Jit2_AsmReset(bsjContext ctx)
{
	int i;

#if 0
	if(!ctx->asm_cts)
	{
		i=16384;
		ctx->asm_cts=gcatomic(i);
		ctx->asm_cte=ctx->asm_cts+i;
	}

	ctx->asm_ct=ctx->asm_cts;
	*ctx->asm_ct=0;
#endif

	if(ctx->stackpos)
		BSVM_Jit2_SanityCheckError(ctx, "Stack Misaligned");
	if(ctx->markstackpos)
		BSVM_Jit2_SanityCheckError(ctx, "Mark-Stack Misaligned");

	ctx->stackpos=0;
	ctx->markstackpos=0;

	BSVM_Jit2_RefBufferReset(&ctx->text_cts, &ctx->text_cte, &ctx->text_ct);
	BSVM_Jit2_RefBufferReset(&ctx->data_cts, &ctx->data_cte, &ctx->data_ct);
	BSVM_Jit2_RefBufferReset(&ctx->bss_cts, &ctx->bss_cte, &ctx->bss_ct);
	BSVM_Jit2_RefBufferReset(&ctx->asm_cts, &ctx->asm_cte, &ctx->asm_ct);
}

/** Flatten ASM output */
BGBSVM_API void BSVM_Jit2_AsmFlatten(bsjContext ctx)
{
	if(strlen(ctx->text_cts)>0)
	{
		BSVM_Jit2_AsmPuts(ctx, "[section .text]\n[bits 32]\n");
		BSVM_Jit2_AsmPuts(ctx, ctx->text_cts);
	}
	if(strlen(ctx->data_cts)>0)
	{
		BSVM_Jit2_AsmPuts(ctx, "[section .data]\n[bits 32]\n");
		BSVM_Jit2_AsmPuts(ctx, ctx->data_cts);
	}
	if(strlen(ctx->bss_cts)>0)
	{
		BSVM_Jit2_AsmPuts(ctx, "[section .bss]\n[bits 32]\n");
		BSVM_Jit2_AsmPuts(ctx, ctx->bss_cts);
	}
}

/** Emit string into ASM output buffer */
BGBSVM_API void BSVM_Jit2_AsmPuts(bsjContext ctx, char *str)
{
	int i, j, k, sz;

	if(!ctx)
	{
		printf("BSVM_Jit2_AsmPuts: No Context, Msg=%s\n", str);
		return;
	}

	if(!ctx->asm_cts)
	{
		i=16384;
		ctx->asm_cts=gcatomic(i);
		ctx->asm_cte=ctx->asm_cts+i;
		ctx->asm_ct=ctx->asm_cts;
	}
	
	sz=strlen(str);
	if((ctx->asm_ct+sz+1)>=ctx->asm_cte)
	{
		i=ctx->asm_cte-ctx->asm_cts;
		j=ctx->asm_ct-ctx->asm_cts;
		
		k=j+sz+1;
		while(i<=k)i+=i>>1;
		ctx->asm_cts=gcrealloc(ctx->asm_cts, i);
		ctx->asm_cte=ctx->asm_cts+i;
		ctx->asm_ct=ctx->asm_cts+j;
	}

	strcpy(ctx->asm_ct, str);
	ctx->asm_ct+=strlen(str);
}

/** Reset a referenced buffer */
BGBSVM_API void BSVM_Jit2_RefBufferReset(
	char **rcts, char **rcte, char **rct)
{
	BSVM_Jit2_RefBufferPuts(rcts, rcte, rct, NULL);
	*rct=*rcts;
	*(*rct)=0;
}

/** Emit string into a referenced buffer */
BGBSVM_API void BSVM_Jit2_RefBufferPuts(
	char **rcts, char **rcte, char **rct, char *str)
{
	char *cts, *cte, *ct;
	int i, j, k, sz;

	cts=*rcts;
	cte=*rcte;
	ct=*rct;

	if(!cts)
	{
		i=16384;
		cts=gcatomic(i);
		cte=cts+i;
		ct=cts;
	}
	
	if(!str)
	{
		*rcts=cts;
		*rcte=cte;
		*rct=ct;
		return;
	}
	
	sz=strlen(str);
	if((ct+sz+1)>=cte)
	{
		i=cte-cts;
		j=ct-cts;
		
		k=j+sz+1;
		while(i<=k)i+=i>>1;
		cts=gcrealloc(cts, i);
		cte=cts+i;
		ct=cts+j;
	}

	strcpy(ct, str);
	ct+=strlen(str);

	*rcts=cts;
	*rcte=cte;
	*rct=ct;
}

/** Emit string into .text section output buffer */
BGBSVM_API void BSVM_Jit2_TextPuts(bsjContext ctx, char *str)
{
	BSVM_Jit2_RefBufferPuts(
		&ctx->text_cts, &ctx->text_cte, &ctx->text_ct,
		str);
}

/** Emit string into .data section output buffer */
BGBSVM_API void BSVM_Jit2_DataPuts(bsjContext ctx, char *str)
{
	BSVM_Jit2_RefBufferPuts(
		&ctx->data_cts, &ctx->data_cte, &ctx->data_ct,
		str);
}

/** Emit string into .bss section output buffer */
BGBSVM_API void BSVM_Jit2_BssPuts(bsjContext ctx, char *str)
{
	BSVM_Jit2_RefBufferPuts(
		&ctx->bss_cts, &ctx->bss_cte, &ctx->bss_ct,
		str);
}

/** Emit string into log output buffer */
BGBSVM_API void BSVM_Jit2_LogPuts(bsjContext ctx, char *str)
{
	int i, j, k, sz;

	if(!ctx)
	{
		printf("BSVM_Jit2_LogPuts: No Context, Msg=%s\n", str);
		return;
	}

	if(!ctx->log_cts)
	{
		i=16384;
		ctx->log_cts=gcatomic(i);
		ctx->log_cte=ctx->log_cts+i;
		ctx->log_ct=ctx->log_cts;
	}
	
	sz=strlen(str);
	if((ctx->log_ct+sz+1)>=ctx->log_cte)
	{
		i=ctx->log_cte-ctx->log_cts;
		j=ctx->log_ct-ctx->log_cts;
		
		k=j+sz+1;
		while(i<=k)i+=i>>1;
		ctx->log_cts=gcrealloc(ctx->log_cts, i);
		ctx->log_cte=ctx->log_cts+i;
		ctx->log_ct=ctx->log_cts+j;
	}

	strcpy(ctx->log_ct, str);
	ctx->log_ct+=strlen(str);
}

BGBSVM_API void bsj2_print_lln(bsjContext ctx,
	char *file, int line, char *str, ...)
{
	char tbuf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(tbuf, str, lst);
//	BSVM_Jit2_AsmPuts(ctx, tbuf);
	BSVM_Jit2_TextPuts(ctx, tbuf);
	BSVM_Jit2_LogPuts(ctx, tbuf);
	va_end(lst);
}

BGBSVM_API void bsj2_printlog_lln(bsjContext ctx,
	char *file, int line, char *str, ...)
{
	char tbuf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(tbuf, str, lst);
	BSVM_Jit2_LogPuts(ctx, tbuf);
	va_end(lst);
}

BGBSVM_API void bsj2_printdata_lln(bsjContext ctx,
	char *file, int line, char *str, ...)
{
	char tbuf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(tbuf, str, lst);
	BSVM_Jit2_DataPuts(ctx, tbuf);
	va_end(lst);
}

BGBSVM_API void bsj2_printbss_lln(bsjContext ctx,
	char *file, int line, char *str, ...)
{
	char tbuf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(tbuf, str, lst);
	BSVM_Jit2_BssPuts(ctx, tbuf);
	va_end(lst);
}

BGBSVM_API void bsj2_printasm_lln(bsjContext ctx,
	char *file, int line, char *str, ...)
{
	char tbuf[4096];
	va_list lst;

	va_start(lst, str);
	vsprintf(tbuf, str, lst);
	BSVM_Jit2_AsmPuts(ctx, tbuf);
	va_end(lst);
}
