#include <bgbbtj.h>

#if 1
void BGBBTJ_BCn_EncodeBlockMB1C(byte *block,
	byte *rgba, int xstride, int ystride,
	byte *rgbastrt, byte *rgbaend)
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
	int cr, cg, cb, ca, cy;
	int dr, dg, db, da, dy, alty;
	int drr, dgr, dbr, drg, dgg, dbg, drb, dgb, dbb;
	int cyr, cyg, cyb;
	int i, j, k, l;
	
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
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
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

	pxy=pxyy;

#if 0
	dy=ncy-mcy;
	dr=ncy3r-mcy3r;
	dg=ncy3g-mcy3g;
	db=ncy3b-mcy3b;
//	if(dr>dy)
	if(dr>((17*dy)>>4))
	{
		mcr=mcr3r; mcg=mcg3r; mcb=mcb3r; mcy=mcy3r;
		ncr=ncr3r; ncg=ncg3r; ncb=ncb3r; ncy=ncy3r;
		pxy=pxyr; dy=ncy-mcy; alty=0;
	}
//	if(dg>dy)
	if(dg>((17*dy)>>4))
	{
		mcr=mcr3g; mcg=mcg3g; mcb=mcb3g; mcy=mcy3g;
		ncr=ncr3g; ncg=ncg3g; ncb=ncb3g; ncy=ncy3g;
		pxy=pxyg; dy=ncy-mcy; alty=0;
	}
//	if(db>dy)
	if(db>((17*dy)>>4))
	{
		mcr=mcr3b; mcg=mcg3b; mcb=mcb3b; mcy=mcy3b;
		ncr=ncr3b; ncg=ncg3b; ncb=ncb3b; ncy=ncy3b;
		pxy=pxyb; dy=ncy-mcy; alty=0;
	}
#endif

#if 1
	dy=ncy-mcy;
	dr=ncy3r-mcy3r;
	dg=ncy3g-mcy3g;
	db=ncy3b-mcy3b;
	if(dg>dy)
//	if(dg>((19*dy)>>4))
	{
		mcr=mcr3g; mcg=mcg3g; mcb=mcb3g; mcy=mcy3g;
		ncr=ncr3g; ncg=ncg3g; ncb=ncb3g; ncy=ncy3g;
		pxy=pxyg; dy=ncy-mcy; alty=0;
	}
	if(dr>dy)
//	if(dr>((19*dy)>>4))
//	if(0)
	{
		mcr=mcr3r; mcg=mcg3r; mcb=mcb3r; mcy=mcy3r;
		ncr=ncr3r; ncg=ncg3r; ncb=ncb3r; ncy=ncy3r;
		pxy=pxyr; dy=ncy-mcy; alty=0;
	}
	if(db>dy)
//	if(db>((19*dy)>>4))
//	if(0)
	{
		mcr=mcr3b; mcg=mcg3b; mcb=mcb3b; mcy=mcy3b;
		ncr=ncr3b; ncg=ncg3b; ncb=ncb3b; ncy=ncy3b;
		pxy=pxyb; dy=ncy-mcy; alty=0;
	}
#endif

#if 0
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

#if 1
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

#if 0
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
#endif

