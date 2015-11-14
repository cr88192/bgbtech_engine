#include <lbxgl2.h>

#define VEC_SZ			1.0

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

LBXGL_API LBXGL_BrushWorld *lbxgl_brushmesh_world;	//AH:ignore

LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_mdl;	//AH:ignore
LBXGL_API LBXGL_MeshGroup *lbxgl_meshsel_grp;	//AH:ignore
LBXGL_API LBXGL_MeshPrim *lbxgl_meshsel_prim;	//AH:ignore
LBXGL_API int lbxgl_meshsel_face, lbxgl_meshsel_vec;	//AH:ignore
LBXGL_API int lbxgl_meshsel_multi;	//AH:ignore

LBXGL_API int lbxgl_brushmesh_mode;	//AH:ignore
LBXGL_API int lbxgl_brushmesh_cmd;	//AH:ignore
// LBXGL_API int lbxgl_brushmesh_loadsave;
// LBXGL_API char *lbxgl_brushmesh_file;

LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_clip;	//AH:ignore


// LBXGL_API int lbxgl_brushmesh_dirty;
// LBXGL_API int lbxgl_brushmesh_lutime;
// LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_undo[16];
// LBXGL_API LBXGL_MeshGroup *lbxgl_brushmesh_redo[16];


LBXGL_API LBXGL_MeshGroup *LBXGL_BrushMesh_GetModel()
	{ return(lbxgl_brushmesh_mdl); }

LBXGL_API void LBXGL_BrushMesh_SetDirty()
{
}

LBXGL_API void LBXGL_BrushMesh_AddUndo()
{
}

void lbxgl_brushmesh_draw_star(float *org)
{
	float v[3];
	int i;

	for(i=1; i<27; i++)
	{
		v[0]=((i/1)%3)?((((i/1)%3)==1)?1:-1):0;
		v[1]=((i/3)%3)?((((i/3)%3)==1)?1:-1):0;
		v[2]=((i/9)%3)?((((i/9)%3)==1)?1:-1):0;
		V3F_NORMALIZE(v, v);

		pdglVertex3f(org[0]-v[0]*VEC_SZ, org[1]-v[1]*VEC_SZ, org[2]-v[2]*VEC_SZ);
		pdglVertex3f(org[0]+v[0]*VEC_SZ, org[1]+v[1]*VEC_SZ, org[2]+v[2]*VEC_SZ);
	}
}


LBXGL_API void LBXGL_BrushMesh_DrawPrimEditOrtho(LBXGL_MeshPrim *prim)
{
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	float io[3], pt[4];
	float f, g;
	int n, p;
	int i0, i1, i2;
	int i, j, k;

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_TEXTURE_2D);
	pdglDisableLighting();
	glBlendFunc(GL_ONE, GL_ZERO);

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{

		for(i=0; i<prim->n_xyz; i++)
		{
			if(prim->fl_xyz[i]&128)continue;

			pdglColor4f(0, 0, 1, 1);

			V3F_COPY(prim->xyz+i*3, io);
			f=V3F_DOT(io, lbxgl_cam->mdir)-V3F_DOT(lbxgl_cam->morg, lbxgl_cam->mdir);
			V3F_ADDSCALE(lbxgl_cam->morg, lbxgl_cam->mdir, f, pt);
			f=V3F_DIST(pt, io);
			if(f<VEC_SZ)
				pdglColor4f(1, 0, 1, 1);

			if(prim->fl_xyz[i]&1)
				pdglColor4f(0, 1, 0, 1);

			pdglBegin(PDGL_LINES);
			lbxgl_brushmesh_draw_star(prim->xyz+i*3);
			pdglEnd();
		}
	}

	pdglColor4f(1, 1, 1, 1);
	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
	{
		i=LBXGL_Mesh_CheckLinePrim(prim, lbxgl_cam->morg, lbxgl_cam->mend);
		if(i)
			pdglColor4f(1, 0, 1, 1);

		if(prim->fl&1)
			pdglColor4f(0, 1, 0, 1);
	}


	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;

		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
		{
			j=LBXGL_Mesh_CheckLineFace(prim, i,
				lbxgl_cam->morg, lbxgl_cam->mend);
			if(j)
				pdglColor4f(1, 0, 1, 1);

			if(prim->fl_face[i]&1)
				pdglColor4f(0, 1, 0, 1);
		}

		pdglBegin(PDGL_LINE_LOOP);
		for(j=0; j<n; j++)
			pdglVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
		pdglEnd();

		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
			pdglColor4f(1, 1, 1, 1);
	}
}

LBXGL_API void LBXGL_BrushMesh_DrawPrimEdit(LBXGL_MeshPrim *prim)
{
	float v0[3], v1[3], v2[3], v3[3];
	float dv0[3], dv1[3], dv2[3];
	float io[3], pt[4];
	float f, g;
	int n, p;
	int i0, i1, i2;
	int i, j, k, fl;

	if(!prim->fl_xyz)prim->fl_xyz=gcalloc(prim->m_xyz);
	if(!prim->fl_st)prim->fl_st=gcalloc(prim->m_st);
	if(!prim->fl_pts)prim->fl_pts=gcalloc(prim->m_pts);
	if(!prim->fl_face)prim->fl_face=gcalloc(prim->m_face);

	if(prim->fl&128)return;

	if(lbxgl_cam->ortho)
	{
		LBXGL_BrushMesh_DrawPrimEditOrtho(prim);
		return;
	}

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_TEXTURE_2D);
//	pdglDisableLighting();
	glBlendFunc(GL_ONE, GL_ZERO);

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{

		for(i=0; i<prim->n_xyz; i++)
		{
			if(prim->fl_xyz[i]&128)continue;

			pdglColor4f(0, 0, 1, 1);

			V3F_COPY(prim->xyz+i*3, io);
			f=V3F_DOT(io, lbxgl_cam->mdir)-V3F_DOT(lbxgl_cam->morg, lbxgl_cam->mdir);
			V3F_ADDSCALE(lbxgl_cam->morg, lbxgl_cam->mdir, f, pt);
			f=V3F_DIST(pt, io);
			if(f<VEC_SZ)
				pdglColor4f(1, 0, 1, 1);

//			if(((prim==lbxgl_meshsel_prim) && (i==lbxgl_meshsel_vec)) ||
//				(prim->fl_xyz[i]&1))
//					pdglColor4f(0, 1, 0, 1);
			if(prim->fl_xyz[i]&1)
				pdglColor4f(0, 1, 0, 1);

			pdglBegin(PDGL_LINES);
			lbxgl_brushmesh_draw_star(prim->xyz+i*3);
			pdglEnd();
		}
		
		pdglColor4f(1, 1, 1, 1);
	}

	fl=LBXGL_Texture_GetImageFlags(prim->texnum);
//	fl|=LBXGL_TXFL_ALPHA;

	if(fl&LBXGL_TXFL_ALPHA)
		fl|=LBXGL_TXFL_WINDOW;

	LBXGL_Shader_Color4f(1, 1, 1, 1);
	if((lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT) ||
		(lbxgl_brushmesh_mode==LBXGL_EDMODE_TEXTURE))
	{
		i=LBXGL_Mesh_CheckLinePrim(prim, lbxgl_cam->morg, lbxgl_cam->mend);
		if(i)
		{
			LBXGL_Shader_Color4f(1, 0, 1, 1);
			fl&=~LBXGL_TXFL_ALPHA;
		}
		if(prim->fl&1)
		{
			LBXGL_Shader_Color4f(0, 1, 0, 1);
			fl&=~LBXGL_TXFL_ALPHA;
		}
	}

	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;

		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
		{
			j=LBXGL_Mesh_CheckLineFace(prim, i,
				lbxgl_cam->morg, lbxgl_cam->mend);
			if(j)
			{
				LBXGL_Shader_Color4f(1, 0, 1, 1);
				fl&=~LBXGL_TXFL_ALPHA;
			}

			if(prim->fl_face[i]&1)
			{
				LBXGL_Shader_Color4f(0, 1, 0, 1);
				fl&=~LBXGL_TXFL_ALPHA;
			}
		}

		if(!(fl&LBXGL_TXFL_ALPHA))
		{
			glBlendFunc(GL_DST_COLOR, GL_ZERO);

			pdglBegin(PDGL_POLYGON);
			for(j=0; j<n; j++)
				pdglVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
			pdglEnd();
		}

		if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
			LBXGL_Shader_Color4f(1, 1, 1, 1);
	}

