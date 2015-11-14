#include <bgbdy.h>

BGBDY_API dyt dyint128(iv128_t v)
{
	dyt t;
	t=gctatomic("_int128_t", sizeof(iv128_t));
	*(iv128_t *)t=v;
	return(t);
}

BGBDY_API dyt dyint128ll(s64 v)
{
	dyt t;
	t=gctatomic("_int128_t", sizeof(iv128_t));
	*(iv128_t *)t=iv128_mkll(v);
	return(t);
}

BGBDY_API dyt dyint128str(char *str)
{
	dyt t;
	t=gctatomic("_int128_t", sizeof(iv128_t));
	*(iv128_t *)t=atoxl_iv(str);
	return(t);
}

BGBDY_API int dyint128p(dyt p)
{
	return(dytypep(p, "_int128_t"));
}

BGBDY_API iv128_t dyint128v(dyt v)
{
	if(dytypep(v, "_int128_t"))
	{
		return(*(iv128_t *)v);
	}
	return(iv128_mkll(dylongv(v)));
}

BGBDY_API s64 iv128_s64v(iv128_t v)
{
	s64 t;
	t=v.a + (((s64)v.b)<<32);
	return(t);
}

BGBDY_API iv128_t iv128_mkll(s64 v)
{
	iv128_t val;
	u32 a, b, c;
	a=(u32)(v&0xFFFFFFFF);
	b=(u32)((v>>32)&0xFFFFFFFF);
	c=(u32)((v<0)?0xFFFFFFFF:0x00000000);	
	val.a=a; val.b=b; val.c=c; val.d=c;
	return(val);
}

BGBDY_API iv128_t iv128_mk2(u64 a, u64 b)
{
	return(iv128_mk4((u32)a, (u32)(a>>32), (u32)b, (u32)(b>>32)));
}

BGBDY_API iv128_t iv128_mk4(u32 a, u32 b, u32 c, u32 d)
{
	iv128_t val;
	
	val.a=a; val.b=b; val.c=c; val.d=d;
	return(val);
}

BGBDY_API iv128_t iv128_mulb(iv128_t a, int b)
{
	iv128_t c;
	u64 li;

	li=(((u64)a.a)*b)+0; c.a=li;
	li=(((u64)a.b)*b)+(li>>32); c.b=li;
	li=(((u64)a.c)*b)+(li>>32); c.c=li;
	li=(((u64)a.d)*b)+(li>>32); c.d=li;
	return(c);
}

BGBDY_API iv128_t iv128_divb(iv128_t a, int b)
{
	iv128_t c;
	u64 li;

#if 1
//	li=(a.a/b)+(((u64)(a.b))<<32)/b;
	li=((a.a)+(((u64)(a.b))<<32))/b;
	c.a=li;
	li=((a.b)+(((u64)(a.c))<<32))/b;
//	li=(((u64)(a.c))<<32)/b+(li>>32);
	c.b=li;
	li=(((u64)(a.d))<<32)/b+(li>>32);
	c.c=li;
	li=(li>>32);
	c.d=li;
#endif

#if 0
	c.d=a.d/b;	c.c=a.c/b;
	c.b=a.b/b;	c.a=a.a/b;
	c.c+=(((u64)(a.d))<<32)/b;
	c.b+=(((u64)(a.c))<<32)/b;
	c.a+=(((u64)(a.b))<<32)/b;
#endif

	return(c);
}

