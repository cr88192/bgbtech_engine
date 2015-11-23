#ifdef _WIN32

#include <windows.h>
#include <vfw.h>
// #include "vfwext.h"

#include <bgbbtj.h>


#ifdef BGBBTJ_VFWDRV

extern u32 btjpg_drv_defaultCodecFcc=BGBBTJ_FCC_bt1c;

HINSTANCE g_hInst;

static LRESULT btjvfw_dumpbitmapinfos(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	if(lParam1)
	{
		btjpg_printf("\tlParam1 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam1->bmiHeader.biCompression),
			lParam1->bmiHeader.biWidth,
			lParam1->bmiHeader.biHeight,
			lParam1->bmiHeader.biPlanes,
			lParam1->bmiHeader.biBitCount);

		if(((lParam1->bmiHeader.biCompression==BGBBTJ_FCC_RGBA) ||
			(lParam1->bmiHeader.biCompression==BI_BITFIELDS)) &&
			(lParam1->bmiHeader.biBitCount==32))
		{
			btjpg_printf("\t\tR=%08X G=%08X B=%08X A=%08X\n",
				((u32 *)((&lParam1->bmiHeader)+1))[0],
				((u32 *)((&lParam1->bmiHeader)+1))[1],
				((u32 *)((&lParam1->bmiHeader)+1))[2],
				((u32 *)((&lParam1->bmiHeader)+1))[3]);
		}

		if(((lParam1->bmiHeader.biCompression==BGBBTJ_FCC_RGBA) ||
			(lParam1->bmiHeader.biCompression==BI_BITFIELDS)) &&
			(lParam1->bmiHeader.biBitCount==16))
		{
			btjpg_printf("\t\tR=%04X G=%04X B=%04X A=%04X\n",
				((u32 *)((&lParam1->bmiHeader)+1))[0],
				((u32 *)((&lParam1->bmiHeader)+1))[1],
				((u32 *)((&lParam1->bmiHeader)+1))[2],
				((u32 *)((&lParam1->bmiHeader)+1))[3]);
		}
	}

	if(lParam2)
	{
		btjpg_printf("\tlParam2 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam2->bmiHeader.biCompression),
			lParam2->bmiHeader.biWidth,
			lParam2->bmiHeader.biHeight,
			lParam2->bmiHeader.biPlanes,
			lParam2->bmiHeader.biBitCount);

		if(((lParam2->bmiHeader.biCompression==BGBBTJ_FCC_RGBA) ||
			(lParam2->bmiHeader.biCompression==BI_BITFIELDS)) &&
			(lParam2->bmiHeader.biBitCount==32))
		{
			btjpg_printf("\t\tR=%08X G=%08X B=%08X A=%08X\n",
				((u32 *)((&lParam2->bmiHeader)+1))[0],
				((u32 *)((&lParam2->bmiHeader)+1))[1],
				((u32 *)((&lParam2->bmiHeader)+1))[2],
				((u32 *)((&lParam2->bmiHeader)+1))[3]);
		}

		if(((lParam2->bmiHeader.biCompression==BGBBTJ_FCC_RGBA) ||
			(lParam2->bmiHeader.biCompression==BI_BITFIELDS)) &&
			(lParam2->bmiHeader.biBitCount==16))
		{
			btjpg_printf("\t\tR=%04X G=%04X B=%04X A=%04X\n",
				((u32 *)((&lParam2->bmiHeader)+1))[0],
				((u32 *)((&lParam2->bmiHeader)+1))[1],
				((u32 *)((&lParam2->bmiHeader)+1))[2],
				((u32 *)((&lParam2->bmiHeader)+1))[3]);
		}
	}

	return(ICERR_OK);
}

