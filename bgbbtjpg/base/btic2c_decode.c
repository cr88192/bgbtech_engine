#include <bgbbtj.h>

extern const int pdjpg_zigzag[64];
extern const int pdjpg_zigzag2[64];

extern const u32 btic2c_dbase[64];
extern const u64 btic2c_dbase2[128];
extern const int btic2c_dextra[128];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

#if 1
void BTIC2C_TransIDCT_Horiz(s32 *iblk, s32 *oblk)
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

void BTIC2C_TransIDCT_Vert(s32 *iblk, s32 *oblk)
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

void BTIC2C_TransIDCT(s32 *iblk, s32 *oblk)
{
	int s[DCTSZ2];
	int t[DCTSZ2];
	int i, j;

	BTIC2C_TransIDCT_Horiz(iblk+0, s+0);
	BTIC2C_TransIDCT_Horiz(iblk+8, s+8);
	BTIC2C_TransIDCT_Horiz(iblk+16, s+16);
	BTIC2C_TransIDCT_Horiz(iblk+24, s+24);
	BTIC2C_TransIDCT_Horiz(iblk+32, s+32);
	BTIC2C_TransIDCT_Horiz(iblk+40, s+40);
	BTIC2C_TransIDCT_Horiz(iblk+48, s+48);
	BTIC2C_TransIDCT_Horiz(iblk+56, s+56);

	BTIC2C_TransIDCT_Vert(s+0, t+0);
	BTIC2C_TransIDCT_Vert(s+1, t+1);
	BTIC2C_TransIDCT_Vert(s+2, t+2);
	BTIC2C_TransIDCT_Vert(s+3, t+3);
	BTIC2C_TransIDCT_Vert(s+4, t+4);
	BTIC2C_TransIDCT_Vert(s+5, t+5);
	BTIC2C_TransIDCT_Vert(s+6, t+6);
	BTIC2C_TransIDCT_Vert(s+7, t+7);

	for(i=0; i<64; i++)
	{
//		j=(t[i]>>16);
		j=((t[i]+32768)>>16);
		oblk[i]=j;
//		oblk[i]=(j<0)?0:((j>255)?255:j);
	}
}
#endif

void BTIC2C_TransIDCT_Flat(s32 *iblk, s32 *oblk)
{
	s32 *ob, *obe;
	int i, j, k;

	/* If DCT block is a flat color, fill directly */
	k=(iblk[0]*8281+32768)>>16;
//	k=((iblk[0]*8281+32768)>>16)+dcbias;
//	k=((iblk[0]*8192+32768)>>16)+dcbias;
	ob=oblk; obe=ob+64;
//	for(i=0; i<8; i++)
	while(ob<obe)
	{
		ob[ 0]=k; ob[ 1]=k; ob[ 2]=k; ob[ 3]=k;
		ob[ 4]=k; ob[ 5]=k; ob[ 6]=k; ob[ 7]=k;
		ob[ 8]=k; ob[ 9]=k; ob[10]=k; ob[11]=k;
		ob[12]=k; ob[13]=k; ob[14]=k; ob[15]=k;
		ob+=16;
	}
}

void BTIC2C_TransIDCT_FlatS(s32 *iblk, s16 *oblk)
{
	short *ob, *obe;
	int i, j, k;

	/* If DCT block is a flat color, fill directly */
	k=(iblk[0]*8281+32768)>>16;
//	k=((iblk[0]*8281+32768)>>16)+dcbias;
//	k=((iblk[0]*8192+32768)>>16)+dcbias;
	ob=oblk; obe=ob+64;
//	for(i=0; i<8; i++)
	while(ob<obe)
	{
		ob[ 0]=k; ob[ 1]=k; ob[ 2]=k; ob[ 3]=k;
		ob[ 4]=k; ob[ 5]=k; ob[ 6]=k; ob[ 7]=k;
		ob[ 8]=k; ob[ 9]=k; ob[10]=k; ob[11]=k;
		ob[12]=k; ob[13]=k; ob[14]=k; ob[15]=k;
		ob+=16;
	}
}

#if 1
void BTIC2C_TransIWHT_Horiz(s32 *iblk, s32 *oblk)
{
	int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	int a0, a1, a2, a3, a4, a5, a6, a7;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	int c0, c1, c2, c3, c4, c5, c6, c7;

	c0=iblk[0];		c4=iblk[1];
	c6=iblk[2];		c2=iblk[3];
	c3=iblk[4];		c7=iblk[5];
	c5=iblk[6];		c1=iblk[7];

	b1=(c0>>1)-c1;	b3=(c2>>1)-c3;
	b5=(c4>>1)-c5;	b7=(c6>>1)-c7;
	b0=c0-b1;	b2=c2-b3;
	b4=c4-b5;	b6=c6-b7;
	a2=(b0>>1)-b2;	a3=(b1>>1)-b3;
	a6=(b4>>1)-b6;	a7=(b5>>1)-b7;
	a0=b0-a2;	a1=b1-a3;
	a4=b4-a6;	a5=b5-a7;
	ob4=(a0>>1)-a4;	ob5=(a1>>1)-a5;
	ob6=(a2>>1)-a6;	ob7=(a3>>1)-a7;
	ob0=a0-ob4;	ob1=a1-ob5;
	ob2=a2-ob6;	ob3=a3-ob7;

	oblk[0]=ob0;	oblk[1]=ob1;
	oblk[2]=ob2;	oblk[3]=ob3;
	oblk[4]=ob4;	oblk[5]=ob5;
	oblk[6]=ob6;	oblk[7]=ob7;
}

void BTIC2C_TransIWHT_Vert(s32 *iblk, s32 *oblk)
{
	int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	int a0, a1, a2, a3, a4, a5, a6, a7;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	int c0, c1, c2, c3, c4, c5, c6, c7;

	c0=iblk[8*0];	c4=iblk[8*1];
	c6=iblk[8*2];	c2=iblk[8*3];
	c3=iblk[8*4];	c7=iblk[8*5];
	c5=iblk[8*6];	c1=iblk[8*7];

	b1=(c0>>1)-c1;	b3=(c2>>1)-c3;
	b5=(c4>>1)-c5;	b7=(c6>>1)-c7;
	b0=c0-b1;	b2=c2-b3;
	b4=c4-b5;	b6=c6-b7;
	a2=(b0>>1)-b2;	a3=(b1>>1)-b3;
	a6=(b4>>1)-b6;	a7=(b5>>1)-b7;
	a0=b0-a2;	a1=b1-a3;
	a4=b4-a6;	a5=b5-a7;
	ob4=(a0>>1)-a4;	ob5=(a1>>1)-a5;
	ob6=(a2>>1)-a6;	ob7=(a3>>1)-a7;
	ob0=a0-ob4;	ob1=a1-ob5;
	ob2=a2-ob6;	ob3=a3-ob7;

	oblk[8*0]=ob0;	oblk[8*1]=ob1;
	oblk[8*2]=ob2;	oblk[8*3]=ob3;
	oblk[8*4]=ob4;	oblk[8*5]=ob5;
	oblk[8*6]=ob6;	oblk[8*7]=ob7;
}

void BTIC2C_TransIWHT(s32 *iblk, s32 *oblk)
{
	s32 s[DCTSZ2];

	BTIC2C_TransIWHT_Vert(iblk+0, s+0);
	BTIC2C_TransIWHT_Vert(iblk+1, s+1);
	BTIC2C_TransIWHT_Vert(iblk+2, s+2);
	BTIC2C_TransIWHT_Vert(iblk+3, s+3);
	BTIC2C_TransIWHT_Vert(iblk+4, s+4);
	BTIC2C_TransIWHT_Vert(iblk+5, s+5);
	BTIC2C_TransIWHT_Vert(iblk+6, s+6);
	BTIC2C_TransIWHT_Vert(iblk+7, s+7);

	BTIC2C_TransIWHT_Horiz(s+0, oblk+0);
	BTIC2C_TransIWHT_Horiz(s+8, oblk+8);
	BTIC2C_TransIWHT_Horiz(s+16, oblk+16);
	BTIC2C_TransIWHT_Horiz(s+24, oblk+24);
	BTIC2C_TransIWHT_Horiz(s+32, oblk+32);
	BTIC2C_TransIWHT_Horiz(s+40, oblk+40);
	BTIC2C_TransIWHT_Horiz(s+48, oblk+48);
	BTIC2C_TransIWHT_Horiz(s+56, oblk+56);
}
#endif

