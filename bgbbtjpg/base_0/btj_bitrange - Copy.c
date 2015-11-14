/*
Bitwise Arithmetic Coder
 */

#include <bgbbtj.h>

#define BGBBTJ_BITARITH_LOWER		0x00000000
#define BGBBTJ_BITARITH_UPPER		0xFFFFFFFF
//#define BGBBTJ_BITARITH_NEUTRAL		0x8000
//#define BGBBTJ_BITARITH_WBITS		16
#define BGBBTJ_BITARITH_WBITS		8
#define BGBBTJ_BITARITH_WMASK		((1<<BGBBTJ_BITARITH_WBITS)-1)

#define BGBBTJ_BITARITH_DATABITS	8	//W: arithmetic coder bits/symbol
//#define BGBBTJ_BITARITH_CTXBITS		16
//#define BGBBTJ_BITARITH_CTXBITS		14
#define BGBBTJ_BITARITH_CTXBITS		13
// #define BGBBTJ_BITARITH_CTXBITS		12
// #define BGBBTJ_BITARITH_CTXBITS		10
// #define BGBBTJ_BITARITH_CTXBITS		8
#define BGBBTJ_BITARITH_CTXMASK		((1<<BGBBTJ_BITARITH_CTXBITS)-1)

u16 bgbbtj_bitarith_divtab[65536];		//division table
// u32 bgbbtj_bitarith_divtab2[65536];		//division table (32 bit)
byte bgbbtj_bitarith_transtab8[256];	//byte transpose table

void BGBBTJ_BitArith_Init()
{
	static int init=0;
	int i, j, k;

	if(init)return;
	init=1;

	for(i=0; i<256; i++)
		for(j=0; j<256; j++)
	{
		k=((i+1)<<BGBBTJ_BITARITH_WBITS)/(i+j+2);
		bgbbtj_bitarith_divtab[i+(j<<8)]=k;
//		bgbbtj_bitarith_divtab2[i+(j<<8)]=k<<16;
	}

	for(i=0; i<256; i++)
	{
		j=((i&1)<<7)|((i&2)<<5)|((i&4)<<3)|((i&8)<<1);
		j|=((i&16)>>1)|((i&32)>>3)|((i&64)>>5)|((i&128)>>7);
		bgbbtj_bitarith_transtab8[i]=j;
	}
}

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
	
//	*(int *)-1=-1;
	*rval=0;
//	return(cs);
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

int BGBBTJ_BitArith_InputByte(BGBBTJ_ArithContext *ctx)
{
	if(ctx->cs>ctx->cse)
		return(0);

	return(*ctx->cs++);
}

void BGBBTJ_BitArith_OutputByte(BGBBTJ_ArithContext *ctx, int i)
{
	if(ctx->ct>ctx->cte)
		return;

	*ctx->ct++=i;
}

void BGBBTJ_BitArith_OutputBit(BGBBTJ_ArithContext *ctx,
	int i, u32 w)
{
	u32 r, r2, v;
	int j;

	r=ctx->rmax-ctx->rmin;
//	v=ctx->rmin+((((u64)r)*((u64)(w<<(32-BGBBTJ_BITARITH_WBITS))))>>32);
	v=ctx->rmin+(r>>BGBBTJ_BITARITH_WBITS)*w;

	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	//while bits
//	while((ctx->rmin&0xFF000000)==(ctx->rmax&0xFF000000))
	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		BGBBTJ_BitArith_OutputByte(ctx, ctx->rmin>>24);
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}

int BGBBTJ_BitArith_InputBit(BGBBTJ_ArithContext *ctx, u32 w)
{
	u32 r, r2, v, i;

	r=ctx->rmax-ctx->rmin;
//	v=ctx->rmin+((((u64)r)*((u64)(w<<(32-BGBBTJ_BITARITH_WBITS))))>>32);
	v=ctx->rmin+(r>>BGBBTJ_BITARITH_WBITS)*w;

	i=(ctx->rval>v);
	if(i)ctx->rmin=v+1;
	else ctx->rmax=v;

	//while bits
//	while((ctx->rmin&0xFF000000)==(ctx->rmax&0xFF000000))
	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;

		ctx->rval<<=8;
		ctx->rval|=BGBBTJ_BitArith_InputByte(ctx);
	}

	return(i);
}

