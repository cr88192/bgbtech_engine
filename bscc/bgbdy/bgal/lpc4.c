#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#if 1
#define DCTBLKSZ	256
#define FMTSZ		8
#define FMTCHN		16
#endif

#if 0
#define DCTBLKSZ	1024
#define FMTSZ		32
#define FMTCHN		16
#endif

#define FMTCNT		1024

#define LPC_ORD		64
#define LPC_SZ		LPC_ORD

#define LPC_SIMSAMP	32	//number of simulated samples
//#define LPC_SIMSTEP	4	//number of simulated steps

#define LPC_SIM1	6	//first test
#define LPC_SIM2	2	//second test
#define LPC_SIMSTEP	(LPC_SIM1+LPC_SIM2)	//number of simulated steps

#define LPC_POP		4096	//population
#define LPC_SELECT	64	//total selected per generation
#define LPC_RPICK	16	//random picks in selection

#define LPC_STEPS	128	//number of generations


typedef unsigned char byte;

#include "wave.c"
#include "util.c"
#include "bsg_dct.c"

#include "bsg_dct_ff.c"
#include "bsg_vsb.c"
#include "bsg_phn.c"

short spc_vec_blk_samp2[FMTCNT][DCTBLKSZ];


float bsg_lpc_test_flt[LPC_POP][LPC_SZ];
float bsg_lpc_test_err[LPC_POP];

float bsg_lpc_pred(short *bp, float *lpc, int ord)
{
	int i, j;
	float f, g;

	g=0;
	for(i=0; i<ord; i++)
	{
		j=*(bp-(i+1));
		g+=lpc[i]*j;
	}

	return(g);
}

#if 1
float bsg_lpc_err(short *bp, short *cbp, int sz, float *lpc, int ord)
{
	int i, j;
	float f, g;

	g=0;
	for(i=0; i<sz; i++)
	{
		f=bsg_lpc_pred(cbp+i, lpc, LPC_ORD);
		g+=fabs(bp[i]-f);
	}

	return(g);
}

float bsg_lpc_test(int irn, int vb, int fn)
{
	short *bp, *cbp;
	int i;
	float f;

	bp=spc_vec_samp[vb][fn];
	cbp=spc_vec_blk_samp[vb];
	i=(rand()%(DCTBLKSZ-(LPC_ORD+LPC_SIMSAMP)))+LPC_ORD;
//	i=LPC_ORD;
	f=bsg_lpc_err(bp+i, bp+i, LPC_SIMSAMP, bsg_lpc_test_flt[irn], LPC_ORD);
	return(f);
}

float bsg_lpc_test2(int irn, int vb, int fn)
{
	short *bp, *cbp;
	int i;
	float f;

	bp=spc_vec_samp[vb][fn];
	cbp=spc_vec_blk_samp[vb];
	i=(rand()%(DCTBLKSZ-(LPC_ORD+LPC_SIMSAMP)))+LPC_ORD;
//	i=LPC_ORD;
	f=bsg_lpc_err(cbp+i, cbp+i, LPC_SIMSAMP, bsg_lpc_test_flt[irn], LPC_ORD);
	return(f);
}
#endif

float bsg_ssqrt(float f)
	{ if(f<0)return(-sqrt(-f)); return(sqrt(f)); }
float bsg_nfrand()
	{ return(rand()/((float)RAND_MAX)); }
float bsg_sfrand()
	{ return(rand()/(RAND_MAX*0.5)-1.0); }
float bsg_sqrrand()
	{ return(sqrt(bsg_nfrand()*65536)/256); }
float bsg_ssqrrand()
	{ return(bsg_ssqrt(bsg_sfrand()*65536)/256); }

void bsg_lpc_swap(int a, int b)
{
	int i;
	float f, g;

	if(a==b)return;

	f=bsg_lpc_test_err[a];
	g=bsg_lpc_test_err[b];
	bsg_lpc_test_err[a]=g;
	bsg_lpc_test_err[b]=f;

	for(i=0; i<LPC_SZ; i++)
	{
		f=bsg_lpc_test_flt[a][i];
		g=bsg_lpc_test_flt[b][i];
		bsg_lpc_test_flt[a][i]=g;
		bsg_lpc_test_flt[b][i]=f;
	}
}

