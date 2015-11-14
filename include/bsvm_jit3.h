#define BSVM_THJIT_CTXFL_LASTTHOP	1
#define BSVM_THJIT_CTXFL_THOPEAX	2

#define BSVM_THJIT_OPFL_SAFETYCHECK	1


#define BSVM_THOP_OFFS(k)	offsetof(BSVM_ThreadOp, k)
#define BSVM_CTX_OFFS(k)	offsetof(BSVM_SVMState, k)
#define BSVM_THTR_OFFS(k)	offsetof(BSVM_ThreadTrace, k)

#define BSVM_ARRAY_OFFS(k)	offsetof(BGBDYC_Array, k)


typedef struct BSVM_ThreadJitContext_s BSVM_ThreadJitContext;

struct BSVM_ThreadJitContext_s {
BSVM_SVMState *vmctx;
BSVM_ThreadTrace *trace;
BSVM_ThreadOp *thop;
int flags;
};