#if 1
int BGBBTJ_BitArith_InputModelBit(BGBBTJ_ArithContext *ctx,
	byte *model, int wctx)
{
	byte *m;
	u32 r, v, w;
	int i, j, k;

	m=model+(wctx<<1);
	r=ctx->rmax-ctx->rmin;
//	w=bgbbtj_bitarith_divtab[((u16 *)model)[wctx]];
	w=bgbbtj_bitarith_divtab[*(u16 *)m];
	v=ctx->rmin+(r>>BGBBTJ_BITARITH_WBITS)*w;

	i=(ctx->rval>v);
	if(i)
	{
		ctx->rmin=v+1;
//		if((++m[1])&0x80)
		if((++m[1])==255)
			{ m[0]>>=1; m[1]>>=1; }
	}else
	{
		ctx->rmax=v;
//		if((++m[0])&0x80)
		if((++m[0])==255)
			{ m[0]>>=1; m[1]>>=1; }
	}

	while(!((ctx->rmin^ctx->rmax)>>24))
	{
		ctx->rmin<<=8;
		ctx->rmax=(ctx->rmax<<8)|255;
//		ctx->rval=(ctx->rval<<8)|BGBBTJ_BitArith_InputByte(ctx);
		ctx->rval=(ctx->rval<<8)|(*ctx->cs++);
	}

	return(i);
}
#endif

void BGBBTJ_BitArith_FlushWBits(BGBBTJ_ArithContext *ctx)
{
	while((ctx->rmin!=BGBBTJ_BITARITH_LOWER) ||
		(ctx->rmax!=BGBBTJ_BITARITH_UPPER))
	{
		BGBBTJ_BitArith_OutputByte(ctx, ctx->rmin>>24);

		ctx->rmin<<=8;
		ctx->rmax<<=8;
		ctx->rmax|=255;
	}
}


int BGBBTJ_BitArith_SetupEncode(BGBBTJ_ArithContext *ctx, byte *out, int sz)
{
	ctx->ct=out;
	ctx->cte=out+sz;
	ctx->rmin=BGBBTJ_BITARITH_LOWER;
	ctx->rmax=BGBBTJ_BITARITH_UPPER;
	return(0);
}

int BGBBTJ_BitArith_SetupDecode(BGBBTJ_ArithContext *ctx, byte *in, int sz)
{
	int i;

	ctx->cs=in;
	ctx->cse=in+sz;
	ctx->rmin=BGBBTJ_BITARITH_LOWER;
	ctx->rmax=BGBBTJ_BITARITH_UPPER;
//	ctx->range=BGBBTJ_BITARITH_UPPER;

	ctx->rval=BGBBTJ_BITARITH_LOWER;
	for(i=0; i<4; i++)
		ctx->rval=(ctx->rval<<8)|BGBBTJ_BitArith_InputByte(ctx);
	return(0);
}

//arithmetic frontend/modeler
#if 1
int BGBBTJ_BitArith_Predict(
	BGBBTJ_ArithContext *ctx, byte *model, int wctx)
{
	return(bgbbtj_bitarith_divtab[((u16 *)ctx->model)[wctx]]);

//	return(bgbbtj_bitarith_divtab[
//		model[(ctx<<1)|0]|
//		(model[(ctx<<1)|1]<<8)]);
}
#endif

#if 0
int BGBBTJ_BitArith_Predict(BGBBTJ_ArithContext *ctx, byte *model, int wctx)
{
	u32 i, j, k;

	wctx<<=1;
	i=model[wctx|0];
	j=model[wctx|1];

	k=((i+1)<<BGBBTJ_BITARITH_WBITS)/(i+j+2);

//	k=(k<WARITH2_CLAMP)?WARITH2_CLAMP:
//		((k>(4096-WARITH2_CLAMP))?(4096-WARITH2_CLAMP):k);
//	k=BGBBTJ_BITARITH_NEUTRAL;

	return(k);
}
#endif

