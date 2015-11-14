#define BTAC2_TWOCC(a, b)			((a)|((b)<<8))
#define BTAC2_FOURCC(a, b, c, d)	((a)|((b)<<8)|((c)<<16)|((d)<<24))

#define BTAC2_TWOCC_A(cc)			((cc)&255)
#define BTAC2_TWOCC_B(cc)			(((cc)>>8)&255)

typedef struct BGBMID_BTAC2_Context_s BGBMID_BTAC2_Context;

struct BGBMID_BTAC2_Context_s {
byte *cs, *css, *cse;
byte *ct, *cts, *cte;
u32 huff_win;
int huff_pos;

byte *stk_cs[8], *stk_css[8], *stk_cse[8];
int stk_pos;

u16 *huff_code[36];
u16 *huff_mask[36];
byte *huff_len[36];
u16 *huff_idx[36];
u16 *huff_next[36];
int *huff_stat[36];

int *qtab_min[36];
int *qtab_max[36];

u64 *tabidx;
int n_tabidx;

int btac_blklen;
int btac_blkbits;
int btac_blkbytes;
int flags;

byte chan;
byte bits;
int rate;
int len;

byte *headtab;
int sz_headtab;

byte *data;
int sz_data;
};
