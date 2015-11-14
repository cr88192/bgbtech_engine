#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbsvm.h>

static int bsvmc_seq=1;

#define bsSet(dst, src)		dst=src

int BSVMC_FormIs(dytf a, char *s)
{
	return(dytfConsP(a) && dytfEqP(dytfCar(a), dytfSymbol(s)));
}

int BSVMC_FormIsBinary(dytf a, char *s)
{
	return(BSVMC_FormIs(a, "binary") &&
		BSVMC_FormIs(dytfCdr(a), s));
}

int BSVMC_FormIsUnary(dytf a, char *s)
{
	return(BSVMC_FormIs(a, "unary") &&
		BSVMC_FormIs(dytfCdr(a), s));
}

int BSVMC_FormCdrIsP(dytf a, char *s)
{
	return(BSVMC_FormIs(dytfCdr(a), s));
}

int BSVMC_SymbolIsP(dytf a, char *s)
{
	return(dytfSymbolP(a) && dytfEqP(a, dytfSymbol(s)));
}

int BSVMC_DebugP(BSVM_SVMCState *ctx)
{
//	return(0);
	return(1);
}

BGBSVM_API void BSVMC_Warning(BSVM_SVMCState *ctx, char *str, ...)
{
	char tb[4096];
	va_list lst;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	
	dyPrintf("warning: %s\n", tb);
}

BGBSVM_API void BSVMC_Error(BSVM_SVMCState *ctx, char *str, ...)
{
	char tb[4096];
	va_list lst;
	
	va_start(lst, str);
	vsprintf(tb, str, lst);
	va_end(lst);
	
	dyPrintf("error: %s\n", tb);
}

BGBSVM_API int BSVMC_CompileSwitch(BSVM_SVMCState *ctx, dytf l)
{
	char **cl, *s, *sdfl;
	byte *ip;
	int i, j, ncl;
	dytf c, t, u, v;

	cl=gcralloc(4096*sizeof(char *));

	ncl=0; c=dytfCddr(l);
	while(dytfConsP(c))
	{
		if(BSVMC_FormIs(dytfCar(c), "case") ||
			BSVMC_FormIs(dytfCar(c), "case_default"))
		{
			cl[ncl++]=BSVMC_GenSym();
			c=dytfCdr(c);
			continue;
		}
		c=dytfCdr(c);
	}

	BSVMC_CompileExpr(ctx, dytfCadr(l));

	i=0; c=dytfCddr(l); sdfl=NULL;
	while(dytfConsP(c))
	{
		if(BSVMC_FormIs(dytfCar(c), "case"))
		{
#if 0
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
			BSVMC_CompileExpr(ctx, dytfCadar(c));

			BSVMC_CompileJmpCond(ctx, BSVM_OPR_EQV, cl[i++]);
#endif

			t=dytfCadar(c);
			t=BSVMC_ReduceExpr(ctx, t);

			if(BSVMC_FormIs(t, "quote"))
			{
				BSVMC_CompileJmpSwitchEqual(ctx, dytfCadr(t), cl[i++]);
			}else if(dytfConsP(t))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
				BSVMC_CompileExpr(ctx, t);
				BSVMC_CompileJmpCond(ctx, BSVM_OPR_EQV, cl[i++]);
			}else
			{
				BSVMC_CompileJmpSwitchEqual(ctx, t, cl[i++]);
			}

			c=dytfCdr(c);
			continue;
		}

		if(BSVMC_FormIs(dytfCar(c), "case_default"))
		{
//			BSVMC_EmitOp(ctx, BSVM_SOP_POP);
//			BSVMC_CompileJmp(ctx, cl[i++]);
			sdfl=cl[i++];
			c=dytfCdr(c);
			continue;
		}
		c=dytfCdr(c);
	}


//	BSVMC_EmitOp(ctx, BSVM_SOP_POP);

	s=BSVMC_GenSym();
	ctx->breakstack[ctx->breakstackpos++]=s;
//	BSVMC_CompileBreak(ctx);

	if(!sdfl)sdfl=s;
	BSVMC_CompileJmpSwitchDefault(ctx, sdfl);

	i=0; c=dytfCddr(l);
	while(dytfConsP(c))
	{
		if(BSVMC_FormIs(dytfCar(c), "case") ||
			BSVMC_FormIs(dytfCar(c), "case_default"))
		{
			BSVMC_EmitLabel(ctx, cl[i++], ctx->ip);
			c=dytfCdr(c);
			continue;
		}

		BSVMC_CompileStatement(ctx, dytfCar(c));
		c=dytfCdr(c);
	}

	BSVMC_EmitLabel(ctx, s, ctx->ip);
//	BSVMC_EmitOp(ctx, BSVM_SOP_POP);

	ctx->breakstackpos--;
	return(0);
}

dytf BSVMC_ProcessFlagsExpr(BSVM_SVMCState *ctx, dytf l, char *tb)
{
	char *s;
	dytf c, c1, c2, v, v1, n;

#if 1
	c=l; s=tb;
	while(1)
	{
		if(BSVMC_FormIs(c, "flag"))
		{
			v=dytfCadr(c);
			c=dytfCaddr(c);
			
			if(dytfEqP(v, dytfSymbol("public")))
				{ *s++='p'; continue; }
			if(dytfEqP(v, dytfSymbol("private")))
				{ *s++='q'; continue; }
			if(dytfEqP(v, dytfSymbol("protected")))
				{ *s++='r'; continue; }
			if(dytfEqP(v, dytfSymbol("static")))
				{ *s++='s'; continue; }
			if(dytfEqP(v, dytfSymbol("native")))
				{ *s++='n'; continue; }
			if(dytfEqP(v, dytfSymbol("abstract")))
				{ *s++='a'; continue; }
			if(dytfEqP(v, dytfSymbol("final")))
				{ *s++='j'; continue; }
			if(dytfEqP(v, dytfSymbol("delegate")))
				{ *s++='d'; continue; }
			if(dytfEqP(v, dytfSymbol("dynamic")))
				{ *s++='x'; continue; }
//			if(dytfEqP(v, dytfSymbol("extern")))
//				{ *s++='x'; continue; }

			if(dytfEqP(v, dytfSymbol("const")))
				{ *s++='k'; continue; }
			if(dytfEqP(v, dytfSymbol("new")))
				{ *s++='k'; continue; }
			if(dytfEqP(v, dytfSymbol("transient")))
				{ *s++='y'; continue; }

			if(dytfEqP(v, dytfSymbol("async")))
				{ *s++='C'; *s++='a'; continue; }
			if(dytfEqP(v, dytfSymbol("typedef")))
				{ *s++='C'; *s++='d'; continue; }
			if(dytfEqP(v, dytfSymbol("extern")))
				{ *s++='C'; *s++='e'; continue; }

			if(dytfEqP(v, dytfSymbol("_setgid")))
				{ *s++='C'; *s++='g'; continue; }
			if(dytfEqP(v, dytfSymbol("_setuid")))
				{ *s++='C'; *s++='h'; continue; }

			if(dytfEqP(v, dytfSymbol("synchronized")))
				{ *s++='C'; *s++='s'; continue; }
			if(dytfEqP(v, dytfSymbol("volatile")))
				{ *s++='C'; *s++='v'; continue; }

			sprintf(s, "u%s;", dytfSymbolv(v));
			s+=strlen(s);
			continue;
		}
		if(BSVMC_FormIs(c, "declspec"))
		{
			c1=dytfCadr(c);
			c=dytfCaddr(c);
			while(dytfConsP(c1))
			{
				v=dytfCar(c1);
				c1=dytfCdr(c1);
				if(dytfSymbolP(v))
				{
					sprintf(s, "Du%s;", dytfSymbolv(v));
					s+=strlen(s);
					continue;
				}

				if(BSVMC_FormIs(v, "funcall"))
				{
					n=dytfCadr(v);
					c2=dytfCddr(v);
					*s++='D'; *s++='(';
					while(dytfConsP(c2))
					{
						v1=dytfCar(c2);
						c2=dytfCdr(c2);

						if(dytfSymbolP(v1))
						{
							sprintf(s, "u%s;", dytfSymbolv(v1));
							s+=strlen(s);
							continue;
						}

						if(dytfIntP(v1))
						{
							sprintf(s, "u%d;", dytfIntv(v1));
							s+=strlen(s);
							continue;
						}
					}
					sprintf(s, ")u%s;", dytfSymbolv(n));
					s+=strlen(s);
				}
			}
			continue;
		}
		break;
	}
	*s++=0;
#endif

	return(c);
}

