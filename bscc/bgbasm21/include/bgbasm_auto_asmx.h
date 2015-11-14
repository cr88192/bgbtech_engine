//AHSRC:asmx/basmx_core.c
BASMX_API char *BASMX_ParseOpcodeI(BASM_Context *ctx,char *str, BASMX_Opcode *dop);
BASMX_API BASMX_OpArg *BASMX_CloneOpArgMalloc(BASM_Context *ctx, BASMX_OpArg *op);
BASMX_API BASMX_Opcode *BASMX_CloneOpcodeMalloc(BASM_Context *ctx, BASMX_Opcode *op);
BASMX_API BASMX_Opcode *BASMX_ParseOpcodeMalloc(BASM_Context *ctx, char **str);
BASMX_API BASMX_Opcode **BASMX_ParseOpcodeListMalloc(BASM_Context *ctx, char **str);
BASMX_API void BASMX_OutOpcode(BASM_Context *ctx, BASMX_Opcode *op);
BASMX_API void BASMX_OutOpcodeList(BASM_Context *ctx, BASMX_Opcode **ops);
BASMX_API void BASMX_BeginAssembly(BASM_Context *ctx, char *name);
BASMX_API void *BASMX_EndAssembly(BASM_Context *ctx);
BASMX_API byte *BASMX_EndAssemblyObjBuf(BASM_Context *ctx, int *rsz);
BASMX_API byte *BASMX_EndAssemblyCacheObjBuf(BASM_Context *ctx, int *rsz);
BASMX_API void BASMX_EndAssemblyQueue(BASM_Context *ctx);
