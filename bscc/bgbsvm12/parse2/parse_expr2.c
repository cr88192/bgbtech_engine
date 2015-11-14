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

// BGBSVM_API dytf BSVMP_Expression(char **str);

BSVM_ParseItem *bsvmp_exprs=NULL;

BGBSVM_API int BSVMP_AddExpression(char *name,
	dytf (*func)(BSVM_ParseContext *ctx, char **s))
{
	BSVM_ParseItem *tmp;

	tmp=gcalloc(sizeof(BSVM_ParseItem));
	tmp->name=dystrdup(name);
	tmp->func=func;

	tmp->next=bsvmp_exprs;
	bsvmp_exprs=tmp;

	return(0);
}

dytf BSVMP_ParseExpressionName(char *name, char **s)
{
	BSVM_ParseItem *cur;

	cur=bsvmp_exprs;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur->func(NULL, s));
		cur=cur->next;
	}
	return(DYTF_NULL);
}

int BSVMP_FormIsP(dytf a, char *s)
{
	return(dytfConsP(a) && dytfEqP(dytfCar(a), dytfSymbol(s)));
}

dytf BSVMP_Number(char *str)
{
	long long l;
	char *s, *t;
	dytf n;
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

		return(dytfInt(l));
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

		return(dytfInt(l));
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

		return(dytfInt(l));
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

		return(dytfInt(l));
	}

	i=0; t=str;
	while(*t) { if(*t=='.')i++; if(*t=='e')i++; t++; }
	if(i)
	{
		n=dytfFloat(atof(str));
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

	return(dytfInt(l));
}

s64 bsvmp_atoll(char *str)
{
	s64 l;
	char *s, *t;
	dytf n;
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

dytf BSVMP_RegEx(char *str)
{
	char *b;
	
	b=gctalloc("_regex_t", strlen(str)+1);
	strcpy(b, str);
	return(dytfPtrF(b));
}

dytf BSVMP_ExpressionBlockString(char **str, int ty)
{
	char tbuf[65536];
	char *s, *t;
	dytf n;
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

	n=dytfString(tbuf);

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionLit(char **str)
{
	char b[256], b2[256];
	char *s, *t, *s1;
	int ty, ty2;
	dytf n, n1, n2, n3, n4;
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
		return(DYTF_NULL);
	if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ","))
		return(DYTF_NULL);

	if(!strcmp(b, "$"))
	{
		s=BSVMP_Token(s, b, &ty); //'$'

		if(ty==BSVM_TOKEN_NAME)
		{
			n1=BSVMP_Expression(&s);
			if(dytfNullP(n1))return(DYTF_NULL);
			n=dytfList2s("unquote", n1);
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

		n=DYTF_NULL;
		if(!strcmp(b, "{"))
		{
			n1=BSVMP_FunArgs(&s);
			if(dytfUndefinedP(n1))return(DYTF_NULL);
			n=dytfConss("list", n1);
		}else
		if(!strcmp(b, "["))
		{
			if(!strcmp(b2, "["))
			{
//				n1=BSVMP_FunArgs(&s);
				n1=BSVMP_IListArgs(&s);
				if(dytfUndefinedP(n1))return(DYTF_NULL);
				n=dytfConss("matrix", n1);
			}else
			{
				n1=BSVMP_FunArgs(&s);
				if(dytfUndefinedP(n1))return(DYTF_NULL);

				BSVMP_Token(s, b2, &ty2);
				if(!strcmp(b2, "Q"))
				{
					s=BSVMP_Token(s, b2, &ty2);
					n=dytfConss("quat", n1);
				}else
				{
					n=dytfConss("vector", n1);
				}
			}
		}else
		if(!strcmp(b, "("))
		{
			n1=BSVMP_FunArgs(&s);
			if(dytfUndefinedP(n1))return(DYTF_NULL);
			n=dytfConss("list", n1);
		}else
		if(!strcmp(b, ":"))
		{
			s=BSVMP_Token(s, b, &ty);	//name
			n=dytfKeyword(b);
		}else
		if(ty==BSVM_TOKEN_NAME)
		{
			n=dytfList2s("quote", dytfSymbol(b));
		}else
		if(ty==BSVM_TOKEN_OPERATOR)
		{
			n=dytfList2s("quote", dytfSymbol(b));
		}else
		if(ty==BSVM_TOKEN_CHARSTRING)
		{
			n=dytfSymbol(b);
		}else
		if(ty==BSVM_TOKEN_STRING)
		{
			n=dytfList2s("quote", dytfSymbol(b));
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
		if(dytfUndefinedP(n))return(DYTF_NULL);
		n=dytfCons(dytfSymbol("begin"), n);

		*str=s;
		return(n);
	}
#endif

	s=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s, b2, &ty2);
	if(!s)return(DYTF_NULL);

	if(!strcmp(b, "[") && (ty==BSVM_TOKEN_BRACE))
	{
		n1=BSVMP_FunArgs(&s);
		if(dytfUndefinedP(n1))return(DYTF_NULL);

		BSVMP_Token(s, b, &ty);
		if(ty==BSVM_TOKEN_NAME)
		{
			n2=BSVMP_TypeForSuffix(b);
			if(!dytfNullP(n2))
			{
				s=BSVMP_Token(s, b, &ty);
				n=dytfCons2s("array_ty", n2, n1);
				*str=s;
				return(n);
			}
		}

		if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ":"))
		{
			s=BSVMP_Token(s, b, &ty);
			n2=BSVMP_ExpressionType(&s);
			n=dytfCons2s("array_ty", n2, n1);
			*str=s;
			return(n);
		}

		n=dytfConss("array", n1);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "{") && (ty==BSVM_TOKEN_BRACE))
	{
		n1=BSVMP_FunArgs2(&s);
		if(dytfUndefinedP(n1))return(DYTF_NULL);
		n=dytfConss("dict", n1);
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
		if(!dytfNullP(n))
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
			}else { n3=DYTF_NULL; }

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
				}else { n1=DYTF_NULL; }

