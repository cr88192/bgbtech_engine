#include <bgbgc.h>
#include <bgbdy.h>

#include <math.h>
// #include <complex.h>

/*
double:
s.e11.m52	sign, exponent(bias=1023), mantissa
if e==0x7FF then NaN or Inf
	m=
		0.0.v50	SNaN
		0.1.v50	Inf
		1.v51	QNaN

SNaN is off limits, as v is used to signal an error type.
QNaN and Inf can be used to encode values, although v=0 is reserved
to encode the "proper" cases (Inf or NaN).

x86:
0x7FF9yyyyxxxxxxxx
encodes a pointer.

0x7FFAxxxxxxxxxxxx
encodes an integer.

x86-64:
0x7FF4xxxxxxxxxxxx (positive)
0xFFF4xxxxxxxxxxxx (negative)
encodes a pointer, NULL->AUXNULL->NULL.

0x7FF8xxxxxxxxxxxx..0x7FFFxxxxxxxxxxxx	spaces (lower)
0xFFF8xxxxxxxxxxxx..0xFFFFxxxxxxxxxxxx	spaces (upper)
 encode spaces (upper half)

the spaces are "rotated" slightly, such that the low 48 bits align with their proper locations in "spaces".

0x0000000000000000 -> 0x7FFFFFFFFFFFFFFF
0x0000000000000001 -> 0x7FF8000000000001
...
0x00007FFFFFFFFFFE -> 0x7FFFFFFFFFFFFFFE
0x00007FFFFFFFFFFF -> 0xFFFFFFFFFFFFFFFE
0x0000800000000000 -> 0xFFFFFFFFFFFFFFFF
0x0000800000000001 -> 0xFFF8000000000001
...
0x0000FFFFFFFFFFFD -> 0xFFFFFFFFFFFFFFFD

as can be noted, the space is small fraction of a bit short of a
full 52 bits.

*/

BGBDY_API dytf dytfWrap(dyt a)
{
	u64 l;
	double f;

#if 1
	if(BGBDY_IsFixnumFP((byte *)a))
	{
		f=BGBDY_UnwrapIntFast(a);
		return(dytfInt((s64)f));
	}
	if(BGBDY_IsFlonumFP((byte *)a))
	{
		f=BGBDY_UnwrapMiniFloat(a);
		return(dytfReal(f));
	}
#endif

#ifdef X86
	l=0x7FF9000000000000ULL | ((u32)a);
	return(*(dytf *)(&l));
//	return((dytf)l);
#else
#ifdef X86_64
	if(gcllSpaceP(a))
	{
		l=gcllSpaceAbsoluteOffset((void *)a);
//		if(l>=0x000B000000000000ULL)
//		if(l>=0x0008000000000000ULL)
		if(l>=0x0007FFFFFFFFFFFFULL)
//		if(l>=0x0008000000000001ULL)
		{
//			l=l-0x0008000000000000ULL;
//			l=l-0x0007FFFFFFFFFFFFULL;
			l=l-0x0008000000000001ULL;
			l=l&0x0007FFFFFFFFFFFFULL;
			l=0xFFF8000000000001ULL+l;

//			l=l-0x000B000000000000ULL;
//			l=0xFFF5000000000001ULL+l;
			return(*(dytf *)(&l));
//			return((dytf)l);
		}

		//rotate
//		l=l-0x0000000000000001ULL;
		if(!l)l=0x0007FFFFFFFFFFFFULL;
//		l=l-2;
//		l=l&0x0007FFFFFFFFFFFFULL;

//		l=0x7FF8000000000001ULL + l;
		l=0x7FF8000000000000ULL + l;
//		l=0x7FF5000000000000ULL + l;
		return(*(dytf *)(&l));
//		return((dytf)l);
	}

	if(a==BGBDY_NULL)a=BGBDY_AUXNULL;
	if(((s64)a)>=0)
	{
//		l=(((u64)a)-1)&0x0007FFFFFFFFFFFFULL;
		l=((u64)a)&0x0000FFFFFFFFFFFFULL;
		if(!l)l=0x0007FFFFFFFFFFFFULL;
		l=0x7FF4000000000000ULL + l;
//		l=0x7FF4000000000001ULL + l;
//		l=0x7FF4000000000000ULL | ((u64)a)&0x0000FFFFFFFFFFFFULL;
	}
	else
	{
		l=0xFFF4000000000000ULL | ((u64)a)&0x0000FFFFFFFFFFFFULL;
	}
	return(*(dytf *)(&l));
//	return((dytf)l);
#else
	l=0x7FF8000000000000ULL;
	return(*(dytf *)(&l));
//	return(l);
#endif
#endif
}