int BSVMC_CompileEmitVar(BSVM_SVMCState *ctx, dytf l)
{
	s64 li;
	char *s;
	dytf n, v, t, u;

	n=l;
	v=DYTF_NULL; t=DYTF_NULL;

	if(dytfFormIsP(n, "set!"))
	{
		v=dytfCaddr(n);
		n=dytfCadr(n);				
	}

	if(dytfFormIsP(n, "cast"))
	{
		t=dytfCadr(n);
		n=dytfCaddr(n);				
	}

//	dyPrintf("vars: %p %p %p\n", n, t, v);

	if(!dytfNullP(t))
		{ BSVMC_CompileExprCast(ctx, v, t); }
	else
		{ BSVMC_CompileExpr(ctx, v); }

	s=BSVMC_InferTypeBaseSig(t);
	u=dytfSymbol(s);

	if(dytfNullP(u) && !ctx->cs_fl)
	{
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIndexLit(ctx, n);
		return(0);
	}

	li=dyllDecodeFlagSig(dysymbolv(ctx->cs_fl));

	if(li&BGBDYC_FLAG_DYNAMIC)
		{ BSVMC_EmitOp(ctx, BSVM_SOP_TYDYNBIND); }
	else if(ctx->lvl)
		{ BSVMC_EmitOp(ctx, BSVM_SOP_TYLEXBIND);}
	else
		{ BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND); }
	BSVMC_EncIndexLit(ctx, n);
	BSVMC_EncIndexLit(ctx, u);
	BSVMC_EncIndexLit(ctx, dytfWrap(ctx->cs_fl));
	
	return(0);
}

int BSVMC_CompileClass(BSVM_SVMCState *ctx, dytf l)
{
	char tb[256];
	dytf c, d, t, u, v, n, fl;
	char *s;
	int i, j, k, i1, lvl;

#if 0
	BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

	i=BSVMC_IndexLit(ctx, dykeyword("parent"));
	BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
	BSVMC_EncIDX(ctx, i);
//	BSVMC_EmitOp(ctx, BSVM_SOP_PUSHSELF);

	s=dystringv(dytfCaddr(l));
	t=BSVMP_Expression(&s);
	BSVMC_CompileExpr(ctx, t);

	j=ctx->lvl;
	ctx->lvl=0;
	ctx->olvl++;

	c=dytfCaddddr(l);
	while(dytfConsP(c))
	{
		t=dytfCar(c);

		if(BSVMC_FormIs(t, "vars"))
		{
			d=dytfCadr(t);
			while(dytfConsP(d))
			{
				n=dytfCar(d);
				v=DYTF_NULL; u=DYTF_NULL;

				if(dytfFormIsP(n, "set!"))
				{
					v=dytfCaddr(n);
					n=dytfCadr(n);				
				}
	
				if(dytfFormIsP(n, "cast"))
				{
					u=dytfCadr(n);
					n=dytfCaddr(n);				
				}

				i=BSVMC_IndexLit(ctx, n);
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
				BSVMC_EncIDX(ctx, i);
				BSVMC_CompileExpr(ctx, v);

				d=dytfCdr(d);
			}
		}

		if(BSVMC_FormIs(t, "attr") || BSVMC_FormIs(t, "set!"))
		{
			i=BSVMC_IndexLit(ctx, dytfCadr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
			BSVMC_EncIDX(ctx, i);
			BSVMC_CompileExpr(ctx, dytfCaddr(t));
		}

		if(BSVMC_FormIs(t, "define"))
		{
			n=dytfCaadr(t);
			v=DYTF_NULL; u=DYTF_NULL;
	
			if(dytfFormIsP(n, "cast"))
			{
				u=dytfCadr(n);
				n=dytfCaddr(n);				
			}

			i=BSVMC_IndexLit(ctx, n);
//			i=BSVMC_IndexLit(ctx, dytfCaadr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
			BSVMC_EncIDX(ctx, i);
			u=dytfCons2(dytfSymbol("lambda"), 
				dytfCdadr(t), dytfCddr(t));
			BSVMC_CompileExpr(ctx, u);
		}

		c=dytfCdr(c);
	}
	BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

	ctx->olvl--;
	ctx->lvl=j;

	i=BSVMC_IndexLit(ctx, dytfCadr(l));
	if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
		else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
	BSVMC_EncIDX(ctx, i);
#endif

#if 0
	t=dytfCaddr(l);
	if(t)
	{
		s=dystringv(t);
		t=BSVMP_Expression(&s);
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_CLONE);
	}else
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		BSVMC_EmitOp(ctx, BSVM_SOP_DICT);
	}
	BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

	i=BSVMC_IndexLit(ctx, dytfCadr(l));
	if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
		else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
	BSVMC_EncIDX(ctx, i);

//	BSVMC_CompileExpr(ctx, dytfCadr(l));
	BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
//	BSVMC_CompileExpr(ctx, dytfCaddr(l));
//	BSVMC_CompileStatement(ctx, dytfCaddddr(l));

	j=ctx->lvl;
	ctx->lvl=0;
	ctx->olvl++;

	c=dytfCaddddr(l);
	while(dytfConsP(c))
	{
		t=dytfCar(c);
		BSVMC_CompileStatement(ctx, t);
		c=dytfCdr(c);
	}

	ctx->olvl--;
	ctx->lvl=j;

//	BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
	BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
//	BSVMC_EmitOp(ctx, BSVM_SOP_POP);
#endif

#if 1
	i=BSVMC_IndexLit(ctx, dytfCadr(l));
	j=BSVMC_IndexLit(ctx, dytfCaddr(l));
	k=BSVMC_IndexLit(ctx, dytfWrap(ctx->cs_fl));
//	k=0;

	i1=BSVM_SOP_BEGIN_CLASS;
	if(dytfFormIsP(l, "interface"))
		i1=BSVM_SOP_BEGIN_IFACE;
	if(dytfFormIsP(l, "struct"))
		i1=BSVM_SOP_BEGIN_STRUCT;
	if(dytfFormIsP(l, "value_class"))
		i1=BSVM_SOP_BEGIN_STRUCT;

