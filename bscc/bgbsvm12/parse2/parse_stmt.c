#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgbsvm.h>

BSVM_ParseItem *bsvmp_stmts=NULL;
BSVM_ParseItem *bsvmp_blkstmts=NULL;

char *bsvmp_curfilename=NULL;
int bsvmp_curlinenum=-1;

BGBSVM_API int BSVMP_AddStatement(char *name,
	dytf (*func)(BSVM_ParseContext *ctx, char **s))
{
	BSVM_ParseItem *tmp;

	tmp=gcalloc(sizeof(BSVM_ParseItem));
	tmp->name=strdup(name);
	tmp->func=func;

	tmp->next=bsvmp_stmts;
	bsvmp_stmts=tmp;

	return(0);
}

dytf BSVMP_ParseStatementName(char *name, char **s)
{
	BSVM_ParseItem *cur;

	cur=bsvmp_stmts;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur->func(NULL, s));
		cur=cur->next;
	}
	return(DYTF_NULL);
}

int BSVMP_AddBlockStatement(char *name,
	dytf (*func)(BSVM_ParseContext *ctx, char **s))
{
	BSVM_ParseItem *tmp;

	tmp=gcalloc(sizeof(BSVM_ParseItem));
	tmp->name=strdup(name);
	tmp->func=func;

	tmp->next=bsvmp_blkstmts;
	bsvmp_blkstmts=tmp;

	return(0);
}

dytf BSVMP_ParseBlockStatementName(char *name, char **s)
{
	BSVM_ParseItem *cur;

	cur=bsvmp_blkstmts;
	while(cur)
	{
		if(!strcmp(name, cur->name))
			return(cur->func(NULL, s));
		cur=cur->next;
	}
	return(DYTF_NULL);
}

char *BSVMP_EatSemicolon(char *s)
{
	if(!s || !*s)return(s);

	s=BSVMP_EatWhite(s);
	if(*s==';')s++;
	s=BSVMP_EatWhite(s);
	return(s);
}

dytf BSVMP_Statement(char **str)
{
	char b[256], b2[256];
	char *s;
	int ty, ty2;
	dytf n, n1, n2;

	s=*str;
	s=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s, b2, &ty2);
	if(!*s)
	{
		*str=s;
		return(DYTF_NULL);
	}

	if(ty!=BSVM_TOKEN_NAME)
	{
		s=*str;
		n=BSVMP_Expression(&s);
		*str=s;
		return(n);
	}

	n=BSVMP_ParseStatementName(b, &s);
	if(!dytfNullP(n))
	{
		*str=s;
		return(n);
	}

	if(!strcmp(b, "break"))
	{
		BSVMP_Token(s, b2, &ty2);
		if(ty2==BSVM_TOKEN_NUMBER)
		{
			s=BSVMP_Token(s, b2, &ty2);
			n=dytfList2s("break", dytfInt(atoi(b2)));
		}else n=dytfList1s("break");

		*str=s;
		return(n);
	}

	if(!strcmp(b, "continue"))
	{
		BSVMP_Token(s, b2, &ty2);
		if(ty2==BSVM_TOKEN_NUMBER)
		{
			s=BSVMP_Token(s, b2, &ty2);
			n=dytfList2s("continue", dytfInt(atoi(b2)));
		}else n=dytfList1s("continue");

		*str=s;
		return(n);
	}

	if(!strcmp(b, "goto"))
	{
//		s=BSVMP_Token(s, b2, &ty2);
//		n=dytfList2s("goto", dystring(b2));
		n1=BSVMP_Expression(&s);
		n=dytfList2s("goto", n1);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "throw"))
	{
		//s=BSVMP_Token(s, b2, &ty2);
		//n=dytfList2s("throw", dystring(b2));

		n1=BSVMP_Expression(&s);
		n=dytfList2s("throw", n1);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "return"))
	{
		BSVMP_Token(s, b2, &ty2);
		if(b2[0]!=';')
			n1=BSVMP_Expression(&s);
			else n1=DYTF_NULL;

		n=dytfList2s("return", n1);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "import"))
	{
		s=BSVMP_TokenQName(s, b2, &ty2);
		n1=dytfString(b2);
		n=dytfList2s("import", n1);
		*str=s;
		return(n);
	}


	s=*str;
	s=BSVMP_EatWhite(s);
	if(!s || !*s)return(DYTF_NULL);

	n=BSVMP_Expression(&s);
	if(!dytfNullP(n))
	{
//		if(dytfSymbolp(n))
		if(BSVMC_FlattenQNameBaseSig(dytfUnwrap(n)))
		{
			//name name, declaration special case
			BSVMP_Token(s, b2, &ty2);
			if(ty2==BSVM_TOKEN_NAME)
				return(DYTF_NULL);
		}

		*str=s;
		return(n);
	}

	BSVM_MSG_Error(s, "PDSCR_Parse_Statement: Unrecognizable statement type\n");
