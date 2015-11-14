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


uint PDPNG_DataAdler32(void *buf, int sz, uint lcrc);
int PDPNG_Paeth(int a, int b, int c);
byte *PDPNG_LoadRaw(char *name, int *w, int *h);

void *bgbv86_ralloc(int sz)
{
	static byte *buf=NULL;
	static byte *bufe, *bufr;
	void *p;

	if(!buf)
	{
		buf=malloc(1<<18);
		bufe=buf+(1<<18);
		bufr=buf;
	}

	sz=(sz+15)&(~15);
	if((bufr+sz)>bufe)bufr=buf;
	p=bufr; bufr+=sz;
	memset(p, 0, sz);
	return(p);
}

char *bgbv86_rstrdup(char *str)
{
	char *t;
	t=bgbv86_ralloc(strlen(str)+1);
	strcpy(t, str);
	return(t);
}

char *bgbv86_rsprint(char *str, ...)
{
	char buf[256];
	va_list lst;
	char *t;

	va_start(lst, str);
	vsprintf(buf, str, lst);
	va_end(lst);

	t=bgbv86_rstrdup(buf);
	return(t);
}

char **bgbv86_rsplit(char *s)
{
	char **a, *t;
	int i;

	a=bgbv86_ralloc(64*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=bgbv86_ralloc(256);
		a[i++]=t;

		if(*s=='"')
		{
			s++;

			while(*s && (*s!='"'))
			{
				if(*s=='\\')
				{
					*t++=*s++;

					if(*s=='x')
					{
						*t++=*s++;
						*t++=*s++; *t++=*s++;
						continue;
					}
					if(*s=='u')
					{
						*t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						continue;
					}
					if(*s=='U')
					{
						*t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						*t++=*s++; *t++=*s++;
						continue;
					}

					*t++=*s++;
					continue;
				}

				*t++=*s++;
			}

			if(*s=='"')s++;

			*t++=0;
			*t++='\"';
			*t++=0;
			continue;
		}

		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
		*t++=0;
	}
	a[i]=NULL;

	return(a);
}

int WAL_ReadInfo(char *name, WalInfo_t *wal)
{
	char tb[1024];
	char **a;
	FILE *fd;
	int i;

//	wal->flags=0;
//	*cnts=0;

	fd=fopen(name, "rt");
	if(!fd)return(-1);

	while(!feof(fd))
	{
		fgets(tb, 1023, fd);
		a=bgbv86_rsplit(tb);

		if(!a)continue;
		if(!a[0])continue;
		if(*(a[0])!='$')continue;

		if(!strcmp(a[0], "$sky"))
		{
			wal->flags|=SURF_SKY;
		}

		if(!strcmp(a[0], "$flags"))
		{
			for(i=1; a[i]; i++)
			{
				if(!strcmp(a[i], "fluid"))
					wal->contents|=CONTENTS_WATER;

				if(!strcmp(a[i], "window"))
					wal->contents|=CONTENTS_WINDOW;
				if(!strcmp(a[i], "water"))
					wal->contents|=CONTENTS_WATER;
				if(!strcmp(a[i], "lava"))
					wal->contents|=CONTENTS_LAVA;
				if(!strcmp(a[i], "slime"))
					wal->contents|=CONTENTS_SLIME;
				if(!strcmp(a[i], "sewer"))
					wal->contents|=CONTENTS_WATER;

				if(!strcmp(a[i], "mist"))
					wal->contents|=CONTENTS_MIST;
				if(!strcmp(a[i], "origin"))
					wal->contents|=CONTENTS_ORIGIN;
				if(!strcmp(a[i], "detail"))
					wal->contents|=CONTENTS_DETAIL;
				if(!strcmp(a[i], "ladder"))
					wal->contents|=CONTENTS_LADDER;

				if(!strcmp(a[i], "slick"))
					wal->flags|=SURF_SLICK;
				if(!strcmp(a[i], "sky"))
					wal->flags|=SURF_SKY;
				if(!strcmp(a[i], "warpst"))
					wal->flags|=SURF_WARP;
				if(!strcmp(a[i], "warp"))
					wal->flags|=SURF_WARP;

				if(!strcmp(a[i], "alpha33"))
					wal->flags|=SURF_TRANS33;
				if(!strcmp(a[i], "alpha66"))
					wal->flags|=SURF_TRANS66;

				if(!strcmp(a[i], "flowing"))
					wal->flags|=SURF_FLOWING;
				if(!strcmp(a[i], "nodraw"))
					wal->flags|=SURF_NODRAW;

				if(!strcmp(a[i], "light"))
				{
					wal->flags|=SURF_LIGHT;
					wal->value=250;
				}

				if(!strcmp(a[i], "clip"))
				{
					wal->flags|=SURF_NODRAW;
					wal->contents|=CONTENTS_PLAYERCLIP;
					wal->contents|=CONTENTS_MONSTERCLIP;
				}
			}
		}
	}

	if(wal->contents&CONTENTS_WATER)
	{
		if(wal->contents&CONTENTS_WINDOW)
			wal->contents&=~CONTENTS_WINDOW;

		wal->flags|=SURF_WARP;

		if(wal->contents&CONTENTS_SLIME)
		{
			wal->contents&=~CONTENTS_WATER;
			wal->flags|=SURF_TRANS66;
		}
		if(wal->contents&CONTENTS_LAVA)
			wal->contents&=~CONTENTS_WATER;

		if((wal->contents&CONTENTS_WATER) &&
			!(wal->flags&SURF_TRANS33) &&
			!(wal->flags&SURF_TRANS66))
		{
			wal->flags|=SURF_TRANS33;
		}
	}

	printf("Info %dx%d\t%08X %08X\n",
		wal->width, wal->height,
		wal->flags, wal->contents);

	fclose(fd);
}

