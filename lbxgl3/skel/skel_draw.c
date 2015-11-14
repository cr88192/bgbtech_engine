#include <lbxgl2.h>
#include <math.h>

extern int ac3d_draw_nhack;	//hack to adjust color based on normals
extern float ac3d_draw_color[4];

// static float skel_txyz[65536*3];
// static float skel_tnorm[16384*4];
// static float skel_tvnorm[65536*4];

// static LBXGL_Skel2Mesh *t_mesh;

float skel_a_xyz[65536*4];
float skel_a_norm[65536*4];
float skel_a_st[65536*2];
float skel_a_rgba[65536*4];
static float skel_norm[4];
static float skel_st[2];
static float skel_rgba[4];
static int skel_prim;
static int skel_nxyz;

void Skel_glNormal3fv(float *fv)
{
	skel_norm[0]=fv[0];
	skel_norm[1]=fv[1];
	skel_norm[2]=fv[2];
}

void Skel_glTexCoord2fv(float *fv)
{
	skel_st[0]=fv[0];
	skel_st[1]=fv[1];
}

void Skel_glColor4fv(float *fv)
{
	skel_rgba[0]=fv[0];
	skel_rgba[1]=fv[1];
	skel_rgba[2]=fv[2];
	skel_rgba[3]=fv[3];
}

void Skel_glColor4f(float r, float g, float b, float a)
{
	skel_rgba[0]=r;
	skel_rgba[1]=g;
	skel_rgba[2]=b;
	skel_rgba[3]=a;
}

void Skel_glVertex3fv(float *fv)
{
	float *xyz, *norm, *st, *rgba;
	int i;
	
	i=skel_nxyz++;

	if(i>=65536)
	{
		*(int *)-1=-1;
	}

	xyz=skel_a_xyz+i*4;
	norm=skel_a_norm+i*4;
	st=skel_a_st+i*2;
	rgba=skel_a_rgba+i*4;
	
	xyz[0]=fv[0];
	xyz[1]=fv[1];
	xyz[2]=fv[2];

	norm[0]=skel_norm[0];
	norm[1]=skel_norm[1];
	norm[2]=skel_norm[2];

	st[0]=skel_st[0];
	st[1]=skel_st[1];

	rgba[0]=skel_rgba[0];
	rgba[1]=skel_rgba[1];
	rgba[2]=skel_rgba[2];
	rgba[3]=skel_rgba[3];
}


void Skel_glBegin(int prim)
{
	skel_prim=prim;
	skel_nxyz=0;
}

void Skel_glEnd()
{
	int i;

	i=4*sizeof(float);
	LBXGL_Shader_DrawArraysNormalTexRGB(
		skel_prim, 0, skel_nxyz,
		3, GL_FLOAT, i, skel_a_xyz,
		2, GL_FLOAT, 0, skel_a_st,
		3, GL_FLOAT, i, skel_a_norm,
		4, GL_FLOAT, 0, skel_a_rgba);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	i=4*sizeof(float);
	glVertexPointer(3, GL_FLOAT, i, skel_a_xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, skel_a_st);
	glNormalPointer(GL_FLOAT, i, skel_a_norm);
	glColorPointer(4, GL_FLOAT, 0, skel_a_rgba);
	glDrawArrays(skel_prim, 0, skel_nxyz);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

//	QGL2_DrawShaderPrim(skel_prim,
//		skel_a_xyz, skel_a_st, skel_a_norm, skel_a_rgba, skel_nxyz);
#endif
}

void Skel_glVertex3fv2(float *fv)
{
	float *xyz, *norm, *st, *rgba;
	int i;
	
	i=skel_nxyz++;

	xyz=skel_a_xyz+i*4;
	norm=skel_a_norm+i*4;
	
	xyz[0]=fv[0];
	xyz[1]=fv[1];
	xyz[2]=fv[2];

	norm[0]=skel_norm[0];
	norm[1]=skel_norm[1];
	norm[2]=skel_norm[2];
}

void Skel_glEnd2()
{
	int i;

	i=4*sizeof(float);
	LBXGL_Shader_DrawArraysNormalFlat(
		skel_prim, 0, skel_nxyz,
		3, GL_FLOAT, i, skel_a_xyz,
		3, GL_FLOAT, i, skel_a_norm);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);

	i=4*sizeof(float);
	glVertexPointer(3, GL_FLOAT, i, skel_a_xyz);
//	glTexCoordPointer(2, GL_FLOAT, 0, skel_a_st);
	glNormalPointer(GL_FLOAT, i, skel_a_norm);
//	glColorPointer(4, GL_FLOAT, 0, skel_a_rgba);
	glDrawArrays(skel_prim, 0, skel_nxyz);

	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);

//	QGL2_DrawShaderPrim(skel_prim,
//		skel_a_xyz, skel_a_st, skel_a_norm, skel_a_rgba, skel_nxyz);
#endif
}



LBXGL_API void Skel_CalcMeshTempVPos(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh)
{
	float tv[3], tv0[3], tv1[3], tv2[3];
	LBXGL_Skel2MeshFace *cur;
	float *txyz;
	float *v0, *v1, *v2, *imv;
	int i, j, k, l, l1, tn;
	float f, g;

	if(!mesh->vidx || !mesh->vnum)return;
	if(!mesh->widx)return;

	txyz=ctx->cur_xyz+mesh->b_vecs*3;

	//calculate vertex poses
	for(i=0; i<mesh->n_vecs; i++)
	{
		V3F_ZERO(txyz+i*3);
		g=0;

		j=mesh->vidx[i];
		k=mesh->vnum[i];
		while(k--)
		{
			v0=mesh->wxyz+j*3;
			v1=ctx->bone_rorg+mesh->widx[j]*3;
			imv=ctx->bone_rmat+mesh->widx[j]*9;

			tv[0]=v0[0]*imv[0]+v0[1]*imv[3]+v0[2]*imv[6]+v1[0];
			tv[1]=v0[0]*imv[1]+v0[1]*imv[4]+v0[2]*imv[7]+v1[1];
			tv[2]=v0[0]*imv[2]+v0[1]*imv[5]+v0[2]*imv[8]+v1[2];

			f=mesh->wval[j];
			txyz[i*3+0]+=tv[0]*f;
			txyz[i*3+1]+=tv[1]*f;
			txyz[i*3+2]+=tv[2]*f;
			g+=f;

			j++;
		}

		if(g<0.01)g=1;
		txyz[i*3+0]/=g;
		txyz[i*3+1]/=g;
		txyz[i*3+2]/=g;

		txyz[i*3+0]=((txyz[i*3+0]-ctx->mdl->mdlorg[0])*
			ctx->mdl->mdlscale[0])+ctx->mdl->mdlxlat[0];
		txyz[i*3+1]=((txyz[i*3+1]-ctx->mdl->mdlorg[1])*
			ctx->mdl->mdlscale[1])+ctx->mdl->mdlxlat[1];
		txyz[i*3+2]=((txyz[i*3+2]-ctx->mdl->mdlorg[2])*
			ctx->mdl->mdlscale[2])+ctx->mdl->mdlxlat[2];
	}
}

