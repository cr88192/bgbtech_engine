#define DYLL_HASASM

#include <bgbgc.h>
#include <bgbdy.h>

#ifdef DYLL_HASASM
#include <bgbasm.h>
#include <bgblink.h>
#endif

static char *dyll_blacklist=
"system;fopen;fclose;fread;fwrite;fgets;fgetc;"
"open;close;ioctl;fcntl;exec;"
"unlink;DeleteFile;chdir;rmdir;"
"dlopen;dlsym;LoadLibrary;LoadLibraryEx;GetProcAddress;"
"FreeLibrary;GetModuleHandle;GetModuleHandleEx;"
"FreeLibraryAndExitThread;SetDllDirectory;"
"DisableThreadLibraryCalls;LoadModule;"
"BASM_AddNameBlacklist;BASM_AddNamesBlacklist;"
"malloc;free;mmap;VirtualAlloc;VirtualFree;"
"CreateFileMapping;CreateFileMappingEx;"
"CreateFile;CreateDirectory;CopyFile;CopyFileEx;"
"CreateSymbolicLink;CreateHardLink;"

#ifdef WIN32
"AreFileApisANSI;CheckNameLegalDOS8Dot3;"
"CloseHandle;CopyFile;CopyFileEx;CopyFileTransacted;"
"CreateFile;CreateFileTransacted;CreateHardLink;CreateHardLinkTransacted;"
"CreateSymbolicLink;CreateSymbolicLinkTransacted;"
"DeleteFile;DeleteFileTransacted;FindClose;FindFirstFile;"
"FindFirstFileEx;FindFirstFileNameTransactedW;"
"FindFirstFileNameW;FindFirstFileTransacted;"
"FindFirstStreamTransactedW;FindFirstStreamW;"
"FindNextFile;FindNextStreamW;GetBinaryType;GetCompressedFileSize;"
"GetCompressedFileSizeTransacted;GetFileAttributes;GetFileAttributesEx;"
"GetFileAttributesTransacted;GetFileBandwidthReservation;"
"GetFileInformationByHandle;GetFileInformationByHandleEx;"
"GetFileSize;GetFileSizeEx;GetFileType;GetFinalPathNameByHandle;"
"GetFullPathName;GetFullPathNameTransacted;"
"GetLongPathName;GetLongPathNameTransacted;GetShortPathName;GetTempFileName;"
"GetTempPath;MoveFile;MoveFileEx;MoveFileTransacted;"
"MoveFileWithProgress;OpenFile;OpenFileById;"
"ReOpenFile;ReplaceFile;RtlIsNameLegalDOS8Dot3;"
"SearchPath;SetFileApisToANSI;SetFileApisToOEM;"
"SetFileAttributes;SetFileAttributesTransacted;"
"SetFileBandwidthReservation;SetFileInformationByHandle;"
"SetFileShortName;SetFileValidData;CancelIo;CancelIoEx;CancelSynchronousIo;"
"CreateIoCompletionPort;FlushFileBuffers;GetQueuedCompletionStatus;"
"GetQueuedCompletionStatusEx;LockFile;LockFileEx;PostQueuedCompletionStatus;"
"ReadFile;ReadFileEx;ReadFileScatter;SetEndOfFile;"
"SetFileCompletionNotificationModes;"
"SetFileIoOverlappedRange;SetFilePointer;SetFilePointerEx;"
"UnlockFile;UnlockFileEx;WriteFile;WriteFileEx;WriteFileGather;"
"AddUsersToEncryptedFile;CloseEncryptedFileRaw;"
"DecryptFile;DuplicateEncryptionInfoFile;EncryptFile;EncryptionDisable;"
"FileEncryptionStatus;FreeEncryptionCertificateHashList;"
"OpenEncryptedFileRaw;QueryRecoveryAgentsOnEncryptedFile;"
"QueryUsersOnEncryptedFile;ReadEncryptedFileRaw;"
"RemoveUsersFromEncryptedFile;SetUserFileEncryptionKey;"
"WriteEncryptedFileRaw;"
"Wow64DisableWow64FsRedirection;Wow64EnableWow64FsRedirection;"
"Wow64RevertWow64FsRedirection;"
"GetExpandedName;LZClose;LZCopy;LZInit;LZOpenFile;LZRead;LZSeek;"
"CopyProgressRoutine;ExportCallback;FileIOCompletionRoutine;ImportCallback;"
#endif
;

//library loading
static char *dyll_loads[4096];		//loaded libraries (full name)
static int dyll_nloads=0;

static char *dyll_lib[256];	//source paths
static int dyll_nlib;		//num source paths

//note: shared with metapath
char *dyll_lmods[4096];		//loaded modules (trimmed name)
int dyll_nlmods=0;

void *dyll_mutex=NULL;

void dyll_lock(void)
{
	thLockFastMutex(dyll_mutex);
}

