#include "bggl3.h"
#include <math.h>

elem bggl3_add(BGGL3_State *ctx, elem args)
{
	float tv[16];
	elem cur, a;
	float f, g;
	int i, n, m;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		for(i=0; i<16; i++)tv[i]=0;
		m=0;

		cur=args;
		while(cur)
		{
			a=BGGL3_CAR(cur);
			n=BGGL3_GetSize(a)/sizeof(float);
			for(i=0; i<n; i++)tv[i]+=((float *)a)[i];
			if(n>m)m=n;

			cur=BGGL3_CDR(cur);
		}

		return(BGGL3_MakeFVector(tv, m));
	}

	if(BGGL3_NUMBERP(BGGL3_CAR(args)))
	{
		f=0; g=0;
		cur=args;
		while(cur)
		{
			f+=BGGL3_GETREAL(BGGL3_CAR(cur));
			g+=BGGL3_GETIMAG(BGGL3_CAR(cur));
			cur=BGGL3_CDR(cur);
		}
		return(BGGL3_COMPLEX(f, g));
	}

	return(NULL);
}

elem bggl3_sub(BGGL3_State *ctx, elem args)
{
	float tv[16];
	elem cur, a;
	float f, g;
	int i, n, m;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		for(i=0; i<16; i++)tv[i]=0;

		a=BGGL3_CAR(args);
		n=BGGL3_GetSize(a)/sizeof(float);
		for(i=0; i<n; i++)tv[i]=((float *)a)[i];
		m=n;

		cur=BGGL3_CDR(args);
		while(cur)
		{
			a=BGGL3_CAR(cur);
			n=BGGL3_GetSize(a)/sizeof(float);
			for(i=0; i<n; i++)tv[i]-=((float *)a)[i];
			if(n>m)m=n;

			cur=BGGL3_CDR(cur);
		}

		return(BGGL3_MakeFVector(tv, m));
	}

	if(BGGL3_NUMBERP(BGGL3_CAR(args)))
	{
		f=BGGL3_GETREAL(BGGL3_CAR(args));
		g=BGGL3_GETIMAG(BGGL3_CAR(args));
		cur=BGGL3_CDR(args);
		while(cur)
		{
			f-=BGGL3_GETREAL(BGGL3_CAR(cur));
			g-=BGGL3_GETIMAG(BGGL3_CAR(cur));
			cur=BGGL3_CDR(cur);
		}
		return(BGGL3_COMPLEX(f, g));
	}

	return(NULL);
}

elem bggl3_mul(BGGL3_State *ctx, elem args)
{
	elem cur, a, b, c;
	float f, g, f0, g0, f1, g1;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		b=BGGL3_CADR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		if(BGGL3_FVECTORP(b))
		{
			f=0;
			for(i=0; i<n; i++)
				f+=(((float *)a)[i])*(((float *)b)[i]);
			return(BGGL3_FLOAT(f));
		}

		if(BGGL3_NUMBERP(b))
		{
			f=BGGL3_TOFLOAT(b);
			c=BGGL3_Alloc("fvector", n*sizeof(float));
			for(i=0; i<n; i++)
				((float *)c)[i]=((float *)a)[i]*f;
			return(c);
		}

		return(NULL);
	}

	if(BGGL3_NUMBERP(BGGL3_CAR(args)))
	{
		f=1; g=0;
		cur=args;
		while(cur)
		{
			f0=f;
			g0=g;

			f1=BGGL3_GETREAL(BGGL3_CAR(cur));
			g1=BGGL3_GETIMAG(BGGL3_CAR(cur));
			f=f0*f1-g0*g1;
			g=f0*g1+g0*f1;

			//(f0+g0i)(f1+g1i)
			//f0*f1+f0*g1i+g0i*f1+g0i*g1i
			//f0*f1+f0*g1i+g0i*f1-g0*g1

			cur=BGGL3_CDR(cur);
		}
		return(BGGL3_COMPLEX(f, g));
	}

	return(NULL);
}

