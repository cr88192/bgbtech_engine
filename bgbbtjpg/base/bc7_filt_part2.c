/*
Encoder Logic for partitioned block modes.
This is for 2-partition blocks.
*/

#include <bgbbtj.h>

extern byte bgbbtj_bc7_partition2[64*16];
extern byte bgbbtj_bc7_partition3[64*16];
extern byte bgbbtj_bc7_anchor2[64];
extern byte bgbbtj_bc7_anchor3a[64];
extern byte bgbbtj_bc7_anchor3b[64];

byte bgbbtj_bc7_part2_idx[1<<16];
byte bgbbtj_bc7_part3_idx1[1<<16];
byte bgbbtj_bc7_part3_idx2[1<<16];
byte bgbbtj_bc7_part3_idxj[64*64];

int BGBBTJ_BC7_PartitionInit(void)
{
	static int init=0;
	byte tpart[16];
	byte *cp, *tp, *cp1, *cp2;
	int te, te2, be, bi;
	int i, j, k, l;
	
	if(init)return(0);
	init=1;
	
	for(i=0; i<65536; i++)
	{
		for(j=0; j<16; j++)
			tpart[j]=(i>>j)&1;
		
		be=256; bi=0;
		for(j=0; j<64; j++)
		{
			tp=tpart;
			cp=bgbbtj_bc7_partition2+(j*64);
			te=0; te2=0;
			for(k=0; k<16; k++)
			{
				te+=(cp[k]^tp[k]);
				te2+=!(cp[k]^tp[k]);
//				if(bgbbtj_bc7_partition2[j*64+k]!=tpart[k])
//					te++;
			}
			if(te<be)
				{ bi=j; be=te; }
			if(te2<be)
				{ bi=j; be=te2; }
		}

		bgbbtj_bc7_part2_idx[i]=bi;
	}

	for(i=0; i<65536; i++)
	{
		for(j=0; j<16; j++)
			tpart[j]=(i>>j)&1;
		
		be=256; bi=0;
		for(j=0; j<64; j++)
		{
			tp=tpart;
			cp=bgbbtj_bc7_partition3+(j*64);
			te=0; te2=0;
			for(k=0; k<16; k++)
			{
//				te+=(cp[k]==tp[k]);
//				te2+=(tp[k]&&cp[k]);
//				te+=(tp[k]&&cp[k])||(cp[k]==tp[k]);
//				te2+=((!tp[k])&&cp[k])||(tp[k]);

				te+=((!cp[k])^(!tp[k]));
				te2+=!((!cp[k])^(!tp[k]));

//				if(bgbbtj_bc7_partition2[j*64+k]!=tpart[k])
//					te++;
			}
			if(te<be)
				{ bi=j; be=te; }
			if(te2<be)
				{ bi=j; be=te2; }
		}

		bgbbtj_bc7_part3_idx1[i]=bi;
		bgbbtj_bc7_part3_idx2[i]=bi;
	}

#if 0
	for(i=0; i<65536; i++)
	{
		for(j=0; j<16; j++)
			tpart[j]=((i>>j)&1)?2:0;
		
		be=256; bi=0;
		for(j=0; j<64; j++)
		{
			tp=tpart;
			cp=bgbbtj_bc7_partition3+(j*64);
			te=0; te2=0;
			for(k=0; k<16; k++)
			{
				te+=((!cp[k])^(!tp[k]));
				te2+=!((!cp[k])^(!tp[k]));

//				te+=(cp[k]==tp[k]);
//				te2+=(cp[k]!=tp[k]);
//				te2+=((!cp[k])&&tp[k])||(!tp[k]);
//				te2+=(tp[k]&&(!cp[k]))||(!tp[k]);

//				te2+=((!tp[k])&&cp[k])||(tp[k]);
//				te2+=(tp[k]&&cp[k]);
//				te+=(tp[k]&&cp[k])||(cp[k]==tp[k]);
//				te2+=((!tp[k])&&cp[k])||(tp[k]);

//				if(bgbbtj_bc7_partition2[j*64+k]!=tpart[k])
//					te++;
			}
			if(te<be)
				{ bi=j; be=te; }
			if(te2<be)
				{ bi=j; be=te2; }
		}

		bgbbtj_bc7_part3_idx2[i]=bi;
	}
#endif

	for(i=0; i<64; i++)
		for(j=0; j<64; j++)
	{
		if(i==j)
		{
			bgbbtj_bc7_part3_idxj[i*64+j]=i;
			continue;
		}

		bgbbtj_bc7_part3_idxj[i*64+j]=i;

		cp1=bgbbtj_bc7_partition3+(i*64);
		cp2=bgbbtj_bc7_partition3+(j*64);

		for(k=0; k<16; k++)
		{
			if(cp1[k])
			{
				if(cp2[k])
					{ tpart[k]=2; }
				else
					{ tpart[k]=1; }
			}else
				{ tpart[k]=0; }
		}

		be=256; bi=0;
		for(k=0; k<64; k++)
		{
			tp=tpart;
			cp=bgbbtj_bc7_partition3+(k*64);
			te=0; te2=0;
			for(l=0; l<16; l++)
			{
				te+=(cp[l]!=tp[l]);
//				te2+=(tp[k]&&(!cp[k]))||(!tp[k]);
			}
			if(te<be)
				{ bi=k; be=te; }
//			if(te2<be)
//				{ bi=k; be=te2; }
		}

		bgbbtj_bc7_part3_idxj[i*64+j]=bi;
	}
	
	return(1);
}

