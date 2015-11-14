//#include <windows.h>

//#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// #include <pdlib.h>

#include <pdgl.h>
#include <pdglui.h>
#include <pdgl_stub.h>

#include <lbxgl2.h>
#include <libvecmath.h>

#include <bgbasm.h>
#include <bgbscr1.h>

#ifndef MESH_API
#ifdef MSVC
#define MESH_API __declspec(dllexport)
#else
#define MESH_API
#endif
#endif

#define MODE_GROUP	0
#define MODE_OBJECT	1
#define MODE_FACE	2
#define MODE_VERTEX	3
#define MODE_TEXTURE	4

#define CMD_SEL		0
#define CMD_TRANS	1
#define CMD_ROT		2
#define CMD_SCALE	3
#define CMD_SCALE2	4

#define CMD_DELETE	16
#define CMD_EXTRUDE	17
#define CMD_SUBDIVIDE	18
#define CMD_MERGE	19
#define CMD_CUT		20
#define CMD_COPY	21
#define CMD_DUPLICATE	22
#define CMD_INVERT	23

#define VEC_SZ		0.1


MESH_API LBXGL_MeshGroup *main_mdl;
MESH_API LBXGL_MeshGroup *sel_grp;
MESH_API LBXGL_MeshPrim *sel_prim;
MESH_API int sel_face, sel_vec;
MESH_API int sel_multi;

MESH_API int main_mode;
MESH_API int main_cmd;
MESH_API int main_loadsave;
MESH_API char *main_file;

MESH_API LBXGL_MeshGroup *main_clip;


MESH_API int main_dirty;
MESH_API int main_lutime;
MESH_API LBXGL_MeshGroup *main_undo[16];
MESH_API LBXGL_MeshGroup *main_redo[16];

//script interface functions
int (*meshtool_handle_input_fp)();
int (*meshtool_draw3d_fp)();
int (*meshtool_draw2d_fp)();

int (*meshtool_menu_event_fp)(char *str);


MESH_API void Mesh_AddUndo();
MESH_API void Mesh_ClearSel();

MESH_API LBXGL_MeshGroup *Mesh_GetModel()
	{ return(main_mdl); }

MESH_API void Mesh_SetDirty()
{
	if(main_dirty && (pdgl_time>(main_lutime+1000)))
		main_dirty=0;

	if(main_dirty)return;
	Mesh_AddUndo();

	main_dirty=1;
	main_lutime=pdgl_time;
}

MESH_API void Mesh_AddUndo()
{
	int i;

//	printf("Add Undo\n");

	for(i=0; i<16; i++)
	{
		if(main_redo[i])LBXGL_Mesh_DestroyGroup(main_redo[i]);
		main_redo[i]=NULL;
	}

	if(main_undo[15])LBXGL_Mesh_DestroyGroup(main_undo[15]);
	for(i=15; i>0; i--)
		main_undo[i]=main_undo[i-1];
	main_undo[0]=LBXGL_Mesh_CloneGroup(main_mdl);
	main_dirty=0;
}

MESH_API void Mesh_Undo()
{
	int i;

	if(!main_undo[0])
		return;

//	printf("Undo\n");

	Mesh_ClearSel();

	if(main_redo[15])LBXGL_Mesh_DestroyGroup(main_redo[15]);
	for(i=15; i>0; i--)
		main_redo[i]=main_redo[i-1];

	main_redo[0]=main_mdl;
	main_mdl=main_undo[0];

	for(i=0; i<15; i++)
		main_undo[i]=main_undo[i+1];
	main_undo[15]=NULL;

	Mesh_ClearSel();
}

MESH_API void Mesh_Redo()
{
	int i;

	if(!main_redo[0])
		return;

//	printf("Redo\n");

	Mesh_ClearSel();

	if(main_undo[15])LBXGL_Mesh_DestroyGroup(main_undo[15]);
	for(i=15; i>0; i--)
		main_undo[i]=main_undo[i-1];

	main_undo[0]=main_mdl;
	main_mdl=main_redo[0];

	for(i=0; i<15; i++)
		main_redo[i]=main_redo[i+1];
	main_redo[15]=NULL;

	Mesh_ClearSel();
}


void main_draw_star(float *org)
{
	float v[3];
	int i;

	for(i=1; i<27; i++)
	{
		v[0]=((i/1)%3)?((((i/1)%3)==1)?1:-1):0;
		v[1]=((i/3)%3)?((((i/3)%3)==1)?1:-1):0;
		v[2]=((i/9)%3)?((((i/9)%3)==1)?1:-1):0;
		V3F_NORMALIZE(v, v);

		glVertex3f(org[0]-v[0]*VEC_SZ, org[1]-v[1]*VEC_SZ, org[2]-v[2]*VEC_SZ);
		glVertex3f(org[0]+v[0]*VEC_SZ, org[1]+v[1]*VEC_SZ, org[2]+v[2]*VEC_SZ);
	}
}


MESH_API void Mesh_DrawPrimOrtho(LBXGL_MeshPrim *prim)
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
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_ONE, GL_ZERO);

	if(main_mode==MODE_VERTEX)
	{

		for(i=0; i<prim->n_xyz; i++)
		{
			if(prim->fl_xyz[i]&128)continue;

			glColor4f(0, 0, 1, 1);

			V3F_COPY(prim->xyz+i*3, io);
			f=V3F_DOT(io, cam->mdir)-V3F_DOT(cam->morg, cam->mdir);
			V3F_ADDSCALE(cam->morg, cam->mdir, f, pt);
			f=V3F_DIST(pt, io);
			if(f<VEC_SZ)glColor4f(1, 0, 1, 1);

			if(prim->fl_xyz[i]&1)glColor4f(0, 1, 0, 1);

			glBegin(GL_LINES);
			main_draw_star(prim->xyz+i*3);
			glEnd();
		}
	}

	glColor4f(1, 1, 1, 1);
	if(main_mode==MODE_OBJECT)
	{
		i=LBXGL_Mesh_CheckLinePrim(prim, cam->morg, cam->mend);
		if(i)glColor4f(1, 0, 1, 1);

		if(prim->fl&1)glColor4f(0, 1, 0, 1);
	}


	for(i=0; i<prim->n_face; i++)
	{
		if(prim->fl_face[i]&128)continue;

		p=prim->face[i*2+0];
		n=prim->face[i*2+1];

		if(main_mode==MODE_FACE)
		{
			j=LBXGL_Mesh_CheckLineFace(prim, i,
				cam->morg, cam->mend);
			if(j)glColor4f(1, 0, 1, 1);

			if(prim->fl_face[i]&1)glColor4f(0, 1, 0, 1);
		}

		glBegin(GL_LINE_LOOP);
		for(j=0; j<n; j++)
			glVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
		glEnd();

		if(main_mode==MODE_FACE)glColor4f(1, 1, 1, 1);
	}
}

MESH_API void Mesh_DrawPrim(LBXGL_MeshPrim *prim)
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

	if(cam->ortho)
	{
		Mesh_DrawPrimOrtho(prim);
		return;
	}

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glBlendFunc(GL_ONE, GL_ZERO);

	if(main_mode==MODE_VERTEX)
	{

		for(i=0; i<prim->n_xyz; i++)
		{
			if(prim->fl_xyz[i]&128)continue;

			glColor4f(0, 0, 1, 1);

			V3F_COPY(prim->xyz+i*3, io);
			f=V3F_DOT(io, cam->mdir)-V3F_DOT(cam->morg, cam->mdir);
			V3F_ADDSCALE(cam->morg, cam->mdir, f, pt);
			f=V3F_DIST(pt, io);
			if(f<VEC_SZ)glColor4f(1, 0, 1, 1);

//			if(((prim==sel_prim) && (i==sel_vec)) ||
//				(prim->fl_xyz[i]&1))
//					glColor4f(0, 1, 0, 1);
			if(prim->fl_xyz[i]&1)glColor4f(0, 1, 0, 1);

			glBegin(GL_LINES);
			main_draw_star(prim->xyz+i*3);
			glEnd();
		}
		
		glColor4f(1, 1, 1, 1);
	}

	fl=LBXGL_Texture_GetImageFlags(prim->texnum);
//	fl|=LBXGL_TXFL_ALPHA;

	if(fl&LBXGL_TXFL_ALPHA)
		fl|=LBXGL_TXFL_WINDOW;

	LBXGL_Shader_Color4f(1, 1, 1, 1);
	if((main_mode==MODE_OBJECT) || (main_mode==MODE_TEXTURE))
	{
		i=LBXGL_Mesh_CheckLinePrim(prim, cam->morg, cam->mend);
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

		if(main_mode==MODE_FACE)
		{
			j=LBXGL_Mesh_CheckLineFace(prim, i,
				cam->morg, cam->mend);
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
			glBegin(GL_POLYGON);
			for(j=0; j<n; j++)
				glVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
			glEnd();
		}

		if(main_mode==MODE_FACE)
			LBXGL_Shader_Color4f(1, 1, 1, 1);
	}


	if((fl&LBXGL_TXFL_ALPHA) || (fl&LBXGL_TXFL_WINDOW))
