#include <lbxgl2.h>
#include <bgbbtj.h>
#include <bgbmid.h>

//int LBXGL_Sound_PlayStream(char *name,
//	int (*getsamples)(short *samples, int cnt, void *data), void *data,
//	int flags, float vol);

LBXGL_AVI_Context *lbxgl_avi_root=NULL;

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

int AVI_DumpMainAVIHeader(LBXGL_MainAVIHeader *avihead)
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

int AVI_DumpAVIStreamHeader(LBXGL_AVIStreamHeader *strhead, int num)
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

int AVI_DumpAVIWaveFmt(LBXGL_WAVEFORMATEX *wavefmt, int num)
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

int AVI_DumpBMIHead(LBXGL_BITMAPINFOHEADER *bmihead, int num)
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

int AVI_ExtractTagNum(int tag)
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

int AVI_Init()
{
	LBXGL_Codec_Init();
	return(0);
}

int avi_getsamples(short *samples, int cnt, void *data)
{
	LBXGL_AVI_Context *ctx;
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

LBXGL_AVI_Context *AVI_LookupAVI(char *name)
{
	LBXGL_AVI_Context *cur;
	
	cur=lbxgl_avi_root;
	while(cur)
	{
		if(!strcmp(name, cur->avi_name))
			return(cur);
		cur=cur->avi_next;
	}
	
	return(NULL);
}

LBXGL_AVI_Context *AVI_LoadAVI(char *name)
{
	int cc1, cc2, sz;
	int i, j, w, h;
	LBXGL_AVI_Context *ctx;
	PDGL_BMPInfoHeader *inhead, *outhead;

	ctx=AVI_LookupAVI(name);
	if(ctx)return(ctx);

	ctx=gctalloc("lbxgl_avi_t", sizeof(LBXGL_AVI_Context));

//	ctx->fd=vffopen(name, "rb");
	ctx->fd=btcl_vffopen(name, "rb");
	if(!ctx->fd)return(NULL);

	ctx->avi_name=dystrdup(name);
	ctx->avi_next=lbxgl_avi_root;
	lbxgl_avi_root=ctx;

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

	AVI_DumpMainAVIHeader(ctx->avihead);

	ctx->ofs_cur=ctx->ofs_hdrl+12;
	ctx->ofs_cur=RIFF_NextChunk(ctx->fd, ctx->ofs_cur);	//avih

	ctx->str_vid=-1;
	ctx->str_aud=-1;
	for(i=0; i<ctx->avihead->dwStreams; i++)
	{
		ctx->strhead=RIFF_FindReadInChunk(ctx->fd, ctx->ofs_cur,
			RIFF_TAG_strh, NULL);
		AVI_DumpAVIStreamHeader(ctx->strhead, i);

		if(ctx->strhead->fccType==RIFF_TAG_auds)
		{
			ctx->wavefmt=RIFF_FindReadInChunk(ctx->fd,
				ctx->ofs_cur, RIFF_TAG_strf, NULL);
			AVI_DumpAVIWaveFmt(ctx->wavefmt, i);
			ctx->audhead=ctx->strhead;
			ctx->str_aud=i;
		}

		if(ctx->strhead->fccType==RIFF_TAG_vids)
		{
			ctx->bmihead=RIFF_FindReadInChunk(ctx->fd,
				ctx->ofs_cur, RIFF_TAG_strf, NULL);
			AVI_DumpBMIHead(ctx->bmihead, i);
			ctx->vidhead=ctx->strhead;
			ctx->str_vid=i;
		}

		ctx->ofs_cur=RIFF_NextChunk(ctx->fd, ctx->ofs_cur);	//strl
	}

	ctx->ofs_movi=RIFF_FindSubChunk(ctx->fd, 0, RIFF_TAG_LIST, RIFF_TAG_movi);
	ctx->avi_index=RIFF_FindReadInChunk(ctx->fd, 0, RIFF_TAG_idx1, &ctx->idxsz);
	printf("AVI: Index %d ents\n", ctx->idxsz/sizeof(LBXGL_AVIINDEXENTRY));

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

		inhead=gcalloc(sizeof(PDGL_BMPInfoHeader));
		outhead=gcalloc(sizeof(PDGL_BMPInfoHeader));
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

		ctx->codec_ctx=LBXGL_Codec_BeginDecompress(i,
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

//	LBXGL_Sound_PlayStream(name, &avi_getsamples, ctx, 0, 1.0);

	return(ctx);
}

LBXGL_Video_Stats *AVI_GetStats(LBXGL_AVI_Context *ctx)
{
	LBXGL_Video_Stats *tmp;
	int i;

	tmp=gcalloc(sizeof(LBXGL_Video_Stats));

	tmp->width=ctx->bmihead->biWidth;
	tmp->height=ctx->bmihead->biHeight;
	tmp->frametime=ctx->avihead->dwMicroSecPerFrame/1000000.0;
	tmp->num_frames=ctx->avihead->dwTotalFrames;

	tmp->vid_fcc=ctx->vidhead->fccHandler;
	tmp->vid_cmpr=ctx->bmihead->biCompression;

	i=ctx->wavefmt->wFormatTag;
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
	tmp->aud_fcc=ctx->audhead->fccHandler;
	tmp->aud_cmpr=i;

	return(tmp);
}

int AVI_DecodeAudio(LBXGL_AVI_Context *ctx, int ofs)
{
	int cc1, cc2, sz, sz2;
	int i, j, k, l, err, bsz;
	byte *buf, *bufe;
	short *sbuf;

	ctx->fcbuf=RIFF_ReadInChunk(ctx->fd, ofs, &sz);

	switch(ctx->wavefmt->wFormatTag)
	{
	case 1:	//PCM wave
		k=sz/(ctx->wavefmt->wBitsPerSample/8)/ctx->wavefmt->nChannels;
		buf=ctx->fcbuf;
		sbuf=ctx->fcbuf;
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
		break;

#if 1
	case 85:	//MP3
		l=0;
		if(!ctx->aud_codec_ctx)
			ctx->aud_codec_ctx=BGBMID_Codec_OpenMP3(2, 44100, 16);
		BGBMID_Codec_Decode(ctx->aud_codec_ctx,
			ctx->fcbuf, sz, ctx->esampbuf, 1<<16, &l);
		ctx->esampbuf+=l;
		break;
#endif

	default:
		break;
	}

	free(ctx->fcbuf);

	return(0);
}

int AVI_DecodeVideo(LBXGL_AVI_Context *ctx, int ofs, int skip)
{
	static int rhack=0;
	int cc1, cc2, sz, sz2;
	int i, j, k, l, w, h, ib, ob;
	byte *buf, *buf2;
	short *sbuf;

	RIFF_ReadChunkInfo(ctx->fd, ofs, &cc1, &sz, &cc2);
//	printf("AVI FRame: @%08X  %.4s %.4s %d\n", ofs, &cc1, &cc2, sz);

	//assume a bad index
	if((sz<0) || (sz>=(1<<22)))
	{
		if(!rhack)
		{
			//try to compensate for encoder bug...
			rhack=1;
			j=ofs-(ctx->ofs_movi+8);
			i=AVI_DecodeVideo(ctx, j, skip);
			rhack=0;
			return(i);
		}
		return(-1);
	}

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
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		LBXGL_MJPEG_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	case RIFF_TAG_BTIC:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		LBXGL_MBTIC_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	case RIFF_TAG_PNG:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		LBXGL_MPNG_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	case RIFF_TAG_azpr:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		LBXGL_RPZA_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	case RIFF_TAG_bt1c:
		w=ctx->bmihead->biWidth;
		h=ctx->bmihead->biHeight;
		LBXGL_BT1C_DecompressFrame(ctx,
			ctx->fcbuf, skip?NULL:ctx->fdbuf,
			sz, w*h*4);
		break;

	default:
		if(ctx->codec_ctx)
		{
			w=ctx->bmihead->biWidth;
			h=ctx->bmihead->biHeight;
			LBXGL_Codec_DecompressFrame(ctx->codec_ctx,
				ctx->fcbuf, skip?NULL:ctx->fdbuf,
				sz, w*h*4);
		}
		break;
	}

	if(ctx->chroma_color && !skip)
		AVI_FilterChroma(ctx, ctx->fdbuf);

	free(ctx->fcbuf);
	return(0);
}

void *AVI_DecodeFrame2(LBXGL_AVI_Context *ctx, int skip)
{
	int cc1, cc2, sz, sz2;
	int i, j, k;
	byte *buf;

	if(ctx->cindex<0)
	{
		ctx->cindex=0;
		ctx->frnum=0;
	}

	for(i=ctx->cindex; i<(ctx->idxsz/sizeof(LBXGL_AVIINDEXENTRY)); i++)
	{
		j=AVI_ExtractTagNum(ctx->avi_index[i].ckid);

		if(j==ctx->str_aud)
		{
			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset+8;
			AVI_DecodeAudio(ctx, j);
		}

		if(j==ctx->str_vid)
		{
			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset+8;
//			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset+12;
//			j=ctx->ofs_movi+ctx->avi_index[i].dwChunkOffset;

			AVI_DecodeVideo(ctx, j, skip);

			ctx->cindex=i+1;
			ctx->frnum++;

			break;
		}
	}

	if(i>=(ctx->idxsz/sizeof(LBXGL_AVIINDEXENTRY)))
	{
		ctx->cindex=-1;
	}

	return(ctx->fdbuf);
}

void *AVI_DecodeFrame(LBXGL_AVI_Context *ctx)
	{ return(AVI_DecodeFrame2(ctx, 0)); }
void *AVI_SkipFrame(LBXGL_AVI_Context *ctx)
	{ return(AVI_DecodeFrame2(ctx, 1)); }

int AVI_SkipFrames(LBXGL_AVI_Context *ctx, int num)
{
	int cc1, cc2, sz, sz2;
	int i, j, k;
	byte *buf;

	if(ctx->cindex<0)
	{
		ctx->cindex=0;
		ctx->frnum=0;
	}

	for(i=ctx->cindex; i<(ctx->idxsz/sizeof(LBXGL_AVIINDEXENTRY)); i++)
	{
		j=AVI_ExtractTagNum(ctx->avi_index[i].ckid);

		if(j==ctx->str_vid)
		{
			ctx->cindex=i+1;
			ctx->frnum++;

			num--;
			if(num<=0)break;
		}
	}

	if(i>=(ctx->idxsz/sizeof(LBXGL_AVIINDEXENTRY)))
	{
		ctx->cindex=-1;
	}

	return(0);
}

int AVI_BackFrames(LBXGL_AVI_Context *ctx, int num)
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
		j=AVI_ExtractTagNum(ctx->avi_index[i].ckid);

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

void *AVI_FrameRaw(LBXGL_AVI_Context *ctx, float dt)
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
		AVI_DecodeFrame(ctx);
		ctx->frame_time+=ctx->avihead->dwMicroSecPerFrame/1000000.0;
		fc=1;
	}
	return(ctx->fdbuf);
}

