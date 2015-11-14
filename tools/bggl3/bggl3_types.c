#include "bggl3.h"

char **bggl3_tyhash;
char **bggl3_symhash;
char **bggl3_kwhash;
char **bggl3_strhash;
byte *bggl3_strhash_cnt;

elem *bggl3_heap=NULL;
int bggl3_rover=0;
byte *bggl3_fixbuf;

int bggl3_id_int, bggl3_id_float, bggl3_id_cons, bggl3_id_null;
int bggl3_id_symbol, bggl3_id_keyword, bggl3_id_string;
int bggl3_id_complex, bggl3_id_fvector;

int BGGL3_IndexType(char *name)
{
	char *s;
	int i, j;

	s=name;
	j=*s++;
	while(1)
	{
		i=j&0xFFF;
		if(!i)i++;
		if(!bggl3_tyhash[i])
		{
			bggl3_tyhash[i]=strdup(name);
			return(i);
		}

		if(!strcmp(bggl3_tyhash[i], name))
			return(i);

		if(!*s)break;
		j=(j>>12)^(j<<3)^(*s++);
	}

	i=0xFFF;
	while((i>0) && bggl3_tyhash[i])
	{
		if(!strcmp(bggl3_tyhash[i], name))
			return(i);
		i--;
	}
	if(!i)
	{
		printf("BGGL3_IndexType: Hash Full\n");
		return(i);
	}

	bggl3_tyhash[i]=strdup(name);
	return(i);
}

void BGGL3_InitHeap()
{
	if(!bggl3_heap)
	{
		bggl3_tyhash=malloc(4096*sizeof(char *));
		memset(bggl3_tyhash, 0, 4096*sizeof(char *));
		bggl3_fixbuf=malloc(65536);

		bggl3_id_int=BGGL3_IndexType("int");
		bggl3_id_float=BGGL3_IndexType("float");
		bggl3_id_cons=BGGL3_IndexType("cons");
		bggl3_id_null=BGGL3_IndexType("null");
		bggl3_id_symbol=BGGL3_IndexType("symbol");
		bggl3_id_keyword=BGGL3_IndexType("keyword");
		bggl3_id_string=BGGL3_IndexType("string");
		bggl3_id_complex=BGGL3_IndexType("complex");
		bggl3_id_fvector=BGGL3_IndexType("fvector");

		bggl3_heap=malloc((1<<22)*sizeof(elem));
		bggl3_rover=0;
	}
}

elem BGGL3_Alloc(char *type, int sz)
{
	elem *p;
	int i, j;

	BGGL3_InitHeap();

	i=(sz+sizeof(elem)-1)/sizeof(elem);
	p=bggl3_heap+bggl3_rover;
	bggl3_rover+=i+1;

	if(bggl3_rover>(1<<22))
	{
		printf("Heap Overflow\n");
		exit(-1);
	}

	j=BGGL3_IndexType(type);
	*p=(elem)((sz<<12)|j);

	return((elem)(p+1));
}

int BGGL3_GetTypeID(elem p)
{
	elem *pa;
	int i;

	if(!p)return(bggl3_id_null);
	if((((byte *)p)>=bggl3_fixbuf) &&
		(((byte *)p)<(bggl3_fixbuf+65536)))
			return(bggl3_id_int);

	pa=((elem *)p)-1;
	i=((int)(*pa))&0xFFF;
	return(i);
}

char *BGGL3_GetType(elem p)
{
	elem *pa;
	int i;

	if(!p)return("null");
	if((((byte *)p)>=bggl3_fixbuf) &&
		(((byte *)p)<(bggl3_fixbuf+65536)))
			return("int");

	pa=((elem *)p)-1;
	i=((int)(*pa))&0xFFF;
	return(bggl3_tyhash[i]);
}

int BGGL3_GetSize(elem p)
{
	elem *pa;
	int i;

	pa=((elem *)p)-1;
	i=((int)(*pa))>>12;
	return(i);
}


char *BGGL3_AllocSymbol(char *s)
{
	char *t;

	t=BGGL3_Alloc("symbol", strlen(s)+1);
	strcpy(t, s);
	return(t);
}

char *BGGL3_AllocKeyword(char *s)
{
	char *t;

	t=BGGL3_Alloc("keyword", strlen(s)+1);
	strcpy(t, s);
	return(t);
}

char *BGGL3_AllocString(char *s)
{
	char *t;

	t=BGGL3_Alloc("string", strlen(s)+1);
	strcpy(t, s);
	return(t);
}

