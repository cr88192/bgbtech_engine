#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef BGGL3_H
#define BGGL3_H

#define BGGL3_TOKEN_NULL	0
#define BGGL3_TOKEN_INTEGER	1
#define BGGL3_TOKEN_FLOAT	2
#define BGGL3_TOKEN_SYMBOL	3
#define BGGL3_TOKEN_STRING	4
#define BGGL3_TOKEN_BRACE	5
#define BGGL3_TOKEN_OPERATOR	6
#define BGGL3_TOKEN_MISC	7

typedef unsigned char byte;
typedef void *elem;

typedef struct BGGL3_Binding_s BGGL3_Binding;
typedef struct BGGL3_Func_s BGGL3_Func;
typedef struct BGGL3_Builtin_s BGGL3_Builtin;
typedef struct BGGL3_State_s BGGL3_State;

struct BGGL3_Binding_s {
BGGL3_Binding *next;
elem var;
elem val;
};

struct BGGL3_Func_s {
BGGL3_Binding *env;
elem args;
elem body;
};

struct BGGL3_Builtin_s {
elem (*func)(BGGL3_State *ctx, elem args);
int n_args;
char *name;
char *desc;
};

struct BGGL3_State_s {
BGGL3_Binding *env;
BGGL3_Binding *denv;
};

#include "bggl3_auto.h"

#endif
