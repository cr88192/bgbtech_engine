#define BGBDYC_SLOT_ABSTRACT	0
#define BGBDYC_SLOT_DIRECT		1
#define BGBDYC_SLOT_INDIRECT	2
#define BGBDYC_SLOT_STATIC		3
#define BGBDYC_SLOT_INTERFACE	4
#define BGBDYC_SLOT_DYNAMIC		5	//method: idx gives slot
#define BGBDYC_SLOT_PROTOTYPE	6	//prototype

#define BGBDYC_FLAG_PUBLIC			0x0001
#define BGBDYC_FLAG_PRIVATE			0x0002
#define BGBDYC_FLAG_PROTECTED		0x0004
#define BGBDYC_FLAG_STATIC			0x0008
#define BGBDYC_FLAG_FINAL			0x0010
#define BGBDYC_FLAG_SUPER			0x0020
#define BGBDYC_FLAG_SYNCHRONIZED	0x0020
#define BGBDYC_FLAG_VOLATILE		0x0040
#define BGBDYC_FLAG_TRANSIENT		0x0080
#define BGBDYC_FLAG_NATIVE			0x0100
#define BGBDYC_FLAG_INTERFACE		0x0200
#define BGBDYC_FLAG_ABSTRACT		0x0400
#define BGBDYC_FLAG_STRICT			0x0800

#define BGBDYC_FLAG_BYREF			0x1000
#define BGBDYC_FLAG_DELEGATE		0x2000
#define BGBDYC_FLAG_GETTER			0x4000
#define BGBDYC_FLAG_SETTER			0x8000

#define BGBDYC_FLAG_SIGNED			0x00010000
#define BGBDYC_FLAG_UNSIGNED		0x00020000
#define BGBDYC_FLAG_EXTERN			0x00040000
#define BGBDYC_FLAG_CONST			0x00080000
#define BGBDYC_FLAG_INLINE			0x00100000
#define BGBDYC_FLAG_EVENT			0x00200000
#define BGBDYC_FLAG_WIDE			0x00400000
#define BGBDYC_FLAG_THREAD			0x00800000
#define BGBDYC_FLAG_XCALL			0x01000000
#define BGBDYC_FLAG_STDCALL			0x02000000	//'__stdcall'
#define BGBDYC_FLAG_CDECL			0x04000000	//'__cdecl'
#define BGBDYC_FLAG_PROXY			0x08000000	//'__proxy'

#define BGBDYC_FLAG_LTLENDIAN		0x10000000	//'__littleendian'
#define BGBDYC_FLAG_BIGENDIAN		0x20000000	//'__bigendian'

#define BGBDYC_FLAG_WIN64			BGBDYC_FLAG_STDCALL
#define BGBDYC_FLAG_AMD64			BGBDYC_FLAG_CDECL
#define BGBDYC_FLAG_SEALED			BGBDYC_FLAG_FINAL
#define BGBDYC_FLAG_READONLY		BGBDYC_FLAG_STRICT
#define BGBDYC_FLAG_INTERNAL		BGBDYC_FLAG_SIGNED
#define BGBDYC_FLAG_NEW				BGBDYC_FLAG_CONST
#define BGBDYC_FLAG_OVERRIDE		BGBDYC_FLAG_STRICT
#define BGBDYC_FLAG_CONST2			BGBDYC_FLAG_FINAL
#define BGBDYC_FLAG_PACKED			BGBDYC_FLAG_SIGNED
#define BGBDYC_FLAG_GC				BGBDYC_FLAG_XCALL
#define BGBDYC_FLAG_NOGC			BGBDYC_FLAG_NATIVE
#define BGBDYC_FLAG_UNSAFE			BGBDYC_FLAG_NATIVE
#define BGBDYC_FLAG_VIRTUAL			BGBDYC_FLAG_VOLATILE
#define BGBDYC_FLAG_DYNAMIC			BGBDYC_FLAG_XCALL

#define BGBDYC_FLAG_STRUCT			BGBDYC_FLAG_CONST

