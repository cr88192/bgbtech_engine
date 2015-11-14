#include "bggl3.h"

char *bggl3_parse_buf;	//buffer
char *bggl3_parse_name;	//filename

int BGGL3_Linenum(char *buf)
{
	char *s;
	int ncr, nlf;

	ncr=0; nlf=0;
	s=bggl3_parse_buf;
	while(s<buf)
	{
		if(*s=='\r')ncr++;
		if(*s=='\n')nlf++;
		s++;
	}

	return((ncr>nlf)?ncr:nlf);
}

void BGGL3_Warning(char *buf, char *str, ...)
{
	va_list lst;
	int i;

	i=BGGL3_Linenum(buf);
	printf("warning %s:%d: ", bggl3_parse_name, i);

	va_start(lst, str);
	vprintf(str, lst);
	va_end(lst);
}

void BGGL3_Error(char *buf, char *str, ...)
{
	va_list lst;
	int i;

	i=BGGL3_Linenum(buf);
	printf("error %s:%d: ", bggl3_parse_name, i);

	va_start(lst, str);
	vprintf(str, lst);
	va_end(lst);
	exit(-1);
}

char *BGGL3_EatWhite(char *s)
{
	int i;

	while(*s && (*s<=' '))s++;
	if(!*s)return(s);

	if((s[0]=='/') && (s[1]=='/'))
	{
		while(*s && (*s!='\r') && (*s!='\n'))s++;
		s=BGGL3_EatWhite(s);
		return(s);
	}

	if((s[0]=='/') && (s[1]=='*'))
	{
		s+=2;
		i=1;
		while(i && *s)
		{
			if((s[0]=='/') && (s[1]=='*')){i++; s++;}
			if((s[0]=='*') && (s[1]=='/')){i--; s++;}
			s++;
		}

		s=BGGL3_EatWhite(s);
		return(s);
	}

	return(s);
}

static int bggl3_fromhex(char c)
{
	return(((c>='0')&&(c<='9'))?(c-'0'):
		((c>='A')&&(c<='F'))?(c-'A'+10):
		((c>='a')&&(c<='f'))?(c-'a'+10):0);
}

static int cmp2(char *s1, char *s2)
{
	return((s1[0]==s2[0]) && (s1[1]==s2[1]));
}

char *BGGL3_ParseToken(char *s, char *b, int *ty)
{
	int i;

	s=BGGL3_EatWhite(s);
	if(!*s)
	{
		*b=0;
		*ty=BGGL3_TOKEN_NULL;
		return(s);
	}

	if((*s>='0') && (*s<='9'))
	{
		while((*s>='0') && (*s<='9'))
			*b++=*s++;
		*ty=BGGL3_TOKEN_INTEGER;
		if(*s=='.')
		{
			*b++=*s++;
			while((*s>='0') && (*s<='9'))
				*b++=*s++;
			*ty=BGGL3_TOKEN_FLOAT;
		}
		if(*s=='e')
		{
			*b++=*s++;
			if(*s=='-')*b++=*s++;
			while((*s>='0') && (*s<='9'))
				*b++=*s++;
			*ty=BGGL3_TOKEN_FLOAT;
		}
		*b++=0;

		return(s);
	}

	if(((*s>='A') && (*s<='Z')) || ((*s>='a') && (*s<='z')) || (*s=='_'))
	{
		while(((*s>='A') && (*s<='Z')) ||
			((*s>='a') && (*s<='z')) ||
			((*s>='0') && (*s<='9')) || (*s=='_'))
				*b++=*s++;
		*b++=0;

		*ty=BGGL3_TOKEN_SYMBOL;
		return(s);
	}

	if((*s=='(') || (*s==')') || (*s=='[') || (*s==']') ||
		(*s=='{') || (*s=='}'))
	{
		*b++=*s++;
		*b++=0;

		*ty=BGGL3_TOKEN_BRACE;
		return(s);
	}

	if(cmp2(s, "<=") || cmp2(s, ">=") || cmp2(s, "==") ||
		cmp2(s, "!=") || cmp2(s, ":=") || cmp2(s, "->") ||
		cmp2(s, "<-") || cmp2(s, "\\\\"))
	{
		*b++=*s++; *b++=*s++; *b++=0;
		*ty=BGGL3_TOKEN_OPERATOR;
		return(s);
	}

	if((*s=='+') || (*s=='-') || (*s=='*') || (*s=='/') ||
		(*s=='\\') || (*s=='%')||
		(*s=='^') || (*s=='<') || (*s=='>') || (*s=='='))
	{
		*b++=*s++; *b++=0;
		*ty=BGGL3_TOKEN_OPERATOR;
		return(s);
	}

	if(*s=='"')
	{
		s++;
		while(*s && (*s!='"'))
		{
			if(*s=='\\')
			{
				s++;
				switch(*s++)
				{
				case 'r':
					*b++='\r';
					break;
				case 'n':
					*b++='\n';
					break;
				case 't':
					*b++='\t';
					break;
				case 'x':
					i=(bggl3_fromhex(s[0])<<8)|
						bggl3_fromhex(s[1]);
					s+=2;
					*b++=i;
					break;

				case '\\':
					*b++='\\';
					break;
				case '\'':
					*b++='\'';
					break;
				case '"':
					*b++='"';
					break;
				default:
					break;
				}
				continue;
			}
			*b++=*s++;
		}
		if(*s=='"')s++;
		*b++=0;

		*ty=BGGL3_TOKEN_STRING;
		return(s);
	}

	*b++=*s++;
	*b++=0;
	*ty=BGGL3_TOKEN_MISC;
	return(s);
}

