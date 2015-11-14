#include <bs2asm.h>

#ifdef WIN32
#include <windows.h>
#endif

#define BS2ASM_SZSTRHASH		4096

void *bs2asm_alloc_block[1024];
int bs2asm_alloc_nblock;

byte *bs2asm_alloc_rov=NULL;
byte *bs2asm_alloc_srov=NULL;
byte *bs2asm_alloc_erov=NULL;

void *bs2asm_alloc_list=NULL;

// char *bs2asm_alloc_strhash[BS2ASM_SZSTRHASH];

char **bs2asm_str_varr;
int *bs2asm_str_carr;
int bs2asm_str_num;
int bs2asm_str_max;
int bs2asm_str_hash[BS2ASM_SZSTRHASH];

byte *bs2asm_rlcbuf=NULL;
int bs2asm_rlcpos;

void BS2ASM_DieError()
{
//#ifdef WIN32
#if 0
	ULONG_PTR args[8];
	RaiseException(BS2ASM_EXCEPTION, EXCEPTION_NONCONTINUABLE,
		1, args);
#else
	*(int *)-1=-1;
#endif
}

void BS2ASM_DieFatal()
{
//#ifdef WIN32
#if 0
	ULONG_PTR args[8];
	RaiseException(BS2ASM_EXCEPTION, EXCEPTION_NONCONTINUABLE,
		1, args);
#else
	*(int *)-1=-1;
#endif
}

void BS2ASM_Error(BS2ASM_Context *ctx, char *str, ...)
{
}

void BS2ASM_CleanupAll()
{
	int i;

	if(bs2asm_alloc_rov)
	{
		printf("BS2ASM_CleanupAll: Used %d block(s)\n",
			bs2asm_alloc_nblock);

		bs2asm_alloc_nblock=1;

		bs2asm_alloc_rov=bs2asm_alloc_block[0];
		bs2asm_alloc_srov=bs2asm_alloc_block[0];
		bs2asm_alloc_erov=bs2asm_alloc_srov+(1<<22);
	}
}

void *bs2asm_tmalloc(char *ty, int sz)
{
	void *p;
	int i, n;

	if(!bs2asm_alloc_rov)
	{
		for(i=0; i<1024; i++)
			bs2asm_alloc_block[i]=NULL;

		bs2asm_alloc_srov=malloc(1<<22);
		bs2asm_alloc_erov=bs2asm_alloc_srov+(1<<22);

		bs2asm_alloc_rov=bs2asm_alloc_srov;

		bs2asm_alloc_block[bs2asm_alloc_nblock++]=
			bs2asm_alloc_rov;
	}

	if((bs2asm_alloc_rov+sz)>=(bs2asm_alloc_erov-4096))
	{
		n=bs2asm_alloc_nblock++;
		if(!bs2asm_alloc_block[n])
		{
			printf("BGBCC: Expand\n");
			bs2asm_alloc_block[n]=malloc(1<<22);
		}
		
		bs2asm_alloc_srov=bs2asm_alloc_block[n];
		bs2asm_alloc_erov=bs2asm_alloc_srov+(1<<22);
		bs2asm_alloc_rov=bs2asm_alloc_srov;
	}

	if(sz>=65536)
	{
		sz+=2*sizeof(void *);
		p=malloc(sz);
		memset(p, 0, sz);
		*(void **)p=bs2asm_alloc_list;
		bs2asm_alloc_list=p;
		p=(void *)(((void **)p)+2);
		return(p);
	}

	p=bs2asm_alloc_rov;
	bs2asm_alloc_rov+=(sz+15)&(~15);

	memset(p, 0, sz);
	return(p);
}

void *bs2asm_malloc(int sz)
{
	return(bs2asm_tmalloc(NULL, sz));
}

