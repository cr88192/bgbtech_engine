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

extern byte bt1c_hf12tob[4096];		//half-float (bits 4-15) to byte

extern int bt1c_basedifftab[8][5];
extern u32 bt1c_blkidx_pixpat[256];	//1BPP -> RPZA Pixels


void BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAX(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	BGBBTJ_BitStream bits;
	byte *csm;
	u32 apix;
	int cra, cga, cba, caa, crb, cgb, cbb, cab;
	int p0, p1, p2, p3;
	int i, j, k;

	if(ct[0]>ct[1])
	{
		csm=bt1c_blkalphamape2;
		j=ct[2]|(ct[3]<<8)|(ct[4]<<16);
		k=ct[5]|(ct[6]<<8)|(ct[7]<<16);
		p0=csm[j&0xFFF]; p1=csm[(j>>12)&0xFFF];
		p2=csm[k&0xFFF]; p3=csm[(k>>12)&0xFFF];
		apix=(p0<<24)|(p1<<16)|(p2<<8)|p3;
	}
	else
	{
		csm=bt1c_blkalphamape1;
		j=ct[2]|(ct[3]<<8)|(ct[4]<<16);
		k=ct[5]|(ct[6]<<8)|(ct[7]<<16);
		p0=csm[j&0xFFF]; p1=csm[(j>>12)&0xFFF];
		p2=csm[k&0xFFF]; p3=csm[(k>>12)&0xFFF];
		apix=(p0<<24)|(p1<<16)|(p2<<8)|p3;
	}

	if(dfl&BGBBTJ_BTIC1C_DFL_FLIPBLOCKS)
	{
		pix=(pix>>24)|(pix<<24)|
			((pix>>8)&0x0000FF00)|
			((pix<<8)&0x00FF0000);

		apix=(apix>>24)|(apix<<24)|
			((apix>>8)&0x0000FF00)|
			((apix<<8)&0x00FF0000);
	}

	if((apix>>24)&0x80)
	{
		caa=ct[0];
		cab=ct[1];
		apix=~apix;
	}else
	{
		caa=ct[1];
		cab=ct[0];
	}

	if((pix>>24)&0x80)
	{
		cra=(clra>>11)&31;
		cga=(clra>>5)&63;
		cba=(clra)&31;
		crb=(clrb>>11)&31;
		cgb=(clrb>>5)&63;
		cbb=(clrb)&31;
		pix=~pix;
	}else
	{
		cra=(clrb>>11)&31;
		cga=(clrb>>5)&63;
		cba=(clrb)&31;
		crb=(clra>>11)&31;
		cgb=(clra>>5)&63;
		cbb=(clra)&31;
	}

	cra=(cra<<3)|(cra>>2);
	cga=(cga<<2)|(cga>>4);
	cba=(cba<<3)|(cba>>2);
	crb=(crb<<3)|(crb>>2);
	cgb=(cgb<<2)|(cgb>>4);
	cbb=(cbb<<3)|(cbb>>2);
	
	BGBBTJ_BitsLE_ClearSetupWrite(&bits, ct, 16);
	BGBBTJ_BitsLE_Write6Bits(&bits, 32);
	BGBBTJ_BitsLE_Write2Bits(&bits, 0);
	
	BGBBTJ_BitsLE_Write7Bits(&bits, (cra)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (crb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cga)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cgb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cba)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cbb)>>1);
	BGBBTJ_BitsLE_Write8Bits(&bits, caa);
	BGBBTJ_BitsLE_Write8Bits(&bits, cab);

	i=(pix>>24)&255;
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
//	BGBBTJ_BitsLE_WriteBit(&bits, (i>>6)&1);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);

	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