void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYUV(
	byte *pxy, byte *pxu, byte *pxv,
	int *min_rgba_y, int *max_rgba_y,
	int *min_rgba_u, int *max_rgba_u,
	int *min_rgba_v, int *max_rgba_v,
	int *avg_rgba,
	int *rmcy, int *rncy, int *racy,
	int *rmcu, int *rncu, int *racu,
	int *rmcv, int *rncv, int *racv,
	byte *rgba, int xstride, int ystride)
{
	int mcy, mcu, mcv, mca;
	int ncy, ncu, ncv, nca;

	int mcry, mcgy, mcby;
	int ncry, ncgy, ncby;
	int mcru, mcgu, mcbu;
	int ncru, ncgu, ncbu;
	int mcrv, mcgv, mcbv;
	int ncrv, ncgv, ncbv;

	int acr, acg, acb, aca, acy, acu, acv;
	int cr, cg, cb, ca, cy, cu, cv;
	int i, j, k, l;

	mca=255; mcy=255; mcu=255; mcv=255;
	nca=  0; ncy=  0; ncu=  0; ncv=  0;
	acr=  0; acg=  0; acb=  0; aca=  0;
	acy=  0; acu=  0; acv=  0;

	mcry=255; mcgy=255; mcby=255;
	ncry=  0; ncgy=  0; ncby=  0;
	mcru=255; mcgu=255; mcbu=255;
	ncru=  0; ncgu=  0; ncbu=  0;
	mcrv=255; mcgv=255; mcbv=255;
	ncrv=  0; ncgv=  0; ncbv=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
//		cy=(cr+(2*cg)+cb)>>2;		//Luma

		cy=19595*cr + 38470*cg + 7471*cb;
		cu=-11056*cr -21712*cg +32768*cb;
		cv= 32768*cr -27440*cg - 5328*cb;

		cy=(cy + 32768)>>16;
		cu=((cu+32768)>>16)+128;
		cv=((cv+32768)>>16)+128;

		pxy[i*4+j]=cy;
		pxu[i*4+j]=cu;
		pxv[i*4+j]=cv;

		//Find Mins/Maxs
		if(cy<mcy) { mcry=cr; mcgy=cg; mcby=cb; mcy=cy; }
		if(cy>ncy) { ncry=cr; ncgy=cg; ncby=cb; ncy=cy; }
		if(cu<mcu) { mcru=cr; mcgu=cg; mcbu=cb; mcu=cu; }
		if(cu>ncu) { ncru=cr; ncgu=cg; ncbu=cb; ncu=cu; }
		if(cv<mcv) { mcrv=cr; mcgv=cg; mcbv=cb; mcv=cv; }
		if(cv>ncv) { ncrv=cr; ncgv=cg; ncbv=cb; ncv=cv; }
//		if(ca<mca) { mca=ca; }
//		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
	
	min_rgba_y[0]=mcry;			min_rgba_y[1]=mcgy;
	min_rgba_y[2]=mcby;			min_rgba_y[3]=mca;
	max_rgba_y[0]=ncry;			max_rgba_y[1]=ncgy;
	max_rgba_y[2]=ncby;			max_rgba_y[3]=nca;
	min_rgba_u[0]=mcru;			min_rgba_u[1]=mcgu;
	min_rgba_u[2]=mcbu;			min_rgba_u[3]=mca;
	max_rgba_u[0]=ncru;			max_rgba_u[1]=ncgu;
	max_rgba_u[2]=ncbu;			max_rgba_u[3]=nca;
	min_rgba_v[0]=mcrv;			min_rgba_v[1]=mcgv;
	min_rgba_v[2]=mcbv;			min_rgba_v[3]=mca;
	max_rgba_v[0]=ncrv;			max_rgba_v[1]=ncgv;
	max_rgba_v[2]=ncbv;			max_rgba_v[3]=nca;

	avg_rgba[0]=acr>>16;	avg_rgba[1]=acg>>16;
	avg_rgba[2]=acb>>16;	avg_rgba[3]=aca>>16;

//	avg_rgba[0]=(mcr+ncr)>>1;	avg_rgba[1]=(mcg+ncg)>>1;
//	avg_rgba[2]=(mcb+ncb)>>1;	avg_rgba[3]=(mca+nca)>>1;
	
	*rmcy=mcy; *rncy=ncy; *racy=(mcy+ncy)>>1;
	*rmcu=mcu; *rncu=ncu; *racu=(mcu+ncu)>>1;
	*rmcv=mcv; *rncv=ncv; *racv=(mcv+ncv)>>1;
//	*rmca=mca; *rnca=nca; *raca=(mca+nca)>>1;
}


