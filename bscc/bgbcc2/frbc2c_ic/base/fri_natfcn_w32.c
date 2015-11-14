/*
Written 2007-2014 by Brendan G Bohannon
*/

#include <frbc2ci.h>

#ifdef WIN32

#include <windows.h>
#include <tlhelp32.h>
#include <dbghelp.h>

HMODULE blnk_hmodapp=NULL;

char *blnk_hmodname[256];
HMODULE blnk_hmoddll[256];
int blnk_numdll=0;

static char *blnk_os_envarr[256];
static char **blnk_os_env=NULL;

BOOL (*SymFromAddr_f) (
  __in HANDLE hProcess, __in DWORD64 Address,
  __out PDWORD64 Displacement,
  __inout PSYMBOL_INFO Symbol );

DWORD (*SymSetOptions_f) ( __in DWORD SymOptions );

BOOL (*SymInitialize_f) (
	__in HANDLE hProcess, __in PCTSTR UserSearchPath,
	__in BOOL fInvadeProcess );


int FR2CI_FFI_GetModuleListOS(char **buf, int max)
{
	int i, n;

	n=blnk_numdll;
	if(n>max)n=max;
	for(i=0; i<n; i++)
		buf[i]=blnk_hmodname[i];
	return(n);
}

char **FR2CI_FFI_GetEnvironOS()
{
	static char *ta[256];
	char *s, *buf;
	int n;

	if(blnk_os_env)
		return(blnk_os_env);

	buf=GetEnvironmentStrings();

	blnk_os_env=blnk_os_envarr;
	s=buf; n=0;
	while(*s)
	{
		blnk_os_env[n++]=fr2ci_strdup(s);
		s+=strlen(s)+1;
	}
	blnk_os_env[n]=NULL;
	
	FreeEnvironmentStrings(buf);
	return(blnk_os_env);
}

void FR2CI_FFI_ProbeTst(byte *buf, int sz)
{
	int i, j;

	for(i=0; i<(sz/4096); i++)
		j=buf[i*4096+2048];
}

void FR2CI_FFI_UpdateDllsOS(void *hmod)
{
	HMODULE hdl;
	DWORD dwPid;
	HANDLE hSnap;
	MODULEENTRY32 me32;

	hdl=(HMODULE)hmod;

	dwPid=GetCurrentProcessId();
	hSnap=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);

	if(hSnap!=INVALID_HANDLE_VALUE)
	{
		me32.dwSize=sizeof(MODULEENTRY32);
		if(Module32First(hSnap, &me32))
		{
			while(1)
			{
				printf("(MOD) %s %p %d\n",
					me32.szModule,
					me32.modBaseAddr, me32.modBaseSize);

//				FR2CI_FFI_ProbeTst(me32.modBaseAddr, me32.modBaseSize);

				if(!hdl || (me32.hModule==hdl))
				{
					FR2CI_FFI_AddExeSection(me32.szModule,
						(s64)me32.modBaseAddr,
						me32.modBaseSize, 0);
				}

				FR2CI_FFI_LoadDynamicOS(me32.szModule);
				if(!Module32Next(hSnap, &me32))
					break;
			}
		}
		CloseHandle(hSnap);
	}
}

int FR2CI_FFI_InitDllsOS()
{
	static int init=0;

	if(init)return(0);
	init=1;

	if(!blnk_hmodapp)
		blnk_hmodapp=GetModuleHandle(NULL);

	FR2CI_FFI_UpdateDllsOS(NULL);

	return(1);
}

