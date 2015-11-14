#include <bgbbtj.h>

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksMB1(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	int i, j;
	
	i=BGBBTJ_BT1C_EncodeBlocksMB1C(
		obuf, osz, blks, lblks, xs, ys, fl, 32);
	j=BGBBTJ_BT1C_EncodeBlocksMB1A(
		obuf+i, osz-i, blks+8, lblks?(lblks+8):NULL, xs, ys,
		fl, 32);
//		fl|BGBBTJ_BTIC1C_EFL_BLOCKLZ, 16);
	return(i+j);
}

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksMB1Z(
	byte *obuf, int osz, byte *blks, byte *lblks,
	int xs, int ys, int fl)
{
	byte *tbuf1, *tbuf2;
	int sz1, sz2;
	
	if(!(fl&BGBBTJ_BTIC1C_EFL_BLOCKZI))
	{
		sz1=BGBBTJ_BT1C_EncodeBlocksMB1(
			obuf, osz, blks, lblks, xs, ys, fl);
		return(sz1);
	}
	
	tbuf1=malloc(osz);
	tbuf2=malloc(osz);
	sz1=BGBBTJ_BT1C_EncodeBlocksMB1(tbuf1, osz, blks, lblks, xs, ys, fl);
	
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

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksMB1C(
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
	
	BGBBTJ_BT1C_Init();
	
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

		if(fl&BGBBTJ_BTIC1C_EFL_EXTMODES)
		{
			if(lblks && BGBBTJ_BT1XMB_CheckBlockSkipP(cs, csl))
			{
				for(i=1; (i<8192) && ((cs+i*stride)<cse); i++)
				{
					if(!BGBBTJ_BT1XMB_CheckBlockSkipP(
							cs+i*stride, csl+i*stride))
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
		}else
		{
			if(lblks && BGBBTJ_BT1XMB_CheckBlockSkipP(cs, csl))
			{
				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(!BGBBTJ_BT1XMB_CheckBlockSkipP(
							cs+i*stride, csl+i*stride))
						break;
				}

				*ct++=0x80+(i-1);
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}
		
		if(lblks && (fl&BGBBTJ_BTIC1C_EFL_MOTION))
		{
			bx=0; by=0; bk=0;

			if(BGBBTJ_BT1XMB_GetBlockTranslate(cs, &bx, &by))
			{
				for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
				{
					if(!BGBBTJ_BT1XMB_CheckBlockTranslateP(
							cs+i*stride, bx, by))
						break;
				}
				bk=i;
			}

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

		i0=BGBBTJ_BT1XMB_GetColorA_As565(cs);
		i1=BGBBTJ_BT1XMB_GetColorB_As565(cs);

		is1bpp=0;
		is2x2=0;

		if(fl&BGBBTJ_BTIC1C_EFL_EXTMODES)
		{
			is1bpp=BGBBTJ_BT1XMB_Is1BppP(cs);
			is2x2=BGBBTJ_BT1XMB_Is2x2P(cs);
			if(is1bpp)
				{ as1bpp=BGBBTJ_BT1XMB_As1BppP(cs); }
			if(is2x2)
				{ as2x2=BGBBTJ_BT1XMB_As2x2P(cs); }
		}

#if 0
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
#endif

		//fully transparent blocks
		if(BGBBTJ_BT1XMB_FullyTransparentP(cs))
		{
			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(!BGBBTJ_BT1XMB_FullyTransparentP(cs+i*stride))
					break;
			}
			*ct++=0xA0+(i-1);
			j=((i0&0xFFC0)>>1)|(i0&0x1F); j|=0x8000;
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct+=2;
			cs+=i*stride; csl+=i*stride;
			continue;
		}

#if 0
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
		}
#endif

		if(BGBBTJ_BT1XMB_IsFlatColorP(cs))
		{
			for(i=0; (i<32) && ((cs+i*stride)<cse); i++)
			{
				j=i*stride;
				if(!BGBBTJ_BT1XMB_IsColorMatchP(cs+j, cs))
					break;
				if(!BGBBTJ_BT1XMB_IsFlatColorP(cs+j))
					break;
			}
			
			if(i>0)
			{
				*ct++=0xA0+(i-1);
				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF; ct+=2;
				cs+=i*stride; csl+=i*stride;
				continue;
			}
		}

		if(BGBBTJ_BT1XMB_IsSimpleOpaqueP(cs))
		{
			csm=bt1c_blkmap3;
//			csm=bt1c_blkmap4;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				j=i*stride;
				if(!BGBBTJ_BT1XMB_IsColorMatchP(cs+j, cs)) break;
				if(is2x2)
					{ if(!BGBBTJ_BT1XMB_Is2x2P(cs+j)) break; }
				else if(is1bpp)
					{ if(!BGBBTJ_BT1XMB_Is1BppP(cs+j)) break; }
			}

#if 1
			if(is2x2)
			{
				if(i>=1)
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
						p2=BGBBTJ_BT1XMB_As2x2x2bpp(cs);
						*ct++=csm[p2];
						cs+=stride; csl+=stride;
					}
					continue;
				}

				*ct++=0xF5;
				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				p2=BGBBTJ_BT1XMB_As2x2x2bpp(cs);
				*ct++=csm[p2];
				cs+=stride; csl+=stride;
				continue;
			}
#endif

#if 1
			if(is1bpp)
			{
				if(i>=1)
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
						p4=BGBBTJ_BT1XMB_As1Bpp(cs);
						*ct++=(p4>>8)&255;	*ct++=p4&255;
						cs+=stride; csl+=stride;
					}
					continue;
				}

				*ct++=0xF4;
				j=((i0&0xFFC0)>>1)|(i0&0x1F);
				k=((i1&0xFFC0)>>1)|(i1&0x1F);
				ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
				ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
				ct+=4;
				p4=BGBBTJ_BT1XMB_As1Bpp(cs);
				*ct++=(p4>>8)&255;	*ct++=p4&255;
				cs+=stride; csl+=stride;
				continue;
			}
