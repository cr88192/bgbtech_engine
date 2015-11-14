#include <bgbbtj.h>

// uint BGBBTJ_PNG_DataAdler32(void *buf, int sz, uint lcrc);
// int BGBBTJ_PNG_Paeth(int a, int b, int c);
// byte *BGBBTJ_PNG_LoadRaw(char *name, int *w, int *h);

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
	return(0);
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
			d=BGBBTJ_PNG_Paeth(a, b, c);
		}else d=0;

//		tbuf[k*(xs*ys)+(i*xs)+j]=l-d;
		tbuf[(i*xs+j)*4+k]=l+d+v;
	}
}

BGBBTJ_API void AWAL_FilterBuf(byte *tbuf, byte *buf, byte *lbuf,
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
			d=BGBBTJ_PNG_Paeth(a, b, c);
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

//		dr=BGBBTJ_PNG_Paeth(ar, br, cr);
//		dg=BGBBTJ_PNG_Paeth(ag, bg, cg);
//		db=BGBBTJ_PNG_Paeth(ab, bb, cb);
//		da=BGBBTJ_PNG_Paeth(aa, ba, ca);
		dr=0; dg=0; db=0; da=0;

		tbuf[0*(xs*ys)+(i*xs)+j]=lr-dr;
		tbuf[1*(xs*ys)+(i*xs)+j]=lg-dg;
		tbuf[2*(xs*ys)+(i*xs)+j]=lb-db;
		tbuf[3*(xs*ys)+(i*xs)+j]=la-da;
	}
#endif
}

BGBBTJ_API int AWAL_Write(FILE *fd, WalInfo_t *inf)
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

BGBBTJ_API int AWAL_WriteFile(char *name, WalInfo_t *inf)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)return(-1);
	AWAL_Write(fd, inf);
	fclose(fd);
	return(-1);
}

BGBBTJ_API byte *AWAL_LoadScaleImageMap(char *base, char *suf,
	int frame, int w, int h)
{
	byte tb[1024];
	byte *tbuf, *buf;
	int w1, h1;

	if(frame>=0)
		{ sprintf(tb, "%s_%d_%s.png", base, frame, suf); }
	else
		{ sprintf(tb, "%s_%s.png", base, suf); }

	tbuf=BGBBTJ_PNG_LoadRaw(tb, &w1, &h1);
	if(tbuf)
	{
//		printf("\tGot Norm\n");

		if((w1==w) && (h1==h))
			return(tbuf);
		buf=malloc(w*h*4);
		BGBBTJ_Tex_ResampleSpline(tbuf, w1, h1, buf, w, h);
		free(tbuf);
		return(buf);
	}

	return(NULL);
}


