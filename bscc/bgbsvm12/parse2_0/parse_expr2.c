#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgbsvm.h>

//Lit:		Literal values/builtin expressions
//PE:		(<expr>) <expr>[<expr>] <expr>.<name>
//IncDec:	++<name> --<name> + - ! ~ ... <name>++ <name>-- <expr>(<args>)       
//E:		<expr>**<expr>
//MD:		* / % \ &	*. /. %. \. &.
//AS:		+ - | ^		+. -. |. ^.
//SHLR:		<< >>
//RCmp:		< > <= >= == === != <=> <<== >>==
//Lop:		&&
//Lop2:		|| ^^
//TCond:	<expr>?<expr>:<expr>
//Attr:		:= :!= :< :> :<= :>= :<< :>> :<<= :>>=
//Equals:	= += -= *= /= \= %= &= |= ^= >>= <<=

// BGBSVM_API dyt BSVMP_Expression(char **str);

BSVM_ParseItem *bsvmp_exprs=NULL;

BGBSVM_API int BSVMP_AddExpression(char *name, dyt (*func)(char **s))
{
	BSVM_ParseItem *tmp;

	tmp=gcalloc(sizeof(BSVM_ParseItem));
	tmp->name=dystrdup(name);
	tmp->func=func;

	tmp->next=bsvmp_exprs;
	bsvmp_exprs=tmp;

	return(0);
}

dyt BSVMP_ParseExpressionName(char *name, char **s)
{
	BSVM_ParseItem *cur;

	cur=bsvmp_exprs;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur->func(s));
		cur=cur->next;
	}
	return(BGBDY_NULL);
}

dyt BSVMP_Number(char *str)
{
	long long l;
	char *s, *t;
	dyt n;
	int i, sg;

	t=str;
	if((t[0]=='0') && (t[1]=='x'))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=16;
			if((*t>='0') && (*t<='9'))l+=*t-'0';
			if((*t>='A') && (*t<='F'))l+=*t-'A'+10;
			if((*t>='a') && (*t<='f'))l+=*t-'a'+10;
			t++;
		}

		return(dyint(l));
	}

//	if(t[0]=='0')
	if((t[0]=='0') && ((t[1]=='o') || (t[1]=='c')))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=8;
			if((*t>='0') && (*t<='7'))l+=*t-'0';
				else break;
			t++;
		}

		return(dyint(l));
	}

	if((t[0]=='0') && (t[1]=='b'))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=2;
			if((*t>='0') && (*t<='1'))l+=*t-'0';
				else break;
			t++;
		}

		return(dyint(l));
	}

	if((t[0]=='0') && (t[1]=='d'))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=10;
			if((*t>='0') && (*t<='9'))l+=*t-'0';
				else break;
			t++;
		}

		return(dyint(l));
	}

	i=0; t=str;
	while(*t) { if(*t=='.')i++; if(*t=='e')i++; t++; }
	if(i)
	{
		n=dyfloat(atof(str));
		return(n);
	}

	t=str; l=0; sg=0;
	if(*t=='-') { sg=1; t++; }
	while(*t)
	{
		if(*t=='_') { t++; continue; }
		l*=10;
		if((*t>='0') && (*t<='9'))l+=*t-'0';
			else break;
		t++;
	}
	if(sg)l=-l;

	return(dyint(l));
}

s64 bsvmp_atoll(char *str)
{
	s64 l;
	char *s, *t;
	dyt n;
	int i, sg;

	t=str;
	if((t[0]=='0') && (t[1]=='x'))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=16;
			if((*t>='0') && (*t<='9'))l+=*t-'0';
			if((*t>='A') && (*t<='F'))l+=*t-'A'+10;
			if((*t>='a') && (*t<='f'))l+=*t-'a'+10;
			t++;
		}

		return(l);
	}

//	if(t[0]=='0')
	if((t[0]=='0') && ((t[1]=='o') || (t[1]=='c')))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=8;
			if((*t>='0') && (*t<='7'))l+=*t-'0';
				else break;
			t++;
		}

		return(l);
	}

	if((t[0]=='0') && (t[1]=='b'))
	{
		l=0; t+=2;
		while(*t)
		{
			if(*t=='_') { t++; continue; }
			l*=2;
			if((*t>='0') && (*t<='1'))l+=*t-'0';
				else break;
			t++;
		}

		return(l);
	}

	t=str; l=0; sg=0;
	if(*t=='-') { sg=1; t++; }
	while(*t)
	{
		if(*t=='_') { t++; continue; }
		l*=10;
		if((*t>='0') && (*t<='9'))l+=*t-'0';
			else break;
		t++;
	}
	if(sg)l=-l;

	return(l);
}

dyt BSVMP_RegEx(char *str)
{
	char *b;
	
	b=gctalloc("_regex_t", strlen(str)+1);
	strcpy(b, str);
	return((dyt)b);
}

