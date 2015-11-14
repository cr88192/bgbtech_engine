#include <lbxgl2.h>

static int iabs(int x)
{
	return((x<0)?-x:x);
}

double lbxgl_skyplane_camorg[3];
int lbxgl_skyplane_skynum;
int lbxgl_skyplane_sunnum;
double lbxgl_skyplane_light;

int LBXGL_SkyPlane_SetCamera(double x, double y, double z)
{
	double w;

	lbxgl_skyplane_camorg[0]=x;
	lbxgl_skyplane_camorg[1]=y;
	lbxgl_skyplane_camorg[2]=z;

	w=VecND_Length(lbxgl_skyplane_camorg, 2);
	if(w<1)w=1;

	w=(81920.0/w);
	if(w>1)w=1;

	w=1;

	lbxgl_skyplane_light=w;

	return(0);
}

int LBXGL_SkyPlane_SetSky(char *name)
{
	lbxgl_skyplane_skynum=LBXGL_Texture_LoadImage(name);
	lbxgl_skyplane_sunnum=LBXGL_Texture_LoadImage(
		"textures/effects/sun0.tga");

	return(0);
}

int LBXGL_SkyPlane_RenderGround(int t)
{
	int i, j, vx, vy;
	double bx, by, ll;
	int dx, dy;
	double d1, d2, d3, d4, ds;
	int vr;

	ll=lbxgl_skyplane_light;

	vx=0;
	vy=0;
	vx=lbxgl_skyplane_camorg[0]/65536;
	vy=lbxgl_skyplane_camorg[1]/65536;
	vr=32;
	ds=10000.0/vr;

	pdglDisableTexture2D();

	pdglBegin(PDGL_QUADS);
	for(i=(vy-vr); i<(vy+vr); i++)for(j=(vx-vr); j<(vx+vr); j++)
	{
		bx=(j*65536);
		by=(i*65536);

		dx=iabs(j-vx);
		dy=iabs(i-vy);
		d1=(dx+dy)*ds;
		dy=iabs((i+1)-vy);
		d2=(dx+dy)*ds;
		dx=iabs((j+1)-vx);
		d3=(dx+dy)*ds;
		dy=iabs(i-vy);
		d4=(dx+dy)*ds;

		pdglColor4f(0.7*ll, 0.4*ll, 0*ll, 1);
		pdglVertex3f(bx, by, -100-d1);
		pdglVertex3f(bx, by+65536, -100-d2);
		pdglVertex3f(bx+65536, by+65536, -100-d3);
		pdglVertex3f(bx+65536, by, -100-d4);
	}

	pdglEnd();

	pdglEnableTexture2D();

	Draw_Bind(lbxgl_skyplane_skynum);
	pdglBegin(PDGL_QUADS);

	for(i=(vy-vr); i<(vy+vr); i++)for(j=(vx-vr); j<(vx+vr); j++)
	{
		bx=(j*65536);
		by=(i*65536);

		dx=iabs(j-vx);
		dy=iabs(i-vy);
		d1=(dx+dy)*ds;
		dy=iabs((i+1)-vy);
		d2=(dx+dy)*ds;
		dx=iabs((j+1)-vx);
		d3=(dx+dy)*ds;
		dy=iabs(i-vy);
		d4=(dx+dy)*ds;

//		pdglColor4f(1*ll, 1*ll, 1*ll, 1);
//		pdglBegin(PDGL_QUADS);

		pdglColor4f(0.7*ll, 0.4*ll, 0*ll, 0.75);

		pdglTexCoord2f(0, 0);
		pdglVertex3f(bx, by, -100-d1);
		pdglTexCoord2f(0, 10);
		pdglVertex3f(bx, by+65536, -100-d2);
		pdglTexCoord2f(10, 10);
		pdglVertex3f(bx+65536, by+65536, -100-d3);
		pdglTexCoord2f(10, 0);
		pdglVertex3f(bx+65536, by, -100-d4);
	}

	pdglEnd();
	return(0);
}