LBXGL_API void Skel_CalcMeshTempFNorm(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh)
{
	float tv[3], tv0[3], tv1[3], tv2[3];
	LBXGL_Skel2MeshFace *cur;
	float *txyz, *tnorm;
	float *v0, *v1, *v2, *imv;
	int i, j, k, l, l1, tn;
	float f, g;

	txyz=ctx->cur_xyz+mesh->b_vecs*3;
	tnorm=ctx->cur_norm+mesh->b_faces*4;

	//calc face normals
	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];
		v0=txyz+cur->vecs[0]*3;
		v1=txyz+cur->vecs[1]*3;
		v2=txyz+cur->vecs[2]*3;
		V3F_SUB(v1, v0, tv0);
		V3F_SUB(v2, v1, tv1);
		V3F_CROSS(tv0, tv1, tv2);
		V3F_FRNORMALIZE(tv2, tnorm+i*4);
		tnorm[i*4+3]=V3F_DOT(v0, tnorm+i*4);
	}
}

LBXGL_API void Skel_CalcMeshTempPos2(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh);

LBXGL_API void Skel_CalcMeshTempVNorm(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh)
{
	LBXGL_Skel2MeshFace *cur, *cur2;
	float im[16];
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *txyz, *tnorm, *tvnorm;
	float *v0, *v1, *imv;
	int i, j, k, l, l1, tn, txfl;
	float f, g;

	if(!mesh)return;

	if(!mesh->vidx)
	{
//		Mat4F_Identity(im);
//		AC3D_DrawMesh(mesh, im);
		return;
	}

	Skel_CalcMeshTempPos2(ctx, mesh);

#if 1
	txfl=LBXGL_Texture_GetImageFlags(mesh->texnum[0]);
//	if((txfl&LBXGL_TXFL_LIGHTMAT) || (txfl&LBXGL_TXFL_WARP_MASK))
//		{ Skel_DrawMeshFlat2(ctx, mesh); return; }
#endif

	txyz=ctx->cur_xyz+mesh->b_vecs*3;
	tnorm=ctx->cur_norm+mesh->b_faces*4;
	tvnorm=ctx->cur_vnorm+mesh->b_faces*12;

	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];

		for(j=0; j<3; j++)
		{
			V3F_ZERO(tv);
			k=cur->vecs[j];
			
			if(txfl&LBXGL_TXFL_WARP_MASK)
			{
				//uniform normals in-case warp uses normals
				for(l=0; l<mesh->vfnum[k]; l++)
				{
					l1=mesh->vfids[mesh->vfidx[k]+l];
					v0=tnorm+i*4; v1=tnorm+l1*4;
					V3F_ADD(tv, v1, tv);
				}

				V3F_FRNORMALIZE(tv, tvnorm+i*12+j*4);
				continue;
			}
			
			for(l=0; l<mesh->vfnum[k]; l++)
			{
				l1=mesh->vfids[mesh->vfidx[k]+l];
				v0=tnorm+i*4;	v1=tnorm+l1*4;
				f=V3F_DOT(v0, v1);

				if(f>0)
				{
					if(f>1)f=1;
					V3F_ADDSCALE(tv, v1, f, tv);
				}
			}

			V3F_FRNORMALIZE(tv, tvnorm+i*12+j*4);
		}
	}
}

LBXGL_API void Skel_CalcMeshTempPos(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh)
{
	if(ctx->frame_calc)return;

	if(!mesh->vidx || !mesh->vnum)return;
	if(!mesh->widx)return;

	Skel_CalcMeshTempVPos(ctx, mesh);
	if(ctx->lod>2)return;
	Skel_CalcMeshTempFNorm(ctx, mesh);
	if(ctx->lod>1)return;
	Skel_CalcMeshTempVNorm(ctx, mesh);
	
//	ctx->frame_calc=1;
}

LBXGL_API void Skel_CalcMeshTempPos2(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh)
{
	if(ctx->frame_calc)return;

	Skel_CalcMeshTempVPos(ctx, mesh);
	Skel_CalcMeshTempFNorm(ctx, mesh);

//	ctx->frame_calc=1;
}