void *AVI_FrameTexnum_cb(void *data)
{
	LBXGL_AVI_Context *ctx;
	
	ctx=data;
	AVI_FrameTexnumI(ctx, 0);
	pdglFinish();
	ctx->flags&=~LBXGL_AVI_FL_UPDATING;
	return(BGBGC_NULLEND);
}

int AVI_GetFrameTexnum(LBXGL_AVI_Context *ctx)
{
	return(ctx->texnum);
}

int AVI_FrameTexnum(LBXGL_AVI_Context *ctx, float dt)
{
	if(!ctx)return(0);

	if(ctx->texnum<=0)
	{
		//force initial setup frame
		return(AVI_FrameTexnumI(ctx, dt));
	}
	
	if((ctx->frame_time>dt) || (ctx->flags&LBXGL_AVI_FL_UPDATING))
	{
//		return(AVI_FrameTexnumI(ctx, dt));

		//don't update, just adjust time
		ctx->frame_time-=dt;
		return(ctx->texnum);
	}

	return(AVI_FrameTexnumI(ctx, dt));

#if 0
	ctx->frame_time-=dt;
	ctx->flags|=LBXGL_AVI_FL_UPDATING;
	pdglAddJob(AVI_FrameTexnum_cb, ctx);
	return(ctx->texnum);
#endif
}