int LBXGL_SkyPlane_RenderSky(int t)
{
	int i, j;
	double bx, by;

	pdglDisableTexture2D();

	pdglBegin(PDGL_QUADS);
	pdglColor4f(0, 0, 1, 0.5);
	pdglVertex3f(-65536*1024, -65536*1024, 50000);
	pdglVertex3f(-65536*1024, 65536*1024, 50000);
	pdglVertex3f(65536*1024, 65536*1024, 50000);
	pdglVertex3f(65536*1024, -65536*1024, 50000);

	pdglColor4f(0, 0, 1, 0.5);
	pdglVertex3f(-65536*1024, -65536*1024, 35000);
	pdglVertex3f(-65536*1024, 65536*1024, 35000);
	pdglVertex3f(65536*1024, 65536*1024, 35000);
	pdglVertex3f(65536*1024, -65536*1024, 35000);

	pdglEnd();

	pdglEnableTexture2D();
	glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_COLOR);

	Draw_Bind(lbxgl_skyplane_sunnum);
	pdglBegin(PDGL_QUADS);

	pdglColor4f(1, 1, 1, 1);
	bx=-(t/1000.0)*100;
	by=0;
	i=4096;

	pdglTexCoord2f(0, 0);
	pdglVertex3f(bx-i, by-i, 30000);
	pdglTexCoord2f(0, 1);
	pdglVertex3f(bx-i, by+i, 30000);
	pdglTexCoord2f(1, 1);
	pdglVertex3f(bx+i, by+i, 30000);
	pdglTexCoord2f(1, 0);
	pdglVertex3f(bx+i, by-i, 30000);
	pdglEnd();	

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Draw_Bind(lbxgl_skyplane_skynum);
	pdglBegin(PDGL_QUADS);

	pdglColor4f(1, 1, 1, 1);

	pdglTexCoord2f(0+(t/500000.0), 0);
	pdglVertex3f(-65536*1024, -65536*1024, 10000);
	pdglTexCoord2f(0+(t/500000.0), 100);
	pdglVertex3f(-65536*1024, 65536*1024, 10000);
	pdglTexCoord2f(100+(t/500000.0), 100);
	pdglVertex3f(65536*1024, 65536*1024, 10000);
	pdglTexCoord2f(100+(t/500000.0), 0);
	pdglVertex3f(65536*1024, -65536*1024, 10000);

	pdglColor4f(1, 1, 1, 0.75);

	pdglTexCoord2f(0-(t/10000000.0), 0);
	pdglVertex3f(-65536*1024, -65536*1024, 25000);
	pdglTexCoord2f(0-(t/10000000.0), 10);
	pdglVertex3f(-65536*1024, 65536*1024, 25000);
	pdglTexCoord2f(10-(t/10000000.0), 10);
	pdglVertex3f(65536*1024, 65536*1024, 25000);
	pdglTexCoord2f(10-(t/10000000.0), 0);
	pdglVertex3f(65536*1024, -65536*1024, 25000);
	pdglEnd();
	return(0);
}

