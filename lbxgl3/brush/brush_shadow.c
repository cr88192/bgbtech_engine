#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

#if 1
LBXGL_API void LBXGL_Brush_DrawShadowVolume2(
	LBXGL_Brush *brush, float *org)
{
	static float xyza[256*3];
	static float xyzb[256*3];
	float *tva[16];
	float *tvb[16];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	float *nv, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0;

	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

	if(brush->n_xyz>=256)
		{ *(int *)-1=-1; }
	if(brush->n_poly>=16)
		{ *(int *)-1=-1; }

	xyz=brush->xyz;
	for(i=0; i<brush->n_xyz; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SUB(tv0, org, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 4, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 1000, xyzb+i*3);
	}

	j=0;
	for(i=0; i<brush->n_poly; i++)
	{
		j=brush->pf_ivec[i];
		l=brush->pf_nvec[i];

		nv=brush->norm+brush->pf_plane[i]*4;
		f=V3F_NDOT(org, nv);
		if(f==0)continue;

		if(f>0)
		{
			for(k=0; k<l; k++)
			{
				i0=brush->vec[(j+k)*4+0];
				tva[k]=xyza+i0*3;
				tvb[k]=xyzb+i0*3;
			}
			tva[k]=tva[0];
			tvb[k]=tvb[0];
		}else
		{
			for(k=0; k<l; k++)
			{
				i0=brush->vec[(j+l-k-1)*4+0];
				tva[k]=xyza+i0*3;
				tvb[k]=xyzb+i0*3;
			}
			tva[k]=tva[0];
			tvb[k]=tvb[0];
		}

		pdglBegin(PDGL_POLYGON);
		for(k=0; k<l; k++)
			pdglVertex3fv(tva[k]);
		pdglEnd();

		pdglBegin(PDGL_QUADS);
		for(k=0; k<l; k++)
		{
			pdglVertex3fv(tva[k+1]);
			pdglVertex3fv(tva[k]);
			pdglVertex3fv(tvb[k]);
			pdglVertex3fv(tvb[k+1]);
		}
		pdglEnd();

		pdglBegin(PDGL_POLYGON);
		for(k=l-1; k>=0; k--)
			pdglVertex3fv(tvb[k]);
		pdglEnd();
	}
}
#endif


#if 0
//Depth-Pass shadow volume (omit end caps, draw silhouette)
LBXGL_API void LBXGL_Brush_DrawShadowVolumeDP(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
	static float xyza[256*3];
	static float xyzb[256*3];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawShadowVolumeDP(brush, light);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

	if(brush->n_xyz>=256)
		{ *(int *)-1=-1; }

	g=LBXGL_Shadow_LightGammaRadius(light);
	xyz=brush->xyz;
	for(i=0; i<brush->n_xyz; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SUB(tv0, light->org, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 4, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*g, xyzb+i*3);
	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

		f=V3F_NDOT(light->org, nv0);
		g=V3F_NDOT(light->org, nv1);

//		i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
//		pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
//		pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);

#if 1
		if((f>0) && (g<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
//			pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
//			pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);

			pdglVertex3fv(xyza+i1*3); pdglVertex3fv(xyza+i0*3);
			pdglVertex3fv(xyzb+i0*3); pdglVertex3fv(xyzb+i1*3);
		}

		if((g>0) && (f<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
//			pdglVertex3fv(xyza+i1*3); pdglVertex3fv(xyza+i0*3);
//			pdglVertex3fv(xyzb+i0*3); pdglVertex3fv(xyzb+i1*3);

			pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
			pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);
		}
#endif
	}
	pdglEnd();

#if 0
	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		nv=brush->norm+brush->pf_plane[i]*4;
		f=V3F_NDOT(light->org, nv);
		if(f>0)xyz=xyza; else xyz=xyzb;

		k=brush->pf_ivec[i];
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<brush->pf_nvec[i]; j++)
			{ pdglVertex3fv(xyz+brush->vec[k*4+0]*3); k++; }
		pdglEnd();
	}
#endif
}
#endif

