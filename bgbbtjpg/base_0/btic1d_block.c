/*

* YYYYUV, Y=3 bits each, UV=2 bits each, total=16 bits.

* Block
** Mins YUV (32 bits, High 3 = ModeHigh)
** Maxs YUV (32 bits, High 3 = ModeLow)
*** Same basic format as mins.
*** N/A for Half-Width Blocks.
** 4x YYYYUV, Y=3 bits each, UV=2 bits each, total=16 bits.
** Half-Width: 4x YYYY, Y=2bpp

* Mode

** 0=Y Linear3 LDR + U,V Linear2
*** LDR, M3.Y11.U9.V9
**** Pixels = 4x YYYYUV, Y=3bpp, U,V=2bpp

** 1=Y Linear3 HDR + U,V Linear2
*** HDR, M3.E4.Y9.U8.V8
**** Pixels = 4x YYYYUV, Y=3bpp, U,V=2bpp

** 2=YUV Linear3 LDR + Direct Alpha
*** LDR+DA, M3.Y11.U9.V9
**** Pixels = 4x YYYYA, Y=3bpp, A=4bpp

** 3=YUV Linear3 HDR + Direct Alpha
*** HDR+DA, M3.E4.Y9.U8.V8
**** Pixels = 4x YYYYA, Y=3bpp, A=4bpp

** 4=YUV Linear2 LDR + Alpha Linear2
*** LDR+LA, M3.Y8.U7.V7.A7
*** Pixels = 4x YYYY, 4x AAAA, Y,A=2bpp

** 5=YUV Linear2 HDR + Alpha Linear2
*** HDR+LA, M3.E4.Y8.U6.V6.A5
*** Pixels = 4x YYYY, 4x AAAA, Y,A=2bpp

** 6=YUV Linear3 LDR + Bit Transparent
*** LDR+BA, M3.Y11.U9.V9
**** Pixels = 4x YYYYAAAA, Y=3bpp, A=1bpp

** 7=YUV Linear3 HDR + Bit Transparent
*** HDR+BA, M3.E4.Y9.U8.V8
**** Pixels = 4x YYYYAAAA, Y=3bpp, A=1bpp

** 56(H) = YUV Linear2 LDR (Half-Width)
*** LDR, M3.A.Ym6.Yn6.Um4.Vm4.Un4.Vn4
*** Pixels = 4x YYYY, Y=2bpp
*** Half-Width low-quality block.
*** A=0: 0=Min, 3=Max
*** A=1: 0=Min, 1=Avg, 2=Max, 3=Trans

Unpacked Blocks
* 0xC0 (48-55)
** Flat
* 0xE0 (56/63)
** Half-Width

* 0xE1
** Half-Width Flat
* 0xE2
** Full-Width Flat



* S=Sign (Inverts relative to Y)
** Inverts whole colorspace relative to Y.
** May encode out-of-gamut colors via a larger exponent.
*** Ex: R,G<0, B>0

* E=Gamma Exponent, Bias=7.

* YUV = Pixels under a color transform:
** Y=(R+2G+B)/4
*** Y=G-(2G-B-R)/4
** U=G-R-(B-R)/2
*** U=(2G-B-R)/2
** V=B-R
* Inverse:
** G=Y+U/2
** R=G-U-V/2
** B=G-U+V/2

* Y is linearly interpoloated, 0=Min, 7=Max
* U,V
** Linear: 0=Min, 3=Max
** Linear Y + Bit Transparent
*** Linear YUV, UV bits give which pixels are transparent.
** Linear Y + Direct Alpha
*** Linear YUV, UV bits give Alpha for 2x2 block.
*** Alpha maps from 0 to 1.


* Flat Color
** Color YUV (32 bits, M3.S.E4.Y8.U8.V8)
*** M=0, Simple Flat Color (LDR)
*** M=1, Simple Flat Color (HDR)
*** M=2, M3.A5.Y8.U8.V8 (Flat Alpha, LDR)
*** M=3, M3.E4.Y8.U6.V6.A5 (Flat Alpha, HDR)
**** Alpha maps linearly from 0 to 1.
*** M=4..7
**** 1.A.Y6.U4.V4
**** LDR Simple Flat Color (Half-Width)
**** A: 0=Opaque, 1=Transparent.


Possible: Use sub-block 2 directly in an RPZA-like format.

Opcode Symbol Ranges:
* 0x00-0x7F: Raw Block
** A Single Block is encoded here.
** This form can't encode Modes 4-7.
* 0x80-0x8F: Skip (1-16 Blocks)
** Blocks from this position are copied from the prior frame.
* 0x90-0x9F: Relative Block From Prior Frame (1-16 Blocks)
** Byte follows
*** 0x00-0x7F: RelY (3), RelX(4)
*** 0x80-0xBF: RelY (7), RelX(7)
*** 0xC0-0xFF: Reserved
* 0xA0-0xAF: Flat Color / Alpha (1-16 Blocks)
** FlatColor used for blocks.
** All blocks will have the same color.
* 0xB0-0xBF: Shared Color (1-16 Blocks)
** Shared Color used for pixel blocks.
** The block color will be given once, followed by pixel-blocks.
** Full width blocks will use 64 bits for the colors and pixel blocks.
** Half-width blocks will use 32 bits for each.
* 0xC0-0xCF: Short RLE (Length=1-16, Dist=1)
* 0xD0-0xDF: Short LZ (Length=1-16, Dist=VLI)
** Following Byte
*** 0x00-0x7F: 0-127 blocks.
*** 0x80-0xBF: 128-16383 blocks.
*** 0xC0-0xDF: 16384-2M blocks.
* 0xE0-0xFF: Command Opcodes
** 0xE1: Image Data, Len=Word24
** 0xE2: TLV A, Tag=Byte, Len=Word16
** 0xE3: TLV B, Len=Word24, Tag=TWOCC
** 0xE4: TLV C, Len=Word24, Tag=FOURCC
** 0xE5: TLV D, Len=Byte, Tag=TWOCC

Will use similar packaging to BTIC1C.

* 0xE1
** Image Blocks
* TWOCC('AX')
** Alpha Extension
* TWOCC('ZL')
** Deflate Compression with Zlib Header
* TWOCC('JP')
** JPEG-compressed image.

*/


#include <bgbbtj.h>

//int bgbbtj_bt1d_quant2(float f)
//{
//}

void BGBBTJ_BT1D_EncodeFlatBlockBX1A_S16(byte *block,
	int cr, int cg, int cb, int ca)
{
	BGBBTJ_BT1D_EncodeFlatBlockBX1A(block,
		cr/255.0, cg/255.0, cb/255.0, ca/255.0);
}

void BGBBTJ_BT1D_EncodeFlatHalfBlockBX1A_S16(byte *block,
	int cr, int cg, int cb, int ca)
{
	BGBBTJ_BT1D_EncodeFlatHalfBlockBX1A(block,
		cr/255.0, cg/255.0, cb/255.0, ca/255.0);
}

void BGBBTJ_BT1D_EncodeFlatBlockBX1A(byte *block,
	float cr, float cg, float cb, float ca)
{
	float cy, cu, cv;

	cy=(cr+(2*cg)+cb)/4;		//Luma
	cu=(2*cg-cr-cb)*0.5;
	cv=cb-cr;
	
	BGBBTJ_BT1D_EncodeFlatBlockBX1A_YUVA(block, cy, cu, cv, ca);
}

void BGBBTJ_BT1D_EncodeFlatHalfBlockBX1A(byte *block,
	float cr, float cg, float cb, float ca)
{
	float cy, cu, cv;

	cy=(cr+(2*cg)+cb)/4;		//Luma
	cu=(2*cg-cr-cb)*0.5;
	cv=cb-cr;
	
	BGBBTJ_BT1D_EncodeFlatHalfBlockBX1A_YUVA(block, cy, cu, cv, ca);
}

void BGBBTJ_BT1D_EncodeFlatHalfBlockBX1A_YUVA(byte *block,
	float cy, float cu, float cv, float ca)
{
	float f, g, h;
	int cyi, cui, cvi, cai;
	int exp;
	int p0;
	int i, j, k, l;

	memset(block, 0, 16);

//	cu=0; cv=0;

	cyi=(cy*63+0.5);
//	cui=(cu*7+0.5)+8;
//	cvi=(cv*7+0.5)+8;
//	cui=(cu*6.5+0.5)+8;
//	cvi=(cv*6.5+0.5)+8;
	cui=((int)(cu*7.5))+8;
	cvi=((int)(cv*7.5))+8;
	cyi=(cyi<0)?0:((cyi>63)?63:cyi);
	cui=(cui<0)?0:((cui>15)?15:cui);
	cvi=(cvi<0)?0:((cvi>15)?15:cvi);
	cai=ca<0.5;

	i=(6<<29);
	j=(1<<31)|(cai<<30)|(cyi<<24)|(cui<<20)|(cvi<<16);

	block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
	block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
	block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
	block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;
}

void BGBBTJ_BT1D_EncodeFlatBlockBX1A_YUVA(byte *block,
	float cy, float cu, float cv, float ca)
{
	float f, g, h;
	int cyi, cui, cvi, cai;
	int exp;
	int p0;
	int i, j, k, l;

	h=max(fabs(cy), max(fabs(cu), fabs(cv)));
	f=(h>0)?(log(h)/log(2)):0;
	exp=ceil(f);

	if(exp>8)exp=8;
	if(exp<-7)exp=-7;

	memset(block, 0, 16);

	if(exp<=0)
//	if(1)
	{
		//LDR branch
		//Opaque?
		if(ca>=0.9375)
		{
			cyi=(cy*2047+0.5);
			cui=(cu*255+0.5)+256;
			cvi=(cv*255+0.5)+256;
			cyi=(cyi<0)?0:((cyi>2047)?2047:cyi);
			cui=(cui<0)?0:((cui> 511)? 511:cui);
			cvi=(cvi<0)?0:((cvi> 511)? 511:cvi);

			i=(6<<29);
			j=(0<<29)|(cyi<<18)|(cui<<9)|cvi;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			return;
		}else
		{
			cyi=(cy*255+0.5);
			cui=(cu*127+0.5)+128;
			cvi=(cv*127+0.5)+128;
			cai=(ca*31+0.5);
			cyi=(cyi<0)?0:((cyi>255)?255:cyi);
			cui=(cui<0)?0:((cui>255)?255:cui);
			cvi=(cvi<0)?0:((cvi>255)?255:cvi);
			cai=(cai<0)?0:((cai>31)?31:cai);

			i=(6<<29);
			j=(2<<29)|(cai<<24)|(cyi<<16)|(cui<<8)|cvi;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			return;
		}
	}else
	{
		//HDR branch

		//Opaque?
		if(ca>=0.9375)
		{
			f=1.0/pow(2, exp);
			cyi=fabs(cy*255*f+0.5)+256;
			cui=fabs(cu*127*f+0.5)+128;
			cvi=fabs(cv*127*f+0.5)+128;
			cyi=(cyi<0)?0:((cyi>511)?511:cyi);
			cui=(cui<0)?0:((cui>255)?255:cui);
			cvi=(cvi<0)?0:((cvi>255)?255:cvi);

			p0=(exp+7);

			i=(6<<29);
			j=(1<<29)|(p0<<25)|(cyi<<16)|(cui<<8)|cvi;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			return;
		}else
		{
			f=1.0/pow(2, exp);
			cyi=fabs(cy*127*f+0.5)+128;
			cui=fabs(cu*31*f+0.5)+32;
			cvi=fabs(cv*31*f+0.5)+32;
			cai=(ca*31+0.5);
			cyi=(cyi<0)?0:((cyi>255)?255:cyi);
			cui=(cui<0)?0:((cui> 63)? 63:cui);
			cvi=(cvi<0)?0:((cvi> 63)? 63:cvi);
			cai=(cai<0)?0:((cai> 31)? 31:cai);

			p0=(exp+7);

			i=(6<<29);
			j=(3<<29)|(p0<<25)|(cyi<<17)|(cui<<11)|(cvi<<5)|cai;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			return;
		}
	}
}

