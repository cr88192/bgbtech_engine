#include <windows.h>

#include <pdgl.h>
#include <pdglui.h>

#ifndef PDGL_STUB_H
#define PDGL_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

int main_kill=0;
//int main_doshot=0;

char **pdgl_argv;
int pdgl_argc;

int pdgl_xs, pdgl_ys;
int pdgl_time, pdgl_dt;
ushort *pdgl_keybuf;

//int pdgl_mx, pdgl_my, pdgl_mb;	//mouse pos
//int pdgl_lmx, pdgl_lmy, pdgl_lmb;	//last most pos
//int pdgl_dmx, pdgl_dmy;		//mouse pos delta
//int pdgl_omx, pdgl_omy;		//last non-clicking mouse pos
//float pdgl_dt_f;			//current frame time
//float pdgl_adt_f;			//avg frame time
//float pdgl_adt2_f;			//avg frame time 2
//float pdgl_time_f;			//current time
int pdgl_maxhz=60;

// HINSTANCE	global_hInstance;
// int			global_nCmdShow;

PDGLUI_InputState *state;
PDGLUI_Camera *cam;

#ifdef __cplusplus
}
#endif

int pdgl_main(int argc, char **argv);

int pdgl_main_startup(int argc, char *argv[]);
int pdgl_main_body();
int pdgl_main_shutdown();

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char buf[256], *s, *t;
	int i, j, x, y, w, h;
	int ct, dt;
	char *argv[64];

//	global_hInstance = hInstance;
//	global_nCmdShow = nCmdShow;

	pdgl_argv=argv;

//	pdgl_argv[0]="";
//	pdgl_argc=1;
//	s=lpCmdLine;

	pdgl_argv[0]="";
	pdgl_argv[1]=NULL;
	pdgl_argc=0;

	s=GetCommandLineA();
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

//	GC22_StackBase(&nCmdShow);
//	MM22_BaseHeap(1<<26);
//	dy_init(&nCmdShow);
	dy_init(&hInstance);

	state=PDGLUI_GetState();
	cam=PDGLUI_GetCamera();

//	GfxDrv_Init(hInstance, nCmdShow, &main_kill);
	GfxDrv_Init(hInstance, nCmdShow, &(state->kill));
	GfxDrv_Start();

	printf("PDGL: Startup\n");

	i=pdgl_main_startup(pdgl_argc, pdgl_argv);

	if(i>0)
	{
		printf("PDGL: Main\n");

		ct=0;
		dt=PDGL_DeltaMS(&ct);
		while(1)
		{
			//framerate limiter
			i=ct;
			dt=PDGL_DeltaMS(&i);
			if((pdgl_maxhz>0) && (dt<(1000/pdgl_maxhz)))
			{
//				printf("dt %d %d\n", dt, ct);
				j=1000/(2*pdgl_maxhz);
				Sleep(j);
				continue;
			}

			ct=i;

//			printf("dt %d\n", dt);

			pdgl_time=ct;
			pdgl_dt=dt;
			GfxDrv_GetWindowSize(&state->xs, &state->ys);
			state->keys=PDGL_GetKeybuf();
//			PDGLUI_SetKeybuf(pdgl_keybuf, pdgl_dt/1000.0);

//			printf("kbuf %p\n", pdgl_keybuf);

			state->lmx=state->mx;
			state->lmy=state->my;
			state->lmb=state->mb;

			Mouse_GetPos(&state->mx, &state->my, &state->mb);

			state->dmx=state->mx-state->lmx;
			state->dmy=state->my-state->lmy;

			if(!(state->lmb&1))
			{
				state->omx=state->mx;
				state->omy=state->my;
			}

			state->dt_f=pdgl_dt/1000.0;
			state->adt_f=(state->adt_f*0.90)+(state->dt_f*0.10);
			state->adt2_f=(state->adt2_f*0.95)+(state->adt_f*0.05);
			state->time_f=pdgl_time/1000.0;

			GfxDrv_BeginDrawing();
			i=pdgl_main_body();

			if(state->doshot==1)
			{
				Tex_DoScreenshot();
				state->doshot=0;
			}

			if(state->doshot==2)
			{
//				Tex_DoScreenshotFrame();
				PDGL_AVI_DoScreenshotFrame(state->dt_f);
			}else
			{
				PDGL_AVI_NoCapture();
			}

			GfxDrv_EndDrawing();

			UI_Camera_SetDoShot(state->doshot);

			if((i<0) || state->kill)break;
		}

		printf("PDGL: Main Break\n");

		i=pdgl_main_shutdown();
	}

	PDGL_AVI_NoCapture();

	GfxDrv_VID_Shutdown();
	vf_umount(NULL);
	gc_deinit();
	return(i);
}

#endif