int BGBBTJ_BC7_PickBlockPartition2(
	byte *rgba, int xstride, int ystride)
{
	byte pxy[16], pxu[16], pxv[16];
	int min_y[4], mcy;
	int max_y[4], ncy;
	int min_u[4], mcu;
	int max_u[4], ncu;
	int min_v[4], mcv;
	int max_v[4], ncv;
	int mca, nca;
	int avg[4], acy, acu, acv, aca;
	int idx, pn;
	int i, j, k;

	BGBBTJ_BC7_PartitionInit();

	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYUV(
		pxy, pxu, pxv,
		min_y, max_y,
		min_u, max_u,
		min_v, max_v,
		avg,
		&mcy, &ncy, &acy,
		&mcu, &ncu, &acu,
		&mcv, &ncv, &acv,
		rgba, xstride, ystride);
	
	if((ncu-mcu)>(ncv-mcv))
	{
		idx=0;
		for(i=0; i<16; i++)
			{ idx|=((pxu[i]>acu)<<i); }
		pn=bgbbtj_bc7_part2_idx[idx];
	}else
	{
		idx=0;
		for(i=0; i<16; i++)
			{ idx|=((pxv[i]>acv)<<i); }
		pn=bgbbtj_bc7_part2_idx[idx];
	}
	return(pn);
}

