#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
//	long long l;
	float ft;
	int l, ly, lm, ld;
	time_t t;

	t=time(NULL);

	l=t;
	ft=(l/31557600.0)+1970;
	ly=ft;
	lm=(ft-ly)*12;
	ld=(((ft-ly)*12)-lm)*30.4375;

	printf("%d %f  %d %d %d\n", l, ft, ly, lm+1, ld+1);
}
