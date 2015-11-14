/*
BGB Extensions:
APP11: BGBTech Tag
	FF,APP11,xx,xx,<ASCIZ TagName>, ...
	Tag-specific data until next marker.
	
	"AlphaColor":
		AlphaColor
		RGBA as string ("red green blue alpha").
		
		Specifies a "Alpha Color" extension, where any color sufficiently
		close to the given color (determined by 'alpha'), will be assumed
		to be a transparent pixel. These values are given in decimal.

	APP11 markers may indicate component layer:
		FF,APP11,xx,xx,"CompLayer\0", <layername:ASCIZ>
			"RGB": Base RGB
			"XYZ": Normal XYZ
			"SpRGB": Specular RGB
			"DASe": Depth, Alpha, Specular-Exponent
			"LuRGB": Luma RGB
			"Alpha": Mono alpha layer

	Component Layouts:
		3 component: (no marker, RGB)
		4 component: RGB+Alpha
		7 component: RGB+Alpha+LuRGB
		8 component: RGB+XYZ+DASe
		12 component: RGB+XYZ+SpRGB+DASe
		16 component: RGB+XYZ+SpRGB+DASe+LuRGB

	The APP11 tag for a layer will be followed by the layer
	(as a JPEG image), which may be placed within LDAT tags.
	
	Images lacking DHT and DQT will reuse those from the base image.

APP12: BGBTech Tag 2
	FF,APP12,xx,xx,tt,tt,tt,tt
		tt=FOURCC Tag.

	'LDAT': Layer Data.
	Gives an image/layer spread across a span of 'LDAT' tags.
	The encoded image will be split linearly across the tags.

APP11,"TagLayer":
	Allows giving a layer name to a collection of component layers.
	All images within a tag-layer will need to be the same resolution.
	Different taglayers may have different resolutions.
	
FF B0: DVFH, Define Video Frame Header
	FF B0 xx xx
	TfTd HmMt Lid ... 

	Tf = 4-bit frame-type (0=I-Frame, 1=P-Frame, 2-15=Reserved)
	Td = 4-bit delta type (0=None, 1=Subtract I-Frame YCbCr, 2-15=Reserved)
	Hm = Huffman table for motion vectors.
	Mt = Motion Vector Type (0=Default, 1=15=Reserved)
	Lid = 8-bit layer ID.

The layer ID will specify the layer used for motion compensation.
A P-Frame will refer to the last I-Frame with the same layer-ID, which will need to have both the same resolution and image-layout.

FF B1: SOMV, Start Of Motion Vectors

	Motion Vectors:
	Huffman coded values: ZcBc
	Zc = zero count
	Bc = bit-count 
*/

// #include <pdgl.h>
#include <walpng.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <math.h>

#ifndef PDGL_API
#define PDGL_API
#endif

#define DCTSZ	8
#define DCTSZ2	64

//typedef unsigned char byte;
//typedef unsigned short ushort;
//typedef unsigned int uint;

#define JPG_SOF0	0xC0
#define JPG_SOF1	0xC1
#define JPG_SOF2	0xC2
#define JPG_SOF3	0xC3
#define JPG_DHT		0xC4
#define JPG_SOF5	0xC5
#define JPG_SOF6	0xC6
#define JPG_SOF7	0xC7
#define JPG_JPG		0xC8
#define JPG_SOF9	0xC9
#define JPG_SOF10	0xCA
#define JPG_SOF11	0xCB
#define JPG_DAC		0xCC
#define JPG_SOF13	0xCD
#define JPG_SOF14	0xCE
#define JPG_SOF15	0xCF

#define JPG_RST0	0xD0
#define JPG_RST1	0xD1
#define JPG_RST2	0xD2
#define JPG_RST3	0xD3
#define JPG_RST4	0xD4
#define JPG_RST5	0xD5
#define JPG_RST6	0xD6
#define JPG_RST7	0xD7

#define JPG_SOI		0xD8
#define JPG_EOI		0xD9
#define JPG_SOS		0xDA
#define JPG_DQT		0xDB
#define JPG_DNL		0xDC
#define JPG_DRI		0xDD
#define JPG_DHP		0xDE
#define JPG_EXP		0xDF

#define JPG_APP0	0xE0
#define JPG_APP1	0xE1
#define JPG_APP2	0xE2
#define JPG_APP3	0xE3
#define JPG_APP4	0xE4
#define JPG_APP5	0xE5
#define JPG_APP6	0xE6
#define JPG_APP7	0xE7
#define JPG_APP8	0xE8
#define JPG_APP9	0xE9
#define JPG_APP10	0xEA
#define JPG_APP11	0xEB
#define JPG_APP12	0xEC
#define JPG_APP13	0xED
#define JPG_APP14	0xEE
#define JPG_APP15	0xEF

#define JPG_JPG0	0xF0
#define JPG_JPG1	0xF1
#define JPG_JPG2	0xF2
#define JPG_JPG3	0xF3
#define JPG_JPG4	0xF4
#define JPG_JPG5	0xF5
#define JPG_JPG6	0xF6
#define JPG_JPG7	0xF7
#define JPG_JPG8	0xF8
#define JPG_JPG9	0xF9
#define JPG_JPG10	0xFA
#define JPG_JPG11	0xFB
#define JPG_JPG12	0xFC
#define JPG_JPG13	0xFD
#define JPG_COM		0xFE

static const char *pdjpg_marker[]={
"SOF0", "SOF1", "SOF2", "SOF3", "DHT", "SOF5", "SOF6", "SOF7",
"JPG", "SOF9", "SOF10", "SOF11", "DAC", "SOF13", "SOF14", "SOF15",
"RST0", "RST1", "RST2", "RST3", "RST4", "RST5", "RST6", "RST7",
"SOI", "EOI", "SOS", "DQT", "DNL", "DRI", "DHP", "EXP",
"APP0", "APP1", "APP2", "APP3", "APP4", "APP5", "APP6", "APP7", 
"APP8", "APP9", "APP10", "APP11", "APP12", "APP13", "APP14", "APP15", 
"JPG0", "JPG1", "JPG2", "JPG3", "JPG4", "JPG5", "JPG6", "JPG7", 
"JPG8", "JPG9", "JPG10", "JPG11", "JPG12", "JPG13", "COM", ""
};

static const int pdjpg_zigzag[64]={
 0,  1,  5,  6, 14, 15, 27, 28,
 2,  4,  7, 13, 16, 26, 29, 42,
 3,  8, 12, 17, 25, 30, 41, 43,
 9, 11, 18, 24, 31, 40, 44, 53,
10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60,
21, 34, 37, 47, 50, 56, 59, 61,
35, 36, 48, 49, 57, 58, 62, 63
};

static const int pdjpg_zigzag2[64]={
 0,  1,  8, 16,  9,  2,  3, 10,
17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63
};

static float pdjpg_exptab[256];
static float pdjpg_scltab[256];

#if 0
byte *pdjhuff_cs;	//current pos in bitstream (input)
byte *pdjhuff_ct;	//current pos in bitstream (output)
uint pdjhuff_win;	//bitstream window
int pdjhuff_pos;	//bitstream offset
int pdjhuff_isend;	//bitstream has broken (decoder)
#endif

#if 1
#define pdjhuff_cs		ctx->huff_cs
#define pdjhuff_ct		ctx->huff_ct
#define pdjhuff_win		ctx->huff_win
#define pdjhuff_pos		ctx->huff_pos
#define pdjhuff_isend	ctx->huff_isend
#endif


PDGL_API PDJPG_Context *PDJPG_AllocContext();
PDGL_API void PDJPG_FreeContext(PDJPG_Context *ctx);
PDGL_API void PDJPG_SetContextAlphaColor(PDJPG_Context *ctx,
	int cr, int cg, int cb, int ca);

//Common

void PDJPG_Init()
{
	static int init=0;
	int i, j;
	float f, g;

	if(init)return;
	init=1;

	for(i=0; i<64; i++)
	{
		f=(i*i)/240.0;
		g=pow(2, f);
//		printf("%d %f %f %f\n", i, f, g, 1/g);
			
		pdjpg_exptab[192+i]=f;
		pdjpg_exptab[192-i]=-f;
		pdjpg_exptab[63+i]=-f;
		pdjpg_exptab[63-i]=f;
			
		pdjpg_scltab[192+i]=g;
		pdjpg_scltab[192-i]=1.0/g;
		pdjpg_scltab[63+i]=-1.0/g;
		pdjpg_scltab[63-i]=-g;
	}

	for(i=0; i<18; i++)
	{
		j=sqrt(i*240)+0.5;
//		printf("%d %d %d %d %d %d\n", i, j,
//			192+j, 192-j, 63-j, 63+j);

		if(j>63)continue;

		g=pow(2, i);

		pdjpg_exptab[192+j]=i;
		pdjpg_exptab[192-j]=-i;
		pdjpg_exptab[63+j]=-i;
		pdjpg_exptab[63-j]=i;

		pdjpg_scltab[192+j]=g;
		pdjpg_scltab[192-j]=1.0/g;
		pdjpg_scltab[63+j]=-1.0/g;
		pdjpg_scltab[63-j]=-g;
	}
		
//	exptab[128]=0;

#if 0
	for(i=0; i<(256/2); i++)
	{
		for(j=0; j<2; j++)
		{
			sprintf(tb, "% 3d: %.4f,%.8f", i*2+j,
				exptab[i*2+j], scltab[i*2+j]);
			printf("%-32s ", tb);
		}
		printf("\n");
	}
#endif
}

#if 1
// void PDJPG_TransDCT_Horiz(byte *iblk, int *oblk)
void PDJPG_TransDCT_Horiz(short *iblk, int *oblk, int dcbias)
{
	int ib[8];

//	ib[0]=iblk[0]-128;	ib[1]=iblk[1]-128;
//	ib[2]=iblk[2]-128;	ib[3]=iblk[3]-128;
//	ib[4]=iblk[4]-128;	ib[5]=iblk[5]-128;
//	ib[6]=iblk[6]-128;	ib[7]=iblk[7]-128;

	ib[0]=iblk[0]-dcbias;	ib[1]=iblk[1]-dcbias;
	ib[2]=iblk[2]-dcbias;	ib[3]=iblk[3]-dcbias;
	ib[4]=iblk[4]-dcbias;	ib[5]=iblk[5]-dcbias;
	ib[6]=iblk[6]-dcbias;	ib[7]=iblk[7]-dcbias;

	oblk[0]=ib[0]*91  +ib[1]*91  +ib[2]*91  +ib[3]*91  
			+ib[4]*91  +ib[5]*91  +ib[6]*91  +ib[7]*91;
	oblk[1]=ib[0]*126 +ib[1]*106 +ib[2]*71  +ib[3]*25  
			-ib[4]*25  -ib[5]*71  -ib[6]*106 -ib[7]*126;
	oblk[2]=ib[0]*118 +ib[1]*49  -ib[2]*49  -ib[3]*118 
			-ib[4]*118 -ib[5]*49  +ib[6]*49  +ib[7]*118;
	oblk[3]=ib[0]*106 -ib[1]*25  -ib[2]*126 -ib[3]*71  
			+ib[4]*71  +ib[5]*126 +ib[6]*25  -ib[7]*106;
	oblk[4]=ib[0]*91  -ib[1]*91  -ib[2]*91  +ib[3]*91  
			+ib[4]*91  -ib[5]*91  -ib[6]*91  +ib[7]*91;
	oblk[5]=ib[0]*71  -ib[1]*126 +ib[2]*25  +ib[3]*106 
			-ib[4]*106 -ib[5]*25  +ib[6]*126 -ib[7]*71;
	oblk[6]=ib[0]*49  -ib[1]*118 +ib[2]*118 -ib[3]*49  
			-ib[4]*49  +ib[5]*118 -ib[6]*118 +ib[7]*49;
	oblk[7]=ib[0]*25  -ib[1]*71  +ib[2]*106 -ib[3]*126 
			+ib[4]*126 -ib[5]*106 +ib[6]*71  -ib[7]*25;
}

void PDJPG_TransDCT_Vert(int *iblk, int *oblk)
{
	oblk[ 0]=iblk[0]*91  +iblk[8]*91  
			+iblk[16]*91  +iblk[24]*91  
			+iblk[32]*91  +iblk[40]*91  
			+iblk[48]*91  +iblk[56]*91;
	oblk[ 8]=iblk[0]*126 +iblk[8]*106 
			+iblk[16]*71  +iblk[24]*25  
			-iblk[32]*25  -iblk[40]*71  
			-iblk[48]*106 -iblk[56]*126;
	oblk[16]=iblk[0]*118 +iblk[8]*49  
			-iblk[16]*49  -iblk[24]*118 
			-iblk[32]*118 -iblk[40]*49  
			+iblk[48]*49  +iblk[56]*118;
	oblk[24]=iblk[0]*106 -iblk[8]*25  
			-iblk[16]*126 -iblk[24]*71  
			+iblk[32]*71  +iblk[40]*126 
			+iblk[48]*25  -iblk[56]*106;
	oblk[32]=iblk[0]*91  -iblk[8]*91  
			-iblk[16]*91  +iblk[24]*91  
			+iblk[32]*91  -iblk[40]*91  
			-iblk[48]*91  +iblk[56]*91;
	oblk[40]=iblk[0]*71  -iblk[8]*126 
			+iblk[16]*25  +iblk[24]*106
			-iblk[32]*106 -iblk[40]*25  
			+iblk[48]*126 -iblk[56]*71;
	oblk[48]=iblk[0]*49  -iblk[8]*118
			+iblk[16]*118 -iblk[24]*49  
			-iblk[32]*49  +iblk[40]*118 
			-iblk[48]*118 +iblk[56]*49;
	oblk[56]=iblk[0]*25  -iblk[8]*71  
			+iblk[16]*106 -iblk[24]*126 
			+iblk[32]*126 -iblk[40]*106 
			+iblk[48]*71  -iblk[56]*25;
}

// void PDJPG_TransDCT(byte *iblk, short *oblk)
void PDJPG_TransDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransDCT_Horiz(iblk+0, s+0, dcbias);
	PDJPG_TransDCT_Horiz(iblk+8, s+8, dcbias);
	PDJPG_TransDCT_Horiz(iblk+16, s+16, dcbias);
	PDJPG_TransDCT_Horiz(iblk+24, s+24, dcbias);
	PDJPG_TransDCT_Horiz(iblk+32, s+32, dcbias);
	PDJPG_TransDCT_Horiz(iblk+40, s+40, dcbias);
	PDJPG_TransDCT_Horiz(iblk+48, s+48, dcbias);
	PDJPG_TransDCT_Horiz(iblk+56, s+56, dcbias);

	PDJPG_TransDCT_Vert(s+0, t+0);
	PDJPG_TransDCT_Vert(s+1, t+1);
	PDJPG_TransDCT_Vert(s+2, t+2);
	PDJPG_TransDCT_Vert(s+3, t+3);
	PDJPG_TransDCT_Vert(s+4, t+4);
	PDJPG_TransDCT_Vert(s+5, t+5);
	PDJPG_TransDCT_Vert(s+6, t+6);
	PDJPG_TransDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i]>>16;
}

#endif

#if 1
void PDJPG_TransIDCT_Horiz(short *iblk, int *oblk)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[0]* 91;	b=iblk[4]* 91;
	c=iblk[2]*118;	d=iblk[2]* 49;
	e=iblk[6]*118;	f=iblk[6]* 49;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[1]*126+iblk[3]*106+iblk[5]* 71+iblk[7]* 25;
	n=iblk[1]*106-iblk[3]* 25-iblk[5]*126-iblk[7]* 71;
	o=iblk[1]* 71-iblk[3]*126+iblk[5]* 25+iblk[7]*106;
	p=iblk[1]* 25-iblk[3]* 71+iblk[5]*106-iblk[7]*126;
	oblk[0]=i+m;	oblk[1]=j+n;	oblk[2]=k+o;	oblk[3]=l+p;
	oblk[4]=l-p;	oblk[5]=k-o;	oblk[6]=j-n;	oblk[7]=i-m;
}

void PDJPG_TransIDCT_Vert(int *iblk, int *oblk)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[ 0]* 91;	b=iblk[32]*91;
	c=iblk[16]*118;	d=iblk[16]*49;
	e=iblk[48]*118;	f=iblk[48]*49;
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[8]*126+iblk[24]*106+iblk[40]* 71+iblk[56]* 25;
	n=iblk[8]*106-iblk[24]* 25-iblk[40]*126-iblk[56]* 71;
	o=iblk[8]* 71-iblk[24]*126+iblk[40]* 25+iblk[56]*106;
	p=iblk[8]* 25-iblk[24]* 71+iblk[40]*106-iblk[56]*126;
	oblk[ 0]=i+m;	oblk[ 8]=j+n;	oblk[16]=k+o;	oblk[24]=l+p;
	oblk[32]=l-p;	oblk[40]=k-o;	oblk[48]=j-n;	oblk[56]=i-m;
}

// void PDJPG_TransIDCT(short *iblk, byte *oblk)
void PDJPG_TransIDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransIDCT_Horiz(iblk+0, s+0);
	PDJPG_TransIDCT_Horiz(iblk+8, s+8);
	PDJPG_TransIDCT_Horiz(iblk+16, s+16);
	PDJPG_TransIDCT_Horiz(iblk+24, s+24);
	PDJPG_TransIDCT_Horiz(iblk+32, s+32);
	PDJPG_TransIDCT_Horiz(iblk+40, s+40);
	PDJPG_TransIDCT_Horiz(iblk+48, s+48);
	PDJPG_TransIDCT_Horiz(iblk+56, s+56);

	PDJPG_TransIDCT_Vert(s+0, t+0);
	PDJPG_TransIDCT_Vert(s+1, t+1);
	PDJPG_TransIDCT_Vert(s+2, t+2);
	PDJPG_TransIDCT_Vert(s+3, t+3);
	PDJPG_TransIDCT_Vert(s+4, t+4);
	PDJPG_TransIDCT_Vert(s+5, t+5);
	PDJPG_TransIDCT_Vert(s+6, t+6);
	PDJPG_TransIDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
	{
		j=(t[i]>>16)+dcbias;
//		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
		oblk[i]=j;
//		oblk[i]=(j<0)?0:((j>255)?255:j);
	}
}

void PDJPG_TransIDCT16(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransIDCT_Horiz(iblk+0, s+0);
	PDJPG_TransIDCT_Horiz(iblk+8, s+8);
	PDJPG_TransIDCT_Horiz(iblk+16, s+16);
	PDJPG_TransIDCT_Horiz(iblk+24, s+24);
	PDJPG_TransIDCT_Horiz(iblk+32, s+32);
	PDJPG_TransIDCT_Horiz(iblk+40, s+40);
	PDJPG_TransIDCT_Horiz(iblk+48, s+48);
	PDJPG_TransIDCT_Horiz(iblk+56, s+56);

	for(i=0; i<DCTSZ2; i++)
		s[i]>>=4;

	PDJPG_TransIDCT_Vert(s+0, t+0);
	PDJPG_TransIDCT_Vert(s+1, t+1);
	PDJPG_TransIDCT_Vert(s+2, t+2);
	PDJPG_TransIDCT_Vert(s+3, t+3);
	PDJPG_TransIDCT_Vert(s+4, t+4);
	PDJPG_TransIDCT_Vert(s+5, t+5);
	PDJPG_TransIDCT_Vert(s+6, t+6);
	PDJPG_TransIDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
	{
		j=(t[i]>>12)+dcbias;
//		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
		oblk[i]=j;
	}
}

#endif

#define FLTOFIX14(x)	((int)((x)*8192+0.5))
#define FIX14TOI(x)	(((x)+4096)>>13)

#if 1
//RDCT: Integer Reversible DCT (Lossless)

// void PDJPG_TransRDCT_Horiz(byte *iblk, int *oblk)
void PDJPG_TransRDCT_Horiz(short *iblk, int *oblk, int dcbias)
{
	int ib[8];

	ib[0]=iblk[2]-dcbias;	ib[1]=iblk[7]-dcbias;
	ib[2]=iblk[4]-dcbias;	ib[3]=iblk[3]-dcbias;
	ib[4]=iblk[6]-dcbias;	ib[5]=iblk[0]-dcbias;
	ib[6]=iblk[1]-dcbias;	ib[7]=iblk[5]-dcbias;

	ib[7]=(-ib[7])+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));
	ib[0]=ib[0]+FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[1]=ib[1]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[2]=ib[2]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[3]=ib[3]+FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[4]=ib[4]+FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[5]=ib[5]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[6]=ib[6]+FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[7]=ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	oblk[0]=ib[3];	oblk[1]=ib[6];
	oblk[2]=ib[4];	oblk[3]=ib[2];
	oblk[4]=ib[5];	oblk[5]=ib[7];
	oblk[6]=ib[0];	oblk[7]=ib[1];
}

void PDJPG_TransRDCT_Vert(int *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[16];	ib[1]=iblk[56];
	ib[2]=iblk[32];	ib[3]=iblk[24];
	ib[4]=iblk[48];	ib[5]=iblk[ 0];
	ib[6]=iblk[ 8];	ib[7]=iblk[40];

	ib[7]=(-ib[7])+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));
	ib[0]=ib[0]+FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[1]=ib[1]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[2]=ib[2]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[3]=ib[3]+FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[4]=ib[4]+FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[5]=ib[5]+FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[6]=ib[6]+FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[7]=ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	oblk[0]=ib[3];	oblk[8]=ib[6];
	oblk[16]=ib[4];	oblk[24]=ib[2];
	oblk[32]=ib[5];	oblk[40]=ib[7];
	oblk[48]=ib[0];	oblk[56]=ib[1];
}

// void PDJPG_TransRDCT(byte *iblk, short *oblk)
void PDJPG_TransRDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransRDCT_Horiz(iblk+0, s+0, dcbias);
	PDJPG_TransRDCT_Horiz(iblk+8, s+8, dcbias);
	PDJPG_TransRDCT_Horiz(iblk+16, s+16, dcbias);
	PDJPG_TransRDCT_Horiz(iblk+24, s+24, dcbias);
	PDJPG_TransRDCT_Horiz(iblk+32, s+32, dcbias);
	PDJPG_TransRDCT_Horiz(iblk+40, s+40, dcbias);
	PDJPG_TransRDCT_Horiz(iblk+48, s+48, dcbias);
	PDJPG_TransRDCT_Horiz(iblk+56, s+56, dcbias);

	PDJPG_TransRDCT_Vert(s+0, t+0);
	PDJPG_TransRDCT_Vert(s+1, t+1);
	PDJPG_TransRDCT_Vert(s+2, t+2);
	PDJPG_TransRDCT_Vert(s+3, t+3);
	PDJPG_TransRDCT_Vert(s+4, t+4);
	PDJPG_TransRDCT_Vert(s+5, t+5);
	PDJPG_TransRDCT_Vert(s+6, t+6);
	PDJPG_TransRDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i];
}
#endif

