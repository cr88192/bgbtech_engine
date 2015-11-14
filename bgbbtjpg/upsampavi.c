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
#define FCC_bt1d	FOURCC('b', 't', '1', 'd')

#define FCC_THEO	FOURCC('T', 'H', 'E', 'O')
#define FCC_theo	FOURCC('t', 'h', 'e', 'o')

byte *animavi_framergba;

int help(int argc, char *argv[])
{
}

struct dec_thread_info_s {
char *inavi;
volatile int cnt;
int dtclrs;
};

int dec_thread(void *ptr)
{
	struct dec_thread_info_s *deci;

	byte *buf, *lbuf, *tbuf, *tbuf1;
	byte *nvbuf, *spbuf, *lubuf;
	byte *obuf, *onvbuf, *ospbuf, *olubuf;

//	WalInfo_t *wal;
	BGBBTJ_AVI_Context *ictx;
	BGBBTJ_AVICtx *octx;
	BGBBTJ_Video_Stats *vstat;
	byte *base, *chain, *s;
	byte *fdbuf;
	char *inavi, *inavi2, *outavi;
	FILE *fd;
	int cy, cu, cv, cr, cg, cb;
	int t0, t1, t2, t3, flags;
	int frszavg, frszlim, frszcnt;
	int frboffs, frbnum;
	int i, j, k, w, h, w1, h1, fcc, qf, qff, qfl, maxfr, cmd, auid, tgtkib;
	double f, g;

	deci=ptr;

	ictx=BGBBTJ_AVI_LoadAVI(deci->inavi);
	w=ictx->bmihead->biWidth;
	h=ictx->bmihead->biHeight;

#if 1
//		printf("Threaded Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
//			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;

			j=0;

//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_BC1);
//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_RGBA);
//			fdbuf=BGBBTJ_AVI_DecodeFrame(ictx);
//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_BC3);
			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, deci->dtclrs);
			ictx->esampbuf=ictx->sampbuf;

			k++;
			deci->cnt++;

//			printf("Decoded %d times, ret=%d, dt=%.4fs "
//				"rate=%.4f/s  %.4fMp/s  \r", k, j,
//				t2/(float)CLOCKS_PER_SEC,
//				k/(t2/(float)CLOCKS_PER_SEC),
//				((w*h)/1000000.0)*(k/(t2/(double)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
//		printf("Decoded %d times\n", k);
#endif

	return(0);
}

