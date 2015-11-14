#include <bgbgc.h>
#include <bgbdy.h>

#include <math.h>

BGBDY_API dytf dytfAdd(dytf a, dytf b)
{
	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfRealvF(a)+dytfRealvF(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfReal(dytfRealvF(a)+dytfRealv(b))); }
	}else if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntvF(a)+dytfIntvF(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfReal(dytfRealv(a)+dytfRealvF(b))); }
	}
	return(dytfWrap(dyadd(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfSub(dytf a, dytf b)
{
	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfRealvF(a)-dytfRealvF(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfReal(dytfRealvF(a)-dytfRealv(b))); }
	}else if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)-dytfIntv(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfReal(dytfRealv(a)-dytfRealvF(b))); }
	}
	return(dytfWrap(dysub(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfMul(dytf a, dytf b)
{
	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfRealvF(a)*dytfRealvF(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfReal(dytfRealvF(a)*dytfRealv(b))); }
	}else if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)*dytfIntv(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfReal(dytfRealv(a)*dytfRealvF(b))); }
	}
	return(dytfWrap(dymul(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfDiv(dytf a, dytf b)
{
	dytf c;

	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
		{
			c=dytfReal(dytfRealvF(a)/dytfRealvF(b));
			return(c);
		}else if(dytfFixnumP(b))
		{
			c=dytfReal(dytfRealvF(a)/dytfRealv(b));
			return(c);
		}
	}else if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
		{
			c=dytfInt(dytfIntv(a)/dytfIntv(b));
			return(c);
		}else if(dytfFlonumP(b))
		{
			c=dytfReal(dytfRealv(a)/dytfRealvF(b));
			return(c);
		}
	}

	c=dytfWrap(dydiv(dytfUnwrap(a), dytfUnwrap(b)));
	return(c);
}

BGBDY_API dytf dytfMod(dytf a, dytf b)
{
	dytf c;

	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
		{
			c=dytfReal(fmod(dytfRealvF(a), dytfRealvF(b)));
			return(c);
		}else if(dytfFixnumP(b))
		{
			c=dytfReal(fmod(dytfRealvF(a), dytfRealv(b)));
			return(c);
		}
	}else if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
		{
			c=dytfInt(dytfIntv(a)%dytfIntv(b));
			return(c);
		}else if(dytfFlonumP(b))
		{
			c=dytfReal(fmod(dytfRealv(a), dytfRealvF(b)));
			return(c);
		}
	}

	c=dytfWrap(dymod(dytfUnwrap(a), dytfUnwrap(b)));
	return(c);
}