BGBDY_API dyt dytfUnwrap(dytf a)
{
	u64 l;
	s64 v;
	dyt p;
	double f;

	l=*(u64 *)(&a);
//	l=a;

	if(dytfRealP(a))
	{
		f=*(double *)(&a);
		return(BGBDY_WrapFlonum(f));
	}

#ifdef X86
	if((l&0xFFFF000000000000ULL) == 0x7FF9000000000000ULL)
		return((dyt)((u32)(l&0xFFFFFFFFULL)));

	if((l&0x7FFF000000000000ULL) == 0x7FFA000000000000ULL)
	{
		v=l&0x0000FFFFFFFFFFFFLL;
		return(BGBDY_WrapInt64(v));
	}
	if((l&0xFFFF000000000000ULL)==0xFFFA000000000000ULL)
	{
		v=l&0x0000FFFFFFFFFFFFLL;
		v=v|(-1LL<<48);
		return(BGBDY_WrapInt64(v));
	}

	return(UNDEFINED);
#endif
#ifdef X86_64
	if((l&0xFFFF000000000000ULL) == 0x7FF4000000000000ULL)
	{
		l&=0x0000FFFFFFFFFFFFULL;
//		if(l&0x0000800000000000ULL)
//			l|=0xFFFF000000000000ULL;

		if(l==0x0007FFFFFFFFFFFFULL)l=0;

		p=(dyt)l;
//		if(p==BGBDY_AUXNULL)p=BGBDY_NULL;
		return(p);
	}

	if((l&0xFFFF000000000000ULL) == 0xFFF4000000000000ULL)
	{
		l&=0x0000FFFFFFFFFFFFULL;
//		if(l&0x0000800000000000ULL)
		l|=0xFFFF000000000000ULL;

		p=(dyt)l;
//		if(p==BGBDY_AUXNULL)p=BGBDY_NULL;
		return(p);
	}

	if((l>=0x7FF8000000000001ULL) && (l<=0x7FFFFFFFFFFFFFFFULL))
	{
//		l=l-0x7FF8000000000001ULL;
//		l=(l+1)&0x0007FFFFFFFFFFFFULL;
		l=l-0x7FF8000000000000ULL;
		if(l==0x0007FFFFFFFFFFFFULL)l=0;
		return(gcllSpaceWrapAbsolute(l));
	}

	if((l>=0xFFF8000000000001ULL) && (l<=0xFFFFFFFFFFFFFFFFULL))
	{
		l=l-0xFFF8000000000000ULL;
		if(l>=0x0007FFFFFFFFFFFEULL)
			{ l=l-0x0007FFFFFFFFFFFEULL; }
		else { l=l+1; }

//		l=l-0xFFF8000000000001ULL;
//		l=l+0x0008000000000000ULL;
//		l=(l+2)&0x0007FFFFFFFFFFFFULL;
		l=l+0x0007FFFFFFFFFFFFULL;
		return(gcllSpaceWrapAbsolute(l));
	}

//	if((l>=0x7FF5000000000000ULL) && (l<=0x7FFFFFFFFFFFFFFFULL))
//	{
//		l=l-0x7FF5000000000000ULL;
//		return(gcllSpaceWrapAbsolute(l));
//	}

//	if((l>=0xFFF5000000000001ULL) && (l<=0xFFFFFFFFFFFFFFFFULL) &&
//		(l!=0xFFF8000000000000ULL))
//	{
//		l=l-0xFFF5000000000001ULL;
//		l=l+0x000B000000000000ULL;
//		return(gcllSpaceWrapAbsolute(l));
//	}
#endif
}

BGBDY_API int dytfRealP(dytf a)
{
	u64 l;

	l=*(u64 *)(&a);

	if((l&0x7FF0000000000000ULL)!=0x7FF0000000000000ULL)
		return(1);	//not NaN/Inf space.
	if((l&0x7FFC000000000000ULL)==0x7FF0000000000000ULL)
		return(1);	//SNaN
	if((l&0x0003FFFFFFFFFFFFULL)==0x0000000000000000ULL)
	{
		if((l&0x0003FFFFFFFFFFFFULL)!=0x000C000000000000ULL)
			return(1);	//proper Inf or QNaN
	}
	return(0);
}

