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

void draw_objs()
{
	static int fca[256], fva[4*256];
	static float va[3*256];

	float rot[3*3], org[3];
	float im1[4*4], iv[4], mins[3], maxs[3], f, g;
	int i, j, k, l;
	int nf, nv, tfv;

	for(i=0; i<main_nobjs; i++)
	{
		j=main_objs[i];

//		glMatrixMode(GL_MODELVIEW_MATRIX);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		bsdeGetAttrFV(j, BSDE_ORIGIN, org, 3);
		bsdeGetAttrFV(j, BSDE_ROTATION_MATRIX, rot, 9);

		glTranslatef(org[0], org[1], org[2]);

#if 0
		glColor4f(1, 1, 1, 1);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(rot[0]*2, rot[1]*2, rot[2]*2);
		glVertex3f(0, 0, 0);
		glVertex3f(rot[3]*2, rot[4]*2, rot[5]*2);
		glVertex3f(0, 0, 0);
		glVertex3f(rot[6]*2, rot[7]*2, rot[8]*2);
		glEnd();
#endif

#if 1
		for(k=0; k<3; k++)for(l=0; l<3; l++)
			im1[k*4+l]=rot[k*3+l];
		im1[0*4+3]=0; im1[1*4+3]=0; im1[2*4+3]=0;
		im1[3*4+0]=0; im1[3*4+1]=0; im1[3*4+2]=0;
		im1[3*4+3]=1;
		glMultMatrixf(im1);
#endif

		k=bsdeGetAttrI(j, BSDE_SOLIDTYPE);
		switch(k)
		{
		case BSDE_SOLID_AABB:
		case BSDE_SOLID_OBB:
			bsdeGetAttrFV(j, BSDE_MINS, mins, 3);
			bsdeGetAttrFV(j, BSDE_MAXS, maxs, 3);
			PD3D_DrawCube(mins[0], mins[1], mins[2],
				maxs[0], maxs[1], maxs[2]);
//			PD3D_DrawGammaCube(mins[0], mins[1], mins[2],
//				maxs[0], maxs[1], maxs[2], cur->gamma);
			break;
		case BSDE_SOLID_SPHERE:
			f=bsdeGetAttrF(j, BSDE_RADIUS);
			PD3D_DrawSphere(0, 0, 0, f);
//			PD3D_DrawGammaSphere(f, cur->gamma);
			break;
		case BSDE_SOLID_CYLINDER:
			f=bsdeGetAttrF(j, BSDE_RADIUS);
			g=bsdeGetAttrF(j, BSDE_HEIGHT);
			PD3D_DrawCylinder(0, 0, 0, g, f, f);
			PD3D_DrawDisk(g*0.5, 0, f);
			PD3D_DrawDisk(-g*0.5, 1, f);
			break;
		case BSDE_SOLID_CCYLINDER:
			f=bsdeGetAttrF(j, BSDE_RADIUS);
			g=bsdeGetAttrF(j, BSDE_HEIGHT);
			PD3D_DrawCylinder(0, 0, 0, g, f, f);
			PD3D_DrawSphere(0, 0, g*0.5, f);
			PD3D_DrawSphere(0, 0, -g*0.5, f);
			break;

		case BSDE_SOLID_PLANE:
			bsdeGetAttrFV(j, BSDE_DIRECTION, iv, 4);
			PD3D_DrawPlane(iv[0], iv[1], iv[2], iv[3], 999);
			break;

		case BSDE_SOLID_TRIMESH:
			nf=bsdeGetAttrI(j, BSDE_NUM_FACES);
			nv=bsdeGetAttrI(j, BSDE_NUM_VERTICES);

			bsdeGetAttrIV(j, BSDE_FACE_VERTICES, fva, nf*3);
			bsdeGetAttrFV(j, BSDE_VERTICES, va, nv*3);

			glColor4f(0, 1, 0, 0.75); 

			glBegin(GL_TRIANGLES);
			for(k=0; k<nf; k++)
			{
				glVertex3fv(va+fva[k*3+0]*3);
				glVertex3fv(va+fva[k*3+1]*3);
				glVertex3fv(va+fva[k*3+2]*3);
			}
			glEnd();

			break;

#if 0
		case BSDE_SOLID_HULL:
			nf=bsdeGetAttrI(j, BSDE_NUM_FACES);
			nv=bsdeGetAttrI(j, BSDE_NUM_VERTICES);
			bsdeGetAttrIV(j, BSDE_FACE_COUNTS, fca, nf);
			tfv=0;
			for(k=0; k<nf; k++)tfv+=fca[k];

			bsdeGetAttrIV(j, BSDE_FACE_VERTICES, fva, tfv);
			bsdeGetAttrFV(j, BSDE_VERTICES, va, nv*3);

			glColor4f(1, 0, 0, 0.75); 


			l=0;
			for(i=0; i<nf; i++)
			{
				glBegin(GL_POLYGON);
				for(j=0; j<fca[i]; j++)
					glVertex3fv(va+fva[k++]*3);
				glEnd();
			}

//			PD3D_DrawCylinder(0, 0, 0, 2, 1, 1);
//			PD3D_DrawDisk(1, 0, 1);
//			PD3D_DrawDisk(-1, 1, 1);
			break;
#endif

		default:
			break;
		}

		glPopMatrix();
	}
}


