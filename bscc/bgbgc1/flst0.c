#include <stdio.h>
#include <stdlib.h>

int main()
{
	int i, j;

	j=256;
	for(i=0; i<64; i++)
	{
		printf("%d %d\n", i, j);
		j=j+(j>>2);
	}
}
