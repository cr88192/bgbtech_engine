#include <bgbgc.h>
#include <bgbdy.h>

#include <math.h>

BGBDY_Operator *dyopr_hash[4096];

char *dyopr_opr_names[1024];
int dyopr_opr_n_names=0;

static int dyopr_hashstr(char *str)
{
	char *s;
	int i;
	
	if(!str)return(0);
	s=str; i=0;
	while(*s)i=(i*4093)+(*s++);
	return(i);
}

void dyopr_init()
{
	static char *oprnamesa[]={
	 "add",   "sub",   "mul",   "div", 
	 "mod",   "and",    "or",   "xor",
	 "shl",   "shr",    "eq",   "neq",
	  "lt",    "gt",    "le",    "ge",
	"idiv",  "dadd",  "dsub",  "dmul",
	"ddiv",  "dmod",  "dand",   "dor",
	"dxor", "didiv",   "exp",  "dshl",
	"dshr",  "shrr", "atan2",
	NULL};
	static char *oprnamesb[]={
	 "neg",   "not",  "lnot",   "pos",
	"umul",  "udiv", "uidiv",  "umod",
	"uand",   "uor",  "uxor", "uland",
	"ulor", "ulxor",   "inc",   "dec",
	 "sin",   "cos",   "tan",  "asin",
	"acos",  "atan",   "sqr",  "sqrt",
	NULL};
	
	static int init=0;
	int i;

	if(init)return;
	init=1;
	
	for(i=0; oprnamesa[i]; i++)
		dyopr_opr_names[i]=oprnamesa[i];
	for(i=0; oprnamesb[i]; i++)
		dyopr_opr_names[64+i]=oprnamesb[i];
	dyopr_opr_n_names=64+i;
}

static int dyopr_indexOprName(char *str)
{
	int i;

	dyopr_init();
	
	for(i=0; i<dyopr_opr_n_names; i++)
		if(dyopr_opr_names[i] &&
			!strcmp(dyopr_opr_names[i], str))
				return(i);

	for(i=0; i<dyopr_opr_n_names; i++)
		if(!dyopr_opr_names[i])
	{
		dyopr_opr_names[i]=dyllStrdup(str);
		return(i);
	}
	
	i=dyopr_opr_n_names++;
	dyopr_opr_names[i]=dyllStrdup(str);
	return(i);
}

static int dyopr_hashopr(int opr, char *lty, char *rty)
{
	int i;
	
	i=dyopr_hashstr(lty);
	i=(i*4093)+dyopr_hashstr(rty);
	i=(i*4093)+opr;
	return(i);
}

BGBDY_Operator *dyopr_lookupopr(int opr, char *lty, char *rty)
{
	BGBDY_Operator *cur;
	int i, h;
	
	i=dyopr_hashopr(opr, lty, rty);
	h=((i*4093)>>8)&4095;
	
	cur=dyopr_hash[h];
	while(cur)
	{
		if(cur->opr!=opr)
			{ cur=cur->next; continue; }

		if(lty)
		{
			if(!cur->lty || strcmp(cur->lty, lty))
				{ cur=cur->next; continue; }
		}else
		{
			if(cur->lty)
				{ cur=cur->next; continue; }
		}

		if(rty)
		{
			if(!cur->rty || strcmp(cur->rty, rty))
				{ cur=cur->next; continue; }
		}else
		{
			if(cur->rty)
				{ cur=cur->next; continue; }
		}
	}
	return(NULL);
}

BGBDY_Operator *dyopr_getopr(int opr, char *lty, char *rty)
{
	BGBDY_Operator *cur;
	int i, h;
	
	cur=dyopr_lookupopr(opr, lty, rty);
	if(cur)return(cur);

	i=dyopr_hashopr(opr, lty, rty);
	h=((i*4093)>>8)&4095;
	
	cur=gctalloc("_bgbdy_operator_t", sizeof(BGBDY_Operator));
	cur->opr=opr;
	cur->lty=dyllString(lty);
	cur->rty=dyllString(rty);
	
	cur->next=dyopr_hash[h];
	dyopr_hash[h]=cur;
	
	return(cur);
}

void dyRegisterBinary(int opr, char *lty, char *rty,
	dyt (*fcn)(dyt a, dyt b))
{
	BGBDY_Operator *cur;

	cur=dyopr_getopr(opr, lty, rty);
	cur->fcn=fcn;
}

void dyRegisterUnary(int opr, char *ty, dyt (*fcn)(dyt a))
{
	BGBDY_Operator *cur;

	cur=dyopr_getopr(opr, ty, NULL);
	cur->ufcn=fcn;
}

void dyRegisterBinaryName(char *opr, char *lty, char *rty,
	dyt (*fcn)(dyt a, dyt b))
{
	int i;

	i=dyopr_indexOprName(opr);
	dyRegisterBinary(i, lty, rty, fcn);
}

void dyRegisterUnaryName(char *opr, char *ty, dyt (*fcn)(dyt a))
{
	int i;

	i=dyopr_indexOprName(opr);
	dyRegisterUnary(i, ty, fcn);
}

dyt dyopr_stradd(dyt a, dyt b)
{
	char *buf;
	char *s0, *s1;
	dyt c;

	s0=dystringv(a);
	s1=dystringv(b);
	buf=gcralloc(strlen(s0)+strlen(s1)+1);
	strcpy(buf, s0);
	strcpy(buf+strlen(s0), s1);

	c=dystring(buf);
	return(c);
}

