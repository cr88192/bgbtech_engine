#include <stdio.h>
#include <math.h>
//#include <bgbasm.h>

//extern float powf(float f);
extern void *_imp__pow;

int BDISASM_PrintOpcodes(void *ip, int fl, int sz);

int bscc_main()
{
	float x;

//	BDISASM_PrintOpcodes(pow, 0, 64);
	BDISASM_PrintOpcodes(_imp__pow, 0, 256);

	x=pow(3.0, 4.0);
	printf("%f\n", x);
}