//	*str=NULL;
	return(DYTF_NULL);
}

dytf BSVMP_BlockInlineC(char **str)
{
	char tb3[16384];
	char b[256], b2[256];
	char *s, *t, *s1, *s2, *s3;
	int ty, ty2;
	dytf n, n1, n2, n3, n4;
	int i, j, k;

	s=*str;
	s1=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s1, b2, &ty2);

	if(!strcmp(b, "{"))
	{
		t=tb3; i=1;
		while(s1 && *s1)
		{
			s2=BSVMP_Token(s1, b2, &ty2);
			if(!strcmp(b2, "{"))i++;
			if(!strcmp(b2, "}"))
				{ i--; if(!i) { s1=s2; break; } }
			while(s1<s2) { *t++=*s1++; }
		}
		*t++=0;
		s=s1;

		n=dytfList2s("inline_c", dytfString(tb3));

		*str=s;
		return(n);
	}

	s1=s; t=tb3; i=0;
	while(s1 && *s1)
	{
		s2=BSVMP_Token(s1, b2, &ty2);
		if(!strcmp(b2, "{"))i++;
		if(!strcmp(b2, "}"))
			{ i--; if(i<0)break; }
		if(!strcmp(b2, "("))i++;
		if(!strcmp(b2, ")"))
			{ i--; if(i<0)break; }
		if(!strcmp(b2, "["))i++;
		if(!strcmp(b2, "]"))
			{ i--; if(i<0)break; }

		if(!i)
		{
			if(!strcmp(b2, ";"))
				{ s1=s2; break; }
				
			s3=s1;
			while(*s3 && (s3<s2) && (*s3<=' ') &&
				(*s3!='\r') && (*s3!='\n'))
					s3++;
			if((s3<s2) && ((*s3=='\r') || (*s3=='\n')))
			{
				s1=s3;
				if(*s1=='\r')s1++;
				if(*s1=='\n')s1++;
				break;
			}
		}

		while(s1<s2)
			{ *t++=*s1++; }
	}
	*t++=0;
	s=s1;

	n=dytfList2s("inline_c", dytfString(tb3));

	*str=s;
	return(n);

//	n1=BSVMP_BlockStatement2(&s);
//	n=dytfList2s("inline_c", n1);
}

