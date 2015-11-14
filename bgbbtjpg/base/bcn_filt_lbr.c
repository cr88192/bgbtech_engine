/*
Lossy Block Reduction Filter.

Goal: Reduce the number of distinct pixel blocks such that subsequent compression of DXT output will compress better.

This is done as an image pre-filter rather than as part of the block-encoding process mostly to generalize over some of the subsequent encoding steps.

Note: May write over the edges or past the end of the image if it is not evenly divisible by the block size.

This estimates the block similarily mostly via MSE (Mean Square Error).
Tests show that this will introduce blocky artifacts with gradients.
 */

#include <bgbbtj.h>

// #define BLKSZ	8
// #define BLKSZ	2
#define BLKSZ	4
#define BLKSZ2	(BLKSZ*BLKSZ)

int *filtlbr_block_vals;
int *filtlbr_block_yval;
byte *filtlbr_block_cvals;
short *filtlbr_block_cyval;
short *filtlbr_block_ychain;
int *filtlbr_block_cnt;
int *filtlbr_block_ucnt;
int filtlbr_n_blocks;

int filtlbr_chains[1024];

int filtlbr_xs;
int filtlbr_ys;

void BGBBTJ_FiltLBR_GetIndexBlock(int idx, byte *buf)
{
//	int i, n;

	memcpy(buf, filtlbr_block_cvals+(idx*(BLKSZ2*4)), BLKSZ2*4);
}

int BGBBTJ_FiltLBR_GetIndexBlockY(int idx)
{
	return(filtlbr_block_cyval[idx]);
}

void BGBBTJ_FiltLBR_AddIndexBlock(int idx, byte *buf)
{
	int i, j, n;

	i=BGBBTJ_FiltLBR_CalcBlockY(buf);
	filtlbr_block_yval[idx]+=i;

	for(i=0; i<(BLKSZ2*4); i++)
	{
		filtlbr_block_vals[idx*(BLKSZ2*4)+i]+=buf[i];
	}
	filtlbr_block_cnt[idx]++;

	n=filtlbr_block_cnt[idx];
	j=filtlbr_block_yval[idx]/n;
	filtlbr_block_cyval[idx]=j;

	for(i=0; i<(BLKSZ2*4); i++)
	{
		j=filtlbr_block_vals[idx*(BLKSZ2*4)+i]/n;
		if(j<0)j=0;
		if(j>255)j=255;
		filtlbr_block_cvals[idx*(BLKSZ2*4)+i]=j;
	}

#if 0
	j=BGBBTJ_FiltLBR_CalcBlockY(filtlbr_block_cvals+(idx*(BLKSZ2*4)));
	filtlbr_block_cyval[idx]=j;
#endif
}

int BGBBTJ_FiltLBR_CalcBlockY(byte *buf)
{
	int i, j, k;
	
	k=0;
	for(i=0; i<BLKSZ2; i++)
	{
//		j=buf[i*4+0]+2*buf[i*4+1]+buf[i*4+2]+(buf[i*4+3]>>1);
		j=(buf[i*4+0]+2*buf[i*4+1]+buf[i*4+2])>>2;
//		j=(19595*buf[i*4+0] + 38470*buf[i*4+1] + 7471*buf[i*4+2])>>16;
		k+=j;
	}
	return(k/BLKSZ2);
}

#if 0
int BGBBTJ_FiltLBR_CompareBlock(byte *blka, byte *blkb)
{
	int ya, ua, va, aa;
	int yb, ub, vb, ab;
	int yc, yd;
	int cr, cg, cb, ca;
	int cra, cga, cba, caa;
	int crb, cgb, cbb, cab;
	int dr, dg, db, da;
	int dy, du, dv;
	int i, j, k, n;

#if 0
	n=0;
	for(i=0; i<BLKSZ2; i++)
	{
		cra=blka[i*4+0]; cga=blka[i*4+1];
		cba=blka[i*4+2]; caa=blka[i*4+3];

		crb=blkb[i*4+0]; cgb=blkb[i*4+1];
		cbb=blkb[i*4+2]; cab=blkb[i*4+3];

		dr=cra-crb;	dr=dr*dr;
		dg=cga-cgb; dg=dg*dg;
		db=cba-cbb; db=db*db;
		da=caa-cab; da=da*da;

		dy=(dr+2*dg+db)>>2;

		j=dr+dg+db+dy+(da>>2);
		n+=j;

//		n+=j*j;
	}
	return(n/(BLKSZ2*2));
#endif

#if 1
	n=0;
	for(i=0; i<BLKSZ2; i++)
	{
		cr=blka[i*4+0]; cg=blka[i*4+1];
		cb=blka[i*4+2]; ca=blka[i*4+3];
		ya=(cr+2*cg+cb)>>2;
//		ua=((cb-cg)>>1)+128;
//		va=((cr-cg)>>1)+128;
		ua=(cb-cg)+128;
		va=(cr-cg)+128;
		aa=((ca-128)>>1)+128;

		cr=blkb[i*4+0]; cg=blkb[i*4+1];
		cb=blkb[i*4+2]; ca=blkb[i*4+3];
		yb=(cr+2*cg+cb)>>2;
//		ub=((cb-cg)>>1)+128;
//		vb=((cr-cg)>>1)+128;
		ub=(cb-cg)+128;
		vb=(cr-cg)+128;
		ab=((ca-128)>>1)+128;

		dy=ya-yb;
		du=ua-ub;
		dv=va-vb;
		da=aa-ab;

		j=(dy*dy)+(du*du)+(dv*dv)+(da*da);
		n+=j;

//		n+=j*j;
	}
	return(n/(BLKSZ2*2));
#endif

#if 0
	n=0;
	for(i=0; i<BLKSZ; i++)
		for(j=0; j<BLKSZ; j++)
	{
		k=i*4+j;
		cr=blka[k*4+0]; cg=blka[k*4+1];
		cb=blka[k*4+2]; ca=blka[k*4+3];
		ya=(cr+2*cg+cb)>>2;
//		ua=((cb-cg)>>1)+128;
//		va=((cr-cg)>>1)+128;
		ua=(cb-cg)+128;
		va=(cr-cg)+128;
		aa=((ca-128)>>1)+128;

		cr=blkb[k*4+0]; cg=blkb[k*4+1];
		cb=blkb[k*4+2]; ca=blkb[k*4+3];
		yb=(cr+2*cg+cb)>>2;
//		ub=((cb-cg)>>1)+128;
//		vb=((cr-cg)>>1)+128;
		ub=(cb-cg)+128;
		vb=(cr-cg)+128;
		ab=((ca-128)>>1)+128;

		dy=ya-yb;
		du=ua-ub;
		dv=va-vb;
		da=aa-ab;

//		k=(dy*dy)+(da*da);
		k=(dy*dy)+(du*du)+(dv*dv)+(da*da);
		n+=k;

#if 1
		if((j+1)<BLKSZ)
		{
			k=i*4+j+1;
			cr=blka[k*4+0]; cg=blka[k*4+1]; cb=blka[k*4+2];
			yc=(cr+2*cg+cb)>>2;
			
			cr=blkb[k*4+0]; cg=blkb[k*4+1]; cb=blkb[k*4+2];
			yd=(cr+2*cg+cb)>>2;
			
			du=yc-ya;
			dv=yd-yb;
			dy=dv-du;
			n+=(dy*dy)>>2;
		}

		if((i+1)<BLKSZ)
		{
			k=(i+1)*4+j;
			cr=blka[k*4+0]; cg=blka[k*4+1]; cb=blka[k*4+2];
			yc=(cr+2*cg+cb)>>2;
			
			cr=blkb[k*4+0]; cg=blkb[k*4+1]; cb=blkb[k*4+2];
			yd=(cr+2*cg+cb)>>2;
			
			du=yc-ya;
			dv=yd-yb;
			dy=dv-du;
			n+=(dy*dy)>>2;
		}
#endif

//		n+=j*j;
	}
	return(n/(BLKSZ2*2));
#endif

#if 0
	n=0;
	for(i=0; i<(BLKSZ2*4); i++)
	{
		j=blka[i]-blkb[i];
		n+=j*j;
	}
	return(n/(BLKSZ2*4));
#endif
}
#endif