//	BGBBTJ_BitsLE_WriteBit(&bits, (i>>6)&1);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	BGBBTJ_BitStream bits;
	int cra, cga, cba, caa, crb, cgb, cbb, cab;
	int p0, p1, p2, p3;
	int i, j, k;

	if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
	{
		if((ct[0]!=255) || (ct[1]!=255))
		{
			BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAX(
				ct, clra, clrb, pix, dfl);
			return;
		}
	}

	if(dfl&BGBBTJ_BTIC1C_DFL_FLIPBLOCKS)
	{
		pix=(pix>>24)|(pix<<24)|
			((pix>>8)&0x0000FF00)|
			((pix<<8)&0x00FF0000);
	}

	if((pix>>24)&0x80)
	{
		cra=(clra>>11)&31;
		cga=(clra>>5)&63;
		cba=(clra)&31;
		crb=(clrb>>11)&31;
		cgb=(clrb>>5)&63;
		cbb=(clrb)&31;
		pix=~pix;
	}else
	{
		cra=(clrb>>11)&31;
		cga=(clrb>>5)&63;
		cba=(clrb)&31;
		crb=(clra>>11)&31;
		cgb=(clra>>5)&63;
		cbb=(clra)&31;
	}

	cra=(cra<<3)|(cra>>2);
	cga=(cga<<2)|(cga>>4);
	cba=(cba<<3)|(cba>>2);
	crb=(crb<<3)|(crb>>2);
	cgb=(cgb<<2)|(cgb>>4);
	cbb=(cbb<<3)|(cbb>>2);

	caa=255;
	cab=255;
	
	BGBBTJ_BitsLE_ClearSetupWrite(&bits, ct, 16);
	BGBBTJ_BitsLE_Write6Bits(&bits, 32);
	BGBBTJ_BitsLE_Write2Bits(&bits, 0);
	
	BGBBTJ_BitsLE_Write7Bits(&bits, (cra)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (crb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cga)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cgb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cba)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cbb)>>1);
	BGBBTJ_BitsLE_Write8Bits(&bits, caa);
	BGBBTJ_BitsLE_Write8Bits(&bits, cab);

	i=(pix>>24)&255;
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
//	BGBBTJ_BitsLE_WriteBit(&bits, (i>>6)&1);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);

	BGBBTJ_BitsLE_Write7Bits(&bits, 0);
	BGBBTJ_BitsLE_Write8Bits(&bits, 0);
	BGBBTJ_BitsLE_Write8Bits(&bits, 0);
	BGBBTJ_BitsLE_Write8Bits(&bits, 0);

#if 0
	i=0;
	BGBBTJ_BitsLE_WriteBit(&bits, (i>>6)&1);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=0;
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=0;
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=0;
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
#endif

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAlpha(
	byte *ct, u16 clra, u16 clrb, u32 pix, int dfl)
{
	BGBBTJ_BitStream bits;
	int cra, cga, cba, caa, crb, cgb, cbb, cab;
	int apix;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i, j, k;

	if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
	{
		if((ct[0]!=255) || (ct[1]!=255))
		{
			BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAX(
				ct, clra, clrb, pix, dfl);
			return;
		}
	}

	if(dfl&BGBBTJ_BTIC1C_DFL_FLIPBLOCKS)
	{
		pix=(pix>>24)|(pix<<24)|
			((pix>>8)&0x0000FF00)|
			((pix<<8)&0x00FF0000);
	}

	i=(pix>>24)&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p4=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	i=(pix>>16)&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p5=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	i=(pix>> 8)&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p6=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	i=(pix    )&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p7=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	apix=(p4<<24)|(p5<<16)|(p6<< 8)|p7;


	if(!((pix>>24)&2))
	{
		cra=(clrb>>11)&31;
		cga=(clrb>>5)&63;
		cba=(clrb)&31;
		crb=(clra>>11)&31;
		cgb=(clra>>5)&63;
		cbb=(clra)&31;
	}else
	{
		cra=(clra>>11)&31;
		cga=(clra>>5)&63;
		cba=(clra)&31;
		crb=(clrb>>11)&31;
		cgb=(clrb>>5)&63;
		cbb=(clrb)&31;
		pix=~pix;
	}

	cra=(cra<<3)|(cra>>2);
	cga=(cga<<2)|(cga>>4);
	cba=(cba<<3)|(cba>>2);
	crb=(crb<<3)|(crb>>2);
	cgb=(cgb<<2)|(cgb>>4);
	cbb=(cbb<<3)|(cbb>>2);

	caa=255;
	cab=255;
	
	BGBBTJ_BitsLE_ClearSetupWrite(&bits, ct, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 32, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, 0, 2);
	
	BGBBTJ_BitsLE_WriteBits(&bits, (cra)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (crb)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cga)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cgb)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cba)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cbb)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, caa, 8);
	BGBBTJ_BitsLE_WriteBits(&bits, cab, 8);

	i=(pix>>24)&255;
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
//	BGBBTJ_BitsLE_WriteBit(&bits, (i>>6)&1);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(pix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);

	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x0C)|
		((i<<1)&0x30)|((i<<5)&0xC0));
//	BGBBTJ_BitsLE_WriteBit(&bits, (i>>6)&1);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(apix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(apix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);
	i=(apix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>6)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>4)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i>>2)&3);
