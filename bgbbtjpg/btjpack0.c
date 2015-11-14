#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include <time.h>

#include <bgbbtj.h>

#ifndef BGBBTJ_NODY

#include <zpack.h>
// #include <bgbmid.h>

static int bgbmid_fdcopy(FILE *ifd, FILE *ofd)
{
	char buf[256];
	int i;

	printf("bgbmid_fdcopy\n");

	while(!feof(ifd))
	{
		i=fread(buf, 1, 256, ifd);
		fwrite(buf, 1, i, ofd);
	}
}

int bgbmid_extwav(char *str)
{
	char *s;

	s=str+strlen(str);
	while((s>str) && (*s!='.'))s--;

	if(!strcmp(s, ".wav"))return(1);
	if(!strcmp(s, ".WAV"))return(1);
	return(0);
}

int bgbmid_extpng(char *str)
{
	char *s;

	s=str+strlen(str);
	while((s>str) && (*s!='.'))s--;

	if(!strcmp(s, ".png"))return(1);
	if(!strcmp(s, ".PNG"))return(1);
	return(0);
}

int bgbmid_extskip(char *str)
{
	char *s;

	s=str+strlen(str);
	while((s>str) && (*s!='.'))s--;

	if(!strcmp(s, ".pdn"))return(1);
	if(!strcmp(s, ".PDN"))return(1);

	if(!strcmp(s, ".xcf"))return(1);
	if(!strcmp(s, ".XCF"))return(1);

	if(!strcmp(s, ".psd"))return(1);
	if(!strcmp(s, ".PSD"))return(1);

	if(!strcmp(s, ".db"))return(1);
	if(!strcmp(s, ".DB"))return(1);

	return(0);
}

int help(char *arg0)
{
	printf("Usage: %s <opts*> -o <pack> [-L <dir>] <files*>\n", arg0);
	printf("-o <name>		Indicates output PACK fike\n");
	printf("-L <dir>		Indicates base-directory for input files\n");
	printf("--splitstereo	Store audio in split-stereo format\n");
	printf("--centermono	Store audio in center-mono format\n");
	printf("--blk256		Use 256-sample blocks (176kbps SQ)\n");
	printf("--blk256lq		Use 256-sample blocks (88kbps LQ)\n");
	printf("--blk256hq		Use 256-sample blocks (352kbps HQ)\n");
	return(0);
}

#if 1

u32 ZPACK_FromCTime(time_t *tp)
{
	struct tm *tmp;
	u32 i;

	tmp=localtime(tp);
	i=(tmp->tm_sec/2)+(tmp->tm_min*32)+(tmp->tm_hour*2048)+
		(tmp->tm_mday*65536)+(tmp->tm_mon*2097152)+
		((tmp->tm_year-80)*33554432);
	return(i);
}

time_t ZPACK_ToCTime(u32 t)
{
	struct tm tmp;

	tmp.tm_year=80+t/33554432;
	tmp.tm_mon=(t/2097152)%12;
	tmp.tm_mday=(t/65536)%31;
	tmp.tm_hour=(t/2048)%24;
	tmp.tm_min=(t/32)%60;
	tmp.tm_sec=(t*2)%60;

	return(mktime(&tmp));
}

char *ZPACK_TimeStr(u32 t)
{
	static char buf[64];

	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
		1980+t/33554432, (t/2097152)%12, (t/65536)%31,
		(t/2048)%24, (t/32)%60, (t*2)%60);
	return(buf);
}

u32 ZPACK_TimeFunc()
{
	time_t t;
	u32 i;

	t=time(NULL);
	i=ZPACK_FromCTime(&t);
	return(i);
}

#endif

void ZPACK_AddFile(ZPACK_Context *ctx, char *name, char *fname)
{
	static byte buffer[16384];
//	struct stat fst;
	ZPACK_STAT st;
	ZPACK_FILE *zfd;
	FILE *fd;
	char *s;
	int i;

	printf("Add: %s\n", name);

//	return;

	fd=fopen(fname, "rb");
	if(!fd)
	{
		printf("Fail Open: %s\n", fname);
		return;
	}

	zfd=ZPACK_OpenFile(ctx, name, "wb");
	if(!zfd)
	{
		printf("Fail Add: %s\n", name);
		return;
	}

	while(!feof(fd))
	{
		i=fread(buffer, 1, 16384, fd);
		ZPACK_Write(zfd, buffer, i);
	}

	fclose(fd);
	ZPACK_Close(zfd);

//	i=stat(name, &fst);
//	if(i>=0)
//	{
//		i=ZPACK_FromCTime(&fst.st_mtime);
//		ZPACK_SetFileTime(ctx, name, i);
//		s=ZPACK_TimeStr(i);
//		printf("Time: %s\n", s);
//	}

	i=ZPACK_StatFile(ctx, name, &st);
	if(i<0)
	{
		printf("Failed to Add: %s\n", name);
		return;
	}

	s="unknown";
	if(st.method==0)s="stored";
	if(st.method==8)s="deflated";
	if(st.method==9)s="deflated(64k)";
	printf("%s %d->%d (%d%%) %08X @%08X..%08X\n",
		s, st.usize, st.csize, 100-((100*st.csize)/st.usize),
		st.crc32,
		st.offset, st.offset+st.csize);
}