void BGBBTJ_BitArith_Update(
	BGBBTJ_ArithContext *ctx, byte *model, int wctx, int v)
{
	int i;

	i=(wctx<<1);
	ctx->model[i|v]++;
//	if(ctx->model[i|v]&0x80)
//	if(ctx->model[i|v]>=254)
	if(ctx->model[i|v]==255)
	{
		ctx->model[i|0]>>=1;
		ctx->model[i|1]>>=1;
	}
}

void BGBBTJ_BitArith_EncodeSymbol(BGBBTJ_ArithContext *ctx, int v)
{
	int i, j, k;

	i=BGBBTJ_BITARITH_DATABITS;
	while(i--)
	{
		j=(v>>i)&1;

		k=BGBBTJ_BitArith_Predict(ctx, ctx->model, ctx->wctx);
		BGBBTJ_BitArith_Update(ctx, ctx->model, ctx->wctx, j);
//		k=BGBBTJ_BITARITH_NEUTRAL;

		BGBBTJ_BitArith_OutputBit(ctx, j, k);
//		ctx->wctx=((ctx->wctx<<1)|j)&BGBBTJ_BITARITH_CTXMASK;
		ctx->wctx=((ctx->wctx<<1)|j)&ctx->ctxmask;
	}
}

int BGBBTJ_BitArith_DecodeSymbol(BGBBTJ_ArithContext *ctx)
{
	int i, j, k, v;

#if 1
	j=ctx->wctx; k=ctx->ctxmask;

	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;

	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;
	i=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, j); j=((j<<1)|i)&k;

	ctx->wctx=j;
#endif

#if 0
//	v=0;
	i=BGBBTJ_BITARITH_DATABITS;
	while(i--)
	{
//		k=BGBBTJ_BitArith_Predict(ctx->model, ctx->wctx);
//		k=BGBBTJ_BITARITH_NEUTRAL;
//		j=BGBBTJ_BitArith_InputBit(k);
//		BGBBTJ_BitArith_Update(ctx->model, ctx->wctx, j);

//		ctx->wctx=v&((1<<WARITH2_CTXBITS)-1);
		j=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, ctx->wctx);

//		ctx->wctx=((ctx->wctx<<1)|j)&((1<<WARITH2_CTXBITS)-1);
//		ctx->wctx=((ctx->wctx<<1)|j)&BGBBTJ_BITARITH_CTXMASK;
		ctx->wctx=((ctx->wctx<<1)|j)&ctx->ctxmask;
	}
#endif

	return(ctx->wctx&0xFF);
//	return(ctx->wctx&((1<<BGBBTJ_BITARITH_DATABITS)-1));
//	return(v);
}

void BGBBTJ_BitArith_EncodeSymbolLE(BGBBTJ_ArithContext *ctx, int v)
{
	int i, j, k;

//	i=BGBBTJ_BITARITH_DATABITS;
//	while(i--)
	for(i=0; i<BGBBTJ_BITARITH_DATABITS; i++)
	{
		j=(v>>i)&1;

		k=BGBBTJ_BitArith_Predict(ctx, ctx->model, ctx->wctx);
		BGBBTJ_BitArith_Update(ctx, ctx->model, ctx->wctx, j);
//		k=BGBBTJ_BITARITH_NEUTRAL;

		BGBBTJ_BitArith_OutputBit(ctx, j, k);
//		ctx->wctx=((ctx->wctx<<1)|j)&BGBBTJ_BITARITH_CTXMASK;
		ctx->wctx=((ctx->wctx<<1)|j)&ctx->ctxmask;
//		ctx->wctx=((ctx->wctx|(j<<ctx->ctxbits))>>1)&ctx->ctxmask;
	}
}

