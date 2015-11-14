#include <windows.h>
#include <GL/gl.h>
#include <math.h>

#ifndef M_PI
#define M_PI		3.1415926535897932384626433832795
#endif

int PD3D_DrawFlatCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz,
	float r, float g, float b, float a)
{
	int i, j;

//	printf("cube (%f %f %f) (%f %f %f)\n",
//		minx, miny, minz,
//		maxx, maxy, maxz);

	float vtxa[8*3];
	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	};

	float *pt1, *pt2, *pt3, *pt4;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=minx;
		vtxa[i*3+1]=miny;
		vtxa[i*3+2]=minz;
		if(i&1)vtxa[i*3+0]=maxx;
		if(i&2)vtxa[i*3+1]=maxy;
		if(i&4)vtxa[i*3+2]=maxz;
	}


	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		glBegin(GL_POLYGON);

		glColor4f(r, g, b, a);
		glVertex3fv(pt1);

		glColor4f(r, g, b, a);
		glVertex3fv(pt2);

		glColor4f(r, g, b, a);
		glVertex3fv(pt3);

		glColor4f(r, g, b, a);
		glVertex3fv(pt4);

		glEnd();
	}
	return(0);
}

int PD3D_DrawFlatLineCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz,
	float r, float g, float b, float a)
{
	int i, j;

//	printf("cube (%f %f %f) (%f %f %f)\n",
//		minx, miny, minz,
//		maxx, maxy, maxz);

	float vtxa[8*3];
	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	};

	float *pt1, *pt2, *pt3, *pt4;

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=minx;
		vtxa[i*3+1]=miny;
		vtxa[i*3+2]=minz;
		if(i&1)vtxa[i*3+0]=maxx;
		if(i&2)vtxa[i*3+1]=maxy;
		if(i&4)vtxa[i*3+2]=maxz;
	}

	glColor4f(r, g, b, a);

	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		glBegin(GL_LINES);
		glVertex3fv(pt1);
		glVertex3fv(pt2);

		glVertex3fv(pt2);
		glVertex3fv(pt3);

		glVertex3fv(pt3);
		glVertex3fv(pt4);

		glVertex3fv(pt4);
		glVertex3fv(pt1);
		glEnd();
	}
	return(0);
}

int PD3D_DrawCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz)
{
	int i, j;
	float r, g, b, a;

//	printf("cube (%f %f %f) (%f %f %f)\n",
//		minx, miny, minz,
//		maxx, maxy, maxz);

	float vtxa[8*3];
	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	};

	float *pt1, *pt2, *pt3, *pt4;

	a=1;

	glDisable(GL_TEXTURE_2D);

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=minx;
		vtxa[i*3+1]=miny;
		vtxa[i*3+2]=minz;
		if(i&1)vtxa[i*3+0]=maxx;
		if(i&2)vtxa[i*3+1]=maxy;
		if(i&4)vtxa[i*3+2]=maxz;
	}


	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		glBegin(GL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		glColor4f(r, g, b, a);
		glVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		glColor4f(r, g, b, a);
		glVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		glColor4f(r, g, b, a);
		glVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		glColor4f(r, g, b, a);
		glVertex3fv(pt4);

		glEnd();
	}
	return(0);
}

int PD3D_DrawGammaCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz, float y)
{
	int i, j;
	float r, g, b, a;

	float vtxa[8*3];
	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	};

	float *pt1, *pt2, *pt3, *pt4;

	a=1;

	glDisable(GL_TEXTURE_2D);

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=minx;
		vtxa[i*3+1]=miny;
		vtxa[i*3+2]=minz;
		if(i&1)vtxa[i*3+0]=maxx;
		if(i&2)vtxa[i*3+1]=maxy;
		if(i&4)vtxa[i*3+2]=maxz;
	}


	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		glBegin(GL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		glColor4f(r*y, g*y, b*y, a);
		glVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		glColor4f(r*y, g*y, b*y, a);
		glVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		glColor4f(r*y, g*y, b*y, a);
		glVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		glColor4f(r*y, g*y, b*y, a);
		glVertex3fv(pt4);

		glEnd();
	}
	return(0);
}

