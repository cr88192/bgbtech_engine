#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgbsvm.h>


dyt BSVMP_FunArgs(char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(s && *s)
	{
		b[0]=0;
		BSVMP_Token(s, b, &ty);
		if(!*b)break;
		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}") || !strcmp(b, ">")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		n=BSVMP_Expression3(&s);
		if(!n || (n==UNDEFINED))return(UNDEFINED);

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		BSVMP_Token(s, b, &ty);
//		if((*b==')') || (*b==']') || (*b=='}'))break;
		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}") || !strcmp(b, ">")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, ";"))
		{
			s=BSVMP_Token(s, b, &ty);

			n=BSVMP_Expression3(&s);
			lst=dynappend(lst, n);
			continue;
		}

#if 0
		if(strcmp(b, ","))
		{
			BSVM_MSG_Error(s, "BSVMP_FunArgs: "
				"Invalid Token '%s'\n", b);
			*(int *)(-1)=-1;
		}
#endif

		if(!strcmp(b, ","))
		{
			s=BSVMP_Token(s, b, &ty);
			continue;
		}
	}

	*str=s;
	return(lst);
}

dyt BSVMP_FunArgs2(char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(s && *s)
	{
		b[0]=0;
		BSVMP_Token(s, b, &ty);
		if(!*b)break;
		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		s1=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s1, b2, &ty2);

		if(((ty==BSVM_TOKEN_NAME) || (ty==BSVM_TOKEN_STRING)) &&
			!strcmp(b2, ":"))
		{
			s=BSVMP_Token(s, b, &ty);
			s=BSVMP_Token(s, b2, &ty2);

			n=BSVMP_Expression3(&s);
			if(!n || (n==UNDEFINED))return(UNDEFINED);

			n=dylist3s("attr", dysymbol(b), n);
			lst=dynappend(lst, dycons(n, BGBDY_NULL));
		}else
		{
			n=BSVMP_Expression3(&s);
			if(!n || (n==UNDEFINED))return(UNDEFINED);

			lst=dynappend(lst, dycons(n, BGBDY_NULL));
		}

		BSVMP_Token(s, b, &ty);

		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, ",") || !strcmp(b, ";"))
		{
			s=BSVMP_Token(s, b, &ty);
			continue;
		}
	}

	*str=s;
	return(lst);
}

dyt BSVMP_IListArgs(char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(s && *s)
	{
		b[0]=0;
		BSVMP_Token(s, b, &ty);
		if(!*b)break;
		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, "(") || !strcmp(b, "[") ||
			!strcmp(b, "{")))
		{
			s=BSVMP_Token(s, b, &ty);
			n=BSVMP_FunArgs(&s);
			if(n==UNDEFINED)return(UNDEFINED);
		}else
		{
			n=BSVMP_Expression3(&s);
			if(!n || (n==UNDEFINED))return(UNDEFINED);
		}

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		BSVMP_Token(s, b, &ty);
		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, ";"))
		{
			s=BSVMP_Token(s, b, &ty);

			n=BSVMP_Expression3(&s);
			if(!n || (n==UNDEFINED))return(UNDEFINED);
			
			lst=dynappend(lst, n);
			continue;
		}

		if(!strcmp(b, ","))
		{
			s=BSVMP_Token(s, b, &ty);
			continue;
		}
	}

	*str=s;
	return(lst);
}

dyt BSVMP_RListArgs(char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(s && *s)
	{
		b[0]=0;
		BSVMP_Token(s, b, &ty);
		if(!*b)break;
		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, "(") || !strcmp(b, "[") ||
			!strcmp(b, "{")))
		{
			s=BSVMP_Token(s, b, &ty);
			n=BSVMP_RListArgs(&s);
			if(n==UNDEFINED)return(UNDEFINED);
		}else
		{
			n=BSVMP_Expression3(&s);
			if(!n || (n==UNDEFINED))return(UNDEFINED);
		}

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		BSVMP_Token(s, b, &ty);
		if((ty==BSVM_TOKEN_BRACE) &&
			(!strcmp(b, ")") || !strcmp(b, "]") ||
			!strcmp(b, "}")))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, ";"))
		{
			s=BSVMP_Token(s, b, &ty);

			n=BSVMP_Expression3(&s);
			if(!n || (n==UNDEFINED))return(UNDEFINED);
			lst=dynappend(lst, n);
			continue;
		}

		if(!strcmp(b, ","))
		{
			s=BSVMP_Token(s, b, &ty);
			continue;
		}
	}

	*str=s;
	return(lst);
}