#if 0
//Depth-Pass shadow volume (omit end caps, draw silhouette)
LBXGL_API void LBXGL_Brush_DrawShadowVolumeDP2(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
	static float xyza[256*3];
	static float xyzb[256*3];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawShadowVolumeDP(brush, light);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&LBXGL_BRFL_HIDDEN)return;
	if(brush->flags&LBXGL_BRFL_ALPHA)return;

	g=LBXGL_Shadow_LightGammaRadius(light);
	xyz=brush->xyz;
	for(i=0; i<brush->n_xyz; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SUB(tv0, light->org, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 4, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*g, xyzb+i*3);
	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

		f=V3F_NDOT(light->org, nv0);
		g=V3F_NDOT(light->org, nv1);

//		i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
//		pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
//		pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);

#if 1
		if((f>0) && (g<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
//			pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
//			pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);

			pdglVertex3fv(xyza+i1*3); pdglVertex3fv(xyza+i0*3);
			pdglVertex3fv(xyzb+i0*3); pdglVertex3fv(xyzb+i1*3);
		}

		if((g>0) && (f<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
//			pdglVertex3fv(xyza+i1*3); pdglVertex3fv(xyza+i0*3);
//			pdglVertex3fv(xyzb+i0*3); pdglVertex3fv(xyzb+i1*3);

			pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
			pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);
		}
#endif
	}
	pdglEnd();

#if 1
	k=0;
	for(i=0; i<brush->n_poly; i++)
	{
		nv=brush->norm+brush->pf_plane[i]*4;
		f=V3F_NDOT(light->org, nv);
		if(f>0)xyz=xyza; else xyz=xyzb;

		k=brush->pf_ivec[i];
		pdglBegin(PDGL_POLYGON);
		for(j=0; j<brush->pf_nvec[i]; j++)
			{ pdglVertex3fv(xyz+brush->vec[k*4+0]*3); k++; }
		pdglEnd();
	}
#endif
}
#endif

#if 1
//Depth-Pass shadow volume (omit end caps, draw silhouette)
LBXGL_API void LBXGL_Brush_DrawShadowVolumeDP3(
	LBXGL_Brush *brush, LBXGL_Light *light, int flags)
{
	static float xyzarr[512*3];
	static short varr[512];
	static short varr1[64];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz, *xyza, *xyzb;
	float f, g, h;
	int i, j, k, l, i0, i1, nva;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawShadowVolumeDP(brush, light);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawShadowVolumeDP(brush, light);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&(LBXGL_BRFL_HIDDEN|LBXGL_BRFL_ALPHA))
		return;

	if(brush->flags&LBXGL_BRFL_QUADS)
	{
		LBXGL_Brush_DrawShadowVolumeDP3Quad(brush, light, flags);
		return;
	}

	if(brush->n_xyz>=256)
		{ *(int *)-1=-1; }

	xyza=xyzarr+0;
	xyzb=xyzarr+256*3;

	g=LBXGL_Shadow_LightGammaRadius(light);
	xyz=brush->xyz;
	for(i=0; i<brush->n_xyz; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SCALEADDSCALE(tv0, 0.99, brush->org, 0.01, tv0);
		V3F_SUB(tv0, light->org, dv0);
//		V3F_NORMALIZE(dv0, dv0);
		V3F_FRNORMALIZE(dv0, dv0);
		V3F_ADD(tv0, lbxgl_cam->fw, tv0);
		V3F_ADDSCALE(tv0, dv0, 2, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*g, xyzb+i*3);
	}

	nva=0;
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

		f=V3F_NDOT(light->org, nv0);
		g=V3F_NDOT(light->org, nv1);

#if 1
		if((f>0) && (g<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			varr[nva++]=i1;			varr[nva++]=i0;
			varr[nva++]=i0+256;		varr[nva++]=i1+256;
		}

		if((g>0) && (f<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			varr[nva++]=i0;			varr[nva++]=i1;
			varr[nva++]=i1+256;		varr[nva++]=i0+256;
		}
#endif
	}

	LBXGL_Shader_DrawElementsSimpleFlat(
		PDGL_QUADS, nva, GL_UNSIGNED_SHORT, varr,
		3, GL_FLOAT, 0, xyzarr);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, xyzarr);
//	glDrawArrays(GL_QUADS, 0, 24);
	glDrawElements(GL_QUADS, nva, GL_UNSIGNED_SHORT, varr);
//	glDisableClientState(GL_VERTEX_ARRAY);
#endif

#if 1
	if(!(flags&LBXGL_SFL_NOENDCAPS))
	{
		k=0;
		for(i=0; i<brush->n_poly; i++)
		{
			nv=brush->norm+brush->pf_plane[i]*4;
			f=V3F_NDOT(light->org, nv);
			i0=(f>0)?0:256;

			k=brush->pf_ivec[i]; l=brush->pf_nvec[i];
			for(j=0; j<l; j++)
				{ varr1[j]=brush->vec[k*4+0]+i0; k++; }
//			glDrawElements(GL_POLYGON, l, GL_UNSIGNED_SHORT, varr1);

			LBXGL_Shader_DrawElementsSimpleFlat(
				PDGL_POLYGON, l, GL_UNSIGNED_SHORT, varr1,
				3, GL_FLOAT, 0, xyzarr);
		}
	}
#endif

//	glDisableClientState(GL_VERTEX_ARRAY);
}
#endif

