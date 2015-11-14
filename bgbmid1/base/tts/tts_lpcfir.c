#include <bgbmid.h>

double bgbmid_lpcfir_ssqr(double f)
{
	return((f>=0)?(f*f):(-(f*f)));
}

double bgbmid_lpcfir_ssqrt(double f)
{
	return((f>=0)?sqrt(f):(-sqrt(-f)));
}

BMID_API int BGBMID_LpcFIR_BuildSampleLPC(
	s16 *samp, int len, double *fir, int szFir)
{
	double f, g;
	int i, j, k;
	
	for(i=0; i<szFir; i++)
		fir[i]=0;
	
	for(i=szFir; i<len; i++)
	{
		for(j=0; j<szFir; j++)
		{
			fir[j]+=((double)samp[i])*((double)samp[i-j-1]);
		}
	}

//	for(i=0; i<szFir; i++)
//		fir[i]=bgbmid_lpcfir_ssqrt(fir[i]);

	for(i=0; i<szFir; i++)
	{
		fir[i]=bgbmid_lpcfir_ssqrt(fir[i]/(len-szFir));
	}

	f=0;
	for(i=0; i<szFir; i++)
	{
		f+=fir[i];
	}

	for(i=0; i<szFir; i++)
	{
		fir[i]=fir[i]/f;
	}

//	for(i=0; i<szFir; i++)
//	{
//		fir[i]=bgbmid_lpcfir_ssqrt(fir[i]/(len-szFir));
//	}
	
	return(0);
}

BMID_API int BGBMID_LpcFIR_ApplySampleLPC(
	s16 *ibp, s16 *obp, int len, double *fir, int szFir)
{
	double f, g;
	int i, j, k;
	
	for(i=0; i<len; i++)
	{
		f=0;
		for(j=0; j<szFir; j++)
		{
			k=i-j-1;
			if(k<0)k+=len;
			k=ibp[k];
			
			f+=k*fir[j];
		}
		
		if(f>32766)f=32766;
		if(f<-32766)f=-32766;
		
		obp[i]=f+0.5;
	}

	return(0);
}

BMID_API int BGBMID_LpcFIR_Normalize(
	s16 *ibp, s16 *obp, int len, int val)
{
	int i, j, k;
	
	j=0;
	for(i=0; i<len; i++)
	{
		j+=abs(ibp[i]);
	}
	j/=len;

	for(i=0; i<len; i++)
	{
		obp[i]=ibp[i]*((float)val)/j;
	}
	return(0);
}

BMID_API double bgbmid_lpcfir_sin_sqr(double f)
{
	int i;
	
	i=f*(1.0/M_PI);
	return((i&1)?-1.0:1.0);
}

BMID_API double bgbmid_lpcfir_sin_saw(double f)
{
	double g;
	int i;
	
	i=f*(256.0/M_PI);
//	g=((i&511)-256)*(1.0/256);
	g=((511-(i&511))-256)*(1.0/256);
	return(g);
	
//	return((i&1)?-1.0:1.0);
}

BMID_API double bgbmid_lpcfir_sin_saw2(double f)
{
	return(bgbmid_lpcfir_sin_saw(f) + 
		bgbmid_lpcfir_sin_saw(2*f)*0.5);
}

BMID_API double bgbmid_lpcfir_sin_tri(double t)
{
	static double a=1.0/(2*M_PI);
	double g;
	g=2*(fabs(2*(t*a-floor(t*a+0.5))))-1;
	return(g);
}