dyt dyopr_doBinary(int opr, char *lty, char *rty, dyt a, dyt b)
{
	BGBDY_Operator *cur;

	cur=dyopr_lookupopr(opr, lty, rty);
	if(!cur || !cur->fcn)
		return(UNDEFINED);
	return(cur->fcn(a, b));
}

dyt dyopr_doUnary(int opr, char *ty, dyt a)
{
	BGBDY_Operator *cur;

	cur=dyopr_lookupopr(opr, ty, NULL);
	if(!cur || !cur->ufcn)
		return(UNDEFINED);
	return(cur->ufcn(a));
}

BGBDY_API dyt dyUnaryS(char *opr, dyt a)
{
	char *ty;
	dyt c;
	int i;
	
	i=dyopr_indexOprName(opr);
	ty=dygettype(a);
	c=dyopr_doUnary(i, ty, a);
	return(c);
}

BGBDY_API dyt dyBinaryS(char *opr, dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;
	int i;
	
	i=dyopr_indexOprName(opr);
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(i, lty, rty, a, b);
	return(c);
}

BGBDY_API int dyIndexOperatorName(char *opr)
{
	return(dyopr_indexOprName(opr));
}

BGBDY_API char *dyOperatorNameForIndex(int opr)
{
	if(opr<0)return(NULL);
	if(opr>=1024)return(NULL);
	return(dyopr_opr_names[opr]);
}

BGBDY_API dyt dyadd(dyt a, dyt b)
{
	char *lty, *rty;
	char *s;
	dyt c;
	int i;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyint(dyIntvF(a)+dyIntvF(b)));
		return(dyflonum(dyfloatv(a)+dyfloatv(b)));
	}
	
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_ADD, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)+dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)+dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_add(dyint128v(a), dyint128v(b))); return(c); }

	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(dyfloatv(a)+dyfloatv(b)); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(dydoublev(a)+dydoublev(b)); return(c); }
	if(dyrealx2p(a) && dyrealx2p(b))
		return(dyfloat128(fv128_add(dyfloat128v(a), dyfloat128v(b))));

	if(dycomplexp(a) && dycomplexp(b))
		{ c=dydcomplex(dtyDCadd(dycomplexv(a), dycomplexv(b))); return(c); }

	if(dyvec2p(a) && dyvec2p(b))
		return(dywvec2(v2add(dyvec2v(a), dyvec2v(b))));
	if(dyvec3p(a) && dyvec3p(b))
		return(dywvec3(v3add(dyvec3v(a), dyvec3v(b))));
	if(dyvec4p(a) && dyvec4p(b))
		return(dywvec4(v4add(dyvec4v(a), dyvec4v(b))));
	if(dyquatp(a) && dyquatp(b))
		return(dywquat(qadd(dyquatv(a), dyquatv(b))));

	if(dyvec2sdp(a) && dyvec2sdp(b))
		return(dywvec2d(v2dadd(dyvec2vd(a), dyvec2vd(b))));
	if(dyvec3sdp(a) && dyvec3sdp(b))
		return(dywvec3d(v3dadd(dyvec3vd(a), dyvec3vd(b))));
	if(dyvec4sdp(a) && dyvec4sdp(b))
		return(dywvec4d(v4dadd(dyvec4vd(a), dyvec4vd(b))));
	if(dyquatsdp(a) && dyquatsdp(b))
		return(dywquatd(qdadd(dyquatvd(a), dyquatvd(b))));

	if(dySmallComplexp(a) && dySmallComplexp(b))
		{ c=dydcomplex(dtyDCadd(dycomplexv(a), dycomplexv(b))); return(c); }
	if(dySmallQuatp(a) && dySmallQuatp(b))
		{ c=dywquat(qadd(dyquatv(a), dyquatv(b))); return(c); }

	if(dystringp(a))
	{
		if(dystringp(b))
		{
			return(dyopr_stradd(a, b));
		}
		if(dyintp(b))
		{
			s=(char *)a; i=dyintv(b);
			if(i<0)return(dysub(a, dyint(-i)));
			while(*s && i)
				{ s=BGBDY_NextChar(s); i--; }
			c=(dyt)s; if(!(*s) && (i>0)) c=NULL;
//			c=(dyt)(((char *)a)+dyintv(b));
			return(c);
		}
		return(dyopr_stradd(a, (dyt)dyToString(b)));
	}

	if(dycArrayP(a))
	{
		if(dyintp(b))
		{
//			c=dycArrayIndexRef((dycArray)a, dyintv(b));
			c=(dyt)dycArrayAddOffset((dycArray)a, dyintv(b));
			return(c);
		}
		return(NULL);
	}
	
	if(dycArrayRefP(a))
	{
		if(dyintp(b))
		{
			c=dycArrayRefAddOffset(a, dyintv(b));
			return(c);
		}
		return(NULL);
	}

	if(dyllTypeboxPtrP(a))
	{
		if(dyintp(b))
			return(dyllTypeboxPtrAddIndex(a, dyintv(b)));
		return(NULL);
	}

	return(NULL);
}