#if 0
	if((fl&LBXGL_TXFL_ALPHA) || (fl&LBXGL_TXFL_WINDOW))
//	if(1)
	{
		pdglDisableLighting();
//		glBlendFunc(GL_ONE, GL_ZERO);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}else
	{
		glEnableLighting();
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
	}
#endif

#if 0
	if(prim->texnum>0)
	{
//		glEnable(GL_TEXTURE_2D);
//		glBindTexture(GL_TEXTURE_2D, prim->texnum);

		LBXGL_Shader_BindTexture(prim->texnum);
	}else
	{
		LBXGL_Shader_ClearTexture();
	}

	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;

		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		i0=prim->pts[(p+0)*2+0];
		i1=prim->pts[(p+1)*2+0];
		i2=prim->pts[(p+2)*2+0];

		V3F_COPY(prim->xyz+i0*3, v0);
		V3F_COPY(prim->xyz+i1*3, v1);
		V3F_COPY(prim->xyz+i2*3, v2);

		V3F_SUB(v0, v1, dv0);
		V3F_SUB(v2, v1, dv1);
		V3F_CROSS(dv1, dv0, dv2);
		V3F_NORMALIZE(dv2, dv2);

//		pdglNormal3fv(dv2);
//		pdglBegin(PDGL_POLYGON);

		if(!(fl&(LBXGL_TXFL_ALPHA|LBXGL_TXFL_WINDOW)))
		{
//			glEnableLighting();
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
		}


		if(fl&LBXGL_TXFL_ALPHA)
			continue;

		LBXGL_Shader_Normal3fv(dv2);
		LBXGL_Shader_BeginPoly();

		for(j=0; j<n; j++)
		{
//			pdglTexCoord2fv(prim->st+prim->pts[(p+j)*2+1]*2);
//			pdglVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);

			LBXGL_Shader_TexCoord2fv(prim->st+prim->pts[(p+j)*2+1]*2);
			LBXGL_Shader_Vertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
		}

//		pdglEnd();
		LBXGL_Shader_EndPoly();
	}
#endif

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
	{
		glDisable(GL_TEXTURE_2D);
//		pdglDisableLighting();
		glBlendFunc(GL_ONE, GL_ZERO);

		pdglColor4f(0, 0, 0, 1);
		pdglBegin(PDGL_LINES);
		for(i=0; i<prim->n_face; i++)
		{
			if(prim->fl_face[i]&128)continue;

			p=prim->face[i*2+0];
			n=prim->face[i*2+1];

			for(j=0; j<n; j++)
			{
				pdglVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
				pdglVertex3fv(prim->xyz+prim->pts[(p+(j+1)%n)*2+0]*3);
			}
		}
		pdglEnd();
	}

	if((lbxgl_brushmesh_mode==LBXGL_EDMODE_TEXTURE) && (prim==lbxgl_meshsel_prim) && (prim->texnum>=0))
	{
		glEnable(GL_TEXTURE_2D);
//		pdglDisableLighting();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, prim->texnum);
		pdglColor4f(1, 1, 1, 0.75);

		if(V3F_LEN(prim->tex_sdir)<0.001)
		{
			V3F_SET(prim->tex_sdir, 1, 0, 0);
			V3F_SET(prim->tex_tdir, 0, 0, 1);
			LBXGL_Mesh_CalcCenter(prim, prim->tex_org);
			prim->tex_org[1]=0;
		}

		if(prim->tex_mode==LBXGL_TEX_PLANAR)
		{
			V3F_ADDSCALE(prim->tex_org, prim->tex_sdir, -0.5, v0);
			V3F_ADDSCALE(prim->tex_org, prim->tex_sdir,  0.5, v1);
			V3F_ADDSCALE(prim->tex_org, prim->tex_sdir,  0.5, v2);
			V3F_ADDSCALE(prim->tex_org, prim->tex_sdir, -0.5, v3);
			V3F_ADDSCALE(v0, prim->tex_tdir, -0.5, v0);
			V3F_ADDSCALE(v1, prim->tex_tdir, -0.5, v1);
			V3F_ADDSCALE(v2, prim->tex_tdir, 0.5, v2);
			V3F_ADDSCALE(v3, prim->tex_tdir, 0.5, v3);

			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);	pdglVertex3fv(v0);
			pdglTexCoord2f(1, 0);	pdglVertex3fv(v1);
			pdglTexCoord2f(1, 1);	pdglVertex3fv(v2);
			pdglTexCoord2f(0, 1);	pdglVertex3fv(v3);
			pdglEnd();
		}

		if(prim->tex_mode==LBXGL_TEX_BOX)
		{
			f=V3F_NORMALIZE(prim->tex_sdir, dv0);
			g=V3F_NORMALIZE(prim->tex_tdir, dv1);
			V3F_CROSS(dv0, dv1, dv2);

			V3F_ADDSCALE(prim->tex_org, dv0, -0.5*f, v0);
			V3F_ADDSCALE(prim->tex_org, dv0,  0.5*f, v1);
			V3F_ADDSCALE(prim->tex_org, dv0,  0.5*f, v2);
			V3F_ADDSCALE(prim->tex_org, dv0, -0.5*f, v3);
			V3F_ADDSCALE(v0, dv1, -0.5*g, v0);
			V3F_ADDSCALE(v1, dv1, -0.5*g, v1);
			V3F_ADDSCALE(v2, dv1,  0.5*g, v2);
			V3F_ADDSCALE(v3, dv1,  0.5*g, v3);

			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);	pdglVertex3fv(v0);
			pdglTexCoord2f(1, 0);	pdglVertex3fv(v1);
			pdglTexCoord2f(1, 1);	pdglVertex3fv(v2);
			pdglTexCoord2f(0, 1);	pdglVertex3fv(v3);
			pdglEnd();

			V3F_ADDSCALE(prim->tex_org, dv0, -0.5*f, v0);
			V3F_ADDSCALE(prim->tex_org, dv0,  0.5*f, v1);
			V3F_ADDSCALE(prim->tex_org, dv0,  0.5*f, v2);
			V3F_ADDSCALE(prim->tex_org, dv0, -0.5*f, v3);
			V3F_ADDSCALE(v0, dv2, -0.5*f, v0);
			V3F_ADDSCALE(v1, dv2, -0.5*f, v1);
			V3F_ADDSCALE(v2, dv2,  0.5*f, v2);
			V3F_ADDSCALE(v3, dv2,  0.5*f, v3);

			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);	pdglVertex3fv(v0);
			pdglTexCoord2f(1, 0);	pdglVertex3fv(v1);
			pdglTexCoord2f(1, 1);	pdglVertex3fv(v2);
			pdglTexCoord2f(0, 1);	pdglVertex3fv(v3);
			pdglEnd();

			V3F_ADDSCALE(prim->tex_org, dv2, -0.5*f, v0);
			V3F_ADDSCALE(prim->tex_org, dv2,  0.5*f, v1);
			V3F_ADDSCALE(prim->tex_org, dv2,  0.5*f, v2);
			V3F_ADDSCALE(prim->tex_org, dv2, -0.5*f, v3);
			V3F_ADDSCALE(v0, dv1, -0.5*g, v0);
			V3F_ADDSCALE(v1, dv1, -0.5*g, v1);
			V3F_ADDSCALE(v2, dv1,  0.5*g, v2);
			V3F_ADDSCALE(v3, dv1,  0.5*g, v3);

			pdglBegin(PDGL_POLYGON);
			pdglTexCoord2f(0, 0);	pdglVertex3fv(v0);
			pdglTexCoord2f(1, 0);	pdglVertex3fv(v1);
			pdglTexCoord2f(1, 1);	pdglVertex3fv(v2);
			pdglTexCoord2f(0, 1);	pdglVertex3fv(v3);
			pdglEnd();
		}

		if(prim->tex_mode==LBXGL_TEX_SPHERICAL)
		{
			f=V3F_NORMALIZE(prim->tex_sdir, dv0);
			V3F_COPY(prim->tex_org, io);
			Prim3D_DrawTextureSphere(io[0], io[1], io[2], f/2,
				prim->texnum);
		}

		if(prim->tex_mode==LBXGL_TEX_CYLINDRICAL)
		{
			f=V3F_NORMALIZE(prim->tex_sdir, dv0);
			g=V3F_NORMALIZE(prim->tex_tdir, dv1);
			V3F_COPY(prim->tex_org, io);
			Prim3D_DrawTextureCylinder(io[0], io[1], io[2], g,
				f/2, f/2, prim->texnum);
		}
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);
	pdglColor4f(1, 1, 1, 1);
}