elem BGGL3_HashSymbol(char *name)
{
	char *s;
	int i, j;

	if(!bggl3_symhash)
	{
		bggl3_symhash=malloc(16384*sizeof(char *));
		memset(bggl3_symhash, 0, 16384*sizeof(char *));
		bggl3_symhash[0]=BGGL3_AllocSymbol("");
	}
	if(!(*name))return(bggl3_symhash[0]);

	s=name;
	j=*s++;
	while(1)
	{
		i=j&16383;
		if(!bggl3_symhash[i])
		{
			bggl3_symhash[i]=BGGL3_AllocSymbol(name);
			return(bggl3_symhash[i]);
		}

		if(!strcmp(bggl3_symhash[i], name))
			return(bggl3_symhash[i]);

		if(!*s)break;
		j=(j>>12)^(j<<5)^(*s++);
	}

	i=16383;
	while((i>0) && bggl3_symhash[i])
	{
		if(!strcmp(bggl3_symhash[i], name))
			return(bggl3_symhash[i]);
		i--;
	}
	if(!i)
	{
		printf("BGGL3_HashSymbol: Hash Full\n");
		return(NULL);
	}

	bggl3_symhash[i]=BGGL3_AllocSymbol(name);
	return(bggl3_symhash[i]);
}

elem BGGL3_HashKeyword(char *name)
{
	char *s;
	int i, j;

	if(!bggl3_kwhash)
	{
		bggl3_kwhash=malloc(16384*sizeof(char *));
		memset(bggl3_kwhash, 0, 16384*sizeof(char *));
		bggl3_kwhash[0]=BGGL3_AllocKeyword("");
	}
	if(!(*name))return(bggl3_kwhash[0]);

	s=name;
	j=*s++;
	while(1)
	{
		i=j&16383;
		if(!bggl3_kwhash[i])
		{
			bggl3_kwhash[i]=BGGL3_AllocKeyword(name);
			return(bggl3_kwhash[i]);
		}

		if(!strcmp(bggl3_kwhash[i], name))
			return(bggl3_kwhash[i]);

		if(!*s)break;
		j=(j>>12)^(j<<5)^(*s++);
	}

	i=16383;
	while((i>0) && bggl3_kwhash[i])
	{
		if(!strcmp(bggl3_kwhash[i], name))
			return(bggl3_kwhash[i]);
		i--;
	}
	if(!i)
	{
		printf("BGGL3_HashKeyword: Hash Full\n");
		return(NULL);
	}

	bggl3_kwhash[i]=BGGL3_AllocKeyword(name);
	return(bggl3_kwhash[i]);
}

elem BGGL3_HashString(char *name)
{
	char *s;
	int i, j;

	if(!bggl3_strhash)
	{
		bggl3_strhash=malloc(65536*sizeof(char *));
		bggl3_strhash_cnt=malloc(65536);
		memset(bggl3_strhash, 0, 65536*sizeof(char *));
		bggl3_strhash[0]=BGGL3_AllocString("");
	}
	if(!(*name))return(bggl3_strhash[0]);

	s=name;
	j=*s++;
	while(1)
	{
		i=j&65535;
		if(!bggl3_strhash[i])
		{
			bggl3_strhash[i]=BGGL3_AllocString(name);
			bggl3_strhash_cnt[i]=16;
			return(bggl3_strhash[i]);
		}

		if(!strcmp(bggl3_strhash[i], name))
		{
			if(bggl3_strhash_cnt[i]<255)
				bggl3_strhash_cnt[i]++;
			return(bggl3_strhash[i]);
		}
		bggl3_strhash_cnt[i]--;
		if(!bggl3_strhash_cnt[i])
		{
			bggl3_strhash[i]=BGGL3_AllocString(name);
			bggl3_strhash_cnt[i]=16;
			return(bggl3_strhash[i]);
		}

		if(!*s)break;
		j=(j>>12)^(j<<5)^(*s++);
	}

	s=BGGL3_AllocString(name);
	return(s);
}

elem BGGL3_CONS(elem a, elem b)
{
	elem *c;

	c=(elem *)BGGL3_Alloc("cons", 2*sizeof(elem));
	c[0]=a;
	c[1]=b;

	return((elem)c);
}

elem BGGL3_CAR(elem a)
{
	return(((elem *)a)[0]);
}

elem BGGL3_CDR(elem a)
{
	return(((elem *)a)[1]);
}

elem BGGL3_CADR(elem a)
{
	return(BGGL3_CAR(BGGL3_CDR(a)));
}