char *BGGL3_EatToken(char *s)
{
	char buf[256];
	int i;

	s=BGGL3_ParseToken(s, buf, &i);
	return(s);
}

elem BGGL3_ParseListItem(char **str)
{
	char buf[256];
	elem a, b;
	int ty;

	BGGL3_ParseToken(*str, buf, &ty);
	if(!strcmp(buf, ")"))return(NULL);
	if(!strcmp(buf, "]"))return(NULL);
	if(!strcmp(buf, "}"))return(NULL);

	if(!strcmp(buf, "."))
	{
		*str=BGGL3_ParseToken(*str, buf, &ty);
		return(BGGL3_ParseExpr(str));
	}

	a=BGGL3_ParseExpr(str);
	b=BGGL3_ParseListItem(str);
	return(BGGL3_CONS(a, b));
}

elem BGGL3_ParseExprInner(char **str)
{
	elem a, b, c;
	char buf[256], buf2[256];
	char *s;
	int ty, ty2;

	s=*str;
	s=BGGL3_ParseToken(s, buf, &ty);
	BGGL3_ParseToken(s, buf2, &ty2);


	if(ty==BGGL3_TOKEN_SYMBOL)
	{
		if(*s==':')
		{
			s++;
			c=BGGL3_HashKeyword(buf);
			*str=s;
			return(c);
		}

		c=BGGL3_HashSymbol(buf);
		*str=s;
		return(c);
	}
	if(ty==BGGL3_TOKEN_STRING)
	{
		c=BGGL3_HashString(buf);
		*str=s;
		return(c);
	}
	if(ty==BGGL3_TOKEN_INTEGER)
	{
		if(*s=='i')
		{
			s++;
			c=BGGL3_COMPLEX(0, atof(buf));
			*str=s;
			return(c);
		}

		c=BGGL3_INT(atoi(buf));
		*str=s;
		return(c);
	}
	if(ty==BGGL3_TOKEN_FLOAT)
	{
		if(*s=='i')
		{
			s++;
			c=BGGL3_COMPLEX(0, atof(buf));
			*str=s;
			return(c);
		}

		c=BGGL3_FLOAT(atof(buf));
		*str=s;
		return(c);
	}

	if(!strcmp(buf, "-"))
	{
		if((ty2==BGGL3_TOKEN_INTEGER) || (ty2==BGGL3_TOKEN_FLOAT))
		{
			s=BGGL3_ParseToken(s, buf2, &ty2);

			if(*s=='i')
			{
				s++;

				c=BGGL3_COMPLEX(0, -atof(buf));
				*str=s;
				return(c);
			}

			c=BGGL3_FLOAT(-atof(buf2));
			*str=s;
			return(c);
		}

/*
		a=BGGL3_ParseExpr(&s);
		*str=s;

		b=BGGL3_HashSymbol("neg");
		c=BGGL3_LIST2(b, a);
		return(c);
*/
	}

	if(!strcmp(buf, "{"))
	{
		a=BGGL3_ParseListItem(&s);
		s=BGGL3_ParseToken(s, buf, &ty);

		*str=s;
		return(a);
	}

	if(!strcmp(buf, "["))
	{
		a=BGGL3_ParseListItem(&s);
		s=BGGL3_ParseToken(s, buf, &ty);
		*str=s;

		if(!a)return(NULL);
		a=BGGL3_CONS(BGGL3_HashSymbol("list"), a);
		return(a);
	}

	if(!strcmp(buf, "("))
	{
		a=BGGL3_ParseListItem(&s);
		s=BGGL3_ParseToken(s, buf, &ty);
		*str=s;

		if(!a)return(NULL);
		if(!BGGL3_CDR(a))return(BGGL3_CAR(a));
		a=BGGL3_CONS(BGGL3_HashSymbol("begin"), a);
		return(a);
	}

	if(!strcmp(buf, "\\"))
	{
		a=BGGL3_ParseExpr(&s);
		b=BGGL3_ParseExpr(&s);
		*str=s;

		c=BGGL3_HashSymbol("fun");
		a=BGGL3_LIST3(c, a, b);
		return(a);
	}

	if(!strcmp(buf, "'"))
	{
		a=BGGL3_ParseExpr(&s);
		*str=s;

		b=BGGL3_HashSymbol("quote");
		c=BGGL3_LIST2(b, a);
		return(c);
	}

	if(!strcmp(buf, ","))
	{
		a=BGGL3_ParseExpr(&s);
		*str=s;

		b=BGGL3_HashSymbol("comma");
		c=BGGL3_LIST2(b, a);
		return(c);
	}

	if(ty==BGGL3_TOKEN_OPERATOR)
	{
		c=BGGL3_HashSymbol(buf);
		*str=s;
		return(c);
	}

	BGGL3_Error(s, "Unexpected token '%s'\n", buf);
	return(NULL);
}

