//AHSRC:link/link_context.c
//AHSRC:link/link_core.c
BASM_API BLNK_GCFuncs_t *BLNK_GetGCFuncs();
BASM_API void BLNK_SetScanRange(int (*fcn)(void **p, int cnt));
BASM_API int BLNK_RegisterLookup(blnk_lookup_ft fcn);
BASM_API int BLNK_RegisterNotify(blnk_notify_ft fcn);
BASM_API void BLNK_MarkData();
BASM_API int BLNK_CheckPointerImageP(void *p);
BASM_API void BLNK_AssignSym(char *name, void *ptr);
BASM_API void BLNK_AddProxyPtr(char *name, void *ptr);
BASM_API void BLNK_AddProxy(char *name);
BASM_API void BLNK_AddProxyImport(char *name);
BASM_API int BLNK_LoadLibrary(char *name);
//AHSRC:link/link_thunk.c
BASM_API BLNK_Scope *BLNK_NewScope(BLNK_Scope *super);
BASM_API void BLNK_FreeScope(BLNK_Scope *scope);
BASM_API int BLNK_IndexScopeSym(BLNK_Scope *scope, char *name);
BASM_API void BLNK_BindScopeSym(BLNK_Scope *scope, char *name, void *ptr);
BASM_API void BLNK_BindScopeSymSig(BLNK_Scope *scope,char *name, char *sig, void *ptr);
BASM_API void BLNK_AssignScopeSym(BLNK_Scope *scope, char *name, void *ptr);
BASM_API void *BLNK_FetchScopeSym(BLNK_Scope *scope, char *name);
BASM_API void *BLNK_FetchScopeSym2(BLNK_Scope *scope, char *name);
BASM_API char *BLNK_FetchScopeSig(BLNK_Scope *scope, char *name);
BASM_API BLNK_Thunk *BLNK_LinkModuleThunk(BLNK_Object *ctx, BLNK_Scope *scope);
BASM_API byte *BLNK_LinkThunkInline(BLNK_Object *ctx, BLNK_Scope *scope);
BASM_API BLNK_Thunk *BLNK_LinkModuleThunkObj(BLNK_Scope *scope, void *buf, int sz);
BASM_API byte *BLNK_LinkModuleThunkInlineObj(BLNK_Scope *scope, void *buf, int sz);
//AHSRC:link/link_lnx.c
BASM_API int BLNK_GetModuleListOS(char **buf, int max);
BASM_API char **BLNK_GetEnvironOS();
BASM_API void *BLNK_LoadModuleFileDataOS(char *name, int *rsz);
BASM_API void *BLNK_LoadModuleWadDataOS(char *name, int *rsz);
BASM_API void BLNK_FreeModuleDataOS(void *buf);
//AHSRC:link/link_nacl.c
BASM_API int BLNK_GetModuleListOS(char **buf, int max);
BASM_API char **BLNK_GetEnvironOS();
BASM_API void *BLNK_LoadModuleFileDataOS(char *name, int *rsz);
BASM_API void *BLNK_LoadModuleWadDataOS(char *name, int *rsz);
BASM_API void BLNK_FreeModuleDataOS(void *buf);
//AHSRC:link/link_w32.c
BASM_API int BLNK_GetModuleListOS(char **buf, int max);
BASM_API char **BLNK_GetEnvironOS();
BASM_API void *BLNK_LoadModuleFileDataOS(char *name, int *rsz);
BASM_API void *BLNK_LoadModuleWadDataOS(char *name, int *rsz);
BASM_API void BLNK_FreeModuleDataOS(void *buf);
//AHSRC:link/link_vfs.c
BASM_API BLNK_IOFuncs_t *BLNK_GetIOFuncs();
//AHSRC:link/bldr_coff.c
//AHSRC:link/bldr_elf.c
