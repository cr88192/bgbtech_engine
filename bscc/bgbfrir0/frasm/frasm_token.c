#include <bgbfrir.h>

#if 1
char *frir_strdup_alloc(char *str)
{
	char *s, *t;
	int i, hi;

	if(!frir_strtab)
	{
		frir_strtab=(char *)malloc(FRASM_SZSTRINIT);
		frir_strtabe=frir_strtab+1;
	}

	if((str>=frir_strtab) && (str<frir_strtabe))
		return(str);

	if(!str)return(NULL);
	if(!*str)return(frir_strtab);

	s=frir_strtabe;
	strcpy(s, str);
	frir_strtabe=s+strlen(s)+1;

	i=frir_strtabe-frir_strtab;
	if(i>=FRASM_SZSTRLIM)
	{
		//new string table
		frir_strtab=(char *)malloc(FRASM_SZSTRINIT);
		frir_strtabe=frir_strtab+1;
	}

	return(s);
}

BGBFRIR_API int frir_strdup_i(char *str)
{
	char *s, *t;
	int i, hi;

	if(!str)return(0);

#if 1
	if(!frir_str_varr)
	{
		for(i=0; i<FRASM_SZSTRHASH; i++)frir_str_hash[i]=0;
		frir_str_varr=malloc(FRASM_SZSTRVINIT*sizeof(char *));
		frir_str_carr=malloc(FRASM_SZSTRVINIT*sizeof(int));

		frir_str_varr[0]=NULL;
		frir_str_carr[0]=0;
		frir_str_num=1;
		frir_str_max=FRASM_SZSTRVINIT;
	}
#endif

	s=str; hi=0;
	while(*s)hi=(hi*251)+(*s++);
	hi&=FRASM_SZSTRHASH-1;

	i=frir_str_hash[hi];
	while(i)
	{
		if(!strcmp(frir_str_varr[i], str))
			return(i);
		i=frir_str_carr[i];
	}

	if((frir_str_num+1)>=frir_str_max)
	{
		i=frir_str_max+(frir_str_max>>1);
		frir_str_varr=realloc(frir_str_varr, i*sizeof(char *));
		frir_str_carr=realloc(frir_str_carr, i*sizeof(int));
		frir_str_max=i;
	}

	t=frir_strdup_alloc(str);

	i=frir_str_num++;
	frir_str_varr[i]=t;
	frir_str_carr[i]=frir_str_hash[hi];
	frir_str_hash[hi]=i;

	return(i);
}

BGBFRIR_API char *frir_strtab_i(int i)
	{ return(frir_str_varr[i]); }

BGBFRIR_API char *frir_strdup(char *str)
{
	int i;

	if(!str)return(NULL);
	i=frir_strdup_i(str);
	return(frir_str_varr[i]);
}
#endif

BGBFRIR_API int frir_strildup_i(char *str)
{
	char *s, *t, *b;
	int i;

	b=frir_ralloc(strlen(str)+1);

	s=str; t=b;
	while(*s)
	{
		i=*s++;
		if((i>='A') && (i<='Z'))
			i=i-'A'+'a';
		*t++=i;
	}
	*t++=0;

	return(frir_strdup_i(b));
}

BGBFRIR_API void *frir_ralloc(int sz)
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

BGBFRIR_API char *frir_rstrdup(char *str)
{
	char *t;
	t=frir_ralloc(strlen(str)+1);
	strcpy(t, str);
	return(t);
}

BGBFRIR_API int frir_strisreal(char *str)
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

u32 frir_atof_u32(char *str)
{
	float f;
	f=atof(str);
	return(*(u32 *)(&f));
}

u64 frir_atof_u64(char *str)
{
	double f;
	f=atof(str);
	return(*(u64 *)(&f));
}

u32 frir_atof_u32_sg(char *str, int sg)
{
	float f;
	f=atof(str)*sg;
	return(*(u32 *)(&f));
}

u64 frir_atof_u64_sg(char *str, int sg)
{
	double f;
	f=atof(str)*sg;
	return(*(u64 *)(&f));
}


s64 frir_atoi(char *str)
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

int frir_stricmp(char *s1, char *s2)
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