BGBDY_API dyt dysub(dyt a, dyt b)
{
	char *lty, *rty;
	char *s, *s1;
	dyt c;
	int i;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyint(dyIntvF(a)-dyIntvF(b)));
		return(dyflonum(dyfloatv(a)-dyfloatv(b)));
	}
	
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_SUB, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)-dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)-dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_sub(dyint128v(a), dyint128v(b))); return(c); }

	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(dyfloatv(a)-dyfloatv(b)); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(dydoublev(a)-dydoublev(b)); return(c); }
	if(dyrealx2p(a) && dyrealx2p(b))
		return(dyfloat128(fv128_sub(dyfloat128v(a), dyfloat128v(b))));

	if(dycomplexp(a) && dycomplexp(b))
		{ c=dydcomplex(dtyDCsub(dycomplexv(a), dycomplexv(b))); return(c); }

	if(dyvec2p(a) && dyvec2p(b))
		return(dywvec2(v2sub(dyvec2v(a), dyvec2v(b))));
	if(dyvec3p(a) && dyvec3p(b))
		return(dywvec3(v3sub(dyvec3v(a), dyvec3v(b))));
	if(dyvec4p(a) && dyvec4p(b))
		return(dywvec4(v4sub(dyvec4v(a), dyvec4v(b))));
	if(dyquatp(a) && dyquatp(b))
		return(dywquat(qsub(dyquatv(a), dyquatv(b))));

	if(dyvec2sdp(a) && dyvec2sdp(b))
		return(dywvec2d(v2dsub(dyvec2vd(a), dyvec2vd(b))));
	if(dyvec3sdp(a) && dyvec3sdp(b))
		return(dywvec3d(v3dsub(dyvec3vd(a), dyvec3vd(b))));
	if(dyvec4sdp(a) && dyvec4sdp(b))
		return(dywvec4d(v4dsub(dyvec4vd(a), dyvec4vd(b))));
	if(dyquatsdp(a) && dyquatsdp(b))
		return(dywquatd(qdsub(dyquatvd(a), dyquatvd(b))));

	if(dySmallComplexp(a) && dySmallComplexp(b))
		{ c=dydcomplex(dtyDCsub(dycomplexv(a), dycomplexv(b))); return(c); }
	if(dySmallQuatp(a) && dySmallQuatp(b))
		{ c=dywquat(qsub(dyquatv(a), dyquatv(b))); return(c); }

	if(dystringp(a))
	{
		if(dyintp(b))
		{
			s=(char *)a; i=dyintv(b);
			if(i<0)return(dyadd(a, dyint(-i)));
			s1=gcgetbase(a);
			while((s>s1) && i)
				{ s=BGBDY_PriorChar(s); i--; }
			c=(dyt)s; if(!(s>s1) && (i>0)) c=NULL;
//			c=(dyt)(((char *)a)-dyintv(b));
			return(c);
		}
		if(dystringp(b))
		{
			return(dyint(((char *)a)-((char *)b)));
		}
		return(NULL);
//		return(dyopr_stradd(a, (dyt)dyToString(b)));
	}

	if(dycArrayP(a))
	{
		if(dyintp(b))
		{
//			c=dycArrayIndexRef((dycArray)a, -dyintv(b));
			c=(dyt)dycArrayAddOffset((dycArray)a, -dyintv(b));
			return(c);
		}
		if(dycArrayP(b))
		{
			c=dyint(dycArraySubtract((dycArray)a, (dycArray)b));
			return(c);
		}
		return(NULL);
	}
	
	if(dycArrayRefP(a))
	{
		if(dyintp(b))
		{
			c=dycArrayRefAddOffset(a, -dyintv(b));
			return(c);
		}
		return(NULL);
	}

	if(dyllTypeboxPtrP(a))
	{
		if(dyintp(b))
			return(dyllTypeboxPtrAddIndex(a, -dyintv(b)));
		return(NULL);
	}

	return(NULL);
}

BGBDY_API dyt dymul(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyint(dyIntvF(a)*dyIntvF(b)));
		return(dyflonum(dyfloatv(a)*dyfloatv(b)));
	}
	
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_MUL, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)*dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)*dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_mul(dyint128v(a), dyint128v(b))); return(c); }

	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(dyfloatv(a)*dyfloatv(b)); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(dydoublev(a)*dydoublev(b)); return(c); }
	if(dyrealx2p(a) && dyrealx2p(b))
		return(dyfloat128(fv128_mul(dyfloat128v(a), dyfloat128v(b))));

	if(dycomplexp(a) && dycomplexp(b))
		{ c=dydcomplex(dtyDCmul(dycomplexv(a), dycomplexv(b))); return(c); }

	if(dyvec2p(a) && dyvec2p(b))
		return(dyflonum(v2dot(dyvec2v(a), dyvec2v(b))));
	if(dyvec3p(a) && dyvec3p(b))
		return(dyflonum(v3dot(dyvec3v(a), dyvec3v(b))));
	if(dyvec4p(a) && dyvec4p(b))
		return(dyflonum(v4dot(dyvec4v(a), dyvec4v(b))));
	if(dyquatp(a) && dyquatp(b))
		return(dywquat(qmul(dyquatv(a), dyquatv(b))));

	if(dyvec2p(a) && dyrealp(b))
		return(dywvec2(v2scale(dyvec2v(a), dydoublev(b))));
	if(dyvec3p(a) && dyrealp(b))
		return(dywvec3(v3scale(dyvec3v(a), dydoublev(b))));
	if(dyvec4p(a) && dyrealp(b))
		return(dywvec4(v4scale(dyvec4v(a), dydoublev(b))));
	if(dyquatp(a) && dyrealp(b))
		return(dywquat(qscale(dyquatv(a), dydoublev(b))));

	if(dyvec2sdp(a) && dyvec2sdp(b))
		return(dydouble(v2ddot(dyvec2vd(a), dyvec2vd(b))));
	if(dyvec3sdp(a) && dyvec3sdp(b))
		return(dydouble(v3ddot(dyvec3vd(a), dyvec3vd(b))));
	if(dyvec4sdp(a) && dyvec4sdp(b))
		return(dydouble(v4ddot(dyvec4vd(a), dyvec4vd(b))));
	if(dyquatsdp(a) && dyquatsdp(b))
		return(dywquatd(qdmul(dyquatvd(a), dyquatvd(b))));

	if(dyvec2dp(a) && dyrealp(b))
		return(dywvec2d(v2dscale(dyvec2vd(a), dydoublev(b))));
	if(dyvec3dp(a) && dyrealp(b))
		return(dywvec3d(v3dscale(dyvec3vd(a), dydoublev(b))));
	if(dyvec4dp(a) && dyrealp(b))
		return(dywvec4d(v4dscale(dyvec4vd(a), dydoublev(b))));
	if(dyquatdp(a) && dyrealp(b))
		return(dywquatd(qdscale(dyquatvd(a), dydoublev(b))));

	if(dySmallComplexp(a) && dySmallComplexp(b))
		{ c=dydcomplex(dtyDCmul(dycomplexv(a), dycomplexv(b))); return(c); }
	if(dySmallQuatp(a) && dySmallQuatp(b))
		{ c=dywquat(qmul(dyquatv(a), dyquatv(b))); return(c); }

	return(NULL);
}

