/*
Purpose:
Convert native packages into C.

Will emit a header and a C source portion.
The header will contain basic prototypes for the exported functions.

The C source portion will contain wrapper functions.

 rettype funcname(<args...>)
 {
 	static dyt hdl=NULL;
 	return(bsvmCachedCallSig<t>(&hdl,
 		"<modname>", "<name>", "<sig>", <args...>));
 }

 */


#include <bgbsvm.h>

FILE *bsvmc_pkg2c_outc=NULL;
FILE *bsvmc_pkg2c_outh=NULL;
FILE *bsvmc_pkg2c_outbs=NULL;
char *bsvmc_pkg2c_pkgname=NULL;
char *bsvmc_pkg2c_apiname=NULL;
char *bsvmc_pkg2c_libhdr=NULL;

char *bsvmc_pkg2c_outbs_buf=NULL;
char *bsvmc_pkg2c_outbs_ct=NULL;

void BSVMC_PKG2C_PrintH(char *str, ...)
{
//	char tb[1024];
	va_list lst;
	
	if(!bsvmc_pkg2c_outh)
		return;
	
	va_start(lst, str);
//	vsprintf(tb, str, lst);
	vfprintf(bsvmc_pkg2c_outh, str, lst);
	va_end(lst);
	
//	fputs(tb, bsvmc_pkg2c_outh);
}

void BSVMC_PKG2C_PrintC(char *str, ...)
{
//	char tb[1024];
	va_list lst;

	if(!bsvmc_pkg2c_outc)
		return;
	
	va_start(lst, str);
//	vsprintf(tb, str, lst);
	vfprintf(bsvmc_pkg2c_outc, str, lst);
	va_end(lst);
	
//	fputs(tb, bsvmc_pkg2c_outh);
}

void BSVMC_PKG2C_PrintBS(char *str, ...)
{
//	char tb[1024];
	va_list lst;

//	if(!bsvmc_pkg2c_outbs)
//		return;
	if(!bsvmc_pkg2c_outbs_ct)
		return;
	
	va_start(lst, str);
//	vsprintf(tb, str, lst);
//	vfprintf(bsvmc_pkg2c_outbs, str, lst);
	vsprintf(bsvmc_pkg2c_outbs_ct, str, lst);
	bsvmc_pkg2c_outbs_ct+=strlen(bsvmc_pkg2c_outbs_ct);
	va_end(lst);
	
//	fputs(tb, bsvmc_pkg2c_outh);
}

char *BSVMC_PKG2C_SigCTypeName(char *sig)
{
	char tb[256];
	char *s, *s0;
	int i;

	switch(*sig)
	{
	case 'a': s="sbyte"; break;
	case 'b': s="bool"; break;
	case 'c': s="char"; break;
	case 'd': s="double"; break;
	case 'e': s="f80"; break;
	case 'f': s="float"; break;
	case 'g': s="f128"; break;
	case 'h': s="byte"; break;
	case 'i': s="s32"; break;
	case 'j': s="u32"; break;
	case 'k': s="float"; break;
	case 'l': s="s64"; break;
	case 'm': s="u64"; break;
	case 'n': s="s128"; break;
	case 'o': s="u128"; break;
	//p, q
	case 'r': s="dyt"; break;
	case 's': s="s16"; break;
	case 't': s="u16"; break;
	//u
	case 'v': s="void"; break;
	case 'w': s="u16"; break;
	case 'x': s="s64"; break;
	case 'y': s="u64"; break;
	case 'z': s="..."; break;

	case 'L': s="dycObject"; break;
	case 'Q': s="dycArray"; break;

	case 'U':
		s=sig+1;
//		s0=dyll_SigReadQName(&s);
		s0=dyllSigGetStructName(sig);
		s=dyllString(s0);
		break;

	case 'P':
		s=sig; i=0;
		while(*s=='P') { s++; i++; }
		s0=BSVMC_PKG2C_SigCTypeName(s);
		strcpy(tb, s0);
		while(i--)
			strcat(tb, "*");
		s=dyllString(tb);
		break;

	default: s=NULL;
	}
	
	return(s);
}

