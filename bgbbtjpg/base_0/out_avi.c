#include <bgbbtj.h>

#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
#include <bgbmid.h>
#endif

#ifndef BGBBTJ_API
#define BGBBTJ_API
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

#define FCC_MBTJ	FOURCC('M', 'B', 'T', 'J')
#define FCC_BTJP	FOURCC('B', 'T', 'J', 'P')

#define FCC_MPNG	FOURCC('M', 'P', 'N', 'G')
#define FCC_PNG		FOURCC('P', 'N', 'G', ' ')
#define FCC_PNG1	FOURCC('P', 'N', 'G', '1')
#define FCC_png1	FOURCC('p', 'n', 'g', '1')

#define FCC_MBTC	FOURCC('M', 'B', 'T', 'C')
#define FCC_BTIC	FOURCC('B', 'T', 'I', 'C')

#define FCC_RGBA	FOURCC('R', 'G', 'B', 'A')
#define FCC_RGBT	FOURCC('R', 'G', 'B', 'T')

#define FCC_BtV0	FOURCC('B', 't', 'V', '0')

#define FCC_00dc	FOURCC('0', '0', 'd', 'c')
#define FCC_01wb	FOURCC('0', '1', 'w', 'b')

#define FCC_rpza	RIFF_MAKETAG('r','p','z','a')
#define FCC_azpr	RIFF_MAKETAG('a','z','p','r')

#define FCC_bt1c	RIFF_MAKETAG('b','t','1','c')
#define FCC_c1tb	RIFF_MAKETAG('c','1','t','b')
#define FCC_bt1d	RIFF_MAKETAG('b','t','1','d')

volatile BGBBTJ_AVICtx *pdgl_avi_cap;

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

BGBBTJ_API void BGBBTJ_AVI_UpdateSize(VFILE *fd, int o)
{
	int i;

	i=vftell(fd);
	vfseek(fd, o, 0);
	BGBBTJ_PNG_WriteInt32LE(fd, i-o-4);
	vfseek(fd, i, 0);
}

BGBBTJ_API BGBBTJ_AVICtx *BGBBTJ_AVI_OpenOutStream(char *name,
	int w, int h, float fps, int fcc)
{
	return(BGBBTJ_AVI_OpenOutStream2(
		name, w, h, fps, fcc, BGBBTJ_AVI_AUID_22K8BM));
}

BGBBTJ_API BGBBTJ_AVICtx *BGBBTJ_AVI_OpenOutStream2(char *name,
	int w, int h, float fps, int fcc, int auid)
{
	byte buf[512];

	VFILE *fd;
	BGBBTJ_AVICtx *ctx;
	BGBBTJ_VidCodecCTX *vcctx;
	int i, j, k;

	memset(buf, 0, 512);


	fd=vffopen(name, "w+b");
	if(!fd)return(NULL);

	ctx=gcalloc(sizeof(BGBBTJ_AVICtx));
	ctx->fd=fd;

	ctx->index=malloc(4096*sizeof(uint));
	ctx->n_index=0;
	ctx->m_index=4096/4;
	ctx->fps=fps;
	ctx->xs=w;
	ctx->ys=h;

	ctx->avih=gcalloc(sizeof(BGBBTJ_MainAVIHeader));
	ctx->vidh=gcalloc(sizeof(BGBBTJ_AVIStreamHeader));
	ctx->vidf=gcalloc(sizeof(BGBBTJ_BMPInfoHeader));


	ctx->vidh->fccType=FCC_vids;

	printf("BGBBTJ_AVI_OpenOutStream: fcc=%08X\n", fcc);

	if(fcc)
	{
		ctx->vidh->fccHandler=fcc;
	}else
	{
		fcc=FCC_MJPG;
		ctx->vidh->fccHandler=FCC_MJPG;
//		ctx->vidh->fccHandler=FCC_RGBT;
//		ctx->vidh->fccHandler=FCC_BtV0;
	}

	ctx->vidh->dwFlags=0;
	ctx->vidh->dwRate=fps*1024+0.5;
	ctx->vidh->dwScale=1024;
	ctx->vidh->dwSuggestedBufferSize=0;
	ctx->vidh->dwQuality=0;

	ctx->vidf->biSize=sizeof(BGBBTJ_BMPInfoHeader);
	ctx->vidf->biWidth=w;
	ctx->vidf->biHeight=h;
	ctx->vidf->biPlanes=1;
	ctx->vidf->biXPelsPerMeter=0;
	ctx->vidf->biYPelsPerMeter=0;

	if(fcc==FCC_MJPG)
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biCompression=FCC_JPEG;
		ctx->vidf->biSizeImage=w*h*3;
	}else if(fcc==FCC_RGBT)
	{
		ctx->vidf->biBitCount=16;
		ctx->vidf->biSizeImage=w*h*2;

//		ctx->vidf->biBitCount=24;
//		ctx->vidf->biCompression=FCC_JPEG;
//		ctx->vidf->biCompression=FCC_RGBT;
//		ctx->vidf->biSizeImage=w*h*3;
	}else if(fcc==FCC_BtV0)
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biCompression=FCC_BtV0;
		ctx->vidf->biSizeImage=w*h*3;
	}else if(fcc==FCC_MPNG)
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biCompression=FCC_PNG;
		ctx->vidf->biSizeImage=w*h*3;
	}else if((fcc==FCC_PNG1) || (fcc==FCC_png1))
	{
		ctx->vidf->biBitCount=32;
		ctx->vidf->biCompression=FCC_PNG1;
		ctx->vidf->biSizeImage=w*h*4;
	}else if(fcc==FCC_MBTC)
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biCompression=FCC_BTIC;
		ctx->vidf->biSizeImage=w*h*3;
	}else if(fcc==FCC_rpza)
	{
		ctx->vidf->biBitCount=32;
		ctx->vidf->biCompression=FCC_azpr;
		ctx->vidf->biSizeImage=w*h*4;
	}else if(fcc==FCC_bt1c)
	{
		ctx->vidh->fccHandler=FCC_BTIC;
		ctx->vidf->biCompression=FCC_bt1c;

		ctx->vidf->biBitCount=32;
//		ctx->vidf->biCompression=FCC_c1tb;
		ctx->vidf->biSizeImage=w*h*4;
	}else if(fcc==FCC_bt1d)
	{
		ctx->vidh->fccHandler=FCC_BTIC;
		ctx->vidf->biCompression=FCC_bt1d;
		ctx->vidf->biBitCount=32;
		ctx->vidf->biSizeImage=w*h*4;
	}else if(fcc==BGBBTJ_FCC_bt2c)
	{
		ctx->vidh->fccHandler=FCC_BTIC;
		ctx->vidf->biCompression=BGBBTJ_FCC_bt2c;
		ctx->vidf->biBitCount=32;
		ctx->vidf->biSizeImage=w*h*4;
	}else
	{
		ctx->vidf->biBitCount=24;
		ctx->vidf->biSizeImage=w*h*3;

		ctx->vidf->biCompression=
			ctx->vidh->fccHandler;

		vcctx=BGBBTJ_Codec_BeginCompress(
			ctx->vidh->fccHandler,
			ctx->vidf, ctx->vidf);
		ctx->vid_codec_ctx=vcctx;
		
		ctx->vidStrd=vcctx->vidStrd;
		ctx->sz_vidStrd=vcctx->sz_vidStrd;
	}

