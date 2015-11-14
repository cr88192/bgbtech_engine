/*
S-Expression Parser
*/

#include <bgbgc.h>
#include <bgbdy.h>

#define BGBDY_TTY_SPECIAL		1
#define BGBDY_TTY_SYMBOL		2
#define BGBDY_TTY_STRING		3


static void *bgbdys_getpos_buf(dysReadStream strm)
	{ return(strm->data); }
static void bgbdys_setpos_buf(dysReadStream strm, void *pos)
	{ strm->data=pos; }

static int bgbdys_peekchar_buf(dysReadStream strm)
{
	if(!(*(char **)strm->data))
		return(-1);
	return(BGBGC_PeekChar((char **)(&strm->data)));
}

static int bgbdys_readchar_buf(dysReadStream strm)
{
	if(!(*(char **)strm->data))
		return(-1);
	return(BGBGC_ParseChar((char **)(&strm->data)));
}

static int bgbdys_peekidx_buf(dysReadStream strm, int idx)
{
	char *s; int i;
	s=strm->data; i=idx;
	while(i--)
	{
		if(!*s)return(-1);
		BGBGC_ParseChar(&s);
	}
	if(!*s)return(-1);
	i=BGBGC_ParseChar(&s);
	return(i);
}

BGBDY_API dysReadStream dysNewReadStreamBuf(char *buf)
{
	dysReadStream strm;

	strm=gctalloc("_dys_readstream_t", sizeof(struct dysReadStream_s));
	strm->data=buf;
	strm->get_pos_f=bgbdys_getpos_buf;
	strm->set_pos_f=bgbdys_setpos_buf;
	strm->peek_char_f=bgbdys_peekchar_buf;
	strm->read_char_f=bgbdys_readchar_buf;
	strm->peek_idx_f=bgbdys_peekidx_buf;

	return(strm);
}

BGBDY_API void dysFreeReadStream(dysReadStream strm)
	{ gcfree(strm); }

BGBDY_API dysReadPos dysGetPos(dysReadStream strm)
	{ return(strm->get_pos_f(strm)); }
BGBDY_API void dysSetPos(dysReadStream strm, dysReadPos pos)
	{ strm->set_pos_f(strm, pos); }
BGBDY_API int dysPeekChar(dysReadStream strm)
	{ return(strm->peek_char_f(strm)); }
BGBDY_API int dysReadChar(dysReadStream strm)
	{ return(strm->read_char_f(strm)); }
BGBDY_API int dysPeekCharIdx(dysReadStream strm, int idx)
	{ return(strm->peek_idx_f(strm, idx)); }

BGBDY_API int dysPeekNibble(dysReadStream strm)
{
	int i;

	i=dysPeekChar(strm);
	if(i<0)return(-1);
	if((i>='0') && (i<='9'))return(i-'0');
	if((i>='A') && (i<='F'))return(i-'A'+10);
	if((i>='a') && (i<='f'))return(i-'a'+10);
	return(-1);
}

BGBDY_API int dysReadNibble(dysReadStream strm)
{
	int i;

	i=dysReadChar(strm);
	if(i<0)return(-1);
	if((i>='0') && (i<='9'))return(i-'0');
	if((i>='A') && (i<='F'))return(i-'A'+10);
	if((i>='a') && (i<='f'))return(i-'a'+10);
	return(-1);
}

BGBDY_API int dysReadHex(dysReadStream strm)
{
	int i, j;

	i=0;
	while(dysPeekNibble(strm)>=0)
	{
		j=dysReadNibble(strm);
		i=(i<<4)|j;
	}
	return(i);
}

BGBDY_API int dysReadHex8(dysReadStream strm)
{
	int i;
	i=dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	return(i);
}

BGBDY_API int dysReadHex16(dysReadStream strm)
{
	int i;
	i=dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	return(i);
}

BGBDY_API int dysReadHex32(dysReadStream strm)
{
	int i;
	i=dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	i=(i<<4)|dysReadNibble(strm);
	return(i);
}


