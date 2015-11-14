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

u32 rpza_blkidx_pixpat[256]=		//1BPP -> RPZA Pixels
{
0x00000000, 0x003C3C00, 0x00282800, 0x28BEBE28, //0x00
0x3C3C3C3C, 0x69696969, 0x28282828, 0x7D7D7D7D, //0x04
0x00FFFF00, 0x55AAAA55, 0x00AAAA00, 0x55FFFF55, //0x08
0x3CFFFF3C, 0x7DFFFF7D, 0x28AAAA28, 0xD35554D2, //0x0C
0x00000F0F, 0x00010A1B, 0x00000A0B, 0x00151F1F, //0x10
0x3C3C3F3F, 0x696D6F6F, 0x28282A2F, 0x7D7D7F7F, //0x14
0x00FFFF0F, 0x55AABF5F, 0x00AAAB0B, 0x55FFFF5F, //0x18
0x3CFFFF3F, 0x7DFFFF7F, 0x28AAAF2F, 0xD3554FCF, //0x1C
0x0000F0F0, 0x0040A0E4, 0x0000A0E0, 0x0054F4F4, //0x20
0x3C3CFCFC, 0x6979F9F9, 0x2828A8F8, 0x7D7DFDFD, //0x24
0x00FFFFF0, 0x55AAFEF5, 0x00AAEAE0, 0x55FFFFF5, //0x28
0x3CFFFFFC, 0x7DFFFFFD, 0x28AAFAF8, 0xD3550502, //0x2C
0x0000FFFF, 0x0055AAFF, 0x0000AAFF, 0x0055FFFF, //0x30
0x3C3CFFFF, 0x697DFFFF, 0x2828AAFF, 0x7D7DFFFF, //0x34
0x00FFFFFF, 0x55AAFFFF, 0x00AAFFFF, 0x55FFFFFF, //0x38
0x3CFFFFFF, 0x7DFFFFFF, 0x28AAFFFF, 0xD354FFFF, //0x3C
0x0F0F0000, 0x1B0A0100, 0x0B0A0000, 0x1F1F1500, //0x40
0x3F3F3C3C, 0x6F6F6D69, 0x2F2A2828, 0x7F7F7D7D, //0x44
0x0FFFFF00, 0x5FBFAA55, 0x0BABAA00, 0x5FFFFF55, //0x48
0x3FFFFF3C, 0x7FFFFF7D, 0x2FAFAA28, 0xD05054D2, //0x4C
0x0F0F0F0F, 0x1B1B1B1B, 0x0B0B0B0B, 0x1F1F1F1F, //0x50
0x3F3F3F3F, 0x6F6F6F6F, 0x2F2F2F2F, 0x7F7F7F7F, //0x54
0x0FFFFF0F, 0x5FBFBF5F, 0x0BABAB0B, 0x5FFFFF5F, //0x58
0x3FFFFF3F, 0x7FFFFF7F, 0x2FAFAF2F, 0xD0504FCF, //0x5C
0x0F0FF0F0, 0x1B5AA5E4, 0x0B0AA0E0, 0x1F5FF5F4, //0x60
0x3F3FFCFC, 0x6F7FFDF9, 0x2F2AA8F8, 0x7F7FFDFD, //0x64
0x0FFFFFF0, 0x5FBFFEF5, 0x0BABEAE0, 0x5FFFFFF5, //0x68
0x3FFFFFFC, 0x7FFFFFFD, 0x2FAFFAF8, 0xD0500502, //0x6C
0x0F0FFFFF, 0x1B5FBFFF, 0x0B0BABFF, 0x1F5FFFFF, //0x70
0x3F3FFFFF, 0x6F7FFFFF, 0x2F2FAFFF, 0x7F7FFFFF, //0x74
0x0FFFFFFF, 0x5FBFFFFF, 0x0BABFFFF, 0x5FFFFFFF, //0x78
0x3FFFFFFF, 0x7FFFFFFF, 0x2FAFFFFF, 0xD04FFFFF, //0x7C
0xF0F00000, 0xE4A04000, 0xE0A00000, 0xF4F45400, //0x80
0xFCFC3C3C, 0xF9F97969, 0xF8A82828, 0xFDFD7D7D, //0x84
0xF0FFFF00, 0xF5FEAA55, 0xE0EAAA00, 0xF5FFFF55, //0x88
0xFCFFFF3C, 0xFDFFFF7D, 0xF8FAAA28, 0x030554D2, //0x8C
0xF0F00F0F, 0xE4A55A1B, 0xE0A00A0B, 0xF4F55F1F, //0x90
0xFCFC3F3F, 0xF9FD7F6F, 0xF8A82A2F, 0xFDFD7F7F, //0x94
0xF0FFFF0F, 0xF5FEBF5F, 0xE0EAAB0B, 0xF5FFFF5F, //0x98
0xFCFFFF3F, 0xFDFFFF7F, 0xF8FAAF2F, 0x03054FCF, //0x9C
0xF0F0F0F0, 0xE4E4E4E4, 0xE0E0E0E0, 0xF4F4F4F4, //0xA0
0xFCFCFCFC, 0xF9F9F9F9, 0xF8F8F8F8, 0xFDFDFDFD, //0xA4
0xF0FFFFF0, 0xF5FEFEF5, 0xE0EAEAE0, 0xF5FFFFF5, //0xA8
0xFCFFFFFC, 0xFDFFFFFD, 0xF8FAFAF8, 0x03050502, //0xAC
0xF0F0FFFF, 0xE4F5FEFF, 0xE0E0EAFF, 0xF4F5FFFF, //0xB0
0xFCFCFFFF, 0xF9FDFFFF, 0xF8F8FAFF, 0xFDFDFFFF, //0xB4
0xF0FFFFFF, 0xF5FEFFFF, 0xE0EAFFFF, 0xF5FFFFFF, //0xB8
0xFCFFFFFF, 0xFDFFFFFF, 0xF8FAFFFF, 0x0304FFFF, //0xBC
0xFFFF0000, 0xFFAA5500, 0xFFAA0000, 0xFFFF5500, //0xC0
0xFFFF3C3C, 0xFFFF7D69, 0xFFAA2828, 0xFFFF7D7D, //0xC4
0xFFFFFF00, 0xFFFFAA55, 0xFFFFAA00, 0xFFFFFF55, //0xC8
0xFFFFFF3C, 0xFFFFFF7D, 0xFFFFAA28, 0x000054D2, //0xCC
0xFFFF0F0F, 0xFFBF5F1B, 0xFFAB0B0B, 0xFFFF5F1F, //0xD0
0xFFFF3F3F, 0xFFFF7F6F, 0xFFAF2F2F, 0xFFFF7F7F, //0xD4
0xFFFFFF0F, 0xFFFFBF5F, 0xFFFFAB0B, 0xFFFFFF5F, //0xD8
0xFFFFFF3F, 0xFFFFFF7F, 0xFFFFAF2F, 0x00004FCF, //0xDC
0xFFFFF0F0, 0xFFFEF5E4, 0xFFEAE0E0, 0xFFFFF5F4, //0xE0
0xFFFFFCFC, 0xFFFFFDF9, 0xFFFAF8F8, 0xFFFFFDFD, //0xE4
0xFFFFFFF0, 0xFFFFFEF5, 0xFFFFEAE0, 0xFFFFFFF5, //0xE8
0xFFFFFFFC, 0xFFFFFFFD, 0xFFFFFAF8, 0x00000502, //0xEC
0xCC33CC33, 0x99669966, 0x44114411, 0x88228822, //0xF0
0xCC30CC03, 0x99659956, 0x44104401, 0x88208802, //0xF4
0xCC330C03, 0x99665956, 0x44110401, 0x88220802, //0xF8
0xCC300C03, 0x99655956, 0x44100401, 0xFFFFFFFF, //0xFC
};

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
		l=(cs[1]<<16)|(cs[2]<<8)|cs[2];
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
		l=(cs[1]<<16)|(cs[2]<<8)|cs[2];
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
				{ j=j<<1; }

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
				cs+=2;
			}
			l=k;

			if(j<=k)
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
			
