#include <bsgen.h>

static void *bsg_ralloc_buf=NULL;
static int bsg_ralloc_pos=0;

static char *bsg_strtab=NULL;
static char *bsg_strtabe=NULL;
static char **bsg_strhash;

#if 0
void *(*bsg_fopen_fp)(char *name, char *mode);
void (*bsg_fclose_fp)(void *fd);
int (*bsg_fread_fp)(void *buf, int i, int j, void *fd);
int (*bsg_fwrite_fp)(void *buf, int i, int j, void *fd);
void (*bsg_fseek_fp)(void *buf, int i, int j);
int (*bsg_fgetc_fp)(void *fd);
int (*bsg_fputc_fp)(int i, void *fd);
int (*bsg_ftell_fp)(void *fd);
int (*bsg_feof_fp)(void *fd);
int (*bsg_fgets_fp)(void *buf, int sz, void *fd);
#endif

BSG_IO *bsg_io=NULL;

void *bsg_ralloc(int sz)
{
	void *p;
	if(!bsg_ralloc_buf)bsg_ralloc_buf=malloc(262144);

	if((bsg_ralloc_pos+sz)>262144)
		bsg_ralloc_pos=0;
	p=((byte *)bsg_ralloc_buf)+bsg_ralloc_pos;
	bsg_ralloc_pos=(bsg_ralloc_pos+sz+3)&(~3);

	return(p);
}

char **bsg_split(char *s)
{
	char **a, *t;
	int i;

	a=bsg_ralloc(128*sizeof(char *));
	memset(a, 0, 128*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=bsg_ralloc(64);
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

char *bsg_rstrdup(char *s)
{
	char *t;

	t=bsg_ralloc(strlen(s)+1);
	strcpy(t, s);
	return(t);
}

#if 0
char *bsg_strdup(char *s)
{
	char *t;

	if(!bsg_strtab)
	{
		bsg_strtab=malloc(1<<24);
		bsg_strtabe=bsg_strtab+1;
		*bsg_strtab='0';
	}

	if(!s)return(NULL);
	if(!*s)return(bsg_strtab);

	t=bsg_strtab+1;
	while(t<bsg_strtabe)
	{
		if(!strcmp(t, s))
			return(t);
		t+=strlen(t)+1;
	}

	strcpy(t, s);
	bsg_strtabe=t+strlen(t)+1;
	return(t);
}
#endif

char *bsg_strdup(char *str)
{
	char *s, *t;
	int i;

	if(!bsg_strhash)
	{
		bsg_strtab=malloc(1<<24);
		bsg_strtabe=bsg_strtab;

		i=4096*sizeof(char *);
		bsg_strhash=malloc(i);
		memset(bsg_strhash, 0, i);
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

	t=bsg_strhash[i];
	while(t)
	{
		s=(char *)(((char **)t)+1);
		t=*(char **)t;
		if(!strcmp(s, str))return(s);
	}

	t=malloc(strlen(str)+1+sizeof(char *));

//	t=bsg_strtabe;
//	bsg_strtabe=t+strlen(str)+1+sizeof(char *);

	s=(char *)(((char **)t)+1);
	strcpy(s, str);

	*(char **)t=bsg_strhash[i];
	bsg_strhash[i]=t;
	return(s);
}


static void *bsg_fopen_i(char *name, char *mode)
{
	char tb[256];
	FILE *fd;

	fd=fopen(name, mode);
	if(fd)return(fd);
	
	sprintf(tb, "../resource/%s", name);
	fd=fopen(tb, mode);
	if(fd)return(fd);

	sprintf(tb, "../resource_dev/%s", name);
	fd=fopen(tb, mode);
	if(fd)return(fd);
	
	return(fd);
}
static void bsg_fclose_i(void *fd)
	{ fclose((FILE *)fd); }
static int bsg_fread_i(void *buf, int i, int j, void *fd)
	{ return(fread(buf, i, j, (FILE *)fd)); }
static int bsg_fwrite_i(void *buf, int i, int j, void *fd)
	{ return(fwrite(buf, i, j, (FILE *)fd)); }
static void bsg_fseek_i(void *fd, int i, int j)
	{ fseek((FILE *)fd, i, j); }
static int bsg_fgetc_i(void *fd)
	{ return(fgetc((FILE *)fd)); }
static int bsg_fputc_i(int i, void *fd)
	{ return(fputc(i, (FILE *)fd)); }
static int bsg_ftell_i(void *fd)
	{ return(ftell((FILE *)fd)); }
static int bsg_feof_i(void *fd)
	{ return(feof((FILE *)fd)); }
static int bsg_fgets_i(void *buf, int sz, void *fd)
	{ fgets(buf, sz, (FILE *)fd); }

BSG_API BSG_IO *bsg_file_getio()
{
	if(!bsg_io)
	{
		bsg_io=malloc(sizeof(BSG_IO));
		memset(bsg_io, 0, sizeof(BSG_IO));
	}

	return(bsg_io);
}

void bsg_file_init()
{
	if(!bsg_io)
	{
		bsg_io=malloc(sizeof(BSG_IO));
		memset(bsg_io, 0, sizeof(BSG_IO));

		bsg_io->fopen_fp=bsg_fopen_i;
		bsg_io->fclose_fp=bsg_fclose_i;
		bsg_io->fread_fp=bsg_fread_i;
		bsg_io->fwrite_fp=bsg_fwrite_i;
		bsg_io->fseek_fp=bsg_fseek_i;
		bsg_io->fgetc_fp=bsg_fgetc_i;
		bsg_io->fputc_fp=bsg_fputc_i;
		bsg_io->ftell_fp=bsg_ftell_i;
		bsg_io->feof_fp=bsg_feof_i;
		bsg_io->fgets_fp=bsg_fgets_i;
	}
}

void *bsg_fopen(char *name, char *mode)
{
	bsg_file_init();
	return(bsg_io->fopen_fp(name, mode));
}

void bsg_fclose(void *fd)
	{ bsg_io->fclose_fp(fd); }

int bsg_fread(void *buf, int i, int j, void *fd)
	{ return(bsg_io->fread_fp(buf, i, j, fd)); }
int bsg_fwrite(void *buf, int i, int j, void *fd)
	{ return(bsg_io->fwrite_fp(buf, i, j, fd)); }
void bsg_fseek(void *buf, int i, int j)
	{ bsg_io->fseek_fp(buf, i, j); }
int bsg_fgetc(void *fd)
	{ return(bsg_io->fgetc_fp(fd)); }
int bsg_fputc(int i, void *fd)
	{ return(bsg_io->fputc_fp(i, fd)); }
int bsg_ftell(void *fd)
	{ return(bsg_io->ftell_fp(fd)); }
int bsg_feof(void *fd)
	{ return(bsg_io->feof_fp(fd)); }
int bsg_fgets(void *buf, int sz, void *fd)
	{ return(bsg_io->fgets_fp(buf, sz, fd)); }

