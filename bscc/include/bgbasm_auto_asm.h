//AHSRC:asm/basm_core.c
//AHSRC:asm/basm_parse.c
BASM_API char *BASM_ComposeLinkMetaName(char *name, char **args);
BASM_API char *BASM_ComposeLinkNotifyName(char *name, char **args);
BASM_API void BASM_RegisterSpecialOp(char *name,char *(*fcn)(BASM_Context *ctx, char *op, char **str));
BASM_API char *basm_strdup(char *str);
//AHSRC:asm/basm_insns_x86.c
//AHSRC:asm/basm_insns_arm.c
//AHSRC:asm/basm_insns_thumb.c
//AHSRC:asm/basm_disasm.c
BASM_API char *BASM_GetPtrName(void *ptr);
BASM_API byte *BDISASM_PrintOpcode(byte *ip, int *rfl);
BASM_API int BDISASM_PrintOpcodes(byte *ip, int fl, int sz);
BASM_API int BDISASM_HexDump(byte *ip, int sz);
//AHSRC:asm/basm_coff.c