static LRESULT btjvfw_compress_query(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	BGBBTJ_BMPInfoHeader bmpin, bmpout;
	int i;

	btjpg_printf("btjvfw_compress_query: %p %p\n",
		(void *)lParam1, (void *)lParam2);
	btjvfw_dumpbitmapinfos(ctx, lParam1, lParam2);

#if 1
	if(lParam1)
	{
		if(lParam1->bmiHeader.biCompression &&
			(lParam1->bmiHeader.biCompression!=BI_RGB) &&
			(lParam1->bmiHeader.biCompression!=BI_BITFIELDS) &&
			(lParam1->bmiHeader.biCompression!=BGBBTJ_FCC_RGBA))
		{
			btjpg_printf("\tReject In Format\n");
			return(ICERR_BADFORMAT);
		}
		if((lParam1->bmiHeader.biBitCount!=24) &&
			(lParam1->bmiHeader.biBitCount!=32))
		{
			btjpg_printf("\tReject In Bits\n");
			return(ICERR_BADFORMAT);
		}
	}
#endif

	if(!lParam2)
	{
		btjpg_printf("btjvfw_compress_query: OK (Any Format)\n");
		return(ICERR_OK);
	}

#if 0
	if(lParam2)
	{
		if(lParam2->bmiHeader.biCompression &&
			(lParam2->bmiHeader.biCompression!=BI_RGB) &&
			(lParam2->bmiHeader.biCompression!=BGBBTJ_FCC_RGBA))
		{
			btjpg_printf("\tReject Out Format\n");
			return(ICERR_BADFORMAT);
		}
		if((lParam2->bmiHeader.biBitCount!=24) &&
			(lParam2->bmiHeader.biBitCount!=32))
		{
			btjpg_printf("\tReject Out Bits\n");
			return(ICERR_BADFORMAT);
		}
	}
#endif

	memset(&bmpin, 0, sizeof(BGBBTJ_BMPInfoHeader));
	memset(&bmpout, 0, sizeof(BGBBTJ_BMPInfoHeader));
	
	bmpin.biCompression=lParam1->bmiHeader.biCompression;
	bmpin.biWidth=lParam1->bmiHeader.biWidth;
	bmpin.biHeight=lParam1->bmiHeader.biHeight;
	bmpin.biPlanes=lParam1->bmiHeader.biPlanes;
	bmpin.biBitCount=lParam1->bmiHeader.biBitCount;

	if(lParam2)
	{
		bmpout.biCompression=lParam2->bmiHeader.biCompression;
		bmpout.biWidth=lParam2->bmiHeader.biWidth;
		bmpout.biHeight=lParam2->bmiHeader.biHeight;
		bmpout.biPlanes=lParam2->bmiHeader.biPlanes;
		bmpout.biBitCount=lParam2->bmiHeader.biBitCount;
	}

	i=BGBBTJ_Codec_CompressQuery(RIFF_TAG_BTIC, &bmpin, &bmpout);

//	i=BGBBTJ_Codec_DecompressQuery(RIFF_TAG_BTIC,
//		(BGBBTJ_BMPInfoHeader *)lParam1,
//		(BGBBTJ_BMPInfoHeader *)lParam2);
	if(i>0)
	{
		btjpg_printf("btjvfw_compress_query: Failed\n");
		return(ICERR_OK);
	}
	btjpg_printf("\tFailed Match\n");
	return(ICERR_BADFORMAT);

//	return(ICERR_BADFORMAT);
}

static LRESULT btjvfw_compress_get_format(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	btjpg_printf("btjvfw_compress_get_format: %p %p\n",
		(void *)lParam1, (void *)lParam2);
	btjvfw_dumpbitmapinfos(ctx, lParam1, lParam2);

	if(!lParam2)
		{ return(sizeof(BITMAPINFO)); }

	lParam2->bmiHeader.biSize=sizeof(BITMAPINFO);
	lParam2->bmiHeader.biCompression=lParam1->bmiHeader.biCompression;
	lParam2->bmiHeader.biWidth=lParam1->bmiHeader.biWidth;
	lParam2->bmiHeader.biHeight=lParam1->bmiHeader.biHeight;
	lParam2->bmiHeader.biPlanes=lParam1->bmiHeader.biPlanes;
	lParam2->bmiHeader.biBitCount=lParam1->bmiHeader.biBitCount;
	lParam2->bmiHeader.biSizeImage=lParam1->bmiHeader.biSizeImage;
	lParam2->bmiHeader.biXPelsPerMeter=0;
	lParam2->bmiHeader.biYPelsPerMeter=0;
	lParam2->bmiHeader.biClrUsed=0;
	lParam2->bmiHeader.biClrImportant=0;

//	lParam2->bmiHeader.biCompression=BGBBTJ_FCC_bt1c;
	lParam2->bmiHeader.biCompression=btjpg_drv_defaultCodecFcc;

	return(ICERR_OK);

//	return(ICERR_BADFORMAT);
}

static LRESULT btjvfw_compress_get_size(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	int i;
	
	btjpg_printf("btjvfw_compress_get_size: %p %p\n",
		(void *)lParam1, (void *)lParam2);
	btjvfw_dumpbitmapinfos(ctx, lParam1, lParam2);

	i=((lParam1->bmiHeader.biWidth+3)/4)*
		((lParam1->bmiHeader.biHeight+3)/4)*
		4*16;
	return(i);
}

static LRESULT btjvfw_compress_frames_info(BGBBTJ_VidCodecCTX *ctx,
	ICCOMPRESSFRAMES *lParam1)
{
	btjpg_printf("btjvfw_compress_frames_info: %p\n", (void *)lParam1);
	return(ICERR_BADFORMAT);
}

