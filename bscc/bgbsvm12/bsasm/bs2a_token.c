/*
Written 2007-2010 by Brendan G Bohannon
This code is re-assigned into the public domain.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bs2asm.h>


BGBSVM_API char *BS2ASM_Parse_EatWhiteOnly(char *s)
{
	while(*s && (*s<=' '))s++;
	return(s);
}

BGBSVM_API char *BS2ASM_Parse_EatWhite(char *s)
{
	char *fn;
	int ln;
	int i;

	while(*s && (*s<=' '))s++;

	if((s[0]=='/') && (s[1]=='/'))
	{
		while(*s && (*s!='\n'))s++;
		s=BS2ASM_Parse_EatWhite(s);
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
		s=BS2ASM_Parse_EatWhite(s);
		return(s);
	}

#if 0
	if(s[0]=='#')
	{
		while(*s && (*s!='\n'))s++;
		s=BS2ASM_Parse_EatWhite(s);
		return(s);
	}
#endif

	return(s);
}

BGBSVM_API int BS2ASM_Parse_IsWhiteOnly(char *s)
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

BGBSVM_API int BS2ASM_Parse_IsLineBreak(char *se)
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


BGBSVM_API int BS2ASM_Parse_OpChar(int c)
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

BGBSVM_API int BS2ASM_Parse_MatchTokenListLen(char *str, char **lst)
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

BGBSVM_API char *BS2ASM_Parse_Token(char *s, char *b, int *ty)
{
	char *token_ops[]={
		"+++", "---", "==", "!=", "<=", ">=", "++", "--",
		"<<", ">>", "(*", "*)", NULL};

	char *t;
	int i;

	*b=0;
	s=BS2ASM_Parse_EatWhite(s);
	if(!*s)
	{
		*ty=BTK_NULL;
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

		*ty=BTK_NAME;
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

			*ty=BTK_NUMBER;
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

		*ty=BTK_NUMBER;
		return(s);
	}

	if((*s=='"') || (*s=='\'')) /* quoted string */
	{
		if(*s=='\'')i=1;
			else i=0;
		s++;
		while(*s)
		{
			if(!i)if(*s=='"')break;
			if(i)if(*s=='\'')break;

			if(*s=='\\')
			{
				s++;
				switch(*s)
				{
				case 'n':
					*t++='\n';
					break;
				case 't':
					*t++='\t';
					break;
				case 'r':
					*t++='\r';
					break;
				case '\\':
					*t++='\\';
					break;
				case '"':
					*t++='"';
					break;
				case '\'':
					*t++='\'';
					break;
				default:
					break;
				}
				s++;
			}else *t++=*s++;
		}
		*t++=0;
		if(*s)s++;

		*ty=BTK_STRING;
		if(i)*ty=BTK_CHARSTRING;
		return(s);
	}

	if((*s==',') || (*s==';') || ((*s=='.') && (s[1]!='.')))
	{
		*t++=*s++;
		*t++=0;

		*ty=BTK_SEPERATOR;
		return(s);
	}

	if(
		(*s=='{') || (*s=='}') ||
		(*s=='[') || (*s==']') ||
		(*s=='(') || (*s==')'))
	{
		*t++=*s++;
		*t++=0;

		*ty=BTK_BRACE;
		return(s);
	}

	i=BS2ASM_Parse_MatchTokenListLen(s, token_ops);
	if(i)
	{
		strncpy(t, s, i);
		t[i]=0; s+=i;
		*ty=BTK_OPERATOR;
		return(s);
	}

	if(BS2ASM_Parse_OpChar(*s))
	{
		*t++=*s++;
		*t++=0;

		*ty=BTK_OPERATOR;
		if(!bs2asm_stricmp(b, ":"))*ty=BTK_SEPERATOR;

		return(s);
	}

	*ty=BTK_NULL;
	return(NULL);
}


BGBSVM_API int BS2ASM_Parse_PeekChar(char **str)
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

BGBSVM_API int BS2ASM_Parse_ParseChar(char **str)
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

BGBSVM_API int BS2ASM_Parse_EmitChar(char **str, int j)
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

//	}else *t++=j;

	*str=(char *)t;
	return(0);
}

