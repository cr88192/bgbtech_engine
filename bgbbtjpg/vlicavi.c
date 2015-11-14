/*

Script Commands:

canvas_size <width> <height>
	Set canvas size.

framerate <fps>
	Set video frame-rate.

sequence_begin
	Begin the main video sequence.
	Commands prior to this command are header commands.

sequence_end
	End the main sequence. This effectively ends the video.

sequence_advance <frames>
	Render out a certain number of frames.

tex_avi <texname> <file>
	Import AVI texture.

tex_png <texname> <file>
	Import PNG texture.

tex_startDelay <texname> <seconds>
	Set start delay time in seconds for video-texture.
	Video playback will not start until this time has passed.

tex_playSpeed <texname> <scale>
	Set playback rate for video texture.

tex_pause <texname> [0/1]
	Set paused status of video texture.

tex_restart <texname>
	Restarts playback of a video texture (to the beginning of the sequence).

tex_seek <texname> <delta_seconds>
	Seek within the video texture a certain amount of time.
	Time value is relative to playback speed.

add_layer <name> <tex> <ox> <oy> <xs> <ys>
	Add a new layer to the top of the layer stack.

delete_layer <name>
	Delete a layer from the layer stack.

layer_addMask <layer_name> <masktex> <blend>
	Add a mask to the layer.
	The blend indicates how the mask pixels will be combined
	with the layer image.

	For blending, src is the base pixel, and dst is the mask pixel.

	Modes:
		zero, one, src_color, dst_color, src_alpha, dst_alpha,
		one_minus_src_color, one_minus_dst_color,
		one_minus_src_alpha, one_minus_dst_alpha

		src_red, dst_red,	src_green, dst_green,	src_blue, dst_blue,
		one_minus_src_red, one_minus_dst_red,
		one_minus_src_green, one_minus_dst_green,
		one_minus_src_blue, one_minus_dst_blue,


layer_blend <layer_name> <blend>
	Combined Blend.
	
	Modes:
		add, subtract, multiply, divide
		screen, overlay, difference,
		darken, lighten,
		color_dodge, color_burn,
		invert
	These modes calculate the blended color, and then alpha-blend this
	with the target color.

layer_blend <layer_name> <sfunc> <dfunc>
	Split Blend.
	A blending function is applied to both the source and destination,
	with the combined result used as the output.
	
	Modes:
		zero, one, src_color, dst_color, src_alpha, dst_alpha,
		one_minus_src_color, one_minus_dst_color,
		one_minus_src_alpha, one_minus_dst_alpha

		src_red, dst_red,	src_green, dst_green,	src_blue, dst_blue,
		one_minus_src_red, one_minus_dst_red,
		one_minus_src_green, one_minus_dst_green,
		one_minus_src_blue, one_minus_dst_blue,
		
layer_alpha <layer_name> <alpha>
	Layer Opacity / Alpha.
	Specifies the alpha value from 0-255.
	This is multiplied with the per-pixel alpha as part of the blend.

layer_transformBase <layer_name> <x> <y> <ang>
	Transform layer base by x and y offsets and angle.
	This is relative to the prior transform.

layer_transformTex <layer_name> <x> <y> <ang>
	Transform layer texture by x and y offsets and angle.
	This is relative to the prior transform.

layer_transformResetBase <layer_name>
	Resets base transform back to identity.
layer_transformResetTex <layer_name>
	Resets texture transform back to identity.
	
*/

#include <bgbbtj.h>

#include <time.h>

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

#define FCC_MJPG	FOURCC('M', 'J', 'P', 'G')
#define FCC_JPEG	FOURCC('J', 'P', 'E', 'G')

#define FCC_RGBA	FOURCC('R', 'G', 'B', 'A')
#define FCC_RGBT	FOURCC('R', 'G', 'B', 'T')

#define FCC_BtV0	FOURCC('B', 't', 'V', '0')

