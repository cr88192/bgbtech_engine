#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include <bgbsvm.h>

#ifdef MSVC
__declspec(dllexport) void bsvm_foo()
{
}
#endif

int main(int argc, char *argv[])
{
	char buf[256];
	char *s;
	dyt a, t;
	int i, it, dt;
	float dtf;

	dy_init(&argc);

//	BSVM_TY_Init();
//	BSVM_Init();
	BSVM_ToolsInit();
	BSVM_PKG2C_Init();

	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			s=argv[i];
			
			if(!strcmp(s, "-Oc"))
			{
				BSVM_PKG2C_SetOutC(argv[i+1]);
				i++; continue;
			}

			if(!strcmp(s, "-Oh"))
			{
				BSVM_PKG2C_SetOutH(argv[i+1]);
				i++; continue;
			}

			if(!strcmp(s, "-Obs"))
			{
				BSVM_PKG2C_SetOutBS(argv[i+1]);
				i++; continue;
			}

			if(!strcmp(s, "-api"))
			{
				BSVM_PKG2C_SetAPI(argv[i+1]);
				i++; continue;
			}

			if(!strcmp(s, "-libhdr"))
			{
				BSVM_PKG2C_SetLibHdr(argv[i+1]);
				i++; continue;
			}
			
			continue;
		}
	
		BSVM_PKG2C_LoadCompileScript(argv[i]);
	}

	BSVM_PKG2C_Finish();

	BSVM_PKG2C_CloseOutC();
	BSVM_PKG2C_CloseOutH();
	BSVM_PKG2C_CloseOutBS();
	return(0);
}
