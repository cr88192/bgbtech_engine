#include <bgbccc.h>

int BGBJP_HashTypeNameI(BGBCP_ParseState *ctx, char *name)
{
	int hi;
	char *s;

	s=name; hi=0;
	while(*s)hi=(hi*251)+(*s++);
//	hi=(hi>>8)&1023;
	hi=hi&1023;
	
	return(hi);
}

BCCX_Node *BGBJP_LookupTypeI(BGBCP_ParseState *ctx, char *name)
{
	static char *tk_name=NULL;
	char tb[256];
	BCCX_Node *c;
	int hi;
	char *s, *sn;

	s=name; hi=0;
	while(*s)hi=(hi*251)+(*s++);
//	hi=(hi>>8)&1023;
	hi=hi&1023;

	if(!tk_name)tk_name=bgbcc_strdup("name");

	c=ctx->type_hash[hi];
	while(c)
	{
		//s=BCCX_Get(c, "name");
		s=BCCX_Get_SI(c, tk_name);
		if(s && !strcmp(s, name))
//		if(s==sn)
			return(c);
		c=c->hnext;
	}

	return(NULL);
}

BCCX_Node *BGBJP_LookupType(BGBCP_ParseState *ctx, char *name)
{
	char tb[256];
	BCCX_Node *c;
	char *s;
	int i;

	if(ctx->cur_ns)
	{
		sprintf(tb, "%s/%s", ctx->cur_ns, name);
		c=BGBJP_LookupTypeI(ctx, tb);
		if(c)return(c);
	}

	if(ctx->cur_nsi)
	{
		for(i=0; ctx->cur_nsi[i]; i++)
		{
			sprintf(tb, "%s/%s", ctx->cur_nsi[i], name);
			c=BGBJP_LookupTypeI(ctx, tb);
			if(c)return(c);
		}
	}

	c=BGBJP_LookupTypeI(ctx, name);
	if(c)return(c);

	return(NULL);
}


int BGBJP_HandleTypedef(BGBCP_ParseState *ctx, BCCX_Node *n)
{
	BCCX_Node *c, *t;
	char *s;
	int i;

	if(BCCX_TagIsP(n, "vars"))
	{
		c=BCCX_Child(n);
		while(c)
		{
			t=BCCX_Clone(c);
			
#if 1
			s=BCCX_Get(c, "name");
			if(!s) { c=BCCX_Next(c); continue; }
			i=BGBJP_HashTypeNameI(ctx, s);

			t->hnext=ctx->type_hash[i];
			ctx->type_hash[i]=t;
#endif		

			ctx->types=BCCX_AddEnd2(ctx->types, &ctx->e_types, t);
			c=BCCX_Next(c);
		}
		return(0);
	}

	BCCX_Print(n);

	BGBJP_Error(NULL, "BGBJP_HandleTypedef: "
			"Bad typedef definition\n");
	return(-1);
}

BCCX_Node *BGBJP_NewStructJ(BGBCP_ParseState *ctx, char *name, int ty)
{
	char tb[256];
	BCCX_Node *n;
	char *s, *s1;

	s=NULL;
	if(ty==1)s="struct";
	if(ty==2)s="union";
	if(ty==3)s="class";
	if(ty==4)s="interface";
	if(!s)return(NULL);

	n=BCCX_New(s);
	BCCX_Set(n, "name", name);

	if(ctx->cur_ns)BCCX_Set(n, "namespace", ctx->cur_ns);

	return(n);
}

int BGBJP_CheckTypeName(BGBCP_ParseState *ctx, char *name)
{
	char *s;
	s=BGBCP_LookupTypeSig(ctx, name);
	if(s)return(1);
	return(0);

//	return(1);
}


static char *bgbcp_basetypes_c[]={
"char", "short", "int", "long", "float", "double", "void",
"__int64",
"__int128", "__float128", "__float16", "__m64", "__m128",
"__vec2", "__vec3", "__vec4", "__quat", 
"__mat2", "__mat3", "__mat4",

"__v2f", "__v3f", "__v4f",
"__m2f", "__m3f", "__m4f",
"__variant", "__variantf",
"__variant_va", "__variant_this", "__variant_cls",
NULL
};

static char *bgbcp_basetypes_j[]={
"char", "byte", "short", "ushort", "int", "uint", "long", "ulong",
"float", "double", "void", "string", "variant",
"__int128", "__uint128",
"__float128", "__float16", "__m64", "__m128",
"__vec2", "__vec3", "__vec4", "__quat", 
"__mat2", "__mat3", "__mat4",

"__v2f", "__v3f", "__v4f",
"__m2f", "__m3f", "__m4f",
"__variantf",
NULL
};

