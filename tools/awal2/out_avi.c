#include <walpng.h>

#ifndef PDGL_API
#define PDGL_API
#endif

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

#define FCC_FORM	FOURCC('F', 'O', 'R', 'M')
#define FCC_RIFF	FOURCC('R', 'I', 'F', 'F')
#define FCC_RIFX	FOURCC('R', 'I', 'F', 'X')

#define FCC_LIST	FOURCC('L', 'I', 'S', 'T')
#define FCC_AVI		FOURCC('A', 'V', 'I', ' ')

#define FCC_hdrl	FOURCC('h', 'd', 'r', 'l')
#define FCC_strl	FOURCC('s', 't', 'r', 'l')
#define FCC_strh	FOURCC('s', 't', 'r', 'h')
#define FCC_strf	FOURCC('s', 't', 'r', 'f')
#define FCC_strd	FOURCC('s', 't', 'r', 'd')

#define FCC_avih	FOURCC('a', 'v', 'i', 'h')
#define FCC_movi	FOURCC('m', 'o', 'v', 'i')
#define FCC_idx1	FOURCC('i', 'd', 'x', '1')

#define FCC_vids	FOURCC('v', 'i', 'd', 's')
#define FCC_auds	FOURCC('a', 'u', 'd', 's')

#define FCC_MJPG	FOURCC('M', 'J', 'P', 'G')
#define FCC_JPEG	FOURCC('J', 'P', 'E', 'G')

#define FCC_RGBA	FOURCC('R', 'G', 'B', 'A')
#define FCC_RGBT	FOURCC('R', 'G', 'B', 'T')

#define FCC_BtV0	FOURCC('B', 't', 'V', '0')

#define FCC_00dc	FOURCC('0', '0', 'd', 'c')
#define FCC_01wb	FOURCC('0', '1', 'w', 'b')

volatile PDGL_AVICtx *pdgl_avi_cap;

int pdgl_avi_nbase=0;
int pdgl_avi_nseq=0;

volatile int pdgl_avi_encloop=0;
volatile int pdgl_avi_enctime=0;

void *pdgl_avi_malloc(int sz)
{
	void *p;
	
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

void PDGL_AVI_UpdateSize(VFILE *fd, int o)
{
	int i;

	i=vftell(fd);
	vfseek(fd, o, 0);
	PDPNG_WriteInt32LE(fd, i-o-4);
	vfseek(fd, i, 0);
}

PDGL_AVICtx *PDGL_AVI_OpenOutStream(char *name,
	int w, int h, int fps, int fcc)
{
	byte buf[512];

	VFILE *fd;
	PDGL_AVICtx *ctx;
	int i, j, k;

	memset(buf, 0, 512);


	fd=vffopen(name, "w+b");
	if(!fd)return(NULL);

	ctx=gcalloc(sizeof(PDGL_AVICtx));
	ctx->fd=fd;

	ctx->index=malloc(4096*sizeof(uint));
	ctx->n_index=0;
	ctx->m_index=4096/4;
	ctx->fps=fps;
	ctx->xs=w;
	ctx->ys=h;

	ctx->avih=gcalloc(sizeof(PDGL_MainAVIHeader));
	ctx->vidh=gcalloc(sizeof(PDGL_AVIStreamHeader));
	ctx->vidf=gcalloc(sizeof(PDGL_BMPInfoHeader));


	ctx->vidh->fccType=FCC_vids;

	printf("PDGL_AVI_OpenOutStream: fcc=%08X\n", fcc);

	if(fcc)
	{
		ctx->vidh->fccHandler=fcc;
	}else
	{
		ctx->vidh->fccHandler=FCC_MJPG;
//		ctx->vidh->fccHandler=FCC_RGBT;
//		ctx->vidh->fccHandler=FCC_BtV0;
	}

	ctx->vidh->dwFlags=0;
	ctx->vidh->dwRate=fps;
	ctx->vidh->dwScale=1;
	ctx->vidh->dwSuggestedBufferSize=0;
	ctx->vidh->dwQuality=0;

	ctx->vidf->biSize=sizeof(PDGL_BMPInfoHeader);
	ctx->vidf->biWidth=w;
	ctx->vidf->biHeight=h;
	ctx->vidf->biPlanes=1;
	ctx->vidf->biXPelsPerMeter=0;
	ctx->vidf->biYPelsPerMeter=0;

#if 0
	ctx->vidf->biBitCount=24;
//	ctx->vidf->biBitCount=16;
	ctx->vidf->biCompression=FCC_JPEG;
//	ctx->vidf->biCompression=FCC_RGBT;
	ctx->vidf->biSizeImage=w*h*3;
//	ctx->vidf->biSizeImage=w*h*2;
#endif

	if(ctx->vidh->fccHandler==FCC_MJPG)
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biCompression=FCC_JPEG;
		ctx->vidf->biSizeImage=w*h*3;
	}

	if(ctx->vidh->fccHandler==FCC_RGBT)
	{
		ctx->vidf->biBitCount=16;
		ctx->vidf->biSizeImage=w*h*2;

//		ctx->vidf->biBitCount=24;
//		ctx->vidf->biCompression=FCC_JPEG;
//		ctx->vidf->biCompression=FCC_RGBT;
//		ctx->vidf->biSizeImage=w*h*3;
	}

	if(ctx->vidh->fccHandler==FCC_BtV0)
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biCompression=FCC_BtV0;
		ctx->vidf->biSizeImage=w*h*3;
	}

#if 0
	ctx->audh=gcalloc(sizeof(PDGL_AVIStreamHeader));
	ctx->audf=gcalloc(sizeof(PDGL_WaveFmt));

	ctx->audh->fccType=FCC_auds;
	ctx->audh->fccHandler=1;
	ctx->audh->dwFlags=0;
	ctx->audh->dwRate=44100;
	ctx->audh->dwScale=1;
	ctx->audh->dwSuggestedBufferSize=0;
	ctx->audh->dwQuality=0;

	ctx->audf->wFormatTag=1;
	ctx->audf->nChannels=1;
	ctx->audf->nSamplesPerSec=44100;
	ctx->audf->nAvgBytesPerSec=44100;
	ctx->audf->nBlockAlign=1;
	ctx->audf->wBitsPerSample=8;
	ctx->audf->cbSize=0;
