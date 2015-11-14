#include <lbxgl2.h>

#define BLKSZ	8
#define BLKSZ2	(BLKSZ*BLKSZ)

#define DCTSZ	8
#define DCTSZ2	(DCTSZ*DCTSZ)


//DCT

static float dct_cv[DCTSZ2];
static float dct_c[DCTSZ2];

static int dct_ci[DCTSZ2];
static int dct_cvi[DCTSZ2];
static int dct_cvi2[DCTSZ2*DCTSZ2];
static int dct_icvi2[DCTSZ2*DCTSZ2];

static int dct_qt[DCTSZ2];
static int dct_sorttab[DCTSZ2];
static int dct_clc=0;
static int dct_ldcv=0;		//last DC value, encoder
static int dct_ldcv2=0;		//last DC value, decoder

static float dct_k=1;
static float dct_q1;
static float dct_q2;

void LBXGL_VidFilterDCT_BeginFrame()
{
	dct_ldcv=0;
	dct_ldcv2=0;
}

void LBXGL_VidFilterDCT_SetQLevels(float k, float q1, float q2)
{
	if((k==dct_k) && (q1==dct_q1) && (q2==dct_q2))
		return;

	dct_k=k;
	dct_q1=q1;
	dct_q2=q2;

	LBXGL_VidFilterDCT_MakeQuantizerTable();
}

void LBXGL_VidFilterDCT_GetQuantTab(short *tab)
{
	int i;

	for(i=0; i<DCTSZ2; i++)
		tab[i]=dct_qt[i];
}

void LBXGL_VidFilterDCT_SetQuantTab(short *tab)
{
	int i;

	for(i=0; i<DCTSZ2; i++)
		dct_qt[i]=tab[i];
}

void LBXGL_VidFilterDCT_MakeSortTab()
{
	int i, j, k, l;

	i=0;
	j=0;
	l=1;
	for(k=0; k<DCTSZ2; k++)
	{
		dct_sorttab[i*DCTSZ+j]=k;
		if(l)
		{
			if(i==0)
			{
				j++;
				l=0;
			}else if(j==(DCTSZ-1))
			{
				i++;
				l=0;
			}else
			{
				i--;
				j++;
			}
		}else
		{
			if(i==(DCTSZ-1))
			{
				j++;
				l=1;
			}else if(j==0)
			{
				i++;
				l=1;
			}else
			{
				i++;
				j--;
			}
		}
	}

#if 0
	for(i=0; i<DCTSZ; i++)
	{
		for(j=0; j<DCTSZ; j++)
		{
//			printf("%3d ", sorttab[i*DCTSZ+j]);
			printf("%3d ", rsorttab[sorttab[i*DCTSZ+j]]);
		}
		printf("\n");
	}
#endif
}

void LBXGL_VidFilterDCT_MakeQuantizerTable()
{
	float c1[DCTSZ], f, f2, f3;
	int i, j, k, l;
	float u, v;
	float q1, q2, q3;

//	k=1;
//	q1=0.25;
//	q2=0.001;
//	q3=0.0000001;

	k=dct_k;
	q1=dct_q1;
	q2=dct_q2;
	q3=0;

	for(i=0; i<DCTSZ; i++)
	{
		for(j=0; j<DCTSZ; j++)
		{
			u=i*sqrt(i);
			v=j*sqrt(j);
//			u=i;
//			v=j;

//			f=sqrt(i*i+j*j)+1;
//			f=sqrt(i+j)+1;
//			f=1.00*sqrt(u)+1.00*sqrt(v)+1;

			f=q1*u+q1*v;
			f2=q2*(u*v);
			f3=q3*(u*u*v*v);

			dct_qt[i*DCTSZ+j]=k*(f+f2+f3+1);
//			dct_qt[i*DCTSZ+j]=4;

			if(j)kprint(" ");
			kprint("%4d", dct_qt[i*DCTSZ+j]);

//			dct_qt[i*DCTSZ+j]=sqrt(c1[i]*c1[i]+c1[j]*c1[j]);
		}
		kprint("\n");
	}
	kprint("\n");
}

