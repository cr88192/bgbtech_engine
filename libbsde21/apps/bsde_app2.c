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

#include <libbsde.h>

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

struct main_entity_s {
struct main_entity_s *next;	//next in total
struct main_entity_s *lnext;	//next in leaf/node

int solid;		//solid id
float org[3];
float rot[9];
float gamma;
};


int main_objs[16384];
int main_nobjs;

float main_cam_org[3];
float main_cam_ang[3];
float main_cam_vel[3];
int main_cam_id;
int main_cam_raw=1;

int main_leafnum;
int main_nodenum;
float main_time;

float main_light_org[256*3];
float main_light_clr[256*3];
float main_light_val[256*2];
int main_lights;

float main_dlight_org[256*3];
float main_dlight_clr[256*3];
float main_dlight_val[256*2];
int main_dlights=0;

void draw_ents(struct main_entity_s *first);
int PDGL_KeyDown(int key);

int PD3D_DrawFlatCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz,
	float r, float g, float b, float a);
int PD3D_DrawFlatLineCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz,
	float r, float g, float b, float a);
int PD3D_DrawCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz);
int PD3D_DrawGammaCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz, float y);
int PD3D_DrawSphere(float x, float y, float z, float ra);
int PD3D_DrawFlatSphere(float x, float y, float z, float ra,
	float r, float g, float b, float a);
int PD3D_DrawGammaSphere(float ra, float gg);
float PD3D_DrawGrid(float x, float y, float w, int ax);
float PD3D_DrawXYGrid(float x, float y, float w);
float PD3D_DrawXZGrid(float x, float z, float w);
int PD3D_DrawCylinder(
	float x, float y, float z,
	float h, float rt, float rb);
int PD3D_DrawDisk(float z, float h, float rad);
int PD3D_DrawPlane(float x, float y, float z, float d, float w);

int Draw_SetSolid_2D(float aspect, int xo, int yo, int xs, int ys);
int Draw_SetSolid2_2D(float aspect, float size,
	float *org, float *angles, int xo, int yo, int xs, int ys);
int Draw_SetOrtho_3D(float aspect, float size,
	float *org, float *angles, int x, int y, int w, int h);
int Draw_SetSolid3_2D(float lxs, float lys,
	float *org, int xo, int yo, int xs, int ys);
int Draw_SetPerspective_3D(float aspect, float fov, float d,
	float *org, float *angles, int xo, int yo, int xs, int ys);
int Draw_Square(int x, int y, int w, int h);
int Draw_FillSquare(int x, int y, int w, int h, float c[4]);
int Draw_FillSquareRGBA(int x, int y, int w, int h,
	int r, int g, int b, int a);
int Draw_Circle(int ox, int oy, int r);
int Draw_FillCircle(int ox, int oy, int ra, int r, int g, int b, int a);
int Draw_PictureCircle(int ox, int oy, int r, int texnum);
int Draw_Bind(int num);
int Draw_DisableTextures();
int Draw_EnableTextures();
int Draw_ColorRGBA(int r, int g, int b, int a);
int Draw_Picture(int x, int y, int w, int h, int num);


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


FILE *main_vidbuf_fd;
float main_vidbuf_t;
byte *main_vidbuf;

#include "bsde_app0_bsp.c"
#include "bsde_app0_bsplm.c"

struct bsp_node_s *main_bsp;
struct main_entity_s *main_ents;

#include "bsde_app0_entity.c"

float *main_gear_vecs;
int *main_gear_tris;
int main_gear_nvecs;
int main_gear_ntris;

int main_indexvec(float *v)
{
	int i;

	for(i=0; i<main_gear_nvecs; i++)
		if(V3F_DIST(main_gear_vecs+i*3, v)<0.001)
			return(i);
	V3F_COPY(v, main_gear_vecs+i*3);
	main_gear_nvecs++;

	return(i);
}