#endif

	ctx->avih->dwMicroSecPerFrame=1000000/fps;
	ctx->avih->dwMaxBytesPerSec=0;
	ctx->avih->dwFlags=16;
	ctx->avih->dwStreams=ctx->audh?2:1;
	ctx->avih->dwSuggestedBufferSize=0;
	ctx->avih->dwWidth=w;
	ctx->avih->dwHeight=h;
	ctx->avih->dwRate=fps;
	ctx->avih->dwScale=1;


	PDPNG_WriteFourcc(ctx->fd, FCC_RIFF);
	ctx->osz_avi=vftell(ctx->fd);
	PDPNG_WriteInt32LE(ctx->fd, 0);
	PDPNG_WriteFourcc(ctx->fd, FCC_AVI);

	PDPNG_WriteFourcc(ctx->fd, FCC_LIST);
	ctx->osz_hdrl=vftell(ctx->fd);
	PDPNG_WriteInt32LE(ctx->fd, 0);
	PDPNG_WriteFourcc(ctx->fd, FCC_hdrl);

	i=sizeof(PDGL_MainAVIHeader);
	PDPNG_WriteFourcc(ctx->fd, FCC_avih);
	PDPNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_avih=vftell(ctx->fd);
	vfwrite(ctx->avih, 1, (i&1)?(i+1):i, ctx->fd);


	PDPNG_WriteFourcc(ctx->fd, FCC_LIST);
	ctx->osz_strl=vftell(ctx->fd);
	PDPNG_WriteInt32LE(ctx->fd, 0);
	PDPNG_WriteFourcc(ctx->fd, FCC_strl);

	i=sizeof(PDGL_AVIStreamHeader);
	PDPNG_WriteFourcc(ctx->fd, FCC_strh);
	PDPNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_vidh=vftell(ctx->fd);
	vfwrite(ctx->vidh, 1, (i&1)?(i+1):i, ctx->fd);

	i=sizeof(PDGL_BMPInfoHeader);
	PDPNG_WriteFourcc(ctx->fd, FCC_strf);
	PDPNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_vidf=vftell(ctx->fd);
	vfwrite(ctx->vidf, 1, (i&1)?(i+1):i, ctx->fd);

	PDGL_AVI_UpdateSize(ctx->fd, ctx->osz_strl);

#if 1
	if(ctx->audh)
	{
		PDPNG_WriteFourcc(ctx->fd, FCC_LIST);
		ctx->osz_strl=vftell(ctx->fd);
		PDPNG_WriteInt32LE(ctx->fd, 0);
		PDPNG_WriteFourcc(ctx->fd, FCC_strl);

		i=sizeof(PDGL_AVIStreamHeader);
		PDPNG_WriteFourcc(ctx->fd, FCC_strh);
		PDPNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_audh=vftell(ctx->fd);
		vfwrite(ctx->audh, 1, (i&1)?(i+1):i, ctx->fd);

		i=sizeof(PDGL_WaveFmt);
		PDPNG_WriteFourcc(ctx->fd, FCC_strf);
		PDPNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_audf=vftell(ctx->fd);
		vfwrite(ctx->audf, 1, (i&1)?(i+1):i, ctx->fd);

		PDGL_AVI_UpdateSize(ctx->fd, ctx->osz_strl);
	}
#endif

	PDGL_AVI_UpdateSize(ctx->fd, ctx->osz_hdrl);

	PDPNG_WriteFourcc(ctx->fd, FCC_LIST);
	ctx->osz_movi=vftell(ctx->fd);
	PDPNG_WriteInt32LE(ctx->fd, 0);
	PDPNG_WriteFourcc(ctx->fd, FCC_movi);

	PDJPG_FlushEncodeFast(ctx->vidCtx);

	return(ctx);
}

void PDGL_AVI_CloseOutStream(PDGL_AVICtx *ctx)
{
	int i;

	PDJPG_FlushEncodeFast(ctx->vidCtx);

	PDGL_AVI_UpdateSize(ctx->fd, ctx->osz_movi);

#if 1
	PDPNG_WriteFourcc(ctx->fd, FCC_idx1);
	PDPNG_WriteInt32LE(ctx->fd, ctx->n_index*4*4);
	for(i=0; i<ctx->n_index; i++)
	{
		PDPNG_WriteFourcc(ctx->fd, ctx->index[i*4+0]);
		PDPNG_WriteInt32LE(ctx->fd, ctx->index[i*4+1]);
		PDPNG_WriteInt32LE(ctx->fd, ctx->index[i*4+2]);
		PDPNG_WriteInt32LE(ctx->fd, ctx->index[i*4+3]);
	}
#endif


	PDGL_AVI_UpdateSize(ctx->fd, ctx->osz_avi);

	if(ctx->audh)
	{
		vfseek(ctx->fd, ctx->offs_audh, 0);
		vfwrite(ctx->audh, 1, sizeof(PDGL_AVIStreamHeader), ctx->fd);

		vfseek(ctx->fd, ctx->offs_audf, 0);
		vfwrite(ctx->audf, 1, sizeof(PDGL_WaveFmt), ctx->fd);
	}

	vfseek(ctx->fd, ctx->offs_vidh, 0);
	vfwrite(ctx->vidh, 1, sizeof(PDGL_AVIStreamHeader), ctx->fd);

	vfseek(ctx->fd, ctx->offs_vidf, 0);
	vfwrite(ctx->vidf, 1, sizeof(PDGL_BMPInfoHeader), ctx->fd);

	vfseek(ctx->fd, ctx->offs_avih, 0);
	vfwrite(ctx->avih, 1, sizeof(PDGL_MainAVIHeader), ctx->fd);

	gcfree(ctx->vidh);
	gcfree(ctx->vidf);
	gcfree(ctx->avih);

	if(ctx->audh)gcfree(ctx->audh);
	if(ctx->audf)gcfree(ctx->audf);

	vfclose(ctx->fd);
	gcfree(ctx);
}

