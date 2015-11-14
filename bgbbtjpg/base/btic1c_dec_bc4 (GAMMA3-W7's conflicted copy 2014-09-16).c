#include <bgbbtj.h>

extern byte bt1c_blkmap1[256];			//RPZA -> DXT1 Pixels (A, B)
extern byte bt1c_blkmap2[256];			//RPZA -> DXT1 Pixels (B, A)
extern byte bt1c_blkmap3[256];			//DXT1 -> RPZA Pixels (A, B)
extern byte bt1c_blkmap4[256];			//DXT1 -> RPZA Pixels (B, A)

extern u16 bt1c_blkalphamap1[256];		//DXT1 -> BC4 Pixels (Bit Alpha)
extern u16 bt1c_blkalphamap2[256];		//DXT1 -> BC4 Pixels (A, B)
extern u16 bt1c_blkalphamap3[256];		//RPZA -> BC4 Pixels (A, B)
extern u16 bt1c_blkalphamap4[256];		//RPZA -> BC4 Pixels (B, A)

extern byte bt1c_blkalphap[256];		//RPZA pixels (Bit Alpha) has alpha
extern byte bt1c_blkalphamape1[4096];	//BC4 -> RPZA Pixels (A, B)
extern byte bt1c_blkalphamape2[4096];	//BC4 -> RPZA Pixels (B, A)

extern u16 bt1c_blkidxcolor[256];		//default block index colors
extern u16 bt1c_blkidxmap1[256];		//1BPP -> RPZA Pixels
extern u32 bt1c_blkidxmap2[256];		//2x2 2BPP -> RPZA Pixels

extern u16 bt1c_difftab[8*256];		//differential color tables
extern int bt1c_basedifftab5[16][32];

extern s16 bt1c_difftaby8[16][128];	//diff7 on RGB24

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte

extern int bt1c_basedifftab[16][5];
extern u32 bt1c_blkidx_pixpat[256];	//1BPP -> RPZA Pixels


#if 0
BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksBC4(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks,
	int xs, int ys, int stride)
{
	byte tb[16];
	byte *cs, *cse;
	u16 *csm;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1, ret;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+stride*n;
	ret=0;

	if(ibuf[0]==0xE3)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[3];
		cs=ibuf+6; cse=ibuf+l;
	}
	
	while((cs<cse) && (ct<cte) && !ret)
	{
		i=*cs++;
		switch(i&0xE0)
		{
		case 0x80:
			n1=(i&31)+1;
			ct+=n1*stride;
			break;
		case 0xA0:
			j=*cs++;
			l=j;

			if(j&0x80)
				{ j=(j<<8)|(*cs++); }
			else
				{ j=j<<1; j|=(j>>7)&1; }

			ctb[0]=j&0xFF;	ctb[1]=j&0xFF;
			ctb[2]=0x00;	ctb[3]=0x00;
			ctb[4]=0x00;	ctb[5]=0x00;
			ctb[6]=0x00;	ctb[7]=0x00;

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				bgbbtj_bt1c_memcpy8(ct, ctb);
				ct+=stride;
			}
			break;
		case 0xC0:
			if(cs[0]&0x80)
			{
				j=(cs[0]<<8)|cs[1];
				k=(cs[2]<<8)|cs[3];
				cs+=4;
			}else
			{
				j=cs[0]<<1;
				k=cs[1]<<1;
				j|=(j>>7)&1;
				k|=(k>>7)&1;
				cs+=2;
			}
			l=k;

//			if(j<=k)
			if(k<=j)
			{
				ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
				csm=bt1c_blkalphamap3;
			}else
			{
				ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
				csm=bt1c_blkalphamap4;
			}

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				j=csm[cs[0]]|(csm[cs[1]]<<12);
				k=csm[cs[2]]|(csm[cs[3]]<<12);
				ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
				ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
				cs+=4;
				bgbbtj_bt1c_memcpy8(ct, ctb);
				ct+=stride;
			}
			break;
			
		case 0xE0:
			switch(i)
			{
			case 0xE0:
				break;
			case 0xE1:
				cs+=3;
				break;
			case 0xE8:
				bgbbtj_bt1c_memcpy8(ct, ct-stride);
				ct+=stride;
				break;
			case 0xE9:
				j=(*cs++)+1;
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-stride);
					ct+=stride;
				}
				break;
			case 0xEA:
				j=(*cs++)+1;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_bt1c_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEB:
				j=(cs[0]<<8)+cs[1]+1;
				cs+=2;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_bt1c_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEC:
				j=(*cs++)+1;
				k=(*cs++)+1;
				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
					ct+=stride;
				}
				break;
			case 0xED:
				j=(cs[0])+1;
				k=(cs[1]<<8)+cs[2]+1;
				cs+=3;
				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
					ct+=stride;
				}
				break;
			default:
				ret=-1;
				break;
			}
			break;

		default:
			j=i;
			k=*cs++;
			
			j=(j&0x7F)<<1;
			k=(k&0x7F)<<1;
			j|=(j>>7)&1;
			k|=(k>>7)&1;
			
