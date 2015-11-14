#ifndef __LBXGL_ENTITY_H__
#define __LBXGL_ENTITY_H__

typedef struct LBXGL_World_s LBXGL_World;
typedef struct LBXGL_WorldNode_s LBXGL_WorldNode;

typedef struct LBXGL_Entity_s LBXGL_Entity;
typedef struct LBXGL_EntProperty_s LBXGL_EntProperty;

typedef struct LBXGL_EventCallback_s LBXGL_EventCallback;

typedef struct LBXGL_Entity_s lbxEntity;
typedef struct LBXGL_World_s lbxWorld;


struct LBXGL_World_s {
LBXGL_Entity *ents;	//entities in world
LBXGL_Light *lights;	//lights in world
LBXGL_Light *dlights;	//dynamic lights in world
LBXGL_WorldNode *root;

BSP_Node *bsp;		//main world bsp
BSP_Model *mdls;	//models for world

int id;			//world id

int num_styles;
char *styles[256];	//lightstyles
float style_gamma[256];	//current lightstyle gamma
int style_flags[256];	//current lightstyle flags
float time;
};


struct LBXGL_WorldNode_s {
LBXGL_WorldNode *lnode, *rnode;
LBXGL_Entity *ents;
LBXGL_Light *lights;	//lights in node
LBXGL_Light *dlights;	//dynamic lights in node

float norm[4];

float mins[3], maxs[3];
float org[3];
float radius;
int num, flags;
};


struct LBXGL_Entity_s {
LBXGL_Entity *next;	//physical next link (world)
LBXGL_Entity *wnext;	//next link (World nodes/leaves)
LBXGL_Entity *lnext;	//next link (BSP nodes/leaves)
LBXGL_Entity *chain;	//temporary next link (queries, ...)
LBXGL_EntProperty *attr;

LBXGL_World *world;	//world context for entity
LBXGL_WorldNode *node;	//owner node

BSP_Model *bmdl;
LBXGL_Brush *brush;
int flags;

int id;			//entity id
int solid;		//solid id
float org[3];
float vel[3];
float rot[9];
};

struct LBXGL_EntProperty_s {
LBXGL_EntProperty *next;
char *name;
void *value;
int flag;
};


struct LBXGL_EventCallback_s {
LBXGL_EventCallback *next;
char *target;
char *event;
void *data;
int (*func)(void *data, char *target, char *event, void **msgdata);
};

#endif
