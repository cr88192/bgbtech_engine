#include <bgbbtj.h>

extern byte rpza_blkmap1[256];
extern byte rpza_blkmap2[256];
extern byte rpza_blkmap3[256];
extern byte rpza_blkmap4[256];

extern byte rpza_blkalphap[256];
extern byte rpza_blkalphamape1[4096];
extern byte rpza_blkalphamape2[4096];
extern u16 rpza_difftab[8*256];		//differential color tables

BGBBTJ_API int BGBBTJ_RPZA_EncodeBlocksDXT1(
	byte *obuf, int osz, byte *blks, byte *lblks, int xs, int ys)
{
	return(BGBBTJ_BT1C_EncodeBlocksDXT1(obuf, osz,
		blks, lblks, xs, ys, 0));

//	return(BGBBTJ_BT1C_EncodeBlocksDXT1(obuf, osz,
//		blks, lblks, xs, ys, 1));
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeBlocksDXT5(
	byte *obuf, int osz, byte *blks, byte *lblks, int xs, int ys)
{
	return(BGBBTJ_BT1C_EncodeBlocksDXT5(obuf, osz,
		blks, lblks, xs, ys, 0));

//	return(BGBBTJ_BT1C_EncodeBlocksDXT5(obuf, osz,
//		blks, lblks, xs, ys, 1));
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
	byte *csc, byte *css, byte *cse,
	int *rlen, int *rdist, int fl)
{
	byte *cs, *cs1;
	int bi, bl, ml;
	int i, j, k;

	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKLZ))
		{ *rlen=0; *rdist=0; return(0); }
		
	bi=0; bl=0; ml=256;
	cs=csc-8;
	while(cs>css)
	{
		i=(csc-cs)/8;
		if(i>=65536) { break; }
		else if(i>=4096) { ml=1; }
		else if(i>=1024) { ml=8; }
		else if(i>=256) { ml=32; }
		else if(i>=64) { ml=64; }
		else if(i>=16) { ml=128; }

		for(j=0; j<256; j++)
			if(memcmp(cs+j*8, csc+j*8, 8))
				break;
		if(j>bl) { bi=i; bl=j; }
		if(bl>=ml)break;
		cs-=8;
	}
	
	if(bl<16)
	{
//		if(bi>256)
//			return(0);
	}
	
	*rlen=bl;
	*rdist=bi;
	return(bl>0);
}