byte *PDTGA_LoadRaw(char *name, int *w, int *h);

int WAL_FileExistsP(char *name)
{
	FILE *fd;
	
	fd=fopen(name, "rb");
	if(!fd)return(0);
	fclose(fd);
	return(1);
}

int WAL_ImageExistsP(char *name)
{
	char tb[256];
	
//	printf("Check Image %s\n", name);
	
	sprintf(tb, "%s.png", name);
	if(WAL_FileExistsP(tb))
		return(1);
	sprintf(tb, "%s.jpg", name);
	if(WAL_FileExistsP(tb))
		return(1);

	return(0);
}

char *WAL_AnimChain(char *name)
{
	char pf[256], sf[256];
	char tb[256];
	char *s, *t, *s0;
	int i, n;

//	if(*name!='+')
//		return(NULL);

	n=0;

	s=name+strlen(name);
	while((s>name) && (*s!='+') && (*s!='/'))s--;
	if(*s!='+')return(NULL);

	s0=name; t=pf;
	while(*s0 && (s0<s))*t++=*s0++;
	*t++=0;
//	printf("PF=%s\n", pf);

	s0=s+1;
	n=atoi(s0);
	while(*s0 && (*s0>='0') && (*s0<='9'))s0++;

	t=sf;
	while(*s0)*t++=*s0++;
	*t++=0;
//	printf("SF=%s\n", sf);

//	s=name+strlen(name)-1;
//	while((s>name) && (*(s-1)>='0') && (*(s-1)<='9'))s--;
//	s0=name; t=tb;
//	while(*s0 && (s0<s))*t++=*s0++;
//	*t++=0;
//	n=atoi(s);
	
//	return(NULL);
	
	sprintf(tb, "%s+%d%s", pf, n+1, sf);
	if(WAL_ImageExistsP(tb))
		return(strdup(tb));

	sprintf(tb, "%s+%d%s", pf, 0, sf);
	if(WAL_ImageExistsP(tb))
		return(strdup(tb));

	return(NULL);
}

void AWAL_UnFilterBuf(byte *tbuf, byte *buf, byte *lbuf,
	int xs, int ys, int fl)
{
	int i, j, k, l, v;
	int a, b, c, d;

//	tb1=malloc(xs*ys*4);
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
			for(k=0; k<4; k++)
	{
		l=lbuf[(i*xs+j)*4+k];
//		l=buf[(i*xs+j)*4+k];

		v=buf[k*(xs*ys)+(i*xs)+j];

		if(fl&1)
//		if(0)
//		if(1)
		{
			a=(j>0)?tbuf[(i*xs+(j-1))*4+k]:0;
			b=(i>0)?tbuf[((i-1)*xs+j)*4+k]:0;
			c=((i>0)&&(j>0))?tbuf[((i-1)*xs+(j-1))*4+k]:0;
			d=PDPNG_Paeth(a, b, c);
		}else d=0;

//		tbuf[k*(xs*ys)+(i*xs)+j]=l-d;
		tbuf[(i*xs+j)*4+k]=l+d+v;
	}
}