LBXGL_API void Skel_SetupMeshGroupTempPos(
	LBXGL_Skel2State *ctx, LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_SetupMeshGroupTempPos(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_CalcMeshTempPos(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_SetupMeshTempPos(LBXGL_Skel2State *ctx)
{
//	if(!ctx)return;
//	Skel_SetupFrame(ctx);
	Skel_SetupMeshGroupTempPos(ctx, Skel_MeshForLod(ctx));
}

#if 1
LBXGL_API void Skel_DrawMesh(LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *mesh)
{
	LBXGL_Skel2MeshFace *cur, *cur2;
	float im[16];
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *txyz, *tnorm, *tvnorm;
	float *v0, *v1, *imv;
	int i, j, k, l, l1, tn;
	float f, g;

	if(!mesh)return;

	if(!mesh->n_vecs || !mesh->n_faces)
		return;

	if(!mesh->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh, im);
		return;
	}

//	return;

	txyz=ctx->cur_xyz+mesh->b_vecs*3;
	tnorm=ctx->cur_norm+mesh->b_faces*4;
	tvnorm=ctx->cur_vnorm+mesh->b_faces*12;

	Skel_CalcMeshTempPos(ctx, mesh);
//	Skel_CalcMeshTempVNorm(ctx, mesh);

	if(ac3d_draw_nhack && ctx->lod)
	{
//		Skel_glColor4f(
		LBXGL_Shader_Color4f(
			ac3d_draw_color[0],
			ac3d_draw_color[1],
			ac3d_draw_color[2],
			ac3d_draw_color[3]);
	}

	for(tn=0; tn<mesh->n_tex; tn++)
	{
		glDisable(GL_TEXTURE_2D);
		if(mesh->texnum[tn]>=0)
		{
			pdglEnableTexture2D();
//			glBindTexture(GL_TEXTURE_2D, mesh->texnum[tn]);
			LBXGL_Texture_BindImage(mesh->texnum[tn]);
		}

//		pdglBegin(PDGL_TRIANGLES);
//		Skel_glBegin(GL_TRIANGLES);
		LBXGL_Shader_Begin(GL_TRIANGLES);
		i=mesh->texface[tn];
		while(1)
		{
			cur=mesh->face[i];

//			pdglNormal3fv(tnorm+i*4);
//			Skel_glNormal3fv(tnorm+i*4);

			for(j=0; j<3; j++)
			{
//				V3F_COPY(tvnorm+i*12+j*4, tv);
				V3F_COPY(tnorm+i*4, tv);

//				pdglNormal3fv(tv);
//				Skel_glNormal3fv(tv);
				LBXGL_Shader_Normal3fv(tv);

#if 1
				if(ac3d_draw_nhack && !ctx->lod)
				{
					f=1+0.100*tv[2]+
						0.025*tv[0]+
						0.025*tv[1];
//					glColor4f(
//					Skel_glColor4f(
					LBXGL_Shader_Color4f(
						ac3d_draw_color[0]*f,
						ac3d_draw_color[1]*f,
						ac3d_draw_color[2]*f,
						ac3d_draw_color[3]);
				}
#endif

//				pdglTexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
//				Skel_glTexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
				LBXGL_Shader_TexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
				V3F_COPY(txyz+(3*cur->vecs[j]), tv);

//				glVertex3fv(tv);
//				Skel_glVertex3fv(tv);
				LBXGL_Shader_Vertex3fv(tv);
			}

			i=cur->tnext;
			if(i<=0)break;
		}
//		pdglEnd();
//		Skel_glEnd();
		LBXGL_Shader_End();
	}
}

LBXGL_API void Skel_DrawMeshGroup(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshGroup(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMesh(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_DrawMeshGroupFinal(
	LBXGL_Skel2State *ctx, LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshGroupFinal(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		Skel_DrawMesh(ctx, mcur);
//		glBlendFunc(GL_SRC_COLOR, GL_ONE);
		glBlendFunc(GL_DST_COLOR, GL_ZERO);
		mcur=mcur->next;
	}
}
#endif

#if 1
LBXGL_API void Skel_DrawMeshFlat(LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *mesh)
{
	LBXGL_Skel2MeshFace *cur;
	float im[16];
	float *txyz, *tnorm, *tvnorm;
	int i, j, k, l, tn, txfl;

	if(!mesh)return;

	if(!mesh->n_vecs || !mesh->n_faces)
		return;

	if(!mesh->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh, im);
		return;
	}

#if 1
	txfl=LBXGL_Texture_GetImageFlags(mesh->texnum[0]);
	if((txfl&LBXGL_TXFL_LIGHTMAT) || (txfl&LBXGL_TXFL_WARP_MASK))
		{ Skel_DrawMeshFlat2(ctx, mesh); return; }
#endif

	txyz=ctx->cur_xyz+mesh->b_vecs*3;
	tnorm=ctx->cur_norm+mesh->b_faces*4;
	tvnorm=ctx->cur_vnorm+mesh->b_faces*12;


	Skel_CalcMeshTempPos(ctx, mesh);

//	pdglNormal3f(-vpn[0], -vpn[1], -vpn[2]);	//normal towards camera

	Skel_glBegin(GL_TRIANGLES);
	for(i=0; i<mesh->n_faces; i++)
	{
		cur=mesh->face[i];
		for(j=0; j<3; j++)
		{
			if(ctx->lod==0)
				Skel_glNormal3fv(tvnorm+i*12+j*4);
			else if(ctx->lod==1)
				Skel_glNormal3fv(tnorm+i*4);
			else {}
			Skel_glVertex3fv2(txyz+(3*cur->vecs[j]));
		}
	}
	Skel_glEnd2();
}

LBXGL_API void Skel_DrawMeshGroupFlat(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshGroupFlat(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshFlat(ctx, mcur);
		mcur=mcur->next;
	}
}
#endif

#if 1
LBXGL_API void Skel_DrawMeshFlat2(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh)
{
	LBXGL_Skel2MeshFace *cur, *cur2;
	float im[16];
	float tv[3], tv0[3], tv1[3], tv2[3];
	float *txyz, *tnorm, *tvnorm;
	float *v0, *v1, *imv;
	int i, j, k, l, l1, tn, txfl;
	float f, g;

	if(!mesh)return;

	if(!mesh->n_vecs || !mesh->n_faces)
		return;

	if(!mesh->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMeshFlat(mesh, im);
		return;
	}

	txyz=ctx->cur_xyz+mesh->b_vecs*3;
	tnorm=ctx->cur_norm+mesh->b_faces*4;
	tvnorm=ctx->cur_vnorm+mesh->b_faces*12;

	Skel_CalcMeshTempPos(ctx, mesh);

	for(tn=0; tn<mesh->n_tex; tn++)
	{
//		LBXGL_Texture_SetImageMaterial(mesh->texnum[tn]);
		LBXGL_Shader_BindTexture(mesh->texnum[tn]);
		LBXGL_Shader_SetCurrentMaterial();
		txfl=LBXGL_Shader_GetCurrentTextureFlags();
//		txfl=LBXGL_Shader_GetTextureFlags(mesh->texnum[tn]);

//		if(!(txfl&LBXGL_TXFL_LIGHTMAT) && !(txfl&LBXGL_TXFL_WARP_MASK))
		if(1)
		{
			Skel_glBegin(GL_TRIANGLES);
			i=mesh->texface[tn];
			while(1)
			{
				cur=mesh->face[i];

				Skel_glNormal3fv(tnorm+i*4);
//				LBXGL_Shader_Normal3fv(tnorm+i*4);

				for(j=0; j<3; j++)
				{
//					Skel_glNormal3fv(tnorm+i*4);
//					Skel_glVertex3fv2(txyz+(3*cur->vecs[j]));
//					Skel_glTexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
					Skel_glNormal3fv(tvnorm+i*12+j*4);
//					Skel_glVertex3fv(txyz+(3*cur->vecs[j]));
					Skel_glVertex3fv2(txyz+(3*cur->vecs[j]));
				}

				i=cur->tnext;
				if(i<=0)break;
			}
			Skel_glEnd2();
			
			continue;
		}

#if 0
//		LBXGL_Texture_SetImageMaterial(mesh->texnum[tn]);
//		LBXGL_Shader_BindTexture(mesh->texnum[tn]);

//		pdglBegin(PDGL_TRIANGLES);
//		Skel_glBegin(GL_TRIANGLES);
		LBXGL_Shader_BeginLight(GL_TRIANGLES);
		i=mesh->texface[tn];
		while(1)
		{
			cur=mesh->face[i];

//			pdglNormal3fv(tnorm+i*4);
//			Skel_glNormal3fv(tnorm+i*4);
			LBXGL_Shader_Normal3fv(tnorm+i*4);

			for(j=0; j<3; j++)
			{
//				Skel_glNormal3fv(tnorm+i*4);
//				Skel_glVertex3fv2(txyz+(3*cur->vecs[j]));
				LBXGL_Shader_TexCoord2fv(mesh->stvecs+(2*cur->stvecs[j]));
				LBXGL_Shader_Normal3fv(tvnorm+i*12+j*4);
				LBXGL_Shader_Vertex3fv(txyz+(3*cur->vecs[j]));
			}

			i=cur->tnext;
			if(i<=0)break;
		}
//		pdglEnd();
//		Skel_glEnd2();
		LBXGL_Shader_EndLight();
#endif
	}

	LBXGL_Shader_BindTexture(0);
}

LBXGL_API void Skel_DrawMeshGroupFlat2(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshGroup *grp)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshGroupFlat2(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshFlat2(ctx, mcur);
		mcur=mcur->next;
	}
}
#endif

LBXGL_API void Skel_DrawMeshShadowVolumeClip(
	LBXGL_Skel2State *ctx, LBXGL_Skel2Mesh *prim,
	float *lorg, float lrad)
{
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	int via[3];
	LBXGL_Skel2MeshFace *cur;
	float *txyz;
	int n, p;
	int i0, i1, i2;
	int i, j, k;

	return;

	txyz=ctx->cur_xyz+prim->b_vecs*3;

	Skel_CalcMeshTempPos2(ctx, prim);

	glDisable(GL_TEXTURE_2D);
	for(i=0; i<prim->n_faces; i++)
	{
		cur=prim->face[i];
		via[0]=cur->vecs[0];
		via[1]=cur->vecs[1];
		via[2]=cur->vecs[2];

		LBXGL_Brush_DrawPolyShadowVolumeDPOrgClip(
			txyz, via, 3, NULL, lorg, lrad, 0);
	}
}

LBXGL_API void Skel_DrawMeshShadow(LBXGL_Skel2State *ctx,
	LBXGL_Skel2Mesh *mesh, float *org, float rad, int flags)
{
	static float *xyzarr=NULL;
	static int *viarr=NULL;
	static int nxyz=0;
	static int nvia=0;
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	LBXGL_Skel2MeshFace *cur;
	float *txyz, *tnorm, *tvnorm;
	float *xyza;
	float *xyzb;
	float *nv, *nv0, *nv1, *tv, *xyz;
	float *v0, *v1, *imv;
	float f, g, h;
	int i, j, k, l, i0, i1, p, n;

	if(!mesh)return;

	if(!mesh->n_vecs || !mesh->n_faces)
		return;

	if(!mesh->vidx)
	{
//		Mat4F_Identity(im);
//		AC3D_DrawMesh(mesh, im);
		return;
	}

	if(!mesh->edge)Skel_BuildMeshEdges(mesh);

#if 0
//	if((flags&1) && LBXGL_Brush_CheckViewSphereShadow(
//		prim->com_org, prim->com_rad, org, rad))
	if(flags&1)
	{
		Skel_DrawMeshShadowVolumeClip(ctx, mesh, org, rad);
		return;
	}
#endif

	if(!xyzarr)
	{
		nxyz=256;
		while(nxyz<=mesh->n_vecs)nxyz+=nxyz>>1;
		xyzarr=malloc(2*nxyz*3*sizeof(float));
	}else if(nxyz<mesh->n_vecs)
	{
		free(xyzarr);
		while(nxyz<=mesh->n_vecs)nxyz+=nxyz>>1;
		xyzarr=malloc(2*nxyz*3*sizeof(float));
	}

	xyza=xyzarr; xyzb=xyza+nxyz*3;

	txyz=ctx->cur_xyz+mesh->b_vecs*3;
	tnorm=ctx->cur_norm+mesh->b_faces*4;
	tvnorm=ctx->cur_vnorm+mesh->b_faces*12;

	Skel_CalcMeshTempPos2(ctx, mesh);

//	i=mesh->n_edge*4;
//	j=mesh->n_faces*3;
//	i=(i<j)?i:j;

	i=(mesh->n_edge*6)+(mesh->n_faces*3);
	if(!viarr)
	{
		nvia=256;
		while(nvia<=i)nvia+=nvia>>1;
		viarr=malloc(nvia*sizeof(int));
	}else if(nvia<i)
	{
		free(viarr);
		while(nvia<=i)nvia+=nvia>>1;
		viarr=malloc(nvia*sizeof(int));
	}

	xyz=txyz;
	for(i=0; i<mesh->n_vecs; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SUB(tv0, org, dv0);
		V3F_FRNORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 1, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*rad, xyzb+i*3);
	}

#if 0
	n=0;
	for(i=0; i<mesh->n_edge; i++)
	{
		i0=mesh->edge[i*4+2]; i1=mesh->edge[i*4+3];

		nv0=tnorm+i0*4;
		nv1=tnorm+i1*4;

		f=V3F_NDOT(org, nv0);
		g=V3F_NDOT(org, nv1);

		if((f>0) && (g<=0))
		{
			i0=mesh->edge[i*4+0]; i1=mesh->edge[i*4+1];
			viarr[n++]=i1;			viarr[n++]=i0;
			viarr[n++]=i0+nxyz;		viarr[n++]=i1+nxyz;
		}else if((g>0) && (f<=0))
		{
			i0=mesh->edge[i*4+0]; i1=mesh->edge[i*4+1];
			viarr[n++]=i0;			viarr[n++]=i1;
			viarr[n++]=i1+nxyz;		viarr[n++]=i0+nxyz;
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, xyzarr);
	glDrawElements(GL_QUADS, n, GL_UNSIGNED_INT, viarr);
//	glDisableClientState(GL_VERTEX_ARRAY);

	n=0;
	for(i=0; i<mesh->n_faces; i++)
	{
		nv=tnorm+i*4;
		f=V3F_NDOT(org, nv);
//		j=(f>0)?0:nxyz;

		cur=mesh->face[i];
		if(f>0)
		{
			viarr[n++]=cur->vecs[0];
			viarr[n++]=cur->vecs[1];
			viarr[n++]=cur->vecs[2];
		}else
		{
			viarr[n++]=cur->vecs[0]+nxyz;
			viarr[n++]=cur->vecs[1]+nxyz;
			viarr[n++]=cur->vecs[2]+nxyz;
		}
	}
	glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, viarr);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

#if 1
	n=0;
	for(i=0; i<mesh->n_edge; i++)
	{
		i0=mesh->edge[i*4+2]; i1=mesh->edge[i*4+3];

		nv0=tnorm+i0*4;
		nv1=tnorm+i1*4;

		f=V3F_NDOT(org, nv0);
		g=V3F_NDOT(org, nv1);

		if((f>0) && (g<=0))
		{
			i0=mesh->edge[i*4+0]; i1=mesh->edge[i*4+1];
			viarr[n++]=i1; viarr[n++]=i0;
			viarr[n++]=i0+nxyz;

			viarr[n++]=i1; viarr[n++]=i0+nxyz;
			viarr[n++]=i1+nxyz;
		}else if((g>0) && (f<=0))
		{
			i0=mesh->edge[i*4+0]; i1=mesh->edge[i*4+1];
			viarr[n++]=i0; viarr[n++]=i1;
			viarr[n++]=i1+nxyz;		
			
			viarr[n++]=i0; viarr[n++]=i1+nxyz;
			viarr[n++]=i0+nxyz;
		}
	}

	for(i=0; i<mesh->n_faces; i++)
	{
		nv=tnorm+i*4;
		f=V3F_NDOT(org, nv);
//		j=(f>0)?0:nxyz;

		cur=mesh->face[i];
		if(f>0)
		{
			viarr[n++]=cur->vecs[0];
			viarr[n++]=cur->vecs[1];
			viarr[n++]=cur->vecs[2];
		}else
		{
			viarr[n++]=cur->vecs[0]+nxyz;
			viarr[n++]=cur->vecs[1]+nxyz;
			viarr[n++]=cur->vecs[2]+nxyz;
		}
	}

	LBXGL_Shader_DrawElementsSimpleFlat(
		GL_TRIANGLES, n, GL_UNSIGNED_INT, viarr,
		3, GL_FLOAT, 0, xyzarr);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, xyzarr);
	glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, viarr);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif
#endif
}

