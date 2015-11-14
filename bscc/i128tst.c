#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *xltoa(__int128 val);

int main()
{
	__int128 lxi, lxj, lxk;
	lxi=1<<32;
	lxj=lxi+lxi;
	lxk=lxi*lxi;

//	lxi=1<<110;
//	lxj=1<<34;

	printf("%s %s %s\n", xltoa(lxi), xltoa(lxj), xltoa(lxk));

	lxj=1<<34;
	lxk=lxj/lxi;
	printf("%s %s %s\n", xltoa(lxi), xltoa(lxj), xltoa(lxk));
}