BGBDY_API dyt dydiv(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyint(dyIntvF(a)/dyIntvF(b)));
		return(dyflonum(dyfloatv(a)/dyfloatv(b)));
	}
	
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_DIV, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)/dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)/dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_div(dyint128v(a), dyint128v(b))); return(c); }

	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(dyfloatv(a)/dyfloatv(b)); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(dydoublev(a)/dydoublev(b)); return(c); }
	if(dyrealx2p(a) && dyrealx2p(b))
		return(dyfloat128(fv128_div(dyfloat128v(a), dyfloat128v(b))));

	if(dycomplexp(a) && dycomplexp(b))
		{ c=dydcomplex(dtyDCdiv(dycomplexv(a), dycomplexv(b))); return(c); }

	if(dyquatp(a) && dyquatp(b))
		return(dywquat(qdiv(dyquatv(a), dyquatv(b))));
	if(dyrealp(a) && dyquatp(b))
		return(dywquat(qdiv(quat(dydoublev(a),0,0,0), dyquatv(b))));

	if(dyquatsdp(a) && dyquatsdp(b))
		return(dywquatd(qddiv(dyquatvd(a), dyquatvd(b))));
	if(dyrealp(a) && dyquatdp(b))
		return(dywquatd(qddiv(quatd(dydoublev(a),0,0,0), dyquatvd(b))));

	if(dyvec2p(a) && dyrealp(b))
		return(dywvec2(v2scale(dyvec2v(a), 1.0/dydoublev(b))));
	if(dyvec3p(a) && dyrealp(b))
		return(dywvec3(v3scale(dyvec3v(a), 1.0/dydoublev(b))));
	if(dyvec4p(a) && dyrealp(b))
		return(dywvec4(v4scale(dyvec4v(a), 1.0/dydoublev(b))));
	if(dyquatp(a) && dyrealp(b))
		return(dywquat(qscale(dyquatv(a), 1.0/dydoublev(b))));

	if(dyvec2sdp(a) && dyrealp(b))
		return(dywvec2d(v2dscale(dyvec2vd(a), 1.0/dydoublev(b))));
	if(dyvec3sdp(a) && dyrealp(b))
		return(dywvec3d(v3dscale(dyvec3vd(a), 1.0/dydoublev(b))));
	if(dyvec4sdp(a) && dyrealp(b))
		return(dywvec4d(v4dscale(dyvec4vd(a), 1.0/dydoublev(b))));
	if(dyquatsdp(a) && dyrealp(b))
		return(dywquatd(qdscale(dyquatvd(a), 1.0/dydoublev(b))));

	if(dySmallComplexp(a) && dySmallComplexp(b))
		{ c=dydcomplex(dtyDCdiv(dycomplexv(a), dycomplexv(b))); return(c); }
	if(dySmallQuatp(a) && dySmallQuatp(b))
		{ c=dywquat(qdiv(dyquatv(a), dyquatv(b))); return(c); }

	return(NULL);
}

BGBDY_API dyt dymod(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyint(dyIntvF(a)%dyIntvF(b)));
		return(dyflonum(fmod(dyfloatv(a), dyfloatv(b))));
	}
	
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_MOD, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)%dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)%dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_mod(dyint128v(a), dyint128v(b))); return(c); }

	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(fmod(dyfloatv(a), dyfloatv(b))); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(fmod(dydoublev(a), dydoublev(b))); return(c); }

	if(dyrealx2p(a) && dyrealx2p(b))
		return(dyfloat128(fv128_mod(dyfloat128v(a), dyfloat128v(b))));

	if(dyvec2p(a) && dyvec2p(b))
		return(dyflonum(v2cross(dyvec2v(a), dyvec2v(b))));
	if(dyvec3p(a) && dyvec3p(b))
		return(dywvec3(v3cross(dyvec3v(a), dyvec3v(b))));
	if(dyvec4p(a) && dyvec4p(b))
		return(dywvec4(v4cross(dyvec4v(a), dyvec4v(b))));
	if(dyquatp(a) && dyquatp(b))
		return(dywquat(qcross(dyquatv(a), dyquatv(b))));

	if(dyvec2sdp(a) && dyvec2sdp(b))
		return(dydouble(v2dcross(dyvec2vd(a), dyvec2vd(b))));
	if(dyvec3sdp(a) && dyvec3sdp(b))
		return(dywvec3d(v3dcross(dyvec3vd(a), dyvec3vd(b))));
	if(dyvec4sdp(a) && dyvec4sdp(b))
		return(dywvec4d(v4dcross(dyvec4vd(a), dyvec4vd(b))));
	if(dyquatsdp(a) && dyquatsdp(b))
		return(dywquatd(qdcross(dyquatvd(a), dyquatvd(b))));