static int bgbcp_chktoklst(char *str, char **lst)
{
	char *s;
	int i;

	for(i=0; lst[i]; i++)
		if(!strcmp(lst[i], str))
			return(1);

	s="__type_";
	if(!strncmp(str, s, strlen(s)))
		return(1);

	return(0);
}

BCCX_Node *BGBJP_EnumVarsList(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int ty, ty2;
	BCCX_Node *n, *n1, *lst;
	int i;

	s=*str;
	lst=NULL;
	i=0;

	while(1)
	{
		BGBJP_Token(s, b, &ty);
		if(!strcmp(b, ";") || !strcmp(b, "}"))
		{
			s=BGBJP_Token(s, b, &ty);
			break;
		}

		s=BGBJP_Token(s, b, &ty);
		BGBJP_Token(s, b2, &ty2);
		if(!strcmp(b2, "="))
		{
			s=BGBJP_Token(s, b2, &ty);	//'='
			n1=BGBJP_Expression(ctx, &s);
			n1=BGBCC_ReduceExpr(NULL, n1);
			i=BCCX_GetInt(n1, "value");
		}

		n=BCCX_New("def");
		BCCX_Set(n, "name", b);
		BCCX_SetInt(n, "value", i++);
		lst=BCCX_AddEnd(lst, n);

		s=BGBJP_Token(s, b, &ty);
		if(!strcmp(b, ";"))break;
		if(!strcmp(b, "}"))break;

		if(strcmp(b, ","))
		{
			BGBJP_Error(s, "PDSCR_CParse_EnumVarsList: "
				"Invalid token %s in vars list\n", b);
			return(NULL);
		}
	}

	*str=s;
	return(lst);
}