#if 1
void BGBBTJ_BT1D_EncodeHalfBlockBX1A(byte *block,
	float *rgba, int xstride, int ystride)
{
	float pxy[16], pxu[16], pxv[16], pxa[16], pxa1[4];
	float mcy, mcu, mcv, mca;
	float ncy, ncu, ncv, nca;
	float acy, acu, acv, aca;
	float dcy, dcu, dcv, dca;
	float mcy2, mcu2, mcv2;
	float ncy2, ncu2, ncv2;
	float cr, cg, cb, ca;
	float cy, cu, cv;
	float f, g, h;
	int p0, p1, p2, p3, p4, p5;
	int mcyi, mcui, mcvi, mcai;
	int ncyi, ncui, ncvi, ncai;
	int mexp, nexp;
	int i, j, k, l;
	
	mcy=65536.0; mcu=65536.0; mcv=65536.0; mca=65536.0;
	ncy=      0; ncu=      0; ncv=      0; nca=      0;
	acy=      0; acu=      0; acv=      0; aca=      0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];

		cy=(cr+(2*cg)+cb)/4;		//Luma
//		cu=(cb-cg)*0.5;
//		cv=(cr-cg)*0.5;
		cu=(2*cg-cr-cb)*0.5;
		cv=cb-cr;

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;
//		pxu[i*4+j]=cu;
//		pxv[i*4+j]=cv;

		//Find Mins/Maxs
		if(cy<mcy) { mcy=cy; mcu=cu; mcv=cv; }
		if(cy>ncy) { ncy=cy; ncu=cu; ncv=cv; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }

		acy+=cy; acu+=cu;
		acv+=cv; aca+=ca;
	}

//	mcu=0; mcv=0;
//	ncu=0; ncv=0;

	acy/=16;	acu/=16;
	acv/=16;	aca/=16;
	
	dcy=ncy-mcy;
	dcu=ncu-mcu;
	dcv=ncv-mcv;
	dca=nca-mca;

#if 1
	if((dcy<0.015) && (dcu<0.06) && (dcv<0.06) && (dca<0.1))
	{
		BGBBTJ_BT1D_EncodeFlatHalfBlockBX1A_YUVA(block,
			acy, acu, acv, aca);
		return;
	}
#endif

	mcyi=(mcy*63+0.5);
//	mcui=(mcu*7+0.5)+8;
//	mcvi=(mcv*7+0.5)+8;
	ncyi=(ncy*63+0.5);
//	ncui=(ncu*7+0.5)+8;
//	ncvi=(ncv*7+0.5)+8;
	mcui=((int)(mcu*7.5))+8;
	mcvi=((int)(mcv*7.5))+8;
	ncui=((int)(ncu*7.5))+8;
	ncvi=((int)(ncv*7.5))+8;
	mcyi=(mcyi<0)?0:((mcyi>63)?63:mcyi);
	mcui=(mcui<0)?0:((mcui>15)?15:mcui);
	mcvi=(mcvi<0)?0:((mcvi>15)?15:mcvi);
	ncyi=(ncyi<0)?0:((ncyi>63)?63:ncyi);
	ncui=(ncui<0)?0:((ncui>15)?15:ncui);
	ncvi=(ncvi<0)?0:((ncvi>15)?15:ncvi);

	mcai=(mca<0.5);

	i=(7<<29);
	j=(7<<29)|(mcai<<28)|
		(mcyi<<22)|(ncyi<<16)|
		(mcui<<12)|(mcvi<< 8)|
		(ncui<< 4)|(ncvi<< 0);

	block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
	block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
	block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
	block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

	if(mcai)
		{ f=(ncy>mcy)?(2.0/(ncy-mcy)):0.0; }
	else
		{ f=(ncy>mcy)?(3.0/(ncy-mcy)):0.0; }

	p0=(pxy[0]-mcy)*f+0.5;		p1=(pxy[1]-mcy)*f+0.5;
	p2=(pxy[4]-mcy)*f+0.5;		p3=(pxy[5]-mcy)*f+0.5;
	p0=(p0<0)?0:((p0>3)?3:p0);	p1=(p1<0)?0:((p1>3)?3:p1);
	p2=(p2<0)?0:((p2>3)?3:p2);	p3=(p3<0)?0:((p3>3)?3:p3);
	if(mcai)
	{	if(pxa[0]<0.5)p0=3;		if(pxa[1]<0.5)p1=3;
		if(pxa[4]<0.5)p2=3;		if(pxa[5]<0.5)p3=3; }
	block[ 8]=(p0<<6)|(p1<<4)|(p2<<2)|(p3<<0);

	p0=(pxy[2]-mcy)*f+0.5;		p1=(pxy[3]-mcy)*f+0.5;
	p2=(pxy[6]-mcy)*f+0.5;		p3=(pxy[7]-mcy)*f+0.5;
	p0=(p0<0)?0:((p0>3)?3:p0);	p1=(p1<0)?0:((p1>3)?3:p1);
	p2=(p2<0)?0:((p2>3)?3:p2);	p3=(p3<0)?0:((p3>3)?3:p3);
	if(mcai)
	{	if(pxa[2]<0.5)p0=3;		if(pxa[3]<0.5)p1=3;
		if(pxa[6]<0.5)p2=3;		if(pxa[7]<0.5)p3=3; }
	block[ 9]=(p0<<6)|(p1<<4)|(p2<<2)|(p3<<0);
			
	p0=(pxy[ 8]-mcy)*f+0.5;		p1=(pxy[ 9]-mcy)*f+0.5;
	p2=(pxy[12]-mcy)*f+0.5;		p3=(pxy[13]-mcy)*f+0.5;
	p0=(p0<0)?0:((p0>3)?3:p0);	p1=(p1<0)?0:((p1>3)?3:p1);
	p2=(p2<0)?0:((p2>3)?3:p2);	p3=(p3<0)?0:((p3>3)?3:p3);
	if(mcai)
	{	if(pxa[ 8]<0.5)p0=3;	if(pxa[ 9]<0.5)p1=3;
		if(pxa[12]<0.5)p2=3;	if(pxa[13]<0.5)p3=3; }
	block[10]=(p0<<6)|(p1<<4)|(p2<<2)|(p3<<0);

	p0=(pxy[10]-mcy)*f+0.5;		p1=(pxy[11]-mcy)*f+0.5;
	p2=(pxy[14]-mcy)*f+0.5;		p3=(pxy[15]-mcy)*f+0.5;
	p0=(p0<0)?0:((p0>3)?3:p0);	p1=(p1<0)?0:((p1>3)?3:p1);
	p2=(p2<0)?0:((p2>3)?3:p2);	p3=(p3<0)?0:((p3>3)?3:p3);
	if(mcai)
	{	if(pxa[10]<0.5)p0=3;	if(pxa[11]<0.5)p1=3;
		if(pxa[14]<0.5)p2=3;	if(pxa[15]<0.5)p3=3; }
	block[11]=(p0<<6)|(p1<<4)|(p2<<2)|(p3<<0);
}
#endif

void BGBBTJ_BT1D_EncodeBlockBX1A(byte *block,
	float *rgba, int xstride, int ystride)
{
	float pxy[16], pxu[16], pxv[16], pxa[16], pxa1[4];
	float mcy, mcu, mcv, mca;
	float ncy, ncu, ncv, nca;
	float acy, acu, acv, aca;
	float dcy, dcu, dcv, dca;
	float mcy2, mcu2, mcv2;
	float ncy2, ncu2, ncv2;
	float cr, cg, cb, ca;
	float cy, cu, cv;
	float f, g, h;
	int p0, p1, p2, p3, p4, p5;
	int mcyi, mcui, mcvi, mcai;
	int ncyi, ncui, ncvi, ncai;
	int mexp, nexp;
	int i, j, k, l;
	
	mcy=65536.0; mcu=65536.0; mcv=65536.0; mca=65536.0;
	ncy=      0; ncu=      0; ncv=      0; nca=      0;
	acy=      0; acu=      0; acv=      0; aca=      0;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		cr=rgba[k+0];	cg=rgba[k+1];
		cb=rgba[k+2];	ca=rgba[k+3];

		cy=(cr+(2*cg)+cb)/4;		//Luma
//		cu=(cb-cg)*0.5;
//		cv=(cr-cg)*0.5;
		cu=(2*cg-cr-cb)*0.5;
		cv=cb-cr;

		pxa[i*4+j]=ca;
		pxy[i*4+j]=cy;
		pxu[i*4+j]=cu;
		pxv[i*4+j]=cv;

		//Find Mins/Maxs
		if(cy<mcy) { mcy=cy; mcu2=cu; mcv2=cv; }
		if(cy>ncy) { ncy=cy; ncu2=cu; ncv2=cv; }
		if(cu<mcu) { mcu=cu; }
		if(cu>ncu) { ncu=cu; }
		if(cv<mcv) { mcv=cv; }
		if(cv>ncv) { ncv=cv; }
		if(ca<mca) { mca=ca; }
		if(ca>nca) { nca=ca; }

		acy+=cy; acu+=cu;
		acv+=cv; aca+=ca;
	}
	
	acy/=16;	acu/=16;
	acv/=16;	aca/=16;
	
	dcy=ncy-mcy;
	dcu=ncu-mcu;
	dcv=ncv-mcv;
	dca=nca-mca;
	
