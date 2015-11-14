#ifndef __LBXGL_SPRITE_H__
#define __LBXGL_SPRITE_H__

typedef struct LBXGL_Sprite_s LBXGL_Sprite;
typedef struct LBXGL_SpriteSeq_s LBXGL_SpriteSeq;
typedef struct LBXGL_SpriteFrame_s LBXGL_SpriteFrame;
typedef struct LBXGL_SpriteState_s LBXGL_SpriteState;

typedef struct LBXGL_LensFlare_s LBXGL_LensFlare;

struct LBXGL_Sprite_s {
LBXGL_Sprite *next;
char *name;		//name of info file
char *base;		//name of base dir
int flags;

LBXGL_SpriteSeq *seqs;

LBXGL_SpriteFrame *frames;
int n_frames;
};

struct LBXGL_SpriteSeq_s {
LBXGL_SpriteSeq *next;
char *name;

// float xs, ys, ox, oy;

float fps;
int n_frames;
int n_angles;
// int *texnums;	//frames*angles
LBXGL_SpriteFrame *frames;
};

struct LBXGL_SpriteFrame_s {
LBXGL_SpriteFrame *next;
float xs, ys, ox, oy;
int num;
int texnum;
};

struct LBXGL_SpriteState_s {
LBXGL_Sprite *sprite;
char *anim;
int frame;
int angle;
};

// struct LBXGL_SpriteInfo_s {
// LBXGL_Sprite *grid[81];		//9x9 grid of sprites, front to back, left to right
// };

struct LBXGL_LensFlare_s {
LBXGL_LensFlare *next;
char *name;

float xs, ys;
float xs2, ys2;

int bnum;	//base texture number
int fnum;	//texture of each sub-flare
float fdist;	//distance of sub flare
int fcnt;
};

#endif