void dyll_unlock(void)
{
	thUnlockFastMutex(dyll_mutex);
}

static s64 dyll_ftell_f(void *fd)
	{ return(vftell((VFILE *)fd)); }
static int dyll_fseek_f(void *fd, s64 pos, int rel)
	{ vfseek((VFILE *)fd, pos, rel); return(0); }

BGBDY_API int BGBGC_InitDyLL()
{
	static int init=0;
	char *ta[256];
	char tb[256];
	char tb2[256];
#ifdef DYLL_HASASM
	BLNK_GCFuncs_t *gcvt;
	BLNK_IOFuncs_t *iovt;
	BGBGC_ImportFuncs_t *givt;
#endif
	byte *modbuf;
	char *s, *s1, *t;
	int i, j, k, sz, exw;

	if(init)return(0);
	init=1;

	dy_init(NULL);	//just in case

	dyll_mutex=thFastMutex();

#ifdef DYLL_HASASM
	BASM_Init();

	gcvt=BLNK_GetGCFuncs();
	gcvt->scanrange_f=BGBGC_ScanRange2;

	gcvt->gcalloc_f=gcalloc;
	gcvt->gcexec_f=gcexec;
	gcvt->gcatomic_f=gcatomic;
	gcvt->gctalloc_f=gctalloc;
	gcvt->gctexec_f=gctexec;
	gcvt->gctatomic_f=gctatomic;

	gcvt->gcfree_f=gcfree;
	gcvt->gcrealloc_f=gcrealloc;

//	BASM_SetScanRange(BGBGC_ScanRange);
//	BASM_SetTAlloc(gctalloc);
//	BASM_SetTExec(gctexec);
//	BASM_SetTAtomic(gctatomic);

	iovt=BLNK_GetIOFuncs();

	iovt->fopen_fp=(void *(*)(char*,char*))&vffopen;
	iovt->fclose_fp=(void (*)(void*))&vfclose;
	iovt->fread_fp=(int (*)(void*,int,int,void*))&vfread;
	iovt->fwrite_fp=(int (*)(void*,int,int,void*))&vfwrite;
	iovt->feof_fp=(int (*)(void*))&vfeof;
	iovt->ftell_fp=(s64 (*)(void*))&dyll_ftell_f;
	iovt->fseek_fp=(int (*)(void*,s64,int))&dyll_fseek_f;

	BLNK_SetScanRange(&BGBGC_ScanRange);
	BGBGC_AddMarkHandler(&BLNK_MarkData);

	givt=BGBGC_GetImportFuncs();
	givt->GetAddr=dyllGetAddr;
	givt->GetAddrName=dyllGetAddrName;
	givt->GetAddrLastName=BASM_GetLastNamePtr;
//	givt->LookupSectionAddrName=BASM_LookupExeSectionPtrName;
	givt->LookupSectionAddrName=BASM_LookupSectionPtrName;

#if 0
//	BASM_AddNameBlacklist("system");
//	BASM_AddNameBlacklist("fopen");
//	BASM_AddNameBlacklist("fclose");
	BASM_AddNamesBlacklist(
		"system;fopen;fclose;fread;fwrite;fgets;fgetc;"
		"open;close;ioctl;fcntl;OpenFile;CloseFile;CloseHandle;"
		"unlink;DeleteFile;chdir;rmdir;"
		"dlopen;dlsym;LoadLibrary;LoadLibraryEx;GetProcAddress;"
		"FreeLibrary;GetModuleHandle;GetModuleHandleEx;"
		"FreeLibraryAndExitThread;SetDllDirectory;"
		"DisableThreadLibraryCalls;LoadModule;"
		"BASM_AddNameBlacklist;BASM_AddNamesBlacklist;"
		"malloc;free;mmap;VirtualAlloc;VirtualFree;"
		"CreateFileMapping;CreateFileMappingEx;"
		"CreateFile;CreateDirectory;CopyFile;CopyFileEx;"
		"CreateSymbolicLink;CreateHardLink;"
		);
#endif

	BASM_AddNamesBlacklist(dyll_blacklist);

//#ifdef WIN32
//	i=GetModuleFileNameA(NULL, tb, 256);
//	if(i>0)BASM_ProcessEXE(tb);
//#endif

// #ifdef _WIN32
#if 1
	k=BLNK_GetModuleListOS(ta, 256);
	for(i=0; i<k; i++)
	{
		s=ta[i]; s1=s;
		while(*s1)
		{
			if(*s1=='/')s=s1+1;
			s1++;
		}

		strcpy(tb, s);

//		strcpy(tb, ta[i]);
//		s=tb+strlen(tb);
//		while((s>tb) && (*s!='.'))s--;

		s=tb;
		while(*s && (*s!='.'))s++;
		*s=0;

		dyll_lmods[dyll_nlmods++]=
			basm_strdup(tb);

		sprintf(tb2, "exwad/%s", tb);
		exw=vf_mount(ta[i], tb2, "exwad", NULL);

#if 0
		modbuf=BLNK_LoadModuleFileDataOS(ta[i], &sz);
//		modbuf=BLNK_LoadModuleWadDataOS(ta[i], &sz);
		if(modbuf)
		{
			dyPrintf("Got Module Data %p %d\n", modbuf, sz);
			BLNK_FreeModuleDataOS(modbuf);
		}
#endif

//		if(*s=='.')
//		if(!(*s) || (*s=='.'))
		if(1)
		{
//			strcpy(s, "_meta.txt");
//			j=dyllMetaLoadDB(tb);

//			if((j<0) && !strncmp(tb, "lib", 3))
//				j=dyllMetaLoadDB(tb+3);		
//			dyPrintf("Load DB '%s': %s\n",
//				tb, (j<0)?"fail":"pass");

			sprintf(tb2, "%s%s", tb, "_meta.txt");
			j=dyllMetaLoadDB2(tb2, tb);

			if((j<0) && !strncmp(tb2, "lib", 3))
				j=dyllMetaLoadDB2(tb2+3, tb);

			if((j<0) && (exw>=0))
			{
//				sprintf(tb2, "exwad/%s/%s%s", tb, tb, "_meta.txt");
				sprintf(tb2, "exwad/%s/%s", tb, "meta.txt");
				j=dyllMetaLoadDB2(tb2, tb);
			}

			dyPrintf("Load DB '%s': %s\n",
				tb2, (j<0)?"fail":"pass");
		}

		dyll_loads[dyll_nloads++]=
			basm_strdup(ta[i]);
	}
#endif

#endif

	BGBGC_InitDyLL_Thunk();

	BGBGC_InitDyLL_Func();
	BGBGC_InitDyLL_Typebox();

	return(1);
}

