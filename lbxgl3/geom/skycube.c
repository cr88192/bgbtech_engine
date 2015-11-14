#include <lbxgl2.h>

#define SKY_DIST	32768

//extern float cam_org[3];
//extern float cam_ang[3];
//extern float cam_rot[9];
//extern int pdgl_time, pdgl_dt;

PDGLUI_InputState *lbxgl_state;
PDGLUI_Camera *lbxgl_cam;


int	lbxgl_skybox_tex[4*6];	//basic skybox texture
char	*lbxgl_skybox_suf[6] = {"rt", "ft", "up", "lf", "bk", "dn"};
char	*lbxgl_skybox_cssuf[6] = {"_r", "_f", "_u", "_l", "_b", "_d"};
char	*lbxgl_skybox_dpsuf[6] = {"_rt", "_ft", "_up", "_lf", "_bk", "_dn"};
VFILE	*render_screen;
int	lbxgl_skybox_on=0;
int	lbxgl_skybox_cs=0;
int	lbxgl_skybox_num=1;

int	lbxgl_skybox_tex_alt[4*6];
int	lbxgl_skybox_has_alt=0;
int	lbxgl_skybox_is_alt=0;

float lbxgl_skybox_org[3];

float lbxgl_skybox_alpha[4];


int	lbxgl_skydome_tex;
int	lbxgl_skydome_tex2;
int	lbxgl_skydome_on=0;
float	lbxgl_skydome_torque[3];
float	lbxgl_skydome_torque2[3];


int	lbxgl_sky_planetex[16];
float	lbxgl_sky_planeorg[16*3];
float	lbxgl_sky_planesdir[16*3];
float	lbxgl_sky_planetdir[16*3];
int	lbxgl_sky_nplanes;

float lbxgl_sky_sun_org[3];
float lbxgl_sky_sun_clr[4];
float lbxgl_sky_sun_light;

LBXGL_API int LBXGL_InitCam()
{
	if(!lbxgl_state)lbxgl_state=PDGLUI_GetState();
	if(!lbxgl_cam)lbxgl_cam=PDGLUI_GetCamera();
	return(0);
}

LBXGL_API void LBXGL_Sky_SetTorque(float *v)
	{ V3F_COPY(v, lbxgl_skydome_torque); }
LBXGL_API void LBXGL_Sky_SetTorque2(float *v)
	{ V3F_COPY(v, lbxgl_skydome_torque2); }


LBXGL_API void LBXGL_Sky_SetSunOrigin(float *v)
	{ V3F_COPY(v, lbxgl_sky_sun_org); }
LBXGL_API void LBXGL_Sky_SetSunColor(float *v)
	{ V3F_COPY(v, lbxgl_sky_sun_clr); }
LBXGL_API void LBXGL_Sky_SetSunLight(float v)
	{ lbxgl_sky_sun_light=v; }

LBXGL_API void LBXGL_Sky_GetSunOrigin(float *v)
	{ V3F_COPY(lbxgl_sky_sun_org, v); }
LBXGL_API void LBXGL_Sky_GetSunColor(float *v)
	{ V3F_COPY(lbxgl_sky_sun_clr, v); }
LBXGL_API float LBXGL_Sky_GetSunLight(void)
	{ return(lbxgl_sky_sun_light); }

LBXGL_API void LBXGL_Sky_SetAlphas(float *v, int n)
	{ V4F_COPY(v, lbxgl_skybox_alpha); }

// int LBXGL_SkyBox_SetCamOrigin(float *org)
// {
// }

LBXGL_API int LBXGL_SkyBox_LoadSky(char *name)
	{ return(LBXGL_SkyBox_LoadSkyNum(name, 0)); }

