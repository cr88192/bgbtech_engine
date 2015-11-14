#include <bgbbtj.h>


#if defined(BGBBTJ_DRV) || defined(BGBBTJ_NODY)

#ifndef BGBBTJ_DRVLOGFILE
#ifdef _MSC_VER
#define BGBBTJ_DRVLOGFILE "F:\\btjdrv_log.txt"
#endif
#ifdef __linux__
#define BGBBTJ_DRVLOGFILE "~/.btjdrv_log.txt"
#endif
#endif

FILE *btjpg_log=NULL;

void BTJPG_DriverInit(void)
{
	static int init=0;

	if(init)return;
	init=1;
	
	btjpg_log=fopen(BGBBTJ_DRVLOGFILE, "at");
	if(btjpg_log)
	{
		btjpg_puts("--------\n");
	}
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