BGBDY_API dytf dytfReal(double a)
{
	u64 l;

	//purpose: clean up any "garbage" bits in a double
	//this allows these bits to be used to encode other values

	l=*(u64 *)(&a);
	if((l&0x7FF0000000000000ULL)!=0x7FF0000000000000ULL)
		return(*(dytf *)(&l));	//not NaN/Inf space.
	if((l&0x7FFC000000000000ULL)==0x7FF0000000000000ULL)
		return(*(dytf *)(&l));	//SNaN
	if((l&0x0003FFFFFFFFFFFFULL)==0x0000000000000000ULL)
	{
		if((l&0x0003FFFFFFFFFFFFULL)!=0x000C000000000000ULL)
			return(*(dytf *)(&l));	//proper Inf or QNaN
		l&=~0x0004000000000000ULL;
		return(*(dytf *)(&l));
	}

	l&=~0x0003FFFFFFFFFFFFULL;
	return(*(dytf *)(&l));
}

BGBDY_API double dytfRealv(dytf a)
{
	dyt p;
	double f;

	if(dytfRealP(a))
		return(*(double *)(&a));
#ifdef X86
	if(dytfIntP(a))
		return((double)dytfIntv(a));
#endif

	p=dytfUnwrap(a);

	if(BGBDY_IsFixnumFP((byte *)p))
	{
		f=BGBDY_UnwrapIntFast((byte *)p);
		return(f);
	}
	if(BGBDY_IsFlonumFP((byte *)p))
	{
		f=BGBDY_UnwrapMiniFloat((byte *)p);
		return(f);
	}

	f=dyrealv(p);
	return(f);
}

BGBDY_API int dytfIntP(dytf a)
{
#ifdef X86
	u64 l;

	l=*(u64 *)(&a);
	if((l&0x7FFF000000000000ULL)==0x7FFA000000000000ULL)
		return(1);	//+Int
	if((l&0xFFFF000000000000ULL)==0xFFFA000000000000ULL)
		return(1);	//-Int
#endif
	return(0);
}

BGBDY_API dytf dytfInt(s64 v)
{
	u64 t;

#ifdef X86
	if(v<0)
	{
		if(v<=(-((1LL)<<48)))
			return(dytfWrap(dylong(v)));
		t=0xFFFA000000000000ULL|v;
		return(*(dytf *)(&t));
	}else
	{
		if(v>=((1LL)<<48))
			return(dytfWrap(dylong(v)));
		t=0x7FFA000000000000ULL|v;
		return(*(dytf *)(&t));
	}
#else
	return(dytfWrap(dylong(v)));
#endif
}

BGBDY_API s64 dytfIntv(dytf a)
{
	s64 v;

#ifdef X86
	u64 l;

	l=*(u64 *)(&a);
	if((l&0x7FFF000000000000ULL)==0x7FFA000000000000ULL)
	{
		v=l&0x0000FFFFFFFFFFFFULL;
		return(v);	//+Int
	}
	if((l&0xFFFF000000000000ULL)==0xFFFA000000000000ULL)
	{
		v=l&0x0000FFFFFFFFFFFFULL;
		v=v|(-1LL<<48);
		return(v);	//+Int
	}
#endif

	if(dytfRealP(a))
	{
		v=(s64)(*(double *)(&a));
		return(v);
	}

	return(dylongv(dytfUnwrap(a)));
}

BGBDY_API double dytfRealvF(dytf a)
	{ return(*(double *)(&a)); }

BGBDY_API void *dytfPtrvF(dytf a)
{
#ifdef X86
//	return((void *)((u32)a));
	return((void *)((u32)(*(u64 *)(&a))));
#else
#ifdef X86_64
	u64 b;
	b=(*(u64 *)(&a))&0x0000FFFFFFFFFFFFULL;
	if(b&0x0000800000000000ULL);
		b|=0xFFFF000000000000ULL;
	return((void *)b);
#else
	return(*(void **)(&a));
#endif
#endif
}

BGBDY_API char *dytfgettype(dytf a)
{
	if(dytfRealP(a))
		return("_double_t");
#ifdef X86
	if(dytfIntP(a))
		return("_fixint_t");
#endif
	return(dygettype(dytfUnwrap(a)));
}

BGBDY_API int dytftypep(dytf a, char *ty)
{
	char *ty1;

	ty1=dytfgettype(a);

	if(!ty && !ty1)return(1);
	if(!ty || !ty1)return(0);

	if(!strcmp(ty1, ty))return(1);
	return(0);
}