#if 1
	mcr2=mcr; mcg2=mcg; mcb2=mcb;
	ncr2=ncr; ncg2=ncg; ncb2=ncb;
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
	mcr3=mcr2-mcr; mcg3=mcg2-mcg; mcb3=mcb2-mcb;
	ncr3=ncr2-ncr; ncg3=ncg2-ncg; ncb3=ncb2-ncb;
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
		i=((mcr<< 8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<< 8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
//		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);

		if(j<=i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
		}else
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
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
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(j<i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
			idxtab=idxtabb;
		}else
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
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
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(i<=j)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtabc;
		}else
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
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
void BGBBTJ_BCn_EncodeBlockMB1C_BGRA(byte *block,
	byte *rgba, int xstride, int ystride,
	byte *rgbastrt, byte *rgbaend)
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

#if 1
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

#if 0
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
#endif

#if 1
	mcr2=mcr; mcg2=mcg; mcb2=mcb;
	ncr2=ncr; ncg2=ncg; ncb2=ncb;
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
	mcr3=mcr2-mcr; mcg3=mcg2-mcg; mcb3=mcb2-mcb;
	ncr3=ncr2-ncr; ncg3=ncg2-ncg; ncb3=ncb2-ncb;
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
		k=0;
		block[0]=i;	block[1]=i>>8;
		block[2]=i;	block[3]=i>>8;
		block[ 8]=k; block[ 9]=k>>8;
		block[10]=k; block[11]=k>>8;

		block[4]=255;	block[5]=255;
		block[6]=255;	block[7]=255;
		block[ 8]=mcr2;	block[ 9]=mcg2;
		block[10]=mcb2;	block[11]=mca;
		block[12]=ncr2;	block[13]=ncg2;
		block[14]=ncb2;	block[15]=nca;
	}else
		if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
//		if(0)
	{	
		//Flat Color Block
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
		if(j<=i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
		}else
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
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
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(j<i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
			idxtab=idxtabb;
		}else
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
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
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(i<=j)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtabc;
		}else
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
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


#if 0
void BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(byte *block,
	byte *rgba, int xstride, int ystride,
	byte *rgbastrt, byte *rgbaend)
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

	pxy=pxyy;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+2];	cg=rgba[k+1];
		cb=rgba[k+0];	ca=rgba[k+3];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

//		cy =(5*cr+8*cg+3*cb)>>4;		//Luma Green
//		cyr=(8*cr+2*cg+6*cb)>>4;		//Magenta
//		cyg=(8*cr+6*cg+2*cb)>>4;		//Yellow
//		cyb=(2*cr+6*cg+8*cb)>>4;		//Cyan

//		if(xstride==3)
//			{ ca=255; }

		k=i*4+j;
//		pxa[k]=ca;
		pxy[k]=cy;

//		k=i*4+j;
//		pxyy[k]=cy;		pxyr[k]=cyr;
//		pxyg[k]=cyg;	pxyb[k]=cyb;

		//Find Mins/Maxs
//		if(ca<mca) { mca=ca; }
//		if(ca>nca) { nca=ca; }

		if(cy<mcy)
		{
			mcr=cr; mcg=cg; mcb=cb; mcy=cy;
//			mcr2=cr; mcg2=cg; mcb2=cb; mcy2=cy;
		}
		if(cy>ncy)
		{
			ncr=cr; ncg=cg; ncb=cb; ncy=cy;
//			ncr2=cr; ncg2=cg; ncb2=cb; ncy2=cy;
		}

#if 0
		if(cyr<mcy3r) { mcr3r=cr; mcg3r=cg; mcb3r=cb; mcy3r=cyr; }
		if(cyr>ncy3r) { ncr3r=cr; ncg3r=cg; ncb3r=cb; ncy3r=cyr; }
		if(cyg<mcy3g) { mcr3g=cr; mcg3g=cg; mcb3g=cb; mcy3g=cyg; }
		if(cyg>ncy3g) { ncr3g=cr; ncg3g=cg; ncb3g=cb; ncy3g=cyg; }
		if(cyb<mcy3b) { mcr3b=cr; mcg3b=cg; mcb3b=cb; mcy3b=cyb; }
		if(cyb>ncy3b) { ncr3b=cr; ncg3b=cg; ncb3b=cb; ncy3b=cyb; }
#endif
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
#endif

	mca=255; nca=255;

	alty=0;

	pxy=pxyy;

