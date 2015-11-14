#define BGBFR2_TOKEN_NAME			1
#define BGBFR2_TOKEN_NUMBER			2
#define BGBFR2_TOKEN_STRING			3
#define BGBFR2_TOKEN_CHARSTRING		4
#define BGBFR2_TOKEN_SEPERATOR		5
#define BGBFR2_TOKEN_OPERATOR		6
#define BGBFR2_TOKEN_BRACE			7
#define BGBFR2_TOKEN_MISC			8
#define BGBFR2_TOKEN_NULL			9
#define BGBFR2_TOKEN_INT			10
#define BGBFR2_TOKEN_FLOAT			11
#define BGBFR2_TOKEN_STRING_OVF		12	//overlong string


typedef struct FR2A_VarInfo_s		FR2A_VarInfo;
typedef struct FR2A_Function_s		FR2A_Function;
typedef struct FR2A_Object_s		FR2A_Object;

struct FR2A_VarInfo_s {
char *name;
char *sig;
char *flags;
int val;
};

struct FR2A_Function_s {
char *name;
char *sig;

char *arg_name[32];
char *arg_sig[32];
int n_arg;

char *var_name[64];
char *var_sig[64];
int var_val[64];
int n_var;

byte *text;
byte *ct, *cte;

char *lbl_name[256];
int lbl_offs[256];
int n_lbl;

char *jmp_name[256];
int jmp_offs[256];
int jmp_tag[256];
int n_jmp;

};

struct FR2A_Object_s {
FR2A_VarInfo **var;
FR2A_Function **func;
int n_var, m_var;
int n_func, m_func;

char *strtab_buf[16];
char *strtab_end[16];
char *str_s, *str_e, *str_c;
int n_strtab;

byte *blobtab;
int sz_blobtab, msz_blobtab;

int *ctab_tag;
FR2_Value *ctab_val;
int n_ctab, m_ctab;

FR2A_Function *cur_func;
};
