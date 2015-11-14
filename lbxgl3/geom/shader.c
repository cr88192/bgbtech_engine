#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

extern int tex_screen;
extern int tex_screen_depth;

extern int tex_light_defnorm;
extern int tex_light_defgloss;

int tex_screen_ok=0;
int tex_screen_depth_ok=0;


LBXGL_TexImage *lbxgl_shader_cur;
LBXGL_Light *lbxgl_shader_light;
int lbxgl_shader_sndmixchan;		//sound-mix channel

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

int lbxgl_shader_lastblend_src;
int lbxgl_shader_lastblend_dst;

float lbxgl_shader_sttmp[2];
float lbxgl_shader_ntmp[4];
float lbxgl_shader_clr[4];
float lbxgl_shader_nxtmp[4];
float lbxgl_shader_nytmp[4];

static int lbxgl_shader_init=0;

int lbxgl_shader_gloss;
int lbxgl_shader_gloss2=-1;

int lbxgl_shader_tex_white;
int lbxgl_shader_tex_black;

int lbxgl_shader_texdefault;
int lbxgl_shader_texdefault_texBase;
int lbxgl_shader_texdefault_texIsUVAY;

int lbxgl_shader_texfinal;
int lbxgl_shader_texfinal_texBase;
int lbxgl_shader_texfinal_texNorm;
int lbxgl_shader_texfinal_texGloss;
int lbxgl_shader_texfinal_texGlow;
int lbxgl_shader_texfinal_texIsUVAY;

int lbxgl_shader_texflat;
int lbxgl_shader_texflat_texBase;
int lbxgl_shader_texflat_texIsUVAY;

byte lbxgl_shader_forcedraw=0;
byte lbxgl_shader_nogloss=0;
byte lbxgl_shader_isbudget=0;
byte lbxgl_shader_useparallax=0;
byte lbxgl_shader_isfinal=0;
byte lbxgl_shader_isflat=0;

int lbxgl_shader_tex_usescreen=0;
int lbxgl_shader_tex_usedepth=0;

float lbxgl_shader_matrixstack[16][16];
int lbxgl_shader_matrixstackpos=-1;
float lbxgl_shader_matrix[16];

LBXGL_API void LBXGL_Shader_LoadIdentityMatrix()
{
	LBXGL_Shader_Init();
	Mat4F_Identity(lbxgl_shader_matrix);
//	glMatrixMode(GL_MODELVIEW_MATRIX);
	pdglModelviewMatrix();
	pdglLoadIdentity();
}

LBXGL_API void LBXGL_Shader_PushMatrix(void)
{
	int i, j;

	LBXGL_Shader_Init();

	if(lbxgl_shader_matrixstackpos<0)
	{
		Mat4F_Identity(lbxgl_shader_matrix);
		lbxgl_shader_matrixstackpos=0;
	}

//	if(lbxgl_shader_matrixstackpos>=16)
	if(lbxgl_shader_matrixstackpos>=2)
	{
		*(int *)-1=-1;
	}

	i=lbxgl_shader_matrixstackpos++;
	for(j=0; j<16; j++)
		lbxgl_shader_matrixstack[i][j]=lbxgl_shader_matrix[j];

	//pdglMatrixMode(GL_MODELVIEW_MATRIX);
	pdglModelviewMatrix();
	pdglPushMatrix();
}

LBXGL_API void LBXGL_Shader_PopMatrix(void)
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

//	glMatrixMode(GL_MODELVIEW_MATRIX);
	pdglModelviewMatrix();
	pdglPopMatrix();
}

LBXGL_API void LBXGL_Shader_MultMatrix(float *mat)
{
	float tmat[16];

	Mat4F_MatMult(lbxgl_shader_matrix, mat, tmat);
	Mat4F_Copy(tmat, lbxgl_shader_matrix);

//	glMatrixMode(GL_MODELVIEW_MATRIX);
	pdglModelviewMatrix();
	pdglMultMatrixf(mat);
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
//	pdglTranslatef(x, y, z);
}

LBXGL_API void LBXGL_Shader_Init(void)
{
	int i;

	if(lbxgl_shader_init)return;
	lbxgl_shader_init=1;

	LBXGL_InitCam();

	lbxgl_shader_gloss=Tex_LoadFile(
		"textures/effects/gloss", NULL, NULL);
	lbxgl_shader_tex_white=Tex_LoadFile(
		"textures/effects/white", NULL, NULL);
	lbxgl_shader_tex_black=Tex_LoadFile(
		"textures/effects/black", NULL, NULL);

	lbxgl_shader_texdefault=PDGL_LoadShader("bgb_texdefault");
	lbxgl_shader_texdefault_texBase=
		pdglGetUniformLocation(lbxgl_shader_texdefault, "texBase");
	lbxgl_shader_texdefault_texIsUVAY=
		pdglGetUniformLocation(lbxgl_shader_texdefault, "texIsUVAY");

	lbxgl_shader_texfinal=PDGL_LoadShader("bgb_texfinal");
	lbxgl_shader_texfinal_texBase=
		pdglGetUniformLocation(lbxgl_shader_texfinal, "texBase");
	lbxgl_shader_texfinal_texNorm=
		pdglGetUniformLocation(lbxgl_shader_texfinal, "texNorm");
	lbxgl_shader_texfinal_texGloss=
		pdglGetUniformLocation(lbxgl_shader_texfinal, "texGloss");
	lbxgl_shader_texfinal_texGlow=
		pdglGetUniformLocation(lbxgl_shader_texfinal, "texGlow");
	lbxgl_shader_texfinal_texIsUVAY=
		pdglGetUniformLocation(lbxgl_shader_texfinal, "texIsUVAY");

	lbxgl_shader_texflat=PDGL_LoadShader("bgb_texflat");
	lbxgl_shader_texflat_texBase=
		pdglGetUniformLocation(lbxgl_shader_texflat, "texBase");
	lbxgl_shader_texflat_texIsUVAY=
		pdglGetUniformLocation(lbxgl_shader_texflat, "texIsUVAY");

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

LBXGL_API void LBXGL_Shader_FlushState(void)
{
	tex_screen_ok=0;
	tex_screen_depth_ok=0;
	
	lbxgl_shader_tex_usescreen=0;
	lbxgl_shader_tex_usedepth=0;
}

LBXGL_TexImage *LBXGL_Shader_BindTexture_AnimMatchSound(LBXGL_TexImage *img)
{
	int dct[64];
	LBXGL_TexImage *cur, *best;
	int i, j, bi;

	if(lbxgl_shader_sndmixchan<=0)
		return(img);

	PDGL_Sound_GetPlayingCurDCT(lbxgl_shader_sndmixchan, dct);
	
	cur=img; best=img; bi=999999999;
	while(cur)
	{
		if(cur->snd_dct)
		{
			for(i=0; i<cur->snd_n_dct; i++)
			{
				j=PDGL_Sound_MatchCompareDCT(
					dct, cur->snd_dct+i*64, 64);
				if((j>=0) && (j<bi))
				{
					best=cur;
					bi=j;
				}
			}
		}
	
		i=cur->anim_next;
		cur=LBXGL_Texture_LookupNum(i);
		if(!cur || (cur==img))break;
	}

	printf("AnimMatchSound: %s %d\n", best->name, bi);

	return(best);
}

LBXGL_API void LBXGL_Shader_FlushTexture()
{
	int i;

	if(lbxgl_shader_light)
	{
#if 0
		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
		glDisableClientState(GL_COLOR_ARRAY);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		for(i=0; i<16; i++)
		{
			pdglActiveTexture(i);
			glDisable(GL_TEXTURE_2D);
		}
		pdglActiveTexture(0);
#endif

		return;
	}

	if(!lbxgl_shader_cur)
		return;
	if(lbxgl_shader_cur->glsdr>0)
	{
		LBXGL_Shader_EndPrim_TeardownForShader(
			lbxgl_shader_cur->glsdr);
		return;
	}

	if(lbxgl_shader_cur->flags2&BTGE_TXFL2_NODEPTHWRITE)
	{
		glDepthMask(GL_TRUE);
	}

	if(lbxgl_shader_cur->blend_src || lbxgl_shader_cur->blend_dst)
	{
		glBlendFunc(
			lbxgl_shader_lastblend_src,
			lbxgl_shader_lastblend_dst);
	}

//	PDGL_BindShader(lbxgl_shader_texdefault);

#if 1
//	pdglColor4f(1, 1, 1, 1);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	for(i=0; i<16; i++)
	{
		pdglActiveTexture(i);
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_CUBE_MAP);
	}
	pdglActiveTexture(0);
//	PDGL_UnbindShader();
	PDGL_BindShader(lbxgl_shader_texdefault);

	glBindTexture(GL_TEXTURE_2D, lbxgl_shader_tex_white);
	glEnable(GL_TEXTURE_2D);
#endif

#if 0
	PDGL_UnbindShader();
#endif
}

