//AHSRC:asm/basm_core.c
BASM2_API BASM_Context *BASM_NewContext();
BASM2_API void BASM_DestroyContext(BASM_Context *ctx);
BASM2_API void *BASM_TempAllocTy(BASM_Context *ctx, char *ty, int sz);
BASM2_API void *BASM_TempAlloc(BASM_Context *ctx, int sz);
BASM2_API void BASM_OutPadText(BASM_Context *ctx, int i);
BASM2_API void BASM_OutPadZero(BASM_Context *ctx, int i);
BASM2_API void BASM_OutPadVLI(BASM_Context *ctx, int i);
BASM2_API void BASM_OutPad(BASM_Context *ctx, int i);
BASM2_API void BASM_Align(BASM_Context *ctx, int i);
BASM2_API int BASM_GetSectionOffset(BASM_Context *ctx);
BASM2_API void BASM_BundleAlign(BASM_Context *ctx);
BASM2_API void BASM_Align(BASM_Context *ctx, int i);
BASM2_API void BASM_AlignVLI(BASM_Context *ctx, int i);
BASM2_API void BASM_OutByte(BASM_Context *ctx, int i);
BASM2_API void BASM_OutWord(BASM_Context *ctx, int i);
BASM2_API void BASM_OutDWord(BASM_Context *ctx, int i);
BASM2_API void BASM_OutQWord(BASM_Context *ctx, long long i);
BASM2_API void BASM_OutUVLIP(BASM_Context *ctx, unsigned long long i, int pad);
BASM2_API void BASM_OutUVLI(BASM_Context *ctx, unsigned long long i);
BASM2_API void BASM_OutSVLI(BASM_Context *ctx, long long i);
BASM2_API void BASM_OutSVLIP(BASM_Context *ctx, long long i, int pad);
BASM2_API void BASM_OutStr8(BASM_Context *ctx, char *str);
BASM2_API void BASM_OutStr16(BASM_Context *ctx, char *str);
BASM2_API void BASM_OutStr8Z(BASM_Context *ctx, char *str);
BASM2_API void BASM_OutStr16Z(BASM_Context *ctx, char *str);
BASM2_API void BASM_OutBytes(BASM_Context *ctx, byte *buf, int sz);
BASM2_API void BASM_OutSection(BASM_Context *ctx, char *str);
BASM2_API void BASM_OutBits(BASM_Context *ctx, int bits);
BASM2_API void BASM_OutRelPtrDisp(BASM_Context *ctx, char *lbl, int disp);
BASM2_API void BASM_OutRelSPtrDisp(BASM_Context *ctx, char *lbl, int disp);
BASM2_API void BASM_LabelDisp(BASM_Context *ctx, char *lbl, int disp);
BASM2_API void BASM_ModRM(BASM_Context *ctx, int reg,int rm, int sc, int idx, int disp, char *lbl);
BASM2_API int BASM_RegREXW(int reg);
BASM2_API int BASM_Reg16P(int reg);
BASM2_API int BASM_Reg32P(int reg);
BASM2_API void BASM_AddrOverride(BASM_Context *ctx, int breg);
BASM2_API void BASM_SegOverride(BASM_Context *ctx);
BASM2_API char *BASM_OutSufBytes(BASM_Context *ctx, char *s);
BASM2_API char *BASM_OutBodyBytes(BASM_Context *ctx, char *s, int rex);
BASM2_API char *BASM_OutImm(BASM_Context *ctx, char *s,long long imm, char *lbl);
BASM2_API char *BASM_OutMOffs(BASM_Context *ctx, char *s, int disp, char *lbl);
BASM2_API char *BASM_OutModRM(BASM_Context *ctx, char *s, int reg,int breg, int sc, int ireg, int disp, char *lbl);
BASM2_API char *BASM_OutArmReg(BASM_Context *ctx, char *s, int reg);
BASM2_API int BASM_SizeOpStr(BASM_Context *ctx, char *str);
BASM2_API void BASM_BundleAlignOpStr(BASM_Context *ctx, char *str);
BASM2_API void BASM_OutOpStr(BASM_Context *ctx, char *s);
BASM2_API void BASM_OutOpStrReg(BASM_Context *ctx, char *s, int reg);
BASM2_API void BASM_OutOpStrImm(BASM_Context *ctx, char *s, long long imm, char *lbl);
BASM2_API void BASM_OutOpStrMem(BASM_Context *ctx, char *s,char *lbl, int breg, int ireg, int sc, int disp);
BASM2_API void BASM_OutOpStrRegReg(BASM_Context *ctx, char *s, int r0, int r1);
BASM2_API void BASM_OutOpStrRegImm(BASM_Context *ctx, char *s, int reg,long long imm, char *lbl);
BASM2_API void BASM_OutOpStrRegMem(BASM_Context *ctx, char *s, int reg,char *lbl, int breg, int ireg, int sc, int disp);
BASM2_API void BASM_OutOpStrMemImm(BASM_Context *ctx, char *s, int w,char *lbl, int breg, int ireg, int sc, int disp, long long imm, char *lbl2);
BASM2_API void BASM_OutOpStrRegRegImm(BASM_Context *ctx, char *s,int r0, int r1, long long imm, char *lbl);
BASM2_API void BASM_OutOpStrRegMemImm(BASM_Context *ctx, char *s, int reg,char *lbl, int breg, int ireg, int sc, int disp, long long imm, char *lbl2);
BASM2_API void BASM_OutOpStrRegRegReg(BASM_Context *ctx, char *s,int r0, int r1, int r2);
BASM2_API void BASM_OutOpStrRegMemReg(BASM_Context *ctx, char *s, int reg,char *lbl, int breg, int ireg, int sc, int disp, int reg2);
BASM2_API int BASM_OpSingleP(int i);
BASM2_API int BASM_GetRegWidth(int reg);
BASM2_API void BASM_OutOpSingle(BASM_Context *ctx, int op);
BASM2_API void BASM_OutOpReg(BASM_Context *ctx, int op, int reg);
BASM2_API void BASM_OutOpImm(BASM_Context *ctx, int op, int w,long long imm, char *lbl);
BASM2_API void BASM_OutOpMem(BASM_Context *ctx, int op, int w,char *lbl, int breg, int ireg, int sc, long long disp);
BASM2_API void BASM_OutOpRegReg(BASM_Context *ctx, int op, int r0, int r1);
BASM2_API void BASM_OutOpRegImm(BASM_Context *ctx, int op,int reg, long long imm, char *lbl);
BASM2_API void BASM_OutOpRegMem(BASM_Context *ctx, int op, int reg,char *lbl, int breg, int ireg, int sc, long long disp);
BASM2_API void BASM_OutOpMemReg(BASM_Context *ctx, int op, int reg,char *lbl, int breg, int ireg, int sc, long long disp);
BASM2_API void BASM_OutOpMemImm(BASM_Context *ctx, int op, int w,char *lbl, int breg, int ireg, int sc, long long disp, long long imm, char *lbl2);
BASM2_API void BASM_OutOpRegRegImm(BASM_Context *ctx, int op, int r0, int r1,long long imm, char *lbl2);
BASM2_API void BASM_OutOpRegMemImm(BASM_Context *ctx, int op, int reg,char *lbl, int breg, int ireg, int sc, long long disp, long long imm, char *lbl2);
BASM2_API void BASM_OutOpMemRegImm(BASM_Context *ctx, int op, int reg,char *lbl, int breg, int ireg, int sc, long long disp, long long imm, char *lbl2);
BASM2_API void BASM_OutOpRegRegReg(BASM_Context *ctx, int op, int r0, int r1, int r2);
BASM2_API void BASM_OutOpRegMemReg(BASM_Context *ctx, int op, int reg,char *lbl, int breg, int ireg, int sc, long long disp, int reg2);
BASM2_API void BASM_OutOpMemRegReg(BASM_Context *ctx, int op, int reg,char *lbl, int breg, int ireg, int sc, long long disp, int reg2);
BASM2_API void BASM_OutOpGeneric1(BASM_Context *ctx, int op, int w,char *lbl, int breg, int ireg, int sc, long long disp);
BASM2_API void BASM_OutOpGeneric2(BASM_Context *ctx, int op, int w,char *lbl0, int breg0, int ireg0, int sc0, long long disp0, char *lbl1, int breg1, int ireg1, int sc1, long long disp1);
BASM2_API void BASM_OutOpGeneric3(BASM_Context *ctx, int op, int w,char *lbl0, int breg0, int ireg0, int sc0, long long disp0, char *lbl1, int breg1, int ireg1, int sc1, long long disp1, char *lbl2, int breg2, int ireg2, int sc2, long long disp2);
BASM2_API void BASM_EmitLabelPos(BASM_Context *ctx, char *name, int pos);
BASM2_API void BASM_EmitGotoPos(BASM_Context *ctx, char *name, int ty, int pos);
BASM2_API void BASM_EmitLabel(BASM_Context *ctx, char *name);
BASM2_API void BASM_EmitGoto(BASM_Context *ctx, char *name, int ty);
BASM2_API void BASM_EmitConst(BASM_Context *ctx, char *name, long long val);
BASM2_API int BASM_PredictPos(BASM_Context *ctx, char *name);
BASM2_API int BASM_PredictDisp(BASM_Context *ctx, char *name);
//AHSRC:asm/basm_parse.c
BASM2_API int basm_strdup_i(char *str);
BASM2_API char *basm_strtab_i(int i);
BASM2_API int basm_strildup_i(char *str);
BASM2_API void *basm_ralloc(int sz);
BASM2_API char *basm_rstrdup(char *str);
BASM2_API int basm_strisreal(char *str);
BASM2_API char *BASM_Parse_EatWhiteOnly(char *s);
BASM2_API char *BASM_Parse_EatWhite(char *s);
BASM2_API int BASM_Parse_IsWhiteOnly(char *s);
BASM2_API int BASM_Parse_IsLineBreak(char *se);
BASM2_API int BASM_Parse_OpChar(int c);
BASM2_API int BASM_Parse_PeekChar(char **str);
BASM2_API int BASM_Parse_ParseChar(char **str);
BASM2_API int BASM_Parse_EmitChar(char **str, int j);
BASM2_API char *BASM_Parse_Token(char *s, char *b, int *ty);
BASM2_API int BASM_ParseReg(BASM_Context *ctx, char *b);
BASM2_API int BASM_ParseReg(BASM_Context *ctx, char *b);
BASM2_API char *BASM_ParseArg(BASM_Context *ctx, char *str,int *breg, int *ireg, int *sc, long long *disp, char **lbl);
BASM2_API int BASM_OpOnlySingleP(int op);
BASM2_API char *BASM_ParseXMeta(BASM_Context *ctx, char *str,char *op, char *tag);
BASM2_API char *BASM_ParseSpecial(BASM_Context *ctx, char *str, char *op);
BASM2_API int BASM_LookupOpcode(BASM_Context *ctx, char *str);
BASM2_API int BASM_LookupOpcodeARMCC(BASM_Context *ctx, char *str);
BASM2_API char *BASM_ParseOpcode(BASM_Context *ctx, char *str);
BASM2_API char *BASM_ParseOpcode(BASM_Context *ctx, char *str);
BASM2_API char *BASM_ParseOpcodeList(BASM_Context *ctx, char *str);
//AHSRC:asm/basm_insns_x86.c
//AHSRC:asm/basm_insns_arm.c
//AHSRC:asm/basm_insns_thumb.c
//AHSRC:asm/basm_disasm.c
//AHSRC:asm/basm_coff.c
BASM2_API int BASM_COFF_StoreObjectBuf(BASM_Context *ctx,byte *buf, int msz);
BASM2_API byte *BASM_COFF_EncodeObject(BASM_Context *ctx, int *rsz);
