#include <lbxgl2.h>

// #include <GL/gl.h>
// #include <math.h>

#if 0
int PD3D_DrawSquare3D(float x, float y, float z, float xs, float ys, float zs)
{
//	pdglBegin(PDGL_QUADS);
//	pdglVertex2f(x,	y);
//	pdglVertex2f(x+w,	y);
//	pdglVertex2f(x+w,	y+h);
//	pdglVertex2f(x,	y+h);
//	pdglEnd();

	pdglBegin(PDGL_QUADS);
	pdglVertex2f(x,	y);
	pdglVertex2f(x,	y+h);
	pdglVertex2f(x+w,	y+h);
	pdglVertex2f(x+w,	y);
	pdglEnd();

	return(0);
}
#endif

LBXGL_API int PD3D_DrawFlatCube(
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

	pdglDisableTexture2D();
//	pdglEnable(PDGL_BLEND);

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

		pdglBegin(PDGL_POLYGON);

		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt1);

		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt2);

		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt3);

		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt4);

		pdglEnd();
	}
	return(0);
}

LBXGL_API int PD3D_DrawFlatLineCube(
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

	pdglDisableTexture2D();
//	pdglEnable(PDGL_BLEND);

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=minx;
		vtxa[i*3+1]=miny;
		vtxa[i*3+2]=minz;
		if(i&1)vtxa[i*3+0]=maxx;
		if(i&2)vtxa[i*3+1]=maxy;
		if(i&4)vtxa[i*3+2]=maxz;
	}

	pdglColor4f(r, g, b, a);

	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		pdglBegin(PDGL_LINES);
		pdglVertex3fv(pt1);
		pdglVertex3fv(pt2);

		pdglVertex3fv(pt2);
		pdglVertex3fv(pt3);

		pdglVertex3fv(pt3);
		pdglVertex3fv(pt4);

		pdglVertex3fv(pt4);
		pdglVertex3fv(pt1);
		pdglEnd();
	}
	return(0);
}

LBXGL_API int PD3D_DrawCube(
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

	pdglDisableTexture2D();

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

		pdglBegin(PDGL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		pdglColor4f(r, g, b, a);
		pdglVertex3fv(pt4);

		pdglEnd();
	}
	return(0);
}

LBXGL_API int PD3D_DrawGammaCube(
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

	pdglDisableTexture2D();

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

		pdglBegin(PDGL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		pdglColor4f(r*y, g*y, b*y, a);
		pdglVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		pdglColor4f(r*y, g*y, b*y, a);
		pdglVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		pdglColor4f(r*y, g*y, b*y, a);
		pdglVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		pdglColor4f(r*y, g*y, b*y, a);
		pdglVertex3fv(pt4);

		pdglEnd();
	}
	return(0);
}

LBXGL_API int PD3D_DrawTextureCube(
	float minx, float miny, float minz,
	float maxx, float maxy, float maxz,
	int texnum)
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

	a=0.5;

	pdglEnableTexture2D();
	Draw_Bind(texnum);

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

		pdglBegin(PDGL_POLYGON);

		pdglTexCoord2f(1, 1);
		pdglVertex3fv(pt1);

		pdglTexCoord2f(1, 0);
		pdglVertex3fv(pt2);

		pdglTexCoord2f(0, 0);
		pdglVertex3fv(pt3);

		pdglTexCoord2f(0, 1);
		pdglVertex3fv(pt4);

		pdglEnd();
	}
	return(0);
}

LBXGL_API int PD3D_DrawFlatCube2(
	float mx, float my, float mz, float nx, float ny, float nz)
{
	int i, j;
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

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=mx; vtxa[i*3+1]=my; vtxa[i*3+2]=mz;
		if(i&1)vtxa[i*3+0]=nx; if(i&2)vtxa[i*3+1]=ny;
		if(i&4)vtxa[i*3+2]=nz;
	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<6; i++)
	{
		pdglVertex3fv(vtxa+(vtxb[i*4+0]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+1]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+2]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+3]*3));
	}
	pdglEnd();
	return(0);
}

