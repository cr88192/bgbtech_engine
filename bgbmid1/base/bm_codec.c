#include <bgbmid.h>

#include <mpg123.h>
#include <mpglib.h>

BMID_API BGBMID_AudioCodecCtx *BGBMID_Codec_OpenMP3(
	int chan, int rate, int bits)
{
	BGBMID_AudioCodecCtx *ctx;
	struct mpstr *mpctx;
	
	ctx=gctalloc("bgbmid_codecctx_t", sizeof(BGBMID_AudioCodecCtx));
	mpctx=gcalloc(sizeof(struct mpstr));
	ctx->data=mpctx;

	ctx->out_chan=chan;
	ctx->out_rate=rate;
	ctx->out_bits=bits;

	InitMP3(mpctx);
	
	ctx->Decode=BGBMID_Codec_DecodeMP3;
	
	return(ctx);
}

BMID_API int BGBMID_Codec_DecodeMP3(BGBMID_AudioCodecCtx *ctx,
	byte *ibuf, int ibsz, short *obuf, int obsz, int *rlen)
{
	short *tbuf;
	byte *ct, *cts, *cte;
	int ret, sz, ofs, rate, chan, bitrate, lty, l, l2;
	float f, g;
	int i, j, k;

	tbuf=malloc(1<<18);

	cts=(byte *)tbuf;
	ct=cts; cte=cts+(1<<18);

	ofs=statMP3(ibuf, &rate, &chan, &bitrate, &lty);
	ctx->in_chan=chan;
	ctx->in_rate=rate;
	ctx->in_bits=16;

	ret=decodeMP3(ctx->data, ibuf, ibsz, ct, cte-ct, &sz);
	while(ret==MP3_OK)
	{
		ct+=sz;
		ret=decodeMP3(ctx->data, NULL, 0, ct, cte-ct, &sz);
	}
	
	l=(ct-cts)/sizeof(short);
	
	if(ctx->in_rate!=ctx->out_rate)
	{
		f=(ctx->out_rate/((float)(ctx->in_rate)));
		g=1.0/f;
		l2=l*f+0.5;
		for(i=0; i<l2; i++)
		{
			j=i*g;
			obuf[i*2+0]=tbuf[j*2+0];
			obuf[i*2+1]=tbuf[j*2+1];
		}
	}else
	{
		l2=l;
		for(i=0; i<l; i++)
		{
			obuf[i*2+0]=tbuf[i*2+0];
			obuf[i*2+1]=tbuf[i*2+1];
		}
	}
	
	free(tbuf);
	
	if(rlen)*rlen=l2;
	return(0);
}

BMID_API BGBMID_AudioCodecCtx *BGBMID_Codec_Open(u32 fcc,
	int chan, int rate, int bits)
{
	if(fcc==BTAC2_FOURCC('M', 'P', '3', ' '))
		{ return(BGBMID_Codec_OpenMP3(chan, rate, bits)); }
	return(NULL);
}

BMID_API int BGBMID_Codec_Decode(BGBMID_AudioCodecCtx *ctx,
	byte *ibuf, int ibsz, short *obuf, int obsz, int *rlen)
{
	if(ctx->Decode)
		return(ctx->Decode(ctx, ibuf, ibsz, obuf, obsz, rlen));

	if(rlen)*rlen=0;
	return(-1);
}