LBXGL_API void LBXGL_Shader_SetupTextureFinal()
{
	float tv[4];
	int txn, tyn;
	int i, j, k, l;

//	*(int *)-1=-1;

	while(glGetError());	//clear errors

	PDGL_BindShader(lbxgl_shader_texfinal);

	glDisable(GL_TEXTURE_2D);

	if(lbxgl_shader_texfinal_texIsUVAY>=0)
	{
		pdglUniform1i(lbxgl_shader_texfinal_texIsUVAY,
			(lbxgl_shader_cur->flags&LBXGL_TXFL_COLOR_UVAY)?1:0);
	}

	l=0;

//	pdglEnable(GL_TEXTURE_2D);
//	pdglEnableTexture2D();
//	if(lbxgl_shader_cur->tex_alt>0)
//	{
//		glBindTexture(GL_TEXTURE_2D,
//			lbxgl_shader_cur->tex_alt);
//	}else
//	{
//		glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);
//	}

	if(lbxgl_shader_texfinal_texBase>=0)
	{
		if(lbxgl_shader_cur->tex_alt>0)
			{ j=lbxgl_shader_cur->tex_alt; }
		else
			{ j=lbxgl_shader_cur->num; }

		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, j);
		glEnable(GL_TEXTURE_2D);
		pdglUniform1i(lbxgl_shader_texfinal_texBase, i);
	}

//	pdglUniform1i(lbxgl_shader_texfinal_texBase, i);

	if(lbxgl_shader_texfinal_texNorm>=0)
	{
		if(lbxgl_shader_cur->tex_norm>0)
			{ j=lbxgl_shader_cur->tex_norm; }
		else
			{ j=tex_light_defnorm; }
	
		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, j);
		glEnable(GL_TEXTURE_2D);
		pdglUniform1i(lbxgl_shader_texfinal_texNorm, i);
	}

	if(lbxgl_shader_texfinal_texGloss>=0)
	{
		if(lbxgl_shader_cur->tex_gloss>0)
			{ j=lbxgl_shader_cur->tex_gloss; }
		else
			{ j=tex_light_defgloss; }

		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, j);
		glEnable(GL_TEXTURE_2D);
		pdglUniform1i(lbxgl_shader_texfinal_texGloss, i);
	}

	if(lbxgl_shader_texfinal_texGlow>=0)
	{
		if(lbxgl_shader_cur->tex_glow>0)
			{ j=lbxgl_shader_cur->tex_glow; }
		else
			{ j=lbxgl_shader_tex_black; }

		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, j);
		glEnable(GL_TEXTURE_2D);
		pdglUniform1i(lbxgl_shader_texfinal_texGlow, i);
	}

	pdglActiveTexture(0);

	V3F_ZERO(tv);
	txn=PDGL_GetAttribLocation("vecSdir");
	tyn=PDGL_GetAttribLocation("vecTdir");
	if(txn>0)pdglVertexAttrib3fv(txn, tv);
	if(tyn>0)pdglVertexAttrib3fv(tyn, tv);

	i=glGetError();
	while(i)
	{
		printf("LBXGL_Shader_SetupTextureFinal: Error %08X\n", i);
		i=glGetError();
	}
}

