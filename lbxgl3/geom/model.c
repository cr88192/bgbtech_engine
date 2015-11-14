#include <lbxgl2.h>

char *lbxgl_mdl_name[4096];
void *lbxgl_mdl_obj[4096];
int lbxgl_mdl_num=0;

LBXGL_ModelState *lbxgl_mdl_curmdl=NULL;
void *lbxgl_mdl_mutex=NULL;

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

void LBXGL_Mdl_Init()
{
	static int init=0;
	
	if(init)return;
	init=1;
	
	lbxgl_mdl_mutex=thFastMutex();
}

void lbxgl_mdl_lock()
{
	thLockFastMutex(lbxgl_mdl_mutex);
}

void lbxgl_mdl_unlock()
{
	thUnlockFastMutex(lbxgl_mdl_mutex);
}

LBXGL_API void *LBXGL_Mdl_LoadModel(char *name)
{
	char tb[256];
	char *ext;
	void *p;
	int i;

	LBXGL_Mdl_Init();

	lbxgl_mdl_lock();
	for(i=0; i<lbxgl_mdl_num; i++)
		if(!strcmp(lbxgl_mdl_name[i], name))
	{
		lbxgl_mdl_unlock();
		return(lbxgl_mdl_obj[i]);
	}
	lbxgl_mdl_unlock();

	ext=vfgetext(name);

//	printf("LBXGL_Mdl_LoadModel: load %s\n", name);

	if(!ext)
	{
		sprintf(tb, "models/%s.ac", name);
		p=LBXGL_Mdl_LoadModel(tb);

		if(!p)
		{
			sprintf(tb, "models/%s.model", name);
			p=LBXGL_Mdl_LoadModel(tb);
		}

		if(!p)
		{
			sprintf(tb, "models/%s.mdl", name);
			p=LBXGL_Mdl_LoadModel(tb);
		}

		if(!p)
		{
			sprintf(tb, "models/%s.md2", name);
			p=LBXGL_Mdl_LoadModel(tb);
		}

		if(!p)
		{
			sprintf(tb, "models/%s.md3", name);
			p=LBXGL_Mdl_LoadModel(tb);
		}

		if(!p)
		{
			sprintf(tb, "sprites/%s_sprinfo.txt", name);
			p=LBXGL_Sprite_LoadSprites(tb);
		}

		if(!p)p=LBXGL_Sprite_LookupFlare(name);
		if(!p)p=LBXGL_Frac_GenModel(name);

		if(!p)p=LBXGL_BrushPrefab_GetBModelName(name);

		if(p)
		{
			lbxgl_mdl_lock();
			i=lbxgl_mdl_num++;
			lbxgl_mdl_name[i]=dystrdup(name);
			lbxgl_mdl_obj[i]=p;
			lbxgl_mdl_unlock();
			return(p);
		}

		printf("LBXGL_Mdl_LoadModel: fail load %s\n", name);

		return(NULL);
	}

	lbxgl_mdl_lock();

	p=NULL;

	if(!strcmp(ext, "ac"))p=LBXGL_AC3D_LoadModel(name);
	if(!strcmp(ext, "model"))p=Skel_LoadModel(name);

	if(!strcmp(ext, "mdl"))p=LBXGL_QuakeMDL_LoadModel(name);
	if(!strcmp(ext, "md2"))p=LBXGL_QuakeMDL_LoadModel(name);
	if(!strcmp(ext, "md3"))p=LBXGL_QuakeMDL_LoadModel(name);

	if(!strcmp(ext, "ase"))p=LBXGL_ASE_LoadModel(name);

	if(!p)p=LBXGL_BrushPrefab_GetBModelName(name);

	if(p)
	{
		i=lbxgl_mdl_num++;
		lbxgl_mdl_name[i]=dystrdup(name);
		lbxgl_mdl_obj[i]=p;

		lbxgl_mdl_unlock();
		return(p);
	}

	printf("LBXGL_Mdl_LoadModel: fail load %s\n", name);

	lbxgl_mdl_unlock();
	return(NULL);
}

void *LBXGL_Mdl_LoadModelInstance_cb(LBXGL_ModelState *tmp)
{
	char *name;
	void *p;

	name=tmp->mdl;

	p=LBXGL_Mdl_LoadModel(name);
	if(!p)return(NULL);
	
	LBXGL_Mdl_SetModel(tmp, p);
	glFinish();
	
	return(BGBGC_NULLEND);
}

LBXGL_API LBXGL_ModelState *LBXGL_Mdl_LoadModelInstance(char *name)
{
	LBXGL_ModelState *tmp;
	void *p;

#if 0
	tmp=LBXGL_Mdl_WrapModelInstance(dystrdup(name));
	pdglAddJob(LBXGL_Mdl_LoadModelInstance_cb, tmp);
	return(tmp);
#endif

#if 1
	p=LBXGL_Mdl_LoadModel(name);
	if(!p)return(NULL);

	tmp=LBXGL_Mdl_WrapModelInstance(p);
	return(tmp);
#endif
}