#if 1
int BGBBTJ_FiltLBR_CompareBlock(byte *blka, byte *blkb)
{
	int ya, ua, va, aa;
	int yb, ub, vb, ab;
	int yc, yd;
	int cr, cg, cb, ca;
	int cra, cga, cba, caa;
	int crb, cgb, cbb, cab;
	int dr, dg, db, da;
	int dy, du, dv;
	int er, eg, eb, ea;
	int i, j, k, n;

#if 1
	n=0; er=0; eg=0; eb=0; ea=0;
	for(i=0; i<BLKSZ2; i++)
	{
		cra=blka[i*4+0]; cga=blka[i*4+1];
		cba=blka[i*4+2]; caa=blka[i*4+3];

		crb=blkb[i*4+0]; cgb=blkb[i*4+1];
		cbb=blkb[i*4+2]; cab=blkb[i*4+3];

		dr=cra-crb;	dr=dr*dr;
		dg=cga-cgb; dg=dg*dg;
		db=cba-cbb; db=db*db;
		da=caa-cab; da=da*da;

		j=(caa>cab)?caa:cab;

		er+=(dr*j)>>8;
		eg+=(dg*j)>>8;
		eb+=(db*j)>>8;
		ea+=(da*j)>>8;

//		er+=dr;
//		eg+=dg;
//		eb+=db;
//		ea+=da;

//		dy=(dr+2*dg+db)>>2;
//		j=dr+dg+db+dy+(da>>2);
//		n+=j;

//		n+=j*j;
	}
	
//	n=(19595*er + 38470*eg + 7471*eb + ea*4096 + 32768)>>16;
//	n=(19595*er + 38470*eg + 7471*eb + ea*4096)>>16;
	n=(19595LL*er + 38470LL*eg + 7471LL*eb + ea*4096LL)>>16;
//	if(n<0)n=(1<<31)-1;
	
	return(n/BLKSZ2);
#endif
}
#endif


int BGBBTJ_FiltLBR_GetImageBlock(
	byte *img, int xstride, int ystride,
	byte *block)
{
	int i, j;
	
	for(i=0; i<BLKSZ; i++)
		for(j=0; j<BLKSZ; j++)
	{
		block[(i*BLKSZ+j)*4+0]=img[(i*ystride)+(j*xstride)+0];
		block[(i*BLKSZ+j)*4+1]=img[(i*ystride)+(j*xstride)+1];
		block[(i*BLKSZ+j)*4+2]=img[(i*ystride)+(j*xstride)+2];
		block[(i*BLKSZ+j)*4+3]=img[(i*ystride)+(j*xstride)+3];
	}
	return(0);
}

int BGBBTJ_FiltLBR_SetImageBlock(
	byte *img, int xstride, int ystride,
	byte *block)
{
	int i, j;
	
	for(i=0; i<BLKSZ; i++)
		for(j=0; j<BLKSZ; j++)
	{
		img[(i*ystride)+(j*xstride)+0]=block[(i*BLKSZ+j)*4+0];
		img[(i*ystride)+(j*xstride)+1]=block[(i*BLKSZ+j)*4+1];
		img[(i*ystride)+(j*xstride)+2]=block[(i*BLKSZ+j)*4+2];
		img[(i*ystride)+(j*xstride)+3]=block[(i*BLKSZ+j)*4+3];
	}
	return(0);
}

int BGBBTJ_FiltLBR_YChainIdx(int yv, int mmse)
{
//	return(yv/(mmse+1));
//	return((yv+mmse)/(2*mmse+1));
	return((yv+(mmse>>1))/(mmse+1));
}

