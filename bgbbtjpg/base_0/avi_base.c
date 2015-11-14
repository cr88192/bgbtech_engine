#include <bgbbtj.h>

#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
#include <bgbmid.h>
#endif

//int BGBBTJ_Sound_PlayStream(char *name,
//	int (*getsamples)(short *samples, int cnt, void *data), void *data,
//	int flags, float vol);

int RIFF_ReadInt32(VFILE *fd)
{
	int i;

	i=vfgetc(fd);
	i+=vfgetc(fd)<<8;
	i+=vfgetc(fd)<<16;
	i+=vfgetc(fd)<<24;

	return(i);
}

int RIFF_ReadChunkInfo(VFILE *fd, int ofs, int *id, int *sz, int *lid)
{
	vfseek(fd, ofs, 0);

	if(id)*id=RIFF_ReadInt32(fd);
	if(sz)*sz=RIFF_ReadInt32(fd);
	if(lid)*lid=RIFF_ReadInt32(fd);

	return(0);
}

int RIFF_NextChunk(VFILE *fd, int ofs)
{
	int cc1, cc2, sz;

	RIFF_ReadChunkInfo(fd, ofs, &cc1, &sz, &cc2);
	if(sz&1)sz++;

	return(ofs+sz+8);
}

int RIFF_FindSubChunk(VFILE *fd, int ofs, int id, int lid)
{
	int cc1, cc2, sz;
	int end, cur;

	RIFF_ReadChunkInfo(fd, ofs, &cc1, &sz, &cc2);

	end=ofs+sz+8;
	cur=ofs+12;
	while(cur<end)
	{
		RIFF_ReadChunkInfo(fd, cur, &cc1, &sz, &cc2);
		if(cc1==id)if(!lid || (cc2==lid))
			return(cur);
		cur=RIFF_NextChunk(fd, cur);
	}
	return(-1);
}

void *RIFF_ReadInChunk(VFILE *fd, int ofs, int *size)
{
	int cc1, cc2, sz;
	void *buf;

	RIFF_ReadChunkInfo(fd, ofs, &cc1, &sz, &cc2);
	if(size)*size=sz;

	buf=malloc(sz+1024);
	vfseek(fd, ofs+8, 0);
	vfread(buf, 1, sz+1024, fd);

	return(buf);
}

void *RIFF_FindReadInChunk(VFILE *fd, int ofs, int id, int *size)
{
	int ch_ofs;
	void *p;

	ch_ofs=RIFF_FindSubChunk(fd, ofs, id, 0);
	if(ch_ofs<0)
	{
		printf("Missing %04s\n", &id);
		return(NULL);
	}

	p=RIFF_ReadInChunk(fd, ch_ofs, size);
	return(p);
}

int BGBBTJ_AVI_DumpMainAVIHeader(BGBBTJ_MainAVIHeader *avihead)
{
	printf("MainAVIHeader:\n");
	printf("dwMicroSecPerFrame=%d\n", avihead->dwMicroSecPerFrame);
	printf("dwMaxBytesPerSec=%d\n", avihead->dwMaxBytesPerSec);
	printf("dwPaddingGranularity=%d\n", avihead->dwPaddingGranularity);
	printf("dwFlags=%d\n", avihead->dwFlags);
	printf("dwTotalFrames=%d\n", avihead->dwTotalFrames);
	printf("dwInitialFrames=%d\n", avihead->dwInitialFrames);
	printf("dwStreams=%d\n", avihead->dwStreams);
	printf("dwSuggestedBufferSize=%d\n", avihead->dwSuggestedBufferSize);
	printf("dwWidth=%d\n", avihead->dwWidth);
	printf("dwHeight=%d\n", avihead->dwHeight);
	printf("\n");
	return(0);
}