LBXGL_API LBXGL_ModelState *LBXGL_Mdl_WrapModelInstance(void *mdl)
{
	LBXGL_ModelState *tmp;

//	if(!mdl)return(NULL);

	tmp=gctalloc("lbxgl_modelstate_t", sizeof(LBXGL_ModelState));
	tmp->mdl=mdl;
	tmp->mdltype=dygettype(mdl);
	tmp->mvt=LBXGL_Method_GetRenderMethods(tmp->mdltype);

	if(dytypep(mdl, "lbxgl_skel2model_t"))
		tmp->state=Skel_NewInstance(mdl);
	if(dytypep(mdl, "lbxgl_sprite_t"))
		tmp->state=LBXGL_Sprite_NewStateInstance(mdl);

	V3F_ZERO(tmp->org);
	V3F_SET(tmp->rot+0, 1, 0, 0);
	V3F_SET(tmp->rot+3, 0, 1, 0);
	V3F_SET(tmp->rot+6, 0, 0, 1);

	return(tmp);
}

LBXGL_API void LBXGL_Mdl_SetModel(LBXGL_ModelState *ctx, void *mdl)
{
	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		Skel_FreeInstance(ctx->state);
		ctx->state=NULL;
	}

	if(dytypep(mdl, "lbxgl_skel2model_t"))
		ctx->state=Skel_NewInstance(mdl);
	if(dytypep(mdl, "lbxgl_sprite_t"))
		ctx->state=LBXGL_Sprite_NewStateInstance(mdl);

	ctx->mdl=mdl;
	ctx->mdltype=dygettype(mdl);
	ctx->mvt=LBXGL_Method_GetRenderMethods(ctx->mdltype);

//	if(dytypep(mdl, "lbxgl_skel2model_t"))
//		ctx->state=Skel_NewInstance(mdl);
//	if(dytypep(mdl, "lbxgl_sprite_t"))
//		ctx->state=LBXGL_Sprite_NewStateInstance(mdl);
}

LBXGL_API void LBXGL_Mdl_SetModelName(LBXGL_ModelState *ctx, char *mdlname)
{
	void *ptr;

	ptr=LBXGL_Mdl_LoadModel(mdlname);
	if(!ptr)return;

	LBXGL_Mdl_SetModel(ctx, ptr);
}

LBXGL_API void LBXGL_Mdl_FreeModel(LBXGL_ModelState *ctx)
{
	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		lbxgl_mdl_lock();
		Skel_FreeInstance(ctx->state);
		ctx->state=NULL;
		lbxgl_mdl_unlock();
	}
	gcfree(ctx);
}

LBXGL_API LBXGL_ModelState *LBXGL_Mdl_GetCurrentModel(void)
	{ return(lbxgl_mdl_curmdl); }

LBXGL_API void LBXGL_Mdl_SetAnim(LBXGL_ModelState *ctx, char *name)
{
	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		lbxgl_mdl_lock();
		Skel_SetAnim(ctx->state, name);
		lbxgl_mdl_unlock();
	}
}

LBXGL_API void LBXGL_Mdl_SetAnimFrame(LBXGL_ModelState *ctx,
	char *name, int frame)
{
	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		lbxgl_mdl_lock();
		Skel_SetAnimFrame(ctx->state, name, frame);
		lbxgl_mdl_unlock();
	}
}

LBXGL_API void LBXGL_Mdl_SetFrame(LBXGL_ModelState *ctx, int frame)
{
	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		lbxgl_mdl_lock();
		Skel_SetFrame(ctx->state, frame);
		lbxgl_mdl_unlock();
	}

	if(dytypep(ctx->mdl, "lbxgl_sprite_t"))
		LBXGL_Sprite_SetFrame(ctx->state, frame);
}

LBXGL_API void LBXGL_Mdl_SetFrameBone(LBXGL_ModelState *ctx,
	int bone, float *org, float *qrot, int flag)
{
	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		lbxgl_mdl_lock();
		Skel_SetFrameBone(ctx->state, bone, org, qrot, flag);
		lbxgl_mdl_unlock();
	}
}

LBXGL_API void LBXGL_Mdl_StepTime(LBXGL_ModelState *ctx, float dt)
{
	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		lbxgl_mdl_lock();
		Skel_StepTime(ctx->state, dt);
		lbxgl_mdl_unlock();
	}
}

LBXGL_API float LBXGL_Mdl_LodForDistance(float d)
{
	float f;
	f=0;
	if(d>250)f=1;
	if(d>500)f=2;
	if(d>=750)f=sqrt(d/100);
	return(f);
}