dyt BSVMP_ExpressionBlockString(char **str, int ty)
{
	char tbuf[65536];
	char *s, *t;
	dyt n;
	int i;
	
	s=*str; t=tbuf; i=1;
	while(*s && (i>0))
	{
		if(ty==0)
		{
			//escaped escapes...
			if(!strncmp(s, "\\<[[", 4) || !strncmp(s, "\\]]>", 4))
			{
				s++;
				//*t++=*s++;
				*t++=*s++; *t++=*s++; *t++=*s++;
				continue;
			}

			//block-string escapes
			if(!strncmp(s, "<[[", 3))
			{
				i++;
				*t++=*s++; *t++=*s++; *t++=*s++;
				continue;
			}
			if(!strncmp(s, "]]>", 3))
			{
				i--;
				if(!i) { s+=3; break; }
				*t++=*s++; *t++=*s++; *t++=*s++;
				continue;
			}
		}

		if(ty==1)
		{
			if(!strncmp(s, "\"\"\"", 3))
			{
				i--;
				if(!i) { s+=3; break; }
				*t++=*s++; *t++=*s++; *t++=*s++;
				continue;
			}
		}
		if(ty==2)
		{
			if(!strncmp(s, "\'\'\'", 3))
			{
				i--;
				if(!i) { s+=3; break; }
				*t++=*s++; *t++=*s++; *t++=*s++;
				continue;
			}
		}

		*t++=*s++;
	}
	*t++=0;

	n=dystring(tbuf);

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionLit(char **str)
{
	char b[256], b2[256];
	char *s, *t, *s1;
	int ty, ty2;
	dyt n, n1, n2, n3, n4;
	int i;

	s=*str;

	s1=BSVMP_EatWhite(s);
	if(!strncmp(s1, "<[[", 3))
	{
		s=s1+3;
		n=BSVMP_ExpressionBlockString(&s, 0);
		*str=s;
		return(n);
	}
	if(!strncmp(s1, "\"\"\"", 3))
	{
		s=s1+3;
		n=BSVMP_ExpressionBlockString(&s, 1);
		*str=s;
		return(n);
	}
	if(!strncmp(s1, "\'\'\'", 3))
	{
		s=s1+3;
		n=BSVMP_ExpressionBlockString(&s, 2);
		*str=s;
		return(n);
	}


	BSVMP_Token(s, b, &ty);

	if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ";"))
		return(NULL);
	if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ","))
		return(NULL);

	if(!strcmp(b, "$"))
	{
		s=BSVMP_Token(s, b, &ty); //'$'

		if(ty==BSVM_TOKEN_NAME)
		{
			n1=BSVMP_Expression(&s);
			if(!n1)return(NULL);
			n=dylist2s("unquote", n1);
		}

		*str=s;
		return(n);
	}

	if(!strcmp(b, "/") && (ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);	//'/'
		t=b2;
		while(*s && (*s!='/'))
			*t++=*s++;
		*t++; if(*s=='/')s++;
		n=BSVMP_RegEx(b2);
		
		*str=s;
		return(n);
	}

	if(!strcmp(b, "#") && (ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);	//'#'
		s=BSVMP_Token(s, b, &ty);	//name
		BSVMP_Token(s, b2, &ty2);	//name

		n=BGBDY_NULL;
		if(!strcmp(b, "{"))
		{
			n1=BSVMP_FunArgs(&s);
			if(n1==UNDEFINED)return(NULL);
			n=dyconss("list", n1);
		}else
		if(!strcmp(b, "["))
		{
			if(!strcmp(b2, "["))
			{
//				n1=BSVMP_FunArgs(&s);
				n1=BSVMP_IListArgs(&s);
				if(n1==UNDEFINED)return(NULL);
				n=dyconss("matrix", n1);
			}else
			{
				n1=BSVMP_FunArgs(&s);
				if(n1==UNDEFINED)return(NULL);

				BSVMP_Token(s, b2, &ty2);
				if(!strcmp(b2, "Q"))
				{
					s=BSVMP_Token(s, b2, &ty2);
					n=dyconss("quat", n1);
				}else
				{
					n=dyconss("vector", n1);
				}
			}
		}else
		if(!strcmp(b, "("))
		{
			n1=BSVMP_FunArgs(&s);
			if(n1==UNDEFINED)return(NULL);
			n=dyconss("list", n1);
		}else
		if(!strcmp(b, ":"))
		{
			s=BSVMP_Token(s, b, &ty);	//name
			n=dykeyword(b);
		}else
		if(ty==BSVM_TOKEN_NAME)
		{
			n=dylist2s("quote", dysymbol(b));
		}else
		if(ty==BSVM_TOKEN_OPERATOR)
		{
			n=dylist2s("quote", dysymbol(b));
		}else
		if(ty==BSVM_TOKEN_CHARSTRING)
		{
			n=dysymbol(b);
		}else
		if(ty==BSVM_TOKEN_STRING)
		{
			n=dylist2s("quote", dysymbol(b));
		}

		*str=s;
		return(n);
	}

	s=*str;

#if 1
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
	{
		s=BSVMP_Token(s, b, &ty);	//(
//		n=BSVMP_Expression(&s);
//		s=BSVMP_Token(s, b, &ty);

		n=BSVMP_FunArgs(&s);
		if(n==UNDEFINED)return(NULL);
		n=dycons(dysymbol("begin"), n);

		*str=s;
		return(n);
	}
