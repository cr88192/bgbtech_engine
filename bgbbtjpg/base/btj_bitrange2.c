/*
Bitwise Arithmetic Coder
 */

#include <bgbbtj.h>

#define BGBBTJ_BITARITH2_LOWER		0x00000000
#define BGBBTJ_BITARITH2_UPPER		0xFFFFFFFF
#define BGBBTJ_BITARITH2_WBITS		8
#define BGBBTJ_BITARITH2_WMASK		((1<<BGBBTJ_BITARITH2_WBITS)-1)

#define BGBBTJ_BITARITH2_DATABITS	8	//W: arithmetic coder bits/symbol
#define BGBBTJ_BITARITH2_CTXBITS	13
#define BGBBTJ_BITARITH2_CTXMASK	((1<<BGBBTJ_BITARITH2_CTXBITS)-1)

u16 bgbbtj_bitarith2_divtab[4096];		//division table
u16 bgbbtj_bitarith2_mdltab_0[4096];	//model update table 0
u16 bgbbtj_bitarith2_mdltab_1[4096];	//model update table 1
byte bgbbtj_bitarith2_transtab8[256];	//byte transpose table

#if 0
	i=(ctx->rval>v);
	if(i)
	{
		ctx->rmin=v+1;
		if((++m[1])==63)
			{ m[0]>>=1; m[1]>>=1; }
	}else
	{
		ctx->rmax=v;
		if((++m[0])==63)
			{ m[0]>>=1; m[1]>>=1; }
	}
#endif

void BGBBTJ_BitArith2_Init()
{
	static int init=0;
	int i1, j1, i2, j2;
	int i, j, k;

	if(init)return;
	init=1;

	for(i=0; i<64; i++)
		for(j=0; j<64; j++)
	{
		k=((i+1)<<BGBBTJ_BITARITH2_WBITS)/(i+j+2);
		bgbbtj_bitarith2_divtab[(j<<6)|i]=k;
	}

	for(i=0; i<64; i++)
		for(j=0; j<64; j++)
	{
		i1=i+1; j1=j;
		if(i1>63) { i1=i1>>1; j1=j1>>1; }
		bgbbtj_bitarith2_mdltab_0[(j<<6)|i]=(j1<<6)|i1;

		i2=i; j2=j+1;
		if(j2>63) { i2=i2>>1; j2=j2>>1; }
		bgbbtj_bitarith2_mdltab_1[(j<<6)|i]=(j2<<6)|i2;
	}

	for(i=0; i<256; i++)
	{
		j=((i&1)<<7)|((i&2)<<5)|((i&4)<<3)|((i&8)<<1);
		j|=((i&16)>>1)|((i&32)>>3)|((i&64)>>5)|((i&128)>>7);
		bgbbtj_bitarith2_transtab8[i]=j;
	}
}