#if 1
				if(!strcmp(b2, ":"))
				{
					s=BSVMP_Token(s, b2, &ty2);
					n4=BSVMP_ExpressionType(&s);
//					n=dytfList3s("cast", n1, n);
					BSVMP_Token(s, b2, &ty);
				}else { n4=DYTF_NULL; }
#endif

				if(dytfUndefinedP(n1))return(DYTF_NULL);
				n2=BSVMP_BlockStatement2(&s);
				if(dytfUndefinedP(n2))return(DYTF_NULL);
				if(!dytfNullP(n4))n2=dytfList3s("cast", n4, n2);
				if(!dytfNullP(n3))
					{ n=dytfList5s("erlambda", n3, dytfSymbol(b), n1, n2); }
				else 
					{ n=dytfList4s("rlambda", dytfSymbol(b), n1, n2); }
				*str=s;
				return(n);
			}

			BSVMP_Token(s, b2, &ty2);

			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2); //'('
				n1=BSVMP_FunVarsList(&s);
				BSVMP_Token(s, b2, &ty2);
			}else { n1=DYTF_NULL; }

#if 1
			if(!strcmp(b2, ":"))
			{
				s=BSVMP_Token(s, b2, &ty2);
				n4=BSVMP_ExpressionType(&s);
//				n=dytfList3s("cast", n1, n);
				BSVMP_Token(s, b2, &ty2);
			}else { n4=DYTF_NULL; }
