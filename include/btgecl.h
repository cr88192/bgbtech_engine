#ifndef BTGECL_H
#define BTGECL_H

#include <lbxgl2.h>

#include <btgecl_conf.h>
#include <btge_shared.h>

#define BTCL_EFL_DIRTY		1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BTCL_MessageHandler_s BTCL_MessageHandler;
typedef struct BTCL_Inventory_s BTCL_Inventory;
typedef struct BTCL_Entity_s BTCL_Entity;
typedef struct BTCL_Light_s BTCL_Light;
typedef struct BTCL_World_s BTCL_World;

typedef struct BTCLUI_Context_s BTCLUI_Context;
typedef struct BTCLUI_Event_s BTCLUI_Event;
typedef struct BTCLUI_Form_s BTCLUI_Form;
typedef struct BTCLUI_WidgetType_s BTCLUI_WidgetType;
typedef struct BTCLUI_Widget_s BTCLUI_Widget;

typedef dytname("btcl_world_t") as_variant
	BTCL_World *btclWorld;

typedef dytname("btclui_context_t") as_variant
	BTCLUI_Context *btcluiContext;
typedef dytname("btclui_event_t") as_variant
	BTCLUI_Event *btcluiEvent;
typedef dytname("btclui_form_t") as_variant
	BTCLUI_Form *btcluiForm;
typedef dytname("btclui_widgettype_t") as_variant
	BTCLUI_WidgetType *btcluiWidgetType;
typedef dytname("btclui_widget_t") as_variant
	BTCLUI_Widget *btcluiWidget;

struct BTCL_MessageHandler_s {
BTCL_MessageHandler *next;	//next (in list)
dyt name;
void (*Handler)(BTCL_World *wrl, dyt msg);
dyt dyHandler;
};

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

//float org[3];
double org[3];
float vel[3];
float ang[3];
float rot[4];
float mins[3];
float maxs[3];
float vorg[3];

float altorg[3];
float altrot[4];

char *mdl;
char *wmdl;
char *anim;
char *wanim;
char *lmdl;
int frm;		//frame
int wfrm;		//weapon frame
int fcfrm;		//face frame
int effects;
int effects2;
int flags;
int sentid;		//SEntity ID

float time2;	//update server prior timestamp
//float org2[3];
double org2[3];
float vel2[3];
float ang2[3];
float rot2[4];

float time3;	//update server prior timestamp
//float org3[3];
double org3[3];

//float p_org[3];
double p_org[3];
float p_ang[3];
float p_rot[4];

int s_chan[8];

float *bone_org;	//origin
float *bone_rot;	//rotation (quat)
int *bone_fl;
int n_bones;
//s64 bone_mask;
};

struct BTCL_Light_s {
BTCL_Light *next;	//next (in list)
//float org[3];
double org[3];

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
char *mapname;			//current map name
BTCL_Entity *ents;		//synchronized entities
BTCL_Light *lights;		//synchronized lights
BTCLUI_Context *guictx;	//GUI contexts
int camnum;				//camera entity number
int guinum;				//bound GUI number

float sv_time;			//server time
float sv_dtadjust;		//time adjustment offset
float msg_time;			//current message time
float msg_cltime;		//current message client time
float msg_estdt;		//current message client time

char *cprint_msg;		//centerprint message
float cprint_time;		//centerprint time

char *cprint_boxmsg;	//box message (dialogue)
char *cprint_boxname;	//box name
char *cprint_boximg;	//box image / portrait

char *cprint_eval;		//centerprint eval
dyt cprint_eval_val;	//centerprint eval value

char *bgm_name;			//background music
int bgm_id;				//background music id

int viewflags;			//view flags
int impfl;				//client impulse flags

int curHP;				//current HP
int maxHP;				//maximum HP
int curMP;				//current MP
int maxMP;				//maximum MP
int curAP;				//current AP
int maxAP;				//maximum AP

float flash_time;		//how much time remains in color flash
float flash_scale;		//alpha=time*scale
int flash_color;		//RGB

float view_mins[3];
float view_maxs[3];
float view_fov;

s64 cam_reforg[3];		//reference point origin
byte cam_rebase;		//reference point changed

dyt msg_queue;			//message queue to server

int clientflags;		//client side state flags
int oldclientflags;		//old client side state flags

BTCL_Inventory *inven;	//inventory
int inv_slot_focus;		//inventory slot focus
int inv_slot_offset;
};

#include <btgecl_widget.h>
#include <btgecl_bsglue.h>

#include <btgecl_auto.h>

#ifdef __cplusplus
}
#endif

#endif
