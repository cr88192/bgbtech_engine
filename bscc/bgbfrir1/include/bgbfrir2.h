#ifndef BGBFRIR2_H
#define BGBFRIR2_H

#define BGBFR2_TWOCC(a, b)				((a)|((b)<<8))
#define BGBFR2_FOURCC(a, b, c, d)		((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define BGBFR2_TWOCC_I1					BGBFR2_TWOCC('I', '1')
#define BGBFR2_TWOCC_I2					BGBFR2_TWOCC('I', '2')
#define BGBFR2_TWOCC_I3					BGBFR2_TWOCC('I', '3')
#define BGBFR2_TWOCC_I4					BGBFR2_TWOCC('I', '4')
#define BGBFR2_TWOCC_I8					BGBFR2_TWOCC('I', '8')
#define BGBFR2_TWOCC_F4					BGBFR2_TWOCC('F', '4')
#define BGBFR2_TWOCC_F8					BGBFR2_TWOCC('F', '8')

#define BGBFR2_TWOCC_SI					BGBFR2_TWOCC('S', 'I')


typedef union FR2_Value_s				FR2_Value;
typedef struct FR2_OpcodeInfo_s		FR2_OpcodeInfo;
typedef struct FR2_PointerInfo_s		FR2_PointerInfo;

typedef struct FR2_StaticVarInfo_s		FR2_StaticVarInfo;
typedef struct FR2_ProgramObject_s		FR2_ProgramObject;
typedef struct FR2_Function_s			FR2_Function;

typedef struct FR2_RunFrame_s			FR2_RunFrame;
typedef struct FR2_RunCtx_s			FR2_RunCtx;
typedef struct FR2_EnvCtx_s			FR2_EnvCtx;
typedef struct FR2_IRunCtx_s			FR2_IRunCtx;
typedef struct FR2_IEnvCtx_s			FR2_IEnvCtx;

#include <fr2_base.h>

#endif
