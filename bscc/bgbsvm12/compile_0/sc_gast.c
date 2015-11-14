/*
	Objective:
	Process GAST and convert to internal S-Exps to feed through BS compiler.
 */

#include <bgbsvm.h>

dyt BSVMC_TranslateGastExpr(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2;
	dyt t, n, lst, t0, t1, t2;
	char *s, *s0, *s1;

	if(dyxTagIsP(l, "ref"))
	{
		s=dyxGet(l, "name");
		return(dysymbol(s));
	}

	if(dyxTagIsP(l, "string"))
	{
		s=dyxGet(l, "value");
		return(dystring(s));
	}

	if(dyxTagIsP(l, "int"))
	{
		s=dyxGet(l, "value");
		return(dyint(atoi(s)));
	}

	if(dyxTagIsP(l, "real"))
	{
		s=dyxGet(l, "value");
		return(dyfloat(atof(s)));
	}

	if(dyxTagIsP(l, "compex"))
	{
		s0=dyxGet(l, "real");
		s1=dyxGet(l, "imag");
		t0=dyfloat(atof(s0));
		t1=dyfloat(atof(s1));
		return(dylist3s("complex", t0, t1));
	}

	if(dyxTagIsP(l, "assign"))
	{
		n0=dyxFetch(l, "left");
		n1=dyxFetch(l, "right");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		s=dyxGet(l, "op");
		if(s) t1=dylist4s("binary", dysymbol(s), t0, t1);
		
		if(dyxTagIsP(n0, "getindex"))
			return(dylist4s("setindex!", dycadr(t0), dycaddr(t0), t1));
		if(dyxTagIsP(n0, "objref"))
			return(dylist4s("objset!", dycadr(t0), dycaddr(t0), t1));
		
		return(dylist3s("set!", t0, t1));
	}

	if(dyxTagIsP(l, "getindex"))
	{
		n0=dyxFetch(l, "array");
		n1=dyxFetch(l, "index");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		return(dylist3s("getindex", t0, t1));
	}

	if(dyxTagIsP(l, "setindex"))
	{
		n0=dyxFetch(l, "array");
		n1=dyxFetch(l, "index");
		n2=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t2=BSVMC_TranslateGastExpr(ctx, n2);
		return(dylist4s("setindex!", t0, t1, t2));
	}

	if(dyxTagIsP(l, "objref"))
	{
		n0=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=dysymbol(dyxGet(l, "name"));
		return(dylist3s("objref", t0, t1));
	}

	if(dyxTagIsP(l, "preinc"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dylist2s("preinc!", t0));
	}
	if(dyxTagIsP(l, "predec"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dylist2s("predec!", t0));
	}
	if(dyxTagIsP(l, "postinc"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dylist2s("postinc!", t0));
	}
	if(dyxTagIsP(l, "postdec"))
	{
		n0=dyxFetch(l, "expr");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		return(dylist2s("postdec!", t0));
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
			return(dylist3s("if", t0, t1));
		}
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t2=BSVMC_TranslateGastExpr(ctx, n2);
		return(dylist4s("if", t0, t1, t2));
	}

	if(dyxTagIsP(l, "begin"))
	{
		n0=dyxChild(l); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastExpr(ctx, n0);
			if(t) lst=dynappend(lst, dylist1(t));
			n0=dyxNext(n0);
		}
		return(dyconss("begin", lst));
	}

	if(dyxTagIsP(l, "binary"))
	{
		n0=dyxFetch(l, "left");
		n1=dyxFetch(l, "right");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t2=dysymbol(dyxGet(l, "op"));
		return(dylist4s("binary", t2, t0, t1));
	}

	if(dyxTagIsP(l, "unary"))
	{
		n0=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t2=dysymbol(dyxGet(l, "op"));
		return(dylist3s("unary", t2, t0));
	}

	if(dyxTagIsP(l, "funcall"))
	{
		n0=dyxFetch(l, "args"); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastExpr(ctx, n0);
			if(t) lst=dynappend(lst, dylist1(t));
			n0=dyxNext(n0);
		}
		
		n0=dyxFetch(l, "value");
		if(n0)
		{
			t0=BSVMC_TranslateGastExpr(ctx, n0);
		}else
		{
			t0=dysymbol(dyxGet(l, "name"));
		}
		
		return(dycons2s("funcall", t0, lst));
	}

	if(dyxTagIsP(l, "methodcall"))
	{
		n0=dyxFetch(l, "args"); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastExpr(ctx, n0);
			if(t) lst=dynappend(lst, dylist1(t));
			n0=dyxNext(n0);
		}
		
		n0=dyxFetch(l, "value");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=dysymbol(dyxGet(l, "name"));
		
		return(dycons3s("methodcall", t0, t1, lst));
	}

	return(NULL);
}

