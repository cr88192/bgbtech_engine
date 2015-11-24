#include <btgecm.h>

BTGE_API char *BTGE_IdArch()
{
#ifdef __i386__
	return("x86");
#endif
#ifdef __x86_64__
	return("x64");
#endif
#ifdef _M_IX86
	return("x86");
#endif
#ifdef _M_X64
	return("x64");
#endif
#ifdef _M_PPC
	return("ppc");
#endif
	return("unk");
}

static char *btge_username=NULL;
static s64 btge_userid=0;

static char base64chars[64]=
"0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"_$";

int BGTE_Base64CharVal(int ch)
{
	int i;
	
	for(i=0; i<64; i++)
		if(base64chars[i]==ch)
			return(i);
	return(-1);
}

int BGTE_InitGlobalFlags()
{
	VFILE *fd;
	char *keybuf;
	char *buf1, *cs, *ct, *cs1, *s0;
	char **a;
	int i, j, sz;

	btge_username=NULL;
	btge_userid=0;

	fd=vffopen("userkey.txt", "rb");
	if(!fd)return(-1);
	keybuf=vf_bufferin_sz(fd, &sz);
	vfclose(fd);
	
	buf1=gcatomic(((sz+3)/4)*3+1);
	cs=keybuf; ct=buf1;
	while(*cs)
	{
		i=0;
		while(*cs && (*cs<=' '))cs++;
		if(*cs)i=BGTE_Base64CharVal(*cs++);
		while(*cs && (*cs<=' '))cs++;
		i=i<<6;
		if(*cs)i=i+BGTE_Base64CharVal(*cs++);
		while(*cs && (*cs<=' '))cs++;
		i=i<<6;
		if(*cs)i=i+BGTE_Base64CharVal(*cs++);
		while(*cs && (*cs<=' '))cs++;
		i=i<<6;
		if(*cs)i=i+BGTE_Base64CharVal(*cs++);
		
		*ct++=(i>>16)&255;
		*ct++=(i>>8)&255;
		*ct++=i&255;
	}
	*ct++=0;
	
	cs=buf1;
	if(*cs!='A')return(-1);
	cs++;
	i=0;
	while((*cs>='0') && (*cs<='9'))
		i=(i*10)+((*cs++)-'0');
	cs1=cs; j=0;
	while(*cs1)
		{ j=(j*4093)+(*cs1++); }
	if(i!=j)return(-1);
	
	while(*cs)
	{
		s0=gcrgets(&cs);
		a=gcrsplit(s0);
		if(!a[0])continue;
		if(!strcmp(a[0], "username"))
			btge_username=dystrdup(a[1]);
		if(!strcmp(a[0], "userid"))
			btge_userid=gcratoi(a[1]);
	}
	return(0);
}

BTGE_API int BGTE_GlobalFlags()
{
	if(btge_username && btge_userid)
		return(0x0B);
	return(0);
}

void *btge_server_mutex=NULL;

void btge_server_lock()
{
	thLockFastMutex(btge_server_mutex);
}

void btge_server_unlock()
{
	thUnlockFastMutex(btge_server_mutex);
}

void BGTE_ServerInit()
{
	static int init=0;
	char tb[256];
	int i;
	
	if(init)return;
	init=1;

	btge_server_mutex=thFastMutex();

	btge_username=NULL;
	btge_userid=0;

	BGTE_InitGlobalFlags();
	
	sprintf(tb, "server%s", BTGE_IdArch());
	i=dyllLoadLibrary(tb);
	if(i<0)
	{
		printf("BGTE_ServerInit: Failed to load server DLL\n");
	}
}

void *btge_server_dummy()
{
	return(NULL);
}

void BTGE_ServerInitFunc(char *name, void **ptr)
{
	if(*ptr)return;
	BGTE_ServerInit();
	*ptr=dyllGetAddr(name);
//	if(!(*ptr))printf("BTGE_ServerInitFunc: Fail %s\n", name);
	printf("BTGE_ServerInitFunc: %s %s %p\n",
		name, (*ptr)?"Pass":"Fail", (*ptr));
	if(!(*ptr))
		*ptr=(void *)btge_server_dummy;
//	return(ptr);
}