static LRESULT btjvfw_compress_begin(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	BGBBTJ_VidCodecCTX *ctx2;
	BGBBTJ_BMPInfoHeader bmpin, bmpout;
	int i;

	btjpg_printf("btjvfw_compress_begin: %p %p\n",
		(void *)lParam1, (void *)lParam2);
	btjvfw_dumpbitmapinfos(ctx, lParam1, lParam2);

	memset(&bmpin, 0, sizeof(BGBBTJ_BMPInfoHeader));
	memset(&bmpout, 0, sizeof(BGBBTJ_BMPInfoHeader));
	
	if(lParam1)
	{
		bmpin.biCompression=lParam1->bmiHeader.biCompression;
		bmpin.biWidth=lParam1->bmiHeader.biWidth;
		bmpin.biHeight=lParam1->bmiHeader.biHeight;
		bmpin.biPlanes=lParam1->bmiHeader.biPlanes;
		bmpin.biBitCount=lParam1->bmiHeader.biBitCount;
	}

	if(lParam2)
	{
		bmpout.biCompression=lParam2->bmiHeader.biCompression;
		bmpout.biWidth=lParam2->bmiHeader.biWidth;
		bmpout.biHeight=lParam2->bmiHeader.biHeight;
		bmpout.biPlanes=lParam2->bmiHeader.biPlanes;
		bmpout.biBitCount=lParam2->bmiHeader.biBitCount;
	}

	bmpin.biCompression=BGBBTJ_FCC_RGBA;

//	ctx2=BGBBTJ_Codec_BeginCompress(RIFF_TAG_BTIC,
//		(BGBBTJ_BMPInfoHeader *)lParam1,
//		(BGBBTJ_BMPInfoHeader *)lParam2);
	ctx2=BGBBTJ_Codec_BeginCompress(RIFF_TAG_BTIC, &bmpin, &bmpout);
	if(!ctx2)
	{
		btjpg_printf("btjvfw_compress_begin: Failed\n");
		return(ICERR_BADFORMAT);
	}
		
	ctx->data=ctx2;
	return(ICERR_OK);

//	return(ICERR_BADFORMAT);
}

static LRESULT btjvfw_compress_end(BGBBTJ_VidCodecCTX *ctx)
{
	if(!ctx->data)
		{ return(ICERR_BADFORMAT); }
	BGBBTJ_VidCodecCTX_EndCompress(ctx->data);
	BGBBTJ_VidCodecCTX_Destroy(ctx->data);
	ctx->data=NULL;
	return(ICERR_OK);

//	return(ICERR_BADFORMAT);
}

static LRESULT btjvfw_compress(BGBBTJ_VidCodecCTX *ctx, ICCOMPRESS *lParam1)
{
	int dsz, clrs, qfl, ql2, rfl, sz;

	if(!ctx->data)
		{ return(ICERR_BADFORMAT); }
	
	dsz=((lParam1->lpbiInput->biWidth+3)/4)*
		((lParam1->lpbiInput->biHeight+3)/4)*
		4*16;
//	clrs=BGBBTJ_JPG_RGBA;
//	if(lParam1->lpbiInput->biBitCount==24)
//		{ clrs=BGBBTJ_JPG_RGB; }

	clrs=BGBBTJ_JPG_BGRX;
	if(lParam1->lpbiInput->biBitCount==24)
		{ clrs=BGBBTJ_JPG_BGR; }

	//Quality level is 0-10000 for VfW, but 0-100 BTJ/BTIC
	qfl=lParam1->dwQuality/100;
	if(qfl<0)qfl=0;
	if(qfl>100)qfl=100;

	ql2=ctx->viQuality/100;
	if(ql2<qfl)
	{
		btjpg_printf("btjvfw_compress: Quality %d -> %d\n", qfl, ql2);
		qfl=ql2;
		if(qfl<0)qfl=0;
		if(qfl>100)qfl=100;
	}

//	qfl=75;

	if(lParam1->dwFlags&ICCOMPRESS_KEYFRAME)
	{
		qfl|=BGBBTJ_QFL_IFRAME;
		ctx->viNextIFrame=64;
	}
	else
	{
		if(ctx->viNextIFrame>0)
		{
			ctx->viNextIFrame--;
			qfl|=BGBBTJ_QFL_PFRAME;
		}else
		{
			qfl|=BGBBTJ_QFL_IFRAME;
			ctx->viNextIFrame=64;
		}
	}

	if(lParam1->lpbiOutput->biCompression==BGBBTJ_FCC_bt1c)
	{
//		qfl|=BGBBTJ_BTIC1C_QFL_BLOCKZI|
//			BGBBTJ_BTIC1C_QFL_MOTION|
//			BGBBTJ_BTIC1C_QFL_INDEXCLR|
//			BGBBTJ_BTIC1C_QFL_EXTMODES|
//			BGBBTJ_BTIC1C_QFL_ALTJPEG;

		qfl|=BGBBTJ_BTIC1C_QFL_BLOCKZI|
			BGBBTJ_BTIC1C_QFL_MOTION|
			BGBBTJ_BTIC1C_QFL_INDEXCLR|
			BGBBTJ_BTIC1C_QFL_DIFFCLR|
			BGBBTJ_BTIC1C_QFL_EXTMODES;
	}
	
	if(!(ctx->viFlags&BGBBTJ_VIFL_FASTENCODE) && ((qfl&255)!=100))
		{ qfl|=BGBBTJ_QFL_BATCHENCODE; }

	rfl=qfl;
	sz=BGBBTJ_Codec_CompressFrame(ctx->data,
		lParam1->lpInput,
		lParam1->lpOutput,
		dsz, qfl, clrs, &rfl);
	
	if(lParam1->lpdwFlags && (rfl&BGBBTJ_QFL_IFRAME))
	{
		*lParam1->lpdwFlags|=AVIIF_KEYFRAME;
	}
	
	lParam1->lpbiOutput->biSizeImage=sz;
	
	return(ICERR_OK);
//	return(ICERR_BADFORMAT);
}


