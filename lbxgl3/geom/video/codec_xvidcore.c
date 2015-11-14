#include <lbxgl.h>
#include <xvid.h>

typedef struct {
HIC hicd;
BITMAPINFOHEADER *ihead;
BITMAPINFOHEADER *ohead;
void *buffer;
}vfw_ctxinfo;

static xvid_gbl_init_t avi_xinit;
static int xerr;

static int vfw_decompress_frame(void *src, void *dst, LBXGL_VidCodecCTX *ctx)
{
	vfw_ctxinfo *info;
	int err, i, j, k, ib, ob;
	byte *sbuf, *tbuf;

	info=ctx->data;

//	memset(dst, 255, info->ohead->biWidth*info->ohead->biHeight*4);

	i=0;
//	i=ICDECOMPRESS_NOTKEYFRAME;
//	err=ICDecompress(info->hicd, 0, info->ihead, src, info->ohead, dst);
	err=ICDecompress(info->hicd, 0, info->ihead, src,
		info->ohead, info->buffer);

	sbuf=info->buffer;
	tbuf=dst;

	j=info->ohead->biHeight;
	if(j<0)j=-j;
	j=info->ohead->biWidth*j;
	k=info->ohead->biBitCount/8;

	if(k==3)LBXGL_ConvBGR2RGB(sbuf, tbuf, j);
	if(k==4)LBXGL_ConvBGRA2RGBA(sbuf, tbuf, j);

#if 0
	j=info->ohead->biHeight;
	if(j<0)j=-j;
	j=info->ohead->biWidth*j;
	k=info->ohead->biBitCount/8;
	for(i=0; i<j; i++)
	{
		tbuf[0]=sbuf[2];
		tbuf[1]=sbuf[1];
		tbuf[2]=sbuf[0];
		tbuf[3]=255;
		sbuf+=k;
		tbuf+=k;
	}
#endif

#if 0
	k=info->ohead->biBitCount/8;
	for(i=0; i<info->ohead->biHeight; i++)
	{
		ib=((info->ohead->biHeight-i-1)*info->ohead->biWidth)*k;
		ob=(i*info->ohead->biWidth)*k;
		for(j=0; j<info->ohead->biWidth; j++)
		{
			tbuf[ob+0]=sbuf[ib+2];
			tbuf[ob+1]=sbuf[ib+1];
			tbuf[ob+2]=sbuf[ib+0];
			if(k==4)tbuf[ob+3]=255;
			ib+=k;
			ob+=k;
		}
	}
#endif

	if(err!=ICERR_OK)kprint("VFW: decompress %d\n", err);

	if(err!=ICERR_OK)return(-1);
	return(0);
}

static LBXGL_VidCodecCTX *vfw_begin_decompress(int fcc,
	BMPInfoHeader *in, BMPInfoHeader *out)
{
	LBXGL_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
	int err;

	ctx=LBXGL_VidCodecCTX_New();
	info=kalloc(sizeof(vfw_ctxinfo));
	ctx->data=info;

	info->ihead=malloc(sizeof(BITMAPINFOHEADER));
	memset(info->ihead, 0, sizeof(BITMAPINFOHEADER));
	info->ihead->biSize		= sizeof(BITMAPINFOHEADER);
	info->ihead->biWidth		= in->width;
	info->ihead->biHeight		= in->height;
	info->ihead->biPlanes		= in->planes;
	info->ihead->biBitCount		= in->bpp;
	info->ihead->biCompression	= in->compression;
	info->ihead->biSizeImage	= in->width*in->height*in->bpp/8;

	info->ohead=malloc(sizeof(BITMAPINFOHEADER));
	memset(info->ohead, 0, sizeof(BITMAPINFOHEADER));
	info->ohead->biSize		= sizeof(BITMAPINFOHEADER);
	info->ohead->biWidth		= out->width;
	info->ohead->biHeight		= -out->height;
	info->ohead->biPlanes		= out->planes;
	info->ohead->biBitCount		= out->bpp;
	info->ohead->biCompression	= out->compression;
	info->ihead->biSizeImage	= out->width*out->height*out->bpp/8;

	info->buffer=malloc(out->width*out->height*out->bpp/8);

//	info->hicd = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_FASTDECOMPRESS);
	info->hicd = ICLocate(ICTYPE_VIDEO, fcc,
		info->ihead, info->ohead, ICMODE_FASTDECOMPRESS);
	if(!info->hicd)
	{
		kprint("VFW: decompress %0.4s %0.4s: fail\n",
			&fcc, &in->compression);
		return(NULL);
	}
	kprint("VFW: decompress %0.4s %0.4s: ok\n", &fcc, &in->compression);


	err=ICDecompressBegin(info->hicd, info->ihead, info->ohead);
	if(err)
	{
		kprint("VFW: err %d, %0.4s %0.4s: fail\n", err,
			&fcc, &in->compression);
		return(NULL);
	}

	ctx->decompress_frame=&vfw_decompress_frame;

	return(ctx);
}

int LBXGL_CodecVFW_Init()
{
	LBXGL_VidCodec *codec;
	ICINFO *info;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	kprint("VFW: Init\n");

	codec=LBXGL_VidCodec_New();
	codec->begin_decompress=&vfw_begin_decompress;

#if 0
	info=malloc(sizeof(ICINFO));
	memset(info, 0, sizeof(ICINFO));

	for(i=0; i<100; i++)
	{
//		info->dwSize=sizeof(ICINFO);
		j=ICInfo(0, i, info);
		if(j!=1)break;
//		kprint("%d: %d %p\n", i, j, info);
//		kprint("dwSize: %d (%d)\n", info->dwSize, sizeof(ICINFO));
		kprint("fccType: %04.4s\n", &info->fccType);
		kprint("fccHandler: %04.4s\n", &info->fccHandler);
//		kprint("dwFlags: %d\n", info->dwFlags);
		if(info->szName[0])
			kprint("szName: %S\n", info->szName);
		if(info->szDescription[0])
			kprint("szDescription: %S\n", info->szDescription);
	}
#endif
}
