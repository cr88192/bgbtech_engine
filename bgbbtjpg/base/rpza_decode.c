#include <bgbbtj.h>

byte rpza_blkmap1[256];			//RPZA -> DXT1 Pixels (A, B)
byte rpza_blkmap2[256];			//RPZA -> DXT1 Pixels (B, A)
byte rpza_blkmap3[256];			//DXT1 -> RPZA Pixels (A, B)
byte rpza_blkmap4[256];			//DXT1 -> RPZA Pixels (B, A)

u16 rpza_blkalphamap1[256];		//DXT1 -> BC4 Pixels (Bit Alpha)
u16 rpza_blkalphamap2[256];		//DXT1 -> BC4 Pixels (A, B)
u16 rpza_blkalphamap3[256];		//RPZA -> BC4 Pixels (A, B)
u16 rpza_blkalphamap4[256];		//RPZA -> BC4 Pixels (B, A)

byte rpza_blkalphap[256];		//RPZA pixels (Bit Alpha) has alpha
byte rpza_blkalphamape1[4096];	//BC4 -> RPZA Pixels (A, B)
byte rpza_blkalphamape2[4096];	//BC4 -> RPZA Pixels (B, A)

u16 rpza_blkidxcolor[256];		//default block index colors
u16 rpza_blkidxmap1[256];		//1BPP -> RPZA Pixels

void BGBBTJ_RPZA_Init()
{
	static int idxa[4]={1, 3, 2, 0};
	static int idxb[4]={0, 2, 3, 1};
	static int idxc[4]={3, 0, 2, 1};
	static int idxd[4]={3, 0, 1, 2};
	static int idxe[4]={0, 1, 3, 5};
	static int idxf[4]={1, 5, 3, 0};
	static int idxg[4]={0, 3, 5, 1};
	static int idxaa[8]={3, 0, 2, 2, 1, 1, 3, 0};
//	static int idxab[8]={3, 0, 2, 2, 2, 1, 1, 1};
	static int idxab[8]={3, 0, 3, 2, 2, 1, 1, 0};
	static int init=0;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int cr, cg, cb;
	int i, j, k, l;
	
	if(init)return;
	init=1;
	
	for(i=0; i<256; i++)
	{
		j=	(idxa[(i>>6)&3]<<0) | (idxa[(i>>4)&3]<<2) |
			(idxa[(i>>2)&3]<<4) | (idxa[(i>>0)&3]<<6);
		k=	(idxb[(i>>6)&3]<<0) | (idxb[(i>>4)&3]<<2) |
			(idxb[(i>>2)&3]<<4) | (idxb[(i>>0)&3]<<6);

		rpza_blkmap1[i]=j;
		rpza_blkmap2[i]=k;

		j=	(idxc[(i>>6)&3]<<0) | (idxc[(i>>4)&3]<<2) |
			(idxc[(i>>2)&3]<<4) | (idxc[(i>>0)&3]<<6);
		k=	(idxd[(i>>6)&3]<<0) | (idxd[(i>>4)&3]<<2) |
			(idxd[(i>>2)&3]<<4) | (idxd[(i>>0)&3]<<6);
		rpza_blkmap3[i]=j;
		rpza_blkmap4[i]=k;
	}

	for(i=0; i<256; i++)
	{
		p0=((i&0x03)!=0x03);
		p1=((i&0x0C)!=0x0C);
		p2=((i&0x30)!=0x30);
		p3=((i&0xC0)!=0xC0);

		k=!(p0&&p1&&p2&&p3);
		rpza_blkalphap[k]=k;

		k=p0|(p1<<3)|(p2<<6)|(p3<<9);
		rpza_blkalphamap1[i]=k;

		p0=idxe[(i>>0)&3];		p1=idxe[(i>>2)&3];
		p2=idxe[(i>>4)&3];		p3=idxe[(i>>6)&3];
		k=p0|(p1<<3)|(p2<<6)|(p3<<9);
		rpza_blkalphamap2[i]=k;

		p0=idxf[(i>>6)&3];		p1=idxf[(i>>4)&3];
		p2=idxf[(i>>2)&3];		p3=idxf[(i>>0)&3];
		k=p0|(p1<<3)|(p2<<6)|(p3<<9);
		rpza_blkalphamap3[i]=k;

		p0=idxg[(i>>6)&3];		p1=idxg[(i>>4)&3];
		p2=idxg[(i>>2)&3];		p3=idxg[(i>>0)&3];
		k=p0|(p1<<3)|(p2<<6)|(p3<<9);
		rpza_blkalphamap4[i]=k;
	}

	for(i=0; i<4096; i++)
	{
		p0=idxaa[(i>>0)&7];		p1=idxaa[(i>>3)&7];
		p2=idxaa[(i>>6)&7];		p3=idxaa[(i>>9)&7];
		k=(p0<<6)|(p1<<4)|(p2<<2)|p3;
		rpza_blkalphamape1[i]=k;

		p0=idxab[(i>>0)&7];		p1=idxab[(i>>3)&7];
		p2=idxab[(i>>6)&7];		p3=idxab[(i>>9)&7];
		k=(p0<<6)|(p1<<4)|(p2<<2)|p3;
		rpza_blkalphamape2[i]=k;
	}

	for(i=0; i<16; i++)
	{
		if(i&8)
		{
			j=(i&4)?(31<<10):(8<<10);
			j|=(i&2)?(31<< 5):(8<< 5);
			j|=(i&1)?(31<< 0):(8<< 0);
		}else
		{
			j=(i&4)?(16<<10):(0<<10);
			j|=(i&2)?(16<< 5):(0<< 5);
			j|=(i&1)?(16<< 0):(0<< 0);
		}
		if(i==8)j|=0x8000;
		
		rpza_blkidxcolor[i]=j;
		
		j=(i*17)>>3;
		k=(j<<10)|(j<<5)|j;
		rpza_blkidxcolor[16+i]=k;
	}

	for(i=0; i<7; i++)
		for(j=0; j<8; j++)
			for(k=0; k<4; k++)
	{
		cr=(i*21)>>2;
		cg=(j*18)>>2;
		cb=(k*42)>>2;
		p0=(cr<<10)|(cg<<5)|cb;
		l=((i*8+j)*4+k);
		rpza_blkidxcolor[32+l]=p0;
	}

	for(i=0; i<256; i++)
	{
		k =(i&128)?0:(3<<14);
		k|=(i& 64)?0:(3<<12);
		k|=(i& 32)?0:(3<<10);
		k|=(i& 16)?0:(3<< 8);
		k|=(i&  8)?0:(3<< 6);
		k|=(i&  4)?0:(3<< 4);
		k|=(i&  2)?0:(3<< 2);
		k|=(i&  1)?0:(3<< 0);
	
		rpza_blkidxmap1[i]=k;
	}
}