BGBDY_API int dysParseEatWhite(dysReadStream strm)
{
	int i, r;

	i=0;
	while((dysPeekChar(strm)>0) && (dysPeekChar(strm)<=' '))
	{
		i=1;
		dysReadChar(strm);
	}

	if(dysPeekChar(strm)==';')
	{
		while((dysPeekChar(strm)>0) && (dysPeekChar(strm)!='\n'))
		{
			i=1;
			dysReadChar(strm);
		}
	}

	if((dysPeekCharIdx(strm, 0)=='#') &&
		(dysPeekCharIdx(strm, 1)=='!'))
	{
		i=1;
		r=1;

		dysReadChar(strm);
		dysReadChar(strm);

		while((dysPeekChar(strm)>0) && r)
		{
			if((dysPeekCharIdx(strm, 0)=='!') &&
				(dysPeekCharIdx(strm, 1)=='#'))
			{
				r--;
				dysReadChar(strm);
				dysReadChar(strm);
				continue;
			}
			if((dysPeekCharIdx(strm, 0)=='#') &&
				(dysPeekCharIdx(strm, 1)=='!'))
			{
				r++;
				dysReadChar(strm);
				dysReadChar(strm);
				continue;
			}
			dysReadChar(strm);
		}
	}
	if((dysPeekCharIdx(strm, 0)=='#') &&
		(dysPeekCharIdx(strm, 1)=='|'))
	{
		i=1;
		r=1;
		dysReadChar(strm);
		dysReadChar(strm);
		while((dysPeekChar(strm)>0) && r)
		{
			if((dysPeekCharIdx(strm, 0)=='|') &&
				(dysPeekCharIdx(strm, 1)=='#'))
			{
				r--;
				dysReadChar(strm);
				dysReadChar(strm);
				continue;
			}
			if((dysPeekCharIdx(strm, 0)=='#') &&
				(dysPeekCharIdx(strm, 1)=='|'))
			{
				r++;
				dysReadChar(strm);
				dysReadChar(strm);
				continue;
			}
			dysReadChar(strm);
		}
	}
	if((dysPeekCharIdx(strm, 0)=='#') &&
		(dysPeekCharIdx(strm, 1)==';'))
	{
		i=1;
		dysReadChar(strm);
		dysReadChar(strm);
		dysParseItem(strm, 0);
	}

	if(i)dysParseEatWhite(strm);

	if(dysPeekChar(strm)<0)
		return(-1);
	return(0);
}

BGBDY_API int dysParseSpecialP(dysReadStream strm)
{
	switch(dysPeekChar(strm))
	{
	case '(': case ')':
	case '[': case ']':
	case '{': case '}':
		return(1);
		break;

	case '\'': case '\\':
	case '`': case '#':
	case ',': case '@':
		return(1);
		break;

//	case '.':
//		return(1);
//		break;

	default:
		return(0);
		break;
	}
	return(0);
}

BGBDY_API char *dysParseToken(dysReadStream strm, int *ty)
{
	char *t, *b;
	int i, j;

	b=gcralloc(256); t=b;
	*b=0;

	if(ty)*ty=0;

	i=dysParseEatWhite(strm);
	if(i<0)return(NULL);

	if(dysParseSpecialP(strm))
	{
		if(ty)*ty=BGBDY_TTY_SPECIAL;
		*t++=dysReadChar(strm);
		*t=0;

	}else if(dysPeekChar(strm)=='"') /* quoted string */
	{
		if(ty)*ty=BGBDY_TTY_STRING;
		dysReadChar(strm);
		while((dysPeekChar(strm)>0) && (dysPeekChar(strm)!='\"'))
		{
			if(dysPeekChar(strm)=='\\')
			{
				dysReadChar(strm);
				switch(dysReadChar(strm))
				{
				case 'a': *t++='\a'; break;
				case 'b': *t++='\b'; break;
				case 'e': *t++='\x1B'; break;
				case 'f': *t++='\f'; break;
				case 'n': *t++='\n'; break;
				case 'r': *t++='\r'; break;
				case 't': *t++='\t'; break;
				case 'v': *t++='\v'; break;

				case '\\': *t++='\\'; break;
				case '"': *t++='"'; break;
				case '\'': *t++='\''; break;

				case 'x': i=dysReadHex8(strm);
					BGBGC_EmitChar(&t, i); break;
				case 'u': i=dysReadHex16(strm);
					BGBGC_EmitChar(&t, i); break;
				case 'U': i=dysReadHex32(strm);
					BGBGC_EmitChar(&t, i); break;

				default: break;
				}
				
			}else BGBGC_EmitChar(&t, dysReadChar(strm));
		}
		*t++=0;
		dysReadChar(strm);
	}else
	{	/* alphanumeric string that can contain '_' */
		if(ty)*ty=BGBDY_TTY_SYMBOL;

		while(dysPeekChar(strm)>' ')
		{
			if(dysPeekChar(strm)=='\\')
			{
				if(dysPeekCharIdx(strm, 1)=='x')
				{	dysReadChar(strm); dysReadChar(strm);
					i=dysReadHex8(strm);
					BGBGC_EmitChar(&t, i); continue; }

				if(dysPeekCharIdx(strm, 1)=='u')
				{	dysReadChar(strm); dysReadChar(strm);
					i=dysReadHex16(strm);
					BGBGC_EmitChar(&t, i); continue; }

				if(dysPeekCharIdx(strm, 1)=='U')
				{	dysReadChar(strm); dysReadChar(strm);
					i=dysReadHex32(strm);
					BGBGC_EmitChar(&t, i); continue; }
			}

			if(dysParseSpecialP(strm))break;
			BGBGC_EmitChar(&t, dysReadChar(strm));
		}
		*t++=0;
	}

	return(b);
}

