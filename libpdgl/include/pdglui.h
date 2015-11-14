#include <pdgl.h>

#ifndef __PDGL_GUI_H__
#define __PDGL_GUI_H__

#ifdef __cplusplus
extern "C" {
#endif

#define PDGLUI_GUISTATE_HASFOCUS		1

#define PDGLUI_EV_NONE		0
#define PDGLUI_EV_MISC		1
#define PDGLUI_EV_CLICKED	2
#define PDGLUI_EV_UNCLICKED	3
#define PDGLUI_EV_CHANGED	4
#define PDGLUI_EV_SELECT	5

#define PDGLUI_CAM_PERSP	0
#define PDGLUI_CAM_ORTHO	1
#define PDGLUI_CAM_XY		2
#define PDGLUI_CAM_XZ		3
#define PDGLUI_CAM_YZ		4
#define PDGLUI_CAM_PERSP2	5
#define PDGLUI_CAM_QUAT		6
#define PDGLUI_CAM_CAD4V	7	//CAD 4 views
#define PDGLUI_CAM_SOLID2D	8
#define PDGLUI_CAM_SPLIT3D	9	//split stereo 3D

#define PDGLUI_CAM_NMODE	10

#define PDGLUI_QMODE_XYZ	0
#define PDGLUI_QMODE_XYW	1
#define PDGLUI_QMODE_XZW	2
#define PDGLUI_QMODE_YZW	3

#define PDGLUI_QMODE_XY		1
#define PDGLUI_QMODE_XZ		2
#define PDGLUI_QMODE_YZ		3

#define PDGLUI_QMODE_LEFT	4
#define PDGLUI_QMODE_RIGHT	5

#define PDGLUI_CAMFL_HFLIP			1
#define PDGLUI_CAMFL_VFLIP			2
#define PDGLUI_CAMFL_RECORDHIDEUI	4	//don't draw UI while recording


typedef struct PDGLUI_Event_s PDGLUI_Event;
typedef struct PDGLUI_Form_s PDGLUI_Form;
typedef struct PDGLUI_WidgetType_s PDGLUI_WidgetType;
typedef struct PDGLUI_Widget_s PDGLUI_Widget;

// typedef struct PDGLUI_MenuItem_s PDGLUI_MenuItem;
typedef struct PDGLUI_MenuItem_s PDGLUI_MenuItemT;
typedef struct PDGLUI_Menu_s PDGLUI_Menu;

typedef struct PDGLUI_InputState_s PDGLUI_InputState;
typedef struct PDGLUI_Camera_s PDGLUI_Camera;

typedef struct PDGLUI_ProgramInfo_s PDGLUI_ProgramInfo;

typedef dytname("pdglui_inputstate_t") as_variant
	PDGLUI_InputState *pdgluiInputState;
typedef dytname("pdglui_camera_t") as_variant
	PDGLUI_Camera *pdgluiCamera;
typedef dytname("pdglui_programinfo_t") as_variant
	PDGLUI_ProgramInfo *pdgluiProgramInfo;

struct PDGLUI_Event_s {
PDGLUI_Event *next;
char *widget;
char *event;
int eventid;
int ival;		//event specific integer value
char *sval;		//event specific string value
};

struct PDGLUI_Form_s {
PDGLUI_Form *next;
char *name;
char *title;

PDGLUI_Event *event;
PDGLUI_Widget *root;
int ox, oy, xs, ys;

//form members
int fgcolor, bgcolor, wbgcolor;
char *style;
};

struct PDGLUI_WidgetType_s {
PDGLUI_WidgetType *next;
char *name;

int (*render)(PDGLUI_Widget *self, int ox, int oy);
int (*handle_input)(PDGLUI_Widget *self,
	PDGLUI_InputState *state, int ox, int oy);
int (*size)(PDGLUI_Widget *self);

PDGLUI_Widget *(*begin)(char *type, char *id);
int (*end)(PDGLUI_Widget *self);
int (*add_widget)(PDGLUI_Widget *self, PDGLUI_Widget *other);

char *(*get_property_s)(PDGLUI_Widget *self, char *var);
int (*set_property_s)(PDGLUI_Widget *self, char *var, char *val);
int (*get_property_i)(PDGLUI_Widget *self, char *var);
int (*set_property_i)(PDGLUI_Widget *self, char *var, int val);
int (*set_property_2i)(PDGLUI_Widget *self, char *var, int x, int y);
int (*set_property_3i)(PDGLUI_Widget *self, char *var, int x, int y, int z);

int (*add_item)(PDGLUI_Widget *self, char *name, char *value);
int (*set_item)(PDGLUI_Widget *self, char *name, char *value);
int (*clear_items)(PDGLUI_Widget *self);
};

struct PDGLUI_Widget_s {
PDGLUI_Widget *next, *prev;	//next/prev in list
PDGLUI_Widget *first, *up;	//child/parent
PDGLUI_Form *form;		//owning form

PDGLUI_Widget *chain;
int state;	//1&=has current focus, 2&=has implicit focus
		//>65536&=widget flags

void *data;			//widget specific data
PDGLUI_WidgetType *funcs;	//widget methods

//form defined members
char *type;
char *name;
char *id;
char *value;
char *label;
int checked;
int size;
int maxlength;
int rows, cols;
int fgcolor, bgcolor, wbgcolor;

//generated members
int ox, oy, xs, ys;	//common: widget origin, size
char *buffer, *bufend;	//textbox/password
int texnum;		//img
int min, max, curval;	//sliders

//textarea
char **bufptrs;		//
int nbufptrs;		//
int wxpos, wypos;	//pos of window
int cxpos, cypos;	//pos of cursor
int sxpos, sypos;	//pos of start of selection

//list
char **item_name;
char **item_value;
int num_items;

};


struct PDGLUI_MenuItem_s {
PDGLUI_MenuItemT *next;		//next in menu
PDGLUI_Menu *sub;			//sub-menu
char *id;					//item id	
char *name;					//item name
char *keycmd;				//keyboard shortcut
PDGLUI_Widget *widget;		//contained widget
int ico_tex;				//icon texture
float ico_sm, ico_sn;		//icon S coords
float ico_tm, ico_tn;		//icon T coords
};

struct PDGLUI_Menu_s {
PDGLUI_Menu *next;		//next menu
PDGLUI_Menu *dnext;		//next drawn menu
PDGLUI_MenuItemT *item;
char *name;
int ox, oy;			//org for menus, like forms, upper left
int xs, ys;

void (*Select)(PDGLUI_Menu *menu, char *id);

int xs_ico, xs_name;
int xs_key, xs_widget;
int ys_item;

char *sel;			//selection id
};


struct PDGLUI_Camera_s {
float org[4];
float ang[8];
float vel[4];
float rot[9];

float ivel[4];	//impulse velocity
float iang[8];	//impulse angle
float irot[9];	//impulse rotation
float izvel;	//impulse Z velocity (up/down keys)
float ipvel[4];	//impulse planar velocity

float speed;	//camera movement speed

float width;	//camera Ortho width
float fov;	//camera fov (Persp)
float width2;	//camera P2 width
float fov2;	//camera fov (P2)

int constrain;	//constrain orientation
int mode;
int ortho;
int raw;		//raw control, 0=frontend specific
int mgrab;		//mouse grab

float lorg[4];
float lang[8];
float lvel[4];
float lrot[9];

float rt[4];
float fw[4];
float up[4];
float zn[4];

float clip_lf[4];
float clip_rt[4];
float clip_up[4];
float clip_dn[4];
float clip_bk[4];

float morg[4];
float mdir[4];
float mend[4];

float lmorg[4];
float lmdir[4];
float lmend[4];

float mins2d[4];	//solid2D mins
float maxs2d[4];	//solid2D maxs

//quat mode
// float qorg[4];
float qrot[16];
int qmode;

float qproj[20];

float *qxform;
int nqxform;

int lockcfg;		//lock camera configuration
float eyedist;		//stereo eye distance
int v_xo, v_yo;		//view x and y origin
int v_xs, v_ys;		//view x and y size
float v_org[4];		//view origin

int flags;
};


struct PDGLUI_InputState_s {
int mx, my;		//current mouse pos
int lmx, lmy;	//mouse pos last frame
int omx, omy;	//last non-dragging pos
int dmx, dmy;	//delta mouse pos
int dx, dy;		//drag from lmx
int odx, ody;	//drag from omx

int rmx, rmy;	//"real" mouse pos
int rlmx, rlmy;	//"real" mouse pos last frame

int mb, lmb;
unsigned short *keys;

int orgx, orgy;
int zoom;

int xs, ys;		//screen x and y size
int lxs, lys;	//screen last x and y size
float dt_f;		//current frame delta time
float adt_f;	//avg frame time
float adt2_f;	//avg frame time 2
float time_f;	//current time

int doshot;
int kill;

//performance
int dt_update;
int dt_vischeck;
int dt_render;
int dt_render_dark;
int dt_render_fullshadow;
int dt_render_noshadow;
int dt_render_vertex;
int dt_render_final;
int dt_render_decal;
int dt_render_alpha;
int dt_render_shadow_face;
int dt_render_shadow_volume;
int dt_render_particle;
int dt_render_avg;

byte keymap[32];

};

struct PDGLUI_ProgramInfo_s
{
char *title;
void *data;

int (*Render2D)(PDGLUI_ProgramInfo *prog);
int (*HandleInput)(PDGLUI_ProgramInfo *prog,
	PDGLUI_InputState *state);

};

#include <pdglui/pdglui_conf.h>

#include <pdglui/auto.h>

#ifdef __cplusplus
}
#endif

#endif
