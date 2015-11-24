#include <pdgl.h>

#ifndef PDGL_STUB_H
#define PDGL_STUB_H

#ifdef __cplusplus
extern "C" {
#endif

int main_kill=0;
// int main_doshot=0;

char **pdgl_argv;
int pdgl_argc;

int pdgl_xs, pdgl_ys;
int pdgl_time, pdgl_dt;
ushort *pdgl_keybuf;

// int pdgl_mx, pdgl_my, pdgl_mb;		//mouse pos
// int pdgl_lmx, pdgl_lmy, pdgl_lmb;	//last most pos
// int pdgl_dmx, pdgl_dmy;			//mouse pos delta
// int pdgl_omx, pdgl_omy;			//last non-clicking mouse pos
// float pdgl_dt_f;
int pdgl_maxhz=90;

PDGLUI_InputState *state;
PDGLUI_Camera *cam;

#ifdef __cplusplus
}
#endif

int pdgl_main(int argc, char **argv);

int pdgl_main_startup(int argc, char *argv[]);
int pdgl_main_body();
int pdgl_main_shutdown();

// int main (int argc, char **argv)
int main_i()
{
	char buf[256], *s, *t;
	int i, j, x, y, w, h;
	int ct, dt;

//	pdgl_argc=argc;
//	pdgl_argv=argv;

#ifdef PDGL_BASEHEAP
	gcBaseHeap(PDGL_BASEHEAP);
#endif

#ifdef PDGL_BASEHEAP_LIMIT
	gcBaseHeapLimit(PDGL_BASEHEAP_LIMIT);
#endif

#ifdef PDGL_BASEHEAP_LOBJ
	gcBaseHeapLObj(PDGL_BASEHEAP_LOBJ);
#endif

#ifdef PDGL_BASEHEAP_CONS
	gcBaseHeapCons(PDGL_BASEHEAP_CONS);
#endif

#ifdef PDGL_PRESTART
	i=pdgl_main_prestart(pdgl_argc, pdgl_argv);
	if(i<0)
	{
		printf("pdgl_stub_lnx: prestart failed\n");
		return(-1);
	}
#endif

	state=PDGLUI_GetState();
	cam=PDGLUI_GetCamera();

//	GC22_StackBase(&argc);
//	MM22_BaseHeap(1<<26);

	GfxDrv_Init(NULL, NULL, &main_kill);
	GfxDrv_Start();

	printf("PDGL: Startup\n");

	i=pdgl_main_startup(pdgl_argc, pdgl_argv);

	if(i<0)
	{
		printf("pdgl_stub_lnx: startup failed\n");
	}

	if(i>=0)
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
//				Sleep(j);
				usleep(j);
//				continue;
			}

			ct=i;

//			printf("dt %d\n", dt);

			pdgl_time=ct;
			pdgl_dt=dt;
//			GfxDrv_GetWindowSize(&pdgl_xs, &pdgl_ys);
			GfxDrv_GetWindowSize(&state->xs, &state->ys);
			state->keys=PDGL_GetKeybuf();

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

//			pdgl_dt_f=pdgl_dt/1000.0;

			state->dt_f=pdgl_dt/1000.0;
			state->adt_f=(state->adt_f*0.90)+(state->dt_f*0.10);
			state->adt2_f=(state->adt2_f*0.95)+(state->adt_f*0.05);
			state->time_f=pdgl_time/1000.0;

			GfxDrv_BeginDrawing();
			i=pdgl_main_body();

//			if(main_doshot==1)
//			if(state->doshot==1)
			if((state->doshot&15)==1)
			{
				Tex_DoScreenshot();
//				main_doshot=0;
				state->doshot=0;
			}

			if(state->doshot&16)
			{
				Tex_DoScreenshot();
				state->doshot&=~16;
			}

//			if(main_doshot==2)
//			if(state->doshot==2)
			if((state->doshot&15)==2)
			{
//				Tex_DoScreenshotFrame();
//				PDGL_AVI_DoScreenshotFrame(pdgl_dt_f);
				PDGL_AVI_DoScreenshotFrame(state->dt_f, state->doshot);
			}else
			{
				PDGL_AVI_NoCapture(state->doshot);
			}

			GfxDrv_EndDrawing();

			UI_Camera_SetDoShot(state->doshot);

			if((i<0) || state->kill)break;
		}

		printf("PDGL: Main Break\n");

		i=pdgl_main_shutdown();
	}

	PDGL_AVI_NoCapture(state->doshot);

	PDGL_Shutdown();
	GfxDrv_Shutdown();
	vf_umount(NULL);

	return(i);
}

int main(int argc, char **argv)
{
	pdgl_argc=argc;
	pdgl_argv=argv;

	dy_init2(main_i);
	return(0);
}

#endif
