#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

extern int shader_nophong;
extern int shader_nolightmat;

#if 0
LBXGL_API void LBXGL_Brush_DrawBrushQuad3_Shader(LBXGL_Brush *brush)
{
	float txyz[256];
	float tst[256];
	float tnorm[256];
	float lorg[3];
	float *xyz, *st, *norm;
	short *vec;
	int tex;
	int i, j, k, l;

//	return;

	LBXGL_Shader_TransformPointLocal(light->org, lorg);

	xyz=brush->xyz; st=brush->st; vec=brush->vec; k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=16; continue; }

		tex=brush->pf_tex[i];

		norm=brush->norm+brush->pf_plane[i]*4;
		if(V3F_NDOT(lorg, norm)<=0)
			{ vec+=16; continue; }

		for(j=0; j<4; j++)
		{
			xyz=brush->xyz+(vec[0]*3);
			st=brush->st+(vec[1]*2);
			V3F_COPY(xyz, txyz+(k*3));
			V2F_COPY(st, tst+(k*2));
			V4F_COPY(norm, tnorm+(k*4));
			vec+=4; k++;
		}
	}

	if(!k)return;

	LBXGL_BrushLight_DrawPrimLight(light, PDGL_QUADS,
		txyz, tst, tnorm, k, tex);
	LBXGL_BrushLight_DrawPrimLight_Reset(light);
}
#endif

LBXGL_API void LBXGL_Brush_DrawFacesQuads(LBXGL_Brush *brush)
{
	int i, j, k, l;

	LBXGL_Shader_BindTexture(brush->pf_tex[0]);

	LBXGL_Shader_Begin(PDGL_QUADS);

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
//		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
//			continue;

//		pdglBindTexture(GL_TEXTURE_2D, brush->texnum[i]);
//		LBXGL_Shader_BindTexture(brush->pf_tex[i]);

		LBXGL_Shader_Normal3fv(brush->norm+brush->pf_plane[i]*4);
//		LBXGL_Shader_Color4f(r, g, b, 1);
//		LBXGL_Shader_BeginPoly();
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			k=brush->pf_ivec[i]+j;
			LBXGL_Shader_TexCoord2fv(
				brush->st+brush->vec[k*4+1]*2);
			LBXGL_Shader_Vertex3fv(
				brush->xyz+brush->vec[k*4+0]*3);
		}
//		LBXGL_Shader_EndPoly();
	}

	LBXGL_Shader_End();
}

LBXGL_API void LBXGL_Brush_DrawFaces(LBXGL_Brush *brush)
{
	float r, g, b, tr, tg, tb;
	int i, j, k, l;

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawFaces(brush);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawFaces(brush);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

//	pdglEnable(GL_TEXTURE_2D);
//	pdglColor4f(1, 1, 1, 1);

	if(brush->flags&LBXGL_BRFL_HIDDEN)
		return;
//	if(brush->flags&LBXGL_BRFL_EFFECT)
//		return;

	if(brush->flags&LBXGL_BRFL_QUADS)
	{
		LBXGL_Brush_DrawFacesQuads(brush);
		return;
	}

//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))
//		return;

//	if(brush->flags&LBXGL_BRFL_ALPHA)
//		pdglEnable(GL_CULL_FACE);

//	*(int *)-1=-1;

	r=1; g=1; b=1;

	if(brush->grp && (brush->grp->flags&LBXGL_BRFL_SEL))
		{ g=0.75; b=0.25; }
	if(brush->se_owner && (brush->se_owner->flags&LBXGL_SEFL_SEL))
		{ g=0.75; b=0.25; }

	if(brush->flags&LBXGL_BRFL_OPSEL) { g=0.5; }
	if(brush->flags&LBXGL_BRFL_SEL) { r=0.5; b=0.5; }

	if(brush->rgb)
//	if(0)
	{
		r=0.1; g=0.1; b=0.1;


		for(i=0; i<brush->n_poly; i++)
		{
//			if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
//				continue;

			LBXGL_Shader_BindTexture(brush->pf_tex[i]);

			LBXGL_Shader_Normal3fv(brush->norm+i*4);
			LBXGL_Shader_BeginPoly();
			for(j=0; j<brush->pf_nvec[i]; j++)
			{
				k=brush->pf_ivec[i]+j;
//				tr=(brush->rgb[k*3+0]/255.0)+r;
//				tg=(brush->rgb[k*3+1]/255.0)+g;
//				tb=(brush->rgb[k*3+2]/255.0)+b;

				tr=brush->rgb[k*3+0]+r;
				tg=brush->rgb[k*3+1]+g;
				tb=brush->rgb[k*3+2]+b;

				LBXGL_Shader_Color4f(tr, tg, tb, 1);
				LBXGL_Shader_TexCoord2fv(
					brush->st+brush->vec[k*4+1]*2);
				LBXGL_Shader_Vertex3fv(
					brush->xyz+brush->vec[k*4+0]*3);
			}
			LBXGL_Shader_EndPoly();
		}

		return;
	}

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
//		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
//			continue;

//		pdglBindTexture(GL_TEXTURE_2D, brush->texnum[i]);
		LBXGL_Shader_BindTexture(brush->pf_tex[i]);

		LBXGL_Shader_Normal3fv(brush->norm+brush->pf_plane[i]*4);
		LBXGL_Shader_Color4f(r, g, b, 1);
		LBXGL_Shader_BeginPoly();
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			k=brush->pf_ivec[i]+j;
			LBXGL_Shader_TexCoord2fv(
				brush->st+brush->vec[k*4+1]*2);
			LBXGL_Shader_Vertex3fv(
				brush->xyz+brush->vec[k*4+0]*3);
		}
		LBXGL_Shader_EndPoly();
	}