int AVI_FrameTexnumI(LBXGL_AVI_Context *ctx, float dt)
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

		if((ctx->frame_time+dt1)<=(-0.25))
		{
			AVI_SkipFrame(ctx);
			ctx->frame_time+=dt1;
			continue;
		}

		if((ctx->frame_time+dt1)<=0)
		{
//			AVI_SkipFrame(ctx);
			AVI_DecodeFrame(ctx);
			ctx->frame_time+=dt1;
			continue;
		}

		if(BGBBTJ_BCn_CheckGlSupportAny())
		{
//			if(BGBBTJ_CheckGlExtension(
//				"GL_ARB_texture_compression_bptc") &&
			if(BGBBTJ_BCn_CheckGlSupportBPTC() &&
				(ctx->bmihead->biCompression==RIFF_TAG_bt1c))
			{
				ctx->mjpg_clrs=BGBBTJ_JPG_BC7;
			}else if(BGBBTJ_BCn_CheckGlSupportS3TC())
			{
				ctx->mjpg_clrs=BGBBTJ_JPG_BC3F;
			}
		}

//		ctx->mjpg_clrs=BGBBTJ_JPG_BC3F;
//		ctx->mjpg_clrs=BGBBTJ_JPG_BC7;
		AVI_DecodeFrame(ctx);
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

		ctx->texnum=Tex_LoadTexture3A(&iw, &ih,
			ctx->fdbuf, ctx->texnum, ctx->mjpg_clrs);

		if(ctx->fdbuf_norm)
		{
			iw=ctx->bmihead->biWidth;
			ih=ctx->bmihead->biHeight;
//			ctx->texnum_norm=Tex_LoadTexture3(&iw, &ih,
//				ctx->fdbuf_norm, ctx->texnum_norm);
			ctx->texnum_norm=Tex_LoadTexture3A(&iw, &ih,
				ctx->fdbuf_norm, ctx->texnum_norm, ctx->mjpg_clrs);
		}
		if(ctx->fdbuf_spec)
		{
			iw=ctx->bmihead->biWidth;
			ih=ctx->bmihead->biHeight;
//			ctx->texnum_spec=Tex_LoadTexture3(&iw, &ih,
//				ctx->fdbuf_spec, ctx->texnum_spec);
			ctx->texnum_spec=Tex_LoadTexture3A(&iw, &ih,
				ctx->fdbuf_spec, ctx->texnum_spec, ctx->mjpg_clrs);
		}
		if(ctx->fdbuf_luma)
		{
			iw=ctx->bmihead->biWidth;
			ih=ctx->bmihead->biHeight;
//			ctx->texnum_luma=Tex_LoadTexture3(&iw, &ih,
//				ctx->fdbuf_luma, ctx->texnum_luma);
			ctx->texnum_luma=Tex_LoadTexture3A(&iw, &ih,
				ctx->fdbuf_luma, ctx->texnum_luma, ctx->mjpg_clrs);
		}