//	h=max(ncy, max(ncu, ncv));
	h=max(fabs(mcy), max(fabs(mcu), fabs(mcv)));
	f=(h>0)?(log(h)/log(2)):0;
	mexp=ceil(f);

//	h=min(ncy, min(ncu, ncv));
	h=max(fabs(ncy), max(fabs(ncu), fabs(ncv)));
	f=(h>0)?(log(h)/log(2)):0;
	nexp=ceil(f);

	if(mexp>8)mexp=8;
	if(nexp>8)nexp=8;
	if(mexp<-7)mexp=-7;
	if(nexp<-7)nexp=-7;

#if 1
	f=1.0/pow(2.0, (mexp+nexp)/2);
//	if(((dcy*f)<0.001) && ((dcu*f)<0.002) && ((dcv*f)<0.002) &&
//		((dca*f)<0.01))
	if(((dcy*f)<0.004) && ((dcu*f)<0.008) && ((dcv*f)<0.008) &&
		((dca*f)<0.02))
	{
		BGBBTJ_BT1D_EncodeFlatBlockBX1A_YUVA(block, acy, acu, acv, aca);
		return;
	}
#endif

	f=(pxu[ 0]+pxu[ 1]+pxu[ 4]+pxu[ 5])*0.25;
	g=(pxv[ 0]+pxv[ 1]+pxv[ 4]+pxv[ 5])*0.25;
	h=(pxa[ 0]+pxa[ 1]+pxa[ 4]+pxa[ 5])*0.25;
	pxu[0]=f; pxv[0]=g; pxa1[0]=h;
	f=(pxu[ 2]+pxu[ 3]+pxu[ 6]+pxu[ 7])*0.25;
	g=(pxv[ 2]+pxv[ 3]+pxv[ 6]+pxv[ 7])*0.25;
	h=(pxa[ 2]+pxa[ 3]+pxa[ 6]+pxa[ 7])*0.25;
	pxu[1]=f; pxv[1]=g; pxa1[1]=h;
	f=(pxu[ 8]+pxu[ 9]+pxu[12]+pxu[13])*0.25;
	g=(pxv[ 8]+pxv[ 9]+pxv[12]+pxv[13])*0.25;
	h=(pxa[ 8]+pxa[ 9]+pxa[12]+pxa[13])*0.25;
	pxu[2]=f; pxv[2]=g; pxa1[2]=h;
	f=(pxu[10]+pxu[11]+pxu[14]+pxu[15])*0.25;
	g=(pxv[10]+pxv[11]+pxv[14]+pxv[15])*0.25;
	h=(pxa[10]+pxa[11]+pxa[14]+pxa[15])*0.25;
	pxu[3]=f; pxv[3]=g; pxa1[3]=h;

	//LDR?
//	if((mexp>=(-4)) && (mexp<=0) &&
//		(nexp>=(-4)) && (nexp<=0) &&
//		(mcy>=0) && (ncy>=0))
	if((mexp<=0) && (nexp<=0))
