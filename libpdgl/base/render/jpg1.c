#include <pdgl.h>

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#include <math.h>
//#include "tga.c"

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

char *pdjpg_marker[]={
"SOF0", "SOF1", "SOF2", "SOF3", "DHT", "SOF5", "SOF6", "SOF7",
"JPG", "SOF9", "SOF10", "SOF11", "DAC", "SOF13", "SOF14", "SOF15",
"RST0", "RST1", "RST2", "RST3", "RST4", "RST5", "RST6", "RST7",
"SOI", "EOI", "SOS", "DQT", "DNL", "DRI", "DHP", "EXP",
"APP0", "APP1", "APP2", "APP3", "APP4", "APP5", "APP6", "APP7", 
"APP8", "APP9", "APP10", "APP11", "APP12", "APP13", "APP14", "APP15", 
"JPG0", "JPG1", "JPG2", "JPG3", "JPG4", "JPG5", "JPG6", "JPG7", 
"JPG8", "JPG9", "JPG10", "JPG11", "JPG12", "JPG13", "COM", ""
};

byte pdjpg_zigzag[64]={
 0,  1,  5,  6, 14, 15, 27, 28,
 2,  4,  7, 13, 16, 26, 29, 42,
 3,  8, 12, 17, 25, 30, 41, 43,
 9, 11, 18, 24, 31, 40, 44, 53,
10, 19, 23, 32, 39, 45, 52, 54,
20, 22, 33, 38, 46, 51, 55, 60,
21, 34, 37, 47, 50, 56, 59, 61,
35, 36, 48, 49, 57, 58, 62, 63
};

byte pdjpg_zigzag2[64]={
 0,  1,  8, 16,  9,  2,  3, 10,
17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34,
27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36,
29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46,
53, 60, 61, 54, 47, 55, 62, 63
};

ushort pdjhuff_code[8*256];
byte pdjhuff_len[8*256];

byte *pdjhuff_cs;	//current pos in bitstream (input)
byte *pdjhuff_ct;	//current pos in bitstream (output)
uint pdjhuff_win;	//bitstream window
int pdjhuff_pos;	//bitstream offset
int pdjhuff_isend;	//bitstream has broken (decoder)

byte pdjpg_qt[4][64];	//quantization tables
int pdjpg_qtfp[4][64];	//quantization tables (fixed point)


//Decoder Specific

int pdjpg_xs, pdjpg_ys;
byte pdjpg_cid[16];
byte pdjpg_ch[16];
byte pdjpg_cv[16];
byte pdjpg_qid[16];
int pdjpg_cxi[16];
int pdjpg_cyi[16];
int pdjpg_nc;
int pdjpg_chm;
int pdjpg_chn;

int pdjpg_scid[4];
int pdjpg_scn[4];
short *pdjpg_scbuf[4];
byte *pdjpg_sibuf[4];

int pdjpg_schs[4];	//scan horizontal size
int pdjpg_scvs[4];	//scan vertical size
float pdjpg_schsc[4];	//scan horizontal scale
float pdjpg_scvsc[4];	//scan vertical scale

//Encoder Specific
int pdjpg_mono;
int pdjpg_tabcacheframe;	//cache table frame count.


//Common

#if 1
void PDJPG_TransDCT_Horiz(byte *iblk, int *oblk)
{
	char ib[8];

	ib[0]=iblk[0]-128;	ib[1]=iblk[1]-128;
	ib[2]=iblk[2]-128;	ib[3]=iblk[3]-128;
	ib[4]=iblk[4]-128;	ib[5]=iblk[5]-128;
	ib[6]=iblk[6]-128;	ib[7]=iblk[7]-128;

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

void PDJPG_TransDCT(byte *iblk, short *oblk)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	PDJPG_TransDCT_Horiz(iblk+0, s+0);
	PDJPG_TransDCT_Horiz(iblk+8, s+8);
	PDJPG_TransDCT_Horiz(iblk+16, s+16);
	PDJPG_TransDCT_Horiz(iblk+24, s+24);
	PDJPG_TransDCT_Horiz(iblk+32, s+32);
	PDJPG_TransDCT_Horiz(iblk+40, s+40);
	PDJPG_TransDCT_Horiz(iblk+48, s+48);
	PDJPG_TransDCT_Horiz(iblk+56, s+56);

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

void PDJPG_TransIDCT(short *iblk, byte *oblk)
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
		j=(t[i]>>16)+128;
		oblk[i]=(j<0)?0:((j>255)?255:j);
	}
}

#endif



//Decoder

int PDJHUFF_NextByte()
{
	int i, j;

	if(pdjhuff_isend)return(0xFF);

	i=*pdjhuff_cs++;
	if(i==0xFF)
	{
		j=*pdjhuff_cs++;
		if(j)
		{
//			printf("bitstream problem\n");
			pdjhuff_isend=1;
		}
	}
	return(i);
}

int PDJHUFF_InitStream(byte *buf)
{
	pdjhuff_pos=0;
	pdjhuff_isend=0;

	pdjhuff_cs=buf;
	pdjhuff_win=PDJHUFF_NextByte();
	pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte();
	pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte();
	pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte();
}

int PDJHUFF_ReadBit()
{
	int i;

	i=(pdjhuff_win>>(31-pdjhuff_pos))&1;
	pdjhuff_pos++;
	if(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte();
		pdjhuff_pos-=8;
	}
	return(i);
}

int PDJHUFF_ReadNBits(int n)
{
	int i;

	i=(pdjhuff_win>>(32-n-pdjhuff_pos))&((1<<n)-1);
	pdjhuff_pos+=n;
	while(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte();
		pdjhuff_pos-=8;
	}
	return(i);
}

void PDJHUFF_SkipNBits(int n)
{
	pdjhuff_pos+=n;
	while(pdjhuff_pos>=8)
	{
		pdjhuff_win=(pdjhuff_win<<8)|PDJHUFF_NextByte();
		pdjhuff_pos-=8;
	}
}

int PDJHUFF_PeekWord()
{
	int i;
	i=(pdjhuff_win>>(16-pdjhuff_pos))&65535;
	return(i);
}

int PDJHUFF_DecodeSymbol(int tab)
{
	int i, j, k;

	tab<<=8;

	i=PDJHUFF_PeekWord();
	for(j=0; j<256; j++)
	{
		k=pdjhuff_len[tab|j];
		if(!k)continue;

		if((i>>(16-k))!=pdjhuff_code[tab|j])
			continue;
		PDJHUFF_SkipNBits(k);
		return(j);
	}
	return(-1);
}

