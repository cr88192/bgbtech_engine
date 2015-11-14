#include <btgecm.h>

int BTGE_Noise_Rand(BTGE_NoiseContext *ctx)
{
	int i;

	ctx->seed=ctx->seed*4294967291LL+1;
	i=(ctx->seed>>32)&0xFFFFFF;
	return(i);
}

float BTGE_Noise_Random(BTGE_NoiseContext *ctx)
{
	int i;

	ctx->seed=ctx->seed*4294967291LL+1;
	i=(ctx->seed>>32)&0xFFFFFF;
	return(i/16777215.0);
}

float BTGE_Noise_SRandom(BTGE_NoiseContext *ctx)
{
	return(2*BTGE_Noise_Random(ctx)-1);
}

float BTGE_Noise_SqRandom(BTGE_NoiseContext *ctx)
{
	float f;
	f=BTGE_Noise_Random(ctx);
	return(f*f);
}

float BTGE_Noise_SSqRandom(BTGE_NoiseContext *ctx)
{
	float f;
	f=BTGE_Noise_Random(ctx);
	f=(f>=0)?(f*f):(-(f*f));
	return(f);
}

BTGE_NoiseContext *BTGE_Noise_CreateContext(
	int ord, int sz, s64 seed)
{
	BTGE_NoiseContext *tmp;
	float f, g;
	int i, j, k, l;
	
	//create context
	tmp=gctalloc("btge_noisectx_t", sizeof(BTGE_NoiseContext));
	tmp->ord=ord;
	tmp->sz=sz;
	tmp->seed=seed;

	//eat initial values
	for(i=0; i<sz; i++)
		BTGE_Noise_Random(tmp);

	//create arrays and fill them with random values
	for(i=0; i<ord; i++)
	{
		tmp->arr[i]=gcalloc(sz*sizeof(float));
		tmp->parr[i]=gcalloc(sz*sizeof(int));
		for(j=0; j<sz; j++)
		{
			tmp->arr[i][j]=BTGE_Noise_SRandom(tmp);
			tmp->parr[i][j]=j;
		}
	}

	//normalize values if applicable
	if(ord>1)
	{
		for(i=0; i<sz; i++)
		{
			g=0;
			for(j=0; j<ord; j++)
				{ f=tmp->arr[j][i]; g+=f*f; }
			f=sqrt(g);
			for(j=0; j<ord; j++)
				{ tmp->arr[j][i]/=f; }
		}
	}

	//randomize permutation arrays
	for(i=0; i<ord; i++)
	{
		for(j=0; j<sz; j++)
		{
			k=BTGE_Noise_Random(tmp)*sz;
			k=(k<0)?0:((k>sz)?sz:k);	//clamp range
		
			//swap elements
			l=tmp->parr[i][j];
			tmp->parr[i][j]=tmp->parr[i][k];
			tmp->parr[i][k]=l;
		}
	}
	
	return(tmp);
}

float BTGE_Noise_GenValue(BTGE_NoiseContext *ctx, float *vec)
{
	int b0[4], b1[4];
	int b2[4], b3[4];
	float r0[4], r1[4];
	float sx[4], sy[4];
	
	float f, g, h;
	int i, j, k, l, hi;
	
	hi=0;
	for(i=0; i<ctx->ord; i++)
	{
		f=vec[i]+4096;
		hi=(hi*251)+((int)f);
//		b0[i]=((int)(f+0))%ctx->sz;
//		b1[i]=((int)(f+1))%ctx->sz;
		b0[i]=((int)(f+0))&(ctx->sz-1);
		b1[i]=((int)(f+1))&(ctx->sz-1);
		r0[i]=f-(int)f;
		r1[i]=r0[i]-1;
	}
	hi=hi*251;

	for(i=0; i<ctx->ord; i++)
	{
		j=(i+1)%ctx->ord;
//		b2[i]=ctx->parr[i][b0[i]];
//		b3[i]=ctx->parr[i][b1[i]];
		k=(hi>>8)%ctx->sz;
//		b2[i]=ctx->parr[j][ctx->parr[i][b0[i]]]^k;
//		b3[i]=ctx->parr[j][ctx->parr[i][b1[i]]]^k;

		k=ctx->parr[i][b0[i]];
		l=ctx->parr[i][b1[i]];
		b2[i]=ctx->parr[j][(k+b1[j])%ctx->sz];
		b3[i]=ctx->parr[j][(l+b0[j])%ctx->sz];

//		b2[i]=ctx->parr[j][ctx->parr[i][b0[i]]];
//		b3[i]=ctx->parr[j][ctx->parr[i][b1[i]]];
	}

	for(i=0; i<ctx->ord; i++)
	{
		g=r0[i];
		sx[i]=g*g*(3-2*g);
		
//		f=r0[i]*ctx->arr[i][ctx->parr[i][b0[i]]];
//		g=r1[i]*ctx->arr[i][ctx->parr[i][b1[i]]];

		f=r0[i]*ctx->arr[i][b2[i]];
		g=r1[i]*ctx->arr[i][b3[i]];
		h=f*(1-sx[i])+(g*sx[i]);
		sy[i]=h;
	}

	f=0;
	for(i=0; i<ctx->ord; i++)
		f+=sy[i];
	f/=ctx->ord;
	return(f);
}