BGBDY_API char *dysPeekToken(dysReadStream strm, int *ty)
{
	dysReadPos pos;
	char *b;
	
	pos=dysGetPos(strm);
	b=dysParseToken(strm, ty);
	dysSetPos(strm, pos);
	return(b);
}

BGBDY_API dyt dysParseList(dysReadStream strm, int ind)
{
	dyt t, l;
	char *b;
	int ty;

	l=NULL;

	while(1)
	{
		b=dysPeekToken(strm, &ty);
		if(!b)return(NULL);

		if(ty!=BGBDY_TTY_STRING)
		{
			if(b[0]==')')break;
			if(b[0]==']')break;
			if(b[0]=='}')break;

			if((b[0]=='.') && (b[1]==0))
			{
				b=dysParseToken(strm, &ty);
				t=dysParseItem(strm, ind);
				b=dysParseToken(strm, &ty);
				l=dynreverse(l);
				dynappend(l, t);
				return(l);
			}
		}

		t=dysParseItem(strm, ind);
		l=dycons(t, l);
	}
	b=dysParseToken(strm, &ty);
	l=dynreverse(l);
	return(l);
}

static double bs1_atof(char *s)
{
	double x, y;
	int sg;

	sg=1;
	x=0;
	y=0;
	while(*s)
	{
		if(*s=='-')
		{
			sg*=-1;
			s++;
		}
		if(*s=='.')
		{
			y=1;
			s++;
		}

		if(*s>='0' && *s<='9')
		{
			x=(x*10)+(*s-'0');
			y=y*10;
			s++;
		}
	}
	if(y==0)y=1;
	x=(x*sg)/y;
	return(x);
}

static int bs1_isdigit(char c)
	{ return((c>='0') && (c<='9')); }
static int bs1_isupper(char c)
	{ return((c>='A') && (c<='Z')); }
static int bs1_islower(char c)
	{ return((c>='a') && (c<='z')); }
static int bs1_isalpha(char c)
	{ return(bs1_isupper(c) || bs1_islower(c)); }
static int bs1_isalnum(char c)
	{ return(bs1_isalpha(c) || bs1_isdigit(c)); }

static int bs1_strlen(char *str)
	{ char *s; s=str; while(*s)s++; return(s-str); }
static int bs1_lcase(char c)
	{ return(((c>='A') && (c<='Z'))?(c-'A'+'a'):c); }
static int bs1_stricmp(char *s0, char *s1)
{
	while(*s0 && *s1 && bs1_lcase(*s0)==bs1_lcase(*s1)) { s0++; s1++; }
	return((*s0==*s1)?0:(*s0>*s1)?1:-1);
}

static int bs1_atoi(char *str)
{
	char *s;
	int i, j;

	s=str;
	if((s[0]=='0') && (s[1]=='x'))
	{
		s+=2; i=0;
		while(*s)
		{
			j=-1;
			if((*s>='0') && (*s<='9'))j=*s-'0';
			if((*s>='A') && (*s<='F'))j=*s-'A'+10;
			if((*s>='a') && (*s<='f'))j=*s-'a'+10;
			if(j<0)break;
			i=i*16+j;
			s++;
		}
		return(i);
	}
	if(*s=='0')
	{
		s++; i=0;
		while(*s)
		{
			j=-1;
			if((*s>='0') && (*s<='7'))j=*s-'0';
			if(j<0)break;
			i=i*8+j;
			s++;
		}
		return(i);
	}

	i=0;
	while(*s)
	{
		j=-1;
		if((*s>='0') && (*s<='9'))j=*s-'0';
		if(j<0)break;
		i=i*10+j;
		s++;
	}
	return(i);
}