int PDJHUFF_DecodeDiffDC(int tab)
{
	int i, j;

	i=PDJHUFF_DecodeSymbol(tab);
	j=PDJHUFF_ReadNBits(i);

	if(!(j&(1<<(i-1))))
		j=(-1<<i)+j+1;
	return(j);
}

int PDJHUFF_DecodeBlock(short *buf, int dctab, int actab,
	int ni, int nn)
{
	int i, j, k;

	for(i=1; i<64; i++)buf[i]=0;

	buf[0]+=PDJHUFF_DecodeDiffDC(dctab);
	for(i=1; i<64; i++)
	{
		j=PDJHUFF_DecodeSymbol(actab);
		if(j<=0)
		{
			if(j<0)
			{
				printf("bad dct block1 %02X %d/%d\n", j, ni, nn);
				return(-1);
			}
			break;
		}

		i+=j>>4;	//preceding 0's
		if(!(j&15))continue;

		j&=15;
		k=PDJHUFF_ReadNBits(j);
		if(!(k&(1<<(j-1))))
			k=(-1<<j)+k+1;

		j=pdjpg_zigzag2[i];
		buf[j]=k;
	}

	if(i>64)
	{
		printf("bad dct block1 %02X %d/%d\n", j, ni, nn);
		return(-1);
	}

	return(0);
}

void PDJHUFF_QuantBlock(short *ibuf, short *obuf, int qid)
{
	int i, j;
	
//	for(i=0; i<64; i++)
//		obuf[i]=ibuf[i]/pdjpg_qt[qid][i];

	for(i=0; i<64; i++)
		obuf[i]=(ibuf[i]*pdjpg_qtfp[qid][i])>>12;
}

void PDJHUFF_DequantBlock(short *ibuf, short *obuf, int qid)
{
	int i, j;
	for(i=0; i<64; i++)
		obuf[i]=ibuf[i]*pdjpg_qt[qid][i];
}

void PDJPG_SetupQuantTabDivFP(int qid)
{
	int i;
	
	for(i=0; i<64; i++)
		pdjpg_qtfp[qid][i]=4096.0/pdjpg_qt[qid][i];
}

int PDJPG_MarkerSOS(byte *buf)
{
	static short dbuf[4*64];
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

		for(j=0; j<pdjpg_nc; j++)
			if(pdjpg_cid[j]==s[0])
		{
			ch[i]=pdjpg_ch[j];
			cv[i]=pdjpg_cv[j];
			qid[i]=pdjpg_qid[j];
			xi[i]=pdjpg_cxi[j];
			yi[i]=pdjpg_cyi[j];

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
		pdjpg_scid[i]=s[0];
		pdjpg_scn[i]=j;
		pdjpg_scbuf[i]=malloc(n*64*sizeof(short));
		pdjpg_sibuf[i]=malloc(n*64);

//		memset(pdjpg_sibuf[i], 0, n*64);

		ccnt[i]=0;

		s+=2;
	}
	s+=3;

	i=(pdjpg_xs+pdjpg_chm*8-1)/(pdjpg_chm*8);
	j=(pdjpg_ys+pdjpg_chn*8-1)/(pdjpg_chn*8);
	n=i*j;

	PDJHUFF_InitStream(s);

	for(i=0; i<(4*64); i++)dbuf[i]=0;

	rt=0;
	for(i=0; i<n; i++)
	{
		for(j=0; j<ns; j++)
			for(k=0; k<cv[j]; k++)
				for(l=0; l<ch[j]; l++)
		{
			if(rt<0)break;

			i1=(i/(wi[j]/ch[j]))*ch[j];
			j1=(i%(wi[j]/ch[j]))*ch[j];
			k1=((i1+k)*wi[j])+(j1+l);

			rt=PDJHUFF_DecodeBlock(dbuf+j*64,
				cdt[j]*2+0, cat[j]*2+1, i, n);
			if(rt<0)break;
			PDJHUFF_DequantBlock(dbuf+j*64,
				pdjpg_scbuf[j]+k1*64, qid[j]);
			PDJPG_TransIDCT(pdjpg_scbuf[j]+k1*64,
				pdjpg_sibuf[j]+k1*64);
		}
	}

	if(rt<0)
	{
		printf("PDJPG_MarkerSOS: Bad Image\n");
		return(-1);
	}

	return(0);
}

int PDJPG_MarkerSOF0(byte *buf)
{
	byte *cnt, *s;
	int i, j, k, l, m, n;

	l=(buf[0]<<8)|buf[1];
//	printf("p %d\n", buf[2]);

	i=(buf[3]<<8)|buf[4];
	j=(buf[5]<<8)|buf[6];

	pdjpg_xs=j;
	pdjpg_ys=i;
//	printf("size %dx%d\n", j, i);

	m=0; n=0;

	k=buf[7];
	s=buf+8;
	for(i=0; i<k; i++)
	{
		pdjpg_cid[i]=s[0];
		pdjpg_ch[i]=s[1]>>4;
		pdjpg_cv[i]=s[1]&15;
		pdjpg_qid[i]=s[2];

		if(pdjpg_ch[i]>m)m=pdjpg_ch[i];
		if(pdjpg_cv[i]>n)n=pdjpg_cv[i];

//		printf("CID %d: %d %d %d\n", pdjpg_cid[i],
//			pdjpg_ch[i], pdjpg_cv[i], pdjpg_qid[i]);
		s+=3;
	}

	pdjpg_chm=m;
	pdjpg_chn=n;

	for(i=0; i<k; i++)
	{
		pdjpg_cxi[i]=(pdjpg_ch[i]*pdjpg_xs)/m;
		pdjpg_cyi[i]=(pdjpg_cv[i]*pdjpg_ys)/n;
	}

	pdjpg_nc=k;

	return(0);
}

int PDJPG_MarkerDQT(byte *buf)
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
			pdjpg_qt[i][pdjpg_zigzag2[j]]=s[j+1];
		s+=65;
	}
	
	return(0);
}

int PDJPG_MarkerDHT(byte *buf)
{
	byte *cnt, *s;
	int i, j, k, l, tn;

	l=(buf[0]<<8)|buf[1];

	j=(buf[2]>>4)&15;
	i=buf[2]&15;
//	printf("dest %d, type %d\n", i, j);

	tn=i*2+j;

	for(i=0; i<256; i++)
		pdjhuff_len[tn*256+i]=0;

	cnt=buf+3;
	s=cnt+16;
	k=0; l=0;
	for(i=0; i<16; i++)
	{
		k<<=1;
		for(j=0; j<cnt[i]; j++)
		{
			pdjhuff_code[tn*256+(*s)]=k++;
			pdjhuff_len[tn*256+(*s)]=i+1;
			l=i+1;
			s++;
		}
	}
//	printf("%04X %d\n", k, l);

	return(0);
}