BTGE_API void btSpawnWorld(void)
{
	static void (*fcn)(void)=NULL;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("btsvSpawnWorld");
	BTGE_ServerInitFunc("btsvSpawnWorld", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn();
		btge_server_unlock();
	}
}

BTGE_API bool btStepWorld(float dt)
{
	static bool (*fcn)(float dt)=NULL;
	static bool b;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("btsvStepWorld");
	BTGE_ServerInitFunc("btsvStepWorld", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		b=fcn(dt);
		btge_server_unlock();
		return(b);
	}
	return(false);
}

BTGE_API int btBindWorld(BTGE_BrushWorld *wrl)
{
	static int (*fcn)(BTGE_BrushWorld *wrl)=NULL;
	int i;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("btsvBindWorld");
	BTGE_ServerInitFunc("btsvBindWorld", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		i=fcn(wrl);
		btge_server_unlock();
		return(i);
	}
	return(-1);
}

BTGE_API int btWorldTeardown(BTGE_BrushWorld *world)
{
	static int (*fcn)(BTGE_BrushWorld *wrl)=NULL;
	int i;
	
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("BTGE_WorldTeardown");
	BTGE_ServerInitFunc("BTGE_WorldTeardown", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		i=fcn(world);
		btge_server_unlock();
		return(i);
	}
	return(-1);
}

BTGE_API dyt btComposeUpdate(int clnum)
{
	static dyt (*fcn)(int clnum)=NULL;
	dyt p;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("btsvComposeUpdate");
	BTGE_ServerInitFunc("btsvComposeUpdate", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		p=fcn(clnum);
		btge_server_unlock();
		return(p);
	}
	return(NULL);
}

BTGE_API void btFinishUpdate(void)
{
	static void (*fcn)(void)=NULL;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("btsvFinishUpdate");
	BTGE_ServerInitFunc("btsvFinishUpdate", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn();
		btge_server_unlock();
	}
}

BTGE_API void btFlushUpdate(void)
{
	static void (*fcn)(void)=NULL;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("btsvFlushUpdate");
	BTGE_ServerInitFunc("btsvFlushUpdate", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn();
		btge_server_unlock();
	}
}

BTGE_API void btFreeUpdate(dyt msg)
{
	static void (*fcn)(dyt msg)=NULL;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("btsvFreeUpdate");
	BTGE_ServerInitFunc("btsvFreeUpdate", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn(msg);
		btge_server_unlock();
	}
}

BTGE_API void btDecodeClientUpdate(dyt msg, float dt, int clnum)
{
	static void (*fcn)(dyt msg, float dt, int clnum)=NULL;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("BTGE_DecodeClientUpdate");
	BTGE_ServerInitFunc("BTGE_DecodeClientUpdate", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn(msg, dt, clnum);
		btge_server_unlock();
	}
}

BTGE_API void btWorldSaveGame(BTGE_BrushWorld *world, char *name)
{
	static void (*fcn)(BTGE_BrushWorld *wrl, char *name)=NULL;
//	BGTE_ServerInit();
//	fcn=dyllGetAddr("BTGE_SaveGame_SaveWorld");
	BTGE_ServerInitFunc("BTGE_SaveGame_SaveWorld", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn(world, name);
		btge_server_unlock();
	}
}

BTGE_API void btsvLoadMapFile(BTGE_BrushWorld *wrl, char *map)
{
	static void (*fcn)(BTGE_BrushWorld *wrl, char *map)=NULL;
	BTGE_ServerInitFunc("BTSV_LoadMapFile", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn(wrl, map);
		btge_server_unlock();
	}
}

BTGE_API void btsvLoadMap(BTGE_BrushWorld *wrl, char *map)
{
	static void (*fcn)(BTGE_BrushWorld *wrl, char *map)=NULL;
	BTGE_ServerInitFunc("BTSV_LoadMap", (void **)(&fcn));
	if(fcn)
	{
		btge_server_lock();
		fcn(wrl, map);
		btge_server_unlock();
	}
}
