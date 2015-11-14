#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bgbsvm.h>
#include <math.h>

#ifndef M_E
#define M_E		2.7182818284590452354
#endif

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

dytf bsvm_abs(BSVM_SVMState *ctx, dytf args)
{
	return(dytfReal(fabs(dytfRealv(dytfCar(args)))));
}

dytf bsvm_fabs(BSVM_SVMState *ctx, dytf args)
{
	return(dytfReal(fabs(dytfRealv(dytfCar(args)))));
}

dytf bsvm_sqr(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=f*f;
	return(dytfReal(f));
}

dytf bsvm_sqrt(BSVM_SVMState *ctx, dytf args)
{
	dcomplex ca;
	dytf a;
	float f0, g0, f, g, h;

	a=dytfCar(args);
	if(dycomplexp(dytfUnwrap(a)))
	{
//		f0=creal(dycomplexv(a));
//		g0=cimag(dycomplexv(a));

		ca=dycomplexv(dytfUnwrap(a));
		f0=ca.r; g0=ca.i;

		g=sqrt(f0*f0+g0*g0);
		h=atan2(g0, f0);
		f=pow(g, 0.5)*cos(0.5*h);
		g=pow(g, 0.5)*sin(0.5*h);
		return(dytfWrap(dycomplex2(f, g)));
	}

	f=dytfRealv(a);
	if(f<0)return(dytfWrap(dycomplex2(0, sqrt(-f))));
	return(dytfReal(sqrt(f)));
}

dytf bsvm_ssqr(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=(f>=0)?(f*f):(-(f*f));
	return(dytfReal(f));
}

dytf bsvm_ssqrt(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=(f>=0)?sqrt(f):-sqrt(-f);
	return(dytfReal(f));
}

dytf bsvm_spow(BSVM_SVMState *ctx, dytf args)
{
	float f, g;
	f=dytfRealv(dytfCar(args));
	g=dytfRealv(dytfCadr(args));
//	f=pow(f, g);
	f=(f>=0)?pow(f, g):-pow(-f, g);
	return(dytfReal(f));
}


dytf bsvm_floor(BSVM_SVMState *ctx, dytf args)
{
	return(dytfReal(floor(dytfRealv(dytfCar(args)))));
}

dytf bsvm_ceil(BSVM_SVMState *ctx, dytf args)
{
	return(dytfReal(ceil(dytfRealv(dytfCar(args)))));
}

dytf bsvm_round(BSVM_SVMState *ctx, dytf args)
{
	return(dytfReal(floor(dytfRealv(dytfCar(args))+0.5)));
}

dytf bsvm_truncate(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f-=(int)f;
	return(dytfReal(f));
}

dytf bsvm_pow(BSVM_SVMState *ctx, dytf args)
{
//	return(BSVM_TYVec_ExpComplex(dytfCar(args), dytfCadr(args)));
//	return(dyfcomplex(cpow(dycomplexv(dytfCar(args)), dycomplexv(dytfCadr(args)))));

	float f, g;
	f=dytfRealv(dytfCar(args));
	g=dytfRealv(dytfCadr(args));
	f=pow(f, g);
	return(dytfReal(f));
}

dytf bsvm_degrees(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f*=180/M_PI;
	return(dytfReal(f));
}

dytf bsvm_radians(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f*=M_PI/180;
	return(dytfReal(f));
}

dytf bsvm_cos(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=cos(dytfRealv(dytfCar(args)));
	return(dytfReal(f));
}

dytf bsvm_sin(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=sin(dytfRealv(dytfCar(args)));
	return(dytfReal(f));
}

dytf bsvm_tan(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=tan(dytfRealv(dytfCar(args)));
	return(dytfReal(f));
}

dytf bsvm_cosd(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=cos(dytfRealv(dytfCar(args))*(M_PI/180));
	return(dytfReal(f));
}

dytf bsvm_sind(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=sin(dytfRealv(dytfCar(args))*(M_PI/180));
	return(dytfReal(f));
}

dytf bsvm_tand(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=tan(dytfRealv(dytfCar(args))*(M_PI/180));
	return(dytfReal(f));
}

dytf bsvm_acos(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=acos(f);
	return(dytfReal(f));
}

dytf bsvm_asin(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=asin(f);
	return(dytfReal(f));
}

