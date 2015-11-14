#include <stdio.h>

int main()
{
	int arr[16];
	int i, j, k, l;

//	for(i=0; i<16; i++)arr[i]=i&7;
	for(i=0; i<16; i++)arr[i]=i;

	for(i=0; i<256; i++)
	{
		j=rand()&15;
		k=rand()&15;
		l=arr[j]; arr[j]=arr[k]; arr[k]=l;
	}

	for(i=0; i<16; i++)printf("%d ", arr[i]);
	printf("\n");
}
