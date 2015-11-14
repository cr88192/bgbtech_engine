#include <lbxgl2.h>

// #include <GL/gl.h>
// #include <math.h>


#if 0
int Prim3D_DrawSquare3D(float x, float y, float z, float xs, float ys, float zs)
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

LBXGL_API int Prim3D_DrawFlatCube(
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

LBXGL_API int Prim3D_DrawFlatLineCube(
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
//	glEnable(GL_BLEND);

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

LBXGL_API int Prim3D_DrawCube(
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

	a=0.5;

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

LBXGL_API int Prim3D_DrawTextureCube(
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

LBXGL_API int Prim3D_DrawSphere(float x, float y, float z, float ra)
{
	int i, j;
	float r, g, b, a;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

//	printf("sphere (%f %f %f) %f\n", x, y, z, r);

	a=0.5;

	pdglDisableTexture2D();
//	pdglEnableTexture2D();
//	Draw_Bind(texture[p->skin].num);

	for(i=0; i<16; i++)
		for(j=0; j<8; j++)
	{
		a1=((M_PI*2)/16)*i;
		a2=((M_PI*2)/16)*(i+1);
		a3=((M_PI*2)/16)*j;
		a4=((M_PI*2)/16)*(j+1);

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

LBXGL_API int Prim3D_DrawFlatSphere(float x, float y, float z, float ra,
	float r, float g, float b, float a)
{
	int i, j, k;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

//	printf("sphere (%f %f %f) %f,  %f %f %f %f\n", x, y, z, r,
//		r, g, b, a);

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

LBXGL_API int Prim3D_DrawTextureSphere(float x, float y, float z, float ra,
	int tex)
{
	int i, j;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

	pdglEnableTexture2D();
	Draw_Bind(tex);

	for(i=0; i<16; i++)
		for(j=0; j<8; j++)
	{
		a1=((M_PI*2)/16)*i;	a2=((M_PI*2)/16)*(i+1);
		a3=((M_PI*2)/16)*j;	a4=((M_PI*2)/16)*(j+1);

		pt1[0]=sin(a3)*sin(a1)*ra;	pt1[1]=sin(a3)*cos(a1)*ra;
		pt1[2]=cos(a3)*ra;	s1=(1.0/16.0)*i; t1=(-1.0/8.0)*j;
		pt2[0]=sin(a4)*sin(a1)*ra;	pt2[1]=sin(a4)*cos(a1)*ra;
		pt2[2]=cos(a4)*ra;	s2=(1.0/16.0)*i; t2=(-1.0/8.0)*(j+1);
		pt3[0]=sin(a4)*sin(a2)*ra;	pt3[1]=sin(a4)*cos(a2)*ra;
		pt3[2]=cos(a4)*ra;	s3=(1.0/16.0)*(i+1); t3=(-1.0/8.0)*(j+1);
		pt4[0]=sin(a3)*sin(a2)*ra;	pt4[1]=sin(a3)*cos(a2)*ra;
		pt4[2]=cos(a3)*ra;	s4=(1.0/16.0)*(i+1); t4=(-1.0/8.0)*j;

		pt1[0]+=x;	pt1[1]+=y;	pt1[2]+=z;
		pt2[0]+=x;	pt2[1]+=y;	pt2[2]+=z;
		pt3[0]+=x;	pt3[1]+=y;	pt3[2]+=z;
		pt4[0]+=x;	pt4[1]+=y;	pt4[2]+=z;

		pdglBegin(PDGL_POLYGON);
		pdglTexCoord2f(s1, t1);	pdglVertex3fv(pt1);
		pdglTexCoord2f(s2, t2);	pdglVertex3fv(pt2);
		pdglTexCoord2f(s3, t3);	pdglVertex3fv(pt3);
		pdglTexCoord2f(s4, t4);	pdglVertex3fv(pt4);
		pdglEnd();
	}

	return(0);
}


LBXGL_API float Prim3D_DrawGrid(float x, float y, float w, int ax)
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

	pdglColor4f(0.5, 0.5, 0.5, 1);

	pdglBegin(GL_LINES);
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
		}else
		{
			pdglVertex3f(cx, 0, ry);
			pdglVertex3f(cx, 0, ey);
		}
		cx+=step;
	}
	pdglEnd();

	pdglBegin(GL_LINES);
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
		}else
		{
			pdglVertex3f(rx, 0, cy);
			pdglVertex3f(ex, 0, cy);
		}
		cy+=step;
	}
	pdglEnd();

	pdglBegin(GL_LINES);

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

//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return(step);
}

LBXGL_API float Prim3D_DrawXYGrid(float x, float y, float w)
{
	return(Prim3D_DrawGrid(x, y, w, 1));
}

LBXGL_API float Prim3D_DrawXZGrid(float x, float z, float w)
{
	return(Prim3D_DrawGrid(x, z, w, 2));
}

LBXGL_API int Prim3D_DrawCylinder(
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


LBXGL_API int Prim3D_DrawTextureCylinder(
	float x, float y, float z,
	float h, float rt, float rb, int tex)
{
	int i, j;
	float *v0, *v1, *v2;
	float r, g, b;

	float a1, x1, y1;
	float a2, x2, y2;
	float s1, s2;
	float pt[3], pt2[3];

	pdglEnableTexture2D();
	Draw_Bind(tex);

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
		pt[0]=x1*rt;	pt[1]=y1*rt;	pt[2]=h/2;
		pt[0]+=x;	pt[1]+=y;	pt[2]+=z;
		pdglTexCoord2f(s1, 0);	pdglVertex3fv(pt);
		pt[0]=x1*rb;	pt[1]=y1*rb;	pt[2]=-h/2;
		pt[0]+=x;	pt[1]+=y;	pt[2]+=z;
		pdglTexCoord2f(s1, 1);	pdglVertex3fv(pt);
		pt[0]=x2*rb;	pt[1]=y2*rb;	pt[2]=-h/2;
		pt[0]+=x;	pt[1]+=y;	pt[2]+=z;
		pdglTexCoord2f(s2, 1);	pdglVertex3fv(pt);
		pt[0]=x2*rt;	pt[1]=y2*rt;	pt[2]=h/2;
		pt[0]+=x;	pt[1]+=y;	pt[2]+=z;
		pdglTexCoord2f(s2, 0);	pdglVertex3fv(pt);
		pdglEnd();
	}

	return(0);
}