//	if(dySmallComplexp(a) && dySmallComplexp(b))
//		{ c=dydcomplex(dtyDCadd(dycomplexv(a), dycomplexv(b))); return(c); }
	if(dySmallQuatp(a) && dySmallQuatp(b))
		{ c=dywquat(qcross(dyquatv(a), dyquatv(b))); return(c); }

	return(NULL);
}

BGBDY_API dyt dyand(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixintp(a) && dyfixintp(b))
		return(dyint(dyIntvF(a)&dyIntvF(b)));
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_AND, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)&dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)&dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_and(dyint128v(a), dyint128v(b))); return(c); }

	if(dystringp(a))
	{
		if(dystringp(b))
		{
			return(dyopr_stradd(a, b));
		}
		return(dyopr_stradd(a, (dyt)dyToString(b)));
	}

	return(NULL);
}

BGBDY_API dyt dyor(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixintp(a) && dyfixintp(b))
		return(dyint(dyIntvF(a)|dyIntvF(b)));
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_OR, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)|dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)|dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_or(dyint128v(a), dyint128v(b))); return(c); }
	return(NULL);
}

BGBDY_API dyt dyxor(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixintp(a) && dyfixintp(b))
		return(dyint(dyIntvF(a)^dyIntvF(b)));
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_XOR, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)^dyintv(b)); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dylonglong(dylonglongv(a)^dylonglongv(b)); return(c); }
	if(dylongx2p(a) && dylongx2p(b))
		{ c=dyint128(iv128_xor(dyint128v(a), dyint128v(b))); return(c); }
	return(NULL);
}

BGBDY_API dyt dyshl(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixintp(a) && dyfixintp(b))
		return(dyint(dyIntvF(a)<<dyIntvF(b)));
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_SHL, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)<<dyintv(b)); return(c); }
	if(dylonglongp(a) && dyintp(b))
		{ c=dylonglong(dylonglongv(a)<<dyintv(b)); return(c); }
	if(dylongx2p(a) && dyintp(b))
		{ c=dyint128(iv128_shl(dyint128v(a), dyintv(b))); return(c); }
	return(NULL);
}

BGBDY_API dyt dyshr(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixintp(a) && dyfixintp(b))
		return(dyint(dyIntvF(a)>>dyIntvF(b)));
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_SHR, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyint(dyintv(a)>>dyintv(b)); return(c); }
	if(dylonglongp(a) && dyintp(b))
		{ c=dylonglong(dylonglongv(a)>>dyintv(b)); return(c); }
	if(dylongx2p(a) && dyintp(b))
		{ c=dyint128(iv128_shr(dyint128v(a), dyintv(b))); return(c); }
	return(NULL);
}

BGBDY_API dyt dyexp(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyflonum(pow(dyIntvF(a), dyIntvF(b))));
		return(dyflonum(pow(dyfloatv(a), dyfloatv(b))));
	}
	
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_EXP, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyfloat(pow(dyintv(a), dyintv(b))); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dydouble(pow(dylongv(a), dylongv(b))); return(c); }
	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(pow(dyfloatv(a), dyfloatv(b))); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(pow(dydoublev(a), dydoublev(b))); return(c); }
//	if(dycomplexp(a) && dycomplexp(b))
//		{ c=dydcomplex(dycomplexv(a)*dycomplexv(b)); return(c); }

	if(dyrealx2p(a) && dyrealx2p(b))
		return(dyfloat128(fv128_pow(dyfloat128v(a), dyfloat128v(b))));

	return(NULL);
}

BGBDY_API dyt dyATan2(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyflonum(atan2(dyIntvF(a), dyIntvF(b))));
		return(dyflonum(atan2(dyfloatv(a), dyfloatv(b))));
	}
	
	lty=dygettype(a); rty=dygettype(b);
	c=dyopr_doBinary(BGBDY_OPR_ATAN2, lty, rty, a, b);
	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyfloat(atan2(dyintv(a), dyintv(b))); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dydouble(atan2(dylongv(a), dylongv(b))); return(c); }
	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(atan2(dyfloatv(a), dyfloatv(b))); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(atan2(dydoublev(a), dydoublev(b))); return(c); }
//	if(dycomplexp(a) && dycomplexp(b))
//		{ c=dydcomplex(dycomplexv(a)*dycomplexv(b)); return(c); }

//	if(dyrealx2p(a) && dyrealx2p(b))
//		return(dyfloat128(fv128_atan2(dyfloat128v(a), dyfloat128v(b))));

	return(NULL);
}

static double spow(double a, double b)
	{ return((a>=0)?pow(a, b):(-pow(-a, b))); }