//			j=cs[0]<<1;
//			k=cs[1]<<1;
//			cs+=2;

			if(j<=k)
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

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT1(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, int xs, int ys)
{
	return(BGBBTJ_RPZA_DecodeBlocksDXT1(ctx, ibuf, isz, blks, xs, ys));
//	return(BGBBTJ_RPZA_DecodeBlocksDXTn(ibuf, isz, blks, xs, ys, 8));
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
			BGBBTJ_RPZA_DecodeBlocksDXTn(ctx, ibuf, isz, blks+8, xs, ys, 16);
			j=BGBBTJ_RPZA_DecodeBlocksBC4(ctx, ibuf+i, isz-i, blks, xs, ys, 16);
			cs=ibuf+i+j;
		}else
		{
//			i=BGBBTJ_RPZA_DecodeBlocksDXTn(ibuf, isz, blks+n*8, xs, ys, 8);
//			BGBBTJ_BCn_TranscodeBC1ToBC3(blks+n*8, blks, xs, ys);
			i=BGBBTJ_BT1C_DecodeBlocksDXT5_NoAX(ctx, ibuf, isz, blks, xs, ys);
			cs=ibuf+i;
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

//	cs=ibuf; csl=lbuf;
//	cs=BGBBTJ_PackBCn_DecodeBlockArray2(cs, &csl, blks, count, 16, scan);
//	cs=BGBBTJ_PackBCn_DecodeBlockArray2(cs, &csl, blks+8, count, 16, scan);
	return(cs-ibuf);
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT5(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, int xs, int ys)
{

	return(BGBBTJ_RPZA_DecodeBlocksDXT5(ctx, ibuf, isz, blks, xs, ys));

#if 0
	byte *cs, *csl;
	int xs1, ys1;
	int i, j, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
	if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
	{
		BGBBTJ_RPZA_DecodeBlocksDXTn(ibuf, isz, blks+8, xs, ys, 16);
		BGBBTJ_RPZA_DecodeBlocksBC4(ibuf, isz, blks, xs, ys, 16);
	}else
	{
//		BGBBTJ_RPZA_DecodeBlocksDXTn(ibuf, isz, blks+n*8, xs, ys, 8);
//		BGBBTJ_BCn_TranscodeBC1ToBC3(blks+n*8, blks, xs, ys);

		BGBBTJ_BT1C_DecodeBlocksDXT5_NoAX(ibuf, isz, blks, xs, ys, 8);
	}
	return(cs-ibuf);
#endif
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT5_NoAX(
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


int BGBBTJ_RPZA_DecompressFrame(BGBBTJ_AVI_Context *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
//	static byte *blkbuf=NULL;
//	static byte *blkbuf2=NULL;
	BGBBTJ_BTIC1C_Context *info;
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int xs, ys, xs1, ys1;
	int i, j, k, n;

//	if(!ctx->vidCtx)

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

//	if(!blkbuf)
//	{
//		blkbuf=malloc((n+64)*16);
//		blkbuf2=malloc((n+64)*16);
//	}

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
		i=BGBBTJ_BT1C_DecodeBlocksDXT1(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(hdr->data, dst, xs, ys, 8);
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
		i=BGBBTJ_BT1C_DecodeBlocksDXT5(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(hdr->data, dst, xs, ys);
		break;
	}
#endif

#if 0
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
//		BGBBTJ_RPZA_DecodeBlocksDXT1(src, ssz, blkbuf, xs, ys);
//		BGBBTJ_BCn_FlipImageDXT1(blkbuf, blkbuf2, xs, ys, 8);
//		BGBBTJ_BCn_DecodeImageDXT1(blkbuf2, dst, xs, ys, 4);

		i=BGBBTJ_BT1C_DecodeBlocksDXT5(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(blkbuf, blkbuf2, xs, ys);
		BGBBTJ_BCn_DecodeImageDXT5(blkbuf2, dst, xs, ys, 4);
//		BGBBTJ_BCn_DecodeImageDXT5(blkbuf, dst, xs, ys, 4);

		break;
	case BGBBTJ_JPG_RGB:
		i=BGBBTJ_RPZA_DecodeBlocksDXT1(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(blkbuf, blkbuf2, xs, ys, 8);
		BGBBTJ_BCn_DecodeImageDXT1(blkbuf2, dst, xs, ys, 3);
		break;
	case BGBBTJ_JPG_BC1:
		i=BGBBTJ_RPZA_DecodeBlocksDXT1(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(blkbuf, dst, xs, ys, 8);
		break;
	case BGBBTJ_JPG_BC3:
		i=BGBBTJ_BT1C_DecodeBlocksDXT5(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(blkbuf, dst, xs, ys);
		break;
	}
#endif
	return(i);
}

int BGBBTJ_BT1C_DecompressFrame(BGBBTJ_AVI_Context *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	return(BGBBTJ_RPZA_DecompressFrame(
		ctx, src, dst, ssz, dsz, clrs));

#if 0
	static byte *blkbuf=NULL;
	static byte *blkbuf2=NULL;
	int xs, ys, xs1, ys1;
	int i, j, k, n;

	xs=ctx->bmihead->biWidth;
	ys=ctx->bmihead->biHeight;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	if(!blkbuf)
	{
		blkbuf=malloc((n+64)*16);
		blkbuf2=malloc((n+64)*16);
	}
	
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		i=BGBBTJ_RPZA_DecodeBlocksDXT5(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(blkbuf, blkbuf2, xs, ys);
		BGBBTJ_BCn_DecodeImageDXT5(blkbuf2, dst, xs, ys, 4);
		break;
	case BGBBTJ_JPG_RGB:
		i=BGBBTJ_RPZA_DecodeBlocksDXT1(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(blkbuf, blkbuf2, xs, ys, 8);
		BGBBTJ_BCn_DecodeImageDXT1(blkbuf2, dst, xs, ys, 3);
		break;
	case BGBBTJ_JPG_BC1:
		i=BGBBTJ_BT1C_DecodeBlocksDXT1(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(blkbuf, dst, xs, ys, 8);
		break;
	case BGBBTJ_JPG_BC3:
		i=BGBBTJ_BT1C_DecodeBlocksDXT5(hdr, src, ssz, blkbuf, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(blkbuf, dst, xs, ys);
		break;
	}
	return(i);
#endif
}


BGBBTJ_API int BGBBTJ_RPZA_DecodeFrameImage(BGBBTJ_BTIC1C_Context *ctx,
	void *src, void *dst, int ssz, int dsz,
	int xs, int ys, int clrs)
{
//	static byte *blkbuf=NULL;
//	static byte *blkbuf2=NULL;
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int xs1, ys1;
	int i, j, k, n;

//	xs=ctx->bmihead->biWidth;
//	ys=ctx->bmihead->biHeight;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
//	if(!blkbuf)
//	{
//		blkbuf=malloc((n+64)*16);
//		blkbuf2=malloc((n+64)*16);
//	}

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
		break;
	case BGBBTJ_JPG_BC1:
	case BGBBTJ_JPG_BC1F:
	case BGBBTJ_JPG_BC1A:
		i=BGBBTJ_BT1C_DecodeBlocksDXT1(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT1(hdr->data, dst, xs, ys, 8);
		break;
	case BGBBTJ_JPG_BC3:
	case BGBBTJ_JPG_BC3F:
		i=BGBBTJ_BT1C_DecodeBlocksDXT5(hdr, src, ssz, hdr->data, xs, ys);
		BGBBTJ_BCn_FlipImageDXT5(hdr->data, dst, xs, ys);
		break;
	}
	return(i);
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

	if((fcc!=RIFF_TAG_rpza) && (fcc!=RIFF_TAG_azpr) &&
			(fcc!=RIFF_TAG_bt1c) && (fcc!=RIFF_TAG_c1tb))
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

#if 0
	info->ihead=gcalloc(sizeof(BGBBTJ_BMPInfoHeader));
	memset(info->ihead, 0, sizeof(BGBBTJ_BMPInfoHeader));
	info->ihead->biSize		= sizeof(BGBBTJ_BMPInfoHeader);
	info->ihead->biWidth		= in->biWidth;
	info->ihead->biHeight		= in->biHeight;
	info->ihead->biPlanes		= in->biPlanes;
	info->ihead->biBitCount		= in->biBitCount;
	info->ihead->biCompression	= in->biCompression;
	info->ihead->biSizeImage	=
		in->biWidth*in->biHeight*in->biBitCount/8;

	info->ohead=gcalloc(sizeof(BGBBTJ_BMPInfoHeader));
	memset(info->ohead, 0, sizeof(BGBBTJ_BMPInfoHeader));
	info->ohead->biSize		= sizeof(BGBBTJ_BMPInfoHeader);
	info->ohead->biWidth		= out->biWidth;
//	info->ohead->biHeight		= -out->biHeight;
	info->ohead->biHeight		= out->biHeight;
	info->ohead->biPlanes		= out->biPlanes;
	info->ohead->biBitCount		= out->biBitCount;
	info->ohead->biCompression	= out->biCompression;
	info->ihead->biSizeImage	=
		out->biWidth*out->biHeight*out->biBitCount/8;
#endif

//	info->buffer=malloc(out->width*out->height*out->bpp/8);

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