dyt BSVMP_VarsList(char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(s && *s)
	{
		BSVMP_Token(s, b, &ty);
		if(!strcmp(b, ";"))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		s=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s, b2, &ty2);

		if(ty!=BSVM_TOKEN_NAME)
		{
			BSVM_MSG_Error(s, "PDSCR_Parse_VarsList: Expected name, "
				"got '%s'\n", b);
			*str=NULL;
			return(BGBDY_NULL);
		}

		n=dysymbol(b);

		BSVMP_Token(s, b2, &ty);

#if 1
		if(!strcmp(b2, ":"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_ExpressionType(&s);
			n=dylist3s("cast", n1, n);
			BSVMP_Token(s, b2, &ty);
		}
#endif

		if(!strcmp(b2, "="))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_Expression(&s);
//			n=dycons(n, dycons(n1, BGBDY_NULL));
			n=dylist3s("set!", n, n1);
			BSVMP_Token(s, b2, &ty2);
		}

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		s=BSVMP_Token(s, b, &ty);
		if(!strcmp(b, ";"))break;

		if(*b!=',')
		{
			BSVM_MSG_Error(s, "PDSCR_Parse_VarsList: Invalid token %s "
				"in vars list\n", b);
			*str=NULL;
			return(BGBDY_NULL);
		}
	}

	*str=s;
	return(lst);
}

dyt BSVMP_TryVarsList2(char **str, dyt type)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(s && *s)
	{
#if 0
		BSVMP_Token(s, b, &ty);
		if(!strcmp(b, ";"))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}
#endif

		s=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s, b2, &ty2);

		if(ty!=BSVM_TOKEN_NAME)
		{
//			printf("BSVMP_TryVarsList: Bad Token A '%s'\n", b);
			return(BGBDY_NULL);
		}

		if(!BSVMP_CheckValidDeclName(b))
			return(BGBDY_NULL);

//		printf("BSVMP_TryVarsList: Name='%s'\n", b);

		n=dysymbol(b);

		BSVMP_Token(s, b2, &ty);

#if 1
		if(!strcmp(b2, ":"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_ExpressionType(&s);
			n=dylist3s("cast", n1, n);
			BSVMP_Token(s, b2, &ty);
		}else if(type)
		{
			n=dylist3s("cast", type, n);
		}
#endif

		if(!strcmp(b2, "="))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_Expression(&s);
			n=dylist3s("set!", n, n1);
			BSVMP_Token(s, b2, &ty2);
		}

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		s=BSVMP_Token(s, b, &ty);
//		printf("BSVMP_TryVarsList: Eat='%s'\n", b);

		if(!strcmp(b, ";"))
			break;

		if(strcmp(b, ","))
		{
//			printf("BSVMP_TryVarsList: Bad Token B '%s'\n", b);
			return(BGBDY_NULL);
		}
	}

	*str=s;
	return(lst);
}

dyt BSVMP_TryVarsList(char **str)
{
	return(BSVMP_TryVarsList2(str, NULL));
}

dyt BSVMP_DefType(char **str)
{
	char b[256];
	char *s;
	int i, ty, fl;

	s=*str;

	BSVMP_Token(s, b, &ty);
	fl=0;
	while(s && *s)
	{
		i=0;
		if(!strcmp(b, "extern"))i=BSVM_TYPEFL_EXTERN;
		if(!strcmp(b, "const"))i=BSVM_TYPEFL_CONST;
		if(!strcmp(b, "final"))i=BSVM_TYPEFL_CONST;
		if(!strcmp(b, "static"))i=BSVM_TYPEFL_STATIC;
		if(!strcmp(b, "native"))i=BSVM_TYPEFL_NATIVE;
		if(!strcmp(b, "public"))i=BSVM_TYPEFL_PUBLIC;
		if(!strcmp(b, "private"))i=BSVM_TYPEFL_PRIVATE;
		if(!strcmp(b, "protected"))i=BSVM_TYPEFL_PROTECTED;
		if(!strcmp(b, "virtual"))i=BSVM_TYPEFL_VIRTUAL;
		if(!strcmp(b, "volatile"))i=BSVM_TYPEFL_VOLATILE;

		if(i)
		{
			fl|=i;
			s=BSVMP_Token(s, b, &ty);
			BSVMP_Token(s, b, &ty);
			continue;
		}

		break;
	}

	if(!strcmp(b, "byte") || !strcmp(b, "sbyte") ||
		!strcmp(b, "short") || !strcmp(b, "ushort") ||
		!strcmp(b, "int") || !strcmp(b, "uint") ||
		!strcmp(b, "long") || !strcmp(b, "ulong") ||
		!strcmp(b, "float") || !strcmp(b, "double") ||
		!strcmp(b, "string") || !strcmp(b, "var") ||
		!strcmp(b, "list") || !strcmp(b, "symbol"))
	{
		s=BSVMP_Token(s, b, &ty);

		if(fl)sprintf(b, "%s$%d", b, fl);

		*str=s;
		return(dykeyword(b));
	}

	return(BGBDY_NULL);
}

