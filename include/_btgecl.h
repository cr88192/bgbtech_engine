#ifndef BTGECL_H
#define BTGECL_H

#include <lbxgl2.h>

#include <btge_shared.h>

#define BTCL_EFL_DIRTY		1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BTCL_Inventory_s BTCL_Inventory;
typedef struct BTCL_Entity_s BTCL_Entity;
typedef struct BTCL_Light_s BTCL_Light;
typedef struct BTCL_World_s BTCL_World;

struct BTCL_Inventory_s {
BTCL_Inventory *next;
char *classname;
int count;
int slot;
};

struct BTCL_Entity_s
{
BTCL_Entity *next;
int entnum;
float time;		//update server timestamp

float org[3];
float vel[3];
float ang[3];
float rot[4];
float mins[3];
float maxs[3];
float vorg[3];
char *mdl;
char *wmdl;
char *anim;
char *wanim;
char *lmdl;
int frm;
int wfrm;
int effects;
int effects2;
int flags;

float time2;	//update server prior timestamp
float org2[3];
float vel2[3];
float ang2[3];
float rot2[4];

float time3;	//update server prior timestamp
float org3[3];

float p_org[3];
float p_ang[3];
float p_rot[4];

int s_chan[8];

float *bone_org;	//origin
float *bone_rot;	//rotation (quat)
int n_bones;
int bone_mask;
};

struct BTCL_Light_s {
BTCL_Light *next;	//next (in list)
float org[3];
float dir[3];
float clr[3];
float val[2];
float coeff[3];
float mins[3];
float maxs[3];
float boxval[3];
float angle;
int flags;		//light flags
int num;		//conceptual light number
int style;		//lightstyle
char *texname;	//texture name
};

struct BTCL_World_s
{
char *mapname;
BTCL_Entity *ents;
BTCL_Light *lights;
int camnum;
float sv_time;		//server time
float sv_dtadjust;	//time adjustment offset
float msg_time;		//current message time
float msg_cltime;	//current message client time
float msg_estdt;	//current message client time

char *cprint_msg;	//centerprint message
float cprint_time;	//centerprint time

char *cprint_boxmsg;	//box message (dialogue)

char *bgm_name;		//background music
int bgm_id;			//background music id

int viewflags;		//view flags
int impfl;			//client impulse flags

int curHP;			//current HP
int maxHP;			//maximum HP
int curMP;			//current MP
int maxMP;			//maximum MP

float flash_time;
float flash_scale;		//alpha=time*scale
int flash_color;		//RGB

float view_mins[3];
float view_maxs[3];
float view_fov;
};

#ifdef __cplusplus
}
#endif

#endif