int BGBBTJ_BT1C_EncodeValueForDiffMode(int b, int d, int sc)
{
	static int sclt[8][4]=
	{
		{1,  2,  2, 0},
		{1,  3,  4, 0},
		{2,  4,  6, 0},
		{2,  6,  8, 0},
		{4,  8, 13, 0},
		{5, 13, 17, 0},
		{7, 17, 23, 0},
		{9, 23, 29, 0},
	};
	int i;

	i=2;
	if((d<0) && ((b-sclt[sc][0])>=0))
	{
		i=1;
		if((d<=(-sclt[sc][1]) && ((b-sclt[sc][2])>0)))i=0;
	}else if((d>0) && ((b+sclt[sc][0])<=31))
	{
		i=3;
		if((d>=(sclt[sc][1])) && ((b+sclt[sc][2])<31))i=4;
	}

	return(i);

#if 0
	int i;
	switch(sc)
	{
	case 0:
		i=2;
		if((d<0) && ((b-1)>=0))
		{
			i=1;
			if((d<=(-3) && ((b-4)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+1)<31))
		{
			i=3;
			if((d>=( 3)) && ((b+4)<31))i=4;
			break;
		}
		break;
	case 1:
		i=2;
		if((d<0) && ((b-2)>=0))
		{
			i=1;
			if((d<=(-6) && ((b-8)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+2)<31))
		{
			i=3;
			if((d>=( 6)) && ((b+8)<31))i=4;
			break;
		}
		break;
	case 2:
		i=2;
		if((d<0) && ((b-5)>=0))
		{
			i=1;
			if((d<=(-13) && ((b-17)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+5)<31))
		{
			i=3;
			if((d>=( 13)) && ((b+17)<31))i=4;
			break;
		}
		break;
	case 3:
		i=2;
		if((d<0) && ((b-9)>=0))
		{
			i=1;
			if((d<=(-23) && ((b-29)>=0)))i=0;
			break;
		}
		if((d>0) && ((b+9)<31))
		{
			i=3;
			if((d>=( 23)) && ((b+29)<31))i=4;
			break;
		}
		break;
	default:
		i=2;
		break;
	}
	return(i);
#endif
}

int BGBBTJ_BT1C_EncodePredictDiffColor(
	int p, int v, int sc)
{
	int br, bg, bb;
	int cr, cg, cb;
	int dr, dg, db;
	int d2r, d2g, d2b;
	int i, j, k;

	br=(p>>11)&0x1F;
	bg=(p>> 6)&0x1F;
	bb=(p    )&0x1F;
	cr=(v>>11)&0x1F;
	cg=(v>> 6)&0x1F;
	cb=(v    )&0x1F;
	dr=cr-br;
	dg=cg-bg;
	db=cb-bb;
	
	d2r=BGBBTJ_BT1C_EncodeValueForDiffMode(br, dr, sc);
	d2g=BGBBTJ_BT1C_EncodeValueForDiffMode(bg, dg, sc);
	d2b=BGBBTJ_BT1C_EncodeValueForDiffMode(bb, db, sc);
	
	i=sc*256+((5*d2r+d2g)*5+d2b);
	j=p+rpza_difftab[i];
	
	return(j);
}

int BGBBTJ_BT1C_EncodePredictDiff(
	int p, int v, int sc)
{
	int br, bg, bb;
	int cr, cg, cb;
	int dr, dg, db;
	int d2r, d2g, d2b;
	int i, j, k;

	br=(p>>11)&0x1F;
	bg=(p>> 6)&0x1F;
	bb=(p    )&0x1F;
	cr=(v>>11)&0x1F;
	cg=(v>> 6)&0x1F;
	cb=(v    )&0x1F;
	dr=cr-br;
	dg=cg-bg;
	db=cb-bb;
	
	d2r=BGBBTJ_BT1C_EncodeValueForDiffMode(br, dr, sc);
	d2g=BGBBTJ_BT1C_EncodeValueForDiffMode(bg, dg, sc);
	d2b=BGBBTJ_BT1C_EncodeValueForDiffMode(bb, db, sc);
	
	i=((5*d2r+d2g)*5+d2b);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffColorA(
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BTIC1C_PredictDiffColorModeA(
		blks, cs, xs1, ys1, stride, 3);
	v=cs[0]|(cs[1]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiffColor(p, v, sc);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffColorB(
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BTIC1C_PredictDiffColorModeB(
		blks, cs, xs1, ys1, stride, 3);
	v=cs[2]|(cs[3]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiffColor(p, v, sc);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffA(
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BTIC1C_PredictDiffColorModeA(
		blks, cs, xs1, ys1, stride, 3);
	v=cs[0]|(cs[1]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiff(p, v, sc);
	return(i);
}

int BGBBTJ_BT1C_EncodePredictDiffB(
	byte *blks, byte *cs,
	int xs1, int ys1, int fl, int stride, int sc)
{
	int p, v;
	int i, j, k;

	p=BGBBTJ_BTIC1C_PredictDiffColorModeB(
		blks, cs, xs1, ys1, stride, 3);
	v=cs[2]|(cs[3]<<8);
	i=BGBBTJ_BT1C_EncodePredictDiff(p, v, sc);
	return(i);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT1(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	return(BGBBTJ_BT1C_EncodeBlocksDXTn(
		obuf, osz, blks, lblks, xs, ys, fl, 8));
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT5(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	int i, j;
	
	i=BGBBTJ_BT1C_EncodeBlocksDXTn(
		obuf, osz, blks+8, lblks?(lblks+8):NULL, xs, ys, fl, 16);
	j=BGBBTJ_BT1C_EncodeBlocksBC4(
		obuf+i, osz-i, blks, lblks, xs, ys,
		fl, 16);
//		fl|BGBBTJ_BTIC1C_EFL_BLOCKLZ, 16);
	return(i+j);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXT5Z(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2;
	int sz1, sz2;
	
	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKZI))
	{
		sz1=BGBBTJ_BT1C_EncodeBlocksDXT5(
			obuf, osz, blks, lblks, xs, ys, fl);
		return(sz1);
	}
	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1C_EncodeBlocksDXT5(tbuf1, osz, blks, lblks, xs, ys, fl);
	
	if(sz1<256)
//	if(1)
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
	
	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 9);
//	sz2=PDZ2_EncodeStream64LvlZl(tbuf1, tbuf2+6, sz1, osz-6, 5);
	sz2+=6;
	if(sz2<(0.85*sz1))
	{
		memcpy(obuf, tbuf2, sz2);
		obuf[0]=0xE3;			obuf[1]=(sz2>>16)&255;
		obuf[2]=(sz2>>8)&255;	obuf[3]=(sz2>> 0)&255;
		obuf[4]='Z';			obuf[5]='I';
		free(tbuf1);
		free(tbuf2);
		return(sz2);
	}else
	{
		memcpy(obuf, tbuf1, sz1);
		free(tbuf1);
		free(tbuf2);
		return(sz1);
	}
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksDXTn(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl, int stride)
{
	static const int mvtab[32]=
	{
	-1, -1,	 0, -1,	 1, -1,  1,  0,
	 1,  1,  0,  1,	-1,  1, -1,  0,
	-2,  0,  0, -2,	 2,  0,  0,  2,
	-3,  0,  0, -3,	 3,  0,  0,  3,
	};

	int stats[256];
	byte tb[16];
	byte *cs, *cse, *csm, *csl, *csle;
	byte *ct, *cte, *ctb, *lct0;
	byte *cs1, *cs2;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i0, i1, i2, i3;
	int pa0, pa1, pa2, pa3;
	int pb0, pb1, pb2, pb3;
	int is1bpp, is2x2, as1bpp, as2x2;
	int bx, by, bk;
	int bl, bd, csc, csmode;
	int i, j, k, l;
	
	BGBBTJ_RPZA_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	if(fl&BGBBTJ_BTIC1C_EFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
	}

	cs=blks; cse=cs+n*stride;
	csl=lblks; csle=csl+n*stride;
	ct=obuf;

	lct0=ct;

	*ct++=0xE1;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;

	csc=0; csmode=0;
//	if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//		{ *ct++=0xF8; *ct++=0x8F; *ct++=0x02;
//		  *ct++=0xF8; *ct++=0x90; }

	if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
		{ *ct++=0xF8; *ct++=0x8F; *ct++=0x03;
		  *ct++=0xF8; *ct++=0x90; }

//	if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//		{ *ct++=0xF8; *ct++=0x8F; *ct++=0x01;
//		  *ct++=0xF8; *ct++=0x90; }

	for(i=0; i<256; i++)
		stats[i]=0;

	while(cs<cse)
	{
		if(lct0[0]<0xE0)
		{
			if(lct0[0]<0x80)
				{ stats[0]++; }
			else
				{ stats[lct0[0]&0xE0]++; }
		}
		else
			{ stats[lct0[0]]++; }

		lct0=ct;

#if 0
		if(lblks && !memcmp(cs, csl, 8))
		{
			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, csl+i*stride, 8))
					break;
			}

			*ct++=0x80+(i-1);
			cs+=i*stride; csl+=i*stride;
			continue;
		}
#endif

		if(fl&BGBBTJ_BTIC1C_EFL_EXTMODES)
		{
#if 1
			if(lblks && !memcmp(cs, csl, 8))
			{
				for(i=1; (i<8192) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, csl+i*stride, 8))
						break;
				}

				if(i>32)
				{
					j=i-1;
					*ct++=0xEE;
					*ct++=0x00+(j>>8);
					*ct++=j&255;
					cs+=i*stride; csl+=i*stride;
					continue;
				}

				*ct++=0x80+(i-1);
				cs+=i*stride; csl+=i*stride;
				continue;
			}
#endif
		}else
		{
#if 1
			if(lblks && !memcmp(cs, csl, 8))
			{
				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, csl+i*stride, 8))
						break;
				}

				*ct++=0x80+(i-1);
				cs+=i*stride; csl+=i*stride;
				continue;
			}
#endif
		}
		
		if(lblks && (fl&BGBBTJ_BTIC1C_EFL_MOTION))
		{
			bx=0; by=0; bk=0;
#if 0
			for(i=-2; i<=2; i++)
			{
				for(j=-2; j<=2; j++)
				{
					cs1=csl+(i*xs1*stride)+j*stride;
					if(cs1<lblks)continue;
					if(cs1>=csle)continue;

					k=0;
					if(!memcmp(cs, cs1, 8))
					{
						for(k=1; (k<32) && ((cs+k*stride)<cse); k++)
						{
							if(memcmp(cs+k*stride, cs1+k*stride, 8))
								break;
						}
					}
					if(k>bk)
						{ bx=j; by=i; bk=k; }
				}
			}
#endif

#if 1
			for(i=0; i<16; i++)
			{
				cs1=csl+
					(mvtab[i*2+1]*xs1*stride)+
					(mvtab[i*2+0]*stride);
				if(cs1<lblks)continue;
				if(cs1>=csle)continue;

				k=0;
				if(!memcmp(cs, cs1, 8))
				{
					for(k=1; (k<32) && ((cs+k*stride)<cse); k++)
					{
						if(memcmp(cs+k*stride, cs1+k*stride, 8))
							break;
					}
					if(k>bk)
						{ bx=mvtab[i*2+0]; by=mvtab[i*2+1]; bk=k; }
				}
			}
#endif

			if(bk>0)
			{
				if(bk>32)bk=32;
			
				*ct++=0xEE;
				*ct++=0x20+(bk-1);
				*ct++=((by+8)<<4)|(bx+8);

				cs+=bk*stride; csl+=bk*stride;
				continue;
			}
		}

		i0=cs[0]|(cs[1]<<8);
		i1=cs[2]|(cs[3]<<8);

		is1bpp=0;
		is2x2=0;

		if(fl&BGBBTJ_BTIC1C_EFL_EXTMODES)
		{
			is1bpp=(!(cs[4]&0xAA)) && (!(cs[5]&0xAA)) &&
				(!(cs[6]&0xAA)) && (!(cs[7]&0xAA));
			is2x2=((cs[4]==cs[5]) && (cs[6]==cs[7]) &&
				(((cs[4]>>2)&0x33)==(cs[4]&0x33)) &&
				(((cs[6]>>2)&0x33)==(cs[7]&0x33)));

			if(is1bpp)
			{
				p0=((cs[4]&1)<<3)|(cs[4]&4)|((cs[4]&16)>>3)|((cs[4]&64)>>6);
				p1=((cs[5]&1)<<3)|(cs[5]&4)|((cs[5]&16)>>3)|((cs[5]&64)>>6);
				p2=((cs[6]&1)<<3)|(cs[6]&4)|((cs[6]&16)>>3)|((cs[6]&64)>>6);
				p3=((cs[7]&1)<<3)|(cs[7]&4)|((cs[7]&16)>>3)|((cs[7]&64)>>6);
				as1bpp=(p0<<12)|(p1<<8)|(p2<<4)|p3;
			}

			if(is2x2)
			{
				p0=((cs[4]&0x03)<<2)|((cs[4]&0x30)>>4);
				p1=((cs[6]&0x03)<<2)|((cs[6]&0x30)>>4);
				as2x2=(p0<<4)|p1;
			}
		}

		if(BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
			cs, blks, cse, &bl, &bd, fl))
//		if((i0!=i1) && BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
//			cs, blks, cse, &bl, &bd, fl))
		{
			if((bl==1) && (bd==1))
				{ *ct++=0xE8; cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd==1))
				{ *ct++=0xE9; *ct++=bl-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=256))
				{ *ct++=0xEA; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=65536))
			{
				j=bd-1; *ct++=0xEB;
				*ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
			if((bl<=256) && (bd<=256))
				{ *ct++=0xEC; *ct++=bl-1; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd<=65536))
			{
				*ct++=0xED; *ct++=bl-1;
				j=bd-1; *ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
		}

		if(i0<=i1)
		{
			//fully transparent blocks
			if(	(cs[4]==0xFF) && (cs[5]==0xFF) &&
				(cs[6]==0xFF) && (cs[7]==0xFF))
			{
				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(memcmp(cs+i*stride, cs, 8))
						break;
					//i2=cs[i*8*0]|(cs[i*8+1]<<8);
					//i3=cs[i*8+2]|(cs[i*8+3]<<8);
					//if((i2!=i0) || (i2!=i3))
					//	break;
				}
			
				*ct++=0xA0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				j|=0x8000;
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct+=2;
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}

		if(fl&BGBBTJ_BTIC1C_EFL_DIFFCLR)
//		if(0)
		{
			l=-1;
			pa0=BGBBTJ_BT1C_EncodePredictDiffColorA(
				blks, cs, xs1, ys1, fl, stride, csc);
			pb0=BGBBTJ_BT1C_EncodePredictDiffColorB(
				blks, cs, xs1, ys1, fl, stride, csc);

			if((i0==pa0) && (i1==pb0))
			{
				l=csc;
			}else
			{
				l=-1;
				for(i=0; i<8; i++)
				{
					pa0=BGBBTJ_BT1C_EncodePredictDiffColorA(
						blks, cs, xs1, ys1, fl, stride, i);
					pb0=BGBBTJ_BT1C_EncodePredictDiffColorB(
						blks, cs, xs1, ys1, fl, stride, i);
					if((i0==pa0) && (i1==pb0))
						{ l=i; break; }
				}
			}

#if 1
			if((i0==i1) && (l>=0))
			{
				for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
				{
					i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
					i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
					if((i2!=i0) || (i2!=i3))
						break;
					if(	rpza_blkalphap[cs[i*stride*4]] |
						rpza_blkalphap[cs[i*stride*5]] |
						rpza_blkalphap[cs[i*stride*6]] |
						rpza_blkalphap[cs[i*stride*7]])
							break;
				}

				if(i>0)
				{
					if(l!=csc)
						{ *ct++=0xF8; *ct++=0x90+l; csc=l; }
					*ct++=0xA0+(i-1);

					j=BGBBTJ_BT1C_EncodePredictDiffA(
						blks, cs, xs1, ys1, fl, stride, l);
					*ct++=j|0x80;
					cs+=i*stride; csl+=i*stride;
					continue;
				}
			}

			if((i0>i1) && (l>=0))
			{
				csm=rpza_blkmap3;
//				csm=rpza_blkmap4;

				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					j=i*stride;
					i2=cs[j+0]|(cs[j+1]<<8);
					i3=cs[j+2]|(cs[j+3]<<8);
					if((i2!=i0) || (i3!=i1))
						break;
					if(is2x2)
					{
						k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
							(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
							(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
						if(!k)break;
					}
				}

				if(is2x2)
				{
#if 1
//					if(i>=1)
					if(1)
					{
						if(l!=csc)
							{ *ct++=0xF8; *ct++=0x90+l; csc=l; }

						n1=i;
						*ct++=0xF5;
						*ct++=0xC0+(i-1);

						j=BGBBTJ_BT1C_EncodePredictDiffA(
							blks, cs, xs1, ys1, fl, stride, l);
						k=BGBBTJ_BT1C_EncodePredictDiffB(
							blks, cs, xs1, ys1, fl, stride, l);
						*ct++=j|0x80;
						*ct++=k|0x80;

						for(i=0; i<n1; i++)
						{
							p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
							p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//							p2=(p0<<4)|p1;
							p2=(p1<<4)|p0;
							*ct++=csm[p2];
							cs+=stride; csl+=stride;
						}
						continue;
					}

#if 0
					j=BGBBTJ_BT1C_EncodePredictDiffA(
						blks, cs, xs1, ys1, fl, stride, l);
					k=BGBBTJ_BT1C_EncodePredictDiffB(
						blks, cs, xs1, ys1, fl, stride, l);

					*ct++=0xF5;
					*ct++=j;
					*ct++=k|0x80;
					p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
					p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
					p2=(p0<<4)|p1;
					*ct++=csm[p2];
					cs+=stride; csl+=stride;
					continue;
#endif
#endif
				}
			
//				if(i>=1)
//				if(0)
				if(1)
				{
					if(l!=csc)
						{ *ct++=0xF8; *ct++=0x90+l; csc=l; }

					n1=i;
					*ct++=0xC0+(i-1);

					j=BGBBTJ_BT1C_EncodePredictDiffA(
						blks, cs, xs1, ys1, fl, stride, l);
					k=BGBBTJ_BT1C_EncodePredictDiffB(
						blks, cs, xs1, ys1, fl, stride, l);
					*ct++=j|0x80;
					*ct++=k|0x80;

//					j=((i0&0xFFC0)>>1)|(i0&0x1F);
//					k=((i1&0xFFC0)>>1)|(i1&0x1F);
		
//					ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//					ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
//					ct+=4;

					for(i=0; i<n1; i++)
					{
						ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
						ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
						cs+=stride; csl+=stride; ct+=4;
					}
					continue;
				}
			}
#endif
		}

#if 1
		if(i0==i1)
//		if(0)
		{
			for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
			{
//				if(memcmp(cs+i*8, cs, 8))
//					break;
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i2!=i3))
					break;
				if(	rpza_blkalphap[cs[i*stride*4]] |
					rpza_blkalphap[cs[i*stride*5]] |
					rpza_blkalphap[cs[i*stride*6]] |
					rpza_blkalphap[cs[i*stride*7]])
						break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct+=2;
			
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}
#endif

		if(i0>i1)
//		if(1)
		{
			csm=rpza_blkmap3;
//			csm=rpza_blkmap4;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				j=i*stride;
				i2=cs[j+0]|(cs[j+1]<<8);
				i3=cs[j+2]|(cs[j+3]<<8);
				if((i2!=i0) || (i3!=i1))
					break;

				if(is2x2)
				{
					k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
						(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
						(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
					if(!k)break;
				}else if(is1bpp)
				{
//					k=((cs[j+4]==cs[j+5]) && (cs[j+6]==cs[j+7]) &&
//						(((cs[j+4]>>2)&0x33)==(cs[j+4]&0x33)) &&
//						(((cs[j+6]>>2)&0x33)==(cs[j+7]&0x33)));
					k=(!(cs[j+4]&0xAA)) && (!(cs[j+5]&0xAA)) &&
						(!(cs[j+6]&0xAA)) && (!(cs[j+7]&0xAA));
					if(!k)break;
				}
			}

#if 1
			if(is2x2)
			{
				if(i>=1)
//				if(1)
				{
					n1=i;
					*ct++=0xF5;
					*ct++=0xC0+(i-1);

					j=((i0&0xFFC0)>>1)|(i0&0x1F);
					k=((i1&0xFFC0)>>1)|(i1&0x1F);
					ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
					ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
					ct+=4;

					for(i=0; i<n1; i++)
					{
						p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
						p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//						p2=(p0<<4)|p1;
						p2=(p1<<4)|p0;
						*ct++=csm[p2];
						cs+=stride; csl+=stride;
					}
					continue;
				}

#if 1
				*ct++=0xF5;

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;

				p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
				p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//				p2=(p0<<4)|p1;
				p2=(p1<<4)|p0;
				*ct++=csm[p2];
				cs+=stride; csl+=stride;
				continue;
#endif
			}
#endif

#if 1
			if(is1bpp)
			{
				if(i>=1)
//				if(1)
				{
					n1=i;
					*ct++=0xF4;
					*ct++=0xC0+(i-1);

					j=((i0&0xFFC0)>>1)|(i0&0x1F);
					k=((i1&0xFFC0)>>1)|(i1&0x1F);
					ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
					ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
					ct+=4;

					for(i=0; i<n1; i++)
					{
						p0=((cs[4]&1)<<3)|(cs[4]&4)|
							((cs[4]&16)>>3)|((cs[4]&64)>>6);
						p1=((cs[5]&1)<<3)|(cs[5]&4)|
							((cs[5]&16)>>3)|((cs[5]&64)>>6);
						p2=((cs[6]&1)<<3)|(cs[6]&4)|
							((cs[6]&16)>>3)|((cs[6]&64)>>6);
						p3=((cs[7]&1)<<3)|(cs[7]&4)|
							((cs[7]&16)>>3)|((cs[7]&64)>>6);
						p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

						*ct++=(p4>>8)&255;
						*ct++=p4&255;

//						p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
//						p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//						p2=(p0<<4)|p1;
//						p2=(p1<<4)|p0;
//						*ct++=csm[p2];
						cs+=stride; csl+=stride;
					}
					continue;
				}

#if 1
				*ct++=0xF4;

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;

				p0=((cs[4]&1)<<3)|(cs[4]&4)|
					((cs[4]&16)>>3)|((cs[4]&64)>>6);
				p1=((cs[5]&1)<<3)|(cs[5]&4)|
					((cs[5]&16)>>3)|((cs[5]&64)>>6);
				p2=((cs[6]&1)<<3)|(cs[6]&4)|
					((cs[6]&16)>>3)|((cs[6]&64)>>6);
				p3=((cs[7]&1)<<3)|(cs[7]&4)|
					((cs[7]&16)>>3)|((cs[7]&64)>>6);
				p4=(p0<<12)|(p1<<8)|(p2<<4)|p3;

				*ct++=(p4>>8)&255;
				*ct++=p4&255;

//				p0=(cs[4]&0x03)|((cs[4]&0x30)>>2);
//				p1=(cs[6]&0x03)|((cs[6]&0x30)>>2);
//				p2=(p0<<4)|p1;
//				p2=(p1<<4)|p0;
//				*ct++=csm[p2];
				cs+=stride; csl+=stride;
				continue;
#endif
			}
#endif
		
			if(i>1)
//			if(0)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
		
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}
			
//			*ct++=0xC0;
			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
			ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
			cs+=stride; csl+=stride; ct+=8;
			continue;
		}

		if(i0<=i1)
		{
			csm=rpza_blkmap4;
//			csm=rpza_blkmap3;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				i2=cs[i*stride+0]|(cs[i*stride+1]<<8);
				i3=cs[i*stride+2]|(cs[i*stride+3]<<8);
				if((i2!=i0) || (i3!=i1))
					break;
			}

#if 1
			if(i>1)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				k|=0x8000;

				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				
				for(i=0; i<n1; i++)
				{
					ct[0]=csm[cs[4]]; ct[1]=csm[cs[5]];
					ct[2]=csm[cs[6]]; ct[3]=csm[cs[7]];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}
#endif

			*ct++=0xC0;
			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			ct[4]=csm[cs[4]];	ct[5]=csm[cs[5]];
			ct[6]=csm[cs[6]];	ct[7]=csm[cs[7]];
			cs+=stride; csl+=stride; ct+=8;
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

	if(1)
	{
		for(j=0; j<256; j++)
		{
			if(!stats[j])continue;
			printf("%02X:%d ", j, stats[j]);
		}
		printf("\n");
	}

	return(i);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksBC4(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl, int stride)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1;
	int xs2, ys2, xs3, ys3;
	int i0, i1, i2, i3;
	int bl, bd;
	int i, j, k, l;
	
	BGBBTJ_RPZA_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	if(fl&BGBBTJ_BTIC1C_EFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
	}
	
	cs=blks; cse=cs+n*stride; csl=lblks;
	ct=obuf;
	
	*ct++=0xE3;		*ct++=0x00;
	*ct++=0x00;		*ct++=0x00;
	*ct++='A';		*ct++='X';
	
	while(cs<cse)
	{
		if(lblks && !memcmp(cs, csl, 8))
		{
			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, csl+i*stride, 8))
					break;
			}

			*ct++=0x80+(i-1);
			cs+=i*stride; csl+=i*stride;
			continue;
		}

		i0=cs[0];
		i1=cs[1];

		if(BGBBTJ_BT1C_EncodeBlocksDXT1_LookupRun(
			cs, blks, cse, &bl, &bd, fl))
		{
			if((bl==1) && (bd==1))
				{ *ct++=0xE8; cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd==1))
				{ *ct++=0xE9; *ct++=bl-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=256))
				{ *ct++=0xEA; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl==1) && (bd<=65536))
			{
				j=bd-1; *ct++=0xEB;
				*ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
			if((bl<=256) && (bd<=256))
				{ *ct++=0xEC; *ct++=bl-1; *ct++=bd-1;
				cs+=bl*stride; csl+=bl*stride; continue; }
			if((bl<=256) && (bd<=65536))
			{
				*ct++=0xED; *ct++=bl-1;
				j=bd-1; *ct++=(j>>8)&255; *ct++=j&255;
				cs+=bl*stride; csl+=bl*stride; continue;
			}
		}

		if(i0==i1)
		{
			for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(memcmp(cs+i*stride, cs, 8))
					break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);