int PDGL_AVI_AddIndex(PDGL_AVICtx *ctx, uint tag, uint len, uint fl)
{
	int i;

	i=ctx->n_index++;

	if(i>=ctx->m_index)
	{
		ctx->m_index+=ctx->m_index>>1;
		ctx->index=realloc(ctx->index, ctx->m_index*4*sizeof(uint));
	}

	ctx->index[i*4+0]=tag;
	ctx->index[i*4+1]=fl;
//	ctx->index[i*4+2]=vftell(ctx->fd);
	ctx->index[i*4+2]=vftell(ctx->fd)-(ctx->osz_movi+4);
	ctx->index[i*4+3]=len;

	return(i);
}

void PDGL_AVI_EmitCodedFrame(PDGL_AVICtx *ctx, byte *buf, int sz)
{
	PDGL_AVI_AddIndex(ctx, FCC_00dc, sz, 16);

	PDPNG_WriteFourcc(ctx->fd, FCC_00dc);
	PDPNG_WriteInt32LE(ctx->fd, sz);
	vfwrite(buf, 1, (sz&1)?(sz+1):sz, ctx->fd);

	ctx->avih->dwTotalFrames++;
	ctx->avih->dwLength++;
	ctx->vidh->dwLength++;
}

void PDGL_AVI_EmitAudioFrame(PDGL_AVICtx *ctx)
{
	byte *tbuf;
	int i, j, k, l, sz;

	tbuf=ctx->capEncodeAuBuf;
	if(!tbuf)
	{
		tbuf=malloc(1<<16);
		ctx->capEncodeAuBuf=tbuf;
	}

#if 1
	if(ctx->audh)
	{
		l=44100/ctx->fps;

		sz=l;
		memset(tbuf, 128, sz);

		for(i=0; i<l; i++)
		{
			j=(ctx->audSampleBufStartPos+i)%
				ctx->audSampleBufSz;
			if(j==ctx->audSampleBufEndPos)
				break;
			k=(ctx->audSampleBuf[j*2+0]+ctx->audSampleBuf[j*2+1])/2;
			k=(k>>8)+128;
			k=(k<0)?0:(k<=255)?k:255;
			tbuf[i]=k;
		}

//		j=(ctx->audSampleBufStartPos+l)%ctx->audSampleBufSz;
		j=(ctx->audSampleBufStartPos+i)%ctx->audSampleBufSz;
		ctx->audSampleBufStartPos=j;

		for(k=0; k<8; k++)
		{
			tbuf[k]=(tbuf[k]-128)*(k/8.0)+128;
			tbuf[i-k]=(tbuf[i-k]-128)*(k/8.0)+128;
		}

		PDGL_AVI_AddIndex(ctx, FCC_01wb, sz, 16);
		PDPNG_WriteFourcc(ctx->fd, FCC_01wb);
		PDPNG_WriteInt32LE(ctx->fd, sz);
		vfwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);

		ctx->audh->dwLength+=l;
	}
#endif
}

void PDGL_AVI_EncodeFrame(PDGL_AVICtx *ctx, byte *ibuf, int w, int h)
{
//	static byte tbuf[1<<22];
//	static byte *tbuf=NULL;
	byte *tbuf;
	int i, j, k, l, sz;

	tbuf=ctx->capEncodeBuf;
	if(!tbuf)
	{
		tbuf=malloc(1<<22);
		ctx->capEncodeBuf=tbuf;
	}

	if(ctx->vidh->fccHandler==FCC_MJPG)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=PDJPG_AllocContext();

//		memset(tbuf, 0, 1<<16);

//		if(!tbuf)tbuf=malloc(1<<20);
//		i=PDJPG_EncodeFast(ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

//		sz=PDJPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

		sz=PDJPG_Encode(ibuf, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

//		printf("PDGL_AVI_EncodeFrame: Size=%d\n", sz);

//		i=PDJPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 100, 0); //70
	}

	if(ctx->vidh->fccHandler==FCC_BtV0)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BTV0_AllocContext();

		sz=BTV0_EncodeCtx(ctx->vidCtx, ibuf, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

//		printf("PDGL_AVI_EncodeFrame: Size=%d\n", sz);

	}

	if(ctx->vidh->fccHandler==0)
	{
//		if(!tbuf)tbuf=malloc(1<<22);

		k=w*h;
		for(i=0; i<k; i++)
		{
			tbuf[i*3+0]=ibuf[i*4+2];
			tbuf[i*3+1]=ibuf[i*4+1];
			tbuf[i*3+2]=ibuf[i*4+0];
		}
		
		sz=w*h*3;
	}

	if(ctx->vidh->fccHandler==FCC_RGBT)
	{
//		if(!tbuf)tbuf=malloc(1<<22);
		
		k=w*h;
		for(i=0; i<k; i++)
		{
			j=(ibuf[i*4+2]>>3) | ((ibuf[i*4+1]&0xF8)<<2) |
				((ibuf[i*4+0]&0xF8)<<7) |
				((ibuf[i*4+3]&0x80)<<8);
			
			tbuf[i*2+0]=j&0xFF;
			tbuf[i*2+1]=(j>>8)&0xFF;
		}
		sz=w*h*2;
	}

	while(ctx->accDt2>=(1.0/ctx->fps))
	{
		PDGL_AVI_EmitCodedFrame(ctx, tbuf, sz);
		PDGL_AVI_EmitAudioFrame(ctx);
		ctx->accDt2-=1.0/ctx->fps;
	}
}