char *BSVMC_PKG2C_SigCTypeSuf(char *sig)
{
	char *s;

	switch(*sig)
	{
	case 'a': s="i"; break;
	case 'b': s="i"; break;
	case 'c': s="i"; break;
	case 'd': s="d"; break;
	case 'e': s="d"; break;
	case 'f': s="f"; break;
	case 'g': s="g"; break;
	case 'h': s="i"; break;
	case 'i': s="i"; break;
	case 'j': s="i"; break;
	case 'k': s="f"; break;
	case 'l': s="l"; break;
	case 'm': s="l"; break;
	case 'n': s="lx"; break;
	case 'o': s="lx"; break;
	//p, q
	case 'r': s="r"; break;
	case 's': s="i"; break;
	case 't': s="i"; break;
	//u
	case 'v': s="v"; break;
	case 'w': s="i"; break;
	case 'x': s="l"; break;
	case 'y': s="l"; break;
	case 'z': s="r"; break;

	case 'L': s="p"; break;
	case 'Q': s="p"; break;
	case 'P': s="p"; break;

	default: s=NULL;
	}
	
	return(s);
}

void BSVMC_PKG2C_EmitProtoNameSig(BSVM_SVMCState *ctx,
	char *name, char *sig)
{
	char *s0, *s1, *s;
	int n;

	if(!bsvmc_pkg2c_pkgname)
		return;

	s0=dyllSigRet(sig);
	s1=BSVMC_PKG2C_SigCTypeName(s0);
	
	if(bsvmc_pkg2c_apiname)
		BSVMC_PKG2C_PrintH("%s ", bsvmc_pkg2c_apiname);
	
	BSVMC_PKG2C_PrintH("%s %s(", s1, name);
	
	s=sig;
	if(*s=='(')s++; n=0;
	while(*s && (*s!=')'))
	{
		s0=BSVMC_PKG2C_SigCTypeName(s);
		BSVMC_PKG2C_PrintH("%s a%d", s0, n++);
		
		s=dyllSigNext(s);
		if(*s && (*s!=')'))
			BSVMC_PKG2C_PrintH(", ");
	}
	
	BSVMC_PKG2C_PrintH(");\n");
}

void BSVMC_PKG2C_EmitFuncNameSig(BSVM_SVMCState *ctx,
	char *name, char *sig)
{
	char *s0, *s1, *s, *sr, *srs;
	int i, n;

	BSVMC_PKG2C_EmitProtoNameSig(ctx, name, sig);

	if(!bsvmc_pkg2c_pkgname)
		return;

	if(bsvmc_pkg2c_apiname)
		BSVMC_PKG2C_PrintC("%s ", bsvmc_pkg2c_apiname);

	sr=dyllSigRet(sig);
	s1=BSVMC_PKG2C_SigCTypeName(sr);
	BSVMC_PKG2C_PrintC("%s %s(", s1, name);

	srs=BSVMC_PKG2C_SigCTypeSuf(sr);
	
	s=sig;
	if(*s=='(')s++; n=0;
	while(*s && (*s!=')'))
	{
		s0=BSVMC_PKG2C_SigCTypeName(s);
		BSVMC_PKG2C_PrintC("%s a%d", s0, n++);
		
		s=dyllSigNext(s);
		if(*s && (*s!=')'))
			BSVMC_PKG2C_PrintC(", ");
	}
	
	BSVMC_PKG2C_PrintC(")\n{\n");
	BSVMC_PKG2C_PrintC("\tstatic dyt hdl=NULL;\n");

	if(*sr=='v')
	{
		BSVMC_PKG2C_PrintC("\tbsvmCachedCallSigv(&hdl, \"%s\", ",
			bsvmc_pkg2c_pkgname);
		BSVMC_PKG2C_PrintC("\"%s\", \"%s\"", name, sig);
		for(i=0; i<n; i++)
			BSVMC_PKG2C_PrintC(", a%d", i);
		BSVMC_PKG2C_PrintC(");\n");
	}else
	{
		BSVMC_PKG2C_PrintC("\treturn(bsvmCachedCallSig%s(&hdl, \"%s\", ",
			srs, bsvmc_pkg2c_pkgname);
		BSVMC_PKG2C_PrintC("\"%s\", \"%s\"", name, sig);
		for(i=0; i<n; i++)
			BSVMC_PKG2C_PrintC(", a%d", i);
		BSVMC_PKG2C_PrintC("));\n");
	}

	BSVMC_PKG2C_PrintC("}\n\n");
}