//				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				j=i0;
				*ct++=j>>1;

//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//				ct+=2;
			
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}

//		if(i0>=i1)
		if(1)
		{
			if(i0>i1)
				{ csm=rpza_blkalphamape2; }
			else
				{ csm=rpza_blkalphamape1; }

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				i2=cs[i*stride+0];
				i3=cs[i*stride+1];
				if((i2!=i0) || (i3!=i1))
					break;
			}
			
			if(i>1)
//			if(0)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=i0; k=i1;
//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
//				ct+=4;

				ct[0]=(j>>1)&0x7F;
				ct[1]=(k>>1)&0x7F;
				ct+=2;

//				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct[2]=k;
//				ct+=3;
				
				for(i=0; i<n1; i++)
				{
					j=cs[2]|(cs[3]<<8)|(cs[4]<<16);
					k=cs[5]|(cs[6]<<8)|(cs[7]<<16);
					ct[0]=csm[j&0xFFF]; ct[1]=csm[(j>>12)&0xFFF];
					ct[2]=csm[k&0xFFF]; ct[3]=csm[(k>>12)&0xFFF];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}
			
			j=i0; k=i1;
//			k|=0x8000;
		
//			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
//			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
//			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct[2]=k;
//			ct+=3;

			ct[0]=(j>>1)&0x7F;
			ct[1]=(k>>1)&0x7F;
			ct+=2;

			j=cs[2]|(cs[3]<<8)|(cs[4]<<16);
			k=cs[5]|(cs[6]<<8)|(cs[7]<<16);
			ct[4]=csm[j&0xFFF];	ct[5]=csm[(j>>12)&0xFFF];
			ct[6]=csm[k&0xFFF];	ct[7]=csm[(k>>12)&0xFFF];
			cs+=stride; csl+=stride; ct+=4;
			continue;
		}
	}

	i=ct-obuf;
	obuf[1]=(i>>16)&0xFF;
	obuf[2]=(i>>8)&0xFF;
	obuf[3]=(i>>0)&0xFF;

	return(i);
}

