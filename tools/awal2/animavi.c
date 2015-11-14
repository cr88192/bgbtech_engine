#include <walpng.h>

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
	float exptab[256];
	float scltab[256];
	byte *buf, *lbuf, *tbuf, *tbuf1;
	byte *nvbuf, *spbuf, *lubuf;
	byte *obuf, *onvbuf, *ospbuf, *olubuf;

//	WalInfo_t *wal;
	PDGL_AVICtx *ctx;
	byte *base, *chain, *s;
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
			if(!strcmp(argv[i], "-btv"))
				fcc=FCC_BtV0;

			if(!strcmp(argv[i], "-dectst"))
				flags|=1;
				
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

#if 1
	//base image, used for size
	sprintf(tb, "%s.png", base);
	buf=PDPNG_LoadRaw(tb, &w, &h);

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
#endif

#if 0
	if(AWAL_LoadNormalMap(base, -1, nvbuf, w, h)<=0)
		{ free(nvbuf); nvbuf=NULL; }
	if(AWAL_LoadSpecularMap(base, -1, spbuf, w, h)<=0)
		{ free(spbuf); spbuf=NULL; }
	if(AWAL_LoadLumaMap(base, -1, lubuf, w, h)<=0)
		{ free(lubuf); lubuf=NULL; }

//	AWAL_LoadSpecularMap(base, i, nvbuf, w, h);
#endif

	printf("Img: %dx%d\n", w, h);

#if 0
	tbuf=malloc(1<<22);
	i=PDJPG_EncodeComponent(
		buf, nvbuf, spbuf, lubuf,
		tbuf, w, h, qf); //90

	sprintf(tb, "%s_dump.jpg", base);
	fd=fopen(tb, "wb");
	if(fd)
	{
		fwrite(tbuf, 1, i, fd);
		fclose(fd);
	}
#endif

	sprintf(tb, "%s.avi", base);
	ctx=PDGL_AVI_OpenOutStream(tb, w, h, 10, fcc);

	free(buf);	//don't need


	animavi_framergba=malloc(w*h*4);

	lbuf=malloc(w*h*4);
	memset(lbuf, 0, w*h*4);
	tbuf=malloc(w*h*4);

	for(i=0; i<1024; i++)
	{
		sprintf(tb, "%s_%d.png", base, i);
		buf=PDPNG_LoadRaw(tb, &w1, &h1);
		if(buf)
		{
			memcpy(animavi_framergba, buf, w*h*4);
//			break;
		}
		
//		memset(animavi_framergba, 128, w*h*4);
		
		if(!buf)break;
		
		printf("Got %s %dx%d\n", tb, w1, h1);

		k=0;
		if(!i)k=15;

//		if(AWAL_LoadNormalMap(base, i, nvbuf, w, h)>0)k|=1;
//		if(AWAL_LoadSpecularMap(base, i, spbuf, w, h)>0)k|=2;
//		if(AWAL_LoadLumaMap(base, i, lubuf, w, h)>0)k|=4;

		if(k)printf("\t k=%d\n", k);

		ctx->accDt2=1.0/ctx->fps;
		PDGL_AVI_EncodeFrame(ctx, animavi_framergba, w, h);
//		PDGL_AVI_EncodeComponentFrame(ctx,
//			buf, (k&1)?nvbuf:NULL,
//			(k&2)?spbuf:NULL, (k&4)?lubuf:NULL,
//			w1, h1);
		free(buf);
	}

	PDGL_AVI_CloseOutStream(ctx);
#endif

	return(0);
}
