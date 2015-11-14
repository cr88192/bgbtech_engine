#define CON_MAX_XS		256
#define CON_MAX_YS		192
#define CON_MAX_HIST	16

#define CON_EFF_MASK_FG		0x000F		//foreground
#define CON_EFF_MASK_BG		0x00F0		//background

#define CON_EFF_ITALIC		0x0100		//italic
#define CON_EFF_UNDERLINE	0x0200		//underline
#define CON_EFF_BLINK		0x0400		//blink
#define CON_EFF_STRIKEOUT	0x0800		//strikeout
#define CON_EFF_FAINT		0x1000		//faint
#define CON_EFF_BOLD		0x2000		//bold
#define CON_EFF_SUBSUPER	0x4000		//subsuper
#define CON_EFF_CHARESC		0x8000		//character escape

#define CON_FLAG_DIRTY		0x00010000	//console has updates
#define CON_FLAG_BLINKSTATE	0x00020000	//console blink state

typedef struct BTGE_Cvar_s BTGE_Cvar;
typedef struct BTGE_ConCmd_s BTGE_ConCmd;
typedef struct BTGE_ConEdit_s BTGE_ConEdit;

typedef struct BTGE_ConsoleInfo_s BTGE_ConsoleInfo;

struct BTGE_Cvar_s {
BTGE_Cvar *next;
char *name;
char *desc;
char *value;
};

struct BTGE_ConCmd_s {
BTGE_ConCmd *next;
char *name;
char *desc;
int (*func)(char **args);
};

struct BTGE_ConEdit_s {
BTGE_ConEdit *next;
char *name;			//module name
char *filename;		//file name

u16 **linebuf;		//editor line buffer
int n_lines;		//number of lines
int m_lines;		//max lines

int win_x;			//edit window X position
int win_y;			//edit window Y position
int cur_x;			//edit cursor X position
int cur_y;			//edit cursor Y position

int sel_sx;			//selection start X
int sel_sy;			//selection start Y
int sel_ex;			//selection end X
int sel_ey;			//selection end Y
// int (*func)(char **args);
int flags;
};

struct BTGE_ConsoleInfo_s {
//short conbuf[75][100];
//short conclr[75][100];
//short promptbuf[100], promptpos;
//short histbuf[16][100], histpos;
//short completebuf[100];

u16 conbuf[CON_MAX_YS][CON_MAX_XS];
u16 conclr[CON_MAX_YS][CON_MAX_XS];
u16 promptbuf[CON_MAX_XS];
u16 histbuf[CON_MAX_HIST][CON_MAX_XS];
u16 completebuf[CON_MAX_XS];
int promptpos;
int histpos;

int curclr;
char *stuffcmd[1024];
int nstuffcmd;

int x, y;		//cursor XY
int xs, ys;		//console size
int flags;

BTGE_ConEdit *edit;		//console edit context
void *vbo_info;			//console VBO context

void (*WriteString)(BTGE_ConsoleInfo *con, char *str);
int (*Update)(BTGE_ConsoleInfo *con, float dt);
int (*HandleKey)(BTGE_ConsoleInfo *con, int num, int down);
int (*Clear)(BTGE_ConsoleInfo *con);
int (*Render)(BTGE_ConsoleInfo *con);
int (*KeyDown)(BTGE_ConsoleInfo *con, int key);

int (*OldHandleKey)(BTGE_ConsoleInfo *con, int num, int down);
};
