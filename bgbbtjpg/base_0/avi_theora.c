#include <bgbbtj.h>

#ifndef BGBBTJ_DRV

#include <theora/theora.h>
#include <theora/theoradec.h>
#include <theora/theoraenc.h>

typedef struct {
// HIC hicd;
BGBBTJ_BMPInfoHeader *ihead;
BGBBTJ_BMPInfoHeader *ohead;
void *buffer;
void *vidCtx;

theora_info ti;
theora_state th;
th_info thi;
th_enc_ctx *td;
th_comment tc;

th_ycbcr_buffer ycbbuf;
}vfw_ctxinfo;

int BGBBTJ_THEO_DecompressFrame(BGBBTJ_AVI_Context *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	byte *sbuf, *tbuf;
	int xs, ys;

	if(!ctx->mjpg_cty)
	{
//		ctx->mjpg_cty=PDJPG_DecodeComponentType(src, ssz);
//		ctx->mjpg_cty=BGBBTJ_JPG_DecodeComponentType(src, ssz);
		if(!ctx->mjpg_cty)
			ctx->mjpg_cty=BGBBTJ_JPG_CTY_RGB;
	}

	if(!dst)return(0);

	xs=ctx->bmihead->biWidth;
	ys=ctx->bmihead->biHeight;
	info=ctx->codec_ctx->data;

//	BGBBTJ_BufPNG_DecodeOBuf(
//		src, ssz, dst, &xs, &ys);

	return(0);
}

int theo_decompress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int ssz, int dsz)
{
	vfw_ctxinfo *info;
	int err, i, j, k, ib, ob, xs, ys;
	byte *sbuf, *tbuf;

	info=ctx->data;

	if(!dst)return(0);

//	if(!info->vidCtx)
//		info->vidCtx=BTIC1_AllocContext();

	i=1<<20;	//dummy...

//	BGBBTJ_BufPNG_DecodeOBuf(
//		src, ssz, dst, &xs, &ys);

	return(0);
}

static BGBBTJ_VidCodecCTX *theo_begin_decompress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
//	byte tbuf[16384];
	BGBBTJ_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
//	byte *ct;
	int err;

	if((fcc!=RIFF_TAG_THEO) && (fcc!=RIFF_TAG_theo) &&
			(fcc!=RIFF_TAG_ther))
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
	info=gcalloc(sizeof(vfw_ctxinfo));
	ctx->data=info;

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

//	info->buffer=malloc(out->width*out->height*out->bpp/8);

	ctx->decompress_frame=&theo_decompress_frame;

	return(ctx);
}