//	pdglDisable(GL_CULL_FACE);
}

LBXGL_API void LBXGL_Brush_DrawFacesAlpha(LBXGL_Brush *brush)
{
	float r, g, b, tr, tg, tb;
	int i, j, k, l;

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawFacesAlpha(brush);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawFacesAlpha(brush);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

//	pdglEnable(GL_TEXTURE_2D);
//	pdglColor4f(1, 1, 1, 1);

	if(brush->flags&LBXGL_BRFL_HIDDEN)
		return;
//	if(brush->flags&LBXGL_BRFL_EFFECT)
//		return;

//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))
//		return;

//	if(brush->flags&LBXGL_BRFL_ALPHA)
//		pdglEnable(GL_CULL_FACE);

//	*(int *)-1=-1;

	r=1; g=1; b=1;

	if(brush->rgb)
	{
		r=0.1; g=0.1; b=0.1;


		for(i=0; i<brush->n_poly; i++)
		{
			if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
				continue;

			LBXGL_Shader_BindTexture(brush->pf_tex[i]);

			LBXGL_Shader_Normal3fv(brush->norm+i*4);
			LBXGL_Shader_BeginPoly();
			for(j=0; j<brush->pf_nvec[i]; j++)
			{
				k=brush->pf_ivec[i]+j;
				tr=brush->rgb[k*3+0]+r;
				tg=brush->rgb[k*3+1]+g;
				tb=brush->rgb[k*3+2]+b;

				LBXGL_Shader_Color4f(tr, tg, tb, 1);
				LBXGL_Shader_TexCoord2fv(
					brush->st+brush->vec[k*4+1]*2);
				LBXGL_Shader_Vertex3fv(
					brush->xyz+brush->vec[k*4+0]*3);
			}
			LBXGL_Shader_EndPoly();
		}

		return;
	}

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			continue;

		LBXGL_Shader_BindTexture(brush->pf_tex[i]);

		LBXGL_Shader_Normal3fv(brush->norm+brush->pf_plane[i]*4);
		LBXGL_Shader_Color4f(r, g, b, 1);
		LBXGL_Shader_BeginPoly();
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			k=brush->pf_ivec[i]+j;
			LBXGL_Shader_TexCoord2fv(
				brush->st+brush->vec[k*4+1]*2);
			LBXGL_Shader_Vertex3fv(
				brush->xyz+brush->vec[k*4+0]*3);
		}
		LBXGL_Shader_EndPoly();
	}
}


LBXGL_API void LBXGL_Brush_DrawFacesShadow(LBXGL_Brush *brush)
{
	float r, g, b, tr, tg, tb;
	float f;
	int i, j, k;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);
	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawFacesShadow(brush);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawFacesShadow(brush);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

//	if(!LBXGL_Brush_CheckSphereFrustum(brush->org, brush->rad))return;
//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;

	for(i=0; i<brush->n_poly; i++)
	{
//		if(V3F_NDOT(lbxgl_cam->org, brush->norm+i*4)<0)
//			continue;

		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			continue;

		pdglBlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_BindTexture(brush->pf_tex[i]);
		LBXGL_Shader_Normal3fv(brush->norm+brush->pf_plane[i]*4);
		LBXGL_Shader_Color4f(1, 1, 1, 1);
		LBXGL_Shader_BeginPoly();
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			k=brush->pf_ivec[i]+j;
			LBXGL_Shader_TexCoord2fv(
				brush->st+brush->vec[k*4+1]*2);
			LBXGL_Shader_Vertex3fv(
				brush->xyz+brush->vec[k*4+0]*3);
		}
		LBXGL_Shader_EndPoly();
	}

	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

LBXGL_API void LBXGL_Brush_DrawBrushFinal(LBXGL_Brush *brush)
{
	LBXGL_Brush_DrawFacesShadow(brush);
}

LBXGL_API void LBXGL_Brush_DrawBrushDark(LBXGL_Brush *brush)
{
	float r, g, b, tr, tg, tb;
	float f;
	int i, j, k;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawBrushDark(brush);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawBrushDark(brush);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

//	if(!LBXGL_Brush_CheckSphereFrustum(brush->org, brush->rad))return;
//	if(!LBXGL_Brush_CheckBrushFrustum(brush))return;
//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;

//	LBXGL_Brush_DrawSolidFlat(brush);

#if 1
	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(0, 0, 0, 1);

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		k=brush->pf_flag[i];
		if(k&(LBXGL_TXFL_NODRAW|LBXGL_TXFL_ALPHA))
			continue;

		k=brush->pf_ivec[i];
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			pdglVertex3fv(brush->xyz+brush->vec[k*4+0]*3);
			k++;
		}
		pdglEnd();
	}
#endif
}

