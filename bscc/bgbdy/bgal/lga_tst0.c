#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#define LGA_NCTX	4096

typedef unsigned char byte;

struct lga_context_s {
float *vec;	//vector array
int *ridx;	//vector-array reverse indices

float *err;	//vector error
int *idx;	//vector-array indices

int rov;	//member alloc rover
int pop;	//current population

int vsz;	//vector size
int maxpop;	//max population
int gp;		//greater-predicate

float mrate;
float merr[16];	//mutation-rate error
float mval[16];	//mutation rate values
};


struct lga_context_s **lga_ctx_arr=NULL;
struct lga_context_s *lga_ctx=NULL;
int lga_nctx;

float lga_ssqrt(float f)
	{ if(f<0)return(-sqrt(-f)); return(sqrt(f)); }
float lga_nfrand()
	{ return(rand()/((float)RAND_MAX)); }
float lga_sfrand()
	{ return(rand()/(RAND_MAX*0.5)-1.0); }
float lga_sqrrand()
	{ return(sqrt(lga_nfrand()*65536)/256); }
float lga_ssqrrand()
	{ return(lga_ssqrt(lga_sfrand()*65536)/256); }

float lga_logrand()
	{ return(log2(lga_nfrand()*65535+1)/16); }

void *lga_malloc(int sz)
{
	void *p;
	p=malloc(sz);
	memset(p, 0, sz);
}

void *lga_realloc(void *p, int sz)
{
	return(realloc(p, sz));
}

int lga_newctx(int pop, int vsz)
{
	int i, ssz, sszp;

	ssz=sizeof(struct lga_context_s);
	sszp=sizeof(struct lga_context_s *);

	lga_ctx=lga_malloc(ssz);

	lga_ctx->maxpop=pop;
	lga_ctx->vsz=vsz;
	lga_ctx->mrate=1;

	for(i=0; i<16; i++)
	{
		lga_ctx->merr[i]=999999999999.0;
		lga_ctx->mval[i]=1;
	}

	lga_ctx->vec=lga_malloc(pop*vsz*sizeof(float));
	lga_ctx->err=lga_malloc(pop*sizeof(float));
	lga_ctx->idx=lga_malloc(pop*sizeof(int));
	lga_ctx->ridx=lga_malloc(pop*sizeof(int));


	if(!lga_ctx_arr)
	{
		lga_ctx_arr=lga_malloc(4096*sszp);
		lga_nctx=4096;
	}

	for(i=0; i<lga_nctx; i++)
		if(!lga_ctx_arr[i])
	{
		lga_ctx_arr[i]=lga_ctx;
		return(i);
	}

	i=lga_nctx;
	lga_nctx+=lga_nctx>>1;

	lga_ctx_arr=lga_realloc(lga_ctx_arr, lga_nctx*sszp);
	memset(lga_ctx_arr+i, 0, (lga_nctx-i)*sszp);

	lga_ctx_arr[i]=lga_ctx;
	return(i);
}

int lga_newctx_max(int pop, int vsz)
{
	int i;

	i=lga_newctx(pop, vsz);
	lga_ctx->gp=1;
	return(i);
}

int lga_bind(int ctx)
{
	if((ctx<0) || (ctx>=lga_nctx))
	{
		lga_ctx=NULL;
		return(-1);
	}

	lga_ctx=lga_ctx_arr[ctx];
	return(lga_ctx?0:-1);
}


void lga_lpc_swap(int a, int b)
{
	int i, j;
	float f, g;

	if(a==b)return;

	f=lga_ctx->err[a];
	g=lga_ctx->err[b];
	lga_ctx->err[a]=g;
	lga_ctx->err[b]=f;

	i=lga_ctx->idx[a];
	j=lga_ctx->idx[b];
	lga_ctx->idx[a]=j;
	lga_ctx->idx[b]=i;
}

void lga_breed_raw(float *pfa, float *pfb, float *pfc, int sz, float m)
{
	float f, g;
	int i;

	for(i=0; i<sz; i++)
	{
		f=lga_nfrand();
		g=pfa[i]*(1-f) + pfb[i]*f;

		g+=lga_ssqrrand()*0.1*m;

		f=lga_nfrand();
		if(f<(0.05*m))g*=(lga_ssqrt(lga_sfrand()*1.5)*m) + (1-m);

		pfc[i]=g;
	}
}

