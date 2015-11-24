#include <pdgl.h>

#ifdef linux

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/gl.h>
#include <GL/glx.h>

// #include <gfxdev/window.h>
// #include <gfxdev/keys.h>

#include <X11/keysym.h>
#include <X11/cursorfont.h>

// #include <X11/extensions/Xxf86dga.h>
// #include <X11/extensions/xf86vmode.h>

#define KEY_MASK (KeyPressMask | KeyReleaseMask)
#define MOUSE_MASK (ButtonPressMask | ButtonReleaseMask | \
			PointerMotionMask | ButtonMotionMask )
#define X_MASK (KEY_MASK | MOUSE_MASK | VisibilityChangeMask | StructureNotifyMask )

struct windowdata_s {
/* both */
int			isglx;
Window		win;
int			mx, my, mb;

/* GLX */
GLXContext	ctx;

/* non-GLX */
GC			gc;
XImage		*img;
byte		*buf;
int			pixsz;
int			linesz;

/* misc */
byte keymap[32];
};

typedef struct displaywindow_s {
struct displaywindow_s *next, *prev;

int width, height;
void *viddata; /* data for whatever the video driver is */
}displaywindow;

Display *x_disp;
Window root;
XVisualInfo *visinfo;
int scrnum;

int win_init = 0;

int mouse_x, mouse_y;
int win_x, win_y;

int	*gfxdrv_rkill;

displaywindow *win_active, *win_root;
int (*key_handler[16])(displaywindow *wnd, int key);

displaywindow *gfx_screen;
// byte *gfxdev_buf;

int window_def_width=800;
int window_def_height=600;
char *window_def_label="PDGL";
int window_max_width=0;
int window_max_height=0;

int window_width;
int window_height;
int window_active;
int window_lastactive;
int window_fullscreen;


static int gfxdrv_log2up(int v)
{
	int i, j;
	
	i=v; j=0;
	while(i>1) { i=(i+1)>>1; j++; }
	return(j);
}

//PDGL_API void GfxDrv_GetWindowSize(int *xs, int *ys)
//	{ *xs=window_width; *ys=window_height; }

PDGL_API void GfxDrv_GetWindowTexSize(int *xs, int *ys)
{
//	*xs=1<<gfxdrv_log2up(window_width);
//	*ys=1<<gfxdrv_log2up(window_height);

	*xs=1<<gfxdrv_log2up(gfx_screen->width);
	*ys=1<<gfxdrv_log2up(gfx_screen->height);
}

PDGL_API void GfxDrv_GetWindowMaxSize(int *xs, int *ys)
	{ *xs=window_max_width; *ys=window_max_height; }

PDGL_API void GfxDrv_GetWindowMaxTexSize(int *xs, int *ys)
{
	*xs=1<<gfxdrv_log2up(window_max_width);
	*ys=1<<gfxdrv_log2up(window_max_height);
}

PDGL_API bool GfxDrv_WindowIsActiveP(void)
	{ return(window_active); }
PDGL_API bool GfxDrv_WindowIsFullActiveP(void)
	{ return(window_active && window_lastactive); }
PDGL_API bool GfxDrv_WindowIsFullscreenP(void)
	{ return(window_fullscreen); }

PDGL_API int GfxDrv_SetupThreadShareLists()
{
}

PDGL_API int GfxDrv_TeardownThreadShareLists()
{
}

PDGL_API void GfxDrv_SetDefaultFullscreen(int fs)
{
//	window_def_fullscreen=fs;
}

PDGL_API void GfxDrv_SetFullscreenResolution(int width, int height)
{
//	window_fullscreen_width=width;
//	window_fullscreen_height=height;
}

int Window_Init()
{
	int attrib[] = {
		GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None
	};

	printf("Window Init.\n");

	if (!(x_disp = XOpenDisplay(NULL))) {
		fprintf(stderr, "Error couldn't open the X display\n");
		return(-1);
	}

	scrnum = DefaultScreen(x_disp);
	root = RootWindow(x_disp, scrnum);

	visinfo = glXChooseVisual(x_disp, scrnum, attrib);
	if (!visinfo) {
		fprintf(stderr, "couldn't get appropriate visual\n");
		return(-1);
	}

//	XAutoRepeatOff(x_disp);

	win_init=1;

	win_active = NULL;
	win_root = NULL;

	return(0);
}