BGBSVM_API int BS2ASM_Parse_NameInitChar(int c)
{
	static int vals[]={
	0x00AA, 0x00BA, 0x207F, 0x0386, 0x038C, 0x03DA, 0x03DC, 0x03DE,
	0x03E0, 0x1F59, 0x1F5B, 0x1F5D, 0x05BF, 0x09B2, 0x0A02, 0x0A5E,
	0x0A74, 0x0A8D, 0x0AD0, 0x0AE0,
	0x0B9C, 0x0CDE, 0x0E84, 0x0E8A, 0x0E8D, 0x0EA5, 0x0EA7, 0x0EC6,
	0x0F00, 0x0F35, 0x0F37, 0x0F39, 0x0F97, 0x0FB9, 0x00B5, 0x00B7,
	0x02BB, 0x037A, 0x0559, 0x093D, 0x0B3D, 0x1FBE, 0x2102, 0x2107,
	0x2115, 0x2124,	0x2126, 0x2128, 0}; 

	static int rngs[]={
	0x0388, 0x038A, 0x038E, 0x03A1, 0x03A3, 0x03CE, 0x03D0, 0x03D6,
	0x03E2, 0x03F3, 0x1F00, 0x1F15, 0x1F18, 0x1F1D, 0x1F20, 0x1F45,
	0x1F48, 0x1F4D, 0x1F50, 0x1F57, 0x1F5F, 0x1F7D, 0x1F80, 0x1FB4,
	0x1FB6, 0x1FBC, 0x1FC2, 0x1FC4, 0x1FC6, 0x1FCC, 0x1FD0, 0x1FD3,
	0x1FD6, 0x1FDB, 0x1FE0, 0x1FEC, 0x1FF2, 0x1FF4, 0x1FF6, 0x1FFC,
	0x0401, 0x040C, 0x040E, 0x044F, 0x0451, 0x045C, 0x045E, 0x0481,
	0x0490, 0x04C4, 0x04C7, 0x04C8, 0x04CB, 0x04CC, 0x04D0, 0x04EB,
	0x04EE, 0x04F5, 0x04F8, 0x04F9, 0x0531, 0x0556, 0x0561, 0x0587,
	0x05B0, 0x05B9, 0x05BB, 0x05BD, 0x05C1, 0x05C2, 0x05D0, 0x05EA,
	0x05F0, 0x05F2, 0x0621, 0x063A, 0x0640, 0x0652, 0x0670, 0x06B7,
	0x06BA, 0x06BE, 0x06C0, 0x06CE, 0x06D0, 0x06DC, 0x06E5, 0x06E8,
	0x06EA, 0x06ED, 0x0901, 0x0903, 0x0905, 0x0939, 0x093E, 0x094D,
	0x0950, 0x0952, 0x0958, 0x0963, 0x0981, 0x0983, 0x0985, 0x098C,
	0x098F, 0x0990, 0x0993, 0x09A8, 0x09AA, 0x09B0, 0x09B6, 0x09B9,
	0x09BE, 0x09C4, 0x09C7, 0x09C8, 0x09CB, 0x09CD, 0x09DC, 0x09DD,
	0x09DF, 0x09E3, 0x09F0, 0x09F1, 0x0A05, 0x0A0A, 0x0A0F, 0x0A10,
	0x0A13, 0x0A28, 0x0A2A, 0x0A30, 0x0A32, 0x0A33, 0x0A35, 0x0A36,
	0x0A38, 0x0A39, 0x0A3E, 0x0A42, 0x0A47, 0x0A48, 0x0A4B, 0x0A4D,
	0x0A59, 0x0A5C, 0x0A81, 0x0A83, 0x0A85, 0x0A8B, 0x0A8F, 0x0A91,
	0x0A93, 0x0AA8, 0x0AC7, 0x0AC9, 0x0ACB, 0x0ACD, 0x00C0, 0x00D6,
	0x00D8, 0x00F6, 0x00F8, 0x01F5, 0x01FA, 0x0217, 0x0250, 0x02A8,
	0x1E00, 0x1E9B, 0x1EA0, 0x1EF9, 0x0AAA, 0x0AB0, 0x0AB2, 0x0AB3,
	0x0AB5, 0x0AB9, 0x0ABD, 0x0AC5,
	0x0B01, 0x0B03, 0x0B05, 0x0B0C, 0x0B0F, 0x0B10, 0x0B13, 0x0B28,
	0x0B2A, 0x0B30, 0x0B32, 0x0B33, 0x0B36, 0x0B39, 0x0B3E, 0x0B43,
	0x0B47, 0x0B48, 0x0B4B, 0x0B4D, 0x0B5C, 0x0B5D, 0x0B5F, 0x0B61,
	0x0B82, 0x0B83, 0x0B85, 0x0B8A, 0x0B8E, 0x0B90, 0x0B92, 0x0B95,
	0x0B99, 0x0B9A, 0x0B9E, 0x0B9F, 0x0BA3, 0x0BA4, 0x0BA8, 0x0BAA,
	0x0BAE, 0x0BB5, 0x0BB7, 0x0BB9, 0x0BBE, 0x0BC2, 0x0BC6, 0x0BC8,
	0x0BCA, 0x0BCD, 0x0C01, 0x0C03, 0x0C05, 0x0C0C, 0x0C0E, 0x0C10,
	0x0C12, 0x0C28, 0x0C2A, 0x0C33, 0x0C35, 0x0C39, 0x0C3E, 0x0C44,
	0x0C46, 0x0C48, 0x0C4A, 0x0C4D, 0x0C60, 0x0C61, 0x0C82, 0x0C83,
	0x0C85, 0x0C8C, 0x0C8E, 0x0C90, 0x0C92, 0x0CA8, 0x0CAA, 0x0CB3,
	0x0CB5, 0x0CB9, 0x0CBE, 0x0CC4, 0x0CC6, 0x0CC8, 0x0CCA, 0x0CCD,
	0x0CE0, 0x0CE1, 0x0D02, 0x0D03, 0x0D05, 0x0D0C, 0x0D0E, 0x0D10,
	0x0D12, 0x0D28, 0x0D2A, 0x0D39, 0x0D3E, 0x0D43, 0x0D46, 0x0D48,
	0x0D4A, 0x0D4D, 0x0D60, 0x0D61, 0x0E01, 0x0E3A, 0x0E40, 0x0E5B,
	0x0E81, 0x0E82, 0x0E87, 0x0E88, 0x0E94, 0x0E97, 0x0E99, 0x0E9F,
	0x0EA1, 0x0EA3, 0x0EAA, 0x0EAB, 0x0EAD, 0x0EAE, 0x0EB0, 0x0EB9,
	0x0EBB, 0x0EBD, 0x0EC0, 0x0EC4, 0x0EC8, 0x0ECD, 0x0EDC, 0x0EDD,
	0x0F18, 0x0F19, 0x0F3E, 0x0F47, 0x0F49, 0x0F69, 0x0F71, 0x0F84,
	0x0F86, 0x0F8B, 0x0F90, 0x0F95, 0x0F99, 0x0FAD, 0x0FB1, 0x0FB7,
	0x10A0, 0x10C5, 0x10D0, 0x10F6, 0x3041, 0x3093, 0x309B, 0x309C,
	0x30A1, 0x30F6, 0x30FB, 0x30FC, 0x3105, 0x312C, 0x4E00, 0x9FA5,
	0xAC00, 0xD7A3, 0x0660, 0x0669, 0x06F0, 0x06F9, 0x0966, 0x096F,
	0x09E6, 0x09EF, 0x0A66, 0x0A6F, 0x0AE6, 0x0AEF, 0x0B66, 0x0B6F,
	0x0BE7, 0x0BEF, 0x0C66, 0x0C6F, 0x0CE6, 0x0CEF, 0x0D66, 0x0D6F,
	0x0E50, 0x0E59, 0x0ED0, 0x0ED9, 0x0F20, 0x0F33, 0x02B0, 0x02B8,
	0x02BD, 0x02C1, 0x02D0, 0x02D1, 0x02E0, 0x02E4, 0x203F, 0x2040,
	0x210A, 0x2113, 0x2118, 0x211D, 0x212A, 0x2131, 0x2133, 0x2138,
	0x2160, 0x2182, 0x3005, 0x3007, 0x3021, 0x3029, 0};

	int i;

	if((c>='a') && (c<='z'))return(1);
	if((c>='A') && (c<='Z'))return(1);
	if(c=='_')return(1);

//	if((c>='0') && (c<='9'))return(0);
	if(c<128)return(0);

	for(i=0; vals[i]; i++)
		if(c==vals[i])return(1);

	for(i=0; rngs[i*2+0]; i++)
		if((c>=rngs[i*2+0]) && (c<=rngs[i*2+1]))
			return(1);

	return(0);
}