BGBBTJ_API BGBBTJ_BTIC1C_Context *BGBBTJ_RPZA_AllocContext()
{
	BGBBTJ_BTIC1C_Context *ctx;
	BGBBTJ_BTIC1C_LayerInfo *lyr;
	BGBBTJ_BTIC1C_ImageInfo *hdr;

	ctx=gctalloc("btic1c_context_t", sizeof(BGBBTJ_BTIC1C_Context));
	lyr=gctalloc("btic1c_layerinfo_t", sizeof(BGBBTJ_BTIC1C_LayerInfo));
	hdr=gctalloc("btic1c_imageinfo_t", sizeof(BGBBTJ_BTIC1C_ImageInfo));

	hdr->ctx=ctx;
	hdr->layer=lyr;

	lyr->image[0]=hdr;
	lyr->n_image=1;
	lyr->ctx=ctx;

	ctx->layer[0]=lyr;
	ctx->image[0]=hdr;
	ctx->n_layer=1;
	ctx->n_image=1;
	
	return(ctx);
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeImageFast(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	return(BGBBTJ_RPZA_EncodeImage(
		ctx, rgba, obuf, osz, xs, ys, qf, clrs));
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeImageFast(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	return(BGBBTJ_BT1C_EncodeImage(
		ctx, rgba, obuf, osz, xs, ys, qf, clrs));
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeImage(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int sz;

	hdr=ctx->image[0];
	
	sz=BGBBTJ_RPZA_EncodeImage2(ctx, hdr,
		rgba, obuf, osz, xs, ys, qf, clrs);
	return(sz);
}

BGBBTJ_API int BGBBTJ_RPZA_EncodeImage2(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *hdr,
	byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	s16 *rgba2;
	byte *ldat, *ct, *ct0;
	int xs1, ys1;
	int xs2, ys2;
	int xs3, ys3;
	int cr, cg, cb, ca;
	int sz, bsz, efl;
	int x1, y1, x2, y2;
	int i, j, k, l, n;
	
	hdr->width=xs;
	hdr->height=ys;
	hdr->flags=qf;
//	hdr->clrs=clrs;

	efl=0;
	if(qf&BGBBTJ_BTIC1C_QFL_DIFFCLR)
		efl|=BGBBTJ_BTIC1C_EFL_DIFFCLR;
	if(qf&BGBBTJ_BTIC1C_QFL_BLOCKZI)
		efl|=BGBBTJ_BTIC1C_EFL_BLOCKZI;
	if(qf&BGBBTJ_BTIC1C_QFL_EXTMODES)
		efl|=BGBBTJ_BTIC1C_EFL_EXTMODES;
	if(qf&BGBBTJ_BTIC1C_QFL_MOTION)
		efl|=BGBBTJ_BTIC1C_EFL_MOTION;

	hdr->width2=(xs+3)/4;
	hdr->height2=(ys+3)/4;
	hdr->nblocks=hdr->width2*hdr->height2;
	hdr->nblocksmip=hdr->nblocks;

	if(qf&BGBBTJ_QFL_MIPMAP)
	{
		xs2=xs; ys2=ys; n=0;
		while((xs2>0) || (ys2>0))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
			n+=xs3*ys3;
			xs2>>=1; ys2>>=1;
		}
		hdr->nblocksmip=n;
	}

	if(!hdr->rgba)
	{
//		i=(4*hdr->width2)*(4*hdr->height2)*4;
//		if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
		i=hdr->nblocksmip*16*4;
		hdr->rgba=gcatomic(i);
		hdr->sz_rgba=i;
	}
	
	if(!hdr->data)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*16;
		hdr->data=gcatomic(i);
		hdr->sz_data=i;
	}

#if 1
	if(!hdr->fpdata)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*16;
		hdr->fpdata=gcatomic(i);
		hdr->sz_fpdata=i;
	}
#endif

	xs1=hdr->width2*4;
	ys1=hdr->height2*4;

#if 1
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		xs2=hdr->width2;
		ys2=hdr->height2;
		for(y1=0; y1<ys2; y1++)
			for(x1=0; x1<xs2; x1++)
		{
			for(k=0; k<4; k++)
			{
				for(l=0; l<4; l++)
				{
					ca=rgba[((y1*4+k)*xs+(x1*4+l))*4+3];
					if(ca>0)break;
				}
				if(l<4)break;
			}
			if(k<4)
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						j=((y1*4+y2)*xs+(x1*4+x2))*4;
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=rgba[j+0]; cg=rgba[j+1];
						cb=rgba[j+2]; ca=rgba[j+3];
						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;
					}
				}
			}else
			{
				for(y2=0; y2<4; y2++)
				{
					for(x2=0; x2<4; x2++)
					{
						k=((y1*4+y2)*xs1+(x1*4+x2))*4;
						cr=255; cg=0; cb=255; ca=0;
						hdr->rgba[k+0]=cr; hdr->rgba[k+1]=cg;
						hdr->rgba[k+2]=cb; hdr->rgba[k+3]=ca;
					}
				}
			}
		}
		
