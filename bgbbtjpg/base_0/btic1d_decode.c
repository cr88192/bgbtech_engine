#include <bgbbtj.h>

void BGBBTJ_BT1D_Init()
{
	static int init=0;

	if(init)return;
	init=1;
}

static void bgbbtj_bt1d_memcpy8(void *dst, void *src)
{
#if defined(X86) || defined(X86_64)
	*(u64 *)dst=*(u64 *)src;
#else
	memcpy(dst, src, 8);
#endif
}

static void bgbbtj_bt1d_memcpy16(void *dst, void *src)
{
#if defined(X86) || defined(X86_64)
	((u64 *)dst)[0]=((u64 *)src)[0];
	((u64 *)dst)[1]=((u64 *)src)[1];
#else
	memcpy(dst, src, 16);
#endif
}

BGBBTJ_API int BGBBTJ_BT1D_DecodeBlocks(
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys)
{
	byte tb[16];
	byte *cs, *cse, *cs2;
	byte *ct, *cte, *ctb, *ctl, *ctle;
	int xs1, ys1, n, n1, ret;
	int i, j, k, l;
	
	BGBBTJ_BT1D_Init();
	
	xs1=(xs+3)/4;
	ys1=(ys+3)/4;
	n=xs1*ys1;
	
	ctb=tb;
	cs=ibuf; cse=cs+isz;
	ct=blks; cte=ct+16*n;
	ctl=lblks; ctle=ctl+16*n;
	ret=0;

	if(ibuf[0]==0xE3)
	{
		l=(cs[1]<<16)|(cs[2]<<8)|cs[2];
		cs=ibuf+6; cse=ibuf+l;
	}
	
	while((cs<cse) && (ct<cte) && !ret)
	{
		i=*cs++;
		switch(i&0xF0)
		{
		case 0x80:
			n1=(i&15)+1;
			for(i=0; i<n1; i++)
			{
				bgbbtj_bt1d_memcpy16(ct, ctl);
//				if(!(ct[0]&0xE0))ct[0]|=0x40;
				switch(ct[0]>>5)
				{
				case 0: case 1:	ct[0]|=0x80; break;
				case 2: case 3:	break;
				case 4: case 5:	break;
				case 6: case 7:	ct[0]|=0x10; break;
				}
				ct+=16; ctl+=16;
			}
			break;
		case 0x90:
			if(cs[0]&0x80)
			{
				l=(cs[0]<<8)|(cs[1]);
				cs+=2;
				j=((l>>7)&127)-64;
				k=(l&127)-64;
			}else
			{
				l=*cs++;
				j=((l>>4)&7)-4;
				k=(l&15)-8;
			}

			n1=(i&15)+1;
		
			cs2=ctl+((j*xs1+k)*16);
			if((cs2<lblks) || (cs2+(n1*16))>ctle)
				{ ret=-1; break; }
		
			for(i=0; i<n1; i++)
			{
				bgbbtj_bt1d_memcpy16(ct, cs2);

				switch(ct[0]>>5)
				{
				case 0: case 1:	break;
				case 2: case 3:	break;
				case 4: case 5:	ct[0]&=~0x80; break;
				case 6: case 7:	ct[0]&=~0x10; break;
				}

				cs2+=16; ct+=16; ctl+=16;
			}
			break;
		case 0xA0:
			if(cs[0]&0x80)
			{
				ctb[0]=0xC0;	ctb[1]=0x00;
				ctb[2]=0x00;	ctb[3]=0x00;
				ctb[4]=cs[0];	ctb[5]=cs[1];
				ctb[6]=0x00;	ctb[7]=0x00;
				cs+=2;
			}
			else
			{
				ctb[0]=0xC0;	ctb[1]=0x00;
				ctb[2]=0x00;	ctb[3]=0x00;
				ctb[4]=cs[0];	ctb[5]=cs[1];
				ctb[6]=cs[2];	ctb[7]=cs[3];
				cs+=4;
			}

			n1=(i&15)+1;
			for(i=0; i<n1; i++)
			{
				bgbbtj_bt1d_memcpy16(ct, ctb);
				ct+=16; ctl+=16;
			}
			break;
		case 0xB0:
			if((cs[0]&0xE0)==0xE0)
			{
				ctb[0]=0xE0;	ctb[1]=0x00;
				ctb[2]=0x00;	ctb[3]=0x00;
				ctb[4]=cs[0];	ctb[5]=cs[1];
				ctb[6]=cs[2];	ctb[7]=cs[3];
				cs+=4;

				n1=(i&15)+1;
				for(i=0; i<n1; i++)
				{
					ctb[ 8]=cs[0];	ctb[ 9]=cs[1];
					ctb[10]=cs[2];	ctb[11]=cs[3];
					cs+=4;
					bgbbtj_bt1d_memcpy16(ct, ctb);
					ct+=16; ctl+=16;
				}
			}else
			{
				bgbbtj_bt1d_memcpy8(ctb, cs);
				cs+=8;

				n1=(i&15)+1;
				for(i=0; i<n1; i++)
				{
					bgbbtj_bt1d_memcpy8(ctb+8, cs);
					cs+=8;
					bgbbtj_bt1d_memcpy16(ct, ctb);
					ct+=16; ctl+=16;
				}
			}
			break;

		case 0xC0:
			n1=(i&15)+1;
//			while(j--)
			for(i=0; i<n1; i++)
			{
				bgbbtj_bt1d_memcpy16(ct, ct-16);
				ct+=16; ctl+=16;
			}
			break;
		case 0xD0:
			if((*cs)&0x80)
			{
				if((*cs)&0x40)
				{
					if((*cs)&0x20)
					{
						k=((cs[0]&0x0F)<<24)|(cs[1]<<16)|
							(cs[2]<<8)|(cs[3]); cs+=4;
					}else
						{ k=((cs[0]&0x1F)<<16)|(cs[1]<<8)|(cs[2]); cs+=3; }
				}else
					{ k=((cs[0]&0x3F)<<8)|(cs[1]); cs+=2; }
			}else
				{ k=*cs++; }

			n1=(i&15)+1;
		
			cs2=ct-(k*16);
			if((cs2<blks) || (cs2+(n1*16))>cte)
				{ ret=-1; break; }
		
			for(i=0; i<n1; i++)
			{
				bgbbtj_bt1d_memcpy16(ct, cs2);
				cs2+=16; ct+=16; ctl+=16;
			}
			break;
			
		case 0xE0:
		case 0xF0:
			switch(i)
			{
			case 0xE0:
				break;
			case 0xE1:
				cs+=3;
				break;
			case 0xE8:
				bgbbtj_bt1d_memcpy16(ct, ct-16);
				ct+=16; ctl+=16;
				break;
			case 0xE9:
				j=(*cs++)+1;
				while(j--)
				{
					bgbbtj_bt1d_memcpy16(ct, ct-16);
					ct+=16; ctl+=16;
				}
				break;
			case 0xEA:
				j=(*cs++)+1;
				if((ct-j*16)<blks) { ret=-1; break; }
				bgbbtj_bt1d_memcpy16(ct, ct-j*16);
				ct+=16; ctl+=16;
				break;
			case 0xEB:
				j=(cs[0]<<8)+cs[1]+1;
				cs+=2;
				if((ct-j*16)<blks) { ret=-1; break; }
				bgbbtj_bt1d_memcpy16(ct, ct-j*16);
				ct+=16; ctl+=16;
				break;
			case 0xEC:
				j=(*cs++)+1;
				k=(*cs++)+1;
				if((ct+j*16)>cte) { ret=-1; break; }
				if((ct-k*16)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_bt1d_memcpy16(ct, ct-k*16);
					ct+=16; ctl+=16;
				}
				break;
			case 0xED:
				j=(cs[0])+1;
				k=(cs[1]<<8)+cs[2]+1;
				cs+=3;
				if((ct+j*16)>cte) { ret=-1; break; }
				if((ct-k*16)<blks) { ret=-1; break; }
				while(j--)
				{
					bgbbtj_bt1d_memcpy16(ct, ct-k*16);
					ct+=16; ctl+=16;
				}
				break;
			default:
				ret=-1;
				break;
			}
			break;

		case 0x00:	case 0x10:	case 0x20:	case 0x30:
		case 0x40:	case 0x50:	case 0x60:	case 0x70:
			bgbbtj_bt1d_memcpy16(ct, cs-1);
			cs+=15; ct+=16; ctl+=16;
			break;
		default:
			ret=-1;
			break;
		}
	}
	
	if(ret)return(ret);
	return(cs-ibuf);
}

