#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#define PDGL_BASEHEAP		(64*1024*1024)
#define PDGL_BASEHEAP_LIMIT	(256*1024*1024)
#define PDGL_BASEHEAP_LOBJ	(1024*1024*1024)
#define PDGL_BASEHEAP_CONS	(64*1024*1024)

#define PDGL_PRESTART

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbsvm.h>

int pdgl_main_prestart(int argc, char *argv[])
{
	char tb[256];
	char *opts[4];
	char *s;
	void *p;
	int xs, ys;
	int i, j, k, l;

	xs=1280; ys=768;

//	n_games=0; start_map=NULL;
	for(i=1; i<argc; i++)
	{
		if(!strcmp(argv[i], "-width"))
			xs=atoi(argv[i+1]);
		if(!strcmp(argv[i], "-height"))
			ys=atoi(argv[i+1]);
	}

	Con_Init();
	ConCmds_Init();

	btCvarSet("r_title", "BGBTech Anim2D");
	btCvarSetf("r_width", xs);
	btCvarSetf("r_height", ys);

	Con_ExecCmdBuf("anim2d/default.cfg");
	Con_DoExecCmds();

	Con_ExecCmdBuf("anim2d/config.cfg");
	Con_DoExecCmds();

	s=btCvarGet("r_title");
	xs=btCvarGetf("r_width");
	ys=btCvarGetf("r_height");

	GfxDrv_SetDefaults(s, xs, ys);
//	Con_ReInit();
}

int pdgl_main_startup(int argc, char *argv[])
{
	vf_mount("resource", "/", "dir", NULL);
	vf_mount("resource_dev", "/", "dir", NULL);
	vf_mount("../resource", "/", "dir", NULL);
	vf_mount("../resource_dev", "/", "dir", NULL);

	GfxFont_Init();

	PDGLUI_Init();
	UI_Camera_Init();

	BTA2D_Init();

	Con_ExecCmdBuf("anim2d/autoexec.cfg");

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

	if(BTA2D_HandleInput())
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
//	PD3D_DrawGrid(0, 0, 16, 1);

	UI_Camera_Setup2D();

	BTA2D_Draw2D();

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