#define BGBDYC_FLAG_DLLIMPORT		0x0000000100000000LL
#define BGBDYC_FLAG_DLLEXPORT		0x0000000200000000LL
// #define BGBDYC_FLAG_THREAD		0x0000000400000000LL
#define BGBDYC_FLAG_ASYNC			0x0000000800000000LL
#define BGBDYC_FLAG_THISCALL		0x0000001000000000LL
#define BGBDYC_FLAG_TYPEDEF			0x0000002000000000LL
#define BGBDYC_FLAG_SETUID			0x0000004000000000LL
#define BGBDYC_FLAG_SETGID			0x0000008000000000LL

#define BGBDYC_FLAG_ENTMASK			0x3F00000000000000LL
#define BGBDYC_FLAG_ENTDEFAULT		0x0000000000000000LL
#define BGBDYC_FLAG_ENTCLASS		0x0100000000000000LL
#define BGBDYC_FLAG_ENTIFACE		0x0200000000000000LL
#define BGBDYC_FLAG_ENTSTRUCT		0x0300000000000000LL
#define BGBDYC_FLAG_ENTUNION		0x0400000000000000LL
#define BGBDYC_FLAG_ENTPKG			0x0500000000000000LL
#define BGBDYC_FLAG_ENTPKGFIELD		0x0600000000000000LL

#define BGBDYC_ARCH_DEFAULT	0	//any arch
#define BGBDYC_ARCH_X86		1	//32-bit x86
#define BGBDYC_ARCH_X64		2	//64-bit x86 / x86-64
#define BGBDYC_ARCH_ARM		3	//ARM
#define BGBDYC_ARCH_PPC		4	//PPC
#define BGBDYC_ARCH_PPC64	5	//PPC64

//binary operators
#define BGBDY_OPR_ADD		0
#define BGBDY_OPR_SUB		1
#define BGBDY_OPR_MUL		2
#define BGBDY_OPR_DIV		3
#define BGBDY_OPR_MOD		4
#define BGBDY_OPR_AND		5
#define BGBDY_OPR_OR		6
#define BGBDY_OPR_XOR		7
#define BGBDY_OPR_SHL		8
#define BGBDY_OPR_SHR		9
#define BGBDY_OPR_EQ		10
#define BGBDY_OPR_NEQ		11
// #define BGBDY_OPR_EQV		12
// #define BGBDY_OPR_NEQV		13
#define BGBDY_OPR_L			14
#define BGBDY_OPR_G			15
#define BGBDY_OPR_LE		16
#define BGBDY_OPR_GE		17
#define BGBDY_OPR_IDIV		18
#define BGBDY_OPR_DADD		19
#define BGBDY_OPR_DSUB		20
#define BGBDY_OPR_DMUL		21
#define BGBDY_OPR_DDIV		22
#define BGBDY_OPR_DMOD		23
#define BGBDY_OPR_DAND		24
#define BGBDY_OPR_DOR		25
#define BGBDY_OPR_DXOR		26
#define BGBDY_OPR_DIDIV		27
#define BGBDY_OPR_EXP		28
#define BGBDY_OPR_DSHL		29
#define BGBDY_OPR_DSHR		30
#define BGBDY_OPR_SHRR		31
#define BGBDY_OPR_ATAN2		32

//unary operators
#define BGBDY_OPR_NEG		64
#define BGBDY_OPR_NOT		65
#define BGBDY_OPR_LNOT		66
#define BGBDY_OPR_POS		67
#define BGBDY_OPR_UMUL		68
#define BGBDY_OPR_UDIV		69
#define BGBDY_OPR_UIDIV		70
#define BGBDY_OPR_UMOD		71
#define BGBDY_OPR_UAND		72
#define BGBDY_OPR_UOR		73
#define BGBDY_OPR_UXOR		74
#define BGBDY_OPR_ULAND		75
#define BGBDY_OPR_ULOR		76
#define BGBDY_OPR_ULXOR		77
#define BGBDY_OPR_INC		78
#define BGBDY_OPR_DEC		79
#define BGBDY_OPR_SIN		80
#define BGBDY_OPR_COS		81
#define BGBDY_OPR_TAN		82
#define BGBDY_OPR_ASIN		83
#define BGBDY_OPR_ACOS		84
#define BGBDY_OPR_ATAN		85
#define BGBDY_OPR_SQR		86
#define BGBDY_OPR_SQRT		87


