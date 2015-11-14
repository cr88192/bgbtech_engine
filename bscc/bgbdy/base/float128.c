/*
Float128

112 bits, mantissa.
15 bit exponent.
1 sign bit.

Bias=16383
*/

#include <bgbdy.h>

static fv128_t fv128_const_zero=
	{0x00000000,0x00000000,0x00000000,0x00000000};
static fv128_t fv128_const_one=
	{0x00000000,0x00000000,0x00000000,0x3FFF0000};
static fv128_t fv128_const_two=
	{0x00000000,0x00000000,0x00000000,0x40000000};
static fv128_t fv128_const_inf=
	{0x00000000,0x00000000,0x00000000,0x7FFF0000};

BGBDY_API dyt dyfloat128(fv128_t v)
{
	dyt t;
	t=gctatomic("_float128_t", sizeof(fv128_t));
	*(fv128_t *)t=v;
	return(t);
}

BGBDY_API dyt dyfloat128d(double v)
{
	return(dyfloat128(fv128_fromDouble(v)));
}

BGBDY_API dyt dyfloat128str(char *str)
{
	return(dyfloat128(fv128_atof(str)));
}

BGBDY_API int dyfloat128p(dyt p)
{
	return(dytypep(p, "_float128_t"));
}

BGBDY_API fv128_t dyfloat128v(dyt v)
{
	if(dytypep(v, "_float128_t"))
	{
		return(*(fv128_t *)v);
	}
	return(fv128_fromDouble(dyrealv(v)));
}

BGBDY_API double dyfloat128dv(dyt v)
{
	if(dytypep(v, "_float128_t"))
	{
		return(fv128_toDouble(*(fv128_t *)v));
	}
	return(dyrealv(v));
}

fv128_t fv128_atof(char *str)
{
	return(fv128_fromDouble(atof(str)));
}

char *fv128_ftoa(fv128_t val)
{
	static char tmp[128];
	sprintf(tmp, "%.16f", fv128_toDouble(val));
	return(tmp);
}

int fv128_getExponent(fv128_t val)
{
	return(((int)((val.d>>16)&32767))-16383);
}

void fv128_setExponent(fv128_t *val, int i)
{
	int j;
	
	j=i+16383;
	if(j<0)j=0;
	if(j>32767)j=32767;
	val->d=(val->d&0x8000FFFF)|(j<<16);
}

int fv128_getSign(fv128_t val)
{
	return((val.d>>31)&1);
}

void fv128_setSign(fv128_t *val, int i)
{
	if(i)	{ val->d|=0x80000000; }
	else	{ val->d&=0x7FFFFFFF; }
}

iv128_t fv128_getMantissa(fv128_t val)
{
	iv128_t t;
	t.a=val.a; t.b=val.b;
	t.c=val.c;
	t.d=(val.d&0xFFFF);
	if(val.d&0x7FFF0000)
		t.d|=0x00010000;
	return(t);
}

void fv128_setMantissa(fv128_t *val, iv128_t fr)
{
	val->a=fr.a;
	val->b=fr.b;
	val->c=fr.c;
	val->d=(val->d&0xFFFF0000) | fr.d&0x0000FFFF;
}

fv128_t fv128_neg(fv128_t a)
{
	fv128_t c;
	
	c.a=a.a; c.b=a.b; c.c=a.c; c.d=a.d;
	c.d^=0x80000000;
	return(c);
}

fv128_t fv128_abs(fv128_t a)
{
	fv128_t c;
	c.a=a.a; c.b=a.b; c.c=a.c;
	c.d=a.d&0x7FFFFFFF;
	return(c);
}

fv128_t fv128_pow2x(fv128_t a, int x)
{
	fv128_t c;
	int i;
	
	if(!x)return(a);
	
	c.a=a.a; c.b=a.b; c.c=a.c;
	c.d=a.d&0x8000FFFF;
	i=(a.d>>16)&0x7FFF;
	if((i+x)<0)return(fv128_const_zero);
	if((i+x)>=32767)return(fv128_const_inf);
	c.d|=(i+x)<<16;
	return(c);
}

double fv128_toDouble(fv128_t a)
{
	u64 f;
	int i;
	
	f=(((u64)(a.d&0x0000FFFF))<<36)|(((u64)a.c)<<4)|(a.b>>28);
	i=fv128_getExponent(a)+1023;
	if(i<0)i=0;
	if(i>=2047) { i=2047; f=0; }
	f|=((u64)i)<<52;
	if((a.d>>31)&1)f|=(1ULL<<63);

//	printf("fv128_toDouble: %08X'%08X'%08X'%08X -> %08X'%08X\n",
//		a.d, a.c, a.b, a.a, (u32)(f>>32), (u32)f);

	return(*(double *)(&f));
}

