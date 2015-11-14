#include <lbxgl2.h>

PDGLUI_InputState *lbxgl_state;
PDGLUI_Camera *lbxgl_cam;

float lbxgl_skyclouds_color[4];
float lbxgl_skyclouds_sunangle;
bool lbxgl_skyclouds_enabled=0;

LBXGL_API void LBXGL_SkyCoulds_SetSunAngle(float angle)
{
	lbxgl_skyclouds_sunangle=angle;
}

LBXGL_API void LBXGL_SkyCoulds_SetColor(float *tv)
{
	V4F_COPY(tv, lbxgl_skyclouds_color);
}

LBXGL_API void LBXGL_SkyCoulds_SetEnabled(bool val)
{
	lbxgl_skyclouds_enabled=val;
}

LBXGL_API void LBXGL_SkyCoulds_DrawSun()
{
	float pt1[3], pt2[3], pt3[3], pt4[3];
	float st1[2], st2[2], st3[2], st4[2];
	float org[3], sd[3], td[3];
	float ra;
	float a1, a2, a3, a4;
	float s1, s2, s3, s4;
	float t1, t2, t3, t4;
	float si1, si2, si3, si4;

	int tex;
	int i, j;

	if(!lbxgl_skyclouds_enabled)
		return;

//	tex=LBXGL_Texture_LoadImage("textures/effects/hazecirc");
//	tex=LBXGL_Texture_LoadImage("textures/effects/gimp_hsparks0");
	tex=LBXGL_Texture_LoadImage("textures/effects/sun1");
	ra=8192;

	a1=0-((M_PI*2)/32);
	a2=0+((M_PI*2)/32);
	a3=lbxgl_skyclouds_sunangle-((M_PI*2)/32);
	a4=lbxgl_skyclouds_sunangle+((M_PI*2)/32);

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

	Vec3F_Add(pt1, lbxgl_cam->org, pt1);
	Vec3F_Add(pt2, lbxgl_cam->org, pt2);
	Vec3F_Add(pt3, lbxgl_cam->org, pt3);
	Vec3F_Add(pt4, lbxgl_cam->org, pt4);

//	s1=si3*sin(a1);		t1=si3*cos(a1);
//	s2=si4*sin(a1);		t2=si4*cos(a1);
//	s3=si4*sin(a2);		t3=si4*cos(a2);
//	s4=si3*sin(a2);		t4=si3*cos(a2);

	s1=0;	t1=0;
	s2=1;	t2=0;
	s3=1;	t3=1;
	s4=0;	t4=1;
			
	glDepthMask(0);

	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	LBXGL_Shader_Color4fv(lbxgl_skyclouds_color);
	LBXGL_Shader_BindTexture(tex);

	LBXGL_Shader_Begin(PDGL_QUADS);

	LBXGL_Shader_TexCoord2f(s4, t4);
	LBXGL_Shader_Vertex3fv(pt4);

	LBXGL_Shader_TexCoord2f(s3, t3);
	LBXGL_Shader_Vertex3fv(pt3);

	LBXGL_Shader_TexCoord2f(s2, t2);
	LBXGL_Shader_Vertex3fv(pt2);

	LBXGL_Shader_TexCoord2f(s1, t1);
	LBXGL_Shader_Vertex3fv(pt1);

#if 1
	LBXGL_Shader_TexCoord2f(s4, t4);
	LBXGL_Shader_Vertex3fv(pt4);

	LBXGL_Shader_TexCoord2f(s3, t3);
	LBXGL_Shader_Vertex3fv(pt3);

	LBXGL_Shader_TexCoord2f(s2, t2);
	LBXGL_Shader_Vertex3fv(pt2);

	LBXGL_Shader_TexCoord2f(s1, t1);
	LBXGL_Shader_Vertex3fv(pt1);
#endif

#if 1
	LBXGL_Shader_TexCoord2f(s4, t4);
	LBXGL_Shader_Vertex3fv(pt4);

	LBXGL_Shader_TexCoord2f(s3, t3);
	LBXGL_Shader_Vertex3fv(pt3);

	LBXGL_Shader_TexCoord2f(s2, t2);
	LBXGL_Shader_Vertex3fv(pt2);

	LBXGL_Shader_TexCoord2f(s1, t1);
	LBXGL_Shader_Vertex3fv(pt1);
#endif

#if 1
	LBXGL_Shader_TexCoord2f(s4, t4);
	LBXGL_Shader_Vertex3fv(pt4);

	LBXGL_Shader_TexCoord2f(s3, t3);
	LBXGL_Shader_Vertex3fv(pt3);

	LBXGL_Shader_TexCoord2f(s2, t2);
	LBXGL_Shader_Vertex3fv(pt2);

	LBXGL_Shader_TexCoord2f(s1, t1);
	LBXGL_Shader_Vertex3fv(pt1);
#endif

	LBXGL_Shader_End();
	glDepthMask(1);

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

LBXGL_API void LBXGL_SkyCoulds_DrawClouds()
{
	float pt1[3], pt2[3], pt3[3], pt4[3];
	float st1[2], st2[2], st3[2], st4[2];
	float org[3], sd[3], td[3];
	float ra;
	float a1, a2, a3, a4;
	float s1, s2, s3, s4;
	float t1, t2, t3, t4;
	float si1, si2, si3, si4;

	int tex;
	int i, j;

	if(!lbxgl_skyclouds_enabled)
		return;

	tex=LBXGL_Texture_LoadImage("textures/effects/sky4");
	ra=8192;

#if 1
	glDepthMask(0);

//	LBXGL_Shader_Color4f(1, 1, 1, 1);
	LBXGL_Shader_Color4fv(lbxgl_skyclouds_color);
	LBXGL_Shader_BindTexture(tex);

	for(j=0; j<16; j++)
	{
		LBXGL_Shader_Begin(PDGL_QUADS);
		for(i=0; i<32; i++)
		{
			a1=((M_PI*2)/32)*i;
			a2=((M_PI*2)/32)*(i+1);
			a3=((M_PI*2)/32)*j;
			a4=((M_PI*2)/32)*(j+1);

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

			Vec3F_Add(pt1, lbxgl_cam->org, pt1);
			Vec3F_Add(pt2, lbxgl_cam->org, pt2);
			Vec3F_Add(pt3, lbxgl_cam->org, pt3);
			Vec3F_Add(pt4, lbxgl_cam->org, pt4);

			if(j<8)
			{
				si3=(1.0/8.0)*j;
				si4=(1.0/8.0)*(j+1);
			}else
			{
				si3=(1.0/8.0)*(16-j);
				si4=(1.0/8.0)*(15-j);
			}

			s1=si3*sin(a1);		t1=si3*cos(a1);
			s2=si4*sin(a1);		t2=si4*cos(a1);
			s3=si4*sin(a2);		t3=si4*cos(a2);
			s4=si3*sin(a2);		t4=si3*cos(a2);
			
			s1=s1*0.15+lbxgl_state->time_f*0.001;
			s2=s2*0.15+lbxgl_state->time_f*0.001;
			s3=s3*0.15+lbxgl_state->time_f*0.001;
			s4=s4*0.15+lbxgl_state->time_f*0.001;

			t1=t1*0.15+lbxgl_state->time_f*0.001;
			t2=t2*0.15+lbxgl_state->time_f*0.001;
			t3=t3*0.15+lbxgl_state->time_f*0.001;
			t4=t4*0.15+lbxgl_state->time_f*0.001;
			
			s1=s1+lbxgl_cam->org[0]*0.000001;
			s2=s2+lbxgl_cam->org[0]*0.000001;
			s3=s3+lbxgl_cam->org[0]*0.000001;
			s4=s4+lbxgl_cam->org[0]*0.000001;

			t1=t1+lbxgl_cam->org[1]*0.000001;
			t2=t2+lbxgl_cam->org[1]*0.000001;
			t3=t3+lbxgl_cam->org[1]*0.000001;
			t4=t4+lbxgl_cam->org[1]*0.000001;
			
//			t1=t1*0.25;
//			t2=t2*0.25;
//			t3=t3*0.25;
//			t4=t4*0.25;

			s1=s1*0.5+0.5;		t1=t1*0.5+0.5;
			s2=s2*0.5+0.5;		t2=t2*0.5+0.5;
			s3=s3*0.5+0.5;		t3=t3*0.5+0.5;
			s4=s4*0.5+0.5;		t4=t4*0.5+0.5;

			LBXGL_Shader_TexCoord2f(s4, t4);
			LBXGL_Shader_Vertex3fv(pt4);

			LBXGL_Shader_TexCoord2f(s3, t3);
			LBXGL_Shader_Vertex3fv(pt3);

			LBXGL_Shader_TexCoord2f(s2, t2);
			LBXGL_Shader_Vertex3fv(pt2);

			LBXGL_Shader_TexCoord2f(s1, t1);
			LBXGL_Shader_Vertex3fv(pt1);
		}
		LBXGL_Shader_End();
	}

	glDepthMask(1);
#endif

#if 0
//	V3F_SET(pt1, -32768, 16384,  32768);
//	V3F_SET(pt2, -32768, 16384, -32768);
//	V3F_SET(pt3,  32768, 16384, -32768);
//	V3F_SET(pt4,  32768, 16384,  32768);

	V3F_SET(pt1, -32768,  32768, 1024);
	V3F_SET(pt2, -32768, -32768, 1024);
	V3F_SET(pt3,  32768, -32768, 1024);
	V3F_SET(pt4,  32768,  32768, 1024);
	
	V2F_SET(st1, 0, 0);
	V2F_SET(st2, 1, 0);
	V2F_SET(st3, 1, 1);
	V2F_SET(st4, 0, 1);
	
	V2F_SCALEADDSCALE(st1, 0.1, lbxgl_cam->org, 0.00001, st1);
	V2F_SCALEADDSCALE(st2, 0.1, lbxgl_cam->org, 0.00001, st2);
	V2F_SCALEADDSCALE(st3, 0.1, lbxgl_cam->org, 0.00001, st3);
	V2F_SCALEADDSCALE(st4, 0.1, lbxgl_cam->org, 0.00001, st4);

	Vec3F_Add(pt1, lbxgl_cam->org, pt1);
	Vec3F_Add(pt2, lbxgl_cam->org, pt2);
	Vec3F_Add(pt3, lbxgl_cam->org, pt3);
	Vec3F_Add(pt4, lbxgl_cam->org, pt4);

	glDepthMask(0);

	LBXGL_Shader_Color4f(1, 1, 1, 1);
	LBXGL_Shader_BindTexture(tex);

	LBXGL_Shader_Begin(PDGL_QUADS);
//	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_TexCoord2fv(st1);
	LBXGL_Shader_Vertex3fv(pt1);
//	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_TexCoord2fv(st2);
	LBXGL_Shader_Vertex3fv(pt2);
//	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_TexCoord2fv(st3);
	LBXGL_Shader_Vertex3fv(pt3);
//	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_TexCoord2fv(st4);
	LBXGL_Shader_Vertex3fv(pt4);
	LBXGL_Shader_End();

	glDepthMask(1);
#endif

}