LBXGL_API void Skel_DrawMeshGroupShadow(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshGroup *grp, float *org, float rad, int flags)
{
	LBXGL_Skel2MeshGroup *gcur;
	LBXGL_Skel2Mesh *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshGroupShadow(ctx, gcur, org, rad, flags);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshShadow(ctx, mcur, org, rad, flags);
		mcur=mcur->next;
	}
}



#if 1
LBXGL_API void Skel_DrawMeshState(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshState *mesh)
{
	float im[16];
	LBXGL_Skel2MeshFace *cur;
	int i, j, k, l, l1, tn;

	if(!mesh)return;

	if(!mesh->prim->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh->prim, im);
		return;
	}

	for(tn=0; tn<mesh->prim->n_tex; tn++)
	{
		glDisable(GL_TEXTURE_2D);
		if(mesh->prim->texnum[tn]>=0)
		{
			pdglEnableTexture2D();
//			glBindTexture(GL_TEXTURE_2D, mesh->prim->texnum[tn]);
			LBXGL_Texture_BindImage(mesh->prim->texnum[tn]);
		}

		pdglBegin(PDGL_TRIANGLES);
		i=mesh->prim->texface[tn];
		while(1)
		{
			cur=mesh->prim->face[i];
			pdglNormal3fv(mesh->curnorms+i*4);
			for(j=0; j<3; j++)
			{
				pdglNormal3fv(mesh->curvnorms+i*12+j*4);
				pdglTexCoord2fv(mesh->prim->stvecs+(2*cur->stvecs[j]));
				pdglVertex3fv(mesh->curvecs+(3*cur->vecs[j]));
			}

			i=cur->tnext;
			if(i<=0)break;
		}
		pdglEnd();
	}
}
#endif