#if 1
	if(auid)
	{
		ctx->audh=gcalloc(sizeof(BGBBTJ_AVIStreamHeader));
		ctx->audf=gcalloc(sizeof(BGBBTJ_WaveFmt));

		ctx->audh->fccType=FCC_auds;
		ctx->audh->fccHandler=1;
		ctx->audh->dwFlags=0;
//		ctx->audh->dwRate=44100;
//		ctx->audh->dwRate=22050;

		switch(auid&BGBBTJ_AVI_AUID_MASK_RATE)
		{
		case BGBBTJ_AVI_AUID_8K:
			ctx->audh->dwRate=8000; break;
		case BGBBTJ_AVI_AUID_11K:
			ctx->audh->dwRate=11025; break;
		case BGBBTJ_AVI_AUID_16K:
			ctx->audh->dwRate=16000; break;
		case BGBBTJ_AVI_AUID_22K:
			ctx->audh->dwRate=22050; break;
		case BGBBTJ_AVI_AUID_32K:
			ctx->audh->dwRate=32000; break;
		case BGBBTJ_AVI_AUID_44K:
			ctx->audh->dwRate=44100; break;
		default:
			break;
		}

		ctx->audh->dwScale=1;
		ctx->audh->dwSuggestedBufferSize=0;
		ctx->audh->dwQuality=0;

		ctx->audf->wFormatTag=1;
//		ctx->audf->nChannels=2;
		ctx->audf->nChannels=1;
//		ctx->audf->nSamplesPerSec=44100;
//		ctx->audf->nAvgBytesPerSec=44100*2*2;
//		ctx->audf->nAvgBytesPerSec=44100;

		ctx->audf->nSamplesPerSec=22050;
		ctx->audf->nAvgBytesPerSec=22050;

//		ctx->audf->nBlockAlign=4;
		ctx->audf->nBlockAlign=1;
//		ctx->audf->wBitsPerSample=16;
		ctx->audf->wBitsPerSample=8;
		ctx->audf->cbSize=0;
	}
#endif

#if 0
	ctx->audh=gcalloc(sizeof(BGBBTJ_AVIStreamHeader));
	ctx->audf=gcalloc(sizeof(BGBBTJ_WaveFmt));

	ctx->audh->fccType=FCC_auds;
	ctx->audh->fccHandler=1;
	ctx->audh->dwFlags=0;
//	ctx->audh->dwRate=44100;
	ctx->audh->dwRate=22050;
	ctx->audh->dwScale=1;
	ctx->audh->dwSuggestedBufferSize=0;
	ctx->audh->dwQuality=0;

	ctx->audf->wFormatTag=1;
//	ctx->audf->nChannels=2;
	ctx->audf->nChannels=1;
//	ctx->audf->nSamplesPerSec=44100;
//	ctx->audf->nAvgBytesPerSec=44100*2*2;
//	ctx->audf->nAvgBytesPerSec=44100;

	ctx->audf->nSamplesPerSec=22050;
	ctx->audf->nAvgBytesPerSec=22050;

//	ctx->audf->nBlockAlign=4;
	ctx->audf->nBlockAlign=1;
//	ctx->audf->wBitsPerSample=16;
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
	ctx->avih->dwRate=fps*1024+0.5;
	ctx->avih->dwScale=1024;


	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_RIFF);
	ctx->osz_avi=vftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_AVI);

	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_LIST);
	ctx->osz_hdrl=vftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_hdrl);

	i=sizeof(BGBBTJ_MainAVIHeader);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_avih);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_avih=vftell(ctx->fd);
	vfwrite(ctx->avih, 1, (i&1)?(i+1):i, ctx->fd);


	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_LIST);
	ctx->osz_strl=vftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strl);

	i=sizeof(BGBBTJ_AVIStreamHeader);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strh);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_vidh=vftell(ctx->fd);
	vfwrite(ctx->vidh, 1, (i&1)?(i+1):i, ctx->fd);

#if 1
	i=sizeof(BGBBTJ_BMPInfoHeader);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strf);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
	ctx->offs_vidf=vftell(ctx->fd);
	vfwrite(ctx->vidf, 1, (i&1)?(i+1):i, ctx->fd);

	if(ctx->vidStrd)
	{
		i=ctx->sz_vidStrd;
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strd);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_vidStrd=vftell(ctx->fd);
		vfwrite(ctx->vidStrd, 1, (i&1)?(i+1):i, ctx->fd);
	}
#endif

#if 0
	if(ctx->vidStrd)
	{
		i=ctx->sz_vidStrd;
		i+=sizeof(BGBBTJ_BMPInfoHeader);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strf);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_vidf=vftell(ctx->fd);
		vfwrite(ctx->vidf, 1, sizeof(BGBBTJ_BMPInfoHeader), ctx->fd);
		vfwrite(ctx->vidStrd, 1, ctx->sz_vidStrd+(i&1), ctx->fd);
	}else
	{
		i=sizeof(BGBBTJ_BMPInfoHeader);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strf);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_vidf=vftell(ctx->fd);
		vfwrite(ctx->vidf, 1, (i&1)?(i+1):i, ctx->fd);
	}
#endif

	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_strl);

#if 1
	if(ctx->audh)
	{
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_LIST);
		ctx->osz_strl=vftell(ctx->fd);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strl);

		i=sizeof(BGBBTJ_AVIStreamHeader);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strh);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_audh=vftell(ctx->fd);
		vfwrite(ctx->audh, 1, (i&1)?(i+1):i, ctx->fd);

		i=sizeof(BGBBTJ_WaveFmt);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strf);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
		ctx->offs_audf=vftell(ctx->fd);
		vfwrite(ctx->audf, 1, (i&1)?(i+1):i, ctx->fd);

		if(ctx->audStrd)
		{
			i=ctx->sz_audStrd;
			BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_strd);
			BGBBTJ_PNG_WriteInt32LE(ctx->fd, i);
			ctx->offs_audStrd=vftell(ctx->fd);
			vfwrite(ctx->audStrd, 1, (i&1)?(i+1):i, ctx->fd);
		}

		BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_strl);
	}
#endif

	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_hdrl);

	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_LIST);
	ctx->osz_movi=vftell(ctx->fd);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, 0);
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_movi);

//	BGBBTJ_JPG_FlushEncodeFast(ctx->vidCtx);

	return(ctx);
}

BGBBTJ_API void BGBBTJ_AVI_CloseOutStream(BGBBTJ_AVICtx *ctx)
{
	int i;

	BGBBTJ_JPG_FlushEncodeFast(ctx->vidCtx);

	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_movi);

#if 1
	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_idx1);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->n_index*4*4);
	for(i=0; i<ctx->n_index; i++)
	{
		BGBBTJ_PNG_WriteFourcc(ctx->fd, ctx->index[i*4+0]);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->index[i*4+1]);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->index[i*4+2]);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, ctx->index[i*4+3]);
	}
#endif


	BGBBTJ_AVI_UpdateSize(ctx->fd, ctx->osz_avi);

	if(ctx->audh)
	{
		vfseek(ctx->fd, ctx->offs_audh, 0);
		vfwrite(ctx->audh, 1, sizeof(BGBBTJ_AVIStreamHeader), ctx->fd);

		vfseek(ctx->fd, ctx->offs_audf, 0);
		vfwrite(ctx->audf, 1, sizeof(BGBBTJ_WaveFmt), ctx->fd);
	}

	vfseek(ctx->fd, ctx->offs_vidh, 0);
	vfwrite(ctx->vidh, 1, sizeof(BGBBTJ_AVIStreamHeader), ctx->fd);

	vfseek(ctx->fd, ctx->offs_vidf, 0);
	vfwrite(ctx->vidf, 1, sizeof(BGBBTJ_BMPInfoHeader), ctx->fd);

	vfseek(ctx->fd, ctx->offs_avih, 0);
	vfwrite(ctx->avih, 1, sizeof(BGBBTJ_MainAVIHeader), ctx->fd);

	gcfree(ctx->vidh);
	gcfree(ctx->vidf);
	gcfree(ctx->avih);

	if(ctx->audh)gcfree(ctx->audh);
	if(ctx->audf)gcfree(ctx->audf);

	vfclose(ctx->fd);
	gcfree(ctx);
}

int BGBBTJ_AVI_AddIndex(BGBBTJ_AVICtx *ctx, uint tag, uint len, uint fl)
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

void BGBBTJ_AVI_EmitCodedFrame(BGBBTJ_AVICtx *ctx, byte *buf,
	int sz, int qf)
{
	int fl;

//	printf("BGBBTJ_AVI_EmitCodedFrame: Sz=%d Qf=%d  \n", sz, qf);

	fl=0;
	if(!(qf&BGBBTJ_QFL_PFRAME))
		{ fl|=16; }

	BGBBTJ_AVI_AddIndex(ctx, FCC_00dc, sz, fl);
//	BGBBTJ_AVI_AddIndex(ctx, FCC_00dc, sz, 16);

	BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_00dc);
	BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
	vfwrite(buf, 1, (sz&1)?(sz+1):sz, ctx->fd);

	ctx->avih->dwTotalFrames++;
	ctx->avih->dwLength++;
	ctx->vidh->dwLength++;
	ctx->frnum++;
}

