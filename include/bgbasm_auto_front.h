//AHSRC:front/basm_api.c
BASM_API char *BASM_CPUID_String();
BASM_API u32 BASM_CPUID_Version();
BASM_API u32 BASM_CPUID_Unit();
BASM_API u32 BASM_CPUID_Ext();
BASM_API u32 BASM_CPUID_Feature();
BASM_API char *BASM_CPUID_FeatureStr();
BASM_API void BASM_AddNameBlacklist(char *name);
BASM_API void BASM_AddNamesBlacklist(char *nameslst);
BASM_API void *BASM_GetEBP();
BASM_API int BASM_GetReturnState(void **regs);
BASM_API int BASM_StackTrace(void **ip, void **bp, int lim);
BASM_API int BASM_StackTrace(void **ip, void **bp, int lim);
BASM_API byte *BASM_Win64LocateEpilogue(byte *ip);
BASM_API byte *BASM_Win64AdjustEpilogueRSP(byte *ip, byte *sp,BGBASM_RegsBuf *regs);
BASM_API int BASM_StackTrace(void **ip, void **bp, int lim);
BASM_API int BASM_StackTrace(void **ip, void **bp, int lim);
BASM_API void basm_putc(int c);
BASM_API void basm_puts(char *str);
BASM_API void basm_vprint(char *str, va_list lst);
BASM_API void basm_print(char *str, ...);
BASM_API void BASM_RegisterPtr(char *lbl, void *ptr);
BASM_API void *BASM_GetPtr(char *lbl);
BASM_API char *BASM_GetPtrName(void *ptr);
BASM_API char *BASM_GetLastNamePtr(void *ptr, void **rbp);
BASM_API void *BASM_FetchSymPrefix(char *name);
BASM_API char *BASM_FetchSymPrefixName(char *name);
BASM_API char *BASM_LookupExeSectionPtrName(void *ptr);
BASM_API char *BASM_LookupSectionPtrName(void *ptr);
BASM_API void *BASM_CreateLabelBSS(char *lbl, int sz);
BASM_API void BASM_DumpModule(char *name);
BASM_API void *BASM_GetFPtrDummy(char *lbl);
BASM_API char *BASM_GetAsmBuf();
BASM_API void BASM_BeginAssembly(char *name);
BASM_API void *BASM_EndAssembly();
BASM_API void *BASM_EndAssemblyQuiet();
BASM_API void *BASM_EndAssemblyDebug();
BASM_API void *BASM_EndAssemblyCache(char *name);
BASM_API void BASM_EndAssemblyFile(char *name);
BASM_API byte *BASM_EndAssemblyObjBuf(int *rsz);
BASM_API byte *BASM_EndAssemblyCacheObjBuf(int *rsz);
BASM_API BLNK_Thunk *BASM_EndAssemblyThunk(BLNK_Scope *scope);
BASM_API byte *BASM_EndAssemblyThunkInline(BLNK_Scope *scope);
BASM_API void BASM_EndAssemblyQueue();
BASM_API void *BASM_EndAssemblyFast();
BASM_API byte *BASM_EndAssemblyObjBufFast(int *rsz);
BASM_API byte *BASM_EndAssemblyCacheObjBufFast(int *rsz);
BASM_API void BASM_EndAssemblyQueueFast();
BASM_API void BASM_EndAssemblyVirtualHeader();
BASM_API void BASM_FreeAssembly(void *ip);
BASM_API void BASM_FreeObjBuf(void *ip);
BASM_API char *BASM_GenSym();
BASM_API char *BASM_GenSym2();
BASM_API int BASM_LoadObjectBuffer(char *name, byte *buf, int sz);
BASM_API int BASM_LoadFile(char *name);
BASM_API int BASM_AssembleFile(char *iname, char *oname);
BASM_API int BASM_ProcessEXE(char *name);
BASM_API void BASM_ThreadLocalInit();
BASM_API void BASM_Init();
//AHSRC:front/basm_rand.c
//AHSRC:front/basm_callsig.c
BASM_API void BASM_CallSig(void *fcn, char *sig, void *buf, void *ret);
BASM_API void BASM_CallSigStd(void *fcn, char *sig, void *buf, void *ret);
BASM_API void BASM_CallSig(void *fcn, char *sig, void *buf, void *ret);
BASM_API void BASM_CallSigStd(void *fcn, char *sig, void *buf, void *ret);
BASM_API void BASM_CallSig(void *fcn, char *sig, void *buf, void *ret);
BASM_API void BASM_CreateStub(void *buf, void *fcn, void *data, char *sig);
BASM_API void *BASM_CreateStubGC(void *fcn, void *data, char *sig);
BASM_API void *BASM_MakeBufferCallStub(void *fcn, char *sig);
BASM_API void *BASM_MakeBufferCallStub(void *fcn, char *sig);
//AHSRC:front/basm_vfs.c
//AHSRC:front/basm_xcall.c
BASM_API int BASM_UnmangleXCall(char *str, char *buf);
BASM_API char **BASM_UnmangleSplit(char *str);
BASM_API int BASM_MangleXCall(char *str, char *buf);
//AHSRC:front/basm_xc_sysv.c
//AHSRC:front/basm_xc_w64.c
//AHSRC:front/basm_xmeta.c
BASM_API int BASM_RegisterLinkMeta(char *name, basm_meta_ft fcn);
BASM_API int BASM_RegisterLinkMetaTrigger(char *name, basm_mtrg_ft fcn);
BASM_API char *BASM_ComposeLinkMetaName(char *name, char **args);
BASM_API char *BASM_ComposeLinkNotifyName(char *name, char **args);
//AHSRC:front/basm_unwind.c
BASM_API int BASM_SaveRegs(BGBASM_RegsBuf *buf);
BASM_API void *BASM_SetJmp(BGBASM_JmpBuf *buf);
BASM_API void BASM_LongJmp(BGBASM_JmpBuf *buf, void *val);
BASM_API void *BASM_StartUnwind(BGBASM_JmpBuf *buf);
BASM_API void BASM_EndUnwind(BGBASM_JmpBuf *buf);
BASM_API void *BASM_BeginUnwind(BGBASM_JmpBuf *buf);
BASM_API void BASM_DoUnwind(void *val);
//AHSRC:front/basm_preproc.c