int BGBBTJ_FiltLBR_LookupMatchIndexBlockSkip(byte *blk,
	int immse, int mmse, int skip)
{
	byte tblk[BLKSZ2*4];
	int bi, be;
	int i, j, yv;
	
	yv=BGBBTJ_FiltLBR_CalcBlockY(blk);

#if 0
	bi=-1; be=mmse;
	for(i=0; i<filtlbr_n_blocks; i++)
	{
		j=BGBBTJ_FiltLBR_GetIndexBlockY(i);
		j=j-yv; j=j*j;
		if(j>be)continue;
	
		BGBBTJ_FiltLBR_GetIndexBlock(i, tblk);
		j=BGBBTJ_FiltLBR_CompareBlock(tblk, blk);
		if(j<be) { bi=i; be=j; }
	}
#endif

#if 1
	bi=-1; be=mmse; i=filtlbr_chains[BGBBTJ_FiltLBR_YChainIdx(yv, immse)];
//	for(i=0; i<filtlbr_n_blocks; i++)
	while(i>=0)
	{
#if 1
		j=BGBBTJ_FiltLBR_GetIndexBlockY(i);
		j=j-yv; j=j*j;
		if(j>be)
		{
			i=filtlbr_block_ychain[i];
			continue;
		}
#endif

		if(skip>=0)
		{
			if(i==skip)
				{ i=filtlbr_block_ychain[i]; continue; }
//			if(filtlbr_block_ucnt[i]<16)
//			if(filtlbr_block_ucnt[i]<8)
			if(filtlbr_block_ucnt[i]<4)
				{ i=filtlbr_block_ychain[i]; continue; }
		}
	
		BGBBTJ_FiltLBR_GetIndexBlock(i, tblk);
		j=BGBBTJ_FiltLBR_CompareBlock(tblk, blk);
		if(j<be) { bi=i; be=j; }
		i=filtlbr_block_ychain[i];
	}
#endif
	
	return(bi);
}

int BGBBTJ_FiltLBR_LookupMatchIndexBlock(byte *blk, int mmse)
{
	return(BGBBTJ_FiltLBR_LookupMatchIndexBlockSkip(
		blk, mmse, mmse, -1));
}

int BGBBTJ_FiltLBR_GetMatchIndexBlock(byte *blk, int mmse)
{
	int i, j, k;
	
	i=BGBBTJ_FiltLBR_LookupMatchIndexBlock(blk, mmse);
	if(i>=0)return(i);
	i=filtlbr_n_blocks++;
	BGBBTJ_FiltLBR_AddIndexBlock(i, blk);

#if 1
	j=BGBBTJ_FiltLBR_YChainIdx(
		BGBBTJ_FiltLBR_GetIndexBlockY(i), mmse);
	filtlbr_block_ychain[i]=filtlbr_chains[j];
	filtlbr_chains[j]=i;
#endif
	
	return(i);
}

BGBBTJ_API void BGBBTJ_FiltLBR_CheckSetupImage(
	int xs, int ys, int qf)
{
	int xs2, ys2;

	xs2=(xs+BLKSZ-1)/BLKSZ;
	ys2=(ys+BLKSZ-1)/BLKSZ;

	if((xs!=filtlbr_xs) || (ys!=filtlbr_ys) || !filtlbr_block_vals)
	{
		if(filtlbr_block_vals)
		{
			free(filtlbr_block_vals);
			free(filtlbr_block_yval);
			free(filtlbr_block_cnt);

			free(filtlbr_block_cvals);
			free(filtlbr_block_cyval);
			free(filtlbr_block_ychain);
		}
	
		filtlbr_block_vals=malloc(2*xs2*ys2*(BLKSZ2*4)*sizeof(int));
		filtlbr_block_yval=malloc(2*xs2*ys2*sizeof(int));
		filtlbr_block_cnt=malloc(2*xs2*ys2*sizeof(int));
		filtlbr_block_ucnt=malloc(2*xs2*ys2*sizeof(int));

		filtlbr_block_cvals=malloc(2*xs2*ys2*(BLKSZ2*4));
		filtlbr_block_cyval=malloc(2*xs2*ys2*sizeof(short));
		filtlbr_block_ychain=malloc(2*xs2*ys2*sizeof(short));

		filtlbr_n_blocks=0;
		filtlbr_xs=xs;
		filtlbr_ys=ys;
	}
}

BGBBTJ_API void BGBBTJ_FiltLBR_SetupImage(
	int xs, int ys, int qf)
{
	int xs2, ys2;
	int i;

	BGBBTJ_FiltLBR_CheckSetupImage(xs, ys, qf);

	xs2=(xs+BLKSZ-1)/BLKSZ;
	ys2=(ys+BLKSZ-1)/BLKSZ;

	filtlbr_n_blocks=0;

	memset(filtlbr_block_vals, 0, 2*xs2*ys2*(BLKSZ2*4)*sizeof(int));
	memset(filtlbr_block_yval, 0, 2*xs2*ys2*sizeof(int));
	memset(filtlbr_block_cnt, 0, 2*xs2*ys2*sizeof(int));
	memset(filtlbr_block_ucnt, 0, 2*xs2*ys2*sizeof(int));

//	memset(filtlbr_chains, 0, 256*sizeof(int));
	for(i=0; i<1024; i++)
		filtlbr_chains[i]=-1;
}

/**
 * Filter image, where irgba and orgba give the input and output images.
 * xs and ys give the size, and qf gives the quality (low 8 bits, 0-100).
 */