#endif

			if(dytfUndefinedP(n1))return(DYTF_NULL);
			n2=BSVMP_BlockStatement2(&s);
			if(dytfUndefinedP(n2))return(DYTF_NULL);
			if(!dytfNullP(n4))n2=dytfList3s("cast", n4, n2);
			if(!dytfNullP(n3))
					{ n=dytfList4s("elambda", n3, n1, n2); }
			else	{ n=dytfList3s("lambda", n1, n2); }
			*str=s;
			return(n);
		}

		if(!strcmp(b, "with"))
		{
			n1=BSVMP_Expression(&s);
			if(dytfUndefinedP(n1))return(DYTF_NULL);
			n2=BSVMP_BlockStatement2(&s);
			if(dytfUndefinedP(n2))return(DYTF_NULL);
			n=dytfList3s("with", n1, n2);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "withtop"))
		{
			n1=BSVMP_Expression(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			n2=BSVMP_BlockStatement2(&s);
			if(dytfNullOrUndefinedP(n2))return(DYTF_NULL);
			n=dytfList3s("with_top", n1, n2);
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
				if(dytfUndefinedP(n1))return(DYTF_NULL);
				n1=dytfConss("begin", n1);
			}else
			{
				n1=BSVMP_BlockStatement2(&s);
				if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			}

			n=dytfList2s("quote", n1);

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
				if(dytfUndefinedP(n1))return(DYTF_NULL);
				n1=dytfConss("begin", n1);
			}else
			{
				n1=BSVMP_BlockStatement2(&s);
				if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			}

			n=dytfList2s("unquote", n1);

			*str=s;
			return(n);
		}

		if(!strcmp(b, "new"))
		{
			n1=BSVMP_TypeExpression(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			n=dytfList2s("new", n1);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "delete"))
		{
			n1=BSVMP_Expression(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			n=dytfList2s("delete", n1);
			*str=s;
			return(n);
		}

		if(!strcmp(b, "begin"))
		{
			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2);
				n1=BSVMP_FunArgs(&s);
				if(dytfUndefinedP(n1))return(DYTF_NULL);
				n2=BSVMP_BlockStatement2(&s);
				if(dytfNullOrUndefinedP(n2))return(DYTF_NULL);
				n=dytfList3s("begin_args", n1, n2);
			}else
			{
				n=BSVMP_BlockStatement2(&s);
				if(dytfNullOrUndefinedP(n))return(DYTF_NULL);
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
				if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
				s=BSVMP_Token(s, b2, &ty2);		//')'
				n=dytfList2s("sizeof", n1);
				*str=s;
				return(n);
			}

			n1=BSVMP_TypeExpression(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			n=dytfList2s("sizeof", n1);
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
			n=dytfSymbol(b);
			*str=s;
			return(n);
		}

		BSVM_MSG_Error(s, "PDSCR_Parse_ExpressionLit: "
			"Invalid Identifier '%s'\n", b);
		return(DYTF_NULL);
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

			n=dytfInt(i);

			BSVMP_Token(s, b2, &ty2);
			if(!BSVMP_IsWhite(s) && (ty2==BSVM_TOKEN_NAME))
			{
				s=BSVMP_Token(s, b2, &ty2);
//				if(!strcmp(b2, "i"))n=dycomplex2(0, i);
//				if(!strcmp(b2, "I"))n=dycomplex2(0, i);
			
				if(!strcmp(b2, "l") || !strcmp(b2, "L"))
					n=dytfLong(bsvmp_atoll(b));
				if(!strcmp(b2, "ul") || !strcmp(b2, "UL"))
					n=dytfLong(bsvmp_atoll(b));
				if(!strcmp(b2, "lx") || !strcmp(b2, "LX"))
					n=dytfWrap(dyint128str(b));
				if(!strcmp(b2, "ulx") || !strcmp(b2, "ULX"))
					n=dytfWrap(dyint128str(b));
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
				n=dytfWrap(dycomplex2(0, atof(b)));
			if(!strcmp(b2, "j") || !strcmp(b2, "J"))
				n=dytfWrap(dyquat(0, 0, atof(b), 0));
			if(!strcmp(b2, "k") || !strcmp(b2, "K"))
				n=dytfWrap(dyquat(0, 0, 0, atof(b)));

			if(!strcmp(b2, "l") || !strcmp(b2, "L"))
				n=dytfLong(bsvmp_atoll(b));
			if(!strcmp(b2, "ul") || !strcmp(b2, "UL"))
				n=dytfLong(bsvmp_atoll(b));
			if(!strcmp(b2, "lx") || !strcmp(b2, "LX"))
				n=dytfWrap(dyint128str(b));
			if(!strcmp(b2, "ulx") || !strcmp(b2, "ULX"))
				n=dytfWrap(dyint128str(b));

			if(!strcmp(b2, "f") || !strcmp(b2, "F"))
				n=dytfFloat(atof(b));
			if(!strcmp(b2, "d") || !strcmp(b2, "D"))
				n=dytfDouble(atof(b));
			if(!strcmp(b2, "g") || !strcmp(b2, "G"))
				n=dytfWrap(dyfloat128str(b));

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

		n=dytfString(b);
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
			n=dytfChar(i);
		}else
		{
			n=dytfString(b);
		}

		*str=s;
		return(n);
	}

	BSVM_MSG_Error(s, "PDSCR_Parse_ExpressionLit: "
		"Invalid Item '%s'\n", b);