dyt BSVMP_FunVarsList(char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int dots, byref, ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(1)
	{
		BSVMP_Token(s, b, &ty);
		if(!strcmp(b, ")"))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

#if 0
		if(!strcmp(b, ";"))
		{
			BSVMP_Token(s, b, &ty);
			n=BSVMP_Expression3(&s);
			lst=dynappend(lst, n);
			continue;
		}
#endif

//		n1=BSVMP_DefType(&s);

		s1=s;
		n1=BSVMP_ExpressionType(&s1);
		if(n1)
		{
			BSVMP_Token(s1, b, &ty);
			if(ty==BSVM_TOKEN_NAME)
					{ s=s1; }
			else	{ n1=NULL; }
		}

		s=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s, b2, &ty2);

#if 1
		dots=0;
		if(!strcmp(b, ";") || !strcmp(b, "..."))
		{
			s=BSVMP_Token(s, b, &ty);
			BSVMP_Token(s, b2, &ty2);
			dots=1;
		}

		byref=0;
		if(!strcmp(b, "&"))
		{
			s=BSVMP_Token(s, b, &ty);
			BSVMP_Token(s, b2, &ty2);
			byref=1;
		}
#endif

		if(ty!=BSVM_TOKEN_NAME)
		{
			BSVM_MSG_Error(s, "BSVMP_FunVarsList: Expected name, "
				"got '%s'\n", b);
			*str=NULL;
			return(BGBDY_NULL);
		}

		n=dysymbol(b);
//		if(n1!=BGBDY_NULL)n=dycons(n1, dycons(n, BGBDY_NULL));
		if(n1!=BGBDY_NULL)n=dylist3s("cast", n1, n);

#if 1
		if(!strcmp(b2, ":"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_ExpressionType(&s);
			n=dylist3s("cast", n1, n);
			BSVMP_Token(s, b2, &ty);
		}

		if(!strcmp(b2, "="))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_Expression(&s);
//			n=dycons(n, dycons(n1, BGBDY_NULL));
			n=dylist3s("set!", n1, n);
			BSVMP_Token(s, b2, &ty2);
		}
#endif

		if(dots)
			n=dylist2s("rest", n);

		if(byref)
			n=dylist2s("byref", n);

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		if(!strcmp(b2, ","))
			s=BSVMP_Token(s, b2, &ty2);
	}

	*str=s;
	return(lst);
}

dyt BSVMP_FunVarsListB(char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int ty, ty2;
	dyt n, n1, n2, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(1)
	{
		BSVMP_Token(s, b, &ty);
		if(!strcmp(b, ")"))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if(!strcmp(b, ";"))
		{
			n=BSVMP_Expression3(&s);
			lst=dynappend(lst, n);
			continue;
		}

		n1=BSVMP_ExpressionType(&s);
		if(!n1)
		{
			BSVMP_Token(s, b, &ty);
			BSVM_MSG_Error(s, "BSVMP_FunVarsListB: Unexpected Token "
				" '%s'\n", b);
			*str=NULL;
			return(BGBDY_NULL);
		}

		s1=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s1, b2, &ty2);
		if(ty==BSVM_TOKEN_NAME)
		{
			s=BSVMP_Token(s, b, &ty);
			BSVMP_Token(s, b2, &ty2);
			n2=dysymbol(b);
			n=dylist3s("cast", n1, n2);
		}else
		{
			n2=dysymbol("_");	//special placeholder
			n=dylist3s("cast", n1, n2);
		}


#if 1
#if 0
		if(!strcmp(b2, ":"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_ExpressionType(&s);
			n=dylist3s("cast", n1, n);
			BSVMP_Token(s, b2, &ty);
		}
#endif

		if(!strcmp(b2, "="))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_Expression(&s);
//			n=dycons(n, dycons(n1, BGBDY_NULL));
			n=dylist3s("set!", n1, n);
			BSVMP_Token(s, b2, &ty2);
		}
#endif

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		if(!strcmp(b2, ","))
			s=BSVMP_Token(s, b2, &ty2);
	}

	*str=s;
	return(lst);
}

