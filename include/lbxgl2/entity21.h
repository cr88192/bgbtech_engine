#ifndef __LBXGL_SENTITY_H__
#define __LBXGL_SENTITY_H__

#define LBXGL_SEFL_SEL		1	//selected
#define LBXGL_SEFL_OPSEL	2	//operator selected
#define LBXGL_SEFL_DEL		4	//deleted
#define LBXGL_SEFL_HIDDEN	8	//hidden

// typedef struct LBXGL_SEntity_s LBXGL_SEntity;
typedef struct LBXGL_SEntPair_s LBXGL_SEntPair;

struct LBXGL_SEntity_s {
LBXGL_SEntity *next;	//physical next link
LBXGL_SEntPair *keys;
int flags;

LBXGL_Brush *brush;
LBXGL_Brush *brush_last;
LBXGL_ModelState *mdl;
LBXGL_BrushModel *bmdl;
LBXGL_Light *light;
};

struct LBXGL_SEntPair_s {
LBXGL_SEntPair *next;
char *name;
char *value;
int flag;
};

#endif