dyt BSVMC_TranslateGastExprList(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2;
	dyt t, n, lst, t0, t1, t2;
	char *s;

	n0=l; lst=NULL;
	while(n0)
	{
		t=BSVMC_TranslateGastExpr(ctx, n0);
		if(t) lst=dynappend(lst, dylist1(t));
		n0=dyxNext(n0);
	}
	return(lst);
}

dyt BSVMC_TranslateGastStmtList(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2;
	dyt t, n, lst, t0, t1, t2;
	char *s;

	n0=l; lst=NULL;
	while(n0)
	{
		t=BSVMC_TranslateGastStmt(ctx, n0);
		if(t) lst=dynappend(lst, dylist1(t));
		n0=dyxNext(n0);
	}
	return(lst);
}

dyt BSVMC_TranslateGastStmt(BSVM_SVMCState *ctx, dyxNode *l)
{
	dyxNode *n0, *n1, *n2, *n3;
	dyt t, n, lst, t0, t1, t2, t3;
	char *s;

	if(dyxTagIsP(l, "break"))
		return(dylist1s("break"));
	if(dyxTagIsP(l, "continue"))
		return(dylist1s("continue"));
	if(dyxTagIsP(l, "default"))
		return(dylist1s("case_default"));

	if(dyxTagIsP(l, "label"))
	{
		s=dyxGet(l, "name");
		return(dylist2s("label", dysymbol(s)));
	}
	if(dyxTagIsP(l, "goto"))
	{
		s=dyxGet(l, "name");
		return(dylist2s("goto", dysymbol(s)));
	}
	if(dyxTagIsP(l, "import"))
	{
		s=dyxGet(l, "name");
		return(dylist2s("import", dysymbol(s)));
	}
	if(dyxTagIsP(l, "using"))
	{
		s=dyxGet(l, "name");
		return(dylist2s("using", dysymbol(s)));
	}

	if(dyxTagIsP(l, "return"))
	{
		n0=dyxFetch(l, "expr");
		t=BSVMC_TranslateGastExpr(ctx, n0);
		return(dylist2s("return", t));
	}

	if(dyxTagIsP(l, "case"))
	{
		n0=dyxFetch(l, "value");
		t=BSVMC_TranslateGastExpr(ctx, n0);
		return(dylist2s("case", t));
	}

	if(dyxTagIsP(l, "throw"))
	{
		n0=dyxFetch(l, "value");
		t=BSVMC_TranslateGastExpr(ctx, n0);
		return(dylist2s("throw", t));
	}

	if(dyxTagIsP(l, "get"))
	{
		n0=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastStmtList(ctx, n0);
		return(dyconss("get", t0));
	}

	if(dyxTagIsP(l, "set"))
	{
		n0=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastStmtList(ctx, n0);
		return(dyconss("set", t0));
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
			return(dylist3s("if", t0, t1));
		}
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmt(ctx, n1);
		t2=BSVMC_TranslateGastStmt(ctx, n2);
		return(dylist4s("if", t0, t1, t2));
	}

	if(dyxTagIsP(l, "begin"))
	{
		n0=dyxChild(l); lst=NULL;
		while(n0)
		{
			t=BSVMC_TranslateGastStmt(ctx, n0);
			if(t) lst=dynappend(lst, dylist1(t));
			n0=dyxNext(n0);
		}
		return(dyconss("begin", lst));
	}

	if(dyxTagIsP(l, "vars"))
	{
		n0=dyxChild(l); lst=NULL;
		while(n0)
		{
			t=dysymbol(dyxGet(n0, "name"));
			n1=dyxFetch(n0, "value");
			if(n1)
			{
				t1=BSVMC_TranslateGastExpr(ctx, n1);
				t=dylist2(t, t1);
			}
			if(t) lst=dynappend(lst, dylist1(t));
			n0=dyxNext(n0);
		}
		return(dyconss("vars", lst));
	}

	if(dyxTagIsP(l, "switch"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmtList(ctx, n1);
		return(dycons2s("switch", t0, t1));
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
		return(dycons4s("for", t0, t1, t2, t3));
	}

	if(dyxTagIsP(l, "for_in"))
	{
		n0=dyxFetch(l, "left");
		n1=dyxFetch(l, "right");
		n3=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastExpr(ctx, n1);
		t3=BSVMC_TranslateGastStmtList(ctx, n3);
		return(dycons3s("for_in", t0, t1, t3));
	}

	if(dyxTagIsP(l, "while"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmtList(ctx, n1);
		return(dycons2s("while", t0, t1));
	}

	if(dyxTagIsP(l, "do_while"))
	{
		n0=dyxFetch(l, "cond");
		n1=dyxFetch(l, "body");
		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t1=BSVMC_TranslateGastStmtList(ctx, n1);
		return(dycons2s("do_while", t0, t1));
	}

	if(dyxTagIsP(l, "defun"))
	{
		n0=dyxFetch(l, "args"); lst=NULL;
		while(n0)
		{
			t=dysymbol(dyxGet(n0, "name"));
			n1=dyxFetch(n0, "value");
			if(n1)
			{
				t1=BSVMC_TranslateGastExpr(ctx, n1);
				t=dylist2(t, t1);
			}
			if(t) lst=dynappend(lst, dylist1(t));
			n0=dyxNext(n0);
		}

		n0=dyxFetch(l, "type");
		n1=dyxFetch(l, "body");
//		t0=BSVMC_TranslateGastExpr(ctx, n0);
		t0=dysymbol("var");
		t1=dysymbol(dyxGet(l, "name"));
		t2=lst;
		
		t3=BSVMC_TranslateGastStmtList(ctx, n1);
		//(defun type name args body*)
		return(dycons4s("defun", t0, t1, t2, t3));
	}

	return(BSVMC_TranslateGastExpr(ctx, l));
}

BGBSVM_API dyt BSVMC_CompileGast(BSVM_SVMCState *ctx, dyxNode *mod)
{
	dyxNode *cur;
	dyt lst, t;

//	mod=DYX_SBXE_ReadNodeList(data, sz);

	dyxPrint(mod);
	printf("\n");

	cur=dyxFetch(mod, "body"); lst=NULL;
	while(cur)
	{
		t=BSVMC_TranslateGastStmt(ctx, cur);
		if(t) lst=dynappend(lst, dylist1(t));
		cur=dyxNext(cur);
	}
	
	t=BSVMC_CompileBlock(ctx, NULL, NULL, lst);
	return(t);
	
//	return(0);
}

BGBSVM_API dyt BSVMC_CompileGastSBXE(
	BSVM_SVMCState *ctx, byte *data, int sz)
{
	dyxNode *mod;
	dyt lst, t;

	mod=DYX_SBXE_ReadNodeList(data, sz);
//	dyxPrint(mod);
	t=BSVMC_CompileGast(ctx, mod);
//	dyxFreeList(mod);
	return(t);
}
