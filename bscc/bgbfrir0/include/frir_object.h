struct FRIR_Object_MetaKey_s {
int key;
int val;
};

struct FRIR_Object_MetaNode_s {
int name;
int type;
int parent;

int key_idx;
int key_cnt;
int child_idx;
int child_cnt;
};

struct FRIR_Object_DataBlob_s {
int type;
int size;
void *data;
};

struct FRIR_Object_ByteCode_s {
int node;
int size;
byte *data;
};

struct FRIR_ObjectContext_s {
char *strtab;
int sz_strtab;
int strtabe;
//byte *datablob;
//byte *bytecode;
//byte *constdata;
//int *constindex;

FRIR_Object_DataBlob **datablobs;
FRIR_Object_ByteCode **bytecode;
FRIR_Object_DataBlob **constdata;

int *metaindex;
FRIR_Object_MetaKey **metakeys;
FRIR_Object_MetaNode **metanodes;

int n_datablobs, m_datablobs;
int n_bytecode, m_bytecode;
int n_constdata, m_constsata;
int n_metakeys, m_metakeys;
int n_metanodes, m_metanodes;
};
