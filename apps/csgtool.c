// #include <windows.h>
// #include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

// #include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbsvm.h>


int pdgl_main_startup(int argc, char *argv[])
{
	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();
	UI_Camera_Init();

	return(1);
}

int main_handle_input()
{
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	int i, j, k;
	unsigned short *kcur;

	if(Con_IsDown())
		return(0);

	if(PDGLUI_HasFocus())
		return(0);

	UI_Camera_Update();

	return(0);
}

int pdgl_main_body()
{
	static int t;
	char buf[64];

	int i, j, k;
	char *s;

	main_handle_input();

	UI_Camera_Setup3D();

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	PD3D_DrawGrid(0, 0, 16, 1);

	UI_Camera_Setup2D();

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
	printf("shutdown\n");
	return(0);
}