#if 0
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
#endif

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	
//	if(!alty)
	if(1)
	{
		acy=(acr+(2*acg)+acb)>>2;
		mcy3=(mcr+(2*mcg)+mcb)>>2;
		ncy3=(ncr+(2*ncg)+ncb)>>2;

//		l=(256*mcy)/(acy+1);
		l=(256*mcy3)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
		mcr=(mcr+cr)>>1;
		mcg=(mcg+cg)>>1;
		mcb=(mcb+cb)>>1;
//		mcr=(3*mcr+cr)>>2;
//		mcg=(3*mcg+cg)>>2;
//		mcb=(3*mcb+cb)>>2;
	
//		l=(256*ncy)/(acy+1);
		l=(256*ncy3)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;

		ncr=(ncr+cr)>>1;
		ncg=(ncg+cg)>>1;
		ncb=(ncb+cb)>>1;
	
//		ncr=(3*ncr+cr)>>2;
//		ncg=(3*ncg+cg)>>2;
//		ncb=(3*ncb+cb)>>2;
	}
#endif

#if 1
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif

#if 0
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
#endif

#if 1
	mcr2=mcr; mcg2=mcg; mcb2=mcb;
	ncr2=ncr; ncg2=ncg; ncb2=ncb;
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
	mcr3=mcr2-mcr; mcg3=mcg2-mcg; mcb3=mcb2-mcb;
	ncr3=ncr2-ncr; ncg3=ncg2-ncg; ncb3=ncb2-ncb;
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
		k=0;
		block[0]=i;	block[1]=i>>8;
		block[2]=i;	block[3]=i>>8;
		block[ 8]=k; block[ 9]=k>>8;
		block[10]=k; block[11]=k>>8;

		block[4]=255;	block[5]=255;
		block[6]=255;	block[7]=255;
		block[ 8]=mcr2;	block[ 9]=mcg2;
		block[10]=mcb2;	block[11]=mca;
		block[12]=ncr2;	block[13]=ncg2;
		block[14]=ncb2;	block[15]=nca;
	}else
		if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
//		if(0)
	{	
		//Flat Color Block
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
		if(j<=i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
		}else
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
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
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(j<i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
			idxtab=idxtabb;
		}else
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
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
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
//		block[0]=i;	block[1]=i>>8;
//		block[2]=j;	block[3]=j>>8;

		if(i<=j)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
//			block[ 8]=k; block[ 9]=k>>8;
//			block[10]=l; block[11]=l>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtabc;
		}else
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
//			block[ 8]=l; block[ 9]=l>>8;
//			block[10]=k; block[11]=k>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
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