//			j=cs[0]<<1;
//			k=cs[1]<<1;
//			cs+=2;

//			if(j<=k)
			if(k<=j)
			{
				ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
				csm=bt1c_blkalphamap3;
			}else
			{
				ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
				csm=bt1c_blkalphamap4;
			}

			j=csm[cs[0]]|(csm[cs[1]]<<12);
			k=csm[cs[2]]|(csm[cs[3]]<<12);
			ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
			ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
			cs+=4;
			bgbbtj_bt1c_memcpy8(ct, ctb);
			ct+=stride;
			break;
		}
	}
	
	if(ret)return(ret);
	return(cs-ibuf);
}
#endif

#if 1
BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksBC4(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks,
	int xs, int ys, int stride)
{
	byte tb[16];
	byte *cs, *cse, *csl;
	u16 *csm;
	byte *ct, *cte, *ctb;
	int xo, yo, xs1, ys1, n, n1, ret, mode, nextmode;
	int diffm, diffsc;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+stride*n;
	csl=lblks;
	if(!csl)csl=blks;

	if(ibuf[0]==0xE3)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[3];
		cs=ibuf+6; cse=ibuf+l;
	}
	
	ret=0; mode=ctx->amodes[0]; nextmode=mode;
	while((cs<cse) && (ct<cte) && !ret)
	{
		i=*cs++;
		switch(i&0xE0)
		{
		case 0x80:
			if(ct!=csl)
			{
				j=(i&31)+1;
				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, csl);
					ct+=stride; csl+=stride;
				}
			}else
			{
				n1=(i&31)+1;
				ct+=n1*stride; csl+=n1*stride;
			}
//			n1=(i&31)+1;
//			ct+=n1*stride;
			break;
			
		case 0xE0:
			switch(i)
			{
			case 0xE0:
				if(mode!=ctx->amodes[0])
					{ mode=ctx->amodes[0]; nextmode=mode; }
				else
					{ ret=1; }
				break;
			case 0xE1:
				mode=ctx->amodes[1];
				nextmode=mode;
				break;
			case 0xE2:
				mode=ctx->amodes[2];
				nextmode=mode;
				break;

			case 0xE3:
			case 0xE4:
				if((cs[3]>='A') && (cs[3]<='Z'))
					{ ret=-1; }
				j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
				cs+=j-1;
				break;
			case 0xE5:
				if((cs[1]>='A') && (cs[1]<='Z'))
					{ ret=-1; }
				j=cs[0];
				cs+=j-1;
				break;

			case 0xED:
				j=*cs++;
				switch((j>>5)&7)
				{
				case 0:
					j=(j&31)+1;
					k=((cs[0]<<8)|(cs[1]))+1; cs+=2;
					break;
				case 1:
					j=(j&31)+1;
					k=(*cs++)+1;
					break;
				case 2:
					k=(((j&31)<<8)|(*cs++))+1;
					j=1;
					break;
				case 3:
					k=(j&31)+1;
					j=1;
					break;
				case 4:
					j=(((j&31)<<8)|(*cs++))+1;
					k=1;
					break;
				case 5:
					j=(j&31)+1;
					k=1;
					break;					
				default:
					j=0; k=0;
					ret=-1;
					break;
				}

				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }

				while(j--)
				{
					bgbbtj_bt1c_memcpy8(ct, ct-k*stride);
					ct+=stride; csl+=stride;
				}
				break;

			case 0xEE:
				j=(cs[0]<<8)+cs[1];
				cs+=2;

				switch((j>>13)&7)
				{
				case 0:
					k=(j&8191)+1;
					xo=0; yo=0;
					break;
				case 1:
					k=((j>>8)&31)+1;
					yo=((j>>4)&15)-8;
					xo=((j   )&15)-8;
					break;
				case 2:
					k=(j&8191)+1;
					yo=cs[0]-128;
					xo=cs[1]-128;
					cs+=2;
					break;
				default:
					xo=0; yo=0; k=0;
					break;
				}