LBXGL_API int LBXGL_SkyBox_LoadSkyNum(char *name, int num)
{
	char tname[64];
	int i, j;

	LBXGL_InitCam();

	lbxgl_skybox_on=0;

	j=0;
	for(i=0; i<6; i++)
	{
//		sprintf(tname, "textures/env/%s%s.tga", name,
//			lbxgl_skybox_suf[i]);
		sprintf(tname, "textures/env/%s%s", name,
			lbxgl_skybox_suf[i]);
		lbxgl_skybox_tex[(num*6)+i]=LBXGL_Texture_LoadImage(tname);
		if(lbxgl_skybox_tex[(num*6)+i]>=0)j=1;
	}

	if(!j)return(-1);
	lbxgl_skybox_on=1;
	lbxgl_skybox_cs=0;

	if(num>=lbxgl_skybox_num)
		lbxgl_skybox_num=num+1;

	printf("LBXGL_SkyBox_LoadSky: %s\n", name);
	return(0);
}

LBXGL_API int LBXGL_SkyBox_LoadSkyCS(char *name)
	{ return(LBXGL_SkyBox_LoadSkyNumCS(name, 0)); }

LBXGL_API int LBXGL_SkyBox_LoadSkyNumCS(char *name, int num)
{
	char tname[64];
	int i, j;

	LBXGL_InitCam();

//	kprint("LBXGL_SkyBox_LoadSkyCS: %s\n", name);

	lbxgl_skybox_on=0;

	for(i=0; i<6; i++)
	{
//		sprintf(tname, "textures/env/%s%s.jpg", name,
//			lbxgl_skybox_cssuf[i]);
		sprintf(tname, "textures/env/%s%s", name,
			lbxgl_skybox_cssuf[i]);
		lbxgl_skybox_tex[(num*6)+i]=LBXGL_Texture_LoadImage(tname);
		if(lbxgl_skybox_tex[(num*6)+i]>=0)j=1;
	}

	if(!j)return(-1);
	lbxgl_skybox_on=1;
	lbxgl_skybox_cs=1;

	if(num>=lbxgl_skybox_num)
		lbxgl_skybox_num=num+1;

	printf("LBXGL_SkyBox_LoadSkyCS: %s\n", name);
	return(0);
}

LBXGL_API int LBXGL_SkyBox_LoadSkyDP(char *name)
	{ return(LBXGL_SkyBox_LoadSkyNumDP(name, 0)); }

LBXGL_API int LBXGL_SkyBox_LoadSkyNumDP(char *name, int num)
{
	char tname[64];
	int i, j;

	LBXGL_InitCam();

//	kprint("LBXGL_SkyBox_LoadSkyCS: %s\n", name);

	lbxgl_skybox_on=0;

	for(i=0; i<6; i++)
	{
//		sprintf(tname, "textures/env/%s%s.jpg", name,
//			lbxgl_skybox_cssuf[i]);
		sprintf(tname, "textures/env/%s/%s%s", name, name,
			lbxgl_skybox_dpsuf[i]);
		lbxgl_skybox_tex[(num*6)+i]=LBXGL_Texture_LoadImage(tname);
		if(lbxgl_skybox_tex[(num*6)+i]>=0)j=1;
	}

	if(!j)return(-1);
	lbxgl_skybox_on=1;
	lbxgl_skybox_cs=0;

	if(num>=lbxgl_skybox_num)
		lbxgl_skybox_num=num+1;

	printf("LBXGL_SkyBox_LoadSkyDP: %s\n", name);
	return(0);
}

LBXGL_API int LBXGL_SkyDome_LoadSky(char *name)
{
	char tname[64];
	int i;

	LBXGL_InitCam();

	lbxgl_skydome_on=0;

	sprintf(tname, "textures/env/%s", name);
	lbxgl_skydome_tex=LBXGL_Texture_LoadImage(tname);

	if(lbxgl_skydome_tex<0)return(-1);

	sprintf(tname, "textures/env/%s_a", name);
	lbxgl_skydome_tex2=LBXGL_Texture_LoadImage(tname);

	if(lbxgl_skydome_tex2<0)
		lbxgl_skydome_tex2=lbxgl_skydome_tex;

	lbxgl_skydome_on=1;

	printf("LBXGL_SkyDome_LoadSky: %s\n", name);
	return(0);
}

