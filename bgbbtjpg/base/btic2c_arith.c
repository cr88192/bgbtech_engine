/*
Bitwise Arithmetic Coder
 */

#include <bgbbtj.h>

byte btlza_weightadjust_0[256];		//weight adjustment tables (0 bit)
byte btlza_weightadjust_1[256];		//weight adjustment tables (1 bit)

void BTIC2C_BitArith_Init()
{
	static int init=0;
	int i, j, k;

	if(init)return;
	init=1;
	
	for(i=0; i<256; i++)
	{
		j=i+((256-i)>>5);
		k=i-(i>>5);
		if((j==i) && (j<255))j++;
		if((k==i) && (k>0))k--;
		btlza_weightadjust_0[i]=j;
		btlza_weightadjust_1[i]=k;
	}
}

int BTIC2C_BitArith_InputByte(BTIC2C_Context *ctx)
{
//	if(ctx->ari_cs>ctx->ari_cse)
//		return(0);
	return(*ctx->ari_cs++);
}

void BTIC2C_BitArith_OutputByte(BTIC2C_Context *ctx, int i)
{
//	if(ctx->ari_ct>ctx->ari_cte)
//		return;
	*ctx->ari_ct++=i;
}

void BTIC2C_BitArith_NormalizeOutput(BTIC2C_Context *ctx)
{
	while(!((ctx->ari_rmin^ctx->ari_rmax)>>24))
	{
		BTIC2C_BitArith_OutputByte(ctx, ctx->ari_rmin>>24);
		ctx->ari_rmin<<=8;
		ctx->ari_rmax<<=8;
		ctx->ari_rmax|=255;
	}
}

void BTIC2C_BitArith_NormalizeInput(BTIC2C_Context *ctx)
{
	while(!((ctx->ari_rmin^ctx->ari_rmax)>>24))
	{
		ctx->ari_rmin<<=8;
		ctx->ari_rmax=(ctx->ari_rmax<<8)|255;
		ctx->ari_rval=(ctx->ari_rval<<8)|
			BTIC2C_BitArith_InputByte(ctx);
	}

}

void BTIC2C_BitArith_FlushWBits(BTIC2C_Context *ctx)
{
	while((ctx->ari_rmin!=0x00000000U) ||
		(ctx->ari_rmax!=0xFFFFFFFFU))
	{
		BTIC2C_BitArith_OutputByte(ctx, ctx->ari_rmin>>24);
		ctx->ari_rmin<<=8;
		ctx->ari_rmax=(ctx->ari_rmax<<8)|255;
	}
}


void BTIC2C_BitArith_OutputFixedBit(BTIC2C_Context *ctx, int i)
{
	u32 r, r2, v;
	int j;

	r=ctx->ari_rmax-ctx->ari_rmin;
	v=ctx->ari_rmin+(r>>1);

	if(i)ctx->ari_rmin=v+1;
	else ctx->ari_rmax=v;
	BTIC2C_BitArith_NormalizeOutput(ctx);
}

int BTIC2C_BitArith_InputFixedBit(BTIC2C_Context *ctx)
{
	u32 r, r2, v, i;

	r=ctx->ari_rmax-ctx->ari_rmin;
	v=ctx->ari_rmin+(r>>1);

	i=(ctx->ari_rval>v);
	if(i)ctx->ari_rmin=v+1;
	else ctx->ari_rmax=v;

	BTIC2C_BitArith_NormalizeInput(ctx);
	return(i);
}

void BTIC2C_BitArith_OutputBit(BTIC2C_Context *ctx,
	int i, u32 w)
{
	u32 r, r2, v;
	int j;

	r=ctx->ari_rmax-ctx->ari_rmin;
	v=ctx->ari_rmin+(r>>8)*w;

	if(i)ctx->ari_rmin=v+1;
	else ctx->ari_rmax=v;

	BTIC2C_BitArith_NormalizeOutput(ctx);
}

