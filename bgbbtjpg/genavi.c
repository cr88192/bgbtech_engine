#include <bgbbtj.h>

#include <time.h>

#ifndef FOURCC
#define FOURCC(a, b, c, d) ((a)|((b)<<8)|((c)<<16)|((d)<<24))
#endif

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

#define FCC_rpza	FOURCC('r', 'p', 'z', 'a')
#define FCC_bt1c	FOURCC('b', 't', '1', 'c')

#define FCC_THEO	FOURCC('T', 'H', 'E', 'O')
#define FCC_theo	FOURCC('t', 'h', 'e', 'o')

#define BTJ_FL_USEIPFRAME	256

int main(int argc, char *argv[])
{
	byte tb[1024];
	float exptab[256];
	float scltab[256];
	byte *buf, *lbuf, *tbuf, *tbuf1;
	byte *nvbuf, *spbuf, *lubuf;
	byte *obuf, *onvbuf, *ospbuf, *olubuf;
	byte *obuf1;
	void *ptr0, *ptr1;

//	WalInfo_t *wal;
	BGBBTJ_JPG_Context *jpctx;
	BGBBTJ_BTIC1_Context *btctx;
	BGBBTJ_AVICtx *ctx;
	byte *base, *chain, *s;
	FILE *fd;
	int cy, cu, cv, cr, cg, cb;
	int t0, t1, t2, flags, fps;
	int i, j, k, w, h, w1, h1, fcc, qf, qfl, qf2;
	float f, g;

	fcc=FCC_MJPG;
	flags=0; fps=10;
	qf=90; qfl=0;

#if 1
	base=NULL;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-btv"))
				fcc=FCC_BtV0;

			if(!strcmp(argv[i], "-mpng"))
				fcc=FCC_png1;
			if(!strcmp(argv[i], "-mbtic"))
				fcc=FCC_MBTC;
			if(!strcmp(argv[i], "-mbtj"))
				fcc=FCC_MBTJ;

			if(!strcmp(argv[i], "-mrpza"))
				fcc=FCC_rpza;
			if(!strcmp(argv[i], "-mbt1c"))
				fcc=FCC_bt1c;
			if(!strcmp(argv[i], "-mtheora"))
				fcc=FCC_THEO;
//				fcc=FCC_theo;

			if(!strcmp(argv[i], "-mrgba"))
				fcc=FCC_RGBA;
			if(!strcmp(argv[i], "-mrgbt"))
				fcc=FCC_RGBT;

			if(!strcmp(argv[i], "-dectst"))
				flags|=1;
			if(!strcmp(argv[i], "-dxttst"))
				flags|=2;
			if(!strcmp(argv[i], "-btictst"))
				flags|=4;
			if(!strcmp(argv[i], "-jfetst"))
				flags|=8;
			if(!strcmp(argv[i], "-pngftst"))
				flags|=16;
			if(!strcmp(argv[i], "-pngdtst"))
				flags|=32;
			if(!strcmp(argv[i], "-rpzatst"))
				flags|=64;

			if(!strcmp(argv[i], "-ipframe"))
				flags|=BTJ_FL_USEIPFRAME;

			if(!strcmp(argv[i], "-rdct"))
				qfl|=BGBBTJ_QFL_RDCT;
			if(!strcmp(argv[i], "-s444"))
				qfl|=BGBBTJ_QFL_444;
			if(!strcmp(argv[i], "-crct"))
				qfl|=BGBBTJ_QFL_ORCT;
			if(!strcmp(argv[i], "-crgb"))
				qfl|=BGBBTJ_QFL_RGB;
			if(!strcmp(argv[i], "-s410"))
				qfl|=BGBBTJ_QFL_410;

			if(!strcmp(argv[i], "-bitalpha"))
				qfl|=BGBBTJ_QFL_BITALPHA;
				
			if(!strcmp(argv[i], "-q"))
			{
				qf=atoi(argv[i+1]);
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

			if(!strcmp(argv[i], "-fps"))
			{
				fps=atoi(argv[i+1]);
				i++; continue;
			}

		}else
		{
			if(!base)
				base=argv[i];
		}
	}

	strcpy(tb, base);
	base=tb+strlen((char *)tb);
	while(base>tb)
	{
		if(*base=='.')
			{ *base=0; break; }
		base--;
	}
	base=strdup(tb);
#endif

	qf|=qfl;

	printf("Base=%s\n", base);

#if 1
	//base image, used for size
	sprintf(tb, "%s.png", base);
	buf=BGBBTJ_PNG_LoadRaw(tb, &w, &h);

	if(!buf)
	{
		printf("Failed Load %s\n", tb);
		return(-1);
	}

