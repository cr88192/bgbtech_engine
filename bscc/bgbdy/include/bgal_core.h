#define BGAL_NULL	0	//end of prog string
#define BGAL_STOP	1	//stop execution and return
#define BGAL_NOP	2	//do nothing
#define BGAL_NOP1	3	//do nothing (resv)

#define BGAL_ADD	4
#define BGAL_SUB	5
#define BGAL_DIV	6
#define BGAL_MUL	7

#define BGAL_XOR	8
#define BGAL_MOD	9
#define BGAL_AND	10
#define BGAL_OR		11

#define BGAL_NOT	12
#define BGAL_NEG	13
#define BGAL_FLOOR	14
#define BGAL_CEIL	15

#define BGAL_SQR	16
#define BGAL_SSQR	17
#define BGAL_SSQRT	18
#define BGAL_SQRT	19

#define BGAL_SIN	20
#define BGAL_COS	21
#define BGAL_ACOS	22
#define BGAL_ASIN	23

#define BGAL_TAN	24
#define BGAL_ATAN	25
#define BGAL_ATAN2	26

#define BGAL_LPRED	27	//a b => 2a-b

#define BGAL_POP	28
#define BGAL_DUP	29
#define BGAL_EXCH	30
#define BGAL_LDVEC	31	//load from vector, oprand=offset

#define BGAL_CALL	32	//call a different string
#define BGAL_CALLSUB	33	//call a relative offset
#define BGAL_IFCALLSUB	34	//conditional, call an offset
#define BGAL_IFCALL	35	//conditional, call a different string
#define BGAL_IFNCALL	36	//conditional !, call a different string
#define BGAL_IFNCALLSUB	37	//conditional !, call an offset

#define BGAL_DIVSI	38	//small int
#define BGAL_ZLDSI	39	//do nothing (resv)
#define BGAL_LDPI	40	//PI
#define BGAL_LDN1	41	//-1

#define BGAL_IFNJMP	42	//conditional !, call an offset
#define BGAL_IFNTCALL	43	//conditional !, call a different string
#define BGAL_IFTCALL	44	//conditional, call a different string
#define BGAL_IFJMP	45	//conditional, call an offset
#define BGAL_JMP	46	//call a relative offset
#define BGAL_TCALL	47	//call a different string

#define BGAL_ZTCALL	48	//call a different string
#define BGAL_ZJMP	49	//call a relative offset
#define BGAL_ZIFJMP	50	//conditional, call an offset
#define BGAL_ZIFTCALL	51	//conditional, call a different string
#define BGAL_ZIFNTCALL	52	//conditional !, call a different string
#define BGAL_ZIFNJMP	53	//conditional !, call an offset

#define BGAL_LD1	54	//1
#define BGAL_LD0	55	//0
#define BGAL_LDSI	56	//small int
#define BGAL_MULSI	57	//small int

#define BGAL_ZIFNCSUB	58	//do nothing (resv)
#define BGAL_ZIFNCALL	59	//do nothing (resv)
#define BGAL_ZIFCALL	60	//do nothing (resv)
#define BGAL_ZIFCSUB	61	//do nothing (resv)
#define BGAL_ZCALLSUB	62	//do nothing (resv)
#define BGAL_ZCALL	63	//do nothing (resv)

#define BGAL_MUL2	64
#define BGAL_MUL3	65
#define BGAL_MUL4	66
#define BGAL_MUL8	67
#define BGAL_DIV8	68
#define BGAL_DIV4	69
#define BGAL_DIV3	70
#define BGAL_DIV2	71

#define BGAL_LD2	72	//2
#define BGAL_LD3	73	//3
#define BGAL_LD4	74	//4
#define BGAL_LD5	75	//5

typedef struct bgalContext_s bgalContext;
typedef struct bgalMember_s bgalMember;

struct bgalContext_s {
bgalMember *live;	//pool of live members
bgalMember *free;	//list of free members
bgalMember *member;	//list of all members
int sz_vec;		//size of vector component
int sz_tune;		//size of tuning array
int sz_prog;		//size of program array
int sz_bits;		//size of bits array

int n_keep;		//number of elements to keep per-generation
int n_breed;		//number of elements to breed
int n_gen;		//number of generations to run

int cnt_live;		//current size of live array

void **data;
int n_data, m_data;

bgalMember *(*alloc_member)(bgalContext *ctx);
void (*free_member)(bgalContext *ctx, bgalMember *skip);
bgalMember *(*pick_member)(bgalContext *ctx, bgalMember *skip);
bgalMember *(*breed)(bgalContext *ctx, bgalMember *obja, bgalMember *objb);
float (*test)(bgalContext *ctx, bgalMember *obj, void *data);
void (*step_test)(bgalContext *ctx, void *data);
void (*step_select)(bgalContext *ctx);
void (*step_breed)(bgalContext *ctx);
void (*step_status)(bgalContext *ctx);
void (*step)(bgalContext *ctx);
void (*setup)(bgalContext *ctx, void *data);
void (*evolve)(bgalContext *ctx, void *data);
};

struct bgalMember_s {
bgalMember *next;	//next in context
bgalMember *lnext;	//next in list
bgalMember *cnext;	//next in chain (for queries, ...)
bgalContext *ctx;

float weight;
float *vec;
float *vel;
float *tune;

byte *bits;		//bitmap
byte **prog;
};

typedef struct {
float stack[256];
int stackpos;

byte *rstack_ip[64];
byte *rstack_ips[64];
byte *rstack_ipe[64];
int rstackpos;

bgalMember *obj;
byte *ip, *ips, *ipe;
}BGAL_ICtx;
