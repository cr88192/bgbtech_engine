#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

int lbxgl_part2_chrome;
int lbxgl_part2_water;

LBXGL_API void LBXGL_Particle_Init2()
{
	LBXGL_ParticleInfo *inf;

	lbxgl_part2_chrome=LBXGL_Texture_LoadImage("textures/metal/chrome");
//	lbxgl_part2_water=LBXGL_Texture_LoadImage("textures/fluid/cs_misty");
	lbxgl_part2_water=LBXGL_Texture_LoadImage("textures/shader/watery_base");

	inf=LBXGL_Particle_GetTypeInfo("glob_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_glob_draw;
	inf->spawn=lbxgl_part_glob_spawn;

	inf=LBXGL_Particle_GetTypeInfo("waterglob_fall");
	inf->step=lbxgl_part_fall_step;
	inf->draw=lbxgl_part_glob_draw;
	inf->spawn=lbxgl_part_waterglob_spawn;

}

LBXGL_API int LBXGL_Particle_DrawSphere(float x, float y, float z,
	float ra, int texnum)
{
	int i, j;
	float r, g, b, a;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

//	printf("sphere (%f %f %f) %f\n", x, y, z, r);

	a=1;

	LBXGL_Shader_BindTexture(texnum);

	for(i=0; i<8; i++)
		for(j=0; j<4; j++)
	{
		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);
		a3=((M_PI*2)/8)*j;
		a4=((M_PI*2)/8)*(j+1);

		pt1[0]=sin(a3)*sin(a1)*ra;
		pt1[1]=sin(a3)*cos(a1)*ra;
		pt1[2]=cos(a3)*ra;
		s1=(1.0/8.0)*i;
		t1=(-1.0/4.0)*j;

		pt2[0]=sin(a4)*sin(a1)*ra;
		pt2[1]=sin(a4)*cos(a1)*ra;
		pt2[2]=cos(a4)*ra;
		s2=(1.0/8.0)*i;
		t2=(-1.0/4.0)*(j+1);

		pt3[0]=sin(a4)*sin(a2)*ra;
		pt3[1]=sin(a4)*cos(a2)*ra;
		pt3[2]=cos(a4)*ra;
		s3=(1.0/8.0)*(i+1);
		t3=(-1.0/4.0)*(j+1);

		pt4[0]=sin(a3)*sin(a2)*ra;
		pt4[1]=sin(a3)*cos(a2)*ra;
		pt4[2]=cos(a3)*ra;
		s4=(1.0/8.0)*(i+1);
		t4=(-1.0/4.0)*j;

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

		LBXGL_Shader_BeginPoly();

		LBXGL_Shader_TexCoord2f(s1, t1);
		LBXGL_Shader_Vertex3fv(pt1);

		LBXGL_Shader_TexCoord2f(s2, t2);
		LBXGL_Shader_Vertex3fv(pt2);

		LBXGL_Shader_TexCoord2f(s3, t3);
		LBXGL_Shader_Vertex3fv(pt3);

		LBXGL_Shader_TexCoord2f(s4, t4);
		LBXGL_Shader_Vertex3fv(pt4);

		LBXGL_Shader_EndPoly();
	}

	return(0);
}

void lbxgl_part_glob_draw(LBXGL_Particle *self)
{
	float v[3];
	float a1, x1, y1;
	float r, g, b;
	int i, j;

	glDepthMask(0);
//	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDisable(GL_TEXTURE_2D);

	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, self->tex);

	LBXGL_Shader_Color4f(
		(self->clr&0xFF)*(1.0/255.0),
		(self->clr&0xFF00)*(1.0/65280.0),
		(self->clr&0xFF0000)*(1.0/16711680.0),
		1.0-(self->time/self->lim));

//	LBXGL_Particle_DrawSphere(
//		self->org[0], self->org[1], self->org[2],
//		self->rad, self->tex);

#if 1
	LBXGL_Shader_BindTexture(self->tex);
	LBXGL_Shader_BeginPoly();
	for(i=0; i<16; i++)
	{
//		j=h?(15-i):i;
		j=i;
		a1=((M_PI*2)/16)*j;
		x1=sin(a1);
		y1=cos(a1);

		V3F_ADDSCALE(self->org, lbxgl_cam->rt, x1*self->rad, v);
		V3F_ADDSCALE(v, lbxgl_cam->up, y1*self->rad, v);

		LBXGL_Shader_TexCoord2f((x1*0.5)+0.5, (y1*0.5)+0.5);
		LBXGL_Shader_Vertex3fv(v);
	}
	LBXGL_Shader_EndPoly();
#endif

#if 0
	LBXGL_Shader_BindTexture(self->tex);

	LBXGL_Shader_BeginPoly();

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex3fv(v);

	LBXGL_Shader_EndPoly();
#endif

#if 0
	glBegin(GL_QUADS);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	glTexCoord2f(0, 1);
	glVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, -self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	glTexCoord2f(0, 0);
	glVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, -self->rad, v);
	glTexCoord2f(1, 0);
	glVertex3fv(v);

	V3F_ADDSCALE(self->org, lbxgl_cam->rt, self->rad, v);
	V3F_ADDSCALE(v, lbxgl_cam->up, self->rad, v);
	glTexCoord2f(1, 1);
	glVertex3fv(v);

	glEnd();
#endif

	glDepthMask(1);
//	glEnable(GL_DEPTH_TEST);
}

void lbxgl_part_glob_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	float f, g;

	lbxgl_part_flame_spawn(self, org, vel);

	V3F_COPY(vel, self->vel);
	V3F_COPY(org, self->org);
	self->org[0]+=0.25*bgbrng_randg();
	self->org[1]+=0.25*bgbrng_randg();
	self->org[2]+=0.25*bgbrng_randg();

	self->time=0;
	self->lim=3.0+(0.25*bgbrng_randg());
//	self->tex=lbxgl_part_flame_blue;
	self->tex=lbxgl_part2_chrome;
	self->rad=15.0+(9*bgbrng_randg());
}

void lbxgl_part_waterglob_spawn(LBXGL_Particle *self, float *org, float *vel)
{
	lbxgl_part_glob_spawn(self, org, vel);
	self->tex=lbxgl_part2_water;
}