void BTJPACK_AddPNG(ZPACK_Context *ctx, char *name, char *fname, int qf)
{
	char tb[256], tb2[256];
//	struct stat fst;
	ZPACK_STAT st;
	ZPACK_FILE *zfd;
	byte *buf, *ibuf;
	char *s;
	int i, j, k, l, n;
	int w, h;

	strcpy(tb, name);
	strcpy(tb2, fname);

	ibuf=BGBBTJ_PNG_LoadRaw(tb2, &w, &h);
	
	if(!ibuf)
	{
		printf("BTJPACK_AddPNG: Fail Load %s\n", tb2);
		return;
	}
	
	if((w&15) || (h&15))
	{
		BGBBTJ_PNG_Free(ibuf);

		ZPACK_AddFile(ctx, name, fname);
		return;
	}
	
	buf=malloc(1<<24);

	k=BGBBTJ_JPG_EncodeComponent(
		ibuf, NULL, NULL, NULL,
		buf, w, h, qf); //90
	BGBBTJ_PNG_Free(ibuf);

	s=tb+strlen(tb);
	while((s>tb) && (*s!='.'))s--;
	strcpy(s, ".jpg");

	zfd=ZPACK_OpenFile(ctx, tb, "wb");
	ZPACK_Write(zfd, buf, k);
	ZPACK_Close(zfd);

	free(buf);

//	j=ftell(ofd);
//	fwrite(buf, 1, k, ofd);

//	i=stat(tb, &fst);
//	if(i>=0)
//	{
//		i=ZPACK_FromCTime(&fst.st_mtime);
//		ZPACK_SetFileTime(ctx, tb, i);
//		s=ZPACK_TimeStr(i);
//		printf("Time: %s\n", s);
//	}

	i=ZPACK_StatFile(ctx, tb, &st);
	if(i<0)
	{
		printf("Failed to Add: %s\n", tb);
		return;
	}

	s="unknown";
	if(st.method==0)s="stored";
	if(st.method==8)s="deflated";
	if(st.method==9)s="deflated(64k)";
	printf("%s %d->%d (%d%%) %08X @%08X..%08X\n",
		s, st.usize, st.csize, 100-((100*st.csize)/st.usize),
		st.crc32,
		st.offset, st.offset+st.csize);
}

int main(int argc, char *argv[])
{
	char *ifn[16384];
	char tb[256], tb2[256];
//	BGBMID_PackHead hdr;
//	BGBMID_PackDirEnt *dir;
	ZPACK_Context *zpctx;
	ZPACK_FILE *zfd;
	FILE *ofd, *ifd;
	byte *buf, *ibuf;
	char *ofn, *lifn;
	char *ldir;
	short *bp;
	char **a;
	char *s;
	int i, j, k, l, n;
	int ch, rt, len, flags, fl, nifn, cmd;
	int w, h, qf, qfl;

	ofn=NULL; ldir=NULL; lifn=NULL;
	nifn=0; flags=0; cmd=0; qf=90; qfl=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
			{
				cmd=8;
				continue;
			}

			if(!strcmp(argv[i], "-o"))
			{
				ofn=argv[i+1];
				i++;
				continue;
			}

			if(!strcmp(argv[i], "-L"))
			{
				ldir=argv[i+1];
				i++;
				continue;
			}

			if(!strcmp(argv[i], "-l"))
			{
				lifn=argv[i+1];
				i++;
				continue;
			}

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

#if 0
			if(!strcmp(argv[i], "--splitstereo"))
			{
				flags|=BGBMID_BTAC_EFL_HALFMONO;
				continue;
			}

			if(!strcmp(argv[i], "--centermono"))
			{
				flags|=BGBMID_BTAC_EFL_CENTERMONO;
				continue;
			}
#endif

			if(!strcmp(argv[i], "--encode"))
			{
				cmd=1;
				continue;
			}

			if(!strcmp(argv[i], "--decode"))
			{
				cmd=2;
				continue;
			}

#if 0
			if(!strcmp(argv[i], "--blk256"))
			{
				flags|=BGBMID_BTAC_EBMODE_BLK256;
				continue;
			}

			if(!strcmp(argv[i], "--blk256lq"))
			{
				flags|=BGBMID_BTAC_EBMODE_BLK256LQ;
				continue;
			}

			if(!strcmp(argv[i], "--blk256hq"))
			{
				flags|=BGBMID_BTAC_EBMODE_BLK256HQ;
				continue;
			}
#endif

			printf("%s: unrecognized argument '%s'\n", argv[0], argv[i]);
			continue;
		}else
		{
			ifn[nifn++]=argv[i];
		}
	}

	qf|=qfl;

	if(!ofn || (cmd==8))
	{
		help(argv[0]);
		return(0);
	}

	if(lifn)
	{
		ifd=fopen(lifn, "rt");
		if(ifd)
		{
			while(!feof(ifd))
			{
				fgets(tb, 255, ifd);
				a=gcrsplit(tb);
				if(!a[0])continue;
				if(*a[0]=='#')continue;
				if(*a[0]=='/')continue;
				ifn[nifn++]=a[0];
			}
			fclose(ifd);
		}
	}

	i=ZPACK_Init();
	if(i<0)return(-1);

	ZPACK_SetTimeFunc(&ZPACK_TimeFunc);