#define FCC_rpza	FOURCC('r', 'p', 'z', 'a')
#define FCC_bt1c	FOURCC('b', 't', '1', 'c')

#define FCC_THEO	FOURCC('T', 'H', 'E', 'O')
#define FCC_theo	FOURCC('t', 'h', 'e', 'o')

byte *animavi_framergba;

int help(int argc, char *argv[])
{
}

int main(int argc, char *argv[])
{
	byte tb[1024];
	byte *buf, *lbuf, *tbuf, *tbuf1;
	byte *nvbuf, *spbuf, *lubuf;
	byte *obuf, *onvbuf, *ospbuf, *olubuf;

//	WalInfo_t *wal;
//	BGBBTJ_AVI_Context *ictx;
	BGBBTJ_AVICtx *octx;
//	BGBBTJ_Video_Stats *vstat;
	BTVLIC_DrawContext *vlctx;
	BTVLIC_Texture2D *vltgt;
	byte *base, *chain, *s;
	byte *fdbuf;
	char *inavi;
	char *inscr;
	char *outavi;
	char **a;
//	FILE *fd;
	VFILE *ifd;
	int cy, cu, cv, cr, cg, cb;
	int t0, t1, t2, t3, flags;
	int frszavg, frszlim, frszcnt;
	int frboffs, frbnum;
	int w, h, w1, h1, fcc, qf, qff, qfl, maxfr;
	int i, j, k, l, n;
	float fps;
	double f, g;

	fcc=FCC_MJPG;
//	fcc=FCC_rpza;
//	fcc=FCC_theo;
//	fcc=FCC_THEO;
	flags=0;
	qf=80; qff=0;
	qfl=0;
	maxfr=0;

#if 1
	base=NULL; inavi=NULL; outavi=NULL;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
//			if(!strcmp(argv[i], "-btv"))
//				fcc=FCC_BtV0;

			if(!strcmp(argv[i], "-mtheora"))
				fcc=FCC_theo;
			if(!strcmp(argv[i], "-mrpza"))
				fcc=FCC_rpza;
			if(!strcmp(argv[i], "-mjpg"))
				fcc=FCC_MJPG;

			if(!strcmp(argv[i], "-scale2x"))
				flags|=1;

			if(!strcmp(argv[i], "-bluescreen"))
				flags|=2;
				
			if(!strcmp(argv[i], "-q"))
			{
				qf=atoi(argv[i+1]);
				qff=qf;
				i++; continue;
			}

			if(!strcmp(argv[i], "-maxframe"))
			{
				maxfr=atoi(argv[i+1]);
				i++; continue;
			}

			if(!strcmp(argv[i], "-o"))
			{
				outavi=argv[i+1];
				i++; continue;
			}

			if(!strcmp(argv[i], "-s"))
			{
				inscr=argv[i+1];
				i++; continue;
			}

		}else
		{
//			if(!inavi)
//				{ inavi=argv[i]; continue; }
//			if(!outavi)
//				{ outavi=argv[i]; continue; }
		}
	}
#endif

	qfl|=BGBBTJ_QFL_BATCHENCODE;

	if(!inscr || !outavi)
	{
		help(argc, argv);
		return(-1);
	}
	
	ifd=vffopen(inscr, "rt");
	if(!ifd)
	{
		printf("%s: Can't open input script %s\n", argv[0], inscr);
		return(-1);
	}

	w=256; h=256; fps=10;

	while(!vfeof(ifd))
	{
		vfgets(tb, 1023, ifd);
		printf("$ %s\n", tb);
		a=gcrsplit(tb);
		
		if(!a)continue;
		if(!a[0])continue;
		
		if(!strcmp(a[0], "sequence_begin"))
			break;

		if(!strcmp(a[0], "canvas_size"))
			{ w=atoi(a[1]); h=atoi(a[2]); }
		if(!strcmp(a[0], "framerate"))
			{ fps=atoi(a[1]); }

//		BTVLIC_HandleCommand(vlctx, a);
	}