int pdgl_main_startup(int argc, char *argv[])
{
	float v[3];
	char *opts[4];
	int i, j, k, l;

	bsdeNewWorld();

	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -9.8), 3);
//	bsdeSetWorldAttrFV(BSDE_GRAVITY, VEC3(0, 0, -2), 3);
	bsdeSetWorldAttrF(BSDE_MINSTEP, 0.005);
	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.01);
//	bsdeSetWorldAttrF(BSDE_MAXSTEP, 0.03);
	bsdeSetWorldAttrF(BSDE_MAXFRAME, 0.1);

//	bsdeSetWorldAttrI(BSDE_COLLIDE_STRICT, 1);

	main_nobjs=0;

//	main_readtris("resource/models/gear0.tri");

//	pdgl_main_readlights("tst0_lights.txt");

	i=bsdeNewAABB(-0.5, -0.5, -1.75, 0.5, 0.5, 0.25);
	bsdeSetAttrF(i, BSDE_DENSITY, 1000);
//	bsdeSetAttrF(i, BSDE_FRICTION, 0.01);
//	bsdeSetAttrF(i, BSDE_FRICTION, 0.1);
	bsdeSetAttrF(i, BSDE_FRICTION, 0.8);
	main_cam_id=i;

#if 1
//	i=bsdeNewAABB(-1, -1, -1, 1, 1, 1);
	i=bsdeNewSphere(1);
	main_objs[main_nobjs++]=i;
//	main_addsolid(i);
	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 0, 1), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1000);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
	bsdeSetAttrI(i, BSDE_NONSOLID, 1);
	j=i;

	v[0]=1;
//	v[1]=1;
	v[1]=0;
	v[2]=0;

	for(i=0; i<10; i++)
	{
		k=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
//		k=bsdeNewSphere(1);
		main_objs[main_nobjs++]=k;
//		main_addsolid(k);
		bsdeSetAttrFV(k, BSDE_ORIGIN, v, 3);
		bsdeSetAttrF(k, BSDE_DENSITY, 1000);
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


	i=bsdeNewAABB(-1, -1, -1, 1, 1, 1);
//	i=bsdeNewSphere(1);
	main_objs[main_nobjs++]=i;

	bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(0, 10, 0), 3);
	bsdeSetAttrF(i, BSDE_DENSITY, 1000);
	bsdeSetAttrI(i, BSDE_STATIC, 1);
	bsdeSetAttrI(i, BSDE_NONSOLID, 1);
	j=i;

	v[0]=5;
	v[1]=10;
	v[2]=0;

	for(i=0; i<10; i++)
	{
		k=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
//		k=bsdeNewSphere(1);
		main_objs[main_nobjs++]=k;
//		main_addsolid(k);
		bsdeSetAttrFV(k, BSDE_ORIGIN, v, 3);
		bsdeSetAttrF(k, BSDE_DENSITY, 1000);
		bsdeSetAttrI(k, BSDE_NONSOLID, 1);

		l=bsdeNewJoint(BSDE_JOINT_ELASTIC);
		bsdeBindJointSolids(l, j, k);
		bsdeSetJointAttrFV(l, BSDE_ORIGIN_A,
			VEC3(v[0]-4, v[1], v[2]), 3);
		bsdeSetJointAttrFV(l, BSDE_ORIGIN_B,
			VEC3(v[0]-1, v[1], v[2]), 3);

#if 0
		bsdeSetJointAttrF(l, BSDE_RADIUS, 3);
		bsdeSetJointAttrF(l, BSDE_STRENGTH, 8000);
//		bsdeSetJointAttrF(l, BSDE_CUBIC_ATTENUATION, 64);
		bsdeSetJointAttrF(l, BSDE_QUADRATIC_ATTENUATION, 1);
//		bsdeSetJointAttrF(l, BSDE_LINEAR_ATTENUATION, 2000);
		bsdeSetJointAttrF(l, BSDE_LINEAR_ATTENUATION, -1);
		bsdeSetJointAttrF(l, BSDE_CONSTANT_ATTENUATION, 1);
#endif

		bsdeSetJointAttrF(l, BSDE_RADIUS, 3);
		bsdeSetJointQuadratic(l, 3, 0, 1, 8000, 1, 0, 0);
		//((x-3)^2)*1000
		//(x^2-6x+9)*1000


		v[0]+=5;
		v[1]+=0;
		v[2]+=0;

		j=k;
	}

	bsdeSetAttrI(j, BSDE_STATIC, 1);

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