LBXGL_API int LBXGL_SkyDome_LoadSkyBottom(char *name)
{
	char tname[64];
	int i;

	LBXGL_InitCam();

	sprintf(tname, "textures/env/%s", name);
	lbxgl_skydome_tex2=LBXGL_Texture_LoadImage(tname);
	lbxgl_skydome_on=1;
	return(0);
}

void LBXGL_Sky_AddPlane(char *tex, float *org, float *sdir, float *tdir)
{
	int i;

	i=lbxgl_sky_nplanes++;
	lbxgl_sky_planetex[i]=LBXGL_Texture_LoadImage(tex);
	Vec3F_Copy(org, lbxgl_sky_planeorg+i*3);
	Vec3F_Copy(sdir, lbxgl_sky_planesdir+i*3);
	Vec3F_Copy(tdir, lbxgl_sky_planetdir+i*3);
}

LBXGL_API int LBXGL_Sky_LoadSkyInfo(char *name)
{
	char tname[256];
	float v0[4], v1[4], v2[4];
	VFILE *fd;
	char **a;

	LBXGL_InitCam();

	sprintf(tname, "textures/env/%s.txt", name);
	fd=vffopen(tname, "rt");
	if(!fd)return(-1);

	while(!vfeof(fd))
	{
		memset(tname, 0, 256);
		vfgets(tname, 255, fd);
		a=gcrsplit(tname);

		if(!a)continue;
		if(!a[0])continue;
		if(*a[0]==';')continue;

		if(!strcmp(a[0], "plane"))
		{
			v0[0]=atof(a[3]);
			v0[1]=atof(a[4]);
			v0[2]=atof(a[5]);

			v1[0]=atof(a[8]);
			v1[1]=atof(a[9]);
			v1[2]=atof(a[10]);

			v2[0]=atof(a[13]);
			v2[1]=atof(a[14]);
			v2[2]=atof(a[15]);

			LBXGL_Sky_AddPlane(a[1], v0, v1, v2);
		}

		if(!strcmp(a[0], "torque"))
		{
			v0[0]=atof(a[1]);
			v0[1]=atof(a[2]);
			v0[2]=atof(a[3]);
			v0[3]=atof(a[4])*(M_PI/180);

			Vec3F_Normalize(v0, v0);

			lbxgl_skydome_torque[0]=v0[0]*v0[3];
			lbxgl_skydome_torque[1]=v0[1]*v0[3];
			lbxgl_skydome_torque[2]=v0[2]*v0[3];
		}

		if(!strcmp(a[0], "torque2"))
		{
			v0[0]=atof(a[1]);
			v0[1]=atof(a[2]);
			v0[2]=atof(a[3]);
			v0[3]=atof(a[4])*(M_PI/180);

			Vec3F_Normalize(v0, v0);

			lbxgl_skydome_torque2[0]=v0[0]*v0[3];
			lbxgl_skydome_torque2[1]=v0[1]*v0[3];
			lbxgl_skydome_torque2[2]=v0[2]*v0[3];
		}
	}

	vfclose(fd);
	return(0);
}

LBXGL_API int LBXGL_Sky_LoadSky(char *name)
{
	int i;

	LBXGL_InitCam();

//	lbxgl_skybox_num=0;

	lbxgl_sky_nplanes=0;
	lbxgl_skybox_on=0;
	lbxgl_skydome_on=0;
	VecNF_Zero(lbxgl_skydome_torque, 3);
	VecNF_Zero(lbxgl_skydome_torque2, 3);

	lbxgl_skybox_has_alt=0;

	lbxgl_skybox_num=0;
	lbxgl_skybox_alpha[0]=1;
	lbxgl_skybox_alpha[1]=0;
	lbxgl_skybox_alpha[2]=0;
	lbxgl_skybox_alpha[3]=0;
	
	i=LBXGL_Sky_LoadSkyNum(name, 0);

	LBXGL_Sky_LoadSkyInfo(name);

	return(i);
}