//	*(int *)(-1)=-1;

//	*str=NULL;
	return(DYTF_NULL);
}

dytf BSVMP_ExpressionPE(char **str)
{
	char b[256], b2[256];
	char *s, *s1, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;
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
		n=dytfCons(dytfSymbol("begin"), n);

		*str=s;
		return(n);
	}
#endif

	n=BSVMP_ExpressionLit(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	while(1)
	{
		if(!BSVMP_IsWhiteOnly(s))break;
		s1=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s1, b2, &ty2);

#if 1
		if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
		{
			if(BSVMP_FormIsP(n, "objref"))
			{
				s=BSVMP_Token(s, b, &ty); //'('
				n1=BSVMP_FunArgs(&s);
				if(dytfUndefinedP(n1))return(DYTF_NULL);
				n=dytfCons3(dytfSymbol("methodcall"),
					dytfCadr(n), dytfCaddr(n), n1);
				continue;
			}

			s=BSVMP_Token(s, b, &ty); //'('
			n1=BSVMP_FunArgs(&s);
			if(dytfUndefinedP(n1))return(DYTF_NULL);
			if(!s)
			{
				*str=s;
				return(n);
			}

			n=dytfCons2s("funcall", n, n1);
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
				n=dytfList2s("emptyindex", n);
				continue;
			}
			
			n1=BSVMP_Expression(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			s=BSVMP_Token(s, b, &ty); //]
			n=dytfList3s("getindex", n, n1);
			continue;
		}

		if(!strcmp(b, "."))
		{
			s=BSVMP_Token(s, b, &ty); //.
			s=BSVMP_Token(s, b, &ty); //name
			n=dytfList3s("objref", n, dytfSymbol(b));
			continue;
		}

		if(!strcmp(b, "->"))
		{
			s=BSVMP_Token(s, b, &ty); //.
			s=BSVMP_Token(s, b, &ty); //name
			n=dytfList3s("objref", n, dytfSymbol(b));
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

dytf BSVMP_ExpressionCast(char **str)
{
	char *s, *t;
	dytf n, n1, n2, n3;

	s=*str;

	n1=DYTF_NULL;
//	n1=BSVMP_CastTypeExpression(&s);
	if(!dytfNullP(n1) && BSVMP_FormIsP(n1, "casttype"))
	{
		n2=BSVMP_ExpressionPE(&s);
		n=dytfList3s("cast", dytfCadr(n1), n2);
		*str=s;
		return(n);
	}

	n=BSVMP_ExpressionPE(&s);
	*str=s;
	return(n);

#if 0
	n1=BSVMP_DefType(str);
	n=BSVMP_ExpressionPE(str);

//	if(n1!=DYTF_NULL)
//		n=dytfList2(n1, n);

	if(n1!=DYTF_NULL)
		n=dytfList3s("cast", n1, n);
	
	return(n);
#endif
}

dytf BSVMP_ExpressionIncDec(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "++") && (ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionIncDec(&s);
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList2s("preinc!", n1);
		*str=s;
		return(n);
	}
	if(!strcmp(b, "--") && (ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionIncDec(&s);
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList2s("predec!", n1);
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
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList3s("unary", dytfSymbol(b), n1);
		*str=s;
		return(n);
	}

	s=*str;
//	n=BSVMP_ExpressionPE(&s);
	n=BSVMP_ExpressionCast(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

#if 0
		if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
		{
			if(BSVMP_FormIsP(n, "objref"))
			{
				s=BSVMP_Token(s, b, &ty); //'('
				n1=BSVMP_FunArgs(&s);
				if(dytfUndefinedP(n1))return(DYTF_NULL);
				n=dytfCons3(dytfSymbol("methodcall"),
					dycadr(n), dycaddr(n), n1);
				continue;
			}

			s=BSVMP_Token(s, b, &ty); //'('
			n1=BSVMP_FunArgs(&s);
			if(dytfUndefinedP(n1))return(DYTF_NULL);
			if(!s)
			{
				*str=s;
				return(n);
			}

			n=dytfCons2(dytfSymbol("funcall"), n, n1);
			continue;
		}
#endif

		if(!strcmp(b, "++"))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dytfList2s("postinc!", n);
			continue;
		}
		if(!strcmp(b, "--"))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dytfList2s("postdec!", n);
			continue;
		}

#if 0
		if(!strcmp(b, "i"))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dytfList2s("complex", n);
			continue;
		}
#endif

		break;
	}

	*str=s;
	return(n);
}