#define BGBGC_RAWFUNC_BASIC		0
#define BGBGC_RAWFUNC_DATA		1
#define BGBGC_RAWFUNC_SELF		2
#define BGBGC_RAWFUNC_SELFDATA	3
#define BGBGC_RAWFUNC_CLASS		4
#define BGBGC_RAWFUNC_CLASSDATA	5

#define BGBDYC_CLASS_BASIC		0
#define BGBDYC_CLASS_INTERFACE	1
#define BGBDYC_CLASS_STRUCT		2

#define BGBDYC_ARRAY_INDEXER	1
#define BGBDYC_ARRAY_BYVALUE	2

#define BGBDY_ACCESS_EXEC_ANY		0x001
#define BGBDY_ACCESS_WRITE_ANY		0x002
#define BGBDY_ACCESS_READ_ANY		0x004
#define BGBDY_ACCESS_SPECIAL_ANY	0x008
#define BGBDY_ACCESS_EXEC_GRP		0x010
#define BGBDY_ACCESS_WRITE_GRP		0x020
#define BGBDY_ACCESS_READ_GRP		0x040
#define BGBDY_ACCESS_SPECIAL_GRP	0x080
#define BGBDY_ACCESS_EXEC_OWN		0x100
#define BGBDY_ACCESS_WRITE_OWN		0x200
#define BGBDY_ACCESS_READ_OWN		0x400
#define BGBDY_ACCESS_SPECIAL_OWN	0x800

#define BGBDY_ACCESS_WRITEEXEC_ANY		0x003
#define BGBDY_ACCESS_READEXEC_ANY		0x005
#define BGBDY_ACCESS_READWRITE_ANY		0x006
#define BGBDY_ACCESS_READWRITE_EXEC_ANY	0x007

#define BGBDY_ACCESS_DEFAULT		0x00000754
#define BGBDY_ACCESS_ROOTONLY		0x00000700
#define BGBDY_ACCESS_USERONLY		0x00000700

#define BGBDY_ACCESS_ROOTDEFAULT	0x00000000

#define BGBDY_ACCESS_MODEMASK		0x00000FFF
#define BGBDY_ACCESS_UIDMASK		0x003FF000
#define BGBDY_ACCESS_GIDMASK		0xFFC00000

#define BGBDY_ACCESS_GID_ROOT		0x00000000
#define BGBDY_ACCESS_GID_CLIENT		0x00400000
#define BGBDY_ACCESS_GID_SERVER		0x00800000
#define BGBDY_ACCESS_GID_GUEST		0x00C00000

#define BGBDY_ACCESS_UID_ROOT		0x00000000
#define BGBDY_ACCESS_UID_CLIENT		0x00001000
#define BGBDY_ACCESS_UID_SERVER		0x00002000
#define BGBDY_ACCESS_UID_GUEST		0x00003000

typedef struct BGBDY_ObjectStream_s BGBDY_ObjectStream;
typedef struct BGBDY_ObjectLump_s BGBDY_ObjectLump;
typedef BGBDY_ObjectStream *dysObjectStream;

typedef struct BGBDY_ObjVTab_s BGBDY_ObjVTab;

typedef u32 dyAccessMode;

#if 1
typedef struct BGBGC_ClassInfo_s BGBGC_ClassInfo;
typedef struct BGBGC_ClassSlot_s BGBGC_ClassSlot;
typedef struct BGBGC_ClassMinf_s BGBGC_ClassMinf;
typedef struct BGBGC_ClassBase_s BGBGC_ClassBase;
typedef struct BGBGC_ClassMeth_s BGBGC_ClassMeth;

typedef struct BGBGC_ClassVinf_s BGBGC_ClassVinf;
typedef struct BGBGC_ObjProto_s BGBGC_ObjProto;

typedef struct BGBGC_Namespace_s BGBGC_Namespace;
typedef struct BGBGC_NamespaceSlot_s BGBGC_NamespaceSlot;
#endif

#if 0
typedef BGBGC_ClassInfo *dycClass;
typedef BGBGC_ClassSlot *dycSlot;
typedef BGBGC_ClassMinf *dycMethod;
typedef BGBGC_ClassBase *dycObject;
#endif

typedef BGBGC_ObjProto *dycObjProto;
typedef BGBGC_ClassVinf *dycClassV;