int BGBJP_DefTypeFlag(BGBCP_ParseState *ctx, char *tag)
{
	int i, n;

	i=0;

#if 0
	n=BGBCC_GetOrdinalLang(tag, ctx->lang);
	if(n<0)return(0);

	switch(n)
	{
	case BOTK_STATIC:			i=BGBCC_TYFL_STATIC; break;
	case BOTK_VOLATILE:			i=BGBCC_TYFL_VOLATILE; break;

	case BOTK_SIGNED:			i=BGBCC_TYFL_SIGNED; break;
	case BOTK_UNSIGNED:			i=BGBCC_TYFL_UNSIGNED; break;
	case BOTK_EXTERN:			i=BGBCC_TYFL_EXTERN; break;
	case BOTK_CONST:			i=BGBCC_TYFL_CONST; break;

	case BOTK_VIRTUAL:			i=BGBCC_TYFL_VIRTUAL; break;

	case BOTK_PUBLIC:			i=BGBCC_TYFL_PUBLIC; break;
	case BOTK_PRIVATE:			i=BGBCC_TYFL_PRIVATE; break;
	case BOTK_PROTECTED:		i=BGBCC_TYFL_PROTECTED; break;

	case BOTK_FINAL:			i=BGBCC_TYFL_FINAL; break;
	case BOTK_NATIVE:			i=BGBCC_TYFL_NATIVE; break;
	case BOTK_ABSTRACT:			i=BGBCC_TYFL_ABSTRACT; break;

	case BOTK__COMPLEX:			i=BGBCC_TYFL_UNSIGNED; break;
	case BOTK__IMAGINARY:		i=BGBCC_TYFL_UNSIGNED; break;

	case BOTK_INLINE:			i=BGBCC_TYFL_INLINE; break;
	case BOTK___INLINE:			i=BGBCC_TYFL_INLINE; break;
	case BOTK___FORCEINLINE:	i=BGBCC_TYFL_INLINE; break;

	case BOTK___STDCALL:		i=BGBCC_TYFL_STDCALL; break;
	case BOTK___CDECL:			i=BGBCC_TYFL_CDECL; break;
	case BOTK___PROXY:			i=BGBCC_TYFL_PROXY; break;

	case BOTK___W64:			i=BGBCC_TYFL_INLINE; break;
	case BOTK___PTR32:			i=BGBCC_TYFL_INLINE; break;
	case BOTK___PTR64:			i=BGBCC_TYFL_INLINE; break;

	case BOTK___WIN64:			i=BGBCC_TYFL_WIN64; break;
	case BOTK___AMD64:			i=BGBCC_TYFL_AMD64; break;
	case BOTK___XCALL:			i=BGBCC_TYFL_XCALL; break;

	case BOTK___PACKED:			i=BGBCC_TYFL_PACKED; break;
	case BOTK___GC:				i=BGBCC_TYFL_GC; break;
	case BOTK___NOGC:			i=BGBCC_TYFL_NOGC; break;

	case BOTK___WIDE:			i=BGBCC_TYFL_WIDE; break;
		
	case BOTK___PUBLIC:			i=BGBCC_TYFL_PUBLIC; break;
	case BOTK___PRIVATE:		i=BGBCC_TYFL_PRIVATE; break;
	case BOTK___PROTECTED:		i=BGBCC_TYFL_PROTECTED; break;

	case BOTK___FINAL:			i=BGBCC_TYFL_FINAL; break;

	case BOTK___VIRTUAL:		i=BGBCC_TYFL_VIRTUAL; break;
	case BOTK___NATIVE:			i=BGBCC_TYFL_NATIVE; break;
	case BOTK___ABSTRACT:		i=BGBCC_TYFL_ABSTRACT; break;

	default: break;
	}

	if(i)return(i);

	return(0);
#endif

	if(!strcmp(tag, "static"))i=BGBCC_TYFL_STATIC;
//	if(!strcmp(tag, "const"))i=BGBCC_TYFL_STATIC;
	if(!strcmp(tag, "volatile"))i=BGBCC_TYFL_VOLATILE;

	if(	(ctx->lang==BGBCC_LANG_C) ||
		(ctx->lang==BGBCC_LANG_CPP))
	{
#if 1
		if(!strcmp(tag, "signed"))i=BGBCC_TYFL_SIGNED;
		if(!strcmp(tag, "unsigned"))i=BGBCC_TYFL_UNSIGNED;

		if(!strcmp(tag, "extern"))i=BGBCC_TYFL_EXTERN;
		if(!strcmp(tag, "const"))i=BGBCC_TYFL_CONST;
		if(!strcmp(tag, "inline"))i=BGBCC_TYFL_INLINE;

		if(!strcmp(tag, "__inline"))i=BGBCC_TYFL_INLINE;
		if(!strcmp(tag, "__forceinline"))i=BGBCC_TYFL_INLINE;

		if(!strcmp(tag, "__stdcall"))i=BGBCC_TYFL_STDCALL;
		if(!strcmp(tag, "__cdecl"))i=BGBCC_TYFL_CDECL;
		if(!strcmp(tag, "__proxy"))i=BGBCC_TYFL_PROXY;

		if(!strcmp(tag, "__w64"))i=BGBCC_TYFL_INLINE;
		if(!strcmp(tag, "__ptr64"))i=BGBCC_TYFL_INLINE;
		if(!strcmp(tag, "__ptr32"))i=BGBCC_TYFL_INLINE;

		if(!strcmp(tag, "__win64"))i=BGBCC_TYFL_WIN64;
		if(!strcmp(tag, "__amd64"))i=BGBCC_TYFL_AMD64;
		if(!strcmp(tag, "__xcall"))i=BGBCC_TYFL_XCALL;

		if(!strcmp(tag, "__packed"))i=BGBCC_TYFL_PACKED;
		if(!strcmp(tag, "__gc"))i=BGBCC_TYFL_GC;
		if(!strcmp(tag, "__nogc"))i=BGBCC_TYFL_NOGC;

		if(!strcmp(tag, "_Complex"))i=BGBCC_TYFL_UNSIGNED;
		if(!strcmp(tag, "_Imaginary"))i=BGBCC_TYFL_UNSIGNED;
		if(!strcmp(tag, "__wide"))i=BGBCC_TYFL_WIDE;
#endif

		if(i)return(i);
	}

	if(ctx->lang==BGBCC_LANG_C)
	{
		if(!strcmp(tag, "__public"))i=BGBCC_TYFL_PUBLIC;
		if(!strcmp(tag, "__private"))i=BGBCC_TYFL_PRIVATE;
		if(!strcmp(tag, "__protected"))i=BGBCC_TYFL_PROTECTED;
		if(!strcmp(tag, "__final"))i=BGBCC_TYFL_FINAL;

		if(!strcmp(tag, "__virtual"))i=BGBCC_TYFL_VIRTUAL;
		if(!strcmp(tag, "__native"))i=BGBCC_TYFL_NATIVE;
		if(!strcmp(tag, "__abstract"))i=BGBCC_TYFL_ABSTRACT;

		if(i)return(i);
	}

	if(ctx->lang==BGBCC_LANG_CPP)
	{
		if(!strcmp(tag, "virtual"))i=BGBCC_TYFL_VIRTUAL;
		if(i)return(i);
	}

	if(	(ctx->lang==BGBCC_LANG_CPP) ||
		(ctx->lang==BGBCC_LANG_JAVA) ||
		(ctx->lang==BGBCC_LANG_CS) ||
		(ctx->lang==BGBCC_LANG_BS2))
	{
		if(!strcmp(tag, "public"))i=BGBCC_TYFL_PUBLIC;
		if(!strcmp(tag, "private"))i=BGBCC_TYFL_PRIVATE;
		if(!strcmp(tag, "protected"))i=BGBCC_TYFL_PROTECTED;

		if(!strcmp(tag, "__signed"))i=BGBCC_TYFL_SIGNED;
		if(!strcmp(tag, "__unsigned"))i=BGBCC_TYFL_UNSIGNED;

		if(i)return(i);
	}

	if(ctx->lang==BGBCC_LANG_CS)
	{
		if(!strcmp(tag, "extern"))i=BGBCC_TYFL_EXTERN;
		if(!strcmp(tag, "const"))i=BGBCC_TYFL_CONST;
		if(!strcmp(tag, "inline"))i=BGBCC_TYFL_INLINE;

		if(!strcmp(tag, "virtual"))i=BGBCC_TYFL_VIRTUAL;
		if(!strcmp(tag, "new"))i=BGBCC_TYFL_NEW;
		if(!strcmp(tag, "override"))i=BGBCC_TYFL_OVERRIDE;
		
		if(!strcmp(tag, "internal"))i=BGBCC_TYFL_INTERNAL;

		if(!strcmp(tag, "sealed"))i=BGBCC_TYFL_SEALED;
		if(!strcmp(tag, "readonly"))i=BGBCC_TYFL_READONLY;

		//if(!strcmp(tag, "native"))i=BGBCC_TYFL_NATIVE;
		if(!strcmp(tag, "abstract"))i=BGBCC_TYFL_ABSTRACT;

		if(!strcmp(tag, "event"))i=BGBCC_TYFL_EVENT;
		if(!strcmp(tag, "unsafe"))i=BGBCC_TYFL_UNSAFE;

		if(i)return(i);
	}

	if(ctx->lang==BGBCC_LANG_JAVA)
	{
		if(!strcmp(tag, "final"))i=BGBCC_TYFL_FINAL;
		if(!strcmp(tag, "transient"))i=BGBCC_TYFL_TRANSIENT;
		if(!strcmp(tag, "native"))i=BGBCC_TYFL_NATIVE;
		if(!strcmp(tag, "abstract"))i=BGBCC_TYFL_ABSTRACT;
		if(!strcmp(tag, "synchronized"))i=BGBCC_TYFL_TRANSIENT;

		if(i)return(i);
	}

	if(ctx->lang==BGBCC_LANG_BS2)
	{
		if(!strcmp(tag, "final"))i=BGBCC_TYFL_FINAL;
		if(!strcmp(tag, "transient"))i=BGBCC_TYFL_TRANSIENT;
		if(!strcmp(tag, "native"))i=BGBCC_TYFL_NATIVE;
		if(!strcmp(tag, "abstract"))i=BGBCC_TYFL_ABSTRACT;
		if(!strcmp(tag, "synchronized"))i=BGBCC_TYFL_TRANSIENT;

		if(!strcmp(tag, "extern"))i=BGBCC_TYFL_EXTERN;
		if(!strcmp(tag, "const"))i=BGBCC_TYFL_CONST;
		if(!strcmp(tag, "inline"))i=BGBCC_TYFL_INLINE;

		if(!strcmp(tag, "virtual"))i=BGBCC_TYFL_VIRTUAL;
		if(!strcmp(tag, "new"))i=BGBCC_TYFL_NEW;
		if(!strcmp(tag, "override"))i=BGBCC_TYFL_OVERRIDE;

		if(!strcmp(tag, "dynamic"))i=BGBCC_TYFL_DYNAMIC;

		if(!strcmp(tag, "ref"))i=BGBCC_TYFL_BYREF;
		if(!strcmp(tag, "out"))i=BGBCC_TYFL_BYREF;
		if(!strcmp(tag, "delegate"))i=BGBCC_TYFL_DELEGATE;

		if(i)return(i);
	}

	return(i);
}