BGBDY_API iv128_t atoxl_iv(char *str)
{
	iv128_t val;
	char *s;
	u64 li;
	int i, sg;

	s=str;
	sg=0; if(*s=='-') { sg=1; s++; }

	val.a=0; val.b=0; val.c=0; val.d=0;

	if((s[0]=='0') && (s[1]=='x'))
	{
		s+=2;

		while(*s)
		{
			i=-1;
			if((*s>='0') && (*s<='9'))i=*s-'0';
			if((*s>='A') && (*s<='F'))i=*s-'A'+10;
			if((*s>='a') && (*s<='f'))i=*s-'a'+10;
			if(i<0)break;
			s++;

			li=(((u64)val.a)*16)+i; val.a=li;
			li=(((u64)val.b)*16)+(li>>32); val.b=li;
			li=(((u64)val.c)*16)+(li>>32); val.c=li;
			li=(((u64)val.d)*16)+(li>>32); val.d=li;
		}
	}else if(s[0]=='0')
	{
		while(*s)
		{
			if((*s<'0') || (*s>'7'))break;
			li=(((u64)val.a)*8)+((*s++)-'0'); val.a=li;
			li=(((u64)val.b)*8)+(li>>32); val.b=li;
			li=(((u64)val.c)*8)+(li>>32); val.c=li;
			li=(((u64)val.d)*8)+(li>>32); val.d=li;
		}
	}else
	{
		while(*s)
		{
			if((*s<'0') || (*s>'9'))break;
			li=(((u64)val.a)*10)+((*s++)-'0'); val.a=li;
			li=(((u64)val.b)*10)+(li>>32); val.b=li;
			li=(((u64)val.c)*10)+(li>>32); val.c=li;
			li=(((u64)val.d)*10)+(li>>32); val.d=li;
		}
	}

	if(sg)
	{
		val.d=~val.d;	val.c=~val.c;
		val.b=~val.b;	val.a=~val.a;
		li=((u64)val.a)+1;
		li=((u64)val.b)+(li>>32); val.b=li;
		li=((u64)val.c)+(li>>32); val.c=li;
		li=((u64)val.d)+(li>>32); val.d=li;
	}

	return(val);
}

BGBDY_API char *xltoa_iv(iv128_t val)
{
	static char tmp[128];
	iv128_t tv, tv1, tv2, c10;
	char *t;
	u64 li;
	int i, sg;

//	printf("xltoa_iv: %08X'%08X'%08X'%08X\n",
//		val.d, val.c, val.b, val.a);

	c10=iv128_mkll(10);

	sg=0;
	if(val.d&0x80000000)
		{ sg=1; val=iv128_neg(val); }

	t=tmp+128;
	*(--t)=0;
	while(1)
	{
//		if(!val.a && !val.b && !val.c && !val.d)break;
		if(iv128_zerop(val))break;

		tv=iv128_div(val, c10);
//		tv=iv128_divb(val, 10);
		tv1=iv128_mulb(tv, 10);

		i=val.a-tv1.a;

//		printf("\ttv =%08X'%08X'%08X'%08X\n",
//			tv.d, tv.c, tv.b, tv.a);
//		printf("\ttv1=%08X'%08X'%08X'%08X %d\n",
//			tv1.d, tv1.c, tv1.b, tv1.a, i);
		
		*(--t)=i+'0';
		val.d=tv.d;	val.c=tv.c;
		val.b=tv.b;	val.a=tv.a;
	}
	if(!(*t))*(--t)='0';
	if(sg)	*(--t)='-';

	return(t);
}

BGBDY_API void iv128_zero(iv128_t *val)
{
	val->a=0; val->b=0; val->c=0; val->d=0;
}

BGBDY_API void iv256_zero(iv256_t *val)
{
	val->a=0; val->b=0; val->c=0; val->d=0;
	val->e=0; val->f=0; val->g=0; val->h=0;
}

BGBDY_API int iv128_zerop(iv128_t val)
{
	return(!val.a && !val.b && !val.c && !val.d);
}

BGBDY_API int iv128_negativep(iv128_t val)
{
	return((val.d>>31)&1);
}

BGBDY_API iv128_t iv128_neg(iv128_t val)
{
	u64 li;

	val.d=~val.d;	val.c=~val.c;
	val.b=~val.b;	val.a=~val.a;
	li=((u64)val.a)+1; 			val.a=li;
	li=((u64)val.b)+(li>>32);	val.b=li;
	li=((u64)val.c)+(li>>32);	val.c=li;
	li=((u64)val.d)+(li>>32);	val.d=li;
	return(val);
}

BGBDY_API iv128_t iv128_not(iv128_t val)
{
	val.d=~val.d;	val.c=~val.c;
	val.b=~val.b;	val.a=~val.a;
	return(val);
}

