#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgbsvm.h>

char *bsvmp_keywords_all[]={
"abstract",
"as",
"async",

"bool",
"boolean",
"break",
"byte",

"call",
"case",
"catch",
"char",
"char16",
"char8",

"class",
"cons",
"const",
"continue",
"cstring",

"default",
"delegate",
"delete",
"do",
"double",
"dynamic",

"else",
"extends",
"extern",

"false",
"final",
"finally",
"fixnum",
"float",
"float128",
"flonum",
"for",
"fun",
"function",

"get",
"goto",

"if",
"ifdef",
"ifndef",
"implements",
"import",
"in",
"instanceof",
"int",
"int128",
"interface",
"internal",
"is",

"long",

"native",
"new",
"null",
"number",

"object",
"offsetof",
"operator",

"package",
"private",
"protected",
"public",

"quat",
"quatd",
"quote",

"return",

"sbyte",
"set",
"short",
"sizeof",
"static",
"string",
"struct",
"super",
"switch",
"symbol",
"synchronized",

"this",
"throw",
"transient",
"true",
"try",
"typeof",

"ubyte",
"uint",
"uint128",
"ulong",
"undefined",
"unquote",
"ushort",

"value_class",
"var",
"variant",
"vec2",
"vec2d",
"vec3",
"vec3d",
"vec4",
"vec4d",
"void",
"volatile",

"while",
"with",
"withtop",

"xml",

NULL};

char *bsvmp_keywords_type[]={
"bool",
"boolean",
"byte",

"char",
"char16",
"char8",
"cons",
"cstring",

"double",

"fixnum",
"float",
"float128",
"flonum",

"int",
"int128",

"long",

"number",

"object",

"quat",
"quatd",

"sbyte",
"short",
"string",
"symbol",

"ubyte",
"uint",
"uint128",
"ulong",
"ushort",

"var",
"variant",
"vec2",
"vec2d",
"vec3",
"vec3d",
"vec4",
"vec4d",
"void",

NULL};


char *bsvmp_keywords_ident[]={
"call",

"else",

"false",

"get",

"null",

"set",
"super",

"this",
"true",

"undefined",

NULL};


int BSVMP_CheckTokenList(char *str, char **lst)
{
	int i;
	for(i=0; lst[i]; i++)
		if(!strcmp(lst[i], str))
			return(i);
	return(-1);
}

int BSVMP_CheckValidTypeName(char *str)
{
	if(BSVMP_CheckTokenList(str, bsvmp_keywords_type)>=0)
		return(1);

	if(BSVMP_CheckTokenList(str, bsvmp_keywords_all)>=0)
		return(0);
		
	return(1);
}

int BSVMP_CheckValidDeclName(char *str)
{
	if(BSVMP_CheckTokenList(str, bsvmp_keywords_all)>=0)
		return(0);
		
	return(1);
}

int BSVMP_CheckValidIdentifierName(char *str)
{
	if(BSVMP_CheckTokenList(str, bsvmp_keywords_ident)>=0)
		return(1);
	if(BSVMP_CheckTokenList(str, bsvmp_keywords_type)>=0)
		return(1);

	if(BSVMP_CheckTokenList(str, bsvmp_keywords_all)>=0)
		return(0);
		
	return(1);
}

dyt BSVMP_TypeForSuffix(char *str)
{
	char *tyn;
	dyt n;
	
	tyn=NULL;
	if(!stricmp(str, "SB"))tyn="sbyte";
	if(!stricmp(str, "B"))tyn="bool";
	if(!stricmp(str, "SC"))tyn="char8";
	if(!stricmp(str, "D"))tyn="double";
	if(!stricmp(str, "LD"))tyn="longdouble";
	if(!stricmp(str, "F"))tyn="float";
	if(!stricmp(str, "G"))tyn="float128";
	if(!stricmp(str, "UB"))tyn="byte";
	if(!stricmp(str, "SI"))tyn="int";
	if(!stricmp(str, "UI"))tyn="uint";
	if(!stricmp(str, "SF"))tyn="float16";
	if(!stricmp(str, "L"))tyn="long";
	if(!stricmp(str, "UL"))tyn="ulong";
	if(!stricmp(str, "LX"))tyn="int128";
	if(!stricmp(str, "ULX"))tyn="uint128";
	if(!stricmp(str, "V"))tyn="var";
	if(!stricmp(str, "SS"))tyn="short";
	if(!stricmp(str, "US"))tyn="ushort";
	if(!stricmp(str, "W"))tyn="char";
	
	if(tyn)
	{
		n=dysymbol(tyn);
		return(n);
	}
	
	return(NULL);
}