#if 1
LBXGL_API void LBXGL_Brush_DrawShadowVolumeDP3Quad(
	LBXGL_Brush *brush, LBXGL_Light *light, int flags)
{
	static float xyzarr[512*3];
	static short varr[512];
	static short varr1[64];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz, *xyza, *xyzb;
	float f, g, h;
	int i, j, k, l, i0, i1, nva;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

//	if(brush->flags&(LBXGL_BRFL_HIDDEN|LBXGL_BRFL_ALPHA))
//		return;

	if(brush->n_xyz>=256)
		{ *(int *)-1=-1; }

	xyza=xyzarr+0;
	xyzb=xyzarr+256*3;

	h=0.01*(brush->rad/1024);
	if(h>0.1)h=0.1;

	g=LBXGL_Shadow_LightGammaRadius(light);
	g=999999;
	
	xyz=brush->xyz;
	for(i=0; i<brush->n_xyz; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SCALEADDSCALE(tv0, (1.0-h), brush->org, h, tv0);
		V3F_SUB(tv0, light->org, dv0);
		V3F_FRNORMALIZE(dv0, dv0);
		V3F_ADD(tv0, lbxgl_cam->fw, tv0);
		V3F_ADDSCALE(tv0, dv0, 2, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*g, xyzb+i*3);
	}

	nva=0;
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

		f=V3F_NDOT(light->org, nv0);
		g=V3F_NDOT(light->org, nv1);

#if 1
		if((f>0) && (g<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			varr[nva++]=i1;			varr[nva++]=i0;
			varr[nva++]=i0+256;		varr[nva++]=i1+256;
		}

		if((g>0) && (f<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			varr[nva++]=i0;			varr[nva++]=i1;
			varr[nva++]=i1+256;		varr[nva++]=i0+256;
		}
#endif
	}

#if 1
	if(!(flags&LBXGL_SFL_NOENDCAPS))
	{
		k=0;
		for(i=0; i<brush->n_poly; i++)
		{
			nv=brush->norm+brush->pf_plane[i]*4;
			f=V3F_NDOT(light->org, nv);
			i0=(f>0)?0:256;

//			k=brush->pf_ivec[i];
//			l=brush->pf_nvec[i];
			for(j=0; j<4; j++)
				{ varr[nva++]=brush->vec[k*4+0]+i0; k++; }
		}
	}
#endif

	LBXGL_Shader_DrawElementsSimpleFlat(
		PDGL_QUADS, nva, GL_UNSIGNED_SHORT, varr,
		3, GL_FLOAT, 0, xyzarr);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, xyzarr);
//	glDrawArrays(GL_QUADS, 0, 24);
	glDrawElements(GL_QUADS, nva, GL_UNSIGNED_SHORT, varr);
	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_VERTEX_ARRAY);
#endif
}
#endif

#if 1
//Depth-Pass shadow volume (draw silhouette)
//1&=skip end caps, 2&="infinite"
//generate clip planes
LBXGL_API int LBXGL_Brush_DrawShadowVolumeDP3ClipPlanes(
	LBXGL_Brush *brush, LBXGL_Light *light, int flags, float *clip)
{
	float g;
	int i;

	g=LBXGL_Shadow_LightGammaRadius(light);
	i=LBXGL_Brush_DrawShadowVolumeDPOrgClipPlanes(
		brush, light->org, g, flags, clip);
	return(i);
}

