/*
Written 2007-2009 by Brendan G Bohannon
This code is re-assigned into the public domain.
*/

/*
Goals:
Parse and process strings of assembler commands;
Parsed strings are sent to the underlying procedural assembler.

Syntax:
'name:',	label;
'opcode [arg[, arg]]'

arg:
immed;
reg;
mem.

mem:
	[reg]
	[reg+reg]
	[reg+reg*scale]
	[reg+reg+disp]
	[reg+reg*scale+disp]
	[reg+disp]

	[label]
	[label+reg]
	[label+reg*scale]
	[label+reg+disp]
	[label+reg*scale+disp]
	[label+disp]

	[addr]
	[addr+reg]
	[addr+reg*scale]
	[addr+reg+disp]
	[addr+reg*scale+disp]
	[addr+disp]


	[<reg|label|addr|const>(+reg(*scale))(+<disp|const>)]
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgbasm_asm.h>


extern char *basm_ops[];
extern int basm_opidx[];
extern int basm_opnums[];

extern char basm_opcat[];
extern char *basm_opdecl[];

extern char *basm_regs[];
extern int basm_regidx[];


char *basm_strtab;	//strings table
char *basm_strtabe;	//strings table end

char *basm_strhash[4096];

char **basm_str_varr;
int *basm_str_carr;
int basm_str_num;
int basm_str_max;
int basm_str_hash[4096];

#if 0
BASM_API char *basm_strdup(char *str)
{
	char *s, *t;
	int i, hi;

#if 1
	if(!basm_strtab)
	{
		basm_strtab=(char *)malloc(1<<22);
		memset(basm_strtab, 0, 1<<22);
		basm_strtabe=basm_strtab+1;
		for(i=0; i<4096; i++)basm_strhash[i]=NULL;
	}
#endif

	if((str>=basm_strtab) && (str<basm_strtabe))
		return(str);

	if(!str)return(NULL);
	if(!*str)return(basm_strtab);


	s=str; i=0;
	while(*s)i=(i*251)+(*s++);
	hi=((i*251)>>8)&0xFFF;

//	t=basm_strhash[hi];
//	if(t && !strcmp(t, str))return(t);

	if(basm_strhash[hi] && !strcmp(basm_strhash[hi], str))
		return(basm_strhash[hi]);

	s=basm_strtab+1;
	while(*s)
	{
		if(!strcmp(s, str))
			return(s);
		s+=strlen(s)+1;
	}

	strcpy(s, str);
	basm_strtabe=s+strlen(s)+1;
	basm_strhash[hi]=s;

	i=basm_strtabe-basm_strtab;
	if(i>=(1<<19))
	{
		printf("basm_strdup: string table size limit approach\n");
	}

	return(s);
}

int basm_strdup_i(char *str)
{
	char *s;
	s=basm_strdup(str);
	return(s-basm_strtab);
}

char *basm_strtab_i(int str)
{
	return(basm_strtab+str);
}
#endif


#if 1
char *basm_strdup_alloc(char *str)
{
	char *s, *t;
	int i, hi;

	if(!basm_strtab)
	{
		basm_strtab=(char *)malloc(1<<20);
		basm_strtabe=basm_strtab+1;
	}

	if((str>=basm_strtab) && (str<basm_strtabe))
		return(str);

	if(!str)return(NULL);
	if(!*str)return(basm_strtab);

	s=basm_strtabe;
	strcpy(s, str);
	basm_strtabe=s+strlen(s)+1;

	i=basm_strtabe-basm_strtab;
	if(i>=((1<<20)-4096))
	{
		//new string table
		basm_strtab=(char *)malloc(1<<20);
		basm_strtabe=basm_strtab+1;
	}

	return(s);
}

int basm_strdup_i(char *str)
{
	char *s, *t;
	int i, hi;

	if(!str)return(0);

#if 1
	if(!basm_str_varr)
	{
		for(i=0; i<4096; i++)basm_str_hash[i]=0;
		basm_str_varr=malloc(16384*sizeof(char *));
		basm_str_carr=malloc(16384*sizeof(int));

		basm_str_varr[0]=NULL;
		basm_str_carr[0]=0;
		basm_str_num=1;
		basm_str_max=16384;
	}
#endif

	s=str; hi=0;
	while(*s)hi=(hi*251)+(*s++);
	hi&=4095;

	i=basm_str_hash[hi];
	while(i)
	{
		if(!strcmp(basm_str_varr[i], str))
			return(i);
		i=basm_str_carr[i];
	}

	if((basm_str_num+1)>=basm_str_max)
	{
		i=basm_str_max+(basm_str_max>>1);
		basm_str_varr=realloc(basm_str_varr, i*sizeof(char *));
		basm_str_carr=realloc(basm_str_carr, i*sizeof(int));
		basm_str_max=i;
	}

	t=basm_strdup_alloc(str);

	i=basm_str_num++;
	basm_str_varr[i]=t;
	basm_str_carr[i]=basm_str_hash[hi];
	basm_str_hash[hi]=i;

	return(i);
}

char *basm_strtab_i(int i)
	{ return(basm_str_varr[i]); }

BASM_API char *basm_strdup(char *str)
{
	int i;

	if(!str)return(NULL);
	i=basm_strdup_i(str);
	return(basm_str_varr[i]);
}
#endif

void *basm_ralloc(int sz)
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

char *basm_rstrdup(char *str)
{
	char *t;
	t=basm_ralloc(strlen(str)+1);
	strcpy(t, str);
	return(t);
}

int basm_strisreal(char *str)
{
	char *s;

	s=str;
	if((s[0]=='0') && (s[1]=='x'))
		return(0);
	while(*s)
	{
		if(*s=='.')break;
		if(*s=='e')break;
		if(*s=='E')break;
		s++;
	}
	if(*s)return(1);
	return(0);
}

u32 basm_atof_u32(char *str)
{
	float f;
	f=atof(str);
	return(*(u32 *)(&f));
}

u64 basm_atof_u64(char *str)
{
	double f;
	f=atof(str);
	return(*(u64 *)(&f));
}

u32 basm_atof_u32_sg(char *str, int sg)
{
	float f;
	f=atof(str)*sg;
	return(*(u32 *)(&f));
}

u64 basm_atof_u64_sg(char *str, int sg)
{
	double f;
	f=atof(str)*sg;
	return(*(u64 *)(&f));
}


s64 basm_atoi(char *str)
{
	long long l;
	char *s;
	int i, sg;

	s=str;
	if((s[0]=='0') && (s[1]=='x'))
	{
		l=0; s+=2;
		while(*s)
		{
			l*=16;
			if((*s>='0') && (*s<='9'))l+=*s-'0';
			if((*s>='A') && (*s<='F'))l+=*s-'A'+10;
			if((*s>='a') && (*s<='f'))l+=*s-'a'+10;
			s++;
		}

		return(l);
	}

	if(s[0]=='0')
	{
		l=0; s++;
		while(*s)
		{
			l*=8;
			if((*s>='0') && (*s<='7'))l+=*s-'0';
				else break;
			s++;
		}

		return(l);
	}

	while(s[1])s++;
	if((*s=='b') || (*s=='B'))
	{
		l=0; s=str;
		while(*s)
		{
			l*=2;
			if((*s>='0') && (*s<='1'))l+=*s-'0';
				else break;
			s++;
		}

		return(l);
	}

	if((*s=='h') || (*s=='H'))
	{
		l=0; s=str;
		while(*s)
		{
			l*=16;
			if((*s>='0') && (*s<='9'))
				{ l+=*s-'0'; }
			else if((*s>='A') && (*s<='F'))
				{ l+=*s-'A'+10; }
			else if((*s>='a') && (*s<='f'))
				{ l+=*s-'a'+10; }
			else break;
			s++;
		}

		return(l);
	}


	s=str;
	l=0; sg=0;
	if(*s=='-') { sg=1; s++; }
	while(*s)
	{
		l*=10;
		if((*s>='0') && (*s<='9')) { l+=*s-'0'; }
			else break;
		s++;
	}
	if(sg)l=-l;

	return(l);
}

int basm_stricmp(char *s1, char *s2)
{
	int i, j;

	if(s1==s2)return(0);

	while(*s1 && *s2)
	{
		i=((*s1>='A') && (*s1<='Z'))?(*s1-'A'+'a'):(*s1);
		j=((*s2>='A') && (*s2<='Z'))?(*s2-'A'+'a'):(*s2);
		if(i<j)return(-1);
		if(i>j)return(1);
		s1++; s2++;
	}

	if(*s1)return(1);
	if(*s2)return(-1);
	return(0);
}


char *BASM_Parse_EatWhiteOnly(char *s)
{
	while(*s && (*s<=' '))s++;
	return(s);
}

char *BASM_Parse_EatWhite(char *s)
{
	char *is;
	int i;

	is=s;
	while(*s && (*s<=' '))s++;

#if 0
	if((s!=is) && (s[0]==';'))
	{
		while(*s && (*s!='\n'))s++;
		s=BASM_Parse_EatWhite(s);
		return(s);
	}
#endif

	if((s[0]=='/') && (s[1]=='/'))
	{
		while(*s && (*s!='\n'))s++;
		s=BASM_Parse_EatWhite(s);
		return(s);
	}
	if((s[0]=='/') && (s[1]=='*'))
	{
		s+=2;
		i=1;
		while(*s && i)
		{
			if((s[0]=='/') && (s[1]=='*'))
			{
				s+=2;
				i++;
				continue;
			}
			if((s[0]=='*') && (s[1]=='/'))
			{
				s+=2;
				i--;
				continue;
			}
			s++;
		}
		s=BASM_Parse_EatWhite(s);
		return(s);
	}

	return(s);
}

int BASM_Parse_IsWhiteOnly(char *s)
{
	while((*s==' ') || (*s=='\t'))
		s++;

	if((s[0]=='/') && (s[1]=='/'))return(0);
	if((s[0]=='/') && (s[1]=='*'))return(0);
	if((s[0]=='*') && (s[1]=='/'))return(0);

	if(*s>' ')return(1);
	if(*s==' ')return(1);
	if(*s=='\t')return(1);

	return(0);
}

int BASM_Parse_IsLineBreak(char *se)
{
	char *s, *b, *t;
	int i, j;

	s=se;
	while(*s && (*s<=' '))
	{
		if(*s=='\r')return(1);
		if(*s=='\n')return(1);
		s++;
	}
	if(!*s)return(1);

	return(0);
}


int BASM_Parse_OpChar(int c)
{
	int i;

	switch(c)
	{
	case '~': case '!': case '@': case '#':
	case '$': case '%': case '^': case '&':
	case '*': case '-': case '=': case '+':
	case '\\': case '|':
	case ':':

	case '<': case '>': case '?': case '/':
		i=1;
		break;

	default:
		i=0;
		break;
	}
	return(i);
}

int BASM_Parse_PeekChar(char **str)
{
	unsigned char *s;
	int i;

	s=(unsigned char *)(*str);
	i=*s++;

	if(i>=0xFC)
	{
		i=(i&1)<<30;
		i+=((*s++)&63)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF8)
	{
		i=(i&3)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF0)
	{
		i=(i&7)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xE0)
	{
		i=(i&15)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xC0)
	{
		i=(i&31)<<6;
		i+=(*s++)&63;
	}else if(i>=0x80)
	{
//		continue;
	}

	return(i);
}

int BASM_Parse_ParseChar(char **str)
{
	unsigned char *s;
	int i;

	s=(unsigned char *)(*str);
	i=*s++;

	if(i>=0xFC)
	{
		i=(i&1)<<30;
		i+=((*s++)&63)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF8)
	{
		i=(i&3)<<24;
		i+=((*s++)&63)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xF0)
	{
		i=(i&7)<<18;
		i+=((*s++)&63)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xE0)
	{
		i=(i&15)<<12;
		i+=((*s++)&63)<<6;
		i+=(*s++)&63;
	}else if(i>=0xC0)
	{
		i=(i&31)<<6;
		i+=(*s++)&63;
	}else if(i>=0x80)
	{
//		continue;
	}

	*str=(char *)s;
	return(i);
}

int BASM_Parse_EmitChar(char **str, int j)
{
	unsigned char *t;
	int i;

	t=(unsigned char *)(*str);

	if(j>0x3FFFFFF)
	{
		*t++=0xFC+(j>>30);
		*t++=0x80+((j>>24)&0x3F);
		*t++=0x80+((j>>18)&0x3F);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0x1FFFFF)
	{
		*t++=0xF8+(j>>24);
		*t++=0x80+((j>>18)&0x3F);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0xFFFF)
	{
		*t++=0xF0+(j>>18);
		*t++=0x80+((j>>12)&0x3F);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if(j>0x7FF)
	{
		*t++=0xE0+(j>>12);
		*t++=0x80+((j>>6)&0x3F);
		*t++=0x80+(j&0x3F);
	}else if((j>0x7F) || !j)
	{
		*t++=0xC0+(j>>6);
		*t++=0x80+(j&0x3F);
	}else
	{
		if(!j) //Modified-UTF8
			{ *t++=0xC0; *t++=0x80; }
			else *t++=j;
	}

	*str=(char *)t;
	return(0);
}

int BASM_Parse_MatchTokenListLen(char *str, char **lst)
{
	char *s, *t;

	while(*lst)
	{
		s=str; t=*lst;

		while(*t)
		{
			if((*s)!=(*t))break;
			s++; t++;
		}

		if(!(*t))return(strlen(*lst));
		lst++;
	}
	return(0);
}

char *BASM_Parse_Token(char *s, char *b, int *ty)
{
	char *token_ops[]={
		"+++", "---", "==", "!=", "<=", ">=", "++", "--",
		"<<", ">>", "(*", "*)", NULL};

	char *t;
	int i, j, k;

	*b=0;
	s=BASM_Parse_EatWhite(s);
	if(!*s)
	{
		*ty=BASM_TOKEN_NULL;
		return(s);
	}
	t=b;

	if((*s=='_') || (*s=='.') || (*s=='$') || (*s=='@') ||
		((*s>='a') && (*s<='z')) || ((*s>='A') && (*s<='Z')))
	{
		while(
			(*s=='_') || (*s=='.') ||
			(*s=='$') || (*s=='@') ||
			((*s>='a') && (*s<='z')) ||
			((*s>='A') && (*s<='Z')) ||
			((*s>='0') && (*s<='9')))
			*t++=*s++;
		*t++=0;

		*ty=BASM_TOKEN_NAME;
		return(s);
	}

	if((*s>='0') && (*s<='9'))
	{
		if((s[0]=='0') && (s[1]=='x'))
		{
			*t++=*s++;
			*t++=*s++;
			while(((*s>='0') && (*s<='9')) ||
				((*s>='A') && (*s<='F')) ||
				((*s>='a') && (*s<='f')))
			{
				*t++=*s++;
			}
			*t++=0;

			*ty=BASM_TOKEN_NUMBER;
			return(s);
		}

		if(*s=='-')*t++=*s++;
		while(((*s>='0') && (*s<='9')) || (*s=='.') || (*s=='e') ||
			(*s=='b') || (*s=='h') || (*s=='B') || (*s=='H'))
		{
			if(*s=='e')
			{
				*t++=*s++;
				if((*s=='+') || (*s=='-'))
					*t++=*s++;
				continue;
			}
			*t++=*s++;
		}
		*t++=0;

		*ty=BASM_TOKEN_NUMBER;
		return(s);
	}

	if((*s=='"') || (*s=='\'')) /* quoted string */
	{
		if(*s=='\'')i=1;
			else i=0;
		s++;
		while(*s)
		{
			if(!i)if(*s=='\"')break;
			if(i)if(*s=='\'')break;

			if((s[0]=='\\') && (s[1]=='x'))
			{
				k=2; s+=2; j=0;
				while(k--)
				{
					j<<=4;
					if((*s>='0') && (*s<='9'))j+=*s-'0';
					if((*s>='A') && (*s<='F'))j+=*s-'A'+10;
					if((*s>='a') && (*s<='f'))j+=*s-'a'+10;
					s++;
				}

				*t++=j;
//				BASM_Parse_EmitChar(&t, j);
				continue;
			}

			if((s[0]=='\\') &&
				((s[1]=='u') || (s[1]=='U') || (s[1]=='X')))
			{
				k=2;
				if(s[1]=='u')k=4;
				if(s[1]=='U')k=8;
				if(s[1]=='X')k=4;	//raw 16-bit hex value

				s+=2; j=0;
				while(k--)
				{
					j<<=4;
					if((*s>='0') && (*s<='9'))j+=*s-'0';
					if((*s>='A') && (*s<='F'))j+=*s-'A'+10;
					if((*s>='a') && (*s<='f'))j+=*s-'a'+10;
					s++;
				}

				BASM_Parse_EmitChar(&t, j);
				continue;
			}

			if(*s=='\\')
			{
				s++;
				switch(*s)
				{
				case 'a': *t++='\b'; break;
				case 'b': *t++='\b'; break;
				case 'v': *t++='\v'; break;
				case 'f': *t++='\f'; break;
				case 'e': *t++='\x1B'; break;
				case 'n': *t++='\n'; break;
				case 't': *t++='\t'; break;
				case 'r': *t++='\r'; break;
				case '\\': *t++='\\'; break;
				case '\"': *t++='\"'; break;
				case '\'': *t++='\''; break;
				case '?': *t++='?'; break;
				default:
					break;
				}
				s++;
				continue;
			}

//			*t++=*s++;
			j=BASM_Parse_ParseChar(&s);
			BASM_Parse_EmitChar(&t, j);
		}
		*t++=0;
		if(*s)s++;

		*ty=BASM_TOKEN_STRING;
		if(i)*ty=BASM_TOKEN_CHARSTRING;
		return(s);
	}

	if((*s==',') || (*s==';') || ((*s=='.') && (s[1]!='.')))
	{
		*t++=*s++;
		*t++=0;

		*ty=BASM_TOKEN_SEPERATOR;
		return(s);
	}

	if(
		(*s=='{') || (*s=='}') ||
		(*s=='[') || (*s==']') ||
		(*s=='(') || (*s==')'))
	{
		*t++=*s++;
		*t++=0;

		*ty=BASM_TOKEN_BRACE;
		return(s);
	}

	i=BASM_Parse_MatchTokenListLen(s, token_ops);
	if(i)
	{
		strncpy(t, s, i);
		t[i]=0; s+=i;
		*ty=BASM_TOKEN_OPERATOR;
		return(s);
	}

	if(BASM_Parse_OpChar(*s))
	{
		*t++=*s++;
		*t++=0;

		*ty=BASM_TOKEN_OPERATOR;
		if(!basm_stricmp(b, ":"))*ty=BASM_TOKEN_SEPERATOR;

		return(s);
	}

	*ty=BASM_TOKEN_NULL;
	return(NULL);
}