BGBSVM_API int BS2ASM_Parse_NameChar(int c)
{
	if(BS2ASM_Parse_NameInitChar(c))return(1);
	if((c>='0') && (c<='9'))return(1);
	return(0);
}


BGBSVM_API char *BS2ASM_Parse_CIfy(char *s)
{
	static char buf[1024];
	char *t;
	int i;

	t=buf;
	*t++='\"';

	while(*s)
	{
		i=BS2ASM_Parse_ParseChar(&s);

		if((i<' ') || (i>='~'))
		{
			if(i>0xFF)
			{
				sprintf(t, "\\u%04X", i); t+=6;
				continue;
			}

			switch(i)
			{
			case '\r': *t++='\\'; *t++='r'; break;
			case '\n': *t++='\\'; *t++='n'; break;
			case '\t': *t++='\\'; *t++='t'; break;
			case '\x1B': *t++='\\'; *t++='e'; break;
			default: sprintf(t, "\\x%02X", i); t+=4; break;
			}
			continue;
		}

		if(i=='\"')*t++='\\';
		if(i=='\'')*t++='\\';
		if(i=='\\')*t++='\\';
		*t++=i;
	}

	*t++='\"';
	*t=0;

	return(buf);
}

BGBSVM_API int bs2asm_strisreal(char *str)
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