int main(int argc, char *argv[])
{
	byte tb[1024];
	struct dec_thread_info_s deci;

	byte *buf, *lbuf, *tbuf, *tbuf1;
	byte *nvbuf, *spbuf, *lubuf;
	byte *obuf, *onvbuf, *ospbuf, *olubuf;

//	WalInfo_t *wal;
	BGBBTJ_AVI_Context *ictx;
	BGBBTJ_AVI_Context *ictx2;
	BGBBTJ_AVICtx *octx;
	BGBBTJ_Video_Stats *vstat;
	byte *base, *chain, *s;
	byte *fdbuf;
	byte *fdbuf2;
	char *inavi, *inavi2, *outavi;
	FILE *fd;
	int cy, cu, cv, cr, cg, cb, ca;
	int mdr, mdg, mdb, dysc, dtclrs;
	int t0, t1, t2, t3, flags;
	int frszavg, frszlim, frszcnt;
	int frboffs, frbnum;
	int i, j, k, w, h, w1, h1, fcc, qf, qff, qfl, maxfr, cmd, auid, tgtkib;
	double f, g;

	fcc=FCC_MJPG;
//	fcc=FCC_rpza;
//	fcc=FCC_theo;
//	fcc=FCC_THEO;
	flags=0;
	qf=80; qff=0;
	qfl=0;
	maxfr=0;
	cmd=0;
	auid=BGBBTJ_AVI_AUID_22K8BM;
	tgtkib=950000;
	dtclrs=BGBBTJ_JPG_RGBA;

#if 1
	base=NULL; inavi=NULL; inavi2=NULL; outavi=NULL;
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

			if(!strcmp(argv[i], "-mbt1c"))
				fcc=FCC_bt1c;
			if(!strcmp(argv[i], "-mbt1d"))
				fcc=FCC_bt1d;
			if(!strcmp(argv[i], "-mbt2c"))
				fcc=BGBBTJ_FCC_bt2c;

			if(!strcmp(argv[i], "-anone"))
				{ auid=BGBBTJ_AVI_AUID_NONE; continue; }

			if(!strcmp(argv[i], "-scale2x"))
				flags|=1;

			if(!strcmp(argv[i], "-bluescreen"))
				flags|=2;

			if(!strcmp(argv[i], "-greenscreen"))
				flags|=4;
				
			if(!strcmp(argv[i], "-q"))
			{
				qf=atoi(argv[i+1]);
				qff=qf;
				i++; continue;
			}

			if(!strcmp(argv[i], "-qblkzi"))
				{ qfl|=BGBBTJ_BTIC1C_QFL_BLOCKZI; }
			if(!strcmp(argv[i], "-qdiffclr"))
				{ qfl|=BGBBTJ_BTIC1C_QFL_DIFFCLR; }
			if(!strcmp(argv[i], "-qblkidx"))
				{ qfl|=BGBBTJ_QFL_BLOCKINDEX; }
			if(!strcmp(argv[i], "-qblkmotion"))
				{ qfl|=BGBBTJ_QFL_BLOCKMOTION; }
			if(!strcmp(argv[i], "-qextmodes"))
				{ qfl|=BGBBTJ_BTIC1C_QFL_EXTMODES; }
			if(!strcmp(argv[i], "-qindexclr"))
				{ qfl|=BGBBTJ_BTIC1C_QFL_INDEXCLR; }

			if(!strcmp(argv[i], "-maxframe"))
			{
				maxfr=atoi(argv[i+1]);
				i++; continue;
			}

			if(!strcmp(argv[i], "-tgtkib"))
			{
				tgtkib=atoi(argv[i+1]);
				i++; continue;
			}

			if(!strcmp(argv[i], "-dectst"))
				{ cmd=1; continue; }

			if(!strcmp(argv[i], "-thdectst"))
				{ cmd=2; continue; }

			if(!strcmp(argv[i], "-compare"))
				{ cmd=3; continue; }

			if(!strcmp(argv[i], "-dtcdxt1"))
				{ dtclrs=BGBBTJ_JPG_DXT1; continue; }
			if(!strcmp(argv[i], "-dtcdxt5"))
				{ dtclrs=BGBBTJ_JPG_DXT5; continue; }

			if(!strcmp(argv[i], "-dtcbc6h"))
				{ dtclrs=BGBBTJ_JPG_BC6H; continue; }
			if(!strcmp(argv[i], "-dtcbc6h_s"))
				{ dtclrs=BGBBTJ_JPG_BC6H_SF16; continue; }
			if(!strcmp(argv[i], "-dtcbc6h_u"))
				{ dtclrs=BGBBTJ_JPG_BC6H_UF16; continue; }
			if(!strcmp(argv[i], "-dtcbc7"))
				{ dtclrs=BGBBTJ_JPG_BC7; continue; }

			if(!strcmp(argv[i], "-dtcrgb"))
				{ dtclrs=BGBBTJ_JPG_RGB; continue; }
			if(!strcmp(argv[i], "-dtcrgba"))
				{ dtclrs=BGBBTJ_JPG_RGBA; continue; }
			if(!strcmp(argv[i], "-dtcbgr"))
				{ dtclrs=BGBBTJ_JPG_BGR; continue; }
			if(!strcmp(argv[i], "-dtcbgra"))
				{ dtclrs=BGBBTJ_JPG_BGRA; continue; }
		}else
		{
			if(!inavi)
				{ inavi=argv[i]; continue; }
			if((cmd==3) && !inavi2)
				{ inavi2=argv[i]; continue; }
			if(!outavi)
				{ outavi=argv[i]; continue; }
		}
	}
