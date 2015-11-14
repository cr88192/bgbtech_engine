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

int window_center_x;
int window_center_y;
int window_mouse_buttons;
int window_width;
int window_height;

int window_def_width=800;
int window_def_height=600;
char *window_def_label="PDGL";

char *gl_vendor, *gl_renderer, *gl_version, *gl_extensions;

PDGL_API void GfxDrv_GetWindowSize(int *xs, int *ys)
	{ *xs=window_width; *ys=window_height; }

PDGL_API void GfxDrv_BeginDrawing()
{
	GfxDrv_V_HandleMessages();
	glViewport(0, 0, window_width, window_height);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}


PDGL_API void GfxDrv_EndDrawing(void)
{
	glFinish();
	SwapBuffers(maindc);
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
	window_center_x=cx+(width/2);
	window_center_y=cy+(height/2)+24;
	window_width=width;
	window_height=height;
}

int GfxDrv_V_SetupWindow()
{
	static DWORD ws=
		WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX;
	HDC	hdc;
	RECT rect;
	int	lastmodestate, width, height;

	mainrect.top=mainrect.left=0;
	mainrect.right=window_def_width;
	mainrect.bottom=window_def_height;

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

	hdc=GetDC(mainwindow);
	PatBlt(hdc, 0, 0, mainrect.right, mainrect.bottom, BLACKNESS);
	ReleaseDC(mainwindow, hdc);

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
	gl_vendor=(char *)glGetString(GL_VENDOR);
	gl_renderer=(char *)glGetString(GL_RENDERER);
	gl_version=(char *)glGetString(GL_VERSION);
	gl_extensions=(char *)glGetString(GL_EXTENSIONS);

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

PDGL_API void GfxDrv_Shutdown()
{
   	HGLRC hrc;
   	HDC	  hdc;

	hrc=wglGetCurrentContext();
	hdc=wglGetCurrentDC();
	wglMakeCurrent(NULL, NULL);
	if(hrc)
		wglDeleteContext(hrc);
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

LONG WINAPI GfxDrv_MainWndProc(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) //AH:ignore
{
	LONG ret;
	int xp, yp;
	int i;

	ret=1;
	switch(uMsg)
	{
		case WM_CREATE:
			break;
		case WM_SYSCHAR:
			break;
		case WM_ACTIVATE:
			break;

		case WM_MOVE:
			xp=(int) LOWORD(lParam);
			yp=(int) HIWORD(lParam);
			window_center_x=xp+(window_width/2);
			window_center_y=yp+(window_height/2);
			break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
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
			window_mouse_buttons=i;
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

PDGL_API int GfxDrv_Start()
{
	PIXELFORMATDESCRIPTOR pfd;
	WNDCLASS wc;
	HDC hdc;
	int i;

//	MessageBox(NULL, "GL Init failed", "Error", MB_OK);

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

	i=ChoosePixelFormat(maindc, &pfd);
	if(!i)return(-1);
	i=SetPixelFormat(maindc, i, &pfd);
	if(!i)return(-1);

	mainrc=wglCreateContext( maindc );
	if(!mainrc)return(-1);

	i=wglMakeCurrent(maindc, mainrc);
	if(!i)return(-1);

	SetCursor(NULL);
	GfxDrv_InitGL();
}

PDGL_API int GfxDrv_Init(void *hInstance, int nCmdShow, int *rkill)
{
	gfxdrv_hInstance=hInstance;
	gfxdrv_nCmdShow=nCmdShow;
	gfxdrv_rkill=rkill;
}

#endif