void bsg_lpc_breed(int a, int b, int c, int gn)
{
	float *pfa, *pfb, *pfc;
	float f, g, m;
	int i;

	bsg_lpc_test_err[c]=-1;

	f=gn/((float)LPC_STEPS);
	m=1.0-(f*f);

	pfa=bsg_lpc_test_flt[a];
	pfb=bsg_lpc_test_flt[b];
	pfc=bsg_lpc_test_flt[c];
	for(i=0; i<LPC_SZ; i++)
	{
		f=bsg_nfrand();
		g=pfa[i]*(1-f) + pfb[i]*f;

		g+=bsg_ssqrrand()*(0.1*m);

		f=bsg_nfrand();
		if(f<(0.05*m))g*=bsg_ssqrt(bsg_sfrand()*1.5);

		pfc[i]=g;
	}
}

void bsg_lpc_step_test(int fn, int gn)
{
	int i, j, k;
	float f, g;

#if 0
	k=rand()&255;
	for(i=0; i<LPC_POP; i++)
	{
		f=0;
		for(j=0; j<16; j++)
			f+=bsg_lpc_test(i, (k+j)&255, fn);
		bsg_lpc_test_err[i]=f;
	}
#endif

#if 1
	for(i=0; i<LPC_POP; i++)
	{
		if(!((i+1)%16))
		{
			printf("%d/%d\r", i+1, LPC_POP);
			fflush(stdout);
		}

		f=0;
		for(j=0; j<LPC_SIM1; j++)
		{
			k=rand()&(FMTCNT-1);
			f+=bsg_lpc_test(i, k, fn);
		}

		for(j=0; j<LPC_SIM2; j++)
		{
			k=rand()&(FMTCNT-1);
			f+=bsg_lpc_test2(i, k, fn);
		}

		bsg_lpc_test_err[i]=f;
	}
	printf("\n");
#endif
}

void bsg_lpc_step_select(int fn, int gn)
{
	int i, j, k;
	float f, g;

	for(i=0; i<(LPC_SELECT-LPC_RPICK); i++)
	{
		k=i; f=bsg_lpc_test_err[i];
		for(j=i+1; j<LPC_POP; j++)
		{
			g=bsg_lpc_test_err[j];
			if(g<f) { f=g; k=j; }
		}
		bsg_lpc_swap(i, k);
	}

	for(i=(LPC_SELECT-LPC_RPICK); i<LPC_SELECT; i++)
	{
		j=(rand()%(LPC_POP-i))+i;
		bsg_lpc_swap(i, j);
	}

	for(i=LPC_SELECT; i<LPC_POP; i++)
	{
		j=sqrt(rand()&(LPC_POP-1));
		k=sqrt(rand()&(LPC_POP-1));
		bsg_lpc_breed(j, k, i, gn);
	}
}

void bsg_lpc_step_status(int fn, int gn)
{
	int i, j, k;
	float f, g;

	for(i=0; i<8; i++)
	{
		printf("%d %.1f  ", i, bsg_lpc_test_err[i]);

		for(j=0; j<12; j++)
			printf("%.1f ", bsg_lpc_test_flt[i][j]);
		printf("\n");
	}

	f=0;
	for(i=0; i<LPC_SELECT; i++)
		f+=bsg_lpc_test_err[i];
	printf("AVG Err %f (%.4f sample)\n",
		f/LPC_SELECT, (f/LPC_SELECT)/(LPC_SIMSTEP*LPC_SIMSAMP));
}

void bsg_lpc_step(int fn, int gn)
{
	bsg_lpc_step_test(fn, gn);
	bsg_lpc_step_select(fn, gn);
	bsg_lpc_step_status(fn, gn);
}

void bsg_lpc_setup(int fn)
{
	float *pf, *pfa, *pfb, *pfc, *pfd;
	int i, j, k, l;
	float f, g;

	for(i=0; i<LPC_POP; i++)
	{
		pf=bsg_lpc_test_flt[i];
		for(j=0; j<LPC_SZ; j++)
			pf[j]=bsg_ssqrrand()*10;
	}

#if 1
	for(i=0; i<LPC_ORD; i++)
	{
		pfa=bsg_lpc_test_flt[i];
		pfb=bsg_lpc_test_flt[i+LPC_ORD];
		for(j=0; j<LPC_ORD; j++)pfa[j]=0;
		for(j=0; j<LPC_ORD; j++)pfb[j]=0;
		pfa[i]=1;
		pfb[i]=-1;
	}
#endif
}

int bsg_lpc_dump_best(FILE *fd)
{
	int i, j, k;

	i=0;
	for(j=0; j<(LPC_SZ/8); j++)
	{
		for(k=0; k<8; k++)
			fprintf(fd, "%.8f ",
				bsg_lpc_test_flt[0][j*8+k]);
		fprintf(fd, "\n");
	}
	fprintf(fd, "\n");
}