BGBBTJ_API void BGBBTJ_FiltLBR_FilterImageB(
	byte *orgba, byte *irgba, int xs, int ys, int qf)
{
	byte tblk[BLKSZ2*4];
	int *bidx, *uidx;
	int x, y, xs2, ys2, xstr, ystr;
	int idx, idx1, mmse;
	int i, j, k;
	
	if((qf&255)>=100)
	{
		if(!orgba)return;
		memcpy(orgba, irgba, xs*ys*4);
		return;
	}

	printf("BGBBTJ_FiltLBR_FilterImage: Begin\n");

	BGBBTJ_FiltLBR_SetupImage(xs, ys, qf);

	xs2=(xs+BLKSZ-1)/BLKSZ;
	ys2=(ys+BLKSZ-1)/BLKSZ;

	bidx=malloc(xs2*ys2*sizeof(int));
//	uidx=malloc(xs2*ys2*sizeof(int));
	uidx=filtlbr_block_ucnt;

	memset(uidx, 0, xs2*ys2*sizeof(int));

	xstr=4; ystr=xs*4;
//	mmse=95-(qf&255);
//	mmse=90-(qf&255);
//	mmse=100-(qf&255);
//	mmse=256-(qf&255);

//	mmse=1.0+10.0*(pow(100.0/(qf&255), 2)-1.0);
	mmse=1.0+10.0*((100.0/(qf&255))-1.0);
//	mmse=1.0+20.0*((100.0/(qf&255))-1.0);

	if(mmse<0)mmse=0;
	mmse=mmse*mmse;

	for(y=0; y<ys2; y++)
	{
		printf("BGBBTJ_FiltLBR_FilterImage: %d/%d %d/%d\r",
			y, ys2, filtlbr_n_blocks, y*xs2);
		for(x=0; x<xs2; x++)
		{
			BGBBTJ_FiltLBR_GetImageBlock(irgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
				xstr, ystr, tblk);
			idx=BGBBTJ_FiltLBR_GetMatchIndexBlock(tblk, mmse);
//			BGBBTJ_FiltLBR_AddIndexBlock(idx, tblk);
			bidx[y*xs2+x]=idx;
			uidx[idx]++;
		}

		printf("BGBBTJ_FiltLBR_FilterImage: %d/%d %d/%d\r",
			y, ys2, filtlbr_n_blocks, y*xs2);
	}
	printf("\n");

#if 1
	for(y=0; y<ys2; y++)
	{
		for(x=0; x<xs2; x++)
		{
			idx=bidx[y*xs2+x];
			
//			if(uidx[idx]<16)
			if(uidx[idx]<8)
			{
				BGBBTJ_FiltLBR_GetImageBlock(
					irgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
					xstr, ystr, tblk);

//				k=mmse+((mmse*(32-uidx[idx]))>>1);
//				k=mmse+((mmse*(16-uidx[idx]))>>1);
//				k=mmse+((mmse*(16-uidx[idx]))>>2);
				k=mmse+((mmse*(8-uidx[idx]))>>2);
//				k=mmse+(mmse*(16-uidx[idx]));
				i=BGBBTJ_FiltLBR_LookupMatchIndexBlockSkip(
					tblk, mmse, k, idx);
				if(i>0)
				{
					bidx[y*xs2+x]=i;
				}
			}
			
//			BGBBTJ_FiltLBR_AddIndexBlock(idx, tblk);
//			bidx[y*xs2+x]=idx;
		}
	}
#endif

#if 1
	for(y=0; y<ys2; y++)
	{
//		printf("BGBBTJ_FiltLBR_FilterImage: %d/%d %d/%d\r",
//			y, ys2, filtlbr_n_blocks, y*xs2);
		for(x=0; x<xs2; x++)
		{
			BGBBTJ_FiltLBR_GetImageBlock(irgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
				xstr, ystr, tblk);
//			idx=BGBBTJ_FiltLBR_GetMatchIndexBlock(tblk, mmse);
			idx=bidx[y*xs2+x];
			BGBBTJ_FiltLBR_AddIndexBlock(idx, tblk);
//			bidx[y*xs2+x]=idx;
		}

//		printf("BGBBTJ_FiltLBR_FilterImage: %d/%d %d/%d\r",
//			y, ys2, filtlbr_n_blocks, y*xs2);
	}
//	printf("\n");
#endif

	if(!orgba)return;

	for(y=0; y<ys2; y++)
		for(x=0; x<xs2; x++)
	{
		idx=bidx[y*xs2+x];

#if 0
		printf("BGBBTJ_FiltLBR_FilterImage: %d/%d %d/%d\r",
			y, ys2, filtlbr_n_blocks, y*xs2);

		BGBBTJ_FiltLBR_GetImageBlock(irgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
			xstr, ystr, tblk);
//		idx1=BGBBTJ_FiltLBR_LookupMatchIndexBlock(tblk, mmse);
		idx1=BGBBTJ_FiltLBR_GetMatchIndexBlock(tblk, mmse);
		if(idx1>=0)idx=idx1;
#endif

		BGBBTJ_FiltLBR_GetIndexBlock(idx, tblk);
		BGBBTJ_FiltLBR_SetImageBlock(orgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
			xstr, ystr, tblk);
	}
//	printf("\n");
	
	free(bidx);
//	free(filtlbr_block_vals);
//	free(filtlbr_block_yval);
//	free(filtlbr_block_cvals);
//	free(filtlbr_block_cyval);
//	free(filtlbr_block_cnt);

	printf("BGBBTJ_FiltLBR_FilterImage: Done\n");
}

/**
 * Filter patch image, where irgba and orgba give the input and output images.
 * xs and ys give the size, and qf gives the quality (low 8 bits, 0-100).
 * Patch images will try to reuse blocks from a prior image.
 */
