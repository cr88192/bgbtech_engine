#include <pdgl.h>

#include <bgbasm.h>
// #include <bgbccc.h>
//#include <bgbscr1.h>

PDGL_API int PDGL_InitASM(int argc, char **argv)
{
	static int init=0;

	if(init)return;
	init=1;

	BASM_Init();
	BASM_ProcessEXE(argv[0]);

//	ccAddInclude("cc_usr/include");
//	ccAddInclude("include");
//	ccAddInclude("");

//	BS1_TY_Init();
//	BS1_PP_AddIncludePathFront("cc_usr/include");
//	BS1_PP_AddIncludePathFront("build/include");
//	BS1_PP_AddIncludePathFront("");
}

PDGL_API int PDGL_LoadModule(char *name)
{
//	return(ccLoadModule(name));
//	return(BS1_CComp_LoadModule(name));
}

PDGL_API void *PDGL_GetPtr(char *name)
{
	return(dyllGetAddr(name));
//	return(BASM_GetPtr(name));
}

PDGL_API int PDGL_CheckLinkage()
{
#if 0
	static char *lst[1024];
	int i, j;

	j=BASM_EnumUndefSyms(lst, 1024);
	if(!j)return(0);

	printf("Undefined Symbol(s):\n");
	for(i=0; i<j; i++)
	{
		printf("\t%s\n", lst[i]);
	}
	return(-1);
#endif

	return(0);
}
