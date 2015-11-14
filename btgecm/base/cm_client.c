#include <btgecm.h>

void BGTE_ClientInit()
{
	static int init=0;
	char tb[256];
	int i;
	
	if(init)return;
	init=1;
	
	sprintf(tb, "client%s", BTGE_IdArch());
	i=dyllLoadLibrary(tb);
	if(i<0)
	{
		printf("BGTE_ClientInit: Failed to load client DLL\n");
	}
}

void btge_client_dummy()
{
}

void BTGE_ClientInitFunc(char *name, void **ptr)
{
	if(*ptr)return;
	BGTE_ClientInit();
	*ptr=dyllGetAddr(name);
//	if(!ptr)
	printf("BTGE_ClientInitFunc: %s %s %p\n",
		name, (*ptr)?"Pass":"Fail", (*ptr));
	if(!(*ptr))
		*ptr=(void *)btge_client_dummy;
//	return(ptr);
}

BTGE_API BTCL_World_Dummy *btclNewWorld(void)
{
	static BTCL_World_Dummy *(*fcn)(void)=NULL;
	BTGE_ClientInitFunc("BTCL_NewWorld", (void **)(&fcn));
	if(fcn)return(fcn());
	return(NULL);
}

BTGE_API void btclBindWorld(BTCL_World_Dummy *wrl)
{
	static void (*fcn)(BTCL_World_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_BindWorld", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

BTGE_API void btclBindBrushWorld(BTCL_BrushWorld_Dummy *wrl)
{
	static void (*fcn)(BTCL_BrushWorld_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_BindBrushWorld", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

BTGE_API void btclStepWorld(BTCL_World_Dummy *wrl, float dt)
{
	static void (*fcn)(BTCL_World_Dummy *wrl, float dt)=NULL;
	BTGE_ClientInitFunc("BTCL_StepWorld", (void **)(&fcn));
	if(fcn)fcn(wrl, dt);
}

BTGE_API void btclLoadMapFile(BTCL_World_Dummy *wrl, char *map)
{
	static void (*fcn)(BTCL_World_Dummy *wrl, char *map)=NULL;
	BTGE_ClientInitFunc("BTCL_LoadMapFile", (void **)(&fcn));
	if(fcn)fcn(wrl, map);
}

BTGE_API void btclLoadMap(BTCL_World_Dummy *wrl, char *map)
{
	static void (*fcn)(BTCL_World_Dummy *wrl, char *map)=NULL;
	BTGE_ClientInitFunc("BTCL_LoadMap", (void **)(&fcn));
	if(fcn)fcn(wrl, map);
}

BTGE_API void btclFlushWorld(BTCL_World_Dummy *wrl)
{
	static void (*fcn)(BTCL_World_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_FlushWorld", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

BTGE_API void btclDecodeUpdate(BTCL_World_Dummy *wrl, dyt msg)
{
	static void (*fcn)(BTCL_World_Dummy *wrl, dyt msg)=NULL;
	BTGE_ClientInitFunc("BTCL_DecodeUpdate", (void **)(&fcn));
	if(fcn)fcn(wrl, msg);
}

BTGE_API void btclSetupClientFrame(BTCL_World_Dummy *wrl)
{
	static void (*fcn)(BTCL_World_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_SetupClientFrame", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

BTGE_API void btclSetupClientCamera(BTCL_World_Dummy *wrl)
{
	static void (*fcn)(BTCL_World_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_SetupClientCamera", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

BTGE_API dyt btclComposeUpdate(BTCL_World_Dummy *wrl)
{
	static dyt (*fcn)(BTCL_World_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_ComposeUpdate", (void **)(&fcn));
	if(fcn)return(fcn(wrl));
	return(NULL);
}

BTGE_API void btclDraw2D(BTCL_World_Dummy *wrl)
{
	static void (*fcn)(BTCL_World_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_Draw2D", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

BTGE_API int btclHandleInput(BTCL_World_Dummy *wrl)
{
	static int (*fcn)(BTCL_World_Dummy *wrl)=NULL;
	BTGE_ClientInitFunc("BTCL_HandleInput", (void **)(&fcn));
	if(fcn)return(fcn(wrl));
	return(-1);
}