//				if((ct+j*32)>cte) { ret=-1; break; }
//				if((ct-k*32)<blks) { ret=-1; break; }
				l=yo*xs1+xo;

				while(k--)
				{
					bgbbtj_bt1c_memcpy8(ct, csl+l*stride);
					ct+=stride; csl+=stride;
				}
				break;

			case 0xEF:
				j=*cs++;
				switch((j>>5)&7)
				{
				case 0:
					k=j&31;
					ctx->optfl[k>>3]|=(1<<(k&7));
					if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
						ctx->dflags|=BGBBTJ_BTIC1C_DFL_RGB23B;
					break;
				case 1:
					k=j&31;
					ctx->optfl[k>>3]&=~(1<<(k&7));
					if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
						ctx->dflags&=~BGBBTJ_BTIC1C_DFL_RGB23B;
					break;
				case 2:
					k=((j&31)<<8)|(*cs++);
					ctx->optfl[k>>3]|=(1<<(k&7));
					if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
						ctx->dflags|=BGBBTJ_BTIC1C_DFL_RGB23B;
					break;
				case 3:
					k=((j&31)<<8)|(*cs++);
					ctx->optfl[k>>3]&=~(1<<(k&7));
					if(k==BGBBTJ_BTIC1C_CTRFL_RGB23B)
						ctx->dflags&=~BGBBTJ_BTIC1C_DFL_RGB23B;
					break;
				case 4:
					k=j&31;
					l=*cs++;
					ctx->optcntrl[k]=l;
					if(k==BGBBTJ_BTIC1C_CTRL_BLOCKMODE)
						{ nextmode=l; mode=l; }
					break;
				case 5:
					k=j&31;
					l=(cs[0]<<8)|(cs[1]); cs+=2;
					ctx->optcntrl[k]=l;
					if(k==BGBBTJ_BTIC1C_CTRL_BLOCKMODE)
						{ nextmode=l; mode=l; }
					if(k==BGBBTJ_BTIC1C_CTRL_SCALEHDR)
						ctx->dflags|=BGBBTJ_BTIC1C_DFL_SCALEHDR2LDR;
					break;
				case 6:
					k=j&31;
					if(k<24)
					{
						nextmode=mode;
						mode=k;
						break;
					}
					if(k<28)
					{
						k=((k&3)<<8)|(*cs++);
						nextmode=mode;
						mode=k;
						break;
					}
					k=(cs[0]<<8)|(cs[1]); cs+=2;
					nextmode=mode;
					mode=k;
					break;
					
				default: break;
				}
				break;

			case 0xF0:
				switch(cs[0])
				{
				case 0xF0: cs++; mode=ctx->amodes[0]; nextmode=mode; break;
				case 0xF1: cs++; mode=ctx->amodes[1]; nextmode=mode; break;
				case 0xF2: cs++; mode=ctx->amodes[2]; nextmode=mode; break;
				case 0xF3: cs++; mode=ctx->amodes[3]; nextmode=mode; break;
				case 0xF4: cs++; mode=ctx->amodes[4]; nextmode=mode; break;
				case 0xF5: cs++; mode=ctx->amodes[5]; nextmode=mode; break;
				case 0xF6: cs++; mode=ctx->amodes[6]; nextmode=mode; break;
				case 0xF7: cs++; mode=ctx->amodes[7]; nextmode=mode; break;
				case 0xEF:
					if((cs[1]>=0xC0) && (cs[1]<0xD8))
					{
						k=cs[1]&31; cs+=2;
						nextmode=k; mode=mode;
						break;
					}
					if((cs[1]>=0xD8) && (cs[1]<0xDC))
					{
						k=((cs[1]&3)<<8)|cs[2]; cs+=3;
						nextmode=k; mode=mode;
						break;
					}
					nextmode=mode; mode=ctx->amodes[0]; break;
				default: nextmode=mode; mode=ctx->amodes[0]; break;
				}
				break;
			case 0xF1: nextmode=mode; mode=ctx->amodes[1]; break;
			case 0xF2: nextmode=mode; mode=ctx->amodes[2]; break;
			case 0xF3: nextmode=mode; mode=ctx->amodes[3]; break;
			case 0xF4: nextmode=mode; mode=ctx->amodes[4]; break;
			case 0xF5: nextmode=mode; mode=ctx->amodes[5]; break;
			case 0xF6: nextmode=mode; mode=ctx->amodes[6]; break;
			case 0xF7: nextmode=mode; mode=ctx->amodes[7]; break;

			case 0xF8:
			case 0xF9:
			case 0xFA:
			case 0xFB:
