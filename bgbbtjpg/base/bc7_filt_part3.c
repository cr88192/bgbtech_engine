/*
Encoder Logic for partitioned block modes.
This is for 3-partition blocks.
*/

#include <bgbbtj.h>

extern byte bgbbtj_bc7_partition2[64*16];
extern byte bgbbtj_bc7_partition3[64*16];
extern byte bgbbtj_bc7_anchor2[64];
extern byte bgbbtj_bc7_anchor3a[64];
extern byte bgbbtj_bc7_anchor3b[64];

extern byte bgbbtj_bc7_part2_idx[1<<16];
extern byte bgbbtj_bc7_part3_idx1[1<<16];
extern byte bgbbtj_bc7_part3_idx2[1<<16];
extern byte bgbbtj_bc7_part3_idxj[64*64];

int BGBBTJ_BC7_PickBlockPartition3(
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
	int idxa, idxb, pn, pna, pnb;
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
		idxa=0; idxb=0;
		for(i=0; i<16; i++)
		{
			idxa|=((pxu[i]>acu)<<i);
			idxb|=((pxv[i]>acv)<<i);
		}
		pna=bgbbtj_bc7_part3_idx1[idxa];
		pnb=bgbbtj_bc7_part3_idx2[idxb];
		pn=bgbbtj_bc7_part3_idxj[(pna<<6)|pnb];
	}else
	{
		idxa=0;
		for(i=0; i<16; i++)
		{
			idxa|=((pxv[i]>acv)<<i);
			idxb|=((pxu[i]>acu)<<i);
		}
		pna=bgbbtj_bc7_part3_idx1[idxa];
		pnb=bgbbtj_bc7_part3_idx2[idxb];
		pn=bgbbtj_bc7_part3_idxj[(pna<<6)|pnb];
	}
	return(pn);
}