#endif

	if(cmd==1)
	{
		if(!inavi)
		{
			help(argc, argv);
			return(-1);
		}

		ictx=BGBBTJ_AVI_LoadAVI(inavi);
		w=ictx->bmihead->biWidth;
		h=ictx->bmihead->biHeight;

#if 1
		printf("Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
//			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;

			j=0;

//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_BC1);
//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_RGBA);
//			fdbuf=BGBBTJ_AVI_DecodeFrame(ictx);
//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_BC3);
			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, dtclrs);
			ictx->esampbuf=ictx->sampbuf;

			k++;

			printf("Decoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s  %.4fMp/s  \r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
//				((1.0/1000000)*(w*h))*(k/(t2/(float)CLOCKS_PER_SEC)));
				((w*h)/1000000.0)*(k/(t2/(double)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("Decoded %d times\n", k);
#endif

		return(0);
	}

#ifndef BGBBTJ_NODY
	if(cmd==2)
	{
		if(!inavi)
		{
			help(argc, argv);
			return(-1);
		}

		ictx=BGBBTJ_AVI_LoadAVI(inavi);
		w=ictx->bmihead->biWidth;
		h=ictx->bmihead->biHeight;

		deci.cnt=0;
		deci.inavi=inavi;
		deci.dtclrs=dtclrs;
		
		thThread(dec_thread, &deci);
		thThread(dec_thread, &deci);
		thThread(dec_thread, &deci);
		thThread(dec_thread, &deci);

#if 1
		printf("Threaded Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
//			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;

			j=0;

//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_BC1);
//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_RGBA);
//			fdbuf=BGBBTJ_AVI_DecodeFrame(ictx);
//			fdbuf=BGBBTJ_AVI_DecodeFrame2(ictx, 0, BGBBTJ_JPG_BC3);
//			ictx->esampbuf=ictx->sampbuf;
			thSleep(100);
			k=deci.cnt;

			printf("Decoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s  %.4fMp/s  \r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
//				((1.0/1000000)*(w*h))*(k/(t2/(float)CLOCKS_PER_SEC)));
				((w*h)/1000000.0)*(k/(t2/(double)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("Decoded %d times\n", k);
#endif

		return(0);
	}
#endif

#if 1
	if(cmd==3)
	{
		qfl|=BGBBTJ_QFL_BATCHENCODE;

		if(!inavi || !inavi2 || !outavi)
		{
			help(argc, argv);
			return(-1);
		}

//		printf("Base=%s\n", base);

//		sprintf(tb, "%s.avi", base);
		ictx=BGBBTJ_AVI_LoadAVI(inavi);
		ictx2=BGBBTJ_AVI_LoadAVI(inavi2);
		vstat=BGBBTJ_AVI_GetStats(ictx);

		w=ictx->bmihead->biWidth;
		h=ictx->bmihead->biHeight;

		w1=w; h1=h;
		w1=w*2; h1=h*2;

		octx=BGBBTJ_AVI_OpenOutStream2(outavi, w1, h1,
			1.0/vstat->frametime, fcc, auid);

		obuf=malloc((w1)*(h1)*4);

		frszlim=(tgtkib*1024)/vstat->num_frames;
//		frszlim=950000000/vstat->num_frames;
//		frszlim=1950000000/vstat->num_frames;
		frszcnt=4;
		frboffs=0;
		frbnum=0;

		cy=1;
		mdr=130;
		mdg=130;
		mdb=130;
		dysc=1;

		printf("\n");
		t0=clock();
		while(ictx->cindex>=0)
		{
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
	
			//attempt to adjust frame-quality to maintain average bitrate
			if(!qff)
			{
				if(frszavg>(frszlim*1.1))
				{
					if(frszcnt<=0)
					{
						if(frszavg>(frszlim*2.0))
							{ qf-=5; }
						else if(frszavg>(frszlim*1.5))
							{ qf-=3; }
						else
							{ qf-=1; }
						frszcnt=16;
						if(qf<10)qf=10;

						frboffs=vftell(octx->fd);
						frbnum=ictx->frnum;
					}else
					{
						frszcnt--;
					}
				}

				if(frszavg<(frszlim*0.9))
				{
					if(frszcnt<=0)
					{
						qf+=1;
						frszcnt=16;
						if(qf>95)qf=95;

						frboffs=vftell(octx->fd);
						frbnum=ictx->frnum;
					}else
					{
						frszcnt--;
					}
				}
			}
	
			fdbuf=BGBBTJ_AVI_DecodeFrame(ictx);
			fdbuf2=BGBBTJ_AVI_DecodeFrame(ictx2);

			if(auid)
			{
				BGBBTJ_AVI_WriteContextStereoSamples(octx,
					ictx->sampbuf, (ictx->esampbuf-ictx->sampbuf)/2);
			}
			ictx->esampbuf=ictx->sampbuf;

//			cy=(mdr+2*mdg+mdb)/4;
//			dysc=96/(cy-128);

			f=0;
			for(i=0; i<h; i++)
				for(j=0; j<w; j++)
			{
				obuf[(i*w1+j)*4+0]=fdbuf[(i*w+j)*4+0];
				obuf[(i*w1+j)*4+1]=fdbuf[(i*w+j)*4+1];
				obuf[(i*w1+j)*4+2]=fdbuf[(i*w+j)*4+2];
				obuf[(i*w1+j)*4+3]=fdbuf[(i*w+j)*4+3];

				obuf[((i+h)*w1+j)*4+0]=fdbuf2[(i*w+j)*4+0];
				obuf[((i+h)*w1+j)*4+1]=fdbuf2[(i*w+j)*4+1];
				obuf[((i+h)*w1+j)*4+2]=fdbuf2[(i*w+j)*4+2];
				obuf[((i+h)*w1+j)*4+3]=fdbuf2[(i*w+j)*4+3];

				cr=(fdbuf[(i*w+j)*4+0]-fdbuf2[(i*w+j)*4+0])+128;
				cg=(fdbuf[(i*w+j)*4+1]-fdbuf2[(i*w+j)*4+1])+128;
				cb=(fdbuf[(i*w+j)*4+2]-fdbuf2[(i*w+j)*4+2])+128;
				ca=(fdbuf[(i*w+j)*4+3]-fdbuf2[(i*w+j)*4+3])+128;
				
				cr=(cr<0)?0:((cr>255)?255:cr);
				cg=(cg<0)?0:((cg>255)?255:cg);
				cb=(cb<0)?0:((cb>255)?255:cb);
				ca=(ca<0)?0:((ca>255)?255:ca);
								
				obuf[(i*w1+j+w)*4+0]=cr;
				obuf[(i*w1+j+w)*4+1]=cg;
				obuf[(i*w1+j+w)*4+2]=cb;
				obuf[(i*w1+j+w)*4+3]=ca;

				cy=(cr+2*cg+cb)/4;
				f+=(cy-128)*(cy-128);

//				if(cr>mdr)mdr=cr;
//				if(cg>mdg)mdg=cg;
//				if(cb>mdb)mdb=cb;
				
				cr=(cr-128)*dysc+128;
				cg=(cg-128)*dysc+128;
				cb=(cb-128)*dysc+128;

				cr=(cr<0)?0:((cr>255)?255:cr);
				cg=(cg<0)?0:((cg>255)?255:cg);
				cb=(cb<0)?0:((cb>255)?255:cb);
				ca=(ca<0)?0:((ca>255)?255:ca);
								
				obuf[((i+h)*w1+j+w)*4+0]=cr;
				obuf[((i+h)*w1+j+w)*4+1]=cg;
				obuf[((i+h)*w1+j+w)*4+2]=cb;
				obuf[((i+h)*w1+j+w)*4+3]=ca;
			}

			f=sqrt(f/(w*h));
			if(f<1)f=1;
//			dysc=96/f;
			dysc=64/f;

//			memcpy(obuf, fdbuf, w1*h1*4);

			octx->accDt2+=1.0/octx->fps;
//			octx->accDt2=1.0/octx->fps;
			BGBBTJ_AVI_EncodeFrame2(octx, obuf, w1, h1, qf|qfl, 0);
		}

		printf("\n");
		printf("Encode End\n");

		BGBBTJ_AVI_CloseOutStream(octx);

		printf("Stream Closed\n");
		
		return(0);
	}
#endif

	qfl|=BGBBTJ_QFL_BATCHENCODE;

	if(!inavi || !outavi)
	{
		help(argc, argv);
		return(-1);
	}

//	printf("Base=%s\n", base);

//	sprintf(tb, "%s.avi", base);
	ictx=BGBBTJ_AVI_LoadAVI(inavi);
	vstat=BGBBTJ_AVI_GetStats(ictx);

	w=ictx->bmihead->biWidth;
	h=ictx->bmihead->biHeight;

	w1=w; h1=h;
	if(flags&1)
	{
		w1=w*2; h1=h*2;
	}

	octx=BGBBTJ_AVI_OpenOutStream2(outavi, w1, h1,
		1.0/vstat->frametime, fcc, auid);

	obuf=malloc((w1)*(h1)*4);

	frszlim=(tgtkib*1024)/vstat->num_frames;
//	frszlim=950000000/vstat->num_frames;
//	frszlim=1950000000/vstat->num_frames;
	frszcnt=4;
	frboffs=0;
	frbnum=0;

#if 0
	for(i=0; i<15; i++)
	{
		j=44100.0/octx->fps+0.5;
		BGBBTJ_AVI_WriteContextStereoSamples(octx,
			ictx->sampbuf, j);
		ictx->esampbuf=ictx->sampbuf;
	}
#endif

	printf("\n");
	t0=clock();
	while(ictx->cindex>=0)
	{
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
	
		//attempt to adjust frame-quality to maintain average bitrate
		if(!qff)
		{
			if(frszavg>(frszlim*1.1))
			{
				if(frszcnt<=0)
				{
					if(frszavg>(frszlim*2.0))
						{ qf-=5; }
					else if(frszavg>(frszlim*1.5))
						{ qf-=3; }
					else
						{ qf-=1; }
					frszcnt=16;
					if(qf<10)qf=10;

					frboffs=vftell(octx->fd);
					frbnum=ictx->frnum;
				}else
				{
					frszcnt--;
				}
			}

			if(frszavg<(frszlim*0.9))
			{
				if(frszcnt<=0)
				{
					qf+=1;
					frszcnt=16;
					if(qf>95)qf=95;

					frboffs=vftell(octx->fd);
					frbnum=ictx->frnum;
				}else
				{
					frszcnt--;
				}
			}
		}
	
		fdbuf=BGBBTJ_AVI_DecodeFrame(ictx);

//		j=44100/octx->fps;
//		BGBBTJ_AVI_WriteContextStereoSamples(octx,
//			ictx->sampbuf, j);
//		ictx->esampbuf=ictx->sampbuf;

		if(auid)
		{
			BGBBTJ_AVI_WriteContextStereoSamples(octx,
				ictx->sampbuf, (ictx->esampbuf-ictx->sampbuf)/2);
		}
		ictx->esampbuf=ictx->sampbuf;

		if(flags&1)
		{
			BGBBTJ_Tex_Resample(fdbuf, w, h, obuf, w1, h1);
//			BGBBTJ_Tex_ResampleSpline(fdbuf, w, h, obuf, w1, h1);
//			BGBBTJ_Tex_DoubleSample(fdbuf, obuf, w, h);
		}else
		{
			memcpy(obuf, fdbuf, w1*h1*4);
		}

		if(flags&2)
		{
			BGBBTJ_Tex_BlueScreen(obuf, obuf, w1, h1);
		}

		if(flags&4)
		{
			BGBBTJ_Tex_GreenScreen(obuf, obuf, w1, h1);
		}

		octx->accDt2+=1.0/octx->fps;
//		octx->accDt2=1.0/octx->fps;
		BGBBTJ_AVI_EncodeFrame2(octx, obuf, w1, h1, qf|qfl, 0);

//		BGBBTJ_AVI_WriteContextStereoSamples(octx,
//			ictx->sampbuf, (ictx->esampbuf-ictx->sampbuf)/2);
//		ictx->esampbuf=ictx->sampbuf;

//		sprintf(tb, "%s_%d.png", base, ctx->frnum-1);
//		BGBBTJ_PNG_SaveRaw(tb, fdbuf, w, h);
	}

	printf("\n");
	printf("Encode End\n");

	BGBBTJ_AVI_CloseOutStream(octx);

	printf("Stream Closed\n");

#if 0
	octx=BGBBTJ_AVI_OpenOutStream(outavi, w*2, h*2,
		1.0/vstat->frametime, fcc);
#endif

#if 0
	printf("Decode Test\n");
	t0=clock(); k=0;
	while(1)
	{
		t1=clock();
		t2=t1-t0;
		if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
//		if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
			break;

		j=0;

		fdbuf=BGBBTJ_AVI_DecodeFrame2(ctx, 0, BGBBTJ_JPG_BC1);

		k++;

		printf("Decoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s  %.4fMp/s\r", k, j,
			t2/(float)CLOCKS_PER_SEC,
			k/(t2/(float)CLOCKS_PER_SEC),
			(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
		if(j<0)break;
	}
	printf("Decoded %d times\n", k);
#endif

	return(0);
}
