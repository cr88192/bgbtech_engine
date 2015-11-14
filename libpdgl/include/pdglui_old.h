#ifndef __PDGL_GUI_H__
#define __PDGL_GUI_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PDGLUI_Form_s PDGLUI_Form;
typedef struct PDGLUI_WidgetType_s PDGLUI_WidgetType;
typedef struct PDGLUI_Widget_s PDGLUI_Widget;

typedef struct {
int mx, my;	//current mouse pos
int lmx, lmy;	//mouse pos last frame
int omx, omy;	//last non-dragging pos
int dx, dy;	//drag from lmx
int odx, ody;	//drag from omx

int rmx, rmy;	//"real" mouse pos
int rlmx, rlmy;	//"real" mouse pos last frame

int mb, lmb;
unsigned short *keys;

int orgx, orgy;
int zoom;
}PDGLUI_InputState;

typedef struct {
int fgcolor;	//text fg color
int bgcolor;	//text bg color
int wbgcolor;	//widget bg color
}PDGLUI_Forms_StyleState;

struct PDGLUI_Form_s {
PDGLUI_Form *next;

char *name;
PDGLUI_Widget *root;
int ox, oy, xs, ys;

int (*callback)(char *form, void *ptr, char *widget,
	char *event, int nargs, void **args);
void *data;

//form members
int fgcolor, bgcolor, wbgcolor;
char *style;
};

struct PDGLUI_WidgetType_s {
PDGLUI_WidgetType *next;

char *name;

PDGLUI_Widget *(*build)(NetParse_Node *node,
	PDGLUI_Form *form, PDGLUI_Forms_StyleState *style);
};

struct PDGLUI_WidgetMethods_s {
int (*render)(PDGLUI_Widget *self, int ox, int oy);
int (*handle_input)(PDGLUI_Widget *self,
	PDGLUI_InputState *state, int ox, int oy);
int (*size)(PDGLUI_Widget *self);

int (*message)(PDGLUI_Widget *self, char *msg, int nargs, void **args);
void *(*get_property)(PDGLUI_Widget *self, char *var);
int (*set_property)(PDGLUI_Widget *self, char *var, void *val);
void *(*call)(PDGLUI_Widget *self, char *msg, int nargs, void **args);
};

struct PDGLUI_Widget_s {
PDGLUI_Widget *next, *prev;	//next/prev in list
PDGLUI_Widget *first, *up;	//child/parent
PDGLUI_Form *form;		//owning form

NetParse_Node *body;		//body xml

PDGLUI_Widget *chain;
int state;	//1&=has current focus, 2&=has implicit focus
		//>65536&=widget flags

void *data;				//widget specific data
struct PDGLUI_WidgetMethods_s *funcs;	//widget methods

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
int texnum;	//img
int min, max, curval;

//textarea
char **bufptrs;		//
int nbufptrs;		//
int wxpos, wypos;	//pos of window
int cxpos, cypos;	//pos of cursor
int sxpos, sypos;	//pos of start of selection

};

#define PDGLUI_GUISTATE_HASFOCUS		1


#include <pdglui/auto.h>

#ifdef __cplusplus
}
#endif

#endif