void AWAL_FilterBuf(byte *tbuf, byte *buf, byte *lbuf,
	int xs, int ys, int fl)
{
//	byte *tb1;
	int i, j, k, l;
//	int lr, lg, lb, la;
//	int ar, br, cr, dr;
//	int ag, bg, cg, dg;
//	int ab, bb, cb, db;
//	int aa, ba, ca, da;
	int a, b, c, d;

#if 1
//	tb1=malloc(xs*ys*4);
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
			for(k=0; k<4; k++)
	{
		l=buf[(i*xs+j)*4+k]-lbuf[(i*xs+j)*4+k];
//		l=buf[(i*xs+j)*4+k];

		if(fl&1)
//		if(0)
//		if(1)
		{
			a=(j>0)?buf[(i*xs+(j-1))*4+k]:0;
			b=(i>0)?buf[((i-1)*xs+j)*4+k]:0;
			c=((i>0)&&(j>0))?buf[((i-1)*xs+(j-1))*4+k]:0;
			d=PDPNG_Paeth(a, b, c);
		}else d=0;

		tbuf[k*(xs*ys)+(i*xs)+j]=l-d;
//		tbuf[(i*xs+j)*4+k]=l-d;
	}
#endif

#if 0
//	tb1=malloc(xs*ys*4);
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		lr=buf[(i*xs+j)*4+0]-lbuf[(i*xs+j)*4+0];
		lg=buf[(i*xs+j)*4+1]-lbuf[(i*xs+j)*4+1];
		lb=buf[(i*xs+j)*4+2]-lbuf[(i*xs+j)*4+2];
		la=buf[(i*xs+j)*4+3]-lbuf[(i*xs+j)*4+3];
//		l=buf[(i*xs+j)*4+k];

//		lr-=lg;
//		lb-=lg;

		ar=(j>0)?buf[(0*(xs*ys))+(i*xs)+(j-1)]:0;
		ag=(j>0)?buf[(1*(xs*ys))+(i*xs)+(j-1)]:0;
		ab=(j>0)?buf[(2*(xs*ys))+(i*xs)+(j-1)]:0;
		aa=(j>0)?buf[(3*(xs*ys))+(i*xs)+(j-1)]:0;

		br=(i>0)?buf[(0*(xs*ys))+((i-1)*xs)+j]:0;
		bg=(i>0)?buf[(1*(xs*ys))+((i-1)*xs)+j]:0;
		bb=(i>0)?buf[(2*(xs*ys))+((i-1)*xs)+j]:0;
		ba=(i>0)?buf[(3*(xs*ys))+((i-1)*xs)+j]:0;

		cr=((i>0)&&(j>0))?buf[(0*(xs*ys))+((i-1)*xs)+(j-1)]:0;
		cg=((i>0)&&(j>0))?buf[(1*(xs*ys))+((i-1)*xs)+(j-1)]:0;
		cb=((i>0)&&(j>0))?buf[(2*(xs*ys))+((i-1)*xs)+(j-1)]:0;
		ca=((i>0)&&(j>0))?buf[(3*(xs*ys))+((i-1)*xs)+(j-1)]:0;

//		dr=PDPNG_Paeth(ar, br, cr);
//		dg=PDPNG_Paeth(ag, bg, cg);
//		db=PDPNG_Paeth(ab, bb, cb);
//		da=PDPNG_Paeth(aa, ba, ca);
		dr=0; dg=0; db=0; da=0;

		tbuf[0*(xs*ys)+(i*xs)+j]=lr-dr;
		tbuf[1*(xs*ys)+(i*xs)+j]=lg-dg;
		tbuf[2*(xs*ys)+(i*xs)+j]=lb-db;
		tbuf[3*(xs*ys)+(i*xs)+j]=la-da;
	}
#endif
}

