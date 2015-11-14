#include <bgbbtj.h>

int BGBBTJ_BCn_DistRGB(byte *rgb0, byte *rgb1)
{
	int dr, dg, db, dy;
	int d, y0, y1;
	
	y0=(rgb0[0]+(2*rgb0[1])+rgb0[2])>>2;
	y1=(rgb1[0]+(2*rgb1[1])+rgb1[2])>>2;

	dr=rgb0[0]-rgb1[0];
	dg=rgb0[1]-rgb1[1];
	db=rgb0[2]-rgb1[2];
	dy=y0-y1;
	
	d=dr*dr + dg*dg + db*db + dy*dy;
	return(d);
}

int BGBBTJ_BCn_IndexRGB_Clr2(byte *rgb, byte *clr0, byte *clr1)
{
	int bi, bd, d;

	if(rgb[3]<128)return(3);

	bi=0; bd=BGBBTJ_BCn_DistRGB(clr0, rgb);
	d=BGBBTJ_BCn_DistRGB(clr1, rgb);
	if(d<bd) { bi=1; bd=d; }
	return(bi);
}

int BGBBTJ_BCn_IndexRGB_Clr3(byte *rgb,
	byte *clr0, byte *clr1, byte *clr2)
{
	int bi, bd, d;
	
	if(rgb[3]<128)return(3);
	
	bi=0; bd=BGBBTJ_BCn_DistRGB(clr0, rgb);
	
	d=BGBBTJ_BCn_DistRGB(clr1, rgb);
	if(d<bd) { bi=1; bd=d; }
	d=BGBBTJ_BCn_DistRGB(clr2, rgb);
	if(d<bd) { bi=2; bd=d; }
	return(bi);
}

int BGBBTJ_BCn_IndexRGB_Clr4(byte *rgb,
	byte *clr0, byte *clr1, byte *clr2, byte *clr3)
{
	int bi, bd, d;
	
	bi=0; bd=BGBBTJ_BCn_DistRGB(clr0, rgb);
	
	d=BGBBTJ_BCn_DistRGB(clr1, rgb);
	if(d<bd) { bi=1; bd=d; }
	d=BGBBTJ_BCn_DistRGB(clr2, rgb);
	if(d<bd) { bi=2; bd=d; }
	d=BGBBTJ_BCn_DistRGB(clr3, rgb);
	if(d<bd) { bi=3; bd=d; }
	return(bi);
}

void BGBBTJ_BCn_EncodeBlockDXT1_Clr4(byte *block,
	byte *clr0, byte *clr1, byte *clr2, byte *clr3,
	byte *rgba,
	int xstride, int ystride)
{
	int cr, cg, cb;
	int p0, p1, p2, p3;
	int i;

//	block[0]=(clr0[0]&0xF8) | (clr0[1]>>5);
//	block[1]=((clr0[1]<<3)&0xE0) | (clr0[2]>>3);
//	block[2]=(clr1[0]&0xF8) | (clr1[1]>>5);
//	block[3]=((clr1[1]<<3)&0xE0) | (clr1[2]>>3);

//	block[0]=((clr0[1]<<3)&0xE0) | (clr0[2]>>3);
//	block[1]=(clr0[0]&0xF8) | (clr0[1]>>5);
//	block[2]=((clr1[1]<<3)&0xE0) | (clr1[2]>>3);
//	block[3]=(clr1[0]&0xF8) | (clr1[1]>>5);
	
	cr=(clr0[0]+4)>>3;
	cg=(clr0[1]+2)>>2;
	cb=(clr0[2]+4)>>3;
	if(cr>31)cr=31;
	if(cg>63)cg=63;
	if(cb>31)cb=31;

	block[0]=cb|(cg<<5);
	block[1]=(cg>>3)|(cr<<3);

	cr=(clr1[0]+4)>>3;
	cg=(clr1[1]+2)>>2;
	cb=(clr1[2]+4)>>3;
	if(cr>31)cr=31;
	if(cg>63)cg=63;
	if(cb>31)cb=31;

	block[2]=cb|(cg<<5);
	block[3]=(cg>>3)|(cr<<3);

	for(i=0; i<4; i++)
	{
		if(clr3)
		{
			p0=BGBBTJ_BCn_IndexRGB_Clr4(rgba+(0*xstride)+(i*ystride),
				clr0, clr1, clr2, clr3);
			p1=BGBBTJ_BCn_IndexRGB_Clr4(rgba+(1*xstride)+(i*ystride),
				clr0, clr1, clr2, clr3);
			p2=BGBBTJ_BCn_IndexRGB_Clr4(rgba+(2*xstride)+(i*ystride),
				clr0, clr1, clr2, clr3);
			p3=BGBBTJ_BCn_IndexRGB_Clr4(rgba+(3*xstride)+(i*ystride),
				clr0, clr1, clr2, clr3);
		}else if(clr2)
		{
			p0=BGBBTJ_BCn_IndexRGB_Clr3(rgba+(0*xstride)+(i*ystride),
				clr0, clr1, clr2);
			p1=BGBBTJ_BCn_IndexRGB_Clr3(rgba+(1*xstride)+(i*ystride),
				clr0, clr1, clr2);
			p2=BGBBTJ_BCn_IndexRGB_Clr3(rgba+(2*xstride)+(i*ystride),
				clr0, clr1, clr2);
			p3=BGBBTJ_BCn_IndexRGB_Clr3(rgba+(3*xstride)+(i*ystride),
				clr0, clr1, clr2);
		}else
		{
			p0=BGBBTJ_BCn_IndexRGB_Clr2(rgba+(0*xstride)+(i*ystride),
				clr0, clr1);
			p1=BGBBTJ_BCn_IndexRGB_Clr2(rgba+(1*xstride)+(i*ystride),
				clr0, clr1);
			p2=BGBBTJ_BCn_IndexRGB_Clr2(rgba+(2*xstride)+(i*ystride),
				clr0, clr1);
			p3=BGBBTJ_BCn_IndexRGB_Clr2(rgba+(3*xstride)+(i*ystride),
				clr0, clr1);
		}
		block[4+i]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}

int BGBBTJ_BCn_ClrTo565(byte *clr, byte *oclr)
{
	int cr, cg, cb;
	int i;

	cr=(clr[0]+4)>>3;
	cg=(clr[1]+2)>>2;
	cb=(clr[2]+4)>>3;
	if(cr>31)cr=31;
	if(cg>63)cg=63;
	if(cb>31)cb=31;
	
	if(oclr)
	{
		oclr[0]=cr<<3;
		oclr[1]=cg<<2;
		oclr[2]=cb<<3;
	}
	
	i=cb|(cg<<5)|(cr<<11);
	return(i);
}

void BGBBTJ_BCn_EncodeBlockDXT1(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	int min[4], max[4], avg[3];
	byte clr0[4], clr1[4], clr2[4], clr3[4];
	int cr, cg, cb, cy;
	int i, j, k;
	
	min[0]=255;	min[1]=255;	min[2]=255; min[3]= 384;
	max[0]=  0;	max[1]=  0;	max[2]=  0; max[3]=-384;
	avg[0]=  0;	avg[1]=  0;	avg[2]=  0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];
		cg=rgba[k+1];
		cb=rgba[k+2];
		cy=(cr+2*cg+cb)>>2;
		
		if(cy<min[3])
			{ min[0]=cr; min[1]=cg; min[2]=cb; min[3]=cy; }
		if(cy>max[3])
			{ max[0]=cr; max[1]=cg; max[2]=cb; max[3]=cy; }
		
#if 0
		if(cr<min[0])min[0]=cr;
		if(cg<min[1])min[1]=cg;
		if(cb<min[2])min[2]=cb;
		if(cr>max[0])max[0]=cr;
		if(cg>max[1])max[1]=cg;
		if(cb>max[2])max[2]=cb;
#endif
		avg[0]+=cr;
		avg[1]+=cg;
		avg[2]+=cb;
	}
	
	avg[0]=(avg[0]+8)>>4;
	avg[1]=(avg[1]+8)>>4;
	avg[2]=(avg[2]+8)>>4;
	if(avg[0]>255)avg[0]=255;
	if(avg[1]>255)avg[1]=255;
	if(avg[2]>255)avg[2]=255;

