#define BTCLUI_GUISTATE_HASFOCUS		1

#define BTCLUI_EV_NONE			0
#define BTCLUI_EV_MISC			1
#define BTCLUI_EV_CLICKED		2
#define BTCLUI_EV_UNCLICKED		3
#define BTCLUI_EV_CHANGED		4
#define BTCLUI_EV_SELECT		5

#define BTCLUI_FL_HASFOCUS			1
#define BTCLUI_FL_IMPLIEDFOCUS		2
#define BTCLUI_FL_HIDDEN			4


// typedef struct BTCLUI_Context_s BTCLUI_Context;
// typedef struct BTCLUI_Event_s BTCLUI_Event;
// typedef struct BTCLUI_Form_s BTCLUI_Form;
// typedef struct BTCLUI_WidgetType_s BTCLUI_WidgetType;
// typedef struct BTCLUI_Widget_s BTCLUI_Widget;

typedef struct BTCLUI_MenuItem_s BTCLUI_MenuItemT;
typedef struct BTCLUI_Menu_s BTCLUI_Menu;

typedef struct PDGLUI_InputState_s BTCLUI_InputState;
typedef struct PDGLUI_Camera_s BTCLUI_Camera;

struct BTCLUI_Context_s {
BTCLUI_Context *next;

int fgcolor;	//text fg color
int bgcolor;	//text bg color
int wbgcolor;	//widget bg color

BTCLUI_InputState *state;
BTCLUI_InputState *state2;
BTCLUI_Event *widget_lastevent;

// BTCLUI_Form *forms_root;
// BTCLUI_Form *forms_focus;
// BTCLUI_Form *form;

BTCLUI_Form *forms_focus;
BTCLUI_Form *forms;

// BTCLUI_Widget *stack[256];
// int stackpos;

BTCLUI_Menu *menu_root;
BTCLUI_Menu *menu_draw;

// BTCLUI_Menu *menu_stack[64];
// int menu_stackpos;

// int formrover;
// int widgetrover;

int forms_index;
int forms_hasfocus;
int idnum;		//ID number
};

struct BTCLUI_Event_s {
BTCLUI_Event *next;
BTCLUI_Form *form;
BTCLUI_Widget *widget;
char *event;
int eventid;
int ival;		//event specific integer value
char *sval;		//event specific string value
};

struct BTCLUI_Form_s {
BTCLUI_Context *uictx;	//UI Context
BTCLUI_Form *next;		//next form
char *name;
char *title;
int idnum;		//ID number
int flags;		//form flags

BTCLUI_Event *event;
BTCLUI_Widget *root;
BTCLUI_Widget *first;
int ox, oy, xs, ys;

//form members
int fgcolor, bgcolor, wbgcolor;
char *style;
};

struct BTCLUI_WidgetType_s {
BTCLUI_WidgetType *next;
char *name;

int (*Render)(BTCLUI_Widget *self, int ox, int oy);
int (*HandleInput)(BTCLUI_Widget *self,
	BTCLUI_InputState *state, int ox, int oy);
int (*Size)(BTCLUI_Widget *self);
int (*AddWidget)(BTCLUI_Widget *self, BTCLUI_Widget *other);
int (*Destroy)(BTCLUI_Widget *self);

// BTCLUI_Widget *(*begin)(char *type, char *id);
// int (*end)(BTCLUI_Widget *self);

// char *(*get_property_s)(BTCLUI_Widget *self, char *var);
// int (*set_property_s)(BTCLUI_Widget *self, char *var, char *val);
// int (*get_property_i)(BTCLUI_Widget *self, char *var);
// int (*set_property_i)(BTCLUI_Widget *self, char *var, int val);
// int (*set_property_2i)(BTCLUI_Widget *self, char *var, int x, int y);
// int (*set_property_3i)(BTCLUI_Widget *self, char *var, int x, int y, int z);

int (*AddItem)(BTCLUI_Widget *self, char *name, char *value);
int (*SetItem)(BTCLUI_Widget *self, char *name, char *value);
int (*ClearItems)(BTCLUI_Widget *self);
};

struct BTCLUI_Widget_s {
// BTCLUI_Widget *next, *prev;	//next/prev in list
// BTCLUI_Widget *first, *up;	//child/parent
BTCLUI_Widget *fnext;	//next in form

BTCLUI_Widget *wparent;	//parent widget
BTCLUI_Widget *wchild;	//child widget
BTCLUI_Widget *wnext;	//next in widget
BTCLUI_Form *form;		//owning form

BTCLUI_Widget *chain;
int flags;	//1&=has current focus, 2&=has implicit focus
		//>65536&=widget flags

void *data;			//widget specific data
BTCLUI_WidgetType *vt;	//widget methods
int idnum;		//ID number

//form defined members
char *type;
char *name;
char *id;
char *value;
char *label;
char *image;
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


struct BTCLUI_MenuItem_s {
BTCLUI_MenuItemT *next;		//next in menu
BTCLUI_Menu *sub;		//sub-menu
char *id;			//item id	
char *name;			//item name
};

struct BTCLUI_Menu_s {
BTCLUI_Menu *next;		//next menu
BTCLUI_Menu *dnext;		//next drawn menu
BTCLUI_MenuItemT *item;
char *name;
int ox, oy;			//org for menus, like forms, upper left
int xs, ys;
int idnum;		//ID number

char *sel;			//selection id
};