//	if(1)
	{
		glDisable(GL_LIGHTING);
//		glBlendFunc(GL_ONE, GL_ZERO);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}else
	{
		glEnable(GL_LIGHTING);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
	}

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

//		glNormal3fv(dv2);
//		glBegin(GL_POLYGON);

		if(!(fl&(LBXGL_TXFL_ALPHA|LBXGL_TXFL_WINDOW)))
		{
			glEnable(GL_LIGHTING);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
		}


		if(fl&LBXGL_TXFL_ALPHA)
			continue;

		LBXGL_Shader_Normal3fv(dv2);
		LBXGL_Shader_BeginPoly();

		for(j=0; j<n; j++)
		{
//			glTexCoord2fv(prim->st+prim->pts[(p+j)*2+1]*2);
//			glVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);

			LBXGL_Shader_TexCoord2fv(prim->st+prim->pts[(p+j)*2+1]*2);
			LBXGL_Shader_Vertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
		}

//		glEnd();
		LBXGL_Shader_EndPoly();
	}

	if(main_mode==MODE_FACE)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_ONE, GL_ZERO);

		glColor4f(0, 0, 0, 1);
		glBegin(GL_LINES);
		for(i=0; i<prim->n_face; i++)
		{
			if(prim->fl_face[i]&128)continue;

			p=prim->face[i*2+0];
			n=prim->face[i*2+1];

			for(j=0; j<n; j++)
			{
				glVertex3fv(prim->xyz+prim->pts[(p+j)*2+0]*3);
				glVertex3fv(prim->xyz+prim->pts[(p+(j+1)%n)*2+0]*3);
			}
		}
		glEnd();
	}

	if((main_mode==MODE_TEXTURE) && (prim==sel_prim) && (prim->texnum>=0))
	{
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, prim->texnum);
		glColor4f(1, 1, 1, 0.75);

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

			glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);	glVertex3fv(v0);
			glTexCoord2f(1, 0);	glVertex3fv(v1);
			glTexCoord2f(1, 1);	glVertex3fv(v2);
			glTexCoord2f(0, 1);	glVertex3fv(v3);
			glEnd();
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

			glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);	glVertex3fv(v0);
			glTexCoord2f(1, 0);	glVertex3fv(v1);
			glTexCoord2f(1, 1);	glVertex3fv(v2);
			glTexCoord2f(0, 1);	glVertex3fv(v3);
			glEnd();

			V3F_ADDSCALE(prim->tex_org, dv0, -0.5*f, v0);
			V3F_ADDSCALE(prim->tex_org, dv0,  0.5*f, v1);
			V3F_ADDSCALE(prim->tex_org, dv0,  0.5*f, v2);
			V3F_ADDSCALE(prim->tex_org, dv0, -0.5*f, v3);
			V3F_ADDSCALE(v0, dv2, -0.5*f, v0);
			V3F_ADDSCALE(v1, dv2, -0.5*f, v1);
			V3F_ADDSCALE(v2, dv2,  0.5*f, v2);
			V3F_ADDSCALE(v3, dv2,  0.5*f, v3);

			glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);	glVertex3fv(v0);
			glTexCoord2f(1, 0);	glVertex3fv(v1);
			glTexCoord2f(1, 1);	glVertex3fv(v2);
			glTexCoord2f(0, 1);	glVertex3fv(v3);
			glEnd();

			V3F_ADDSCALE(prim->tex_org, dv2, -0.5*f, v0);
			V3F_ADDSCALE(prim->tex_org, dv2,  0.5*f, v1);
			V3F_ADDSCALE(prim->tex_org, dv2,  0.5*f, v2);
			V3F_ADDSCALE(prim->tex_org, dv2, -0.5*f, v3);
			V3F_ADDSCALE(v0, dv1, -0.5*g, v0);
			V3F_ADDSCALE(v1, dv1, -0.5*g, v1);
			V3F_ADDSCALE(v2, dv1,  0.5*g, v2);
			V3F_ADDSCALE(v3, dv1,  0.5*g, v3);

			glBegin(GL_POLYGON);
			glTexCoord2f(0, 0);	glVertex3fv(v0);
			glTexCoord2f(1, 0);	glVertex3fv(v1);
			glTexCoord2f(1, 1);	glVertex3fv(v2);
			glTexCoord2f(0, 1);	glVertex3fv(v3);
			glEnd();
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
}

MESH_API void Mesh_DrawPrimAlpha(LBXGL_MeshPrim *prim)
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

	if(cam->ortho)
		return;

	glDepthFunc(GL_LEQUAL);
//	glDisable(GL_TEXTURE_2D);

	fl=LBXGL_Texture_GetImageFlags(prim->texnum);
//	fl|=LBXGL_TXFL_ALPHA;

	if(!(fl&LBXGL_TXFL_ALPHA))
		return;

	glDisable(GL_LIGHTING);
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

MESH_API void Mesh_DrawGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_DrawGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_DrawPrim(mcur);
		mcur=mcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_DrawPrimAlpha(mcur);
		mcur=mcur->next;
	}
}


MESH_API void Mesh_ClearSelPrim(LBXGL_MeshPrim *prim)
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

MESH_API void Mesh_ClearSelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_ClearSelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_ClearSelPrim(mcur);
		mcur=mcur->next;
	}

	grp->fl&=~1;
}

MESH_API void Mesh_ClearOpSelPrim(LBXGL_MeshPrim *prim)
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

MESH_API void Mesh_ClearOpSelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	grp->fl&=~14;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_ClearOpSelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_ClearOpSelPrim(mcur);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_ClearSel()
{
	sel_grp=NULL; sel_prim=NULL;
	sel_face=-1; sel_vec=-1;
	Mesh_ClearSelGroup(main_mdl);
	sel_multi=0;
}


MESH_API void Mesh_SelPrim(LBXGL_MeshPrim *prim)
{
	float mins[3], maxs[3];
	float io[3], pt[4];
	float f;
	int i, j, k;

	if(prim->fl&128)return;

	if((main_mode==MODE_OBJECT) || (main_mode==MODE_TEXTURE))
	{
		i=LBXGL_Mesh_CheckLinePrim(prim, cam->morg, cam->mend);
		if(i && PDGL_KeyDown(K_SHIFT))
		{
			sel_multi=1;
			prim->fl|=1;
			return;
		}

		if(i && !sel_prim)
		{
			sel_prim=prim;
			prim->fl|=1;
		}else if(prim==sel_prim)
		{
			sel_prim=NULL;
			prim->fl&=~1;
		}
	}

	if(main_mode==MODE_FACE)
	{
		if(PDGL_KeyDown(K_SHIFT))
		{
			sel_multi=1;
			for(i=0; i<prim->n_face; i++)
			{
				j=LBXGL_Mesh_CheckLineFace(
					prim, i, cam->morg, cam->mend);
				if(j)prim->fl_face[i]|=1;
			}
			return;
		}

		for(i=0; i<prim->n_face; i++)
		{
			j=LBXGL_Mesh_CheckLineFace(prim, i, cam->morg, cam->mend);
			if(j && (sel_face<0))
			{
				prim->fl_face[i]|=1;
				sel_prim=prim;
				sel_face=i;
			}else if((prim==sel_prim) && (i==sel_face))
			{
				prim->fl_face[i]&=~1;
				sel_prim=NULL;
				sel_face=-1;
			}
		}
	}

	if(main_mode==MODE_VERTEX)
	{
		if(PDGL_KeyDown(K_SHIFT))
		{
			sel_multi=1;
			for(i=0; i<prim->n_xyz; i++)
			{
				V3F_COPY(prim->xyz+i*3, io);

				f=V3F_DOT(io, cam->mdir)-V3F_DOT(cam->morg, cam->mdir);
				V3F_ADDSCALE(cam->morg, cam->mdir, f, pt);
				f=V3F_DIST(pt, io);
				if(f<VEC_SZ)prim->fl_xyz[i]|=1;
			}
			return;
		}

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_COPY(prim->xyz+i*3, io);

			f=V3F_DOT(io, cam->mdir)-V3F_DOT(cam->morg, cam->mdir);
			V3F_ADDSCALE(cam->morg, cam->mdir, f, pt);
			f=V3F_DIST(pt, io);
			if((f<VEC_SZ) && (sel_vec<0))
			{
				prim->fl_xyz[i]|=1;
				sel_prim=prim;
				sel_vec=i;
			}else if((prim==sel_prim) && (i==sel_vec))
			{
				prim->fl_xyz[i]&=~1;
				sel_prim=NULL;
				sel_vec=-1;
			}
		}
	}
}

MESH_API void Mesh_SelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_SelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_SelPrim(mcur);
		mcur=mcur->next;
	}
}