#if 1
	nvbuf=malloc(w*h*4);
	spbuf=malloc(w*h*4);
	lubuf=malloc(w*h*4);
	memset(nvbuf, 128, w*h*4);
	memset(spbuf, 128, w*h*4);
	memset(lubuf, 128, w*h*4);

	if(AWAL_LoadNormalMap(base, -1, nvbuf, w, h)<=0)
		{ free(nvbuf); nvbuf=NULL; }
	if(AWAL_LoadSpecularMap(base, -1, spbuf, w, h)<=0)
		{ free(spbuf); spbuf=NULL; }
	if(AWAL_LoadLumaMap(base, -1, lubuf, w, h)<=0)
		{ free(lubuf); lubuf=NULL; }

//	AWAL_LoadSpecularMap(base, i, nvbuf, w, h);
#endif

	printf("Img: %dx%d\n", w, h);

	tbuf=malloc(1<<24);
	i=BGBBTJ_JPG_EncodeComponent(
		buf, nvbuf, spbuf, lubuf,
		tbuf, w, h, qf); //90

	sprintf(tb, "%s_dump.jpg", base);
	fd=fopen(tb, "wb");
	if(fd)
	{
		fwrite(tbuf, 1, i, fd);
		fclose(fd);
	}

#if 1
	if(flags&1)
	{
		obuf=malloc(w*h*4);
		onvbuf=malloc(w*h*4);
		ospbuf=malloc(w*h*4);
		olubuf=malloc(w*h*4);
		memcpy(obuf, buf, w*h*4);
		if(nvbuf)memcpy(onvbuf, nvbuf, w*h*4);
		if(spbuf)memcpy(ospbuf, spbuf, w*h*4);
		if(lubuf)memcpy(olubuf, lubuf, w*h*4);

		memset(buf, 0, w*h*4);
		if(nvbuf)memset(nvbuf, 0, w*h*4);
		if(spbuf)memset(spbuf, 0, w*h*4);
		if(lubuf)memset(lubuf, 0, w*h*4);

		jpctx=BGBBTJ_JPG_AllocContext();

		printf("Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;

			j=0;
//			BGBBTJ_JPG_DecodeCtxClrs(jpctx, tbuf, i,
//				BGBBTJ_JPG_BC1, &w1, &h1);
	
			j=BGBBTJ_JPG_DecodeComponent(tbuf, i,
				buf, nvbuf, spbuf, lubuf,
				&w1, &h1);

//			j=BGBBTJ_JPG_DecodeComponentClrs(tbuf, i,
//				BGBBTJ_JPG_BC3,
//				buf, nvbuf, spbuf, lubuf,
//				&w1, &h1);

			k++;

			printf("Decoded %d times, ret=%d, dt=%.4fs "
					"rate=%.4f/s  %.4fMp/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("Decoded %d times\n", k);

		i=!memcmp(buf, obuf, w*h*4);
		printf("Lossless RGBA %s\n", i?"Yes":"No");
		if(!i)imgstat(buf, obuf, w*h);

		if(nvbuf)
		{
			i=!memcmp(nvbuf, onvbuf, w*h*4);
			printf("Lossless Norm %s\n", i?"Yes":"No");
			if(!i)imgstat(nvbuf, onvbuf, w*h);
		}

		if(spbuf)
		{
			i=!memcmp(spbuf, ospbuf, w*h*4);
			printf("Lossless Specular %s\n", i?"Yes":"No");
			if(!i)imgstat(spbuf, ospbuf, w*h);
		}

		if(lubuf)
		{
			i=!memcmp(lubuf, olubuf, w*h*4);
			printf("Lossless Luma %s\n", i?"Yes":"No");
			if(!i)imgstat(lubuf, olubuf, w*h);
		}

#if 1
		i=BGBBTJ_JPG_EncodeComponent(
			buf, nvbuf, spbuf, lubuf,
			tbuf, w, h, qf); //90

		sprintf(tb, "%s_dump2.jpg", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif
	}
#endif

#if 1
	if(flags&2)
	{
		obuf=malloc(w*h*4);
		obuf1=malloc(w*h*4);

#if 1
		BGBBTJ_BCn_EncodeImageDXT5(obuf, buf, w, h, 4);
		BGBBTJ_BCn_DecodeImageDXT5(obuf, obuf1, w, h, 4);
		i=!memcmp(obuf1, buf, w*h*4);
		printf("Lossless DXT5 %s\n", i?"Yes":"No");
		if(!i)imgstat(obuf1, buf, w*h);
#endif

#if 1
		BGBBTJ_BCn_EncodeImageDXT5_UVAY(obuf, buf, w, h, 4);
		BGBBTJ_BCn_DecodeImageDXT5_UVAY(obuf, obuf1, w, h, 4);
		i=!memcmp(obuf1, buf, w*h*4);
		printf("Lossless DXT5 UVAY %s\n", i?"Yes":"No");
		if(!i)imgstat(obuf1, buf, w*h);
#endif

		printf("DXT Encode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;

//			BGBBTJ_BCn_EncodeImageDXT1F(obuf, buf, w, h, 4);
//			BGBBTJ_BCn_EncodeImageDXT1(obuf, buf, w, h, 4);
//			BGBBTJ_BCn_EncodeImageDXT5(obuf, buf, w, h, 4);
			BGBBTJ_BCn_EncodeImageDXT5_UVAY(obuf, buf, w, h, 4);

//			BGBBTJ_BCn_EncodeImageDXT1F_YUV(obuf, buf, w, h, 4);
	
//			j=BGBBTJ_JPG_DecodeComponent(tbuf, i,
//				buf, nvbuf, spbuf, lubuf,
//				&w1, &h1);
			k++;

			printf("Encoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s %.4fMp/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("\nEncoded %d times\n", k);

#if 1
		i=(w/4)*(h/4);
		j=BGBBTJ_PackBCn_EncodeBlocksDXT5(obuf1, obuf, i);

		memset(obuf, 0x55, i*16);
		memset(buf, 0x55, w*h*4);

		printf("Encoded Pack %d -> %d\n", i*16, j);

		printf("PackDXT Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;
			k++;

			j=BGBBTJ_PackBCn_DecodeBlocksDXT5(obuf1, obuf, i);

			printf("Decoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s %.4fMp/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("\nDecoded %d times\n", k);
#endif

//		BGBBTJ_BCn_DecodeImageDXT5(obuf, buf, w, h, 4);
		BGBBTJ_BCn_DecodeImageDXT5_UVAY(obuf, buf, w, h, 4);

#if 1
		i=BGBBTJ_JPG_EncodeComponent(
			buf, nvbuf, spbuf, lubuf,
			tbuf, w, h, 95); //90

		sprintf(tb, "%s_dump_dxt1.jpg", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif
	}
#endif

#if 1
	if(flags&4)
	{
		obuf=malloc(w*h*4);
		obuf1=malloc(w*h*4);

#if 1
		BGBBTJ_FiltLBR_FilterImage(obuf1, buf, w, h, qf);

		i=BTIC1_EncodeComponent(
			obuf1, nvbuf, spbuf, lubuf,
			tbuf, w, h, 95); //90
#endif

#if 0
		i=BTIC1_EncodeComponent(
			buf, nvbuf, spbuf, lubuf,
			tbuf, w, h, qf); //90
#endif

		sprintf(tb, "%s_dump.btic", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}


		j=BTIC1_DecodeComponent(tbuf, i,
			obuf, nvbuf, spbuf, lubuf,
			&w1, &h1, BGBBTJ_JPG_RGBA);

		i=!memcmp(obuf, buf, w*h*4);
		printf("Lossless BTIC1 DXT5 %s\n", i?"Yes":"No");
		if(!i)imgstat(obuf, buf, w*h);

#if 1
		memset(obuf, 0x55, i*16);
		memset(buf, 0x55, w*h*4);

		printf("BTIC1 Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;
			k++;

//			j=BTIC1_DecodeComponent(tbuf, i,
//				obuf, nvbuf, spbuf, lubuf,
//				&w1, &h1, BGBBTJ_JPG_RGBA);

			j=BTIC1_DecodeComponent(tbuf, i,
				obuf1, nvbuf, spbuf, lubuf,
				&w1, &h1, BGBBTJ_JPG_BC3);

			printf("Decoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s %.4fMp/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("\nDecoded %d times\n", k);
#endif

#if 1
//		i=BGBBTJ_JPG_EncodeComponent(
//			obuf, nvbuf, spbuf, lubuf,
//			tbuf, w, h, 95); //90

		i=BGBBTJ_BufPNG_Encode(tbuf, 1<<20, obuf, w, h);

//		sprintf(tb, "%s_dump_btic1.jpg", base);
		sprintf(tb, "%s_dump_btic1.png", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif

#if 0
		BGBBTJ_FiltLBR_FilterImage(obuf1, buf, w, h, qf);

		i=!memcmp(obuf1, buf, w*h*4);
		printf("Lossless LBR %s\n", i?"Yes":"No");
		if(!i)imgstat(obuf1, buf, w*h);

#if 1
		i=BGBBTJ_JPG_EncodeComponent(
			obuf1, NULL, NULL, NULL,
			tbuf, w, h, qf); //90

		sprintf(tb, "%s_dump_flbr1.jpg", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif
#endif

#if 0
		BGBBTJ_BCn_EncodeImageDXT5(obuf, buf, w, h, 4);
		BGBBTJ_BCn_DecodeImageDXT5(obuf, obuf1, w, h, 4);
		i=!memcmp(obuf1, buf, w*h*4);
		printf("Lossless DXT5 %s\n", i?"Yes":"No");
		if(!i)imgstat(obuf1, buf, w*h);
#endif

#if 1

		tbuf1=malloc(1<<24);

#if 1
		btctx=BTIC1_AllocContext();

		i=BTIC1_EncodeComponentCtx(btctx,
			obuf, nvbuf, spbuf, lubuf,
			tbuf, w, h, 95|BGBBTJ_QFL_IFRAME);

		j=BTIC1_EncodeComponentCtx(btctx,
			obuf, nvbuf, spbuf, lubuf,
			tbuf1, w, h, 95|BGBBTJ_QFL_PFRAME);
			
		BTIC1_FreeContext(btctx);
#endif

		printf("BTIC1 I-Frame Test, Sz=%d\n", i);
		printf("BTIC1 P-Frame Test, Sz=%d\n", j);

		sprintf(tb, "%s_dump_pf.btic", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf1, 1, j, fd);
			fclose(fd);
		}

		memset(obuf1, 0, w*h*4);

		btctx=BTIC1_AllocContext();

		BTIC1_DecodeComponentCtx(btctx, tbuf, i,
			obuf1, nvbuf, spbuf, lubuf,
			&w1, &h1, BGBBTJ_JPG_RGBA);

		memset(obuf1, 0, w*h*4);

		j=BTIC1_DecodeComponentCtx(btctx, tbuf1, j,
			obuf1, nvbuf, spbuf, lubuf,
			&w1, &h1, BGBBTJ_JPG_RGBA);

		i=BGBBTJ_BufPNG_Encode(tbuf, 1<<20, obuf1, w, h);
		sprintf(tb, "%s_dump_pf_btic1.png", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}

#endif

	}
#endif


#if 1
	if(flags&8)
	{
		obuf=malloc(w*h*4);
		obuf1=malloc(w*h*4);

		printf("JFE Encode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;
			k++;

			j=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf, BGBBTJ_JPG_RGBA);

			printf("Encoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s %.4fMp/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("\nEncoded %d times\n", k);

#if 1
//		i=BGBBTJ_JPG_EncodeComponent(
//			obuf, nvbuf, spbuf, lubuf,
//			tbuf, w, h, 95); //90

		i=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf, BGBBTJ_JPG_RGBA);

		sprintf(tb, "%s_dump_jhe1.jpg", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif

//		j=BGBBTJ_BitArith_EncodeTestData(tbuf, i, obuf, 1<<20);
		j=BGBBTJ_BitArith2_EncodeTestData(tbuf, i, obuf, 1<<20);
		printf("%d -> %d\n", i, j);
	}
#endif

#if 1
	if(flags&16)
	{
		obuf=malloc(w*h*4);
		obuf1=malloc(w*h*4);

		printf("PNG-Fast Encode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;
			k++;

//			j=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf);
			j=BGBBTJ_BufPNG_EncodeFast(tbuf, 1<<20, buf, w, h);

			printf("Encoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s %.4fMp/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("\nEncoded %d times\n", k);

#if 1
//		i=BGBBTJ_JPG_EncodeComponent(
//			obuf, nvbuf, spbuf, lubuf,
//			tbuf, w, h, 95); //90

//		i=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf);
		i=BGBBTJ_BufPNG_EncodeFast(tbuf, 1<<20, buf, w, h);

		sprintf(tb, "%s_dump_pngf.png", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif
	}
#endif

#if 1
	if(flags&32)
	{
		obuf=malloc(w*h*4);
//		obuf1=malloc(w*h*4);

//		i=BGBBTJ_BufPNG_EncodeFast(tbuf, 1<<20, buf, w, h);
		i=BGBBTJ_BufPNG_Encode(tbuf, 1<<20, buf, w, h);

		printf("PNG Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;
			k++;

//			j=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf);
//			j=BGBBTJ_BufPNG_EncodeFast(tbuf, 1<<20, buf, w, h);
//			obuf1=BGBBTJ_BufPNG_Decode(tbuf, j, &w2, &h2);
//			memcpy(obuf, obuf1, w2*h2*4);
			j=BGBBTJ_BufPNG_DecodeOBuf(tbuf, i, obuf, &w1, &h1);

			printf("Decoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s %.4fMp/s    \r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("\nEncoded %d times\n", k);

#if 1
//		i=BGBBTJ_JPG_EncodeComponent(
//			obuf, nvbuf, spbuf, lubuf,
//			tbuf, w, h, 95); //90

//		i=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf);
//		i=BGBBTJ_BufPNG_EncodeFast(tbuf, 1<<20, obuf, w, h);
		i=BGBBTJ_BufPNG_Encode(tbuf, 1<<20, obuf, w, h);

		sprintf(tb, "%s_dump_pngd.png", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif
	}
#endif

#if 1
	if(flags&64)
	{
		obuf=malloc(w*h*4);
		obuf1=malloc(w*h*4);

		ptr0=BGBBTJ_RPZA_AllocContext();

		printf("RPZA Encode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
//			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;
			k++;

//			j=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf);
//			j=BGBBTJ_BufPNG_EncodeFast(tbuf, 1<<20, buf, w, h);

			j=BGBBTJ_RPZA_EncodeImage(ctx, buf, tbuf, 1<<20, w, h, qf, 0);

			printf("Encoded %d times, ret=%d, dt=%.4fs "
				"rate=%.4f/s %.4fMp/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC),
				(1.0/1000000)*w*h*(k/(t2/(float)CLOCKS_PER_SEC)));
			if(j<0)break;
		}
		printf("\nEncoded %d times\n", k);

#if 0
//		i=BGBBTJ_JPG_EncodeComponent(
//			obuf, nvbuf, spbuf, lubuf,
//			tbuf, w, h, 95); //90

//		i=BTJ_JFE_EncodeFast(buf, tbuf, w, h, qf);
		i=BGBBTJ_BufPNG_EncodeFast(tbuf, 1<<20, buf, w, h);

		sprintf(tb, "%s_dump_rpaz.png", base);
		fd=fopen(tb, "wb");
		if(fd)
		{
			fwrite(tbuf, 1, i, fd);
			fclose(fd);
		}
#endif
	}
#endif


//	*(int *)-1=-1;

	sprintf(tb, "%s.avi", base);
//	ctx=BGBBTJ_AVI_OpenOutStream(tb, w, h, 10, fcc);
//	ctx=BGBBTJ_AVI_OpenOutStream(tb, w, h, fps, fcc);
	ctx=BGBBTJ_AVI_OpenOutStream2(tb, w, h, fps, fcc, 0);

//	free(buf);	//don't need

	lbuf=malloc(w*h*4);
	memset(lbuf, 0, w*h*4);
	tbuf=malloc(w*h*4);

	for(i=0; i<1024; i++)
	{
		sprintf(tb, "%s_%d.png", base, i);
		buf=BGBBTJ_PNG_LoadRaw(tb, &w1, &h1);
		if(!buf)break;
		
		printf("Got %s %dx%d\n", tb, w1, h1);

		k=0;
		if(!i)k=15;

		if(AWAL_LoadNormalMap(base, i, nvbuf, w, h)>0)k|=1;
		if(AWAL_LoadSpecularMap(base, i, spbuf, w, h)>0)k|=2;
		if(AWAL_LoadLumaMap(base, i, lubuf, w, h)>0)k|=4;

		if(k)printf("\t k=%d\n", k);

		ctx->accDt2=1.0/ctx->fps;
//		BGBBTJ_AVI_EncodeFrame(ctx, buf, w1, h1);
//		BGBBTJ_AVI_EncodeComponentFrame(ctx,
//			buf, (k&1)?nvbuf:NULL,
//			(k&2)?spbuf:NULL, (k&4)?lubuf:NULL,
//			w1, h1);

		qf2=qf;
		if(flags&BTJ_FL_USEIPFRAME)
		{
			if(!(i%16))
				{ qf2|=BGBBTJ_QFL_IFRAME; }
			else
				{ qf2|=BGBBTJ_QFL_PFRAME; }
		}

		BGBBTJ_AVI_EncodeComponentFrame2(ctx,
			buf, (k&1)?nvbuf:NULL,
			(k&2)?spbuf:NULL, (k&4)?lubuf:NULL,
			w1, h1, qf2, BGBBTJ_JPG_RGBA);
//		free(buf);
	}

	BGBBTJ_AVI_CloseOutStream(ctx);

//	sprintf(tb, "%s.wal", base);
//	AWAL_WriteFile(tb, wal);
#endif

	return(0);
}