fv128_t fv128_fromDouble(double val)
{
	fv128_t c;
	u64 v, w;
	int i, sg;

	*(double *)(&v)=val;
	sg=(v>>63)&1;

	if(!(v&0x7FFFFFFFFFFFFFFFULL))
	{
		c=fv128_const_zero;
		if(sg)c.d|=0x80000000;
		return(c);
	}

	i=((int)((v>>52)&2047))-1023;
	w=v&0x000FFFFFFFFFFFFFULL;

	if(i<=(-1023))
	{
		while(!(w&0x0010000000000000ULL))
			{ i--; w<<=1; }
	}
	
	c.a=0;
	c.b=(u32)(w<<28);
	c.c=(u32)(w>>4);
	c.d=((u32)(w>>36))&0x0000FFFF;
	c.d|=(i+16383)<<16;
	if(sg)c.d|=0x80000000;

//	printf("fv128_fromDouble: %08X'%08X -> %08X'%08X'%08X'%08X\n",
//		(u32)(v>>32), (u32)v,
//		c.d, c.c, c.b, c.a);

	return(c);
}

int fv128_zerop(fv128_t a)
{
	if(a.d || a.a || a.b || a.c)
		return(0);
	return(1);
}

int fv128_eqp(fv128_t a, fv128_t b)
{
	return((a.d==b.d) && (a.a==b.a) && (a.b==b.b) && (a.c==b.c));
}

int fv128_eq1p(fv128_t a)
{
	return((a.d==0x3FFF) && !a.a && !a.b && !a.c);
}

int fv128_cmp(fv128_t a, fv128_t b)
{
	fv128_t c;

	if(fv128_eqp(a, b))
		return(0);
	c=fv128_sub(a, b);
	if(fv128_zerop(c))
		return(0);
	if(fv128_getSign(c))
		return(-1);
	return(1);
}

int fv128_gtp(fv128_t a, fv128_t b)
	{ return(fv128_cmp(a, b)>0); }
int fv128_ltp(fv128_t a, fv128_t b)
	{ return(fv128_cmp(a, b)<0); }
int fv128_gep(fv128_t a, fv128_t b)
	{ return(fv128_cmp(a, b)>=0); }
int fv128_lep(fv128_t a, fv128_t b)
	{ return(fv128_cmp(a, b)<=0); }

int fv128_ge1p(fv128_t a)
{
	return(fv128_gep(a, fv128_const_one));
}

fv128_t fv128_add(fv128_t a, fv128_t b)
{
	iv128_t fr0, fr1, fr2, fr3;
	fv128_t c;
	int e0, e1, e2, s0, s1, s2;
	int i;

	e0=fv128_getExponent(a);
	e1=fv128_getExponent(b);
	
	if(e1>e0)return(fv128_add(b, a));
	
	i=e0-e1;
	if(i>110)return(a);
	
	s0=fv128_getSign(a);
	s1=fv128_getSign(b);
	fr0=fv128_getMantissa(a);
	fr1=fv128_getMantissa(b);
	fr2=iv128_shr(fr1, i);
	
	if(s0==s1)
	{
		s2=s0; e2=e0;
		fr3=iv128_add(fr0, fr2);
		while(fr3.d&0xFFFE0000)
			{ fr3=iv128_shr(fr3, 1); e2++; }
	}else
	{
		s2=s0; e2=e0;
		fr3=iv128_sub(fr0, fr2);
		if(iv128_negativep(fr3))
			{ fr3=iv128_neg(fr3); s2=!s2; }
		while(!(fr3.d&0x00010000) && ((e2+16383)>0))
			{ fr3=iv128_shl(fr3, 1); e2--; }
	}
	
	//check for infinite
	if(e2>=16384)
	{
		c.a=0; c.b=0; c.c=0;
		c.d=0x7FFF0000;
		if(s2)c.d|=0x80000000;
		return(c);
	}
	
	c.a=fr3.a;
	c.b=fr3.b;
	c.c=fr3.c;
	c.d=fr3.d&0x0000FFFF;
	c.d|=(e2+16383)<<16;
	if(s2)c.d|=0x80000000;
	return(c);
}

fv128_t fv128_sub(fv128_t a, fv128_t b)
	{ return(fv128_add(a, fv128_neg(b))); }