dytf BSVMP_BlockStatementInner(char **str)
{
	char b[256], b2[256];
	char *s, *s2;
	int ty, ty2;
	dytf n, n1, n2, n3, n4;

	s=*str;
	s=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s, b2, &ty2);
	if(!s || !*s)
	{
		*str=s;
		return(DYTF_NULL);
	}

	if(ty==BSVM_TOKEN_SEPERATOR)
	{
		*str=s;
		return(DYTF_NULL);
	}

	if((ty==BSVM_TOKEN_NAME) && !strcmp(b2, ":"))
	{
		s=BSVMP_Token(s, b2, &ty2);	//:

		if(!strcmp(b, "default"))
		{
			n=dytfList1s("case_default");
			*str=s;
			return(n);
		}

		n=dytfList2s("label", dytfSymbol(b));

		*str=s;
		return(n);
	}

	if(!strcmp(b, "case"))
	{
		n1=BSVMP_Expression(&s);
		s=BSVMP_Token(s, b2, &ty2);	//:
		n=dytfList2s("case", n1);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "function"))
	{
		s=BSVMP_Token(s, b, &ty);
//		BSVMP_Token(s, b2, &ty2);

		if(!strcmp(b, "~"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			sprintf(b, "_%s_deinit_", b2);
			ty=BSVM_TOKEN_NAME;
		}

		if(ty!=BSVM_TOKEN_NAME)
		{
			BSVM_MSG_Error(s, "PDSCR_Parse_Function: Invalid name %s\n", b);
			*str=NULL;
			return(DYTF_NULL);
		}

		if(!strcmp(b, "get") || !strcmp(b, "set"))
		{
			BSVMP_Token(s, b2, &ty2);
			if(!strcmp(b2, "_") || !strcmp(b2, "*"))
			{
				s=BSVMP_Token(s, b2, &ty2);
				strcat(b, "_any_");
			}else if(ty2==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b2, &ty2);
				strcat(b, "_"); strcat(b, b2);
			}
		}

		if(!strcmp(b, "operator"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			//...
		}

		if(!strcmp(b, "call"))
		{
			BSVMP_Token(s, b2, &ty2);
			if(!strcmp(b2, "*"))
			{
				s=BSVMP_Token(s, b2, &ty2);
				strcpy(b, "doesNotUnderstand");
			}
		}

		n1=dytfSymbol(b);
		s=BSVMP_Token(s, b, &ty); //(
		n2=BSVMP_FunVarsList(&s);

		//overloaded operator
		if(!strcmp(b, "operator"))
		{
#if 1
			if(ty2==BSVM_TOKEN_OPERATOR)
			{
				if(!dytfNullP(dytfCdr(n2)))
				{
					s2="unknown";
					if(!strcmp(b2, "+"))s2="add";
					if(!strcmp(b2, "-"))s2="sub";
					if(!strcmp(b2, "*"))s2="mul";
					if(!strcmp(b2, "/"))s2="div";
					if(!strcmp(b2, "%"))s2="mod";
					if(!strcmp(b2, "&"))s2="and";
					if(!strcmp(b2, "|"))s2="or";
					if(!strcmp(b2, "^"))s2="xor";
					if(!strcmp(b2, "~"))s2="not";
					if(!strcmp(b2, "!"))s2="lnot";
					if(!strcmp(b2, "<<"))s2="shl";
					if(!strcmp(b2, ">>"))s2="shr";
					if(!strcmp(b2, ">>>"))s2="shrr";

					if(!strcmp(b2, "=="))s2="eqv";
					if(!strcmp(b2, "!="))s2="neqv";
					if(!strcmp(b2, "==="))s2="eq";
					if(!strcmp(b2, "!=="))s2="neq";

					if(!strcmp(b2, "<"))s2="lt";
					if(!strcmp(b2, ">"))s2="gt";
					if(!strcmp(b2, "<="))s2="le";
					if(!strcmp(b2, ">="))s2="ge";
				}else
				{
					s2="unknown";
					if(!strcmp(b2, "+"))s2="pos";
					if(!strcmp(b2, "-"))s2="neg";
					if(!strcmp(b2, "*"))s2="umul";
					if(!strcmp(b2, "/"))s2="udiv";
					if(!strcmp(b2, "%"))s2="umod";
					if(!strcmp(b2, "&"))s2="uand";
					if(!strcmp(b2, "|"))s2="uor";
					if(!strcmp(b2, "^"))s2="uxor";
					if(!strcmp(b2, "~"))s2="not";
					if(!strcmp(b2, "!"))s2="lnot";
					if(!strcmp(b2, "++"))s2="inc";
					if(!strcmp(b2, "--"))s2="dec";
				}

				strcat(b, "_");
				strcat(b, s2);
			}else if(ty2==BSVM_TOKEN_NAME)
			{
				strcat(b, "_"); strcat(b, b2);
			}
#endif

			n1=dytfSymbol(b);
		}

#if 1
		BSVMP_Token(s, b2, &ty);
		if(!strcmp(b2, ":"))
		{
			s=BSVMP_Token(s, b2, &ty2);
			n=BSVMP_ExpressionType(&s);
			n1=dytfList3s("cast", n, n1);
			BSVMP_Token(s, b2, &ty);
		}
#endif

		bsvmp_curfilename=NULL;
		n3=BSVMP_BlockStatement2(&s);
		n=dytfList3s("define", dytfCons(n1, n2), n3);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "if"))
	{
		s=BSVMP_Token(s, b, &ty);	//(
		n1=BSVMP_Expression(&s);
		s=BSVMP_Token(s, b, &ty);	//)
		n2=BSVMP_BlockStatement2(&s);

		BSVMP_Token(s, b, &ty);
		if(strcmp(b, "else"))
		{
			n=dytfList3s("if", n1, n2);
			*str=s;
			return(n);
		}

		s=BSVMP_Token(s, b, &ty);	//else
		n3=BSVMP_BlockStatement2(&s);
		n=dytfList4s("if", n1, n2, n3);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "begin"))
	{
		n1=BSVMP_BlockStatement2(&s);
		n=dytfList2s("begin", n1);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "while"))
	{
		s=BSVMP_Token(s, b, &ty);	//'('
		n1=BSVMP_Expression(&s);
		s=BSVMP_Token(s, b, &ty);	//')'

		n2=BSVMP_BlockStatement2(&s);
		n=dytfList3s("while", n1, n2);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "$ifdef") ||
		!strcmp(b, "$ifndef") ||
		!strcmp(b, "$if"))
	{
		s=BSVMP_Token(s, b2, &ty2);	//'('
		n1=BSVMP_Expression(&s);
		s=BSVMP_Token(s, b2, &ty2);	//')'

		n2=BSVMP_BlockStatement2(&s);
		n=dytfList3s(b+1, n1, n2);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "ifdef") ||
		!strcmp(b, "ifndef"))
	{
//		b[0]='$';
	
		s=BSVMP_Token(s, b2, &ty2);	//'('
		n1=BSVMP_Expression(&s);
		s=BSVMP_Token(s, b2, &ty2);	//')'

		n2=BSVMP_BlockStatement2(&s);
		n=dytfList3s(b, n1, n2);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "for"))
	{
		s=BSVMP_Token(s, b, &ty);		//(
		s2=BSVMP_Token(s, b, &ty);		//
		s2=BSVMP_Token(s2, b2, &ty);	//

		if(!strcmp(b2, "in"))
		{
			s=s2;

			n1=dytfSymbol(b);
			n2=BSVMP_Expression(&s);
			s=BSVMP_EatSemicolon(s);
			s=BSVMP_Token(s, b, &ty);	//)

			n3=BSVMP_BlockStatement2(&s);
			n=dytfList4s("for_in", n1, n2, n3);

			*str=s;
			return(n);
		}

		n1=BSVMP_Expression2(&s);
		s=BSVMP_EatSemicolon(s);
		n2=BSVMP_Expression2(&s);
		s=BSVMP_EatSemicolon(s);
		n3=BSVMP_Expression2(&s);

		s=BSVMP_Token(s, b, &ty);	//)
		n4=BSVMP_BlockStatement2(&s);

		n=dytfList5s("for", n1, n2, n3, n4);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "switch"))
	{
		s=BSVMP_Token(s, b, &ty);	//(
		n1=BSVMP_Expression(&s);
		s=BSVMP_Token(s, b, &ty);	//)

		s=BSVMP_Token(s, b, &ty); //{
		n2=BSVMP_Block(&s);

		n=dytfCons2(dytfSymbol("switch"), n1, n2);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "try"))
	{
		n=BSVMP_BlockStatement2(&s);
		n=dytfList2s("try", n);

		BSVMP_Token(s, b, &ty);
		while(!strcmp(b, "catch"))
		{
			s=BSVMP_Token(s, b, &ty);	//'catch'

			BSVMP_Token(s, b2, &ty2);
			if(!strcmp(b2, "("))
			{
				s=BSVMP_Token(s, b2, &ty2);	//'('

				s=BSVMP_Token(s, b, &ty);	//name
				n2=dytfSymbol(b);

				s=BSVMP_Token(s, b2, &ty2);	//')'
				
				if(ty2==BSVM_TOKEN_NAME)
				{
					n1=dytfSymbol(b2);
					n2=dytfList3s("cast", n2, n1);

					s=BSVMP_Token(s, b2, &ty2);	//')'
				}
				
				if(!strcmp(b2, ":"))
				{
					s=BSVMP_Token(s, b, &ty);	//name
					n1=dytfSymbol(b);

					n2=dytfList3s("cast", n1, n2);

					s=BSVMP_Token(s, b2, &ty2);	//')'
				}
			}else
			{
				n2=DYTF_NULL;
			}

			n1=BSVMP_BlockStatement2(&s);
			n=dytfList4s("catch", n2, n, n1);

			BSVMP_Token(s, b, &ty);
		}

		if(!strcmp(b, "finally"))
		{
			s=BSVMP_Token(s, b, &ty);	//'finally'

			n1=BSVMP_BlockStatement2(&s);
			n=dytfList3s("finally", n, n1);
		}

		*str=s;
		return(n);
	}

	if(!strcmp(b, "package"))
	{
		s2=BSVMP_TokenQName(s, b2, &ty2);
		if(ty2!=BSVM_TOKEN_NAME)
		{
			n1=BSVMP_BlockStatement2(&s);
			n=dytfList3s("flag", dytfSymbol("package"), n1);
			*str=s;
			return(n);
		}

		s=s2;
		n2=dytfSymbol(b2);
		n1=BSVMP_BlockStatement2(&s);
		n=dytfList3s("package", n2, n1);

		*str=s;
		return(n);
	}

	if(!strcmp(b, "synchronized") && !strcmp(b2, "("))
	{
		s=BSVMP_EatToken(s, "(", BSVM_TOKEN_BRACE);
		n1=BSVMP_Expression(&s);
//		n1=BSVMP_FunArgs(&s);
		s=BSVMP_EatToken(s, ")", BSVM_TOKEN_BRACE);
		n2=BSVMP_BlockStatement2(&s);

		n=dytfList3s("synchronized", n1, n2);
		*str=s;
		return(n);
	}

	if(	!strcmp(b, "public") || !strcmp(b, "private") ||
		!strcmp(b, "protected") || !strcmp(b, "internal") ||
		!strcmp(b, "static") ||
		!strcmp(b, "native") || !strcmp(b, "abstract") ||
		!strcmp(b, "final") || !strcmp(b, "synchronized") ||
		!strcmp(b, "const") || !strcmp(b, "transient") ||
		!strcmp(b, "volatile") ||
//		!strcmp(b, "new") ||
		!strcmp(b, "async") ||
		!strcmp(b, "dynamic") ||
		!strcmp(b, "extern") ||
		!strcmp(b, "typedef") ||
		!strcmp(b, "delegate") ||

		!strcmp(b, "_setuid") ||
		!strcmp(b, "_setgid")
		)
	{
//		n1=BSVMP_BlockStatement2(&s);
		n1=BSVMP_BlockStatement2V(&s);
		n=dytfList3s("flag", dytfSymbol(b), n1);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "$") && !strcmp(b2, "["))
	{
		s=BSVMP_Token(s, b2, &ty2);	//'['
//		n1=BSVMP_Expression(&s);
		n1=BSVMP_FunArgs(&s);
//		s=BSVMP_Token(s, b2, &ty2);	//']'
		n2=BSVMP_BlockStatement2(&s);

		n=dytfList3s("declspec", n1, n2);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "class") ||
		!strcmp(b, "interface") ||
		!strcmp(b, "struct") ||
		!strcmp(b, "value_class"))
	{
		n=BSVMP_Class(str);
		return(n);
	}