#if 0
#undef FLTOFIX14
#undef FIX14TOI
#define FLTOFIX14(x)	((int)((x)*16384+0.5))
#define FIX14TOI(x)	(((x)+8192)>>14)
#endif

#if 1
void PDJPG_TransIRDCT_Horiz(int *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[6];	ib[1]=iblk[7];
	ib[2]=iblk[3];	ib[3]=iblk[0];
	ib[4]=iblk[2];	ib[5]=iblk[4];
	ib[6]=iblk[1];	ib[7]=iblk[5];

	ib[7]=ib[7]-FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	ib[6]=ib[6]-FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[5]=ib[5]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[4]=ib[4]-FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[3]=ib[3]-FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[2]=ib[2]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[1]=ib[1]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[0]=ib[0]-FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[7]=-ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));

	oblk[0]=ib[5];	oblk[1]=ib[6];
	oblk[2]=ib[0];	oblk[3]=ib[3];
	oblk[4]=ib[2];	oblk[5]=ib[7];
	oblk[6]=ib[4];	oblk[7]=ib[1];
}

void PDJPG_TransIRDCT_Vert(short *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[48];	ib[1]=iblk[56];
	ib[2]=iblk[24];	ib[3]=iblk[ 0];
	ib[4]=iblk[16];	ib[5]=iblk[32];
	ib[6]=iblk[ 8];	ib[7]=iblk[40];

	ib[7]=ib[7]-FIX14TOI(
		ib[0]*FLTOFIX14(1.1020)+		ib[1]*FLTOFIX14(-2.0306)+
		ib[2]*FLTOFIX14(-0.3881)+		ib[3]*FLTOFIX14(0.6561)+
		ib[4]*FLTOFIX14(1.2405)+		ib[5]*FLTOFIX14(1.6577)+
		ib[6]*FLTOFIX14(-1.1914));
	ib[6]=ib[6]-FIX14TOI(
		ib[0]*FLTOFIX14(1.0024)+		ib[1]*FLTOFIX14(-0.7180)+
		ib[2]*FLTOFIX14(-0.0928)+		ib[3]*FLTOFIX14(-0.0318)+
		ib[4]*FLTOFIX14(0.4170)+		ib[5]*FLTOFIX14(1.1665)+
		ib[7]*FLTOFIX14(0.4904));
	ib[5]=ib[5]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.6573)+		ib[1]*FLTOFIX14(0.5810)+
		ib[2]*FLTOFIX14(-0.2931)+		ib[3]*FLTOFIX14(-0.5307)+
		ib[4]*FLTOFIX14(-0.8730)+		ib[6]*FLTOFIX14(-0.1594)+
		ib[7]*FLTOFIX14(-0.3560));
	ib[4]=ib[4]-FIX14TOI(
		ib[0]*FLTOFIX14(0.4591)+		ib[1]*FLTOFIX14(0.4108)+
		ib[2]*FLTOFIX14(-0.2073)+		ib[3]*FLTOFIX14(-1.0824)+
		ib[5]*FLTOFIX14(0.7071)+		ib[6]*FLTOFIX14(0.8873)+
		ib[7]*FLTOFIX14(-0.2517));
	ib[3]=ib[3]-FIX14TOI(
		ib[0]*FLTOFIX14(0.7957)+		ib[1]*FLTOFIX14(0.9664)+
		ib[2]*FLTOFIX14(0.4439)+		ib[4]*FLTOFIX14(0.6173)+
		ib[5]*FLTOFIX14(-0.1422)+		ib[6]*FLTOFIX14(1.0378)+
		ib[7]*FLTOFIX14(-0.1700));
	ib[2]=ib[2]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0364)+		ib[1]*FLTOFIX14(-1.7104)+
		ib[3]*FLTOFIX14(-1.0000)+		ib[4]*FLTOFIX14(0.3066)+
		ib[5]*FLTOFIX14(0.6671)+		ib[6]*FLTOFIX14(-0.5953)+
		ib[7]*FLTOFIX14(0.2039));
	ib[1]=ib[1]-FIX14TOI(
		ib[0]*FLTOFIX14(-0.0685)+		ib[2]*FLTOFIX14(0.2708)+
		ib[3]*FLTOFIX14(-0.2708)+		ib[4]*FLTOFIX14(-0.2235)+
		ib[5]*FLTOFIX14(0.2568)+		ib[6]*FLTOFIX14(-0.3205)+
		ib[7]*FLTOFIX14(0.3841));
	ib[0]=ib[0]-FIX14TOI(
		ib[1]*FLTOFIX14(-1.1129)+		ib[2]*FLTOFIX14(0.0570)+
		ib[3]*FLTOFIX14(-0.4712)+		ib[4]*FLTOFIX14(0.1029)+
		ib[5]*FLTOFIX14(0.0156)+		ib[6]*FLTOFIX14(-0.4486)+
		ib[7]*FLTOFIX14(-0.4619));
	ib[7]=-ib[7]+FIX14TOI(
		ib[0]*FLTOFIX14(1.1648)+		ib[1]*FLTOFIX14(-2.8234)+
		ib[2]*FLTOFIX14(0.5375)+		ib[3]*FLTOFIX14(-0.6058)+
		ib[4]*FLTOFIX14(1.2228)+		ib[5]*FLTOFIX14(-0.3805)+
		ib[6]*FLTOFIX14(0.0288));

	oblk[ 0]=ib[5];	oblk[ 8]=ib[6];
	oblk[16]=ib[0];	oblk[24]=ib[3];
	oblk[32]=ib[2];	oblk[40]=ib[7];
	oblk[48]=ib[4];	oblk[56]=ib[1];
}

// void PDJPG_TransIRDCT(short *iblk, byte *oblk)
void PDJPG_TransIRDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransIRDCT_Vert(iblk+0, s+0);
	PDJPG_TransIRDCT_Vert(iblk+1, s+1);
	PDJPG_TransIRDCT_Vert(iblk+2, s+2);
	PDJPG_TransIRDCT_Vert(iblk+3, s+3);
	PDJPG_TransIRDCT_Vert(iblk+4, s+4);
	PDJPG_TransIRDCT_Vert(iblk+5, s+5);
	PDJPG_TransIRDCT_Vert(iblk+6, s+6);
	PDJPG_TransIRDCT_Vert(iblk+7, s+7);

	PDJPG_TransIRDCT_Horiz(s+0, t+0);
	PDJPG_TransIRDCT_Horiz(s+8, t+8);
	PDJPG_TransIRDCT_Horiz(s+16, t+16);
	PDJPG_TransIRDCT_Horiz(s+24, t+24);
	PDJPG_TransIRDCT_Horiz(s+32, t+32);
	PDJPG_TransIRDCT_Horiz(s+40, t+40);
	PDJPG_TransIRDCT_Horiz(s+48, t+48);
	PDJPG_TransIRDCT_Horiz(s+56, t+56);

	for(i=0; i<64; i++)
	{
		j=(t[i])+dcbias;
//		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
		oblk[i]=j;
	}
}
#endif


#if 0
//HAD: Hadamard Transform (Lossless)

void PDJPG_TransHAD_Horiz(byte *iblk, int *oblk)
{
	int ia[8];
	int ib[8];
	int ic[8];
	int id[8];

	ib[0]=iblk[0]-128;	ib[1]=iblk[1]-128;
	ib[2]=iblk[2]-128;	ib[3]=iblk[3]-128;
	ib[4]=iblk[4]-128;	ib[5]=iblk[5]-128;
	ib[6]=iblk[6]-128;	ib[7]=iblk[7]-128;

	ia[0]=ib[0]+ib[4];
	ia[1]=ib[1]+ib[5];
	ia[2]=ib[2]+ib[6];
	ia[3]=ib[3]+ib[7];
	ia[4]=(ia[0]>>1)-ib[4];
	ia[5]=(ia[1]>>1)-ib[5];
	ia[6]=(ia[2]>>1)-ib[6];
	ia[7]=(ia[3]>>1)-ib[7];

	ic[0]=ia[0]+ia[2];
	ic[1]=ia[1]+ia[3];
	ic[2]=(ic[0]>>1)-ia[2];
	ic[3]=(ic[1]>>1)-ia[3];
	ic[4]=ia[4]+ia[6];
	ic[5]=ia[5]+ia[7];
	ic[6]=(ic[4]>>1)-ia[6];
	ic[7]=(ic[5]>>1)-ia[7];

	id[0]=ic[0]+ic[1];
	id[1]=(id[0]>>1)-ic[1];
	id[2]=ic[2]+ic[3];
	id[3]=(id[2]>>1)-ic[3];
	id[4]=ic[4]+ic[5];
	id[5]=(id[4]>>1)-ic[5];
	id[6]=ic[6]+ic[7];
	id[7]=(id[6]>>1)-ic[7];

	oblk[0]=id[0];	oblk[1]=id[4];
	oblk[2]=id[6];	oblk[3]=id[2];
	oblk[4]=id[3];	oblk[5]=id[7];
	oblk[6]=id[5];	oblk[7]=id[1];
}

void PDJPG_TransHAD_Vert(int *iblk, int *oblk)
{
	int ib[8];

	ib[0]=iblk[16];	ib[1]=iblk[56];
	ib[2]=iblk[32];	ib[3]=iblk[24];
	ib[4]=iblk[48];	ib[5]=iblk[ 0];
	ib[6]=iblk[ 8];	ib[7]=iblk[40];

	oblk[0]=ib[3];	oblk[8]=ib[6];
	oblk[16]=ib[4];	oblk[24]=ib[2];
	oblk[32]=ib[5];	oblk[40]=ib[7];
	oblk[48]=ib[0];	oblk[56]=ib[1];
}

void PDJPG_TransHAD(byte *iblk, short *oblk)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransHAD_Horiz(iblk+0, s+0);
	PDJPG_TransHAD_Horiz(iblk+8, s+8);
	PDJPG_TransHAD_Horiz(iblk+16, s+16);
	PDJPG_TransHAD_Horiz(iblk+24, s+24);
	PDJPG_TransHAD_Horiz(iblk+32, s+32);
	PDJPG_TransHAD_Horiz(iblk+40, s+40);
	PDJPG_TransHAD_Horiz(iblk+48, s+48);
	PDJPG_TransRDCT_Horiz(iblk+56, s+56);

	PDJPG_TransRDCT_Vert(s+0, t+0);
	PDJPG_TransRDCT_Vert(s+1, t+1);
	PDJPG_TransRDCT_Vert(s+2, t+2);
	PDJPG_TransRDCT_Vert(s+3, t+3);
	PDJPG_TransRDCT_Vert(s+4, t+4);
	PDJPG_TransRDCT_Vert(s+5, t+5);
	PDJPG_TransRDCT_Vert(s+6, t+6);
	PDJPG_TransRDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i];
}
#endif


#if 1
void PDJPG_TransQDCT_Horiz(short *iblk, int *oblk, int dcbias)
{
	int ib[8];

	ib[0]=iblk[0]-dcbias;	ib[1]=iblk[1]-dcbias;
	ib[2]=iblk[2]-dcbias;	ib[3]=iblk[3]-dcbias;
	ib[4]=iblk[4]-dcbias;	ib[5]=iblk[5]-dcbias;
	ib[6]=iblk[6]-dcbias;	ib[7]=iblk[7]-dcbias;

	oblk[0]=ib[0]+ib[1]+ib[2]+ib[3]+ib[4]+ib[5]+ib[6]+ib[7];
	oblk[1]=ib[0]+ib[1]+ib[2]+ib[3]-ib[4]-ib[5]-ib[6]-ib[7];
	oblk[2]=ib[0]+ib[1]-ib[2]-ib[3]-ib[4]-ib[5]+ib[6]+ib[7];
	oblk[3]=ib[0]-ib[1]-ib[2]-ib[3]+ib[4]+ib[5]+ib[6]-ib[7];
	oblk[4]=ib[0]-ib[1]-ib[2]+ib[3]+ib[4]-ib[5]-ib[6]+ib[7];
	oblk[5]=ib[0]-ib[1]+ib[2]+ib[3]-ib[4]-ib[5]+ib[6]-ib[7];
	oblk[6]=ib[0]-ib[1]+ib[2]-ib[3]-ib[4]+ib[5]-ib[6]+ib[7];
	oblk[7]=ib[0]-ib[1]+ib[2]-ib[3]+ib[4]-ib[5]+ib[6]-ib[7];
}

void PDJPG_TransQDCT_Vert(int *ib, int *oblk)
{
	oblk[ 0]=ib[0]+ib[8]+ib[16]+ib[24]+ib[32]+ib[40]+ib[48]+ib[56];
	oblk[ 8]=ib[0]+ib[8]+ib[16]+ib[24]-ib[32]-ib[40]-ib[48]-ib[56];
	oblk[16]=ib[0]+ib[8]-ib[16]-ib[24]-ib[32]-ib[40]+ib[48]+ib[56];
	oblk[24]=ib[0]-ib[8]-ib[16]-ib[24]+ib[32]+ib[40]+ib[48]-ib[56];
	oblk[32]=ib[0]-ib[8]-ib[16]+ib[24]+ib[32]-ib[40]-ib[48]+ib[56];
	oblk[40]=ib[0]-ib[8]+ib[16]+ib[24]-ib[32]-ib[40]+ib[48]-ib[56];
	oblk[48]=ib[0]-ib[8]+ib[16]-ib[24]-ib[32]+ib[40]-ib[48]+ib[56];
	oblk[56]=ib[0]-ib[8]+ib[16]-ib[24]+ib[32]-ib[40]+ib[48]-ib[56];
}

void PDJPG_TransQDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransQDCT_Horiz(iblk+0, s+0, dcbias);
	PDJPG_TransQDCT_Horiz(iblk+8, s+8, dcbias);
	PDJPG_TransQDCT_Horiz(iblk+16, s+16, dcbias);
	PDJPG_TransQDCT_Horiz(iblk+24, s+24, dcbias);
	PDJPG_TransQDCT_Horiz(iblk+32, s+32, dcbias);
	PDJPG_TransQDCT_Horiz(iblk+40, s+40, dcbias);
	PDJPG_TransQDCT_Horiz(iblk+48, s+48, dcbias);
	PDJPG_TransQDCT_Horiz(iblk+56, s+56, dcbias);

	PDJPG_TransQDCT_Vert(s+0, t+0);
	PDJPG_TransQDCT_Vert(s+1, t+1);
	PDJPG_TransQDCT_Vert(s+2, t+2);
	PDJPG_TransQDCT_Vert(s+3, t+3);
	PDJPG_TransQDCT_Vert(s+4, t+4);
	PDJPG_TransQDCT_Vert(s+5, t+5);
	PDJPG_TransQDCT_Vert(s+6, t+6);
	PDJPG_TransQDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
		oblk[i]=t[i];
}

#endif

#if 1
void PDJPG_TransIQDCT_Horiz(short *iblk, int *oblk)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[0];
	b=iblk[2];
	c=iblk[4];
	d=iblk[6];
	i=a+b+c+d;	j=a+b-c-d;
	k=a-b-c+d;	l=a-b+c-d;
	m=iblk[1]+iblk[3]+iblk[5]+iblk[7];
	n=iblk[1]-iblk[3]-iblk[5]-iblk[7];
	o=iblk[1]-iblk[3]+iblk[5]+iblk[7];
	p=iblk[1]-iblk[3]+iblk[5]-iblk[7];
	oblk[0]=i+m;	oblk[1]=j+n;	oblk[2]=k+o;	oblk[3]=l+p;
	oblk[4]=l-p;	oblk[5]=k-o;	oblk[6]=j-n;	oblk[7]=i-m;
}

void PDJPG_TransIQDCT_Vert(int *iblk, int *oblk)
{
	int a, b, c, d, e, f;
	int i, j, k, l;
	int m, n, o, p;

	a=iblk[ 0];	b=iblk[32];
	c=iblk[16];	d=iblk[16];
	e=iblk[48];	f=iblk[48];
	i=a+c+b+f;	j=a+d-b-e;	k=a-d-b+e;	l=a-c+b-f;
	m=iblk[8]+iblk[24]+iblk[40]+iblk[56];
	n=iblk[8]-iblk[24]-iblk[40]-iblk[56];
	o=iblk[8]-iblk[24]+iblk[40]+iblk[56];
	p=iblk[8]-iblk[24]+iblk[40]-iblk[56];
	oblk[ 0]=i+m;	oblk[ 8]=j+n;	oblk[16]=k+o;	oblk[24]=l+p;
	oblk[32]=l-p;	oblk[40]=k-o;	oblk[48]=j-n;	oblk[56]=i-m;
}

void PDJPG_TransIQDCT(short *iblk, short *oblk, int dcbias)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransIQDCT_Horiz(iblk+0, s+0);
	PDJPG_TransIQDCT_Horiz(iblk+8, s+8);
	PDJPG_TransIQDCT_Horiz(iblk+16, s+16);
	PDJPG_TransIQDCT_Horiz(iblk+24, s+24);
	PDJPG_TransIQDCT_Horiz(iblk+32, s+32);
	PDJPG_TransIQDCT_Horiz(iblk+40, s+40);
	PDJPG_TransIQDCT_Horiz(iblk+48, s+48);
	PDJPG_TransIQDCT_Horiz(iblk+56, s+56);

	PDJPG_TransIQDCT_Vert(s+0, t+0);
	PDJPG_TransIQDCT_Vert(s+1, t+1);
	PDJPG_TransIQDCT_Vert(s+2, t+2);
	PDJPG_TransIQDCT_Vert(s+3, t+3);
	PDJPG_TransIQDCT_Vert(s+4, t+4);
	PDJPG_TransIQDCT_Vert(s+5, t+5);
	PDJPG_TransIQDCT_Vert(s+6, t+6);
	PDJPG_TransIQDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
	{
		j=t[i]+dcbias;
//		if(j&(~255))j=(j<0)?0:((j>255)?255:j);
		oblk[i]=j;
//		oblk[i]=(j<0)?0:((j>255)?255:j);
	}
}
#endif



//Decoder

int PDJHUFF_NextByte(PDJPG_Context *ctx)
{
	int i, j;

//	if(pdjhuff_isend)return(0xFF);
	if(pdjhuff_isend)return(0x00);

	i=*pdjhuff_cs++;
	if(i==0xFF)
	{
		j=*pdjhuff_cs++;
		if(j)
		{
//			printf("PDJHUFF_NextByte: Bitstream Marker %02X\n", j);
			pdjhuff_isend=1;
			return(0x00);
		}
	}
	return(i);
}

int PDJHUFF_InitStream(PDJPG_Context *ctx, byte *buf)
{
	pdjhuff_pos=0;
	pdjhuff_isend=0;

	pdjhuff_cs=buf;
	pdjhuff_win=PDJHUFF_NextByte(ctx);
	pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
	pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
	pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
	return(0);
}

int PDJHUFF_ReadBit(PDJPG_Context *ctx)
{
	int i;

	i=(pdjhuff_win>>(31-pdjhuff_pos))&1;
	pdjhuff_pos++;
	if(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
		pdjhuff_pos-=8;
	}
	return(i);
}

int PDJHUFF_ReadNBits(PDJPG_Context *ctx, int n)
{
	int i;

	if(n<=0)return(0);

	i=(pdjhuff_win>>(32-n-pdjhuff_pos))&((1<<n)-1);
	pdjhuff_pos+=n;
	while(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
		pdjhuff_pos-=8;
	}
	return(i);
}

void PDJHUFF_SkipNBits(PDJPG_Context *ctx, int n)
{
	pdjhuff_pos+=n;
	while(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
		pdjhuff_pos-=8;
	}
}

int PDJHUFF_PeekWord(PDJPG_Context *ctx)
{
	int i;
	i=(pdjhuff_win>>(16-pdjhuff_pos))&65535;
	return(i);
}

#if 1
int PDJHUFF_DecodeSymbol(PDJPG_Context *ctx, int tab)
{
	int i, j, k, l;

//	i=PDJHUFF_PeekWord(ctx);
//	i=(pdjhuff_win>>(16-pdjhuff_pos))&65535;
//	i=(u16)(pdjhuff_win>>(16-pdjhuff_pos));

	i=(byte)(pdjhuff_win>>(24-pdjhuff_pos));
	
	tab<<=8;
//	j=ctx->huff_idx[tab|k];
//	j=ctx->huff_idx[tab|(i>>8)];
	j=ctx->huff_idx[tab|i];

#if 1
//	l=tab|j;
//	k=ctx->huff_len[l];
	k=ctx->huff_len[tab|j];
//	if(0)
	if(k<=8)
//	if(k<8)
	{
		pdjhuff_pos+=k;
//		while(pdjhuff_pos>=8)
		if(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
			pdjhuff_pos-=8;
		}

//		PDJHUFF_SkipNBits(ctx, k);
		return(j);
	}
#endif

	i=(u16)(pdjhuff_win>>(16-pdjhuff_pos));
	j=ctx->huff_idx[tab|(i>>8)];

	while(1)
	{
		l=tab|j;
		k=ctx->huff_len[l];
		if(!k)
		{
//			printf("PDJHUFF_DecodeSymbol: Trap A\n");
			break;
		}

#if 1
		if((i>>(16-k))!=ctx->huff_code[l])
//		if((k>=8) && (i>>(16-k))!=ctx->huff_code[l])
		{
			j=ctx->huff_next[l];
//			if(!j)
			if(j<0)
			{
//				printf("PDJHUFF_DecodeSymbol: Trap B\n");
				break;
			}
			continue;
		}
#endif

		pdjhuff_pos+=k;
		while(pdjhuff_pos>=8)
//		if(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte(ctx);
			pdjhuff_pos-=8;
		}

//		PDJHUFF_SkipNBits(ctx, k);
		return(j);
	}

#if 0
	i=PDJHUFF_PeekWord(ctx);
	for(j=0; j<256; j++)
	{
		k=ctx->huff_len[tab|j];
		if(!k)continue;

		if((i>>(16-k))!=ctx->huff_code[tab|j])
			continue;
		PDJHUFF_SkipNBits(ctx, k);
		return(j);
	}
#endif

	return(-1);
}
#endif