dyt BSVMP_TypeArgs(char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dyt n, n1, lst;

	s=*str;
	lst=BGBDY_NULL;

	while(1)
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

		if((ty==BSVM_TOKEN_OPERATOR) && !strcmp(b, ">"))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

//		n=BSVMP_TypeExpression(&s);
		n=BSVMP_ExpressionAS(&s);
		lst=dynappend(lst, dylist1(n));

		BSVMP_Token(s, b, &ty);
		if(!strcmp(b, ","))
		{
			s=BSVMP_Token(s, b, &ty);
			continue;
		}
	}

	*str=s;
	return(lst);
}

dyt BSVMP_TypeExpressionLit(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2;
	int i;

	s=*str;

	BSVMP_Token(s, b, &ty);

	if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ";"))
		return(NULL);
	if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ","))
		return(NULL);

	s=BSVMP_Token(s, b, &ty);
	if(!s)return(BGBDY_NULL);

	if(ty==BSVM_TOKEN_NAME)
	{
#if 0
		n=BSVMP_ParseExpressionName(b, &s);
		if(n)
		{
			*str=s;
			return(n);
		}
#endif

#if 0
		if(!strcmp(b, "lambda") || !strcmp(b, "fun") ||
			!strcmp(b, "function"))
		{
			s=BSVMP_Token(s, b, &ty); //'('

			if(ty==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b2, &ty2); //'('

				n1=BSVMP_FunVarsList(&s);
				n2=BSVMP_BlockStatement2(&s);
				n=dylist4s("rlambda", dysymbol(b), n1, n2);
				*str=s;
				return(n);
			}

			n1=BSVMP_FunVarsList(&s);
			n2=BSVMP_BlockStatement2(&s);
			n=dylist3s("lambda", n1, n2);
			*str=s;
			return(n);
		}
#endif

		if(!BSVMP_CheckValidTypeName(b))
			return(NULL);

		n=dysymbol(b);
		*str=s;
		return(n);
	}

	return(BGBDY_NULL);
}

dyt BSVMP_TypeExpressionPE(char **str)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;
	int i;

	s=*str;
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
	{
//		s=BSVMP_Token(s, b, &ty);	//(
//		n=BSVMP_TypeExpression(&s);
//		s=BSVMP_Token(s, b, &ty);
//		n=dylist2s("begin", n);

//		n=BSVMP_FunArgs(&s);
		n=BSVMP_TypeArgs(&s);
		n=dycons(dysymbol("begin"), n);

		*str=s;
		return(n);
	}

	n=BSVMP_TypeExpressionLit(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	while(1)
	{
		if(!BSVMP_IsWhiteOnly(s))break;
		BSVMP_Token(s, b, &ty);

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

#if 1
		if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
		{
			s=BSVMP_Token(s, b, &ty); //'('
			n1=BSVMP_FunArgs(&s);
			if(!s)return(BGBDY_NULL);
			
			n=dycons2s("funcall", n, n1);
			continue;
		}

		if(!strcmp(b, "<") && (ty==BSVM_TOKEN_OPERATOR))
		{
			s=BSVMP_Token(s, b, &ty); //'<'
			n1=BSVMP_TypeArgs(&s);
			if(!s)return(BGBDY_NULL);

			n=dycons2s("sharpcall", n, n1);
			continue;
		}

#if 0
		if((!strcmp(b, "^") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BSVM_TOKEN_OPERATOR))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dylist3s("unary", dysymbol(b), n);
			continue;
		}
#endif
#endif

		break;
	}

	*str=s;
	return(n);
}

dyt BSVMP_TypeExpressionUnary(char **str)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;

	s=*str;
	BSVMP_Token(s, b, &ty);

	if((!strcmp(b, "^") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_TypeExpressionUnary(&s);
		n=dylist3s("unary", dysymbol(b), n1);
		*str=s;
		return(n);
	}

	s=*str;
	n=BSVMP_TypeExpressionPE(&s);
	if(n==BGBDY_NULL) return(BGBDY_NULL);

	*str=s;
	return(n);
}

dyt BSVMP_TypeExpression(char **str)
{
	return(BSVMP_TypeExpressionUnary(str));
}

dyt BSVMP_CastTypeExpression(char **str)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2;
	dyt n, n1, n2, n3;
	int i;

	s=*str;
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
	{
		s=BSVMP_Token(s, b, &ty);	//(
		n=BSVMP_TypeExpression(&s);
		if(!n)return(NULL);
		s=BSVMP_Token(s, b, &ty);	//)
		if(strcmp(b, ")") || (ty!=BSVM_TOKEN_BRACE))
			return(NULL);

		n=dylist2s("casttype", n);

		*str=s;
		return(n);
	}

	return(NULL);
}