static LRESULT btjvfw_decompress_query(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	BGBBTJ_BMPInfoHeader bmpin, bmpout;
	u32 cr, cg, cb, ca;
	int i;

	btjpg_printf("btjvfw_decompress_query: %p %p\n",
		(void *)lParam1, (void *)lParam2);
	
	if(lParam1)
	{
		btjpg_printf("\tlParam1 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam1->bmiHeader.biCompression),
			lParam1->bmiHeader.biWidth,
			lParam1->bmiHeader.biHeight,
			lParam1->bmiHeader.biPlanes,
			lParam1->bmiHeader.biBitCount);
	}

	if(lParam2)
	{
		btjpg_printf("\tlParam2 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam2->bmiHeader.biCompression),
			lParam2->bmiHeader.biWidth,
			lParam2->bmiHeader.biHeight,
			lParam2->bmiHeader.biPlanes,
			lParam2->bmiHeader.biBitCount);

		if(((lParam2->bmiHeader.biCompression==BGBBTJ_FCC_RGBA) ||
			(lParam2->bmiHeader.biCompression==BI_BITFIELDS)) &&
			(lParam2->bmiHeader.biBitCount==32))
		{
			btjpg_printf("\t\tR=%08X G=%08X B=%08X A=%08X\n",
				((u32 *)((&lParam2->bmiHeader)+1))[0],
				((u32 *)((&lParam2->bmiHeader)+1))[1],
				((u32 *)((&lParam2->bmiHeader)+1))[2],
				((u32 *)((&lParam2->bmiHeader)+1))[3]);
		}

		if(((lParam2->bmiHeader.biCompression==BGBBTJ_FCC_RGBA) ||
			(lParam2->bmiHeader.biCompression==BI_BITFIELDS)) &&
			(lParam2->bmiHeader.biBitCount==16))
		{
			btjpg_printf("\t\tR=%04X G=%04X B=%04X A=%04X\n",
				((u32 *)((&lParam2->bmiHeader)+1))[0],
				((u32 *)((&lParam2->bmiHeader)+1))[1],
				((u32 *)((&lParam2->bmiHeader)+1))[2],
				((u32 *)((&lParam2->bmiHeader)+1))[3]);
		}
	}
	
	if(lParam2)
	{
		if(lParam2->bmiHeader.biCompression &&
			(lParam2->bmiHeader.biCompression!=BI_RGB) &&
			(lParam2->bmiHeader.biCompression!=BI_BITFIELDS) &&
			(lParam2->bmiHeader.biCompression!=BGBBTJ_FCC_RGBA) &&
			(lParam2->bmiHeader.biCompression!=BGBBTJ_FCC_DXT1) &&
			(lParam2->bmiHeader.biCompression!=BGBBTJ_FCC_DXT5) &&
			(lParam2->bmiHeader.biCompression!=BGBBTJ_FCC_BC6H) &&
			(lParam2->bmiHeader.biCompression!=BGBBTJ_FCC_BC7))
		{
			btjpg_printf("\tReject Out Format\n");
			return(ICERR_BADFORMAT);
		}
		if((lParam2->bmiHeader.biBitCount!=24) &&
			(lParam2->bmiHeader.biBitCount!=32))
		{
			btjpg_printf("\tReject Out Bits\n");
			return(ICERR_BADFORMAT);
		}
		
		if((lParam2->bmiHeader.biCompression==BI_BITFIELDS) ||
			(lParam2->bmiHeader.biCompression==BGBBTJ_FCC_RGBA))
		{
			cr=((u32 *)((&lParam2->bmiHeader)+1))[0];
			cg=((u32 *)((&lParam2->bmiHeader)+1))[1];
			cb=((u32 *)((&lParam2->bmiHeader)+1))[2];
			ca=((u32 *)((&lParam2->bmiHeader)+1))[3];
			if(	((cr!=0x00FF0000) && (cr!=0x000000FF)) ||
				((cb!=0x000000FF) && (cb!=0x00FF0000)) ||
				((cg!=0x0000FF00)))
			{
				btjpg_printf("\tBitfields: Reject Out Bitfields\n");
				return(ICERR_BADFORMAT);
			}
		}
	}
	
//	return(ICERR_BADFORMAT);

	memset(&bmpin, 0, sizeof(BGBBTJ_BMPInfoHeader));
	memset(&bmpout, 0, sizeof(BGBBTJ_BMPInfoHeader));
	
	bmpin.biCompression=lParam1->bmiHeader.biCompression;
	bmpin.biWidth=lParam1->bmiHeader.biWidth;
	bmpin.biHeight=lParam1->bmiHeader.biHeight;
	bmpin.biPlanes=lParam1->bmiHeader.biPlanes;
	bmpin.biBitCount=lParam1->bmiHeader.biBitCount;

	if(lParam2)
	{
		bmpout.biWidth=lParam2->bmiHeader.biWidth;
		bmpout.biHeight=lParam2->bmiHeader.biHeight;
		bmpout.biPlanes=lParam2->bmiHeader.biPlanes;
		bmpout.biBitCount=lParam2->bmiHeader.biBitCount;
	}

	i=BGBBTJ_Codec_DecompressQuery(RIFF_TAG_BTIC, &bmpin, &bmpout);

//	i=BGBBTJ_Codec_DecompressQuery(RIFF_TAG_BTIC,
//		(BGBBTJ_BMPInfoHeader *)lParam1,
//		(BGBBTJ_BMPInfoHeader *)lParam2);
	if(i>0)
		{ return(ICERR_OK); }
	btjpg_printf("\tFailed Match\n");
	return(ICERR_BADFORMAT);
}