int BGBBTJ_AVI_DumpAVIStreamHeader(BGBBTJ_AVIStreamHeader *strhead, int num)
{
	printf("AVIStreamHeader(%d):\n", num);

	printf("fccType=%0.4s\n", &strhead->fccType);
	printf("fccHandler=%0.4s\n", &strhead->fccHandler);
	printf("dwFlags=%d\n", strhead->dwFlags);
	printf("wPriority=%d\n", strhead->wPriority);
	printf("wLanguage=%d\n", strhead->wLanguage);
	printf("dwInitialFrames=%d\n", strhead->dwInitialFrames);
	printf("dwScale=%d\n", strhead->dwScale);
	printf("dwRate=%d\n", strhead->dwRate);
	printf("dwStart=%d\n", strhead->dwStart);
	printf("dwLength=%d\n", strhead->dwLength);
	printf("dwSuggestedBufferSize=%d\n", strhead->dwSuggestedBufferSize);
	printf("dwQuality=%d\n", strhead->dwQuality);
	printf("dwSampleSize=%d\n", strhead->dwSampleSize);
	printf("rcFrame=left:%d top:%d right:%d bottom:%d\n",
		strhead->rcFrame.left, strhead->rcFrame.top,
		strhead->rcFrame.right, strhead->rcFrame.bottom);
	printf("\n");
	return(0);
}

int BGBBTJ_AVI_DumpAVIWaveFmt(BGBBTJ_WAVEFORMATEX *wavefmt, int num)
{
	printf("WAVEFORMATEX(%d):\n", num);
	printf("wFormatTag=%d\n", wavefmt->wFormatTag);
	printf("nChannels=%d\n", wavefmt->nChannels);
	printf("nSamplesPerSec=%d\n", wavefmt->nSamplesPerSec);
	printf("nAvgBytesPerSec=%d\n", wavefmt->nAvgBytesPerSec);
	printf("nBlockAlign=%d\n", wavefmt->nBlockAlign);
	printf("wBitsPerSample=%d\n", wavefmt->wBitsPerSample);
	printf("cbSize=%d\n", wavefmt->cbSize);
	printf("\n");
	return(0);
}

int BGBBTJ_AVI_DumpBMIHead(BGBBTJ_BITMAPINFOHEADER *bmihead, int num)
{
	printf("BITMAPINFOHEADER(%d):\n", num);
	printf("biSize=%d\n", bmihead->biSize);
	printf("biWidth=%d\n", bmihead->biWidth);
	printf("biHeight=%d\n", bmihead->biHeight);
	printf("biPlanes=%d\n", bmihead->biPlanes);
	printf("biBitCount=%d\n", bmihead->biBitCount);
	printf("biCompression=%0.4s\n", &bmihead->biCompression);
	printf("biSizeImage=%d\n", bmihead->biSizeImage);
	printf("biXPelsPerMeter=%d\n", bmihead->biXPelsPerMeter);
	printf("biYPelsPerMeter=%d\n", bmihead->biYPelsPerMeter);
	printf("biClrUsed=%d\n", bmihead->biClrUsed);
	printf("biClrImportant=%d\n", bmihead->biClrImportant);
	printf("\n");
	return(0);
}

int BGBBTJ_AVI_ExtractTagNum(int tag)
{
	int i, c;

	i=0;

	c=tag&0xff;
	if((c>='0') && (c<='9'))i+=c-'0';
	if((c>='A') && (c<='Z'))i+=c-'A'+10;
	if((c>='a') && (c<='z'))i+=c-'a'+10;

	i<<=4;

	c=(tag>>8)&0xff;
	if((c>='0') && (c<='9'))i+=c-'0';
	if((c>='A') && (c<='Z'))i+=c-'A'+10;
	if((c>='a') && (c<='z'))i+=c-'a'+10;

	return(i);
}

int BGBBTJ_AVI_Init()
{
	BGBBTJ_Codec_Init();
	return(0);
}

int avi_getsamples(short *samples, int cnt, void *data)
{
	BGBBTJ_AVI_Context *ctx;
	int i, i2, j, rl, l, sr;
	short *ss, *st;

	ctx=data;

	sr=ctx->wavefmt->nSamplesPerSec;

	l=(ctx->esampbuf-ctx->sampbuf)/2;
//	l*=44100.0/48000.0;
	l*=44100.0/sr;
	if(cnt<l)l=cnt;

//	printf("avi: mix %d samples, have %d\n", cnt, l);

	if(l<cnt)
	{
//		printf("avi: sample drain\n");
//		ctx->drain+=(cnt-l);
	}

	for(i=0; i<l; i++)
	{
		i2=i;
//		i2*=(48000.0/44100.0);
		i2*=(sr/44100.0);

		j=ctx->sampbuf[i2*2];
//		j=(ctx->sampbuf[i2*2]+ctx->sampbuf[(i2*2)+1])/2;
//		if(j<-32767)j=-32767;
//		if(j>32767)j=32767;
		samples[i]=j;
	}


	i2=cnt;
//	i2*=(48000.0/44100.0);
	i2*=(sr/44100.0);
	rl=(ctx->esampbuf-ctx->sampbuf)/2;
	if(i2<rl)rl=i2;

	ss=ctx->sampbuf+(rl*2);
	st=ctx->sampbuf;

	while(ss<ctx->esampbuf)*st++=*ss++;
	ctx->esampbuf=st;

	return(l);
}