LBXGL_API int LBXGL_Brush_DrawShadowVolumeDPOrgClipPlanes(
	LBXGL_Brush *brush, float *lorg, float lrad, int flags, float *clip)
{
	static float xyzarr[512*3];
	static short varr[512];
	static short varr1[64];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz, *xyza, *xyzb;
	float f, g, h;
	int i, j, k, l, i0, i1, i2, nva, npl;

//	if(!brush->xyz)LBXGL_Brush_BuildFaces(brush);

	if(!brush->xyz)
	{
		if(brush->patch)
		{
//			LBXGL_BrushPatch_DrawShadowVolumeDP(brush, light);
			LBXGL_BrushPatch_DrawShadowVolumeOrg(brush, lorg, lrad);
			return(0);
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawShadowVolumeOrg(brush, lorg, lrad);
			return(0);
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	if(brush->flags&(LBXGL_BRFL_HIDDEN|LBXGL_BRFL_ALPHA))
		return(0);

	if(brush->n_xyz>=256)
		{ *(int *)-1=-1; }

	xyza=xyzarr+0;
	xyzb=xyzarr+256*3;

//	g=LBXGL_Shadow_LightGammaRadius(light);
	g=lrad;
	if(flags&LBXGL_SFL_INFINITE)g=999999;
	xyz=brush->xyz;
	for(i=0; i<brush->n_xyz; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SCALEADDSCALE(tv0, 0.99, brush->org, 0.01, tv0);
//		V3F_SUB(tv0, light->org, dv0);
		V3F_SUB(tv0, lorg, dv0);
//		V3F_NORMALIZE(dv0, dv0);
		V3F_FRNORMALIZE(dv0, dv0);
		V3F_ADD(tv0, lbxgl_cam->fw, tv0);
		V3F_ADDSCALE(tv0, dv0, 2, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*g, xyzb+i*3);
	}

	nva=0;
	for(i=0; i<brush->n_edge; i++)
	{
		i0=brush->edge[i*4+2]; i1=brush->edge[i*4+3];
		nv0=brush->norm+brush->pf_plane[i0]*4;
		nv1=brush->norm+brush->pf_plane[i1]*4;

//		f=V3F_NDOT(light->org, nv0);
//		g=V3F_NDOT(light->org, nv1);

		f=V3F_NDOT(lorg, nv0);
		g=V3F_NDOT(lorg, nv1);

#if 1
		if((f>0) && (g<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			varr[nva++]=i1;			varr[nva++]=i0;
			varr[nva++]=i0+256;		varr[nva++]=i1+256;
		}

		if((g>0) && (f<=0))
		{
			i0=brush->edge[i*4+0]; i1=brush->edge[i*4+1];
			varr[nva++]=i0;			varr[nva++]=i1;
			varr[nva++]=i1+256;		varr[nva++]=i0+256;
		}
#endif
	}

	LBXGL_Shader_DrawElementsSimpleFlat(
		PDGL_QUADS, nva, GL_UNSIGNED_SHORT, varr,
		3, GL_FLOAT, 0, xyzarr);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, xyzarr);
//	glDrawArrays(GL_QUADS, 0, 24);
	glDrawElements(GL_QUADS, nva, GL_UNSIGNED_SHORT, varr);
//	glDisableClientState(GL_VERTEX_ARRAY);
#endif

#if 1
	if(!(flags&LBXGL_SFL_NOENDCAPS))
	{
		k=0;
		for(i=0; i<brush->n_poly; i++)
		{
			nv=brush->norm+brush->pf_plane[i]*4;
//			f=V3F_NDOT(light->org, nv);
			f=V3F_NDOT(lorg, nv);
			i0=(f>0)?0:256;
			if(f==0)continue;

			k=brush->pf_ivec[i]; l=brush->pf_nvec[i];
			for(j=0; j<l; j++)
				{ varr1[j]=brush->vec[k*4+0]+i0; k++; }
//			glDrawElements(GL_POLYGON, l, GL_UNSIGNED_SHORT, varr1);

			LBXGL_Shader_DrawElementsSimpleFlat(
				PDGL_POLYGON, l, GL_UNSIGNED_SHORT, varr1,
				3, GL_FLOAT, 0, xyzarr);
		}
	}
#endif

//	glDisableClientState(GL_VERTEX_ARRAY);

	npl=0;

	if(clip)
	{
		npl=nva/4;
		for(i=0; i<npl; i++)
		{
			i0=varr[i*4+0];
			i1=varr[i*4+1];
			i2=varr[i*4+2];
			
			V3F_SUB(xyzarr+i1*3, xyzarr+i0*3, tv0);
			V3F_SUB(xyzarr+i2*3, xyzarr+i1*3, tv1);
//			V3F_CROSS(tv1, tv0, tv2);
			V3F_CROSS(tv0, tv1, tv2);
			V3F_NORMALIZE(tv2, tv2);
			tv2[3]=V3F_DOT(xyzarr+i0*3, tv2);
			
			V4F_COPY(tv2, clip+i*4);
		}

#if 0
		for(i=0; i<brush->n_face; i++)
		{
			f=V3F_NDOT(light->org, brush->norm+i*4);
			if(f<=0)continue;

			j=npl++;
			V4F_COPY(brush->norm+i*4, tv2);
			V4F_COPY(tv2, clip+j*4);
		}
#endif

#if 1
		for(i=0; i<brush->n_poly; i++)
		{
			nv=brush->norm+brush->pf_plane[i]*4;
//			f=V3F_NDOT(light->org, nv);
			f=V3F_NDOT(lorg, nv);
			j=(f>0)?0:256;
			if(f==0)continue;

			k=brush->pf_ivec[i]; l=brush->pf_nvec[i];
			i0=brush->vec[(k+0)*4+0]+j;
			i1=brush->vec[(k+1)*4+0]+j;
			i2=brush->vec[(k+2)*4+0]+j;

			V3F_SUB(xyzarr+i1*3, xyzarr+i0*3, tv0);
			V3F_SUB(xyzarr+i2*3, xyzarr+i1*3, tv1);
//			V3F_CROSS(tv1, tv0, tv2);
			V3F_CROSS(tv0, tv1, tv2);
			g=V3F_NORMALIZE(tv2, tv2);
			tv2[3]=V3F_DOT(xyzarr+i0*3, tv2);
			if(g<=0)continue;

			j=npl++;
			V4F_COPY(tv2, clip+j*4);
		}
#endif
	}
	
	return(npl);
}
#endif

LBXGL_API void LBXGL_Brush_DrawShadowVolume(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
	if(!brush->xyz)
	{
		if(brush->patch)
		{
			LBXGL_BrushPatch_DrawShadowVolume(brush, light);
			return;
		}

		if(brush->mesh)
		{
			LBXGL_BrushMesh_DrawShadowVolume(brush, light);
			return;
		}

		LBXGL_Brush_BuildFaces(brush);
	}

	LBXGL_Brush_DrawShadowVolumeDP3(brush, light, 0);
}

LBXGL_API void LBXGL_Brush_DrawShadowVolumeDPClip(
	LBXGL_Brush *brush, LBXGL_Light *light)
{
	float g;
	int i;

	g=LBXGL_Shadow_LightGammaRadius(light);
	LBXGL_Brush_DrawShadowVolumeDPOrgClip(
		brush, light, light->org, g);
	return;
}

LBXGL_API void LBXGL_Brush_DrawShadowVolumeDPOrgClip(
	LBXGL_Brush *brush, LBXGL_Light *light,
	float *lorg, float lrad)
{
	float clip[64*4];
	float pts[64*3];
	float nv[4], vorg[4], vfw[4], tv[4];
//	float lrad;
	int i, npts, nclip;

	if(brush->patch)
	{
		LBXGL_BrushPatch_DrawShadowVolumeDPOrgClip(
			brush, light, lorg, lrad);
		return;
	}

	if(brush->mesh)
	{
		LBXGL_BrushMesh_DrawShadowVolumeDPOrgClip(
			brush, light, lorg, lrad);
		return;
	}

//	glColorMask(1, 1, 1, 1);

//	LBXGL_Brush_DrawShadowVolumeDP3(brush, light);
//	nclip=LBXGL_Brush_DrawShadowVolumeDP3ClipPlanes(
//		brush, light, 2, clip);
//	nclip=LBXGL_Brush_DrawShadowVolumeDPOrgClipPlanes(
//		brush, lorg, lrad, 3, clip);
	nclip=LBXGL_Brush_DrawShadowVolumeDPOrgClipPlanes(
		brush, lorg, lrad, 0, clip);

	if(!nclip)
	{
		//skip if no clip places
		return;
	}

//	glColorMask(0, 0, 0, 0);

#if 1
//	lrad=LBXGL_Shadow_LightGammaRadius(light);

	if(!light)
	{
		LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, vorg);
		LBXGL_Shader_TransformVectorLocal(lbxgl_cam->fw, vfw);

		V3F_SCALE(vfw, -1, nv);
		nv[3]=V3F_DOT(vorg, nv)-0.25;
	}else
	{
		V3F_SCALE(lbxgl_cam->fw, -1, nv);
		nv[3]=V3F_DOT(lbxgl_cam->org, nv)-0.25;
	}

//	V3F_SCALE(lbxgl_cam->fw, 1, nv);
//	nv[3]=V3F_DOT(lbxgl_cam->org, nv)+1;

	Hull_MakePlaneFace(nv, pts);
	npts=LBXGL_Brush_ClipFacePlanes(clip, nclip, pts, pts, 4);
//	npts=LBXGL_Brush_ClipFaceBrushVolume(
//		brush, light->org, lrad, pts, pts, 4);
		
	if(npts>0)
	{
#if 0
		if(!light)
		{
			for(i=0; i<npts; i++)
			{
				LBXGL_Shader_TransformPoint(pts+i*3, tv);
				
				if(V3F_DIST(tv, pts+i*3)>0)
				{ 
					V3F_COPY(tv, pts+i*3);
//					*(int *)-1=-1;
				}
				
				V3F_COPY(tv, pts+i*3);
			}
		}
#endif

//		glColorMask(0, 0, 0, 0);
//		glColorMask(1, 1, 1, 1);

		pdglBegin(PDGL_POLYGON);
		for(i=0; i<npts; i++)
			{ pdglVertex3fv(pts+i*3); }
		pdglEnd();

//		glColorMask(0, 0, 0, 0);
	}
#endif
}

LBXGL_API void LBXGL_Brush_DrawShadowVolumeDPOrg(
	LBXGL_Brush *brush, LBXGL_Light *light,
	float *lorg, float lrad)
{
	LBXGL_Brush_DrawShadowVolumeDPOrgClipPlanes(
		brush, lorg, lrad, 0, NULL);
}


#if 0
//LBXGL_API void LBXGL_Brush_DrawIndexShadowVolumeDPOrg(
	float *xyz, int *idx, int nidx,
	LBXGL_Light *light,
	float *lorg, float lrad)
{
//	LBXGL_Brush_DrawShadowVolumeDPOrgClipPlanes(
//		brush, lorg, lrad, 2, NULL);
}
#endif

