typedef struct BTGECM_Pack2Head_s BTGECM_Pack2Head;
typedef struct BTGECM_Pack2DirEnt_s BTGECM_Pack2DirEnt;
typedef struct BTGECM_Pack2State_s BTGECM_Pack2State;

struct BTGECM_Pack2Head_s {
char fcc[4];
byte version;
byte blocksz;
byte headsz[2];
byte offs[8];
byte ents[4];
byte maxents[4];
};

struct BTGECM_Pack2DirEnt_s {
char name[96];
byte offs[8];
byte size[4];
byte csize[4];
byte crc[4];
byte mtime[4];
byte type;
byte method;
byte flags[2];
byte nextdirent[4];
};

struct BTGECM_Pack2State_s {
BTGECM_Pack2State *next;
char *name;

BTGECM_PackHead hdr;
BTGECM_PackDirEnt *dir;
void *fda[32];
s64 doffs;
int dents, mdents;
bool readwrite;
bool dirty;
};

struct BTGECM_Pack2FileInfo_s {
char *name;
s64 offs;
int size;
int csize;
u32 crc;
u32 mtime;
byte type;
byte method;
u16 flags;
u32 nextdirent;
};