LBXGL_API int LBXGL_Sky_LoadSkyNum(char *name, int num)
{
	int i;

	i=LBXGL_SkyDome_LoadSky(name);
	if(i<0)i=LBXGL_SkyBox_LoadSkyNum(name, num);
	if(i<0)i=LBXGL_SkyBox_LoadSkyNumCS(name, num);
	if(i<0)i=LBXGL_SkyBox_LoadSkyNumDP(name, num);

	return(0);
}


void LBXGL_SkyBox_DrawSkyFace(int fn)
{
	float nv[3], sv[3], tv[3];
	float pt0[3], pt1[3], pt2[3], pt3[3];
	float sn, sp;
	float tn, tp;
	float dist, alpha;
	int i, j, k, cs, tex;

	VecNF_Zero(sv, 3);
	VecNF_Zero(tv, 3);
	VecNF_Zero(nv, 3);

	cs=lbxgl_skybox_cs;

	switch(fn%6)
	{
	case 0:	//rt
		sv[1]=1;
		tv[2]=1;
		nv[0]=1;
		break;
	case 1: //ft
		sv[0]=-1;
		tv[2]=1;
		nv[1]=1;
		break;
	case 2: //up
		if(cs)
		{
			sv[0]=-1;
			tv[1]=-1;
			nv[2]=1;
		}else
		{
			sv[1]=1;
			tv[0]=-1;
			nv[2]=1;
		}
		break;
	case 3: //lf
		sv[1]=-1;
		tv[2]=1;
		nv[0]=-1;
		break;
	case 4: //bk
		sv[0]=1;
		tv[2]=1;
		nv[1]=-1;
		break;
	case 5: //dn
		if(cs)
		{
			sv[0]=-1;
			tv[1]=1;
			nv[2]=-1;
		}else
		{
			sv[1]=1;
			tv[0]=1;
			nv[2]=-1;
		}
		break;
	default:
		break;
	}

	dist=SKY_DIST;
	if(lbxgl_skybox_is_alt)
	{
		dist=6144;
		dist-=(fn/6)*64;
	}else
	{
		dist-=(fn/6)*2048;
	}

	VecNF_Scale(nv, dist, pt0, 3);
	VecNF_AddScale(pt0, sv, -dist, pt0, 3);
	VecNF_AddScale(pt0, tv, dist, pt0, 3);
	VecNF_Scale(nv, dist, pt1, 3);
	VecNF_AddScale(pt1, sv, -dist, pt1, 3);
	VecNF_AddScale(pt1, tv, -dist, pt1, 3);
	VecNF_Scale(nv, dist, pt2, 3);
	VecNF_AddScale(pt2, sv, dist, pt2, 3);
	VecNF_AddScale(pt2, tv, -dist, pt2, 3);
	VecNF_Scale(nv, dist, pt3, 3);
	VecNF_AddScale(pt3, sv, dist, pt3, 3);
	VecNF_AddScale(pt3, tv, dist, pt3, 3);

	if(lbxgl_skybox_is_alt)
	{
		Vec3F_Add(pt0, lbxgl_skybox_org, pt0);
		Vec3F_Add(pt1, lbxgl_skybox_org, pt1);
		Vec3F_Add(pt2, lbxgl_skybox_org, pt2);
		Vec3F_Add(pt3, lbxgl_skybox_org, pt3);
	}else
	{
		Vec3F_Add(pt0, lbxgl_cam->org, pt0);
		Vec3F_Add(pt1, lbxgl_cam->org, pt1);
		Vec3F_Add(pt2, lbxgl_cam->org, pt2);
		Vec3F_Add(pt3, lbxgl_cam->org, pt3);
	}

//	Draw_Bind(lbxgl_skybox_tex[fn]);
//	LBXGL_Shader_BindTexture(lbxgl_skybox_tex[fn]);

	if(lbxgl_skybox_num>1)
	{
		alpha=lbxgl_skybox_alpha[fn/6];
	}else
	{
		alpha=1.0;
	}

	tex=lbxgl_skybox_tex[fn];
	if(lbxgl_skybox_is_alt)
		tex=lbxgl_skybox_tex_alt[fn];
	LBXGL_Shader_BindTexture(tex);
//	Draw_Bind(tex);

	if(cs)
	{
		sn=255.0/256.0;
		sp=1.0/256.0;
	}else
	{
		sn=1.0/256.0;
		sp=255.0/256.0;
	}
	tn=1.0/256.0;
	tp=255.0/256.0;

#if 0
	pdglColor4f(1, 1, 1, 1);
	pdglEnableTexture2D();

	pdglBegin(PDGL_POLYGON);
	pdglTexCoord2f(sn, tp);
	pdglVertex3fv(pt0);
	pdglTexCoord2f(sn, tn);
	pdglVertex3fv(pt1);
	pdglTexCoord2f(sp, tn);
	pdglVertex3fv(pt2);
	pdglTexCoord2f(sp, tp);
	pdglVertex3fv(pt3);
	pdglEnd();
#endif

#if 1
	LBXGL_Shader_Color4f(1, 1, 1, alpha);
//	glEnable(GL_TEXTURE_2D);

	LBXGL_Shader_BeginPoly();
	LBXGL_Shader_TexCoord2f(sn, tp);
	LBXGL_Shader_Vertex3fv(pt0);
	LBXGL_Shader_TexCoord2f(sn, tn);
	LBXGL_Shader_Vertex3fv(pt1);
	LBXGL_Shader_TexCoord2f(sp, tn);
	LBXGL_Shader_Vertex3fv(pt2);
	LBXGL_Shader_TexCoord2f(sp, tp);
	LBXGL_Shader_Vertex3fv(pt3);
	LBXGL_Shader_EndPoly();
#endif
}