LBXGL_API void LBXGL_BrushMesh_DrawPrimEditAlpha(LBXGL_MeshPrim *prim)
{
	float v0[3], v1[3], v2[3], v3[3];
	float dv0[3], dv1[3], dv2[3];
	float io[3], pt[4];
	float f, g;
	int n, p;
	int i0, i1, i2;
	int i, j, k, fl;

	if(!prim)return;

	if(!prim->fl_xyz)prim->fl_xyz=gcalloc(prim->m_xyz);
	if(!prim->fl_st)prim->fl_st=gcalloc(prim->m_st);
	if(!prim->fl_pts)prim->fl_pts=gcalloc(prim->m_pts);
	if(!prim->fl_face)prim->fl_face=gcalloc(prim->m_face);

	if(prim->fl&128)return;

	if(lbxgl_cam->ortho)
		return;

	glDepthFunc(GL_LEQUAL);
//	glDisable(GL_TEXTURE_2D);

	fl=LBXGL_Texture_GetImageFlags(prim->texnum);
//	fl|=LBXGL_TXFL_ALPHA;

	if(!(fl&LBXGL_TXFL_ALPHA))
		return;

	pdglDisableLighting();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if(prim->texnum>0)
	{
		LBXGL_Shader_BindTexture(prim->texnum);
	}else
	{
		LBXGL_Shader_ClearTexture();
	}

	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;

		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		i0=prim->pts[(p+0)*2+0];
		i1=prim->pts[(p+1)*2+0];
		i2=prim->pts[(p+2)*2+0];

		V3F_COPY(prim->xyz+i0*3, v0);
		V3F_COPY(prim->xyz+i1*3, v1);
		V3F_COPY(prim->xyz+i2*3, v2);

		V3F_SUB(v0, v1, dv0);
		V3F_SUB(v2, v1, dv1);
		V3F_CROSS(dv1, dv0, dv2);
		V3F_NORMALIZE(dv2, dv2);

		LBXGL_Shader_Normal3fv(dv2);
		LBXGL_Shader_BeginPoly();

		for(j=0; j<n; j++)
		{
			LBXGL_Shader_TexCoord2fv(prim->st+prim->pts[(p+j)*2+1]*2);
			LBXGL_Shader_Vertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
		}

		LBXGL_Shader_EndPoly();
	}
}

LBXGL_API void LBXGL_BrushMesh_DrawGroupEdit(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_DrawGroupEdit(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_DrawPrimEdit(mcur);
		mcur=mcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_DrawPrimEditAlpha(mcur);
		mcur=mcur->next;
	}
}


LBXGL_API void LBXGL_BrushMesh_ClearSelPrim(LBXGL_MeshPrim *prim)
{
	int i;

	for(i=0; i<prim->n_xyz; i++)
		prim->fl_xyz[i]&=~1;
	for(i=0; i<prim->n_st; i++)
		prim->fl_st[i]&=~1;
	for(i=0; i<prim->n_pts; i++)
		prim->fl_pts[i]&=~1;
	for(i=0; i<prim->n_face; i++)
		prim->fl_face[i]&=~1;
	prim->fl&=~1;
}

LBXGL_API void LBXGL_BrushMesh_ClearSelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_ClearSelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_ClearSelPrim(mcur);
		mcur=mcur->next;
	}

	grp->fl&=~1;
}

LBXGL_API void LBXGL_BrushMesh_ClearOpSelPrim(LBXGL_MeshPrim *prim)
{
	int i;

	for(i=0; i<prim->n_xyz; i++)
		prim->fl_xyz[i]&=~14;
	for(i=0; i<prim->n_st; i++)
		prim->fl_st[i]&=~14;
	for(i=0; i<prim->n_pts; i++)
		prim->fl_pts[i]&=~14;
	for(i=0; i<prim->n_face; i++)
		prim->fl_face[i]&=~14;
	prim->fl&=~14;
}

LBXGL_API void LBXGL_BrushMesh_ClearOpSelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	grp->fl&=~14;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_ClearOpSelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_ClearOpSelPrim(mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_ClearSel()
{
	lbxgl_meshsel_grp=NULL; lbxgl_meshsel_prim=NULL;
	lbxgl_meshsel_face=-1; lbxgl_meshsel_vec=-1;
	LBXGL_BrushMesh_ClearSelGroup(lbxgl_brushmesh_mdl);
	lbxgl_meshsel_multi=0;
}


LBXGL_API void LBXGL_BrushMesh_SelPrim(LBXGL_MeshPrim *prim)
{
	float mins[3], maxs[3];
	float io[3], pt[4];
	float f;
	int i, j, k;

	if(prim->fl&128)return;

	if((lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT) || (lbxgl_brushmesh_mode==LBXGL_EDMODE_TEXTURE))
	{
		i=LBXGL_Mesh_CheckLinePrim(prim, lbxgl_cam->morg, lbxgl_cam->mend);
		if(i && PDGL_KeyDown(K_SHIFT))
		{
			lbxgl_meshsel_multi=1;
			prim->fl|=1;
			return;
		}

		if(i && !lbxgl_meshsel_prim)
		{
			lbxgl_meshsel_prim=prim;
			prim->fl|=1;
		}else if(prim==lbxgl_meshsel_prim)
		{
			lbxgl_meshsel_prim=NULL;
			prim->fl&=~1;
		}
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
	{
		if(PDGL_KeyDown(K_SHIFT))
		{
			lbxgl_meshsel_multi=1;
			for(i=0; i<prim->n_face; i++)
			{
				j=LBXGL_Mesh_CheckLineFace(
					prim, i, lbxgl_cam->morg, lbxgl_cam->mend);
				if(j)prim->fl_face[i]|=1;
			}
			return;
		}

		for(i=0; i<prim->n_face; i++)
		{
			j=LBXGL_Mesh_CheckLineFace(prim, i, lbxgl_cam->morg, lbxgl_cam->mend);
			if(j && (lbxgl_meshsel_face<0))
			{
				prim->fl_face[i]|=1;
				lbxgl_meshsel_prim=prim;
				lbxgl_meshsel_face=i;
			}else if((prim==lbxgl_meshsel_prim) && (i==lbxgl_meshsel_face))
			{
				prim->fl_face[i]&=~1;
				lbxgl_meshsel_prim=NULL;
				lbxgl_meshsel_face=-1;
			}
		}
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{
		if(PDGL_KeyDown(K_SHIFT))
		{
			lbxgl_meshsel_multi=1;
			for(i=0; i<prim->n_xyz; i++)
			{
				V3F_COPY(prim->xyz+i*3, io);

				f=V3F_DOT(io, lbxgl_cam->mdir)-V3F_DOT(lbxgl_cam->morg, lbxgl_cam->mdir);
				V3F_ADDSCALE(lbxgl_cam->morg, lbxgl_cam->mdir, f, pt);
				f=V3F_DIST(pt, io);
				if(f<VEC_SZ)prim->fl_xyz[i]|=1;
			}
			return;
		}

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_COPY(prim->xyz+i*3, io);

			f=V3F_DOT(io, lbxgl_cam->mdir)-V3F_DOT(lbxgl_cam->morg, lbxgl_cam->mdir);
			V3F_ADDSCALE(lbxgl_cam->morg, lbxgl_cam->mdir, f, pt);
			f=V3F_DIST(pt, io);
			if((f<VEC_SZ) && (lbxgl_meshsel_vec<0))
			{
				prim->fl_xyz[i]|=1;
				lbxgl_meshsel_prim=prim;
				lbxgl_meshsel_vec=i;
			}else if((prim==lbxgl_meshsel_prim) && (i==lbxgl_meshsel_vec))
			{
				prim->fl_xyz[i]&=~1;
				lbxgl_meshsel_prim=NULL;
				lbxgl_meshsel_vec=-1;
			}
		}
	}
}

LBXGL_API void LBXGL_BrushMesh_SelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_SelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_SelPrim(mcur);
		mcur=mcur->next;
	}
}