#if 1
LBXGL_API void LBXGL_Brush_DrawBrushLight2(LBXGL_Brush *brush)
{
	float *xyz;
	short *vec;
	float f, g, d;
	int i, j, k, l;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawBrushLight2(brush);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawBrushLight2(brush);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;

	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=brush->pf_nvec[i]*4; continue; }
		pdglNormal3fv(brush->norm+brush->pf_plane[i]*4);
		pdglBegin(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4;
		}
		pdglEnd();
	}
}
#endif

#if 0
LBXGL_API void LBXGL_Brush_DrawBrushLightQuad3(LBXGL_Brush *brush,
	LBXGL_Light *light)
{
	float txyz[256];
	float tnorm[256];
	float lorg[3];
	float *xyz, *norm;
	short *vec;
	int i, j, k, l;

//	return;

	LBXGL_Shader_TransformPointLocal(light->org, lorg);

	xyz=brush->xyz; vec=brush->vec; k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=16; continue; }

		norm=brush->norm+brush->pf_plane[i]*4;
		if(V3F_NDOT(lorg, norm)<=0)
			{ vec+=16; continue; }

		for(j=0; j<4; j++)
		{
			xyz=brush->xyz+(*vec)*3;
			V3F_COPY(xyz, txyz+(k*3));
			V4F_COPY(norm, tnorm+(k*4));
			vec+=4; k++;
		}
	}

	if(!k)return;

	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglEnableClientState(GL_NORMAL_ARRAY);
	pdglVertexPointer(3, GL_FLOAT, 0, txyz);
	pdglNormalPointer(GL_FLOAT, 4*sizeof(float), tnorm);
	pdglDrawArrays(GL_QUADS, 0, k);
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_NORMAL_ARRAY);
}
#endif

#if 1
LBXGL_API void LBXGL_Brush_DrawBrushLightQuad3_Shader(LBXGL_Brush *brush,
	LBXGL_Light *light)
{
	float txyz[256];
	float tst[256];
	float tnorm[256];
	float lorg[3];
	float *xyz, *st, *norm;
	short *vec;
	int tex;
	int i, j, k, l;

//	return;

	LBXGL_Shader_TransformPointLocal(light->org, lorg);

	xyz=brush->xyz; st=brush->st; vec=brush->vec; k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=16; continue; }

		tex=brush->pf_tex[i];

		norm=brush->norm+brush->pf_plane[i]*4;
		if(V3F_NDOT(lorg, norm)<=0)
			{ vec+=16; continue; }

		for(j=0; j<4; j++)
		{
			xyz=brush->xyz+(vec[0]*3);
			st=brush->st+(vec[1]*2);
			V3F_COPY(xyz, txyz+(k*3));
			V2F_COPY(st, tst+(k*2));
			V4F_COPY(norm, tnorm+(k*4));
			vec+=4; k++;
		}
	}

	if(!k)return;

	LBXGL_BrushLight_DrawPrimLight(light, PDGL_QUADS,
		txyz, tst, tnorm, k, tex);
	LBXGL_BrushLight_DrawPrimLight_Reset(light);
}
#endif

LBXGL_API void LBXGL_Brush_DrawBrushLight3_Shader(LBXGL_Brush *brush,
	LBXGL_Light *light)
{
	float txyz[256];
	float tst[256];
	float tnorm[256];
	float lorg[3];

	float *xyz, *st, *norm, *sdir, *tdir;
	short *vec;
	float f, g, d;
	int i, j, k, l;

	LBXGL_Shader_TransformPointLocal(light->org, lorg);

#if 1
	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=brush->pf_nvec[i]*4; continue; }
		norm=brush->norm+brush->pf_plane[i]*4;
		if(V3F_NDOT(lorg, norm)<=0)
			{ vec+=brush->pf_nvec[i]*4; continue; }

		sdir=brush->sdir+brush->pf_plane[i]*4;
		tdir=brush->tdir+brush->pf_plane[i]*4;

		LBXGL_Shader_BindTexture(brush->pf_tex[i]);
		LBXGL_Shader_SetCurrentMaterial();

		LBXGL_Shader_Normal4fv(norm);
		LBXGL_Shader_NormalX4fv(sdir);
		LBXGL_Shader_NormalY4fv(tdir);

		LBXGL_Shader_BeginLight(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			xyz=brush->xyz+(vec[0]*3);
			st=brush->st+(vec[1]*2);
			LBXGL_Shader_TexCoord2fv(st);
			LBXGL_Shader_Vertex3fv(xyz);
			vec+=4;
		}
		LBXGL_Shader_EndLight();
		
//		LBXGL_BrushLight_DrawPrimLight(light, GL_POLYGON,
//			txyz, tst, tnorm, l, brush->pf_tex[i]);
	}
//	LBXGL_BrushLight_DrawPrimLight_Reset(light);
	LBXGL_Shader_BindTexture(0);
#endif

#if 0
	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=brush->pf_nvec[i]*4; continue; }
		norm=brush->norm+brush->pf_plane[i]*4;
		if(V3F_NDOT(lorg, norm)<=0)
			{ vec+=brush->pf_nvec[i]*4; continue; }