int BSVMC_PKG2C_CompilePackage(BSVM_SVMCState *ctx, dyt l)
{
	char *opkg;
	char *s;
	dyt t;
	int opcap, oplvl;
	int i, j;

	opcap=ctx->pcap;
	oplvl=ctx->plvl++;
	opkg=bsvmc_pkg2c_pkgname;

	ctx->pcap++;

//	i=BSVMC_IndexLit(ctx, dycadr(l));
//	j=BSVMC_IndexLit(ctx, ctx->cs_fl);
//	BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_PACKAGE);
//	BSVMC_EncIDX(ctx, i);
//	BSVMC_EncIDX(ctx, j);

	s=dysymbolv(dycadr(l));
	if(!strncmp(s, "C/", 2))
	{
		bsvmc_pkg2c_pkgname=s+2;
	}else if(!strcmp(s, "C"))
	{
		bsvmc_pkg2c_pkgname="";
	}
	

	t=dycaddr(l);
	if(t)
	{
		BSVMC_PKG2C_CompileStatement(ctx, t);

		while(ctx->plvl>oplvl)
		{
//			BSVMC_EmitOp(ctx, BSVM_SOP_END_PACKAGE);
//			BSVMC_EmitOp(ctx, BSVM_SOP_POP);
			ctx->plvl--;
		}

		ctx->pcap=opcap;
		bsvmc_pkg2c_pkgname=opkg;
	}

	return(0);
}

void BSVMC_PKG2C_CompileForm(BSVM_SVMCState *ctx, dyt l)
{
	char tb[256];

	dyt c, t, x, d, n, v, u;
	char *s;
	int i, j;

	l=BSVMC_ProcessFlagsExpr(ctx, l, tb);
	if(tb[0])ctx->cs_fl=dysymbol(tb);
		else ctx->cs_fl=NULL;

	if(dyFormIsP(l, "package"))
	{
		BSVMC_PKG2C_CompilePackage(ctx, l);
		return;
	}

	if(dyFormIsP(l, "inline_c"))
	{
		v=dycadr(l);
		if(dystringp(v))
		{
			s=dystringv(v);
			BSVMC_PKG2C_PrintC("%s\n", s);
			return;
		}
		return;
	}

	if(dyFormIsP(l, "unary"))
	{
		return;
	}

	if(BSVMC_FormIs(l, "binary"))
	{
		return;
	}

	if(dyFormIsP(l, "if"))
	{
		return;
	}

	if(dyFormIsP(l, "begin"))
	{
		t=ctx->cb_fl;
		ctx->cb_fl=ctx->cs_fl;
		c=dycdr(l);
		while(dyconsp(c))
		{
			BSVMC_PKG2C_CompileStatement(ctx, dycar(c));
			c=dycdr(c);
		}
		ctx->cb_fl=t;
		return;
	}

	if(BSVMC_FormIs(l, "vars"))
	{
		d=dycadr(l);
		while(dyconsp(d))
		{
			n=dycar(d);
			v=NULL; u=NULL;

			if(dyFormIsP(n, "set!"))
			{
				v=dycaddr(n);
				n=dycadr(n);				
			}
	
			if(dyFormIsP(n, "cast"))
			{
				u=dycadr(n);
				n=dycaddr(n);				
			}

//			BSVMC_CompileExpr(ctx, v);

			s=BSVMC_InferTypeBaseSig(u);
//			u=dysymbol(s);

//			BSVMC_PKG2C_EmitVarNameSig(ctx, dysymbolv(n), s);

//			i=BSVMC_IndexLit(ctx, n);
//			j=BSVMC_IndexLit(ctx, u);
//			k=BSVMC_IndexLit(ctx, fl);
//			BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND);
//			BSVMC_EncIDX(ctx, i);
//			BSVMC_EncIDX(ctx, j);
//			BSVMC_EncIDX(ctx, k);

			d=dycdr(d);
		}
	}

	if(BSVMC_FormIs(l, "define"))
	{
		n=dycaadr(l);
		v=NULL; u=NULL;
	
		if(dyFormIsP(n, "cast"))
		{
			u=dycadr(n);
			n=dycaddr(n);				
		}

//		u=dycons2(dysymbol("lambda"), 
//			dycdadr(t), dycddr(t));
//		BSVMC_CompileExpr(ctx, u);

		s=BSVMC_InferArgsBaseSig(dycadr(l));
//		u=dysymbol(s);

		BSVMC_PKG2C_EmitFuncNameSig(ctx, dysymbolv(n), s);

//		i=BSVMC_IndexLit(ctx, n);
//		j=BSVMC_IndexLit(ctx, u);
//		k=BSVMC_IndexLit(ctx, fl);
//		BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND);
//		BSVMC_EncIDX(ctx, i);
//		BSVMC_EncIDX(ctx, j);
//		BSVMC_EncIDX(ctx, k);
	}

	return;
}