BCCX_Node *BGBJP_DefClass(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *s1, *s2;
	int i, j, ty, ty2, fl, cty;
	BCCX_Node *n, *n1, *n2, *anl;

	s=*str;

	anl=NULL;

	fl=0; j=0;
	while(1)
	{
		BGBJP_Token(s, b, &ty);
		i=BGBJP_DefTypeFlag(ctx, b);
		if(i)
		{
			fl|=i;
			s=BGBJP_Token(s, b, &ty);
			j++;
			continue;
		}

		while(1)
		{
			n1=BGBJP_DeclAttribute(ctx, &s);
			if(!n1)break;
			anl=BCCX_AddEnd(anl, n1);
		}

		break;
	}

	if(!strcmp(b, "struct") || !strcmp(b, "union") ||
		!strcmp(b, "class") || !strcmp(b, "interface") ||
		!strcmp(b, "__class") || !strcmp(b, "__interface"))
	{
		s=BGBJP_Token(s, b, &ty);	//struct

		cty=0;
		if(!strcmp(b, "struct"))cty=1;
		if(!strcmp(b, "union"))cty=2;
		if(!strcmp(b, "class"))cty=3;
		if(!strcmp(b, "interface"))cty=4;

		if(!strcmp(b, "__class"))cty=3;
		if(!strcmp(b, "__interface"))cty=4;

		while(1)
		{
			n1=BGBJP_DeclAttribute(ctx, &s);
			if(!n1)break;
			anl=BCCX_AddEnd(anl, n1);
		}

		s1=BGBJP_Token(s, b, &ty);	//name, '{', ...
		BGBJP_Token(s1, b2, &ty2);	//name, '{', ...

		n=NULL;
		if(ty==BTK_NAME)
		{
			s=BGBJP_Token(s, b, &ty);	//name
			n=BGBJP_NewStructJ(ctx, b, cty);

			s1=BGBJP_Token(s, b, &ty);	//name, '{', ...
			BGBJP_Token(s1, b2, &ty2);	//name, '{', ...
		}

		if(n && !strcmp(b, "<"))
		{
			s=BGBJP_Token(s, b, &ty);	//'<'
			n1=BGBJP_TypeArgs(ctx, &s);
			BCCX_Add(n, BCCX_New1("sharpargs", n1));

			s1=BGBJP_Token(s, b, &ty);	//name, '{', ...
			BGBJP_Token(s1, b2, &ty2);	//name, '{', ...
		}

#if 0
		if(n && !strcmp(b, ":"))
		{
			s=BGBJP_Token(s, b, &ty);	//':'

			n1=NULL;
			if(!n1)n1=BCCX_New("super");
			while(1)
			{
				n2=BGBJP_DefType(ctx, &s);
				BCCX_Add(n1, n2);

				BGBJP_Token(s, b, &ty);
				if(strcmp(b, ","))break;
				s=BGBJP_Token(s, b, &ty);
			}
			BCCX_Add(n, n1);
			BGBJP_Token(s, b, &ty);
		}
#endif

#if 1
		if(n && !strcmp(b, ":"))
		{
			s=BGBJP_Token(s, b, &ty);	//':'

			n1=BGBJP_DefType(ctx, &s);
			BCCX_Add(n, BCCX_New1("super", n1));

			BGBJP_Token(s, b, &ty);
			n1=NULL;
			while(!strcmp(b, ","))
			{
				if(!n1)n1=BCCX_New("impl");
				s=BGBJP_Token(s, b, &ty);	//','
				n2=BGBJP_DefType(ctx, &s);
				BCCX_Add(n1, n2);

				BGBJP_Token(s, b, &ty);
			}
			BCCX_Add(n, n1);
			BGBJP_Token(s, b, &ty);
		}
#endif

#if 1
		if(n && !strcmp(b, "extends"))
		{
			s=BGBJP_Token(s, b, &ty);	//'extends'

			n1=BGBJP_DefType(ctx, &s);
			BCCX_Add(n, BCCX_New1("super", n1));
			BGBJP_Token(s, b, &ty);
		}

		if(n && !strcmp(b, "implements"))
		{
			s=BGBJP_Token(s, b, &ty);	//'implements'

			n1=BCCX_New("impl");

			while(1)
			{
				n2=BGBJP_DefType(ctx, &s);
				BCCX_Add(n1, n2);

				BGBJP_Token(s, b, &ty);
				if(strcmp(b, ","))
					break;
				s=BGBJP_Token(s, b, &ty);	//','
			}
			BCCX_Add(n, n1);
			BGBJP_Token(s, b, &ty);
		}
#endif

		if(!n)
		{

			if(!strcmp(b, "{"))
			{
				s1=s; i=0; j=1;
				while(*s1 && j)
				{
					s1=BGBJP_Token(s1, b2, &ty2);
					if(!strcmp(b2, "{"))j++;
					if(!strcmp(b2, "}"))j--;
					s2=b2;
					while(*s2)i=i*127+(*s2++);
					i=i*127+' ';
				}
				sprintf(b2, "HTX%08X", i);
				s1=bgbcc_strdup(b2);
			}else
			{
				s1=BGBJP_GetLastFileName();
				i=BGBJP_GetLastLineNumber();
				sprintf(b2, "%s:%d", s1, i);
				s1=b2; i=0; while(*s1)i=i*127+(*s1++);

				sprintf(b2, "HFN%08X", i);
				s1=bgbcc_strdup(b2);
			}

			n=BGBJP_NewStructJ(ctx, s1, cty);
		}

		if(!strcmp(b, "{"))
		{
			s1=ctx->cur_class;
			ctx->cur_class=BCCX_Get(n, "name");
			
			s=BGBJP_Token(s, b, &ty); //'{'
			n1=BGBJP_Block(ctx, &s);
			BCCX_Add(n, BCCX_New1("body", n1));
			
			ctx->cur_class=s1;
		}

		BCCX_SetInt(n, "flags", fl);
		if(anl)BCCX_Add(n, BCCX_New1("attributes", anl));

		*str=s;
		return(n);
	}

	if(!strcmp(b, "enum"))
	{
		s=BGBJP_Token(s, b, &ty);	//enum
		s1=BGBJP_Token(s, b, &ty);	//name|'{'
		BGBJP_Token(s1, b2, &ty2);	//name|'{'|...

		if(ty==BTK_NAME)
		{
			s=BGBJP_Token(s, b, &ty);	//name
//			n=BGBJP_GetEnum(ctx, b);

			n=BCCX_New("enum");
			BCCX_Set(n, "name", b);

			BGBJP_Token(s, b2, &ty2);	//'{', ...
			if(!strcmp(b2, "{"))
			{
				s=BGBJP_Token(s, b2, &ty2); //'{'
				n1=BGBJP_EnumVarsList(ctx, &s);
				BCCX_Add(n, BCCX_New1("defs", n1));
			}
		}else if(!strcmp(b, "{"))
		{
			s=BGBJP_Token(s, b, &ty); //'{'
			n1=BGBJP_EnumVarsList(ctx, &s);

			s1=BGBCC_GenSym2();
			n=BCCX_New("enum");
			BCCX_Set(n, "name", s1);
			BCCX_Add(n, BCCX_New1("defs", n1));
		}

		BCCX_SetInt(n, "flags", fl);
		if(anl)BCCX_Add(n, anl);

		*str=s;
		return(n);
	}

	return(NULL);
}