void *bs2asm_tmalloc2(char *ty, int sz)
{
	void *p;
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

void *bs2asm_malloc2(int sz)
{
	return(bs2asm_tmalloc2(NULL, sz));
}

void bs2asm_free(void *p)
{
//	free(p);
}

void bs2asm_free2(void *p)
{
	free(p);
}

void *bs2asm_realloc(void *ptr, int sz)
{
	void *p;

	p=bs2asm_malloc(sz);
	memcpy(p, ptr, sz);

	return(p);
}

void *bs2asm_realloc2(void *ptr, int sz)
{
	void *p;

//	p=bs2asm_malloc2(sz);
//	memcpy(p, ptr, sz);
	p=realloc(ptr, sz);

	return(p);
}

void *bs2asm_ctalloc(BS2ASM_Context *ctx, char *ty, int sz)
{
	return(bs2asm_tmalloc(ty, sz));
}

void *bs2asm_cmalloc(BS2ASM_Context *ctx, int sz)
{
	return(bs2asm_malloc(sz));
}

void *bs2asm_crealloc(BS2ASM_Context *ctx, void *ptr, int sz)
{
	return(bs2asm_realloc(ptr, sz));
}

int bs2asm_strdup_i(char *str)
{
	char *s, *t;
	int i, hi;

	if(!str)return(0);

#if 1
	if(!bs2asm_str_varr)
	{
		for(i=0; i<BS2ASM_SZSTRHASH; i++)bs2asm_str_hash[i]=0;
		bs2asm_str_varr=malloc(16384*sizeof(char *));
		bs2asm_str_carr=malloc(16384*sizeof(int));

		bs2asm_str_varr[0]=NULL;
		bs2asm_str_carr[0]=0;
		bs2asm_str_num=1;
		bs2asm_str_max=16384;
	}
#endif

	s=str; hi=0;
	while(*s)hi=(hi*251)+(*s++);
	hi&=(BS2ASM_SZSTRHASH-1);
//	hi=(hi>>8)&(BS2ASM_SZSTRHASH-1);

	i=bs2asm_str_hash[hi];
	while(i)
	{
		if(!strcmp(bs2asm_str_varr[i], str))
			return(i);
		i=bs2asm_str_carr[i];
	}

	if((bs2asm_str_num+1)>=bs2asm_str_max)
	{
		i=bs2asm_str_max+(bs2asm_str_max>>1);
		bs2asm_str_varr=realloc(bs2asm_str_varr, i*sizeof(char *));
		bs2asm_str_carr=realloc(bs2asm_str_carr, i*sizeof(int));
		bs2asm_str_max=i;
	}

//	t=bs2asm_malloc(strlen(str)+1);
	t=malloc(strlen(str)+1);
	strcpy(t, str);

	i=bs2asm_str_num++;
	bs2asm_str_varr[i]=t;
	bs2asm_str_carr[i]=bs2asm_str_hash[hi];
	bs2asm_str_hash[hi]=i;

	return(i);
}

char *bs2asm_strtab_i(int i)
	{ return(bs2asm_str_varr[i]); }

char *bs2asm_strdup(char *str)
{
	int i;

	if(!str)return(NULL);
	i=bs2asm_strdup_i(str);
	return(bs2asm_str_varr[i]);
}


void *bs2asm_ralloc(int sz)
{
	void *p;

	if(!bs2asm_rlcbuf)
	{
		bs2asm_rlcbuf=(byte *)malloc(1<<18);
		bs2asm_rlcpos=0;
	}

	if((bs2asm_rlcpos+sz)>=((1<<18)-4096))
		bs2asm_rlcpos=0;
	p=(void *)(bs2asm_rlcbuf+bs2asm_rlcpos);
	bs2asm_rlcpos=(bs2asm_rlcpos+sz+15)&(~15);

	return(p);
}

char *bs2asm_rstrdup(char *s)
{
	char *t;
	t=(char *)bs2asm_ralloc(strlen(s)+1);
	strcpy(t, s);
	return(t);
}

char **bs2asm_split(char *s)
{
	char **a, *t;
	int i;

	a=bs2asm_ralloc(64*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		if(*s=='#')break;
		if(*s==';')break;
		if((s[0]=='/') && (s[1]=='/'))
			break;

		t=bs2asm_ralloc(256);
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

char *bs2asm_rstradd(char *s0, char *s1)
{
	char *t;

	t=(char *)bs2asm_ralloc(strlen(s0)+strlen(s1)+1);
	strcpy(t, s0);
	strcat(t, s1);
	return(t);
}

char *bs2asm_rstradd2(char *s0, char *s1, char *s2)
{
	char *t;
	int i;

	i=strlen(s0)+strlen(s1)+strlen(s2);
	t=(char *)bs2asm_ralloc(i+1);
	strcpy(t, s0);
	strcat(t, s1);
	strcat(t, s2);
	return(t);
}

char *bs2asm_rstradd3(char *s0, char *s1, char *s2, char *s3)
{
	char *t;
	int i;

	i=strlen(s0)+strlen(s1)+strlen(s2)+strlen(s3);
	t=(char *)bs2asm_ralloc(i+1);
	strcpy(t, s0); strcat(t, s1);
	strcat(t, s2); strcat(t, s3);
	return(t);
}


struct bs2asm_ordtab_s
{
	char **tab;
	short *chain;
	short hash[256];
	int num;
};

void *bs2asm_neword(char **tab)
{
	struct bs2asm_ordtab_s *tmp;
	char *s;
	int i, j, n;
	
	for(n=0; tab[n]; n++);
	tmp=bs2asm_malloc2(sizeof(struct bs2asm_ordtab_s));
	tmp->tab=bs2asm_malloc2((n+1)*sizeof(char *));
	tmp->chain=bs2asm_malloc2((n+1)*sizeof(short));
	tmp->num=n;

	for(i=0; i<256; i++)
		tmp->hash[i]=-1;

	for(i=0; i<n; i++)
		tmp->tab[i]=bs2asm_strdup(tab[i]);

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

int bs2asm_strord(char *str, void *tab)
{
	struct bs2asm_ordtab_s *tmp;
	char *s;
	int i, j, n;

	tmp=(struct bs2asm_ordtab_s *)tab;

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


double BS2ASM_ParseNumber(char *str)
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
