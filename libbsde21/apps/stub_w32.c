#ifdef _WIN32

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

int main_kill=0;

char **pdgl_argv;
int pdgl_argc;

int pdgl_xs, pdgl_ys;
int pdgl_time, pdgl_dt;
unsigned short *pdgl_keybuf;

int pdgl_mx, pdgl_my, pdgl_mb;		//mouse pos
int pdgl_lmx, pdgl_lmy, pdgl_lmb;	//last most pos
int pdgl_dmx, pdgl_dmy;			//mouse pos delta
int pdgl_omx, pdgl_omy;			//last non-clicking mouse pos
float pdgl_dt_f;

HINSTANCE	global_hInstance;
int			global_nCmdShow;

#ifdef __cplusplus
}
#endif

int pdgl_main(int argc, char **argv);

int pdgl_main_startup(int argc, char *argv[]);
int pdgl_main_body();
int pdgl_main_shutdown();

void GfxDrv_GetWindowSize(int *xs, int *ys);
void GfxDrv_BeginDrawing();
void GfxDrv_EndDrawing();
int GfxDrv_Init();
void GfxDrv_Start();
void GfxDrv_VID_Shutdown();

int PDGL_DeltaMS(int *l);
unsigned short *PDGL_GetKeybuf();
int Mouse_GetPos(int *mx, int *my, int *mb);

void panic(char *str)
{
	exit(-1);
}

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char buf[256], *s, *t;
	int i, x, y, w, h;
	int ct, dt;
	char *argv[64];

	global_hInstance = hInstance;
	global_nCmdShow = nCmdShow;

	pdgl_argv=argv;

	pdgl_argv[0]="";
	pdgl_argc=1;

	s=lpCmdLine;
	while(*s)
	{
		while(*s && (*s<=' '))s++;

		if(*s=='"')
		{

			s++;
			t=buf;
			while(*s && (*s!='"'))*t++=*s++;
			*t++=0;
			pdgl_argv[pdgl_argc++]=strdup(buf);
		}else if(*s)
		{
			t=buf;
			while((*s>' '))*t++=*s++;
			*t++=0;
			pdgl_argv[pdgl_argc++]=strdup(buf);
		}
	}

	GfxDrv_Init();
	GfxDrv_Start();

	i=pdgl_main_startup(pdgl_argc, pdgl_argv);

	if(i>=0)
	{
		ct=0;
		dt=PDGL_DeltaMS(&ct);
		while(!main_kill)
		{
			//framerate limiter
			i=ct;
			dt=PDGL_DeltaMS(&i);
			if(dt<(1000/60))	//eg: 60Hz
			{
				Sleep(0);
				continue;
			}
			ct=i;

			pdgl_time=ct;
			pdgl_dt=dt;
			GfxDrv_GetWindowSize(&pdgl_xs, &pdgl_ys);
			pdgl_keybuf=PDGL_GetKeybuf();

			pdgl_lmx=pdgl_mx;
			pdgl_lmy=pdgl_my;
			pdgl_lmb=pdgl_mb;

			Mouse_GetPos(&pdgl_mx, &pdgl_my, &pdgl_mb);

			pdgl_dmx=pdgl_mx-pdgl_lmx;
			pdgl_dmy=pdgl_my-pdgl_lmy;

			if(!(pdgl_lmb&1))
			{
				pdgl_omx=pdgl_mx;
				pdgl_omy=pdgl_my;
			}

			pdgl_dt_f=pdgl_dt/1000.0;

			GfxDrv_BeginDrawing();

			pdgl_main_body();

			GfxDrv_EndDrawing();
		}

		i=pdgl_main_shutdown();
	}

	GfxDrv_Shutdown();

	return(i);
}

#endif

