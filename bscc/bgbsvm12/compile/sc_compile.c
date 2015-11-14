#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <bgbsvm.h>

static int bsvmc_seq=1;

int BSVMC_FormIs(dyt a, char *s)
{
	return(dyconsp(a) && (dycar(a)==dysymbol(s)));
}

int BSVMC_FormIsBinary(dyt a, char *s)
{
	return(BSVMC_FormIs(a, "binary") &&
		BSVMC_FormIs(dycdr(a), s));
}

int BSVMC_FormIsUnary(dyt a, char *s)
{
	return(BSVMC_FormIs(a, "unary") &&
		BSVMC_FormIs(dycdr(a), s));
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

BGBSVM_API int BSVMC_CompileSwitch(BSVM_SVMCState *ctx, dyt l)
{
	char **cl, *s, *sdfl;
	byte *ip;
	int i, j, ncl;
	dyt c, t, u, v;

	cl=gcralloc(4096*sizeof(char *));

	ncl=0; c=dycddr(l);
	while(dyconsp(c))
	{
		if(BSVMC_FormIs(dycar(c), "case") ||
			BSVMC_FormIs(dycar(c), "case_default"))
		{
			cl[ncl++]=BSVMC_GenSym();
			c=dycdr(c);
			continue;
		}
		c=dycdr(c);
	}

	BSVMC_CompileExpr(ctx, dycadr(l));

	i=0; c=dycddr(l); sdfl=NULL;
	while(dyconsp(c))
	{
		if(BSVMC_FormIs(dycar(c), "case"))
		{
#if 0
			BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
			BSVMC_CompileExpr(ctx, dycadar(c));

			BSVMC_CompileJmpCond(ctx, BSVM_OPR_EQV, cl[i++]);
#endif

			t=dycadar(c);
			t=BSVMC_ReduceExpr(ctx, t);

			if(BSVMC_FormIs(t, "quote"))
			{
				BSVMC_CompileJmpSwitchEqual(ctx, dycadr(t), cl[i++]);
			}else if(dyconsp(t))
			{
				BSVMC_EmitOp(ctx, BSVM_SOP_DUP);
				BSVMC_CompileExpr(ctx, t);
				BSVMC_CompileJmpCond(ctx, BSVM_OPR_EQV, cl[i++]);
			}else
			{
				BSVMC_CompileJmpSwitchEqual(ctx, t, cl[i++]);
			}

			c=dycdr(c);
			continue;
		}

		if(BSVMC_FormIs(dycar(c), "case_default"))
		{
//			BSVMC_EmitOp(ctx, BSVM_SOP_POP);
//			BSVMC_CompileJmp(ctx, cl[i++]);
			sdfl=cl[i++];
			c=dycdr(c);
			continue;
		}
		c=dycdr(c);
	}


//	BSVMC_EmitOp(ctx, BSVM_SOP_POP);

	s=BSVMC_GenSym();
	ctx->breakstack[ctx->breakstackpos++]=s;
//	BSVMC_CompileBreak(ctx);

	if(!sdfl)sdfl=s;
	BSVMC_CompileJmpSwitchDefault(ctx, sdfl);

	i=0; c=dycddr(l);
	while(dyconsp(c))
	{
		if(BSVMC_FormIs(dycar(c), "case") ||
			BSVMC_FormIs(dycar(c), "case_default"))
		{
			BSVMC_EmitLabel(ctx, cl[i++], ctx->ip);
			c=dycdr(c);
			continue;
		}

		BSVMC_CompileStatement(ctx, dycar(c));
		c=dycdr(c);
	}

	BSVMC_EmitLabel(ctx, s, ctx->ip);
//	BSVMC_EmitOp(ctx, BSVM_SOP_POP);

	ctx->breakstackpos--;
	return(0);
}

dyt BSVMC_ProcessFlagsExpr(BSVM_SVMCState *ctx, dyt l, char *tb)
{
	char *s;
	dyt c, c1, c2, v, v1, n;

#if 1
	c=l; s=tb;
	while(1)
	{
		if(BSVMC_FormIs(c, "flag"))
		{
			v=dycadr(c);
			c=dycaddr(c);
			
			if(v==dysymbol("public")) { *s++='p'; continue; }
			if(v==dysymbol("private")) { *s++='q'; continue; }
			if(v==dysymbol("protected")) { *s++='r'; continue; }
			if(v==dysymbol("static")) { *s++='s'; continue; }
			if(v==dysymbol("native")) { *s++='n'; continue; }
			if(v==dysymbol("abstract")) { *s++='a'; continue; }
			if(v==dysymbol("final")) { *s++='j'; continue; }
			if(v==dysymbol("delegate")) { *s++='d'; continue; }
			if(v==dysymbol("dynamic")) { *s++='x'; continue; }
//			if(v==dysymbol("extern")) { *s++='x'; continue; }

			if(v==dysymbol("const")) { *s++='k'; continue; }
			if(v==dysymbol("new")) { *s++='k'; continue; }
			if(v==dysymbol("transient")) { *s++='y'; continue; }

			if(v==dysymbol("async")) { *s++='C'; *s++='a'; continue; }
			if(v==dysymbol("typedef")) { *s++='C'; *s++='d'; continue; }
			if(v==dysymbol("extern")) { *s++='C'; *s++='e'; continue; }

			if(v==dysymbol("_setgid")) { *s++='C'; *s++='g'; continue; }
			if(v==dysymbol("_setuid")) { *s++='C'; *s++='h'; continue; }

			if(v==dysymbol("synchronized")) { *s++='C'; *s++='s'; continue; }
			if(v==dysymbol("volatile")) { *s++='C'; *s++='v'; continue; }

			sprintf(s, "u%s;", dysymbolv(v));
			s+=strlen(s);
			continue;
		}
		if(BSVMC_FormIs(c, "declspec"))
		{
			c1=dycadr(c);
			c=dycaddr(c);
			while(c1)
			{
				v=dycar(c1);
				c1=dycdr(c1);
				if(dysymbolp(v))
				{
					sprintf(s, "Du%s;", dysymbolv(v));
					s+=strlen(s);
					continue;
				}

				if(BSVMC_FormIs(v, "funcall"))
				{
					n=dycadr(v);
					c2=dycddr(v);
					*s++='D'; *s++='(';
					while(c2)
					{
						v1=dycar(c2);
						c2=dycdr(c2);

						if(dysymbolp(v1))
						{
							sprintf(s, "u%s;", dysymbolv(v1));
							s+=strlen(s);
							continue;
						}

						if(dyintp(v1))
						{
							sprintf(s, "u%d;", dyintv(v1));
							s+=strlen(s);
							continue;
						}
					}
					sprintf(s, ")u%s;", dysymbolv(n));
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

int BSVMC_CompileEmitVar(BSVM_SVMCState *ctx, dyt l)
{
	s64 li;
	char *s;
	dyt n, v, t, u;

	n=l;
	v=NULL; t=NULL;

	if(dyFormIsP(n, "set!"))
	{
		v=dycaddr(n);
		n=dycadr(n);				
	}

	if(dyFormIsP(n, "cast"))
	{
		t=dycadr(n);
		n=dycaddr(n);				
	}

//	dyDPrintf("vars: %p %p %p\n", n, t, v);

	if(t)	{ BSVMC_CompileExprCast(ctx, v, t); }
	else	{ BSVMC_CompileExpr(ctx, v); }

	s=BSVMC_InferTypeBaseSig(t);
//	if(!s)s="r";
	u=dysymbol(s);

	if(!u && !ctx->cs_fl)
	{
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIndexLit(ctx, n);
		return(0);
	}

	if(!u)u=dysymbol("r");

	li=dyllDecodeFlagSig(dysymbolv(ctx->cs_fl));

	if(li&BGBDYC_FLAG_DYNAMIC)
		{ BSVMC_EmitOp(ctx, BSVM_SOP_TYDYNBIND); }
	else if(ctx->lvl)
		{ BSVMC_EmitOp(ctx, BSVM_SOP_TYLEXBIND);}
	else
		{ BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND); }
	BSVMC_EncIndexLit(ctx, n);
	BSVMC_EncIndexLit(ctx, u);
	BSVMC_EncIndexLit(ctx, ctx->cs_fl);
	
	return(0);
}

int BSVMC_CompileClass(BSVM_SVMCState *ctx, dyt l)
{
	char tb[256];
	dyt c, d, t, u, v, n, fl;
	char *s;
	int i, j, k, i1, lvl;

#if 0
	BSVMC_EmitOp(ctx, BSVM_SOP_MARK);

	i=BSVMC_IndexLit(ctx, dykeyword("parent"));
	BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
	BSVMC_EncIDX(ctx, i);
//	BSVMC_EmitOp(ctx, BSVM_SOP_PUSHSELF);

	s=dystringv(dycaddr(l));
	t=BSVMP_Expression(&s);
	BSVMC_CompileExpr(ctx, t);

	j=ctx->lvl;
	ctx->lvl=0;
	ctx->olvl++;

	c=dycaddddr(l);
	while(dyconsp(c))
	{
		t=dycar(c);

		if(BSVMC_FormIs(t, "vars"))
		{
			d=dycadr(t);
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

				i=BSVMC_IndexLit(ctx, n);
				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
				BSVMC_EncIDX(ctx, i);
				BSVMC_CompileExpr(ctx, v);

				d=dycdr(d);
			}
		}

		if(BSVMC_FormIs(t, "attr") || BSVMC_FormIs(t, "set!"))
		{
			i=BSVMC_IndexLit(ctx, dycadr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
			BSVMC_EncIDX(ctx, i);
			BSVMC_CompileExpr(ctx, dycaddr(t));
		}

		if(BSVMC_FormIs(t, "define"))
		{
			n=dycaadr(t);
			v=NULL; u=NULL;
	
			if(dyFormIsP(n, "cast"))
			{
				u=dycadr(n);
				n=dycaddr(n);				
			}

			i=BSVMC_IndexLit(ctx, n);
//			i=BSVMC_IndexLit(ctx, dycaadr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
			BSVMC_EncIDX(ctx, i);
			u=dycons2(dysymbol("lambda"), 
				dycdadr(t), dycddr(t));
			BSVMC_CompileExpr(ctx, u);
		}

		c=dycdr(c);
	}
	BSVMC_EmitOp(ctx, BSVM_SOP_DICT);

	ctx->olvl--;
	ctx->lvl=j;

	i=BSVMC_IndexLit(ctx, dycadr(l));
	if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
		else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
	BSVMC_EncIDX(ctx, i);
#endif

#if 0
	t=dycaddr(l);
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

	i=BSVMC_IndexLit(ctx, dycadr(l));
	if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
		else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
	BSVMC_EncIDX(ctx, i);

//	BSVMC_CompileExpr(ctx, dycadr(l));
	BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
//	BSVMC_CompileExpr(ctx, dycaddr(l));
//	BSVMC_CompileStatement(ctx, dycaddddr(l));

	j=ctx->lvl;
	ctx->lvl=0;
	ctx->olvl++;

	c=dycaddddr(l);
	while(dyconsp(c))
	{
		t=dycar(c);
		BSVMC_CompileStatement(ctx, t);
		c=dycdr(c);
	}

	ctx->olvl--;
	ctx->lvl=j;

//	BSVMC_EmitOp(ctx, BSVM_SOP_EXCH);
	BSVMC_EmitOp(ctx, BSVM_SOP_EXCH_SELF);
//	BSVMC_EmitOp(ctx, BSVM_SOP_POP);
#endif

#if 1
	i=BSVMC_IndexLit(ctx, dycadr(l));
	j=BSVMC_IndexLit(ctx, dycaddr(l));
	k=BSVMC_IndexLit(ctx, ctx->cs_fl);
//	k=0;

	i1=BSVM_SOP_BEGIN_CLASS;
	if(dyFormIsP(l, "interface"))
		i1=BSVM_SOP_BEGIN_IFACE;
	if(dyFormIsP(l, "struct"))
		i1=BSVM_SOP_BEGIN_STRUCT;
	if(dyFormIsP(l, "value_class"))
		i1=BSVM_SOP_BEGIN_STRUCT;

//	BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_CLASS);
	BSVMC_EmitOp(ctx, i1);
	BSVMC_EncIDX(ctx, i);
	BSVMC_EncIDX(ctx, j);
	BSVMC_EncIDX(ctx, k);

	lvl=ctx->lvl;
	ctx->lvl=0;
	ctx->olvl++;

	c=dycaddddr(l);
	while(dyconsp(c))
	{
		t=dycar(c);

		t=BSVMC_ProcessFlagsExpr(ctx, t, tb);
		if(tb[0])fl=dysymbol(tb);
			else fl=NULL;
		ctx->cs_fl=fl;

		if(BSVMC_FormIs(t, "vars"))
		{
			d=dycadr(t);
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

//				i=BSVMC_IndexLit(ctx, n);
//				BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//				BSVMC_EncIDX(ctx, i);
//				BSVMC_CompileExpr(ctx, v);

				if(u)	{ BSVMC_CompileExprCast(ctx, v, u); }
				else	{ BSVMC_CompileExpr(ctx, v); }

				s=BSVMC_InferTypeBaseSig(u);
				u=dysymbol(s);

				i=BSVMC_IndexLit(ctx, n);
				j=BSVMC_IndexLit(ctx, u);
				k=BSVMC_IndexLit(ctx, fl);
				BSVMC_EmitOp(ctx, BSVM_SOP_TYBIND);
				BSVMC_EncIDX(ctx, i);
				BSVMC_EncIDX(ctx, j);
				BSVMC_EncIDX(ctx, k);

				d=dycdr(d);
			}
		}

		if(BSVMC_FormIs(t, "define"))
		{
			n=dycaadr(t);
			v=NULL; u=NULL;
	
			if(dyFormIsP(n, "cast"))
			{
				u=dycadr(n);
				n=dycaddr(n);				
			}

//			i=BSVMC_IndexLit(ctx, n);
//			i=BSVMC_IndexLit(ctx, dycaadr(t));
//			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
//			BSVMC_EncIDX(ctx, i);

//			u=dycons2(dysymbol("lambda"), 
//				dycdadr(t), dycddr(t));
			u=dycons3(dysymbol("rlambda"), 
				n, dycdadr(t), dycddr(t));
			BSVMC_CompileExpr(ctx, u);

			s=BSVMC_InferArgsBaseSig(dycadr(t));
			u=dysymbol(s);

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
			u=dycons2s("lambda", NULL, dycdr(t));
			BSVMC_CompileExpr(ctx, u);
			BSVMC_EmitOp(ctx, BSVM_SOP_CALL);
		}

		c=dycdr(c);
	}
//	BSVMC_EmitOp(ctx, BSVM_SOP_END_CLASS);

	i=BSVM_SOP_END_CLASS;
	if(dyFormIsP(l, "interface"))
		i=BSVM_SOP_END_IFACE;
	if(dyFormIsP(l, "struct"))
		i=BSVM_SOP_END_STRUCT;
	if(dyFormIsP(l, "value_class"))
		i=BSVM_SOP_END_STRUCT;
	BSVMC_EmitOp(ctx, i);

	ctx->olvl--;
	ctx->lvl=lvl;

//	BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

//	i=BSVMC_IndexLit(ctx, dycadr(l));
//	if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
//		else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
//	BSVMC_EncIDX(ctx, i);
#endif

	return(0);
}

int BSVMC_CompilePackage(BSVM_SVMCState *ctx, dyt l)
{
	dyt t;
	int opcap, oplvl;
	int i, j;

	opcap=ctx->pcap;
	oplvl=ctx->plvl++;

	ctx->pcap++;

	i=BSVMC_IndexLit(ctx, dycadr(l));
	j=BSVMC_IndexLit(ctx, ctx->cs_fl);
	BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_PACKAGE);
	BSVMC_EncIDX(ctx, i);
	BSVMC_EncIDX(ctx, j);

	t=dycaddr(l);
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

void BSVMC_CompileGoto(BSVM_SVMCState *ctx, dyt l)
{
	dyt t;

	if(dysymbolp(dycadr(l)))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
		BSVMC_EncWord(ctx, 0);
		BSVMC_EmitGoto(ctx,
			dysymbolv(dycadr(l)), ctx->ip);
		return;
	}

//	t=dycadr(l);
	t=BSVMC_ReduceExpr(ctx, dycadr(l));

	if(dyFormIsP(t, "unary"))
	{
		if(dycadr(t)==dysymbol("*"))
		{
			BSVMC_CompileExpr(ctx, dycaddr(t));
			BSVMC_EmitOp(ctx, BSVM_SOP_JMPIA);
			return;
		}
	}

	BSVMC_CompileExpr(ctx, t);
	BSVMC_EmitOp(ctx, BSVM_SOP_JMPIA);
	return;

//	dyDPrintf("BSVMC_CompileGoto: Invalid Expression\n");
}

void BSVMC_CompileBegin(BSVM_SVMCState *ctx, dyt l)
{
	dyt t, c;
	s64 li;

	li=dyllDecodeFlagSig(dysymbolv(ctx->cs_fl));

	if(li&BGBDYC_FLAG_ASYNC)
	{
//		t=dycons(dysymbol("lambda"), dycons(dycdadr(l), dycddr(l)));
//		BSVMC_CompileExpr(ctx, t);

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		t=dycons(dysymbol("lambda"), dycons(NULL, dycdr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_ASYNC);
		return;
	}

	if(li&BGBDYC_FLAG_SYNCHRONIZED)
	{
		t=ctx->cb_fl;
		ctx->cb_fl=ctx->cs_fl;
		c=dycdr(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_SYNC);
		while(dyconsp(c))
		{
			BSVMC_CompileStatement(ctx, dycar(c));
			c=dycdr(c);
		}
		BSVMC_EmitOp(ctx, BSVM_SOP_END_SYNC);
		ctx->cb_fl=t;
		return;
	}

	t=ctx->cb_fl;
	ctx->cb_fl=ctx->cs_fl;
	c=dycdr(l);
	while(dyconsp(c))
	{
		BSVMC_CompileStatement(ctx, dycar(c));
		c=dycdr(c);
	}
	ctx->cb_fl=t;
	return;
}

void BSVMC_CompileBeginExpr(BSVM_SVMCState *ctx, dyt l)
{
	dyt t, c;
	s64 li;

	li=dyllDecodeFlagSig(dysymbolv(ctx->cs_fl));

	if(li&BGBDYC_FLAG_ASYNC)
	{
//		t=dycons(dysymbol("lambda"), dycons(dycdadr(l), dycddr(l)));
//		BSVMC_CompileExpr(ctx, t);

		BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
		t=dycons(dysymbol("lambda"), dycons(NULL, dycdr(l)));
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_CALL_ASYNC);
		return;
	}

	if(li&BGBDYC_FLAG_SYNCHRONIZED)
	{
		t=ctx->cb_fl;
		ctx->cb_fl=ctx->cs_fl;
		c=dycdr(l);
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_SYNC);
		while(dyconsp(dycdr(c)))
//		while(dyconsp(c))
		{
			BSVMC_CompileStatement(ctx, dycar(c));
			c=dycdr(c);
		}
//		BSVMC_CompileExprT(ctx, dycar(c));
		BSVMC_CompileExpr(ctx, dycar(c));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_SYNC);
		ctx->cb_fl=t;
		return;
	}

	t=ctx->cb_fl;
	ctx->cb_fl=ctx->cs_fl;
	c=dycdr(l);
	while(dyconsp(dycdr(c)))
	{
		BSVMC_CompileStatement(ctx, dycar(c));
		c=dycdr(c);
	}
	BSVMC_CompileExprT(ctx, dycar(c));
	ctx->cb_fl=t;
	return;
}

void BSVMC_CompileIfdef(BSVM_SVMCState *ctx, dyt l)
{
	dyt t, n, c;
	int op;

	n=dycadr(l);
	if(dyFormIsP(l, "ifdef"))op=BSVM_SOP_CALL_IFDEF;
	if(dyFormIsP(l, "ifndef"))op=BSVM_SOP_CALL_IFNDEF;

	BSVMC_EmitOp(ctx, BSVM_SOP_MARK);
	t=dycons(dysymbol("lambda"), dycons(NULL, dycddr(l)));
	BSVMC_CompileExpr(ctx, t);
	BSVMC_EmitOp(ctx, op);
	BSVMC_EncIndexLit(ctx, n);
	return;
}

void BSVMC_CompileStatement(BSVM_SVMCState *ctx, dyt l)
{
	char tb[256];
	byte *ip0, *ip1;
	dyt c, ct, cv, t, n, u, v, n1;
	char *s, *s1, *s2, *s3, *s4;
	s64 li, lj;
	int i, j, k;

	t=l;
	l=BSVMC_ReduceExpr(ctx, l);

	if(BSVMC_DebugP(ctx))
	{
		dyDPrintf("stmt: ");
		dyDPrint(t);
		dyDPrintf(" -> ");
		dyDPrint(l);
		dyDPrintf("\n");
	}

	//references/literals are useless in statement position
	if(!dyconsp(l))
	{
		if(!l)return;	//empty statement

		dyDPrintf("useless expression in statement position\n");
		return;
	}

	l=BSVMC_ProcessFlagsExpr(ctx, l, tb);
	if(tb[0])ctx->cs_fl=dysymbol(tb);
		else ctx->cs_fl=NULL;

	if(dyFormIsP(l, "module"))
	{
		ctx->mname=dycadr(l);
		return;
	}

	if(dyFormIsP(l, "import"))
	{
		i=BSVMC_IndexLit(ctx, dycadr(l));
		j=BSVMC_IndexLit(ctx, ctx->cs_fl);
		BSVMC_EmitOp(ctx, BSVM_SOP_IMPORT);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		ctx->pcap++;

		return;
	}

	if(dyFormIsP(l, "dbgmark"))
	{
		i=BSVMC_IndexLit(ctx, dycadr(l));
		j=dyintv(dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DBGMARK);
		BSVMC_EncIDX(ctx, i);
		BSVMC_EncIDX(ctx, j);
		return;
	}

	if(dyFormIsP(l, "linenum"))
	{
		i=dyintv(dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_LN);
		BSVMC_EncIDX(ctx, i);
		return;
	}

	if(dyFormIsP(l, "switch"))
	{
		BSVMC_CompileSwitch(ctx, l);
		return;
	}

	if(dyFormIsP(l, "class") ||
		dyFormIsP(l, "interface") ||
		dyFormIsP(l, "struct"))
	{
		BSVMC_CompileClass(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dyFormIsP(l, "package"))
	{
		BSVMC_CompilePackage(ctx, l);
//		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dyFormIsP(l, "goto"))
	{
		BSVMC_CompileGoto(ctx, l);
		return;
	}

	if(dyFormIsP(l, "label"))
	{
		BSVMC_EmitLabel(ctx,
			dysymbolv(dycadr(l)), ctx->ip);
		return;
	}

	if(dyFormIsP(l, "continue"))
	{
		s=ctx->contstack[ctx->contstackpos-1];

		BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
		BSVMC_EncWord(ctx, 0);
		BSVMC_EmitGoto(ctx, s, ctx->ip);
		return;
	}

	if(dyFormIsP(l, "break"))
	{
		s=ctx->breakstack[ctx->breakstackpos-1];
		BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
		BSVMC_EncWord(ctx, 0);
		BSVMC_EmitGoto(ctx, s, ctx->ip);
		return;
	}

	if(dyFormIsP(l, "return"))
	{
//		n=BSVMC_ReduceExpr(ctx, dycadr(l));
//		t=BSVMC_InferExpr(ctx, n);

//		BSVMC_CompileExpr(ctx, n);
		BSVMC_CompileExprCast(ctx, dycadr(l), ctx->cf_ty);

//		BSVMC_CompileExpr(ctx, dycadr(l));
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

	if(dyFormIsP(l, "set!"))
	{
		t=dycadr(l);
		c=ctx->lenv; ct=ctx->tenv; cv=ctx->venv;
		i=0; j=0;
		while(dyconsp(c))
		{
			if(c==ctx->llenv)i=1;
			if(dycar(c)==t)break;
			c=dycdr(c); ct=dycdr(ct); cv=dycdr(cv);
			j++;
		}
		if(dyconsp(c) && i)ctx->cap++;

		if(dyconsp(c))
		{
			t=BSVMC_InferExpr(ctx, dycaddr(l));
			n=dycar(ct);

#if 0
			if((n!=NULL) && (t==NULL))
			{
				i=BSVMC_IndexLit(ctx, n);
				BSVMC_EmitOp(ctx, BSVM_SOP_CAST_S);
				BSVMC_EncIDX(ctx, i);
				t=n;
			}

			if((n!=NULL) && (t!=n))
			{
				dyDPrintf("warn: inferencer violation\n");
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
				BSVMC_CompileExprCast(ctx, dycaddr(l), n);
				BSVMC_EmitOp(ctx, i);
				BSVMC_EncByte(ctx, j);
				return;
			}
#endif

//			if((t==dykeyword("int")) || (t==dykeyword("float")))
			if(BSVMC_TypeSmallFixRealP(ctx, t))
			{
				BSVMC_CompileExpr(ctx, dycaddr(l));
				BSVMC_EmitOp(ctx, BSVM_SOP_LSTORE_F);
				BSVMC_EncByte(ctx, j);
				return;
			}

			BSVMC_CompileExpr(ctx, dycaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_LSTORE);
			BSVMC_EncByte(ctx, j);
			return;
		}


		BSVMC_CompileExpr(ctx, dycaddr(l));
		i=BSVMC_IndexLit(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STORE);
		BSVMC_EncIDX(ctx, i);

		return;
	}

	if((dyFormIsP(l, "setindex!")) || (dyFormIsP(l, "vector-set!")))
	{
		BSVMC_CompileSetIndexStmt(ctx, l);
		return;

#if 0
		BSVMC_CompileExpr(ctx, dycadddr(l));
//		BSVMC_CompileExpr(ctx, dycaddr(l));
//		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_CompileExpr(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_STOREINDEX);
		return;
#endif
	}

	if(dyFormIsP(l, "preinc!") || dyFormIsP(l, "predec!") ||
		dyFormIsP(l, "postinc!") || dyFormIsP(l, "postdec!"))
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		u=BSVMC_InferExpr(ctx, t);

		i=BSVM_OPR_DEC;
		if(dyFormIsP(l, "preinc!") || dyFormIsP(l, "postinc!"))
			i=BSVM_OPR_INC;

//		if(dysymbolp(t) && (u==dykeyword("int")))
		if(dysymbolp(t))
		{
			c=ctx->lenv; ct=ctx->tenv; cv=ctx->venv;
			k=0; j=0;
			while(dyconsp(c))
			{
				if(c==ctx->llenv)k=1;
				if(dycar(c)==t)break;
				c=dycdr(c); ct=dycdr(ct); cv=dycdr(cv);
				j++;
			}

			if(dyconsp(c))
			{
				t=dycar(ct);
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

		if(dysymbolp(t))
		{
			if(i==BSVM_OPR_INC)BSVMC_EmitOp(ctx, BSVM_SOP_INC_S);
				else BSVMC_EmitOp(ctx, BSVM_SOP_DEC_S);
			BSVMC_EncIndexLit(ctx, t);
			return;
		}

#if 0
		if(dysymbolp(t))
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

	if(dyFormIsP(l, "define"))
	{
		if(!dyconsp(dycadr(l)))
		{
			BSVMC_CompileExpr(ctx, dycaddr(l));
			i=BSVMC_IndexLit(ctx, dycadr(l));
			if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
				else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		n=dycaadr(l);
//		if(dyFormIsP(n, "cast"))
//			n=dycaddr(n);

//		t=dycons(dysymbol("lambda"), dycons(dycdadr(l), dycddr(l)));
		t=dycons3s("rlambda", n, dycdadr(l), dycddr(l));
		BSVMC_CompileExpr(ctx, t);
//		n=dycaadr(l);
		if(dyFormIsP(n, "cast"))
			n=dycaddr(n);

#if 1
		s=BSVMC_InferArgsBaseSig(dycadr(l));
		u=dysymbol(s);

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
	if(dyFormIsP(l, "defvar"))
	{
		if(!dyconsp(dycadr(l)))
		{
			BSVMC_CompileExpr(ctx, dycaddr(l));
			i=BSVMC_IndexLit(ctx, dycadr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
			BSVMC_EncIDX(ctx, i);
			return;
		}

		t=dycons(dysymbol("lambda"), dycons(dycdadr(l), dycddr(l)));
		BSVMC_CompileExpr(ctx, t);
		i=BSVMC_IndexLit(ctx, dycaadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DYNBIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}
#endif

	if(dyFormIsP(l, "vars"))
	{
//		c=dycaddr(l);
//		t=dycadr(l);

		c=dycadr(l);
		t=NULL;

		while(dyconsp(c))
		{
			n=dycar(c);
			v=NULL; t=NULL;

			if(dyFormIsP(n, "set!"))
			{
				v=dycaddr(n);
				n=dycadr(n);				
			}

			if(dyFormIsP(n, "cast"))
			{
				t=dycadr(n);
				n=dycaddr(n);				
			}

			BSVMC_CompileEmitVar(ctx, dycar(c));

			if(ctx->lvl)
			{
				n1=NULL;
				if(!BSVMC_IsExpr(v))n1=v;

				gcSet(ctx->lenv, dycons(n, ctx->lenv));
				gcSet(ctx->tenv, dycons(t, ctx->tenv));
				gcSet(ctx->venv, dycons(n1, ctx->venv));
			}else if(!ctx->olvl)
			{
				n1=NULL;
				if(!BSVMC_IsExpr(v))n1=v;

				gcSet(ctx->mlenv, dycons(n, ctx->mlenv));
				gcSet(ctx->mtenv, dycons(t, ctx->mtenv));
				gcSet(ctx->mvenv, dycons(n1, ctx->mvenv));
			}

			c=dycdr(c);
		}
		return;
	}

#if 0
	if(dyFormIsP(l, "defun"))
	{
		if(!ctx->lvl && !ctx->olvl)
		{
			t=BSVMC_CompileBlock(ctx,
				dylist2(dycadr(l), dycaddr(l)),
				dycadddr(l), dycddddr(l));

			i=BSVMC_IndexLit(ctx, t);
			BSVMC_EmitOp(ctx, BSVM_SOP_PUSH);
			BSVMC_EncIDX(ctx, i);

			gcSet(ctx->mfcns, dycons(t, ctx->mfcns));

			i=BSVMC_IndexLit(ctx, dycaddr(l));
			BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
			BSVMC_EncIDX(ctx, i);

			return;
		}


//		t=CONS2S("lambda", dycadddr(l), dycddddr(l));

		t=dycons3s("rlambda", dylist2(dycadr(l), dycaddr(l)),
			dycadddr(l), dycddddr(l));

//		BSVM_TY_PrintLN(t);

		BSVMC_CompileExpr(ctx, t);
		i=BSVMC_IndexLit(ctx, dycaddr(l));
		if(ctx->lvl)BSVMC_EmitOp(ctx, BSVM_SOP_LEXBIND);
			else BSVMC_EmitOp(ctx, BSVM_SOP_BIND);
		BSVMC_EncIDX(ctx, i);
		return;
	}
#endif

	if(dyFormIsP(l, "if"))
	{
		t=BSVMC_ReduceExpr(ctx, dycadr(l));

#if 0
		i=BSVMC_BoolExpr(ctx, t);
		if(i==1)
		{
			BSVMC_CompileStatement(ctx, dycaddr(l));
			return;
		}
		if(i==0)
		{
			if(dyconsp(dycdddr(l)))
				BSVMC_CompileStatement(ctx, dycadddr(l));
			return;
		}
#endif

		dyDPrintf("if: ");
		dyDPrint(t);
		dyDPrintf("\n");

		BSVMC_CompileJCF(ctx, t);
		ip0=ctx->ip;


		BSVMC_CompileStatement(ctx, dycaddr(l));

		if(dyconsp(dycdddr(l)))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_JMP);
			BSVMC_EncWord(ctx, 0);
			ip1=ctx->ip;

			i=ctx->ip-ip0;
			*(ip0-2)=i&0xFF;
			*(ip0-1)=(i>>8)&0xFF;

			BSVMC_CompileStatement(ctx, dycadddr(l));
			ip0=ip1;
		}
		i=ctx->ip-ip0;
		*(ip0-2)=i&0xFF;
		*(ip0-1)=(i>>8)&0xFF;
		return;
	}

	if(dyFormIsP(l, "begin"))
	{
		BSVMC_CompileBegin(ctx, l);
		return;
	}

	if(dyFormIsP(l, "synchronized"))
	{
		BSVMC_CompileExpr(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_DUP);

		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_SYNC_OBJ);
		BSVMC_CompileStatement(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_SYNC_OBJ);
		return;
	}

	if(dyFormIsP(l, "throw"))
	{
		t=dycadr(l);
		
		if(dysymbolp(t))
		{
			BSVMC_EmitOp(ctx, BSVM_SOP_THROW_S);
			BSVMC_EncIndexLit(ctx, t);
			return;
		}
		
		BSVMC_CompileExpr(ctx, t);
		BSVMC_EmitOp(ctx, BSVM_SOP_THROW_OBJ);
		return;
	}

	if(dyFormIsP(l, "try"))
	{
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_TRY);
		BSVMC_CompileStatement(ctx, dycadr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_TRY);
		return;
	}

	if(dyFormIsP(l, "catch"))
	{
		s1=BSVMC_GenSym();
		t=dycadr(l); n=NULL;

		if(dyFormIsP(t, "cast"))
		{
			n=dycaddr(t);
			t=dycadr(t);
		}

		s=BSVMC_InferArgsBaseSig(t);
		u=dysymbol(s);

		BSVMC_EmitOp(ctx, BSVM_SOP_CATCH);
		BSVMC_EncIndexLit(ctx, u);
		BSVMC_EmitJmpAddr(ctx, s1);

		BSVMC_CompileStatement(ctx, dycaddr(l));

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
		
		BSVMC_CompileStatement(ctx, dycadddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_CATCH);
		return;
	}

	if(dyFormIsP(l, "finally"))
	{
		s1=BSVMC_GenSym();

		BSVMC_EmitOp(ctx, BSVM_SOP_CATCH);
		BSVMC_EncIndexLit(ctx, NULL);
		BSVMC_EmitJmpAddr(ctx, s1);

		BSVMC_CompileStatement(ctx, dycadr(l));

		BSVMC_EmitLabel(ctx, s1, ctx->ip);
		BSVMC_EmitOp(ctx, BSVM_SOP_BEGIN_CATCH_FINAL);
//		BSVMC_EncIndexLit(ctx, NULL);
		BSVMC_CompileStatement(ctx, dycaddr(l));
		BSVMC_EmitOp(ctx, BSVM_SOP_END_CATCH);
		return;
	}

	if(dyFormIsP(l, "ifdef") ||
		dyFormIsP(l, "ifndef"))
	{
		BSVMC_CompileIfdef(ctx, l);
		BSVMC_EmitOp(ctx, BSVM_SOP_POP);
		return;
	}

	if(dyFormIsP(l, "for"))
	{
		s1=BSVMC_GenSym();
		s2=BSVMC_GenSym();
		s3=BSVMC_GenSym();

		ctx->contstack[ctx->contstackpos++]=s1;
		ctx->breakstack[ctx->breakstackpos++]=s2;

		BSVMC_CompileStatement(ctx, dycadr(l));

//		BSVMC_EmitLabel(ctx, s1, ctx->ip);
		BSVMC_EmitLabel(ctx, s3, ctx->ip);

//		BSVMC_CompileExpr(ctx, dycaddr(l));
//		BSVMC_CompileBreakFalse(ctx);

		BSVMC_CompileJCF(ctx, dycaddr(l));
		BSVMC_EmitGoto(ctx, s2, ctx->ip);

		BSVMC_CompileStatement(ctx, dycaddddr(l));

		BSVMC_EmitLabel(ctx, s1, ctx->ip);
		BSVMC_CompileStatement(ctx, dycadddr(l));
//		BSVMC_CompileContinue(ctx);
		BSVMC_EmitJmp(ctx, s3);

		BSVMC_EmitLabel(ctx, s2, ctx->ip);

		ctx->contstackpos--;
		ctx->breakstackpos--;
		return;
	}

	if(dyFormIsP(l, "while"))
	{
		s1=BSVMC_GenSym();
		s2=BSVMC_GenSym();

		ctx->contstack[ctx->contstackpos++]=s1;
		ctx->breakstack[ctx->breakstackpos++]=s2;

		BSVMC_EmitLabel(ctx, s1, ctx->ip);

		t=BSVMC_ReduceExpr(ctx, dycadr(l));
		i=BSVMC_BoolExpr(ctx, t);

		if(i==0)return;

		if(i==1)
		{
			BSVMC_CompileStatement(ctx, dycaddr(l));
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

		BSVMC_CompileStatement(ctx, dycaddr(l));

		BSVMC_CompileContinue(ctx);
		BSVMC_EmitLabel(ctx, s2, ctx->ip);

		ctx->contstackpos--;
		ctx->breakstackpos--;
		return;
	}


	if(dyFormIsP(l, "do_while"))
	{
		s1=BSVMC_GenSym();
		s2=BSVMC_GenSym();
		ctx->contstack[ctx->contstackpos++]=s1;
		ctx->breakstack[ctx->breakstackpos++]=s2;

		BSVMC_EmitLabel(ctx, s1, ctx->ip);

		BSVMC_CompileStatement(ctx, dycaddr(l));

		t=BSVMC_ReduceExpr(ctx, dycadr(l));
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

	if(dyFormIsP(l, "inline_c"))
	{
		t=dycadr(l);
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

byte *BSVMC_DumpOpcode(byte *ip, dyt lit)
	{ return(BSVMC_DumpOpcodeFl(ip, lit, 0)); }

byte *BSVMC_DumpOpcodeFl(byte *ip, dyt lit, int fl)
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

	dyt t;
	byte *oip, *ip1, *ip2;
	char *s;
	int i, j, k, l;

	oip=ip;
	i=*ip++;
	if(i>=240)
	{
		i=(i-240)*256+(*ip++);
		i=(i*256)+(*ip++);
		dyDPrintf("%08X %s%02X%02X%02X ", oip,
			(fl&1)?"  ":"", oip[0], oip[1], oip[2]);
	}else
	if(i>=192)
	{
		i=(i-192)*256+(*ip++);
		dyDPrintf("%08X %s%02X%02X ", oip, (fl&1)?"  ":"", oip[0], oip[1]);
	}else
	{
		dyDPrintf("%08X %s%02X ", oip, (fl&1)?"  ":"", i);
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
		dyDPrintf("%02X", *ip1++);

	if(i<12288)
		dyDPrintf("  ");
	if(i<192)
		dyDPrintf("  ");
	for(; j<4; j++)
		dyDPrintf("  ");

	dyDPrintf(" %s ", bsvm_ops_strs[i]);

	s=bsvm_ops_args[i];
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			t=dyarrayidx(lit, i);
			dyDPrint(t);
			dyDPrintf(" ");
			s++;
			continue;
		}
		if((*s=='N') || (*s=='J'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			dyDPrintf("%d ", i);
			s++;
			continue;
		}
		if(*s=='A')
		{
			i=*ip++; i|=(*ip++)<<8;
			dyDPrintf("%08X ", ip+i);
			s++;
			continue;
		}
		if(*s=='C') { i=(signed char)(*ip++); dyDPrintf("%d ", i);
			s++; continue; }
		if(*s=='B') { i=*ip++; dyDPrintf("%d ", i); s++; continue; }
		if(*s=='W') { i=*ip++; i|=(*ip++)<<8; dyDPrintf("%d ", i);
			s++; continue; }
		if(*s=='K') { i=*ip++; i|=(*ip++)<<8;
			i=(short)i; dyDPrintf("%d ", i);
			s++; continue; }
		if(*s=='H') { i=*ip++; i|=(*ip++)<<8;
			i=(short)i; dyDPrintf("%g ", i/100.0);
			s++; continue; }
		if(*s=='G') { i=*ip++; i|=(*ip++)<<8;
			i=(short)i; dyDPrintf("%g ", i/10000.0);
			s++; continue; }

		if(*s=='U')
		{
			i=BS2ASM_DecVLI(&ip);
			dyDPrintf("%d ", i);
			s++; continue;
		}
		if(*s=='V')
		{
			i=BS2ASM_DecSVLI(&ip);
			dyDPrintf("%d ", i);
			s++; continue;
		}

		if(*s=='D')
		{
			i=BS2ASM_DecSVLI(&ip);
			j=BS2ASM_DecSVLI(&ip);
			dyDPrintf("%f ", i*pow(2, j));
			s++; continue;
		}

		if(*s=='u') { i=*ip++; dyDPrintf("%s ", uops[i]); s++; continue; }
		if(*s=='b') { i=*ip++; dyDPrintf("%s ", bops[i]); s++; continue; }
		s++;
	}

	dyDPrintf("\n");

	return(ip);
}

byte *BSVMC_DumpOpcodeList(byte *ips, byte *ipe, dyt lit)
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

dyt BSVMC_CompileBlock(BSVM_SVMCState *lctx,
	dyt name, dyt args, dyt body)
{
	return(BSVMC_CompileBlockEnv(lctx, lctx->lenv, name, args, body));
}

dyt BSVMC_CompileBlockEnv(BSVM_SVMCState *lctx,
	dyt lenv, dyt name, dyt args, dyt body)
{
	BSVM_SVMBlock *blk;
	BSVM_SVMCState *ctx;

	char **gname, **lname;
	byte **gdest, **ldest;
	char **cname, **bname;

	char *sig;

	dyt *lit;
	byte *ip, *cip, *ipe;
	s64 li;

	dyt fargs, blkn;
	dyt c, t, u, v, n;
	int i, j, k;

	li=dyllDecodeFlagSig(dysymbolv(lctx->cs_fl));

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

	gcSet(ctx->lenv, lctx->lenv);
	gcSet(ctx->llenv, ctx->lenv);
	gcSet(ctx->tenv, lctx->tenv);
	gcSet(ctx->ltenv, ctx->tenv);
	gcSet(ctx->venv, lctx->venv);
	gcSet(ctx->lvenv, ctx->venv);

	if(dyFormIsP(name, "cast"))
	{
		ctx->cf_ty=dycadr(name);
		name=dycaddr(name);
		ctx->cf_n=name;
	}

//	if(dyconsp(name))
//	{
//		ctx->cf_n=dycadr(name);
//		ctx->cf_ty=dycar(name);
//	}
	
	ctx->cf_n=name;
	blkn=ctx->cf_n;

	ctx->cf_fl=lctx->cs_fl;
	ctx->access=lctx->access;


	c=lenv; u=NULL;
	while(dyconsp(c))
	{
		n=dycar(c);
		t=NULL;
		v=NULL;

		if(dyFormIsP(n, "set!"))
		{
			v=dycaddr(n);
			n=dycadr(n);
		}

		if(dyFormIsP(n, "cast"))
		{
			t=dycadr(n);
			n=dycaddr(n);
		}

		gcSet(ctx->lenv, dycons(n, ctx->lenv));
		gcSet(ctx->tenv, dycons(t, ctx->tenv));
		gcSet(ctx->venv, dycons(v, ctx->venv));

		n=BSVMC_BuildVarInfo(dycar(c));

		u=dycons(n, u);
		c=dycdr(c);
	}

	lenv=dynreverse(u);


	c=args; u=NULL;
	while(dyconsp(c))
	{
		n=dycar(c);
		t=NULL;
		v=NULL;

		if(dyFormIsP(n, "rest"))
		{
//			v=dycaddr(n);
			n=dycadr(n);
		}

		if(dyFormIsP(n, "set!"))
		{
			v=dycaddr(n);
			n=dycadr(n);
		}

		if(dyFormIsP(n, "cast"))
		{
			t=dycadr(n);
			n=dycaddr(n);
		}

		gcSet(ctx->lenv, dycons(n, ctx->lenv));
		gcSet(ctx->tenv, dycons(t, ctx->tenv));
		gcSet(ctx->venv, dycons(v, ctx->venv));

#if 0
		sig=BSVMC_InferTypeBaseSig(t);
		if(sig && *sig && strcmp(sig, "r"))
		{
			c=dycdr(c);
			continue;
		}
#endif

		n=BSVMC_BuildVarInfo(dycar(c));

		u=dycons(n, u);
		c=dycdr(c);

#if 0
		t=dycar(c);
		if(dyconsp(t))
		{
			gcSet(ctx->lenv, dycons(dycadr(t), ctx->lenv));
			gcSet(ctx->tenv, dycons(dycar(t), ctx->tenv));
			gcSet(ctx->venv, dycons(NULL, ctx->venv));
			u=dycons(dycadr(t), u);
		}else
		{
			gcSet(ctx->lenv, dycons(dycar(c), ctx->lenv));
			gcSet(ctx->tenv, dycons(NULL, ctx->tenv));
			gcSet(ctx->venv, dycons(NULL, ctx->venv));
			u=dycons(t, u);
		}
		c=dycdr(c);
#endif
	}

	if(c)
	{
		ctx->lenv=dycons(c, ctx->lenv);
		fargs=NULL;
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

	if(dyconsp(body))
	{
		c=body;
		while(dyconsp(c))
		{
			if(dyconsp(dycdr(c)))
			{
				t=BSVMC_ReverseStepBlock(ctx, dycdr(c));
				BSVMC_CompileStatement(ctx, dycar(c));
				c=t;
				continue;
			}else
			{
				BSVMC_CompileExprTail(ctx, dycar(c));
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

//	dyDPrintf("gotos %d, labels %d\n", ctx->n_goto, ctx->n_lbl);

	if(ctx->cap)fargs=NULL;

	//resolve gotos
	for(i=0; i<ctx->n_goto; i++)
	{
		for(j=0; j<ctx->n_lbl; j++)
			if(!strcmp(lname[j], gname[i]))
		{
			k=ldest[j]-gdest[i];
//			dyDPrintf("res goto %s %s, %08X->%08X (%d)\n",
//				gname[i], lname[j], gdest[i], ldest[j], k);

			*(gdest[i]-2)=k&0xFF;
			*(gdest[i]-1)=(k>>8)&0xFF;
			break;
		}

		if(j>=ctx->n_lbl)
			dyDPrintf("missing label %s\n", gname[i]);
	}


//	t=BSVM_MM_AllocObj(BSVM_TYH_SVMBLK, sizeof(BSVM_SVMBlock));
	t=gctalloc("svm_blk_t", sizeof(BSVM_SVMBlock));
	blk=(BSVM_SVMBlock *)t;

	gcSet(blk->name, blkn);
	gcSet(blk->args, args);
	gcSet(blk->fargs, fargs);
	gcSet(blk->exps, body);
	gcSet(blk->lenv, lenv);
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

	gcSet(blk->lit, u);

	blk->szbody=ctx->ip-ip;
	blk->body=gctatomic("svm_blk_bytecode_t", blk->szbody);
	memcpy(blk->body, ip, blk->szbody);

	lctx->cap=lctx->cap||ctx->cap;

	gcSet(ctx->lenv, NULL);
	gcSet(ctx->llenv, NULL);

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
		dyDPrintf("BLK %s, SA %s, Capture=%s\n",
			dystringv(blkn), fargs?"Yes":"No",
			ctx->i_cap?"Yes":"No");

		ip=blk->body;
		ipe=blk->body+blk->szbody;

		ip=BSVMC_DumpOpcodeList(ip, ipe, blk->lit);

//		while(ip<ipe)
//		{
//			ip=BSVMC_DumpOpcode(ip, blk->lit);
//		}

		dyDPrintf("\n");
	}
#endif

//	BSVM_JIT_CompileBlock(t);

	return(t);
}