elem BGGL3_ParseExprDot(char **str)
{
	elem a, b, c;
	char buf[256];
	char *s;
	int ty;

	s=*str;
	a=BGGL3_ParseExprInner(&s);
	while(1)
	{
		BGGL3_ParseToken(s, buf, &ty);
		if((*s!='.') && strcmp(buf, "->") &&
			strcmp(buf, "<-"))break;
		s=BGGL3_ParseToken(s, buf, &ty);

//		printf("DOT %s\n", buf);

		b=BGGL3_ParseExprInner(&s);

		if(!strcmp(buf, "."))
		{
			c=BGGL3_HashSymbol("quote");
			a=BGGL3_LIST2(a, BGGL3_LIST2(c, b));
		}
		if(!strcmp(buf, "->"))
			a=BGGL3_LIST2(b, a);
		if(!strcmp(buf, "<-"))
			a=BGGL3_LIST2(a, b);
	}

	*str=s;
	return(a);
}

elem BGGL3_ParseExprExp(char **str)
{
	elem a, b, c;
	char buf[256];
	char *s;
	int ty;

	s=*str;
	a=BGGL3_ParseExprDot(&s);
	while(1)
	{
		BGGL3_ParseToken(s, buf, &ty);
		if(strcmp(buf, "^"))break;
		s=BGGL3_ParseToken(s, buf, &ty);

		c=BGGL3_HashSymbol(buf);
		b=BGGL3_ParseExprDot(&s);
		a=BGGL3_LIST3(c, a, b);
	}

	*str=s;
	return(a);
}

elem BGGL3_ParseExprMD(char **str)
{
	elem a, b, c;
	char buf[256];
	char *s;
	int ty;

	s=*str;
	a=BGGL3_ParseExprExp(&s);
	while(1)
	{
		BGGL3_ParseToken(s, buf, &ty);
		if(strcmp(buf, "*") && strcmp(buf, "/") &&
			strcmp(buf, "\\\\") && strcmp(buf, "%"))
				break;
		s=BGGL3_ParseToken(s, buf, &ty);

		c=BGGL3_HashSymbol(buf);
		b=BGGL3_ParseExprExp(&s);
		a=BGGL3_LIST3(c, a, b);
	}

	*str=s;
	return(a);
}