#if 0
int PDJHUFF_DecodeSymbol(PDJPG_Context *ctx, int tab)
{
	int i, j, k, l;

	tab<<=8;

#if 0
	i=PDJHUFF_PeekWord(ctx);
	j=ctx->huff_idx[tab|(i>>8)];
	while(1)
	{
		l=tab|j;
		k=ctx->huff_len[l];
//		if(!k)break;

		if((i>>(16-k))!=ctx->huff_code[l])
		{
			j=ctx->huff_next[l];
			if(!j)break;
			continue;
		}
		PDJHUFF_SkipNBits(ctx, k);
		return(j);
	}
#endif

#if 1
	i=PDJHUFF_PeekWord(ctx);
	for(j=0; j<256; j++)
	{
		k=ctx->huff_len[tab|j];
		if(!k)continue;

		if((i>>(16-k))!=ctx->huff_code[tab|j])
			continue;
		PDJHUFF_SkipNBits(ctx, k);
		return(j);
	}
#endif

	return(-1);
}
#endif

int PDJHUFF_DecodeDiffDC(PDJPG_Context *ctx, int tab)
{
	int i, j;

	i=PDJHUFF_DecodeSymbol(ctx, tab);
	j=PDJHUFF_ReadNBits(ctx, i);

	if(!(j&(1<<(i-1))))
		j=(-1<<i)+j+1;
	return(j);
}

int PDJHUFF_DecodeBlock(PDJPG_Context *ctx,
	short *buf, int dctab, int actab,
	int ni, int nn)
{
	int i, j, k;

//	for(i=1; i<64; i++)buf[i]=0;
	memset(buf+1, 0, 63*sizeof(short));

	buf[0]+=PDJHUFF_DecodeDiffDC(ctx, dctab);
	for(i=1; i<64; i++)
	{
		j=PDJHUFF_DecodeSymbol(ctx, actab);
		if(j<=0)
		{
			if(j<0)
			{
				printf("bad dct block1 %02X(%d) %d/%d\n", j, j, ni, nn);
				return(-1);
			}
			break;
		}

		i+=(j>>4)&15;	//preceding 0's
		if(!(j&15))continue;
		if(i>=64)
		{
//			printf("bad dct block3 %02X(%d) len=%d %d/%d\n",
//				j, j, i, ni, nn);
			break;
		}

		j&=15;
		k=PDJHUFF_ReadNBits(ctx, j);
		if(!(k&(1<<(j-1))))
			k=(-1<<j)+k+1;

		j=pdjpg_zigzag2[i];
		buf[j]=k;
	}

#if 0
	for(j=i+1; j<64; j++)
	{
//		buf[i]=0;
		buf[pdjpg_zigzag2[j]]=0;
	}
#endif

	if(i>64)
	{
		printf("bad dct block2 %02X(%d) len=%d %d/%d\n", j, j, i, ni, nn);
		return(-1);
	}

	return(0);
}

void PDJHUFF_QuantBlock(
	PDJPG_Context *ctx, short *ibuf, short *obuf, int qid)
{
	int i, j;
	
//	for(i=0; i<64; i++)
//		obuf[i]=ibuf[i]/ctx->jpg_qt[qid][i];

	for(i=0; i<64; i++)
		obuf[i]=(ibuf[i]*ctx->jpg_qtfp[qid][i])>>12;
}

void PDJHUFF_DequantBlock(
	PDJPG_Context *ctx, short *ibuf, short *obuf, int qid)
{
	int i, j;
	for(i=0; i<64; i++)
		obuf[i]=ibuf[i]*ctx->jpg_qt[qid][i];
}

void PDJPG_SetupQuantTabDivFP(PDJPG_Context *ctx, int qid)
{
	int i;
	
	for(i=0; i<64; i++)
		ctx->jpg_qtfp[qid][i]=4096.0/ctx->jpg_qt[qid][i];
}

int PDJPG_MarkerSOS(PDJPG_Context *ctx, byte *buf)
{
	short dbuf[4*64];
	byte ch[4], cv[4], qid[4];
	byte step[4];
	int xi[4], yi[4], wi[4], hi[4];
	byte cdt[4], cat[4];
	int ccnt[4];

	byte *s;
	int i, j, k, l, i1, j1, k1, l1;
	int w, h, n, ns, rt;

	l=(buf[0]<<8)|buf[1];
//	printf("ns %d\n", buf[2]);

	ns=buf[2]; rt=0;
	s=buf+3;
	for(i=0; i<buf[2]; i++)
	{
//		printf("%d %d %d\n", s[0], s[1]>>4, s[1]&15);

		cdt[i]=s[1]>>4;
		cat[i]=s[1]&15;

		for(j=0; j<ctx->jpg_nc; j++)
			if(ctx->jpg_cid[j]==s[0])
		{
			ch[i]=ctx->jpg_ch[j];
			cv[i]=ctx->jpg_cv[j];
			qid[i]=ctx->jpg_qid[j];
			xi[i]=ctx->jpg_cxi[j];
			yi[i]=ctx->jpg_cyi[j];

			step[i]=ch[i]*cv[i];
			break;
		}

		w=(xi[i]+7)/8;
		h=(yi[i]+7)/8;
//		w=(xi[i]+15)/8;
//		h=(yi[i]+15)/8;
		wi[i]=w;
		hi[i]=h;

		n=(w+1)*(h+2);
		ctx->jpg_scid[i]=s[0];
		ctx->jpg_scn[i]=j;
		if(!ctx->jpg_scbuf[i])
			ctx->jpg_scbuf[i]=malloc(n*64*sizeof(short));
		if(!ctx->jpg_sibuf[i])
			ctx->jpg_sibuf[i]=malloc(n*64*sizeof(short));

//		memset(ctx->jpg_sibuf[i], 0, n*64);

		ccnt[i]=0;

		s+=2;
	}
	s+=3;

	i=(ctx->xs+ctx->jpg_chm*8-1)/(ctx->jpg_chm*8);
	j=(ctx->ys+ctx->jpg_chn*8-1)/(ctx->jpg_chn*8);
	n=i*j;

	PDJHUFF_InitStream(ctx, s);

	for(i=0; i<(4*64); i++)dbuf[i]=0;

	rt=0;
	for(i=0; i<n; i++)
	{
		for(j=0; j<ns; j++)
		{
			for(k=0; k<cv[j]; k++)
			{
				for(l=0; l<ch[j]; l++)
				{
					if(rt<0)break;

					i1=(i/(wi[j]/ch[j]))*ch[j];
					j1=(i%(wi[j]/ch[j]))*ch[j];
					k1=((i1+k)*wi[j])+(j1+l);

					rt=PDJHUFF_DecodeBlock(ctx, dbuf+j*64,
						cdt[j]*2+0, cat[j]*2+1, i, n);
					if(rt<0)break;
					PDJHUFF_DequantBlock(ctx, dbuf+j*64,
						ctx->jpg_scbuf[j]+k1*64, qid[j]);
						
					if(ctx->jpg_rdct)
					{
						PDJPG_TransIRDCT(ctx->jpg_scbuf[j]+k1*64,
							ctx->jpg_sibuf[j]+k1*64, 128);
					}else
					{
						PDJPG_TransIDCT(ctx->jpg_scbuf[j]+k1*64,
							ctx->jpg_sibuf[j]+k1*64, 128);

//						PDJPG_TransIQDCT(ctx->jpg_scbuf[j]+k1*64,
//							ctx->jpg_sibuf[j]+k1*64, 128);
					}
				}
				if(rt<0)break;
			}
			if(rt<0)break;
		}
		if(rt<0)break;
	}

	if(rt<0)
	{
		printf("PDJPG_MarkerSOS: Bad Image\n");
		return(-1);
	}

	i=ctx->huff_cs-buf;
	return(i);

//	return(0);
}

int PDJPG_MarkerSOF0(PDJPG_Context *ctx, byte *buf)
{
	byte *cnt, *s;
	int i, j, k, l, m, n;

	l=(buf[0]<<8)|buf[1];
//	printf("p %d\n", buf[2]);

	i=(buf[3]<<8)|buf[4];
	j=(buf[5]<<8)|buf[6];

	ctx->xs=j;
	ctx->ys=i;
//	printf("size %dx%d\n", j, i);

	m=0; n=0;

	k=buf[7];
	s=buf+8;
	for(i=0; i<k; i++)
	{
		ctx->jpg_cid[i]=s[0];
		ctx->jpg_ch[i]=s[1]>>4;
		ctx->jpg_cv[i]=s[1]&15;
		ctx->jpg_qid[i]=s[2];

		if(ctx->jpg_ch[i]>m)m=ctx->jpg_ch[i];
		if(ctx->jpg_cv[i]>n)n=ctx->jpg_cv[i];

//		printf("CID %d: %d %d %d\n", ctx->jpg_cid[i],
//			ctx->jpg_ch[i], ctx->jpg_cv[i], ctx->jpg_qid[i]);
		s+=3;
	}

	ctx->jpg_chm=m;
	ctx->jpg_chn=n;

	for(i=0; i<k; i++)
	{
		ctx->jpg_cxi[i]=(ctx->jpg_ch[i]*ctx->xs)/m;
		ctx->jpg_cyi[i]=(ctx->jpg_cv[i]*ctx->ys)/n;
	}

	ctx->jpg_nc=k;

	return(0);
}

int PDJPG_MarkerDQT(PDJPG_Context *ctx, byte *buf)
{
	byte *s, *se;
	int i, j, l;

	l=(buf[0]<<8)|buf[1];

	s=buf+2;
	se=buf+l;

	while(s<se)
	{
		i=(s[0]>>4)&15;
		if(i)
		{
//			printf("bad bits %d\n", i);
			return(-1);
		}
		i=s[0]&15;
//		printf("dest %d\n", i);

		for(j=0; j<64; j++)
			ctx->jpg_qt[i][pdjpg_zigzag2[j]]=s[j+1];
		s+=65;
	}
	
	return(s-buf);
//	return(0);
}

int PDJPG_MarkerDHT(PDJPG_Context *ctx, byte *buf)
{
	byte *cnt, *s;
	int i, j, k, l, tn;

	l=(buf[0]<<8)|buf[1];

	j=(buf[2]>>4)&15;
	i=buf[2]&15;
//	printf("dest %d, type %d\n", i, j);

	tn=i*2+j;

	for(i=0; i<256; i++)
	{
		ctx->huff_len[tn*256+i]=0;
		ctx->huff_next[tn*256+i]=-1;
		ctx->huff_idx[tn*256+i]=-1;
	}

	cnt=buf+3;
	s=cnt+16;
	k=0; l=0;
	for(i=0; i<16; i++)
	{
		k<<=1;
		for(j=0; j<cnt[i]; j++)
		{
			ctx->huff_code[tn*256+(*s)]=k++;
			ctx->huff_len[tn*256+(*s)]=i+1;
			ctx->huff_mask[tn*256+(*s)]=(1<<(i+1))-1;
			l=i+1;
			s++;
		}
	}
//	printf("%04X %d\n", k, l);

	//build lookup indices / chains

	for(i=0; i<256; i++)
	{
//		ctx->huff_next[tn*256+i]=0;
//		ctx->huff_idx[tn*256+i]=0;
		ctx->huff_next[tn*256+i]=-1;
		ctx->huff_idx[tn*256+i]=-1;
	}

	for(i=0; i<256; i++)
	{
		l=ctx->huff_len[tn*256+i];
		if(!l)continue;
		j=ctx->huff_code[tn*256+i];
		
		if(l<=8)
		{
			j=j<<(8-l);
			k=1<<(8-l);
			while((k--) && (j<256))
			{
				ctx->huff_idx[tn*256+j]=i;
				j++;
			}
		}else
		{
			j=j>>(l-8);
			ctx->huff_next[tn*256+i]=
				ctx->huff_idx[tn*256+j];
			ctx->huff_idx[tn*256+j]=i;
		}
	}

	return(s-buf);
//	return(0);
}

int PDJPG_MarkerAPP0(PDJPG_Context *ctx, byte *buf)
{
	return(0);
}

int PDJPG_MarkerAPP9(PDJPG_Context *ctx, byte *buf)
{
	int i;
	buf+=2;

	if(!memcmp(buf, "JPSERM", 6))
	{
		ctx->jpg_rdct=1;
		return(1);
	}
	
	return(0);
}

int PDJPG_MarkerAPP11(PDJPG_Context *ctx, byte *buf)
{
	int cr, cg, cb, ca;

	buf+=2;

	if(!strcmp((char *)buf, "AlphaColor"))
	{
		buf+=strlen((char *)buf)+1;
		sscanf(buf, "%d %d %d %d", &cr, &cg, &cb, &ca);
		PDJPG_SetContextAlphaColor(ctx, cr, cg, cb, ca);
		return(1);
	}
	
	return(0);
}

int PDJPG_MarkerAPP12(PDJPG_Context *ctx, byte *buf)
{
	int i;
	buf+=2;

	i=(buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|buf[3];
//	printf("PDJPG_MarkerAPP12: %08X\n", i);

	if(!memcmp(buf, "BCST", 4))
	{
		ctx->bcst_version=(buf[4]<<8)|buf[5];
		ctx->bcst_trans=buf[6];
		ctx->bcst_bpp=buf[7];
		ctx->bcst_flags=
			(buf[8]<<24)|(buf[9]<<16)|
			(buf[10]<<8)|buf[11];
		
		ctx->jpg_clrtrans=ctx->bcst_trans;
		
		if(ctx->bcst_flags&PDJPG_BCSFL_RDCT)
			ctx->jpg_rdct=1;
			
//		printf("PDJPG_MarkerAPP12: BCST %04X %02X %02X %08X\n",
//			ctx->bcst_version, ctx->bcst_trans,
//			ctx->bcst_bpp, ctx->bcst_flags);
		return(1);
	}

	return(0);
}

int PDJPG_MarkerAPP14(PDJPG_Context *ctx, byte *buf)
{
	buf+=2;

#if 0
	if(!memcmp((char *)buf, "Adobe", 5))
	{
		ctx->jpg_app14_dctversion=(buf[5]<<8)|buf[6];
		ctx->jpg_app14_flags0=(buf[7]<<8)|buf[8];
		ctx->jpg_app14_flags1=(buf[9]<<8)|buf[10];
		ctx->jpg_app14_trans=buf[11];
		
		switch(ctx->jpg_app14_trans)
		{
		case 0: ctx->jpg_clrtrans=PDJPG_CLRS_RGB; break;
		case 1: ctx->jpg_clrtrans=PDJPG_CLRS_YCBCR; break;
		case 2: ctx->jpg_clrtrans=PDJPG_CLRS_YCCK; break;
		}
		
		return(1);
	}
#endif

	return(0);
}

int PDJPG_MarkerAPPN(PDJPG_Context *ctx, byte *buf)
{
	return(0);
}

#if 0
int PDJPG_GetComponentPixel(PDJPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

//	id=ctx->jpg_scn[cn];

	i=x*ctx->jpg_schsc[cn];
	j=y*ctx->jpg_scvsc[cn];
//	i=(ctx->jpg_ch[id]*x)/ctx->jpg_chm;
//	j=(ctx->jpg_cv[id]*y)/ctx->jpg_chn;
	k=i>>3;
	l=j>>3;

	w=ctx->jpg_schs[cn];
	h=ctx->jpg_scvs[cn];
//	w=(ctx->jpg_cxi[id]+7)/8;
//	h=(ctx->jpg_cyi[id]+7)/8;
//	w=(ctx->jpg_cxi[id]+15)/8;
//	h=(ctx->jpg_cyi[id]+15)/8;
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}
#endif

#if 1
int PDJPG_GetComponentPixel(PDJPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

//	i=x*ctx->jpg_schsc[cn];
//	j=y*ctx->jpg_scvsc[cn];

	i=(x*ctx->jpg_schsci[cn])>>16;
	j=(y*ctx->jpg_scvsci[cn])>>16;

	k=i>>3;
	l=j>>3;

	w=ctx->jpg_schs[cn];
//	h=ctx->jpg_scvs[cn];
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}
#endif

#if 1
int PDJPG_GetComponentPixelS1(PDJPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

	i=x>>3;
	j=y>>3;
	w=ctx->jpg_schs[cn];
//	h=ctx->jpg_scvs[cn];
	n=(j*w)+i;

	k=x&7;
	l=y&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}

int PDJPG_GetComponentPixelS2(PDJPG_Context *ctx, int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

	i=x>>1;
	j=y>>1;

	k=i>>3;
	l=j>>3;
	w=ctx->jpg_schs[cn];
//	h=ctx->jpg_scvs[cn];
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=ctx->jpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}

int PDJPG_GetComponentPixelS122(PDJPG_Context *ctx, int x, int y,
	int *ry, int *ru, int *rv)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	x1=x>>1; y1=y>>1;

	w0=ctx->jpg_schs[0];
	w1=ctx->jpg_schs[1];

	i=x>>3; j=y>>3;
	k=x1>>3; l=y1>>3;
	n0=(j*w0)+i;
	n1=(l*w1)+k;

	i=x&7; j=y&7;
	k=x1&7; l=y1&7;
	t0=(n0<<6)+((j<<3)+i);
	t1=(n1<<6)+((l<<3)+k);

	*ry=ctx->jpg_sibuf[0][t0];
	*ru=ctx->jpg_sibuf[1][t1];
	*rv=ctx->jpg_sibuf[2][t1];

	return(0);
}

int PDJPG_GetComponentPixelS111(PDJPG_Context *ctx, int x, int y,
	int *ry, int *ru, int *rv)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	w0=ctx->jpg_schs[0];
	i=x>>3; j=y>>3;
	n0=(j*w0)+i;

	i=x&7; j=y&7;
	t0=(n0<<6)+((j<<3)+i);

	*ry=ctx->jpg_sibuf[0][t0];
	*ru=ctx->jpg_sibuf[1][t0];
	*rv=ctx->jpg_sibuf[2][t0];

	return(0);
}

int PDJPG_GetComponentPixelS420(PDJPG_Context *ctx, int x, int y,
	int *ry0, int *ry1, int *ry2, int *ry3,
	int *ru, int *rv)
{
	int i, j, k, l;
	int t0, t1, w0, w1, n0, n1, x1, y1;

	x1=x>>1; y1=y>>1;

	w0=ctx->jpg_schs[0];
	w1=ctx->jpg_schs[1];

	i=x>>3; j=y>>3;
	k=x1>>3; l=y1>>3;
	n0=(j*w0)+i;
	n1=(l*w1)+k;

	i=x&7; j=y&7;
	k=x1&7; l=y1&7;
	t0=(n0<<6)+((j<<3)+i);
	t1=(n1<<6)+((l<<3)+k);

	*ry0=ctx->jpg_sibuf[0][t0];
	*ry1=ctx->jpg_sibuf[0][t0+1];
	*ry2=ctx->jpg_sibuf[0][t0+8];
	*ry3=ctx->jpg_sibuf[0][t0+9];

	*ru=ctx->jpg_sibuf[1][t1];
	*rv=ctx->jpg_sibuf[2][t1];

	return(0);
}

#endif

