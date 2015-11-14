#include <lbxgl2.h>


// extern float cam_org[3];
// extern float cam_ang[3];
// extern float cam_rot[9];

// extern float cam_rt[3];
// extern float cam_fw[3];
// extern float cam_up[3];

// extern int pdgl_time, pdgl_dt;

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

extern int tex_screen;
extern int tex_screen_depth;

int tex_screen_ok=0;
int tex_screen_depth_ok=0;


LBXGL_TexImage *lbxgl_shader_cur;
LBXGL_Light *lbxgl_shader_light;

float *lbxgl_shader_xyzbuf;
float *lbxgl_shader_stbuf;
float *lbxgl_shader_nbuf;
float *lbxgl_shader_clrbuf;
float *lbxgl_shader_nxbuf;
float *lbxgl_shader_nybuf;

int lbxgl_shader_nvec;
int lbxgl_shader_mvec;

int lbxgl_shader_prim;
int lbxgl_shader_blend_src;
int lbxgl_shader_blend_dst;

float lbxgl_shader_sttmp[2];
float lbxgl_shader_ntmp[4];
float lbxgl_shader_clr[4];
float lbxgl_shader_nxtmp[4];
float lbxgl_shader_nytmp[4];

static int lbxgl_shader_init=0;

int lbxgl_shader_gloss;
int lbxgl_shader_gloss2=-1;

int lbxgl_shader_forcedraw=0;
int lbxgl_shader_nogloss=0;
int lbxgl_shader_isbudget=0;

float lbxgl_shader_matrixstack[16][16];
int lbxgl_shader_matrixstackpos=-1;
float lbxgl_shader_matrix[16];

LBXGL_API void LBXGL_Shader_LoadIdentityMatrix()
{
	LBXGL_Shader_Init();
	Mat4F_Identity(lbxgl_shader_matrix);
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glLoadIdentity();
}

LBXGL_API void LBXGL_Shader_PushMatrix()
{
	int i, j;

	LBXGL_Shader_Init();

	if(lbxgl_shader_matrixstackpos<0)
	{
		Mat4F_Identity(lbxgl_shader_matrix);
		lbxgl_shader_matrixstackpos=0;
	}

	if(lbxgl_shader_matrixstackpos>=16)
	{
		*(int *)-1=-1;
	}

	i=lbxgl_shader_matrixstackpos++;
	for(j=0; j<16; j++)
		lbxgl_shader_matrixstack[i][j]=lbxgl_shader_matrix[j];

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();
}

LBXGL_API void LBXGL_Shader_PopMatrix()
{
	int i, j;

//	LBXGL_Shader_Init();

	i=--lbxgl_shader_matrixstackpos;
	if(i<0)
	{
		Mat4F_Identity(lbxgl_shader_matrix);
		lbxgl_shader_matrixstackpos=0;
		return;
	}

	for(j=0; j<16; j++)
		lbxgl_shader_matrix[j]=lbxgl_shader_matrixstack[i][j];

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPopMatrix();
}

LBXGL_API void LBXGL_Shader_MultMatrix(float *mat)
{
	float tmat[16];

	Mat4F_MatMult(lbxgl_shader_matrix, mat, tmat);
	Mat4F_Copy(tmat, lbxgl_shader_matrix);

	glMatrixMode(GL_MODELVIEW_MATRIX);
	glMultMatrixf(mat);
}

LBXGL_API void LBXGL_Shader_PushMultMatrix(float *mat)
{
	LBXGL_Shader_PushMatrix();
	LBXGL_Shader_MultMatrix(mat);
}

LBXGL_API void LBXGL_Shader_TransformPoint(float *src, float *dst)
{
//	Mat4F_TransformPoint(src, lbxgl_shader_matrix, dst);
	Mat4F_TransformPointT(src, lbxgl_shader_matrix, dst);
//	*(int *)-1=-1;
}

LBXGL_API void LBXGL_Shader_TransformPointLocal(float *src, float *dst)
{
//	Mat4F_TransformPointLocal(src, lbxgl_shader_matrix, dst);
	Mat4F_TransformPointLocalT(src, lbxgl_shader_matrix, dst);
//	*(int *)-1=-1;
}

LBXGL_API void LBXGL_Shader_TransformVector(float *src, float *dst)
{
//	Mat4F_TransformVector(src, lbxgl_shader_matrix, dst);
	Mat4F_TransformVectorT(src, lbxgl_shader_matrix, dst);
}

LBXGL_API void LBXGL_Shader_TransformVectorLocal(float *src, float *dst)
{
//	Mat4F_TransformVectorT(src, lbxgl_shader_matrix, dst);
	Mat4F_TransformVector(src, lbxgl_shader_matrix, dst);
}

LBXGL_API void LBXGL_Shader_Translatef(float x, float y, float z)
{
	float tmat[16];

	Mat4F_Identity(tmat);
	tmat[12]=x; tmat[13]=y; tmat[14]=z;
	LBXGL_Shader_MultMatrix(tmat);
}

LBXGL_API void LBXGL_Shader_Init()
{
	int i;

	if(lbxgl_shader_init)return;
	lbxgl_shader_init=1;

	LBXGL_InitCam();

	lbxgl_shader_gloss=Tex_LoadFile("textures/effects/gloss", NULL, NULL);

	i=256;
	lbxgl_shader_xyzbuf=gcatomic(i*4*sizeof(float));
	lbxgl_shader_stbuf=gcatomic(i*2*sizeof(float));
	lbxgl_shader_nbuf=gcatomic(i*4*sizeof(float));
	lbxgl_shader_nxbuf=gcatomic(i*4*sizeof(float));
	lbxgl_shader_nybuf=gcatomic(i*4*sizeof(float));
	lbxgl_shader_clrbuf=gcatomic(i*4*sizeof(float));
	lbxgl_shader_mvec=i;

	LBXGL_Shader_PushMatrix();
	LBXGL_Shader_LoadIdentityMatrix();

	lbxgl_shader_blend_src=GL_SRC_ALPHA;
	lbxgl_shader_blend_dst=GL_ONE_MINUS_SRC_ALPHA;
//	glBlendFunc(lbxgl_shader_blend_src, lbxgl_shader_blend_dst);

//	LBXGL_ASM_Init();
}