#if 1
void BTIC2C_TransIWHT_HorizS(s32 *iblk, s16 *oblk)
{
	int ob0, ob1, ob2, ob3, ob4, ob5, ob6, ob7;
	int a0, a1, a2, a3, a4, a5, a6, a7;
	int b0, b1, b2, b3, b4, b5, b6, b7;
	int c0, c1, c2, c3, c4, c5, c6, c7;

	c0=iblk[0];		c4=iblk[1];
	c6=iblk[2];		c2=iblk[3];
	c3=iblk[4];		c7=iblk[5];
	c5=iblk[6];		c1=iblk[7];

	b1=(c0>>1)-c1;	b3=(c2>>1)-c3;
	b5=(c4>>1)-c5;	b7=(c6>>1)-c7;
	b0=c0-b1;	b2=c2-b3;
	b4=c4-b5;	b6=c6-b7;
	a2=(b0>>1)-b2;	a3=(b1>>1)-b3;
	a6=(b4>>1)-b6;	a7=(b5>>1)-b7;
	a0=b0-a2;	a1=b1-a3;
	a4=b4-a6;	a5=b5-a7;
	ob4=(a0>>1)-a4;	ob5=(a1>>1)-a5;
	ob6=(a2>>1)-a6;	ob7=(a3>>1)-a7;
	ob0=a0-ob4;	ob1=a1-ob5;
	ob2=a2-ob6;	ob3=a3-ob7;

	oblk[0]=ob0;	oblk[1]=ob1;
	oblk[2]=ob2;	oblk[3]=ob3;
	oblk[4]=ob4;	oblk[5]=ob5;
	oblk[6]=ob6;	oblk[7]=ob7;
}

void BTIC2C_TransIWHTS(s32 *iblk, s16 *oblk)
{
	s32 s[DCTSZ2];

	BTIC2C_TransIWHT_Vert(iblk+0, s+0);
	BTIC2C_TransIWHT_Vert(iblk+1, s+1);
	BTIC2C_TransIWHT_Vert(iblk+2, s+2);
	BTIC2C_TransIWHT_Vert(iblk+3, s+3);
	BTIC2C_TransIWHT_Vert(iblk+4, s+4);
	BTIC2C_TransIWHT_Vert(iblk+5, s+5);
	BTIC2C_TransIWHT_Vert(iblk+6, s+6);
	BTIC2C_TransIWHT_Vert(iblk+7, s+7);

	BTIC2C_TransIWHT_HorizS(s+0, oblk+0);
	BTIC2C_TransIWHT_HorizS(s+8, oblk+8);
	BTIC2C_TransIWHT_HorizS(s+16, oblk+16);
	BTIC2C_TransIWHT_HorizS(s+24, oblk+24);
	BTIC2C_TransIWHT_HorizS(s+32, oblk+32);
	BTIC2C_TransIWHT_HorizS(s+40, oblk+40);
	BTIC2C_TransIWHT_HorizS(s+48, oblk+48);
	BTIC2C_TransIWHT_HorizS(s+56, oblk+56);
}
#endif

void BTIC2C_TransIWHT_Horiz_Flat(s32 v, s32 *oblk)
{
	oblk[0]=oblk[1]=oblk[2]=oblk[3]=v;
	oblk[4]=oblk[5]=oblk[6]=oblk[7]=v;
}

void BTIC2C_TransIWHT_Horiz_FlatS(s16 v, s16 *oblk)
{
	oblk[0]=oblk[1]=oblk[2]=oblk[3]=v;
	oblk[4]=oblk[5]=oblk[6]=oblk[7]=v;
}

void BTIC2C_TransIWHT_Flat(s32 *iblk, s32 *oblk)
{
	s32 v;
	
	v=iblk[0]>>6;
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+ 0);
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+ 8);
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+16);
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+24);
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+32);
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+40);
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+48);
	BTIC2C_TransIWHT_Horiz_Flat(v, oblk+56);
}

void BTIC2C_TransIWHT_FlatS(s32 *iblk, s16 *oblk)
{
	s16 v;
	
	v=iblk[0]>>6;
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+ 0);
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+ 8);
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+16);
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+24);
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+32);
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+40);
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+48);
	BTIC2C_TransIWHT_Horiz_FlatS(v, oblk+56);
}

void BTIC2C_TransIBLKT(s32 *iblk, s32 *oblk, int blkt)
{
	if(blkt==BTIC2C_BLKT_WHT)
	{
		BTIC2C_TransIWHT(iblk, oblk);
		return;
	}else if((blkt==BTIC2C_BLKT_DCT) || (blkt==BTIC2C_BLKT_RDCT))
	{
		BTIC2C_TransIDCT(iblk, oblk);
		return;
	}
}

void BTIC2C_TransIBLKT_Flat(s32 *iblk, s32 *oblk, int blkt)
{
	if(blkt==BTIC2C_BLKT_WHT)
	{
		BTIC2C_TransIWHT_Flat(iblk, oblk);
		return;
	}

	if((blkt==BTIC2C_BLKT_DCT) || (blkt==BTIC2C_BLKT_RDCT))
	{
		BTIC2C_TransIDCT_Flat(iblk, oblk);
		return;
	}

	BTIC2C_TransIWHT_Flat(iblk, oblk);
}

void BTIC2C_TransIBLKT_Add(s32 *iblk, s32 *oblk, int blkt)
{
	s32 s[DCTSZ2];
	s32 *cs, *ct;
	int i;
	
	BTIC2C_TransIBLKT(iblk, s, blkt);
	cs=s; ct=oblk;
	for(i=0; i<8; i++)
	{
		ct[0]+=cs[0];	ct[1]+=cs[1];
		ct[2]+=cs[2];	ct[3]+=cs[3];
		ct[4]+=cs[4];	ct[5]+=cs[5];
		ct[6]+=cs[6];	ct[7]+=cs[7];
		ct+=8; cs+=8;
	}
}

void BTIC2C_TransIBLKT_MoveAdd(s32 *iblk, s32 *oblk,
	s32 *img, int xo, int yo, int xs, int ys, int blkt)
{
	s32 s[DCTSZ2];
	s32 t[DCTSZ2];
	s32 *cs0, *cs1, *ct;
	int i;

#if 0
	if((xo<0) || (yo<0) || ((xo+8)>xs) || ((yo+8)>ys))
	{
		for(i=0; i<DCTSZ2; i++)
		{
			oblk[i]=(i&1)?255:0;
		}
		return;
	}
#endif

//	BTIC2C_GetImgBlk(t, xo, yo, img, xs, ys);
	BTIC2C_GetImgPlaneBlk(t, xo, yo, img, xs, ys);
	
	BTIC2C_TransIBLKT(iblk, s, blkt);
	cs0=t; cs1=s; ct=oblk;
	for(i=0; i<8; i++)
	{
		ct[0]=cs0[0]+cs1[0];	ct[1]=cs0[1]+cs1[1];
		ct[2]=cs0[2]+cs1[2];	ct[3]=cs0[3]+cs1[3];
		ct[4]=cs0[4]+cs1[4];	ct[5]=cs0[5]+cs1[5];
		ct[6]=cs0[6]+cs1[6];	ct[7]=cs0[7]+cs1[7];
		ct+=8; cs0+=8; cs1+=8;
	}
}