char *BASM_Parse_TokenGAS(char *s, char *b, int *ty)
{
	char *t;
	int i;

	*b=0;
	s=BASM_Parse_EatWhite(s);
	if(!*s)
	{
		*ty=BASM_TOKEN_NULL;
		return(s);
	}
	t=b;

	if((*s=='_') || (*s=='.') ||
		((*s>='a') && (*s<='z')) || ((*s>='A') && (*s<='Z')))
	{
		while(
			(*s=='_') || (*s=='.') ||
			(*s=='$') || (*s=='@') ||
			((*s>='a') && (*s<='z')) ||
			((*s>='A') && (*s<='Z')) ||
			((*s>='0') && (*s<='9')))
			*t++=*s++;
		*t++=0;

		*ty=BASM_TOKEN_NAME;
		return(s);
	}

	s=BASM_Parse_Token(s, b, ty);
	return(s);
}

#if 0
int BASM_ParseReg(BASM_Context *ctx, char *b)
{
	int i;

	i=BASM_Z;

	if(!basm_stricmp(b, "al"))i=BASM_AL;
	if(!basm_stricmp(b, "cl"))i=BASM_CL;
	if(!basm_stricmp(b, "dl"))i=BASM_DL;
	if(!basm_stricmp(b, "bl"))i=BASM_BL;
	if(!basm_stricmp(b, "ah"))i=BASM_AH;
	if(!basm_stricmp(b, "ch"))i=BASM_CH;
	if(!basm_stricmp(b, "dh"))i=BASM_DH;
	if(!basm_stricmp(b, "bh"))i=BASM_BH;

	if(!basm_stricmp(b, "ax"))i=BASM_AX;
	if(!basm_stricmp(b, "cx"))i=BASM_CX;
	if(!basm_stricmp(b, "dx"))i=BASM_DX;
	if(!basm_stricmp(b, "bx"))i=BASM_BX;
	if(!basm_stricmp(b, "sp"))i=BASM_SP;
	if(!basm_stricmp(b, "bp"))i=BASM_BP;
	if(!basm_stricmp(b, "si"))i=BASM_SI;
	if(!basm_stricmp(b, "di"))i=BASM_DI;

	if(!basm_stricmp(b, "eax"))i=BASM_EAX;
	if(!basm_stricmp(b, "ecx"))i=BASM_ECX;
	if(!basm_stricmp(b, "edx"))i=BASM_EDX;
	if(!basm_stricmp(b, "ebx"))i=BASM_EBX;
	if(!basm_stricmp(b, "esp"))i=BASM_ESP;
	if(!basm_stricmp(b, "ebp"))i=BASM_EBP;
	if(!basm_stricmp(b, "esi"))i=BASM_ESI;
	if(!basm_stricmp(b, "edi"))i=BASM_EDI;

#if 1
	if(!basm_stricmp(b, "r0b"))i=BASM_AL;
	if(!basm_stricmp(b, "r1b"))i=BASM_CL;
	if(!basm_stricmp(b, "r2b"))i=BASM_DL;
	if(!basm_stricmp(b, "r3b"))i=BASM_BL;
	if(!basm_stricmp(b, "r0h"))i=BASM_AH;
	if(!basm_stricmp(b, "r1h"))i=BASM_CH;
	if(!basm_stricmp(b, "r2h"))i=BASM_DH;
	if(!basm_stricmp(b, "r3h"))i=BASM_BH;

	if(!basm_stricmp(b, "r0w"))i=BASM_AX;
	if(!basm_stricmp(b, "r1w"))i=BASM_CX;
	if(!basm_stricmp(b, "r2w"))i=BASM_DX;
	if(!basm_stricmp(b, "r3w"))i=BASM_BX;
	if(!basm_stricmp(b, "r4w"))i=BASM_SP;
	if(!basm_stricmp(b, "r5w"))i=BASM_BP;
	if(!basm_stricmp(b, "r6w"))i=BASM_SI;
	if(!basm_stricmp(b, "r7w"))i=BASM_DI;

	if(!basm_stricmp(b, "r0d"))i=BASM_EAX;
	if(!basm_stricmp(b, "r1d"))i=BASM_ECX;
	if(!basm_stricmp(b, "r2d"))i=BASM_EDX;
	if(!basm_stricmp(b, "r3d"))i=BASM_EBX;
	if(!basm_stricmp(b, "r4d"))i=BASM_ESP;
	if(!basm_stricmp(b, "r5d"))i=BASM_EBP;
	if(!basm_stricmp(b, "r6d"))i=BASM_ESI;
	if(!basm_stricmp(b, "r7d"))i=BASM_EDI;
#endif

	if(i!=BASM_Z)return(i);

#if 1
	if(!basm_stricmp(b, "rb0"))i=BASM_AL;
	if(!basm_stricmp(b, "rb1"))i=BASM_CL;
	if(!basm_stricmp(b, "rb2"))i=BASM_DL;
	if(!basm_stricmp(b, "rb3"))i=BASM_BL;
	if(!basm_stricmp(b, "rh0"))i=BASM_AH;
	if(!basm_stricmp(b, "rh1"))i=BASM_CH;
	if(!basm_stricmp(b, "rh2"))i=BASM_DH;
	if(!basm_stricmp(b, "rh3"))i=BASM_BH;

	if(!basm_stricmp(b, "rw0"))i=BASM_AX;
	if(!basm_stricmp(b, "rw1"))i=BASM_CX;
	if(!basm_stricmp(b, "rw2"))i=BASM_DX;
	if(!basm_stricmp(b, "rw3"))i=BASM_BX;
	if(!basm_stricmp(b, "rw4"))i=BASM_SP;
	if(!basm_stricmp(b, "rw5"))i=BASM_BP;
	if(!basm_stricmp(b, "rw6"))i=BASM_SI;
	if(!basm_stricmp(b, "rw7"))i=BASM_DI;

	if(!basm_stricmp(b, "rd0"))i=BASM_EAX;
	if(!basm_stricmp(b, "rd1"))i=BASM_ECX;
	if(!basm_stricmp(b, "rd2"))i=BASM_EDX;
	if(!basm_stricmp(b, "rd3"))i=BASM_EBX;
	if(!basm_stricmp(b, "rd4"))i=BASM_ESP;
	if(!basm_stricmp(b, "rd5"))i=BASM_EBP;
	if(!basm_stricmp(b, "rd6"))i=BASM_ESI;
	if(!basm_stricmp(b, "rd7"))i=BASM_EDI;
#endif


	if(!basm_stricmp(b, "st0"))i=BASM_MM0;
	if(!basm_stricmp(b, "st1"))i=BASM_MM1;
	if(!basm_stricmp(b, "st2"))i=BASM_MM2;
	if(!basm_stricmp(b, "st3"))i=BASM_MM3;
	if(!basm_stricmp(b, "st4"))i=BASM_MM4;
	if(!basm_stricmp(b, "st5"))i=BASM_MM5;
	if(!basm_stricmp(b, "st6"))i=BASM_MM6;
	if(!basm_stricmp(b, "st7"))i=BASM_MM7;

	if(!basm_stricmp(b, "mm0"))i=BASM_MM0;
	if(!basm_stricmp(b, "mm1"))i=BASM_MM1;
	if(!basm_stricmp(b, "mm2"))i=BASM_MM2;
	if(!basm_stricmp(b, "mm3"))i=BASM_MM3;
	if(!basm_stricmp(b, "mm4"))i=BASM_MM4;
	if(!basm_stricmp(b, "mm5"))i=BASM_MM5;
	if(!basm_stricmp(b, "mm6"))i=BASM_MM6;
	if(!basm_stricmp(b, "mm7"))i=BASM_MM7;

	if(!basm_stricmp(b, "xmm0"))i=BASM_XMM0;
	if(!basm_stricmp(b, "xmm1"))i=BASM_XMM1;
	if(!basm_stricmp(b, "xmm2"))i=BASM_XMM2;
	if(!basm_stricmp(b, "xmm3"))i=BASM_XMM3;
	if(!basm_stricmp(b, "xmm4"))i=BASM_XMM4;
	if(!basm_stricmp(b, "xmm5"))i=BASM_XMM5;
	if(!basm_stricmp(b, "xmm6"))i=BASM_XMM6;
	if(!basm_stricmp(b, "xmm7"))i=BASM_XMM7;

	if(!basm_stricmp(b, "cr0"))i=BASM_CR0;
	if(!basm_stricmp(b, "cr1"))i=BASM_CR1;
	if(!basm_stricmp(b, "cr2"))i=BASM_CR2;
	if(!basm_stricmp(b, "cr3"))i=BASM_CR3;
	if(!basm_stricmp(b, "cr4"))i=BASM_CR4;
	if(!basm_stricmp(b, "cr5"))i=BASM_CR5;
	if(!basm_stricmp(b, "cr6"))i=BASM_CR6;
	if(!basm_stricmp(b, "cr7"))i=BASM_CR7;

	if(!basm_stricmp(b, "dr0"))i=BASM_DR0;
	if(!basm_stricmp(b, "dr1"))i=BASM_DR1;
	if(!basm_stricmp(b, "dr2"))i=BASM_DR2;
	if(!basm_stricmp(b, "dr3"))i=BASM_DR3;
	if(!basm_stricmp(b, "dr4"))i=BASM_DR4;
	if(!basm_stricmp(b, "dr5"))i=BASM_DR5;
	if(!basm_stricmp(b, "dr6"))i=BASM_DR6;
	if(!basm_stricmp(b, "dr7"))i=BASM_DR7;

	if(!basm_stricmp(b, "cs"))i=BASM_CS;
	if(!basm_stricmp(b, "ss"))i=BASM_SS;
	if(!basm_stricmp(b, "ds"))i=BASM_DS;
	if(!basm_stricmp(b, "es"))i=BASM_ES;
	if(!basm_stricmp(b, "fs"))i=BASM_FS;
	if(!basm_stricmp(b, "gs"))i=BASM_GS;

	if(i!=BASM_Z)return(i);


	if(!basm_stricmp(b, "spl"))i=BASM_SPL;
	if(!basm_stricmp(b, "bpl"))i=BASM_BPL;
	if(!basm_stricmp(b, "sil"))i=BASM_SIL;
	if(!basm_stricmp(b, "dil"))i=BASM_DIL;

	if(!basm_stricmp(b, "r4b"))i=BASM_SPL;
	if(!basm_stricmp(b, "r5b"))i=BASM_BPL;
	if(!basm_stricmp(b, "r6b"))i=BASM_SIL;
	if(!basm_stricmp(b, "r7b"))i=BASM_DIL;
	if(!basm_stricmp(b, "r8b"))i=BASM_EL;
	if(!basm_stricmp(b, "r9b"))i=BASM_FL;
	if(!basm_stricmp(b, "r10b"))i=BASM_GL;
	if(!basm_stricmp(b, "r11b"))i=BASM_HL;
	if(!basm_stricmp(b, "r12b"))i=BASM_IL;
	if(!basm_stricmp(b, "r13b"))i=BASM_JL;
	if(!basm_stricmp(b, "r14b"))i=BASM_KL;
	if(!basm_stricmp(b, "r15b"))i=BASM_LL;


	if(!basm_stricmp(b, "r8w"))i=BASM_EX;
	if(!basm_stricmp(b, "r9w"))i=BASM_FX;
	if(!basm_stricmp(b, "r10w"))i=BASM_GX;
	if(!basm_stricmp(b, "r11w"))i=BASM_HX;
	if(!basm_stricmp(b, "r12w"))i=BASM_IX;
	if(!basm_stricmp(b, "r13w"))i=BASM_JX;
	if(!basm_stricmp(b, "r14w"))i=BASM_KX;
	if(!basm_stricmp(b, "r15w"))i=BASM_LX;

	if(!basm_stricmp(b, "r8d"))i=BASM_EEX;
	if(!basm_stricmp(b, "r9d"))i=BASM_EFX;
	if(!basm_stricmp(b, "r10d"))i=BASM_EGX;
	if(!basm_stricmp(b, "r11d"))i=BASM_EHX;
	if(!basm_stricmp(b, "r12d"))i=BASM_EIX;
	if(!basm_stricmp(b, "r13d"))i=BASM_EJX;
	if(!basm_stricmp(b, "r14d"))i=BASM_EKX;
	if(!basm_stricmp(b, "r15d"))i=BASM_ELX;


	if(!basm_stricmp(b, "rax"))i=BASM_RAX;
	if(!basm_stricmp(b, "rcx"))i=BASM_RCX;
	if(!basm_stricmp(b, "rdx"))i=BASM_RDX;
	if(!basm_stricmp(b, "rbx"))i=BASM_RBX;
	if(!basm_stricmp(b, "rsp"))i=BASM_RSP;
	if(!basm_stricmp(b, "rbp"))i=BASM_RBP;
	if(!basm_stricmp(b, "rsi"))i=BASM_RSI;
	if(!basm_stricmp(b, "rdi"))i=BASM_RDI;

	if(!basm_stricmp(b, "r0"))i=BASM_RAX;
	if(!basm_stricmp(b, "r1"))i=BASM_RCX;
	if(!basm_stricmp(b, "r2"))i=BASM_RDX;
	if(!basm_stricmp(b, "r3"))i=BASM_RBX;
	if(!basm_stricmp(b, "r4"))i=BASM_RSP;
	if(!basm_stricmp(b, "r5"))i=BASM_RBP;
	if(!basm_stricmp(b, "r6"))i=BASM_RSI;
	if(!basm_stricmp(b, "r7"))i=BASM_RDI;
	if(!basm_stricmp(b, "r8"))i=BASM_REX;
	if(!basm_stricmp(b, "r9"))i=BASM_RFX;
	if(!basm_stricmp(b, "r10"))i=BASM_RGX;
	if(!basm_stricmp(b, "r11"))i=BASM_RHX;
	if(!basm_stricmp(b, "r12"))i=BASM_RIX;
	if(!basm_stricmp(b, "r13"))i=BASM_RJX;
	if(!basm_stricmp(b, "r14"))i=BASM_RKX;
	if(!basm_stricmp(b, "r15"))i=BASM_RLX;

#if 1
	if(!basm_stricmp(b, "el"))i=BASM_EL;
	if(!basm_stricmp(b, "fl"))i=BASM_FL;
	if(!basm_stricmp(b, "gl"))i=BASM_GL;
	if(!basm_stricmp(b, "hl"))i=BASM_HL;
	if(!basm_stricmp(b, "il"))i=BASM_IL;
	if(!basm_stricmp(b, "jl"))i=BASM_JL;
	if(!basm_stricmp(b, "kl"))i=BASM_KL;
	if(!basm_stricmp(b, "ll"))i=BASM_LL;

	if(!basm_stricmp(b, "ex"))i=BASM_EX;
	if(!basm_stricmp(b, "fx"))i=BASM_FX;
	if(!basm_stricmp(b, "gx"))i=BASM_GX;
	if(!basm_stricmp(b, "hx"))i=BASM_HX;
	if(!basm_stricmp(b, "ix"))i=BASM_IX;
	if(!basm_stricmp(b, "jx"))i=BASM_JX;
	if(!basm_stricmp(b, "kx"))i=BASM_KX;
	if(!basm_stricmp(b, "lx"))i=BASM_LX;

	if(!basm_stricmp(b, "eex"))i=BASM_EEX;
	if(!basm_stricmp(b, "efx"))i=BASM_EFX;
	if(!basm_stricmp(b, "egx"))i=BASM_EGX;
	if(!basm_stricmp(b, "ehx"))i=BASM_EHX;
	if(!basm_stricmp(b, "eix"))i=BASM_EIX;
	if(!basm_stricmp(b, "ejx"))i=BASM_EJX;
	if(!basm_stricmp(b, "ekx"))i=BASM_EKX;
	if(!basm_stricmp(b, "elx"))i=BASM_ELX;

	if(!basm_stricmp(b, "rex"))i=BASM_REX;
	if(!basm_stricmp(b, "rfx"))i=BASM_RFX;
	if(!basm_stricmp(b, "rgx"))i=BASM_RGX;
	if(!basm_stricmp(b, "rhx"))i=BASM_RHX;
	if(!basm_stricmp(b, "rix"))i=BASM_RIX;
	if(!basm_stricmp(b, "rjx"))i=BASM_RJX;
	if(!basm_stricmp(b, "rkx"))i=BASM_RKX;
	if(!basm_stricmp(b, "rlx"))i=BASM_RLX;
#endif

#if 1
	if(!basm_stricmp(b, "rb4"))i=BASM_SPL;
	if(!basm_stricmp(b, "rb5"))i=BASM_BPL;
	if(!basm_stricmp(b, "rb6"))i=BASM_SIL;
	if(!basm_stricmp(b, "rb7"))i=BASM_DIL;
	if(!basm_stricmp(b, "rb8"))i=BASM_EL;
	if(!basm_stricmp(b, "rb9"))i=BASM_FL;
	if(!basm_stricmp(b, "rb10"))i=BASM_GL;
	if(!basm_stricmp(b, "rb11"))i=BASM_HL;
	if(!basm_stricmp(b, "rb12"))i=BASM_IL;
	if(!basm_stricmp(b, "rb13"))i=BASM_JL;
	if(!basm_stricmp(b, "rb14"))i=BASM_KL;
	if(!basm_stricmp(b, "rb15"))i=BASM_LL;

	if(!basm_stricmp(b, "rw8"))i=BASM_EX;
	if(!basm_stricmp(b, "rw9"))i=BASM_FX;
	if(!basm_stricmp(b, "rw10"))i=BASM_GX;
	if(!basm_stricmp(b, "rw11"))i=BASM_HX;
	if(!basm_stricmp(b, "rw12"))i=BASM_IX;
	if(!basm_stricmp(b, "rw13"))i=BASM_JX;
	if(!basm_stricmp(b, "rw14"))i=BASM_KX;
	if(!basm_stricmp(b, "rw15"))i=BASM_LX;

	if(!basm_stricmp(b, "rd8"))i=BASM_EEX;
	if(!basm_stricmp(b, "rd9"))i=BASM_EFX;
	if(!basm_stricmp(b, "rd10"))i=BASM_EGX;
	if(!basm_stricmp(b, "rd11"))i=BASM_EHX;
	if(!basm_stricmp(b, "rd12"))i=BASM_EIX;
	if(!basm_stricmp(b, "rd13"))i=BASM_EJX;
	if(!basm_stricmp(b, "rd14"))i=BASM_EKX;
	if(!basm_stricmp(b, "rd15"))i=BASM_ELX;

	if(!basm_stricmp(b, "rq0"))i=BASM_RAX;
	if(!basm_stricmp(b, "rq1"))i=BASM_RCX;
	if(!basm_stricmp(b, "rq2"))i=BASM_RDX;
	if(!basm_stricmp(b, "rq3"))i=BASM_RBX;
	if(!basm_stricmp(b, "rq4"))i=BASM_RSP;
	if(!basm_stricmp(b, "rq5"))i=BASM_RBP;
	if(!basm_stricmp(b, "rq6"))i=BASM_RSI;
	if(!basm_stricmp(b, "rq7"))i=BASM_RDI;
	if(!basm_stricmp(b, "rq8"))i=BASM_REX;
	if(!basm_stricmp(b, "rq9"))i=BASM_RFX;
	if(!basm_stricmp(b, "rq10"))i=BASM_RGX;
	if(!basm_stricmp(b, "rq11"))i=BASM_RHX;
	if(!basm_stricmp(b, "rq12"))i=BASM_RIX;
	if(!basm_stricmp(b, "rq13"))i=BASM_RJX;
	if(!basm_stricmp(b, "rq14"))i=BASM_RKX;
	if(!basm_stricmp(b, "rq15"))i=BASM_RLX;
#endif


	if(!basm_stricmp(b, "xmm8"))i=BASM_XMM8;
	if(!basm_stricmp(b, "xmm9"))i=BASM_XMM9;
	if(!basm_stricmp(b, "xmm10"))i=BASM_XMM10;
	if(!basm_stricmp(b, "xmm11"))i=BASM_XMM11;
	if(!basm_stricmp(b, "xmm12"))i=BASM_XMM12;
	if(!basm_stricmp(b, "xmm13"))i=BASM_XMM13;
	if(!basm_stricmp(b, "xmm14"))i=BASM_XMM14;
	if(!basm_stricmp(b, "xmm15"))i=BASM_XMM15;

	if(!basm_stricmp(b, "cr8"))i=BASM_CR0+8;
	if(!basm_stricmp(b, "cr9"))i=BASM_CR0+9;
	if(!basm_stricmp(b, "cr10"))i=BASM_CR0+10;
	if(!basm_stricmp(b, "cr11"))i=BASM_CR0+11;
	if(!basm_stricmp(b, "cr12"))i=BASM_CR0+12;
	if(!basm_stricmp(b, "cr13"))i=BASM_CR0+13;
	if(!basm_stricmp(b, "cr14"))i=BASM_CR0+14;
	if(!basm_stricmp(b, "cr15"))i=BASM_CR0+15;

	if(!basm_stricmp(b, "dr8"))i=BASM_DR0+8;
	if(!basm_stricmp(b, "dr9"))i=BASM_DR0+9;
	if(!basm_stricmp(b, "dr10"))i=BASM_DR0+10;
	if(!basm_stricmp(b, "dr11"))i=BASM_DR0+11;
	if(!basm_stricmp(b, "dr12"))i=BASM_DR0+12;
	if(!basm_stricmp(b, "dr13"))i=BASM_DR0+13;
	if(!basm_stricmp(b, "dr14"))i=BASM_DR0+14;
	if(!basm_stricmp(b, "dr15"))i=BASM_DR0+15;

	if(!basm_stricmp(b, "rip"))i=BASM_RIP;

	if((i!=BASM_Z) && !(ctx->fl&1))
	{
		printf("BASM_ParseReg: Use of x86-64 reg '%s' in %d bit code\n",
			b, (ctx->fl&2)?16:32);
	}

	return(i);
}
#endif