BGBBTJ_API BGBBTJ_AVI_Context *BGBBTJ_AVI_LoadAVI(char *name)
{
	int cc1, cc2, sz;
	int i, j, w, h;
	BGBBTJ_AVI_Context *ctx;
	BGBBTJ_BMPInfoHeader *inhead, *outhead;

	ctx=gctalloc("lbxgl_avi_t", sizeof(BGBBTJ_AVI_Context));

	ctx->fd=vffopen(name, "rb");
//	ctx->fd=vffopen(name, "rb");
	if(!ctx->fd)return(NULL);

	RIFF_ReadChunkInfo(ctx->fd, 0, &cc1, &sz, &cc2);
	if((cc1!=RIFF_TAG_RIFF) || (cc2!=RIFF_TAG_AVI))
	{
		printf("AVI: Not AVI\n");
		return(NULL);
	}

	ctx->ofs_hdrl=RIFF_FindSubChunk(ctx->fd, 0,
		RIFF_TAG_LIST, RIFF_TAG_hdrl);
	ctx->avihead=RIFF_FindReadInChunk(ctx->fd, ctx->ofs_hdrl,
		RIFF_TAG_avih, NULL);

	BGBBTJ_AVI_DumpMainAVIHeader(ctx->avihead);

	ctx->ofs_cur=ctx->ofs_hdrl+12;
	ctx->ofs_cur=RIFF_NextChunk(ctx->fd, ctx->ofs_cur);	//avih

	ctx->str_vid=-1;
	ctx->str_aud=-1;
	for(i=0; i<ctx->avihead->dwStreams; i++)
	{
		ctx->strhead=RIFF_FindReadInChunk(ctx->fd, ctx->ofs_cur,
			RIFF_TAG_strh, NULL);
		BGBBTJ_AVI_DumpAVIStreamHeader(ctx->strhead, i);

		if(ctx->strhead->fccType==RIFF_TAG_auds)
		{
			ctx->wavefmt=RIFF_FindReadInChunk(ctx->fd,
				ctx->ofs_cur, RIFF_TAG_strf, NULL);
			BGBBTJ_AVI_DumpAVIWaveFmt(ctx->wavefmt, i);
			ctx->audhead=ctx->strhead;
			ctx->str_aud=i;
		}

		if(ctx->strhead->fccType==RIFF_TAG_vids)
		{
			ctx->bmihead=RIFF_FindReadInChunk(ctx->fd,
				ctx->ofs_cur, RIFF_TAG_strf, NULL);
			BGBBTJ_AVI_DumpBMIHead(ctx->bmihead, i);
			ctx->vidhead=ctx->strhead;
			ctx->str_vid=i;
		}

		ctx->ofs_cur=RIFF_NextChunk(ctx->fd, ctx->ofs_cur);	//strl
	}

	ctx->ofs_movi=RIFF_FindSubChunk(ctx->fd, 0, RIFF_TAG_LIST, RIFF_TAG_movi);
	ctx->avi_index=RIFF_FindReadInChunk(ctx->fd, 0, RIFF_TAG_idx1, &ctx->idxsz);
	printf("AVI: Index %d ents\n", ctx->idxsz/sizeof(BGBBTJ_AVIINDEXENTRY));

	w=ctx->bmihead->biWidth;
	h=ctx->bmihead->biHeight;
	if(w<0)w=-w;
	if(h<0)h=-h;

	switch(ctx->bmihead->biCompression)
	{
	case 0:
		break;

	default:
		printf("AVI: codec api\n");
		i=ctx->vidhead->fccHandler;
//		if(i==RIFF_MAKETAG('d','i','v','3'))
//			i=RIFF_MAKETAG('d','i','v','x');

		inhead=gcalloc(sizeof(BGBBTJ_BMPInfoHeader));
		outhead=gcalloc(sizeof(BGBBTJ_BMPInfoHeader));
//		inhead->biWidth=ctx->bmihead->biWidth;
//		inhead->biHeight=ctx->bmihead->biHeight;
		inhead->biWidth=w;
		inhead->biHeight=h;
		inhead->biPlanes=ctx->bmihead->biPlanes;
		inhead->biBitCount=ctx->bmihead->biBitCount;
		inhead->biCompression=ctx->bmihead->biCompression;

//		outhead->biWidth=ctx->bmihead->biWidth;
//		outhead->biHeight=ctx->bmihead->biHeight;
		outhead->biWidth=w;
		outhead->biHeight=h;
		outhead->biPlanes=1;
		outhead->biBitCount=32;
		outhead->biCompression=0;

		ctx->codec_ctx=BGBBTJ_Codec_BeginDecompress(i,
			inhead, outhead);
		break;
	}

	ctx->fdbuf=malloc(w*h*4);
	ctx->fadbuf=malloc(16384);

	ctx->frnum=0;
	ctx->cindex=0;
	ctx->frame_time=0;
	ctx->texnum=-1;
	ctx->mjpg_clrs=BGBBTJ_JPG_RGBA;

	ctx->sampbuf=gcalloc(1<<18);
	ctx->esampbuf=ctx->sampbuf;
	ctx->sz_sampbuf=(1<<18)/4;

//	BGBBTJ_Sound_PlayStream(name, &avi_getsamples, ctx, 0, 1.0);

	return(ctx);
}