//	BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_CLASS);
	BSVMC_EmitOp(ctx, i1);
	BSVMC_EncIDX(ctx, i);
	BSVMC_EncIDX(ctx, j);
	BSVMC_EncIDX(ctx, k);

	lvl=ctx->lvl;
	ctx->lvl=0;
	ctx->olvl++;

	c=dytfCaddddr(l);
	while(dytfConsP(c))
	{
		t=dytfCar(c);

		t=BSVMC_ProcessFlagsExpr(ctx, t, tb);
		if(tb[0])fl=dytfSymbol(tb);
			else fl=DYTF_NULL;
		ctx->cs_fl=dytfUnwrap(fl);

		if(BSVMC_FormIs(t, "vars"))
		{
			d=dytfCadr(t);
			while(dytfConsP(d))
			{
				n=dytfCar(d);
				v=DYTF_NULL; u=DYTF_NULL;

				if(dytfFormIsP(n, "set!"))
				{
					v=dytfCaddr(n);
					n=dytfCadr(n);				
				}
	
				if(dytfFormIsP(n, "cast"))
				{
					u=dytfCadr(n);
					n=dytfCaddr(n);				
				}

//				i=BSVMC_IndexLit(ctx, n);
//				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//				BSVMC_EncIDX(ctx, i);
//				BSVMC_CompileExpr(ctx, v);

				if(!dytfNullP(u))
					{ BSVMC_CompileExprCast(ctx, v, u); }
				else
					{ BSVMC_CompileExpr(ctx, v); }

				s=BSVMC_InferTypeBaseSig(u);
				u=dytfSymbol(s);

				i=BSVMC_IndexLit(ctx, n);
				j=BSVMC_IndexLit(ctx, u);
				k=BSVMC_IndexLit(ctx, fl);
				BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND);
				BSVMC_EncIDX(ctx, i);
				BSVMC_EncIDX(ctx, j);
				BSVMC_EncIDX(ctx, k);

				d=dytfCdr(d);
			}
		}

		if(BSVMC_FormIs(t, "define"))
		{
			n=dytfCaadr(t);
			v=DYTF_NULL; u=DYTF_NULL;
	
			if(dytfFormIsP(n, "cast"))
			{
				u=dytfCadr(n);
				n=dytfCaddr(n);				
			}

//			i=BSVMC_IndexLit(ctx, n);
//			i=BSVMC_IndexLit(ctx, dytfCaadr(t));
//			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//			BSVMC_EncIDX(ctx, i);

//			u=dytfCons2(dytfSymbol("lambda"), 
//				dytfCdadr(t), dytfCddr(t));
			u=dytfCons3(dytfSymbol("rlambda"), 
				n, dytfCdadr(t), dytfCddr(t));
			BSVMC_CompileExpr(ctx, u);

			s=BSVMC_InferArgsBaseSig(dytfCadr(t));
			u=dytfSymbol(s);

			i=BSVMC_IndexLit(ctx, n);
			j=BSVMC_IndexLit(ctx, u);
			k=BSVMC_IndexLit(ctx, fl);
			BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND);
			BSVMC_EncIDX(ctx, i);
			BSVMC_EncIDX(ctx, j);
			BSVMC_EncIDX(ctx, k);
		}

		if(BSVMC_FormIs(t, "begin"))
		{
//			BSVMC_CompileStatement(ctx, t);

			BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
			u=dytfCons2s("lambda", DYTF_NULL, dytfCdr(t));
			BSVMC_CompileExpr(ctx, u);
			BSVMC_EmitOp(ctx, BSVM_SOP_CALL);
		}

		c=dytfCdr(c);
	}
//	BSVMC_EmitOp(ctx, BSVM_SOP_END_CLASS);

	i=BSVM_SOP_END_CLASS;
	if(dytfFormIsP(l, "interface"))
		i=BSVM_SOP_END_IFACE;
	if(dytfFormIsP(l, "struct"))
		i=BSVM_SOP_END_STRUCT;
	if(dytfFormIsP(l, "value_class"))
		i=BSVM_SOP_END_STRUCT;
	BSVMC_EmitOp(ctx, i);

	ctx->olvl--;
	ctx->lvl=lvl;

//	BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

//	i=BSVMC_IndexLit(ctx, dytfCadr(l));
//	if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
//		else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
//	BSVMC_EncIDX(ctx, i);
#endif

	return(0);
}

int BSVMC_CompilePackage(BSVM_SVMCState *ctx, dytf l)
{
	dytf t;
	int opcap, oplvl;
	int i, j;

	opcap=ctx->pcap;
	oplvl=ctx->plvl++;

	ctx->pcap++;

	i=BSVMC_IndexLit(ctx, dytfCadr(l));
	j=BSVMC_IndexLit(ctx, dytfWrap(ctx->cs_fl));
	BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_PACKAGE);
	BSVMC_EncIDX(ctx, i);
	BSVMC_EncIDX(ctx, j);

	t=dytfCaddr(l);
	if(t)
	{
		BSVMC_CompileStatement(ctx, t);

		while(ctx->plvl>oplvl)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_END_PACKAGE);
			BSVMC_EmitOp(ctx, BSVM_SOP_POP);
			ctx->plvl--;
		}

		ctx->pcap=opcap;
	}

	return(0);
}

void BSVMC_CompileGoto(BSVM_SVMCState *ctx, dytf l)
{
	dytf t;

	if(dytfSymbolP(dytfCadr(l)))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
		BSVMC_EncWord(ctx, 0);
		BSVMC_EmitGoto(ctx,
			dytfSymbolv(dytfCadr(l)), ctx->ip);
		return;
	}

//	t=dytfCadr(l);
	t=BSVMC_ReduceExpr(ctx, dytfCadr(l));

	if(dytfFormIsP(t, "unary"))
	{
		if(dytfEqP(dytfCadr(t), dytfSymbol("*")))
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_JMPIA);
			return;
		}
	}

	BSVMC_CompileExpr(ctx, t);
	BSVMC_EmitOp(ctx, BSVM_SOP_JMPIA);
	return;

//	dyPrintf("BSVMC_CompileGoto: Invalid Expression\n");
}