//		BGBBTJ_Tex_Dither15(hdr->rgba, hdr->rgba, xs1, ys1);
	}
#endif

#if 0
	if(clrs==BGBBTJ_JPG_RGBA)
	{
		for(i=0; i<ys; i++)
			for(j=0; j<xs; j++)
		{
			BGBBTJ_BlendMagentaEncodeColor(
				rgba+((i*xs+j)*4),
				hdr->rgba+((i*xs1+j)*4));
		
#if 0
			cr=rgba[(i*xs+j)*4+0];		cg=rgba[(i*xs+j)*4+1];
			cb=rgba[(i*xs+j)*4+2];		ca=rgba[(i*xs+j)*4+3];
		
			l=255-ca;
			k=255-((l*l)>>8);
//			k=(ca*ca)>>8;
		
//			if(ca<224)
//			if(k<224)
			if(k<252)
			{
//				cr=((cr*k)+(0*(256-k)))>>8;
//				cg=((cg*k)+(255*(256-k)))>>8;
//				cb=((cb*k)+(255*(256-k)))>>8;

				cr=((cr*k)+(255*(256-k)))>>8;
				cg=((cg*k)+(0*(256-k)))>>8;
				cb=((cb*k)+(255*(256-k)))>>8;

#if 0
				if(((i>>6)^(j>>6))&1)
				{
					cr=((cr*k)+(255*(256-k)))>>8;
					cg=((cg*k)+(0*(256-k)))>>8;
					cb=((cb*k)+(255*(256-k)))>>8;
				}else
				{
					cr=((cr*k)+(0*(256-k)))>>8;
					cg=((cg*k)+(255*(256-k)))>>8;
					cb=((cb*k)+(255*(256-k)))>>8;
				}
#endif
			}

			hdr->rgba[(i*xs+j)*4+0]=cr;
			hdr->rgba[(i*xs+j)*4+1]=cg;
			hdr->rgba[(i*xs+j)*4+2]=cb;
			hdr->rgba[(i*xs+j)*4+3]=ca;
#endif
		}
	}