BGBBTJ_API BGBBTJ_Video_Stats *BGBBTJ_AVI_GetStats(BGBBTJ_AVI_Context *ctx)
{
	BGBBTJ_Video_Stats *tmp;
	int i;

	tmp=gcalloc(sizeof(BGBBTJ_Video_Stats));

	tmp->width=ctx->bmihead->biWidth;
	tmp->height=ctx->bmihead->biHeight;
	tmp->frametime=ctx->avihead->dwMicroSecPerFrame/1000000.0;
	tmp->num_frames=ctx->avihead->dwTotalFrames;

	tmp->vid_fcc=ctx->vidhead->fccHandler;
	tmp->vid_cmpr=ctx->bmihead->biCompression;

	i=0;
	if(ctx->wavefmt)
		{ i=ctx->wavefmt->wFormatTag; }
	switch(i)
	{
	case 1:
		i=RIFF_MAKETAG('P', 'C', 'M', ' ');
		break;
	case 85:
		i=RIFF_MAKETAG('M', 'P', '3', ' ');
		break;
	default:
		i=RIFF_MAKEHEXTAG(i);
		break;
	}
//	tmp->aud_fcc=ctx->audhead->fccHandler;
	tmp->aud_cmpr=i;

	i=0;
	if(ctx->audhead)
		{ i=ctx->audhead->fccHandler; }
	tmp->aud_fcc=i;

	return(tmp);
}