void BSVMC_CompileBegin(BSVM_SVMCState *ctx, dytf l)
{
	dytf t, c;
	s64 li;

	li=dyllDecodeFlagSig(dysymbolv(ctx->cs_fl));

	if(li&BGBDYC_FLAG_ASYNC)
	{
//		t=dytfCons(dytfSymbol("lambda"), dytfCons(dytfCdadr(l), dytfCddr(l)));
//		BSVMC_CompileExpr(ctx, t);

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		t=dytfCons(dytfSymbol("lambda"), dytfCons(DYTF_NULL, dytfCdr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_ASYNC);
		return;
	}

	if(li&BGBDYC_FLAG_SYNCHRONIZED)
	{
		t=dytfWrap(ctx->cb_fl);
		ctx->cb_fl=ctx->cs_fl;
		c=dytfCdr(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_SYNC);
		while(dytfConsP(c))
		{
			BSVMC_CompileStatement(ctx, dytfCar(c));
			c=dytfCdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_END_SYNC);
		ctx->cb_fl=dytfUnwrap(t);
		return;
	}

	t=dytfWrap(ctx->cb_fl);
	ctx->cb_fl=ctx->cs_fl;
	c=dytfCdr(l);
	while(dytfConsP(c))
	{
		BSVMC_CompileStatement(ctx, dytfCar(c));
		c=dytfCdr(c);
	}
	ctx->cb_fl=dytfUnwrap(t);
	return;
}

void BSVMC_CompileBeginExpr(BSVM_SVMCState *ctx, dytf l)
{
	dytf t, c;
	s64 li;

	li=dyllDecodeFlagSig(dysymbolv(ctx->cs_fl));

	if(li&BGBDYC_FLAG_ASYNC)
	{
//		t=dytfCons(dytfSymbol("lambda"), dytfCons(dytfCdadr(l), dytfCddr(l)));
//		BSVMC_CompileExpr(ctx, t);

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		t=dytfCons(dytfSymbol("lambda"), dytfCons(DYTF_NULL, dytfCdr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_ASYNC);
		return;
	}

	if(li&BGBDYC_FLAG_SYNCHRONIZED)
	{
		t=dytfWrap(ctx->cb_fl);
		ctx->cb_fl=ctx->cs_fl;
		c=dytfCdr(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_SYNC);
		while(dytfConsP(dytfCdr(c)))
//		while(dytfConsP(c))
		{
			BSVMC_CompileStatement(ctx, dytfCar(c));
			c=dytfCdr(c);
		}
//		BSVMC_CompileExprT(ctx, dytfCar(c));
		BSVMC_CompileExpr(ctx, dytfCar(c));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_SYNC);
		ctx->cb_fl=dytfUnwrap(t);
		return;
	}

	t=dytfWrap(ctx->cb_fl);
	ctx->cb_fl=ctx->cs_fl;
	c=dytfCdr(l);
	while(dytfConsP(dytfCdr(c)))
	{
		BSVMC_CompileStatement(ctx, dytfCar(c));
		c=dytfCdr(c);
	}
	BSVMC_CompileExprT(ctx, dytfCar(c));
	ctx->cb_fl=dytfUnwrap(t);
	return;
}

void BSVMC_CompileIfdef(BSVM_SVMCState *ctx, dytf l)
{
	dytf t, n, c;
	int op;

	n=dytfCadr(l);
	if(dytfFormIsP(l, "ifdef"))op=BSVM_SOP_CALL_IFDEF;
	if(dytfFormIsP(l, "ifndef"))op=BSVM_SOP_CALL_IFNDEF;

	BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
	t=dytfCons(dytfSymbol("lambda"), dytfCons(DYTF_NULL, dytfCddr(l)));
	BSVMC_CompileExpr(ctx, t);
	BSVMC_EmitOp(ctx, op);
	BSVMC_EncIndexLit(ctx, n);
	return;
}

void BSVMC_CompileStatement(BSVM_SVMCState *ctx, dytf l)
{
	char tb[256];
	byte *ip0, *ip1;
	dytf c, ct, cv, t, n, u, v, n1;
	char *s, *s1, *s2, *s3, *s4;
	s64 li, lj;
	int i, j, k;

	t=l;
	l=BSVMC_ReduceExpr(ctx, l);

	if(BSVMC_DebugP(ctx))
	{
		dyPrintf("stmt: ");
		dyPrint(dytfUnwrap(t));
		dyPrintf(" -> ");
		dyPrint(dytfUnwrap(l));
		dyPrintf("\n");
	}

	//references/literals are useless in statement position
	if(!dytfConsP(l))
	{
		if(!l)return;	//empty statement

		dyPrintf("useless expression in statement position\n");
		return;
	}

	l=BSVMC_ProcessFlagsExpr(ctx, l, tb);
	if(tb[0])ctx->cs_fl=dysymbol(tb);
		else ctx->cs_fl=NULL;

	if(dytfFormIsP(l, "module"))
	{
//		ctx->mname=dytfCadr(l);
		return;
	}

	if(dytfFormIsP(l, "import"))
	{
		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		j=BSVMC_IndexLit(ctx, ctx->cs_fl);
		BSVMC_EmitOp(ctx, BSVM_SOP_IMPORT);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		ctx->pcap++;

		return;
	}

	if(dytfFormIsP(l, "dbgmark"))
	{
		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		j=dytfIntv(dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DBGMARK);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		return;
	}

	if(dytfFormIsP(l, "linenum"))
	{
		i=dytfIntv(dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_LN);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dytfFormIsP(l, "switch"))
	{
		BSVMC_CompileSwitch(ctx, l);
		return;
	}

	if(dytfFormIsP(l, "class") ||
		dytfFormIsP(l, "interface") ||
		dytfFormIsP(l, "struct"))
	{
		BSVMC_CompileClass(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dytfFormIsP(l, "package"))
	{
		BSVMC_CompilePackage(ctx, l);
//		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dytfFormIsP(l, "goto"))
	{
		BSVMC_CompileGoto(ctx, l);
		return;
	}

	if(dytfFormIsP(l, "label"))
	{
		BSVMC_EmitLabel(ctx,
			dytfSymbolv(dytfCadr(l)), ctx->ip);
		return;
	}

	if(dytfFormIsP(l, "continue"))
	{
		s=ctx->contstack[ctx->contstackpos-1];

		BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
		BSVMC_EncWord(ctx, 0);
		BSVMC_EmitGoto(ctx, s, ctx->ip);
		return;
	}

	if(dytfFormIsP(l, "break"))
	{
		s=ctx->breakstack[ctx->breakstackpos-1];
		BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
		BSVMC_EncWord(ctx, 0);
		BSVMC_EmitGoto(ctx, s, ctx->ip);
		return;
	}

	if(dytfFormIsP(l, "return"))
	{
//		n=BSVMC_ReduceExpr(ctx, dytfCadr(l));
//		t=BSVMC_InferExpr(ctx, n);

//		BSVMC_CompileExpr(ctx, n);
		BSVMC_CompileExprCast(ctx, dytfCadr(l), dytfWrap(ctx->cf_ty));

//		BSVMC_CompileExpr(ctx, dytfCadr(l));
		if(!(ctx->i_cap))
			BSVMC_EmitOp(ctx, BSVM_SOP_CLEARENV);

//#ifdef BSVM_USE_BVT
#if 0
		if(BSVMC_TypeSmallIntP(ctx, ctx->cf_ty))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
		if(BSVMC_TypeLongP(ctx, ctx->cf_ty))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XL);
		if(BSVMC_TypeFloatP(ctx, ctx->cf_ty))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XF);
		if(BSVMC_TypeDoubleP(ctx, ctx->cf_ty))
			BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XD);
#endif

		BSVMC_EmitOp(ctx, BSVM_SOP_RET);
		return;
	}

	if(dytfFormIsP(l, "set!"))
	{
		t=dytfCadr(l);
		c=ctx->lenv; ct=ctx->tenv; cv=ctx->venv;
		i=0; j=0;
		while(dytfConsP(c))
		{
			if(c==ctx->llenv)i=1;
			if(dytfCar(c)==t)break;
			c=dytfCdr(c); ct=dytfCdr(ct); cv=dytfCdr(cv);
			j++;
		}
		if(dytfConsP(c) && i)ctx->cap++;

		if(dytfConsP(c))
		{
			t=BSVMC_InferExpr(ctx, dytfCaddr(l));
			n=dytfCar(ct);

#if 0
			if((n!=DYTF_NULL) && (t==DYTF_NULL))
			{
				i=BSVMC_IndexLit(ctx, n);
				BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
				BSVMC_EncIDX(ctx, i);
				t=n;
			}

			if((n!=DYTF_NULL) && (t!=n))
			{
				dyPrintf("warn: inferencer violation\n");
				dysetcar(ct, NULL);
			}
#endif

			dysetcar(cv, NULL);

#ifdef BSVM_USE_BVT
			i=-1;
			if(BSVMC_TypeSmallIntP(ctx, n))
				i=BSVM_SOP_LSTORE_XI;
			if(BSVMC_TypeLongP(ctx, n))
				i=BSVM_SOP_LSTORE_XL;
			if(BSVMC_TypeFloatP(ctx, n))
				i=BSVM_SOP_LSTORE_XF;
			if(BSVMC_TypeDoubleP(ctx, n))
				i=BSVM_SOP_LSTORE_XD;

			if(i>=0)
			{
				BSVMC_CompileExprCast(ctx, dytfCaddr(l), n);
				BSVMC_EmitOp(ctx, i);
				BSVMC_EncByte(ctx, j);
				return;
			}
#endif

//			if((t==dykeyword("int")) || (t==dykeyword("float")))
			if(BSVMC_TypeSmallFixRealP(ctx, t))
			{
				BSVMC_CompileExpr(ctx, dytfCaddr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_LSTORE_F);
				BSVMC_EncByte(ctx, j);
				return;
			}

			BSVMC_CompileExpr(ctx, dytfCaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_LSTORE);
			BSVMC_EncByte(ctx, j);
			return;
		}


		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		i=BSVMC_IndexLit(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STORE);
		BSVMC_EncIDX(ctx, i);

		return;
	}

	if((dytfFormIsP(l, "setindex!")) || (dytfFormIsP(l, "vector-set!")))
	{
		BSVMC_CompileSetIndexStmt(ctx, l);
		return;

#if 0
		BSVMC_CompileExpr(ctx, dytfCadddr(l));
//		BSVMC_CompileExpr(ctx, dytfCaddr(l));
//		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_CompileExpr(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
		return;
#endif
	}

	if(dytfFormIsP(l, "preinc!") || dytfFormIsP(l, "predec!") ||
		dytfFormIsP(l, "postinc!") || dytfFormIsP(l, "postdec!"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		u=BSVMC_InferExpr(ctx, t);

		i=BSVM_OPR_DEC;
		if(dytfFormIsP(l, "preinc!") || dytfFormIsP(l, "postinc!"))
			i=BSVM_OPR_INC;

//		if(dytfSymbolP(t) && (u==dykeyword("int")))
		if(dytfSymbolP(t))
		{
			c=ctx->lenv; ct=ctx->tenv; cv=ctx->venv;
			k=0; j=0;
			while(dytfConsP(c))
			{
				if(c==ctx->llenv)k=1;
				if(dytfCar(c)==t)break;
				c=dytfCdr(c); ct=dytfCdr(ct); cv=dytfCdr(cv);
				j++;
			}

			if(dytfConsP(c))
			{
				t=dytfCar(ct);
				dysetcar(cv, NULL);

				if(k)ctx->cap++;

#ifdef BSVM_USE_BVT
				if(BSVMC_TypeSmallIntP(ctx, t))
					BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XI);
				if(BSVMC_TypeLongP(ctx, t))
					BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XL);
				if(BSVMC_TypeFloatP(ctx, t))
					BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XF);
				if(BSVMC_TypeDoubleP(ctx, t))
					BSVMC_EmitOp(ctx, BSVM_SOP_PF_HINT_XD);
#endif

//				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_LINC_FN);
//					else BSVMC_EmitOp(ctx, BSVM_SOP_LDEC_FN);
				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_LINC);
					else BSVMC_EmitOp(ctx, BSVM_SOP_LDEC);
//				BSVMC_EncByte(ctx, j);
				BSVMC_EncIDX(ctx, j);
				return;
			}
		}

		if(dytfSymbolP(t))
		{
			if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC_S);
				else BSVMC_EmitOp(ctx, BSVM_SOP_DEC_S);
			BSVMC_EncIndexLit(ctx, t);
			return;
		}