int AWAL_Write(FILE *fd, WalInfo_t *inf)
{
	byte tb[256];
	int i, j;

	memset(tb, 0, 256);
	if(inf->name)
		strcpy(tb+ 0, inf->name);
//	if(inf->animname)
//		strcpy(tb+56, inf->animname);

	WAL_Set32LE(tb+32, inf->width);
	WAL_Set32LE(tb+36, inf->height);
	WAL_Set32LE(tb+52, inf->flags);
	WAL_Set32LE(tb+56, inf->contents);
	WAL_Set32LE(tb+60, inf->value);

	i=100;
	WAL_Set32LE(tb+40, i);
//	WAL_Set32LE(tb+44, j);

	fwrite(tb, 1, 100, fd);

	for(i=0; i<inf->n_frames; i++)
	{
		fputc(i?2:1, fd);
		fputc(0, fd);
		fputc(0, fd);
		fputc(0, fd);

		j=inf->frames_sz[i];
		fputc(j&255, fd);
		fputc((j>>8)&255, fd);
		fputc((j>>16)&255, fd);
		fputc((j>>24)&255, fd);

		fwrite(inf->frames[i], 1, j, fd);
	}

	j=ftell(fd)-100;
	WAL_Set32LE(tb+44, j);

//	i=inf->width*inf->height;
//	fwrite(inf->data[0], 1, i, fd);

//	i=(inf->width/2)*(inf->height/2);
//	fwrite(inf->data[1], 1, i, fd);

//	i=(inf->width/4)*(inf->height/4);
//	fwrite(inf->data[2], 1, i, fd);

//	i=(inf->width/8)*(inf->height/8);
//	fwrite(inf->data[3], 1, i, fd);

	fseek(fd, 0, 0);
	fwrite(tb, 1, 100, fd);

	return(0);
}

int AWAL_WriteFile(char *name, WalInfo_t *inf)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)return(-1);
	AWAL_Write(fd, inf);
	fclose(fd);
	return(-1);
}

byte *AWAL_LoadScaleImageMap(char *base, char *suf,
	int frame, int w, int h)
{
	byte tb[1024];
	byte *tbuf, *buf;
	int w1, h1;

	if(frame>=0)
		{ sprintf(tb, "%s_%d_%s.png", base, frame, suf); }
	else
		{ sprintf(tb, "%s_%s.png", base, suf); }

	tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
	if(tbuf)
	{
//		printf("\tGot Norm\n");

		if((w1==w) && (h1==h))
			return(tbuf);
		buf=malloc(w*h*4);
		Tex_ResampleSpline(tbuf, w1, h1, buf, w, h);
		free(tbuf);
		return(buf);
	}

	return(NULL);
}


int AWAL_LoadNormalMap(char *base, int frame,
	byte *buf, int w, int h)
{
	byte *tbuf;
	int i, j, k, l, n, ret;

	if(!buf)return(0);

	ret=0;
	tbuf=AWAL_LoadScaleImageMap(base, "norm", frame, w, h);
	if(tbuf)
	{
		n=w*h; l=0;
		for(i=0; i<n; i++)
		{
			buf[i*4+0]=tbuf[i*4+0];
			buf[i*4+1]=tbuf[i*4+1];
			buf[i*4+2]=tbuf[i*4+2];
			
			j=tbuf[i*4+3];
			buf[i*4+3]=j;
			l+=j;
//			buf[i*4+3]=128;
		}
		
		l/=n;
		if(l>200)
		{
			for(i=0; i<n; i++)
			{
				j=buf[i*4+3];
				buf[i*4+3]=j*(128.0/l);
			}
		}

		free(tbuf);
		ret++;
	}

	tbuf=AWAL_LoadScaleImageMap(base, "bmp", frame, w, h);
	if(tbuf)
	{
		n=w*h;
		for(i=0; i<n; i++)
			{ buf[i*4+3]=tbuf[i*4+1]; }
		free(tbuf);
		ret++;
	}

	return(ret);
}