int PDJPG_DecodeColorTransformYCbCr420(PDJPG_Context *ctx, byte *obuf)
{
	int y, u, v, r, g, b, a, u1, v1;
	int y0, y1, y2, y3;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	int r2, g2, b2, a2;
	int r3, g3, b3, a3;
	int l0, l1, l2, l3;
	int i, j, k, l;

#if 1
		for(i=0; i<ctx->ys; i+=2)
			for(j=0; j<ctx->xs; j+=2)
		{
//			PDJPG_GetComponentPixelS122(ctx, j, i, &y0, &u, &v);
//			y1=PDJPG_GetComponentPixelS1(ctx, 0, j+1, i+0);
//			y2=PDJPG_GetComponentPixelS1(ctx, 0, j+0, i+1);
//			y3=PDJPG_GetComponentPixelS1(ctx, 0, j+1, i+1);

			PDJPG_GetComponentPixelS420(ctx, j, i,
				&y0, &y1, &y2, &y3, &u, &v);

			if(ctx->alphaClr)
			{
				a=255;

				k=y0-ctx->alphaClrY; l=k*k;
				k=y1-ctx->alphaClrY; k=k*k; if(k<l)l=k;
				k=y2-ctx->alphaClrY; k=k*k; if(k<l)l=k;
				k=y3-ctx->alphaClrY; k=k*k; if(k<l)l=k;
				
//				k=y0-ctx->alphaClrY; l=k*k;
				k=u-ctx->alphaClrU; l+=k*k;
				k=v-ctx->alphaClrV; l+=k*k;
				
				a=ctx->alphaClrA;
				a=(l<=(a*a))?0:255;
			}else { a=255; }

			u1=u-128; v1=v-128;
			l0=91881*v1;
			l1=-22554*u1-46802*v1;
			l2=116130*u1;

			y0<<=16; y1<<=16; y2<<=16; y3<<=16;
			r0=y0+l0; g0=y0+l1; b0=y0+l2;
			r1=y1+l0; g1=y1+l1; b1=y1+l2;
			r2=y2+l0; g2=y2+l1; b2=y2+l2;
			r3=y3+l0; g3=y3+l1; b3=y3+l2;

//			r0=65536*y0+l0; g0=65536*y0+l1; b0=65536*y0+l2;
//			r1=65536*y1+l0; g1=65536*y1+l1; b1=65536*y1+l2;
//			r2=65536*y2+l0; g2=65536*y2+l1; b2=65536*y2+l2;
//			r3=65536*y3+l0; g3=65536*y3+l1; b3=65536*y3+l2;

			r0>>=16; g0>>=16; b0>>=16;
			r1>>=16; g1>>=16; b1>>=16;
			r2>>=16; g2>>=16; b2>>=16;
			r3>>=16; g3>>=16; b3>>=16;

#if 0
			if((r0|r1|r2|r3)&(~255))
			{
				r0=(r0<0)?0:((r0>255)?255:r0);
				r1=(r1<0)?0:((r1>255)?255:r1);
				r2=(r2<0)?0:((r2>255)?255:r2);
				r3=(r3<0)?0:((r3>255)?255:r3);
			}

			if((g0|g1|g2|g3)&(~255))
			{
				g0=(g0<0)?0:((g0>255)?255:g0);
				g1=(g1<0)?0:((g1>255)?255:g1);
				g2=(g2<0)?0:((g2>255)?255:g2);
				g3=(g3<0)?0:((g3>255)?255:g3);
			}

			if((b0|b1|b2|b3)&(~255))
			{
				b0=(b0<0)?0:((b0>255)?255:b0);
				b1=(b1<0)?0:((b1>255)?255:b1);
				b2=(b2<0)?0:((b2>255)?255:b2);
				b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if 1
			if((r0|r1|r2|r3)&(~255))
			{
				if(r0&(~255))r0=(r0<0)?0:((r0>255)?255:r0);
				if(r1&(~255))r1=(r1<0)?0:((r1>255)?255:r1);
				if(r2&(~255))r2=(r2<0)?0:((r2>255)?255:r2);
				if(r3&(~255))r3=(r3<0)?0:((r3>255)?255:r3);
			}

			if((g0|g1|g2|g3)&(~255))
			{
				if(g0&(~255))g0=(g0<0)?0:((g0>255)?255:g0);
				if(g1&(~255))g1=(g1<0)?0:((g1>255)?255:g1);
				if(g2&(~255))g2=(g2<0)?0:((g2>255)?255:g2);
				if(g3&(~255))g3=(g3<0)?0:((g3>255)?255:g3);
			}

			if((b0|b1|b2|b3)&(~255))
			{
				if(b0&(~255))b0=(b0<0)?0:((b0>255)?255:b0);
				if(b1&(~255))b1=(b1<0)?0:((b1>255)?255:b1);
				if(b2&(~255))b2=(b2<0)?0:((b2>255)?255:b2);
				if(b3&(~255))b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if 0
			k=r0|r1|r2|r3 | g0|g1|g2|g3 | b0|b1|b2|b3;
			if(k&(~255))
			{
				if(r0&(~255))r0=(r0<0)?0:((r0>255)?255:r0);
				if(g0&(~255))g0=(g0<0)?0:((g0>255)?255:g0);
				if(b0&(~255))b0=(b0<0)?0:((b0>255)?255:b0);
				if(r1&(~255))r1=(r1<0)?0:((r1>255)?255:r1);
				if(g1&(~255))g1=(g1<0)?0:((g1>255)?255:g1);
				if(b1&(~255))b1=(b1<0)?0:((b1>255)?255:b1);
				if(r2&(~255))r2=(r2<0)?0:((r2>255)?255:r2);
				if(g2&(~255))g2=(g2<0)?0:((g2>255)?255:g2);
				if(b2&(~255))b2=(b2<0)?0:((b2>255)?255:b2);
				if(r3&(~255))r3=(r3<0)?0:((r3>255)?255:r3);
				if(g3&(~255))g3=(g3<0)?0:((g3>255)?255:g3);
				if(b3&(~255))b3=(b3<0)?0:((b3>255)?255:b3);
			}
#endif

#if 0
			r0=(!(r0&(~255)))?r0:((r0<0)?0:((r0>255)?255:r0));
			g0=(!(g0&(~255)))?g0:((g0<0)?0:((g0>255)?255:g0));
			b0=(!(b0&(~255)))?b0:((b0<0)?0:((b0>255)?255:b0));
			r1=(!(r1&(~255)))?r1:((r1<0)?0:((r1>255)?255:r1));
			g1=(!(g1&(~255)))?g1:((g1<0)?0:((g1>255)?255:g1));
			b1=(!(b1&(~255)))?b1:((b1<0)?0:((b1>255)?255:b1));
			r2=(!(r2&(~255)))?r2:((r2<0)?0:((r2>255)?255:r2));
			g2=(!(g2&(~255)))?g2:((g2<0)?0:((g2>255)?255:g2));
			b2=(!(b2&(~255)))?b2:((b2<0)?0:((b2>255)?255:b2));
			r3=(!(r3&(~255)))?r3:((r3<0)?0:((r3>255)?255:r3));
			g3=(!(g3&(~255)))?g3:((g3<0)?0:((g3>255)?255:g3));
			b3=(!(b3&(~255)))?b3:((b3<0)?0:((b3>255)?255:b3));
#endif

#if 0
			r0=(r0<0)?0:((r0>255)?255:r0);
			g0=(g0<0)?0:((g0>255)?255:g0);
			b0=(b0<0)?0:((b0>255)?255:b0);
			r1=(r1<0)?0:((r1>255)?255:r1);
			g1=(g1<0)?0:((g1>255)?255:g1);
			b1=(b1<0)?0:((b1>255)?255:b1);
			r2=(r2<0)?0:((r2>255)?255:r2);
			g2=(g2<0)?0:((g2>255)?255:g2);
			b2=(b2<0)?0:((b2>255)?255:b2);
			r3=(r3<0)?0:((r3>255)?255:r3);
			g3=(g3<0)?0:((g3>255)?255:g3);
			b3=(b3<0)?0:((b3>255)?255:b3);
#endif

			k=ctx->ys-1-i;
			l0=((k*ctx->xs)+j+0)*4;
//			l1=((k*ctx->xs)+j+1)*4;
			k=ctx->ys-2-i;
			l2=((k*ctx->xs)+j+0)*4;
//			l3=((k*ctx->xs)+j+1)*4;

//			ct=obuf+l0; *ct++=r0; *ct++=g0; *ct++=b0; *ct++=a;
//			ct=obuf+l1; *ct++=r1; *ct++=g1; *ct++=b1; *ct++=a;
//			ct=obuf+l2; *ct++=r2; *ct++=g2; *ct++=b2; *ct++=a;
//			ct=obuf+l3; *ct++=r3; *ct++=g3; *ct++=b3; *ct++=a;

			obuf[l0+0]=r0; obuf[l0+1]=g0; obuf[l0+2]=b0; obuf[l0+3]=a;
			obuf[l0+4]=r1; obuf[l0+5]=g1; obuf[l0+6]=b1; obuf[l0+7]=a;
			obuf[l2+0]=r2; obuf[l2+1]=g2; obuf[l2+2]=b2; obuf[l2+3]=a;
			obuf[l2+4]=r3; obuf[l2+5]=g3; obuf[l2+6]=b3; obuf[l2+7]=a;

//			obuf[l0+0]=r0; obuf[l0+1]=g0; obuf[l0+2]=b0; obuf[l0+3]=a;
//			obuf[l1+0]=r1; obuf[l1+1]=g1; obuf[l1+2]=b1; obuf[l1+3]=a;
//			obuf[l2+0]=r2; obuf[l2+1]=g2; obuf[l2+2]=b2; obuf[l2+3]=a;
//			obuf[l3+0]=r3; obuf[l3+1]=g3; obuf[l3+2]=b3; obuf[l3+3]=a;
		}
#endif

#if 0
		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			PDJPG_GetComponentPixelS122(ctx, j, i, &y, &u, &v);

			if(ctx->alphaClr)
			{
				k=y-ctx->alphaClrY; l=k*k;
				k=u-ctx->alphaClrU; l+=k*k;
				k=v-ctx->alphaClrV; l+=k*k;
				
				a=ctx->alphaClrA;
				a=(l<=(a*a))?0:255;
			}else { a=255; }

			u1=u-128; v1=v-128;
			r=65536*y          +91881*v1;
			g=65536*y- 22554*u1-46802*v1;
			b=65536*y+116130*u1;
			r>>=16; g>>=16; b>>=16;

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			obuf[l+0]=r;
			obuf[l+1]=g;
			obuf[l+2]=b;
			obuf[l+3]=a;
		}
#endif
}

/** Simulate basic decoding, but doesn't actually decode image.
  * Can be used to retrieve resolution or initialize Huffman tables
  * at a lower cost than a full decode. */
PDGL_API int PDJPG_DummyDecodeCtx(PDJPG_Context *ctx,
	byte *buf, int sz, int *xs, int *ys)
{
	byte *obuf, *ct;
	int y, u, v, r, g, b, a, u1, v1;
	int y0, y1, y2, y3;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	int r2, g2, b2, a2;
	int r3, g3, b3, a3;
	int l0, l1, l2, l3;
	int i, j, k, l;

	if((buf[0]!=0xFF) || (buf[1]!=JPG_SOI))
		return(-1);

	ctx->jpg_is420=0;
	ctx->jpg_rdct=0;
	ctx->jpg_clrtrans=0;

	ctx->bcst_version=0;
	ctx->bcst_flags=0;
	ctx->bcst_trans=0;
	ctx->bcst_bpp=0;

	for(i=0; i<4; i++)
	{
		ctx->jpg_scbuf[i]=NULL;
		ctx->jpg_sibuf[i]=NULL;
	}

	for(i=0; i<sz; i++)
	{
//		if((buf[i]==0xFF) && (buf[i+1]!=0x00) && (buf[i+1]!=0xFF))
		if(buf[i]==0xFF)
		{
			if(buf[i+1]==0x00)
				{ i++; continue; }
			if(buf[i+1]==0xFF)
				{ i++; continue; }

			j=buf[i+1]-0xC0;
			if(j<0)j=63;

//			printf("@ %d %s(%02X)\n", i,
//				pdjpg_marker[j], buf[i+1]);

			if(buf[i+1]==JPG_DQT)PDJPG_MarkerDQT(ctx, buf+i+2);
			if(buf[i+1]==JPG_DHT)PDJPG_MarkerDHT(ctx, buf+i+2);
			if(buf[i+1]==JPG_SOF0)PDJPG_MarkerSOF0(ctx, buf+i+2);
			if(buf[i+1]==JPG_SOS)
			{
//				j=PDJPG_MarkerSOS(ctx, buf+i+2);
//				if(j>0) { i+=j; }
				continue;
			}

			if((buf[i+1]>=JPG_APP0) && (buf[i+1]<=JPG_APP15))
			{
				if(buf[i+1]==JPG_APP0)PDJPG_MarkerAPP0(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP9)PDJPG_MarkerAPP9(ctx, buf+i+2);
				
				if(buf[i+1]==JPG_APP11)PDJPG_MarkerAPP11(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP12)PDJPG_MarkerAPP12(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP14)PDJPG_MarkerAPP14(ctx, buf+i+2);

				//skip over marker
				j=(buf[i+2]<<8)|buf[i+3];
				i+=j;
				continue;
			}

			if(buf[i+1]==JPG_EOI)break;
		}
	}

	*xs=ctx->xs;
	*ys=ctx->ys;

	return(0);
}


PDGL_API byte *PDJPG_DecodeCtx(PDJPG_Context *ctx,
	byte *buf, int sz, int *xs, int *ys)
{
	byte *obuf, *ct;
	int y, u, v, r, g, b, a, u1, v1;
	int y0, y1, y2, y3;
	int r0, g0, b0, a0;
	int r1, g1, b1, a1;
	int r2, g2, b2, a2;
	int r3, g3, b3, a3;
	int l0, l1, l2, l3;
	int i, j, k, l;

	if((buf[0]!=0xFF) || (buf[1]!=JPG_SOI))
		return(NULL);

	ctx->jpg_is420=0;
	ctx->jpg_rdct=0;
	ctx->jpg_clrtrans=0;

	ctx->bcst_version=0;
	ctx->bcst_flags=0;
	ctx->bcst_trans=0;
	ctx->bcst_bpp=0;

	for(i=0; i<4; i++)
	{
		ctx->jpg_scbuf[i]=NULL;
		ctx->jpg_sibuf[i]=NULL;
	}

	for(i=0; i<sz; i++)
	{
//		if((buf[i]==0xFF) && (buf[i+1]!=0x00) && (buf[i+1]!=0xFF))
		if(buf[i]==0xFF)
		{
			if(buf[i+1]==0x00)
				{ i++; continue; }
			if(buf[i+1]==0xFF)
				{ i++; continue; }

			j=buf[i+1]-0xC0;
			if(j<0)j=63;

//			printf("@ %d %s(%02X)\n", i,
//				pdjpg_marker[j], buf[i+1]);

			if(buf[i+1]==JPG_DQT)PDJPG_MarkerDQT(ctx, buf+i+2);
			if(buf[i+1]==JPG_DHT)PDJPG_MarkerDHT(ctx, buf+i+2);
			if(buf[i+1]==JPG_SOF0)PDJPG_MarkerSOF0(ctx, buf+i+2);
			if(buf[i+1]==JPG_SOS)
			{
				j=PDJPG_MarkerSOS(ctx, buf+i+2);
				if(j>0) { i+=j; }
				continue;
			}

			if((buf[i+1]>=JPG_APP0) && (buf[i+1]<=JPG_APP15))
			{
				if(buf[i+1]==JPG_APP0)PDJPG_MarkerAPP0(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP11)PDJPG_MarkerAPP11(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP12)PDJPG_MarkerAPP12(ctx, buf+i+2);
				if(buf[i+1]==JPG_APP14)PDJPG_MarkerAPP14(ctx, buf+i+2);

				//skip over marker
				j=(buf[i+2]<<8)|buf[i+3];
				i+=j;
				continue;
			}

			if(buf[i+1]==JPG_EOI)break;
		}
	}

	obuf=ctx->jpg_imgbuf;
	if(!obuf || (ctx->xs!=ctx->lxs) || (ctx->ys!=ctx->lys))
	{
		if(ctx->jpg_imgbuf)
			free(ctx->jpg_imgbuf);
		
		obuf=malloc((ctx->xs+1)*(ctx->ys+1)*4);
		memset(obuf, 0xFF, ctx->xs*ctx->ys*4);
		
		ctx->jpg_imgbuf=obuf;
		ctx->lxs=ctx->xs;
		ctx->lys=ctx->ys;
	}

	for(i=0; i<ctx->jpg_nc; i++)
	{
		j=ctx->jpg_scn[i];
//		ctx->jpg_schsc[i]=ctx->jpg_ch[j]/(ctx->jpg_chm*1.0);
//		ctx->jpg_scvsc[i]=ctx->jpg_cv[j]/(ctx->jpg_chn*1.0);

		ctx->jpg_schsci[i]=(65536*ctx->jpg_ch[j])/ctx->jpg_chm;
		ctx->jpg_scvsci[i]=(65536*ctx->jpg_cv[j])/ctx->jpg_chn;

		ctx->jpg_schs[i]=(ctx->jpg_cxi[j]+7)/8;
		ctx->jpg_scvs[i]=(ctx->jpg_cyi[j]+7)/8;
		
	}

	if(ctx->jpg_nc==3)
	{
		i=1;
		if(ctx->jpg_schsci[0]!=65536)i=0;
		if(ctx->jpg_scvsci[0]!=65536)i=0;
		if(ctx->jpg_schsci[1]!=32768)i=0;
		if(ctx->jpg_scvsci[1]!=32768)i=0;
		if(ctx->jpg_schsci[2]!=32768)i=0;
		if(ctx->jpg_scvsci[2]!=32768)i=0;
		ctx->jpg_is420=i;

		i=1;
		if(ctx->jpg_schsci[0]!=65536)i=0;
		if(ctx->jpg_scvsci[0]!=65536)i=0;
		if(ctx->jpg_schsci[1]!=65536)i=0;
		if(ctx->jpg_scvsci[1]!=65536)i=0;
		if(ctx->jpg_schsci[2]!=65536)i=0;
		if(ctx->jpg_scvsci[2]!=65536)i=0;
		ctx->jpg_is444=i;
	}else
	{
		ctx->jpg_is420=0;
		ctx->jpg_is444=0;
	}

	if(ctx->jpg_is420 && !((ctx->xs|ctx->ys)&1) &&
		(ctx->jpg_clrtrans==PDJPG_CLRS_YCBCR))
	{
		PDJPG_DecodeColorTransformYCbCr420(ctx, obuf);
	}else
	{
		for(i=0; i<ctx->ys; i++)
			for(j=0; j<ctx->xs; j++)
		{
			if(ctx->jpg_nc==3)
			{
				if(ctx->jpg_is420)
				{
//					y=PDJPG_GetComponentPixelS1(ctx, 0, j, i);
//					u=PDJPG_GetComponentPixelS2(ctx, 1, j, i);
//					v=PDJPG_GetComponentPixelS2(ctx, 2, j, i);
					PDJPG_GetComponentPixelS122(ctx, j, i, &y, &u, &v);
					a=255;
				}else if(ctx->jpg_is444)
				{
					PDJPG_GetComponentPixelS111(ctx, j, i, &y, &u, &v);
					a=255;
				}else
				{
					y=PDJPG_GetComponentPixel(ctx, 0, j, i);
					u=PDJPG_GetComponentPixel(ctx, 1, j, i);
					v=PDJPG_GetComponentPixel(ctx, 2, j, i);
					a=255;
				}

				if(ctx->alphaClr)
				{
					k=y-ctx->alphaClrY; l=k*k;
					k=u-ctx->alphaClrU; l+=k*k;
					k=v-ctx->alphaClrV; l+=k*k;
				
					a=ctx->alphaClrA;
					a=(l<=(a*a))?0:255;
				
//					if(l<(16*16))
//					{
//						a=255*((16*sqrt(l))/ctx->alphaClrA);
//						a=(a<0)?0:(a>255)?255:a;
//					}
				}

//				r=y+1.40200*(v-128);
//				g=y-0.34414*(u-128)-0.71414*(v-128);
//				b=y+1.77200*(u-128);

				if(ctx->jpg_clrtrans==PDJPG_CLRS_YCBCR)
				{
					u1=u-128; v1=v-128;
					r=65536*y          +91881*v1;
					g=65536*y- 22554*u1-46802*v1;
					b=65536*y+116130*u1;
					r>>=16; g>>=16; b>>=16;
				}else if(ctx->jpg_clrtrans==PDJPG_CLRS_ORCT)
				{
					u1=u-128; v1=v-128;
					g=y-(u1+v1)/4;
					r=v1+g;
					b=u1+g;
				}else if(ctx->jpg_clrtrans==PDJPG_CLRS_RGB)
				{
					r=y;
					g=u;
					b=v;
				}

				r=(r<0)?0:((r>255)?255:r);
				g=(g<0)?0:((g>255)?255:g);
				b=(b<0)?0:((b>255)?255:b);

//				r=(r<0)?0:((r>16777215)?255:(r>>16));
//				g=(g<0)?0:((g>16777215)?255:(g>>16));
//				b=(b<0)?0:((b>16777215)?255:(b>>16));
			}else if(ctx->jpg_nc==1)
			{
				y=PDJPG_GetComponentPixel(ctx, 0, j, i);
				r=y; g=y; b=y; a=255;
			}

			k=ctx->ys-1-i;
			l=((k*ctx->xs)+j)*4;

			obuf[l+0]=r;
			obuf[l+1]=g;
			obuf[l+2]=b;
			obuf[l+3]=a;
		}
	}

	for(i=0; i<4; i++)
	{
		if(ctx->jpg_scbuf[i])free(ctx->jpg_scbuf[i]);
		if(ctx->jpg_sibuf[i])free(ctx->jpg_sibuf[i]);
		ctx->jpg_scbuf[i]=NULL;
		ctx->jpg_sibuf[i]=NULL;
	}

	*xs=ctx->xs;
	*ys=ctx->ys;
	return(obuf);
}

PDGL_API void PDJPG_Free(byte *buf)
{
	if(!buf)return;
	free(buf);
}

byte *PDJPG_DecodeScanForComponentLayer(byte *buf, int sz, char *name)
{
	byte *cs, *cs2, *cse;
	int i;
	
	cs=buf; cse=buf+sz;
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==JPG_APP11))
		{
			i=(cs[2]<<8)|cs[3];
			cs2=cs+i+2;

			cs+=4;
			if(!strcmp((char *)cs, "CompLayer"))
			{
				cs+=strlen((char *)cs)+1;
				if(!strcmp((char *)cs, name))
				{
//					printf("Found %s\n", name);
					cs+=strlen((char *)cs)+1;
					return(cs2);
				}

				cs+=strlen((char *)cs)+1;
				continue;
			}
			cs+=i-2;
			continue;
		}
		if((cs[0]==0xFF) && (cs[1]>=JPG_APP0) && (cs[1]<=JPG_APP15))
		{
			i=(cs[2]<<8)|cs[3];
			cs+=i+2;
			continue;
		}

		cs++;
	}
	
	return(NULL);
}

PDGL_API byte *PDJPG_Decode(byte *buf, int sz, int *xs, int *ys)
{
	PDJPG_Context *ctx;
	byte *obuf;
	byte *otbuf;
	byte *csl;
	int i, n, sz1;
	
	ctx=PDJPG_AllocContext();
	obuf=PDJPG_DecodeCtx(ctx, buf, sz, xs, ys);
	ctx->jpg_imgbuf=NULL;
	
	csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
	if(csl)
	{
		sz1=sz-(csl-buf);
		otbuf=PDJPG_DecodeCtx(ctx, csl, sz1, xs, ys);
		
		n=ctx->xs*ctx->ys;
		for(i=0; i<n; i++)
		{
			obuf[i*4+3]=otbuf[i*4+1];
		}
	}

	PDJPG_FreeContext(ctx);
	return(obuf);
}

PDGL_API int PDJPG_DecodeComponentType(byte *buf, int sz)
{
	byte *csl;
	int sz1;

//	sz1=(sz>1024)?1024:sz;
//	csl=PDJPG_DecodeScanForComponentLayer(buf, sz1, "RGB");
//	if(!csl)return(0);

	csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "DASe");
	if(csl)
	{
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
		if(csl)return(16);
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "SpRGB");
		if(csl)return(12);
		return(8);
	}else
	{
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
		if(csl)return(7);

		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
		if(csl)return(4);

		return(3);
	}
}

PDGL_API byte *PDJPG_DecodeLDatCtx(PDJPG_Context *ctx,
	byte *buf, int sz, int *xs, int *ys)
{
	byte *tbuf, *tbuf2, *cs, *ct;
	int i;

	if((buf[0]!=0xFF) || (buf[1]!=JPG_APP12) ||
		(buf[4]!='L') || (buf[5]!='D') ||
		(buf[6]!='A') || (buf[7]!='T'))
	{
		tbuf=PDJPG_DecodeCtx(ctx, buf, sz, xs, ys);
		return(tbuf);
	}
	
	tbuf=malloc(1<<20);
	cs=buf; ct=tbuf;

	while((cs[0]==0xFF) && (cs[1]==JPG_APP12) &&
		(cs[4]=='L') && (cs[5]=='D') &&
		(cs[6]=='A') && (cs[7]=='T'))
	{
		i=(cs[2]<<8)|cs[3];
		memcpy(ct, cs+8, i-6);
		cs+=i+2; ct+=(i-6);
	}

	i=ct-tbuf;
	i=PDJPG_EscapeDecodeSingleBuffer(tbuf, i);

	tbuf2=PDJPG_DecodeCtx(ctx, tbuf, i, xs, ys);
	free(tbuf);
	return(tbuf2);
}

