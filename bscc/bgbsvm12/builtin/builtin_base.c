#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgbsvm.h>

dytf bsvm_print(BSVM_SVMState *ctx, dytf args)
{
	dytf c;
	dyt t;

	c=args;
	while(dytfConsP(c))
	{
		t=dytfUnwrap(dytfCar(c));
		if(dystringp(t))dyPrintf("%s", dystringv(t));
		if(dysymbolp(t))dyPrintf("%s", dysymbolv(t));
		if(dykeywordp(t))dyPrintf("%s", dysymbolv(t));
//		if(dyintp(t))dyPrintf("%d", dyintv(t));
		if(dyrealp(t))dyPrintf("%g", dyrealv(t));
//		if(dyfloatp(t))dyPrintf("%g", dyfloatv(t));
//		if(BSVM_CHARP(t))dyPrintf("%c", TOINT(t));

//		if(dyconsp(dytfCar(c)))BSVM_TY_Print(dytfCar(c));
		if(dytfConsP(dytfCar(c)))dyPrint(dytfUnwrap(dytfCar(c)));

//		if(BSVM_FVECTORP(dytfCar(c)))dyPrintf("%s", BSVM_S_PrintS(dytfCar(c)));
//		if(BSVM_FMATRIXP(dytfCar(c)))dyPrintf("%s", BSVM_S_PrintS(dytfCar(c)));

		c=dytfCdr(c);
	}

	return(DYTF_NULL);
}

dytf bsvm_println(BSVM_SVMState *ctx, dytf args)
{
	dytf t;

	t=bsvm_print(ctx, args);
	printf("\n");
	return(t);
}

dytf bsvm_load(BSVM_SVMState *ctx, dytf args)
{
	VFILE *fd;
	dytf t0, t1, t2;
	char *s, *buf;
	int sz;

	if(!dytfConsP(args))
		return(DYTF_NULL);

	t0=dytfCar(args); t1=ctx->self;
	if(dytfConsP(dytfCdr(args)))
		t1=dytfCadr(args);

	s=dystringv(dytfUnwrap(t0));
	t2=dytfWrap(BSVM_LoadScript(s, dytfUnwrap(t1)));
	return(t2);
}

dytf bsvm_eval(BSVM_SVMState *ctx, dytf args)
{
	FILE *fd;
	dytf t0, t1, t2;
	char *s, *buf;
	int sz;

	if(!dytfConsP(args))
		return(DYTF_NULL);

	t0=dytfCar(args); t1=ctx->self;
	if(dytfConsP(dytfCdr(args)))
		t1=dytfCadr(args);

	if(dystringp(dytfUnwrap(t0)))
	{
		s=dystringv(dytfUnwrap(t0));
		t2=dytfWrap(BSVM_EvalStringObj(s, dytfUnwrap(t1)));
	}else
	{
		t2=dytfWrap(BSVM_EvalExprsObj(dylist1(dytfUnwrap(t0)), dytfUnwrap(t1)));
	}
	return(t2);
}

// dytf bsvm_car(BSVM_SVMState *ctx, dytf args) { return(dytfCar(dytfCar(args))); }
// dytf bsvm_cdr(BSVM_SVMState *ctx, dytf args)	{ return(dytfCdr(dytfCar(args))); }

// dytf bsvm_caar(BSVM_SVMState *ctx, dytf args) { return(dycaar(dytfCar(args))); }
// dytf bsvm_cdar(BSVM_SVMState *ctx, dytf args) { return(dycdar(dytfCar(args))); }
// dytf bsvm_cadr(BSVM_SVMState *ctx, dytf args) { return(dycadr(dytfCar(args))); }
// dytf bsvm_cddr(BSVM_SVMState *ctx, dytf args) { return(dycddr(dytfCar(args))); }

