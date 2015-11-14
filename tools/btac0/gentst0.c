#include <bgbtac.h>

double ssqrt(double v)
{
	double f;
	f=(v>=0)?sqrt(v):-sqrt(-v);
	return(f);
}

double ssqr(double v)
{
	return((v>=0)?(v*v):(-v*v));
}

double spow(double v, double e)
{
	int i;
	double f;
	f=(v>=0)?pow(v, e):-pow(-v, e);
	return(f);
}

float clamp(float val, float min, float max)
{
	return((val<min)?min:((val<max)?val:max));
}

double sgn(double v)
{
	return((v>=0)?1.0:-1.0);
}

double sin_square(double v)
{
	float f, g;

	f=sin(v);
	g=(f>0)?1:-1;
	return(g);
}

double sin_wub(double v)
{
	float f, g;

//	g=sin(f*35*2*M_PI);
//	g=sin(f*110*2*M_PI);
	g=sin(v);

//	g=g*g*g;
	g=ssqrt(g);
//	g=spow(g, 0.75);
//	g=ssqr(g);
//	g=spow(g, 3);

//	if(fabs(g)>0.75)
//		g=spow(g, 4);

	if(fabs(g)>0.75)
	{
		g=sgn(g)*(0.75+pow(4*(fabs(g)-0.75), 4)*0.25);
	}

//	f=sin_square(v*0.25);
//	f=sin_square(v*2);
//	f=clamp(f, 0.5, 1.0);
//	g=g*f;

//	g=clamp(g, -0.9, 0.9);
//	g=clamp(g, -0.75, 0.75);
//	g=clamp(g, -0.5, 0.5);
//	g=clamp(g, -0.33, 0.33);

	return(g);
}

double sin2(double v)
{
	float f;
	f=sin(v);
	f=ssqr(f);
	return(f);
}

double sin_wub2(double v)
{
	return(
		sin_wub(v) +
		sin2(2*v)*0.000001 + 
		sin2(4*v)*0.00001 + 
		sin2(8*v)*0.0001 + 
		sin2(16*v)*0.001 + 
		sin2(32*v)*0.0025 + 
		sin2(64*v)*0.003 + 
		sin2(128*v)*0.001
		);
}

double sin_spike(double v)
{
	float f, g;
	
	f=1.0-(fmod(v, 2*M_PI)/M_PI);
	f=ssqr(f);
	return(f);
}

static double noisetab[256];

float btge_lerp(float a, float b, float t)
	{ return(a*(1-t)+(b*t)); }
float btge_scurve(float a)
	{ return(a*a*(3-2*a)); }

double noise(double v)
{
	float f, g, t;
	int i;
	
	f=v/(2*M_PI);
	i=(int)floor(f);
	t=f-i;
	
	t=btge_scurve(t);
	
	f=noisetab[(i+0)&255];
	g=noisetab[(i+1)&255];
	f=(1.0-t)*f+t*g;
	return(f);
}

int main(int argc, char *argv[])
{
	static float tune_hz[12]={
	 9/12.0,	//A
	11/12.0,	//B
	 0/12.0,	//C
	 2/12.0,	//D
	 4/12.0,	//E
	 5/12.0,	//F
	 7/12.0,	//G
	10/12.0,	//H / B_
	 1/12.0,	//I / C#
	 3/12.0,	//J / E_
	 6/12.0,	//K / F#
	 8/12.0,	//L / G#
	};
	char *notes="ABCD";
	char tb[256];
	float *buf;
	short *buf1;
	float f, g, h, hn, nhz;
	int ch, rt, bits, len, flags, cmd;
	int i, j, k, n, nt, ont, ntt, ontt;

	ch=1; rt=44100; len=60*44100;
	buf=malloc(len*2*sizeof(short));

	for(i=0; i<len; i++)
		{ buf[0]=0; }
		
	for(i=0; i<256; i++)
	{
		noisetab[i]=(rand()&32767)/16383.0-1.0;
	}
	
	ont=-1; nt=-1; ntt=-1; ontt=-1;
	for(i=0; i<len; i++)
	{
		f=i/44100.0;

		hn=noise(f);

		h=sin_spike(f*2*2*M_PI);

//		g=sin_wub(f*35*2*M_PI);
//		g=sin_wub(f*110*2*M_PI);
//		g=sin_wub((1.0+0.1*h)*f*60*2*M_PI);
//		g=sin_wub(f*60*2*M_PI);
		g=sin_wub((1.0+0.1*h+0.1*hn)*fmod(f, 1)*60*2*M_PI);
//		g*=0.2;

//		h=sin(f*2*2*M_PI);
		h=sin_spike(f*2*2*M_PI);
//		h=h+0.5;
		h=clamp(h, 0, 1);
		g*=h;

		buf[i]+=g;

		h=sin(f*0.5*2*M_PI)+0.25;
//		g=sin_wub(f*35*2*M_PI);
//		g=sin_wub((1.0+0.1*h+0.2*hn)*fmod(f, 1)*35*2*M_PI);
		g=sin_wub2((1.0+0.1*h+0.2*hn)*fmod(f, 1)*35*2*M_PI);

//		g*=1.5;

//		h=sin(f*0.5*2*M_PI)+0.25;
//		h=sin_spike(f*0.5*2*M_PI)+0.25;
		h=clamp(h, 0, 1);
		g*=h;

		buf[i]+=g;

//		g=sin(f*8*2*M_PI);
//		f=f*(1.0+g*0.001);
		
		ntt=((int)(f*4))%strlen(notes);
//		nt=notes[ntt]-'A';
		if(ntt!=ontt)
			{ nt=rand()%12; }
		ontt=ntt;

		nhz=1.0+tune_hz[nt]-tune_hz[0];
		if(nt!=ont)
		{
//			printf("%c %f\n", nt+'A', nhz);
			ont=nt;
		}

//		g=sin_wub(nhz*f*7040*2*M_PI);
//		g=sin_wub(nhz*f*3520*2*M_PI);
		g=sin_wub(nhz*f*1760*2*M_PI);
//		g=sin_wub(nhz*f*880*2*M_PI);
//		g=sin_wub(nhz*f*440*2*M_PI);
//		g=sin_wub(nhz*f*220*2*M_PI);
		g*=0.1;

//		h=sin(f*8*2*M_PI)+0.25;
//		h=clamp(h, 0, 1);
//		g*=h;

//		h=spow(sin(f*4*2*M_PI), 4);
		h=spow(sin_spike(f*4*2*M_PI), 4);
		h=clamp(h, 0, 1);
		g*=h;

		buf[i]+=g;

#if 1
		g=sin_wub(f*110*2*M_PI)*0.1;

		h=sin(f*4*2*M_PI)+0.25;
		h=clamp(h, 0, 1);
		g*=h;

		buf[i]+=g;
#endif

#if 1
		g=sin_wub(f*20*2*M_PI)*0.25;

		h=sin(f*0.125*2*M_PI)+0.25;
		h=clamp(h, 0, 1);
		g*=h;

		buf[i]+=g;
#endif

//		g*=2;
//		g=clamp(g, -1, 1);
		
	}


	buf1=malloc(len*2*sizeof(short));

	for(i=0; i<len; i++)
	{
		f=buf[i];
		f=clamp(f, -1, 1);		
		buf1[i]=f*32767;
	}

	sprintf(tb, "tst_dump0.wav");
	BGBTAC_StoreWAV(tb, (byte *)buf1, ch, rt, 16, len);
}