//	if(1)
	{
		//LDR branch
		//Opaque?
		if(mca>=0.9375)
		{
			mcyi=(mcy*2047+0.5);
			mcui=(mcu*255+0.5)+256;
			mcvi=(mcv*255+0.5)+256;
			ncyi=(ncy*2047+0.5);
			ncui=(ncu*255+0.5)+256;
			ncvi=(ncv*255+0.5)+256;
			mcyi=(mcyi<0)?0:((mcyi>2047)?2047:mcyi);
			mcui=(mcui<0)?0:((mcui> 511)? 511:mcui);
			mcvi=(mcvi<0)?0:((mcvi> 511)? 511:mcvi);
			ncyi=(ncyi<0)?0:((ncyi>2047)?2047:ncyi);
			ncui=(ncui<0)?0:((ncui> 511)? 511:ncui);
			ncvi=(ncvi<0)?0:((ncvi> 511)? 511:ncvi);

			i=(0<<29)|(mcyi<<18)|(mcui<<9)|mcvi;
			j=(0<<29)|(ncyi<<18)|(ncui<<9)|ncvi;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			f=(ncy>mcy)?(7.0/(ncy-mcy)):0.0;
			g=(ncu>mcu)?(3.0/(ncu-mcu)):0.0;
			h=(ncv>mcv)?(3.0/(ncv-mcv)):0.0;

			p0=(pxy[0]-mcy)*f+0.5;		p1=(pxy[1]-mcy)*f+0.5;
			p2=(pxy[4]-mcy)*f+0.5;		p3=(pxy[5]-mcy)*f+0.5;
			p4=(pxu[0]-mcu)*g+0.5;		p5=(pxv[0]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[8]=(i>>8)&255; block[9]=(i>>0)&255;

			p0=(pxy[2]-mcy)*f+0.5;		p1=(pxy[3]-mcy)*f+0.5;
			p2=(pxy[6]-mcy)*f+0.5;		p3=(pxy[7]-mcy)*f+0.5;
			p4=(pxu[1]-mcu)*g+0.5;		p5=(pxv[1]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[10]=(i>>8)&255; block[11]=(i>>0)&255;
			
			p0=(pxy[ 8]-mcy)*f+0.5;		p1=(pxy[ 9]-mcy)*f+0.5;
			p2=(pxy[12]-mcy)*f+0.5;		p3=(pxy[13]-mcy)*f+0.5;
			p4=(pxu[ 2]-mcu)*g+0.5;		p5=(pxv[ 2]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[12]=(i>>8)&255; block[13]=(i>>0)&255;

			p0=(pxy[10]-mcy)*f+0.5;		p1=(pxy[11]-mcy)*f+0.5;
			p2=(pxy[14]-mcy)*f+0.5;		p3=(pxy[15]-mcy)*f+0.5;
			p4=(pxu[ 3]-mcu)*g+0.5;		p5=(pxv[ 3]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[14]=(i>>8)&255; block[15]=(i>>0)&255;

			return;
		}else
		{
			mcyi=(mcy*2047+0.5);
			mcui=(mcu2*255+0.5)+256;
			mcvi=(mcv2*255+0.5)+256;
			ncyi=(ncy*2047+0.5);
			ncui=(ncu2*255+0.5)+256;
			ncvi=(ncv2*255+0.5)+256;
			mcyi=(mcyi<0)?0:((mcyi>2047)?2047:mcyi);
			mcui=(mcui<0)?0:((mcui> 511)? 511:mcui);
			mcvi=(mcvi<0)?0:((mcvi> 511)? 511:mcvi);
			ncyi=(ncyi<0)?0:((ncyi>2047)?2047:ncyi);
			ncui=(ncui<0)?0:((ncui> 511)? 511:ncui);
			ncvi=(ncvi<0)?0:((ncvi> 511)? 511:ncvi);

			i=(0<<29)|(mcyi<<18)|(mcui<<9)|mcvi;
			j=(2<<29)|(ncyi<<18)|(ncui<<9)|ncvi;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			f=(ncy>mcy)?(7.0/(ncy-mcy)):0.0;

			p0=(pxy[0]-mcy)*f+0.5;		p1=(pxy[1]-mcy)*f+0.5;
			p2=(pxy[4]-mcy)*f+0.5;		p3=(pxy[5]-mcy)*f+0.5;
			p5=pxa1[0]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[8]=(i>>8)&255; block[9]=(i>>0)&255;

			p0=(pxy[2]-mcy)*f+0.5;		p1=(pxy[3]-mcy)*f+0.5;
			p2=(pxy[6]-mcy)*f+0.5;		p3=(pxy[7]-mcy)*f+0.5;
			p5=pxa1[1]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[10]=(i>>8)&255; block[11]=(i>>0)&255;
			
			p0=(pxy[ 8]-mcy)*f+0.5;		p1=(pxy[ 9]-mcy)*f+0.5;
			p2=(pxy[12]-mcy)*f+0.5;		p3=(pxy[13]-mcy)*f+0.5;
			p5=pxa1[ 2]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[12]=(i>>8)&255; block[13]=(i>>0)&255;

			p0=(pxy[10]-mcy)*f+0.5;		p1=(pxy[11]-mcy)*f+0.5;
			p2=(pxy[14]-mcy)*f+0.5;		p3=(pxy[15]-mcy)*f+0.5;
			p5=pxa1[ 3]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[14]=(i>>8)&255; block[15]=(i>>0)&255;

			return;
		}
	}else
	{
		//HDR branch

		//Opaque?
		if(mca>=0.9375)
		{
			f=1.0/pow(2, mexp);
			g=1.0/pow(2, nexp);
			mcyi=fabs(mcy*255*f+0.5)+256;
			mcui=fabs(mcu2*127*f+0.5)+128;
			mcvi=fabs(mcv2*127*f+0.5)+128;
			ncyi=fabs(ncy*255*g+0.5)+256;
			ncui=fabs(ncu2*127*g+0.5)+128;
			ncvi=fabs(ncv2*127*g+0.5)+128;
			mcyi=(mcyi<0)?0:((mcyi>511)?511:mcyi);
			mcui=(mcui<0)?0:((mcui>255)?255:mcui);
			mcvi=(mcvi<0)?0:((mcvi>255)?255:mcvi);
			ncyi=(ncyi<0)?0:((ncyi>511)?511:ncyi);
			ncui=(ncui<0)?0:((ncui>255)?255:ncui);
			ncvi=(ncvi<0)?0:((ncvi>255)?255:ncvi);

			p0=(mexp+7);
			p1=(nexp+7);

			i=(0<<29)|(p0<<25)|(mcyi<<16)|(mcui<<8)|mcvi;
			j=(1<<29)|(p1<<25)|(ncyi<<16)|(ncui<<8)|ncvi;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			f=(ncy>mcy)?(7.0/(ncy-mcy)):0.0;
			g=(ncu>mcu)?(3.0/(ncu-mcu)):0.0;
			h=(ncv>mcv)?(3.0/(ncv-mcv)):0.0;

			p0=(pxy[0]-mcy)*f+0.5;		p1=(pxy[1]-mcy)*f+0.5;
			p2=(pxy[4]-mcy)*f+0.5;		p3=(pxy[5]-mcy)*f+0.5;
			p4=(pxu[0]-mcu)*g+0.5;		p5=(pxv[0]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[8]=(i>>8)&255; block[9]=(i>>0)&255;

			p0=(pxy[2]-mcy)*f+0.5;		p1=(pxy[3]-mcy)*f+0.5;
			p2=(pxy[6]-mcy)*f+0.5;		p3=(pxy[7]-mcy)*f+0.5;
			p4=(pxu[1]-mcu)*g+0.5;		p5=(pxv[1]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[10]=(i>>8)&255; block[11]=(i>>0)&255;
			
			p0=(pxy[ 8]-mcy)*f+0.5;		p1=(pxy[ 9]-mcy)*f+0.5;
			p2=(pxy[12]-mcy)*f+0.5;		p3=(pxy[13]-mcy)*f+0.5;
			p4=(pxu[ 2]-mcu)*g+0.5;		p5=(pxv[ 2]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[12]=(i>>8)&255; block[13]=(i>>0)&255;

			p0=(pxy[10]-mcy)*f+0.5;		p1=(pxy[11]-mcy)*f+0.5;
			p2=(pxy[14]-mcy)*f+0.5;		p3=(pxy[15]-mcy)*f+0.5;
			p4=(pxu[ 3]-mcu)*g+0.5;		p5=(pxv[ 3]-mcv)*h+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p4=(p4<0)?0:((p4>3)?3:p4);	p5=(p5<0)?0:((p5>3)?3:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|(p4<<2)|p5;
			block[14]=(i>>8)&255; block[15]=(i>>0)&255;

			return;
		}else
		{
			f=1.0/pow(2, mexp);
			g=1.0/pow(2, nexp);
			mcyi=fabs(mcy*255*f+0.5)+256;
			mcui=fabs(mcu2*127*f+0.5)+128;
			mcvi=fabs(mcv2*127*f+0.5)+128;
			ncyi=fabs(ncy*255*g+0.5)+256;
			ncui=fabs(ncu2*127*g+0.5)+128;
			ncvi=fabs(ncv2*127*g+0.5)+128;
			mcyi=(mcyi<0)?0:((mcyi>511)?511:mcyi);
			mcui=(mcui<0)?0:((mcui>255)?255:mcui);
			mcvi=(mcvi<0)?0:((mcvi>255)?255:mcvi);
			ncyi=(ncyi<0)?0:((ncyi>511)?511:ncyi);
			ncui=(ncui<0)?0:((ncui>255)?255:ncui);
			ncvi=(ncvi<0)?0:((ncvi>255)?255:ncvi);

			p0=(mexp+7);
			p1=(nexp+7);

			i=(0<<29)|(p0<<25)|(mcyi<<16)|(mcui<<8)|mcvi;
			j=(3<<29)|(p1<<25)|(ncyi<<16)|(ncui<<8)|ncvi;

			block[0]=(i>>24)&255;	block[1]=(i>>16)&255;
			block[2]=(i>> 8)&255;	block[3]=(i>> 0)&255;
			block[4]=(j>>24)&255;	block[5]=(j>>16)&255;
			block[6]=(j>> 8)&255;	block[7]=(j>> 0)&255;

			f=(ncy>mcy)?(7.0/(ncy-mcy)):0.0;

			p0=(pxy[0]-mcy)*f+0.5;		p1=(pxy[1]-mcy)*f+0.5;
			p2=(pxy[4]-mcy)*f+0.5;		p3=(pxy[5]-mcy)*f+0.5;
			p5=pxa1[0]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[8]=(i>>8)&255; block[9]=(i>>0)&255;

			p0=(pxy[2]-mcy)*f+0.5;		p1=(pxy[3]-mcy)*f+0.5;
			p2=(pxy[6]-mcy)*f+0.5;		p3=(pxy[7]-mcy)*f+0.5;
			p5=pxa1[1]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[10]=(i>>8)&255; block[11]=(i>>0)&255;
			
			p0=(pxy[ 8]-mcy)*f+0.5;		p1=(pxy[ 9]-mcy)*f+0.5;
			p2=(pxy[12]-mcy)*f+0.5;		p3=(pxy[13]-mcy)*f+0.5;
			p5=pxa1[ 2]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[12]=(i>>8)&255; block[13]=(i>>0)&255;

			p0=(pxy[10]-mcy)*f+0.5;		p1=(pxy[11]-mcy)*f+0.5;
			p2=(pxy[14]-mcy)*f+0.5;		p3=(pxy[15]-mcy)*f+0.5;
			p5=pxa1[ 3]*15+0.5;
			p0=(p0<0)?0:((p0>7)?7:p0);	p1=(p1<0)?0:((p1>7)?7:p1);
			p2=(p2<0)?0:((p2>7)?7:p2);	p3=(p3<0)?0:((p3>7)?7:p3);
			p5=(p5<0)?0:((p5>15)?15:p5);
			i=(p0<<13)|(p1<<10)|(p2<<7)|(p3<<4)|p5;
			block[14]=(i>>8)&255; block[15]=(i>>0)&255;

			return;
		}
	}
}

void BGBBTJ_BT1D_EncodeBlockBX1A_S16(byte *block,
	short *rgba, int xstride, int ystride)
{
	float trgba[16*4];
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=(i*4+j)*4;
		trgba[l+0]=rgba[k+0]/255.0;
		trgba[l+1]=rgba[k+1]/255.0;
		trgba[l+2]=rgba[k+2]/255.0;
		trgba[l+3]=rgba[k+3]/255.0;
	}
	
	BGBBTJ_BT1D_EncodeBlockBX1A(block, trgba, 4, 4*4);
}

void BGBBTJ_BT1D_EncodeHalfBlockBX1A_S16(byte *block,
	short *rgba, int xstride, int ystride)
{
	float trgba[16*4];
	int i, j, k, l;
	
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=i*ystride+j*xstride;
		l=(i*4+j)*4;
		trgba[l+0]=rgba[k+0]/255.0;
		trgba[l+1]=rgba[k+1]/255.0;
		trgba[l+2]=rgba[k+2]/255.0;
		trgba[l+3]=rgba[k+3]/255.0;
	}
	
	BGBBTJ_BT1D_EncodeHalfBlockBX1A(block, trgba, 4, 4*4);
}

void BGBBTJ_BT1D_DecodeBlockBX1A_FillPlanesFlat(
	float *pxy, float *pxu, float *pxv, float *pxa,
	float cy, float cu, float cv, float ca)
{
	pxy[ 0]=pxy[ 1]=pxy[ 4]=pxy[ 5]=cy;
	pxy[ 2]=pxy[ 3]=pxy[ 6]=pxy[ 7]=cy;
	pxy[ 8]=pxy[ 9]=pxy[12]=pxy[13]=cy;
	pxy[10]=pxy[11]=pxy[14]=pxy[15]=cy;
	pxu[ 0]=pxu[ 1]=pxu[ 4]=pxu[ 5]=cu;
	pxu[ 2]=pxu[ 3]=pxu[ 6]=pxu[ 7]=cu;
	pxu[ 8]=pxu[ 9]=pxu[12]=pxu[13]=cu;
	pxu[10]=pxu[11]=pxu[14]=pxu[15]=cu;
	pxv[ 0]=pxv[ 1]=pxv[ 4]=pxv[ 5]=cv;
	pxv[ 2]=pxv[ 3]=pxv[ 6]=pxv[ 7]=cv;
	pxv[ 8]=pxv[ 9]=pxv[12]=pxv[13]=cv;
	pxv[10]=pxv[11]=pxv[14]=pxv[15]=cv;
	pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=ca;
	pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=ca;
	pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=ca;
	pxa[10]=pxa[11]=pxa[14]=pxa[15]=ca;
}

void BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
	float *pxy, float *pxu, float *pxv, float *pxa,
	float *rgba, int xstride, int ystride)
{
	float cy, cu, cv, cr, cg, cb, ca;
	int i, j, k, l;

#if 1
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		l=i*4+j;
		cy=pxy[l];	cu=pxu[l];
		cv=pxv[l];	ca=pxa[l];
	 	cg=cy+(cu/2);
		cr=cg-cu-(cv/2);
		cb=cg-cu+(cv/2);
		rgba[k+0]=cr;	rgba[k+1]=cg;
		rgba[k+2]=cb;	rgba[k+3]=ca;
	}
#endif
}

void BGBBTJ_BT1D_DecodeBlockBX1A_FlatToRGBA(
	float cy, float cu, float cv, float ca,
	float *rgba, int xstride, int ystride)
{
	float cr, cg, cb;
	int i, j, k, l;

 	cg=cy+(cu/2);
	cr=cg-cu-(cv/2);
	cb=cg-cu+(cv/2);

#if 1
	for(i=0; i<4; i++)
		for(j=0; j<4; j++)
	{
		k=(i*ystride)+(j*xstride);
		rgba[k+0]=cr;	rgba[k+1]=cg;
		rgba[k+2]=cb;	rgba[k+3]=ca;
	}
#endif
}

void BGBBTJ_BT1D_DecodeBlockBX1A(byte *block,
	float *rgba, int xstride, int ystride)
{
	float pty[8], ptu[8], ptv[8], pta[8];
	float pxy[16], pxu[16], pxv[16], pxa[16];
	float pxu1[4], pxv1[4], pxa1[4];
	float cy, cu, cv, cr, cg, cb, ca;
	float f, g, h;
	int myi, mui, mvi, nyi, nui, nvi, nai;
	int bm, emi, eni;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int p0, p1, p2, p3;
	int i, j, k, l;
	
//	bm=((block[0]>>5)&0x07)|((block[4]>>2)&0x38);
	bm=((block[0]>>2)&0x38)|((block[4]>>5)&0x07);
	i0=(block[0]<<24)|(block[1]<<16)|(block[2]<<8)|(block[3]);
	i1=(block[4]<<24)|(block[5]<<16)|(block[6]<<8)|(block[7]);
//	i2=(block[8]<<24)|(block[9]<<16)|(block[10]<<8)|(block[11]);
//	i3=(block[12]<<24)|(block[13]<<16)|(block[14]<<8)|(block[15]);
	j0=(block[ 8]<<8)|(block[ 9]);	j1=(block[10]<<8)|(block[11]);
	j2=(block[12]<<8)|(block[13]);	j3=(block[14]<<8)|(block[15]);
	
	if(bm==0)
	{
		myi=(i0>>18)&2047;	mui=(i0>>9)&511;	mvi=(i0>>0)&511;
		nyi=(i1>>18)&2047;	nui=(i1>>9)&511;	nvi=(i1>>0)&511;
		
		pty[0]=myi/2047.0; pty[7]=nyi/2047.0;
		pty[1]=((6.0/7.0)*pty[0])+((1.0/7.0)*pty[7]);
		pty[2]=((5.0/7.0)*pty[0])+((2.0/7.0)*pty[7]);
		pty[3]=((4.0/7.0)*pty[0])+((3.0/7.0)*pty[7]);
		pty[4]=((3.0/7.0)*pty[0])+((4.0/7.0)*pty[7]);
		pty[5]=((2.0/7.0)*pty[0])+((5.0/7.0)*pty[7]);
		pty[6]=((1.0/7.0)*pty[0])+((6.0/7.0)*pty[7]);

		ptu[0]=(mui-256)/255.0; ptu[3]=(nui-256)/255.0;
		ptv[0]=(mvi-256)/255.0; ptv[3]=(nvi-256)/255.0;
		ptu[1]=((2.0/3.0)*ptu[0])+((1.0/3.0)*ptu[3]);
		ptu[2]=((1.0/3.0)*ptu[0])+((2.0/3.0)*ptu[3]);
		ptv[1]=((2.0/3.0)*ptv[0])+((1.0/3.0)*ptv[3]);
		ptv[2]=((1.0/3.0)*ptv[0])+((2.0/3.0)*ptv[3]);
		
		pxy[ 0]=pty[(j0>>13)&7];	pxy[ 1]=pty[(j0>>10)&7];
		pxy[ 4]=pty[(j0>> 7)&7];	pxy[ 5]=pty[(j0>> 4)&7];
		pxy[ 2]=pty[(j1>>13)&7];	pxy[ 3]=pty[(j1>>10)&7];
		pxy[ 6]=pty[(j1>> 7)&7];	pxy[ 7]=pty[(j1>> 4)&7];
		pxy[ 8]=pty[(j2>>13)&7];	pxy[ 9]=pty[(j2>>10)&7];
		pxy[12]=pty[(j2>> 7)&7];	pxy[13]=pty[(j2>> 4)&7];
		pxy[10]=pty[(j3>>13)&7];	pxy[11]=pty[(j3>>10)&7];
		pxy[14]=pty[(j3>> 7)&7];	pxy[15]=pty[(j3>> 4)&7];
		pxu1[0]=ptu[(j0>> 2)&3];	pxv1[0]=ptv[(j0    )&3];
		pxu1[1]=ptu[(j1>> 2)&3];	pxv1[1]=ptv[(j1    )&3];
		pxu1[2]=ptu[(j2>> 2)&3];	pxv1[2]=ptv[(j2    )&3];
		pxu1[3]=ptu[(j3>> 2)&3];	pxv1[3]=ptv[(j3    )&3];

		pxu[ 0]=pxu[ 1]=pxu[ 4]=pxu[ 5]=pxu1[0];
		pxv[ 0]=pxv[ 1]=pxv[ 4]=pxv[ 5]=pxv1[0];
		pxu[ 2]=pxu[ 3]=pxu[ 6]=pxu[ 7]=pxu1[1];
		pxv[ 2]=pxv[ 3]=pxv[ 6]=pxv[ 7]=pxv1[1];
		pxu[ 8]=pxu[ 9]=pxu[12]=pxu[13]=pxu1[2];
		pxv[ 8]=pxv[ 9]=pxv[12]=pxv[13]=pxv1[2];
		pxu[10]=pxu[11]=pxu[14]=pxu[15]=pxu1[3];
		pxv[10]=pxv[11]=pxv[14]=pxv[15]=pxv1[3];
		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=1.0;
		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=1.0;
		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=1.0;
		pxa[10]=pxa[11]=pxa[14]=pxa[15]=1.0;
		
		BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride);
	}else if(bm==1)
	{
		myi=(i0>>16)&511;	mui=(i0>>8)&255;	mvi=(i0>>0)&255;
		nyi=(i1>>16)&511;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;
		emi=(i0>>25)&15;	eni=(i1>>25)&15;
		f=pow(2.0, emi-7);
		g=pow(2.0, eni-7);
		
		pty[0]=((myi-256)/255.0)*f; pty[7]=((nyi-256)/255.0)*g;
		pty[1]=((6.0/7.0)*pty[0])+((1.0/7.0)*pty[7]);
		pty[2]=((5.0/7.0)*pty[0])+((2.0/7.0)*pty[7]);
		pty[3]=((4.0/7.0)*pty[0])+((3.0/7.0)*pty[7]);
		pty[4]=((3.0/7.0)*pty[0])+((4.0/7.0)*pty[7]);
		pty[5]=((2.0/7.0)*pty[0])+((5.0/7.0)*pty[7]);
		pty[6]=((1.0/7.0)*pty[0])+((6.0/7.0)*pty[7]);

		ptu[0]=((mui-128)/127.0)*f; ptu[3]=((nui-128)/127.0)*g;
		ptv[0]=((mvi-128)/127.0)*f; ptv[3]=((nvi-128)/127.0)*g;
		ptu[1]=((2.0/3.0)*ptu[0])+((1.0/3.0)*ptu[3]);
		ptu[2]=((1.0/3.0)*ptu[0])+((2.0/3.0)*ptu[3]);
		ptv[1]=((2.0/3.0)*ptv[0])+((1.0/3.0)*ptv[3]);
		ptv[2]=((1.0/3.0)*ptv[0])+((2.0/3.0)*ptv[3]);
		
		pxy[ 0]=pty[(j0>>13)&7];	pxy[ 1]=pty[(j0>>10)&7];
		pxy[ 4]=pty[(j0>> 7)&7];	pxy[ 5]=pty[(j0>> 4)&7];
		pxy[ 2]=pty[(j1>>13)&7];	pxy[ 3]=pty[(j1>>10)&7];
		pxy[ 6]=pty[(j1>> 7)&7];	pxy[ 7]=pty[(j1>> 4)&7];
		pxy[ 8]=pty[(j2>>13)&7];	pxy[ 9]=pty[(j2>>10)&7];
		pxy[12]=pty[(j2>> 7)&7];	pxy[13]=pty[(j2>> 4)&7];
		pxy[10]=pty[(j3>>13)&7];	pxy[11]=pty[(j3>>10)&7];
		pxy[14]=pty[(j3>> 7)&7];	pxy[15]=pty[(j3>> 4)&7];
		pxu1[0]=ptu[(j0>> 2)&3];	pxv1[0]=ptv[(j0    )&3];
		pxu1[1]=ptu[(j1>> 2)&3];	pxv1[1]=ptv[(j1    )&3];
		pxu1[2]=ptu[(j2>> 2)&3];	pxv1[2]=ptv[(j2    )&3];
		pxu1[3]=ptu[(j3>> 2)&3];	pxv1[3]=ptv[(j3    )&3];
		
		pxu[ 0]=pxu[ 1]=pxu[ 4]=pxu[ 5]=pxu1[0];
		pxv[ 0]=pxv[ 1]=pxv[ 4]=pxv[ 5]=pxv1[0];
		pxu[ 2]=pxu[ 3]=pxu[ 6]=pxu[ 7]=pxu1[1];
		pxv[ 2]=pxv[ 3]=pxv[ 6]=pxv[ 7]=pxv1[1];
		pxu[ 8]=pxu[ 9]=pxu[12]=pxu[13]=pxu1[2];
		pxv[ 8]=pxv[ 9]=pxv[12]=pxv[13]=pxv1[2];
		pxu[10]=pxu[11]=pxu[14]=pxu[15]=pxu1[3];
		pxv[10]=pxv[11]=pxv[14]=pxv[15]=pxv1[3];
		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=1.0;
		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=1.0;
		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=1.0;
		pxa[10]=pxa[11]=pxa[14]=pxa[15]=1.0;

		BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride);
	}else if(bm==2)
	{
		myi=(i0>>18)&2047;	mui=(i0>>9)&511;	mvi=(i0>>0)&511;
		nyi=(i1>>18)&2047;	nui=(i1>>9)&511;	nvi=(i1>>0)&511;
		
		pty[0]=myi/2047.0; pty[7]=nyi/2047.0;
		pty[1]=((6.0/7.0)*pty[0])+((1.0/7.0)*pty[7]);
		pty[2]=((5.0/7.0)*pty[0])+((2.0/7.0)*pty[7]);
		pty[3]=((4.0/7.0)*pty[0])+((3.0/7.0)*pty[7]);
		pty[4]=((3.0/7.0)*pty[0])+((4.0/7.0)*pty[7]);
		pty[5]=((2.0/7.0)*pty[0])+((5.0/7.0)*pty[7]);
		pty[6]=((1.0/7.0)*pty[0])+((6.0/7.0)*pty[7]);

		ptu[0]=(mui-256)/255.0; ptu[7]=(nui-256)/255.0;
		ptv[0]=(mvi-256)/255.0; ptv[7]=(nvi-256)/255.0;
		ptu[1]=((6.0/7.0)*ptu[0])+((1.0/7.0)*ptu[7]);
		ptu[2]=((5.0/7.0)*ptu[0])+((2.0/7.0)*ptu[7]);
		ptu[3]=((4.0/7.0)*ptu[0])+((3.0/7.0)*ptu[7]);
		ptu[4]=((3.0/7.0)*ptu[0])+((4.0/7.0)*ptu[7]);
		ptu[5]=((2.0/7.0)*ptu[0])+((5.0/7.0)*ptu[7]);
		ptu[6]=((1.0/7.0)*ptu[0])+((6.0/7.0)*ptu[7]);
		ptv[1]=((6.0/7.0)*ptv[0])+((1.0/7.0)*ptv[7]);
		ptv[2]=((5.0/7.0)*ptv[0])+((2.0/7.0)*ptv[7]);
		ptv[3]=((4.0/7.0)*ptv[0])+((3.0/7.0)*ptv[7]);
		ptv[4]=((3.0/7.0)*ptv[0])+((4.0/7.0)*ptv[7]);
		ptv[5]=((2.0/7.0)*ptv[0])+((5.0/7.0)*ptv[7]);
		ptv[6]=((1.0/7.0)*ptv[0])+((6.0/7.0)*ptv[7]);
		
		p0=(j0>>13)&7;	p1=(j0>>10)&7;	p2=(j0>> 7)&7;	p3=(j0>> 4)&7;
		pxy[ 0]=pty[p0]; pxy[ 1]=pty[p1]; pxy[ 4]=pty[p2]; pxy[ 5]=pty[p3];
		pxu[ 0]=ptu[p0]; pxu[ 1]=ptu[p1]; pxu[ 4]=ptu[p2]; pxu[ 5]=ptu[p3];
		pxv[ 0]=ptv[p0]; pxv[ 1]=ptv[p1]; pxv[ 4]=ptv[p2]; pxv[ 5]=ptv[p3];
		p0=(j1>>13)&7;	p1=(j1>>10)&7;	p2=(j1>> 7)&7;	p3=(j1>> 4)&7;
		pxy[ 2]=pty[p0]; pxy[ 3]=pty[p1]; pxy[ 6]=pty[p2]; pxy[ 7]=pty[p3];
		pxu[ 2]=ptu[p0]; pxu[ 3]=ptu[p1]; pxu[ 6]=ptu[p2]; pxu[ 7]=ptu[p3];
		pxv[ 2]=ptv[p0]; pxv[ 3]=ptv[p1]; pxv[ 6]=ptv[p2]; pxv[ 7]=ptv[p3];
		p0=(j2>>13)&7;	p1=(j2>>10)&7;	p2=(j2>> 7)&7;	p3=(j2>> 4)&7;
		pxy[ 8]=pty[p0]; pxy[ 9]=pty[p1]; pxy[12]=pty[p2]; pxy[13]=pty[p3];
		pxu[ 8]=ptu[p0]; pxu[ 9]=ptu[p1]; pxu[12]=ptu[p2]; pxu[13]=ptu[p3];
		pxv[ 8]=ptv[p0]; pxv[ 9]=ptv[p1]; pxv[12]=ptv[p2]; pxv[13]=ptv[p3];
		p0=(j3>>13)&7;	p1=(j3>>10)&7;	p2=(j3>> 7)&7;	p3=(j3>> 4)&7;
		pxy[10]=pty[p0]; pxy[11]=pty[p1]; pxy[14]=pty[p2]; pxy[15]=pty[p3];
		pxu[10]=ptu[p0]; pxu[11]=ptu[p1]; pxu[14]=ptu[p2]; pxu[15]=ptu[p3];
		pxv[10]=ptv[p0]; pxv[11]=ptv[p1]; pxv[14]=ptv[p2]; pxv[15]=ptv[p3];

		pxa1[0]=(j0&15)/15.0;		pxa1[1]=(j1&15)/15.0;
		pxa1[2]=(j2&15)/15.0;		pxa1[3]=(j3&15)/15.0;
		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=pxa1[0];
		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=pxa1[1];
		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=pxa1[2];
		pxa[10]=pxa[11]=pxa[14]=pxa[15]=pxa1[3];

		BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride);
	}else if(bm==3)
	{
		myi=(i0>>16)&511;	mui=(i0>>8)&255;	mvi=(i0>>0)&255;
		nyi=(i1>>16)&511;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;
		emi=(i0>>25)&15;	eni=(i1>>25)&15;
		f=pow(2.0, emi-7);
		g=pow(2.0, eni-7);
		
		pty[0]=((myi-256)/255.0)*f; pty[7]=((nyi-256)/255.0)*g;
		pty[1]=((6.0/7.0)*pty[0])+((1.0/7.0)*pty[7]);
		pty[2]=((5.0/7.0)*pty[0])+((2.0/7.0)*pty[7]);
		pty[3]=((4.0/7.0)*pty[0])+((3.0/7.0)*pty[7]);
		pty[4]=((3.0/7.0)*pty[0])+((4.0/7.0)*pty[7]);
		pty[5]=((2.0/7.0)*pty[0])+((5.0/7.0)*pty[7]);
		pty[6]=((1.0/7.0)*pty[0])+((6.0/7.0)*pty[7]);

		ptu[0]=((mui-128)/127.0)*f; ptu[7]=((nui-128)/127.0)*g;
		ptv[0]=((mvi-128)/127.0)*f; ptv[7]=((nvi-128)/127.0)*g;
		ptu[1]=((6.0/7.0)*ptu[0])+((1.0/7.0)*ptu[7]);
		ptu[2]=((5.0/7.0)*ptu[0])+((2.0/7.0)*ptu[7]);
		ptu[3]=((4.0/7.0)*ptu[0])+((3.0/7.0)*ptu[7]);
		ptu[4]=((3.0/7.0)*ptu[0])+((4.0/7.0)*ptu[7]);
		ptu[5]=((2.0/7.0)*ptu[0])+((5.0/7.0)*ptu[7]);
		ptu[6]=((1.0/7.0)*ptu[0])+((6.0/7.0)*ptu[7]);
		ptv[1]=((6.0/7.0)*ptv[0])+((1.0/7.0)*ptv[7]);
		ptv[2]=((5.0/7.0)*ptv[0])+((2.0/7.0)*ptv[7]);
		ptv[3]=((4.0/7.0)*ptv[0])+((3.0/7.0)*ptv[7]);
		ptv[4]=((3.0/7.0)*ptv[0])+((4.0/7.0)*ptv[7]);
		ptv[5]=((2.0/7.0)*ptv[0])+((5.0/7.0)*ptv[7]);
		ptv[6]=((1.0/7.0)*ptv[0])+((6.0/7.0)*ptv[7]);
		
		p0=(j0>>13)&7;	p1=(j0>>10)&7;	p2=(j0>> 7)&7;	p3=(j0>> 4)&7;
		pxy[ 0]=pty[p0]; pxy[ 1]=pty[p1]; pxy[ 4]=pty[p2]; pxy[ 5]=pty[p3];
		pxu[ 0]=ptu[p0]; pxu[ 1]=ptu[p1]; pxu[ 4]=ptu[p2]; pxu[ 5]=ptu[p3];
		pxv[ 0]=ptv[p0]; pxv[ 1]=ptv[p1]; pxv[ 4]=ptv[p2]; pxv[ 5]=ptv[p3];
		p0=(j1>>13)&7;	p1=(j1>>10)&7;	p2=(j1>> 7)&7;	p3=(j1>> 4)&7;
		pxy[ 2]=pty[p0]; pxy[ 3]=pty[p1]; pxy[ 6]=pty[p2]; pxy[ 7]=pty[p3];
		pxu[ 2]=ptu[p0]; pxu[ 3]=ptu[p1]; pxu[ 6]=ptu[p2]; pxu[ 7]=ptu[p3];
		pxv[ 2]=ptv[p0]; pxv[ 3]=ptv[p1]; pxv[ 6]=ptv[p2]; pxv[ 7]=ptv[p3];
		p0=(j2>>13)&7;	p1=(j2>>10)&7;	p2=(j2>> 7)&7;	p3=(j2>> 4)&7;
		pxy[ 8]=pty[p0]; pxy[ 9]=pty[p1]; pxy[12]=pty[p2]; pxy[13]=pty[p3];
		pxu[ 8]=ptu[p0]; pxu[ 9]=ptu[p1]; pxu[12]=ptu[p2]; pxu[13]=ptu[p3];
		pxv[ 8]=ptv[p0]; pxv[ 9]=ptv[p1]; pxv[12]=ptv[p2]; pxv[13]=ptv[p3];
		p0=(j3>>13)&7;	p1=(j3>>10)&7;	p2=(j3>> 7)&7;	p3=(j3>> 4)&7;
		pxy[10]=pty[p0]; pxy[11]=pty[p1]; pxy[14]=pty[p2]; pxy[15]=pty[p3];
		pxu[10]=ptu[p0]; pxu[11]=ptu[p1]; pxu[14]=ptu[p2]; pxu[15]=ptu[p3];
		pxv[10]=ptv[p0]; pxv[11]=ptv[p1]; pxv[14]=ptv[p2]; pxv[15]=ptv[p3];

		pxa1[0]=(j0&15)/15.0;		pxa1[1]=(j1&15)/15.0;
		pxa1[2]=(j2&15)/15.0;		pxa1[3]=(j3&15)/15.0;
		pxa[ 0]=pxa[ 1]=pxa[ 4]=pxa[ 5]=pxa1[0];
		pxa[ 2]=pxa[ 3]=pxa[ 6]=pxa[ 7]=pxa1[1];
		pxa[ 8]=pxa[ 9]=pxa[12]=pxa[13]=pxa1[2];
		pxa[10]=pxa[11]=pxa[14]=pxa[15]=pxa1[3];

		BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride);
	}else if((bm>=48) && (bm<56))
	{
		//Flat Color
		if(bm==48)
		{
			//LDR Flat Color
			nyi=(i1>>18)&2047;	nui=(i1>>9)&511;	nvi=(i1>>0)&511;
			cy=nyi/2047.0;
			cu=(nui-256)/255.0;
			cv=(nvi-256)/255.0;
//			BGBBTJ_BT1D_DecodeBlockBX1A_FillPlanesFlat(
//				pxy, pxu, pxv, pxa, cy, cu, cv, 1.0);
//			BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
//				pxy, pxu, pxv, pxa, rgba, xstride, ystride);
			BGBBTJ_BT1D_DecodeBlockBX1A_FlatToRGBA(
				cy, cu, cv, 1.0, rgba, xstride, ystride);
		}else if(bm==49)
		{
			//HDR Flat Color
			nyi=(i1>>16)&511;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;
			eni=(i1>>25)&15;
			f=pow(2.0, eni-7);

			cy=f*((nyi-256)/255.0);
			cu=f*((nui-128)/127.0);
			cv=f*((nvi-128)/127.0);
//			BGBBTJ_BT1D_DecodeBlockBX1A_FillPlanesFlat(
//				pxy, pxu, pxv, pxa, cy, cu, cv, 1.0);
//			BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
//				pxy, pxu, pxv, pxa, rgba, xstride, ystride);
			BGBBTJ_BT1D_DecodeBlockBX1A_FlatToRGBA(
				cy, cu, cv, 1.0, rgba, xstride, ystride);
		}else if(bm==50)
		{
			//LDR Flat Color + Alpha
			nai=(i1>>24)&31;
			nyi=(i1>>16)&255;	nui=(i1>>8)&255;	nvi=(i1>>0)&255;
			cy=nyi/255.0;
			cu=(nui-128)/127.0;
			cv=(nvi-128)/127.0;
			ca=nai/31.0;
//			BGBBTJ_BT1D_DecodeBlockBX1A_FillPlanesFlat(
//				pxy, pxu, pxv, pxa, cy, cu, cv, ca);
//			BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
//				pxy, pxu, pxv, pxa, rgba, xstride, ystride);
			BGBBTJ_BT1D_DecodeBlockBX1A_FlatToRGBA(
				cy, cu, cv, ca, rgba, xstride, ystride);
		}else if(bm==51)
		{
			//HDR Flat Color + Alpha
			nyi=(i1>>17)&255;	nui=(i1>>11)&63;	nvi=(i1>>5)&63;
			nai=(i1>>0)&31; eni=(i1>>25)&15;
			f=pow(2.0, eni-7);

			cy=f*((nyi-128)/127.0);
			cu=f*((nui-32)/31.0);
			cv=f*((nvi-32)/31.0);
			ca=nai/31.0;
//			BGBBTJ_BT1D_DecodeBlockBX1A_FillPlanesFlat(
//				pxy, pxu, pxv, pxa, cy, cu, cv, ca);
//			BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
//				pxy, pxu, pxv, pxa, rgba, xstride, ystride);
			BGBBTJ_BT1D_DecodeBlockBX1A_FlatToRGBA(
				cy, cu, cv, ca, rgba, xstride, ystride);
		}else
		{
			//Half-Width Flat Color
			nai=(i1>>30)&1;
			nyi=(i1>>24)&63;	nui=(i1>>20)&15;	nvi=(i1>>16)&15;
			cy=nyi/63.0;
			cu=(nui-8)/7.0;
			cv=(nvi-8)/7.0;
			ca=nai?0.0:1.0;
//			BGBBTJ_BT1D_DecodeBlockBX1A_FillPlanesFlat(
//				pxy, pxu, pxv, pxa, cy, cu, cv, ca);
//			BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
//				pxy, pxu, pxv, pxa, rgba, xstride, ystride);
			BGBBTJ_BT1D_DecodeBlockBX1A_FlatToRGBA(
				cy, cu, cv, ca, rgba, xstride, ystride);
		}
	}else if(bm==63)
	{
		//LDR Half-Width
		
		myi=(i1>>22)&63;	mui=(i1>>12)&15;	mvi=(i1>>8)&15;
		nyi=(i1>>16)&63;	nui=(i1>> 4)&15;	nvi=(i1>>0)&15;
		nai=(i1>>28)&1;
		
		if(nai)
		{
			pty[0]=myi/63.0; pty[2]=nyi/63.0;
			ptu[0]=(mui-8)/7.0; ptu[2]=(nui-8)/7.0;
			ptv[0]=(mvi-8)/7.0; ptv[2]=(nvi-8)/7.0;
			pty[1]=((1.0/2.0)*pty[0])+((1.0/2.0)*pty[2]);
			ptu[1]=((1.0/2.0)*ptu[0])+((1.0/2.0)*ptu[2]);
			ptv[1]=((1.0/2.0)*ptv[0])+((1.0/2.0)*ptv[2]);
			pta[0]=1.0;	pta[1]=1.0;	pta[2]=1.0;
			pty[3]=pty[1];	ptu[3]=ptu[1];
			ptv[3]=ptv[1];	pta[3]=0.0;
		}else
		{
			pty[0]=myi/63.0; pty[3]=nyi/63.0;
			ptu[0]=(mui-8)/7.0; ptu[3]=(nui-8)/7.0;
			ptv[0]=(mvi-8)/7.0; ptv[3]=(nvi-8)/7.0;
			pty[1]=((2.0/3.0)*pty[0])+((1.0/3.0)*pty[3]);
			pty[2]=((1.0/3.0)*pty[0])+((2.0/3.0)*pty[3]);
			ptu[1]=((2.0/3.0)*ptu[0])+((1.0/3.0)*ptu[3]);
			ptu[2]=((1.0/3.0)*ptu[0])+((2.0/3.0)*ptu[3]);
			ptv[1]=((2.0/3.0)*ptv[0])+((1.0/3.0)*ptv[3]);
			ptv[2]=((1.0/3.0)*ptv[0])+((2.0/3.0)*ptv[3]);
			pta[0]=1.0;	pta[1]=1.0;	pta[2]=1.0;	pta[3]=1.0;
		}
		
		p0=(j0>>14)&3;	p1=(j0>>12)&3;	p2=(j0>>10)&3;	p3=(j0>> 8)&3;
		pxy[ 0]=pty[p0]; pxy[ 1]=pty[p1]; pxy[ 4]=pty[p2]; pxy[ 5]=pty[p3];
		pxu[ 0]=ptu[p0]; pxu[ 1]=ptu[p1]; pxu[ 4]=ptu[p2]; pxu[ 5]=ptu[p3];
		pxv[ 0]=ptv[p0]; pxv[ 1]=ptv[p1]; pxv[ 4]=ptv[p2]; pxv[ 5]=ptv[p3];
		pxa[ 0]=pta[p0]; pxa[ 1]=pta[p1]; pxa[ 4]=pta[p2]; pxa[ 5]=pta[p3];
		p0=(j0>> 6)&3;	p1=(j0>> 4)&3;	p2=(j0>> 2)&3;	p3=(j0>> 0)&3;
		pxy[ 2]=pty[p0]; pxy[ 3]=pty[p1]; pxy[ 6]=pty[p2]; pxy[ 7]=pty[p3];
		pxu[ 2]=ptu[p0]; pxu[ 3]=ptu[p1]; pxu[ 6]=ptu[p2]; pxu[ 7]=ptu[p3];
		pxv[ 2]=ptv[p0]; pxv[ 3]=ptv[p1]; pxv[ 6]=ptv[p2]; pxv[ 7]=ptv[p3];
		pxa[ 2]=pta[p0]; pxa[ 3]=pta[p1]; pxa[ 6]=pta[p2]; pxa[ 7]=pta[p3];
		p0=(j1>>14)&3;	p1=(j1>>12)&3;	p2=(j1>>10)&3;	p3=(j1>> 8)&3;
		pxy[ 8]=pty[p0]; pxy[ 9]=pty[p1]; pxy[12]=pty[p2]; pxy[13]=pty[p3];
		pxu[ 8]=ptu[p0]; pxu[ 9]=ptu[p1]; pxu[12]=ptu[p2]; pxu[13]=ptu[p3];
		pxv[ 8]=ptv[p0]; pxv[ 9]=ptv[p1]; pxv[12]=ptv[p2]; pxv[13]=ptv[p3];
		pxa[ 8]=pta[p0]; pxa[ 9]=pta[p1]; pxa[12]=pta[p2]; pxa[13]=pta[p3];
		p0=(j1>> 6)&3;	p1=(j1>> 4)&3;	p2=(j1>> 2)&3;	p3=(j1>> 0)&3;
		pxy[10]=pty[p0]; pxy[11]=pty[p1]; pxy[14]=pty[p2]; pxy[15]=pty[p3];
		pxu[10]=ptu[p0]; pxu[11]=ptu[p1]; pxu[14]=ptu[p2]; pxu[15]=ptu[p3];
		pxv[10]=ptv[p0]; pxv[11]=ptv[p1]; pxv[14]=ptv[p2]; pxv[15]=ptv[p3];
		pxa[10]=pta[p0]; pxa[11]=pta[p1]; pxa[14]=pta[p2]; pxa[15]=pta[p3];
		
		BGBBTJ_BT1D_DecodeBlockBX1A_PlanesToRGBA(
			pxy, pxu, pxv, pxa, rgba, xstride, ystride);
	}
}

