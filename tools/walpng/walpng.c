#include <walpng.h>

byte q2pal[768]=
{
  0,   0,   0,   15,  15,  15,   31,  31,  31,   47,  47,  47,
 63,  63,  63,   75,  75,  75,   91,  91,  91,  107, 107, 107,
123, 123, 123,  139, 139, 139,  155, 155, 155,  171, 171, 171,
187, 187, 187,  203, 203, 203,  219, 219, 219,  235, 235, 235,
 99,  75,  35,   91,  67,  31,   83,  63,  31,   79,  59,  27,
 71,  55,  27,   63,  47,  23,   59,  43,  23,   51,  39,  19,
 47,  35,  19,   43,  31,  19,   39,  27,  15,   35,  23,  15,
 27,  19,  11,   23,  15,  11,   19,  15,   7,   15,  11,   7,
 95,  95, 111,   91,  91, 103,   91,  83,  95,   87,  79,  91,
 83,  75,  83,   79,  71,  75,   71,  63,  67,   63,  59,  59,
 59,  55,  55,   51,  47,  47,   47,  43,  43,   39,  39,  39,
 35,  35,  35,   27,  27,  27,   23,  23,  23,   19,  19,  19,
143, 119,  83,  123,  99,  67,  115,  91,  59,  103,  79,  47,
207, 151,  75,  167, 123,  59,  139, 103,  47,  111,  83,  39,
235, 159,  39,  203, 139,  35,  175, 119,  31,  147,  99,  27,
119,  79,  23,   91,  59,  15,   63,  39,  11,   35,  23,   7,
167,  59,  43,  159,  47,  35,  151,  43,  27,  139,  39,  19,
127,  31,  15,  115,  23,  11,  103,  23,   7,   87,  19,   0,
 75,  15,   0,   67,  15,   0,   59,  15,   0,   51,  11,   0,
 43,  11,   0,   35,  11,   0,   27,   7,   0,   19,   7,   0,
123,  95,  75,  115,  87,  67,  107,  83,  63,  103,  79,  59,
 95,  71,  55,   87,  67,  51,   83,  63,  47,   75,  55,  43,
 67,  51,  39,   63,  47,  35,   55,  39,  27,   47,  35,  23,
 39,  27,  19,   31,  23,  15,   23,  15,  11,   15,  11,   7,
111,  59,  23,   95,  55,  23,   83,  47,  23,   67,  43,  23,
 55,  35,  19,   39,  27,  15,   27,  19,  11,   15,  11,   7,
179,  91,  79,  191, 123, 111,  203, 155, 147,  215, 187, 183,
203, 215, 223,  179, 199, 211,  159, 183, 195,  135, 167, 183,
115, 151, 167,   91, 135, 155,   71, 119, 139,   47, 103, 127,
 23,  83, 111,   19,  75, 103,   15,  67,  91,   11,  63,  83,
  7,  55,  75,    7,  47,  63,    7,  39,  51,    0,  31,  43,
  0,  23,  31,    0,  15,  19,    0,   7,  11,    0,   0,   0,
139,  87,  87,  131,  79,  79,  123,  71,  71,  115,  67,  67,
107,  59,  59,   99,  51,  51,   91,  47,  47,   87,  43,  43,
 75,  35,  35,   63,  31,  31,   51,  27,  27,   43,  19,  19,
 31,  15,  15,   19,  11,  11,   11,   7,   7,    0,   0,   0,
151, 159, 123,  143, 151, 115,  135, 139, 107,  127, 131,  99,
119, 123,  95,  115, 115,  87,  107, 107,  79,   99,  99,  71,
 91,  91,  67,   79,  79,  59,   67,  67,  51,   55,  55,  43,
 47,  47,  35,   35,  35,  27,   23,  23,  19,   15,  15,  11,
159,  75,  63,  147,  67,  55,  139,  59,  47,  127,  55,  39,
119,  47,  35,  107,  43,  27,   99,  35,  23,   87,  31,  19,
 79,  27,  15,   67,  23,  11,   55,  19,  11,   43,  15,   7,
 31,  11,   7,   23,   7,   0,   11,   0,   0,    0,   0,   0,
119, 123, 207,  111, 115, 195,  103, 107, 183,   99,  99, 167,
 91,  91, 155,   83,  87, 143,   75,  79, 127,   71,  71, 115,
 63,  63, 103,   55,  55,  87,   47,  47,  75,   39,  39,  63,
 35,  31,  47,   27,  23,  35,   19,  15,  23,   11,   7,   7,
155, 171, 123,  143, 159, 111,  135, 151,  99,  123, 139,  87,
115, 131,  75,  103, 119,  67,   95, 111,  59,   87, 103,  51,
 75,  91,  39,   63,  79,  27,   55,  67,  19,   47,  59,  11,
 35,  47,   7,   27,  35,   0,   19,  23,   0,   11,  15,   0,
  0, 255,   0,   35, 231,  15,   63, 211,  27,   83, 187,  39,
 95, 167,  47,   95, 143,  51,   95, 123,  51,  255, 255, 255,
255, 255, 211,  255, 255, 167,  255, 255, 127,  255, 255,  83,
255, 255,  39,  255, 235,  31,  255, 215,  23,  255, 191,  15,
255, 171,   7,  255, 147,   0,  239, 127,   0,  227, 107,   0,
211,  87,   0,  199,  71,   0,  183,  59,   0,  171,  43,   0,
155,  31,   0,  143,  23,   0,  127,  15,   0,  115,   7,   0,
 95,   0,   0,   71,   0,   0,   47,   0,   0,   27,   0,   0,
239,   0,   0,   55,  55, 255,  255,   0,   0,    0,   0, 255,
 43,  43,  35,   27,  27,  23,   19,  19,  15,  235, 151, 127,
195, 115,  83,  159,  87,  51,  123,  63,  27,  235, 211, 199,
199, 171, 155,  167, 139, 119,  135, 107,  87,  159,  91,  83,
};


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