int fv128_infp(fv128_t a)
{
	if((a.d&0x7FFF)==0x7FFF)
	{
		if(a.a || a.b || a.c || (a.d&0x0000FFFF))
			return(0);
		return(1);
	}
	return(0);
}

int fv128_nanp(fv128_t a)
{
	if((a.d&0x7FFF)==0x7FFF)
	{
		if(a.a || a.b || a.c || (a.d&0x0000FFFF))
			return(1);
		return(0);
	}
	return(0);
}

fv128_t fv128_mul(fv128_t a, fv128_t b)
{
	iv128_t fr0, fr1, fr2, fr3;
	iv256_t frx;
	fv128_t c;
	int e0, e1, e2, s0, s1, s2;
	int i;

	e0=fv128_getExponent(a);
	e1=fv128_getExponent(b);
	
	s0=fv128_getSign(a);
	s1=fv128_getSign(b);
	fr0=fv128_getMantissa(a);
	fr1=fv128_getMantissa(b);

	//Inf and NaN
	if((e0>=16384) || (e1>=16384))
	{
		s2=s0^s1;

		//(x*NaN)|(NaN*x)=NaN
		if(fv128_nanp(a) || fv128_nanp(b))
		{
			c.a=0; c.b=0; c.c=0;
			c.d=0x7FFF8000;
			if(s2)c.d|=0x80000000;
			return(c);
		}

		//Inf*Inf=Inf
		if(fv128_infp(a) && fv128_infp(b))
		{
			c.a=0; c.b=0; c.c=0;
			c.d=0x7FFF0000;
			if(s2)c.d|=0x80000000;
			return(c);
		}

		//(0*Inf)|(Inf*0)=NaN
		if(fv128_zerop(a) || fv128_zerop(b))
		{
			c.a=0; c.b=0; c.c=0;
			c.d=0x7FFF8000;
			if(s2)c.d|=0x80000000;
			return(c);
		}

		//else, Inf
		c.a=0; c.b=0; c.c=0;
		c.d=0x7FFF0000;
		if(s2)c.d|=0x80000000;
		return(c);
	}

	//special case for 0's
	if(iv128_zerop(fr0) || iv128_zerop(fr1))
	{
		s2=s0^s1;

		c.a=0; c.b=0; c.c=0; c.d=0;
		if(s2)c.d|=0x80000000;
		return(c);
	}

	//fixed-point multiply of fraction
	frx=iv128_mulw(fr0, fr1);
	frx=iv256_shr(frx, 112);
	fr2.a=frx.a; fr2.b=frx.b;
	fr2.c=frx.c; fr2.d=frx.d;

	s2=s0^s1;
	e2=e0+e1;
	
	//quick handle if result is 0
	if(iv128_zerop(fr2) || ((e2+16383)<=(-112)))
	{
		c.a=0; c.b=0; c.c=0; c.d=0;
		if(s2)c.d|=0x80000000;
		return(c);
	}
	
	//overflow to Inf
	if(e2>=16384)
	{
		c.a=0; c.b=0; c.c=0;
		c.d=0x7FFF0000;
		if(s2)c.d|=0x80000000;
		return(c);
	}
	
	//try to normalize result
	while(fr2.d&0xFFFE0000)
		{ fr2=iv128_shr(fr2, 1); e2++; }
	while(!(fr2.d&0x00010000) && ((e2+16383)>0))
		{ fr2=iv128_shl(fr2, 1); e2--; }

	c.a=fr2.a; c.b=fr2.b; c.c=fr2.c;
	c.d=fr2.d&0x0000FFFF;
	c.d|=(e2+16383)<<16;
	if(s2)c.d|=0x80000000;
	return(c);
}

#if 0
fv128_t fv128_rcp(fv128_t a)
{
	iv128_t fr0, fr1, fr2, fr3, frx;
	fv128_t c;
	int e0, e1, e2, s0, s1, s2;
	int i;

	s0=fv128_getSign(a);
	e0=fv128_getExponent(a);
	fr0=fv128_getMantissa(a);

	return(c);
}
#endif