//typedef struct BGBDYC_Array_s *dycArray;
//typedef struct BGBDYC_ArrayN_s *dycArrayN;

//typedef struct BGBDYC_TciTable_s BGBDYC_TciTable;
//typedef struct BGBDYC_RciTable_s BGBDYC_RciTable;
//typedef struct BGBDYC_RciDB_s BGBDYC_RciDB;

typedef struct BGBDYC_Array_s BGBDYC_Array;
typedef struct BGBDYC_ArrayN_s BGBDYC_ArrayN;

typedef struct BGBDYC_MetaNode_s BGBDYC_MetaNode;
typedef struct BGBDYC_MetaLeaf_s BGBDYC_MetaLeaf;
typedef struct BGBDYC_MetaBiNode_s BGBDYC_MetaBiNode;

typedef struct BGBDYC_RDB_s BGBDYC_RDB;
typedef struct BGBDYC_RDBTable_s BGBDYC_RDBTable;

typedef void *(*BGBDYC_ApplyV_t)(void *obj, dyt mth, char *sig, va_list args);
typedef void *(*BGBDYC_ApplyU_t)(void *obj, dyt mth, char *sig, void **args, int szargs);


struct BGBDY_ObjVTab_s {
BGBDY_ObjVTab *next;
char *name;
char *classAlias;
int size, idx;

char **vtab_name;
void **vtab_func;
int n_vtab, m_vtab;

void *(*get_slot)	(void *p, char *s);
void *(*set_slot)	(void *p, char *s, void *v);
char *(*nxt_slot)	(void *p, char *s);
void *(*call_method)	(void *p, char *s, void **a, int n);
void *(*apply)		(void *p, void **a, int n);

void *(*try_set_slot)	(void *p, char *s, void *v);
void *(*get_del)	(void *p, char *s);
void *(*set_del)	(void *p, char *s, void *v);
char *(*nxt_del)	(void *p, char *s);

void *(*applyobj)	(void *p, void *o, void **a, int n);

char *(*tostring)	(void *p);

void (*encodeBinary)	(dysObjectStream ctx, dyt val);
dyt (*decodeBinary)		(dysObjectStream ctx);

//value types
dyt (*copyValue)	(dyt p);
int (*dropValue)	(dyt p);
};


typedef struct {
int nargs;
union {
dyt (*fa)(dyt *args, int nargs);
dyt (*fad)(dyt data, dyt *args, int nargs);
dyt (*f0)();
dyt (*f1)(dyt);
dyt (*f2)(dyt,dyt);
dyt (*f3)(dyt,dyt,dyt);
dyt (*f4)(dyt,dyt,dyt,dyt);
dyt (*f5)(dyt,dyt,dyt,dyt,dyt);
dyt (*f6)(dyt,dyt,dyt,dyt,dyt,dyt);
dyt (*f7)(dyt,dyt,dyt,dyt,dyt,dyt,dyt);
dyt (*f8)(dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt);
}fcn;
dyt data;
}BGBGC_Func;

typedef struct {
int nargs;
union {
dyt (*fa)(dyt self, dyt *args, int nargs);
dyt (*fad)(dyt self, dyt data, dyt *args, int nargs);
dyt (*f0)(dyt);
dyt (*f1)(dyt,dyt);
dyt (*f2)(dyt,dyt,dyt);
dyt (*f3)(dyt,dyt,dyt,dyt);
dyt (*f4)(dyt,dyt,dyt,dyt,dyt);
dyt (*f5)(dyt,dyt,dyt,dyt,dyt,dyt);
dyt (*f6)(dyt,dyt,dyt,dyt,dyt,dyt,dyt);
dyt (*f7)(dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt);
dyt (*f8)(dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt,dyt);
}fcn;
dyt data;
}BGBGC_Method;

typedef struct {
dycSlot *vari;
char **var;
dytf *val;
int cnt;

char **dvar;
dyt *dval;
int dcnt;

dyAccessMode access;	//object access
}BGBGC_Object;


typedef struct {
char *sig;
char *sig2;
char *esig;
void *fcn;
void *data;
void *thunk;	//specialized application thunk
int type;
#ifdef BGBDY_USEGENAPPLY
int sig2id;		//GenApply ID number
#endif
}BGBGC_RawFunc;

typedef struct {
char *name;
char *altname;
}BGBGC_Alias;