LBXGL_API void LBXGL_Shader_FlushState()
{
	tex_screen_ok=0;
}

LBXGL_API void LBXGL_Shader_BindTexture(int num)
{
	float f, g, dt;
	int i, j;

	if(num<=0)
	{
		if(!lbxgl_shader_cur)
			return;
		lbxgl_shader_cur=NULL;
		if(lbxgl_shader_light)
			LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
		return;
	}

//	if(lbxgl_shader_cur && (lbxgl_shader_cur->num==num))
//		return;

	if(!lbxgl_shader_cur || (lbxgl_shader_cur->num!=num))
		lbxgl_shader_cur=LBXGL_Texture_LookupNum(num);

//	lbxgl_shader_cur=LBXGL_Texture_LookupNum(num);

	if(lbxgl_shader_cur)
	{
		if(lbxgl_shader_cur->anim_next>0)
		{
			j=lbxgl_state->time_f*5;
			j%=64;
			while(j--)
			{
				i=lbxgl_shader_cur->anim_next;
				lbxgl_shader_cur=LBXGL_Texture_LookupNum(i);
			}
		}else if(lbxgl_shader_cur->anim_video)
		{
			f=lbxgl_state->time_f;
			g=lbxgl_shader_cur->anim_time;
			dt=(g>0)?(f-g):0;
			if(dt>0.25)dt=0;
			lbxgl_shader_cur->anim_time=f;
			lbxgl_shader_cur->tex_alt=
				AVI_FrameTexnum(lbxgl_shader_cur->anim_video, dt);
		}
		
		if(lbxgl_shader_light)
		{
			if(!(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT))
				LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
		}
	}else
	{
		if(lbxgl_shader_light)
			LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
	}
}

LBXGL_API int LBXGL_Shader_GetCurrentTexture()
{
	if(lbxgl_shader_cur)
		return(lbxgl_shader_cur->num);
	return(0);
}

LBXGL_API int LBXGL_Shader_GetCurrentTextureFlags()
{
	if(lbxgl_shader_cur)
		return(lbxgl_shader_cur->flags);
	return(0);
}

LBXGL_API int LBXGL_Shader_GetTextureFlags(int num)
{
	LBXGL_TexImage *cur;
	if(num<=0)
		return(0);
	cur=LBXGL_Texture_LookupNum(num);
	if(cur)return(cur->flags);
	return(0);
}

LBXGL_API void LBXGL_Shader_SetCurrentMaterial()
{
	LBXGL_Texture_SetImageMaterialImg(lbxgl_shader_cur);
}

LBXGL_API void LBXGL_Shader_SetCurrentLight(LBXGL_Light *light)
{
	lbxgl_shader_light=light;
}

LBXGL_API void LBXGL_Shader_SetGlossEnv(int tex)
	{ lbxgl_shader_gloss2=tex; }
LBXGL_API void LBXGL_Shader_SetNoGloss(int fl)
	{ lbxgl_shader_nogloss=fl; }
LBXGL_API void LBXGL_Shader_SetBudget(int fl)
	{ lbxgl_shader_isbudget=fl; }

/** Set current blending functions */
LBXGL_API void LBXGL_Shader_BlendFunc(int src, int dst)
{
	lbxgl_shader_blend_src=src;
	lbxgl_shader_blend_dst=dst;
}

LBXGL_API void LBXGL_Shader_ForceDraw(int fl)
{
	lbxgl_shader_forcedraw=fl;
}

LBXGL_API void LBXGL_Shader_ClearTexture()
{
	lbxgl_shader_cur=NULL;
}

LBXGL_API void LBXGL_Shader_BeginPoly()
{
	LBXGL_Shader_Init();
	lbxgl_shader_nvec=0;
	lbxgl_shader_prim=GL_POLYGON;
}

LBXGL_API void LBXGL_Shader_EndPoly()
{
	LBXGL_Shader_EndPrim(GL_POLYGON);
}

LBXGL_API void LBXGL_Shader_Begin(int prim)
{
	LBXGL_Shader_Init();
	lbxgl_shader_nvec=0;
	lbxgl_shader_prim=prim;
}

LBXGL_API void LBXGL_Shader_End()
	{ LBXGL_Shader_EndPrim(lbxgl_shader_prim); }
LBXGL_API void LBXGL_Shader_EndFlat()
	{ LBXGL_Shader_EndPrimFlat(lbxgl_shader_prim); }
LBXGL_API void LBXGL_Shader_EndFlatVA()
	{ LBXGL_Shader_EndPrimFlatVA(lbxgl_shader_prim); }

LBXGL_API void LBXGL_Shader_BeginLight(int prim)
{
	LBXGL_Shader_Init();
	lbxgl_shader_nvec=0;
	lbxgl_shader_prim=prim;
}

LBXGL_API void LBXGL_Shader_EndLight()
{
	if(lbxgl_shader_cur &&
		(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT))
	{
		LBXGL_Shader_EndPrim_DoWarp(lbxgl_shader_prim);
		LBXGL_BrushLight_DrawPrimLightImage2(
			lbxgl_shader_cur, lbxgl_shader_light, lbxgl_shader_prim,
			lbxgl_shader_xyzbuf, lbxgl_shader_stbuf,
			lbxgl_shader_nbuf,
			lbxgl_shader_nxbuf, lbxgl_shader_nybuf,
			lbxgl_shader_nvec);
//		LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
		return;
	}
	LBXGL_Shader_EndPrimFlatNoRGB(lbxgl_shader_prim);

//	LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
}

