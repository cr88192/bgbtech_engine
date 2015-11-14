#include <bgbasm.h>

#ifndef BGBASMX_H
#define BGBASMX_H

#define BASMX_OP_NOP	0x010000
#define BASMX_OP_LABEL	0x010001
#define BASMX_OP_CONST	0x010002

typedef struct BASMX_OpArg_s BASMX_OpArg;
typedef struct BASMX_Opcode_s BASMX_Opcode;

struct BASMX_OpArg_s {
int breg;
int ireg;
int sc;
s64 disp;
char *lbl;
};

struct BASMX_Opcode_s {
int opnum, width, narg;
int seg, flag;
BASMX_OpArg **arg;
};


#include <bgbasm_auto_asmx.h>
#include <bgbasm_autoi_asmx.h>

#endif