LBXGL_API void LBXGL_Shader_SetupTexture()
{
	float tv[4];
	int txn, tyn;
	int i, j, k, l;

	if(lbxgl_shader_light)
	{
		LBXGL_BrushLight_DrawSetupLightImage(
			lbxgl_shader_light, lbxgl_shader_cur);
		return;
	}

	if(!lbxgl_shader_cur)
		return;
	if(lbxgl_shader_cur->glsdr>0)
	{
		LBXGL_Shader_EndPrim_SetupForShader(
			lbxgl_shader_cur->glsdr);
		return;
	}

	lbxgl_shader_lastblend_src=lbxgl_shader_blend_src;
	lbxgl_shader_lastblend_dst=lbxgl_shader_blend_dst;
	if(lbxgl_shader_cur->blend_src || lbxgl_shader_cur->blend_dst)
	{
		glBlendFunc(
			lbxgl_shader_cur->blend_src,
			lbxgl_shader_cur->blend_dst);
	}else
	{
//		glBlendFunc(
//			lbxgl_shader_blend_src,
//			lbxgl_shader_blend_dst);
	}

//	if(lbxgl_shader_cur->tex_alt>0)
	if(1)
	{		
//		if(lbxgl_shader_isfinal &&
//		if(1 &&
//			(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT))
//		if(1)
//		if(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT)
		if(lbxgl_shader_isfinal &&
			(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT))
		{
			LBXGL_Shader_SetupTextureFinal();
		}else if(lbxgl_shader_isflat)
		{
//			if(lbxgl_shader_texflat>0)
				{ PDGL_BindShader(lbxgl_shader_texflat); }

//			pdglEnable(GL_TEXTURE_2D);
			pdglEnableTexture2D();
			if(lbxgl_shader_cur->tex_alt>0)
			{
				glBindTexture(GL_TEXTURE_2D,
					lbxgl_shader_cur->tex_alt);
			}else
			{
				glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);
			}

//			if(lbxgl_shader_texflat>0)
			{
//				PDGL_Uniform1i("texBase", 0);
				pdglUniform1i(lbxgl_shader_texflat_texBase, 0);
				pdglUniform1i(lbxgl_shader_texflat_texIsUVAY,
					(lbxgl_shader_cur->flags&LBXGL_TXFL_COLOR_UVAY)?1:0);
			}
		}else
		{
//			if(lbxgl_shader_texdefault>0)
				{ PDGL_BindShader(lbxgl_shader_texdefault); }

//			pdglEnable(GL_TEXTURE_2D);
			pdglEnableTexture2D();
			if(lbxgl_shader_cur->tex_alt>0)
			{
				glBindTexture(GL_TEXTURE_2D,
					lbxgl_shader_cur->tex_alt);
			}else
			{
				glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);
			}

//			if(lbxgl_shader_texdefault>0)
			{
//				PDGL_Uniform1i("texBase", 0);
				pdglUniform1i(lbxgl_shader_texdefault_texBase, 0);
				pdglUniform1i(lbxgl_shader_texdefault_texIsUVAY,
					(lbxgl_shader_cur->flags&LBXGL_TXFL_COLOR_UVAY)?1:0);
			}
		}
	}
#if 0
	else
	{
//		pdglEnable(GL_TEXTURE_2D);
		pdglEnableTexture2D();

		if(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT)
		{
			PDGL_BindShader(lbxgl_shader_texfinal);
		}else
		{
			PDGL_BindShader(lbxgl_shader_texdefault);
		}

//		PDGL_BindShader(lbxgl_shader_texdefault);
		glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);
//		PDGL_Uniform1i("texBase", 0);
		pdglUniform1i(lbxgl_shader_texdefault_texBase, 0);
		pdglUniform1i(lbxgl_shader_texdefault_texIsUVAY,
			(lbxgl_shader_cur->flags&LBXGL_TXFL_COLOR_UVAY)?1:0);
	}
#endif

	if(lbxgl_shader_cur->flags2&BTGE_TXFL2_NODEPTHWRITE)
	{
		glDepthMask(GL_FALSE);
	}
}

/** Bind current shader texture */
LBXGL_API void LBXGL_Shader_BindTexture(int num)
{
	LBXGL_TexImage *cur;
	short *pcm;
	char *s;
	float f, g, dt;
	int i, j;

	if(num<=0)
	{
		if(!lbxgl_shader_cur)
			return;
		LBXGL_Shader_FlushTexture();
		lbxgl_shader_cur=NULL;
		if(lbxgl_shader_light)
		{
			LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
		}else
		{
			PDGL_UnbindShader();
		}
		return;
	}

//	if(lbxgl_shader_cur && (lbxgl_shader_cur->num==num))
//		return;

	if(!lbxgl_shader_cur || (lbxgl_shader_cur->num!=num))
	{
		LBXGL_Shader_FlushTexture();
		lbxgl_shader_cur=LBXGL_Texture_LookupNum(num);
	}

//	lbxgl_shader_cur=LBXGL_Texture_LookupNum(num);

	if(lbxgl_shader_cur)
	{
		if(lbxgl_shader_cur->anim_next>0)
		{
			if(lbxgl_shader_cur->flags2&BTGE_TXFL2_ANIMMATCHSOUND)
			{
				lbxgl_shader_cur=
					LBXGL_Shader_BindTexture_AnimMatchSound(
						lbxgl_shader_cur);
			}else
			{
				j=lbxgl_state->time_f*5;
				j%=64;
				while(j--)
				{
					i=lbxgl_shader_cur->anim_next;
//					lbxgl_shader_cur=LBXGL_Texture_LookupNum(i);
					cur=LBXGL_Texture_LookupNum(i);
					if(!cur)break;
					lbxgl_shader_cur=cur;
				}
			}
		}else if(lbxgl_shader_cur->anim_video)
		{
			if(!AVI_GetDeltaTick(lbxgl_shader_cur->anim_video))
			{
				AVI_SetDeltaTick(lbxgl_shader_cur->anim_video);

				f=lbxgl_state->time_f;
				g=lbxgl_shader_cur->anim_time;
				dt=(g>0)?(f-g):0;
				if(dt>0.25)dt=0;
				lbxgl_shader_cur->anim_time=f;

				lbxgl_shader_cur->tex_alt=
					AVI_FrameTexnum(lbxgl_shader_cur->anim_video, dt);

				if(AVI_HasAudioP(lbxgl_shader_cur->anim_video));
				{
					AVI_GetAudioSamples(lbxgl_shader_cur->anim_video,
						&pcm, &j);
					s=AVI_GetVideoName(lbxgl_shader_cur->anim_video);
					PDGL_Sound_WriteStreamStereoSamples(s, pcm, j);
					AVI_ClearAudioSamples(lbxgl_shader_cur->anim_video);
				}
			}else
			{
				lbxgl_shader_cur->tex_alt=
					AVI_GetFrameTexnum(lbxgl_shader_cur->anim_video);
//				lbxgl_shader_cur->tex_alt=
//					AVI_FrameTexnum(lbxgl_shader_cur->anim_video, 0);
			}			
		}
		
		if(lbxgl_shader_light)
		{
			if(!(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT))
				LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
		}

		LBXGL_Shader_SetupTexture();
	}else
	{
		if(lbxgl_shader_light)
			LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
	}
}

/** Get currently bound texture */
LBXGL_API int LBXGL_Shader_GetCurrentTexture(void)
{
	if(lbxgl_shader_cur)
		return(lbxgl_shader_cur->num);
	return(0);
}

