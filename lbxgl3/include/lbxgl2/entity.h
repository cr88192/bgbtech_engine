#ifndef __LBXGL_ENTITY_H__
#define __LBXGL_ENTITY_H__

typedef struct LBXGL_Entity_s LBXGL_Entity;
typedef struct LBXGL_World_s LBXGL_World;
typedef struct LBXGL_EntProperty_s LBXGL_EntProperty;
typedef struct LBXGL_EntClassInfo_s LBXGL_EntClassInfo;

typedef struct LBXGL_EventCallback_s LBXGL_EventCallback;

struct LBXGL_Entity_s {
LBXGL_Entity *next;	//physical next link
LBXGL_Entity *lnext;	//next link (nodes/leaves)
LBXGL_EntProperty *attr;

LBXGL_World *world;	//world context for entity
BSP_Model *bmdl;

int solid;		//solid id
float org[3];
float rot[9];
float gamma;
};

struct LBXGL_World_s {
BSP_Node *bsp;		//main world bsp
BSP_Model *mdls;	//models for world
LBXGL_Entity *ents;	//entities in world
LBXGL_Light *lights;	//lights in world
LBXGL_Light *dlights;	//dynamic lights in world

int id;			//world id
};

struct LBXGL_EntProperty_s {
LBXGL_EntProperty *next;
char *name;
void *value;
};


struct LBXGL_EntClassInfo_s {
LBXGL_EntClassInfo *next;
LBXGL_EntClassInfo *parent;
char *classname;
void *toplevel;
};

struct LBXGL_EventCallback_s {
LBXGL_EventCallback *next;
char *target;
char *event;
void *data;
int (*func)(void *data, char *target, char *event, void **msgdata);
};

#endif