PDGL_API int PDJPG_DecodeComponentCtx(PDJPG_Context *ctx,
	byte *buf, int sz,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *xs, int *ys)
{
	return(PDJPG_DecodeComponentCtxI(ctx, buf, sz,
		rgba, norm, spec, luma, NULL,
		xs, ys));
}

PDGL_API int PDJPG_DecodeFloatComponentCtx(PDJPG_Context *ctx,
	byte *buf, int sz,
	float *rgba, float *norm, float *spec, float *luma,
	int *xs, int *ys)
{
	byte *trgba, *tnorm, *tspec, *tluma, *tlebe;
	int i, n;
	float f, g;

	PDJPG_DummyDecodeCtx(ctx, buf, sz, xs, ys);
	if(i<0)return(-1);

	n=ctx->xs*ctx->ys;
	if(!ctx->imgbuf_rgba)
		ctx->imgbuf_rgba=malloc(4*n);
	if(!ctx->imgbuf_norm)
		ctx->imgbuf_norm=malloc(4*n);
	if(!ctx->imgbuf_spec)
		ctx->imgbuf_spec=malloc(4*n);
	if(!ctx->imgbuf_luma)
		ctx->imgbuf_luma=malloc(4*n);

	if(!ctx->imgbuf_lebe)
	{
		ctx->imgbuf_lebe=malloc(4*n);
		memset(ctx->imgbuf_lebe, 192, 4*n);
	}

	i=PDJPG_DecodeComponentCtxI(ctx, buf, sz,
		ctx->imgbuf_rgba, ctx->imgbuf_norm,
		ctx->imgbuf_spec, ctx->imgbuf_luma,
		ctx->imgbuf_lebe,
		xs, ys);
	if(i<0)
	{
		return(-1);
	}
	
	trgba=ctx->imgbuf_rgba;
	tnorm=ctx->imgbuf_norm;
	tspec=ctx->imgbuf_spec;
	tluma=ctx->imgbuf_luma;
	tlebe=ctx->imgbuf_lebe;
	
	if(rgba)
	{
		for(i=0; i<n; i++)
		{
			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			rgba[i*4+0]=trgba[i*4+0]*f;
			rgba[i*4+1]=trgba[i*4+1]*f;
			rgba[i*4+2]=trgba[i*4+2]*f;
			rgba[i*4+3]=trgba[i*4+3]*(1.0/256);
		}
	}

	if(norm)
	{
		for(i=0; i<n; i++)
		{
//			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			f=1.0/256;
			g=(1.0/256)*pdjpg_scltab[tlebe[i*4+2]];
			norm[i*4+0]=tnorm[i*4+0]*f;
			norm[i*4+1]=tnorm[i*4+1]*f;
			norm[i*4+2]=tnorm[i*4+2]*f;
			norm[i*4+3]=tnorm[i*4+3]*g;
		}
	}

	if(spec)
	{
		for(i=0; i<n; i++)
		{
//			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			f=1.0/256;
			spec[i*4+0]=tspec[i*4+0]*f;
			spec[i*4+1]=tspec[i*4+1]*f;
			spec[i*4+2]=tspec[i*4+2]*f;
			spec[i*4+3]=tspec[i*4+3]*f;
		}
	}

	if(luma)
	{
		for(i=0; i<n; i++)
		{
			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+1]];
			luma[i*4+0]=tluma[i*4+0]*f;
			luma[i*4+1]=tluma[i*4+1]*f;
			luma[i*4+2]=tluma[i*4+2]*f;
			luma[i*4+3]=tluma[i*4+3]*(1.0/256);
		}
	}
}

PDGL_API int PDJPG_ExtractComponentCtx(PDJPG_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe)
{
	int n;
	
	n=ctx->xs*ctx->ys;
	if(rgba && ctx->imgbuf_rgba)
		memcpy(rgba, ctx->imgbuf_rgba, n*4);
	if(norm && ctx->imgbuf_norm)
		memcpy(norm, ctx->imgbuf_norm, n*4);
	if(spec && ctx->imgbuf_spec)
		memcpy(spec, ctx->imgbuf_spec, n*4);
	if(luma && ctx->imgbuf_luma)
		memcpy(luma, ctx->imgbuf_luma, n*4);
	if(lebe && ctx->imgbuf_lebe)
		memcpy(lebe, ctx->imgbuf_lebe, n*4);
}

PDGL_API int PDJPG_DecodeComponentCtxI(PDJPG_Context *ctx,
	byte *buf, int sz,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe,
	int *xs, int *ys)
{
	byte *tbuf;
	byte *csl;
	int i, n, sz1;

	tbuf=PDJPG_DecodeCtx(ctx, buf, sz, xs, ys);
	if(!tbuf)return(-1);

	n=ctx->xs*ctx->ys;

	for(i=0; i<n; i++)
	{
		rgba[i*4+0]=tbuf[i*4+0];
		rgba[i*4+1]=tbuf[i*4+1];
		rgba[i*4+2]=tbuf[i*4+2];
//		rgba[i*4+3]=tbuf[i*4+3];
	}

	if(norm)
	{
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "XYZ");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=PDJPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				norm[i*4+0]=tbuf[i*4+0];
				norm[i*4+1]=tbuf[i*4+2];
				norm[i*4+2]=tbuf[i*4+1];
//				norm[i*4+3]=tbuf[i*4+3];
			}
		}
	}

	if(spec)
	{
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "SpRGB");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=PDJPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				spec[i*4+0]=tbuf[i*4+0];
				spec[i*4+1]=tbuf[i*4+1];
				spec[i*4+2]=tbuf[i*4+2];
//				spec[i*4+3]=tbuf[i*4+3];
			}
		}
	}

	csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "DASe");
	if(csl)
	{
		sz1=sz-(csl-buf);
		tbuf=PDJPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
		for(i=0; i<n; i++)
		{
			rgba[i*4+3]=tbuf[i*4+1];
			if(norm) { norm[i*4+3]=tbuf[i*4+0]; }
			if(spec) { spec[i*4+3]=tbuf[i*4+2]; }
		}
	}else
	{
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "Alpha");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=PDJPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
				{ rgba[i*4+3]=tbuf[i*4+1]; }
		}
		
		if(norm)
		{
			csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "Depth");
			if(csl)
			{
				sz1=sz-(csl-buf);
				tbuf=PDJPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
				for(i=0; i<n; i++)
					{ norm[i*4+3]=tbuf[i*4+1]; }
			}
		}
	}
	
	if(luma)
	{
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "LuRGB");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=PDJPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				luma[i*4+0]=tbuf[i*4+0];
				luma[i*4+1]=tbuf[i*4+1];
				luma[i*4+2]=tbuf[i*4+2];
				luma[i*4+3]=255;
			}
		}
	}

	if(lebe)
	{
		csl=PDJPG_DecodeScanForComponentLayer(buf, sz, "LeBe");
		if(csl)
		{
			sz1=sz-(csl-buf);
			tbuf=PDJPG_DecodeLDatCtx(ctx, csl, sz1, xs, ys);
			for(i=0; i<n; i++)
			{
				lebe[i*4+0]=tbuf[i*4+0];
				lebe[i*4+1]=tbuf[i*4+1];
				lebe[i*4+2]=tbuf[i*4+2];
			}
		}
	}

	return(0);
}

PDGL_API int PDJPG_DecodeComponent(byte *buf, int sz,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int *xs, int *ys)
{
	PDJPG_Context *ctx;
	int i;
	
	ctx=PDJPG_AllocContext();
	i=PDJPG_DecodeComponentCtx(ctx, buf, sz,
		rgba, norm, spec, luma, xs, ys);
//	ctx->jpg_imgbuf=NULL;
	PDJPG_FreeContext(ctx);
	return(i);
}

//Encoder

void PDJHUFF_WriteBit(PDJPG_Context *ctx, int i)
{
	pdjhuff_win|=i<<(31-pdjhuff_pos);
	pdjhuff_pos++;
	if(pdjhuff_pos>=8)
	{
		i=(pdjhuff_win>>24)&0xFF;
		*pdjhuff_ct++=i;
		if(i==0xFF)*pdjhuff_ct++=0x00;
		pdjhuff_win<<=8;
		pdjhuff_pos-=8;
	}
}

void PDJHUFF_WriteNBits(PDJPG_Context *ctx, int v, int n)
{
	int i, j, k;

	v&=(1<<n)-1;

	j=pdjhuff_pos+n;
	k=pdjhuff_win|(v<<(32-j));
	while(j>=8)
	{
		i=(k>>24)&0xFF;
		*pdjhuff_ct++=i;
		if(i==0xFF)*pdjhuff_ct++=0x00;
		k<<=8;
		j-=8;
	}
	pdjhuff_pos=j;
	pdjhuff_win=k;

//	pdjhuff_win|=i<<((32-n)-pdjhuff_pos);
//	pdjhuff_pos+=n;
//	while(pdjhuff_pos>=8)
//	{
//		i=(pdjhuff_win>>24)&0xFF;
//		*pdjhuff_ct++=i;
//		if(i==0xFF)*pdjhuff_ct++=0x00;
//		pdjhuff_win<<=8;
//		pdjhuff_pos-=8;
//	}
}

void PDJHUFF_FlushBits(PDJPG_Context *ctx)
{
	int i;
	while(pdjhuff_pos>0)
	{
		i=(pdjhuff_win>>24)&0xFF;
		*pdjhuff_ct++=i;
		if(i==0xFF)*pdjhuff_ct++=0x00;
		pdjhuff_win<<=8;
		pdjhuff_pos-=8;
	}
}

void PDJHUFF_WriteString(PDJPG_Context *ctx, char *str)
{
	char *s;

	PDJHUFF_FlushBits(ctx);	
	s=str;
	while(*s)
		*pdjhuff_ct++=*s++;
	*pdjhuff_ct++=0;
}

int PDJHUFF_BalanceTree_r(short *nodes, short *nlen, int root, int h, int ml)
{
	int h0, h1, h2, h3;
	int l0, l1, l2;

	if(root<0)return(0);

	h1=PDJHUFF_BalanceTree_r(nodes, nlen, nodes[root*2+0], h+1, ml);
	h2=PDJHUFF_BalanceTree_r(nodes, nlen, nodes[root*2+1], h+1, ml);
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

	if((h+h0)<=ml)	//depth limit not exceeded
		return(h0);

	//ok, so part of the tree is too deep
	if((h1+1)<h2)
	{
		l0=nodes[root*2+1];
		l1=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[root*2+0];
		nodes[root*2+0]=l0;
		nodes[root*2+1]=l1;
	}else if((h2+1)<h1)
	{
		l0=nodes[root*2+0];
		l1=nodes[l0*2+0];
		nodes[l0*2+0]=nodes[l0*2+1];
		nodes[l0*2+1]=nodes[root*2+1];
		nodes[root*2+0]=l1;
		nodes[root*2+1]=l0;
	}else
	{
		//rotating would be ineffective or would make things worse...
		return(h0);
	}

	//recalc depth of modified sub-tree
	l1=nodes[l0*2+0];
	l2=nodes[l0*2+1];
	h1=(l1<0)?0:nlen[l1];
	h2=(l2<0)?0:nlen[l2];
	h3=((h1>h2)?h1:h2)+1;
	nlen[l0]=h3;

	//recalc height of root node
	l1=nodes[root*2+0];
	l2=nodes[root*2+1];
	h1=(l1<0)?0:nlen[l1];
	h2=(l2<0)?0:nlen[l2];
	h0=((h1>h2)?h1:h2)+1;
	nlen[root]=h0;

	return(h0);
}

void PDJHUFF_CalcLengths_r(short *nodes, byte *cl, int root, int h)
{
	if(root<0)
	{
		if(root==-513)return;
		cl[(-root)-1]=h;
		return;
	}

	PDJHUFF_CalcLengths_r(nodes, cl, nodes[root*2+0], h+1);
	PDJHUFF_CalcLengths_r(nodes, cl, nodes[root*2+1], h+1);
}

int PDJHUFF_BuildLengths(int *stat, int nc, byte *cl, int ml)
{
//	static short nodes[1024], nlen[512];
//	static short roots[512], clen[512];
//	static int cnts[512];

	short nodes[1024], nlen[512];
	short roots[512], clen[512];
	int cnts[512];
	int nr, nn;
	int i, j, k, l;

	nr=0; nn=0;
	for(i=0; i<nc; i++)
	{
		if(!stat[i])continue;
		roots[nr]=-(i+1);
		cnts[nr]=stat[i];
		clen[nr]=0;
		nr++;
	}

	roots[nr]=-513;
	cnts[nr]=0;
	clen[nr]=0;
	nr++;


	for(i=0; i<nc; i++)cl[i]=0;
	if(!nr)return(-1);


	while(nr>1)
	{
		if(cnts[0]>=cnts[1]) { j=0; k=1; }
			else { j=1; k=0; }
		for(i=2; i<nr; i++)
		{
			if(cnts[i]<=cnts[k])
			{
				j=k; k=i;
				continue;
			}
			if(cnts[i]<=cnts[j])
			{
				j=i;
				continue;
			}
		}

		nlen[nn]=((clen[j]>clen[k])?clen[j]:clen[k])+1;
		nodes[nn*2+0]=roots[j];
		nodes[nn*2+1]=roots[k];

		roots[nr]=nn;
		cnts[nr]=cnts[j]+cnts[k];
		clen[nr]=nlen[nn];

//		printf("%d %d %d\n", cnts[j], cnts[k], cnts[nr]);

		nn++; nr++;

		l=0;
		for(i=0; i<nr; i++)
		{
			if((i==j) || (i==k))continue;
			roots[l]=roots[i];
			cnts[l]=cnts[i];
			clen[l]=clen[i];
			l++;
		}
		nr=l;
	}

	l=roots[0];
	j=clen[0];
	k=j;

	i=4;
	while((i--) && (k>ml))
		k=PDJHUFF_BalanceTree_r(nodes, nlen, l, 0, ml);
	if(k>ml)return(-2);	//failed to balance tree

	PDJHUFF_CalcLengths_r(nodes, cl, l, 0);
	return(0);
}

int PDJHUFF_BuildLengthsAdjust(int *stat, int nc, byte *cl, int ml)
{
	int i, j;

	while(1)
	{
		j=PDJHUFF_BuildLengths(stat, nc, cl, ml);
		if(j<0)
			printf("PDJHUFF_BuildLengthsAdjust: Huff Fail %d\n");

		for(i=0; i<nc; i++)
			if(stat[i] && !cl[i])
				break;
		if(i>=nc)break;

		printf("PDJHUFF_BuildLengthsAdjust: Fiddle Adjust\n");
		for(i=0; i<nc; i++)
			stat[i]++;
		continue;
	}
	return(0);
}

void PDJHUFF_EncodeSymbol(PDJPG_Context *ctx, int tab, int v)
{
	tab<<=8;
	PDJHUFF_WriteNBits(ctx, ctx->huff_code[tab|v], ctx->huff_len[tab|v]);
}