dytf bsvm_atan(BSVM_SVMState *ctx, dytf args)
{
	float f, g;
	if(dytfEqqP(dytfCdr(args), DYTF_NULL))
	{
		f=dytfRealv(dytfCar(args));
		f=atan(f);
	}else
	{
		f=dytfRealv(dytfCar(args));
		g=dytfRealv(dytfCadr(args));
		f=atan2(f, g);
	}
	return(dytfReal(f));
}

dytf bsvm_atan2(BSVM_SVMState *ctx, dytf args)
{
	float f, g;

	f=dytfRealv(dytfCar(args));
	g=dytfRealv(dytfCadr(args));
	f=atan2(f, g);
	return(dytfReal(f));
}

dytf bsvm_ln(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=log(f);
	return(dytfReal(f));
}

dytf bsvm_log(BSVM_SVMState *ctx, dytf args)
{
	float f, g;

	f=dytfRealv(dytfCar(args));

	g=M_E;
	if(dytfNEqqP(dytfCdr(args), DYTF_NULL))g=dytfRealv(dytfCadr(args));
	f=log(f)/log(g);
	return(dytfReal(f));
}

dytf bsvm_log2(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=log(f)/log(2);
	return(dytfReal(f));
}

dytf bsvm_log10(BSVM_SVMState *ctx, dytf args)
{
	float f;
	f=dytfRealv(dytfCar(args));
	f=log10(f);
	return(dytfReal(f));
}

dytf bsvm_real(BSVM_SVMState *ctx, dytf args)
	{ return(dytfReal(dycreal(dytfUnwrap(dytfCar(args))))); }

dytf bsvm_imag(BSVM_SVMState *ctx, dytf args)
	{ return(dytfReal(dycimag(dytfUnwrap(dytfCar(args))))); }


int BSVM_BuiltinMath_Init()
{
	static int init=0;

	if(init)return(0);
	init=1;

	BSVM_AddBinding("math.PI", dytfReal(M_PI));
	BSVM_AddBinding("math.TAU", dytfReal(M_TAU));
	BSVM_AddBinding("math.E", dytfReal(M_E));
	BSVM_AddBinding("math.I", dytfWrap(dycomplex2(0, 1)));
//	BSVM_AddBinding("math.CJ", PDSCR_Maths_MakeComplex2(0, 0, 1));
//	BSVM_AddBinding("math.CK", PDSCR_Maths_MakeComplex3(0, 0, 0, 1));

	BSVM_AddBuiltin("math.sqrt",
		"(x): square root",
		&bsvm_sqrt, 1);
	BSVM_AddBuiltin("math.floor",
		"(x): round down",
		&bsvm_floor, 1);
	BSVM_AddBuiltin("math.ceil",
		"(x): round up",
		&bsvm_ceil, 1);
	BSVM_AddBuiltin("math.round",
		"(x): round nearest",
		&bsvm_round, 1);
	BSVM_AddBuiltin("math.truncate",
		"(x): fractional part",
		&bsvm_truncate, 1);
	BSVM_AddBuiltin("math.pow",
		"(x, y): raise x to y",
		&bsvm_pow, 2);

	BSVM_AddBuiltin("math.degrees",
		"(x): radians to degrees",
		&bsvm_degrees, 1);
	BSVM_AddBuiltin("math.radians",
		"(x): degrees to radians",
		&bsvm_radians, 1);

	BSVM_AddBuiltin("math.abs", "(x): cosine", &bsvm_abs, 1);
	BSVM_AddBuiltin("math.fabs", "(x): cosine", &bsvm_fabs, 1);
	BSVM_AddBuiltin("math.sqr", "(x): square", &bsvm_sqr, 1);

	BSVM_AddBuiltin("math.ssqr", "(x): signed square", &bsvm_ssqr, 1);
	BSVM_AddBuiltin("math.ssqrt", "(x): signed square root", &bsvm_ssqrt, 1);
	BSVM_AddBuiltin("math.spow", "(f, g): signed power", &bsvm_spow, 2);

	BSVM_AddBuiltin("math.cos", "(x): cosine",
		&bsvm_cos, 1);
	BSVM_AddBuiltin("math.sin", "(x): sine",
		&bsvm_sin, 1);
	BSVM_AddBuiltin("math.tan", "(x): tangent",
		&bsvm_tan, 1);

	BSVM_AddBuiltin("math.cos_d", "(x): cosine degrees",
		&bsvm_cosd, 1);
	BSVM_AddBuiltin("math.sin_d", "(x): sine degrees",
		&bsvm_sind, 1);
	BSVM_AddBuiltin("math.tan_d", "(x): tangent degrees",
		&bsvm_tand, 1);

	BSVM_AddBuiltin("math.acos", "(x): arc cosine",
		&bsvm_acos, 1);
	BSVM_AddBuiltin("math.asin", "(x): arc sine",
		&bsvm_asin, 1);
	BSVM_AddBuiltin("math.atan", "(f)|(f, g): arc tangent",
		&bsvm_atan, -2);
	BSVM_AddBuiltin("math.atan2", "(f, g): arc tangent",
		&bsvm_atan, -2);

	BSVM_AddBuiltin("math.pow", "(f, g): power", &bsvm_pow, 2);

	BSVM_AddBuiltin("math.ln", "(x): natural log",
		&bsvm_ln, 1);
	BSVM_AddBuiltin("math.log", "(x[, b]):logarithm",
		&bsvm_log, -2);
	BSVM_AddBuiltin("math.log2", "(x): log 2",
		&bsvm_log2, 1);
	BSVM_AddBuiltin("math.log10", "(x): log 10",
		&bsvm_log10, 1);

	BSVM_AddBuiltin("math.real", "(x): real portion of a complex",
		&bsvm_real, 1);
	BSVM_AddBuiltin("math.imag", "(x): imaginary portion of a complex",
		&bsvm_imag, 1);

	return(1);
}