LBXGL_API float LBXGL_Mdl_LodForModelDistance(
	LBXGL_ModelState *ctx, float d)
{
	float mins[3], maxs[3], vsz[3];
	float f, sz;

#if 1
	LBXGL_Mdl_BoxModel(ctx, mins, maxs);
	V3F_SUB(maxs, mins, vsz);
	sz=V3F_LEN(vsz);
	
	f=sz/128;
	if(f<1)f=1;
	
	d*=1/f;
#endif
	
	f=0;
	if(d>250)f=1;
	if(d>500)f=2;
	if(d>=750)f=sqrt(d/100);
	return(f);
}

LBXGL_API int LBXGL_Mdl_AlphaModelP(LBXGL_ModelState *ctx)
{
	char *s;
	
	s=ctx->mdltype;
	if(!s)s=dygettype(ctx->mdl);
	if(!strcmp(s, "lbxgl_lensflare_t"))return(1);
	if(!strcmp(s, "lbxgl_sprite_t"))return(1);

//	if(!strcmp(s, "lbxgl_quakemdl_t"))
//		return(1);
	
	if(ctx->mvt)
	{
		if(ctx->mvt->drawmdl_draw && !(ctx->mvt->drawmdl_dark))
			return(1);
	}

	if(ctx->flags&(LBXGL_TXFL_ALPHA|LBXGL_TXFL_VALPHA))
		return(1);

	return(0);
}

LBXGL_API int LBXGL_Mdl_ViewModelP(LBXGL_ModelState *ctx)
{
	if(ctx->effects2&BT_EF2_VIEWMODEL)
		return(1);
	return(0);
}

LBXGL_API float LBXGL_Mdl_CalcModelScale(LBXGL_ModelState *ctx)
{
	float f;
	int i;

	if(ctx->flags&LBXGL_TXFL_SCALE_MASK)
	{
		i=(ctx->flags&LBXGL_TXFL_SCALE_MASK) >>
			LBXGL_TXFL_SCALE_LSHIFT;
			
		f=pow(2, (i>>4)-7)*(1.0+((i&15)/16.0));
//		f=(i&31)/8.0;
//		f=f*f+1;
//		if(i&32)f=1.0/f;
		return(f);
	}
	
	return(1.0);
}

LBXGL_API void LBXGL_Mdl_CalcModelTransform(
	LBXGL_ModelState *ctx, float *tmat)
{
	float scale;
	int i, j;

	scale=LBXGL_Mdl_CalcModelScale(ctx);

	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)
		tmat[i*4+j]=ctx->rot[i*3+j]*scale;

	if(ctx->effects2&BT_EF2_CAM_WEAPONFLIP)
	{
		tmat[0*4+0]*=-1;
		tmat[0*4+1]*=-1;
		tmat[0*4+2]*=-1;

//		tmat[0*4+0]*=-1;
//		tmat[1*4+0]*=-1;
//		tmat[2*4+0]*=-1;
	}

//	tmat[0]=1;
//	tmat[5]=1;
//	tmat[10]=1;

	tmat[12]=ctx->org[0];
	tmat[13]=ctx->org[1];
	tmat[14]=ctx->org[2];
	tmat[15]=1;
}

LBXGL_API void LBXGL_Mdl_CalcModelDelayIndexTransform(
	LBXGL_ModelState *ctx, float *tmat, int idx)
{
	QuatF_ToMatrix(ctx->lr_rot+idx*4, tmat);
	tmat[12]=ctx->lr_org[idx*3+0];
	tmat[13]=ctx->lr_org[idx*3+1];
	tmat[14]=ctx->lr_org[idx*3+2];
}

LBXGL_API void LBXGL_Mdl_CalcModelDelayTransform(
	LBXGL_ModelState *ctx, float *tmat, float delay)
{
	float tm0[16], tm1[16];
	float t, dt, dtq, dtr;
	float f, g;
	int i;

	t=lbxgl_state->time_f;
	dt=t-ctx->lr_time[0];
	if(dt<0)dt=0;
	
	//small delay, interpolate between current and last-saved position
	if(delay<=dt)
	{
		if(dt<0.01)
		{
			LBXGL_Mdl_CalcModelTransform(ctx, tmat);
			return;
		}
	
		LBXGL_Mdl_CalcModelTransform(ctx, tm0);
		LBXGL_Mdl_CalcModelDelayIndexTransform(ctx, tm1, 0);
		f=delay/dt;
		Mat4F_InterpolateTransform(tm0, tm1, tmat, f);
		return;
	}
	
	dtq=(delay-dt)/LBXGL_MDL_SAVEREL_DT;
	i=(int)dtq;
	dtr=dtq-i;
	
	//delay exceeds window, clamp to window
	if(i>=(LBXGL_MDL_SAVEREL_NUM-1))
	{
		LBXGL_Mdl_CalcModelDelayIndexTransform(
			ctx, tmat, LBXGL_MDL_SAVEREL_NUM-1);
		return;
	}

	LBXGL_Mdl_CalcModelDelayIndexTransform(ctx, tm0, i);
	LBXGL_Mdl_CalcModelDelayIndexTransform(ctx, tm1, i+1);
	Mat4F_InterpolateTransform(tm0, tm1, tmat, dtr);
}

