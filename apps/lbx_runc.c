// #include <windows.h>
// #include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbasm.h>
#include <bgbgc.h>
#include <bgbdy.h>
#include <bgbccc.h>


int pdgl_main_startup(int argc, char *argv[])
{
	char *uds[64], *args[256];
	void *p;
	int n, m, nargs;
	int i;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();
	UI_Camera_Init();

	nargs=0;
	args[nargs++]=argv[0];

	n=0; m=0;
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]=='-')
		{
			if(!strcmp(argv[i], "--"))
				{ i++; break; }
			if(!strcmp(argv[i], "--pc"))
				m|=1;
			continue;
		}
		uds[n++]=argv[i];
	}

	nargs=0; args[nargs++]=argv[0];
	for(; i<argc; i++)args[nargs++]=argv[i];

#ifdef WIN32
//	ccLoadLibrary("msvcrt");
#endif

//	for(i=0; i<n; i++)
//		ccLoadModule(uds[i]);

//	for(i=1; i<argc; i++)
//		ccLoadModule(argv[i]);

//	p=ccGetAddr("runc_start");
	if(p)
	{
		printf("Got %p\n", p);
		i=((int(*)(int argc, char **argv))p)(nargs, args);
		return(i);
	}

	return(1);
}

int main_handle_input()
{
	char buf[64];
	void *p;
	int i, j, k;

	if(Con_IsDown())
		return(0);

//	BS3D_HandleInput();

	if(PDGLUI_HasFocus())
		return(0);

//	p=ccGetAddr("runc_handle_input");
	if(p)
	{
		printf("Got %p\n", p);
		i=((int(*)())p)();
		if(i)return(i);
	}

	UI_Camera_Update();

	return(0);
}

int pdgl_main_body()
{
	void *p;
	int i, j, k, ret;

	ret=0;

	i=main_handle_input();
	if(i)ret=i;

	UI_Camera_Setup3D();

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
//	PD3D_DrawGrid(0, 0, 16, 1);

//	p=ccGetAddr("runc_draw3d");
	if(p)
	{
//		printf("Got %p\n", p);
		i=((int(*)())p)();
		if(i)ret=i;
	}

	UI_Camera_Setup2D();

//	p=ccGetAddr("runc_draw2d");
	if(p)
	{
//		printf("Got %p\n", p);
		i=((int(*)())p)();
		if(i)ret=i;
	}

	Con_Render();
	PDGLUI_Frame();

	pdglDisableTexture2D();
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(GL_LINES);
	pdglVertex2f(state->mx-10, -state->my);
	pdglVertex2f(state->mx+10, -state->my);
	pdglVertex2f(state->mx, -state->my-10);
	pdglVertex2f(state->mx, -state->my+10);
	pdglEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	void *p;
	int i;

//	p=ccGetAddr("runc_shutdown");
	if(p)
	{
		i=((int(*)())p)();
		return(i);
	}

	printf("shutdown\n");
	return(0);
}