LBXGL_API int LBXGL_BrushMesh_PointInMBox(float *org)
{
	int mx, my, nx, ny;
	float io[3], pt[4];
	float f, g;

	mx=(lbxgl_state->mx<lbxgl_state->omx)?lbxgl_state->mx:lbxgl_state->omx;
	my=(lbxgl_state->my<lbxgl_state->omy)?lbxgl_state->my:lbxgl_state->omy;
	nx=(lbxgl_state->mx>lbxgl_state->omx)?lbxgl_state->mx:lbxgl_state->omx;
	ny=(lbxgl_state->my>lbxgl_state->omy)?lbxgl_state->my:lbxgl_state->omy;

	io[0]=V3F_DOT(org, lbxgl_cam->rt)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->rt);
	io[1]=-(V3F_DOT(org, lbxgl_cam->up)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->up));
	io[2]=V3F_DOT(org, lbxgl_cam->fw)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);

	if(!lbxgl_cam->ortho)
	{
		if(io[2]<1.0)return(0);
		io[0]/=io[2];
		io[1]/=io[2];
	}else
	{
		io[0]/=lbxgl_cam->width;
		io[1]/=lbxgl_cam->width;
	}

	pt[0]=mx/400.0;	pt[1]=my/400.0;
	pt[2]=nx/400.0;	pt[3]=ny/400.0;

	if(io[0]<pt[0])return(0);
	if(io[1]<pt[1])return(0);
	if(io[0]>pt[2])return(0);
	if(io[1]>pt[3])return(0);
	return(1);
}

LBXGL_API void LBXGL_BrushMesh_SelPrimBox(LBXGL_MeshPrim *prim)
{
	float mins[3], maxs[3];
	float io[3], pt[4];
	float f;
	int p, n;
	int i, j, k;

	if(prim->fl&128)return;

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
	{
		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_COPY(prim->xyz+i*3, io);
			j=LBXGL_BrushMesh_PointInMBox(io);
			if(j)
			{
				if(lbxgl_meshsel_prim)lbxgl_meshsel_multi=1;
				lbxgl_meshsel_prim=prim;
				prim->fl|=1;
			}
		}
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
	{
		for(i=0; i<prim->n_face; i++)
		{
			if(prim->fl_face[i]&128)continue;

			p=prim->face[i*2+0];
			n=prim->face[i*2+1];

			for(j=0; j<n; j++)
			{
				V3F_COPY(prim->xyz+prim->pts[(p+j)*2+0]*3, io);
				k=LBXGL_BrushMesh_PointInMBox(io);
				if(k)
				{
					if(lbxgl_meshsel_prim)lbxgl_meshsel_multi=1;
					lbxgl_meshsel_prim=prim;
					lbxgl_meshsel_face=i;
					prim->fl_face[i]|=1;
					break;
				}
			}
		}
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{
		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_COPY(prim->xyz+i*3, io);
			j=LBXGL_BrushMesh_PointInMBox(io);
			if(j)
			{
				if(lbxgl_meshsel_prim)lbxgl_meshsel_multi=1;
				lbxgl_meshsel_prim=prim;
				lbxgl_meshsel_vec=i;
				prim->fl_xyz[i]|=1;
			}
		}
	}
}

LBXGL_API void LBXGL_BrushMesh_SelGroupBox(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_SelGroupBox(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_SelPrimBox(mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_ExtrudeFace(LBXGL_MeshPrim *prim, int fn)
{
	int avn[16], ast[16];
	int i, j, p, n, p1;

	if(prim->fl_face[fn]&128)return;

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
	{
		avn[i]=LBXGL_Mesh_CopyVertex(prim, prim->pts[(p+i)*2+0]);
		ast[i]=LBXGL_Mesh_CopyST(prim, prim->pts[(p+i)*2+1]);
	}

	for(i=0; i<n; i++)
	{
		p1=LBXGL_Mesh_AllocPTS(prim, 4);
		prim->pts[(p1+0)*2+0]=prim->pts[(p+i)*2+0];
		prim->pts[(p1+1)*2+0]=prim->pts[(p+(i+1)%n)*2+0];
		prim->pts[(p1+2)*2+0]=avn[(i+1)%n];
		prim->pts[(p1+3)*2+0]=avn[i];

		prim->pts[(p1+0)*2+1]=prim->pts[(p+i)*2+1];
		prim->pts[(p1+1)*2+1]=prim->pts[(p+(i+1)%n)*2+1];
		prim->pts[(p1+2)*2+1]=ast[(i+1)%n];
		prim->pts[(p1+3)*2+1]=ast[i];

		j=LBXGL_Mesh_AllocFace(prim);
		prim->face[j*2+0]=p1;
		prim->face[j*2+1]=4;
	}

	for(i=0; i<n; i++)
	{
		prim->pts[(p+i)*2+0]=avn[i];
		prim->pts[(p+i)*2+1]=ast[i];
	}
}

LBXGL_API void LBXGL_BrushMesh_DivideFace(LBXGL_MeshPrim *prim, int fn)
{
	int avn[16], ast[16];
	float io[3], so[2], pt[3], spt[2];
	int i0, i1, i2;
	int j0, j1, j2;

	int i, j, p, n, p1;

	if(prim->fl_face[fn]&128)return;

	//Calc Face Center
	V3F_ZERO(io);
	V2F_ZERO(so);
	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];
	for(i=0; i<n; i++)
	{
		V3F_ADD(io, prim->xyz+prim->pts[(p+i)*2+0]*3, io);
		V2F_ADD(so, prim->st+prim->pts[(p+i)*2+1]*2, so);
	}
	V3F_SCALE(io, 1.0/n, io);
	V2F_SCALE(so, 1.0/n, so);

	for(i=0; i<n; i++)
	{
		V3F_SCALEADDSCALE(io, 0.5,
			prim->xyz+prim->pts[(p+i)*2+0]*3, 0.5, pt);
		V2F_SCALEADDSCALE(so, 0.5,
			prim->st+prim->pts[(p+i)*2+1]*2, 0.5, spt);
		avn[i]=LBXGL_Mesh_IndexXYZ(prim, pt);
		ast[i]=LBXGL_Mesh_IndexST(prim, spt);
	}


	for(i=0; i<n; i++)
	{
		p1=LBXGL_Mesh_AllocPTS(prim, 4);
		prim->pts[(p1+0)*2+0]=prim->pts[(p+i)*2+0];
		prim->pts[(p1+1)*2+0]=prim->pts[(p+(i+1)%n)*2+0];
		prim->pts[(p1+2)*2+0]=avn[(i+1)%n];
		prim->pts[(p1+3)*2+0]=avn[i];

		prim->pts[(p1+0)*2+1]=prim->pts[(p+i)*2+1];
		prim->pts[(p1+1)*2+1]=prim->pts[(p+(i+1)%n)*2+1];
		prim->pts[(p1+2)*2+1]=ast[(i+1)%n];
		prim->pts[(p1+3)*2+1]=ast[i];

		j=LBXGL_Mesh_AllocFace(prim);
		prim->face[j*2+0]=p1;
		prim->face[j*2+1]=4;
	}

	for(i=0; i<n; i++)
	{
		prim->pts[(p+i)*2+0]=avn[i];
		prim->pts[(p+i)*2+1]=ast[i];
	}
}

LBXGL_API void LBXGL_BrushMesh_FlipFace(LBXGL_MeshPrim *prim, int fn)
{
	int avn[16], ast[16];
	int i, j, p, n;

	if(prim->fl_face[fn]&128)return;

	p=prim->face[fn*2+0];
	n=prim->face[fn*2+1];

	for(i=0; i<n; i++)
	{
		avn[i]=prim->pts[(p+i)*2+0];
		ast[i]=prim->pts[(p+i)*2+1];
	}

	for(i=0; i<n; i++)
	{
		prim->pts[(p+i)*2+0]=avn[n-(i+1)];
		prim->pts[(p+i)*2+1]=ast[n-(i+1)];
	}
}

LBXGL_API void LBXGL_BrushMesh_CreateFace(LBXGL_MeshPrim *prim)
{
	int avn[16], ast[16];
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	float io[3];
	float f, g, d, a, b;
	int i0, i1, i2;
	int i, j, k, p, n;

	if(!prim)return;

	n=0; V3F_ZERO(io);
	for(i=0; i<prim->n_xyz; i++)
		if(prim->fl_xyz[i]&1)
	{
		if(n>=16)break;
		V3F_ADD(io, prim->xyz+i*3, io);
		avn[n++]=i;
	}

	if(n<3)return;

	V3F_SCALE(io, 1.0/n, io);


	V3F_COPY(prim->xyz+avn[0]*3, v0);
	V3F_COPY(prim->xyz+avn[1]*3, v1);
	V3F_COPY(prim->xyz+avn[2]*3, v2);

	V3F_SUB(v0, v1, dv0);
	V3F_SUB(v2, v1, dv1);
	V3F_CROSS(dv1, dv0, dv2);
	V3F_NORMALIZE(dv2, dv2);
	d=V3F_DOT(v0, dv2);

	V3F_NORMALIZE(dv0, dv0);
	V3F_CROSS(dv0, dv2, dv1);

	for(i=0; i<n; i++)
	{
		f=V3F_DOT(prim->xyz+avn[i]*3, dv0)-V3F_DOT(io, dv0);
		g=V3F_DOT(prim->xyz+avn[i]*3, dv1)-V3F_DOT(io, dv1);
		a=atan2(g, f);

		for(j=(i+1); j<n; j++)
		{
			f=V3F_DOT(prim->xyz+avn[j]*3, dv0)-V3F_DOT(io, dv0);
			g=V3F_DOT(prim->xyz+avn[j]*3, dv1)-V3F_DOT(io, dv1);
			b=atan2(g, f);
			if(b<a) { k=avn[i]; avn[i]=avn[j]; avn[j]=k; a=b; }
		}
	}

	p=LBXGL_Mesh_AllocPTS(prim, n);
	for(i=0; i<n; i++)
	{
		prim->pts[(p+i)*2+0]=avn[i];
		prim->pts[(p+i)*2+1]=0;
	}

	i=LBXGL_Mesh_AllocFace(prim);
	prim->face[i*2+0]=p;
	prim->face[i*2+1]=n;
}

LBXGL_API LBXGL_MeshPrim *LBXGL_BrushMesh_CreateCube()
{
/*	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	}; */
	static int vtxb[6*4]=
	{
		0, 2, 3, 1,
		5, 7, 6, 4,
		1, 3, 7, 5,
		4, 6, 2, 0,
		4, 0, 1, 5,
		2, 6, 7, 3
	};
	static int rov=0;

	char tb[64];
	LBXGL_MeshPrim *prim;
	float org[3];
	int i, j;

	while(1)
	{
		sprintf(tb, "cube-%d", rov++);
		prim=LBXGL_Mesh_LookupPrimGroup(lbxgl_brushmesh_mdl, tb);
		if(!prim)break;
	}

	prim=LBXGL_Mesh_CreatePrim(lbxgl_brushmesh_mdl, tb);
//	prim=LBXGL_Mesh_CreatePrim(lbxgl_brushmesh_mdl, "cube");
	V3F_ADDSCALE(lbxgl_cam->org, lbxgl_cam->fw, 24, org);

	if(lbxgl_cam->ortho) { V3F_COPY(lbxgl_cam->org, org); }

	printf("Mesh Create Cube\n");
	printf("\t(%g %g %g)\n", org[0], org[1], org[2]);

	for(i=0; i<8; i++)for(j=0; j<3; j++)
		prim->xyz[i*3+j]=org[j]+((i&(1<<j))?8.0:-8.0);

	prim->st[0*2+0]=0;	prim->st[0*2+1]=0;
	prim->st[1*2+0]=1;	prim->st[1*2+1]=0;
	prim->st[2*2+0]=1;	prim->st[2*2+1]=1;
	prim->st[3*2+0]=0;	prim->st[3*2+1]=1;

	for(i=0; i<6; i++)for(j=0; j<4; j++)
	{
		prim->pts[(i*4+j)*2+0]=vtxb[i*4+j];
		prim->pts[(i*4+j)*2+1]=j;
	}

	for(i=0; i<6; i++)
	{
		prim->face[i*2+0]=i*4;
		prim->face[i*2+1]=4;
	}

	prim->n_xyz=8;
	prim->n_st=4;
	prim->n_pts=24;
	prim->n_face=6;

	V3F_COPY(org, prim->tex_org);
	V3F_SET(prim->tex_sdir, 1, 0, 0);
	V3F_SET(prim->tex_tdir, 0, 0, 1);
	prim->tex_mode=LBXGL_TEX_BOX;
	
	return(prim);
}

LBXGL_API void LBXGL_BrushMesh_CalcTranslate(float *org, float *offs)
{
	float io[3], pt[4];
	float f;

	f=V3F_DOT(org, lbxgl_cam->mdir)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->mdir);
	V3F_SUB(lbxgl_cam->morg, lbxgl_cam->lmorg, io);
	V3F_SUB(lbxgl_cam->org, lbxgl_cam->lorg, pt);
	V3F_SUB(io, pt, io);
//	V3F_SUB(lbxgl_cam->mdir, lbxgl_cam->lmdir, io);
	if(!lbxgl_cam->ortho) { V3F_SCALE(io, f, io); }
	V3F_ADD(io, pt, offs);
}

LBXGL_API float LBXGL_BrushMesh_CalcRotate(float *org)
{
	float io[3], pt[4];
	float f, g;

	io[0]=V3F_DOT(org, lbxgl_cam->rt)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->rt);
	io[1]=V3F_DOT(org, lbxgl_cam->up)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->up);
	io[2]=V3F_DOT(org, lbxgl_cam->fw)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);

	if(!lbxgl_cam->ortho)
	{
		if(io[2]<1)return(0.0);
		io[0]/=io[2]; io[1]/=io[2];
	}else
	{
		io[0]/=lbxgl_cam->width;
		io[1]/=lbxgl_cam->width;
	}

	pt[0]=lbxgl_state->mx/400.0;	pt[1]=-lbxgl_state->my/400.0;
	pt[2]=lbxgl_state->lmx/400.0;	pt[3]=-lbxgl_state->lmy/400.0;
	f=atan2(pt[1]-io[1], pt[0]-io[0]);
	g=atan2(pt[3]-io[1], pt[2]-io[0]);
	return(f-g);
}