void main_readtris(char *name)
{
	char buf[256];
	float tv[3];
	FILE *fd;
	char **a;
	int n;

	main_gear_vecs=malloc(65536*sizeof(float));
	main_gear_tris=malloc(65536*sizeof(int));
	main_gear_nvecs=0;
	main_gear_ntris=0;

	fd=fopen(name, "rt");
	if(!fd)
	{
		printf("can't read '%s'\n", name);
		return;
	}

	n=0;
	while(!feof(fd))
	{
		memset(buf, 0, 256);
		fgets(buf, 255, fd);

		a=ksplit(buf);
		if(!a[0])continue;

		tv[0]=atof(a[0]);
		tv[1]=atof(a[1]);
		tv[2]=atof(a[2]);
		main_gear_tris[n*3+0]=main_indexvec(tv);

		tv[0]=atof(a[3]);
		tv[1]=atof(a[4]);
		tv[2]=atof(a[5]);
		main_gear_tris[n*3+1]=main_indexvec(tv);

		tv[0]=atof(a[6]);
		tv[1]=atof(a[7]);
		tv[2]=atof(a[8]);
		main_gear_tris[n*3+2]=main_indexvec(tv);

		n++;
	}

	main_gear_ntris=n;
}


int pdgl_main_startup(int argc, char *argv[])
{
	float v[3];
	char *opts[4];
	int i, j, k, l;

	bsdeNewWorld();

//	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -9.8), 3);
//	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -2), 3);
//	bsdeSetWorldAttrF(BSDE_MINSTEP, 0.005);
//	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.01);
//	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.03);
//	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.01667);
//	bsdeSetWorldAttrF(BSDE_MAXFRAME, 0.1);

	bsdeSetWorld3f(BSDE_GRAVITY, 0, 0, -9.8);
//	bsdeSetWorldf(BSDE_MINSTEP, 0.005);
	bsdeSetWorldf(BSDE_MINSTEP, 0.001);
//	bsdeSetWorldf(BSDE_MAXSTEP, 0.01);
//	bsdeSetWorldf(BSDE_MAXSTEP, 0.03);
	bsdeSetWorldf(BSDE_MAXSTEP, 0.01667);
	bsdeSetWorldf(BSDE_MAXFRAME, 0.1);

//	bsdeSetWorldf(BSDE_MINSTEP, 0.0005);
//	bsdeSetWorldf(BSDE_MAXSTEP, 0.005);

//	bsdeSetWorldAttrI(BSDE_COLLIDE_STRICT, 1);

	main_nobjs=0;

//	main_readtris("resource/models/gear0.tri");

	pdgl_main_readlights("tst0_lights.txt");

#if 0
	main_bsp=pdgl_main_readbsp("tst0_bsp.txt");
	bsdeBegin(BSDE_SOLID_BSP);
	pdgl_main_sendnode(main_bsp);
	i=bsdeEnd();
	main_objs[main_nobjs++]=i;
	main_addsolid(i);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
#endif

	load_map("E1M1.MAP");

	i=bsdeNewAABB(-0.5, -0.5, -1.75, 0.5, 0.5, 0.25);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);
//	bsdeSetAttrF(i, BSDE_FRICTION, 0.01);
//	bsdeSetAttrF(i, BSDE_FRICTION, 0.1);
	bsdeSetAttrF(i, BSDE_FRICTION, 0.8);
	bsdeSetAttrI(i, BSDE_ACTOR, 1);
	main_cam_id=i;

#if 0
//	i=bsdeNewAABB(-1, -1, -1, 1, 1, 1);
	i=bsdeNewSphere(1);
//	main_objs[main_nobjs++]=i;
	main_addsolid(i);
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 40), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
	bsdeSetAttrI(i, BSDE_NONSOLID, 1);
	j=i;

	v[0]=1;