BCCX_Node *BGBJP_DeclAttribute(BGBCP_ParseState *ctx, char **str)
{
	char b[4096], b2[4096];
	char *s, *s1, *bty;
	int i, j, ty, ty2, fl;
	BCCX_Node *n, *n1, *n2, *nl;

	s=*str;

	BGBJP_Token(s, b, &ty);
//	if(ty!=BTK_NAME)return(NULL);

	if(!strcmp(b, "__declspec"))
	{
		nl=NULL;
	
		s=BGBJP_Token(s, b, &ty);	//__declspec
		s=BGBJP_Token(s, b, &ty);	//'('
		while(1)
		{
			BGBJP_Token(s, b, &ty);
			if(!strcmp(b, ")"))
			{
				s=BGBJP_Token(s, b, &ty);
				break;
			}
			if(!strcmp(b, ","))
			{
				s=BGBJP_Token(s, b, &ty);
				continue;
			}
			if(ty!=BTK_NAME)break;

			s=BGBJP_Token(s, b, &ty);	//name
			BGBJP_Token(s, b2, &ty2);	//'('
			if(!strcmp(b2, "("))
			{
				s=BGBJP_Token(s, b2, &ty2);	//'('
				n2=BGBJP_FunArgs(ctx, &s);
				n1=BCCX_New1("attr", BCCX_New1("args", n2));
				BCCX_Set(n1, "name", b);
			}else
			{
				n1=BCCX_New("attr");
				BCCX_Set(n1, "name", b);
			}

			nl=BCCX_AddEnd(nl, n1);
		}
			
		n=BCCX_New1("declspec", nl);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "__attribute__"))
	{
		nl=NULL;
	
		s=BGBJP_Token(s, b, &ty);	//__attribute__
		s=BGBJP_Token(s, b, &ty);	//'('
		s=BGBJP_Token(s, b, &ty);	//'('
		while(1)
		{
			BGBJP_Token(s, b, &ty);
			if(!strcmp(b, ")"))
			{
				s=BGBJP_Token(s, b, &ty);	//')'
				s=BGBJP_Token(s, b, &ty);	//')'
				break;
			}
			if(!strcmp(b, ","))
			{
				s=BGBJP_Token(s, b, &ty);
				continue;
			}
			if(ty!=BTK_NAME)break;

			s=BGBJP_Token(s, b, &ty);	//name
			BGBJP_Token(s, b2, &ty2);	//'('
			if(!strcmp(b2, "("))
			{
				s=BGBJP_Token(s, b2, &ty2);	//'('
				n2=BGBJP_FunArgs(ctx, &s);
				n1=BCCX_New1("attr", BCCX_New1("args", n2));
				BCCX_Set(n1, "name", b);
			}else
			{
				n1=BCCX_New("attr");
				BCCX_Set(n1, "name", b);
			}

			nl=BCCX_AddEnd(nl, n1);
		}
			
		n=BCCX_New1("attribute", nl);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "throws"))
	{
		s=BGBJP_Token(s, b, &ty);	//'implements'

		n1=BCCX_New("throws");

		while(1)
		{
			n2=BGBJP_DefType(ctx, &s);
			BCCX_Add(n1, n2);

			BGBJP_Token(s, b, &ty);
			if(strcmp(b, ","))
				break;
			s=BGBJP_Token(s, b, &ty);	//','
		}

		n=BCCX_New1("attribute", n1);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "["))
	{
		nl=NULL;
	
		s=BGBJP_Token(s, b, &ty);	//'['
		while(1)
		{
			BGBJP_Token(s, b, &ty);
			if(!strcmp(b, "]"))
			{
				s=BGBJP_Token(s, b, &ty);
				break;
			}
			if(!strcmp(b, ","))
			{
				s=BGBJP_Token(s, b, &ty);
				continue;
			}
			if(ty!=BTK_NAME)break;

			s=BGBJP_Token(s, b, &ty);	//name
			BGBJP_Token(s, b2, &ty2);	//'('
			if(!strcmp(b2, "("))
			{
				s=BGBJP_Token(s, b2, &ty2);	//'('
				n2=BGBJP_FunArgs(ctx, &s);
				n1=BCCX_New1("attr", BCCX_New1("args", n2));
				BCCX_Set(n1, "name", b);
			}else
			{
				n1=BCCX_New("attr");
				BCCX_Set(n1, "name", b);
			}

			nl=BCCX_AddEnd(nl, n1);
		}

		n=BCCX_New1("declspec", nl);
		*str=s;
		return(n);
	}

	if(!strcmp(b, "<"))
	{
		s=BGBJP_Token(s, b, &ty);	//'<'
		nl=BGBJP_TypeArgs(ctx, &s);
		n=BCCX_New1("attribute", BCCX_New1("sharpargs", nl));
		*str=s;
		return(n);
	}

	if(!strcmp(b, "@"))
	{
		nl=NULL;
	
		s=BGBJP_Token(s, b, &ty);	//'@'
		BGBJP_Token(s, b, &ty);
		if(ty!=BTK_NAME) return(NULL);

		s=BGBJP_Token(s, b, &ty);	//name
		BGBJP_Token(s, b2, &ty2);	//'('
		if(!strcmp(b2, "("))
		{
			s=BGBJP_Token(s, b2, &ty2);	//'('
			n2=BGBJP_FunArgs(ctx, &s);
			n1=BCCX_New1("annotation", BCCX_New1("args", n2));
			BCCX_Set(n1, "name", b);
		}else
		{
			n1=BCCX_New("annotation");
			BCCX_Set(n1, "name", b);
		}

		n=BCCX_New1("attribute", n1);
		*str=s;
		return(n);
	}

	return(NULL);
}