#if 1
void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA_Part2(
	byte *pxy, byte *pxa,
	int *min_rgba0, int *max_rgba0, int *avg_rgba0,
	int *min_rgba1, int *max_rgba1, int *avg_rgba1,
	int *rmcy0, int *rncy0, int *racy0,
	int *rmcy1, int *rncy1, int *racy1,
	int *rmca0, int *rnca0, int *raca0,
	int *rmca1, int *rnca1, int *raca1,
	byte *rgba, int xstride, int ystride,
	int part)
{
	byte pxyy[16], pxyr[16], pxyg[16], pxyb[16];
	byte *tpart;
	byte *tpxy0, *tpxy1;

	int acr0, acg0, acb0, aca0, acy0, acn0;
	int acr1, acg1, acb1, aca1, acy1, acn1;

	int mcr0, mcg0, mcb0, mca0, mcy0;
	int ncr0, ncg0, ncb0, nca0, ncy0;
	int mcr1, mcg1, mcb1, mca1, mcy1;
	int ncr1, ncg1, ncb1, nca1, ncy1;

	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;

	int mcr3y0, mcg3y0, mcb3y0, mca3y0, mcy3y0;
	int ncr3y0, ncg3y0, ncb3y0, nca3y0, ncy3y0;
	int mcr3y1, mcg3y1, mcb3y1, mca3y1, mcy3y1;
	int ncr3y1, ncg3y1, ncb3y1, nca3y1, ncy3y1;

	int mcr3r0, mcg3r0, mcb3r0, ncr3r0, ncg3r0, ncb3r0, mcy3r0, ncy3r0;
	int mcr3g0, mcg3g0, mcb3g0, ncr3g0, ncg3g0, ncb3g0, mcy3g0, ncy3g0;
	int mcr3b0, mcg3b0, mcb3b0, ncr3b0, ncg3b0, ncb3b0, mcy3b0, ncy3b0;
	int mcr3r1, mcg3r1, mcb3r1, ncr3r1, ncg3r1, ncb3r1, mcy3r1, ncy3r1;
	int mcr3g1, mcg3g1, mcb3g1, ncr3g1, ncg3g1, ncb3g1, mcy3g1, ncy3g1;
	int mcr3b1, mcg3b1, mcb3b1, ncr3b1, ncg3b1, ncb3b1, mcy3b1, ncy3b1;

	int dr, dg, db, da, dy;
	int cr, cg, cb, ca, cy;
	int cyr, cyg, cyb;
	int i, j, k, l;

	tpart=bgbbtj_bc7_partition2+(part*16);

	acr0=  0; acg0=  0; acb0=  0; aca0=  0; acy0=  0; acn0=  0;
	acr1=  0; acg1=  0; acb1=  0; aca1=  0; acy1=  0; acn1=  0;

	mcr3y0=255; mcg3y0=255; mcb3y0=255; mca3y0=255; mcy3y0=255;
	ncr3y0=  0; ncg3y0=  0; ncb3y0=  0; nca3y0=  0; ncy3y0=  0;
	mcr3r0=255; mcg3r0=255; mcb3r0=255; mcy3r0=255;
	ncr3r0=  0; ncg3r0=  0; ncb3r0=  0; ncy3r0=  0;
	mcr3g0=255; mcg3g0=255; mcb3g0=255; mcy3g0=255;
	ncr3g0=  0; ncg3g0=  0; ncb3g0=  0; ncy3g0=  0;
	mcr3b0=255; mcg3b0=255; mcb3b0=255; mcy3b0=255;
	ncr3b0=  0; ncg3b0=  0; ncb3b0=  0; ncy3b0=  0;

	mcr3y1=255; mcg3y1=255; mcb3y1=255; mca3y1=255; mcy3y1=255;
	ncr3y1=  0; ncg3y1=  0; ncb3y1=  0; nca3y1=  0; ncy3y1=  0;
	mcr3r1=255; mcg3r1=255; mcb3r1=255; mcy3r1=255;
	ncr3r1=  0; ncg3r1=  0; ncb3r1=  0; ncy3r1=  0;
	mcr3g1=255; mcg3g1=255; mcb3g1=255; mcy3g1=255;
	ncr3g1=  0; ncg3g1=  0; ncb3g1=  0; ncy3g1=  0;
	mcr3b1=255; mcg3b1=255; mcb3b1=255; mcy3b1=255;
	ncr3b1=  0; ncg3b1=  0; ncb3b1=  0; ncy3b1=  0;

	mca0=  255;	nca0=  0;
	mca1=  255;	nca1=  0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
//		cy=(cr+(2*cg)+cb)>>2;		//Luma

		cy =(5*cr+8*cg+3*cb)>>4;		//Luma (Default)
//		cyr=(8*cr+5*cg+3*cb)>>4;
		cyr=(8*cr+2*cg+6*cb)>>4;
		cyg=(8*cr+6*cg+2*cb)>>4;
		cyb=(2*cr+6*cg+8*cb)>>4;

		pxa[i*4+j]=ca;
//		pxy[i*4+j]=cy;

		pxyy[i*4+j]=cy;
		pxyr[i*4+j]=cyr;
		pxyg[i*4+j]=cyg;
		pxyb[i*4+j]=cyb;

		if(tpart[i*4+j])
		{
			if(ca<mca1) { mca1=ca; }
			if(ca>nca1) { nca1=ca; }

			if(cy <mcy3y1) { mcr3y1=cr; mcg3y1=cg; mcb3y1=cb; mcy3y1=cy; }
			if(cy >ncy3y1) { ncr3y1=cr; ncg3y1=cg; ncb3y1=cb; ncy3y1=cy; }
			if(cyr<mcy3r1) { mcr3r1=cr; mcg3r1=cg; mcb3r1=cb; mcy3r1=cyr; }
			if(cyr>ncy3r1) { ncr3r1=cr; ncg3r1=cg; ncb3r1=cb; ncy3r1=cyr; }
			if(cyg<mcy3g1) { mcr3g1=cr; mcg3g1=cg; mcb3g1=cb; mcy3g1=cyg; }
			if(cyg>ncy3g1) { ncr3g1=cr; ncg3g1=cg; ncb3g1=cb; ncy3g1=cyg; }
			if(cyb<mcy3b1) { mcr3b1=cr; mcg3b1=cg; mcb3b1=cb; mcy3b1=cyb; }
			if(cyb>ncy3b1) { ncr3b1=cr; ncg3b1=cg; ncb3b1=cb; ncy3b1=cyb; }

			acr1+=cr; acg1+=cg;
			acb1+=cb; acy1+=cy;
			acn1++;
		}else
		{
			if(ca<mca0) { mca0=ca; }
			if(ca>nca0) { nca0=ca; }

			if(cy <mcy3y0) { mcr3y0=cr; mcg3y0=cg; mcb3y0=cb; mcy3y0=cy; }
			if(cy >ncy3y0) { ncr3y0=cr; ncg3y0=cg; ncb3y0=cb; ncy3y0=cy; }
			if(cyr<mcy3r0) { mcr3r0=cr; mcg3r0=cg; mcb3r0=cb; mcy3r0=cyr; }
			if(cyr>ncy3r0) { ncr3r0=cr; ncg3r0=cg; ncb3r0=cb; ncy3r0=cyr; }
			if(cyg<mcy3g0) { mcr3g0=cr; mcg3g0=cg; mcb3g0=cb; mcy3g0=cyg; }
			if(cyg>ncy3g0) { ncr3g0=cr; ncg3g0=cg; ncb3g0=cb; ncy3g0=cyg; }
			if(cyb<mcy3b0) { mcr3b0=cr; mcg3b0=cg; mcb3b0=cb; mcy3b0=cyb; }
			if(cyb>ncy3b0) { ncr3b0=cr; ncg3b0=cg; ncb3b0=cb; ncy3b0=cyb; }

			acr0+=cr; acg0+=cg;
			acb0+=cb; acy0+=cy;
			acn0++;
		}
	}

	mcr0=mcr3y0; mcg0=mcg3y0; mcb0=mcb3y0; mcy0=mcy3y0;
	ncr0=ncr3y0; ncg0=ncg3y0; ncb0=ncb3y0; ncy0=ncy3y0;
	tpxy0=pxyy; dy=ncy0-mcy0;

	dy=ncy3y0-mcy3y0;
	dr=ncy3r0-mcy3r0;
	dg=ncy3g0-mcy3g0;
	db=ncy3b0-mcy3b0;
	if(dr>dy)
	{
		mcr0=mcr3r0; mcg0=mcg3r0; mcb0=mcb3r0; mcy0=mcy3r0;
		ncr0=ncr3r0; ncg0=ncg3r0; ncb0=ncb3r0; ncy0=ncy3r0;
		tpxy0=pxyr; dy=ncy0-mcy0;
	}
	if(dg>dy)
	{
		mcr0=mcr3g0; mcg0=mcg3g0; mcb0=mcb3g0; mcy0=mcy3g0;
		ncr0=ncr3g0; ncg0=ncg3g0; ncb0=ncb3g0; ncy0=ncy3g0;
		tpxy0=pxyg; dy=ncy0-mcy0;
	}
	if(db>dy)
	{
		mcr0=mcr3b0; mcg0=mcg3b0; mcb0=mcb3b0; mcy0=mcy3b0;
		ncr0=ncr3b0; ncg0=ncg3b0; ncb0=ncb3b0; ncy0=ncy3b0;
		tpxy0=pxyb; dy=ncy0-mcy0;
	}


	mcr1=mcr3y1; mcg1=mcg3y1; mcb1=mcb3y1; mcy1=mcy3y1;
	ncr1=ncr3y1; ncg1=ncg3y1; ncb1=ncb3y1; ncy1=ncy3y1;
	tpxy1=pxyy; dy=ncy1-mcy1;

	dy=ncy3y1-mcy3y1;
	dr=ncy3r1-mcy3r1;
	dg=ncy3g1-mcy3g1;
	db=ncy3b1-mcy3b1;
	if(dr>dy)
	{
		mcr1=mcr3r1; mcg1=mcg3r1; mcb1=mcb3r1; mcy1=mcy3r1;
		ncr1=ncr3r1; ncg1=ncg3r1; ncb1=ncb3r1; ncy1=ncy3r1;
		tpxy1=pxyr; dy=ncy1-mcy1;
	}
	if(dg>dy)
	{
		mcr1=mcr3g1; mcg1=mcg3g1; mcb1=mcb3g1; mcy1=mcy3g1;
		ncr1=ncr3g1; ncg1=ncg3g1; ncb1=ncb3g1; ncy1=ncy3g1;
		tpxy1=pxyg; dy=ncy1-mcy1;
	}
	if(db>dy)
	{
		mcr1=mcr3b1; mcg1=mcg3b1; mcb1=mcb3b1; mcy1=mcy3b1;
		ncr1=ncr3b1; ncg1=ncg3b1; ncb1=ncb3b1; ncy1=ncy3b1;
		tpxy1=pxyb; dy=ncy1-mcy1;
	}