//		LBXGL_Texture_SetImageMaterial(brush->pf_tex[i]);
		pdglNormal3fv(norm);
		
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			xyz=brush->xyz+(vec[0]*3);
			st=brush->st+(vec[1]*2);
			V3F_COPY(xyz, txyz+(j*3));
			V2F_COPY(st, tst+(j*2));
			V4F_COPY(norm, tnorm+(j*4));
			vec+=4;
		}
		
		LBXGL_BrushLight_DrawPrimLight(light, PDGL_POLYGON,
			txyz, tst, tnorm, l, brush->pf_tex[i]);
	}
	LBXGL_BrushLight_DrawPrimLight_Reset(light);
#endif
}

LBXGL_API void LBXGL_Brush_DrawBrushLight3(LBXGL_Brush *brush,
	LBXGL_Light *light)
{
	float lorg[3], vorg[3];
	float *xyz, *norm;
	short *vec;
	float f, g, d;
	int i, j, k, l;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawBrushLight3(brush, light);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawBrushLight3(brush, light);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

	if((brush->texfl&LBXGL_TXFL_LIGHTMAT) && !shader_nolightmat &&
		!(lbxgl_world->flags&BTGE_WFL_LIGHTBUDGET))
	{
		LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, vorg);
		f=LBXGL_Brush_DistancePointBrush(brush, vorg);
		if(f<256)
//		if(V3F_DIST(brush->org, vorg)<(brush->rad+16+256))
		{
#if 1
			if(brush->flags&LBXGL_BRFL_QUADS)
			{
				LBXGL_Brush_DrawBrushLightQuad3_Shader(brush, light);
				return;
			}
#endif

			LBXGL_Brush_DrawBrushLight3_Shader(brush, light);
			return;
		}
	}

//	if((brush->flags&LBXGL_BRFL_QUADS) && !shader_nophong)
//		{ LBXGL_Brush_DrawBrushLightQuad3(brush, light); return; }

//	if(!LBXGL_Brush_CheckBrushFrustumVisible2(brush))return;

	LBXGL_Shader_TransformPointLocal(light->org, lorg);

	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
			{ vec+=brush->pf_nvec[i]*4; continue; }
		norm=brush->norm+brush->pf_plane[i]*4;
		if(V3F_NDOT(lorg, norm)<=0)
			{ vec+=brush->pf_nvec[i]*4; continue; }
		LBXGL_Texture_SetImageMaterial(brush->pf_tex[i]);
		pdglNormal3fv(norm);
		pdglBegin(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4;
		}
		pdglEnd();
	}
}

LBXGL_API void LBXGL_Brush_DrawList(LBXGL_Brush *fst)
{
	LBXGL_Brush *cur;

	cur=fst;
	while(cur)
	{
		LBXGL_Brush_DrawFaces(cur);
		cur=cur->next;
	}
}

LBXGL_API void LBXGL_Brush_DrawGroup(LBXGL_BrushGroup *grp)
{
	LBXGL_BrushGroup *cur;

	LBXGL_Brush_DrawList(grp->brush);

	cur=grp->first;
	while(cur)
	{
		LBXGL_Brush_DrawGroup(cur);
		cur=cur->next;
	}
}

#if 1
LBXGL_API void LBXGL_Brush_DrawGlow(LBXGL_Brush *brush)
{
	float *xyz;
	short *vec;
	float f, g, d;
	int i, j, k, l;

	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		k=LBXGL_Texture_GetImageGlow(brush->pf_tex[i]);
		if(k>0)
		{
			pdglEnable(GL_TEXTURE_2D);
			pdglBindTexture(GL_TEXTURE_2D, k);
			pdglColor4f(1, 1, 1, 1);
		}else
		{
			pdglDisable(GL_TEXTURE_2D);
			pdglColor4f(0, 0, 0, 1);
		}

		pdglNormal3fv(brush->norm+brush->pf_plane[i]*4);
		pdglBegin(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4;
		}
		pdglEnd();
	}
}
#endif

LBXGL_API void LBXGL_Brush_DrawBSPGlow(LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;

	pdglStencilFunc(GL_ALWAYS, node->id, 255);

	cur=node->mid;
	while(cur)
	{
		LBXGL_Brush_DrawGlow(cur);
		cur=cur->nnext;
	}
}

#if 0
LBXGL_API void LBXGL_Brush_DrawFlatQuad(LBXGL_Brush *brush)
{
	short varr[64];
	short *vec;
	int i, j, k, l;

	vec=brush->vec;

	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglVertexPointer(3, GL_FLOAT, 0, brush->xyz);
	k=0; l=brush->n_poly;
	for(i=0; i<l; i++)
	{
		for(j=0; j<4; j++)
			{ varr[k++]=*vec; vec+=4; }
	}
	pdglDrawElements(GL_QUADS, l*4, GL_UNSIGNED_SHORT, varr);
	pdglDisableClientState(GL_VERTEX_ARRAY);
}
#endif

LBXGL_API void LBXGL_Brush_DrawFlat(LBXGL_Brush *brush)
{
	short varr[64];
	float *xyz;
	short *vec;
	float f, g, d;
	int i, j, k, l;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawFlat(brush);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawFlat(brush);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

#if 1
//	if(brush->flags&LBXGL_BRFL_QUADS)
//		{ LBXGL_Brush_DrawFlatQuad(brush); return; }

	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		pdglNormal3fv(brush->norm+brush->pf_plane[i]*4);
		pdglBegin(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4;
		}
		pdglEnd();
	}
#endif
}