#endif

	s=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s, b2, &ty2);
	if(!s)return(BGBDY_NULL);

	if(!strcmp(b, "[") && (ty==BSVM_TOKEN_BRACE))
	{
		n1=BSVMP_FunArgs(&s);
		if(n1==UNDEFINED)return(NULL);

		BSVMP_Token(s, b, &ty);
		if(ty==BSVM_TOKEN_NAME)
		{
			n2=BSVMP_TypeForSuffix(b);
			if(n2)
			{
				s=BSVMP_Token(s, b, &ty);
				n=dycons2s("array_ty", n2, n1);
				*str=s;
				return(n);
			}
		}

		if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ":"))
		{
			s=BSVMP_Token(s, b, &ty);
			n2=BSVMP_ExpressionType(&s);
			n=dycons2s("array_ty", n2, n1);
			*str=s;
			return(n);
		}

		n=dyconss("array", n1);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "{") && (ty==BSVM_TOKEN_BRACE))
	{
		n1=BSVMP_FunArgs2(&s);
		if(n1==UNDEFINED)return(NULL);
		n=dyconss("dict", n1);
		*str=s;
		return(n);
	}

//	if(!strcmp(b, "<[[") && (ty==BSVM_TOKEN_BRACE))
//	{
//		n=BSVMP_ExpressionBlockString(&s);
//		*str=s;
//		return(n);
//	}

	if(ty==BSVM_TOKEN_NAME)
	{
		n=BSVMP_ParseExpressionName(b, &s);
		if(n)
		{
			*str=s;
			return(n);
		}

		if(!strcmp(b, "lambda") || !strcmp(b, "fun") ||
			!strcmp(b, "function"))
		{
			BSVMP_Token(s, b, &ty); //'('
			
			if(!strcmp(b, "["))
			{
				s=BSVMP_Token(s, b, &ty); //'['
				n3=BSVMP_FunVarsListC(&s);
				BSVMP_Token(s, b, &ty); //'('
			}else { n3=NULL; }

			if(ty==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b, &ty);
//				s=BSVMP_Token(s, b2, &ty2);
				BSVMP_Token(s, b2, &ty2);

				if(!strcmp(b2, "("))
				{
					s=BSVMP_Token(s, b2, &ty2); //'('
					n1=BSVMP_FunVarsList(&s);
					BSVMP_Token(s, b2, &ty2);
				}else { n1=NULL; }

#if 1
				if(!strcmp(b2, ":"))
				{
					s=BSVMP_Token(s, b2, &ty2);
					n4=BSVMP_ExpressionType(&s);
//					n=dylist3s("cast", n1, n);
					BSVMP_Token(s, b2, &ty);
				}else { n4=NULL; }
#endif

				if(n1==UNDEFINED)return(NULL);
				n2=BSVMP_BlockStatement2(&s);
				if(n2==UNDEFINED)return(NULL);
				if(n4)n2=dylist3s("cast", n4, n2);
				if(n3)
					{ n=dylist5s("erlambda", n3, dysymbol(b), n1, n2); }
				else 
					{ n=dylist4s("rlambda", dysymbol(b), n1, n2); }
				*str=s;
				return(n);
			}

			BSVMP_Token(s, b2, &ty2);

			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2); //'('
				n1=BSVMP_FunVarsList(&s);
				BSVMP_Token(s, b2, &ty2);
			}else { n1=NULL; }

#if 1
			if(!strcmp(b2, ":"))
			{
				s=BSVMP_Token(s, b2, &ty2);
				n4=BSVMP_ExpressionType(&s);
//				n=dylist3s("cast", n1, n);
				BSVMP_Token(s, b2, &ty2);
			}else { n4=NULL; }