BGBSVM_API s64 bs2asm_atoi(char *str)
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

BGBSVM_API char *bs2asm_itoa(s64 val)
{
	char buf[64];
	char *t;
	int sg;

	if(!val)return("0");

	t=buf+64;
	*(--t)=0;

	if(val<0) { sg=1; val=-val; } else { sg=0; }

	while(val)
	{
		*(--t)='0'+(val%10);
		val/=10;
	}
	if(sg)*(--t)='-';

	return(bs2asm_rstrdup(t));
}

BGBSVM_API int bs2asm_stricmp(char *s1, char *s2)
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


BGBSVM_API char *BS2ASM_Parse_TokenC(char *s, char *b, int *ty)
{
	char *t;
	int i, j, k, sti;

	*b=0;
	s=BS2ASM_Parse_EatWhite(s);
	if(!*s)
	{
		*ty=BTK_NULL;
		return(s);
	}
	t=b;

	i=BS2ASM_Parse_PeekChar(&s);
	if(BS2ASM_Parse_NameInitChar(i))
	{
		while(1)
		{
			if((s[0]=='\\') && ((s[1]=='u') || (s[1]=='U')))
			{
				k=2;
				if(s[1]=='u')k=4;
				if(s[1]=='U')k=8;

				s+=2; j=0;
				while(k--)
				{
					j<<=4;
					if((*s>='0') && (*s<='9'))j+=*s-'0';
					if((*s>='A') && (*s<='F'))j+=*s-'A'+10;
					if((*s>='a') && (*s<='f'))j+=*s-'a'+10;
					s++;
				}
				BS2ASM_Parse_EmitChar(&t, j);
				continue;
			}

			i=BS2ASM_Parse_PeekChar(&s);
			if(!BS2ASM_Parse_NameChar(i))break;

			i=BS2ASM_Parse_ParseChar(&s);
			BS2ASM_Parse_EmitChar(&t, i);
		}

		*t++=0;

		*ty=BTK_NAME;
		return(s);
	}

	if((*s>='0') && (*s<='9'))
	{
		sti=0;

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

//			*ty=BTK_INT;
			*ty=BTK_NUMBER;
			return(s);
		}

		while((*s>='0') && (*s<='9'))
			*t++=*s++;
		if(*s=='.')
		{
			sti=1;
			*t++=*s++;
			while((*s>='0') && (*s<='9'))
				*t++=*s++;
		}
		if(*s=='e')
		{
			sti=1;
			*t++=*s++;
			if((*s=='+') || (*s=='-'))*t++=*s++;
			while((*s>='0') && (*s<='9'))
				*t++=*s++;
		}

		*t++=0;

//		*ty=BTK_INT;
//		if(sti)*ty=BTK_FLOAT;

		*ty=BTK_NUMBER;
		return(s);
	}

	if((s[0]=='.') && (s[1]>='0') && (s[1]<='9'))
	{
		*t++='0';
		*t++='.';

		while((*s>='0') && (*s<='9'))
			*t++=*s++;
		if(*s=='e')
		{
			*t++=*s++;
			if((*s=='+') || (*s=='-'))*t++=*s++;
			while((*s>='0') && (*s<='9'))
				*t++=*s++;
		}

		*t++=0;

//		*ty=BTK_FLOAT;
		*ty=BTK_NUMBER;
		return(s);
	}

	if((*s=='"') || (*s=='\'')) /* quoted string */
	{
		if(*s=='\'')sti=1;
			else sti=0;
		s++;
		while(*s)
		{
			if(!sti)if(*s=='"')break;
			if(sti)if(*s=='\'')break;

			if((s[0]=='\\') && (s[1]=='x'))
			{
				s+=2; j=0; k=2;
				while(k--)
				{
					j<<=4;
					if((*s>='0') && (*s<='9'))j+=*s-'0';
					if((*s>='A') && (*s<='F'))j+=*s-'A'+10;
					if((*s>='a') && (*s<='f'))j+=*s-'a'+10;
					s++;
				}

				BS2ASM_Parse_EmitChar(&t, j);
				continue;
			}

			if((s[0]=='\\') && ((s[1]=='u') || (s[1]=='U')))
			{
				k=2;
				if(s[1]=='u')k=4;
				if(s[1]=='U')k=8;

				s+=2; j=0;
				while(k--)
				{
					j<<=4;
					if((*s>='0') && (*s<='9'))j+=*s-'0';
					if((*s>='A') && (*s<='F'))j+=*s-'A'+10;
					if((*s>='a') && (*s<='f'))j+=*s-'a'+10;
					s++;
				}
				BS2ASM_Parse_EmitChar(&t, j);
				continue;
			}

			if(*s=='\\')
			{
				s++;
				switch(*s++)
				{
				case 'a':
					*t++='\b';
					break;
				case 'b':
					*t++='\b';
					break;
				case 't':
					*t++='\t';
					break;
				case 'n':
					*t++='\n';
					break;
				case 'v':
					*t++='\v';
					break;
				case 'f':
					*t++='\f';
					break;
				case 'r':
					*t++='\r';
					break;
				case 'e':
					*t++='\x1B';
					break;
				case '\\':
					*t++='\\';
					break;
				case '"':
					*t++='"';
					break;
				case '\'':
					*t++='\'';
					break;
				case '?':
					*t++='?';
					break;
				default:
					break;
				}
				continue;
			}

			i=BS2ASM_Parse_ParseChar(&s);
			BS2ASM_Parse_EmitChar(&t, i);
		}
		*t++=0;
		if(*s)s++;

		*ty=BTK_STRING;
		if(sti)*ty=BTK_CHARSTRING;
		return(s);
	}

	if(!strncmp(s, "...", 3))
	{
		*t++=*s++;
		*t++=*s++;
		*t++=*s++;
		*t++=0;

		*ty=BTK_MISC;
		return(s);
	}

	if((*s==',') || (*s==';') || ((*s=='.') && (s[1]!='.')))
	{
		*t++=*s++;
		*t++=0;

		*ty=BTK_SEPERATOR;
		return(s);
	}

	if(	!strncmp(s, "#<`", 3) || !strncmp(s, "#{`", 3) ||
		!strncmp(s, "#[`", 3) || !strncmp(s, "#(`", 3)
		)
	{
		*t++=*s++; *t++=*s++; *t++=*s++; *t++=0;
		*ty=BTK_BRACE;
		return(s);
	}

	if(	(*s=='{') || (*s=='}') ||
		(*s=='[') || (*s==']') ||
		(*s=='(') || (*s==')'))
	{
		*t++=*s++;
		*t++=0;

		*ty=BTK_BRACE;
		return(s);
	}

	if(BS2ASM_Parse_OpChar(*s))
	{
		if(	!strncmp(s, "<<=", 3) ||
			!strncmp(s, ">>=", 3))
		{
			*t++=*s++;
			*t++=*s++;
			*t++=*s++;
			*t++=0;
		}else if(
			!strncmp(s, "==", 2) ||
			!strncmp(s, "<=", 2) ||
			!strncmp(s, ">=", 2) ||
			!strncmp(s, "!=", 2) ||
			!strncmp(s, "<<", 2) ||
			!strncmp(s, ">>", 2) ||
			!strncmp(s, "&&", 2) ||
			!strncmp(s, "||", 2) ||

			!strncmp(s, "++", 2) ||
			!strncmp(s, "--", 2) ||

			!strncmp(s, "+=", 2) ||
			!strncmp(s, "-=", 2) ||
			!strncmp(s, "*=", 2) ||
			!strncmp(s, "/=", 2) ||
			!strncmp(s, "%=", 2) ||
			!strncmp(s, "&=", 2) ||
			!strncmp(s, "|=", 2) ||
			!strncmp(s, "^=", 2) ||

			!strncmp(s, "##", 2) ||
			!strncmp(s, "->", 2))
		{
			*t++=*s++;
			*t++=*s++;
			*t++=0;
		}else
		{
			*t++=*s++;
			*t++=0;
		}

		*ty=BTK_OPERATOR;
		if(!strcmp(b, ":"))*ty=BTK_SEPERATOR;

		return(s);
	}