#if 1
	acr0=acr0/acn0;
	acg0=acg0/acn0;
	acb0=acb0/acn0;
	
	acy0=(acr0+(2*acg0)+acb0)>>2;
	mcy3=(mcr0+(2*mcg0)+mcb0)>>2;
	ncy3=(ncr0+(2*ncg0)+ncb0)>>2;

	l=(256*mcy3)/(acy0+1);
	cr=(acr0*l+128)>>8;
	cg=(acg0*l+128)>>8;
	cb=(acb0*l+128)>>8;
	mcr0=(3*mcr0+cr)>>2;
	mcg0=(3*mcg0+cg)>>2;
	mcb0=(3*mcb0+cb)>>2;
	
	l=(256*ncy3)/(acy0+1);
	cr=(acr0*l+128)>>8;
	cg=(acg0*l+128)>>8;
	cb=(acb0*l+128)>>8;
	ncr0=(3*ncr0+cr)>>2;
	ncg0=(3*ncg0+cg)>>2;
	ncb0=(3*ncb0+cb)>>2;
#endif

#if 1
	acr1=acr1/acn1;
	acg1=acg1/acn1;
	acb1=acb1/acn1;
	
	acy1=(acr1+(2*acg1)+acb1)>>2;
	mcy3=(mcr1+(2*mcg1)+mcb1)>>2;
	ncy3=(ncr1+(2*ncg1)+ncb1)>>2;

	l=(256*mcy3)/(acy1+1);
	cr=(acr1*l+128)>>8;
	cg=(acg1*l+128)>>8;
	cb=(acb1*l+128)>>8;
	mcr1=(3*mcr1+cr)>>2;
	mcg1=(3*mcg1+cg)>>2;
	mcb1=(3*mcb1+cb)>>2;
	
	l=(256*ncy3)/(acy1+1);
	cr=(acr1*l+128)>>8;
	cg=(acg1*l+128)>>8;
	cb=(acb1*l+128)>>8;
	ncr1=(3*ncr1+cr)>>2;
	ncg1=(3*ncg1+cg)>>2;
	ncb1=(3*ncb1+cb)>>2;