BGBSVM_API dyt BSVMP_TryParseDeclaration(char **str)
{
	char b[256], b2[256];
	char *s, *s2;
	int ty, ty2;
	dyt n, n1, n2, n3, n4;

	s=*str;
	
	s2=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s2, b2, &ty2);
//	printf("BSVMP_TryParseDeclaration 0 '%s' '%s'\n", b, b2);

	n1=BSVMP_ExpressionType(&s);
	if(!n1)return(NULL);

	s2=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s2, b2, &ty2);
//	printf("BSVMP_TryParseDeclaration 1 '%s' '%s'\n", b, b2);
	
	n2=BSVMP_TryVarsList2(&s, n1);
	if(n2)
	{
//		printf("BSVMP_TryParseDeclaration 2\n");
		n=dylist2s("vars", n2);
		*str=s;
		return(n);
	}

//	printf("BSVMP_TryParseDeclaration 3\n");

#if 1
	s2=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s2, b2, &ty2);

	if((ty==BSVM_TOKEN_NAME) && (ty2==BSVM_TOKEN_NAME) &&
		(!strcmp(b, "get") || !strcmp(b, "set")))
	{
		s2=BSVMP_Token(s2, b2, &ty2);
		BSVMP_Token(s2, b2, &ty2);
	}

	//typed function definition
	if((ty==BSVM_TOKEN_NAME) && !strcmp(b2, "("))
	{
//		printf("BSVMP_TryParseDeclaration 4\n");

		s=BSVMP_Token(s, b, &ty);	//name
		s=BSVMP_Token(s, b2, &ty2);	//(

#if 0
		if((ty==BSVM_TOKEN_NAME) && (ty2==BSVM_TOKEN_NAME) &&
			(!strcmp(b, "get") || !strcmp(b, "set")))
		{
			strcat(b, "_");
			strcat(b, b2);
			s=BSVMP_Token(s, b2, &ty2);
		}
#endif

#if 1
		if(!strcmp(b, "get") || !strcmp(b, "set"))
		{
//			BSVMP_Token(s, b2, &ty2);
			if(!strcmp(b2, "_") || !strcmp(b2, "*"))
			{
//				s=BSVMP_Token(s, b2, &ty2);
				strcat(b, "_any_");
			}else if(ty2==BSVM_TOKEN_NAME)
			{
//				s=BSVMP_Token(s, b2, &ty2);
				strcat(b, "_"); strcat(b, b2);
			}

			s=BSVMP_Token(s, b2, &ty2);
		}
#endif

		n2=BSVMP_FunVarsList(&s);
//		n2=BSVMP_FunVarsListB(&s);
		n2=dycons(dylist3s("cast", n1, dysymbol(b)), n2);
			
		n3=BSVMP_BlockStatement2(&s);
		n=dylist3s("define", n2, n3);
		*str=s;
		return(n);
	}

//	printf("BSVMP_TryParseDeclaration 5\n");
#endif

#if 0
	n2=BSVMP_ExpressionFunc(&s);
	if(n2)
	{
		if(BSVMC_FormIs(n2, "funcall"))
		{
			n1=dylist3s("cast", n1, dycadr(n2));
			n2=dycons(n1, dycddr(n2));
			n=dylist2s("proto", n2);

			*str=s;
			return(n);
		}

		if(BSVMC_FormIs(n2, "define"))
		{
			n1=dylist3s("cast", n1, dycaadr(n2));
			n2=dycons(n1, dycdadr(n2));
			n=dylist2s("define", n2);

			*str=s;
			return(n);
		}
	}
#endif

#if 0
	s=*str;

	n=BSVMP_DefType(&s);
	if(n!=BGBDY_NULL)
	{
		s2=BSVMP_Token(s, b, &ty);
		BSVMP_Token(s2, b2, &ty2);

		//typed function definition
		if((ty==BSVM_TOKEN_NAME) && !strcmp(b2, "("))
		{
			s=BSVMP_Token(s, b, &ty);	//name
			s=BSVMP_Token(s, b2, &ty2);	//(

			bsvmp_curfilename=NULL;
			n1=BSVMP_FunVarsList(&s);
			n2=BSVMP_BlockStatement2(&s);
			n=dylist5s("defun", n, dysymbol(b), n1, n2);
			*str=s;
			return(n);
		}

		n1=BSVMP_VarsList(&s);
		n=dylist3s("vars", n, n1);
		*str=s;
		return(n);
	}
#endif

	return(NULL);
}