#endif

#if 1
	if(clrs==BGBBTJ_JPG_RGBA_Q11_4)
	{
		if(!hdr->tmm_rgba)
		{
//			i=(4*hdr->width2)*(4*hdr->height2)*4;
//			if(qf&BGBBTJ_QFL_MIPMAP)i*=2;
			i=hdr->nblocksmip*4*sizeof(s16);
			hdr->tmm_rgba=gcatomic(i);
			hdr->tmn_rgba=gcatomic(i);
			hdr->sz_tmrgba=i;
		}

		BGBBTJ_SplitHdrBlockToneMap_FixedQ3_11(
			(s16 *)rgba, hdr->tmm_rgba, hdr->tmn_rgba,
			hdr->rgba, xs, ys, qf);
	}
#endif

#if 1
	if(qf&BGBBTJ_QFL_MIPMAP)
	{
		xs2=xs1; ys2=ys1; ct0=hdr->rgba;
		while((xs2>1) || (ys2>1))
		{
			if(!xs2)xs2++;
			if(!ys2)ys2++;
			xs3=(xs2+3)/4;
			ys3=(ys2+3)/4;
//			n+=xs3*ys3;
//			ct=ct0+(xs3*ys3)*4;
			ct=ct0+(xs2*ys2)*4;
			BGBBTJ_Tex_HalfSampleDest(ct0, ct, xs2, ys2);
			xs2>>=1; ys2>>=1;
			ct0=ct;
		}
	}
