#include <bgbgc.h>
#include <bgbdy.h>

#ifndef BGBSVM_H
#define BGBSVM_H

#define BSVM_THREADCODE

#define BSVM_USE_BVT
#define BSVM_VARSTACK


#define BSVM_OPR_ADD	0
#define BSVM_OPR_SUB	1
#define BSVM_OPR_MUL	2
#define BSVM_OPR_DIV	3
#define BSVM_OPR_MOD	4
#define BSVM_OPR_AND	5
#define BSVM_OPR_OR		6
#define BSVM_OPR_XOR	7
#define BSVM_OPR_SHL	8
#define BSVM_OPR_SHR	9
#define BSVM_OPR_EQ		10
#define BSVM_OPR_EQV	11
#define BSVM_OPR_NEQ	12
#define BSVM_OPR_NEQV	13
#define BSVM_OPR_L		14
#define BSVM_OPR_G		15
#define BSVM_OPR_LE		16
#define BSVM_OPR_GE		17
#define BSVM_OPR_IDIV	18
#define BSVM_OPR_DADD	19
#define BSVM_OPR_DSUB	20
#define BSVM_OPR_DMUL	21
#define BSVM_OPR_DDIV	22
#define BSVM_OPR_DMOD	23
#define BSVM_OPR_DAND	24
#define BSVM_OPR_DOR	25
#define BSVM_OPR_DXOR	26
#define BSVM_OPR_DIDIV	27
#define BSVM_OPR_EXP	28
#define BSVM_OPR_SHRR	29
#define BSVM_OPR_CONS	30
#define BSVM_OPR_ATAN2	31


#define BSVM_OPR_NEG		0
#define BSVM_OPR_NOT		1
#define BSVM_OPR_LNOT		2
#define BSVM_OPR_POS		3

#define BSVM_OPR_UMUL		4
#define BSVM_OPR_UDIV		5
#define BSVM_OPR_UIDIV		6
#define BSVM_OPR_UMOD		7
#define BSVM_OPR_UAND		8
#define BSVM_OPR_UOR		9
#define BSVM_OPR_UXOR		10
#define BSVM_OPR_ULAND		11
#define BSVM_OPR_ULOR		12
#define BSVM_OPR_ULXOR		13

#define BSVM_OPR_INC		14
#define BSVM_OPR_DEC		15

#define BSVM_OPR_DEFER		16
#define BSVM_OPR_CAR		17
#define BSVM_OPR_CDR		18
#define BSVM_OPR_COPYVALUE	19
#define BSVM_OPR_DROPVALUE	20
#define BSVM_OPR_DELETE		21
#define BSVM_OPR_CLONE		22

#define BSVM_OPR_SIN		23
#define BSVM_OPR_COS		24
#define BSVM_OPR_TAN		25
#define BSVM_OPR_ASIN		26
#define BSVM_OPR_ACOS		27
#define BSVM_OPR_ATAN		28
#define BSVM_OPR_SQR		29
#define BSVM_OPR_SQRT		30

#include <bsvm_conf.h>

#ifndef BSPKG2C_API
#define BSPKG2C_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <bsvm_base.h>
#include <bsvm_svm.h>
#include <bsvm_ops.h>
#include <bsvm_jit.h>
#include <bsvm_jit2.h>
#include <bsvm_jit3.h>

#include <bsvm_bvt.h>

#include <bs2asm.h>

#include <bsvm_auto.h>

#ifdef __cplusplus
}
#endif

#endif