BGBBTJ_API int AWAL_LoadNormalMap(char *base, int frame,
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

BGBBTJ_API int AWAL_LoadSpecularMap(char *base, int frame,
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

BGBBTJ_API int AWAL_LoadLumaMap(char *base, int frame,
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

	tbuf=BGBBTJ_PNG_LoadRaw(tb, &w1, &h1);
	if(tbuf)
	{
//		printf("\tGot Luma\n");
		BGBBTJ_Tex_ResampleSpline(tbuf, w1, h1, buf, w, h);
		free(tbuf);
		return(1);
	}
	return(0);
}

BGBBTJ_API int imgstat(byte *simg, byte *dimg, int n)
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
	return(0);
}

BGBBTJ_API int bgbbtj_imgflip(byte *simg, byte *dimg, int xs, int ys)
{
	int i, j;
	
	for(i=0; i<ys; i++)
		for(j=0; j<xs; j++)
	{
		dimg[((ys-i-1)*xs+j)*4+0]=simg[(i*xs+j)*4+0];
		dimg[((ys-i-1)*xs+j)*4+1]=simg[(i*xs+j)*4+1];
		dimg[((ys-i-1)*xs+j)*4+2]=simg[(i*xs+j)*4+2];
		dimg[((ys-i-1)*xs+j)*4+3]=simg[(i*xs+j)*4+3];
	}
	return(0);
}

BGBBTJ_API int bgbbtj_imgflip_single(byte *img, int xs, int ys)
{
	byte *timg;
	
	timg=malloc(xs*ys*4);
	bgbbtj_imgflip(img, timg, xs, ys);
	memcpy(img, timg, xs*ys*4);
	free(timg);
	return(0);
}

BGBBTJ_API int BGBBTJ_BlendMagentaEncodeColor(byte *src, byte *dst)
{
	int cr, cg, cb, ca;
	int i, j, k, l;

	cr=src[0];	cg=src[1];
	cb=src[2];	ca=src[3];
	
	//Avoid Magenta in output
	i=(cr-255);
	j=(cg-0);
	k=(cb-255);
	l=i*i+j*j+k*k;
	if(l<0)l=0;
	if(l>256)l=256;

//	j=(cr+2*cg+cb)>>2;
//	cr=((cr*l)+(j*(256-k)))>>8;
//	cg=((cg*l)+(j*(256-k)))>>8;
//	cb=((cb*l)+(j*(256-k)))>>8;

//	i=(224*cr)>>8;
//	j=(224*cb)>>8;
//	cr=((cr*l)+(i*(256-l)))>>8;
//	cb=((cb*l)+(j*(256-l)))>>8;

	j=(255-ca)>>4;
	k=(255-l)>>4;
	cr=cr-(j+k);
	cb=cb-(j+k);
	
	//Blend Alpha As Magenta
	l=255-ca;
	k=255-((l*l)>>8);
//	k=(ca*ca)>>8;

	cr=((cr*k)+(255*(256-k)))>>8;
	cg=((cg*k)+(0*(256-k)))>>8;
	cb=((cb*k)+(255*(256-k)))>>8;
	
	dst[0]=cr;	dst[1]=cg;
	dst[2]=cb;	dst[3]=ca;
	return(0);
}

BGBBTJ_API int BGBBTJ_BlendMagentaDecodeColor(byte *src, byte *dst)
{
	int cr, cg, cb, ca;
	int i, j, k, l;

	cr=src[0];	cg=src[1];
	cb=src[2];	ca=src[3];
	
	i=(cr-255);
	j=(cg-0);
	k=(cb-255);
	l=i*i+j*j+k*k;
	if(l<0)l=0;
	if(l>255)l=255;

	k=(255-l)>>4;
	cr=cr+k;
	cb=cb+k;
	if(cr>255)cr=255;
	if(cb>255)cb=255;

	ca=l;

	dst[0]=cr;	dst[1]=cg;
	dst[2]=cb;	dst[3]=ca;
	return(0);
}

BGBBTJ_API void BGBBTJ_BlendMagentaEncodeImage(byte *simg, byte *dimg, int n)
{
	int i;
	for(i=0; i<n; i++)
		{ BGBBTJ_BlendMagentaEncodeColor(simg+i*4, dimg+i*4); }
}

BGBBTJ_API void BGBBTJ_BlendMagentaDecodeImage(byte *simg, byte *dimg, int n)
{
	int i;
	for(i=0; i<n; i++)
		{ BGBBTJ_BlendMagentaDecodeColor(simg+i*4, dimg+i*4); }
}

BGBBTJ_API int BGBBTJ_SuperMagenta8_PointDist(
	int cy0, int cu0, int cv0,
	int cy1, int cu1, int cv1)
{
	int i, j, k;
	
	i=cy0-cy1;
	j=cu0-cu1;
	k=cv0-cv1;
	k=(i*i)+(j*j)+(k*k);
//	if(k>255)k=255;
	return(k);
}

BGBBTJ_API int BGBBTJ_SuperMagenta8_YuvDist(int cy, int cu, int cv)
{
	int i, j, k;
	
	i=cy-255;
	j=cu-255;
	k=cv-255;
	k=(i*i)+(j*j)+(k*k);
//	if(k>255)k=255;
	return(k);
}

BGBBTJ_API int BGBBTJ_SuperMagenta8_YUV2A(int cy, int cu, int cv)
{
	int i, j, k;
	
	i=cy-255;
	j=cu-255;
	k=cv-255;
	k=(i*i)+(j*j)+(k*k);
//	k=k/4;
//	k=k/16;
	k=k/48;
	if(k>255)k=255;
	return(k);
}

BGBBTJ_API void BGBBTJ_SuperMagenta8_ConvRGBA2YUV(
	int cr, int cg, int cb, int ca,
	int *rcy, int *rcu, int *rcv)
{
//	int cr, cg, cb, ca;
	int cy, cu, cv;
	int py, pu, pv;
	int i, j, k, l;
	float f, g;

	//opaque
//	if(ca>=252)
	if(ca>=224)
//	if(1)
	{
		*rcy=(( 19595*cr + 38470*cg + 7471*cb + 32768)>>16)+0;
		*rcu=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;
		*rcv=(( 32768*cr - 27440*cg - 5328*cb + 32768)>>16)+128;

#if 0
		cy=( 19595*cr + 38470*cg + 7471*cb + 32768)>>16;
		cu=(-11056*cr - 21712*cg +32768*cb + 32768)>>16;
		cv=( 32768*cr - 27440*cg - 5328*cb + 32768)>>16;

		l=BGBBTJ_SuperMagenta8_YUV2A(cy, cu, cv);
		if(l<255)
		{
			cu-=sqrt(255-l);
			cv-=sqrt(255-l);
		}

		*rcy=cy;
		*rcu=cu;
		*rcv=cv;
#endif
		return;
	}
	
	//fully transparent
	if(ca<=16)
	{
		*rcy=255;
		*rcu=255;
		*rcv=255;
		return;
	}

//	cy=(( 19595*cr + 38470*cg + 7471*cb + 32768)>>16)+0;
//	cu=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;
//	cv=(( 32768*cr - 27440*cg - 5328*cb + 32768)>>16)+128;

//	cy=(( 27787*cr + 30278*cg + 7471*cb + 32768)>>16)+0;
//	cy=(( 25297*cr + 32768*cg + 7471*cb + 32768)>>16)+0;

//	cu=(( 16384*cr + 16384*cg - 32768*cb + 32768)>>16)+128;
//	cv=((-32768*cr + 16384*cg + 16384*cb + 32768)>>16)+128;

	cy=(( 22446*cr + 35619*cg + 7471*cb + 32768)>>16)+0;
	cu=((-11056*cr - 21712*cg +32768*cb + 32768)>>16)+128;

//	cu=128; cv=128;

#if 1
	py=(ca/4);
//	pu=((cy+cv)/2)/4;
//	pu=((3*cy+cv)/4)/4;
//	pu=((cy+(cv-128)/2))/4;
	pu=cy/4;
	pv=cu/4;

//	pu=cu/4;
//	pv=cv/4;
	
	cy=255-py;
	cu=255-pu;
	cv=255-pv;
#endif

#if 0
#if 1
	j=cy*cy+cu*cu+cv*cv;
	f=255.0/sqrt(j);
//	f=1.5-0.5*(cy/128.0);
	cy=cy*f;
	cu=cu*f;
	cv=cv*f;
#endif

//	l=255-ca;
//	k=255-((l*l)>>8);
//	k=(ca*ca)>>8;
//	k=255-((int)(4*sqrt(l)));
	k=4*sqrt(ca);

	cy=(cy*k)>>8;
	cu=(cu*k)>>8;
	cv=(cv*k)>>8;

	cy=255-cy;
	cu=255-cu;
	cv=255-cv;

//	cy=(255-k)+cy;
//	cu=(255-k)+cu;
//	cv=(255-k)+cv;

//	j=BGBBTJ_SuperMagenta8_YuvDist(cy, cu, cv);
//	j=sqrt(j);
//	cy=255-((255-cy)*(k/(float)j));
//	cu=255-((255-cu)*(k/(float)j));
//	cv=255-((255-cv)*(k/(float)j));

//	cy=((cy*k)+(255*(256-k)))>>8;
//	cu=((cu*k)+(255*(256-k)))>>8;
//	cv=((cv*k)+(255*(256-k)))>>8;
#endif

	*rcy=cy;
	*rcu=cu;
	*rcv=cv;
}

BGBBTJ_API void BGBBTJ_SuperMagenta8_ConvYUV2RGBA(
	int cy, int cu, int cv,
	int *rcr, int *rcg, int *rcb, int *rca)
{
	int pr, pg, pb, pa;
	int py, pu, pv;
	int mu, mv;
	float f, g;
	int d;

//	pa=BGBBTJ_SuperMagenta8_YUV2A(cy, cu, cv);
	d=BGBBTJ_SuperMagenta8_YuvDist(cy, cu, cv);
	
//	if(pa>=252)
//	if(1)
	if(d>=12288)
	{
		mu=cu-128; mv=cv-128;
		pr=65536*cy          +91881*mv;
		pg=65536*cy- 22554*mu-46802*mv;
		pb=65536*cy+116130*mu;
		pr>>=16; pg>>=16; pb>>=16;

		*rcr=(pr<0)?0:((pr>255)?255:pr);
		*rcg=(pg<0)?0:((pg>255)?255:pg);
		*rcb=(pb<0)?0:((pb>255)?255:pb);
		*rca=255;
		return;
	}
	
//	if(pa<=16)
//	if(0)
	if(d<=16)
	{
		*rcr=0;
		*rcg=0;
		*rcb=0;
		*rca=0;
		return;
	}

#if 0
	pu=(255-cu)*4;
	pv=(255-cv)*4;
#endif

#if 1
	py=(255-cu)*4;
//	pu=py;
//	pu=128;
	pu=(255-cv)*4;
//	pv=py;
	pv=128;
//	pv=(py+128)/2;
//	pv=((py-128)/2)+128;
#endif

//	pu=((py-128)/2)+128;
//	pv=(255-cv)*4;
	pa=(255-cy)*4;
	
#if 0
//	py=cy;
//	pu=cu;
//	pv=cv;
	
//	py=cy*(4*(256.0/pa));
//	pu=cu*(4*(256.0/pa));
//	pv=cv*(4*(256.0/pa));

	py=(255-cy)*(4*(256.0/pa));
	pu=(255-cu)*(4*(256.0/pa));
	pv=(255-cv)*(4*(256.0/pa));
	
#if 1
//	f=py/148.0;
//	f=sqrt(py/148.0);
//	f=sqrt(py/128.0);
	f=0.71+0.72*pow(py/180.0, 2);
//	f=1.0/(1.5-0.5*(py/128.0));
	py=py*f;
	pu=pu*f;
	pv=pv*f;
#endif

#if 0
	//x/sqrt(x)

//	pu=pu*sqrt()

	f=(py-128)/128.0;
	g=sqrt(f*f);
//	pu=128+pu-128/(1.0-f);
//	pv=128+pv-128/(1.0-f);

	pu=pu*(1.0-f);
	pv=pv*(1.0-f);

//	j=cy*cy+cu*cu+cv*cv;
//	f=255.0/sqrt(j);
//	cy=cy/f;
//	cu=cu/f;
//	cv=cv/f;
#endif
#endif

#if 0
	mu=pu-128; mv=pv-128;

	pr=65536*pu;
	pg=32768*pu+32768*pv;
	pb=65536*pv;
	if(pg<0)pg=-pg;
#endif

#if 0
//	pu=128; pv=128;

	mu=pu-128; mv=pv-128;
	pr=65536*py          +91881*mv;
	pg=65536*py- 22554*mu-46802*mv;
	pb=65536*py+116130*mu;

//	pr=65536*py;
//	pr=84891*py;
//	pr=50594*py;
//	pg=65536*py- 22554*mu;
//	pb=65536*py+116130*mu;

	pr>>=16; pg>>=16; pb>>=16;
#endif

#if 1
	mv=pu-128;
	pr=65536*py;
	pg=65536*py-116130*mv;
	pb=65536*py+116130*mv;
#endif

//	pr=(pr<0)?0:((pr>255)?255:pr);
//	pg=(pg<0)?0:((pg>255)?255:pg);
//	pb=(pb<0)?0:((pb>255)?255:pb);

	*rcr=(pr<0)?0:((pr>255)?255:pr);
	*rcg=(pg<0)?0:((pg>255)?255:pg);
	*rcb=(pb<0)?0:((pb>255)?255:pb);
	*rca=pa;
}


static float bgbbtj_scurve(float a)
	{ return(a*a*(3-2*a)); }

static int bgbbtj_scurvei8(int a)
{
	return((a*a*(767-2*a)+32768)>>16);
}

BGBBTJ_API int BGBBTJ_Tex_BlueScreen(byte *src, byte *dst, int w, int h)
{
	byte *tbuf;
	int cr, cg, cb, ca;
	int cr2, cg2, cb2;
	int ca0, ca1, ca2, ca3;
	int ca4, ca5, ca6, ca7, ca8;
	int k0, k1, k2, k3, k4, k5, k6, k7, k8;

	int n;
	int i, j, k, l;

	tbuf=malloc(w*h*4);

	n=w*h;
//	for(i=0; i<n; i++)
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		k=(i*w+j)*4;
		cr=src[k+0];		cg=src[k+1];
		cb=src[k+2];		ca=src[k+3];

//		if((i>8) && (j>8))
		if(0)
		{
			k0=((i-0)*w+(j-0))*4;
			k1=((i-0)*w+(j-4))*4;
			k2=((i-4)*w+(j-0))*4;
			k3=((i-4)*w+(j-4))*4;

			k4=((i-0)*w+(j-2))*4;
			k5=((i-4)*w+(j-2))*4;
			k6=((i-0)*w+(j-8))*4;
			k7=((i-4)*w+(j-8))*4;

			cr2=(src[k0+0]+src[k1+0]+src[k2+0]+src[k3+0]+
				 src[k4+0]+src[k5+0]+src[k6+0]+src[k7+0])>>3;
			cg2=(src[k0+1]+src[k1+1]+src[k2+1]+src[k3+1]+
				 src[k4+1]+src[k5+1]+src[k6+1]+src[k7+1])>>3;
			cb2=(src[k0+2]+src[k1+2]+src[k2+2]+src[k3+2]+
				 src[k4+2]+src[k5+2]+src[k6+2]+src[k7+2])>>3;

//			cr2=(src[k0+0]+src[k1+0]+
//				 src[k2+0]+src[k3+0])>>2;
//			cg2=(src[k0+1]+src[k1+1]+
//				 src[k2+1]+src[k3+1])>>2;
//			cb2=(src[k0+2]+src[k1+2]+
//				 src[k2+2]+src[k3+2])>>2;
		}else
		{
			cr2=cr; cg2=cg; cb2=cb;
		}

#if 1
//		if((cb>=(1.1*cg)) && (cb>=(1.1*cr)))
//		if((cg>=(1.1*cr)) && (cb>=(1.1*cr)))
//		if((cg>=(1.05*cr)) && (cb>=(1.05*cr)))
//		if((cg2>=(1.05*cr2)) && (cb2>=(1.05*cr2)))
//		if((cg2>=(1.025*cr2)) && (cb2>=(1.025*cr2)))
//		if((cg2>=(1.001*cr2)) && (cb2>=(1.001*cr2)))
//		if((cg2>=(1.000*cr2)) && (cb2>=(1.000*cr2)))
//		if(((cg2>=(1.025*cr2)) && (cb2>=(1.025*cr2))) &&
//			(cb>=16) && (cr<192))
		if((((cg2>=(1.01*cr2)) && (cb2>=(1.01*cr2))) ||
				(cb2>=(1.01*cr2))) &&
			(cb2>=16) && (cr2<192))
		{
//			cr=255;
//			cg=0;
//			cb=255;
			ca=0;
		}
#if 0
		else if((cb2>=16) && (cr2<192))
		{
			k=2.5*cr2-cg2-cb2;
			k=k*k;
			if(k>255)k=255;
			ca=k;
		}
#endif
#endif

		k=(i*w+j)*4;
		dst[k+0]=cr;		dst[k+1]=cg;
		dst[k+2]=cb;		dst[k+3]=ca;
	}

	for(k=0; k<4; k++)
	{
		for(i=1; i<(h-1); i++)
			for(j=1; j<(w-1); j++)
		{
//			k0=((i+0)*w+(j+0))*4;
//			k1=((i+0)*w+(j-1))*4;
//			k2=((i-1)*w+(j+0))*4;
//			k3=((i-1)*w+(j-1))*4;

//			k4=((i-1)*w+(j+1))*4;
//			k5=((i+0)*w+(j+1))*4;
//			k6=((i+1)*w+(j-1))*4;
//			k7=((i+1)*w+(j+1))*4;

			k0=((i-1)*w+(j-1))*4; k1=((i-1)*w+(j+0))*4; k2=((i-1)*w+(j+1))*4;
			k3=((i+0)*w+(j-1))*4; k4=((i+0)*w+(j+0))*4; k5=((i+0)*w+(j+1))*4;
			k6=((i+1)*w+(j-1))*4; k7=((i+1)*w+(j+0))*4; k8=((i+1)*w+(j+1))*4;
		
			ca0=dst[k0+3];	ca1=dst[k1+3];	ca2=dst[k2+3];
			ca3=dst[k3+3];	ca4=dst[k4+3];	ca5=dst[k5+3];
			ca6=dst[k6+3];	ca7=dst[k7+3];	ca8=dst[k8+3];

//			ca=(ca0+ca1+ca2+ca3)/4;
//			ca=(ca0+ca1+ca2+ca3+ca4+ca5+ca6+ca7)/8;
			ca=(	2*ca0+4*ca1+2*ca2+
					4*ca3+8*ca4+4*ca5+
					2*ca6+4*ca7+2*ca8)/32;
//			dst[k0+3]=ca;
			tbuf[k0+3]=ca;
//			if(ca<=128)
//			{
//				dst[k0+3]=0;
//			}
		}
		
		for(i=1; i<(h-1); i++)
			for(j=1; j<(w-1); j++)
		{
			k=(i*w+j)*4;
			ca=tbuf[k+3];
			dst[k+3]=ca;
		}
	}

#if 0
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		k=(i*w+j)*4;
		ca=dst[k+3];
		if(ca<192)
//		if(ca<224)
		{
			dst[k+0]=255;
			dst[k+1]=0;
			dst[k+2]=255;
			dst[k+3]=0;
		}
	}
