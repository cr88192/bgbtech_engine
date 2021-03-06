#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

//#ifdef linux
//#include <sys/utsname.h>
//#endif

#include <bgbccc.h>

char *bgbcc_loads[4096];
int bgbcc_nloads=0;

char *bgbcc_src[16];	//source paths
int bgbcc_nsrc;		//num source paths

char *bgbcc_inc[16];	//source paths
int bgbcc_ninc;		//num source paths

char *bgbcc_lib[16];	//source paths
int bgbcc_nlib;		//num source paths

int bgbcc_arch;

int bgbcc_argc;
char **bgbcc_argv;

BGBCC_API int BGBCC_BindSig(BGBCC_State *ctx, char *name, char *sig)
{
	char tb[256];

	DYLL_MetaPath_BindKey(name, "func");
	sprintf(tb, "%s:sig", name);
	DYLL_MetaPath_BindKey(tb, sig);

	return(0);
}

int BGBCC_BindTypeSig(BGBCC_State *ctx, char *name, char *sig)
{
	char tb[256];

	DYLL_MetaPath_BindKey(name, "type");
	sprintf(tb, "%s:sig", name);
	DYLL_MetaPath_BindKey(tb, sig);
	return(0);
}

int BGBCC_BindVarSig(BGBCC_State *ctx, char *name, char *sig)
{
	char tb[256];
	char *ts;

	ts="var";
	if(ctx->cur_struct)ts="field";

	DYLL_MetaPath_BindKey(name, ts);
	sprintf(tb, "%s:sig", name);
	DYLL_MetaPath_BindKey(tb, sig);
	return(0);
}

int BGBCC_BindVarInfo(BGBCC_State *ctx, char *name, char *key, char *val)
{
	char tb[256];

	sprintf(tb, "%s:%s", name, key);
	DYLL_MetaPath_BindKey(tb, val);
	return(0);
}

int BGBCC_BindTypeInfo(BGBCC_State *ctx, char *name, char *key, char *val)
	{ return(BGBCC_BindVarInfo(ctx, name, key, val)); }

int BGBCC_BindDyTypeSig(BGBCC_State *ctx, char *name, char *sig)
{
	char tb[256];

	sprintf(tb, "bgbdy/%s", name);
	DYLL_MetaPath_BindKey(tb, "dytype");
	sprintf(tb, "bgbdy/%s:sig", name);
	DYLL_MetaPath_BindKey(tb, sig);
	return(0);
}


char *BGBCP_LookupSig(BGBCP_ParseState *ctx, char *name)
{
	char tb[256];
	char *s;

	s=DYLL_MetaPath_LookupKey(name);
	if(!s)return(NULL);
	if(strcmp(s, "func"))return(NULL);

	sprintf(tb, "%s:sig", name);
	s=DYLL_MetaPath_LookupKey(tb);
	return(s);
}

char *BGBCP_LookupTypeSig(BGBCP_ParseState *ctx, char *name)
{
	char tb[256];
	char *s;

	s=DYLL_MetaPath_LookupKey(name);

	if(!s)return(NULL);
	if(strcmp(s, "type"))return(NULL);

	sprintf(tb, "%s:sig", name);
	s=DYLL_MetaPath_LookupKey(tb);
	return(s);
}

int BGBCC_BindStructSig(BGBCC_State *ctx, char *name, char *sig)
{
	char tb[256];

	DYLL_MetaPath_BindKey(name, sig);

	return(0);
}

int BGBCC_BindStructInfo(BGBCC_State *ctx, char *name, char *key, char *val)
{
	char tb[256];

	sprintf(tb, "%s:%s", name, key);
	DYLL_MetaPath_BindKey(tb, val);
	return(0);
}

int BGBCC_BindStructInfoIdx(BGBCC_State *ctx,
	char *name, char *key, int idx, char *val)
{
	char tb[256];

	sprintf(tb, "%s:%s.%d", name, key, idx);
	DYLL_MetaPath_BindKey(tb, val);
	return(0);
}

int BGBCC_BindFieldSig(BGBCC_State *ctx, char *cls, char *name, char *sig)
{
	char tb[256];

	sprintf(tb, "%s/%s", cls, name);
	DYLL_MetaPath_BindKey(tb, "field");

	sprintf(tb, "%s/%s:sig", cls, name);
	DYLL_MetaPath_BindKey(tb, sig);
	return(0);
}

int BGBCC_BindFieldInfo(BGBCC_State *ctx,
	char *cls, char *name, char *key, char *val)
{
	char tb[256];

	sprintf(tb, "%s/%s:%s", cls, name, key);
	DYLL_MetaPath_BindKey(tb, val);
	return(0);
}