static void bgbbtj_rpza_memcpy8(void *dst, void *src)
{
#if defined(X86) || defined(X86_64)
	*(u64 *)dst=*(u64 *)src;
#else
	memcpy(dst, src, 8);
#endif
}

BGBBTJ_API int BGBBTJ_RPZA_DecodeBlocksDXTn(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, int xs, int ys, int stride)
{
	byte tb[16];
	byte *cs, *cse, *csm;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1, ret;
	int i, j, k, l;
	
	BGBBTJ_RPZA_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+stride*n;
//	cs+=4;

	if(ibuf[0]==0xE1)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[3];
		cs=ibuf+4; cse=ibuf+l;
	}
	
	ret=0;
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
			j=(cs[0]<<8)|cs[1];
			cs+=2;
			l=j;
			j=((j&0x7FE0)<<1)|(j&0x1F);

			if(l&0x8000)
			{
				ctb[0]=j&0xFF;
				ctb[1]=(j>>8)&0xFF;
				ctb[2]=j&0xFF;
				ctb[3]=(j>>8)&0xFF;
				ctb[4]=0xFF; ctb[5]=0xFF;
				ctb[6]=0xFF; ctb[7]=0xFF;
			}else
			{
				ctb[0]=j&0xFF;
				ctb[1]=(j>>8)&0xFF;
				ctb[2]=j&0xFF;
				ctb[3]=(j>>8)&0xFF;
				ctb[4]=0; ctb[5]=0;
				ctb[6]=0; ctb[7]=0;
			}

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				bgbbtj_rpza_memcpy8(ct, ctb);
				ct+=stride;
			}
			