LBXGL_API void LBXGL_Shader_BeginShadow(int prim)
{
	LBXGL_Shader_Init();
	lbxgl_shader_nvec=0;
	lbxgl_shader_prim=prim;
}

LBXGL_API void LBXGL_Shader_EndShadow()
{
//	LBXGL_Shader_EndPrimFlat(lbxgl_shader_prim);
	LBXGL_Brush_DrawPrimShadowArrays(
		lbxgl_shader_prim,
		lbxgl_shader_xyzbuf,
		lbxgl_shader_stbuf,
		lbxgl_shader_nbuf,
		lbxgl_shader_clrbuf,
		lbxgl_shader_nvec,
		lbxgl_shader_light);
}

LBXGL_API void LBXGL_Shader_EndPrim_DrawArrays(
	int prim,
	float *xyz, float *st, float *norm, float *rgba,
	int nxyz)
{
	int i;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	i=4*sizeof(float);
	glVertexPointer(3, GL_FLOAT, i, xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, st);
	glNormalPointer(GL_FLOAT, i, norm);
	glColorPointer(4, GL_FLOAT, 0, rgba);
	glDrawArrays(prim, 0, nxyz);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

LBXGL_API void LBXGL_Shader_EndPrim_JiggleWarpArrays(
	int prim,
	float *xyz, float *st, float *norm, float *rgba,
	int nxyz)
{
	float trmat[16];
	float tv0[3];
	LBXGL_ModelState *ctx;
	float *v;
	float f, g;
	int i, j;
	
	ctx=LBXGL_Mdl_GetCurrentModel();
	if(!ctx)return;
	
//	f=lbxgl_shader_cur->warp_speed;
	f=lbxgl_shader_cur->warp_scale;
	LBXGL_Mdl_CalcModelDelayRelTransform(ctx, trmat, f);

	for(i=0; i<nxyz; i++)
	{
		v=xyz+i*4;
		Mat4F_TransformPoint(v, trmat, tv0);
		V3F_COPY(tv0, v);
	}
}

LBXGL_API void LBXGL_Shader_EndPrim_DoWarp(int prim)
{
	char tb[64];
	float v0[4], v1[4];
	float trorg[4], trrt[4], trfw[4], trup[4];
	LBXGL_TexImage *tmp;
	float *s, *t;
	float u, v, w, f, g;
	int i, j, k, l, sc;

	if(!lbxgl_shader_cur)
		return;

//	if(lbxgl_shader_cur->flags&LBXGL_TXFL_SCROLLST)
	i=lbxgl_shader_cur->flags&LBXGL_TXFL_WARP_MASK;
	
//	if(LBXGL_Mdl_GetCurrentModel())
//		i=LBXGL_TXFL_WARP_JIGGLE;
//		i=LBXGL_TXFL_WARP_NBULGE;
	
	switch(i)
	{
	case LBXGL_TXFL_WARP_NONE:
		break;
	case LBXGL_TXFL_WARP_SCROLLST:
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4;
			t=lbxgl_shader_stbuf+i*2;

			f=lbxgl_state->time_f;
			g=f*lbxgl_shader_cur->warp_trans[2];
			v0[0]=(t[0]-0.5)*cos(f)+(t[1]-0.5)*sin(f);
			v0[1]=(t[1]-0.5)*cos(f)-(t[0]-0.5)*sin(f);
			t[0]=v0[0]+f*lbxgl_shader_cur->warp_trans[0]+0.5;
			t[0]=v0[1]+f*lbxgl_shader_cur->warp_trans[1]+0.5;
		}
		break;
	case LBXGL_TXFL_WARP_ST:
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4;
			t=lbxgl_shader_stbuf+i*2;

			f=V3F_LEN(s);
			g=lbxgl_state->time_f;
			t[0]+=sin(g+f)/lbxgl_shader_cur->width*4;
			t[1]+=cos(g+f)/lbxgl_shader_cur->height*4;
		}
		break;
	case LBXGL_TXFL_WARP_XYZ:
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4; t=s;

			f=V3F_LEN(s);
			g=lbxgl_state->time_f*lbxgl_shader_cur->warp_speed;
			t[0]+=sin(g+f)*lbxgl_shader_cur->warp_scale;
			t[1]+=cos(g+f)*lbxgl_shader_cur->warp_scale;
			t[2]+=sin(g+f-(M_PI/2))*lbxgl_shader_cur->warp_scale;
		}
		break;
	case LBXGL_TXFL_WARP_NBULGE:
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4;
			t=lbxgl_shader_nbuf+i*4;

//			V3F_FRNORMALIZE(s, v0);
//			t=v0;

			f=lbxgl_state->time_f*lbxgl_shader_cur->warp_speed;
			g=sin(f)*lbxgl_shader_cur->warp_scale;
			s[0]+=t[0]*g;
			s[1]+=t[1]*g;
			s[2]+=t[2]*g;
		}
		break;
	case LBXGL_TXFL_WARP_JIGGLE:
		LBXGL_Shader_EndPrim_JiggleWarpArrays(prim,
			lbxgl_shader_xyzbuf,
			lbxgl_shader_stbuf,
			lbxgl_shader_nbuf,
			lbxgl_shader_clrbuf,
			lbxgl_shader_nvec);
		break;
	default:
		break;
	}
}

