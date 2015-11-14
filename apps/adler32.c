#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

unsigned int StrAdler32(byte *s)
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

inline unsigned int Data2Adler32(void *buf, int sz)
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

int UnAdler32_r(byte *buf, int c, int s1, int s2, int lim)
{
	int i, j, ds1, ds2;
	byte *s;
	s=" etaosnihrldcumgfwypb,.kv_\"\x0A\x0Djx'?0()Iz-/92q18:BS36D457GLMOT";

	if((s1==1) && (s2==0))
	{
		*buf=0;
		return(c);
	}

	if(c<lim)
	{
		for(i=0; s[i]; i++)
		{
			//s1=s1+(*s++);
			//s2=s1+s2;

			ds2=s2-s1;
			if(ds2<0)ds2+=65521;
			ds1=s1-s[i];
			if(ds1<0)ds1+=65521;

			j=UnAdler32_r(buf+1, c+1, ds1, ds2, lim);
			if(j>=0)
			{
				*buf=s[i];
				return(j);
			}
		}

/*		for(i=0; i<255; i++)
		{
			ds2=s2-s1;
			if(ds2<0)ds2+=65521;
			ds1=s1-i;
			if(ds1<0)ds1+=65521;

			j=UnAdler32_r(buf+1, c+1, ds1, ds2);
			if(j>=0)
			{
				*buf=i;
				return(j);
			}
		} */
	}

	*buf=0xFF;
	return(-1);
}

int UnAdler32_r2(byte *buf, unsigned int crc)
{
	byte tbuf[256];
	unsigned int c2;
	int i, j, k, l, ds1, ds2;
	int s1, s2;
	byte *s;
	s=" etaosnihrldcumgfwypb,.kv_\"\x0A\x0Djx'?0()Iz-/92q18:BS36D457GLMOT";

	s1=crc&0xFFFF;
	s2=(crc>>16)&0xFFFF;

	if((s1==1) && (s2==0))
	{
		*buf=0;
		return(0);
	}

	for(l=1; l<10; l++)
	{
		for(i=0; s[i]; i++)
		{
			ds2=s2-s1;
			if(ds2<0)ds2+=65521;
			ds1=s1-s[i];
			if(ds1<0)ds1+=65521;

			j=UnAdler32_r(buf+1, 1, ds1, ds2, l);
			if(j>=0)
			{
				*buf=s[i];

				//verify crc consistency
				for(k=0; k<j; k++)
					tbuf[k]=buf[(j-1)-k];
				tbuf[k]=0;

				c2=StrAdler32(tbuf);

				if(c2==crc)return(j);
			}
		}
	}

	*buf=0xFF;
	return(-1);
}

int UnAdler32(byte *buf, unsigned int crc)
{
	byte tbuf[256];
	byte *s;
	int i, c, s1, s2, ds1, ds2;

	s=" etaosnihrldcumgfwypb,.kv\"\x0A\x0Djx'?0()Iz-/92q18:BS36D457GLMOT";

	s1=crc&0xFFFF;
	s2=(crc>>16)&0xFFFF;
	c=0;

	c=UnAdler32_r2(tbuf, crc);
	if(c>=0)
	{
		for(i=0; i<c; i++)
			buf[i]=tbuf[(c-1)-i];
		buf[i]=0;

		return(c);
	}
	return(-1);

/*	while(((s1!=1) || (s2!=0)) && (c<255))
	{
		for(i=0; s[i]; i++)
		{
			ds2=s2-s1;
			if(ds2<0)ds2+=65521;
			ds1=s1-s[i];
			if(ds1<0)ds1+=65521;
		}
	}
*/
}

inline int proof_inc(char *s, int sz)
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

	if(!strcmp(argv[1], "-crack"))
	{
		sscanf(argv[2], "0x%X", &c);
		i=UnAdler32(buf, c);
		if(i>=0)
			printf("crack 0x%08X: %s\n", c, buf);
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
