#include <bgbbtj.h>

#define BGBBTJ_FLOAT16_0P0000 0x0000

#define BGBBTJ_FLOAT16_1P0000 0x3C00
#define BGBBTJ_FLOAT16_0P5000 0x3800
#define BGBBTJ_FLOAT16_0P2500 0x3400
#define BGBBTJ_FLOAT16_0P1250 0x3000
#define BGBBTJ_FLOAT16_0P0625 0x2C00

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte
extern u16 bt1c_btohf[256];			//byte to half-float

#if 1
void BGBBTJ_BCn_EncodeBlockMB1C_F16(byte *block,
	u16 *rgba, int xstride, int ystride,
	u16 *rgbastrt, u16 *rgbaend, int fl)
{
//	static const char idxtab[32]=
//		{  0,  0,  0,  0,  0,  0,  0,  0,
//		   0,  1,  2,  3,  4,  5,  6,  7,
//		   8,  9, 10, 11, 12, 13, 14, 15,
//		  15, 15, 15, 15, 15, 15, 15, 15};
	static const char idxtab[32]=
		{  0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 2, 2, 2, 2, 2,
		   3, 3, 3, 3, 3, 1, 1, 1,
		   1, 1, 1, 1, 1, 1, 1, 1};
	u16 pxa[16], pxyy[16], pxyr[16], pxyg[16], pxyb[16];
	u16 *pxy;
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
	
	mcr=  65535; mcg=  65535; mcb=  65535; mca=  65535; mcy=65535;
	ncr=      0; ncg=      0; ncb=      0; nca=      0; ncy=    0;
	acr=      0; acg=      0; acb=      0; aca=      0; acy=    0;
	mcr2= 65535; mcg2= 65535; mcb2= 65535; mcy2= 65535;
	ncr2=     0; ncg2=     0; ncb2=     0; ncy2=     0;
	mcr3= 65535; mcg3= 65535; mcb3= 65535; mcy3= 65535;
	ncr3=     0; ncg3=     0; ncb3=     0; ncy3=     0;
	mcr3r=65535; mcg3r=65535; mcb3r=65535; mcy3r=65535;
	ncr3r=    0; ncg3r=    0; ncb3r=    0; ncy3r=    0;
	mcr3g=65535; mcg3g=65535; mcb3g=65535; mcy3g=65535;
	ncr3g=    0; ncg3g=    0; ncb3g=    0; ncy3g=    0;
	mcr3b=65535; mcg3b=65535; mcb3b=65535; mcy3b=65535;
	ncr3b=    0; ncg3b=    0; ncb3b=    0; ncy3b=    0;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		
		if(fl&1)
		{
			cr=rgba[k+2];	cg=rgba[k+1];
			cb=rgba[k+0];	ca=rgba[k+3];
		}else
		{
			cr=rgba[k+0];	cg=rgba[k+1];
			cb=rgba[k+2];	ca=rgba[k+3];
		}
		
//		cy=(cr+(2*cg)+cb)>>2;		//Luma

		cy =(5*cr+8*cg+3*cb)>>4;		//Luma (Default)
//		cyr=(8*cr+5*cg+3*cb)>>4;
		cyr=(8*cr+2*cg+6*cb)>>4;
		cyg=(8*cr+6*cg+2*cb)>>4;
		cyb=(2*cr+6*cg+8*cb)>>4;

		if(xstride==3)
			{ ca=65535; }

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
			if((ca&0x7FFF)>=BGBBTJ_FLOAT16_0P2500)
				{ mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
			mcr2=cr; mcg2=cg; mcb2=cb; mcy2=cy;
		}
		if(cy>ncy)
		{
			if((ca&0x7FFF)>=BGBBTJ_FLOAT16_0P2500)
				{ ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
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
	if((nca&0x7FFF)<BGBBTJ_FLOAT16_0P2500)
	{
		mcr=mcr2; mcg=mcg2; mcb=mcb2; mcy=mcy2;
		ncr=ncr2; ncg=ncg2; ncb=ncb2; ncy=ncy2;
	}
#endif

	alty=0;

	pxy=pxyy;

#if 1
	dy=ncy-mcy;
	dr=ncy3r-mcy3r;
	dg=ncy3g-mcy3g;
	db=ncy3b-mcy3b;
	if(dg>dy)
	{
		mcr=mcr3g; mcg=mcg3g; mcb=mcb3g; mcy=mcy3g;
		ncr=ncr3g; ncg=ncg3g; ncb=ncb3g; ncy=ncy3g;
		pxy=pxyg; dy=ncy-mcy; alty=0;
	}
	if(dr>dy)
	{
		mcr=mcr3r; mcg=mcg3r; mcb=mcb3r; mcy=mcy3r;
		ncr=ncr3r; ncg=ncg3r; ncb=ncb3r; ncy=ncy3r;
		pxy=pxyr; dy=ncy-mcy; alty=0;
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
	
	if(!alty)
	{
		acy=(acr+(2*acg)+acb)>>2;
		mcy3=(mcr+(2*mcg)+mcb)>>2;
		ncy3=(ncr+(2*ncg)+ncb)>>2;

		l=(256*mcy3)/(acy+1);
		cr=(acr*l+128)>>8;
		cg=(acg*l+128)>>8;
		cb=(acb*l+128)>>8;
	
		mcr=(3*mcr+cr)>>2;
		mcg=(3*mcg+cg)>>2;
		mcb=(3*mcb+cb)>>2;
	
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
	mcr=(mcr<0)?0:((mcr>65535)?65535:mcr);
	mcg=(mcg<0)?0:((mcg>65535)?65535:mcg);
	mcb=(mcb<0)?0:((mcb>65535)?65535:mcb);
	ncr=(ncr<0)?0:((ncr>65535)?65535:ncr);
	ncg=(ncg<0)?0:((ncg>65535)?65535:ncg);
	ncb=(ncb<0)?0:((ncb>65535)?65535:ncb);
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

	if((nca&0x7FFF)<BGBBTJ_FLOAT16_0P5000)
	{
		//Fully Transparent Block
		block[0]=0x55;	block[1]=0;
		block[2]=0xA9;	block[3]=0;

		block[4]=255;	block[5]=255;
		block[6]=255;	block[7]=255;
	
		block[ 8]=mcr2;	block[ 9]=mcr2>>8;
		block[10]=mcg2;	block[11]=mcg2>>8;
		block[12]=mcb2;	block[13]=mcb2>>8;
		block[14]=mca;	block[15]=mca>>8;

		block[16]=ncr2;	block[17]=ncr2>>8;
		block[18]=ncg2;	block[19]=ncg2>>8;
		block[20]=ncb2;	block[21]=ncb2>>8;
		block[22]=nca;	block[23]=nca>>8;
	}else
		if((mcr==ncr) && (mcg==ncg) && (mcb==ncb) && (mca==nca))
	{	
		i=((mcr<< 8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<< 8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);

		block[0]=0x55;	block[1]=0;
		block[2]=0xA9;	block[3]=0;

		block[ 8]=mcr;	block[ 9]=mcr>>8;
		block[10]=mcg;	block[11]=mcg>>8;
		block[12]=mcb;	block[13]=mcb>>8;
		block[14]=mca;	block[15]=mca>>8;

		block[16]=ncr;	block[17]=ncr>>8;
		block[18]=ncg;	block[19]=ncg>>8;
		block[20]=ncb;	block[21]=ncb>>8;
		block[22]=nca;	block[23]=nca>>8;

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

//		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l1=(32768)/(ncy-acy+1);				//Fix-Point Scale (Luma)
		l3a=65536-1024;
		l3b=65536+1024;

		block[0]=0x55;	block[1]=0;
		block[2]=0xA9;	block[3]=0;

		block[ 8]=mcr2;	block[ 9]=mcr2>>8;
		block[10]=mcg2;	block[11]=mcg2>>8;
		block[12]=mcb2;	block[13]=mcb2>>8;
		block[14]=mca;	block[15]=mca>>8;

		block[16]=ncr2;	block[17]=ncr2>>8;
		block[18]=ncg2;	block[19]=ncg2>>8;
		block[20]=ncb2;	block[21]=ncb2>>8;
		block[22]=nca;	block[23]=nca>>8;

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>12];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>12];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>12];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>12];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>12];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>12];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>12];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>12];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>12];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>12];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>12];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>12];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>12];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>12];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>12];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>12];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}
#endif


