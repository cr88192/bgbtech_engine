#include <pdgl.h>
#include <bgbbtj.h>

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

#define FCC_00dc	FOURCC('0', '0', 'd', 'c')
#define FCC_01wb	FOURCC('0', '1', 'w', 'b')

#define FCC_rpza	FOURCC('r','p','z','a')
#define FCC_azpr	FOURCC('a','z','p','r')

#define FCC_bt1c	FOURCC('b','t','1','c')
#define FCC_c1tb	FOURCC('c','1','t','b')

#define FCC_BTIC	FOURCC('B', 'T', 'I', 'C')

volatile PDGL_AVICtx *pdgl_avi_cap;
volatile void *pdgl_avi_lock;

int pdgl_avi_nbase=0;
int pdgl_avi_nseq=0;

volatile int pdgl_avi_encloop=0;
volatile int pdgl_avi_enctime=0;


void PDGL_AVI_UpdateSize(VFILE *fd, int o)
{
	int i;

	i=vftell(fd);
	vfseek(fd, o, 0);
	PDPNG_WriteInt32LE(fd, i-o-4);
	vfseek(fd, i, 0);
}

PDGL_AVICtx *PDGL_AVI_OpenOutStream(char *name,
	int w, int h, float fps, int fcc)
{
	return(PDGL_AVI_OpenOutStream2(
		name, w, h, fps, fcc, BGBBTJ_AVI_AUID_22K8BM));
}

PDGL_AVICtx *PDGL_AVI_OpenOutStream2(char *name,
	int w, int h, float fps, int fcc, int auid)
{
	byte buf[512];

	VFILE *fd;
	PDGL_AVICtx *ctx;
	int i, j, k;

	memset(buf, 0, 512);

	if(!pdgl_avi_lock)
		pdgl_avi_lock=thFastMutex();

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

	if(fcc)
	{
		ctx->vidh->fccHandler=fcc;
	}else
	{
//		fcc=FCC_MJPG;
		fcc=FCC_rpza;
//		ctx->vidh->fccHandler=FCC_MJPG;
//		ctx->vidh->fccHandler=FCC_RGBT;
		ctx->vidh->fccHandler=FCC_rpza;
	}

	ctx->vidh->dwFlags=0;
	ctx->vidh->dwRate=fps*1024+0.5;
	ctx->vidh->dwScale=1024;
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

	if(fcc==FCC_MJPG)
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biCompression=FCC_JPEG;
		ctx->vidf->biSizeImage=w*h*3;
	}

	if(fcc==FCC_RGBT)
	{
		ctx->vidf->biBitCount=16;
		ctx->vidf->biSizeImage=w*h*2;

//		ctx->vidf->biBitCount=24;
//		ctx->vidf->biCompression=FCC_JPEG;
//		ctx->vidf->biCompression=FCC_RGBT;
//		ctx->vidf->biSizeImage=w*h*3;
	}

	if(fcc==FCC_rpza)
	{
		ctx->vidf->biBitCount=32;
		ctx->vidf->biCompression=FCC_azpr;
		ctx->vidf->biSizeImage=w*h*4;
	}

	if(fcc==FCC_bt1c)
	{
		ctx->vidh->fccHandler=FCC_BTIC;
		ctx->vidf->biCompression=FCC_bt1c;

		ctx->vidf->biBitCount=32;
//		ctx->vidf->biCompression=FCC_c1tb;
		ctx->vidf->biSizeImage=w*h*4;
	}

#if 1
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
//	ctx->audf->nChannels=1;
	ctx->audf->nChannels=2;
	ctx->audf->nSamplesPerSec=44100;
//	ctx->audf->nAvgBytesPerSec=44100;
	ctx->audf->nAvgBytesPerSec=44100*2*2;
//	ctx->audf->nBlockAlign=1;
	ctx->audf->nBlockAlign=4;
//	ctx->audf->wBitsPerSample=8;
	ctx->audf->wBitsPerSample=16;
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

//	PDJPG_FlushEncodeFast((PDJPG_Context *)ctx->vidCtx);
	BTJ_JFE_FlushEncodeFast((BGBBTJ_JFE_Context *)ctx->vidCtx);

	return(ctx);
}