LBXGL_API void LBXGL_Sky_CalcRotateTorque()
{
	float tv[3];
	float f;

	f=Vec3F_Normalize(lbxgl_skydome_torque, tv);
	f*=lbxgl_state->time_f;
	pdglRotatef(f, tv[0], tv[1], tv[2]);

	f=Vec3F_Normalize(lbxgl_skydome_torque2, tv);
	f*=lbxgl_state->time_f;
	pdglRotatef(f, tv[0], tv[1], tv[2]);
}

LBXGL_API void LBXGL_SkyBox_DrawSkyBox()
{
	int i;

	LBXGL_InitCam();

	if(!lbxgl_skybox_on)return;

//	pdglModelviewMatrix();
	pdglModelviewMatrix();
	pdglPushMatrix();

	LBXGL_Sky_CalcRotateTorque();

//	if(num>=lbxgl_skybox_num)
//		lbxgl_skybox_num=num+1;

//	for(i=0; i<6; i++)
//		LBXGL_SkyBox_DrawSkyFace(i);

	for(i=0; i<(6*lbxgl_skybox_num); i++)
		LBXGL_SkyBox_DrawSkyFace(i);

	LBXGL_Shader_BindTexture(-1);

	pdglPopMatrix();
}

LBXGL_API void LBXGL_SkyDome_DrawSkyDome()
{
	int i, j;

	float a1, a2, a3, a4;
	float pt1[3], pt2[3], pt3[3], pt4[3];

	float s1, s2, s3, s4;
	float t1, t2, t3, t4;

	float si1, si2, si3, si4;

	float ra;

	if(!lbxgl_skydome_on)return;

	LBXGL_InitCam();

	ra=32768;

	pdglModelviewMatrix();
	pdglPushMatrix();

	pdglTranslatef(lbxgl_cam->org[0], lbxgl_cam->org[1], lbxgl_cam->org[2]);

//	a1=Vec3F_Normalize(lbxgl_skydome_torque, pt1);
//	a1*=lbxgl_state->time_f;
//	glRotatef(a1, pt1[0], pt1[1], pt1[2]);

	LBXGL_Sky_CalcRotateTorque();

	pdglColor4f(1, 1, 1, 1);

//	glDisable(GL_TEXTURE_2D);
	pdglEnableTexture2D();
	Draw_Bind(lbxgl_skydome_tex);

#if 0
//	glBegin(GL_QUADS);
	for(i=0; i<16; i++)
		for(j=0; j<8; j++)
	{
		if(lbxgl_skydome_tex!=lbxgl_skydome_tex2)
		{
			if(j==0)Draw_Bind(lbxgl_skydome_tex);
			if(j==4)Draw_Bind(lbxgl_skydome_tex2);
		}

		a1=((M_PI*2)/16)*i;
		a2=((M_PI*2)/16)*(i+1);
		a3=((M_PI*2)/16)*j;
		a4=((M_PI*2)/16)*(j+1);

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

//		Vec3F_Add(pt1, lbxgl_cam->org, pt1);
//		Vec3F_Add(pt2, lbxgl_cam->org, pt2);
//		Vec3F_Add(pt3, lbxgl_cam->org, pt3);
//		Vec3F_Add(pt4, lbxgl_cam->org, pt4);

//		si3=sin(a3);
//		si4=sin(a4);
//		si3=si3*si3;
//		si4=si4*si4;

		if(j<4)
		{
			si3=(1.0/4.0)*j;
			si4=(1.0/4.0)*(j+1);
		}else
		{
			si3=(1.0/4.0)*(8-j);
			si4=(1.0/4.0)*(7-j);
		}

		s1=si3*sin(a1);		t1=si3*cos(a1);
		s2=si4*sin(a1);		t2=si4*cos(a1);
		s3=si4*sin(a2);		t3=si4*cos(a2);
		s4=si3*sin(a2);		t4=si3*cos(a2);

		s1=s1*0.5+0.5;		t1=t1*0.5+0.5;
		s2=s2*0.5+0.5;		t2=t2*0.5+0.5;
		s3=s3*0.5+0.5;		t3=t3*0.5+0.5;
		s4=s4*0.5+0.5;		t4=t4*0.5+0.5;

		glBegin(GL_POLYGON);
		glTexCoord2f(s4, t4);		glVertex3fv(pt4);
		glTexCoord2f(s3, t3);		glVertex3fv(pt3);
		glTexCoord2f(s2, t2);		glVertex3fv(pt2);
		glTexCoord2f(s1, t1);		glVertex3fv(pt1);
		glEnd();
	}
//	glEnd();

#endif

#if 1
	for(j=0; j<16; j++)
	{
		if(j==0)Draw_Bind(lbxgl_skydome_tex);
		if(j==8)Draw_Bind(lbxgl_skydome_tex2);

		pdglBegin(PDGL_QUADS);
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

			s1=s1*0.5+0.5;		t1=t1*0.5+0.5;
			s2=s2*0.5+0.5;		t2=t2*0.5+0.5;
			s3=s3*0.5+0.5;		t3=t3*0.5+0.5;
			s4=s4*0.5+0.5;		t4=t4*0.5+0.5;

//			glBegin(GL_POLYGON);
			pdglTexCoord2f(s4, t4);		pdglVertex3fv(pt4);
			pdglTexCoord2f(s3, t3);		pdglVertex3fv(pt3);
			pdglTexCoord2f(s2, t2);		pdglVertex3fv(pt2);
			pdglTexCoord2f(s1, t1);		pdglVertex3fv(pt1);
//			glEnd();
		}
		pdglEnd();
	}