elem BGGL3_CDDR(elem a)
{
	return(BGGL3_CDR(BGGL3_CDR(a)));
}

elem BGGL3_CADDR(elem a)
{
	return(BGGL3_CAR(BGGL3_CDDR(a)));
}

elem BGGL3_CDDDR(elem a)
{
	return(BGGL3_CDR(BGGL3_CDDR(a)));
}

elem BGGL3_CADDDR(elem a)
{
	return(BGGL3_CAR(BGGL3_CDDDR(a)));
}

elem BGGL3_CDDDDR(elem a)
{
	return(BGGL3_CDR(BGGL3_CDDDR(a)));
}

elem BGGL3_CADDDDR(elem a)
{
	return(BGGL3_CAR(BGGL3_CDDDDR(a)));
}

elem BGGL3_CDDDDDR(elem a)
{
	return(BGGL3_CDR(BGGL3_CDDDDR(a)));
}

elem BGGL3_CADDDDDR(elem a)
{
	return(BGGL3_CAR(BGGL3_CDDDDDR(a)));
}

elem BGGL3_CDDDDDDR(elem a)
{
	return(BGGL3_CDR(BGGL3_CDDDDDR(a)));
}


void BGGL3_CARN(elem a, elem b)
{
	((elem *)a)[0]=b;
}

void BGGL3_CDRN(elem a, elem b)
{
	((elem *)a)[1]=b;
}

elem BGGL3_INT(int i)
{
	elem *c;

	if((i>=-32768) && (i<=32767))
		return((elem)(bggl3_fixbuf+i+32768));
	c=(elem *)BGGL3_Alloc("int", sizeof(int));
	c[0]=(elem)i;
	return((elem)c);
}

elem BGGL3_FLOAT(float f)
{
	elem *c;
	int i;

	i=f;
	if((i==f) && (i>=-32768) && (i<=32767))
		return((elem)(bggl3_fixbuf+i+32768));

	i=*(int *)(&f);
	c=(elem *)BGGL3_Alloc("float", sizeof(int));
	c[0]=(elem)i;
	return((elem)c);
}

elem BGGL3_COMPLEX(float f, float g)
{
	float *c;
	int i, j;

	if(g==0)return(BGGL3_FLOAT(f));

	c=(float *)BGGL3_Alloc("complex", 2*sizeof(float));
	c[0]=f;
	c[1]=g;

	return((elem)c);
}

int BGGL3_TOINT(elem a)
{
	int i;

	if(!a)return(0);
	if((((byte *)a)>=bggl3_fixbuf) &&
		(((byte *)a)<(bggl3_fixbuf+65536)))
			return(((byte *)a)-(bggl3_fixbuf+32768));
	i=BGGL3_GetTypeID(a);

	if(i==bggl3_id_int)return((int)(*(elem *)a));
	if(i==bggl3_id_float)
	{
		i=(int)(*(elem *)a);
		i=(int)(*(float *)(&i));
		return(i);
	}
	if(i==bggl3_id_complex)
		return((int)(((float *)a)[0]));
	return(-1);
}

float BGGL3_TOFLOAT(elem a)
{
	int i;

	if(!a)return(0);
	if((((byte *)a)>=bggl3_fixbuf) &&
		(((byte *)a)<(bggl3_fixbuf+65536)))
			return(((byte *)a)-(bggl3_fixbuf+32768));
	i=BGGL3_GetTypeID(a);

	if(i==bggl3_id_int)return((int)(*(elem *)a));
	if(i==bggl3_id_float)
	{
		i=(int)(*(elem *)a);
		return(*(float *)(&i));
	}
	if(i==bggl3_id_complex)
		return(((float *)a)[0]);
	return(0);
}

float BGGL3_GETREAL(elem a)
{
	int i;

	i=BGGL3_GetTypeID(a);
	if(i==bggl3_id_int)return(BGGL3_TOINT(a));
	if(i==bggl3_id_float)return(BGGL3_TOFLOAT(a));
	if(i==bggl3_id_complex)
		return(((float *)a)[0]);
	return(0);
}

float BGGL3_GETIMAG(elem a)
{
	int i;

	i=BGGL3_GetTypeID(a);
	if(i==bggl3_id_complex)
		return(((float *)a)[1]);
	return(0);
}


