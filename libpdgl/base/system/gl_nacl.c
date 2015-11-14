#include <pdgl.h>

#ifdef NATIVECLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int win_init = 0;

int mouse_x, mouse_y;
int win_x, win_y;

int	*gfxdrv_rkill;

//displaywindow *win_active, *win_root;
//int (*key_handler[16])(displaywindow *wnd, int key);

//displaywindow *gfx_screen;
// byte *gfxdev_buf;

int window_def_width=800;
int window_def_height=600;
char *window_def_label="PDGL";

void GfxDrv_InitGL()
{
//	gl_vendor=(char *)glGetString(GL_VENDOR);
//	gl_renderer=(char *)glGetString(GL_RENDERER);
//	gl_version=(char *)glGetString(GL_VERSION);
//	gl_extensions=(char *)glGetString(GL_EXTENSIONS);

//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//	glShadeModel(GL_SMOOTH);

	glClearColor(0, 0, 0, 1);
	glCullFace(GL_FRONT);
//	glEnable(GL_TEXTURE_2D);

//	glAlphaFunc(GL_GREATER, 0.75);
//	glDisable(GL_ALPHA_TEST);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


PDGL_API void GfxDrv_GetWindowSize(int *xs, int *ys)
	{ }

PDGL_API void GfxDrv_BeginDrawing()
{
//	Window_BeginDrawing();
}

PDGL_API void GfxDrv_EndDrawing()
{
//	Window_EndDrawing();
}

PDGL_API void GfxDrv_SetDefaults(char *label, int width, int height)
{
	if(label)window_def_label=label;
	window_def_width=width;
	window_def_height=height;
}

PDGL_API int GfxDrv_Start()
{
//	gfx_screen=Window_NewWindow(
//		window_def_width, window_def_height);
//	Window_SetWindow(gfx_screen);

	GfxDrv_InitGL();

//	Window_BeginDrawing();
//	gfxdev_buf=Window_GetWindowBuffer(gfx_screen);
//	Window_EndDrawing();
}

PDGL_API int GfxDrv_Init(void *hInstance, int nCmdShow, int *rkill)
{
	gfxdrv_rkill=rkill;
//	Window_Init();
}

PDGL_API void GfxDrv_Shutdown()
{
//	Window_DeInit();
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

//	Window_GetMousePos(gfx_screen, &x, &y, &b);
	
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
//	struct windowdata_s *tmpd;
	int x, y, b, w, h;

//	x=window_center_x+mx;
//	y=window_center_y+my;

//	GfxDrv_GetWindowSize(&w, &h);

//	tmpd=gfx_screen->viddata;

//	XWarpPointer(x_disp, None, tmpd->win,
//	 0, 0, 0, 0, w/2+mx, h/2+my);

//	SetCursorPos (x, y);
	mouse_lx = mx;
	mouse_ly = my;

	return(0);
}

#endif