#if 0
// void BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(byte *block,
//	byte *rgba, int xstride, int ystride,
//	byte *rgbastrt, byte *rgbaend)
{
	static char idxtaba[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static char idxtabb[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static char idxtabc[16]=
		{ 0,0,0,0, 0,0,2,2, 2,2,1,1, 1,1,1,1 };
	static char idxtabd[16]=
		{ 1,1,1,1, 1,1,2,2, 2,2,0,0, 0,0,0,0 };
	byte pxy[16];
	char *idxtab;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int mcr2, mcg2, mcb2, mcy2;
	int ncr2, ncg2, ncb2, ncy2;
	int mcr3, mcg3, mcb3, mcy3;
	int ncr3, ncg3, ncb3, ncy3;
//	int mcr3r, mcg3r, mcb3r, ncr3r, ncg3r, ncb3r, mcy3r, ncy3r;
//	int mcr3g, mcg3g, mcb3g, ncr3g, ncg3g, ncb3g, mcy3g, ncy3g;
//	int mcr3b, mcg3b, mcb3b, ncr3b, ncg3b, ncb3b, mcy3b, ncy3b;
//	int dr, dg, db, da, dy, alty;
//	int drr, dgr, dbr, drg, dgg, dbg, drb, dgb, dbb;
	int cr, cg, cb, ca, cy, cyr, cyg, cyb;
	int cr0, cg0, cb0, cy0;
	int cr1, cg1, cb1, cy1;
	int cr2, cg2, cb2, cy2;
	int cr3, cg3, cb3, cy3;
	int i, j, k, l;

#if 0
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;

	pxy=pxyy;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+2];	cg=rgba[k+1];
		cb=rgba[k+0];
		cy=(cr+(2*cg)+cb)>>2;		//Luma

		k=i*4+j;
		pxy[k]=cy;

		if(cy<mcy)
			{ mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy)
			{ ncr=cr; ncg=cg; ncb=cb; ncy=cy; }

		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}
#endif

#if 1
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
	for(i=0; i<4; i++)
	{
//		j=(i*ystride);
//		l0=j+(0*xstride);
//		l1=j+(1*xstride);
//		l2=j+(2*xstride);
//		l3=j+(3*xstride);

		l0=i*ystride;
		l1=l0+xstride;
		l2=l1+xstride;
		l3=l2+xstride;

		cr0=rgba[l0+2];	cg0=rgba[l0+1]; cb0=rgba[l0+0];
		cr1=rgba[l1+2];	cg1=rgba[l1+1]; cb1=rgba[l1+0];
		cr2=rgba[l2+2];	cg2=rgba[l2+1]; cb2=rgba[l2+0];
		cr3=rgba[l3+2];	cg3=rgba[l3+1]; cb3=rgba[l3+0];

		cy0=(cr0+(2*cg0)+cb0)>>2;
		cy1=(cr1+(2*cg1)+cb1)>>2;
		cy2=(cr2+(2*cg2)+cb2)>>2;
		cy3=(cr3+(2*cg3)+cb3)>>2;

		k=i*4;
		pxy[k+0]=cy0;	pxy[k+1]=cy1;
		pxy[k+2]=cy2;	pxy[k+3]=cy3;

		if(cy0<mcy)	{ mcr=cr0; mcg=cg0; mcb=cb0; mcy=cy0; }
		if(cy1<mcy)	{ mcr=cr1; mcg=cg1; mcb=cb1; mcy=cy1; }
		if(cy2<mcy)	{ mcr=cr2; mcg=cg2; mcb=cb2; mcy=cy2; }
		if(cy3<mcy)	{ mcr=cr3; mcg=cg3; mcb=cb3; mcy=cy3; }
		if(cy0>ncy)	{ ncr=cr0; ncg=cg0; ncb=cb0; ncy=cy0; }
		if(cy1>ncy)	{ ncr=cr1; ncg=cg1; ncb=cb1; ncy=cy1; }
		if(cy2>ncy)	{ ncr=cr2; ncg=cg2; ncb=cb2; ncy=cy2; }
		if(cy3>ncy)	{ ncr=cr3; ncg=cg3; ncb=cb3; ncy=cy3; }

		acr+=cr0+cr1+cr2+cr3;
		acg+=cg0+cg1+cg2+cg3;
		acb+=cb0+cb1+cb2+cb3;
		acy+=cy0+cy1+cy2+cy3;
	}
#endif

	mca=255; nca=255;
//	alty=0;

#if 0
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	
	acy=(acr+(2*acg)+acb)>>2;
	mcy3=(mcr+(2*mcg)+mcb)>>2;
	ncy3=(ncr+(2*ncg)+ncb)>>2;

//	l=(256*mcy)/(acy+1);
	l=(256*mcy3)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	
	mcr=(mcr+cr)>>1;
	mcg=(mcg+cg)>>1;
	mcb=(mcb+cb)>>1;
//	mcr=(3*mcr+cr)>>2;
//	mcg=(3*mcg+cg)>>2;
//	mcb=(3*mcb+cb)>>2;
	
//	l=(256*ncy)/(acy+1);
	l=(256*ncy3)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;

	ncr=(ncr+cr)>>1;
	ncg=(ncg+cg)>>1;
	ncb=(ncb+cb)>>1;	
//	ncr=(3*ncr+cr)>>2;
//	ncg=(3*ncg+cg)>>2;
//	ncb=(3*ncb+cb)>>2;

#if 1
	mcr=(mcr<0)?0:((mcr>255)?255:mcr);
	mcg=(mcg<0)?0:((mcg>255)?255:mcg);
	mcb=(mcb<0)?0:((mcb>255)?255:mcb);
	ncr=(ncr<0)?0:((ncr>255)?255:ncr);
	ncg=(ncg<0)?0:((ncg>255)?255:ncg);
	ncb=(ncb<0)?0:((ncb>255)?255:ncb);
#endif
#endif

#if 1
	mcr2=mcr; mcg2=mcg; mcb2=mcb;
	ncr2=ncr; ncg2=ncg; ncb2=ncb;
	mcr=BGBBTJ_BCn_RoundColorNearest5Bits(mcr);
	mcg=BGBBTJ_BCn_RoundColorNearest6Bits(mcg);
	mcb=BGBBTJ_BCn_RoundColorNearest5Bits(mcb);
	ncr=BGBBTJ_BCn_RoundColorNearest5Bits(ncr);
	ncg=BGBBTJ_BCn_RoundColorNearest6Bits(ncg);
	ncb=BGBBTJ_BCn_RoundColorNearest5Bits(ncb);
	mcr3=mcr2-mcr; mcg3=mcg2-mcg; mcb3=mcb2-mcb;
	ncr3=ncr2-ncr; ncg3=ncg2-ncg; ncb3=ncb2-ncb;
#endif

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
	l2=65536-2048;

//	if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
	if((mcr2==ncr2) && (mcg2==ncg2) && (mcb2==ncb2))
	{	
		//Flat Color Block
		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);
		if(j<=i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
		}else
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
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
		//Emit RGB Block (Opaque)

		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
		k=((mcr3<<11)&0xF800) | ((mcg3<<5)&0x07E0) | ((mcb3)&0x001F);
		l=((ncr3<<11)&0xF800) | ((ncg3<<5)&0x07E0) | ((ncb3)&0x001F);

		if(j<i)
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
			idxtab=idxtaba;
		}else if(i<j)
		{
			block[ 0]=j; block[ 1]=j>>8;
			block[ 2]=i; block[ 3]=i>>8;
			block[ 8]=ncr2;	block[ 9]=ncg2;
			block[10]=ncb2;	block[11]=nca;
			block[12]=mcr2;	block[13]=mcg2;
			block[14]=mcb2;	block[15]=mca;
			idxtab=idxtabb;
		}else
		{
			block[ 0]=i; block[ 1]=i>>8;
			block[ 2]=j; block[ 3]=j>>8;
			block[ 8]=mcr2;	block[ 9]=mcg2;
			block[10]=mcb2;	block[11]=mca;
			block[12]=ncr2;	block[13]=ncg2;
			block[14]=ncb2;	block[15]=nca;
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

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB1C(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
//	xs1=xs>>2; ys1=ys>>2;
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB1C(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB1C_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
//	xs1=xs>>2; ys1=ys>>2;
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB1C_BGRA(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB1CF_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB1CFB_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB5C(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockMB1C(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB5C_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockMB1C_BGRA(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB5CF_BGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB1CFB_FlipBGRA(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	byte tblk[32];
	byte *cs, *ct, *rgbaend;
	int xs1, ys1;
	int i, j;
	
	rgbaend=rgba+xs*ys*stride;
	
//	xs1=xs>>2; ys1=ys>>2;
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB1CF_BGRA(
//		BGBBTJ_BCn_EncodeBlockMB1CF2_BGRA(
//			block+(i*xs1+j)*32+8,
			tblk+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgbaend);

		cs=tblk;
		ct=block+((ys1-i-1)*xs1+j)*32;

		ct[ 8]=cs[ 8];			ct[ 9]=cs[ 9];
		ct[10]=cs[10];			ct[11]=cs[11];
		ct[12]=cs[15];			ct[13]=cs[14];
		ct[14]=cs[13];			ct[15]=cs[12];

		if((cs[8]==0x55) && (cs[10]==0xAB))
		{
			bgbbtj_bt1c_memcpy8(ct+16, cs+24);
			bgbbtj_bt1c_memcpy8(ct+24, cs+16);
		}else
		{
			bgbbtj_bt1c_memcpy16(ct+16, cs+16);
		}
	}
}

BGBBTJ_API void BGBBTJ_BCn_FlipImageMB5C(byte *iblk,
	byte *oblk, int xs, int ys)
{
	byte *cs, *ct;
	int xs1, ys1;
	int i, j, k, l, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs=iblk+((ys1-i-1)*xs1+j)*32;
		ct=oblk+(i*xs1+j)*32;
		
		k=cs[2]|(cs[3]<<8)|(cs[4]<<16);
		l=cs[5]|(cs[6]<<8)|(cs[7]<<16);
		k=(k>>12)|((k&4095)<<12);
		l=(l>>12)|((l&4095)<<12);
		
		ct[ 0]=cs[ 0];			ct[ 1]=cs[ 1];
		ct[ 2]=l&255;			ct[ 3]=(l>>8)&255;
		ct[ 4]=(l>>16)&255;		ct[ 5]=k&255;
		ct[ 6]=(k>>8)&255;		ct[ 7]=(k>>16)&255;
		ct[ 8]=cs[ 8];			ct[ 9]=cs[ 9];
		ct[10]=cs[10];			ct[11]=cs[11];
		ct[12]=cs[15];			ct[13]=cs[14];
		ct[14]=cs[13];			ct[15]=cs[12];

		if((cs[8]==0x55) && (cs[10]==0xAB))
		{
			ct[16]=cs[24];			ct[17]=cs[25];
			ct[18]=cs[26];			ct[19]=cs[27];
			ct[20]=cs[28];			ct[21]=cs[29];
			ct[22]=cs[30];			ct[23]=cs[31];
			ct[24]=cs[16];			ct[25]=cs[17];
			ct[26]=cs[18];			ct[27]=cs[19];
			ct[28]=cs[20];			ct[29]=cs[21];
			ct[30]=cs[22];			ct[31]=cs[23];
		}else
		{
			ct[16]=cs[16];			ct[17]=cs[17];
			ct[18]=cs[18];			ct[19]=cs[19];
			ct[20]=cs[20];			ct[21]=cs[21];
			ct[22]=cs[22];			ct[23]=cs[23];
			ct[24]=cs[24];			ct[25]=cs[25];
			ct[26]=cs[26];			ct[27]=cs[27];
			ct[28]=cs[28];			ct[29]=cs[29];
			ct[30]=cs[30];			ct[31]=cs[31];
		}
	}
}

BGBBTJ_API void BGBBTJ_BCn_FlipImageMB5C_Mip(
	byte *iblk, byte *oblk, int xs, int ys)
{
	byte *ct, *ct1;
	byte *cs, *cs1;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	
	xs1=xs; ys1=ys;

	xs2=xs1; ys2=ys1; cs=iblk; ct=oblk;
	while((xs2>0) || (ys2>0))
	{
		if(!xs2)xs2++;
		if(!ys2)ys2++;
		xs3=(xs2+3)/4;
		ys3=(ys2+3)/4;
//		n+=xs3*ys3;
		cs1=cs+(xs3*ys3)*32;
		ct1=ct+(xs3*ys3)*32;
//		BGBBTJ_Tex_HalfSampleDest(ct0, ct, xs2, ys2);
		BGBBTJ_BCn_FlipImageDXT5(cs, ct, xs2, ys2);
		xs2>>=1; ys2>>=1;
		cs=cs1; ct=ct1;
	}
}