LBXGL_API int LBXGL_Brush_DrawPolyShadowVolumeDPOrgClipPlanes(
	float *xyz, int *pts, int nvec,
	float *lorg, float lrad, int flags, float *clip)
{
	float tva[64*3];
	float tvb[64*3];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3], dvn[4];
	float *tv;
	float f, g;
	int i, j, k, l, npl, xyzstep;

//	glDisable(GL_TEXTURE_2D);
//	glColor4f(0, 1, 1, 0.1);

	xyzstep=3;
	if(flags&LBXGL_SFL_XYZSTEP4)
		xyzstep=4;

	i=pts[0]; j=pts[1]; k=pts[2];
	V3F_SUB(xyz+i*xyzstep, xyz+j*xyzstep, dv0);
	V3F_SUB(xyz+k*xyzstep, xyz+j*xyzstep, dv1);
	V3F_CROSS(dv1, dv0, dvn);
	V3F_NORMALIZE(dvn, dvn);
	dvn[3]=V3F_DOT(xyz+i*xyzstep, dvn);

	f=V3F_NDOT(lorg, dvn);
	if(f==0)return(0);

	if(f>0)
	{
		for(i=0; i<nvec; i++)
		{
			tv=xyz+pts[i]*xyzstep;
			V3F_COPY(tv, tva+i*3);
		}
	}else
	{
		for(i=0; i<nvec; i++)
		{
			tv=xyz+pts[nvec-i-1]*xyzstep;
			V3F_COPY(tv, tva+i*3);
		}
	}

	for(i=0; i<nvec; i++)
	{
		V3F_COPY(tva+i*3, tv0);
		V3F_SUB(tv0, lorg, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, lrad, tvb+i*3);
	}

	if(!(flags&LBXGL_SFL_NOENDCAPS))
	{
		pdglBegin(PDGL_POLYGON);
		for(i=0; i<nvec; i++)
			pdglVertex3fv(tva+i*3);
		pdglEnd();
	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<nvec; i++)
	{
		pdglVertex3fv(tva+((i+1)%nvec)*3);
		pdglVertex3fv(tva+i*3);
		pdglVertex3fv(tvb+i*3);
		pdglVertex3fv(tvb+((i+1)%nvec)*3);
	}
	pdglEnd();

	if(!(flags&LBXGL_SFL_NOENDCAPS))
	{
		pdglBegin(PDGL_POLYGON);
		for(i=nvec-1; i>=0; i--)
			pdglVertex3fv(tvb+i*3);
		pdglEnd();
	}

	npl=0;

	if(clip)
	{
		//front plane
		V3F_SUB(tva+1*3, tva+0*3, dv0);
		V3F_SUB(tva+2*3, tva+1*3, dv1);
		V3F_CROSS(dv0, dv1, dvn);
		V3F_NORMALIZE(dvn, dvn);
		dvn[3]=V3F_DOT(tva+0*3, dvn);

		i=npl++;
		V4F_COPY(dvn, clip+i*4);

		//rear plane
		V3F_SUB(tvb+1*3, tvb+2*3, dv0);
		V3F_SUB(tvb+0*3, tvb+1*3, dv1);
		V3F_CROSS(dv0, dv1, dvn);
		V3F_NORMALIZE(dvn, dvn);
		dvn[3]=V3F_DOT(tvb+0*3, dvn);

		i=npl++;
		V4F_COPY(dvn, clip+i*4);

		//side planes
		for(i=0; i<nvec; i++)
		{
			j=(i+1)%nvec;
			V3F_SUB(tva+i*3, lorg, dv0);
			V3F_SUB(tva+j*3, tva+i*3, dv1);
			V3F_CROSS(dv0, dv1, dvn);
//			V3F_CROSS(dv1, dv0, dvn);
			V3F_NORMALIZE(dvn, dvn);
			dvn[3]=V3F_DOT(tva+i*3, dvn);

			j=npl++;
			V4F_COPY(dvn, clip+j*4);
		}
	}

	return(npl);
}