void BGBBTJ_AVI_EmitAudioFrame(BGBBTJ_AVICtx *ctx)
{
	static byte *tbuf=NULL;
	static s16 *tsbuf=NULL;
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int i, j, k, l, n, sz, bl, bsz;

	tbuf=ctx->capEncodeAuBuf;
	if(!tbuf)
	{
		tbuf=malloc(1<<18);
		ctx->capEncodeAuBuf=tbuf;
		tsbuf=(s16 *)tbuf;
	}

#if 1
	if(ctx->audh)
	{
//		l=44100/ctx->fps;
//		l=22050/ctx->fps;
		l=ctx->audf->nSamplesPerSec/ctx->fps;

//		sz=l*4;
//		sz=l;
		sz=(l*ctx->audf->nAvgBytesPerSec)/ctx->audf->nSamplesPerSec;

		if(ctx->audf->wBitsPerSample==8)
			{ memset(tbuf, 128, sz); }
		else
			{ memset(tbuf, 0, sz); }

		if(ctx->audf->wFormatTag==BGBBTJ_WAVE_FORMAT_IMAADPCM)
		{
#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
			bsz=ctx->audf->nBlockAlign;
			bl=BGBMID_MsImaAdpcm_StereoSamplesFromBlockSize(bsz);
			n=(l+bl-1)/bl;
			l=n*bl;
			sz=n*bsz;
			memset(tbuf, 0, sz);

			if(!ctx->audSampleBuf)
			{
				BGBBTJ_AVI_AddIndex(ctx, FCC_01wb, sz, 16);
				BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_01wb);
				BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
				vfwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);
				ctx->audh->dwLength+=l;
				return;
			}
			
			for(i=0; i<n; i++)
			{
				j=(ctx->audSampleBufStartPos+i*bl)%
					ctx->audSampleBufSz;
				BGBMID_MsImaAdpcm_EncodeBlockStereo(
					ctx->audSampleBuf+j*2, tbuf+i*bsz, bl);
			}

			j=(ctx->audSampleBufStartPos+i*bl)%ctx->audSampleBufSz;
			ctx->audSampleBufStartPos=j;
#endif
		}
		else if(ctx->audf->wFormatTag==BGBBTJ_WAVE_FORMAT_PCM)
		{
			if(!ctx->audSampleBuf)
			{
				BGBBTJ_AVI_AddIndex(ctx, FCC_01wb, sz, 16);
				BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_01wb);
				BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
				vfwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);
				ctx->audh->dwLength+=l;
				return;
			}

			for(i=0; i<l; i++)
			{
//				k=(ctx->audSampleBuf[j*2+0]+ctx->audSampleBuf[j*2+1])/2;
//				k=(k>>8)+128;
//				k=(k<0)?0:(k<=255)?k:255;
//				tbuf[i]=k;

//				i0=ctx->audSampleBuf[j*2+0];
//				i1=ctx->audSampleBuf[j*2+1];

				if(ctx->audf->nSamplesPerSec==22050)
				{
					j0=(ctx->audSampleBufStartPos+(2*i+0))%
						ctx->audSampleBufSz;
					j1=(ctx->audSampleBufStartPos+(2*i+1))%
						ctx->audSampleBufSz;
					if(j0==ctx->audSampleBufEndPos)break;
					if(j1==ctx->audSampleBufEndPos)break;

					i0=(ctx->audSampleBuf[j0*2+0]+
						ctx->audSampleBuf[j1*2+0])/2;
					i1=(ctx->audSampleBuf[j0*2+1]+
						ctx->audSampleBuf[j1*2+1])/2;
				}else
				{
					j=(ctx->audSampleBufStartPos+i)%
						ctx->audSampleBufSz;
					i0=ctx->audSampleBuf[j*2+0];
					i1=ctx->audSampleBuf[j*2+1];
				}
	
				if(ctx->audf->nChannels==1)
				{
					if(ctx->audf->wBitsPerSample==8)
					{
						k=(i0+i1)/2;
						k=(k>>8)+128;
						k=(k<0)?0:(k<=255)?k:255;
						tbuf[i]=k;
					}else
					{
						k=(i0+i1)/2;
						k=(k<(-32768))?(-32768):(k>32767)?32767:k;
						tsbuf[i]=k;
					}
				}else
				{
					if(ctx->audf->wBitsPerSample==8)
					{
						j0=(i0>>8)+128;
						j1=(i1>>8)+128;
						j0=(j0<0)?0:(j0<=255)?j0:255;
						j1=(j1<0)?0:(j1<=255)?j1:255;
						tbuf[i*2+0]=j0;
						tbuf[i*2+1]=j1;
					}else
					{
						tsbuf[i*2+0]=i0;
						tsbuf[i*2+1]=i1;
					}
				}
		
//				i0=((j/64)&1)?1024:-1024; i1=-i0;

//				tbuf[i*2+0]=i0;
//				tbuf[i*2+1]=i1;
			}

//			j=(ctx->audSampleBufStartPos+l)%ctx->audSampleBufSz;
//			j=(ctx->audSampleBufStartPos+i)%ctx->audSampleBufSz;
			if(ctx->audf->nSamplesPerSec==22050)
			{
				j=(ctx->audSampleBufStartPos+2*i)%ctx->audSampleBufSz;
			}else
			{
				j=(ctx->audSampleBufStartPos+i)%ctx->audSampleBufSz;
			}

			ctx->audSampleBufStartPos=j;
		}

//		for(k=0; k<8; k++)
//		{
//			tbuf[k]=(tbuf[k]-128)*(k/8.0)+128;
//			tbuf[i-k]=(tbuf[i-k]-128)*(k/8.0)+128;
//		}

		BGBBTJ_AVI_AddIndex(ctx, FCC_01wb, sz, 16);
		BGBBTJ_PNG_WriteFourcc(ctx->fd, FCC_01wb);
		BGBBTJ_PNG_WriteInt32LE(ctx->fd, sz);
		vfwrite(tbuf, 1, (sz&1)?(sz+1):sz, ctx->fd);

		ctx->audh->dwLength+=l;
	}
#endif
}

BGBBTJ_API void BGBBTJ_AVI_EncodeFrame(BGBBTJ_AVICtx *ctx,
	byte *ibuf, int w, int h)
{
	BGBBTJ_AVI_EncodeFrame2(ctx, ibuf, w, h, 90, 0);
}

BGBBTJ_API void BGBBTJ_AVI_EncodeFrame2(BGBBTJ_AVICtx *ctx,
	byte *ibuf, int w, int h, int qf, int clrs)
{
//	static byte tbuf[1<<22];
//	static byte *tbuf=NULL;
	byte *tbuf;
	int i, j, k, l, sz, fl;

	tbuf=ctx->capEncodeBuf;
	if(!tbuf)
	{
		tbuf=malloc(1<<22);
		ctx->capEncodeBuf=tbuf;
	}

//	qf=75;
//	qf=85;
//	qf=80;

	sz=-999;

	if((ctx->vidh->fccHandler==FCC_MJPG) ||
		(ctx->vidh->fccHandler==FCC_MBTJ))
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_JPG_AllocContext();

//		memset(tbuf, 0, 1<<16);

//		if(!tbuf)tbuf=malloc(1<<20);
//		i=BGBBTJ_JPG_EncodeFast(ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

//		sz=BGBBTJ_JPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

//		sz=BGBBTJ_JPG_Encode(ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, qf); //70

		sz=BGBBTJ_JPG_EncodeComponentCtx(ctx->vidCtx,
			ibuf, NULL, NULL, NULL, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf); //70

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);