void LBXGL_VidFilterDCT_Init()
{
	float c1[DCTSZ];
	int i, j, k, l;
	int t;
	float f;

	if(dct_clc)return;
	dct_clc=1;

	for(i=0; i<DCTSZ; i++)
		c1[i]=sqrt(2.0/DCTSZ);
	c1[0]=sqrt(1.0/DCTSZ);

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		dct_c[i*DCTSZ+j]=c1[i]*c1[j];
		dct_ci[i*DCTSZ+j]=c1[i]*c1[j]*256;
	}

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		f=cos((M_PI*i)/(2*DCTSZ)*(2*j+1));
		dct_cv[i*DCTSZ+j]=f;
		dct_cvi[i*DCTSZ+j]=f*256;
	}


	t=0;
	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		for(k=0; k<DCTSZ; k++)
			for(l=0; l<DCTSZ; l++)
		{
			f=cos((M_PI*i)/(2*DCTSZ)*(2*k+1))*
				cos((M_PI*j)/(2*DCTSZ)*(2*l+1));
			dct_cvi2[t]=f*256;

			f=dct_c[(k*DCTSZ)+l]*
				cos((M_PI*k)/(2*DCTSZ)*(2*i+1))*
				cos((M_PI*l)/(2*DCTSZ)*(2*j+1));
			dct_icvi2[t]=f*65536;

			t++;
		}
	}


	for(i=0; i<DCTSZ; i++)
		c1[i]=DCTSZ*((i+1)/2.0);

	LBXGL_VidFilterDCT_MakeQuantizerTable();
	LBXGL_VidFilterDCT_MakeSortTab();
}

#if 0
void LBXGL_VidFilter_TransDCT(byte *iblk, short *oblk)
{
	int i, j, k, l;
	float s;

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		s=0;
		for(k=0; k<DCTSZ; k++)
			for(l=0; l<DCTSZ; l++)
		{
//			s+=iblk[(k*8)+l]*
//				cos((M_PI*i)/(2*8)*(2*k+1))*
//				cos((M_PI*j)/(2*8)*(2*l+1));

			s+=iblk[(k*DCTSZ)+l]*
				dct_cv[(i*DCTSZ)+k]*dct_cv[(j*DCTSZ)+l];
		}
		s*=dct_c[(i*DCTSZ)+j];

//		xf_min=(s<xf_min)?s:xf_min;
//		xf_max=(s>xf_max)?s:xf_max;

		oblk[(i*DCTSZ)+j]=s;
	}
}
#endif

void LBXGL_VidFilter_TransDCT(byte *iblk, short *oblk)
{
	int i, j, k, l;
	int s;

	k=0;
	for(i=0; i<DCTSZ2; i++)
	{
		s=0;
		for(j=0; j<DCTSZ2; j++)
			s+=iblk[j]*dct_cvi2[k++];
		s*=dct_ci[i];
		oblk[i]=s>>16;
	}
}

void LBXGL_VidFilter_TransDCTQ(short *iblk, short *oblk)
{
	int i, j, k, l;
	int s;

	k=0;
	for(i=0; i<DCTSZ2; i++)
	{
		s=0;
		for(j=0; j<DCTSZ2; j++)
			s+=iblk[j]*dct_cvi2[k++];

		s*=dct_ci[i];

		l=dct_sorttab[i];
//		s=((s>>16)+(dct_qt[i]>>2))/dct_qt[i];
		s=(s>>16)/dct_qt[i];
		oblk[l]=s;
	}
}