LBXGL_API void Skel_DrawMeshStateGroup(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroup(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshState(ctx, mcur);
		mcur=mcur->next;
	}
}


LBXGL_API void Skel_DrawMeshStateFlat(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshState *mesh)
{
	float im[16];
	LBXGL_Skel2MeshFace *cur;
	int i, j, k, l, tn;

	if(!mesh)return;

	if(!mesh->prim->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh->prim, im);
		return;
	}

	pdglBegin(PDGL_TRIANGLES);
	for(i=0; i<mesh->prim->n_faces; i++)
	{
		cur=mesh->prim->face[i];
		pdglVertex3fv(mesh->curvecs+(3*cur->vecs[0]));
		pdglVertex3fv(mesh->curvecs+(3*cur->vecs[1]));
		pdglVertex3fv(mesh->curvecs+(3*cur->vecs[2]));
	}
	pdglEnd();
}

LBXGL_API void Skel_DrawMeshStateFlat2(LBXGL_Skel2State *ctx, LBXGL_Skel2MeshState *mesh)
{
	LBXGL_Skel2MeshFace *cur;
	float im[16];
	int i, j, k, l, tn;

	if(!mesh)return;

	if(!mesh->prim->vidx)
	{
		Mat4F_Identity(im);
		AC3D_DrawMesh(mesh->prim, im);
		return;
	}

	pdglBegin(PDGL_TRIANGLES);
	for(i=0; i<mesh->prim->n_faces; i++)
	{
		cur=mesh->prim->face[i];
		for(j=0; j<3; j++)
		{
			pdglNormal3fv(mesh->curvnorms+i*12+j*4);
			pdglVertex3fv(mesh->curvecs+(3*cur->vecs[j]));
		}
	}
	pdglEnd();
}