//	v[1]=1;
	v[1]=0;
	v[2]=39;

	for(i=0; i<10; i++)
	{
		k=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
//		k=bsdeNewSphere(1);
//		main_objs[main_nobjs++]=k;
		main_addsolid(k);
		bsdeSetAttrFV(k, BSDE_ORIGIN, v, 3);
		bsdeSetAttrF(k, BSDE_DENSITY, 1);
		bsdeSetAttrI(k, BSDE_STATIC, 0);
		bsdeSetAttrI(k, BSDE_NONSOLID, 1);

#if 1
		l=bsdeNewJoint(BSDE_JOINT_BALL);
//		l=bsdeNewJoint(BSDE_JOINT_FIXED);
		bsdeBindJointSolids(l, j, k);
//		bsdeSetJointAttrFV(l, BSDE_ORIGIN,
//			VEC3(v[0]-0.5, v[1]-0.5, v[2]+0.5), 3);
		bsdeSetJointAttrFV(l, BSDE_ORIGIN_123,
			VEC3(v[0]-0.5, v[1]-0.5, v[2]+0.5), 3);
#endif


#if 0
		l=bsdeNewJoint(BSDE_JOINT_HINGE);
		bsdeBindJointSolids(l, j, k);
		bsdeSetJointAttrFV(l, BSDE_ORIGIN,
			VEC3(v[0]-0.5, v[1]-0.5, v[2]+0.5), 3);
		bsdeSetJointAttrFV(l, BSDE_DIRECTION,
			VEC3(0, 1, 0), 3);
#endif

#if 0
		l=bsdeNewJoint(BSDE_JOINT_UNIVERSAL);
		bsdeBindJointSolids(l, j, k);
		bsdeSetJointAttrFV(l, BSDE_ORIGIN,
			VEC3(v[0]-0.5, v[1], v[2]+0.5), 3);
		bsdeSetJointAttrFV(l, BSDE_DIRECTION_A,
			VEC3(0, 1, 0), 3);
		bsdeSetJointAttrFV(l, BSDE_DIRECTION_B,
			VEC3(1, 0, 0), 3);
#endif

		v[0]+=1;
		v[1]+=1;
		v[2]-=1;

		j=k;
	}
#endif

#if 0
	i=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 10), 3);
	bsdeSetAttrFV(i, BSDE_TORQUE, VEC3(0, 0, 16*M_PI), 3);
//	bsdeSetAttrFV(i, BSDE_ANGULAR_VELOCITY, VEC3(0, 0, M_PI), 3);
//	bsdeSetAttrF(i, BSDE_DENSITY, 1);
	bsdeSetAttrI(i, BSDE_NONSOLID, 1);
#endif

#if 0
#if 1
	i=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
//	i=bsdeNewSphere(1);
//	i=bsdeNewCylinder(1, 1);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(1.5, 0, 10), 3);
//	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(2.5, 0, 10), 3);
	bsdeSetAttrFV(i, BSDE_VELOCITY, VEC3(0, 0, -5), 3);
//	bsdeSetAttrFV(i, BSDE_TORQUE, VEC3(0, 0, 16*M_PI), 3);
//	bsdeSetAttrFV(i, BSDE_ANGULAR_VELOCITY, VEC3(0, 0, M_PI), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);
#endif

#if 1
	i=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 5), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);

	i=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 0), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);

	i=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(-10, 0, -5), 3);
	bsdeSetAttrFV(i, BSDE_VELOCITY, VEC3(0, 5, 0), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);

#if 1
	i=bsdeNewOBB(-2, -2, -2, 2, 2, 2);
//	i=bsdeNewSphere(2);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, -5), 3);
//	bsdeSetAttrFV(i, BSDE_VELOCITY, VEC3(0, 0, 1), 3);
//	bsdeSetAttrFV(i, BSDE_VELOCITY, VEC3(0, 0, -1), 3);
//	bsdeSetAttrF(i, BSDE_MASS, 100000);
	bsdeSetAttrF(i, BSDE_DENSITY, 10000);
//	bsdeSetAttrFV(i, BSDE_TORQUE, VEC3(0, 0, 4096*M_PI), 3);
//	bsdeSetAttrFV(i, BSDE_ANGULAR_VELOCITY, VEC3(0, 0, M_PI), 3);
#endif

#if 0
	i=bsdeNewOBB(-2, -2, -2, 2, 2, 2);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(250, 500, 100), 3);
//	bsdeSetAttrFV(i, BSDE_VELOCITY, VEC3(0, 0, 1), 3);
	bsdeSetAttrFV(i, BSDE_VELOCITY, VEC3(0, 0, -100), 3);
//	bsdeSetAttrF(i, BSDE_MASS, 100000);
	bsdeSetAttrF(i, BSDE_DENSITY, 1000000);
//	bsdeSetAttrFV(i, BSDE_TORQUE, VEC3(0, 0, 4096*M_PI), 3);
//	bsdeSetAttrFV(i, BSDE_ANGULAR_VELOCITY, VEC3(0, 0, M_PI), 3);
#endif

#endif
#endif

#if 0
	i=bsdeNewAABB(-1000, -1000, -1, 1000, 1000, 1);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, -10), 3);
//	bsdeSetAttrFV(i, BSDE_VELOCITY, VEC3(0, 0, 1), 3);
//	bsdeSetAttrF(i, BSDE_MASS, 100000000);
	bsdeSetAttrF(i, BSDE_DENSITY, 1);