BGBDY_API dytf dytfAdd(dytf a, dytf b)
{
	if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfRealvF(a)+dytfRealvF(b))); }
		else if(dytfIntP(b))
			{ return(dytfReal(dytfRealvF(a)+dytfRealv(b))); }
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)+dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfReal(dytfRealv(a)+dytfRealvF(b))); }
	}
	return(dytfWrap(dyadd(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfSub(dytf a, dytf b)
{
	if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfRealvF(a)-dytfRealvF(b))); }
		else if(dytfIntP(b))
			{ return(dytfReal(dytfRealvF(a)-dytfRealv(b))); }
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)-dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfReal(dytfRealv(a)-dytfRealvF(b))); }
	}
	return(dytfWrap(dysub(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfMul(dytf a, dytf b)
{
	if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfRealvF(a)*dytfRealvF(b))); }
		else if(dytfIntP(b))
			{ return(dytfReal(dytfRealvF(a)*dytfRealv(b))); }
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)*dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfReal(dytfRealv(a)*dytfRealvF(b))); }
	}
	return(dytfWrap(dymul(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfDiv(dytf a, dytf b)
{
	dytf c;

	if(dytfRealP(a))
	{
		if(dytfRealP(b))
		{
			c=dytfReal(dytfRealvF(a)/dytfRealvF(b));
			return(c);
		}else if(dytfIntP(b))
		{
			c=dytfReal(dytfRealvF(a)/dytfRealv(b));
			return(c);
		}
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
		{
			c=dytfInt(dytfIntv(a)/dytfIntv(b));
			return(c);
		}else if(dytfRealP(b))
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

	if(dytfRealP(a))
	{
		if(dytfRealP(b))
		{
			c=dytfReal(fmod(dytfRealvF(a), dytfRealvF(b)));
			return(c);
		}else if(dytfIntP(b))
		{
			c=dytfReal(fmod(dytfRealvF(a), dytfRealv(b)));
			return(c);
		}
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
		{
			c=dytfInt(dytfIntv(a)%dytfIntv(b));
			return(c);
		}else if(dytfRealP(b))
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
	if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)&dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfInt(dytfIntv(a)&dytfIntv(b))); }
	}else if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfIntv(a)&dytfIntv(b))); }
		else if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)&dytfIntv(b))); }
	}
	return(dytfWrap(dyand(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfOr(dytf a, dytf b)
{
	if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)|dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfInt(dytfIntv(a)|dytfIntv(b))); }
	}else if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfIntv(a)|dytfIntv(b))); }
		else if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)|dytfIntv(b))); }
	}
	return(dytfWrap(dyor(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfXor(dytf a, dytf b)
{
	if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)^dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfInt(dytfIntv(a)^dytfIntv(b))); }
	}else if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfIntv(a)^dytfIntv(b))); }
		else if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)^dytfIntv(b))); }
	}
	return(dytfWrap(dyxor(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfShl(dytf a, dytf b)
{
	if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)<<dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfInt(dytfIntv(a)<<dytfIntv(b))); }
	}else if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfIntv(a)<<dytfIntv(b))); }
		else if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)<<dytfIntv(b))); }
	}
	return(dytfWrap(dyshl(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfShr(dytf a, dytf b)
{
	if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)>>dytfIntv(b))); }
		else if(dytfRealP(b))
			{ return(dytfInt(dytfIntv(a)>>dytfIntv(b))); }
	}else if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfReal(dytfIntv(a)>>dytfIntv(b))); }
		else if(dytfIntP(b))
			{ return(dytfInt(dytfIntv(a)>>dytfIntv(b))); }
	}
	return(dytfWrap(dyshr(dytfUnwrap(a), dytfUnwrap(b))));
}