void BSVMC_PKG2C_CompileExpr(BSVM_SVMCState *ctx, dyt l)
{
	char tb[256];
	dyt t, cn, cv, ct;
	char *s, *name;

	if(dyconsp(l))
	{
		BSVMC_PKG2C_CompileForm(ctx, l);
		return;
	}

	if(dysymbolp(l))
	{
	}

	return;
}

void BSVMC_PKG2C_CompileStatement(BSVM_SVMCState *ctx, dyt l)
{
	BSVMC_PKG2C_CompileExpr(ctx, l);
}

BGBSVM_API void BSVM_PKG2C_SetAPI(char *name)
{
	bsvmc_pkg2c_apiname=name;
}

BGBSVM_API void BSVM_PKG2C_SetLibHdr(char *name)
{
	bsvmc_pkg2c_libhdr=name;
}

BGBSVM_API void BSVM_PKG2C_SetOutC(char *name)
{
	bsvmc_pkg2c_outc=fopen(name, "wt");

	BSVMC_PKG2C_PrintC("#include <bgbsvm.h>\n");
	if(bsvmc_pkg2c_libhdr)
		BSVMC_PKG2C_PrintC("#include <%s>\n", bsvmc_pkg2c_libhdr);
	BSVMC_PKG2C_PrintC("\n");

	BSVMC_PKG2C_PrintC(
		"#ifdef __cplusplus\n"
		"extern \"C\" {\n"
		"#endif\n\n");
	
	if(bsvmc_pkg2c_apiname)
	{
		BSVMC_PKG2C_PrintC(
			"#ifdef public\n"
			"#define public %s\n"
			"#endif\n\n", bsvmc_pkg2c_apiname);
	}
}

BGBSVM_API void BSVM_PKG2C_SetOutH(char *name)
{
	bsvmc_pkg2c_outh=fopen(name, "wt");

	BSVMC_PKG2C_PrintH("#include <bgbsvm.h>\n");
	if(bsvmc_pkg2c_libhdr)
		BSVMC_PKG2C_PrintH("#include <%s>\n", bsvmc_pkg2c_libhdr);
	BSVMC_PKG2C_PrintH("\n");

	BSVMC_PKG2C_PrintH(
		"#ifdef __cplusplus\n"
		"extern \"C\" {\n"
		"#endif\n\n");
}

BGBSVM_API void BSVM_PKG2C_SetOutBS(char *name)
{
	bsvmc_pkg2c_outbs=fopen(name, "wt");

//	BSVMC_PKG2C_PrintH("#include <bgbsvm.h>\n");
//	if(bsvmc_pkg2c_libhdr)
//		BSVMC_PKG2C_PrintH("#include <%s>\n", bsvmc_pkg2c_libhdr);
//	BSVMC_PKG2C_PrintH("\n");

//	BSVMC_PKG2C_PrintH(
//		"#ifdef __cplusplus\n"
//		"extern \"C\" {\n"
//		"#endif\n\n");

//	BSVMC_PKG2C_PrintBS(
//		"native package C {\n");
}