LBXGL_API float LBXGL_BrushMesh_CalcScale(float *org)
{
	float io[3], pt[4];
	float f, g;

	io[0]=V3F_DOT(org, lbxgl_cam->rt)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->rt);
	io[1]=V3F_DOT(org, lbxgl_cam->up)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->up);
	io[2]=V3F_DOT(org, lbxgl_cam->fw)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);

	if(!lbxgl_cam->ortho)
	{
		if(io[2]<1)return(1.0);
		io[0]/=io[2]; io[1]/=io[2];
	}else
	{
		io[0]/=lbxgl_cam->width;
		io[1]/=lbxgl_cam->width;
	}

	pt[0]=lbxgl_state->mx/400.0;	pt[1]=-lbxgl_state->my/400.0;
	pt[2]=lbxgl_state->lmx/400.0;	pt[3]=-lbxgl_state->lmy/400.0;
	f=sqrt(SQR(pt[0]-io[0])+SQR(pt[1]-io[1]));
	g=sqrt(SQR(pt[2]-io[0])+SQR(pt[3]-io[1]));
	if(g<0.01)return(1.0);
	f/=g;

	return(f);
}

LBXGL_API void LBXGL_BrushMesh_CalcScaleHV(float *org, float *sc)
{
	float io[3], pt[4];
	float f, g;

	sc[0]=1.0;
	sc[1]=1.0;

	io[0]=V3F_DOT(org, lbxgl_cam->rt)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->rt);
	io[1]=V3F_DOT(org, lbxgl_cam->up)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->up);
	io[2]=V3F_DOT(org, lbxgl_cam->fw)-V3F_DOT(lbxgl_cam->org, lbxgl_cam->fw);

	if(!lbxgl_cam->ortho)
	{
		if(io[2]<1)return;
		io[0]/=io[2]; io[1]/=io[2];
	}else
	{
		io[0]/=lbxgl_cam->width;
		io[1]/=lbxgl_cam->width;
	}

	pt[0]=lbxgl_state->mx/400.0;	pt[1]=-lbxgl_state->my/400.0;
	pt[2]=lbxgl_state->lmx/400.0;	pt[3]=-lbxgl_state->lmy/400.0;

//	f=sqrt(SQR(pt[0]-io[0])+SQR(pt[1]-io[1]));
//	g=sqrt(SQR(pt[2]-io[0])+SQR(pt[3]-io[1]));
//	if(g<0.01)return(1.0);
//	f/=g;

	f=fabs(pt[2]-io[0]);
	g=fabs(pt[3]-io[1]);

	if(f>0.01)sc[0]=fabs(pt[0]-io[0])/f;
	if(g>0.01)sc[1]=fabs(pt[1]-io[1])/g;
}


// void LBXGL_Mesh_RotatePointOrgAxisAngle(
//	float *o, float *n, float th, float *pt);