void BGBBTJ_BT1D_DecodeBlockBX1A_RGBA32(byte *block,
	byte *rgba, int xstride, int ystride)
{
	float tblk[16*4];
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	BGBBTJ_BT1D_DecodeBlockBX1A(
		block,
		tblk, 4, 4*4);
		
	if(xstride==3)
	{
		for(k=0; k<4; k++)
			for(l=0; l<4; l++)
		{
			l0=(k*4+l)*4;
			l1=(k*ystride)+(l*3);
			i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
			i2=tblk[l0+2]*255;
			i0=(i0<0)?0:((i0>255)?255:i0);
			i1=(i1<0)?0:((i1>255)?255:i1);
			i2=(i2<0)?0:((i2>255)?255:i2);
			rgba[l1+0]=i0;	rgba[l1+1]=i1;
			rgba[l1+2]=i2;
		}
	}else
	{
		for(k=0; k<4; k++)
			for(l=0; l<4; l++)
		{
			l0=(k*4+l)*4;
			l1=(k*ystride)+(l*xstride);
			i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
			i2=tblk[l0+2]*255;	i3=tblk[l0+3]*255;
			i0=(i0<0)?0:((i0>255)?255:i0);
			i1=(i1<0)?0:((i1>255)?255:i1);
			i2=(i2<0)?0:((i2>255)?255:i2);
			i3=(i3<0)?0:((i3>255)?255:i3);
			rgba[l1+0]=i0;	rgba[l1+1]=i1;
			rgba[l1+2]=i2;	rgba[l1+3]=i3;
		}
	}
}