LBXGL_API void Skel_DrawMeshStateGroupFlat(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroupFlat(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshStateFlat(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_DrawMeshStateGroupFlat2(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	if(!grp)return;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroupFlat2(ctx, gcur);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshStateFlat2(ctx, mcur);
		mcur=mcur->next;
	}
}

LBXGL_API void Skel_DrawMeshStateShadow(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshState *mesh, float *org, float rad)
{
	static float *xyza=NULL;
	static float *xyzb=NULL;
	static int nxyz;
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3];
	LBXGL_Skel2Mesh *prim;
	LBXGL_Skel2MeshFace *cur;
	float *nv, *nv0, *nv1, *tv, *xyz;
	float f, g, h;
	int i, j, k, l, i0, i1, p, n;

	prim=mesh->prim;

	if(!prim->edge)Skel_BuildMeshEdges(prim);

	if(!xyza)
	{
		nxyz=256;
		while(nxyz<=prim->n_vecs)nxyz+=nxyz>>1;
		xyza=malloc(nxyz*3*sizeof(float));
		xyzb=malloc(nxyz*3*sizeof(float));
	}

	if(nxyz<prim->n_vecs)
	{
		free(xyza); free(xyzb);
		while(nxyz<=prim->n_vecs)nxyz+=nxyz>>1;
		xyza=malloc(nxyz*3*sizeof(float));
		xyzb=malloc(nxyz*3*sizeof(float));
	}


	xyz=mesh->curvecs;
	for(i=0; i<prim->n_vecs; i++)
	{
		V3F_COPY(xyz+i*3, tv0);
		V3F_SUB(tv0, org, dv0);
		V3F_NORMALIZE(dv0, dv0);
		V3F_ADDSCALE(tv0, dv0, 1, xyza+i*3);
		V3F_ADDSCALE(tv0, dv0, 2*rad, xyzb+i*3);
	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<prim->n_edge; i++)
	{
		i0=prim->edge[i*4+2]; i1=prim->edge[i*4+3];
		nv0=mesh->curnorms+i0*4;
		nv1=mesh->curnorms+i1*4;

		f=V3F_NDOT(org, nv0);
		g=V3F_NDOT(org, nv1);

		if((f>0) && (g<=0))
		{
			i0=prim->edge[i*4+0]; i1=prim->edge[i*4+1];
			pdglVertex3fv(xyza+i1*3); pdglVertex3fv(xyza+i0*3);
			pdglVertex3fv(xyzb+i0*3); pdglVertex3fv(xyzb+i1*3);
		}

		if((g>0) && (f<=0))
		{
			i0=prim->edge[i*4+0]; i1=prim->edge[i*4+1];
			pdglVertex3fv(xyza+i0*3); pdglVertex3fv(xyza+i1*3);
			pdglVertex3fv(xyzb+i1*3); pdglVertex3fv(xyzb+i0*3);
		}
	}
	pdglEnd();

	pdglBegin(PDGL_TRIANGLES);
	for(i=0; i<prim->n_faces; i++)
	{
		nv=mesh->curnorms+i*4;
		f=V3F_NDOT(org, nv);
		if(f>0)xyz=xyza; else xyz=xyzb;

		cur=prim->face[i];
		pdglVertex3fv(xyz+(3*cur->vecs[0]));
		pdglVertex3fv(xyz+(3*cur->vecs[1]));
		pdglVertex3fv(xyz+(3*cur->vecs[2]));
	}
	pdglEnd();
}

LBXGL_API void Skel_DrawMeshStateGroupShadow(LBXGL_Skel2State *ctx,
	LBXGL_Skel2MeshStateGroup *grp, float *org, float rad)
{
	LBXGL_Skel2MeshStateGroup *gcur;
	LBXGL_Skel2MeshState *mcur;

	gcur=grp->first;
	while(gcur)
	{
		Skel_DrawMeshStateGroupShadow(ctx, gcur, org, rad);
		gcur=gcur->next;
	}

	mcur=grp->prim;
	while(mcur)
	{
		Skel_DrawMeshStateShadow(ctx, mcur, org, rad);
		mcur=mcur->next;
	}
}


LBXGL_Skel2MeshGroup *Skel_MeshForLod(LBXGL_Skel2State *ctx)
{
	if((ctx->lod>=8) && ctx->mdl->mesh_lod[7])
		return(ctx->mdl->mesh_lod[7]);
	if((ctx->lod>=7) && ctx->mdl->mesh_lod[6])
		return(ctx->mdl->mesh_lod[6]);
	if((ctx->lod>=6) && ctx->mdl->mesh_lod[5])
		return(ctx->mdl->mesh_lod[5]);
	if((ctx->lod>=5) && ctx->mdl->mesh_lod[4])
		return(ctx->mdl->mesh_lod[4]);
	if((ctx->lod>=4) && ctx->mdl->mesh_lod[3])
		return(ctx->mdl->mesh_lod[3]);
	if((ctx->lod>=3) && ctx->mdl->mesh_lod[2])
		return(ctx->mdl->mesh_lod[2]);
	if((ctx->lod>=2) && ctx->mdl->mesh_lod[1])
		return(ctx->mdl->mesh_lod[1]);
	if((ctx->lod>=1) && ctx->mdl->mesh_lod[0])
		return(ctx->mdl->mesh_lod[0]);
	return(ctx->mdl->mesh);
}

LBXGL_API void Skel_DrawModel(LBXGL_Skel2State *ctx)
{
	if(!ctx)return;

//	pdglColor4f(1, 1, 1, 1);
	Skel_glColor4f(1, 1, 1, 1);

	Skel_SetupFrame(ctx);
	if(ctx->mesh)Skel_DrawMeshStateGroup(ctx, ctx->mesh);
		else Skel_DrawMeshGroup(ctx, Skel_MeshForLod(ctx));
}

LBXGL_API void Skel_DrawModelAlpha(LBXGL_Skel2State *ctx, float alpha)
{
	if(!ctx)return;

//	glColor4f(1, 1, 1, alpha);
	Skel_glColor4f(1, 1, 1, alpha);

	Skel_SetupFrame(ctx);
	if(ctx->mesh)Skel_DrawMeshStateGroup(ctx, ctx->mesh);
		else Skel_DrawMeshGroup(ctx, Skel_MeshForLod(ctx));
}

LBXGL_API void Skel_DrawModelDark(LBXGL_Skel2State *ctx)
{
	if(!ctx)return;

//	if(!ctx->mesh)ctx->mesh=Skel_NewGroupInstance(ctx->mdl->mesh);
	Skel_SetupFrame(ctx);

	glDisable(GL_TEXTURE_2D);
//	glColor4f(0, 0, 0, 1);
	Skel_glColor4f(0, 0, 0, 1);

	if(ctx->mesh)
	{
		Skel_DrawMeshStateGroupFlat(ctx, ctx->mesh);
	}else
	{
//		Skel_DrawMeshGroupFlat(ctx, Skel_MeshForLod(ctx));
		SkelVBO_DrawModelFlatVL(ctx);
	}
}

LBXGL_API void Skel_DrawModelLight(LBXGL_Skel2State *ctx)
{
	if(!ctx)return;
	Skel_SetupFrame(ctx);

	glDisable(GL_TEXTURE_2D);
//	glColor4f(1, 1, 1, 1);
	Skel_glColor4f(1, 1, 1, 1);

	if(ctx->mesh)
	{
		Skel_DrawMeshStateGroupFlat2(ctx, ctx->mesh);
	}else
	{
//		Skel_DrawMeshGroupFlat2(ctx, Skel_MeshForLod(ctx));
		SkelVBO_DrawModelFlat(ctx);
	}
}

LBXGL_API void Skel_DrawModelLightColor(
	LBXGL_Skel2State *ctx, float cr, float cg, float cb)
{
	if(!ctx)return;
	Skel_SetupFrame(ctx);

	glDisable(GL_TEXTURE_2D);
//	glColor4f(1, 1, 1, 1);
//	Skel_glColor4f(1, 1, 1, 1);

//	glColor4f(cr, cg, cb, 1);
	Skel_glColor4f(cr, cg, cb, 1);

	if(ctx->mesh)Skel_DrawMeshStateGroupFlat2(ctx, ctx->mesh);
		else Skel_DrawMeshGroupFlat2(ctx, Skel_MeshForLod(ctx));
}

LBXGL_API void Skel_DrawModelFlat(LBXGL_Skel2State *ctx)
{
	if(!ctx)return;
	Skel_SetupFrame(ctx);

	if(ctx->mesh)Skel_DrawMeshStateGroupFlat2(ctx, ctx->mesh);
		else Skel_DrawMeshGroupFlat(ctx, Skel_MeshForLod(ctx));
}

LBXGL_API void Skel_DrawModelFinal(LBXGL_Skel2State *ctx)
{
	if(!ctx)return;
	Skel_SetupFrame(ctx);

//	glDisable(GL_CULL_FACE);
//	glShadeModel (GL_SMOOTH);
//	GL_TexEnv( GL_MODULATE );
//	glEnable (GL_BLEND);

//	GL_TexEnv( GL_REPLACE );


	pdglEnableTexture2D();
//	glColor4f(1, 1, 1, 1);
	Skel_glColor4f(1, 1, 1, 1);

	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	if(ctx->mesh)
	{
		Skel_DrawMeshStateGroup(ctx, ctx->mesh);
	}else
	{
//		Skel_DrawMeshGroupFinal(ctx, Skel_MeshForLod(ctx));
		SkelVBO_DrawModelFinal(ctx);
	}
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
}

LBXGL_API void Skel_DrawModelShadow(LBXGL_Skel2State *ctx, float *org, float rad)
{
	if(!ctx)return;
//	Skel_SetupFrame(ctx);
	if(!(ctx->frame_calc))
		return;

	if(ctx->mesh)
	{
		Skel_DrawMeshStateGroupShadow(ctx, ctx->mesh, org, rad);
	}else
	{
		Skel_DrawMeshGroupShadow(ctx,
			Skel_MeshForLod(ctx), org, rad, 0);
	}
}

LBXGL_API void Skel_DrawModelShadowFlags(LBXGL_Skel2State *ctx,
	float *org, float rad, int flags)
{
	if(!ctx)return;
//	Skel_SetupFrame(ctx);
	if(!(ctx->frame_calc))
		return;

	if(ctx->mesh)
	{
		Skel_DrawMeshStateGroupShadow(ctx, ctx->mesh, org, rad);
	}else
	{
		Skel_DrawMeshGroupShadow(ctx, Skel_MeshForLod(ctx),
			org, rad, flags);
	}
}

LBXGL_API int Skel_DrawFlatCubeOrgRot(LBXGL_Skel2State *ctx,
	float *mins, float *maxs, float *org, float *rot)
{
	static int vtxb[6*4]={
		1, 3, 2, 0,  4, 6, 7, 5,
		5, 7, 3, 1,  0, 2, 6, 4,
		5, 1, 0, 4,  3, 7, 6, 2};
	float vtxa[8*3];
	float tv0[3], tv1[3];
	int i, j;

	for(i=0; i<8; i++)
	{
		tv0[0]=(i&1)?maxs[0]:mins[0];
		tv0[1]=(i&2)?maxs[1]:mins[1];
		tv0[2]=(i&4)?maxs[2]:mins[2];
		V3F_ADDSCALE(org, rot+0, tv0[0], tv1);
		V3F_ADDSCALE(tv1, rot+3, tv0[1], tv1);
		V3F_ADDSCALE(tv1, rot+6, tv0[2], tv1);

		tv1[0]=((tv1[0]-ctx->mdl->mdlorg[0])*
			ctx->mdl->mdlscale[0])+ctx->mdl->mdlxlat[0];
		tv1[1]=((tv1[1]-ctx->mdl->mdlorg[1])*
			ctx->mdl->mdlscale[1])+ctx->mdl->mdlxlat[1];
		tv1[2]=((tv1[2]-ctx->mdl->mdlorg[2])*
			ctx->mdl->mdlscale[2])+ctx->mdl->mdlxlat[2];

		V3F_COPY(tv1, vtxa+(i*3));
	}

	pdglBegin(PDGL_QUADS);
	for(i=0; i<6; i++)
	{
		pdglVertex3fv(vtxa+(vtxb[i*4+0]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+1]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+2]*3));
		pdglVertex3fv(vtxa+(vtxb[i*4+3]*3));
	}
	pdglEnd();
	return(0);
}

