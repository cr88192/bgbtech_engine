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

extern u16 bt1c_difftab[16*256];		//differential color tables
extern u32 bt1c_difftab24[16*256];		//diff7 on RGB24

extern int bt1c_basedifftab5[16][32];

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte

extern int bt1c_basedifftab[8][5];
extern u32 bt1c_blkidx_pixpat[256];	//1BPP -> RPZA Pixels


void BGBBTJ_BT1C_FillColorDeblock(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte *ctc, int idx,
	int xs1, int ys1, int stride, int mode)
{
	byte *ct0, *ct1, *ct2;
	int p0, p1, p2, p3, p4, p5;

	if(!mode)
	{
		bgbbtj_bt1c_memcpy8(ct, ctc);
		return;
	}
	
	ct0=ct-xs1*stride-stride;
	ct1=ct-xs1*stride;
	ct2=ct-stride;

//	p0=0; p1=0; p2=0;

	p3=(ctc[0])|((ctc[1])<<8);
	p0=p3; p1=p3; p2=p3;
	
	if(ct0>=blks)
	{
		//1111 0111 1101 1111
		p4=(ct0[0])|((ct0[1])<<8);
		p5=(ct0[2])|((ct0[3])<<8);
		p4=p4&0xF7DF; p5=p5&0xF7DF;
		p0=(p4+p5)>>1;
//		p0=(ct0[2])|((ct0[3])<<8);
//		p0=BGBBTJ_BT1C_ColorCloserAB(p4, p5, p3);
		p0=BGBBTJ_BT1C_ColorCloserABC(p4, p5, p0, p3);
	}
	if(ct1>=blks)
	{
		p4=(ct1[0])|((ct1[1])<<8);
		p5=(ct1[2])|((ct1[3])<<8);
		p4=p4&0xF7DF; p5=p5&0xF7DF;
		p1=(p4+p5)>>1;
//		p1=(ct1[2])|((ct1[3])<<8);
//		p1=BGBBTJ_BT1C_ColorCloserAB(p4, p5, p3);
		p1=BGBBTJ_BT1C_ColorCloserABC(p4, p5, p1, p3);
	}
	if(ct2>=blks)
	{
		p4=(ct2[0])|((ct2[1])<<8);
		p5=(ct2[2])|((ct2[3])<<8);
		p4=p4&0xF7DF; p5=p5&0xF7DF;
		p2=(p4+p5)>>1;
//		p2=(ct2[2])|((ct2[3])<<8);
//		p2=BGBBTJ_BT1C_ColorCloserAB(p4, p5, p3);
		p2=BGBBTJ_BT1C_ColorCloserABC(p4, p5, p2, p3);
	}

//	p3=(ctc[0])|((ctc[1])<<8);

	switch(mode)
	{
	case 0:
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	case 1:
		if(!idx)
		{
			//00 10 11 01 -> 78
			bgbbtj_bt1c_setule16(ct+0, p2);
			bgbbtj_bt1c_setule16(ct+2, p3);
			bgbbtj_bt1c_setule32(ct+4, 0x78787878);
			break;
		}
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	case 2:
		//00 10 00 10 -> 88
		//11 10 11 10 -> BB
		//10 11 10 11 -> DD
		//01 11 01 11 -> EE
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p3);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;
	case 3:
		if(!idx)
		{
			//00 00 10 10 -> A0
			//00 10 11 11 -> F8
			//10 10 11 01 -> 7A
			//11 11 01 01 -> 5F
			bgbbtj_bt1c_setule16(ct+0, p0);
			bgbbtj_bt1c_setule16(ct+2, p3);
			bgbbtj_bt1c_setule32(ct+4, 0xA0F87A5F);
			break;
		}
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p3);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;
	case 4:
		//00 00 10 10 -> A0
		//00 10 11 11 -> F8
		//10 10 11 01 -> 7A
		//11 11 01 01 -> 5F
		p4=BGBBTJ_BT1C_Paeth(p2, p1, p0);
		bgbbtj_bt1c_setule16(ct+0, p4);
		bgbbtj_bt1c_setule16(ct+2, p3);
		bgbbtj_bt1c_setule32(ct+4, 0xA0F87A5F);
		break;
	case 5:
//		p4=2*p3-p2;
		p4=BGBBTJ_BT1C_LinearExtrapolate565(p2, p3);
		if(!idx)
		{
			//00 10 11 01 -> 78
			bgbbtj_bt1c_setule16(ct+0, p2);
			bgbbtj_bt1c_setule16(ct+2, p4);
			bgbbtj_bt1c_setule32(ct+4, 0x78787878);
			break;
		}
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	case 6:
		//00 10 00 10 -> 88
		//11 10 11 10 -> BB
		//10 11 10 11 -> DD
		//01 11 01 11 -> EE
//		p4=2*p3-p1;
		p4=BGBBTJ_BT1C_LinearExtrapolate565(p1, p3);
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p4);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;
	case 7:
		if(!idx)
		{
			//00 00 10 10 -> A0
			//00 10 11 11 -> F8
			//10 10 11 01 -> 7A
			//11 11 01 01 -> 5F
//			p4=2*p3-p0;
			p4=BGBBTJ_BT1C_LinearExtrapolate565(p0, p3);
			bgbbtj_bt1c_setule16(ct+0, p0);
			bgbbtj_bt1c_setule16(ct+2, p4);
			bgbbtj_bt1c_setule32(ct+4, 0xA0F87A5F);
			break;
		}