typedef struct BGBGC_StubMethod_s BGBGC_StubMethod;

struct BGBGC_StubMethod_s {
int nargs;
char *sig;
dyt method;
dyt data;
void *(*fcn)(BGBGC_StubMethod *inf, dyt self, char *sig,
	void **args, int szargs);
};

typedef struct BGBDY_Operator_s BGBDY_Operator;

struct BGBDY_Operator_s {
BGBDY_Operator *next;
int opr;
char *lty;
char *rty;
dyt (*fcn)(dyt a, dyt b);
dyt (*ufcn)(dyt a);
};


struct BGBGC_ClassSlot_s {
BGBGC_ClassInfo *info;	//owner class info
char *name;
char *sig;
char *qname;	//qualified name ('<class>:<name>')

void *val;	//value (static slots)
void *val_[4];	//if value is inline

int hash;	//hash value
int qhash;	//qualified name hash

int offs;	//offset in object
int size;	//size in bytes

short type;	//slot lookup type
short num;	//slot number
s64 flags;	//slot flags
dyAccessMode access;	//access control
};

struct BGBGC_ClassMinf_s {
BGBGC_ClassInfo *info;	//owner class info

char *name;
char *sig;
char *qname;	//qualified name ('<class>:<name>')

int hash;	//hash value
int qhash;	//qualified name hash

short type;	//method lookup type
short num;	//method number in object
short idx;	//vtable index, -1 for iface methods
s64 flags;	//special flags
dyAccessMode access;	//access mode
// dyt fcn;
};

struct BGBGC_ClassMeth_s {
BGBGC_ClassInfo *info;	//owner class info
dyt fcn;

BGBDYC_ApplyU_t ApplyU;
BGBDYC_ApplyV_t ApplyV;
};

struct BGBGC_ClassVinf_s {
BGBGC_ClassInfo *info;		//owner class
short *vers;			//superclass versions

int start;			//offset of first slot (excludes superclass)
int length;			//length of class (approx start of subclasses)

int s_minf;			//start method index
int e_minf;			//end method index

int **slot_offs;		//indirect slot offset
int **minf_offs;		//indirect vtab offset

BGBGC_ClassMeth **vtab;		//method table
int sz_vtab;

void *idata;			//initial object state
int sz_idata;

//Multiple Inheritence
BGBGC_ClassMeth ***mi_vtab;	//MI vtable
};

struct BGBGC_ClassInfo_s {
//Class management data

BGBGC_ClassInfo *next;		//next (global class list)
BGBGC_ClassInfo *snext;		//next (superclass)
BGBGC_ClassInfo *super;		//superclass
BGBGC_ClassInfo *first;		//first subclass

BGBGC_Namespace *ns;		//owning namespace
BGBGC_ClassInfo *nsnext;	//next in namespace


char *sname;	//short name
char *qname;	//qualified name
int qhash;	//qualified name hash

short ver;		//current class version
char is_mi;		//uses multiple inheritence
char is_iface;	//is an interface
char is_struct;

s64 flags;		//class flags

dyt own_data;	//owner data


//Class Structure

BGBGC_ClassSlot **slot;		//instance slots
int n_slot, m_slot;		//num and max instance slots

BGBGC_ClassMinf **minf;		//instance slots
int n_minf, m_minf;		//num and max instance slots

BGBGC_ClassVinf **vinf;		//version info
int n_vinf, m_vinf;		//num and max vinfos

BGBGC_ClassInfo **iface;	//implemented interfaces
int n_iface, m_iface;		//num and max interfaces

//Multiple Inheritence

BGBGC_ClassInfo **misup;	//MI super (at point of inheritence)
int n_misup;			//number of MI supers

BGBGC_ClassInfo **mi_class;	//MI class
int n_mi, m_mi;			//number of MI classes (0==SI)
};

struct BGBGC_ClassBase_s {
BGBGC_ClassInfo *info;	//class-info for object
BGBGC_ClassVinf *vinf;	//class-version info for object
byte *data;
//short *vers;
byte **mi_data;		//multi-data
BGBGC_ObjProto *proto;	//prototype object
dyAccessMode access;	//object access mode
};