elem bggl3_div(BGGL3_State *ctx, elem args)
{
	elem a, b, c, cur;
	float f, g, f0, g0, f1, g1, h;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		b=BGGL3_CADR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		if(BGGL3_NUMBERP(b))
		{
			f=BGGL3_TOFLOAT(b);
			c=BGGL3_Alloc("fvector", n*sizeof(float));
			for(i=0; i<n; i++)
				((float *)c)[i]=((float *)a)[i]/f;
			return(c);
		}

		return(NULL);
	}

	if(BGGL3_NUMBERP(BGGL3_CAR(args)))
	{
		f=BGGL3_GETREAL(BGGL3_CAR(args));
		g=BGGL3_GETIMAG(BGGL3_CAR(args));
		cur=BGGL3_CDR(args);
		while(cur)
		{
			f0=f;
			g0=g;

			f1=BGGL3_GETREAL(BGGL3_CAR(cur));
			g1=BGGL3_GETIMAG(BGGL3_CAR(cur));

			h=f1*f1+g1*g1;
			if(h==0)
			{
//				printf("Divide by zero\n");
//				exit(-1);
				return(NULL);
			}

			f=(f0*f1+g0*g1)/h;
			g=-(f0*g1-g0*f1)/h;

			cur=BGGL3_CDR(cur);
		}
		return(BGGL3_COMPLEX(f, g));
	}

	return(NULL);
}

elem bggl3_idiv(BGGL3_State *ctx, elem args)
{
	int i;

	if(BGGL3_NUMBERP(BGGL3_CAR(args)))
	{
		i=BGGL3_TOINT(BGGL3_CAR(args))/
			BGGL3_TOINT(BGGL3_CADR(args));
		return(BGGL3_INT(i));
	}
}

elem bggl3_mod(BGGL3_State *ctx, elem args)
{
	float f;

	if(BGGL3_NUMBERP(BGGL3_CAR(args)))
	{
		f=fmod(BGGL3_TOFLOAT(BGGL3_CAR(args)),
			BGGL3_TOFLOAT(BGGL3_CADR(args)));
		return(BGGL3_FLOAT(f));
	}
}

elem bggl3_exp(BGGL3_State *ctx, elem args)
{
	elem a, b;
	float *fa, *fb, *fc;
	float bf, bg, ef, eg, f, g, f0, g0, h, h0;
	int i, j, n;

	a=BGGL3_CAR(args);
	b=BGGL3_CADR(args);

	if(BGGL3_FVECTORP(a))
	{
		n=BGGL3_GetSize(a)/sizeof(float);

		if(!BGGL3_FVECTORP(b))return(NULL);

		fa=(float *)a;
		fb=(float *)b;

		if(n==2)
		{
			f=fa[0]*fb[1]-fa[1]*fb[0];
			return(BGGL3_FLOAT(f));
		}

		if(n==3)
		{
			fc=(float *)BGGL3_Alloc("fvector", n*sizeof(float));
			fc[0]=fa[1]*fb[2]-fa[2]*fb[1];
			fc[1]=fa[2]*fb[0]-fa[0]*fb[2];
			fc[2]=fa[0]*fb[1]-fa[1]*fb[0];
			return((elem)fc);
		}
	}

	if(BGGL3_COMPLEXP(a) || BGGL3_COMPLEXP(b))
	{
		bf=BGGL3_GETREAL(a);
		bg=BGGL3_GETIMAG(a);

		//complex^complex
		if(BGGL3_COMPLEXP(b))
		{
			ef=BGGL3_GETREAL(b);
			eg=BGGL3_GETIMAG(b);

			g=atan2(bg, bf);
			h=pow(bf*bf+bg*bg, ef/2)*pow(M_E, -eg*g);
			h0=ef*g + 0.5*ef*log(bf*bf+bg*bg);

			f0=h*cos(h0);
			g0=h*sin(h0);

			return(BGGL3_COMPLEX(f0, g0));
		}

		//complex^int
		if(BGGL3_INTP(b))
		{
			f=1; g=0;

			i=BGGL3_TOINT(b);
			j=(i<0)?(-i):i;
			while(j--)
			{
				f0=f; g0=g;
				f=f0*bf-g0*bg;
				g=f0*bg+g0*bf;
			}
			if(i<0)
			{
				h=f*f+g*g;
				f=f/h;
				g=-g/h;
			}

			return(BGGL3_COMPLEX(f, g));
		}

		//complex^real
		f=BGGL3_TOFLOAT(b);

		g=sqrt(bf*bf+bg*bg);
		h=atan2(bg, bf);

		f0=pow(g, f)*cos(f*h);
		g0=pow(g, f)*sin(f*h);

		return(BGGL3_COMPLEX(f0, g0));
	}

	if(BGGL3_NUMBERP(a))
	{
		f=BGGL3_TOFLOAT(a);
		g=BGGL3_TOFLOAT(b);

		if(f<0)
		{
			h=pow(fabs(f), g);
			f0=h*cos(g*M_PI);
			g0=h*sin(g*M_PI);

			if(fabs(f0)<(1.0/1000000000000.0))f0=0;
			if(fabs(g0)<(1.0/1000000000000.0))g0=0;

			return(BGGL3_COMPLEX(f0, g0));
		}

		return(BGGL3_FLOAT(pow(f, g)));
	}
}