void PDGL_AVI_CloseOutStream(PDGL_AVICtx *ctx)
{
	int i;

//	PDJPG_FlushEncodeFast((PDJPG_Context *)ctx->vidCtx);
	BTJ_JFE_FlushEncodeFast((BGBBTJ_JFE_Context *)ctx->vidCtx);

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
	ctx->frnum++;
}

#if 0
void PDGL_AVI_EmitAudioFrame(PDGL_AVICtx *ctx)
{
	byte *tbuf;
	int i, j, k, l, sz;

	tbuf=(byte *)ctx->capEncodeAuBuf;
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
#endif

#if 1
void PDGL_AVI_EmitAudioFrame(PDGL_AVICtx *ctx)
{
	short *tbuf;
	int i, j, k, l, l2, sz;

	PDGL_AVI_FrameLock();

	tbuf=(short *)ctx->capEncodeAuBuf;
	if(!tbuf)
	{
		tbuf=malloc((1<<16)*2*sizeof(short));
		ctx->capEncodeAuBuf=(volatile byte *)tbuf;
	}

#if 1
	if(ctx->audh)
	{
		l=44100/ctx->fps;

//		sz=l;
		sz=l*2*2;
//		memset(tbuf, 128, sz);
		memset(tbuf, 0, sz);

		if(ctx->audSampleBufSz>0)
		{
			for(i=0; i<l; i++)
			{
				j=(ctx->audSampleBufStartPos+i)%
					ctx->audSampleBufSz;
				if(j==ctx->audSampleBufEndPos)
				{
					for(k=0; k<64; k++)
					{
//						if((i-k)<0)break;
						if((i-k)<0)continue;
						tbuf[(i-k)*2+0]*=k/64.0;
						tbuf[(i-k)*2+1]*=k/64.0;
					}
				
					break;
				}
//				k=(ctx->audSampleBuf[j*2+0]+ctx->audSampleBuf[j*2+1])/2;
//				k=(k>>8)+128;
//				k=(k<0)?0:(k<=255)?k:255;
//				tbuf[i]=k;

				tbuf[i*2+0]=ctx->audSampleBuf[j*2+0];
				tbuf[i*2+1]=ctx->audSampleBuf[j*2+1];
			}

			l2=ctx->audSampleBufEndPos-ctx->audSampleBufStartPos;
			if(l2<0)l2+=ctx->audSampleBufSz;
			if(l2>0.4*44100)
			{
//				j=(ctx->audSampleBufStartPos+l)%ctx->audSampleBufSz;
				j=(ctx->audSampleBufStartPos+i)%ctx->audSampleBufSz;
				ctx->audSampleBufStartPos=j;
			}
		}

#if 0
		for(k=0; k<8; k++)
		{
//			tbuf[k]=(tbuf[k]-128)*(k/8.0)+128;
//			tbuf[i-k]=(tbuf[i-k]-128)*(k/8.0)+128;

			tbuf[k*2+0]=tbuf[k*2+0]*(k/8.0);
			tbuf[k*2+1]=tbuf[k*2+1]*(k/8.0);
			
			tbuf[(l-k-1)*2+0]=tbuf[(l-k-1)*2+0]*(k/8.0);
			tbuf[(l-k-1)*2+1]=tbuf[(l-k-1)*2+1]*(k/8.0);
		}
#endif

		PDGL_AVI_AddIndex(ctx, FCC_01wb, sz, 16);
		PDPNG_WriteFourcc(ctx->fd, FCC_01wb);
		PDPNG_WriteInt32LE(ctx->fd, sz);
		vfwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);

		ctx->audh->dwLength+=l;
	}
#endif

	PDGL_AVI_FrameUnlock();
}
#endif

bool PDGL_AVI_CheckEatAudioFrame(PDGL_AVICtx *ctx)
{
	int i, j, k, l, l2;

	l=44100/ctx->fps;

	PDGL_AVI_FrameLock();
	l2=ctx->audSampleBufEndPos-ctx->audSampleBufStartPos;
	if(l2<0)l2+=ctx->audSampleBufSz;

//	if(l2>=(24*l))
	if(l2>=(0.8*44100))
	{
		i=(ctx->audSampleBufStartPos+l)%ctx->audSampleBufSz;
		ctx->audSampleBufStartPos=i;
		PDGL_AVI_FrameUnlock();
		return(true);
	}
	PDGL_AVI_FrameUnlock();
	return(false);
}