LBXGL_API void LBXGL_Shader_EndPrim(int prim)
{
	static float *tmp_xyz=NULL;
	static float *tmp_st=NULL;
	static int tmp_xyz_sz=0;

	char tb[64];
	float v0[4], v1[4];
	float trorg[4], trrt[4], trfw[4], trup[4];
	LBXGL_TexImage *tmp;
	float *s, *t;
	float u, v, w, f, g;
	int i, j, k, l, sc;

	if(lbxgl_cam->mode==PDGLUI_CAM_QUAT)
	{
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4;
			UI_Camera_Project4D(s, s);
			if(s[3]<=0)
				lbxgl_shader_clrbuf[i*4+3]=0;

			t=lbxgl_shader_nbuf+i*4;
			UI_Camera_ProjectNormal4D(t, t);
		}
	}

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDisable(GL_ALPHA_TEST);
//	glDisable(GL_LIGHTING);

	if(!lbxgl_shader_cur)
	{
		glColor4fv(lbxgl_shader_clr);
		glDisable(GL_TEXTURE_2D);

		glBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			glColor4fv(lbxgl_shader_clrbuf+i*4);
			glNormal3fv(lbxgl_shader_nbuf+i*4);
			glVertex3fv(lbxgl_shader_xyzbuf+i*4);
		}
		glEnd();

		return;
	}

	if(lbxgl_shader_cur->flags&LBXGL_TXFL_NODRAW)
	{
		if(!lbxgl_shader_forcedraw)
			return;
//		glColor4f(
//			lbxgl_shader_clr[0],
//			lbxgl_shader_clr[1],
//			lbxgl_shader_clr[2],
//			lbxgl_shader_clr[3]*0.25);
	}

#if 0
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);

	if(lbxgl_shader_cur->flags&LBXGL_TXFL_WARPST)
	{
		glBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4;

			u=lbxgl_shader_stbuf[i*2+0];
			v=lbxgl_shader_stbuf[i*2+1];

			f=V3F_LEN(s);
			g=lbxgl_state->time_f;
			u+=sin(g+f)/lbxgl_shader_cur->width*4;
			v+=cos(g+f)/lbxgl_shader_cur->height*4;

			glTexCoord2f(u, v);
			glNormal3fv(lbxgl_shader_nbuf+i*4);
			glVertex3fv(s);
		}
		glEnd();
	}else
	{
		glBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			glTexCoord2fv(lbxgl_shader_stbuf+i*2);
			glNormal3fv(lbxgl_shader_nbuf+i*4);
			glVertex3fv(lbxgl_shader_xyzbuf+i*4);
		}
		glEnd();
	}
#endif

#if 1
	LBXGL_Shader_EndPrim_DoWarp(prim);

	if(lbxgl_shader_cur->ndraw)
	{
		for(i=0; i<lbxgl_shader_cur->ndraw; i++)
			lbxgl_shader_cur->draw[i](lbxgl_shader_cur, i,
				lbxgl_shader_nvec,
				lbxgl_shader_xyzbuf, lbxgl_shader_stbuf,
				lbxgl_shader_nbuf, lbxgl_shader_clrbuf);
	}else if(lbxgl_shader_cur->glsdr>0)
	{
		PDGL_BindShader(lbxgl_shader_cur->glsdr);
		PDGL_Uniform1f("time", lbxgl_state->time_f);

		glDisable(GL_TEXTURE_2D);

		l=0;

		j=PDGL_Uniform1i("texScreen", 0);
		if(j>0)
		{
			if(tex_screen<=0)
			{
				tex_screen=Tex_ScreenshotTexture();
				tex_screen_ok=0;
			}

			if(lbxgl_shader_cur->flags&LBXGL_TXFL_NEWSCREEN)
				tex_screen_ok=0;

			if(!tex_screen_ok)
//			if(1)
			{
//				printf(".\n");
				Tex_FillTextureScreenshot(tex_screen);
				tex_screen_ok=1;
			}

			i=l++;
			pdglActiveTexture(i);
			glBindTexture(GL_TEXTURE_2D, tex_screen);
			glEnable(GL_TEXTURE_2D);
			PDGL_Uniform1i("texScreen", i);
		}

		j=PDGL_Uniform1i("texScreenDepth", 0);
		if(j>0)
		{
			if(tex_screen<=0)
			{
				tex_screen_depth=Tex_ScreenshotDepthTexture();
				tex_screen_depth_ok=0;
			}

			if(lbxgl_shader_cur->flags&LBXGL_TXFL_NEWSCREEN)
				tex_screen_depth_ok=0;

			if(!tex_screen_depth_ok)
			{
				Tex_FillTextureScreenshotDepth(tex_screen_depth);
				tex_screen_depth_ok=1;
			}

			i=l++;
			pdglActiveTexture(i);
			glBindTexture(GL_TEXTURE_2D, tex_screen_depth);
			glEnable(GL_TEXTURE_2D);
			PDGL_Uniform1i("texScreenDepth", i);
		}

		if(lbxgl_shader_cur->num>=0)
		{
			i=l++;
			pdglActiveTexture(i);
			glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);
			glEnable(GL_TEXTURE_2D);
			PDGL_Uniform1i("texBase", i);
		}

		j=lbxgl_shader_cur->utn;
		for(i=0; i<j && (l<8); i++)
		{
			if(lbxgl_shader_cur->utex[i]<=0)continue;

			sprintf(tb, "texParm%d", i);
			i=l++;
			pdglActiveTexture(i);
			glBindTexture(GL_TEXTURE_2D,
				lbxgl_shader_cur->utex[i]);
			glEnable(GL_TEXTURE_2D);
			PDGL_Uniform1i(tb, i);
		}

		j=lbxgl_shader_cur->ufn;
		for(i=0; i<j; i++)
		{
			sprintf(tb, "floatParm%d", i);
			PDGL_Uniform1f(tb, lbxgl_shader_cur->ufv[i]);
		}

		j=lbxgl_shader_cur->ufvn;
		for(i=0; i<j; i++)
		{
			sprintf(tb, "vecParm%d", i);
			PDGL_Uniform4fv(tb, 1, lbxgl_shader_cur->uf4v+i*4);
		}


//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4fv(lbxgl_shader_clr);