#if 0
		if(dytfSymbolP(t))
		{
			BSVMC_CompileExpr(ctx, t);

			if(u==dykeyword("int"))
			{
				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC_FN);
					else BSVMC_EmitOp(ctx, BSVM_SOP_DEC_FN);
			}else
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
				BSVMC_EncByte(ctx, i);
			}

			BSVMC_CompileAssign(ctx, t);
			return;
		}
#endif

		if(1)
		{
			BSVMC_CompileExpr(ctx, t);

			if(u==dykeyword("int"))
			{
				if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC_FN);
					else BSVMC_EmitOp(ctx, BSVM_SOP_DEC_FN);
			}else
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_UNARYOP);
				BSVMC_EncByte(ctx, i);
			}

			BSVMC_CompileAssign(ctx, t);
			return;
		}
		return;
	}

	if(dytfFormIsP(l, "define"))
	{
		if(!dytfConsP(dytfCadr(l)))
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(l));
			i=BSVMC_IndexLit(ctx, dytfCadr(l));
			if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
				else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		n=dytfCaadr(l);
//		if(dytfFormIsP(n, "cast"))
//			n=dytfCaddr(n);

//		t=dytfCons(dytfSymbol("lambda"), dytfCons(dytfCdadr(l), dytfCddr(l)));
		t=dytfCons3s("rlambda", n, dytfCdadr(l), dytfCddr(l));
		BSVMC_CompileExpr(ctx, t);
//		n=dytfCaadr(l);
		if(dytfFormIsP(n, "cast"))
			n=dytfCaddr(n);

#if 1
		s=BSVMC_InferArgsBaseSig(dytfCadr(l));
		u=dytfSymbol(s);

		i=BSVMC_IndexLit(ctx, n);
		j=BSVMC_IndexLit(ctx, u);
		k=BSVMC_IndexLit(ctx, ctx->cs_fl);
		if(ctx->lvl)
			{ BSVMC_EmitOp(ctx, BSVM_SOP_TYLEXBIND); }
		else
			{ BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND); }
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		BSVMC_EncIDX(ctx, k);
#endif

		return;
	}

