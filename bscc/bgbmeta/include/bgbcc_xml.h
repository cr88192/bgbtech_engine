#ifndef BCCX_H
#define BCCX_H

#define BGBDY_TTY_NULL			0
#define BGBDY_TTY_SPECIAL		1
#define BGBDY_TTY_SYMBOL		2
#define BGBDY_TTY_STRING		3

typedef struct BCCX_Node_s BCCX_Node;
typedef struct BCCX_Attr_s BCCX_Attr;

typedef struct BCCX_PrintStream_s *BCCX_PrintStream;
typedef struct BCCX_ReadStream_s *BCCX_ReadStream;

typedef void *BCCX_ReadPos;

struct BCCX_Node_s {
BCCX_Node *next;
BCCX_Node *prev;
BCCX_Node *up;
BCCX_Node *down;

BCCX_Attr *attr;
BCCX_Node *down_end;

char *ns;
char *tag;
char *text;

BCCX_Node *hnext;		//hash next
// int type;
};

struct BCCX_Attr_s {
BCCX_Attr *next;
char *ns;
char *var;
char *val;
};

struct BCCX_PrintStream_s {
BCCX_Node *sarr;
BCCX_Node *marr;
int n_sarr;
int n_marr;

void *buf;
void (*prints_f)(void *buf, char *str);
int ind;
};

struct BCCX_ReadStream_s {
void *data;
void *(*get_pos_f)(BCCX_ReadStream strm);
void (*set_pos_f)(BCCX_ReadStream strm, void *pos);
int (*peek_char_f)(BCCX_ReadStream strm);
int (*read_char_f)(BCCX_ReadStream strm);
int (*peek_idx_f)(BCCX_ReadStream strm, int idx);
};

#endif
