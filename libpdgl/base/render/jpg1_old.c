#include <pdlib.h>

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

byte pdjpg_qt[4][64];


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

//Encoder Specific
int pdjpg_mono;

//DCT
static float pdjpg_dct_cv[DCTSZ2];
static float pdjpg_dct_c[DCTSZ2];
static int pdjpg_dct_ci[DCTSZ2];
static int pdjpg_dct_cvi[DCTSZ2];
static int pdjpg_dct_cvi2[DCTSZ2*DCTSZ2];
static int pdjpg_dct_icvi2[DCTSZ2*DCTSZ2];


//Common

void PDJPG_InitDCT()
{
	static int clc=0;
	float c1[DCTSZ], f, f2, f3;
	int i, j, k, l, t;
	float u, v;
	float q1, q2, q3;

	if(clc)return;
	clc=1;	

	for(i=0; i<DCTSZ; i++)
		c1[i]=sqrt(2.0/DCTSZ);
	c1[0]=sqrt(1.0/DCTSZ);

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		pdjpg_dct_c[i*DCTSZ+j]=c1[i]*c1[j];
		pdjpg_dct_ci[i*DCTSZ+j]=round(c1[i]*c1[j]*65536);
	}

	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		f=cos((M_PI*i)/(2*DCTSZ)*(2*j+1));
		pdjpg_dct_cv[i*DCTSZ+j]=f;
		pdjpg_dct_cvi[i*DCTSZ+j]=round(f*256);
	}

	t=0;
	for(i=0; i<DCTSZ; i++)for(j=0; j<DCTSZ; j++)
	{
		for(k=0; k<DCTSZ; k++)
			for(l=0; l<DCTSZ; l++)
		{
			f=cos((M_PI*i)/(2*DCTSZ)*(2*k+1))*
				cos((M_PI*j)/(2*DCTSZ)*(2*l+1));
			pdjpg_dct_cvi2[t]=round(f*65536);

			f=pdjpg_dct_c[(k*DCTSZ)+l]*
				cos((M_PI*k)/(2*DCTSZ)*(2*i+1))*
				cos((M_PI*l)/(2*DCTSZ)*(2*j+1));
			pdjpg_dct_icvi2[t]=round(f*65536);

			t++;
		}
	}
}

void PDJPG_TransDCT(byte *iblk, short *oblk)
{
	int i, j, k, l;
	int s;

	k=0;
	for(i=0; i<DCTSZ2; i++)
	{
		s=0;
		for(j=0; j<DCTSZ2; j++)
			s+=(iblk[j]-128)*pdjpg_dct_cvi2[k++];
		s>>=16;
		s*=pdjpg_dct_ci[i];
		oblk[i]=s>>16;
	}
}

void PDJPG_TransIDCT(short *iblk, byte *oblk)
{
	int i, j, k, l;
	int s;

	k=0;
	for(i=0; i<DCTSZ2; i++)
	{
		s=0;
		for(j=0; j<DCTSZ2; j++)
			s+=iblk[j]*pdjpg_dct_icvi2[k++];
		s=(s>>16)+128;
		s=(s<0)?0:((s>255)?255:s);
		oblk[i]=s;
	}
}


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
		if(!j)break;

		i+=j>>4;	//preceding 0's
		if(!(j&15))continue;

		j&=15;
		k=PDJHUFF_ReadNBits(j);
		if(!(k&(1<<(j-1))))
			k=(-1<<j)+k+1;

		j=pdjpg_zigzag2[i];
		buf[j]=k;
	}

	if(i>64)printf("bad dct block1 %02X %d/%d\n", j, ni, nn);

	return(0);
}

void PDJHUFF_QuantBlock(short *ibuf, short *obuf, int qid)
{
	int i, j;
	for(i=0; i<64; i++)
		obuf[i]=ibuf[i]/pdjpg_qt[qid][i];
}