LBXGL_API int PD3D_DrawFlatCube3(
	float mx, float my, float mz, float nx, float ny, float nz)
{
	int i, j;
	float vtxa[8*3];
	static int vtxb[12*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2,
		5, 7, 2, 0,
		5, 1, 2, 6,
		1, 3, 6, 4,
		0, 2, 7, 5,
		1, 7, 6, 0,
		4, 2, 3, 5,
	};

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=mx; vtxa[i*3+1]=my; vtxa[i*3+2]=mz;
		if(i&1)vtxa[i*3+0]=nx; if(i&2)vtxa[i*3+1]=ny;
		if(i&4)vtxa[i*3+2]=nz;
	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<12; i++)
	{
		pdglVertex3fv(vtxa+(vtxb[i*4+0]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+1]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+2]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+3]*3));
	}
	pdglEnd();
	return(0);
}

LBXGL_API int PD3D_DrawFlatCube4(
	float mx, float my, float mz, float nx, float ny, float nz)
{
	float fb[4]={1.0, 0.75, 0.66, 0.58};
	float f, g;
	int i;

	for(i=0; i<4; i++)
	{
		f=fb[i]; g=1.0-f;
		PD3D_DrawFlatCube3(
			f*mx+g*nx, f*my+g*ny, f*mz+g*nz,
			f*nx+g*mx, f*ny+g*my, f*nz+g*mz);
	}
}

LBXGL_API int PD3D_DrawSphere(float x, float y, float z, float ra)
{
	int i, j;
	float r, g, b, a;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

//	printf("sphere (%f %f %f) %f\n", x, y, z, r);

	a=1;

	pdglDisableTexture2D();
//	pdglEnableTexture2D();
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

		pdglBegin(PDGL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		pdglColor4f(r, g, b, a);
//		pdglTexCoord2f(s1, t1);
		pdglVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		pdglColor4f(r, g, b, a);
//		pdglTexCoord2f(s2, t2);
		pdglVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		pdglColor4f(r, g, b, a);
//		pdglTexCoord2f(s3, t3);
		pdglVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		pdglColor4f(r, g, b, a);
//		pdglTexCoord2f(s4, t4);
		pdglVertex3fv(pt4);

		pdglEnd();
	}

//	pdglDisableTexture2D();

	return(0);
}

LBXGL_API int PD3D_DrawFlatSphere(float x, float y, float z, float ra,
	float r, float g, float b, float a)
{
	int i, j, k;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	pdglDisableTexture2D();
	pdglColor4f(r, g, b, a);

	pdglBegin(PDGL_QUADS);
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

		pdglVertex3fv(pt1);
		pdglVertex3fv(pt2);
		pdglVertex3fv(pt3);
		pdglVertex3fv(pt4);
	}
	pdglEnd();

//	pdglDisableTexture2D();

	return(0);
}

LBXGL_API int PD3D_DrawGammaSphere(float ra, float gg)
{
	int i, j;
	float r, g, b, a;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

	a=1;

	pdglDisableTexture2D();

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

		pdglBegin(PDGL_POLYGON);

		r=0.5+(sin(pt1[0])*0.5);
		g=0.5+(sin(pt1[1])*0.5);
		b=0.5+(sin(pt1[2])*0.5);
		pdglColor4f(r*gg, g*gg, b*gg, a);
		pdglVertex3fv(pt1);

		r=0.5+(sin(pt2[0])*0.5);
		g=0.5+(sin(pt2[1])*0.5);
		b=0.5+(sin(pt2[2])*0.5);
		pdglColor4f(r*gg, g*gg, b*gg, a);
		pdglVertex3fv(pt2);

		r=0.5+(sin(pt3[0])*0.5);
		g=0.5+(sin(pt3[1])*0.5);
		b=0.5+(sin(pt3[2])*0.5);
		pdglColor4f(r*gg, g*gg, b*gg, a);
		pdglVertex3fv(pt3);

		r=0.5+(sin(pt4[0])*0.5);
		g=0.5+(sin(pt4[1])*0.5);
		b=0.5+(sin(pt4[2])*0.5);
		pdglColor4f(r*gg, g*gg, b*gg, a);
		pdglVertex3fv(pt4);

		pdglEnd();
	}

	return(0);
}