elem bggl3_neg(BGGL3_State *ctx, elem args)
{
	elem a, b;
	float f, g;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		b=BGGL3_Alloc("fvector", n*sizeof(float));
		for(i=0; i<n; i++)((float *)b)[i]=-((float *)a)[i];
		return(b);
	}

	f=BGGL3_GETREAL(BGGL3_CAR(args));
	g=BGGL3_GETIMAG(BGGL3_CAR(args));
	return(BGGL3_COMPLEX(-f, -g));
}

elem bggl3_cmpl(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_INT(f<g));
}

elem bggl3_cmpg(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_INT(f>g));
}

elem bggl3_cmple(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_INT(f<=g));
}

elem bggl3_cmpge(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_INT(f>=g));
}

elem bggl3_cmpe(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_INT(f==g));
}

elem bggl3_cmpne(BGGL3_State *ctx, elem args)
{
	float f, g;

	f=BGGL3_TOFLOAT(BGGL3_CAR(args));
	g=BGGL3_TOFLOAT(BGGL3_CADR(args));
	return(BGGL3_INT(f!=g));
}

void BGGL3_InitBuiltinOpr()
{
	BGGL3_AddBuiltin("+", "add", &bggl3_add, -1);
	BGGL3_AddBuiltin("-", "sub", &bggl3_sub, -1);
	BGGL3_AddBuiltin("*", "mul", &bggl3_mul, -1);
	BGGL3_AddBuiltin("/", "div", &bggl3_div, -1);
	BGGL3_AddBuiltin("%", "mod", &bggl3_mod, 2);
	BGGL3_AddBuiltin("^", "exp", &bggl3_exp, 2);

	BGGL3_AddBuiltin("\\\\", "idiv", &bggl3_idiv, 2);

	BGGL3_AddBuiltin("neg", "negate", &bggl3_neg, 1);

	BGGL3_AddBuiltin("<", "less", &bggl3_cmpl, 2);
	BGGL3_AddBuiltin(">", "greater", &bggl3_cmpg, 2);
	BGGL3_AddBuiltin("<=", "less equal", &bggl3_cmple, 2);
	BGGL3_AddBuiltin(">=", "greater equal", &bggl3_cmpge, 2);

	BGGL3_AddBuiltin("==", "equal", &bggl3_cmpe, 2);
	BGGL3_AddBuiltin("!=", "not equal", &bggl3_cmpne, 2);
}