//	bsdeSetAttrF(i, BSDE_ELASTIC, 1);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
#endif

#if 0
	i=bsdeNewAABB(-10, -10, -3, 10, 10, 3);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, -7), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 2000);
	bsdeSetAttrF(i, BSDE_VISCOSITY, 0.01);
//	bsdeSetAttrF(i, BSDE_VISCOSITY, 0.025);
//	bsdeSetAttrF(i, BSDE_VISCOSITY, 0.0);
//	bsdeSetAttrF(i, BSDE_VISCOSITY, 100);
//	bsdeSetAttrF(i, BSDE_VISCOSITY, 10);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
	bsdeSetAttrI(i, BSDE_FLUID, 1);

//	bsdeSetAttrI(i, BSDE_NONSOLID, 1);
#endif

#if 0
	i=bsdeNewPlane(0, 0, 1, -10);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrI(i, BSDE_STATIC, 1);
#endif

#if 0
	i=bsdeNewPlane(1, 0, 1, 0);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrI(i, BSDE_STATIC, 1);
#endif

#if 0
	i=bsdeNewPlane(1, 0, 0, -10);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrI(i, BSDE_STATIC, 1);

	i=bsdeNewPlane(-1, 0, 0, -10);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrI(i, BSDE_STATIC, 1);

	i=bsdeNewPlane(0, 1, 0, -10);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrI(i, BSDE_STATIC, 1);

	i=bsdeNewPlane(0, -1, 0, -10);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrI(i, BSDE_STATIC, 1);
#endif

#if 0
	i=bsdeNewAABB(-10, -1, -2, 10, 1, 2);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 10, -6), 3);
	bsdeSetAttrI(i, BSDE_STATIC, 1);

	i=bsdeNewAABB(-10, -1, -2, 10, 1, 2);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, -10, -6), 3);
	bsdeSetAttrI(i, BSDE_STATIC, 1);

	i=bsdeNewAABB(-1, -10, -2, 1, 10, 2);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(-10, 0, -6), 3);
	bsdeSetAttrI(i, BSDE_STATIC, 1);

	i=bsdeNewAABB(-1, -10, -2, 1, 10, 2);
	main_objs[main_nobjs++]=i;
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(10, 0, -6), 3);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
#endif

//VEC3(0, -10, 5), VEC3(90-45, 0, 0)

#if 0
	i=pdgl_main_loadlms(main_bsp, "tst0_lm.dat");
	if(i<0)
	{
		pdgl_main_buildlms(main_bsp);
		pdgl_main_savelms(main_bsp, "tst0_lm.dat");
	}
#endif

	main_cam_org[0]=0;
	main_cam_org[1]=-10;
	main_cam_org[2]=5;
	bsdeSetAttrFV(main_cam_id, BSDE_ORIGIN, main_cam_org, 3);

	main_cam_ang[0]=90-45;
	main_cam_ang[1]=0;
	main_cam_ang[2]=0;

	main_time=0;

	V3F_ZERO(main_dlight_org);
	V3F_CONST(main_dlight_clr, sqrt(3));
//	main_dlight_val[0]=1024;
	main_dlight_val[0]=0;
	main_dlight_val[1]=0;
	main_dlights=1;

	V3F_ZERO(main_dlight_org+1*3);
	V3F_CONST(main_dlight_clr+1*3, sqrt(3));
	main_dlight_val[1*2+0]=10000;
	main_dlight_val[1*2+1]=0;
	main_dlights=2;

	main_vidbuf_fd=NULL;
//	main_vidbuf_fd=fopen("viddump.bin", "wb");
	main_vidbuf=malloc(800*600*4);
	main_vidbuf_t=0;

	return(1);
}

int pdgl_main_drawhull()
{
	float sa, sb, ca, cb, wa, wb;
	int i;

	for(i=0; i<8; i++)
	{
		wa=i*((M_PI*2)/7.0);
		wb=((i+1)&7)*((M_PI*2)/7.0);

		sa=sin(wa); ca=cos(wa);
		sb=sin(wb); cb=cos(wb);
	}
}

