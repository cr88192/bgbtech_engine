#include "bggl3.h"
#include <math.h>

int bggl3_id_int, bggl3_id_float, bggl3_id_cons, bggl3_id_null;
int bggl3_id_symbol, bggl3_id_string, bggl3_id_complex;
int bggl3_id_fvector;

elem bggl3_print(BGGL3_State *ctx, elem args)
{
	elem a, c;
	int i;

	c=args;
	while(c)
	{
		a=BGGL3_CAR(c);

		i=BGGL3_GetTypeID(a);
		if(i==bggl3_id_int)printf("%d", BGGL3_TOINT(a));
		if(i==bggl3_id_float)printf("%g", BGGL3_TOFLOAT(a));
		if(i==bggl3_id_string)printf("%s", a);
		if(i==bggl3_id_symbol)printf("%s", a);
		if(i==bggl3_id_cons)BGGL3_Print(a);
		if(i==bggl3_id_complex)BGGL3_Print(a);
		if(i==bggl3_id_fvector)BGGL3_Print(a);

		c=BGGL3_CDR(c);
		if(c)printf(" ");
	}
	return(NULL);
}

elem bggl3_println(BGGL3_State *ctx, elem args)
{
	bggl3_print(ctx, args);
	printf("\n");
	return(NULL);
}


elem bggl3_sqrt(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));

	if(f<0)
	{
		return(BGGL3_COMPLEX(0, sqrt(-f)));
	}else
	{
		return(BGGL3_FLOAT(sqrt(f)));
	}
}

elem bggl3_log2(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(log2(f)));
}

elem bggl3_log10(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(log10(f)));
}

elem bggl3_ln(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(log(f)));
}

elem bggl3_log(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_FLOAT(log(g)/log(f)));
}

elem bggl3_floor(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(floor(f)));
}

elem bggl3_ceil(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(ceil(f)));
}

elem bggl3_round(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(round(f)));
}



elem bggl3_sin(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(sin(f)));
}

elem bggl3_cos(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(cos(f)));
}

elem bggl3_tan(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(tan(f)));
}

elem bggl3_asin(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(asin(f)));
}

elem bggl3_acos(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(acos(f)));
}

elem bggl3_atan(BGGL3_State *ctx, elem args)
{
	float f;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	return(BGGL3_FLOAT(atan(f)));
}

elem bggl3_atan2(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_FLOAT(atan2(f, g)));
}


elem bggl3_real(BGGL3_State *ctx, elem args)
{
	return(BGGL3_FLOAT(BGGL3_GETREAL(BGGL3_CAR(args))));
}

elem bggl3_imag(BGGL3_State *ctx, elem args)
{
	return(BGGL3_FLOAT(BGGL3_GETIMAG(BGGL3_CAR(args))));
}

elem bggl3_abs(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_GETREAL(BGGL3_CAR(args));
	g=BGGL3_GETIMAG(BGGL3_CAR(args));
	return(BGGL3_FLOAT(sqrt(f*f+g*g)));
}

elem bggl3_angle(BGGL3_State *ctx, elem args)
{
	float f, g;

	if(BGGL3_COMPLEXP(BGGL3_CAR(args)))
	{
		f=BGGL3_GETREAL(BGGL3_CAR(args));
		g=BGGL3_GETIMAG(BGGL3_CAR(args));
		return(BGGL3_FLOAT(atan2(g, f)));
	}
	return(NULL);
}

elem bggl3_conj(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_GETREAL(BGGL3_CAR(args));
	g=BGGL3_GETIMAG(BGGL3_CAR(args));
	return(BGGL3_COMPLEX(f, -g));
}

elem bggl3_fact(BGGL3_State *ctx, elem args)
{
	float f, g;
	int i;

	i=BGGL3_TOINT(BGGL3_CAR(args));
	f=1; while(i>1) { f*=i; i--; }
	return(BGGL3_FLOAT(f));
}