LBXGL_API void LBXGL_Mdl_CalcModelDelayRelTransform(
	LBXGL_ModelState *ctx, float *tmat, float delay)
{
	float tm0[16], tm1[16], tm2[16];
	
	LBXGL_Mdl_CalcModelDelayTransform(ctx, tm0, delay);
	LBXGL_Mdl_CalcModelTransform(ctx, tm1);
	Mat4F_InverseTransform(tm1, tm2);
	Mat4F_MatMult(tm0, tm2, tmat);
}

LBXGL_API void LBXGL_Mdl_SaveRelPosition(LBXGL_ModelState *ctx)
{
	float t;
	int i, j;
	
	t=lbxgl_state->time_f;
	if(t<(ctx->lr_time[0]+LBXGL_MDL_SAVEREL_DT))
		return;	//too soon
		
	for(i=LBXGL_MDL_SAVEREL_NUM-1; i>0; i--)
	{
		V3F_COPY(ctx->lr_org+(i-1)*3, ctx->lr_org+i*3);
		V4F_COPY(ctx->lr_rot+(i-1)*4, ctx->lr_rot+i*4);
		ctx->lr_time[i]=ctx->lr_time[i-1];
	}
	
	V3F_COPY(ctx->org, ctx->lr_org);
	QuatF_From3Matrix(ctx->rot, ctx->lr_rot);
	ctx->lr_time[0]=t;
}

LBXGL_API void LBXGL_Mdl_DrawModel(LBXGL_ModelState *ctx)
{
	float tmat[16];
	float dist, lod;
	int i, j;
	float f, alpha, scale;

	if(!ctx->mdl && ctx->modelname)
		{ LBXGL_Mdl_SetModelName(ctx, ctx->modelname); }

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_draw)
		{ ctx->mvt->drawmdl_draw(ctx); return; }

	LBXGL_Mdl_SaveRelPosition(ctx);
	lbxgl_mdl_curmdl=ctx;

	dist=V3F_DIST(lbxgl_cam->org, ctx->org);
//	lod=LBXGL_Mdl_LodForDistance(dist);
	lod=LBXGL_Mdl_LodForModelDistance(ctx, dist);

	glDisable(GL_CULL_FACE);
	
	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

	scale=LBXGL_Mdl_CalcModelScale(ctx);

#if 0
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];

//	tmat[0]=1;
//	tmat[5]=1;
//	tmat[10]=1;

	tmat[12]=ctx->org[0];
	tmat[13]=ctx->org[1];
	tmat[14]=ctx->org[2];
	tmat[15]=1;
#endif

	alpha=1.0;
	if(ctx->flags&LBXGL_TXFL_VALPHA)
	{
//		i=ctx->flags&LBXGL_TXFL_FLUID_MASK;
//		alpha=i/((float)LBXGL_TXFL_FLUID_MASK);

		i=ctx->flags&LBXGL_TXFL_ALPHA_MASK;
		alpha=i/((float)LBXGL_TXFL_ALPHA_MASK);
		LBXGL_Shader_Color4f(1, 1, 1, alpha);
	}

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_Mesh_DrawGroup(ctx->mdl);
//		LBXGL_Mesh_DrawGroupFinal(ctx->mdl);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}
	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
	{
		LBXGL_Shader_PushMultMatrix(tmat);
		Skel_DrawSetLOD(ctx->state, lod);
		LBXGL_Mesh_DrawPrim(ctx->mdl);
//		LBXGL_Mesh_DrawPrimFinal(ctx->mdl);
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glDisable(GL_CULL_FACE); }

//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		lbxgl_mdl_lock();
		Skel_DrawSetLOD(ctx->state, lod);
//		Skel_DrawModel(ctx->state);
		Skel_DrawModelAlpha(ctx->state, alpha);
//		glPopMatrix();
		lbxgl_mdl_unlock();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;

		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glEnable(GL_CULL_FACE); }
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_QuakeMDL_RenderState(ctx);
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModel(ctx->mdl);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_lensflare_t"))
	{
		LBXGL_Sprite_RenderFlare(ctx->mdl, ctx->org, 100);
		lbxgl_mdl_curmdl=NULL;
		return;
	}
	if(dytypep(ctx->mdl, "lbxgl_sprite_t"))
	{
//		LBXGL_Sprite_RenderState(ctx->state, ctx->org);
		LBXGL_Sprite_RenderStateScale(ctx->state, ctx->org, scale);
		lbxgl_mdl_curmdl=NULL;
		return;
	}
	
	lbxgl_mdl_curmdl=NULL;
}

LBXGL_API void LBXGL_Mdl_DrawModelAlpha(LBXGL_ModelState *ctx)
{
	float tmat[16];
	float dist, lod;
	int i, j;
	float f, alpha;

	if(LBXGL_Mdl_AlphaModelP(ctx))
	{
		LBXGL_Mdl_DrawModel(ctx);
		return;
	}

	if(!ctx->mdl && ctx->modelname)
		{ LBXGL_Mdl_SetModelName(ctx, ctx->modelname); }

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_draw)
		{ ctx->mvt->drawmdl_draw(ctx); return; }

	LBXGL_Mdl_SaveRelPosition(ctx);
	lbxgl_mdl_curmdl=ctx;

	dist=V3F_DIST(lbxgl_cam->org, ctx->org);