#if 0
	if(dytfFormIsP(l, "defvar"))
	{
		if(!dytfConsP(dytfCadr(l)))
		{
			BSVMC_CompileExpr(ctx, dytfCaddr(l));
			i=BSVMC_IndexLit(ctx, dytfCadr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		t=dytfCons(dytfSymbol("lambda"), dytfCons(dytfCdadr(l), dytfCddr(l)));
		BSVMC_CompileExpr(ctx, t);
		i=BSVMC_IndexLit(ctx, dytfCaadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}
#endif

	if(dytfFormIsP(l, "vars"))
	{
//		c=dytfCaddr(l);
//		t=dytfCadr(l);

		c=dytfCadr(l);
		t=DYTF_NULL;

		while(dytfConsP(c))
		{
			n=dytfCar(c);
			v=DYTF_NULL; t=DYTF_NULL;

			if(dytfFormIsP(n, "set!"))
			{
				v=dytfCaddr(n);
				n=dytfCadr(n);				
			}

			if(dytfFormIsP(n, "cast"))
			{
				t=dytfCadr(n);
				n=dytfCaddr(n);				
			}

			BSVMC_CompileEmitVar(ctx, dytfCar(c));

			if(ctx->lvl)
			{
				n1=DYTF_NULL;
				if(!BSVMC_IsExpr(v))n1=v;

				bsSet(ctx->lenv, dytfCons(n, ctx->lenv));
				bsSet(ctx->tenv, dytfCons(t, ctx->tenv));
				bsSet(ctx->venv, dytfCons(n1, ctx->venv));
			}else if(!ctx->olvl)
			{
				n1=DYTF_NULL;
				if(!BSVMC_IsExpr(v))n1=v;

				bsSet(ctx->mlenv, dytfCons(n, ctx->mlenv));
				bsSet(ctx->mtenv, dytfCons(t, ctx->mtenv));
				bsSet(ctx->mvenv, dytfCons(n1, ctx->mvenv));
			}

			c=dytfCdr(c);
		}
		return;
	}

#if 0
	if(dytfFormIsP(l, "defun"))
	{
		if(!ctx->lvl && !ctx->olvl)
		{
			t=BSVMC_CompileBlock(ctx,
				dytfList2(dytfCadr(l), dytfCaddr(l)),
				dytfCadddr(l), dytfCddddr(l));

			i=BSVMC_IndexLit(ctx, t);
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
			BSVMC_EncIDX(ctx, i);

			bsSet(ctx->mfcns, dytfCons(t, ctx->mfcns));

			i=BSVMC_IndexLit(ctx, dytfCaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIDX(ctx, i);

			return;
		}


//		t=CONS2S("lambda", dytfCadddr(l), dytfCddddr(l));

		t=dytfCons3s("rlambda", dytfList2(dytfCadr(l), dytfCaddr(l)),
			dytfCadddr(l), dytfCddddr(l));

//		BSVM_TY_PrintLN(t);

		BSVMC_CompileExpr(ctx, t);
		i=BSVMC_IndexLit(ctx, dytfCaddr(l));
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}
#endif

	if(dytfFormIsP(l, "if"))
	{
		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));

#if 0
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			BSVMC_CompileStatement(ctx, dytfCaddr(l));
			return;
		}
		if(i==0)
		{
			if(dytfConsP(dytfCdddr(l)))
				BSVMC_CompileStatement(ctx, dytfCadddr(l));
			return;
		}
#endif

		dyPrintf("if: ");
		dyPrint(t);
		dyPrintf("\n");

		BSVMC_CompileJCF(ctx, t);
		ip0=ctx->ip;


		BSVMC_CompileStatement(ctx, dytfCaddr(l));

		if(dytfConsP(dytfCdddr(l)))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
			BSVMC_EncWord(ctx, 0);
			ip1=ctx->ip;

			i=ctx->ip-ip0;
			*(ip0-2)=i&0xFF;
			*(ip0-1)=(i>>8)&0xFF;

			BSVMC_CompileStatement(ctx, dytfCadddr(l));
			ip0=ip1;
		}
		i=ctx->ip-ip0;
		*(ip0-2)=i&0xFF;
		*(ip0-1)=(i>>8)&0xFF;
		return;
	}

	if(dytfFormIsP(l, "begin"))
	{
		BSVMC_CompileBegin(ctx, l);
		return;
	}

	if(dytfFormIsP(l, "synchronized"))
	{
		BSVMC_CompileExpr(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_SYNC_OBJ);
		BSVMC_CompileStatement(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_SYNC_OBJ);
		return;
	}

	if(dytfFormIsP(l, "throw"))
	{
		t=dytfCadr(l);
		
		if(dytfSymbolP(t))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_THROW_S);
			BSVMC_EncIndexLit(ctx, t);
			return;
		}
		
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_THROW_OBJ);
		return;
	}

	if(dytfFormIsP(l, "try"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_TRY);
		BSVMC_CompileStatement(ctx, dytfCadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_TRY);
		return;
	}

	if(dytfFormIsP(l, "catch"))
	{
		s1=BSVMC_GenSym();
		t=dytfCadr(l); n=DYTF_NULL;

		if(dytfFormIsP(t, "cast"))
		{
			n=dytfCaddr(t);
			t=dytfCadr(t);
		}

		s=BSVMC_InferArgsBaseSig(t);
		u=dytfSymbol(s);

		BSVMC_EmitOp(ctx, BSVM_SOP_CATCH);
		BSVMC_EncIndexLit(ctx, u);
		BSVMC_EmitJmpAddr(ctx, s1);

		BSVMC_CompileStatement(ctx, dytfCaddr(l));

		BSVMC_EmitLabel(ctx, s1, ctx->ip);
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_CATCH);
		BSVMC_EncIndexLit(ctx, u);
		
		if(n)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND);
			BSVMC_EncIndexLit(ctx, n);
			BSVMC_EncIndexLit(ctx, u);
			BSVMC_EncIndexLit(ctx, NULL);
		}else
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		}
		
		BSVMC_CompileStatement(ctx, dytfCadddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_CATCH);
		return;
	}

	if(dytfFormIsP(l, "finally"))
	{
		s1=BSVMC_GenSym();

		BSVMC_EmitOp(ctx, BSVM_SOP_CATCH);
		BSVMC_EncIndexLit(ctx, NULL);
		BSVMC_EmitJmpAddr(ctx, s1);

		BSVMC_CompileStatement(ctx, dytfCadr(l));

		BSVMC_EmitLabel(ctx, s1, ctx->ip);
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_CATCH_FINAL);
//		BSVMC_EncIndexLit(ctx, NULL);
		BSVMC_CompileStatement(ctx, dytfCaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_CATCH);
		return;
	}

	if(dytfFormIsP(l, "ifdef") ||
		dytfFormIsP(l, "ifndef"))
	{
		BSVMC_CompileIfdef(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dytfFormIsP(l, "for"))
	{
		s1=BSVMC_GenSym();
		s2=BSVMC_GenSym();
		s3=BSVMC_GenSym();

		ctx->contstack[ctx->contstackpos++]=s1;
		ctx->breakstack[ctx->breakstackpos++]=s2;

		BSVMC_CompileStatement(ctx, dytfCadr(l));

//		BSVMC_EmitLabel(ctx, s1, ctx->ip);
		BSVMC_EmitLabel(ctx, s3, ctx->ip);

//		BSVMC_CompileExpr(ctx, dytfCaddr(l));
//		BSVMC_CompileBreakFalse(ctx);

		BSVMC_CompileJCF(ctx, dytfCaddr(l));
		BSVMC_EmitGoto(ctx, s2, ctx->ip);

		BSVMC_CompileStatement(ctx, dytfCaddddr(l));

		BSVMC_EmitLabel(ctx, s1, ctx->ip);
		BSVMC_CompileStatement(ctx, dytfCadddr(l));
//		BSVMC_CompileContinue(ctx);
		BSVMC_EmitJmp(ctx, s3);

		BSVMC_EmitLabel(ctx, s2, ctx->ip);

		ctx->contstackpos--;
		ctx->breakstackpos--;
		return;
	}

	if(dytfFormIsP(l, "while"))
	{
		s1=BSVMC_GenSym();
		s2=BSVMC_GenSym();

		ctx->contstack[ctx->contstackpos++]=s1;
		ctx->breakstack[ctx->breakstackpos++]=s2;

		BSVMC_EmitLabel(ctx, s1, ctx->ip);

		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		i=BSVMC_BoolExpr(ctx, t);

		if(i==0)return;

		if(i==1)
		{
			BSVMC_CompileStatement(ctx, dytfCaddr(l));
			BSVMC_CompileContinue(ctx);

			BSVMC_EmitLabel(ctx, s2, ctx->ip);

			ctx->contstackpos--;
			ctx->breakstackpos--;
			return;
		}

//		BSVMC_CompileExpr(ctx, t);
//		BSVMC_CompileBreakFalse(ctx);

		BSVMC_CompileJCF(ctx, t);
		BSVMC_EmitGoto(ctx, s2, ctx->ip);

		BSVMC_CompileStatement(ctx, dytfCaddr(l));

		BSVMC_CompileContinue(ctx);
		BSVMC_EmitLabel(ctx, s2, ctx->ip);

		ctx->contstackpos--;
		ctx->breakstackpos--;
		return;
	}


	if(dytfFormIsP(l, "do_while"))
	{
		s1=BSVMC_GenSym();
		s2=BSVMC_GenSym();
		ctx->contstack[ctx->contstackpos++]=s1;
		ctx->breakstack[ctx->breakstackpos++]=s2;

		BSVMC_EmitLabel(ctx, s1, ctx->ip);

		BSVMC_CompileStatement(ctx, dytfCaddr(l));

		t=BSVMC_ReduceExpr(ctx, dytfCadr(l));
		i=BSVMC_BoolExpr(ctx, t);
		if(i==0)return;

		if(i==1)
		{
			BSVMC_CompileContinue(ctx);
			BSVMC_EmitLabel(ctx, s2, ctx->ip);
			ctx->contstackpos--;
			ctx->breakstackpos--;
			return;
		}

//		BSVMC_CompileExpr(ctx, t);
//		BSVMC_CompileContinueTrue(ctx);

		BSVMC_CompileJCT(ctx, t);
		BSVMC_EmitGoto(ctx, s1, ctx->ip);

		BSVMC_EmitLabel(ctx, s2, ctx->ip);

		ctx->contstackpos--;
		ctx->breakstackpos--;
		return;
	}

	if(dytfFormIsP(l, "inline_c"))
	{
		t=dytfCadr(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_CGENAST);
		BSVMC_EncIndexLit(ctx, t);
		return;
	}


	BSVMC_CompileExpr(ctx, l);
	BSVMC_EmitOp(ctx, BSVM_SOP_POP);
}

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];

byte *BSVMC_NextOpcode(byte *ip)
{
	char *s;
	int i, j, k;

	i=*ip++;
	if(i>=192)i=(i-192)*256+(*ip++);

	s=bsvm_ops_args[i];
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N'))
			{ j=*ip++; if(j&128)ip++; s++; continue; }
		if(*s=='A') { ip+=2; s++; continue; }

		if(*s=='C') { ip++; s++; continue; }
		if(*s=='B') { ip++; s++; continue; }
		if(*s=='W') { ip++; s++; continue; }
		if(*s=='H') { ip++; s++; continue; }

		if(*s=='u') { ip++; s++; continue; }
		if(*s=='b') { ip++; s++; continue; }

		s++;
	}

	return(ip);
}

byte *BSVMC_DumpOpcode(byte *ip, dytf lit)
	{ return(BSVMC_DumpOpcodeFl(ip, lit, 0)); }

