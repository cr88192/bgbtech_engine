
/*

Sparse Array

Will have an indirect chaining structure.
The first 16 values will be represented directly.
The next spot will point to a block of 16 values pointing to blocks
of 16 values;
The next will point to a block to 16 values pointing to blocks as above;
...

Unused parts of the array will simply contain holes.


So, the base will have:
16 direct spots;
8 indirect spots;
a length.

*/

#include <bgbgc.h>
#include <bgbdy.h>


struct bgbgc_sparse_s {
dyt val[16];
dyt idx[8];
int len;
};

dyt dyNewSparse()
{
	struct bgbgc_sparse_s *tmp;

	tmp=gctalloc("_sparse_t", sizeof(struct bgbgc_sparse_s));
	return((dyt)tmp);
}

int dySparseP(dyt obj)
{
	return(dytypep(obj, "_sparse_t"));
}

dyt BGBGC_SparseGetIdx(dyt arr, int idx)
{
	struct bgbgc_sparse_s *tmp;
	dyt *pa;

	tmp=(struct bgbgc_sparse_s *)arr;
	if(idx>=tmp->len)return(NULL);
	if(idx<0)return(NULL);

	if(idx<16)return(tmp->val[idx]);

	if(idx<256)
	{
		pa=(dyt *)tmp->idx[1];		if(!pa)return(NULL);
		pa=(dyt *)pa[idx>>4];		if(!pa)return(NULL);
		return(pa[idx&15]);
	}

	if(idx<4096)
	{
		pa=(dyt *)tmp->idx[2];		if(!pa)return(NULL);
		pa=(dyt *)pa[idx>>8];		if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>4)&15];	if(!pa)return(NULL);
		return(pa[idx&15]);
	}

	if(idx<65536)
	{
		pa=(dyt *)tmp->idx[3];		if(!pa)return(NULL);
		pa=(dyt *)pa[idx>>12];		if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>8)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>4)&15];	if(!pa)return(NULL);
		return(pa[idx&15]);
	}

	if(idx<1048576)
	{
		pa=(dyt *)tmp->idx[4];		if(!pa)return(NULL);
		pa=(dyt *)pa[idx>>16];		if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>12)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>> 8)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>> 4)&15];	if(!pa)return(NULL);
		return(pa[idx&15]);
	}

	if(idx<16777216)
	{
		pa=(dyt *)tmp->idx[5];		if(!pa)return(NULL);
		pa=(dyt *)pa[idx>>20];		if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>16)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>12)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>> 8)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>> 4)&15];	if(!pa)return(NULL);
		return(pa[idx&15]);
	}

	if(idx<268435456)
	{
		pa=(dyt *)tmp->idx[6];		if(!pa)return(NULL);
		pa=(dyt *)pa[idx>>24];		if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>20)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>16)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>>12)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>> 8)&15];	if(!pa)return(NULL);
		pa=(dyt *)pa[(idx>> 4)&15];	if(!pa)return(NULL);
		return(pa[idx&15]);
	}

	pa=(dyt *)tmp->idx[7];		if(!pa)return(NULL);
	pa=(dyt *)pa[idx>>28];		if(!pa)return(NULL);
	pa=(dyt *)pa[(idx>>24)&15];	if(!pa)return(NULL);
	pa=(dyt *)pa[(idx>>20)&15];	if(!pa)return(NULL);
	pa=(dyt *)pa[(idx>>16)&15];	if(!pa)return(NULL);
	pa=(dyt *)pa[(idx>>12)&15];	if(!pa)return(NULL);
	pa=(dyt *)pa[(idx>> 8)&15];	if(!pa)return(NULL);
	pa=(dyt *)pa[(idx>> 4)&15];	if(!pa)return(NULL);
	return(pa[idx&15]);
}

dyt *BGBGC_SparseGetPtrIdx(dyt *pp, int idx)
{
	dyt *pa;

	pa=(dyt *)(*pp);
	if(!pa)
	{
		pa=gctalloc("_sparse_seg_t", 16*sizeof(dyt));
		*pp=(dyt)pa; pa[0]=(dyt)pp;
	}
	return(&(pa[idx&15]));
}

int BGBGC_SparseSetIdx(dyt arr, int idx, dyt val)
{
	struct bgbgc_sparse_s *tmp;
	dyt *pa;

	tmp=(struct bgbgc_sparse_s *)arr;
	if(idx<0)return(-1);
	if(idx>=tmp->len)tmp->len=idx+1;

	if(idx<16)
	{
		tmp->val[idx]=val;
		return(0);
	}

	if(idx<256)
	{
		pa=BGBGC_SparseGetPtrIdx(&tmp->idx[1], idx>>4);
		pa=BGBGC_SparseGetPtrIdx(pa, idx);
		*pa=val; return(0);
	}

	if(idx<4096)
	{
		pa=BGBGC_SparseGetPtrIdx(&tmp->idx[2], idx>>8);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>4);
		pa=BGBGC_SparseGetPtrIdx(pa, idx);
		*pa=val; return(0);
	}

	if(idx<65536)
	{
		pa=BGBGC_SparseGetPtrIdx(&tmp->idx[3], idx>>12);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>8);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>4);
		pa=BGBGC_SparseGetPtrIdx(pa, idx);
		*pa=val; return(0);
	}

	if(idx<1048576)
	{
		pa=BGBGC_SparseGetPtrIdx(&tmp->idx[4], idx>>16);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>12);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>8);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>4);
		pa=BGBGC_SparseGetPtrIdx(pa, idx);
		*pa=val; return(0);
	}

	if(idx<16777216)
	{
		pa=BGBGC_SparseGetPtrIdx(&tmp->idx[5], idx>>20);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>16);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>12);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>8);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>4);
		pa=BGBGC_SparseGetPtrIdx(pa, idx);
		*pa=val; return(0);
	}

	if(idx<268435456)
	{
		pa=BGBGC_SparseGetPtrIdx(&tmp->idx[6], idx>>24);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>20);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>16);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>12);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>8);
		pa=BGBGC_SparseGetPtrIdx(pa, idx>>4);
		pa=BGBGC_SparseGetPtrIdx(pa, idx);
		*pa=val; return(0);
	}

	pa=BGBGC_SparseGetPtrIdx(&tmp->idx[7], idx>>28);
	pa=BGBGC_SparseGetPtrIdx(pa, idx>>24);
	pa=BGBGC_SparseGetPtrIdx(pa, idx>>20);
	pa=BGBGC_SparseGetPtrIdx(pa, idx>>16);
	pa=BGBGC_SparseGetPtrIdx(pa, idx>>12);
	pa=BGBGC_SparseGetPtrIdx(pa, idx>>8);
	pa=BGBGC_SparseGetPtrIdx(pa, idx>>4);
	pa=BGBGC_SparseGetPtrIdx(pa, idx);
	*pa=val; return(0);
}