void PDJHUFF_EncodeVal(PDJPG_Context *ctx, int tab, int z, int v)
{
	int i, j, k;

	if(!v) { PDJHUFF_EncodeSymbol(ctx, tab, z<<4); return; }

	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		if(i>15)
		{
			printf("PDJHUFF_EncodeVal: Overflow A\n");
			i=15;
		}
		
		PDJHUFF_EncodeSymbol(ctx, tab, (z<<4)|i);
		PDJHUFF_WriteNBits(ctx, v, i);
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	if(i>15)
	{
		printf("PDJHUFF_EncodeVal: Overflow B\n");
		i=15;
	}

	PDJHUFF_EncodeSymbol(ctx, tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	PDJHUFF_WriteNBits(ctx, k, i);
	return;
}

void PDJHUFF_StatVal(int *stat, int z, int v)
{
	int i, j, k;

	if(!v) { stat[z<<4]++; return; }

	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		if(i>15)i=15;
		stat[(z<<4)|i]++;
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	if(i>15)i=15;
	stat[(z<<4)|i]++;
}

#if 1
int PDJHUFF_EncodeBlock(PDJPG_Context *ctx,
	short *buf, int dctab, int actab)
{
	int i, j, k;

	PDJHUFF_EncodeVal(ctx, dctab, 0, buf[0]);

	for(i=1; i<64; i++)
	{
		if(buf[pdjpg_zigzag2[i]])
		{
			PDJHUFF_EncodeVal(ctx, actab, 0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			PDJHUFF_EncodeSymbol(ctx, actab, 0);
			break;
		}

		j-=i; if(j>15)j=15;
		PDJHUFF_EncodeVal(ctx, actab, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
	return(0);
}


int PDJHUFF_StatBlock(short *buf, int *dcstat, int *acstat)
{
	int i, j, k;

	PDJHUFF_StatVal(dcstat, 0, buf[0]);
	for(i=1; i<64; i++)
	{
		if(buf[pdjpg_zigzag2[i]])
		{
			PDJHUFF_StatVal(acstat, 0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			acstat[0]++;
			break;
		}

		j-=i; if(j>15)j=15;
		PDJHUFF_StatVal(acstat, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
	return(0);
}
#endif

#if defined(X86) || defined(X86_64)
#define memcpy8(dst, src)	(*(double *)(dst)=*(double *)(src))
#else
#define memcpy8(dst, src)	memcpy(dst, src, 8)
#endif

#define memcpy16(dst, src)	memcpy(dst, src, 16)

#if 0
int PDJPG_GetImgBlk(char *blk, int xo, int yo, byte *img, int xs, int ys)
{
	byte *cs;
	char *ct;
	int i, j, k;

	cs=img+(yo*xs+xo); ct=blk;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); ct+=8; cs+=xs;
	memcpy8(ct, cs); // ct+=8; cs+=xs;
	return(0);
}
#endif

#if 1
int PDJPG_GetImgBlk16(
	short *blk, int xo, int yo,
	short *img, int xs, int ys)
{
	short *cs;
	short *ct;
	int i, j, k;

	cs=img+(yo*xs+xo); ct=blk;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs); ct+=8; cs+=xs;
	memcpy16(ct, cs);
	return(0);
}
#endif

// int PDJPG_FilterImageDCT(byte *ibuf, short *obuf, int xs, int ys)
int PDJPG_FilterImageDCT(short *ibuf, short *obuf,
	int xs, int ys, int dcbias)
{
//	static short tblk[DCTSZ2], tblk2[DCTSZ2];
//	static char blk[DCTSZ2];
	short tblk[DCTSZ2], tblk2[DCTSZ2];
//	char blk[DCTSZ2];
	short blk[DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/DCTSZ); i++)
		for(j=0; j<(xs/DCTSZ); j++)
	{
//		PDJPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		PDJPG_GetImgBlk16(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		PDJPG_TransDCT(blk, obuf+k*DCTSZ2, dcbias);
//		PDJPG_TransQDCT(blk, obuf+k*DCTSZ2, dcbias);
		k++;
	}
	return(0);
}

//int PDJPG_FilterImageRDCT(byte *ibuf, short *obuf, int xs, int ys)
int PDJPG_FilterImageRDCT(short *ibuf, short *obuf,
	int xs, int ys, int dcbias)
{
//	static short tblk[DCTSZ2], tblk2[DCTSZ2];
//	static char blk[DCTSZ2];
	short tblk[DCTSZ2], tblk2[DCTSZ2];
//	char blk[DCTSZ2];
	short blk[DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/DCTSZ); i++)
		for(j=0; j<(xs/DCTSZ); j++)
	{
//		PDJPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		PDJPG_GetImgBlk16(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		PDJPG_TransRDCT(blk, obuf+k*DCTSZ2, dcbias);
		k++;
	}
	return(0);
}

void PDJPG_EmitDQT(PDJPG_Context *ctx, int n)
{
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_DQT;

	i=64+3;
	*pdjhuff_ct++=i>>8;
	*pdjhuff_ct++=i&0xFF;

	*pdjhuff_ct++=n;
	for(i=0; i<64; i++)
		*pdjhuff_ct++=ctx->jpg_qt[n][pdjpg_zigzag2[i]];
}

void PDJPG_EmitSOF(PDJPG_Context *ctx, int xs, int ys)
{
	byte *p;
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOF0;

	i=8+(ctx->jpg_mono?1:3)*3;
	p=pdjhuff_ct;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	*pdjhuff_ct++=8; 	//P

	*pdjhuff_ct++=ys>>8;	//Y
	*pdjhuff_ct++=ys&0xFF;	//Y
	*pdjhuff_ct++=xs>>8;	//X
	*pdjhuff_ct++=xs&0xFF;	//X

	if(ctx->jpg_mono)
	{
		*pdjhuff_ct++=1;	//Nf

		*pdjhuff_ct++=1;	//Ci
		*pdjhuff_ct++=0x11;	//Hi Vi
		*pdjhuff_ct++=0;	//Tqi
	}else if(ctx->jpg_is420)
	{
		if(!ctx->jpg_yuvw)
		{
			//YUV 4:2:0
		
			*pdjhuff_ct++=3;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x22;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}else
		{
			//YUVW 4:2:0

			*pdjhuff_ct++=4;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x22;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=4;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}
	}else
	{
		if(!ctx->jpg_yuvw)
		{
			//YUV 4:4:4

			*pdjhuff_ct++=3;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}else
		{
			//YUVW 4:4:4

			*pdjhuff_ct++=4;	//Nf

			*pdjhuff_ct++=1;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=0;	//Tqi
			*pdjhuff_ct++=2;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=3;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
			*pdjhuff_ct++=4;	//Ci
			*pdjhuff_ct++=0x11;	//Hi Vi
			*pdjhuff_ct++=1;	//Tqi
		}
	}

	i=pdjhuff_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
}

void PDJPG_EmitSOS(PDJPG_Context *ctx)
{
	byte *p;
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOS;

	i=6+(ctx->jpg_mono?1:3)*2;
	p=pdjhuff_ct;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	if(ctx->jpg_yuvw)
	{
		*pdjhuff_ct++=4; 	//Ns

		*pdjhuff_ct++=1;	//Csi
		*pdjhuff_ct++=0x00;	//Tdi Tai
		*pdjhuff_ct++=2;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
		*pdjhuff_ct++=3;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
		*pdjhuff_ct++=4;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
	}else if(!ctx->jpg_mono)
	{
		*pdjhuff_ct++=3; 	//Ns

		*pdjhuff_ct++=1;	//Csi
		*pdjhuff_ct++=0x00;	//Tdi Tai
		*pdjhuff_ct++=2;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
		*pdjhuff_ct++=3;	//Csi
		*pdjhuff_ct++=0x11;	//Tdi Tai
	}else
	{
		*pdjhuff_ct++=1; 	//Ns

		*pdjhuff_ct++=1;	//Csi
		*pdjhuff_ct++=0x00;	//Tdi Tai
	}

	*pdjhuff_ct++=0; 	//Ss
	*pdjhuff_ct++=63; 	//Se
	*pdjhuff_ct++=0x00; 	//Ah Al

	i=pdjhuff_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
}

void PDJPG_EmitDHT(PDJPG_Context *ctx, int tab)
{
	byte *p;
	int i, j, k;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_DHT;

	i=8+3*1;	//temp value
	p=pdjhuff_ct;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	i=(tab/2)|((tab&1)<<4);
	*pdjhuff_ct++=i; 	//Tc Th

	tab<<=8;
	for(i=1; i<=16; i++)
	{
		k=0;
		for(j=0; j<256; j++)
			if(ctx->huff_len[tab|j]==i)
				k++;
		*pdjhuff_ct++=k; 	//Li
	}

	k=0;
	for(i=1; i<=16; i++)
	{
		k<<=1;
		for(j=0; j<256; j++)
			if(ctx->huff_len[tab|j]==i)
		{
			*pdjhuff_ct++=j; 	//Vi
			ctx->huff_code[tab|j]=k++;
		}
	}

	if(k>=65536)
		printf("PDJPG_EmitDHT: Error %04X\n", k);

//	printf("DHT %04X\n", k);

	i=pdjhuff_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
}


static int pdjpg_ijg_qtab_y[64] = {
16, 11, 10, 16,  24,  40,  51,  61,
12, 12, 14, 19,  26,  58,  60,  55,
14, 13, 16, 24,  40,  57,  69,  56,
14, 17, 22, 29,  51,  87,  80,  62,
18, 22, 37, 56,  68, 109, 103,  77,
24, 35, 55, 64,  81, 104, 113,  92,
49, 64, 78, 87, 103, 121, 120, 101,
72, 92, 95, 98, 112, 100, 103,  99};

static int pdjpg_ijg_qtab_uv[64] = {
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
99, 99, 99, 99, 99, 99, 99, 99,
47, 66, 99, 99, 99, 99, 99, 99,
24, 26, 56, 99, 99, 99, 99, 99,
18, 21, 26, 66, 99, 99, 99, 99,
17, 18, 24, 47, 99, 99, 99, 99};

void PDJPG_MakeQuantTabFastIJG_Y(
	short *inv, int cnt, byte *tab, float qf)
{
	double s, q;
	int i, j;

	q = (qf * 100);
	s = (q < 50) ? 5000 / q : (200 - 2 * q);
	for (i = 0; i < 64; i++)
	{
		j = (int)((s * pdjpg_ijg_qtab_y[i] + 50) / 100);
		j = (j < 1) ? 1 : ((j < 256) ? j : 255);
		tab[i] = j;
	}
}

void PDJPG_MakeQuantTabFastIJG_UV(
	short *inv, int cnt, byte *tab, float qf)
{
	double s, q;
	int i, j;

	q = (qf * 100);
	s = (q < 50) ? 5000 / q : (200 - 2 * q);
	for (i = 0; i < 64; i++)
	{
		j = (int)((s * pdjpg_ijg_qtab_y[i] + 50) / 100);
		j = (j < 1) ? 1 : ((j < 256) ? j : 255);
		tab[i] = j;
	}
}

void PDJPG_MakeQuantTabInput(short *in, int cnt, byte *tab, float q)
{
	PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
}

void PDJPG_MakeQuantTabInputY(short[] inv, int cnt, BytePtr tab, float q)
{
	byte taba[64], tabb[64];
	int i;

	//PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
	//PDJPG_MakeQuantTabFastIJG_UV(inv, cnt, tab, q);

	PDJPG_MakeQuantTabInput_AA(inv, cnt, taba, q);
	PDJPG_MakeQuantTabFastIJG_UV(inv, cnt, tabb, q);
	for (i = 0; i < 64; i++)
		tab[i] = (taba[i] + tabb[i]) / 2;
}

void PDJPG_MakeQuantTabInputUV(short[] inv, int cnt, BytePtr tab, float q)
{
	byte taba[64], tabb[64];
	int i;

	//PDJPG_MakeQuantTabInput_AA(inv, cnt, tab, q);
	//PDJPG_MakeQuantTabFastIJG_UV(inv, cnt, tab, q);

	PDJPG_MakeQuantTabInput_AA(inv, cnt, taba, q);
	PDJPG_MakeQuantTabFastIJG_UV(inv, cnt, tabb, q);
	for (i = 0; i < 64; i++)
		tab[i] = (taba[i] + tabb[i]) / 2;
}

void PDJPG_MakeQuantTabInput_AA(short *in, int cnt, byte *tab, float q)
{
//	static double deltas[DCTSZ2];
	double deltas[DCTSZ2];
	double f, g, te;
	int i, j, k;

	if(q>=1)
	{
		for(i=0; i<DCTSZ2; i++)
			tab[i]=1;
		return;
	}

	for(j=0; j<DCTSZ2; j++)
		deltas[j]=0;

	for(i=0; i<cnt; i++)
		for(j=0; j<DCTSZ2; j++)
	{
		f=in[i*DCTSZ2+j];
		deltas[j]+=f*f;
	}
	for(j=0; j<DCTSZ2; j++)
	{
		deltas[j]=sqrt(deltas[j]/cnt);
//		deltas[j]/=cnt;
	}

	te=0;
	for(j=0; j<DCTSZ2; j++)
		te+=deltas[j];

	for(i=0; i<DCTSZ; i++)
	{
		for(j=0; j<DCTSZ; j++)
		{
			f=deltas[i*DCTSZ+j];

			f=f/te;
			if(f<0.00001)f=0.00001;
			g=(1.0-q);
			f=10*g*g*g/f;
			k=f;
			if(k<1)k=1;
			if(k>255)k=255;

//			printf("%f/%f %d\n", deltas[i*DCTSZ+j], te, k);

//			if(f<1)f=1;
//			if(f>255)f=255;
			tab[i*DCTSZ+j]=k;
		}
	}

#if 1
	if(tab[DCTSZ2-3]>1)tab[DCTSZ2-3]*=0.75;
	if(tab[DCTSZ2-2]>1)tab[DCTSZ2-2]*=0.75;
	if(tab[DCTSZ2-1]>1)tab[DCTSZ2-1]*=0.5;
#endif
}

void PDJPG_MakeQuantTabInputFast(
	short *in, int cnt, byte *tab, float q)
{
	int i;
//	PDJPG_MakeQuantTabInput(in, cnt, tab, q);

	i=cnt/16;
	if(i<1)i=1;
	PDJPG_MakeQuantTabInputY(in, i, tab, q);
}

void PDJPG_EmitMarkerBCST(PDJPG_Context *ctx)
{
	byte *lfp;
	int i, j, k;

	if(ctx->jpg_rdct)
	{
		//Emit libjpeg SERM marker
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP9;

		lfp=pdjhuff_ct;
		*pdjhuff_ct++=0x00;	//length
		*pdjhuff_ct++=0x00;
		*pdjhuff_ct++='J';
		*pdjhuff_ct++='P';
		*pdjhuff_ct++='S';
		*pdjhuff_ct++='E';
		*pdjhuff_ct++='R';
		*pdjhuff_ct++='M';
		*pdjhuff_ct++=0;

		i=pdjhuff_ct-lfp;
		lfp[0]=i>>8;	//length
		lfp[1]=i&0xFF;
	}

	if(ctx->jpg_clrtrans==PDJPG_CLRS_RGB)
	{
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP14;

		lfp=pdjhuff_ct;
		*pdjhuff_ct++=0x00;	//length
		*pdjhuff_ct++=0x00;

		*pdjhuff_ct++='A';
		*pdjhuff_ct++='d';
		*pdjhuff_ct++='o';
		*pdjhuff_ct++='b';
		*pdjhuff_ct++='e';

		i=100;
		*pdjhuff_ct++=(i>>8)&0xFF;
		*pdjhuff_ct++=i&0xFF;

		i=0;
		*pdjhuff_ct++=(i>>24)&0xFF;
		*pdjhuff_ct++=(i>>16)&0xFF;
		*pdjhuff_ct++=(i>>8)&0xFF;
		*pdjhuff_ct++=i&0xFF;

		*pdjhuff_ct++=0;	//RGB
//		*pdjhuff_ct++=ctx->bcst_bpp;

		i=pdjhuff_ct-lfp;
		lfp[0]=i>>8;	//length
		lfp[1]=i&0xFF;
	}

	ctx->bcst_version=0x0100;
	ctx->bcst_flags=0;
	ctx->bcst_trans=ctx->jpg_clrtrans;
	ctx->bcst_bpp=8;

	if(ctx->jpg_rdct)
		ctx->bcst_flags|=PDJPG_BCSFL_RDCT;

	//check if not to bother with marker...
	if(!ctx->bcst_flags && !ctx->bcst_trans)
	{
//		printf("PDJPG_EmitMarkerBCST: No Emit\n");
		return;
	}

//	printf("PDJPG_EmitMarkerBCST: Emit\n");

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_APP12;

	lfp=pdjhuff_ct;
	*pdjhuff_ct++=0x00;	//length
	*pdjhuff_ct++=0x00;

	*pdjhuff_ct++='B';
	*pdjhuff_ct++='C';
	*pdjhuff_ct++='S';
	*pdjhuff_ct++='T';

	*pdjhuff_ct++=(ctx->bcst_version>>8)&0xFF;
	*pdjhuff_ct++=ctx->bcst_version&0xFF;

	*pdjhuff_ct++=ctx->bcst_trans;
	*pdjhuff_ct++=ctx->bcst_bpp;

	*pdjhuff_ct++=(ctx->bcst_flags>>24)&0xFF;
	*pdjhuff_ct++=(ctx->bcst_flags>>16)&0xFF;
	*pdjhuff_ct++=(ctx->bcst_flags>>8)&0xFF;
	*pdjhuff_ct++=ctx->bcst_flags&0xFF;


	i=pdjhuff_ct-lfp;
	lfp[0]=i>>8;	//length
	lfp[1]=i&0xFF;
}

PDGL_API int PDJPG_EncodeCtx_Prepare(
	PDJPG_Context *ctx, int xs, int ys, int qfl, int pf)
{
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, cy, cu, cv;
	int i, j, k, l, qf;

	qf=qfl&255;

	ctx->jpg_is420=1;

	if((qf==110) || (qf==111) || (qfl&PDJPG_QFL_444) ||
			(qf==102) || (qf==103))
		ctx->jpg_is420=0;

//	ctx->jpg_is444=0;

	if(ctx->jpg_is420)
	{
		xs2=((xs+7)/8)*8;
		ys2=((ys+7)/8)*8;
//		xs3=(xs2+1)/2;
//		ys3=(ys2+1)/2;

		xs3=((xs+15)/16)*8;
		ys3=((ys+15)/16)*8;
		ctx->jpg_is444=0;
	}else
	{
		xs2=((xs+7)/8)*8;
		ys2=((ys+7)/8)*8;
		xs3=xs2;
		ys3=ys2;
		ctx->jpg_is444=1;
	}
	
	ctx->xs=xs;
	ctx->ys=ys;
	ctx->xs2=xs2;
	ctx->ys2=ys2;
	ctx->xs3=xs3;
	ctx->ys3=ys3;

	ctx->jpg_mono=0;
	ctx->jpg_yuvw=0;
	ctx->jpg_tabcacheframe=0;
	ctx->xs=xs;
	ctx->ys=ys;

	if(pf==PDJPG_YYYA)
		ctx->jpg_mono=1;

	ctx->jpg_rdct=0;
	if((qf==110) || (qf==111) || (qfl&PDJPG_QFL_RDCT))
		ctx->jpg_rdct=1;

	ctx->jpg_clrtrans=0;
	if((qf==110) || (qfl&PDJPG_QFL_ORCT) || (qf==101) || (qf==103))
		ctx->jpg_clrtrans=PDJPG_CLRS_ORCT;
	if((qf==111) || (qfl&PDJPG_QFL_RGB) || (qf==104))
		ctx->jpg_clrtrans=PDJPG_CLRS_RGB;

	//full quality
	for(i=0; i<64; i++)ctx->jpg_qt[0][i]=1;
	for(i=0; i<64; i++)ctx->jpg_qt[1][i]=1;

	//dummy huffman tables
	for(i=0; i<256; i++)ctx->huff_len[0*256+i]=0;
	for(i=0; i<256; i++)ctx->huff_len[1*256+i]=0;
	for(i=0; i<256; i++)ctx->huff_len[2*256+i]=0;
	for(i=0; i<256; i++)ctx->huff_len[3*256+i]=0;

	for(i=0; i<16; i++)ctx->huff_len[0*256+i]=4;
	for(i=0; i<16; i++)ctx->huff_len[2*256+i]=4;
	for(i=0; i<255; i++)ctx->huff_len[1*256+i]=8;
	for(i=0; i<255; i++)ctx->huff_len[3*256+i]=8;

//	for(i=0; i<255; i++)ctx->huff_len[0*256+i]=8;
//	for(i=0; i<255; i++)ctx->huff_len[2*256+i]=8;


//	ctx->huff_len[1*256+0]=7;

//	printf("M0\n");

	//make sure intermediate buffers are present and the correct size
	if(!ctx->yb || (xs!=ctx->lxs) || (ys!=ctx->lys))
	{
		if(ctx->yb)
		{
			free(ctx->yb);
			free(ctx->ub);
			free(ctx->vb);
			free(ctx->ydb);
			free(ctx->udb);
			free(ctx->vdb);
		}

		//W channel is rare, and so is handled specially
		if(ctx->wb)
		{
			free(ctx->wb);
			free(ctx->wdb);
		}

//		ctx->yb=malloc(xs2*ys2);
//		ctx->ub=malloc(xs2*ys2);
//		ctx->vb=malloc(xs2*ys2);

		ctx->yb=malloc(xs2*ys2*sizeof(short));
		ctx->ub=malloc(xs2*ys2*sizeof(short));
		ctx->vb=malloc(xs2*ys2*sizeof(short));

		ctx->ydb=malloc((xs2+8)*(ys2+8)*sizeof(short));
//		ctx->udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
//		ctx->vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		ctx->udb=malloc((xs2+8)*(ys2+8)*sizeof(short));
		ctx->vdb=malloc((xs2+8)*(ys2+8)*sizeof(short));

		if(ctx->jpg_yuvw)
		{
			ctx->wb=malloc(xs2*ys2*sizeof(short));
			ctx->wdb=malloc((xs2+8)*(ys2+8)*sizeof(short));
		}

		ctx->lxs=xs;
		ctx->lys=ys;
	}

//	memset(ctx->yb, 128, xs2*ys2);
//	memset(ctx->ub, 128, xs2*ys2);
//	memset(ctx->vb, 128, xs2*ys2);

	memset(ctx->yb, 128, xs2*ys2*sizeof(short));
	memset(ctx->ub, 128, xs2*ys2*sizeof(short));
	memset(ctx->vb, 128, xs2*ys2*sizeof(short));

	memset(ctx->ydb, 0, xs2*(ys2+8)*sizeof(short));
	memset(ctx->udb, 0, xs3*(ys3+8)*sizeof(short));
	memset(ctx->vdb, 0, xs3*(ys3+8)*sizeof(short));

	if(ctx->jpg_yuvw)
	{
		memset(ctx->wb, 128, xs2*ys2*sizeof(short));
		memset(ctx->wdb, 0, xs2*(ys2+8)*sizeof(short));
	}
}

PDGL_API int PDJPG_EncodeCtx_ColorTransform(
	PDJPG_Context *ctx, byte *ibuf, int xs, int ys, int pf)
{
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, ca, cy, cu, cv, cw;
	int i, j, k, l, qf;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1);
		cr=ibuf[(k*xs+j)*4+0];
		cg=ibuf[(k*xs+j)*4+1];
		cb=ibuf[(k*xs+j)*4+2];
		ca=ibuf[(k*xs+j)*4+2];

//		if(qf==110)
		if(ctx->jpg_clrtrans==PDJPG_CLRS_YCBCR)
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;

			cy=(cy<0)?0:((cy>255)?255:cy);
			cu=(cu<0)?0:((cu>255)?255:cu);
			cv=(cv<0)?0:((cv>255)?255:cv);
		}else if(ctx->jpg_clrtrans==PDJPG_CLRS_ORCT)
		{
//			cy=(cr+2*cg+cb)/4;
			cy=cg+(cb+cr-2*cg)/4;
			cu=(cb-cg)+128;
			cv=(cr-cg)+128;
			cw=ca;
//			k=cy-(cu+cv-256)/4;
//			cy+=cg-k;
//			if(k!=cg)cy++;

//			cu=cb-cr;
//			cv=cg-cr-0.5*cu;
//			cy=cg-0.5*cv;
//			cu+=128; cv+=128;
		}else if(ctx->jpg_clrtrans==PDJPG_CLRS_RGB)
		{
			cy=cr;
			cu=cg;
			cv=cb;
			cw=ca;
		}else
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;
		}

//		cu=128; cv=128;

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);

		ctx->yb[i*xs2+j]=cy;
		ctx->ub[i*xs2+j]=cu;
		ctx->vb[i*xs2+j]=cv;
		if(ctx->jpg_yuvw)
			ctx->wb[i*xs2+j]=cw;
	}

}

PDGL_API int PDJPG_EncodeCtx_ColorTransform16(
	PDJPG_Context *ctx, short *ibuf, int xs, int ys, int pf)
{
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, ca, cy, cu, cv, cw;
	int i, j, k, l, qf;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1);
		cr=ibuf[(k*xs+j)*4+0];
		cg=ibuf[(k*xs+j)*4+1];
		cb=ibuf[(k*xs+j)*4+2];
		ca=ibuf[(k*xs+j)*4+2];

//		if(qf==110)
		if(ctx->jpg_clrtrans==PDJPG_CLRS_YCBCR)
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;

			//cy=(cy<0)?0:((cy>255)?255:cy);
			//cu=(cu<0)?0:((cu>255)?255:cu);
			//cv=(cv<0)?0:((cv>255)?255:cv);
		}else if(ctx->jpg_clrtrans==PDJPG_CLRS_ORCT)
		{
//			cy=(cr+2*cg+cb)/4;
			cy=cg+(cb+cr-2*cg)/4;
			cu=(cb-cg)+128;
			cv=(cr-cg)+128;
			cw=ca;
		}else if(ctx->jpg_clrtrans==PDJPG_CLRS_RGB)
		{
			cy=cr; cu=cg; cv=cb; cw=ca;
		}else
		{
			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
			cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;
			cw=ca;
		}

//		cu=128; cv=128;

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);

		ctx->yb[i*xs2+j]=cy;
		ctx->ub[i*xs2+j]=cu;
		ctx->vb[i*xs2+j]=cv;
		if(ctx->jpg_yuvw)
			ctx->wb[i*xs2+j]=cw;
	}

}

PDGL_API int PDJPG_EncodeCtx_DownSample(PDJPG_Context *ctx)
{
	int xs2, ys2, xs3, ys3;
	int i, j, k, l;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

//	if((xs3!=xs2) || (ys3!=ys2))
	if(ctx->jpg_is420)
	{
		for(i=0; i<ys3; i++)
			for(j=0; j<xs3; j++)
		{
			k=ctx->ub[(i*2)*xs2+j*2]+
				ctx->ub[(i*2)*xs2+j*2+1]+
				ctx->ub[(i*2+1)*xs2+j*2]+
				ctx->ub[(i*2+1)*xs2+j*2+1];
			ctx->ub[i*xs3+j]=k/4;

			k=ctx->vb[(i*2)*xs2+j*2]+
				ctx->vb[(i*2)*xs2+j*2+1]+
				ctx->vb[(i*2+1)*xs2+j*2]+
				ctx->vb[(i*2+1)*xs2+j*2+1];
			ctx->vb[i*xs3+j]=k/4;

			if(ctx->jpg_yuvw)
			{
				k=ctx->wb[(i*2)*xs2+j*2]+
					ctx->wb[(i*2)*xs2+j*2+1]+
					ctx->wb[(i*2+1)*xs2+j*2]+
					ctx->wb[(i*2+1)*xs2+j*2+1];
				ctx->wb[i*xs3+j]=k/4;
			}
		}
	}
}

PDGL_API int PDJPG_EncodeCtx(
	PDJPG_Context *ctx, byte *ibuf, byte *obuf, int xs, int ys,
	int qfl, int pf)
{
	int i;
	PDJPG_EncodeCtx_Prepare(ctx, xs, ys, qfl, pf);
	PDJPG_EncodeCtx_ColorTransform(ctx, ibuf, xs, ys, pf);
	PDJPG_EncodeCtx_DownSample(ctx);
	i=PDJPG_EncodeCtx_Inner(ctx, obuf, xs, ys, qfl);
	return(i);
}

PDGL_API int PDJPG_EncodeCtxSS(
	PDJPG_Context *ctx, short *ibuf, byte *obuf, int xs, int ys,
	int qfl, int pf)
{
	int i;
	PDJPG_EncodeCtx_Prepare(ctx, xs, ys, qfl, pf);
	PDJPG_EncodeCtx_ColorTransform16(ctx, ibuf, xs, ys, pf);
	PDJPG_EncodeCtx_DownSample(ctx);
	i=PDJPG_EncodeCtx_Inner(ctx, obuf, xs, ys, qfl);
	return(i);
}