int AWAL_LoadSpecularMap(char *base, int frame,
	byte *buf, int w, int h)
{
	byte *tbuf;
	int i, j, k, l, n, ret;

	if(!buf)return(0);

	ret=0;
	tbuf=AWAL_LoadScaleImageMap(base, "spec", frame, w, h);
	if(tbuf)
	{
		n=w*h; l=0;
		for(i=0; i<n; i++)
		{
			buf[i*4+0]=tbuf[i*4+0];
			buf[i*4+1]=tbuf[i*4+1];
			buf[i*4+2]=tbuf[i*4+2];
//			buf[i*4+3]=tbuf[i*4+3];
//			buf[i*4+3]=128;

			j=tbuf[i*4+3];
			buf[i*4+3]=j;
			l+=j;
		}

		l/=n;
		if(l>200)
		{
			for(i=0; i<n; i++)
			{
				j=buf[i*4+3];
				buf[i*4+3]=j*(128.0/l);
			}
		}
		free(tbuf);
		ret++;
	}

	tbuf=AWAL_LoadScaleImageMap(base, "spexp", frame, w, h);
	if(tbuf)
	{
		n=w*h;
		for(i=0; i<n; i++)
			{ buf[i*4+3]=tbuf[i*4+1]; }
		free(tbuf);
		ret++;
	}

	return(ret);
}

int AWAL_LoadLumaMap(char *base, int frame,
	byte *buf, int w, int h)
{
	byte tb[1024];
	byte *tbuf;
	int w1, h1;

	if(!buf)return(0);

	if(frame>=0)
		{ sprintf(tb, "%s_%d_%s.png", base, frame, "luma"); }
	else
		{ sprintf(tb, "%s_%s.png", base, "luma"); }

	tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
	if(tbuf)
	{
//		printf("\tGot Luma\n");
		Tex_ResampleSpline(tbuf, w1, h1, buf, w, h);
		free(tbuf);
		return(1);
	}
	return(0);
}

int imgstat(byte *simg, byte *dimg, int n)
{
	float er, eg, eb, ea;
	int i, j;
	
	er=0; eg=0; eb=0; ea=0;
	for(i=0; i<n; i++)
	{
		j=dimg[i*4+0]-simg[i*4+0];	er+=j*j;
		j=dimg[i*4+1]-simg[i*4+1];	eg+=j*j;
		j=dimg[i*4+2]-simg[i*4+2];	eb+=j*j;
		j=dimg[i*4+3]-simg[i*4+3];	ea+=j*j;
	}
	
	printf("%f %f %f %f\n", sqrt(er/n), sqrt(eg/n), sqrt(eb/n), sqrt(ea/n));
}

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

#if 0
	strcpy(tb, argv[1]);
	base=tb+strlen((char *)tb);
	while(base>tb)
	{
		if(*base=='.')
			{ *base=0; break; }
		base--;
	}
	base=strdup(tb);
#endif

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

	if(AWAL_LoadNormalMap(base, -1, nvbuf, w, h)<=0)
		{ free(nvbuf); nvbuf=NULL; }
	if(AWAL_LoadSpecularMap(base, -1, spbuf, w, h)<=0)
		{ free(spbuf); spbuf=NULL; }
	if(AWAL_LoadLumaMap(base, -1, lubuf, w, h)<=0)
		{ free(lubuf); lubuf=NULL; }

//	AWAL_LoadSpecularMap(base, i, nvbuf, w, h);
#endif

#if 0
	nvbuf=NULL;
	spbuf=NULL;
	lubuf=NULL;

	sprintf(tb, "%s_norm.png", base);
	tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
	if(tbuf)
	{
		nvbuf=malloc(w*h*4);
		Tex_ResampleSpline(tbuf, w1, h1, nvbuf, w, h);
		free(tbuf);
	}

	sprintf(tb, "%s_spec.png", base);
	tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
	if(tbuf)
	{
		spbuf=malloc(w*h*4);
		Tex_ResampleSpline(tbuf, w1, h1, spbuf, w, h);
		free(tbuf);
	}

	sprintf(tb, "%s_luma.png", base);
	tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
	if(tbuf)
	{
		lubuf=malloc(w*h*4);
		Tex_ResampleSpline(tbuf, w1, h1, lubuf, w, h);
		free(tbuf);
	}
#endif

	printf("Img: %dx%d\n", w, h);

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

