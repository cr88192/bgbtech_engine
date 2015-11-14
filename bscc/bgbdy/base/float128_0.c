#include <bgbgc.h>

f128_t f128_atof(char *str)
{
	iv128_t val;
	char *s;
	u64 li;
	int i, sg;

	s=str;
	sg=0; if(*s=='-') { sg=1; s++; }

	val.a=0; val.b=0; val.c=0; val.d=0;

	while(*s)
	{
		if((*s<'0') || (*s>'9'))break;
		li=(((u64)val.a)*10)+((*s++)-'0'); val.a=li;
		li=(((u64)val.b)*10)+(li>>32); val.b=li;
		li=(((u64)val.c)*10)+(li>>32); val.c=li;
		li=(((u64)val.d)*10)+(li>>32); val.d=li;
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

char *f128_ftoa(f128_t val)
{
	static char tmp[128];
	iv128_t tv;
	char *t;
	u64 li;
	int i, sg;

	if(val.d&0x80000000)
	{
		sg=1;
		val.d=~val.d;	val.c=~val.c;
		val.b=~val.b;	val.a=~val.a;
		li=((u64)val.a)+1;
		li=((u64)val.b)+(li>>32); val.b=li;
		li=((u64)val.c)+(li>>32); val.c=li;
		li=((u64)val.d)+(li>>32); val.d=li;
	}

	t=tmp+128;
	*(--t)=0;
	while(1)
	{
		if(!val.a && !val.b && !val.c && !val.d)break;
		tv.d=val.d/10;	tv.c=val.c/10;
		tv.b=val.b/10;	tv.a=val.a/10;
		tv.c+=(((u64)(val.d))<<32)/10;
		tv.b+=(((u64)(val.c))<<32)/10;
		tv.a+=(((u64)(val.b))<<32)/10;
		i=val.a%10;	*(--t)=i+'0';
		val.d=tv.d;	val.c=tv.c;
		val.b=tv.b;	val.a=tv.a;
	}
	if(sg)	*(--t)='-';

	return(t);
}

void f128_zero(f128_t *val)
{
	val->a=0; val->b=0; val->c=0; val->d=0;
}

f128_t f128_neg(f128_t val)
{
	u64 li;

	val.d=~val.d;	val.c=~val.c;
	val.b=~val.b;	val.a=~val.a;
	li=((u64)val.a)+1;
	li=((u64)val.b)+(li>>32); val.b=li;
	li=((u64)val.c)+(li>>32); val.c=li;
	li=((u64)val.d)+(li>>32); val.d=li;
	return(val);
}

iv128_t iv128_shl(iv128_t a, int b)
{
	iv128_t c;

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

iv128_t iv128_shr(iv128_t a, int b)
{
	iv128_t c;

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

iv128_t iv128_sar(iv128_t a, int b)
{
	iv128_t c;
	u32 e;

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

iv128_t iv128_rol(iv128_t a, int b)
{
	iv128_t c;

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

iv128_t iv128_ror(iv128_t a, int b)
{
	iv128_t c;

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

int iv128_cmp_eq(iv128_t a, iv128_t b)
{
	if(a.d!=b.d)return(0);
	if(a.c!=b.c)return(0);
	if(a.b!=b.b)return(0);
	if(a.a!=b.a)return(0);
	return(1);
}

int iv128_cmp_g(iv128_t a, iv128_t b)
{
	if(a.d<b.d)return(0);
	if(a.c<b.c)return(0);
	if(a.b<b.b)return(0);
	if(a.a<=b.a)return(0);
	return(1);
}

int iv128_cmp_ge(iv128_t a, iv128_t b)
{
	if(a.d<b.d)return(0);
	if(a.c<b.c)return(0);
	if(a.b<b.b)return(0);
	if(a.a<b.a)return(0);
	return(1);
}

int iv128_cmp_l(iv128_t a, iv128_t b)
	{ return(!iv128_cmp_ge(a, b)); }
int iv128_cmp_le(iv128_t a, iv128_t b)
	{ return(!iv128_cmp_g(a, b)); }


iv128_t iv128_add(iv128_t a, iv128_t b)
{
	iv128_t c;
	u64 li;

	li=((u64)a.a)+((u64)b.a); c.a=li;
	li=(li>>32)+((u64)a.b)+((u64)b.b); c.b=li;
	li=(li>>32)+((u64)a.c)+((u64)b.c); c.c=li;
	li=(li>>32)+((u64)a.d)+((u64)b.d); c.d=li;
	return(c);
}

iv128_t iv128_sub(iv128_t a, iv128_t b)
{
	iv128_t c;

	c=iv128_neg(b);
	c=iv128_add(a, c);
	return(c);
}

iv128_t iv128_mul(iv128_t a, iv128_t b)
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

iv128_t iv128_mulh(iv128_t a, iv128_t b)
{
	iv128_t c;
	u64 li;

	li=((u64)a.b)*b.d + ((u64)a.d)*b.b + ((u64)a.c)*b.c; c.a=li;
	li=(li>>32) + ((u64)a.c)*b.d + ((u64)a.d)*b.c; c.b=li;
	li=(li>>32) + ((u64)a.d)*b.d; c.c=li;
	c.d=li>>32;

	return(c);
}

iv256_t iv128_mulw(iv128_t a, iv128_t b)
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


iv128_t iv128_udiv(iv128_t a, iv128_t b)
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

iv128_t iv128_div(iv128_t a, iv128_t b)
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

iv128_t iv128_umod(iv128_t a, iv128_t b)
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

iv128_t iv128_mod(iv128_t a, iv128_t b)
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


iv256_t iv256_neg(iv256_t val)
{
	u64 li;

	val.h=~val.h;	val.g=~val.g;
	val.f=~val.f;	val.e=~val.e;
	val.d=~val.d;	val.c=~val.c;
	val.b=~val.b;	val.a=~val.a;
	li=((u64)val.a)+1;
	li=((u64)val.b)+(li>>32); val.b=li;
	li=((u64)val.c)+(li>>32); val.c=li;
	li=((u64)val.d)+(li>>32); val.d=li;
	li=((u64)val.e)+(li>>32); val.e=li;
	li=((u64)val.f)+(li>>32); val.f=li;
	li=((u64)val.g)+(li>>32); val.g=li;
	li=((u64)val.h)+(li>>32); val.h=li;
	return(val);
}

iv256_t iv256_shl(iv256_t a, int b)
{
	iv256_t c;

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

iv256_t iv256_shr(iv256_t a, int b)
{
	iv256_t c;

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

int iv256_cmp_eq(iv256_t a, iv256_t b)
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

int iv256_cmp_g(iv256_t a, iv256_t b)
{
	if(a.h<b.h)return(0);
	if(a.g<b.g)return(0);
	if(a.f<b.f)return(0);
	if(a.e<b.e)return(0);
	if(a.d<b.d)return(0);
	if(a.c<b.c)return(0);
	if(a.b<b.b)return(0);
	if(a.a<=b.a)return(0);
	return(1);
}

int iv256_cmp_ge(iv256_t a, iv256_t b)
{
	if(a.h<b.h)return(0);
	if(a.g<b.g)return(0);
	if(a.f<b.f)return(0);
	if(a.e<b.e)return(0);
	if(a.d<b.d)return(0);
	if(a.c<b.c)return(0);
	if(a.b<b.b)return(0);
	if(a.a<=b.a)return(0);
	return(1);
}

int iv256_cmp_l(iv256_t a, iv256_t b)
	{ return(!iv256_cmp_ge(a, b)); }
int iv256_cmp_le(iv256_t a, iv256_t b)
	{ return(!iv256_cmp_g(a, b)); }

iv256_t iv256_add(iv256_t a, iv256_t b)
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

iv256_t iv256_sub(iv256_t a, iv256_t b)
{
	iv256_t c;

	c=iv256_neg(b);
	c=iv256_add(a, c);
	return(c);
}