//	ofd=fopen(ofn, "wb");
	ZPACK_CreateImage(ofn);
	zpctx=ZPACK_OpenImage(ofn, 0);

//	fwrite(&hdr, 1, 12, ofd);
//	dir=malloc((1<<16)*sizeof(BGBMID_PackDirEnt));

	buf=malloc(1<<24);

	n=0;
	for(i=0; i<nifn; i++)
	{
		if(ldir)
		{
			sprintf(tb2, "%s/%s", ldir, ifn[i]);
		}else
		{
			strcpy(tb2, ifn[i]);
		}
	
//		strcpy(tb, argv[i]);
		strcpy(tb, ifn[i]);

		if(bgbmid_extskip(tb2))
			{ continue; }

		if(bgbmid_extpng(tb2))
		{
			BTJPACK_AddPNG(zpctx, tb, tb2, qf);
		}else
#if 0
		if(bgbmid_extwav(tb2))
		{
			printf("WAV %s\n", tb2);

			s=tb+strlen(tb);
			while((s>tb) && (*s!='.'))s--;
			strcpy(s, ".btac");

//			bp=BGBMID_LoadWAV_Mono16(ifn[i], &rt, &len);
			bp=BGBMID_LoadWAV16(tb2, &ch, &rt, &len);
			if(!bp)continue;
			
			fl=flags;
			
//			k=SGWZ_Encode(bp, rt, len, buf);
			k=BGBMID_BTAC_Encode(bp, ch, rt, len, fl, buf, 1<<24);

			j=ftell(ofd);
			fwrite(buf, 1, k, ofd);

			bgbmid_free(bp);
		}else
#endif
		{
			ZPACK_AddFile(zpctx, tb, tb2);
			
#if 0
			printf("RAW %s\n", tb2);
			ifd=fopen(tb2, "rb");
			if(!ifd)continue;

			j=ftell(ofd);
			bgbmid_fdcopy(ifd, ofd);
			k=ftell(ofd)-j;

			fclose(ifd);
#endif
		}

#if 0
		if(strlen(tb)>=56)
		{
			l=ftell(ofd);
			fwrite(tb, 1, strlen(tb)+1, ofd);
			sprintf(tb, "@@0x%08X", l);
		}

		strcpy(dir[n].name, tb);
		*(int *)(dir[n].offs)=j;
		*(int *)(dir[n].size)=k;
		n++;

		printf("%-56s %10d %10d\n", tb, j, k);
#endif
	}

#if 0
	j=ftell(ofd);
	fwrite(dir, 64, n, ofd);

	*(int *)(hdr.offs)=j;
	*(int *)(hdr.ents)=n;

	hdr.fcc[0]='P';
	hdr.fcc[1]='A';
	hdr.fcc[2]='C';
	hdr.fcc[3]='K';

	fseek(ofd, 0, 0);
	fwrite(&hdr, 1, 12, ofd);
	fclose(ofd);
#endif

	ZPACK_CloseImage(zpctx);
}

#else
int main(int argc, char *argv[])
{
	printf("Tool not available with NODY build\n");
}
#endif
