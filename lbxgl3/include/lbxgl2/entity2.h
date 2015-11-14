#ifndef __LBXGL_ENTITY_H__
#define __LBXGL_ENTITY_H__

typedef struct LBXGL_Entity_s LBXGL_Entity;
typedef struct LBXGL_World_s LBXGL_World;
typedef struct LBXGL_EntProperty_s LBXGL_EntProperty;
typedef struct LBXGL_EntCallback_s LBXGL_EntCallback;
typedef struct LBXGL_EntClassInfo_s LBXGL_EntClassInfo;

typedef struct LBXGL_EventCallback_s LBXGL_EventCallback;

typedef struct LBXGL_Entity_s Entity;
typedef struct LBXGL_World_s World;


struct LBXGL_Entity_s {
LBXGL_Entity *next;	//physical next link
LBXGL_Entity *lnext;	//next link (nodes/leaves)
LBXGL_Entity *chain;	//temporary next link (queries, ...)

LBXGL_EntProperty *attr;
LBXGL_EntCallback *cb;

LBXGL_World *world;	//world context for entity
BSP_Model *bmdl;
};

struct LBXGL_World_s {
BSP_Node *bsp;		//main world bsp
BSP_Model *mdls;	//models for world
LBXGL_Entity *ents;	//entities in world
LBXGL_Light *lights;	//lights in world
LBXGL_Light *dlights;	//dynamic lights in world

int id;			//world id

int num_styles;
char *styles[256];	//lightstyles
float style_gamma[256];	//current lightstyle gamma
int style_flags[256];	//current lightstyle flags
float time;
};

struct LBXGL_EntProperty_s {
LBXGL_EntProperty *next;
char *name;
void *value;
int flag;
};

struct LBXGL_EntCallback_s {
LBXGL_EntCallback *next;
char *name;
void *(*func)(LBXGL_Entity *ent, void *data, int nargs, void **args);
void *data;

void *(*f0)(LBXGL_Entity *ent);
void *(*f1)(LBXGL_Entity *ent, void *a);
void *(*f2)(LBXGL_Entity *ent, void *a, void *b);
void *(*f3)(LBXGL_Entity *ent, void *a, void *b, void *c);
void *(*f4)(LBXGL_Entity *ent, void *a, void *b, void *c, void *d);
void *(*f5)(LBXGL_Entity *ent, void *a, void *b, void *c, void *d, void *e);
void *(*f6)(LBXGL_Entity *ent, void *a, void *b, void *c, void *d, void *e,
	void *f);
void *(*f7)(LBXGL_Entity *ent, void *a, void *b, void *c, void *d, void *e,
	void *f, void *g);
void *(*f8)(LBXGL_Entity *ent, void *a, void *b, void *c, void *d, void *e,
	void *f, void *g, void *h);
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
