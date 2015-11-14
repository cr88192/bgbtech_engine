#include "bggl3.h"
#include <math.h>

elem bggl3_len(BGGL3_State *ctx, elem args)
{
	elem a;
	float f;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		f=0;
		for(i=0; i<n; i++)f+=(((float *)a)[i])*(((float *)a)[i]);
		return(BGGL3_FLOAT(sqrt(f)));
	}

	return(NULL);
}

elem bggl3_norm(BGGL3_State *ctx, elem args)
{
	elem a, b;
	float f;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		f=0;
		for(i=0; i<n; i++)f+=(((float *)a)[i])*(((float *)a)[i]);
		f=sqrt(f);
		if(f==0)f=1;

		b=BGGL3_Alloc("fvector", n*sizeof(float));
		for(i=0; i<n; i++)((float *)b)[i]=(((float *)a)[i])/f;

		return(b);
	}

	return(NULL);
}

elem bggl3_ndot(BGGL3_State *ctx, elem args)
{
	elem a, b;
	float f;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		b=BGGL3_CADR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		f=0;
		for(i=0; i<n; i++)f+=(((float *)a)[i])*(((float *)b)[i]);
		f-=(((float *)b)[i]);

		return(BGGL3_FLOAT(f));
	}

	return(NULL);
}

elem bggl3_npoint(BGGL3_State *ctx, elem args)
{
	elem a, b, c;
	float f;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		b=BGGL3_CADR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		f=0;
		for(i=0; i<n; i++)f+=(((float *)a)[i])*(((float *)b)[i]);
		f-=(((float *)b)[i]);

		c=BGGL3_Alloc("fvector", n*sizeof(float));
		for(i=0; i<n; i++)((float *)c)[i]=
			(((float *)a)[i])-f*(((float *)b)[i]);

		return(c);
	}

	return(NULL);
}

elem bggl3_ndir(BGGL3_State *ctx, elem args)
{
	elem a, b;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		b=BGGL3_Alloc("fvector", (n-1)*sizeof(float));
		for(i=0; i<(n-1); i++)((float *)b)[i]=(((float *)a)[i]);

		return(b);
	}

	return(NULL);
}

elem bggl3_ndist(BGGL3_State *ctx, elem args)
{
	elem a, b;
	int i, n;

	if(BGGL3_FVECTORP(BGGL3_CAR(args)))
	{
		a=BGGL3_CAR(args);
		n=BGGL3_GetSize(a)/sizeof(float);

		return(BGGL3_FLOAT(((float *)a)[n-1]));
	}

	return(NULL);
}

void BGGL3_InitBuiltinVector()
{
	BGGL3_AddBuiltin("len", "vector length", &bggl3_len, 1);
	BGGL3_AddBuiltin("norm", "normalize vector", &bggl3_norm, 1);
	BGGL3_AddBuiltin("ndot", "dist above plane", &bggl3_ndot, 2);
	BGGL3_AddBuiltin("npoint", "point on plane", &bggl3_npoint, 2);
	BGGL3_AddBuiltin("ndir", "vector part of normal", &bggl3_ndir, 1);
	BGGL3_AddBuiltin("ndist", "normal distance", &bggl3_ndist, 1);
}
