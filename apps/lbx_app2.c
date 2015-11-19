#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bs3d.h>


int pdgl_main_startup(int argc, char *argv[])
{
	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();
	BS3D_Init();

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

	BS3D_HandleInput();

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

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);
	PD3D_DrawGrid(0, 0, 16, 1);

	BS3D_DrawFrame3D();


	UI_Camera_Setup2D();

	BS3D_DrawFrame2D();
	Con_Render();
	PDGLUI_Frame();

	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(GL_LINES);
	pdglVertex2f(pdgl_mx-10, -pdgl_my);
	pdglVertex2f(pdgl_mx+10, -pdgl_my);
	pdglVertex2f(pdgl_mx, -pdgl_my-10);
	pdglVertex2f(pdgl_mx, -pdgl_my+10);
	pdglEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	printf("shutdown\n");
	return(0);
}