//	BGBBTJ_BitsLE_Write2Bits(&bits, (i   )&3);

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAX_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	BGBBTJ_BitStream bits;
	byte *csm;
	u32 apix;
	int cra, cga, cba, caa, crb, cgb, cbb, cab;
	int p0, p1, p2, p3;
	int i, j, k;

	if(ct[0]>ct[1])
	{
		csm=bt1c_blkalphamape2;
		j=ct[2]|(ct[3]<<8)|(ct[4]<<16);
		k=ct[5]|(ct[6]<<8)|(ct[7]<<16);
		p0=csm[j&0xFFF]; p1=csm[(j>>12)&0xFFF];
		p2=csm[k&0xFFF]; p3=csm[(k>>12)&0xFFF];
		apix=(p0<<24)|(p1<<16)|(p2<<8)|p3;
	}
	else
	{
		csm=bt1c_blkalphamape1;
		j=ct[2]|(ct[3]<<8)|(ct[4]<<16);
		k=ct[5]|(ct[6]<<8)|(ct[7]<<16);
		p0=csm[j&0xFFF]; p1=csm[(j>>12)&0xFFF];
		p2=csm[k&0xFFF]; p3=csm[(k>>12)&0xFFF];
		apix=(p0<<24)|(p1<<16)|(p2<<8)|p3;
	}

	if(dfl&BGBBTJ_BTIC1C_DFL_FLIPBLOCKS)
	{
		pix=(pix>>24)|(pix<<24)|
			((pix>>8)&0x0000FF00)|
			((pix<<8)&0x00FF0000);

		apix=(apix>>24)|(apix<<24)|
			((apix>>8)&0x0000FF00)|
			((apix<<8)&0x00FF0000);
	}

	if((apix>>24)&0x80)
	{
		caa=ct[0];
		cab=ct[1];
		apix=~apix;
	}else
	{
		caa=ct[1];
		cab=ct[0];
	}

	if((pix>>24)&0x80)
	{
		cra=(clra>>16)&255;
		cga=(clra>> 8)&255;
		cba=(clra    )&255;
		crb=(clrb>>16)&255;
		cgb=(clrb>> 8)&255;
		cbb=(clrb    )&255;
		pix=~pix;
	}else
	{
		cra=(clrb>>16)&255;
		cga=(clrb>> 8)&255;
		cba=(clrb    )&255;
		crb=(clra>>16)&255;
		cgb=(clra>> 8)&255;
		cbb=(clra    )&255;
	}
	
	BGBBTJ_BitsLE_ClearSetupWrite(&bits, ct, 16);
	BGBBTJ_BitsLE_Write6Bits(&bits, 32);
	BGBBTJ_BitsLE_Write2Bits(&bits, 0);
	
	BGBBTJ_BitsLE_Write7Bits(&bits, (cra)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (crb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cga)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cgb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cba)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cbb)>>1);
	BGBBTJ_BitsLE_Write8Bits(&bits, caa);
	BGBBTJ_BitsLE_Write8Bits(&bits, cab);

	i=(pix>>24)&255;
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
	i=(pix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(pix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(pix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));

	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	BGBBTJ_BitStream bits;
	int cra, cga, cba, caa, crb, cgb, cbb, cab;
	int p0, p1, p2, p3;
	int i, j, k;

	if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
	{
		if((ct[0]!=255) || (ct[1]!=255))
		{
			BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAX_RGB24(
				ct, clra, clrb, pix, dfl);
			return;
		}
	}

	if(dfl&BGBBTJ_BTIC1C_DFL_FLIPBLOCKS)
	{
		pix=(pix>>24)|(pix<<24)|
			((pix>>8)&0x0000FF00)|
			((pix<<8)&0x00FF0000);
	}

	if((pix>>24)&0x80)
	{
		cra=(clra>>16)&255;
		cga=(clra>> 8)&255;
		cba=(clra    )&255;
		crb=(clrb>>16)&255;
		cgb=(clrb>> 8)&255;
		cbb=(clrb    )&255;
		pix=~pix;
	}else
	{
		cra=(clrb>>16)&255;
		cga=(clrb>> 8)&255;
		cba=(clrb    )&255;
		crb=(clra>>16)&255;
		cgb=(clra>> 8)&255;
		cbb=(clra    )&255;
	}

	caa=255;
	cab=255;
	
	BGBBTJ_BitsLE_ClearSetupWrite(&bits, ct, 16);
	BGBBTJ_BitsLE_Write6Bits(&bits, 32);
	BGBBTJ_BitsLE_Write2Bits(&bits, 0);
	
	BGBBTJ_BitsLE_Write7Bits(&bits, (cra)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (crb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cga)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cgb)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cba)>>1);
	BGBBTJ_BitsLE_Write7Bits(&bits, (cbb)>>1);
	BGBBTJ_BitsLE_Write8Bits(&bits, caa);
	BGBBTJ_BitsLE_Write8Bits(&bits, cab);

	i=(pix>>24)&255;
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
	i=(pix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(pix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(pix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));

	BGBBTJ_BitsLE_Write7Bits(&bits, 0);
	BGBBTJ_BitsLE_Write8Bits(&bits, 0);
	BGBBTJ_BitsLE_Write8Bits(&bits, 0);
	BGBBTJ_BitsLE_Write8Bits(&bits, 0);

	BGBBTJ_BitsLE_FlushBits(&bits);
}

void BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAlpha_RGB24(
	byte *ct, u32 clra, u32 clrb, u32 pix, int dfl)
{
	BGBBTJ_BitStream bits;
	int cra, cga, cba, caa, crb, cgb, cbb, cab;
	int apix;
	int p0, p1, p2, p3, p4, p5, p6, p7;
	int i, j, k;

	if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
	{
		if((ct[0]!=255) || (ct[1]!=255))
		{
			BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAX_RGB24(
				ct, clra, clrb, pix, dfl);
			return;
		}
	}

	if(dfl&BGBBTJ_BTIC1C_DFL_FLIPBLOCKS)
	{
		pix=(pix>>24)|(pix<<24)|
			((pix>>8)&0x0000FF00)|
			((pix<<8)&0x00FF0000);
	}

	i=(pix>>24)&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p4=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	i=(pix>>16)&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p5=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	i=(pix>> 8)&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p6=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	i=(pix    )&255;
	p0=(((i>>6)&3)==2)?0:3;	p1=(((i>>4)&3)==2)?0:3;
	p2=(((i>>2)&3)==2)?0:3;	p3=(((i   )&3)==2)?0:3;
	p7=(p0<<6)|(p1<<4)|(p2<<2)|p3;

	apix=(p4<<24)|(p5<<16)|(p6<< 8)|p7;

	if((pix>>24)&0x80)
	{
		cra=(clra>>16)&255;
		cga=(clra>> 8)&255;
		cba=(clra    )&255;
		crb=(clrb>>16)&255;
		cgb=(clrb>> 8)&255;
		cbb=(clrb    )&255;
		pix=~pix;
	}else
	{
		cra=(clrb>>16)&255;
		cga=(clrb>> 8)&255;
		cba=(clrb    )&255;
		crb=(clra>>16)&255;
		cgb=(clra>> 8)&255;
		cbb=(clra    )&255;
	}

	caa=255;
	cab=255;
	
	BGBBTJ_BitsLE_ClearSetupWrite(&bits, ct, 16);
	BGBBTJ_BitsLE_WriteBits(&bits, 32, 6);
	BGBBTJ_BitsLE_WriteBits(&bits, 0, 2);
	
	BGBBTJ_BitsLE_WriteBits(&bits, (cra)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (crb)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cga)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cgb)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cba)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, (cbb)>>1, 7);
	BGBBTJ_BitsLE_WriteBits(&bits, caa, 8);
	BGBBTJ_BitsLE_WriteBits(&bits, cab, 8);

	i=(pix>>24)&255;
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x06)|
		((i<<1)&0x18)|((i<<5)&0x60));
//	BGBBTJ_BitsLE_Write7Bits(&bits,
//		((i>>6)&0x01)|((i>>3)&0x0C)|
//		((i<<1)&0x30)|((i<<5)&0xC0));
	i=(pix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(pix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(pix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));

	i=(apix>>24)&255;
	BGBBTJ_BitsLE_Write7Bits(&bits,
		((i>>6)&0x01)|((i>>3)&0x0C)|
		((i<<1)&0x30)|((i<<5)&0xC0));
	i=(apix>>16)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(apix>>8)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));
	i=(apix)&255;
	BGBBTJ_BitsLE_Write8Bits(&bits,
		((i>>6)&0x03)|((i>>2)&0x0C)|
		((i<<2)&0x30)|((i<<6)&0xC0));

	BGBBTJ_BitsLE_FlushBits(&bits);
}


void BGBBTJ_BT1C_FillColorDeblockBC7(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, u16 clr, int idx,
	int xs1, int ys1, int stride, int mode, int dfl)
{
	BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(ct, clr, clr, 0, dfl);
}

void BGBBTJ_BT1C_DecodeBlockColorArrayBC7(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte *csa, byte **rcs, int num,
	int xs1, int ys1, int stride, int mode, int dfl)
{
	byte *cs;
	int i, j, k;
	
	cs=*rcs;
	for(i=0; i<num; i++)
	{
		j=BGBBTJ_BT1C_DecodeSingleColor565(ctx, &cs,
			mode, BGBBTJ_BTIC1C_EFL_DIFFCLR);
		BGBBTJ_BT1C_FillColorDeblockBC7(ctx,
			blks, ct, j, 0, xs1, ys1, stride,
			ctx->clrdb_mode, dfl);
		ct+=stride;
	}
	*rcs=cs;
}