int BGBCC_BindStructInfoI(BGBCC_State *ctx,
	char *name, char *key, int val)
{
	char tb[64];

	sprintf(tb, "%d", val);
	BGBCC_BindStructInfo(ctx, name, key, tb);
	return(0);
}

int BGBCC_BindFieldInfoI(BGBCC_State *ctx,
	char *cls, char *name, char *key, int val)
{
	char tb[64];

	sprintf(tb, "%d", val);
	BGBCC_BindFieldInfo(ctx, cls, name, key, tb);
	return(0);
}

char *BGBCP_LookupPPKey(BGBCP_ParseState *ctx, char *name)
{
	char tb[256];
	char *s;

	sprintf(tb, "CPreProc/%s", name);
//	s=DYLL_MetaPath_LookupKey(tb);
	s=dyllMetaLookupKey(tb);
	return(s);
}

int BGBCC_BindPPKey(BGBCP_ParseState *ctx, char *name, char *sig)
{
	char tb[256];

	sprintf(tb, "CPreProc/%s", name);

//	DYLL_MetaPath_BindKey(tb, sig);
	dyllMetaBindKey(tb, sig);
	return(0);
}

int BGBCC_StoreTextFile(char *name, char *buf)
{
	FILE *fd;

	fd=fopen(name, "wt");
	if(fd)
	{
		fwrite(buf, 1, strlen(buf), fd);
		fclose(fd);
	}

	return(0);
}

int BGBCC_StoreFile(char *name, void *buf, int sz)
{
	FILE *fd;

	fd=fopen(name, "wb");
	if(!fd)return(-1);
	
	if(fd)
	{
		fwrite(buf, 1, sz, fd);
		fclose(fd);
	}

	return(0);
}

int bgbcc_storefile(char *name, void *buf, int sz)
	{ return(BGBCC_StoreFile(name, buf, sz)); }

char *bgbcc_loadfile(char *name, int *rsz)
{
	FILE *fd;
	void *buf;
	int sz;

//	printf("bgbcc_loadfile: %s\n", name);

	fd=fopen(name, "rb");
	if(!fd)return(NULL);

	fseek(fd, 0, 2);
	sz=ftell(fd);
	fseek(fd, 0, 0);

//	buf=bgbcc_malloc(sz+16);
	buf=bgbcc_malloc2(sz+16);
	memset(buf, 0, sz+16);
	fread(buf, 1, sz, fd);

	fclose(fd);

	if(rsz)*rsz=sz;
	return(buf);
}

int BGBCC_LoadCMeta(char *name)
{
	char tb[256], tb1[256];
	FILE *fd;
	BCCX_Node *t;
	byte *obj;
	char *s, *buf, *buf1, *buf2, *mod;
	int i, sz, t0, t1, dt;

	buf=bgbcc_loadfile(name, &sz);

	if(!buf)
	{
		for(i=0; i<bgbcc_nsrc; i++)
		{
			sprintf(tb, "%s/%s", bgbcc_src[i], name);
			buf=bgbcc_loadfile(tb, &sz);
			if(buf)break;
		}
	}

	if(!buf)
	{
		buf=BGBPP_LoadInclude(NULL, name, &sz);
	}

	if(!buf)
	{
		printf("BGBCC_LoadCMeta: fail load '%s'\n", name);
		return(-1);
	}

	printf("BGBCC_LoadCMeta: %s %d bytes\n", name, sz);

	strcpy(tb, name);
	s=tb;
	while(*s)
	{
		if(*s=='/')*s='_';
		if(*s=='\\')*s='_';
		if(*s==':')*s='_';
		s++;
	}

	while((s>tb) && (*s!='.'))s--;
	if(s>tb)*s=0;

	mod=bgbcc_strdup(tb);


	t0=clock();
	t=BGBCP_ModuleBuffer(name, mod, buf);

	if(!t)return(-1);

#if 1
	if(mod)
	{
		sprintf(tb1, "dump/%s_ast.txt", mod);
		fd=fopen(tb1, "wt");
		if(fd)
		{
			BCCX_PrintFD(fd, t);
			fclose(fd);
		}

#if 0
		buf1=malloc(1<<24);
		i=SBXE_WriteNodeList(buf1, t);
		printf("SBXE Encoded %d bytes\n", i);

		sprintf(tb1, "dump/%s_ast.sbxe", mod);
		fd=fopen(tb1, "wb");
		if(fd && (i>0))
		{
			fwrite(buf1, 1, i, fd);
			fclose(fd);
		}
		free(buf1);
#endif
	}
#endif

	t1=clock();
	dt=t1-t0;
	printf("BGBCC_LoadModule: Parse took %dms\n",
		(int)(1000*(dt/(float)CLOCKS_PER_SEC)));

//	printf("BGBCC_LoadModule: Parse OK\n");

	buf1=BGBCC_CompileModule(mod, t);

	BGBCC_GAstCompileModule(mod, t, &i);

	if(mod && buf1)
	{
		sprintf(tb1, "dump/%s_ril.txt", mod);
		BGBCC_StoreTextFile(tb1, buf1);
	}

	buf1=BGBCC_BS2IL_CompileModule(mod, t);

	if(mod && buf1)
	{
		sprintf(tb1, "dump/%s_bs2il.txt", mod);
		BGBCC_StoreTextFile(tb1, buf1);
	}

//	DYLL_MetaPath_BindKey(name, "header");
//	dyllMetaBindKey(name, "header");

//	BGBGC_SaveSig();
	dyllMetaCommit();

//	BGBCC_CleanupAll();

//	BGBCC_UpdateCheckFile(name);

	bgbcc_loads[bgbcc_nloads++]=strdup(name);

//	free(buf1);
	return(0);
}

