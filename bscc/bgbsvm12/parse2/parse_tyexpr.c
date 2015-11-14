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

char *bsvmp_keywords_modifier[]={
"abstract",
"async",

"const",

"delegate",
"dynamic",

"extern",

"final",

"internal",

"native",

"private",
"protected",
"public",

"static",
"synchronized",

"transient",
"typedef",

"volatile",

"_setuid",
"_setgid",

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

int BSVMP_CheckModifierName(char *str)
{
	if(BSVMP_CheckTokenList(str, bsvmp_keywords_modifier)>=0)
		return(1);
	return(0);
}

dytf BSVMP_TypeForSuffix(char *str)
{
	char *tyn;
	dytf n;
	
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
		n=dytfSymbol(tyn);
		return(n);
	}
	
	return(DYTF_NULL);
}

dytf BSVMP_TypeArgs(char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dytf n, n1, lst;

	s=*str;
	lst=DYTF_NULL;

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
		lst=dytfNAppend(lst, dytfList1(n));

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

dytf BSVMP_TypeExpressionLit(char **str)
{
	char b[256], b2[256];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2;
	int i;

	s=*str;

	BSVMP_Token(s, b, &ty);

	if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ";"))
		return(DYTF_NULL);
	if((ty==BSVM_TOKEN_SEPERATOR) && !strcmp(b, ","))
		return(DYTF_NULL);

	s=BSVMP_Token(s, b, &ty);
	if(!s)return(DYTF_NULL);

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
				n=dytfList4s("rlambda", dytfSymbol(b), n1, n2);
				*str=s;
				return(n);
			}

			n1=BSVMP_FunVarsList(&s);
			n2=BSVMP_BlockStatement2(&s);
			n=dytfList3s("lambda", n1, n2);
			*str=s;
			return(n);
		}
#endif

		if(!BSVMP_CheckValidTypeName(b))
			return(DYTF_NULL);

		n=dytfSymbol(b);
		*str=s;
		return(n);
	}

	return(DYTF_NULL);
}

dytf BSVMP_TypeExpressionPE(char **str)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;
	int i;

	s=*str;
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
	{
//		s=BSVMP_Token(s, b, &ty);	//(
//		n=BSVMP_TypeExpression(&s);
//		s=BSVMP_Token(s, b, &ty);
//		n=dytfList2s("begin", n);

//		n=BSVMP_FunArgs(&s);
		n=BSVMP_TypeArgs(&s);
		n=dytfCons(dytfSymbol("begin"), n);

		*str=s;
		return(n);
	}

	n=BSVMP_TypeExpressionLit(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

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
				n=dytfList2s("emptyindex", n);
				continue;
			}
			
			n1=BSVMP_Expression(&s);
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

#if 1
		if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
		{
			s=BSVMP_Token(s, b, &ty); //'('
			n1=BSVMP_FunArgs(&s);
			if(!s)return(DYTF_NULL);
			
			n=dytfCons2s("funcall", n, n1);
			continue;
		}

		if(!strcmp(b, "<") && (ty==BSVM_TOKEN_OPERATOR))
		{
			s=BSVMP_Token(s, b, &ty); //'<'
			n1=BSVMP_TypeArgs(&s);
			if(!s)return(DYTF_NULL);

			n=dytfCons2s("sharpcall", n, n1);
			continue;
		}

#if 0
		if((!strcmp(b, "^") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BSVM_TOKEN_OPERATOR))
		{
			s=BSVMP_Token(s, b, &ty);
			n=dytfList3s("unary", dytfSymbol(b), n);
			continue;
		}
#endif
#endif

		break;
	}

	*str=s;
	return(n);
}

dytf BSVMP_TypeExpressionUnary(char **str)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;
	BSVMP_Token(s, b, &ty);

	if((!strcmp(b, "^") || !strcmp(b, "&") || !strcmp(b, "*")) &&
			(ty==BSVM_TOKEN_OPERATOR))
	{
		s=BSVMP_Token(s, b, &ty);
		n1=BSVMP_TypeExpressionUnary(&s);
		n=dytfList3s("unary", dytfSymbol(b), n1);
		*str=s;
		return(n);
	}

	s=*str;
	n=BSVMP_TypeExpressionPE(&s);
	if(dytfNullP(n)) return(DYTF_NULL);

	*str=s;
	return(n);
}

dytf BSVMP_TypeExpression(char **str)
{
	return(BSVMP_TypeExpressionUnary(str));
}

dytf BSVMP_ColonTypeExpression(char **str)
{
	char b[64], b2[64];
	char *s, *t;
	dytf n;
	int ty, ty2;

	s=*str;
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, ":"))
	{
		s=BSVMP_Token(s, b, &ty);	//':'
		n=BSVMP_TypeExpressionUnary(&s);
		*str=s;
		return(n);
	}

	return(DYTF_NULL);
}

dytf BSVMP_CastTypeExpression(char **str)
{
	char b[64], b2[64];
	char *s, *t;
	int ty, ty2;
	dytf n, n1, n2, n3;
	int i;

	s=*str;
	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "(") && (ty==BSVM_TOKEN_BRACE))
	{
		s=BSVMP_Token(s, b, &ty);	//(
		n=BSVMP_TypeExpression(&s);
		if(dytfNullP(n))return(DYTF_NULL);
		s=BSVMP_Token(s, b, &ty);	//)
		if(strcmp(b, ")") || (ty!=BSVM_TOKEN_BRACE))
			return(DYTF_NULL);

		n=dytfList2s("casttype", n);

		*str=s;
		return(n);
	}

	return(DYTF_NULL);
}

BGBSVM_API dytf BSVMP_TryParseDeclaration(char **str)
{
	char b[256], b2[256];
	char *s, *s2;
	int ty, ty2;
	dytf n, n1, n2, n3, n4;

	s=*str;
	
	s2=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s2, b2, &ty2);
//	printf("BSVMP_TryParseDeclaration 0 '%s' '%s'\n", b, b2);

	n1=BSVMP_ExpressionType(&s);
	if(dytfNullP(n1))return(DYTF_NULL);

	s2=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s2, b2, &ty2);
//	printf("BSVMP_TryParseDeclaration 1 '%s' '%s'\n", b, b2);
	
	n2=BSVMP_TryVarsList2(&s, n1);
	if(!dytfNullP(n2))
	{
//		printf("BSVMP_TryParseDeclaration 2\n");
		n=dytfList2s("vars", n2);
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
		n2=dytfCons(dytfList3s("cast", n1, dytfSymbol(b)), n2);
			
		n3=BSVMP_BlockStatement2(&s);
		n=dytfList3s("define", n2, n3);
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
			n1=dytfList3s("cast", n1, dycadr(n2));
			n2=dytfCons(n1, dycddr(n2));
			n=dytfList2s("proto", n2);

			*str=s;
			return(n);
		}

		if(BSVMC_FormIs(n2, "define"))
		{
			n1=dytfList3s("cast", n1, dycaadr(n2));
			n2=dytfCons(n1, dycdadr(n2));
			n=dytfList2s("define", n2);

			*str=s;
			return(n);
		}
	}
#endif

#if 0
	s=*str;

	n=BSVMP_DefType(&s);
	if(n!=DYTF_NULL)
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
			n=dytfList5s("defun", n, dytfSymbol(b), n1, n2);
			*str=s;
			return(n);
		}

		n1=BSVMP_VarsList(&s);
		n=dytfList3s("vars", n, n1);
		*str=s;
		return(n);
	}
#endif

	return(DYTF_NULL);
}
