#include <btgecm.h>

void BGTE_Tile2DInit()
{
	static int init=0;
	char tb[256];
	int i;
	
	if(init)return;
	init=1;
	
	sprintf(tb, "tile2d%s", BTGE_IdArch());
	i=dyllLoadLibrary(tb);
	if(i<0)
	{
		printf("BGTE_Tile2DInit: Failed to load client DLL\n");
	}
}

void btge_tile2d_dummy()
{
}

void BTGE_Tile2DInitFunc(char *name, void **ptr)
{
	if(*ptr)return;
	BGTE_Tile2DInit();
	*ptr=dyllGetAddr(name);
//	if(!ptr)
	printf("BTGE_Tile2DInitFunc: %s %s %p\n",
		name, (*ptr)?"Pass":"Fail", (*ptr));
	if(!(*ptr))
		*ptr=(void *)btge_tile2d_dummy;
//	return(ptr);
}

BTGE_API void bt2dUpdateWorld(float dt)
{
	static void (*fcn)(float dt)=NULL;
	BTGE_Tile2DInitFunc("Tile2D_UpdateWorld", (void **)(&fcn));
	if(fcn)fcn(dt);
}

BTGE_API void bt2dDrawWorld(void)
{
	static void (*fcn)(void)=NULL;
	BTGE_Tile2DInitFunc("Tile2D_DrawWorld", (void **)(&fcn));
	if(fcn)fcn();
}

BTGE_API int bt2dHandleInput(void)
{
	static int (*fcn)()=NULL;
	BTGE_Tile2DInitFunc("Tile2D_HandleInput", (void **)(&fcn));
	if(fcn)return(fcn());
	return(-1);
}

#if 0

// BTGE_API BT2D_World_Dummy *bt2dNewWorld(void)
{
	static BT2D_World_Dummy *(*fcn)(void)=NULL;
	BTGE_Tile2DInitFunc("BT2D_NewWorld", (void **)(&fcn));
	if(fcn)return(fcn());
	return(NULL);
}

// BTGE_API void bt2dBindWorld(BT2D_World_Dummy *wrl)
{
	static void (*fcn)(BT2D_World_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_BindWorld", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

// BTGE_API void bt2dBindBrushWorld(BT2D_BrushWorld_Dummy *wrl)
{
	static void (*fcn)(BT2D_BrushWorld_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_BindBrushWorld", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

// BTGE_API void bt2dStepWorld(BT2D_World_Dummy *wrl, float dt)
{
	static void (*fcn)(BT2D_World_Dummy *wrl, float dt)=NULL;
	BTGE_Tile2DInitFunc("BT2D_StepWorld", (void **)(&fcn));
	if(fcn)fcn(wrl, dt);
}

// BTGE_API void bt2dLoadMapFile(BT2D_World_Dummy *wrl, char *map)
{
	static void (*fcn)(BT2D_World_Dummy *wrl, char *map)=NULL;
	BTGE_Tile2DInitFunc("BT2D_LoadMapFile", (void **)(&fcn));
	if(fcn)fcn(wrl, map);
}

// BTGE_API void bt2dLoadMap(BT2D_World_Dummy *wrl, char *map)
{
	static void (*fcn)(BT2D_World_Dummy *wrl, char *map)=NULL;
	BTGE_Tile2DInitFunc("BT2D_LoadMap", (void **)(&fcn));
	if(fcn)fcn(wrl, map);
}

// BTGE_API void bt2dFlushWorld(BT2D_World_Dummy *wrl)
{
	static void (*fcn)(BT2D_World_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_FlushWorld", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

// BTGE_API void bt2dDecodeUpdate(BT2D_World_Dummy *wrl, dyt msg)
{
	static void (*fcn)(BT2D_World_Dummy *wrl, dyt msg)=NULL;
	BTGE_Tile2DInitFunc("BT2D_DecodeUpdate", (void **)(&fcn));
	if(fcn)fcn(wrl, msg);
}

// BTGE_API void bt2dSetupTile2DFrame(BT2D_World_Dummy *wrl)
{
	static void (*fcn)(BT2D_World_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_SetupTile2DFrame", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

// BTGE_API void bt2dSetupTile2DCamera(BT2D_World_Dummy *wrl)
{
	static void (*fcn)(BT2D_World_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_SetupTile2DCamera", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

// BTGE_API dyt bt2dComposeUpdate(BT2D_World_Dummy *wrl)
{
	static dyt (*fcn)(BT2D_World_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_ComposeUpdate", (void **)(&fcn));
	if(fcn)return(fcn(wrl));
	return(NULL);
}

// BTGE_API void bt2dDraw2D(BT2D_World_Dummy *wrl)
{
	static void (*fcn)(BT2D_World_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_Draw2D", (void **)(&fcn));
	if(fcn)fcn(wrl);
}

// BTGE_API int bt2dHandleInput(BT2D_World_Dummy *wrl)
{
	static int (*fcn)(BT2D_World_Dummy *wrl)=NULL;
	BTGE_Tile2DInitFunc("BT2D_HandleInput", (void **)(&fcn));
	if(fcn)return(fcn(wrl));
	return(-1);
}

#endif
