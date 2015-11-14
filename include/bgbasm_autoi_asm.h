//AHSRC:asm/basm_core.c
void BASM_SetCPU(BASM_Context *ctx, int cpu);
void basm_warning(char *str, ...);
void basm_error(char *str, ...);
void BASM_ModRM16(BASM_Context *ctx, int reg,int rm, int sc, int idx, int disp, char *lbl);
void BASM_Sib(BASM_Context *ctx, int rm, int sc, int idx);
void BASM_LabelDispRel(BASM_Context *ctx, char *lbl, int disp);
void BASM_ModRM64(BASM_Context *ctx, int reg,int rm, int sc, int idx, int disp, char *lbl);
int BASM_OpArgRegP(int i, int j, int reg);
int BASM_OpArgRegP(int i, int j, int reg);
int BASM_OpArgMemP(int i, int j, int w, int fl);
int BASM_OpArgMemP(int i, int j, int w, int fl);
int BASM_OpArgImmP(int i, int j, int w, long long imm);
int BASM_OpArchP(BASM_Context *ctx, int i);
int BASM_OpRegP(int i, int reg);
int BASM_OpImmP(int i, int w, long long imm);
int BASM_OpMemP(int i, int w);
int BASM_OpRegRegP(int i, int r0, int r1);
int BASM_OpRegImmP(int i, int reg, long long imm);
int BASM_OpRegMemP(int i, int reg, int fl);
int BASM_OpMemRegP(int i, int reg, int fl);
int BASM_OpMemImmP(int i, int w, long long imm);
int BASM_OpRegRegImmP(int i, int r0, int r1, long long imm);
int BASM_OpRegMemImmP(int i, int reg, int fl, long long imm);
int BASM_OpMemRegImmP(int i, int reg, int fl, long long imm);
int BASM_OpRegRegRegP(int i, int r0, int r1, int r2);
int BASM_OpRegMemRegP(int i, int r0, int r1, int fl);
int BASM_OpMemRegRegP(int i, int r0, int r1, int fl);
//AHSRC:asm/basm_parse.c
char *basm_strdup_alloc(char *str);
u32 basm_atof_u32(char *str);
u64 basm_atof_u64(char *str);
u32 basm_atof_u32_sg(char *str, int sg);
u64 basm_atof_u64_sg(char *str, int sg);
s64 basm_atoi(char *str);
int basm_stricmp(char *s1, char *s2);
int BASM_Parse_MatchTokenListLen(char *str, char **lst);
char *BASM_Parse_TokenGAS(char *s, char *b, int *ty);
char *BASM_ParseArgGAS(BASM_Context *ctx, char *str,int *breg, int *ireg, int *sc, long long *disp, char **lbl);
int BASM_CheckARMCC(BASM_Context *ctx, char **str);
char *BASM_ParseOpcodeInner(BASM_Context *ctx, char *str,char *opname, int opnum);
//AHSRC:asm/basm_insns_x86.c
//AHSRC:asm/basm_insns_arm.c
//AHSRC:asm/basm_insns_thumb.c
//AHSRC:asm/basm_disasm.c
int BDISASM_CheckOpStr(byte **rip, char **str, int *rfl, int op);
byte *BDISASM_PrintModRM_RM16(byte *ip, int op, int fl, int w, int ar);
byte *BDISASM_PrintModRM_RM(byte *ip, int op, int fl, int w, int ar);
byte *BDISASM_PrintModRM_RegRM(byte *ip, int op, int fl, int w);
byte *BDISASM_PrintModRM_RMReg(byte *ip, int op, int fl, int w);
byte *BDISASM_SkipModRM(byte *ip, int fl);
int BDISASM_SizeOpStr(byte *oip, char *str, int fl);
char *BDISASM_GetFixReg(int i, int j);
void BDISASM_PrintFixRegPF(int i);
void BDISASM_PrintFixRegSF(int i);
//AHSRC:asm/basm_coff.c
int BASM_COFF_ResolveRelocs(BASM_Context *ctx, BASM_COFF_Info *inf, int sec,int offs, int cnt);
BASM_Context *BASM_COFF_LoadObjectBuf(char *name, byte *buf, int sz);
BASM_Context *BASM_COFF_LoadObject(char *name);
int BASM_COFF_LoadLibrary(char *name);
void basm_coff_set16(byte *ptr, int i);
void basm_coff_set32(byte *ptr, int i);
int basm_coff_get16(byte *ptr);
int basm_coff_get32(byte *ptr);
int basm_coff_idxstr(char *strs, char *str);
int basm_coff_idxstr2(char *strs, char *str, int fl);
int BASM_COFF_StoreObject(char *name, BASM_Context *ctx);