int PDJPG_GetComponentPixel(int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

//	id=pdjpg_scn[cn];

	i=x*pdjpg_schsc[cn];
	j=y*pdjpg_scvsc[cn];
//	i=(pdjpg_ch[id]*x)/pdjpg_chm;
//	j=(pdjpg_cv[id]*y)/pdjpg_chn;
	k=i>>3;
	l=j>>3;

	w=pdjpg_schs[cn];
	h=pdjpg_scvs[cn];
//	w=(pdjpg_cxi[id]+7)/8;
//	h=(pdjpg_cyi[id]+7)/8;
//	w=(pdjpg_cxi[id]+15)/8;
//	h=(pdjpg_cyi[id]+15)/8;
	n=(l*w)+k;

	k=i&7;
	l=j&7;

	v=pdjpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}

PDGL_API byte *PDJPG_Decode(byte *buf, int sz, int *xs, int *ys)
{
	byte *obuf;
	int i, j, k, l;
	int y, u, v, r, g, b;

	if((buf[0]!=0xFF) || (buf[1]!=JPG_SOI))
		return(NULL);

	for(i=0; i<4; i++)
	{
		pdjpg_scbuf[i]=NULL;
		pdjpg_sibuf[i]=NULL;
	}

	for(i=0; i<sz; i++)
	{
		if((buf[i]==0xFF) && (buf[i+1]!=0x00) && (buf[i+1]!=0xFF))
		{
			j=buf[i+1]-0xC0;
			if(j<0)j=63;

//			printf("@ %d %s(%02X)\n", i,
//				pdjpg_marker[j], buf[i+1]);

			if(buf[i+1]==JPG_DQT)PDJPG_MarkerDQT(buf+i+2);
			if(buf[i+1]==JPG_DHT)PDJPG_MarkerDHT(buf+i+2);
			if(buf[i+1]==JPG_SOF0)PDJPG_MarkerSOF0(buf+i+2);
			if(buf[i+1]==JPG_SOS)PDJPG_MarkerSOS(buf+i+2);

			if(buf[i+1]==JPG_EOI)break;
		}
	}

	obuf=malloc((pdjpg_xs+1)*(pdjpg_ys+1)*4);
	memset(obuf, 0xFF, pdjpg_xs*pdjpg_ys*4);

	for(i=0; i<pdjpg_nc; i++)
	{
		j=pdjpg_scn[i];
		pdjpg_schsc[i]=pdjpg_ch[j]/(pdjpg_chm*1.0);
		pdjpg_scvsc[i]=pdjpg_cv[j]/(pdjpg_chn*1.0);
		pdjpg_schs[i]=(pdjpg_cxi[j]+7)/8;
		pdjpg_scvs[i]=(pdjpg_cyi[j]+7)/8;
		
	}

	for(i=0; i<pdjpg_ys; i++)
		for(j=0; j<pdjpg_xs; j++)
	{
		if(pdjpg_nc==3)
		{
			y=PDJPG_GetComponentPixel(0, j, i);
			u=PDJPG_GetComponentPixel(1, j, i);
			v=PDJPG_GetComponentPixel(2, j, i);

			r=y+1.40200*(v-128);
			g=y-0.34414*(u-128)-0.71414*(v-128);
			b=y+1.77200*(u-128);

			r=(r<0)?0:((r>255)?255:r);
			g=(g<0)?0:((g>255)?255:g);
			b=(b<0)?0:((b>255)?255:b);
		}else if(pdjpg_nc==1)
		{
			y=PDJPG_GetComponentPixel(0, j, i);
			r=y; g=y; b=y;
		}

		k=pdjpg_ys-1-i;

		obuf[((k*pdjpg_xs)+j)*4+0]=r;
		obuf[((k*pdjpg_xs)+j)*4+1]=g;
		obuf[((k*pdjpg_xs)+j)*4+2]=b;
	}

	for(i=0; i<4; i++)
	{
		if(pdjpg_scbuf[i])free(pdjpg_scbuf[i]);
		if(pdjpg_sibuf[i])free(pdjpg_sibuf[i]);
	}

	*xs=pdjpg_xs;
	*ys=pdjpg_ys;
	return(obuf);
}

PDGL_API void PDJPG_Free(byte *buf)
{
	if(!buf)return;
	free(buf);
}


//Encoder

void PDJHUFF_WriteBit(int i)
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

void PDJHUFF_WriteNBits(int i, int n)
{
	i&=(1<<n)-1;

	pdjhuff_win|=i<<((32-n)-pdjhuff_pos);
	pdjhuff_pos+=n;
	while(pdjhuff_pos>=8)
	{
		i=(pdjhuff_win>>24)&0xFF;
		*pdjhuff_ct++=i;
		if(i==0xFF)*pdjhuff_ct++=0x00;
		pdjhuff_win<<=8;
		pdjhuff_pos-=8;
	}
}

void PDJHUFF_FlushBits()
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
	static short nodes[1024], nlen[512];
	static short roots[512], clen[512];
	static int cnts[512];
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


void PDJHUFF_EncodeSymbol(int tab, int v)
{
	tab<<=8;
	PDJHUFF_WriteNBits(pdjhuff_code[tab|v], pdjhuff_len[tab|v]);
}

void PDJHUFF_EncodeVal(int tab, int z, int v)
{
	int i, j, k;

	if(!v) { PDJHUFF_EncodeSymbol(tab, z<<4); return; }

	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		PDJHUFF_EncodeSymbol(tab, (z<<4)|i);
		PDJHUFF_WriteNBits(v, i);
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	PDJHUFF_EncodeSymbol(tab, (z<<4)|i);

	k=(1<<i)-(j+1);
	PDJHUFF_WriteNBits(k, i);
	return;
}

void PDJHUFF_StatVal(int *stat, int z, int v)
{
	int i, j, k;

	if(!v) { stat[z<<4]++; return; }

	if(v>0)
	{
		i=1; while(v>=(1<<i))i++;
		stat[(z<<4)|i]++;
		return;
	}

	i=1; j=-v; while(j>=(1<<i))i++;
	stat[(z<<4)|i]++;
}

