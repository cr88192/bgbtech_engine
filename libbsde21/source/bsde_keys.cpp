#include "bsde_internal.h"

/*
Flags:
	low 8=array size (bsde_real and int arrays)
	mid 4=type:
		0=unused
		1=int
		2=bsde_real
		3=int vec
		4=bsde_real vec
 */

int *bsde_key_key;	//user keys (sorted)
int *bsde_key_idx;	//index (bsde_reals, vectors, ...), value (int)
short *bsde_key_flag;	//flags

int *bsde_key_iarr;	//int array (int vectors)
bsde_real *bsde_key_farr;	//bsde_real array (bsde_real vectors)

int bsde_key_nkey, bsde_key_mkey;
int bsde_key_nint, bsde_key_mint;
int bsde_key_nflt, bsde_key_mflt;


int BSDE_FindKeyPos(int key)
{
	int b, r;
	int i, j;

	if(!bsde_key_nkey)return(-1);

	b=0; r=bsde_key_nkey;
	for(i=0; i<12; i++)
	{
		j=b+(r>>1);
		if(use_key[j]==key)
			return(j);
		if(key>use_key[j])b=j;
		r>>=1;
		if(!r)break;
	}
	return(-1);
}

void BSDE_CheckExpandKey()
{
	int i, j;

	if(!bsde_key_mkey)
	{
		j=16;
		bsde_key_key=malloc(j*sizeof(int));
		bsde_key_idx=malloc(j*sizeof(int));
		bsde_key_flag=malloc(j*sizeof(int));
		bsde_key_mkey=j;
		return;
	}

	i=bsde_key_nkey+1;
	if(i>=bsde_key_mkey)
	{
		j=bsde_key_mkey+(bsde_key_mkey>>1);
		bsde_key_key=realloc(bsde_key_key, j*sizeof(int));
		bsde_key_idx=realloc(bsde_key_idx, j*sizeof(int));
		bsde_key_flag=realloc(bsde_key_flag, j*sizeof(short));
	}
}

int BSDE_GetKeyPos(int key)
{
	int b, r;
	int i, j;

	if(!bsde_key_nkey)
	{
		BSDE_CheckExpandKey();

		return(-1);
	}

	b=0; r=bsde_key_nkey;

	if(key>bsde_key_key[r-1])
	{
		BSDE_CheckExpandKey();
		i=bsde_key_nkey++;
		bsde_key_key[i]=key;
		bsde_key_idx[i]=0;
		bsde_key_flag[i]=0;
	}

	if(key==bsde_key_key[r-1])
		return(r-1);

	for(i=0; i<20; i++)
	{
		j=b+(r>>1);
		if(bsde_key_key[j]==key)
			return(j);
		if(key>bsde_key_key[j])b=j+1;
		r>>=1;
		if(!r)break;
	}

	BSDE_CheckExpandKey();

	i=bsde_key_nkey++;
	for(j=i; j>b; j--)
	{
		bsde_key_key[j]=bsde_key_key[j-1];
		bsde_key_idx[j]=bsde_key_idx[j-1];
		bsde_key_flag[j]=bsde_key_flag[j-1];
	}
	bsde_key_key[b]=key;
	bsde_key_idx[b]=0;
	bsde_key_flag[b]=0;

	return(-1);
}


int BSDE_AllocKeyFloats(int cnt)
{
	bsde_real *flt;
	int i, j, k, l, n, m;


	if(!bsde_key_mflt)
	{
		m=256;
		flt=malloc(m*sizeof(bsde_real));
		bsde_key_mflt=m;
		bsde_key_nflt=0;
	}

	if((bsde_key_nflt+cnt)>=bsde_key_mflt)
	{
		m=bsde_key_mflt;
		while((bsde_key_nflt+cnt)>=m)m=m+(m>>1);

		flt=malloc(m*sizeof(bsde_real));

		n=0;
		for(i=0; i<bsde_key_nkey; i++)
		{
			j=(bsde_key_flag[i]>>8)&0xF;

			if(j==2)
			{
				k=bsde_key_idx[i];
				flt[n]=bsde_key_farr[k];
				bsde_key_idx[i]=n++;
				continue;
			}

			if(j==4)
			{
				k=bsde_key_idx[i];
				l=bsde_key_flag[i]&0xFF;
				for(j=0; j<l; j++)
					flt[n+j]=bsde_key_farr[k+j];
				bsde_key_idx[i]=n;
				n+=l;
				continue;
			}
		}

		free(bsde_key_farr);
		bsde_key_farr=flt;
		bsde_key_mflt=m;
		bsde_key_nflt=n;
	}

	i=bsde_key_nflt;
	bsde_key_nflt+=cnt;
	return(i);
}

int BSDE_AllocKeyInts(int cnt)
{
	bsde_real *ilt;
	int i, j, k, l, n, m;


	if(!bsde_key_mint)
	{
		m=256;
		ilt=malloc(m*sizeof(bsde_real));
		bsde_key_mint=m;
		bsde_key_nint=0;
	}

	if((bsde_key_nint+cnt)>=bsde_key_mint)
	{
		m=bsde_key_mint;
		while((bsde_key_nint+cnt)>=m)m=m+(m>>1);

		ilt=malloc(m*sizeof(bsde_real));

		n=0;
		for(i=0; i<bsde_key_nkey; i++)
		{
			j=(bsde_key_flag[i]>>8)&0xF;

			if(j==3)
			{
				k=bsde_key_idx[i];
				l=bsde_key_flag[i]&0xFF;
				for(j=0; j<l; j++)
					ilt[n+j]=bsde_key_iarr[k+j];
				bsde_key_idx[i]=n;
				n+=l;
				continue;
			}
		}

		free(bsde_key_iarr);
		bsde_key_iarr=ilt;
		bsde_key_mint=m;
		bsde_key_nint=n;
	}

	i=bsde_key_nint;
	bsde_key_nint+=cnt;
	return(i);
}


int BSDE_GetKeyI(int key)
{
	int i, j;

	i=BSDE_FindKeyPos(key);
	if(i<0)return(0);

	j=(bsde_key_flag[i]>>8)&0xF;
	if(i==1)return(bsde_key_idx[i]);
	if(i==2)return(bsde_key_farr[bsde_key_idx[i]]);
}

void BSDE_SetKeyI(int key, int val)
{
	int i;

	i=BSDE_GetKeyPos(key);
	bsde_key_idx[i]=val;
	bsde_key_flag[i]=256;
}

bsde_real BSDE_GetKeyF(int key)
{
	int i, j;

	i=BSDE_FindKeyPos(key);
	if(i<0)return(0);

	j=(bsde_key_flag[i]>>8)&0xF;
	if(i==1)return(bsde_key_idx[i]);
	if(i==2)return(bsde_key_farr[bsde_key_idx[i]]);
}

void BSDE_SetKeyF(int key, bsde_real val)
{
	int i, j;

	i=BSDE_GetKeyPos(key);

	j=(bsde_key_flag[i]>>8)&0xF;

	if(j!=2)
	{
		bsde_key_idx[i]=BSDE_AllocKeyFloats(1);
		bsde_key_flag[i]=512;
	}

	bsde_key_farr[bsde_key_idx[i]]=val;
}

