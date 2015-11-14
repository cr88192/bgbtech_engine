#include <frbc2ci.h>

FRBC2CI_API FR2CI_Context *FR2CI_AllocContext(FR2CI_LoaderCtx *ldr)
{
	FR2CI_Context *img;
	
	img=malloc(sizeof(FR2CI_Context));
	memset(img, 0, sizeof(FR2CI_Context));
	img->loader=ldr;
	return(img);
}

FRBC2CI_API int FR2CI_RunContext(FR2CI_Context *ctx)
{
	FR2CI_Trace *trace;
	
	ctx->status=0;
	trace=ctx->nexttrace;
	while(trace)
		{ trace=trace->run(ctx->frame, trace); }
	return(ctx->status);
}

FR2CI_Frame *FR2CI_AllocFrame(FR2CI_Context *ctx)
{
	FR2CI_FrameSlab *slab;
	FR2CI_Frame *tmp;
	int i;

	if(ctx->free_frame)
	{
		tmp=ctx->free_frame;
		ctx->free_frame=*(FR2CI_Frame **)tmp;
		memset(tmp, 0, sizeof(FR2CI_Frame));
		tmp->ctx=ctx;
		return(tmp);
	}
	
	slab=fr2ci_malloc(sizeof(FR2CI_FrameSlab));
	slab->next=ctx->slab_frame;
	ctx->slab_frame=slab;
	
	for(i=0; i<256; i++)
	{
		tmp=&(slab->fr[i]);
		*(FR2CI_Frame **)tmp=ctx->free_frame;
		ctx->free_frame=tmp;
	}
	
	tmp=ctx->free_frame;
	ctx->free_frame=*(FR2CI_Frame **)tmp;
	memset(tmp, 0, sizeof(FR2CI_Frame));
	tmp->ctx=ctx;
	return(tmp);
}

void FR2CI_FreeFrame(FR2CI_Context *ctx, FR2CI_Frame *op)
{
	*(FR2CI_Frame **)op=ctx->free_frame;
	ctx->free_frame=op;
}

void FR2CI_PushNewFrame(FR2CI_Context *ctx)
{
	ctx->framestack[ctx->framestackpos++]=ctx->frame;
	ctx->frame=FR2CI_AllocFrame(ctx);
}

void *FR2CI_ContextPoolAlloc(FR2CI_Context *img, int sz)
{
	byte *pb, *pt;
	void *p;
	int xi, xn;
	int i, j;
	
	xi=(sz+15)>>4;
	if(xi<64)
	{
		if(img->free_pool[xi])
		{
			p=img->free_pool[xi];
			img->free_pool[xi]=*(void **)p;
			memset(p, 0, sz);
			return(p);
		}
		
		xn=65536/(xi<<4);
		pb=fr2ci_malloc(xn*xi*16);
		pt=pb;
		for(i=0; i<xn; i++)
		{
			*(void **)pt=img->free_pool[xi];
			img->free_pool[xi]=(void *)pt;
			pt+=xi<<4;
		}

		p=img->free_pool[xi];
		img->free_pool[xi]=*(void **)p;
		memset(p, 0, sz);
		return(p);
	}
	
	return(fr2ci_malloc(sz));
}

void FR2CI_ContextPoolFree(FR2CI_Context *img, void *ptr, int sz)
{
	int xi, xn;

	xi=(sz+15)>>4;
	if(xi<64)
	{
		*(void **)ptr=img->free_pool[xi];
		img->free_pool[xi]=(void *)ptr;
		return;
	}
	
	fr2ci_free(ptr);
}

void FR2CI_SetupFrameRegs(FR2CI_Context *ctx, FR2CI_Frame *frm, int nreg)
{
	frm->reg=FR2CI_ContextPoolAlloc(ctx, nreg*sizeof(FR2CI_Value));
	frm->n_reg=nreg;
}

void FR2CI_SetupFrameArgs(FR2CI_Context *ctx, FR2CI_Frame *frm, int narg)
{
	frm->arg=FR2CI_ContextPoolAlloc(ctx, narg*sizeof(FR2CI_Value));
	frm->n_arg=narg;
}