#endif

#if 1
	mcr0=(mcr0<0)?0:((mcr0>255)?255:mcr0);
	mcg0=(mcg0<0)?0:((mcg0>255)?255:mcg0);
	mcb0=(mcb0<0)?0:((mcb0>255)?255:mcb0);
	ncr0=(ncr0<0)?0:((ncr0>255)?255:ncr0);
	ncg0=(ncg0<0)?0:((ncg0>255)?255:ncg0);
	ncb0=(ncb0<0)?0:((ncb0>255)?255:ncb0);
#endif

#if 1
	mcr1=(mcr1<0)?0:((mcr1>255)?255:mcr1);
	mcg1=(mcg1<0)?0:((mcg1>255)?255:mcg1);
	mcb1=(mcb1<0)?0:((mcb1>255)?255:mcb1);
	ncr1=(ncr1<0)?0:((ncr1>255)?255:ncr1);
	ncg1=(ncg1<0)?0:((ncg1>255)?255:ncg1);
	ncb1=(ncb1<0)?0:((ncb1>255)?255:ncb1);
#endif

//	for(i=0; i<16; i++)
//		{ pxy[i]=tpxy[i]; }

	for(i=0; i<16; i++)
		{ pxy[i]=tpart[i]?tpxy1[i]:tpxy0[i]; }

	min_rgba0[0]=mcr0;			min_rgba0[1]=mcg0;
	min_rgba0[2]=mcb0;			min_rgba0[3]=mca0;
	max_rgba0[0]=ncr0;			max_rgba0[1]=ncg0;
	max_rgba0[2]=ncb0;			max_rgba0[3]=nca0;
	avg_rgba0[0]=(mcr0+ncr0)>>1;
	avg_rgba0[1]=(mcg0+ncg0)>>1;
	avg_rgba0[2]=(mcb0+ncb0)>>1;
	avg_rgba0[3]=(mca0+nca0)>>1;
	
	min_rgba1[0]=mcr1;			min_rgba1[1]=mcg1;
	min_rgba1[2]=mcb1;			min_rgba1[3]=mca1;
	max_rgba1[0]=ncr1;			max_rgba1[1]=ncg1;
	max_rgba1[2]=ncb1;			max_rgba1[3]=nca1;
	avg_rgba1[0]=(mcr1+ncr1)>>1;
	avg_rgba1[1]=(mcg1+ncg1)>>1;
	avg_rgba1[2]=(mcb1+ncb1)>>1;
	avg_rgba1[3]=(mca1+nca1)>>1;
	
	*rmcy0=mcy0; *rncy0=ncy0; *racy0=(mcy0+ncy0)>>1;
	*rmca0=mca0; *rnca0=nca0; *raca0=(mca0+nca0)>>1;
	*rmcy1=mcy1; *rncy1=ncy1; *racy1=(mcy1+ncy1)>>1;
	*rmca1=mca1; *rnca1=nca1; *raca1=(mca1+nca1)>>1;
}
#endif

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
	byte *pxv, int n, int min, int max, int part)
{
	byte *tp;
	int i, j, k;

	tp=bgbbtj_bc7_partition2+(part*16);
	
	for(i=0; i<n; i++)
	{
		if(tp[i]==0)
			{ pxv[i]=max-(pxv[i]-min); }
	}
}

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
	byte *pxv, int n, int min, int max, int part)
{
	byte *tp;
	int i, j, k;

	tp=bgbbtj_bc7_partition2+(part*16);
	
	for(i=0; i<n; i++)
	{
		if(tp[i]==1)
			{ pxv[i]=max-(pxv[i]-min); }
	}
}