#endif

#if 1
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		k=(i*w+j)*4;
		ca=dst[k+3];
//		ca=(ca*ca)>>8;
		ca=bgbbtj_scurvei8(ca);
		if(ca<0)ca=0;
		if(ca>255)ca=255;
		dst[k+3]=ca;

		if(ca<=0)
		{
			dst[k+0]=255;
			dst[k+1]=0;
			dst[k+2]=255;
			dst[k+3]=0;
		}
	}
#endif
	
	free(tbuf);
	return(0);
}

BGBBTJ_API int BGBBTJ_Tex_GreenScreen(byte *src, byte *dst, int w, int h)
{
	static byte *lsbuf=NULL;
	byte *tbuf;
	float f, g;
	int cr, cg, cb, ca;
	int cy, cu, cv;
	int cr2, cg2, cb2;
	int ca0, ca1, ca2, ca3;
	int ca4, ca5, ca6, ca7, ca8;

	int cy0, cy1, cy2, cy3;
	int cy4, cy5, cy6, cy7, cy8;

	int k0, k1, k2, k3, k4, k5, k6, k7, k8;
	int p0, p1, p2, p3;
	int q0, q1, q2, q3;

	int n;
	int i, j, k, l;

	if(!lsbuf)
	{
		lsbuf=malloc(w*h*4);
		memcpy(lsbuf, src, w*h*4);
	}

	tbuf=malloc(w*h*4);

	n=w*h;
//	for(i=0; i<n; i++)
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		k=(i*w+j)*4;
		cr=src[k+0];		cg=src[k+1];
		cb=src[k+2];		ca=src[k+3];
		cy=(2*cg+cr+cb)>>2;

//		cr2=(src[k+0]+lsbuf[k+0])>>1;
//		cg2=(src[k+1]+lsbuf[k+1])>>1;
//		cb2=(src[k+2]+lsbuf[k+2])>>1;

		cr2=(3*src[k+0]+lsbuf[k+0])>>2;
		cg2=(3*src[k+1]+lsbuf[k+1])>>2;
		cb2=(3*src[k+2]+lsbuf[k+2])>>2;

#if 0
//		cy1=(cy>>1);
		cy1=2*cy;
		if(!cy1)cy1=1;
		cr2=(256*cr2)/cy1;
		cg2=(256*cg2)/cy1;
		cb2=(256*cb2)/cy1;

		cr2=(cr2<0)?0:((cr2>255)?255:cr2);
		cg2=(cg2<0)?0:((cg2>255)?255:cg2);
		cb2=(cb2<0)?0:((cb2>255)?255:cb2);
#endif

		cy2=(2*cg2+cr2+cb2)>>2;

//		cr2=cr; cg2=cg; cb2=cb;

#if 0
//		if(cg2>16)
//		if(cy>24)
		if(cy>16)
		{
//			cy0=(2*cg2+cr2+cb2)>>2;
			cy0=(cr2+cb2)>>1;
			if(cy0<1)cy0=1;
//			ca2=(cg2-cr2)+(cg2-cb2);
			ca2=cg2-cy0;
//			ca2=(256*ca2)/cg2;
			ca2=(256*ca2)/cy0;
//			ca2=(128*ca2)/cy0;
//			ca2=ca2*2;
			ca2=(ca2*3)>>1;
			if(ca2<0)ca2=0;
			ca2=(ca2*ca2)>>8;
//			ca2=(ca2*ca2*ca2)>>16;
			ca=255-ca2;
			if(ca<0)ca=0;
			if(ca>255)ca=255;
		}
#endif

#if 1
//		if(((cg2>=(1.10*cr2)) && (cg2>=(1.10*cb2))) &&
//			(cg2>=16) && (cr2<192))
//		if(((cg2>=(1.30*cr2)) && (cg2>=(1.10*cb2))) &&
//		if(((cg2>=(1.30*cr2)) && (cg2>=(1.20*cb2))) &&
//			(cg2>=16) && (cr2<192) && (cb2<192))
//		if(((cg2>=(1.40*cr2)) && (cg2>=(1.30*cb2))) &&
//			(cg2>=16) && (cr2<192) && (cb2<192))
//		if(((cg2>=(1.40*cr2)) && (cg2>=(1.40*cb2))) &&
//			(cg2>=8) && (cr2<224) && (cb2<224))
//				{ ca=0; }

//		f=2.0-cy2*(0.6/32.0);
		f=2.0-cy2*(0.6/24.0);
		if(f<1.2)f=1.2;
		if(((cg2>=(f*cr2)) && (cg2>=(f*cb2))) &&
			(cg2>=8) && (cr2<224) && (cb2<224))
		{
			ca=0;
		}
#endif

		k=(i*w+j)*4;
//		dst[k+0]=cr;		dst[k+1]=cg;
//		dst[k+2]=cb;		dst[k+3]=ca;

//		tbuf[k+0]=cr;		tbuf[k+1]=cg;
//		tbuf[k+2]=cb;		tbuf[k+3]=ca;

		tbuf[k+0]=cr2;		tbuf[k+1]=cg2;
		tbuf[k+2]=cb2;		tbuf[k+3]=ca;
	}