BGBSVM_API void BSVM_PKG2C_CloseOutC()
{
	BSVMC_PKG2C_PrintC(
		"\n"
		"#ifdef __cplusplus\n"
		"}\n"
		"#endif\n");
		
	if(bsvmc_pkg2c_outc)
		fclose(bsvmc_pkg2c_outc);
}

BGBSVM_API void BSVM_PKG2C_CloseOutH()
{
	BSVMC_PKG2C_PrintH(
		"\n"
		"#ifdef __cplusplus\n"
		"}\n"
		"#endif\n");

	if(bsvmc_pkg2c_outh)
		fclose(bsvmc_pkg2c_outh);
}

BGBSVM_API void BSVM_PKG2C_CloseOutBS()
{
//	BSVMC_PKG2C_PrintH(
//		"\n"
//		"#ifdef __cplusplus\n"
//		"}\n"
//		"#endif\n");

//	BSVMC_PKG2C_PrintBS("}\n");

	if(bsvmc_pkg2c_outbs)
		fclose(bsvmc_pkg2c_outbs);
}

void BSVM_PKG2C_PrecompileC_EmitToken(char *str, int ty)
{
	char tb[1024];

	if(ty==BSVM_TOKEN_STRING)
	{
		strcpy(tb, BS2ASM_Parse_CIfy(str));
		BSVMC_PKG2C_PrintC("%s", tb);
		return;
	}

	if(ty==BSVM_TOKEN_CHARSTRING)
	{
		BSVMC_PKG2C_PrintC("'%s'", str);
		return;
	}
		
	BSVMC_PKG2C_PrintC("%s", str);
}

char *BSVM_PKG2C_PrecompileC_EmitStrToken(char *str, char *tok, int ty)
{
	char tb[1024];
	char *s, *t;

	if(ty==BSVM_TOKEN_STRING)
	{
		strcpy(tb, BS2ASM_Parse_CIfy(tok));
		strcpy(str, tb);
		str+=strlen(str);
//		BSVMC_PKG2C_PrintC("%s", tb);
		return(str);
	}

	if(ty==BSVM_TOKEN_CHARSTRING)
	{
//		BSVMC_PKG2C_PrintC("'%s'", tok);
		*t++='\'';
		s=tok; t=str;
		while(*s) { *t++=*s++; }
		*t++='\'';
		*t=0;
		return(t);
	}
		
	s=tok; t=str;
	while(*s) { *t++=*s++; }
	*t=0;
	return(t);
}

void BSVM_PKG2C_PrecompileC_BSTop(char **str)
{
	char tbo[4096];
	char tb[1024], tb1[1024], tb2[1024];
	char *s, *t, *s0;
	int ty, ty1, ty2;
	int bl;
	
	s=*str; t=tbo; bl=0;
	while(*s && (bl>=0))
	{
		if(BSVMP_IsWhite(s))
		{
			while(BSVMP_IsWhite(s))
				{ *t++=*s++; }
			continue;
		}
		
		s=BSVMP_Token(s, tb, &ty);
		BSVMP_Token(s, tb2, &ty2);
				
		if(!strcmp(tb, "{") && (ty==BSVM_TOKEN_BRACE))
		{
			t=BSVM_PKG2C_PrecompileC_EmitStrToken(t, tb, ty);
			bl++;
			continue;
		}

		if(!strcmp(tb, "}") && (ty==BSVM_TOKEN_BRACE))
		{
			if(bl>0)
				t=BSVM_PKG2C_PrecompileC_EmitStrToken(t, tb, ty);
			bl--;
			continue;
		}
		
		t=BSVM_PKG2C_PrecompileC_EmitStrToken(t, tb, ty);
	}
	*str=s;
	*t=0;

	BSVMC_PKG2C_PrintBS("%s\n", tbo);
	
	BSVM_PKG2C_CompileScriptFragment(tbo);
}