#if 1
int BASM_ParseReg(BASM_Context *ctx, char *b)
{
	int i;

	for(i=0; basm_regs[i]; i++)
		if(!strcmp(basm_regs[i], b))
			return(basm_regidx[i]);

	return(-1);
}
#endif

char *BASM_ParseArgGAS(BASM_Context *ctx, char *str,
	int *breg, int *ireg, int *sc, long long *disp, char **lbl)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2, sg;
	int i;

	*breg=BASM_Z; *ireg=BASM_Z;
	*sc=0; *disp=0;
	*lbl=NULL;

	s=BASM_Parse_TokenGAS(str, b, &ty);

	if(!basm_stricmp(b, "%"))
	{
		s=BASM_Parse_TokenGAS(str, b, &ty);

		i=-1;
		if(!basm_stricmp(b, "cs"))i=BASM_REG_CS;
		if(!basm_stricmp(b, "ss"))i=BASM_REG_SS;
		if(!basm_stricmp(b, "ds"))i=BASM_REG_DS;
		if(!basm_stricmp(b, "es"))i=BASM_REG_ES;
		if(!basm_stricmp(b, "fs"))i=BASM_REG_FS;
		if(!basm_stricmp(b, "gs"))i=BASM_REG_GS;

		if(i<0)
		{
			*breg=BASM_ParseReg(ctx, b);
			return(s);
		}

		ctx->seg=i;
	}else if(!basm_stricmp(b, "$"))
	{
		s=BASM_Parse_TokenGAS(str, b, &ty);

		if(ty==BASM_TOKEN_NAME)
		{
			*lbl=basm_strdup(b);
			return(s);
		}

		sg=1;
		if(!basm_stricmp(b, "-"))
			{ s=BASM_Parse_TokenGAS(s, b, &ty); sg=-1; }
		if(ty==BASM_TOKEN_NUMBER)
		{
			if(basm_strisreal(b))
			{
				if(ctx->w==64)
				{
					*disp=basm_atof_u64_sg(b, sg);
					return(s);
				}

				ctx->w=32;	//force 32 bits
				*disp=basm_atof_u32_sg(b, sg);
				return(s);
			}

			*disp=sg*basm_atoi(b);
			return(s);
		}
	}

	*sc=1;

	if(ty==BASM_TOKEN_NAME)
	{
		*lbl=basm_strdup(b);
		s=BASM_Parse_TokenGAS(s, b, &ty);
	}

	sg=1;
	if(!basm_stricmp(b, "-")) { s=BASM_Parse_TokenGAS(s, b, &ty); sg=-1; }
	if(ty==BASM_TOKEN_NUMBER)
	{
		*disp=sg*basm_atoi(b);
		s=BASM_Parse_TokenGAS(s, b, &ty);
	}

	if(!basm_stricmp(b, "(") && (ty==BASM_TOKEN_BRACE))
	{
		*sc=1;

		s=BASM_Parse_TokenGAS(s, b, &ty);
		if(!basm_stricmp(b, "%"))s=BASM_Parse_TokenGAS(s, b, &ty);
		BASM_Parse_TokenGAS(s, b2, &ty2);

		if(ty==BASM_TOKEN_NAME)
		{
			*breg=BASM_ParseReg(ctx, b);
			s=BASM_Parse_TokenGAS(s, b, &ty);
		}

		if(!basm_stricmp(b, ")"))return(s);
		if(!basm_stricmp(b, ","))
			s=BASM_Parse_TokenGAS(s, b, &ty);
		if(!basm_stricmp(b, "%"))s=BASM_Parse_TokenGAS(s, b, &ty);
		BASM_Parse_TokenGAS(s, b2, &ty2);

		if(ty==BASM_TOKEN_NAME)
		{
			*ireg=BASM_ParseReg(ctx, b);;
			s=BASM_Parse_TokenGAS(s, b, &ty);
		}

		if(!basm_stricmp(b, ")"))return(s);
		if(!basm_stricmp(b, ","))
			s=BASM_Parse_TokenGAS(s, b, &ty);
		BASM_Parse_TokenGAS(s, b2, &ty2);

		sg=1;
		if(!basm_stricmp(b, "-"))
			{ s=BASM_Parse_TokenGAS(s, b, &ty); sg=-1; }
		if(ty==BASM_TOKEN_NUMBER)
		{
			*sc+=sg*basm_atoi(b);
			s=BASM_Parse_TokenGAS(s, b, &ty);
		}

		if(!basm_stricmp(b, ")"))return(s);

		printf("BASM_ParseArgGAS: Bad token '%s'\n", b);
		return(s);
	}

	printf("BASM_ParseArgGAS: Bad token '%s'\n", b);
	return(s);
}