void BGGL3_Print(elem a)
{
	elem c;
	float f, g, *fa;
	int i, j, n;

	if(!a)
	{
		printf("()");
		return;
	}

	i=BGGL3_GetTypeID(a);
	if(i==bggl3_id_int)
	{
		printf("%d", BGGL3_TOINT(a));
		return;
	}
	if(i==bggl3_id_float)
	{
		f=BGGL3_TOFLOAT(a);
//		if(fabs(f)<(1.0/65536.0))f=0;
		if(fabs(f)<(1.0/1000000000000.0))f=0;
		printf("%g", f);
		return;
	}

	if(i==bggl3_id_complex)
	{
		f=BGGL3_GETREAL(a);
		g=BGGL3_GETIMAG(a);

//		if(fabs(f)<(1.0/65536.0))f=0;
//		if(fabs(g)<(1.0/65536.0))g=0;
		if(fabs(f)<(1.0/1000000000000.0))f=0;
		if(fabs(g)<(1.0/1000000000000.0))g=0;

		if(g==0)printf("%g", f);
			else if(f==0)printf("%gi", g);
			else if(g>0)printf("%g+%gi", f, g);
			else printf("%g%gi", f, g);
		return;
	}

	if(i==bggl3_id_symbol)
	{
		printf("%s", (char *)a);
		return;
	}
	if(i==bggl3_id_keyword)
	{
		printf("%s:", (char *)a);
		return;
	}
	if(i==bggl3_id_string)
	{
		printf("\"%s\"", (char *)a);
		return;
	}

	if(i==bggl3_id_cons)
	{
		printf("{");
		c=a;
		while(BGGL3_GetTypeID(c)==bggl3_id_cons)
		{
			BGGL3_Print(BGGL3_CAR(c));
			c=BGGL3_CDR(c);
			if(c)printf(" ");
		}
		if(c)
		{
			printf(". ");
			BGGL3_Print(c);
		}
		printf("}");
		return;
	}

	if(i==bggl3_id_fvector)
	{
		fa=(float *)a;
		n=BGGL3_GetSize(a)/sizeof(float);

		printf("{");
		for(j=0; j<n; j++)
		{
			printf("%g", fa[j]);
			if((j+1)<n)printf(" ");
		}
		printf("}");
		return;
	}

	printf("<%s:%p>", BGGL3_GetType(a), a);
}

elem BGGL3_LIST2(elem a, elem b)
{
	return(BGGL3_CONS(a, BGGL3_CONS(b, NULL)));
}

elem BGGL3_LIST3(elem a, elem b, elem c)
{
	return(BGGL3_CONS(a, BGGL3_CONS(b, BGGL3_CONS(c, NULL))));
}

int BGGL3_ListLength(elem a)
{
	int i;

	i=0;
	while(BGGL3_CONSP(a))
	{
		i++;
		a=BGGL3_CDR(a);
	}

	return(i);
}

int BGGL3_CONSP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_cons);
}

int BGGL3_SYMBOLP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_symbol);
}

int BGGL3_KEYWORDP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_keyword);
}

int BGGL3_STRINGP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_string);
}

int BGGL3_INTP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_int);
}

int BGGL3_FLOATP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_float);
}

int BGGL3_COMPLEXP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_complex);
}

int BGGL3_NUMBERP(elem a)
{
	int i;

	i=BGGL3_GetTypeID(a);
	return((i==bggl3_id_int) || (i==bggl3_id_float) ||
		(i==bggl3_id_complex));
}

int BGGL3_FVECTORP(elem a)
{
	return(BGGL3_GetTypeID(a)==bggl3_id_fvector);
}

elem BGGL3_List2FVector(elem a)
{
	float *fa;
	elem c;
	int i;

	c=a; i=0;
	while(BGGL3_CONSP(c))
	{
		i++;
		c=BGGL3_CDR(c);
	}

	fa=(float *)BGGL3_Alloc("fvector", i*sizeof(float));
	c=a; i=0;
	while(BGGL3_CONSP(c))
	{
		fa[i]=BGGL3_TOFLOAT(BGGL3_CAR(c));
		i++;
		c=BGGL3_CDR(c);
	}

	return((elem)fa);
}

elem BGGL3_MakeFVector(float *va, int n)
{
	float *fa;
	int i;

	fa=(float *)BGGL3_Alloc("fvector", n*sizeof(float));
	for(i=0; i<n; i++)fa[i]=va[i];

	return((elem)fa);
}

void BGGL3_ToVector(elem a, float *va)
{
	float *fa;
	int i, n;

	fa=(float *)a;
	n=BGGL3_GetSize(a)/sizeof(float);

	for(i=0; i<n; i++)va[i]=fa[i];
}