void BSVM_PKG2C_PrecompileC_BSExpr(char **str)
{
	char tbo[4096], tbo2[4096];
	char tb[1024], tb1[1024], tb2[1024];
	char *lvn[64];
	char *lvt[64];
	char *s, *t, *s0, *s1, *rty;
	int ty, ty1, ty2;
	int bl, nlv;
	int i, j, k;
	
	s=*str; t=tbo; bl=0; nlv=0;
	while(*s && (bl>=0))
	{
		if(BSVMP_IsWhite(s))
		{
			while(BSVMP_IsWhite(s))
			{
				if(*s=='\r') { s++; continue; }
				*t++=*s++;
			}
			continue;
		}
		
		s=BSVMP_Token(s, tb, &ty);
		BSVMP_Token(s, tb2, &ty2);
				
		if(!strcmp(tb, "{") && (ty==BSVM_TOKEN_BRACE))
		{
			t=BSVM_PKG2C_PrecompileC_EmitStrToken(t, tb, ty);
			bl++;
			continue;
		}

		if(!strcmp(tb, "}") && (ty==BSVM_TOKEN_BRACE))
		{
			if(bl>0)
				t=BSVM_PKG2C_PrecompileC_EmitStrToken(t, tb, ty);
			bl--;
			continue;
		}
		
		if(ty==BSVM_TOKEN_NAME)
		{
			s0=BSVM_PKG2C_PrecompileC_FetchVarType(tb);
			if(s0)
			{
				for(i=0; i<nlv; i++)
					if(!strcmp(tb, lvn[i]))
						break;
				if(i>=nlv)
				{
					i=nlv++;
					lvn[i]=dystrsym(tb);
					lvt[i]=dystrsym(s0);
				}
			}

			t=BSVM_PKG2C_PrecompileC_EmitStrToken(t, tb, ty);
			continue;
		}
		
		t=BSVM_PKG2C_PrecompileC_EmitStrToken(t, tb, ty);
	}
	
	rty=NULL;
	if(*s)
	{
		s0=BSVMP_Token(s, tb, &ty);
		s1=BSVMP_Token(s0, tb2, &ty2);
		if(!strcmp(tb, ":") && (ty2==BSVM_TOKEN_NAME))
		{
			rty=dystrsym(tb2);
			s=s1;
		}
	}
	
	*str=s;
	*t=0;

//	s0=BSVMC_GenSym();
	s0=dyllGenSym();

	t=tbo2;
//	sprintf(t, "function %s() { ", s0);
//	t+=strlen(t);

	sprintf(t, "function %s(", s0);
	t+=strlen(t);
	for(i=0; i<nlv; i++)
	{
		sprintf(tb2, "%s:%s", lvn[i], lvt[i]);
		if((i+1)<nlv)strcat(tb2, ", ");
		sprintf(t, "%s", tb2);
		t+=strlen(t);
	}

	if(rty)
	{
		sprintf(t, "):%s { ", rty);
		t+=strlen(t);
	}else
	{
		sprintf(t, ") { ");
		t+=strlen(t);
	}

	sprintf(t, "%s", tbo);
	t+=strlen(t);

	strcpy(t, "}");
	t+=strlen(t);
	
	BSVMC_PKG2C_PrintC("%s(", s0);
	for(i=0; i<nlv; i++)
	{
		BSVMC_PKG2C_PrintC("%s", lvn[i]);
		if((i+1)<nlv)
			BSVMC_PKG2C_PrintC(", ");
	}
	BSVMC_PKG2C_PrintC(")");

	if(BSVMP_IsLineBreak(s) && !rty)
	{
		BSVMC_PKG2C_PrintC(";");

//		BSVMC_PKG2C_PrintC("%s();", s0);
	}else
	{
//		BSVMC_PKG2C_PrintC("%s()", s0);
	}

	BSVMC_PKG2C_PrintBS("%s\n", tbo2);
//	BSVM_PKG2C_CompileScriptFragment(tbo2);
}

char *bsvm_pkg2c_pc_varstk_n[256];
char *bsvm_pkg2c_pc_varstk_t[256];
int bsvm_pkg2c_pc_varstk_pos;

int bsvm_pkg2c_pc_markstk[256];
int bsvm_pkg2c_pc_markstk_pos;

void BSVM_PKG2C_PrecompileC_PushMark()
{
	int i;
	i=bsvm_pkg2c_pc_markstk_pos++;
	bsvm_pkg2c_pc_markstk[i]=
		bsvm_pkg2c_pc_varstk_pos;
}