//			ct+=(i&31)*8;
			break;
		case 0xC0:
			j=(cs[0]<<8)|cs[1];
			k=(cs[2]<<8)|cs[3];
			cs+=4;
			l=k;
			j=((j&0x7FE0)<<1)|(j&0x1F);
			k=((k&0x7FE0)<<1)|(k&0x1F);

			if(l&0x8000)
			{
				if(j<=k)
				{
					ctb[0]=j&0xFF;
					ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;
					ctb[3]=(k>>8)&0xFF;
					csm=rpza_blkmap1;
				}else
				{
					ctb[0]=k&0xFF;
					ctb[1]=(k>>8)&0xFF;
					ctb[2]=j&0xFF;
					ctb[3]=(j>>8)&0xFF;
					csm=rpza_blkmap2;
				}
			}else
			{
				if(j>k)
				{
					ctb[0]=j&0xFF;
					ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;
					ctb[3]=(k>>8)&0xFF;
					csm=rpza_blkmap1;
				}else
				{
					ctb[0]=k&0xFF;
					ctb[1]=(k>>8)&0xFF;
					ctb[2]=j&0xFF;
					ctb[3]=(j>>8)&0xFF;
					csm=rpza_blkmap2;
				}
			}

//			tb[4]=0; tb[5]=0;
//			tb[6]=0; tb[7]=0;

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
				ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
				cs+=4;
				bgbbtj_rpza_memcpy8(ct, ctb);
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
				bgbbtj_rpza_memcpy8(ct, ct-stride);
				ct+=stride;
				break;
			case 0xE9:
				j=(*cs++)+1;
				while(j--)
				{
					bgbbtj_rpza_memcpy8(ct, ct-stride);
					ct+=stride;
				}
				break;
			case 0xEA:
				j=(*cs++)+1;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_rpza_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEB:
				j=(cs[0]<<8)+cs[1]+1;
				cs+=2;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_rpza_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEC:
				j=(*cs++)+1;
				k=(*cs++)+1;
				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_rpza_memcpy8(ct, ct-k*stride);
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
					bgbbtj_rpza_memcpy8(ct, ct-k*stride);
					ct+=stride;
				}
				break;
				
			case 0xF0:
				i=*cs++;
				switch(i&0xE0)
				{
				case 0x80:
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pal256[cs[0]];
						k=ctx->pal256[cs[1]];
						cs+=2;
						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);

						ctb[0]=j&0xFF;
						ctb[1]=(j>>8)&0xFF;
						ctb[2]=k&0xFF;
						ctb[3]=(k>>8)&0xFF;
						csm=rpza_blkmap1;

						j=rpza_blkidxmap1[cs[0]];
						k=rpza_blkidxmap1[cs[1]];
						cs+=2;

						ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
						ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
						bgbbtj_rpza_memcpy8(ct, ctb);
						ct+=stride;
					}
					break;
				case 0xA0:
					j=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);

					ctb[0]=j&0xFF;
					ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;
					ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						bgbbtj_rpza_memcpy8(ct, ctb);
						ct+=stride;
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);

					ctb[0]=j&0xFF;
					ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;
					ctb[3]=(k>>8)&0xFF;
					csm=rpza_blkmap1;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=rpza_blkidxmap1[cs[0]];
						k=rpza_blkidxmap1[cs[1]];
						cs+=2;

						ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
						ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
						bgbbtj_rpza_memcpy8(ct, ctb);
						ct+=stride;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);

					ctb[0]=j&0xFF;
					ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;
					ctb[3]=(k>>8)&0xFF;
					csm=rpza_blkmap1;

					j=rpza_blkidxmap1[cs[0]];
					k=rpza_blkidxmap1[cs[1]];
					cs+=2;

					ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
					ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
					bgbbtj_rpza_memcpy8(ct, ctb);
					ct+=stride;

					break;
				}
				break;
			default:
				break;
			}
			break;

		default:
			if(cs[1]&0x80)
//			if(1)
			{
				cs--;
				j=(cs[0]<<8)|cs[1];
				k=(cs[2]<<8)|cs[3];
				cs+=4;
				j=((j&0x7FE0)<<1)|(j&0x1F);
				k=((k&0x7FE0)<<1)|(k&0x1F);

				if(j>k)
				{
					ctb[0]=j&0xFF;
					ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;
					ctb[3]=(k>>8)&0xFF;
					csm=rpza_blkmap1;
				}else
				{
					ctb[0]=k&0xFF;
					ctb[1]=(k>>8)&0xFF;
					ctb[2]=j&0xFF;
					ctb[3]=(j>>8)&0xFF;
					csm=rpza_blkmap2;
				}

				ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
				ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
				cs+=4;
				bgbbtj_rpza_memcpy8(ct, ctb);
				ct+=stride;
			}else
			{
				memset(ctb, 0, 8);
				//dummy...
				cs+=31;
				bgbbtj_rpza_memcpy8(ct, ctb); ct+=stride;
			}
			
			break;
		}
	}
	
	if(ret)return(ret);
	
