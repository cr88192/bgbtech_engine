typedef int RILBC_String;
typedef int RILBC_StrIdx;
typedef int RILBC_Lists;
typedef int RILBC_TREF;

struct RILBC_Table_s {
RILBC_String name;
RILBC_StrIdx fields;
};

struct RILBC_Opcode_s {
RILBC_String name;
RILBC_String args;
RILBC_Lists args;
};

struct RILBC_Context_s {
int n_strings, m_strings;
char *strings;

int n_stridx, m_stridx;
int *stridx;

int n_lists, m_lists;
int *lists;

int n_tables, m_tables;
RILBC_Table **tables;

int n_opcodes, m_opcodes;
RILBC_Opcode **opcodes;
};
