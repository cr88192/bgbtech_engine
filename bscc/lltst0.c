#include <stdio.h>

#if 1
int main()
{
	long long li, lj, lk;

	li=12345; lj=67890;
	printf("%d %d\n", (int)li, (int)lj);

	printf("%d\n", (int)(lj-li));

	lk=lj-li;
	printf("%ld\n", lk);
	lk=li-lj;
	printf("%ld\n", lk);

	printf("%d\n", (int)(lj*li));
	printf("%d\n", (int)(lj/li));
	printf("%d\n", (int)(lj%li));
}
#endif