BGBDY_API dytf dytfExp(dytf a, dytf b)
{
	dytf c;

	if(dytfRealP(a))
	{
		if(dytfRealP(b))
		{
			c=dytfReal(pow(dytfRealvF(a), dytfRealvF(b)));
			return(c);
		}else if(dytfIntP(b))
		{
			c=dytfReal(pow(dytfRealvF(a), dytfRealv(b)));
			return(c);
		}
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
		{
			c=dytfInt(pow(dytfIntv(a), dytfIntv(b)));
			return(c);
		}else if(dytfRealP(b))
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
	if(dytfRealP(a))
		{ return(dytfReal(-dytfRealvF(a))); }
	else if(dytfIntP(a))
		{ return(dytfInt(-dytfIntv(a))); }
	return(dytfWrap(dyneg(dytfUnwrap(a))));
}

BGBDY_API dytf dytfNot(dytf a)
{
	if(dytfRealP(a))
		{ return(dytfReal(~dytfIntv(a))); }
	else if(dytfIntP(a))
		{ return(dytfInt(~dytfIntv(a))); }
	return(dytfWrap(dynot(dytfUnwrap(a))));
}

BGBDY_API dytf dytfLNot(dytf a)
{
	if(dytfRealP(a))
		{ return(dytfReal(!dytfIntv(a))); }
	else if(dytfIntP(a))
		{ return(dytfInt(!dytfIntv(a))); }
	return(dytfWrap(dylnot(dytfUnwrap(a))));
}

BGBDY_API int dytfEqP(dytf a, dytf b)
{
//	if(a==b)return(1);
	if((*(u64 *)(&a))==(*(u64 *)(&b)))return(1);

	if(dytfRealP(a) && dytfRealP(b))
		return(dytfRealvF(a) == dytfRealvF(b));
//	if(dytfIntP(a) && dytfIntP(b))
//		return(dytfIntv(a) == dytfIntv(b));
	return(dyeqp(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API int dytfNEqP(dytf a, dytf b)
{
	return(!dytfEqP(a, b));
}


BGBDY_API int dytfLtP(dytf a, dytf b)
{
	if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfRealvF(a)<dytfRealvF(b)); }
		else if(dytfIntP(b))
			{ return(dytfRealvF(a)<dytfRealv(b)); }
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfIntv(a)<dytfIntv(b)); }
		else if(dytfRealP(b))
			{ return(dytfRealv(a)<dytfRealvF(b)); }
	}
	return(dyltp(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API int dytfGtP(dytf a, dytf b)
{
	if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfRealvF(a)>dytfRealvF(b)); }
		else if(dytfIntP(b))
			{ return(dytfRealvF(a)>dytfRealv(b)); }
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfIntv(a)>dytfIntv(b)); }
		else if(dytfRealP(b))
			{ return(dytfRealv(a)>dytfRealvF(b)); }
	}
	return(dygtp(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API int dytfLeP(dytf a, dytf b)
{
	if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfRealvF(a)<=dytfRealvF(b)); }
		else if(dytfIntP(b))
			{ return(dytfRealvF(a)<=dytfRealv(b)); }
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfIntv(a)<=dytfIntv(b)); }
		else if(dytfRealP(b))
			{ return(dytfRealv(a)<=dytfRealvF(b)); }
	}
	return(dylep(dytfUnwrap(a), dytfUnwrap(b)));
}

BGBDY_API int dytfGeP(dytf a, dytf b)
{
	if(dytfRealP(a))
	{
		if(dytfRealP(b))
			{ return(dytfRealvF(a)>=dytfRealvF(b)); }
		else if(dytfIntP(b))
			{ return(dytfRealvF(a)>=dytfRealv(b)); }
	}else if(dytfIntP(a))
	{
		if(dytfIntP(b))
			{ return(dytfIntv(a)>=dytfIntv(b)); }
		else if(dytfRealP(b))
			{ return(dytfRealv(a)>=dytfRealvF(b)); }
	}
	return(dygep(dytfUnwrap(a), dytfUnwrap(b)));
}

static double dytf_sqr(double x) { return(x*x); }
BGBDY_API dytf dytfSqr(dytf a)
{
	if(dytfRealP(a))
		{ return(dytfReal(dytf_sqr(dytfRealvF(a)))); }
	else if(dytfIntP(a))
		{ return(dytfInt(dytf_sqr(dytfIntv(a)))); }
	return(dytfWrap(dysqr(dytfUnwrap(a))));
}

BGBDY_API dytf dytfSqrt(dytf a)
{
	if(dytfRealP(a))
		{ return(dytfReal(sqrt(dytfRealvF(a)))); }
	else if(dytfIntP(a))
		{ return(dytfReal(sqrt(dytfIntv(a)))); }
	return(dytfWrap(dysqrt(dytfUnwrap(a))));
}

BGBDY_API int dytfTrueP(dytf a)
	{ return(dytruep(dytfUnwrap(a))); }
BGBDY_API int dytfFalseP(dytf a)
	{ return(dyfalsep(dytfUnwrap(a))); }

BGBDY_API dytf dytfBool(int a)
	{ return(dytfWrap(dybool(a))); }