BGBBTJ_API int BGBBTJ_BT1D_DecodeImageBlocks(
	byte *ibuf, int isz, byte *blks, byte *lblks, int xs, int ys)
{
	byte *tbuf;
	int i, j, k;

	if(ibuf[0]==0xE1)
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		if((ibuf[i+0]==0xE3) && (ibuf[i+4]=='A') && (ibuf[i+5]=='X'))
		{
			BGBBTJ_BT1D_DecodeBlocks(ibuf, isz, blks, lblks, xs, ys);
			j=(ibuf[i+1]<<16)|(ibuf[i+2]<<8)|(ibuf[i+3]<<0);
			k=i+j;
		}else
		{
			k=BGBBTJ_BT1D_DecodeBlocks(ibuf, isz, blks, lblks, xs, ys);
		}
	}else if((ibuf[0]==0xE3) && (ibuf[4]=='Z') && (ibuf[5]=='I'))
	{
		i=(ibuf[1]<<16)|(ibuf[2]<<8)|(ibuf[3]<<0);
		j=((xs+3)>>2)*((ys+3)>>2)*3*16; k=0;
		
		tbuf=malloc(j);
		PDUNZ_DecodeStreamSz(ibuf+8, tbuf, i-8, j, &k, 0);
		BGBBTJ_BT1D_DecodeBlocks(tbuf, k, blks, lblks, xs, ys);
		free(tbuf);
		k=i;
	}else
	{
		return(-1);
	}
	return(k);
}

