#include <stdio.h>

int main()
{
	int i, j;

	j=5;
	for(i=0; i<48; i++)
	{
		printf("%d ", j);
		j=j+(j>>2);
	}
	printf("\n");

	j=5;
	for(i=0; i<48; i++)
	{
		printf("%d ", 8192/(j*4));
		j=j+(j>>2);
	}
	printf("\n");
}