void BGBBTJ_BT1D_DecodeBlockBX1A_Q11_4(byte *block,
	short *rgba, int xstride, int ystride)
{
	float tblk[16*4];
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	BGBBTJ_BT1D_DecodeBlockBX1A(
		block,
		tblk, 4, 4*4);
		
	if(xstride==3)
	{
		for(k=0; k<4; k++)
			for(l=0; l<4; l++)
		{
			l0=(k*4+l)*4;
			l1=(k*ystride)+(l*3);
			i0=tblk[l0+0]*4080;	i1=tblk[l0+1]*4080;
			i2=tblk[l0+2]*4080;
			i0=(i0<(-32768))?(-32768):((i0>32767)?32767:i0);
			i1=(i1<(-32768))?(-32768):((i1>32767)?32767:i1);
			i2=(i2<(-32768))?(-32768):((i2>32767)?32767:i2);
			rgba[l1+0]=i0;	rgba[l1+1]=i1;
			rgba[l1+2]=i2;
		}
	}else
	{
		for(k=0; k<4; k++)
			for(l=0; l<4; l++)
		{
			l0=(k*4+l)*4;
			l1=(k*ystride)+(l*xstride);
			i0=tblk[l0+0]*4080;	i1=tblk[l0+1]*4080;
			i2=tblk[l0+2]*4080;	i3=tblk[l0+3]*4080;
			i0=(i0<(-32768))?(-32768):((i0>32767)?32767:i0);
			i1=(i1<(-32768))?(-32768):((i1>32767)?32767:i1);
			i2=(i2<(-32768))?(-32768):((i2>32767)?32767:i2);
			i3=(i3<(-32768))?(-32768):((i3>32767)?32767:i3);
			rgba[l1+0]=i0;	rgba[l1+1]=i1;
			rgba[l1+2]=i2;	rgba[l1+3]=i3;
		}
	}
}