LBXGL_API void LBXGL_BrushMesh_CmdSelPrim(LBXGL_MeshPrim *prim, int cmd)
{
	float mins[3], maxs[3];
	float io[3], pt[4], hv[2];
	float f, g, h;
	int i, j, k;

	if(prim!=lbxgl_meshsel_prim)return;
	if(prim->fl&128)return;
	LBXGL_BrushMesh_SetDirty();

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
		switch(cmd)
	{
	case LBXGL_EDCMD_TRANS:
		LBXGL_Mesh_CalcCenter(prim, io);
		LBXGL_BrushMesh_CalcTranslate(io, io);
		for(i=0; i<prim->n_xyz; i++)
			{ V3F_ADD(prim->xyz+i*3, io, prim->xyz+i*3); }
		V3F_ADD(prim->tex_org, io, prim->tex_org);
		break;

	case LBXGL_EDCMD_ROT:
		LBXGL_Mesh_CalcCenter(prim, io);
		f=LBXGL_BrushMesh_CalcRotate(io);
		for(i=0; i<prim->n_xyz; i++)
			LBXGL_Mesh_RotatePointOrgAxisAngle(
				io, lbxgl_cam->fw, -f, prim->xyz+i*3);

		LBXGL_Mesh_RotatePointOrgAxisAngle(
			io, lbxgl_cam->fw, -f, prim->tex_org);
		LBXGL_Mesh_RotatePointAxisAngle(lbxgl_cam->fw, -f, prim->tex_sdir);
		LBXGL_Mesh_RotatePointAxisAngle(lbxgl_cam->fw, -f, prim->tex_tdir);
		break;
	case LBXGL_EDCMD_SCALE:
		LBXGL_Mesh_CalcCenter(prim, io);
		f=LBXGL_BrushMesh_CalcScale(io);

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_SUB(prim->xyz+i*3, io, pt);
			V3F_ADDSCALE(io, pt, f, prim->xyz+i*3);
		}
		V3F_SCALE(prim->tex_sdir, f, prim->tex_sdir);
		V3F_SCALE(prim->tex_tdir, f, prim->tex_tdir);

		break;

	case LBXGL_EDCMD_SCALE2:
		LBXGL_Mesh_CalcCenter(prim, io);
		LBXGL_BrushMesh_CalcScaleHV(io, hv);

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_SUB(prim->xyz+i*3, io, pt);
			f=V3F_DOT(pt, lbxgl_cam->rt);
			V3F_ADDSCALE(pt, lbxgl_cam->rt, -f, pt);
			g=V3F_DOT(pt, lbxgl_cam->up);
			V3F_ADDSCALE(pt, lbxgl_cam->up, -g, pt);

			V3F_ADDSCALE(pt, lbxgl_cam->rt, f*hv[0], pt);
			V3F_ADDSCALE(pt, lbxgl_cam->up, g*hv[1], pt);

			V3F_ADD(io, pt, prim->xyz+i*3);
		}
		break;

	case LBXGL_EDCMD_DELETE:
		LBXGL_BrushMesh_AddUndo();
		prim->fl&=~1;
		prim->fl|=128;
		lbxgl_meshsel_prim=NULL;
		break;

	case LBXGL_EDCMD_SUBDIVIDE:
		LBXGL_BrushMesh_AddUndo();
		LBXGL_Mesh_SubdivideMesh(lbxgl_meshsel_prim);
		LBXGL_BrushMesh_ClearSel();
		break;

	default:
		break;
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
	{
		if(lbxgl_meshsel_face<0)return;
		i=lbxgl_meshsel_face;

		switch(cmd)
		{
		case LBXGL_EDCMD_TRANS:
			LBXGL_Mesh_CalcFaceCenter(prim, i, io);
			LBXGL_BrushMesh_CalcTranslate(io, io);
			LBXGL_Mesh_TranslateFace(prim, i, io);
			break;
		case LBXGL_EDCMD_ROT:
			LBXGL_Mesh_CalcFaceCenter(prim, i, io);
			f=LBXGL_BrushMesh_CalcRotate(io);
			LBXGL_Mesh_RotateFace(prim, i, io, lbxgl_cam->fw, -f);
			break;
		case LBXGL_EDCMD_SCALE:
			LBXGL_Mesh_CalcFaceCenter(prim, i, io);
			f=LBXGL_BrushMesh_CalcScale(io);
			LBXGL_Mesh_ScaleFace(prim, i, io, f);
			break;

		case LBXGL_EDCMD_DELETE:
			LBXGL_BrushMesh_AddUndo();
			LBXGL_Mesh_DeleteFace(prim, i);
			break;
		case LBXGL_EDCMD_EXTRUDE:
			LBXGL_BrushMesh_AddUndo();
			LBXGL_BrushMesh_ExtrudeFace(prim, i);
			break;
		case LBXGL_EDCMD_SUBDIVIDE:
			LBXGL_BrushMesh_AddUndo();
			LBXGL_BrushMesh_DivideFace(prim, i);
			break;
		case LBXGL_EDCMD_INVERT:
			LBXGL_BrushMesh_AddUndo();
			LBXGL_BrushMesh_FlipFace(prim, i);
			break;

		default:
			break;
		}

		LBXGL_BrushMesh_ReprojectPrim(prim);
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{
		if(lbxgl_meshsel_vec<0)return;
		i=lbxgl_meshsel_vec;

		switch(cmd)
		{
		case LBXGL_EDCMD_TRANS:
			LBXGL_BrushMesh_CalcTranslate(prim->xyz+i*3, io);
			V3F_ADD(prim->xyz+i*3, io, prim->xyz+i*3);
			break;

		case LBXGL_EDCMD_DELETE:
			LBXGL_BrushMesh_AddUndo();
			LBXGL_Mesh_DeleteVertex(prim, i);
			break;

		default:
			break;
		}

		LBXGL_BrushMesh_ReprojectPrim(prim);
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_TEXTURE)
	{
		switch(cmd)
		{
		case LBXGL_EDCMD_TRANS:
			LBXGL_BrushMesh_CalcTranslate(prim->tex_org, io);
			V3F_ADD(prim->tex_org, io, prim->tex_org);
			break;
		case LBXGL_EDCMD_ROT:
			V3F_COPY(prim->tex_org, io);
			f=LBXGL_BrushMesh_CalcRotate(io);
			LBXGL_Mesh_RotatePointAxisAngle(
				lbxgl_cam->fw, -f, prim->tex_sdir);
			LBXGL_Mesh_RotatePointAxisAngle(
				lbxgl_cam->fw, -f, prim->tex_tdir);
			break;
		case LBXGL_EDCMD_SCALE:
			V3F_COPY(prim->tex_org, io);
			f=LBXGL_BrushMesh_CalcScale(io);
			V3F_SCALE(prim->tex_sdir, f, prim->tex_sdir);
			V3F_SCALE(prim->tex_tdir, f, prim->tex_tdir);
			break;

		case LBXGL_EDCMD_SCALE2:
			V3F_COPY(prim->tex_org, io);
			LBXGL_BrushMesh_CalcScaleHV(io, hv);

			f=V3F_DOT(prim->tex_sdir, lbxgl_cam->rt);
			g=V3F_DOT(prim->tex_sdir, lbxgl_cam->up);
			h=hv[0]*f+hv[1]*g;
			V3F_SCALE(prim->tex_sdir, h, prim->tex_sdir);

			f=V3F_DOT(prim->tex_tdir, lbxgl_cam->rt);
			g=V3F_DOT(prim->tex_tdir, lbxgl_cam->up);
			h=hv[0]*f+hv[1]*g;
			V3F_SCALE(prim->tex_tdir, h, prim->tex_tdir);
			break;

		default:
			break;
		}

		LBXGL_BrushMesh_ReprojectPrim(prim);
	}
}

LBXGL_API void LBXGL_BrushMesh_CmdSelGroup(LBXGL_MeshGroup *grp, int cmd)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_CmdSelGroup(gcur, cmd);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_CmdSelPrim(mcur, cmd);
		mcur=mcur->next;
	}
}


LBXGL_API void LBXGL_BrushMesh_BoxSelPrim(LBXGL_MeshPrim *prim,
	float *mins, float *maxs)
{
	int i, j, k;

	if(prim->fl&128)return;

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
	{
		if(!(prim->fl&1))return;

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_MIN(mins, prim->xyz+i*3, mins);
			V3F_MAX(maxs, prim->xyz+i*3, maxs);
		}
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
	{
		for(i=0; i<prim->n_face; i++)
			if(prim->fl_face[i]&1)
				LBXGL_Mesh_BoxFace(prim, i, mins, maxs);
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{
		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&1)
		{
			V3F_MIN(mins, prim->xyz+i*3, mins);
			V3F_MAX(maxs, prim->xyz+i*3, maxs);
		}
	}
}