int BGBBTJ_AVI_DecodeAudio(BGBBTJ_AVI_Context *ctx, int ofs)
{
	static int rhack=0;
	int cc1, cc2, sz, sz2, bsz;
	int i, j, k, l, err;
	byte *buf, *bufe;
	short *sbuf;

	RIFF_ReadChunkInfo(ctx->fd, ofs, &cc1, &sz, &cc2);
//	printf("AVI Frame: @%08X  %.4s %.4s %d\n", ofs, &cc1, &cc2, sz);

	//assume a bad index
	if((sz<0) || (sz>=(1<<22)) ||
		(cc1!=RIFF_MAKETAG('0', '1', 'w', 'b')))
	{
		if(!rhack)
		{
			//try to compensate for encoder bug...
			rhack=1;
			j=ofs-(ctx->ofs_movi+8);
//			printf("B:\n");
			i=BGBBTJ_AVI_DecodeAudio(ctx, j);
			rhack=0;
			return(i);
		}
		return(-1);
	}

	ctx->fcbuf=RIFF_ReadInChunk(ctx->fd, ofs, &sz);

	switch(ctx->wavefmt->wFormatTag)
	{
	case 1:	//PCM wave
		k=sz/(ctx->wavefmt->wBitsPerSample/8)/ctx->wavefmt->nChannels;
		buf=ctx->fcbuf;
		sbuf=ctx->fcbuf;
		
		//skip if buffer would overflow
		if((ctx->esampbuf+(k*2))>=(ctx->sampbuf+ctx->sz_sampbuf*2))
			break;
		
		for(i=0; i<k; i++)
		{
			if(ctx->wavefmt->wBitsPerSample==8)
			{
				j=(buf[i*ctx->wavefmt->nChannels]-128)*256;
				if(j<-32767)j=-32767;
				if(j>32767)j=32767;
			}else
			{
				j=sbuf[i*ctx->wavefmt->nChannels];
			}
			ctx->esampbuf[i*2]=j;

			if(ctx->wavefmt->nChannels>1)
			{
				if(ctx->wavefmt->wBitsPerSample==8)
				{
					j=(buf[i*ctx->wavefmt->nChannels+1]
						-128)*256;
					if(j<-32767)j=-32767;
					if(j>32767)j=32767;
				}else
				{
					j=sbuf[i*ctx->wavefmt->nChannels+1];
				}
			}
			ctx->esampbuf[(i*2)+1]=j;
		}
		ctx->esampbuf+=k*2;
		break;

	case 17:
#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
		if(ctx->wavefmt->nChannels>1)
		{
			buf=ctx->fcbuf;
			bufe=buf+sz;
			bsz=ctx->wavefmt->nBlockAlign;
			l=BGBMID_MsImaAdpcm_StereoSamplesFromBlockSize(bsz);
			while(buf<bufe)
			{
				BGBMID_MsImaAdpcm_DecodeBlockStereo(
					buf, ctx->esampbuf, l);
				ctx->esampbuf+=l*2;
				buf+=bsz;
			}
		}else
		{
			buf=ctx->fcbuf;
			bufe=buf+sz;
			bsz=ctx->wavefmt->nBlockAlign;
			l=BGBMID_MsImaAdpcm_MonoSamplesFromBlockSize(bsz);
			while(buf<bufe)
			{
				BGBMID_MsImaAdpcm_DecodeBlockMonoAsStereo(
					buf, ctx->esampbuf, l);
				ctx->esampbuf+=l*2;
				buf+=bsz;
			}
		}
#endif
		break;

	case 85:	//MP3
		l=0;
#if !defined(BGBBTJ_DRV) && !defined(BGBBTJ_NODY)
		if(!ctx->aud_codec_ctx)
			ctx->aud_codec_ctx=BGBMID_Codec_OpenMP3(2, 44100, 16);
		BGBMID_Codec_Decode(ctx->aud_codec_ctx,
			ctx->fcbuf, sz, ctx->esampbuf, 1<<16, &l);
#endif
		ctx->esampbuf+=l;
		break;

	default:
		break;
	}

	free(ctx->fcbuf);

	return(0);
}

int BGBBTJ_AVI_DecodeVideo(BGBBTJ_AVI_Context *ctx,
	int ofs, int skip, int clrs)
{
	static int rhack=0;
	int cc1, cc2, sz, sz2;
	int i, j, k, l, w, h, ib, ob;
	byte *buf, *buf2;
	short *sbuf;

	RIFF_ReadChunkInfo(ctx->fd, ofs, &cc1, &sz, &cc2);
//	printf("AVI Frame: @%08X  %.4s %.4s %d\n", ofs, &cc1, &cc2, sz);

	//assume a bad index
	if((sz<0) || (sz>=(1<<22)) ||
		(cc1!=RIFF_MAKETAG('0', '0', 'd', 'c')))
	{
		if(!rhack)
		{
			//try to compensate for encoder bug...
			rhack=1;
			j=ofs-(ctx->ofs_movi+8);
//			printf("B:\n");
			i=BGBBTJ_AVI_DecodeVideo(ctx, j, skip, clrs);
			rhack=0;
			return(i);
		}
		return(-1);
	}

//	printf("AVI Frame: @%08X  %.4s %.4s %d\n", ofs, &cc1, &cc2, sz);

	ctx->fcbuf=RIFF_ReadInChunk(ctx->fd, ofs, &sz);

	switch(ctx->bmihead->biCompression)
	{
	case 0:
		buf=ctx->fcbuf;
		buf2=ctx->fdbuf;
		k=ctx->bmihead->biBitCount/8;
		l=ctx->bmihead->biWidth*ctx->bmihead->biHeight;

		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;

		if(sz<(k*l))break;

		for(i=0; i<ctx->bmihead->biHeight; i++)
		{
			ib=(((h-i-1)*w)*k);
			ob=((i*w)*4);
			for(j=0; j<ctx->bmihead->biWidth; j++)
			{
				buf2[ob+(j*4)+0]=buf[ib+(j*k)+2];
				buf2[ob+(j*4)+1]=buf[ib+(j*k)+1];
				buf2[ob+(j*4)+2]=buf[ib+(j*k)+0];
				buf2[ob+(j*4)+3]=255;
			}
		}
		break;

	case RIFF_TAG_JPEG:
	case RIFF_TAG_BTJP:
	case RIFF_TAG_MJPG:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		BGBBTJ_MJPEG_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	case RIFF_TAG_BTIC:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		BGBBTJ_MBTIC_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	case RIFF_TAG_PNG:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		BGBBTJ_MPNG_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	case RIFF_TAG_azpr:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		BGBBTJ_RPZA_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4, clrs);
		break;

	case RIFF_TAG_bt1c:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		BGBBTJ_BT1C_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4, clrs);
		break;

	default:
		if(ctx->codec_ctx)
		{
			w=ctx->bmihead->biWidth;
			h=ctx->bmihead->biHeight;
			BGBBTJ_Codec_DecompressFrame(ctx->codec_ctx,
				ctx->fcbuf, skip?NULL:ctx->fdbuf,
				sz, w*h*4);
		}
		break;
	}

	if(ctx->chroma_color && !skip)
		BGBBTJ_AVI_FilterChroma(ctx, ctx->fdbuf);

	free(ctx->fcbuf);
	return(0);
}