#endif

//	ldat=hdr->ldata;
//	if(qf&BGBBTJ_QFL_IFRAME)
//		ldat=NULL;

	ldat=NULL;
	if(qf&BGBBTJ_QFL_PFRAME)
		ldat=hdr->ldata;

	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		if(qf&BGBBTJ_QFL_MIPMAP)
		{
			efl|=BGBBTJ_BTIC1C_EFL_MIPMAP;
		
			BGBBTJ_BCn_EncodeImageDXT5B_Mip(hdr->fpdata,
				hdr->rgba, xs1, ys1, 4);
			BGBBTJ_BCn_FlipImageDXT5_Mip(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_RPZA_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_RPZA_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_BT1C_EncodeBlocksDXT5Z(obuf, osz,
				hdr->data, ldat, xs1, ys1, efl);
			bsz=16;
		}else
		{
#if 0
			BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
			sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz,
				hdr->data, ldat, xs, ys);
			bsz=8;
#endif
#if 1
			BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, hdr->rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(hdr->fpdata, rgba, xs, ys, 4);
//			BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 4);
			BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
			BGBBTJ_RPZA_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
				xs1, ys1, qf);
//			sz=BGBBTJ_RPZA_EncodeBlocksDXT5(obuf, osz,
//				hdr->data, ldat, xs, ys);
			sz=BGBBTJ_BT1C_EncodeBlocksDXT5Z(obuf, osz,
				hdr->data, ldat, xs1, ys1, efl);
			bsz=16;
#endif
		}
		break;
	case BGBBTJ_JPG_RGB:
		BGBBTJ_BCn_EncodeImageDXT1A(hdr->fpdata, rgba, xs, ys, 3);
//		BGBBTJ_BCn_EncodeImageDXT1F(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz, hdr->data, ldat, xs, ys);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGRA:
		BGBBTJ_BCn_EncodeImageDXT1A_BGRA(hdr->fpdata, rgba, xs, ys, 4);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz, hdr->data, ldat, xs, ys);
		bsz=8;
		break;
	case BGBBTJ_JPG_BGR:
		BGBBTJ_BCn_EncodeImageDXT1F_BGRA(hdr->fpdata, rgba, xs, ys, 3);
		BGBBTJ_BCn_FlipImageDXT1(hdr->fpdata, hdr->data, xs, ys, 8);
		sz=BGBBTJ_RPZA_EncodeBlocksDXT1(obuf, osz, hdr->data, ldat, xs, ys);
		bsz=8;
		break;
	case BGBBTJ_JPG_RGBA_Q11_4:
		BGBBTJ_BCn_EncodeImageDXT5B(hdr->fpdata, hdr->rgba, xs1, ys1, 4);
		BGBBTJ_BCn_FlipImageDXT5(hdr->fpdata, hdr->data, xs1, ys1);
		BGBBTJ_RPZA_QuantizeBlocksDXT5(hdr->data, hdr->data, ldat,
			xs1, ys1, qf);
		sz=BGBBTJ_BT1C_EncodeBlocksDXT5Z(obuf, osz,
			hdr->data, ldat, xs1, ys1, efl);
		i=BGBBTJ_BT1C_EncodeToneMap(obuf+sz, osz-sz,
			hdr->tmm_rgba, hdr->tmn_rgba, xs1, ys1, qf, 4);
		if(i>0)sz+=i;
		bsz=16;
		break;
	default:
		break;
	}
	