int theo_compress_frame(BGBBTJ_VidCodecCTX *ctx,
	void *src, void *dst, int dsz, int qfl, int clrs, int *rfl)
{
	vfw_ctxinfo *info;
	ogg_packet op;
	int err, i, j, k, ib, ob, w, h, xs, ys, xs1, ys1;
	int cr0, cg0, cb0, ca0;
	int cr1, cg1, cb1, ca1;
	int cr2, cg2, cb2, ca2;
	int cr3, cg3, cb3, ca3;
	int i0, i1;
	int cy0, cy1, cy2, cy3;
	int cu0, cu1, cu2, cu3;
	int cv0, cv1, cv2, cv3;
	int cu, cv, ca;
	byte *sbuf, *tbuf, *ct;

	info=ctx->data;

//	printf("theo_compress_frame: Debug\n");

	if(!dst)
	{
		printf("theo_compress_frame: No Dest\n");
		return(0);
	}

	w=info->ohead->biWidth;
	h=info->ohead->biHeight;
	xs=(w+15)&(~15);
	ys=(h+15)&(~15);
	xs1=xs>>1;
	ys1=ys>>1;

	sbuf=src;
	for(i=0; i<ys1; i++)
		for(j=0; j<xs1; j++)
	{
//		i0=((i*2+0)*w;
//		i1=((i*2+1)*w;

		i0=(ys-i*2-1)*w;
		i1=(ys-i*2-2)*w;

		cr0=sbuf[(i0+(j*2+0))*4+0];	cr1=sbuf[(i0+(j*2+1))*4+0];
		cr2=sbuf[(i1+(j*2+0))*4+0];	cr3=sbuf[(i1+(j*2+1))*4+0];
		cg0=sbuf[(i0+(j*2+0))*4+1];	cg1=sbuf[(i0+(j*2+1))*4+1];
		cg2=sbuf[(i1+(j*2+0))*4+1];	cg3=sbuf[(i1+(j*2+1))*4+1];
		cb0=sbuf[(i0+(j*2+0))*4+2];	cb1=sbuf[(i0+(j*2+1))*4+2];
		cb2=sbuf[(i1+(j*2+0))*4+2];	cb3=sbuf[(i1+(j*2+1))*4+2];
		ca0=sbuf[(i0+(j*2+0))*4+3];	ca1=sbuf[(i0+(j*2+1))*4+3];
		ca2=sbuf[(i1+(j*2+0))*4+3];	ca3=sbuf[(i1+(j*2+1))*4+3];

		ca=(ca0+ca1+ca2+ca3)>>2;

		BGBBTJ_SuperMagenta8_ConvRGBA2YUV(cr0, cg0, cb0, ca,
			&cy0, &cu0, &cv0);
		BGBBTJ_SuperMagenta8_ConvRGBA2YUV(cr1, cg1, cb1, ca,
			&cy1, &cu1, &cv1);
		BGBBTJ_SuperMagenta8_ConvRGBA2YUV(cr2, cg2, cb2, ca,
			&cy2, &cu2, &cv2);
		BGBBTJ_SuperMagenta8_ConvRGBA2YUV(cr3, cg3, cb3, ca,
			&cy3, &cu3, &cv3);

#if 0
		cy0=0.299*cr0+0.587*cg0+0.114*cb0;
		cy1=0.299*cr1+0.587*cg1+0.114*cb1;
		cy2=0.299*cr2+0.587*cg2+0.114*cb2;
		cy3=0.299*cr3+0.587*cg3+0.114*cb3;

		cu0=-0.1687*cr0-0.3313*cg0+0.5*cb0+128;
		cu1=-0.1687*cr1-0.3313*cg1+0.5*cb1+128;
		cu2=-0.1687*cr2-0.3313*cg2+0.5*cb2+128;
		cu3=-0.1687*cr3-0.3313*cg3+0.5*cb3+128;

		cv0=0.5*cr0-0.4187*cg0-0.0813*cb0+128;
		cv1=0.5*cr1-0.4187*cg1-0.0813*cb1+128;
		cv2=0.5*cr2-0.4187*cg2-0.0813*cb2+128;
		cv3=0.5*cr3-0.4187*cg3-0.0813*cb3+128;
#endif

		cu=((cu0+cu1+cu2+cu3)>>2);
		cv=((cv0+cv1+cv2+cv3)>>2);

		cy0=(cy0<0)?0:((cy0>255)?255:cy0);
		cy1=(cy1<0)?0:((cy1>255)?255:cy1);
		cy2=(cy2<0)?0:((cy2>255)?255:cy2);
		cy3=(cy3<0)?0:((cy3>255)?255:cy3);

		cu=(cu<0)?0:((cu>255)?255:cu);
		cv=(cv<0)?0:((cv>255)?255:cv);

//		cy0=(cr0+2*cg0+cb0)>>2;
//		cy1=(cr1+2*cg1+cb1)>>2;
//		cy2=(cr2+2*cg2+cb2)>>2;
//		cy3=(cr3+2*cg3+cb3)>>2;
		
//		k=((cg0+cg1+cg2+cg3)>>2);
//		k=((cy0+cy1+cy2+cy3)>>2);
//		cu=(((((cb0+cb1+cb2+cb3)>>2)-k)*3+8)>>4)+128;
//		cv=(((((cr0+cr1+cr2+cr3)>>2)-k)*5+8)>>4)+128;

		info->ycbbuf[0].data[(i*2+0)*xs+j*2+0]=cy0;
		info->ycbbuf[0].data[(i*2+0)*xs+j*2+1]=cy1;
		info->ycbbuf[0].data[(i*2+1)*xs+j*2+0]=cy2;
		info->ycbbuf[0].data[(i*2+1)*xs+j*2+1]=cy3;
		info->ycbbuf[1].data[i*xs1+j]=cu;
		info->ycbbuf[2].data[i*xs1+j]=cv;
	}

	err=th_encode_ycbcr_in(info->td, info->ycbbuf);
	if(err)
	{
		printf("theo_compress_frame: Encode Error=%d\n", err);
	}
	
	tbuf=dst;
	ct=tbuf;
	while(1)
	{
		err=th_encode_packetout(info->td, 0, &op);
		if(err<=0)break;
		memcpy(ct, op.packet, op.bytes);
		ct+=op.bytes;
	}
	
	if(err<0)
	{
		printf("theo_compress_frame: Err=%d Sz=%d  \n", err, ct-tbuf);
	}

	if(rfl)
	{
		qfl&=(~(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME));
		if(tbuf[0]&0x40)
			{ qfl|=BGBBTJ_QFL_PFRAME; }
		else { qfl|=BGBBTJ_QFL_IFRAME; }
		*rfl=qfl;
	}

//	printf("theo_compress_frame: Sz=%d  \n", ct-tbuf);
	
	return(ct-tbuf);
}