float btge_clamp(float x, float m, float n)
{
	float y;
	y=(x<m)?m:((x<=n)?x:n);
	return(y);
}

float btge_lerp(float a, float b, float t)
	{ return(a*(1-t)+(b*t)); }
float btge_scurve(float a)
	{ return(a*a*(3-2*a)); }

int btge_clampi(int x, int m, int n)
{
	int y;
	y=(x<m)?m:((x<=n)?x:n);
	return(y);
}

float btge_noise2(BTGE_NoiseContext *ctx, float *vec)
{
	int b0[4], b1[4];
	int b2[4], b3[4];
	float r0[4], r1[4];
	float sx[4], sy[4];
	int b00, b10, b01, b11;
	float a, b, t, u, v;
	float f, g, h;
	int i, j, k, l, hi;
	
	for(i=0; i<ctx->ord; i++)
	{
		f=vec[i]+4096;
//		b0[i]=((int)(f+0))%ctx->sz;
//		b1[i]=((int)(f+1))%ctx->sz;
		b0[i]=((int)(f+0))&(ctx->sz-1);
		b1[i]=((int)(f+1))&(ctx->sz-1);
		r0[i]=f-(int)f;
		r1[i]=r0[i]-1;
		sx[i] = btge_scurve(r0[i]);
	}

	i = ctx->parr[0][b0[0]];
	j = ctx->parr[0][b1[0]];

	b00 = ctx->parr[0][(i+b0[1])%ctx->sz];
	b10 = ctx->parr[0][(j+b0[1])%ctx->sz];
	b01 = ctx->parr[0][(i+b1[1])%ctx->sz];
	b11 = ctx->parr[0][(j+b1[1])%ctx->sz];

	u=r0[0]*ctx->arr[0][b00] + r0[1]*ctx->arr[1][b00];
	v=r1[0]*ctx->arr[0][b10] + r0[1]*ctx->arr[1][b10];
	a=btge_lerp(u, v, sx[0]);

	u=r0[0]*ctx->arr[0][b01] + r1[1]*ctx->arr[1][b01];
	v=r1[0]*ctx->arr[0][b11] + r1[1]*ctx->arr[1][b11];
	b=btge_lerp(u, v, sx[0]);

	return(btge_lerp(a, b, sx[1]));
}