displaywindow *Window_NewWindow(int width, int height) //AH:ignore
{
	displaywindow *tmp;
	struct windowdata_s *tmpd;
	GLXContext ctx;

	XSetWindowAttributes attr;
	unsigned long mask;
	Window win;

	if(!win_init)
	{
		printf("X support not inited.\n");
		return(NULL);
	}

	tmp=gcalloc(sizeof(displaywindow)+sizeof(struct windowdata_s));
	tmpd=(struct windowdata_s *)(tmp+1);
	tmp->viddata=tmpd;

	tmp->width=width;
	tmp->height=height;

	window_width=width;
	window_height=height;
	window_active=1;
	window_lastactive=1;
	window_fullscreen=0;
	
	window_max_width=width;
	window_max_height=height;

	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(x_disp, root, visinfo->visual, AllocNone);
	attr.event_mask = X_MASK;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	win = XCreateWindow(x_disp, root, 0, 0, width, height,
						0, visinfo->depth, InputOutput,
						visinfo->visual, mask, &attr);
	XMapWindow(x_disp, win);

	tmpd->win = win;

	XFlush(x_disp);

	ctx = glXCreateContext(x_disp, visinfo, NULL, True);

	tmpd->ctx = ctx;

	glXMakeCurrent(x_disp, win, ctx);

	tmpd->isglx=1;

	win_active = tmp;
	if(win_root) win_root->prev = tmp;
	tmp->next = win_root;
	win_root = tmp;

	return(tmp);
}

displaywindow *Window_NewWindow_NOGL(int width, int height) //AH:ignore
{
	displaywindow *tmp;
	struct windowdata_s *tmpd;

	XSetWindowAttributes attr;
	unsigned long mask;
	Window win;

	XGCValues xgcvalues;
	int valuemask, i;
	XEvent event;

	if(!win_init)
	{
		printf("X support not inited.\n");
		return(NULL);
	}

	tmp=gcalloc(sizeof(displaywindow)+sizeof(struct windowdata_s));
	tmpd=(struct windowdata_s *)(tmp+1);
	tmp->viddata=tmpd;

	tmp->width=width;
	tmp->height=height;

	/* window attributes */
	attr.background_pixel = 0;
	attr.border_pixel = 0;
	attr.colormap = XCreateColormap(x_disp, root, visinfo->visual, AllocNone);
	attr.event_mask = X_MASK;
	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	win = XCreateWindow(x_disp, root, 0, 0, width, height,
						0, visinfo->depth, InputOutput,
						visinfo->visual, mask, &attr);

	tmpd->win = win;

	valuemask = GCGraphicsExposures;
	xgcvalues.graphics_exposures = False;
	tmpd->gc = XCreateGC(x_disp, win, valuemask, &xgcvalues);

	XMapWindow(x_disp, win);

	/* wait for window ready */
/*	while (1)
	{
		XNextEvent(x_disp, &event);
		if (event.type == Expose && !event.xexpose.count)
			break;
	} */

	i = visinfo->depth / 8;
	if (i == 3) i = 4;
	tmpd->pixsz = i;
	tmpd->linesz = (width*i+7)&(~7);
	tmpd->buf = malloc(tmpd->linesz * height);
	tmpd->img = XCreateImage(x_disp, visinfo->visual, visinfo->depth, ZPixmap, 0, tmpd->buf, width, height, 32, 0);

	XFlush(x_disp);

	printf("created window nongl: w=%d h=%d depth=%d\n", width, height, visinfo->depth);

	tmpd->isglx=0;

	win_active = tmp;
	if(win_root) win_root->prev = tmp;
	tmp->next = win_root;
	win_root = tmp;

	return(tmp);
}

int Window_Destroy(displaywindow *wnd) //AH:ignore
{
	struct windowdata_s *tmpd;
	tmpd=wnd->viddata;
	if(tmpd->isglx)
	{
		glXDestroyContext(x_disp, tmpd->ctx);
		XDestroyWindow(x_disp, tmpd->win);
	}else
	{
		XDestroyImage(tmpd->img);
		XFreeGC(x_disp, tmpd->gc);
		XDestroyWindow(x_disp, tmpd->win);
	}
	if(wnd->next)wnd->next->prev=wnd->prev;
	if(wnd->prev)wnd->prev->next=wnd->next;
		else win_root=wnd->next;
	gcfree(wnd);
	return(0);
}

int Window_DeInit() //AH:ignore
{
	displaywindow *wcur, *wcur2;

	printf("Window DeInitialize.\n");

	XAutoRepeatOn(x_disp);

	wcur=win_root;
	while(wcur)
	{
		wcur2=wcur->next;
		Window_Destroy(wcur);
		wcur=wcur2;
	}
	XCloseDisplay(x_disp);
	win_init = 0;
	return(0);
}

