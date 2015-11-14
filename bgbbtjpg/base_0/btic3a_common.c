#include <bgbbtj.h>

extern const u32 btic2c_dbase[64];
extern const int btic2c_dextra[64];
extern const int btic2c_lbase[64];
extern const int btic2c_lextra[64];

const int btic3c_hilbert_idx[16]=
{
0, 1, 5, 4, 8, 12, 13, 9, 10, 14, 15, 11, 7, 6, 2, 3
};

const int btic3c_hilbert_xy[32]=
{
0, 0,  1, 0,  1, 1,  0, 1,
0, 2,  0, 3,  1, 3,  1, 2,
2, 2,  2, 3,  3, 3,  3, 2,
3, 1,  2, 1,  2, 0,  3, 0
};