BGBBTJ_API void BGBBTJ_FiltLBR_FilterPatchImageB(
	byte *orgba, byte *irgba, int xs, int ys, int qf)
{
	byte tblk[BLKSZ2*4];
	int onchn[1024];
	int *bidx;
	int x, y, xs2, ys2, xstr, ystr;
	int idx, idx1, mmse, onb;
	int i;
	
	if((qf&255)>=100)
	{
		memcpy(orgba, irgba, xs*ys*4);
		return;
	}

	printf("BGBBTJ_FiltLBR_FilterImage: Begin\n");

	onb=filtlbr_n_blocks;
	for(i=0; i<1024; i++)
		onchn[i]=filtlbr_chains[i];

	BGBBTJ_FiltLBR_CheckSetupImage(xs, ys, qf);

	xs2=(xs+BLKSZ-1)/BLKSZ;
	ys2=(ys+BLKSZ-1)/BLKSZ;

	bidx=malloc(xs2*ys2*sizeof(int));

	xstr=4; ystr=xs*4;
//	mmse=95-(qf&255);
//	mmse=90-(qf&255);
//	mmse=100-(qf&255);
//	mmse=256-(qf&255);

//	mmse=1.0+10.0*(pow(100.0/(qf&255), 2)-1.0);
	mmse=1.0+10.0*((100.0/(qf&255))-1.0);
//	mmse=1.0+20.0*((100.0/(qf&255))-1.0);

	if(mmse<0)mmse=0;
	mmse=mmse*mmse;

	for(y=0; y<ys2; y++)
	{
		printf("BGBBTJ_FiltLBR_FilterImage: %d/%d %d/%d\r",
			y, ys2, filtlbr_n_blocks, y*xs2);
		for(x=0; x<xs2; x++)
		{
			BGBBTJ_FiltLBR_GetImageBlock(irgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
				xstr, ystr, tblk);
			idx=BGBBTJ_FiltLBR_GetMatchIndexBlock(tblk, mmse);
			bidx[y*xs2+x]=idx;
		}

		printf("BGBBTJ_FiltLBR_FilterImage: %d/%d %d/%d\r",
			y, ys2, filtlbr_n_blocks, y*xs2);
	}
	printf("\n");

#if 1
	for(y=0; y<ys2; y++)
	{
		for(x=0; x<xs2; x++)
		{
			BGBBTJ_FiltLBR_GetImageBlock(irgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
				xstr, ystr, tblk);
			idx=bidx[y*xs2+x];
			if(idx<onb)continue;
			BGBBTJ_FiltLBR_AddIndexBlock(idx, tblk);
		}
	}
#endif

	for(y=0; y<ys2; y++)
		for(x=0; x<xs2; x++)
	{
		idx=bidx[y*xs2+x];

		BGBBTJ_FiltLBR_GetIndexBlock(idx, tblk);
		BGBBTJ_FiltLBR_SetImageBlock(orgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
			xstr, ystr, tblk);
	}

	//restore prior state
	filtlbr_n_blocks=onb;
	for(i=0; i<1024; i++)
		filtlbr_chains[i]=onchn[i];

	free(bidx);
	printf("BGBBTJ_FiltLBR_FilterImage: Done\n");
}

BGBBTJ_API void BGBBTJ_FiltLBR_PreFilterImage_Reduce(
	byte *rgba, int xs, int ys, int qf)
{
	byte tblk[BLKSZ2*4], tblk2[BLKSZ2*4];
	byte tdblk[16];
	int x, y, xs2, ys2, xstr, ystr;
	int pxa, pxb, pxc, pxd;
	int cr, cg, cb, ca;
	int idx, idx1, mmse, onb;
	int i, j;

	xs2=(xs+BLKSZ-1)/BLKSZ;
	ys2=(ys+BLKSZ-1)/BLKSZ;

	xstr=4;
	ystr=xs*xstr;

	mmse=1.0+10.0*((100.0/(qf&255))-1.0);
//	mmse=1.0+20.0*((100.0/(qf&255))-1.0);

	if(mmse<0)mmse=0;
	mmse=mmse*mmse;

	for(y=0; y<ys2; y++)
	{
		for(x=0; x<xs2; x++)
		{
			BGBBTJ_FiltLBR_GetImageBlock(rgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
				xstr, ystr, tblk);
//			idx=BGBBTJ_FiltLBR_GetMatchIndexBlock(tblk, mmse);
//			bidx[y*xs2+x]=idx;

			for(i=0; i<2; i++)
				for(j=0; j<2; j++)
			{
				pxa=((i*2+0)*4+j*2+0)*4;	pxb=((i*2+0)*4+j*2+1)*4;
				pxc=((i*2+1)*4+j*2+0)*4;	pxd=((i*2+1)*4+j*2+1)*4;
				cr=(tblk[pxa+0]+tblk[pxb+0]+tblk[pxc+0]+tblk[pxd+0])>>2;
				cg=(tblk[pxa+1]+tblk[pxb+1]+tblk[pxc+1]+tblk[pxd+1])>>2;
				cb=(tblk[pxa+2]+tblk[pxb+2]+tblk[pxc+2]+tblk[pxd+2])>>2;
				ca=(tblk[pxa+3]+tblk[pxb+3]+tblk[pxc+3]+tblk[pxd+3])>>2;
				tblk2[pxa+0]=cr;	tblk2[pxb+0]=cr;
				tblk2[pxc+0]=cr;	tblk2[pxd+0]=cr;
				tblk2[pxa+1]=cg;	tblk2[pxb+1]=cg;
				tblk2[pxc+1]=cg;	tblk2[pxd+1]=cg;
				tblk2[pxa+2]=cb;	tblk2[pxb+2]=cb;
				tblk2[pxc+2]=cb;	tblk2[pxd+2]=cb;
				tblk2[pxa+3]=ca;	tblk2[pxb+3]=ca;
				tblk2[pxc+3]=ca;	tblk2[pxd+3]=ca;
			}

			i=BGBBTJ_FiltLBR_CompareBlock(tblk2, tblk);
			if((i*i)<=mmse)
			{
				BGBBTJ_FiltLBR_SetImageBlock(
					rgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
					xstr, ystr, tblk2);
				memcpy(tblk, tblk2, BLKSZ2*4);
			}

#if 0
			BGBBTJ_BCn_EncodeBlockDXT1F(
				tdblk, tblk, 4, 4*4, 3);
			BGBBTJ_BCn_DecodeBlockDXT1(tdblk, tblk2, 4, 4*4, 3);

			i=BGBBTJ_FiltLBR_CompareBlock(tblk2, tblk);
			if((i*i)<=mmse)
			{
				BGBBTJ_FiltLBR_SetImageBlock(
					rgba+(y*BLKSZ*ystr)+(x*BLKSZ*xstr),
					xstr, ystr, tblk2);
				memcpy(tblk, tblk2, BLKSZ2*4);
			}
#endif
		}
	}
}