int bsg_lpc_evolve()
{
	char tb[256];
	FILE *fd;
	int i, j;

	for(i=0; i<FMTCHN; i++)
	{
		bsg_lpc_setup(i);
		for(j=0; j<LPC_STEPS; j++)
		{
			printf("Chan %d Gen %d\n", i, j);
			bsg_lpc_step(i, j);
		}

		sprintf(tb, "lpc41_result_%d.txt", i);
		fd=fopen(tb, "wt");
		bsg_lpc_dump_best(fd);
		fclose(fd);
	}
}


int bsg_synth_vsb(int *vsb, int sn, int hm, short *bp, int cnt)
{
	double f, g;
	int i, j, k, l;

	for(i=0; i<cnt; i++)
	{
		bp[i]=0;

		g=0;
		for(j=0; j<FMTCHN; j++)
		{
			f=vsb[j]/(DCTBLKSZ*1.0);
			k=sn+i; l=j*FMTSZ+hm;
			g+=f*sin(M_PI*k*(l+0.5)/DCTBLKSZ);
		}
		bp[i]=(int)g;
	}
}

int dct_strip_synth_vsb(int *tb, short *bp, int th, int hm)
{
	int vsb[FMTCHN];
	dct_split_vsb(tb, vsb);
	bsg_synth_vsb(vsb, th, hm, bp, DCTBLKSZ);
}

int dct_gen_vec_samp2(int vb)
{
	char tb[256];
	int tdb[DCTBLKSZ];
	int vsb[FMTCHN];
	double f, g;
	int i, j, k, th, hm;

	IDCT_LinearBlock(spc_vec_blk[vb],
		spc_vec_blk_samp[vb], DCTBLKSZ);

#if 0
	dct_split_vsb(spc_vec_blk[vb], vsb);

	hm=FMTSZ/2; th=0;

#if 0
	hm=FMTSZ/2; g=0;
	for(i=0; i<FMTSZ; i++)
	{
		f=fabs(spc_vec_blk[vb][i]);
		if(f>g) { hm=i; g=f; }
	}

	th=0;
	g=1000000000.0;
	for(i=0; i<256; i++)
	{
		bsg_synth_vsb(vsb, i, hm, spc_vec_blk_samp2[vb], 64);
		f=0; 
		for(j=0; j<64; j++)
			f+=fabs(spc_vec_blk_samp2[vb][j]-
				spc_vec_blk_samp[vb][j]);
		if(f<g) { th=i; g=f; }
	}
#endif

	dct_strip_synth_vsb(spc_vec_blk[vb],
		spc_vec_blk_samp2[vb], th, hm);

#if 0
	sprintf(tb, "tst_vec/%d_n.wav", vb, i);
	WAVE_StoreWAV(tb, (byte *)spc_vec_blk_samp[vb],
		1, 16000, 16, DCTBLKSZ);

	sprintf(tb, "tst_vec/%d_th.wav", vb, i);
	WAVE_StoreWAV(tb, (byte *)spc_vec_blk_samp2[vb],
		1, 16000, 16, DCTBLKSZ);
#endif
#endif

	for(i=0; i<FMTCHN; i++)
	{
		for(j=0; j<DCTBLKSZ; j++)
			tdb[j]=spc_vec_blk[vb][j]*spc_vec_mask[i][j];
		IDCT_LinearBlock(tdb, spc_vec_samp[vb][i], DCTBLKSZ);

#if 0
		sprintf(tb, "tst_vec/%d_%d.wav", vb, i);
		WAVE_StoreWAV(tb, (byte *)spc_vec_samp[vb][i],
			1, 16000, 16, DCTBLKSZ);
#endif
	}
}

int dct_split_vec3_pick(int vb)
{
	int tb[DCTBLKSZ];
	double f, g;
	int i, j;

	i=rand()%spc_num; j=rand()%spc_bcnt[i];
	dct_copy(tb, spc_block[i]+j*DCTBLKSZ, DCTBLKSZ);
	dct_copy(spc_vec_blk[vb], tb, DCTBLKSZ);
}