#if 0
void lga_remove_idx(int n)
{
	int i, j, k, l;

	for(j=0; j<lga_ctx->pop; j++)
	{
		if(lga_ctx->idx[j]==n)
		{
//			i=lga_ctx->pop--;
//			memmove(lga_ctx->err+j, lga_ctx->err+j+1, (i-j)*sizeof(float));
//			memmove(lga_ctx->idx+j, lga_ctx->idx+j+1, (i-j)*sizeof(int));

			for(k=j; k<(lga_ctx->pop-1); k++)
			{
				lga_ctx->err[k]=lga_ctx->err[k+1];
//				lga_ctx->idx[k]=lga_ctx->idx[k+1];

				l=lga_ctx->idx[k+1];
				lga_ctx->idx[k]=l;
				lga_ctx->ridx[l]=k;
			}
			break;
		}
	}

	i=lga_ctx->pop--;
}
#endif

void lga_remove_idx(int n)
{
	int i, j, k, l;

	j=lga_ctx->ridx[n];

	i=(--lga_ctx->pop);
	for(k=j; k<i; k++)
	{
		lga_ctx->err[k]=lga_ctx->err[k+1];

		l=lga_ctx->idx[k+1];
		lga_ctx->idx[k]=l;
		lga_ctx->ridx[l]=k;
	}
}


void lga_insert_idx_min(float w, int n)
{
	int i, j, k, l;

	j=0; k=lga_ctx->pop>>1;
	while(k)
	{
		if(w>lga_ctx->err[j+k])
			j+=k;
		k>>=1;
	}

//	for(j=0; j<lga_ctx->pop; j++)
	for(; j<lga_ctx->pop; j++)
		if(w<=lga_ctx->err[j])
			break;

	i=lga_ctx->pop++;
	for(k=i; k>j; k--)
	{
		lga_ctx->err[k]=lga_ctx->err[k-1];

		l=lga_ctx->idx[k-1];
		lga_ctx->idx[k]=l;
		lga_ctx->ridx[l]=k;
	}

//	memmove(lga_ctx->err+j+1, lga_ctx->err+j, (i-j)*sizeof(float));
//	memmove(lga_ctx->idx+j+1, lga_ctx->idx+j, (i-j)*sizeof(int));

	lga_ctx->err[j]=w;
	lga_ctx->idx[j]=n;
	lga_ctx->ridx[n]=j;


	j=rand()&15;
	for(k=j; k<15; k++)
	{
		lga_ctx->merr[k]=lga_ctx->merr[k+1];
		lga_ctx->mval[k]=lga_ctx->mval[k+1];
	}

	for(j=0; j<15; j++)
		if(w<=lga_ctx->merr[j])
			break;

	for(k=15; k>j; k--)
	{
		lga_ctx->merr[k]=lga_ctx->merr[k-1];
		lga_ctx->mval[k]=lga_ctx->mval[k-1];
	}

	lga_ctx->merr[j]=w;
	lga_ctx->mval[j]=lga_ctx->mrate;
}

void lga_insert_idx_max(float w, int n)
{
	int i, j, k, l;

	j=0; k=lga_ctx->pop>>1;
	while(k)
	{
		if(w<lga_ctx->err[j+k])j+=k;
		k>>=1;
	}

	for(; j<lga_ctx->pop; j++)
		if(w>=lga_ctx->err[j])
			break;

	i=lga_ctx->pop++;
	for(k=i; k>j; k--)
	{
		lga_ctx->err[k]=lga_ctx->err[k-1];

		l=lga_ctx->idx[k-1];
		lga_ctx->idx[k]=l;
		lga_ctx->ridx[l]=k;
	}

	lga_ctx->err[j]=w;
	lga_ctx->idx[j]=n;
	lga_ctx->ridx[n]=j;



	j=rand()&15;
	for(k=j; k<15; k++)
	{
		lga_ctx->merr[k]=lga_ctx->merr[k+1];
		lga_ctx->mval[k]=lga_ctx->mval[k+1];
	}

	for(j=0; j<15; j++)
		if(w>=lga_ctx->merr[j])
			break;

	for(k=15; k>j; k--)
	{
		lga_ctx->merr[k]=lga_ctx->merr[k-1];
		lga_ctx->mval[k]=lga_ctx->mval[k-1];
	}

	lga_ctx->merr[j]=w;
	lga_ctx->mval[j]=lga_ctx->mrate;
}

void lga_insert(float *pf, float w)
{
	int i, j, k;

	i=lga_ctx->rov;
	lga_ctx->rov=(lga_ctx->rov+1)%lga_ctx->maxpop;

	if(lga_ctx->pop>=lga_ctx->maxpop)
		lga_remove_idx(i);

	for(j=0; j<lga_ctx->vsz; j++)
		lga_ctx->vec[i*lga_ctx->vsz+j]=pf[j];

	if(lga_ctx->gp)lga_insert_idx_max(w, i);
		else lga_insert_idx_min(w, i);
}

