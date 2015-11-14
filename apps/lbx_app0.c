#include <windows.h>

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

//#include <pdlib.h>

#include <pdgl.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbscr1.h>

float cam_vel[3];
int cam_id;

LBXGL_World *main_world;
LBXGL_Light *main_light0;
LBXGL_Light *main_light1;

LBXGL_Skel2Model *main_tstmdl;
LBXGL_Skel2State *main_tstctx;
LBXGL_Entity *main_tstent;

int (*engine_handle_input_fp)();

#if 0

byte *trace_grid;

void build_trace()
{
	float v0[3], v1[3];
	int i, j, k;

	trace_grid=malloc(256*256*256);
	memset(trace_grid, 0, 256*256*256);

	for(i=0; i<256; i++)
		for(j=0; j<256; j++)
			for(k=0; k<256; k++)
	{
		v0[0]=(i*16)-2048;
		v0[1]=(j*16)-2048;
		v0[2]=(k*16)-2048;
		BSP_TraceNode(main_world->mdls->root, 
	}
}

#endif


int pdgl_main_startup(int argc, char *argv[])
{
	float v0[3], v1[3], v2[3], f, g, h;
	char *opts[4];
	void *p;
	int i, j, k, l;

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGL_InitASM(argc, argv);

	PDGLUI_Init();
	UI_Camera_Init();

	LBXGL_Entity_Init();

	PDGL_LoadModule("scripts/e_world.c");

	i=PDGL_CheckLinkage();
	if(i<0)return(-1);

	p=PDGL_GetPtr("engine_main");
	if(p)	((int(*)(int argc, char **argv))p)(argc, argv);

	p=PDGL_GetPtr("engine_handle_input");
	engine_handle_input_fp=(int (*)())p;

	cam_org[0]=0;
	cam_org[1]=-10;
	cam_org[2]=16;

	cam_ang[0]=90;
	cam_ang[1]=0;
	cam_ang[2]=0;

	main_world=BSP_ReadBSP(argv[1]);

#if 1
	LBXGL_Physics_SpawnWorld(main_world);

	V3F_SET(v0, -0.5*40, -0.5*40, -1.75*40);
	V3F_SET(v1, 0.5*40, 0.5*40, 0.25*40);

//	i=bsdeNewAABB(-0.5*40, -0.5*40, -1.75*40, 0.5*40, 0.5*40, 0.25*40);
	i=bsdeNewAABB(v0[0], v0[1], v0[2], v1[0], v1[1], v1[2]);
//	bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_IN3);
	bsdeSetAttrF(i, BSDE_DENSITY, LBXGL_DENSITY_LB_FT3);
//	bsdeSetAttrF(i, BSDE_FRICTION, 0.01);
//	bsdeSetAttrF(i, BSDE_FRICTION, 0.1);
	bsdeSetAttrF(i, BSDE_FRICTION, 0.8);
	cam_id=i;

//	bsdeSetAttrF(i, BSDE_MASS, 200);

	V3F_SUB(v1, v0, v2);
	f=bsdeGetAttrF(i, BSDE_MASS);
	g=(v2[0]*v2[1]*v2[2])/(12*12*12);
	h=bsdeGetAttrF(i, BSDE_DENSITY);

	printf("player's weight %glb vol %gft^3 density %glb/ft^3, sz (%g %g %g)\n",
		f, g, h*(12*12*12),
		v2[0], v2[1], v2[2]);

#endif

#if 0
	main_light1=kalloc(sizeof(LBXGL_Light));
//	V3F_SET(main_light1->clr, 1, 1, 1);
	V3F_SET(main_light1->clr, 1, 0, 0);
	Vec3F_Normalize(main_light1->clr, main_light1->clr);
	main_light1->val[0]=25000;

	main_light0=kalloc(sizeof(LBXGL_Light));
	V3F_SET(main_light0->clr, 1, 1, 1);
	Vec3F_Normalize(main_light0->clr, main_light0->clr);
	main_light0->val[0]=50000;

	main_light0->next=main_light1;

	main_world->dlights=main_light0;
#endif

#if 1
	main_tstmdl=Skel_LoadModel("model/human/human2.model");
	main_tstctx=Skel_NewInstance(main_tstmdl);
	Skel_SetAnim(main_tstctx, "walk");

	main_tstent=LBXGL_Entity_NewWorld(main_world);
//	LBXGL_Entity_SetProperty(main_tstent, "mdl", main_tstctx);
	LBXGL_Entity_SetPropertyFVector(
		main_tstent, "origin", VEC3(0, 256, 0), 3);
#endif

//	main_time=0;
//	main_ltime=main_time;

	PDGL_Sound_Init();

//	PDGL_Sound_PlayPointSample("sounds/alarm/crashbuz", PDGL_SOUND_LOOP,
//		1.0, 0.1, VEC3(0, 0, 0), VEC3(0, 0, 0));

//	PDGL_Sound_PlayPointSample("sounds/tonal/hz440s", PDGL_SOUND_LOOP,
//		1.0, 0.1, VEC3(0, 0, 0), VEC3(0, 0, 0));

//	PDGL_Sound_SetEcho(0.2, 0.5);
//	PDGL_Sound_SetDampen(0.99);

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

	if(engine_handle_input_fp)
	{
		i=engine_handle_input_fp();
		if(i>0)return(0);
	}

	UI_Camera_Update();

	bsdeSetAttrFV(cam_id, BSDE_ORIGIN, cam_org, 3);
	bsdeSetAttrFV(cam_id, BSDE_VELOCITY, cam_vel, 3);
}

int pdgl_main_body()
{
	static int t;
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;
	BSP_Model *mcur;

	int i, j, k;
	unsigned short *kcur;
	char *s;


#if 0
	main_light0->org[0]=sin(pdgl_time/1000.0)*2048;
	main_light0->org[1]=cos(pdgl_time/1000.0)*2048;
	main_light0->flags|=LBXGL_LFL_CHANGED;
	main_light0->leaf=BSP_PointLeafNode(main_world->mdls->root,
		main_light0->org);

	main_light1->org[0]=sin(-pdgl_time/1000.0)*512;
	main_light1->org[1]=cos(-pdgl_time/1000.0)*512;
	main_light1->flags|=LBXGL_LFL_CHANGED;
	main_light1->leaf=BSP_PointLeafNode(main_world->mdls->root,
		main_light1->org);
#endif

//	LBXGL_Entity_SetPropertyFVector(main_tstent, "origin",
//		VEC3(sin(-main_time*0.5)*384, cos(-main_time*0.5)*384, -96), 3);

//	if((main_time-main_ltime)>(1.0/15))
//	{
//		BSP_LightWorld(main_world);
//		main_ltime=main_time;
//	}

	BSP_LightWorld(main_world);


#if 1
	bsdeGetAttrFV(cam_id, BSDE_ORIGIN, cam_org, 3);
	bsdeGetAttrFV(cam_id, BSDE_VELOCITY, cam_vel, 3);

//	if(cam_raw)
	if(1)
	{
		bsdeHide(cam_id);
	}else
	{
		bsdeUnhide(cam_id);
	}

	LBXGL_Physics_StepWorld(main_world, pdgl_dt_f);

	V3F_SCALE(cam_org, 1.0/40.0, io);
	V3F_SCALE(cam_vel, 1.0/40.0, iv);
	PDGL_Sound_CamOriginFV(io);
	PDGL_Sound_CamVelocityFV(iv);
	PDGL_Sound_Frame(pdgl_dt_f);

//	main_time+=pdgl_dt_f;

#endif

	main_handle_input();


	UI_Camera_Setup3D();

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
//	PD3D_DrawGrid(0, 0, 16, 1);

	glBegin(GL_LINES);

	glColor4f(1, 0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(1000, 0, 0);

	glColor4f(0, 1, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1000, 0);

	glColor4f(0, 0, 1, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1000);

	glEnd();

	LBXGL_SetCamOriginF(cam_org);

	LBXGL_SkyBox_DrawSkyBox();
	LBXGL_SkyDome_DrawSkyDome();

//	BSP_RelinkEnts(main_world);

//	BSP_DrawBSP()
//	BSP_DrawNode(main_world, main_world->mdls, main_world->mdls->root);

	if(main_world)BSP_DrawModel(main_world, main_world->mdls);

#if 0
	mcur=main_world->mdls;
	while(mcur)
	{
		glColor4f(1, 1, 1, 1);
//		BSP_DrawNode(main_world, mcur, mcur->root);
		BSP_DrawModel(main_world, mcur);
		mcur=mcur->next;
	}
#endif

//	if(main_tstctx)
//	{
//		Skel_DrawModel(main_tstctx);
//		Skel_StepTime(main_tstctx, pdgl_dt_f);
//	}


	Draw_SetSolid2_2D(4.0/3.0, 400,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	Con_Render();

	GfxFont_SetFont("fixed", 0);

	sprintf(buf, "(%g %g %g) (%g %g %g)",
		cam_org[0], cam_org[1], cam_org[2],
		cam_ang[0], cam_ang[1], cam_ang[2]);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);


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