void BSVM_PKG2C_PrecompileC_PopMark()
{
	int i;
	i=--bsvm_pkg2c_pc_markstk_pos;
	if(i>=0)
	{
		bsvm_pkg2c_pc_varstk_pos=
			bsvm_pkg2c_pc_markstk[i];
	}else
	{
		bsvm_pkg2c_pc_varstk_pos=0;
	}
}

void BSVM_PKG2C_PrecompileC_PushVar(char *name, char *type)
{
	int i;

	i=bsvm_pkg2c_pc_varstk_pos++;
	bsvm_pkg2c_pc_varstk_n[i]=dystrdup(name);
	bsvm_pkg2c_pc_varstk_t[i]=dystrdup(type);
}

char *BSVM_PKG2C_PrecompileC_FetchVarType(char *name)
{
	int i;
	
	for(i=0; i<bsvm_pkg2c_pc_varstk_pos; i++)
	{
		if(!strcmp(bsvm_pkg2c_pc_varstk_n[i], name))
			return(bsvm_pkg2c_pc_varstk_t[i]);
	}
	return(NULL);
}

bool BSVM_PKG2C_PrecompileC_CModifierP(char *tok)
{
	if(!strcmp(tok, "const"))return(true);
	if(!strcmp(tok, "extern"))return(true);
	if(!strcmp(tok, "static"))return(true);
	if(!strcmp(tok, "unsigned"))return(true);
	if(!strcmp(tok, "volatile"))return(true);
	return(false);
}

BGBSVM_API void BSVM_PKG2C_PrecompileC(char *str)
{
	char tb[1024], tb1[1024], tb2[1024];
	char *s, *s0, *s1, *s2;
	int ty, ty1, ty2;
	int bl;
	
	bsvm_pkg2c_pc_varstk_pos=0;
	bsvm_pkg2c_pc_markstk_pos=0;
	
	s=str; bl=0;
	while(*s)
	{
		if(BSVMP_IsLineBreak(s))
		{
//			BSVMC_PKG2C_PrintC("\n");
			while(BSVMP_IsWhite(s))
			{
				if(*s=='\r') { s++; continue; }
				BSVMC_PKG2C_PrintC("%c", *s++);
			}

#if 1
			s0=s;
			BSVMP_Token(s0, tb, &ty);
			while(*s0 && BSVM_PKG2C_PrecompileC_CModifierP(tb))
			{
				s0=BSVMP_Token(s0, tb, &ty);
			}

			s0=BSVMP_Token(s0, tb, &ty);
			s1=BSVMP_Token(s0, tb1, &ty1);
			s2=BSVMP_Token(s1, tb2, &ty2);
			if((ty==BSVM_TOKEN_NAME) &&
				(ty1==BSVM_TOKEN_NAME) &&
					(!strcmp(tb2, ",") ||
					!strcmp(tb2, ";") ||
					!strcmp(tb2, "=")))
			{
				while(*s0)
				{
					s1=BSVMP_Token(s0, tb1, &ty1);
					s2=BSVMP_Token(s1, tb2, &ty2);
					if(ty1!=BSVM_TOKEN_NAME)
						break;

					BSVM_PKG2C_PrecompileC_PushVar(tb1, tb);
					
					if(!strcmp(tb2, ";"))
						break;
					if(!strcmp(tb2, ","))
					{
						s0=s2;
						continue;
					}
					
					break;
				}
			}
#endif
			continue;
		}
		
		if(BSVMP_IsWhite(s))
		{
			while(BSVMP_IsWhite(s))
			{
				if(*s=='\r') { s++; continue; }
				BSVMC_PKG2C_PrintC("%c", *s++);
			}
			continue;
		}
		
		s=BSVMP_Token(s, tb, &ty);
		BSVMP_Token(s, tb2, &ty2);
		
		if(!strcmp(tb, "$") && !strcmp(tb2, "{"))
		{
			s=BSVMP_Token(s, tb2, &ty2);
			
			if(!bl)
			{
				BSVM_PKG2C_PrecompileC_BSTop(&s);
			}else
			{
				BSVM_PKG2C_PrecompileC_BSExpr(&s);
			}
			continue;
		}
		
		if(!strcmp(tb, "{") && (ty==BSVM_TOKEN_BRACE))
		{
			BSVM_PKG2C_PrecompileC_PushMark();
			BSVMC_PKG2C_PrintC("%s", tb);
			bl++; continue;
		}
		
		if(!strcmp(tb, "}") && (ty==BSVM_TOKEN_BRACE))
		{
			BSVM_PKG2C_PrecompileC_PopMark();
			BSVMC_PKG2C_PrintC("%s", tb);
			bl--; continue;
		}
		
		BSVM_PKG2C_PrecompileC_EmitToken(tb, ty);
	}
	
//	BSVMC_PKG2C_PrintC()
	BSVMC_PKG2C_PrintC("\n\n");
}