#endif

			if(n1==UNDEFINED)return(NULL);
			n2=BSVMP_BlockStatement2(&s);
			if(n2==UNDEFINED)return(NULL);
			if(n4)n2=dylist3s("cast", n4, n2);
			if(n3)
					{ n=dylist4s("elambda", n3, n1, n2); }
			else	{ n=dylist3s("lambda", n1, n2); }
			*str=s;
			return(n);
		}

		if(!strcmp(b, "with"))
		{
			n1=BSVMP_Expression(&s);
			if(n1==UNDEFINED)return(NULL);
			n2=BSVMP_BlockStatement2(&s);
			if(n2==UNDEFINED)return(NULL);
			n=dylist3s("with", n1, n2);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "withtop"))
		{
			n1=BSVMP_Expression(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
			n2=BSVMP_BlockStatement2(&s);
			if(!n2 || (n2==UNDEFINED))return(NULL);
			n=dylist3s("with_top", n1, n2);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "quote"))
		{
			BSVMP_Token(s, b2, &ty2);

			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2);
				n1=BSVMP_FunArgs(&s);
				if(n1==UNDEFINED)return(NULL);
				n1=dyconss("begin", n1);
			}else
			{
				n1=BSVMP_BlockStatement2(&s);
				if(!n1 || (n1==UNDEFINED))return(NULL);
			}

			n=dylist2s("quote", n1);

			*str=s;
			return(n);
		}

		if(!strcmp(b, "unquote"))
		{
			BSVMP_Token(s, b2, &ty2);

			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2);
				n1=BSVMP_FunArgs(&s);
				if(n1==UNDEFINED)return(NULL);
				n1=dyconss("begin", n1);
			}else
			{
				n1=BSVMP_BlockStatement2(&s);
				if(!n1 || (n1==UNDEFINED))return(NULL);
			}

			n=dylist2s("unquote", n1);

			*str=s;
			return(n);
		}

		if(!strcmp(b, "new"))
		{
			n1=BSVMP_TypeExpression(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
			n=dylist2s("new", n1);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "delete"))
		{
			n1=BSVMP_Expression(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
			n=dylist2s("delete", n1);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "begin"))
		{
			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2);
				n1=BSVMP_FunArgs(&s);
				if(n1==UNDEFINED)return(NULL);
				n2=BSVMP_BlockStatement2(&s);
				if(!n2 || (n2==UNDEFINED))return(NULL);
				n=dylist3s("begin_args", n1, n2);
			}else
			{
				n=BSVMP_BlockStatement2(&s);
				if(!n || (n==UNDEFINED))return(NULL);
			}

			*str=s;
			return(n);
		}

		if(!strcmp(b, "sizeof"))
		{
			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2);		//'('
				n1=BSVMP_TypeExpression(&s);
				if(!n1 || (n1==UNDEFINED))return(NULL);
				s=BSVMP_Token(s, b2, &ty2);		//')'
				n=dylist2s("sizeof", n1);
				*str=s;
				return(n);
			}

			n1=BSVMP_TypeExpression(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
			n=dylist2s("sizeof", n1);
			*str=s;
			return(n);
		}

#if 0
		if((*s==':') && s[1] && (s[1]<=' '))
		{
			s=BSVMP_Token(s, b2, &ty2); //':'

			n=dykeyword(b);
			*str=s;
			return(n);
		}
#endif

#if 1
		BSVMP_Token(s, b2, &ty2); //':'
		while(!strcmp(b2, "::"))
		{
			s=BSVMP_Token(s, b2, &ty2); //'::'
			BSVMP_Token(s, b2, &ty2); //name
			strcat(b, "/");
			if(ty2==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b2, &ty2); //name
				strcat(b, b2);
			}
			BSVMP_Token(s, b2, &ty2); //name
		}
#endif

		if(BSVMP_CheckValidIdentifierName(b))
		{
			n=dysymbol(b);
			*str=s;
			return(n);
		}

		BSVM_MSG_Error(s, "PDSCR_Parse_ExpressionLit: "
			"Invalid Identifier '%s'\n", b);
		return(BGBDY_NULL);
	}

	BSVMP_Token(s, b2, &ty2);

	if(ty==BSVM_TOKEN_NUMBER)
	{
		if((b[0]=='0') && (b[1]=='x'))
		{
			i=0; t=b+2;
			while(*t)
			{
				if(*t=='_') { t++; continue; }
				i*=16;
				if((*t>='0') && (*t<='9'))i+=*t-'0';
				if((*t>='A') && (*t<='F'))i+=*t-'A'+10;
				if((*t>='a') && (*t<='f'))i+=*t-'a'+10;
				t++;
			}

			n=dyint(i);

			BSVMP_Token(s, b2, &ty2);
			if(!BSVMP_IsWhite(s) && (ty2==BSVM_TOKEN_NAME))
			{
				s=BSVMP_Token(s, b2, &ty2);
//				if(!strcmp(b2, "i"))n=dycomplex2(0, i);
//				if(!strcmp(b2, "I"))n=dycomplex2(0, i);
			
				if(!strcmp(b2, "l") || !strcmp(b2, "L"))
					n=dylong(bsvmp_atoll(b));
				if(!strcmp(b2, "ul") || !strcmp(b2, "UL"))
					n=dylong(bsvmp_atoll(b));
				if(!strcmp(b2, "lx") || !strcmp(b2, "LX"))
					n=dyint128str(b);
				if(!strcmp(b2, "ulx") || !strcmp(b2, "ULX"))
					n=dyint128str(b);
			}

			*str=s;
			return(n);
		}

//		i=0; t=b;
//		while(*t) { if(*t=='.')i++; if(*t=='e')i++; t++; }
//		if(i)n=dyfloat(atof(b)); else n=dyint(atoi(b));

		BSVMP_Token(s, b2, &ty2);
//		if(ty2==BSVM_TOKEN_NAME)
		if(!BSVMP_IsWhite(s) && (ty2==BSVM_TOKEN_NAME))
		{
			n=BSVMP_Number(b);

			s=BSVMP_Token(s, b2, &ty2);
			if(!strcmp(b2, "i") || !strcmp(b2, "I"))
				n=dycomplex2(0, atof(b));
			if(!strcmp(b2, "j") || !strcmp(b2, "J"))
				n=dyquat(0, 0, atof(b), 0);
			if(!strcmp(b2, "k") || !strcmp(b2, "K"))
				n=dyquat(0, 0, 0, atof(b));

			if(!strcmp(b2, "l") || !strcmp(b2, "L"))
				n=dylong(bsvmp_atoll(b));
			if(!strcmp(b2, "ul") || !strcmp(b2, "UL"))
				n=dylong(bsvmp_atoll(b));
			if(!strcmp(b2, "lx") || !strcmp(b2, "LX"))
				n=dyint128str(b);
			if(!strcmp(b2, "ulx") || !strcmp(b2, "ULX"))
				n=dyint128str(b);

			if(!strcmp(b2, "f") || !strcmp(b2, "F"))
				n=dyfloat(atof(b));
			if(!strcmp(b2, "d") || !strcmp(b2, "D"))
				n=dydouble(atof(b));
			if(!strcmp(b2, "g") || !strcmp(b2, "G"))
				n=dyfloat128str(b);

			*str=s;
			return(n);

		}

		n=BSVMP_Number(b);
		*str=s;
		return(n);
	}

	if(ty==BSVM_TOKEN_STRING)
	{
#if 0
		BSVMP_Token(s, b2, &ty2);
		while(ty2==BSVM_TOKEN_STRING)
		{
			s=BSVMP_Token(s, b2, &ty2);
			strcat(b, b2);
			BSVMP_Token(s, b2, &ty2);
		}
#endif

		n=dystring(b);
		*str=s;
		return(n);
	}

	if(ty==BSVM_TOKEN_CHARSTRING)
	{
#if 0
		BSVMP_Token(s, b2, &ty2);
		while(ty2==BSVM_TOKEN_CHARSTRING)
		{
			s=BSVMP_Token(s, b2, &ty2);
			strcat(b, b2);
			BSVMP_Token(s, b2, &ty2);
		}
#endif

//		n=NetParse_NewNode();
//		n->key=kstrdup("charstring");
//		NetParse_AddAttr(n, "value", b);

		s1=b;
		i=BSVMP_ParseChar(&s1);
		if(!(*s1))
		{
			n=dychar(i);
		}else
		{
			n=dystring(b);
		}

		*str=s;
		return(n);
	}

	BSVM_MSG_Error(s, "PDSCR_Parse_ExpressionLit: "
		"Invalid Item '%s'\n", b);