void PDGL_AVI_EncodeComponentFrame(PDGL_AVICtx *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int w, int h)
{
//	static byte tbuf[1<<22];
//	static byte *tbuf=NULL;
	byte *tbuf;
	int i, j, k, l, sz;

	tbuf=ctx->capEncodeBuf;
	if(!tbuf)
	{
		tbuf=malloc(1<<22);
		ctx->capEncodeBuf=tbuf;
	}

	if(ctx->vidh->fccHandler==FCC_MJPG)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=PDJPG_AllocContext();

//		memset(tbuf, 0, 1<<16);

//		if(!tbuf)tbuf=malloc(1<<20);
//		i=PDJPG_EncodeFast(ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

//		sz=PDJPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

		sz=PDJPG_EncodeComponentCtx(ctx->vidCtx,
			rgba, norm, spec, luma, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

//		printf("PDGL_AVI_EncodeFrame: Size=%d\n", sz);

//		i=PDJPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 100, 0); //70
	}

	if(ctx->vidh->fccHandler==FCC_BtV0)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BTV0_AllocContext();

		sz=BTV0_EncodeComponentCtx(ctx->vidCtx,
			rgba, norm, spec, luma, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

//		printf("PDGL_AVI_EncodeFrame: Size=%d\n", sz);
	}

	if(ctx->vidh->fccHandler==0)
	{
//		if(!tbuf)tbuf=malloc(1<<22);

		k=w*h;
		for(i=0; i<k; i++)
		{
			tbuf[i*3+0]=rgba[i*4+2];
			tbuf[i*3+1]=rgba[i*4+1];
			tbuf[i*3+2]=rgba[i*4+0];
		}
		
		sz=w*h*3;
	}

	if(ctx->vidh->fccHandler==FCC_RGBT)
	{
//		if(!tbuf)tbuf=malloc(1<<22);
		
		k=w*h;
		for(i=0; i<k; i++)
		{
			j=(rgba[i*4+2]>>3) | ((rgba[i*4+1]&0xF8)<<2) |
				((rgba[i*4+0]&0xF8)<<7) |
				((rgba[i*4+3]&0x80)<<8);
			
			tbuf[i*2+0]=j&0xFF;
			tbuf[i*2+1]=(j>>8)&0xFF;
		}
		sz=w*h*2;
	}

	while(ctx->accDt2>=(1.0/ctx->fps))
	{
		PDGL_AVI_EmitCodedFrame(ctx, tbuf, sz);
		PDGL_AVI_EmitAudioFrame(ctx);
		ctx->accDt2-=1.0/ctx->fps;
	}
}


void PDGL_AVI_Scale800x600to320x240(byte *ibuf, byte *obuf)
{
	int i, j, k, l;
	byte *sb, *tb;

#if 0
	for(i=0; i<120; i++)
		for(j=0; j<160; j++)
			for(k=0; k<3; k++)
	{
		tb=ibuf+(i*5*800+j*5)*4+k;

		l=tb[0*800*4+0]+tb[0*800*4+4]+tb[1*800*4+0]+tb[1*800*4+4];
		obuf[((i*2+0)*320+(j*2+0))*4+k]=l>>2;

		l=tb[0*800*4+12]+tb[0*800*4+16]+tb[1*800*4+12]+tb[1*800*4+16];
		obuf[((i*2+0)*320+(j*2+1))*4+k]=l>>2;

		l=tb[3*800*4+0]+tb[3*800*4+4]+tb[4*800*4+0]+tb[4*800*4+4];
		obuf[((i*2+1)*320+(j*2+0))*4+k]=l>>2;

		l=tb[3*800*4+12]+tb[3*800*4+16]+tb[4*800*4+12]+tb[4*800*4+16];
		obuf[((i*2+1)*320+(j*2+1))*4+k]=l>>2;
	}
#endif

#if 1
	for(i=0; i<120; i++)
		for(j=0; j<160; j++)
			for(k=0; k<3; k++)
	{
		sb=ibuf+(i*5*800+j*5)*4+k;
		tb=obuf+((i*2*320+j*2)*4+k);

		l=	sb[0*800*4+0]+sb[0*800*4+4]+sb[0*800*4+8]+
			sb[1*800*4+0]+sb[1*800*4+4]+sb[1*800*4+8]+
			sb[2*800*4+0]+sb[2*800*4+4]+sb[2*800*4+8];
		tb[0]=l/9;

		l=	sb[0*800*4+8]+sb[0*800*4+12]+sb[0*800*4+16]+
			sb[1*800*4+8]+sb[1*800*4+12]+sb[1*800*4+16]+
			sb[2*800*4+8]+sb[2*800*4+12]+sb[2*800*4+16];
		tb[4]=l/9;

		l=	sb[2*800*4+0]+sb[2*800*4+4]+sb[2*800*4+8]+
			sb[3*800*4+0]+sb[3*800*4+4]+sb[3*800*4+8]+
			sb[4*800*4+0]+sb[4*800*4+4]+sb[4*800*4+8];
		tb[320*4]=l/9;

		l=	sb[2*800*4+8]+sb[2*800*4+12]+sb[2*800*4+16]+
			sb[3*800*4+8]+sb[3*800*4+12]+sb[3*800*4+16]+
			sb[4*800*4+8]+sb[4*800*4+12]+sb[4*800*4+16];
		tb[320*4+4]=l/9;
	}
#endif

#if 0
	for(i=0; i<120; i++)
		for(j=0; j<160; j++)
			for(k=0; k<3; k++)
	{
		tb=ibuf+(i*5*800+j*5)*4+k;
		l=(i*2*320+j*2)*4+k;
		obuf[l]=tb[1*800*4+4];
		obuf[l+4]=tb[1*800*4+12];
		obuf[l+320*4]=tb[3*800*4+4];
		obuf[l+320*4+4]=tb[3*800*4+12];
	}
#endif

#if 0
	for(i=0; i<120; i++)
		for(j=0; j<160; j++)
			for(k=0; k<3; k++)
	{
		tb=buf+(i*5*800+j*5)*4+k;

		l=	(tb[0*800*4+0]+tb[0*800*4+4]+tb[0*800*4+8]+
			tb[1*800*4+0]+tb[1*800*4+4]+tb[1*800*4+8]+
			tb[2*800*4+0]+tb[2*800*4+4]+tb[2*800*4+8])/9;
		buf[((i*2+0)*320+(j*2+0))*4+k]=(l<0)?0:(l>255)?255:l;

		l=	(tb[0*800*4+8]+tb[0*800*4+12]+tb[0*800*4+16]+
			tb[1*800*4+8]+tb[1*800*4+12]+tb[1*800*4+16]+
			tb[2*800*4+8]+tb[2*800*4+12]+tb[2*800*4+16])/9;
		buf[((i*2+0)*320+(j*2+1))*4+k]=(l<0)?0:(l>255)?255:l;

		l=	(tb[2*800*4+0]+tb[2*800*4+4]+tb[2*800*4+8]+
			tb[3*800*4+0]+tb[3*800*4+4]+tb[3*800*4+8]+
			tb[4*800*4+0]+tb[4*800*4+4]+tb[4*800*4+8])/9;
		buf[((i*2+1)*320+(j*2+0))*4+k]=(l<0)?0:(l>255)?255:l;

		l=	(tb[2*800*4+8]+tb[2*800*4+12]+tb[2*800*4+16]+
			tb[3*800*4+8]+tb[3*800*4+12]+tb[3*800*4+16]+
			tb[4*800*4+8]+tb[4*800*4+12]+tb[4*800*4+16])/9;
		buf[((i*2+1)*320+(j*2+1))*4+k]=(l<0)?0:(l>255)?255:l;
	}
#endif

#if 0
	for(i=0; i<600; i++)
		for(j=0; j<160; j++)
			for(k=0; k<3; k++)
	{
		l=buf[(i*800+(j*5+0))*4+k];
		l+=buf[(i*800+(j*5+1))*4+k];
		l+=buf[(i*800+(j*5+2))*4+k];
		buf[(i*320+j*2+0)*4+k]=l/3;

		l=buf[(i*800+(j*5+2))*4+k];
		l+=buf[(i*800+(j*5+3))*4+k];
		l+=buf[(i*800+(j*5+4))*4+k];
		buf[(i*320+j*2+1)*4+k]=l/3;
	}

	for(i=0; i<120; i++)
		for(j=0; j<320; j++)
			for(k=0; k<3; k++)
	{
		l=buf[((i*5+0)*320+j)*4+k];
		l+=buf[((i*5+1)*320+j)*4+k];
		l+=buf[((i*5+2)*320+j)*4+k];
		buf[((i*2+0)*320+j)*4+k]=l/3;

		l=buf[((i*5+2)*320+j)*4+k];
		l+=buf[((i*5+2)*320+j)*4+k];
		l+=buf[((i*5+4)*320+j)*4+k];
		buf[((i*2+1)*320+j)*4+k]=l/3;
	}
#endif
}

