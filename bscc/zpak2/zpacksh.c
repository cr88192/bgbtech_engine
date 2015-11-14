#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <errno.h>

#include <dirent.h>
#include <time.h>

// #define USE_READLINE

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif


//#include "libzpack.c"
#include <zpack.h>

char zpsh_rpwd[256], zpsh_vpwd[256];
ZPACK_Context *zpsh_ctx;

static void *kralloc_buf=NULL;
static int kralloc_pos=0;

void *kralloc(int sz)
{
	void *p;
	if(!kralloc_buf)kralloc_buf=malloc(262144);

	if((kralloc_pos+sz)>262144)
		kralloc_pos=0;
	p=kralloc_buf+kralloc_pos;
	kralloc_pos=(kralloc_pos+sz+3)&(~3);

	return(p);
}

void *kalloc(int sz)
{
	void *p;
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

char **ksplit(char *s)
{
	char **a, *t;
	int i;

	a=kralloc(128*sizeof(char *));
	memset(a, 0, 128*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=kralloc(256);
		a[i++]=t;

		if(*s=='"')
		{
			s++;
			while(*s && (*s!='"'))*t++=*s++;
			if(*s=='"')s++;

			*t++=0;
			continue;
		}

		while(*s && (*s>' '))
		{
			if((s[0]=='\\') && (s[1]==' '))s++;
			*t++=*s++;
		}
		*t++=0;
	}
	a[i]=NULL;

	return(a);
}


u32 ZPACK_FromCTime(time_t *tp)
{
	struct tm *tmp;
	u32 i;

//	localtime_r(tp, &tmp);
//	i=tmp.tm_sec+(tmp.tm_min*60)+(tmp.tm_hour*3600)+
//		(tmp.tm_mday*86400)+(tmp.tm_mon*2678400)+
//		((tmp.tm_year-80)*32140800);

	tmp=localtime(tp);
	i=(tmp->tm_sec/2)+(tmp->tm_min*32)+(tmp->tm_hour*2048)+
		(tmp->tm_mday*65536)+(tmp->tm_mon*2097152)+
		((tmp->tm_year-80)*33554432);
	return(i);
}

time_t ZPACK_ToCTime(u32 t)
{
	struct tm tmp;

//	tmp.tm_year=80+t/32140800;
//	tmp.tm_mon=(t/2678400)%12;
//	tmp.tm_mday=(t/86400)%31;
//	tmp.tm_hour=(t/3600)%24;
//	tmp.tm_min=(t/60)%60;
//	tmp.tm_sec=t%60;

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
	char *b;

	b=kralloc(64);
	sprintf(b, "%04d-%02d-%02d %02d:%02d:%02d",
		1980+t/32140800, (t/2678400)%12, (t/86400)%31,
		(t/3600)%24, (t/60)%60, t%60);
	return(b);
}

u32 ZPACK_TimeFunc()
{
	time_t t;
	u32 i;

	t=time(NULL);
	i=ZPACK_FromCTime(&t);
	return(i);
}

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

	zfd=ZPACK_OpenFile(ctx, name, "wb");
	if(!zfd)
	{
		printf("Fail Add: %s\n", name);
		return;
	}

	fd=fopen(name, "rb");
	if(!fd)
	{
		printf("Fail Open: %s\n", name);
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

void usage(int argc, char **argv)
{
	printf("Usage: %s <image>\n", argv[0]);

	printf(
"-c	Create New Image\n"
//"-l	List Contents\n"
//"-t	Test Contents\n"
//"-f	Dump Spans\n"
//"-x	Extract Contents\n"
);
	exit(-1);
}

void ZPSH_CD(char **a)
{
	char buf[256];
	ZPACK_STAT stat;
	char *s;
	int i;

	if(!a[1])return;

	if(a[1][0]=='/')
	{
		strcpy(buf, a[1]);
	}else if(strcmp(zpsh_vpwd, "/"))
	{
		sprintf(buf, "%s/%s", zpsh_vpwd, a[1]);
	}else sprintf(buf, "/%s", a[1]);

	//rip off any terminal slashes
	s=buf+strlen(buf)-1;
	while((s>buf) && (*s=='/')) { *s--==0; }

	if(strcmp(buf, "/"))
	{
		i=ZPACK_StatFile(zpsh_ctx, buf, &stat);
		if(i<0)
		{
			printf("%s: No such directory\n", buf);
			return;
		}
		if(!(stat.flags&2))
		{
			printf("%s: Not a directory\n", buf);
			return;
		}
	}

	strcpy(zpsh_vpwd, buf);
}

void ZPSH_LS(char **a)
{
	char buf[256];
	ZPACK_STAT st;
	ZPACK_DIR *dir;
	char *s;
	int i;

	strcpy(buf, zpsh_vpwd);
	if(a[1])
	{
		if(a[1][0]=='/')strcpy(buf, a[1]);
			else sprintf(buf, "%s/%s", zpsh_vpwd, a[1]);
	}

	dir=ZPACK_OpenDir(zpsh_ctx, buf);
	if(!dir)
	{
		printf("Fail Open Dir: %s\n", zpsh_vpwd);
		return;
	}

	printf(
"<name>                             <ucize>   <csize>    %% <CRC32>\n");

	while(1)
	{
		s=ZPACK_ReadDir(dir);
		if(!s)break;

		sprintf(buf, "%s/%s", zpsh_vpwd, s);
		i=ZPACK_StatFile(zpsh_ctx, buf, &st);

		if(i<0)
		{
			printf("? %-32s\n", s);
			continue;
		}

		strcpy(buf, s);
		if(st.flags&2)sprintf(buf, "%s/", s);

		i=0;
		if(st.usize)i=100-((100*st.csize)/st.usize);


		printf("%-32s %9d %9d %3d%% %08X\n", buf,
			st.usize, st.csize, i,
			st.crc32);
	}
	ZPACK_CloseDir(dir);
}

void ZPSH_LLS(char **a)
{
	char buf[256];
	struct stat st;
	DIR *dir;
	struct dirent *de;
	char *s;
	int i;

	strcpy(buf, zpsh_rpwd);
	if(a[1])
	{
		if(a[1][0]=='/')strcpy(buf, a[1]);
			else sprintf(buf, "%s/%s", zpsh_rpwd, a[1]);
	}

	dir=opendir(buf);
	if(!dir)
	{
		printf("Fail Open Dir: %s\n", zpsh_rpwd);
		return;
	}

	printf(
"<name>                             <ucize>   <csize>   %% <CRC32>\n");

	while(1)
	{
		de=readdir(dir);
		if(!de)break;
		s=de->d_name;

		sprintf(buf, "%s/%s", zpsh_rpwd, s);
		i=stat(buf, &st);

		if(i<0)
		{
			printf("? %-32s\n", s);
			continue;
		}

		strcpy(buf, s);
		if(S_ISDIR(st.st_mode))sprintf(buf, "%s/", s);

		printf("%-32s %9d\n", buf,
			st.st_size);
	}
	closedir(dir);
}

void ZPSH_MkDir(char **a)
{
	char buf[256];
	int i;

	sprintf(buf, "%s/%s", zpsh_vpwd, a[1]);
	i=ZPACK_MkDir(zpsh_ctx, buf);
	if(i<0)
	{
		printf("Failed to create directory '%s'\n", a[1]);
	}
}

void ZPSH_LMkDir(char **a)
{
	char buf[256];
	int i;

	sprintf(buf, "%s/%s", zpsh_rpwd, a[1]);

#ifndef WIN32
	i=mkdir(buf, 0);
#else
	i=mkdir(buf);
#endif

	if(i<0)
	{
		printf("Failed to create directory '%s'\n", a[1]);
	}
}

void ZPSH_Import_R(char *path, char *pat)
{
	char buf[1024];
	char rbuf[256];
	char vbuf[256];
	struct stat st;
	DIR *dir;
	struct dirent *de;
	ZPACK_FILE *zfd;
	FILE *fd;
	char *s;
	int i;

	sprintf(buf, "%s/%s", zpsh_rpwd, path);
	dir=opendir(buf);
	if(!dir)return;

	while(1)
	{
		de=readdir(dir);
		if(!de)break;
		s=de->d_name;

		if(!strcmp(s, "."))continue;
		if(!strcmp(s, ".."))continue;

		strcpy(buf, s);
		if(*path)sprintf(buf, "%s/%s", path, s);

		sprintf(rbuf, "%s/%s", zpsh_rpwd, buf);
		sprintf(vbuf, "%s/%s", zpsh_vpwd, buf);

//		printf("try %s\n", buf);

		i=stat(rbuf, &st);
		if(i<0)continue;

		if(S_ISDIR(st.st_mode))
		{
			ZPSH_Import_R(buf, pat);
			continue;
		}
		if(!ZPACK_Match2(pat, buf))continue;

		printf("%s\n", buf);

		zfd=ZPACK_OpenFile(zpsh_ctx, vbuf, "wb");
		fd=fopen(rbuf, "rb");
		if(!zfd || !fd)
		{
			if(zfd)ZPACK_Close(zfd);
			if(fd)fclose(fd);
			continue;
		}

		while(!feof(fd))
		{
			i=fread(buf, 1, 1024, fd);
			ZPACK_Write(zfd, buf, i);
		}

		fclose(fd);
		ZPACK_Close(zfd);

		i=ZPACK_FromCTime(&st.st_mtime);
		ZPACK_SetFileTime(zpsh_ctx, vbuf, i);
	}
	closedir(dir);
}

void ZPSH_Export_R(char *path, char *pat)
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

	sprintf(buf, "%s/%s", zpsh_vpwd, path);
	dir=ZPACK_OpenDir(zpsh_ctx, buf);
	if(!dir)return;

	while(1)
	{
		s=ZPACK_ReadDir(dir);
		if(!s)break;

		strcpy(buf, s);
		if(*path)sprintf(buf, "%s/%s", path, s);

		sprintf(rbuf, "%s/%s", zpsh_rpwd, buf);
		sprintf(vbuf, "%s/%s", zpsh_vpwd, buf);

		i=ZPACK_StatFile(zpsh_ctx, vbuf, &st);
		if(i<0)continue;

		if(st.flags&2)
		{
			ZPSH_Export_R(buf, pat);
			continue;
		}
		if(!ZPACK_Match2(pat, buf))continue;

		printf("%s\n", buf);

		zfd=ZPACK_OpenFile(zpsh_ctx, vbuf, "rb");
		fd=fopen(rbuf, "wb");
		if(!zfd || !fd)
		{
			if(zfd)ZPACK_Close(zfd);
			if(fd)fclose(fd);
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

void ZPSH_Import(char **a)
{
	int i;
	for(i=1; a[i]; i++)ZPSH_Import_R("", a[i]);
}

void ZPSH_Export(char **a)
{
	int i;
	for(i=1; a[i]; i++)ZPSH_Export_R("", a[i]);
}

void ZPSH_Delete_R(char *path, char *pat)
{
	char buf[1024];
	char vbuf[256];
	ZPACK_STAT st;
	ZPACK_DIR *dir;
	char *s;
	int i;

	sprintf(buf, "%s/%s", zpsh_vpwd, path);
	dir=ZPACK_OpenDir(zpsh_ctx, buf);
	if(!dir)return;

	while(1)
	{
		s=ZPACK_ReadDir(dir);
		if(!s)break;

		strcpy(buf, s);
		if(*path)sprintf(buf, "%s/%s", path, s);
		sprintf(vbuf, "%s/%s", zpsh_vpwd, buf);

		i=ZPACK_StatFile(zpsh_ctx, vbuf, &st);
		if(i<0)continue;

		if(st.flags&2)
		{
			ZPSH_Delete_R(buf, pat);
			continue;
		}
		if(!ZPACK_Match2(pat, buf))continue;

		printf("%s\n", buf);

		ZPACK_DeleteFile(zpsh_ctx, buf);
	}
	ZPACK_CloseDir(dir);
}

void ZPSH_Delete(char **a)
{
	int i;
	for(i=1; a[i]; i++)ZPSH_Delete_R("", a[i]);
}

void ZPSH_Help(char **a)
{
	printf(
"quit			Quit\n"
"cd <path>		Change current directory\n"
"lcd <path>		Change current directory (host fs)\n"
"ls [path]		List directory\n"
"lls [path]		List directory (host fs)\n"
"mkdir <path>		Create directory\n"
"lmkdir <path>		Create directory (host fs)\n"
"import <pattern*>	Import files matching patterns\n"
"export <pattern*>	Export files matching patterns\n"
"rm <pattern*>		Delete files matching patterns\n"
);

	printf("\n");
	printf(
"Pattern Rules:\n"
"Any non-special characters must match exactly\n"
"? Will match a single character\n"
"* Will match zero or more characters but will not cross '/'\n"
"** Will match zero or more characters and will cross '/'\n"
"\\ Will allow treating a special character as a non-special one\n"
"Examples:\n"
"	*.txt will match all text files in the current directory\n"
"	*/*.txt will match all text files in a direct sub directory\n"
"	**.txt will match all text files in the current and sub-directories\n"
"	foo\\*bar.txt will match the file 'foo*bar.txt'\n"
"	...\n"
);
	printf("\n\n");
}

int main(int argc, char **argv)
{
	char buf[256];

	static char **files;
	ZPACK_Context *ctx;
	ZPACK_FILE *zfd;
	FILE *fd;
	char *img, *s, **a;
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
//				case 'l': cm=2; break;
//				case 't': cm=3; break;
//				case 'f': cm=4; break;
//				case 'x': cm=5; break;
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

#if 0
	if(cm==2)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_ListFiles(ctx);
		return(0);
	}
	if(cm==3)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_TestFiles(ctx);
		return(0);
	}
	if(cm==4)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_PrintFreeSpans(ctx);
		return(0);
	}
	if(cm==5)
	{
		ctx=ZPACK_OpenImage(img, 1);
		ZPACK_ExtractFiles(ctx, files);
		return(0);
	}
#endif

	if((cm!=0) && (cm!=1))
		usage(argc, argv);

	zpsh_ctx=ZPACK_OpenImage(img, 0);
	for(i=0; i<nf; i++)ZPACK_AddFile(zpsh_ctx, files[i]);

	getcwd(zpsh_rpwd, 256);
	strcpy(zpsh_vpwd, "/");
	while(1)
	{
#ifdef USE_READLINE
		sprintf(buf, "%s:%s> ", zpsh_rpwd, zpsh_vpwd);
		s=readline(buf);
#else
		printf("%s:%s> ", zpsh_rpwd, zpsh_vpwd);
		fgets(buf, 256, stdin);
		s=strdup(buf);
//		s=readline(buf);
#endif
		a=ksplit(s);

		if(!a[0])continue;
		if(!strcmp(a[0], "quit"))break;

#ifdef USE_READLINE
		add_history(s);
		free(s);
#endif

		if(!strcmp(a[0], "cd")) { ZPSH_CD(a); continue; }
		if(!strcmp(a[0], "lcd"))
		{
			errno=0;
			i=chdir(a[1]);
			if(errno==ENOENT)
			{
				printf("%s: No such directory\n", a[1]);
				continue;
			}
			if(errno==ENOTDIR)
			{
				printf("%s: Not a directory\n", a[1]);
				continue;
			}
			getcwd(zpsh_rpwd, 256);

			continue;
		}

		if(!strcmp(a[0], "mkdir")) { ZPSH_MkDir(a); continue; }
		if(!strcmp(a[0], "md")) { ZPSH_MkDir(a); continue; }
		if(!strcmp(a[0], "lmkdir")) { ZPSH_LMkDir(a); continue; }
		if(!strcmp(a[0], "lmd")) { ZPSH_LMkDir(a); continue; }

		if(!strcmp(a[0], "ls")) { ZPSH_LS(a); continue; }
		if(!strcmp(a[0], "lls")) { ZPSH_LLS(a); continue; }

		if(!strcmp(a[0], "import")) { ZPSH_Import(a); continue; }
		if(!strcmp(a[0], "export")) { ZPSH_Export(a); continue; }
//		if(!strcmp(a[0], "rimport")) { ZPSH_RImport(a); continue; }
//		if(!strcmp(a[0], "rexport")) { ZPSH_RExport(a); continue; }

		if(!strcmp(a[0], "rm")) { ZPSH_Delete(a); continue; }

		if(!strcmp(a[0], "help")) { ZPSH_Help(a); continue; }
		if(!strcmp(a[0], "?")) { ZPSH_Help(a); continue; }

		printf("Bad Command '%s' ('?' or 'help' for help)\n", a[0]);
	}

	ZPACK_CloseImage(zpsh_ctx);
	return(0);
}
