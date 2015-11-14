#include "bsde_internal.h"

static int *bsde_build_array=NULL;
static bsde_real *bsde_vector_array=NULL;
static bsde_real *bsde_scalar_array=NULL;
static int bsde_build_level=0;
static int bsde_build_array_pos=0;
static int bsde_vector_array_pos=0;
static int bsde_scalar_array_pos=0;

static int bsde_build_array_sz=0;
static int bsde_vector_array_sz=0;
static int bsde_scalar_array_sz=0;

static int bsde_build_toplevel=0;

void BSDE_BuildEmitInt(int val)
{
	int i;

	i=bsde_build_array_pos++;
	if(i>=bsde_build_array_sz)
	{
		bsde_build_array_sz+=bsde_build_array_sz>>1;
		bsde_build_array=(int *)realloc(bsde_build_array,
			bsde_build_array_sz*sizeof(int));
	}

	bsde_build_array[i]=val;
}

void BSDE_BuildEmitScalar(bsde_real val)
{
	int i;

	i=bsde_scalar_array_pos++;
	if(i>=bsde_scalar_array_sz)
	{
		bsde_scalar_array_sz+=bsde_scalar_array_sz>>1;
		bsde_scalar_array=(bsde_real *)realloc(bsde_scalar_array,
			bsde_scalar_array_sz*sizeof(bsde_real));
	}

	bsde_scalar_array[i]=val;
}

void BSDE_BuildEmitVector(bsde_real x, bsde_real y, bsde_real z, bsde_real w)
{
	int i;

	i=bsde_vector_array_pos++;
	if(i>=bsde_vector_array_sz)
	{
		bsde_vector_array_sz+=bsde_vector_array_sz>>1;
		bsde_vector_array=(bsde_real *)realloc(bsde_vector_array,
			bsde_vector_array_sz*4*sizeof(bsde_real));
	}

	bsde_vector_array[i*4+0]=x;
	bsde_vector_array[i*4+1]=y;
	bsde_vector_array[i*4+2]=z;
	bsde_vector_array[i*4+3]=w;
}

void BSDE_Begin(BSDE_World *wrl, int type)
{
	if(!bsde_build_array)
	{
		bsde_build_array_sz=1<<12;
		bsde_vector_array_sz=1<<8;
		bsde_scalar_array_sz=1<<10;

		bsde_build_array=(int *)malloc(bsde_build_array_sz*sizeof(int));
		bsde_vector_array=(bsde_real *)malloc(bsde_vector_array_sz*4*sizeof(bsde_real));
		bsde_scalar_array=(bsde_real *)malloc(bsde_scalar_array_sz*sizeof(bsde_real));
	}

	if(!bsde_build_level)
	{
		bsde_build_array_pos=0;
		bsde_vector_array_pos=0;
		bsde_scalar_array_pos=0;
		bsde_build_toplevel=type;
	}else
	{
		BSDE_BuildEmitInt(BSDE_TAG_BEGIN);
		BSDE_BuildEmitInt(type);
	}
	bsde_build_level++;
}

int BSDE_End(BSDE_World *wrl)
{
	BSDE_Solid *tmp;
	bsde_real *varr, *sarr;
	int *arr;
	int i;

	if(bsde_build_level<=0)return(-1);
	bsde_build_level--;

	if(bsde_build_level)
	{
		BSDE_BuildEmitInt(BSDE_TAG_END);
	}else
	{
		//encoded so end is still obvious to decoder
		BSDE_BuildEmitInt(BSDE_TAG_END);

		arr=bsde_build_array;
		varr=bsde_vector_array;
		sarr=bsde_scalar_array;
		tmp=BSDE_NewSolidArray(wrl, bsde_build_toplevel,
			&arr, &varr, &sarr);
		i=BSDE_AddSolidWorld(wrl, tmp);
		return(i);
	}
	return(0);
}