#if 0
	clr0[0]=(min[0]*11+avg[0]*5)>>4;
	clr0[1]=(min[1]*11+avg[1]*5)>>4;
	clr0[2]=(min[2]*11+avg[2]*5)>>4;
	clr1[0]=(max[0]*11+avg[0]*5)>>4;
	clr1[1]=(max[1]*11+avg[1]*5)>>4;
	clr1[2]=(max[2]*11+avg[2]*5)>>4;
#endif

#if 1
	clr0[0]=min[0];
	clr0[1]=min[1];
	clr0[2]=min[2];
	clr1[0]=max[0];
	clr1[1]=max[1];
	clr1[2]=max[2];
#endif

	if(nclr>=0)
	{
//		i=BGBBTJ_BCn_ClrTo565(clr0, NULL);
//		j=BGBBTJ_BCn_ClrTo565(clr1, NULL);
		i=BGBBTJ_BCn_ClrTo565(clr0, clr0);
		j=BGBBTJ_BCn_ClrTo565(clr1, clr1);
		if(j<i)
//		if(j>i)
		{
			k=clr0[0]; clr0[0]=clr1[0]; clr1[0]=k;
			k=clr0[1]; clr0[1]=clr1[1]; clr1[1]=k;
			k=clr0[2]; clr0[2]=clr1[2]; clr1[2]=k;
		}
	}else
	{
//		i=BGBBTJ_BCn_ClrTo565(clr0, NULL);
//		j=BGBBTJ_BCn_ClrTo565(clr1, NULL);
		i=BGBBTJ_BCn_ClrTo565(clr0, clr0);
		j=BGBBTJ_BCn_ClrTo565(clr1, clr1);
		if(j>i)
//		if(j<i)
		{
			k=clr0[0]; clr0[0]=clr1[0]; clr1[0]=k;
			k=clr0[1]; clr0[1]=clr1[1]; clr1[1]=k;
			k=clr0[2]; clr0[2]=clr1[2]; clr1[2]=k;
		}
		
		if((i==j) && (nclr==(-4)))
		{
			nclr=3;
		}
	}

	switch(nclr)
	{
	case 2:
	case -2:
		BGBBTJ_BCn_EncodeBlockDXT1_Clr4(block,
			clr0, clr1, NULL, NULL,
			rgba, xstride, ystride);
		break;
	case 3:
	case -3:
		clr2[0]=(clr0[0]+clr1[0])>>1;
		clr2[1]=(clr0[1]+clr1[1])>>1;
		clr2[2]=(clr0[2]+clr1[2])>>1;
		BGBBTJ_BCn_EncodeBlockDXT1_Clr4(block,
			clr0, clr1, clr2, NULL,
			rgba, xstride, ystride);
		break;
	case 4:
	case -4:
		clr2[0]=(clr0[0]*171+clr1[0]*85+128)>>8;
		clr2[1]=(clr0[1]*171+clr1[1]*85+128)>>8;
		clr2[2]=(clr0[2]*171+clr1[2]*85+128)>>8;
		clr3[0]=(clr0[0]*85+clr1[0]*171+128)>>8;
		clr3[1]=(clr0[1]*85+clr1[1]*171+128)>>8;
		clr3[2]=(clr0[2]*85+clr1[2]*171+128)>>8;
		BGBBTJ_BCn_EncodeBlockDXT1_Clr4(block,
			clr0, clr1, clr2, clr3,
			rgba, xstride, ystride);
		break;
	}
}

void BGBBTJ_BCn_EncodeBlockDXT1F(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
//	int min[3], max[3], avg[3];
	byte clr0[4], clr1[4];
	byte pxy[16];
	int p0, p1, p2, p3;
	int mcr, mcg, mcb, mcy;
	int ncr, ncg, ncb, ncy;
	int acr, acg, acb;
	int cr, cg, cb, cy, acy;
	int i, j, k;
	
	mcr=255; mcg=255; mcb=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];
		cg=rgba[k+1];
		cb=rgba[k+2];
		cy=(cr+2*cg+cb)>>2;
		pxy[i*4+j]=cy;

		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
//		if(cr<mcr)mcr=cr;
//		if(cg<mcg)mcg=cg;
//		if(cb<mcb)mcb=cb;
//		if(cr>ncr)ncr=cr;
//		if(cg>ncg)ncg=cg;
//		if(cb>ncb)ncb=cb;
		acr+=cr;
		acg+=cg;
		acb+=cb;
	}
	
	acr=acr>>4;
	acg=acg>>4;
	acb=acb>>4;

	clr0[0]=(mcr*11+acr*5)>>4;
	clr0[1]=(mcg*11+acg*5)>>4;
	clr0[2]=(mcb*11+acb*5)>>4;
	clr1[0]=(ncr*11+acr*5)>>4;
	clr1[1]=(ncg*11+acg*5)>>4;
	clr1[2]=(ncb*11+acb*5)>>4;

	block[0]=((clr0[1]<<3)&0xE0) | (clr0[2]>>3);
	block[1]=(clr0[0]&0xF8) | (clr0[1]>>5);
	block[2]=((clr1[1]<<3)&0xE0) | (clr1[2]>>3);
	block[3]=(clr1[0]&0xF8) | (clr1[1]>>5);
	
	clr0[3]=(clr0[0]+2*clr0[1]+clr0[2])>>2;
	clr1[3]=(clr1[0]+2*clr1[1]+clr1[2])>>2;

	acy=(clr0[3]+clr1[3])>>1;

#if 0
	for(i=0; i<4; i++)
	{
		k=i*4;
		p0=-((acy-pxy[k+0])>>16);
		p1=-((acy-pxy[k+1])>>16);
		p2=-((acy-pxy[k+2])>>16);
		p3=-((acy-pxy[k+3])>>16);
		block[4+i]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
#endif

	p0=-((acy-pxy[ 0])>>16);
	p1=-((acy-pxy[ 1])>>16);
	p2=-((acy-pxy[ 2])>>16);
	p3=-((acy-pxy[ 3])>>16);
	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[ 4])>>16);
	p1=-((acy-pxy[ 5])>>16);
	p2=-((acy-pxy[ 6])>>16);
	p3=-((acy-pxy[ 7])>>16);
	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[ 8])>>16);
	p1=-((acy-pxy[ 9])>>16);
	p2=-((acy-pxy[10])>>16);
	p3=-((acy-pxy[11])>>16);
	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[12])>>16);
	p1=-((acy-pxy[13])>>16);
	p2=-((acy-pxy[14])>>16);
	p3=-((acy-pxy[15])>>16);
	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}