#if 0
int PDGL_AVI_EncoderLoop(void *p)
{
	byte *buf0, *buf1;
	int t0, t1;
	int i, j;

//	thSleep(rand()&15);

	if(pdgl_avi_encloop>0)
		return(0);
		
	i=pdgl_avi_encloop++;
	if(i>1)
	{
		pdgl_avi_encloop--;
		return(0);
	}
	
	printf("PDGL_AVI_EncoderLoop\n");
	
	while(1)
	{
		if(!pdgl_avi_cap)
		{
			thSleep(10);
			continue;
//			break;
		}

#if 1
		if(vftell(pdgl_avi_cap->fd)>=((1<<30)-(1<<24)))
		{
			PDGL_AVI_CloseOutStream(pdgl_avi_cap);
			pdgl_avi_cap=NULL;
			continue;
		}
#endif

		if(pdgl_avi_cap->capReqClose)
		{
			printf("PDGL_AVI_EncoderLoop: Close Stream\n");

			PDGL_AVI_CloseOutStream(pdgl_avi_cap);
			pdgl_avi_cap=NULL;
			pdgl_avi_nbase=0;
			pdgl_avi_nseq=0;
			continue;
		}

		if(pdgl_avi_cap->capFrameLastIdx==pdgl_avi_cap->capFrameIdx)
		{
			thSleep(5);
			continue;
		}

		if(pdgl_avi_cap->accDt2<=(1.0/pdgl_avi_cap->fps))
		{
			thSleep(5);
			continue;
		}

		t0=PDGL_TimeMS();

		j=pdgl_avi_cap->xs*pdgl_avi_cap->ys;
		if(!pdgl_avi_cap->capFrameBuf2)
			pdgl_avi_cap->capFrameBuf2=malloc(j*4);
//		memcpy(pdgl_avi_cap->capFrameBuf2,
//			pdgl_avi_cap->capFrameBuf, i);

		buf0=pdgl_avi_cap->capFrameBuf;
		buf1=pdgl_avi_cap->capFrameBuf2;
		for(i=0; i<j; i++)
		{
			buf1[0]=buf0[2]; buf1[1]=buf0[1];
			buf1[2]=buf0[0]; buf1[3]=buf0[3];
			buf0+=4; buf1+=4;
		}

		PDGL_AVI_EncodeFrame(pdgl_avi_cap,
			pdgl_avi_cap->capFrameBuf2,
			pdgl_avi_cap->xs, pdgl_avi_cap->ys);
		pdgl_avi_cap->capFrameLastIdx=
			pdgl_avi_cap->capFrameIdx;
			
		t1=PDGL_TimeMS();
		pdgl_avi_enctime=t1-t0;

		thSleep(5);
	}

	pdgl_avi_encloop--;
	return(0);
}
#endif

PDGL_API int PDGL_AVI_GetEncodeTime()
	{ return(pdgl_avi_enctime); }

