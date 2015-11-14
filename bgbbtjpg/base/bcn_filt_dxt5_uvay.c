#include <bgbbtj.h>

#if 0
void BGBBTJ_BCn_EncodeBlockDXT5_UVAY(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
	static const char idxtab2[16]=
		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	short pxy[16], pxu[16], pxv[16], pxa[16], pxz[16];
//	int peuva[3], pxuva[3], pwuva[3];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1;
	int mcy, mcu, mcv, mca, mcz;
	int ncy, ncu, ncv, nca, ncz;
	int acy, acu, acv, aca, acz;
	int cr, cg, cb, ca;
	int cy, cu, cv, cz, czb;
	int i, j, k, l;
	
	mcy=255; mcu=255; mcv=255; mca=255; mcz=255;
	ncy=  0; ncu=  0; ncv=  0; nca=  0; ncz=  0;
	acy=  0; acu=  0; acv=  0; aca=  0; acz=  0;
	czb=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];
//		cy=(cr+(2*cg)+cb)>>2;		//Y (Luma)
		cy=cg+(cb+cr-2*cg)>>2;		//Y (Luma)
		cu=((cb-cg)>>1)+128;
		cv=((cr-cg)>>1)+128;
//		ca=(ca>>1)+128;
		ca=(ca>>1);

#if 0
//		if(ca>=252)
		if(ca>=120)
		{
			l0=(cu>=128)?(cu-128):(128-cu);
			l1=(cv>=128)?(cv-128):(128-cv);
			l=(l0>=l1)?l0:l1;
//			l=128/(l+1);
//			l=(l>>2)<<2;
			l=64/(l+1);
			if(!l)l=1;
			
			cu=(cu-128)*l+128;
			cv=(cv-128)*l+128;
//			ca=128+(128/l);
//			ca=128-(128/l);
//			ca=(128/l);
			ca=256-(128/l);
		}
#endif

//		ca=255-ca;

//		cz=(cu+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)
//		cz=((3*cu)+(4*cv)+ca)>>3;	//Z (Chroma+Alpha)
		cz=((256-cu)+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)

		czb+=(cu-128)*(cv-128);

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);
//		ca=(ca<0)?0:((ca>255)?255:ca);
//		cz=(cz<0)?0:((cz>255)?255:cz);

#if 0
		if((cy+32)>=ca)
			{ cy=(224*ca)>>8; }
#endif

		pxy[i*4+j]=cy;
		pxz[i*4+j]=cz;

		pxu[i*4+j]=cu;
		pxv[i*4+j]=cv;
		pxa[i*4+j]=ca;

#if 0
		//Energy Balance (UVA)
		peuva[0]+=(cu-128)*(cu-128);
		peuva[1]+=(cv-128)*(cv-128);
		peuva[2]+=(ca-128)*(ca-128);
		//Correlation (UVA)
		pxuva[0]+=(cu-128)*(cv-128);
		pxuva[1]+=(cv-128)*(ca-128);
		pxuva[2]+=(cu-128)*(ca-128);
#endif

		//Find Mins/Maxs YUVA
		if(cz<mcz) { mcu=cu; mcv=cv; mca=ca; mcz=cz; }
		if(cz>ncz) { ncu=cu; ncv=cv; nca=ca; ncz=cz; }
		if(cy<mcy) { mcy=cy; }
		if(cy>ncy) { ncy=cy; }

#if 0
		if(cu<mcu) { mcu=cu; }
		if(cu>ncu) { ncu=cu; }
		if(cv<mcv) { mcv=cv; }
		if(cv>ncv) { ncv=cv; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }
		if(cz<mcz) { mcz=cz; }
		if(cz>ncz) { ncz=cz; }
#endif

		//For Average YUVA
		acy+=cy;	acu+=cu;
		acv+=cv;	aca+=ca;
	}
	
	//Calculate Average YUVA
	acy=acy>>4;	acu=acu>>4;
	acv=acv>>4;	aca=aca>>4;

//	if(pxuva[0]<0) { k=mcu; mcu=ncu; ncu=k; }
//	if(pxuva[1]<0) { k=mca; mca=nca; nca=k; }

#if 0
	k=peuva[0]+peuva[1]+peuva[2];
	l=(256*peuva[0])/k;
	pwuva[0]=(pxuva[0]>>28)*l;
	l=(256*peuva[1])/k;
	pwuva[1]=l;
	l=(256*peuva[2])/k;
	pwuva[2]=(pxuva[2]>>28)*l;

	if(pxuva[0]<0) { k=mcu; mcu=ncu; ncu=k; }
	if(pxuva[1]<0) { k=mca; mca=nca; nca=k; }

	mcz=255; ncz=0;
	for(i=0; i<16; i++)
	{
		cu=pxu[i]; cv=pxv[i]; ca=pxv[i];
		cz=(cu*pwuva[0]+cv*pwuva[1]+cv*pwuva[2])>>8;
		pxz[i]=cz;
		if(cz<mcz) { mcz=cz; }
		if(cz>ncz) { ncz=cz; }
	}

