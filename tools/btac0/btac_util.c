#include <bgbtac.h>

static void *bgbtac_ralloc_buf=NULL;
static int bgbtac_ralloc_pos=0;

static char *bgbtac_strtab=NULL;
static char *bgbtac_strtabe=NULL;
static char **bgbtac_strhash;

void *(*bgbtac_fopen_fp)(char *name, char *mode);
void (*bgbtac_fclose_fp)(void *fd);
int (*bgbtac_fread_fp)(void *buf, int i, int j, void *fd);
int (*bgbtac_fwrite_fp)(void *buf, int i, int j, void *fd);
void (*bgbtac_fseek_fp)(void *buf, int i, int j);
int (*bgbtac_fgetc_fp)(void *fd);
int (*bgbtac_fputc_fp)(int i, void *fd);
int (*bgbtac_ftell_fp)(void *fd);
int (*bgbtac_feof_fp)(void *fd);
int (*bgbtac_fgets_fp)(void *buf, int sz, void *fd);

BTAC_API void *bgbtac_ralloc(int sz)
{
	void *p;
	if(!bgbtac_ralloc_buf)bgbtac_ralloc_buf=malloc(262144);

	if((bgbtac_ralloc_pos+sz)>262144)
		bgbtac_ralloc_pos=0;
	p=(void *)(((byte *)bgbtac_ralloc_buf)+bgbtac_ralloc_pos);
	bgbtac_ralloc_pos=(bgbtac_ralloc_pos+sz+3)&(~3);

	return(p);
}

BTAC_API char **bgbtac_split(char *s)
{
	char **a, *t;
	int i;

	a=bgbtac_ralloc(128*sizeof(char *));
	memset(a, 0, 128*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=bgbtac_ralloc(64);
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

BTAC_API char *bgbtac_rstrdup(char *s)
{
	char *t;

	t=bgbtac_ralloc(strlen(s)+1);
	strcpy(t, s);
	return(t);
}

#if 0
BTAC_API char *bgbtac_strdup(char *s)
{
	char *t;

	if(!bgbtac_strtab)
	{
		bgbtac_strtab=malloc(1<<24);
		bgbtac_strtabe=bgbtac_strtab+1;
		*bgbtac_strtab='0';
	}

	if(!s)return(NULL);
	if(!*s)return(bgbtac_strtab);

	t=bgbtac_strtab+1;
	while(t<bgbtac_strtabe)
	{
		if(!strcmp(t, s))
			return(t);
		t+=strlen(t)+1;
	}

	strcpy(t, s);
	bgbtac_strtabe=t+strlen(t)+1;
	return(t);
}
#endif

BTAC_API char *bgbtac_strdup(char *str)
{
	char *s, *t;
	int i;

	if(!bgbtac_strhash)
	{
		bgbtac_strtab=malloc(1<<24);
		bgbtac_strtabe=bgbtac_strtab;

		i=4096*sizeof(char *);
		bgbtac_strhash=malloc(i);
		memset(bgbtac_strhash, 0, i);
	}

	if(!str)
	{
//		*(int *)-1=-1;
		return(NULL);
	}
	if(!*str)return("");

	i=0; s=str;
	while(*s)i=i*251+(*s++);
	i=((i*251)>>8)&0xFFF;

	t=bgbtac_strhash[i];
	while(t)
	{
		s=(char *)(((char **)t)+1);
		t=*(char **)t;
		if(!strcmp(s, str))return(s);
	}

	t=malloc(strlen(str)+1+sizeof(char *));

//	t=bgbtac_strtabe;
//	bgbtac_strtabe=t+strlen(str)+1+sizeof(char *);

	s=(char *)(((char **)t)+1);
	strcpy(s, str);

	*(char **)t=bgbtac_strhash[i];
	bgbtac_strhash[i]=t;
	return(s);
}


static void *bgbtac_fopen_i(char *name, char *mode)
{
	char tb[256];
	FILE *fd;

	fd=fopen(name, mode);
	if(fd)return(fd);
	sprintf(tb, "resource/%s", name);
	fd=fopen(tb, mode);
	if(fd)return(fd);
	sprintf(tb, "baseq2/%s", name);
	fd=fopen(tb, mode);
	if(fd)return(fd);

	return(NULL);
}
static void bgbtac_fclose_i(void *fd)
	{ fclose((FILE *)fd); }
static int bgbtac_fread_i(void *buf, int i, int j, void *fd)
	{ return(fread(buf, i, j, (FILE *)fd)); }
static int bgbtac_fwrite_i(void *buf, int i, int j, void *fd)
	{ return(fwrite(buf, i, j, (FILE *)fd)); }
static void bgbtac_fseek_i(void *fd, int i, int j)
	{ fseek((FILE *)fd, i, j); }
static int bgbtac_fgetc_i(void *fd)
	{ return(fgetc((FILE *)fd)); }
static int bgbtac_fputc_i(int i, void *fd)
	{ return(fputc(i, (FILE *)fd)); }
static int bgbtac_ftell_i(void *fd)
	{ return(ftell((FILE *)fd)); }
static int bgbtac_feof_i(void *fd)
	{ return(feof((FILE *)fd)); }
static int bgbtac_fgets_i(void *buf, int sz, void *fd)
	{ fgets(buf, sz, (FILE *)fd); }

BTAC_API void bgbtac_file_init()
{
	bgbtac_fopen_fp=bgbtac_fopen_i;
	bgbtac_fclose_fp=bgbtac_fclose_i;
	bgbtac_fread_fp=bgbtac_fread_i;
	bgbtac_fwrite_fp=bgbtac_fwrite_i;
	bgbtac_fseek_fp=bgbtac_fseek_i;
	bgbtac_fgetc_fp=bgbtac_fgetc_i;
	bgbtac_fputc_fp=bgbtac_fputc_i;
	bgbtac_ftell_fp=bgbtac_ftell_i;
	bgbtac_feof_fp=bgbtac_feof_i;
	bgbtac_fgets_fp=bgbtac_fgets_i;
}

BTAC_API void *bgbtac_fopen(char *name, char *mode)
{
	if(!bgbtac_fopen_fp)
		bgbtac_file_init();
	return(bgbtac_fopen_fp(name, mode));
}

BTAC_API void bgbtac_fclose(void *fd)
	{ bgbtac_fclose_fp(fd); }

BTAC_API int bgbtac_fread(void *buf, int i, int j, void *fd)
	{ return(bgbtac_fread_fp(buf, i, j, fd)); }
BTAC_API int bgbtac_fwrite(void *buf, int i, int j, void *fd)
	{ return(bgbtac_fwrite_fp(buf, i, j, fd)); }
BTAC_API void bgbtac_fseek(void *buf, int i, int j)
	{ bgbtac_fseek_fp(buf, i, j); }
BTAC_API int bgbtac_fgetc(void *fd)
	{ return(bgbtac_fgetc_fp(fd)); }
BTAC_API int bgbtac_fputc(int i, void *fd)
	{ return(bgbtac_fputc_fp(i, fd)); }
BTAC_API int bgbtac_ftell(void *fd)
	{ return(bgbtac_ftell_fp(fd)); }
BTAC_API int bgbtac_feof(void *fd)
	{ return(bgbtac_feof_fp(fd)); }
BTAC_API int bgbtac_fgets(void *buf, int sz, void *fd)
	{ return(bgbtac_fgets_fp(buf, sz, fd)); }

