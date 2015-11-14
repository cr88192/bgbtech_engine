#include <bgbgc.h>
#include <bgbdy.h>

struct dyoo_ctx_s {
dyt obj;
dyt status;

dyt stack[64];
int stackpos;

dyAccessMode access;

dyt rec_objstack[64];
int rec_objstackpos;
};

int dyoo_ctx_tls=-1;
int dyoo_access_tls=-1;

void dyo_init()
{
	static int init=0;

	if(init)return;
	init=1;

	dyoo_ctx_tls=thAllocTls();
	dyoo_access_tls=thAllocTls();
}

BGBDY_API dyt dyStatus()
{
	struct dyoo_ctx_s *ctx;

	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)return(NULL);
	return(ctx->status);
}

BGBDY_API void dySetStatus(char *str)
{
	struct dyoo_ctx_s *ctx;

	dyo_init();
#if 0
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		gclock(ctx);
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
#endif
	ctx=DYO_GetCtx();

	ctx->status=dysymbol(str);
}

BGBDY_API void dyClearStatus()
{
	struct dyoo_ctx_s *ctx;

	dyo_init();
#if 0
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		gclock(ctx);
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
#endif
	ctx=DYO_GetCtx();

	ctx->status=NULL;
}

BGBDY_API dyAccessMode dyGetAccessMode()
{
	struct dyoo_ctx_s *ctx;
	dyAccessMode acc;

	dyo_init();
//	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
//	if(!ctx)return(0);
//	return(ctx->access);

	acc=(dyAccessMode)thGetTlsValue(dyoo_access_tls);
	return(acc);
}

BGBDY_API void dySetAccessMode(dyAccessMode access)
{
	struct dyoo_ctx_s *ctx;

#if 0
	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}

	ctx->access=access;
#endif

	thSetTlsValue(dyoo_access_tls, (void *)access);
}

void *DYO_GetCtx(void)
{
	struct dyoo_ctx_s *ctx;

	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s));
		gclock(ctx);

//		ctx=(struct dyoo_ctx_s *)
//			malloc(sizeof(struct dyoo_ctx_s));
//		memset(ctx, 0, sizeof(struct dyoo_ctx_s));

		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
	return((void *)ctx);
}

void DYO_RecursivePush(dyt obj)
{
	struct dyoo_ctx_s *ctx;

	ctx=DYO_GetCtx();

//	if(ctx->rec_objstackpos>=256)
	if((ctx->rec_objstackpos<0) || (ctx->rec_objstackpos>=64))
		{ *(int *)-1=-1; }

	gcSet(ctx->rec_objstack[ctx->rec_objstackpos++], obj);

//	ctx->status=NULL;
}

dyt DYO_RecursivePop(void)
{
	struct dyoo_ctx_s *ctx;
	dyt t;

	ctx=DYO_GetCtx();

//	if(ctx->rec_objstackpos<=0)
	if((ctx->rec_objstackpos<=0) || (ctx->rec_objstackpos>64))
		{ *(int *)-1=-1; }

	t=ctx->rec_objstack[--ctx->rec_objstackpos];
	return(t);
}

bool DYO_RecursiveCheck(dyt obj)
{
	struct dyoo_ctx_s *ctx;
	int i;

	ctx=DYO_GetCtx();

	if((ctx->rec_objstackpos<0) || (ctx->rec_objstackpos>64))
		{ *(int *)-1=-1; }
	
	for(i=0; i<ctx->rec_objstackpos; i++)
		if(ctx->rec_objstack[i]==obj)
			return(true);
	return(false);
}

dyt dyoThis()
{
	struct dyoo_ctx_s *ctx;

	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)return(NULL);
	return(ctx->obj);
}

void dyoSetThis(dyt obj)
{
	struct dyoo_ctx_s *ctx;

#if 0
	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
#endif
	ctx=DYO_GetCtx();

	ctx->obj=obj;
}

void dyoPushThis(dyt obj)
{
	struct dyoo_ctx_s *ctx;

#if 0
	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
#endif
	ctx=DYO_GetCtx();

	if(ctx->stackpos>=256)
		{ *(int *)-1=-1; }

	ctx->stack[ctx->stackpos++]=ctx->obj;
	ctx->obj=obj;
}

void dyoPopThis()
{
	struct dyoo_ctx_s *ctx;

#if 0
	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
#endif

	ctx=DYO_GetCtx();

	if(ctx->stackpos<=0)
		{ *(int *)-1=-1; }

	ctx->stackpos--;
	if(ctx->stackpos<0)
	{
		ctx->stackpos=0;
		ctx->obj=NULL;
		return;
	}
	ctx->obj=ctx->stack[ctx->stackpos];
}

void dyoPushThis2(dyt obj, int *stk)
{
	struct dyoo_ctx_s *ctx;

#if 0
	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
#endif
	ctx=DYO_GetCtx();

	if(ctx->stackpos>=256)
		{ *(int *)-1=-1; }

	if(stk)*stk=ctx->stackpos;
	ctx->stack[ctx->stackpos++]=ctx->obj;
	ctx->obj=obj;
}

