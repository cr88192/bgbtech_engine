#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>

int main(int argc, char *argv[])
{
	long lc, offs, ofs, len, i, j, c;
	FILE *in;
	unsigned char buf[16], *s;

	in=NULL;
	offs=0;
	len=-1;
	for(i=1;i<argc;i++)
	{
//		printf("Arg %d: %s\n", i, argv[i]);

		if(argv[i][0]=='-')
		{
			switch(argv[i][1])
			{
				case 'o':
					ofs=atoi(argv[i+1]);
					i++;
					break;
				case 'l':
					len=atoi(argv[i+1]);
					i++;
					break;
				default:
					break;
			}
			continue;
		}
		if(!in)in=fopen(argv[i],"r+b");
	}

	if(!in)
	{
		printf("no input.\n");
		exit(-1);
	}

#if 0
	fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) & ~FNDELAY);
	fcntl(0, F_SETFL, fcntl (0, F_GETFL, 0) | FNDELAY);
#endif

	while(1)
	{
		fseek(in, offs, 0);
		ofs=offs;
		for(i=0; i<23; i++)
		{
			lc=fread(&buf, 1, ((16<len)||(len==-1))?16:len, in);
			printf("%08X", ofs);
			for(j=0;j<lc;j++)
			{
				if(!(j%4))printf(" ");
				printf("%02X ", buf[j]);
			}
			for(;j<16;j++)
			{
				if(!(j%4))printf(" ");
				printf("   ");
			}
			for(j=0;j<lc;j++)
			{
				if(buf[j]<' ')buf[j]='~';
				if(buf[j]>='\x7f')buf[j]='~';
				printf("%c", buf[j]);
			}
			ofs+=16;
			printf("\n");
			if(len>0)
			{
				len-=lc;
				if(len<=0)break;
			}
		}

		l0:
		printf(": ");
		fgets((char *)(&buf), 15, stdin);
		switch(buf[0])
		{
			case '\n':
				offs+=16*23;
				break;
			case 'h':
				printf("\\n		next page\n");
				printf("-, w		back 16 bytes\n");
				printf("s		forward 16 bytes\n");
				printf("p, a		prev page\n");
				printf("n, d		next page\n");
				printf("q		quit\n");
				printf("g, e <adr>	goto addr\n");
				printf("t <adr> <val>	set val at addr\n");
				goto l0;
				break;

			case '-':
			case 'w':
				offs-=16;
				break;
			case 's':
				offs+=16;
				break;
			case 'p':
			case 'a':
				offs-=16*23;
				break;
			case 'n':
			case 'd':
				offs+=16*23;
				break;
			case 'q':
				fclose(in);
				exit(0);
				break;
			case 'g':
			case 'e':
				s=buf;
				s++;
				j=0;
				while(*s<=' ')s++;
				while(*s>' ')
				{
					j<<=4;
					if(*s>='0' && *s<='9')j+=*s-'0';
					if(*s>='A' && *s<='F')j+=*s-'A'+10;
					if(*s>='a' && *s<='f')j+=*s-'a'+10;
					s++;
				}
				offs=j;
				i=strlen((char *)(&buf))-1;
				break;
			case 't':
				s=buf;
				s++;
				j=0;
				while(*s<=' ')s++;
				while(*s>' ')
				{
					j<<=4;
					if(*s>='0' && *s<='9')j+=*s-'0';
					if(*s>='A' && *s<='F')j+=*s-'A'+10;
					if(*s>='a' && *s<='f')j+=*s-'a'+10;
					s++;
				}

				fseek(in, j, 0);

				while(*s<=' ')s++;
				while(*s>' ')
				{
					c=*s;
					s++;
					if(c=='\\')
					{
						j=0;
						for(c=0; c<2; c++)
						{
							j<<=4;
							if(*s>='0' && *s<='9')j+=*s-'0';
							if(*s>='A' && *s<='F')j+=*s-'A'+10;
							if(*s>='a' && *s<='f')j+=*s-'a'+10;
							s++;
						}
						c=j;
					}
					fputc(c, in);
				}

				i=strlen((char *)(&buf))-1;
				break;

			default:
				break;
		}
	}
}