int BGBCC_GetArch()
	{ return(bgbcc_arch); }

int BGBCC_InitEnv(int argc, char **argv, char **env)
{
	char *ta[16];
	char buf[256];
	static int init=0;
	int inc_ok;
	int i, m;
	char *s, *t;
	char *mach_name, *gcc_ver;
//#ifdef linux
//	struct utsname utsbuf;
//#endif

	if(init)return(-1);
	init=1;

	mach_name=NULL; gcc_ver=NULL;
#ifdef __GNUC__
	sprintf(buf, "%d.%d.%d",
		__GNUC__,  __GNUC_MINOR__,  __GNUC_PATCHLEVEL__);
	gcc_ver=strdup(buf);
#endif

#ifdef linux
//	uname(&utsbuf);
//	mach_name=utsbuf.machine;
#define MKSTR(arg) #arg
	mach_name=MACHTYPE;
	printf("MachName=%s\n", mach_name);
#endif

	m=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='/')
		{
			if(!strncmp(argv[i], "/FZx", 4))
			{
				m|=16;
				continue;
			}
			if(!strncmp(argv[i], "/FZy", 4))
			{
				m|=32;
				continue;
			}

			continue;
		}

		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "--"))
				{ i++; break; }
			if(!strcmp(argv[i], "--pc"))
				m|=1;

			if(!strncmp(argv[i], "-I", 2))
			{
				ccAddInclude(argv[i]+2);
				continue;
			}
			if(!strncmp(argv[i], "-L", 2))
			{
				ccAddLibrary(argv[i]+2);
				continue;
			}
			if(!strncmp(argv[i], "-S", 2))
			{
				ccAddSource(argv[i]+2);
				continue;
			}

			if(!strncmp(argv[i], "-D", 2))
			{
				ccAddDefineString(argv[i]+2);
				continue;
			}

			continue;
		}
	}

	inc_ok=0;
	for(i=0; env[i]; i++)
	{
//		printf("ENV %s\n", env[i]);
#if 1
		if(!(m&32) && !strnicmp(env[i], "Include=", strlen("Include=")))
//		if(0)
		{
			s=env[i]+strlen("Include=");
			while(*s)
			{
				while(*s==' ')s++;

				t=buf;
				while(*s && (*s!=';'))
					*t++=*s++;
				*t++=0;

				while(*s==';')s++;

				if(buf[0])
				{
					ccAddInclude(buf);
					inc_ok=1;
				}
			}
		}
#endif

#if 0
		if(!strnicmp(env[i], "MACHTYPE=", strlen("MACHTYPE=")))
		{
			s=env[i]+strlen("MACHTYPE=");
			mach_name=s;
		}
#endif
	}

#ifdef linux
	if(gcc_ver && mach_name)
	{
		sprintf(buf, "/usr/lib/gcc/%s/%s/include", mach_name, gcc_ver);
		ccAddInclude(buf);
	}

	if(mach_name)
	{
		sprintf(buf, "/usr/%s/include", mach_name);
		ccAddInclude(buf);
	}

	ccAddInclude("/usr/include");
	ccAddInclude("/usr/local/include");
	inc_ok=1;
#endif

	if(!inc_ok)
	{
		BGBPP_AddIncludePathFront("cc_usr/include");
		BGBPP_AddIncludePathFront("cc_usr/bscc/include");
//		BGBPP_AddIncludePathFront("build/include");
//		BGBPP_AddIncludePathFront("include");
//		BGBPP_AddIncludePathFront("");
	}