void BTIC2C_TransIBLKT_Move(s32 *oblk,
	s32 *img, int xo, int yo, int xs, int ys, int blkt)
{
	s32 s[DCTSZ2];
	s32 t[DCTSZ2];
	s32 *cs0, *cs1, *ct;
	int i;

#if 0
	if((xo<0) || (yo<0) || ((xo+8)>xs) || ((yo+8)>ys))
	{
		for(i=0; i<DCTSZ2; i++)
		{
			oblk[i]=(i&1)?255:0;
		}
		return;
	}
#endif

//	BTIC2C_GetImgBlk(t, xo, yo, img, xs, ys);
	BTIC2C_GetImgPlaneBlk(t, xo, yo, img, xs, ys);
	
//	BTIC2C_TransIBLKT(iblk, s, blkt);
	cs0=t; ct=oblk;
	for(i=0; i<8; i++)
	{
		ct[0]=cs0[0];	ct[1]=cs0[1];
		ct[2]=cs0[2];	ct[3]=cs0[3];
		ct[4]=cs0[4];	ct[5]=cs0[5];
		ct[6]=cs0[6];	ct[7]=cs0[7];
		ct+=8; cs0+=8;
	}
}

void BTIC2C_TransIBLKS(s32 *iblk, s16 *oblk, int blkt)
{
	s32 s[DCTSZ2];
	s32 *cs;
	s16 *ct;
	int i;

#if 1
	if(blkt==BTIC2C_BLKT_WHT)
	{
		BTIC2C_TransIWHTS(iblk, oblk);
		return;
	}
#endif

	BTIC2C_TransIBLKT(iblk, s, blkt);
	cs=s; ct=oblk;
	for(i=0; i<8; i++)
	{
		ct[0]=cs[0];	ct[1]=cs[1];
		ct[2]=cs[2];	ct[3]=cs[3];
		ct[4]=cs[4];	ct[5]=cs[5];
		ct[6]=cs[6];	ct[7]=cs[7];
		ct+=8; cs+=8;
	}
}

void BTIC2C_TransIBLKS_Flat(s32 *iblk, s16 *oblk, int blkt)
{
	if(blkt==BTIC2C_BLKT_WHT)
	{
		BTIC2C_TransIWHT_FlatS(iblk, oblk);
		return;
	}

	if((blkt==BTIC2C_BLKT_DCT) || (blkt==BTIC2C_BLKT_RDCT))
	{
		BTIC2C_TransIDCT_FlatS(iblk, oblk);
		return;
	}

	BTIC2C_TransIWHT_FlatS(iblk, oblk);
}

void BTIC2C_TransIBLKS_Add(s32 *iblk, s16 *oblk, int blkt)
{
	s32 s[DCTSZ2];
	s32 *cs;
	s16 *ct;
	int i;
	
	BTIC2C_TransIBLKT(iblk, s, blkt);
	cs=s; ct=oblk;
	for(i=0; i<8; i++)
	{
		ct[0]+=cs[0];	ct[1]+=cs[1];
		ct[2]+=cs[2];	ct[3]+=cs[3];
		ct[4]+=cs[4];	ct[5]+=cs[5];
		ct[6]+=cs[6];	ct[7]+=cs[7];
		ct+=8; cs+=8;
	}
}

void BTIC2C_GetImgPlaneBlk(
	s32 *blk, int xo, int yo,
	s32 *img, int xs, int ys)
{
	int x, y, bx, by, px, py, w;
	int i, j, k, l, n;

	if((xo<0) || (yo<0) || ((xo+8)>xs) || ((yo+8)>ys))
	{
		w=xs>>3;
		for(i=0; i<8; i++)
			for(j=0; j<8; j++)
		{
			x=xo+j;		y=yo+i;
			x=(x<0)?0:((x>xs)?xs:x);
			y=(y<0)?0:((y>ys)?ys:y);
			
			bx=x>>3;	by=y>>3;
			px=x&7;		py=y&7;
			n=(by*w)+bx;
			k=img[n*64+(py*8+px)];
			blk[i*8+j]=k;
		}

		return;
	}

	w=xs>>3;
	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
	{
		x=xo+j;		y=yo+i;
		bx=x>>3;	by=y>>3;
		px=x&7;		py=y&7;
		n=(by*w)+bx;
		k=img[n*64+(py*8+px)];
		blk[i*8+j]=k;
	}
}

void BTIC2C_GetImgPlaneBlkS(
	s32 *blk, int xo, int yo,
	s16 *img, int xs, int ys)
{
	int x, y, bx, by, px, py, w;
	int i, j, k, l, n;

	if((xo<0) || (yo<0) || ((xo+8)>xs) || ((yo+8)>ys))
	{
		w=xs>>3;
		for(i=0; i<8; i++)
			for(j=0; j<8; j++)
		{
			x=xo+j;		y=yo+i;
			x=(x<0)?0:((x>xs)?xs:x);
			y=(y<0)?0:((y>ys)?ys:y);
			
			bx=x>>3;	by=y>>3;
			px=x&7;		py=y&7;
			n=(by*w)+bx;
			k=img[n*64+(py*8+px)];
			blk[i*8+j]=k;
		}

		return;
	}

	w=xs>>3;
	for(i=0; i<8; i++)
		for(j=0; j<8; j++)
	{
		x=xo+j;		y=yo+i;
		bx=x>>3;	by=y>>3;
		px=x&7;		py=y&7;
		n=(by*w)+bx;
		k=img[n*64+(py*8+px)];
		blk[i*8+j]=k;
	}
}

void BTIC2C_TransIBLKS_MoveAdd(s32 *iblk, s16 *oblk,
	s16 *img, int xo, int yo, int xs, int ys, int blkt)
{
	s32 s[DCTSZ2];
	s32 t[DCTSZ2];
	s32 *cs0, *cs1;
	s16 *ct;
	int i;

#if 0
	if((xo<0) || (yo<0) || ((xo+8)>xs) || ((yo+8)>ys))
	{
		for(i=0; i<DCTSZ2; i++)
		{
//			oblk[i]=(i&1)?255:0;
			oblk[i]=((i^(i>>3))&1)?255:0;
		}
		return;
	}
#endif

//	BTIC2C_GetImgBlk(t, xo, yo, img, xs, ys);
//	BTIC2C_GetImgBlkS(t, xo, yo, img, xs, ys);
	BTIC2C_GetImgPlaneBlkS(t, xo, yo, img, xs, ys);
	
	BTIC2C_TransIBLKT(iblk, s, blkt);
	cs0=t; cs1=s; ct=oblk;
	for(i=0; i<8; i++)
	{
		ct[0]=cs0[0]+cs1[0];	ct[1]=cs0[1]+cs1[1];
		ct[2]=cs0[2]+cs1[2];	ct[3]=cs0[3]+cs1[3];
		ct[4]=cs0[4]+cs1[4];	ct[5]=cs0[5]+cs1[5];
		ct[6]=cs0[6]+cs1[6];	ct[7]=cs0[7]+cs1[7];
		ct+=8; cs0+=8; cs1+=8;
	}
}

void BTIC2C_TransIBLKS_Move(s16 *oblk,
	s16 *img, int xo, int yo, int xs, int ys, int blkt)
{
	s32 s[DCTSZ2];
	s32 t[DCTSZ2];
	s32 *cs0, *cs1;
	s16 *ct;
	int i;

#if 0
	if((xo<0) || (yo<0) || ((xo+8)>xs) || ((yo+8)>ys))
	{
		for(i=0; i<DCTSZ2; i++)
		{
//			oblk[i]=(i&1)?255:0;
			oblk[i]=((i^(i>>3))&1)?255:0;
		}
		return;
	}
#endif

//	BTIC2C_GetImgBlk(t, xo, yo, img, xs, ys);
//	BTIC2C_GetImgBlkS(t, xo, yo, img, xs, ys);
	BTIC2C_GetImgPlaneBlkS(t, xo, yo, img, xs, ys);
	
//	BTIC2C_TransIBLKT(iblk, s, blkt);
	cs0=t; ct=oblk;
	for(i=0; i<8; i++)
	{
		ct[0]=cs0[0];	ct[1]=cs0[1];
		ct[2]=cs0[2];	ct[3]=cs0[3];
		ct[4]=cs0[4];	ct[5]=cs0[5];
		ct[6]=cs0[6];	ct[7]=cs0[7];
		ct+=8; cs0+=8;
	}
}