BGBFRIR_API char *FRASM_Parse_EatWhiteOnly(char *s)
{
	while(*s && (*s<=' '))s++;
	return(s);
}

BGBFRIR_API char *FRASM_Parse_EatWhite(char *s)
{
	char *is;
	int i;

	is=s;
	while(*s && (*s<=' '))s++;

#if 0
	if((s!=is) && (s[0]==';'))
	{
		while(*s && (*s!='\n'))s++;
		s=FRASM_Parse_EatWhite(s);
		return(s);
	}
#endif

	if((s[0]=='/') && (s[1]=='/'))
	{
		while(*s && (*s!='\n'))s++;
		s=FRASM_Parse_EatWhite(s);
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
		s=FRASM_Parse_EatWhite(s);
		return(s);
	}

	return(s);
}

BGBFRIR_API int FRASM_Parse_IsWhiteOnly(char *s)
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

BGBFRIR_API int FRASM_Parse_IsLineBreak(char *se)
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


BGBFRIR_API int FRASM_Parse_OpChar(int c)
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

BGBFRIR_API int FRASM_Parse_PeekChar(char **str)
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

BGBFRIR_API int FRASM_Parse_ParseChar(char **str)
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

BGBFRIR_API int FRASM_Parse_EmitChar(char **str, int j)
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

int FRASM_Parse_MatchTokenListLen(char *str, char **lst)
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

BGBFRIR_API char *FRASM_Parse_Token(char *s, char *b, int *ty)
{
	static char *token_ops[]={
		"+++", "---", "==", "!=", "<=", ">=", "++", "--",
		"<<", ">>", "(*", "*)", NULL};

	char *t;
	int i, j, k;

	*b=0;
	s=FRASM_Parse_EatWhite(s);
	if(!*s)
	{
		*ty=FRASM_TOKEN_NULL;
		return(s);
	}
	t=b;

	if(	((*s>='a') && (*s<='z')) ||
		((*s>='A') && (*s<='Z')) ||
		(*s=='_') || (*s=='.') || (*s=='$') || (*s=='@')
		)
	{
		while(
			((*s>='a') && (*s<='z')) ||
			((*s>='A') && (*s<='Z')) ||
			((*s>='0') && (*s<='9')) ||
			(*s=='_') || (*s=='.') ||
			(*s=='$') || (*s=='@')
			)
			*t++=*s++;
		*t++=0;

		*ty=FRASM_TOKEN_NAME;
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

			*ty=FRASM_TOKEN_NUMBER;
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

		*ty=FRASM_TOKEN_NUMBER;
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
//				FRASM_Parse_EmitChar(&t, j);
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

				FRASM_Parse_EmitChar(&t, j);
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
			j=FRASM_Parse_ParseChar(&s);
			FRASM_Parse_EmitChar(&t, j);
		}
		*t++=0;
		if(*s)s++;

		*ty=FRASM_TOKEN_STRING;
		if(i)*ty=FRASM_TOKEN_CHARSTRING;
		return(s);
	}

//	if((*s==',') || (*s==';') || ((*s=='.') && (s[1]!='.')))
	if((*s==',') || (*s==';'))
	{
		*t++=*s++;
		*t++=0;

		*ty=FRASM_TOKEN_SEPERATOR;
		return(s);
	}

	if(
		(*s=='[') || (*s==']') ||
		(*s=='{') || (*s=='}') ||
		(*s=='(') || (*s==')'))
	{
		*t++=*s++;
		*t++=0;

		*ty=FRASM_TOKEN_BRACE;
		return(s);
	}

	i=FRASM_Parse_MatchTokenListLen(s, token_ops);
	if(i)
	{
		strncpy(t, s, i);
		t[i]=0; s+=i;
		*ty=FRASM_TOKEN_OPERATOR;
		return(s);
	}

	if(FRASM_Parse_OpChar(*s))
	{
		*t++=*s++;
		*t++=0;

		*ty=FRASM_TOKEN_OPERATOR;
		if(!strcmp(b, ":"))*ty=FRASM_TOKEN_SEPERATOR;

		return(s);
	}

	*ty=FRASM_TOKEN_NULL;
	return(NULL);
}
