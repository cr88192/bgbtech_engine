#include <pdlib.h>

#include <math.h>

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

byte pdjpg_zigzag2[64];

ushort pdjhuff_code[8*256];
byte pdjhuff_len[8*256];

byte *pdjhuff_cs;	//current pos in bitstream
uint pdjhuff_win;	//bitstream window
int pdjhuff_pos;	//bitstream offset
int pdjhuff_isend;	//bitstream has broken

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

byte *pdjpg_qt[4];
int pdjpg_scid[4];
int pdjpg_scn[4];
short *pdjpg_scbuf[4];
byte *pdjpg_sibuf[4];

static float pdjpg_dct_cv[DCTSZ2];
static float pdjpg_dct_c[DCTSZ2];
static int pdjpg_dct_ci[DCTSZ2];
static int pdjpg_dct_cvi[DCTSZ2];
static int pdjpg_dct_cvi2[DCTSZ2*DCTSZ2];
static int pdjpg_dct_icvi2[DCTSZ2*DCTSZ2];

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

void PDJHUFF_DequantBlock(short *ibuf, short *obuf, int qid)
{
	int i, j;

	for(i=0; i<64; i++)
		obuf[i]=ibuf[i]*pdjpg_qt[qid][i];
}

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

		n=w*h;
		pdjpg_scid[i]=s[0];
		pdjpg_scn[i]=j;
		pdjpg_scbuf[i]=malloc(n*64*sizeof(short));
		pdjpg_sibuf[i]=malloc(n*64);
		ccnt[i]=0;

		s+=2;
	}
	s+=3;

	w=(xi[0]+7)/8;
	h=(yi[0]+7)/8;
	n=(w*h)/step[0];

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
	int i, j, l;

	l=(buf[0]<<8)|buf[1];

	i=(buf[2]>>4)&15;
	if(i)
	{
//		printf("bad bits %d\n", i);
		return(-1);
	}
	i=buf[2]&15;
//	printf("dest %d\n", i);

	pdjpg_qt[i]=buf+3;

//	for(i=0; i<64; i++)
//	{
//		printf("%d ", buf[i+3]);
//	}
//	printf("\n");
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

	*xs=pdjpg_xs;
	*ys=pdjpg_ys;
	return(obuf);
}

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
