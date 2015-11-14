#include <walpng.h>


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

float Gen_Clamp(float v, float m, float n)
{
	if(v<m)return(m);
	if(v>n)return(n);
	return(v);
}

int Gen_CalcPoint(float x, float y, float *rgba)
{
	float f, g, h;
	
	f=(x*2)-1;
	g=(y*2)-1;
//	h=1.0-sqrt(f*f+g*g);
//	h=1.0-fabs(f)-fabs(g);

//	h=(1.0-fabs(f))+(1.0-fabs(g));
//	h=fabs(f)*fabs(g);

//	h=fmax((1.0-fabs(f)), (1.0-fabs(g)));
//	h=fmin((1.0-fabs(f)), (1.0-fabs(g)));
//	h=sqrt(3)*fmin((1.0-fabs(f)), (1.0-fabs(g)));

//	h=fmin(pow(1.0-fabs(f), 0.5), pow(1.0-fabs(g), 0.5));
	h=fmin((1.0-pow(fabs(f), 2)), (1.0-pow(fabs(g), 2)));

	rgba[0]=h;
	rgba[1]=h;
	rgba[2]=h;
	rgba[3]=1;
}

int main(int argc, char *argv[])
{
	byte tb[1024];
	float rgba[4];
	byte *buf;
//	WalInfo_t *wal;
	byte *base, *chain, *s;
	char *fn;
	FILE *fd;
	int i, j, w, h;

	w=256; h=256; fn=NULL;
	for(i=1; i<argc; i++)
	{
		if(!strcmp(argv[i], "-xs"))
		{
			w=atoi(argv[i+1]);
			i++; continue;
		}
		if(!strcmp(argv[i], "-ys"))
		{
			h=atoi(argv[i+1]);
			i++; continue;
		}
		if(!strcmp(argv[i], "-sz"))
		{
			w=atoi(argv[i+1]); h=w;
			i++; continue;
		}
		
		if(argv[i][0]=='-')
			continue;
			
		fn=argv[i];
	}

	buf=malloc(w*h*4);
	
	for(i=0; i<h; i++)
		for(j=0; j<w; j++)
	{
		Gen_CalcPoint(j/(1.0*w), i/(1.0*h), rgba);
		rgba[0]=Gen_Clamp(rgba[0], 0, 1);
		rgba[1]=Gen_Clamp(rgba[1], 0, 1);
		rgba[2]=Gen_Clamp(rgba[2], 0, 1);
		rgba[3]=Gen_Clamp(rgba[3], 0, 1);
		buf[(i*w+j)*4+0]=rgba[0]*255;
		buf[(i*w+j)*4+1]=rgba[1]*255;
		buf[(i*w+j)*4+2]=rgba[2]*255;
		buf[(i*w+j)*4+3]=rgba[3]*255;
	}

	PDPNG_SaveRaw(fn, buf, w, h);

#if 0
	sprintf(tb, "%s.tga", base);
	buf=PDTGA_LoadRaw(tb, &w, &h);

	sprintf(tb, "%s_alt.png", base);
	PDPNG_SaveRaw(tb, buf, w, h);
#endif

#if 0
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