/** Get texture image for currently bound texture. */
LBXGL_API int LBXGL_Shader_GetCurrentTextureImage(void)
{
	if(lbxgl_shader_cur)
	{
		if(lbxgl_shader_cur->tex_alt>0)
			return(lbxgl_shader_cur->tex_alt);
		return(lbxgl_shader_cur->num);
	}
	return(0);
}

LBXGL_API int LBXGL_Shader_GetCurrentTextureFlags(void)
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

LBXGL_API void LBXGL_Shader_SetCurrentMaterial(void)
{
	LBXGL_Texture_SetImageMaterialImg(lbxgl_shader_cur);
}

LBXGL_API void LBXGL_Shader_SetCurrentLight(LBXGL_Light *light)
{
	lbxgl_shader_light=light;
}

LBXGL_API void LBXGL_Shader_SetCurrentMixerChannel(int num)
{
	lbxgl_shader_sndmixchan=num;
}

LBXGL_API void LBXGL_Shader_SetGlossEnv(int tex)
	{ lbxgl_shader_gloss2=tex; }
LBXGL_API void LBXGL_Shader_SetNoGloss(int fl)
	{ lbxgl_shader_nogloss=fl; }
LBXGL_API void LBXGL_Shader_SetBudget(int fl)
	{ lbxgl_shader_isbudget=fl; }
LBXGL_API void LBXGL_Shader_SetParallax(int fl)
	{ lbxgl_shader_useparallax=fl; }

LBXGL_API void LBXGL_Shader_SetFlat(int fl)
	{ lbxgl_shader_isflat=fl; }
LBXGL_API void LBXGL_Shader_SetFinal(int fl)
	{ lbxgl_shader_isfinal=fl; }

/** Set current blending functions */
LBXGL_API void LBXGL_Shader_BlendFunc(int src, int dst)
{
	lbxgl_shader_blend_src=src;
	lbxgl_shader_blend_dst=dst;

	lbxgl_shader_lastblend_src=src;
	lbxgl_shader_lastblend_dst=dst;

	glBlendFunc(src, dst);
}

LBXGL_API void LBXGL_Shader_SetUseScreen(int tex)
	{ lbxgl_shader_tex_usescreen=tex; }
LBXGL_API void LBXGL_Shader_SetUseDepth(int tex)
	{ lbxgl_shader_tex_usedepth=tex; }

LBXGL_API void LBXGL_Shader_ForceDraw(int fl)
{
	lbxgl_shader_forcedraw=fl;
}

LBXGL_API void LBXGL_Shader_ClearTexture(void)
{
	lbxgl_shader_cur=NULL;
}

LBXGL_API void LBXGL_Shader_BeginPoly(void)
{
	LBXGL_Shader_Init();
	lbxgl_shader_nvec=0;
	lbxgl_shader_prim=PDGL_POLYGON;
}

LBXGL_API void LBXGL_Shader_EndPoly(void)
{
	LBXGL_Shader_EndPrim(PDGL_POLYGON);
}

LBXGL_API void LBXGL_Shader_Begin(int prim)
{
	LBXGL_Shader_Init();
	lbxgl_shader_nvec=0;
	lbxgl_shader_prim=prim;
}

LBXGL_API void LBXGL_Shader_End(void)
	{ LBXGL_Shader_EndPrim(lbxgl_shader_prim); }
LBXGL_API void LBXGL_Shader_EndFlat(void)
	{ LBXGL_Shader_EndPrimFlat(lbxgl_shader_prim); }
LBXGL_API void LBXGL_Shader_EndFlatVA(void)
	{ LBXGL_Shader_EndPrimFlatVA(lbxgl_shader_prim); }

LBXGL_API void LBXGL_Shader_BeginLight(int prim)
{
	LBXGL_Shader_Init();
	lbxgl_shader_nvec=0;
	lbxgl_shader_prim=prim;
}

