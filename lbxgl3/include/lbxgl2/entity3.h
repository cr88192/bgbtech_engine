#ifndef LBXGL_ENTITY3_H
#define LBXGL_ENTITY3_H

typedef void *lbxEntity;
typedef void *lbxWorld;
typedef void *lbxEntList;

lbxEntity lbxNewEntity(lbxWorld wrl, char *classname);

float lbxGetf(lbxEntity ent, char *field);
double lbxGetd(lbxEntity ent, char *field);
void lbxGet2f(lbxEntity ent, char *field, float *vec);
void lbxGet3f(lbxEntity ent, char *field, float *vec);
void lbxGet4f(lbxEntity ent, char *field, float *vec);

void lbxSetf(lbxEntity ent, char *field, float val);
void lbxSet3f(lbxEntity ent, char *field, float *vec);

void lbxSetSolid(lbxEntity ent, char *solidtype);
void lbxSetMove(lbxEntity ent, char *movetype);
void lbxGetOriginf(lbxEntity ent, float *org);
void lbxSetOriginf(lbxEntity ent, float *org);


lbxEntList lbxQueryLine(lbxWorld wrl, float *start, float *end);
lbxEntList lbxQuerySphere(lbxWorld wrl, float *org, float rad);
lbxEntity lbxEntListEntity(lbxEntList lst);
lbxEntList lbxEntListNext(lbxEntList lst);

void lbxDamage(lbxEntity targ, lbxEntity from, float dmg);
void lbxTrigger(lbxEntity targ, lbxEntity from, char *event);

void lbxSetDamage(lbxEntity self,
	void (*fcn)(lbxEntity self, lbxEntity other, float hp));


#endif
