#include <bgbccc.h>

#ifdef WIN32
#include <windows.h>
#endif

#define BGBCC_SZSTRHASH		16384

void *bgbcc_alloc_block[1024];
int bgbcc_alloc_nblock;

byte *bgbcc_alloc_rov=NULL;
byte *bgbcc_alloc_srov=NULL;
byte *bgbcc_alloc_erov=NULL;

void *bgbcc_alloc_list=NULL;

// char *bgbcc_alloc_strhash[BGBCC_SZSTRHASH];

char **bgbcc_str_varr;
int *bgbcc_str_carr;
int bgbcc_str_num;
int bgbcc_str_max;
int bgbcc_str_hash[BGBCC_SZSTRHASH];

byte *bgbcc_rlcbuf=NULL;
int bgbcc_rlcpos;

void BGBCC_DieError()
{
//#ifdef WIN32
#if 0
	ULONG_PTR args[8];
	RaiseException(BGBCC_EXCEPTION, EXCEPTION_NONCONTINUABLE,
		1, args);
#else
//	*(int *)-1=-1;
#endif
}

void BGBCC_DieFatal()
{
//#ifdef WIN32
#if 0
	ULONG_PTR args[8];
	RaiseException(BGBCC_EXCEPTION, EXCEPTION_NONCONTINUABLE,
		1, args);
#else
	*(int *)-1=-1;
#endif
}

void BGBCC_CleanupAll()
{
	int i;

//	bgbcc_alloc_rov=0;
//	bgbcc_alloc_srov=0;
//	bgbcc_alloc_erov=0;

//	for(i=0; i<bgbcc_alloc_nblock; i++)
//		free(bgbcc_alloc_block[i]);
//	bgbcc_alloc_nblock=0;

	if(bgbcc_alloc_rov)
	{
		printf("BGBCC_CleanupAll: Used %d block(s)\n",
			bgbcc_alloc_nblock);

//		for(i=1; i<bgbcc_alloc_nblock; i++)
//			free(bgbcc_alloc_block[i]);
		bgbcc_alloc_nblock=1;

		bgbcc_alloc_rov=bgbcc_alloc_block[0];
		bgbcc_alloc_srov=bgbcc_alloc_block[0];
		bgbcc_alloc_erov=bgbcc_alloc_srov+(1<<22);
	}

//	for(i=0; i<BGBCC_SZSTRHASH; i++)
//		bgbcc_alloc_strhash[i]=NULL;

//	for(i=0; i<BGBCC_SZSTRHASH; i++)
//		bgbcc_str_hash[i]=0;
//	bgbcc_str_varr=NULL;
//	bgbcc_str_carr=NULL;
}

void *bgbcc_tmalloc(char *ty, int sz)
{
	void *p;
	int i, n;

	if(!bgbcc_alloc_rov)
	{
		for(i=0; i<1024; i++)
			bgbcc_alloc_block[i]=NULL;

		bgbcc_alloc_srov=malloc(1<<22);
		bgbcc_alloc_erov=bgbcc_alloc_srov+(1<<22);

		bgbcc_alloc_rov=bgbcc_alloc_srov;

		bgbcc_alloc_block[bgbcc_alloc_nblock++]=
			bgbcc_alloc_rov;
	}

	if((bgbcc_alloc_rov+sz)>=(bgbcc_alloc_erov-4096))
	{
		n=bgbcc_alloc_nblock++;
		if(!bgbcc_alloc_block[n])
		{
			printf("BGBCC: Expand\n");
			bgbcc_alloc_block[n]=malloc(1<<22);
		}
		
		bgbcc_alloc_srov=bgbcc_alloc_block[n];
		bgbcc_alloc_erov=bgbcc_alloc_srov+(1<<22);
		bgbcc_alloc_rov=bgbcc_alloc_srov;
	}

	if(sz>=65536)
	{
		sz+=2*sizeof(void *);
		p=malloc(sz);
		memset(p, 0, sz);
		*(void **)p=bgbcc_alloc_list;
		bgbcc_alloc_list=p;
		p=(void *)(((void **)p)+2);
		return(p);
	}

	p=bgbcc_alloc_rov;
	bgbcc_alloc_rov+=(sz+15)&(~15);

	memset(p, 0, sz);
	return(p);
}

void *bgbcc_malloc(int sz)
{
	return(bgbcc_tmalloc(NULL, sz));
}

