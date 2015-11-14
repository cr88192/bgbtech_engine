#include <bgbdy.h>

fcomplex dtyFComplex(float r, float i)
{
	fcomplex c;
	c.r=r; c.i=i;
	return(c);
}

float dtyFCreal(fcomplex a)
	{ return(a.r); }
float dtyFCimag(fcomplex a)
	{ return(a.i); }

fcomplex dtyFCadd(fcomplex a, fcomplex b)
	{ fcomplex c; c.r=a.r+b.r; c.i=a.i+b.i; return(c); }
fcomplex dtyFCsub(fcomplex a, fcomplex b)
	{ fcomplex c; c.r=a.r-b.r; c.i=a.i-b.i; return(c); }

fcomplex dtyFCmul(fcomplex a, fcomplex b)
	{ fcomplex c; c.r=a.r*b.r-a.i*b.i; c.i=a.i*b.r+a.r*b.i; return(c); }
fcomplex dtyFCscale(fcomplex a, float x)
	{ fcomplex c; c.r=x*a.r; c.i=x*a.i; return(c); }
fcomplex dtyFCrscale(float x, fcomplex a)
	{ fcomplex c; c.r=x*a.r; c.i=x*a.i; return(c); }

fcomplex dtyFConj(fcomplex z)
	{ fcomplex c; c.r=z.r; c.i=-z.i; return(c); }

fcomplex dtyFCdiv(fcomplex a, fcomplex b)
{
	fcomplex c;
	float r,d;

	if(fabs(b.r)>=fabs(b.i))
	{
		r=b.i/b.r;
		d=b.r+r*b.i;
		c.r=(a.r+r*a.i)/d;
		c.i=(a.i-r*a.r)/d;
	}else
	{
		r=b.r/b.i;
		d=b.i+r*b.r;
		c.r=(a.r*r+a.i)/d;
		c.i=(a.i*r-a.r)/d;
	}
	return(c);
}

float dtyFCabs(fcomplex a)
{
	float x, y, z, f;

	x=fabs(a.r);
	y=fabs(a.i);
	if(x==0.0)
		{ z=y; }
	else if(y==0.0)
		{ z=x; }
	else if(x>y)
		{ f=y/x; z=x*sqrt(1.0+f*f); }
	else	{ f=x/y; z=y*sqrt(1.0+f*f); }
	return(z);
}

fcomplex dtyFCsqrt(fcomplex z)
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



dcomplex dtyDComplex(double r, double i)
{
	dcomplex c;
	c.r=r; c.i=i;
	return(c);
}

double dtyDCreal(dcomplex a)
	{ return(a.r); }
double dtyDCimag(dcomplex a)
	{ return(a.i); }

dcomplex dtyDCadd(dcomplex a, dcomplex b)
	{ dcomplex c; c.r=a.r+b.r; c.i=a.i+b.i; return(c); }
dcomplex dtyDCsub(dcomplex a, dcomplex b)
	{ dcomplex c; c.r=a.r-b.r; c.i=a.i-b.i; return(c); }

dcomplex dtyDCmul(dcomplex a, dcomplex b)
	{ dcomplex c; c.r=a.r*b.r-a.i*b.i; c.i=a.i*b.r+a.r*b.i; return(c); }
dcomplex dtyDCscale(dcomplex a, double x)
	{ dcomplex c; c.r=x*a.r; c.i=x*a.i; return(c); }
dcomplex dtyDCrscale(double x, dcomplex a)
	{ dcomplex c; c.r=x*a.r; c.i=x*a.i; return(c); }

dcomplex dtyDConj(dcomplex z)
	{ dcomplex c; c.r=z.r; c.i=-z.i; return(c); }

dcomplex dtyDCdiv(dcomplex a, dcomplex b)
{
	dcomplex c;
	double r,d;

	if(fabs(b.r)>=fabs(b.i))
	{
		r=b.i/b.r;
		d=b.r+r*b.i;
		c.r=(a.r+r*a.i)/d;
		c.i=(a.i-r*a.r)/d;
	}else
	{
		r=b.r/b.i;
		d=b.i+r*b.r;
		c.r=(a.r*r+a.i)/d;
		c.i=(a.i*r-a.r)/d;
	}
	return(c);
}

double dtyDCabs(dcomplex a)
{
	double x, y, z, f;

	x=fabs(a.r);
	y=fabs(a.i);
	if(x==0.0)
		{ z=y; }
	else if(y==0.0)
		{ z=x; }
	else if(x > y)
		{ f=y/x; z=x*sqrt(1.0+f*f); }
	else	{ f=x/y; z=y*sqrt(1.0+f*f); }
	return(z);
}

dcomplex dtyDCsqrt(dcomplex z)
{
	dcomplex c;
	double x,y,w,r;

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
		}else
		{
			c.i=(z.i >= 0)?w:-w;
			c.r=z.i/(2.0*c.i);
		}
	}
	return(c);
}