#if 0
LBXGL_API void LBXGL_Brush_DrawSolidFlatQuad(LBXGL_Brush *brush)
{
	short varr[64];
	short *vec;
	int i, j, k, l;

	vec=brush->vec;

//		pdglNormal3fv(brush->norm+brush->pf_plane[i]*4);

	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglVertexPointer(3, GL_FLOAT, 0, brush->xyz);
	k=0; l=brush->n_poly;
	for(i=0; i<l; i++)
	{
		for(j=0; j<4; j++)
			{ varr[k++]=*vec; vec+=4; }
	}
	pdglDrawElements(GL_QUADS, l*4, GL_UNSIGNED_SHORT, varr);
	pdglDisableClientState(GL_VERTEX_ARRAY);
}
#endif

LBXGL_API void LBXGL_Brush_DrawSolidFlat(LBXGL_Brush *brush)
{
	short varr[64];
	float *xyz;
	short *vec;
	float f, g, d;
	int i, j, k, l;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawSolidFlat(brush);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawSolidFlat(brush);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

#if 1
	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;
	if(brush->flags&LBXGL_BRFL_EFFECT)return;

//	if(brush->flags&LBXGL_BRFL_QUADS)
//		{ LBXGL_Brush_DrawSolidFlatQuad(brush); return; }

	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
//		if(brush->pf_flag[i]&LBXGL_TXFL_NODRAW)
//			{ vec+=brush->pf_nvec[i]*4; continue; }

		pdglNormal3fv(brush->norm+brush->pf_plane[i]*4);
		pdglBegin(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4;
		}
		pdglEnd();
	}
#endif
}

#if 0
LBXGL_API void LBXGL_Brush_DrawSolidFlatRGBQuad(LBXGL_Brush *brush)
{
	short varr[64];
	short *vec;
	int i, j, k, l;

	vec=brush->vec;

//		pdglNormal3fv(brush->norm+brush->pf_plane[i]*4);

	pdglEnableClientState(GL_VERTEX_ARRAY);
	pdglEnableClientState(GL_COLOR_ARRAY);
	pdglVertexPointer(3, GL_FLOAT, 0, brush->xyz);
	pdglColorPointer(3, GL_FLOAT, 0, brush->rgb);
	k=0; l=brush->n_poly;
	for(i=0; i<l; i++)
	{
		for(j=0; j<4; j++)
			{ varr[k++]=*vec; vec+=4; }
	}
	pdglDrawElements(GL_QUADS, l*4, GL_UNSIGNED_SHORT, varr);
	pdglDisableClientState(GL_VERTEX_ARRAY);
	pdglDisableClientState(GL_COLOR_ARRAY);
}
#endif

LBXGL_API void LBXGL_Brush_DrawSolidFlatRGB(LBXGL_Brush *brush)
{
	short varr[64];
	float *xyz;
	short *vec;
	float *rgb;
	float f, g, d;
	int i, j, k, l;

	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(brush->flags&
		(LBXGL_BRFL_HIDDEN|LBXGL_BRFL_ALPHA|LBXGL_BRFL_EFFECT))
			return;

#if 0
	if(brush->flags&LBXGL_BRFL_QUADS)
	{
		LBXGL_Brush_DrawSolidFlatRGBQuad(brush);
		return;
	}
#endif

	xyz=brush->xyz; vec=brush->vec; rgb=brush->rgb;
	for(i=0; i<brush->n_poly; i++)
	{
		pdglNormal3fv(brush->norm+brush->pf_plane[i]*4);
		pdglBegin(PDGL_POLYGON);
//		k=brush->pf_ivec[i];
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglColor3fv(rgb);
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4; rgb+=3;
		}
		pdglEnd();
	}
}

LBXGL_API void LBXGL_Brush_DrawBSPNodeColorsMid(LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	int i, j, k;

	if(!node->mid)return;
	if(!LBXGL_BrushWorld_CheckNodeFrustum(node))
		return;

	pdglDisable(GL_TEXTURE_2D);

	i=node->id&63;
	j=(node->id>>6)&63;
	k=(node->id>>12)&63;
	pdglColor4ub((i<<2)+2, (j<<2)+2, (k<<2)+2, 255);

	cur=node->mid;
	while(cur)
	{
#if 0
		cur->flags&=~LBXGL_BRFL_VISCLIP;
		if(!LBXGL_Brush_CheckBrushFrustum(cur))
		{
			cur->flags|=LBXGL_BRFL_VISCLIP;
			cur=cur->nnext;
			continue;
		}
#endif

		LBXGL_Brush_DrawSolidFlat(cur);
		cur=cur->nnext;
	}
}

LBXGL_API void LBXGL_Brush_DrawBSPNodeColors(LBXGL_BrushNode *node)
{
	float f;

	if(!node)return;

	LBXGL_Brush_DrawBSPNodeColorsMid(node);

	f=V3F_NDOT(lbxgl_cam->org, node->norm);
	if(f<0)
	{
		LBXGL_Brush_DrawBSPNodeColors(node->lnode);
		LBXGL_Brush_DrawBSPNodeColors(node->rnode);
	}else
	{
		LBXGL_Brush_DrawBSPNodeColors(node->rnode);
		LBXGL_Brush_DrawBSPNodeColors(node->lnode);
	}
}