//	lod=LBXGL_Mdl_LodForDistance(dist);
	lod=LBXGL_Mdl_LodForModelDistance(ctx, dist);

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

#if 0
//	glDisable(GL_CULL_FACE);
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];

	tmat[12]=ctx->org[0];
	tmat[13]=ctx->org[1];
	tmat[14]=ctx->org[2];
	tmat[15]=1;
#endif


	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModelAlpha(ctx->mdl);
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
//		LBXGL_Mesh_DrawGroup(ctx->mdl);
		LBXGL_Mesh_DrawGroupAlpha(ctx->mdl);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}
	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
	{
		LBXGL_Shader_PushMultMatrix(tmat);
		Skel_DrawSetLOD(ctx->state, lod);
//		LBXGL_Mesh_DrawPrim(ctx->mdl);
		LBXGL_Mesh_DrawPrimAlpha(ctx->mdl);
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	lbxgl_mdl_curmdl=NULL;
}

LBXGL_API void LBXGL_Mdl_DrawModelDark(LBXGL_ModelState *ctx)
{
	float tmat[16];
	float dist, lod;
	int i, j;

//	printf("LBXGL_Mdl_DrawModelDark: Draw model type '%s'\n",
//		dygettype(ctx->mdl));

	if(!ctx->mdl && ctx->modelname)
		{ LBXGL_Mdl_SetModelName(ctx, ctx->modelname); }

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_dark)
		{ ctx->mvt->drawmdl_dark(ctx); return; }

	LBXGL_Mdl_SaveRelPosition(ctx);
	lbxgl_mdl_curmdl=ctx;

	dist=V3F_DIST(lbxgl_cam->org, ctx->org);
//	lod=LBXGL_Mdl_LodForDistance(dist);
	lod=LBXGL_Mdl_LodForModelDistance(ctx, dist);

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

#if 0
//	glDisable(GL_CULL_FACE);
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];
	tmat[12]=ctx->org[0]; tmat[13]=ctx->org[1]; tmat[14]=ctx->org[2]; tmat[15]=1;
#endif

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_Mesh_DrawGroupDark(ctx->mdl);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
	{
		pdglColor4f(0, 0, 0, 1);
		LBXGL_Mesh_DrawPrimFlat(ctx->mdl);
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glDisable(GL_CULL_FACE); }

//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		lbxgl_mdl_lock();
		Skel_DrawSetLOD(ctx->state, lod);
		Skel_DrawModelDark(ctx->state);
		lbxgl_mdl_unlock();
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;

		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glEnable(GL_CULL_FACE); }
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModelDark(ctx->mdl);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_QuakeMDL_RenderStateDark(ctx);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_lensflare_t"))
		return;
	if(dytypep(ctx->mdl, "lbxgl_sprite_t"))
		return;

	printf("LBXGL_Mdl_DrawModelDark: Unable to draw model type '%s'\n",
		dygettype(ctx->mdl));
	lbxgl_mdl_curmdl=NULL;
}

LBXGL_API void LBXGL_Mdl_DrawModelLight(LBXGL_ModelState *ctx,
	LBXGL_Light *light)
{
	float tmat[16], lorg[3], tv[3];
	float rad, gam;
	int i, j;

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_light)
		{ ctx->mvt->drawmdl_light(ctx); return; }

	lbxgl_mdl_curmdl=ctx;

#if 0
//	glDisable(GL_CULL_FACE);
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];
	tmat[12]=ctx->org[0]; tmat[13]=ctx->org[1]; tmat[14]=ctx->org[2]; tmat[15]=1;
#endif

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

	V3F_SUB(light->org, ctx->org, tv);
	lorg[0]=V3F_DOT(tv, ctx->rot+0);
	lorg[1]=V3F_DOT(tv, ctx->rot+3);
	lorg[2]=V3F_DOT(tv, ctx->rot+6);
	rad=LBXGL_Shadow_LightGammaRadius(light);

	if(light->flags&LBXGL_LFL_SUN)
		{ gam=LBXGL_Voxel_GetPointSunGamma(lbxgl_world, ctx->org); }
	else { gam=1; }

