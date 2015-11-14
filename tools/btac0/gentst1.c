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

void block_harr(s16 *iblk, int *oblk)
{
	int tblk[64];
	int tblk2[64];
	int i, j, n, b;

	b=0;
	for(i=0; i<64; i++)
		{ b+=iblk[i]; }
	b/=64;

	for(i=0; i<64; i++)
		{ tblk[i]=iblk[i]-b; }

//	for(i=0; i<64; i++)
//		{ tblk[i]=iblk[i]; }

	n=32;
	while(n>0)
	{
		for(i=0; i<n; i++)
		{
			tblk2[i]=(tblk[i*2+0]+tblk[i*2+1])>>1;
			tblk2[n+i]=tblk[i*2+0]-tblk[i*2+1];
		}

		for(i=0; i<(2*n); i++)
			{ tblk[i]=tblk2[i]; }

		n>>=1;
	}
	
	for(i=0; i<64; i++)
		{ oblk[i]=tblk[i]; }
}

int main(int argc, char *argv[])
{
	char tb[256];
	double actblk[64];
	int tblk[64];
	short *buf;
	float f, g, h, hn, nhz;
	int ch, rt, bits, len, flags, cmd;
	int i, j, k, n, nt, ont, ntt, ontt;

	sprintf(tb, "%s.wav", argv[1]);
//	sprintf(tb, "%s.wav", ifn);
//	buf=BGBTAC_LoadWAV_44Mono16(tb, &len);
//	buf=BGBTAC_LoadWAV_44Stereo16(tb, &len);
//	buf=BGBTAC_LoadWAV_Stereo16(tb, &rt, &len);
//	flags|=BGBTAC_EFL_STEREO;

	buf=BGBTAC_LoadWAV_Mono16(tb, &rt, &len);

//	buf=BGBTAC_LoadWAV16(tb, &ch, &rt, &len);
	if(!buf)
	{
		printf("%s: load failed '%s'\n", argv[0], argv[1]);
		return(-1);
	}

	for(i=0; i<64; i++)
		{ actblk[i]=0; }

	n=(len+63)/64;
	for(i=0; i<n; i++)
	{
		block_harr(buf+i*64, tblk);

		g=0;
		for(j=0; j<64; j++)
		{
			f=tblk[j];
			
//			h=f; f=f-g; g=h;

//			f=ssqrt(f);
			f=log(fabs(f)+1);
//			f=ssqrt(f);
//			f=log(fabs(f))/log(2.0);
//			f/=16;
			actblk[j]+=f*f;
		}
	}

	for(i=0; i<64; i++)
	{
		f=sqrt(actblk[i]/n);
		printf("%d: %f\n", i, f);
	}

#if 0
	for(i=0; i<len; i++)
		for(j=0; j<ch; j++)
	{
		f=buf[i*ch+j]/32767.0;

		g=0.1;
		if(fabs(f)>g)
		{
			h=1-g;
			f=sgn(f)*(g+pow((1/h)*(fabs(f)-g), 4)*h);
		}

		f=clamp(f, -1, 1);
		buf[i*ch+j]=f*32767;
	}

	sprintf(tb, "tst_dump1.wav");
	BGBTAC_StoreWAV(tb, (byte *)buf, ch, rt, 16, len);
#endif
}