BGBDY_API dytf dytfAnd(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)&dytfIntv(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfInt(dytfIntv(a)&dytfIntv(b))); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfIntv(a)&dytfIntv(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)&dytfIntv(b))); }
	}
	return(dytfWrap(dyand(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfOr(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)|dytfIntv(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfInt(dytfIntv(a)|dytfIntv(b))); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfIntv(a)|dytfIntv(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)|dytfIntv(b))); }
	}
	return(dytfWrap(dyor(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfXor(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)^dytfIntv(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfInt(dytfIntv(a)^dytfIntv(b))); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfIntv(a)^dytfIntv(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)^dytfIntv(b))); }
	}
	return(dytfWrap(dyxor(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfShl(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)<<dytfIntv(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfInt(dytfIntv(a)<<dytfIntv(b))); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfIntv(a)<<dytfIntv(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)<<dytfIntv(b))); }
	}
	return(dytfWrap(dyshl(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfShr(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)>>dytfIntv(b))); }
		else if(dytfFlonumP(b))
			{ return(dytfInt(dytfIntv(a)>>dytfIntv(b))); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfReal(dytfIntv(a)>>dytfIntv(b))); }
		else if(dytfFixnumP(b))
			{ return(dytfInt(dytfIntv(a)>>dytfIntv(b))); }
	}
	return(dytfWrap(dyshr(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfExp(dytf a, dytf b)
{
	dytf c;

	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
		{
			c=dytfReal(pow(dytfRealvF(a), dytfRealvF(b)));
			return(c);
		}else if(dytfFixnumP(b))
		{
			c=dytfReal(pow(dytfRealvF(a), dytfRealv(b)));
			return(c);
		}
	}else if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
		{
			c=dytfInt(pow(dytfIntv(a), dytfIntv(b)));
			return(c);
		}else if(dytfFlonumP(b))
		{
			c=dytfReal(pow(dytfRealv(a), dytfRealvF(b)));
			return(c);
		}
	}

	c=dytfWrap(dyexp(dytfUnwrap(a), dytfUnwrap(b)));
	return(c);
}

BGBDY_API dytf dytfNeg(dytf a)
{
	if(dytfFlonumP(a))
		{ return(dytfReal(-dytfRealvF(a))); }
	else if(dytfFixnumP(a))
		{ return(dytfInt(-dytfIntv(a))); }
	return(dytfWrap(dyneg(dytfUnwrap(a))));
}

BGBDY_API dytf dytfNot(dytf a)
{
	if(dytfFlonumP(a))
		{ return(dytfReal(~dytfIntv(a))); }
	else if(dytfFixnumP(a))
		{ return(dytfInt(~dytfIntv(a))); }
	return(dytfWrap(dynot(dytfUnwrap(a))));
}

BGBDY_API dytf dytfLNot(dytf a)
{
	if(dytfFlonumP(a))
		{ return(dytfReal(!dytfIntv(a))); }
	else if(dytfFixnumP(a))
		{ return(dytfInt(!dytfIntv(a))); }
	return(dytfWrap(dylnot(dytfUnwrap(a))));
}

BGBDY_API dytf dytfUMul(dytf a)
{
	return(dytfWrap(dyumul(dytfUnwrap(a))));
}

BGBDY_API dytf dytfUDiv(dytf a)
{
	return(dytfWrap(dyudiv(dytfUnwrap(a))));
}

BGBDY_API dytf dytfUMod(dytf a)
{
	return(dytfWrap(dyumod(dytfUnwrap(a))));
}

BGBDY_API dytf dytfUAnd(dytf a)
{
	return(dytfWrap(dyuand(dytfUnwrap(a))));
}

BGBDY_API dytf dytfUOr(dytf a)
{
	return(dytfWrap(dyuor(dytfUnwrap(a))));
}

BGBDY_API dytf dytfUXor(dytf a)
{
	return(dytfWrap(dyuxor(dytfUnwrap(a))));
}

BGBDY_API dytf dytfSin(dytf a)
{
	if(dytfFixrealP(a))
		{ return(dytfReal(sin(dytfRealv(a)))); }
	return(dytfWrap(dySin(dytfUnwrap(a))));
}

BGBDY_API dytf dytfCos(dytf a)
{
	if(dytfFixrealP(a))
		{ return(dytfReal(cos(dytfRealv(a)))); }
	return(dytfWrap(dyCos(dytfUnwrap(a))));
}

BGBDY_API dytf dytfTan(dytf a)
{
	if(dytfFixrealP(a))
		{ return(dytfReal(tan(dytfRealv(a)))); }
	return(dytfWrap(dyTan(dytfUnwrap(a))));
}

BGBDY_API dytf dytfASin(dytf a)
{
	if(dytfFixrealP(a))
		{ return(dytfReal(asin(dytfRealv(a)))); }
	return(dytfWrap(dyASin(dytfUnwrap(a))));
}

BGBDY_API dytf dytfACos(dytf a)
{
	if(dytfFixrealP(a))
		{ return(dytfReal(acos(dytfRealv(a)))); }
	return(dytfWrap(dyACos(dytfUnwrap(a))));
}

BGBDY_API dytf dytfATan(dytf a)
{
	if(dytfFixrealP(a))
		{ return(dytfReal(atan(dytfRealv(a)))); }
	return(dytfWrap(dyATan(dytfUnwrap(a))));
}

BGBDY_API dytf dytfATan2(dytf a, dytf b)
{
	if(dytfFixrealP(a) && dytfFixrealP(a))
		{ return(dytfReal(atan2(dytfRealv(a), dytfRealv(b)))); }
	return(dytfWrap(dyATan2(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API bool dytfEqP(dytf a, dytf b)
{
	if(a.v==b.v)return(true);
//	if((*(u64 *)(&a))==(*(u64 *)(&b)))return(true);

	if(dytfFixnumP(a) && dytfFixnumP(b))
		return(dytfIntvF(a) == dytfIntvF(b));
	if(dytfFlonumP(a) && dytfFlonumP(b))
		return(dytfRealvF(a) == dytfRealvF(b));
	return(dyeqp(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API bool dytfNEqP(dytf a, dytf b)
{
	return(!dytfEqP(a, b));
}

BGBDY_API bool dytfEqvP(dytf a, dytf b)
{
	if(a.v==b.v)return(true);
//	if((*(u64 *)(&a))==(*(u64 *)(&b)))return(true);

	if(dytfFlonumP(a) && dytfFlonumP(b))
		return(dytfRealvF(a) == dytfRealvF(b));
//	if(dytfFixnumP(a) && dytfFixnumP(b))
//		return(dytfIntv(a) == dytfIntv(b));
	return(dyeqvp(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API bool dytfNEqvP(dytf a, dytf b)
{
	return(!dytfEqvP(a, b));
}


BGBDY_API bool dytfLtP(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfIntv(a)<dytfIntv(b)); }
		else if(dytfFlonumP(b))
			{ return(dytfRealv(a)<dytfRealvF(b)); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfRealvF(a)<dytfRealvF(b)); }
		else if(dytfFixnumP(b))
			{ return(dytfRealvF(a)<dytfRealv(b)); }
	} 
	return(dyltp(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API bool dytfGtP(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfIntv(a)>dytfIntv(b)); }
		else if(dytfFlonumP(b))
			{ return(dytfRealv(a)>dytfRealvF(b)); }
	}else
	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfRealvF(a)>dytfRealvF(b)); }
		else if(dytfFixnumP(b))
			{ return(dytfRealvF(a)>dytfRealv(b)); }
	} 
	return(dygtp(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API bool dytfLeP(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfIntv(a)<=dytfIntv(b)); }
		else if(dytfFlonumP(b))
			{ return(dytfRealv(a)<=dytfRealvF(b)); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfRealvF(a)<=dytfRealvF(b)); }
		else if(dytfFixnumP(b))
			{ return(dytfRealvF(a)<=dytfRealv(b)); }
	}if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfIntv(a)<=dytfIntv(b)); }
		else if(dytfFlonumP(b))
			{ return(dytfRealv(a)<=dytfRealvF(b)); }
	}
	return(dylep(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API bool dytfGeP(dytf a, dytf b)
{
	if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(dytfIntv(a)>=dytfIntv(b)); }
		else if(dytfFlonumP(b))
			{ return(dytfRealv(a)>=dytfRealvF(b)); }
	}else if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(dytfRealvF(a)>=dytfRealvF(b)); }
		else if(dytfFixnumP(b))
			{ return(dytfRealvF(a)>=dytfRealv(b)); }
	}
	return(dygep(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API bool dytfUnP(dytf a, dytf b)
{
	if(dytfFlonumP(a))
	{
		if(dytfFlonumP(b))
			{ return(!(dytfRealvF(a)>=dytfRealvF(b))&&
				!(dytfRealvF(a)<=dytfRealvF(b))); }
		else if(dytfFixnumP(b))
			{ return(!(dytfRealvF(a)>=dytfRealvF(b))&&
				!(dytfRealvF(a)<=dytfRealvF(b))); }
	}else if(dytfFixnumP(a))
	{
		if(dytfFixnumP(b))
			{ return(0); }
		else if(dytfFlonumP(b))
			{ return(!(dytfRealvF(a)>=dytfRealvF(b))&&
				!(dytfRealvF(a)<=dytfRealvF(b))); }
	}
	return(dyunp(dytfUnwrap(a), dytfUnwrap(b)));
}

static double dytf_sqr(double x) { return(x*x); }
BGBDY_API dytf dytfSqr(dytf a)
{
	if(dytfFlonumP(a))
		{ return(dytfReal(dytf_sqr(dytfRealvF(a)))); }
	else if(dytfFixnumP(a))
		{ return(dytfInt(dytf_sqr(dytfIntv(a)))); }
	return(dytfWrap(dysqr(dytfUnwrap(a))));
}

BGBDY_API dytf dytfSqrt(dytf a)
{
	if(dytfFlonumP(a))
		{ return(dytfReal(sqrt(dytfRealvF(a)))); }
	else if(dytfFixnumP(a))
		{ return(dytfReal(sqrt(dytfIntv(a)))); }
	return(dytfWrap(dysqrt(dytfUnwrap(a))));
}

BGBDY_API dytf dytfSSqr(dytf a)
{
//	if(dytfFlonumP(a))
//		{ return(dytfReal(dytf_sqr(dytfRealvF(a)))); }
//	else if(dytfFixnumP(a))
//		{ return(dytfInt(dytf_sqr(dytfIntv(a)))); }
	return(dytfWrap(dyssqr(dytfUnwrap(a))));
}

BGBDY_API dytf dytfSSqrt(dytf a)
{
//	if(dytfFlonumP(a))
//		{ return(dytfReal(sqrt(dytfRealvF(a)))); }
//	else if(dytfFixnumP(a))
//		{ return(dytfReal(sqrt(dytfIntv(a)))); }
	return(dytfWrap(dyssqrt(dytfUnwrap(a))));
}

BGBDY_API dytf dytfInc(dytf a)
	{ return(dytfWrap(dyinc(dytfUnwrap(a)))); }
BGBDY_API dytf dytfDec(dytf a)
	{ return(dytfWrap(dydec(dytfUnwrap(a)))); }

BGBDY_API int dytfTrueP(dytf a)
	{ return(dytruep(dytfUnwrap(a))); }
BGBDY_API int dytfFalseP(dytf a)
	{ return(dyfalsep(dytfUnwrap(a))); }

BGBDY_API dytf dytfBool(int a)
	{ return(dytfWrap(dybool(a))); }

BGBDY_API dytf dytfChar(int a)
	{ return(dytfWrap(dychar(a))); }

BGBDY_API dytf dytfUnaryS(char *opr, dytf a)
{
	return(dytfWrap(dyUnaryS(opr, dytfUnwrap(a))));
}

BGBDY_API dytf dytfBinaryS(char *opr, dytf a, dytf b)
{
	return(dytfWrap(dyBinaryS(opr, dytfUnwrap(a), dytfUnwrap(b))));
}