LBXGL_API void LBXGL_Brush_DrawBSPNodeBlack(LBXGL_BrushNode *node)
{
	LBXGL_Brush *cur;
	float f;
	int i, j, k;

	if(!node)return;

	pdglDisable(GL_TEXTURE_2D);
	pdglColor4f(0, 0, 0, 1);

	cur=node->mid;
	while(cur)
	{
		LBXGL_Brush_DrawSolidFlat(cur);
		cur=cur->nnext;
	}

	f=V3F_NDOT(lbxgl_cam->org, node->norm);

	if(f<0)
	{
		LBXGL_Brush_DrawBSPNodeBlack(node->lnode);
		LBXGL_Brush_DrawBSPNodeBlack(node->rnode);
	}else
	{
		LBXGL_Brush_DrawBSPNodeBlack(node->rnode);
		LBXGL_Brush_DrawBSPNodeBlack(node->lnode);
	}
}

LBXGL_API void LBXGL_Brush_DrawBSPShadowNodeColors(LBXGL_BrushNode *node,
	float *org, float rad)
{
	LBXGL_Brush *cur;
	float f;
	int i, j, k;

	if(!node)return;

	pdglDisable(GL_TEXTURE_2D);

	i=node->id&63;
	j=(node->id>>6)&63;
	k=(node->id>>12)&63;
	pdglColor4ub((i<<2)+2, (j<<2)+2, (k<<2)+2, 255);

	cur=node->mid;
	while(cur)
	{
		if(!LBXGL_Brush_CheckBrushShadowFrustum(cur, org))
			{ cur=cur->nnext; continue; }

		LBXGL_Brush_DrawShadowVolume2(cur, org);
		cur=cur->nnext;
	}

	f=V3F_NDOT(org, node->norm);

	if(f<=(-rad))
	{
		LBXGL_Brush_DrawBSPShadowNodeColors(node->lnode, org, rad);
		return;
	}

	if(f>=rad)
	{
		LBXGL_Brush_DrawBSPShadowNodeColors(node->rnode, org, rad);
		return;
	}

	if(f<0)
	{
		LBXGL_Brush_DrawBSPShadowNodeColors(node->lnode, org, rad);
		LBXGL_Brush_DrawBSPShadowNodeColors(node->rnode, org, rad);
	}else
	{
		LBXGL_Brush_DrawBSPShadowNodeColors(node->rnode, org, rad);
		LBXGL_Brush_DrawBSPShadowNodeColors(node->lnode, org, rad);
	}
}


//Fluid

LBXGL_API void LBXGL_Brush_DrawFluidFlat(LBXGL_Brush *brush)
{
	float *xyz;
	short *vec;
	float f, g, d;
	int i, j, k, l;

	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	xyz=brush->xyz; vec=brush->vec;
	for(i=0; i<brush->n_poly; i++)
	{
		pdglBegin(PDGL_POLYGON);
		l=brush->pf_nvec[i];
		for(j=0; j<l; j++)
		{
			pdglVertex3fv(xyz+(*vec)*3);
			vec+=4;
		}
		pdglEnd();
	}
}

LBXGL_API void LBXGL_Brush_DrawFluid(LBXGL_Brush *brush)
{
	float sdir[3], tdir[3];
	float *vec;
	float s, t, th;
	int i, j, k, l;

//	pdglEnable(GL_TEXTURE_2D);
//	pdglColor4f(1, 1, 1, 1);

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		pdglBindTexture(GL_TEXTURE_2D, brush->pf_tex[i]);

		vec=brush->norm+brush->pf_plane[i]*4;
		pdglNormal3fv(vec);
		Hull_AdjacentNormals21(vec, sdir, tdir);

		pdglColor4f(1, 1, 1, 0.33);
		if((brush->flags&LBXGL_TXFL_FLUID_MASK)==LBXGL_TXFL_FLUID_LAVA)
			pdglColor4f(1, 1, 1, 1);
		if((brush->flags&LBXGL_TXFL_FLUID_MASK)==LBXGL_TXFL_FLUID_SEWER)
			pdglColor4f(1, 1, 1, 0.66);
		if((brush->flags&LBXGL_TXFL_FLUID_MASK)==LBXGL_TXFL_FLUID_SLIME)
			pdglColor4f(1, 1, 1, 0.66);

		pdglBegin(PDGL_POLYGON);
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			k=brush->pf_ivec[i]+j;
			vec=brush->xyz+brush->vec[k*4+0]*3;

			th=V3F_LEN(vec);

			s=V3F_DOT(vec, sdir)/64; t=V3F_DOT(vec, tdir)/64;
			s+=12*sin(lbxgl_state->time_f+th)/64;
			t+=12*cos(lbxgl_state->time_f+th)/64;

			pdglTexCoord2f(s, t);

//			pdglTexCoord2fv(brush->st+brush->vec[k*4+1]*2);
			pdglVertex3fv(vec);
		}
		pdglEnd();
	}

//	pdglDisable(GL_CULL_FACE);
}