void FR2CI_SetupFrameLocals(FR2CI_Context *ctx, FR2CI_Frame *frm, int nloc)
{
	frm->loc=FR2CI_ContextPoolAlloc(ctx, nloc*sizeof(FR2CI_Value));
	frm->n_loc=nloc;
}

void FR2CI_CleanupFrame(FR2CI_Context *ctx, FR2CI_Frame *frm)
{
	FR2CI_CleanupOpcode *clop, *clnxt;
	
	if(frm->clop)
	{
		clop=frm->clop;
		while(clop)
		{
			clop->run(frm, clop);
			clnxt=clop->next;
			clop->next=ctx->free_clop;
			ctx->free_clop=clop;
			clop=clnxt;
		}
	}
	
	if(frm->reg)
	{
		FR2CI_ContextPoolFree(ctx, frm->reg,
			frm->n_reg*sizeof(FR2CI_Value));
	}
	if(frm->arg)
	{
		FR2CI_ContextPoolFree(ctx, frm->arg,
			frm->n_arg*sizeof(FR2CI_Value));
	}
	if(frm->loc)
	{
		FR2CI_ContextPoolFree(ctx, frm->loc,
			frm->n_loc*sizeof(FR2CI_Value));
	}
}

FR2CI_CleanupOpcode *FR2CI_FrameNewCleanupOp(
	FR2CI_Context *ctx, FR2CI_Frame *frm)
{
	FR2CI_CleanupOpcode *op;
	op=FR2CI_AllocCleanupOpcode(ctx);
	op->next=frm->clop;
	frm->clop=op;
	return(op);
}

FR2CI_CleanupOpcode *FR2CI_AllocCleanupOpcode(FR2CI_Context *img)
{
	FR2CI_CleanupOpcode *op;

	if(img->free_clop)
	{
		op=img->free_clop;
		img->free_clop=op->next;
		return(op);
	}

	return(FR2CI_ContextPoolAlloc(img, sizeof(FR2CI_CleanupOpcode)));
}

void FR2CI_FreeCleanupOpcode(FR2CI_Context *img, FR2CI_CleanupOpcode *op)
{
	op->next=img->free_clop;
	img->free_clop=op;

//	FR2CI_ContextPoolFree(img, op, sizeof(FR2CI_CleanupOpcode));
}

FR2CI_Trace *FR2CI_SetupRawCall(FR2CI_Context *ctx, FR2CI_Function *func,
	FR2CI_Value *arg, int narg)
{
	FR2CI_Frame *frm;
	FR2CI_Trace *tr;
	int i;

	frm=FR2CI_AllocFrame(ctx);
//	ctx->framestack[ctx->framestackpos++]=frm;
	ctx->frame=frm;

	FR2CI_SetupFrameArgs(ctx, frm, narg);
	FR2CI_SetupFrameRegs(ctx, frm, func->nregs);
	FR2CI_SetupFrameLocals(ctx, frm, func->nlocals);

	for(i=0; i<narg; i++)
		{ frm->arg[i]=arg[i]; }

	tr=FR2CI_GetFunctionEntryTrace(func);
	ctx->nexttrace=tr;
	
	return(tr);
}

//void FR2CI_Op_BenchNop0(FR2CI_Frame *frm, FR2CI_Opcode *op)
//	{ frm->ctx->stat_n_op++; }
//void FR2CI_Op_BenchNop1(FR2CI_Frame *frm, FR2CI_Opcode *op)
//	{ frm->ctx->stat_n_op++; }
//void FR2CI_Op_BenchNop2(FR2CI_Frame *frm, FR2CI_Opcode *op)
//	{ frm->ctx->stat_n_op++; }
//void FR2CI_Op_BenchNop3(FR2CI_Frame *frm, FR2CI_Opcode *op)
//	{ frm->ctx->stat_n_op++; }