BGBDY_API void dyllAddLibraryPath(char *name)
	{ dyll_lib[dyll_nlib++]=basm_strdup(name); }

BGBDY_API void dyllAddLibraryPathFront(char *name)
{
	int i;
	for(i=dyll_nlib++; i>0; i--)
		dyll_lib[i]=dyll_lib[i-1];
	dyll_lib[0]=basm_strdup(name);
}

#ifdef DYLL_HASASM
BGBDY_API char *dyllString(char *sym)
	{ return(basm_strdup(sym)); }
BGBDY_API char *dyllStrdup(char *sym)
	{ return(basm_strdup(sym)); }

BGBDY_API void *dyllGetAddr(char *sym)
{
	void *p;

	BGBGC_InitDyLL();

//	BGBCC_Init();
//	thLockMutex(dyll_mutex);
	dyll_lock();
	p=BASM_GetPtr(sym);
	dyll_unlock();
//	thUnlockMutex(dyll_mutex);
	return(p);
}

BGBDY_API void *dyllGetProcAddress(dyt lib, char *sym)
	{ return(dyllGetAddr(sym)); }

BGBDY_API char *dyllGetAddrName(void *ptr)
{
	char *s;

	BGBGC_InitDyLL();
	dyll_lock();
	s=BASM_GetPtrName(ptr);
	dyll_unlock();
	return(s);
}

BGBDY_API void dyllSetAddr(char *sym, void *ptr)
{
	char buf[256];
	void *p;

	BGBGC_InitDyLL();

//	BGBCC_Init();
//	thLockMutex(dyll_mutex);
	dyll_lock();
	BLNK_AssignSym(sym, ptr);
	dyll_unlock();
//	thUnlockMutex(dyll_mutex);
}

BGBDY_API void *dyllCreateVar(char *sym, char *sig)
{
	void *p;
	int sz;

	BGBGC_InitDyLL();
	sz=dyllSigQuickSize(sig);
	dyll_lock();
	p=BASM_CreateLabelBSS(sym, sz);
	dyll_unlock();
	return(p);
}

BGBDY_API void *dyllGetPtr(char *sym)
{
	void *p; p=dyllGetAddr(sym);
	if(p)return(*(void **)p);
	return(NULL);
}

BGBDY_API void dyllSetPtr(char *sym, void *v)
{
	void *p; p=dyllGetAddr(sym);
	if(p)*(void **)p=v;
}

BGBDY_API int dyllGetInt(char *sym)
{
	void *p; p=dyllGetAddr(sym);
	if(p)return(*(int *)p);
	return(0);
}

BGBDY_API void dyllSetInt(char *sym, int v)
{
	void *p; p=dyllGetAddr(sym);
	if(p)*(int *)p=v;
}

BGBDY_API s64 dyllGetLong(char *sym)
{
	void *p; p=dyllGetAddr(sym);
	if(p)return(*(s64 *)p);
	return(0);
}

BGBDY_API void dyllSetLong(char *sym, s64 v)
{
	void *p; p=dyllGetAddr(sym);
	if(p)*(s64 *)p=v;
}

