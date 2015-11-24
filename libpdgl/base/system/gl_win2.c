#include <pdgl.h>

#ifdef _WIN32

#include <windows.h>
#include <commctrl.h>

#include <GL/GL.h>

HWND	mainwindow;
HGLRC	mainrc;
HDC		maindc;
RECT	mainrect;

HINSTANCE gfxdrv_hInstance;
int gfxdrv_nCmdShow;
int	*gfxdrv_rkill;

volatile int gfxdrv_waitdc=0;
volatile int gfxdrv_waitok=0;

void *gfxdrv_mutex=NULL;

int window_center_x;
int window_center_y;
int window_mouse_buttons;
int window_width;
int window_height;
int window_active;
int window_lastactive;
int window_fullscreen;

int window_def_width=800;
int window_def_height=600;
char *window_def_label="PDGL";
int window_def_fullscreen=0;
int window_fullscreen_width=0;
int window_fullscreen_height=0;
int window_desktop_width=1680;
int window_desktop_height=1050;
int window_max_width=0;
int window_max_height=0;

char *pdgl_vendor, *pdgl_renderer, *pdgl_version, *pdgl_extensions;


BOOL (APIENTRY *pwglCopyContext)(HGLRC hglrc1, HGLRC hglrc2, UINT mask);
HGLRC (APIENTRY *pwglCreateContext)(HDC hdc);
BOOL (APIENTRY *pwglDeleteContext)(HGLRC hglrc);
HGLRC (APIENTRY *pwglGetCurrentContext)(void);
HDC (APIENTRY *pwglGetCurrentDC)(void);
BOOL (APIENTRY *pwglMakeCurrent)(HDC hdc, HGLRC hglrc);
BOOL (APIENTRY *pwglShareLists)(HGLRC a0, HGLRC a1);

int (APIENTRY *pwglChoosePixelFormat)(
	HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd);
int (APIENTRY *pwglGetPixelFormat)(HDC hdc);
BOOL (APIENTRY *pwglSetPixelFormat)(
	HDC hdc, INT iPixelFormat, const PIXELFORMATDESCRIPTOR *ppfd);
int (APIENTRY *pwglDescribePixelFormat)(
	HDC hdc, INT iPixelFormat, UINT nBytes, LPPIXELFORMATDESCRIPTOR ppfd);
BOOL (APIENTRY *pwglSwapBuffers)(HDC  hdc);


BOOL pdwglCopyContext(
		HGLRC hglrc1, HGLRC hglrc2, UINT mask) //AH:ignore
{
	if(pwglCopyContext)
		{ return(pwglCopyContext(hglrc1, hglrc2, mask)); }
	pwglCopyContext=pdglGetProcAddress("wglCopyContext");
	return(pwglCopyContext(hglrc1, hglrc2, mask));
}

HGLRC pdwglCreateContext(HDC hdc) //AH:ignore
{
	if(pwglCreateContext)
		{ return(pwglCreateContext(hdc)); }
	pwglCreateContext=pdglGetProcAddress("wglCreateContext");
	return(pwglCreateContext(hdc));
}

BOOL pdwglDeleteContext(HGLRC hglrc) //AH:ignore
{
	if(pwglDeleteContext)
		{ return(pwglDeleteContext(hglrc)); }
	pwglDeleteContext=pdglGetProcAddress("wglDeleteContext");
	return(pwglDeleteContext(hglrc));
}

HGLRC pdwglGetCurrentContext(void) //AH:ignore
{
	if(pwglGetCurrentContext)
		{ return(pwglGetCurrentContext()); }
	pwglGetCurrentContext=pdglGetProcAddress("wglGetCurrentContext");
	return(pwglGetCurrentContext());
}

HDC pdwglGetCurrentDC(void) //AH:ignore
{
	if(pwglGetCurrentDC)
		{ return(pwglGetCurrentDC()); }
	pwglGetCurrentDC=pdglGetProcAddress("wglGetCurrentDC");
	return(pwglGetCurrentDC());
}

BOOL pdwglMakeCurrent(HDC hdc, HGLRC hglrc) //AH:ignore
{
	if(pwglMakeCurrent)
		{ return(pwglMakeCurrent(hdc, hglrc)); }
	pwglMakeCurrent=pdglGetProcAddress("wglMakeCurrent");
	return(pwglMakeCurrent(hdc, hglrc));
}