int Window_SetWindow(displaywindow *wind) //AH:ignore
{
	struct windowdata_s *tmpd;

	win_active=wind;
	tmpd=wind->viddata;

	if(tmpd->isglx)
	{
		glXMakeCurrent(x_disp, tmpd->win, tmpd->ctx);

		glClearColor (1, 1, 1, 1);
//		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|
			GL_STENCIL_BUFFER_BIT);

		glCullFace (GL_FRONT_AND_BACK);
	}
}

static void HandleEvents(void);

int Window_BeginDrawing() //AH: ignore
{
	struct windowdata_s *tmpd;
	tmpd=win_active->viddata;

	HandleEvents();

//	printf("%d %d\n", mouse_x, mouse_y);

	if(tmpd->isglx)
	{
		glViewport (0, 0, win_active->width, win_active->height);

//		glClearColor (1, 1, 1, 1);
		glClearColor (0, 0, 0, 1);

//		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|
			GL_STENCIL_BUFFER_BIT);
	}
}

int Window_EndDrawing() //AH: ignore
{
	struct windowdata_s *tmpd;
	tmpd=win_active->viddata;

	if(tmpd->isglx)
	{
		glFinish();
//		glFlush();
		glXSwapBuffers(x_disp, tmpd->win);
	}
	else
	{
		XPutImage(x_disp, tmpd->win, tmpd->gc, tmpd->img, 0, 0, 0, 0, win_active->width, win_active->height);
		XSync(x_disp, False);
	}
}

int Window_RegisterKeyHandler(
	int (*handler)(displaywindow *wnd, int key)) //AH: ignore
{
	int i;
	for(i=0; i<16; i++)if(!key_handler[i])
	{
		key_handler[i]=handler;
		return(i);
	}
	return(-1);
}

int Window_RemoveKeyHandler(int hnd) //AH: ignore
{
	key_handler[hnd]=NULL;
	return(0);
}

byte *Window_GetKeyMap(displaywindow *wnd) //AH: ignore
{
	struct windowdata_s *tmpd;
	tmpd=wnd->viddata;
	return(tmpd->keymap);
}

static int SendKeyToHandlers(displaywindow *wnd, int key, int down) //AH: ignore
{
	int i;

//	printf("key: %d %d\n", key, down);

	GfxDev_Key_Event(key, down);

	for(i=0; i<16; i++)if(key_handler[i])
		key_handler[i](wnd, key);
	return(0);
}

static int HandleKeyEvent(XKeyEvent *ev,
	displaywindow *wnd, int press) //AH: ignore
{
	int key;
	char buf[64];
	KeySym keysym;
	struct windowdata_s *tmpd;

	tmpd=wnd->viddata;

	key=0;

	XLookupString(ev, buf, sizeof buf, &keysym, 0);

	switch(keysym)
	{
		case XK_KP_Page_Up: case XK_Page_Up:
			key=K_PGUP; break;
		case XK_KP_Page_Down: case XK_Page_Down:
			key=K_PGDN; break;
		case XK_KP_Home: case XK_Home:
			key=K_HOME; break;
		case XK_KP_End: case XK_End:
			key=K_END; break;
		case XK_KP_Left: case XK_Left:
			key=K_LEFTARROW; break;
		case XK_KP_Right: case XK_Right:
			key=K_RIGHTARROW; break;
		case XK_KP_Down: case XK_Down:
			key=K_DOWNARROW; break;
		case XK_KP_Up: case XK_Up:
			key=K_UPARROW;  break;
		case XK_Escape:
			key=K_ESCAPE; break;
		case XK_KP_Enter: case XK_Return:
			key=K_ENTER;  break;

		case XK_Tab: key=K_TAB;  break;
		case XK_F1: key=K_F1; break;
		case XK_F2: key=K_F2; break;
		case XK_F3: key=K_F3; break;
		case XK_F4: key=K_F4; break;
		case XK_F5: key=K_F5; break;
		case XK_F6: key=K_F6; break;
		case XK_F7: key=K_F7; break;
		case XK_F8: key=K_F8; break;
		case XK_F9: key=K_F9; break;
		case XK_F10: key=K_F10;  break;
		case XK_F11: key=K_F11;  break;
		case XK_F12: key=K_F12;  break;
		case XK_BackSpace: key=K_BACKSPACE; break;

		case XK_KP_Delete: case XK_Delete:
			key=K_DEL; break;
		case XK_Shift_L: case XK_Shift_R:
			key=K_SHIFT; break;
		case XK_Execute: case XK_Control_L: case XK_Control_R:
			key=K_CTRL;  break;

		case XK_Alt_L: case XK_Meta_L: 
		case XK_Alt_R: case XK_Meta_R:
			key=K_ALT; break;

		case XK_KP_Begin: key='5'; break;

		case XK_KP_Insert: case XK_Insert:
			key=K_INS; break;

		case XK_KP_Multiply: key='*'; break;
		case XK_KP_Add: key='+'; break;
		case XK_KP_Subtract: key='-'; break;
		case XK_KP_Divide: key='/'; break;

		default:
			key=*(unsigned char*)buf;
			if((key>='A')&&(key<='Z'))
				key=key-'A'+'a';
			break;
	} 

	if(press)
		tmpd->keymap[key/8]|=1<<(key%8);
		else tmpd->keymap[key/8]&=~(1<<(key%8));

//	if(press && (key<128))
		SendKeyToHandlers(wnd, key, press);

	return key;
}

