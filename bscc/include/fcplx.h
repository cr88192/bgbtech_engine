#ifndef FCOMPLEX_H
#define FCOMPLEX_H

#include <math.h>


#ifndef FCOMPLEX_T
#define FCOMPLEX_T
typedef struct {float r, i;} fcomplex;
#endif

#ifndef DCOMPLEX_T
#define DCOMPLEX_T
typedef struct {double r, i;} dcomplex;
#endif

#define fcomplex(r,i)	_fcomplex(r,i)

static fcomplex _fcomplex(float r, float i)
	{ fcomplex t; t.r=r; t.i=r; return(t); }

static fcomplex _cplxvf(float *x)
	{ return(_fcomplex(x[0], x[1])); };

static float fcreal(fcomplex a)	{ return(a.r); }
static float fcimag(fcomplex a)	{ return(a.i); }

static float fcdot(fcomplex a, fcomplex b)
	{ return(a.r*b.r+a.i*b.i); }
static float fccross(fcomplex a, fcomplex b)
	{ return(a.r*b.i-a.i*b.r); }

static float fclen(fcomplex a)
	{ return(sqrt(a.r*a.r+a.i*a.i)); }
static float fclen2(fcomplex a)
	{ return(a.r*a.r+a.i*a.i); }

static fcomplex fcnorm(fcomplex a)
{	fcomplex c; float l;
	l=fclen(a); if(l!=0)l=1.0/l;
	c.r=a.r*l; c.i=a.i*l;
	return(c); }

static fcomplex fcneg(fcomplex a)
	{ fcomplex c; c.r=-a.r; c.i=-a.i; return(c); }

static fcomplex fcadd(fcomplex a, fcomplex b)
	{ fcomplex c; c.r=a.r+b.r; c.i=a.i+b.i; return(c); }
static fcomplex fcsub(fcomplex a, fcomplex b)
	{ fcomplex c; c.r=a.r-b.r; c.i=a.i-b.i; return(c); }

static fcomplex fcmul(fcomplex a, fcomplex b)
	{ fcomplex c; c.r=a.r*b.r-a.i*b.i; c.i=a.r*b.i+a.i*b.r; return(c); }

static fcomplex fcrcp(fcomplex a)
{
	float f; f=a.r*a.r+a.i*a.i;
	return(fcomplex(a.r/f, -a.i/f));
}

static fcomplex fcdiv(fcomplex a, fcomplex b)
	{ fcomplex c; c=fcmul(a, fcrcp(b)); return(c); }

static float fcdist(fcomplex a, fcomplex b)
	{ return(fclen(fcsub(a, b))); }
static fcomplex fcscale(fcomplex a, float f)
	{ fcomplex c; c.r=a.r*f; c.i=a.i*f; return(c); }


static int fceqp(fcomplex a, fcomplex b)
	{ return((a.r==b.r) && (a.i==b.i)); }
static int fcneqp(fcomplex a, fcomplex b)
	{ return((a.r!=b.r) || (a.i!=b.i)); }

static float fcabs(fcomplex a)
	{ return(sqrt(a.r*a.r + a.i*a.i)); }
static float fcarg(fcomplex a);
	{ return(atan2(a.i, a.r)); }
static fcomplex fconj(fcomplex a)
	{ return(fcomplex(a.r, -a.i)); }

static fcomplex fcsqrt(fcomplex z)
{
	fcomplex c;
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

static fcomplex fclog(fcomplex a)
	{ return(fcomplex(0.5*log(fclen2(a)), fcarg(a))); }

static fcomplex fcpow2(fcomplex a)
	{ fcomplex c; c.r=a.r*a.r-a.i*a.i; c.i=2*a.r*a.i; return(c); }

static fcomplex fcpow3(fcomplex a)
{
	fcomplex b, c;
	b.r=a.r*a.r-a.i*a.i; b.i=2*a.r*a.i;
	c.r=a.r*b.r-a.i*b.i; c.i=a.r*b.i+a.i*b.r;
	return(c);
}

static fcomplex fcpow4(fcomplex a)
	{ return(fcmul(fcpow2(a), fcpow2(a))); }
static fcomplex fcpow5(fcomplex a)
	{ return(fcmul(fcpow2(a), fcpow3(a))); }
static fcomplex fcpow6(fcomplex a)
	{ return(fcmul(fcpow3(a), fcpow3(a))); }

static fcomplex fcaddscale(fcomplex a, fcomplex b, float v)
	{ fcomplex c; c.r=a.r+(b.r*v); c.i=a.i+(b.i*v); return(c); }

static fcomplex fcexp(fcomplex a)
{
	fcomplex b, x;

	b=fcomplex(1.0+a.r, a.i); x=fcpow2(a);
	b=fcaddscale(b, x, 1.0/        2.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/        6.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/       24.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/      120.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/      720.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/     5040.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/    40320.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/   362880.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/  3628800.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/ 39916800.0); x=fcmul(x, a);
	b=fcaddscale(b, x, 1.0/479001600.0); x=fcmul(x, a);
	return(b);
}

static fcomplex fcpow(fcomplex a, fcomplex b);
	{ fcomplex c; c=fcexp(fcmul(fclog(a), b)); return(c); }

// static fcomplex fcproj(fcomplex a);

static fcomplex fcacos(fcomplex a);
static fcomplex fcasin(fcomplex a);
static fcomplex fcacosh(fcomplex a);
static fcomplex fcasinh(fcomplex a);

static fcomplex fcatan(fcomplex a);
static fcomplex fcatanh(fcomplex a);
static fcomplex fccos(fcomplex a);
static fcomplex fccosh(fcomplex a);
static fcomplex fcsin(fcomplex a);
static fcomplex fcsinh(fcomplex a);
static fcomplex fctan(fcomplex a);
static fcomplex fctanh(fcomplex a);

#endif