BGBDY_API float dyllGetFloat(char *sym)
{
	void *p; p=dyllGetAddr(sym);
	if(p)return(*(float *)p);
	return(0);
}

BGBDY_API void dyllSetFloat(char *sym, float v)
{
	void *p; p=dyllGetAddr(sym);
	if(p)*(float *)p=v;
}

BGBDY_API double dyllGetDouble(char *sym)
{
	void *p; p=dyllGetAddr(sym);
	if(p)return(*(double *)p);
	return(0);
}

BGBDY_API void dyllSetDouble(char *sym, double v)
{
	void *p; p=dyllGetAddr(sym);
	if(p)*(double *)p=v;
}

BGBDY_API dyt dyllGetDynamic(char *sym)
{
	void *p;
	char *sig;
	dyt v;

	BGBGC_InitDyLL();

	v=DYLL_CTopGetHash(sym);
	if(v)return(v);

	v=(dyt)dycGetClass(sym);
	if(v && (v!=UNDEFINED))
		return(v);

//	sig=BGBGC_LookupSig(sym);
	sig=dyllGetNameSig(sym);
	if(!sig)return(UNDEFINED);

	p=dyllGetAddr(sym);
	if(!p)return(UNDEFINED);

	v=dyllGetVarSig(p, sig);
	if(*sig=='(')
		DYLL_CTopSetHash(sym, v);
	return(v);
}

BGBDY_API void dyllSetDynamic(char *sym, dyt val)
{
	void *p;
	char *sig;

	BGBGC_InitDyLL();

//	sig=BGBGC_LookupSig(sym);
	sig=dyllGetNameSig(sym);
	if(!sig)return;

	p=dyllGetAddr(sym);
	if(!p)return;

	dyllSetVarSig(p, sig, val);
	return;
}


BGBDY_API dyt dyllNewEnv(dyt super)
{
	BGBGC_InitDyLL();
	return((dyt)BLNK_NewScope((BLNK_Scope *)super));
}

BGBDY_API void dyllEnvBindAddr(dyt env, char *name, void *addr)
	{ BLNK_BindScopeSym((BLNK_Scope *)env, name, addr); }
BGBDY_API void dyllEnvBindAddrSig(dyt env, char *name, char *sig, void *addr)
	{ BLNK_BindScopeSymSig((BLNK_Scope *)env, name, sig, addr); }

BGBDY_API void dyllEnvSetAddr(dyt env, char *name, void *addr)
	{ BLNK_AssignScopeSym((BLNK_Scope *)env, name, addr); }
BGBDY_API void *dyllEnvGetAddr(dyt env, char *name)
	{ return(BLNK_FetchScopeSym((BLNK_Scope *)env, name)); }
BGBDY_API char *dyllEnvGetSig(dyt env, char *name)
	{ return((char *)dysymbol(BLNK_FetchScopeSig((BLNK_Scope *)env, name))); }

BGBDY_API void *dyllEnvAllocVar(dyt env, char *name, char *sig)
{
	void *ptr;
	int i;

	BGBGC_InitDyLL();

	i=dyllSigQuickSize(sig);
	ptr=gcalloc(i);
	BLNK_BindScopeSymSig((BLNK_Scope *)env, name, sig, ptr);
	return(ptr);
}

BGBDY_API dyt dyllThunkEnv(dyt fcn)
{
	BLNK_Thunk *obj;
	obj=(BLNK_Thunk *)fcn;
	return((dyt)(obj->scope));
}

BGBDY_API void dyllThunkBindAddr(dyt fcn, char *name, void *addr)
	{ dyllEnvBindAddr(dyllThunkEnv(fcn), name, addr); }
BGBDY_API void dyllThunkBindAddrSig(dyt fcn, char *name, char *sig, void *addr)
	{ dyllEnvBindAddrSig(dyllThunkEnv(fcn), name, sig, addr); }

BGBDY_API void dyllThunkSetAddr(dyt fcn, char *name, void *addr)
	{ dyllEnvSetAddr(dyllThunkEnv(fcn), name, addr); }
BGBDY_API void *dyllThunkGetAddr(dyt fcn, char *name)
	{ return(dyllEnvGetAddr(dyllThunkEnv(fcn), name)); }
BGBDY_API char *dyllThunkGetSig(dyt fcn, char *name)
	{ return(dyllEnvGetSig(dyllThunkEnv(fcn), name)); }

BGBDY_API dyt dyllAsmThunk(char *buf)
{
	BLNK_Thunk *obj;

	BGBGC_InitDyLL();

	dyll_lock();
	BASM_BeginAssembly(NULL);
	basm_puts(buf);
	obj=BASM_EndAssemblyThunk(NULL);
	dyll_unlock();

	return((dyt)obj);
}