//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);

		pdglActiveTexture(0);
		glBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			glColor4fv(lbxgl_shader_clrbuf+i*4);
			glTexCoord2fv(lbxgl_shader_stbuf+i*2);
			glNormal3fv(lbxgl_shader_nbuf+i*4);
			glVertex3fv(lbxgl_shader_xyzbuf+i*4);
		}
		glEnd();

		for(i=0; i<l; i++)
		{
			pdglActiveTexture(i);
			glDisable(GL_TEXTURE_2D);
		}
		pdglActiveTexture(0);

		PDGL_UnbindShader();

		if(lbxgl_shader_cur->flags&LBXGL_TXFL_KEEPSCREEN)
			tex_screen_ok=0;

	}else if(lbxgl_shader_cur->num>=0)
	{
		if(lbxgl_shader_cur->blend_src || lbxgl_shader_cur->blend_dst)
		{
			glBlendFunc(
				lbxgl_shader_cur->blend_src,
				lbxgl_shader_cur->blend_dst);
		}
	
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4fv(lbxgl_shader_clr);

		if(lbxgl_shader_cur->tex_alt>0)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,
				lbxgl_shader_cur->tex_alt);
		}else
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);
		}

		if(lbxgl_shader_cur->flags&LBXGL_TXFL_NODRAW)
		{
			glColor4f(
				lbxgl_shader_clr[0],
				lbxgl_shader_clr[1],
				lbxgl_shader_clr[2],
				lbxgl_shader_clr[3]*0.25);

			glBegin(prim);
			for(i=0; i<lbxgl_shader_nvec; i++)
			{
//				glColor4fv(lbxgl_shader_clrbuf+i*4);
				glTexCoord2fv(lbxgl_shader_stbuf+i*2);
				glNormal3fv(lbxgl_shader_nbuf+i*4);
				glVertex3fv(lbxgl_shader_xyzbuf+i*4);
			}
			glEnd();
		}else
		{
#if 0
			glBegin(prim);
			for(i=0; i<lbxgl_shader_nvec; i++)
			{
				glColor4fv(lbxgl_shader_clrbuf+i*4);
				glTexCoord2fv(lbxgl_shader_stbuf+i*2);
				glNormal3fv(lbxgl_shader_nbuf+i*4);
				glVertex3fv(lbxgl_shader_xyzbuf+i*4);
			}
			glEnd();
#endif

//			if(!(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT) &&
//				!lbxgl_shader_isbudget)
			if(1)
			{
				LBXGL_Shader_EndPrim_DrawArrays(prim,
					lbxgl_shader_xyzbuf,
					lbxgl_shader_stbuf,
					lbxgl_shader_nbuf,
					lbxgl_shader_clrbuf,
					lbxgl_shader_nvec);
			}
		}

		if(lbxgl_shader_cur->tex_glow>0)
		{
			glBlendFunc(GL_SRC_COLOR, GL_ONE);
			glColor4f(1, 1, 1, 1);

			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,
				lbxgl_shader_cur->tex_glow);

			glBegin(prim);
			for(i=0; i<lbxgl_shader_nvec; i++)
			{
				glTexCoord2fv(lbxgl_shader_stbuf+i*2);
				glNormal3fv(lbxgl_shader_nbuf+i*4);
				glVertex3fv(lbxgl_shader_xyzbuf+i*4);
			}
			glEnd();

//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendFunc(lbxgl_shader_blend_src, lbxgl_shader_blend_dst);
		}
	}else
	{
		glDisable(GL_TEXTURE_2D);

		glBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			glColor4fv(lbxgl_shader_clrbuf+i*4);
			glNormal3fv(lbxgl_shader_nbuf+i*4);
			glVertex3fv(lbxgl_shader_xyzbuf+i*4);
		}
		glEnd();
	}
#endif


	if((lbxgl_shader_cur->flags&LBXGL_TXFL_GLOSS) &&
		!lbxgl_shader_nogloss)