//		p4=2*p3-p1;
		p4=BGBBTJ_BT1C_LinearExtrapolate565(p1, p3);
		bgbbtj_bt1c_setule16(ct+0, p1);
		bgbbtj_bt1c_setule16(ct+2, p4);
		bgbbtj_bt1c_setule32(ct+4, 0x88BBDDEE);
		break;

	default:
		bgbbtj_bt1c_memcpy8(ct, ctc);
		break;
	}
}

void BGBBTJ_BT1C_DecodeRawBlockDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte *csa, byte **rcs, int num,
	int xs1, int ys1, int stride, int mode, int dfl)
{
	byte tblk[32];
	u32 aclr[16], bclr[4];
	byte pixt[16*4], bpixt[4*4], cpixt[16*4];
	byte *cs, *csm;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int blt;
	int i, j, k;
	
	cs=*rcs;
//	memset(ct, 0, 32);
	switch(mode)
	{
	case 0: case 4:
	case 5: case 14:
		if(!(cs[2]&0x80) && (cs[4]&0x80))
		{
			j=(cs[0]<<8)|(cs[1]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			j=(cs[2]<<8)|(cs[3]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			j=(cs[4]<<8)|(cs[5]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			j=(cs[6]<<8)|(cs[7]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			cs+=8;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<8)|(cs[1]);
			k=BGBBTJ_BT1C_ConvRGB15ToRGB24(ctx, j);
			aclr[i]=k;
			cs+=2;
		}
		blt=0;
//		cs+=32;
		break;
	case 6:		case 10:
	case 11:	case 12:
	case 13:
		if(!(cs[3]&0x80) && (cs[6]&0x80))
		{
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			j=(cs[3]<<16)|(cs[4]<<8)|(cs[5]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			j=(cs[6]<<16)|(cs[7]<<8)|(cs[8]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			j=(cs[9]<<16)|(cs[10]<<8)|(cs[11]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			cs+=12;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
			k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
			aclr[i]=k;
			cs+=3;
		}
		blt=0;
//		cs+=48;
		break;

	case 7:
		if(!(cs[4]&0x80) && (cs[8]&0x80))
		{
			j=(cs[ 0]<<24)|(cs[ 1]<<16)|(cs[ 2]<<8)|(cs[ 3]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			j=(cs[ 4]<<24)|(cs[ 5]<<16)|(cs[ 6]<<8)|(cs[ 7]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			j=(cs[ 8]<<24)|(cs[ 9]<<16)|(cs[10]<<8)|(cs[11]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			j=(cs[12]<<24)|(cs[13]<<16)|(cs[14]<<8)|(cs[15]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			cs+=16;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=BGBBTJ_BT1C_ConvRGB31FSToRGB24(ctx, j);
			aclr[i]=k;
			cs+=4;
		}
		blt=0;
//		cs+=48;
		break;
	case 8:
		if(!(cs[4]&0x80) && (cs[8]&0x80))
		{
			j=(cs[ 0]<<24)|(cs[ 1]<<16)|(cs[ 2]<<8)|(cs[ 3]);
			bclr[0]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			j=(cs[ 4]<<24)|(cs[ 5]<<16)|(cs[ 6]<<8)|(cs[ 7]);
			bclr[1]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			j=(cs[ 8]<<24)|(cs[ 9]<<16)|(cs[10]<<8)|(cs[11]);
			bclr[2]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			j=(cs[12]<<24)|(cs[13]<<16)|(cs[14]<<8)|(cs[15]);
			bclr[3]=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			cs+=16;
			blt=1;
			break;
		}
		
		for(i=0; i<16; i++)
		{
			j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
			k=BGBBTJ_BT1C_ConvRGB31FUToRGB24(ctx, j);
			aclr[i]=k;
			cs+=4;
		}
		blt=0;
//		cs+=48;
		break;

	default:
		blt=0;
		break;
	}

	if(!blt)
	{
#if defined(X86) || defined(X86_64) || defined(ARM)
		BGBBTJ_BCn_EncodeBlockDXT1F_BGRA(ct, (byte *)(&aclr), 4, 16, 3);
#else
		for(i=0; i<16; i++)
		{
			pixt[i*4+0]=(aclr[i]>>16)&255;
			pixt[i*4+1]=(aclr[i]>>8)&255;
			pixt[i*4+2]=(aclr[i]>>0)&255;
			pixt[i*4+3]=(aclr[i]>>24)&255;
		}
		BGBBTJ_BCn_EncodeBlockDXT1F(ct, pixt, 4, 16, 3);
#endif
		*rcs=cs;
		return;
	}

	for(i=0; i<4; i++)
	{
		bpixt[i*4+0]=(bclr[i]>>16)&255;
		bpixt[i*4+1]=(bclr[i]>>8)&255;
		bpixt[i*4+2]=(bclr[i]>>0)&255;
		bpixt[i*4+3]=(bclr[i]>>24)&255;
	}

#if 0
	if(stride>=16)
	{
		memcpy(tblk, ct-8, 8);
		memset(tblk+8, 0, 24);
	}else
	{
		memset(tblk, 0, 32);
		tblk[0]=255; tblk[1]=255;
	}

	i=*cs;
	switch((i>>5)&7)
	{
	case 4:
		tblk[ 8]=0x55;		tblk[ 9]=0x00;
		tblk[10]=0xAC;		tblk[11]=i&31;
		tblk[16]=bpixt[ 0];	tblk[17]=bpixt[ 1];
		tblk[18]=bpixt[ 2];	tblk[19]=bpixt[ 3];
		tblk[20]=bpixt[ 4];	tblk[21]=bpixt[ 5];
		tblk[22]=bpixt[ 6];	tblk[23]=bpixt[ 7];
		tblk[24]=bpixt[ 8];	tblk[25]=bpixt[ 9];
		tblk[26]=bpixt[10];	tblk[27]=bpixt[11];
		tblk[28]=bpixt[12];	tblk[29]=bpixt[13];
		tblk[30]=bpixt[14];	tblk[31]=bpixt[15];
		break;
	case 5:
		tblk[ 8]=0x55;	tblk[ 9]=0x00;
		tblk[10]=0xAB;	tblk[11]=i&31;
		tblk[16]=bpixt[ 0];	tblk[17]=bpixt[ 1];
		tblk[18]=bpixt[ 2];	// ct[19]=bpixt[ 3];
		tblk[20]=bpixt[ 4];	tblk[21]=bpixt[ 5];
		tblk[22]=bpixt[ 6];	// ct[23]=bpixt[ 7];
		tblk[24]=bpixt[ 8];	tblk[25]=bpixt[ 9];
		tblk[26]=bpixt[10];	// ct[27]=bpixt[11];
		tblk[28]=bpixt[12];	tblk[29]=bpixt[13];
		tblk[30]=bpixt[14];	// ct[31]=bpixt[15];
		
		i=(cs[0]<<16)|(cs[1]<<8)|(cs[2]);
		cs+=3;
		tblk[19]=((i>>15)&31)<<3;
		tblk[23]=((i>>10)&31)<<3;
		tblk[25]=((i>>5)&31)<<3;
		tblk[29]=((i>>0)&31)<<3;

		csm=bt1c_blkmap1;
		tblk[12]=csm[cs[0]]; tblk[13]=csm[cs[1]];
		tblk[14]=csm[cs[2]]; tblk[15]=csm[cs[3]];
		cs+=4;
		break;
	case 6:
		tblk[ 8]=0x55;	tblk[ 9]=0x00;
		tblk[10]=0xAB;	tblk[11]=i&31;
		tblk[16]=bpixt[ 0];	tblk[17]=bpixt[ 1];
		tblk[18]=bpixt[ 2];	// ct[19]=bpixt[ 3];
		tblk[20]=bpixt[ 4];	tblk[21]=bpixt[ 5];
		tblk[22]=bpixt[ 6];	// ct[23]=bpixt[ 7];
		tblk[24]=bpixt[ 8];	tblk[25]=bpixt[ 9];
		tblk[26]=bpixt[10];	// ct[27]=bpixt[11];
		tblk[28]=bpixt[12];	tblk[29]=bpixt[13];
		tblk[30]=bpixt[14];	// ct[31]=bpixt[15];

		switch(i&31)
		{
		case 0:
			tblk[19]=cs[1];	tblk[23]=cs[2];
			tblk[27]=cs[3];	tblk[31]=cs[4];
			cs+=5;
			break;
		case 1:
//			ct[11]=cs[2];	ct[15]=cs[3];
//			ct[19]=cs[4];	ct[23]=cs[5];
			cs+=6;
			break;
		}

		csm=bt1c_blkmap1;
		tblk[12]=csm[cs[0]]; tblk[13]=csm[cs[1]];
		tblk[14]=csm[cs[2]]; tblk[15]=csm[cs[3]];
		cs+=4;
		
		break;
	}
#endif

#if 1
	i=*cs;
	switch((i>>5)&7)
	{
	case 4:
		switch(i&31)
		{
		case 0:
			for(j=0; j<4; j++)
			{
				pixt[0*16+0*4+j]=bpixt[0*4+j];
				pixt[0*16+1*4+j]=bpixt[0*4+j];
				pixt[1*16+0*4+j]=bpixt[0*4+j];
				pixt[1*16+1*4+j]=bpixt[0*4+j];
				pixt[0*16+2*4+j]=bpixt[1*4+j];
				pixt[0*16+3*4+j]=bpixt[1*4+j];
				pixt[1*16+2*4+j]=bpixt[1*4+j];
				pixt[1*16+3*4+j]=bpixt[1*4+j];
				pixt[2*16+0*4+j]=bpixt[2*4+j];
				pixt[2*16+1*4+j]=bpixt[2*4+j];
				pixt[3*16+0*4+j]=bpixt[2*4+j];
				pixt[3*16+1*4+j]=bpixt[2*4+j];
				pixt[2*16+2*4+j]=bpixt[3*4+j];
				pixt[2*16+3*4+j]=bpixt[3*4+j];
				pixt[3*16+2*4+j]=bpixt[3*4+j];
				pixt[3*16+3*4+j]=bpixt[3*4+j];
			}
			break;
		case 1:
			for(j=0; j<4; j++)
			{
				j0=bpixt[0*4+j];	j1=bpixt[1*4+j];
				j2=bpixt[2*4+j];	j3=bpixt[3*4+j];
				
				i0=16*j0+ 0*j2;		i1=16*j1+ 0*j3;
				pixt[0*16+0*4+j]=(16*i0+ 0*i1)>>5;
				pixt[0*16+1*4+j]=(11*i0+ 5*i1)>>5;
				pixt[0*16+2*4+j]=( 5*i0+11*i1)>>5;
				pixt[0*16+3*4+j]=( 0*i0+16*i1)>>5;
				i0=11*j0+ 5*j2;		i1=11*j1+ 5*j3;
				pixt[1*16+0*4+j]=(16*i0+ 0*i1)>>5;
				pixt[1*16+1*4+j]=(11*i0+ 5*i1)>>5;
				pixt[1*16+2*4+j]=( 5*i0+11*i1)>>5;
				pixt[1*16+3*4+j]=( 0*i0+16*i1)>>5;
				i0= 5*j0+11*j2;		i1= 5*j1+11*j3;
				pixt[2*16+0*4+j]=(16*i0+ 0*i1)>>5;
				pixt[2*16+1*4+j]=(11*i0+ 5*i1)>>5;
				pixt[2*16+2*4+j]=( 5*i0+11*i1)>>5;
				pixt[2*16+3*4+j]=( 0*i0+16*i1)>>5;
				i0= 0*j0+16*j2;		i1= 0*j1+16*j3;
				pixt[3*16+0*4+j]=(16*i0+ 0*i1)>>5;
				pixt[3*16+1*4+j]=(11*i0+ 5*i1)>>5;
				pixt[3*16+2*4+j]=( 5*i0+11*i1)>>5;
				pixt[3*16+3*4+j]=( 0*i0+16*i1)>>5;
			}
			break;
		default:
			break;
		}
		break;
	case 5:
		cs+=3;
		cs+=4;
		break;
	case 6:
		switch(i&31)
		{
		case 0:
			cs++;
//			i0=cs[0]; i1=cs[1];
//			i2=cs[2]; i3=cs[3];
//			cs+=4;
			
			for(j=0; j<4; j++)
				for(k=0; k<4; k++)
			{
				cpixt[j*16+12+k]=(bpixt[j* 4+k]<<1)-cs[j];
				cpixt[j*16+ 0+k]=cpixt[j*16+12+k]+cs[j];
				cpixt[j*16+ 4+k]=(11*cpixt[j*16+ 0+k]+
					 5*cpixt[j*16+12+k])>>4;
				cpixt[j*16+ 8+k]=( 5*cpixt[j*16+0+k]+
					11*cpixt[j*16+12+k])>>4;
			}
			cs+=4;
			for(j=0; j<4; j++)
			{
				i0=(cs[0]>>6)&3;	i1=(cs[1]>>4)&3;
				i2=(cs[2]>>2)&3;	i3=(cs[3]>>0)&3;
				pixt[0*16+0*4+j]=cpixt[0*16+i0*4+j];
				pixt[0*16+1*4+j]=cpixt[0*16+i1*4+j];
				pixt[0*16+2*4+j]=cpixt[1*16+i2*4+j];
				pixt[0*16+3*4+j]=cpixt[1*16+i3*4+j];
				i0=(cs[0]>>6)&3;	i1=(cs[1]>>4)&3;
				i2=(cs[2]>>2)&3;	i3=(cs[3]>>0)&3;
				pixt[1*16+0*4+j]=cpixt[0*16+i0*4+j];
				pixt[1*16+1*4+j]=cpixt[0*16+i1*4+j];
				pixt[1*16+2*4+j]=cpixt[1*16+i2*4+j];
				pixt[1*16+3*4+j]=cpixt[1*16+i3*4+j];
				i0=(cs[0]>>6)&3;	i1=(cs[1]>>4)&3;
				i2=(cs[2]>>2)&3;	i3=(cs[3]>>0)&3;
				pixt[2*16+0*4+j]=cpixt[2*16+i0*4+j];
				pixt[2*16+1*4+j]=cpixt[2*16+i1*4+j];
				pixt[2*16+2*4+j]=cpixt[3*16+i2*4+j];
				pixt[2*16+3*4+j]=cpixt[3*16+i3*4+j];
				i0=(cs[0]>>6)&3;	i1=(cs[1]>>4)&3;
				i2=(cs[2]>>2)&3;	i3=(cs[3]>>0)&3;
				pixt[3*16+0*4+j]=cpixt[2*16+i0*4+j];
				pixt[3*16+1*4+j]=cpixt[2*16+i1*4+j];
				pixt[3*16+2*4+j]=cpixt[3*16+i2*4+j];
				pixt[3*16+3*4+j]=cpixt[3*16+i3*4+j];
			}

			break;
		case 1:
			cs+=6;
			cs+=4;
			break;
		default:
			break;
		}

//		csm=bt1c_blkmap1;
		
		break;
	}
#endif

	BGBBTJ_BCn_EncodeBlockDXT1F(ct, pixt, 4, 16, 3);

	*rcs=cs;
}

void BGBBTJ_BT1C_DecodeBlockColorArrayDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte **rcs, int num,
	int xs1, int ys1, int stride, int mode)
{
	byte ctb[16];
	byte *cs;
	int i, j, k;
	
	cs=*rcs;
	for(i=0; i<num; i++)
	{
		j=BGBBTJ_BT1C_DecodeSingleColor565(ctx, &cs, mode);
		bgbbtj_bt1c_setule16(ctb+0, j);
		bgbbtj_bt1c_setule16(ctb+2, j);
		bgbbtj_bt1c_setule32(ctb+4, 0x00000000);
		BGBBTJ_BT1C_FillColorDeblock(ctx,
			blks, ct, ctb, 0, xs1, ys1, stride,
			ctx->clrdb_mode);
		ct+=stride;
	}
	*rcs=cs;
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXTn(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *ibuf, int isz,
	byte *blks, byte *lblks,
	int xs, int ys, int stride, int dfl)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl;
	byte *ct, *cte, *ctb;
	int diffm, diffsc;
	int xs1, ys1, n, n1, ret, mode, nextmode;
	int xo, yo;
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
//	cs+=4;

	if(ibuf[0]==0xE1)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[3];
		cs=ibuf+4; cse=ibuf+l;
	}

	dfl|=ctx->dflags&BGBBTJ_BTIC1C_DFL_FRAMESTICKY;
	ctx->dflags=dfl;

	diffm=0; diffsc=0;
	ctx->diff_m=0; ctx->diff_sc=0;

//	ret=0; mode=0; nextmode=0;
	ret=0; mode=ctx->modes[0]; nextmode=mode;
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
			break;			
		case 0xE0:
			switch(i)
			{
			case 0xE0:
				if(mode!=ctx->modes[0])
					{ mode=ctx->modes[0]; nextmode=mode; }
				else
					{ ret=1; }
//				if(mode)
//					{ mode=0; nextmode=0; }
//				else
//					{ ret=1; }
				break;
			case 0xE1:
				mode=ctx->modes[1];
				nextmode=mode;
//				cs+=3;
				break;
			case 0xE2:
				mode=ctx->modes[2];
				nextmode=mode;
//				if((cs[0]>='A') && (cs[0]<='Z'))
//					{ ret=-1; }
//				j=(cs[1]<<8)|cs[2];
//				cs+=j-1;
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

#if 1
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
#endif

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

//				if((ct+j*stride)>cte) { ret=-1; break; }
//				if((ct-k*stride)<blks) { ret=-1; break; }
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
				case 0xF0: cs++; mode=ctx->modes[0]; nextmode=mode; break;
				case 0xF1: cs++; mode=ctx->modes[1]; nextmode=mode; break;
				case 0xF2: cs++; mode=ctx->modes[2]; nextmode=mode; break;
				case 0xF3: cs++; mode=ctx->modes[3]; nextmode=mode; break;
				case 0xF4: cs++; mode=ctx->modes[4]; nextmode=mode; break;
				case 0xF5: cs++; mode=ctx->modes[5]; nextmode=mode; break;
				case 0xF6: cs++; mode=ctx->modes[6]; nextmode=mode; break;
				case 0xF7: cs++; mode=ctx->modes[7]; nextmode=mode; break;
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
					nextmode=mode; mode=ctx->modes[0]; break;
				default: nextmode=mode; mode=ctx->modes[0]; break;
				}
				break;
			case 0xF1: nextmode=mode; mode=ctx->modes[1]; break;
			case 0xF2: nextmode=mode; mode=ctx->modes[2]; break;
			case 0xF3: nextmode=mode; mode=ctx->modes[3]; break;
			case 0xF4: nextmode=mode; mode=ctx->modes[4]; break;
			case 0xF5: nextmode=mode; mode=ctx->modes[5]; break;
			case 0xF6: nextmode=mode; mode=ctx->modes[6]; break;
			case 0xF7: nextmode=mode; mode=ctx->modes[7]; break;
			
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
					ctx->modes[(j>>12)&7]=j&4095;
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
			default:
				break;
			}
			break;

		default:
			switch(mode)
			{
			case 0:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=k+j;
						l=0;
					}else
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;
						l=j;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					if(l&0x8000)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, j);
						bgbbtj_bt1c_setule32(ctb+4, 0xFFFFFFFFU);
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, j);
						bgbbtj_bt1c_setule32(ctb+4, 0x00000000);
					}

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
			
					break;
				case 0xC0:
					if(diffm && (cs[0]&0x80))
					{
						j=*cs++;
						k=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=bt1c_difftab[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;

						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;

						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;

						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
							ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
							cs+=4;
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
						break;
					}
					
					j=(cs[0]<<8)|cs[1];
					k=(cs[2]<<8)|cs[3];
					cs+=4;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					l=k;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					if(l&0x8000)
					{
						if(j<=k)
						{
							bgbbtj_bt1c_setule16(ctb+0, j);
							bgbbtj_bt1c_setule16(ctb+2, k);
							csm=bt1c_blkmap1;
						}else
						{
							bgbbtj_bt1c_setule16(ctb+0, k);
							bgbbtj_bt1c_setule16(ctb+2, j);
							csm=bt1c_blkmap2;
						}
					}else
					{
						if(j>k)
						{
							bgbbtj_bt1c_setule16(ctb+0, j);
							bgbbtj_bt1c_setule16(ctb+2, k);
							csm=bt1c_blkmap1;
						}else
						{
							bgbbtj_bt1c_setule16(ctb+0, k);
							bgbbtj_bt1c_setule16(ctb+2, j);
							csm=bt1c_blkmap2;
						}
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					if((ctx->optcntrl[1]>0) && !(cs[1]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockDXTn(ctx,
							blks, ct, ct-8, &cs, 1,
							xs1, ys1, stride, mode, dfl);
						ct+=stride; csl+=stride;
						break;
					}

					if(cs[1]&0x80)
					{
						cs--;
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;

						if(j>k)
						{
							bgbbtj_bt1c_setule16(ctb+0, j);
							bgbbtj_bt1c_setule16(ctb+2, k);
							csm=bt1c_blkmap1;
						}else
						{
							bgbbtj_bt1c_setule16(ctb+0, k);
							bgbbtj_bt1c_setule16(ctb+2, j);
							csm=bt1c_blkmap2;
						}

						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else
					{
						memset(ctb, 0, 8);
						//dummy...
						cs+=31;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				}
				break;
			case 1:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

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
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=bt1c_blkidxmap1[cs[0]];
					k=bt1c_blkidxmap1[cs[1]];
					cs+=2;

					ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
					ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;

					break;
				}
				break;
			case 2:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[*cs++];
					k=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

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
					break;
				default:
					cs--;
					j=ctx->pal256[*cs++];
					k=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=bt1c_blkidxmap2[*cs++];
					ctb[4]=csm[(j>>24)&255];
					ctb[5]=csm[(j>>16)&255];
					ctb[6]=csm[(j>> 8)&255];
					ctb[7]=csm[j&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
			case 3:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=ctx->pat256[*cs++];
					ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
					ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;

					break;
				}
				break;

#if 1
			case 4:
			case 5:
			case 14:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=k+j;

						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}
					
					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, j);
					bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=*cs++; k=*cs++;
						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=bt1c_difftab[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
						k=(cs[0]<<8)|cs[1]; cs+=2;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, k);
					csm=bt1c_blkmap1;

					if(mode==4)
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
					}else if(mode==14)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=ctx->pat256[*cs++];
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
					if((ctx->optcntrl[1]>0) && !(cs[1]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockDXTn(ctx,
							blks, ct, ct-8, &cs, 1,
							xs1, ys1, stride, mode, dfl);
						ct+=stride; csl+=stride;
						break;
					}

					cs--;
					j=(cs[0]<<8)|cs[1]; cs+=2;
					k=(cs[0]<<8)|cs[1]; cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, k);
					csm=bt1c_blkmap1;

					if(mode==4)
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
						ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else if(mode==14)
					{
						j=ctx->pat256[*cs++];
						ctb[4]=csm[(j>>24)&255];
						ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255];
						ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				}
				break;
#endif

#if 1
			case 6:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;

//						j=*cs++;
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
//							blks, ct, xs1, ys1, stride);
//						j=k+j;
					
						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 16, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}

					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, j);
					bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;

						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 16, j);
						k=BGBBTJ_BT1C_CalcDiff15ColorB24(ctx,
							blks, ct, xs1, ys1, 16, k);
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, k);

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockDXTn(ctx,
							blks, ct, ct-8, &cs, 1,
							xs1, ys1, stride, mode, dfl);
						ct+=stride; csl+=stride;
						break;
					}

					cs--;
					j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
					k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
					cs+=6;