#if 0
void LBXGL_VidFilter_TransIDCT(short *iblk, byte *oblk)
{
	int i, j, k, l;
	float s;

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		s=0;
		for(k=0; k<DCTSZ; k++)for(l=0; l<DCTSZ; l++)
		{
//			s+=c[(k*8)+l]*iblk[(k*8)+l]*
//				cos((M_PI*i)/(2*8)*(2*k+1))*
//				cos((M_PI*j)/(2*8)*(2*l+1));

			s+=dct_c[(k*DCTSZ)+l]*iblk[(k*DCTSZ)+l]*
				dct_cv[(k*DCTSZ)+i]*dct_cv[(l*DCTSZ)+j];
		}
		if(s<0)s=0;
		if(s>255)s=255;

		oblk[(i*DCTSZ)+j]=s;
	}
}
#endif

void LBXGL_VidFilter_TransIDCT(short *iblk, byte *oblk)
{
	int i, j, k, l;
	int s;

	k=0;
	for(i=0; i<DCTSZ2; i++)
	{
		s=0;
		for(j=0; j<DCTSZ2; j++)
			s+=iblk[j]*dct_icvi2[k++];
		if(s<0)s=0;
		if(s>((1<<24)-1))s=((1<<24)-1);
		oblk[i]=s>>16;
	}
}

void LBXGL_VidFilter_TransIDCTQ(short *iblk, short *oblk)
{
	int i, j, k, l;
	int s;

	k=0;
	for(i=0; i<DCTSZ2; i++)
	{
		s=0;
		for(j=0; j<DCTSZ2; j++)
		{
			l=dct_sorttab[j];
			l=iblk[l]*dct_qt[j];
			s+=l*dct_icvi2[k++];
		}
//		if(s<0)s=0;
//		if(s>((1<<24)-1))s=((1<<24)-1);
		oblk[i]=s>>16;
	}
}

//static short quant_lblk[DCTSZ2];
//static short dequant_lblk[DCTSZ2];

int LBXGL_VidFilterDCT_QuantBlock(short *iblk, short *oblk)
{
	int i, j, k, l;

	for(i=0; i<DCTSZ2; i++)
	{
		j=dct_sorttab[i];
		k=(iblk[i]+(dct_qt[i]>>2))/dct_qt[i];
//		k=iblk[i]/qt[i];

		if(!i)
		{
			l=k;
			k-=dct_ldcv;
			dct_ldcv=l;
		}

		oblk[j]=k;

//		oblk[j]=k-quant_lblk[j];
//		quant_lblk[j]=k;
	}
}

int LBXGL_VidFilterDCT_DeQuantBlock(short *iblk, short *oblk)
{
	int i, j, k, l;

	for(i=0; i<DCTSZ2; i++)
	{
		j=dct_sorttab[i];
		k=iblk[j]*dct_qt[i];

		if(!i)
		{
			k+=dct_ldcv2;
			dct_ldcv2=k;
		}

//		k+=dequant_lblk[j];
//		dequant_lblk[j]=k;
		oblk[i]=k;
	}

}

//Motion Prediction

void LBXGL_VidFilter_CopyBlockFromImg(byte *blk, int ox, int oy,
	byte *img, int xs, int ys)
{
	int i, j, x, y, x0, y0, x1, y1;

	x0=ox;
	y0=oy;
	x1=ox+BLKSZ;
	y1=oy+BLKSZ;

	memset(blk, 0, BLKSZ2);

	if(((x1<=0) || (y1<=0)) || ((x0>=xs) || (y0>=ys)))
		return;

	if(x0<0)x0=0;
	if(y0<0)y0=0;
	if(x1>xs)x1=xs;
	if(y1>ys)y1=ys;

	for(i=y0; i<y1; i++)
		for(j=x0; j<x1; j++)
	{
		x=j-ox;
		y=i-oy;

		blk[(y*BLKSZ)+x]=img[i*xs+j];
	}
}

void LBXGL_VidFilter_CopyBlockToImg(byte *blk, int ox, int oy,
	byte *img, int xs, int ys)
{
	int i, j, x, y, x0, y0, x1, y1;

	x0=ox;
	y0=oy;
	x1=ox+BLKSZ;
	y1=oy+BLKSZ;

	if(((x1<=0) || (y1<=0)) || ((x0>=xs) || (y0>=ys)))
		return;

	if(x0<0)x0=0;
	if(y0<0)y0=0;
	if(x1>xs)x1=xs;
	if(y1>ys)y1=ys;

	for(i=y0; i<y1; i++)
		for(j=x0; j<x1; j++)
	{
		x=j-ox;
		y=i-oy;

		img[i*xs+j]=blk[(y*BLKSZ)+x];
	}
}