//	if(1)
	{
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if(lbxgl_shader_gloss2>0)
		{
			glDisable(GL_TEXTURE_2D);

			LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, trorg);

			glEnable(GL_TEXTURE_CUBE_MAP);
			glBindTexture(GL_TEXTURE_CUBE_MAP, lbxgl_shader_gloss2);

			glBegin(prim);
			for(i=0; i<lbxgl_shader_nvec; i++)
			{
				s=lbxgl_shader_clrbuf+i*4;
				V4F_COPY(s, v0);
				v0[3]*=lbxgl_shader_cur->gloss;
				glColor4fv(v0);

				s=lbxgl_shader_xyzbuf+i*4;
				t=lbxgl_shader_nbuf+i*4;

				V3F_SUB(s, trorg, v0);
				f=V3F_DOT(v0, t);
				V3F_ADDSCALE(v0, t, -2*f, v0);

				LBXGL_Shader_TransformVector(v0, v0);
//				LBXGL_Shader_TransformVector(t, v0);

				glTexCoord3fv(v0);
//				glTexCoord3fv(t);

				glNormal3fv(t);
				glVertex3fv(s);
			}
			glEnd();

			glDisable(GL_TEXTURE_CUBE_MAP);
			glEnable(GL_TEXTURE_2D);

			glBlendFunc(lbxgl_shader_blend_src, lbxgl_shader_blend_dst);
		}else
		{
			glBindTexture(GL_TEXTURE_2D, lbxgl_shader_gloss);

//			u=0.5;
			u=lbxgl_shader_cur->gloss;
//			V3F_COPY(lbxgl_shader_clr, v0);
//			v0[3]=lbxgl_shader_clr[3]*u;
//			glColor4fv(v0);

//			glColor4f(1, 1, 1, 0.5);
//			glColor4f(1, 1, 1, lbxgl_shader_cur->gloss);

			V3F_COPY(lbxgl_cam->org, trorg);
//			V3F_COPY(lbxgl_cam->rt, trrt);
//			V3F_COPY(lbxgl_cam->up, trup);
//			V3F_COPY(lbxgl_cam->fw, trfw);

//			Mat4F_TransformPointLocal(lbxgl_cam->org, lbxgl_shader_matrix, trorg);
//			Mat4F_TransformPointN(lbxgl_cam->org, lbxgl_shader_matrix, trorg);
//			Mat4F_TransformVectorT(lbxgl_cam->rt, lbxgl_shader_matrix, trrt);
//			Mat4F_TransformVectorT(lbxgl_cam->up, lbxgl_shader_matrix, trup);
//			Mat4F_TransformVectorT(lbxgl_cam->fw, lbxgl_shader_matrix, trfw);

//			Mat4F_TransformVector(lbxgl_cam->rt, lbxgl_shader_matrix, trrt);
//			Mat4F_TransformVector(lbxgl_cam->up, lbxgl_shader_matrix, trup);
//			Mat4F_TransformVector(lbxgl_cam->fw, lbxgl_shader_matrix, trfw);

			LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, trorg);
			LBXGL_Shader_TransformVectorLocal(lbxgl_cam->rt, trrt);
			LBXGL_Shader_TransformVectorLocal(lbxgl_cam->up, trup);
			LBXGL_Shader_TransformVectorLocal(lbxgl_cam->fw, trfw);

			glBegin(prim);
			for(i=0; i<lbxgl_shader_nvec; i++)
			{
				s=lbxgl_shader_clrbuf+i*4;
				V4F_COPY(s, v0);
				v0[3]*=lbxgl_shader_cur->gloss;
				glColor4fv(v0);

				s=lbxgl_shader_xyzbuf+i*4;
				t=lbxgl_shader_nbuf+i*4;

				u=V3F_DOT(s, trrt)-V3F_DOT(trorg, trrt);
				v=V3F_DOT(s, trup)-V3F_DOT(trorg, trup);
				w=V3F_DOT(s, trfw)-V3F_DOT(trorg, trfw);
//				w*=4;
//				if(w<1)w=1;
				if(w<16)w=16;
				u/=w; v/=w;

				u+=V3F_DOT(t, trrt);
				v+=V3F_DOT(t, trup);

				glTexCoord2f(u*0.25, v*0.25);
				glNormal3fv(t);
				glVertex3fv(s);
			}
			glEnd();

			glBlendFunc(lbxgl_shader_blend_src, lbxgl_shader_blend_dst);
		}
	}

	if(lbxgl_shader_cur->n_layer>0)
	{
		//ugly hack to draw layers...
		
		if(!tmp_xyz || (lbxgl_shader_nvec>tmp_xyz_sz))
		{
			tmp_xyz=realloc(tmp_xyz, lbxgl_shader_nvec*4*sizeof(float));
			tmp_st=realloc(tmp_st, lbxgl_shader_nvec*2*sizeof(float));
			tmp_xyz_sz=lbxgl_shader_nvec;
		}
		
		//save copy of XYZ and ST coords
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4; t=tmp_xyz+i*4;
			V3F_COPY(s, t);
			s=lbxgl_shader_stbuf+i*2; t=tmp_st+i*2;
			V3F_COPY(s, t);
		}
		
		tmp=lbxgl_shader_cur;
		for(i=0; i<tmp->n_layer; i++)
		{
			LBXGL_Shader_BindTexture(tmp->layer_link[i]);
			LBXGL_Shader_EndPrim(prim);

			//restore from copy
			for(j=0; j<lbxgl_shader_nvec; j++)
			{
				s=lbxgl_shader_xyzbuf+j*4; t=tmp_xyz+j*4;
				V3F_COPY(t, s);
				s=lbxgl_shader_stbuf+j*2; t=tmp_st+j*2;
				V3F_COPY(t, s);
			}
		}
		lbxgl_shader_cur=tmp;
	}
}

LBXGL_API void LBXGL_Shader_VertexDownProject()
{
	float *s, *t;
	int i, j, k;

	if(lbxgl_cam->mode==PDGLUI_CAM_QUAT)
	{
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_xyzbuf+i*4;
			UI_Camera_Project4D(s, s);
			if(s[3]<=0)
				lbxgl_shader_clrbuf[i*4+3]=0;

			t=lbxgl_shader_nbuf+i*4;
			UI_Camera_ProjectNormal4D(t, t);
		}
	}
}

LBXGL_API void LBXGL_Shader_EndPrimFlat(int prim)
{
	char tb[64];
	float v0[4];
	float trorg[4], trrt[4], trfw[4], trup[4];
	LBXGL_TexImage *tmp;
	float *s, *t;
	float u, v, w, f, g;
	int i, j, k, l, sc;

//	LBXGL_Shader_VertexDownProject();
	LBXGL_Shader_EndPrim_DoWarp(prim);

	glBegin(prim);
	for(i=0; i<lbxgl_shader_nvec; i++)
	{
		glTexCoord2fv(lbxgl_shader_stbuf+i*2);
		glColor4fv(lbxgl_shader_clrbuf+i*4);
		glNormal3fv(lbxgl_shader_nbuf+i*4);
		glVertex3fv(lbxgl_shader_xyzbuf+i*4);
	}
	glEnd();
}

LBXGL_API void LBXGL_Shader_EndPrimFlatNoRGB(int prim)
{
	char tb[64];
	float v0[4];
	float trorg[4], trrt[4], trfw[4], trup[4];
	LBXGL_TexImage *tmp;
	float *s, *t;
	float u, v, w, f, g;
	int i, j, k, l, sc;

//	LBXGL_Shader_VertexDownProject();
	LBXGL_Shader_EndPrim_DoWarp(prim);

	glBegin(prim);
	for(i=0; i<lbxgl_shader_nvec; i++)
	{
		glTexCoord2fv(lbxgl_shader_stbuf+i*2);
//		glColor4fv(lbxgl_shader_clrbuf+i*4);
		glNormal3fv(lbxgl_shader_nbuf+i*4);
		glVertex3fv(lbxgl_shader_xyzbuf+i*4);
	}
	glEnd();
}

