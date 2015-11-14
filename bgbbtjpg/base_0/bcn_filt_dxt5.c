#include <bgbbtj.h>

#if 1
void BGBBTJ_BCn_EncodeBlockDXT5(byte *block,
	byte *rgba, int xstride, int ystride)
{
//	static const char idxtab[16]=
//		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static const char idxtab[16]=
		{ 1,1,1,1, 1,1,3,3, 2,2,0,0, 0,0,0,0 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	byte pxa[16], pxy[16];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2, l3a, l3b;
	int mcr, mcg, mcb, mca, mcy;
	int ncr, ncg, ncb, nca, ncy;
	int acr, acg, acb, aca, acy;
	int cr, cg, cb, ca, cy;
	int i, j, k, l;
	
	mcr=255; mcg=255; mcb=255; mca=255; mcy=255;
	ncr=  0; ncg=  0; ncb=  0; nca=  0; ncy=  0;
	acr=  0; acg=  0; acb=  0; aca=  0; acy=  0;
	
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
		if(cy<mcy) { mcr=cr; mcg=cg; mcb=cb; mcy=cy; }
		if(cy>ncy) { ncr=cr; ncg=cg; ncb=cb; ncy=cy; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		
		acr+=cr; acg+=cg;
		acb+=cb; acy+=cy;
	}

#if 1
	acr=(acr+8)>>4;
	acg=(acg+8)>>4;
	acb=(acb+8)>>4;
	acy=(acr+(2*acg)+acb)>>2;
	
	l=(256*mcy)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	
//	mcr=(11*mcr+5*cr+8)>>4;
//	mcg=(11*mcg+5*cg+8)>>4;
//	mcb=(11*mcb+5*cb+8)>>4;
//	mcr=(5*mcr+11*cr+8)>>4;
//	mcg=(5*mcg+11*cg+8)>>4;
//	mcb=(5*mcb+11*cb+8)>>4;
	mcr=(mcr+cr)>>1;
	mcg=(mcg+cg)>>1;
	mcb=(mcb+cb)>>1;
	

	l=(256*ncy)/(acy+1);
	cr=(acr*l+128)>>8;
	cg=(acg*l+128)>>8;
	cb=(acb*l+128)>>8;
	
//	ncr=(11*ncr+5*cr+8)>>4;
//	ncg=(11*ncg+5*cg+8)>>4;
//	ncb=(11*ncb+5*cb+8)>>4;
//	ncr=(5*ncr+11*cr+8)>>4;
//	ncg=(5*ncg+11*cg+8)>>4;
//	ncb=(5*ncb+11*cb+8)>>4;
	ncr=(ncr+cr)>>1;
	ncg=(ncg+cg)>>1;
	ncb=(ncb+cb)>>1;
#endif

	aca=(mca+nca)>>1;			//Median Alpha
	acy=(mcy+ncy)>>1;			//Median Luma
	l0=49152/(nca-aca+1);		//Fix-Point Scale (Alpha)
	l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
//	l2=4096;
//	l3=4096;
//	l2=-4096;
//	l3=-4096;
	l2=65536-2048;
//	l3a=65536-2048;
//	l3b=65536-2048;
//	l3b=65536;

	if(mca==nca)
	{
		block[0]=mca; block[1]=nca;
		block[2]=0;	block[3]=0;
		block[4]=0;	block[5]=0;
		block[6]=0;	block[7]=0;
	}else
	{
		//Emit Alpha Block
		block[0]=mca;
		block[1]=nca;
	
		p0=idxtab2[((pxa[ 0]-aca)*l0+l2)>>13];
		p1=idxtab2[((pxa[ 1]-aca)*l0+l2)>>13];
		p2=idxtab2[((pxa[ 2]-aca)*l0+l2)>>13];
		p3=idxtab2[((pxa[ 3]-aca)*l0+l2)>>13];
		p4=idxtab2[((pxa[ 4]-aca)*l0+l2)>>13];
		p5=idxtab2[((pxa[ 5]-aca)*l0+l2)>>13];
		p6=idxtab2[((pxa[ 6]-aca)*l0+l2)>>13];
		p7=idxtab2[((pxa[ 7]-aca)*l0+l2)>>13];
		block[2]=p0|(p1<<3)|(p2<<6);
		block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
		block[4]=(p5>>1)|(p6<<2)|(p7<<5);
		p0=idxtab2[((pxa[ 8]-aca)*l0+l2)>>13];
		p1=idxtab2[((pxa[ 9]-aca)*l0+l2)>>13];
		p2=idxtab2[((pxa[10]-aca)*l0+l2)>>13];
		p3=idxtab2[((pxa[11]-aca)*l0+l2)>>13];
		p4=idxtab2[((pxa[12]-aca)*l0+l2)>>13];
		p5=idxtab2[((pxa[13]-aca)*l0+l2)>>13];
		p6=idxtab2[((pxa[14]-aca)*l0+l2)>>13];
		p7=idxtab2[((pxa[15]-aca)*l0+l2)>>13];
		block[5]=p0|(p1<<3)|(p2<<6);
		block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
		block[7]=(p5>>1)|(p6<<2)|(p7<<5);
	}

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
//		block[ 8]=i;	block[ 9]=i>>8;
//		block[10]=j;	block[11]=j>>8;
		block[ 8]=j;	block[ 9]=j>>8;
		block[10]=i;	block[11]=i>>8;
		block[12]=(2<<6)|(3<<4)|(2<<2)|3;
		block[13]=(3<<6)|(2<<4)|(3<<2)|2;
		block[14]=(2<<6)|(3<<4)|(2<<2)|3;
		block[15]=(3<<6)|(2<<4)|(3<<2)|2;
	}else
	{
		//Emit RGB Block
#if 0
		mcr=((mcr+4)>>3)<<3;
		mcg=((mcg+2)>>2)<<2;
		mcb=((mcb+4)>>3)<<3;
		ncr=((ncr+4)>>3)<<3;
		ncg=((ncg+2)>>2)<<2;
		ncb=((ncb+4)>>3)<<3;
		mcr=(mcr<0)?0:((mcr>255)?255:mcr);
		mcg=(mcg<0)?0:((mcg>255)?255:mcg);
		mcb=(mcb<0)?0:((mcb>255)?255:mcb);
		ncr=(ncr<0)?0:((ncr>255)?255:ncr);
		ncg=(ncg<0)?0:((ncg>255)?255:ncg);
		ncb=(ncb<0)?0:((ncb>255)?255:ncb);
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

//		mcy=(mcr+(2*mcg)+mcb)>>2;
//		ncy=(ncr+(2*ncg)+ncb)>>2;
//		acy=(mcy+ncy)>>1;			//Median Luma

//		l1=32768/(ncy-acy+1);		//Fix-Point Scale (Luma)
//		l1=(32768-8192)/(ncy-acy+1);		//Fix-Point Scale (Luma)
		l1=(32768-6144)/(ncy-acy+1);		//Fix-Point Scale (Luma)
//		l1=(16384)/(ncy-acy+1);		//Fix-Point Scale (Luma)

//		l3a=65536-2048;
//		l3b=65536-2048;
//		l3b=65536;
//		l3a=65536;
//		l3b=65536+2048;
		l3a=65536-1024;
		l3b=65536+1024;

		i=((mcr<<8)&0xF800) | ((mcg<<3)&0x07E0) | ((mcb>>3)&0x001F);
		j=((ncr<<8)&0xF800) | ((ncg<<3)&0x07E0) | ((ncb>>3)&0x001F);
//		block[ 8]=i;	block[ 9]=i>>8;
//		block[10]=j;	block[11]=j>>8;
		block[ 8]=j;	block[ 9]=j>>8;
		block[10]=i;	block[11]=i>>8;

		p0=idxtab[((pxy[ 0]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 1]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[ 2]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[ 3]-acy)*l1+l3b)>>13];
		block[12]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 4]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[ 5]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[ 6]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[ 7]-acy)*l1+l3a)>>13];
		block[13]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[ 8]-acy)*l1+l3a)>>13];
		p1=idxtab[((pxy[ 9]-acy)*l1+l3b)>>13];
		p2=idxtab[((pxy[10]-acy)*l1+l3a)>>13];
		p3=idxtab[((pxy[11]-acy)*l1+l3b)>>13];
		block[14]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
		p0=idxtab[((pxy[12]-acy)*l1+l3b)>>13];
		p1=idxtab[((pxy[13]-acy)*l1+l3a)>>13];
		p2=idxtab[((pxy[14]-acy)*l1+l3b)>>13];
		p3=idxtab[((pxy[15]-acy)*l1+l3a)>>13];
		block[15]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	}
}
#endif

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT5(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5F(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		BGBBTJ_BCn_EncodeBlockBC4(
		BGBBTJ_BCn_EncodeBlockBC4L(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockDXT1F(
			block+(i*xs1+j)*16+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 4);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeIdxImageAlphaDXT5(byte *block,
	byte *rgba, int xs, int ys, int stride, int idx)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride+idx,
			stride, xs*stride);
//		BGBBTJ_BCn_EncodeBlockDXT1F(
//			block+(i*xs1+j)*16+8,
//			rgba+(i*4*xs+j*4)*stride,
//			stride, xs*stride, 4);
	}
}

BGBBTJ_API void BGBBTJ_BCn_FillImageAlphaDXT5(byte *block,
	int val, int xs, int ys)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_FillBlockBC4(
			block+(i*xs1+j)*16, val);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5B(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=(xs+3)>>2; ys1=(ys+3)>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockBC4(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride+3,
			stride, xs*stride);
		BGBBTJ_BCn_EncodeBlockDXT1B(
			block+(i*xs1+j)*16+8,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5B_Mip(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	byte *ct, *ct1;
	byte *cs, *cs1;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;

	xs1=xs; ys1=ys;

	xs2=xs1; ys2=ys1; cs=rgba; ct=block;
	while((xs2>0) || (ys2>0))
	{
		if(!xs2)xs2++;
		if(!ys2)ys2++;
		xs3=(xs2+3)/4;
		ys3=(ys2+3)/4;
//		n+=xs3*ys3;
		cs1=cs+(xs2*ys2)*4;
		ct1=ct+(xs3*ys3)*16;
//		BGBBTJ_Tex_HalfSampleDest(ct0, ct, xs2, ys2);
		BGBBTJ_BCn_EncodeImageDXT5B(ct, cs, xs2, ys2, stride);
		xs2>>=1; ys2>>=1;
		cs=cs1; ct=ct1;
	}
}



BGBBTJ_API void BGBBTJ_BCn_DeblockImageDXT5(byte *iblk,
	byte *oblk, int xs, int ys)
{
	byte *cs, *ct, *cs0, *cs1;
	int cr, cg, cb;
	int cr0, cg0, cb0, cr1, cg1, cb1;
	int cra0, cga0, cba0, cra1, cga1, cba1;
	int crb0, cgb0, cbb0, crb1, cgb1, cbb1;
	int cra, cga, cba;
	int crb, cgb, cbb;
	int xs1, ys1;
	int clra, clrb;
	int clra0, clrb0;
	int clra1, clrb1;
	int clra2, clrb2;
	int clra3, clrb3;
	int i, j, k, l, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		cs=iblk+(i*xs1+j)*16;
		ct=oblk+(i*xs1+j)*16;
		
		clra=cs[ 8]|(cs[ 9]<<8);
		clrb=cs[10]|(cs[11]<<8);
		
		if((clra!=clrb) ||
			((i<1) || ((i+1)>=ys1)) ||
			((j<1) || ((j+1)>=xs1)))
		{
			if(cs!=ct)
				{ bgbbtj_bt1c_memcpy16(ct, cs); }
			continue;
		}

		cs0=iblk+((i-1)*xs1+j)*16;
		cs1=iblk+((i+1)*xs1+j)*16;
		clra0=cs0[ 8]|(cs0[ 9]<<8);
		clrb0=cs0[10]|(cs0[11]<<8);
		clra1=cs1[ 8]|(cs1[ 9]<<8);
		clrb1=cs1[10]|(cs1[11]<<8);

//		if((clra0!=clrb0) || (clra1!=clrb1) ||
//			((clra==clra0) && (clra==clra1)))
//		{
//			if(cs!=ct)
//				{ bgbbtj_bt1c_memcpy16(ct, cs); }
//			continue;
//		}

		if(cs!=ct)
			{ bgbbtj_bt1c_memcpy8(ct, cs); }

		cr=(clra>>11)&31; cr=(cr<<3)|(cr>>2);
		cg=(clra>> 5)&63; cg=(cg<<2)|(cg>>4);
		cb=(clra    )&31; cb=(cb<<3)|(cb>>2);
//		cr0=(clra0>>11)&31; cr0=(cr0<<3)|(cr0>>2);
//		cg0=(clra0>> 5)&63; cg0=(cg0<<2)|(cg0>>4);
//		cb0=(clra0    )&31; cb0=(cb0<<3)|(cb0>>2);
//		cr1=(clra1>>11)&31; cr1=(cr1<<3)|(cr1>>2);
//		cg1=(clra1>> 5)&63; cg1=(cg1<<2)|(cg1>>4);
//		cb1=(clra1    )&31; cb1=(cb1<<3)|(cb1>>2);

		cra0=(clra0>>11)&31; cra0=(cra0<<3)|(cra0>>2);
		cga0=(clra0>> 5)&63; cga0=(cga0<<2)|(cga0>>4);
		cba0=(clra0    )&31; cba0=(cba0<<3)|(cba0>>2);
		cra1=(clra1>>11)&31; cra1=(cra1<<3)|(cra1>>2);
		cga1=(clra1>> 5)&63; cga1=(cga1<<2)|(cga1>>4);
		cba1=(clra1    )&31; cba1=(cba1<<3)|(cba1>>2);

		crb0=(clra0>>11)&31; crb0=(crb0<<3)|(crb0>>2);
		cgb0=(clra0>> 5)&63; cgb0=(cgb0<<2)|(cgb0>>4);
		cbb0=(clra0    )&31; cbb0=(cbb0<<3)|(cbb0>>2);
		crb1=(clra1>>11)&31; crb1=(crb1<<3)|(crb1>>2);
		cgb1=(clra1>> 5)&63; cgb1=(cgb1<<2)|(cgb1>>4);
		cbb1=(clra1    )&31; cbb1=(cbb1<<3)|(cbb1>>2);

		cr0=(cra0+crb0)>>1; cr1=(cra1+crb1)>>1;
		cg0=(cga0+cgb0)>>1; cg1=(cga1+cgb1)>>1;
		cb0=(cba0+cbb0)>>1; cb1=(cba1+cbb1)>>1;

//		cra=(11*cr+5*cr0)>>4;
//		cga=(11*cg+5*cg0)>>4;
//		cba=(11*cb+5*cb0)>>4;
//		crb=(11*cr+5*cr1)>>4;
//		cgb=(11*cg+5*cg1)>>4;
//		cbb=(11*cb+5*cb1)>>4;

//		cra=(5*cr+11*cr0)>>4;
//		cga=(5*cg+11*cg0)>>4;
//		cba=(5*cb+11*cb0)>>4;
//		crb=(5*cr+11*cr1)>>4;
//		cgb=(5*cg+11*cg1)>>4;
//		cbb=(5*cb+11*cb1)>>4;

		cra=(cr+cr0)>>1;
		cga=(cg+cg0)>>1;
		cba=(cb+cb0)>>1;
		crb=(cr+cr1)>>1;
		cgb=(cg+cg1)>>1;
		cbb=(cb+cb1)>>1;

		clra=((cra<<8)&0xF800) | ((cga<<3)&0x07E0) | ((cba>>3)&0x001F);
		clrb=((crb<<8)&0xF800) | ((cgb<<3)&0x07E0) | ((cbb>>3)&0x001F);
		ct[ 8]=clra;	ct[ 9]=clra>>8;
		ct[10]=clrb;	ct[11]=clrb>>8;

//		ct[12]=(2<<6)|(3<<4)|(2<<2)|3;
//		ct[13]=(3<<6)|(2<<4)|(3<<2)|2;
//		ct[14]=(2<<6)|(3<<4)|(2<<2)|3;
//		ct[15]=(3<<6)|(2<<4)|(3<<2)|2;

		ct[12]=(2<<6)|(0<<4)|(2<<2)|0;
		ct[13]=(3<<6)|(2<<4)|(3<<2)|2;
		ct[14]=(2<<6)|(3<<4)|(2<<2)|3;
		ct[15]=(3<<6)|(1<<4)|(3<<2)|1;

//		ct[ 8]=cs[ 8];			ct[ 9]=cs[ 9];
//		ct[10]=cs[10];			ct[11]=cs[11];
//		ct[12]=cs[15];			ct[13]=cs[14];
//		ct[14]=cs[13];			ct[15]=cs[12];
	}
}