BMID_API void BGBMID_LpcFIR_LowPass(
	s16 *ibp, s16 *obp, int len, float q, float r)
{
	double f, g;
	double o, v;
	int i, j, k;
	
	o=0; v=0;
	for(i=0; i<len; i++)
	{
		v=v+(ibp[i]-o)*q;
		o=o+v;	v=v*r;
		j=o+0.5;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_HighPass(
	s16 *ibp, s16 *obp, int len, float q, float r)
{
	double f, g;
	double o, v;
	int i, j, k;
	
	o=0; v=0;
	for(i=0; i<len; i++)
	{
		v=v+(ibp[i]-o)-(o*q);
		o=o+v;	v=v*r;
		j=o+0.5;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

float bgbmid_scurve(float a)
	{ return(a*a*(3-2*a)); }

BMID_API void BGBMID_LpcFIR_LowPassLerp(
	s16 *ibp, s16 *obp, int len,
	float q0, float q1,
	float r0, float r1)
{
	double o, v, q, r;
	double f, g, t;
	int i, j, k;
	
	o=0; v=0;
	for(i=0; i<len; i++)
	{
		t=((double)i)/len;
//		t=1.0;
		t=0.5;
//		t=bgbmid_scurve(t);
		q=(1.0-t)*q0 + t*q1;
		r=(1.0-t)*r0 + t*r1;
	
		v=v+(ibp[i]-o)*q;
		o=o+v;	v=v*r;
		j=o+0.5;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_HighPassLerp(
	s16 *ibp, s16 *obp, int len,
	float q0, float q1,
	float r0, float r1)
{
	double o, v, q, r;
	double f, g, t;
	int i, j, k;
	
	o=0; v=0;
	for(i=0; i<len; i++)
	{
		t=((double)i)/len;
//		t=1.0;
		t=0.5;
//		t=bgbmid_scurve(t);
		q=(1.0-t)*q0 + t*q1;
		r=(1.0-t)*r0 + t*r1;
	
		v=v+(ibp[i]-o)-(o*q);
		o=o+v;	v=v*r;
		j=o+0.5;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_BandPass(
	s16 *ibp, s16 *obp, int len, float ql, float qh, float r)
{
	BGBMID_LpcFIR_LowPass(ibp, obp, len, ql, r);
	BGBMID_LpcFIR_HighPass(obp, obp, len, qh, r);
}

BMID_API void BGBMID_LpcFIR_BandPassLerp(
	s16 *ibp, s16 *obp, int len,
	float ql0, float ql1, float qh0, float qh1, float r0, float r1)
{
	BGBMID_LpcFIR_LowPassLerp(ibp, obp, len, ql0, ql1, r0, r1);
	BGBMID_LpcFIR_HighPassLerp(obp, obp, len, qh0, qh1, r0, r1);
}

BMID_API void BGBMID_LpcFIR_BandPassAddOut(
	s16 *ibp, s16 *obp, int len, float ql, float qh, float r)
{
	s16 *tbp;
	
	tbp=malloc(len*sizeof(s16));
	BGBMID_LpcFIR_LowPass(ibp, tbp, len, ql, r);
	BGBMID_LpcFIR_HighPass(tbp, tbp, len, qh, r);
	BGBMID_LpcFIR_AddSamples(obp, tbp, obp, len);
	free(tbp);
}

/** Band Pass filter taking a center frequency and bandwidth. */
BMID_API void BGBMID_LpcFIR_BandPass2(
	s16 *ibp, s16 *obp, int len, int rt, float hz, float w, float r)
{
	float ql, qh;
	
	ql=(hz+w)/(2*rt);
	qh=(hz-w)/(2*rt);
//	BGBMID_LpcFIR_BandPass(ibp, obp, len, ql, qh, r);
	BGBMID_LpcFIR_BandPass(ibp, obp, len, qh, ql, r);
}

/** Band Pass filter taking a center frequency and bandwidth. */
BMID_API void BGBMID_LpcFIR_BandPassLerp2(
	s16 *ibp, s16 *obp, int len, int rt,
	float hz0, float hz1, float w0, float w1, float r0, float r1)
{
	float ql0, qh0;
	float ql1, qh1;
	
	ql0=(hz0+w0)/(2*rt);
	qh0=(hz0-w0)/(2*rt);
	ql1=(hz1+w1)/(2*rt);
	qh1=(hz1-w1)/(2*rt);

//	BGBMID_LpcFIR_BandPass(ibp, obp, len, ql, qh, r);
//	BGBMID_LpcFIR_BandPass(ibp, obp, len, qh, ql, r);
//	BGBMID_LpcFIR_BandPass(ibp, obp, len, qh0, ql0, r0);

	BGBMID_LpcFIR_BandPassLerp(ibp, obp, len,
		qh0, qh1, ql0, ql1, r0, r1);
}

BMID_API void BGBMID_LpcFIR_BandPassAddOut2(
	s16 *ibp, s16 *obp, int len, int rt, float hz, float w, float r)
{
	float ql, qh;
	
	ql=(hz+w)/(2*rt);
	qh=(hz-w)/(2*rt);
//	BGBMID_LpcFIR_BandPass(ibp, obp, len, ql, qh, r);
	BGBMID_LpcFIR_BandPassAddOut(ibp, obp, len, qh, ql, r);
}

BMID_API void BGBMID_LpcFIR_AddNoise(
	s16 *ibp, s16 *obp, int len, float vol, float q, float r)
{
	double o, v;
	double f, g;
	int h;
	int i, j, k;

	h=(nlint)ibp;
	h=h*65521+((nlint)obp);
	h=h*65521;

	o=0; v=0;
	for(i=0; i<len; i++)
	{
		j=ibp[i];
		f=((h>>16)&255)/255.0;

		g=f*f*vol;
		v=v+(g-o)*q;
		o=o+v;	v=v*r;

		k=j+o;
		k=(k<(-32768))?(-32768):((k>32767)?32767:k);
		obp[i]=k;
		h=h*65521;
	}
}

BMID_API void BGBMID_LpcFIR_AddWhiteNoise(
	s16 *ibp, s16 *obp, int len, float vol)
{
	double f, g;
	int h;
	int i, j, k;

	h=(nlint)ibp;
	h=h*65521+((nlint)obp);
	h=h*65521;

	if(!ibp)
	{
		if(!obp)
			return;
	
		for(i=0; i<len; i++)
		{
			f=((h>>16)&255)/255.0;
			g=f*f*vol;
			k=g;
			obp[i]=k;
			h=h*65521;
		}

		return;
	}

	for(i=0; i<len; i++)
	{
		j=ibp[i];
		f=((h>>16)&255)/255.0;

		g=f*f*vol;
		k=j+g;
		obp[i]=k;
		h=h*65521;
	}
}

BMID_API void BGBMID_LpcFIR_AddSamples(
	s16 *ibp0, s16 *ibp1, s16 *obp, int len)
{
	int i, j, k;

	for(i=0; i<len; i++)
	{
		j=ibp0[i]+ibp1[i];
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_ScaleSamples(
	s16 *ibp, s16 *obp, int len, float vol)
{
	int i, j, k;

	for(i=0; i<len; i++)
	{
		j=ibp[i]*vol;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_AddScaleSamples(
	s16 *ibp0, s16 *ibp1, s16 *obp, int len, float vol)
{
	int i, j, k;

	for(i=0; i<len; i++)
	{
		j=ibp0[i]+ibp1[i]*vol;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_ScaleSamplesLerp(
	s16 *ibp, s16 *obp, int len, float vol0, float vol1)
{
	float f, g, t;
	int i, j, k;

	for(i=0; i<len; i++)
	{
		t=((float)i)/len;
		f=(1.0-t)*vol0 + t*vol1;
		
		j=ibp[i]*f;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_AddScaleSamplesLerp(
	s16 *ibp0, s16 *ibp1, s16 *obp, int len,
	float vol0, float vol1)
{
	float f, g, t;
	int i, j, k;

	for(i=0; i<len; i++)
	{
		t=((float)i)/len;
		f=(1.0-t)*vol0 + t*vol1;
		
		j=ibp0[i]+ibp1[i]*f;
		j=(j<(-32768))?(-32768):((j>32767)?32767:j);
		obp[i]=j;
	}
}

BMID_API void BGBMID_LpcFIR_AddNoiseBandPass2(
	s16 *ibp, s16 *obp, int len, int rt,
	float vol, float hz, float w, float r)
{
	s16 *tbp;
	float ql, qh;
	
	tbp=malloc(len*sizeof(s16));
	BGBMID_LpcFIR_AddWhiteNoise(NULL, tbp, len, vol);
	
	ql=(hz+w)/(2*rt);
	qh=(hz-w)/(2*rt);
//	BGBMID_LpcFIR_BandPass(ibp, obp, len, ql, qh, r);
	BGBMID_LpcFIR_BandPass(tbp, tbp, len, qh, ql, r);
	BGBMID_LpcFIR_AddSamples(ibp, tbp, obp, len);
	free(tbp);
}

BMID_API int BGBMID_LpcFIR_AddFormant(
	s16 *ibp, s16 *obp, int len, float hz, float vol)
{
	double f, g;
	int i, j, k;
	
	j=0;
	for(i=0; i<len; i++)
	{
		j=ibp[i];
//		f=sin(i*(2*M_PI)*(hz/44100));
//		f=bgbmid_lpcfir_ssqr(f);
//		f=bgbmid_lpcfir_sin_sqr(i*(2*M_PI)*(hz/44100));
//		f=bgbmid_lpcfir_sin_saw2(i*(2*M_PI)*(hz/44100));
//		f=bgbmid_lpcfir_sin_tri(i*(2*M_PI)*(hz/44100));
		f=bgbmid_lpcfir_sin_tri(i*M_PI*(hz/44100));
		k=vol * 1024 * f + 0.5;
		obp[i]=j+k;
	}
	return(0);
}