//Decoder

int BTIC2CH_NextByte_Basic(BTIC2C_Context *ctx)
{
	int i, j;
	i=*ctx->huff_cs++;
	return(i);
}

int BTIC2CH_NextByte_Arith(BTIC2C_Context *ctx)
{
	int i, j;
	i=BTIC2C_BitArith_DecodeSymbol8(ctx,
		ctx->ari_model, ctx->ari_ctxmask);
	return(i);
}

int BTIC2CH_NextByte(BTIC2C_Context *ctx)
	{ return(ctx->Huff_NextByte(ctx)); }

int BTIC2CH_InitStream(BTIC2C_Context *ctx, byte *buf)
{
	ctx->Huff_NextByte=BTIC2CH_NextByte_Basic;

	ctx->huff_pos=0;
	ctx->huff_isend=0;

	ctx->huff_cs=buf;
	ctx->huff_win=BTIC2CH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
	return(0);
}

int BTIC2CH_InitStreamArith(BTIC2C_Context *ctx,
	byte *buf, int szbuf, int bits)
{
	BTIC2C_BitArith_Init();
	BTIC2C_BitArith_SetupContextBits(ctx, 8+bits);
	BTIC2C_BitArith_SetupDecode(ctx, buf, szbuf);

	ctx->Huff_NextByte=BTIC2CH_NextByte_Arith;

	ctx->huff_pos=0;
	ctx->huff_isend=0;

	ctx->huff_cs=NULL;
	ctx->huff_win=BTIC2CH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
	ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
	return(0);
}

int BTIC2CH_ReadBit(BTIC2C_Context *ctx)
{
	int i;

	i=(ctx->huff_win>>(31-ctx->huff_pos))&1;
	ctx->huff_pos++;
	if(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
		ctx->huff_pos-=8;
	}
	return(i);
}

int BTIC2CH_ReadNBits(BTIC2C_Context *ctx, int n)
{
	int i;

	i=(ctx->huff_win>>(32-n-ctx->huff_pos))&((1<<n)-1);
	ctx->huff_pos+=n;
	while(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
		ctx->huff_pos-=8;
	}
	return(i);
}

u32 BTIC2CH_ReadNBitsL(BTIC2C_Context *ctx, int n)
{
	u32 v;

	if(n>16)
	{
		v=BTIC2CH_ReadNBitsL(ctx, n-16);
		v=(v<<16)|BTIC2CH_ReadNBits(ctx, n-16);
		return(v);
	}
	
	return(BTIC2CH_ReadNBits(ctx, n));
}

u64 BTIC2CH_ReadNBitsX(BTIC2C_Context *ctx, int n)
{
	u64 v;

	if(n>16)
	{
		v=BTIC2CH_ReadNBitsX(ctx, n-16);
		v=(v<<16)|BTIC2CH_ReadNBits(ctx, n-16);
		return(v);
	}
	
	return(BTIC2CH_ReadNBits(ctx, n));
}

int BTIC2CH_Read2Bits(BTIC2C_Context *ctx)
	{ return(BTIC2CH_ReadNBits(ctx, 2)); }
int BTIC2CH_Read3Bits(BTIC2C_Context *ctx)
	{ return(BTIC2CH_ReadNBits(ctx, 3)); }
int BTIC2CH_Read4Bits(BTIC2C_Context *ctx)
	{ return(BTIC2CH_ReadNBits(ctx, 4)); }
int BTIC2CH_Read5Bits(BTIC2C_Context *ctx)
	{ return(BTIC2CH_ReadNBits(ctx, 5)); }

void BTIC2CH_SkipNBits(BTIC2C_Context *ctx, int n)
{
	ctx->huff_pos+=n;
	while(ctx->huff_pos>=8)
	{
		ctx->huff_win=(ctx->huff_win<<8)|BTIC2CH_NextByte(ctx);
		ctx->huff_pos-=8;
	}
}

int BTIC2CH_ReadRice(BTIC2C_Context *ctx, int n)
{
	int i, j;
	
	i=0;
	while(BTIC2CH_ReadBit(ctx))i++;
	j=BTIC2CH_ReadNBits(ctx, n);
	return((i<<n)|j);
}