BGBDY_API dyt dyllAsmThunkEnv(char *buf, dyt env)
{
	BLNK_Thunk *obj;

	BGBGC_InitDyLL();

	dyll_lock();
	BASM_BeginAssembly(NULL);
	basm_puts(buf);
	obj=BASM_EndAssemblyThunk((BLNK_Scope *)env);
	dyll_unlock();

	return((dyt)obj);
}

BGBDY_API dyt dyllAsmThunkSuperEnv(char *buf, dyt senv)
	{ return(dyllAsmThunkEnv(buf, dyllNewEnv(senv))); }

BGBDY_API dyt dyllAsmThunkInline(char *buf)
{
	byte *obj;

	BGBGC_InitDyLL();

	dyll_lock();
	BASM_BeginAssembly(NULL);
	basm_puts(buf);
	obj=BASM_EndAssemblyThunkInline(NULL);
	dyll_unlock();

	return((dyt)obj);
}

BGBDY_API dyt dyllAsmThunkInlineEnv(char *buf, dyt env)
{
	byte *obj;

	BGBGC_InitDyLL();

	dyll_lock();
	BASM_BeginAssembly(NULL);
	basm_puts(buf);
	obj=BASM_EndAssemblyThunkInline((BLNK_Scope *)env);
	dyll_unlock();

	return((dyt)obj);
}

BGBDY_API dyt dyllAsmThunkInlineSuperEnv(char *buf, dyt senv)
	{ return(dyllAsmThunkInlineEnv(buf, dyllNewEnv(senv))); }

BGBDY_API dyt dyllAsmModuleBuffer(char *name, char *buf)
{
	byte *obj;

	BGBGC_InitDyLL();

	dyll_lock();
	BASM_BeginAssembly(name);
	basm_puts(buf);
	obj=BASM_EndAssembly();
	dyll_unlock();

	return((dyt)obj);
}


BGBDY_API void dyllAsmBegin()
{
	BGBGC_InitDyLL();

	dyll_lock();
	BASM_BeginAssembly(NULL);
	dyll_unlock();
}

BGBDY_API void *dyllAsmEnd()
{
	void *p;
	
	dyll_lock();
	p=BASM_EndAssembly();
	dyll_unlock();
	return(p);
}

BGBDY_API void *dyllAsmEndDebug()
{
	void *p;
	char *str;
	
	str=BASM_GetAsmBuf();
//	dyPrintf("dyllAsmEndDebug:\n%s\n", str);
	gc_logprintf("dyllAsmEndDebug: [[\n%s]]\n", str);

	dyll_lock();
	p=BASM_EndAssembly();
	dyll_unlock();
	return(p);
}

BGBDY_API void dyllAsmBeginThunkInline()
{
	BGBGC_InitDyLL();

	dyll_lock();
	BASM_BeginAssembly(NULL);
	dyll_unlock();
}

BGBDY_API void *dyllAsmEndThunkInline()
{
	void *p;
	
//	char *str;
//	str=BASM_GetAsmBuf();
//	dyPrintf("dyllAsmEndThunkInline:\n%s\n", str);

	dyll_lock();
	p=BASM_EndAssemblyThunkInline(NULL);
	dyll_unlock();
	return(p);
}

BGBDY_API void *dyllAsmEndThunkInlineDebug()
{
	char *str;
	void *p;
	
	str=BASM_GetAsmBuf();
	dyPrintf("dyllAsmEndThunkInlineDebug:\n%s\n", str);

	dyll_lock();
	p=BASM_EndAssemblyThunkInline(NULL);
	dyll_unlock();
	return(p);
}

BGBDY_API void dyllAsmPuts(char *str)
{
	dyll_lock();
	basm_puts(str);
	dyll_unlock();
}

BGBDY_API void dyllAsmPrintv(char *str, va_list lst)
{
	dyll_lock();
	basm_vprint(str, lst);
	dyll_unlock();
}

BGBDY_API void dyllAsmPrint(char *str, ...)
{
	va_list lst;
	va_start(lst, str);
	dyll_lock();
	basm_vprint(str, lst);
	dyll_unlock();
	va_end(lst);
}

//char *dyllGenSym()
//	{ return(BASM_GenSym()); }

// int BASM_RegisterLinkMeta(char *name, basm_meta_ft fcn);
// int BASM_RegisterLinkMetaTrigger(char *name, basm_mtrg_ft fcn);

BGBDY_API int dyllAsmRegisterLinkMeta(char *name,
	void *(*fcn)(char *sym, char *name, char **args))
{
	BGBGC_InitDyLL();
	return(BASM_RegisterLinkMeta(name, fcn));
}

BGBDY_API int dyllAsmRegisterLinkNotify(char *name,
	void (*fcn)(char *sym, char *name, char **args, void *ptr))
{
	BGBGC_InitDyLL();
	return(BASM_RegisterLinkMetaTrigger(name, fcn));
}

BGBDY_API char *dyllComposeLinkMetaName(char *name, char **args)
	{ return(BASM_ComposeLinkMetaName(name, args)); }