int FR2CI_FFI_LoadDynamicOS(char *name)
{
	char buf[256];
	HMODULE hdl;
	char *s;
	int i;

	FR2CI_FFI_InitDllsOS();

	for(i=0; i<blnk_numdll; i++)
		if(!strcmp(blnk_hmodname[i], name))
			return(0);

	if(FR2CI_FFI_LookupBlacklist(name)>0)
		return(-1);

	//LoadLibrary does not like '/'...
	strcpy(buf, name);
	s=buf; while(*s) { if(*s=='/')*s='\\'; s++; }

	hdl=GetModuleHandle(buf);
	if(!hdl)hdl=LoadLibrary(buf);

	if(hdl)
	{
		printf("FR2CI_FFI_LoadDynamicOS: Pass Load lib %s\n", name);

		i=blnk_numdll++;
		blnk_hmodname[i]=fr2ci_strdup(name);
		blnk_hmoddll[i]=hdl;

		FR2CI_FFI_UpdateDllsOS(hdl);
		return(0);
	}

	printf("FR2CI_FFI_LoadDynamicOS: Fail Load lib %s\n", name);

	return(-1);
}

void *FR2CI_FFI_LoadModuleFileDataOS(char *name, int *rsz)
{
	char nbuf[1024];
	HMODULE hdl;
	HANDLE hFile;
	LARGE_INTEGER szFile;
	DWORD dwBytesRead;
	void *buf;
	char *s;
	int i, sz;

	FR2CI_FFI_InitDllsOS();

	if(FR2CI_FFI_LookupBlacklist(name)>0)
		return(-1);

	hdl=GetModuleHandle(name);
	i=GetModuleFileName(hdl, nbuf, 1024);
	if(!i)return(NULL);

	hFile=CreateFile(nbuf,		// module name
		GENERIC_READ,			// open for reading
		FILE_SHARE_READ,		// share for reading
		NULL,					// default security
		OPEN_EXISTING,			// existing file only
		FILE_ATTRIBUTE_NORMAL,	// normal file
		NULL);					// no attr. template

	if (hFile==INVALID_HANDLE_VALUE) 
		return(NULL);

	if(!GetFileSizeEx(hFile, &szFile))
	{
		CloseHandle(hFile);
		return(NULL);
	}

	sz=szFile.QuadPart;

	//fail file is overly large
	if(sz>(1<<26))
	{
		CloseHandle(hFile);
		return(NULL);
	}

	buf=malloc(sz);

	if(!ReadFile(hFile, buf, sz, &dwBytesRead, NULL))
	{
//	printf("Could not read from file (error %d)\n", GetLastError());
		CloseHandle(hFile);
		return(NULL);
	}

	CloseHandle(hFile);
	if(rsz)*rsz=sz;
	return(buf);
}

/** load file data, if it is an ExWAD image */
void *FR2CI_FFI_LoadModuleWadDataOS(char *name, int *rsz)
{
	void *p;
	byte *pb;
	int i, sz;

	p=FR2CI_FFI_LoadModuleFileDataOS(name, &sz);
	if(!p)return(NULL);
	
	pb=(byte *)p;
	for(i=0; i<(sz/16); i++)
	{
		if(!memcmp(pb+i*16, "ExWAD101", 6) &&
			!memcmp(pb+(i+1)*16, "DAWxE\r\n\xFF", 8))
		{
			if(rsz)*rsz=sz;
			return(p);
		}
	}

	//no ExWAD sig found
	free(p);
	return(NULL);
}

void FR2CI_FFI_FreeModuleDataOS(void *buf)
{
	free(buf);
}

void FR2CI_FFI_UpdateProxyOS(char *name, void *ptr)
{
	char ntmp[256];
	void *p;
	int i;

	FR2CI_FFI_InitDllsOS();

#ifdef X86_32
	sprintf(ntmp, "__iproxy_%s", name);
#else
	sprintf(ntmp, "_iproxy_%s", name);
#endif

	if(blnk_hmodapp)
	{
		p=(void *)GetProcAddress(blnk_hmodapp, TEXT(ntmp));
		if(p)*(void **)p=ptr;
	}

	for(i=0; i<blnk_numdll; i++)
	{
		p=(void *)GetProcAddress(blnk_hmoddll[i], TEXT(ntmp));
		if(p)*(void **)p=ptr;
	}
}

void *FR2CI_FFI_LookupLabelOS_1(char *name)
{
	char ntmp[64];
	void *p, *q;
	int i;

	p=NULL;

	if(blnk_hmodapp)
	{
		p=(void *)GetProcAddress(blnk_hmodapp, TEXT(name));
		if(p)return(p);
	}

	for(i=0; i<blnk_numdll; i++)
	{
		p=(void *)GetProcAddress(blnk_hmoddll[i], TEXT(name));
		if(p)break;
	}

	return(p);
}