char *BASM_ParseArg(BASM_Context *ctx, char *str,
	int *breg, int *ireg, int *sc, long long *disp, char **lbl)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2, sg;
	int i;

	*breg=BASM_Z; *ireg=BASM_Z;
	*sc=0; *disp=0;
	*lbl=NULL;

	s=BASM_Parse_EatWhite(str);
	if((*s=='%') || (*s=='$') || (ctx->fl&BASM_FL_GAS))
	{
		ctx->fl|=BASM_FL_GAS;
		s=BASM_ParseArgGAS(ctx, s, breg, ireg, sc, disp, lbl);
		return(s);
	}


	s=BASM_Parse_Token(str, b, &ty);

	if(ty==BASM_TOKEN_NAME)
	{
		*breg=BASM_ParseReg(ctx, b);
		if(*breg!=BASM_Z)return(s);

		*lbl=basm_strdup(b);
		return(s);
	}

	sg=1;
	if(!basm_stricmp(b, "-")) { s=BASM_Parse_Token(s, b, &ty); sg=-1; }
	if(ty==BASM_TOKEN_NUMBER)
	{
		BASM_Parse_Token(s, b2, &ty2);

		if(!strcmp(b2, "("))
		{
			ctx->fl|=BASM_FL_GAS;
			s=BASM_ParseArgGAS(ctx, str,
				breg, ireg, sc, disp, lbl);
			return(s);
		}


		if(basm_strisreal(b))
		{
			if(ctx->w==64)
			{
				*disp=basm_atof_u64_sg(b, sg);
				return(s);
			}

			ctx->w=32;	//force 32 bits
			*disp=basm_atof_u32_sg(b, sg);
			return(s);
		}

		*disp=sg*basm_atoi(b);
		return(s);
	}

	if(!basm_stricmp(b, "[") && (ty==BASM_TOKEN_BRACE))
	{
		*sc=1;

		s=BASM_Parse_Token(s, b, &ty);
		BASM_Parse_Token(s, b2, &ty2);

		if((!basm_stricmp(b, "cs") || !basm_stricmp(b, "ss") ||
			!basm_stricmp(b, "ds") || !basm_stricmp(b, "es") ||
			!basm_stricmp(b, "fs") || !basm_stricmp(b, "gs")) &&
			!strcmp(b2, ":"))
		{
			s=BASM_Parse_Token(s, b2, &ty2);	//':'
			if(!basm_stricmp(b, "cs"))ctx->seg=BASM_REG_CS;
			if(!basm_stricmp(b, "ss"))ctx->seg=BASM_REG_SS;
			if(!basm_stricmp(b, "ds"))ctx->seg=BASM_REG_DS;
			if(!basm_stricmp(b, "es"))ctx->seg=BASM_REG_ES;
			if(!basm_stricmp(b, "fs"))ctx->seg=BASM_REG_FS;
			if(!basm_stricmp(b, "gs"))ctx->seg=BASM_REG_GS;

			s=BASM_Parse_Token(s, b, &ty);
			BASM_Parse_Token(s, b2, &ty2);
		}

		sg=1;
		if(!basm_stricmp(b, "+") || !basm_stricmp(b, "-"))
		{
			if(!basm_stricmp(b, "-"))sg=-1;

			*sc=-1;
			s=BASM_Parse_Token(s, b, &ty);
			BASM_Parse_Token(s, b2, &ty2);
		}

		if(ty==BASM_TOKEN_NAME)
		{
			*breg=BASM_ParseReg(ctx, b);
			if(*breg==BASM_Z)
			{
				for(i=0; i<ctx->n_const; i++)
					if(!basm_stricmp(ctx->const_name[i], b))
				{
					*disp=ctx->const_value[i];
					break;
				}
				if(i>=ctx->n_const)*lbl=basm_strdup(b);
			}

			s=BASM_Parse_Token(s, b, &ty);
			if(!basm_stricmp(b, "]"))return(s);
			if(!basm_stricmp(b, "+"))s=BASM_Parse_Token(s, b, &ty);
			BASM_Parse_Token(s, b2, &ty2);
		}else if(ty==BASM_TOKEN_NUMBER)
		{
			*disp=sg*basm_atoi(b);
			s=BASM_Parse_Token(s, b, &ty);
			if(!basm_stricmp(b, "]"))return(s);
			if(!basm_stricmp(b, "+"))s=BASM_Parse_Token(s, b, &ty);
			BASM_Parse_Token(s, b2, &ty2);
		}else
		{
			printf("BASM_ParseArg: Bad token '%s'\n", b);
			return(s);
		}

		i=BASM_Z;
		if(ty==BASM_TOKEN_NAME)
			i=BASM_ParseReg(ctx, b);

		if((ty==BASM_TOKEN_NAME) && basm_stricmp(b2, "*") &&
			(i!=BASM_Z) && (*breg==BASM_Z))
		{
			*breg=i;

			s=BASM_Parse_Token(s, b, &ty);
			if(!basm_stricmp(b, "]"))return(s);
			if(!basm_stricmp(b, "+"))s=BASM_Parse_Token(s, b, &ty);

			if(ty==BASM_TOKEN_NAME)
				i=BASM_ParseReg(ctx, b);
		}

		if((ty==BASM_TOKEN_NAME) && (i!=BASM_Z))
		{
			*ireg=i;
			if(!basm_stricmp(b2, "*"))
			{
				s=BASM_Parse_Token(s, b, &ty);
				s=BASM_Parse_Token(s, b, &ty);
				*sc=atoi(b);
			}

			s=BASM_Parse_Token(s, b, &ty);
			if(!basm_stricmp(b, "]"))return(s);
			if(!basm_stricmp(b, "+"))s=BASM_Parse_Token(s, b, &ty);
		}

		sg=1;
		if(!basm_stricmp(b, "-")) { s=BASM_Parse_Token(s, b, &ty); sg=-1; }
		if(ty==BASM_TOKEN_NUMBER)
		{
			*disp+=sg*basm_atoi(b);
			s=BASM_Parse_Token(s, b, &ty);
		}

		if(ty==BASM_TOKEN_NAME)
		{
			for(i=0; i<ctx->n_const; i++)
				if(!basm_stricmp(ctx->const_name[i], b))
			{
				*disp+=sg*ctx->const_value[i];
				break;
			}

			if(i>=ctx->n_const)*lbl=basm_strdup(b);

			s=BASM_Parse_Token(s, b, &ty);
		}

		if(basm_stricmp(b, "]"))
			printf("BASM_ParseArg: Bad token '%s'\n", b);
		return(s);
	}

	printf("BASM_ParseArg: Bad token '%s'\n", b);
	return(s);
}


