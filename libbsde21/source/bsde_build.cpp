#include "bsde_internal.h"

// static int *bsde_build_array=NULL;
// static bsde_real *bsde_vector_array=NULL;
// static bsde_real *bsde_scalar_array=NULL;
// static int bsde_build_level=0;
// static int bsde_build_array_pos=0;
// static int bsde_vector_array_pos=0;
// static int bsde_scalar_array_pos=0;
// static int bsde_build_array_sz=0;
// static int bsde_vector_array_sz=0;
// static int bsde_scalar_array_sz=0;

// static int bsde_build_toplevel=0;

void BSDE_BuildEmitInt(int val)
{
	BSDE_ThreadState *ctx;
	int i;

	ctx=BSDE_GetThreadState();

	i=ctx->build_array_pos++;
	if(i>=ctx->build_array_sz)
	{
		ctx->build_array_sz+=ctx->build_array_sz>>1;
		ctx->build_array=(int *)realloc(ctx->build_array,
			ctx->build_array_sz*sizeof(int));
	}

	ctx->build_array[i]=val;
}

void BSDE_BuildEmitScalar(bsde_real val)
{
	BSDE_ThreadState *ctx;
	int i;

	ctx=BSDE_GetThreadState();

	i=ctx->scalar_array_pos++;
	if(i>=ctx->scalar_array_sz)
	{
		ctx->scalar_array_sz+=ctx->scalar_array_sz>>1;
		ctx->scalar_array=(bsde_real *)realloc(ctx->scalar_array,
			ctx->scalar_array_sz*sizeof(bsde_real));
	}

	ctx->scalar_array[i]=val;
}

void BSDE_BuildEmitVector(bsde_real x, bsde_real y, bsde_real z, bsde_real w)
{
	BSDE_ThreadState *ctx;
	int i;

	ctx=BSDE_GetThreadState();

	i=ctx->vector_array_pos++;
	if(i>=ctx->vector_array_sz)
	{
		ctx->vector_array_sz+=ctx->vector_array_sz>>1;
		ctx->vector_array=(bsde_real *)realloc(ctx->vector_array,
			ctx->vector_array_sz*4*sizeof(bsde_real));
	}

	ctx->vector_array[i*4+0]=x;
	ctx->vector_array[i*4+1]=y;
	ctx->vector_array[i*4+2]=z;
	ctx->vector_array[i*4+3]=w;
}

void BSDE_Begin(BSDE_World *wrl, int type)
{
	BSDE_ThreadState *ctx;

	ctx=BSDE_GetThreadState();

	if(!ctx->build_array)
	{
		ctx->build_array_sz=1<<12;
		ctx->vector_array_sz=1<<8;
		ctx->scalar_array_sz=1<<10;

		ctx->build_array=(int *)malloc(ctx->build_array_sz*sizeof(int));
		ctx->vector_array=(bsde_real *)malloc(ctx->vector_array_sz*4*sizeof(bsde_real));
		ctx->scalar_array=(bsde_real *)malloc(ctx->scalar_array_sz*sizeof(bsde_real));
	}

	if(!ctx->build_level)
	{
		ctx->build_array_pos=0;
		ctx->vector_array_pos=0;
		ctx->scalar_array_pos=0;
		ctx->build_toplevel=type;
	}else
	{
		BSDE_BuildEmitInt(BSDE_TAG_BEGIN);
		BSDE_BuildEmitInt(type);
	}
	ctx->build_level++;
}

int BSDE_End(BSDE_World *wrl)
{
	BSDE_ThreadState *ctx;
	BSDE_Solid *tmp;
	bsde_real *varr, *sarr;
	int *arr;
	int i;

	ctx=BSDE_GetThreadState();

	if(ctx->build_level<=0)return(-1);
	ctx->build_level--;

	if(ctx->build_level)
	{
		BSDE_BuildEmitInt(BSDE_TAG_END);
	}else
	{
		//encoded so end is still obvious to decoder
		BSDE_BuildEmitInt(BSDE_TAG_END);

		arr=ctx->build_array;
		varr=ctx->vector_array;
		sarr=ctx->scalar_array;
		tmp=BSDE_NewSolidArray(wrl, ctx->build_toplevel,
			&arr, &varr, &sarr);
		i=BSDE_AddSolidWorld(wrl, tmp);
		return(i);
	}
	return(0);
}