#endif

#if 0
	if(czb<0)
//	if(czb>0)
	{
		k=mcu; mcu=ncu; ncu=k;
		mcz=255; ncz=0;
		for(i=0; i<16; i++)
		{
			cu=256-pxu[i];
			cv=pxv[i];
			ca=pxv[i];
			cz=(l+(2*cv)+ca)>>2;	//Z (Chroma+Alpha)
//			cz=((4*l)+(3*cv)+ca)>>3;	//Z (Chroma+Alpha)
			pxz[i]=cz;
			if(cz<mcz) { mcz=cz; }
			if(cz>ncz) { ncz=cz; }
		}
	}
#endif

	mcu=((mcu+4)>>3)<<3;
	mcv=((mcv+2)>>2)<<2;
	ncu=((ncu+4)>>3)<<3;
	ncv=((ncv+2)>>2)<<2;

//	mcu=(((mcu-128)>>3)<<3)+128;
//	mcv=(((mcv-128)>>2)<<2)+128;
//	ncu=(((ncu-128)>>3)<<3)+128;
//	ncv=(((ncv-128)>>2)<<2)+128;

//	mcu=(11*mcu+5*acu)>>4;
//	mcv=(11*mcv+5*acv)>>4;
//	mca=(11*mca+5*aca)>>4;
//	ncu=(11*ncu+5*acu)>>4;
//	ncv=(11*ncv+5*acv)>>4;
//	nca=(11*nca+5*aca)>>4;

//	mcu=(mcu<128)?(((mcu+7)>>3)<<3):mcu;
//	mcv=(mcv<128)?(((mcv+3)>>2)<<2):mcv;

	mcy=(mcy<0)?0:((mcy>255)?255:mcy);
	mcu=(mcu<0)?0:((mcu>255)?255:mcu);
	mcv=(mcv<0)?0:((mcv>255)?255:mcv);
	mca=(mca<0)?0:((mca>255)?255:mca);
	mcz=(mcz<0)?0:((mcz>255)?255:mcz);
	ncy=(ncy<0)?0:((ncy>255)?255:ncy);
	ncu=(ncu<0)?0:((ncu>255)?255:ncu);
	ncv=(ncv<0)?0:((ncv>255)?255:ncv);
	nca=(nca<0)?0:((nca>255)?255:nca);
	ncz=(ncz<0)?0:((ncz>255)?255:ncz);

//	if(mcy<0)mcy=0;
//	if(mcu<0)mcu=0;
//	if(mcv<0)mcv=0;
//	if(mca<0)mca=0;
//	if(mcz<0)mcz=0;
//	if(ncy>255)ncy=255;
//	if(ncu>255)ncu=255;
//	if(ncv>255)ncv=255;
//	if(nca>255)nca=255;
//	if(ncz>255)ncz=255;

	acy=(mcy+ncy)>>1;			//Median Y
	acz=(mcz+ncz)>>1;			//Median Z
//	l0=(256+128)/(ncy-acy+1);	//Fix-Point Scale (Y)
//	l1=(256+64)/(ncz-acz+1);	//Fix-Point Scale (Z)
//	l0=(ncy>acy)?(32768/(ncy-acy)):0;		//Fix-Point Scale (Y)
//	l1=(ncz>acz)?(32768/(ncz-acz)):0;		//Fix-Point Scale (Z)
//	l0=32768/(ncy-acy+1);		//Fix-Point Scale (Y)
	l0=49152/(ncy-acy+1);		//Fix-Point Scale (Y)
	l1=32768/(ncz-acz+1);		//Fix-Point Scale (Z)

	//Emit Y Block
	block[0]=mcy;
	block[1]=ncy;
	
	p0=idxtab2[(((pxy[ 0]-acy)*l0)>>13)+8];
	p1=idxtab2[(((pxy[ 1]-acy)*l0)>>13)+8];
	p2=idxtab2[(((pxy[ 2]-acy)*l0)>>13)+8];
	p3=idxtab2[(((pxy[ 3]-acy)*l0)>>13)+8];
	p4=idxtab2[(((pxy[ 4]-acy)*l0)>>13)+8];
	p5=idxtab2[(((pxy[ 5]-acy)*l0)>>13)+8];
	p6=idxtab2[(((pxy[ 6]-acy)*l0)>>13)+8];
	p7=idxtab2[(((pxy[ 7]-acy)*l0)>>13)+8];
	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	p0=idxtab2[(((pxy[ 8]-acy)*l0)>>13)+8];
	p1=idxtab2[(((pxy[ 9]-acy)*l0)>>13)+8];
	p2=idxtab2[(((pxy[10]-acy)*l0)>>13)+8];
	p3=idxtab2[(((pxy[11]-acy)*l0)>>13)+8];
	p4=idxtab2[(((pxy[12]-acy)*l0)>>13)+8];
	p5=idxtab2[(((pxy[13]-acy)*l0)>>13)+8];
	p6=idxtab2[(((pxy[14]-acy)*l0)>>13)+8];
	p7=idxtab2[(((pxy[15]-acy)*l0)>>13)+8];
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);