int pdgl_main_body()
{
	static float hull_planes[]={
	1, 0, 0, 1,
	1, 1, 0, 1,
	0, 1, 0, 1,
	-1, 1, 0, 1,
	-1, 0, 0, 1,
	-1, -1, 0, 1,
	0, -1, 0, 1,
	1, -1, 0, 1,
	0, 0, 1, 1,
	0, 0, -1, 1,
	};

	static int t;
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16];
	int i, j, k;
	unsigned short *kcur;

	main_time+=pdgl_dt_f;
	V3F_COPY(main_cam_org, main_dlight_org);

	V3F_ZERO(main_dlight_org+3);
	main_dlight_org[3]=sin(main_time*0.25)*75;
	main_dlight_org[4]=cos(main_time*0.25)*75;
	main_dlight_org[5]=10;

	kcur=pdgl_keybuf;

	f=main_cam_ang[2]*(M_PI/180);
	fw[0]=-sin(f);
	fw[1]=cos(f);

	bsdeGetAttrFV(main_cam_id, BSDE_ORIGIN, main_cam_org, 3);
	bsdeGetAttrFV(main_cam_id, BSDE_VELOCITY, main_cam_vel, 3);

	if(main_cam_raw)
	{
		bsdeHide(main_cam_id);
	}else
	{
		bsdeUnhide(main_cam_id);
	}

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
		if(*kcur==K_F10)
		{
//			Tex_DoScreenshot();
		}

		if(*kcur==K_F2)
		{
			main_cam_raw=!main_cam_raw;
		}

		if(*kcur=='a')
		{
			i=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
			main_objs[main_nobjs++]=i;
			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='A')
		{
			i=bsdeNewAABB(-2, -2, -2, 2, 2, 2);
			main_objs[main_nobjs++]=i;
			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='s')
		{
			i=bsdeNewSphere(1);
			main_objs[main_nobjs++]=i;
			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='c')
		{
			i=bsdeNewCappedCylinder(2, 1);
			main_objs[main_nobjs++]=i;
			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='C')
		{
			i=bsdeNewCylinder(2, 1);
			main_objs[main_nobjs++]=i;
			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='h')
		{
			i=bsdeNewHullF(hull_planes, 10);
			main_objs[main_nobjs++]=i;
			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='m')
		{
			i=bsdeNewTriMeshF(main_gear_vecs, main_gear_tris,
				main_gear_nvecs, main_gear_ntris);
			main_objs[main_nobjs++]=i;
			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		kcur++;
	}

	V3F_ZERO(iv);
	if(PDGL_KeyDown(K_SHIFT))
	{
		f=main_cam_ang[2]*(M_PI/180);
		if(PDGL_KeyDown(K_LEFTARROW))
		{
//			main_cam_org[0]-=pdgl_dt_f*15*cos(f);
//			main_cam_org[1]-=pdgl_dt_f*15*sin(f);
			iv[0]-=pdgl_dt_f*15*cos(f);
			iv[1]-=pdgl_dt_f*15*sin(f);
		}
		if(PDGL_KeyDown(K_RIGHTARROW))
		{
//			main_cam_org[0]+=pdgl_dt_f*15*cos(f);
//			main_cam_org[1]+=pdgl_dt_f*15*sin(f);
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
//		main_cam_org[0]-=pdgl_dt_f*15*sin(f);
//		main_cam_org[1]+=pdgl_dt_f*15*cos(f);
		iv[0]-=pdgl_dt_f*15*sin(f);
		iv[1]+=pdgl_dt_f*15*cos(f);
	}
	if(PDGL_KeyDown(K_END))
	{
//		main_cam_org[0]+=pdgl_dt_f*15*sin(f);
//		main_cam_org[1]-=pdgl_dt_f*15*cos(f);
		iv[0]+=pdgl_dt_f*15*sin(f);
		iv[1]-=pdgl_dt_f*15*cos(f);
	}

//	if(PDGL_KeyDown(K_PGUP))main_cam_org[2]+=pdgl_dt_f*15;
//	if(PDGL_KeyDown(K_PGDN))main_cam_org[2]-=pdgl_dt_f*15;
	if(PDGL_KeyDown(K_PGUP))iv[2]+=pdgl_dt_f*15;
	if(PDGL_KeyDown(K_PGDN))iv[2]-=pdgl_dt_f*15;

	if(main_cam_raw)
	{
		V3F_ADD(main_cam_org, iv, pt);
		bsdeSetAttrFV(main_cam_id, BSDE_ORIGIN, pt, 3);

		V3F_ZERO(pt);
		bsdeSetAttrFV(main_cam_id, BSDE_VELOCITY, pt, 3);
	}else
	{
//		Vec3F_Add(main_cam_vel, iv, pt);
//		Vec3F_Scale(iv, 1.0/pdgl_dt_f, pt);
//		pt[0]=iv[0]/pdgl_dt_f;
//		pt[1]=iv[1]/pdgl_dt_f;
//		pt[2]=iv[2]/pdgl_dt_f+main_cam_vel[2];
//		bsdeSetAttrFV(main_cam_id, BSDE_VELOCITY, pt, 3);

//		bsdeGetAttrFV(main_cam_id, BSDE_VELOCITY, pt, 3);
//		V3F_ADDSCALE(pt, iv, 1.0/pdgl_dt_f, pt);

//		pt[0]=pt[0]*0.1+iv[0]*(1.0/pdgl_dt_f);
//		pt[1]=pt[1]*0.1+iv[1]*(1.0/pdgl_dt_f);
//		pt[2]=pt[2]+iv[2]*(1.0/pdgl_dt_f);

//		V3F_SCALE(iv, 1.0/pdgl_dt_f, pt);
//		bsdeSetAttrFV(main_cam_id, BSDE_CONTACT_VELOCITY, pt, 3);
//		bsdeSetAttrF(main_cam_id, BSDE_CONTACT_POWER, 500);

//		bsdeSetAttrFV(main_cam_id, BSDE_VELOCITY, pt, 3);

		V3F_SCALE(iv, 0.25*1.0/pdgl_dt_f, pt);
		pt[2]*=0.15;

		bsdeSetAttrFV(main_cam_id, BSDE_WANTED_VELOCITY, pt, 3);
		bsdeSetAttrI(main_cam_id, BSDE_WANTED_VELOCITY, 1);
	}

//	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
//		VEC3(0, -15, 5), VEC3(90-45, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

//	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
//		VEC3(0, -10, 5), VEC3(90-45, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

//	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
//		VEC3(0, -15, 0), VEC3(90, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
		main_cam_org, main_cam_ang, 0, 0, pdgl_xs, pdgl_ys);

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	pt[0]=0.8;
	pt[1]=0.8;
	pt[2]=0.8;
	pt[3]=1;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, pt);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);

	pt[0]=main_cam_org[0];
	pt[1]=main_cam_org[1];
	pt[2]=main_cam_org[2];
	pt[3]=1;
	glLightfv(GL_LIGHT0, GL_POSITION, pt);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/1000.0);