#endif
		
			if(i>1)
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
					p0=BGBBTJ_BT1XMB_As4x4x2bpp(cs);
					ct[0]=csm[(p0    )&255]; ct[1]=csm[(p0>> 8)&255];
					ct[2]=csm[(p0>>16)&255]; ct[3]=csm[(p0>>24)&255];
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
			ct+=4;
			p0=BGBBTJ_BT1XMB_As4x4x2bpp(cs);
			ct[0]=csm[(p0    )&255]; ct[1]=csm[(p0>> 8)&255];
			ct[2]=csm[(p0>>16)&255]; ct[3]=csm[(p0>>24)&255];
			cs+=stride; csl+=stride; ct+=8;
			continue;
		}

		if(i0<=i1)
		{
			csm=bt1c_blkmap4;
//			csm=bt1c_blkmap3;

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				if(!BGBBTJ_BT1XMB_IsColorMatchP(cs+(i*stride), cs))
					break;
			}

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
					p0=BGBBTJ_BT1XMB_As4x4x2bpp(cs);
					ct[0]=csm[(p0    )&255]; ct[1]=csm[(p0>> 8)&255];
					ct[2]=csm[(p0>>16)&255]; ct[3]=csm[(p0>>24)&255];
					cs+=stride; csl+=stride; ct+=4;
				}
				continue;
			}

			*ct++=0xC0;
			j=((i0&0xFFC0)>>1)|(i0&0x1F);
			k=((i1&0xFFC0)>>1)|(i1&0x1F);
			k|=0x8000;
		
			ct[0]=(j>>8)&0xFF;	ct[1]=j&0xFF;
			ct[2]=(k>>8)&0xFF;	ct[3]=k&0xFF;
			p0=BGBBTJ_BT1XMB_As4x4x2bpp(cs);
			ct[0]=csm[(p0    )&255]; ct[1]=csm[(p0>> 8)&255];
			ct[2]=csm[(p0>>16)&255]; ct[3]=csm[(p0>>24)&255];
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

BGBBTJ_API int BGBBTJ_BT1C_EncodeBlocksMB1A(
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
	
	BGBBTJ_BT1C_Init();
	
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
				{ csm=bt1c_blkalphamape2; }
			else
				{ csm=bt1c_blkalphamape1; }

			for(i=1; (i<32) && ((cs+i*stride)<cse); i++)
			{
				i2=cs[i*stride+0];
				i3=cs[i*stride+1];
				if((i2!=i0) || (i3!=i1))
					break;
			}
			
			if(i>1)
			{
				n1=i;
				*ct++=0xC0+(i-1);

				j=i0; k=i1;

				ct[0]=(j>>1)&0x7F;
				ct[1]=(k>>1)&0x7F;
				ct+=2;

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