dyt BSVMP_FunVarsListC(char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int ty, ty2;
	dyt n, n1, n2, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(1)
	{
		BSVMP_Token(s, b, &ty);
		if(!strcmp(b, "]"))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

#if 0
		n1=BSVMP_ExpressionType(&s);
		if(!n1)
		{
			BSVMP_Token(s, b, &ty);
			BSVM_MSG_Error(s, "BSVMP_FunVarsListB: Unexpected Token "
				" '%s'\n", b);
			*str=NULL;
			return(BGBDY_NULL);
		}
#endif

#if 0
		s1=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s1, b2, &ty2);
		if(ty==BSVM_TOKEN_NAME)
		{
			s=BSVMP_Token(s, b, &ty);
			BSVMP_Token(s, b2, &ty2);
			n2=dysymbol(b);
			n=dylist3s("cast", n1, n2);
		}else
		{
			n2=dysymbol("_");	//special placeholder
			n=dylist3s("cast", n1, n2);
		}
#endif

		if(!strcmp(b, "="))
		{
			s=BSVMP_Token(s, b, &ty);

			BSVMP_Token(s, b, &ty);
			if(ty==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b, &ty);
				n=dylist2s("byvalue", dysymbol(b));
			}
		}else if(!strcmp(b, "&"))
		{
			s=BSVMP_Token(s, b, &ty);
			BSVMP_Token(s, b, &ty);
			if(ty==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b, &ty);
				n=dylist2s("byref", dysymbol(b));
			}
		}else
		{
			if(ty==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b, &ty);
				n=dysymbol(b);
			}
		}

#if 0
#if 0
		if(!strcmp(b2, ":"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_ExpressionType(&s);
			n=dylist3s("cast", n1, n);
			BSVMP_Token(s, b2, &ty);
		}
#endif

		if(!strcmp(b2, "="))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n1=BSVMP_Expression(&s);
//			n=dycons(n, dycons(n1, BGBDY_NULL));
			n=dylist3s("set!", n1, n);
			BSVMP_Token(s, b2, &ty2);
		}
#endif

		lst=dynappend(lst, dycons(n, BGBDY_NULL));

		BSVMP_Token(s, b2, &ty2);
		if(!strcmp(b2, ","))
			s=BSVMP_Token(s, b2, &ty2);
	}

	*str=s;
	return(lst);
}

dyt BSVMP_Class(char **str)
{
	char b[256], b2[256];
	char *s, *s2;
	int ty, ty2;
	dyt n, n1, n2, n3, n4;
	dyt cty, ctn, cts, cti, ctb;

	s=*str;
	s=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s, b2, &ty2);
	cty=dysymbol(b);	//class, interface, struct, ...

	s=BSVMP_TokenQName(s, b2, &ty2);
//	s=BSVMP_Token(s, b2, &ty2);
	ctn=dysymbol(b2);	//classname

	cts=NULL;
	cti=NULL;
	ctb=NULL;

	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "extends"))
	{
//		s=BSVMP_Token(s, b, &ty);
		s=BSVMP_TokenQName(s, b2, &ty2);
		s=BSVMP_Token(s, b2, &ty2);
		cts=dysymbol(b2);	//superclass
	}

	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "implements"))
	{
		s=BSVMP_Token(s, b, &ty);
		while(1)
		{
			s=BSVMP_TokenQName(s, b2, &ty2);
//			s=BSVMP_Token(s, b2, &ty2);
			if(!s && !*s)break;

			n1=dysymbol(b2);
			cti=dynappend(cti, dylist1(n1));

			BSVMP_Token(s, b, &ty);
			if(!strcmp(b, ","))
			{
				s=BSVMP_Token(s, b, &ty);
				continue;
			}
			break;
		}
	}
		
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "{"))
	{
		s=BSVMP_Token(s, b, &ty);
		ctb=BSVMP_Block(&s);
	}

	n=dylist5(cty, ctn, cts, cti, ctb);

	*str=s;
	return(n);
}