#if 1
void BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY_Part3(
	byte *pxy,
	int *min_rgba0, int *max_rgba0, int *avg_rgba0,
	int *min_rgba1, int *max_rgba1, int *avg_rgba1,
	int *min_rgba2, int *max_rgba2, int *avg_rgba2,
	int *rmcy0, int *rncy0, int *racy0,
	int *rmcy1, int *rncy1, int *racy1,
	int *rmcy2, int *rncy2, int *racy2,
	byte *rgba, int xstride, int ystride,
	int part)
{
	byte pxyy[16], pxyr[16], pxyg[16], pxyb[16];
	byte *tpart;
	byte *tpxy0, *tpxy1, *tpxy2;

	int acr0, acg0, acb0, acy0, acn0;
	int acr1, acg1, acb1, acy1, acn1;
	int acr2, acg2, acb2, acy2, acn2;

	int mcr0, mcg0, mcb0, mcy0;
	int ncr0, ncg0, ncb0, ncy0;
	int mcr1, mcg1, mcb1, mcy1;
	int ncr1, ncg1, ncb1, ncy1;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;

	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;

	int mcr3y0, mcg3y0, mcb3y0, mca3y0, mcy3y0;
	int ncr3y0, ncg3y0, ncb3y0, nca3y0, ncy3y0;
	int mcr3y1, mcg3y1, mcb3y1, mca3y1, mcy3y1;
	int ncr3y1, ncg3y1, ncb3y1, nca3y1, ncy3y1;
	int mcr3y2, mcg3y2, mcb3y2, mca3y2, mcy3y2;
	int ncr3y2, ncg3y2, ncb3y2, nca3y2, ncy3y2;

	int mcr3r0, mcg3r0, mcb3r0, ncr3r0, ncg3r0, ncb3r0, mcy3r0, ncy3r0;
	int mcr3g0, mcg3g0, mcb3g0, ncr3g0, ncg3g0, ncb3g0, mcy3g0, ncy3g0;
	int mcr3b0, mcg3b0, mcb3b0, ncr3b0, ncg3b0, ncb3b0, mcy3b0, ncy3b0;
	int mcr3r1, mcg3r1, mcb3r1, ncr3r1, ncg3r1, ncb3r1, mcy3r1, ncy3r1;
	int mcr3g1, mcg3g1, mcb3g1, ncr3g1, ncg3g1, ncb3g1, mcy3g1, ncy3g1;
	int mcr3b1, mcg3b1, mcb3b1, ncr3b1, ncg3b1, ncb3b1, mcy3b1, ncy3b1;
	int mcr3r2, mcg3r2, mcb3r2, ncr3r2, ncg3r2, ncb3r2, mcy3r2, ncy3r2;
	int mcr3g2, mcg3g2, mcb3g2, ncr3g2, ncg3g2, ncb3g2, mcy3g2, ncy3g2;
	int mcr3b2, mcg3b2, mcb3b2, ncr3b2, ncg3b2, ncb3b2, mcy3b2, ncy3b2;

	int dr, dg, db, da, dy;
	int cr, cg, cb, ca, cy;
	int cyr, cyg, cyb;
	int i, j, k, l;

	tpart=bgbbtj_bc7_partition3+(part*16);

	acr0=  0; acg0=  0; acb0=  0; acy0=  0; acn0=  0;
	acr1=  0; acg1=  0; acb1=  0; acy1=  0; acn1=  0;
	acr2=  0; acg2=  0; acb2=  0; acy2=  0; acn2=  0;

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

	mcr3y2=255; mcg3y2=255; mcb3y2=255; mca3y2=255; mcy3y2=255;
	ncr3y2=  0; ncg3y2=  0; ncb3y2=  0; nca3y2=  0; ncy3y2=  0;
	mcr3r2=255; mcg3r2=255; mcb3r2=255; mcy3r2=255;
	ncr3r2=  0; ncg3r2=  0; ncb3r2=  0; ncy3r2=  0;
	mcr3g2=255; mcg3g2=255; mcb3g2=255; mcy3g2=255;
	ncr3g2=  0; ncg3g2=  0; ncb3g2=  0; ncy3g2=  0;
	mcr3b2=255; mcg3b2=255; mcb3b2=255; mcy3b2=255;
	ncr3b2=  0; ncg3b2=  0; ncb3b2=  0; ncy3b2=  0;

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

//		pxa[i*4+j]=ca;
//		pxy[i*4+j]=cy;

		pxyy[i*4+j]=cy;
		pxyr[i*4+j]=cyr;
		pxyg[i*4+j]=cyg;
		pxyb[i*4+j]=cyb;

		if(tpart[i*4+j]==2)
		{
			if(cy <mcy3y2) { mcr3y2=cr; mcg3y2=cg; mcb3y2=cb; mcy3y2=cy; }
			if(cy >ncy3y2) { ncr3y2=cr; ncg3y2=cg; ncb3y2=cb; ncy3y2=cy; }
			if(cyr<mcy3r2) { mcr3r2=cr; mcg3r2=cg; mcb3r2=cb; mcy3r2=cyr; }
			if(cyr>ncy3r2) { ncr3r2=cr; ncg3r2=cg; ncb3r2=cb; ncy3r2=cyr; }
			if(cyg<mcy3g2) { mcr3g2=cr; mcg3g2=cg; mcb3g2=cb; mcy3g2=cyg; }
			if(cyg>ncy3g2) { ncr3g2=cr; ncg3g2=cg; ncb3g2=cb; ncy3g2=cyg; }
			if(cyb<mcy3b2) { mcr3b2=cr; mcg3b2=cg; mcb3b2=cb; mcy3b2=cyb; }
			if(cyb>ncy3b2) { ncr3b2=cr; ncg3b2=cg; ncb3b2=cb; ncy3b2=cyb; }

			acr2+=cr; acg2+=cg;
			acb2+=cb; acy2+=cy;
			acn2++;
		}else if(tpart[i*4+j]==1)
		{
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


	mcr2=mcr3y2; mcg2=mcg3y2; mcb2=mcb3y2; mcy2=mcy3y2;
	ncr2=ncr3y2; ncg2=ncg3y2; ncb2=ncb3y2; ncy2=ncy3y2;
	tpxy2=pxyy; dy=ncy2-mcy2;

	dy=ncy3y2-mcy3y2;
	dr=ncy3r2-mcy3r2;
	dg=ncy3g2-mcy3g2;
	db=ncy3b2-mcy3b2;
	if(dr>dy)
	{
		mcr2=mcr3r2; mcg2=mcg3r2; mcb2=mcb3r2; mcy2=mcy3r2;
		ncr2=ncr3r2; ncg2=ncg3r2; ncb2=ncb3r2; ncy2=ncy3r2;
		tpxy2=pxyr; dy=ncy2-mcy2;
	}
	if(dg>dy)
	{
		mcr2=mcr3g2; mcg2=mcg3g2; mcb2=mcb3g2; mcy2=mcy3g2;
		ncr2=ncr3g2; ncg2=ncg3g2; ncb2=ncb3g2; ncy2=ncy3g2;
		tpxy2=pxyg; dy=ncy2-mcy2;
	}
	if(db>dy)
	{
		mcr2=mcr3b2; mcg2=mcg3b2; mcb2=mcb3b2; mcy2=mcy3b2;
		ncr2=ncr3b2; ncg2=ncg3b2; ncb2=ncb3b2; ncy2=ncy3b2;
		tpxy2=pxyb; dy=ncy2-mcy2;
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
	acr2=acr2/acn2;
	acg2=acg2/acn2;
	acb2=acb2/acn2;
	
	acy2=(acr2+(2*acg2)+acb2)>>2;
	mcy3=(mcr2+(2*mcg2)+mcb2)>>2;
	ncy3=(ncr2+(2*ncg2)+ncb2)>>2;

	l=(256*mcy3)/(acy2+1);
	cr=(acr2*l+128)>>8;
	cg=(acg2*l+128)>>8;
	cb=(acb2*l+128)>>8;
	mcr2=(3*mcr2+cr)>>2;
	mcg2=(3*mcg2+cg)>>2;
	mcb2=(3*mcb2+cb)>>2;
	
	l=(256*ncy3)/(acy2+1);
	cr=(acr2*l+128)>>8;
	cg=(acg2*l+128)>>8;
	cb=(acb2*l+128)>>8;
	ncr2=(3*ncr2+cr)>>2;
	ncg2=(3*ncg2+cg)>>2;
	ncb2=(3*ncb2+cb)>>2;
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

#if 1
	mcr2=(mcr2<0)?0:((mcr2>255)?255:mcr2);
	mcg2=(mcg2<0)?0:((mcg2>255)?255:mcg2);
	mcb2=(mcb2<0)?0:((mcb2>255)?255:mcb2);
	ncr2=(ncr2<0)?0:((ncr2>255)?255:ncr2);
	ncg2=(ncg2<0)?0:((ncg2>255)?255:ncg2);
	ncb2=(ncb2<0)?0:((ncb2>255)?255:ncb2);
#endif

//	for(i=0; i<16; i++)
//		{ pxy[i]=tpxy[i]; }

	for(i=0; i<16; i++)
	{
		pxy[i]=(tpart[i]==2)?tpxy2[i]:
			(tpart[i]?tpxy1[i]:tpxy0[i]);
	}

	min_rgba0[0]=mcr0;			min_rgba0[1]=mcg0;
	min_rgba0[2]=mcb0;			// min_rgba0[3]=mca0;
	max_rgba0[0]=ncr0;			max_rgba0[1]=ncg0;
	max_rgba0[2]=ncb0;			// max_rgba0[3]=nca0;
	avg_rgba0[0]=(mcr0+ncr0)>>1;
	avg_rgba0[1]=(mcg0+ncg0)>>1;
	avg_rgba0[2]=(mcb0+ncb0)>>1;
//	avg_rgba0[3]=(mca0+nca0)>>1;
	
	min_rgba1[0]=mcr1;			min_rgba1[1]=mcg1;
	min_rgba1[2]=mcb1;			// min_rgba1[3]=mca1;
	max_rgba1[0]=ncr1;			max_rgba1[1]=ncg1;
	max_rgba1[2]=ncb1;			// max_rgba1[3]=nca1;
	avg_rgba1[0]=(mcr1+ncr1)>>1;
	avg_rgba1[1]=(mcg1+ncg1)>>1;
	avg_rgba1[2]=(mcb1+ncb1)>>1;
//	avg_rgba1[3]=(mca1+nca1)>>1;

	min_rgba2[0]=mcr2;			min_rgba2[1]=mcg2;
	min_rgba2[2]=mcb2;			// min_rgba2[3]=mca2;
	max_rgba2[0]=ncr2;			max_rgba2[1]=ncg2;
	max_rgba2[2]=ncb2;			// max_rgba2[3]=nca2;
	avg_rgba2[0]=(mcr2+ncr2)>>1;
	avg_rgba2[1]=(mcg2+ncg2)>>1;
	avg_rgba2[2]=(mcb2+ncb2)>>1;
//	avg_rgba2[3]=(mca2+nca2)>>1;
	
	*rmcy0=mcy0; *rncy0=ncy0; *racy0=(mcy0+ncy0)>>1;
//	*rmca0=mca0; *rnca0=nca0; *raca0=(mca0+nca0)>>1;
	*rmcy1=mcy1; *rncy1=ncy1; *racy1=(mcy1+ncy1)>>1;
//	*rmca1=mca1; *rnca1=nca1; *raca1=(mca1+nca1)>>1;
	*rmcy2=mcy2; *rncy2=ncy2; *racy2=(mcy2+ncy2)>>1;
//	*rmca2=mca2; *rnca2=nca2; *raca2=(mca2+nca2)>>1;
}
#endif

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3A(
	byte *pxv, int n, int min, int max, int part)
{
	byte *tp;
	int i, j, k;

	tp=bgbbtj_bc7_partition3+(part*16);
	
	for(i=0; i<n; i++)
	{
		if(tp[i]==0)
			{ pxv[i]=max-(pxv[i]-min); }
	}
}

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3B(
	byte *pxv, int n, int min, int max, int part)
{
	byte *tp;
	int i, j, k;

	tp=bgbbtj_bc7_partition3+(part*16);
	
	for(i=0; i<n; i++)
	{
		if(tp[i]==1)
			{ pxv[i]=max-(pxv[i]-min); }
	}
}

void BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3C(
	byte *pxv, int n, int min, int max, int part)
{
	byte *tp;
	int i, j, k;

	tp=bgbbtj_bc7_partition3+(part*16);
	
	for(i=0; i<n; i++)
	{
		if(tp[i]==2)
			{ pxv[i]=max-(pxv[i]-min); }
	}
}


void BGBBTJ_BC7_EncodeBlock_Mode0(byte *block,
	byte *rgba, int xstride, int ystride, int part)
{
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
	int min2[4], mcy2, mca2;
	int max2[4], ncy2, nca2;
	int avg2[4], acy2, aca2;

	int acr, acg, acb, ank1, ank2;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	byte *tp;

	part&=15;
	tp=bgbbtj_bc7_partition3+(part*16);	

//	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY2(
//		pxy, min, max, avg,
//		&mcy, &ncy, &acy,
//		rgba, xstride, ystride);
		
//	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrYA_Part2(
//		pxy, pxa,
//		min0, max0, avg0,
//		min1, max1, avg1,
//		&mcy0, &ncy0, &acy0,
//		&mcy1, &ncy1, &acy1,
//		&mca0, &nca0, &aca0,
//		&mca1, &nca1, &aca1,
//		rgba, xstride, ystride, part);

	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY_Part3(
		pxy,
		min0, max0, avg0,
		min1, max1, avg1,
		min2, max2, avg2,
		&mcy0, &ncy0, &acy0,
		&mcy1, &ncy1, &acy1,
		&mcy2, &ncy2, &acy2,
		rgba, xstride, ystride, part);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
//	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, 1, 1);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 4);

	if(pxy[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3A(
			pxy, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor3a[part];
	if(pxy[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3B(
			pxy, 16, mcy1, ncy1, part);
	}

	i=bgbbtj_bc7_anchor3b[part];
	if(pxy[i]>=acy2)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min2, max2);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3C(
			pxy, 16, mcy2, ncy2, part);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (min2[0])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max2[0])>>4, 4);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (min2[1])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max2[1])>>4, 4);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (min2[2])>>4, 4);
	BGBBTJ_BitsLE_WriteBits(&bits, (max2[2])>>4, 4);

	p0=(min0[0]&min0[1])|(min0[1]&min0[2])|(min0[0]&min0[2]);
	p1=(max0[0]&max0[1])|(max0[1]&max0[2])|(max0[0]&max0[2]);
	p2=(min1[0]&min1[1])|(min1[1]&min1[2])|(min1[0]&min1[2]);
	p3=(max1[0]&max1[1])|(max1[1]&max1[2])|(max1[0]&max1[2]);
	p4=(min2[0]&min2[1])|(min2[1]&min2[2])|(min2[0]&min2[2]);
	p5=(max2[0]&max2[1])|(max2[1]&max2[2])|(max2[0]&max2[2]);
	BGBBTJ_BitsLE_WriteBit(&bits, p0>>3);
	BGBBTJ_BitsLE_WriteBit(&bits, p1>>3);
	BGBBTJ_BitsLE_WriteBit(&bits, p2>>3);
	BGBBTJ_BitsLE_WriteBit(&bits, p3>>3);
	BGBBTJ_BitsLE_WriteBit(&bits, p4>>3);
	BGBBTJ_BitsLE_WriteBit(&bits, p5>>3);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=32768/(ncy2-acy2+1);		//Fix-Point Scale (Luma 2)
//	l2=65536-2048;
	l3a=65536;

	ank1=bgbbtj_bc7_anchor3a[part];
	ank2=bgbbtj_bc7_anchor3b[part];

	p0=idxtab2[((pxy[0]-acy0)*l1+l3a)>>13];
	BGBBTJ_BitsLE_Write2Bits(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==2)
		{
			if(i==ank2)
			{
				p0=idxtab2[((pxy[i]-acy2)*l2+l3a)>>13];
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}else
			{
				p0=idxtab2[((pxy[i]-acy2)*l2+l3a)>>13];
				BGBBTJ_BitsLE_Write3Bits(&bits, p0);
			}
		}else if(tp[i]==1)
		{
			if(i==ank1)
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

void BGBBTJ_BC7_EncodeBlock_Mode2(byte *block,
	byte *rgba, int xstride, int ystride, int part)
{
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
	int min2[4], mcy2, mca2;
	int max2[4], ncy2, nca2;
	int avg2[4], acy2, aca2;

	int acr, acg, acb, ank1, ank2;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	byte *tp;

	tp=bgbbtj_bc7_partition3+(part*16);	

	BGBBTJ_BC7_EncodeBlock_SplitMinMaxClrY_Part3(
		pxy,
		min0, max0, avg0,
		min1, max1, avg1,
		min2, max2, avg2,
		&mcy0, &ncy0, &acy0,
		&mcy1, &ncy1, &acy1,
		&mcy2, &ncy2, &acy2,
		rgba, xstride, ystride, part);

	BGBBTJ_BitsLE_ClearSetupWrite(&bits, block, 16);
//	BGBBTJ_BitsLE_WriteBits(&bits, 64, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, 4, 3);
	BGBBTJ_BitsLE_WriteBits(&bits, part, 6);

	if(pxy[0]>=acy0)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min0, max0);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3A(
			pxy, 16, mcy0, ncy0, part);
	}

	i=bgbbtj_bc7_anchor3a[part];
	if(pxy[i]>=acy1)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min1, max1);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3B(
			pxy, 16, mcy1, ncy1, part);
	}

	i=bgbbtj_bc7_anchor3b[part];
	if(pxy[i]>=acy2)
	{
		BGBBTJ_BC7_EncodeBlock_VecSwapRGBA(min2, max2);
		BGBBTJ_BC7_EncodeBlock_VecInvertPixels_Part3C(
			pxy, 16, mcy2, ncy2, part);
	}

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min2[0])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max2[0])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min2[1])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max2[1])>>3, 5);

	BGBBTJ_BitsLE_WriteBits(&bits, (min0[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max0[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min1[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max1[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (min2[2])>>3, 5);
	BGBBTJ_BitsLE_WriteBits(&bits, (max2[2])>>3, 5);

	l0=32768/(ncy0-acy0+1);		//Fix-Point Scale (Luma 0)
	l1=32768/(ncy1-acy1+1);		//Fix-Point Scale (Luma 1)
	l2=32768/(ncy2-acy2+1);		//Fix-Point Scale (Luma 2)
//	l2=65536-2048;
	l3a=65536;

	ank1=bgbbtj_bc7_anchor3a[part];
	ank2=bgbbtj_bc7_anchor3b[part];

	p0=idxtab2[((pxy[0]-acy0)*l1+l3a)>>13];
	BGBBTJ_BitsLE_WriteBit(&bits, p0);
	for(i=1; i<16; i++)
	{
		if(tp[i]==2)
		{
			if(i==ank2)
			{
				p0=idxtab[((pxy[i]-acy2)*l2+l3a)>>13];
				BGBBTJ_BitsLE_WriteBit(&bits, p0);
			}else
			{
				p0=idxtab[((pxy[i]-acy2)*l2+l3a)>>13];
				BGBBTJ_BitsLE_Write2Bits(&bits, p0);
			}
		}else if(tp[i]==1)
		{
			if(i==ank1)
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

void BGBBTJ_BC7_EncodeBlockBest_Part3(byte *block,
	byte *rgba, int xstride, int ystride)
{
	byte tblk[16], tpix[16*4];
	int terr, berr, part;

	part=BGBBTJ_BC7_PickBlockPartition3(rgba, xstride, ystride);

	BGBBTJ_BC7_EncodeBlock_Mode0(block, rgba, xstride, ystride, part);
	BGBBTJ_BC7_DecodeBlock(block, tpix, 4, 16, 0);
	berr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	
	BGBBTJ_BC7_EncodeBlock_Mode2(tblk, rgba, xstride, ystride, part);
	BGBBTJ_BC7_DecodeBlock(tblk, tpix, 4, 16, 0);
	terr=BGBBTJ_BC7_CalcBlockError(rgba, xstride, ystride, tpix, 4, 16);
	if(terr<berr)
		{ memcpy(block, tblk, 16); berr=terr; }
}
