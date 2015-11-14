#define BGBDY_TTY_NULL			0
#define BGBDY_TTY_SPECIAL		1
#define BGBDY_TTY_SYMBOL		2
#define BGBDY_TTY_STRING		3

typedef struct dyxNode_s dyxNode;
typedef struct dyxAttr_s dyxAttr;

typedef struct dysPrintStream_s *dysPrintStream;
typedef struct dysReadStream_s *dysReadStream;

typedef void *dysReadPos;

struct dyxNode_s {
dyxNode *next;
dyxNode *prev;
dyxNode *up;
dyxNode *down;

dyxAttr *attr;

char *ns;
char *tag;
char *text;
dyt dyval;
};

struct dyxAttr_s {
dyxAttr *next;
char *ns;
char *var;
char *val;
double dval;
dyt dyval;
};

struct dysPrintStream_s {
dyt *sarr;
dyt *marr;
int n_sarr;
int n_marr;

void *buf;
void (*prints_f)(void *buf, char *str);
int ind;
};

struct dysReadStream_s {
void *data;
void *(*get_pos_f)(dysReadStream strm);
void (*set_pos_f)(dysReadStream strm, void *pos);
int (*peek_char_f)(dysReadStream strm);
int (*read_char_f)(dysReadStream strm);
int (*peek_idx_f)(dysReadStream strm, int idx);
};
