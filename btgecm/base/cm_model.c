#include <btgecm.h>

char *btge_mdl_name[4096];
void *btge_mdl_obj[4096];
int btge_mdl_num=0;

BTGE_ModelState *btge_mdl_curmdl=NULL;

// extern PDGLUI_InputState *btge_state;
// extern PDGLUI_Camera *btge_cam;

BTGE_API void *BTGE_Mdl_LoadModel(char *name)
{
	char tb[256];
	char *ext;
	void *p;
	int i;

	for(i=0; i<btge_mdl_num; i++)
		if(!strcmp(btge_mdl_name[i], name))
			return(btge_mdl_obj[i]);

	ext=vfgetext(name);

//	printf("BTGE_Mdl_LoadModel: load %s\n", name);

	if(!ext)
	{
		sprintf(tb, "models/%s.ac", name);
		p=BTGE_Mdl_LoadModel(tb);

		if(!p)
		{
			sprintf(tb, "models/%s.model", name);
			p=BTGE_Mdl_LoadModel(tb);
		}

		if(!p)
		{
			sprintf(tb, "models/%s.mdl", name);
			p=BTGE_Mdl_LoadModel(tb);
		}

		if(!p)
		{
			sprintf(tb, "models/%s.md2", name);
			p=BTGE_Mdl_LoadModel(tb);
		}

		if(!p)
		{
			sprintf(tb, "models/%s.md3", name);
			p=BTGE_Mdl_LoadModel(tb);
		}

//		if(!p)
//		{
//			sprintf(tb, "sprites/%s_sprinfo.txt", name);
//			p=BTGE_Sprite_LoadSprites(tb);
//		}

//		if(!p)p=BTGE_Sprite_LookupFlare(name);
//		if(!p)p=BTGE_Frac_GenModel(name);

		if(!p)p=BTGE_BrushPrefab_GetBModelName(name);

		if(p)
		{
			i=btge_mdl_num++;
			btge_mdl_name[i]=dystrdup(name);
			btge_mdl_obj[i]=p;
			return(p);
		}

		printf("BTGE_Mdl_LoadModel: fail load %s\n", name);

		return(NULL);
	}

	p=NULL;

	if(!strcmp(ext, "ac"))p=BTGE_AC3D_LoadModel(name);
//	if(!strcmp(ext, "model"))p=Skel_LoadModel(name);

//	if(!strcmp(ext, "mdl"))p=BTGE_QuakeMDL_LoadModel(name);
//	if(!strcmp(ext, "md2"))p=BTGE_QuakeMDL_LoadModel(name);
//	if(!strcmp(ext, "md3"))p=BTGE_QuakeMDL_LoadModel(name);

	if(!p)p=BTGE_BrushPrefab_GetBModelName(name);

	if(p)
	{
		i=btge_mdl_num++;
		btge_mdl_name[i]=dystrdup(name);
		btge_mdl_obj[i]=p;
		return(p);
	}

	printf("BTGE_Mdl_LoadModel: fail load %s\n", name);

	return(NULL);
}

BTGE_API BTGE_ModelState *BTGE_Mdl_LoadModelInstance(char *name)
{
	BTGE_ModelState *tmp;
	void *p;

	p=BTGE_Mdl_LoadModel(name);
	if(!p)return(NULL);

	tmp=BTGE_Mdl_WrapModelInstance(p);
	return(tmp);
}

BTGE_API BTGE_ModelState *BTGE_Mdl_WrapModelInstance(void *mdl)
{
	BTGE_ModelState *tmp;

	if(!mdl)return(NULL);

	tmp=gctalloc("btge_modelstate_t", sizeof(BTGE_ModelState));
	tmp->mdl=mdl;
	tmp->mdltype=dygettype(mdl);
//	tmp->mvt=BTGE_Method_GetRenderMethods(tmp->mdltype);

//	if(dytypep(mdl, "btge_skel2model_t"))
//		tmp->state=Skel_NewInstance(mdl);
//	if(dytypep(mdl, "btge_sprite_t"))
//		tmp->state=BTGE_Sprite_NewStateInstance(mdl);

	V3F_ZERO(tmp->org);
	V3F_SET(tmp->rot+0, 1, 0, 0);
	V3F_SET(tmp->rot+3, 0, 1, 0);
	V3F_SET(tmp->rot+6, 0, 0, 1);

	return(tmp);
}