byte *BSVMC_DumpOpcodeFl(byte *ip, dytf lit, int fl)
{
//	char *uops[]={"neg", "not", "lnot"};
	char *uops[]={"neg", "not", "lnot", "pos", "umul", "udiv", "uidiv",
		"umod", "uand", "uor", "uxor", "uland", "ulor", "ulxor", "inc",
		"dec", "defer", "car", "cdr",
		"copyvalue", "dropvalue", "delete", "clone",
		"sin", "cos", "tan", "asin", "acos", "atan", "sqr", "sqrt"};
	char *bops[]={"add", "sub", "mul", "div", "mod", "and", "or",
		"xor", "shl", "shr", "eq", "eqv", "neq", "neqv", "l",
		"g", "le", "ge", "idiv", "dadd", "dsub", "dmul", "ddiv",
		"dmod", "dand", "dor", "dxor", "didiv", "exp", "shrr",
		"cons", "atan2"};

	dytf t;
	byte *oip, *ip1, *ip2;
	char *s;
	int i, j, k, l;

	oip=ip;
	i=*ip++;
	if(i>=240)
	{
		i=(i-240)*256+(*ip++);
		i=(i*256)+(*ip++);
		dyPrintf("%08X %s%02X%02X%02X ", oip,
			(fl&1)?"  ":"", oip[0], oip[1], oip[2]);
	}else
	if(i>=192)
	{
		i=(i-192)*256+(*ip++);
		dyPrintf("%08X %s%02X%02X ", oip, (fl&1)?"  ":"", oip[0], oip[1]);
	}else
	{
		dyPrintf("%08X %s%02X ", oip, (fl&1)?"  ":"", i);
	}

	k=0; s=bsvm_ops_args[i]; ip1=ip;
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N') || (*s=='J'))
		{
			j=*ip1++; k++;
			if(j&128) { ip1++; k++; }
			s++; continue;
		}
		if(*s=='A') { ip1+=2; k+=2; s++; continue; }

		if(*s=='C') { ip1++; k++; s++; continue; }
		if(*s=='B') { ip1++; k++; s++; continue; }
		if(*s=='W') { ip1+=2; k+=2; s++; continue; }
		if(*s=='K') { ip1+=2; k+=2; s++; continue; }
		if(*s=='H') { ip1+=2; k+=2; s++; continue; }
		if(*s=='G') { ip1+=2; k+=2; s++; continue; }

		if(*s=='u') { ip1++; k++; s++; continue; }
		if(*s=='b') { ip1++; k++; s++; continue; }

		if((*s=='U') || (*s=='V'))
		{
			ip2=ip1; BS2ASM_DecVLI(&ip2); l=ip2-ip1;
			ip1+=l; k+=l; s++; continue;
		}

		if(*s=='D')
		{
			ip2=ip1; BS2ASM_DecVLI(&ip2);
			BS2ASM_DecVLI(&ip2); l=ip2-ip1;
			ip1+=l; k+=l; s++; continue;
		}

		s++;
	}

	ip1=ip;
	for(j=0; j<k; j++)
		dyPrintf("%02X", *ip1++);

	if(i<12288)
		dyPrintf("  ");
	if(i<192)
		dyPrintf("  ");
	for(; j<4; j++)
		dyPrintf("  ");

	dyPrintf(" %s ", bsvm_ops_strs[i]);

	s=bsvm_ops_args[i];
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			t=dyarrayidx(lit, i);
			dyPrint(t);
			dyPrintf(" ");
			s++;
			continue;
		}
		if((*s=='N') || (*s=='J'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			dyPrintf("%d ", i);
			s++;
			continue;
		}
		if(*s=='A')
		{
			i=*ip++; i|=(*ip++)<<8;
			dyPrintf("%08X ", ip+i);
			s++;
			continue;
		}
		if(*s=='C') { i=(signed char)(*ip++); dyPrintf("%d ", i);
			s++; continue; }
		if(*s=='B') { i=*ip++; dyPrintf("%d ", i); s++; continue; }
		if(*s=='W') { i=*ip++; i|=(*ip++)<<8; dyPrintf("%d ", i);
			s++; continue; }
		if(*s=='K') { i=*ip++; i|=(*ip++)<<8;
			i=(short)i; dyPrintf("%d ", i);
			s++; continue; }
		if(*s=='H') { i=*ip++; i|=(*ip++)<<8;
			i=(short)i; dyPrintf("%g ", i/100.0);
			s++; continue; }
		if(*s=='G') { i=*ip++; i|=(*ip++)<<8;
			i=(short)i; dyPrintf("%g ", i/10000.0);
			s++; continue; }

		if(*s=='U')
		{
			i=BS2ASM_DecVLI(&ip);
			dyPrintf("%d ", i);
			s++; continue;
		}
		if(*s=='V')
		{
			i=BS2ASM_DecSVLI(&ip);
			dyPrintf("%d ", i);
			s++; continue;
		}

		if(*s=='D')
		{
			i=BS2ASM_DecSVLI(&ip);
			j=BS2ASM_DecSVLI(&ip);
			dyPrintf("%f ", i*pow(2, j));
			s++; continue;
		}

		if(*s=='u') { i=*ip++; dyPrintf("%s ", uops[i]); s++; continue; }
		if(*s=='b') { i=*ip++; dyPrintf("%s ", bops[i]); s++; continue; }
		s++;
	}

	dyPrintf("\n");

	return(ip);
}

byte *BSVMC_DumpOpcodeList(byte *ips, byte *ipe, dytf lit)
{
	byte *ip;
	char *s;
	int i, j;

	ip=ips; i=0;
	while(ip<ipe)
	{
		j=ip[0];
		if(j>=192)j=(j-192)*256+(ip[1]);

		ip=BSVMC_DumpOpcodeFl(ip, lit, i);

		i=0;
		s=bsvm_ops_strs[j];
		if(s && !strncmp(s, "pf_", 3))
			i|=1;
	}
	return(ip);
}

dytf BSVMC_CompileBlock(BSVM_SVMCState *lctx,
	dytf name, dytf args, dytf body)
{
	return(BSVMC_CompileBlockEnv(lctx, lctx->lenv, name, args, body));
}