int main(int argc, char *argv[])
{
	byte tb[1024];
	byte *buf;
	WalInfo_t *wal;
	byte *base, *chain, *s;
	FILE *fd;
	int i, j, w, h;

#if 0
	PCX_LoadRaw("colormap.pcx", NULL, NULL, tb);

	for(i=0; i<64; i++)
	{
		for(j=0; j<4; j++)
			printf("%3d, %3d, %3d,  ",
				tb[(i*4+j)*3+0],
				tb[(i*4+j)*3+1],
				tb[(i*4+j)*3+2]);
		printf("\n");
	}
#endif

	strcpy(tb, argv[1]);
	base=tb+strlen((char *)tb);
	while(base>tb)
	{
		if(*base=='.')
			{ *base=0; break; }
		base--;
	}
	base=strdup(tb);

	s=base+strlen(base)-strlen("_alt");
	if(!strcmp(s, "_alt"))return(0);

	s=base+strlen(base)-strlen("_luma");
	if(!strcmp(s, "_luma"))return(0);

	printf("Base=%s\n", base);

	chain=WAL_AnimChain(base);
	if(chain)
		printf("Chain=%s\n", chain);

#if 0
	sprintf(tb, "%s.tga", base);
	buf=PDTGA_LoadRaw(tb, &w, &h);

	sprintf(tb, "%s_alt.png", base);
	PDPNG_SaveRaw(tb, buf, w, h);
#endif

#if 1
	sprintf(tb, "%s.png", base);
	buf=PDPNG_LoadRaw(tb, &w, &h);

	if(!buf)return(-1);

	while((w>256) || (h>256))
	{
		WAL_HalfSample(buf, w, h);
		w=w>>1; h=h>>1;
	}

	wal=WAL_CreateWal(buf, w, h, q2pal);
	wal->name=strdup(base);
	if(chain)
		wal->animname=strdup(chain);

	sprintf(tb, "%s.txt", base);
	WAL_ReadInfo(tb, wal);

//	printf("%dx%d\t%08X %08X\n",
//		wal->width, wal->height,
//		wal->flags, wal->contents);

	sprintf(tb, "%s.wal", base);
	WAL_WriteFile(tb, wal);


//	sprintf(tb, "%s.q1wal", base);
//	WAL_WriteFileQ1(tb, wal);
#endif

	return(0);
}