//	*(int *)(-1)=-1;

//	*str=NULL;
	return(BGBDY_NULL);
}

dyt BSVMP_ExpressionPE(char **str)
{
	char b[256], b2[256];
	char *s, *s1, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;
	int i;

	s=*str;
	
#if 0
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
	{
		s=BSVMP_Token(s, b, &ty);	//(
//		n=BSVMP_Expression(&s);
//		s=BSVMP_Token(s, b, &ty);

		n=BSVMP_FunArgs(&s);
		n=dycons(dysymbol("begin"), n);

		*str=s;
		return(n);
	}
#endif

	n=BSVMP_ExpressionLit(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		if(!BSVMP_IsWhiteOnly(s))break;
		s1=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s1, b2, &ty2);

#if 1
		if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
		{
			if(BSVMC_FormIs(n, "objref"))
			{
				s=BSVMP_Token(s, b, &ty); //'('
				n1=BSVMP_FunArgs(&s);
				if(n1==UNDEFINED)return(NULL);
				n=dycons3(dysymbol("methodcall"),
					dycadr(n), dycaddr(n), n1);
				continue;
			}

			s=BSVMP_Token(s, b, &ty); //'('
			n1=BSVMP_FunArgs(&s);
			if(n1==UNDEFINED)return(NULL);
			if(!s)
			{
				*str=s;
				return(n);
			}

			n=dycons2(dysymbol("funcall"), n, n1);
			continue;
		}
#endif

		if(!strcmp(b, "["))
		{
			s=BSVMP_Token(s, b, &ty); //[
			BSVMP_Token(s, b2, &ty2); //]
			if(!strcmp(b2, "]"))
			{
				s=BSVMP_Token(s, b2, &ty2); //]
				n=dylist2s("emptyindex", n);
				continue;
			}
			
			n1=BSVMP_Expression(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
			s=BSVMP_Token(s, b, &ty); //]
			n=dylist3s("getindex", n, n1);
			continue;
		}

		if(!strcmp(b, "."))
		{
			s=BSVMP_Token(s, b, &ty); //.
			s=BSVMP_Token(s, b, &ty); //name
			n=dylist3s("objref", n, dysymbol(b));
			continue;
		}

		if(!strcmp(b, "->"))
		{
			s=BSVMP_Token(s, b, &ty); //.
			s=BSVMP_Token(s, b, &ty); //name
			n=dylist3s("objref", n, dysymbol(b));
			continue;
		}

#if 0
		if(!strcmp(b, "::") && (ty2==BSVM_TOKEN_NAME))
		{
		}
#endif

		break;
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionCast(char **str)
{
	char *s, *t;
	dyt n, n1, n2, n3;

	s=*str;

	n1=NULL;
//	n1=BSVMP_CastTypeExpression(&s);
	if(n1 && BSVMC_FormIs(n1, "casttype"))
	{
		n2=BSVMP_ExpressionPE(&s);
		n=dylist3s("cast", dycadr(n1), n2);
		*str=s;
		return(n);
	}

	n=BSVMP_ExpressionPE(&s);
	*str=s;
	return(n);

#if 0
	n1=BSVMP_DefType(str);
	n=BSVMP_ExpressionPE(str);

//	if(n1!=BGBDY_NULL)
//		n=dylist2(n1, n);

	if(n1!=BGBDY_NULL)
		n=dylist3s("cast", n1, n);
	
	return(n);
#endif
}

dyt BSVMP_ExpressionIncDec(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "++") && (ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionIncDec(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist2s("preinc!", n1);
		*str=s;
		return(n);
	}
	if(!strcmp(b, "--") && (ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionIncDec(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist2s("predec!", n1);
		*str=s;
		return(n);
	}

	if((!strcmp(b, "+") || !strcmp(b, "-") || !strcmp(b, "!") ||
		!strcmp(b, "~") || !strcmp(b, "|") ||
//		!strcmp(b, "/") ||
		!strcmp(b, "\\") || !strcmp(b, "%") || !strcmp(b, "^") ||
		!strcmp(b, "&") || !strcmp(b, "*") || !strcmp(b, "&&") ||
		!strcmp(b, "||") || !strcmp(b, "^^") ||
		!strcmp(b, "<<") || !strcmp(b, ">>")) &&
			(ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionIncDec(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist3s("unary", dysymbol(b), n1);
		*str=s;
		return(n);
	}

	s=*str;
//	n=BSVMP_ExpressionPE(&s);
	n=BSVMP_ExpressionCast(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

#if 0
		if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
		{
			if(BSVMC_FormIs(n, "objref"))
			{
				s=BSVMP_Token(s, b, &ty); //'('
				n1=BSVMP_FunArgs(&s);
				if(n1==UNDEFINED)return(NULL);
				n=dycons3(dysymbol("methodcall"),
					dycadr(n), dycaddr(n), n1);
				continue;
			}

			s=BSVMP_Token(s, b, &ty); //'('
			n1=BSVMP_FunArgs(&s);
			if(n1==UNDEFINED)return(NULL);
			if(!s)
			{
				*str=s;
				return(n);
			}

			n=dycons2(dysymbol("funcall"), n, n1);
			continue;
		}
#endif

		if(!strcmp(b, "++"))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dylist2s("postinc!", n);
			continue;
		}
		if(!strcmp(b, "--"))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dylist2s("postdec!", n);
			continue;
		}

#if 0
		if(!strcmp(b, "i"))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dylist2s("complex", n);
			continue;
		}
#endif

		break;
	}

	*str=s;
	return(n);
}

#if 0
dyt BSVMP_ExpressionE(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionIncDec(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		t=BSVMP_Token(s, b, &ty);
		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if((s!=t) && (*s<=' ') && !(*t<=' '))
			break;
		if(strcmp(b, "**") && strcmp(b, "<<") &&
			strcmp(b, ">>") && strcmp(b, ">>>"))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionIncDec(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist4s("binary", dysymbol(b), n, n1);
	}

	*str=s;
	return(n);
}
#endif

dyt BSVMP_ExpressionMD(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionIncDec(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		t=BSVMP_Token(s, b, &ty);
		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if((s!=t) && (*s<=' ') && !(*t<=' '))
			break;

		if(strcmp(b, "*") && strcmp(b, "/") && strcmp(b, "%") &&
			strcmp(b, "\\") && strcmp(b, "&") &&
			strcmp(b, "*.") && strcmp(b, "/.") &&
			strcmp(b, "%.") && strcmp(b, "\\.") &&
			strcmp(b, "&."))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionIncDec(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist4s("binary", dysymbol(b), n, n1);
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionAS(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionMD(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		t=BSVMP_Token(s, b, &ty);
		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if((s!=t) && (*s<=' ') && !(*t<=' '))
			break;

		if(strcmp(b, "+") && strcmp(b, "-") &&
			strcmp(b, "|") && strcmp(b, "^") &&
			strcmp(b, "+.") && strcmp(b, "-.") &&
			strcmp(b, "|.") && strcmp(b, "^."))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionMD(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist4s("binary", dysymbol(b), n, n1);
	}

	*str=s;
	return(n);
}

#if 1
dyt BSVMP_ExpressionSHLR(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionAS(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		t=BSVMP_Token(s, b, &ty);
		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if((s!=t) && (*s<=' ') && !(*t<=' '))
			break;
		if(strcmp(b, "<<") && strcmp(b, ">>") && strcmp(b, ">>>"))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionAS(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist4s("binary", dysymbol(b), n, n1);
	}

	*str=s;
	return(n);
}
#endif

dyt BSVMP_ExpressionRCmp(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionSHLR(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(!strcmp(b, "instanceof"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionSHLR(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
			n=dylist3s("instanceof", n, n1);
			continue;
		}

		if(!strcmp(b, "is"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionSHLR(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
			n=dylist3s("instanceof", n, n1);
			continue;
		}

		if(!strcmp(b, "as"))
		{
			s=BSVMP_Token(s, b, &ty);
			if(*s=='!')
			{
				s++;

				n1=BSVMP_ExpressionSHLR(&s);
				if(!n1 || (n1==UNDEFINED))return(NULL);
				n=dylist3s("cast_force", n1, n);
				continue;
			}
			
			n1=BSVMP_ExpressionSHLR(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);
//			n=dylist3s("instanceof", n, n1);
//			n=dylist3s("cast", n, n1);
			n=dylist3s("cast", n1, n);
			continue;
		}

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(strcmp(b, "<") && strcmp(b, ">") &&
			strcmp(b, "<=") && strcmp(b, ">=") &&
			strcmp(b, "==") && strcmp(b, "!=") &&
			strcmp(b, "<=>") && strcmp(b, "<<==") &&
			strcmp(b, ">>==") && strcmp(b, "===") &&
			strcmp(b, "!=="))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionSHLR(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
		n=dylist4s("binary", dysymbol(b), n, n1);
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionLop(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionRCmp(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(strcmp(b, "&&"))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionRCmp(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
//		n=dylist4s("binary", dysymbol(b), n, n1);

		if(BSVMC_FormIs(n, "and"))
		{
			n=dynappend(n, dylist1(n1));
			continue;
		}

		n=dylist3s("and", n, n1);
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionLop2(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionLop(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(strcmp(b, "||"))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionLop(&s);
		if(!n1 || (n1==UNDEFINED))return(NULL);
//		n=dylist4s("binary", dysymbol(b), n, n1);

		if(BSVMC_FormIs(n, "or"))
		{
			n=dynappend(n, dylist1(n1));
			continue;
		}

		n=dylist3s("or", n, n1);
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionLop3(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionRCmp(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(!strcmp(b, "&&"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionRCmp(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);

			if(BSVMC_FormIs(n, "and"))
			{
				n=dynappend(n, dylist1(n1));
				continue;
			}

			n=dylist3s("and", n, n1);
			continue;
		}

		if(!strcmp(b, "||"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionRCmp(&s);
			if(!n1 || (n1==UNDEFINED))return(NULL);

			if(BSVMC_FormIs(n, "or"))
			{
				n=dynappend(n, dylist1(n1));
				continue;
			}

			n=dylist3s("or", n, n1);
			continue;
		}
		
		break;
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionTCond(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
//	n1=BSVMP_ExpressionLop2(&s);
//	n1=BSVMP_ExpressionLop(&s);
	n1=BSVMP_ExpressionLop3(&s);
	if(!n1 || (n1==UNDEFINED))return(NULL);
	BSVMP_Token(s, b, &ty);
	if(strcmp(b, "?"))
	{
		*str=s;
		return(n1);
	}

	s=BSVMP_Token(s, b, &ty); //?
	n2=BSVMP_ExpressionLop2(&s);
	if(!n2 || (n2==UNDEFINED))return(NULL);
	s=BSVMP_Token(s, b, &ty); //:
	n3=BSVMP_ExpressionTCond(&s);
	if(!n3 || (n3==UNDEFINED))return(NULL);
	n=dylist4s("if", n1, n2, n3);

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionFunc(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, c, n1, n2, n3;
	int i;

	s=*str;
	n=BSVMP_ExpressionTCond(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	BSVMP_Token(s, b, &ty);
	if((ty==BSVM_TOKEN_OPERATOR) || (ty==BSVM_TOKEN_SEPERATOR))
		{ *str=s; return(n); }

	if((ty==BSVM_TOKEN_BRACE) &&
		(!strcmp(b, ")") || !strcmp(b, "]") || !strcmp(b, "}")))
		{ *str=s; return(n); }

	if(!BSVMP_IsWhiteOnly(s) && !BSVMP_IsNextIndented(s))
		{ *str=s; return(n); }

	t=s;
//	printf("tf0 '%s'\n", b);

	if(BSVMC_FormIs(n, "funcall"))
	{
		c=dycddr(n);
		while(dyconsp(c))
		{
			n1=dycar(c);
			if(!dysymbolp(n1) &&
				!BSVMC_FormIs(n1, "cast"))
					break;
			if(BSVMC_FormIs(n1, "cast") && !dysymbolp(dycadr(n1)))
				break;
			c=dycdr(c);
		}

		if(c!=BGBDY_NULL) { *str=s; return(n); }

		n1=BSVMP_BlockStatement2(&t);
		if(n1!=BGBDY_NULL)
		{
			n=dylist3s("define", dycdr(n), n1);
			s=t;
		}

		*str=s;
		return(n);
	}

#if 0
	if(BSVMC_FormIs(n, "begin"))
	{
//		printf("tf1\n");

		n1=BSVMP_BlockStatement2(&t);
		if(n1!=BGBDY_NULL)
		{
			n=dylist3s("lambda", dycdr(n), n1);
			s=t;
		}

		*str=s;
		return(n);
	}
#endif

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionAttr(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
//	n=BSVMP_ExpressionTCond(&s);
	n=BSVMP_ExpressionFunc(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(strcmp(b, ":=") && strcmp(b, ":!=") && strcmp(b, ":<") &&
			strcmp(b, ":>") && strcmp(b, ":<=")  &&
			strcmp(b, ":>=") && strcmp(b, ":<<") && 
			strcmp(b, ":>>") && strcmp(b, ":<<=") &&
			strcmp(b, ":>>="))
			break;

		t=NULL;
		if(!strcmp(b, ":!="))t="!=";
		if(!strcmp(b, ":<"))t="<";
		if(!strcmp(b, ":>"))t=">";
		if(!strcmp(b, ":<="))t="<=";
		if(!strcmp(b, ":>="))t=">=";
		if(!strcmp(b, ":<<"))t="<<";
		if(!strcmp(b, ":>>"))t=">>";
		if(!strcmp(b, ":>>>"))t=">>>";
		if(!strcmp(b, ":<<="))t="<<==";
		if(!strcmp(b, ":>>="))t=">>==";

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionAttr(&s);
		if(t)n=dylist4s("relattr", dysymbol(t), n, n1);
			else n=dylist3s("attr", n, n1);
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionEquals(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionAttr(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;

		if(strcmp(b, "=") && strcmp(b, "+=") && strcmp(b, "-=") &&
		strcmp(b, "*=") && strcmp(b, "/=")  && strcmp(b, "%=")  &&
		strcmp(b, "\\=")  && strcmp(b, "&=") && strcmp(b, "|=") &&
		strcmp(b, "^=") && strcmp(b, "<<=") &&
		strcmp(b, ">>=") && strcmp(b, ">>>="))
			break;

		t=NULL;
		if(!strcmp(b, "+="))t="+";
		if(!strcmp(b, "-="))t="-";
		if(!strcmp(b, "*="))t="*";
		if(!strcmp(b, "/="))t="/";
		if(!strcmp(b, "%="))t="%";
		if(!strcmp(b, "\\="))t="\\";
		if(!strcmp(b, "&="))t="&";
		if(!strcmp(b, "|="))t="|";
		if(!strcmp(b, "^="))t="^";
		if(!strcmp(b, "<<="))t="<<";
		if(!strcmp(b, ">>="))t=">>";
		if(!strcmp(b, ">>>="))t=">>>";

		s=BSVMP_Token(s, b, &ty);

		if(dysymbolp(n))
		{
			n1=BSVMP_ExpressionEquals(&s);
			if(t)n=dylist3s("set!", n,
					dylist4s("binary", dysymbol(t), n, n1));
				else n=dylist3s("set!", n, n1);
			continue;
		}

		if(BSVMC_FormIs(n, "objref") || BSVMC_FormIs(n, "getindex"))
		{
			if(BSVMC_FormIs(n, "objref"))
			{
				n1=BSVMP_ExpressionEquals(&s);
				if(t)n1=dylist4s("binary", dysymbol(t), n, n1);
				n=dylist4s("objset!", dycadr(n), dycaddr(n), n1);
				continue;
			}
			if(BSVMC_FormIs(n, "getindex"))
			{
				n1=BSVMP_ExpressionEquals(&s);
				if(t)n1=dylist4s("binary", dysymbol(t), n, n1);
				n=dylist4s("setindex!", dycadr(n), dycaddr(n), n1);
				continue;
			}
		}

		if(BSVMC_FormIs(n, "begin"))
		{
			n1=BSVMP_ExpressionEquals(&s);
			n=dylist3s("setmulti!", dycdr(n), n1);
			continue;
		}

		n1=BSVMP_ExpressionEquals(&s);
		if(t)n1=dylist3s(t, n, n1);
		n=dylist3s("setexpr!", n, n1);
		continue;
	}

	*str=s;
	return(n);
}

dyt BSVMP_ExpressionComma(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionEquals(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);
		if(ty!=BSVM_TOKEN_SEPERATOR)break;
		if(strcmp(b, ","))break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionEquals(&s);
		n=dylist3s("comma", n, n1);
	}

	*str=s;
	return(n);
}

BGBSVM_API dyt BSVMP_Expression(char **str)
{
	return(BSVMP_ExpressionEquals(str));
}

dyt BSVMP_Expression2(char **str)
{
	return(BSVMP_ExpressionComma(str));
}

dyt BSVMP_Expression3(char **str)
{
	char b[256], b2[256];
	int ty, ty2;
	char *s;
	dyt n;

	s=*str;
	n=BSVMP_ExpressionEquals(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, ":"))
	{
		s=BSVMP_Token(s, b, &ty);
		if(dysymbolp(n))n=dykeyword(dysymbolv(n));
			else n=dylist2s("colon", n);
	}

	*str=s;
	return(n);
}

BGBSVM_API dyt BSVMP_ExpressionType(char **str)
{
//	return(BSVMP_ExpressionIncDec(str));
	return(BSVMP_TypeExpression(str));
}