int BTIC2C_BitArith_InputBit(BTIC2C_Context *ctx, u32 w)
{
	u32 r, r2, v, i;

	r=ctx->ari_rmax-ctx->ari_rmin;
	v=ctx->ari_rmin+(r>>8)*w;

	i=(ctx->ari_rval>v);
	if(i)ctx->ari_rmin=v+1;
	else ctx->ari_rmax=v;

	BTIC2C_BitArith_NormalizeInput(ctx);
	return(i);
}

void BTIC2C_BitArith_OutputModelBit(BTIC2C_Context *ctx,
	int i, byte *mctx)
{
	u32 r, v, w;
	int j;

	r=ctx->ari_rmax-ctx->ari_rmin; w=*mctx;
	v=ctx->ari_rmin+(r>>8)*w;

	if(i)
	{
		ctx->ari_rmin=v+1;
		w=btlza_weightadjust_1[w];
		*mctx=w;
	}else
	{
		ctx->ari_rmax=v;
		w=btlza_weightadjust_0[w];
		*mctx=w;
	}

	BTIC2C_BitArith_NormalizeOutput(ctx);
}

int BTIC2C_BitArith_InputModelBit(BTIC2C_Context *ctx, byte *mctx)
{
	u32 r, v, w;
	int i, j, k;

	r=ctx->ari_rmax-ctx->ari_rmin; w=*mctx;
	v=ctx->ari_rmin+(r>>8)*w;

	i=(ctx->ari_rval>v);
	if(i)
	{
		ctx->ari_rmin=v+1;
		w=btlza_weightadjust_1[w];
		*mctx=w;
	}else
	{
		ctx->ari_rmax=v;
		w=btlza_weightadjust_0[w];
		*mctx=w;
	}

	BTIC2C_BitArith_NormalizeInput(ctx);
	return(i);
}

void BTIC2C_BitArith_OutputFixedBits(BTIC2C_Context *ctx,
	int v, int n)
{
	int i, j, k, l;

	i=n;
	while(i--)
		{ BTIC2C_BitArith_OutputFixedBit(ctx, (v>>i)&1); }
}

int BTIC2C_BitArith_InputFixedBits(BTIC2C_Context *ctx, int n)
{
	int i, j, k, l;

	i=n; j=0;
	while(i--)
	{
		k=BTIC2C_BitArith_InputFixedBit(ctx);
		j=(j<<1)|k;
	}
	return(j);
}

int BTIC2C_BitArith_InputContextFixedBits(
	BTIC2C_Context *ctx, int n)
{
	int i, j, k, l;

	i=n; j=ctx->ari_wctx;
	while(i--)
	{
		k=BTIC2C_BitArith_InputFixedBit(ctx);
		j=(j<<1)|k;
	}
	ctx->ari_wctx=j;
	return(j&((1<<n)-1));
}