//		i=BGBBTJ_JPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 100, 0); //70
	}

	if((ctx->vidh->fccHandler==FCC_MPNG) ||
		(ctx->vidh->fccHandler==FCC_PNG1) ||
		(ctx->vidh->fccHandler==FCC_png1))
	{
//		if(!ctx->vidCtx)
//			ctx->vidCtx=BTV0_AllocContext();

		sz=BGBBTJ_BufPNG_Encode(tbuf, 1<<22, ibuf, 
			ctx->avih->dwWidth, ctx->avih->dwHeight);

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);

	}

	if(ctx->vidh->fccHandler==FCC_rpza)
	{
		if(!(qf&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME)))
		{
//			if(!(ctx->frnum&15))
			if(!(ctx->frnum&63))
				{ qf|=BGBBTJ_QFL_IFRAME; }
			else
				{ qf|=BGBBTJ_QFL_PFRAME; }
		}

		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_RPZA_AllocContext();
		sz=BGBBTJ_RPZA_EncodeImage(ctx->vidCtx, ibuf, tbuf, 1<<22,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf, clrs);
	}

	if((ctx->vidh->fccHandler==FCC_bt1c) ||
		(ctx->vidf->biCompression==FCC_bt1c))
	{
		if(!(qf&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME)))
		{
//			if(!(ctx->frnum&15))
			if(!(ctx->frnum&63))
				{ qf|=BGBBTJ_QFL_IFRAME; }
			else
				{ qf|=BGBBTJ_QFL_PFRAME; }
		}

		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_BT1C_AllocContext();
		sz=BGBBTJ_BT1C_EncodeImage(ctx->vidCtx, ibuf, tbuf, 1<<22,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf, clrs);
	}

	if(ctx->vidh->fccHandler==FCC_MBTC)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BTIC1_AllocContext();

		sz=BTIC1_EncodeComponentCtx(ctx->vidCtx,
			ibuf, NULL, NULL, NULL,
			tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);

	}

	if(ctx->vidh->fccHandler==FCC_BtV0)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BTV0_AllocContext();

		sz=BTV0_EncodeCtx(ctx->vidCtx, ibuf, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);

	}

	if(ctx->vid_codec_ctx)
	{
		if(!(qf&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME)))
		{
//			if(!(ctx->frnum&15))
			if(!(ctx->frnum&63))
				{ qf|=BGBBTJ_QFL_IFRAME; }
			else
				{ qf|=BGBBTJ_QFL_PFRAME; }
		}

		sz=BGBBTJ_Codec_CompressFrame(ctx->vid_codec_ctx,
			ibuf, tbuf, 1<<22, qf, 0, &fl);
		qf&=(~(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME));
		qf|=fl&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME);
	}

	if(sz==-999)
	{
		ctx->vid_codec_ctx=BGBBTJ_Codec_BeginCompress(
			ctx->vidh->fccHandler, ctx->vidf, ctx->vidf);

		if(ctx->vid_codec_ctx)
		{
			sz=BGBBTJ_Codec_CompressFrame(ctx->vid_codec_ctx,
				ibuf, tbuf, 1<<22, qf, 0, &fl);
			qf&=(~(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME));
			qf|=fl&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME);
		}
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

	if(qf&BGBBTJ_QFL_BATCHENCODE)
	{
		BGBBTJ_AVI_EmitCodedFrame(ctx, tbuf, sz, qf);
		BGBBTJ_AVI_EmitAudioFrame(ctx);
		ctx->accDt2=0;
	}else
	{
		while(ctx->accDt2>=(1.0/ctx->fps))
		{
			BGBBTJ_AVI_EmitCodedFrame(ctx, tbuf, sz, qf);
			BGBBTJ_AVI_EmitAudioFrame(ctx);
			ctx->accDt2-=1.0/ctx->fps;
		}
	}
}

BGBBTJ_API void BGBBTJ_AVI_EncodeComponentFrame(BGBBTJ_AVICtx *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int w, int h)
{
	BGBBTJ_AVI_EncodeComponentFrame2(
		ctx, rgba, norm, spec, luma, w, h,
		90, BGBBTJ_JPG_RGBA);
}

BGBBTJ_API void BGBBTJ_AVI_EncodeComponentFrame2(BGBBTJ_AVICtx *ctx,
	byte *rgba, byte *norm, byte *spec, byte *luma,
	int w, int h, int qf, int pf)
{
//	static byte tbuf[1<<22];
//	static byte *tbuf=NULL;
	byte *tbuf;
	int i, j, k, l, sz, fl;

	tbuf=ctx->capEncodeBuf;
	if(!tbuf)
	{
		tbuf=malloc(1<<22);
		ctx->capEncodeBuf=tbuf;
	}

	sz=-999;

	if(ctx->vidh->fccHandler==FCC_MJPG)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_JPG_AllocContext();

//		memset(tbuf, 0, 1<<16);

//		if(!tbuf)tbuf=malloc(1<<20);
//		i=BGBBTJ_JPG_EncodeFast(ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

//		sz=BGBBTJ_JPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90, 0); //70

//		sz=BGBBTJ_JPG_EncodeComponentCtx(ctx->vidCtx,
//			rgba, norm, spec, luma, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 90); //70

		sz=BGBBTJ_JPG_EncodeComponentCtx(ctx->vidCtx,
			rgba, norm, spec, luma, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf); //70

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);

//		i=BGBBTJ_JPG_EncodeFastCtx(ctx->vidCtx, ibuf, tbuf,
//			ctx->avih->dwWidth, ctx->avih->dwHeight, 100, 0); //70
	}

	if((ctx->vidh->fccHandler==FCC_MPNG) ||
		(ctx->vidh->fccHandler==FCC_PNG1) ||
		(ctx->vidh->fccHandler==FCC_png1))
	{
//		if(!ctx->vidCtx)
//			ctx->vidCtx=BTV0_AllocContext();

		sz=BGBBTJ_BufPNG_Encode(tbuf, 1<<22, rgba, 
			ctx->avih->dwWidth, ctx->avih->dwHeight); //70

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);

	}

	if(ctx->vidh->fccHandler==FCC_rpza)
	{
		if(!(qf&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME)))
		{
//			if(!(ctx->frnum&15))
			if(!(ctx->frnum&63))
				{ qf|=BGBBTJ_QFL_IFRAME; }
			else
				{ qf|=BGBBTJ_QFL_PFRAME; }
		}

		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_RPZA_AllocContext();
		sz=BGBBTJ_RPZA_EncodeImage(ctx->vidCtx, rgba, tbuf, 1<<22,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf, 0);
	}

	if((ctx->vidh->fccHandler==FCC_bt1c)||
		(ctx->vidf->biCompression==FCC_bt1c))
	{
		if(!(qf&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME)))
		{
//			if(!(ctx->frnum&15))
			if(!(ctx->frnum&63))
				{ qf|=BGBBTJ_QFL_IFRAME; }
			else
				{ qf|=BGBBTJ_QFL_PFRAME; }
		}

		if(!ctx->vidCtx)
			ctx->vidCtx=BGBBTJ_BT1C_AllocContext();
		sz=BGBBTJ_BT1C_EncodeImage(ctx->vidCtx, rgba, tbuf, 1<<22,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf, 0);
	}

	if(ctx->vidh->fccHandler==FCC_MBTC)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BTIC1_AllocContext();

		sz=BTIC1_EncodeComponentCtx(ctx->vidCtx,
			rgba, norm, spec, luma,
			tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf); //70

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);

	}

	if(ctx->vidh->fccHandler==FCC_BtV0)
	{
		if(!ctx->vidCtx)
			ctx->vidCtx=BTV0_AllocContext();

		sz=BTV0_EncodeComponentCtx(ctx->vidCtx,
			rgba, norm, spec, luma, tbuf,
			ctx->avih->dwWidth, ctx->avih->dwHeight, qf, 0); //70

//		printf("BGBBTJ_AVI_EncodeFrame: Size=%d\n", sz);
	}

	if(ctx->vid_codec_ctx)
	{
		sz=BGBBTJ_Codec_CompressFrame(ctx->vid_codec_ctx,
			rgba, tbuf, 1<<22, qf, 0, &fl);
		qf&=(~(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME));
		qf|=fl&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME);
	}

	if(sz==-999)
	{
		ctx->vid_codec_ctx=BGBBTJ_Codec_BeginCompress(
			ctx->vidh->fccHandler, ctx->vidf, ctx->vidf);

		if(ctx->vid_codec_ctx)
		{
			sz=BGBBTJ_Codec_CompressFrame(ctx->vid_codec_ctx,
				rgba, tbuf, 1<<22, qf, 0, &fl);
			qf&=(~(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME));
			qf|=fl&(BGBBTJ_QFL_IFRAME|BGBBTJ_QFL_PFRAME);
		}
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
		BGBBTJ_AVI_EmitCodedFrame(ctx, tbuf, sz, qf);
		BGBBTJ_AVI_EmitAudioFrame(ctx);
		ctx->accDt2-=1.0/ctx->fps;
	}
}