LBXGL_API void LBXGL_Shader_EndPrimFlatVA(int prim)
{
	char tb[64];
	float v0[4];
	float trorg[4], trrt[4], trfw[4], trup[4];
	LBXGL_TexImage *tmp;
	float *s, *t;
	float u, v, w, f, g;
	int i, j, k, l, sc;

//	LBXGL_Shader_VertexDownProject();
	LBXGL_Shader_EndPrim_DoWarp(prim);

	LBXGL_Shader_EndPrim_DrawArrays(prim,
		lbxgl_shader_xyzbuf,
		lbxgl_shader_stbuf,
		lbxgl_shader_nbuf,
		lbxgl_shader_clrbuf,
		lbxgl_shader_nvec);

#if 0
	glBegin(prim);
	for(i=0; i<lbxgl_shader_nvec; i++)
	{
		glTexCoord2fv(lbxgl_shader_stbuf+i*2);
		glColor4fv(lbxgl_shader_clrbuf+i*4);
		glNormal3fv(lbxgl_shader_nbuf+i*4);
		glVertex3fv(lbxgl_shader_xyzbuf+i*4);
	}
	glEnd();
#endif
}

LBXGL_API void LBXGL_Shader_VertexCheckExpand()
{
	int i;

	if(lbxgl_shader_nvec>=lbxgl_shader_mvec)
	{
		i=lbxgl_shader_mvec+(lbxgl_shader_mvec>>1);
		lbxgl_shader_xyzbuf=
			gcrealloc(lbxgl_shader_xyzbuf, i*4*sizeof(float));
		lbxgl_shader_stbuf=
			gcrealloc(lbxgl_shader_stbuf, i*2*sizeof(float));
		lbxgl_shader_nbuf=
			gcrealloc(lbxgl_shader_nbuf, i*4*sizeof(float));
		lbxgl_shader_clrbuf=
			gcrealloc(lbxgl_shader_clrbuf, i*4*sizeof(float));
		lbxgl_shader_mvec=i;

		//light
		lbxgl_shader_nxbuf=
			gcrealloc(lbxgl_shader_nxbuf, i*4*sizeof(float));
		lbxgl_shader_nybuf=
			gcrealloc(lbxgl_shader_nybuf, i*4*sizeof(float));
	}
}

LBXGL_API void LBXGL_Shader_Vertex3fv(float *v)
{
	float *t;
	int i;

	LBXGL_Shader_VertexCheckExpand();

	t=lbxgl_shader_stbuf+lbxgl_shader_nvec*2;
	V2F_COPY(lbxgl_shader_sttmp, t);

	t=lbxgl_shader_nbuf+lbxgl_shader_nvec*4;
	V4F_COPY(lbxgl_shader_ntmp, t);

	t=lbxgl_shader_xyzbuf+lbxgl_shader_nvec*4;
	V3F_COPY(v, t); t[3]=0;

	t=lbxgl_shader_clrbuf+lbxgl_shader_nvec*4;
	V4F_COPY(lbxgl_shader_clr, t);


	if(lbxgl_shader_light)
	{
		t=lbxgl_shader_nxbuf+lbxgl_shader_nvec*4;
		V4F_COPY(lbxgl_shader_nxtmp, t);

		t=lbxgl_shader_nybuf+lbxgl_shader_nvec*4;
		V4F_COPY(lbxgl_shader_nytmp, t);
	}

	lbxgl_shader_nvec++;
}

LBXGL_API void LBXGL_Shader_Vertex4fv(float *v)
{
	float *t;
	int i;

	LBXGL_Shader_VertexCheckExpand();

	t=lbxgl_shader_stbuf+lbxgl_shader_nvec*2;
	V2F_COPY(lbxgl_shader_sttmp, t);

	t=lbxgl_shader_clrbuf+lbxgl_shader_nvec*4;
	V4F_COPY(lbxgl_shader_clr, t);

	t=lbxgl_shader_nbuf+lbxgl_shader_nvec*4;
//	V4F_COPY(lbxgl_shader_ntmp, t);
	UI_Camera_XFormNormal4D(lbxgl_shader_ntmp, t);

	if(lbxgl_cam->mode!=PDGLUI_CAM_QUAT)
		UI_Camera_Project4D_3D(t, t);

	t=lbxgl_shader_xyzbuf+lbxgl_shader_nvec*4;
//	V4F_COPY(v, t);
	UI_Camera_XForm4D(v, t);

	if(lbxgl_cam->mode!=PDGLUI_CAM_QUAT)
	{
		UI_Camera_Project4D_3D(t, t);

		if(t[3]<=0)
		{
			t=lbxgl_shader_clrbuf+lbxgl_shader_nvec*4;
			t[3]=0;
		}
	}

	lbxgl_shader_nvec++;
}

LBXGL_API void LBXGL_Shader_TexCoord2fv(float *v)
{
	float *t;
	V2F_COPY(v, lbxgl_shader_sttmp);
}

LBXGL_API void LBXGL_Shader_Normal3fv(float *v)
{
	float *t;

	V3F_COPY(v, lbxgl_shader_ntmp);
	lbxgl_shader_ntmp[3]=0;
}

LBXGL_API void LBXGL_Shader_Normal4fv(float *v)
{
	float *t;
	V4F_COPY(v, lbxgl_shader_ntmp);
}

LBXGL_API void LBXGL_Shader_NormalX3fv(float *v)
{
	V3F_COPY(v, lbxgl_shader_nxtmp);
	lbxgl_shader_nxtmp[3]=0;
}

LBXGL_API void LBXGL_Shader_NormalX4fv(float *v)
	{ V4F_COPY(v, lbxgl_shader_nxtmp); }