int LBXGL_VidFilter_DeltaBlockImg(byte *blk, int ox, int oy,
	byte *img, int xs, int ys)
{
	int i, j, k, l, x, y;
	int x0, y0, x1, y1, d;

	x0=ox;
	y0=oy;
	x1=ox+BLKSZ;
	y1=oy+BLKSZ;

	if(((x1<=0) || (y1<=0)) || ((x0>=xs) || (y0>=ys)))
	{
		return(BLKSZ2*255);

#if 0
		d=0;
		for(i=0; i<BLKSZ; i++)
			for(j=0; j<BLKSZ; j++)
			d+=blk[(i*BLKSZ)+j];
		return(d);
#endif
	}

//	d=0;

	if(x0<0)x0=0;
	if(y0<0)y0=0;
	if(x1>xs)x1=xs;
	if(y1>ys)y1=ys;

	d=(BLKSZ2*255)-((x1-x0)*(y1-y0)*255);

#if 0
	if(x0<0)
	{
		for(i=0; i<BLKSZ; i++)
			for(j=0; j<(x0-ox); j++)
			d+=blk[(i*BLKSZ)+j];
		x0=0;
	}
	if(y0<0)
	{
		for(i=(y0-oy); i<BLKSZ; i++)
			for(j=0; j<BLKSZ; j++)
			d+=blk[(i*BLKSZ)+j];
		y0=0;
	}
	if(x1>xs)
	{
		for(i=0; i<BLKSZ; i++)
			for(j=(x1-xs); j<BLKSZ; j++)
			d+=blk[(i*BLKSZ)+j];
		x0=0;
		x1=xs;
	}
	if(y1>ys)
	{
		for(i=(y1-ys); i<BLKSZ; i++)
			for(j=0; j<BLKSZ; j++)
			d+=blk[(i*BLKSZ)+j];
		y1=ys;
	}
#endif

	for(i=y0; i<y1; i++)
		for(j=x0; j<x1; j++)
	{
		x=j-ox;
		y=i-oy;

		k=img[i*xs+j]-blk[(y*BLKSZ)+x];
		d+=(k>=0)?k:-k;

//		k=(k>=0)?k:-k;
//		l=k-256;
//		l=(l>=0)?l:-l;
//		d+=(k<l)?k:l;
	}
	return(d);
}

int LBXGL_VidFilter_DeltaImgImg(int ox, int oy, int w, int h,
	byte *img0, byte *img1, int xs, int ys)
{
	int i, j, k, l, x0, y0, x1, y1, d;

	x0=ox;
	y0=oy;
	x1=ox+w;
	y1=oy+h;

	if(((x1<=0) || (y1<=0)) || ((x0>=xs) || (y0>=ys)))
		return(0);

	if(x0<0)x0=0;
	if(y0<0)y0=0;
	if(x1>xs)x1=xs;
	if(y1>ys)y1=ys;

	d=0;
	for(i=y0; i<y1; i++)
		for(j=x0; j<x1; j++)
	{
		k=img1[i*xs+j]-img0[i*xs+j];
		d+=(k>=0)?k:-k;

//		k=(k>=0)?k:-k;
//		l=k-256;
//		l=(l>=0)?l:-l;
//		d+=(k<l)?k:l;
	}
	return(d);
}