BGBDY_API char *dyllComposeLinkNotifyName(char *name, char **args)
	{ return(BASM_ComposeLinkNotifyName(name, args)); }

#endif

BGBDY_API char *dyllComposeLinkMetaName0(char *name)
{
	char *args[4];
	args[0]=NULL;
	return(BASM_ComposeLinkMetaName(name, args));
}

BGBDY_API char *dyllComposeLinkMetaName1(char *name, char *ar0)
{	char *args[4]; args[0]=ar0; args[1]=NULL;
	return(BASM_ComposeLinkMetaName(name, args));
}

BGBDY_API char *dyllComposeLinkMetaName2(char *name, char *ar0, char *ar1)
{	char *args[4]; args[0]=ar0; args[1]=ar1; args[2]=NULL;
	return(BASM_ComposeLinkMetaName(name, args));
}

BGBDY_API char *dyllComposeLinkMetaName3(char *name, char *ar0, char *ar1, char *ar2)
{	char *args[4]; args[0]=ar0; args[1]=ar1; args[2]=ar2; args[3]=NULL;
	return(BASM_ComposeLinkMetaName(name, args));
}

BGBDY_API char *dyllComposeLinkMetaName4(char *name,
		char *ar0, char *ar1, char *ar2, char *ar3)
{	char *args[8];
	args[0]=ar0; args[1]=ar1; args[2]=ar2; args[3]=ar3; args[4]=NULL;
	return(BASM_ComposeLinkMetaName(name, args));
}

BGBDY_API char *dyllGetAddrSig(void *fcn)
{
	byte *buf;
	char *s, *s1;
	int i;

	s=dyllGetAddrName(fcn);
	if(!s)return(NULL);

	if(!strncmp(s, "_XC_", 4))
	{
		buf=gcralloc(256);
		BASM_UnmangleXCall(s+4, buf);

		s1=buf;
		while(*s1 && (*s1!='('))s1++;
		if(*s1!='(')return(NULL);
		return(s1);
	}

	s1=dyllMetaLookupKey(s);
	if(!s1 || strcmp(s1, "func"))
		return(NULL);

	buf=gcralloc(256);
	sprintf(buf, "%s:sig", s);
	s1=dyllMetaLookupKey(buf);
	if(!s1)return(NULL);

	return(s1);
}

BGBDY_API char *dyllGetNameSig(char *name)
{
	byte *buf;
	char *s, *s1;
	int i;

	s=name;
	if(!s)return(NULL);

	if(!strncmp(s, "_XC_", 4))
	{
		buf=gcralloc(256);
		BASM_UnmangleXCall(s+4, buf);

		s1=buf;
		while(*s1 && (*s1!='('))s1++;
		if(*s1!='(')return(NULL);
		return(s1);
	}

	s1=dyllMetaLookupKey(s);
	if(!s1) return(NULL);

//	if(!s1 || strcmp(s1, "func"))
//		return(NULL);

	if(!strcmp(s1, "func"))
	{
		buf=gcralloc(256);
		sprintf(buf, "%s:sig", s);
		s1=dyllMetaLookupKey(buf);
		if(!s1)return(NULL);
		return(s1);
	}

	if(!strcmp(s1, "var"))
	{
		buf=gcralloc(256);
		sprintf(buf, "%s:sig", s);
		s1=dyllMetaLookupKey(buf);
		if(!s1)return(NULL);
		return(s1);
	}

	return(NULL);
}

BGBDY_API void *dyllApplyPtrV(void *fcn, va_list lst)
{
	byte *buf;
	char *s;
	int i;

	s=dyllGetAddrSig(fcn);
	if(!s)return(NULL);

	buf=gcralloc(256);
	i=dyllRepackBufSigV(s, buf, lst);
	if(i<0)return(NULL);
	BASM_CallSig(fcn, s, buf, buf);
	return(buf);
}

BGBDY_API void *dyllApplyPtrU(void *fcn, void **args)
{
	byte *buf;
	char *s;
	int i;

	s=dyllGetAddrSig(fcn);
	if(!s)return(NULL);

	buf=gcralloc(256);
	i=dyllRepackBufSigU(s, buf, args);
	if(i<0)return(NULL);
	BASM_CallSig(fcn, s, buf, buf);
	return(buf);
}

BGBDY_API int dyllApplyPtrVi(void *fcn, va_list lst)
	{ void *p; p=dyllApplyPtrV(fcn, lst);
	return(p?(*(int *)p):0); }
BGBDY_API s64 dyllApplyPtrVl(void *fcn, va_list lst)
	{ void *p; p=dyllApplyPtrV(fcn, lst);
	return(p?(*(s64 *)p):0); }
BGBDY_API float dyllApplyPtrVf(void *fcn, va_list lst)
	{ void *p; p=dyllApplyPtrV(fcn, lst);
	return(p?(*(float *)p):0); }