BGBBTJ_API void BGBBTJ_FiltLBR_FilterImage(
	byte *orgba, byte *irgba, int xs, int ys, int qf)
{
	byte *trgba, *tblk;
	int fmt;

	if((qf&255)>90)
	{
		BGBBTJ_FiltLBR_FilterImageB(orgba, irgba, xs, ys, qf);
		return;
	}
	
	trgba=malloc(xs*ys*4);
	tblk=malloc(((xs+3)/4)*((ys+3)/4)*16);
	
	fmt=BGBBTJ_JPG_BC3;
//	if(fmt<75)
//		{ fmt=BGBBTJ_JPG_BC3F; }
//	if(fmt<50)
//		{ fmt=BGBBTJ_JPG_BC1A; }
	
	BGBBTJ_BCn_EncodeImageDXTn(tblk, irgba, xs, ys, 4, fmt);
	BGBBTJ_BCn_DecodeImageDXTn(tblk, trgba, xs, ys, 4, fmt);

	BGBBTJ_FiltLBR_ColorQuantizeImage(trgba, trgba, xs, ys, qf);
	
//	BGBBTJ_FiltLBR_PreFilterImage_Reduce(trgba, xs, ys, qf);
	
	BGBBTJ_FiltLBR_FilterImageB(orgba, trgba, xs, ys, qf);

//	BGBBTJ_FiltLBR_PreFilterImage_Reduce(orgba, xs, ys, qf);

//	BGBBTJ_FiltLBR_ColorQuantizeImage(orgba, orgba, xs, ys, qf);

	free(trgba);
	free(tblk);
	
//	BGBBTJ_FiltLBR_StatImageBlocks(orgba, xs, ys, qf);
//	BGBBTJ_FiltLBR_StatImageBlocks(irgba, xs, ys, qf);
}

BGBBTJ_API void BGBBTJ_FiltLBR_FilterPatchImage(
	byte *orgba, byte *irgba, int xs, int ys, int qf)
{
	byte *trgba, *tblk;

	if((qf&255)>90)
	{
		BGBBTJ_FiltLBR_FilterPatchImageB(orgba, irgba, xs, ys, qf);
		return;
	}
	
	trgba=malloc(xs*ys*4);
	tblk=malloc(((xs+3)/4)*((ys+3)/4)*16);
	
	BGBBTJ_BCn_EncodeImageDXTn(tblk, irgba, xs, ys, 4, BGBBTJ_JPG_BC3);
	BGBBTJ_BCn_DecodeImageDXTn(tblk, trgba, xs, ys, 4, BGBBTJ_JPG_BC3);
	BGBBTJ_FiltLBR_FilterPatchImageB(orgba, trgba, xs, ys, qf);

//	BGBBTJ_FiltLBR_ColorQuantizeImage(orgba, orgba, xs, ys, qf);

#if 0
	BGBBTJ_BCn_EncodeImageDXTn(tblk, orgba, xs, ys, 4, BGBBTJ_JPG_BC3);
#endif

	free(trgba);
	free(tblk);
}

BGBBTJ_API void BGBBTJ_FiltLBR_ColorQuantizeImage(
	byte *orgba, byte *irgba, int xs, int ys, int qf)
{
	int cr, cg, cb, ca;
	int cy, cu, cv;
	int i, j, k, n;

//	return;

	if((qf&255)>50)
	{
		if(orgba!=irgba)
			{ memcpy(orgba, irgba, xs*ys*4); }
		return;
	}
	
	n=xs*ys;
	for(i=0; i<n; i++)
	{
		cr=irgba[i*4+0];
		cg=irgba[i*4+1];
		cb=irgba[i*4+2];
		ca=irgba[i*4+3];

#if 0
		cr=((cr+21)/43)*43;
		cg=((cg+21)/43)*43;
		cb=((cb+21)/43)*43;
		cr=(cr<0)?0:((cr>255)?255:cr);
		cg=(cg<0)?0:((cg>255)?255:cg);
		cb=(cb<0)?0:((cb>255)?255:cb);
#endif

#if 1
		cy=(cr+2*cg+cb)>>2;
		cu=(cb-cg)+128;
		cv=(cr-cg)+128;
//		cu=((cb-cg)/2)+128;
//		cv=((cr-cg)/2)+128;

		cu=((cu-128)*1.5)+128;
		cv=((cv-128)*1.5)+128;

#if 1
//		cy=(cy*10+128)>>8;
//		cu=(cu*5+128)>>8;
//		cv=(cv*5+128)>>8;

//		cy=(cy*10+127)>>8;
//		cu=(cu*5+127)>>8;
//		cv=(cv*5+127)>>8;

		cy=(cy*10+128)>>8;
		cu=(((cu-128)*5+128)>>8)+2;
		cv=(((cv-128)*5+128)>>8)+2;

//		cy=(cy*10)>>8;
//		cu=(cu*5)>>8;
//		cv=(cv*5)>>8;

//		ca=ca>>4;
		cy=(cy<0)?0:((cy>9)?9:cy);
		cu=(cu<0)?0:((cu>4)?4:cu);
		cv=(cv<0)?0:((cv>4)?4:cv);
		
//		cy=(cy* 6554+128)>>8;
//		cu=(cu*13107+128)>>8;
//		cv=(cv*13107+128)>>8;

//		cy=(((cy-2)* 6554+128)>>8)+128;
		cy=(cy* 6554+128)>>8;
		cu=(((cu-2)*13107+128)>>8)+128;
		cv=(((cv-2)*13107+128)>>8)+128;

//		ca=ca<<4;
#endif

//		cu-=128; cv-=128;
//		cu=(cu-128)*2; cv=(cv-128)*2;
		cu=(cu-128)*0.67; cv=(cv-128)*0.67;

		cg=cy-((cu+cv)>>2);
		cb=cg+cu;
		cr=cg+cv;

		cr=(cr<0)?0:((cr>255)?255:cr);
		cg=(cg<0)?0:((cg>255)?255:cg);
		cb=(cb<0)?0:((cb>255)?255:cb);
#endif

		orgba[i*4+0]=cr;
		orgba[i*4+1]=cg;
		orgba[i*4+2]=cb;
		orgba[i*4+3]=ca;
	}
}