int PD3D_DrawSphere(float x, float y, float z, float ra)
{
	int i, j;
	float r, g, b, a;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

//	printf("sphere (%f %f %f) %f\n", x, y, z, r);

	a=1;

	glDisable(GL_TEXTURE_2D);
//	glEnable(GL_TEXTURE_2D);
//	Draw_Bind(texture[p->skin].num);

//	for(i=0; i<16; i++)
//		for(j=0; j<8; j++)
	for(i=0; i<8; i++)
		for(j=0; j<4; j++)
	{
//		a1=((M_PI*2)/16)*i;
//		a2=((M_PI*2)/16)*(i+1);
//		a3=((M_PI*2)/16)*j;
//		a4=((M_PI*2)/16)*(j+1);

		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);
		a3=((M_PI*2)/8)*j;
		a4=((M_PI*2)/8)*(j+1);

		pt1[0]=sin(a3)*sin(a1)*ra;
		pt1[1]=sin(a3)*cos(a1)*ra;
		pt1[2]=cos(a3)*ra;
		s1=(1.0/16.0)*i;
		t1=(-1.0/8.0)*j;

		pt2[0]=sin(a4)*sin(a1)*ra;
		pt2[1]=sin(a4)*cos(a1)*ra;
		pt2[2]=cos(a4)*ra;
		s2=(1.0/16.0)*i;
		t2=(-1.0/8.0)*(j+1);

		pt3[0]=sin(a4)*sin(a2)*ra;
		pt3[1]=sin(a4)*cos(a2)*ra;
		pt3[2]=cos(a4)*ra;
		s3=(1.0/16.0)*(i+1);
		t3=(-1.0/8.0)*(j+1);

		pt4[0]=sin(a3)*sin(a2)*ra;
		pt4[1]=sin(a3)*cos(a2)*ra;
		pt4[2]=cos(a3)*ra;
		s4=(1.0/16.0)*(i+1);
		t4=(-1.0/8.0)*j;

		pt1[0]+=x;
		pt1[1]+=y;
		pt1[2]+=z;

		pt2[0]+=x;
		pt2[1]+=y;
		pt2[2]+=z;

		pt3[0]+=x;
		pt3[1]+=y;
		pt3[2]+=z;

		pt4[0]+=x;
		pt4[1]+=y;
		pt4[2]+=z;

		glBegin(GL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		glColor4f(r, g, b, a);
//		glTexCoord2f(s1, t1);
		glVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		glColor4f(r, g, b, a);
//		glTexCoord2f(s2, t2);
		glVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		glColor4f(r, g, b, a);
//		glTexCoord2f(s3, t3);
		glVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		glColor4f(r, g, b, a);
//		glTexCoord2f(s4, t4);
		glVertex3fv(pt4);

		glEnd();
	}

//	glDisable(GL_TEXTURE_2D);

	return(0);
}

int PD3D_DrawFlatSphere(float x, float y, float z, float ra,
	float r, float g, float b, float a)
{
	int i, j, k;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	glDisable(GL_TEXTURE_2D);
	glColor4f(r, g, b, a);

	glBegin(GL_QUADS);
	for(i=0; i<16; i++)
		for(j=0; j<8; j++)
	{
		a1=((M_PI*2)/16)*i;
		a2=((M_PI*2)/16)*(i+1);
		a3=((M_PI*2)/16)*j;
		a4=((M_PI*2)/16)*(j+1);

		pt1[0]=sin(a3)*sin(a1)*ra+x;
		pt1[1]=sin(a3)*cos(a1)*ra+y;
		pt1[2]=cos(a3)*ra+z;

		pt2[0]=sin(a4)*sin(a1)*ra+x;
		pt2[1]=sin(a4)*cos(a1)*ra+y;
		pt2[2]=cos(a4)*ra+z;

		pt3[0]=sin(a4)*sin(a2)*ra+x;
		pt3[1]=sin(a4)*cos(a2)*ra+y;
		pt3[2]=cos(a4)*ra+z;

		pt4[0]=sin(a3)*sin(a2)*ra+x;
		pt4[1]=sin(a3)*cos(a2)*ra+y;
		pt4[2]=cos(a3)*ra+z;

		glVertex3fv(pt1);
		glVertex3fv(pt2);
		glVertex3fv(pt3);
		glVertex3fv(pt4);
	}
	glEnd();

//	glDisable(GL_TEXTURE_2D);

	return(0);
}