LBXGL_API void LBXGL_Brush_DrawFluid2(LBXGL_Brush *brush)
{
	float sdir[3], tdir[3];
	float *vec;
	float s, t;
	int i, j, k, l, tex;

	if((brush->flags&LBXGL_TXFL_FLUID_MASK)==LBXGL_TXFL_FLUID_LAVA)
		return; //lava doesn't need overlay...

	tex=LBXGL_Texture_LoadImage("textures/shader/watery_base");
	LBXGL_Shader_BindTexture(tex);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		LBXGL_Shader_Normal3fv(brush->norm+brush->pf_plane[i]*4);
		LBXGL_Shader_BeginPoly();
		for(j=0; j<brush->pf_nvec[i]; j++)
		{
			k=brush->pf_ivec[i]+j;
			LBXGL_Shader_TexCoord2fv(
				brush->st+brush->vec[k*4+1]*2);
			LBXGL_Shader_Vertex3fv(
				brush->xyz+brush->vec[k*4+0]*3);
		}
		LBXGL_Shader_EndPoly();
	}
}

LBXGL_API void LBXGL_Brush_DrawLayerFog(float *rgba, float dist, float step, int cnt)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	float d;

	int i;

	pdglDisable(GL_TEXTURE_2D);
	pdglDepthMask(0);

//	pdglColor4f(rgba[0], rgba[1], rgba[2], rgba[3]*step);

	for(i=0; i<cnt; i++)
	{
		d=dist+(cnt-i-1)*step;
		pdglColor4f(rgba[0], rgba[1], rgba[2], rgba[3]*d);

		V3F_SCALEADDSCALE(lbxgl_cam->rt, -1*d, lbxgl_cam->up, -0.75*d, pt0);
		V3F_SCALEADDSCALE(lbxgl_cam->rt,  1*d, lbxgl_cam->up, -0.75*d, pt1);
		V3F_SCALEADDSCALE(lbxgl_cam->rt,  1*d, lbxgl_cam->up,  0.75*d, pt2);
		V3F_SCALEADDSCALE(lbxgl_cam->rt, -1*d, lbxgl_cam->up,  0.75*d, pt3);

		V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1*d, pt0);
		V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1*d, pt1);
		V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1*d, pt2);
		V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1*d, pt3);

		V3F_ADD(pt0, lbxgl_cam->org, pt0);
		V3F_ADD(pt1, lbxgl_cam->org, pt1);
		V3F_ADD(pt2, lbxgl_cam->org, pt2);
		V3F_ADD(pt3, lbxgl_cam->org, pt3);

		pdglBegin(PDGL_POLYGON);
		pdglTexCoord2f(0, 0);
		pdglVertex3fv(pt0);
		pdglTexCoord2f(1, 0);
		pdglVertex3fv(pt1);
		pdglTexCoord2f(1, 1);
		pdglVertex3fv(pt2);
		pdglTexCoord2f(0, 1);
		pdglVertex3fv(pt3);
		pdglEnd();
	}

	pdglDepthMask(1);
	pdglEnable(GL_TEXTURE_2D);
}

LBXGL_API void LBXGL_Brush_DrawFogVolume(
	LBXGL_BrushWorld *world, LBXGL_Brush *lst, float *rgba)
{
	float pt0[3], pt1[3], pt2[3], pt3[3];
	LBXGL_Brush *cur;
	float f, g, d, m, n, s;
	int i, cnt;

	if(!lst)return;

#if 1
	cur=lst; m=999999; n=-999999;
	while(cur)
	{
		LBXGL_Brush_GetExtents(cur, lbxgl_cam->fw, &f, &g);
		if(f<m)m=f; if(g>n)n=g;
		cur=cur->chain;
	}

//	if(!lst) { m=0; n=8192; }

	f=V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);
	m-=f; n-=f;

	if(m<=1)m=1;
	if(n<=2)return;
#endif

//	m=4;
//	n=1024;

//	cnt=0.25/rgba[3];
	s=0.0625/rgba[3];
	cnt=(n-m)/s+1;

	if(cnt<8)cnt=8;
	if(cnt>256)cnt=256;
	s=(n-m)/cnt;

//	cnt=64;
//	s=(n-m)/cnt;

	pdglDisable(GL_TEXTURE_2D);
//	pdglDepthMask(0);

//	pdglColor4f(rgba[0], rgba[1], rgba[2], rgba[3]*step);

	for(i=0; i<cnt; i++)
	{

		//compute the layer pos
		d=m+(cnt-i-1)*s;
//		pdglColor4f(rgba[0], rgba[1], rgba[2], rgba[3]*s);

		V3F_SCALEADDSCALE(lbxgl_cam->rt, -1*d, lbxgl_cam->up, -0.75*d, pt0);
		V3F_SCALEADDSCALE(lbxgl_cam->rt,  1*d, lbxgl_cam->up, -0.75*d, pt1);
		V3F_SCALEADDSCALE(lbxgl_cam->rt,  1*d, lbxgl_cam->up,  0.75*d, pt2);
		V3F_SCALEADDSCALE(lbxgl_cam->rt, -1*d, lbxgl_cam->up,  0.75*d, pt3);

		V3F_ADDSCALE(pt0, lbxgl_cam->fw, 1*d, pt0);
		V3F_ADDSCALE(pt1, lbxgl_cam->fw, 1*d, pt1);
		V3F_ADDSCALE(pt2, lbxgl_cam->fw, 1*d, pt2);
		V3F_ADDSCALE(pt3, lbxgl_cam->fw, 1*d, pt3);

		V3F_ADD(pt0, lbxgl_cam->org, pt0);
		V3F_ADD(pt1, lbxgl_cam->org, pt1);
		V3F_ADD(pt2, lbxgl_cam->org, pt2);
		V3F_ADD(pt3, lbxgl_cam->org, pt3);

		pdglDepthMask(1);
		pdglColorMask(0, 0, 0, 0);

		pdglBegin(PDGL_POLYGON);
		pdglTexCoord2f(0, 0);
		pdglVertex3fv(pt0);
		pdglTexCoord2f(1, 0);
		pdglVertex3fv(pt1);
		pdglTexCoord2f(1, 1);
		pdglVertex3fv(pt2);
		pdglTexCoord2f(0, 1);
		pdglVertex3fv(pt3);
		pdglEnd();


//		pdglDisable(GL_TEXTURE_2D);

		pdglClear(GL_STENCIL_BUFFER_BIT);
		pdglEnable(GL_STENCIL_TEST);

		pdglDepthMask(0);
		pdglColorMask(0, 0, 0, 0);

		pdglDepthFunc(GL_GEQUAL);
		pdglEnable(GL_CULL_FACE);
		pdglStencilFunc(GL_ALWAYS, 0, 255);

		pdglCullFace(GL_FRONT);
//		pdglCullFace(GL_BACK);
		pdglStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluidFlat(cur);
			cur=cur->chain;
		}