int BGBBTJ_BitArith_DecodeSymbolLE(BGBBTJ_ArithContext *ctx)
{
	int i, j, k, v;

//	v=0;
	i=BGBBTJ_BITARITH_DATABITS;
	while(i--)
//	for(i=0; i<BGBBTJ_BITARITH_DATABITS; i++)
	{
//		k=BGBBTJ_BitArith_Predict(ctx->model, ctx->wctx);
//		k=BGBBTJ_BITARITH_NEUTRAL;
//		j=BGBBTJ_BitArith_InputBit(k);
//		BGBBTJ_BitArith_Update(ctx->model, ctx->wctx, j);

//		ctx->wctx=v&((1<<WARITH2_CTXBITS)-1);
		j=BGBBTJ_BitArith_InputModelBit(ctx, ctx->model, ctx->wctx);

//		ctx->wctx=((ctx->wctx<<1)|j)&((1<<WARITH2_CTXBITS)-1);
//		ctx->wctx=((ctx->wctx<<1)|j)&BGBBTJ_BITARITH_CTXMASK;
		ctx->wctx=((ctx->wctx<<1)|j)&ctx->ctxmask;
//		ctx->wctx=((ctx->wctx|(j<<ctx->ctxbits))>>1)&ctx->ctxmask;
	}

//	return(ctx->wctx&0xFF);
//	return(ctx->wctx&((1<<BGBBTJ_BITARITH_DATABITS)-1));
//	return(v);
//	return((ctx->wctx>>(ctx->ctxbits-BGBBTJ_BITARITH_DATABITS))&
//		((1<<BGBBTJ_BITARITH_DATABITS)-1));

	return(bgbbtj_bitarith_transtab8[ctx->wctx&0xFF]);
}

void BGBBTJ_BitArith_SetupContextBits(
	BGBBTJ_ArithContext *ctx, int bits)
{
	int i, j;

	if(ctx->model && (ctx->ctxbits==bits))
	{
		j=1<<bits;
		for(i=0; i<j*2; i++)ctx->model[i]=0;
		ctx->wctx=0;
		return;
	}

	if(ctx->model)
		{ gcfree(ctx->model); }
	
	j=1<<bits;
	ctx->model=gcalloc(j*2*sizeof(byte));
	ctx->ctxbits=bits;
	ctx->ctxmask=(1<<bits)-1;
	ctx->wctx=0;
}

BGBBTJ_API int BGBBTJ_BitArith_EncodeDataCtx(
	BGBBTJ_ArithContext *ctx,
	byte *ibuf, int isz, byte *obuf, int osz, int flags)
{
	int i, j, k, l, ll, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<=0)return(0);

	BGBBTJ_BitArith_Init();

	BGBBTJ_BitArith_SetupContextBits(ctx,
		BGBBTJ_BITARITH_CTXBITS);

	ct=obuf; cte=obuf+osz;
	le=flags&32; ll=flags&31;

	le=1;

	//Emit Header
	*ct++=0xBA;		//magic byte
//	*ct++=0x00|((BGBBTJ_BITARITH_CTXBITS-12)&15)|(le?16:0);
//	*ct++=0x00|((ll-12)&15)|(le?16:0);

	ct=BGBBTJ_EmitVLI(ct, ((ll-12)&15)|(le?16:0));
	ct=BGBBTJ_EmitVLI(ct, isz);

	BGBBTJ_BitArith_SetupEncode(ctx, ct, cte-ct);
//	BGBBTJ_BitArith_SetupEncode(ctx, obuf, osz);

	cs=ibuf;
	cse=ibuf+isz;
	i=0;

	if(le)
	{
		while(cs<cse)
		{
			j=*cs++;
			BGBBTJ_BitArith_EncodeSymbolLE(ctx, j);
		}
	}else
	{
		while(cs<cse)
		{
			j=*cs++;
			BGBBTJ_BitArith_EncodeSymbol(ctx, j);
		}
	}
	BGBBTJ_BitArith_FlushWBits(ctx);

	i=ctx->ct-obuf;
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith_DecodeDataCtx(
	BGBBTJ_ArithContext *ctx,
	byte *ibuf, int isz, byte *obuf, int osz)
{
	int i, j, k, l, fl, ll, osz1, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<=0)return(0);

	BGBBTJ_BitArith_Init();

	cs=ibuf; cse=ibuf+isz;
