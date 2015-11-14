#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
// #include <unistd.h>
#include <time.h>

//#include "libzpack.c"
#include <zpack.h>

#if 0
u32 ZPACK_FromCTime(time_t *tp)
{
	struct tm tmp;
	u32 i;

//	localtime_r(tp, &tmp);
	i=tmp.tm_sec+(tmp.tm_min*60)+(tmp.tm_hour*3600)+
		(tmp.tm_mday*86400)+(tmp.tm_mon*2678400)+
		((tmp.tm_year-80)*32140800);
	return(i);
}

time_t ZPACK_ToCTime(u32 t)
{
	struct tm tmp;

	tmp.tm_year=80+t/32140800;
	tmp.tm_mon=(t/2678400)%12;
	tmp.tm_mday=(t/86400)%31;
	tmp.tm_hour=(t/3600)%24;
	tmp.tm_min=(t/60)%60;
	tmp.tm_sec=t%60;

	return(mktime(&tmp));
}

char *ZPACK_TimeStr(u32 t)
{
	static char buf[64];

	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
		1980+t/32140800, (t/2678400)%12, (t/86400)%31,
		(t/3600)%24, (t/60)%60, t%60);
	return(buf);
}
#endif

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

void ZPACK_AddFile(ZPACK_Context *ctx, char *name)
{
	static byte buffer[16384];
	struct stat fst;
	ZPACK_STAT st;
	ZPACK_FILE *zfd;
	FILE *fd;
	char *s;
	int i;

	printf("Add: %s\n", name);

	fd=fopen(name, "rb");
	if(!fd)
	{
		printf("Fail Open: %s\n", name);
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

	i=stat(name, &fst);
	if(i>=0)
	{
		i=ZPACK_FromCTime(&fst.st_mtime);
		ZPACK_SetFileTime(ctx, name, i);
		s=ZPACK_TimeStr(i);
		printf("Time: %s\n", s);
	}

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


void ZPACK_ExtractFile(ZPACK_Context *ctx, char *name)
{
	static byte buffer[16384];
	ZPACK_FILE *zfd;
	FILE *fd;
	char *s;
	int i;

	printf("Extract: %s\n", name);

	zfd=ZPACK_OpenFile(ctx, name, "rb");
	if(!zfd)
	{
		printf("Fail Extract: %s\n", name);
		return;
	}

	fd=fopen(name, "wb");
	if(!fd)
	{
		printf("Fail Open Write: %s\n", name);
		return;
	}


	while(1)
	{
		i=ZPACK_Read(zfd, buffer, i);
		if(!i)break;
		fwrite(buffer, 1, i, fd);
	}

	fclose(fd);
	ZPACK_Close(zfd);
}

void ZPACK_ExtractFiles_R(ZPACK_Context *ctx, char *path, char **ms)
{
	char buf[1024];
	char rbuf[256];
	char vbuf[256];
	ZPACK_STAT st;
	ZPACK_DIR *dir;
	ZPACK_FILE *zfd;
	FILE *fd;
	char *s;
	int i;

	dir=ZPACK_OpenDir(ctx, path);
	if(!dir)return;

	while(1)
	{
		s=ZPACK_ReadDir(dir);
		if(!s)break;

		strcpy(buf, s);
		if(*path)sprintf(buf, "%s/%s", path, s);

		i=ZPACK_StatFile(ctx, buf, &st);
		if(i<0)continue;

		if(st.flags&2)
		{
			ZPACK_ExtractFiles_R(ctx, buf, ms);
			continue;
		}
		if(!ZPACK_MultiMatch(ms, buf))continue;

		printf("%s\n", buf);

		zfd=ZPACK_OpenFile(ctx, buf, "rb");
		fd=fopen(buf, "wb");
		if(!zfd || !fd)
		{
			if(!zfd)ZPACK_Close(zfd);
			if(!fd)fclose(fd);
			continue;
		}

		while(1)
		{
			i=ZPACK_Read(zfd, buf, 1024);
			if(!i)break;
			fwrite(buf, 1, i, fd);
		}

		fclose(fd);
		ZPACK_Close(zfd);
	}
	ZPACK_CloseDir(dir);
}

void ZPACK_ExtractFiles(ZPACK_Context *ctx, char **ms)
{
	ZPACK_ExtractFiles_R(ctx, "", ms);
}

void ZPACK_ListFiles_R(ZPACK_Context *ctx, char *path, int t)
{
	char buf[256];
	ZPACK_STAT st;
	ZPACK_DIR *dir;
	ZPACK_FILE *zfd;
	char *s;
	int i, j, k;

	dir=ZPACK_OpenDir(ctx, path);
	if(!dir)
	{
		printf("Fail Open Dir: %s\n", path);
		return;
	}

	while(1)
	{
		s=ZPACK_ReadDir(dir);
		if(!s)break;

		strcpy(buf, s);
		if(*path)sprintf(buf, "%s/%s", path, s);
		i=ZPACK_StatFile(ctx, buf, &st);

		if(i<0)
		{
			printf("? %-32s\n", s);
			continue;
		}

		if(st.flags&2)
		{
			ZPACK_ListFiles_R(ctx, buf, t);
			continue;
		}

		s="";
		if(st.method==0)s="S";
		if(st.method==8)s="Z";
		if(st.method==9)s="Z2";
		if(st.flags&1)
		{
			s="F";
			if(st.method==8)s="FZ";
			if(st.method==9)s="FZ";
		}

		if(t)
		{
			zfd=ZPACK_OpenFile(ctx, buf, "rb");
//			j = !zfd || !zfd->cache->buffer;
			j=!zfd;
			ZPACK_Close(zfd);
		}

		i=0;
		if(st.usize)i=100-((100*st.csize)/st.usize);

		if(t)
		{
			if(strlen(buf)>72)
				strcpy(buf+69, "...");

			printf("%-72s %s\n", buf, j?"Fail":"Pass");
		}else
		{
			if(strlen(buf)>40)
				strcpy(buf+37, "...");

			printf("%-40s %9d %9d %3d%% %-2s %08X %s\n", buf,
				st.usize, st.csize, i, s,
				st.crc32, t?(j?"Fail":"Pass"):"");
		}
	}
	ZPACK_CloseDir(dir);
}

void ZPACK_ListFiles(ZPACK_Context *ctx)
{
	printf(
"<name>                                     <ucize>   <csize>    %% M "
	" <CRC32>\n");
	ZPACK_ListFiles_R(ctx, "", 0);
}

void ZPACK_TestFiles(ZPACK_Context *ctx)
{
	printf(
"<name>                             <ucize>   <csize>    %% M  <CRC32>  <status>\n");
	ZPACK_ListFiles_R(ctx, "", 1);
}

void usage(int argc, char **argv)
{
	printf("Usage: %s [options] <image> [commands/files]\n", argv[0]);

	printf(
"-c	Create New Image\n"
"-l	List Contents\n"
"-t	Test Contents\n"
"-f	Dump Free Spans\n"
"-s	Dump Spans\n"
"-x	Extract Contents\n"
"-k	Destroy Central Directory\n"
);
	exit(-1);
}

int main(int argc, char **argv)
{
	static char **files;
	ZPACK_Context *ctx;
	char *img;
	int i, j, cm, nf;

	if(argc<2)usage(argc, argv);

	i=ZPACK_Init();
	if(i<0)return(-1);

	ZPACK_SetTimeFunc(&ZPACK_TimeFunc);

	files=malloc(4096*sizeof(char *));
	for(i=0; i<4096; i++)files[i]=NULL;

	cm=0; nf=0; img=NULL;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			for(j=1; argv[i][j]; j++)
				switch(argv[i][j])
			{
				case 'c': cm=1; break;
				case 'l': cm=2; break;
				case 't': cm=3; break;
				case 'f': cm=4; break;
				case 'x': cm=5; break;
				case 's': cm=6; break;
				case 'k': cm=7; break;
				default: usage(argc, argv); break;
			}
			continue;
		}
		if(!img)
		{
			img=argv[i];
			continue;
		}

		files[nf++]=argv[i];
	}
	files[nf]=NULL;

	if(cm==1)ZPACK_CreateImage(img);

	if(cm==2)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_ListFiles(ctx);
		ZPACK_CloseImage(ctx);
		return(0);
	}
	if(cm==3)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_TestFiles(ctx);
		ZPACK_CloseImage(ctx);
		return(0);
	}
	if(cm==4)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_PrintFreeSpans(ctx);
		ZPACK_CloseImage(ctx);
		return(0);
	}
	if(cm==5)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_ExtractFiles(ctx, files);
		ZPACK_CloseImage(ctx);
		return(0);
	}
	if(cm==6)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_PrintSpans(ctx);
		ZPACK_CloseImage(ctx);
		return(0);
	}

	if(cm==7)
	{
		ctx=ZPACK_OpenImage(img, 0);
		return(0);
	}

	if(!nf)return(0);


	if((cm==0) || (cm==1))
	{
		ctx=ZPACK_OpenImage(img, 0);
		for(i=0; i<nf; i++)ZPACK_AddFile(ctx, files[i]);
		ZPACK_CloseImage(ctx);
	}

	return(0);
}