LBXGL_API void LBXGL_BrushMesh_BoxSelGroup(LBXGL_MeshGroup *grp, float *mins, float *maxs)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_BoxSelGroup(gcur, mins, maxs);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_BoxSelPrim(mcur, mins, maxs);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_OpSelPrim(LBXGL_MeshPrim *prim)
{
	int p, n;
	int i, j, k;

	if(prim->fl&128)return;

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
	{
		if(!(prim->fl&1))return;

		for(i=0; i<prim->n_xyz; i++)
			prim->fl_xyz[i]|=2;
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
	{
		for(i=0; i<prim->n_face; i++)
			if(prim->fl_face[i]&1)
		{
			prim->fl_face[i]|=2;

			p=prim->face[i*2+0];
			n=prim->face[i*2+1];
			for(j=0; j<n; j++)
			{
				k=prim->pts[(p+j)*2+0];
				prim->fl_xyz[k]|=2;
				prim->fl_pts[p+j]|=2;
			}
		}
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{
		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&1)
				prim->fl_xyz[i]|=2;
	}
}

LBXGL_API void LBXGL_BrushMesh_OpSelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_OpSelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_OpSelPrim(mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_CmdOpSelPrim(LBXGL_MeshPrim *prim, int cmd, float *org)
{
	float io[3], pt[4], hv[2];
	float f, g;
	int i, j, k;

	if(prim->fl&128)return;
	LBXGL_BrushMesh_SetDirty();

	switch(cmd)
	{
	case LBXGL_EDCMD_TRANS:
		LBXGL_BrushMesh_CalcTranslate(org, io);
		for(i=0; i<prim->n_xyz; i++)if(prim->fl_xyz[i]&2)
			{ V3F_ADD(prim->xyz+i*3, io, prim->xyz+i*3); }
		break;

	case LBXGL_EDCMD_ROT:
		f=LBXGL_BrushMesh_CalcRotate(org);
		for(i=0; i<prim->n_xyz; i++)if(prim->fl_xyz[i]&2)
			LBXGL_Mesh_RotatePointOrgAxisAngle(
				org, lbxgl_cam->fw, -f, prim->xyz+i*3);
		break;
	case LBXGL_EDCMD_SCALE:
		f=LBXGL_BrushMesh_CalcScale(org);

		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&2)
		{
			V3F_SUB(prim->xyz+i*3, org, pt);
			V3F_ADDSCALE(org, pt, f, prim->xyz+i*3);
		}
		break;

	case LBXGL_EDCMD_SCALE2:
		LBXGL_BrushMesh_CalcScaleHV(org, hv);

		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&2)
		{
			V3F_SUB(prim->xyz+i*3, org, pt);
			f=V3F_DOT(pt, lbxgl_cam->rt);
			V3F_ADDSCALE(pt, lbxgl_cam->rt, -f, pt);
			g=V3F_DOT(pt, lbxgl_cam->up);
			V3F_ADDSCALE(pt, lbxgl_cam->up, -g, pt);

			V3F_ADDSCALE(pt, lbxgl_cam->rt, f*hv[0], pt);
			V3F_ADDSCALE(pt, lbxgl_cam->up, g*hv[1], pt);

			V3F_ADD(org, pt, prim->xyz+i*3);
		}
		break;
	default:
		break;
	}
}