//	if(czb<0) { k=mcu; mcu=ncu; ncu=k; }

	//Emit UVA Block
	i=((mcu<<8)&0xF800) | ((mcv<<3)&0x07E0) | ((mca>>3)&0x001F);
	j=((ncu<<8)&0xF800) | ((ncv<<3)&0x07E0) | ((nca>>3)&0x001F);
	block[ 8]=i;	block[ 9]=i>>8;
	block[10]=j;	block[11]=j>>8;

	p0=idxtab[(((pxz[ 0]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[ 1]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[ 2]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[ 3]-acz)*l1)>>13)+8];
	block[12]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 4]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[ 5]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[ 6]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[ 7]-acz)*l1)>>13)+8];
	block[13]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 8]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[ 9]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[10]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[11]-acz)*l1)>>13)+8];
	block[14]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[12]-acz)*l1)>>13)+8];
	p1=idxtab[(((pxz[13]-acz)*l1)>>13)+8];
	p2=idxtab[(((pxz[14]-acz)*l1)>>13)+8];
	p3=idxtab[(((pxz[15]-acz)*l1)>>13)+8];
	block[15]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}
#endif

#if 1
void BGBBTJ_BCn_EncodeBlockDXT5_UVAY(byte *block,
	byte *rgba, int xstride, int ystride, int nclr)
{
	static const char idxtab[16]=
		{ 0,0,0,0, 0,0,2,2, 3,3,1,1, 1,1,1,1 };
//	static const char idxtab2[16]=
//		{ 0,0,0,0, 2,2,3,3, 4,4,5,5, 1,1,1,1 };
	static const char idxtab2[16]=
		{ 1,1,7,7, 6,6,5,5, 4,4,3,3, 2,2,0,0 };
	short pxy[16], pxu[16], pxv[16], pxa[16], pxz[16];
//	int peuva[3], pxuva[3], pwuva[3];
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int l0, l1, l2a, l2b, l3a, l3b;
	int mcy, mcu, mcv, mca, mcz;
	int ncy, ncu, ncv, nca, ncz;
	int acy, acu, acv, aca, acz;
	int cr, cg, cb, ca;
	int cy, cu, cv, cz, czb;
	int i, j, k, l;
	
	mcy=255; mcu=255; mcv=255; mca=255; mcz=255;
	ncy=  0; ncu=  0; ncv=  0; nca=  0; ncz=  0;
	acy=  0; acu=  0; acv=  0; aca=  0; acz=  0;
	czb=  0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];

#if 0
//		cy=(cr+(2*cg)+cb)>>2;		//Y (Luma)
		cy=cg+((cb+cr-2*cg)>>2);	//Y (Luma)
		cu=((cb-cg)>>1)+128;
		cv=((cr-cg)>>1)+128;
//		ca=(ca>>1)+128;
		ca=(ca>>1);
#endif

#if 1
		cy=cg-((2*cg-cb-cr)>>2);
//		cy=cg+((cb+cr-2*cg)>>2);	//Y (Luma)
//		cu=((cb-cr)>>1)+128;
//		cv=((2*cg-cb-cr)>>2)+128;

		cu=(cb-cr)+128;
		cv=((2*cg-cb-cr)>>1)+128;
		ca=(ca>>1);
#endif

