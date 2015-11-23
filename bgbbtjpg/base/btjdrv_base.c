#include <bgbbtj.h>


#if defined(BGBBTJ_DRV) || defined(BGBBTJ_NODY)

#ifndef BGBBTJ_DRVLOGFILE
#ifdef _MSC_VER
#define BGBBTJ_DRVLOGFILE "F:\\btjdrv_log.txt"
#define BGBBTJ_DRVCFGFILE "F:\\btjdrv_conf.txt"
#endif
#ifdef __linux__
#define BGBBTJ_DRVLOGFILE "~/.btjdrv_log.txt"
#define BGBBTJ_DRVCFGFILE "~/.btjdrv_conf.txt"
#endif
#endif

FILE *btjpg_log=NULL;
char *btjpg_home;

u32 btjpg_drv_defaultCodecFcc;

int BTJPG_DriverTryLoadConfig(char *name)
{
	char tb[256];
	FILE *fd;
	char **a;

	fd=fopen(name, "rt");
	if(!fd)return(0);
	
	while(!feof(fd))
	{
		fgets(tb, 256, fd);
		
		a=bgbv86_rsplit(tb);
		if(!a[0])continue;
		if(a[0][0]=='#')continue;
		if(a[0][0]==';')continue;
		if(a[0][0]=='/')continue;
		
		if(!strcmp(a[0], "defaultCodec"))
		{
			btjpg_drv_defaultCodecFcc=RIFF_MAKETAG(
				a[1][0], a[1][1], a[1][2], a[1][3]);
			continue;
		}
	}
	
	fclose(fd);
	
	return(1);
}

void BTJPG_DriverInit(void)
{
	static int init=0;

	if(init)return;
	init=1;
	
	btjpg_log=fopen(BGBBTJ_DRVLOGFILE, "at");
	if(btjpg_log)
		{ btjpg_puts("--------\n"); }

	BTJPG_DriverTryLoadConfig(BGBBTJ_DRVCFGFILE);
}

void BTJPG_DriverDeinit(void)
{
	if(btjpg_log)
	{
		fclose(btjpg_log);
		btjpg_log=NULL;
	}
}

void btjpg_puts(char *str)
{
	if(btjpg_log)
	{
		fputs(str, btjpg_log);
		fflush(btjpg_log);
	}
}

void btjpg_printf(char *str, ...)
{
	char buf[65536];
	va_list lst;

	if(!btjpg_log)
		return;
		
	va_start(lst, str);
//	vfprintf(btjpg_log, str, lst);
	vsprintf(buf, str, lst);
	va_end(lst);
	
	btjpg_puts(buf);
}

void *btjpg_gcalloc(size_t sz)
{
	void *ptr;
	
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void *btjpg_gctalloc(char *str, size_t sz)
{
	void *ptr;
	
	ptr=malloc(sz);
	memset(ptr, 0, sz);
	return(ptr);
}

void btjpg_gcfree(void *ptr)
{
	free(ptr);
}

char *btjpg_strdup(char *str)
{
	char *s1;
	
	s1=strdup(str);
	return(s1);
}

#endif