int PD3D_DrawGammaSphere(float ra, float gg)
{
	int i, j;
	float r, g, b, a;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

	a=1;

	glDisable(GL_TEXTURE_2D);

//	for(i=0; i<16; i++)
//		for(j=0; j<8; j++)
//	for(i=0; i<8; i++)
//		for(j=0; j<4; j++)
	for(i=0; i<12; i++)
		for(j=0; j<6; j++)
	{
//		a1=((M_PI*2)/16)*i;
//		a2=((M_PI*2)/16)*(i+1);
//		a3=((M_PI*2)/16)*j;
//		a4=((M_PI*2)/16)*(j+1);

//		a1=((M_PI*2)/8)*i;
//		a2=((M_PI*2)/8)*(i+1);
//		a3=((M_PI*2)/8)*j;
//		a4=((M_PI*2)/8)*(j+1);

		a1=((M_PI*2)/12)*i;
		a2=((M_PI*2)/12)*(i+1);
		a3=((M_PI*2)/12)*j;
		a4=((M_PI*2)/12)*(j+1);

		pt1[0]=sin(a3)*sin(a1)*ra;
		pt1[1]=sin(a3)*cos(a1)*ra;
		pt1[2]=cos(a3)*ra;

		pt2[0]=sin(a4)*sin(a1)*ra;
		pt2[1]=sin(a4)*cos(a1)*ra;
		pt2[2]=cos(a4)*ra;

		pt3[0]=sin(a4)*sin(a2)*ra;
		pt3[1]=sin(a4)*cos(a2)*ra;
		pt3[2]=cos(a4)*ra;

		pt4[0]=sin(a3)*sin(a2)*ra;
		pt4[1]=sin(a3)*cos(a2)*ra;
		pt4[2]=cos(a3)*ra;

		glBegin(GL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		glColor4f(r*gg, g*gg, b*gg, a);
		glVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		glColor4f(r*gg, g*gg, b*gg, a);
		glVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		glColor4f(r*gg, g*gg, b*gg, a);
		glVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		glColor4f(r*gg, g*gg, b*gg, a);
		glVertex3fv(pt4);

		glEnd();
	}

	return(0);
}


float PD3D_DrawGrid(float x, float y, float w, int ax)
{
	float rx, cx, ry, cy, ex, ey, step;

	step=1;
	while((w/step)>16)step*=2;
	while((w/step)<8)step/=2;

	rx=floor((x-w)/step)*step;
	ry=floor((y-w)/step)*step;

	ex=ceil((rx+(2*w))/step)*step;
	ey=ceil((ry+(2*w))/step)*step;

//	printf("for w=%f step=%f bx=%f rx=%f\n", w, step, x-w, rx);

	glDisable (GL_TEXTURE_2D);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor4f(0.5, 0.5, 0.5, 1);

	glBegin(GL_LINES);
	cx=rx;
	while(cx<=ex)
	{
		if(cx==0)
		{
			cx+=step;
			continue;
		}

		if(ax==1)
		{
			glVertex3f(cx, ry, 0);
			glVertex3f(cx, ey, 0);
		}else
		{
			glVertex3f(cx, 0, ry);
			glVertex3f(cx, 0, ey);
		}
		cx+=step;
	}
	glEnd();

	glBegin(GL_LINES);
	cy=ry;
	while(cy<=ey)
	{
		if(cy==0)
		{
			cy+=step;
			continue;
		}

		if(ax==1)
		{
			glVertex3f(rx, cy, 0);
			glVertex3f(ex, cy, 0);
		}else
		{
			glVertex3f(rx, 0, cy);
			glVertex3f(ex, 0, cy);
		}
		cy+=step;
	}
	glEnd();

	glBegin(GL_LINES);

	glColor4f(1, 0, 0, 1);
	glVertex3f(-1000000, 0, 0);
	glVertex3f( 1000000, 0, 0);

	glColor4f(0, 1, 0, 1);
	glVertex3f(0, -1000000, 0);
	glVertex3f(0,  1000000, 0);

	glColor4f(0, 0, 1, 1);
	glVertex3f(0, 0, -1000000);
	glVertex3f(0, 0,  1000000);

	glEnd();

//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return(step);
}

float PD3D_DrawXYGrid(float x, float y, float w)
{
	return(PD3D_DrawGrid(x, y, w, 1));
}

float PD3D_DrawXZGrid(float x, float z, float w)
{
	return(PD3D_DrawGrid(x, z, w, 2));
}

int PD3D_DrawCylinder(
	float x, float y, float z,
	float h, float rt, float rb)
{
	int i, j;
	float *v0, *v1, *v2;
	float r, g, b;

	float a1, x1, y1;
	float a2, x2, y2;
	float s1, s2;
	float pt[3], pt2[3];

//	glEnable(GL_TEXTURE_2D);
//	Draw_Bind(texture[p->skin].num);

	glDisable(GL_TEXTURE_2D);

	for(i=0; i<16; i++)
	{
		a1=((M_PI*2)/16)*i;
		s1=(1.0/16.0)*i;
		x1=sin(a1);
		y1=cos(a1);

		a2=((M_PI*2)/16)*(i+1);
		s2=(1.0/16.0)*(i+1);
		x2=sin(a2);
		y2=cos(a2);

		glBegin(GL_POLYGON);

		pt[0]=x1*rt;
		pt[1]=y1*rt;
		pt[2]=h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		glColor4f(r, g, b, 1.0);
//		glTexCoord2f(s1, 0);
		glVertex3fv(pt);

		pt[0]=x1*rb;
		pt[1]=y1*rb;
		pt[2]=-h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		glColor4f(r, g, b, 1.0);
//		glTexCoord2f(s1, 1);
		glVertex3fv(pt);

		pt[0]=x2*rb;
		pt[1]=y2*rb;
		pt[2]=-h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		glColor4f(r, g, b, 1.0);
//		glTexCoord2f(s2, 1);
		glVertex3fv(pt);

		pt[0]=x2*rt;
		pt[1]=y2*rt;
		pt[2]=h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		glColor4f(r, g, b, 1.0);
//		glTexCoord2f(s2, 0);
		glVertex3fv(pt);

		glEnd();
	}

//	glDisable(GL_TEXTURE_2D);

	return(0);
}

int PD3D_DrawDisk(float z, float h, float rad)
{
	float pt[3];
	float a1, x1, y1;
	float r, g, b;
	int i, j;

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);
	for(i=0; i<16; i++)
	{
		j=h?(15-i):i;
		a1=((M_PI*2)/16)*j;
		x1=sin(a1);
		y1=cos(a1);

		pt[0]=x1*rad;
		pt[1]=y1*rad;
		pt[2]=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		glColor4f(r, g, b, 1.0);
		glVertex3fv(pt);
	}
	glEnd();
}