//					j=BGBBTJ_BT1C_ConvRGB23ToRGB16(ctx, j);
//					k=BGBBTJ_BT1C_ConvRGB23ToRGB16(ctx, k);

					j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, k);

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
					ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
					cs+=4;
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
#endif

#if 1
			case 7:
			case 8:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;

//						j=*cs++;
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=k+j;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=j;
						if(mode==8)
							{ j=BGBBTJ_BT1C_ConvRGB31FUToRGB15(ctx, j); }
						else
							{ j=BGBBTJ_BT1C_ConvRGB31FSToRGB15(ctx, j); }
						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, j);
					bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						k=bt1c_difftab[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA(ctx,
							blks, ct, xs1, ys1, stride);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, stride);
						k=l+k;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						k=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
						cs+=8;
						l=k;
						if(mode==8)
						{
							j=BGBBTJ_BT1C_ConvRGB31FUToRGB15(ctx, j);
							k=BGBBTJ_BT1C_ConvRGB31FUToRGB15(ctx, k);
						}
						else
						{
							j=BGBBTJ_BT1C_ConvRGB31FSToRGB15(ctx, j);
							k=BGBBTJ_BT1C_ConvRGB31FSToRGB15(ctx, k);
						}
						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;
					}

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
						ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
						cs+=4;
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;

				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockDXTn(ctx,
							blks, ct, ct-8, &cs, 1,
							xs1, ys1, stride, mode, dfl);
						ct+=stride; csl+=stride;
						break;
					}

					cs--;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					cs+=8;
					if(mode==8)
					{
						j=BGBBTJ_BT1C_ConvRGB31FUToRGB15(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB31FUToRGB15(ctx, k);
					}
					else
					{
						j=BGBBTJ_BT1C_ConvRGB31FSToRGB15(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB31FSToRGB15(ctx, k);
					}
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					if(j>k)
					{
						bgbbtj_bt1c_setule16(ctb+0, j);
						bgbbtj_bt1c_setule16(ctb+2, k);
						csm=bt1c_blkmap1;
					}else
					{
						bgbbtj_bt1c_setule16(ctb+0, k);
						bgbbtj_bt1c_setule16(ctb+2, j);
						csm=bt1c_blkmap2;
					}

					ctb[4]=csm[cs[0]]; ctb[5]=csm[cs[1]];
					ctb[6]=csm[cs[2]]; ctb[7]=csm[cs[3]];
					cs+=4;
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
#endif

#if 1
			case 10:
			case 11:
			case 12:
				switch(i&0xE0)
				{
				case 0xA0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;
						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 16, j);
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, j);
					bgbbtj_bt1c_setule32(ctb+4, 0x00000000);

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 16, j);
						k=BGBBTJ_BT1C_CalcDiff15ColorB24(ctx,
							blks, ct, xs1, ys1, 16, k);
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, k);

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, k);
					csm=bt1c_blkmap1;

					if(mode==10)
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
					}else if(mode==11)
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
					}else if(mode==12)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=ctx->pat256[*cs++];
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
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockDXTn(ctx,
							blks, ct, ct-8, &cs, 1,
							xs1, ys1, stride, mode, dfl);
						ct+=stride; csl+=stride;
						break;
					}

					cs--;
					j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
					k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
					cs+=6;
					j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, k);

					bgbbtj_bt1c_setule16(ctb+0, j);
					bgbbtj_bt1c_setule16(ctb+2, k);
					csm=bt1c_blkmap1;

					if(mode==10)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						ctb[4]=csm[(j>>8)&255]; ctb[5]=csm[j&255];
						ctb[6]=csm[(k>>8)&255]; ctb[7]=csm[k&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else if(mode==11)
					{
						j=bt1c_blkidxmap2[*cs++];
						ctb[4]=csm[(j>>24)&255];
						ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255];
						ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}else if(mode==12)
					{
						j=ctx->pat256[*cs++];
						ctb[4]=csm[(j>>24)&255];
						ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255];
						ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				}
				break;