BGBBTJ_API void *BGBBTJ_AVI_DecodeFrame2(
	BGBBTJ_AVI_Context *ctx, int skip, int clrs)
{
	int cc1, cc2, sz, sz2;
	int i, j, k;
	byte *buf;

	if(ctx->cindex<0)
	{
		ctx->cindex=0;
		ctx->frnum=0;
	}

	for(i=ctx->cindex; i<(ctx->idxsz/sizeof(BGBBTJ_AVIINDEXENTRY)); i++)
	{
		j=BGBBTJ_AVI_ExtractTagNum(ctx->avi_index[i].ckid);

		if(j==ctx->str_aud)
		{
			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset+8;
			BGBBTJ_AVI_DecodeAudio(ctx, j);
		}

		if(j==ctx->str_vid)
		{
			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset+8;
//			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset+12;
//			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset;

			BGBBTJ_AVI_DecodeVideo(ctx, j, skip, clrs);

			ctx->cindex=i+1;
			ctx->frnum++;

			break;
		}
	}

	if(i>=(ctx->idxsz/sizeof(BGBBTJ_AVIINDEXENTRY)))
	{
		ctx->cindex=-1;
	}

	return(ctx->fdbuf);
}

BGBBTJ_API void *BGBBTJ_AVI_DecodeFrame(BGBBTJ_AVI_Context *ctx)
	{ return(BGBBTJ_AVI_DecodeFrame2(ctx, 0, BGBBTJ_JPG_RGBA)); }
BGBBTJ_API void *BGBBTJ_AVI_SkipFrame(BGBBTJ_AVI_Context *ctx)
	{ return(BGBBTJ_AVI_DecodeFrame2(ctx, 1, BGBBTJ_JPG_RGBA)); }

BGBBTJ_API int BGBBTJ_AVI_SkipFrames(BGBBTJ_AVI_Context *ctx, int num)
{
	int cc1, cc2, sz, sz2;
	int i, j, k;
	byte *buf;

	if(ctx->cindex<0)
	{
		ctx->cindex=0;
		ctx->frnum=0;
	}

	for(i=ctx->cindex; i<(ctx->idxsz/sizeof(BGBBTJ_AVIINDEXENTRY)); i++)
	{
		j=BGBBTJ_AVI_ExtractTagNum(ctx->avi_index[i].ckid);

		if(j==ctx->str_vid)
		{
			ctx->cindex=i+1;
			ctx->frnum++;

			num--;
			if(num<=0)break;
		}
	}

	if(i>=(ctx->idxsz/sizeof(BGBBTJ_AVIINDEXENTRY)))
	{
		ctx->cindex=-1;
	}

	return(0);
}

BGBBTJ_API int BGBBTJ_AVI_BackFrames(BGBBTJ_AVI_Context *ctx, int num)
{
	int cc1, cc2, sz, sz2;
	int i, j, k;
	byte *buf;

	if(ctx->cindex<0)
	{
		ctx->cindex=0;
		ctx->frnum=0;
	}

	for(i=ctx->cindex; i>0; i--)
	{
		j=BGBBTJ_AVI_ExtractTagNum(ctx->avi_index[i].ckid);

		if(j==ctx->str_vid)
		{
			ctx->cindex=i;
			ctx->frnum--;

			num--;
			if(num<=0)break;
		}
	}

	return(0);
}