MESH_API int Mesh_PointInMBox(float *org)
{
	int mx, my, nx, ny;
	float io[3], pt[4];
	float f, g;

	mx=(state->mx<state->omx)?state->mx:state->omx;
	my=(state->my<state->omy)?state->my:state->omy;
	nx=(state->mx>state->omx)?state->mx:state->omx;
	ny=(state->my>state->omy)?state->my:state->omy;

	io[0]=V3F_DOT(org, cam->rt)-V3F_DOT(cam->org, cam->rt);
	io[1]=-(V3F_DOT(org, cam->up)-V3F_DOT(cam->org, cam->up));
	io[2]=V3F_DOT(org, cam->fw)-V3F_DOT(cam->org, cam->fw);

	if(!cam->ortho)
	{
		if(io[2]<1.0)return(0);
		io[0]/=io[2];
		io[1]/=io[2];
	}else
	{
		io[0]/=cam->width;
		io[1]/=cam->width;
	}

	pt[0]=mx/400.0;	pt[1]=my/400.0;
	pt[2]=nx/400.0;	pt[3]=ny/400.0;

	if(io[0]<pt[0])return(0);
	if(io[1]<pt[1])return(0);
	if(io[0]>pt[2])return(0);
	if(io[1]>pt[3])return(0);
	return(1);
}

MESH_API void Mesh_SelPrimBox(LBXGL_MeshPrim *prim)
{
	float mins[3], maxs[3];
	float io[3], pt[4];
	float f;
	int p, n;
	int i, j, k;

	if(prim->fl&128)return;

	if(main_mode==MODE_OBJECT)
	{
		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_COPY(prim->xyz+i*3, io);
			j=Mesh_PointInMBox(io);
			if(j)
			{
				if(sel_prim)sel_multi=1;
				sel_prim=prim;
				prim->fl|=1;
			}
		}
	}

	if(main_mode==MODE_FACE)
	{
		for(i=0; i<prim->n_face; i++)
		{
			if(prim->fl_face[i]&128)continue;

			p=prim->face[i*2+0];
			n=prim->face[i*2+1];

			for(j=0; j<n; j++)
			{
				V3F_COPY(prim->xyz+prim->pts[(p+j)*2+0]*3, io);
				k=Mesh_PointInMBox(io);
				if(k)
				{
					if(sel_prim)sel_multi=1;
					sel_prim=prim;
					sel_face=i;
					prim->fl_face[i]|=1;
					break;
				}
			}
		}
	}

	if(main_mode==MODE_VERTEX)
	{
		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_COPY(prim->xyz+i*3, io);
			j=Mesh_PointInMBox(io);
			if(j)
			{
				if(sel_prim)sel_multi=1;
				sel_prim=prim;
				sel_vec=i;
				prim->fl_xyz[i]|=1;
			}
		}
	}
}

MESH_API void Mesh_SelGroupBox(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_SelGroupBox(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_SelPrimBox(mcur);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_ExtrudeFace(LBXGL_MeshPrim *prim, int fn)
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

MESH_API void Mesh_DivideFace(LBXGL_MeshPrim *prim, int fn)
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

MESH_API void Mesh_FlipFace(LBXGL_MeshPrim *prim, int fn)
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

MESH_API void Mesh_CreateFace(LBXGL_MeshPrim *prim)
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

MESH_API void Mesh_CreateCube()
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
		prim=LBXGL_Mesh_LookupPrimGroup(main_mdl, tb);
		if(!prim)break;
	}

	prim=LBXGL_Mesh_CreatePrim(main_mdl, tb);
//	prim=LBXGL_Mesh_CreatePrim(main_mdl, "cube");
	V3F_ADDSCALE(cam->org, cam->fw, 10, org);

	if(cam->ortho) { V3F_COPY(cam->org, org); }

	printf("Mesh Create Cube\n");
	printf("\t(%g %g %g)\n", org[0], org[1], org[2]);

	for(i=0; i<8; i++)for(j=0; j<3; j++)
		prim->xyz[i*3+j]=org[j]+((i&(1<<j))?1.0:-1.0);

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
}

MESH_API void Mesh_CalcTranslate(float *org, float *offs)
{
	float io[3], pt[4];
	float f;

	f=V3F_DOT(org, cam->mdir)-V3F_DOT(cam->org, cam->mdir);
	V3F_SUB(cam->morg, cam->lmorg, io);
	V3F_SUB(cam->org, cam->lorg, pt);
	V3F_SUB(io, pt, io);
//	V3F_SUB(cam->mdir, cam->lmdir, io);
	if(!cam->ortho) { V3F_SCALE(io, f, io); }
	V3F_ADD(io, pt, offs);
}

MESH_API float Mesh_CalcRotate(float *org)
{
	float io[3], pt[4];
	float f, g;

	io[0]=V3F_DOT(org, cam->rt)-V3F_DOT(cam->org, cam->rt);
	io[1]=V3F_DOT(org, cam->up)-V3F_DOT(cam->org, cam->up);
	io[2]=V3F_DOT(org, cam->fw)-V3F_DOT(cam->org, cam->fw);

	if(!cam->ortho)
	{
		if(io[2]<1)return(0.0);
		io[0]/=io[2]; io[1]/=io[2];
	}else
	{
		io[0]/=cam->width;
		io[1]/=cam->width;
	}

	pt[0]=state->mx/400.0;	pt[1]=-state->my/400.0;
	pt[2]=state->lmx/400.0;	pt[3]=-state->lmy/400.0;
	f=atan2(pt[1]-io[1], pt[0]-io[0]);
	g=atan2(pt[3]-io[1], pt[2]-io[0]);
	return(f-g);
}

MESH_API float Mesh_CalcScale(float *org)
{
	float io[3], pt[4];
	float f, g;

	io[0]=V3F_DOT(org, cam->rt)-V3F_DOT(cam->org, cam->rt);
	io[1]=V3F_DOT(org, cam->up)-V3F_DOT(cam->org, cam->up);
	io[2]=V3F_DOT(org, cam->fw)-V3F_DOT(cam->org, cam->fw);

	if(!cam->ortho)
	{
		if(io[2]<1)return(1.0);
		io[0]/=io[2]; io[1]/=io[2];
	}else
	{
		io[0]/=cam->width;
		io[1]/=cam->width;
	}

	pt[0]=state->mx/400.0;	pt[1]=-state->my/400.0;
	pt[2]=state->lmx/400.0;	pt[3]=-state->lmy/400.0;
	f=sqrt(SQR(pt[0]-io[0])+SQR(pt[1]-io[1]));
	g=sqrt(SQR(pt[2]-io[0])+SQR(pt[3]-io[1]));
	if(g<0.01)return(1.0);
	f/=g;

	return(f);
}