int dct_split_vec3(int vb)
{
	int tb[DCTBLKSZ];
	double f, g;
	int i, j;

	dct_copy(tb, spc_vec_blk[vb], DCTBLKSZ);

	for(i=0; i<FMTCHN; i++)
	{
		for(j=0; j<DCTBLKSZ; j++)
			spc_vec[vb][i][j]=tb[j]*spc_vec_mask[i][j];
	}

	for(i=0; i<FMTCHN; i++)
	{
		f=0;
		for(j=0; j<DCTBLKSZ; j++)
			{ g=spc_vec[vb][i][j]; f+=g*g; }
		f=sqrt(f);

		for(j=0; j<DCTBLKSZ; j++)
		{
			g=spc_vec[vb][i][j]/f;
			spc_vec[vb][i][j]=g;
		}
	}
}

int dct_gen_vec()
{
	double tda[1024], f, g;
	int tdb[1024];
	short *bp;
	int i, j, k, l, n;

	spc_vsblock=malloc(4096*sizeof(int *));
	spc_vslin=malloc(4096*sizeof(int *));

	dct_init_mask();

#if 0
	for(i=0; i<DCTBLKSZ; i++)tda[i]=0;
	l=0;

	for(i=0; i<spc_num; i++)
	{
		n=spc_bcnt[i];
		n=1;

		for(j=0; j<n; j++)
		{
			for(k=0; k<DCTBLKSZ; k++)
				tda[k]+=spc_block[i][j*DCTBLKSZ+k];
			l++;
		}
	}

	for(i=0; i<DCTBLKSZ; i++)tdb[i]=tda[i]/l;

	dct_copy(spc_vec_blk[0], tdb, DCTBLKSZ);
//	dct_split_vec(tdb, 0);
#endif

	for(i=0; i<FMTCNT; i++)
		dct_split_vec3_pick(i);

	for(i=0; i<FMTCNT; i++)
		dct_split_vec3(i);

#if 1
	fprintf(stderr, "VecSamps:\n");
	for(i=0; i<FMTCNT; i++)
	{
		fprintf(stderr, "%d/%d\r", i+1, FMTCNT);
		dct_gen_vec_samp2(i);
	}
	fprintf(stderr, "\n");
#endif

//	dct_dump_vec(tdb);
}

#if 1
int main(int argc, char *argv[])
{
	double tda[1024], f, g;
	int tdb[1024], tdb0[1024], tdb1[1024], tdb2[1024], tdb3[1024];
	short bpb[4096];
	char tb[256];

	short *bp;
	int i, j, k, l, n;

#if (DCTBLKSZ==256)
	read_dct("blocks0.dct");
#endif

#if (DCTBLKSZ==512)
	read_dct("blocks1.dct");
#endif

#if (DCTBLKSZ==1024)
	read_dct("blocks2.dct");
#endif

	dct_gen_vec();

	bsg_lpc_evolve();

#if 0
	fprintf(stderr, "\n");

	for(i=0; i<spc_num; i++)
//	for(i=0; i<64; i++)
	{
		fprintf(stderr, "%d/%d %s    \r", i, spc_num, spc_names[i]);

		k=spc_bcnt[i];

		l=k*FMTCHN*sizeof(int);
		spc_vsblock[i]=malloc(l);

		for(j=0; j<k; j++)
		{
			dct_copy(tdb, spc_block[i]+j*DCTBLKSZ, DCTBLKSZ);
			dct_split_vsb(tdb, spc_vsblock[i]+j*FMTCHN);
		}
	}

	fprintf(stderr, "\n");

#endif

//	dct_dump_vsb();

//	for(i=0; i<spc_num; i++)
//		vsb_regen_samp(i);

#if 0
	fprintf(stderr, "\n");

	for(i=0; i<spc_num; i++)
//	for(i=0; i<16; i++)
	{
		fprintf(stderr, "%d/%d %s    \r", i, spc_num, spc_names[i]);

		k=spc_bcnt[i];

		l=(k+2)*DCTBLKSZ*sizeof(short);
		bp=malloc(l);
		memset(bp, 0, l);

		for(j=0; j<k; j++)
		{
			dct_copy(tdb, spc_block[i]+j*DCTBLKSZ, DCTBLKSZ);
//			dct_strip(tdb);
			IDCT_LinearBlock(tdb, bp+j*DCTBLKSZ, DCTBLKSZ);
			idct_soften(bp+j*DCTBLKSZ);

//			IMDCT_LinearBlock(tdb, bp+j*DCTBLKSZ, DCTBLKSZ);
		}

		sprintf(tb, "tst_wav3/%s.wav", spc_names[i]);
		WAVE_StoreWAV(tb, bp, 1, 16000, 16, k*DCTBLKSZ);
		free(bp);
	}

	fprintf(stderr, "\n");

#endif
}
#endif

