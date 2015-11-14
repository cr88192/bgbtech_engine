#include <lbxgl2.h>

#ifdef _WIN32

#include <windows.h>
#include <vfw.h>

#define CATCH_BLOCK_SEH

typedef struct {
HIC hicd;
BITMAPINFOHEADER *ihead;
BITMAPINFOHEADER *ohead;
void *buffer;
}vfw_ctxinfo;

HIC vfw_hicd[256];
int vfw_n_hicd=0;

void LBXGL_ConvBGR2RGB(byte *sbuf, byte *tbuf, int sz)
{
	int i;
	
	for(i=0; i<sz; i++)
	{
		tbuf[i*3+0]=sbuf[i*3+2];
		tbuf[i*3+1]=sbuf[i*3+1];
		tbuf[i*3+2]=sbuf[i*3+0];
	}
}

void LBXGL_ConvBGRA2RGBA(byte *sbuf, byte *tbuf, int sz)
{
	int i;
	
	for(i=0; i<sz; i++)
	{
		tbuf[i*4+0]=sbuf[i*4+2];
		tbuf[i*4+1]=sbuf[i*4+1];
		tbuf[i*4+2]=sbuf[i*4+0];
//		tbuf[i*4+3]=sbuf[i*4+3];
		tbuf[i*4+3]=255;
	}
}

static int vfw_decompress_frame(LBXGL_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	int err, i, j, k, ib, ob;
	byte *sbuf, *tbuf;

	info=ctx->data;

	info->ihead->biSizeImage=ssz;
	info->ohead->biSizeImage=dsz;

//	memset(dst, 255, info->ohead->biWidth*info->ohead->biHeight*4);

#ifdef CATCH_BLOCK_SEH
	__try {
#endif

	i=(!dst)?ICDECOMPRESS_PREROLL:0;	//frame skip
//	i=ICDECOMPRESS_NOTKEYFRAME;
//	err=ICDecompress(info->hicd, 0, info->ihead, src, info->ohead, dst);
	err=ICDecompress(info->hicd, i, info->ihead, src,
		info->ohead, info->buffer);

#ifdef CATCH_BLOCK_SEH
	} __except(EXCEPTION_EXECUTE_HANDLER)
	{
		printf("RECAUGHT VFW\n");
		return(-1);
	}
#endif

	if(dst)
	{
		sbuf=info->buffer;
		tbuf=dst;

		j=info->ohead->biHeight;
		if(j<0)j=-j;
		j=info->ohead->biWidth*j;
		k=info->ohead->biBitCount/8;

		if(k==3)LBXGL_ConvBGR2RGB(sbuf, tbuf, j);
		if(k==4)LBXGL_ConvBGRA2RGBA(sbuf, tbuf, j);
	}

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

	if(err!=ICERR_OK)printf("VFW: decompress %d\n", err);

	if(err!=ICERR_OK)return(-1);
	return(0);
}

static LBXGL_VidCodecCTX *vfw_begin_decompress(int fcc,
	PDGL_BMPInfoHeader *in, PDGL_BMPInfoHeader *out)
{
	LBXGL_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
	int err;

	ctx=LBXGL_VidCodecCTX_New();
	info=gcalloc(sizeof(vfw_ctxinfo));
	ctx->data=info;

	info->ihead=malloc(sizeof(BITMAPINFOHEADER));
	memset(info->ihead, 0, sizeof(BITMAPINFOHEADER));
	info->ihead->biSize		= sizeof(BITMAPINFOHEADER);
	info->ihead->biWidth		= in->biWidth;
	info->ihead->biHeight		= in->biHeight;
	info->ihead->biPlanes		= in->biPlanes;
	info->ihead->biBitCount		= in->biBitCount;
	info->ihead->biCompression	= in->biCompression;
	info->ihead->biSizeImage	=
		in->biWidth*in->biHeight*in->biBitCount/8;

	info->ohead=malloc(sizeof(BITMAPINFOHEADER));
	memset(info->ohead, 0, sizeof(BITMAPINFOHEADER));
	info->ohead->biSize		= sizeof(BITMAPINFOHEADER);
	info->ohead->biWidth		= out->biWidth;
//	info->ohead->biHeight		= -out->biHeight;
	info->ohead->biHeight		= out->biHeight;
	info->ohead->biPlanes		= out->biPlanes;
	info->ohead->biBitCount		= out->biBitCount;
	info->ohead->biCompression	= out->biCompression;
	info->ihead->biSizeImage	=
		out->biWidth*out->biHeight*out->biBitCount/8;

	info->buffer=malloc(out->biWidth*out->biHeight*out->biBitCount/8);

//	info->hicd = ICOpen(ICTYPE_VIDEO, fcc, ICMODE_FASTDECOMPRESS);
	info->hicd = ICLocate(ICTYPE_VIDEO, fcc,
		info->ihead, info->ohead, ICMODE_FASTDECOMPRESS);
	if(!info->hicd)
	{
		printf("VFW: decompress %0.4s %0.4s: fail\n",
			&fcc, &in->biCompression);
		return(NULL);
	}
	printf("VFW: decompress %0.4s %0.4s: ok\n", &fcc, &in->biCompression);


	err=ICDecompressBegin(info->hicd, info->ihead, info->ohead);
	if(err)
	{
		printf("VFW: err %d, %0.4s %0.4s: fail\n", err,
			&fcc, &in->biCompression);
		return(NULL);
	}

	vfw_hicd[vfw_n_hicd++]=info->hicd;

	ctx->decompress_frame=&vfw_decompress_frame;

	return(ctx);
}

static void vfw_shutdown()
{
	int i;
	
	for(i=0; i<vfw_n_hicd; i++)
		ICDecompressEnd(vfw_hicd[i]);
}

int LBXGL_CodecVFW_Init()
{
	LBXGL_VidCodec *codec;
	ICINFO *info;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("VFW: Init\n");

	codec=LBXGL_VidCodec_New();
	codec->begin_decompress=&vfw_begin_decompress;

	PDGL_RegisterShutdown(&vfw_shutdown);

#if 1
	info=malloc(sizeof(ICINFO));
	memset(info, 0, sizeof(ICINFO));

	for(i=0; i<1024; i++)
	{
//		info->dwSize=sizeof(ICINFO);
		j=ICInfo(0, i, info);
		if(j!=1)break;
//		kprint("%d: %d %p\n", i, j, info);
//		kprint("dwSize: %d (%d)\n", info->dwSize, sizeof(ICINFO));
		printf("fccType: %04.4s\n", &info->fccType);
		printf("fccHandler: %04.4s\n", &info->fccHandler);
//		kprint("dwFlags: %d\n", info->dwFlags);
		if(info->szName[0])
			printf("szName: %S\n", info->szName);
		if(info->szDescription[0])
			printf("szDescription: %S\n", info->szDescription);
	}
#endif

	return(0);
}

#else

int LBXGL_CodecVFW_Init()
{
	return(-1);
}

#endif