PDGL_API int PDGL_AVI_WriteStereoSamples(short *buf, int cnt)
{
	int i, j, k;

	if(!pdgl_avi_cap)
		return(-1);
		
	if(!pdgl_avi_cap->audSampleBuf)
	{
		i=44100;

		pdgl_avi_cap->audSampleBufSz=i;
		pdgl_avi_cap->audSampleBufStartPos=0;
		pdgl_avi_cap->audSampleBufEndPos=0;

		pdgl_avi_cap->audSampleBuf=malloc(i*2*sizeof(short));
	}
	
	for(i=0; i<cnt; i++)
	{
		j=(pdgl_avi_cap->audSampleBufEndPos+i)%
			pdgl_avi_cap->audSampleBufSz;
		pdgl_avi_cap->audSampleBuf[j*2+0]=buf[i*2+0];
		pdgl_avi_cap->audSampleBuf[j*2+1]=buf[i*2+1];
	}
	j=(pdgl_avi_cap->audSampleBufEndPos+cnt)%
		pdgl_avi_cap->audSampleBufSz;
	pdgl_avi_cap->audSampleBufEndPos=j;
	
	return(0);
}

#if 0
PDGL_API int PDGL_AVI_Screenshot()
{
	static byte *buf=NULL, *buf1=NULL;
	static int v=0;
	VFILE *fd;
	int w0, h0, w1, h1;
	int i, j, k, l;

//	GfxDrv_GetWindowSize(&w0, &h0);

	w0=pdgl_avi_cap->xs;
	h0=pdgl_avi_cap->ys;

//	w0=800; h0=600;
	w1=320;	h1=240;

	if(!buf)buf=malloc(w0*(h0+1)*4);
//	if(!buf1)buf1=malloc(w1*(h1+1)*4);

	if(!pdgl_avi_cap->capFrameBuf)
		pdgl_avi_cap->capFrameBuf=malloc(w0*(h0+1)*4);

	glFinish();
//	glReadPixels (0, 0, w0, h0, GL_RGBA, GL_UNSIGNED_BYTE, buf); 
	glReadPixels (0, 0, w0, h0, GL_BGRA, GL_UNSIGNED_BYTE, buf); 
//	glReadPixels (0, 0, w0, h0, GL_BGR, GL_UNSIGNED_BYTE, buf); 

	memcpy((void *)pdgl_avi_cap->capFrameBuf, buf, w0*h0*4);
	pdgl_avi_cap->capFrameIdx++;

#if 0
	//gamma correct
	for(i=0; i<(w0*h0); i++)
	{
		l=i*4;
		for(j=0; j<3; j++)
		{
			k=buf[l];
			k=k*1.25+32;
			if(k>255)k=255;
			buf[l++]=k;
		}
	}
#endif

//	glFinish();

//	if(v)
//	PDGL_AVI_EncodeFrame(pdgl_avi_cap, buf, w0, h0);

//	PDGL_AVI_Scale800x600to320x240(buf, buf1);
//	PDGL_AVI_EncodeFrame(pdgl_avi_cap, buf1, w1, h1);

//	glFinish();
//	glReadPixels (0, 0, w0, h0, GL_RGBA, GL_UNSIGNED_BYTE, buf); 
//	v=1;

	return(0);
}


PDGL_API int PDGL_AVI_DoScreenshotFrame(float dt)
{
	static char *base;
//	static float t;
	char buf[64];
	int xs, ys;

#if 0
	if(pdgl_avi_cap)
	{
		if(vftell(pdgl_avi_cap->fd)>=((1<<30)-(1<<24)))
		{
			PDGL_AVI_CloseOutStream(pdgl_avi_cap);
			pdgl_avi_cap=NULL;
		}
	}
#endif

	if(!pdgl_avi_cap)
	{
//		base=strdup(Sys_GetTimeStr2());

		if(!pdgl_avi_nbase)
			pdgl_avi_nbase=bgbrng_genvalue();

		sprintf(buf, "%08X_%d", pdgl_avi_nbase, pdgl_avi_nseq++);
		base=strdup(buf);
//		sprintf(buf, "record_%s.avi", base);
		sprintf(buf, "record/record_%s.avi", base);

		GfxDrv_GetWindowSize(&xs, &ys);

//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, 320, 240, 10);
//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, 800, 600, 12);
		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, xs, ys, 12);
//		t=0;
		pdgl_avi_cap->accDt=0;

		if(pdgl_avi_encloop<=0)
		{
			thThread(PDGL_AVI_EncoderLoop, NULL);
			thSleep(10);
		}
	}

	if(pdgl_avi_cap)
	{
		pdgl_avi_cap->accDt+=dt;
		pdgl_avi_cap->accDt2+=dt;

//		if(t>(1.0/pdgl_avi_cap->fps))
		if(pdgl_avi_cap->accDt>=(1.0/pdgl_avi_cap->fps))
		{
			PDGL_AVI_Screenshot();
//			t-=1.0/pdgl_avi_cap->fps;

//			pdgl_avi_cap->accDt=0;

			while(pdgl_avi_cap->accDt>=(1.0/pdgl_avi_cap->fps))
				pdgl_avi_cap->accDt-=1.0/pdgl_avi_cap->fps;

			//lag, drop frame
//			while(t>(1.0/(2*pdgl_avi_cap->fps)))
//				t-=1.0/pdgl_avi_cap->fps;
		}
//		pdgl_avi_cap->accDt+=dt;
//		t+=dt;
	}

	return(0);
}

PDGL_API int PDGL_AVI_NoCapture()
{
	if(pdgl_avi_cap)
	{
		pdgl_avi_cap->capReqClose=1;
	}

#if 0
	if(pdgl_avi_cap)
	{
		PDGL_AVI_CloseOutStream(pdgl_avi_cap);
		pdgl_avi_cap=NULL;
		pdgl_avi_nbase=0;
		pdgl_avi_nseq=0;
	}
#endif
}

#endif