static LRESULT btjvfw_decompress_get_format(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	int xs, ys;

	btjpg_printf("btjvfw_decompress_get_format: %p %p\n",
		(void *)lParam1, (void *)lParam2);

	if(!lParam2)
	{
//		return(sizeof(BITMAPINFO));
		return(sizeof(BITMAPINFO)+4*sizeof(u32));
	}

	if(lParam1)
	{
		btjpg_printf("\tlParam1 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam1->bmiHeader.biCompression),
			lParam1->bmiHeader.biWidth,
			lParam1->bmiHeader.biHeight,
			lParam1->bmiHeader.biPlanes,
			lParam1->bmiHeader.biBitCount);
	}

	if(lParam2)
	{
		btjpg_printf("\tlParam2 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam2->bmiHeader.biCompression),
			lParam2->bmiHeader.biWidth,
			lParam2->bmiHeader.biHeight,
			lParam2->bmiHeader.biPlanes,
			lParam2->bmiHeader.biBitCount);
	}

//	xs=lParam2->bmiHeader.biWidth;
//	ys=lParam2->bmiHeader.biHeight;
	xs=lParam1->bmiHeader.biWidth;
	ys=lParam1->bmiHeader.biHeight;

//	lParam2->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	lParam2->bmiHeader.biSize=sizeof(BITMAPINFOHEADER)+4*sizeof(u32);

	lParam2->bmiHeader.biWidth=xs;
	lParam2->bmiHeader.biHeight=ys;
	lParam2->bmiHeader.biPlanes=1;
	lParam2->bmiHeader.biBitCount=32;
//	lParam2->bmiHeader.biCompression=BI_RGB;
	lParam2->bmiHeader.biCompression=BGBBTJ_FCC_RGBA;
//	lParam2->bmiHeader.biCompression=BI_BITFIELDS;
	lParam2->bmiHeader.biSizeImage=xs*ys*4;
	lParam2->bmiHeader.biXPelsPerMeter=0;
	lParam2->bmiHeader.biYPelsPerMeter=0;
	lParam2->bmiHeader.biClrUsed=0;
	lParam2->bmiHeader.biClrImportant=0;

	((u32 *)((&lParam2->bmiHeader)+1))[0]=0x00FF0000;
	((u32 *)((&lParam2->bmiHeader)+1))[1]=0x0000FF00;
	((u32 *)((&lParam2->bmiHeader)+1))[2]=0x000000FF;
	((u32 *)((&lParam2->bmiHeader)+1))[3]=0xFF000000;

	return(ICERR_OK);
}