int BTIC2CH_ReadSRice(BTIC2C_Context *ctx, int n)
{
	int i;
	i=BTIC2CH_ReadRice(ctx, n);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC2CH_ReadAdRice(BTIC2C_Context *ctx, int *rk)
{
	int v, p;
	int i, j, k;
	
	k=*rk;
	if(k<0)
	{
		i=BTIC2CH_ReadRice(ctx, -k);
		return(i);
	}
	
	p=0;
	while(BTIC2CH_ReadBit(ctx))p++;
	i=BTIC2CH_ReadNBits(ctx, k);
	v=(p<<k)|i;

	if(!p && (k>0))k--;
	if(p>1)
	{
		j=p;
		while(j>1)
			{ k++; j>>=1; }
	}
	*rk=k;

	return(v);
}

int BTIC2CH_ReadAdSRice(BTIC2C_Context *ctx, int *rk)
{
	int i;
	i=BTIC2CH_ReadAdRice(ctx, rk);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

int BTIC2CH_PeekWord(BTIC2C_Context *ctx)
{
	int i;
	i=(ctx->huff_win>>(16-ctx->huff_pos))&65535;
	return(i);
}

int BTIC2CH_DecodeSymbol(BTIC2C_Context *ctx, int tab)
{
	int i, j, k, l, t;

	i=(byte)(pdjhuff_win>>(24-pdjhuff_pos));
	
	t=tab<<8;
	j=ctx->huff_idx[t|i];

	k=ctx->huff_len[t|j];
	if(k<=8)
	{
		pdjhuff_pos+=k;
		if(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|BTIC2CH_NextByte(ctx);
			pdjhuff_pos-=8;
		}
		return(j);
	}

	i=(u16)(pdjhuff_win>>(16-pdjhuff_pos));
	j=ctx->huff_idx[t|(i>>8)];

	while(1)
	{
		l=t|j;
		k=ctx->huff_len[l];

		if((i>>(16-k))!=ctx->huff_code[l])
		{
			j=ctx->huff_next[l];
			continue;
		}

		pdjhuff_pos+=k;
		while(pdjhuff_pos>=8)
		{
			pdjhuff_win=(pdjhuff_win<<8)|BTIC2CH_NextByte(ctx);
			pdjhuff_pos-=8;
		}
		return(j);
	}

	return(-1);
}

int BTIC2CH_DecodeDiffDC(BTIC2C_Context *ctx, int tab, int cn, int *rfl)
{
	int arg[16];
	int cmd, narg;
	int i, j, k, l;

	i=BTIC2CH_DecodeSymbol(ctx, tab);

	while(i>=128)
	{
		j=(i>>5)&7;
		if(j==4)
		{
			j=i&31;
			cmd=btic2c_lbase[j]+BTIC2CH_ReadNBits(ctx, btic2c_lextra[j]);
			narg=0;
			i=BTIC2CH_DecodeSymbol(ctx, tab);
			while(((i>>5)&7)==5)
			{
				j=i&31;
				k=btic2c_dbase[j]+BTIC2CH_ReadNBitsL(ctx, btic2c_dextra[j]);
				k=(k&1)?(-((k+1)>>1)):(k>>1);
				arg[narg++]=k;
				i=BTIC2CH_DecodeSymbol(ctx, tab);
			}
			
			switch(cmd)
			{
			case 1:
				if(narg>0)
					{ ctx->skip_cnt=arg[0]; }
				else
					{ ctx->skip_cnt=1; }
				break;
			case 2:
				if(narg>0)
					{ ctx->blk_mode[cn]=arg[0]; }
				else
					{ ctx->blk_mode[cn]=0; }
				break;
			case 3:
				ctx->blk_move[2*cn+0]=arg[0];
				ctx->blk_move[2*cn+1]=arg[1];
//				ctx->blk_move[2*cn+0]=-arg[0];
//				ctx->blk_move[2*cn+1]=-arg[1];
				break;
			case 4:
				ctx->blk_move[2*cn+0]+=arg[0];
				ctx->blk_move[2*cn+1]+=arg[1];
//				ctx->blk_move[2*cn+0]-=arg[0];
//				ctx->blk_move[2*cn+1]-=arg[1];
				break;

			case 5:
				for(j=0; j<ctx->img_nc; j++)
				{
					ctx->blk_move[2*j+0]=
						(arg[0]*ctx->img_ch[j])/ctx->img_chm;
					ctx->blk_move[2*j+1]=
						(arg[1]*ctx->img_cv[j])/ctx->img_chn;
				}
				break;
			case 6:
				for(j=0; j<ctx->img_nc; j++)
				{
					ctx->blk_move[2*j+0]+=
						(arg[0]*ctx->img_ch[j])/ctx->img_chm;
					ctx->blk_move[2*j+1]+=
						(arg[1]*ctx->img_cv[j])/ctx->img_chn;
				}
				break;

			default:
				break;
			}
			
			continue;
		}

		i=BTIC2CH_DecodeSymbol(ctx, tab);
	}

	l=(i>>6)&1;
	j=i&63;
	k=btic2c_dbase[j]+BTIC2CH_ReadNBitsL(ctx, btic2c_dextra[j]);
//	k=(k&1)?(-((k+1)>>1)):(k>>1);
	k=(k>>1)^((k<<31)>>31);
	*rfl=l;
	return(k);
}

int BTIC2CH_DecodeBlock(BTIC2C_Context *ctx,
	s32 *buf, int dctab, int actab, int dc2tab, int ac2tab,
	int cn, int ni, int nn)
{
	int c, n;
	int i, j, k, l;

	memset(buf+1, 0, 63*sizeof(s32));

	buf[0]+=BTIC2CH_DecodeDiffDC(ctx, dctab, cn, &l);
	if(ctx->skip_cnt)
		return(1);
	//Implicit EOB
	if(l&1)
		return(2);
	
	for(i=1; i<64; i++)
	{
		j=BTIC2CH_DecodeSymbol(ctx, actab);
		if(j<=0)
		{
			if(j<0)
			{
				printf("bad dct block1 %02X %d/%d\n", j, ni, nn);
				return(-1);
			}
			break;
		}

//		i+=j>>5;	//preceding 0's
//		if(!(j&31))continue;

		k=j>>5;
		if(k==7)
		{
			k=j&31;
			k=btic2c_lbase[k]+BTIC2CH_ReadNBits(ctx, btic2c_lextra[k]);
			i+=k;
			if(i>=64)
			{
				printf("BTIC2CH_DecodeBlock: "
					"Bad ZCE=%d %02X %d/%d\n", k, j, ni, nn);
				return(-1);
			}

			j=BTIC2CH_DecodeSymbol(ctx, ac2tab);
			
//			if((j&0x70)==0x60)
			if((j&0xE0)==0xC0)
			{
				c=((j>>3)&3)+1;
				if(c==1)
				{
					n=(j&7)+3;
					for(j=0; j<n; j++)
					{
						k=BTIC2CH_ReadBit(ctx);
//						l=(k&1)?(-1):1;
						l=((k<<31)>>30)+1;
						k=pdjpg_zigzag2[i+j];
						buf[k]=l;
					}
					i+=n-1;
					continue;
				}else if(c<4)
				{
					n=(j&7)+3;
					for(j=0; j<n; j++)
					{
						k=BTIC2CH_ReadNBits(ctx, c);
//						l=(k&1)?(-((k+1)>>1)):(k>>1);
						l=(k>>1)^((k<<31)>>31);
						k=pdjpg_zigzag2[i+j];
						buf[k]=l;
					}
					i+=n-1;
					continue;
				}else
				{
					k=j&7;
					switch(k)
					{
					case 0:
						c=BTIC2CH_ReadNBits(ctx, 2)+1;
						n=BTIC2CH_ReadNBits(ctx, 4)+3;
						break;
					case 1:
						c=BTIC2CH_ReadNBits(ctx, 2)+1;
						n=BTIC2CH_ReadNBits(ctx, 5)+3;
						break;
					case 2:
						c=BTIC2CH_ReadNBits(ctx, 2)+1;
						n=BTIC2CH_ReadNBits(ctx, 6)+3;
						break;
					case 3:
						c=BTIC2CH_ReadNBits(ctx, 3)+1;
						n=BTIC2CH_ReadNBits(ctx, 4)+3;
						break;
					case 4:
						c=BTIC2CH_ReadNBits(ctx, 3)+1;
						n=BTIC2CH_ReadNBits(ctx, 5)+3;
						break;
					case 5:
						c=BTIC2CH_ReadNBits(ctx, 3)+1;
						n=BTIC2CH_ReadNBits(ctx, 6)+3;
						break;
					case 6:
						c=BTIC2CH_ReadNBits(ctx, 4)+1;
						n=BTIC2CH_ReadNBits(ctx, 6)+3;
						break;
					default:
						break;
					}
					for(j=0; j<n; j++)
					{
						k=BTIC2CH_ReadNBits(ctx, c);
//						l=(k&1)?(-((k+1)>>1)):(k>>1);
						l=(k>>1)^((k<<31)>>31);
						k=pdjpg_zigzag2[i+j];
						buf[k]=l;
					}
					i+=n-1;
					continue;
				}
			}
			
			if(j>=64)
			{
				//escape command...
				//currently unsupported here...
				printf("Block Unsupported Escape %d/%d\n", ni, nn);
				return(-1);
			}
			
			j&=63;
			if(!j)continue;

			j=btic2c_dbase[j]+BTIC2CH_ReadNBitsL(ctx, btic2c_dextra[j]);
//			k=(j&1)?(-((j+1)>>1)):(j>>1);
			k=(j>>1)^((j<<31)>>31);
		}else
		{
			i+=k;
			if(i>=64)
			{
				printf("BTIC2CH_DecodeBlock: "
					"Bad ZC=%d %02X %d/%d\n", k, j, ni, nn);
				return(-1);
			}

			j&=31;
			if(!j)continue;

			j=btic2c_dbase[j]+BTIC2CH_ReadNBits(ctx, btic2c_dextra[j]);
//			k=(j&1)?(-((j+1)>>1)):(j>>1);
			k=(j>>1)^((j<<31)>>31);
		}

		j=pdjpg_zigzag2[i];
		buf[j]=k;
	}

	if(i>64)
	{
		printf("bad dct block1 %02X %d/%d\n", j, ni, nn);
		return(-1);
	}

	if(i==1)return(2);

	return(0);
}

void BTIC2CH_DequantBlock(
	BTIC2C_Context *ctx, s32 *ibuf, s32 *obuf, int qid)
{
	u16 *qt;
	int i, j;

	qt=ctx->img_qt[qid];
	for(i=0; i<64; i+=8)
	{
		obuf[i+0]=ibuf[i+0]*qt[i+0];
		obuf[i+1]=ibuf[i+1]*qt[i+1];
		obuf[i+2]=ibuf[i+2]*qt[i+2];
		obuf[i+3]=ibuf[i+3]*qt[i+3];
		obuf[i+4]=ibuf[i+4]*qt[i+4];
		obuf[i+5]=ibuf[i+5]*qt[i+5];
		obuf[i+6]=ibuf[i+6]*qt[i+6];
		obuf[i+7]=ibuf[i+7]*qt[i+7];
	}
}

void BTIC2CH_DequantBlock_Flat(
	BTIC2C_Context *ctx, s32 *ibuf, s32 *obuf, int qid)
{
	int i, j;

	obuf[0]=ibuf[0]*ctx->img_qt[qid][0];
}

int BTIC2C_MarkerImgData(BTIC2C_Context *ctx, byte *buf, int len,
	byte *obuf, int clrs)
{
	return(BTIC2C_MarkerImgData2(ctx, buf, len, obuf, clrs, -1));
}

int BTIC2C_MarkerImgData2(BTIC2C_Context *ctx, byte *buf, int len,
	byte *obuf, int clrs, int aribits)
{
	s32 dbuf[4*64];
	s32 ebuf[4*64];
	byte ch[4], cv[4], qid[4];
	byte step[4];
	int xi[4], yi[4], wi[4], hi[4];
	byte cdt[4], cat[4];
	byte cd2t[4], ca2t[4];
	int ccnt[4];
	s32 *scb, *sib, *sdb;
	s16 *sjb;

	int bw, bh, mbxs, mbys;
	int i, j, k, l, i1, j1, k1, l1;
	int w, h, n, ns, rt;

#if 1
	ns=ctx->img_ns; rt=0;
	for(i=0; i<ns; i++)
	{
//		printf("%d %d %d\n", s[0], s[1]>>4, s[1]&15);

		cdt[i]=ctx->img_cdt[i];
		cat[i]=ctx->img_cat[i];
		cd2t[i]=ctx->img_cd2t[i];
		ca2t[i]=ctx->img_ca2t[i];

		for(j=0; j<ctx->img_nc; j++)
			if(ctx->img_cid[j]==ctx->img_scid[i])
		{
			ch[i]=ctx->img_ch[j];
			cv[i]=ctx->img_cv[j];
			qid[i]=ctx->img_qid[j];
			xi[i]=ctx->img_cxi[j];
			yi[i]=ctx->img_cyi[j];

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
//		ctx->img_scid[i]=s[0];
//		ctx->img_scn[i]=j;
//		if(!ctx->img_scbuf[i])
//			{ ctx->img_scbuf[i]=malloc(n*64*sizeof(s32)); }
//		if(!ctx->img_sibuf[i])
//			{ ctx->img_sibuf[i]=malloc(n*64*sizeof(s32)); }

		if(ctx->img_pbits<16)
		{
			if(!ctx->img_sjbuf[i])
			{
				ctx->img_sjbuf[i]=malloc(n*64*sizeof(s16));
				ctx->img_lsjbuf[i]=malloc(n*64*sizeof(s16));
			}
		}else
		{
			if(!ctx->img_sibuf[i])
			{
				ctx->img_sibuf[i]=malloc(n*64*sizeof(s32));
				ctx->img_lsibuf[i]=malloc(n*64*sizeof(s32));
			}
		}

		ccnt[i]=0;
	}
#endif

#if 1
	bw=(ctx->xs+ctx->img_chm*8-1)/(ctx->img_chm*8);
	bh=(ctx->ys+ctx->img_chn*8-1)/(ctx->img_chn*8);
	n=bw*bh;

	mbxs=ctx->img_chm*8;	mbys=ctx->img_chn*8;
	ctx->mbxs=mbxs;			ctx->mbys=mbys;

	if(aribits>=0)
	{
		BTIC2CH_InitStreamArith(ctx, buf, len, aribits);
	}else
	{
		BTIC2CH_InitStream(ctx, buf);
	}

	for(i=0; i<(4*64); i++)dbuf[i]=0;

	if(!ctx->skip_map)
		ctx->skip_map=malloc((n+7)/8);
	memset(ctx->skip_map, 0, (n+7)/8);

	for(i=0; i<8; i++)
	{
		ctx->blk_mode[i]=0;
		ctx->blk_move[i*2+0]=0;
		ctx->blk_move[i*2+1]=0;
	}
//	ctx->blk_move[0]=0;
//	ctx->blk_move[1]=0;

	rt=0;
	for(i=0; i<n; i++)
	{
		if(!ctx->skip_cnt)
		{
			for(j=0; j<ns; j++)
			{
				if(ctx->skip_cnt)break;
				sdb=dbuf+j*64;
				
				for(k=0; k<cv[j]; k++)
					for(l=0; l<ch[j]; l++)
				{
					if(rt<0)break;
					if(ctx->skip_cnt)break;

					i1=(i/(wi[j]/ch[j]))*ch[j];
					j1=(i%(wi[j]/ch[j]))*ch[j];
					k1=((i1+k)*wi[j])+(j1+l);
					
					scb=ebuf+j*64;
//					scb=ctx->img_scbuf[j]+k1*64;
					sib=ctx->img_sibuf[j]+k1*64;
					sjb=ctx->img_sjbuf[j]+k1*64;

					rt=BTIC2CH_DecodeBlock(ctx, sdb,
						cdt[j], cat[j], cd2t[j], ca2t[j], j, i, n);
					if(rt)
					{
						if(rt<0)break;
						if(rt==1)
							{ /* Skip*/ break; }
						if(rt==2)
						{
							/* Flat Colored Block */
							if(ctx->blk_mode[j]==BTIC2C_BLKM_SKIP)
							{
								//Debug
//								scb[0]=255*64;
//								BTIC2C_TransIBLKS_Flat(
//									scb, sjb, ctx->img_blktrans);
								continue;
							}else if(ctx->blk_mode[j]==BTIC2C_BLKM_REPLACE)
							{
								BTIC2CH_DequantBlock_Flat(ctx, sdb,
									scb, qid[j]);
								if(sjb)
								{
									BTIC2C_TransIBLKS_Flat(
										scb, sjb, ctx->img_blktrans);
								}else
								{
									BTIC2C_TransIBLKT_Flat(
										scb, sib, ctx->img_blktrans);
								}
								continue;
							}
						}
					}
					BTIC2CH_DequantBlock(ctx, sdb, scb, qid[j]);
					if(ctx->blk_mode[j]==BTIC2C_BLKM_REPLACE)
					{
						if(sjb)
						{
							BTIC2C_TransIBLKS(
								scb, sjb,
								ctx->img_blktrans);
						}else
						{
							BTIC2C_TransIBLKT(
								scb, sib,
								ctx->img_blktrans);
						}
					}else if(ctx->blk_mode[j]==BTIC2C_BLKM_ADD)
					{
						if(sjb)
						{
							BTIC2C_TransIBLKS_Add(
								scb, sjb,
								ctx->img_blktrans);
						}else
						{
							BTIC2C_TransIBLKT_Add(
								scb, sib,
								ctx->img_blktrans);
						}
					}else if(ctx->blk_mode[j]==BTIC2C_BLKM_MOVEADD)
					{
						if(sjb)
						{
//							BTIC2C_TransIBLKS_Add(
//								scb, sjb,
//								ctx->img_blktrans);
							BTIC2C_TransIBLKS_MoveAdd(
								scb, sjb,
								ctx->img_lsjbuf[j],
								(j1+l)*8+ctx->blk_move[j*2+0],
								(i1+k)*8+ctx->blk_move[j*2+1],
								xi[j], yi[j],
								ctx->img_blktrans);
						}else
						{
							BTIC2C_TransIBLKT_MoveAdd(
								scb, sib,
								ctx->img_lsibuf[j],
								(j1+l)*8+ctx->blk_move[j*2+0],
								(i1+k)*8+ctx->blk_move[j*2+1],
								xi[j], yi[j],
								ctx->img_blktrans);
						}
					}else if(ctx->blk_mode[j]==BTIC2C_BLKM_MOVESKIP)
					{
						if(sjb)
						{
							BTIC2C_TransIBLKS_Move(
								sjb,
								ctx->img_lsjbuf[j],
								(j1+l)*8+ctx->blk_move[j*2+0],
								(i1+k)*8+ctx->blk_move[j*2+1],
								xi[j], yi[j],
								ctx->img_blktrans);
						}else
						{
							BTIC2C_TransIBLKT_Move(
								sib,
								ctx->img_lsibuf[j],
								(j1+l)*8+ctx->blk_move[j*2+0],
								(i1+k)*8+ctx->blk_move[j*2+1],
								xi[j], yi[j],
								ctx->img_blktrans);
						}
					}else if(ctx->blk_mode[j]==BTIC2C_BLKM_SKIP)
					{
					}
				}
			}

			if(!ctx->skip_cnt)
			{
				j=i%bw; k=i/bw;
				BTIC2C_DecodeColorTransformBlock(ctx,
					obuf, j*mbxs, k*mbys, clrs);
			}else
			{
//				j=i%bw; k=i/bw;
//				BTIC2C_DecodeColorTransform_DebugBlock(ctx,
//					obuf, j*16, k*16, clrs);
			}
		}

		if(ctx->skip_cnt)
		{
//			j=i%bw; k=i/bw;
//			BTIC2C_DecodeColorTransform_DebugBlock(ctx,
//				obuf, j*16, k*16, clrs);

			ctx->skip_map[i>>3]|=1<<(i&7);
			ctx->skip_cnt--;
			continue;
		}
	}

	for(i=0; i<ns; i++)
	{
		n=wi[i]*hi[i];

		if(ctx->img_sjbuf[i])
		{
			memcpy(ctx->img_lsjbuf[i], ctx->img_sjbuf[i],
				n*64*sizeof(s16));
		}
		if(ctx->img_sibuf[i])
		{
			memcpy(ctx->img_lsibuf[i], ctx->img_sibuf[i],
				n*64*sizeof(s32));
		}
	}

	if(rt<0)
	{
		printf("BTIC2C_MarkerImgData: Bad Image\n");
		return(-1);
	}
#endif

	return(0);
}

int BTIC2C_MarkerSOS(BTIC2C_Context *ctx, byte *buf, int len)
{
	byte ch[4], cv[4], qid[4];
	byte step[4];
	int xi[4], yi[4], wi[4], hi[4];
	int ccnt[4];

	byte *s, *se;
	int i, j, k, l, i1, j1, k1, l1;
	int w, h, c, n, ns, rt;

	ns=buf[0]; rt=0;
	ctx->img_ns=ns;
	s=buf+1; se=buf+len;
	for(i=0; i<ns; i++)
	{
//		printf("%d %d %d\n", s[0], s[1]>>4, s[1]&15);

		c=*s++;
		j=*s++;
		ctx->img_cdt[i]=j>>4;
		ctx->img_cat[i]=j&15;
		ctx->img_cd2t[i]=j>>4;
		ctx->img_ca2t[i]=j&15;

#if 1
		if(c&16)
		{
			j=*s++;
			ctx->img_cd2t[i]=j>>4;
			ctx->img_ca2t[i]=j&15;
		}
#endif

//		ctx->img_cdt[i]=s[1]>>4;
//		ctx->img_cat[i]=s[1]&15;

		for(j=0; j<ctx->img_nc; j++)
			if(ctx->img_cid[j]==(c&15))
		{
			ch[i]=ctx->img_ch[j];
			cv[i]=ctx->img_cv[j];
			qid[i]=ctx->img_qid[j];
			xi[i]=ctx->img_cxi[j];
			yi[i]=ctx->img_cyi[j];

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
		ctx->img_scid[i]=c&15;
		ctx->img_scn[i]=j;
//		ctx->img_scbuf[i]=malloc(n*64*sizeof(short));
//		ctx->img_sibuf[i]=malloc(n*64);

//		memset(ctx->img_sibuf[i], 0, n*64);

		ccnt[i]=0;

//		s+=2;
	}
	s+=3;

	while(s<se)
	{
		if(!(*s))
			{ break; }
		switch(*s++)
		{
		default:
			break;
		}
	}

#if 1
	for(i=0; i<ctx->img_nc; i++)
	{
		j=ctx->img_scn[i];
//		ctx->img_schsc[i]=ctx->img_ch[j]/(ctx->img_chm*1.0);
//		ctx->img_scvsc[i]=ctx->img_cv[j]/(ctx->img_chn*1.0);

		ctx->img_schsci[i]=(65536*ctx->img_ch[j])/ctx->img_chm;
		ctx->img_scvsci[i]=(65536*ctx->img_cv[j])/ctx->img_chn;

		ctx->img_schs[i]=(ctx->img_cxi[j]+7)/8;
		ctx->img_scvs[i]=(ctx->img_cyi[j]+7)/8;
		
	}

	if(ctx->img_nc==4)
	{
		ctx->img_alpha=1;
		
		i=1;
		if(ctx->img_schsci[0]!=65536)i=0;
		if(ctx->img_scvsci[0]!=65536)i=0;
		if(ctx->img_schsci[1]!=32768)i=0;
		if(ctx->img_scvsci[1]!=32768)i=0;
		if(ctx->img_schsci[2]!=32768)i=0;
		if(ctx->img_scvsci[2]!=32768)i=0;
		if(ctx->img_schsci[3]!=32768)i=0;
		if(ctx->img_scvsci[3]!=32768)i=0;
		ctx->img_is420=i;

		i=1;
		if(ctx->img_schsci[0]!=65536)i=0;
		if(ctx->img_scvsci[0]!=65536)i=0;
		if(ctx->img_schsci[1]!=65536)i=0;
		if(ctx->img_scvsci[1]!=65536)i=0;
		if(ctx->img_schsci[2]!=65536)i=0;
		if(ctx->img_scvsci[2]!=65536)i=0;
		if(ctx->img_schsci[3]!=65536)i=0;
		if(ctx->img_scvsci[3]!=65536)i=0;
		ctx->img_is444=i;
	}else if(ctx->img_nc==3)
	{
		i=1;
		if(ctx->img_schsci[0]!=65536)i=0;
		if(ctx->img_scvsci[0]!=65536)i=0;
		if(ctx->img_schsci[1]!=32768)i=0;
		if(ctx->img_scvsci[1]!=32768)i=0;
		if(ctx->img_schsci[2]!=32768)i=0;
		if(ctx->img_scvsci[2]!=32768)i=0;
		ctx->img_is420=i;

		i=1;
		if(ctx->img_schsci[0]!=65536)i=0;
		if(ctx->img_scvsci[0]!=65536)i=0;
		if(ctx->img_schsci[1]!=65536)i=0;
		if(ctx->img_scvsci[1]!=65536)i=0;
		if(ctx->img_schsci[2]!=65536)i=0;
		if(ctx->img_scvsci[2]!=65536)i=0;
		ctx->img_is444=i;
	}else
	{
		ctx->img_is420=0;
		ctx->img_is444=0;
	}
#endif

	return(0);
}

int BTIC2C_MarkerSOF0(BTIC2C_Context *ctx, byte *buf, int len)
{
	byte *cnt, *s, *se;
	int i, j, k, l, m, n;

	ctx->img_pbits=buf[0];
	ctx->img_pclrs=buf[1];
	
	i=(buf[2]<<8)|buf[3];
	j=(buf[4]<<8)|buf[5];

	ctx->xs=j;
	ctx->ys=i;
//	printf("size %dx%d\n", j, i);

	m=0; n=0;

	k=buf[6];
	s=buf+7; se=buf+len;
	for(i=0; i<k; i++)
	{
		ctx->img_cid[i]=s[0];
		ctx->img_ch[i]=s[1]>>4;
		ctx->img_cv[i]=s[1]&15;
		ctx->img_qid[i]=s[2];

		if(ctx->img_ch[i]>m)m=ctx->img_ch[i];
		if(ctx->img_cv[i]>n)n=ctx->img_cv[i];

//		printf("CID %d: %d %d %d\n", ctx->img_cid[i],
//			ctx->img_ch[i], ctx->img_cv[i], ctx->img_qid[i]);
		s+=3;
	}

	ctx->img_clrtrans=BTIC2C_CLRS_YCGCO;
	ctx->img_blktrans=BTIC2C_BLKT_WHT;

	while(s<se)
	{
		if(!(*s))
			{ s+=2; break; }
		switch(*s++)
		{
		case 0x01:
			l=*s++;
			ctx->img_clrtrans=s[0];
			ctx->img_blktrans=s[1];
			s+=l;
			break;
		default:
			l=*s++; s+=l;
			break;
		}
	}

	ctx->img_chm=m;
	ctx->img_chn=n;

	for(i=0; i<k; i++)
	{
		ctx->img_cxi[i]=(ctx->img_ch[i]*ctx->xs)/m;
		ctx->img_cyi[i]=(ctx->img_cv[i]*ctx->ys)/n;
	}

	ctx->img_nc=k;

	return(0);
}

int BTIC2C_DecodeQuantTable(BTIC2C_Context *ctx, int tab, int *rk)
{
	int i, j, k, l, n;

	l=0;
	for(i=0; i<64; i++)
	{
		j=BTIC2CH_ReadAdSRice(ctx, rk);
		if(j<0)
		{
			k=i+(-j);
			for(j=i; (j<k) && (j<64); j++)
			{
				ctx->img_qt[tab][pdjpg_zigzag2[j]]=l;
			}
			i=k-1;
			continue;
		}
		
//		l=j;
		k=btic2c_lbase[j]+BTIC2CH_ReadNBits(ctx, btic2c_lextra[j]);
		k=(k&1)?(-((k+1)>>1)):(k>>1);
		l=l+k;

		ctx->img_qt[tab][pdjpg_zigzag2[i]]=l;
	}
	return(0);
}

int BTIC2C_MarkerDQT(BTIC2C_Context *ctx, byte *buf, int tn, int len)
{
	byte *s, *se;
	int i, j, k, l;

	s=buf; se=buf+len;
	while(s<se)
	{
		i=(s[0]>>4)&15;
		
		if(i==1)
		{
			k=4;
			BTIC2CH_InitStream(ctx, s+1);
			BTIC2C_DecodeQuantTable(ctx, tn, &k);
			s=ctx->huff_cs;
			continue;
		}
		
		if(i)
		{
//			printf("bad bits %d\n", i);
			return(-1);
		}
		i=s[0]&15;
//		printf("dest %d\n", i);

		for(j=0; j<64; j++)
		{
			ctx->img_qt[tn][pdjpg_zigzag2[j]]=s[j+1];
//			ctx->img_qt[i][pdjpg_zigzag2[j]]=s[j+1];
		}
		s+=65;
	}
	
	return(0);
}

#if 1
static int btic2c_lorder[]={
	16, 17, 18, 0, 8,7, 9,6, 10,5, 11,4, 12,3, 13,2, 14,1, 15, 19};

int BTIC2C_DecodeHufftabSymbol(BTIC2C_Context *ctx, int *rk)
{
	int i;
	i=BTIC2CH_ReadAdRice(ctx, rk);
//	i=BTIC2CH_ReadRice(ctx, 2);
	return(btic2c_lorder[i]);
}

int BTIC2C_DecodeCodeLengths(BTIC2C_Context *ctx,
	byte *cl, int ncl, int *rk)
{
	int i, j, k;

	for(i=0; i<ncl; i++)cl[i]=0;

	i=0; k=0;
	while(i<ncl)
	{
		j=BTIC2C_DecodeHufftabSymbol(ctx, rk);
		if(j<0)return(j);

		if(j<17)
			{ k=j; cl[i++]=j; continue; }

		if(j==17)
		{
			j=BTIC2CH_Read3Bits(ctx)+3;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==18)
		{
			j=BTIC2CH_ReadNBits(ctx, 7)+11;
			while(j--)cl[i++]=k;
			continue;
		}
		if(j==19)break;

		return(-7);
	}
	return(0);
}
#endif

int BTIC2C_MarkerDHT(BTIC2C_Context *ctx, byte *buf, int tn, int len)
{
	byte *cnt, *s;
	int i, j, k, l;

	for(i=0; i<256; i++)
	{
		ctx->huff_len[tn*256+i]=0;
		ctx->huff_next[tn*256+i]=-1;
		ctx->huff_idx[tn*256+i]=-1;
	}

//	for(i=0; i<4096; i++)
//		{ ctx->huff_idx12[tn*4096+i]=-1; }

	s=buf;
	
	i=*s++;
	
//	if(i==0x10)
	if((i&0xF0)==0x10)
	{
//		k=4;
		if(i&15)
			{ k=-(i&15); }
		else
			{ k=4; }

		BTIC2CH_InitStream(ctx, s);
		BTIC2C_DecodeCodeLengths(ctx, ctx->huff_len+tn*256, 256, &k);

		k=0; l=0;
		for(i=1; i<=16; i++)
		{
			k<<=1;
			for(j=0; j<256; j++)
			{
				if(ctx->huff_len[tn*256+j]==i)
				{
					ctx->huff_code[tn*256+j]=k++;
//					ctx->huff_len[tn*256+j]=i;
					ctx->huff_mask[tn*256+j]=(1<<i)-1;
					l=i;
				}
			}
		}
	}else if(!i)
	{
		cnt=s; s+=16;
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
//		printf("%04X %d\n", k, l);
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

	return(0);
}

int BTIC2C_DecodeCtx(BTIC2C_Context *ctx,
	byte *ibuf, byte *obuf, int isz, int osz,
	int *rxs, int *rys, int clrs)
{
	byte *cs, *cse;
	int i, j, k, l;

	cs=ibuf; cse=ibuf+isz;
	while(cs<cse)
	{
		if(*cs==0xE0)
			break;

		if(*cs==0xE1)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			BTIC2C_MarkerImgData(ctx, cs+4, l-4, obuf, clrs);
			cs+=l; continue;
		}	

		if(*cs==0xE3)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);

			if((cs[4]=='A') && (cs[5]>='0') && (cs[5]<='9'))
			{
				BTIC2C_MarkerImgData2(ctx,
					cs+6, l-6, obuf, clrs, cs[5]-'0');
				cs+=l; continue;
			}

			if((cs[4]=='A') && (cs[5]>='a') && (cs[5]<='f'))
			{
				BTIC2C_MarkerImgData2(ctx,
					cs+6, l-6, obuf, clrs, (cs[5]-'a')+10);
				cs+=l; continue;
			}
			
			if((cs[4]=='Q') && (cs[5]>='0') && (cs[5]<='9'))
			{	BTIC2C_MarkerDQT(ctx, cs+6, cs[5]-'0', l-6);
				cs+=l; continue; }
			if((cs[4]=='H') && (cs[5]>='0') && (cs[5]<='9'))
			{	BTIC2C_MarkerDHT(ctx, cs+6, cs[5]-'0', l-6);
				cs+=l; continue; }
			if((cs[4]=='S') && (cs[5]=='F'))
			{	BTIC2C_MarkerSOF0(ctx, cs+6, l-6);
				cs+=l; continue; }
			if((cs[4]=='S') && (cs[5]=='S'))
			{	BTIC2C_MarkerSOS(ctx, cs+6, l-6);
				cs+=l; continue; }
			cs+=l; continue;
		}

		if(*cs==0xE4)
		{
			l=(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			cs+=l; continue;
		}

		if(*cs==0xE5)
		{
			l=cs[1];
			if((cs[2]=='Q') && (cs[3]>='0') && (cs[3]<='9'))
			{	BTIC2C_MarkerDQT(ctx, cs+4, cs[3]-'0', l-4);
				cs+=l; continue; }
			if((cs[2]=='H') && (cs[3]>='0') && (cs[3]<='9'))
			{	BTIC2C_MarkerDHT(ctx, cs+4, cs[3]-'0', l-4);
				cs+=l; continue; }
			if((cs[2]=='S') && (cs[3]=='F'))
			{	BTIC2C_MarkerSOF0(ctx, cs+4, l-4);
				cs+=l; continue; }
			if((cs[2]=='S') && (cs[3]=='S'))
			{	BTIC2C_MarkerSOS(ctx, cs+4, l-4);
				cs+=l; continue; }
			cs+=l; continue;
		}
	}

	if(!obuf)
	{
		*rxs=ctx->xs;
		*rys=ctx->ys;
		return(0);
	}

//	obuf=ctx->img_imgbuf;
	if(!obuf || (ctx->xs!=ctx->lxs) || (ctx->ys!=ctx->lys))
	{
		ctx->lxs=ctx->xs;
		ctx->lys=ctx->ys;
	}

	*rxs=ctx->xs;
	*rys=ctx->ys;
	return(0);
}