#if 1
	if(0)
	{
		for(i=0; i<64; i++)
		{
			f=(i*i)/240.0;
			g=pow(2, f);
			printf("%d %f %f %f\n", i, f, g, 1/g);
			
			exptab[192+i]=f;
			exptab[192-i]=-f;
			exptab[63+i]=-f;
			exptab[63-i]=f;
			
			scltab[192+i]=g;
			scltab[192-i]=1.0/g;
			scltab[63+i]=-1.0/g;
			scltab[63-i]=-g;
		}

		for(i=0; i<18; i++)
		{
			j=sqrt(i*240)+0.5;
			printf("%d %d %d %d %d %d\n", i, j,
				192+j, 192-j, 63-j, 63+j);

			if(j>64)continue;

			g=pow(2, i);

			exptab[192+j]=i;
			exptab[192-j]=-i;
			exptab[63+j]=-i;
			exptab[63-j]=i;

			scltab[192+j]=g;
			scltab[192-j]=1.0/g;
			scltab[63+j]=-1.0/g;
			scltab[63-j]=-g;
		}
		
//		exptab[128]=0;

		for(i=0; i<(256/2); i++)
		{
			for(j=0; j<2; j++)
			{
				sprintf(tb, "% 3d: %.4f,%.8f", i*2+j,
					exptab[i*2+j], scltab[i*2+j]);
				printf("%-32s ", tb);
			}
			printf("\n");
		}

		return(0);
	}

	if(0)
	{
		printf("ORCT Test\n");
		
		for(i=0; i<256; i++)
			for(j=0; j<256; j++)
				for(k=0; k<256; k++)
		{
			cy=(i+2*j+k)/4;
			cu=(k-j)+128;
			cv=(i-j)+128;

//			cy=cg+(cb+cr-2*cg)/4;
			cy=j+(k+i-2*j)/4;

//			cg=cy-(cu+cv-256)/4;
//			cy+=j-cg;
//			if(cg!=j)cy++;
			
			cu-=128; cv-=128;
			cg=cy-(cu+cv)/4;
			cr=cv+cg;
			cb=cu+cg;
			
			if((cr!=i) || (cg!=j) || (cb!=k))
				printf("%d,%d,%d -> %d,%d,%d -> %d,%d,%d\n",
					i, j, k, cy, cu, cv, cr,cg,cb);
		}
		return(0);
	}
#endif

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

		printf("Decode Test\n");
		t0=clock(); k=0;
		while(1)
		{
			t1=clock();
			t2=t1-t0;
//			if((t2<0) || (t2>(30*CLOCKS_PER_SEC)))
			if((t2<0) || (t2>(10*CLOCKS_PER_SEC)))
				break;
	
			j=PDJPG_DecodeComponent(tbuf, i,
				buf, nvbuf, spbuf, lubuf,
				&w1, &h1);
			k++;

			printf("Decoded %d times, ret=%d, dt=%.4fs rate=%.4f/s\r", k, j,
				t2/(float)CLOCKS_PER_SEC,
				k/(t2/(float)CLOCKS_PER_SEC));
			if(j<0)break;
		}
		printf("Decoded %d times\n", k);

		i=!memcmp(buf, obuf, w*h*4);
		printf("Lossless RGBA %s\n", i?"Yes":"No");
		if(!i)imgstat(buf, obuf, w*h);

		i=!memcmp(nvbuf, onvbuf, w*h*4);
		printf("Lossless Norm %s\n", i?"Yes":"No");
		if(!i)imgstat(nvbuf, onvbuf, w*h);

		i=!memcmp(spbuf, ospbuf, w*h*4);
		printf("Lossless Specular %s\n", i?"Yes":"No");
		if(!i)imgstat(spbuf, ospbuf, w*h);

		i=!memcmp(lubuf, olubuf, w*h*4);
		printf("Lossless Luma %s\n", i?"Yes":"No");
		if(!i)imgstat(lubuf, olubuf, w*h);