#if 1
	if(!strcmp(b, "var"))
	{
		n1=BSVMP_VarsList(&s);
		n=dytfList2s("vars", n1);
		*str=s;
		return(n);
	}
#endif

#if 1
//	if(!strcmp(b, "$C") && !strcmp(b2, "{"))
//	{
//		s=BSVMP_Token(s, b2, &ty2);	//'{'
//	}

	if(!strcmp(b, "$C"))
	{
		n=BSVMP_BlockInlineC(&s);
	
//		n1=BSVMP_BlockStatement2(&s);
//		n=dytfList2s("inline_c", n1);

		*str=s;
		return(n);
	}
#endif

	n=BSVMP_ParseBlockStatementName(b, &s);
	if(!dytfNullP(n))
	{
		*str=s;
		return(n);
	}

#if 1
	s=*str;
	n=BSVMP_TryParseDeclaration(&s);
	if(!dytfNullP(n))
	{
		*str=s;
		return(n);
	}
#endif

	s=*str;
	n=BSVMP_Statement(&s);
	if(!dytfNullP(n))
	{
		s=BSVMP_EatSemicolon(s);
		*str=s;
		return(n);
	}

#if 0
	s=*str;
	n=BSVMP_TryParseDeclaration(&s);
	if(!dytfNullP(n))
	{
		*str=s;
		return(n);
	}
