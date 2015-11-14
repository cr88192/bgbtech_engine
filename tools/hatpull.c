#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

char *in_txt;
char *out_c;
char *out_h;
char *out_suf, *out_suf2;

char *ops[16384];
char *args[16384];
char *sfis[16384];

char *kralloc_buffer=NULL;
int kralloc_pos=0;

int basm_genseed()
{
	int i, j, k, l;

	l=clock(); j=0; k=l;
	while(1)
	{
		i=clock();
		if(((i-l)>=10) || (i<l))break;

		k=(k>>16)^(k<<1)^i;
		j++;
	}

//	printf("seed %08X, cnt %d\n", k, j);
	return(k);
}

void *kralloc(int sz)
{
	char *s;

	if(!kralloc_buffer)kralloc_buffer=malloc(262144);
	if((kralloc_pos+sz)>=262144)kralloc_pos=0;

	s=kralloc_buffer+kralloc_pos;
	kralloc_pos+=sz;
	return((void *)s);
}

char **ksplit(char *s)
{
	char **a, *t;
	int i;

	a=kralloc(64*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		if(*s=='#')break;
		if(*s==';')break;
		if((s[0]=='/') && (s[1]=='/'))
			break;

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

		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
	}
	a[i]=NULL;

	return(a);
}

int is_sfi(char *s)
{
	while(*s)
	{
		if(*s=='=')return(1);
		s++;
	}
	return(0);
}

int main(int argc, char *argv[])
{
	char buf[256];
	FILE *fd;
	char *s, *t, **a;
	int i, j, k, l, n, cn;

//	in_txt="hatpull_words.txt";

	in_txt=NULL; cn=5;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-n"))
			{
				cn=atoi(argv[i+1]);
				i++;
				continue;
			}
			continue;
		}
		
		in_txt=argv[i];
	}

	if(!in_txt)
		in_txt="hatpull_words.txt";

	memset(ops, 0, 16384*sizeof(char *));
	memset(args, 0, 16384*sizeof(char *));
	memset(sfis, 0, 16384*sizeof(char *));

	fd=fopen(in_txt, "rt");
	if(!fd)
	{
		printf("fail open %s\n", in_txt);
		return(-1);
	}

	n=0;
	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);

		s=buf;
		while(*s && (*s<=' '))s++;
		if(!*s)continue;
		if(*s==';')continue;
		if(*s=='#')continue;
		if(*s=='/')continue;

		a=ksplit(s);
		if(!a[0])continue;

		i=0;
		while(ops[i] && (i<n))i++;

		ops[i]=strdup(a[0]);

#if 0
		if(a[2])
		{
			if(strcmp(a[2], "-"))
			{
				if(!is_sfi(a[2]))
				{
					args[i]=strdup(a[2]);

					if(a[3] && strcmp(a[3], "-"))
						sfis[i]=strdup(a[3]);
				}else sfis[i]=strdup(a[2]);
			}else
			{
				if(a[3] && strcmp(a[3], "-"))
					sfis[i]=strdup(a[3]);
			}
		}
#endif

		if(i>=n)n=i+1;
	}
	fclose(fd);

	i=time(NULL)^basm_genseed();
	srand(i);

	for(i=0; i<cn; i++)
	{
		j=rand()%n;
		printf("%d: %s\n", j, ops[j]);
	}

	return(0);
}