float btge_noise3(BTGE_NoiseContext *ctx, float *vec)
{
	int b0[4], b1[4];
	int b2[4], b3[4];
	float r0[4], r1[4];
	float sx[4], sy[4];
	int b00, b10, b01, b11;
	int b000, b100, b010, b110;
	int b001, b101, b011, b111;
	float a, b, c, d, t, u, v;
	float f, g, h;
	int i, j, k, l, hi, mi;
	
	for(i=0; i<ctx->ord; i++)
	{
		f=vec[i]+4096;
//		b0[i]=((int)(f+0))%ctx->sz;
//		b1[i]=((int)(f+1))%ctx->sz;
		b0[i]=((int)(f+0))&(ctx->sz-1);
		b1[i]=((int)(f+1))&(ctx->sz-1);
		r0[i]=f-(int)f;
		r1[i]=r0[i]-1;
		sx[i] = btge_scurve(r0[i]);
	}

	i = ctx->parr[0][b0[0]];
	j = ctx->parr[0][b1[0]];

#if 0
	b00 = ctx->parr[0][(i+b0[1])%ctx->sz];
	b10 = ctx->parr[0][(j+b0[1])%ctx->sz];
	b01 = ctx->parr[0][(i+b1[1])%ctx->sz];
	b11 = ctx->parr[0][(j+b1[1])%ctx->sz];

	b000 = (b00+b0[2])%ctx->sz;
	b100 = (b10+b0[2])%ctx->sz;
	b010 = (b01+b0[2])%ctx->sz;
	b110 = (b11+b0[2])%ctx->sz;
	b001 = (b00+b1[2])%ctx->sz;
	b101 = (b10+b1[2])%ctx->sz;
	b011 = (b01+b1[2])%ctx->sz;
	b111 = (b11+b1[2])%ctx->sz;
#endif

#if 1
	mi=ctx->sz-1;

	b00 = ctx->parr[0][(i+b0[1])&mi];
	b10 = ctx->parr[0][(j+b0[1])&mi];
	b01 = ctx->parr[0][(i+b1[1])&mi];
	b11 = ctx->parr[0][(j+b1[1])&mi];

	b000 = (b00+b0[2])&mi;
	b100 = (b10+b0[2])&mi;
	b010 = (b01+b0[2])&mi;
	b110 = (b11+b0[2])&mi;
	b001 = (b00+b1[2])&mi;
	b101 = (b10+b1[2])&mi;
	b011 = (b01+b1[2])&mi;
	b111 = (b11+b1[2])&mi;
#endif

	u=  r0[0]*ctx->arr[0][b000] +
		r0[1]*ctx->arr[1][b000] +
		r0[2]*ctx->arr[2][b000];
	v=  r1[0]*ctx->arr[0][b100] +
		r0[1]*ctx->arr[1][b100] +
		r0[2]*ctx->arr[2][b100];
	a = btge_lerp(u, v, sx[0]);

	u=  r0[0]*ctx->arr[0][b010] +
		r1[1]*ctx->arr[1][b010] +
		r0[2]*ctx->arr[2][b010];
	v=  r1[0]*ctx->arr[0][b110] +
		r1[1]*ctx->arr[1][b110] +
		r0[2]*ctx->arr[2][b110];
	b = btge_lerp(u, v, sx[0]);

	c = btge_lerp(a, b, sx[1]);

	u=  r0[0]*ctx->arr[0][b001] +
		r0[1]*ctx->arr[1][b001] +
		r1[2]*ctx->arr[2][b001];
	v=  r1[0]*ctx->arr[0][b101] +
		r0[1]*ctx->arr[1][b101] +
		r1[2]*ctx->arr[2][b101];
	a = btge_lerp(u, v, sx[0]);

	u=  r0[0]*ctx->arr[0][b011] +
		r1[1]*ctx->arr[1][b011] +
		r1[2]*ctx->arr[2][b011];
	v=  r1[0]*ctx->arr[0][b111] +
		r1[1]*ctx->arr[1][b111] +
		r1[2]*ctx->arr[2][b111];
	b = btge_lerp(u, v, sx[0]);

	d = btge_lerp(a, b, sx[1]);
	return(btge_lerp(c, d, sx[2]));
}

float BTGE_Noise_GenValue1(BTGE_NoiseContext *ctx, float x)
{
	float tv[4];
	tv[0]=x; tv[1]=0; tv[2]=0; tv[3]=0;
	return(BTGE_Noise_GenValue(ctx, tv));
}

float BTGE_Noise_GenValue2(BTGE_NoiseContext *ctx, float x, float y)
{
	float tv[4];
	tv[0]=x; tv[1]=y; tv[2]=0; tv[3]=0;
//	return(BTGE_Noise_GenValue(ctx, tv));
	return(btge_noise2(ctx, tv));
}

float BTGE_Noise_GenValue3(BTGE_NoiseContext *ctx,
	float x, float y, float z)
{
	float tv[4];
	tv[0]=x; tv[1]=y; tv[2]=z; tv[3]=0;
//	return(BTGE_Noise_GenValue(ctx, tv));
	return(btge_noise3(ctx, tv));
}