//	gam=1;

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_Mesh_DrawGroupLight(ctx->mdl);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
	{
//		glColor4f(1, 1, 1, 1);
		pdglColor4f(gam, gam, gam, 1);
		LBXGL_Mesh_DrawPrimFlat(ctx->mdl);
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glDisable(GL_CULL_FACE); }
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
//		Skel_DrawModelLight(ctx->state);
		lbxgl_mdl_lock();
		LBXGL_Shader_Color4f(gam, gam, gam, 1);
		Skel_DrawModelLightColor(ctx->state, gam, gam, gam);
		lbxgl_mdl_unlock();
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		lbxgl_mdl_curmdl=NULL;
		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glEnable(GL_CULL_FACE); }
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModelLight(ctx->mdl, light, lorg, rad);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_QuakeMDL_RenderStateLight(ctx);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_lensflare_t"))
		return;
	if(dytypep(ctx->mdl, "lbxgl_sprite_t"))
		return;

	printf("LBXGL_Mdl_DrawModelLight: Unable to draw model type '%s'\n",
		dygettype(ctx->mdl));
	lbxgl_mdl_curmdl=NULL;
}

//LBXGL_API void LBXGL_Mdl_DrawModelShadow(LBXGL_ModelState *ctx,
//	float *org, float rad)
LBXGL_API void LBXGL_Mdl_DrawModelShadow(LBXGL_ModelState *ctx,
	LBXGL_Light *light, float *org, float rad)
{
	float tmat[16], lorg[3], tv[3];
	LBXGL_RenderMethods *mth;
	float f;
	int i, j;

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_shadow)
		{ ctx->mvt->drawmdl_shadow(ctx, org, rad); return; }

//	lbxgl_mdl_curmdl=ctx;
	lbxgl_mdl_curmdl=NULL;

#if 0
//	glDisable(GL_CULL_FACE);
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];
	tmat[12]=ctx->org[0]; tmat[13]=ctx->org[1]; tmat[14]=ctx->org[2]; tmat[15]=1;
#endif

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);


	V3F_SUB(org, ctx->org, tv);
	lorg[0]=V3F_DOT(tv, ctx->rot+0);
	lorg[1]=V3F_DOT(tv, ctx->rot+3);
	lorg[2]=V3F_DOT(tv, ctx->rot+6);

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
//		i=Skel_DrawGetLOD(ctx->state);
//		if(i>0)return;
//		if(i>1)return;

		f=V3F_LEN(ctx->maxs);
		i=LBXGL_Brush_CheckViewSphereShadowFlags(
			ctx->org, f, org, 2*rad, 0);
//		if(i)return;

//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_Mesh_DrawGroupShadowVolume(ctx->mdl, lorg, rad, i);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		return;
	}

//	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
//		{ LBXGL_Mesh_DrawGroupShadowVolume(ctx->mdl, lorg); return; }
	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
	{
//		i=Skel_DrawGetLOD(ctx->state);
//		if(i>0)return;
//		if(i>1)return;

		f=V3F_LEN(ctx->maxs);
		i=LBXGL_Brush_CheckViewSphereShadow(ctx->org, f, org, 2*rad);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_Mesh_DrawPrimShadowVolume(ctx->mdl, lorg, rad, 0);
		LBXGL_Shader_PopMatrix();
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		if(ctx->effects2&BT_EF2_VIEWMODEL)
			return;

		j=LBXGL_Brush_CheckViewSphereShadowFlags(
			ctx->org, 64, org, 2*rad, 0);
//		if(j)return;
	
		i=Skel_DrawGetLOD(ctx->state);
//		if(i>0)return;
		if(i>1)return;
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		lbxgl_mdl_lock();
//		Skel_DrawModelShadow(ctx->state, lorg, rad);
		Skel_DrawModelShadowFlags(ctx->state, lorg, rad, j);
		lbxgl_mdl_unlock();
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModelShadow(ctx->mdl, lorg, rad);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
		i=LBXGL_Brush_CheckViewSphereShadow(ctx->org, 64, org, rad);
		if(i)return;

//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_QuakeMDL_RenderStateShadow(ctx, lorg, rad);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_lensflare_t"))
		return;
	if(dytypep(ctx->mdl, "lbxgl_sprite_t"))
		return;
}

LBXGL_API void LBXGL_Mdl_DrawModelFinal(LBXGL_ModelState *ctx)
{
	float tmat[16];
	int i, j;

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_final)
		{ ctx->mvt->drawmdl_final(ctx); return; }

	lbxgl_mdl_curmdl=ctx;

#if 0
//	glDisable(GL_CULL_FACE);
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];
	tmat[12]=ctx->org[0]; tmat[13]=ctx->org[1]; tmat[14]=ctx->org[2]; tmat[15]=1;
#endif

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		glEnable(GL_CULL_FACE);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Mesh_DrawGroupFinal(ctx->mdl);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

//	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
//		{ LBXGL_Mesh_DrawGroup(ctx->mdl); return; }
	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
		{ LBXGL_Mesh_DrawPrim(ctx->mdl); return; }

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glDisable(GL_CULL_FACE); }