#endif

//	glDisable(GL_TEXTURE_2D);
	pdglPopMatrix();

	return;
}

void LBXGL_Sky_DrawPlane(int num)
{
	float pt1[3], pt2[3], pt3[3], pt4[3];
	float org[3], sd[3], td[3];

	Vec3F_Add(lbxgl_sky_planeorg+num*3, lbxgl_cam->org, org);
	Vec3F_Copy(lbxgl_sky_planesdir+num*3, sd);
	Vec3F_Copy(lbxgl_sky_planetdir+num*3, td);

	Vec3F_Subtract(org, sd, pt1); Vec3F_Subtract(pt1, td, pt1);
	Vec3F_Add(org, sd, pt2); Vec3F_Subtract(pt2, td, pt2);
	Vec3F_Add(org, sd, pt3); Vec3F_Add(pt3, td, pt3);
	Vec3F_Subtract(org, sd, pt4); Vec3F_Add(pt4, td, pt4);

//	pdglColor4f(1, 1, 1, 1);
//	glDisable(GL_TEXTURE_2D);
//	pdglEnableTexture2D();
//	Draw_Bind(lbxgl_sky_planetex[num]);

	LBXGL_Shader_Color4f(1, 1, 1, 1);
	LBXGL_Shader_BindTexture(lbxgl_sky_planetex[num]);

	LBXGL_Shader_Begin(PDGL_QUADS);
	LBXGL_Shader_TexCoord2f(0, 0);
	LBXGL_Shader_Vertex3fv(pt1);
	LBXGL_Shader_TexCoord2f(1, 0);
	LBXGL_Shader_Vertex3fv(pt2);
	LBXGL_Shader_TexCoord2f(1, 1);
	LBXGL_Shader_Vertex3fv(pt3);
	LBXGL_Shader_TexCoord2f(0, 1);
	LBXGL_Shader_Vertex3fv(pt4);
	LBXGL_Shader_End();

//	pdglBegin(PDGL_POLYGON);
//	pdglTexCoord2f(0, 0);	pdglVertex3fv(pt1);
//	pdglTexCoord2f(1, 0);	pdglVertex3fv(pt2);
//	pdglTexCoord2f(1, 1);	pdglVertex3fv(pt3);
//	pdglTexCoord2f(0, 1);	pdglVertex3fv(pt4);
//	pdglEnd();
}


