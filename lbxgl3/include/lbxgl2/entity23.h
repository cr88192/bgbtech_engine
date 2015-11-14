#ifndef __LBXGL_ENTITY_H__
#define __LBXGL_ENTITY_H__

#define LBXGL_ENTFL_DIRTY	1

typedef struct lbxEntProperty_s lbxEntProperty;
typedef struct lbxEntCallback_s lbxEntCallback;
typedef struct lbxEntClassInfo_s lbxEntClassInfo;

// typedef struct lbxEntity_s lbxEntity;
typedef struct LBXGL_BrushWorld_s lbxWorld;


struct lbxEntity_s {
lbxEntity *next;	//physical next link
lbxEntity *chain;	//temporary next link (queries, ...)

lbxEntProperty *attr;
lbxEntCallback *cb;

lbxWorld *world;	//world context for entity
};

struct lbxEntProperty_s {
lbxEntProperty *next;
char *name;
void *value;
int flag;
};

struct lbxEntCallback_s {
lbxEntCallback *next;
char *name;
void *func;
};

#if 0
struct lbxEntCallback_s {
lbxEntCallback *next;
char *name;
void *(*func)(lbxEntity *ent, void *data, void **args, int nargs);
void *data;

void *(*f0)(lbxEntity *ent);
void *(*f1)(lbxEntity *ent, void *a);
void *(*f2)(lbxEntity *ent, void *a, void *b);
void *(*f3)(lbxEntity *ent, void *a, void *b, void *c);
void *(*f4)(lbxEntity *ent, void *a, void *b, void *c, void *d);
void *(*f5)(lbxEntity *ent, void *a, void *b, void *c, void *d, void *e);
void *(*f6)(lbxEntity *ent, void *a, void *b, void *c, void *d, void *e,
	void *f);
void *(*f7)(lbxEntity *ent, void *a, void *b, void *c, void *d, void *e,
	void *f, void *g);
void *(*f8)(lbxEntity *ent, void *a, void *b, void *c, void *d, void *e,
	void *f, void *g, void *h);
};
#endif


struct lbxEntClassInfo_s {
lbxEntClassInfo *next;
lbxEntClassInfo *parent;
char *classname;
void *toplevel;
};

#endif