fv128_t fv128_rcp(fv128_t a)
{
	fv128_t f, g, xo, xn;
	int i;

	if(fv128_eq1p(a))
		return(a);

	if(fv128_getSign(a))
	{
		xn=fv128_neg(a);
		f=fv128_rcp(xn);
		g=fv128_neg(f);
		return(g);
	}

//	printf("fv128_rcp: %08X'%08X'%08X'%08X\n",
//		a.d, a.c, a.b, a.a);

	i=fv128_getExponent(a);
	xo=fv128_pow2x(fv128_const_one, -i);

//	printf("fv128_rcp: guess=%08X'%08X'%08X'%08X 2^%d\n",
//		xo.d, xo.c, xo.b, xo.a, -i);

	for(i=0; i<100; i++)
	{
		f=fv128_mul(a, xo);
		g=fv128_sub(fv128_const_two, f);
		xn=fv128_mul(xo, g);

#if 0
		printf("fv128_rcp:  f=%08X'%08X'%08X'%08X\n",
			f.d, f.c, f.b, f.a);
		printf("fv128_rcp:  g=%08X'%08X'%08X'%08X\n",
			g.d, g.c, g.b, g.a);
		printf("fv128_rcp: xo=%08X'%08X'%08X'%08X\n",
			xo.d, xo.c, xo.b, xo.a);
		printf("fv128_rcp: xn=%08X'%08X'%08X'%08X\n",
			xn.d, xn.c, xn.b, xn.a);
#endif

		if(fv128_eqp(xo, xn))
			return(xn);
		xo=xn;
	}
	return(xn);
}

fv128_t fv128_div(fv128_t a, fv128_t b)
{
	return(fv128_mul(a, fv128_rcp(b)));
}

fv128_t fv128_mod(fv128_t a, fv128_t b)
{
	fv128_t t, x, y;
	
	t=fv128_div(a, b);
	t=fv128_floor(t);
	x=fv128_mul(b, t);
	y=fv128_sub(a, x);
	return(y);
//	return(fv128_mul(a, fv128_rcp(b)));
}

fv128_t fv128_pow(fv128_t a, fv128_t b)
{
	fv128_t t;
	double f, g;

	f=fv128_toDouble(a);
	g=fv128_toDouble(b);
	t=fv128_fromDouble(pow(f, g));
	return(t);
}

fv128_t fv128_floor(fv128_t a)
{
	fv128_t t;
	double f;
	f=fv128_toDouble(a);
	t=fv128_fromDouble(floor(f));
	return(t);
}

fv128_t fv128_scale(fv128_t a, double b)
{
	return(fv128_mul(a, fv128_fromDouble(b)));
}


fv128_t fv128_sin(fv128_t q)
{
	fv128_t qa, qs, qt;

	qs=fv128_mul(q, q);
	qa=q; qt=fv128_mul(q, qs);

	qa=fv128_add(qa, fv128_scale(qt, -1.0/         6.0));
	qt=fv128_mul(qt, qs);
	qa=fv128_add(qa, fv128_scale(qt,  1.0/       120.0));
	qt=fv128_mul(qt, qs);
	qa=fv128_add(qa, fv128_scale(qt, -1.0/      5040.0));
	qt=fv128_mul(qt, qs);
	qa=fv128_add(qa, fv128_scale(qt,  1.0/    362880.0));
	qt=fv128_mul(qt, qs);
	qa=fv128_add(qa, fv128_scale(qt, -1.0/  39916800.0));
	qt=fv128_mul(qt, qs);
	qa=fv128_add(qa, fv128_scale(qt,  1.0/6227020800.0));
	qt=fv128_mul(qt, qs);

	return(qa);
}

fv128_t fv128_cos(fv128_t q)
{
	fv128_t qa, qs, qt;

	qs=fv128_mul(q, q);
	qa=fv128_const_one; qt=qs;

	qa=fv128_add(qa, fv128_scale(qt, -1.0/        2.0));
	qt=fv128_mul(qt, q);
	qa=fv128_add(qa, fv128_scale(qt,  1.0/       24.0));
	qt=fv128_mul(qt, q);
	qa=fv128_add(qa, fv128_scale(qt, -1.0/      720.0));
	qt=fv128_mul(qt, q);
	qa=fv128_add(qa, fv128_scale(qt,  1.0/    40320.0));
	qt=fv128_mul(qt, q);
	qa=fv128_add(qa, fv128_scale(qt, -1.0/  3628800.0));
	qt=fv128_mul(qt, q);
	qa=fv128_add(qa, fv128_scale(qt,  1.0/479001600.0));
	qt=fv128_mul(qt, q);

	return(qa);
}

fv128_t fv128_tan(fv128_t q)
{
	return(fv128_div(fv128_sin(q), fv128_cos(q)));
}

fv128_t fv128_cot(fv128_t q)
{
	return(fv128_div(fv128_cos(q), fv128_sin(q)));
}