#ifdef _WIN32
	BGBPP_AddStaticDefine(NULL, "_WIN32", "");
//	BGBPP_AddStaticDefine(NULL, "HWND", "void *");
#endif

#ifdef linux
	BGBPP_AddStaticDefine(NULL, "linux", "");
#endif

#ifdef __i386__
	BGBPP_AddStaticDefine(NULL, "__i386__", "");
	bgbcc_arch=BGBCC_ARCH_X86;
#endif
#ifdef __x86_64__
	BGBPP_AddStaticDefine(NULL, "__x86_64__", "");
	bgbcc_arch=BGBCC_ARCH_X64;
#endif

#ifdef _M_IX86
//	BGBPP_AddStaticDefine(NULL, "_X86_", "");
	BGBPP_AddStaticDefine(NULL, "_M_IX86", "600");
	bgbcc_arch=BGBCC_ARCH_X86;
#endif
#ifdef _M_X64
	BGBPP_AddStaticDefine(NULL, "_M_X64", "");
	bgbcc_arch=BGBCC_ARCH_X64;
#endif

	BGBPP_AddStaticDefine(NULL, "_BGBMETA", "");

	BGBPP_AddStaticDefine(NULL, "__inline__", "");

	BGBPP_AddStaticDefine(NULL, "__GNUC__", "1");

//	ta[0]="v"; ta[1]=NULL;
//	BGBPP_AddStaticDefineArgs(NULL, "__declspec", ta, "");

//	ta[0]="v"; ta[1]=NULL;
//	BGBPP_AddStaticDefineArgs(NULL, "DECLARE_HANDLE", ta, "typedef void *v");

	BGBPP_AddStaticDefine(NULL, "POINTER_32", "");
	BGBPP_AddStaticDefine(NULL, "POINTER_64", "");

	BGBPP_AddStaticDefine(NULL, "WINGDIAPI", "__declspec(dllimport)");
	BGBPP_AddStaticDefine(NULL, "CALLBACK", "");

#ifdef _M_IX86
	BGBPP_AddStaticDefine(NULL, "APIENTRY", "__stdcall");
#else
	BGBPP_AddStaticDefine(NULL, "APIENTRY", "");
#endif

#if 1
#ifdef _MSC_VER
	sprintf(buf, "%d", _MSC_VER);
	BGBPP_AddStaticDefine(NULL, "_MSC_VER", buf);
#endif

#ifdef _MSC_FULL_VER
	sprintf(buf, "%d", _MSC_FULL_VER);
	BGBPP_AddStaticDefine(NULL, "_MSC_FULL_VER", buf);
#endif
#endif

#ifdef _INTEGRAL_MAX_BITS
	sprintf(buf, "%d", _INTEGRAL_MAX_BITS);
	BGBPP_AddStaticDefine(NULL, "_INTEGRAL_MAX_BITS", buf);
#endif

//	BGBPP_AddStaticDefine(NULL, "__midl", "0");

#if 0
	BGBPP_AddStaticDefine(NULL, "DBG", "0");
	BGBPP_AddStaticDefine(NULL, "_DBG_MEMCPY_INLINE_", "0");
	BGBPP_AddStaticDefine(NULL, "_WIN32_WCE", "0x0600");
	BGBPP_AddStaticDefine(NULL, "WINXP", "0x06000000");
	BGBPP_AddStaticDefine(NULL, "NTDDI_WXP", "0x06000000");
	BGBPP_AddStaticDefine(NULL, "NTDDI_XPSP2", "NTDDI_WINXPSP2");
	BGBPP_AddStaticDefine(NULL, "NTDDI_WIN2003", "NTDDI_WS03");
	
	BGBPP_AddStaticDefine(NULL, "NTDDI_WINLH", "NTDDI_VERSION");
	BGBPP_AddStaticDefine(NULL, "NTDDK_VERSION", "NTDDI_VERSION");
#endif

	BGBPP_AddStaticDefine(NULL, "__STDC__", "1");
	BGBPP_AddStaticDefine(NULL, "__STDC_HOSTED__", "1");
	BGBPP_AddStaticDefine(NULL, "__STDC_VERSION__", "199901");

	return(0);
}

int BGBCC_Init()
	{ return(BGBCC_InitEnv(0, NULL, NULL)); }

