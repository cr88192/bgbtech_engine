#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

#include <pdgl_keys.h>
#include <pdgl_math.h>

#ifdef __cplusplus
extern "C" {
#endif

int main_kill;

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


float main_cam_org[3];
float main_cam_ang[3];
float main_cam_vel[3];
int main_cam_id;
int main_cam_raw=1;

float main_time;

#include "glstub_auto.h"


#include "hull.h"
#include "hull_base.c"

static byte *kralloc_buf=NULL;
static int kralloc_pos=0;

byte *kralloc(int sz)
{
	void *p;
	if(!kralloc_buf)kralloc_buf=malloc(262144);

	if((kralloc_pos+sz)>262144)
		kralloc_pos=0;
	p=kralloc_buf+kralloc_pos;
	kralloc_pos=(kralloc_pos+sz+3)&(~3);

	return(p);
}

char **ksplit(char *s)
{
	char **a, *t;
	int i;

	a=(char **)kralloc(128*sizeof(char *));
	memset(a, 0, 128*sizeof(char *));
	i=0;
	while(*s)
	{
		while(*s && (*s<=' '))s++;
		if(!*s)break;
		t=kralloc(256);
		a[i++]=t;

		if(*s=='"')
		{
			s++;
			while(*s && (*s!='"'))*t++=*s++;
			if(*s=='"')s++;

			*t++=0;
			continue;
		}

		while(*s && (*s>' '))*t++=*s++;
		*t++=0;
	}
	a[i]=NULL;

	return(a);
}

float *PD3D_Vec_AllocVec(int n)
{
	return((float *)kralloc(n*sizeof(float)));
}


Solid_Hull *main_hulls;
Solid_Hull *main_mhull;

float main_hull_norms[6*4]=
{
	-1,  0,  0, 1,
	 1,  0,  0, 1,
	 0, -1,  0, 1,
	 0,  1,  0, 1,
	 0,  0, -1, 1,
	 0,  0,  1, 1,
};


int pdgl_main_setstate()
{
	main_hulls=Hull_BuildHull(main_hull_norms, 6);

	main_mhull=Hull_BuildHull(main_hull_norms, 6);
	main_mhull->next=main_hulls;
	main_hulls=main_mhull;

	main_mhull->org[2]=2;
}

int pdgl_main_startup(int argc, char *argv[])
{
	float v[3];
	char *opts[4];
	int i, j, k, l;

	main_cam_org[0]=0;
	main_cam_org[1]=-10;
	main_cam_org[2]=5;

	main_cam_ang[0]=90-45;
	main_cam_ang[1]=0;
	main_cam_ang[2]=0;

	pdgl_main_setstate();

	return(1);
}

int pdgl_main_body()
{
	static int t;
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16];
	int i, j, k;
	unsigned short *kcur;
	Solid_Hull *hcur, *hcur2;


	main_time+=pdgl_dt_f;

	kcur=pdgl_keybuf;

	f=main_cam_ang[2]*(M_PI/180);
	fw[0]=-sin(f);
	fw[1]=cos(f);

	io[0]=main_cam_org[0]-10*sin(f);
	io[1]=main_cam_org[1]+10*cos(f);
	io[2]=main_cam_org[2];

	f=(((rand()&0xFFF)/2048.0)-1)*0.1;
	g=(((rand()&0xFFF)/2048.0)-1)*0.1;
	io[0]+=f;
	io[1]+=g;

	v=1000;

	while(*kcur)
	{
		if(*kcur==K_ESCAPE)
		{
			main_kill=1;
			return(-1);
		}

		if(*kcur==K_F2)
		{
			main_cam_raw=!main_cam_raw;
		}

		if(*kcur==K_F5)
		{
			pdgl_main_setstate();
		}

		if(*kcur==' ')
		{
			hcur=main_hulls;
			while(hcur)
			{
				V3F_ZERO(hcur->vel);
				V3F_ZERO(hcur->tvel);
				hcur=hcur->next;
			}
		}

		kcur++;
	}

	V3F_ZERO(iv);
	if(PDGL_KeyDown(K_SHIFT))
	{
		f=main_cam_ang[2]*(M_PI/180);
		if(PDGL_KeyDown(K_LEFTARROW))
		{
			iv[0]-=pdgl_dt_f*15*cos(f);
			iv[1]-=pdgl_dt_f*15*sin(f);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
			iv[0]+=pdgl_dt_f*15*cos(f);
			iv[1]+=pdgl_dt_f*15*sin(f);
		}
	}else
	{
		if(PDGL_KeyDown(K_LEFTARROW))main_cam_ang[2]+=90*pdgl_dt_f;
		if(PDGL_KeyDown(K_RIGHTARROW))main_cam_ang[2]-=90*pdgl_dt_f;
	}
	if(PDGL_KeyDown(K_UPARROW))main_cam_ang[0]+=90*pdgl_dt_f;
	if(PDGL_KeyDown(K_DOWNARROW))main_cam_ang[0]-=90*pdgl_dt_f;

	f=main_cam_ang[2]*(M_PI/180);
	if(PDGL_KeyDown(K_HOME))
	{
		iv[0]-=pdgl_dt_f*15*sin(f);
		iv[1]+=pdgl_dt_f*15*cos(f);
	}
	if(PDGL_KeyDown(K_END))
	{
		iv[0]+=pdgl_dt_f*15*sin(f);
		iv[1]-=pdgl_dt_f*15*cos(f);
	}

	if(PDGL_KeyDown(K_PGUP))iv[2]+=pdgl_dt_f*15;
	if(PDGL_KeyDown(K_PGDN))iv[2]-=pdgl_dt_f*15;

	V3F_ADD(main_cam_org, iv, main_cam_org);


	V3F_ZERO(iv);

	f=main_cam_ang[2]*(M_PI/180);
	if(PDGL_KeyDown('e'))
	{
		iv[0]-=pdgl_dt_f*1*sin(f);
		iv[1]+=pdgl_dt_f*1*cos(f);
	}
	if(PDGL_KeyDown('q'))
	{
		iv[0]+=pdgl_dt_f*1*sin(f);
		iv[1]-=pdgl_dt_f*1*cos(f);
	}

	if(PDGL_KeyDown('a'))
	{
		iv[0]-=pdgl_dt_f*1*cos(f);
		iv[1]-=pdgl_dt_f*1*sin(f);
	}
	if(PDGL_KeyDown('d'))
	{
		iv[0]+=pdgl_dt_f*1*cos(f);
		iv[1]+=pdgl_dt_f*1*sin(f);
	}

	if(PDGL_KeyDown('w'))iv[2]+=pdgl_dt_f*1;
	if(PDGL_KeyDown('s'))iv[2]-=pdgl_dt_f*1;

	V3F_ADD(main_mhull->org, iv, main_mhull->org);

	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
		main_cam_org, main_cam_ang, 0, 0, pdgl_xs, pdgl_ys);

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