void FR2CI_Op_BenchNop0(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{  }
void FR2CI_Op_BenchNop1(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{  }
void FR2CI_Op_BenchNop2(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{  }
void FR2CI_Op_BenchNop3(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{  }

void FR2CI_Op_BenchArith0(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ frm->reg[op->d].i=frm->reg[op->s].i + frm->reg[op->t].i; }
void FR2CI_Op_BenchArith1(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ frm->reg[op->d].i=frm->reg[op->s].i - frm->reg[op->t].i; }
void FR2CI_Op_BenchArith2(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ frm->reg[op->d].i=frm->reg[op->s].i & frm->reg[op->t].i; }
void FR2CI_Op_BenchArith3(FR2CI_Frame *frm, FR2CI_Opcode *op)
	{ frm->reg[op->d].i=frm->reg[op->s].i ^ frm->reg[op->t].i; }

FR2CI_Trace *FR2CI_TailOp_BenchNext0(FR2CI_Frame *frm, FR2CI_TailOpcode *op)
{
	frm->ctx->stat_n_op+=op->trace->n_op+1;
	frm->ctx->stat_n_tr++;
	if(frm->ctx->stat_n_op>=(1<<20))
		return(NULL);
	return(op->nexttrace);
}


FRBC2CI_API int FR2CI_SetupNopBench(FR2CI_Context *ctx)
{
//	FR2CI_Opcode *t_ops[4096];
//	FR2CI_TailOpcode *t_tops[1024];
//	FR2CI_Trace *t_traces[1024];
	FR2CI_Opcode **t_ops;
	FR2CI_TailOpcode **t_tops;
	FR2CI_Trace **t_traces;

	FR2CI_Image *img;
	FR2CI_Opcode *op;
	FR2CI_TailOpcode *top;
	FR2CI_Trace *trace;
//	byte *cs, *cse, *csip;
	int nops, ntops, ntraces, opsn, topsn, tracesn;
	int i, j, k, ni, nj;

	img=FR2CI_AllocImage();

	t_ops=fr2ci_malloc(4096*sizeof(FR2CI_Opcode *));
	t_tops=fr2ci_malloc(1024*sizeof(FR2CI_TailOpcode *));
	t_traces=fr2ci_malloc(1024*sizeof(FR2CI_Trace *));

	ni=64+(rand()&31);

	nops=0; ntops=0; ntraces=0;
	while(ntraces<ni)
	{
		nj=rand()&31;
//		csip=cs;
		opsn=nops; topsn=ntops;
		while((nops-opsn)<nj)
		{
//			op=FR2CI_DecodeOpcode(func->image, &cs, cse);
			op=FR2CI_AllocOpcode(img);
			
//			op->run=FR2CI_Op_BenchNop0;
			op->run=FR2CI_Op_BenchArith0;

#if 0
			switch(rand()&3)
			{
			case 0: op->run=FR2CI_Op_BenchNop0; break;
			case 1: op->run=FR2CI_Op_BenchNop1; break;
			case 2: op->run=FR2CI_Op_BenchNop2; break;
			case 3: op->run=FR2CI_Op_BenchNop3; break;
			}
#endif

#if 1
			switch(rand()&3)
			{
			case 0: op->run=FR2CI_Op_BenchArith0; break;
			case 1: op->run=FR2CI_Op_BenchArith1; break;
			case 2: op->run=FR2CI_Op_BenchArith2; break;
			case 3: op->run=FR2CI_Op_BenchArith3; break;
			}
#endif
			
			if(!op)break;
			t_ops[nops++]=op;
		}
		
//		top=FR2CI_DecodeTailOpcode(func->image, &cs, cse);
		top=FR2CI_AllocTailOpcode(img);
		top->run=FR2CI_TailOp_BenchNext0;

		if(!top)break;
		t_tops[ntops++]=top;

		trace=FR2CI_CreateTrace(img, t_ops+opsn, top, nops-opsn);
		t_traces[ntraces++]=trace;
//		trace->ip=csip;
	}
//	t_traces[ntraces]=NULL;
	t_traces[ntraces]=t_traces[0];

	for(i=0; i<ntraces; i++)
	{
		trace=t_traces[i];
		trace->top->nexttrace=t_traces[i+1];		
	}

	ctx->frame=FR2CI_AllocFrame(ctx);
	ctx->frame->reg=fr2ci_malloc(16*sizeof(FR2CI_Value));
	ctx->nexttrace=t_traces[0];
	
	return(0);
}