void BGBBTJ_BT1C_DecodeRawBlockBC7(
	BGBBTJ_BTIC1C_ImageInfo *ctx, 
	byte *blks, byte *ct, byte *csa, byte **rcs, int num,
	int xs1, int ys1, int stride, int mode, int dfl)
{
	u32 aclr[16], bclr[4], cclr[8];
	byte pixt[16*4], bpixt[4*4], cpixt[16*4];
	byte *cs, *csm;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int blt;
	int i, j, k;
	
	cs=*rcs;
	memset(ct, 0, 16);
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
		for(i=0; i<16; i++)
		{
			pixt[i*4+0]=(aclr[i]>>16)&255;
			pixt[i*4+1]=(aclr[i]>>8)&255;
			pixt[i*4+2]=(aclr[i]>>0)&255;
			pixt[i*4+3]=(aclr[i]>>24)&255;
		}
		if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
		{
			BGBBTJ_BCn_DecodeBlockBC4(ct, pixt+3, 4, 16);
		}
//		BGBBTJ_BC7_EncodeBlock(ct, pixt, 4, 16);
		BGBBTJ_BC7_EncodeBlockBest(ct, pixt, 4, 16);
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

	if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
	{
		BGBBTJ_BCn_DecodeBlockBC4(ct, pixt+3, 4, 16);
	}

//	BGBBTJ_BC7_EncodeBlock(ct, pixt, 4, 16);
	BGBBTJ_BC7_EncodeBlockBest(ct, pixt, 4, 16);
	*rcs=cs;
}

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksBC7_I(
	BGBBTJ_BTIC1C_ImageInfo *ctx, byte *ibuf, int isz,
	byte *blks, byte *lblks, byte *ablks,
	int xs, int ys, int dfl)
{
	byte tb[16];
	byte *cs, *cse, *csm, *csl, *csa;
	byte *ct, *cte, *ctb;
	u64 clraw, clrbw;
	int diffm, diffsc;
	int xs1, ys1, n, n1, ret, mode, nextmode;
	int xo, yo, clra, clrb;
	int i, j, k, l;
	
	BGBBTJ_BT1C_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+16*n;
	csl=lblks; csa=ablks;
	if(!csl)csl=blks;
	if(!csa)csa=blks;
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

	ctx->diff_lclra=0;
	ctx->diff_lclrb=0;
	ctx->diff_lclrc=0;
	ctx->diff_lclrd=0;

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
					bgbbtj_bt1c_memcpy16(ct, csl);
					ct+=16; csl+=16; csa+=8;
				}
			}else
			{
				n1=(i&31)+1;
				ct+=n1*16; csl+=n1*16; csa+=n1*8;
			}
			break;			
		case 0xE0:
			switch(i)
			{
			case 0xE0: case 0xE1: case 0xE2: case 0xE3:
			case 0xE4: case 0xE5: case 0xE6: case 0xE7:
			case 0xE8: case 0xE9: case 0xEA: case 0xEB:
			case 0xEC:
			case 0xEF:

			case 0xF0: case 0xF1: case 0xF2: case 0xF3:
			case 0xF4: case 0xF5: case 0xF6: case 0xF7:
				ctx->curmode=mode;
				ctx->nextmode=nextmode;
				ret=BGBBTJ_BT1C_DecodeBlocksCommandBasic(ctx, &cs, i);
				mode=ctx->curmode;
				nextmode=ctx->nextmode;
				break;
				
			case 0xED: case 0xEE:
				ret=BGBBTJ_BT1C_DecodeBlocksCommandBlockCopy16A8(
					ctx, &cs, &ct, &csl, &csa, i);
				break;

			case 0xF8:
			case 0xF9:
			case 0xFA:
			case 0xFB:
				j=((i-0xF8)<<8)|(*cs++);
				ctx->curmode=mode;
				ctx->nextmode=nextmode;
				ret=BGBBTJ_BT1C_DecodeBlocksCommandExt(ctx, &cs, j);
				diffm=ctx->diff_m;
				diffsc=ctx->diff_sc;
				mode=ctx->curmode;
				nextmode=ctx->nextmode;
				break;
			case 0xFC:
			case 0xFD:
			case 0xFE:
			case 0xFF:
				j=((i-0xFC)<<8)|(*cs++);
				if((j>=0x80) && (j<=0x9F))
				{
					n1=(j&31)+1;
					BGBBTJ_BT1C_DecodeBlockColorArrayBC7(ctx,
						blks, ct, csa, &cs, n1,
						xs1, ys1, 16, mode, dfl);
					ct+=n1*16; csl+=n1*16; csa+=n1*8;
					break;
				}
				break;
			default:
				ret=-1;
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
							blks, ct, xs1, ys1, 16);
						j=k+j;

						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;
					}else
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;
						l=j;
						j=((j&0x7FE0)<<1)|(j&0x1F);
						j|=(j>>5)&0x20;

						k=BGBBTJ_BT1C_ConvRGB16ToRGB24(ctx, j);
						ctx->diff_lclra=k;
						ctx->diff_lclrb=k;
					}

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ct, j, j, 0x00000000, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						if(l&0x8000)
						{
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAlpha(
								ctb, j, j, 0xFFFFFFFFU, dfl);
						}else
						{
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ctb, j, j, 0x00000000, dfl);
						}

						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
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
							blks, ct, xs1, ys1, 16);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, 16);
						k=l+k;

						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
						clra=j; clrb=k;

						j=((j&0x7FE0)<<1)|(j&0x1F);
						k=((k&0x7FE0)<<1)|(k&0x1F);
						j|=(j>>5)&0x20;
						k|=(k>>5)&0x20;

						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
							cs+=4;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ct, clra, clrb, k, dfl);
							ct+=16; csl+=16; csa+=8;
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

					clra=j; clrb=k;
					
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						if(l&0x8000)
						{
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasicAlpha(
								ct, clra, clrb, k, dfl);
						}else
						{
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ct, clra, clrb, k, dfl);
						}
						ct+=16; csl+=16; csa+=8;
					}
					break;

				default:
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

						clra=j; clrb=k;
						k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
							ct, clra, clrb, k, dfl);
						ct+=16; csl+=16; csa+=8;
					}else
					{
//						memset(ctb, 0, 16);
						//dummy...
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockBC7(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
//						cs+=31;
//						bgbbtj_bt1c_memcpy16(ct, ctb);
						ct+=16; csl+=16; csa+=8;
					}
					break;
				}
				break;
			case 1:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					j|=(j>>5)&0x20;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					j=bt1c_blkidxmap1[cs[0]];
					k=bt1c_blkidxmap1[cs[1]];
					cs+=2;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, (j<<16)|k, dfl);
					ct+=16; csl+=16; csa+=8;
					break;
				}
				break;
			case 2:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					j|=(j>>5)&0x20;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;
					clra=j; clrb=k;
					j=bt1c_blkidxmap2[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=16; csl+=16; csa+=8;
					break;
				}
				break;
			case 3:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal256[*cs++];
					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					j|=(j>>5)&0x20;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal256[cs[0]];
					k=ctx->pal256[cs[1]];
					cs+=2;
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;
					clra=j; clrb=k;
					j=ctx->pat256[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=16; csl+=16; csa+=8;
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
							blks, ct, xs1, ys1, 16);
						j=k+j;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

					j=((j&0x7FE0)<<1)|(j&0x1F);
					j|=(j>>5)&0x20;

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
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
							blks, ct, xs1, ys1, 16);
						j=l+j;
						l=BGBBTJ_BT1C_PredictDiffColorB(ctx,
							blks, ct, xs1, ys1, 16);
						k=l+k;
					}else
					{
						j=(cs[0]<<8)|cs[1]; cs+=2;
						k=(cs[0]<<8)|cs[1]; cs+=2;
					}

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					if(mode==4)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ct, clra, clrb, (j<<16)|k, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else if(mode==5)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ct, clra, clrb, j, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else if(mode==14)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=ctx->pat256[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
								ct, clra, clrb, j, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[1]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockBC7(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
						ct+=16; csl+=16; csa+=8;
						break;
					}

					cs--;

					j=(cs[0]<<8)|cs[1]; cs+=2;
					k=(cs[0]<<8)|cs[1]; cs+=2;
					j=((j&0x7FE0)<<1)|(j&0x1F);
					k=((k&0x7FE0)<<1)|(k&0x1F);
					j|=(j>>5)&0x20;
					k|=(k>>5)&0x20;

					ctx->diff_lclra=j;
					ctx->diff_lclrb=j;

//					ctx->diff_lclra=BGBBTJ_BT1C_ConvRGB16ToRGB24(ctx, j);
//					ctx->diff_lclrb=BGBBTJ_BT1C_ConvRGB16ToRGB24(ctx, k);

					clra=j; clrb=k;
					if(mode==4)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=16; csl+=16; csa+=8;
					}else if(mode==5)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}else if(mode==14)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
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
//							blks, ct, xs1, ys1, 16);
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

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
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
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;

				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockBC7(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
						ct+=16; csl+=16; csa+=8;
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

					clra=j; clrb=k;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
					cs+=4;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=16; csl+=16; csa+=8;
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

						clraw=BGBBTJ_BT1C_CalcDiff15ColorA48(ctx,
							blks, ct, xs1, ys1, 16, j);
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clraw;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						cs+=4;
						l=j;
						if(mode==8)
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, j);
						}
						else
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, j);
						}
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clraw;
					}
					
					j=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clraw);

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(diffm && (cs[0]&0x80))
					{
						j=(cs[0]<<8)|cs[1];
						k=(cs[2]<<8)|cs[3];
						cs+=4;
						clraw=BGBBTJ_BT1C_CalcDiff15ColorA48(ctx,
							blks, ct, xs1, ys1, 16, j);
						clrbw=BGBBTJ_BT1C_CalcDiff15ColorB48(ctx,
							blks, ct, xs1, ys1, 16, k);
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clrbw;
					}else
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
						k=(cs[4]<<24)|(cs[5]<<16)|(cs[6]<<8)|cs[7];
						cs+=8;
						l=k;
						if(mode==8)
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, j);
							clrbw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, k);
						}
						else
						{
							clraw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, j);
							clrbw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, k);
						}
						ctx->diff_lclraw=clraw;
						ctx->diff_lclrbw=clrbw;
					}

					j=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clraw);
					k=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clrbw);
					clra=j; clrb=k;

					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
						cs+=4;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;

				default:
					if((ctx->optcntrl[1]>0) && !(cs[3]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockBC7(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
						ct+=16; csl+=16; csa+=8;
						break;
					}

					cs--;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					k=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|cs[3];
					cs+=8;

					if(mode==8)
					{
						clraw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, j);
						clrbw=BGBBTJ_BT1C_ConvRGB31FUToRGB48F(ctx, k);
					}
					else
					{
						clraw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, j);
						clrbw=BGBBTJ_BT1C_ConvRGB31FSToRGB48F(ctx, k);
					}
					ctx->diff_lclraw=clraw;
					ctx->diff_lclrbw=clrbw;

					j=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clraw);
					k=BGBBTJ_BT1C_ConvRGB48FToRGB24(ctx, clrbw);
					clra=j; clrb=k;
					j=(cs[0]<<24)|(cs[1]<<16)|(cs[2]<<8)|(cs[3]);
					cs+=4;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=16; csl+=16; csa+=8;
					break;
				}
				break;
