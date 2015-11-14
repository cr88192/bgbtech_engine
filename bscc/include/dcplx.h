#ifndef DCOMPLEX_H
#define DCOMPLEX_H

#include <math.h>


#ifndef FCOMPLEX_T
#define FCOMPLEX_T
typedef struct {float r, i;} fcomplex;
#endif

#ifndef DCOMPLEX_T
#define DCOMPLEX_T
typedef struct {double r, i;} dcomplex;
#endif

#define dcomplex(r,i)	_dcomplex(r,i)

static dcomplex _dcomplex(float r, float i)
	{ dcomplex t; t.r=r; t.i=r; return(t); };

static dcomplex _cplxvf(float *x)
	{ return(_dcomplex(x[0], x[1])); };

static float dcreal(dcomplex a)	{ return(a.r); };
static float dcimag(dcomplex a)	{ return(a.i); };

static float dcdot(dcomplex a, dcomplex b)
	{ return(a.r*b.r+a.i*b.i); }
static float dccross(dcomplex a, dcomplex b)
	{ return(a.r*b.i-a.i*b.r); }

static float dclen(dcomplex a)
	{ return(sqrt(a.r*a.r+a.i*a.i)); }
static float dclen2(dcomplex a)
	{ return(a.r*a.r+a.i*a.i); }

static dcomplex dcnorm(dcomplex a)
{	dcomplex c; float l;
	l=dclen(a); if(l!=0)l=1.0/l;
	c.r=a.r*l; c.i=a.i*l;
	return(c); }

static dcomplex dcneg(dcomplex a)
	{ dcomplex c; c.r=-a.r; c.i=-a.i; return(c); }

static dcomplex dcadd(dcomplex a, dcomplex b)
	{ dcomplex c; c.r=a.r+b.r; c.i=a.i+b.i; return(c); }
static dcomplex dcsub(dcomplex a, dcomplex b)
	{ dcomplex c; c.r=a.r-b.r; c.i=a.i-b.i; return(c); }

static dcomplex dcmul(dcomplex a, dcomplex b)
	{ dcomplex c; c.r=a.r*b.r-a.i*b.i; c.i=a.r*b.i+a.i*b.r; return(c); }

static dcomplex dcrcp(dcomplex a)
{
	float f; f=a.r*a.r+a.i*a.i;
	return(dcomplex(a.r/f, -a.i/f));
}

static dcomplex dcdiv(dcomplex a, dcomplex b)
	{ dcomplex c; c=dcmul(a, dcrcp(b)); return(c); }

static float dcdist(dcomplex a, dcomplex b)
	{ return(dclen(dcsub(a, b))); }
static dcomplex dcscale(dcomplex a, float f)
	{ dcomplex c; c.r=a.r*f; c.i=a.i*f; return(c); }


static int dceqp(dcomplex a, dcomplex b)
	{ return((a.r==b.r) && (a.i==b.i)); }
static int dcneqp(dcomplex a, dcomplex b)
	{ return((a.r!=b.r) || (a.i!=b.i)); }

static float dcabs(dcomplex a)
	{ return(sqrt(a.r*a.r + a.i*a.i)); }
static float dcarg(dcomplex a);
	{ return(atan2(a.i, a.r)); }
static dcomplex dconj(dcomplex a)
	{ return(dcomplex(a.r, -a.i)); }

static dcomplex dcsqrt(dcomplex z)
{
	dcomplex c;
	float x,y,w,r;

	if((z.r==0.0) && (z.i==0.0))
	{
		c.r=0; c.i=0;
	}else
	{
		x=fabs(z.r);
		y=fabs(z.i);
		if(x>=y)
		{
			r=y/x;
			w=sqrt(x)*sqrt(0.5*(1.0+sqrt(1.0+r*r)));
		} else
		{
			r=x/y;
			w=sqrt(y)*sqrt(0.5*(r+sqrt(1.0+r*r)));
		}
		if(z.r>=0.0)
		{
			c.r=w;
			c.i=z.i/(2.0*w);
		} else
		{
			c.i=(z.i >= 0)?w:-w;
			c.r=z.i/(2.0*c.i);
		}
	}
	return(c);
}

static dcomplex dclog(dcomplex a)
	{ return(dcomplex(0.5*log(dclen2(a)), dcarg(a))); }

static dcomplex dcpow2(dcomplex a)
	{ dcomplex c; c.r=a.r*a.r-a.i*a.i; c.i=2*a.r*a.i; return(c); }

static dcomplex dcpow3(dcomplex a)
{
	dcomplex b, c;
	b.r=a.r*a.r-a.i*a.i; b.i=2*a.r*a.i;
	c.r=a.r*b.r-a.i*b.i; c.i=a.r*b.i+a.i*b.r;
	return(c);
}

static dcomplex dcpow4(dcomplex a)
	{ return(dcmul(dcpow2(a), dcpow2(a))); }
static dcomplex dcpow5(dcomplex a)
	{ return(dcmul(dcpow2(a), dcpow3(a))); }
static dcomplex dcpow6(dcomplex a)
	{ return(dcmul(dcpow3(a), dcpow3(a))); }

static dcomplex dcaddscale(dcomplex a, dcomplex b, float v)
	{ dcomplex c; c.r=a.r+(b.r*v); c.i=a.i+(b.i*v); return(c); }

static dcomplex dcexp(dcomplex a)
{
	dcomplex b, x;

	b=dcomplex(1.0+a.r, a.i); x=dcpow2(a);
	b=dcaddscale(b, x, 1.0/        2.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/        6.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/       24.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/      120.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/      720.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/     5040.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/    40320.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/   362880.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/  3628800.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/ 39916800.0); x=dcmul(x, a);
	b=dcaddscale(b, x, 1.0/479001600.0); x=dcmul(x, a);
	return(b);
}

static dcomplex dcpow(dcomplex a, dcomplex b);
	{ dcomplex c; c=dcexp(dcmul(dclog(a), b)); return(c); }

// static dcomplex dcproj(dcomplex a);

static dcomplex dcacos(dcomplex a);
static dcomplex dcasin(dcomplex a);
static dcomplex dcacosh(dcomplex a);
static dcomplex dcasinh(dcomplex a);

static dcomplex dcatan(dcomplex a);
static dcomplex dcatanh(dcomplex a);
static dcomplex dccos(dcomplex a);
static dcomplex dccosh(dcomplex a);
static dcomplex dcsin(dcomplex a);
static dcomplex dcsinh(dcomplex a);
static dcomplex dctan(dcomplex a);
static dcomplex dctanh(dcomplex a);

#endif