BOOL pdwglShareLists(HGLRC a0, HGLRC a1) //AH:ignore
{
	if(pwglShareLists)
		{ return(pwglShareLists(a0, a1)); }
	pwglShareLists=pdglGetProcAddress("wglShareLists");
	return(pwglShareLists(a0, a1));
}

int pdwglChoosePixelFormat(
	HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd) //AH:ignore
{
	if(pwglChoosePixelFormat)
		{ return(pwglChoosePixelFormat(hdc, ppfd)); }
	if(!pdglGlDriverIsGL())
		pwglChoosePixelFormat=pdglGetProcAddress("wglChoosePixelFormat");
	if(!pwglChoosePixelFormat)
		pwglChoosePixelFormat=pdglGetProcAddress("ChoosePixelFormat");
	return(pwglChoosePixelFormat(hdc, ppfd));
}

int pdwglGetPixelFormat(HDC hdc) //AH:ignore
{
	if(pwglGetPixelFormat)
		{ return(pwglGetPixelFormat(hdc)); }
	if(!pdglGlDriverIsGL())
		pwglGetPixelFormat=pdglGetProcAddress("wglGetPixelFormat");
	if(!pwglGetPixelFormat)
		pwglGetPixelFormat=pdglGetProcAddress("GetPixelFormat");
	return(pwglGetPixelFormat(hdc));
}

BOOL pdwglSetPixelFormat(
	HDC hdc, INT iPixelFormat, const PIXELFORMATDESCRIPTOR *ppfd) //AH:ignore
{
	if(pwglSetPixelFormat)
		{ return(pwglSetPixelFormat(hdc, iPixelFormat, ppfd)); }
	if(!pdglGlDriverIsGL())
		pwglSetPixelFormat=pdglGetProcAddress("wglSetPixelFormat");
	if(!pwglSetPixelFormat)
		pwglSetPixelFormat=pdglGetProcAddress("SetPixelFormat");
	return(pwglSetPixelFormat(hdc, iPixelFormat, ppfd));
}

int pdwglDescribePixelFormat(
	HDC hdc, INT iPixelFormat, UINT nBytes,
	LPPIXELFORMATDESCRIPTOR ppfd) //AH:ignore
{
	if(pwglDescribePixelFormat)
		{ return(pwglDescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd)); }
	if(!pdglGlDriverIsGL())
		pwglDescribePixelFormat=pdglGetProcAddress("wglDescribePixelFormat");
	if(!pwglDescribePixelFormat)
		pwglDescribePixelFormat=pdglGetProcAddress("DescribePixelFormat");
	return(pwglDescribePixelFormat(hdc, iPixelFormat, nBytes, ppfd));
}

BOOL pdwglSwapBuffers(HDC hdc) //AH:ignore
{
	if(pwglSwapBuffers)
		{ return(pwglSwapBuffers(hdc)); }
	if(!pdglGlDriverIsGL())
		pwglSwapBuffers=pdglGetProcAddress("wglSwapBuffers");
	if(!pwglSwapBuffers)
		pwglSwapBuffers=pdglGetProcAddress("SwapBuffers");
	return(pwglSwapBuffers(hdc));
}


void gfxdrv_lock()
{
	thLockFastMutex(gfxdrv_mutex);
}

void gfxdrv_unlock()
{
	thUnlockFastMutex(gfxdrv_mutex);
}

static int gfxdrv_log2up(int v)
{
	int i, j;
	
	i=v; j=0;
	while(i>1) { i=(i+1)>>1; j++; }
	return(j);
}

PDGL_API void GfxDrv_GetWindowSize(int *xs, int *ys)
	{ *xs=window_width; *ys=window_height; }