static LRESULT btjvfw_decompress_begin(BGBBTJ_VidCodecCTX *ctx,
	BITMAPINFO *lParam1, BITMAPINFO *lParam2)
{
	BGBBTJ_BMPInfoHeader bmpin, bmpout;
	BGBBTJ_VidCodecCTX *ctx2;
	u32 cr, cg, cb, ca;
	u32 clrsfcc;

	btjpg_printf("btjvfw_decompress_begin: %p %p\n",
		(void *)lParam1, (void *)lParam2);

	if(lParam1)
	{
		btjpg_printf("\tlParam1 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam1->bmiHeader.biCompression),
			lParam1->bmiHeader.biWidth,
			lParam1->bmiHeader.biHeight,
			lParam1->bmiHeader.biPlanes,
			lParam1->bmiHeader.biBitCount);
	}

	clrsfcc=RIFF_TAG_BGRX;

	if(lParam2)
	{
		btjpg_printf("\tlParam2 fcc=%.4s w=%d h=%d p=%d b=%d\n",
			&(lParam2->bmiHeader.biCompression),
			lParam2->bmiHeader.biWidth,
			lParam2->bmiHeader.biHeight,
			lParam2->bmiHeader.biPlanes,
			lParam2->bmiHeader.biBitCount);

		clrsfcc=RIFF_TAG_BGRX;

		if((lParam2->bmiHeader.biCompression==BGBBTJ_FCC_DXT1) ||
			(lParam2->bmiHeader.biCompression==BGBBTJ_FCC_DXT5) ||
			(lParam2->bmiHeader.biCompression==BGBBTJ_FCC_BC6H) ||
			(lParam2->bmiHeader.biCompression==BGBBTJ_FCC_BC7))
		{
			clrsfcc=lParam2->bmiHeader.biCompression;
		}

		if((lParam2->bmiHeader.biCompression==BI_BITFIELDS) ||
			(lParam2->bmiHeader.biCompression==BGBBTJ_FCC_RGBA))
		{
			cr=((u32 *)((&lParam2->bmiHeader)+1))[0];
			cg=((u32 *)((&lParam2->bmiHeader)+1))[1];
			cb=((u32 *)((&lParam2->bmiHeader)+1))[2];
			ca=((u32 *)((&lParam2->bmiHeader)+1))[3];
//			if(	((cr!=0x00FF0000) && (cr!=0x000000FF)) ||
//				((cb!=0x000000FF) && (cb!=0x00FF0000))
//				((cg!=0x0000FF00)))

			if(	(cr==0x00FF0000UL) &&
				(cg==0x0000FF00UL) &&
				(cb==0x000000FFUL) &&
				(ca==0xFF000000UL))
			{
				clrsfcc=RIFF_TAG_BGRA;
			}

			if(	(cr==0x000000FFUL) &&
				(cg==0x0000FF00UL) &&
				(cb==0x00FF0000UL) &&
				(ca==0xFF000000UL))
			{
				clrsfcc=RIFF_TAG_RGBA;
			}

			if(	(cr==0x00FF0000UL) &&
				(cg==0x0000FF00UL) &&
				(cb==0x000000FFUL) &&
				(ca!=0xFF000000UL))
			{
				clrsfcc=RIFF_TAG_BGRX;
			}

			if(	(cr==0x000000FFUL) &&
				(cg==0x0000FF00UL) &&
				(cb==0x00FF0000UL) &&
				(ca!=0xFF000000UL))
			{
				clrsfcc=RIFF_TAG_RGBX;
			}
		}
	}

	memset(&bmpin, 0, sizeof(BGBBTJ_BMPInfoHeader));
	memset(&bmpout, 0, sizeof(BGBBTJ_BMPInfoHeader));
	
	bmpin.biCompression=lParam1->bmiHeader.biCompression;
	bmpin.biWidth=lParam1->bmiHeader.biWidth;
	bmpin.biHeight=lParam1->bmiHeader.biHeight;
	bmpin.biPlanes=lParam1->bmiHeader.biPlanes;
	bmpin.biBitCount=lParam1->bmiHeader.biBitCount;

//	bmpout.biCompression=RIFF_TAG_BGRX;
	bmpout.biCompression=clrsfcc;
	bmpout.biWidth=lParam2->bmiHeader.biWidth;
	bmpout.biHeight=lParam2->bmiHeader.biHeight;
	bmpout.biPlanes=lParam2->bmiHeader.biPlanes;
	bmpout.biBitCount=lParam2->bmiHeader.biBitCount;

//	return(ICERR_BADFORMAT);

	ctx2=BGBBTJ_Codec_BeginDecompress(RIFF_TAG_BTIC, &bmpin, &bmpout);

//	ctx2=BGBBTJ_Codec_BeginDecompress(RIFF_TAG_BTIC,
//		(BGBBTJ_BMPInfoHeader *)lParam1,
//		(BGBBTJ_BMPInfoHeader *)lParam2);
	if(!ctx2)
		{ return(ICERR_BADFORMAT); }
		
	ctx->data=ctx2;
	return(ICERR_OK);
}

static LRESULT btjvfw_decompress_end(BGBBTJ_VidCodecCTX *ctx)
{
	btjpg_printf("btjvfw_decompress_end:\n");

	if(!ctx->data)
		{ return(ICERR_BADFORMAT); }
	BGBBTJ_VidCodecCTX_EndDecompress(ctx->data);
	BGBBTJ_VidCodecCTX_Destroy(ctx->data);
	ctx->data=NULL;
	return(ICERR_OK);
}

static LRESULT btjvfw_decompress(BGBBTJ_VidCodecCTX *ctx,
	ICDECOMPRESS *lParam1)
{
	int ret;

//	btjpg_printf("btjvfw_decompress: %p\n",
//		(void *)lParam1);

	if(!ctx->data)
		{ return(ICERR_BADFORMAT); }
	
//	ret=-1;
	ret=BGBBTJ_Codec_DecompressFrame(ctx->data,
		lParam1->lpInput, lParam1->lpOutput,
		lParam1->lpbiInput->biSizeImage,
		lParam1->lpbiOutput->biSizeImage);
	if(ret<0)
		{ return(ICERR_BADFORMAT); }
	return(ICERR_OK);
}


BGBBTJ_API INT_PTR WINAPI DllMain(
		HANDLE hModule, 
		DWORD  ul_reason_for_call, 
		LPVOID lpReserved)
{
	g_hInst=(HINSTANCE)hModule;
    return(TRUE);
}