int LBXGL_SkyPlane_RenderSky2(int t)
{
	int i, j, vx, vy;
	double bx, by, tx, ty;
	double ts, s1, t1, s2, t2;
	int dx, dy;
	double d1, d2, d3, d4, ds;
	double ll;
	int vr;

	ll=lbxgl_skyplane_light;

	vx=0;
	vy=0;
	vx=lbxgl_skyplane_camorg[0]/65536;
	vy=lbxgl_skyplane_camorg[1]/65536;
	vr=32;
	ds=50000.0/vr;

	pdglDisableTexture2D();

	pdglBegin(PDGL_QUADS);
	for(i=(vy-vr); i<(vy+vr); i++)for(j=(vx-vr); j<(vx+vr); j++)
	{
		bx=(j*65536);
		by=(i*65536);

		dx=iabs(j-vx);
		dy=iabs(i-vy);
		d1=(dx+dy)*ds;
		dy=iabs((i+1)-vy);
		d2=(dx+dy)*ds;
		dx=iabs((j+1)-vx);
		d3=(dx+dy)*ds;
		dy=iabs(i-vy);
		d4=(dx+dy)*ds;

		pdglColor4f(0, 0, 1*(ll*ll), 1);
//		pdglVertex3f(-65536*1024, -65536*1024, 50000);
//		pdglVertex3f(-65536*1024, 65536*1024, 50000);
//		pdglVertex3f(65536*1024, 65536*1024, 50000);
//		pdglVertex3f(65536*1024, -65536*1024, 50000);

//		pdglVertex3f(-65536*1024, -65536*1024, 35000);
//		pdglVertex3f(-65536*1024, 65536*1024, 35000);
//		pdglVertex3f(65536*1024, 65536*1024, 35000);
//		pdglVertex3f(65536*1024, -65536*1024, 35000);

		pdglVertex3f(bx, by, 50000-d1);
		pdglVertex3f(bx, by+65536, 50000-d2);
		pdglVertex3f(bx+65536, by+65536, 50000-d3);
		pdglVertex3f(bx+65536, by, 50000-d4);

//		pdglVertex3f(bx, by, 35000-d1);
//		pdglVertex3f(bx, by+65536, 35000-d2);
//		pdglVertex3f(bx+65536, by+65536, 35000-d3);
//		pdglVertex3f(bx+65536, by, 35000-d4);
	}
	pdglEnd();

#if 0
	pdglEnableTexture2D();
//	glBlendFunc (GL_ONE, GL_ONE_MINUS_SRC_COLOR);

//	Draw_Bind(lbxgl_skyplane_sunnum);
//	pdglBegin(PDGL_QUADS);

	pdglColor4f(10, 10, 10, 1);
	bx=-(t/1000.0)*100;
	by=0;
	i=4096;

	pdglTexCoord2f(0, 0);
	pdglVertex3f(bx-i, by-i, 30000);
	pdglTexCoord2f(0, 1);
	pdglVertex3f(bx-i, by+i, 30000);
	pdglTexCoord2f(1, 1);
	pdglVertex3f(bx+i, by+i, 30000);
	pdglTexCoord2f(1, 0);
	pdglVertex3f(bx+i, by-i, 30000);
	pdglEnd();
#endif

	bx=-(t/1000.0)*100;
	by=0;
	j=4096;

	pdglColor4f(10, 10, 10, 1);
	pdglBegin(PDGL_POLYGON);
	for(i=0; i<16; i++)
	{
		tx=bx+(sin(M_PI*(i/8.0))*j);
		ty=by+(cos(M_PI*(i/8.0))*j);
		pdglVertex3f(tx, ty, 30000);
	}
	pdglEnd();

	pdglEnableTexture2D();

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Draw_Bind(lbxgl_skyplane_skynum);
	pdglBegin(PDGL_QUADS);

	pdglColor4f(1*ll, 1*ll, 1*ll, 0.75);
	for(i=(vy-vr); i<(vy+vr); i++)for(j=(vx-vr); j<(vx+vr); j++)
	{
		bx=(j*65536);
		by=(i*65536);

		dx=iabs(j-vx);
		dy=iabs(i-vy);
		d1=(dx+dy)*ds;
		dy=iabs((i+1)-vy);
		d2=(dx+dy)*ds;
		dx=iabs((j+1)-vx);
		d3=(dx+dy)*ds;
		dy=iabs(i-vy);
		d4=(dx+dy)*ds;

		ts=t/10000000.0;
		s1=j*(10.0/1024.0)+ts;
		t1=i*(10.0/1024.0);
		s2=(j+1)*(10.0/1024.0)+ts;
		t2=(i+1)*(10.0/1024.0);

		pdglTexCoord2f(s1, t1);
		pdglVertex3f(bx, by, 25000-d1);
		pdglTexCoord2f(s1, t2);
		pdglVertex3f(bx, by+65536, 25000-d2);
		pdglTexCoord2f(s2, t2);
		pdglVertex3f(bx+65536, by+65536, 25000-d3);
		pdglTexCoord2f(s2, t1);
		pdglVertex3f(bx+65536, by, 25000-d4);

	}

	pdglColor4f(1*ll, 1*ll, 1*ll, 1);
	for(i=(vy-vr); i<(vy+vr); i++)for(j=(vx-vr); j<(vx+vr); j++)
	{
		bx=(j*65536);
		by=(i*65536);

		dx=iabs(j-vx);
		dy=iabs(i-vy);
		d1=(dx+dy)*ds;
		dy=iabs((i+1)-vy);
		d2=(dx+dy)*ds;
		dx=iabs((j+1)-vx);
		d3=(dx+dy)*ds;
		dy=iabs(i-vy);
		d4=(dx+dy)*ds;


		ts=t/500000.0;
		s1=j*(100.0/1024.0)+ts;
		t1=i*(100.0/1024.0);
		s2=(j+1)*(100.0/1024.0)+ts;
		t2=(i+1)*(100.0/1024.0);

		pdglTexCoord2f(s1, t1);
		pdglVertex3f(bx, by, 10000-d1);
		pdglTexCoord2f(s1, t2);
		pdglVertex3f(bx, by+65536, 10000-d2);
		pdglTexCoord2f(s2, t2);
		pdglVertex3f(bx+65536, by+65536, 10000-d3);
		pdglTexCoord2f(s2, t1);
		pdglVertex3f(bx+65536, by, 10000-d4);
	}
	pdglEnd();
	return(0);
}