#if 0
BGBBTJ_API void BGBBTJ_FiltLBR_StatImageBlocks(
	byte *rgba, int xs, int ys, int qf)
{
	int *colors;
	int *bpat;
	byte *tbuf, *tcda;
	byte *tblk, *sb;
	int cr, cg, cb;
	int cya, cua, cva, cyb, cub, cvb;
	int i, j, k, l, n, nc, np;
	int stm, stn, sta;


	n=((xs+3)/4)*((ys+3)/4);

	colors=malloc(65536*sizeof(int));
	memset(colors, 0, 65536*sizeof(int));

	bpat=malloc(n*sizeof(int));
	tbuf=malloc(1<<22);

	tblk=malloc(n*16);
	BGBBTJ_BCn_EncodeImageDXTn(tblk, rgba, xs, ys, 4, BGBBTJ_JPG_BC3);

	tcda=malloc(n*6);
	
	printf("BGBBTJ_FiltLBR_StatImageBlocks:\n");

	np=0;
	for(i=0; i<n; i++)
	{
		sb=tblk+i*16+8;
		colors[sb[0]|(sb[1]<<8)]++;
		colors[sb[2]|(sb[3]<<8)]++;
		
#if 1
//		k=sb[4]|(sb[5]<<8)|(sb[6]<<16)|(sb[7]<<24);

#if 1
		k=(sb[4]&15)|((sb[5]&15)<<4);
		j=((sb[4]>>4)&15)|(((sb[5]>>4)&15)<<4);
		k|=j<<8;
		j=(sb[6]&15)|((sb[7]&15)<<4);
		k|=j<<16;
		j=((sb[6]>>4)&15)|(((sb[7]>>4)&15)<<4);
		k|=j<<24;
#endif
		
		for(j=0; j<np; j++)
			if(bpat[j]==k)
				break;
		if(j>=np)
		{
			j=np++;
			bpat[j]=k;

			tcda[j*4+0]=sb[0];
			tcda[j*4+1]=sb[1];
			tcda[j*4+2]=sb[2];
			tcda[j*4+3]=sb[3];

			if(j)
			{
//				k=sb[0]|(sb[1]<<8)|(sb[2]<<16)|(sb[3]<<24);
//				l=sb[0-16]|(sb[1-16]<<8)|(sb[2-16]<<16)|(sb[3-16]<<24);

				k=sb[0]|(sb[1]<<8);
				l=sb[0-16]|(sb[1-16]<<8);

				k=k-l;
				tcda[j*4+0]=k;
				tcda[j*4+1]=k>>8;
//				tcda[j*4+2]=k>>16;
//				tcda[j*4+3]=k>>24;

				k=sb[2]|(sb[3]<<8);
				l=sb[2-16]|(sb[3-16]<<8);

				k=k-l;
				tcda[j*4+2]=k;
				tcda[j*4+3]=k>>8;

//				tcda[j*4+0]=sb[0]-sb[-16];
//				tcda[j*4+1]=sb[1]-sb[-15];
//				tcda[j*4+2]=sb[2]-sb[-14];
//				tcda[j*4+3]=sb[3]-sb[-13];

//				tcda[j*4+0]=sb[0]-tcda[(j-1)*4+0];
//				tcda[j*4+1]=sb[1]-tcda[(j-1)*4+1];
//				tcda[j*4+2]=sb[2]-tcda[(j-1)*4+2];
//				tcda[j*4+3]=sb[3]-tcda[(j-1)*4+3];
			}

#if 0
			k=sb[0]|(sb[1]<<8);
			cb=(k&0x001F)<<3;
			cg=(k&0x07E0)>>3;
			cr=(k&0xF800)>>8;
//			tcda[j*6+0]=(cr+2*cg+cb)>>2;
//			tcda[j*6+1]=cb-cg;
//			tcda[j*6+2]=cr-cg;
			cya=(cr+2*cg+cb)>>2;
			cua=cb-cg;	cva=cr-cg;

			k=sb[2]|(sb[3]<<8);
			cb=(k&0x001F)<<3;
			cg=(k&0x07E0)>>3;
			cr=(k&0xF800)>>8;
//			tcda[j*6+3]=(cr+2*cg+cb)>>2;
//			tcda[j*6+4]=cb-cg;
//			tcda[j*6+5]=cr-cg;
			cyb=(cr+2*cg+cb)>>2;
			cub=cb-cg;	cvb=cr-cg;

			tcda[j*6+0]=cya;
			tcda[j*6+1]=cua;
			tcda[j*6+2]=cva;
			tcda[j*6+3]=cyb-cya;
			tcda[j*6+4]=cub-cua;
			tcda[j*6+5]=cvb-cva;
#endif
		}
#endif
	}
	
	nc=0; stm=99999; stn=0; sta=0;
	for(i=0; i<65536; i++)
	{
		if(colors[i])
		{
			if(colors[i]<stm)
				stm=colors[i];
			if(colors[i]>stn)
				stn=colors[i];
			sta+=colors[i];
			nc++;
		}
	}

	sta/=nc;

	printf("BGBBTJ_FiltLBR_NumColors: Nc=%d Stm=%d Stn=%d Sta=%d\n",
		nc, stm, stn, sta);
	printf("BGBBTJ_FiltLBR_NumPatterns: %d\n", np);

	i=PDZ2_EncodeStreamLvl(bpat, tbuf, np*4, 1<<22, 9);
	printf("BGBBTJ_FiltLBR_BPatSz: %d -> %d\n", np*4, i);

//	i=PDZ2_EncodeStreamLvl(tcda, tbuf, np*6, 1<<22, 9);
//	printf("BGBBTJ_FiltLBR_TcdaSz: %d -> %d, R=%d\n", np*6, i, np*4);

	i=PDZ2_EncodeStreamLvl(tcda, tbuf, np*4, 1<<22, 9);
	printf("BGBBTJ_FiltLBR_TcdaSz: %d -> %d, R=%d\n", np*4, i, np*4);

	printf("BGBBTJ_FiltLBR_StatImageBlocks: OK\n");
}
#endif