#if 1
void BGBBTJ_BCn_EncodeBlockDXT1A(byte *block,
	byte *rgba, int xstride, int ystride)
{
	static char idxtaba[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static char idxtabb[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static char idxtabc[16]=
		{ 0,0,0,0, 0,0,2,2, 2,2,1,1, 1,1,1,1 };
	static char idxtabd[16]=
		{ 1,1,1,1, 1,1,2,2, 2,2,0,0, 0,0,0,0 };
	byte pxa[16], pxy[16];
	char *idxtab;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;
//	int mcr3, mcg3, mcb3, mcy3;
//	int ncr3, ncg3, ncb3, ncy3;
	int mcr3r, mcg3r, mcb3r, ncr3r, ncg3r, ncb3r;
	int mcr3g, mcg3g, mcb3g, ncr3g, ncg3g, ncb3g;
	int mcr3b, mcg3b, mcb3b, ncr3b, ncg3b, ncb3b;
	int cr, cg, cb, ca, cy;
	int dr, dg, db, da, dy, alty;
	int drr, dgr, dbr, drg, dgg, dbg, drb, dgb, dbb;
	int i, j, k, l;
	
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	mcr2=255; mcg2=255; mcb2=255; mcy2=255;
	ncr2=  0; ncg2=  0; ncb2=  0; ncy2=  0;
//	mcr3=255; mcg3=255; mcb3=255; mcy3=255;
//	ncr3=  0; ncg3=  0; ncb3=  0; ncy3=  0;
	mcr3r=255; mcg3r=255; mcb3r=255;
	ncr3r=  0; ncg3r=  0; ncb3r=  0;
	mcr3g=255; mcg3g=255; mcb3g=255;
	ncr3g=  0; ncg3g=  0; ncb3g=  0;
	mcr3b=255; mcg3b=255; mcb3b=255;
	ncr3b=  0; ncg3b=  0; ncb3b=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		if(xstride==3)
			{ ca=255; }

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
//		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
//		if((cy<mcy) && (ca>=64)) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if((cy>ncy) && (ca>=64)) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }

		if(cy<mcy)
		{
			if(ca>=64) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
			mcr2=cr; mcg2=cg; mcb2=cb; mcy2=cy;
		}
		if(cy>ncy)
		{
			if(ca>=64) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
			ncr2=cr; ncg2=cg; ncb2=cb; ncy2=cy;
		}
		
//		if(cr<mcr3) { mcr3=cr; }
//		if(cg<mcg3) { mcg3=cg; }
//		if(cb<mcb3) { mcb3=cb; }
//		if(cr>ncr3) { ncr3=cr; }
//		if(cg>ncg3) { ncg3=cg; }
//		if(cb>ncb3) { ncb3=cb; }

#if 0
		if(cr<mcr3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; }
		if(cg<mcg3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; }
		if(cb<mcb3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; }
		if(cr>ncr3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; }
		if(cg>ncg3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; }
		if(cb>ncb3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; }
#endif

		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}

#if 1
	if(nca<64)
	{
		mcr=mcr2; mcg=mcg2; mcb=mcb2; mcy=mcy2;
		ncr=ncr2; ncg=ncg2; ncb=ncb2; ncy=ncy2;
	}
#endif

	alty=0;

#if 0
	drr=ncr3r-mcr3r;	dgr=ncg3r-mcg3r;	dbr=ncb3r-mcb3r;
	drg=ncr3g-mcr3g;	dgg=ncg3g-mcg3g;	dbg=ncb3g-mcb3g;
	drb=ncr3b-mcr3b;	dgb=ncg3b-mcg3b;	dbb=ncb3b-mcb3b;
	dy=ncy-mcy;
//	if((dr>dy) || (dg>dy) || (db>dy))
	if((drr>dy) || (dgg>dy) || (dbb>dy))
	{
		if((drr>dgg) && (drr>dbb))
		{
			mcr=mcr3r; mcg=mcg3r; mcb=mcb3r;
			ncr=ncr3r; ncg=ncg3r; ncb=ncb3r;
		}else if(dgg>=dbb)
		{
			mcr=mcr3g; mcg=mcg3g; mcb=mcb3g;
			ncr=ncr3g; ncg=ncg3g; ncb=ncb3g;
		}else
		{
			mcr=mcr3b; mcg=mcg3b; mcb=mcb3b;
			ncr=ncr3b; ncg=ncg3b; ncb=ncb3b;
		}

		mcy=255; ncy=0;

		dr=ncr-mcr;
		dg=ncg-mcg;
		db=ncb-mcb;

		//normalize
		da=sqrt((dr*dr)+(dg*dg)+(db*db));
		dr=(256*dr)/da;
		dg=(256*dg)/da;
		db=(256*db)/da;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			cr=rgba[k+0];	cg=rgba[k+1];
			cb=rgba[k+2];
//			cy=(cr+(2*cg)+cb)>>2;		//Luma
			cy=(cr*dr+cg*dg+cb*db)>>8;

			pxy[i*4+j]=cy;

			if(cy<mcy) { mcy=cy; }
			if(cy>ncy) { ncy=cy; }
		}
		
		alty=1;
	}
#endif

#if 0
	dr=ncr3-mcr3;	dg=ncg3-mcg3;
	db=ncb3-mcb3;	dy=ncy-mcy;
	if((dr>dy) || (dg>dy) || (db>dy))
	{
		mcr=mcr3; mcg=mcg3; mcb=mcb3; mcy=mcy3;
		ncr=ncr3; ncg=ncg3; ncb=ncb3; ncy=ncy3;
		mcy=255; ncy=0;

		//normalize
		da=sqrt((dr*dr)+(dg*dg)+(db*db));
		dr=(256*dr)/da;
		dg=(256*dg)/da;
		db=(256*db)/da;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			cr=rgba[k+0];	cg=rgba[k+1];
			cb=rgba[k+2];
//			cy=(cr+(2*cg)+cb)>>2;		//Luma
			cy=(cr*dr+cg*dg+cb*db)>>8;

			pxy[i*4+j]=cy;

			if(cy<mcy) { mcy=cy; }
			if(cy>ncy) { ncy=cy; }
		}
		
		alty=1;
	}
#endif

#if 0
	if(nca<64)
	{
		cr=rgba[0];	cg=rgba[1];
		cb=rgba[2];	ca=rgba[3];
		
		cy=(cr+(2*cg)+cb)>>2;		//Luma
		mcr=cr; mcg=cg; mcb=cb; mcy=cy;
		ncr=cr; ncg=cg; ncb=cb; ncy=cy;
	}
#endif

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	acy=(acr+(2*acg)+acb)>>2;

	if(!alty)
	{
		l=(256*mcy)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
//		mcr=(mcr+cr)>>1;
//		mcg=(mcg+cg)>>1;
//		mcb=(mcb+cb)>>1;

		mcr=(3*mcr+cr)>>2;
		mcg=(3*mcg+cg)>>2;
		mcb=(3*mcb+cb)>>2;
	
		l=(256*ncy)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
//		ncr=(ncr+cr)>>1;
//		ncg=(ncg+cg)>>1;
//		ncb=(ncb+cb)>>1;

		ncr=(3*ncr+cr)>>2;
		ncg=(3*ncg+cg)>>2;
		ncb=(3*ncb+cb)>>2;
	}
#endif

#if 1
	mcr=((mcr+3)>>3)<<3;
	mcg=((mcg+1)>>2)<<2;
	mcb=((mcb+3)>>3)<<3;
	ncr=((ncr+5)>>3)<<3;
	ncg=((ncg+3)>>2)<<2;
	ncb=((ncb+5)>>3)<<3;
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;

	if(nca<128)
//	if(0)
	{
		//Fully Transparent Block
		i=((acr<<8)&0xF800) | ((acg<<3)&0x07E0) | ((acb>>3)&0x001F);
		block[0]=i;	block[1]=i>>8;
		block[2]=i;	block[3]=i>>8;

		block[4]=255;	block[5]=255;
		block[6]=255;	block[7]=255;
	}else
		if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
//		if(0)
	{	
		//Flat Color Block
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		if(j<=i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
		}else
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
		}

		if(i!=j)
		{
			block[4]=(2<<6)|(3<<4)|(2<<2)|3;
			block[5]=(3<<6)|(2<<4)|(3<<2)|2;
			block[6]=(2<<6)|(3<<4)|(2<<2)|3;
			block[7]=(3<<6)|(2<<4)|(3<<2)|2;
		}else
		{
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
		}
	}else
		if(mca>=128)
//		if(0)
	{
		//Emit RGB Block (Opaque)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(j<i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
			idxtab=idxtabb;
		}else
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtabc;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}else
	{
		//Emit RGB Block (Alpha)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(i<=j)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtabc;
		}else
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
			idxtab=idxtabd;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13] | (((pxa[ 0]-128)>>10)&3);
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13] | (((pxa[ 1]-128)>>10)&3);
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13] | (((pxa[ 2]-128)>>10)&3);
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13] | (((pxa[ 3]-128)>>10)&3);
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13] | (((pxa[ 4]-128)>>10)&3);
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13] | (((pxa[ 5]-128)>>10)&3);
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13] | (((pxa[ 6]-128)>>10)&3);
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13] | (((pxa[ 7]-128)>>10)&3);
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13] | (((pxa[ 8]-128)>>10)&3);
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13] | (((pxa[ 9]-128)>>10)&3);
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13] | (((pxa[10]-128)>>10)&3);
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13] | (((pxa[11]-128)>>10)&3);
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13] | (((pxa[12]-128)>>10)&3);
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13] | (((pxa[13]-128)>>10)&3);
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13] | (((pxa[14]-128)>>10)&3);
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13] | (((pxa[15]-128)>>10)&3);
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}
#endif