void ccAddInclude(char *path)
{
	int i;

	printf("Add include '%s'\n", path);

	BGBCC_Init();
	i=bgbcc_nlib++;
	bgbcc_lib[i]=bgbcc_strdup(path);
	BGBPP_AddIncludePathFront(path);
}

void ccAddLibrary(char *path)
{
	int i;

	BGBCC_Init();
	i=bgbcc_nlib++;
	bgbcc_lib[i]=bgbcc_strdup(path);
}

void ccAddSource(char *path)
{
	int i;

	BGBCC_Init();
	i=bgbcc_nsrc++;
	bgbcc_src[i]=bgbcc_strdup(path);
}

void ccAddDefineString(char *str)
{
	char tb[256], tb2[256];
	char *s, *t;
	
	s=str; t=tb;
	while(*s && (*s!='='))
		*t++=*s++;
	*t++=0;

	if(*s=='=')s++;

	t=tb2;
	while(*s)
		*t++=*s++;
	*t++=0;

	BGBPP_AddStaticDefine(NULL, tb, tb2);
}

int main(int argc, char *argv[], char **env)
{
	char tb[256];
	char *uds[256], *args[256], *adds[256];
	ExWAD_Context *wad;
	char *metafn;
	char *wadfn;
	void *p;
	char *s, *t, *s0;
	int n, m, nuds, nargs, nadds;
	int t0, dt, te, sz;
	int i, j;

//	dy_init(&argc);

	BGBCC_InitEnv(argc, argv, env);

	metafn=NULL;
	wadfn=NULL;

	nargs=0;
	args[nargs++]=argv[0];

	nadds=0;
	nuds=0;

	n=0; m=0; nadds=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='/')
		{
			if(!strncmp(argv[i], "/I", 2))
			{
				ccAddInclude(argv[i]+2);
				continue;
			}
			if(!strncmp(argv[i], "/L", 2))
			{
				ccAddLibrary(argv[i]+2);
				continue;
			}
			if(!strncmp(argv[i], "/S", 2))
			{
				ccAddSource(argv[i]+2);
				continue;
			}

			if(!strncmp(argv[i], "/D", 2))
			{
				ccAddDefineString(argv[i]+2);
				continue;
			}

			if(!strncmp(argv[i], "/Fn", 3))
			{
				metafn=argv[i]+3;
				continue;
			}

			if(!strncmp(argv[i], "/Fw", 3))
			{
				wadfn=argv[i]+3;
				continue;
			}

			if(!strcmp(argv[i], "/add"))
			{
				for(j=0; j<(argc-i-1); j++)
				{
					if(argv[i+j+1][0]=='/')break;
					if(argv[i+j+1][0]=='-')break;
					adds[nadds++]=argv[i+j+1];
				}
				i+=j;
				continue;
			}

			if(!strncmp(argv[i], "/FZx", 4))
			{
				m|=16;
				continue;
			}

			continue;
		}

		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "--"))
				{ i++; break; }
			if(!strcmp(argv[i], "--pc"))
				m|=1;
			continue;
		}
		uds[nuds++]=argv[i];
	}

	nargs=0; args[nargs++]=argv[0];
	for(; i<argc; i++)args[nargs++]=argv[i];

	for(i=0; i<nuds; i++)
		BGBCC_LoadCMeta(uds[i]);

	if(m&16)
	{
		t0=clock();
		te=t0+30*CLOCKS_PER_SEC;
		while(t0<te)
		{
			for(i=0; i<nuds; i++)
				BGBCC_LoadCMeta(uds[i]);
			t0=clock();

			BGBCC_CleanupAll();
		}
	}

//	for(i=0; i<nuds; i++)
//		ccLoadModule(uds[i]);

//	for(i=1; i<argc; i++)
//		ccLoadModule(argv[i]);

	wad=NULL;
	if(wadfn)
	{
		wad=ExWAD_LoadImage2(wadfn);
	}

	if(wad)
	{
		for(i=0; i<nadds; i++)
			ExWAD_AddFile(wad, adds[i]);
			
		if(nuds)
		{
#if 0
			p=DYLL_MetaBin_SaveBinaryBuf("metabin.dat", &sz);
			if(p)
			{
				ExWAD_StoreFile(wad, "metabin.dat", p, sz);
			}
#endif

#if 1
			s0=malloc(1<<24);
			i=DYLL_MetaPath_SaveDBBuffer(s0, 1<<24);
			if(i>0)
			{
				ExWAD_StoreFile(wad, "meta.txt", (byte *)s0, i);
			}
			free(s0);
#endif
		}
		
		ExWAD_SaveImage(wad);
	}

	if(metafn)
		DYLL_MetaPath_SaveDB(metafn);
}