#if 0
byte *BGBBTJ_EmitVLI(byte *ct, int val)
{
	if(val<0)
	{
		*ct++=0;
		return(ct);
	}

	if(val<128)
	{
		*ct++=val;
		return(ct);
	}

	if(val<16384)
	{
		*ct++=0x80|(val>>8);
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<2097152)
	{
		*ct++=0xC0|(val>>16);
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<268435456)
	{
		*ct++=0xE0|(val>>24);
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	*ct++=0xF0;
	*ct++=(val>>24)&0xFF;
	*ct++=(val>>16)&0xFF;
	*ct++=(val>>8)&0xFF;
	*ct++=val&0xFF;
	return(ct);
}

byte *BGBBTJ_EmitSVLI(byte *ct, int val)
{
	return(BGBBTJ_EmitVLI(ct,
		(val>=0)?(val<<1):(((-val)<<1)-1)));
}

byte *BGBBTJ_DecodeVLI(byte *cs, int *rval)
{
	int i;
	
	if(!cs)return(NULL);
	
	i=*cs++;
	if(!(i&0x80))
	{
		*rval=i; return(cs);
	}else if((i&0xC0)==0x80)
	{
		i=((i&0x3F)<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xE0)==0xC0)
	{
		i=((i&0x1F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xF0)==0xE0)
	{
		i=((i&0x0F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xF8)==0xF0)
	{
		i=((i&0x0F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}
	
	*rval=0;
	return(NULL);
}

byte *BGBBTJ_DecodeSVLI(byte *cs, int *rval)
{
	int i;
	
	cs=BGBBTJ_DecodeVLI(cs, &i);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	
	*rval=i;
	return(cs);
}
#endif

int BGBBTJ_BitArith2_InputByte(BGBBTJ_ArithContext *ctx)
{
	if(ctx->cs>ctx->cse)
		return(0);

	return(*ctx->cs++);
}

void BGBBTJ_BitArith2_OutputByte(BGBBTJ_ArithContext *ctx, int i)
{
	if(ctx->ct>ctx->cte)
		return;

	*ctx->ct++=i;
}

void BGBBTJ_BitArith2_OutputFixedBit(BGBBTJ_ArithContext *ctx, int i)
{
	u32 r, r2, v;
	int j;

	r=ctx->rmax-ctx->rmin;
	v=ctx->rmin+(r>>1);

	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		BGBBTJ_BitArith2_OutputByte(ctx, ctx->rmin>>24);
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}

int BGBBTJ_BitArith2_InputFixedBit(BGBBTJ_ArithContext *ctx)
{
	u32 r, r2, v, i;

	r=ctx->rmax-ctx->rmin;
	v=ctx->rmin+(r>>1);

	i=(ctx->rval>v);
	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
		ctx->rval<<=8;
		ctx->rval|=BGBBTJ_BitArith2_InputByte(ctx);
	}

	return(i);
}

void BGBBTJ_BitArith2_OutputBit(BGBBTJ_ArithContext *ctx,
	int i, u32 w)
{
	u32 r, r2, v;
	int j;

	r=ctx->rmax-ctx->rmin;
//	v=ctx->rmin+((((u64)r)*((u64)(w<<(32-BGBBTJ_BITARITH2_WBITS))))>>32);
	v=ctx->rmin+(r>>BGBBTJ_BITARITH2_WBITS)*w;

	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	//while bits
	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		BGBBTJ_BitArith2_OutputByte(ctx, ctx->rmin>>24);
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}

int BGBBTJ_BitArith2_InputBit(BGBBTJ_ArithContext *ctx, u32 w)
{
	u32 r, r2, v, i;

	r=ctx->rmax-ctx->rmin;
//	v=ctx->rmin+((((u64)r)*((u64)(w<<(32-BGBBTJ_BITARITH2_WBITS))))>>32);
	v=ctx->rmin+(r>>BGBBTJ_BITARITH2_WBITS)*w;

	i=(ctx->rval>v);
	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	//while bits
	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;

		ctx->rval<<=8;
		ctx->rval|=BGBBTJ_BitArith2_InputByte(ctx);
	}

	return(i);
}

#if 0
int BGBBTJ_BitArith2_InputModelBit(BGBBTJ_ArithContext *ctx,
	u16 *model, int wctx)
{
	u16 *m;
	u32 r, v, w;
	int i, j, k, mv;

	m=model+wctx; mv=*m;
	r=ctx->rmax-ctx->rmin;
	w=bgbbtj_bitarith2_divtab[mv];
	v=ctx->rmin+(r>>BGBBTJ_BITARITH2_WBITS)*w;

	i=(ctx->rval>v);
	if(i)
		{ ctx->rmin=v+1; *m=bgbbtj_bitarith2_mdltab_1[mv]; }
	else
		{ ctx->rmax=v; *m=bgbbtj_bitarith2_mdltab_0[mv]; }

	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		ctx->rmin<<=8;
		ctx->rmax=(ctx->rmax<<8)|255;
		ctx->rval=(ctx->rval<<8)|(*ctx->cs++);
	}

	return(i);
}
#endif

#if 1
void BGBBTJ_BitArith2_OutputContextBit(BGBBTJ_ArithContext *ctx,
	int i, u16 *mctx)
{
	u32 r, v, w;
	int j, mv;

	mv=*mctx;
	r=ctx->rmax-ctx->rmin;
	w=bgbbtj_bitarith2_divtab[mv];
	v=ctx->rmin+(r>>BGBBTJ_BITARITH2_WBITS)*w;

	if(i)
		{ ctx->rmin=v+1; *mctx=bgbbtj_bitarith2_mdltab_1[mv]; }
	else
		{ ctx->rmax=v; *mctx=bgbbtj_bitarith2_mdltab_0[mv]; }

	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		BGBBTJ_BitArith2_OutputByte(ctx, ctx->rmin>>24);
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}

int BGBBTJ_BitArith2_InputContextBit(BGBBTJ_ArithContext *ctx, u16 *mctx)
{
	u32 r, v, w;
	int i, j, k, mv;

	mv=*mctx;
	r=ctx->rmax-ctx->rmin;
	w=bgbbtj_bitarith2_divtab[mv];
	v=ctx->rmin+(r>>BGBBTJ_BITARITH2_WBITS)*w;

	i=(ctx->rval>v);
	if(i)
		{ ctx->rmin=v+1; *mctx=bgbbtj_bitarith2_mdltab_1[mv]; }
	else
		{ ctx->rmax=v; *mctx=bgbbtj_bitarith2_mdltab_0[mv]; }

	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		ctx->rmin<<=8;
		ctx->rmax=(ctx->rmax<<8)|255;
		ctx->rval=(ctx->rval<<8)|(*ctx->cs++);
	}

	return(i);
}
#endif

int BGBBTJ_BitArith2_CtxIndex(int ctx, int mask)
{
	int i;

//	i=((ctx>>8)*251)>>8;
//	return((ctx^(i<<8))&mask);
	
	i=((ctx>>16)*251)>>8;
	return((ctx^(i<<16))&mask);

//	i=((ctx>>12)*251)>>8;
//	return((ctx^(i<<12))&mask);

//	return(ctx&mask);
}

void BGBBTJ_BitArith2_FlushWBits(BGBBTJ_ArithContext *ctx)
{
	while((ctx->rmin!=BGBBTJ_BITARITH2_LOWER) ||
		(ctx->rmax!=BGBBTJ_BITARITH2_UPPER))
	{
		BGBBTJ_BitArith2_OutputByte(ctx, ctx->rmin>>24);

		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}


int BGBBTJ_BitArith2_SetupEncode(BGBBTJ_ArithContext *ctx, byte *out, int sz)
{
	ctx->ct=out;
	ctx->cte=out+sz;
	ctx->rmin=BGBBTJ_BITARITH2_LOWER;
	ctx->rmax=BGBBTJ_BITARITH2_UPPER;
	return(0);
}

int BGBBTJ_BitArith2_SetupDecode(BGBBTJ_ArithContext *ctx, byte *in, int sz)
{
	int i;

	ctx->cs=in;
	ctx->cse=in+sz;
	ctx->rmin=BGBBTJ_BITARITH2_LOWER;
	ctx->rmax=BGBBTJ_BITARITH2_UPPER;

	ctx->rval=BGBBTJ_BITARITH2_LOWER;
	for(i=0; i<4; i++)
		ctx->rval=(ctx->rval<<8)|BGBBTJ_BitArith2_InputByte(ctx);
	return(0);
}

//arithmetic frontend/modeler
int BGBBTJ_BitArith2_Predict(
	BGBBTJ_ArithContext *ctx, u16 *model, int wctx)
{
	return(bgbbtj_bitarith2_divtab[model[wctx]]);
}

void BGBBTJ_BitArith2_Update(
	BGBBTJ_ArithContext *ctx, u16 *model, int wctx, int v)
{
	u16 *m;
	int i, mv;

	m=model+wctx; mv=*m;
	if(v)
		{ *m=bgbbtj_bitarith2_mdltab_1[mv]; }
	else
		{ *m=bgbbtj_bitarith2_mdltab_0[mv]; }
}

void BGBBTJ_BitArith2_EncodeSymbol(BGBBTJ_ArithContext *ctx, int v)
{
	int wctx;
	int i, j, k;

	i=BGBBTJ_BITARITH2_DATABITS;
	while(i--)
	{
		j=(v>>i)&1;
		wctx=BGBBTJ_BitArith2_CtxIndex(ctx->wctx, ctx->ctxmask);
		k=BGBBTJ_BitArith2_Predict(ctx, ctx->model16, wctx);
		BGBBTJ_BitArith2_Update(ctx, ctx->model16, wctx, j);
		BGBBTJ_BitArith2_OutputBit(ctx, j, k);
		ctx->wctx=(ctx->wctx<<1)|j;

//		k=BGBBTJ_BitArith2_Predict(ctx, ctx->model16, ctx->wctx);
//		BGBBTJ_BitArith2_Update(ctx, ctx->model16, ctx->wctx, j);
//		BGBBTJ_BitArith2_OutputBit(ctx, j, k);
//		ctx->wctx=((ctx->wctx<<1)|j)&ctx->ctxmask;
	}
}

void BGBBTJ_BitArith2_EncodeSymbolLE(BGBBTJ_ArithContext *ctx, int v)
{
	int wctx;
	int i, j, k;

	for(i=0; i<BGBBTJ_BITARITH2_DATABITS; i++)
	{
		j=(v>>i)&1;
		wctx=BGBBTJ_BitArith2_CtxIndex(ctx->wctx, ctx->ctxmask);
		k=BGBBTJ_BitArith2_Predict(ctx, ctx->model16, wctx);
		BGBBTJ_BitArith2_Update(ctx, ctx->model16, wctx, j);
		BGBBTJ_BitArith2_OutputBit(ctx, j, k);
		ctx->wctx=(ctx->wctx<<1)|j;

//		k=BGBBTJ_BitArith2_Predict(ctx, ctx->model16, ctx->wctx);
//		BGBBTJ_BitArith2_Update(ctx, ctx->model16, ctx->wctx, j);
//		BGBBTJ_BitArith2_OutputBit(ctx, j, k);
//		ctx->wctx=((ctx->wctx<<1)|j)&ctx->ctxmask;
	}
}

int BGBBTJ_BitArith2_DecodeSymbol(BGBBTJ_ArithContext *ctx)
{
	int i, j, k, l, v;

	j=ctx->wctx; k=ctx->ctxmask;

	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;
	
	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;

	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;

	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;


	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;

	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;

	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;

	l=BGBBTJ_BitArith2_CtxIndex(j, k);
	i=BGBBTJ_BitArith2_InputContextBit(ctx, ctx->model16+l);
	j=(j<<1)|i;


//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;

//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
//	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;

	ctx->wctx=j;

	return(ctx->wctx&0xFF);
}


int BGBBTJ_BitArith2_DecodeSymbolLE(BGBBTJ_ArithContext *ctx)
{
	int i, j, k, v;

#if 0
	j=ctx->wctx; k=ctx->ctxmask;

	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;

	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith2_InputModelBit(ctx, ctx->model16, j); j=((j<<1)|i)&k;

	ctx->wctx=j;
	return(bgbbtj_bitarith2_transtab8[ctx->wctx&0xFF]);
#endif

	i=BGBBTJ_BitArith2_DecodeSymbol(ctx);
	return(bgbbtj_bitarith2_transtab8[i]);
}

void BGBBTJ_BitArith2_SetupContextBits(
	BGBBTJ_ArithContext *ctx, int bits)
{
	int i, j;

	if(ctx->model16 && (ctx->ctxbits==bits))
	{
		j=1<<bits;
		for(i=0; i<j; i++)ctx->model16[i]=0;
		ctx->wctx=0;
		return;
	}

	if(ctx->model16)
		{ gcfree(ctx->model16); }
	
	j=1<<bits;
	ctx->model16=gcalloc(j*sizeof(u16));
	ctx->ctxbits=bits;
	ctx->ctxmask=(1<<bits)-1;
	ctx->wctx=0;
}

BGBBTJ_API int BGBBTJ_BitArith2_EncodeDataCtx(
	BGBBTJ_ArithContext *ctx,
	byte *ibuf, int isz, byte *obuf, int osz, int flags)
{
	int i, j, k, l, ll, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<0)return(-1);
	if(isz<=0)return(0);

	BGBBTJ_BitArith2_Init();

//	BGBBTJ_BitArith2_SetupContextBits(ctx,
//		BGBBTJ_BITARITH2_CTXBITS);

	ct=obuf; cte=obuf+osz;
	le=flags&32; ll=flags&31;

	BGBBTJ_BitArith2_SetupContextBits(ctx, ll);

	le=1;

	//Emit Header
	*ct++=0xBB;		//magic byte
	*ct++=0xA2;		//magic byte
	ct=BGBBTJ_EmitVLI(ct, ((ll-8)&15)|(le?16:0));
	ct=BGBBTJ_EmitVLI(ct, isz);

	BGBBTJ_BitArith2_SetupEncode(ctx, ct, cte-ct);

	cs=ibuf;
	cse=ibuf+isz;
	i=0;

	if(le)
	{
		while(cs<cse)
			{ BGBBTJ_BitArith2_EncodeSymbolLE(ctx, *cs++); }
	}else
	{
		while(cs<cse)
			{ BGBBTJ_BitArith2_EncodeSymbol(ctx, *cs++); }
	}
	BGBBTJ_BitArith2_FlushWBits(ctx);

	i=ctx->ct-obuf;
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith2_DecodeDataCtx(
	BGBBTJ_ArithContext *ctx,
	byte *ibuf, int isz, byte *obuf, int osz)
{
	int i, j, k, l, fl, ll, osz1, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<=0)return(0);

	BGBBTJ_BitArith2_Init();

	cs=ibuf; cse=ibuf+isz;

	if((*cs++)!=0xBB)
		return(-1);
	if((*cs++)!=0xA2)
		return(-1);

	cs=BGBBTJ_DecodeVLI(cs, &fl);

	if(fl&(~31))
		return(-1);

	BGBBTJ_BitArith2_SetupContextBits(ctx, 8+(fl&15));
	le=(fl&16)?1:0;

	cs=BGBBTJ_DecodeVLI(cs, &osz1);
	if(osz1>osz)return(-1);

	BGBBTJ_BitArith2_SetupDecode(ctx, cs, cse-cs);

	ct=obuf;
	cte=obuf+osz1;
	i=0;

	if(le)
	{
		while(ct<cte)
			{ *ct++=BGBBTJ_BitArith2_DecodeSymbolLE(ctx); }
	}else
	{
		while(ct<cte)
			{ *ct++=BGBBTJ_BitArith2_DecodeSymbol(ctx); }
	}

	i=ct-obuf;
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith2_EncodeData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_ArithContext *ctx;
	int i, j;
	
	ctx=gcalloc(sizeof(BGBBTJ_ArithContext));
	ctx->cs=NULL; ctx->ct=NULL;
	i=BGBBTJ_BitArith2_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 13);
	if(ctx->model)gcfree(ctx->model);
	gcfree(ctx);
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith2_DecodeData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_ArithContext *ctx;
	int i, j;
	
	ctx=gcalloc(sizeof(BGBBTJ_ArithContext));
	ctx->cs=NULL; ctx->ct=NULL;
	i=BGBBTJ_BitArith2_DecodeDataCtx(ctx, ibuf, isz, obuf, osz);
	if(ctx->model)gcfree(ctx->model);
	gcfree(ctx);
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith2_EncodeTestData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_ArithContext *ctx;
	byte *tbuf;
	int i, j, k;

	if(isz<0)return(-1);
	if(isz<=0)return(0);
	
	ctx=gcalloc(sizeof(BGBBTJ_ArithContext));
	ctx->cs=NULL; ctx->ct=NULL;
//	i=BGBBTJ_BitArith2_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 9);
	i=BGBBTJ_BitArith2_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 13);
//	i=BGBBTJ_BitArith2_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 15);

	tbuf=malloc(2*isz);
	j=BGBBTJ_BitArith2_DecodeDataCtx(ctx, obuf, i, tbuf, 2*isz);
	
	if(j!=isz)
	{
		printf("BGBBTJ_BitArith2_EncodeTestData: Size Check %d->%d\n",
			isz, j);
	}
	
	for(k=0; k<j; k++)
	{
		if(ibuf[k]!=tbuf[k])
		{
			printf("BGBBTJ_BitArith2_EncodeTestData: "
				"Decode Fail At %d, %02X!=%02X\n", k, ibuf[k], tbuf[k]);
			break;
		}
	}

	if(k>=j)
	{
//		printf("BGBBTJ_BitArith2_EncodeTestData: OK\n");
	}else
	{
		printf("BGBBTJ_BitArith2_EncodeTestData: Fail\n");
		i=-1;
	}

	free(tbuf);

	if(ctx->model)gcfree(ctx->model);
	gcfree(ctx);
	
	return(i);
}