PDGL_API int Tex_SplinePolateRGBA8(byte *src, int w, int h,
	float x, float y, float *rgba)
{
	float tv[16], tva[4], tvb[4], tvc[4], tvd[4];
	double f, g, xf, yf, xg, yg;
	int i, j, k, l, xi, yi;
	int xi0, xi1, xi2, xi3;
	int yi0, yi1, yi2, yi3;

//	if(x<0)x=0; if(x>w)x=w;
//	if(y<0)y=0; if(y>h)y=h;

	xi=floor(x); yi=floor(y);
	xf=x-xi; yf=y-yi; xg=1-xf; yg=1-yf;

#if 0
	if((xi<1) || (xi>=(w-2)) || (yi<1) || (yi>=(h-2)))
	{
		if((xi>=(w-1)) || (yi>=(h-1)))
		{
			//far border, LERP not possible

			if((xi>=(w-1)) && (yi>=(h-1)))
			{
				k=((y-1)*w+(x-1))*4;
				rgba[0]=src[k+0]; rgba[1]=src[k+1];
				rgba[2]=src[k+2]; rgba[3]=src[k+3];
				return(0);
				//lowest corner
			}

			//at borders, use nearest pixel
			//FIX: could use H/V split here
			i=xi; j=yi;
			if(xi>=w)xi=w-1;
			if(yi>=y)yi=h-1;

			k=(j*w+i)*4;
			rgba[0]=src[k+0]; rgba[1]=src[k+1];
			rgba[2]=src[k+2]; rgba[3]=src[k+3];
			return(0);
		}

		//near edges, use LERP
		for(k=0; k<4; k++)
		{
			tv[0]=src[((yi  )*w+xi  )*4+k];
			tv[1]=src[((yi  )*w+xi+1)*4+k];
			tv[2]=src[((yi+1)*w+xi  )*4+k];
			tv[3]=src[((yi+1)*w+xi+1)*4+k];

			rgba[k]=tv[0]*xg*yg+tv[1]*xf*yg+
				tv[2]*xg*yf+tv[3]*xf*yf;
		}
		return(0);
	}

	for(k=0; k<4; k++)
	{
		tv[ 0]=src[((yi-1)*w+xi-1)*4+k];
		tv[ 1]=src[((yi-1)*w+xi  )*4+k];
		tv[ 2]=src[((yi-1)*w+xi+1)*4+k];
		tv[ 3]=src[((yi-1)*w+xi+2)*4+k];
		tv[ 4]=src[((yi  )*w+xi-1)*4+k];
		tv[ 5]=src[((yi  )*w+xi  )*4+k];
		tv[ 6]=src[((yi  )*w+xi+1)*4+k];
		tv[ 7]=src[((yi  )*w+xi+2)*4+k];
		tv[ 8]=src[((yi+1)*w+xi-1)*4+k];
		tv[ 9]=src[((yi+1)*w+xi  )*4+k];
		tv[10]=src[((yi+1)*w+xi+1)*4+k];
		tv[11]=src[((yi+1)*w+xi+2)*4+k];
		tv[12]=src[((yi+2)*w+xi-1)*4+k];
		tv[13]=src[((yi+2)*w+xi  )*4+k];
		tv[14]=src[((yi+2)*w+xi+1)*4+k];
		tv[15]=src[((yi+2)*w+xi+2)*4+k];

		tva[0]=tv[ 1]+xf*(tv[ 1]-tv[ 0]);
		tva[1]=tv[ 5]+xf*(tv[ 5]-tv[ 4]);
		tva[2]=tv[ 9]+xf*(tv[ 9]-tv[ 8]);
		tva[3]=tv[13]+xf*(tv[13]-tv[12]);

		tvb[0]=tv[ 2]+xg*(tv[ 2]-tv[ 3]);
		tvb[1]=tv[ 6]+xg*(tv[ 6]-tv[ 7]);
		tvb[2]=tv[10]+xg*(tv[10]-tv[11]);
		tvb[3]=tv[14]+xg*(tv[14]-tv[15]);

		tvc[0]=tv[ 4]+yf*(tv[ 4]-tv[ 0]);
		tvc[1]=tv[ 5]+yf*(tv[ 5]-tv[ 1]);
		tvc[2]=tv[ 6]+yf*(tv[ 6]-tv[ 2]);
		tvc[3]=tv[ 7]+yf*(tv[ 7]-tv[ 3]);

		tvd[0]=tv[ 8]+yg*(tv[ 8]-tv[12]);
		tvd[1]=tv[ 9]+yg*(tv[ 9]-tv[13]);
		tvd[2]=tv[10]+yg*(tv[10]-tv[14]);
		tvd[3]=tv[11]+yg*(tv[11]-tv[15]);


		tva[0]=tva[0]*xg+tvb[0]*xf;
		tva[1]=tva[1]*xg+tvb[1]*xf;
		tva[2]=tva[2]*xg+tvb[2]*xf;
		tva[3]=tva[3]*xg+tvb[3]*xf;

		tvb[0]=tvc[0]*yg+tvd[0]*yf;
		tvb[1]=tvc[1]*yg+tvd[1]*yf;
		tvb[2]=tvc[2]*yg+tvd[2]*yf;
		tvb[3]=tvc[3]*yg+tvd[3]*yf;

		tvc[0]=tva[1]+yf*(tva[1]-tva[0]);
		tvc[1]=tva[2]+yg*(tva[2]-tva[3]);
		tvc[2]=tvb[1]+xf*(tvb[1]-tvb[0]);
		tvc[3]=tvb[2]+xg*(tvb[2]-tvb[3]);

//		f=xf-0.5; g=yf-0.5;
//		g=1.0-(xf*xf+yf*yf);

		f=(tvc[0]*yg+tvc[1]*yf+
		   tvc[2]*xg+tvc[3]*xf)*0.5;

		rgba[k]=f;


//		f=tva[1]+yf*(tva[1]-tva[0]);
//		g=tva[2]+yg*(tva[2]-tva[3]);
//		rgba[k]=f*yg+g*yf;
	}
#endif

	xi0=xi-1; xi1=xi; xi2=xi+1; xi3=xi+2;
	yi0=yi-1; yi1=yi; yi2=yi+1; yi3=yi+2;

	if((xi<1) || (xi>=(w-2)) || (yi<1) || (yi>=(h-2)))
	{
		while(xi0<0)xi0+=w; while(xi0>=w)xi0-=w;
		while(xi1<0)xi1+=w; while(xi1>=w)xi1-=w;
		while(xi2<0)xi2+=w; while(xi2>=w)xi2-=w;
		while(xi3<0)xi3+=w; while(xi3>=w)xi3-=w;

		while(yi0<0)yi0+=h; while(yi0>=h)yi0-=h;
		while(yi1<0)yi1+=h; while(yi1>=h)yi1-=h;
		while(yi2<0)yi2+=h; while(yi2>=h)yi2-=h;
		while(yi3<0)yi3+=h; while(yi3>=h)yi3-=h;
	}

	for(k=0; k<4; k++)
	{
		tv[ 0]=src[(yi0*w+xi0)*4+k];
		tv[ 1]=src[(yi0*w+xi1)*4+k];
		tv[ 2]=src[(yi0*w+xi2)*4+k];
		tv[ 3]=src[(yi0*w+xi3)*4+k];
		tv[ 4]=src[(yi1*w+xi0)*4+k];
		tv[ 5]=src[(yi1*w+xi1)*4+k];
		tv[ 6]=src[(yi1*w+xi2)*4+k];
		tv[ 7]=src[(yi1*w+xi3)*4+k];
		tv[ 8]=src[(yi2*w+xi0)*4+k];
		tv[ 9]=src[(yi2*w+xi1)*4+k];
		tv[10]=src[(yi2*w+xi2)*4+k];
		tv[11]=src[(yi2*w+xi3)*4+k];
		tv[12]=src[(yi3*w+xi0)*4+k];
		tv[13]=src[(yi3*w+xi1)*4+k];
		tv[14]=src[(yi3*w+xi2)*4+k];
		tv[15]=src[(yi3*w+xi3)*4+k];

		tva[0]=tv[ 1]+xf*(tv[ 1]-tv[ 0]);
		tva[1]=tv[ 5]+xf*(tv[ 5]-tv[ 4]);
		tva[2]=tv[ 9]+xf*(tv[ 9]-tv[ 8]);
		tva[3]=tv[13]+xf*(tv[13]-tv[12]);

		tvb[0]=tv[ 2]+xg*(tv[ 2]-tv[ 3]);
		tvb[1]=tv[ 6]+xg*(tv[ 6]-tv[ 7]);
		tvb[2]=tv[10]+xg*(tv[10]-tv[11]);
		tvb[3]=tv[14]+xg*(tv[14]-tv[15]);

		tvc[0]=tv[ 4]+yf*(tv[ 4]-tv[ 0]);
		tvc[1]=tv[ 5]+yf*(tv[ 5]-tv[ 1]);
		tvc[2]=tv[ 6]+yf*(tv[ 6]-tv[ 2]);
		tvc[3]=tv[ 7]+yf*(tv[ 7]-tv[ 3]);

		tvd[0]=tv[ 8]+yg*(tv[ 8]-tv[12]);
		tvd[1]=tv[ 9]+yg*(tv[ 9]-tv[13]);
		tvd[2]=tv[10]+yg*(tv[10]-tv[14]);
		tvd[3]=tv[11]+yg*(tv[11]-tv[15]);


		tva[0]=tva[0]*xg+tvb[0]*xf;
		tva[1]=tva[1]*xg+tvb[1]*xf;
		tva[2]=tva[2]*xg+tvb[2]*xf;
		tva[3]=tva[3]*xg+tvb[3]*xf;

		tvb[0]=tvc[0]*yg+tvd[0]*yf;
		tvb[1]=tvc[1]*yg+tvd[1]*yf;
		tvb[2]=tvc[2]*yg+tvd[2]*yf;
		tvb[3]=tvc[3]*yg+tvd[3]*yf;

		tvc[0]=tva[1]+yf*(tva[1]-tva[0]);
		tvc[1]=tva[2]+yg*(tva[2]-tva[3]);
		tvc[2]=tvb[1]+xf*(tvb[1]-tvb[0]);
		tvc[3]=tvb[2]+xg*(tvb[2]-tvb[3]);

//		f=xf-0.5; g=yf-0.5;
//		g=1.0-(xf*xf+yf*yf);

		f=(tvc[0]*yg+tvc[1]*yf+
		   tvc[2]*xg+tvc[3]*xf)*0.5;

		rgba[k]=f;


//		f=tva[1]+yf*(tva[1]-tva[0]);
//		g=tva[2]+yg*(tva[2]-tva[3]);
//		rgba[k]=f*yg+g*yf;
	}
}

PDGL_API int Tex_ResampleSpline(byte *src, int iw, int ih, byte *dst, int ow, int oh)
{
	float tv[4];
	float xs, ys, xc, yc;
	float ix, iy, fx, fy;
	int i, j, k, l, ik, il;

//	return(0);

	xs=((float)iw/(float)ow);
	ys=((float)ih/(float)oh);

	for(i=0; i<oh; i++)
		for(j=0; j<ow; j++)
	{
		Tex_SplinePolateRGBA8(src, iw, ih, j*xs, i*ys, tv);
		k=tv[0]; if(k<0)k=0; if(k>255)k=255; dst[0]=k;
		k=tv[1]; if(k<0)k=0; if(k>255)k=255; dst[1]=k;
		k=tv[2]; if(k<0)k=0; if(k>255)k=255; dst[2]=k;
		k=tv[3]; if(k<0)k=0; if(k>255)k=255; dst[3]=k;

		dst+=4;
	}
	return(0);
}
