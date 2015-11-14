#ifdef _MSC_VER
#pragma warning(disable:4996)
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef BGBASM_ASM_H
#define BGBASM_ASM_H

#include <bgbasm_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <bgbasm_cpu.h>
#include <bgbasm_obj.h>

#include <bgbasm_x86ops.h>
#include <bgbasm_armops.h>
#include <bgbasm_thumbops.h>

#include <bgbasm_auto_asm.h>
#include <bgbasm_auto2_asm.h>
#include <bgbasm_autoi_asm.h>

#ifdef __cplusplus
}
#endif

#endif