void BGBBTJ_BC7_EncodeBlock_Mode1(byte *block,
	byte *rgba, int xstride, int ystride, int part)
{
//	static const char idxtab[32]=
//		{  0,  0,  0,  0,  0,  0,  0,  0,
//		   0,  1,  2,  3,  4,  5,  6,  7,
//		   8,  9, 10, 11, 12, 13, 14, 15,
//		  15, 15, 15, 15, 15, 15, 15, 15};
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	byte pxy[16], pxa[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;

	int min0[4], mcy0, mca0;
	int max0[4], ncy0, nca0;
	int avg0[4], acy0, aca0;
	int min1[4], mcy1, mca1;
	int max1[4], ncy1, nca1;
	int avg1[4], acy1, aca1;

	int acr, acg, acb, ank;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	byte *tp;

	tp=bgbbtj_bc7_partition2+(part*16);	

//	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY2(
//		pxy, min, max, avg,
//		&mcy, &ncy, &acy,
//		rgba, xstride, ystride);
		
	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA_Part2(
		pxy, pxa,
		min0, max0, avg0,
		min1, max1, avg1,
		&mcy0, &ncy0, &acy0,
		&mcy1, &ncy1, &acy1,
		&mca0, &nca0, &aca0,
		&mca1, &nca1, &aca1,
		rgba, xstride, ystride, part);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
//	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, 2, 2);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 6);

	if(pxy[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
			pxy, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor2[part];
	if(pxy[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
			pxy, 16, mcy1, ncy1, part);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>2, 6);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>2, 6);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>2, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>2, 6);

//	BGBBTJ_BitsLE_WriteBits(&bits, (min0[3])>>1, 6);
//	BGBBTJ_BitsLE_WriteBits(&bits, (max0[3])>>1, 6);
//	BGBBTJ_BitsLE_WriteBits(&bits, min[1], 1);
//	BGBBTJ_BitsLE_WriteBits(&bits, max[1], 1);

//	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>1, 1);
//	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>1, 1);

	p0=(min0[0]&min0[1])|(min0[1]&min0[2])|(min0[0]&min0[2]);
	p1=(max0[0]&max0[1])|(max0[1]&max0[2])|(max0[0]&max0[2]);
	p2=(min1[0]&min1[1])|(min1[1]&min1[2])|(min1[0]&min1[2]);
	p3=(max1[0]&max1[1])|(max1[1]&max1[2])|(max1[0]&max1[2]);
	p0=p0&p2;	p1=p1&p3;
	BGBBTJ_BitsLE_WriteBit(&bits, p0>>1);
	BGBBTJ_BitsLE_WriteBit(&bits, p1>>1);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=65536-2048;
	l3a=65536;
	ank=bgbbtj_bc7_anchor2[part];

	p0=idxtab2[((pxy[0]-acy0)*l1+l3a)>>13];
	BGBBTJ_BitsLE_Write2Bits(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==1)
		{
			if(i==ank)
			{
				p0=idxtab2[((pxy[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}else
			{
				p0=idxtab2[((pxy[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_Write3Bits(&bits, p0);
			}
		}else
		{
			p0=idxtab2[((pxy[i]-acy0)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write3Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode3(byte *block,
	byte *rgba, int xstride, int ystride, int part)
{
//	static const char idxtab[32]=
//		{  0,  0,  0,  0,  0,  0,  0,  0,
//		   0,  1,  2,  3,  4,  5,  6,  7,
//		   8,  9, 10, 11, 12, 13, 14, 15,
//		  15, 15, 15, 15, 15, 15, 15, 15};
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	byte pxy[16], pxa[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;

	int min0[4], mcy0, mca0;
	int max0[4], ncy0, nca0;
	int avg0[4], acy0, aca0;
	int min1[4], mcy1, mca1;
	int max1[4], ncy1, nca1;
	int avg1[4], acy1, aca1;

	int acr, acg, acb, ank;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	byte *tp;

	tp=bgbbtj_bc7_partition2+(part*16);	

//	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY2(
//		pxy, min, max, avg,
//		&mcy, &ncy, &acy,
//		rgba, xstride, ystride);
		
	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA_Part2(
		pxy, pxa,
		min0, max0, avg0,
		min1, max1, avg1,
		&mcy0, &ncy0, &acy0,
		&mcy1, &ncy1, &acy1,
		&mca0, &nca0, &aca0,
		&mca1, &nca1, &aca1,
		rgba, xstride, ystride, part);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
//	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, 8, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 6);

	if(pxy[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
			pxy, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor2[part];
	if(pxy[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
			pxy, 16, mcy1, ncy1, part);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>1, 7);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>1, 7);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>1, 7);

//	BGBBTJ_BitsLE_WriteBits(&bits, (min0[3])>>1, 6);
//	BGBBTJ_BitsLE_WriteBits(&bits, (max0[3])>>1, 6);
//	BGBBTJ_BitsLE_WriteBits(&bits, min[1], 1);
//	BGBBTJ_BitsLE_WriteBits(&bits, max[1], 1);

//	BGBBTJ_BitsLE_WriteBit(&bits, min0[1]);
//	BGBBTJ_BitsLE_WriteBit(&bits, max0[1]);
//	BGBBTJ_BitsLE_WriteBit(&bits, min1[1]);
//	BGBBTJ_BitsLE_WriteBit(&bits, max1[1]);

	p0=(min0[0]&min0[1])|(min0[1]&min0[2])|(min0[0]&min0[2]);
	p1=(max0[0]&max0[1])|(max0[1]&max0[2])|(max0[0]&max0[2]);
	p2=(min1[0]&min1[1])|(min1[1]&min1[2])|(min1[0]&min1[2]);
	p3=(max1[0]&max1[1])|(max1[1]&max1[2])|(max1[0]&max1[2]);
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	BGBBTJ_BitsLE_WriteBit(&bits, p1);
	BGBBTJ_BitsLE_WriteBit(&bits, p2);
	BGBBTJ_BitsLE_WriteBit(&bits, p3);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=65536-2048;
	l3a=65536;
	ank=bgbbtj_bc7_anchor2[part];

	p0=idxtab[((pxy[0]-acy0)*l1+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==1)
		{
			if(i==ank)
			{
				p0=idxtab[((pxy[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_WriteBit(&bits, p0);
			}else
			{
				p0=idxtab[((pxy[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}
		}else
		{
			p0=idxtab[((pxy[i]-acy0)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlock_Mode7(byte *block,
	byte *rgba, int xstride, int ystride, int part)
{
//	static const char idxtab[32]=
//		{  0,  0,  0,  0,  0,  0,  0,  0,
//		   0,  1,  2,  3,  4,  5,  6,  7,
//		   8,  9, 10, 11, 12, 13, 14, 15,
//		  15, 15, 15, 15, 15, 15, 15, 15};
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,1,1, 2,2,3,3, 3,3,3,3 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 0,1,2,3, 4,5,6,7, 7,7,7,7 };
	
	BGBBTJ_BitStream bits;

	byte pxy[16], pxa[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;

	int min0[4], mcy0, mca0;
	int max0[4], ncy0, nca0;
	int avg0[4], acy0, aca0;
	int min1[4], mcy1, mca1;
	int max1[4], ncy1, nca1;
	int avg1[4], acy1, aca1;

	int acr, acg, acb, ank;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	byte *tp;

	tp=bgbbtj_bc7_partition2+(part*16);	

//	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY2(
//		pxy, min, max, avg,
//		&mcy, &ncy, &acy,
//		rgba, xstride, ystride);
		
	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA_Part2(
		pxy, pxa,
		min0, max0, avg0,
		min1, max1, avg1,
		&mcy0, &ncy0, &acy0,
		&mcy1, &ncy1, &acy1,
		&mca0, &nca0, &aca0,
		&mca1, &nca1, &aca1,
		rgba, xstride, ystride, part);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
//	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, 128, 8);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 6);

	if(pxy[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2A(
			pxy, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor2[part];
	if(pxy[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part2B(
			pxy, 16, mcy1, ncy1, part);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[3])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[3])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[3])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[3])>>3, 5);

//	BGBBTJ_BitsLE_WriteBit(&bits, min0[1]);
//	BGBBTJ_BitsLE_WriteBit(&bits, max0[1]);
//	BGBBTJ_BitsLE_WriteBit(&bits, min1[1]);
//	BGBBTJ_BitsLE_WriteBit(&bits, max1[1]);

	p0=(min0[0]&min0[1])|(min0[1]&min0[2])|(min0[0]&min0[2]);
	p1=(max0[0]&max0[1])|(max0[1]&max0[2])|(max0[0]&max0[2]);
	p2=(min1[0]&min1[1])|(min1[1]&min1[2])|(min1[0]&min1[2]);
	p3=(max1[0]&max1[1])|(max1[1]&max1[2])|(max1[0]&max1[2]);
	BGBBTJ_BitsLE_WriteBit(&bits, p0>>2);
	BGBBTJ_BitsLE_WriteBit(&bits, p1>>2);
	BGBBTJ_BitsLE_WriteBit(&bits, p2>>2);
	BGBBTJ_BitsLE_WriteBit(&bits, p3>>2);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=65536-2048;
	l3a=65536;
	ank=bgbbtj_bc7_anchor2[part];

	p0=idxtab[((pxy[0]-acy0)*l1+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==1)
		{
			if(i==ank)
			{
				p0=idxtab[((pxy[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_WriteBit(&bits, p0);
			}else
			{
				p0=idxtab[((pxy[i]-acy1)*l1+l3a)>>13];
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}
		}else
		{
			p0=idxtab[((pxy[i]-acy0)*l0+l3a)>>13];
			BGBBTJ_BitsLE_Write2Bits(&bits, p0);
		}
	}

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BC7_EncodeBlockBest_Part2(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte tblk[16], tpix[16*4];
	int terr, berr, part;

	part=BGBBTJ_BC7_PickBlockPartition2(rgba, xstride, ystride);

	BGBBTJ_BC7_EncodeBlock_Mode1(block, rgba, xstride, ystride, part);
	BGBBTJ_BC7_DecodeBlock(block, tpix, 4, 16, 0);
	berr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	
	BGBBTJ_BC7_EncodeBlock_Mode3(tblk, rgba, xstride, ystride, part);
	BGBBTJ_BC7_DecodeBlock(tblk, tpix, 4, 16, 0);
	terr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(terr<berr)
		{ memcpy(block, tblk, 16); berr=terr; }

	BGBBTJ_BC7_EncodeBlock_Mode7(tblk, rgba, xstride, ystride, part);
	BGBBTJ_BC7_DecodeBlock(tblk, tpix, 4, 16, 0);
	terr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(terr<berr)
		{ memcpy(block, tblk, 16); berr=terr; }

//	BGBBTJ_BC7_EncodeBlock_Mode4(block, rgba, xstride, ystride);
//	BGBBTJ_BC7_EncodeBlock_Mode5(block, rgba, xstride, ystride);
//	BGBBTJ_BC7_EncodeBlock_Mode6(block, rgba, xstride, ystride);
}