//	csl=lbuf;
//	cs=BGBBTJ_PackBCn_DecodeBlockArray2(ibuf, &csl, blks, count, 8, scan);
	return(cs-ibuf);
}


BGBBTJ_API int BGBBTJ_RPZA_DecodeBlocksBC4(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, int xs, int ys, int stride)
{
	byte tb[16];
	byte *cs, *cse;
	u16 *csm;
	byte *ct, *cte, *ctb;
	int xs1, ys1, n, n1, ret;
	int i, j, k, l;
	
	BGBBTJ_RPZA_Init();
	
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
				bgbbtj_rpza_memcpy8(ct, ctb);
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

			if(j<=k)
//			if(k<=j)
			{
				ctb[0]=j&0xFF;
				ctb[1]=k&0xFF;
				csm=rpza_blkalphamap3;
			}else
			{
				ctb[0]=k&0xFF;
				ctb[1]=j&0xFF;
				csm=rpza_blkalphamap4;
			}

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				j=csm[cs[0]]|(csm[cs[1]]<<12);
				k=csm[cs[2]]|(csm[cs[3]]<<12);
				ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
				ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
				cs+=4;
				bgbbtj_rpza_memcpy8(ct, ctb);
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
				bgbbtj_rpza_memcpy8(ct, ct-stride);
				ct+=stride;
				break;
			case 0xE9:
				j=(*cs++)+1;
				while(j--)
				{
					bgbbtj_rpza_memcpy8(ct, ct-stride);
					ct+=stride;
				}
				break;
			case 0xEA:
				j=(*cs++)+1;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_rpza_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEB:
				j=(cs[0]<<8)+cs[1]+1;
				cs+=2;
				if((ct-j*stride)<blks) { ret=-1; break; }
				bgbbtj_rpza_memcpy8(ct, ct-j*stride);
				ct+=stride;
				break;
			case 0xEC:
				j=(*cs++)+1;
				k=(*cs++)+1;
				if((ct+j*stride)>cte) { ret=-1; break; }
				if((ct-k*stride)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_rpza_memcpy8(ct, ct-k*stride);
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
					bgbbtj_rpza_memcpy8(ct, ct-k*stride);
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

			if(j<=k)
//			if(k<=j)
			{
				ctb[0]=j&0xFF;
				ctb[1]=k&0xFF;
				csm=rpza_blkalphamap3;
			}else
			{
				ctb[0]=k&0xFF;
				ctb[1]=j&0xFF;
				csm=rpza_blkalphamap4;
			}

			j=csm[cs[0]]|(csm[cs[1]]<<12);
			k=csm[cs[2]]|(csm[cs[3]]<<12);
			ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
			ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
			cs+=4;
			bgbbtj_rpza_memcpy8(ct, ctb);
			ct+=stride;
			break;
		}
	}
	
	if(ret)return(ret);
	return(cs-ibuf);
}


BGBBTJ_API int BGBBTJ_RPZA_DecodeBlocksDXT1(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, int xs, int ys)
{
	byte *tbuf;
	int i, j, k;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
		{
			BGBBTJ_RPZA_DecodeBlocksDXTn(ctx, ibuf, isz, blks, xs, ys, 8);
			j=(ibuf[i+1]<<16)|(ibuf[i+2]<<8)|(ibuf[i+3]<<0);
			k=i+j;
		}else
		{
			k=BGBBTJ_RPZA_DecodeBlocksDXTn(ctx, ibuf, isz, blks, xs, ys, 8);
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);
		PDUNZ_DecodeStreamSz(ibuf+8, tbuf, i-8, j, &k, 0);
		BGBBTJ_RPZA_DecodeBlocksDXT1(ctx, tbuf, k, blks, xs, ys);
		free(tbuf);
		k=i;
	}else
	{
		return(-1);
	}
	return(k);
}