BGBSVM_API void BSVM_PKG2C_CompileScriptFragment(char *buf)
{
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	dyt c, t, ct, l, f;
	char *s;

	BSVMP_SetLinenum("<pkg2c-temp>", buf, 1);

	s=buf;
//	l=BSVMP_Block(&s);
	l=dytfUnwrap(BSVMP_Block(&s));
//	dyPrint(l);
//	gcfree(buf);

	bsvmc_pkg2c_pkgname=NULL;
//	bsvmc_pkg2c_pkgname="";

	cctx=gcalloc(sizeof(BSVM_SVMCState));

	cctx->lvl=-1;
//	f=BSVMC_CompileBlock(cctx, NULL, NULL, l);

	c=l;
	while(dyconsp(c))
	{
		t=dycar(c);
		BSVMC_PKG2C_CompileStatement(cctx, t);
		c=dycdr(c);
	}

	gcfree(cctx);
}

BGBSVM_API void BSVM_PKG2C_LoadCompileScript(char *name)
{
	char tb[256];
	BSVM_SVMCState *cctx;
	BSVM_SVMState *ctx;
	VFILE *fd;
	byte *tbuf;
	dyt c, t, ct, l, f;
	char *buf;
	char *s, *ext;
	int sz;
	int i;

//	BSVM_Init();
	BSVM_ToolsInit();

	ext=vfgetext(name);

	if(!strcmp(ext, "c"))
	{
		buf=BSVM_ScriptBufferIn(name, &sz);
		if(!buf)return;

		BSVMP_SetLinenum(name, buf, 1);
		BSVM_PKG2C_PrecompileC(buf);
		return;
	}

	if(!strcmp(ext, "bs") ||
		!strcmp(ext, "js") ||
		!strcmp(ext, "es"))
	{
		buf=BSVM_ScriptBufferIn(name, &sz);
		if(!buf)return;

		BSVMP_SetLinenum(name, buf, 1);

		s=buf;
//		l=BSVMP_Block(&s);
		l=dytfUnwrap(BSVMP_Block(&s));
//		dyPrint(l);
		gcfree(buf);

		bsvmc_pkg2c_pkgname=NULL;

		cctx=gcalloc(sizeof(BSVM_SVMCState));

		cctx->lvl=-1;
//		f=BSVMC_CompileBlock(cctx, NULL, NULL, l);

		c=l;
		while(dyconsp(c))
		{
			t=dycar(c);
			BSVMC_PKG2C_CompileStatement(cctx, t);
			c=dycdr(c);
		}

		gcfree(cctx);
	}
}

BGBSVM_API void BSVM_PKG2C_Init()
{
	bsvmc_pkg2c_outbs_buf=malloc(1<<16);
	bsvmc_pkg2c_outbs_ct=bsvmc_pkg2c_outbs_buf;

	BSVMC_PKG2C_PrintBS(
		"native package C {\n");
}

BGBSVM_API void BSVM_PKG2C_Finish()
{
	BSVMC_PKG2C_PrintBS("}\n");

	BSVM_PKG2C_CompileScriptFragment(bsvmc_pkg2c_outbs_buf);

	if(bsvmc_pkg2c_outbs)
	{
		fputs(bsvmc_pkg2c_outbs_buf, bsvmc_pkg2c_outbs);
	}
}