BGBDY_API dyt dySexp(dyt a, dyt b)
{
	char *lty, *rty;
	dyt c;

	if(dyfixrealp(a) && dyfixrealp(b))
	{
		if(dyfixintp(a) && dyfixintp(b))
			return(dyflonum(spow(dyIntvF(a), dyIntvF(b))));
		return(dyflonum(spow(dyfloatv(a), dyfloatv(b))));
	}
	
//	lty=dygettype(a); rty=dygettype(b);
//	c=dyopr_doBinary(BGBDY_OPR_EXP, lty, rty, a, b);
//	if(c!=UNDEFINED)return(c);
	
	if(dyintp(a) && dyintp(b))
		{ c=dyfloat(spow(dyintv(a), dyintv(b))); return(c); }
	if(dylonglongp(a) && dylonglongp(b))
		{ c=dydouble(spow(dylongv(a), dylongv(b))); return(c); }
	if(dyfloatp(a) && dyfloatp(b))
		{ c=dyfloat(spow(dyfloatv(a), dyfloatv(b))); return(c); }
	if(dyrealp(a) && dyrealp(b))
		{ c=dydouble(spow(dydoublev(a), dydoublev(b))); return(c); }
//	if(dycomplexp(a) && dycomplexp(b))
//		{ c=dydcomplex(dycomplexv(a)*dycomplexv(b)); return(c); }

//	if(dyrealx2p(a) && dyrealx2p(b))
//		return(dyfloat128(fv128_pow(dyfloat128v(a), dyfloat128v(b))));

	return(NULL);
}

BGBDY_API dyt dyneg(dyt a)
{
	char *ty;
	dyt c;

	if(dyintp(a))return(dyint(-dyintv(a)));
	if(dylonglongp(a))return(dylonglong(-dylonglongv(a)));
	if(dylongx2p(a))return(dyint128(iv128_neg(dyint128v(a))));
	if(dyfloatp(a))return(dyfloat(-dyfloatv(a)));
	if(dyrealp(a))return(dydouble(-dydoublev(a)));

	if(dyrealx2p(a))
		return(dyfloat128(fv128_neg(dyfloat128v(a))));

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_NEG, ty, a);
	if(c!=UNDEFINED)return(c);

	return(NULL);
}

BGBDY_API dyt dynot(dyt a)
{
	char *ty;
	dyt c;

	if(dyintp(a))return(dyint(~dyintv(a)));
	if(dylonglongp(a))return(dylonglong(~dylonglongv(a)));
	if(dylongx2p(a))return(dyint128(iv128_not(dyint128v(a))));

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_NOT, ty, a);
	if(c!=UNDEFINED)return(c);

	return(NULL);
}

BGBDY_API dyt dylnot(dyt a)
{
	char *ty;
	dyt c;

	if(a==BGBDY_TRUE)return(BGBDY_FALSE);
	if(a==BGBDY_FALSE)return(BGBDY_TRUE);

	if(dyintp(a))return(dybool(!dyintv(a)));
	if(dylonglongp(a))return(dybool(!dylonglongv(a)));
	if(dylongx2p(a))return(dybool(iv128_zerop(dyint128v(a))));

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_LNOT, ty, a);
	if(c!=UNDEFINED)return(c);

	return(dybool(dyfalsep(a)));
//	return(NULL);
}

BGBDY_API dyt dyumul(dyt a)
{
	char *ty, *s;
	void *p;
	int i;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_UMUL, ty, a);
	if(c!=UNDEFINED)return(c);

	if(dystringp(a))
	{
		s=(char *)a;
		i=BGBDY_PeekChar(&s);
		return(dychar(i));
//		return(dychar(*(char *)a));
//		return(dychar(*(char *)a));
	}

	if(dycArrayP(a))
	{
		c=dycGetArrayVar((dycArray)a, 0);
		return(c);
	}
	
	if(dycArrayRefP(a))
	{
		c=(dyt)dycArrayRefArray(a);
		if(!c)return(NULL);
		i=dycArrayRefIndex(a);
		c=dycGetArrayVar((dycArray)c, i);
		return(c);
	}

	if(dyllTypeboxPtrP(a))
	{
		s=dyllTypeboxPtrSig(a);
		p=dyllTypeboxPtrData(a);
		c=dyllGetVarSig(p, s);
		return(c);
	}

	return(NULL);
}

BGBDY_API dyt dyudiv(dyt a)
{
	char *ty;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_UDIV, ty, a);
	if(c!=UNDEFINED)return(c);

	return(NULL);
}

BGBDY_API dyt dyumod(dyt a)
{
	char *ty;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_UMOD, ty, a);
	if(c!=UNDEFINED)return(c);

	return(NULL);
}

BGBDY_API dyt dyuand(dyt a)
{
	char *ty;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_UAND, ty, a);
	if(c!=UNDEFINED)return(c);

	return(NULL);
}

BGBDY_API dyt dyuor(dyt a)
{
	char *ty;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_UOR, ty, a);
	if(c!=UNDEFINED)return(c);

	return(NULL);
}

BGBDY_API dyt dyuxor(dyt a)
{
	char *ty;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_UXOR, ty, a);
	if(c!=UNDEFINED)return(c);

	return(NULL);
}

BGBDY_API dyt dyinc(dyt a)
{
	char *ty;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_INC, ty, a);
	if(c!=UNDEFINED)return(c);

	c=dyadd(a, dyint(1));
	if(c)return(c);

	return(NULL);
}

BGBDY_API dyt dydec(dyt a)
{
	char *ty;
	dyt c;

	ty=dygettype(a);
	c=dyopr_doUnary(BGBDY_OPR_DEC, ty, a);
	if(c!=UNDEFINED)return(c);

	c=dysub(a, dyint(1));
	if(c)return(c);

	return(NULL);
}

