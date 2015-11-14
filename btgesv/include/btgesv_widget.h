#define BTSVUI_GUISTATE_HASFOCUS		1

#define BTSVUI_EV_NONE		0
#define BTSVUI_EV_MISC		1
#define BTSVUI_EV_CLICKED		2
#define BTSVUI_EV_UNCLICKED	3
#define BTSVUI_EV_CHANGED		4
#define BTSVUI_EV_SELECT		5


typedef struct BTSVUI_Context_s BTSVUI_Context;
typedef struct BTSVUI_Event_s BTSVUI_Event;
typedef struct BTSVUI_Form_s BTSVUI_Form;
typedef struct BTSVUI_WidgetType_s BTSVUI_WidgetType;
typedef struct BTSVUI_Widget_s BTSVUI_Widget;

typedef struct BTSVUI_MenuItem_s BTSVUI_MenuItemT;
typedef struct BTSVUI_Menu_s BTSVUI_Menu;

// typedef struct BTSVUI_InputState_s BTSVUI_InputState;
// typedef struct BTSVUI_Camera_s BTSVUI_Camera;

// typedef struct PDGLUI_InputState_s BTSVUI_InputState;
// typedef struct PDGLUI_Camera_s BTSVUI_Camera;

struct BTSVUI_Context_s {
// int fgcolor;	//text fg color
// int bgcolor;	//text bg color
// int wbgcolor;	//widget bg color

// BTSVUI_InputState *state;
// BTSVUI_InputState *state2;
// BTSVUI_Event *widget_lastevent;

BTSVUI_Form *forms_root;
BTSVUI_Form *forms_focus;
BTSVUI_Form *form;

BTSVUI_Widget *widget_stack[256];
int widget_stackpos;

BTSVUI_Menu *menu_root=NULL;
BTSVUI_Menu *menu_draw=NULL;

BTSVUI_Menu *menu_stack[64];
int menu_stackpos;

int formrover;
int widgetrover;

int forms_index;
int forms_hasfocus;
int idnum;		//ID number

};

struct BTSVUI_Event_s {
BTSVUI_Event *next;
BTSVUI_Form *form;
BTSVUI_Widget *widget;
char *event;
int eventid;
int ival;		//event specific integer value
char *sval;		//event specific string value
};

struct BTSVUI_Form_s {
BTSVUI_Form *next;
char *name;
char *title;
int idnum;		//ID number

BTSVUI_Event *event;
BTSVUI_Widget *root;
int ox, oy, xs, ys;

//form members
int fgcolor, bgcolor, wbgcolor;
char *style;
};

struct BTSVUI_WidgetType_s {
BTSVUI_WidgetType *next;
char *name;

// int (*render)(BTSVUI_Widget *self, int ox, int oy);
// int (*handle_input)(BTSVUI_Widget *self,
//	BTSVUI_InputState *state, int ox, int oy);
// int (*size)(BTSVUI_Widget *self);

BTSVUI_Widget *(*begin)(char *type, char *id);
int (*end)(BTSVUI_Widget *self);
int (*add_widget)(BTSVUI_Widget *self, BTSVUI_Widget *other);

char *(*get_property_s)(BTSVUI_Widget *self, char *var);
int (*set_property_s)(BTSVUI_Widget *self, char *var, char *val);
int (*get_property_i)(BTSVUI_Widget *self, char *var);
int (*set_property_i)(BTSVUI_Widget *self, char *var, int val);
int (*set_property_2i)(BTSVUI_Widget *self, char *var, int x, int y);
int (*set_property_3i)(BTSVUI_Widget *self, char *var, int x, int y, int z);

int (*add_item)(BTSVUI_Widget *self, char *name, char *value);
int (*set_item)(BTSVUI_Widget *self, char *name, char *value);
int (*clear_items)(BTSVUI_Widget *self);
};

struct BTSVUI_Widget_s {
BTSVUI_Widget *next, *prev;	//next/prev in list
BTSVUI_Widget *first, *up;	//child/parent
BTSVUI_Form *form;		//owning form

BTSVUI_Widget *chain;
int state;	//1&=has current focus, 2&=has implicit focus
		//>65536&=widget flags

void *data;			//widget specific data
BTSVUI_WidgetType *wvt;	//widget methods
int idnum;		//ID number

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


struct BTSVUI_MenuItem_s {
BTSVUI_MenuItemT *next;		//next in menu
BTSVUI_Menu *sub;		//sub-menu
char *id;			//item id	
char *name;			//item name
};

struct BTSVUI_Menu_s {
BTSVUI_Menu *next;		//next menu
BTSVUI_Menu *dnext;		//next drawn menu
BTSVUI_MenuItemT *item;
char *name;
int ox, oy;			//org for menus, like forms, upper left
int xs, ys;
int idnum;		//ID number

char *sel;			//selection id
};
