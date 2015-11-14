#include <bgbfrir.h>

FRIR_StaticMethodInfo **frir_staticmethod;
int frir_n_staticmethod;
int frir_m_staticmethod;

BGBFRIR_API FRIR_StaticMethodInfo *FRIR_LookupStaticMethod(
	char *path, char *name, char *sig)
{
	FRIR_StaticMethodInfo *cur;
	int i;
	
	for(i=0; i<frir_n_staticmethod; i++)
	{
		cur=frir_staticmethod[i];
		if(strcmp(cur->path, path))
			continue;
		if(strcmp(cur->name, name))
			continue;
		if(strcmp(cur->sig, sig))
			continue;
		return(cur);
	}
	
	return(NULL);
}

BGBFRIR_API FRIR_StaticMethodInfo *FRIR_LookupStaticMethod(
	char *path, char *name, char *sig)
{
	FRIR_StaticMethodInfo *cur;
	int i;
	
	cur=FRIR_LookupStaticMethod(path, name, sig);
	if(cur)return(cur);
	
	if(!frir_staticmethod)
	{
		i=256;
		frir_staticmethod=gcalloc(i*sizeof(FRIR_StaticMethodInfo *));
		frir_m_staticmethod=i;
	}
	
	if((frir_n_staticmethod+1)>=frir_m_staticmethod)
	{
		i=frir_m_staticmethod; i=i+(i>>1);
		frir_staticmethod=gcrealloc(frir_staticmethod,
			i*sizeof(FRIR_StaticMethodInfo *));
		frir_m_staticmethod=i;
	}
	
	i=frir_n_staticmethod++;
	cur=gctalloc("frir_staticmethodinfo_t", sizeof(FRIR_StaticMethodInfo));
	frir_staticmethod[i]=cur;
	
	cur->path=dystrdup(path);
	cur->name=dystrdup(name);
	cur->sig=dystrdup(sig);
	return(cur);
}

BGBFRIR_API FRIR_MethodCallInfo *FRIR_AllocMethodCallInfo()
{
	FRIR_MethodCallInfo *tmp;
	
	tmp=gctalloc("frir_methodcallinfo_t", sizeof(FRIR_MethodCallInfo));
//	tmp->tr_label=-1;
	return(tmp);
}

BGBFRIR_API FRIR_Context *FRIR_AllocContext()
{
	FRIR_Context *tmp;
	
	tmp=gctalloc("frir_context_t", sizeof(FRIR_Context));
	
	tmp->framestack=gcalloc(256*sizeof(FRIR_Frame *));
	tmp->regstack=gcalloc(1024*sizeof(FRIR_Value));
	
	tmp->fspos=tmp->framestack;
	tmp->rspos=tmp->regstack;
	
	return(tmp);
}

BGBFRIR_API FRIR_Frame *FRIR_Thread_NewFrame(FRIR_Context *ctx)
{
	FRIR_Frame *frame;
	int i;

	ctx->fspos++;
	frame=ctx->fspos[-1];
	if(!frame)
	{
		frame=gctalloc("frir_frame_t", sizeof(FRIR_Frame));
		ctx->fspos[-1]=frame;
	}
	
	return(frame);
}

BGBFRIR_API FRIR_Frame *FRIR_Thread_PushFrame(FRIR_Context *ctx)
{
	FRIR_Frame *frame;
	int i;

	frame=FRIR_Thread_NewFrame(ctx);
	frame->r=ctx->r;
	frame->vspos=ctx->vspos;
	frame->aspos=ctx->aspos;
	frame->rspos=ctx->rspos;
	frame->trace=ctx->trace;
	
	ctx->frame=frame;
	return(frame);
}

BGBFRIR_API FRIR_Frame *FRIR_Thread_PopFrame(FRIR_Context *ctx)
{
	FRIR_Frame *frame;
	int i;

	ctx->fspos--;
	if(ctx->fspos<=ctx->framestack)
	{
		ctx->trace=NULL;
		ctx->frame=NULL;
		return(NULL);
	}
	frame=ctx->fspos[-1];
	
	ctx->r=frame->r;
	ctx->vspos=frame->vspos;
	ctx->aspos=frame->aspos;
	ctx->rspos=frame->rspos;
	ctx->trace=frame->trace;
	ctx->frame=frame;
	
	return(frame);
}

BGBFRIR_API void FRIR_Thread_ApplyMethod(FRIR_Context *ctx,
	FRIR_Method *mth, FRIR_Value **args, int nargs)
{
	FRIR_Frame *frame;
	int i;

	frame=FRIR_Thread_PushFrame(ctx);

	ctx->vspos+=mth->req_vars;
	ctx->aspos+=nargs;
	ctx->rspos+=mth->req_regs;

	ctx->lvar=frame->vspos;
	ctx->larg=frame->aspos;
	ctx->r=frame->rspos;
	ctx->trace=mth->trace[0];
	
	for(i=0; i<nargs; i++)
		ctx->larg[i]=*(args[i]);
}

FRIR_Trace *FRIR_ThOpE_RET(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	FRIR_Value val;
	int rret;
//	if(ctx->r[op->a].i!=0)return(tr->jmpnext);
//	return(tr->next);

	val=ctx->r[op->a];
	rret=ctx->frame->reg_ret;
	FRIR_Thread_PopFrame(ctx);
	if(!ctx->trace)
	{
		ctx->ret=FRIR_RET_NORMAL;
		return(NULL);
	}

	if(rret>=0)
		{ ctx->r[rret]=val; }
	return(ctx->trace);
}

FRIR_Trace *FRIR_ThOpE_RETV(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	FRIR_Value val;
	int rret;
//	if(ctx->r[op->a].i!=0)return(tr->jmpnext);
//	return(tr->next);

//	val=ctx->r[op->a];
	val.l=0;

	rret=ctx->frame->reg_ret;
	FRIR_Thread_PopFrame(ctx);
	if(!ctx->trace)
	{
		ctx->ret=FRIR_RET_NORMAL;
		return(NULL);
	}

	if(rret>=0)
		{ ctx->r[rret]=val; }
	return(ctx->trace);
}

FRIR_Trace *FRIR_ThOpE_CALL_S(
	FRIR_Context *ctx, FRIR_Trace *tr, FRIR_Opcode *op)
{
	FRIR_MethodCallInfo *ci;
	FRIR_Value *args[64];
	int i, n;
	
	ci=op->i.pv;
	
	n=ci->nargs;
	for(i=0; i<n; i++)
		{ args[i]=ctx->r+ci->args[i]; }
	
	if(ci->mth)
	{
		ctx->trace=tr->next;
		FRIR_Thread_ApplyMethod(ctx, ci->mth, args, n);
		ctx->frame->reg_ret=op->c;
		return(ctx->trace);
	}
	
	ctx->ret=FRIR_RET_BADMETHOD;
	return(NULL);
}