#if 0
dytf BSVMP_ExpressionE(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionIncDec(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList4s("binary", dytfSymbol(b), n, n1);
	}

	*str=s;
	return(n);
}
#endif

dytf BSVMP_ExpressionMD(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionIncDec(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList4s("binary", dytfSymbol(b), n, n1);
	}

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionAS(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionMD(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList4s("binary", dytfSymbol(b), n, n1);
	}

	*str=s;
	return(n);
}

#if 1
dytf BSVMP_ExpressionSHLR(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionAS(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList4s("binary", dytfSymbol(b), n, n1);
	}

	*str=s;
	return(n);
}
#endif

dytf BSVMP_ExpressionRCmp(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionSHLR(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(!strcmp(b, "instanceof"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionSHLR(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			n=dytfList3s("instanceof", n, n1);
			continue;
		}

		if(!strcmp(b, "is"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionSHLR(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
			n=dytfList3s("instanceof", n, n1);
			continue;
		}

		if(!strcmp(b, "as"))
		{
			s=BSVMP_Token(s, b, &ty);
			if(*s=='!')
			{
				s++;

				n1=BSVMP_ExpressionSHLR(&s);
				if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
				n=dytfList3s("cast_force", n1, n);
				continue;
			}
			
			n1=BSVMP_ExpressionSHLR(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
//			n=dytfList3s("instanceof", n, n1);
//			n=dytfList3s("cast", n, n1);
			n=dytfList3s("cast", n1, n);
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
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
		n=dytfList4s("binary", dytfSymbol(b), n, n1);
	}

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionLop(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionRCmp(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(strcmp(b, "&&"))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionRCmp(&s);
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
//		n=dytfList4s("binary", dytfSymbol(b), n, n1);

		if(BSVMP_FormIsP(n, "and"))
		{
			n=dytfNAppend(n, dytfList1(n1));
			continue;
		}

		n=dytfList3s("and", n, n1);
	}

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionLop2(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionLop(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(strcmp(b, "||"))
			break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionLop(&s);
		if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
//		n=dytfList4s("binary", dytfSymbol(b), n, n1);

		if(BSVMP_FormIsP(n, "or"))
		{
			n=dytfNAppend(n, dytfList1(n1));
			continue;
		}

		n=dytfList3s("or", n, n1);
	}

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionLop3(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionRCmp(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);

		if(ty!=BSVM_TOKEN_OPERATOR)
			break;
		if(!strcmp(b, "&&"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionRCmp(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);

			if(BSVMP_FormIsP(n, "and"))
			{
				n=dytfNAppend(n, dytfList1(n1));
				continue;
			}

			n=dytfList3s("and", n, n1);
			continue;
		}

		if(!strcmp(b, "||"))
		{
			s=BSVMP_Token(s, b, &ty);
			n1=BSVMP_ExpressionRCmp(&s);
			if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);

			if(BSVMP_FormIsP(n, "or"))
			{
				n=dytfNAppend(n, dytfList1(n1));
				continue;
			}

			n=dytfList3s("or", n, n1);
			continue;
		}
		
		break;
	}

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionTCond(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
//	n1=BSVMP_ExpressionLop2(&s);
//	n1=BSVMP_ExpressionLop(&s);
	n1=BSVMP_ExpressionLop3(&s);
	if(dytfNullOrUndefinedP(n1))return(DYTF_NULL);
	BSVMP_Token(s, b, &ty);
	if(strcmp(b, "?"))
	{
		*str=s;
		return(n1);
	}

	s=BSVMP_Token(s, b, &ty); //?
	n2=BSVMP_ExpressionLop2(&s);
	if(dytfNullOrUndefinedP(n2))return(DYTF_NULL);
	s=BSVMP_Token(s, b, &ty); //:
	n3=BSVMP_ExpressionTCond(&s);
	if(dytfNullOrUndefinedP(n3))return(DYTF_NULL);
	n=dytfList4s("if", n1, n2, n3);

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionFunc(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, c, n1, n2, n3;
	int i;

	s=*str;
	n=BSVMP_ExpressionTCond(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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

	if(BSVMP_FormIsP(n, "funcall"))
	{
		c=dytfCddr(n);
		while(dytfConsP(c))
		{
			n1=dytfCar(c);
			if(!dytfSymbolP(n1) &&
				!BSVMP_FormIsP(n1, "cast"))
					break;
			if(BSVMP_FormIsP(n1, "cast") && !dytfSymbolP(dytfCadr(n1)))
				break;
			c=dytfCdr(c);
		}

		if(!dytfNullP(c)) { *str=s; return(n); }

		n1=BSVMP_BlockStatement2(&t);
		if(!dytfNullP(n1))
		{
			n=dytfList3s("define", dytfCdr(n), n1);
			s=t;
		}

		*str=s;
		return(n);
	}

#if 0
	if(BSVMP_FormIsP(n, "begin"))
	{
//		printf("tf1\n");

		n1=BSVMP_BlockStatement2(&t);
		if(n1!=DYTF_NULL)
		{
			n=dytfList3s("lambda", dytfCdr(n), n1);
			s=t;
		}

		*str=s;
		return(n);
	}
#endif

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionAttr(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
//	n=BSVMP_ExpressionTCond(&s);
	n=BSVMP_ExpressionFunc(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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
		if(t)n=dytfList4s("relattr", dytfSymbol(t), n, n1);
			else n=dytfList3s("attr", n, n1);
	}

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionEquals(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionAttr(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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

		if(dytfSymbolP(n))
		{
			n1=BSVMP_ExpressionEquals(&s);
			if(t)n=dytfList3s("set!", n,
					dytfList4s("binary", dytfSymbol(t), n, n1));
				else n=dytfList3s("set!", n, n1);
			continue;
		}

		if(BSVMP_FormIsP(n, "objref") || BSVMP_FormIsP(n, "getindex"))
		{
			if(BSVMP_FormIsP(n, "objref"))
			{
				n1=BSVMP_ExpressionEquals(&s);
				if(t)n1=dytfList4s("binary", dytfSymbol(t), n, n1);
				n=dytfList4s("objset!", dytfCadr(n), dytfCaddr(n), n1);
				continue;
			}
			if(BSVMP_FormIsP(n, "getindex"))
			{
				n1=BSVMP_ExpressionEquals(&s);
				if(t)n1=dytfList4s("binary", dytfSymbol(t), n, n1);
				n=dytfList4s("setindex!", dytfCadr(n), dytfCaddr(n), n1);
				continue;
			}
		}

		if(BSVMP_FormIsP(n, "begin"))
		{
			n1=BSVMP_ExpressionEquals(&s);
			n=dytfList3s("setmulti!", dytfCdr(n), n1);
			continue;
		}

		n1=BSVMP_ExpressionEquals(&s);
		if(t)n1=dytfList3s(t, n, n1);
		n=dytfList3s("setexpr!", n, n1);
		continue;
	}

	*str=s;
	return(n);
}

dytf BSVMP_ExpressionComma(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	n=BSVMP_ExpressionEquals(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	while(1)
	{
		BSVMP_Token(s, b, &ty);
		if(ty!=BSVM_TOKEN_SEPERATOR)break;
		if(strcmp(b, ","))break;

		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_ExpressionEquals(&s);
		n=dytfList3s("comma", n, n1);
	}

	*str=s;
	return(n);
}

BGBSVM_API dytf BSVMP_Expression(char **str)
{
	return(BSVMP_ExpressionEquals(str));
}

dytf BSVMP_Expression2(char **str)
{
	return(BSVMP_ExpressionComma(str));
}

dytf BSVMP_Expression3(char **str)
{
	char b[256], b2[256];
	int ty, ty2;
	char *s;
	dytf n;

	s=*str;
	n=BSVMP_ExpressionEquals(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, ":"))
	{
		s=BSVMP_Token(s, b, &ty);
		if(dytfSymbolP(n))n=dytfKeyword(dytfSymbolv(n));
			else n=dytfList2s("colon", n);
	}

	*str=s;
	return(n);
}

BGBSVM_API dytf BSVMP_ExpressionType(char **str)
{
//	return(BSVMP_ExpressionIncDec(str));
	return(BSVMP_TypeExpression(str));
}