#if 1
void BGBBTJ_BCn_EncodeBlockDXT1B(byte *block,
	byte *rgba, int xstride, int ystride)
{
	static char idxtaba[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static char idxtabb[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static char idxtabc[16]=
		{ 0,0,0,0, 0,0,2,2, 2,2,1,1, 1,1,1,1 };
	byte pxa[16], pxy[16];
	char *idxtab;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;

	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	mcr2=255; mcg2=255; mcb2=255; mcy2=255;
	ncr2=  0; ncg2=  0; ncb2=  0; ncy2=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
//		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
//		if((cy<mcy) && (ca>=64)) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if((cy>ncy) && (ca>=64)) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }

		if(cy<mcy)
		{
			if(ca>=64) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
			mcr2=cr; mcg2=cg; mcb2=cb; mcy2=cy;
		}
		if(cy>ncy)
		{
			if(ca>=64) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
			ncr2=cr; ncg2=cg; ncb2=cb; ncy2=cy;
		}

		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}

#if 1
	if(nca<64)
	{
		mcr=mcr2; mcg=mcg2; mcb=mcb2; mcy=mcy2;
		ncr=ncr2; ncg=ncg2; ncb=ncb2; ncy=ncy2;
	}
#endif

#if 0
	if(nca<64)
	{
		cr=rgba[0];	cg=rgba[1];
		cb=rgba[2];	ca=rgba[3];
		
		cy=(cr+(2*cg)+cb)>>2;		//Luma
		mcr=cr; mcg=cg; mcb=cb; mcy=cy;
		ncr=cr; ncg=cg; ncb=cb; ncy=cy;
	}
#endif

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	acy=(acr+(2*acg)+acb)>>2;
	
	l=(256*mcy)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	
	mcr=(mcr+cr)>>1;
	mcg=(mcg+cg)>>1;
	mcb=(mcb+cb)>>1;
	

	l=(256*ncy)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	
	ncr=(ncr+cr)>>1;
	ncg=(ncg+cg)>>1;
	ncb=(ncb+cb)>>1;
#endif

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;

	if((mcr==ncr) && (mcg==ncg) && (mcb==ncb))
	{
		//Flat Color Block
#if 1
		mcr=((mcr+3)>>3)<<3;
		mcg=((mcg+1)>>2)<<2;
		mcb=((mcb+3)>>3)<<3;
		ncr=((ncr+5)>>3)<<3;
		ncg=((ncg+3)>>2)<<2;
		ncb=((ncb+5)>>3)<<3;
		mcr=(mcr<0)?0:((mcr>255)?255:mcr);
		mcg=(mcg<0)?0:((mcg>255)?255:mcg);
		mcb=(mcb<0)?0:((mcb>255)?255:mcb);
		ncr=(ncr<0)?0:((ncr>255)?255:ncr);
		ncg=(ncg<0)?0:((ncg>255)?255:ncg);
		ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		if(j<=i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
		}else
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
		}

		if(i!=j)
		{
			block[4]=(2<<6)|(3<<4)|(2<<2)|3;
			block[5]=(3<<6)|(2<<4)|(3<<2)|2;
			block[6]=(2<<6)|(3<<4)|(2<<2)|3;
			block[7]=(3<<6)|(2<<4)|(3<<2)|2;
		}else
		{
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
		}
	}else
	{
		//Emit RGB Block
#if 1
		mcr=((mcr+3)>>3)<<3;
		mcg=((mcg+1)>>2)<<2;
		mcb=((mcb+3)>>3)<<3;
		ncr=((ncr+5)>>3)<<3;
		ncg=((ncg+3)>>2)<<2;
		ncb=((ncb+5)>>3)<<3;
		mcr=(mcr<0)?0:((mcr>255)?255:mcr);
		mcg=(mcg<0)?0:((mcg>255)?255:mcg);
		mcb=(mcb<0)?0:((mcb>255)?255:mcb);
		ncr=(ncr<0)?0:((ncr>255)?255:ncr);
		ncg=(ncg<0)?0:((ncg>255)?255:ncg);
		ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(j<i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
			idxtab=idxtabb;
		}else
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtabc;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}
#endif


BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_EncodeBlockDXT1(
//			block+(i*xs1+j)*8,
//			rgba+(i*4*xs+j*4)*stride,
//			stride, xs*stride, -4);

//		BGBBTJ_BCn_EncodeBlockDXT1B(
//			block+(i*xs1+j)*8,
//			rgba+(i*4*xs+j*4)*stride,
//			stride, xs*stride);

		BGBBTJ_BCn_EncodeBlockDXT1A(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1A(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_EncodeBlockDXT1(
//			block+(i*xs1+j)*8,
//			rgba+(i*4*xs+j*4)*stride,
//			stride, xs*stride, 3);

		BGBBTJ_BCn_EncodeBlockDXT1A(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1F(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1F(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 3);
	}
}


void BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(byte *block,
	short *pxy, short *pxu, short *pxv)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };

	int p0, p1, p2, p3;
	int mcy, ncy, acy;
	int acr, acg, acb;
	int mcu, mcv;
	int ncu, ncv;
	int cy, cu, cv;
	int cra, cga, cba;
	int crb, cgb, cbb;
//	int u1, v1, l0, l1, l2;
	int l0, l1, l2, l3, l4, l5;
	int u0, v0, u1, v1, u2, v2, u3, v3;
	int i, j, k;

#if 0
	mcy=255; ncy=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=pxy[i*8+j];
		if(k<mcy)mcy=k;
		if(k>ncy)ncy=k;
	}
#endif

#if 1
	u0=pxu[0]; v0=pxv[0];
	u1=pxu[1]; v1=pxv[1];
	u2=pxu[8]; v2=pxv[8];
	u3=pxu[9]; v3=pxv[9];

//	cu=(pxu[0]+pxu[1]+pxu[8]+pxu[9])>>2;
//	cv=(pxv[0]+pxv[1]+pxv[8]+pxv[9])>>2;

	mcy=pxy[0];	ncy=mcy;
	mcu=u0; mcv=v0; ncu=u0; ncv=v0;
	i=pxy[1];
	if(i<mcy) { mcy=i; mcu=u0; mcv=v0; }
	else if(i>ncy) { ncy=i; ncu=u0; ncv=v0; }
	i=pxy[2];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }
	i=pxy[3];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }

	i=pxy[8];
	if(i<mcy) { mcy=i; mcu=u0; mcv=v0; }
	else if(i>ncy) { ncy=i; ncu=u0; ncv=v0; }
	i=pxy[9];
	if(i<mcy) { mcy=i; mcu=u0; mcv=v0; }
	else if(i>ncy) { ncy=i; ncu=u0; ncv=v0; }
	i=pxy[10];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }
	i=pxy[11];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }

	i=pxy[16];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[17];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[18];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }
	i=pxy[19];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }

	i=pxy[24];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[25];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[26];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }
	i=pxy[27];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }
#endif

#if 0
	cu=(pxu[0]+pxu[1]+pxu[8]+pxu[9])>>2;
	cv=(pxv[0]+pxv[1]+pxv[8]+pxv[9])>>2;

	mcy=pxy[0];	ncy=mcy;
	i=pxy[ 1];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 2];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 3];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 8];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 9];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[10];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[11];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[16];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[17];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[18];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[19];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[24];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[25];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[26];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[27];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
#endif

	acy=(mcy+ncy)>>1;

#if 0
	u1=cu-128; v1=cv-128;
//	l0=91881*v1;
//	l1=-22554*u1-46802*v1;
//	l2=116130*u1;

	l0=359*v1;
	l1=-88*u1-183*v1;
	l2=454*u1;

	cra=(256*mcy+l0)>>8;	cga=(256*mcy+l1)>>8;	cba=(256*mcy+l2)>>8;
	crb=(256*ncy+l0)>>8;	cgb=(256*ncy+l1)>>8;	cbb=(256*ncy+l2)>>8;