//	memcpy(lsbuf, src, w*h*4);
#if 1
	l=w*h*4;
	for(i=0; i<l; i++)
	{
//		lsbuf[i]=lsbuf[i]*0.75+src[i]*0.25;
//		lsbuf[i]=lsbuf[i]*0.50+src[i]*0.50;
		lsbuf[i]=lsbuf[i]*0.25+src[i]*0.75;
	}
#endif

	memcpy(dst, tbuf, w*h*4);

	for(k=0; k<4; k++)
	{
		for(i=1; i<(h-1); i++)
			for(j=1; j<(w-1); j++)
		{
			k0=((i-1)*w+(j-1))*4; k1=((i-1)*w+(j+0))*4; k2=((i-1)*w+(j+1))*4;
			k3=((i+0)*w+(j-1))*4; k4=((i+0)*w+(j+0))*4; k5=((i+0)*w+(j+1))*4;
			k6=((i+1)*w+(j-1))*4; k7=((i+1)*w+(j+0))*4; k8=((i+1)*w+(j+1))*4;

//			cy0=dst[k0+1];	cy1=dst[k1+1];	cy2=dst[k2+1];
//			cy3=dst[k3+1];	cy4=dst[k4+1];	cy5=dst[k5+1];
//			cy6=dst[k6+1];	cy7=dst[k7+1];	cy8=dst[k8+1];

			cy0=(2*dst[k0+1]+dst[k0+0]+dst[k0+2])>>2;
			cy1=(2*dst[k1+1]+dst[k1+0]+dst[k1+2])>>2;
			cy2=(2*dst[k2+1]+dst[k2+0]+dst[k2+2])>>2;
			cy3=(2*dst[k3+1]+dst[k3+0]+dst[k3+2])>>2;
			cy4=(2*dst[k4+1]+dst[k4+0]+dst[k4+2])>>2;
			cy5=(2*dst[k5+1]+dst[k5+0]+dst[k5+2])>>2;
			cy6=(2*dst[k6+1]+dst[k6+0]+dst[k6+2])>>2;
			cy7=(2*dst[k7+1]+dst[k7+0]+dst[k7+2])>>2;
			cy8=(2*dst[k8+1]+dst[k8+0]+dst[k8+2])>>2;
		
//			p0=cy1-cy7; p0=abs(p0);
//			p1=cy3-cy5; p1=abs(p1);

			p0=cy1-cy4; p0=abs(p0);
			p1=cy3-cy4; p1=abs(p1);
			p2=cy5-cy4; p2=abs(p2);
			p3=cy7-cy4; p3=abs(p3);
		
			ca0=dst[k0+3];	ca1=dst[k1+3];	ca2=dst[k2+3];
			ca3=dst[k3+3];	ca4=dst[k4+3];	ca5=dst[k5+3];
			ca6=dst[k6+3];	ca7=dst[k7+3];	ca8=dst[k8+3];

//			ca=(ca0+ca1+ca2+ca3)/4;
//			ca=(ca0+ca1+ca2+ca3+ca4+ca5+ca6+ca7)/8;
#if 1
//			if((p0>64) || (p1>64) || (p2>64) || (p3>64))
//			if((p0>16) || (p1>16) || (p2>16) || (p3>16))
			if((p0>32) || (p1>32) || (p2>32) || (p3>32))
			{
				q0=256-p0*3; if(q0<0)q0=0;
				q1=256-p1*3; if(q1<0)q1=0;
				q2=256-p2*3; if(q2<0)q2=0;
				q3=256-p3*3; if(q3<0)q3=0;
			
				l=512; ca=512*ca4;
				ca+=q0*cy1; l+=q0;
				ca+=q1*cy3; l+=q1;
				ca+=q2*cy5; l+=q2;
				ca+=q3*cy7; l+=q3;
				
				ca=ca/l;
			}else
#endif
#if 0
			if((p0>64) || (p1>64) || (p2>64) || (p3>64))
			{
				l=8; ca=8*ca4;
				if(p0<64) { ca+=4*cy1; l+=4; }
				if(p1<64) { ca+=4*cy3; l+=4; }
				if(p2<64) { ca+=4*cy5; l+=4; }
				if(p3<64) { ca+=4*cy7; l+=4; }
				
				ca=ca/l;
			}else
#endif
#if 0
			if((p0>32) || (p1>32))
			{
				if((p0>32) && (p1>32))
				{
					ca=ca4;
				}else if(p0>0)
				{
					ca=(4*ca3+8*ca4+4*ca5)/16;
				}else
				{
					ca=(4*ca1+8*ca4+4*ca7)/16;
				}
			}else
#endif
			{
				ca=(	2*ca0+4*ca1+2*ca2+
						4*ca3+8*ca4+4*ca5+
						2*ca6+4*ca7+2*ca8)/32;
			}
//			dst[k0+3]=ca;
//			tbuf[k0+3]=ca;
			tbuf[k4+3]=ca;
//			if(ca<=128)
//			{
//				dst[k0+3]=0;
//			}
		}
		
		for(i=1; i<(h-1); i++)
			for(j=1; j<(w-1); j++)
		{
			k=(i*w+j)*4;
			ca=tbuf[k+3];
			dst[k+3]=ca;
		}
	}