BGBBTJ_API int BGBBTJ_AVI_WriteContextStereoSamples(
	BGBBTJ_AVICtx *ctx, short *buf, int cnt)
{
	int i, j, k;

	if(!ctx)
		return(-1);
		
	if(!ctx->audSampleBuf)
	{
//		i=2*44100;
		i=2*65536;

		ctx->audSampleBufSz=i;
		ctx->audSampleBufStartPos=0;
		ctx->audSampleBufEndPos=0;

		ctx->audSampleBuf=malloc(i*2*sizeof(short));
	}
	
	for(i=0; i<cnt; i++)
	{
		j=(ctx->audSampleBufEndPos+i)%
			ctx->audSampleBufSz;
		ctx->audSampleBuf[j*2+0]=buf[i*2+0];
		ctx->audSampleBuf[j*2+1]=buf[i*2+1];
	}
	j=(ctx->audSampleBufEndPos+cnt)%
		ctx->audSampleBufSz;
	ctx->audSampleBufEndPos=j;
	
	return(0);
}


void BGBBTJ_AVI_Scale800x600to320x240(byte *ibuf, byte *obuf)
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
int BGBBTJ_AVI_EncoderLoop(void *p)
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
	
	printf("BGBBTJ_AVI_EncoderLoop\n");
	
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
			BGBBTJ_AVI_CloseOutStream(pdgl_avi_cap);
			pdgl_avi_cap=NULL;
			continue;
		}
#endif

		if(pdgl_avi_cap->capReqClose)
		{
			printf("BGBBTJ_AVI_EncoderLoop: Close Stream\n");

			BGBBTJ_AVI_CloseOutStream(pdgl_avi_cap);
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

		t0=BGBBTJ_TimeMS();

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

		BGBBTJ_AVI_EncodeFrame(pdgl_avi_cap,
			pdgl_avi_cap->capFrameBuf2,
			pdgl_avi_cap->xs, pdgl_avi_cap->ys);
		pdgl_avi_cap->capFrameLastIdx=
			pdgl_avi_cap->capFrameIdx;
			
		t1=BGBBTJ_TimeMS();
		pdgl_avi_enctime=t1-t0;

		thSleep(5);
	}

	pdgl_avi_encloop--;
	return(0);
}
#endif

BGBBTJ_API int BGBBTJ_AVI_GetEncodeTime()
	{ return(pdgl_avi_enctime); }

BGBBTJ_API int BGBBTJ_AVI_WriteStereoSamples(short *buf, int cnt)
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
BGBBTJ_API int BGBBTJ_AVI_Screenshot()
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
//	BGBBTJ_AVI_EncodeFrame(pdgl_avi_cap, buf, w0, h0);

//	BGBBTJ_AVI_Scale800x600to320x240(buf, buf1);
//	BGBBTJ_AVI_EncodeFrame(pdgl_avi_cap, buf1, w1, h1);

//	glFinish();
//	glReadPixels (0, 0, w0, h0, GL_RGBA, GL_UNSIGNED_BYTE, buf); 
//	v=1;

	return(0);
}


BGBBTJ_API int BGBBTJ_AVI_DoScreenshotFrame(float dt)
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
			BGBBTJ_AVI_CloseOutStream(pdgl_avi_cap);
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

//		pdgl_avi_cap=BGBBTJ_AVI_OpenOutStream(buf, 320, 240, 10);
//		pdgl_avi_cap=BGBBTJ_AVI_OpenOutStream(buf, 800, 600, 12);
		pdgl_avi_cap=BGBBTJ_AVI_OpenOutStream(buf, xs, ys, 12);
//		t=0;
		pdgl_avi_cap->accDt=0;

		if(pdgl_avi_encloop<=0)
		{
			thThread(BGBBTJ_AVI_EncoderLoop, NULL);
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
			BGBBTJ_AVI_Screenshot();
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

BGBBTJ_API int BGBBTJ_AVI_NoCapture()
{
	if(pdgl_avi_cap)
	{
		pdgl_avi_cap->capReqClose=1;
	}

#if 0
	if(pdgl_avi_cap)
	{
		BGBBTJ_AVI_CloseOutStream(pdgl_avi_cap);
		pdgl_avi_cap=NULL;
		pdgl_avi_nbase=0;
		pdgl_avi_nseq=0;
	}
#endif
}

#endif


BGBBTJ_API int BGBBTJ_Tex_Resample(
	byte *src, int iw, int ih,
	byte *dst, int ow, int oh)
{
	float xs, ys, xc, yc;
	float ix, iy, fx, fy, fxn, fyn;
	float fxya, fxyb, fxyc, fxyd;
	int i, j, k, l, ik, il;
	int la, lb, lc, ld, iw4;

//	return(0);

	xs=((float)iw/(float)ow);
	ys=((float)ih/(float)oh);
	iw4=iw*4;

	for(i=0; i<oh; i++)
	{
		iy=i*ys;
		ik=iy; fy=iy-ik; fyn=1-fy;

		for(j=0; j<ow; j++)
		{
			ix=j*xs;
			il=ix; fx=ix-il;
			l=(ik*iw+il)*4;
			fxn=1-fx;

			fxya=fxn*fyn;	fxyb=fx*fyn;
			fxyc=fxn*fy;	fxyd=fx*fy;
			la=l;			lb=l+4;
			lc=l+iw4;		ld=l+iw4+4;

			for(k=0; k<4; k++)
				dst[k]=
					src[la+k]*fxya+
					src[lb+k]*fxyb+
					src[lc+k]*fxyc+
					src[ld+k]*fxyd;
			dst+=4;
		}
	}
	return(0);
}


BGBBTJ_API int BGBBTJ_Tex_SplinePolateRGBA8(byte *src, int w, int h,
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
	return(0);
}

BGBBTJ_API int BGBBTJ_Tex_ResampleSpline(byte *src, int iw, int ih, byte *dst, int ow, int oh)
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
		BGBBTJ_Tex_SplinePolateRGBA8(src, iw, ih, j*xs, i*ys, tv);
		k=tv[0]; if(k<0)k=0; if(k>255)k=255; dst[0]=k;
		k=tv[1]; if(k<0)k=0; if(k>255)k=255; dst[1]=k;
		k=tv[2]; if(k<0)k=0; if(k>255)k=255; dst[2]=k;
		k=tv[3]; if(k<0)k=0; if(k>255)k=255; dst[3]=k;

		dst+=4;
	}
	return(0);
}