#endif

#if 1
	u0=mcu-128; v0=mcv-128;
	u1=ncu-128; v1=ncv-128;

//	u1=cu-128; v1=cv-128;

//	l0=91881*v1;
//	l1=-22554*u1-46802*v1;
//	l2=116130*u1;

	l0=359*v0;
	l1=-88*u0-183*v0;
	l2=454*u0;

	l3=359*v1;
	l4=-88*u1-183*v1;
	l5=454*u1;

	cra=(256*mcy+l0)>>8;	cga=(256*mcy+l1)>>8;	cba=(256*mcy+l2)>>8;
	crb=(256*ncy+l3)>>8;	cgb=(256*ncy+l4)>>8;	cbb=(256*ncy+l5)>>8;
#endif

	if((cra|cga|cba|crb|cgb|cbb)&(~255))
	{
		cra=(cra<0)?0:((cra>255)?255:cra);
		cga=(cga<0)?0:((cga>255)?255:cga);
		cba=(cba<0)?0:((cba>255)?255:cba);
		crb=(crb<0)?0:((crb>255)?255:crb);
		cgb=(cgb<0)?0:((cgb>255)?255:cgb);
		cbb=(cbb<0)?0:((cbb>255)?255:cbb);
	}

	i=((cra<<8)&0xF800) | ((cga<<3)&0x07E0) | (cba>>3);
	j=((crb<<8)&0xF800) | ((cgb<<3)&0x07E0) | (cbb>>3);
	block[0]=i;		block[1]=i>>8;
	block[2]=j;		block[3]=j>>8;

	if(i==j)
	{
		block[4]=0;	block[5]=0;
		block[6]=0;	block[7]=0;
		return;
	}

#if 1
//	l0=(256+64)/(ncy-mcy+1);
//	l0=(256+64)/(ncy-acy+1);
//	l0=65536/(ncy-acy+1);
	l0=32768/(ncy-acy+1);

	p0=idxtab[(((pxy[24]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[25]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[26]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[27]-acy)*l0)>>13)+8];
	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[16]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[17]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[18]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[19]-acy)*l0)>>13)+8];
	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 8]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[ 9]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[10]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[11]-acy)*l0)>>13)+8];
	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 0]-acy)*l0)>>13)+8];
	p1=idxtab[(((pxy[ 1]-acy)*l0)>>13)+8];
	p2=idxtab[(((pxy[ 2]-acy)*l0)>>13)+8];
	p3=idxtab[(((pxy[ 3]-acy)*l0)>>13)+8];
	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
#endif

#if 0
	l0=256/(ncy-mcy+1);
	for(i=0; i<4; i++)
	{
		p0=idxtab[(((pxy[(3-i)*8+0]-acy)*l0)>>13)+8];
		p1=idxtab[(((pxy[(3-i)*8+1]-acy)*l0)>>13)+8];
		p2=idxtab[(((pxy[(3-i)*8+2]-acy)*l0)>>13)+8];
		p3=idxtab[(((pxy[(3-i)*8+3]-acy)*l0)>>13)+8];
		block[4+i]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
#endif

#if 0
	p0=-((acy-pxy[ 0])>>16);
	p1=-((acy-pxy[ 1])>>16);
	p2=-((acy-pxy[ 2])>>16);
	p3=-((acy-pxy[ 3])>>16);
	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[ 8])>>16);
	p1=-((acy-pxy[ 9])>>16);
	p2=-((acy-pxy[10])>>16);
	p3=-((acy-pxy[11])>>16);
	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[16])>>16);
	p1=-((acy-pxy[17])>>16);
	p2=-((acy-pxy[18])>>16);
	p3=-((acy-pxy[19])>>16);
	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[24])>>16);
	p1=-((acy-pxy[25])>>16);
	p2=-((acy-pxy[26])>>16);
	p3=-((acy-pxy[27])>>16);
	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
#endif
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1F_YUV(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(
			block+(i*xs1+j)*8,
			(short *)(rgba+(i*xs1+j)*16),
			(short *)(rgba+(i*xs1+j)*16),
			(short *)(rgba+(i*xs1+j)*16));
	}
}


void BGBBTJ_BCn_EncodeBlockDXT1A_YUV420(byte *block,
	short *pxy, short *pxu, short *pxv, int ab)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,2,2,2, 2,2,2,1, 1,1,1,1 };

	int p0, p1, p2, p3;
	int mcy, ncy, acy;
	int acr, acg, acb;
	int mcu, mcv;
	int ncu, ncv;
	int cy, cu, cv;
	int cra, cga, cba;
	int crb, cgb, cbb;
//	int u1, v1, l0, l1, l2;
	int l0, l1, l2, l3, l4, l5;
	int u0, v0, u1, v1, u2, v2, u3, v3;
	int i, j, k;

	if(ab==0xFFFF)
	{
		BGBBTJ_BCn_EncodeBlockDXT1F_YUV420(block, pxy, pxu, pxv);
		return;
	}
	
	if(ab==0x0000)
	{
		block[0]=0x00;	block[1]=0x00;
		block[2]=0x00;	block[3]=0x00;
		block[4]=0xFF;	block[5]=0xFF;
		block[6]=0xFF;	block[7]=0xFF;
		return;
	}

#if 1
	u0=pxu[0]; v0=pxv[0];
	u1=pxu[1]; v1=pxv[1];
	u2=pxu[8]; v2=pxv[8];
	u3=pxu[9]; v3=pxv[9];

//	cu=(pxu[0]+pxu[1]+pxu[8]+pxu[9])>>2;
//	cv=(pxv[0]+pxv[1]+pxv[8]+pxv[9])>>2;

	mcy=pxy[0];	ncy=mcy;
	mcu=u0; mcv=v0; ncu=u0; ncv=v0;
	i=pxy[1];
	if(i<mcy) { mcy=i; mcu=u0; mcv=v0; }
	else if(i>ncy) { ncy=i; ncu=u0; ncv=v0; }
	i=pxy[2];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }
	i=pxy[3];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }

	i=pxy[8];
	if(i<mcy) { mcy=i; mcu=u0; mcv=v0; }
	else if(i>ncy) { ncy=i; ncu=u0; ncv=v0; }
	i=pxy[9];
	if(i<mcy) { mcy=i; mcu=u0; mcv=v0; }
	else if(i>ncy) { ncy=i; ncu=u0; ncv=v0; }
	i=pxy[10];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }
	i=pxy[11];
	if(i<mcy) { mcy=i; mcu=u1; mcv=v1; }
	else if(i>ncy) { ncy=i; ncu=u1; ncv=v1; }

	i=pxy[16];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[17];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[18];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }
	i=pxy[19];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }

	i=pxy[24];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[25];
	if(i<mcy) { mcy=i; mcu=u2; mcv=v2; }
	else if(i>ncy) { ncy=i; ncu=u2; ncv=v2; }
	i=pxy[26];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }
	i=pxy[27];
	if(i<mcy) { mcy=i; mcu=u3; mcv=v3; }
	else if(i>ncy) { ncy=i; ncu=u3; ncv=v3; }
#endif

#if 0
	cu=(pxu[0]+pxu[1]+pxu[8]+pxu[9])>>2;
	cv=(pxv[0]+pxv[1]+pxv[8]+pxv[9])>>2;

	mcy=pxy[0];	ncy=mcy;
	i=pxy[ 1];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 2];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 3];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 8];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[ 9];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[10];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[11];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[16];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[17];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[18];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[19];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[24];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[25];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[26];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
	i=pxy[27];	j=(i-mcy)>>24; k=(ncy-i)>>24;
	mcy=(j&i)|((~j)&mcy); ncy=(k&i)|((~k)&ncy);
#endif

	acy=(mcy+ncy)>>1;

	u0=mcu-128; v0=mcv-128;
	u1=ncu-128; v1=ncv-128;

//	u1=cu-128; v1=cv-128;