void PDJHUFF_DequantBlock(short *ibuf, short *obuf, int qid)
{
	int i, j;
	for(i=0; i<64; i++)
		obuf[i]=ibuf[i]*pdjpg_qt[qid][i];
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
	int w, h, n, ns;

	l=(buf[0]<<8)|buf[1];
//	printf("ns %d\n", buf[2]);

	ns=buf[2];
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

	for(i=0; i<n; i++)
	{
		for(j=0; j<ns; j++)
			for(k=0; k<cv[j]; k++)
				for(l=0; l<ch[j]; l++)
		{
			i1=(i/(wi[j]/ch[j]))*ch[j];
			j1=(i%(wi[j]/ch[j]))*ch[j];
			k1=((i1+k)*wi[j])+(j1+l);

			PDJHUFF_DecodeBlock(dbuf+j*64,
				cdt[j]*2+0, cat[j]*2+1, i, n);
			PDJHUFF_DequantBlock(dbuf+j*64,
				pdjpg_scbuf[j]+k1*64, qid[j]);
			PDJPG_TransIDCT(pdjpg_scbuf[j]+k1*64,
				pdjpg_sibuf[j]+k1*64);
		}
	}
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
}

int PDJPG_GetComponentPixel(int cn, int x, int y)
{
	int i, j, k, l;
	int id, w, h, n, v;

	id=pdjpg_scn[cn];

	i=(pdjpg_ch[id]*x)/pdjpg_chm;
	j=(pdjpg_cv[id]*y)/pdjpg_chn;
	k=i/8;
	l=j/8;

	w=(pdjpg_cxi[id]+7)/8;
	h=(pdjpg_cyi[id]+7)/8;
	n=(l*w)+k;

	k=i%8;
	l=j%8;

	v=pdjpg_sibuf[cn][n*64+(l*8+k)];
	return(v);
}

byte *PDJPG_Decode(byte *buf, int sz, int *xs, int *ys)
{
	byte *obuf;
	int i, j, k, l;
	int y, u, v, r, g, b;

	if((buf[0]!=0xFF) || (buf[1]!=JPG_SOI))
		return(NULL);

	for(i=0; i<64; i++)pdjpg_zigzag2[pdjpg_zigzag[i]]=i;
	PDJPG_InitDCT();

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

	obuf=malloc(pdjpg_xs*pdjpg_ys*4);
	memset(obuf, 0xFF, pdjpg_xs*pdjpg_ys*4);

	for(i=0; i<pdjpg_ys; i++)
		for(j=0; j<pdjpg_xs; j++)
	{
		if(pdjpg_nc==3)
		{
			y=PDJPG_GetComponentPixel(0, j, i);
			u=PDJPG_GetComponentPixel(1, j, i);
			v=PDJPG_GetComponentPixel(2, j, i);

			r=y+				1.402*(v-128);
			g=y-	0.34414*(u-128)-	0.71414*(v-128);
			b=y+	1.772*(u-128);

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

	printf("DHT %04X\n", k);

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

int PDJPG_Encode(byte *ibuf, byte *obuf, int xs, int ys, int qf)
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
	xs3=(xs2+1)/2;
	ys3=(ys2+1)/2;

	pdjpg_mono=0;

//	for(i=0; i<64; i++)pdjpg_zigzag2[pdjpg_zigzag[i]]=i;
	PDJPG_InitDCT();

//	for(i=0; i<8; i++)
//	{
//		for(j=0; j<8; j++)
//			printf("%d ", pdjpg_zigzag[i*8+j]);
//		printf("\n");
//	}

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

	printf("M0\n");

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

	printf("M1\n");


	PDJPG_FilterImageDCT(yb, ydb, xs2, ys2);
	PDJPG_FilterImageDCT(ub, udb, xs3, ys3);
	PDJPG_FilterImageDCT(vb, vdb, xs3, ys3);

	j=(xs2/8)*(ys2/8);
	PDJPG_MakeQuantTabInput(ydb, j, pdjpg_qt[0], qf/100.0);

	j=(xs3/8)*(ys3/8);
	PDJPG_MakeQuantTabInput(udb, j, pdjpg_qt[1], qf/100.0);
	PDJPG_MakeQuantTabInput(vdb, j, pdjpg_qt[2], qf/100.0);
	for(i=0; i<64; i++)pdjpg_qt[1][i]=(pdjpg_qt[1][i]+pdjpg_qt[2][i])/2;

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

	printf("M2\n");

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

	printf("M3\n");

	i=pdjhuff_ct-obuf;

	return(i);
}

#if 1
byte *PDJPG_Load(VFILE *fd, int *xs, int *ys)
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

int PDJPG_Store(VFILE *fd, byte *ibuf, int xs, int ys, int qf)
{
	byte *buf;
	int fsz;

	buf=malloc(1<<22);

	fsz=PDJPG_Encode(ibuf, buf, xs, ys, qf);
	if(fsz<0)return(fsz);

	vfwrite(buf, 1, fsz, fd);
	free(buf);

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
