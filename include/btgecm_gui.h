#define BTCMUI_FL_HASFOCUS			1
#define BTCMUI_FL_IMPLIEDFOCUS		2
#define BTCMUI_FL_HIDDEN			4

// typedef struct BTCMUI_Context_s BTCMUI_Context;
// typedef struct BTCMUI_Event_s BTCMUI_Event;
// typedef struct BTCMUI_Form_s BTCMUI_Form;
// typedef struct BTCMUI_WidgetType_s BTCMUI_WidgetType;
// typedef struct BTCMUI_Widget_s BTCMUI_Widget;

typedef struct BTCMUI_MenuItem_s BTCMUI_MenuItemT;
typedef struct BTCMUI_Menu_s BTCMUI_Menu;

// typedef struct PDGLUI_InputState_s BTCMUI_InputState;
// typedef struct PDGLUI_Camera_s BTCMUI_Camera;

struct BTCMUI_Context_s {
BTCMUI_Context *next;
BTCMUI_Context *old;
int idnum;		//ID number

int flags;		//status flags
int fgcolor;	//text fg color
int bgcolor;	//text bg color
int wbgcolor;	//widget bg color

BTGE_BrushWorld *world;
BTCMUI_Form *forms;

// BTCMUI_Menu *menu_root;
// BTCMUI_Menu *menu_draw;

// BTCMUI_Widget *stack[256];
// int stackpos;

// BTCMUI_Menu *menu_stack[64];
// int menu_stackpos;

// int formrover;
// int widgetrover;

// int forms_index;
// int forms_hasfocus;
};

struct BTCMUI_Event_s {
BTCMUI_Event *next;
BTCMUI_Form *form;
BTCMUI_Widget *widget;
char *event;
int eventid;
int ival;		//event specific integer value
char *sval;		//event specific string value
};

struct BTCMUI_Form_s {
BTCMUI_Context *uictx;	//UI Context
BTCMUI_Form *next;		//next form
BTCMUI_Form *old;		//old copy
char *name;
char *title;
char *style;
int idnum;		//ID number
int flags;		//form flags

BTCMUI_Event *event;
BTCMUI_Widget *root;
BTCMUI_Widget *first;
int ox, oy, xs, ys;

//form members
int fgcolor, bgcolor, wbgcolor;

void (*HandleEvent)(BTCMUI_Form *form, BTCMUI_Event *ev);
};

struct BTCMUI_WidgetType_s {
BTCMUI_WidgetType *next;
char *name;

// int (*Render)(BTCMUI_Widget *self, int ox, int oy);
// int (*HandleInput)(BTCMUI_Widget *self,
//	BTCMUI_InputState *state, int ox, int oy);

// int (*Size)(BTCMUI_Widget *self);
// BTCMUI_Widget *(*begin)(char *type, char *id);
// int (*end)(BTCMUI_Widget *self);
// int (*AddWidget)(BTCMUI_Widget *self, BTCMUI_Widget *other);

// char *(*get_property_s)(BTCMUI_Widget *self, char *var);
// int (*set_property_s)(BTCMUI_Widget *self, char *var, char *val);
// int (*get_property_i)(BTCMUI_Widget *self, char *var);
// int (*set_property_i)(BTCMUI_Widget *self, char *var, int val);
// int (*set_property_2i)(BTCMUI_Widget *self, char *var, int x, int y);
// int (*set_property_3i)(BTCMUI_Widget *self, char *var, int x, int y, int z);

int (*add_item)(BTCMUI_Widget *self, char *name, char *value);
int (*set_item)(BTCMUI_Widget *self, char *name, char *value);
int (*clear_items)(BTCMUI_Widget *self);
};

struct BTCMUI_Widget_s {
// BTCMUI_Widget *next, *prev;	//next/prev in list
// BTCMUI_Widget *first, *up;	//child/parent
BTCMUI_Widget *fnext;	//next in form
BTCMUI_Widget *old;		//old copy

BTCMUI_Widget *wparent;	//parent widget
BTCMUI_Widget *wchild;	//child widget
BTCMUI_Widget *wnext;	//next in widget
BTCMUI_Form *form;		//owning form

BTCMUI_Widget *chain;
int flags;	//1&=has current focus, 2&=has implicit focus
		//>65536&=widget flags

void *data;			//widget specific data
BTCMUI_WidgetType *vt;	//widget methods
int idnum;		//ID number

//form defined members
char *type;
char *name;
char *id;
char *value;
char *label;
char *image;
// int checked;
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


struct BTCMUI_MenuItem_s {
BTCMUI_MenuItemT *next;		//next in menu
BTCMUI_Menu *sub;		//sub-menu
char *id;			//item id	
char *name;			//item name
};

struct BTCMUI_Menu_s {
BTCMUI_Menu *next;		//next menu
BTCMUI_Menu *dnext;		//next drawn menu
BTCMUI_MenuItemT *item;
char *name;
int ox, oy;			//org for menus, like forms, upper left
int xs, ys;
int idnum;		//ID number

char *sel;			//selection id
};