MESH_API void Mesh_CalcScaleHV(float *org, float *sc)
{
	float io[3], pt[4];
	float f, g;

	sc[0]=1.0;
	sc[1]=1.0;

	io[0]=V3F_DOT(org, cam->rt)-V3F_DOT(cam->org, cam->rt);
	io[1]=V3F_DOT(org, cam->up)-V3F_DOT(cam->org, cam->up);
	io[2]=V3F_DOT(org, cam->fw)-V3F_DOT(cam->org, cam->fw);

	if(!cam->ortho)
	{
		if(io[2]<1)return;
		io[0]/=io[2]; io[1]/=io[2];
	}else
	{
		io[0]/=cam->width;
		io[1]/=cam->width;
	}

	pt[0]=state->mx/400.0;	pt[1]=-state->my/400.0;
	pt[2]=state->lmx/400.0;	pt[3]=-state->lmy/400.0;

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

MESH_API void Mesh_CmdSelPrim(LBXGL_MeshPrim *prim, int cmd)
{
	float mins[3], maxs[3];
	float io[3], pt[4], hv[2];
	float f, g, h;
	int i, j, k;

	if(prim!=sel_prim)return;
	if(prim->fl&128)return;
	Mesh_SetDirty();

	if(main_mode==MODE_OBJECT)
		switch(cmd)
	{
	case CMD_TRANS:
		LBXGL_Mesh_CalcCenter(prim, io);
		Mesh_CalcTranslate(io, io);
		for(i=0; i<prim->n_xyz; i++)
			{ V3F_ADD(prim->xyz+i*3, io, prim->xyz+i*3); }
		V3F_ADD(prim->tex_org, io, prim->tex_org);
		break;

	case CMD_ROT:
		LBXGL_Mesh_CalcCenter(prim, io);
		f=Mesh_CalcRotate(io);
		for(i=0; i<prim->n_xyz; i++)
			LBXGL_Mesh_RotatePointOrgAxisAngle(
				io, cam->fw, -f, prim->xyz+i*3);

		LBXGL_Mesh_RotatePointOrgAxisAngle(
			io, cam->fw, -f, prim->tex_org);
		LBXGL_Mesh_RotatePointAxisAngle(cam->fw, -f, prim->tex_sdir);
		LBXGL_Mesh_RotatePointAxisAngle(cam->fw, -f, prim->tex_tdir);
		break;
	case CMD_SCALE:
		LBXGL_Mesh_CalcCenter(prim, io);
		f=Mesh_CalcScale(io);

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_SUB(prim->xyz+i*3, io, pt);
			V3F_ADDSCALE(io, pt, f, prim->xyz+i*3);
		}
		V3F_SCALE(prim->tex_sdir, f, prim->tex_sdir);
		V3F_SCALE(prim->tex_tdir, f, prim->tex_tdir);

		break;

	case CMD_SCALE2:
		LBXGL_Mesh_CalcCenter(prim, io);
		Mesh_CalcScaleHV(io, hv);

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_SUB(prim->xyz+i*3, io, pt);
			f=V3F_DOT(pt, cam->rt);
			V3F_ADDSCALE(pt, cam->rt, -f, pt);
			g=V3F_DOT(pt, cam->up);
			V3F_ADDSCALE(pt, cam->up, -g, pt);

			V3F_ADDSCALE(pt, cam->rt, f*hv[0], pt);
			V3F_ADDSCALE(pt, cam->up, g*hv[1], pt);

			V3F_ADD(io, pt, prim->xyz+i*3);
		}
		break;

	case CMD_DELETE:
		Mesh_AddUndo();
		prim->fl&=~1;
		prim->fl|=128;
		sel_prim=NULL;
		break;

	case CMD_SUBDIVIDE:
		Mesh_AddUndo();
		LBXGL_Mesh_SubdivideMesh(sel_prim);
		Mesh_ClearSel();
		break;

	default:
		break;
	}

	if(main_mode==MODE_FACE)
	{
		if(sel_face<0)return;
		i=sel_face;

		switch(cmd)
		{
		case CMD_TRANS:
			LBXGL_Mesh_CalcFaceCenter(prim, i, io);
			Mesh_CalcTranslate(io, io);
			LBXGL_Mesh_TranslateFace(prim, i, io);
			break;
		case CMD_ROT:
			LBXGL_Mesh_CalcFaceCenter(prim, i, io);
			f=Mesh_CalcRotate(io);
			LBXGL_Mesh_RotateFace(prim, i, io, cam->fw, -f);
			break;
		case CMD_SCALE:
			LBXGL_Mesh_CalcFaceCenter(prim, i, io);
			f=Mesh_CalcScale(io);
			LBXGL_Mesh_ScaleFace(prim, i, io, f);
			break;

		case CMD_DELETE:
			Mesh_AddUndo();
			LBXGL_Mesh_DeleteFace(prim, i);
			break;
		case CMD_EXTRUDE:
			Mesh_AddUndo();
			Mesh_ExtrudeFace(prim, i);
			break;
		case CMD_SUBDIVIDE:
			Mesh_AddUndo();
			Mesh_DivideFace(prim, i);
			break;
		case CMD_INVERT:
			Mesh_AddUndo();
			Mesh_FlipFace(prim, i);
			break;

		default:
			break;
		}
	}

	if(main_mode==MODE_VERTEX)
	{
		if(sel_vec<0)return;
		i=sel_vec;

		switch(cmd)
		{
		case CMD_TRANS:
			Mesh_CalcTranslate(prim->xyz+i*3, io);
			V3F_ADD(prim->xyz+i*3, io, prim->xyz+i*3);
			break;

		case CMD_DELETE:
			Mesh_AddUndo();
			LBXGL_Mesh_DeleteVertex(prim, i);
			break;

		default:
			break;
		}
	}

	if(main_mode==MODE_TEXTURE)
	{
		switch(cmd)
		{
		case CMD_TRANS:
			Mesh_CalcTranslate(prim->tex_org, io);
			V3F_ADD(prim->tex_org, io, prim->tex_org);
			break;
		case CMD_ROT:
			V3F_COPY(prim->tex_org, io);
			f=Mesh_CalcRotate(io);
			LBXGL_Mesh_RotatePointAxisAngle(
				cam->fw, -f, prim->tex_sdir);
			LBXGL_Mesh_RotatePointAxisAngle(
				cam->fw, -f, prim->tex_tdir);
			break;
		case CMD_SCALE:
			V3F_COPY(prim->tex_org, io);
			f=Mesh_CalcScale(io);
			V3F_SCALE(prim->tex_sdir, f, prim->tex_sdir);
			V3F_SCALE(prim->tex_tdir, f, prim->tex_tdir);
			break;

		case CMD_SCALE2:
			V3F_COPY(prim->tex_org, io);
			Mesh_CalcScaleHV(io, hv);

			f=V3F_DOT(prim->tex_sdir, cam->rt);
			g=V3F_DOT(prim->tex_sdir, cam->up);
			h=hv[0]*f+hv[1]*g;
			V3F_SCALE(prim->tex_sdir, h, prim->tex_sdir);

			f=V3F_DOT(prim->tex_tdir, cam->rt);
			g=V3F_DOT(prim->tex_tdir, cam->up);
			h=hv[0]*f+hv[1]*g;
			V3F_SCALE(prim->tex_tdir, h, prim->tex_tdir);
			break;

		default:
			break;
		}

		LBXGL_Mesh_ReprojectPrim(prim);
	}
}

MESH_API void Mesh_CmdSelGroup(LBXGL_MeshGroup *grp, int cmd)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_CmdSelGroup(gcur, cmd);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_CmdSelPrim(mcur, cmd);
		mcur=mcur->next;
	}
}


MESH_API void Mesh_BoxSelPrim(LBXGL_MeshPrim *prim, float *mins, float *maxs)
{
	int i, j, k;

	if(prim->fl&128)return;

	if(main_mode==MODE_OBJECT)
	{
		if(!(prim->fl&1))return;

		for(i=0; i<prim->n_xyz; i++)
		{
			V3F_MIN(mins, prim->xyz+i*3, mins);
			V3F_MAX(maxs, prim->xyz+i*3, maxs);
		}
	}

	if(main_mode==MODE_FACE)
	{
		for(i=0; i<prim->n_face; i++)
			if(prim->fl_face[i]&1)
				LBXGL_Mesh_BoxFace(prim, i, mins, maxs);
	}

	if(main_mode==MODE_VERTEX)
	{
		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&1)
		{
			V3F_MIN(mins, prim->xyz+i*3, mins);
			V3F_MAX(maxs, prim->xyz+i*3, maxs);
		}
	}
}

MESH_API void Mesh_BoxSelGroup(LBXGL_MeshGroup *grp, float *mins, float *maxs)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_BoxSelGroup(gcur, mins, maxs);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_BoxSelPrim(mcur, mins, maxs);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_OpSelPrim(LBXGL_MeshPrim *prim)
{
	int p, n;
	int i, j, k;

	if(prim->fl&128)return;

	if(main_mode==MODE_OBJECT)
	{
		if(!(prim->fl&1))return;

		for(i=0; i<prim->n_xyz; i++)
			prim->fl_xyz[i]|=2;
	}

	if(main_mode==MODE_FACE)
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

	if(main_mode==MODE_VERTEX)
	{
		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&1)
				prim->fl_xyz[i]|=2;
	}
}