BGBBTJ_API void *BGBBTJ_AVI_FrameRaw(BGBBTJ_AVI_Context *ctx, float dt)
{
	int fc;
	int iw, ih;

	if(!ctx)return(NULL);

//	if((dt<=0) || (dt>=10))
//		return(ctx->fdbuf);

	fc=0;
	ctx->frame_time-=dt;
	while(ctx->frame_time<=0)
	{
		BGBBTJ_AVI_DecodeFrame(ctx);
		ctx->frame_time+=ctx->avihead->dwMicroSecPerFrame/1000000.0;
		fc=1;
	}
	return(ctx->fdbuf);
}

#if 0
void *BGBBTJ_AVI_FrameTexnum_cb(void *data)
{
	BGBBTJ_AVI_Context *ctx;
	
	ctx=data;
	BGBBTJ_AVI_FrameTexnumI(ctx, 0);
//	glFinish();
	ctx->flags&=~BGBBTJ_AVI_FL_UPDATING;
	return(BGBGC_NULLEND);
}
#endif

#if 0
int BGBBTJ_AVI_FrameTexnum(BGBBTJ_AVI_Context *ctx, float dt)
{
	if(!ctx)return(0);

	if(ctx->texnum<=0)
	{
		//force initial setup frame
		return(AVI_FrameTexnumI(ctx, dt));
	}
	
	if((ctx->frame_time>dt) || (ctx->flags&BGBBTJ_AVI_FL_UPDATING))
	{
//		return(AVI_FrameTexnumI(ctx, dt));

		//don't update, just adjust time
		ctx->frame_time-=dt;
		return(ctx->texnum);
	}

	return(AVI_FrameTexnumI(ctx, dt));

#if 0
	ctx->frame_time-=dt;
	ctx->flags|=BGBBTJ_AVI_FL_UPDATING;
	pdglAddJob(AVI_FrameTexnum_cb, ctx);
	return(ctx->texnum);
#endif
}
#endif

#if 0
int BGBBTJ_AVI_FrameTexnumI(BGBBTJ_AVI_Context *ctx, float dt)
{
	byte *buf;
	float dt1;
	int fc;
	int iw, ih;
	int i, j;

	if(!ctx)return(0);

//	if((dt<=0) || (dt>=10))return(0);

	fc=0;
	ctx->frame_time-=dt;
	while(ctx->frame_time<=0)
	{
		dt1=ctx->avihead->dwMicroSecPerFrame/1000000.0;
		if((ctx->frame_time+dt1)<=0)
		{
			BGBBTJ_AVI_SkipFrame(ctx);
			ctx->frame_time+=dt1;
			continue;
		}
		ctx->mjpg_clrs=BGBBTJ_JPG_BC3F;
		BGBBTJ_AVI_DecodeFrame(ctx);
		ctx->frame_time+=dt1;
		fc=1;
	}

	if((ctx->texnum<=0) || fc)
	{
		iw=ctx->bmihead->biWidth;
		ih=ctx->bmihead->biHeight;

#if 0
		buf=ctx->fdbuf;
		for(i=0; i<ih; i++)
			for(j=0; j<iw; j++)
		{
//			buf[(i*iw+j)*4+0]=(i&1)?255:0;
//			buf[(i*iw+j)*4+1]=0;
//			buf[(i*iw+j)*4+2]=(j&1)?255:0;
			buf[(i*iw+j)*4+3]=255;
		}
#endif
		
//		if((ctx->texnum<=0) || !(ctx->frnum&1))

//		ctx->texnum=Tex_LoadTexture3(&iw, &ih,
//			ctx->fdbuf, ctx->texnum);

//		ctx->texnum=Tex_LoadTexture3A(&iw, &ih,
//			ctx->fdbuf, ctx->texnum, ctx->mjpg_clrs);

		if(ctx->fdbuf_norm)
		{
			iw=ctx->bmihead->biWidth;
			ih=ctx->bmihead->biHeight;
//			ctx->texnum_norm=Tex_LoadTexture3(&iw, &ih,
//				ctx->fdbuf_norm, ctx->texnum_norm);
//			ctx->texnum_norm=Tex_LoadTexture3A(&iw, &ih,
//				ctx->fdbuf_norm, ctx->texnum_norm, ctx->mjpg_clrs);
		}
		if(ctx->fdbuf_spec)
		{
			iw=ctx->bmihead->biWidth;
			ih=ctx->bmihead->biHeight;
//			ctx->texnum_spec=Tex_LoadTexture3(&iw, &ih,
//				ctx->fdbuf_spec, ctx->texnum_spec);
//			ctx->texnum_spec=Tex_LoadTexture3A(&iw, &ih,
//				ctx->fdbuf_spec, ctx->texnum_spec, ctx->mjpg_clrs);
		}
		if(ctx->fdbuf_luma)
		{
			iw=ctx->bmihead->biWidth;
			ih=ctx->bmihead->biHeight;
//			ctx->texnum_luma=Tex_LoadTexture3(&iw, &ih,
//				ctx->fdbuf_luma, ctx->texnum_luma);
//			ctx->texnum_luma=Tex_LoadTexture3A(&iw, &ih,
//				ctx->fdbuf_luma, ctx->texnum_luma, ctx->mjpg_clrs);
		}

//		printf("Tex=%d, dt=%f, fc=%d\n", ctx->texnum, dt, fc);
	}

//	printf("Tex=%d, dt=%f, fc=%d\n", ctx->texnum, dt, fc);

	return(ctx->texnum);
}