LBXGL_API void LBXGL_Shader_EndLight(void)
{
	if(lbxgl_shader_cur &&
		(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT))
	{
		LBXGL_Shader_EndPrim_DoWarp(lbxgl_shader_prim);
		LBXGL_BrushLight_DrawPrimLightImage3(
			lbxgl_shader_cur, lbxgl_shader_light, lbxgl_shader_prim,
			lbxgl_shader_xyzbuf, lbxgl_shader_stbuf,
			lbxgl_shader_nbuf, lbxgl_shader_clrbuf,
			lbxgl_shader_nxbuf, lbxgl_shader_nybuf,
			lbxgl_shader_nvec);
//		LBXGL_BrushLight_DrawPrimLight_Reset(lbxgl_shader_light);
		return;
	}
//	LBXGL_Shader_EndPrimFlatNoRGB(lbxgl_shader_prim);
	LBXGL_Shader_EndPrimFlat(lbxgl_shader_prim);

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

	i=4*sizeof(float);
	PDGL_DrawPrim_DrawArraysNormalTexRGB(
		prim, 0, nxyz,
		3, GL_FLOAT, i, xyz,
		2, GL_FLOAT, 0, st,
		3, GL_FLOAT, i, norm,
		4, GL_FLOAT, 0, rgba);

#if 0
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
#endif
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

int lbxgl_shader_mtex_layers;

LBXGL_API void LBXGL_Shader_GetWindowOrigin(int *rxs, int *rys)
{
	*rxs=lbxgl_cam->v_xo;
	*rys=lbxgl_cam->v_yo;
//	GfxDrv_GetWindowSize(rxs, rys);
}

LBXGL_API void LBXGL_Shader_GetWindowSize(int *rxs, int *rys)
{
	*rxs=lbxgl_cam->v_xs;
	*rys=lbxgl_cam->v_ys;
//	GfxDrv_GetWindowSize(rxs, rys);
}

LBXGL_API void LBXGL_Shader_GetWindowMaxSize(int *rxs, int *rys)
{
//	*rxs=lbxgl_cam->v_xs;
//	*rys=lbxgl_cam->v_ys;
	GfxDrv_GetWindowMaxSize(rxs, rys);
}

LBXGL_API void LBXGL_Shader_GetWindowTexSize(int *rxs, int *rys)
{
//	*rxs=lbxgl_cam->v_xs;
//	*rys=lbxgl_cam->v_ys;
	GfxDrv_GetWindowTexSize(rxs, rys);
}

LBXGL_API void LBXGL_Shader_GetWindowMaxTexSize(int *rxs, int *rys)
{
//	*rxs=lbxgl_cam->v_xs;
//	*rys=lbxgl_cam->v_ys;
	GfxDrv_GetWindowMaxTexSize(rxs, rys);
}

LBXGL_API void LBXGL_Shader_EndPrim_SetupForShader(int shader)
{
	static char *texParm[]={
		"texParm0", "texParm1", "texParm2", "texParm3",
		"texParm4", "texParm5", "texParm6", "texParm7"};
	static char *floatParm[]={
		"floatParm0", "floatParm1", "floatParm2", "floatParm3",
		"floatParm4", "floatParm5", "floatParm6", "floatParm7"};
	static char *vecParm[]={
		"vecParm0", "vecParm1", "vecParm2", "vecParm3",
		"vecParm4", "vecParm5", "vecParm6", "vecParm7"};

	char tb[64];
	float tv0[4];
	float f, g;
	char *s;
	int txn, tyn, txyn;
	int xs, ys;
	int i, j, k, l;


	while(glGetError());	//clear errors

	PDGL_BindShader(shader);
//	PDGL_Uniform1f("time", lbxgl_state->time_f);

	glDisable(GL_TEXTURE_2D);

	l=0;

	if(PDGL_CheckForUniform("screenOrigin"))
	{
//		GfxDrv_GetWindowSize(&xs, &ys);
		LBXGL_Shader_GetWindowOrigin(&xs, &ys);
		PDGL_Uniform2f("screenOrigin", xs, ys);
	}

	if(PDGL_CheckForUniform("screenSize"))
	{
//		GfxDrv_GetWindowSize(&xs, &ys);
		LBXGL_Shader_GetWindowSize(&xs, &ys);
		PDGL_Uniform2f("screenSize", xs, ys);
	}

	if(PDGL_CheckForUniform("screenTexSize"))
	{
//		GfxDrv_GetWindowSize(&xs, &ys);
//		GfxDrv_GetWindowTexSize(&xs, &ys);
//		LBXGL_Shader_GetWindowTexSize(&xs, &ys);
		LBXGL_Shader_GetWindowMaxTexSize(&xs, &ys);
		PDGL_Uniform2f("screenTexSize", xs, ys);
	}

//	j=PDGL_Uniform1i("texScreen", 0);
//	if(j>0)
	if(PDGL_CheckForUniform("texScreen"))
	{
		if(tex_screen<=0)
		{
			tex_screen=Tex_ScreenshotTexture();
			tex_screen_ok=0;
		}

		if(lbxgl_shader_cur->flags&LBXGL_TXFL_NEWSCREEN)
			tex_screen_ok=0;

		if(!tex_screen_ok && (lbxgl_shader_tex_usescreen<=0))
		{
			Tex_FillTextureScreenshot(tex_screen);
			tex_screen_ok=1;
		}

		i=l++;
		pdglActiveTexture(i);
		if(lbxgl_shader_tex_usescreen>0)
			{ glBindTexture(GL_TEXTURE_2D, lbxgl_shader_tex_usescreen); }
		else
			{ glBindTexture(GL_TEXTURE_2D, tex_screen); }
		glEnable(GL_TEXTURE_2D);
		PDGL_Uniform1i("texScreen", i);
	}

//	j=PDGL_Uniform1i("texScreenDepth", 0);
//	if(j>0)
	if(PDGL_CheckForUniform("texScreenDepth"))
	{
//		printf("")
//		*(int *)-1=-1;
	
		if(tex_screen_depth<=0)
		{
			tex_screen_depth=Tex_ScreenshotDepthTexture();
			tex_screen_depth_ok=0;
		}

		if(lbxgl_shader_cur->flags&LBXGL_TXFL_NEWSCREEN)
			tex_screen_depth_ok=0;

		if(!tex_screen_depth_ok && (lbxgl_shader_tex_usedepth<=0))
		{
			Tex_FillTextureScreenshotDepth(tex_screen_depth);
			tex_screen_depth_ok=1;
		}

		i=l++;
		pdglActiveTexture(i);
		if(lbxgl_shader_tex_usedepth>0)
			{ glBindTexture(GL_TEXTURE_2D, lbxgl_shader_tex_usedepth); }
		else
			{ glBindTexture(GL_TEXTURE_2D, tex_screen_depth); }
		glEnable(GL_TEXTURE_2D);
		PDGL_Uniform1i("texScreenDepth", i);

		i=glGetError();
		while(i)
		{
			printf("LBXGL_Shader_EndPrim_DrawShader(B): Error %08X\n", i);
			i=glGetError();
		}
	}

	if((lbxgl_shader_cur->num>=0) && PDGL_CheckForUniform("texBase"))
	{
		i=l++;
		pdglActiveTexture(i);
		if(lbxgl_shader_cur->tex_alt>0)
			{ glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->tex_alt); }
		else
			{ glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num); }
		glEnable(GL_TEXTURE_2D);
		PDGL_Uniform1i("texBase", i);
	}

	if((lbxgl_shader_cur->tex_norm>0) && PDGL_CheckForUniform("texNorm"))
	{
		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->tex_norm);
		glEnable(GL_TEXTURE_2D);
		PDGL_Uniform1i("texNorm", i);
	}

	if((lbxgl_shader_cur->tex_gloss>0) && PDGL_CheckForUniform("texGloss"))
	{
		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->tex_gloss);
		glEnable(GL_TEXTURE_2D);
		PDGL_Uniform1i("texGloss", i);
	}

#if 0
	if(lbxgl_shader_cur->tex_spec>0)
	{
		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->tex_spec);
		glEnable(GL_TEXTURE_2D);
		PDGL_Uniform1i("texSpec", i);
	}
#endif

//	j=PDGL_Uniform1i("texGlow", 0);
	if(PDGL_CheckForUniform("texGlow"))
	{
		k=(lbxgl_shader_cur->tex_glow>0)?
			lbxgl_shader_cur->tex_glow:
			lbxgl_shader_tex_black;
//			lbxgl_shader_gloss;

		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D, k);
		glEnable(GL_TEXTURE_2D);
		PDGL_Uniform1i("texGlow", i);
	}

	j=lbxgl_shader_cur->utn;
	for(i=0; i<j && (l<8); i++)
	{
		if(lbxgl_shader_cur->utex[i]<=0)continue;

//		if(lbxgl_shader_cur->utexvn[i])
//			{ strcpy(tb, lbxgl_shader_cur->utexvn[i]); }
//		else
//			{ sprintf(tb, "texParm%d", i); }

		if(lbxgl_shader_cur->utexvn[i])
			{ s=lbxgl_shader_cur->utexvn[i]; }
		else
			{ s=texParm[i]; }

		i=l++;
		pdglActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D,
			lbxgl_shader_cur->utex[i]);
		glEnable(GL_TEXTURE_2D);
