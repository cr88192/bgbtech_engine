#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#include <pdgl.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>


extern int tex_screen;

int tex_firegen;
int tex_firegen2;

int ConCmds_Load(char **a)
{
}

int ConCmds_Save(char **a)
{
}


void draw_update_defsdr()
{
	int def;
	int i;

//	glDisable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, tex_firegen);
	glEnable(GL_TEXTURE_2D);

//	def=Tex_LoadFile("textures/effects/fire/turb0", NULL, NULL);
	def=Tex_LoadFile("textures/effects/norm_noise1", NULL, NULL);

#if 1
	i=PDGL_LoadShader("bgb_deform3");
	PDGL_BindShader(i);

	PDGL_Uniform1f("time", pdgl_time/1000.0);
	PDGL_Uniform1f("scale", 0.25);
	PDGL_Uniform1f("rate", 0.25);

	PDGL_Uniform1i("texBase", 0);
	PDGL_Uniform1i("texDeform", 1);

	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, tex_firegen);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, def);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
#endif

	glColor4f(0.9, 0.9, 0.9, 1);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(400-256, -300+256);

	glTexCoord2f(0, 0);
	glVertex2f(400-256, -300);

	glTexCoord2f(1, 0);
	glVertex2f(400, -300);

	glTexCoord2f(1, 1);
	glVertex2f(400, -300+256);
	glEnd();

#if 1
	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
	PDGL_UnbindShader();
#endif

	glDisable(GL_TEXTURE_2D);

	glColor4f(0, 0, 0, 1);
	glBegin(GL_QUADS);
	glVertex2f(400-256, -300+16);
	glVertex2f(400-256, -300);
	glVertex2f(400, -300);
	glVertex2f(400, -300+16);
	glEnd();

	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
	glVertex2f(400-128-32, -300+16);
	glVertex2f(400-128-32, -300);
	glVertex2f(400-128+32, -300);
	glVertex2f(400-128+32, -300+16);
	glEnd();


	glFinish();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_firegen);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800-256, 0, 256, 256, 0);

//	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800-256, 600-256, 256, 256, 0);
//	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, 256, 256, 0);
}

void draw_update_defsdr2()
{
	int def;
	int i;

	glBindTexture(GL_TEXTURE_2D, tex_firegen);
	glEnable(GL_TEXTURE_2D);

	def=Tex_LoadFile("textures/effects/norm_noise1", NULL, NULL);

#if 1
	i=PDGL_LoadShader("bgb_deform4");
	PDGL_BindShader(i);

	PDGL_Uniform1f("time", pdgl_time/1000.0);
	PDGL_Uniform1f("scale", 0.25);
	PDGL_Uniform1f("rate", 0.25);

	PDGL_Uniform1i("texBase", 0);
	PDGL_Uniform1i("texDeform", 1);

	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, tex_firegen);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, def);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
#endif

	glColor4f(0.9, 0.9, 0.9, 1);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 1);
	glVertex2f(400-256, -300+256);

	glTexCoord2f(0, 0);
	glVertex2f(400-256, -300);

	glTexCoord2f(1, 0);
	glVertex2f(400, -300);

	glTexCoord2f(1, 1);
	glVertex2f(400, -300+256);
	glEnd();

#if 1
	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
	PDGL_UnbindShader();
#endif

	glFinish();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex_firegen2);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800-256, 0, 256, 256, 0);
}

void draw_test_defsdr()
{
	int sdr, tex, def;

//	return;

	Tex_FillTextureScreenshot(tex_screen);

	tex=Tex_LoadFile("textures/effects/fire/bgb_fire3", NULL, NULL);
	def=Tex_LoadFile("textures/effects/fire/turb0", NULL, NULL);

	sdr=PDGL_LoadShader("bgb_deform1");
//	sdr=PDGL_LoadShader("warp0");
	PDGL_BindShader(sdr);

	PDGL_Uniform1f("time", pdgl_time/1000.0);
	PDGL_Uniform1f("scale", 0.05);
	PDGL_Uniform1f("rate", 0.25);

//	PDGL_Uniform1i("basetex", tex_screen);
//	PDGL_Uniform1i("deformtex", def);

	PDGL_Uniform1i("texScreen", 0);
	PDGL_Uniform1i("texBase", 1);
	PDGL_Uniform1i("texDeform", 2);

//	glDisable(GL_TEXTURE_2D);

//	glBindTexture(GL_TEXTURE_2D, tex_screen);
//	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, tex_screen);
//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, tex);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(2);
	glBindTexture(GL_TEXTURE_2D, def);
	glEnable(GL_TEXTURE_2D);

	pdglActiveTexture(0);

//	glColor4f(0.01, 0.01, 0.01, 1);
	glColor4f(1, 1, 1, 0.75);

	glBegin(GL_QUADS);

	pdglMultiTexCoord2f(0, 0, 1);
	pdglMultiTexCoord2f(1, 0, 1);
	glVertex3f(-16, 16, 16);

	pdglMultiTexCoord2f(0, 0, 0);
	pdglMultiTexCoord2f(1, 0, 0);
	glVertex3f(-16, 16, -16);

	pdglMultiTexCoord2f(0, 1, 0);
	pdglMultiTexCoord2f(1, 1, 0);
	glVertex3f(16, 16, -16);

	pdglMultiTexCoord2f(0, 1, 1);
	pdglMultiTexCoord2f(1, 1, 1);
	glVertex3f(16, 16, 16);

	glEnd();


	pdglActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(1);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(2);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	pdglActiveTexture(0);

	PDGL_UnbindShader();
}



int pdgl_main_startup(int argc, char *argv[])
{
	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	UI_Camera_Init();

	if(tex_screen<=0)
		tex_screen=Tex_ScreenshotTexture();

	tex_firegen=Tex_EmptyTexture(256, 256);

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

	UI_Camera_Update();
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

	draw_test_defsdr();


	UI_Camera_Setup2D();

	draw_update_defsdr();
	draw_update_defsdr2();

	Con_Render();

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 0, 0, 1);

	glBegin(GL_LINES);
	glVertex2f(pdgl_mx-10, -pdgl_my);
	glVertex2f(pdgl_mx+10, -pdgl_my);
	glVertex2f(pdgl_mx, -pdgl_my-10);
	glVertex2f(pdgl_mx, -pdgl_my+10);
	glEnd();

	return(0);
}

int pdgl_main_shutdown()
{
	printf("shutdown\n");
	return(0);
}