void lga_populate(float e)
{
	int i, j, k;
	float w;

	if(lga_ctx->gp)w=-999999999999.0;
		else w=999999999999.0;

	while(lga_ctx->pop<lga_ctx->maxpop)
	{
		j=lga_ctx->rov;
		lga_ctx->rov=(lga_ctx->rov+1)%lga_ctx->maxpop;

//		lga_remove_idx(j);

		for(k=0; k<lga_ctx->vsz; k++)
			lga_ctx->vec[j*lga_ctx->vsz+k]=lga_ssqrrand()*e;

		i=lga_ctx->pop++;
		lga_ctx->err[i]=w;
		lga_ctx->idx[i]=j;
	}
}

void lga_breed(float *pf)
{
	int i, j, k;
	float f, g, m;

	if(!lga_ctx)return;

	if(lga_ctx->pop<16)
	{
		for(i=0; i<lga_ctx->vsz; i++)
			pf[i]=lga_ssqrrand()*10;
		return;
	}

	if(lga_ctx->gp)
	{
		i=lga_sqrrand()*4; j=lga_sqrrand()*4;
		f=(lga_ctx->mval[i]+lga_ctx->mval[j])*0.5;
		f=f+lga_sfrand()*0.01*f;

		m=(lga_ctx->mrate+f)/2;
		if(m<0)m=0; if(m>1)m=1;

		lga_ctx->mrate=m;
	}else
	{
//		f=lga_ctx->err[0]/lga_ctx->err[15];
//		g=1.0/(lga_ctx->err[0]+1);
//		g=(1.1-f)/(lga_ctx->err[0]+1);
//		m=1.0-g;

		i=lga_sqrrand()*4; j=lga_sqrrand()*4;
		f=(lga_ctx->mval[i]+lga_ctx->mval[j])*0.5;
//		f=f+lga_sfrand()*0.01*pow(f, 0.75);
//		f=f+lga_sfrand()*0.01*pow(f, 1-f);
//		f=f+lga_sfrand()*0.01*pow(f, 1-sqrt(f));
		f=f+lga_sfrand()*0.01*f;

//		g=1.0/(lga_ctx->err[0]/lga_ctx->vsz+1);
//		m=(lga_ctx->mrate+f+(1.0-g))/3;
		m=(lga_ctx->mrate+f)/2;
//		m=f;
		if(m<0)m=0; if(m>1)m=1;

//		g=1.0/(lga_ctx->err[0]/lga_ctx->vsz+1);
//		m=1.0-g;

//		m=m*0.33+f*0.66;

		lga_ctx->mrate=m;
	}


//	i=lga_sqrrand()*lga_ctx->pop;
//	j=lga_sqrrand()*lga_ctx->pop;
//	if(i>=lga_ctx->pop)i=lga_ctx->pop-1;
//	if(j>=lga_ctx->pop)j=lga_ctx->pop-1;

	i=lga_sqrrand()*(lga_ctx->pop/16);
	j=lga_sqrrand()*(lga_ctx->pop/16);

//	i=lga_logrand()*lga_ctx->pop;
//	j=lga_logrand()*lga_ctx->pop;
//	if(i>=lga_ctx->pop)i=lga_ctx->pop-1;
//	if(j>=lga_ctx->pop)j=lga_ctx->pop-1;

	i=lga_ctx->idx[i];
	j=lga_ctx->idx[j];

	k=lga_ctx->vsz;
	lga_breed_raw(lga_ctx->vec+i*k, lga_ctx->vec+j*k, pf, k, m);
}


int main()
{
	float v0[64];
	float e, f, g;
	int i, j;

	lga_newctx(4096, 64);
//	lga_newctx(256, 64);
	lga_populate(10);

	for(i=0; i<(1<<24); i++)
	{
		lga_breed(v0);

		e=0;
		for(j=0; j<64; j++)
		{
			f=sin(2*M_PI*j/64);
			e+=fabs(v0[j]-f);
		}

		lga_insert(v0, e);

		if(!(i%1024))
		{
			printf("%d %f %f   \r", i, e, lga_ctx->mrate);
			fflush(stdout);
		}

		if(e<0.0001)break;
	}

	printf("\n");
	for(j=0; j<64; j++)
	{
		printf("%f ", v0[j]);
	}
	printf("\n");
}