//		PDGL_Uniform1i(tb, i);
		PDGL_Uniform1i(s, i);
	}

	j=lbxgl_shader_cur->ufn;
	for(i=0; i<j; i++)
	{
//		if(lbxgl_shader_cur->uf1vn[i])
//			{ strcpy(tb, lbxgl_shader_cur->uf1vn[i]); }
//		else
//			{ sprintf(tb, "floatParm%d", i); }
		if(lbxgl_shader_cur->uf1vn[i])
			{ s=lbxgl_shader_cur->uf1vn[i]; }
		else
			{ s=floatParm[i]; }

		if(lbxgl_shader_cur->uf1vcn[i])
		{
			f=btCvarGetf(lbxgl_shader_cur->uf1vcn[i]);
//			PDGL_Uniform1f(tb, f);
			PDGL_Uniform1f(s, f);
		}
		else
		{
//			PDGL_Uniform1f(tb, lbxgl_shader_cur->ufv[i]);
			PDGL_Uniform1f(s, lbxgl_shader_cur->ufv[i]);
		}
	}

	j=lbxgl_shader_cur->ufvn;
	for(i=0; i<j; i++)
	{
//		if(lbxgl_shader_cur->uf4vn[i])
//			{ strcpy(tb, lbxgl_shader_cur->uf4vn[i]); }
//		else
//			{ sprintf(tb, "vecParm%d", i); }

		if(lbxgl_shader_cur->uf4vn[i])
			{ s=lbxgl_shader_cur->uf4vn[i]; }
		else
			{ s=vecParm[i]; }

		if(lbxgl_shader_cur->uf4vcn[i])
		{
			btCvarGet4fv(lbxgl_shader_cur->uf4vcn[i], tv0);
//			PDGL_Uniform4fv(tb, 1, tv0);
			PDGL_Uniform4fv(s, 1, tv0);
		}
		else
		{
//			PDGL_Uniform4fv(tb, 1, lbxgl_shader_cur->uf4v+i*4);
			PDGL_Uniform4fv(s, 1, lbxgl_shader_cur->uf4v+i*4);
		}
	}

	PDGL_Uniform1f("time", lbxgl_state->time_f);

	PDGL_Uniform1i("texIsUVAY",
		(lbxgl_shader_cur->flags&LBXGL_TXFL_COLOR_UVAY)?1:0);

	pdglActiveTexture(0);
	lbxgl_shader_mtex_layers=l;
}

LBXGL_API void LBXGL_Shader_EndPrim_TeardownForShader(int shader)
{
	int i, j, k, l;

	l=lbxgl_shader_mtex_layers;
	for(i=0; i<l; i++)
	{
		pdglActiveTexture(i);
//		glDisable(GL_TEXTURE_2D);
		pdglDisableTexture2D();
		glDisable(GL_TEXTURE_CUBE_MAP);
	}
	pdglActiveTexture(0);

	PDGL_UnbindShader();

	if(lbxgl_shader_cur->flags&LBXGL_TXFL_KEEPSCREEN)
	{
		tex_screen_ok=0;
		tex_screen_depth_ok=0;
	}

	i=glGetError();
	while(i)
	{
		printf("LBXGL_Shader_EndPrim_DrawShader: Error %08X\n", i);
		i=glGetError();
	}
}

LBXGL_API void LBXGL_Shader_EndPrim_DrawShader(int prim, int shader)
{
	char tb[64];
	int txn, tyn, txyn;
	int i, j, k, l;

	LBXGL_Shader_EndPrim_SetupForShader(shader);

	txn=PDGL_GetAttribLocation("vecSdir");
	tyn=PDGL_GetAttribLocation("vecTdir");
	txyn=((txn>0)&&(tyn>0))?1:0;

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	pdglColor4fv(lbxgl_shader_clr);

//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, lbxgl_shader_cur->num);

	pdglBegin(prim);
	for(i=0; i<lbxgl_shader_nvec; i++)
	{
		pdglColor4fv(lbxgl_shader_clrbuf+i*4);
		pdglTexCoord2fv(lbxgl_shader_stbuf+i*2);
		pdglNormal3fv(lbxgl_shader_nbuf+i*4);
		pdglVertex3fv(lbxgl_shader_xyzbuf+i*4);

		if(txyn)
		{
			pdglVertexAttrib3fv(txn, lbxgl_shader_nxbuf+i*4);
			pdglVertexAttrib3fv(tyn, lbxgl_shader_nybuf+i*4);
		}
		
	}
	pdglEnd();

	LBXGL_Shader_EndPrim_TeardownForShader(shader);
}


LBXGL_API void LBXGL_Shader_EndPrim_DrawGlow(int prim)
{
	int i, j, k, l;

	glBlendFunc(GL_SRC_COLOR, GL_ONE);
	pdglColor4f(1, 1, 1, 1);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,
		lbxgl_shader_cur->tex_glow);

	pdglBegin(prim);
	for(i=0; i<lbxgl_shader_nvec; i++)
	{
		pdglTexCoord2fv(lbxgl_shader_stbuf+i*2);
		pdglNormal3fv(lbxgl_shader_nbuf+i*4);
		pdglVertex3fv(lbxgl_shader_xyzbuf+i*4);
	}
	pdglEnd();

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(lbxgl_shader_blend_src, lbxgl_shader_blend_dst);
}

LBXGL_API void LBXGL_Shader_EndPrim_DrawTexture(int prim)
{
	int i, j, k, l;

#if 0
	if(lbxgl_shader_cur->blend_src || lbxgl_shader_cur->blend_dst)
	{
		glBlendFunc(
			lbxgl_shader_cur->blend_src,
			lbxgl_shader_cur->blend_dst);
	}
#endif

	if((lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT) &&
		!lbxgl_shader_isbudget && lbxgl_shader_useparallax &&
		(lbxgl_shader_texfinal>0))
	{
		LBXGL_Shader_EndPrim_DrawShader(prim, lbxgl_shader_texfinal);

		if(lbxgl_shader_cur->tex_glow>0)
			{ LBXGL_Shader_EndPrim_DrawGlow(prim); }

		return;
	}

//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	pdglColor4fv(lbxgl_shader_clr);

#if 0
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
#endif

	if(lbxgl_shader_cur->flags&LBXGL_TXFL_NODRAW)
	{
		pdglColor4f(
			lbxgl_shader_clr[0],
			lbxgl_shader_clr[1],
			lbxgl_shader_clr[2],
			lbxgl_shader_clr[3]*0.25);

		pdglBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
//			pdglColor4fv(lbxgl_shader_clrbuf+i*4);
			pdglTexCoord2fv(lbxgl_shader_stbuf+i*2);
			pdglNormal3fv(lbxgl_shader_nbuf+i*4);
			pdglVertex3fv(lbxgl_shader_xyzbuf+i*4);
		}
		pdglEnd();
	}else
	{
//		if(!(lbxgl_shader_cur->flags&LBXGL_TXFL_LIGHTMAT) &&
//			!lbxgl_shader_isbudget)
		LBXGL_Shader_EndPrim_DrawArrays(prim,
			lbxgl_shader_xyzbuf,
			lbxgl_shader_stbuf,
			lbxgl_shader_nbuf,
			lbxgl_shader_clrbuf,
			lbxgl_shader_nvec);
	}

	if(lbxgl_shader_cur->tex_glow>0)
		{ LBXGL_Shader_EndPrim_DrawGlow(prim); }
}