#if 1
//		if(ca>=252)
		if(ca>=120)
		{
			l0=(cu>=128)?(cu-128):(128-cu);
			l1=(cv>=128)?(cv-128):(128-cv);
			l=(l0>=l1)?l0:l1;
			
			if(l<64)
			{
//				l=128/(l+1);
//				l=(l>>2)<<2;
				l=64/(l+1);
				if(!l)l=1;

//				k=((128/l+2)&0xFC);
				k=((128/l+4)&0xF8);
//				k=(128/l);
				if(!k)k=1;
				l=128/k;
				if(!l)l=1;
			
				cu=(cu-128)*l+128;
				cv=(cv-128)*l+128;
//				ca=128+(128/l);
//				ca=128-(128/l);
//				ca=(128/l);
//				ca=256-(128/l);
				ca=256-(64/l);
			}else if(l<128)
			{
				ca=192;
			}else
			{
				cu=((cu-128)>>1)+128;
				cv=((cv-128)>>1)+128;
				ca=128;
			}
		}
#endif

		cz=(cu+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)
//		cz=((3*cu)+(4*cv)+ca)>>3;	//Z (Chroma+Alpha)
//		cz=((256-cu)+(2*cv)+ca)>>2;		//Z (Chroma+Alpha)

		czb+=(cu-128)*(cv-128);

		pxy[i*4+j]=cy;
		pxz[i*4+j]=cz;

		pxu[i*4+j]=cu;
		pxv[i*4+j]=cv;
		pxa[i*4+j]=ca;

		//Find Mins/Maxs YUVA
		if(cz<mcz) { mcu=cu; mcv=cv; mca=ca; mcz=cz; }
		if(cz>ncz) { ncu=cu; ncv=cv; nca=ca; ncz=cz; }
		if(cy<mcy) { mcy=cy; }
		if(cy>ncy) { ncy=cy; }

		//For Average YUVA
		acy+=cy;	acu+=cu;
		acv+=cv;	aca+=ca;
	}
	
	//Calculate Average YUVA
//	acy=acy>>4;	acu=acu>>4;
//	acv=acv>>4;	aca=aca>>4;

	acy=(acy+8)>>4;	acu=(acu+8)>>4;
	acv=(acv+8)>>4;	aca=(aca+8)>>4;

#if 1
	mcu=(mcu*13+acu*3)>>4;
	mcv=(mcv*13+acv*3)>>4;
	mca=(mca*13+aca*3)>>4;

	ncu=(ncu*13+acu*3)>>4;
	ncv=(ncv*13+acv*3)>>4;
	nca=(nca*13+aca*3)>>4;
#endif

#if 0
	acz=(acu+(2*acv)+aca)>>2;
	
	l=(256*mcz)/(acz+1);
	cu=(acu*l+128)>>8;
	cv=(acv*l+128)>>8;
	ca=(aca*l+128)>>8;
	
	mcu=(mcu*3+cu)>>2;
	mcv=(mcv*3+cv)>>2;
	mca=(mca*3+ca)>>2;

	l=(256*ncz)/(acz+1);
	cu=(acu*l+128)>>8;
	cv=(acv*l+128)>>8;
	ca=(aca*l+128)>>8;
	
	ncu=(ncu*3+cu)>>2;
	ncv=(ncv*3+cv)>>2;
	nca=(nca*3+ca)>>2;
#endif

	mcu=((mcu+4)>>3)<<3;
	mcv=((mcv+2)>>2)<<2;
	mca=((mca+4)>>3)<<3;
	ncu=((ncu+4)>>3)<<3;
	ncv=((ncv+2)>>2)<<2;
	nca=((nca+4)>>3)<<3;

//	mcu=((mcu+3)>>3)<<3;
//	mcv=((mcv+1)>>2)<<2;
//	ncu=((ncu+5)>>3)<<3;
//	ncv=((ncv+3)>>2)<<2;

	mcy=(mcy<0)?0:((mcy>255)?255:mcy);
	mcu=(mcu<0)?0:((mcu>255)?255:mcu);
	mcv=(mcv<0)?0:((mcv>255)?255:mcv);
	mca=(mca<0)?0:((mca>255)?255:mca);
	mcz=(mcz<0)?0:((mcz>255)?255:mcz);
	ncy=(ncy<0)?0:((ncy>255)?255:ncy);
	ncu=(ncu<0)?0:((ncu>255)?255:ncu);
	ncv=(ncv<0)?0:((ncv>255)?255:ncv);
	nca=(nca<0)?0:((nca>255)?255:nca);
	ncz=(ncz<0)?0:((ncz>255)?255:ncz);

	acy=(mcy+ncy)>>1;			//Median Y
	acz=(mcz+ncz)>>1;			//Median Z
//	l0=49152/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l1=32768/(ncz-acz+1);		//Fix-Point Scale (Z)

//	l0=(49152-8192)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-1024)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-12288)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-6144)/(ncy-acy+1);		//Fix-Point Scale (Y)
//	l0=(49152-6144)/(ncy-acy+1);		//Fix-Point Scale (Y)

	l0=65536/(ncy-acy+1);				//Fix-Point Scale (Y)