//				j=*cs++;
				j=((i-0xF8)<<8)|(*cs++);
				switch(j)
				{
				case 0x81:
					for(j=0; j<256; j++)
						{ ctx->pal256[j]=bt1c_blkidxcolor[j]; }
					for(j=0; j<16; j++)
						{ ctx->pal16[j]=bt1c_blkidxcolor[j]; }
					for(j=0; j<256; j++)
						{ ctx->pat256[j]=bt1c_blkidx_pixpat[j]; }
					for(j=0; j<8; j++)
						{ ctx->modes[j]=j; }
					for(j=0; j<8; j++)
						{ ctx->amodes[j]=j; }
					break;
				case 0x82:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<8)|cs[1];
						cs+=2;
						ctx->pal256[j]=l;
					}
					break;
				case 0x83:
					j=(cs[0]>>4)&15;
					k=(cs[0]&15)+1;
					for(; j<k; j++)
						{ ctx->pal16[j]=ctx->pal256[*cs++]; }
					break;
				case 0x84:
					j=(cs[0]>>4)&15;
					k=(cs[0]&15)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<8)|cs[1]; cs+=2;
						ctx->pal16[j]=l;
					}
					break;
				case 0x85:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						ctx->pat256[j]=l;
					}
					break;

				case 0x86:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=	((l&0x3E0000)>>7)|
							((l&0x007C00)>>5)|
							((l&0x00007C)>>2);
						ctx->pal256[j]=l;
					}
					break;
				case 0x87:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=	((l&0x3E000000)>>15)|
							((l&0x007C0000)>>13)|
							((l&0x00007C00)>>10);
						ctx->pal256[j]=l;
					}
					break;
				case 0x88:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=BGBBTJ_BT1C_ConvRGB31FSToRGB15(ctx, l);
						ctx->pal256[j]=l;
					}
					break;
				case 0x89:
					j=*cs++;
					k=(*cs++)+1;
					for(; j<k; j++)
					{
						l=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=BGBBTJ_BT1C_ConvRGB31FUToRGB15(ctx, l);
						ctx->pal256[j]=l;
					}
					break;

				case 0x8A:
					j=(cs[0]<<8)|cs[1]; cs+=2;
					ctx->amodes[(j>>12)&7]=j&4095;
					break;

				case 0x8F:
					j=*cs++;
					diffm=j;
					ctx->diff_m=diffm;
					break;
				case 0x90: case 0x91:
				case 0x92: case 0x93:
				case 0x94: case 0x95:
				case 0x96: case 0x97:
				case 0x98: case 0x99:
				case 0x9A: case 0x9B:
				case 0x9C: case 0x9D:
				case 0x9E: case 0x9F:
					if(!diffm)
						diffm=1;
					diffsc=j-0x90;
					ctx->diff_m=diffm;
					ctx->diff_sc=diffsc;
					break;
				case 0xA0: case 0xA1:
				case 0xA2: case 0xA3:
				case 0xA4: case 0xA5:
				case 0xA6: case 0xA7:
					k=j-0xA0;
					ctx->clrdb_mode=k;
					break;

				default:
					break;
				}
				break;