BGBBTJ_API int BGBBTJ_RPZA_DecodeBlocksDXT5(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, int xs, int ys)
{
	byte *cs, *csl;
	byte *tbuf;
	int xs1, ys1;
	int i, j, k, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
		{
			BGBBTJ_RPZA_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks+8, xs, ys, 16);
			j=BGBBTJ_RPZA_DecodeBlocksBC4(ctx, ibuf+i, isz-i,
				blks, xs, ys, 16);
			cs=ibuf+i+j;
			k=cs-ibuf;
		}else
		{
			i=BGBBTJ_RPZA_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks+n*8, xs, ys, 8);
			BGBBTJ_BCn_TranscodeBC1ToBC3(blks+n*8, blks, xs, ys);
//			i=BGBBTJ_BT1C_DecodeBlocksDXT5_NoAX(ctx, ibuf, isz,
//				blks, xs, ys);
			cs=ibuf+i;
			k=cs-ibuf;
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);
		PDUNZ_DecodeStreamSz(ibuf+8, tbuf, i-8, j, &k, 0);
		BGBBTJ_RPZA_DecodeBlocksDXT5(ctx, tbuf, k, blks, xs, ys);
		free(tbuf);
		k=i;
	}else
	{
		return(-1);
	}
	return(k);
}