//		printf("Tex=%d, dt=%f, fc=%d\n", ctx->texnum, dt, fc);
	}

//	printf("Tex=%d, dt=%f, fc=%d\n", ctx->texnum, dt, fc);

	return(ctx->texnum);
}

int AVI_FrameTexnumNorm(LBXGL_AVI_Context *ctx, float dt)
	{ return(ctx->texnum_norm); }
int AVI_FrameTexnumSpec(LBXGL_AVI_Context *ctx, float dt)
	{ return(ctx->texnum_spec); }
int AVI_FrameTexnumLuma(LBXGL_AVI_Context *ctx, float dt)
	{ return(ctx->texnum_luma); }

int AVI_Seek(LBXGL_AVI_Context *ctx, float dt)
{
	int fc;
	int iw, ih;

	fc=dt/(ctx->avihead->dwMicroSecPerFrame/1000000.0);
	if(fc>0)AVI_SkipFrames(ctx, fc);
	if(fc<0)AVI_BackFrames(ctx, -fc);
	return(0);
}

float AVI_GetPosition(LBXGL_AVI_Context *ctx)
{
	float t;

	t=ctx->frnum*(ctx->avihead->dwMicroSecPerFrame/1000000.0);
	return(t);
}

int AVI_SetChromaKey(LBXGL_AVI_Context *ctx, int r, int g, int b, int rad)
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

int AVI_FilterChroma(LBXGL_AVI_Context *ctx, byte *buf)
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

int AVI_HasAudioP(LBXGL_AVI_Context *ctx)
{
	return(ctx->audhead && ctx->wavefmt);
}

int AVI_GetAudioSampleRate(LBXGL_AVI_Context *ctx)
{
	return(ctx->wavefmt->nSamplesPerSec);
}

void AVI_GetAudioSamples(LBXGL_AVI_Context *ctx, short **rpcm, int *rlen)
{
	int j;
	
	j=(ctx->esampbuf-ctx->sampbuf)/2;
	*rpcm=ctx->sampbuf;
	*rlen=j;
}

void AVI_ClearAudioSamples(LBXGL_AVI_Context *ctx)
{
	ctx->esampbuf=ctx->sampbuf;
}

char *AVI_GetVideoName(LBXGL_AVI_Context *ctx)
{
	return(ctx->avi_name);
}

bool AVI_GetDeltaTick(LBXGL_AVI_Context *ctx)
{
	return(ctx->flags&LBXGL_AVI_FL_DELTATICK);
}

void AVI_ClearDeltaTick(LBXGL_AVI_Context *ctx)
	{ ctx->flags&=~LBXGL_AVI_FL_DELTATICK; }
void AVI_SetDeltaTick(LBXGL_AVI_Context *ctx)
	{ ctx->flags|=LBXGL_AVI_FL_DELTATICK; }

void AVI_ClearAllDeltaTicks(void)
{
	LBXGL_AVI_Context *cur;
	cur=lbxgl_avi_root;
	while(cur)
	{
		cur->flags&=~LBXGL_AVI_FL_DELTATICK;
		cur=cur->avi_next;
	}
}