int BGBBTJ_BCn_IndexBC4Y_F16(int val, int min, int max)
{
	int y, r, v;

	if(val<=min)return(0);
	if(val>=max)return(1);
//	if(val==0)return(6);
//	if(val==255)return(7);
	
	y=val-min;
	r=max-min;
	
//	v=y*(256/(r+1));
//	v=(v+32)>>6;
	v=y*(32768/(r+1));
	v=(v+4096)>>13;
	if(v<0)v=0;
	if(v>3)v=3;
	return(v+2);
}

void BGBBTJ_BCn_FillBlockBC4_F16(byte *block, int val)
{
	block[0]=val; block[1]=val;
	block[2]=0;	block[3]=0;	block[4]=0;
	block[5]=0;	block[6]=0;	block[7]=0;
	return;
}

int BGBBTJ_BCn_CheckHfLdr(u16 val)
{
	if(!val)return(1);
	if(val>BGBBTJ_FLOAT16_1P0000)
		return(0);
	if(val<BGBBTJ_FLOAT16_0P0625)
		return(0);
	return(1);
}

void BGBBTJ_BCn_EncodeBlockBC4_F16(byte *block,
	u16 *rgba, int xstride, int ystride)
{
	static const char idxtab[32]=
		{  0, 0, 0, 0, 0, 0, 0, 0,
		   0, 0, 0, 2, 2, 2, 2, 2,
		   3, 3, 3, 3, 3, 1, 1, 1,
		   1, 1, 1, 1, 1, 1, 1, 1};
	u16 pxy[16];
	int l0, l1, l2, l3a, l3b;

	int min, max, avg, min2, max2;
	int cy;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i, j;

	min=65535; max=0; avg=0;
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		cy=rgba[(i*ystride)+(j*xstride)+0];
		pxy[i*4+j]=cy;
		if(cy<min)min=cy;
		if(cy>max)max=cy;
		avg+=cy;
	}
	avg=(avg+8)>>4;
	
	min2=bt1c_hf12tob[min>>4];
	max2=bt1c_hf12tob[max>>4];
	
	if(BGBBTJ_BCn_CheckHfLdr(min) && BGBBTJ_BCn_CheckHfLdr(max))
	{
		if(min==max)
		{
			block[0]=min2;
			block[1]=max2;
			block[2]=0;	block[3]=0;	block[4]=0;
			block[5]=0;	block[6]=0;	block[7]=0;
			return;
		}
	
		block[0]=min2;
		block[1]=max2;

		p0=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(0*xstride)+(0*ystride)], min, max);
		p1=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(1*xstride)+(0*ystride)], min, max);
		p2=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(2*xstride)+(0*ystride)], min, max);
		p3=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(3*xstride)+(0*ystride)], min, max);

		p4=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(0*xstride)+(1*ystride)], min, max);
		p5=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(1*xstride)+(1*ystride)], min, max);
		p6=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(2*xstride)+(1*ystride)], min, max);
		p7=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(3*xstride)+(1*ystride)], min, max);

		block[2]=p0|(p1<<3)|(p2<<6);
		block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
		block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	
		p0=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(0*xstride)+(2*ystride)], min, max);
		p1=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(1*xstride)+(2*ystride)], min, max);
		p2=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(2*xstride)+(2*ystride)], min, max);
		p3=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(3*xstride)+(2*ystride)], min, max);

		p4=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(0*xstride)+(3*ystride)], min, max);
		p5=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(1*xstride)+(3*ystride)], min, max);
		p6=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(2*xstride)+(3*ystride)], min, max);
		p7=BGBBTJ_BCn_IndexBC4Y_F16(rgba[(3*xstride)+(3*ystride)], min, max);
	
		block[5]=p0|(p1<<3)|(p2<<6);
		block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
		block[7]=(p5>>1)|(p6<<2)|(p7<<5);
		
		return;
	}else
	{
		l1=(32768)/(max-avg+1);
		l3a=65536-1024;
		l3b=65536+1024;

		block[0]=0x80|(min>>12);	block[1]=(min>>4);
		block[2]=0x70|(max>>12);	block[3]=(max>>4);

		p0=idxtab[((pxy[ 0]-avg)*l1+l3a)>>12];
		p1=idxtab[((pxy[ 1]-avg)*l1+l3b)>>12];
		p2=idxtab[((pxy[ 2]-avg)*l1+l3a)>>12];
		p3=idxtab[((pxy[ 3]-avg)*l1+l3b)>>12];
		block[4]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-avg)*l1+l3b)>>12];
		p1=idxtab[((pxy[ 5]-avg)*l1+l3a)>>12];
		p2=idxtab[((pxy[ 6]-avg)*l1+l3b)>>12];
		p3=idxtab[((pxy[ 7]-avg)*l1+l3a)>>12];
		block[5]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-avg)*l1+l3a)>>12];
		p1=idxtab[((pxy[ 9]-avg)*l1+l3b)>>12];
		p2=idxtab[((pxy[10]-avg)*l1+l3a)>>12];
		p3=idxtab[((pxy[11]-avg)*l1+l3b)>>12];
		block[6]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-avg)*l1+l3b)>>12];
		p1=idxtab[((pxy[13]-avg)*l1+l3a)>>12];
		p2=idxtab[((pxy[14]-avg)*l1+l3b)>>12];
		p3=idxtab[((pxy[15]-avg)*l1+l3a)>>12];
		block[7]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}


BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB1C_F16(byte *block,
	u16 *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
//	xs1=xs>>2; ys1=ys>>2;
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB1C_F16(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride, 0);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB1C_F16_BGRA(byte *block,
	u16 *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
//	xs1=xs>>2; ys1=ys>>2;
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockMB1C_F16(
			block+(i*xs1+j)*8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride, 1);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB5C_F16(byte *block,
	u16 *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4_F16(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockMB1C_F16(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride, 0);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageMB5C_F16_BGRA(byte *block,
	u16 *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4_F16(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockMB1C_F16(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride,
			rgba, rgba+xs*ys*stride, 1);
	}
}

void BGBBTJ_BCn_DecodeCopyBlockMB1C_F16(
	byte *block, byte *rgba, int xstride, int ystride, int flag)
{
	int i, j, k, l;

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=(i*4+j)*4;
		rgba[k+0]=bt1c_btohf[block[l+0]];
		rgba[k+1]=bt1c_btohf[block[l+1]];
		rgba[k+2]=bt1c_btohf[block[l+2]];
		rgba[k+3]=bt1c_btohf[block[l+3]];
	}
}

void BGBBTJ_BCn_DecodeBlockMB1C_55A9_F16(
	byte *block, byte *rgba, int xstride, int ystride, int flag)
{
	u16 tpixa[16*4];
	byte tpixb[16*4];
	u16 clr[4*4];
	int dr, dg, db;
	int i, j, k, l;
	
	i=block[0]|(block[1]<<8);
	j=block[2]|(block[3]<<8);

	clr[0]=block[ 8]|(block[ 9]<<8);
	clr[1]=block[10]|(block[11]<<8);
	clr[2]=block[12]|(block[13]<<8);
//	clr[3]=block[14]|(block[15]<<8);
	clr[4]=block[16]|(block[17]<<8);
	clr[5]=block[18]|(block[19]<<8);
	clr[6]=block[20]|(block[21]<<8);
//	clr[7]=block[22]|(block[23]<<8);

	clr[ 8]=((s16)clr[0]*11+(s16)clr[4]* 5)>>4;
	clr[ 9]=((s16)clr[1]*11+(s16)clr[5]* 5)>>4;
	clr[10]=((s16)clr[2]*11+(s16)clr[6]* 5)>>4;
//	clr[11]=((s16)clr[3]*11+(s16)clr[7]* 5)>>4;
	clr[12]=((s16)clr[0]* 5+(s16)clr[4]*11)>>4;
	clr[13]=((s16)clr[1]* 5+(s16)clr[5]*11)>>4;
	clr[14]=((s16)clr[2]* 5+(s16)clr[6]*11)>>4;
//	clr[15]=((s16)clr[3]* 5+(s16)clr[7]*11)>>4;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=((block[4+i]>>(2*j))&3)*4;
		rgba[k+0]=clr[l+0];
		rgba[k+1]=clr[l+1];
		rgba[k+2]=clr[l+2];
//		rgba[k+3]=clr[l+3];
	}
}

void BGBBTJ_BCn_DecodeBlockMB1C_F16(
	byte *block, byte *rgba, int xstride, int ystride, int flag)
{
	u16 tpixa[16*4];
	byte tpixb[16*4];
	u16 clr[4*4];
	int dr, dg, db;
	int i, j, k, l;
	
	if(block[0]==0x55)
	{
		if(block[2]==0xAB)
		{
			BGBBTJ_BCn_DecodeBlockMB1C_4ClrRGB24(
				block, tpixb, 4, 4*4, flag);
			BGBBTJ_BCn_DecodeCopyBlockMB1C_F16(
				tpixb, rgba, xstride, ystride, flag);
			return;
		}
	}

	if(block[0]==0x55)
	{
		if(block[2]==0xA9)
		{
			BGBBTJ_BCn_DecodeBlockMB1C_55A9_F16(
				block, rgba, xstride, ystride, flag);
		}
	}
	
	BGBBTJ_BCn_DecodeBlockMB1C(block, tpixb, 4, 4*4, flag);
	BGBBTJ_BCn_DecodeCopyBlockMB1C_F16(
		tpixb, rgba, xstride, ystride, flag);
	
}

void BGBBTJ_BCn_DecodeBlockBC4_F16(byte *block,
	byte *rgba, int xstride, int ystride)
{
	u16 val[8];
	u16 min, max;
	int i, j, k, l;

#if 0
	if(block[0]==block[1])
	{
		l=block[0];

#if 1
		for(i=0; i<4; i++)
		{
			k=i*ystride;
			rgba[k+0*xstride]=l;
			rgba[k+1*xstride]=l;
			rgba[k+2*xstride]=l;
			rgba[k+3*xstride]=l;
		}
#endif
		return;
	}
#endif
	
	if(((block[0]&0xF0)==0x80) && ((block[1]&0xF0)==0x70))
	{
		min=((block[0]&15)<<8)|(block[2]);
		max=((block[1]&15)<<8)|(block[3]);
		min=(min<<4)&65535;
		max=(max<<4)&65535;

		val[0]=min;
		val[1]=max;
		val[2]=(val[0]*11+val[1]*5)>>4;
		val[3]=(val[0]*5+val[1]*11)>>4;

		for(i=0; i<4; i++)
			for(j=0; j<4; j++)
		{
			k=i*ystride+j*xstride;
			l=(block[4+i]>>(2*j))&3;
			rgba[k]=val[l];
		}
		return;
	}
	
	if((block[8]==0x55) && (block[10]==0xA9))
	{
		min=block[22]|(block[23]<<8);
		max=block[30]|(block[31]<<8);
	}else
	{
//		min=BGBBTJ_ConvFloat8ToFloat16(block[0]);
//		max=BGBBTJ_ConvFloat8ToFloat16(block[1]);
		min=bt1c_btohf[block[0]];
		max=bt1c_btohf[block[1]];
	}
	
//	if(block[0]>block[1])
	if(0)
	{
		val[0]=min;
		val[1]=max;
		val[2]=(val[0]*219+val[1]* 37)>>8;
		val[3]=(val[0]*183+val[1]* 73)>>8;
		val[4]=(val[0]*146+val[1]*110)>>8;
		val[5]=(val[0]*110+val[1]*146)>>8;
		val[6]=(val[0]* 73+val[1]*183)>>8;
		val[7]=(val[0]* 37+val[1]*219)>>8;
	}else
	{
		val[0]=min;
		val[1]=max;
		val[2]=(val[0]*205+val[1]* 51)>>8;
		val[3]=(val[0]*154+val[1]*102)>>8;
		val[4]=(val[0]*102+val[1]*154)>>8;
		val[5]=(val[0]* 51+val[1]*205)>>8;
		val[6]=BGBBTJ_FLOAT16_0P0000;
		val[7]=BGBBTJ_FLOAT16_1P0000;
	}

	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*4+j)*3;
		l=((block[2+(k>>3)]|(block[3+(k>>3)]<<8))>>(k&7))&7;
		k=i*ystride+j*xstride;
		rgba[k]=val[l];
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB1C_F16(byte *block,
	byte *rgba, int xs, int ys, int stride, int flag)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_DecodeBlockBC4_F16(
//			block+(i*xs1+j)*32,
//			rgba+(i*4*xs+j*4)*stride+3,
//			stride, xs*stride);
		BGBBTJ_BCn_DecodeBlockMB1C_F16(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
	}
}

BGBBTJ_API void BGBBTJ_BCn_DecodeImageMB5C_F16(byte *block,
	byte *rgba, int xs, int ys, int stride, int flag)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_DecodeBlockBC4_F16(
			block+(i*xs1+j)*32,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_DecodeBlockMB1C_F16(
			block+(i*xs1+j)*32+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 0);
	}
}