#if 1
		i=PDJPG_EncodeComponent(
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

	sprintf(tb, "%s.avi", base);
	ctx=PDGL_AVI_OpenOutStream(tb, w, h, 10, fcc);

//	wal=WAL_CreateWal(buf, w, h, NULL);
//	wal->name=strdup(base);
//	sprintf(tb, "%s.txt", base);
//	WAL_ReadInfo(tb, wal);

//	printf("%dx%d\t%08X %08X\n",
//		wal->width, wal->height,
//		wal->flags, wal->contents);

	free(buf);	//don't need

//	wal->frames=malloc(1024*sizeof(byte *));
//	wal->frames_sz=malloc(1024*sizeof(int));
//	wal->n_frames=0;

	lbuf=malloc(w*h*4);
	memset(lbuf, 0, w*h*4);
	tbuf=malloc(w*h*4);

	for(i=0; i<1024; i++)
	{
		sprintf(tb, "%s_%d.png", base, i);
		buf=PDPNG_LoadRaw(tb, &w1, &h1);
		if(!buf)break;
		
		printf("Got %s %dx%d\n", tb, w1, h1);

		k=0;
		if(!i)k=15;

		if(AWAL_LoadNormalMap(base, i, nvbuf, w, h)>0)k|=1;
		if(AWAL_LoadSpecularMap(base, i, spbuf, w, h)>0)k|=2;
		if(AWAL_LoadLumaMap(base, i, lubuf, w, h)>0)k|=4;

#if 0
		if(nvbuf)
		{
			sprintf(tb, "%s_%d_norm.png", base, i);
			tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
			if(tbuf)
			{
//				printf("\tGot Norm\n");
				Tex_ResampleSpline(tbuf, w1, h1, nvbuf, w, h);
				free(tbuf);
				k|=1;
			}
		}
		if(spbuf)
		{
			sprintf(tb, "%s_%d_spec.png", base, i);
			tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
			if(tbuf)
			{
//				printf("\tGot Spec\n");
				Tex_ResampleSpline(tbuf, w1, h1, spbuf, w, h);
				free(tbuf);
				k|=2;
			}
		}
		if(lubuf)
		{
			sprintf(tb, "%s_%d_luma.png", base, i);
			tbuf=PDPNG_LoadRaw(tb, &w1, &h1);
			if(tbuf)
			{
//				printf("\tGot Luma\n");
				Tex_ResampleSpline(tbuf, w1, h1, lubuf, w, h);
				free(tbuf);
				k|=4;
			}
		}
#endif

		if(k)printf("\t k=%d\n", k);

		ctx->accDt2=1.0/ctx->fps;
//		PDGL_AVI_EncodeFrame(ctx, buf, w1, h1);
		PDGL_AVI_EncodeComponentFrame(ctx,
			buf, (k&1)?nvbuf:NULL,
			(k&2)?spbuf:NULL, (k&4)?lubuf:NULL,
			w1, h1);
		free(buf);

#if 0
//		AWAL_FilterBuf(tbuf, buf, lbuf, w, h, i?0:1);

		tbuf1=malloc(w*h*8);

//		for(j=0; j<w*h*4; j++)
//			tbuf1[j]=tbuf[j];
//		for(j=0; j<w*h; j++)
//			tbuf1[j*4+3]=255;

//		AWAL_UnFilterBuf(tbuf1, tbuf, lbuf, w, h, i?0:1);

//		sprintf(tb, "%s_X_%d.png", base, i);
//		PDPNG_SaveRaw(tb, tbuf1, w, h);

		memcpy(lbuf, buf, w*h*4);
		free(buf);
				
		k=0;
		for(j=0; j<w*h*4; j++)
			k+=tbuf[j];
			
		printf("Entropy=%d, AvgEntropy=%d\n", k, k/j);
		
		//zlib header
		j=(7<<12)|(8<<8)|(3<<6);
		k=j%31;
		if(k)j|=31-k;
		tbuf1[0]=j>>8;
		tbuf1[1]=j&0xFF;

		//deflated frame data
		j=PDZ2_EncodeStreamLvl(tbuf, tbuf1+2, w*h*4, w*h*8, 9);
		
		//checksum
		k=PDPNG_DataAdler32(tbuf, w*h*4, 1);
		tbuf1[2+j+0]=(k>>24)&0xFF;
		tbuf1[2+j+1]=(k>>16)&0xFF;
		tbuf1[2+j+2]=(k>>8)&0xFF;
		tbuf1[2+j+3]=k&0xFF;

		printf("Frame Size: %d\n", j+6);

		wal->frames[i]=tbuf1;
		wal->frames_sz[i]=j+6;
		wal->n_frames=i+1;
#endif

	}

	PDGL_AVI_CloseOutStream(ctx);

//	sprintf(tb, "%s.wal", base);
//	AWAL_WriteFile(tb, wal);
#endif

	return(0);
}