BGBDY_API iv128_t iv128_shl(iv128_t a, int b)
{
	iv128_t c;

	if(!b)return(a);
	if(b<0)return(iv128_shr(a, -b));
	if(b>=32)
	{
		c.d=a.c; c.c=a.b; c.b=a.a; c.a=0;
		c=iv128_shl(c, b-32);
		return(c);
	}

	c.d=(a.d<<b)|(a.c>>(32-b));
	c.c=(a.c<<b)|(a.b>>(32-b));
	c.b=(a.b<<b)|(a.a>>(32-b));
	c.a=(a.a<<b);
	return(c);
}

BGBDY_API iv128_t iv128_shr(iv128_t a, int b)
{
	iv128_t c;

	if(!b)return(a);
	if(b<0)return(iv128_shl(a, -b));
	if(b>=32)
	{
		c.a=a.b; c.b=a.c; c.c=a.d; c.d=0;
		c=iv128_shr(c, b-32);
		return(c);
	}

	c.a=(a.a>>b)|(a.b<<(32-b));
	c.b=(a.b>>b)|(a.c<<(32-b));
	c.c=(a.c>>b)|(a.d<<(32-b));
	c.d=(a.d>>b);
	return(c);
}

BGBDY_API iv128_t iv128_sar(iv128_t a, int b)
{
	iv128_t c;
	u32 e;

	if(!b)return(a);

	e=(a.d&0x80000000)?0xFFFFFFFF:0x00000000;
	if(b>=32)
	{
		c.a=a.b; c.b=a.c; c.c=a.d; c.d=e;
		c=iv128_shr(c, b-32);
		return(c);
	}

	c.a=(a.a>>b)|(a.b<<(32-b));
	c.b=(a.b>>b)|(a.c<<(32-b));
	c.c=(a.c>>b)|(a.d<<(32-b));
	c.d=(a.d>>b)|(e<<(32-b));
	return(c);
}

BGBDY_API iv128_t iv128_rol(iv128_t a, int b)
{
	iv128_t c;

	if(!b)return(a);
	if(b>=32)
	{
		c.d=a.c; c.c=a.b; c.b=a.a; c.a=a.d;
		c=iv128_rol(c, b-32);
		return(c);
	}

	c.d=(a.d<<b)|(a.c>>(32-b));
	c.c=(a.c<<b)|(a.b>>(32-b));
	c.b=(a.b<<b)|(a.a>>(32-b));
	c.a=(a.a<<b)|(a.d>>(32-b));
	return(c);
}

BGBDY_API iv128_t iv128_ror(iv128_t a, int b)
{
	iv128_t c;

	if(!b)return(a);
	if(b>=32)
	{
		c.a=a.b; c.b=a.c; c.c=a.d; c.d=a.a;
		c=iv128_ror(c, b-32);
		return(c);
	}

	c.a=(a.a>>b)|(a.b<<(32-b));
	c.b=(a.b>>b)|(a.c<<(32-b));
	c.c=(a.c>>b)|(a.d<<(32-b));
	c.d=(a.d>>b)|(a.a<<(32-b));
	return(c);
}

BGBDY_API int iv128_cmp_eq(iv128_t a, iv128_t b)
{
	if(a.d!=b.d)return(0);
	if(a.c!=b.c)return(0);
	if(a.b!=b.b)return(0);
	if(a.a!=b.a)return(0);
	return(1);
}

BGBDY_API int iv128_cmp_g(iv128_t a, iv128_t b)
{
	if(a.d<b.d)return(0);
	if(a.d>b.d)return(1);
	if(a.c<b.c)return(0);
	if(a.c>b.c)return(1);
	if(a.b<b.b)return(0);
	if(a.b>b.b)return(1);
	if(a.a<=b.a)return(0);
	return(1);
}

BGBDY_API int iv128_cmp_ge(iv128_t a, iv128_t b)
{
	if(a.d<b.d)return(0);
	if(a.d>b.d)return(1);
	if(a.c<b.c)return(0);
	if(a.c>b.c)return(1);
	if(a.b<b.b)return(0);
	if(a.b>b.b)return(1);
	if(a.a<b.a)return(0);
	return(1);
}

