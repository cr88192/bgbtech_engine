/*
	Objective:
	Process GAST and convert to internal S-Exps to feed through BS compiler.
 */

#include <bgbsvm.h>

dytf BSVMC_TranslateGastExpr(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2;
	dytf t, n, lst, t0, t1, t2;
	char *s, *s0, *s1;

	if(dyxTagIsP(l, "ref"))
	{
		s=dyxGet(l, "name");
		return(dytfSymbol(s));
	}

	if(dyxTagIsP(l, "string"))
	{
		s=dyxGet(l, "value");
		return(dytfString(s));
	}

	if(dyxTagIsP(l, "int"))
	{
		s=dyxGet(l, "value");
		return(dytfInt(atoi(s)));
	}

	if(dyxTagIsP(l, "real"))
	{
		s=dyxGet(l, "value");
		return(dytfFloat(atof(s)));
	}

	if(dyxTagIsP(l, "compex"))
	{
		s0=dyxGet(l, "real");
		s1=dyxGet(l, "imag");
		t0=dytfFloat(atof(s0));
		t1=dytfFloat(atof(s1));
		return(dytfList3s("complex", t0, t1));
	}

	if(dyxTagIsP(l, "assign"))
	{
		n0=dyxFetch(l, "left");
		n1=dyxFetch(l, "right");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		s=dyxGet(l, "op");
		if(s) t1=dytfList4s("binary", dytfSymbol(s), t0, t1);
		
		if(dyxTagIsP(n0, "getindex"))
			return(dytfList4s("setindex!", dycadr(t0), dycaddr(t0), t1));
		if(dyxTagIsP(n0, "objref"))
			return(dytfList4s("objset!", dycadr(t0), dycaddr(t0), t1));
		
		return(dytfList3s("set!", t0, t1));
	}

	if(dyxTagIsP(l, "getindex"))
	{
		n0=dyxFetch(l, "array");
		n1=dyxFetch(l, "index");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		return(dytfList3s("getindex", t0, t1));
	}

	if(dyxTagIsP(l, "setindex"))
	{
		n0=dyxFetch(l, "array");
		n1=dyxFetch(l, "index");
		n2=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t2=BSVMC_TranslateGastExpr(ctx, n2);
		return(dytfList4s("setindex!", t0, t1, t2));
	}

	if(dyxTagIsP(l, "objref"))
	{
		n0=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=dytfSymbol(dyxGet(l, "name"));
		return(dytfList3s("objref", t0, t1));
	}

	if(dyxTagIsP(l, "preinc"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dytfList2s("preinc!", t0));
	}
	if(dyxTagIsP(l, "predec"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dytfList2s("predec!", t0));
	}
	if(dyxTagIsP(l, "postinc"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dytfList2s("postinc!", t0));
	}
	if(dyxTagIsP(l, "postdec"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dytfList2s("postdec!", t0));
	}

	if(dyxTagIsP(l, "if"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "then");
		n2=dyxFetch(l, "else");
		if(!n2)
		{
			t0=BSVMC_TranslateGastExpr(ctx, n0);
			t1=BSVMC_TranslateGastExpr(ctx, n1);
			return(dytfList3s("if", t0, t1));
		}
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t2=BSVMC_TranslateGastExpr(ctx, n2);
		return(dytfList4s("if", t0, t1, t2));
	}

	if(dyxTagIsP(l, "begin"))
	{
		n0=dyxChild(l); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastExpr(ctx, n0);
			if(t) lst=dytfNAppend(lst, dytfList1(t));
			n0=dyxNext(n0);
		}
		return(dytfConss("begin", lst));
	}

	if(dyxTagIsP(l, "binary"))
	{
		n0=dyxFetch(l, "left");
		n1=dyxFetch(l, "right");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t2=dytfSymbol(dyxGet(l, "op"));
		return(dytfList4s("binary", t2, t0, t1));
	}

	if(dyxTagIsP(l, "unary"))
	{
		n0=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t2=dytfSymbol(dyxGet(l, "op"));
		return(dytfList3s("unary", t2, t0));
	}

	if(dyxTagIsP(l, "funcall"))
	{
		n0=dyxFetch(l, "args"); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastExpr(ctx, n0);
			if(t) lst=dytfNAppend(lst, dytfList1(t));
			n0=dyxNext(n0);
		}
		
		n0=dyxFetch(l, "value");
		if(n0)
		{
			t0=BSVMC_TranslateGastExpr(ctx, n0);
		}else
		{
			t0=dytfSymbol(dyxGet(l, "name"));
		}
		
		return(dytfCons2s("funcall", t0, lst));
	}

	if(dyxTagIsP(l, "methodcall"))
	{
		n0=dyxFetch(l, "args"); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastExpr(ctx, n0);
			if(t) lst=dytfNAppend(lst, dytfList1(t));
			n0=dyxNext(n0);
		}
		
		n0=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=dytfSymbol(dyxGet(l, "name"));
		
		return(dytfCons3s("methodcall", t0, t1, lst));
	}

	return(NULL);
}