//predict motion in terms of 8x8 blocks
//assume mono (eg: post decorelation)
void LBXGL_VidFilter_PredictMotion(byte *buf0, byte *buf1, int xs, int ys,
	char *deltas, int *num)
{
	byte blk[BLKSZ2];
	int x0, y0, x1, y1;
	int i, j, k, l, d0, d1;
	int bx, by, bd;
	int n, dn;

	n=0;
	dn=0;
	for(i=0; i<(ys/BLKSZ); i++)
		for(j=0; j<(xs/BLKSZ); j++)
	{
		LBXGL_VidFilter_CopyBlockFromImg(blk,
			j*BLKSZ, i*BLKSZ, buf0, xs, ys);

		x0=(j*BLKSZ)-16;
		y0=(i*BLKSZ)-16;
		x1=(j*BLKSZ)+16;
		y1=(i*BLKSZ)+16;

		if(x0<0)x0=0;
		if(y0<0)y0=0;
		if(x1>xs)x1=xs;
		if(y1>ys)y1=ys;

		bx=(j*BLKSZ);
		by=(i*BLKSZ);
		bd=BLKSZ2*255;

#if 0
		for(k=y0; k<=y1; k+=16)
			for(l=x0; l<=x1; l+=16)
		{
//			d0=LBXGL_VidFilter_DeltaImgImg(l, k, BLKSZ, BLKSZ,
//				buf0, buf1, xs, ys);
			d1=LBXGL_VidFilter_DeltaBlockImg(blk, l, k, buf1, xs, ys);

//			if((d1<d0) && (d1<bd))
			if(d1<bd)
			{
				bx=l;
				by=k;
				bd=d1;
			}
		}

		x0=bx-8;
		y0=by-8;
		x1=bx+8;
		y1=by+8;
#endif

#if 1
		for(k=y0; k<=y1; k+=8)
			for(l=x0; l<=x1; l+=8)
		{
			d1=LBXGL_VidFilter_DeltaBlockImg(blk, l, k, buf1, xs, ys);

			if(d1<bd)
			{
				bx=l;
				by=k;
				bd=d1;
			}
		}

		x0=bx-4;
		y0=by-4;
		x1=bx+4;
		y1=by+4;
#endif

		for(k=y0; k<=y1; k+=2)
			for(l=x0; l<=x1; l+=2)
		{
			d1=LBXGL_VidFilter_DeltaBlockImg(blk, l, k, buf1, xs, ys);

			if(d1<bd)
			{
				bx=l;
				by=k;
				bd=d1;
			}
		}


		bx-=j*BLKSZ;
		by-=i*BLKSZ;

#if 1
		d0=LBXGL_VidFilter_DeltaImgImg(j*BLKSZ, i*BLKSZ, BLKSZ, BLKSZ,
			buf0, buf1, xs, ys);
		if(d0<=bd)
		{
			bx=0;
			by=0;
		}
#endif


//		if((bx!=0) || (by!=0))
//		{
//			kprint("b %d (%d %d) %d\n", n, bx, by, bd);
//			dn++;
//		}

		deltas[n*2+0]=bx;
		deltas[n*2+1]=by;
		n++;
	}
	if(num)*num=n;

//	kprint("delta %d blocks\n", dn);
}

void LBXGL_VidFilter_DummyMotion(int xs, int ys, char *deltas, int *num)
{
	int i, j, k, l;
	int n, dn;

	n=0;
	dn=0;
	for(i=0; i<(ys/BLKSZ); i++)
		for(j=0; j<(xs/BLKSZ); j++)
	{
		deltas[n*2+0]=0;
		deltas[n*2+1]=0;
		n++;
	}
	if(num)*num=n;
}

void LBXGL_VidFilter_ApplyMotion(byte *ibuf, byte *obuf, int xs, int ys,
	char *deltas, int num)
{
	byte blk[BLKSZ2];
	int i, j, k;
	int x, y, n;

	n=0;
	for(i=0; i<(ys/BLKSZ); i++)
		for(j=0; j<(xs/BLKSZ); j++)
	{
		if((!deltas[n*2+0]) && (!deltas[n*2+1]))
		{
			n++;
			continue;
		}

		LBXGL_VidFilter_CopyBlockFromImg(blk,
			j*BLKSZ, i*BLKSZ, ibuf, xs, ys);

//		memset(blk, 0, BLKSZ2);

		x=(j*BLKSZ)+deltas[n*2+0];
		y=(i*BLKSZ)+deltas[n*2+1];

		LBXGL_VidFilter_CopyBlockToImg(blk, x, y, obuf, xs, ys);

		n++;
	}
}