void *bgbcc_tmalloc2(char *ty, int sz)
{
	void *p;
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

void *bgbcc_malloc2(int sz)
{
	return(bgbcc_tmalloc2(NULL, sz));
}

void bgbcc_free(void *p)
{
//	free(p);
}

#if 0
char *bgbcc_strdup(char *str)
{
	char *s, *t;
	int i, hi;

	if(!str)return(NULL);

	s=str; hi=0;
	while(*s)hi=(hi*4093)+(*s++);
	i=((hi*4093)>>12)&4095;

	t=bgbcc_alloc_strhash[i];
	if(t && !strcmp(t, str))
		return(t);

	t=bgbcc_malloc(strlen(str)+1);
	strcpy(t, str);
	bgbcc_alloc_strhash[i]=t;

	return(t);
}
#endif

int bgbcc_strdup_i(char *str)
{
	char *s, *t;
	int i, hi;

	if(!str)return(0);

#if 1
	if(!bgbcc_str_varr)
	{
		for(i=0; i<BGBCC_SZSTRHASH; i++)bgbcc_str_hash[i]=0;
		bgbcc_str_varr=malloc(16384*sizeof(char *));
		bgbcc_str_carr=malloc(16384*sizeof(int));

		bgbcc_str_varr[0]=NULL;
		bgbcc_str_carr[0]=0;
		bgbcc_str_num=1;
		bgbcc_str_max=16384;
	}
#endif

	s=str; hi=0;
	while(*s)hi=(hi*251)+(*s++);
	hi&=(BGBCC_SZSTRHASH-1);
//	hi=(hi>>8)&(BGBCC_SZSTRHASH-1);

	i=bgbcc_str_hash[hi];
	while(i)
	{
		if(!strcmp(bgbcc_str_varr[i], str))
			return(i);
		i=bgbcc_str_carr[i];
	}

	if((bgbcc_str_num+1)>=bgbcc_str_max)
	{
		i=bgbcc_str_max+(bgbcc_str_max>>1);
		bgbcc_str_varr=realloc(bgbcc_str_varr, i*sizeof(char *));
		bgbcc_str_carr=realloc(bgbcc_str_carr, i*sizeof(int));
		bgbcc_str_max=i;
	}

//	t=bgbcc_malloc(strlen(str)+1);
	t=malloc(strlen(str)+1);
	strcpy(t, str);

	i=bgbcc_str_num++;
	bgbcc_str_varr[i]=t;
	bgbcc_str_carr[i]=bgbcc_str_hash[hi];
	bgbcc_str_hash[hi]=i;

	return(i);
}

char *bgbcc_strtab_i(int i)
	{ return(bgbcc_str_varr[i]); }

char *bgbcc_strdup(char *str)
{
	int i;

	if(!str)return(NULL);
	i=bgbcc_strdup_i(str);
	return(bgbcc_str_varr[i]);
}


void *bgbcc_ralloc(int sz)
{
	void *p;

	if(!bgbcc_rlcbuf)
	{
		bgbcc_rlcbuf=(byte *)malloc(1<<18);
		bgbcc_rlcpos=0;
	}

	if((bgbcc_rlcpos+sz)>=((1<<18)-4096))
		bgbcc_rlcpos=0;
	p=(void *)(bgbcc_rlcbuf+bgbcc_rlcpos);
	bgbcc_rlcpos=(bgbcc_rlcpos+sz+15)&(~15);

	return(p);
}

char *bgbcc_rstrdup(char *s)
{
	char *t;
	t=(char *)bgbcc_ralloc(strlen(s)+1);
	strcpy(t, s);
	return(t);
}

char **bgbcc_split(char *s)
{
	char **a, *t;
	int i;

	a=bgbcc_ralloc(64*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		if(*s=='#')break;
		if(*s==';')break;
		if((s[0]=='/') && (s[1]=='/'))
			break;

		t=bgbcc_ralloc(256);
		a[i++]=t;

		if(*s=='"')
		{
			s++;
			while(*s && (*s!='"'))*t++=*s++;
			if(*s=='"')s++;

			*t++=0;
			continue;
		}

		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
	}
	a[i]=NULL;

	return(a);
}

char *bgbcc_rstradd(char *s0, char *s1)
{
	char *t;

	t=(char *)bgbcc_ralloc(strlen(s0)+strlen(s1)+1);
	strcpy(t, s0);
	strcat(t, s1);
	return(t);
}

char *bgbcc_rstradd2(char *s0, char *s1, char *s2)
{
	char *t;
	int i;

	i=strlen(s0)+strlen(s1)+strlen(s2);
	t=(char *)bgbcc_ralloc(i+1);
	strcpy(t, s0);
	strcat(t, s1);
	strcat(t, s2);
	return(t);
}

char *bgbcc_rstradd3(char *s0, char *s1, char *s2, char *s3)
{
	char *t;
	int i;

	i=strlen(s0)+strlen(s1)+strlen(s2)+strlen(s3);
	t=(char *)bgbcc_ralloc(i+1);
	strcpy(t, s0); strcat(t, s1);
	strcat(t, s2); strcat(t, s3);
	return(t);
}


struct bgbcc_ordtab_s
{
	char **tab;
	short *chain;
	short hash[256];
	int num;
};

void *bgbcc_neword(char **tab)
{
	struct bgbcc_ordtab_s *tmp;
	char *s;
	int i, j, n;
	
	for(n=0; tab[n]; n++);
	tmp=bgbcc_malloc2(sizeof(struct bgbcc_ordtab_s));
	tmp->tab=bgbcc_malloc2((n+1)*sizeof(char *));
	tmp->chain=bgbcc_malloc2((n+1)*sizeof(short));
	tmp->num=n;

	for(i=0; i<256; i++)
		tmp->hash[i]=-1;

	for(i=0; i<n; i++)
		tmp->tab[i]=bgbcc_strdup(tab[i]);

	for(i=0; i<n; i++)
	{
		s=tab[i]; j=0;
		while(*s)j=(j*251)+(*s++);
		j&=255;
		tmp->chain[i]=tmp->hash[j];
		tmp->hash[j]=i;
	}
	
	return((void *)tmp);
}

int bgbcc_strord(char *str, void *tab)
{
	struct bgbcc_ordtab_s *tmp;
	char *s;
	int i, j, n;

	tmp=(struct bgbcc_ordtab_s *)tab;

	s=str; j=0;
	while(*s)j=(j*251)+(*s++);
	j&=255;
	
	i=tmp->hash[j];
	while(i>=0)
	{
		if(!strcmp(tmp->tab[i], str))
			return(i);
		i=tmp->chain[i];
	}
	return(-1);
}

extern char *bgbcc_tokord_strs[];
extern char *bgbcc_tokord_args[];

int BGBCC_GetOrdinal(char *str)
{
	static void *tab=NULL;
	if(!tab)
	{
		tab=bgbcc_neword(bgbcc_tokord_strs);
	}
	return(bgbcc_strord(str, tab));
}

int BGBCC_CheckOrdinalFlag(int i, int ty)
{
	char *s;

	if(i<0)return(0);
	s=bgbcc_tokord_args[i];
	if(s && *s)
	{
		while(*s) { if(*s==ty)break; s++; }
		if(*s)return(1);
	}
	return(0);
}

int BGBCC_GetOrdinalLang(char *str, int lang)
{
	static char *lchrs="-CPSJEF";
	static void *tab=NULL;
	char *s;
	int i, c;
	
	if(!tab)
		{ tab=bgbcc_neword(bgbcc_tokord_strs); }

	i=bgbcc_strord(str, tab);
	if(i<0)return(-1);
	s=bgbcc_tokord_args[i];
	if(s && *s && (lang>0))
	{
		c=lchrs[lang];	
		while(*s) { if(*s==c)break; s++; }
		if(!(*s))return(-1);
	}
	return(i);
}

int BGBCC_GetOrdinalLangType(char *str, int lang, int ty)
{
	char *s;
	int i;

	i=BGBCC_GetOrdinalLang(str, lang);
	if(i<0)return(-1);
	s=bgbcc_tokord_args[i];
	if(s && *s && (lang>0))
	{
		while(*s) { if(*s==ty)break; s++; }
		if(!(*s))return(-1);
	}
	return(i);
}


double BGBCC_ParseNumber(char *str)
{
	long long l;
	char *s, *t;
	int i, sg;

	t=str;
	if((t[0]=='0') && (t[1]=='x'))
	{
		l=0; t+=2;
		while(*t)
		{
			l*=16;
			if((*t>='0') && (*t<='9'))l+=*t-'0';
			if((*t>='A') && (*t<='F'))l+=*t-'A'+10;
			if((*t>='a') && (*t<='f'))l+=*t-'a'+10;
			t++;
		}

		return(l);
	}

	if(t[0]=='0')
	{
		l=0; t++;
		while(*t)
		{
			l*=8;
			if((*t>='0') && (*t<='7'))l+=*t-'0';
				else break;
			t++;
		}

		return(l);
	}

	i=0; t=str;
	while(*t) { if(*t=='.')i++; if(*t=='e')i++; t++; }
	if(i)
	{
		return(atof(str));
	}

	t=str; l=0; sg=0;
	if(*t=='-') { sg=1; t++; }
	while(*t)
	{
		l*=10;
		if((*t>='0') && (*t<='9'))l+=*t-'0';
			else break;
		t++;
	}
	if(sg)l=-l;

	return(l);
}