dytf BSVMC_TranslateGastExprList(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2;
	dytf t, n, lst, t0, t1, t2;
	char *s;

	n0=l; lst=NULL;
	while(n0)
	{
		t=BSVMC_TranslateGastExpr(ctx, n0);
		if(t) lst=dytfNAppend(lst, dytfList1(t));
		n0=dyxNext(n0);
	}
	return(lst);
}

dytf BSVMC_TranslateGastStmtList(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2;
	dytf t, n, lst, t0, t1, t2;
	char *s;

	n0=l; lst=NULL;
	while(n0)
	{
		t=BSVMC_TranslateGastStmt(ctx, n0);
		if(t) lst=dytfNAppend(lst, dytfList1(t));
		n0=dyxNext(n0);
	}
	return(lst);
}

dytf BSVMC_TranslateGastStmt(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2, *n3;
	dytf t, n, lst, t0, t1, t2, t3;
	char *s;

	if(dyxTagIsP(l, "break"))
		return(dytfList1s("break"));
	if(dyxTagIsP(l, "continue"))
		return(dytfList1s("continue"));
	if(dyxTagIsP(l, "default"))
		return(dytfList1s("case_default"));

	if(dyxTagIsP(l, "label"))
	{
		s=dyxGet(l, "name");
		return(dytfList2s("label", dytfSymbol(s)));
	}
	if(dyxTagIsP(l, "goto"))
	{
		s=dyxGet(l, "name");
		return(dytfList2s("goto", dytfSymbol(s)));
	}
	if(dyxTagIsP(l, "import"))
	{
		s=dyxGet(l, "name");
		return(dytfList2s("import", dytfSymbol(s)));
	}
	if(dyxTagIsP(l, "using"))
	{
		s=dyxGet(l, "name");
		return(dytfList2s("using", dytfSymbol(s)));
	}

	if(dyxTagIsP(l, "return"))
	{
		n0=dyxFetch(l, "expr");
		t=BSVMC_TranslateGastExpr(ctx, n0);
		return(dytfList2s("return", t));
	}

	if(dyxTagIsP(l, "case"))
	{
		n0=dyxFetch(l, "value");
		t=BSVMC_TranslateGastExpr(ctx, n0);
		return(dytfList2s("case", t));
	}

	if(dyxTagIsP(l, "throw"))
	{
		n0=dyxFetch(l, "value");
		t=BSVMC_TranslateGastExpr(ctx, n0);
		return(dytfList2s("throw", t));
	}

	if(dyxTagIsP(l, "get"))
	{
		n0=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastStmtList(ctx, n0);
		return(dytfConss("get", t0));
	}

	if(dyxTagIsP(l, "set"))
	{
		n0=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastStmtList(ctx, n0);
		return(dytfConss("set", t0));
	}

	if(dyxTagIsP(l, "if"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "then");
		n2=dyxFetch(l, "else");
		if(!n2)
		{
			t0=BSVMC_TranslateGastExpr(ctx, n0);
			t1=BSVMC_TranslateGastStmt(ctx, n1);
			return(dytfList3s("if", t0, t1));
		}
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmt(ctx, n1);
		t2=BSVMC_TranslateGastStmt(ctx, n2);
		return(dytfList4s("if", t0, t1, t2));
	}

	if(dyxTagIsP(l, "begin"))
	{
		n0=dyxChild(l); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastStmt(ctx, n0);
			if(t) lst=dytfNAppend(lst, dytfList1(t));
			n0=dyxNext(n0);
		}
		return(dytfConss("begin", lst));
	}

	if(dyxTagIsP(l, "vars"))
	{
		n0=dyxChild(l); lst=NULL;
		while(n0)
		{
			t=dytfSymbol(dyxGet(n0, "name"));
			n1=dyxFetch(n0, "value");
			if(n1)
			{
				t1=BSVMC_TranslateGastExpr(ctx, n1);
				t=dytfList2(t, t1);
			}
			if(t) lst=dytfNAppend(lst, dytfList1(t));
			n0=dyxNext(n0);
		}
		return(dytfConss("vars", lst));
	}

	if(dyxTagIsP(l, "switch"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmtList(ctx, n1);
		return(dytfCons2s("switch", t0, t1));
	}

	if(dyxTagIsP(l, "for"))
	{
		n0=dyxFetch(l, "init");
		n1=dyxFetch(l, "cond");
		n2=dyxFetch(l, "step");
		n3=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t2=BSVMC_TranslateGastExpr(ctx, n2);
		t3=BSVMC_TranslateGastStmtList(ctx, n3);
		return(dytfCons4s("for", t0, t1, t2, t3));
	}

	if(dyxTagIsP(l, "for_in"))
	{
		n0=dyxFetch(l, "left");
		n1=dyxFetch(l, "right");
		n3=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t3=BSVMC_TranslateGastStmtList(ctx, n3);
		return(dytfCons3s("for_in", t0, t1, t3));
	}

	if(dyxTagIsP(l, "while"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmtList(ctx, n1);
		return(dytfCons2s("while", t0, t1));
	}

	if(dyxTagIsP(l, "do_while"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmtList(ctx, n1);
		return(dytfCons2s("do_while", t0, t1));
	}

	if(dyxTagIsP(l, "defun"))
	{
		n0=dyxFetch(l, "args"); lst=NULL;
		while(n0)
		{
			t=dytfSymbol(dyxGet(n0, "name"));
			n1=dyxFetch(n0, "value");
			if(n1)
			{
				t1=BSVMC_TranslateGastExpr(ctx, n1);
				t=dytfList2(t, t1);
			}
			if(t) lst=dytfNAppend(lst, dytfList1(t));
			n0=dyxNext(n0);
		}

		n0=dyxFetch(l, "type");
		n1=dyxFetch(l, "body");
//		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t0=dytfSymbol("var");
		t1=dytfSymbol(dyxGet(l, "name"));
		t2=lst;
		
		t3=BSVMC_TranslateGastStmtList(ctx, n1);
		//(defun type name args body*)
		return(dytfCons4s("defun", t0, t1, t2, t3));
	}

	return(BSVMC_TranslateGastExpr(ctx, l));
}

BGBSVM_API dytf BSVMC_CompileGast(BSVM_SVMCState *ctx, dyxNode *mod)
{
	dyxNode *cur;
	dytf lst, t;

//	mod=DYX_SBXE_ReadNodeList(data, sz);

	dyxPrint(mod);
	printf("\n");

	cur=dyxFetch(mod, "body"); lst=NULL;
	while(cur)
	{
		t=BSVMC_TranslateGastStmt(ctx, cur);
		if(t) lst=dytfNAppend(lst, dytfList1(t));
		cur=dyxNext(cur);
	}
	
	t=BSVMC_CompileBlock(ctx, NULL, NULL, lst);
	return(t);
	
//	return(0);
}

BGBSVM_API dytf BSVMC_CompileGastSBXE(
	BSVM_SVMCState *ctx, byte *data, int sz)
{
	dyxNode *mod;
	dytf lst, t;

	mod=DYX_SBXE_ReadNodeList(data, sz);
//	dyxPrint(mod);
	t=BSVMC_CompileGast(ctx, mod);
//	dyxFreeList(mod);
	return(t);
}
