#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>


double apx_pi(double f)
{
	return(f/(sqrt(f*f+1)+1));
}

double bgbdy_pow2(int i)
{
	if(i>0)return(1<<i);
	if(i<0)return(1.0/(1<<(-i)));
	return(1);
}

double bgbdy_ssqrt(double f)
{
	if(f>0)return(sqrt(f));
	if(f<0)return(-sqrt(-f));
	return(0);
}

double bgbdy_ssqr(double f)
{
	if(f>0)return(f*f);
	if(f<0)return(-(f*f));
	return(0);
}

int enc_g0(double f)
{
	int i, j;
	unsigned int k;

	//S.7.20, bias=24

	if(f<0) { f=-f; k|=0x8000000; } else k=0;
	i=ceil(log2(f));
	if(i<(-40))return(0);
	if(i<(-24))i=-24;
	if(i>=104)return(0);
//	j=(int)((f/pow(2, i))*1048576.0+0.5);
	j=(int)((f/bgbdy_pow2(i))*1048576.0+0.5);
	if(j>0xFFFFF)j=0xFFFFF;
	k|=((i+24)<<20)|j;

	return(k);
}

double dec_g0(int i)
{
	double f;
	int j;

//	f=((i&0xFFFFF)/1048576.0)*pow(2, ((i>>20)&127)-24);
	f=((i&0xFFFFF)/1048576.0)*bgbdy_pow2(((i>>20)&127)-24);
	if(i&0x8000000)f=-f;
	return(f);
}

int enc_g1(double f)
{
	double f0, f1, lh, ll;
	int i;

//	lh=log(pow(2, 80)); ll=log(1.0/pow(2, 32));
//	f0=log(f)-ll; f1=lh-ll;

	lh=bgbdy_ssqrt(log(pow(2, 80)));
	ll=bgbdy_ssqrt(log(1.0/pow(2, 32)));
	f0=bgbdy_ssqrt(log(f))-ll; f1=lh-ll;

	i=(f0/f1)*(1<<27)+0.5;

	if(i<0)i=0;
	if(i>=(1<<27))i=(1<<27)-1;

	return(i);
}

double dec_g1(int i)
{
	double f0, f1, lh, ll;

//	lh=log(pow(2, 80)); ll=log(1.0/pow(2, 32));
//	f0=i/((double)(1<<27));
//	f0=exp(f0*(lh-ll)+ll);

	lh=bgbdy_ssqrt(log(pow(2, 80)));
	ll=bgbdy_ssqrt(log(1.0/pow(2, 32)));
	f0=i/((double)(1<<27));
	f0=exp(bgbdy_ssqr(f0*(lh-ll)+ll));

	return(f0);
}

double flt_g0(double f)
{
	return(dec_g0(enc_g0(f)));
}

double flt_g1(double f)
{
	return(dec_g1(enc_g1(f)));
}

int main()
{
	double f0, f1, f2;
	double g0, g1, g2;
	double e0, e1, e2;
	float ff0;
	int i;

	e0=0; e1=0; e2=0;

	f0=1.0/sqrt(3); f1=f0; f2=f0;
	for(i=0; i<32; i++)
	{
		g0=(6*pow(2, i))*f0;
		g1=(6*pow(2, i))*f1;
		g2=(6*pow(2, i))*f2;

//		printf("%d %.16f %.16f %.16f %d %f\n", i,
//			g0, g1, g2, enc_g1(f2), log(f2));
		f0=apx_pi(f0);
		f1=flt_g0(apx_pi(f1));
		f2=flt_g1(apx_pi(f2));

		ff0=g0;
		e0+=fabs(ff0-g0);
		e1+=fabs(g1-g0);
		e2+=fabs(g2-g0);
	}

	printf("%.16f %.16f %.16f\n", e0, e1, e2);


	e0=0; e1=0;

	for(i=0; i<32; i++)
	{
		f0=sin(i*(M_PI/16));
		f1=flt_g0(f0);
		f2=flt_g1(f0);

		ff0=f0;
		e0+=fabs(ff0-f0);
		e1+=fabs(f1-f0);
		e2+=fabs(f2-f0);
	}

	printf("%.16f %.16f %.16f\n", e0, e1, e2);
}