struct BGBGC_ObjProto_s {
BGBGC_ClassSlot **slot;
void **sval;
int n_slot, m_slot;

BGBGC_ClassMinf **minf;
dyt *mval;
int n_minf, m_minf;

//char **dvar;
//dyt *dval;
//int dcnt;
};


struct BGBGC_NamespaceSlot_s {
BGBGC_NamespaceSlot *next;
BGBGC_Namespace *ns;	//owning namespace

char *name;
char *sig;

void *val;	//pointer to value
void *val_i[2];	//if value is inline

int flags;	//slot flags
};

struct BGBGC_Namespace_s {
BGBGC_Namespace *next;
BGBGC_Namespace *first;
BGBGC_Namespace *up;

BGBGC_NamespaceSlot *slot;
BGBGC_NamespaceSlot *func;
BGBGC_ClassInfo *info;	//namespece class

char *name;
char *qname;
};

struct BGBDYC_Array_s {
void *data;		//array data
char *sig;		//array signature
int cnt;		//element count
int offs;		//base offset
int step;		//element step
int flags;		//array flags
};

struct BGBDYC_ArrayN_s {
void *data;	//payload
char *sig;	//sig string
int *psz;	//size (per-dimension)
int *offs;	//offset (per-dimension)
int ord;	//array order
int step;	//item size
int cnt;	//total count
};

#if 0
struct BGBDYC_TciTable_s {
dyt *lit;
byte **tab;
int nlit, mlit;
int ntab, mtab;
};

struct BGBDYC_RciTable_s {
BGBDYC_RciTable *next;
char *name;
char **col;
dyt *row;
byte *map;
int ncol;
int nrow, mrow;
};

struct BGBDYC_RciDB_s {
BGBDYC_RciTable *table;
};
#endif

#if 0
struct BGBDYC_MetaNode_s {
BGBDYC_MetaNode *next;		//next node in linear node list
BGBDYC_MetaNode *prev;		//prev node in linear node list

BGBDYC_MetaNode *first;		//first sub-node in linear list
BGBDYC_MetaNode *owner;		//super-node

BGBDYC_MetaNode *biroot;	//binary-root of sub-nodes
BGBDYC_MetaNode *bilnode;	//binary-left node
BGBDYC_MetaNode *birnode;	//binary-right node
int bidepth;			//binary tree depth

BGBDYC_MetaLeaf *leaf;		//leaf-key value
char *name;			//node name
};
#endif

#if 0
struct BGBDYC_MetaNode_s {
BGBDYC_MetaNode *next;		//next node in linear node list
BGBDYC_MetaNode *first;		//first sub-node in linear list
BGBDYC_MetaLeaf *leaf;		//leaf-key value
char *name;			//node name
};
#endif

#if 1
struct BGBDYC_MetaNode_s {
BGBDYC_MetaNode *next;		//next node in list
BGBDYC_MetaNode *prev;		//prev node in list
BGBDYC_MetaNode *first;		//first sub-node in list
BGBDYC_MetaNode *owner;		//owner node

BGBDYC_MetaBiNode *biroot;	//binary-root of sub-nodes

BGBDYC_MetaLeaf *leaf;		//leaf-key value
char *name;			//node name
};

struct BGBDYC_MetaBiNode_s {
BGBDYC_MetaBiNode *lnode;	//left sub-node
BGBDYC_MetaBiNode *rnode;	//right sub-node
BGBDYC_MetaNode *mid;		//node item
int depth;			//current tree depth
};
#endif

struct BGBDYC_MetaLeaf_s {
BGBDYC_MetaLeaf *next;
char *key;
char *val;
byte pf;
byte idx;
};

struct BGBDYC_RDB_s {
BGBDYC_RDB *next;
char *name;
void *db;	//actual database
int cnt;
};

struct BGBDYC_RDBTable_s {
BGBDYC_RDB *db;
char **cols;
char **rows;
int nrow, ncol;
};

struct BGBDY_ObjectLump_s {
char *type;
int ty_idx;
int size;
byte *data;
dyt value;

byte *ip, *ipe;
};

struct BGBDY_ObjectStream_s {
BGBDY_ObjectLump **arr;
int n_arr;
int m_arr;
BGBDY_ObjectLump *imglump;	//image lump
BGBDY_ObjectLump *lmp;	//current lump
char *magic;
};