LBXGL_API void LBXGL_Shader_EndPrim_DrawGloss(int prim)
{
	float v0[4], v1[4];
	float trorg[4], trrt[4], trfw[4], trup[4];
	float *s, *t;
	float u, v, w, f, g;
	int i, j, k, l;

//	glEnable(GL_TEXTURE_2D);
//	glDisable(GL_LIGHTING);
	pdglEnableTexture2D();
	pdglDisableLighting();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(lbxgl_shader_gloss2>0)
	{
#ifndef GLES
//		glDisable(GL_TEXTURE_2D);
		pdglDisableTexture2D();

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
//			LBXGL_Shader_TransformVector(t, v0);

			glTexCoord3fv(v0);
//			glTexCoord3fv(t);

			glNormal3fv(t);
			glVertex3fv(s);
		}
		glEnd();

		glDisable(GL_TEXTURE_CUBE_MAP);
		glEnable(GL_TEXTURE_2D);

		glBlendFunc(lbxgl_shader_blend_src, lbxgl_shader_blend_dst);
#endif
	}else
	{
		glBindTexture(GL_TEXTURE_2D, lbxgl_shader_gloss);

//		u=0.5;
		u=lbxgl_shader_cur->gloss;
		V3F_COPY(lbxgl_cam->org, trorg);

		LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, trorg);
		LBXGL_Shader_TransformVectorLocal(lbxgl_cam->rt, trrt);
		LBXGL_Shader_TransformVectorLocal(lbxgl_cam->up, trup);
		LBXGL_Shader_TransformVectorLocal(lbxgl_cam->fw, trfw);

		pdglBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			s=lbxgl_shader_clrbuf+i*4;
			V4F_COPY(s, v0);
			v0[3]*=lbxgl_shader_cur->gloss;
			pdglColor4fv(v0);

			s=lbxgl_shader_xyzbuf+i*4;
			t=lbxgl_shader_nbuf+i*4;

			u=V3F_DOT(s, trrt)-V3F_DOT(trorg, trrt);
			v=V3F_DOT(s, trup)-V3F_DOT(trorg, trup);
			w=V3F_DOT(s, trfw)-V3F_DOT(trorg, trfw);
//			w*=4;
//			if(w<1)w=1;
			if(w<16)w=16;
			u/=w; v/=w;

			u+=V3F_DOT(t, trrt);
			v+=V3F_DOT(t, trup);

			pdglTexCoord2f(u*0.25, v*0.25);
			pdglNormal3fv(t);
			pdglVertex3fv(s);
		}
		pdglEnd();

		glBlendFunc(lbxgl_shader_blend_src, lbxgl_shader_blend_dst);
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

	if(!lbxgl_shader_cur)
	{
		pdglColor4fv(lbxgl_shader_clr);
//		glDisable(GL_TEXTURE_2D);
		pdglDisableTexture2D();

		PDGL_DrawPrim_DrawArraysNormalFlatRGB(
			prim, 0, lbxgl_shader_nvec,
			3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_xyzbuf,
			3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_nbuf,
			4, GL_FLOAT, 4*sizeof(float), lbxgl_shader_clrbuf);

#if 0
		glBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			glColor4fv(lbxgl_shader_clrbuf+i*4);
			glNormal3fv(lbxgl_shader_nbuf+i*4);
			glVertex3fv(lbxgl_shader_xyzbuf+i*4);
		}
		glEnd();
#endif

		return;
	}

	if(lbxgl_shader_cur->flags&LBXGL_TXFL_NODRAW)
	{
		if(!lbxgl_shader_forcedraw)
			return;
	}

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
		LBXGL_Shader_EndPrim_DrawShader(prim, lbxgl_shader_cur->glsdr);
	}else if(lbxgl_shader_cur->num>=0)
	{
		LBXGL_Shader_EndPrim_DrawTexture(prim);
	}else
	{
//		glDisable(GL_TEXTURE_2D);
		pdglDisableTexture2D();

		PDGL_DrawPrim_DrawArraysNormalFlatRGB(
			prim, 0, lbxgl_shader_nvec,
			3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_xyzbuf,
			3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_nbuf,
			4, GL_FLOAT, 4*sizeof(float), lbxgl_shader_clrbuf);

#if 0
		glBegin(prim);
		for(i=0; i<lbxgl_shader_nvec; i++)
		{
			glColor4fv(lbxgl_shader_clrbuf+i*4);
			glNormal3fv(lbxgl_shader_nbuf+i*4);
			glVertex3fv(lbxgl_shader_xyzbuf+i*4);
		}
		glEnd();
#endif
	}