#if 1
	if(!ldat)
	{
		ct=obuf+sz;

		ct0=ct;
		*ct++=0xE5;		*ct++=0x00;
		*ct++='H';		*ct++='D';

		*ct++=(xs>>8)&255;	*ct++=(xs>>0)&255;
		*ct++=(ys>>8)&255;	*ct++=(ys>>0)&255;
		*ct++=0x18;			*ct++=clrs;
		*ct++=0x00;			*ct++=0x00;
		*ct++=(qf>>24)&255;	*ct++=(qf>>16)&255;
		*ct++=(qf>>8)&255;	*ct++=(qf>>0)&255;
		
		ct0[1]=ct-ct0;
	
		sz=ct-obuf;
	}
#endif

#if 1
	if(!hdr->ldata)
	{
//		i=hdr->nblocks*16;
		i=hdr->nblocksmip*16;
		hdr->ldata=gcatomic(i);
		hdr->sz_ldata=i;
	}
//	memcpy(hdr->ldata, hdr->data, hdr->nblocks*bsz);
	memcpy(hdr->ldata, hdr->data, hdr->nblocksmip*bsz);
#endif

	return(sz);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeImage(
	BGBBTJ_BTIC1C_Context *ctx, byte *rgba, byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	return(BGBBTJ_RPZA_EncodeImage(ctx, rgba,
		obuf, osz, xs, ys, qf, clrs));
}

byte *BGBBTJ_BT1C_EmitCompLayerHeader(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *img,
	byte *cts, byte *cte)
{
	byte *ct;
	char *s;
	
	ct=cts;
	*ct++=0xE5;		*ct++=0x00;
	*ct++='C';		*ct++='L';

	s=img->name;
	while(*s) { *ct++=*s++; }
	*ct++=0;
	*ct++=0;
	cts[1]=ct-cts;
	
	return(ct);
}

byte *BGBBTJ_BT1C_EmitTagLayerHeader(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *img,
	byte *cts, byte *cte)
{
	byte *ct;
	char *s;
	
	ct=cts;
	*ct++=0xE5;		*ct++=0x00;
	*ct++='T';		*ct++='L';

	s=img->name;
	while(*s) { *ct++=*s++; }
	*ct++=0;
	*ct++=0;
	cts[1]=ct-cts;
	
	return(ct);
}

BGBBTJ_API byte *BGBBTJ_BT1C_EmitLayerImage(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_ImageInfo *img,
	byte *ctt, byte *cte, int qfl)
{
	byte *ldat;
	byte *ct;
	int i, bsz;

	ldat=NULL;
	if(qfl&BGBBTJ_QFL_PFRAME)
		ldat=img->ldata;

	ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ctt, cte);
	switch(img->imgtype)
	{
	case BGBBTJ_JPG_DXT1:
	case BGBBTJ_JPG_DXT1F:
	case BGBBTJ_JPG_DXT1A:
		i=BGBBTJ_RPZA_EncodeBlocksDXT1(ct, cte-ct, img->data, ldat,
			img->width, img->height);
		ct+=i; bsz=8;
		break;
	case BGBBTJ_JPG_DXT5:
	case BGBBTJ_JPG_DXT5F:
		i=BGBBTJ_RPZA_EncodeBlocksDXT5(ct, cte-ct, img->data, ldat,
			img->width, img->height);
		ct+=i; bsz=16;
		break;
	default:
		break;
	}

#if 1
	if(!img->ldata)
	{
		i=img->nblocks*16;
		img->ldata=gcatomic(i);
		img->sz_ldata=i;
	}
	memcpy(img->ldata, img->data, img->nblocks*bsz);
#endif
	
	return(ct);
//	i=BGBBTJ_BT1C_EncodeImage(img, norm, ct, cte-ct, xs, ys, qf, clrs);
}

BGBBTJ_API byte *BGBBTJ_BT1C_EmitTagLayer(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *layer,
	byte *ctt, byte *cte, int qfl)
{
	byte *ct;
	int i;
	
	ct=ctt;
	ct=BGBBTJ_BT1C_EmitTagLayerHeader(ctx, layer, ctt, cte);
	for(i=0; i<layer->n_image; i++)
	{
		ct=BGBBTJ_BT1C_EmitLayerImage(ctx,
			layer->image[i], ct, cte, qfl);
	}
	return(ct);
}

#if 0
// BGBBTJ_API int BGBBTJ_BT1C_EncodeLayerComponentImage(
	BGBBTJ_BTIC1C_Context *ctx, BGBBTJ_BTIC1C_LayerInfo *layer,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	byte *obuf, int osz,
	int xs, int ys, int qf, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *img;
	byte *ct, *cte;

	ct=obuf; cte=obuf+osz;

	img=BTIC1_GetLayerImage(ctx, layer, "RGBA");
	ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
	BGBBTJ_BT1C_EncodeImage(ctx, img, rgba,
		ct, cte-ct, xs, ys, qf, clrs);

	if(norm)
	{
		img=BTIC1_GetLayerImage(ctx, layer, "XYZD");
		ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
		BGBBTJ_BT1C_EncodeImage(ctx, img, norm,
			ct, cte-ct, xs, ys, qf, clrs);
	}

	if(spec)
	{
		img=BTIC1_GetLayerImage(ctx, layer, "SpRGB");
		ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
		BGBBTJ_BT1C_EncodeImage(ctx, img, spec,
			ct, cte-ct, xs, ys, qf, clrs);
	}

	if(luma)
	{
		img=BTIC1_GetLayerImage(ctx, layer, "LuRGB");
		ct=BGBBTJ_BT1C_EmitCompLayerHeader(ctx, img, ct, cte);
		BGBBTJ_BT1C_EncodeImage(ctx, img, luma,
			ct, cte-ct, xs, ys, qf, clrs);
	}
	
	return(ct-obuf);
}
#endif