BGBDY_API dyt dysParseItem(dysReadStream strm, int ind)
{
	int ty, i;
	dyt t;
	char *s2, *t2, *b, *b1;

	t=NULL;

	dysParseEatWhite(strm);

	b=dysParseToken(strm, &ty);
	if(!b)return(NULL);

	switch(ty)
	{
	case BGBDY_TTY_STRING:
		t=dystring(b);
		break;
	case BGBDY_TTY_SYMBOL:
		if(bs1_isdigit(b[0]) || ((b[0]=='-') && bs1_isdigit(b[1])))
		{
			i=0; s2=b;
			while(*s2)
			{
				if(*s2=='.')i++;
				if(*s2=='e')i++;
				s2++;
			}

			if(i)t=dydouble(bs1_atof(b));
				else t=dylong(bs1_atoi(b));
		}else
		{
			if(b[0]==':') { t=dykeyword(b+1); }
				else if(b[bs1_strlen(b)-1]==':')
			{
				b[bs1_strlen(b)-1]=0;
				t=dykeyword(b);
			}else t=dysymbol(b);
		}
		break;
	case BGBDY_TTY_SPECIAL:
		switch(b[0])
		{
		case '(':
			t=dysParseList(strm, ind);
			break;
		case '[':
			t=dysParseList(strm, ind);
			t=dycons(dysymbol("list"), t);
			break;
		case '#':
			switch(dysPeekChar(strm))
			{
			case 't':
				b=dysParseToken(strm, &ty);
				t=BGBDY_TRUE;
				break;
			case 'f':
				b=dysParseToken(strm, &ty);
				t=BGBDY_FALSE;
				break;
			case 'z':
				b=dysParseToken(strm, &ty);
				t=NULL;
				break;

			case 'o':
				b=dysParseToken(strm, &ty);
				b[0]='0';
				t=dyint(bs1_atoi(b));
				break;
			case '(':
				t=dysParseItem(strm, ind);
				t=dyListToArray(t);
				break;
			case '[':
				dysReadChar(strm);
				t=dysParseList(strm, ind);
				t=dycons(dysymbol("vector"), t);
				break;
			case '\\':
				dysReadChar(strm);
				i=dysPeekChar(strm);
				b[1]=0;
				if(bs1_isalpha(i))b=dysParseToken(strm, &ty); /* char name */
					else dysReadChar(strm);
				if(b[1])
				{
					t=NULL;
					if(!bs1_stricmp(b, "space"))t=dyint(' ');
					if(!bs1_stricmp(b, "backspace"))t=dyint('\x08');
					if(!bs1_stricmp(b, "tab"))t=dyint('\t');
					if(!bs1_stricmp(b, "newline"))t=dyint('\n');
					if(!bs1_stricmp(b, "return"))t=dyint('\r');
					if(b[0]=='x')
					{
						s2=b+1;
						i=BGBGC_ParseHex(&s2);
						t=dyint(i);
					}
				}else t=dyint(i);
				break;
			case '"':
				b=dysParseToken(strm, &ty);
				t=dysymbol(b);
				break;
			case ':':
				dysReadChar(strm);
				b=dysParseToken(strm, &ty);
				t=dykeyword(b);
				break;

			case 'X':
				dysReadChar(strm);
				t=(dyt)dyxParseExpr(strm);
				break;

			default:
				//unknown
				t=NULL;
				break;
			}
			break;
		case '\'':
			t=dysParseItem(strm, ind);
			t=dycons(t, NULL);
			t=dycons(dysymbol("quote"), t);
			break;
		case '`':
			t=dysParseItem(strm, ind);
			t=dycons(t, NULL);
			t=dycons(dysymbol("quasiquote"), t);
			break;
		case ',':
			if(dysPeekChar(strm)=='@')
			{
				dysReadChar(strm);
				t=dysParseItem(strm, ind);
				t=dycons(t, NULL);
				t=dycons(dysymbol("unquote-splicing"), t);
			}else if(dysPeekChar(strm)==',')
			{
				dysReadChar(strm);
				t=dysParseItem(strm, ind);
				t=dycons(t, NULL);
				t=dycons(dysymbol("doublemark"), t);
			}else
			{
				t=dysParseItem(strm, ind);
				t=dycons(t, NULL);
				t=dycons(dysymbol("unquote"), t);
			}
			break;
		default:
//			printf("parse: unknown special '%c'.\n", buf[0]);
			break;
		}
		break;
	default:
//		printf("parse wrong type.\n");
		return(NULL);
		break;
	}
	return(t);
}

BGBDY_API dyt dysParseFromString(char *s)
{
	return(dysParseItem(dysNewReadStreamBuf(s), 0));
}

BGBDY_API dyt dysParseMultiFromString(char *s)
{
	dysReadStream strm;
	dyt t, l;
	int i;

	strm=dysNewReadStreamBuf(s);

	l=NULL;
	while(1)
	{
		i=dysParseEatWhite(strm);
		if(i<0)break;

		t=dysParseItem(strm, 0);
		l=dycons(t, l);
	}
	l=dynreverse(l);

	return(l);
}