int BASM_OpOnlySingleP(int op)
{
	int i;

	i=basm_opidx[op];
	for(; basm_opnums[i]==op; i++)
	{
		if(!BASM_OpSingleP(i))return(0);
	}
//	if(i!=basm_opidx[op])return(0);
	return(1);
}

char *BASM_ParseSpecial(BASM_Context *ctx, char *str, char *op)
{
	char b[64], b2[64];
	char *s, *t, *l0, *l1;
	byte *ip;
	long long li;
	int ty, ty2;
	int i, j, k, w, sg;

	s=str;
	if(!basm_stricmp(op, "A16")) { ctx->fl|=2; return(s); }
	if(!basm_stricmp(op, "A32")) { ctx->fl&=~3; return(s); }
	if(!basm_stricmp(op, "A64")) { ctx->fl|=1; return(s); }

	if(!basm_stricmp(op, ".a16")) { ctx->fl|=2; return(s); }
	if(!basm_stricmp(op, ".a32")) { ctx->fl&=~3; return(s); }
	if(!basm_stricmp(op, ".a64")) { ctx->fl|=1; return(s); }

	if(!basm_stricmp(op, "use16")) { ctx->fl|=2; return(s); }
	if(!basm_stricmp(op, "use32")) { ctx->fl&=~3; return(s); }
	if(!basm_stricmp(op, "use64")) { ctx->fl|=1; return(s); }

	if(!basm_stricmp(op, ".text")) { ctx->fl&=~4; return(s); }
	if(!basm_stricmp(op, ".data")) { ctx->fl|=4; return(s); }

	if(!basm_stricmp(op, ".proxy"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		i=ctx->n_proxy++;
		if(i>=ctx->m_proxy)
		{
			j=i+(i>>1);
			ctx->proxy_name=realloc(ctx->proxy_name,
				j*sizeof(char *));
			ctx->m_proxy=j;
		}
		ctx->proxy_name[i]=basm_strdup(b);
		return(s);
	}

	if(!basm_stricmp(op, "section"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		if(!basm_stricmp(b, ".text"))ctx->fl&=~4;
		if(!basm_stricmp(b, ".data"))ctx->fl|=4;
		if(!basm_stricmp(b, ".bss"))ctx->fl|=4;
		return(s);
	}

	if(!basm_stricmp(op, "bits"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		if(!basm_stricmp(b, "16")) ctx->fl|=2;
		if(!basm_stricmp(b, "32")) ctx->fl&=~3;
		if(!basm_stricmp(b, "64")) ctx->fl|=1;
		return(s);
	}

	if(!basm_stricmp(op, "extern"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		return(s);
	}

	if(!basm_stricmp(op, "global"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		return(s);
	}

	if(!basm_stricmp(op, ".align"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		BASM_Align(ctx, 1<<basm_atoi(b));
		return(s);
	}

	if(!basm_stricmp(op, "align") || !basm_stricmp(op, ".balign"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		BASM_Align(ctx, basm_atoi(b));
		return(s);
	}

	if(!basm_stricmp(op, "align_vli"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		BASM_AlignVLI(ctx, basm_atoi(b));
		return(s);
	}


	if(!basm_stricmp(op, ".ascii"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			if(ty2==BASM_TOKEN_STRING)
				{ BASM_OutStr8(ctx, b2); }
			if(ty2==BASM_TOKEN_CHARSTRING)
				{ BASM_OutStr8(ctx, b2); }
		}
		return(s);
	}

	if(!basm_stricmp(op, ".asciz"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			if(ty2==BASM_TOKEN_STRING)
				{ BASM_OutStr8(ctx, b2); }
			if(ty2==BASM_TOKEN_CHARSTRING)
				{ BASM_OutStr8(ctx, b2); }
		}
		BASM_OutByte(ctx, 0);
		return(s);
	}

	if(!basm_stricmp(op, "db") || !basm_stricmp(op, ".byte"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			sg=1;
			if(!basm_stricmp(b2, "-"))
			{
				s=BASM_Parse_Token(s, b2, &ty2);
				sg=-1;
			}

			if(ty2==BASM_TOKEN_NUMBER)
				BASM_OutByte(ctx, basm_atoi(b2)*sg);
//			if(ty2==BASM_TOKEN_STRING)
//				{ t=b2; while(*t)BASM_OutByte(ctx, *t++); }
//			if(ty2==BASM_TOKEN_CHARSTRING)
//				{ t=b2; while(*t)BASM_OutByte(ctx, *t++); }

			if(ty2==BASM_TOKEN_STRING)
				{ BASM_OutStr8(ctx, b2); }
			if(ty2==BASM_TOKEN_CHARSTRING)
				{ BASM_OutStr8(ctx, b2); }
		}
		return(s);
	}

	if(!basm_stricmp(op, "dw") || !basm_stricmp(op, ".short") ||
		!basm_stricmp(op, ".hword"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			sg=1;
			if(!basm_stricmp(b2, "-"))
			{
				s=BASM_Parse_Token(s, b2, &ty2);
				sg=-1;
			}

			if(ty2==BASM_TOKEN_NUMBER)
				BASM_OutWord(ctx, basm_atoi(b2)*sg);
//			if(ty2==BASM_TOKEN_STRING)
//				{ t=b2; while(*t)BASM_OutWord(ctx, *t++); }
//			if(ty2==BASM_TOKEN_CHARSTRING)
//				{ t=b2; while(*t)BASM_OutWord(ctx, *t++); }

			if(ty2==BASM_TOKEN_STRING)
				{ BASM_OutStr16(ctx, b2); }
			if(ty2==BASM_TOKEN_CHARSTRING)
				{ BASM_OutStr16(ctx, b2); }
		}
		return(s);
	}

	if(!basm_stricmp(op, "dd") ||
		!basm_stricmp(op, ".int") || !basm_stricmp(op, ".long"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			if(ty2==BASM_TOKEN_NAME)
			{
				BASM_OutDWord(ctx, 0);
				BASM_EmitGoto(ctx, b2, BASM_JMP_ABS32);
				continue;
			}

			sg=1;
			if(!basm_stricmp(b2, "-"))
			{
				s=BASM_Parse_Token(s, b2, &ty2);
				sg=-1;
			}

			if(ty2==BASM_TOKEN_NUMBER)
			{
				if(basm_strisreal(b2))
				{
					BASM_OutDWord(ctx,
						basm_atof_u32_sg(b2, sg));
				}else
				{
					BASM_OutDWord(ctx, basm_atoi(b2)*sg);
				}
			}
		}
		return(s);
	}

	if(!basm_stricmp(op, "dq") || !basm_stricmp(op, ".quad"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;
//			if(ty2==BASM_TOKEN_NUMBER)
//				BASM_OutQWord(ctx, basm_atoi(b2));

			if(ty2==BASM_TOKEN_NAME)
			{
				BASM_OutQWord(ctx, 0);
				BASM_EmitGoto(ctx, b2, BASM_JMP_ABS64);
				continue;
			}

			sg=1;
			if(!basm_stricmp(b2, "-"))
			{
				s=BASM_Parse_Token(s, b2, &ty2);
				sg=-1;
			}

			if(ty2==BASM_TOKEN_NUMBER)
			{
				if(basm_strisreal(b2))
				{
					BASM_OutQWord(ctx,
						basm_atof_u64_sg(b2, sg));
				}else
				{
					BASM_OutQWord(ctx, basm_atoi(b2)*sg);
				}
			}
		}
		return(s);
	}

	if(!basm_stricmp(op, ".float"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			sg=1;
			if(!basm_stricmp(b2, "-"))
				{ s=BASM_Parse_Token(s, b2, &ty2); sg=-1; }

			if(ty2==BASM_TOKEN_NUMBER)
				BASM_OutDWord(ctx, basm_atof_u32_sg(b2, sg));
		}
		return(s);
	}

	if(!basm_stricmp(op, ".double"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			sg=1;
			if(!basm_stricmp(b2, "-"))
				{ s=BASM_Parse_Token(s, b2, &ty2); sg=-1; }

			if(ty2==BASM_TOKEN_NUMBER)
				BASM_OutQWord(ctx, basm_atof_u64_sg(b2, sg));
		}
		return(s);
	}

	if(!basm_stricmp(op, "duv"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			if(ty2==BASM_TOKEN_NAME)
			{
				li=BASM_PredictDisp(ctx, b2);

				if(!li)
				{
					BASM_OutDWord(ctx, 0xE0000000);
					continue;
				}

				if(li<((1<<7)-16))li-=1;
				if(li<((1<<14)-16))li-=2;
				if(li<((1<<28)-16))li-=3;
				BASM_OutUVLIP(ctx, li, 16);
				continue;
			}

			if(ty2==BASM_TOKEN_NUMBER)
			{
				BASM_OutUVLI(ctx, basm_atoi(b2));
			}
		}
		return(s);
	}

	if(!basm_stricmp(op, "dsv"))
	{
		while(1)
		{
			if(BASM_Parse_IsLineBreak(s))break;
			s=BASM_Parse_Token(s, b2, &ty2);
			if(!basm_stricmp(b2, ";"))break;
			if(!basm_stricmp(b2, ","))continue;

			if(ty2==BASM_TOKEN_NAME)
			{
				li=BASM_PredictDisp(ctx, b2);

				if(!li)
				{
					BASM_OutDWord(ctx, 0xE0000000);
					continue;
				}

				BASM_OutSVLIP(ctx, li, 16);
				continue;
			}

			sg=1;
			if(!basm_stricmp(b2, "-"))
			{
				s=BASM_Parse_Token(s, b2, &ty2);
				sg=-1;
			}

			if(ty2==BASM_TOKEN_NUMBER)
			{
				BASM_OutSVLI(ctx, basm_atoi(b2)*sg);
			}
		}
		return(s);
	}

	if(!basm_stricmp(op, "resb"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		i=basm_atoi(b);
		while(i--)BASM_OutByte(ctx, 0);
		return(s);
	}

	if(!basm_stricmp(op, "resw"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		i=basm_atoi(b);
		while(i--)BASM_OutWord(ctx, 0);
		return(s);
	}

	if(!basm_stricmp(op, "resd"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		i=basm_atoi(b);
		while(i--)BASM_OutDWord(ctx, 0);
		return(s);
	}

	if(!basm_stricmp(op, "resq"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		i=basm_atoi(b);
		while(i--)BASM_OutQWord(ctx, 0);
		return(s);
	}

	if(!basm_stricmp(op, "getip"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		i=BASM_ParseReg(ctx, b);

		//call .l0; .l0:
		BASM_OutByte(ctx, 0xE8);	//call
		if(ctx->fl&2)BASM_OutWord(ctx, 0);
			else BASM_OutDWord(ctx, 0);

		//pop reg
		if(i&8)BASM_OutByte(ctx, 0x41);	//REX.B
		BASM_OutByte(ctx, 0x58|(i&0x7));

		if(i&8)BASM_OutByte(ctx, 0x41);	//REX.B
		BASM_OutByte(ctx, 0x83);
		BASM_OutByte(ctx, 0xC0|(0<<3)|(i&3));
		BASM_OutByte(ctx, (i&8)?6:4);

		return(s);
	}

	if(!basm_stricmp(op, "getbase"))
	{
		s=BASM_Parse_Token(s, b, &ty);
		i=BASM_ParseReg(ctx, b);

		//call .l0; .l0:
		BASM_OutByte(ctx, 0xE8);	//call
		if(ctx->fl&2)BASM_OutWord(ctx, 0);
			else BASM_OutDWord(ctx, 0);
		j=ctx->ip-ctx->base_ip;

		//pop reg
		if(i&8)BASM_OutByte(ctx, 0x41);	//REX.B
		BASM_OutByte(ctx, 0x58|(i&0x7));

		//sub reg, offs
		if(i&8)BASM_OutByte(ctx, 0x41);	//REX.B

		if(j<128)
		{
			BASM_OutByte(ctx, 0x83);
			BASM_OutByte(ctx, 0xC0|(5<<3)|(i&3));
			BASM_OutByte(ctx, j);
		}else
		{
			BASM_OutByte(ctx, 0x81);
			BASM_OutByte(ctx, 0xC0|(5<<3)|(i&3));
			if(ctx->fl&2)BASM_OutWord(ctx, j);
				else BASM_OutDWord(ctx, j);
		}

		return(s);
	}

	if(!basm_stricmp(op, "setbase"))
	{
		s=BASM_Parse_Token(s, b, &ty);
//		ip=BASM_LookupLabel(ctx, b);
//		if(ip)ctx->base_ip=ip;
		return(s);
	}

	return(NULL);
}

int BASM_LookupOpcode(BASM_Context *ctx, char *str)
{
	static int *hash=NULL;
	int i, h;
	char *s;

	if(!hash)
	{
		hash=malloc(1024*sizeof(int));
		for(i=0; i<1024; i++)
			hash[i]=-1;
	}

	s=str; h=0;
	while(*s)h=h*251+(*s++);
	h&=0x3FF;

	if(hash[h]>=0)
	{
		i=hash[h];
		if(!basm_stricmp(basm_ops[i], str))
			return(i);
	}

	for(i=0; basm_ops[i]; i++)
		if(!basm_stricmp(basm_ops[i], str))
			break;
	if(basm_ops[i])hash[h]=i;

	return(i);
}

char *BASM_ParseOpcode(BASM_Context *ctx, char *str)
{
	char b[64], b2[64];
	char *s, *t, *l0, *l1, *l2;
	int ty, ty2;
	int i, j;

	long long disp0, disp1, disp2;
	int br0, ir0, sc0;
	int br1, ir1, sc1;
	int br2, ir2, sc2;

	b[0]=0; b2[0]=0;

	s=BASM_Parse_EatWhite(str);
	if(!(*s))return(s);

	s=BASM_Parse_Token(s, b, &ty);
	BASM_Parse_Token(s, b2, &ty2);

	if(!b[0])return(s);

	if(!strcmp(b, "["))
	{
		s=BASM_Parse_Token(s, b, &ty);
		t=BASM_ParseSpecial(ctx, s, b);
		if(t)
		{
			s=t;
			BASM_Parse_Token(s, b, &ty);
			if(!strcmp(b, "]"))
				s=BASM_Parse_Token(s, b, &ty);
			return(s);
		}

		//fail, seek to next ']'

		while(*s && (*s!=']'))s++;
		if(*s==']')s++;
		return(s);
	}

	if((ty==BASM_TOKEN_NAME) &&
		(!basm_stricmp(b2, ":") || !basm_stricmp(b2, "db") ||
		!basm_stricmp(b2, "dw") || !basm_stricmp(b2, "dd") ||
		!basm_stricmp(b2, "dq") || !basm_stricmp(b2, "resb") ||
		!basm_stricmp(b2, "resw") || !basm_stricmp(b2, "resd") ||
		!basm_stricmp(b2, "resq") ))
	{
		if(!basm_stricmp(b2, ":"))
			s=BASM_Parse_Token(s, b2, &ty2);

		if(b[0]=='.')
		{
			if(!ctx->label_base)
			{
				printf("BASM_ParseOpcode: "
					"local label '%s' absent base\n", b);
				return(s);
			}

			sprintf(b2, "%s%s", ctx->label_base, b);
			BASM_EmitLabel(ctx, b2);
			return(s);
		}else
		{
			ctx->label_base=basm_strdup(b);
		}

		BASM_EmitLabel(ctx, b);
		return(s);
	}

#if 1
	if((ty==BASM_TOKEN_NAME) && !basm_stricmp(b2, "equ") )
	{
		s=BASM_Parse_Token(s, b2, &ty2);		//equ
		s=BASM_Parse_Token(s, b2, &ty2);		//value

		i=1;
		if(!basm_stricmp(b2, "-"))
		{
			i=-1;
			s=BASM_Parse_Token(s, b2, &ty2);		//value
		}
			

		BASM_EmitConst(ctx, b, i*basm_atoi(b2));
		return(s);
	}
#endif

	if(ty!=BASM_TOKEN_NAME)
	{
		printf("BASM_ParseOpcode: Bad Opcode Token '%s'\n", b);
		return(s);
	}

	t=BASM_ParseSpecial(ctx, s, b);
	if(t)return(t);

//	ctx->fl&=~8;

	ctx->fl&=~(BASM_FL_ADDRSZ|BASM_FL_GAS);

	ctx->seg=BASM_Z;
	ctx->w=0;

	if(!basm_stricmp(b, "a16"))
	{
		if(!(ctx->fl&2))ctx->fl|=BASM_FL_ADDRSZ;
		t=BASM_ParseSpecial(ctx, s, b);
	}

	if(!basm_stricmp(b, "a32"))
	{
		if(ctx->fl&2)ctx->fl|=BASM_FL_ADDRSZ;
		t=BASM_ParseSpecial(ctx, s, b);
	}


//	for(i=0; basm_ops[i]; i++)
//		if(!basm_stricmp(basm_ops[i], b))
//			break;

	i=BASM_LookupOpcode(ctx, b);
	if(i<0)
	{
		printf("BASM_ParseOpcode: Bad Opcode '%s'\n", b);
		return(s);
	}

	if(!basm_ops[i])
	{
		j=b[strlen(b)-1];
		if((j=='b') || (j=='w') || (j=='l'))
		{
			ctx->fl|=BASM_FL_GAS;
			if(j=='b')ctx->w=8;
			if(j=='w')ctx->w=16;
			if(j=='l')ctx->w=32;
			b[j]=0;

//			for(i=0; basm_ops[i]; i++)
//				if(!basm_stricmp(basm_ops[i], b))
//					break;
			i=BASM_LookupOpcode(ctx, b);

		}else if(b[0]=='l')
		{
			ctx->fl|=BASM_FL_GAS; ctx->w=32;
//			for(i=0; basm_ops[i]; i++)
//				if(!basm_stricmp(basm_ops[i], b+1))
//					break;

			i=BASM_LookupOpcode(ctx, b+1);
		}
	}

	if((i<0) || !basm_ops[i])
	{
		printf("BASM_ParseOpcode: Bad Opcode '%s'\n", b);
		return(s);
	}

	if(BASM_OpOnlySingleP(i))
	{
		BASM_OutOpSingle(ctx, i);
//		if(!basm_stricmp(b2, ";"))s=BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

//	if(!basm_stricmp(b2, ";") || BASM_Parse_IsLineBreak(s))
	if((*s==';') || BASM_Parse_IsLineBreak(s) || !basm_stricmp(b2, "|"))
	{
		BASM_OutOpSingle(ctx, i);
//		if(!basm_stricmp(b2, ";"))s=BASM_Parse_Token(s, b2, &ty2);
		if(!basm_stricmp(b2, "|"))s=BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

	BASM_Parse_Token(s, b2, &ty2);


	if(!basm_stricmp(b2, "byte")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=8; }
	if(!basm_stricmp(b2, "word")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=16; }
	if(!basm_stricmp(b2, "dword")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=32; }
	if(!basm_stricmp(b2, "qword")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=64; }
	if(!basm_stricmp(b2, "oword")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=128; }
	if(!basm_stricmp(b2, "yword")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=256; }
	if(!basm_stricmp(b2, "tword")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=80; }

	if(!basm_stricmp(b2, "dqword")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=128; }

	if(!basm_stricmp(b2, "short")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=8; }
	if(!basm_stricmp(b2, "near16")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=16; }
	if(!basm_stricmp(b2, "near32")) { s=BASM_Parse_Token(s, b2, &ty2); ctx->w=32; }

	BASM_Parse_Token(s, b2, &ty2);

	if(!basm_stricmp(b2, "ptr")) s=BASM_Parse_Token(s, b2, &ty2);
	if(!basm_stricmp(b2, "offset")) s=BASM_Parse_Token(s, b2, &ty2);

	BASM_Parse_Token(s, b2, &ty2);

	s=BASM_ParseArg(ctx, s, &br0, &ir0, &sc0, &disp0, &l0);
	if(l0 && (*l0=='.') && ctx->label_base)
		{ sprintf(b2, "%s%s", ctx->label_base, l0);
		  l0=basm_strdup(b2); }
	BASM_Parse_Token(s, b2, &ty2);

//	if(!basm_stricmp(b2, ";") || BASM_Parse_IsLineBreak(s))
	if((*s==';') || BASM_Parse_IsLineBreak(s) || !basm_stricmp(b2, "|"))
	{
		BASM_OutOpGeneric1(ctx, i, ctx->w, l0, br0, ir0, sc0, disp0);

		BASM_Parse_Token(s, b2, &ty2);
		if(!basm_stricmp(b2, "|"))s=BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

	if(!basm_stricmp(b2, ","))s=BASM_Parse_Token(s, b2, &ty2);

	s=BASM_ParseArg(ctx, s, &br1, &ir1, &sc1, &disp1, &l1);
	if(l1 && (*l1=='.') && ctx->label_base)
		{ sprintf(b2, "%s%s", ctx->label_base, l1);
		  l1=basm_strdup(b2); }
	BASM_Parse_Token(s, b2, &ty2);

	if((*s==';') || BASM_Parse_IsLineBreak(s) || !basm_stricmp(b2, "|"))
	{
		if(ctx->fl&BASM_FL_GAS)
		{
			BASM_OutOpGeneric2(ctx, i, ctx->w,
				l1, br1, ir1, sc1, disp1,
				l0, br0, ir0, sc0, disp0);
		}else
		{
			BASM_OutOpGeneric2(ctx, i, ctx->w,
				l0, br0, ir0, sc0, disp0,
				l1, br1, ir1, sc1, disp1);
		}

		BASM_Parse_Token(s, b2, &ty2);
		if(!basm_stricmp(b2, "|"))s=BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

	BASM_Parse_Token(s, b2, &ty2);
	if(!basm_stricmp(b2, ","))s=BASM_Parse_Token(s, b2, &ty2);

	s=BASM_ParseArg(ctx, s, &br2, &ir2, &sc2, &disp2, &l2);
	if(l2 && (*l2=='.') && ctx->label_base)
		{ sprintf(b2, "%s%s", ctx->label_base, l2);
		  l2=basm_strdup(b2); }
	BASM_Parse_Token(s, b2, &ty2);

	if((*s==';') || BASM_Parse_IsLineBreak(s) || !basm_stricmp(b2, "|"))
	{
		if(ctx->fl&BASM_FL_GAS)
		{
			BASM_OutOpGeneric3(ctx, i, ctx->w,
				l2, br2, ir2, sc2, disp2,
				l1, br1, ir1, sc1, disp1,
				l0, br0, ir0, sc0, disp0);
		}else
		{
			BASM_OutOpGeneric3(ctx, i, ctx->w,
				l0, br0, ir0, sc0, disp0,
				l1, br1, ir1, sc1, disp1,
				l2, br2, ir2, sc2, disp2);
		}

		BASM_Parse_Token(s, b2, &ty2);
		if(!basm_stricmp(b2, "|"))s=BASM_Parse_Token(s, b2, &ty2);
		if(*s==';')s++;
		return(s);
	}

	printf("BASM_ParseOpcode: Opcode Parse Error\n");

	BASM_Parse_Token(s, b2, &ty2);
//	if(!basm_stricmp(b2, ";"))s=BASM_Parse_Token(s, b2, &ty2);
	if(!basm_stricmp(b2, "|"))s=BASM_Parse_Token(s, b2, &ty2);
	if(*s==';')s++;
	return(s);
}

char *BASM_ParseOpcodeList(BASM_Context *ctx, char *str)
{
	char *s;

	s=str;

	while(*s)
	{
		if(ctx->seg==0)
		{
			while(*s && (*s!='\n'))s++;
			if(*s=='\n')ctx->seg=BASM_Z;
			continue;
		}

		while(*s && (*s<=' '))s++;
		if(!*s)break;

		if((*s==';') || (*s=='#'))
		{
			ctx->seg=0;
//			printf("ASM: Comment\n");
			while(*s && (*s!='\n'))s++;
			if(*s=='\n')ctx->seg=BASM_Z;
			continue;
		}

		s=BASM_ParseOpcode(ctx, s);
	}

	return(s);
}