void PDGL_AVI_EncodeFrame(PDGL_AVICtx *ctx, byte *ibuf, int w, int h)
{
//	static byte tbuf[1<<22];
//	static byte *tbuf=NULL;
	byte *tbuf;
	int i, j, k, l, sz, qf;

//	if(ctx->accDt2<(1.0/ctx->fps))
//		return;

	tbuf=(byte *)ctx->capEncodeBuf;
	if(!tbuf)
	{
		tbuf=malloc(1<<22);
		ctx->capEncodeBuf=tbuf;
	}

	qf=95;

	if(ctx->vidh->fccHandler==FCC_MJPG)
	{
		if(!ctx->vidCtx)
		{
//			ctx->vidCtx=PDJPG_AllocContext();
			ctx->vidCtx=BTJ_JFE_AllocContext();
		}

//		memset(tbuf, 0, 1<<16);

//		if(!tbuf)tbuf=malloc(1<<20);
//		i=PDJPG_EncodeFast(ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

//		sz=PDJPG_EncodeFastCtx((PDJPG_Context *)ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

//		sz=PDJPG_EncodeFastCtx((PDJPG_Context *)ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 95, 0); //70

		sz=BTJ_JFE_EncodeFastCtx((BGBBTJ_JFE_Context *)ctx->vidCtx,
			ibuf, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight,
			95, BGBBTJ_JPG_BGRA); //70

//		printf("PDGL_AVI_EncodeFrame: Size=%d\n", sz);

//		i=PDJPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 100, 0); //70
	}

	if(ctx->vidh->fccHandler==FCC_rpza)
	{
		if(!(ctx->frnum&63))
			{ qf|=BGBBTJ_QFL_IFRAME; }
		else
			{ qf|=BGBBTJ_QFL_PFRAME; }

		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_RPZA_AllocContext();
		sz=BGBBTJ_RPZA_EncodeImageFast(
			ctx->vidCtx, ibuf, tbuf, 1<<22,
			ctx->avih->dwWidth, ctx->avih->dwHeight,
			qf, BGBBTJ_JPG_BGRA);
	}

	if((ctx->vidh->fccHandler==FCC_bt1c) ||
		(ctx->vidf->biCompression==FCC_bt1c))
	{
		if(!(ctx->frnum&63))
			{ qf|=BGBBTJ_QFL_IFRAME; }
		else
			{ qf|=BGBBTJ_QFL_PFRAME; }

		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_RPZA_AllocContext();
		sz=BGBBTJ_BT1CF_EncodeImageFast(
			ctx->vidCtx, ibuf, tbuf, 1<<22,
			ctx->avih->dwWidth, ctx->avih->dwHeight,
			qf, BGBBTJ_JPG_BGRA);
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

#if 1
	PDGL_AVI_EmitCodedFrame(ctx, tbuf, sz);
	PDGL_AVI_EmitAudioFrame(ctx);
	ctx->accDt2-=1.0/ctx->fps;

	while(ctx->accDt2>(1.0/ctx->fps))
	{
		PDGL_AVI_EmitCodedFrame(ctx, NULL, 0);
		PDGL_AVI_EmitAudioFrame(ctx);
		ctx->accDt2-=1.0/ctx->fps;
	}
#endif

#if 0
	while(ctx->accDt2>=(1.0/ctx->fps))
	{
		PDGL_AVI_EmitCodedFrame(ctx, tbuf, sz);
		PDGL_AVI_EmitAudioFrame(ctx);
		ctx->accDt2-=1.0/ctx->fps;
	}
#endif

	while(PDGL_AVI_CheckEatAudioFrame(ctx))
		{ /* NO-OP */ }
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

void PDGL_AVI_Lock(void)
{
	thLockFastMutex(pdgl_avi_lock);
}

void PDGL_AVI_Unlock(void)
{
	thUnlockFastMutex(pdgl_avi_lock);
}

void PDGL_AVI_FrameLock(void)
{
	thLockFastMutex(pdgl_avi_lock);
}

void PDGL_AVI_FrameUnlock(void)
{
	thUnlockFastMutex(pdgl_avi_lock);
}

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
//		if(vftell(pdgl_avi_cap->fd)>=((1<<30)-(1<<24)))
		if((vftell(pdgl_avi_cap->fd)>=((1<<30)-(1<<24))) ||
			pdgl_avi_cap->capReqBreak)
		{
			PDGL_AVI_CloseOutStream((PDGL_AVICtx *)pdgl_avi_cap);
			pdgl_avi_cap=NULL;
			continue;
		}
#endif

		if(pdgl_avi_cap->capReqClose)
		{
			printf("PDGL_AVI_EncoderLoop: Close Stream\n");

			PDGL_AVI_CloseOutStream((PDGL_AVICtx *)pdgl_avi_cap);
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

#if 0
		if(pdgl_avi_cap->accDt2<=(1.0/pdgl_avi_cap->fps))
		{
			thSleep(5);
			continue;
		}
#endif

		t0=PDGL_TimeMS();

#if 0
		j=pdgl_avi_cap->xs*pdgl_avi_cap->ys;
		if(!pdgl_avi_cap->capFrameBuf2)
			pdgl_avi_cap->capFrameBuf2=malloc(j*4);
//		memcpy(pdgl_avi_cap->capFrameBuf2,
//			pdgl_avi_cap->capFrameBuf, i);

		buf0=(byte *)pdgl_avi_cap->capFrameBuf;
		buf1=(byte *)pdgl_avi_cap->capFrameBuf2;
		for(i=0; i<j; i++)
		{
			buf1[0]=buf0[2]; buf1[1]=buf0[1];
			buf1[2]=buf0[0]; buf1[3]=buf0[3];
			buf0+=4; buf1+=4;
		}
#endif

#if 1
		j=pdgl_avi_cap->xs*pdgl_avi_cap->ys;
		if(!pdgl_avi_cap->capFrameBuf2)
			pdgl_avi_cap->capFrameBuf2=malloc(j*4);
		PDGL_AVI_FrameLock();
		memcpy(pdgl_avi_cap->capFrameBuf2,
			pdgl_avi_cap->capFrameBuf, j*4);
		PDGL_AVI_FrameUnlock();
#endif

//		PDGL_AVI_EncodeFrame((PDGL_AVICtx *)pdgl_avi_cap,
//			(byte *)pdgl_avi_cap->capFrameBuf,
//			pdgl_avi_cap->xs, pdgl_avi_cap->ys);

		PDGL_AVI_EncodeFrame((PDGL_AVICtx *)pdgl_avi_cap,
			(byte *)pdgl_avi_cap->capFrameBuf2,
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

PDGL_API int PDGL_AVI_GetEncodeTime()
	{ return(pdgl_avi_enctime); }

PDGL_API int PDGL_AVI_WriteStereoSamples(short *buf, int cnt)
{
	int al, ar;
	int i, j, k;

	PDGL_AVI_FrameLock();
	if(!pdgl_avi_cap)
	{
		PDGL_AVI_FrameUnlock();
		return(-1);
	}
		
	if(!pdgl_avi_cap->audSampleBuf)
	{
//		i=44100;
		i=65536;

		pdgl_avi_cap->audSampleBufSz=i;
		pdgl_avi_cap->audSampleBufStartPos=0;
		pdgl_avi_cap->audSampleBufEndPos=0;

		pdgl_avi_cap->audSampleBuf=malloc(i*2*sizeof(short));
	}
	
	for(i=0; i<cnt; i++)
	{
		j=(pdgl_avi_cap->audSampleBufEndPos+i)%
			pdgl_avi_cap->audSampleBufSz;
		if(buf)
		{
			pdgl_avi_cap->audSampleBuf[j*2+0]=buf[i*2+0];
			pdgl_avi_cap->audSampleBuf[j*2+1]=buf[i*2+1];
		}else
		{
			pdgl_avi_cap->audSampleBuf[j*2+0]=0;
			pdgl_avi_cap->audSampleBuf[j*2+1]=0;
		}
	}

#if 1
	j=pdgl_avi_cap->audSampleBufEndPos;
	k=j-1;
	if(k<0)k+=pdgl_avi_cap->audSampleBufSz;
	al=(pdgl_avi_cap->audSampleBuf[k*2+0]+
		pdgl_avi_cap->audSampleBuf[j*2+0])/2;
	ar=(pdgl_avi_cap->audSampleBuf[k*2+1]+
		pdgl_avi_cap->audSampleBuf[j*2+1])/2;

	for(i=0; i<16; i++)
	{
		j=(pdgl_avi_cap->audSampleBufEndPos+i)%
			pdgl_avi_cap->audSampleBufSz;
		pdgl_avi_cap->audSampleBuf[j*2+0]=
			pdgl_avi_cap->audSampleBuf[j*2+0]*(i/16.0)+al*(1.0-(i/16.0));
		pdgl_avi_cap->audSampleBuf[j*2+1]=
			pdgl_avi_cap->audSampleBuf[j*2+1]*(i/16.0)+ar*(1.0-(i/16.0));

		j=(pdgl_avi_cap->audSampleBufEndPos-i)%
			pdgl_avi_cap->audSampleBufSz;
		if(j<0)j+=pdgl_avi_cap->audSampleBufSz;
		pdgl_avi_cap->audSampleBuf[j*2+0]=
			pdgl_avi_cap->audSampleBuf[j*2+0]*(i/16.0)+al*(1.0-(i/16.0));
		pdgl_avi_cap->audSampleBuf[j*2+1]=
			pdgl_avi_cap->audSampleBuf[j*2+1]*(i/16.0)+ar*(1.0-(i/16.0));
	}
#endif

	j=(pdgl_avi_cap->audSampleBufEndPos+cnt)%
		pdgl_avi_cap->audSampleBufSz;
	pdgl_avi_cap->audSampleBufEndPos=j;
	
	PDGL_AVI_FrameUnlock();
	return(0);
}

PDGL_API int PDGL_AVI_Screenshot(int doshot)
{
	static byte *buf=NULL, *buf1=NULL;
	static int v=0;
	VFILE *fd;
	int w0, h0, w1, h1;
	int i, j, k, l;

//	GfxDrv_GetWindowSize(&w0, &h0);

	PDGL_AVI_FrameLock();
	if(!pdgl_avi_cap)
	{
		PDGL_AVI_FrameUnlock();
		return(-1);
	}

	w0=pdgl_avi_cap->xs;
	h0=pdgl_avi_cap->ys;

//	w0=800; h0=600;
	w1=320;	h1=240;

	if(!buf)buf=malloc(w0*(h0+1)*4);
//	if(!buf1)buf1=malloc(w1*(h1+1)*4);

	if(!pdgl_avi_cap->capFrameBuf)
		pdgl_avi_cap->capFrameBuf=malloc(w0*(h0+1)*4);

	PDGL_AVI_FrameUnlock();

	pdglFinish();

#ifdef GLES
	pdglReadPixels (0, 0, w0, h0, GL_RGBA, GL_UNSIGNED_BYTE, buf); 
#else
//	pdglReadPixels (0, 0, w0, h0, GL_RGBA, GL_UNSIGNED_BYTE, buf); 
//	pdglReadPixels (0, 0, w0, h0, GL_BGRA, GL_UNSIGNED_BYTE, buf); 
//	pdglReadPixels (0, 0, w0, h0, GL_BGR, GL_UNSIGNED_BYTE, buf); 

	pdglReadPixels (0, 0, w0, h0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, buf); 
#endif

//	thLockFastMutex(pdgl_avi_cap->lock);
	PDGL_AVI_FrameLock();
	if(pdgl_avi_cap && pdgl_avi_cap->capFrameBuf)
	{
		memcpy((void *)pdgl_avi_cap->capFrameBuf, buf, w0*h0*4);
		pdgl_avi_cap->capFrameIdx++;
	}
	PDGL_AVI_FrameUnlock();
//	thUnlockFastMutex(pdgl_avi_cap->lock);

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

//	pdglFinish();

	if(doshot&32)
	{
		PDGL_AVI_EncodeFrame(pdgl_avi_cap, buf, w0, h0);
	}

//	if(v)
//	PDGL_AVI_EncodeFrame(pdgl_avi_cap, buf, w0, h0);

//	PDGL_AVI_Scale800x600to320x240(buf, buf1);
//	PDGL_AVI_EncodeFrame(pdgl_avi_cap, buf1, w1, h1);

//	pdglFinish();
//	pdglReadPixels (0, 0, w0, h0, GL_RGBA, GL_UNSIGNED_BYTE, buf); 
//	v=1;

	return(0);
}


PDGL_API int PDGL_AVI_DoScreenshotFrame(double dt, int doshot)
{
	static char *base;
//	static float t;
	char buf[64];
	int xs, ys, fcc;

	if(!pdgl_avi_lock)
		pdgl_avi_lock=thFastMutex();

#if 1
	if((doshot&32) && pdgl_avi_cap)
	{
		if(vftell(pdgl_avi_cap->fd)>=((1<<30)-(1<<24)))
		{
			PDGL_AVI_CloseOutStream(pdgl_avi_cap);
			pdgl_avi_cap=NULL;
		}
	}
#endif

	PDGL_AVI_FrameLock();

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

//		fcc=FCC_rpza;
		fcc=FCC_bt1c;
		if(doshot&32)
			fcc=FCC_MJPG;

//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, 320, 240, 10);
//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, 800, 600, 12);
//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, xs, ys, 12, fcc);
//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, xs, ys, 16, fcc);
		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, xs, ys, 24, fcc);
//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, xs, ys, 18, fcc);
//		pdgl_avi_cap=PDGL_AVI_OpenOutStream(buf, xs, ys, 30, fcc);
//		t=0;
		pdgl_avi_cap->accDt=0;

		if(!(doshot&32))
		{
			if(pdgl_avi_encloop<=0)
			{
				thThread(PDGL_AVI_EncoderLoop, NULL);
				thSleep(10);
			}
		}
		
		PDGL_AVI_FrameUnlock();
		PDGL_AVI_WriteStereoSamples(NULL,
			12*44100/pdgl_avi_cap->fps);
		PDGL_AVI_FrameLock();
	}
//	PDGL_AVI_FrameUnlock();

	if(pdgl_avi_cap)
	{
#if 1
		//break off AVI if resolution has changed
		GfxDrv_GetWindowSize(&xs, &ys);
		if((pdgl_avi_cap->xs!=xs) || (pdgl_avi_cap->ys!=ys))
		{
			pdgl_avi_cap->capReqBreak=1;

			if(doshot&32)
			{
				PDGL_AVI_CloseOutStream(pdgl_avi_cap);
				pdgl_avi_cap=NULL;
//				pdgl_avi_nbase=0;
//				pdgl_avi_nseq=0;
			}

			PDGL_AVI_FrameUnlock();
			return(0);
		}
#endif

		pdgl_avi_cap->accDt+=dt;
		pdgl_avi_cap->accDt2+=dt;

//		if(t>(1.0/pdgl_avi_cap->fps))
		if(pdgl_avi_cap->accDt>=(1.0/pdgl_avi_cap->fps))
		{
			PDGL_AVI_FrameUnlock();
			PDGL_AVI_Screenshot(doshot);
			PDGL_AVI_FrameLock();
//			t-=1.0/pdgl_avi_cap->fps;

//			pdgl_avi_cap->accDt=0;

			while(pdgl_avi_cap &&
					(pdgl_avi_cap->accDt>=(1.0/pdgl_avi_cap->fps)))
				pdgl_avi_cap->accDt-=1.0/pdgl_avi_cap->fps;

			//lag, drop frame
//			while(t>(1.0/(2*pdgl_avi_cap->fps)))
//				t-=1.0/pdgl_avi_cap->fps;
		}
//		pdgl_avi_cap->accDt+=dt;
//		t+=dt;
	}
	PDGL_AVI_FrameUnlock();

	return(0);
}

PDGL_API int PDGL_AVI_NoCapture(int doshot)
{
	PDGL_AVI_FrameLock();
	if(pdgl_avi_cap)
	{
		pdgl_avi_cap->capReqClose=1;
	}
//	PDGL_AVI_FrameUnlock();

#if 1
	if((doshot&32) && pdgl_avi_cap)
	{
		PDGL_AVI_CloseOutStream(pdgl_avi_cap);
		pdgl_avi_cap=NULL;
		pdgl_avi_nbase=0;
		pdgl_avi_nseq=0;
	}
#endif

	PDGL_AVI_FrameUnlock();
}

PDGL_API double PDGL_AVI_GetFrameTime()
{
	if(!pdgl_avi_cap)
		return(1.0/10);
	return(1.0/pdgl_avi_cap->fps);
}