static int CheckWheelEvent(Display *disp, XEvent *event) //AH:ignore
{
	XEvent event2;
	int i;
	
	if(XPending(disp))
	{
		XPeekEvent(disp, &event2);
		if((event2.type == ButtonRelease) &&
			(event2.xbutton.button == event->xbutton.button) &&
			(event2.xbutton.time == event->xbutton.time))
		{
			i=0;

			//button 4 is wheel up
			if (event->xbutton.button == Button4)
				{ i = 1; }
			//button 4 is wheel down
			if (event->xbutton.button == Button5)
				{ i = -1; }

			//eat event
			XNextEvent(disp, &event2);
			return(i);
		}
	}
	return(0);
}

static void HandleEvents(void) //AH: ignore
{
	XEvent event;
	KeySym ks;
	int i, b;
	struct windowdata_s *tmpd;
	displaywindow *wcur, *targwin;

	if (!x_disp)
		return;

	while (XPending(x_disp))
	{
		XNextEvent(x_disp, &event);

		/* figure where this goes */
		wcur=win_root;
		while(wcur)
		{
			tmpd=wcur->viddata;
			if(tmpd->win == event.xany.window)
				targwin=wcur;
			else
			{
				tmpd->mx=-1;
				tmpd->my=-1;
				tmpd->mb=0;
			}
			wcur=wcur->next;
		}
		tmpd=targwin->viddata;

		switch (event.type)
		{
		case KeyPress:
			HandleKeyEvent(&event.xkey, targwin, 1);
			break;

		case KeyRelease:
			HandleKeyEvent(&event.xkey, targwin, 0);
			break;

		case MotionNotify:
			tmpd->mx = event.xmotion.x;
			tmpd->my = event.xmotion.y;
//			printf("mouse: %d: %d %d\n", tmpd->win, tmpd->mx, tmpd->my);
			break;

		case ButtonPress:
			i=CheckWheelEvent(x_disp, &event);
			if(i>0)
			{
				GfxDev_Key_Event(K_MWHEELUP, -1);
				GfxDev_Key_Event(K_MWHEELUP, 0);
				break;
			}else if(i<0)
			{
				GfxDev_Key_Event(K_MWHEELDOWN, -1);
				GfxDev_Key_Event(K_MWHEELDOWN, 0);
				break;
			}

			b=-1;
			if (event.xbutton.button == 1)
				b = 0;
			else if (event.xbutton.button == 2)
				b = 1;
			else if (event.xbutton.button == 3)
				b = 2;
//			if (b>=0)
//				Key_Event(K_MOUSE1 + b, true);

			if(b>=0)
				tmpd->mb |= 1<<b;
			break;

		case ButtonRelease:
			b=-1;
			if (event.xbutton.button == 1)
				b = 0;
			else if (event.xbutton.button == 2)
				b = 1;
			else if (event.xbutton.button == 3)
				b = 2;
//			if (b>=0)
//				Key_Event(K_MOUSE1 + b, false);
			if(b>=0)
				tmpd->mb &= ~(1<<b);
			break;

		case CreateNotify :
			win_x = event.xcreatewindow.x;
			win_y = event.xcreatewindow.y;
			break;

		case ConfigureNotify :
			win_x = event.xconfigure.x;
			win_y = event.xconfigure.y;
			break;
		}
	}
}

int Window_GetMousePos(displaywindow *wind, int *mx, int *my, int *mb) //AH:ignore
{
	struct windowdata_s *tmpd;
	tmpd=wind->viddata;

	*mx=tmpd->mx;
	*my=tmpd->my;
	*mb=tmpd->mb;
	if(*mx==-1)return(-1);
	return(0);
}