#if 1
BGBBTJ_API int BGBBTJ_Tex_HalfSample(byte *src, int w, int h)
{
	int w2, h2, i2, j2;
	int a1, a2, a3, a4;
	int af1, af2, af3, af4;
	int i, j, k;

	w2=w>>1;
	h2=h>>1;
	for(i=0;i<h2; i++)
		for(j=0; j<w2; j++)
	{
		i2=i<<1;
		j2=j<<1;

		a1=src[((i2*w+j2)<<2)+3];
		a2=src[((i2*w+(j2+1))<<2)+3];
		a3=src[(((i2+1)*w+j2)<<2)+3];
		a4=src[(((i2+1)*w+(j2+1))<<2)+3];

		k=a1+a2+a3+a4;
		if(!k || (k==1020))
		{
//			af1=1023;
//			af2=1023;
//			af3=1023;
//			af4=1023;

			af1=1024;
			af2=1024;
			af3=1024;
			af4=1024;
		}else
		{
//			af1=(4095*a1)/k;
//			af2=(4095*a2)/k;
//			af3=(4095*a3)/k;
//			af4=(4095*a4)/k;

			af1=(4096*a1)/k;
			af2=(4096*a2)/k;
			af3=(4096*a3)/k;
			af4=(4096*a4)/k;
		}
		
		src[((i*w2+j)<<2)+0]=
			(src[((i2*w+j2)<<2)+0]*af1+
			src[((i2*w+(j2+1))<<2)+0]*af2+
			src[(((i2+1)*w+j2)<<2)+0]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+0]*af4)>>12;

		src[((i*w2+j)<<2)+1]=
			(src[((i2*w+j2)<<2)+1]*af1+
			src[((i2*w+(j2+1))<<2)+1]*af2+
			src[(((i2+1)*w+j2)<<2)+1]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+1]*af4)>>12;

		src[((i*w2+j)<<2)+2]=
			(src[((i2*w+j2)<<2)+2]*af1+
			src[((i2*w+(j2+1))<<2)+2]*af2+
			src[(((i2+1)*w+j2)<<2)+2]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+2]*af4)>>12;

		src[((i*w2+j)<<2)+3]=
			(src[((i2*w+j2)<<2)+3]*af1+
			src[((i2*w+(j2+1))<<2)+3]*af2+
			src[(((i2+1)*w+j2)<<2)+3]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+3]*af4)>>12;
	}
	return(0);
}
#endif

#if 1
BGBBTJ_API int BGBBTJ_Tex_HalfSampleDest(
	byte *src, byte *dst, int w, int h)
{
	int w2, h2, i2, j2;
	int a1, a2, a3, a4;
	int af1, af2, af3, af4;
	int i, j, k;

	w2=w>>1;
	h2=h>>1;
	for(i=0;i<h2; i++)
		for(j=0; j<w2; j++)
	{
		i2=i<<1;
		j2=j<<1;

		a1=src[((i2*w+j2)<<2)+3];
		a2=src[((i2*w+(j2+1))<<2)+3];
		a3=src[(((i2+1)*w+j2)<<2)+3];
		a4=src[(((i2+1)*w+(j2+1))<<2)+3];

		k=a1+a2+a3+a4;
		if(!k || (k==1020))
		{
//			af1=1023;
//			af2=1023;
//			af3=1023;
//			af4=1023;

			af1=1024;
			af2=1024;
			af3=1024;
			af4=1024;
		}else
		{
//			af1=(4095*a1)/k;
//			af2=(4095*a2)/k;
//			af3=(4095*a3)/k;
//			af4=(4095*a4)/k;

			af1=(4096*a1)/k;
			af2=(4096*a2)/k;
			af3=(4096*a3)/k;
			af4=(4096*a4)/k;
		}
		
		dst[((i*w2+j)<<2)+0]=
			(src[((i2*w+j2)<<2)+0]*af1+
			src[((i2*w+(j2+1))<<2)+0]*af2+
			src[(((i2+1)*w+j2)<<2)+0]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+0]*af4)>>12;

		dst[((i*w2+j)<<2)+1]=
			(src[((i2*w+j2)<<2)+1]*af1+
			src[((i2*w+(j2+1))<<2)+1]*af2+
			src[(((i2+1)*w+j2)<<2)+1]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+1]*af4)>>12;

		dst[((i*w2+j)<<2)+2]=
			(src[((i2*w+j2)<<2)+2]*af1+
			src[((i2*w+(j2+1))<<2)+2]*af2+
			src[(((i2+1)*w+j2)<<2)+2]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+2]*af4)>>12;

		dst[((i*w2+j)<<2)+3]=
			(src[((i2*w+j2)<<2)+3]*af1+
			src[((i2*w+(j2+1))<<2)+3]*af2+
			src[(((i2+1)*w+j2)<<2)+3]*af3+
			src[(((i2+1)*w+(j2+1))<<2)+3]*af4)>>12;
	}
	return(0);
}
#endif


#if 1
BGBBTJ_API int BGBBTJ_DoubleSample_InterpolatePoint(byte *src,
	int w, int h, int pt)
{
	int ha, hb;
	int va, vb;

	switch(pt)
	{
	case 0:
		ha=192; hb= 64;
		va=192; vb= 64;
		break;
	case 1:
		ha= 64; hb=192;
		va=192; vb= 64;
		break;
	case 2:
		ha=192; hb= 64;
		va= 64; vb=192;
		break;
	case 3:
		ha= 64; hb=192;
		va= 64; vb=192;
		break;
	}
	
	return(BGBBTJ_DoubleSample_InterpolateWeightPoint(src, w, h, hb, vb));
}