#endif

	BSVM_MSG_Error(s, "PDSCR_Parse_BlockStatementInner: Unrecognizable statement type\n");
//	*str=NULL;
	return(DYTF_UNDEFINED);
}

BGBSVM_API dytf BSVMP_BlockStatement(char **str)
{
	char b[64];
	int ty;
	dytf n, n1;
	int i;
	char *s;

	s=*str;
	n=BSVMP_BlockStatementInner(&s);

#if 0
	BSVMP_Token(s, b, &ty);
	while(!strcmp(b, "catch"))
	{
		s=BSVMP_Token(s, b, &ty);	//'catch'
		s=BSVMP_Token(s, b, &ty);	//name

		n1=BSVMP_BlockStatement2(&s);
		n=dytfList4s("catch", dytfSymbol(b), n, n1);
	}
#endif

	*str=s;
	return(n);

#if 0
//	i=BSVMP_CalcLinenum(*str);

	if(!n)return(n);
	NetParse_AddAttr(n, "linenum", kitoa(i));

	s=BSVMP_GetFilename();
	if(s!=bsvmp_curfilename)
	{
		NetParse_AddAttr(n, "filename", s);
		bsvmp_curfilename=s;
	}

	return(n);
#endif
}

BGBSVM_API dytf BSVMP_BlockDebugMark(char **str)
{
	char *s;
	dytf n;
	int i;

#if 1
	i=BSVMP_CalcLinenum(*str);

	s=BSVMP_GetFilename();
	if(s && (s!=bsvmp_curfilename))
	{
		bsvmp_curfilename=s;
		n=dytfList3s("dbgmark", dytfString(s), dytfInt(i));
		return(n);
	}

	if(s && (i!=bsvmp_curlinenum))
	{
		bsvmp_curlinenum=i;
		n=dytfList2s("linenum", dytfInt(i));
		return(n);
	}

	return(DYTF_NULL);
#endif
}

