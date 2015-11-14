typedef struct {
int fl;
unsigned int lstk;	//literal stack (bit mask)
char *cf_n;

dyt lit;		//function literal table
dyt args;		//function args
dyt local;		//local variables
byte *ip;		//current bytecoded op

dyt func;		//function being compiled
BSVM_SVMBlock *blk;	//block definition

int n_lbl;
byte *lbl[256];
char *lbln[256];
}BSVM_JitContext;