#if 0
int PDJHUFF_EncodeBlock(short *buf, int dctab, int actab)
{
	int i, j, k;

	PDJHUFF_EncodeVal(dctab, 0, buf[0]);

	for(i=1; i<64; i++)
	{
		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			PDJHUFF_EncodeSymbol(actab, 0);
			break;
		}

		j-=i; if(j>15)j=15;
		PDJHUFF_EncodeVal(actab, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
}


int PDJHUFF_StatBlock(short *buf, int *dcstat, int *acstat)
{
	int i, j, k;

	PDJHUFF_StatVal(dcstat, 0, buf[0]);
	for(i=1; i<64; i++)
	{
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
}
#endif


#if 1
int PDJHUFF_EncodeBlock(short *buf, int dctab, int actab)
{
	int i, j, k;

	PDJHUFF_EncodeVal(dctab, 0, buf[0]);

	for(i=1; i<64; i++)
	{
		if(buf[pdjpg_zigzag2[i]])
		{
			PDJHUFF_EncodeVal(actab, 0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
		for(j=i; j<64; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=64)
		{
			PDJHUFF_EncodeSymbol(actab, 0);
			break;
		}

		j-=i; if(j>15)j=15;
		PDJHUFF_EncodeVal(actab, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
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
}
#endif

#if 0
int PDJHUFF_EncodeBlock(short *buf, int dctab, int actab)
{
	int i, j, k;

	PDJHUFF_EncodeVal(dctab, 0, buf[0]);

	for(i=1; i<64; i++)
	{
#if 1
		if(buf[pdjpg_zigzag2[i]])
		{
			PDJHUFF_EncodeVal(actab, 0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
#endif
//		k=i+16; if(k>64)k=64;
		k=(i>=48)?64:(i+16);
		for(j=i; j<k; j++)
			if(buf[pdjpg_zigzag2[j]])
				break;
		if(j>=k)
		{
			for(; k<64; k++)
				if(buf[pdjpg_zigzag2[k]])
					break;
			if(k>=64)
			{
				PDJHUFF_EncodeSymbol(actab, 0);
				break;
			}
		}

		j-=i; if(j>15)j=15;
		PDJHUFF_EncodeVal(actab, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
}
#endif

#if 0
int PDJHUFF_StatBlock(short *buf, int *dcstat, int *acstat)
{
	int i, j, k;

	PDJHUFF_StatVal(dcstat, 0, buf[0]);
	for(i=1; i<64; i++)
	{
#if 1
		if(buf[pdjpg_zigzag2[i]])
		{
			PDJHUFF_StatVal(acstat, 0, buf[pdjpg_zigzag2[i]]);
			continue;
		}
#endif
//		k=i+16; if(k>64)k=64;
		k=(i>=48)?64:(i+16);
		for(j=i; j<k; j++)
			if(buf[pdjpg_zigzag2[j]])break;
		if(j>=k)
		{
			for(; k<64; k++)
				if(buf[pdjpg_zigzag2[k]])
					break;
			if(k>=64)
			{
				acstat[0]++;
				break;
			}
		}

		j-=i; if(j>15)j=15;
		PDJHUFF_StatVal(acstat, j, buf[pdjpg_zigzag2[i+j]]);
		i+=j;
	}
}
#endif

#if 0
int PDJPG_GetImgBlk(char *blk, int xo, int yo, byte *img, int xs, int ys)
{
	int i, j, k;

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		k=img[(i+yo)*xs+(j+xo)];
//		blk[i*DCTSZ+j]=k-128;
		blk[i*DCTSZ+j]=k;
	}
}
#endif

#if 1
int PDJPG_GetImgBlk(char *blk, int xo, int yo, byte *img, int xs, int ys)
{
	byte *cs;
	char *ct;
	int i, j, k;

#if 0
	memcpy(blk   , img+((yo+0)*xs+xo), 8);
	memcpy(blk+ 8, img+((yo+1)*xs+xo), 8);
	memcpy(blk+16, img+((yo+2)*xs+xo), 8);
	memcpy(blk+24, img+((yo+3)*xs+xo), 8);
	memcpy(blk+32, img+((yo+4)*xs+xo), 8);
	memcpy(blk+40, img+((yo+5)*xs+xo), 8);
	memcpy(blk+48, img+((yo+6)*xs+xo), 8);
	memcpy(blk+56, img+((yo+7)*xs+xo), 8);
#endif

#if 1
	cs=img+(yo*xs+xo); ct=blk;
	memcpy(ct, cs, 8); ct+=8; cs+=xs;
	memcpy(ct, cs, 8); ct+=8; cs+=xs;
	memcpy(ct, cs, 8); ct+=8; cs+=xs;
	memcpy(ct, cs, 8); ct+=8; cs+=xs;
	memcpy(ct, cs, 8); ct+=8; cs+=xs;
	memcpy(ct, cs, 8); ct+=8; cs+=xs;
	memcpy(ct, cs, 8); ct+=8; cs+=xs;
	memcpy(ct, cs, 8); // ct+=8; cs+=xs;
#endif

#if 0
	ct=blk;
	for(i=0; i<DCTSZ; i++)
	{
		cs=img+((i+yo)*xs+xo);
		memcpy(ct, cs, 8); ct+=8;

//		*ct++=*cs++; *ct++=*cs++; *ct++=*cs++; *ct++=*cs++;
//		*ct++=*cs++; *ct++=*cs++; *ct++=*cs++; *ct++=*cs++;
//		for(j=0; j<DCTSZ; j++)
//			{ *ct++=*cs++; }
	}
#endif
}
#endif

int PDJPG_FilterImageDCT(byte *ibuf, short *obuf, int xs, int ys)
{
	static short tblk[DCTSZ2], tblk2[DCTSZ2];
	static char blk[DCTSZ2];
	int i, j, k, l;

	k=0;
	for(i=0; i<(ys/DCTSZ); i++)
		for(j=0; j<(xs/DCTSZ); j++)
	{
		PDJPG_GetImgBlk(blk, j*DCTSZ, i*DCTSZ, ibuf, xs, ys);
		PDJPG_TransDCT(blk, obuf+k*DCTSZ2);
		k++;
	}
}

void PDJPG_EmitDQT(int n)
{
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_DQT;

	i=64+3;
	*pdjhuff_ct++=i>>8;
	*pdjhuff_ct++=i&0xFF;

	*pdjhuff_ct++=n;
	for(i=0; i<64; i++)
		*pdjhuff_ct++=pdjpg_qt[n][pdjpg_zigzag2[i]];
}

void PDJPG_EmitSOF(int xs, int ys)
{
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOF0;

	i=8+(pdjpg_mono?1:3)*3;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	*pdjhuff_ct++=8; 	//P

	*pdjhuff_ct++=ys>>8;	//Y
	*pdjhuff_ct++=ys&0xFF;	//Y
	*pdjhuff_ct++=xs>>8;	//X
	*pdjhuff_ct++=xs&0xFF;	//X

	if(pdjpg_mono)
	{
		*pdjhuff_ct++=1;	//Nf

		*pdjhuff_ct++=1;	//Ci
		*pdjhuff_ct++=0x11;	//Hi Vi
		*pdjhuff_ct++=0;	//Tqi
	}else
	{
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
	}
}

void PDJPG_EmitSOS()
{
	int i;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOS;

	i=6+(pdjpg_mono?1:3)*2;
	*pdjhuff_ct++=i>>8;	//Lf
	*pdjhuff_ct++=i&0xFF;

	if(!pdjpg_mono)
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
}

void PDJPG_EmitDHT(int tab)
{
	byte *p;
	int i, j, k;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_DHT;

//	i=8+3*1;
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
			if(pdjhuff_len[tab|j]==i)
				k++;
		*pdjhuff_ct++=k; 	//Li
	}

	k=0;
	for(i=1; i<=16; i++)
	{
		k<<=1;
		for(j=0; j<256; j++)
			if(pdjhuff_len[tab|j]==i)
		{
			*pdjhuff_ct++=j; 	//Vi
			pdjhuff_code[tab|j]=k++;
		}
	}

//	printf("DHT %04X\n", k);

	i=pdjhuff_ct-p;
	p[0]=i>>8;	//Lf
	p[1]=i&0xFF;
}

void PDJPG_MakeQuantTabInput(short *in, int cnt, byte *tab, float q)
{
	static double deltas[DCTSZ2];
	double f, g, te;
	int i, j, k;

	for(j=0; j<DCTSZ2; j++)
		deltas[j]=0;

	for(i=0; i<cnt; i++)
		for(j=0; j<DCTSZ2; j++)
			deltas[j]+=fabs(in[i*DCTSZ2+j]);
	for(j=0; j<DCTSZ2; j++)
		deltas[j]/=cnt;

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
	PDJPG_MakeQuantTabInput(in, i, tab, q);
}

PDGL_API int PDJPG_Encode(byte *ibuf, byte *obuf, int xs, int ys, int qf)
{
	static byte *yb=NULL, *ub, *vb;
	static short *ydb=NULL, *udb, *vdb;
	static int lxs=0, lys=0;

	static int dcs[256], acs[256];
	static int dcsuv[256], acsuv[256];
	short *tp;
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, cy, cu, cv;
	int i, j, k, l;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;
//	xs3=(xs2+1)/2;
//	ys3=(ys2+1)/2;

	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;

	pdjpg_mono=0;
	pdjpg_tabcacheframe=0;

	//full quality
	for(i=0; i<64; i++)pdjpg_qt[0][i]=1;
	for(i=0; i<64; i++)pdjpg_qt[1][i]=1;

	//dummy huffman tables
	for(i=0; i<256; i++)pdjhuff_len[0*256+i]=0;
	for(i=0; i<256; i++)pdjhuff_len[1*256+i]=0;
	for(i=0; i<256; i++)pdjhuff_len[2*256+i]=0;
	for(i=0; i<256; i++)pdjhuff_len[3*256+i]=0;

	for(i=0; i<16; i++)pdjhuff_len[0*256+i]=4;
	for(i=0; i<16; i++)pdjhuff_len[2*256+i]=4;
	for(i=0; i<255; i++)pdjhuff_len[1*256+i]=8;
	for(i=0; i<255; i++)pdjhuff_len[3*256+i]=8;

//	for(i=0; i<255; i++)pdjhuff_len[0*256+i]=8;
//	for(i=0; i<255; i++)pdjhuff_len[2*256+i]=8;


//	pdjhuff_len[1*256+0]=7;

//	printf("M0\n");

	if(!yb || (xs!=lxs) || (ys!=lys))
	{
		if(yb)
		{
			free(yb);
			free(ub);
			free(vb);
			free(ydb);
			free(udb);
			free(vdb);
		}

		yb=malloc(xs2*ys2);
		ub=malloc(xs2*ys2);
		vb=malloc(xs2*ys2);

		ydb=malloc((xs2+8)*(ys2+8)*sizeof(short));
		udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));

		lxs=xs;
		lys=ys;
	}


	memset(yb, 128, xs2*ys2);
	memset(ub, 128, xs2*ys2);
	memset(vb, 128, xs2*ys2);

	memset(ydb, 0, xs2*(ys2+8)*sizeof(short));
	memset(udb, 0, xs3*(ys3+8)*sizeof(short));
	memset(vdb, 0, xs3*(ys3+8)*sizeof(short));

	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1);
		cr=ibuf[(k*xs+j)*4+0];
		cg=ibuf[(k*xs+j)*4+1];
		cb=ibuf[(k*xs+j)*4+2];

		cy=0.299*cr	+0.587*cg	+0.114*cb;
		cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
		cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;

//		cu=128; cv=128;

		cy=(cy<0)?0:((cy>255)?255:cy);
		cu=(cu<0)?0:((cu>255)?255:cu);
		cv=(cv<0)?0:((cv>255)?255:cv);

		yb[i*xs2+j]=cy; ub[i*xs2+j]=cu; vb[i*xs2+j]=cv;
	}

	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=ub[(i*2)*xs2+j*2]+
			ub[(i*2)*xs2+j*2+1]+
			ub[(i*2+1)*xs2+j*2]+
			ub[(i*2+1)*xs2+j*2+1];
		ub[i*xs3+j]=k/4;

		k=vb[(i*2)*xs2+j*2]+
			vb[(i*2)*xs2+j*2+1]+
			vb[(i*2+1)*xs2+j*2]+
			vb[(i*2+1)*xs2+j*2+1];
		vb[i*xs3+j]=k/4;
	}

//	printf("M1\n");


	PDJPG_FilterImageDCT(yb, ydb, xs2, ys2);
	PDJPG_FilterImageDCT(ub, udb, xs3, ys3);
	PDJPG_FilterImageDCT(vb, vdb, xs3, ys3);

	j=(xs2/8)*(ys2/8);
	PDJPG_MakeQuantTabInput(ydb, j, pdjpg_qt[0], qf/100.0);

	j=(xs3/8)*(ys3/8);
	PDJPG_MakeQuantTabInput(udb, j, pdjpg_qt[1], qf/100.0);
	PDJPG_MakeQuantTabInput(vdb, j, pdjpg_qt[2], qf/100.0);
	for(i=0; i<64; i++)pdjpg_qt[1][i]=(pdjpg_qt[1][i]+pdjpg_qt[2][i])/2;

	PDJPG_SetupQuantTabDivFP(0);
	PDJPG_SetupQuantTabDivFP(1);

//	free(yb);
//	free(ub);
//	free(vb);


	if(pdjpg_mono)
	{
		j=(xs2/8)*(ys2/8); k=0;
		for(i=0; i<j; i++)
		{
			PDJHUFF_QuantBlock(ydb+i*64, ydb+i*64, 0);
			ydb[i*64+0]-=k; k=ydb[i*64+0]+k;
		}
	}

	if(!pdjpg_mono)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			tp=ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
		}
	}

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		PDJHUFF_QuantBlock(udb+i*64, udb+i*64, 1);
		PDJHUFF_QuantBlock(vdb+i*64, vdb+i*64, 1);
		udb[i*64+0]-=k; k=udb[i*64+0]+k;
		vdb[i*64+0]-=l; l=vdb[i*64+0]+l;
	}

//	printf("M2\n");

	for(i=0; i<256; i++)dcs[i]=0;
	for(i=0; i<256; i++)acs[i]=0;
	for(i=0; i<256; i++)dcsuv[i]=0;
	for(i=0; i<256; i++)acsuv[i]=0;

	j=(xs2/8)*(ys2/8);
	k=(xs3/8)*(ys3/8);
	for(i=0; i<j; i++)PDJHUFF_StatBlock(ydb+i*64, dcs, acs);
	for(i=0; i<k; i++)PDJHUFF_StatBlock(udb+i*64, dcsuv, acsuv);
	for(i=0; i<k; i++)PDJHUFF_StatBlock(vdb+i*64, dcsuv, acsuv);

	PDJHUFF_BuildLengths(dcs, 256, pdjhuff_len+0*256, 16);
	PDJHUFF_BuildLengths(acs, 256, pdjhuff_len+1*256, 16);
	PDJHUFF_BuildLengths(dcsuv, 256, pdjhuff_len+2*256, 16);
	PDJHUFF_BuildLengths(acsuv, 256, pdjhuff_len+3*256, 16);

	pdjhuff_ct=obuf;
	pdjhuff_win=0;
	pdjhuff_pos=0;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOI;

	PDJPG_EmitDQT(0);
	if(!pdjpg_mono)PDJPG_EmitDQT(1);

	PDJPG_EmitSOF(xs, ys);

	PDJPG_EmitDHT(0);
	PDJPG_EmitDHT(1);
	if(!pdjpg_mono)
	{
		PDJPG_EmitDHT(2);
		PDJPG_EmitDHT(3);
	}

	PDJPG_EmitSOS();

	if(pdjpg_mono)
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)PDJHUFF_EncodeBlock(ydb+i*64, 0, 1);
	}else
	{
		for(i=0; i<=((ys3)/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			PDJHUFF_EncodeBlock(ydb+((i*2+0)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ydb+((i*2+0)*(xs2/8)+j*2+1)*64, 0, 1);
			PDJHUFF_EncodeBlock(ydb+((i*2+1)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ydb+((i*2+1)*(xs2/8)+j*2+1)*64, 0, 1);

			k=i*(xs3/8)+j;
			PDJHUFF_EncodeBlock(udb+k*64, 2, 3);
			PDJHUFF_EncodeBlock(vdb+k*64, 2, 3);
		}
	}

	PDJHUFF_FlushBits();

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_EOI;

//	free(ydb);
//	free(udb);
//	free(vdb);

//	printf("M3\n");

	i=pdjhuff_ct-obuf;

	return(i);
}

PDGL_API int PDJPG_FlushEncodeFast(void)
{
	pdjpg_tabcacheframe=0;	//force rebuild
}

PDGL_API int PDJPG_EncodeFast(byte *ibuf, byte *obuf,
	int xs, int ys, int qf)
{
	static byte *yb=NULL, *ub, *vb;
	static short *ydb=NULL, *udb, *vdb;
	static int lxs=0, lys=0;

	static int dcs[256], acs[256];
	static int dcsuv[256], acsuv[256];
	short *tp;
	byte *cs, *cse, *cty, *ctu, *ctv;
	int xs2, ys2, xs3, ys3;
	int cr, cg, cb, cy, cu, cv;

	int cra, cga, cba, cya;
	int crb, cgb, cbb, cyb;
	int crc, cgc, cbc, cyc;
	int crd, cgd, cbd, cyd;
	byte *cs1, *cs2, *cty1, *cty2;

	int i, j, k, l, n;

	xs2=((xs+7)/8)*8;
	ys2=((ys+7)/8)*8;
	xs3=((xs+15)/16)*8;
	ys3=((ys+15)/16)*8;

	pdjpg_mono=0;

	if(!pdjpg_tabcacheframe)
	{
		//full quality
		for(i=0; i<64; i++)pdjpg_qt[0][i]=1;
		for(i=0; i<64; i++)pdjpg_qt[1][i]=1;
	}

	if(!yb || (xs!=lxs) || (ys!=lys))
	{
		if(yb)
		{
			free(yb);
			free(ub);
			free(vb);
			free(ydb);
			free(udb);
			free(vdb);
		}

		yb=malloc(xs2*ys2);
		ub=malloc(xs2*ys2);
		vb=malloc(xs2*ys2);

		ydb=malloc((xs2+8)*(ys2+16)*sizeof(short));
		udb=malloc((xs3+8)*(ys3+8)*sizeof(short));
		vdb=malloc((xs3+8)*(ys3+8)*sizeof(short));

		lxs=xs;
		lys=ys;

		memset(yb, 128, xs2*ys2);
		memset(ub, 128, xs2*ys2);
		memset(vb, 128, xs2*ys2);

		memset(ydb, 0, xs2*(ys2+8)*sizeof(short));
		memset(udb, 0, xs3*(ys3+8)*sizeof(short));
		memset(vdb, 0, xs3*(ys3+8)*sizeof(short));

		pdjpg_tabcacheframe=0;	//force rebuild
	}


//	memset(yb, 128, xs2*ys2);
//	memset(ub, 128, xs2*ys2);
//	memset(vb, 128, xs2*ys2);

//	memset(ydb, 0, xs2*(ys2+8)*sizeof(short));
//	memset(udb, 0, xs3*(ys3+8)*sizeof(short));
//	memset(vdb, 0, xs3*(ys3+8)*sizeof(short));

#if 1
	n=(ys+1)/2;
	for(i=0; i<n; i++)
	{
		k=((ys-(2*i+1))*xs)*4;
		cs1=ibuf+k; cse=cs1+xs*4;
		l=((ys-(2*i+2))*xs)*4;
		cs2=ibuf+l;

		cty1=yb+(2*i+0)*xs2;
		cty2=yb+(2*i+1)*xs2;

		l=i*xs3;
		ctu=ub+l; ctv=vb+l;

		while(cs1<cse)
		{
//			cr=*cs++; cg=*cs++; cb=*cs++; cs++;
			cra=*(cs1+0); cga=*(cs1+1); cba=*(cs1+2);
			crb=*(cs1+4); cgb=*(cs1+5); cbb=*(cs1+6);
			crc=*(cs2+0); cgc=*(cs2+1); cbc=*(cs2+2);
			crd=*(cs2+4); cgd=*(cs2+5); cbd=*(cs2+6);

//			cy=0.299*cr	+0.587*cg	+0.114*cb;
			cya=19595*cra + 38470*cga + 7471*cba;
			cyb=19595*crb + 38470*cgb + 7471*cbb;
			cyc=19595*crc + 38470*cgc + 7471*cbc;
			cyd=19595*crd + 38470*cgd + 7471*cbd;
			*cty1++=cya>>16;
			*cty1++=cyb>>16;
			*cty2++=cyc>>16;
			*cty2++=cyd>>16;

//			cya= 77*cra +150*cga + 29*cba;
//			cyb= 77*crb +150*cgb + 29*cbb;
//			cyc= 77*crc +150*cgc + 29*cbc;
//			cyd= 77*crd +150*cgd + 29*cbd;
//			*cty1++=cya>>8;
//			*cty1++=cyb>>8;
//			*cty2++=cyc>>8;
//			*cty2++=cyd>>8;

			cr=(cra+crb+crc+crd)>>2;
			cg=(cga+cgb+cgc+cgd)>>2;
			cb=(cba+cbb+cbc+cbd)>>2;

//			cu=-0.1687*cr-0.3313*cg+0.5*cb 	+128;
//			cv=0.5*cr-0.4187*cg-0.0813*cb	+128;
			cu=-11056*cr -21712*cg +32768*cb;
			cv= 32768*cr -27440*cg - 5328*cb;
			*ctu++=(cu>>16)+128;
			*ctv++=(cv>>16)+128;

//			cu=-43*cr - 84*cg +128*cb;
//			cv=128*cr -107*cg - 20*cb;
//			*ctu++=(cu>>8)+128;
//			*ctv++=(cv>>8)+128;

			cs1+=8; cs2+=8;
		}
	}
#endif

#if 0
	for(i=0; i<ys; i++)
	{
		k=((ys-(i+1))*xs)*4;
		cs=ibuf+k; cse=cs+xs*4;

		l=i*xs2;
		cty=yb+l; ctu=ub+l; ctv=vb+l;

		while(cs<cse)
		{
//			cr=*cs++; cg=*cs++; cb=*cs++; cs++;
			cr=*(cs+0); cg=*(cs+1); cb=*(cs+2); cs+=4;
			cy= 76*cr +150*cg + 29*cb;
			cu=-43*cr - 84*cg +128*cb;
			cv=128*cr -107*cg - 20*cb;
			*cty++=cy>>8; *ctu++=(cu>>8)+128; *ctv++=(cv>>8)+128;
		}
	}
#endif

#if 0
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		k=ys-(i+1); l=(k*xs+j)*4;
//		cr=ibuf[l+0];
//		cg=ibuf[l+1];
//		cb=ibuf[l+2];

		cr=ibuf[l++]; cg=ibuf[l++]; cb=ibuf[l++];

//		cy=0.299*cr	+0.587*cg	+0.114*cb;
//		cu=-0.1687*cr	-0.3313*cg	+0.5*cb 	+128;
//		cv=0.5*cr	-0.4187*cg	-0.0813*cb	+128;

//		cu=128; cv=128;

//		cy=(cy<0)?0:((cy>255)?255:cy);
//		cu=(cu<0)?0:((cu>255)?255:cu);
//		cv=(cv<0)?0:((cv>255)?255:cv);

		cy= 76*cr +150*cg + 29*cb;
		cu=-43*cr - 84*cg +128*cb;
		cv=128*cr -107*cg - 20*cb;
		cy>>=8; cu=(cu>>8)+128; cv=(cv>>8)+128;

		l=i*xs2+j;
		yb[l]=cy; ub[l]=cu; vb[l]=cv;
	}
#endif

#if 0
	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		k=ub[(i*2)*xs2+j*2]+
			ub[(i*2)*xs2+j*2+1]+
			ub[(i*2+1)*xs2+j*2]+
			ub[(i*2+1)*xs2+j*2+1];
		ub[i*xs3+j]=k/4;

		k=vb[(i*2)*xs2+j*2]+
			vb[(i*2)*xs2+j*2+1]+
			vb[(i*2+1)*xs2+j*2]+
			vb[(i*2+1)*xs2+j*2+1];
		vb[i*xs3+j]=k/4;
	}
#endif

#if 0
	for(i=0; i<ys3; i++)
		for(j=0; j<xs3; j++)
	{
		ub[i*xs3+j]=ub[(i*2)*xs2+j*2];
		vb[i*xs3+j]=vb[(i*2)*xs2+j*2];
	}
#endif


	PDJPG_FilterImageDCT(yb, ydb, xs2, ys2);
	PDJPG_FilterImageDCT(ub, udb, xs3, ys3);
	PDJPG_FilterImageDCT(vb, vdb, xs3, ys3);

	if(pdjpg_tabcacheframe<=0)
	{
		j=(xs2/8)*(ys2/8);
		PDJPG_MakeQuantTabInputFast(ydb, j, pdjpg_qt[0], qf/100.0);

		j=(xs3/8)*(ys3/8);
		PDJPG_MakeQuantTabInputFast(udb, j, pdjpg_qt[1], qf/100.0);
		PDJPG_MakeQuantTabInputFast(vdb, j, pdjpg_qt[2], qf/100.0);
		for(i=0; i<64; i++)
			pdjpg_qt[1][i]=(pdjpg_qt[1][i]+pdjpg_qt[2][i])/2;

		PDJPG_SetupQuantTabDivFP(0);
		PDJPG_SetupQuantTabDivFP(1);
	}

//	free(yb);
//	free(ub);
//	free(vb);


	if(pdjpg_mono)
	{
		j=(xs2/8)*(ys2/8); k=0;
		for(i=0; i<j; i++)
		{
			PDJHUFF_QuantBlock(ydb+i*64, ydb+i*64, 0);
			ydb[i*64+0]-=k; k=ydb[i*64+0]+k;
		}
	}

	if(!pdjpg_mono)
	{
		l=0;
		for(i=0; i<=(ys3/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			tp=ydb+((i*2+0)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ydb+((i*2+0)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ydb+((i*2+1)*(xs2/8)+j*2+0)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;

			tp=ydb+((i*2+1)*(xs2/8)+j*2+1)*64;
			PDJHUFF_QuantBlock(tp, tp, 0);
			tp[0]-=l; l=tp[0]+l;
		}
	}

	j=(xs3/8)*(ys3/8); k=0; l=0;
	for(i=0; i<j; i++)
	{
		PDJHUFF_QuantBlock(udb+i*64, udb+i*64, 1);
		PDJHUFF_QuantBlock(vdb+i*64, vdb+i*64, 1);
		udb[i*64+0]-=k; k=udb[i*64+0]+k;
		vdb[i*64+0]-=l; l=vdb[i*64+0]+l;
	}

//	printf("M2\n");

	if(pdjpg_tabcacheframe<=0)
	{
//		for(i=0; i<256; i++)dcs[i]=0;
//		for(i=0; i<256; i++)acs[i]=0;
//		for(i=0; i<256; i++)dcsuv[i]=0;
//		for(i=0; i<256; i++)acsuv[i]=0;

		for(i=0; i<256; i++)dcs[i]=1;
		for(i=0; i<256; i++)acs[i]=1;
		for(i=0; i<256; i++)dcsuv[i]=1;
		for(i=0; i<256; i++)acsuv[i]=1;

		j=(xs2/8)*(ys2/8);
		k=(xs3/8)*(ys3/8);
		for(i=0; i<j; i++)PDJHUFF_StatBlock(ydb+i*64, dcs, acs);
		for(i=0; i<k; i++)PDJHUFF_StatBlock(udb+i*64, dcsuv, acsuv);
		for(i=0; i<k; i++)PDJHUFF_StatBlock(vdb+i*64, dcsuv, acsuv);

		PDJHUFF_BuildLengths(dcs, 256, pdjhuff_len+0*256, 16);
		PDJHUFF_BuildLengths(acs, 256, pdjhuff_len+1*256, 16);
		PDJHUFF_BuildLengths(dcsuv, 256, pdjhuff_len+2*256, 16);
		PDJHUFF_BuildLengths(acsuv, 256, pdjhuff_len+3*256, 16);
	}

	pdjhuff_ct=obuf;
	pdjhuff_win=0;
	pdjhuff_pos=0;

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_SOI;

	if(pdjpg_tabcacheframe<=0)
	{
		PDJPG_EmitDQT(0);
		if(!pdjpg_mono)PDJPG_EmitDQT(1);
	}

	PDJPG_EmitSOF(xs, ys);

	if(pdjpg_tabcacheframe<=0)
	{
		PDJPG_EmitDHT(0);
		PDJPG_EmitDHT(1);
		if(!pdjpg_mono)
		{
			PDJPG_EmitDHT(2);
			PDJPG_EmitDHT(3);
		}
	}

	PDJPG_EmitSOS();

	if(pdjpg_mono)
	{
		j=(xs2/8)*(ys2/8);
		for(i=0; i<j; i++)PDJHUFF_EncodeBlock(ydb+i*64, 0, 1);
	}else
	{
		for(i=0; i<=((ys3)/8); i++)
			for(j=0; j<(xs3/8); j++)
		{
			PDJHUFF_EncodeBlock(ydb+((i*2+0)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ydb+((i*2+0)*(xs2/8)+j*2+1)*64, 0, 1);
			PDJHUFF_EncodeBlock(ydb+((i*2+1)*(xs2/8)+j*2+0)*64, 0, 1);
			PDJHUFF_EncodeBlock(ydb+((i*2+1)*(xs2/8)+j*2+1)*64, 0, 1);

			k=i*(xs3/8)+j;
			PDJHUFF_EncodeBlock(udb+k*64, 2, 3);
			PDJHUFF_EncodeBlock(vdb+k*64, 2, 3);
		}
	}

	PDJHUFF_FlushBits();

	*pdjhuff_ct++=0xFF;
	*pdjhuff_ct++=JPG_EOI;

	if(pdjpg_tabcacheframe<=0)
	{
		pdjpg_tabcacheframe=16;
	}else
	{
		pdjpg_tabcacheframe--;
	}

//	free(ydb);
//	free(udb);
//	free(vdb);

//	printf("M3\n");

	i=pdjhuff_ct-obuf;

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

#if 0
byte *PDJPG_ReadImg(FILE *fd, int *xs, int *ys)
{
	byte *buf, *obuf;
	int fsz;

	printf("RM0\n");

	fseek(fd, 0, 2);
	fsz=ftell(fd);
	fseek(fd, 0, 0);

	printf("RM1\n");

	buf=malloc(2*fsz);
	fread(buf, 1, fsz, fd);

	obuf=PDJPG_Decode(buf, fsz, xs, ys);

	printf("RM2\n");

	free(buf);

	return(obuf);
}

int main(int argc, char *argv[])
{
	byte *ibuf, *obuf;
	char *s;
	FILE *fd;
	int w, h, n, fsz;
	int i, j, k, l;

	s=argv[1];
	s+=strlen(s);
	while((s>argv[1]) && (*s!='.'))s--;

	if(!strcmp(s, ".tga"))
	{
		fd=fopen(argv[1], "rb");
		if(!fd)
		{
			printf("Fail Open Input\n");
			return(-1);
		}

		ibuf=Img_LoadTGA(fd, &w, &h);
		fclose(fd);

		obuf=malloc(1<<22);
		fsz=PDJPG_Encode(ibuf, obuf, w, h, 75);

		fd=fopen(argv[2], "wb");
		if(!fd)
		{
			printf("Fail Open Output\n");
			return(-1);
		}

		fwrite(obuf, 1, fsz, fd);
		fclose(fd);
	}

	if(!strcmp(s, ".jpg"))
	{
		fd=fopen(argv[1], "rb");
		if(!fd)
		{
			printf("Fail Open Input\n");
			return(-1);
		}

		obuf=PDJPG_ReadImg(fd, &w, &h);
		fclose(fd);

		printf("RM3\n");

		fd=fopen(argv[2], "wb");
		if(!fd)
		{
			printf("Fail Open Output\n");
			return(-1);
		}

		Img_StoreTGA(fd, obuf, w, h);
		fclose(fd);
	}
}
#endif