PDGL_API int PDJPG_EncodeCtx_Inner(
	PDJPG_Context *ctx, byte *obuf, int xs, int ys, int qfl)
{
//	static byte *yb=NULL, *ub, *vb;
//	static short *ydb=NULL, *udb, *vdb;
//	static int lxs=0, lys=0;

	int dcs[256], acs[256];
	int dcsuv[256], acsuv[256];
	short *tp;
	int xs2, ys2, xs3, ys3, qf;
	int cr, cg, cb, cy, cu, cv;
	int i, j, k, l;

	qf=qfl&255;

	xs2=ctx->xs2;
	ys2=ctx->ys2;
	xs3=ctx->xs3;
	ys3=ctx->ys3;

//	printf("M1\n");

//	if(qf==110)
	if(ctx->jpg_rdct)
	{
//		printf("PDJPG_EncodeCtx: Lossless Detect\n");
	
		PDJPG_FilterImageRDCT(ctx->yb, ctx->ydb, xs2, ys2, 128);
		PDJPG_FilterImageRDCT(ctx->ub, ctx->udb, xs3, ys3, 128);
		PDJPG_FilterImageRDCT(ctx->vb, ctx->vdb, xs3, ys3, 128);
	}else
	{
		PDJPG_FilterImageDCT(ctx->yb, ctx->ydb, xs2, ys2, 128);
		PDJPG_FilterImageDCT(ctx->ub, ctx->udb, xs3, ys3, 128);
		PDJPG_FilterImageDCT(ctx->vb, ctx->vdb, xs3, ys3, 128);
	}

	j=(xs2/8)*(ys2/8);
	PDJPG_MakeQuantTabInput(ctx->ydb, j, ctx->jpg_qt[0], qf/100.0);

	j=(xs3/8)*(ys3/8);
	PDJPG_MakeQuantTabInput(ctx->udb, j, ctx->jpg_qt[1], qf/100.0);
	PDJPG_MakeQuantTabInput(ctx->vdb, j, ctx->jpg_qt[2], qf/100.0);
	for(i=0; i<64; i++)ctx->jpg_qt[1][i]=(ctx->jpg_qt[1][i]+ctx->jpg_qt[2][i])/2;

	PDJPG_SetupQuantTabDivFP(ctx, 0);
	PDJPG_SetupQuantTabDivFP(ctx, 1);

//	free(yb);
//	free(ub);
//	free(vb);


	if(ctx->jpg_mono || !ctx->jpg_is420)
	{
		j=(xs2/8)*(ys2/8); k=0;
		for(i=0; i<j; i++)
		{
			PDJHUFF_QuantBlock(ctx, ctx->ydb+i*64, ctx->ydb+i*64, 0);
			ctx->ydb[i*64+0]-=k; k=ctx->ydb[i*64+0]+k;
		}
	}

	if(!ctx->jpg_mono && ctx->jpg_is420)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
		}
	}

#if 0
	if(!ctx->jpg_mono && !ctx->jpg_is420)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			PDJHUFF_QuantBlock(ctx, ctx->ydb+i*64, ctx->ydb+i*64, 1);
			ctx->ydb[i*64+0]-=l; l=ctx->ydb[i*64+0]+l;
		}
	}
#endif

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		PDJHUFF_QuantBlock(ctx, ctx->udb+i*64, ctx->udb+i*64, 1);
		PDJHUFF_QuantBlock(ctx, ctx->vdb+i*64, ctx->vdb+i*64, 1);
		ctx->udb[i*64+0]-=k; k=ctx->udb[i*64+0]+k;
		ctx->vdb[i*64+0]-=l; l=ctx->vdb[i*64+0]+l;
	}

	if(ctx->jpg_yuvw)
	{
		j=(xs3/8)*(ys3/8); k=0;
		for(i=0; i<j; i++)
		{
			PDJHUFF_QuantBlock(ctx, ctx->wdb+i*64, ctx->wdb+i*64, 1);
			ctx->wdb[i*64+0]-=k; k=ctx->wdb[i*64+0]+k;
		}
	}

//	printf("M2\n");

	for(i=0; i<256; i++)dcs[i]=0;
	for(i=0; i<256; i++)acs[i]=0;
	for(i=0; i<256; i++)dcsuv[i]=0;
	for(i=0; i<256; i++)acsuv[i]=0;

	j=(xs2/8)*(ys2/8);
	k=(xs3/8)*(ys3/8);
	for(i=0; i<j; i++)PDJHUFF_StatBlock(ctx->ydb+i*64, dcs, acs);
	for(i=0; i<k; i++)PDJHUFF_StatBlock(ctx->udb+i*64, dcsuv, acsuv);
	for(i=0; i<k; i++)PDJHUFF_StatBlock(ctx->vdb+i*64, dcsuv, acsuv);
	
	if(ctx->jpg_yuvw)
		{ for(i=0; i<k; i++)PDJHUFF_StatBlock(ctx->wdb+i*64, dcsuv, acsuv); }

//	PDJHUFF_BuildLengths(dcs, 256, ctx->huff_len+0*256, 16);
//	PDJHUFF_BuildLengths(acs, 256, ctx->huff_len+1*256, 16);
//	PDJHUFF_BuildLengths(dcsuv, 256, ctx->huff_len+2*256, 16);
//	PDJHUFF_BuildLengths(acsuv, 256, ctx->huff_len+3*256, 16);

	PDJHUFF_BuildLengthsAdjust(dcs, 256, ctx->huff_len+0*256, 16);
	PDJHUFF_BuildLengthsAdjust(acs, 256, ctx->huff_len+1*256, 16);
	PDJHUFF_BuildLengthsAdjust(dcsuv, 256, ctx->huff_len+2*256, 16);
	PDJHUFF_BuildLengthsAdjust(acsuv, 256, ctx->huff_len+3*256, 16);

	pdjhuff_ct=obuf;
	pdjhuff_win=0;
	pdjhuff_pos=0;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOI;

	PDJPG_EmitMarkerBCST(ctx);

	PDJPG_EmitDQT(ctx, 0);
	if(!ctx->jpg_mono)PDJPG_EmitDQT(ctx, 1);

	PDJPG_EmitSOF(ctx, xs, ys);

	PDJPG_EmitDHT(ctx, 0);
	PDJPG_EmitDHT(ctx, 1);
	if(!ctx->jpg_mono)
	{
		PDJPG_EmitDHT(ctx, 2);
		PDJPG_EmitDHT(ctx, 3);
	}

	PDJPG_EmitSOS(ctx);

	pdjhuff_win=0;
	pdjhuff_pos=0;

	if(ctx->jpg_mono)
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)PDJHUFF_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1);
	}else if(ctx->jpg_is420)
	{
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64, 0, 1);

			k=i*(xs3/8)+j;
			PDJHUFF_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
			PDJHUFF_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
			if(ctx->jpg_yuvw)
				{ PDJHUFF_EncodeBlock(ctx, ctx->wdb+k*64, 2, 3); }
		}
	}else
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)
		{
			PDJHUFF_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx, ctx->udb+i*64, 2, 3);
			PDJHUFF_EncodeBlock(ctx, ctx->vdb+i*64, 2, 3);
			if(ctx->jpg_yuvw)
				{ PDJHUFF_EncodeBlock(ctx, ctx->wdb+i*64, 2, 3); }
		}

#if 0
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			k=i*(xs3/8)+j;
			PDJHUFF_EncodeBlock(ctx, ctx->ydb+k*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
			PDJHUFF_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
		}
#endif
	}

	PDJHUFF_FlushBits(ctx);

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_EOI;

//	free(ydb);
//	free(udb);
//	free(vdb);

//	printf("M3\n");

	i=pdjhuff_ct-obuf;

	return(i);
}

PDGL_API PDJPG_Context *PDJPG_AllocContext()
{
	PDJPG_Context *ctx;

	PDJPG_Init();
	ctx=malloc(sizeof(PDJPG_Context));
	memset(ctx, 0, sizeof(PDJPG_Context));
	return(ctx);
}

PDGL_API void PDJPG_FreeContext(PDJPG_Context *ctx)
{
	if(ctx->yb)
	{
		free(ctx->yb);
		free(ctx->ub);
		free(ctx->vb);
		free(ctx->ydb);
		free(ctx->udb);
		free(ctx->vdb);
	}

	if(ctx->jpg_imgbuf)
		free(ctx->jpg_imgbuf);

	if(ctx->imgbuf_rgba)
		free(ctx->imgbuf_rgba);
	if(ctx->imgbuf_norm)
		free(ctx->imgbuf_norm);
	if(ctx->imgbuf_spec)
		free(ctx->imgbuf_spec);
	if(ctx->imgbuf_luma)
		free(ctx->imgbuf_luma);
	if(ctx->imgbuf_lebe)
		free(ctx->imgbuf_lebe);

	free(ctx);
}

PDGL_API int PDJPG_Encode(
	byte *ibuf, byte *obuf, int xs, int ys, int qf)
{
	PDJPG_Context *ctx;
	int sz;
	
	ctx=PDJPG_AllocContext();
	sz=PDJPG_EncodeCtx(ctx, ibuf, obuf, xs, ys, qf, 0);
	PDJPG_FreeContext(ctx);
	return(sz);
}

PDGL_API void PDJPG_FlushEncodeFast(PDJPG_Context *ctx)
{
	if(!ctx)return;
	ctx->jpg_tabcacheframe=0;	//force rebuild
	ctx->oldAlphaClr=0;
}

PDGL_API void PDJPG_SetContextAlphaColor(PDJPG_Context *ctx,
	int cr, int cg, int cb, int ca)
{
	int cy, cu, cv;
	if(!ctx)return;

	if(ca<1)
	{
		ctx->alphaClr=0;
		return;
	}

	ctx->alphaClr=cr+(cg<<8)+(cb<<16)+(ca<<24);

	cy= 19595*cr +38470*cg + 7471*cb;
	cu=-11056*cr -21712*cg +32768*cb;
	cv= 32768*cr -27440*cg - 5328*cb;
	ctx->alphaClrY=cy>>16;
	ctx->alphaClrU=(cu>>16)+128;
	ctx->alphaClrV=(cv>>16)+128;
	ctx->alphaClrA=ca;
}

#if 0
//static void pdjpg_getPixel2_RGBA(byte *rgb,
//		int *ra, int *ga, int *ba,
//		int *rb, int *gb, int *bb)
//{
//	*ra=rgb[0]; *ga=rgb[1]; *ba=rgb[2];
//	*ra=rgb[4]; *ga=rgb[5]; *ba=rgb[6];
//}
#endif

void PDJPG_ConvertImageYUV(PDJPG_Context *ctx,
	byte *ibuf, int xs, int ys, int pf,
	int xs2, int ys2, int xs3, int ys3)
{
//	void (*getPixel2)(byte *rgb,
//		int *ra, int *ga, int *ba,
//		int *rb, int *gb, int *bb);
	byte *cs, *cse;
	short *cty, *ctu, *ctv;
	byte *cs1, *cs2;
	short *cty1, *cty2;

	int cr, cg, cb, ca, cy, cu, cv;

	int cra, cga, cba, caa, cya, cua, cva;
	int crb, cgb, cbb, cab, cyb, cub, cvb;
	int crc, cgc, cbc, cac, cyc, cuc, cvc;
	int crd, cgd, cbd, cad, cyd, cud, cvd;
	int psz, psz2;

	int i, j, k, l, n;

	if((pf==PDJPG_RGBA) && !ctx->alphaClr)
	{
		n=(ys+1)/2;
		for(i=0; i<n; i++)
		{
			k=((ys-(2*i+1))*xs)*4;
			cs1=ibuf+k; cse=cs1+xs*4;
			l=((ys-(2*i+2))*xs)*4;
			cs2=ibuf+l;

			cty1=ctx->yb+(2*i+0)*xs2;
			cty2=ctx->yb+(2*i+1)*xs2;

			l=i*xs3;
			ctu=ctx->ub+l;
			ctv=ctx->vb+l;

			while(cs1<cse)
			{
				cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2);
				crb=*(cs1+4); cgb=*(cs1+5); cbb=*(cs1+6);
				crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2);
				crd=*(cs2+4); cgd=*(cs2+5); cbd=*(cs2+6);

				cya=19595*cra + 38470*cga + 7471*cba;
				cyb=19595*crb + 38470*cgb + 7471*cbb;
				cyc=19595*crc + 38470*cgc + 7471*cbc;
				cyd=19595*crd + 38470*cgd + 7471*cbd;
				*cty1++=cya>>16;
				*cty1++=cyb>>16;
				*cty2++=cyc>>16;
				*cty2++=cyd>>16;

				cr=(cra+crb+crc+crd)>>2;
				cg=(cga+cgb+cgc+cgd)>>2;
				cb=(cba+cbb+cbc+cbd)>>2;

				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				*ctu++=(cu>>16)+128;
				*ctv++=(cv>>16)+128;

				cs1+=8; cs2+=8;
			}
		}
	}else
	if((pf==PDJPG_RGBA) || (pf==PDJPG_RGB) ||
		(pf==PDJPG_BGRA) || (pf==PDJPG_BGR))
	{
		switch(pf)
		{
		case PDJPG_RGBA: case PDJPG_BGRA:
			psz=4; break;
		case PDJPG_RGB: case PDJPG_BGR:
			psz=3; break;
		default: psz=4; break;
		}
		psz2=psz*2;

		n=(ys+1)/2;
		for(i=0; i<n; i++)
		{
			k=((ys-(2*i+1))*xs)*psz;
			cs1=ibuf+k; cse=cs1+xs*psz;
			l=((ys-(2*i+2))*xs)*psz;
			cs2=ibuf+l;

			cty1=ctx->yb+(2*i+0)*xs2;
			cty2=ctx->yb+(2*i+1)*xs2;

			l=i*xs3;
			ctu=ctx->ub+l;
			ctv=ctx->vb+l;

			while(cs1<cse)
			{
//				getPixel2(cs1, &cra, &cga, &cba, &crb, &cgb, &cbb);
//				getPixel2(cs2, &crc, &cgc, &cbc, &crd, &cgd, &cbd);

				switch(pf)
				{
				case PDJPG_RGBA:
					cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2); caa=*(cs1+3);
					crb=*(cs1+4); cgb=*(cs1+5); cbb=*(cs1+6); cab=*(cs1+7);
					crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2); cac=*(cs2+3);
					crd=*(cs2+4); cgd=*(cs2+5); cbd=*(cs2+6); cad=*(cs2+7);
					psz=8; break;
				case PDJPG_BGRA:
					cra=*(cs1+2); cga=*(cs1+1); cba=*(cs1+0); caa=*(cs1+3);
					crb=*(cs1+6); cgb=*(cs1+5); cbb=*(cs1+4); cab=*(cs1+7);
					crc=*(cs2+2); cgc=*(cs2+1); cbc=*(cs2+0); cac=*(cs2+3);
					crd=*(cs2+6); cgd=*(cs2+5); cbd=*(cs2+4); cad=*(cs2+7);
					psz=8; break;
				case PDJPG_RGB:
					cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2); caa=255;
					crb=*(cs1+3); cgb=*(cs1+4); cbb=*(cs1+5); cab=255;
					crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2); cac=255;
					crd=*(cs2+3); cgd=*(cs2+4); cbd=*(cs2+5); cad=255;
					psz=6; break;
				case PDJPG_BGR:
					cra=*(cs1+2); cga=*(cs1+1); cba=*(cs1+0); caa=255;
					crb=*(cs1+5); cgb=*(cs1+4); cbb=*(cs1+3); cab=255;
					crc=*(cs2+2); cgc=*(cs2+1); cbc=*(cs2+0); cac=255;
					crd=*(cs2+5); cgd=*(cs2+4); cbd=*(cs2+3); cad=255;
					psz=6; break;
				}

				if(ctx->alphaClr)
				{
					ca=(caa+cab+cac+cad)>>2;
					if(ca<ctx->alphaClrA)
					{
						cy=ctx->alphaClrY;
						cu=ctx->alphaClrU;
						cv=ctx->alphaClrV;
						*cty1++=cy; *cty1++=cy;
						*cty2++=cy; *cty2++=cy;
						*ctu++=cu; *ctv++=cv;
						continue;
					}
				}

				cya=19595*cra + 38470*cga + 7471*cba;
				cyb=19595*crb + 38470*cgb + 7471*cbb;
				cyc=19595*crc + 38470*cgc + 7471*cbc;
				cyd=19595*crd + 38470*cgd + 7471*cbd;
				*cty1++=cya>>16;
				*cty1++=cyb>>16;
				*cty2++=cyc>>16;
				*cty2++=cyd>>16;

				cr=(cra+crb+crc+crd)>>2;
				cg=(cga+cgb+cgc+cgd)>>2;
				cb=(cba+cbb+cbc+cbd)>>2;

				cu=-11056*cr -21712*cg +32768*cb;
				cv= 32768*cr -27440*cg - 5328*cb;
				*ctu++=(cu>>16)+128;
				*ctv++=(cv>>16)+128;

				cs1+=psz2; cs2+=psz2;
			}
		}
	}else
	{
		switch(pf)
		{
		case PDJPG_YUVA: psz=4; psz2=8; break;
		case PDJPG_YUV: psz=3; psz2=6; break;
		case PDJPG_YUV422: psz=2; psz2=4; break;
		case PDJPG_YUV420: psz=3; psz2=3; break;
		case PDJPG_YA: psz=2; psz2=4; break;
		case PDJPG_Y: psz=1; psz2=2; break;
		case PDJPG_YYYA: psz=4; psz2=8; break;
		default: psz=4; psz2=8; break;
		}

		n=(ys+1)/2;
		for(i=0; i<n; i++)
		{
			if(pf==PDJPG_YUV420)
			{
				k=(((ys-(2*i+1))*xs2)*psz2)>>1;
				cs1=ibuf+k; cse=cs1+((xs*psz2)>>1);
				l=(((ys-(2*i+2))*xs2)*psz2)>>1;
				cs2=ibuf+l;
			}else
			{
				k=((ys-(2*i+1))*xs)*psz;
				cs1=ibuf+k; cse=cs1+xs*psz;
				l=((ys-(2*i+2))*xs)*psz;
				cs2=ibuf+l;
			}

			cty1=ctx->yb+(2*i+0)*xs2;
			cty2=ctx->yb+(2*i+1)*xs2;

			l=i*xs3;
			ctu=ctx->ub+l;
			ctv=ctx->vb+l;

			while(cs1<cse)
			{
//				getPixel2(cs1, &cra, &cga, &cba, &crb, &cgb, &cbb);
//				getPixel2(cs2, &crc, &cgc, &cbc, &crd, &cgd, &cbd);

				switch(pf)
				{
				case PDJPG_YUVA:
					cya=*(cs1+0); cua=*(cs1+1); cva=*(cs1+2);
					cyb=*(cs1+4); cub=*(cs1+5); cvb=*(cs1+6);
					cyc=*(cs2+0); cuc=*(cs2+1); cvc=*(cs2+2);
					cyd=*(cs2+4); cud=*(cs2+5); cvd=*(cs2+6);
					cu=(cua+cub+cuc+cud)>>2;
					cv=(cva+cvb+cvc+cvd)>>2;
					break;
				case PDJPG_YUV:
					cya=*(cs1+0); cua=*(cs1+1); cva=*(cs1+2);
					cyb=*(cs1+3); cub=*(cs1+4); cvb=*(cs1+5);
					cyc=*(cs2+0); cuc=*(cs2+1); cvc=*(cs2+2);
					cyd=*(cs2+3); cud=*(cs2+4); cvd=*(cs2+5);
					cu=(cua+cub+cuc+cud)>>2;
					cv=(cva+cvb+cvc+cvd)>>2;
					break;
				case PDJPG_YUV422:
					cya=*(cs1+0); cua=*(cs1+1); cyb=*(cs1+2); cva=*(cs1+3);
					cyc=*(cs2+0); cuc=*(cs2+1); cyd=*(cs2+2); cvc=*(cs2+3);
					cu=(cua+cuc)>>1; cv=(cva+cvc)>>1;
					break;
				case PDJPG_YUV420:
					cya=*(cs1+0); cyb=*(cs1+1); cu=*(cs1+2);
					cyc=*(cs2+0); cyd=*(cs2+1); cv=*(cs2+2);
					break;
				case PDJPG_YA:
					cya=*(cs1+0); cyb=*(cs1+2);
					cyc=*(cs2+0); cyd=*(cs2+2);
					cu=0; cv=0;
					break;
				case PDJPG_Y:
					cya=*(cs1+0); cyb=*(cs1+1);
					cyc=*(cs2+0); cyd=*(cs2+1);
					cu=0; cv=0;
					break;

				case PDJPG_YYYA:
					cya=*(cs1+1); cyb=*(cs1+5);
					cyc=*(cs2+1); cyd=*(cs2+5);
					cu=0; cv=0;
					break;
				}

				*cty1++=cya; *cty1++=cyb;
				*cty2++=cyc; *cty2++=cyd;
				*ctu++=cu; *ctv++=cv;

				cs1+=psz2; cs2+=psz2;
			}
		}
	}
}

PDGL_API int PDJPG_EncodeFastCtx(PDJPG_Context *ctx,
	byte *ibuf, byte *obuf, int xs, int ys, int qf, int pf)
{
	char tb[256];
	short *tp;
//	byte *cs, *cse, *cty, *ctu, *ctv;
	int xs2, ys2, xs3, ys3;
//	int cr, cg, cb, cy, cu, cv;

//	int cra, cga, cba, cya;
//	int crb, cgb, cbb, cyb;
//	int crc, cgc, cbc, cyc;
//	int crd, cgd, cbd, cyd;
//	byte *cs1, *cs2, *cty1, *cty2;
	byte *ctt;

	int i, j, k, l, n;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;
	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;

	ctx->jpg_mono=0;

	if(!ctx->jpg_tabcacheframe)
	{
		//full quality
		for(i=0; i<64; i++)ctx->jpg_qt[0][i]=1;
		for(i=0; i<64; i++)ctx->jpg_qt[1][i]=1;
	}

	if(!ctx->yb || (xs!=ctx->lxs) || (ys!=ctx->lys))
	{
		if(ctx->yb)
		{
			free(ctx->yb);
			free(ctx->ub);
			free(ctx->vb);
			free(ctx->ydb);
			free(ctx->udb);
			free(ctx->vdb);
		}

		ctx->yb=malloc(xs2*ys2);
		ctx->ub=malloc(xs2*ys2);
		ctx->vb=malloc(xs2*ys2);

		ctx->ydb=malloc((xs2+8)*(ys2+16)*sizeof(short));
		ctx->udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		ctx->vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));

		ctx->lxs=xs;
		ctx->lys=ys;

		memset(ctx->yb, 128, xs2*ys2);
		memset(ctx->ub, 128, xs2*ys2);
		memset(ctx->vb, 128, xs2*ys2);

		memset(ctx->ydb, 0, xs2*(ys2+8)*sizeof(short));
		memset(ctx->udb, 0, xs3*(ys3+8)*sizeof(short));
		memset(ctx->vdb, 0, xs3*(ys3+8)*sizeof(short));

		ctx->jpg_tabcacheframe=0;	//force rebuild
	}

	PDJPG_ConvertImageYUV(ctx,
		ibuf, xs, ys, pf,
		xs2, ys2, xs3, ys3);

	PDJPG_FilterImageDCT(ctx->yb, ctx->ydb, xs2, ys2, 128);
	PDJPG_FilterImageDCT(ctx->ub, ctx->udb, xs3, ys3, 128);
	PDJPG_FilterImageDCT(ctx->vb, ctx->vdb, xs3, ys3, 128);

	if(ctx->jpg_tabcacheframe<=0)
	{
		j=(xs2/8)*(ys2/8);
		PDJPG_MakeQuantTabInputFast(ctx->ydb, j, ctx->jpg_qt[0], qf/100.0);

		j=(xs3/8)*(ys3/8);
		PDJPG_MakeQuantTabInputFast(ctx->udb, j, ctx->jpg_qt[1], qf/100.0);
		PDJPG_MakeQuantTabInputFast(ctx->vdb, j, ctx->jpg_qt[2], qf/100.0);
		for(i=0; i<64; i++)
			ctx->jpg_qt[1][i]=(ctx->jpg_qt[1][i]+ctx->jpg_qt[2][i])/2;

		PDJPG_SetupQuantTabDivFP(ctx, 0);
		PDJPG_SetupQuantTabDivFP(ctx, 1);
	}

	if(ctx->jpg_mono)
	{
		j=(xs2/8)*(ys2/8); k=0;
		for(i=0; i<j; i++)
		{
			PDJHUFF_QuantBlock(ctx, ctx->ydb+i*64, ctx->ydb+i*64, 0);
			ctx->ydb[i*64+0]-=k;
			k=ctx->ydb[i*64+0]+k;
		}
	}

	if(!ctx->jpg_mono)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(ctx, tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
		}
	}

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		PDJHUFF_QuantBlock(ctx, ctx->udb+i*64, ctx->udb+i*64, 1);
		PDJHUFF_QuantBlock(ctx, ctx->vdb+i*64, ctx->vdb+i*64, 1);
		ctx->udb[i*64+0]-=k; k=ctx->udb[i*64+0]+k;
		ctx->vdb[i*64+0]-=l; l=ctx->vdb[i*64+0]+l;
	}