//		LBXGL_Voxel_DrawWorldFluidFlat(world);

		pdglCullFace(GL_BACK);
//		pdglCullFace(GL_FRONT);
		pdglStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluidFlat(cur);
			cur=cur->chain;
		}
//		LBXGL_Voxel_DrawWorldFluidFlat(world);

		pdglColorMask(1, 1, 1, 1);
		pdglDepthMask(1);

		pdglDepthFunc(GL_LEQUAL);

		pdglCullFace(GL_BACK);
//		pdglStencilFunc(GL_EQUAL, 0, 255);
		pdglStencilFunc(GL_NOTEQUAL, 0, 255);

		pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		pdglColor4f(rgba[0], rgba[1], rgba[2], rgba[3]*s);

		pdglBegin(PDGL_POLYGON);
		pdglTexCoord2f(0, 0);
		pdglVertex3fv(pt0);
		pdglTexCoord2f(1, 0);
		pdglVertex3fv(pt1);
		pdglTexCoord2f(1, 1);
		pdglVertex3fv(pt2);
		pdglTexCoord2f(0, 1);
		pdglVertex3fv(pt3);
		pdglEnd();

		pdglDisable(GL_STENCIL_TEST);
	}

	pdglDepthMask(1);
	pdglEnable(GL_TEXTURE_2D);

	pdglDepthFunc(GL_LEQUAL);
}

LBXGL_API void LBXGL_Brush_DrawFluidSurfaces(
	LBXGL_BrushWorld *world, LBXGL_Brush *lst)
{
	LBXGL_Brush *cur;

	if(!lst && !world->fluidcam)
		return;

#if 0

	pdglDisable(GL_TEXTURE_2D);

	pdglClear(GL_STENCIL_BUFFER_BIT);
	pdglEnable(GL_STENCIL_TEST);

	pdglDepthMask(0);
	pdglColorMask(0, 0, 0, 0);

	pdglDepthFunc(GL_GEQUAL);

	pdglEnable(GL_CULL_FACE);
	pdglStencilFunc(GL_ALWAYS, 0, 255);

	pdglCullFace(GL_FRONT);
//	pdglCullFace(GL_BACK);
	pdglStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluidFlat(cur);
		cur=cur->chain;
	}
	
//	LBXGL_Voxel_DrawWorldFluidFlat(world);

	pdglCullFace(GL_BACK);
//	pdglCullFace(GL_FRONT);
	pdglStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluidFlat(cur);
		cur=cur->chain;
	}

//	LBXGL_Voxel_DrawWorldFluidFlat(world);

	pdglColorMask(1, 1, 1, 1);
	pdglDepthMask(1);

	pdglDepthFunc(GL_LEQUAL);

	pdglCullFace(GL_BACK);
//	pdglStencilFunc(GL_EQUAL, 0, 255);
//	pdglStencilFunc(GL_NOTEQUAL, 0, 255);

#endif

	pdglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#if 1
	pdglDepthMask(0);
	pdglEnable(GL_TEXTURE_2D);

	pdglDisable(GL_CULL_FACE);
//	pdglStencilFunc(GL_NOTEQUAL, 0, 255);
	pdglStencilFunc(GL_EQUAL, 0, 255);

	pdglDepthFunc(GL_LESS);

	cur=lst;
	while(cur)
	{
		LBXGL_Brush_DrawFluid(cur);
		cur=cur->chain;
	}

	LBXGL_Voxel_DrawWorldFluid(world);

	if(!shader_nophong)
	{
		LBXGL_Shader_FlushState();
		pdglDepthFunc(GL_LEQUAL);

		cur=lst;
		while(cur)
		{
			LBXGL_Brush_DrawFluid2(cur);
			cur=cur->chain;
		}

		LBXGL_Voxel_DrawWorldFluid2(world);
	}

	pdglDepthMask(1);
#endif

	pdglDepthFunc(GL_LEQUAL);
	pdglDisable(GL_STENCIL_TEST);
}