int LBXGL_SkyPlane_Render(int t)
{
//	glDisable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	if(lbxgl_skyplane_camorg[2]>0)
	{
		LBXGL_SkyPlane_RenderGround(t);
		LBXGL_SkyPlane_RenderSky2(t);
	}else
	{
		LBXGL_SkyPlane_RenderSky2(t);
		LBXGL_SkyPlane_RenderGround(t);
	}
	glEnable(GL_DEPTH_TEST);
	return(0);
}

#if 0
		glDisable(GL_DEPTH_TEST);
		pdglDisableTexture2D();

		pdglBegin(PDGL_QUADS);
		pdglColor4f(0, 0, 1, 0.5);
		pdglVertex3f(-65536*1024, -65536*1024, 30000);
		pdglVertex3f(-65536*1024, 65536*1024, 30000);
		pdglVertex3f(65536*1024, 65536*1024, 30000);
		pdglVertex3f(65536*1024, -65536*1024, 30000);

		pdglColor4f(0, 0, 1, 0.5);
		pdglVertex3f(-65536*1024, -65536*1024, 20000);
		pdglVertex3f(-65536*1024, 65536*1024, 20000);
		pdglVertex3f(65536*1024, 65536*1024, 20000);
		pdglVertex3f(65536*1024, -65536*1024, 20000);

		pdglColor4f(0.7, 0.4, 0, 1);
		pdglVertex3f(-65536*1024, -65536*1024, -100);
		pdglVertex3f(-65536*1024, 65536*1024, -100);
		pdglVertex3f(65536*1024, 65536*1024, -100);
		pdglVertex3f(65536*1024, -65536*1024, -100);

		pdglEnd();

		pdglEnableTexture2D();

		Draw_Bind(skynum);
		pdglColor4f(1, 1, 1, 1);

		pdglBegin(PDGL_QUADS);

		pdglTexCoord2f(0+(t/500000.0), 0);
		pdglVertex3f(-65536*1024, -65536*1024, 10000);
		pdglTexCoord2f(0+(t/500000.0), 100);
		pdglVertex3f(-65536*1024, 65536*1024, 10000);
		pdglTexCoord2f(100+(t/500000.0), 100);
		pdglVertex3f(65536*1024, 65536*1024, 10000);
		pdglTexCoord2f(100+(t/500000.0), 0);
		pdglVertex3f(65536*1024, -65536*1024, 10000);

		pdglTexCoord2f(0-(t/10000000.0), 0);
		pdglVertex3f(-65536*1024, -65536*1024, 15000);
		pdglTexCoord2f(0-(t/10000000.0), 10);
		pdglVertex3f(-65536*1024, 65536*1024, 15000);
		pdglTexCoord2f(10-(t/10000000.0), 10);
		pdglVertex3f(65536*1024, 65536*1024, 15000);
		pdglTexCoord2f(10-(t/10000000.0), 0);
		pdglVertex3f(65536*1024, -65536*1024, 15000);

		pdglColor4f(0.7, 0.4, 0, 0.75);
		pdglTexCoord2f(0, 0);
		pdglVertex3f(-65536*1024, -65536*1024, -100);
		pdglTexCoord2f(0, 10);
		pdglVertex3f(-65536*1024, 65536*1024, -100);
		pdglTexCoord2f(10, 10);
		pdglVertex3f(65536*1024, 65536*1024, -100);
		pdglTexCoord2f(10, 0);
		pdglVertex3f(65536*1024, -65536*1024, -100);

		pdglEnd();

		glEnable(GL_DEPTH_TEST);
#endif

#if 0
		glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);

		glEnable(GL_LIGHTING);

//		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glRotatef (-ang[1],  0, 1, 0);
		glRotatef (-ang[0],  1, 0, 0);
		glRotatef (-ang[2],  0, 0, 1);
		glTranslatef (-org[0],  -org[1],  -org[2]);

		pt[0]=0;
		pt[1]=0;
		pt[2]=0;
		pt[3]=1;
		glLightfv(GL_LIGHT0, GL_POSITION, pt);

		pt[0]=0;
		pt[1]=0;
		pt[2]=1;
		pt[3]=0;
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, pt);

		glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180);
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0);
		glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 1/10.0);

		glEnable(GL_LIGHT0);

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

//		pt[0]=0.5;
//		pt[1]=0.5;
//		pt[2]=0.5;
//		pt[3]=0;
//		glMaterialfv(GL_FRONT, GL_SPECULAR, pt);
//		glMaterialf(GL_FRONT, GL_SHININESS, 0.5);
#endif