void BTIC2C_BitArith_OutputContextModelBits(BTIC2C_Context *ctx,
	int v, int n, byte *mdl, int ctxmask)
{
	int i, j, k, l;

	i=n; j=ctx->ari_wctx; k=ctxmask;
	while(i--)
	{
		l=(v>>i)&1;
		BTIC2C_BitArith_OutputModelBit(ctx, l,
			mdl+btic2c_arith_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->ari_wctx=j;
}

int BTIC2C_BitArith_InputContextModelBits(BTIC2C_Context *ctx,
	int n, byte *mdl, int ctxmask)
{
	int i, j, k, l;

	i=n; j=ctx->ari_wctx; k=ctxmask;
	while(i--)
	{
		l=BTIC2C_BitArith_InputModelBit(ctx,
			mdl+btic2c_arith_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->ari_wctx=j;
	return(j&((1<<n)-1));
}

void BTIC2C_BitArith_OutputExtraBits(BTIC2C_Context *ctx,
	int v, int n, byte *mdl, int ctxmask)
{
	switch(n)
	{
	case 0:
		break;
	case 1: case 2:
	case 3: case 4:
		BTIC2C_BitArith_OutputContextModelBits(ctx,
			v, n, mdl, ctxmask);
		break;
	default:
		BTIC2C_BitArith_OutputContextModelBits(ctx,
			v>>(n-4), 4, mdl, ctxmask);
		BTIC2C_BitArith_OutputFixedBits(ctx, v, n-4);
		break;
	}
}

int BTIC2C_BitArith_InputExtraBits(BTIC2C_Context *ctx,
	int n, byte *mdl, int ctxmask)
{
	int i, j;
	int v;

	switch(n)
	{
	case 0: v=0; break;
	case 1: case 2:
	case 3: case 4:
		v=BTIC2C_BitArith_InputContextModelBits(ctx,
			n, mdl, ctxmask);
		break;
	default:
		i=BTIC2C_BitArith_InputContextModelBits(ctx,
			4, mdl, ctxmask);
		j=BTIC2C_BitArith_InputFixedBits(ctx, n-4);
		v=(i<<(n-4))|j;
		break;
	}
	return(v);
}

int BTIC2C_BitArith_SetupEncode(BTIC2C_Context *ctx, byte *out, int sz)
{
	ctx->ari_ct=out;
	ctx->ari_cte=out+sz;
	ctx->ari_rmin=0x00000000U;
	ctx->ari_rmax=0xFFFFFFFFU;
	return(0);
}

int BTIC2C_BitArith_SetupDecode(BTIC2C_Context *ctx, byte *in, int sz)
{
	int i;

	ctx->ari_cs=in;
	ctx->ari_cse=in+sz;
	ctx->ari_rmin=0x00000000U;
	ctx->ari_rmax=0xFFFFFFFFU;
	ctx->ari_rval=0x00000000U;
	for(i=0; i<4; i++)
		ctx->ari_rval=(ctx->ari_rval<<8)|BTIC2C_BitArith_InputByte(ctx);
	return(0);
}

int BTIC2C_BitArith_SetupBeginEncode(BTIC2C_Context *ctx)
{
	ctx->ari_rmin=0x00000000U;
	ctx->ari_rmax=0xFFFFFFFFU;
	return(0);
}

int BTIC2C_BitArith_SetupBeginDecode(BTIC2C_Context *ctx)
{
	int i;

	if(ctx->ari_rmax)
	{
		return(0);
	}

	ctx->ari_rmin=0x00000000U;
	ctx->ari_rmax=0xFFFFFFFFU;
	ctx->ari_rval=0x00000000U;
	for(i=0; i<4; i++)
		ctx->ari_rval=(ctx->ari_rval<<8)|BTIC2C_BitArith_InputByte(ctx);
	return(0);
}

int BTIC2C_BitArith_SetupEndDecode(BTIC2C_Context *ctx)
{
	ctx->ari_rmin=0;
	ctx->ari_rmax=0;
	return(0);
}

int btic2c_arith_ctxindex(int ctx, int mask)
{
	int i;

	i=((ctx>>16)*251)>>8;
	return((ctx^(i<<16))&mask);
}

void BTIC2C_BitArith_EncodeSymbol8(BTIC2C_Context *ctx, int v,
	byte *mdl, int ctxmask)
{
	int i, j, k, l;

	i=8; j=ctx->ari_wctx; k=ctxmask;
	while(i--)
	{
		l=(v>>i)&1;
		BTIC2C_BitArith_OutputModelBit(ctx, l,
			mdl+btic2c_arith_ctxindex(j, k));
		j=(j<<1)|l;
	}
	ctx->ari_wctx=j;
}

int BTIC2C_BitArith_DecodeSymbol8(BTIC2C_Context *ctx,
	byte *mdl, int ctxmask)
{
	int i, j, k, l, v;

	j=ctx->ari_wctx; k=ctxmask;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	l=btic2c_arith_ctxindex(j, k);
	i=BTIC2C_BitArith_InputModelBit(ctx, mdl+l);
	j=(j<<1)|i;

	ctx->ari_wctx=j;
	return(j&0xFF);
}

void BTIC2C_BitArith_ResetContextWeights(
	BTIC2C_Context *ctx)
{
	int i, j;

	j=1<<ctx->ari_ctxbits;
	for(i=0; i<j; i++)
		ctx->ari_model[i]=128;
	ctx->ari_wctx=0;
}

void BTIC2C_BitArith_SetupContextBits(
	BTIC2C_Context *ctx, int bits)
{
	int i, j;

	BTIC2C_BitArith_Init();

	if(ctx->ari_model && (ctx->ari_ctxbits==bits))
	{
		BTIC2C_BitArith_ResetContextWeights(ctx);
		return;
	}

	if(ctx->ari_model && (ctx->ari_ctxbits!=bits))
		{ free(ctx->ari_model); ctx->ari_model=NULL; }
	
	j=1<<bits;
	if(!ctx->ari_model)
		{ ctx->ari_model=malloc(j*sizeof(byte)); }
	ctx->ari_ctxbits=bits;
	ctx->ari_ctxmask=(1<<bits)-1;
	ctx->ari_wctx=0;

	BTIC2C_BitArith_ResetContextWeights(ctx);
}

BGBBTJ_API int BTIC2C_BitArith_EncodeDataCtx(
	BTIC2C_Context *ctx,
	byte *ibuf, int isz, byte *obuf, int osz, int flags)
{
	int i, j, k, l, ll, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<0)return(-1);
	if(isz<=0)return(0);

	BTIC2C_BitArith_Init();

//	BTIC2C_BitArith_SetupContextBits(ctx,
//		BTIC2C_BitArith_BITARITH_CTXBITS);

	ct=obuf; cte=obuf+osz;
	le=flags&32; ll=flags&31;

	BTIC2C_BitArith_SetupContextBits(ctx, ll);

	//Emit Header
	*ct++=0xBB;		//magic byte
	*ct++=0xA1;		//magic byte
	ct=BGBBTJ_EmitVLI(ct, ((ll-8)&15)|(le?16:0));
	ct=BGBBTJ_EmitVLI(ct, isz);

	BTIC2C_BitArith_SetupEncode(ctx, ct, cte-ct);

	cs=ibuf;
	cse=ibuf+isz;
	i=0;

	while(cs<cse)
	{
		BTIC2C_BitArith_EncodeSymbol8(ctx, *cs++,
			ctx->ari_model, ctx->ari_ctxmask);
	}
	BTIC2C_BitArith_FlushWBits(ctx);

	i=ctx->ari_ct-obuf;
	return(i);
}

BGBBTJ_API int BTIC2C_BitArith_DecodeDataCtx(
	BTIC2C_Context *ctx,
	byte *ibuf, int isz, byte *obuf, int osz)
{
	int i, j, k, l, fl, ll, osz1, le;
	byte *cs, *ct, *cse, *cte, *s2;

	if(isz<=0)return(0);

	BTIC2C_BitArith_Init();

	cs=ibuf; cse=ibuf+isz;

	if((*cs++)!=0xBB)
		return(-1);
	if((*cs++)!=0xA1)
		return(-1);

	cs=BGBBTJ_DecodeVLI(cs, &fl);

	if(fl&(~31))
		return(-1);

	BTIC2C_BitArith_SetupContextBits(ctx, 8+(fl&15));
	le=(fl&16)?1:0;

	cs=BGBBTJ_DecodeVLI(cs, &osz1);
	if(osz1>osz)return(-1);

	BTIC2C_BitArith_SetupDecode(ctx, cs, cse-cs);

	ct=obuf;
	cte=obuf+osz1;
	i=0;

	while(ct<cte)
	{
		*ct++=BTIC2C_BitArith_DecodeSymbol8(ctx,
			ctx->ari_model, ctx->ari_ctxmask);
	}
	i=ct-obuf;
	return(i);
}