LBXGL_API void Skel_DrawModelFlatApprox(LBXGL_Skel2State *ctx)
{
	int i;

	if(!ctx)return;
//	Skel_SetupFrame(ctx);
	Skel_SetupFrameBasic(ctx);

	for(i=0; i<ctx->mdl->num_bones; i++)
	{
		Skel_DrawFlatCubeOrgRot(ctx,
			ctx->mdl->bone_mins+(i*3),
			ctx->mdl->bone_maxs+(i*3),
			ctx->bone_rorg+(i*3),
			ctx->bone_rmat+(i*9));
	}

//	if(ctx->mesh)Skel_DrawMeshStateGroupFlat2(ctx, ctx->mesh);
//		else Skel_DrawMeshGroupFlat(ctx, Skel_MeshForLod(ctx));
}

LBXGL_API void Skel_DrawSetLOD(LBXGL_Skel2State *ctx, int lod)
{
	if(ctx->lod!=lod)
	{
		ctx->lod=lod;
		ctx->frame_calc=0;	//force recalc
	}
}

LBXGL_API int Skel_DrawGetLOD(LBXGL_Skel2State *ctx)
{
	return(ctx->lod);
}



LBXGL_API int Skel_BoxCubeOrgRot(LBXGL_Skel2State *ctx,
	float *mins, float *maxs, float *org, float *rot,
	float *rmins, float *rmaxs)
{
	static int vtxb[6*4]={
		1, 3, 2, 0,  4, 6, 7, 5,
		5, 7, 3, 1,  0, 2, 6, 4,
		5, 1, 0, 4,  3, 7, 6, 2};
	float vtxa[8*3];
	float tv0[3], tv1[3];
	int i, j;

	for(i=0; i<8; i++)
	{
		tv0[0]=(i&1)?maxs[0]:mins[0];
		tv0[1]=(i&2)?maxs[1]:mins[1];
		tv0[2]=(i&4)?maxs[2]:mins[2];
		V3F_ADDSCALE(org, rot+0, tv0[0], tv1);
		V3F_ADDSCALE(tv1, rot+3, tv0[1], tv1);
		V3F_ADDSCALE(tv1, rot+6, tv0[2], tv1);

		tv1[0]=((tv1[0]-ctx->mdl->mdlorg[0])*
			ctx->mdl->mdlscale[0])+ctx->mdl->mdlxlat[0];
		tv1[1]=((tv1[1]-ctx->mdl->mdlorg[1])*
			ctx->mdl->mdlscale[1])+ctx->mdl->mdlxlat[1];
		tv1[2]=((tv1[2]-ctx->mdl->mdlorg[2])*
			ctx->mdl->mdlscale[2])+ctx->mdl->mdlxlat[2];

		if(tv1[0]<rmins[0])rmins[0]=tv1[0];
		if(tv1[1]<rmins[1])rmins[1]=tv1[1];
		if(tv1[2]<rmins[2])rmins[2]=tv1[2];

		if(tv1[0]>rmaxs[0])rmaxs[0]=tv1[0];
		if(tv1[1]>rmaxs[1])rmaxs[1]=tv1[1];
		if(tv1[2]>rmaxs[2])rmaxs[2]=tv1[2];

//		V3F_COPY(tv1, vtxa+(i*3));
	}

	return(0);
}

LBXGL_API void Skel_BoxModelApprox(LBXGL_Skel2State *ctx,
	float *rmins, float *rmaxs)
{
	int i;

	if(!ctx)return;
//	Skel_SetupFrame(ctx);
	Skel_SetupFrameBasic(ctx);

	for(i=0; i<ctx->mdl->num_bones; i++)
	{
		Skel_BoxCubeOrgRot(ctx,
			ctx->mdl->bone_mins+(i*3),
			ctx->mdl->bone_maxs+(i*3),
			ctx->bone_rorg+(i*3),
			ctx->bone_rmat+(i*9),
			rmins, rmaxs);
	}

//	if(ctx->mesh)Skel_DrawMeshStateGroupFlat2(ctx, ctx->mesh);
//		else Skel_DrawMeshGroupFlat(ctx, Skel_MeshForLod(ctx));
}
