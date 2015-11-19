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


char *ctex_files[4096];
int ctex_nfiles;
int ctex_filepos;

int pdgl_main_startup(int argc, char *argv[])
{
	VFDIR *dir;
	char *s, *s1, *s2;
	int i;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();
	UI_Camera_Init();

	dir=vfopendir("textures/qrp_tex");
	if(!dir)return(-1);

	ctex_nfiles=0;
	ctex_filepos=0;

	while(1)
	{
		s=vfreaddir(dir);
		if(!s)break;

		s1=vfgetext(s);
		s2="tga;png";

		if(s1 && !vfcmpext(s1, s2))
			continue;

		if(s1)*(s1-1)=0;

		gc_printf("Texture %s\n", s);

		i=ctex_nfiles++;
		ctex_files[i]=dystring(s);
	}

	vfclosedir(dir);

	gc_printf("Found %d textures\n", ctex_nfiles);

	return(1);
}

int main_handle_input()
{
	char buf[64];
	void *p;
	int i, j, k;

	if(Con_IsDown())
		return(0);

	if(PDGLUI_HasFocus())
		return(0);

	UI_Camera_Update();

	return(0);
}

int pdgl_main_body()
{
	char tb[256];
	byte *buf0, *buf1, *buf2, *tbuf;
	void *p;
	int i, j, k, ret;
	int w, h, w1, h1, w2, h2;
	char *s;

	ret=0;

	i=main_handle_input();
	if(i)ret=i;

	if(ctex_filepos < ctex_nfiles)
	{
		i=ctex_filepos++;
		s=ctex_files[i];

		gc_printf("Processing %s\n", s);

		sprintf(tb, "textures/qrp_tex/%s.tga", s);
		buf0=Tex_LoadFileRaw(tb, &w, &h);

		sprintf(tb, "textures/q1tex/%s.png", s);
		buf1=Tex_LoadFileRaw(tb, &w1, &h1);

		sprintf(tb, "textures/qrp_tex/%s_luma.tga", s);
		buf2=Tex_LoadFileRaw(tb, &w2, &h2);

		if(!buf0 || !buf1)
		{
			gc_printf("Failed %s\n", s);
//			return(0);

//			if(buf0)
//			{
//				sprintf(tb, "textures/q1tex2/%s.png", s);
//				Tex_StoreFile(tb, buf0, w, h);
//				Tex_FreeRaw(buf0);
//			}
		}else
		{
			sprintf(tb, "textures/q1tex2/%s_alt.png", s);
			Tex_StoreFile(tb, buf0, w, h);

			memset(buf1, 0, w1*h1*4);

			tbuf=malloc(w*h*4);
			Tex_FilterBlur(buf0, tbuf, w, h, (w/w1)>>1);
			Tex_ResampleSpline(tbuf, w, h, buf1, w1, h1);
//			Tex_ResampleSinc(tbuf, w, h, buf1, w1, h1);
			free(tbuf);

			sprintf(tb, "textures/q1tex2/%s.png", s);
			Tex_StoreFile(tb, buf1, w1, h1);

			if(buf2)
			{
				tbuf=malloc(w2*h2*4);
				Tex_FilterBlur(buf2, tbuf, w, h, (w2/w1)>>1);
				Tex_ResampleSpline(tbuf, w2, h2, buf1, w1, h1);
//				Tex_ResampleSinc(tbuf, w2, h2, buf1, w1, h1);
				free(tbuf);

				sprintf(tb, "textures/q1tex2/%s_luma.png", s);
				Tex_StoreFile(tb, buf1, w1, h1);

				Tex_FreeRaw(buf2);
			}

			Tex_FreeRaw(buf0);
			Tex_FreeRaw(buf1);
		}
	}

	UI_Camera_Setup3D();

	pdglDisable(GL_CULL_FACE);
	pdglDisable(GL_TEXTURE_2D);
//	PD3D_DrawGrid(0, 0, 16, 1);

	UI_Camera_Setup2D();

	Con_Render();
	PDGLUI_Frame();

//	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(1, 0, 0, 1);

	pdglBegin(PDGL_LINES);
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

	printf("shutdown\n");
	return(0);
}