//	l0=91881*v1;
//	l1=-22554*u1-46802*v1;
//	l2=116130*u1;

	l0=359*v0;
	l1=-88*u0-183*v0;
	l2=454*u0;

	l3=359*v1;
	l4=-88*u1-183*v1;
	l5=454*u1;

	cra=(256*mcy+l0)>>8;	cga=(256*mcy+l1)>>8;	cba=(256*mcy+l2)>>8;
	crb=(256*ncy+l3)>>8;	cgb=(256*ncy+l4)>>8;	cbb=(256*ncy+l5)>>8;
	if((cra|cga|cba|crb|cgb|cbb)&(~255))
	{
		cra=(cra<0)?0:((cra>255)?255:cra);
		cga=(cga<0)?0:((cga>255)?255:cga);
		cba=(cba<0)?0:((cba>255)?255:cba);
		crb=(crb<0)?0:((crb>255)?255:crb);
		cgb=(cgb<0)?0:((cgb>255)?255:cgb);
		cbb=(cbb<0)?0:((cbb>255)?255:cbb);
	}

	i=((cra<<8)&0xF800) | ((cga<<3)&0x07E0) | (cba>>3);
	j=((crb<<8)&0xF800) | ((cgb<<3)&0x07E0) | (cbb>>3);
	block[0]=i;		block[1]=i>>8;
	block[2]=j;		block[3]=j>>8;

	if(i==j)
	{
		block[4]=0;	block[5]=0;
		block[6]=0;	block[7]=0;
		return;
	}

#if 1
//	l0=(256+64)/(ncy-mcy+1);
//	l0=(256+64)/(ncy-acy+1);
//	l0=65536/(ncy-acy+1);
	l0=32768/(ncy-acy+1);

	p0=idxtab[(((pxy[24]-acy)*l0)>>13)+8]|(((ab>> 3)-1)&3);
	p1=idxtab[(((pxy[25]-acy)*l0)>>13)+8]|(((ab>> 2)-1)&3);
	p2=idxtab[(((pxy[26]-acy)*l0)>>13)+8]|(((ab>> 1)-1)&3);
	p3=idxtab[(((pxy[27]-acy)*l0)>>13)+8]|(((ab>> 0)-1)&3);
	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[16]-acy)*l0)>>13)+8]|(((ab>> 7)-1)&3);
	p1=idxtab[(((pxy[17]-acy)*l0)>>13)+8]|(((ab>> 6)-1)&3);
	p2=idxtab[(((pxy[18]-acy)*l0)>>13)+8]|(((ab>> 5)-1)&3);
	p3=idxtab[(((pxy[19]-acy)*l0)>>13)+8]|(((ab>> 4)-1)&3);
	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 8]-acy)*l0)>>13)+8]|(((ab>>11)-1)&3);
	p1=idxtab[(((pxy[ 9]-acy)*l0)>>13)+8]|(((ab>>10)-1)&3);
	p2=idxtab[(((pxy[10]-acy)*l0)>>13)+8]|(((ab>> 9)-1)&3);
	p3=idxtab[(((pxy[11]-acy)*l0)>>13)+8]|(((ab>> 8)-1)&3);
	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxy[ 0]-acy)*l0)>>13)+8]|(((ab>>15)-1)&3);
	p1=idxtab[(((pxy[ 1]-acy)*l0)>>13)+8]|(((ab>>14)-1)&3);
	p2=idxtab[(((pxy[ 2]-acy)*l0)>>13)+8]|(((ab>>13)-1)&3);
	p3=idxtab[(((pxy[ 3]-acy)*l0)>>13)+8]|(((ab>>12)-1)&3);
	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
#endif
}


void BGBBTJ_BCn_EncodeBlockDXT1F_BGRA(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	byte clr0[4], clr1[4];
	byte pxy[16];
	int p0, p1, p2, p3;
	int mcr, mcg, mcb, mcy;
	int ncr, ncg, ncb, ncy;
	int acr, acg, acb;
	int cr, cg, cb, cy, acy;
	int i, j, k;
	
	mcr=255; mcg=255; mcb=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+2];
		cg=rgba[k+1];
		cb=rgba[k+0];
		cy=(cr+2*cg+cb)>>2;
		pxy[i*4+j]=cy;

		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		acr+=cr; acg+=cg; acb+=cb;
	}
	
	acr=acr>>4;
	acg=acg>>4;
	acb=acb>>4;

	clr0[0]=(mcr*11+acr*5)>>4;
	clr0[1]=(mcg*11+acg*5)>>4;
	clr0[2]=(mcb*11+acb*5)>>4;
	clr1[0]=(ncr*11+acr*5)>>4;
	clr1[1]=(ncg*11+acg*5)>>4;
	clr1[2]=(ncb*11+acb*5)>>4;

	block[0]=((clr0[1]<<3)&0xE0) | (clr0[2]>>3);
	block[1]=(clr0[0]&0xF8) | (clr0[1]>>5);
	block[2]=((clr1[1]<<3)&0xE0) | (clr1[2]>>3);
	block[3]=(clr1[0]&0xF8) | (clr1[1]>>5);
	
	clr0[3]=(clr0[0]+2*clr0[1]+clr0[2])>>2;
	clr1[3]=(clr1[0]+2*clr1[1]+clr1[2])>>2;

	acy=(clr0[3]+clr1[3])>>1;

	p0=-((acy-pxy[ 0])>>16);
	p1=-((acy-pxy[ 1])>>16);
	p2=-((acy-pxy[ 2])>>16);
	p3=-((acy-pxy[ 3])>>16);
	block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[ 4])>>16);
	p1=-((acy-pxy[ 5])>>16);
	p2=-((acy-pxy[ 6])>>16);
	p3=-((acy-pxy[ 7])>>16);
	block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[ 8])>>16);
	p1=-((acy-pxy[ 9])>>16);
	p2=-((acy-pxy[10])>>16);
	p3=-((acy-pxy[11])>>16);
	block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=-((acy-pxy[12])>>16);
	p1=-((acy-pxy[13])>>16);
	p2=-((acy-pxy[14])>>16);
	p3=-((acy-pxy[15])>>16);
	block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1F_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1F_BGRA(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 3);
	}
}