static int bgbbtj_logn16(float v, float ls)
{
	float f;
	
	if(v>0)
	{
		f=(log(v)*ls)+16384;
	}else
	{
		f=-(log(-v)*ls)-16384;
	}
	return(f+0.5);
}

static int bgbbtj_logn16h(float v)
{
	return((*(int *)(&v))>>16);
}

void BGBBTJ_BT1D_DecodeBlockBX1A_LS16(byte *block,
	short *rgba, int xstride, int ystride)
{
	float tblk[16*4];
	float ls;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	BGBBTJ_BT1D_DecodeBlockBX1A(
		block, tblk, 4, 4*4);
	
	ls=16383/log(256.0);
	
	for(k=0; k<4; k++)
		for(l=0; l<4; l++)
	{
		l0=(k*4+l)*4;
		l1=(k*ystride)+(l*xstride);
//		i0=log(tblk[l0+0])*ls;
//		i1=log(tblk[l0+1])*ls;
//		i2=log(tblk[l0+2])*ls;
//		i3=log(tblk[l0+3])*ls;
		i0=bgbbtj_logn16(tblk[l0+0], ls);
		i1=bgbbtj_logn16(tblk[l0+1], ls);
		i2=bgbbtj_logn16(tblk[l0+2], ls);
		i3=bgbbtj_logn16(tblk[l0+3], ls);
//		i0=bgbbtj_logn16h(tblk[l0+0]);
//		i1=bgbbtj_logn16h(tblk[l0+1]);
//		i2=bgbbtj_logn16h(tblk[l0+2]);
//		i3=bgbbtj_logn16h(tblk[l0+3]);
		i0=(i0<(-32768))?(-32768):((i0>32767)?32767:i0);
		i1=(i1<(-32768))?(-32768):((i1>32767)?32767:i1);
		i2=(i2<(-32768))?(-32768):((i2>32767)?32767:i2);
		i3=(i3<(-32768))?(-32768):((i3>32767)?32767:i3);
		rgba[l1+0]=i0;	rgba[l1+1]=i1;
		rgba[l1+2]=i2;	rgba[l1+3]=i3;
	}
}