#if 1
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		k=(i*w+j)*4;
		ca=dst[k+3];
//		ca=(ca*ca)>>8;
		ca=bgbbtj_scurvei8(ca);
		if(ca<0)ca=0;
		if(ca>255)ca=255;
		dst[k+3]=ca;

//		if(ca<=0)
		if(ca<=16)
		{
			dst[k+0]=255;
			dst[k+1]=0;
			dst[k+2]=255;
			dst[k+3]=0;
		}
	}
#endif

	free(tbuf);
	return(0);
}

BGBBTJ_API void BGBBTJ_Tex_Dither15(byte *src, byte *dst, int w, int h)
{
	u32 s;
	int r0, r1, r2, r3;
	int ra0, ra1, ra2, ra3;
	int x, y, r, le;
	int i, j, k;

	s=0x55AA55AA;
	s=s*65521;
	s=s*65521;
	
	r3=0; r2=0; r1=0; r0=0;
	
	for(y=0; y<h; y++)
		for(x=0; x<w; x++)
	{
		r3=r2; r2=r1; r1=r0;
		r0=(s>>24)&255;
		
		ra0=(r0+r1)>>1;
		ra1=(r2+r3)>>1;
		ra2=(ra0+ra1)>>1;
//		r=(ra2+2*ra0+r0)>>2;
		r=(ra2+3*ra0-ra1+r0)>>2;
		
		r=(r*r)>>8;
//		r=(r*r*224)>>16;
		r=(r&1)?(r>>1):(-(r>>1));
//		r=(r&1)?r:(-r);
		s=s*65521;

		for(i=0; i<3; i++)
		{
			j=src[(y*w+x)*4+i];
		
//			k=(j*32+128+r)>>8;
//			if(k<0)k=0;
//			if(k>31)k=31;	
//			dst[(y*w+x)*4+i]=k<<3;

//			r=r+(le>>1);
//			r=r-(le>>1);

//			k=(j*32+128+r)>>5;
			k=(j*32+16+r)>>5;
			if(k<0)k=0;
			if(k>255)k=255;	
			dst[(y*w+x)*4+i]=k;
		
			le=j-k;
		}

		j=src[(y*w+x)*4+3];
		dst[(y*w+x)*4+3]=j;
	}
}