BTGE_API void BTGE_Mdl_SetModel(BTGE_ModelState *ctx, void *mdl)
{
//	if(dytypep(ctx->mdl, "btge_skel2model_t"))
//		Skel_FreeInstance(ctx->state);

	ctx->mdl=mdl;
	ctx->mdltype=dygettype(mdl);
//	ctx->mvt=BTGE_Method_GetRenderMethods(ctx->mdltype);

//	if(dytypep(mdl, "btge_skel2model_t"))
//		ctx->state=Skel_NewInstance(mdl);
//	if(dytypep(mdl, "btge_sprite_t"))
//		ctx->state=BTGE_Sprite_NewStateInstance(mdl);
}

BTGE_API void BTGE_Mdl_FreeModel(BTGE_ModelState *ctx)
{
//	if(dytypep(ctx->mdl, "btge_skel2model_t"))
//		Skel_FreeInstance(ctx->state);
	gcfree(ctx);
}

BTGE_API BTGE_ModelState *BTGE_Mdl_GetCurrentModel(void)
	{ return(btge_mdl_curmdl); }

BTGE_API void BTGE_Mdl_SetAnim(BTGE_ModelState *ctx, char *name)
{
//	if(dytypep(ctx->mdl, "btge_skel2model_t"))
//		Skel_SetAnim(ctx->state, name);
}

BTGE_API void BTGE_Mdl_SetAnimFrame(BTGE_ModelState *ctx,
	char *name, int frame)
{
//	if(dytypep(ctx->mdl, "btge_skel2model_t"))
//		Skel_SetAnimFrame(ctx->state, name, frame);
}

BTGE_API void BTGE_Mdl_SetFrame(BTGE_ModelState *ctx, int frame)
{
//	if(dytypep(ctx->mdl, "btge_skel2model_t"))
//		Skel_SetFrame(ctx->state, frame);
//	if(dytypep(ctx->mdl, "btge_sprite_t"))
//		BTGE_Sprite_SetFrame(ctx->state, frame);
}

BTGE_API void BTGE_Mdl_StepTime(BTGE_ModelState *ctx, float dt)
{
//	if(dytypep(ctx->mdl, "btge_skel2model_t"))
//		Skel_StepTime(ctx->state, dt);
}

BTGE_API float BTGE_Mdl_LodForDistance(float d)
{
	float f;
	if(d>250)f=1;
	if(d>500)f=2;
	if(d>=750)f=sqrt(d/100);
	return(f);
}

BTGE_API int BTGE_Mdl_AlphaModelP(BTGE_ModelState *ctx)
{
	char *s;
	
	s=ctx->mdltype;
	if(!s)s=dygettype(ctx->mdl);
	if(!strcmp(s, "btge_lensflare_t"))return(1);
	if(!strcmp(s, "btge_sprite_t"))return(1);

//	if(!strcmp(s, "btge_quakemdl_t"))
//		return(1);
	
//	if(ctx->mvt)
//	{
//		if(ctx->mvt->drawmdl_draw && !(ctx->mvt->drawmdl_dark))
//			return(1);
//	}

	if(ctx->flags&(BTGE_TXFL_ALPHA|BTGE_TXFL_VALPHA))
		return(1);

	return(0);
}