#if 0
// int BGBBTJ_BT1D_DecompressFrame(BGBBTJ_AVI_Context *ctx,
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
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A_RGBA32(hdr->data, dst, xs, ys, 4);
		break;
	case BGBBTJ_JPG_RGB:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
//		BGBBTJ_BCn_FlipImageDXT1(hdr->data, hdr->ldata, xs, ys, 8);
//		BGBBTJ_BCn_DecodeImageDXT1(hdr->ldata, dst, xs, ys, 3);
		BGBBTJ_BT1D_DecodeImageBX1A_RGBA32(hdr->data, dst, xs, ys, 3);
		break;
	case BGBBTJ_JPG_RGBA_Q11_4:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A_RGBA_Q11_4(hdr->data, dst, xs, ys, 4);
		break;
	case BGBBTJ_JPG_RGB_Q11_4:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A_RGBA_Q11_4(hdr->data, dst, xs, ys, 3);
		break;
	case BGBBTJ_JPG_RGBA_F32:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A(hdr->data, dst, xs, ys, 4);
		break;
	case BGBBTJ_JPG_RGB_F32:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A(hdr->data, dst, xs, ys, 3);
		break;
	default:
		break;
	}
#endif
	return(i);
}
#endif

BGBBTJ_API int BGBBTJ_BT1D_DecodeFrameImage(BGBBTJ_BTIC1C_Context *ctx,
	void *src, void *dst, int ssz, int dsz,
	int xs, int ys, int clrs)
{
	BGBBTJ_BTIC1C_ImageInfo *hdr;
	int xs1, ys1;
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

#if 1
	switch(clrs)
	{
	case BGBBTJ_JPG_RGBA:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
		memcpy(hdr->ldata, hdr->data, hdr->sz_ldata);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		if(ctx->flip)
		{	BGBBTJ_BT1D_DecodeImageBX1A_FlipRGBA32(
				hdr->data, dst, xs, ys, 4); }
		else
		{
//			 BGBBTJ_BT1D_DecodeImageBX1A_RGBA32(
//				hdr->data, dst, xs, ys, 4);
			 BGBBTJ_BT1D_DecodeImageBX1AI_RGBA32(
				hdr->data, dst, xs, ys, 4);
		}
		break;
	case BGBBTJ_JPG_RGB:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
		memcpy(hdr->ldata, hdr->data, hdr->sz_ldata);
//		BGBBTJ_BCn_FlipImageDXT1(hdr->data, hdr->ldata, xs, ys, 8);
//		BGBBTJ_BCn_DecodeImageDXT1(hdr->ldata, dst, xs, ys, 3);
		if(ctx->flip)
		{	BGBBTJ_BT1D_DecodeImageBX1A_FlipRGBA32(
				hdr->data, dst, xs, ys, 3); }
		else
		{
//			 BGBBTJ_BT1D_DecodeImageBX1A_RGBA32(
//				hdr->data, dst, xs, ys, 3);
			 BGBBTJ_BT1D_DecodeImageBX1AI_RGBA32(
				hdr->data, dst, xs, ys, 3);
		}
		break;
	case BGBBTJ_JPG_BGRA:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
		memcpy(hdr->ldata, hdr->data, hdr->sz_ldata);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		if(ctx->flip)
		{	BGBBTJ_BT1D_DecodeImageBX1A_FlipBGRA32(
				hdr->data, dst, xs, ys, 4); }
		else
		{
//			 BGBBTJ_BT1D_DecodeImageBX1A_BGRA32(
//				hdr->data, dst, xs, ys, 4);
			 BGBBTJ_BT1D_DecodeImageBX1AI_BGRA32(
				hdr->data, dst, xs, ys, 4);
		}
		break;
	case BGBBTJ_JPG_RGBA_Q11_4:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
		memcpy(hdr->ldata, hdr->data, hdr->sz_ldata);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A_RGBA_Q11_4(hdr->data, dst, xs, ys, 4);
		break;
	case BGBBTJ_JPG_RGB_Q11_4:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
		memcpy(hdr->ldata, hdr->data, hdr->sz_ldata);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A_RGBA_Q11_4(hdr->data, dst, xs, ys, 3);
		break;
	case BGBBTJ_JPG_RGBA_F32:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
		memcpy(hdr->ldata, hdr->data, hdr->sz_ldata);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A(hdr->data, dst, xs, ys, 4);
		break;
	case BGBBTJ_JPG_RGB_F32:
		i=BGBBTJ_BT1D_DecodeImageBlocks(src, ssz,
			hdr->data, hdr->ldata, xs, ys);
		memcpy(hdr->ldata, hdr->data, hdr->sz_ldata);
//		BGBBTJ_BCn_FlipImageDXT5(hdr->data, hdr->ldata, xs, ys);
		BGBBTJ_BT1D_DecodeImageBX1A(hdr->data, dst, xs, ys, 3);
		break;
	default:
		break;
	}
#endif

	return(i);
}

