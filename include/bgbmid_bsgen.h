typedef struct BSG_Samp_s BSG_Samp;
typedef struct BSG_DictEnt_s BSG_DictEnt;
typedef struct BSG_Voice_s BSG_Voice;
typedef struct BSG_State_s BSG_State;
typedef struct BSG_PackState_s BSG_PackState;

struct BSG_Samp_s {
BSG_Samp *next;		//next
char *name;

int len;
short *buf;
};

struct BSG_DictEnt_s {
BSG_DictEnt *next;
char *name;

BSG_Samp *samp;
char **pat;
char *tpat[4];
};

struct BSG_Voice_s {
BSG_Voice *next;
BSG_Samp *samp[256];
BSG_DictEnt *dict[256];
BSG_DictEnt *phon;

BSG_Voice *parent;
char *name;
char *base;
};

struct BSG_State_s {
BSG_Voice *voice;
int tone;
char **wordstack;
int wordstackpos;

short *obuf;
int obufpos, obufsz;

short *obuf1;
int obuf1pos, obuf1sz;

int (*out_cb)(BSG_State *ctx, short *buf, int cnt);

int ivl;	//interval length
int isl;	//interval scale length (shorter=higher frequency)
int irl;	//interval rate length (shorter=faster rate)

int lphone;	//last phone

BGBMID_Context *bm_ctx;
int bm_prog;
};

struct BSG_PackHead_t {
char fcc[4];
byte offs[4];
byte ents[4];
};

struct BSG_DirEnt_t {
char name[56];
byte offs[4];
byte size[4];
};

struct BSG_PackState_s {
BSG_PackState *next;
char *name;

struct BSG_PackHead_t hdr;
struct BSG_DirEnt_t *dir;
void *fd;
int doffs, dents;
};

typedef struct {
void *(*fopen_fp)(char *name, char *mode);
void (*fclose_fp)(void *fd);
int (*fread_fp)(void *buf, int i, int j, void *fd);
int (*fwrite_fp)(void *buf, int i, int j, void *fd);
void (*fseek_fp)(void *buf, int i, int j);
int (*fgetc_fp)(void *fd);
int (*fputc_fp)(int i, void *fd);
int (*ftell_fp)(void *fd);
int (*feof_fp)(void *fd);
int (*fgets_fp)(void *buf, int sz, void *fd);
}BSG_IO;