BGBDY_API int iv128_cmp_l(iv128_t a, iv128_t b)
	{ return(!iv128_cmp_ge(a, b)); }
BGBDY_API int iv128_cmp_le(iv128_t a, iv128_t b)
	{ return(!iv128_cmp_g(a, b)); }

BGBDY_API iv128_t iv128_and(iv128_t a, iv128_t b)
{
	iv128_t c;
	c.a=a.a&b.a;	c.b=a.a&b.b;
	c.c=a.a&b.c;	c.d=a.a&b.d;
	return(c);
}

BGBDY_API iv128_t iv128_or(iv128_t a, iv128_t b)
{
	iv128_t c;
	c.a=a.a|b.a;	c.b=a.a|b.b;
	c.c=a.a|b.c;	c.d=a.a|b.d;
	return(c);
}

BGBDY_API iv128_t iv128_xor(iv128_t a, iv128_t b)
{
	iv128_t c;
	c.a=a.a^b.a;	c.b=a.a^b.b;
	c.c=a.a^b.c;	c.d=a.a^b.d;
	return(c);
}

BGBDY_API iv128_t iv128_add(iv128_t a, iv128_t b)
{
	iv128_t c;
	u64 li;

	li=((u64)a.a)+((u64)b.a); c.a=li;
	li=(li>>32)+((u64)a.b)+((u64)b.b); c.b=li;
	li=(li>>32)+((u64)a.c)+((u64)b.c); c.c=li;
	li=(li>>32)+((u64)a.d)+((u64)b.d); c.d=li;
	return(c);
}

BGBDY_API iv128_t iv128_sub(iv128_t a, iv128_t b)
{
	iv128_t c;

	c=iv128_neg(b);
	c=iv128_add(a, c);
	return(c);
}

BGBDY_API iv128_t iv128_mul(iv128_t a, iv128_t b)
{
	iv128_t c;
	u64 li;

	li=((u64)a.a)*b.a; c.a=li;
	li=(li>>32) + ((u64)a.a)*b.b + ((u64)a.b)*b.a; c.b=li;
	li=(li>>32) + ((u64)a.a)*b.c + ((u64)a.c)*b.a +
		((u64)a.b)*b.b; c.c=li;
	li=(li>>32) + ((u64)a.a)*b.d + ((u64)a.d)*b.a +
		((u64)a.b)*b.c + ((u64)a.c)*b.b; c.d=li;

	return(c);
}

BGBDY_API iv128_t iv128_mulh(iv128_t a, iv128_t b)
{
	iv128_t c;
	u64 li;

	li=((u64)a.b)*b.d + ((u64)a.d)*b.b + ((u64)a.c)*b.c; c.a=li;
	li=(li>>32) + ((u64)a.c)*b.d + ((u64)a.d)*b.c; c.b=li;
	li=(li>>32) + ((u64)a.d)*b.d; c.c=li;
	c.d=li>>32;

	return(c);
}

BGBDY_API iv256_t iv128_mulw(iv128_t a, iv128_t b)
{
	iv256_t c;
	u64 li;

	li=((u64)a.a)*b.a; c.a=li;
	li=(li>>32) + ((u64)a.a)*b.b + ((u64)a.b)*b.a; c.b=li;
	li=(li>>32) + ((u64)a.a)*b.c + ((u64)a.c)*b.a +
		((u64)a.b)*b.b; c.c=li;
	li=(li>>32) + ((u64)a.a)*b.d + ((u64)a.d)*b.a +
		((u64)a.b)*b.c + ((u64)a.c)*b.b; c.d=li;
	li=(li>>32) + ((u64)a.b)*b.d + ((u64)a.d)*b.b +
		((u64)a.c)*b.c; c.e=li;
	li=(li>>32) + ((u64)a.c)*b.d + ((u64)a.d)*b.c; c.f=li;
	li=(li>>32) + ((u64)a.d)*b.d; c.g=li;
	c.h=li>>32;

	return(c);
}