void *FR2CI_FFI_LookupLabelOS_2(char *name, int *rfl)
{
	char ntmp[256];
	void *p;
	int i;

	*rfl=0;

	if(*name!='_')
	{
		*rfl|=1;

		for(i=9; i>0; i--)
		{
			sprintf(ntmp, "BSRT%d__%s", i, name);
			p=FR2CI_FFI_LookupLabelOS_1(ntmp);
			if(p)break;
		}

		if(p)return(p);

		sprintf(ntmp, "BSRT__%s", name);
		p=FR2CI_FFI_LookupLabelOS_1(ntmp);
		if(p)return(p);

		*rfl&=~1;
	}

	p=FR2CI_FFI_LookupLabelOS_1(name);
	return(p);
}

void *FR2CI_FFI_LookupLabelOS(char *name)
{
	void *p, *q;
	int fl;

	FR2CI_FFI_InitDllsOS();

	if(FR2CI_FFI_LookupBlacklist(name)>0)
		return(NULL);
	if((*name=='_') && (FR2CI_FFI_LookupBlacklist(name+1)>0))
		return(NULL);

//	printf("FR2CI_FFI_LookupLabelOS: module %p\n", blnk_hmodapp);

	p=FR2CI_FFI_LookupLabelOS_2(name, &fl);

#ifdef X86_64
//	if(p && (((s64)p)&0xFFFFFFFF00000000LL) )
	if(p && FR2CI_FFI_CheckPtrBigSpace(p))
	{
		FR2CI_FFI_AddProxyPtr(name, p);
		q=FR2CI_FFI_FetchSym(name);
//		printf("FR2CI_FFI_LookupLabelOS: Proxy %s->%p @%p\n", name, p, q);
		return(q);
	}
#endif

	if(p && (fl&1))
	{
		FR2CI_FFI_AddProxyPtr(name, p);
		q=FR2CI_FFI_FetchSym(name);
		return(q);
	}

	return(p);
}

int FR2CI_FFI_InitDbgHelpOS()
{
	static int init=0;
	DWORD dwPid;
	HANDLE hSnap;
	MODULEENTRY32 me32;

	if(init)return(0);
	init=1;

	FR2CI_FFI_LoadDynamicOS("dbghelp");

	SymFromAddr_f=FR2CI_FFI_LookupLabel("SymFromAddr");
	SymSetOptions_f=FR2CI_FFI_LookupLabel("SymSetOptions");
	SymInitialize_f=FR2CI_FFI_LookupLabel("SymInitialize");

	if(!SymFromAddr_f || !SymSetOptions_f || !SymInitialize_f)
	{
		printf("Failed load dbgHelp\n");
		return(-1);
	}

	SymSetOptions_f(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
	SymInitialize_f(GetCurrentProcess(), NULL, TRUE);

	return(1);
}

char *FR2CI_FFI_LookupAddrNameOS(void *addr, void **addr2)
{
	char buf[1024];
	HANDLE hProcess;
	DWORD64 offs;
	SYMBOL_INFO *pSym;
	int i;

	FR2CI_FFI_InitDbgHelpOS();

	if(!SymFromAddr_f)
		return(NULL);

	memset(buf, 0, 1024);
	pSym=(SYMBOL_INFO *)buf;
	pSym->SizeOfStruct=sizeof(SYMBOL_INFO);
	pSym->MaxNameLen=1024-sizeof(SYMBOL_INFO);

	hProcess=GetCurrentProcess();
	i=SymFromAddr_f(hProcess, (DWORD64)addr, &offs, pSym);

	if(i)
	{
		if(addr2)*addr2=(void *)(pSym->Address);
		return(fr2ci_strdup(pSym->Name));
	}

	if(addr2)*addr2=NULL;
	return(NULL);
}

int FR2CI_FFI_ProcessEXE(char *name)
{
	return(0);
}

#endif