void dyoPopThis2(int j)
{
	struct dyoo_ctx_s *ctx;

#if 0
	dyo_init();
	ctx=(struct dyoo_ctx_s *)thGetTlsValue(dyoo_ctx_tls);
	if(!ctx)
	{
		ctx=(struct dyoo_ctx_s *)
			gcalloc(sizeof(struct dyoo_ctx_s *));
		thSetTlsValue(dyoo_ctx_tls, ctx);
	}
#endif

	ctx=DYO_GetCtx();

	ctx->stackpos=j;
	ctx->obj=ctx->stack[ctx->stackpos];
}


dyt dyoGet(char *name)
{
	dyt obj;

	obj=dyoThis();
	if(!obj)return(NULL);
	return(dyGet(obj, name));
}

void dyoSet(char *name, dyt val)
{
	dyt obj;

	obj=dyoThis();
	if(!obj)return;
	dySet(obj, name, val);
}

dyt dyoG(char *name)		{ return(dyoGet(name)); }
void dyoS(char *name, dyt val)	{ dyoSet(name, val); }

char *dyoGs(char *name)		{ return((char *)dyoGet(name)); }
int dyoGi(char *name)		{ return(dyintv(dyoGet(name))); }
s64 dyoGl(char *name)		{ return(dylongv(dyoGet(name))); }
double dyoGf(char *name)	{ return(dydoublev(dyoGet(name))); }
double dyoGd(char *name)	{ return(dydoublev(dyoGet(name))); }
vec2 dyoGv2(char *name)		{ return(dyvec2v(dyoGet(name))); }
vec3 dyoGv3(char *name)		{ return(dyvec3v(dyoGet(name))); }
vec4 dyoGv4(char *name)		{ return(dyvec4v(dyoGet(name))); }

void dyoSs(char *name, char *str)	{ dyoSet(name, dystring(str)); }
void dyoSi(char *name, int val)		{ dyoSet(name, dyint(val)); }
void dyoSl(char *name, s64 val)		{ dyoSet(name, dylong(val)); }
void dyoSf(char *name, double val)	{ dyoSet(name, dyfloat(val)); }
void dyoSd(char *name, double val)	{ dyoSet(name, dydouble(val)); }

void dyoSv2(char *name, vec2 val)	
	{ dyoSet(name, dyvec2(v2x(val), v2y(val))); }
void dyoSv3(char *name, vec3 val)
	{ dyoSet(name, dyvec3(v3x(val), v3y(val), v3z(val))); }
void dyoSv4(char *name, vec4 val)
	{ dyoSet(name, dyvec4(v4x(val), v4y(val), v4z(val), v4w(val))); }


dyt dyoC(char *name)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall0(obj, name)); }
dyt dyoC1(char *name, dyt a)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall1(obj, name, a)); }
dyt dyoC2(char *name, dyt a, dyt b)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall2(obj, name, a, b)); }
dyt dyoC3(char *name, dyt a, dyt b, dyt c)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall3(obj, name, a, b, c)); }
dyt dyoC4(char *name, dyt a, dyt b, dyt c, dyt d)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall4(obj, name, a, b, c, d)); }
dyt dyoC5(char *name, dyt a, dyt b, dyt c, dyt d, dyt e)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall5(obj, name, a, b, c, d, e)); }
dyt dyoC6(char *name, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall6(obj, name, a, b, c, d, e, f)); }
dyt dyoC7(char *name, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall7(obj, name, a, b, c, d, e, f, g)); }
dyt dyoC8(char *name, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h)
	{ dyt obj; obj=dyoThis(); if(!obj)return(NULL);
		return(dyCall8(obj, name, a, b, c, d, e, f, g, h)); }

dyt dyoM(dyt obj, char *name)
	{ return(dyCall0(obj, name)); }
dyt dyoM1(dyt obj, char *name, dyt a)
	{ return(dyCall1(obj, name, a)); }
dyt dyoM2(dyt obj, char *name, dyt a, dyt b)
	{ return(dyCall2(obj, name, a, b)); }
dyt dyoM3(dyt obj, char *name, dyt a, dyt b, dyt c)
	{ return(dyCall3(obj, name, a, b, c)); }
dyt dyoM4(dyt obj, char *name, dyt a, dyt b, dyt c, dyt d)
	{ return(dyCall4(obj, name, a, b, c, d)); }
dyt dyoM5(dyt obj, char *name, dyt a, dyt b, dyt c, dyt d, dyt e)
	{ return(dyCall5(obj, name, a, b, c, d, e)); }
dyt dyoM6(dyt obj, char *name, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dyCall6(obj, name, a, b, c, d, e, f)); }
dyt dyoM7(dyt obj, char *name, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g)
	{ return(dyCall7(obj, name, a, b, c, d, e, f, g)); }
dyt dyoM8(dyt obj, char *name, dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h)
	{ return(dyCall8(obj, name, a, b, c, d, e, f, g, h)); }