//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		lbxgl_mdl_lock();
		Skel_DrawModelFinal(ctx->state);
		lbxgl_mdl_unlock();
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;

		if(ctx->effects2&BT_EF2_VIEWMODEL)
			{ glEnable(GL_CULL_FACE); }
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModelFinal(ctx->mdl);
//		glPopMatrix();
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_QuakeMDL_RenderStateFinal(ctx);
		LBXGL_Shader_PopMatrix();
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_lensflare_t"))
	{
		LBXGL_Sprite_RenderFlare(ctx->mdl, ctx->org, 100);
		lbxgl_mdl_curmdl=NULL;
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_sprite_t"))
	{
		LBXGL_Sprite_RenderState(ctx->mdl, ctx->org);
		lbxgl_mdl_curmdl=NULL;
		return;
	}
}


LBXGL_API void LBXGL_Mdl_DrawModelFlat(LBXGL_ModelState *ctx)
{
	float tmat[16];
	LBXGL_RenderMethods *mth;
	float dist, lod;
	int i, j;

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_flat)
		{ ctx->mvt->drawmdl_flat(ctx); return; }

	dist=V3F_DIST(lbxgl_cam->org, ctx->org);
//	lod=LBXGL_Mdl_LodForDistance(dist);
	lod=LBXGL_Mdl_LodForModelDistance(ctx, dist);

//	lbxgl_mdl_curmdl=ctx;
	lbxgl_mdl_curmdl=NULL;

#if 0
//	glDisable(GL_CULL_FACE);
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];

	tmat[12]=ctx->org[0];
	tmat[13]=ctx->org[1];
	tmat[14]=ctx->org[2];
	tmat[15]=1;
#endif

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_Mesh_DrawGroupFlat(ctx->mdl);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		return;
	}
	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
		{ LBXGL_Mesh_DrawPrimFlat(ctx->mdl); return; }

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		lbxgl_mdl_lock();
		Skel_DrawSetLOD(ctx->state, lod);
		Skel_DrawModelFlat(ctx->state);
		lbxgl_mdl_unlock();
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		return;
	}

//	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
//		LBXGL_QuakeMDL_RenderState(ctx);

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModelFlat(ctx->mdl);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_QuakeMDL_RenderStateFlat(ctx);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		return;
	}
}

LBXGL_API void LBXGL_Mdl_DrawModelFlatApprox(LBXGL_ModelState *ctx)
{
	float tmat[16];
	float tv0[4], tv1[4];
	int i, j;

	if(!ctx->mdl && ctx->modelname)
		{ LBXGL_Mdl_SetModelName(ctx, ctx->modelname); }

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_flat)
		{ ctx->mvt->drawmdl_flat(ctx); return; }

#if 0
//	glDisable(GL_CULL_FACE);
	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[i*4+j]=ctx->rot[i*3+j];

	tmat[12]=ctx->org[0];
	tmat[13]=ctx->org[1];
	tmat[14]=ctx->org[2];
	tmat[15]=1;
#endif

	LBXGL_Mdl_CalcModelTransform(ctx, tmat);

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
		LBXGL_Mdl_BoxModel(ctx, tv0, tv1);
		PD3D_DrawFlatCube2(
			tv0[0], tv0[1], tv0[2], tv1[0], tv1[1], tv1[2]);
		return;
	}
	if(dytypep(ctx->mdl, "lbxgl_meshprim_t"))
	{
		LBXGL_Mdl_BoxModel(ctx, tv0, tv1);
		PD3D_DrawFlatCube2(
			tv0[0], tv0[1], tv0[2], tv1[0], tv1[1], tv1[2]);
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
//		Skel_DrawSetLOD(ctx->state, lod);
		Skel_DrawModelFlatApprox(ctx->state);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		return;

//		LBXGL_Mdl_BoxModel(ctx, tv0, tv1);
//		PD3D_DrawFlatCube2(
//			tv0[0], tv0[1], tv0[2], tv1[0], tv1[1], tv1[2]);
//		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
		LBXGL_Mdl_BoxModel(ctx, tv0, tv1);
		PD3D_DrawFlatCube2(
			tv0[0], tv0[1], tv0[2], tv1[0], tv1[1], tv1[2]);
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
//		glMatrixMode(GL_MODELVIEW_MATRIX);
//		glPushMatrix(); glMultMatrixf(tmat);
		LBXGL_Shader_PushMultMatrix(tmat);
		LBXGL_BrushModel_DrawBrushModelFlat(ctx->mdl);
		LBXGL_Shader_PopMatrix();
//		glPopMatrix();
		return;
	}
}

LBXGL_API void LBXGL_Mdl_ObbPlaneExtents(float *mins, float *maxs,
	float *org, float *rot, float *norm, float *rmin, float *rmax)
{
	float w[3];
	float m, n, d;
	int i;

	w[0]=V3F_DOT(rot+0, norm);
	w[1]=V3F_DOT(rot+3, norm);
	w[2]=V3F_DOT(rot+6, norm);
	d=V3F_NDOT(org, norm);

	m=0; n=0;
	for(i=0; i<3; i++)
	{
		if(w[i]>0) { m+=mins[i]*w[i]; n+=maxs[i]*w[i]; }
			else { m+=maxs[i]*w[i]; n+=mins[i]*w[i]; }
	}
	*rmin=m+d; *rmax=n+d;
}