#endif

	if((lbxgl_shader_cur->flags&LBXGL_TXFL_GLOSS) &&
		!lbxgl_shader_nogloss)
	{
		LBXGL_Shader_EndPrim_DrawGloss(prim);
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

	PDGL_DrawPrim_DrawArraysNormalTexRGB(
		prim, 0, lbxgl_shader_nvec,
		3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_xyzbuf,
		2, GL_FLOAT, 2*sizeof(float), lbxgl_shader_stbuf,
		3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_nbuf,
		4, GL_FLOAT, 4*sizeof(float), lbxgl_shader_clrbuf);

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

	PDGL_DrawPrim_DrawArraysNormalTex(
		prim, 0, lbxgl_shader_nvec,
		3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_xyzbuf,
		2, GL_FLOAT, 2*sizeof(float), lbxgl_shader_stbuf,
		3, GL_FLOAT, 4*sizeof(float), lbxgl_shader_nbuf);

#if 0
	glBegin(prim);
	for(i=0; i<lbxgl_shader_nvec; i++)
	{
		glTexCoord2fv(lbxgl_shader_stbuf+i*2);
//		glColor4fv(lbxgl_shader_clrbuf+i*4);
		glNormal3fv(lbxgl_shader_nbuf+i*4);
		glVertex3fv(lbxgl_shader_xyzbuf+i*4);
	}
	glEnd();
#endif
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


//	if(lbxgl_shader_light)
	if(1)
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

LBXGL_API void LBXGL_Shader_Normal3sbv(sbyte *v)
{
//	float *t;

	lbxgl_shader_ntmp[0]=v[0]*(1.0/127.0);
	lbxgl_shader_ntmp[1]=v[1]*(1.0/127.0);
	lbxgl_shader_ntmp[2]=v[2]*(1.0/127.0);

//	V3F_COPY(v, lbxgl_shader_ntmp);
	lbxgl_shader_ntmp[3]=0;
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
	pdglColor3fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_Color4fv(float *v)
{
	V4F_COPY(v, lbxgl_shader_clr);
	pdglColor4fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_Color4ubv(byte *v)
{
	lbxgl_shader_clr[0]=v[0]*(1.0/255.0);
	lbxgl_shader_clr[1]=v[1]*(1.0/255.0);
	lbxgl_shader_clr[2]=v[2]*(1.0/255.0);
	lbxgl_shader_clr[3]=v[3]*(1.0/255.0);

//	V4F_COPY(v, lbxgl_shader_clr);
	pdglColor4fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_Color3f(float r, float g, float b)
{
	lbxgl_shader_clr[0]=r;
	lbxgl_shader_clr[1]=g;
	lbxgl_shader_clr[2]=b;

	pdglColor4fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_Color4f(float r, float g, float b, float a)
{
	lbxgl_shader_clr[0]=r;
	lbxgl_shader_clr[1]=g;
	lbxgl_shader_clr[2]=b;
	lbxgl_shader_clr[3]=a;

	pdglColor4fv(lbxgl_shader_clr);
}

LBXGL_API void LBXGL_Shader_TexCoord2f(float s, float t)
{
	lbxgl_shader_sttmp[0]=s;
	lbxgl_shader_sttmp[1]=t;
}

LBXGL_API void LBXGL_Shader_Vertex2f(float x, float y)
{
	float tv[3];
	tv[0]=x; tv[1]=y; tv[2]=0;
	LBXGL_Shader_Vertex3fv(tv);
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

LBXGL_API void LBXGL_Shader_NormalX3f(float x, float y, float z)
{
	lbxgl_shader_nxtmp[0]=x;
	lbxgl_shader_nxtmp[1]=y;
	lbxgl_shader_nxtmp[2]=z;
	lbxgl_shader_nxtmp[3]=0;
}

LBXGL_API void LBXGL_Shader_NormalY3f(float x, float y, float z)
{
	lbxgl_shader_nytmp[0]=x;
	lbxgl_shader_nytmp[1]=y;
	lbxgl_shader_nytmp[2]=z;
	lbxgl_shader_nytmp[3]=0;
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

LBXGL_API void LBXGL_Shader_DrawArraysSimpleFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz)
{
#ifdef GLES
	PDGL_DrawPrim_DrawArraysSimpleFlat(prim, base, nxyz,
		xyzsize, xyztype, xyzstep, xyz);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}

LBXGL_API void LBXGL_Shader_DrawArraysNormalFlat(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm)
{
#ifdef GLES
	PDGL_DrawPrim_DrawArraysNormalFlat(prim, base, nxyz,
		xyzsize, xyztype, xyzstep, xyz,
		normsize, normtype, normstep, norm);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glNormalPointer(normtype, normstep, norm);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
#endif
}

LBXGL_API void LBXGL_Shader_DrawArraysNormalFlatRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
#ifdef GLES
	PDGL_DrawPrim_DrawArraysNormalFlatRGB(prim, base, nxyz,
		xyzsize, xyztype, xyzstep, xyz,
		normsize, normtype, normstep, norm,
		rgbsize, rgbtype, rgbstep, rgb);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glNormalPointer(normtype, normstep, norm);
	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
}

LBXGL_API void LBXGL_Shader_DrawArraysNormalTex(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm)
{
#ifdef GLES
	PDGL_DrawPrim_DrawArraysNormalTex(prim, base, nxyz,
		xyzsize, xyztype, xyzstep, xyz,
		stsize, sttype, ststep, st,
		normsize, normtype, normstep, norm);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glTexCoordPointer(stsize, sttype, ststep, st);
	glNormalPointer(normtype, normstep, norm);
//	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);
#endif
}

LBXGL_API void LBXGL_Shader_DrawArraysNormalTexRGB(
	int prim, int base, int nxyz,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
#ifdef GLES
	PDGL_DrawPrim_DrawArraysNormalTexRGB(prim, base, nxyz,
		xyzsize, xyztype, xyzstep, xyz,
		stsize, sttype, ststep, st,
		normsize, normtype, normstep, norm,
		rgbsize, rgbtype, rgbstep, rgb);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glTexCoordPointer(stsize, sttype, ststep, st);
	glNormalPointer(normtype, normstep, norm);
	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
	glDrawArrays(prim, base, nxyz);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
}


LBXGL_API void LBXGL_Shader_DrawElementsSimpleFlat(
	int prim, int nelem, int elemtype, void *elems,
	int xyzsize, int xyztype, int xyzstep, void *xyz)
{
#ifdef GLES
//	PDGL_DrawPrim_DrawArraysSimpleFlat(prim, base, nxyz,
//		xyzsize, xyztype, xyzstep, xyz);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
//	glDrawArrays(prim, base, nxyz);
	glDrawElements(prim, nelem, elemtype, elems);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}


LBXGL_API void LBXGL_Shader_DrawElementsNormalTexRGB(
	int prim, int nelem, int elemtype, void *elems,
	int xyzsize, int xyztype, int xyzstep, void *xyz, 
	int stsize, int sttype, int ststep, void *st, 
	int normsize, int normtype, int normstep, void *norm,
	int rgbsize, int rgbtype, int rgbstep, void *rgb)
{
#ifdef GLES
	PDGL_DrawPrim_DrawElementsNormalTexRGB(
		prim, nelem, elemtype, elems,
		xyzsize, xyztype, xyzstep, xyz,
		stsize, sttype, ststep, st,
		normsize, normtype, normstep, norm,
		rgbsize, rgbtype, rgbstep, rgb);
#else
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(xyzsize, xyztype, xyzstep, xyz);
	glTexCoordPointer(stsize, sttype, ststep, st);
	glNormalPointer(normtype, normstep, norm);
	glColorPointer(rgbsize, rgbtype, rgbstep, rgb);
//	glDrawArrays(prim, base, nxyz);
	glDrawElements(prim, nelem, elemtype, elems);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
}