int bt1d_decompress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	BGBBTJ_BTIC1C_Context *info;
	
	info=ctx->data;

	return(BGBBTJ_BT1D_DecodeFrameImage(
		info, src, dst, ssz, dsz,
		info->width, info->height, info->clrs));
}

int bt1d_decompress_frame_clrs(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz, int clrs)
{
	BGBBTJ_BTIC1C_Context *info;
	
	info=ctx->data;

	return(BGBBTJ_BT1D_DecodeFrameImage(
		info, src, dst, ssz, dsz,
		info->width, info->height, clrs));
}

int bt1d_compress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl)
{
	BGBBTJ_BTIC1C_Context *info;
	int sz;
	
	info=ctx->data;

	sz=BGBBTJ_BT1D_EncodeImage(ctx->data, src, dst, dsz,
		info->width, info->height, qfl, clrs);
	return(sz);
}

BGBBTJ_VidCodecCTX *bt1d_begin_decompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
	BGBBTJ_BTIC1C_Context *info;
	int err;

	if((fcc!=RIFF_TAG_bt1d) && (in->biCompression!=RIFF_TAG_bt1d))
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
//	info=gcalloc(sizeof(vfw_ctxinfo));
	info=BGBBTJ_BT1D_AllocContext();
	ctx->data=info;

	info->width=abs(in->biWidth);
	info->height=abs(in->biHeight);

	if(out)
	{
		info->clrs=(out->biBitCount==24)?
			BGBBTJ_JPG_RGB:BGBBTJ_JPG_RGBA;
			
		if(out->biCompression==RIFF_TAG_BGRA)
			info->clrs=BGBBTJ_JPG_BGRA;
	}
	else
		{ info->clrs=BGBBTJ_JPG_RGBA; }

	info->flip=((short)(out->biHeight))<0;

	ctx->decompress_frame=&bt1d_decompress_frame;

	return(ctx);
}

BGBBTJ_VidCodecCTX *bt1d_begin_compress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	BGBBTJ_VidCodecCTX *ctx;
	BGBBTJ_BTIC1C_Context *info;
	int err;

	if((fcc!=RIFF_TAG_bt1d) && (in->biCompression!=RIFF_TAG_bt1d))
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
//	info=gcalloc(sizeof(vfw_ctxinfo));
	info=BGBBTJ_BT1D_AllocContext();
	ctx->data=info;

	info->width=in->biWidth;
	info->height=in->biHeight;
	info->clrs=BGBBTJ_JPG_RGBA;

	if(in && (in->biBitCount==24))
		{ info->clrs=BGBBTJ_JPG_RGB; }

	ctx->compress_frame=&bt1d_compress_frame;

	return(ctx);
}

int BGBBTJ_CodecBT1D_Init()
{
	BGBBTJ_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("BT1D: Init\n");

	codec=BGBBTJ_VidCodec_New();
	codec->begin_decompress=&bt1d_begin_decompress;
	codec->begin_compress=&bt1d_begin_compress;
	return(0);
}