LBXGL_API void LBXGL_Brush_DrawPolyShadowVolumeDPOrgClip(
	float *xyz, int *idx, int nvec,
	LBXGL_Light *light,
	float *lorg, float lrad, int flags)
{
	float clip[64*4];
	float pts[64*3];
	float nv[4], vorg[4], vfw[4], tv[4];
//	float lrad;
	int i, npts, nclip;

//	glColorMask(1, 1, 1, 1);

	nclip=LBXGL_Brush_DrawPolyShadowVolumeDPOrgClipPlanes(
		xyz, idx, nvec,
		lorg, lrad, flags, clip);

	if(!nclip)
	{
		//skip if no clip places
		return;
	}

//	glColorMask(0, 0, 0, 0);

#if 1
//	lrad=LBXGL_Shadow_LightGammaRadius(light);

	if(!light)
	{
		LBXGL_Shader_TransformPointLocal(lbxgl_cam->org, vorg);
		LBXGL_Shader_TransformVectorLocal(lbxgl_cam->fw, vfw);

		V3F_SCALE(vfw, -1, nv);
		nv[3]=V3F_DOT(vorg, nv)-0.25;
	}else
	{
		V3F_SCALE(lbxgl_cam->fw, -1, nv);
		nv[3]=V3F_DOT(lbxgl_cam->org, nv)-0.25;
	}

	Hull_MakePlaneFace(nv, pts);
	npts=LBXGL_Brush_ClipFacePlanes(clip, nclip, pts, pts, 4);
		
	if(npts>0)
	{
//		glColorMask(0, 0, 0, 0);
//		glColorMask(1, 1, 1, 1);

		pdglBegin(PDGL_POLYGON);
		for(i=0; i<npts; i++)
			{ pdglVertex3fv(pts+i*3); }
		pdglEnd();

//		glColorMask(0, 0, 0, 0);
	}
#endif
}