int main_addsolid(int id)
{
	main_objs[main_nobjs++]=id;
	return(0);
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
/*
		if(*kcur==K_F10)
		{
			Tex_DoScreenshot();
		}
*/

		if(*kcur==K_F2)
		{
			main_cam_raw=!main_cam_raw;
		}

		if(*kcur=='a')
		{
			i=bsdeNewOBB(-1, -1, -1, 1, 1, 1);
			main_objs[main_nobjs++]=i;
//			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='A')
		{
			i=bsdeNewAABB(-2, -2, -2, 2, 2, 2);
			main_objs[main_nobjs++]=i;
//			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='s')
		{
			i=bsdeNewSphere(1);
			main_objs[main_nobjs++]=i;
//			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='c')
		{
			i=bsdeNewCappedCylinder(2, 1);
			main_objs[main_nobjs++]=i;
//			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='C')
		{
			i=bsdeNewCylinder(2, 1);
			main_objs[main_nobjs++]=i;
//			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

		if(*kcur=='h')
		{
			i=bsdeNewHullF(hull_planes, 10);
			main_objs[main_nobjs++]=i;
//			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}

/*
		if(*kcur=='m')
		{
			i=bsdeNewTriMeshF(main_gear_vecs, main_gear_tris,
				main_gear_nvecs, main_gear_ntris);
			main_objs[main_nobjs++]=i;
//			main_addsolid(i);
//			bsdeSetAttrFV(i, BSDE_ORIGIN, VEC3(f, g, 10), 3);
			bsdeSetAttrFV(i, BSDE_ORIGIN, io, 3);
			bsdeSetAttrF(i, BSDE_DENSITY, 1000);

			bsdeSetAttrFV(i, BSDE_VELOCITY, main_cam_vel, 3);

			if(PDGL_KeyDown(K_ALT))
				bsdeSetAttrFV(i, BSDE_VELOCITY,
					VEC3(v*fw[0], v*fw[1], 0), 3);
		}
*/

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

	if(main_cam_raw)
	{
		V3F_ADD(main_cam_org, iv, pt);
		bsdeSetAttrFV(main_cam_id, BSDE_ORIGIN, pt, 3);
	}else
	{
//		Vec3F_Add(main_cam_vel, iv, pt);
//		Vec3F_Scale(iv, 1.0/pdgl_dt_f, pt);
//		pt[0]=iv[0]/pdgl_dt_f;
//		pt[1]=iv[1]/pdgl_dt_f;
//		pt[2]=iv[2]/pdgl_dt_f+main_cam_vel[2];
//		bsdeSetAttrFV(main_cam_id, BSDE_VELOCITY, pt, 3);

		V3F_SCALE(iv, 1.0/pdgl_dt_f, pt);
		bsdeSetAttrFV(main_cam_id, BSDE_CONTACT_VELOCITY, pt, 3);

		bsdeSetAttrF(main_cam_id, BSDE_CONTACT_POWER, 500);
	}

	Draw_SetPerspective_3D(4.0/3.0, 90, 0,
		main_cam_org, main_cam_ang, 0, 0, pdgl_xs, pdgl_ys);

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glColor4f(1, 0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(io[0], io[1], io[2]);
	glVertex3f(io[0], io[1], io[2]-50);
	glEnd();

	draw_objs();


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
	return(0);
}

int pdgl_main_shutdown()
{
	printf("app1 shutdown\n");
	return(0);
}