#endif

#if 1
			case 9:
				switch(i&0xE0)
				{
				case 0xA0:
					j=ctx->pal16[(*cs++)&15];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					j=bt1c_blkidxmap1[cs[0]];
					k=bt1c_blkidxmap1[cs[1]];
					cs+=2;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, (j<<16)|k, dfl);
					ct+=16; csl+=16; csa+=8;
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
					if(cs[0]&0x80)
					{
						j=(cs[0]<<8)|cs[1];
						cs+=2;
						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
							blks, ct, xs1, ys1, 32, j);
					
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
					
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
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
							blks, ct, xs1, ys1, 32, j);
						k=BGBBTJ_BT1C_CalcDiff15ColorB24(ctx,
							blks, ct, xs1, ys1, 32, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;
					if(mode==10)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, clra, clrb, (j<<16)|k, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else if(mode==11)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else if(mode==12)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=ctx->pat256[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockBC7(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
						ct+=16; csl+=16; csa+=8;
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

					clra=j; clrb=k;
					if(mode==10)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=16; csl+=16; csa+=8;
					}else if(mode==11)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}else if(mode==12)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
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
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
//							blks, ct, xs1, ys1, 32, j);
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

					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
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
//						j=bt1c_difftab[(diffsc*256)+(j&127)];
//						k=bt1c_difftab[(diffsc*256)+(k&127)];
//						j=BGBBTJ_BT1C_CalcDiff15ColorA24(ctx,
//							blks, ct, xs1, ys1, 32, j);
//						k=BGBBTJ_BT1C_CalcDiff15ColorB24(ctx,
//							blks, ct, xs1, ys1, 32, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=(cs[0]<<16)|(cs[1]<<8)|cs[2];
						k=(cs[3]<<16)|(cs[4]<<8)|cs[5];
						cs+=6;
						l=k;
						j=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvRGB23ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[2]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockBC7(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
						ct+=16; csl+=16; csa+=8;
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

					clra=j; clrb=k;
					j=bt1c_blkidxmap2[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=16; csl+=16; csa+=8;
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
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					j|=(j>>5)&0x20;
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				case 0xC0:
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					n1=(i&31)+1;
					for(i=0; i<n1; i++)
					{
						j=ctx->pat256[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}
					break;
				default:
					cs--;
					j=ctx->pal16[(cs[0]>>4)&15];
					k=ctx->pal16[(cs[0]>>0)&15];
					cs++;
//					j=((j&0x7FE0)<<1)|(j&0x1F);
//					k=((k&0x7FE0)<<1)|(k&0x1F);
//					j|=(j>>5)&0x20;
//					k|=(k>>5)&0x20;

					clra=j; clrb=k;
					j=ctx->pat256[*cs++];
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
						{ bgbbtj_bt1c_memcpy8(ct, csa); }
					BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
						ct, clra, clrb, j, dfl);
					ct+=16; csl+=16; csa+=8;
					break;
				}
				break;
#endif

#if 1
			case 16:
			case 17:
			case 18:
				switch(i&0xE0)
				{
				case 0xA0:
					if(cs[0]&0x80)
					{
						j=*cs++;
						k=ctx->diff_lclra;
						j=BGBBTJ_BT1C_AddPred24ToDiff7Y(ctx, k, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}else
					{
						j=*cs++;
						j=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, j);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=j;
					}
					
					if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy8(ct, csa);
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, j, j, 0, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else
					{
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ctb, j, j, 0, dfl);
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							bgbbtj_bt1c_memcpy16(ct, ctb);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				case 0xC0:
					if(cs[0]&0x80)
					{
						j=cs[0]; k=cs[1]; cs+=2;
						l=ctx->diff_lclra;
						j=BGBBTJ_BT1C_AddPred24ToDiff7Y(ctx, l, j);
						l=ctx->diff_lclrb;
						k=BGBBTJ_BT1C_AddPred24ToDiff7Y(ctx, l, k);

						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}else
					{
						j=cs[0]; k=cs[1]; cs+=2;
						j=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, j);
						k=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, k);
						ctx->diff_lclra=j;
						ctx->diff_lclrb=k;
					}

					clra=j; clrb=k;
					if(mode==16)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=(cs[0]<<24)|(cs[1]<<16)|
								(cs[2]<<8)|(cs[3]);
							cs+=4;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else if(mode==17)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap1[cs[0]];
							k=bt1c_blkidxmap1[cs[1]];
							cs+=2;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, clra, clrb, (j<<16)|k, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else if(mode==18)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=bt1c_blkidxmap2[*cs++];
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}
					break;
				default:
					if((ctx->optcntrl[1]>0) && !(cs[0]&0x80))
					{
						cs--;
						BGBBTJ_BT1C_DecodeRawBlockBC7(ctx,
							blks, ct, csa, &cs, 1,
							xs1, ys1, 16, mode, dfl);
						ct+=16; csl+=16; csa+=8;
						break;
					}

					cs--;
					j=cs[0]; k=cs[1]; cs+=2;
					j=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, j);
					k=BGBBTJ_BT1C_ConvYYY7ToRGB24(ctx, k);
					ctx->diff_lclra=j;
					ctx->diff_lclrb=k;

					clra=j; clrb=k;
					if(mode==16)
					{
						n1=(i&31)+1;
						for(i=0; i<n1; i++)
						{
							j=(cs[0]<<24)|(cs[1]<<16)|
								(cs[2]<<8)|(cs[3]);
							cs+=4;
							if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
								{ bgbbtj_bt1c_memcpy8(ct, csa); }
							BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
								ct, clra, clrb, j, dfl);
							ct+=16; csl+=16; csa+=8;
						}
					}else if(mode==17)
					{
						j=bt1c_blkidxmap1[cs[0]];
						k=bt1c_blkidxmap1[cs[1]];
						cs+=2;
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, (j<<16)|k, dfl);
						ct+=16; csl+=16; csa+=8;
					}else if(mode==18)
					{
						j=bt1c_blkidxmap2[*cs++];
						if(dfl&BGBBTJ_BTIC1C_DFL_HASAX)
							{ bgbbtj_bt1c_memcpy8(ct, csa); }
						BGBBTJ_BT1C_DecodeBlocksBC7_EncBlockBasic_RGB24(
							ct, clra, clrb, j, dfl);
						ct+=16; csl+=16; csa+=8;
					}
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