#if 0
void BGBBTJ_BCn_EncodeBlockDXT1A_BGRA(byte *block,
	byte *rgba, int xstride, int ystride)
{
	static char idxtaba[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static char idxtabb[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static char idxtabc[16]=
		{ 0,0,0,0, 0,0,2,2, 2,2,1,1, 1,1,1,1 };
	static char idxtabd[16]=
		{ 1,1,1,1, 1,1,2,2, 2,2,0,0, 0,0,0,0 };
	byte pxa[16], pxy[16];
	char *idxtab;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;
	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;
	int mcr3r, mcg3r, mcb3r, ncr3r, ncg3r, ncb3r;
	int mcr3g, mcg3g, mcb3g, ncr3g, ncg3g, ncb3g;
	int mcr3b, mcg3b, mcb3b, ncr3b, ncg3b, ncb3b;
	int dr, dg, db, da, dy, alty;
	int drr, dgr, dbr, drg, dgg, dbg, drb, dgb, dbb;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;

#if 1
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	mcr2=255; mcg2=255; mcb2=255; mcy2=255;
	ncr2=  0; ncg2=  0; ncb2=  0; ncy2=  0;
	mcr3=255; mcg3=255; mcb3=255; mcy3=255;
	ncr3=  0; ncg3=  0; ncb3=  0; ncy3=  0;
	mcr3r=255; mcg3r=255; mcb3r=255;
	ncr3r=  0; ncg3r=  0; ncb3r=  0;
	mcr3g=255; mcg3g=255; mcb3g=255;
	ncr3g=  0; ncg3g=  0; ncb3g=  0;
	mcr3b=255; mcg3b=255; mcb3b=255;
	ncr3b=  0; ncg3b=  0; ncb3b=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+2];	cg=rgba[k+1];
		cb=rgba[k+0];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		if(xstride==3)
			{ ca=255; }

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
//		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
//		if((cy<mcy) && (ca>=64)) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if((cy>ncy) && (ca>=64)) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }

		if(cy<mcy)
		{
			if(ca>=64) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
			mcr2=cr; mcg2=cg; mcb2=cb; mcy2=cy;
		}
		if(cy>ncy)
		{
			if(ca>=64) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
			ncr2=cr; ncg2=cg; ncb2=cb; ncy2=cy;
		}
		
		if(cr<mcr3) { mcr3=cr; }
		if(cg<mcg3) { mcg3=cg; }
		if(cb<mcb3) { mcb3=cb; }
		if(cr>ncr3) { ncr3=cr; }
		if(cg>ncg3) { ncg3=cg; }
		if(cb>ncb3) { ncb3=cb; }

		if(cr<mcr3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; }
		if(cg<mcg3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; }
		if(cb<mcb3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; }
		if(cr>ncr3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; }
		if(cg>ncg3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; }
		if(cb>ncb3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; }

		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
#endif

#if 0	
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+2];	cg=rgba[k+1];
		cb=rgba[k+0];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		if(xstride==3)
			{ ca=255; }

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;

		//Find Mins/Maxs
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
#endif

	alty=0;

#if 0
	drr=ncr3r-mcr3r;	dgr=ncg3r-mcg3r;	dbr=ncb3r-mcb3r;
	drg=ncr3g-mcr3g;	dgg=ncg3g-mcg3g;	dbg=ncb3g-mcb3g;
	drb=ncr3b-mcr3b;	dgb=ncg3b-mcg3b;	dbb=ncb3b-mcb3b;
	dy=ncy-mcy;
//	if((dr>dy) || (dg>dy) || (db>dy))
	if((drr>dy) || (dgg>dy) || (dbb>dy))
	{
		if((drr>dgg) && (drr>dbb))
		{
			mcr=mcr3r; mcg=mcg3r; mcb=mcb3r;
			ncr=ncr3r; ncg=ncg3r; ncb=ncb3r;
		}else if(dgg>=dbb)
		{
			mcr=mcr3g; mcg=mcg3g; mcb=mcb3g;
			ncr=ncr3g; ncg=ncg3g; ncb=ncb3g;
		}else
		{
			mcr=mcr3b; mcg=mcg3b; mcb=mcb3b;
			ncr=ncr3b; ncg=ncg3b; ncb=ncb3b;
		}

		mcy=255; ncy=0;

		dr=ncr-mcr;
		dg=ncg-mcg;
		db=ncb-mcb;

		//normalize
		da=sqrt((dr*dr)+(dg*dg)+(db*db));
		dr=(256*dr)/da;
		dg=(256*dg)/da;
		db=(256*db)/da;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			cr=rgba[k+2];	cg=rgba[k+1];
			cb=rgba[k+0];
//			cy=(cr+(2*cg)+cb)>>2;		//Luma
			cy=(cr*dr+cg*dg+cb*db)>>8;

			pxy[i*4+j]=cy;

			if(cy<mcy) { mcy=cy; }
			if(cy>ncy) { ncy=cy; }
		}
		
		alty=1;
	}
#endif

#if 0
	dr=ncr3-mcr3;	dg=ncg3-mcg3;
	db=ncb3-mcb3;	dy=ncy-mcy;
	if((dr>dy) || (dg>dy) || (db>dy))
	{
		mcr=mcr3; mcg=mcg3; mcb=mcb3; mcy=mcy3;
		ncr=ncr3; ncg=ncg3; ncb=ncb3; ncy=ncy3;
		mcy=255; ncy=0;

		//normalize
		da=sqrt((dr*dr)+(dg*dg)+(db*db));
		dr=(256*dr)/da;
		dg=(256*dg)/da;
		db=(256*db)/da;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=(i*ystride)+(j*xstride);
			cr=rgba[k+2];	cg=rgba[k+1];
			cb=rgba[k+0];
//			cy=(cr+(2*cg)+cb)>>2;		//Luma
			cy=(cr*dr+cg*dg+cb*db)>>8;

			pxy[i*4+j]=cy;

			if(cy<mcy) { mcy=cy; }
			if(cy>ncy) { ncy=cy; }
		}
		
		alty=1;
	}
#endif

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	acy=(acr+(2*acg)+acb)>>2;
	
	if(!alty)
	{
		l=(256*mcy)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
		mcr=(3*mcr+cr)>>2;
		mcg=(3*mcg+cg)>>2;
		mcb=(3*mcb+cb)>>2;
	
		l=(256*ncy)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
		ncr=(3*ncr+cr)>>2;
		ncg=(3*ncg+cg)>>2;
		ncb=(3*ncb+cb)>>2;
	}
#endif

#if 1
	mcr=((mcr+3)>>3)<<3;
	mcg=((mcg+1)>>2)<<2;
	mcb=((mcb+3)>>3)<<3;
	ncr=((ncr+5)>>3)<<3;
	ncg=((ncg+3)>>2)<<2;
	ncb=((ncb+5)>>3)<<3;
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;

	if(nca<128)
//	if(0)
	{
		//Fully Transparent Block
		i=((acr<<8)&0xF800) | ((acg<<3)&0x07E0) | ((acb>>3)&0x001F);
		block[0]=i;	block[1]=i>>8;
		block[2]=i;	block[3]=i>>8;

		block[4]=255;	block[5]=255;
		block[6]=255;	block[7]=255;
	}else
		if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
//		if(0)
	{	
		//Flat Color Block
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		if(j<=i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
		}else
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
		}

		if(i!=j)
		{
			block[4]=(2<<6)|(3<<4)|(2<<2)|3;
			block[5]=(3<<6)|(2<<4)|(3<<2)|2;
			block[6]=(2<<6)|(3<<4)|(2<<2)|3;
			block[7]=(3<<6)|(2<<4)|(3<<2)|2;
		}else
		{
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
		}
	}else
		if(mca>=128)
//		if(0)
	{
		//Emit RGB Block (Opaque)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(j<i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
			idxtab=idxtabb;
		}else
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtabc;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}else
	{
		//Emit RGB Block (Alpha)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(i<=j)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtabc;
		}else
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
			idxtab=idxtabd;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13] | (((pxa[ 0]-128)>>10)&3);
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13] | (((pxa[ 1]-128)>>10)&3);
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13] | (((pxa[ 2]-128)>>10)&3);
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13] | (((pxa[ 3]-128)>>10)&3);
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13] | (((pxa[ 4]-128)>>10)&3);
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13] | (((pxa[ 5]-128)>>10)&3);
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13] | (((pxa[ 6]-128)>>10)&3);
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13] | (((pxa[ 7]-128)>>10)&3);
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13] | (((pxa[ 8]-128)>>10)&3);
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13] | (((pxa[ 9]-128)>>10)&3);
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13] | (((pxa[10]-128)>>10)&3);
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13] | (((pxa[11]-128)>>10)&3);
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13] | (((pxa[12]-128)>>10)&3);
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13] | (((pxa[13]-128)>>10)&3);
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13] | (((pxa[14]-128)>>10)&3);
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13] | (((pxa[15]-128)>>10)&3);
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}
#endif