#if 0
// BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT5_NoAX(
	BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, int xs, int ys)
{
	byte tb[16];
	byte *cs, *cse, *csm;
	byte *ct, *cte, *ctb;
	u16 *csa;
	int xs1, ys1, n, n1;
	int i0, i1;
	int i, j, k, l;
	
	BGBBTJ_RPZA_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+16*n;
//	cs+=4;
	
	while((cs<cse) && (ct<cte))
	{
		i=*cs++;
		switch(i&0xE0)
		{
		case 0x80:
			n1=(i&31)+1;
			ct+=n1*16;
			break;
		case 0xA0:
			j=(cs[0]<<8)|cs[1];
			cs+=2;
			l=j;

			if(j&0x8000)
//			if(0)
			{
//				k=(j>>7)&0xF0;
//				k=(k*272)>>8;
//				if(k==240)k=255;
				k=0;
				ctb[0]=k;		ctb[1]=k;		ctb[2]=0;	ctb[3]=0;
				ctb[4]=0;		ctb[5]=0;		ctb[6]=0;	ctb[7]=0;

//				k=((j&0x0700)<<5)|((j&0x0018)<<8)|
//					((j&0x00FF)<<2)|((j&0x0018)>>3);
//				j=((j&0x7FE0)<<1)|(j&0x1F);
				k=((j&0x7FE0)<<1)|(j&0x1F);
				ctb[8]=k&0xFF;
				ctb[9]=(k>>8)&0xFF;
				ctb[10]=k&0xFF;
				ctb[11]=(k>>8)&0xFF;
				ctb[12]=0xFF; ctb[13]=0xFF;
				ctb[14]=0xFF; ctb[15]=0xFF;
			}else
			{
				j=((j&0x7FE0)<<1)|(j&0x1F);
				ctb[0]=255;		ctb[1]=255;		ctb[2]=0;	ctb[3]=0;
				ctb[4]=0;		ctb[5]=0;		ctb[6]=0;	ctb[7]=0;
				ctb[8]=j&0xFF;
				ctb[9]=(j>>8)&0xFF;
				ctb[10]=j&0xFF;
				ctb[11]=(j>>8)&0xFF;
				ctb[12]=0; ctb[13]=0;
				ctb[14]=0; ctb[15]=0;
			}

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
#if defined(X86) || defined(X86_64)
				((u64 *)ct)[0]=((u64 *)ctb)[0];
				((u64 *)ct)[1]=((u64 *)ctb)[1];
#else
				memcpy(ct, ctb, 16);
#endif
				ct+=16;
			}
			
//			ct+=(i&31)*8;
			break;
		case 0xC0:
			i0=(cs[0]<<8)|cs[1];
			i1=(cs[2]<<8)|cs[3];
			cs+=4;
			l=i1;

			if(i1&0x8000)
//			if(0)
			{
				j=((i0&0x7FE0)<<1)|(i0&0x1F);
				k=((i1&0x7FE0)<<1)|(i1&0x1F);

				ctb[0]=0;			ctb[1]=255;
				ctb[8]=j&0xFF;		ctb[9]=(j>>8)&0xFF;
				ctb[10]=k&0xFF;		ctb[11]=(k>>8)&0xFF;
				csm=rpza_blkmap1;
				csa=rpza_blkalphamap1;

			}else
				if(i0&0x8000)
//				if(0)
			{
				j=(i0>>7)&0xF0;		j=(j*272)>>8;
				k=(i1>>7)&0xF0;		k=(k*272)>>8;
				ctb[0]=j;			ctb[1]=k;

				j=((i0&0x0700)<<5)|((i0&0x0018)<<8)|
					((i0&0x00FF)<<2)|((i0&0x0018)>>3);
				k=((i1&0x0700)<<5)|((i1&0x0018)<<8)|
					((i1&0x00FF)<<2)|((i1&0x0018)>>3);
				ctb[8]=j&0xFF;		ctb[9]=(j>>8)&0xFF;
				ctb[10]=k&0xFF;		ctb[11]=(k>>8)&0xFF;

				csm=rpza_blkmap1;
				csa=rpza_blkalphamap2;
			}else
			{
				j=((i0&0x7FE0)<<1)|(i0&0x1F);
				k=((i1&0x7FE0)<<1)|(i1&0x1F);
				ctb[0]=255;				ctb[1]=255;
				
				ctb[8]=j&0xFF;			ctb[9]=(j>>8)&0xFF;
				ctb[10]=k&0xFF;			ctb[11]=(k>>8)&0xFF;
				csm=rpza_blkmap1;
				csa=rpza_blkalphamap2;
			}

			n1=(i&31)+1;
			for(i=0; i<n1; i++)
			{
				ctb[12]=csm[cs[0]]; ctb[13]=csm[cs[1]];
				ctb[14]=csm[cs[2]]; ctb[15]=csm[cs[3]];

				j=csa[ctb[12]]|(csa[ctb[13]]<<12);
				k=csa[ctb[14]]|(csa[ctb[15]]<<12);
				ctb[2]=j&0xFF; ctb[3]=(j>>8)&0xFF; ctb[4]=(j>>16)&0xFF;
				ctb[5]=k&0xFF; ctb[6]=(k>>8)&0xFF; ctb[7]=(k>>16)&0xFF;
					
				cs+=4;
#if defined(X86) || defined(X86_64)
				((u64 *)ct)[0]=((u64 *)ctb)[0];
				((u64 *)ct)[1]=((u64 *)ctb)[1];
#else
				memcpy(ct, ctb, 16);
#endif
				ct+=16;
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
				memcpy(ct, ct-16, 16);
				ct+=16;
				break;
			case 0xE9:
				j=(*cs++)+1;
				while(j--)
				{
					memcpy(ct, ct-16, 16);
					ct+=16;
				}
				break;
			case 0xEA:
				j=(*cs++)+1;
				memcpy(ct, ct-j*16, 16);
				ct+=16;
				break;
			case 0xEB:
				j=(cs[0]<<8)+cs[1]+1;
				cs+=2;
				memcpy(ct, ct-j*16, 16);
				ct+=16;
				break;
			case 0xEC:
				j=(*cs++)+1;
				k=(*cs++)+1;
				while(j--)
				{
					memcpy(ct, ct-k*16, 16);
					ct+=16;
				}
				break;
			case 0xED:
				j=(cs[0])+1;
				k=(cs[1]<<8)+cs[2]+1;
				cs+=3;
				while(j--)
				{
					memcpy(ct, ct-k*16, 16);
					ct+=16;
				}
				break;
			default:
				break;
			}
			break;

		default:
			if(cs[1]&0x80)
//			if(1)
			{
				cs--;
				j=(cs[0]<<8)|cs[1];
				k=(cs[2]<<8)|cs[3];
				cs+=4;
				j=((j&0x7FE0)<<1)|(j&0x1F);
				k=((k&0x7FE0)<<1)|(k&0x1F);

				csm=rpza_blkmap1;

				ctb[0]=255;				ctb[1]=255;
				ctb[2]=0;				ctb[3]=0;
				ctb[4]=0;				ctb[5]=0;
				ctb[6]=0;				ctb[7]=0;
				ctb[8]=j&0xFF;			ctb[9]=(j>>8)&0xFF;
				ctb[10]=k&0xFF;			ctb[11]=(k>>8)&0xFF;
				ctb[12]=csm[cs[0]];		ctb[13]=csm[cs[1]];
				ctb[14]=csm[cs[2]];		ctb[15]=csm[cs[3]];
				cs+=4;
//				memcpy(ct, ctb, 8);
#if defined(X86) || defined(X86_64)
				((u64 *)ct)[0]=((u64 *)ctb)[0];
				((u64 *)ct)[1]=((u64 *)ctb)[1];
#else
				memcpy(ct, ctb, 16);
#endif
				ct+=16;
			}else
			{
				memset(ctb, 0, 8);
				//dummy...
				cs+=31;
				memcpy(ct, ctb, 16); ct+=16;
			}
			
			break;
		}
	}
	