MESH_API void Mesh_OpSelGroup(LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_OpSelGroup(gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_OpSelPrim(mcur);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_CmdOpSelPrim(LBXGL_MeshPrim *prim, int cmd, float *org)
{
	float io[3], pt[4], hv[2];
	float f, g;
	int i, j, k;

	if(prim->fl&128)return;
	Mesh_SetDirty();

	switch(cmd)
	{
	case CMD_TRANS:
		Mesh_CalcTranslate(org, io);
		for(i=0; i<prim->n_xyz; i++)if(prim->fl_xyz[i]&2)
			{ V3F_ADD(prim->xyz+i*3, io, prim->xyz+i*3); }
		break;

	case CMD_ROT:
		f=Mesh_CalcRotate(org);
		for(i=0; i<prim->n_xyz; i++)if(prim->fl_xyz[i]&2)
			LBXGL_Mesh_RotatePointOrgAxisAngle(
				org, cam->fw, -f, prim->xyz+i*3);
		break;
	case CMD_SCALE:
		f=Mesh_CalcScale(org);

		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&2)
		{
			V3F_SUB(prim->xyz+i*3, org, pt);
			V3F_ADDSCALE(org, pt, f, prim->xyz+i*3);
		}
		break;

	case CMD_SCALE2:
		Mesh_CalcScaleHV(org, hv);

		for(i=0; i<prim->n_xyz; i++)
			if(prim->fl_xyz[i]&2)
		{
			V3F_SUB(prim->xyz+i*3, org, pt);
			f=V3F_DOT(pt, cam->rt);
			V3F_ADDSCALE(pt, cam->rt, -f, pt);
			g=V3F_DOT(pt, cam->up);
			V3F_ADDSCALE(pt, cam->up, -g, pt);

			V3F_ADDSCALE(pt, cam->rt, f*hv[0], pt);
			V3F_ADDSCALE(pt, cam->up, g*hv[1], pt);

			V3F_ADD(org, pt, prim->xyz+i*3);
		}
		break;
	default:
		break;
	}
}

MESH_API void Mesh_CmdOpSelGroup(LBXGL_MeshGroup *grp, int cmd, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_CmdOpSelGroup(gcur, cmd, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_CmdOpSelPrim(mcur, cmd, org);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_CmdMSelPrim(LBXGL_MeshPrim *prim, int cmd, float *org)
{
	float io[3], pt[4];
	float f, g;
	int i, j, k;

	if(prim->fl&128)return;
	Mesh_SetDirty();

//	printf("cmd multi %d %d\n", main_mode, cmd);


	if(main_mode==MODE_FACE)
		switch(cmd)
	{
	case CMD_SUBDIVIDE:
//		printf("multi divide\n");
		for(i=0; i<prim->n_face; i++)
			if(prim->fl_face[i]&1)
				Mesh_DivideFace(prim, i);
		break;
	default:
		break;
	}

	if(main_mode==MODE_VERTEX)
		switch(cmd)
	{
	case CMD_MERGE:
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

MESH_API void Mesh_CmdMSelGroup(LBXGL_MeshGroup *grp, int cmd, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_CmdMSelGroup(gcur, cmd, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_CmdMSelPrim(mcur, cmd, org);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_CopyMSelPrim(
	LBXGL_MeshGroup *gtmp, LBXGL_MeshPrim *prim, int cmd, float *org)
{
	LBXGL_MeshPrim *tmp;
	float *fa, *fb;
	float io[3];
	int p, p1, n;
	int i, j, k;

	i=0;
	switch(main_mode)
	{
	case MODE_OBJECT:
		if(prim->fl&1)i=1;
		break;
	case MODE_FACE:
		for(j=0; j<prim->n_face; j++)
			if(prim->fl_face[j]&1)i=1;
		break;
	case MODE_VERTEX:
		for(j=0; j<prim->n_xyz; j++)
			if(prim->fl_xyz[j]&1)i=1;
		break;
	default:
		break;
	}

	if(!i)return;

	if(main_mode==MODE_OBJECT)
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
		tmp->tex=dystring(prim->tex);
		tmp->texnum=prim->texnum;
		V3F_COPY(prim->tex_org, tmp->tex_org);
		V3F_COPY(prim->tex_sdir, tmp->tex_sdir);
		V3F_COPY(prim->tex_tdir, tmp->tex_tdir);
		tmp->tex_mode=prim->tex_mode;
		tmp->tex_fl=prim->tex_fl;
	}

	if(main_mode==MODE_FACE)
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

	if(main_mode==MODE_VERTEX)
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

MESH_API void Mesh_CopyMSelGroup(
	LBXGL_MeshGroup *gtmp, LBXGL_MeshGroup *grp, int cmd, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_CopyMSelGroup(gtmp, gcur, cmd, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_CopyMSelPrim(gtmp, mcur, cmd, org);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_InsertPrim(LBXGL_MeshGroup *gtmp, LBXGL_MeshPrim *prim, float *org)
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

MESH_API void Mesh_InsertGroup(LBXGL_MeshGroup *gtmp, LBXGL_MeshGroup *grp, float *org)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_InsertGroup(gtmp, gcur, org);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Mesh_InsertPrim(gtmp, mcur, org);
		mcur=mcur->next;
	}
}

MESH_API void Mesh_MergeSelGroup(LBXGL_MeshPrim *prim, LBXGL_MeshGroup *grp)
{
	LBXGL_MeshGroup *gcur;
	LBXGL_MeshPrim *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Mesh_MergeSelGroup(prim, gcur);
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

MESH_API void Mesh_CmdMultiSel(int cmd)
{
	LBXGL_MeshGroup *grp;
	LBXGL_MeshPrim *prim;
	float mins[3], maxs[3], org[3];
	float io[3], pt[4];
	float f, g;
	int i, j, k;

	if(cmd==CMD_CUT)
	{
		Mesh_CmdMultiSel(CMD_COPY);
		Mesh_CmdMultiSel(CMD_DELETE);
		return;
	}

	if((main_mode==MODE_OBJECT) && (cmd==CMD_MERGE))
	{
		Mesh_AddUndo();

		prim=LBXGL_Mesh_CreatePrim(main_mdl, "merged");
		Mesh_MergeSelGroup(prim, main_mdl);
		sel_prim=NULL;
		Mesh_ClearSel();
		return;
	}

	V3F_SET(mins,  9999,  9999,  9999);
	V3F_SET(maxs, -9999, -9999, -9999);
	Mesh_BoxSelGroup(main_mdl, mins, maxs);

	if(mins[0]>maxs[0])return;	//nothing selected

	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);

	if((cmd==CMD_SUBDIVIDE) || (cmd==CMD_MERGE))
	{
		Mesh_CmdMSelGroup(main_mdl, cmd, org);
		return;
	}

	if(cmd==CMD_COPY)
	{
		if(main_clip)LBXGL_Mesh_DestroyGroup(main_clip);
		main_clip=LBXGL_Mesh_CreateGroup(NULL, "clip");
		Mesh_CopyMSelGroup(main_clip, main_mdl, cmd, org);
		return;
	}

	if(cmd==CMD_DUPLICATE)
	{
		Mesh_AddUndo();

		grp=LBXGL_Mesh_CreateGroup(NULL, "clip");
		Mesh_CopyMSelGroup(grp, main_mdl, cmd, org);

		V3F_COPY(org, io);
		io[0]-=1; io[1]-=1; io[2]-=1;

		Mesh_InsertGroup(main_mdl, grp, io);

		return;
	}

	Mesh_ClearOpSelGroup(main_mdl);
	Mesh_OpSelGroup(main_mdl);
	Mesh_CmdOpSelGroup(main_mdl, cmd, org);
}

MESH_API void Mesh_CmdSel(int cmd)
{
	if((cmd==CMD_COPY) || (cmd==CMD_CUT) || (cmd==CMD_DUPLICATE))
	{
		Mesh_CmdMultiSel(cmd);
		return;
	}

	if(sel_multi)Mesh_CmdMultiSel(cmd);
	else Mesh_CmdSelGroup(main_mdl, cmd);
}

MESH_API void Mesh_Paste()
{
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4];
	float f, g, v;

	if(!main_clip)return;

	Mesh_AddUndo();

//	V3F_ADD(org, cam->fw, io);
//	V3F_ADD(io, cam->up, io);
//	V3F_ADD(io, cam->rt, io);

	V3F_SET(mins,  9999,  9999,  9999);
	V3F_SET(maxs, -9999, -9999, -9999);

	LBXGL_Mesh_BoxGroup(main_clip, mins, maxs);
	f=MAX(V3F_LEN(mins), V3F_LEN(maxs));
	if(f<1.0)f=1.0;

//	printf("Paste Size %g\n", f);

	V3F_ADDSCALE(cam->org, cam->fw, 5*f, io);
//	printf("Paste At (%g %g %g)\n", io[0], io[1], io[2]);

	Mesh_InsertGroup(main_mdl, main_clip, io);
}


int ConCmds_Sky(char **a)
{
	LBXGL_Sky_LoadSky(a[1]);
	return(0);
}

int pdgl_main_startup(int argc, char *argv[])
{
	void *p;

//	PDGL_InitASM(argc, argv);
//	ccLoadModule("scripts/e_meshtool.c");

	GfxFont_Init();
	Con_Init();
	ConCmds_Init();

	PDGLUI_Init();
	UI_Camera_Init();

	ConCmds_Register("sky", "Set sky", ConCmds_Sky);

	p=BASM_GetPtr("meshtool_init");
//	printf("Got %p\n", p);

	if(p)
	{
		((int(*)(int argc, char **argv))p)(argc, argv);
	}

	p=BASM_GetPtr("meshtool_handle_input");
	meshtool_handle_input_fp=(int (*)())p;

	p=BASM_GetPtr("meshtool_draw_3d");
	meshtool_draw3d_fp=(int (*)())p;

	p=BASM_GetPtr("meshtool_draw_2d");
	meshtool_draw2d_fp=(int (*)())p;

	p=BASM_GetPtr("meshtool_menu_event");
	meshtool_menu_event_fp=(int (*)(char *))p;



	PDGLUI_BeginMenu("menu");

	PDGLUI_BeginSubMenu("&File");
	PDGLUI_MenuItem("new",		"&New           (CTRL--N)");
	PDGLUI_MenuItem("open",		"&Open          (CTRL--O)");
	PDGLUI_MenuItem("save",		"&Save          (CTRL--S)");
	PDGLUI_MenuItem("save_as",	"Save &As");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("&Edit");
	PDGLUI_MenuItem("edit_undo",	"Undo           (CTRL--Z)");
	PDGLUI_MenuItem("edit_redo",	"Redo           (CTRL--R)");
	PDGLUI_MenuItem(NULL, NULL);
	PDGLUI_MenuItem("edit_cut",	"Cut            (CTRL--X)");
	PDGLUI_MenuItem("edit_copy",	"Copy           (CTRL--C)");
	PDGLUI_MenuItem("edit_paste",	"Paste          (CTRL--V)");
	PDGLUI_MenuItem("edit_dup",	"Duplicate      (CTRL--D)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Mode");
	PDGLUI_MenuItem("mode_group",	"Group          (G)");
	PDGLUI_MenuItem("mode_object",	"Object         (O)");
	PDGLUI_MenuItem("mode_face",	"Face           (F)");
	PDGLUI_MenuItem("mode_vertex",	"Vertex         (V)");
	PDGLUI_MenuItem("mode_texture",	"Texture        (SHIFT--T)");

	PDGLUI_MenuItem(NULL, NULL);

	PDGLUI_MenuItem("mode_persp",	"Perspective 3D (SHIFT--P)");
	PDGLUI_MenuItem("mode_ortho",	"Ortho 3D       (SHIFT--O)");
	PDGLUI_MenuItem("mode_orth_xy",	"Ortho XY       (SHIFT--X)");
	PDGLUI_MenuItem("mode_orth_xz",	"Ortho XZ       (SHIFT--Z)");
	PDGLUI_MenuItem("mode_orth_yz",	"Ortho YZ       (SHIFT--Y)");
	PDGLUI_MenuItem("mode_cad4v",	"4 Views        (SHIFT--Q)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Tool");
	PDGLUI_MenuItem("tool_sel",	"Select         (S)");
	PDGLUI_MenuItem("tool_trans",	"Translate      (T)");
	PDGLUI_MenuItem("tool_rot",	"Rotate         (R)");
	PDGLUI_MenuItem("tool_scale",	"Scale          (SHIFT--S)");
	PDGLUI_MenuItem("tool_scale2",	"Scale HV       (SHIFT--A)");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Create");
	PDGLUI_MenuItem("create_cube",	"Cube");
	p=BASM_GetPtr("meshtool_menu_create");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_delete", 	"Delete Selection        (DEL)");
	PDGLUI_MenuItem("cmd_subdiv",	"Subdivide Object//Faces  (SHIFT--D)");
	PDGLUI_MenuItem("cmd_extrude", 	"Extrude Face            (SHIFT--E)");
	PDGLUI_MenuItem("cmd_merge", 	"Merge Vertices//Objects  (SHIFT--M)");
	PDGLUI_MenuItem("cmd_invert", 	"Invert//Flip Normal      (SHIFT--N)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");

	p=BASM_GetPtr("meshtool_menu_command");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Texture");
//	PDGLUI_MenuItem("tex_set",	"Set Texture             (CTRL--T)");
	PDGLUI_MenuItem("cmd_settex",	"Set Texture             (CTRL--T)");
	PDGLUI_MenuItem("tex_plane",	"Planar Projection       (CTRL--SHIFT--P)");
	PDGLUI_MenuItem("tex_box",	"Box Projection          (CTRL--SHIFT--B)");
	PDGLUI_MenuItem("tex_sphere",	"Spherical Projection    (CTRL--SHIFT--S)");
	PDGLUI_MenuItem("tex_cylinder",	"Cylindrical Projection  (CTRL--SHIFT--C)");

	PDGLUI_MenuItem(NULL, NULL);

	PDGLUI_MenuItem("tex_proj_xy",	"Project Texture XY      ()");
	PDGLUI_MenuItem("tex_proj_xz",	"Project Texture XZ      ()");

	PDGLUI_MenuItem(NULL, NULL);
	p=BASM_GetPtr("meshtool_menu_texture");
	if(p) ((int (*)())p)();
	PDGLUI_EndSubMenu();

	p=BASM_GetPtr("meshtool_menu_top");
	if(p) ((int (*)())p)();

	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();

#if 0
	PDGLUI_BeginSubMenu("Clear");
	PDGLUI_MenuItem("clear_all", "Clear All");
	PDGLUI_MenuItem("clear_bones", "Clear Bones");
	PDGLUI_MenuItem("clear_anim", "Clear Anim");
	PDGLUI_MenuItem("clear_mesh", "Clear Meshes");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Load");
	PDGLUI_MenuItem("load_bones", "Load &Bones");
	PDGLUI_MenuItem("load_anim", "Load &Anim");
	PDGLUI_MenuItem("load_model", "Load &Model");
	PDGLUI_MenuItem(NULL, NULL);
	PDGLUI_MenuItem("load_ac3d", "Import &AC3D");
	PDGLUI_MenuItem("load_smdref", "Import SMD Reference");
	PDGLUI_MenuItem("load_smdanim", "Import SMD Anim");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Save");
	PDGLUI_MenuItem("save_bones", "Save &Bones");
	PDGLUI_MenuItem("save_anim", "Save &Anim");
//	PDGLUI_MenuItem("save_model", "Save &Model");
//	PDGLUI_MenuItem("save_ac3d", "Save &AC3D");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Command");
	PDGLUI_MenuItem("cmd_rebind", "Rebind Meshes");
	PDGLUI_MenuItem("cmd_unbind", "Unbind Meshes");
	PDGLUI_EndSubMenu();

	PDGLUI_BeginSubMenu("Help");
	PDGLUI_MenuItem("help_generic", "General");
	PDGLUI_MenuItem("help_about", "About");
	PDGLUI_EndSubMenu();
#endif

	PDGLUI_EndMenu();

//	main_mdl=LBXGL_AC3D_LoadModel("model/monitor3.ac");
	main_mdl=gctalloc("lbxgl_meshgroup_t", sizeof(LBXGL_MeshGroup));

	UI_TexFinder_SetTitle("Textures");

//	cam->mode=PDGLUI_CAM_PERSP;
	cam->mode=PDGLUI_CAM_CAD4V;

	return(1);
}

int main_handle_input()
{
	char buf[64];
	float mins[3], maxs[3];
	float io[3], iv[4], fw[2], pt[4], f, g, v;
	float im[16], im1[16], *imv;

	int i, j, k;
	unsigned short *kcur;
	char *s;

	if(Con_IsDown() || main_loadsave)
		return(0);

	if(PDGLUI_HasFocus())
		return(0);

//	if(main_dirty && (pdgl_time>(main_lutime+500)))
//		Mesh_AddUndo();

//	i=((int (*)())BASM_GetFPtrDummy("meshtool_handle_input"))();
//	if(i>0)return(0);

	if(meshtool_handle_input_fp)
	{
		i=meshtool_handle_input_fp();
		if(i>0)return(0);
	}

	UI_Camera_Update();

	if(state->mb&4)
	{
		PDGLUI_ShowMenu("menu", state->mx, -state->my);
		return(0);
	}

	s=PDGLUI_GetMenuSelection("menu");
	if(s)
	{
		if(meshtool_menu_event_fp)
		{
			i=meshtool_menu_event_fp(s);
			if(i>0)return(0);
		}

		if(!strcmp(s, "new"))
		{
			main_mdl=gctalloc("lbxgl_meshgroup_t",
				sizeof(LBXGL_MeshGroup));
		}

		if(!strcmp(s, "open"))
		{
			UI_LoadSave_SetTitle("Open", "ac;acx");
			main_loadsave=1;
		}
		if(!strcmp(s, "save_as"))
		{
			UI_LoadSave_SetTitle("Save As", "ac;acx");
			main_loadsave=2;
		}
		if(!strcmp(s, "cmd_settex"))
		{
			UI_TexFinder_SetTitle("Textures");
			main_loadsave=16;
		}

		if(!strcmp(s, "edit_undo"))Mesh_Undo();
		if(!strcmp(s, "edit_redo"))Mesh_Redo();

		if(!strcmp(s, "edit_cut"))Mesh_CmdSel(CMD_CUT);
		if(!strcmp(s, "edit_copy"))Mesh_CmdSel(CMD_COPY);
		if(!strcmp(s, "edit_paste"))Mesh_Paste();
		if(!strcmp(s, "edit_dup"))Mesh_CmdSel(CMD_DUPLICATE);

		if(!strcmp(s, "mode_group"))
			{ main_mode=MODE_GROUP; Mesh_ClearSel(); }
//		if(!strcmp(s, "mode_object"))
//			{ main_mode=MODE_OBJECT; Mesh_ClearSel(); }
		if(!strcmp(s, "mode_face"))
			{ main_mode=MODE_FACE; Mesh_ClearSel(); }
		if(!strcmp(s, "mode_vertex"))
			{ main_mode=MODE_VERTEX; Mesh_ClearSel(); }

		if(!strcmp(s, "mode_object"))
		{
			if((main_mode!=MODE_OBJECT) &&
				(main_mode!=MODE_TEXTURE))
					Mesh_ClearSel();
			main_mode=MODE_OBJECT;
		}
		if(!strcmp(s, "mode_texture"))
		{
			if((main_mode!=MODE_OBJECT) &&
				(main_mode!=MODE_TEXTURE))
					Mesh_ClearSel();
			main_mode=MODE_TEXTURE;
		}

		if(!strcmp(s, "mode_persp"))cam->mode=PDGLUI_CAM_PERSP;
		if(!strcmp(s, "mode_ortho"))cam->mode=PDGLUI_CAM_ORTHO;
		if(!strcmp(s, "mode_orth_xy"))cam->mode=PDGLUI_CAM_XY;
		if(!strcmp(s, "mode_orth_xz"))cam->mode=PDGLUI_CAM_XZ;
		if(!strcmp(s, "mode_orth_yz"))cam->mode=PDGLUI_CAM_YZ;
		if(!strcmp(s, "mode_cad4v"))cam->mode=PDGLUI_CAM_CAD4V;

		if(!strcmp(s, "tool_sel"))main_cmd=CMD_SEL;
		if(!strcmp(s, "tool_trans"))main_cmd=CMD_TRANS;
		if(!strcmp(s, "tool_rot"))main_cmd=CMD_ROT;
		if(!strcmp(s, "tool_scale"))main_cmd=CMD_SCALE;
		if(!strcmp(s, "tool_scale2"))main_cmd=CMD_SCALE2;

		if(!strcmp(s, "create_cube"))
		{
			Mesh_AddUndo();
			Mesh_CreateCube();
		}

		if(!strcmp(s, "cmd_delete"))Mesh_CmdSel(CMD_DELETE);
		if(!strcmp(s, "cmd_extrude"))Mesh_CmdSel(CMD_EXTRUDE);
		if(!strcmp(s, "cmd_subdiv"))Mesh_CmdSel(CMD_SUBDIVIDE);
		if(!strcmp(s, "cmd_merge"))Mesh_CmdSel(CMD_MERGE);
		if(!strcmp(s, "cmd_invert"))Mesh_CmdSel(CMD_INVERT);

		if(!strcmp(s, "tex_plane") && sel_prim)
			sel_prim->tex_mode=LBXGL_TEX_PLANAR;
		if(!strcmp(s, "tex_box") && sel_prim)
			sel_prim->tex_mode=LBXGL_TEX_BOX;
		if(!strcmp(s, "tex_sphere") && sel_prim)
			sel_prim->tex_mode=LBXGL_TEX_SPHERICAL;
		if(!strcmp(s, "tex_cylinder") && sel_prim)
			sel_prim->tex_mode=LBXGL_TEX_CYLINDRICAL;

		if(!strcmp(s, "tex_proj_xy") && sel_prim)
			LBXGL_Mesh_ReprojectPrimXY(sel_prim);
		if(!strcmp(s, "tex_proj_xz") && sel_prim)
			LBXGL_Mesh_ReprojectPrimXZ(sel_prim);

		return(0);
	}


	kcur=state->keys;
	while(*kcur)
	{
		if(PDGL_KeyDown(K_CTRL))
		{
			if(*kcur=='z')Mesh_Undo();
			if(*kcur=='r')Mesh_Redo();

			if(*kcur=='c')Mesh_CmdSel(CMD_COPY);
			if(*kcur=='x')Mesh_CmdSel(CMD_CUT);
			if(*kcur=='d')Mesh_CmdSel(CMD_DUPLICATE);
			if(*kcur=='v')Mesh_Paste();

			if(*kcur=='o')
			{
				UI_LoadSave_SetTitle("Open", "ac;acx");
				main_loadsave=1;
			}

			if(*kcur=='s')
			{
				if(!main_file)
				{
					UI_LoadSave_SetTitle("Save As", "ac;acx");
					main_loadsave=2;
				}else
				{
//					LBXGL_AC3D_SaveModel(main_mdl, main_file);
					LBXGL_Mesh_SaveModel(main_file, main_mdl);
				}
			}

			if(*kcur=='t')
			{
				UI_TexFinder_SetTitle("Textures");
				main_loadsave=16;
			}

			if(*kcur=='P' && sel_prim)
				sel_prim->tex_mode=LBXGL_TEX_PLANAR;
			if(*kcur=='B' && sel_prim)
				sel_prim->tex_mode=LBXGL_TEX_BOX;
			if(*kcur=='S' && sel_prim)
				sel_prim->tex_mode=LBXGL_TEX_SPHERICAL;
			if(*kcur=='C' && sel_prim)
				sel_prim->tex_mode=LBXGL_TEX_CYLINDRICAL;

			kcur++;
			continue;
		}

		if(*kcur=='g') { main_mode=MODE_GROUP; Mesh_ClearSel(); }
		if(*kcur=='f') { main_mode=MODE_FACE; Mesh_ClearSel(); }
		if(*kcur=='v') { main_mode=MODE_VERTEX; Mesh_ClearSel(); }

		if(*kcur=='o')
		{
			if((main_mode!=MODE_OBJECT) &&
				(main_mode!=MODE_TEXTURE))
					Mesh_ClearSel();
			main_mode=MODE_OBJECT;
		}
		if(*kcur=='T')
		{
			if((main_mode!=MODE_OBJECT) &&
				(main_mode!=MODE_TEXTURE))
					Mesh_ClearSel();
			main_mode=MODE_TEXTURE;
		}

		if(*kcur=='s')main_cmd=CMD_SEL;
		if(*kcur=='t')main_cmd=CMD_TRANS;
		if(*kcur=='r')main_cmd=CMD_ROT;
		if(*kcur=='S')main_cmd=CMD_SCALE;
		if(*kcur=='A')main_cmd=CMD_SCALE2;

		if(*kcur=='P')cam->mode=PDGLUI_CAM_PERSP;
		if(*kcur=='O')cam->mode=PDGLUI_CAM_ORTHO;
		if(*kcur=='X')cam->mode=PDGLUI_CAM_XY;
		if(*kcur=='Z')cam->mode=PDGLUI_CAM_XZ;
		if(*kcur=='Y')cam->mode=PDGLUI_CAM_YZ;
		if(*kcur=='Q')cam->mode=PDGLUI_CAM_CAD4V;


		if(*kcur==K_ENTER)
		{
			if(!PDGL_KeyDown(K_SHIFT) && sel_multi)
				Mesh_ClearSel();
			Mesh_SelGroup(main_mdl);
		}

		if(*kcur==K_INS)
		{
			if(PDGL_KeyDown(K_SHIFT))
				Mesh_Paste();
		}

		if(*kcur==K_DEL)Mesh_CmdSel(CMD_DELETE);
		if(*kcur=='E')Mesh_CmdSel(CMD_EXTRUDE);
		if(*kcur=='D')Mesh_CmdSel(CMD_SUBDIVIDE);
		if(*kcur=='M')Mesh_CmdSel(CMD_MERGE);
		if(*kcur=='N')Mesh_CmdSel(CMD_INVERT);

		if((main_mode==MODE_VERTEX) && (*kcur=='F'))
			Mesh_CreateFace(sel_prim);

		kcur++;
	}

	if(PDGL_KeyDown(K_CTRL))
	{
		if(!(state->mb&1) && (state->lmb&1))
		{
			i=(state->mx>state->omx)?(state->mx-state->omx):(state->omx-state->mx);
			j=(state->my>state->omy)?(state->my-state->omy):(state->omy-state->my);

			if((i>5) && (j>5))
			{
//				printf("Sel Box\n");
				if(!PDGL_KeyDown(K_SHIFT))
					Mesh_ClearSel();
				Mesh_SelGroupBox(main_mdl);
				return(0);
			}
		}

		return(0);
	}

	if((main_cmd==CMD_SEL) && (state->mb&1) && !(state->lmb&1))
	{
		if(!PDGL_KeyDown(K_SHIFT) && sel_multi)
			Mesh_ClearSel();
		Mesh_SelGroup(main_mdl);
		return(0);
	}

	if((main_cmd==CMD_SEL) && !(state->mb&1) && (state->lmb&1))
	{
		i=(state->mx>state->omx)?(state->mx-state->omx):(state->omx-state->mx);
		j=(state->my>state->omy)?(state->my-state->omy):(state->omy-state->my);

		if((i>5) && (j>5))
		{
//			printf("Sel Box\n");
			if(!PDGL_KeyDown(K_SHIFT))
				Mesh_ClearSel();
			Mesh_SelGroupBox(main_mdl);
			return(0);
		}
	}

	if((main_cmd!=CMD_SEL) && (state->mb&1))
	{
//		Mesh_CmdSelGroup(main_mdl, main_cmd);
		Mesh_CmdSel(main_cmd);
		return(0);
	}

	return(0);
}

// void PD3D_DrawXYGrid2(float w, float s);
// void PD3D_DrawXZGrid2(float w, float s);
// void PD3D_DrawYZGrid2(float w, float s);

int Mesh_DrawScene()
{
	static int t;
	float pt[4];
	char buf[64];

	int i, j, k;
	char *s;

	if(!cam->ortho)
		LBXGL_Sky_DrawSky();

	glDisable(GL_CULL_FACE);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);

//	i=1;
//	if(cam->width>=(4*12))i=12;

	i=0;
	if(cam->width<=(4*12))i=1;
	if(cam->width<=4)i=2;

	glDisable(GL_TEXTURE_2D);
//	glDisable(GL_DEPTH_TEST);

//	glDepthFunc(GL_LEQUAL);
	glDepthFunc(GL_ALWAYS);

//	switch(cam->mode)
	switch(cam->ortho)
	{
	case 1:
	case 2:
		glColor4f(0.25, 0.25, 0.25, 1);
		if(i>1)PD3D_DrawXYGrid2(256, 1.0/16);
		glColor4f(0.5, 0.5, 0.5, 1);
		if(i)PD3D_DrawXYGrid2(256, 1);
		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawXYGrid2(256, 12);
		break;
	case 3:
		glColor4f(0.25, 0.25, 0.25, 1);
		if(i>1)PD3D_DrawXZGrid2(256, 1.0/16);
		glColor4f(0.5, 0.5, 0.5, 1);
		if(i)PD3D_DrawXZGrid2(256, 1);
		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawXZGrid2(256, 12);
		break;
	case 4:
		glColor4f(0.25, 0.25, 0.25, 1);
		if(i>1)PD3D_DrawYZGrid2(256, 1.0/16);
		glColor4f(0.5, 0.5, 0.5, 1);
		if(i)PD3D_DrawYZGrid2(256, 1);
		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawYZGrid2(256, 12);
		break;

	default:
#if 1
//		PD3D_DrawGrid(0, 0, 16, 1);

		if(fabs(cam->org[2])<50)
		{
			glColor4f(0.5, 0.5, 0.5, 1);
			PD3D_DrawXYGrid2(256, 1);
		}

		glColor4f(0.75, 0.75, 0.75, 1);
		PD3D_DrawXYGrid2(256, 12);

//		glColor4f(0.75, 0.5, 0.5, 1);
//		PD3D_DrawXYGrid2(256, 10);
#endif
		break;
	}

	glDepthFunc(GL_LEQUAL);
//	glEnable(GL_DEPTH_TEST);


	glColor4f(1, 1, 1, 1);
	glDisable(GL_CULL_FACE);

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

	if(main_mode!=MODE_FACE)
	{
		pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pt);

//		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, pt);
	}else
	{
		pt[0]=1; pt[1]=1; pt[2]=1; pt[3]=1;
		glMaterialfv(GL_FRONT, GL_DIFFUSE, pt);

//		glMaterialfv(GL_FRONT, GL_SPECULAR, pt);

		pt[0]=0.5; pt[1]=0.5; pt[2]=0.25; pt[3]=1;
		glMaterialfv(GL_BACK, GL_DIFFUSE, pt);
//		glMaterialfv(GL_BACK, GL_SPECULAR, pt);
	}

	UI_Camera_GetOrgFV(pt);
	pt[3]=1;
	glLightfv(GL_LIGHT0, GL_POSITION, pt);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 1.0/1000.0);
	glEnable(GL_LIGHT0);

	Mesh_DrawGroup(main_mdl);

	if(meshtool_draw3d_fp)
		meshtool_draw3d_fp();

	glDisable(GL_LIGHTING);

	LBXGL_Particle_Step(state->dt_f);
	LBXGL_Particle_Draw();

	LBXGL_Shader_FlushState();

//	return(-1);
}

int pdgl_main_body()
{
	static int t;
	float pt[4];
	char buf[64];

	int i, j, k;
	char *s;

	main_handle_input();

	if(cam->mode==PDGLUI_CAM_PERSP2)
		cam->mode=PDGLUI_CAM_PERSP;

	if(UI_Camera_4ViewsP())
	{
		UI_Camera_Setup3D_A();
		Mesh_DrawScene();
		UI_Camera_Setup3D_B();
		Mesh_DrawScene();
		UI_Camera_Setup3D_C();
		Mesh_DrawScene();
		UI_Camera_Setup3D_D();
		Mesh_DrawScene();
	}else
	{
		UI_Camera_Setup3D();
		Mesh_DrawScene();
	}

	UI_Camera_Setup2D();

	Con_Render();

	GfxFont_SetFont("fixed", 0);

#if 1
	GfxFont_SetFont("fixed", 0);

	sprintf(buf, "(%.3f %.3f %.3f) (%g %g %g)",
		cam->org[0], cam->org[1], cam->org[2],
		cam->ang[0], cam->ang[1], cam->ang[2]);
	GfxFont_DrawString(buf, -400, 300-12,
		8, 8,  0, 255, 0, 255);

	sprintf(buf, "(%.3f %.3f %.3f) (%.3f %.3f %.3f)",
		cam->morg[0], cam->morg[1], cam->morg[2],
		cam->mdir[0], cam->mdir[1], cam->mdir[2]);
	GfxFont_DrawString(buf, -400, 300-12-8,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(main_mode==MODE_GROUP)strcpy(buf, "Group");
	if(main_mode==MODE_OBJECT)strcpy(buf, "Object");
	if(main_mode==MODE_FACE)strcpy(buf, "Face");
	if(main_mode==MODE_VERTEX)strcpy(buf, "Vertex");
	if(main_mode==MODE_TEXTURE)strcpy(buf, "Texture");
	GfxFont_DrawString(buf, 400-(16*8), 300-12,
		8, 8,  0, 255, 0, 255);

	strcpy(buf, "");
	if(main_cmd==CMD_SEL)strcpy(buf, "Select");
	if(main_cmd==CMD_TRANS)strcpy(buf, "Translate");
	if(main_cmd==CMD_ROT)strcpy(buf, "Rotate");
	if(main_cmd==CMD_SCALE)strcpy(buf, "Scale");
	if(main_cmd==CMD_SCALE2)strcpy(buf, "Scale-HV");
	GfxFont_DrawString(buf, 400-(16*8), 300-12-8,
		8, 8,  0, 255, 0, 255);

	if(cam->mode==0)strcpy(buf, "Perspective 3D");
	if(cam->mode==1)strcpy(buf, "Ortho 3D");
	if(cam->mode==2)strcpy(buf, "Ortho XY");
	if(cam->mode==3)strcpy(buf, "Ortho XZ");
	if(cam->mode==4)strcpy(buf, "Ortho YZ");
	if(cam->mode==5)strcpy(buf, "Perspective 2");
	if(cam->mode==6)strcpy(buf, "4D");
	if(cam->mode==7)strcpy(buf, "4 Views");
	GfxFont_DrawString(buf, 400-(16*8), 300-12-16,
		8, 8,  0, 255, 0, 255);
#endif


	if(meshtool_draw2d_fp)
		meshtool_draw2d_fp();

	if(main_loadsave && (main_loadsave<16))
	{
		UI_LoadSave_Draw();
		s=UI_LoadSave_GetName();

		if(s)
		{
			if(*s && (main_loadsave==1))
			{
				main_file=dystring(s);
//				main_mdl=LBXGL_AC3D_LoadModel(s);
				main_mdl=LBXGL_Mesh_LoadModel(s);
			}
			if(*s && (main_loadsave==2))
			{
				main_file=dystring(s);
//				LBXGL_AC3D_SaveModel(main_mdl, s);
				LBXGL_Mesh_SaveModel(s, main_mdl);
			}
			main_loadsave=0;
		}
	}

	if(main_loadsave && (main_loadsave>=16))
	{
		UI_TexFinder_Draw();

		s=UI_TexFinder_GetName();
		if(s)
		{
			main_loadsave=0;
			if(sel_prim)
			{
				sprintf(buf, "textures/%s", s);
//				printf("Set Tex Prim %s\n", buf);

//				sel_prim->tex=dystring(buf);
//				sel_prim->texnum=Tex_LoadFile(buf, NULL, NULL);

				sel_prim->tex=dystring(buf);
				sel_prim->texnum=LBXGL_Texture_LoadImage(buf);

				printf("Set Tex Prim %s %d\n", buf, sel_prim->texnum);
			}
		}
	}


	PDGLUI_Frame();

	glDisable(GL_TEXTURE_2D);
	glColor4f(1, 0, 0, 1);

	glBegin(GL_LINES);
	glVertex2f(state->mx-10, -state->my);
	glVertex2f(state->mx+10, -state->my);
	glVertex2f(state->mx, -state->my-10);
	glVertex2f(state->mx, -state->my+10);
	glEnd();

	if(state->mb&1)
	{
		if((main_cmd==CMD_SEL) || PDGL_KeyDown(K_CTRL))
		{
			glColor4f(0.85, 0.85, 0.85, 1);

			glBegin(GL_LINES);
			glVertex2f(state->omx, -state->omy);
			glVertex2f(state->mx, -state->omy);

			glVertex2f(state->omx, -state->my);
			glVertex2f(state->mx, -state->my);

			glVertex2f(state->omx, -state->omy);
			glVertex2f(state->omx, -state->my);

			glVertex2f(state->mx, -state->omy);
			glVertex2f(state->mx, -state->my);
			glEnd();
		}else
		{
			glColor4f(0.85, 0.85, 0.85, 1);

			glBegin(GL_LINES);
			glVertex2f(state->omx, -state->omy);
			glVertex2f(state->mx, -state->my);
			glEnd();
		}
	}

	return(0);
}

int pdgl_main_shutdown()
{
	printf("shutdown\n");
	return(0);
}
