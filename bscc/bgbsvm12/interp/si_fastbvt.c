/*
Stuff for faster Boxed-Value-Types.
*/

#include <bgbsvm.h>

static int bsvm_bvt_init=0;
static int bsvm_bvt_szint;
static int bsvm_bvt_szlong;
static int bsvm_bvt_szfloat;
static int bsvm_bvt_szdouble;

void BSVM_BVT_Init()
{
	int i, szp;
	if(bsvm_bvt_init)return;
	bsvm_bvt_init=1;
	
	szp=sizeof(void *);
	i=sizeof(s32);		bsvm_bvt_szint=(i>=szp)?i:szp;
	i=sizeof(s64);		bsvm_bvt_szlong=(i>=szp)?i:szp;
	i=sizeof(f32);		bsvm_bvt_szfloat=(i>=szp)?i:szp;
	i=sizeof(f64);		bsvm_bvt_szdouble=(i>=szp)?i:szp;
}

void BSVM_BVT_Teardown(BSVM_SVMState *ctx)
{
	void *p, *q;

	p=ctx->fbvt_int;
	while(p) { q=*(void **)p; BGBDY_FreeInt32(p); p=q; }
	p=ctx->fbvt_long;
	while(p) { q=*(void **)p; BGBDY_FreeInt64(p); p=q; }
	p=ctx->fbvt_float;
	while(p) { q=*(void **)p; BGBDY_FreeFloat32(p); p=q; }
	p=ctx->fbvt_double;
	while(p) { q=*(void **)p; BGBDY_FreeFloat64(p); p=q; }

	ctx->fbvt_int=NULL;
	ctx->fbvt_long=NULL;
	ctx->fbvt_float=NULL;
	ctx->fbvt_double=NULL;
}

s32 *BSVM_AllocInt(BSVM_SVMState *ctx)
{
	s32 *p;

	if(ctx->fbvt_int)
	{
		p=ctx->fbvt_int;
		ctx->fbvt_int=*(s32 **)p;
		return(p);
	}
	
//	BSVM_BVT_Init();
//	p=gctalloc("_int_t", bsvm_bvt_szint);

	p=BGBDY_AllocInt32();
	return(p);
}

s64 *BSVM_AllocLong(BSVM_SVMState *ctx)
{
	s64 *p;

	if(ctx->fbvt_long)
	{
		p=ctx->fbvt_long;
		ctx->fbvt_long=*(s64 **)p;
		return(p);
	}
	
//	BSVM_BVT_Init();
//	p=gctalloc("_long_t", bsvm_bvt_szlong);
	p=BGBDY_AllocInt64();
	return(p);
}

f32 *BSVM_AllocFloat(BSVM_SVMState *ctx)
{
	f32 *p;

	if(ctx->fbvt_float)
	{
		p=ctx->fbvt_float;
		ctx->fbvt_float=*(f32 **)p;
		return(p);
	}
	
//	BSVM_BVT_Init();
//	p=gctalloc("_float_t", bsvm_bvt_szfloat);
	p=BGBDY_AllocFloat32();
	return(p);
}

f64 *BSVM_AllocDouble(BSVM_SVMState *ctx)
{
	f64 *p;

	if(ctx->fbvt_double)
	{
		p=ctx->fbvt_double;
		ctx->fbvt_double=*(f64 **)p;
		return(p);
	}
	
//	BSVM_BVT_Init();
//	p=gctalloc("_double_t", bsvm_bvt_szdouble);
	p=BGBDY_AllocFloat64();
	return(p);
}


void BSVM_FreeInt(BSVM_SVMState *ctx, s32 *p)
{
	BGBDY_FreeInt32(p);
//	*(s32 **)p=ctx->fbvt_int;
//	ctx->fbvt_int=p;
}

void BSVM_FreeLong(BSVM_SVMState *ctx, s64 *p)
{
	*(s64 **)p=ctx->fbvt_long;
	ctx->fbvt_long=p;
}

void BSVM_FreeFloat(BSVM_SVMState *ctx, f32 *p)
{
	*(f32 **)p=ctx->fbvt_float;
	ctx->fbvt_float=p;
}

void BSVM_FreeDouble(BSVM_SVMState *ctx, f64 *p)
{
	*(f64 **)p=ctx->fbvt_double;
	ctx->fbvt_double=p;
}

s32 *BSVM_WrapAllocInt(BSVM_SVMState *ctx, s32 v)
{
	s32 *p;
	p=BSVM_AllocInt(ctx);
	*p=v;
	return(p);
}

s64 *BSVM_WrapAllocLong(BSVM_SVMState *ctx, s64 v)
{
	s64 *p;
	p=BSVM_AllocLong(ctx);
	*p=v;
	return(p);
}

f32 *BSVM_WrapAllocFloat(BSVM_SVMState *ctx, f32 v)
{
	f32 *p;
	p=BSVM_AllocFloat(ctx);
	*p=v;
	return(p);
}

f64 *BSVM_WrapAllocDouble(BSVM_SVMState *ctx, f64 v)
{
	f64 *p;
	p=BSVM_AllocDouble(ctx);
	*p=v;
	return(p);
}

int BSVM_BoxedIntP(BSVM_SVMState *ctx, dytf p)
{
	char *ty;
	if(dytfFixintP(p))return(0);
	
	ty=dytfgettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_int_t"))return(1);
	return(0);
}

int BSVM_BoxedLongP(BSVM_SVMState *ctx, dytf p)
{
	char *ty;
	if(dytfFixintP(p))return(0);
	
	ty=dytfgettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_long_t"))return(1);
	return(0);
}

int BSVM_BoxedFloatP(BSVM_SVMState *ctx, dytf p)
{
	char *ty;
	if(dytfFixintP(p))return(0);
	
	ty=dytfgettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_float_t"))return(1);
	return(0);
}

int BSVM_BoxedDoubleP(BSVM_SVMState *ctx, dytf p)
{
	char *ty;
	if(dytfFixintP(p))return(0);
	
	ty=dytfgettype(p);
	if(!ty)return(0);
	if(!strcmp(ty, "_double_t"))return(1);
	return(0);
}