BGBDY_API double dyllApplyPtrVd(void *fcn, va_list lst)
	{ void *p; p=dyllApplyPtrV(fcn, lst);
	return(p?(*(double *)p):0); }
BGBDY_API void *dyllApplyPtrVp(void *fcn, va_list lst)
	{ void *p; p=dyllApplyPtrV(fcn, lst);
	return(p?(*(void **)p):NULL); }

BGBDY_API void *dyllApplyPtrObjV(void *fcn, void *obj, va_list lst)
{
	byte *buf;
	char *s, *s1;
	int i;

	s=dyllGetAddrSig(fcn);
	if(!s)return(NULL);

	s1=s;
	if(*s1=='(')s1++;
	s1=dyllSigNext(s1);

	buf=gcralloc(256);
	i=dyllRepackBufSigV(s1, buf+sizeof(void *), lst);
	*(void **)buf=obj;

	if(i<0)return(NULL);
	dyll_lock();
	BASM_CallSig(fcn, s, buf, buf);
	dyll_unlock();
	return(buf);
}

BGBDY_API void *dyllApplyPtrObjU(void *fcn, void *obj, void **args)
{
	byte *buf;
	char *s, *s1;
	int i;

	s=dyllGetAddrSig(fcn);
	if(!s)return(NULL);

	s1=s;
	if(*s1=='(')s1++;
	s1=dyllSigNext(s1);

	buf=gcralloc(256);
	i=dyllRepackBufSigU(s1, buf+sizeof(void *), args);
	*(void **)buf=obj;

	if(i<0)return(NULL);
	dyll_lock();
	BASM_CallSig(fcn, s, buf, buf);
	dyll_unlock();
	return(buf);
}


BGBDY_API void *dyllApplyPtrSigV(void *fcn, char *sig, va_list lst)
{
	byte *buf;
	int i;

	buf=gcralloc(256);
	i=dyllRepackBufSigV(sig, buf, lst);
	if(i<0)return(NULL);
	dyll_lock();
	BASM_CallSig(fcn, sig, buf, buf);
	dyll_unlock();
	return(buf);
}

BGBDY_API void *dyllApplyPtrSigU(void *fcn, char *sig, void **args)
{
	byte *buf;
	int i;

	buf=gcralloc(256);
	i=dyllRepackBufSigU(sig, buf, args);
	if(i<0)return(NULL);
	dyll_lock();
	BASM_CallSig(fcn, sig, buf, buf);
	dyll_unlock();
	return(buf);
}

BGBDY_API int dyllApplyPtrSigVi(void *fcn, char *sig, va_list lst)
	{ void *p; p=dyllApplyPtrSigV(fcn, sig, lst);
	return(p?(*(int *)p):0); }
BGBDY_API s64 dyllApplyPtrSigVl(void *fcn, char *sig, va_list lst)
	{ void *p; p=dyllApplyPtrSigV(fcn, sig, lst);
	return(p?(*(s64 *)p):0); }
BGBDY_API float dyllApplyPtrSigVf(void *fcn, char *sig, va_list lst)
	{ void *p; p=dyllApplyPtrSigV(fcn, sig, lst);
	return(p?(*(float *)p):0); }
BGBDY_API double dyllApplyPtrSigVd(void *fcn, char *sig, va_list lst)
	{ void *p; p=dyllApplyPtrSigV(fcn, sig, lst);
	return(p?(*(double *)p):0); }
BGBDY_API void *dyllApplyPtrSigVp(void *fcn, char *sig, va_list lst)
	{ void *p; p=dyllApplyPtrSigV(fcn, sig, lst);
	return(p?(*(void **)p):NULL); }

BGBDY_API void *dyllApplyPtrSigObjV(void *fcn, char *sig,
	void *obj, va_list lst)
{
	byte *buf;
	char *s, *s1;
	int i;

	s1=sig;
	if(*s1=='(')s1++;
	s1=dyllSigNext(s1);

	buf=gcralloc(256);
	i=dyllRepackBufSigV(s1, buf+sizeof(void *), lst);
	*(void **)buf=obj;

	if(i<0)return(NULL);
	dyll_lock();
	BASM_CallSig(fcn, s1, buf, buf);
	dyll_unlock();
	return(buf);
}

BGBDY_API void *dyllApplyPtrSigObjU(void *fcn, char *sig,
	void *obj, void **args)
{
	byte *buf;
	char *s, *s1;
	int i;

	s=dyllGetAddrSig(fcn);
	if(!s)return(NULL);

	s1=sig;
	if(*s1=='(')s1++;
	s1=dyllSigNext(s1);

	buf=gcralloc(256);
	i=dyllRepackBufSigU(s1, buf+sizeof(void *), args);
	*(void **)buf=obj;

	if(i<0)return(NULL);
	dyll_lock();
	BASM_CallSig(fcn, s, buf, buf);
	dyll_unlock();
	return(buf);
}