BGBBTJ_API int BGBBTJ_DoubleSample_InterpolateWeightPoint(byte *src,
	int w, int h, int hf, int vf)
{
	int r00, r01, r02, r03;
	int r10, r11, r12, r13;
	int r20, r21, r22, r23;
	int r30, r31, r32, r33;

	int s00, s01, s02, s03;
	int s10, s11, s12, s13;
	int s20, s21, s22, s23;
	int s30, s31, s32, s33;

//	int l11, l12, l13;
//	int l21, l22, l23;
//	int l31, l32, l33;
	int p0a, p0b, p0c, p0d;
	int p1a, p1b, p1c, p1d;
	int p2a, p2b, p2c, p2d;
	int p3a, p3b, p3c, p3d;

	int l0a, l0b, l0c, l0d;
	int l1a, l1b, l1c, l1d;

	int q0a, q0b, q0c, q0d;
	int q1a, q1b, q1c, q1d;
	
	int ha, hb;
	int va, vb;
	int p;

	ha=256-hf; hb=hf;
	va=256-vf; vb=vf;

	r00=src[((-1)*w+(-1))*4];	r01=src[((-1)*w+( 0))*4];
	r02=src[((-1)*w+( 1))*4];	r03=src[((-1)*w+( 2))*4];
	r10=src[(( 0)*w+(-1))*4];	r11=src[(( 0)*w+( 0))*4];
	r12=src[(( 0)*w+( 1))*4];	r13=src[(( 0)*w+( 2))*4];
	r20=src[(( 1)*w+(-1))*4];	r21=src[(( 1)*w+( 0))*4];
	r22=src[(( 1)*w+( 1))*4];	r23=src[(( 1)*w+( 2))*4];
	r30=src[(( 2)*w+(-1))*4];	r31=src[(( 2)*w+( 0))*4];
	r32=src[(( 2)*w+( 1))*4];	r33=src[(( 2)*w+( 2))*4];

#if 0
//	s00=(11*r00+2*r01+2*r10+r11+8)>>4;

//	s11=(11*r11+2*r01+2*r10+r00+8)>>4;
//	s12=(11*r12+2*r02+2*r13+r03+8)>>4;

//	s11=(2*r00+5*r01+2*r02+5*r10+32*r11+5*r12+2*r20+5*r21+2*r22+32)>>6;
//	s12=(2*r01+5*r02+2*r03+5*r11+32*r12+5*r13+2*r21+5*r22+2*r23+32)>>6;
//	s21=(2*r10+5*r11+2*r12+5*r20+32*r21+5*r22+2*r30+5*r31+2*r32+32)>>6;
//	s22=(2*r11+5*r12+2*r13+5*r21+32*r22+5*r23+2*r31+5*r32+2*r33+32)>>6;

//	s11=(1*r00+4*r01+1*r02+4*r10+40*r11+4*r12+1*r20+4*r21+1*r22+32)>>6;
//	s12=(1*r01+4*r02+1*r03+4*r11+40*r12+4*r13+1*r21+4*r22+1*r23+32)>>6;
//	s21=(1*r10+4*r11+1*r12+4*r20+40*r21+4*r22+1*r30+4*r31+1*r32+32)>>6;
//	s22=(1*r11+4*r12+1*r13+4*r21+40*r22+4*r23+1*r31+4*r32+1*r33+32)>>6;

//	s11=(1*r00+4*r01+1*r02+4*r10+44*r11+4*r12+1*r20+4*r21+1*r22+32)>>6;
//	s12=(1*r01+4*r02+1*r03+4*r11+44*r12+4*r13+1*r21+4*r22+1*r23+32)>>6;
//	s21=(1*r10+4*r11+1*r12+4*r20+44*r21+4*r22+1*r30+4*r31+1*r32+32)>>6;
//	s22=(1*r11+4*r12+1*r13+4*r21+44*r22+4*r23+1*r31+4*r32+1*r33+32)>>6;

//	s11=(4*r01+4*r10+48*r11+4*r12+4*r21+32)>>6;
//	s12=(4*r02+4*r11+48*r12+4*r13+4*r22+32)>>6;
//	s21=(4*r11+4*r20+48*r21+4*r22+4*r31+32)>>6;
//	s22=(4*r12+4*r21+48*r22+4*r23+4*r32+32)>>6;

	s11=(2*r01+2*r10+56*r11+2*r12+2*r21+32)>>6;
	s12=(2*r02+2*r11+56*r12+2*r13+2*r22+32)>>6;
	s21=(2*r11+2*r20+56*r21+2*r22+2*r31+32)>>6;
	s22=(2*r12+2*r21+56*r22+2*r23+2*r32+32)>>6;

	r11=s11;	r12=s12;
	r21=s22;	r22=s21;
#endif

	l0a=r10+r01-r00;
	l0b=r13+r02-r03;
	l0c=r20+r31-r30;
	l0d=r23+r32-r33;
	
//	l1a=(l0a+r11)>>1;
//	l1b=(l0b+r12)>>1;
//	l1c=(l0c+r21)>>1;
//	l1d=(l0d+r22)>>1;

//	l1a=(5*l0a+11*r11)>>4;
//	l1b=(5*l0b+11*r12)>>4;
//	l1c=(5*l0c+11*r21)>>4;
//	l1d=(5*l0d+11*r22)>>4;

//	l1a=(3*l0a+13*r11+8)>>4;
//	l1b=(3*l0b+13*r12+8)>>4;
//	l1c=(3*l0c+13*r21+8)>>4;
//	l1d=(3*l0d+13*r22+8)>>4;

#if 1
	l1a=2*r11-l0a;
	l1b=2*r12-l0b;
	l1c=2*r21-l0c;
	l1d=2*r22-l0d;

	l1a=(5*l1a+11*r11+8)>>4;
	l1b=(5*l1b+11*r12+8)>>4;
	l1c=(5*l1c+11*r21+8)>>4;
	l1d=(5*l1d+11*r22+8)>>4;

//	l1a=(3*l1a+13*r11+8)>>4;
//	l1b=(3*l1b+13*r12+8)>>4;
//	l1c=(3*l1c+13*r21+8)>>4;
//	l1d=(3*l1d+13*r22+8)>>4;
#endif

//	l1a=(2*l0a+14*r11+8)>>4;
//	l1b=(2*l0b+14*r12+8)>>4;
//	l1c=(2*l0c+14*r21+8)>>4;
//	l1d=(2*l0d+14*r22+8)>>4;

//	l1a=(1*l0a+15*r11+8)>>4;
//	l1b=(1*l0b+15*r12+8)>>4;
//	l1c=(1*l0c+15*r21+8)>>4;
//	l1d=(1*l0d+15*r22+8)>>4;

//	l1a=(1*l0a+63*r11+32)>>6;
//	l1b=(1*l0b+63*r12+32)>>6;
//	l1c=(1*l0c+63*r21+32)>>6;
//	l1d=(1*l0d+63*r22+32)>>6;
	
//	l0a=BGBBTJ_BufPNG_Paeth(r10, r01, r00);
//	l0b=BGBBTJ_BufPNG_Paeth(r13, r02, r03);
//	l0c=BGBBTJ_BufPNG_Paeth(r20, r31, r30);
//	l0d=BGBBTJ_BufPNG_Paeth(r23, r32, r33);

#if 0
	q0a=2*r11-r10;
	q0b=2*r12-r13;
	q0c=2*r21-r20;
	q0d=2*r22-r23;

	q1a=2*r11-r01;
	q1b=2*r12-r02;
	q1c=2*r21-r31;
	q1d=2*r22-r32;
#endif

#if 1
	q0a=2*l1a-r10;
	q0b=2*l1b-r13;
	q0c=2*l1c-r20;
	q0d=2*l1d-r23;

	q1a=2*l1a-r01;
	q1b=2*l1b-r02;
	q1c=2*l1c-r31;
	q1d=2*l1d-r32;
#endif

//	p0a=(hb*q0a+ha*r11)>>2;
//	p0b=(ha*q0b+hb*r12)>>2;
//	p0c=(hb*q0c+ha*r21)>>2;
//	p0d=(ha*q0d+hb*r22)>>2;

//	p1a=(vb*q1a+va*r11)>>2;
//	p1b=(va*q1b+vb*r12)>>2;
//	p1c=(vb*q1c+va*r21)>>2;
//	p1d=(va*q1d+vb*r22)>>2;

#if 0
	p0a=(hb*q0a+ha*l0a+128)>>8;
	p0b=(ha*q0b+hb*l0b+128)>>8;
	p0c=(hb*q0c+ha*l0c+128)>>8;
	p0d=(ha*q0d+hb*l0d+128)>>8;

	p1a=(vb*q1a+va*l0a+128)>>8;
	p1b=(va*q1b+vb*l0b+128)>>8;
	p1c=(vb*q1c+va*l0c+128)>>8;
	p1d=(va*q1d+vb*l0d+128)>>8;
#endif

#if 1
	p0a=(hb*q0a+ha*l1a+128)>>8;
	p0b=(ha*q0b+hb*l1b+128)>>8;
	p0c=(hb*q0c+ha*l1c+128)>>8;
	p0d=(ha*q0d+hb*l1d+128)>>8;

	p1a=(vb*q1a+va*l1a+128)>>8;
	p1b=(va*q1b+vb*l1b+128)>>8;
	p1c=(vb*q1c+va*l1c+128)>>8;
	p1d=(va*q1d+vb*l1d+128)>>8;
#endif

	p2a=(ha*p0a+hb*p0b+128)>>8;
	p2b=(ha*p0c+hb*p0d+128)>>8;
	p2c=(ha*p1a+hb*p1b+128)>>8;
	p2d=(ha*p1c+hb*p1d+128)>>8;
		
	p3a=(va*p2a+vb*p2b+128)>>8;
	p3b=(va*p2c+vb*p2d+128)>>8;
	
	p=(p3a+p3b)>>1;
	if(p<0)p=0;
	if(p>255)p=255;
	return(p);
}

BGBBTJ_API int BGBBTJ_DoubleSample_SoftenPoint(byte *src,
	int w, int h)
{
	int r00, r01, r02;
	int r10, r11, r12;
	int r20, r21, r22;

//	int s00, s01, s02;
//	int s10, s11, s12;
//	int s20, s21, s22;

	int p;

	r00=src[((-1)*w+(-1))*4];	r01=src[((-1)*w+( 0))*4];
	r02=src[((-1)*w+( 1))*4];
	r10=src[(( 0)*w+(-1))*4];	r11=src[(( 0)*w+( 0))*4];
	r12=src[(( 0)*w+( 1))*4];
	r20=src[(( 1)*w+(-1))*4];	r21=src[(( 1)*w+( 0))*4];
	r22=src[(( 1)*w+( 1))*4];

//	p=(1*r00+4*r01+1*r02+4*r10+44*r11+4*r12+1*r20+4*r21+1*r22+32)>>6;
	p=(1*r00+3*r01+1*r02+3*r10+48*r11+3*r12+1*r20+3*r21+1*r22+32)>>6;
//	p=(2*r01+2*r10+56*r11+2*r12+2*r21+32)>>6;

	if(p<0)p=0;
	if(p>255)p=255;
	return(p);
}

int BGBBTJ_DoubleSample_DoublePoint(
	byte *src, byte *dst, int w, int h)
{
	int r, g, b, a;
	
	r=src[0];	g=src[1];
	b=src[2];	a=src[3];
	
	dst[0]=r;	dst[1]=g;
	dst[2]=b;	dst[3]=a;
	dst[4]=r;	dst[5]=g;
	dst[6]=b;	dst[7]=a;

	dst[w*4+0]=r;	dst[w*4+1]=g;
	dst[w*4+2]=b;	dst[w*4+3]=a;
	dst[w*4+4]=r;	dst[w*4+5]=g;
	dst[w*4+6]=b;	dst[w*4+7]=a;
	return(0);
}