// dytf bsvm_caaar(BSVM_SVMState *ctx, dytf args) { return(dycaaar(dytfCar(args))); }
// dytf bsvm_cdaar(BSVM_SVMState *ctx, dytf args) { return(dycdaar(dytfCar(args))); }
// dytf bsvm_cadar(BSVM_SVMState *ctx, dytf args) { return(dycadar(dytfCar(args))); }
// dytf bsvm_cddar(BSVM_SVMState *ctx, dytf args) { return(dycddar(dytfCar(args))); }
// dytf bsvm_caadr(BSVM_SVMState *ctx, dytf args) { return(dycaadr(dytfCar(args))); }
// dytf bsvm_cdadr(BSVM_SVMState *ctx, dytf args) { return(dycdadr(dytfCar(args))); }
// dytf bsvm_caddr(BSVM_SVMState *ctx, dytf args) { return(dycaddr(dytfCar(args))); }
// dytf bsvm_cdddr(BSVM_SVMState *ctx, dytf args) { return(dycdddr(dytfCar(args))); }


//dytf bsvm_heapused(BSVM_SVMState *ctx, dytf args)
//	{ return(FIXNUM(BSVM_MM_HeapUsed())); }
//dytf bsvm_heapfree(BSVM_SVMState *ctx, dytf args)
//	{ return(FIXNUM(BSVM_MM_HeapFree())); }
//dytf bsvm_heapsize(BSVM_SVMState *ctx, dytf args)
//	{ return(FIXNUM(BSVM_MM_HeapSize())); }

//dytf bsvm_symsused(BSVM_SVMState *ctx, dytf args)
//	{ return(FIXNUM(BSVM_TY_SymsUsed())); }
//dytf bsvm_symsfree(BSVM_SVMState *ctx, dytf args)
//	{ return(FIXNUM(BSVM_TY_SymsMax()-BSVM_TY_SymsUsed())); }
//dytf bsvm_symsmax(BSVM_SVMState *ctx, dytf args)
//	{ return(FIXNUM(BSVM_TY_SymsMax())); }

int BSVM_Builtins_Init()
{
//	BSVM_AddBinding("quit", dykeyword("quit"));

	BSVM_AddBuiltin("print", "", &bsvm_print, -1);
	BSVM_AddBuiltin("println", "", &bsvm_println, -1);
	BSVM_AddBuiltin("load", "", &bsvm_load, -2);
	BSVM_AddBuiltin("eval", "", &bsvm_eval, -2);

//	BSVM_AddBuiltin("car", "", &bsvm_car, 1);
//	BSVM_AddBuiltin("cdr", "", &bsvm_cdr, 1);
//	BSVM_AddBuiltin("caar", "", &bsvm_caar, 1);
//	BSVM_AddBuiltin("cdar", "", &bsvm_cdar, 1);
//	BSVM_AddBuiltin("cadr", "", &bsvm_cadr, 1);
//	BSVM_AddBuiltin("cddr", "", &bsvm_cddr, 1);
//	BSVM_AddBuiltin("caaar", "", &bsvm_caaar, 1);
//	BSVM_AddBuiltin("cdaar", "", &bsvm_cdaar, 1);
//	BSVM_AddBuiltin("cadar", "", &bsvm_cadar, 1);
//	BSVM_AddBuiltin("cddar", "", &bsvm_cddar, 1);
//	BSVM_AddBuiltin("caadr", "", &bsvm_caadr, 1);
//	BSVM_AddBuiltin("cdadr", "", &bsvm_cdadr, 1);
//	BSVM_AddBuiltin("caddr", "", &bsvm_caddr, 1);
//	BSVM_AddBuiltin("cdddr", "", &bsvm_cdddr, 1);

//	BSVM_AddBuiltin("heap_used", "", &bsvm_heapused, 0);
//	BSVM_AddBuiltin("heap_free", "", &bsvm_heapfree, 0);
//	BSVM_AddBuiltin("heap_size", "", &bsvm_heapsize, 0);
//	BSVM_AddBuiltin("syms_used", "", &bsvm_symsused, 0);
//	BSVM_AddBuiltin("syms_free", "", &bsvm_symsfree, 0);
//	BSVM_AddBuiltin("syms_max", "", &bsvm_symsmax, 0);

	BSVM_BuiltinMath_Init();

	return(0);
}
