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

byte *animavi_framergba;

int main(int argc, char *argv[])
{
	byte tb[1024];
	byte *buf, *lbuf, *tbuf, *tbuf1;
	byte *nvbuf, *spbuf, *lubuf;
	byte *obuf, *onvbuf, *ospbuf, *olubuf;

//	WalInfo_t *wal;
	BGBBTJ_AVI_Context *ctx;
	byte *base, *chain, *s;
	byte *fdbuf;
	FILE *fd;
	int cy, cu, cv, cr, cg, cb;
	int t0, t1, t2, flags;
	int i, j, k, w, h, w1, h1, fcc, qf;
	float f, g;

	fcc=FCC_MJPG;
	flags=0;
	qf=90;

#if 1
	base=NULL;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
//			if(!strcmp(argv[i], "-btv"))
//				fcc=FCC_BtV0;

//			if(!strcmp(argv[i], "-dectst"))
//				flags|=1;
				
			if(!strcmp(argv[i], "-q"))
			{
				qf=atoi(argv[i+1]);
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

	printf("Base=%s\n", base);

	sprintf(tb, "%s.avi", base);
	ctx=BGBBTJ_AVI_LoadAVI(tb);

	w=ctx->bmihead->biWidth;
	h=ctx->bmihead->biHeight;

	while(ctx->cindex>=0)
	{
		fdbuf=BGBBTJ_AVI_DecodeFrame(ctx);

		sprintf(tb, "%s_%d.png", base, ctx->frnum-1);
		BGBBTJ_PNG_SaveRaw(tb, fdbuf, w, h);
	}

#if 1
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