BGBDY_API iv128_t iv128_udiv(iv128_t a, iv128_t b)
{
	iv256_t c, d;
	iv128_t e, f;
	int i;

	iv256_zero(&c);
	iv256_zero(&d);

	c.a=a.a; c.b=a.b; c.c=a.c; c.d=a.d;
	d.e=b.a; d.f=b.b; d.g=b.c; d.h=b.d;

	iv128_zero(&e);
	iv128_zero(&f);
	f.d=0x80000000;

	d=iv256_shr(d, 1);
	for(i=0; i<128; i++)
	{
		if(iv256_cmp_ge(c, d))
		{
			c=iv256_sub(c, d);
			e=iv128_add(e, f);
		}

		d=iv256_shr(d, 1);
		f=iv128_shr(f, 1);
	}

	return(e);
}

BGBDY_API iv128_t iv128_umod(iv128_t a, iv128_t b)
{
	iv256_t c, d;
	iv128_t e;
	int i;

	iv256_zero(&c);
	iv256_zero(&d);

	c.a=a.a; c.b=a.b; c.c=a.c; c.d=a.d;
	d.e=b.a; d.f=b.b; d.g=b.c; d.h=b.d;

	d=iv256_shr(d, 1);
	for(i=0; i<128; i++)
	{
		if(iv256_cmp_ge(c, d))
			c=iv256_sub(c, d);
		d=iv256_shr(d, 1);
	}

	e.a=c.a; e.b=c.b; e.c=c.c; e.d=c.d;
	return(e);
}

BGBDY_API iv128_t iv128_div(iv128_t a, iv128_t b)
{
	iv128_t c;
	int sg;

	sg=0;
	if(a.d&0x80000000) { sg=!sg; a=iv128_neg(a); }
	if(b.d&0x80000000) { sg=!sg; b=iv128_neg(b); }

	c=iv128_udiv(a, b);
	if(sg) c=iv128_neg(c);
	return(c);
}

BGBDY_API iv128_t iv128_mod(iv128_t a, iv128_t b)
{
	iv128_t c;
	int sg;

	sg=0;
	if(a.d&0x80000000) { sg=!sg; a=iv128_neg(a); }
	if(b.d&0x80000000) { sg=!sg; b=iv128_neg(b); }

	c=iv128_umod(a, b);
//	if(sg) c=iv128_neg(c);
	return(c);
}


BGBDY_API iv256_t iv256_neg(iv256_t val)
{
	u64 li;

	val.h=~val.h;	val.g=~val.g;
	val.f=~val.f;	val.e=~val.e;
	val.d=~val.d;	val.c=~val.c;
	val.b=~val.b;	val.a=~val.a;
	li=((u64)val.a)+1;			val.a=li;
	li=((u64)val.b)+(li>>32);	val.b=li;
	li=((u64)val.c)+(li>>32);	val.c=li;
	li=((u64)val.d)+(li>>32);	val.d=li;
	li=((u64)val.e)+(li>>32);	val.e=li;
	li=((u64)val.f)+(li>>32);	val.f=li;
	li=((u64)val.g)+(li>>32);	val.g=li;
	li=((u64)val.h)+(li>>32);	val.h=li;
	return(val);
}

BGBDY_API iv256_t iv256_shl(iv256_t a, int b)
{
	iv256_t c;

	if(!b)return(a);
	if(b>=32)
	{
		c.h=a.g; c.g=a.f; c.f=a.e; c.e=a.d;
		c.d=a.c; c.c=a.b; c.b=a.a; c.a=0;
		c=iv256_shl(c, b-32);
		return(c);
	}

	c.h=(a.h<<b)|(a.g>>(32-b));
	c.g=(a.g<<b)|(a.f>>(32-b));
	c.f=(a.f<<b)|(a.e>>(32-b));
	c.e=(a.e<<b)|(a.d>>(32-b));
	c.d=(a.d<<b)|(a.c>>(32-b));
	c.c=(a.c<<b)|(a.b>>(32-b));
	c.b=(a.b<<b)|(a.a>>(32-b));
	c.a=(a.a<<b);
	return(c);
}