//	printf("Base=%s\n", base);

//	sprintf(tb, "%s.avi", base);
//	ictx=BGBBTJ_AVI_LoadAVI(inavi);
//	vstat=BGBBTJ_AVI_GetStats(ictx);

//	w=ictx->bmihead->biWidth;
//	h=ictx->bmihead->biHeight;

	octx=BGBBTJ_AVI_OpenOutStream(outavi, w, h, fps, fcc);
//	obuf=malloc(w*h*4);
	obuf=malloc(w*h*4*2);

	vltgt=BTVLIC_TextureFromRGBA(NULL, w, h);
	vlctx=BTVLIC_NewDrawContext(vltgt);

	frszcnt=4;
	frboffs=0;
	frbnum=0;

	printf("\n");
	t0=clock();
	while(!vfeof(ifd))
	{
		vfgets(tb, 1023, ifd);
		printf("$ %s\n", tb);

		a=gcrsplit(tb);
		
		if(!a)continue;
		if(!a[0])continue;
		
		if(!strcmp(a[0], "sequence_end"))
			break;

		if(!strcmp(a[0], "sequence_advance"))
		{
			n=atoi(a[1]);
//			while(n--)
			for(i=0; i<n; i++)
			{
				printf("Frame %d/%d\r", i, n);
				BTVLIC_UpdateContext(vlctx, 1.0/fps);
				BTVLIC_RenderContext(vlctx);
				
				BTVLIC_TextureToRGBA(vltgt, obuf);
//				BTVLIC_TextureToRGBA_Q11_4(vltgt, obuf);
				
				octx->accDt2+=1.0/fps;
				BGBBTJ_AVI_EncodeFrame2(octx, obuf, w, h, qf|qfl, 0);
//				BGBBTJ_AVI_EncodeFrame2(octx, obuf,
//					w, h, qf|qfl, BGBBTJ_JPG_RGBA_Q11_4);
			}
			printf("\n");
			continue;
		}

		BTVLIC_HandleCommand(vlctx, a);

#if 0
		t1=clock();
		f=(t1-t0)/((double)CLOCKS_PER_SEC);
		g=((double)vstat->num_frames)/(ictx->frnum+1);
		g=f*g;
		
		t2=(int)f;
		t3=(int)g;
		
		frszavg=((vftell(octx->fd)-frboffs)/((ictx->frnum-frbnum)+1));

		printf("Fr=%d/%d %d%% "
				"T=%ds(%d:%02d:%02d) "
				"Eta=%ds(%d:%02d:%02d) FrAvg=%.1f/%.1fkB Q=%d \r",
			ictx->frnum, vstat->num_frames,
			(100*ictx->frnum)/vstat->num_frames,
			t2, t2/3600, (t2/60)%60, t2%60,
			t3, t3/3600, (t3/60)%60, t3%60,
			frszavg/1024.0, frszlim/1024.0, qf);

		if((maxfr>0) && (ictx->frnum>=maxfr))
			break;
	
//		fdbuf=BGBBTJ_AVI_DecodeFrame(ictx);

		BGBBTJ_AVI_WriteContextStereoSamples(octx,
			ictx->sampbuf, (ictx->esampbuf-ictx->sampbuf)/2);
		ictx->esampbuf=ictx->sampbuf;


		if(flags&2)
		{
			BGBBTJ_Tex_BlueScreen(obuf, obuf, w, h);
		}

		octx->accDt2+=1.0/fps;
		BGBBTJ_AVI_EncodeFrame2(octx, obuf, w, h, qf|qfl, 0);

//		sprintf(tb, "%s_%d.png", base, ctx->frnum-1);
//		BGBBTJ_PNG_SaveRaw(tb, fdbuf, w, h);
#endif
	}

	printf("\n");
	printf("Encode End\n");

	BGBBTJ_AVI_CloseOutStream(octx);

	printf("Stream Closed\n");

	return(0);
}
