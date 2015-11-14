#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

int cc_bscc_main();
int rf_bscc_main();

int main()
{
	printf("Test: Ref\n");
	__try
	{
		rf_bscc_main();
	}__except(EXCEPTION_EXECUTE_HANDLER)
	{
		printf("Test: Crashed\n");
	}

	printf("Test: CC\n");
//	__try
//	{
		cc_bscc_main();
//	}__except(EXCEPTION_EXECUTE_HANDLER)
//	{
//		printf("Test: Crashed\n");
//	}
}