#endif

#if 1
			case 13:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=*cs++;
						j=bt1c_difftab24[(diffsc*256)+(j&127)];
						k=BGBBTJ_BT1C_PredictDiffColorA24(ctx,
							blks, ct, xs1, ys1, 16);
						j=k+j;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						cs+=3;
						l=j;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=cs[0]; k=cs[1]; cs+=2;
						j=bt1c_difftab24[(diffsc*256)+(j&127)];
						k=bt1c_difftab24[(diffsc*256)+(k&127)];
						l=BGBBTJ_BT1C_PredictDiffColorA24(ctx,
							blks, ct, xs1, ys1, 16);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB24(ctx,
							blks, ct, xs1, ys1, 16);
						k=l+k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, k);

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

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
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockDXTn(ctx,
							blks, ct, ct-8, &cs, 1,
							xs1, ys1, stride, mode, dfl);
						ct+=stride; csl+=stride;
						break;
					}

					cs--;
					j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
					k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
					cs+=6;
					j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					j=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, j);
					k=BGBBTJ_BT1C_ConvRGB24ToRGB16(ctx, k);

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=bt1c_blkidxmap2[*cs++];
					ctb[4]=csm[(j>>24)&255];
					ctb[5]=csm[(j>>16)&255];
					ctb[6]=csm[(j>> 8)&255];
					ctb[7]=csm[j&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;
					break;
				}
				break;