elem BGGL3_ParseExprAS(char **str)
{
	elem a, b, c;
	char buf[256];
	char *s;
	int ty;

	s=*str;

	BGGL3_ParseToken(s, buf, &ty);
	if(!strcmp(buf, "-"))
	{
		s=BGGL3_ParseToken(s, buf, &ty);
		c=BGGL3_ParseExprMD(&s);
		*str=s;

		b=BGGL3_HashSymbol("neg");
		a=BGGL3_LIST2(b, c);
	}else a=BGGL3_ParseExprMD(&s);

	while(1)
	{
		BGGL3_ParseToken(s, buf, &ty);
		if(strcmp(buf, "+") && strcmp(buf, "-"))break;
		if((*s!='+') && (*s!='-'))break;
		s=BGGL3_ParseToken(s, buf, &ty);

		c=BGGL3_HashSymbol(buf);
		b=BGGL3_ParseExprMD(&s);
		a=BGGL3_LIST3(c, a, b);
	}

	*str=s;
	return(a);
}

elem BGGL3_ParseExprCmp(char **str)
{
	elem a, b, c;
	char buf[256];
	char *s;
	int ty;

	s=*str;
	a=BGGL3_ParseExprAS(&s);
	while(1)
	{
		BGGL3_ParseToken(s, buf, &ty);
		if(strcmp(buf, "==") && strcmp(buf, "!=") &&
			strcmp(buf, "<") && strcmp(buf, ">") &&
			strcmp(buf, "<=") && strcmp(buf, ">="))break;
		s=BGGL3_ParseToken(s, buf, &ty);

		c=BGGL3_HashSymbol(buf);
		b=BGGL3_ParseExprAS(&s);
		a=BGGL3_LIST3(c, a, b);
	}

	*str=s;
	return(a);
}

elem BGGL3_ParseExprEQ(char **str)
{
	elem a, b, c;
	char buf[256];
	char *s;
	int ty;

	s=*str;
	a=BGGL3_ParseExprCmp(&s);
	while(1)
	{
		BGGL3_ParseToken(s, buf, &ty);
		if(strcmp(buf, "=") && strcmp(buf, ":="))break;
		s=BGGL3_ParseToken(s, buf, &ty);

		c=BGGL3_HashSymbol(buf);
		b=BGGL3_ParseExprCmp(&s);
		a=BGGL3_LIST3(c, a, b);
	}

	*str=s;
	return(a);
}

elem BGGL3_ParseExpr(char **str)
{
	return(BGGL3_ParseExprEQ(str));
}

elem BGGL3_EvalString(BGGL3_State *ctx, char *name, char *buf)
{
	char *s;
	elem a, b, c;

	bggl3_parse_buf=buf;
	bggl3_parse_name=name;

	a=NULL;
	s=buf;
	while(1)
	{
		s=BGGL3_EatWhite(s);
		if(!*s)break;

		c=BGGL3_ParseExpr(&s);
//		BGGL3_Print(c);
//		printf("\n");

		a=BGGL3_EvalExpr(ctx, c);
//		BGGL3_Print(a);
//		printf("\n");
	}
	return(a);
}

elem BGGL3_LoadFile(BGGL3_State *ctx, char *name)
{
	FILE *fd;
	char *buf, *s;
	elem a, b, c;
	int sz;

	fd=fopen(name, "rb");
	if(!fd)return(NULL);

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);

	buf=malloc(sz+1);
	fread(buf, 1, sz, fd);
	buf[sz]=0;

	fclose(fd);

	bggl3_parse_buf=buf;
	bggl3_parse_name=name;

	a=NULL;
	s=buf;
	while(1)
	{
		s=BGGL3_EatWhite(s);
		if(!*s)break;

		c=BGGL3_ParseExpr(&s);
		BGGL3_Print(c);
		printf("\n");

		a=BGGL3_EvalExpr(ctx, c);
		BGGL3_Print(a);
		printf("\n");
	}
	return(a);
}