PDGL_API void GfxDrv_GetWindowTexSize(int *xs, int *ys)
{
	*xs=1<<gfxdrv_log2up(window_width);
	*ys=1<<gfxdrv_log2up(window_height);
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

PDGL_API void GfxDrv_BeginDrawing()
{
	GfxDrv_V_HandleMessages();
	pdglViewport(0, 0, window_width, window_height);
	pdglClearColor(0, 0, 0, 1);
	pdglClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}


PDGL_API void GfxDrv_EndDrawing(void)
{
	pdglFinish();
//	SwapBuffers(maindc);
	pdwglSwapBuffers(maindc);

	window_lastactive=window_active;

	if(gfxdrv_waitdc)
	{
		gfxdrv_lock();
		pdwglMakeCurrent(NULL, NULL);
		gfxdrv_waitok=1;
		gfxdrv_unlock();

		while(gfxdrv_waitdc>0)
			thSleep(10);
		gfxdrv_waitok=0;
		thSleep(10);

		gfxdrv_lock();
		pdwglMakeCurrent(maindc, mainrc);
		gfxdrv_unlock();
	}
}

void GfxDrv_V_HandleMessages()
{
	MSG	msg;

	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void GfxDrv_V_CenterWindow(HWND hWnd,
	int width, int height) //AH:ignore
{
	RECT rect;
	int cx, cy;

	cx=GetSystemMetrics(SM_CXSCREEN);
	cy=GetSystemMetrics(SM_CYSCREEN);
	cx=(cx-width)/2; cy=(cy-height)/2;
	if(cx>cy*2)cx/=2;
	if(cx<0)cx=0;
	if(cy<0)cy=0;
	SetWindowPos(hWnd, NULL, cx, cy, 0, 0,
		SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_DRAWFRAME);
//	SetWindowPos(hWnd, NULL, cx, cy, width, height,
//		SWP_NOZORDER|SWP_SHOWWINDOW|SWP_DRAWFRAME);
	window_center_x=cx+(width/2);
	window_center_y=cy+(height/2)+24;
	window_width=width;
	window_height=height;
}

int GfxDrv_V_SetWindowedMode()
{
	static DWORD ws=
		WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
	RECT rect;

	ChangeDisplaySettings (NULL, 0);
	window_fullscreen=0;

	SetWindowLong(mainwindow, GWL_STYLE, ws);
	SetWindowPos(mainwindow, NULL, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|
		SWP_DRAWFRAME|SWP_FRAMECHANGED);

	rect.top=rect.left=0;
	rect.right=window_def_width;
	rect.bottom=window_def_height;

	AdjustWindowRectEx(&rect, ws, FALSE, 0);

	window_width=rect.right-rect.left;
	window_height=rect.bottom-rect.top;

	MoveWindow(mainwindow, 0, 0, window_width, window_height, TRUE );
		
	GfxDrv_V_CenterWindow(mainwindow, window_width, window_height);
}

int GfxDrv_V_GetDesktopSize()
{
	DEVMODE devmode;

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
	window_desktop_width=devmode.dmPelsWidth;
	window_desktop_height=devmode.dmPelsHeight;
	
	if(window_fullscreen_width<=0)
	{
		window_fullscreen_width=window_desktop_width;
		window_fullscreen_height=window_desktop_height;
	}
	
	window_max_width=window_def_width;
	window_max_height=window_def_height;
	if(window_fullscreen_width>window_max_width)
		{ window_max_width=window_fullscreen_width; }
	if(window_fullscreen_height>window_max_height)
		{ window_max_height=window_fullscreen_height; }
}

int GfxDrv_V_SetFullscreenMode()
{
	DEVMODE devmode;
	RECT rect;
	DWORD ws;
	int width, height;
	int i;

//	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devmode);
//	width=devmode.dmPelsWidth;
//	height=devmode.dmPelsHeight;

//	width=window_def_width;
//	height=window_def_height;
	width=window_fullscreen_width;
	height=window_fullscreen_height;

	devmode.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
	devmode.dmBitsPerPel=32;
	devmode.dmPelsWidth=width;
	devmode.dmPelsHeight=height;
	devmode.dmSize=sizeof(devmode);

	i=ChangeDisplaySettingsEx(NULL, &devmode, NULL, CDS_FULLSCREEN, NULL);

//	if(ChangeDisplaySettings(&devmode, CDS_FULLSCREEN)!= 
//		DISP_CHANGE_SUCCESSFUL)
	if(i!=DISP_CHANGE_SUCCESSFUL)
	{
		printf("GfxDrv_V_SetFullscreenMode: Failed To Set Fullscreen\n");
		window_fullscreen=0;
		return(-1);
	}

	rect.top=rect.left=0;
	rect.right=width;
	rect.bottom=height;

	ws=WS_POPUP;
	AdjustWindowRectEx(&rect, ws, FALSE, 0);

	SetWindowLong(mainwindow, GWL_STYLE, ws);
	SetWindowPos(mainwindow, NULL, 0, 0, 0, 0,
		SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_FRAMECHANGED);

	window_width=rect.right-rect.left;
	window_height=rect.bottom-rect.top;

	MoveWindow(mainwindow, 0, 0, window_width, window_height, TRUE );

	window_center_x=window_width/2;
	window_center_y=window_height/2;

	return(1);
}

void GfxDrv_UpdateMode()
{
#if 0
	gfxdrv_lock();
	pdwglMakeCurrent(NULL, NULL);
//	gfxdrv_waitok=1;
	gfxdrv_unlock();

	if(maindc && mainwindow)
	{
		ReleaseDC(mainwindow, maindc);
		maindc=NULL;
	}
#endif

	if(window_fullscreen)
	{
		GfxDrv_V_SetFullscreenMode();
	}else
	{
		GfxDrv_V_SetWindowedMode();
	}

#if 0
	GfxDrv_SetupMainDC();

	gfxdrv_lock();
	pdwglMakeCurrent(maindc, mainrc);
	gfxdrv_unlock();
#endif
}

int GfxDrv_V_SetupWindow()
{
//	static DWORD ws=
//		WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
	static DWORD ws=WS_POPUP;

	HDC	hdc;
	RECT rect;
	int	lastmodestate, width, height;

	GfxDrv_V_GetDesktopSize();

	mainrect.top=mainrect.left=0;
//	mainrect.right=window_def_width;
//	mainrect.bottom=window_def_height;
//	mainrect.right=window_desktop_width;
//	mainrect.bottom=window_desktop_height;
	mainrect.right=window_max_width;
	mainrect.bottom=window_max_height;

	rect=mainrect;
	AdjustWindowRectEx(&rect, ws, FALSE, 0);

	width=rect.right-rect.left;
	height=rect.bottom-rect.top;

	mainwindow=CreateWindowEx(
		0,
		"bgbui",
		window_def_label,
		ws,
		rect.left, rect.top,
		width, height,
		NULL, NULL,
		gfxdrv_hInstance,
		NULL);

	GfxDrv_V_CenterWindow(mainwindow,
		mainrect.right-mainrect.left,
		mainrect.bottom-mainrect.top);

	ShowWindow(mainwindow, SW_SHOWDEFAULT);
	UpdateWindow(mainwindow);

//	hdc=GetDC(mainwindow);
//	PatBlt(hdc, 0, 0, mainrect.right, mainrect.bottom, BLACKNESS);
//	ReleaseDC(mainwindow, hdc);

	GfxDrv_V_HandleMessages();
	Sleep(100);
	SetWindowPos(mainwindow, HWND_TOP, 0, 0, 0, 0,
				  SWP_DRAWFRAME | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
				  SWP_NOCOPYBITS);
	SetForegroundWindow(mainwindow);
	return(-1);
}

void GfxDrv_InitGL()
{
	char tb[1024];
	char *s;

	pdgl_vendor=(char *)pdglGetString(GL_VENDOR);
	pdgl_renderer=(char *)pdglGetString(GL_RENDERER);
	pdgl_version=(char *)pdglGetString(GL_VERSION);
	pdgl_extensions=(char *)pdglGetString(GL_EXTENSIONS);

	printf("OpenGL Vendor=%s\n", pdgl_vendor);
	printf("OpenGL Renderer=%s\n", pdgl_renderer);
	printf("OpenGL Version=%s\n", pdgl_version);
//	printf("OpenGL Extensions=%s\n", pdgl_extensions);

	BGBBTJ_SetGlExtensions(pdgl_extensions);

	s=pdgl_extensions;
	printf("OpenGL Extensions=<[[\n");
	while(strlen(s)>=1024)
	{
		memcpy(tb, s, 1020);
		tb[1020]=0;
		printf("%s", tb);
		s+=1020;
	}
	printf("%s ]]>\n\n", s);

	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	pdglShadeModel(GL_SMOOTH);

	pdglClearColor(0, 0, 0, 1);
	pdglCullFace(GL_FRONT);
	pdglEnable(GL_TEXTURE_2D);

	pdglAlphaFunc(GL_GREATER, 0.75);
	pdglDisable(GL_ALPHA_TEST);

	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void GfxDrv_InitGL2()
{
	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	pdglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	pdglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	pdglShadeModel(GL_SMOOTH);

	pdglClearColor(0, 0, 0, 1);
	pdglCullFace(GL_FRONT);
	pdglEnable(GL_TEXTURE_2D);

	pdglAlphaFunc(GL_GREATER, 0.75);
	pdglDisable(GL_ALPHA_TEST);

	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

PDGL_API void GfxDrv_Shutdown()
{
   	HGLRC hrc;
   	HDC	  hdc;

	if(window_fullscreen)
		{ ChangeDisplaySettings (NULL, 0); }

	hrc=pdwglGetCurrentContext();
	hdc=pdwglGetCurrentDC();
	pdwglMakeCurrent(NULL, NULL);
	if(hrc)
		pdwglDeleteContext(hrc);
	if(hdc && mainwindow)
		ReleaseDC(mainwindow, hdc);
	if(maindc && mainwindow)
		ReleaseDC(mainwindow, maindc);
}

static int scantokey[128]=
{
	0, K_ESC, '1','2','3','4','5','6', 
	'7','8','9','0','-','=', K_BACKSPACE, 9,
	'q','w','e','r','t','y','u','i', 
	'o','p','[',']',13 , K_CTRL, 'a','s',
	'd','f','g','h','j','k','l',';', 
	'\'', '`', K_SHIFT,'\\', 'z','x','c','v',
	'b','n','m',',','.','/', K_SHIFT,'*', 
	K_ALT, ' ', 0, K_F1, K_F2, K_F3, K_F4, K_F5,
	K_F6, K_F7, K_F8, K_F9, K_F10, K_PAUSE, 0,  K_HOME, 
	K_UPARROW, K_PGUP, '-', K_LEFTARROW,'5', K_RIGHTARROW, '+', K_END,
	K_DOWNARROW, K_PGDN, K_INS, K_DEL, 0, 0, 0,
	K_F11, 
	K_F12, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0
}; 

int GfxDrv_MapKey(int key)
{
	key=(key>>16)&255;
	if(key>127)
		return 0;
//	if(!scantokey[key])
//		printf("no mapping for key 0x%02X\n", key);
	return(scantokey[key]);
}

void GfxDrv_AppActivate(bool active, bool minimized)
{
	window_active=active&&(!minimized);

	if(window_fullscreen)
	{
		if(window_active)
		{
			GfxDrv_V_SetFullscreenMode();
		}else
		{
			ChangeDisplaySettings(NULL, 0);
		}
	}
}

LONG WINAPI GfxDrv_MainWndProc(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) //AH:ignore
{
	LONG ret;
	int xp, yp;
	int i, j;

	ret=1;
	switch(uMsg)
	{
		case WM_CREATE:
			break;
		case WM_SYSCHAR:
			break;
		case WM_ACTIVATE:
			i = LOWORD(wParam);
			j = (BOOL) HIWORD(wParam);

			GfxDrv_AppActivate(i!=WA_INACTIVE, j);
			break;

		case WM_MOVE:
			if(!window_fullscreen)
			{
				xp=(int) LOWORD(lParam);
				yp=(int) HIWORD(lParam);
				window_center_x=xp+(window_width/2);
				window_center_y=yp+(window_height/2);
			}
			break;

		case WM_SYSKEYDOWN:
			if(wParam==0xFF)break;
			if(wParam==0x0D)
			{
				window_fullscreen=!window_fullscreen;
				GfxDrv_UpdateMode();
			}

			if(wParam==0x3E)
				{ if(gfxdrv_rkill)*gfxdrv_rkill=1; }

			GfxDev_Key_Event(GfxDrv_MapKey(lParam),-1);
			break;

		case WM_KEYDOWN:
//		case WM_SYSKEYDOWN:
			if(wParam==0xFF)break;
			GfxDev_Key_Event(GfxDrv_MapKey(lParam),-1);
			break;
			
		case WM_KEYUP:
		case WM_SYSKEYUP:
			if(wParam==0xFF)break;
			GfxDev_Key_Event(GfxDrv_MapKey(lParam), 0);
			break;

		case WM_LBUTTONDOWN:	case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:	case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:	case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
			i=0;
			if(wParam&MK_LBUTTON)i|=1;
			if(wParam&MK_RBUTTON)i|=4;
			if(wParam&MK_MBUTTON)i|=2;
			j=window_mouse_buttons^i;
			window_mouse_buttons=i;
			if(j&1)GfxDev_Key_Event(K_MOUSE1, (i&1)?(-1):0);
			if(j&2)GfxDev_Key_Event(K_MOUSE2, (i&2)?(-1):0);
			if(j&4)GfxDev_Key_Event(K_MOUSE3, (i&4)?(-1):0);
			SetCursor(NULL);
			break;

		case WM_MOUSEWHEEL: 
			if(((short)HIWORD(wParam))>0)
			{
				GfxDev_Key_Event(K_MWHEELUP, -1);
				GfxDev_Key_Event(K_MWHEELUP, 0);
			}else
			{
				GfxDev_Key_Event(K_MWHEELDOWN, -1);
				GfxDev_Key_Event(K_MWHEELDOWN, 0);
			}
			break;

		case WM_SIZE:
			xp=(int)LOWORD(lParam);
			yp=(int)HIWORD(lParam);
			window_width=xp;
			window_height=yp;
			break;

		case WM_CLOSE:
			if(gfxdrv_rkill)*gfxdrv_rkill=1;
			break;

		case WM_DESTROY:
			if(mainwindow)
				DestroyWindow(mainwindow);
			PostQuitMessage(0);
			break;
		default:
			ret=DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
	}
	return(ret);
}

PDGL_API void GfxDrv_SetDefaults(char *label, int width, int height)
{
	if(label)window_def_label=label;
	window_def_width=width;
	window_def_height=height;
}

PDGL_API void GfxDrv_SetDefaultFullscreen(int fs)
{
	window_def_fullscreen=fs;
}

PDGL_API void GfxDrv_SetFullscreenResolution(int width, int height)
{
	window_fullscreen_width=width;
	window_fullscreen_height=height;
}

PDGL_API int GfxDrv_SetupMainDC()
{
	PIXELFORMATDESCRIPTOR pfd;
	int i;

#if 1
	maindc=GetDC(mainwindow);

	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize=sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion=1;
	pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
	pfd.iPixelType=PFD_TYPE_RGBA;
	pfd.cColorBits=24;
	pfd.cDepthBits=32;
	pfd.cStencilBits=16;
	pfd.iLayerType=PFD_MAIN_PLANE;

	i=pdwglChoosePixelFormat(maindc, &pfd);
	if(!i)return(-1);
	i=pdwglSetPixelFormat(maindc, i, &pfd);
	if(!i)return(-1);
#endif
	return(0);
}

PDGL_API int GfxDrv_Start()
{
//	PIXELFORMATDESCRIPTOR pfd;
	WNDCLASS wc;
	HDC hdc;
	int i;

//	MessageBox(NULL, "GL Init failed", "Error", MB_OK);

	gfxdrv_mutex=thFastMutex();

	memset(&wc, 0, sizeof(WNDCLASS));
	wc.lpfnWndProc=(WNDPROC)GfxDrv_MainWndProc;
	wc.hInstance=gfxdrv_hInstance;
	wc.lpszClassName="bgbui";

	i=RegisterClass(&wc);
	if(!i)return(-1);

	hdc=GetDC(NULL);
	if(GetDeviceCaps(hdc, RASTERCAPS) & RC_PALETTE)
		return(-1);
	ReleaseDC(NULL, hdc);

	GfxDrv_V_SetupWindow();
	if(window_def_fullscreen)
	{
		window_fullscreen=1;
		GfxDrv_V_SetFullscreenMode();
	}

	GfxDrv_SetupMainDC();

	mainrc=pdwglCreateContext(maindc);
	if(!mainrc)return(-1);

	i=pdwglMakeCurrent(maindc, mainrc);
	if(!i)return(-1);

	SetCursor(NULL);
	GfxDrv_InitGL();

	if(!window_def_fullscreen)
	{
		GfxDrv_V_SetWindowedMode();
	}
}

PDGL_API int GfxDrv_SetupThreadShareLists()
{
	HGLRC	altrc;
	int i;

//	*(int *)-1=-1;

	gfxdrv_waitdc++;
	while(!gfxdrv_waitok)
		thSleep(10);

	gfxdrv_lock();

	altrc=pdwglCreateContext(maindc);
	if(!altrc)
	{
		gfxdrv_unlock();
		return(-1);
	}

	i=pdwglShareLists(mainrc, altrc);
	if(!i)
	{
		gfxdrv_unlock();
		return(-1);
	}

	i=pdwglMakeCurrent(maindc, altrc);
	if(!i)
	{
		gfxdrv_unlock();
		return(-1);
	}
	
	GfxDrv_InitGL2();
	
	gfxdrv_waitdc--;
	gfxdrv_unlock();

	return(0);
}

PDGL_API int GfxDrv_TeardownThreadShareLists()
{
   	HGLRC hrc;

	hrc=pdwglGetCurrentContext();
	pdwglMakeCurrent(NULL, NULL);
	if(hrc)
		pdwglDeleteContext(hrc);
}

PDGL_API int GfxDrv_Init(void *hInstance, int nCmdShow, int *rkill)
{
	gfxdrv_hInstance=hInstance;
	gfxdrv_nCmdShow=nCmdShow;
	gfxdrv_rkill=rkill;
}

#endif