#endif

#if 1
			case 15:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal16[(*cs++)&15];
					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=j&0xFF;	ctb[3]=(j>>8)&0xFF;
					ctb[4]=0; ctb[5]=0;
					ctb[6]=0; ctb[7]=0;

					if(ctx->clrdb_mode)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							BGBBTJ_BT1C_FillColorDeblock(ctx,
								blks, ct, ctb, i, xs1, ys1, stride,
								ctx->clrdb_mode);
//							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}else
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, ctb);
							ct+=stride; csl+=stride;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
						ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
						bgbbtj_bt1c_memcpy8(ct, ctb);
						ct+=stride; csl+=stride;
					}
					break;
				default:
					cs--;
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctb[0]=j&0xFF;	ctb[1]=(j>>8)&0xFF;
					ctb[2]=k&0xFF;	ctb[3]=(k>>8)&0xFF;
					csm=bt1c_blkmap1;

					j=ctx->pat256[*cs++];
					ctb[4]=csm[(j>>24)&255]; ctb[5]=csm[(j>>16)&255];
					ctb[6]=csm[(j>> 8)&255]; ctb[7]=csm[(j    )&255];
					bgbbtj_bt1c_memcpy8(ct, ctb);
					ct+=stride; csl+=stride;

					break;
				}
				break;