/*
	glColor4f(1, 0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(io[0], io[1], io[2]);
	glVertex3f(io[0], io[1], io[2]-50);
	glEnd();
*/

	PD3D_DrawPlane(0, 0, 1, -10, 1000);

	hcur=main_hulls;
	while(hcur)
	{
		Hull_DrawHull(hcur);
		hcur=hcur->next;
	}

	iv[0]=0;
	iv[1]=0;
	iv[2]=1;
	iv[3]=-10;

	hcur=main_hulls;
	while(hcur)
	{
		hcur2=hcur->next;
		while(hcur2)
		{
			Hull_CollideHull(hcur, hcur2);
			hcur2=hcur2->next;
		}

		Hull_CollideHullPlane(hcur, iv);
		hcur=hcur->next;
	}

	hcur=main_hulls;
	while(hcur)
	{
		hcur->vel[2]-=2*pdgl_dt_f;
		Hull_StepHull(hcur, pdgl_dt_f);
		hcur=hcur->next;
	}

	Draw_SetSolid2_2D(4.0/3.0, 400,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	kcur=pdgl_keybuf;
	while(*kcur)
	{
		if(*kcur==K_F10)
		{
			glFinish();
//			Tex_DoScreenshot();
		}
		kcur++;
	}
	return(0);
}

int pdgl_main_shutdown()
{
	printf("app1 shutdown\n");
	return(0);
}