BGBSVM_API dytf BSVMP_Block(char **str)
{
	char b[256];
	char *s;
	int ty, ty2;
	dytf n, lst;

	s=*str;
	lst=DYTF_NULL;
	while(1)
	{
		s=BSVMP_EatWhite(s);
		if(!s || !(*s))
			break;
		BSVMP_Token(s, b, &ty);
		if(!*s || (*b=='}'))
		{
			s=BSVMP_Token(s, b, &ty);
			break;
		}

		if(*s==';')
		{
			s++;
			continue;
		}

		n=BSVMP_BlockDebugMark(&s);
		if(!dytfNullP(n)) { lst=dytfNAppend(lst, dytfList1(n)); }

		n=BSVMP_BlockStatement(&s);
//		if(!n)
		if(dytfUndefinedP(n))
		{
			BSVM_MSG_Error(s, "PDSCR_Parse_Block: Bad Statement\n");
			return(DYTF_UNDEFINED);
		}
		lst=dytfNAppend(lst, dytfList1(n));
	}

	*str=s;
	return(lst);
}

BGBSVM_API dytf BSVMP_BlockStatement2(char **str)
{
	char b[64];
	char *s;
	int ty;
	dytf n;

	s=*str;

	BSVMP_Token(s, b, &ty);
	if(!strcmp(b, "{"))
	{
		s=BSVMP_Token(s, b, &ty);
		n=BSVMP_Block(&s);
		if(dytfUndefinedP(n))
			return(DYTF_UNDEFINED);
		n=dytfCons(dytfSymbol("begin"), n);

		*str=s;
		return(n);
	}

	n=BSVMP_BlockStatement(str);
	return(n);
}