BGBBTJ_API LRESULT WINAPI DriverProc(
	DWORD_PTR dwDriverId, 
	HDRVR hDriver, 
	UINT uMsg, 
	LPARAM lParam1, 
	LPARAM lParam2) 
{
	ICOPEN * icopen;
	ICINFO *icinfo;
	BGBBTJ_VidCodecCTX *ctx;
	
	ctx=(BGBBTJ_VidCodecCTX *)dwDriverId;

//	BTJPG_DriverInit();
//	btjpg_printf("DriverProc: %d %p %p\n",
//		uMsg, (void *)lParam1, (void *)lParam2);

	switch(uMsg)
	{
	case DRV_LOAD:
	case DRV_FREE:
		return(DRVCNF_OK);

	case DRV_OPEN:
		btjpg_printf("DriverProc: DRV_OPEN: %d %p %p\n",
			uMsg, (void *)lParam1, (void *)lParam2);

		icopen=(ICOPEN *)lParam2;
		if(icopen && (icopen->fccType!=ICTYPE_VIDEO))
			{ return(DRVCNF_CANCEL); }

		if(icopen)
		{
			btjpg_printf("\tdwSize=%d\n", icopen->dwSize);
			btjpg_printf("\tfccType=0x%08X (%.4s)\n",
				icopen->fccType, &(icopen->fccType));
			btjpg_printf("\tfccHandler=0x%08X (%.4s)\n",
				icopen->fccHandler, &(icopen->fccHandler));
			btjpg_printf("\tdwVersion=0x%08X\n", icopen->dwVersion);
			btjpg_printf("\tdwFlags=0x%08X\n", icopen->dwFlags);
			btjpg_printf("\tdwError=0x%08X\n", icopen->dwError);
			btjpg_printf("\tpV1Reserved=%p\n", icopen->pV1Reserved);
			btjpg_printf("\tpV2Reserved=%p\n", icopen->pV2Reserved);
			btjpg_printf("\tdnDevNode=0x%08X\n", icopen->dnDevNode);
		}

		BTJPG_DriverInit();

//		ctx = malloc(sizeof(CODEC));
		ctx=BGBBTJ_VidCodecCTX_New();
		ctx->viFlags=0;

		if(!ctx)
		{
			if(icopen)
				{ icopen->dwError = ICERR_MEMORY; }
			return(0);
		}

//		ctx->viQuality=75*100;
		ctx->viQuality=100*100;

		if(icopen)
		{
			if((icopen->dwFlags&ICMODE_FASTCOMPRESS)==ICMODE_FASTCOMPRESS)
				{ ctx->viFlags|=BGBBTJ_VIFL_FASTENCODE; }
			if((icopen->dwFlags&ICMODE_FASTDECOMPRESS)==ICMODE_FASTDECOMPRESS)
				{ ctx->viFlags|=BGBBTJ_VIFL_FASTDECODE; }
			icopen->dwError=ICERR_OK;
		}
		return((LRESULT)ctx);

	case DRV_CLOSE :
//		compress_end(ctx);
//		decompress_end(ctx);
//		clean_dll_bindings(ctx);
//		status_destroy_always(&ctx->status);
//		free(ctx);
		BGBBTJ_VidCodecCTX_Destroy(ctx);
		BTJPG_DriverDeinit();
		return(DRVCNF_OK);

	case DRV_DISABLE:
	case DRV_ENABLE:
		return(DRVCNF_OK);

	case DRV_INSTALL:
	case DRV_REMOVE:
		return(DRVCNF_OK);

	case DRV_QUERYCONFIGURE:
	case DRV_CONFIGURE:
		return(DRVCNF_CANCEL);

	case ICM_GETINFO:
		btjpg_printf("ICM_GETINFO: %p %p\n",
			(void *)lParam1, (void *)lParam2);

		if (lParam1 && (lParam2>=sizeof(ICINFO)))
		{
			icinfo = (ICINFO *)lParam1;

			icinfo->fccType = ICTYPE_VIDEO;
			icinfo->fccHandler = RIFF_TAG_BTIC;
			icinfo->dwFlags =
				VIDCF_FASTTEMPORALC |
				VIDCF_FASTTEMPORALD |
				VIDCF_QUALITY |
				VIDCF_TEMPORAL |
				VIDCF_COMPRESSFRAMES;

			icinfo->dwVersion = 0;
#if !defined(ICVERSION)
#define ICVERSION       0x0104
#endif
			icinfo->dwVersionICM = ICVERSION;
			
//			wcscpy(icinfo->szName, XVID_NAME_L); 
//			wcscpy(icinfo->szDescription, XVID_DESC_L);

			wcscpy(icinfo->szName, L"BTIC"); 
			wcscpy(icinfo->szDescription, L"BGBTech Image Codec");
						
			return(lParam2);
		}

		return(0);
		
	case ICM_ABOUT :
		btjpg_printf("ICM_ABOUT: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		return(ICERR_UNSUPPORTED);
//		DialogBoxParam(g_hInst,
//			MAKEINTRESOURCE(IDD_ABOUT),
//			(HWND)lParam1, about_proc, 0);
//		return(ICERR_OK);

	case ICM_CONFIGURE:
	case ICM_GETSTATE:
	case ICM_SETSTATE:
		btjpg_printf("ICM_CONFIGURE: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		return(ICERR_UNSUPPORTED);

	case ICM_GET:
		btjpg_printf("ICM_GET: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		return(ICERR_UNSUPPORTED);
	case ICM_SET:
		btjpg_printf("ICM_SET: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		return(ICERR_UNSUPPORTED);

	case ICM_GETDEFAULTQUALITY:
		btjpg_printf("ICM_GETDEFAULTQUALITY: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		*(DWORD *)lParam1=90*100;
		return(ICERR_OK);
	case ICM_GETQUALITY:
		btjpg_printf("ICM_GETQUALITY: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		*(DWORD *)lParam1=ctx->viQuality;
		return(ICERR_OK);
	case ICM_SETQUALITY:
		btjpg_printf("ICM_SETQUALITY: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		ctx->viQuality=*(DWORD *)lParam1;
		btjpg_printf("ICM_SETQUALITY: Q=%d%%\n",
			ctx->viQuality/100);
		return(ICERR_OK);

	case ICM_GETBUFFERSWANTED:
		btjpg_printf("ICM_GETBUFFERSWANTED: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		ctx->viQuality=*(DWORD *)lParam1;
		return(ICERR_UNSUPPORTED);

	case ICM_GETDEFAULTKEYFRAMERATE:
		btjpg_printf("ICM_GETDEFAULTKEYFRAMERATE: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		ctx->viQuality=*(DWORD *)lParam1;
		return(ICERR_UNSUPPORTED);

	case ICM_COMPRESS_QUERY:
		return(btjvfw_compress_query(ctx,
			(BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2));
	case ICM_COMPRESS_GET_FORMAT:
		return(btjvfw_compress_get_format(ctx,
			(BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2));
	case ICM_COMPRESS_GET_SIZE:
		return(btjvfw_compress_get_size(ctx,
			(BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2));
	case ICM_COMPRESS_FRAMES_INFO:
		return(btjvfw_compress_frames_info(ctx,
			(ICCOMPRESSFRAMES *)lParam1));
	case ICM_COMPRESS_BEGIN:
		return(btjvfw_compress_begin(ctx,
			(BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2));
	case ICM_COMPRESS_END:
		return(btjvfw_compress_end(ctx));
	case ICM_COMPRESS:
		return(btjvfw_compress(ctx, (ICCOMPRESS *)lParam1));

	case ICM_DECOMPRESS_QUERY:
		return(btjvfw_decompress_query(ctx,
			(BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2));
	case ICM_DECOMPRESS_GET_FORMAT:
		return(btjvfw_decompress_get_format(ctx,
			(BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2));
	case ICM_DECOMPRESS_BEGIN:
		return(btjvfw_decompress_begin(ctx,
			(BITMAPINFO *)lParam1, (BITMAPINFO *)lParam2));
	case ICM_DECOMPRESS_END:
		return(btjvfw_decompress_end(ctx));
	case ICM_DECOMPRESS:
		return(btjvfw_decompress(ctx, (ICDECOMPRESS *)lParam1));

	case ICM_DECOMPRESS_GET_PALETTE:
		btjpg_printf("ICM_DECOMPRESS_GET_PALETTE: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		return(ICERR_UNSUPPORTED);
	case ICM_DECOMPRESS_SET_PALETTE:
		btjpg_printf("ICM_DECOMPRESS_SET_PALETTE: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		return(ICERR_UNSUPPORTED);

	case ICM_DECOMPRESSEX_QUERY:
	case ICM_DECOMPRESSEX_BEGIN:
	case ICM_DECOMPRESSEX_END:
	case ICM_DECOMPRESSEX:
		btjpg_printf("ICM_DECOMPRESSEX: %p %p\n",
			(void *)lParam1, (void *)lParam2);
		return(ICERR_UNSUPPORTED);

    /* VFWEXT entry point */
    case ICM_USER+0x0fff :
		btjpg_printf("ICM_USER: %p %p\n",
			(void *)lParam1, (void *)lParam2);
        return(ICERR_UNSUPPORTED);

	default:
		btjpg_printf("Default: %d %p %p\n", uMsg,
			(void *)lParam1, (void *)lParam2);
		if(uMsg<DRV_USER)
		{
			return(DefDriverProc(dwDriverId,
				hDriver, uMsg, lParam1, lParam2));
		}
		else 
			{ return(ICERR_UNSUPPORTED); }
	}
}

BGBBTJ_API void WINAPI Configure(
	HWND hwnd,
	HINSTANCE hinst,
	LPTSTR lpCmdLine,
	int nCmdShow)
{
	LRESULT dwDriverId;

	dwDriverId=(LRESULT)DriverProc(0, 0, DRV_OPEN, 0, 0);
	if(dwDriverId!=(LRESULT)NULL)
	{
		if(!lstrcmpi(lpCmdLine, "about"))
		{
			DriverProc(dwDriverId, 0, ICM_ABOUT,
				(LPARAM)GetDesktopWindow(), 0);
		}else
		{
			DriverProc(dwDriverId, 0, ICM_CONFIGURE,
				(LPARAM)GetDesktopWindow(), 0);
		}
		DriverProc(dwDriverId, 0, DRV_CLOSE, 0, 0);
	}
}


#endif //BGBBTJ_VFWDRV

#endif //_WIN32