BGBDY_API int dyeqp(dyt a, dyt b)
{
	char *lty, *rty;
//	dyt c;

	if(a==b)return(1);

	if(dyfixrealp(a) && dyfixrealp(b))
		{ return(dyfloatv(a)==dyfloatv(b)); }
//	lty=dygettype(a); rty=dygettype(b);
//	c=dyopr_doCompare(BGBDY_OPR_EQ, lty, rty, a, b);
//	if(c!=UNDEFINED)return(c);

	if(dyintp(a) && dyintp(b))
		return(dyintv(a)==dyintv(b));
	if(dylonglongp(a) && dylonglongp(b))
		return(dylonglongv(a)==dylonglongv(b));
	if(dylongx2p(a) && dylongx2p(b))
		return(iv128_cmp_eq(dyint128v(a), dyint128v(b)));
	if(dyrealp(a) && dyrealp(b))
		return(dydoublev(a)==dydoublev(b));

	if(dyrealx2p(a) && dyrealx2p(b))
		return(fv128_eqp(dyfloat128v(a), dyfloat128v(b)));

	if(dyllTypeboxSigP(a) || dyllTypeboxSigP(b))
	{
		if(dyllTypeboxSigP(a) && dystringp(b))
			return(dyllTypeboxSigIsStrP(a, (char *)b));
		if(dystringp(a) && dyllTypeboxSigP(b))
			return(dyllTypeboxSigIsStrP(b, (char *)a));
		if(dyllTypeboxSigP(a) && dyllTypeboxSigP(b))
			return(dyllTypeboxSigEqP(a, b));
		return(0);
	}

	if(dygettype(a)!=dygettype(b))
		return(0);

	if(dystringp(a) && dystringp(b))
		return(!strcmp((char *)a, (char *)b));
	return(0);
}

BGBDY_API int dyneqp(dyt a, dyt b)
{
	return(!dyeqp(a, b));
}

BGBDY_API int dyltp(dyt a, dyt b)
{
	if(dyfixrealp(a) && dyfixrealp(b))
		{ return(dyfloatv(a)<dyfloatv(b)); }

	if(dyintp(a) && dyintp(b))
		return(dyintv(a)<dyintv(b));
	if(dylonglongp(a) && dylonglongp(b))
		return(dylonglongv(a)<dylonglongv(b));
	if(dylongx2p(a) && dylongx2p(b))
		return(iv128_cmp_l(dyint128v(a), dyint128v(b)));
	if(dyrealp(a) && dyrealp(b))
		return(dydoublev(a)<dydoublev(b));
	if(dyrealx2p(a) && dyrealx2p(b))
		return(fv128_ltp(dyfloat128v(a), dyfloat128v(b)));
	return(0);
}

BGBDY_API int dygtp(dyt a, dyt b)
{
	if(dyfixrealp(a) && dyfixrealp(b))
		{ return(dyfloatv(a)>dyfloatv(b)); }

	if(dyintp(a) && dyintp(b))
		return(dyintv(a)>dyintv(b));
	if(dylonglongp(a) && dylonglongp(b))
		return(dylonglongv(a)>dylonglongv(b));
	if(dylongx2p(a) && dylongx2p(b))
		return(iv128_cmp_g(dyint128v(a), dyint128v(b)));
	if(dyrealp(a) && dyrealp(b))
		return(dydoublev(a)>dydoublev(b));
	if(dyrealx2p(a) && dyrealx2p(b))
		return(fv128_gtp(dyfloat128v(a), dyfloat128v(b)));
	return(0);
}

BGBDY_API int dylep(dyt a, dyt b)
{
	if(dyfixrealp(a) && dyfixrealp(b))
		{ return(dyfloatv(a)<=dyfloatv(b)); }

	if(dyintp(a) && dyintp(b))
		return(dyintv(a)<=dyintv(b));
	if(dylonglongp(a) && dylonglongp(b))
		return(dylonglongv(a)<=dylonglongv(b));
	if(dylongx2p(a) && dylongx2p(b))
		return(iv128_cmp_le(dyint128v(a), dyint128v(b)));
	if(dyrealp(a) && dyrealp(b))
		return(dydoublev(a)<=dydoublev(b));
	if(dyrealx2p(a) && dyrealx2p(b))
		return(fv128_lep(dyfloat128v(a), dyfloat128v(b)));
	return(0);
}

BGBDY_API int dygep(dyt a, dyt b)
{
	if(dyfixrealp(a) && dyfixrealp(b))
		{ return(dyfloatv(a)>=dyfloatv(b)); }

	if(dyintp(a) && dyintp(b))
		return(dyintv(a)>=dyintv(b));
	if(dylonglongp(a) && dylonglongp(b))
		return(dylonglongv(a)>=dylonglongv(b));
	if(dylongx2p(a) && dylongx2p(b))
		return(iv128_cmp_ge(dyint128v(a), dyint128v(b)));
	if(dyrealp(a) && dyrealp(b))
		return(dydoublev(a)>=dydoublev(b));
	if(dyrealx2p(a) && dyrealx2p(b))
		return(fv128_gep(dyfloat128v(a), dyfloat128v(b)));
	return(0);
}

BGBDY_API int dyunp(dyt a, dyt b)
{
	if(dylep(a, b))
		return(0);
	if(dygep(a, b))
		return(0);
	if(dyeqp(a, b))
		return(0);
	return(1);
}

