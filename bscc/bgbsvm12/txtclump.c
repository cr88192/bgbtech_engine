#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES	65536

// char *in_txt;
char *out_c, *out_var;
char *eat_pfx;

char *lines[MAX_LINES];
int n_lines;

char *kralloc_buffer=NULL;
int kralloc_pos=0;

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
	char buf[1024], buf1[1024];
	char *in_fn[256];
	FILE *fd;
	char *s, *s0, *t, **a;
	int i, j, k, l, n, n_in;

	
	out_c=NULL;
	out_var=NULL;
//	in_txt=NULL;
	eat_pfx=NULL;
	n_in=0;
	
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "-o"))
			{
				out_c=strdup(argv[i+1]);
				i++;
				continue;
			}

			if(!strcmp(argv[i], "-v"))
			{
				out_var=strdup(argv[i+1]);
				i++;
				continue;
			}

			if(!strcmp(argv[i], "-eat"))
			{
				eat_pfx=strdup(argv[i+1]);
				i++;
				continue;
			}
			
			printf("unknown argument %s\n", argv[i]);
			break;
		}
		
		in_fn[n_in++]=strdup(argv[i]);
		continue;
	}

	if((i<argc) || !out_c || !out_var || (n_in<1))
	{
		printf("Usage: %s -o <out_c> -v <var_name> <in_txt*>\n", argv[0]);
		return(-1);
	}

	memset(lines, 0, MAX_LINES*sizeof(char *));
	n_lines=0;

	for(i=0; i<n_in; i++)
	{
		fd=fopen(in_fn[i], "rt");
		if(!fd)
		{
			printf("fail open %s\n", in_fn[i]);
			continue;
		}

		s=in_fn[i];
		if(eat_pfx && !strncmp(s, eat_pfx, strlen(eat_pfx)))
			s+=strlen(eat_pfx);
		
		sprintf(buf, "$CLUMP %s\\n", s);
		lines[n_lines++]=strdup(buf);

		while(!feof(fd))
		{
			memset(buf, 0, 1024);
			fgets(buf, 1023, fd);

			s=buf; t=buf1;
			
//			if(*s=='$')
//				{ s++; *t++='$'; *t++='$'; }
			
			while(*s)
			{
				if((*s<' ') || (*s>'~'))
				{
					if(*s=='\r')
						{ s++; *t++='\\'; *t++='r'; continue; }
					if(*s=='\n')
						{ s++; *t++='\\'; *t++='n'; continue; }
					if(*s=='\t')
						{ s++; *t++='\\'; *t++='t'; continue; }
					if(*s=='\v')
						{ s++; *t++='\\'; *t++='v'; continue; }
					if(*s=='\b')
						{ s++; *t++='\\'; *t++='b'; continue; }

					if((*s>0) && (*s<' '))
					{
						sprintf(t, "\\x%02X", (*s++)&0xFF);
						t+=4; continue;
					}

//					sprintf(t, "\\x%02X", (*s++)&0xFF);
					sprintf(t, "$x%02X", (*s++)&0xFF);
					t+=4; continue;
				}
				
				if(*s=='\"')
					{ s++; *t++='\\'; *t++='"'; continue; }
				if(*s=='\\')
					{ s++; *t++='\\'; *t++='\\'; continue; }

				if(*s=='$')
					{ s++; *t++='$'; *t++='$'; continue; }
				
				*t++=*s++;
			}
			*t++=0;

			lines[n_lines++]=strdup(buf1);
		}
		fclose(fd);

		sprintf(buf, "$CLUMPEOF\\n");
		lines[n_lines++]=strdup(buf);
	}

	fd=fopen(out_c, "wt");
	if(!fd)
	{
		printf("can't open output %s\n", out_c);
		return(-1);
	}

	fprintf(fd, "/* Autogenerated source */\n");

	fprintf(fd, "char *%s[]={\n\"", out_var);
	j=1; s=NULL;
	for(i=0; i<n_lines; i++)
	{
		s0=s;
		s=lines[i];

		if((s0 && ((*s0=='$') || (*s=='$'))) ||
			((j>1) && ((j+strlen(s))>72)))
		{
			fprintf(fd, "\",\n\"");
			j=1;
		}

		fprintf(fd, "%s", s);
		j+=strlen(s);
	}
	fprintf(fd, "\", (char *)0};\n\n");
	fclose(fd);

	return(0);
}