LBXGL_API void LBXGL_Brush_DrawPolyShadowVolumeDPOrg(
	float *xyz, int *pts, int nvec,
	LBXGL_Light *light,
	float *lorg, float lrad, int flags)
{
	LBXGL_Brush_DrawPolyShadowVolumeDPOrgClipPlanes(
		xyz, pts, nvec,
		lorg, lrad, flags, NULL);
}

LBXGL_API void LBXGL_Brush_DrawPrimShadowArrays(
	int prim,
	float *xyz, float *st, float *norm, float *rgba,
	int nxyz, LBXGL_Light *light)
{
	int pts[256];
	float lorg[3];
	float lrad;
	int i, j;

	LBXGL_Shader_TransformPointLocal(light->org, lorg);
	lrad=LBXGL_Shadow_LightGammaRadius(light);

	if(prim==PDGL_POLYGON)
	{
		for(i=0; i<nxyz; i++)
			pts[i]=i;
		LBXGL_Brush_DrawPolyShadowVolumeDPOrgClip(xyz, pts, nxyz,
			light, lorg, lrad, LBXGL_SFL_XYZSTEP4);
		return;
	}

	if(prim==PDGL_QUADS)
	{
		for(i=0; i<(nxyz/4); i++)
		{
			pts[0]=i*4+0;
			pts[1]=i*4+1;
			pts[2]=i*4+2;
			pts[3]=i*4+3;
			LBXGL_Brush_DrawPolyShadowVolumeDPOrgClip(xyz, pts, 4,
				light, lorg, lrad, LBXGL_SFL_XYZSTEP4);
		}
		return;
	}

	if(prim==PDGL_TRIANGLES)
	{
		for(i=0; i<(nxyz/3); i++)
		{
			pts[0]=i*3+0;
			pts[1]=i*3+1;
			pts[2]=i*3+2;
			LBXGL_Brush_DrawPolyShadowVolumeDPOrgClip(xyz, pts, 3,
				light, lorg, lrad, LBXGL_SFL_XYZSTEP4);
		}
		return;
	}
}

LBXGL_API void LBXGL_Brush_DrawEdgeShadowVolume_ViewCap(
	float *xyza, float *xyzb, int n_xyz,
	float *a_norm, int n_norm,		//XYZW
	int *a_tedge, int n_tedge,		//v0 v1 n0 n1
	LBXGL_Light *light, float *lorg, float lrad, int flags)
{
	static float *xyzc=NULL;
	static int nxyz;
	float fnorm[4];
	float forg[4];
	int i0, i1;
	int i;

	if(!xyzc || (nxyz<n_xyz))
	{
		nxyz=256;
		while(nxyz<=n_xyz)nxyz+=nxyz>>1;
		xyzc=realloc(xyzc, nxyz*3*sizeof(float));
	}

	V3F_COPY(lbxgl_cam->fw, fnorm);
	fnorm[3]=V3F_DOT(lbxgl_cam->fw, fnorm)+1;

	for(i=0; i<n_xyz; i++)
	{
		Hull_LinePlaneIntersect(xyza+i*3, xyzb+i*3, fnorm, xyzc+i*3);
	}

	V3F_COPY(xyzc+a_tedge[0]*3, forg);
	pdglBegin(PDGL_TRIANGLES);
	for(i=0; i<n_tedge; i++)
	{
		i0=a_tedge[i*4+0];
		i1=a_tedge[i*4+1];

		pdglVertex3fv(xyzc+i0*3);
		pdglVertex3fv(xyzc+i1*3);
		pdglVertex3fv(forg);
	}
	pdglEnd();
}