LBXGL_API void LBXGL_Mdl_BoxObb(float *mins, float *maxs, float *org, float *rot,
	float *rmins, float *rmaxs)
{
	float norm[4];

	V4F_SET(norm, 1, 0, 0, 0);
	LBXGL_Mdl_ObbPlaneExtents(mins, maxs, org, rot,
		norm, rmins+0, rmaxs+0);
	V4F_SET(norm, 0, 1, 0, 0);
	LBXGL_Mdl_ObbPlaneExtents(mins, maxs, org, rot,
		norm, rmins+1, rmaxs+1);
	V4F_SET(norm, 0, 0, 1, 0);
	LBXGL_Mdl_ObbPlaneExtents(mins, maxs, org, rot,
		norm, rmins+2, rmaxs+2);
}

LBXGL_API void LBXGL_Mdl_BoxModel(LBXGL_ModelState *ctx, float *mins, float *maxs)
{
	float tmins[3], tmaxs[3];

	if(!ctx->mdl && ctx->modelname)
		{ LBXGL_Mdl_SetModelName(ctx, ctx->modelname); }

	if(!ctx->mdl)
		return;

	if(ctx->mvt && ctx->mvt->drawmdl_getBox)
		{ ctx->mvt->drawmdl_getBox(ctx, mins, maxs); return; }

	if(dytypep(ctx->mdl, "lbxgl_meshgroup_t"))
	{
		V3F_SET(tmins,  999999,  999999,  999999);
		V3F_SET(tmaxs, -999999, -999999, -999999);
		LBXGL_Mesh_BoxGroup(ctx->mdl, tmins, tmaxs);
		LBXGL_Mdl_BoxObb(tmins, tmaxs, ctx->org, ctx->rot,
			mins, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_skel2model_t"))
	{
//		V3F_SET(tmins, -64, -64, -64);
//		V3F_SET(tmaxs,  64,  64,  64);

//		V3F_ADD(tmins, ctx->org, mins);
//		V3F_ADD(tmaxs, ctx->org, maxs);

//		V3F_SUB(mins, ctx->org, ctx->mins);
//		V3F_SUB(maxs, ctx->org, ctx->maxs);

		V3F_SET(tmins,  999999,  999999,  999999);
		V3F_SET(tmaxs, -999999, -999999, -999999);
		Skel_BoxModelApprox(ctx->state, tmins, tmaxs);

		LBXGL_Mdl_BoxObb(tmins, tmaxs, ctx->org, ctx->rot,
			mins, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);

		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_brushmodel_t"))
	{
		LBXGL_BrushModel_BoxModel(ctx->mdl, tmins, tmaxs);
		V3F_ADD(tmins, ctx->org, mins);
		V3F_ADD(tmaxs, ctx->org, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_quakemdl_t"))
	{
		V3F_SET(tmins, -64, -64, -64);
		V3F_SET(tmaxs,  64,  64,  64);

		V3F_ADD(tmins, ctx->org, mins);
		V3F_ADD(tmaxs, ctx->org, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}

	if(dytypep(ctx->mdl, "lbxgl_sprite_t"))
	{
		LBXGL_Sprite_BoxState(ctx->mdl, tmins, tmaxs);
		V3F_ADD(tmins, ctx->org, mins);
		V3F_ADD(tmaxs, ctx->org, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}
}

LBXGL_API int LBXGL_Mdl_CheckModelLight(
	LBXGL_ModelState *ctx, LBXGL_Light *light)
{
	float org[3], amins[3], amaxs[3];
	float f, g;
	int i;

	if(light->flags&LBXGL_LFL_SCENERY)
	{
		if(!(ctx->effects2&BT_EF2_SCENERY))
			return(0);
	}

	V3F_ADD(ctx->org, ctx->mins, amins);
	V3F_ADD(ctx->org, ctx->maxs, amaxs);
	if(!SolidAABB_BoxCollideP(
			amins, amaxs, light->amins, light->amaxs))
		return(0);

	g=0;
	for(i=0; i<3; i++)
	{
		org[i]=(ctx->mins[i]+ctx->maxs[i])*0.5;
		f=fabs(ctx->mins[i]-org[i]); if(f>g)g=f;
		f=fabs(ctx->maxs[i]-org[i]); if(f>g)g=f;
	}

	V3F_ADD(org, ctx->org, org);

	f=LBXGL_Shadow_LightGammaPoint(light, org, g);

	if(f<0.05)return(0);
	return(1);
}


LBXGL_API BTGE_BrushDrawFace *LBXGL_Mdl_QueryDrawFaces(
	LBXGL_ModelState *ctx)
{
	if(!ctx->mdl)
		return(NULL);

	if(ctx->mvt && ctx->mvt->drawmdl_queryDrawFaces)
		{ return(ctx->mvt->drawmdl_queryDrawFaces(ctx)); }

	return(NULL);
}