BTGE_API void BTGE_Mdl_CalcModelTransform(
	BTGE_ModelState *ctx, float *tmat)
{
	int i, j;

	for(i=0; i<16; i++)tmat[i]=0;
//	for(i=0; i<3; i++)for(j=0; j<3; j++)tmat[j*4+i]=ctx->rot[i*3+j];
	for(i=0; i<3; i++)for(j=0; j<3; j++)
		tmat[i*4+j]=ctx->rot[i*3+j];

//	tmat[0]=1;
//	tmat[5]=1;
//	tmat[10]=1;

	tmat[12]=ctx->org[0];
	tmat[13]=ctx->org[1];
	tmat[14]=ctx->org[2];
	tmat[15]=1;
}

#if 0
// BTGE_API void BTGE_Mdl_CalcModelDelayIndexTransform(
	BTGE_ModelState *ctx, float *tmat, int idx)
{
	QuatF_ToMatrix(ctx->lr_rot+idx*4, tmat);
	tmat[12]=ctx->lr_org[idx*3+0];
	tmat[13]=ctx->lr_org[idx*3+1];
	tmat[14]=ctx->lr_org[idx*3+2];
}

// BTGE_API void BTGE_Mdl_CalcModelDelayTransform(
	BTGE_ModelState *ctx, float *tmat, float delay)
{
	float tm0[16], tm1[16];
	float t, dt, dtq, dtr;
	float f, g;
	int i;

	t=btge_state->time_f;
	dt=t-ctx->lr_time[0];
	if(dt<0)dt=0;
	
	//small delay, interpolate between current and last-saved position
	if(delay<=dt)
	{
		if(dt<0.01)
		{
			BTGE_Mdl_CalcModelTransform(ctx, tmat);
			return;
		}
	
		BTGE_Mdl_CalcModelTransform(ctx, tm0);
		BTGE_Mdl_CalcModelDelayIndexTransform(ctx, tm1, 0);
		f=delay/dt;
		Mat4F_InterpolateTransform(tm0, tm1, tmat, f);
		return;
	}
	
	dtq=(delay-dt)/BTGE_MDL_SAVEREL_DT;
	i=(int)dtq;
	dtr=dtq-i;
	
	//delay exceeds window, clamp to window
	if(i>=(BTGE_MDL_SAVEREL_NUM-1))
	{
		BTGE_Mdl_CalcModelDelayIndexTransform(
			ctx, tmat, BTGE_MDL_SAVEREL_NUM-1);
		return;
	}

	BTGE_Mdl_CalcModelDelayIndexTransform(ctx, tm0, i);
	BTGE_Mdl_CalcModelDelayIndexTransform(ctx, tm1, i+1);
	Mat4F_InterpolateTransform(tm0, tm1, tmat, dtr);
}

// BTGE_API void BTGE_Mdl_CalcModelDelayRelTransform(
	BTGE_ModelState *ctx, float *tmat, float delay)
{
	float tm0[16], tm1[16], tm2[16];
	
	BTGE_Mdl_CalcModelDelayTransform(ctx, tm0, delay);
	BTGE_Mdl_CalcModelTransform(ctx, tm1);
	Mat4F_InverseTransform(tm1, tm2);
	Mat4F_MatMult(tm0, tm2, tmat);
}

// BTGE_API void BTGE_Mdl_SaveRelPosition(BTGE_ModelState *ctx)
{
	float t;
	int i, j;
	
	t=btge_state->time_f;
	if(t<(ctx->lr_time[0]+BTGE_MDL_SAVEREL_DT))
		return;	//too soon
		
	for(i=BTGE_MDL_SAVEREL_NUM-1; i>0; i--)
	{
		V3F_COPY(ctx->lr_org+(i-1)*3, ctx->lr_org+i*3);
		V4F_COPY(ctx->lr_rot+(i-1)*4, ctx->lr_rot+i*4);
		ctx->lr_time[i]=ctx->lr_time[i-1];
	}
	
	V3F_COPY(ctx->org, ctx->lr_org);
	QuatF_From3Matrix(ctx->rot, ctx->lr_rot);
	ctx->lr_time[0]=t;
}
#endif