//	l1=(32768-6144)/(ncz-acz+1);		//Fix-Point Scale (Z)
//	l1=(32768-4096)/(ncz-acz+1);		//Fix-Point Scale (Z)
	l1=(32768-8192)/(ncz-acz+1);		//Fix-Point Scale (Z)

	l2a=65536-1024;
	l2b=65536+1024;

	l3a=65536-1024;
	l3b=65536+1024;

	//Emit Y Block
//	block[0]=mcy;
//	block[1]=ncy;
	block[0]=ncy;
	block[1]=mcy;
	
	p0=idxtab2[(((pxy[ 0]-acy)*l0+l2a)>>13)];
	p1=idxtab2[(((pxy[ 1]-acy)*l0+l2b)>>13)];
	p2=idxtab2[(((pxy[ 2]-acy)*l0+l2a)>>13)];
	p3=idxtab2[(((pxy[ 3]-acy)*l0+l2b)>>13)];
	p4=idxtab2[(((pxy[ 4]-acy)*l0+l2b)>>13)];
	p5=idxtab2[(((pxy[ 5]-acy)*l0+l2a)>>13)];
	p6=idxtab2[(((pxy[ 6]-acy)*l0+l2b)>>13)];
	p7=idxtab2[(((pxy[ 7]-acy)*l0+l2a)>>13)];
	block[2]=p0|(p1<<3)|(p2<<6);
	block[3]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[4]=(p5>>1)|(p6<<2)|(p7<<5);
	p0=idxtab2[(((pxy[ 8]-acy)*l0+l2a)>>13)];
	p1=idxtab2[(((pxy[ 9]-acy)*l0+l2b)>>13)];
	p2=idxtab2[(((pxy[10]-acy)*l0+l2a)>>13)];
	p3=idxtab2[(((pxy[11]-acy)*l0+l2b)>>13)];
	p4=idxtab2[(((pxy[12]-acy)*l0+l2b)>>13)];
	p5=idxtab2[(((pxy[13]-acy)*l0+l2a)>>13)];
	p6=idxtab2[(((pxy[14]-acy)*l0+l2b)>>13)];
	p7=idxtab2[(((pxy[15]-acy)*l0+l2a)>>13)];
	block[5]=p0|(p1<<3)|(p2<<6);
	block[6]=(p2>>2)|(p3<<1)|(p4<<4)|(p5<<7);
	block[7]=(p5>>1)|(p6<<2)|(p7<<5);

	//Emit UVA Block
	i=((mcu<<8)&0xF800) | ((mcv<<3)&0x07E0) | ((mca>>3)&0x001F);
	j=((ncu<<8)&0xF800) | ((ncv<<3)&0x07E0) | ((nca>>3)&0x001F);
	block[ 8]=i;	block[ 9]=i>>8;
	block[10]=j;	block[11]=j>>8;

	p0=idxtab[(((pxz[ 0]-acz)*l1+l3a)>>13)];
	p1=idxtab[(((pxz[ 1]-acz)*l1+l3b)>>13)];
	p2=idxtab[(((pxz[ 2]-acz)*l1+l3a)>>13)];
	p3=idxtab[(((pxz[ 3]-acz)*l1+l3b)>>13)];
	block[12]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 4]-acz)*l1+l3b)>>13)];
	p1=idxtab[(((pxz[ 5]-acz)*l1+l3a)>>13)];
	p2=idxtab[(((pxz[ 6]-acz)*l1+l3b)>>13)];
	p3=idxtab[(((pxz[ 7]-acz)*l1+l3a)>>13)];
	block[13]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[ 8]-acz)*l1+l3a)>>13)];
	p1=idxtab[(((pxz[ 9]-acz)*l1+l3b)>>13)];
	p2=idxtab[(((pxz[10]-acz)*l1+l3a)>>13)];
	p3=idxtab[(((pxz[11]-acz)*l1+l3b)>>13)];
	block[14]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
	p0=idxtab[(((pxz[12]-acz)*l1+l3b)>>13)];
	p1=idxtab[(((pxz[13]-acz)*l1+l3a)>>13)];
	p2=idxtab[(((pxz[14]-acz)*l1+l3b)>>13)];
	p3=idxtab[(((pxz[15]-acz)*l1+l3a)>>13)];
	block[15]=(p3<<6)|(p2<<4)|(p1<<2)|p0;
}
#endif

BGBBTJ_API void BGBBTJ_BCn_EncodeImageDXT5_UVAY(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BCn_EncodeBlockDXT5_UVAY(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride, 4);
	}
}