//	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0/100000.0);

	glEnable(GL_LIGHT0);

	glDisable(GL_LIGHTING);

	if(main_bsp)
	{
		relink_ents();
//		pdgl_main_drawlightnode(main_bsp, main_bsp);
		pdgl_main_faceid=0;
		pdgl_main_drawnode(main_bsp);
	}else
	{
		draw_ents2();
	}

	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	glColor4f(1, 0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(io[0], io[1], io[2]);
	glVertex3f(io[0], io[1], io[2]-50);
	glEnd();


	f=bsdeGetWorldAttrF(BSDE_REALTIME);
//	g=bsdeGetWorldAttrF(BSDE_WORLDTIME);
//	printf("rt %f wt %f\n", f, g);

	bsdeStepWorld(pdgl_dt/1000.0);
//	bsdeStepWorld(1.0/1000.0);

	Draw_SetSolid2_2D(4.0/3.0, 400,
		VEC3(0, 0, 0), VEC3(0, 0, 0), 0, 0, pdgl_xs, pdgl_ys);

//	GfxFont_SetFont("fixed", 0);
//	kprints(buf, "%2.2f fps", 1.0/(pdgl_dt/1000.0));
//	GfxFont_DrawString(buf, -400, 300-12-8,
//		8, 8,  0, 255, 0, 255);

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

	main_vidbuf_t+=pdgl_dt_f;
	if(main_vidbuf_fd && (main_vidbuf_t>1.0/15.0))
	{
		main_vidbuf_t-=1.0/15.0;

		glFinish();
		glReadPixels (0, 0, 800, 600, GL_RGBA, GL_UNSIGNED_BYTE,
			main_vidbuf);
		fwrite(main_vidbuf, 1, 800*600*4, main_vidbuf_fd);
	}

	return(0);
}

int pdgl_main_shutdown()
{
	printf("app1 shutdown\n");
	return(0);
}