//	BS2ASM_Parse_Error(s, "BS2ASM_Parse_Token: Parse Error Char '%c'\n", *s);
	*ty=BTK_NULL;
	return(NULL);
}

BGBSVM_API char *BS2ASM_Parse_MultiToken(char *str, char **ab, int *at, int n)
{
	char tb[256];
	char *s;
	int i, ty;

	s=str;
	for(i=0; i<n; i++)
	{
		s=BS2ASM_Parse_TokenC(s, tb, &ty);
		if(ab)ab[i]=bs2asm_rstrdup(tb);
		if(at)at[i]=ty;
	}

	return(s);
}

BGBSVM_API char *BS2ASM_Parse_EatMultiToken(char *str, int n)
{
	char tb[256];
	char *s;
	int i, ty;

	s=str;
	for(i=0; i<n; i++)
		s=BS2ASM_Parse_TokenC(s, tb, &ty);
	return(s);
}

BGBSVM_API char *BS2ASM_Parse_EatToken(char *str)
	{ return(BS2ASM_Parse_EatMultiToken(str, 1)); }

BGBSVM_API char *BS2ASM_Parse_TokenQN(char *str, char *b, int *rty)
{
	char tb[256], tb1[256];
	char *s, *t, *s1;
	int ty;

	s1=BS2ASM_Parse_TokenC(str, tb, &ty);
	if(ty!=BTK_NAME)
	{
		strcpy(b, tb);
		*rty=ty;
		return(s1);
	}

	s=s1; t=tb+strlen(tb);
	while(1)
	{
		s1=s;
		s1=BS2ASM_Parse_TokenC(s1, tb1, &ty);
		if(strcmp(tb1, "."))break;
		s1=BS2ASM_Parse_TokenC(s1, tb1, &ty);
		if(ty!=BTK_NAME)break;

		*t++='/';
		strcpy(t, tb1);
		t+=strlen(t);

		s=s1;
	}

	strcpy(b, tb);
	*rty=BTK_NAME;
	return(s);
}

BGBSVM_API char *BS2ASM_Parse_MultiTokenQN(char *str, char **ab, int *at, int n)
{
	char tb[256];
	char *s;
	int i, ty;

	s=str;
	for(i=0; i<n; i++)
	{
		s=BS2ASM_Parse_TokenQN(s, tb, &ty);
		if(ab)ab[i]=bs2asm_rstrdup(tb);
		if(at)at[i]=ty;
	}

	return(s);
}

BGBSVM_API char *BS2ASM_Parse_EatMultiTokenQN(char *str, int n)
{
	char tb[256];
	char *s;
	int i, ty;

	s=str;
	for(i=0; i<n; i++)
		s=BS2ASM_Parse_TokenQN(s, tb, &ty);
	return(s);
}

BGBSVM_API char *BS2ASM_Parse_EatTokenQN(char *str)
	{ return(BS2ASM_Parse_EatMultiTokenQN(str, 1)); }