BCCX_Node *BGBJP_DefType(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *s1;
	int i, j, ty, ty2, fl;
	BCCX_Node *n, *n1, *n2, *attrl, *attrle;

	s=*str;

	fl=0; j=0; attrl=NULL; attrle=NULL;
	while(1)
	{
		BGBJP_Token(s, b, &ty);

//		if(ty!=BTK_NAME)break;
		i=BGBJP_DefTypeFlag(ctx, b);
		if(i)
		{
			fl|=i;
			s=BGBJP_Token(s, b, &ty);
			j++;
			continue;
		}

		n1=BGBJP_DeclAttribute(ctx, &s);
		if(n1)
		{
			attrl=BCCX_AddEnd2(attrl, &attrle, n1);
			//j++;
			continue;
		}

		break;
	}

#if 0
	if(!strcmp(b, "struct") || !strcmp(b, "union") ||
		!strcmp(b, "enum") || !strcmp(b, "class") ||
		!strcmp(b, "interface"))
	{
		n=BGBJP_DefClassC(ctx, &s);
//		if(attrl)BCCX_Add(n, attrl);

		*str=s;
		return(n);	
	}
#endif

	n1=BGBJP_TypeExpression(ctx, &s);
	if(n1)
	{
		if(BCCX_TagIsP(n1, "ref"))
		{
			s1=BCCX_Get(n1, "name");

			n=BCCX_New("type");
			BCCX_SetInt(n, "flags", fl);
			BCCX_Set(n, "name", s1);

			if(attrl)BCCX_Add(n, attrl);

			*str=s;
			return(n);
		}
	
//		n=BCCX_FindTag(n, "type");

		n1=BCCX_Clone(n1);
//		i=BCCX_GetInt(n, "flags");
//		BCCX_SetInt(n, "flags", i|fl);

		n=BCCX_New("type");
		BCCX_Add(n, BCCX_New1("expr", n1));
		BCCX_SetInt(n, "flags", fl);

		if(attrl)BCCX_Add(n, BCCX_New1("attributes", attrl));

		*str=s;
		return(n);
	}

	return(NULL);
}



