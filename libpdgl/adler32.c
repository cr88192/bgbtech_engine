#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* test comment */

typedef unsigned char byte;

unsigned int StrAdler32(char *s)
{
	int c, s1, s2;

	s1=1;
	s2=0;
	while(*s)
	{
		s1=(s1+(*s++))%65521;
		s2=(s1+s2)%65521;
	}
	return((s2<<16)+s1);
}

unsigned int DataAdler32(void *buf, int sz, unsigned int lcrc)
{
	byte *s;
	int i, c, s1, s2;

	s=buf;
	s1=lcrc&0xFFFF;
	s2=(lcrc>>16)&0xFFFF;
	for(i=0; i<sz; i++)
	{
		s1=(s1+(s[i]))%65521;
		s2=(s1+s2)%65521;
	}
	return((s2<<16)+s1);
}

unsigned int Data2Adler32(void *buf, int sz)
{
	byte *s;
	int i, c, s1, s2;

	s=buf;
	s1=1;
	s2=0;
	for(i=0; i<sz; i++)
	{
		s1=(s1+(*s++))%65521;
		s2=(s1+s2)%65521;
	}
	return((s2<<16)+s1);
}

int proof_inc(char *s, int sz)
{
	while(*s)
	{
		*s=*s+1;
		if(*s<0x7F)break;
		*s=0x20;
		s++;
	}
}

int proof_perm(int sz1, int sz2)
{
	char buf1[64], buf2[64];
	int i, j;
	int c1, c2;

	printf("proof %d %d\n", sz1, sz2);

	memset(buf1, 0, 64);
	memset(buf2, 0, 64);

	for(i=0; i<sz1; i++)buf1[i]=0x20;

	while(buf1[sz1-1]<0x7E)
	{
		for(i=0; i<sz2; i++)buf2[i]=0x20;
		while(buf2[sz2-1]<0x7E)
		{
			c1=Data2Adler32(buf1, sz1);
			c2=Data2Adler32(buf2, sz2);
			if(c1==c2)
			{
				if(sz1==sz2)
				{
					j=0;
					for(i=0; i<sz1; i++)
						if(buf1[i]!=buf2[i])
							j=1;
				}else j=1;
				if(j)printf("FM: [%s] [%s]\n", buf1, buf2);
			}
			proof_inc(buf2, sz2);
		}
		proof_inc(buf1, sz1);
	}
}

int proof()
{
	int i, j;

	for(i=1; i<10; i++)
		proof_perm(i, i);

//	for(i=1; i<10; i++)for(j=i; j<10; j++)
//		proof_perm(i, j);
}

char *upcase(char *s)
{
	char buf[64], c;
	char *t;

	t=buf;
	while(*s)
	{
		c=*s++;
		if((c>='a') && (c<='z'))c=c-'a'+'A';
		*t++=c;
	}
	*t++=0;

	return(strdup(buf));
}

int main(int argc, char *argv[])
{
	FILE *infd, *outfd;
	char buf[256];
	char *s;
	int i, c;

	if(argc<=1)
	{
		c=1;
		while(!feof(stdin))
		{
			i=fread(buf, 1, 256, stdin);
			c=DataAdler32(buf, i, c);
		}
		printf("0x%08X\n", c);
		return(0);
	}

	if(!strcmp(argv[1], "-proof"))
	{
		proof();
		return(0);
	}

	if(!strcmp(argv[1], "-header"))
	{
		infd=fopen(argv[2], "rt");
		outfd=fopen(argv[3], "wt");

		while(!feof(infd))
		{
			memset(buf, 0, 256);
			fgets(buf, 255, infd);
			s=buf;
			while(*s && (*s>' '))s++;
			*s=0;

			if(!buf[0])continue;
			if(buf[0]=='#')continue;
			if((buf[0]=='/') && (buf[1]=='/'))
				continue;

			fprintf(outfd, "#define TYCRC_%s\t0x%08X\n",
				upcase(buf), StrAdler32(buf));
		}
		return(0);
	}

	for(i=1; i<argc; i++)
	{
		printf("CRC for '%s': 0x%08X\n", argv[i],
			StrAdler32(argv[i]));
	}
	return(0);
}