BGBSVM_API dytf BSVMP_BlockStatement2V(char **str)
{
	char b[64], b2[64];
	char *s, *s1, *s2;
	int ty, ty2;
	dytf n, n1, n2, n3;

	s=*str;

	s1=BSVMP_Token(s, b, &ty);
	BSVMP_Token(s1, b2, &ty2);

	if(!strcmp(b, "{"))
	{
		s=BSVMP_Token(s, b, &ty);
		n=BSVMP_Block(&s);
		if(dytfUndefinedP(n))
			return(DYTF_UNDEFINED);
		n=dytfCons(dytfSymbol("begin"), n);

		*str=s;
		return(n);
	}

	if(BSVMP_CheckModifierName(b))
	{
		s=BSVMP_Token(s, b, &ty);

//		n1=BSVMP_BlockStatement2(&s);
		n1=BSVMP_BlockStatement2V(&s);
		n=dytfList3s("flag", dytfSymbol(b), n1);
		*str=s;
		return(n);
	}

#if 1
	if(ty==BSVM_TOKEN_NAME)
	{
#if 1
		if(!strcmp(b, "get") || !strcmp(b, "set"))
		{
			s=BSVMP_Token(s, b, &ty);

			BSVMP_Token(s, b2, &ty2);
			if(!strcmp(b2, "_") || !strcmp(b2, "*"))
			{
				s=BSVMP_Token(s, b2, &ty2);
				strcat(b, "_any_");
			}else if(ty2==BSVM_TOKEN_NAME)
			{
				s=BSVMP_Token(s, b2, &ty2);
				strcat(b, "_"); strcat(b, b2);
			}

			n1=dytfSymbol(b);
			s=BSVMP_Token(s, b, &ty); //(
			n2=BSVMP_FunVarsList(&s);

			n=BSVMP_ColonTypeExpression(&s);
			if(!dytfNullP(n))
				{ n1=dytfList3s("cast", n, n1); }
			bsvmp_curfilename=NULL;
			n3=BSVMP_BlockStatement2(&s);
			n=dytfList3s("define", dytfCons(n1, n2), n3);

			*str=s;
			return(n);
		}
#endif

#if 1
		if(!strcmp(b, "operator"))
		{
			s1=BSVMP_Token(s, b, &ty);
			s=BSVMP_Token(s, b2, &ty2);

			n1=dytfSymbol(b);
			s=BSVMP_Token(s, b, &ty); //(
			n2=BSVMP_FunVarsList(&s);

#if 1
			if(ty2==BSVM_TOKEN_OPERATOR)
			{
				if(!dytfNullP(dytfCdr(n2)))
					{ s2=BSVMP_NameForBinaryOperator(b2); }
				else
					{ s2=BSVMP_NameForUnaryOperator(b2); }
				strcat(b, "_"); strcat(b, s2);
			}else if(ty2==BSVM_TOKEN_NAME)
				{ strcat(b, "_"); strcat(b, b2); }
#endif

			n1=dytfSymbol(b);

			n=BSVMP_ColonTypeExpression(&s);
			if(!dytfNullP(n))
				{ n1=dytfList3s("cast", n, n1); }
			bsvmp_curfilename=NULL;
			n3=BSVMP_BlockStatement2(&s);
			n=dytfList3s("define", dytfCons(n1, n2), n3);

			*str=s;
			return(n);
		}
#endif

		if(BSVMP_CheckValidDeclName(b) && !strcmp(b2, "("))
		{
			s=BSVMP_Token(s, b, &ty);

			n1=dytfSymbol(b);
			s=BSVMP_Token(s, b, &ty); //(
			n2=BSVMP_FunVarsList(&s);

			n=BSVMP_ColonTypeExpression(&s);
			if(!dytfNullP(n))
				{ n1=dytfList3s("cast", n, n1); }

			bsvmp_curfilename=NULL;
			n3=BSVMP_BlockStatement2(&s);
			n=dytfList3s("define", dytfCons(n1, n2), n3);

			*str=s;
			return(n);
		}


#if 1
//		if(!strcmp(b, "var"))
		if(BSVMP_CheckValidDeclName(b))
		{
			n1=BSVMP_VarsList(&s);
			n=dytfList2s("vars", n1);
			*str=s;
			return(n);
		}
#endif

	}
#endif

	n=BSVMP_BlockStatement(str);
	return(n);
}