//	printf("M2\n");

	if(ctx->jpg_tabcacheframe<=0)
	{
		for(i=0; i<256; i++)ctx->dcs[i]=1;
		for(i=0; i<256; i++)ctx->acs[i]=1;
		for(i=0; i<256; i++)ctx->dcsuv[i]=1;
		for(i=0; i<256; i++)ctx->acsuv[i]=1;

		j=(xs2/8)*(ys2/8);
		k=(xs3/8)*(ys3/8);
		for(i=0; i<j; i++)
			PDJHUFF_StatBlock(ctx->ydb+i*64, ctx->dcs, ctx->acs);
		for(i=0; i<k; i++)
			PDJHUFF_StatBlock(ctx->udb+i*64, ctx->dcsuv, ctx->acsuv);
		for(i=0; i<k; i++)
			PDJHUFF_StatBlock(ctx->vdb+i*64, ctx->dcsuv, ctx->acsuv);

//		PDJHUFF_BuildLengths(ctx->dcs, 256, ctx->huff_len+0*256, 16);
//		PDJHUFF_BuildLengths(ctx->acs, 256, ctx->huff_len+1*256, 16);
//		PDJHUFF_BuildLengths(ctx->dcsuv, 256, ctx->huff_len+2*256, 16);
//		PDJHUFF_BuildLengths(ctx->acsuv, 256, ctx->huff_len+3*256, 16);

		PDJHUFF_BuildLengthsAdjust(
			ctx->dcs, 256, ctx->huff_len+0*256, 16);
		PDJHUFF_BuildLengthsAdjust(
			ctx->acs, 256, ctx->huff_len+1*256, 16);
		PDJHUFF_BuildLengthsAdjust(
			ctx->dcsuv, 256, ctx->huff_len+2*256, 16);
		PDJHUFF_BuildLengthsAdjust(
			ctx->acsuv, 256, ctx->huff_len+3*256, 16);
	}

	pdjhuff_ct=obuf;
	pdjhuff_win=0;
	pdjhuff_pos=0;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOI;

	if(ctx->alphaClr && (ctx->alphaClr!=ctx->oldAlphaClr))
	{
		sprintf(tb, "%d %d %d %d",
			(ctx->alphaClr&0xFF),
			((ctx->alphaClr>>8)&0xFF),
			((ctx->alphaClr>>16)&0xFF),
			((ctx->alphaClr>>24)&0xFF));
	
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP11;
		ctt=pdjhuff_ct;
		*pdjhuff_ct++=0x00;
		*pdjhuff_ct++=0x00;
		PDJHUFF_WriteString(ctx, "AlphaColor");
		PDJHUFF_WriteString(ctx, tb);
		i=(pdjhuff_ct-ctt);
		ctt[0]=(i>>8)&0xFF; ctt[1]=i&0xFF;
	}


	if(ctx->jpg_tabcacheframe<=0)
	{
		PDJPG_EmitDQT(ctx, 0);
		if(!ctx->jpg_mono)PDJPG_EmitDQT(ctx, 1);
	}

	PDJPG_EmitSOF(ctx, xs, ys);

//	if(ctx->jpg_tabcacheframe<=0)
	if(1)
	{
		PDJPG_EmitDHT(ctx, 0);
		PDJPG_EmitDHT(ctx, 1);
		if(!ctx->jpg_mono)
		{
			PDJPG_EmitDHT(ctx, 2);
			PDJPG_EmitDHT(ctx, 3);
		}
	}

	PDJPG_EmitSOS(ctx);

	pdjhuff_win=0;
	pdjhuff_pos=0;

	if(ctx->jpg_mono)
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)PDJHUFF_EncodeBlock(ctx, ctx->ydb+i*64, 0, 1);
	}else
	{
		for(i=0; i<=((ys3)/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+0)*(xs2/8)+j*2+1)*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ctx,
				ctx->ydb+((i*2+1)*(xs2/8)+j*2+1)*64, 0, 1);

			k=i*(xs3/8)+j;
			PDJHUFF_EncodeBlock(ctx, ctx->udb+k*64, 2, 3);
			PDJHUFF_EncodeBlock(ctx, ctx->vdb+k*64, 2, 3);
		}
	}

	PDJHUFF_FlushBits(ctx);

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_EOI;

	if(ctx->jpg_tabcacheframe<=0)
	{
//		ctx->jpg_tabcacheframe=16;
		ctx->jpg_tabcacheframe=8;
	}else
	{
		ctx->jpg_tabcacheframe--;
	}

//	printf("M3\n");

	i=pdjhuff_ct-obuf;

	return(i);
}

PDGL_API int PDJPG_EncodeFast(
	byte *ibuf, byte *obuf, int xs, int ys, int qf)
{
	static PDJPG_Context *ctx=NULL;
	int sz;
	
	if(!ctx)ctx=PDJPG_AllocContext();
	sz=PDJPG_EncodeFastCtx(ctx, ibuf, obuf, xs, ys, qf, 0);
//	PDJPG_FreeContext(ctx);
	return(sz);
}

byte *PDJPG_EmitComponentLayer(PDJPG_Context *ctx,
	byte *ct, char *name)
{
	byte *ctt;
	int i;

	pdjhuff_ct=ct; pdjhuff_win=0; pdjhuff_pos=0;
	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_APP11;
	ctt=pdjhuff_ct;
	*pdjhuff_ct++=0x00;
	*pdjhuff_ct++=0x00;
	PDJHUFF_WriteString(ctx, "CompLayer");
	PDJHUFF_WriteString(ctx, name);
	i=pdjhuff_ct-ctt;
	ctt[0]=(i>>8)&0xFF; ctt[1]=i&0xFF;
	return(pdjhuff_ct);
}

byte *PDJPG_EmitMarkerJFIF(PDJPG_Context *ctx, byte *ct)
{
	byte *ctt;
	int i;

	if(ctx->jpg_clrtrans!=PDJPG_CLRS_YCBCR)
		return(ct);

	pdjhuff_ct=ct; pdjhuff_win=0; pdjhuff_pos=0;
	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_APP0;
	ctt=pdjhuff_ct;
	*pdjhuff_ct++=0x00;
	*pdjhuff_ct++=0x00;
	PDJHUFF_WriteString(ctx, "JFIF");

	*pdjhuff_ct++=0x01;		//version high
	*pdjhuff_ct++=0x02;		//version low

	*pdjhuff_ct++=0x00;		//no units

	*pdjhuff_ct++=0x00;		//X density
	*pdjhuff_ct++=0x01;

	*pdjhuff_ct++=0x00;		//Y density
	*pdjhuff_ct++=0x01;

	*pdjhuff_ct++=0x00;		//thumbnail
	*pdjhuff_ct++=0x00;

//	PDJHUFF_WriteString(ctx, name);
	i=pdjhuff_ct-ctt;
	ctt[0]=(i>>8)&0xFF; ctt[1]=i&0xFF;
	return(pdjhuff_ct);
}

PDGL_API int PDJPG_EscapeEncodeBuffer(byte *ibuf, int isz,
	byte *obuf, int osz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	
	while((cs<cse) && (ct<cte))
	{
		if(*cs==0xFF)
		{
			cs++;
			*ct++=0xFF;
			*ct++=0x00;
			continue;
		}
		
		*ct++=*cs++;
	}
	
	if(ct>=cte)return(-1);
	return(ct-obuf);
}

PDGL_API int PDJPG_EscapeEncodeSingleBuffer(byte *buf, int sz)
{
	byte *tbuf;
	int i;
	
	tbuf=malloc(sz*2);
	i=PDJPG_EscapeEncodeBuffer(buf, sz, tbuf, sz*2);
	if(i<0)return(i);
	memcpy(buf, tbuf, i);
	free(tbuf);
	return(i);
}

PDGL_API int PDJPG_EscapeDecodeBuffer(byte *ibuf, int isz,
	byte *obuf, int osz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	
	while((cs<cse) && (ct<cte))
	{
		if((cs[0]==0xFF) && (cs[1]==0x00))
			{ cs+=2; *ct++=0xFF; continue; }
		*ct++=*cs++;
	}
	if(ct>=cte)return(-1);
	return(ct-obuf);
}

PDGL_API int PDJPG_EscapeDecodeSingleBuffer(byte *buf, int sz)
{
	byte *cs, *ct, *cse, *cte;
	
	cs=buf; cse=buf+sz;
	ct=buf; cte=buf+sz;
	
	while((cs<cse) && (ct<cte))
	{
		if((cs[0]==0xFF) && (cs[1]==0x00))
			{ cs+=2; *ct++=0xFF; continue; }
		*ct++=*cs++;
	}
	if(ct>=cte)return(-1);
	return(ct-buf);
}

PDGL_API int PDJPG_EncodeLDatCtx(
	PDJPG_Context *ctx, byte *ibuf, byte *obuf,
	int xs, int ys, int qf, int pf)
{
	byte *tbuf, *cts;
	int i, j, sz, tsz;
	
//	if(!ctx)ctx=PDJPG_AllocContext();

	tbuf=malloc(1<<20);
	sz=PDJPG_EncodeCtx(ctx, ibuf, tbuf, xs, ys, qf, pf);

	sz=PDJPG_EscapeEncodeSingleBuffer(tbuf, sz);

	cts=tbuf; tsz=sz;
	pdjhuff_ct=obuf; pdjhuff_win=0; pdjhuff_pos=0;
	while(tsz>=65528)
	{
		i=65529;
		if(cts[i-1]==0xFF)i--;
		j=i+6;
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP12;
		*pdjhuff_ct++=(j>>8)&0xFF;
		*pdjhuff_ct++=j&0xFF;

		*pdjhuff_ct++='L';
		*pdjhuff_ct++='D';
		*pdjhuff_ct++='A';
		*pdjhuff_ct++='T';
		
		memcpy(pdjhuff_ct, cts, i);
		cts+=i; pdjhuff_ct+=i;
		tsz-=i;
	}
	
	if((tsz>0) && (tsz<65528))
	{
		i=tsz+6;
		*pdjhuff_ct++=0xFF;
		*pdjhuff_ct++=JPG_APP12;
		*pdjhuff_ct++=(i>>8)&0xFF;
		*pdjhuff_ct++=i&0xFF;

		*pdjhuff_ct++='L';
		*pdjhuff_ct++='D';
		*pdjhuff_ct++='A';
		*pdjhuff_ct++='T';
		
		memcpy(pdjhuff_ct, cts, tsz);
		pdjhuff_ct+=tsz;
	}

	sz=pdjhuff_ct-obuf;
	free(tbuf);
	
	return(sz);
}

PDGL_API int PDJPG_EncodeBaseCtx(
	PDJPG_Context *ctx, byte *ibuf, byte *obuf,
	int xs, int ys, int qf, int pf)
{
	byte *tbuf, *cs, *cse, *ct;
	int sz;

	tbuf=malloc(1<<20);
	sz=PDJPG_EncodeCtx(ctx, ibuf, tbuf, xs, ys, qf, pf);
	
	cs=tbuf; cse=tbuf+sz; ct=obuf;
	if((cs[0]==0xFF) && (cs[1]==JPG_SOI))
		cs+=2;
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==JPG_EOI))
			break;
		*ct++=*cs++;
	}
	
	free(tbuf);
	return(ct-obuf);
}

PDGL_API int PDJPG_EncodeComponentCtx(
	PDJPG_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int xs, int ys, int qf)
{
	return(PDJPG_EncodeComponentCtxI(ctx,
		rgba, norm, spec, luma, NULL,
		obuf, xs, ys, qf));
}

PDGL_API int PDJPG_EncodeComponentCtxI(
	PDJPG_Context *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma, byte *lebe,
	byte *obuf, int xs, int ys, int qf)
{
	byte *tbuf;
	byte *ct;
	int i, j, k, n;

	if(!rgba)return(-1);

	n=xs*ys;
	tbuf=malloc(xs*ys*4);

	ct=obuf;

	*ct++=0xFF;
	*ct++=JPG_SOI;

	ct=PDJPG_EmitMarkerJFIF(ctx, ct);

	ct=PDJPG_EmitComponentLayer(ctx, ct, "RGB");
//	i=PDJPG_EncodeCtx(ctx, rgba, ct, xs, ys, qf, 0);
	i=PDJPG_EncodeBaseCtx(ctx, rgba, ct, xs, ys, qf, 0);
	if(i<0) { free(tbuf); return(i); }
	ct+=i;
	
//	if((ct[-2]==0xFF) && (ct[-2]==JPG_EOI))
//		{ ct-=2; }
	
	if(norm)
	{
		for(i=0; i<n; i++)
		{
			tbuf[i*4+0]=norm[i*4+0];
			tbuf[i*4+1]=norm[i*4+2];
			tbuf[i*4+2]=norm[i*4+1];
			tbuf[i*4+3]=255;
		}

		ct=PDJPG_EmitComponentLayer(ctx, ct, "XYZ");
		i=PDJPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	if(spec)
	{
		ct=PDJPG_EmitComponentLayer(ctx, ct, "SpRGB");
		i=PDJPG_EncodeLDatCtx(ctx, spec, ct, xs, ys, qf, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	if(norm || spec)
	{
		for(i=0; i<n; i++)
		{
			j=rgba[i*4+3];
			tbuf[i*4+0]=norm?norm[i*4+3]:j;
			tbuf[i*4+1]=rgba[i*4+3];
			tbuf[i*4+2]=spec?spec[i*4+3]:j;
			tbuf[i*4+3]=255;
		}

		ct=PDJPG_EmitComponentLayer(ctx, ct, "DASe");
		i=PDJPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}else
	{
		k=0;
		for(i=0; i<n; i++)
		{
			j=rgba[i*4+3];
			if(j!=255)k=1;
			tbuf[i*4+0]=j;
			tbuf[i*4+1]=j;
			tbuf[i*4+2]=j;
			tbuf[i*4+3]=255;
		}

		if(k)
		{
			ct=PDJPG_EmitComponentLayer(ctx, ct, "Alpha");
			i=PDJPG_EncodeLDatCtx(ctx, tbuf, ct, xs, ys, qf, PDJPG_YYYA);
			if(i<0) { free(tbuf); return(i); }
			ct+=i;
		}
	}

	if(luma)
	{
		ct=PDJPG_EmitComponentLayer(ctx, ct, "LuRGB");
		i=PDJPG_EncodeLDatCtx(ctx, luma, ct, xs, ys, qf, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	if(lebe)
	{
		ct=PDJPG_EmitComponentLayer(ctx, ct, "LeBe");
		i=PDJPG_EncodeLDatCtx(ctx, lebe, ct, xs, ys, qf, 0);
		if(i<0) { free(tbuf); return(i); }
		ct+=i;
	}

	*ct++=0xFF;
	*ct++=JPG_EOI;

	free(tbuf);
	return(ct-obuf);
}

PDGL_API int PDJPG_EncodeComponent(
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int xs, int ys, int qf)
{
	PDJPG_Context *ctx;
	int sz;
	
	ctx=PDJPG_AllocContext();
	sz=PDJPG_EncodeComponentCtx(ctx,
		rgba, norm, spec, luma,
		obuf, xs, ys, qf);
	PDJPG_FreeContext(ctx);
	return(sz);
}

float pdjpg_ssqr(float f)
	{ return((f>=0)?(f*f):(-f*f)); }
float pdjpg_ssqrt(float f)
	{ return((f>=0)?sqrt(f):(-sqrt(-f))); }

int PDJPG_CalcComponentValueExponent(float val)
{
	float f, g;
	int i, j;

	if(val>=0)
	{
		f=log(val)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=192+i;
		return(j);
	}else
	{
		f=log(-val)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=63-i;
		return(j);
	}
}

void PDJPG_EncodeFloatPixel(float *ipix, byte *opix, byte *oexp)
{
	float f, g;
	int i, j, k;

	if((ipix[0]>=0) && (ipix[0]<=1) &&
		(ipix[1]>=0) && (ipix[1]<=1) &&
		(ipix[2]>=0) && (ipix[2]<=1))
	{
		*oexp=192;
		opix[0]=ipix[0]*255;
		opix[1]=ipix[1]*255;
		opix[2]=ipix[2]*255;
		return;
	}
	
	f=ipix[1];
	if(fabs(ipix[0])>fabs(f))f=ipix[0];
	if(fabs(ipix[2])>fabs(f))f=ipix[2];
	
	if(f>=0)
	{
		f=log(f)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=192+i;
		g=pdjpg_scltab[j];

		*oexp=j;
		f=255/g;
		k=ipix[0]*f; k=(k<0)?0:((k<=255)?k:255); opix[0]=k;
		k=ipix[1]*f; k=(k<0)?0:((k<=255)?k:255); opix[1]=k;
		k=ipix[2]*f; k=(k<0)?0:((k<=255)?k:255); opix[2]=k;
		return;
	}else
	{
		f=log(-f)/log(2);
		i=pdjpg_ssqrt(f*240)+0.5;
		if(i>63)i=63;
		if(i<(-63))i=-63;
		j=63-i;
		g=pdjpg_scltab[j];

		*oexp=j;
		f=255/g;
		k=ipix[0]*f; k=(k<0)?0:((k<=255)?k:255); opix[0]=k;
		k=ipix[1]*f; k=(k<0)?0:((k<=255)?k:255); opix[1]=k;
		k=ipix[2]*f; k=(k<0)?0:((k<=255)?k:255); opix[2]=k;
		return;
	}
}

PDGL_API int PDJPG_EncodeFloatComponentCtx(PDJPG_Context *ctx,
	float *rgba, float *norm, float *spec, float *luma,
	byte *obuf, int xs, int ys, int qf)
{
	byte *trgba, *tnorm, *tspec, *tluma, *tlebe;
	int i, j, n;
	float f, g;

	n=xs*ys;
	if(!ctx->imgbuf_rgba)
		ctx->imgbuf_rgba=malloc(4*n);
	if(!ctx->imgbuf_norm)
		ctx->imgbuf_norm=malloc(4*n);
	if(!ctx->imgbuf_spec)
		ctx->imgbuf_spec=malloc(4*n);
	if(!ctx->imgbuf_luma)
		ctx->imgbuf_luma=malloc(4*n);

	if(!ctx->imgbuf_lebe)
	{
		ctx->imgbuf_lebe=malloc(4*n);
		memset(ctx->imgbuf_lebe, 192, 4*n);
	}
	
	trgba=ctx->imgbuf_rgba;
	tnorm=ctx->imgbuf_norm;
	tspec=ctx->imgbuf_spec;
	tluma=ctx->imgbuf_luma;
	tlebe=ctx->imgbuf_lebe;
	
	if(rgba)
	{
		for(i=0; i<n; i++)
		{
			PDJPG_EncodeFloatPixel(rgba, trgba+i*4, tlebe+i*4+0);
		}
	}

	if(norm)
	{
		for(i=0; i<n; i++)
		{
//			f=(1.0/256)*pdjpg_scltab[tlebe[i*4+0]];
			j=PDJPG_CalcComponentValueExponent(norm[i*4+3]);
			f=255/pdjpg_scltab[j];
			tnorm[i*4+0]=norm[i*4+0]*255;
			tnorm[i*4+1]=norm[i*4+1]*255;
			tnorm[i*4+2]=norm[i*4+2]*255;
			tnorm[i*4+3]=norm[i*4+3]*f;
			tlebe[i*4+3]=j;
		}
	}

	if(spec)
	{
		for(i=0; i<n; i++)
		{
			tspec[i*4+0]=spec[i*4+0]*255;
			tspec[i*4+1]=spec[i*4+1]*255;
			tspec[i*4+2]=spec[i*4+2]*255;
			tspec[i*4+3]=spec[i*4+3]*255;
		}
	}

	if(luma)
	{
		for(i=0; i<n; i++)
		{
			PDJPG_EncodeFloatPixel(luma, tluma+i*4, tlebe+i*4+1);
		}
	}

	i=PDJPG_EncodeComponentCtxI(ctx,
		trgba, tnorm, tspec, tluma, tlebe,
		obuf, xs, ys, qf);
	return(i);
}

#if 1
PDGL_API byte *PDJPG_Load(VFILE *fd, int *xs, int *ys)
{
	byte *buf, *obuf;
	int fsz;

	vfseek(fd, 0, 2);
	fsz=vftell(fd);
	vfseek(fd, 0, 0);

	buf=malloc(fsz+256);
	vfread(buf, 1, fsz, fd);

	obuf=PDJPG_Decode(buf, fsz, xs, ys);
	free(buf);

	return(obuf);
}

PDGL_API int PDJPG_Store(VFILE *fd, byte *ibuf, int xs, int ys, int qf)
{
	static byte *buf=NULL;
	int fsz;

	if(!buf)buf=malloc(1<<22);

	fsz=PDJPG_Encode(ibuf, buf, xs, ys, qf);
	if(fsz<0)return(fsz);

	vfwrite(buf, 1, fsz, fd);
//	free(buf);

	return(0);
}
#endif
