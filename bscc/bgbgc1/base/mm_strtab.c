#include <bgbgc.h>

//#ifndef BGBGC_SHAVEMEM
#if 0
#define BGBGC_SZSTRHASH 4096
#define BGBGC_SZSTRINIT (1<<20)
#define BGBGC_SZSTRLIM (BGBGC_SZSTRINIT-4096)
#define BGBGC_SZSTRVINIT 16384
#else
#define BGBGC_SZSTRHASH 256
#define BGBGC_SZSTRINIT (1<<14)
#define BGBGC_SZSTRLIM (BGBGC_SZSTRINIT-256)
#define BGBGC_SZSTRVINIT 1024
#endif

char *bgbgc_strtab;		//strings table
char *bgbgc_strtabe;	//strings table end

char **bgbgc_str_varr;
int *bgbgc_str_carr;
int bgbgc_str_num;
int bgbgc_str_max;
int bgbgc_str_hash[BGBGC_SZSTRHASH];

#if 1
char *bgbgc_strdup_alloc(char *str)
{
	char *s, *t;
	int i, hi;

	if(!bgbgc_strtab)
	{
		bgbgc_strtab=(char *)malloc(BGBGC_SZSTRINIT);
		bgbgc_strtabe=bgbgc_strtab+1;
	}

	if((str>=bgbgc_strtab) && (str<bgbgc_strtabe))
		return(str);

	if(!str)return(NULL);
	if(!*str)return(bgbgc_strtab);

	s=bgbgc_strtabe;
	strcpy(s, str);
	bgbgc_strtabe=s+strlen(s)+1;

	i=bgbgc_strtabe-bgbgc_strtab;
	if(i>=BGBGC_SZSTRLIM)
	{
		//new string table
		bgbgc_strtab=(char *)malloc(BGBGC_SZSTRINIT);
		bgbgc_strtabe=bgbgc_strtab+1;
	}

	return(s);
}

BGBGC_API int bgbgc_strdup_i(char *str)
{
	char *s, *t;
	int i, hi;

	if(!str)return(0);

#if 1
	if(!bgbgc_str_varr)
	{
		for(i=0; i<BGBGC_SZSTRHASH; i++)bgbgc_str_hash[i]=0;
		bgbgc_str_varr=malloc(BGBGC_SZSTRVINIT*sizeof(char *));
		bgbgc_str_carr=malloc(BGBGC_SZSTRVINIT*sizeof(int));

		bgbgc_str_varr[0]=NULL;
		bgbgc_str_carr[0]=0;
		bgbgc_str_num=1;
		bgbgc_str_max=BGBGC_SZSTRVINIT;
	}
#endif

	s=str; hi=0;
	while(*s)hi=(hi*251)+(*s++);
	hi&=BGBGC_SZSTRHASH-1;

	i=bgbgc_str_hash[hi];
	while(i)
	{
		if(!strcmp(bgbgc_str_varr[i], str))
			return(i);
		i=bgbgc_str_carr[i];
	}

	if((bgbgc_str_num+1)>=bgbgc_str_max)
	{
		i=bgbgc_str_max+(bgbgc_str_max>>1);
		bgbgc_str_varr=realloc(bgbgc_str_varr, i*sizeof(char *));
		bgbgc_str_carr=realloc(bgbgc_str_carr, i*sizeof(int));
		bgbgc_str_max=i;
	}

	t=bgbgc_strdup_alloc(str);

	i=bgbgc_str_num++;
	bgbgc_str_varr[i]=t;
	bgbgc_str_carr[i]=bgbgc_str_hash[hi];
	bgbgc_str_hash[hi]=i;

	return(i);
}

BGBGC_API char *bgbgc_strtab_i(int i)
	{ return(bgbgc_str_varr[i]); }

BGBGC_API char *bgbgc_strdup(char *str)
{
	int i;

	if(!str)return(NULL);
	i=bgbgc_strdup_i(str);
	return(bgbgc_str_varr[i]);
}
#endif

BGBGC_API int bgbgc_strildup_i(char *str)
{
	char *s, *t, *b;
	int i;

	b=bgbgc_ralloc(strlen(str)+1);

	s=str; t=b;
	while(*s)
	{
		i=*s++;
		if((i>='A') && (i<='Z'))
			i=i-'A'+'a';
		*t++=i;
	}
	*t++=0;

	return(bgbgc_strdup_i(b));
}

BGBGC_API void *bgbgc_ralloc(int sz)
{
	static char *buf=NULL;
	static int pos=0;
	char *s;

	if(!buf)buf=malloc(1<<16);
	if((pos+sz)>=(1<<16))pos=0;

	s=buf+pos;
	pos=(pos+sz+7)&(~7);
	return(s);
}

BGBGC_API char *bgbgc_rstrdup(char *str)
{
	char *t;
	t=bgbgc_ralloc(strlen(str)+1);
	strcpy(t, str);
	return(t);
}