/** Returns true if arguments have the same value and same type */
BGBDY_API int dyeqvp(dyt a, dyt b)
{
	char *lty, *rty;
//	dyt c;

	if(a==b)return(1);

	if(dyfixrealp(a) || dyfixrealp(b))
		{ return(0); }		//either type or value mismatch

	if(dyconsp(a) && dyconsp(b))		//compare lists
		return(dyListEqvP(a, b));

	if(dygettype(a)!=dygettype(b))
		return(0);

	if(dyintp(a) && dyintp(b))
		return(dyintv(a)==dyintv(b));
	if(dylonglongp(a) && dylonglongp(b))
		return(dylonglongv(a)==dylonglongv(b));
	if(dylongx2p(a) && dylongx2p(b))
		return(iv128_cmp_eq(dyint128v(a), dyint128v(b)));

	if(dyfloatp(a) && dyfloatp(b))
		return(dyfloatv(a)==dyfloatv(b));
	if(dydoublep(a) && dydoublep(b))
		return(dydoublev(a)==dydoublev(b));

	if(dyrealx2p(a) && dyrealx2p(b))
		return(fv128_eqp(dyfloat128v(a), dyfloat128v(b)));

	if(dyllTypeboxSigP(a) || dyllTypeboxSigP(b))
	{
		if(dyllTypeboxSigP(a) && dyllTypeboxSigP(b))
			return(dyllTypeboxSigEqP(a, b));
		return(0);
	}

	if(dystringp(a) && dystringp(b))
		return(!strcmp((char *)a, (char *)b));

	return(0);
}


BGBDY_API int dytruep(dyt a)
{
	if(!a)return(0);
	if(a==UNDEFINED)return(0);
	if(a==BGBDY_FALSE)return(0);
	if(a==BGBDY_TRUE)return(1);

	if(dyintp(a) && !dyIntvF(a))return(0);
	return(1);
}

BGBDY_API int dyfalsep(dyt a)
{
	if(!a)return(1);
	if(a==UNDEFINED)return(1);
	if(a==BGBDY_FALSE)return(1);
	if(a==BGBDY_TRUE)return(0);

	if(dyintp(a) && !dyIntvF(a))return(1);
	return(0);
}


BGBDY_API dyt dysqr(dyt a)
{
	if(dyintp(a))return(dylonglong(dyintv(a)*dyintv(a)));
	if(dylonglongp(a))return(dylonglong(dylonglongv(a)*dylonglongv(a)));
	if(dyfloatp(a))return(dyfloat(dyfloatv(a)*dyfloatv(a)));
	if(dyrealp(a))return(dydouble(dydoublev(a)*dydoublev(a)));
	return(NULL);
}

BGBDY_API dyt dysqrt(dyt a)
{
	if(dyfloatp(a))return(dyfloat(sqrt(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(sqrt(dydoublev(a))));
	return(NULL);
}

BGBDY_API dyt dyssqr(dyt a)
{
	double f;
	s64 lv;

	if(dyintp(a) || dylongp(a))
	{
		lv=dylongv(a);
		lv=(lv>=0)?(lv*lv):(-lv*lv);
		return(dylong(lv));
	}

	if(dyrealp(a))
	{
		f=dylongv(a);
		f=(f>=0)?(f*f):(-f*f);
		return(dydouble(f));
	}

//	if(dyintp(a))return(dylonglong(dyintv(a)*dyintv(a)));
//	if(dylonglongp(a))return(dylonglong(dylonglongv(a)*dylonglongv(a)));
	if(dyfloatp(a))return(dyfloat(dyfloatv(a)*dyfloatv(a)));
	if(dyrealp(a))return(dydouble(dydoublev(a)*dydoublev(a)));
	return(NULL);
}

static double ssqrt(double a)
	{ return((a>=0)?sqrt(a):(-sqrt(-a))); }

BGBDY_API dyt dyssqrt(dyt a)
{
	if(dyfloatp(a))return(dyfloat(ssqrt(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(ssqrt(dydoublev(a))));
	return(NULL);
}

BGBDY_API dyt dySin(dyt a)
{
	if(dyfloatp(a))return(dyfloat(sin(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(sin(dydoublev(a))));
	return(NULL);
}

BGBDY_API dyt dyCos(dyt a)
{
	if(dyfloatp(a))return(dyfloat(cos(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(cos(dydoublev(a))));
	return(NULL);
}

BGBDY_API dyt dyTan(dyt a)
{
	if(dyfloatp(a))return(dyfloat(tan(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(tan(dydoublev(a))));
	return(NULL);
}

BGBDY_API dyt dyASin(dyt a)
{
	if(dyfloatp(a))return(dyfloat(asin(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(asin(dydoublev(a))));
	return(NULL);
}

BGBDY_API dyt dyACos(dyt a)
{
	if(dyfloatp(a))return(dyfloat(acos(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(acos(dydoublev(a))));
	return(NULL);
}

BGBDY_API dyt dyATan(dyt a)
{
	if(dyfloatp(a))return(dyfloat(atan(dyfloatv(a))));
	if(dyrealp(a))return(dydouble(atan(dydoublev(a))));
	return(NULL);
}


BGBDY_API dyt dyEqv(dyt a, dyt b)
	{ return(dybool(dyeqp(a,b))); }
BGBDY_API dyt dyNeqv(dyt a, dyt b)
	{ return(dybool(dyneqp(a,b))); }
BGBDY_API dyt dyEq(dyt a, dyt b)
	{ return(dybool(a==b)); }
BGBDY_API dyt dyNeq(dyt a, dyt b)
	{ return(dybool(a!=b)); }

BGBDY_API dyt dyLt(dyt a, dyt b)
	{ return(dybool(dyltp(a,b))); }
BGBDY_API dyt dyGt(dyt a, dyt b)
	{ return(dybool(dygtp(a,b))); }
BGBDY_API dyt dyLe(dyt a, dyt b)
	{ return(dybool(dylep(a,b))); }
BGBDY_API dyt dyGe(dyt a, dyt b)
	{ return(dybool(dygep(a,b))); }
