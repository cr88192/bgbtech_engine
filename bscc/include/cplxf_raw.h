typedef struct {float r,i;} fcplx;

#define fcplx(r,i)	_fcplx(r,i)

static fcplx _fcplx(float r, float i)
	{ fcplx t; t.r=r; t.i=r; return(t); };

static fcplx _vec2vf(float *x)
	{ return(_vec2i(x[0], x[1])); };

static float *_vfvec2(vec4 v)
{
	float *p;
	p=gcralloc(4*sizeof(float));
	p[0]=v.r; p[1]=v.i; p[2]=0; p[3]=0;
	return(p);
}

static float fcreal(fcplx a)	{ return(a.r); };
static float fcimag(fcplx a)	{ return(a.i); };

static float fcdot(fcplx a, fcplx b)
	{ return(a.r*b.r+a.i*b.i); }
static float fccross(fcplx a, fcplx b)
	{ return(a.r*b.i-a.i*b.r); }

static float fclen(fcplx a)
	{ return(sqrt(a.r*a.r+a.i*a.i)); }
static float fclen2(fcplx a)
	{ return(a.r*a.r+a.i*a.i); }

static fcplx fcnorm(fcplx a)
{	fcplx c; float l;
	l=fclen(a); if(l!=0)l=1.0/l;
	c.r=a.r*l; c.i=a.i*l;
	return(c); }

static fcplx fcneg(fcplx a)
	{ fcplx c; c.r=-a.r; c.i=-a.i; return(c); }

static fcplx fcadd(fcplx a, fcplx b)
	{ fcplx c; c.r=a.r+b.r; c.i=a.i+b.i; return(c); }
static fcplx fcsub(fcplx a, fcplx b)
	{ fcplx c; c.r=a.r-b.r; c.i=a.i-b.i; return(c); }

static fcplx fcmul(fcplx a, fcplx b)
	{ fcplx c; c.r=a.r*b.r-a.i*b.i; c.i=a.r*b.i+a.i*b.r; return(c); }

static fcplx fcrcp(fcplx a)
{
	float f; f=a.r*a.r+a.i*a.i;
	return(fcplx(a.r/f, -a.i/f));
}

static fcplx fcdiv(fcplx a, fcplx b)
	{ fcplx c; c=fcmul(a, fcrcp(b)); return(c); }

static float fcdist(fcplx a, fcplx b)
	{ return(fclen(fcsub(a, b))); }
static fcplx fcscale(fcplx a, float f)
	{ fcplx c; c.r=a.r*f; c.i=a.i*f; return(c); }


static int fceqp(fcplx a, fcplx b)
	{ return((a.r==b.r) && (a.i==b.i)); }
static int fcneqp(fcplx a, fcplx b)
	{ return((a.r!=b.r) || (a.i!=b.i)); }

static float fcabs(fcplx a)
	{ return(sqrt(a.r*a.r + a.i*a.i)); }
static float fcarg(fcplx a);
	{ return(atan2(a.i, a.r)); }
static fcplx fconj(fcplx a)
	{ return(fcplx(a.r, -a.i)); }

static fcplx fcsqrt(fcplx z)
{
	fcplx c;
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

static fcplx fclog(fcplx a)
	{ return(fcplx(0.5*log(fclen2(a)), fcarg(a))); }

static fcplx fcpow2(fcplx a)
	{ fcplx c; c.r=a.r*a.r-a.i*a.i; c.i=2*a.r*a.i; return(c); }

static fcplx fcpow3(fcplx a)
{
	fcplx b, c;
	b.r=a.r*a.r-a.i*a.i; b.i=2*a.r*a.i;
	c.r=a.r*b.r-a.i*b.i; c.i=a.r*b.i+a.i*b.r;
	return(c);
}

static fcplx fcpow4(fcplx a)
	{ return(fcmul(fcpow2(a), fcpow2(a))); }
static fcplx fcpow5(fcplx a)
	{ return(fcmul(fcpow2(a), fcpow3(a))); }
static fcplx fcpow6(fcplx a)
	{ return(fcmul(fcpow3(a), fcpow3(a))); }

static fcplx fcaddscale(fcplx a, fcplx b, float v)
	{ fcplx c; c.r=a.r+(b.r*v); c.i=a.i+(b.i*v); return(c); }

static fcplx fcexp(fcplx a)
{
	fcplx b, x;

	b=fcplx(1.0+a.r, a.i); x=fcpow2(a);
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

static fcplx fcpow(fcplx a, fcplx b);
	{ fcplx c; c=fcexp(fcmul(fclog(p), q)); return(c); }


static fcplx fcproj(fcplx a);

static fcplx fcacos(fcplx a);
static fcplx fcasin(fcplx a);
static fcplx fcacosh(fcplx a);
static fcplx fcasinh(fcplx a);

static fcplx fcatan(fcplx a);
static fcplx fcatanh(fcplx a);
static fcplx fccos(fcplx a);
static fcplx fccosh(fcplx a);
static fcplx fcsin(fcplx a);
static fcplx fcsinh(fcplx a);
static fcplx fctan(fcplx a);
static fcplx fctanh(fcplx a);