#if 0
			case 0xFC:
			case 0xFD:
			case 0xFE:
			case 0xFF:
				j=((i-0xFC)<<8)|(*cs++);
				if((j>=0x80) && (j<=0x9F))
				{
					n1=(j&31)+1;
					BGBBTJ_BT1C_DecodeBlockColorArrayDXTn(ctx,
						blks, ct, &cs, n1,
						xs1, ys1, stride, mode);
					ct+=n1*stride; csl+=n1*stride;
					break;
				}
				break;
#endif

			default:
				ret=-1;
				break;
			}
			break;

		default:
			switch(mode)
			{
#if 1
			case 0:
				switch(i&0xE0)
				{
				case 0xA0:
					j=*cs++;
					l=j;

					if(j&0x80)
					{
						j=bt1c_difftaby8[ctx->diff_sc][j&127];
						k=ctx->diff_lclra;
						j=k+j;
					}else
					{
						j=j<<1;
						j|=(j>>7)&1;
					}
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

//					if(j&0x80)
//						{ j=(j<<8)|(*cs++); }
//					else
//						{ j=j<<1; j|=(j>>7)&1; }

					ctb[0]=j&0xFF;	ctb[1]=j&0xFF;
					ctb[2]=0x00;	ctb[3]=0x00;
					ctb[4]=0x00;	ctb[5]=0x00;
					ctb[6]=0x00;	ctb[7]=0x00;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=bt1c_difftaby8[ctx->diff_sc][cs[0]&0x7F];
						k=bt1c_difftaby8[ctx->diff_sc][cs[1]&0x7F];
						cs+=2;

						j=ctx->diff_lclra+j;
						k=ctx->diff_lclrb+k;

//						j=(cs[0]<<8)|cs[1];
//						k=(cs[2]<<8)|cs[3];
//						cs+=4;
					}else
					{
						j=cs[0]<<1;
						k=cs[1]<<1;
						j|=(j>>7)&1;
						k|=(k>>7)&1;
						cs+=2;
					}
					l=k;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

//					if(j<=k)
					if(k<=j)
					{
						ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
						csm=bt1c_blkalphamap3;
					}else
					{
						ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
						csm=bt1c_blkalphamap4;
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=csm[cs[0]]|(csm[cs[1]]<<12);
						k=csm[cs[2]]|(csm[cs[3]]<<12);
						ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF;
						ctb[4]=(j>>16)&0xFF;
						ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF;
						ctb[7]=(k>>16)&0xFF;
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					j=i;
					k=*cs++;
			
					j=(j&0x7F)<<1;
					k=(k&0x7F)<<1;
					j|=(j>>7)&1;
					k|=(k>>7)&1;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
			
					if(k<=j)
					{
						ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
						csm=bt1c_blkalphamap3;
					}else
					{
						ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
						csm=bt1c_blkalphamap4;
					}

					j=csm[cs[0]]|(csm[cs[1]]<<12);
					k=csm[cs[2]]|(csm[cs[3]]<<12);
					ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
					ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
					cs+=4;
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
#endif

#if 1
			case 1:
				switch(i&0xE0)
				{
				case 0xA0:
					j=*cs++;
					l=j;

					if(j&0x80)
					{
						j=bt1c_difftaby8[ctx->diff_sc][j&127];
						k=ctx->diff_lclra;
						j=k+j;
					}else
					{
						j=j<<1;
						j|=(j>>7)&1;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

//					if(j&0x80)
//						{ j=(j<<8)|(*cs++); }
//					else
//						{ j=j<<1; j|=(j>>7)&1; }

					ctb[0]=j&0xFF;	ctb[1]=j&0xFF;
					ctb[2]=0x00;	ctb[3]=0x00;
					ctb[4]=0x00;	ctb[5]=0x00;
					ctb[6]=0x00;	ctb[7]=0x00;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=bt1c_difftaby8[ctx->diff_sc][cs[0]&0x7F];
						k=bt1c_difftaby8[ctx->diff_sc][cs[1]&0x7F];
						cs+=2;

						j=ctx->diff_lclra+j;
						k=ctx->diff_lclrb+k;

//						j=(cs[0]<<8)|cs[1];
//						k=(cs[2]<<8)|cs[3];
//						cs+=4;
					}else
					{
						j=cs[0]<<1;
						k=cs[1]<<1;
						j|=(j>>7)&1;
						k|=(k>>7)&1;
						cs+=2;
					}
					l=k;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					if(k<=j)
					{
						ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
						csm=bt1c_blkalphamap3;
					}else
					{
						ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
						csm=bt1c_blkalphamap4;
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;

						j=csm[(j>>8)&255]|(csm[j&255]<<12);
						k=csm[(k>>8)&255]|(csm[k&255]<<12);
						ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF;
						ctb[4]=(j>>16)&0xFF;
						ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF;
						ctb[7]=(k>>16)&0xFF;

						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					j=i;
					k=*cs++;
			
					j=(j&0x7F)<<1;
					k=(k&0x7F)<<1;
					j|=(j>>7)&1;
					k|=(k>>7)&1;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					if(k<=j)
					{
						ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
						csm=bt1c_blkalphamap3;
					}else
					{
						ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
						csm=bt1c_blkalphamap4;
					}

					j=bt1c_blkidxmap1[cs[0]];
					k=bt1c_blkidxmap1[cs[1]];
					cs+=2;
					j=csm[(j>>8)&255]|(csm[j&255]<<12);
					k=csm[(k>>8)&255]|(csm[k&255]<<12);
					ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
					ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
#endif

#if 1
			case 2:
				switch(i&0xE0)
				{
				case 0xA0:
					j=*cs++;
					l=j;

					if(j&0x80)
					{
						j=bt1c_difftaby8[ctx->diff_sc][j&127];
						k=ctx->diff_lclra;
						j=k+j;
					}else
					{
						j=j<<1;
						j|=(j>>7)&1;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

//					if(j&0x80)
//						{ j=(j<<8)|(*cs++); }
//					else
//						{ j=j<<1; j|=(j>>7)&1; }

					ctb[0]=j&0xFF;	ctb[1]=j&0xFF;
					ctb[2]=0x00;	ctb[3]=0x00;
					ctb[4]=0x00;	ctb[5]=0x00;
					ctb[6]=0x00;	ctb[7]=0x00;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=bt1c_difftaby8[ctx->diff_sc][cs[0]&0x7F];
						k=bt1c_difftaby8[ctx->diff_sc][cs[1]&0x7F];
						cs+=2;

						j=ctx->diff_lclra+j;
						k=ctx->diff_lclrb+k;

//						j=(cs[0]<<8)|cs[1];
//						k=(cs[2]<<8)|cs[3];
//						cs+=4;
					}else
					{
						j=cs[0]<<1;
						k=cs[1]<<1;
						j|=(j>>7)&1;
						k|=(k>>7)&1;
						cs+=2;
					}
					l=k;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					if(k<=j)
					{
						ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
						csm=bt1c_blkalphamap3;
					}else
					{
						ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
						csm=bt1c_blkalphamap4;
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						l=bt1c_blkidxmap2[*cs++];
						j=csm[(l>>24)&255]|(csm[(l>>16)&255]<<12);
						k=csm[(l>> 8)&255]|(csm[(l    )&255]<<12);
						ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF;
						ctb[4]=(j>>16)&0xFF;
						ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF;
						ctb[7]=(k>>16)&0xFF;

						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					j=i;
					k=*cs++;
			
					j=(j&0x7F)<<1;
					k=(k&0x7F)<<1;
					j|=(j>>7)&1;
					k|=(k>>7)&1;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
			
					if(k<=j)
					{
						ctb[0]=j&0xFF;	ctb[1]=k&0xFF;
						csm=bt1c_blkalphamap3;
					}else
					{
						ctb[0]=k&0xFF;	ctb[1]=j&0xFF;
						csm=bt1c_blkalphamap4;
					}

					l=bt1c_blkidxmap2[*cs++];
					j=csm[(l>>24)&255]|(csm[(l>>16)&255]<<12);
					k=csm[(l>> 8)&255]|(csm[(l    )&255]<<12);
					ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
					ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
#endif

#if 1
			case 3:
			case 4:
			case 5:
				switch(i&0xE0)
				{
				case 0xA0:
					j=(cs[0]<<8)|cs[1]; cs+=2;
					l=j;

					if(j&0x8000)
					{
//						j=bt1c_difftaby8[ctx->diff_sc][j&127];
						j=(j&0x7FFF)<<ctx->diff_sc;
						k=ctx->diff_lclra;
						j=k+j;
					}else
					{
						j=j<<1;
						j|=(j>>15)&1;
					}
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

					ctb[0]=0x80|((j>>12)&0x0F);
					ctb[1]=0x70|((j>>12)&0x0F);
					ctb[2]=(j>>4)&0xFF;
					ctb[3]=(j>>4)&0xFF;
					ctb[4]=0x00;	ctb[5]=0x00;
					ctb[6]=0x00;	ctb[7]=0x00;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				case 0xC0:
					j=(cs[0]<<8)|cs[1];
					k=(cs[2]<<8)|cs[3];
					cs+=4;

					if(j&0x8000)
					{
						j=ctx->diff_lclra+((j&0x7FFF)<<ctx->diff_sc);
						k=ctx->diff_lclrb+((k&0x7FFF)<<ctx->diff_sc);
					}else
					{
						j=j<<1;
						k=k<<1;
						j|=(j>>15)&1;
						k|=(k>>15)&1;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					csm=bt1c_blkmap1;

					ctb[0]=0x80|((j>>12)&0x0F);
					ctb[1]=0x70|((j>>12)&0x0F);
					ctb[2]=(j>>4)&0xFF;
					ctb[3]=(j>>4)&0xFF;

					if(mode==3)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
							ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
							cs+=4;
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else if(mode==4)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
							ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else if(mode==5)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							ctb[4]=csm[(j>>24)&255];
							ctb[5]=csm[(j>>16)&255];
							ctb[6]=csm[(j>> 8)&255];
							ctb[7]=csm[j&255];
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				default:
					cs--;
					j=(cs[0]<<8)|cs[1];
					k=(cs[2]<<8)|cs[3];
					cs+=4;
			
					j=(j&0x7FFF)<<1;
					k=(k&0x7FFF)<<1;
					j|=(j>>15)&1;
					k|=(k>>15)&1;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
			
					csm=bt1c_blkmap1;

					ctb[0]=0x80|((j>>12)&0x0F);
					ctb[1]=0x70|((j>>12)&0x0F);
					ctb[2]=(j>>4)&0xFF;
					ctb[3]=(j>>4)&0xFF;

					if(mode==3)
					{
						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else if(mode==4)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
						ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else if(mode==5)
					{
						j=bt1c_blkidxmap2[*cs++];
						ctb[4]=csm[(j>>24)&255];
						ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255];
						ctb[7]=csm[j&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				}
				break;
#endif
			}
		}
	}
	
	if(ret)return(ret);
	return(cs-ibuf);
}
#endif