//	if((cs[0]!=0xBA)||(cs[1]!=0x00))
//	if((cs[0]!=0xBA)||(cs[1]&0xE0))
//		return(-1);

	if(*cs!=0xBA)
		return(-1);
	cs++;

	cs=BGBBTJ_DecodeVLI(cs, &fl);

	if(fl&(~31))
		return(-1);

	BGBBTJ_BitArith_SetupContextBits(ctx, 12+(fl&15));
	le=(fl&16)?1:0;

//	BGBBTJ_BitArith_SetupContextBits(ctx, 12+(cs[1]&15));
//	le=(cs[1]&16)?1:0;

//	cs+=2;

	cs=BGBBTJ_DecodeVLI(cs, &osz1);
	if(osz1>osz)return(-1);

	BGBBTJ_BitArith_SetupDecode(ctx, cs, cse-cs);
//	BGBBTJ_BitArith_SetupDecode(ctx, ibuf, isz);

	ct=obuf;
//	cte=obuf+osz;
	cte=obuf+osz1;
	i=0;

	if(le)
	{
		while(ct<cte)
		{
//			j=BGBBTJ_BitArith_DecodeSymbol(ctx);
			j=BGBBTJ_BitArith_DecodeSymbolLE(ctx);
			*ct++=j;
		}
	}else
	{
		while(ct<cte)
		{
			j=BGBBTJ_BitArith_DecodeSymbol(ctx);
//			j=BGBBTJ_BitArith_DecodeSymbolLE(ctx);
			*ct++=j;
		}
	}

//	i=ctx->cs-ibuf;
	i=ct-obuf;
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith_EncodeData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_ArithContext *ctx;
	int i, j;
	
	ctx=gcalloc(sizeof(BGBBTJ_ArithContext));
	ctx->cs=NULL; ctx->ct=NULL;
//	j=1<<BGBBTJ_BITARITH_CTXBITS;
//	ctx->model=gcalloc(j*2*sizeof(byte));
	i=BGBBTJ_BitArith_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 13);

	if(ctx->model)gcfree(ctx->model);
	gcfree(ctx);
	
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith_DecodeData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_ArithContext *ctx;
	int i, j;
	
	ctx=gcalloc(sizeof(BGBBTJ_ArithContext));
	ctx->cs=NULL; ctx->ct=NULL;
//	j=1<<BGBBTJ_BITARITH_CTXBITS;
//	ctx->model=gcalloc(j*2*sizeof(byte));
	i=BGBBTJ_BitArith_DecodeDataCtx(ctx, ibuf, isz, obuf, osz);
	
	if(ctx->model)gcfree(ctx->model);
	gcfree(ctx);
	
	return(i);
}

BGBBTJ_API int BGBBTJ_BitArith_EncodeTestData(
	byte *ibuf, int isz, byte *obuf, int osz)
{
	BGBBTJ_ArithContext *ctx;
	byte *tbuf;
	int i, j, k;
	
	ctx=gcalloc(sizeof(BGBBTJ_ArithContext));
	ctx->cs=NULL; ctx->ct=NULL;
//	j=1<<BGBBTJ_BITARITH_CTXBITS;
//	ctx->model=gcalloc(j*2*sizeof(byte));
	i=BGBBTJ_BitArith_EncodeDataCtx(ctx, ibuf, isz, obuf, osz, 13);

	tbuf=malloc(2*isz);
	j=BGBBTJ_BitArith_DecodeDataCtx(ctx, obuf, i, tbuf, 2*isz);
	
	if(j!=isz)
	{
		printf("BGBBTJ_BitArith_EncodeTestData: Size Check %d->%d\n",
			isz, j);
	}
	
	for(k=0; k<j; k++)
	{
		if(ibuf[k]!=tbuf[k])
		{
			printf("BGBBTJ_BitArith_EncodeTestData: "
				"Decode Fail At %d\n", k);
			break;
		}
	}

	if(k>=j)
	{
		printf("BGBBTJ_BitArith_EncodeTestData: OK\n");
	}

	free(tbuf);

	if(ctx->model)gcfree(ctx->model);
	gcfree(ctx);
	
	return(i);
}