elem bggl3_graph(BGGL3_State *ctx, elem args);

elem bggl3_load(BGGL3_State *ctx, elem args)
{
	elem a;
	a=BGGL3_LoadFile(ctx, BGGL3_CAR(args));
	return(a);
}

elem bggl3_dumpmesh(BGGL3_State *ctx, elem args)
{
	FILE *fd;
	elem a, c;
	float *fa, *fb, *fc;

	fd=fopen(BGGL3_CAR(args), "wt");
	if(!fd)return(NULL);

	c=BGGL3_CADR(args);
	while(c)
	{
		a=BGGL3_CAR(c);
		fa=(float *)BGGL3_CAR(a);
		fb=(float *)BGGL3_CADR(a);
		fc=(float *)BGGL3_CADDR(a);

		fprintf(fd, "%f %f %f  %f %f %f  %f %f %f 0xFFFFFF\n",
			fa[0], fa[1], fa[2], fb[0], fb[1], fb[2],
			fc[0], fc[1], fc[2]);

		c=BGGL3_CDR(c);
	}

	fclose(fd);
	return(NULL);
}

void BGGL3_InitBuiltin()
{
	BGGL3_BindValue("PI", "PI", BGGL3_FLOAT(M_PI));
	BGGL3_BindValue("E", "E", BGGL3_FLOAT(M_E));
	BGGL3_BindValue("I", "Complex i", BGGL3_COMPLEX(0, 1));

	BGGL3_AddBuiltin("print", "print stuff", &bggl3_print, -1);
	BGGL3_AddBuiltin("println", "print line", &bggl3_println, -1);

	BGGL3_InitBuiltinOpr();
	BGGL3_InitBuiltinVector();

	BGGL3_AddBuiltin("sqrt", "square root", &bggl3_sqrt, 1);
	BGGL3_AddBuiltin("log2", "log 2", &bggl3_log2, 1);
	BGGL3_AddBuiltin("log10", "log 10", &bggl3_log10, 1);
	BGGL3_AddBuiltin("ln", "log e", &bggl3_ln, 1);
	BGGL3_AddBuiltin("log", "log x", &bggl3_log, 2);

	BGGL3_AddBuiltin("floor", "floor x", &bggl3_floor, 1);
	BGGL3_AddBuiltin("ceil", "ceil x", &bggl3_ceil, 1);
	BGGL3_AddBuiltin("round", "round x", &bggl3_round, 1);

	BGGL3_AddBuiltin("sin", "sine", &bggl3_sin, 1);
	BGGL3_AddBuiltin("cos", "cosine", &bggl3_cos, 1);
	BGGL3_AddBuiltin("tan", "tangent", &bggl3_tan, 1);
	BGGL3_AddBuiltin("asin", "arc sine", &bggl3_asin, 1);
	BGGL3_AddBuiltin("acos", "arc cosine", &bggl3_acos, 1);
	BGGL3_AddBuiltin("atan", "arc tangent", &bggl3_atan, 1);
	BGGL3_AddBuiltin("atan2", "arc tangent", &bggl3_atan2, 2);

	BGGL3_AddBuiltin("real", "real part", &bggl3_real, 1);
	BGGL3_AddBuiltin("imag", "imaginary part", &bggl3_imag, 1);
	BGGL3_AddBuiltin("abs", "abs value/complex arg", &bggl3_abs, 1);
	BGGL3_AddBuiltin("angle", "complex phase angle", &bggl3_angle, 1);
	BGGL3_AddBuiltin("conj", "complex conjugate", &bggl3_conj, 1);

	BGGL3_AddBuiltin("fact", "factorial", &bggl3_fact, 1);

	BGGL3_AddBuiltin("load", "load script", &bggl3_load, 1);

	BGGL3_AddBuiltin("graph", "graph function", &bggl3_graph, 6);
	BGGL3_AddBuiltin("dumpmesh", "dump tri mesh", &bggl3_dumpmesh, 2);
}