LBXGL_API void LBXGL_Shader_NormalY3fv(float *v)
{
	V3F_COPY(v, lbxgl_shader_nytmp);
	lbxgl_shader_nytmp[3]=0;
}

LBXGL_API void LBXGL_Shader_NormalY4fv(float *v)
	{ V4F_COPY(v, lbxgl_shader_nytmp); }


LBXGL_API void LBXGL_Shader_Color3fv(float *v)
{
	V3F_COPY(v, lbxgl_shader_clr);
	glColor3fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_Color4fv(float *v)
{
	V4F_COPY(v, lbxgl_shader_clr);
	glColor4fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_Color3f(float r, float g, float b)
{
	lbxgl_shader_clr[0]=r;
	lbxgl_shader_clr[1]=g;
	lbxgl_shader_clr[2]=b;

	glColor4fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_Color4f(float r, float g, float b, float a)
{
	lbxgl_shader_clr[0]=r;
	lbxgl_shader_clr[1]=g;
	lbxgl_shader_clr[2]=b;
	lbxgl_shader_clr[3]=a;

	glColor4fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_TexCoord2f(float s, float t)
{
	lbxgl_shader_sttmp[0]=s;
	lbxgl_shader_sttmp[1]=t;
}

LBXGL_API void LBXGL_Shader_Vertex3f(float x, float y, float z)
{
	float tv[3];
	tv[0]=x; tv[1]=y; tv[2]=z;
	LBXGL_Shader_Vertex3fv(tv);
}

LBXGL_API void LBXGL_Shader_Normal3f(float x, float y, float z)
{
	lbxgl_shader_ntmp[0]=x;
	lbxgl_shader_ntmp[1]=y;
	lbxgl_shader_ntmp[2]=z;
	lbxgl_shader_ntmp[3]=0;
}

LBXGL_API void LBXGL_Shader_Normal4f(float x, float y, float z, float w)
{
	lbxgl_shader_ntmp[0]=x;
	lbxgl_shader_ntmp[1]=y;
	lbxgl_shader_ntmp[2]=z;
	lbxgl_shader_ntmp[3]=w;
}

LBXGL_API void LBXGL_Shader_Vertex4f(float x, float y, float z, float w)
{
	float tv[4];
	tv[0]=x; tv[1]=y; tv[2]=z; tv[3]=w;
	LBXGL_Shader_Vertex4fv(tv);
}

LBXGL_API void LBXGL_Shader_PushMatrix4D()
{
	float *a, *b;

	a=lbxgl_cam->qxform+
		(lbxgl_cam->nqxform-1)*20;
	lbxgl_cam->nqxform++;
	b=lbxgl_cam->qxform+
		(lbxgl_cam->nqxform-1)*20;
	memcpy(b, a, 20*sizeof(float));
}

LBXGL_API void LBXGL_Shader_PopMatrix4D()
{
	lbxgl_cam->nqxform--;
}

LBXGL_API void LBXGL_Shader_LoadIdentity4D()
{
	float *mat;

	mat=lbxgl_cam->qxform+
		(lbxgl_cam->nqxform-1)*20;
	Mat4F_Identity(mat);
	V4F_ZERO(mat+16);
}

LBXGL_API void LBXGL_Shader_Translate4D(float x, float y, float z, float w)
{
	float tv[4];
	float *mat;

	mat=lbxgl_cam->qxform+
		(lbxgl_cam->nqxform-1)*20;
	tv[0]=x; tv[1]=y; tv[2]=z; tv[3]=w;
	UI_Camera_XFormMatNormal4D(tv, mat, tv);
	V4F_ADD(mat+16, tv, mat+16);
}

LBXGL_API void LBXGL_Shader_RotateAngles4D(float *ang)
{
	float *mat;
	mat=lbxgl_cam->qxform+
		(lbxgl_cam->nqxform-1)*20;
	Mat4F_Rotate4MatrixAngles(mat, ang, mat);
}

LBXGL_API void LBXGL_Shader_RotateAngles4D_f(
	float yz, float xz, float xy,
	float xw, float yw, float zw)
{
	float ang[8];

	ang[0]=yz; ang[1]=xz; ang[2]=xy;
	ang[3]=xw; ang[4]=yw; ang[5]=zw;

	LBXGL_Shader_RotateAngles4D(ang);
}

LBXGL_API void LBXGL_Shader_DrawSilhouette(
	float *xyz, int n_xyz,
	float *norm, int n_norm,
	int *edge, int n_edge, int flags)
{
	LBXGL_Light *light;
	float lorg[3];
	float lrad;

	light=lbxgl_shader_light;
	if(!light)return;
	
	LBXGL_Shader_TransformPointLocal(light->org, lorg);
	lrad=LBXGL_Shadow_LightGammaRadius(light);

	LBXGL_Brush_DrawEdgeShadowVolume(
		xyz, n_xyz, norm, n_norm, edge, n_edge,
		NULL, 0, NULL, 0,
		light, lorg, lrad, flags);
}

LBXGL_API void LBXGL_Shader_DrawSilhouetteFacePts(
	float *xyz, int n_xyz,			//XYZ or XYZW
	float *norm, int n_norm,		//XYZW
	int *edge, int n_edge,			//v0 v1 f0 f1
	int *pts, int n_pts,			//xyz st
	int *faces, int n_faces,		//point count
	int flags)
{
	LBXGL_Light *light;
	float lorg[3];
	float lrad;

	light=lbxgl_shader_light;
	if(!light)return;
	
	LBXGL_Shader_TransformPointLocal(light->org, lorg);
	lrad=LBXGL_Shadow_LightGammaRadius(light);

	LBXGL_Brush_DrawEdgeShadowVolume(
		xyz, n_xyz, norm, n_norm, edge, n_edge,
		pts, n_pts, faces, n_faces,
		light, lorg, lrad, flags);
}