LBXGL_API void LBXGL_Sky_DrawSkyBasic()
{
	int i;

	LBXGL_InitCam();

//	LBXGL_SkyCoulds_DrawClouds();

	LBXGL_SkyDome_DrawSkyDome();
	LBXGL_SkyBox_DrawSkyBox();

	LBXGL_SkyCoulds_DrawSun();
	LBXGL_SkyCoulds_DrawClouds();

	for(i=0; i<lbxgl_sky_nplanes; i++)
		LBXGL_Sky_DrawPlane(i);

	LBXGL_Shader_BindTexture(-1);
}

LBXGL_API void LBXGL_Sky_DrawSky()
{
	int cs, sbon, sdon;

	cs=lbxgl_skybox_cs;
	sbon=lbxgl_skybox_on;
	sdon=lbxgl_skydome_on;
	if(lbxgl_skybox_has_alt)
	{
		lbxgl_skybox_is_alt=1;
		lbxgl_skybox_cs=0;
		lbxgl_skybox_on=1;
		lbxgl_skydome_on=0;
	}

	LBXGL_Sky_DrawSkyBasic();
	LBXGL_Shader_BindTexture(-1);

	lbxgl_skybox_is_alt=0;
	lbxgl_skybox_cs=cs;
	lbxgl_skybox_on=sbon;
	lbxgl_skydome_on=sdon;
}

LBXGL_API int LBXGL_Sky_GetHasAlt()
	{ return(lbxgl_skybox_has_alt); }
LBXGL_API void LBXGL_Sky_SetHasAlt(int fl)
	{ lbxgl_skybox_has_alt=fl; }
LBXGL_API int LBXGL_Sky_GetSkyAlt(int n)
	{ return(lbxgl_skybox_tex_alt[n]); }
LBXGL_API void LBXGL_Sky_SetSkyAlt(int n, int tex)
	{ lbxgl_skybox_tex_alt[n]=tex; }
LBXGL_API int *LBXGL_Sky_RefSkyAlt()
	{ return(lbxgl_skybox_tex_alt); }

LBXGL_API void LBXGL_Sky_GetAltOrigin(float *org)
	{ V3F_COPY(lbxgl_skybox_org, org); }
LBXGL_API void LBXGL_Sky_SetAltOrigin(float *org)
	{ V3F_COPY(org, lbxgl_skybox_org); }