LBXGL_API float PD3D_DrawGrid(float x, float y, float w, int ax)
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

	pdglDisableTexture2D();
//	pdglPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	pdglColor4f(0.5, 0.5, 0.5, 1);

	pdglBegin(PDGL_LINES);
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
			pdglVertex3f(cx, ry, 0);
			pdglVertex3f(cx, ey, 0);
		}else if(ax==2)
		{
			pdglVertex3f(cx, 0, ry);
			pdglVertex3f(cx, 0, ey);
		}else
		{
			pdglVertex3f(0, cx, ry);
			pdglVertex3f(0, cx, ey);
		}
		cx+=step;
	}
	pdglEnd();

	pdglBegin(PDGL_LINES);
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
			pdglVertex3f(rx, cy, 0);
			pdglVertex3f(ex, cy, 0);
		}else if(ax==2)
		{
			pdglVertex3f(rx, 0, cy);
			pdglVertex3f(ex, 0, cy);
		}else
		{
			pdglVertex3f(0, rx, cy);
			pdglVertex3f(0, ex, cy);
		}
		cy+=step;
	}
	pdglEnd();

	pdglBegin(PDGL_LINES);

	pdglColor4f(1, 0, 0, 1);
	pdglVertex3f(-10000, 0, 0);
	pdglVertex3f( 10000, 0, 0);

	pdglColor4f(0, 1, 0, 1);
	pdglVertex3f(0, -10000, 0);
	pdglVertex3f(0,  10000, 0);

	pdglColor4f(0, 0, 1, 1);
	pdglVertex3f(0, 0, -10000);
	pdglVertex3f(0, 0,  10000);

	pdglEnd();

//	pdglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return(step);
}

LBXGL_API float PD3D_DrawXYGrid(float x, float y, float w)
{
	return(PD3D_DrawGrid(x, y, w, 1));
}

LBXGL_API float PD3D_DrawXZGrid(float x, float z, float w)
{
	return(PD3D_DrawGrid(x, z, w, 2));
}

LBXGL_API float PD3D_DrawYZGrid(float y, float z, float w)
{
	return(PD3D_DrawGrid(y, z, w, 3));
}

LBXGL_API void PD3D_DrawGrid2(float w, float step)
{
	float rx, cx, ry, cy, ex, ey;

	rx=floor(-w/step)*step;
	ry=floor(-w/step)*step;

	ex=ceil(w/step)*step;
	ey=ceil(w/step)*step;

//	pdglDisable (GL_TEXTURE_2D);
//	pdglColor4f(0.5, 0.5, 0.5, 1);

	pdglBegin(PDGL_LINES);
	cx=rx;
	while(cx<=ex)
	{
		if(cx==0)
		{
			cx+=step;
			continue;
		}

		pdglVertex3f(cx, ry, 0);
		pdglVertex3f(cx, ey, 0);

		cx+=step;
	}
	pdglEnd();

	pdglBegin(PDGL_LINES);
	cy=ry;
	while(cy<=ey)
	{
		if(cy==0)
		{
			cy+=step;
			continue;
		}

		pdglVertex3f(rx, cy, 0);
		pdglVertex3f(ex, cy, 0);
		cy+=step;
	}
	pdglEnd();

	pdglBegin(PDGL_LINES);

	pdglColor4f(1, 0, 0, 1);
	pdglVertex3f(-1000000, 0, 0);
	pdglVertex3f( 1000000, 0, 0);

	pdglColor4f(0, 1, 0, 1);
	pdglVertex3f(0, -1000000, 0);
	pdglVertex3f(0,  1000000, 0);

	pdglColor4f(0, 0, 1, 1);
	pdglVertex3f(0, 0, -1000000);
	pdglVertex3f(0, 0,  1000000);

	pdglEnd();
}