#if 1
void BGBBTJ_BCn_EncodeBlockDXT1A_BGRA(byte *block,
	byte *rgba, int xstride, int ystride)
{
	static char idxtaba[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static char idxtabb[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static char idxtabc[16]=
		{ 0,0,0,0, 0,0,2,2, 2,2,1,1, 1,1,1,1 };
	static char idxtabd[16]=
		{ 1,1,1,1, 1,1,2,2, 2,2,0,0, 0,0,0,0 };
	byte pxa[16], pxyy[16], pxyr[16], pxyg[16], pxyb[16];
	byte *pxy;
	char *idxtab;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;
	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;
	int mcr3r, mcg3r, mcb3r, ncr3r, ncg3r, ncb3r, mcy3r, ncy3r;
	int mcr3g, mcg3g, mcb3g, ncr3g, ncg3g, ncb3g, mcy3g, ncy3g;
	int mcr3b, mcg3b, mcb3b, ncr3b, ncg3b, ncb3b, mcy3b, ncy3b;
	int dr, dg, db, da, dy, alty;
	int drr, dgr, dbr, drg, dgg, dbg, drb, dgb, dbb;
	int cr, cg, cb, ca, cy, cyr, cyg, cyb;
	int i, j, k, l;

#if 1
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	mcr2=255; mcg2=255; mcb2=255; mcy2=255;
	ncr2=  0; ncg2=  0; ncb2=  0; ncy2=  0;
	mcr3=255; mcg3=255; mcb3=255; mcy3=255;
	ncr3=  0; ncg3=  0; ncb3=  0; ncy3=  0;
	mcr3r=255; mcg3r=255; mcb3r=255; mcy3r=255;
	ncr3r=  0; ncg3r=  0; ncb3r=  0; ncy3r=  0;
	mcr3g=255; mcg3g=255; mcb3g=255; mcy3g=255;
	ncr3g=  0; ncg3g=  0; ncb3g=  0; ncy3g=  0;
	mcr3b=255; mcg3b=255; mcb3b=255; mcy3b=255;
	ncr3b=  0; ncg3b=  0; ncb3b=  0; ncy3b=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+2];	cg=rgba[k+1];
		cb=rgba[k+0];	ca=rgba[k+3];
//		cy=(cr+(2*cg)+cb)>>2;		//Luma

		cy =(5*cr+8*cg+3*cb)>>4;		//Luma (Default)
//		cyr=(8*cr+5*cg+3*cb)>>4;
		cyr=(8*cr+2*cg+6*cb)>>4;
		cyg=(8*cr+6*cg+2*cb)>>4;
		cyb=(2*cr+6*cg+8*cb)>>4;

		if(xstride==3)
			{ ca=255; }

		pxa[i*4+j]=ca;
//		pxy[i*4+j]=cy;

		pxyy[i*4+j]=cy;
		pxyr[i*4+j]=cyr;
		pxyg[i*4+j]=cyg;
		pxyb[i*4+j]=cyb;

		//Find Mins/Maxs
//		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
//		if((cy<mcy) && (ca>=64)) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
//		if((cy>ncy) && (ca>=64)) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }

		if(cy<mcy)
		{
			if(ca>=64) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
			mcr2=cr; mcg2=cg; mcb2=cb; mcy2=cy;
		}
		if(cy>ncy)
		{
			if(ca>=64) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
			ncr2=cr; ncg2=cg; ncb2=cb; ncy2=cy;
		}

		if(cyr<mcy3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; mcy3r=cyr; }
		if(cyr>ncy3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; ncy3r=cyr; }
		if(cyg<mcy3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; mcy3g=cyg; }
		if(cyg>ncy3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; ncy3g=cyg; }
		if(cyb<mcy3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; mcy3b=cyb; }
		if(cyb>ncy3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; ncy3b=cyb; }
		
//		if(cr<mcr3) { mcr3=cr; }
//		if(cg<mcg3) { mcg3=cg; }
//		if(cb<mcb3) { mcb3=cb; }
//		if(cr>ncr3) { ncr3=cr; }
//		if(cg>ncg3) { ncg3=cg; }
//		if(cb>ncb3) { ncb3=cb; }

//		if(cr<mcr3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; }
//		if(cg<mcg3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; }
//		if(cb<mcb3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; }
//		if(cr>ncr3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; }
//		if(cg>ncg3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; }
//		if(cb>ncb3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; }

		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
#endif

	alty=0;

	pxy=pxyy;
	
	dy=ncy-mcy;
	dr=ncy3r-mcy3r;
	dg=ncy3g-mcy3g;
	db=ncy3b-mcy3b;
	if(dr>dy)
	{
		mcr=mcr3r; mcg=mcg3r; mcb=mcb3r; mcy=mcy3r;
		ncr=ncr3r; ncg=ncg3r; ncb=ncb3r; ncy=ncy3r;
		pxy=pxyr; dy=ncy-mcy; alty=0;
	}
	if(dg>dy)
	{
		mcr=mcr3g; mcg=mcg3g; mcb=mcb3g; mcy=mcy3g;
		ncr=ncr3g; ncg=ncg3g; ncb=ncb3g; ncy=ncy3g;
		pxy=pxyg; dy=ncy-mcy; alty=0;
	}
	if(db>dy)
	{
		mcr=mcr3b; mcg=mcg3b; mcb=mcb3b; mcy=mcy3b;
		ncr=ncr3b; ncg=ncg3b; ncb=ncb3b; ncy=ncy3b;
		pxy=pxyb; dy=ncy-mcy; alty=0;
	}

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	
	if(!alty)
	{
		acy=(acr+(2*acg)+acb)>>2;
		mcy3=(mcr+(2*mcg)+mcb)>>2;
		ncy3=(ncr+(2*ncg)+ncb)>>2;

//		l=(256*mcy)/(acy+1);
		l=(256*mcy3)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
		mcr=(3*mcr+cr)>>2;
		mcg=(3*mcg+cg)>>2;
		mcb=(3*mcb+cb)>>2;
	
//		l=(256*ncy)/(acy+1);
		l=(256*ncy3)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
		ncr=(3*ncr+cr)>>2;
		ncg=(3*ncg+cg)>>2;
		ncb=(3*ncb+cb)>>2;
	}
#endif

#if 0
	mcr=((mcr+3)>>3)<<3;
	mcg=((mcg+1)>>2)<<2;
	mcb=((mcb+3)>>3)<<3;
	ncr=((ncr+5)>>3)<<3;
	ncg=((ncg+3)>>2)<<2;
	ncb=((ncb+5)>>3)<<3;
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

#if 1
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
#endif

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;

	if(nca<128)
//	if(0)
	{
		//Fully Transparent Block
		i=((acr<<8)&0xF800) | ((acg<<3)&0x07E0) | ((acb>>3)&0x001F);
		block[0]=i;	block[1]=i>>8;
		block[2]=i;	block[3]=i>>8;

		block[4]=255;	block[5]=255;
		block[6]=255;	block[7]=255;
	}else
		if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
//		if(0)
	{	
		//Flat Color Block
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		if(j<=i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
		}else
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
		}

		if(i!=j)
		{
			block[4]=(2<<6)|(3<<4)|(2<<2)|3;
			block[5]=(3<<6)|(2<<4)|(3<<2)|2;
			block[6]=(2<<6)|(3<<4)|(2<<2)|3;
			block[7]=(3<<6)|(2<<4)|(3<<2)|2;
		}else
		{
			block[4]=0;	block[5]=0;
			block[6]=0;	block[7]=0;
		}
	}else
		if(mca>=128)
//		if(0)
	{
		//Emit RGB Block (Opaque)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(j<i)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
			idxtab=idxtabb;
		}else
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtabc;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}else
	{
		//Emit RGB Block (Alpha)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(i<=j)
		{
			block[0]=i;	block[1]=i>>8;
			block[2]=j;	block[3]=j>>8;
			idxtab=idxtabc;
		}else
		{
			block[0]=j;	block[1]=j>>8;
			block[2]=i;	block[3]=i>>8;
			idxtab=idxtabd;
		}

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13] | (((pxa[ 0]-128)>>10)&3);
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13] | (((pxa[ 1]-128)>>10)&3);
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13] | (((pxa[ 2]-128)>>10)&3);
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13] | (((pxa[ 3]-128)>>10)&3);
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13] | (((pxa[ 4]-128)>>10)&3);
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13] | (((pxa[ 5]-128)>>10)&3);
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13] | (((pxa[ 6]-128)>>10)&3);
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13] | (((pxa[ 7]-128)>>10)&3);
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13] | (((pxa[ 8]-128)>>10)&3);
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13] | (((pxa[ 9]-128)>>10)&3);
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13] | (((pxa[10]-128)>>10)&3);
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13] | (((pxa[11]-128)>>10)&3);
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13] | (((pxa[12]-128)>>10)&3);
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13] | (((pxa[13]-128)>>10)&3);
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13] | (((pxa[14]-128)>>10)&3);
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13] | (((pxa[15]-128)>>10)&3);
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}
#endif


BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT1A_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT1A_BGRA(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}