BGBSVM_API double BSAPI_Math_E=2.7182818284590452354;
BGBSVM_API double BSAPI_Math_LN10=2.302585092994046;
BGBSVM_API double BSAPI_Math_LN2=0.6931471805599453;
BGBSVM_API double BSAPI_Math_LOG2E=1.4426950408889634;
BGBSVM_API double BSAPI_Math_LOG10E=0.4342944819032518;
BGBSVM_API double BSAPI_Math_PI=3.1415926535897932384626433832795;
BGBSVM_API double BSAPI_Math_SQRT1_2=0.7071067811865476;
BGBSVM_API double BSAPI_Math_SQRT2=1.4142135623730951;

BGBSVM_API double BSAPI_Math_abs(double x)
	{ return(fabs(x)); }
BGBSVM_API double BSAPI_Math_acos(double x)
	{ return(acos(x)); }
BGBSVM_API double BSAPI_Math_asin(double x)
	{ return(asin(x)); }
BGBSVM_API double BSAPI_Math_atan(double x)
	{ return(atan(x)); }
BGBSVM_API double BSAPI_Math_atan2(double y, double x)
	{ return(atan2(y, x)); }
BGBSVM_API double BSAPI_Math_ceil(double x)
	{ return(ceil(x)); }
BGBSVM_API double BSAPI_Math_cos(double x)
	{ return(cos(x)); }
BGBSVM_API double BSAPI_Math_exp(double x)
	{ return(exp(x)); }
BGBSVM_API double BSAPI_Math_floor(double x)
	{ return(floor(x)); }
BGBSVM_API double BSAPI_Math_log(double x)
	{ return(log(x)); }
BGBSVM_API double BSAPI_Math_log2(double x)
	{ return(log(x)/log(2)); }
BGBSVM_API double BSAPI_Math_pow(double x, double y)
	{ return(pow(x, y)); }
BGBSVM_API double BSAPI_Math_round(double x)
	{ return(floor(x+0.5)); }
BGBSVM_API double BSAPI_Math_sin(double x)
	{ return(sin(x)); }
BGBSVM_API double BSAPI_Math_sqr(double x)
	{ return(x*x); }
BGBSVM_API double BSAPI_Math_sqrt(double x)
	{ return(sqrt(x)); }
BGBSVM_API double BSAPI_Math_cbrt(double x)
	{ return(pow(x, 1.0/3.0)); }
BGBSVM_API double BSAPI_Math_tan(double x)
	{ return(tan(x)); }

BGBSVM_API double BSAPI_Math_ssqr(double x)
	{ return((x>=0)?(x*x):(-x*x)); }
BGBSVM_API double BSAPI_Math_ssqrt(double x)
	{ return((x>=0)?sqrt(x):-sqrt(-x)); }
BGBSVM_API double BSAPI_Math_spow(double x, double y)
	{ return((x>=0)?pow(x, y):-pow(-x, y)); }

BGBSVM_API double BSAPI_Math_min(double x, double y)
	{ return((x<=y)?x:y); }
BGBSVM_API double BSAPI_Math_max(double x, double y)
	{ return((x>=y)?x:y); }

BGBSVM_API double BSAPI_Math_random()
	{ return(bgbrng_randf()); }