LBXGL_API void PD3D_DrawGrid3(float w, float step, int ax)
{
	float rx, cx, ry, cy, ex, ey;

	rx=floor(-w/step)*step;
	ry=floor(-w/step)*step;

	ex=ceil(w/step)*step;
	ey=ceil(w/step)*step;

//	pdglDisable (GL_TEXTURE_2D);
//	pdglColor4f(0.5, 0.5, 0.5, 1);

	pdglBegin(PDGL_LINES);
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
			pdglVertex3f(cx, ry, 0);
			pdglVertex3f(cx, ey, 0);
		}else if(ax==2)
		{
			pdglVertex3f(cx, 0, ry);
			pdglVertex3f(cx, 0, ey);
		}else
		{
			pdglVertex3f(0, cx, ry);
			pdglVertex3f(0, cx, ey);
		}

		cx+=step;
	}
	pdglEnd();

	pdglBegin(PDGL_LINES);
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
			pdglVertex3f(rx, cy, 0);
			pdglVertex3f(ex, cy, 0);
		}else if(ax==2)
		{
			pdglVertex3f(rx, 0, cy);
			pdglVertex3f(ex, 0, cy);
		}else
		{
			pdglVertex3f(0, rx, cy);
			pdglVertex3f(0, ex, cy);
		}

		cy+=step;
	}
	pdglEnd();

#if 0
	pdglBegin(PDGL_LINES);

	pdglColor4f(1, 0, 0, 1);
	pdglVertex3f(-10000, 0, 0);
	pdglVertex3f( 10000, 0, 0);

	pdglColor4f(0, 1, 0, 1);
	pdglVertex3f(0, -10000, 0);
	pdglVertex3f(0,  10000, 0);

	pdglColor4f(0, 0, 1, 1);
	pdglVertex3f(0, 0, -10000);
	pdglVertex3f(0, 0,  10000);

	pdglEnd();
#endif