int PD3D_DrawPlane(float x, float y, float z, float d, float w)
{
	float u[3], v[3], n[3], pt[3];

	n[0]=x; n[1]=y; n[2]=z;

	u[0]=y; u[1]=z; u[2]=x;
	v[0]=z; v[1]=x; v[2]=y;

//	Vec3F_AddScale(u, n, -Vec3F_DotProduct(u, n), u);
//	Vec3F_AddScale(v, n, -Vec3F_DotProduct(v, n), v);
//	Vec3F_Normalize(u, u);
//	Vec3F_Normalize(v, v);

	glBegin(GL_QUADS);

	glColor4f(0, 0.5, 0, 1);
	pt[0]=0 - u[0]*w - v[0]*w + d*x;
	pt[1]=0 - u[1]*w - v[1]*w + d*y;
	pt[2]=0 - u[2]*w - v[2]*w + d*z;
	glVertex3fv(pt);

	glColor4f(0.5, 0.5, 0, 1);
	pt[0]=0 + u[0]*w - v[0]*w + d*x;
	pt[1]=0 + u[1]*w - v[1]*w + d*y;
	pt[2]=0 + u[2]*w - v[2]*w + d*z;
	glVertex3fv(pt);

	glColor4f(0, 0.5, 0.5, 1);
	pt[0]=0 + u[0]*w + v[0]*w + d*x;
	pt[1]=0 + u[1]*w + v[1]*w + d*y;
	pt[2]=0 + u[2]*w + v[2]*w + d*z;
	glVertex3fv(pt);

	glColor4f(0.5, 0.5, 0.5, 1);
	pt[0]=0 - u[0]*w + v[0]*w + d*x;
	pt[1]=0 - u[1]*w + v[1]*w + d*y;
	pt[2]=0 - u[2]*w + v[2]*w + d*z;
	glVertex3fv(pt);

	glEnd();
}