BTGE_API void BTGE_Mdl_ObbPlaneExtents(float *mins, float *maxs,
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

BTGE_API void BTGE_Mdl_BoxObb(float *mins, float *maxs, float *org, float *rot,
	float *rmins, float *rmaxs)
{
	float norm[4];

	V4F_SET(norm, 1, 0, 0, 0);
	BTGE_Mdl_ObbPlaneExtents(mins, maxs, org, rot,
		norm, rmins+0, rmaxs+0);
	V4F_SET(norm, 0, 1, 0, 0);
	BTGE_Mdl_ObbPlaneExtents(mins, maxs, org, rot,
		norm, rmins+1, rmaxs+1);
	V4F_SET(norm, 0, 0, 1, 0);
	BTGE_Mdl_ObbPlaneExtents(mins, maxs, org, rot,
		norm, rmins+2, rmaxs+2);
}

BTGE_API void BTGE_Mdl_BoxModel(BTGE_ModelState *ctx, float *mins, float *maxs)
{
	float tmins[3], tmaxs[3];

	if(!ctx->mdl)
		return;

//	if(ctx->mvt && ctx->mvt->drawmdl_getBox)
//		{ ctx->mvt->drawmdl_getBox(ctx, mins, maxs); return; }

	if(dytypep(ctx->mdl, "btge_meshgroup_t"))
	{
		V3F_SET(tmins,  999999,  999999,  999999);
		V3F_SET(tmaxs, -999999, -999999, -999999);
		BTGE_Mesh_BoxGroup(ctx->mdl, tmins, tmaxs);
		BTGE_Mdl_BoxObb(tmins, tmaxs, ctx->org, ctx->rot,
			mins, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}

	if(dytypep(ctx->mdl, "btge_skel2model_t"))
	{
		V3F_SET(tmins, -64, -64, -64);
		V3F_SET(tmaxs,  64,  64,  64);

		V3F_ADD(tmins, ctx->org, mins);
		V3F_ADD(tmaxs, ctx->org, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}

	if(dytypep(ctx->mdl, "btge_brushmodel_t"))
	{
		BTGE_BrushModel_BoxModel(ctx->mdl, tmins, tmaxs);
		V3F_ADD(tmins, ctx->org, mins);
		V3F_ADD(tmaxs, ctx->org, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}

	if(dytypep(ctx->mdl, "btge_quakemdl_t"))
	{
		V3F_SET(tmins, -64, -64, -64);
		V3F_SET(tmaxs,  64,  64,  64);

		V3F_ADD(tmins, ctx->org, mins);
		V3F_ADD(tmaxs, ctx->org, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}

	if(dytypep(ctx->mdl, "btge_sprite_t"))
	{
		V3F_SET(tmins, -64, -64, -64);
		V3F_SET(tmaxs,  64,  64,  64);

//		BTGE_Sprite_BoxState(ctx->mdl, tmins, tmaxs);
		V3F_ADD(tmins, ctx->org, mins);
		V3F_ADD(tmaxs, ctx->org, maxs);

		V3F_SUB(mins, ctx->org, ctx->mins);
		V3F_SUB(maxs, ctx->org, ctx->maxs);
		return;
	}
}

BTGE_API int BTGE_Mdl_CheckModelLight(
	BTGE_ModelState *ctx, BTGE_Light *light)
{
	float org[3];
	float f, g;
	int i;

	g=0;
	for(i=0; i<3; i++)
	{
		org[i]=(ctx->mins[i]+ctx->maxs[i])*0.5;
		f=fabs(ctx->mins[i]-org[i]); if(f>g)g=f;
		f=fabs(ctx->maxs[i]-org[i]); if(f>g)g=f;
	}

	V3F_ADD(org, ctx->org, org);

//	f=BTGE_Shadow_LightGammaPoint(light, org, g);
	f=light->val[0];

	if(f<0.05)return(0);
	return(1);
}