#if 1
	pdglColor4f(1, 0, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(     0, 0, 0);
	pdglVertex3f( 10000, 0, 0);
	pdglEnd();

	pdglColor4f(0, 1, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0,      0, 0);
	pdglVertex3f(0,  10000, 0);
	pdglEnd();

	pdglColor4f(0, 0, 1, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0, 0,      0);
	pdglVertex3f(0, 0,  10000);
	pdglEnd();

	pdglColor4f(0.5, 0, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(-10000, 0, 0);
	pdglVertex3f(     0, 0, 0);
	pdglEnd();

	pdglColor4f(0, 0.5, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0, -10000, 0);
	pdglVertex3f(0,      0, 0);
	pdglEnd();

	pdglColor4f(0, 0, 0.5, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0, 0, -10000);
	pdglVertex3f(0, 0,      0);
	pdglEnd();
#endif
}

LBXGL_API void PD3D_DrawXYGrid2(float w, float s)
{
	PD3D_DrawGrid3(w, s, 1);
}

LBXGL_API void PD3D_DrawXZGrid2(float w, float s)
{
	PD3D_DrawGrid3(w, s, 2);
}

LBXGL_API void PD3D_DrawYZGrid2(float w, float s)
{
	PD3D_DrawGrid3(w, s, 3);
}

LBXGL_API void PD3D_DrawGrid3D(float w, float step)
{
	float rx, cx, ry, cy, rz, cz, ex, ey, ez;

	rx=floor(-w/step)*step;
	ry=floor(-w/step)*step;
	rz=floor(-w/step)*step;

	ex=ceil(w/step)*step;
	ey=ceil(w/step)*step;
	ez=ceil(w/step)*step;

//	pdglDisable (GL_TEXTURE_2D);
//	pdglColor4f(0.5, 0.5, 0.5, 1);

	cz=rz;
	while(cz<=ez)
	{
		pdglBegin(PDGL_LINES);
		cx=rx;
		while(cx<=ex)
		{
//			if(cx==0)
//			{
//				cx+=step;
//				continue;
//			}

			pdglVertex3f(cx, ry, cz);
			pdglVertex3f(cx, ey, cz);

//			pdglVertex3f(cx, 0, ry);
//			pdglVertex3f(cx, 0, ey);

			cx+=step;
		}
		pdglEnd();

		pdglBegin(PDGL_LINES);
		cy=ry;
		while(cy<=ey)
		{
//			if(cy==0)
//			{
//				cy+=step;
//				continue;
//			}

			pdglVertex3f(rx, cy, cz);
			pdglVertex3f(ex, cy, cz);

//			pdglVertex3f(rx, 0, cy);
//			pdglVertex3f(ex, 0, cy);

			cy+=step;
		}
		pdglEnd();

		cz+=step;
	}

#if 1
	pdglBegin(PDGL_LINES);
	cx=rx;
	while(cx<=ex)
	{
//		if(cx==0)
//		{
//			cx+=step;
//			continue;
//		}

		cy=ry;
		while(cy<=ey)
		{
//			if(cy==0)
//			{
//				cy+=step;
//				continue;
//			}

			pdglVertex3f(cx, cy, rz);
			pdglVertex3f(cx, cy, ez);

			cy+=step;
		}

		cx+=step;
	}
	pdglEnd();
#endif

#if 0
	pdglBegin(PDGL_LINES);

	pdglColor4f(1, 0, 0, 1);
	pdglVertex3f(-10000, 0, 0);
	pdglVertex3f( 10000, 0, 0);

	pdglColor4f(0, 1, 0, 1);
	pdglVertex3f(0, -10000, 0);
	pdglVertex3f(0,  10000, 0);

	pdglColor4f(0, 0, 1, 1);
	pdglVertex3f(0, 0, -10000);
	pdglVertex3f(0, 0,  10000);

	pdglEnd();
#endif

#if 1
	pdglColor4f(1, 0, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(     0, 0, 0);
	pdglVertex3f( 10000, 0, 0);
	pdglEnd();

	pdglColor4f(0, 1, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0,      0, 0);
	pdglVertex3f(0,  10000, 0);
	pdglEnd();

	pdglColor4f(0, 0, 1, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0, 0,      0);
	pdglVertex3f(0, 0,  10000);
	pdglEnd();

	pdglColor4f(0.5, 0, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(-10000, 0, 0);
	pdglVertex3f(     0, 0, 0);
	pdglEnd();

	pdglColor4f(0, 0.5, 0, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0, -10000, 0);
	pdglVertex3f(0,      0, 0);
	pdglEnd();

	pdglColor4f(0, 0, 0.5, 1);
	pdglBegin(PDGL_LINES);
	pdglVertex3f(0, 0, -10000);
	pdglVertex3f(0, 0,      0);
	pdglEnd();
#endif
}


LBXGL_API int PD3D_DrawCylinder(
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

//	pdglEnableTexture2D();
//	Draw_Bind(texture[p->skin].num);

	pdglDisableTexture2D();

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

		pdglBegin(PDGL_POLYGON);

		pt[0]=x1*rt;
		pt[1]=y1*rt;
		pt[2]=h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		pdglColor4f(r, g, b, 1.0);
//		pdglTexCoord2f(s1, 0);
		pdglVertex3fv(pt);

		pt[0]=x1*rb;
		pt[1]=y1*rb;
		pt[2]=-h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		pdglColor4f(r, g, b, 1.0);
//		pdglTexCoord2f(s1, 1);
		pdglVertex3fv(pt);

		pt[0]=x2*rb;
		pt[1]=y2*rb;
		pt[2]=-h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		pdglColor4f(r, g, b, 1.0);
//		pdglTexCoord2f(s2, 1);
		pdglVertex3fv(pt);

		pt[0]=x2*rt;
		pt[1]=y2*rt;
		pt[2]=h/2;
		pt[0]+=x;
		pt[1]+=y;
		pt[2]+=z;
		r=0.5+(sin(pt[0])*0.5);
		g=0.5+(sin(pt[1])*0.5);
		b=0.5+(sin(pt[2])*0.5);
		pdglColor4f(r, g, b, 1.0);
//		pdglTexCoord2f(s2, 0);
		pdglVertex3fv(pt);

		pdglEnd();
	}

//	pdglDisableTexture2D();

	return(0);
}

LBXGL_API int PD3D_DrawDisk(float z, float h, float rad)
{
	float pt[3];
	float a1, x1, y1;
	float r, g, b;
	int i, j;

	pdglDisableTexture2D();

	pdglBegin(PDGL_POLYGON);
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
		pdglColor4f(r, g, b, 1.0);
		pdglVertex3fv(pt);
	}
	pdglEnd();
	return(0);
}

LBXGL_API int PD3D_DrawPlane(float x, float y, float z, float d, float w)
{
	float u[3], v[3], n[3], pt[3];

	n[0]=x; n[1]=y; n[2]=z;

	u[0]=y; u[1]=z; u[2]=x;
	v[0]=z; v[1]=x; v[2]=y;

//	Vec3F_AddScale(u, n, -Vec3F_DotProduct(u, n), u);
//	Vec3F_AddScale(v, n, -Vec3F_DotProduct(v, n), v);
//	Vec3F_Normalize(u, u);
//	Vec3F_Normalize(v, v);

	pdglBegin(PDGL_QUADS);

	pdglColor4f(0, 0.5, 0, 1);
	pt[0]=0 - u[0]*w - v[0]*w + d*x;
	pt[1]=0 - u[1]*w - v[1]*w + d*y;
	pt[2]=0 - u[2]*w - v[2]*w + d*z;
	pdglVertex3fv(pt);

	pdglColor4f(0.5, 0.5, 0, 1);
	pt[0]=0 + u[0]*w - v[0]*w + d*x;
	pt[1]=0 + u[1]*w - v[1]*w + d*y;
	pt[2]=0 + u[2]*w - v[2]*w + d*z;
	pdglVertex3fv(pt);

	pdglColor4f(0, 0.5, 0.5, 1);
	pt[0]=0 + u[0]*w + v[0]*w + d*x;
	pt[1]=0 + u[1]*w + v[1]*w + d*y;
	pt[2]=0 + u[2]*w + v[2]*w + d*z;
	pdglVertex3fv(pt);

	pdglColor4f(0.5, 0.5, 0.5, 1);
	pt[0]=0 - u[0]*w + v[0]*w + d*x;
	pt[1]=0 - u[1]*w + v[1]*w + d*y;
	pt[2]=0 - u[2]*w + v[2]*w + d*z;
	pdglVertex3fv(pt);

	pdglEnd();
	return(0);
}


LBXGL_API void PD3D_AdjacentNormals(float *norm, float *udir, float *vdir)
{
	float f;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	if(fabs(norm[0])>fabs(norm[1]))
	{
		if(fabs(norm[0])>fabs(norm[2]))
		{
			udir[(norm[0]>0)?1:2]=1; 
			vdir[(norm[0]>0)?2:1]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}else
	{
		if(fabs(norm[1])>fabs(norm[2]))
		{
			udir[(norm[1]>0)?2:0]=1; 
			vdir[(norm[1]>0)?0:2]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}

	f=V3F_DOT(udir, norm);
	V3F_ADDSCALE(udir, norm, -f, udir);
	f=V3F_DOT(vdir, norm);
	V3F_ADDSCALE(vdir, norm, -f, vdir);

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

LBXGL_API int PD3D_DrawFlatCapsule(float *v0, float *v1, float ra,
	float r, float g, float b, float a)
{
	float u[3], v[3], nv[3], pt[3];
	float tv0[3], tv1[3];
	float a1, a2, a3, a4, x1, y1, x2, y2;
	int i, j;

	V3F_SUB(v1, v0, nv);
	V3F_NORMALIZE(nv, nv);
	PD3D_AdjacentNormals(nv, u, v);

	pdglDisableTexture2D();
	pdglColor4f(r, g, b, a);

	pdglBegin(PDGL_QUADS);

	for(i=0; i<8; i++)
	{
		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);

		x1=sin(a1)*ra; y1=cos(a1)*ra;
		x2=sin(a2)*ra; y2=cos(a2)*ra;

		tv0[0]=x1*u[0]+y1*v[0];
		tv0[1]=x1*u[1]+y1*v[1];
		tv0[2]=x1*u[2]+y1*v[2];

		tv1[0]=x2*u[0]+y2*v[0];
		tv1[1]=x2*u[1]+y2*v[1];
		tv1[2]=x2*u[2]+y2*v[2];


		pt[0]=v1[0]+tv0[0];
		pt[1]=v1[1]+tv0[1];
		pt[2]=v1[2]+tv0[2];
		pdglVertex3fv(pt);

		pt[0]=v0[0]+tv0[0];
		pt[1]=v0[1]+tv0[1];
		pt[2]=v0[2]+tv0[2];
		pdglVertex3fv(pt);

		pt[0]=v0[0]+tv1[0];
		pt[1]=v0[1]+tv1[1];
		pt[2]=v0[2]+tv1[2];
		pdglVertex3fv(pt);

		pt[0]=v1[0]+tv1[0];
		pt[1]=v1[1]+tv1[1];
		pt[2]=v1[2]+tv1[2];
		pdglVertex3fv(pt);
	}

	for(i=0; i<8; i++)
		for(j=2; j<4; j++)
	{
		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);
		a3=((M_PI*2)/8)*j;
		a4=((M_PI*2)/8)*(j+1);

		pt[0]=sin(a3)*sin(a1)*ra;
		pt[1]=sin(a3)*cos(a1)*ra;
		pt[2]=cos(a3)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];
		pdglVertex3fv(tv0);

		pt[0]=sin(a4)*sin(a1)*ra;
		pt[1]=sin(a4)*cos(a1)*ra;
		pt[2]=cos(a4)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];
		pdglVertex3fv(tv0);

		pt[0]=sin(a4)*sin(a2)*ra;
		pt[1]=sin(a4)*cos(a2)*ra;
		pt[2]=cos(a4)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];
		pdglVertex3fv(tv0);

		pt[0]=sin(a3)*sin(a2)*ra;
		pt[1]=sin(a3)*cos(a2)*ra;
		pt[2]=cos(a3)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];
		pdglVertex3fv(tv0);
	}

	for(i=0; i<8; i++)
		for(j=0; j<2; j++)
	{
		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);
		a3=((M_PI*2)/8)*j;
		a4=((M_PI*2)/8)*(j+1);

		pt[0]=sin(a3)*sin(a1)*ra;
		pt[1]=sin(a3)*cos(a1)*ra;
		pt[2]=cos(a3)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];
		pdglVertex3fv(tv0);

		pt[0]=sin(a4)*sin(a1)*ra;
		pt[1]=sin(a4)*cos(a1)*ra;
		pt[2]=cos(a4)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];
		pdglVertex3fv(tv0);

		pt[0]=sin(a4)*sin(a2)*ra;
		pt[1]=sin(a4)*cos(a2)*ra;
		pt[2]=cos(a4)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];
		pdglVertex3fv(tv0);

		pt[0]=sin(a3)*sin(a2)*ra;
		pt[1]=sin(a3)*cos(a2)*ra;
		pt[2]=cos(a3)*ra;
		tv0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		tv0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		tv0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];
		pdglVertex3fv(tv0);
	}

	pdglEnd();
	return(0);
}