//	csl=lbuf;
//	cs=BGBBTJ_PackBCn_DecodeBlockArray2(ibuf, &csl, blks, count, 8, scan);
	return(cs-ibuf);
}
#endif

int BGBBTJ_RPZA_DecompressFrame(BGBBTJ_AVI_Context *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	BGBBTJ_BTIC1C_Context *info;
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int xs, ys, xs1, ys1;
	int i, j, k, n;

	info=ctx->codec_ctx->data;
	hdr=info->image[0];

	xs=ctx->bmihead->biWidth;
	ys=ctx->bmihead->biHeight;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	hdr->width=xs;
	hdr->height=ys;
	hdr->nblocks=n;

	if(!hdr->data)
	{
		i=hdr->nblocks*16;
		hdr->data=gcatomic(i);
		hdr->sz_data=i;
	}

	if(!hdr->ldata)
	{
		i=hdr->nblocks*16;
		hdr->ldata=gcatomic(i);
		hdr->sz_ldata=i;
	}

#if 1
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		i=BGBBTJ_RPZA_DecodeBlocksDXT5(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BCn_DecodeImageDXT5(hdr->ldata, dst, xs, ys, 4);
		break;
	case BGBBTJ_JPG_RGB:
		i=BGBBTJ_RPZA_DecodeBlocksDXT1(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(hdr->data, hdr->ldata, xs, ys, 8);
		BGBBTJ_BCn_DecodeImageDXT1(hdr->ldata, dst, xs, ys, 3);
		break;
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		i=BGBBTJ_RPZA_DecodeBlocksDXT1(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(hdr->data, dst, xs, ys, 8);
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
		i=BGBBTJ_RPZA_DecodeBlocksDXT5(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(hdr->data, dst, xs, ys);
		break;
	}
#endif
	return(i);
}

BGBBTJ_API int BGBBTJ_RPZA_DecodeFrameImage(BGBBTJ_BTIC1C_Context *ctx,
	void *src, void *dst, int ssz, int dsz,
	int xs, int ys, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int xs1, ys1, bsz;
	int i, j, k, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	hdr=ctx->image[0];
	
	hdr->width=xs;
	hdr->height=ys;
	hdr->nblocks=n;

	if(!hdr->data)
	{
		i=hdr->nblocks*16;
		hdr->data=gcatomic(i);
		hdr->sz_data=i;
	}

	if(!hdr->ldata)
	{
		i=hdr->nblocks*16;
		hdr->ldata=gcatomic(i);
		hdr->sz_ldata=i;
	}

	if(clrs==BGBBTJ_JPG_BC7)
//	if(1)
	{
		if(!hdr->adata)
		{
			i=hdr->nblocks*8;
			hdr->adata=gcatomic(i);
			hdr->sz_data=i;
		}

		if(!hdr->ladata)
		{
			i=hdr->nblocks*8;
			hdr->ladata=gcatomic(i);
//			hdr->sz_ldata=i;
		}
	}

	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		i=BGBBTJ_RPZA_DecodeBlocksDXT5(hdr, src, ssz, hdr->data, xs, ys);
		if(ctx->flip)
		{	 BGBBTJ_BCn_DecodeImageDXT5(hdr->data, dst, xs, ys, 4); }
		else
		{
			BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
			BGBBTJ_BCn_DecodeImageDXT5(hdr->ldata, dst, xs, ys, 4);
		}
		bsz=16;
		break;
	case BGBBTJ_JPG_RGB:
		i=BGBBTJ_RPZA_DecodeBlocksDXT1(hdr, src, ssz, hdr->data, xs, ys);
		if(ctx->flip)
		{	 BGBBTJ_BCn_DecodeImageDXT1(hdr->data, dst, xs, ys, 3); }
		else
		{
			BGBBTJ_BCn_FlipImageDXT1(hdr->data, hdr->ldata, xs, ys, 8);
			BGBBTJ_BCn_DecodeImageDXT1(hdr->ldata, dst, xs, ys, 3);
		}
		bsz=8;
		break;
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		i=BGBBTJ_RPZA_DecodeBlocksDXT1(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(hdr->data, dst, xs, ys, 8);
		bsz=8;
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
		i=BGBBTJ_RPZA_DecodeBlocksDXT5(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(hdr->data, dst, xs, ys);
		bsz=16;
		break;
	case BGBBTJ_JPG_BC7:
		if(ctx->flip)
		{
			i=BGBBTJ_BT1C_DecodeBlocksBC7(hdr, src, ssz,
				hdr->data, hdr->ldata, xs, ys,
				BGBBTJ_BTIC1C_DFL_NONE);
			memcpy(dst, hdr->data, hdr->nblocks*16);
		}
		else
		{
			i=BGBBTJ_BT1C_DecodeBlocksBC7(hdr, src, ssz,
				hdr->data, hdr->ldata, xs, ys,
				BGBBTJ_BTIC1C_DFL_FLIPBLOCKS);
			BGBBTJ_BC7_FlipImageBlocksBC7(hdr->data, dst, xs, ys);
		}
		bsz=16;
		break;
	case BGBBTJ_JPG_BC6H_SF16:
		if(ctx->flip)
		{
			i=BGBBTJ_BT1C_DecodeBlocksBC6H(hdr, src, ssz,
				hdr->data, hdr->ldata, xs, ys, BGBBTJ_BTIC1C_DFL_NONE);
			memcpy(dst, hdr->data, hdr->nblocks*16);
		}
		else
		{
			i=BGBBTJ_BT1C_DecodeBlocksBC6H(hdr, src, ssz,
				hdr->data, hdr->ldata, xs, ys,
				BGBBTJ_BTIC1C_DFL_FLIPBLOCKS);
			BGBBTJ_BC7_FlipImageBlocksBC7(hdr->data, dst, xs, ys);
		}
		bsz=16;
		break;
	case BGBBTJ_JPG_BC6H_UF16:
		if(ctx->flip)
		{
			i=BGBBTJ_BT1C_DecodeBlocksBC6H(hdr, src, ssz,
				hdr->data, hdr->ldata, xs, ys, BGBBTJ_BTIC1C_DFL_BC6H_UF);
			memcpy(dst, hdr->data, hdr->nblocks*16);
		}
		else
		{
			i=BGBBTJ_BT1C_DecodeBlocksBC6H(hdr, src, ssz,
				hdr->data, hdr->ldata, xs, ys,
				BGBBTJ_BTIC1C_DFL_FLIPBLOCKS|BGBBTJ_BTIC1C_DFL_BC6H_UF);
			BGBBTJ_BC7_FlipImageBlocksBC7(hdr->data, dst, xs, ys);
		}
		bsz=16;
		break;
	}

	memcpy(hdr->ldata, hdr->data, hdr->nblocks*bsz);
	if(clrs==BGBBTJ_JPG_BC7)
		{ memcpy(hdr->ladata, hdr->adata, hdr->nblocks*8); }

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

int rpza_decompress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	BGBBTJ_BTIC1C_Context *info;
	
	info=ctx->data;

	return(BGBBTJ_RPZA_DecodeFrameImage(
		info, src, dst, ssz, dsz, info->width, info->height, info->clrs));
}

BGBBTJ_VidCodecCTX *rpza_begin_decompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
	BGBBTJ_BTIC1C_Context *info;
	int err;

	if((fcc!=RIFF_TAG_rpza) && (fcc!=RIFF_TAG_azpr))
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
//	info=gcalloc(sizeof(vfw_ctxinfo));
	info=BGBBTJ_RPZA_AllocContext();
	ctx->data=info;

	info->width=in->biWidth;
	info->height=in->biHeight;

	if(out)
		{ info->clrs=(out->biBitCount==24)?
			BGBBTJ_JPG_RGB:BGBBTJ_JPG_RGBA; }
	else
		{ info->clrs=BGBBTJ_JPG_RGBA; }

	ctx->decompress_frame=&rpza_decompress_frame;

	return(ctx);
}

int BGBBTJ_CodecRPZA_Init()
{
	BGBBTJ_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("RPZA: Init\n");

	codec=BGBBTJ_VidCodec_New();
	codec->begin_decompress=&rpza_begin_decompress;
	return(0);
}