void LBXGL_VidFilter_ApplyMotion2(byte *ibuf, byte *obuf, int xs, int ys,
	char *deltas, int num)
{
	byte blk[BLKSZ2];
	int i, j, k;
	int x, y, n;

	n=0;
	for(i=0; i<(ys/BLKSZ); i++)
		for(j=0; j<(xs/BLKSZ); j++)
	{
		if((!deltas[n*2+0]) && (!deltas[n*2+1]))
		{
			n++;
			continue;
		}

		x=(j*BLKSZ)+deltas[n*2+0];
		y=(i*BLKSZ)+deltas[n*2+1];

		LBXGL_VidFilter_CopyBlockFromImg(blk, x, y, ibuf, xs, ys);

//		memset(blk, 0, BLKSZ2);

		LBXGL_VidFilter_CopyBlockToImg(blk,
			j*BLKSZ, i*BLKSZ, obuf, xs, ys);

		n++;
	}
}

void LBXGL_VidFilter_GenDeltaBlocks(byte *buf0, byte *buf1, int xs, int ys,
	char *deltas, short *obuf, int num)
{
	byte blk0[BLKSZ2];
	byte blk1[BLKSZ2];

	int i, j, k;
	int x, y, n;

	n=0;
	for(i=0; i<(ys/BLKSZ); i++)
		for(j=0; j<(xs/BLKSZ); j++)
	{
		x=(j*BLKSZ)+deltas[n*2+0];
		y=(i*BLKSZ)+deltas[n*2+1];

		LBXGL_VidFilter_CopyBlockFromImg(blk0, x, y, buf0, xs, ys);

//		memset(blk, 0, BLKSZ2);

		LBXGL_VidFilter_CopyBlockFromImg(blk1,
			j*BLKSZ, i*BLKSZ, buf1, xs, ys);

		for(k=0; k<BLKSZ2; k++)
			obuf[n*BLKSZ2+k]=blk1[k]-blk0[k];
//			obuf[n*BLKSZ2+k]=blk1[k];

//		LBXGL_VidFilter_CopyBlockToImg(blk,
//			j*BLKSZ, i*BLKSZ, obuf, xs, ys);

		n++;
	}
}

void LBXGL_VidFilter_ApplyDeltaBlocks(byte *buf0, byte *buf1, int xs, int ys,
	char *deltas, short *ibuf, int num)
{
	byte blk0[BLKSZ2];
	byte blk1[BLKSZ2];

	int i, j, k, l;
	int x, y, n;

	n=0;
	for(i=0; i<(ys/BLKSZ); i++)
		for(j=0; j<(xs/BLKSZ); j++)
	{
		x=(j*BLKSZ)+deltas[n*2+0];
		y=(i*BLKSZ)+deltas[n*2+1];

		LBXGL_VidFilter_CopyBlockFromImg(blk0, x, y, buf0, xs, ys);

//		memset(blk, 0, BLKSZ2);

//		LBXGL_VidFilter_CopyBlockFromImg(blk1,
//			j*BLKSZ, i*BLKSZ, buf1, xs, ys);

		for(k=0; k<BLKSZ2; k++)
		{
			l=ibuf[n*BLKSZ2+k]+blk0[k];
//			l=blk0[k];
//			l=ibuf[n*BLKSZ2+k]+128;
			if(l<0)l=0;
			if(l>255)l=255;
			blk1[k]=l;

//			blk1[k]=ibuf[n*BLKSZ2+k];
		}

		LBXGL_VidFilter_CopyBlockToImg(blk1,
			j*BLKSZ, i*BLKSZ, buf1, xs, ys);

		n++;
	}
}