LBXGL_API void LBXGL_Brush_DrawEdgeShadowVolume(
	float *a_xyz, int n_xyz,		//XYZ or XYZW
	float *a_norm, int n_norm,		//XYZW
	int *a_edge, int n_edge,		//v0 v1 n0 n1
	int *a_pts, int n_pts,			//xyz st
	int *a_face, int n_face,		//point count
	LBXGL_Light *light, float *lorg, float lrad, int flags)
{
	static float *xyza=NULL;
	static float *xyzb=NULL;
	static int *tedge=NULL;
	static int nxyz, ntedge;
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1, i2, i3, p, n, ne;

	if(!xyza || (nxyz<n_xyz))
	{
		if(xyza)
			{ free(xyza); free(xyzb); free(tedge); }
		nxyz=256;
		while(nxyz<=n_xyz)nxyz+=nxyz>>1;
		xyza=malloc(nxyz*3*sizeof(float));
		xyzb=malloc(nxyz*3*sizeof(float));

		ntedge=256;
		while(ntedge<=n_edge)ntedge+=ntedge>>1;
		tedge=malloc(ntedge*4*sizeof(int));
	}

	n=3;
	if(flags&LBXGL_SFL_XYZSTEP4)n=4;
	for(i=0; i<n_xyz; i++)
	{
		V3F_COPY(a_xyz+i*n, tv0);
		V3F_SUB(tv0, lorg, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 1, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 1000, xyzb+i*3);
	}

	ne=0;
	pdglBegin(PDGL_QUADS);
	for(i=0; i<n_edge; i++)
	{
		i2=a_edge[i*4+2]; i3=a_edge[i*4+3];
		nv0=a_norm+i2*4;
		nv1=a_norm+i3*4;

		f=V3F_NDOT(lorg, nv0);
		g=V3F_NDOT(lorg, nv1);

		if((f>0) && (g<=0))
		{
			i0=a_edge[i*4+0]; i1=a_edge[i*4+1];
			pdglVertex3fv(xyza+i1*3); pdglVertex3fv(xyza+i0*3);
			pdglVertex3fv(xyzb+i0*3); pdglVertex3fv(xyzb+i1*3);
			tedge[ne*4+0]=i0; tedge[ne*4+1]=i1;
			tedge[ne*4+2]=i2; tedge[ne*4+3]=i3;
			ne++;
		}

		if((g>0) && (f<=0))
		{
			i0=a_edge[i*4+0]; i1=a_edge[i*4+1];
			pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
			pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);
			tedge[ne*4+0]=i1; tedge[ne*4+1]=i0;
			tedge[ne*4+2]=i3; tedge[ne*4+3]=i2;
			ne++;
		}
	}
	pdglEnd();

	if(flags&LBXGL_SFL_VIEWCAP)
	{
		LBXGL_Brush_DrawEdgeShadowVolume_ViewCap(xyza, xyzb, n_xyz,
			a_norm, n_norm, tedge, ne,
			light, lorg, lrad, flags);
	}

	//drawing end-caps?
	if(!(flags&LBXGL_SFL_NOENDCAPS))
	{
		//have faces, so use them
		if(a_face)
		{
#if 1
			for(i=0; i<n_face; i++)
			{
				p=a_face[i*2+0];
				n=a_face[i*2+1];

				nv=a_norm+i*4;
				f=V3F_NDOT(lorg, nv);
				if(f>0)xyz=xyza; else xyz=xyzb;

				pdglBegin(PDGL_POLYGON);
				for(j=0; j<n; j++)
					pdglVertex3fv(xyz+a_pts[(p+j)*2+0]*3);
				pdglEnd();
			}
#endif
		}else
		{
			//otherwise, fake it
			V3F_ZERO(tv0);
			V3F_ZERO(tv1);
			for(i=0; i<ne; i++)
			{
				i0=tedge[i*4+0]; i1=tedge[i*4+1];
				V3F_ADD(tv0, xyza+i0, tv0);
				V3F_ADD(tv0, xyza+i1, tv0);
				V3F_ADD(tv1, xyzb+i0, tv1);
				V3F_ADD(tv1, xyzb+i1, tv1);
			}
			f=1.0/(ne*2);
			V3F_SCALE(tv0, f, tv0);
			V3F_SCALE(tv1, f, tv1);

			pdglBegin(PDGL_TRIANGLES);
			for(i=0; i<ne; i++)
			{
				i0=tedge[i*4+0]; i1=tedge[i*4+1];
			
				pdglVertex3fv(xyza+i1*3);
				pdglVertex3fv(xyza+i0*3);
				pdglVertex3fv(tv0);

				pdglVertex3fv(xyzb+i0*3);
				pdglVertex3fv(xyzb+i1*3);
				pdglVertex3fv(tv1);
			}
			pdglEnd();
		}
	}
}