BGBBTJ_API int BGBBTJ_BT1C_DecodeBlocksBC7(BGBBTJ_BTIC1C_ImageInfo *ctx,
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys, int dfl)
{
	byte *cs, *csl;
	byte *tbuf;
	int xs1, ys1, xs2, ys2, clrs2;
	int i, j, k, n;

	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
		{
			j=BGBBTJ_BT1C_DecodeBlocksBC4(ctx, ibuf+i, isz-i,
				ctx->adata, ctx->ladata, xs, ys, 8);
//			memcpy(ctx->ladata, ctx->adata, ctx->nblocks*8);
//			memcpy(blks, ctx->adata, ctx->nblocks*16);

			BGBBTJ_BT1C_DecodeBlocksBC7_I(ctx, ibuf, isz,
				blks, lblks, ctx->adata, xs, ys,
				dfl|BGBBTJ_BTIC1C_DFL_HASAX);

//			j=BGBBTJ_BT1C_DecodeBlocksBC4(ctx, ibuf+i, isz-i,
//				blks, lblks, xs, ys, 16);
//			BGBBTJ_BT1C_DecodeBlocksBC7_I(ctx, ibuf, isz,
//				blks, lblks, xs, ys, dfl|BGBBTJ_BTIC1C_DFL_HASAX);
			cs=ibuf+i+j;
			k=cs-ibuf;
		}else
		{
			i=BGBBTJ_BT1C_DecodeBlocksBC7_I(ctx, ibuf, isz,
				blks, NULL, NULL, xs, ys, dfl);
			cs=ibuf+i;
			k=cs-ibuf;
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
		BGBBTJ_BT1C_DecodeBlocksBC7(ctx, tbuf, k, blks, lblks, xs, ys, dfl);
//		free(tbuf);
		k=i;
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='J') && (ibuf[5]=='P'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
//		i+=6;
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		if(!ctx->jpg_ctx)
			ctx->jpg_ctx=BGBBTJ_JPG_AllocContext();
			
		clrs2=BGBBTJ_JPG_BC7_VF;
		if(dfl&BGBBTJ_BTIC1C_DFL_FLIPBLOCKS)
			clrs2=BGBBTJ_JPG_BC7_VF2;
		BGBBTJ_JPG_DecodeBufferClrsCtx(
			ctx->jpg_ctx, ibuf+6, i-6,
			blks, clrs2, &xs2, &ys2);
//		BGBBTJ_JPG_DecodeBufferClrs(ibuf+6, i-6,
//			blks, BGBBTJ_JPG_BC7_VF, &xs2, &ys2);
		k=i;
		cs=ibuf+i;
	}else
	{
		return(-1);
	}
	return(k);
}