BGBDY_API void *dyllWrapClosure(void *fcn, void *data, char *sig)
{
	void *p;

	BGBGC_InitDyLL();
	dyll_lock();
	p=BASM_CreateStubGC(fcn, data, sig);
	dyll_unlock();
	return(p);
}

BGBDY_API dyt dyllGetFuncAddrObj(char *name)
{
	return(dyllGetDynamic(name));
}



int DYLL_LoadLibraryI(char *name)
{
	char tb[256], tb2[256], tbn[256];
	char *s;
	int i, j, exw;

	for(i=0; i<dyll_nloads; i++)
		if(!strcmp(dyll_loads[i], name))
			return(1);

	dyPrintf("DYLL_LoadLibraryI: Load %s\n", name);
	dyll_lock();
	i=BLNK_LoadLibrary(name);
	dyll_unlock();

	if(i>=0)
	{
		strcpy(tbn, name);
		dyPrintf("DYLL_LoadLibraryI: Success Loading %s\n", name);
	}

	if(i<0)
	{
		for(j=0; j<dyll_nlib; j++)
		{
			sprintf(tbn, "%s/%s", dyll_lib[j], name);
			dyPrintf("DYLL_LoadLibraryI: Load %s\n", tbn);
			dyll_lock();
			i=BLNK_LoadLibrary(tbn);
			dyll_unlock();
			if(i>=0)
			{
				dyPrintf("DYLL_LoadLibraryI: Success Loading %s\n", tbn);
				break;
			}	
		}
	}

	if(i<0)return(-1);

	strcpy(tb, name);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.'))s--;

//	if((*s=='.') || (s>=tb))
	if((*s=='.') && (s>tb))
	{
		*s=0;
	
		sprintf(tb2, "exwad/%s", tb);
		exw=vf_mount(tbn, tb2, "exwad", NULL);

//		if(s<=tb)s=tb+strlen(tb);
//		strcpy(s, "_meta.txt");

		sprintf(tb2, "%s%s", tb, "_meta.txt");
//		j=dyllMetaLoadDB(tb2);
		j=dyllMetaLoadDB2(tb2, tb);

		if((j<0) && (exw>=0))
		{
			sprintf(tb2, "exwad/%s/%s", tb, "meta.txt");
			j=dyllMetaLoadDB2(tb2, tb);
		}

		if(j>=0)
		{
			dyll_lmods[dyll_nlmods++]=
				basm_strdup(tb);
		}

//		j=dyllMetaLoadDB(tb);
		dyPrintf("Load DB '%s': %s\n",
			tb, (j<0)?"fail":"pass");
	}

	dyll_loads[dyll_nloads++]=
		basm_strdup(name);
	return(i);
}

int DYLL_LoadLibraryI2(char *name)
{
	char tb[256];
	int i;

//	i=DYLL_LoadLibraryI(name);
//	if(i>=0)return(i);

#ifdef linux
	sprintf(tb, "%s.so", name);
	i=DYLL_LoadLibraryI(tb);
	if(i>=0)return(i);

	sprintf(tb, "lib%s.so", name);
	i=DYLL_LoadLibraryI(tb);
	if(i>=0)return(i);
#endif

#ifdef WIN32
	sprintf(tb, "%s.dll", name);
	i=DYLL_LoadLibraryI(tb);
	if(i>=0)return(i);

//	sprintf(tb, "%s.lib", name);
//	i=DYLL_LoadLibraryI(tb);
//	if(i>=0)return(i);
#endif

#if 0
	sprintf(tb, "%s.a", name);
	i=DYLL_LoadLibraryI(tb);
	if(i>=0)return(i);

	sprintf(tb, "lib%s.a", name);
	i=DYLL_LoadLibraryI(tb);
	if(i>=0)return(i);
#endif

	return(-1);
}

int DYLL_LoadLibrary(char *name)
{
	int i;

	for(i=0; i<dyll_nloads; i++)
		if(!strcmp(dyll_loads[i], name))
			return(1);
	for(i=0; i<dyll_nlmods; i++)
		if(!strcmp(dyll_lmods[i], name))
			return(1);

	i=DYLL_LoadLibraryI2(name);
	if(i<0)return(-1);

	printf("DYLL_LoadLibrary: Loaded %s\n", name);

	dyll_loads[dyll_nloads++]=
		basm_strdup(name);
	dyll_lmods[dyll_nlmods++]=
		basm_strdup(name);
	return(i);
}

BGBDY_API int dyllLoadLibrary(char *name)
{
	BGBGC_InitDyLL();
	return(DYLL_LoadLibrary(name));
}

BGBDY_API dyt dyllCallTopA(char *name, dyt *args, int nargs)
{
	dyt t, f;
	f=dyllGetDynamic(name);
	if(!f || (f==UNDEFINED))
		return(UNDEFINED);
	t=dyApplyMethod(NULL, f, args, nargs);
	return(t);
}