int Window_PollKey(displaywindow *wnd, int key) //AH:ignore
{
	struct windowdata_s *tmpd;
	tmpd=wnd->viddata;

	return(tmpd->keymap[key/8]&(1<<(key%8)));
}

byte *Window_GetWindowBuffer(displaywindow *wind) //AH:ignore
{
	struct windowdata_s *tmpd;

	win_active=wind;
	tmpd=wind->viddata;
	return(tmpd->buf);
}

int Window_DrawFromBuf(int x, int y, int w, int h,
	int bx, int by, int bw, int bh,
	displaywindow *wnd, byte *buf) //AH:ignore
{
	struct windowdata_s *tmpd;
	byte *scrbuf, *ibuf, *obuf;
	int i, j;

	tmpd=wnd->viddata;
	if(tmpd->isglx)return(-1);
	scrbuf=Window_GetWindowBuffer(wnd);

	for(i=0; i<h; i++)
	{
		ibuf=buf+((i*w)*4);
		obuf=scrbuf+((i+y)*tmpd->linesz)+(x*tmpd->pixsz);
		for(j=0; j<w; j++)
		{
			*obuf++=*ibuf++;
			*obuf++=*ibuf++;
			*obuf++=*ibuf++;
			ibuf++;
			obuf++;
		}
	}
}

void GfxDrv_InitGL()
{
//	gl_vendor=(char *)glGetString(GL_VENDOR);
//	gl_renderer=(char *)glGetString(GL_RENDERER);
//	gl_version=(char *)glGetString(GL_VERSION);
//	gl_extensions=(char *)glGetString(GL_EXTENSIONS);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH);

	glClearColor(0, 0, 0, 1);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);

	glAlphaFunc(GL_GREATER, 0.75);
	glDisable(GL_ALPHA_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


PDGL_API void GfxDrv_GetWindowSize(int *xs, int *ys)
	{ *xs=gfx_screen->width; *ys=gfx_screen->height; }

PDGL_API void GfxDrv_BeginDrawing()
{
	Window_BeginDrawing();
//	memset(gfxdev_buf, 0, 640*480*4);
}

PDGL_API void GfxDrv_EndDrawing()
{
	Window_EndDrawing();
}

PDGL_API void GfxDrv_SetDefaults(char *label, int width, int height)
{
	if(label)window_def_label=label;
	window_def_width=width;
	window_def_height=height;
}

PDGL_API int GfxDrv_Start()
{
	gfx_screen=Window_NewWindow(
		window_def_width, window_def_height);
	Window_SetWindow(gfx_screen);

	GfxDrv_InitGL();

//	Window_BeginDrawing();
//	gfxdev_buf=Window_GetWindowBuffer(gfx_screen);
//	Window_EndDrawing();
}

PDGL_API int GfxDrv_Init(void *hInstance, int nCmdShow, int *rkill)
{
	gfxdrv_rkill=rkill;
	Window_Init();
}

PDGL_API void GfxDrv_Shutdown()
{
	Window_DeInit();
}

int	mouse_lx, mouse_ly, mouse_lb;

// extern int window_center_x, window_center_y;
// extern int window_mouse_buttons;

PDGL_API int Mouse_GetPos(int *mx, int *my, int *mb)
{
	int x, y, b;

//	GetCursorPos (&mouse_pos);
//	SetCursorPos (window_center_x, window_center_y);
//	x=mouse_pos.x-window_center_x;
//	y=mouse_pos.y-window_center_y;
//	b=window_mouse_buttons;

	Window_GetMousePos(gfx_screen, &x, &y, &b);
	
	x-=400;
	y-=300;

	if ((x<=-400) || (x>=400) || (y<=-300) || (y>=300))
	{
		x=mouse_lx;
		y=mouse_ly;
//		b=mouse_lb;
	}

	*mx=x;
	*my=y;
	mouse_lx=x;
	mouse_ly=y;
	mouse_lb=b;

	*mb=b&7;

	return(0);
}

PDGL_API int Mouse_SetPos(int mx, int my)
{
	struct windowdata_s *tmpd;
	int x, y, b, w, h;

//	x=window_center_x+mx;
//	y=window_center_y+my;

	GfxDrv_GetWindowSize(&w, &h);

	tmpd=gfx_screen->viddata;

	XWarpPointer(x_disp, None, tmpd->win,
	 0, 0, 0, 0, w/2+mx, h/2+my);

//	SetCursorPos (x, y);
	mouse_lx = mx;
	mouse_ly = my;

	return(0);
}

#endif