static BGBBTJ_VidCodecCTX *theo_begin_compress(int fcc,
	BGBBTJ_BMPInfoHeader *in, BGBBTJ_BMPInfoHeader *out)
{
	byte tbuf[16384];
	ogg_packet op;
	BGBBTJ_VidCodecCTX *ctx;
	vfw_ctxinfo *info;
	byte *ct;
	int err, sz, xs, ys, xs1, ys1, px, py;

	if((fcc!=RIFF_TAG_THEO) && (fcc!=RIFF_TAG_theo) &&
			(fcc!=RIFF_TAG_ther))
		return(NULL);

	ctx=BGBBTJ_VidCodecCTX_New();
	info=gcalloc(sizeof(vfw_ctxinfo));
	ctx->data=info;

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

//	out->biCompression=RIFF_TAG_ther;
	out->biCompression=RIFF_TAG_theo;

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

//	info->buffer=malloc(out->width*out->height*out->bpp/8);

	theora_info_init(&(info->ti));
//	if(err<0)printf("theo_begin_compress: A Err=%d\n");

	xs=(out->biWidth+15)&(~15);
	ys=(out->biHeight+15)&(~15);
	xs1=xs>>1;
	ys1=ys>>1;

    px=((xs-out->biWidth)>>1)&(~1);
    py=((ys-out->biHeight)>>1)&(~1);

	info->ti.width = out->biWidth;
	info->ti.height = out->biHeight;
	info->ti.frame_width = xs;
	info->ti.frame_height = ys;
    info->ti.offset_x=px;
	info->ti.offset_y=py;
//	info->ti.pixel_fmt=TH_PF_420;
	info->ti.pixelformat=TH_PF_420;
//	info->ti.quality=56;
	info->ti.quality=63;
	info->ti.colorspace=TH_CS_UNSPECIFIED;

	info->ti.fps_numerator=24;
	info->ti.fps_denominator=1;
	info->ti.aspect_numerator=1;
	info->ti.aspect_denominator=1;

	th_info_init(&(info->thi));
//	if(err<0)printf("theo_begin_compress: B Err=%d\n");

	info->thi.pic_width = out->biWidth;
	info->thi.pic_height = out->biHeight;
	info->thi.frame_width = xs;
	info->thi.frame_height = ys;
    info->thi.pic_x=px;
	info->thi.pic_y=py;
	info->thi.fps_numerator=24;
	info->thi.fps_denominator=1;

	info->thi.aspect_numerator=1;
	info->thi.aspect_denominator=1;
	
	info->thi.colorspace=TH_CS_UNSPECIFIED;
	info->thi.pixel_fmt=TH_PF_420;
//	info->thi.pixelformat=TH_PF_420;
//	info->thi.quality=48;
	info->thi.quality=63;

	if(theora_encode_init(&(info->th), &(info->ti)) != OC_DISABLED)
		{ theora_clear(&(info->th)); }

	info->td=th_encode_alloc(&(info->thi));
	th_info_clear(&(info->thi));
//	if(err<0)printf("theo_begin_compress: C Err=%d\n");

	th_comment_init(&(info->tc));
//	if(err<0)printf("theo_begin_compress: D Err=%d\n");

	ct=tbuf;
	
    err=th_encode_flushheader(info->td, &(info->tc), &op);
	if(err<0)printf("theo_begin_compress: E Err=%d\n");

	ct[0]=(op.bytes>>8)&255;
	ct[1]=op.bytes&255;
	ct+=2;

    memcpy(ct, op.packet, op.bytes);
    ct+=op.bytes;

    while(1)
    {
		err=th_encode_flushheader(info->td, &(info->tc), &op);
		if(err<=0)break;

		ct[0]=(op.bytes>>8)&255;
		ct[1]=op.bytes&255;
		ct+=2;

		memcpy(ct, op.packet, op.bytes);
		ct+=op.bytes;
    }

	if(err<0)printf("theo_begin_compress: F Err=%d\n");

	sz=ct-tbuf;
	ctx->vidStrd=gcalloc(sz);
	ctx->sz_vidStrd=sz;
	memcpy(ctx->vidStrd, tbuf, sz);

	info->ycbbuf[0].width=xs;
	info->ycbbuf[0].height=ys;
	info->ycbbuf[0].stride=xs;
	info->ycbbuf[0].data=malloc(xs*ys);

	info->ycbbuf[1].width=xs1;
	info->ycbbuf[1].height=ys1;
	info->ycbbuf[1].stride=xs1;
	info->ycbbuf[1].data=malloc(xs1*ys1);

	info->ycbbuf[2].width=xs1;
	info->ycbbuf[2].height=ys1;
	info->ycbbuf[2].stride=xs1;
	info->ycbbuf[2].data=malloc(xs1*ys1);

	ctx->compress_frame=&theo_compress_frame;

	return(ctx);
}

int BGBBTJ_CodecTHEO_Init()
{
	BGBBTJ_VidCodec *codec;
	int i, j;
	short *sw;
	char buf[256];
	char *t;

	printf("THEO: Init\n");

	codec=BGBBTJ_VidCodec_New();
	codec->begin_decompress=&theo_begin_decompress;
	codec->begin_compress=&theo_begin_compress;
	return(0);
}

#else

int BGBBTJ_CodecTHEO_Init()
{
	return(-1);
}

#endif