#if 1
BGBBTJ_API void BGBBTJ_FiltLBR_StatImageBlocks(
	byte *rgba, int xs, int ys, int qf)
{
	int stats[1024];
	byte *tbuf, *lbuf, *tlcbuf, *tlpbuf;
	byte *tpbuf, *tplbuf;
	byte *tzbuf, *tzlbuf;
	byte *tblk, *sb;
	u64 li, lj;
	int cr, cg, cb;
	int cya, cua, cva, cyb, cub, cvb;
	int i, j, k, l, n, nc, np;
	int stm, stn, sta, sz, szlb, szp, szplb;


	n=((xs+3)/4)*((ys+3)/4);

	tbuf=malloc(1<<22);
	lbuf=malloc(1<<22);
	tblk=malloc(2*n*16);
	BGBBTJ_BCn_EncodeImageDXTn(tblk, rgba, xs, ys, 4, BGBBTJ_JPG_BC3);
	BGBBTJ_BCn_EncodeImageDXTn(tblk+n*16, rgba, xs, ys, 4, BGBBTJ_JPG_BC3);
	sz=BGBBTJ_PackBCn_EncodeBlocks2DXT5(
		tbuf, lbuf, tblk, n, ((xs+3)/4), &szlb);

	tpbuf=malloc(1<<22);
	tplbuf=malloc(1<<22);
	szp=BGBBTJ_PackBCn_EncodePatchBlocks2DXT5(
		tpbuf, tplbuf, tblk, n, ((xs+3)/4), &szplb);
	
	printf("BGBBTJ_FiltLBR_StatImageBlocks: Sz=%d, SzLb=%d\n", sz, szlb);
	printf("BGBBTJ_FiltLBR_StatImageBlocks: SzP=%d, SzLbP=%d\n", szp, szplb);

	for(i=0; i<1024; i++)stats[i]=0;
//	BGBBTJ_PackBCn_StatBlockArray2(tbuf, stats, 2*n, ((xs+3)/4));
	BGBBTJ_PackBCn_StatBlockArray2(tpbuf, stats, 2*n, ((xs+3)/4));

	for(i=0; i<32; i++)
	{
		printf("B%3d: ", i*8);
		for(j=0; j<8; j++)
		{
			printf("%5d ", stats[i*8+j]);
		}
		printf("\n");
	}

	for(i=0; i<32; i++)
	{
		printf("I%3d: ", i*8);
		for(j=0; j<8; j++)
		{
			printf("%5d ", stats[256+i*8+j]);
		}
		printf("\n");
	}

	for(i=0; i<32; i++)
	{
		printf("L%3d: ", i*8);
		for(j=0; j<8; j++)
		{
			printf("%5d ", stats[512+i*8+j]);
		}
		printf("\n");
	}

	tzbuf=malloc(1<<22);
	tzlbuf=malloc(1<<22);
	
	tlcbuf=malloc(1<<22);
	tlpbuf=malloc(1<<22);

	lj=0;
	for(i=0; i<(szlb/8); i++)
	{
		li=*(u64 *)(lbuf+(i*8));
//		*(u32 *)(lbuf+(i*8))=li^lj;
//		lj=li;

		j=*(u32 *)(lbuf+(i*8+0));
		*(u32 *)(tlcbuf+i*4)=j;

		j=*(u32 *)(lbuf+(i*8+4));
		*(u32 *)(tlpbuf+i*4)=j;
	}

	i=PDZ2_EncodeStreamLvl(tbuf, tzbuf, sz, 1<<22, 9);
	printf("BGBBTJ_FiltLBR_StatImageBlocks: TzBuf %d -> %d\n", sz, i);

	j=PDZ2_EncodeStreamLvl(lbuf, tzlbuf, szlb, 1<<22, 9);
	printf("BGBBTJ_FiltLBR_StatImageBlocks: TzlBuf %d -> %d\n", szlb, j);

	printf("BGBBTJ_FiltLBR_StatImageBlocks: TotBuf %d -> %d\n", sz+szlb, i+j);

	j=PDZ2_EncodeStreamLvl(tlcbuf, tzlbuf, szlb/2, 1<<22, 9);
	printf("BGBBTJ_FiltLBR_StatImageBlocks: TzlcBuf %d -> %d\n", szlb/2, j);

	k=PDZ2_EncodeStreamLvl(tlpbuf, tzlbuf, szlb/2, 1<<22, 9);
	printf("BGBBTJ_FiltLBR_StatImageBlocks: TzlpBuf %d -> %d\n", szlb/2, k);

	printf("BGBBTJ_FiltLBR_StatImageBlocks: TotBufB %d -> %d\n",
		sz+szlb, i+j+k);
}
#endif