BGBBTJ_API int BGBBTJ_Tex_DoubleSample(byte *src, byte *dst, int w, int h)
{
	static byte *tsrc=NULL;
	static byte *tdst=NULL;
	int w2, h2, i2, j2;
//	int a1, a2, a3, a4;
//	int af1, af2, af3, af4;
	int r0, r1, r2, r3;
	int g0, g1, g2, g3;
	int b0, b1, b2, b3;
	int a0, a1, a2, a3;
	int i, j, k;

	w2=w*2;
	h2=h*2;

	if(!tsrc)
		{ tsrc=malloc(w*h*4); }
	if(!tdst)
		{ tdst=malloc(w2*h2*4); }

#if 1
	for(i=0; i<h; i++)
	{
		for(j=0; j<4; j++)
		{
			BGBBTJ_DoubleSample_DoublePoint(
				src+((i*w+j)*4),
				dst+(((i*2)*w2+(j*2))*4),
				w2, h2);
		}

		for(j=w-4; j<w; j++)
		{
			BGBBTJ_DoubleSample_DoublePoint(
				src+((i*w+j)*4),
				dst+(((i*2)*w2+(j*2))*4),
				w2, h2);
		}
	}

	for(j=0; j<w; j++)
	{
		for(i=0; i<4; i++)
		{
			BGBBTJ_DoubleSample_DoublePoint(
				src+((i*w+j)*4),
				dst+(((i*2)*w2+(j*2))*4),
				w2, h2);
		}

		for(i=h-4; i<h; i++)
		{
			BGBBTJ_DoubleSample_DoublePoint(
				src+((i*w+j)*4),
				dst+(((i*2)*w2+(j*2))*4),
				w2, h2);
		}
	}
#endif

#if 0
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		i2=i<<1;
		j2=j<<1;
		
		r0=src[((i*w+j)*4)+0];
		g0=src[((i*w+j)*4)+1];
		b0=src[((i*w+j)*4)+2];
		a0=src[((i*w+j)*4)+3];

		dst[(((i*2+0)*w2+(j*2+0))*4)+0]=r0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+0]=r0;
		dst[(((i*2+1)*w2+(j*2+0))*4)+0]=r0;
		dst[(((i*2+1)*w2+(j*2+1))*4)+0]=r0;

		dst[(((i*2+0)*w2+(j*2+0))*4)+1]=g0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+1]=g0;
		dst[(((i*2+1)*w2+(j*2+0))*4)+1]=g0;
		dst[(((i*2+1)*w2+(j*2+1))*4)+1]=g0;

		dst[(((i*2+0)*w2+(j*2+0))*4)+2]=b0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+2]=b0;
		dst[(((i*2+1)*w2+(j*2+0))*4)+2]=b0;
		dst[(((i*2+1)*w2+(j*2+1))*4)+2]=b0;

		dst[(((i*2+0)*w2+(j*2+0))*4)+3]=a0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+3]=a0;
		dst[(((i*2+1)*w2+(j*2+0))*4)+3]=a0;
		dst[(((i*2+1)*w2+(j*2+1))*4)+3]=a0;
	}
#endif

#if 1
	for(i=1; i<(h-1); i++)
		for(j=1; j<(w-1); j++)
	{
//		r0=src[(((i-1)*w+j)*4)+0];
		r0=BGBBTJ_DoubleSample_SoftenPoint(src+(i*w+j)*4+0, w, h);
		g0=BGBBTJ_DoubleSample_SoftenPoint(src+(i*w+j)*4+1, w, h);
		b0=BGBBTJ_DoubleSample_SoftenPoint(src+(i*w+j)*4+2, w, h);
		tsrc[(i*w+j)*4+0]=r0;
		tsrc[(i*w+j)*4+1]=g0;
		tsrc[(i*w+j)*4+2]=b0;
	}
#endif

	for(i=1; i<(h-2); i++)
		for(j=1; j<(w-2); j++)
	{
		i2=i<<1;
		j2=j<<1;

//		a1=src[(((i+0)*w+(j+0))*4)+3];
//		a2=src[(((i+0)*w+(j+1))*4)+3];
//		a3=src[(((i+1)*w+(j+0))*4)+3];
//		a4=src[(((i+1)*w+(j+1))*4)+3];

		a0=src[((i*w+j)*4)+3];	a1=a0;
		a2=a0; a3=a1;

#if 0
		r0=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+0, w, h, 0);
		r1=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+0, w, h, 1);
		r2=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+0, w, h, 2);
		r3=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+0, w, h, 3);
		g0=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+1, w, h, 0);
		g1=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+1, w, h, 1);
		g2=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+1, w, h, 2);
		g3=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+1, w, h, 3);
		b0=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+2, w, h, 0);
		b1=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+2, w, h, 1);
		b2=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+2, w, h, 2);
		b3=BGBBTJ_DoubleSample_InterpolatePoint(src+(i*w+j)*4+2, w, h, 3);
#endif

#if 1
		r0=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+0, w, h, 0);
		r1=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+0, w, h, 1);
		r2=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+0, w, h, 2);
		r3=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+0, w, h, 3);
		g0=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+1, w, h, 0);
		g1=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+1, w, h, 1);
		g2=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+1, w, h, 2);
		g3=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+1, w, h, 3);
		b0=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+2, w, h, 0);
		b1=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+2, w, h, 1);
		b2=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+2, w, h, 2);
		b3=BGBBTJ_DoubleSample_InterpolatePoint(tsrc+(i*w+j)*4+2, w, h, 3);
#endif

		dst[(((i*2+0)*w2+(j*2+0))*4)+0]=r0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+0]=r1;
		dst[(((i*2+1)*w2+(j*2+0))*4)+0]=r2;
		dst[(((i*2+1)*w2+(j*2+1))*4)+0]=r3;

		dst[(((i*2+0)*w2+(j*2+0))*4)+1]=g0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+1]=g1;
		dst[(((i*2+1)*w2+(j*2+0))*4)+1]=g2;
		dst[(((i*2+1)*w2+(j*2+1))*4)+1]=g3;

		dst[(((i*2+0)*w2+(j*2+0))*4)+2]=b0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+2]=b1;
		dst[(((i*2+1)*w2+(j*2+0))*4)+2]=b2;
		dst[(((i*2+1)*w2+(j*2+1))*4)+2]=b3;

		dst[(((i*2+0)*w2+(j*2+0))*4)+3]=a0;
		dst[(((i*2+0)*w2+(j*2+1))*4)+3]=a1;
		dst[(((i*2+1)*w2+(j*2+0))*4)+3]=a2;
		dst[(((i*2+1)*w2+(j*2+1))*4)+3]=a3;
	}

#if 1
	for(i=1; i<(h2-1); i++)
		for(j=1; j<(w2-1); j++)
	{
//		r0=src[(((i-1)*w+j)*4)+0];
		r0=BGBBTJ_DoubleSample_SoftenPoint(dst+(i*w2+j)*4+0, w2, h2);
		g0=BGBBTJ_DoubleSample_SoftenPoint(dst+(i*w2+j)*4+1, w2, h2);
		b0=BGBBTJ_DoubleSample_SoftenPoint(dst+(i*w2+j)*4+2, w2, h2);
		tdst[(i*w2+j)*4+0]=r0;
		tdst[(i*w2+j)*4+1]=g0;
		tdst[(i*w2+j)*4+2]=b0;
	}

	for(i=1; i<(h-1); i++)
		for(j=1; j<(w-1); j++)
	{
		r0=tdst[(i*w2+j)*4+0];
		g0=tdst[(i*w2+j)*4+1];
		b0=tdst[(i*w2+j)*4+2];
		dst[(i*w2+j)*4+0]=r0;
		dst[(i*w2+j)*4+1]=g0;
		dst[(i*w2+j)*4+2]=b0;
	}
#endif

	return(0);
}
#endif