#endif

			default:
				break;
			}
			mode=nextmode;
			break;
		}
	}
	
	if(ret)return(ret);
	return(cs-ibuf);
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT1(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys)
{
	byte *tbuf, *tbuf2;
	int i, j, k, sz1;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
		{
			BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks, lblks, xs, ys, 8, 0);
			j=(ibuf[i+1]<<16)|(ibuf[i+2]<<8)|(ibuf[i+3]<<0);
			k=i+j;
		}else
		{
			k=BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks, lblks, xs, ys, 8, 0);
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);
//		BGBBTJ_BT1C_Lock();
//		PDUNZ_DecodeStreamSz(ibuf+8, tbuf, i-8, j, &k, 0);
//		PDUNZ_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
		BTLZA_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
//		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT1(ctx, tbuf, k, blks, lblks, xs, ys);
		free(tbuf);
		k=i;

//		cs=ibuf+i;
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='A'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);		tbuf2=malloc(j);
		BGBBTJ_BT1C_Lock();
		sz1=BGBBTJ_BitArith_DecodeData(ibuf+6, i-6, tbuf2, j);
//		PDUNZ_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BTLZA_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT1(ctx, tbuf, k, blks, lblks, xs, ys);
		free(tbuf);			 free(tbuf2);
		k=i;