BGBDY_API iv256_t iv256_shr(iv256_t a, int b)
{
	iv256_t c;

	if(!b)return(a);
	if(b>=32)
	{
		c.a=a.b; c.b=a.c; c.c=a.d; c.d=a.e;
		c.e=a.f; c.f=a.g; c.g=a.h; c.h=0;
		c=iv256_shr(c, b-32);
		return(c);
	}

	c.a=(a.a>>b)|(a.b<<(32-b));
	c.b=(a.b>>b)|(a.c<<(32-b));
	c.c=(a.c>>b)|(a.d<<(32-b));
	c.d=(a.d>>b)|(a.e<<(32-b));
	c.e=(a.e>>b)|(a.f<<(32-b));
	c.f=(a.f>>b)|(a.g<<(32-b));
	c.g=(a.g>>b)|(a.h<<(32-b));
	c.h=(a.h>>b);
	return(c);
}

BGBDY_API int iv256_cmp_eq(iv256_t a, iv256_t b)
{
	if(a.h!=b.h)return(0);
	if(a.g!=b.g)return(0);
	if(a.f!=b.f)return(0);
	if(a.e!=b.e)return(0);
	if(a.d!=b.d)return(0);
	if(a.c!=b.c)return(0);
	if(a.b!=b.b)return(0);
	if(a.a!=b.a)return(0);
	return(1);
}

BGBDY_API int iv256_cmp_g(iv256_t a, iv256_t b)
{
	if(a.h<b.h)return(0);
	if(a.h>b.h)return(1);
	if(a.g<b.g)return(0);
	if(a.g>b.g)return(1);
	if(a.f<b.f)return(0);
	if(a.f>b.f)return(1);
	if(a.e<b.e)return(0);
	if(a.e>b.e)return(1);
	if(a.d<b.d)return(0);
	if(a.d>b.d)return(1);
	if(a.c<b.c)return(0);
	if(a.c>b.c)return(1);
	if(a.b<b.b)return(0);
	if(a.b>b.b)return(1);
	if(a.a<=b.a)return(0);
	return(1);
}

BGBDY_API int iv256_cmp_ge(iv256_t a, iv256_t b)
{
	if(a.h<b.h)return(0);
	if(a.h>b.h)return(1);
	if(a.g<b.g)return(0);
	if(a.g>b.g)return(1);
	if(a.f<b.f)return(0);
	if(a.f>b.f)return(1);
	if(a.e<b.e)return(0);
	if(a.e>b.e)return(1);
	if(a.d<b.d)return(0);
	if(a.d>b.d)return(1);
	if(a.c<b.c)return(0);
	if(a.c>b.c)return(1);
	if(a.b<b.b)return(0);
	if(a.b>b.b)return(1);
	if(a.a<b.a)return(0);
	return(1);
}

BGBDY_API int iv256_cmp_l(iv256_t a, iv256_t b)
	{ return(!iv256_cmp_ge(a, b)); }
BGBDY_API int iv256_cmp_le(iv256_t a, iv256_t b)
	{ return(!iv256_cmp_g(a, b)); }

BGBDY_API iv256_t iv256_add(iv256_t a, iv256_t b)
{
	iv256_t c;
	u64 li;

	li=((u64)a.a)+((u64)b.a); c.a=li;
	li=(li>>32)+((u64)a.b)+((u64)b.b); c.b=li;
	li=(li>>32)+((u64)a.c)+((u64)b.c); c.c=li;
	li=(li>>32)+((u64)a.d)+((u64)b.d); c.d=li;
	li=(li>>32)+((u64)a.e)+((u64)b.e); c.e=li;
	li=(li>>32)+((u64)a.f)+((u64)b.f); c.f=li;
	li=(li>>32)+((u64)a.g)+((u64)b.g); c.g=li;
	li=(li>>32)+((u64)a.h)+((u64)b.h); c.h=li;
	return(c);
}

BGBDY_API iv256_t iv256_sub(iv256_t a, iv256_t b)
{
	iv256_t c;

	c=iv256_neg(b);
	c=iv256_add(a, c);
	return(c);
}

