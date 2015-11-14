#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#ifndef PF
#define PF rf_
#endif

#ifndef _BGBMETA
#define FN(a) rf_##a
#else
#define FN_I(a, b) a ## b
#define FN(a) FN_I(PF, a)
#endif

void FN(hsortm)(int *array, int nmemb)
{
    int p, child, parent;
    int temp;

    if (nmemb>1)
    {
        p=(--nmemb)/2;
        do {
            parent=p;
            child=parent * 2;
            temp=array[parent];
            while ((nmemb-parent)>parent)
            {
                if (array[child+1]>array[child]) child++;
                if (array[child]>temp)
                {
                    array[parent]=array[child];
                    parent=child;
                    child*=2;
                }else break;
            }
            if ((nmemb==child) && (array[child]>temp))
            {
                array[parent]=array[child];
                parent=child;
            }
            array[parent]=temp;
        } while (p--);

        temp=array[0];
        array[0]=array[nmemb];
        array[nmemb]=temp;

        while (--nmemb)
        {
            parent=0;
            child=parent*2;
            temp=array[parent];
            while ((nmemb-parent)>parent)
            {
                if (array[child+1]>array[child]) child++;

                if (array[child]>temp)
                {
                    array[parent]=array[child];
                    parent=child;
                    child*=2;
                } else break;
            }
            if ((nmemb==child) && (array[child]>temp))
            {
                array[parent]=array[child];
                parent=child;
            }
            array[parent]=temp;

            temp=array[0];
            array[0]=array[nmemb];
            array[nmemb]=temp;
        }
    }
}

void FN(Swap)(int *arr, int i, int j)
{
    int k;
    if(i==j)return;
    k=arr[i]; arr[i]=arr[j]; arr[j]=k;
}

//#define Swap(arr,i,j) { k=arr[i]; arr[i]=arr[j]; arr[j]=k; }while(0)

void FN(Sort0)(int *arr, int base, int lim)
{
    static int depth=0;
    int mp;
    int low, mid, high;
    int i, j, k;

	if((lim-base)<2)return;

	if(depth>=64) //bad case, use selection sort
	{
		for(i=base; i<lim; i++)
			for(j=i+1; j<lim; j++)
				if(arr[j]<arr[i])FN(Swap)(arr, i, j);
		return;
	}

	depth++;

    low=base; high=lim; mid=(low+high)/2;
//    mp=arr[mid];
    i=arr[low]; j=arr[mid]; k=arr[high-1];
    mp=(i<j)?((k<i)?i:j):((i<k)?i:j);

    i=low;
    while(i<high)
    {
        if(arr[i]<mp) { FN(Swap)(arr, low++, i++); continue; }
        if(arr[i]>mp) { FN(Swap)(arr, --high, i); continue; }
        i++;
    }

    FN(Sort0)(arr, base, low);
    FN(Sort0)(arr, high, lim);
    depth--;
}

void FN(Sort1)(int *arr, int base, int lim)
{
    static int depth=0;
    int mp;
    int low, mid, high;
    int i, j, k;

    if((lim-base)<2)return;

   if(depth>=64) //bad case, use selection sort
    {
        j=1; while(j)
        {
            j=0; lim--;
            for(i=base; i<lim; i++)
                if(arr[i+1]<arr[i]) { FN(Swap)(arr, i, i+1); j++; }
            for(i=lim; i>base; i--)
                if(arr[i]<arr[i-1]) { FN(Swap)(arr, i, i-1); j++; }
            base++;
        }
        return;
    }
   depth++;

    low=base; high=lim; mid=(low+high)/2;
//    mp=arr[mid];
    i=arr[low]; j=arr[mid]; k=arr[high-1];
    mp=(i<j)?((k<i)?i:j):((i<k)?i:j);

    i=low;
    while(i<high)
    {
        if(arr[i]<mp) { FN(Swap)(arr, low++, i++); continue; }
        if(arr[i]>mp) { FN(Swap)(arr, --high, i); continue; }
        i++;
    }

    FN(Sort1)(arr, base, low);
    FN(Sort1)(arr, high, lim);
    depth--;
}

void FN(Sort2)(int *arr, int base, int lim)
{
    static int depth=0;
    int mp;
    int low, mid, high;
    int i, j, k;

	if((lim-base)<2)return;

#if 1	//heapsort fallback, aka: crude intro sort
	if(depth>=64)
	{
		FN(hsortm)(arr+base, lim-base);
		return;
	}
#endif

	depth++;

    low=base; high=lim; mid=(low+high)/2;
//    mp=arr[mid];
    i=arr[low]; j=arr[mid]; k=arr[high-1];
    mp=(i<j)?((k<i)?i:j):((i<k)?i:j);

    i=low;
    while(i<high)
    {
        if(arr[i]<mp) { FN(Swap)(arr, low++, i++); continue; }
        if(arr[i]>mp) { FN(Swap)(arr, --high, i); continue; }
        i++;
    }

    FN(Sort2)(arr, base, low);
    FN(Sort2)(arr, high, lim);
    depth--;
}

void FN(sortchk)(int *arr, int sz)
{
	int i, j;

	j=sz-1;
	for(i=0; i<j; i++)
		if(arr[i]>arr[i+1])
	{
		printf("sort fail\n");
		return;
	}
}

int FN(qs_cmp_i)(const void *a, const void *b)
{
	if((*(int *)a) < (*(int *)b))return(-1);
	if((*(int *)a) > (*(int *)b))return(1);
	return(0);
}

int FN(bscc_main)()
{
	int *arr;
	int i, j, t, sz;

	sz=1<<22;

	arr=malloc(sz*sizeof(int));

	for(i=0; i<sz; i++)arr[i]=rand()*251+rand();

	t=clock();
	FN(hsortm)(arr, sz);
	printf("HSort %d\n", clock()-t);
	FN(sortchk)(arr, sz);


	for(i=0; i<sz; i++)arr[i]=rand()*251+rand();

	t=clock();
	FN(Sort0)(arr, 0, sz);
	printf("Sort0 %d\n", clock()-t);
	FN(sortchk)(arr, sz);


	for(i=0; i<sz; i++)arr[i]=rand()*251+rand();

	t=clock();
	FN(Sort1)(arr, 0, sz);
	printf("Sort1 %d\n", clock()-t);
	FN(sortchk)(arr, sz);

	for(i=0; i<sz; i++)arr[i]=rand()*251+rand();

	t=clock();
	FN(Sort2)(arr, 0, sz);
	printf("Sort2 %d\n", clock()-t);
	FN(sortchk)(arr, sz);


	for(i=0; i<sz; i++)arr[i]=rand()*251+rand();
	t=clock();
	qsort(arr, sz, 4, FN(qs_cmp_i));
	printf("QSort %d\n", clock()-t);
	FN(sortchk)(arr, sz);

	return(0);
}

