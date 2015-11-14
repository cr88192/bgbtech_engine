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

#include <bgbbtj.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <math.h>


const char *pdjpg_marker[]={
"SOF0", "SOF1", "SOF2", "SOF3", "DHT", "SOF5", "SOF6", "SOF7",
"JPG", "SOF9", "SOF10", "SOF11", "DAC", "SOF13", "SOF14", "SOF15",
"RST0", "RST1", "RST2", "RST3", "RST4", "RST5", "RST6", "RST7",
"SOI", "EOI", "SOS", "DQT", "DNL", "DRI", "DHP", "EXP",
"APP0", "APP1", "APP2", "APP3", "APP4", "APP5", "APP6", "APP7", 
"APP8", "APP9", "APP10", "APP11", "APP12", "APP13", "APP14", "APP15", 
"JPG0", "JPG1", "JPG2", "JPG3", "JPG4", "JPG5", "JPG6", "JPG7", 
"JPG8", "JPG9", "JPG10", "JPG11", "JPG12", "JPG13", "COM", ""
};

const int pdjpg_zigzag[64]={
 0,  1,  5,  6, 14, 15, 27, 28,
 2,  4,  7, 13, 16, 26, 29, 42,
 3,  8, 12, 17, 25, 30, 41, 43,
 9, 11, 18, 24, 31, 40, 44, 53,
10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60,
21, 34, 37, 47, 50, 56, 59, 61,
35, 36, 48, 49, 57, 58, 62, 63
};

const int pdjpg_zigzag2[64]={
 0,  1,  8, 16,  9,  2,  3, 10,
17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63
};

float pdjpg_exptab[256];
float pdjpg_scltab[256];


BGBBTJ_API BGBBTJ_JPG_Context *BGBBTJ_JPG_AllocContext();
BGBBTJ_API void BGBBTJ_JPG_FreeContext(BGBBTJ_JPG_Context *ctx);
BGBBTJ_API void BGBBTJ_JPG_SetContextAlphaColor(BGBBTJ_JPG_Context *ctx,
	int cr, int cg, int cb, int ca);

//Common

void BGBBTJ_JPG_Init()
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

void BGBBTJ_JPG_SetupQuantTabDivFP(BGBBTJ_JPG_Context *ctx, int qid)
{
	int i;
	
	for(i=0; i<64; i++)
		ctx->jpg_qtfp[qid][i]=4096.0/ctx->jpg_qt[qid][i];
}

#if 1
BGBBTJ_API byte *BGBBTJ_JPG_Load(VFILE *fd, int *xs, int *ys)
{
	byte *buf, *obuf;
	int fsz;

	vfseek(fd, 0, 2);
	fsz=vftell(fd);
	vfseek(fd, 0, 0);

	buf=malloc(fsz+256);
	vfread(buf, 1, fsz, fd);

	obuf=BGBBTJ_JPG_Decode(buf, fsz, xs, ys);
	free(buf);

	return(obuf);
}

BGBBTJ_API int BGBBTJ_JPG_Store(VFILE *fd, byte *ibuf, int xs, int ys, int qf)
{
	static byte *buf=NULL;
	int fsz;

	if(!buf)buf=malloc(1<<22);

	fsz=BGBBTJ_JPG_Encode(ibuf, buf, xs, ys, qf);
	if(fsz<0)return(fsz);

	vfwrite(buf, 1, fsz, fd);
//	free(buf);

	return(0);
}
#endif