dytf BSVMC_CompileBlockEnv(BSVM_SVMCState *lctx,
	dytf lenv, dytf name, dytf args, dytf body)
{
	BSVM_SVMBlock *blk;
	BSVM_SVMCState *ctx;

	char **gname, **lname;
	byte **gdest, **ldest;
	char **cname, **bname;

	char *sig;

	dytf *lit;
	byte *ip, *cip, *ipe;
	s64 li;

	dytf fargs, blkn;
	dytf c, t, u, v, n;
	int i, j, k;

	li=dyllDecodeFlagSig(dytfSymbolv(lctx->cs_fl));

	ctx=gcalloc(sizeof(BSVM_SVMCState));
	memset(ctx, 0, sizeof(BSVM_SVMCState));

	gname=gcalloc(4096*sizeof(char *));
	gdest=gcalloc(4096*sizeof(byte *));
	lname=gcalloc(256*sizeof(char *));
	ldest=gcalloc(256*sizeof(byte *));

	cname=gcalloc(16*sizeof(char *));
	bname=gcalloc(16*sizeof(char *));

	ctx->goto_name=gname;
	ctx->goto_dest=gdest;
	ctx->lbl_name=lname;
	ctx->lbl_dest=ldest;

	ctx->contstack=cname;
	ctx->breakstack=bname;

	bsSet(ctx->lenv, lctx->lenv);
	bsSet(ctx->llenv, ctx->lenv);
	bsSet(ctx->tenv, lctx->tenv);
	bsSet(ctx->ltenv, ctx->tenv);
	bsSet(ctx->venv, lctx->venv);
	bsSet(ctx->lvenv, ctx->venv);

	if(dytfFormIsP(name, "cast"))
	{
		ctx->cf_ty=dytfCadr(name);
		name=dytfCaddr(name);
		ctx->cf_n=name;
	}

//	if(dytfConsP(name))
//	{
//		ctx->cf_n=dytfCadr(name);
//		ctx->cf_ty=dytfCar(name);
//	}
	
	ctx->cf_n=name;
	blkn=ctx->cf_n;

	ctx->cf_fl=lctx->cs_fl;
	ctx->access=lctx->access;


	c=lenv; u=DYTF_NULL;
	while(dytfConsP(c))
	{
		n=dytfCar(c);
		t=DYTF_NULL;
		v=DYTF_NULL;

		if(dytfFormIsP(n, "set!"))
		{
			v=dytfCaddr(n);
			n=dytfCadr(n);
		}

		if(dytfFormIsP(n, "cast"))
		{
			t=dytfCadr(n);
			n=dytfCaddr(n);
		}

		bsSet(ctx->lenv, dytfCons(n, ctx->lenv));
		bsSet(ctx->tenv, dytfCons(t, ctx->tenv));
		bsSet(ctx->venv, dytfCons(v, ctx->venv));

		n=BSVMC_BuildVarInfo(dytfCar(c));

		u=dytfCons(n, u);
		c=dytfCdr(c);
	}

	lenv=dynreverse(u);


	c=args; u=DYTF_NULL;
	while(dytfConsP(c))
	{
		n=dytfCar(c);
		t=DYTF_NULL;
		v=DYTF_NULL;

		if(dytfFormIsP(n, "rest"))
		{
//			v=dytfCaddr(n);
			n=dytfCadr(n);
		}

		if(dytfFormIsP(n, "set!"))
		{
			v=dytfCaddr(n);
			n=dytfCadr(n);
		}

		if(dytfFormIsP(n, "cast"))
		{
			t=dytfCadr(n);
			n=dytfCaddr(n);
		}

		bsSet(ctx->lenv, dytfCons(n, ctx->lenv));
		bsSet(ctx->tenv, dytfCons(t, ctx->tenv));
		bsSet(ctx->venv, dytfCons(v, ctx->venv));

#if 0
		sig=BSVMC_InferTypeBaseSig(t);
		if(sig && *sig && strcmp(sig, "r"))
		{
			c=dytfCdr(c);
			continue;
		}
#endif

		n=BSVMC_BuildVarInfo(dytfCar(c));

		u=dytfCons(n, u);
		c=dytfCdr(c);

#if 0
		t=dytfCar(c);
		if(dytfConsP(t))
		{
			bsSet(ctx->lenv, dytfCons(dytfCadr(t), ctx->lenv));
			bsSet(ctx->tenv, dytfCons(dytfCar(t), ctx->tenv));
			bsSet(ctx->venv, dytfCons(DYTF_NULL, ctx->venv));
			u=dytfCons(dytfCadr(t), u);
		}else
		{
			bsSet(ctx->lenv, dytfCons(dytfCar(c), ctx->lenv));
			bsSet(ctx->tenv, dytfCons(DYTF_NULL, ctx->tenv));
			bsSet(ctx->venv, dytfCons(DYTF_NULL, ctx->venv));
			u=dytfCons(t, u);
		}
		c=dytfCdr(c);
#endif
	}

	if(c)
	{
		ctx->lenv=dytfCons(c, ctx->lenv);
		fargs=DYTF_NULL;
		ctx->i_cap=1;
	}else
	{
//		fargs=BSVM_TY_NReverse(u);
		fargs=dynreverse(u);
		ctx->i_cap=BSVMC_GuessCloseBlock(ctx, body);
	}

	body=BSVMC_ReverseStepBlock(ctx, body);

	lit=gcalloc(4096*sizeof(dyt));
	ip=gcatomic(65536);

	ctx->lvl=lctx->lvl+1;
	ctx->lit=lit;
	ctx->ip=ip;

	if(dytfConsP(body))
	{
		c=body;
		while(dytfConsP(c))
		{
			if(dytfConsP(dytfCdr(c)))
			{
				t=BSVMC_ReverseStepBlock(ctx, dytfCdr(c));
				BSVMC_CompileStatement(ctx, dytfCar(c));
				c=t;
				continue;
			}else
			{
				BSVMC_CompileExprTail(ctx, dytfCar(c));
				break;
			}
		}

		//incomplete packages
		while(ctx->plvl>0)
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_END_PACKAGE);
			BSVMC_EmitOp(ctx, BSVM_SOP_POP);
			ctx->plvl--;
		}

		if(!(ctx->i_cap))
			BSVMC_EmitOp(ctx, BSVM_SOP_CLEARENV);
		BSVMC_EmitOp(ctx, BSVM_SOP_RET);
	}else
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_PUSH_NULL);
		if(!(ctx->i_cap))
			BSVMC_EmitOp(ctx, BSVM_SOP_CLEARENV);
		BSVMC_EmitOp(ctx, BSVM_SOP_RET);
	}

//	dyPrintf("gotos %d, labels %d\n", ctx->n_goto, ctx->n_lbl);

	if(ctx->cap)fargs=DYTF_NULL;

	//resolve gotos
	for(i=0; i<ctx->n_goto; i++)
	{
		for(j=0; j<ctx->n_lbl; j++)
			if(!strcmp(lname[j], gname[i]))
		{
			k=ldest[j]-gdest[i];
//			dyPrintf("res goto %s %s, %08X->%08X (%d)\n",
//				gname[i], lname[j], gdest[i], ldest[j], k);

			*(gdest[i]-2)=k&0xFF;
			*(gdest[i]-1)=(k>>8)&0xFF;
			break;
		}

		if(j>=ctx->n_lbl)
			dyPrintf("missing label %s\n", gname[i]);
	}


//	t=BSVM_MM_AllocObj(BSVM_TYH_SVMBLK, sizeof(BSVM_SVMBlock));
	t=gctalloc("svm_blk_t", sizeof(BSVM_SVMBlock));
	blk=(BSVM_SVMBlock *)t;

	bsSet(blk->name, blkn);
	bsSet(blk->args, args);
	bsSet(blk->fargs, fargs);
	bsSet(blk->exps, body);
	bsSet(blk->lenv, lenv);
	blk->flags=li;
	
	blk->flags=0;
	if(fargs && !(ctx->i_cap))
		blk->vmflags|=BSVM_VMFL_VARSTACK;

	blk->access=ctx->access&(~BGBDY_ACCESS_MODEMASK);
	blk->access|=BGBDY_ACCESS_DEFAULT;

	for(i=1; lit[i]; i++);
//	u=BSVM_TY_NewArrayFromArray(lit, i);
	u=dyarray(i);
	for(j=0; j<i; j++)dyarraysetidx(u, j, lit[j]);

	bsSet(blk->lit, u);

	blk->szbody=ctx->ip-ip;
	blk->body=gctatomic("svm_blk_bytecode_t", blk->szbody);
	memcpy(blk->body, ip, blk->szbody);

	lctx->cap=lctx->cap||ctx->cap;

	bsSet(ctx->lenv, NULL);
	bsSet(ctx->llenv, NULL);

	for(i=0; i<ctx->n_goto; i++)gcfree(gname[i]);
	for(i=0; i<ctx->n_lbl; i++)gcfree(lname[i]);
	gcfree(gname);
	gcfree(gdest);
	gcfree(lname);
	gcfree(ldest);

	gcfree(cname);
	gcfree(bname);

	gcfree(lit);
	gcfree(ip);
	gcfree(ctx);

#if 0
	if(fargs)
	{
		ip=blk->body;
		ipe=blk->body+blk->szbody;

		while(ip<ipe)
		{
			if(*ip==BSVM_SOP_LLOAD_F)*ip=BSVM_SOP_LLOAD_F2;
			if(*ip==BSVM_SOP_LSTORE_F)*ip=BSVM_SOP_LSTORE_F2;
			if(*ip==BSVM_SOP_LINC_FN)*ip=BSVM_SOP_LINC_FN2;
			if(*ip==BSVM_SOP_LDEC_FN)*ip=BSVM_SOP_LDEC_FN2;
			ip=BSVMC_NextOpcode(ip);
		}
	}
#endif

#if 1
	if(BSVMC_DebugP(ctx))
	{
		dyPrintf("BLK %s, SA %s, Capture=%s\n",
			dystringv(blkn), fargs?"Yes":"No",
			ctx->i_cap?"Yes":"No");

		ip=blk->body;
		ipe=blk->body+blk->szbody;

		ip=BSVMC_DumpOpcodeList(ip, ipe, blk->lit);

//		while(ip<ipe)
//		{
//			ip=BSVMC_DumpOpcode(ip, blk->lit);
//		}

		dyPrintf("\n");
	}
#endif

//	BSVM_JIT_CompileBlock(t);

	return(t);
}