LBXGL_API void LBXGL_BrushMesh_CmdOpSelGroup(LBXGL_MeshGroup *grp, int cmd, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_CmdOpSelGroup(gcur, cmd, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_CmdOpSelPrim(mcur, cmd, org);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_CmdMSelPrim(LBXGL_MeshPrim *prim, int cmd, float *org)
{
	float io[3], pt[4];
	float f, g;
	int i, j, k;

	if(prim->fl&128)return;
	LBXGL_BrushMesh_SetDirty();

//	printf("cmd multi %d %d\n", lbxgl_brushmesh_mode, cmd);


	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
		switch(cmd)
	{
	case LBXGL_EDCMD_SUBDIVIDE:
//		printf("multi divide\n");
		for(i=0; i<prim->n_face; i++)
			if(prim->fl_face[i]&1)
				LBXGL_BrushMesh_DivideFace(prim, i);
		break;
	default:
		break;
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
		switch(cmd)
	{
	case LBXGL_EDCMD_MERGE:
		V3F_ZERO(io); j=0;
		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&1)
				{ V3F_ADD(io, prim->xyz+i*3, io); j++; }
		V3F_SCALE(io, 1.0/j, io);
		j=LBXGL_Mesh_IndexXYZ(prim, io);
		for(i=0; i<prim->n_pts; i++)
			if(prim->fl_xyz[prim->pts[i*2+0]]&1)
				prim->pts[i*2+0]=j;
		for(i=0; i<prim->n_xyz; i++)
			if((prim->fl_xyz[i]&1) && (i!=j))
				{ prim->fl_xyz[i]&=~1; prim->fl_xyz[i]|=128; }
		LBXGL_Mesh_CleanFaceEdges(prim);
		break;
	default:
		break;
	}
}

LBXGL_API void LBXGL_BrushMesh_CmdMSelGroup(LBXGL_MeshGroup *grp, int cmd, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_CmdMSelGroup(gcur, cmd, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_CmdMSelPrim(mcur, cmd, org);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_CopyMSelPrim(
	LBXGL_MeshGroup *gtmp, LBXGL_MeshPrim *prim, int cmd, float *org)
{
	LBXGL_MeshPrim *tmp;
	float *fa, *fb;
	float io[3];
	int p, p1, n;
	int i, j, k;

	i=0;
	switch(lbxgl_brushmesh_mode)
	{
	case LBXGL_EDMODE_OBJECT:
		if(prim->fl&1)i=1;
		break;
	case LBXGL_EDMODE_FACE:
		for(j=0; j<prim->n_face; j++)
			if(prim->fl_face[j]&1)i=1;
		break;
	case LBXGL_EDMODE_VERTEX:
		for(j=0; j<prim->n_xyz; j++)
			if(prim->fl_xyz[j]&1)i=1;
		break;
	default:
		break;
	}

	if(!i)return;

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT)
	{
		tmp=LBXGL_Mesh_ClonePrim(prim);
		tmp->next=gtmp->prim;
		gtmp->prim=tmp;
		tmp->fl&=~1;

		for(i=0; i<tmp->n_xyz; i++)
		{
			if(tmp->fl_xyz[i]&128)continue;
			fa=tmp->xyz+i*3;
			V3F_SUB(fa, org, fa);
		}
		return;
	}

	tmp=LBXGL_Mesh_CreatePrim(gtmp, prim->name);
	if(prim->tex)
	{
		tmp->tex=dystrdup(prim->tex);
		tmp->texnum=prim->texnum;
		V3F_COPY(prim->tex_org, tmp->tex_org);
		V3F_COPY(prim->tex_sdir, tmp->tex_sdir);
		V3F_COPY(prim->tex_tdir, tmp->tex_tdir);
		tmp->tex_mode=prim->tex_mode;
		tmp->tex_fl=prim->tex_fl;
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_FACE)
	{
		for(i=0; i<prim->n_face; i++)
		{
			if(prim->fl_face[i]&128)continue;
			if(!(prim->fl_face[i]&1))continue;
			p=prim->face[i*2+0];
			n=prim->face[i*2+1];

			p1=LBXGL_Mesh_AllocPTS(tmp, n);
			for(j=0; j<n; j++)
			{
				fa=prim->xyz+prim->pts[(p+j)*2+0]*3;
				fb=prim->st+prim->pts[(p+j)*2+1]*2;
				V3F_SUB(fa, org, io);

				tmp->pts[(p1+j)*2+0]=LBXGL_Mesh_IndexXYZ(tmp, io);
				tmp->pts[(p1+j)*2+1]=LBXGL_Mesh_IndexST(tmp, fb);
			}

			j=LBXGL_Mesh_AllocFace(tmp);
			tmp->face[j*2+0]=p1;
			tmp->face[j*2+1]=n;
		}
		return;
	}

	if(lbxgl_brushmesh_mode==LBXGL_EDMODE_VERTEX)
	{
		for(i=0; i<prim->n_xyz; i++)
		{
			if(prim->fl_xyz[i]&128)continue;
			if(!(prim->fl_xyz[i]&1))continue;

			fa=prim->xyz+i*3;
			V3F_SUB(fa, org, io);
			LBXGL_Mesh_IndexXYZ(tmp, io);
		}
		return;
	}
}

LBXGL_API void LBXGL_BrushMesh_CopyMSelGroup(
	LBXGL_MeshGroup *gtmp, LBXGL_MeshGroup *grp, int cmd, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_CopyMSelGroup(gtmp, gcur, cmd, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_CopyMSelPrim(gtmp, mcur, cmd, org);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_InsertPrim(LBXGL_MeshGroup *gtmp, LBXGL_MeshPrim *prim, float *org)
{
	LBXGL_MeshPrim *tmp;
	int i;

	tmp=LBXGL_Mesh_ClonePrim(prim);
	tmp->next=gtmp->prim;
	gtmp->prim=tmp;

	for(i=0; i<tmp->n_xyz; i++)
		{ V3F_ADD(tmp->xyz+i*3, org, tmp->xyz+i*3); }
	V3F_ADD(tmp->tex_org, org, tmp->tex_org);
}

LBXGL_API void LBXGL_BrushMesh_InsertGroup(LBXGL_MeshGroup *gtmp, LBXGL_MeshGroup *grp, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_InsertGroup(gtmp, gcur, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		LBXGL_BrushMesh_InsertPrim(gtmp, mcur, org);
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_MergeSelGroup(LBXGL_MeshPrim *prim, LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		LBXGL_BrushMesh_MergeSelGroup(prim, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		if(mcur->fl&1)
		{
			LBXGL_Mesh_MergePrim(prim, mcur);
			mcur->fl&=~1; mcur->fl|=128;
		}
		mcur=mcur->next;
	}
}

LBXGL_API void LBXGL_BrushMesh_CmdMultiSel(int cmd)
{
	LBXGL_MeshGroup *grp;
	LBXGL_MeshPrim *prim;
	float mins[3], maxs[3], org[3];
	float io[3], pt[4];
	float f, g;
	int i, j, k;

	if(cmd==LBXGL_EDCMD_CUT)
	{
		LBXGL_BrushMesh_CmdMultiSel(LBXGL_EDCMD_COPY);
		LBXGL_BrushMesh_CmdMultiSel(LBXGL_EDCMD_DELETE);
		return;
	}

	if((lbxgl_brushmesh_mode==LBXGL_EDMODE_OBJECT) && (cmd==LBXGL_EDCMD_MERGE))
	{
		LBXGL_BrushMesh_AddUndo();

		prim=LBXGL_Mesh_CreatePrim(lbxgl_brushmesh_mdl, "merged");
		LBXGL_BrushMesh_MergeSelGroup(prim, lbxgl_brushmesh_mdl);
		lbxgl_meshsel_prim=NULL;
		LBXGL_BrushMesh_ClearSel();
		return;
	}

	V3F_SET(mins,  9999,  9999,  9999);
	V3F_SET(maxs, -9999, -9999, -9999);
	LBXGL_BrushMesh_BoxSelGroup(lbxgl_brushmesh_mdl, mins, maxs);

	if(mins[0]>maxs[0])return;	//nothing selected

	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);

	if((cmd==LBXGL_EDCMD_SUBDIVIDE) || (cmd==LBXGL_EDCMD_MERGE))
	{
		LBXGL_BrushMesh_CmdMSelGroup(lbxgl_brushmesh_mdl, cmd, org);
		return;
	}

	if(cmd==LBXGL_EDCMD_COPY)
	{
		if(lbxgl_brushmesh_clip)LBXGL_Mesh_DestroyGroup(lbxgl_brushmesh_clip);
		lbxgl_brushmesh_clip=LBXGL_Mesh_CreateGroup(NULL, "clip");
		LBXGL_BrushMesh_CopyMSelGroup(lbxgl_brushmesh_clip, lbxgl_brushmesh_mdl, cmd, org);
		return;
	}

	if(cmd==LBXGL_EDCMD_DUPLICATE)
	{
		LBXGL_BrushMesh_AddUndo();

		grp=LBXGL_Mesh_CreateGroup(NULL, "clip");
		LBXGL_BrushMesh_CopyMSelGroup(grp, lbxgl_brushmesh_mdl, cmd, org);

		V3F_COPY(org, io);
		io[0]-=1; io[1]-=1; io[2]-=1;

		LBXGL_BrushMesh_InsertGroup(lbxgl_brushmesh_mdl, grp, io);

		return;
	}

	LBXGL_BrushMesh_ClearOpSelGroup(lbxgl_brushmesh_mdl);
	LBXGL_BrushMesh_OpSelGroup(lbxgl_brushmesh_mdl);
	LBXGL_BrushMesh_CmdOpSelGroup(lbxgl_brushmesh_mdl, cmd, org);
}

LBXGL_API void LBXGL_BrushMesh_CmdSel(int cmd)
{
	if((cmd==LBXGL_EDCMD_COPY) || (cmd==LBXGL_EDCMD_CUT) || (cmd==LBXGL_EDCMD_DUPLICATE))
	{
		LBXGL_BrushMesh_CmdMultiSel(cmd);
		return;
	}

	if(lbxgl_meshsel_multi)
		LBXGL_BrushMesh_CmdMultiSel(cmd);
	else LBXGL_BrushMesh_CmdSelGroup(lbxgl_brushmesh_mdl, cmd);
}

LBXGL_API void LBXGL_BrushMesh_Paste()
{
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4];
	float f, g, v;

	if(!lbxgl_brushmesh_clip)return;

	LBXGL_BrushMesh_AddUndo();

//	V3F_ADD(org, lbxgl_cam->fw, io);
//	V3F_ADD(io, lbxgl_cam->up, io);
//	V3F_ADD(io, lbxgl_cam->rt, io);

	V3F_SET(mins,  9999,  9999,  9999);
	V3F_SET(maxs, -9999, -9999, -9999);

	LBXGL_Mesh_BoxGroup(lbxgl_brushmesh_clip, mins, maxs);
	f=MAX(V3F_LEN(mins), V3F_LEN(maxs));
	if(f<1.0)f=1.0;

//	printf("Paste Size %g\n", f);

	V3F_ADDSCALE(lbxgl_cam->org, lbxgl_cam->fw, 5*f, io);
//	printf("Paste At (%g %g %g)\n", io[0], io[1], io[2]);

	LBXGL_BrushMesh_InsertGroup(lbxgl_brushmesh_mdl, lbxgl_brushmesh_clip, io);
}


LBXGL_API void LBXGL_BrushMesh_ReprojectPrim(LBXGL_MeshPrim *prim)
{
	char tb[256];
	float sdir[4], tdir[4], udir[4], torg[3];
	float io[3], nv[4];
	float st[2], sv, tv;
	float *fa, *fb;
	int p, n, w, h;
	int i, j, k;

	w=-1; h=-1;
//	i=Tex_LoadFile(prim->tex, &w, &h);
	i=LBXGL_Mesh_LoadTexture(prim, &w, &h);
	if((i<0) || (w<=0) || (h<=0))return;


//	printf("Remap Tex Coords: %d F %d V\n", prim->n_face, prim->n_xyz);

	sv=V3F_NORMALIZE(prim->tex_sdir, sdir);
	tv=V3F_NORMALIZE(prim->tex_tdir, tdir);
	V3F_CROSS(sdir, tdir, udir);
//	sdir[3]=V3F_DOT(prim->tex_org, sdir)-sv*0.5;
//	tdir[3]=V3F_DOT(prim->tex_org, tdir)-tv*0.5;
	sdir[3]=V3F_DOT(prim->tex_org, sdir);
	tdir[3]=V3F_DOT(prim->tex_org, tdir);
	udir[3]=V3F_DOT(prim->tex_org, udir);

	sv*=w;
	tv*=h;

	prim->n_st=0;
	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;
		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		LBXGL_Mesh_CalcFaceNormal(prim, i, nv);

		for(j=0; j<n; j++)
		{
			fa=prim->xyz+prim->pts[(p+j)*2+0]*3;

			LBXGL_Mesh_ProjectPointST(prim, fa, st, nv,
				sdir, tdir, udir, sv, tv, w, h);

			prim->pts[(p+j)*2+1]=LBXGL_Mesh_IndexST2(prim, st);
		}
	}
}
