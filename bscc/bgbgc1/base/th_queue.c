#include <bgbgc.h>
#include <setjmp.h>

static void *dy_queue_mutex=NULL;

static int dyex_handler_tls=-1;
static int dyex_throw_tls=-1;
static int dyex_final_tls=-1;

static void queue_lock()
	{ thLockMutex(dy_queue_mutex); }
static void queue_unlock()
	{ thUnlockMutex(dy_queue_mutex); }

void *thNewQueue()
{
	BGBGC_Queue *ctx;
	if(!dy_queue_mutex)dy_queue_mutex=thMutex();
	ctx=gctalloc("_queue_t", sizeof(BGBGC_Queue));
	return(ctx);
}

int thQueueMsg(void *obj)
{
	BGBGC_Queue *ctx;
	int i;
	queue_lock();
	ctx=(BGBGC_Queue *)obj; i=(ctx->orov-ctx->irov)&63;
	queue_unlock();
	return(i);
}

int thQueueMsgP(void *obj)
{
	BGBGC_Queue *ctx;
	ctx=(BGBGC_Queue *)obj;
	return(ctx->orov!=ctx->irov);
}

void *thQueueRecv(void *obj)
{
	BGBGC_Queue *ctx;
	void *p;
	ctx=(BGBGC_Queue *)obj;

	if(ctx->orov==ctx->irov)
	{
#if 0
		if(ctx->ovf)
		{
			queue_lock();
			p=dycar(ctx->ovf);
			ctx->ovf=dycdr(ctx->ovf);
			queue_unlock();
			return(p);
		}
#endif

		while(ctx->orov==ctx->irov)thWaitPtr(obj);
	}

	queue_lock();
	p=(void *)(ctx->msg[ctx->irov]);
	ctx->irov=(ctx->irov+1)&63;
	queue_unlock();
	return(p);
}

void *thQueueTryRecv(void *obj)
{
	BGBGC_Queue *ctx;
	void *p;
	ctx=(BGBGC_Queue *)obj;

	if(ctx->orov==ctx->irov)
		return(NULL);
	queue_lock();
	p=(void *)(ctx->msg[ctx->irov]);
	ctx->irov=(ctx->irov+1)&63;
	queue_unlock();
	return(p);
}

void *thQueueSend(void *obj, void *val)
{
	BGBGC_Queue *ctx;

	ctx=(BGBGC_Queue *)obj;
	queue_lock();
	if(((ctx->orov+1)&63)==ctx->irov)
	{
//		ctx->ovf=dynappend(ctx->ovf, dylist1(val));
		queue_unlock();
		thSignalPtr(obj);
		return(NULL);
	}
	ctx->msg[ctx->orov]=val;
	ctx->orov=(ctx->orov+1)&63;
	queue_unlock();
	thSignalPtr(obj);
	return(NULL);
}

struct bgbgc_handler_s {
struct bgbgc_handler_s *last;
jmp_buf jmp;
char *str;

void *data;
void (*final_f)(void *data);
};

void thThrow(char *name)
{
	struct bgbgc_handler_s *ctx;

	if(dyex_handler_tls<0)dyex_handler_tls=thAllocTls();

	ctx=thGetTlsValue(dyex_handler_tls);

	if(ctx->final_f)
		ctx->final_f(ctx->data);

	ctx->str=gcstrdup(name);
	longjmp(ctx->jmp, -1);
}

void thTryCatchFinal(void *data,
	void (*try_f)(void *data),
	void (*catch_f)(void *data, char *name),
	void (*final_f)(void *data))
{
	struct bgbgc_handler_s ctx;
	char *str;

	if(dyex_handler_tls<0)dyex_handler_tls=thAllocTls();

	ctx.data=data;

	ctx.last=thGetTlsValue(dyex_handler_tls);
	if(!setjmp(ctx.jmp))
	{
		ctx.final_f=NULL;
		thSetTlsValue(dyex_handler_tls, &ctx);
		try_f(data);
		thSetTlsValue(dyex_handler_tls, ctx.last);
		if(final_f)final_f(data);
	}else
	{
		if(catch_f)
		{
			ctx.last->data=data;
			ctx.last->final_f=final_f;
			thSetTlsValue(dyex_handler_tls, ctx.last);
			catch_f(data, ctx.str);
			ctx.last->final_f=NULL;
			final_f(data);
		}else
		{
			if(final_f)final_f(data);
			thThrow(ctx.str);
		}
	}
}

void thTryCatch(void *data,
	void (*try_f)(void *data),
	void (*catch_f)(void *data, char *name))
		{ thTryCatchFinal(data, try_f, catch_f, NULL); }

void thTryFinal(void *data,
	void (*try_f)(void *data),
	void (*final_f)(void *data))
		{ thTryCatchFinal(data, try_f, NULL, final_f); }