void BGBBTJ_BT1D_DecodeBlockBX1A_S16(byte *block,
	short *rgba, int xstride, int ystride)
{
	float tblk[16*4];
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	BGBBTJ_BT1D_DecodeBlockBX1A(
		block, tblk, 4, 4*4);
		
	if(xstride==3)
	{
		for(k=0; k<4; k++)
			for(l=0; l<4; l++)
		{
			l0=(k*4+l)*4;
			l1=(k*ystride)+(l*3);
			i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
			i2=tblk[l0+2]*255;
			i0=(i0<(-32768))?(-32768):((i0>32767)?32767:i0);
			i1=(i1<(-32768))?(-32768):((i1>32767)?32767:i1);
			i2=(i2<(-32768))?(-32768):((i2>32767)?32767:i2);
			rgba[l1+0]=i0;	rgba[l1+1]=i1;
			rgba[l1+2]=i2;
		}
	}else
	{
		for(k=0; k<4; k++)
			for(l=0; l<4; l++)
		{
			l0=(k*4+l)*4;
			l1=(k*ystride)+(l*xstride);
			i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
			i2=tblk[l0+2]*255;	i3=tblk[l0+3]*255;
			i0=(i0<(-32768))?(-32768):((i0>32767)?32767:i0);
			i1=(i1<(-32768))?(-32768):((i1>32767)?32767:i1);
			i2=(i2<(-32768))?(-32768):((i2>32767)?32767:i2);
			i3=(i3<(-32768))?(-32768):((i3>32767)?32767:i3);
			rgba[l1+0]=i0;	rgba[l1+1]=i1;
			rgba[l1+2]=i2;	rgba[l1+3]=i3;
		}
	}
}

BGBBTJ_API void BGBBTJ_BT1D_EncodeImageBX1A(byte *block,
	float *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_EncodeBlockBX1A(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BT1D_EncodeImageBX1A_Mip(byte *block,
	float *rgba, int xs, int ys, int stride)
{
	byte *ct, *ct1;
	float *cs, *cs1;
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
		BGBBTJ_BT1D_EncodeImageBX1A(ct, cs, xs2, ys2, stride);
		xs2>>=1; ys2>>=1;
		cs=cs1; ct=ct1;
	}
}

BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1A(byte *block,
	float *rgba, int xs, int ys, int stride)
{
	int xs1, ys1;
	int i, j;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1A(
			block+(i*xs1+j)*16,
			rgba+(i*4*xs+j*4)*stride,
			stride, xs*stride);
	}
}

BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1A_RGBA32(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	float tblk[16*4];
	float bgr, bgg, bgb;
	float cr, cg, cb, ca;
	float f, g;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1A(
			block+(i*xs1+j)*16,
			tblk, 4, 4*4);
			
#if 0
		if((i^j)&4)
		{
			bgr=0.66;
			bgg=0.66;
			bgb=0.66;
		}else
		{
			bgr=0.33;
			bgg=0.33;
			bgb=0.33;
		}
#endif

		if(stride==3)
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((i*4+k)*xs+(j*4+l))*3;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;
				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				rgba[l1+0]=i0;	rgba[l1+1]=i1;
				rgba[l1+2]=i2;
			}
		}else
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((i*4+k)*xs+(j*4+l))*stride;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;	i3=tblk[l0+3]*255;

#if 0
				cr=tblk[l0+0];	cg=tblk[l0+1];
				cb=tblk[l0+2];	ca=tblk[l0+3];
				f=1.0-ca; f=1.0-(f*f);
				cr=cr*f + bgr*(1.0-f);
				cg=cg*f + bgg*(1.0-f);
				cb=cb*f + bgb*(1.0-f);
				i0=cr*255;	i1=cg*255;
				i2=cb*255;	i3=ca*255;
#endif

				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				i3=(i3<0)?0:((i3>255)?255:i3);
				rgba[l1+0]=i0;	rgba[l1+1]=i1;
				rgba[l1+2]=i2;	rgba[l1+3]=i3;
			}
		}
	}
}

BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1A_FlipRGBA32(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	float tblk[16*4];
	float bgr, bgg, bgb;
	float cr, cg, cb, ca;
	float f, g;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1A(
			block+(i*xs1+j)*16,
			tblk, 4, 4*4);

		if(stride==3)
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((ys-(i*4+k)-1)*xs+(j*4+l))*3;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;
				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				rgba[l1+0]=i0;	rgba[l1+1]=i1;
				rgba[l1+2]=i2;
			}
		}else
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((ys-(i*4+k)-1)*xs+(j*4+l))*stride;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;	i3=tblk[l0+3]*255;

				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				i3=(i3<0)?0:((i3>255)?255:i3);
				rgba[l1+0]=i0;	rgba[l1+1]=i1;
				rgba[l1+2]=i2;	rgba[l1+3]=i3;
			}
		}
	}
}

BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1A_RGBA_Q11_4(byte *block,
	s16 *rgba, int xs, int ys, int stride)
{
	float tblk[16*4];
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1A(
			block+(i*xs1+j)*16,
			tblk, 4, 4*4);
		if(stride==3)
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((i*4+k)*xs+(j*4+l))*3;
				i0=tblk[l0+0]*4080;	i1=tblk[l0+1]*4080;
				i2=tblk[l0+2]*4080;
				i0=(i0<(-32768))?(-32768):((i0>32767)?32767:i0);
				i1=(i1<(-32768))?(-32768):((i1>32767)?32767:i1);
				i2=(i2<(-32768))?(-32768):((i2>32767)?32767:i2);
				rgba[l1+0]=i0;	rgba[l1+1]=i1;
				rgba[l1+2]=i2;
			}
		}else
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((i*4+k)*xs+(j*4+l))*stride;
				i0=tblk[l0+0]*4080;	i1=tblk[l0+1]*4080;
				i2=tblk[l0+2]*4080;	i3=tblk[l0+3]*4080;
				i0=(i0<(-32768))?(-32768):((i0>32767)?32767:i0);
				i1=(i1<(-32768))?(-32768):((i1>32767)?32767:i1);
				i2=(i2<(-32768))?(-32768):((i2>32767)?32767:i2);
				i3=(i3<(-32768))?(-32768):((i3>32767)?32767:i3);
				rgba[l1+0]=i0;	rgba[l1+1]=i1;
				rgba[l1+2]=i2;	rgba[l1+3]=i3;
			}
		}
	}
}


BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1A_BGRA32(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	float tblk[16*4];
	float bgr, bgg, bgb;
	float cr, cg, cb, ca;
	float f, g;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1A(
			block+(i*xs1+j)*16,
			tblk, 4, 4*4);

		if(stride==3)
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((i*4+k)*xs+(j*4+l))*3;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;
				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				rgba[l1+0]=i2;	rgba[l1+1]=i1;
				rgba[l1+2]=i0;
			}
		}else
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((i*4+k)*xs+(j*4+l))*stride;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;	i3=tblk[l0+3]*255;

				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				i3=(i3<0)?0:((i3>255)?255:i3);
				rgba[l1+0]=i2;	rgba[l1+1]=i1;
				rgba[l1+2]=i0;	rgba[l1+3]=i3;
			}
		}
	}
}

BGBBTJ_API void BGBBTJ_BT1D_DecodeImageBX1A_FlipBGRA32(byte *block,
	byte *rgba, int xs, int ys, int stride)
{
	float tblk[16*4];
	float bgr, bgg, bgb;
	float cr, cg, cb, ca;
	float f, g;
	int xs1, ys1;
	int i, j, k, l;
	int i0, i1, i2, i3;
	int l0, l1;
	
	xs1=xs>>2; ys1=ys>>2;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
		BGBBTJ_BT1D_DecodeBlockBX1A(
			block+(i*xs1+j)*16,
			tblk, 4, 4*4);

		if(stride==3)
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((ys-(i*4+k)-1)*xs+(j*4+l))*3;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;
				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				rgba[l1+0]=i2;	rgba[l1+1]=i1;
				rgba[l1+2]=i0;
			}
		}else
		{
			for(k=0; k<4; k++)
				for(l=0; l<4; l++)
			{
				l0=(k*4+l)*4;
				l1=((ys-(i*4+k)-1)*xs+(j*4+l))*stride;
				i0=tblk[l0+0]*255;	i1=tblk[l0+1]*255;
				i2=tblk[l0+2]*255;	i3=tblk[l0+3]*255;

				i0=(i0<0)?0:((i0>255)?255:i0);
				i1=(i1<0)?0:((i1>255)?255:i1);
				i2=(i2<0)?0:((i2>255)?255:i2);
				i3=(i3<0)?0:((i3>255)?255:i3);
				rgba[l1+0]=i2;	rgba[l1+1]=i1;
				rgba[l1+2]=i0;	rgba[l1+3]=i3;
			}
		}
	}
}