int BGBBTJ_AVI_FrameTexnumNorm(BGBBTJ_AVI_Context *ctx, float dt)
	{ return(ctx->texnum_norm); }
int BGBBTJ_AVI_FrameTexnumSpec(BGBBTJ_AVI_Context *ctx, float dt)
	{ return(ctx->texnum_spec); }
int BGBBTJ_AVI_FrameTexnumLuma(BGBBTJ_AVI_Context *ctx, float dt)
	{ return(ctx->texnum_luma); }
#endif

BGBBTJ_API int BGBBTJ_AVI_Seek(BGBBTJ_AVI_Context *ctx, float dt)
{
	int fc;
	int iw, ih;

	fc=dt/(ctx->avihead->dwMicroSecPerFrame/1000000.0);
	if(fc>0)BGBBTJ_AVI_SkipFrames(ctx, fc);
	if(fc<0)BGBBTJ_AVI_BackFrames(ctx, -fc);
	return(0);
}

BGBBTJ_API float BGBBTJ_AVI_GetPosition(BGBBTJ_AVI_Context *ctx)
{
	float t;

	t=ctx->frnum*(ctx->avihead->dwMicroSecPerFrame/1000000.0);
	return(t);
}

BGBBTJ_API int BGBBTJ_AVI_Restart(BGBBTJ_AVI_Context *ctx)
{
	ctx->cindex=-1;
	ctx->frnum=0;
	return(0);
}

BGBBTJ_API int BGBBTJ_AVI_SetChromaKey(BGBBTJ_AVI_Context *ctx,
	int r, int g, int b, int rad)
{
	int i;

	r=(r>=0)?((r<=255)?r:255):0;
	g=(g>=0)?((g<=255)?g:255):0;
	b=(b>=0)?((b<=255)?b:255):0;
	rad=(rad>=0)?((rad<=255)?rad:255):0;

	i=r|(g<<8)|(b<<16)|(rad<<24);
	ctx->chroma_color=i;
	
	printf("AVI Chroma Color: %08X\n", i);
	
	return(0);
}

BGBBTJ_API int BGBBTJ_AVI_FilterChroma(BGBBTJ_AVI_Context *ctx, byte *buf)
{
	int r, g, b, ra, ra2;
	int dr, dg, db, d;
	int w, h;
	int i;
	
	w=ctx->bmihead->biWidth;
	h=ctx->bmihead->biHeight;
	
	r=ctx->chroma_color&255;
	g=(ctx->chroma_color>>8)&255;
	b=(ctx->chroma_color>>16)&255;
	ra=(ctx->chroma_color>>24)&255;
	ra2=ra*ra;
	
	for(i=0; i<w*h; i++)
	{
		dr=r-buf[i*4+0];
		dg=g-buf[i*4+1];
		db=b-buf[i*4+2];
		d=(dr*dr)+(dg*dg)+(db*db);
		if(d<ra2)
		{
			buf[i*4+3]=0;
		}
	}
	
	return(0);
}
