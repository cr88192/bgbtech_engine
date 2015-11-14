//AHSRC:front/basm_api.c
int BASM_Win64CheckEpilogueRet(byte *ip);
void *BASM_GetIP();
void *BASM_GetDP();
//AHSRC:front/basm_rand.c
int basm_genseed();
void BASM_InitRand();
u32 basm_rand();
char *basm_rand_key12();
char *basm_rand_key18();
//AHSRC:front/basm_callsig.c
int BASM_LenSig(char *sig, char **esig);
int BASM_LenSig(char *sig, char **esig);
int BASM_SplitArgsSig(char *sig, char **esig,byte *buf, byte *abuf, byte *greg, byte *xreg);
int BASM_SplitArgsSig(char *sig, char **esig,byte *buf, byte *abuf, byte *greg, byte *xreg);
void BASM_ProcessSigArgs(char *sig, va_list lst, void *buf);
int BASM_MergeArgsSig(char *sig, char **esig,byte *buf, byte *abuf, byte *greg, byte *xreg);
int BASM_MergeArgsSig(char *sig, char **esig,byte *buf, byte *abuf, byte *greg, byte *xreg);
void BASM_CreateStub_Inner32(void *args, void **data);
void BASM_CreateStub_Inner64(void *args, void **data, void *greg, void *xreg);
void BASM_InitSig();
//AHSRC:front/basm_vfs.c
s64 basm_tell_tmp(void *fd);
int basm_seek_tmp(void *fd, s64 pos, int rel);
BGBASM_IOFuncs_t *basm_getio();
void basm_initvfs();
void *basm_fopen(char *name, char *mode);
void basm_fclose(void *fd);
int basm_fread(void *buf, int m, int n, void *fd);
int basm_fwrite(void *buf, int m, int n, void *fd);
int basm_feof(void *fd);
s64 basm_ftell(void *fd);
int basm_fseek(void *fd, s64 pos, int rel);
int basm_fgetc(void *fd);
void basm_fputc(int c, void *fd);
void *basm_loadfile(char *name, int *rsz);
int basm_storefile(char *name, void *buf, int sz);
int basm_storetextfile(char *name, char *buf);
//AHSRC:front/basm_xcall.c
void *BASM_GetLabelXCall(char *name);
void *BASM_GetLabelNativeXCall(char *name);
void *BASM_GetLabelXTls(char *name);
//AHSRC:front/basm_xc_sysv.c
int BASM_XCallSplitSigSysV(char *sig, char **esig, int *rni, int *rnx, int *rns);
int BASM_XCallGenRepackSigSysV(char *sig);
void *BASM_XCallGenThunkSysV(char *name);
//AHSRC:front/basm_xc_w64.c
int BASM_XCallSplitSigW64(char *sig, char **esig, int *rni, int *rns);
int BASM_XCallGenRepackSigW64(char *sig);
int BASM_XCallGenNatRepackSigW64(char *sig);
void *BASM_XCallGenThunkW64(char *name);
int BASM_XCallCheckSigDirectW64(char *sig);
void *BASM_XCallGenNatThunkW64(char *name, char *xcname);
//AHSRC:front/basm_xmeta.c
BASM_XMeta *BASM_LookupXMetaInfo(char *name);
BASM_XMeta *BASM_GetXMetaInfo(char *name);
void *BASM_GetLabelXMeta(char *name);
void BASM_NotifyLabelXMeta(char *name, void *ptr);
void *basm_xmeta_lookup(char *name);
void basm_xmeta_notify(char *name, void *ptr);
int BASM_InitXMeta();
//AHSRC:front/basm_unwind.c
void BASM_InitUnwind();
//AHSRC:front/basm_preproc.c
void *basm_loadfile(char *name, int *rsz);
int BASM_PP_AddIncludePathFront(char *str);
int BASM_PP_AddIncludePathBack(char *str);
int BASM_PP_AddVirtualHeader(char *name, char *buf);
char *BASM_PP_LookupVirtualHeader(char *name);
char *BASM_PP_EmitString(char *t, char *s);
char *BASM_PP_EmitCharString(char *t, char *s);
void BASM_PP_DeleteDefine(char *name);
void BASM_PP_CleanupDefines();
void BASM_PP_CleanupLocalDefines();
void BASM_PP_AddDefine(char *name, char *str);
void BASM_PP_AddDefineArgs(char *name, char **args, char *str);
void BASM_PP_AppendDefineLine(char *name, char *str);
void BASM_PP_AddStaticDefine(char *name, char *str);
void BASM_PP_AddStaticDefineArgs(char *name, char **args, char *str);
void BASM_PP_AddLocalDefine(char *name, char *str);
void BASM_PP_AddLocalDefineArgs(char *name, char **args, char *str);
char *BASM_PP_LoadInclude(char *name, int *rsz);
void BASM_PP_Include(char *str);
void BASM_PP_PushIncludeLevel();
void BASM_PP_PopIncludeLevel();
void BASM_PP_Directive2(char *str);
void BASM_PP_Directive(char *str);
char *BASM_PP_ParseArg(char *s, char *b);
char *BASM_PP_PrintName(char *t, char *s);
char *BASM_PP_PrintToken(char *t, char *s, int ty);
int BASM_PP_Expand(char **src, char **dst, char *op);
int BASM_PP_ExpandBinary(char *larg, char *op, char *rarg, char *dst);
int BASM_PP_LineArith(char *str);
int BASM_PP_Line(char *str);
char *BASM_PP_ParseLine(char *s, char *b);
void BASM_PP_Buffer(char *ibuf);
int BASM_PP_Filter(char *ibuf, char *obuf);