//		cs=ibuf+i;
	}else
	{
		return(-1);
	}
	return(k);
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksDXT5(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys)
{
	byte *cs, *csl;
	byte *tbuf, *tbuf2;
	int xs1, ys1, xs2, ys2;
	int i, j, k, n, sz1;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
//		if(0)
		{
			BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks+8, lblks?(lblks+8):NULL, xs, ys, 16, 0);
			j=BGBBTJ_BT1C_DecodeBlocksBC4(ctx, ibuf+i, isz-i,
				blks, lblks, xs, ys, 16);
			cs=ibuf+i+j;
		}else
		{
//			i=BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
//				blks+n*8, lblks, xs, ys, 8);
			BGBBTJ_BT1C_DecodeBlocksDXTn(ctx, ibuf, isz,
				blks+8, lblks?(lblks+8):NULL, xs, ys, 16, 0);
//			BGBBTJ_BCn_TranscodeBC1ToBC3(blks+n*8, blks, xs, ys);
//			BGBBTJ_BCn_TranscodeBC1ToBC3B(blks, blks, xs, ys);
			BGBBTJ_BCn_TranscodeBC1ToBC3B_Opaque(blks, blks, xs, ys);

//			i=BGBBTJ_BT1C_DecodeBlocksDXT5_NoAX(ctx, ibuf, isz, blks, xs, ys);
			cs=ibuf+i;
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		BGBBTJ_BT1C_CheckSizeTBuf1(ctx, j);
		tbuf=ctx->tbuf1;
		
//		tbuf=malloc(j);
//		BGBBTJ_BT1C_Lock();
//		PDUNZ_DecodeStreamSz(ibuf+8, tbuf, i-8, j, &k, 0);
//		PDUNZ_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
		BTLZA_DecodeStreamSzZl(ibuf+6, tbuf, i-6, j, &k, 0);
//		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT5(ctx, tbuf, k, blks, lblks, xs, ys);
//		free(tbuf);
		k=i;

		cs=ibuf+i;
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='A'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;

		BGBBTJ_BT1C_CheckSizeTBuf1(ctx, j);
		BGBBTJ_BT1C_CheckSizeTBuf2(ctx, j);
		tbuf=ctx->tbuf1;
		tbuf2=ctx->tbuf2;
		
//		tbuf=malloc(j);		tbuf2=malloc(j);
		BGBBTJ_BT1C_Lock();
		sz1=BGBBTJ_BitArith_DecodeData(ibuf+6, i-6, tbuf2, j);
//		PDUNZ_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BTLZA_DecodeStreamSzZl(tbuf2, tbuf, sz1, j, &k, 0);
		BGBBTJ_BT1C_Unlock();
		BGBBTJ_BT1C_DecodeBlocksDXT5(ctx, tbuf, k, blks, lblks, xs, ys);
//		free(tbuf);			 free(tbuf2);
		k=i;
		cs=ibuf+i;
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='J') && (ibuf[5]=='P'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		if(!ctx->jpg_ctx)
			ctx->jpg_ctx=BGBBTJ_JPG_AllocContext();
		BGBBTJ_JPG_DecodeBufferClrsCtx(
			ctx->jpg_ctx, ibuf+6, i-6,
			blks, BGBBTJ_JPG_BC3_VF, &xs2, &ys2);
//		BGBBTJ_JPG_DecodeBufferClrs(ibuf+6, i-6,
//			blks, BGBBTJ_JPG_BC3_VF, &xs2, &ys2);
		k=i;
		cs=ibuf+i;
	}else
	{
		return(-1);
	}
	return(cs-ibuf);
}