BCCX_Node *BGBJP_FlagsBlock(BGBCP_ParseState *ctx, char **str)
{
	char b[256], b2[256];
	char *s, *s1, *s2;
	int i, j, ty, ty2, fl;
	BCCX_Node *n, *n1, *n2, *n3, *attrl, *attrle;

	s=*str;

	fl=0; j=0; attrl=NULL; attrle=NULL;
	while(1)
	{
		BGBJP_Token(s, b, &ty);
		if(ty!=BTK_NAME)break;

		i=BGBJP_DefTypeFlag(ctx, b);
		if(i)
		{
			fl|=i;
			s=BGBJP_Token(s, b, &ty);
			j++;
			continue;
		}

		n1=BGBJP_DeclAttribute(ctx, &s);
		if(n1)
		{
			attrl=BCCX_AddEnd2(attrl, &attrle, n1);
			j++;
			continue;
		}

		break;
	}

	if(!j)return(NULL);

#if 1
	s1=BGBJP_Token(s, b, &ty);
	s1=BGBJP_Token(s1, b2, &ty2);
		
	if(ctx->cur_class && !strcmp(b, ctx->cur_class) &&
		!strcmp(b2, "("))
	{
		s=s1;
		n1=BGBJP_FunVarsList(ctx, &s);
		if(!n1 && (s==s1))
			return(NULL);

		s1=BGBJP_Token(s, b, &ty);
		s1=BGBJP_Token(s1, b2, &ty2);
		if(!strcmp(b, ":") && (ty2==BTK_NAME))
		{
			s=BGBJP_Token(s1, b, &ty);
			if(strcmp(b, "("))
				return(NULL);
			s1=s;
//			n3=BGBJP_FunVarsList(ctx, &s);
			n3=BGBJP_FunArgs(ctx, &s);
			if((!n3 && (s==s1)) || !s)
				return(NULL);
			n3=BCCX_New1("defer", BCCX_New1("args", n3));
			BCCX_Set(n3, "name", b2);
		}else n3=NULL;

		n2=BGBJP_BlockStatement2(ctx, &s);
		if(!n2)return(NULL);
			
		n=BCCX_New2("constructor",
			BCCX_New1("args", n1),
			BCCX_New1("body", n2));
		BCCX_SetInt(n, "flags", fl);
		if(attrl)BCCX_Add(n, attrl);
		if(n3)BCCX_Add(n, n3);

		*str=s;
		return(n);
	}
#endif

	n1=BGBJP_BlockStatement2(ctx, &s);
	if(!n1) return(NULL);

#if 0
	if(BCCX_TagIsP(n1, "funcall"))
	{
		BGBJP_Token(s, b, &ty);
		if(!strcmp(b, "{"))
		{
			n2=BGBJP_BlockStatement2(ctx, &s);
			BCCX_Add(n1, BCCX_New1("body", n2));
			
			n1->tag=bscc_strdup("constructor");
		}
	}
#endif

	n=BCCX_New1("groupflags", BCCX_New1("body", n1));
	BCCX_SetInt(n, "flags", fl);
	if(attrl)BCCX_Add(n, attrl);

	*str=s;
	return(n);
}
