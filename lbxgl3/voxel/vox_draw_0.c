#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;

static int lbxgl_voxel_chkleft;
static int lbxgl_voxel_chkstart;

static int lbxgl_voxel_reforg[3];

int LBXGL_Voxel_MapTexture(LBXGL_VoxelChunk *chk, int id)
{
	BTGE_TexImage *cur;
	int tn;

	cur=BTGE_Texture_LookupNum(id);
	if(!cur)return(-1);
	if(cur->altnum)
		return(cur->altnum);
	tn=LBXGL_Texture_LoadImage(cur->name);
	cur->altnum=tn;
	return(tn);
}

int LBXGL_Voxel_MapTexture2(LBXGL_VoxelChunk *chk, int id)
{
	LBXGL_TexImage *img;
	int tn, tn1;

	tn=LBXGL_Voxel_MapTexture(chk, id);
	LBXGL_Shader_BindTexture(tn);
	tn1=LBXGL_Shader_GetCurrentTextureImage();
	return(tn1);
}

void LBXGL_Voxel_BoxChunk(LBXGL_VoxelChunk *chk,
	float *mins, float *maxs)
{
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l;

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

//	mins[0]=chk->ox*vsz-vsz2;
//	mins[1]=chk->oy*vsz-vsz2;
//	mins[2]=chk->oz*vsz-vsz2;

	mins[0]=chk->ox*vsz;
	mins[1]=chk->oy*vsz;
	mins[2]=chk->oz*vsz;
	maxs[0]=mins[0]+chk->xs*vsz;
	maxs[1]=mins[1]+chk->ys*vsz;
	maxs[2]=mins[2]+chk->zs*vsz;
}

void LBXGL_Voxel_BoxRegion(LBXGL_VoxelRegion *rgn,
	float *mins, float *maxs)
{
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l;

	vsz=rgn->voxsz;
	vsz2=vsz*0.5;

//	mins[0]=rgn->ox*vsz-vsz2;
//	mins[1]=rgn->oy*vsz-vsz2;
//	mins[2]=rgn->oz*vsz-vsz2;
	mins[0]=rgn->ox*vsz;
	mins[1]=rgn->oy*vsz;
	mins[2]=rgn->oz*vsz;
	maxs[0]=mins[0]+(rgn->xs*rgn->xst)*vsz;
	maxs[1]=mins[1]+(rgn->ys*rgn->yst)*vsz;
	maxs[2]=mins[2]+(rgn->zs*rgn->zst)*vsz;
}


void LBXGL_Voxel_BoxChunkRel(LBXGL_VoxelChunk *chk,
	float *mins, float *maxs)
{
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l;

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

//	mins[0]=(chk->ox*vsz-vsz2)-lbxgl_voxel_reforg[0];
//	mins[1]=(chk->oy*vsz-vsz2)-lbxgl_voxel_reforg[1];
//	mins[2]=(chk->oz*vsz-vsz2)-lbxgl_voxel_reforg[2];
	mins[0]=(chk->ox*vsz)-lbxgl_voxel_reforg[0];
	mins[1]=(chk->oy*vsz)-lbxgl_voxel_reforg[1];
	mins[2]=(chk->oz*vsz)-lbxgl_voxel_reforg[2];
	maxs[0]=mins[0]+chk->xs*vsz;
	maxs[1]=mins[1]+chk->ys*vsz;
	maxs[2]=mins[2]+chk->zs*vsz;
}

void LBXGL_Voxel_BoxRegionRel(LBXGL_VoxelRegion *rgn,
	float *mins, float *maxs)
{
	float vsz, vsz2;
	int x,y,z;
	int i, j, k, l;

	vsz=rgn->voxsz;
	vsz2=vsz*0.5;

//	mins[0]=(rgn->ox*vsz-vsz2)-lbxgl_voxel_reforg[0];
//	mins[1]=(rgn->oy*vsz-vsz2)-lbxgl_voxel_reforg[1];
//	mins[2]=(rgn->oz*vsz-vsz2)-lbxgl_voxel_reforg[2];
	mins[0]=(rgn->ox*vsz)-lbxgl_voxel_reforg[0];
	mins[1]=(rgn->oy*vsz)-lbxgl_voxel_reforg[1];
	mins[2]=(rgn->oz*vsz)-lbxgl_voxel_reforg[2];
	maxs[0]=mins[0]+(rgn->xs*rgn->xst)*vsz;
	maxs[1]=mins[1]+(rgn->ys*rgn->yst)*vsz;
	maxs[2]=mins[2]+(rgn->zs*rgn->zst)*vsz;
}

bool LBXGL_Voxel_CheckChunkFrustum(LBXGL_VoxelChunk *chk)
{
	float mins[3], maxs[3], org[3];
	float f, g;
	
//	LBXGL_Voxel_BoxChunk(chk, mins, maxs);
	LBXGL_Voxel_BoxChunkRel(chk, mins, maxs);

//	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
//		return(false);
	
//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 8192))
	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 4096))
//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 6144))
		return(false);
	
	return(true);
}

bool LBXGL_Voxel_CheckChunkLight(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
	float mins[3], maxs[3];
	LBXGL_Voxel_BoxChunkRel(chk, mins, maxs);
//	if(!BTGE_Brush_CheckBBoxFrustum(mins, maxs))
//		return(false);
	if(!SolidAABB_BoxCollideP(mins, maxs, light->amins, light->amaxs))
		return(false);
	return(true);
}

bool LBXGL_Voxel_CheckRegionLight(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	float mins[3], maxs[3];
	LBXGL_Voxel_BoxRegionRel(rgn, mins, maxs);
//	if(!BTGE_Brush_CheckBBoxFrustum(mins, maxs))
//		return(false);
	if(!SolidAABB_BoxCollideP(mins, maxs, light->amins, light->amaxs))
		return(false);
	return(true);
}

float LBXGL_Voxel_GetChunkFrustumDistance(LBXGL_VoxelChunk *chk)
{
	float mins[3], maxs[3], org[3];
	float f, g;
	
//	LBXGL_Voxel_BoxChunk(chk, mins, maxs);
	LBXGL_Voxel_BoxChunkRel(chk, mins, maxs);
	return(LBXGL_Brush_GetBBoxFrustumDistance(mins, maxs));
}

bool LBXGL_Voxel_CheckRegionFrustum(LBXGL_VoxelRegion *rgn)
{
	float mins[3], maxs[3], org[3];
	float f, g;
	
//	LBXGL_Voxel_BoxRegion(rgn, mins, maxs);
	LBXGL_Voxel_BoxRegionRel(rgn, mins, maxs);

	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(false);

//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 4096))
//		return(false);
	
	return(true);
}

bool LBXGL_Voxel_CheckRegionChunkFrustumDistance(
	LBXGL_VoxelRegion *rgn, int x, int y, int z, float dist)
{
	float mins[3], maxs[3];
	float vsz, vsz2;
	float f, g;
	int i, j, k, l;

	vsz=rgn->voxsz;
	vsz2=vsz*0.5;

//	mins[0]=((rgn->ox+x)*vsz-vsz2)-lbxgl_voxel_reforg[0];
//	mins[1]=((rgn->oy+y)*vsz-vsz2)-lbxgl_voxel_reforg[1];
//	mins[2]=((rgn->oz+z)*vsz-vsz2)-lbxgl_voxel_reforg[2];
	mins[0]=((rgn->ox+(x*rgn->xst))*vsz)-lbxgl_voxel_reforg[0];
	mins[1]=((rgn->oy+(y*rgn->yst))*vsz)-lbxgl_voxel_reforg[1];
	mins[2]=((rgn->oz+(z*rgn->zst))*vsz)-lbxgl_voxel_reforg[2];
	maxs[0]=mins[0]+rgn->xst*vsz;
	maxs[1]=mins[1]+rgn->yst*vsz;
	maxs[2]=mins[2]+rgn->zst*vsz;

	if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
		return(false);
	
	f=LBXGL_Brush_GetBBoxFrustumDistance(mins, maxs);
	if(f>dist)return(false);
	
	return(true);
}

void LBXGL_Voxel_DrawChunkSimpleFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->n_face)
		return;

//	glEnableClientState(GL_VERTEX_ARRAY);
//	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
//	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
//	glDisableClientState(GL_VERTEX_ARRAY);

	LBXGL_Shader_DrawArraysSimpleFlat(PDGL_QUADS, 0, chk->n_face*4,
		3, GL_FLOAT, 0, chk->face_xyz);
}

void LBXGL_Voxel_DrawChunkFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	LBXGL_Shader_DrawArraysNormalFlat(GL_QUADS, 0, chk->n_face*4,
		3, GL_FLOAT, 0, chk->face_xyz,
		3, GL_BYTE, 3, chk->face_norm);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
#ifndef BTGE_VOXEL_BYTERGBA
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
#else
	glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
#endif
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
#endif

}

void LBXGL_Voxel_DrawChunkFlatRGB(LBXGL_VoxelChunk *chk)
{
	if(!chk->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	LBXGL_Shader_DrawArraysNormalFlatRGB(GL_QUADS, 0, chk->n_face*4,
		3, GL_FLOAT, 0, chk->face_xyz,
		3, GL_BYTE, 3, chk->face_norm,
		4, GL_UNSIGNED_BYTE, 0, chk->face_rgba);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
#ifndef BTGE_VOXEL_BYTERGBA
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glColorPointer(4, GL_FLOAT, 0, chk->face_rgba);
#else
	glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, chk->face_rgba);
#endif
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
}

void LBXGL_Voxel_DrawChunkFlatVLRGB(LBXGL_VoxelChunk *chk)
{
	if(!chk->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	LBXGL_Shader_DrawArraysNormalFlatRGB(GL_QUADS, 0, chk->n_face*4,
		3, GL_FLOAT, 0, chk->face_xyz,
		3, GL_BYTE, 3, chk->face_norm,
		4, GL_UNSIGNED_BYTE, 0, chk->face_vlrgba);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
#ifndef BTGE_VOXEL_BYTERGBA
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glColorPointer(4, GL_FLOAT, 0, chk->face_vlrgba);
#else
	glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, chk->face_vlrgba);
#endif
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif

}

void LBXGL_Voxel_DrawChunkLight(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
#ifndef BTGE_VOXEL_BYTERGBA
	float *rgba;
#else
	byte *rgba;
#endif
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

#if 1
	if(chk->flags&VOX_CHFL_USESHADER)
//	if(true)
	{
//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		if(light->flags&LBXGL_LFL_SUN)
			rgba=chk->face_slrgba;
		else rgba=chk->face_rgba;
		useva=false;

		//LBXGL_Shader_Color4f(1,1,1,1);
		for(i=0; i<chk->n_face; i++)
		{
			fl=BTGE_Texture_GetImageFlags(chk->face_tex[i]);
			if(!(fl&BTGE_TXFL_LIGHTMAT))
			{
				for(j=i; j<chk->n_face; j++)
					if(chk->face_tex[j]!=chk->face_tex[i])
						break;
				i=j-1;
				useva=true;
				continue;
			}

			tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);
//			LBXGL_Shader_BindTexture(chk->face_tex[i]);

			LBXGL_Shader_BeginLight(GL_QUADS);
			for(j=i; j<chk->n_face; j++)
			{
				if(chk->face_tex[j]!=chk->face_tex[i])
					break;

				for(k=0; k<4; k++)
				{
#ifndef BTGE_VOXEL_BYTERGBA
					LBXGL_Shader_Normal3fv(chk->face_norm+(j*4+k)*3);
					LBXGL_Shader_Color4fv(rgba+(j*4+k)*4);
#else
					LBXGL_Shader_Normal3sbv(chk->face_norm+(j*4+k)*3);
					LBXGL_Shader_Color4ubv(rgba+(j*4+k)*4);
#endif

//					LBXGL_Shader_Normal3fv(chk->face_norm+(j*4+k)*3);
					LBXGL_Shader_TexCoord2fv(chk->face_st+(j*4+k)*2);
					LBXGL_Shader_Vertex3fv(chk->face_xyz+(j*4+k)*3);
				}
			}
			LBXGL_Shader_EndLight();
			i=j-1;
		}

#if 1
		if(useva)
		{
			LBXGL_Shader_BindTexture(-1);

			for(i=0; i<chk->n_face; i++)
			{
				fl=BTGE_Texture_GetImageFlags(chk->face_tex[i]);
				if(fl&BTGE_TXFL_LIGHTMAT)
				{
					for(j=i; j<chk->n_face; j++)
						if(chk->face_tex[j]!=chk->face_tex[i])
							break;
					i=j-1;
					continue;
				}

				for(j=i; j<chk->n_face; j++)
					if(chk->face_tex[j]!=chk->face_tex[i])
						break;

				k=j-i;

				LBXGL_Shader_DrawArraysNormalFlatRGB(
					GL_QUADS, i*4, k*4,
					3, GL_FLOAT, 0, chk->face_xyz,
					3, GL_BYTE, 3, chk->face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba);

				i+=k-1;
			}
		}
#endif

#if 0
		if(useva)
		{
			LBXGL_Shader_BindTexture(-1);

//			glEnable(GL_TEXTURE_2D);
//			glBindTexture(GL_TEXTURE_2D, chk->face_tex[0]);
//			tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[0]);
//			glBindTexture(GL_TEXTURE_2D, tn);

			glEnableClientState(GL_VERTEX_ARRAY);
//			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
//			glTexCoordPointer(2, GL_FLOAT, 0, chk->face_st);
#ifndef BTGE_VOXEL_BYTERGBA
			glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
			glColorPointer(4, GL_FLOAT, 0, rgba);
#else
			glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
			glColorPointer(4, GL_UNSIGNED_BYTE, 0, rgba);
#endif

			for(i=0; i<chk->n_face; i++)
			{
				fl=BTGE_Texture_GetImageFlags(chk->face_tex[i]);
				if(fl&BTGE_TXFL_LIGHTMAT)
				{
					for(j=i; j<chk->n_face; j++)
						if(chk->face_tex[j]!=chk->face_tex[i])
							break;
					i=j-1;
					continue;
				}

				for(j=i; j<chk->n_face; j++)
					if(chk->face_tex[j]!=chk->face_tex[i])
						break;

				k=j-i;

//				tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
//				tn=LBXGL_Voxel_MapTexture2(chk, chk->face_tex[i]);
//				glBindTexture(GL_TEXTURE_2D, tn);

//				glBindTexture(GL_TEXTURE_2D, chk->face_tex[i]);
				glDrawArrays(GL_QUADS, i*4, k*4);

				i+=k-1;
			}

			glDisableClientState(GL_VERTEX_ARRAY);
//			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
		}
#endif

		return;
	}
#endif

	if(light->flags&LBXGL_LFL_SUN)
	{
		LBXGL_Shader_DrawArraysNormalFlatRGB(GL_QUADS, 0, chk->n_face*4,
			3, GL_FLOAT, 0, chk->face_xyz,
			3, GL_BYTE, 3, chk->face_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->face_slrgba);

#if 0
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
#ifndef BTGE_VOXEL_BYTERGBA
		glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
		glColorPointer(4, GL_FLOAT, 0, chk->face_slrgba);
#else
		glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, chk->face_slrgba);
#endif
		glDrawArrays(GL_QUADS, 0, chk->n_face*4);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
#endif

		return;
	}

	LBXGL_Shader_DrawArraysNormalFlatRGB(GL_QUADS, 0, chk->n_face*4,
		3, GL_FLOAT, 0, chk->face_xyz,
		3, GL_BYTE, 3, chk->face_norm,
		4, GL_UNSIGNED_BYTE, 0, chk->face_rgba);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
#ifndef BTGE_VOXEL_BYTERGBA
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glColorPointer(4, GL_FLOAT, 0, chk->face_rgba);
#else
	glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, chk->face_rgba);
#endif
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
#endif
}

void LBXGL_Voxel_DrawChunkShadow(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
//	static float xyzarr[512*3];
	static float *xyzarr=NULL;
	static int nxyz=0;

	float txyza[4*3], txyzb[4*3];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3], lorg[3];
	float *xyz, *v0;

	float f, g, h;
	int i, j, k, l, n, i0, i1, nva;

	return;

	if(!chk->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 1024))
//		return;
	if(LBXGL_Voxel_GetChunkFrustumDistance(chk)>=1024)
		return;

	n=chk->n_face*6*4;
	if(!xyzarr || (n>nxyz))
	{
		xyzarr=realloc(xyzarr, n*3*sizeof(float));
		nxyz=n;
	}

	h=0.01*(256.0/1024);
	if(h>0.1)h=0.1;

	lorg[0]=light->org[0]+lbxgl_voxel_reforg[0];
	lorg[1]=light->org[1]+lbxgl_voxel_reforg[1];
	lorg[2]=light->org[2]+lbxgl_voxel_reforg[2];
	g=LBXGL_Shadow_LightGammaRadius(light);
//	g=99999;

	xyz=xyzarr;
	for(i=0; i<chk->n_face; i++)
	{
		for(j=0; j<4; j++)
		{
			v0=chk->face_xyz+(i*4+j)*3;
			V3F_COPY(v0, tv0);
//			V3F_SCALEADDSCALE(tv0, (1.0-h), brush->org, h, tv0);
//			V3F_SUB(tv0, light->org, dv0);
			V3F_SUB(tv0, lorg, dv0);
			V3F_FRNORMALIZE(dv0, dv0);
			V3F_ADD(tv0, lbxgl_cam->fw, tv0);
			V3F_ADDSCALE(tv0, dv0, 1, txyza+j*3);
			V3F_ADDSCALE(tv0, dv0, 2*g, txyzb+j*3);
		}

#ifndef BTGE_VOXEL_BYTERGBA
//		v0=chk->face_norm+(i*4)*3;
		V3F_COPY(chk->face_norm+(i*4)*3, tv0);
		f=V3F_DOT(light->org, tv0)-V3F_DOT(txyza, tv0);
//		f=-f;
#else
		tv0[0]=(chk->face_norm+(i*4)*3)[0]*(1.0/127.0);
		tv0[1]=(chk->face_norm+(i*4)*3)[1]*(1.0/127.0);
		tv0[2]=(chk->face_norm+(i*4)*3)[2]*(1.0/127.0);
		f=V3F_DOT(light->org, tv0)-V3F_DOT(txyza, tv0);
#endif

#if 0
		V3F_COPY(txyza+0*3, xyz+0*3);
		V3F_COPY(txyza+1*3, xyz+1*3);
		V3F_COPY(txyza+2*3, xyz+2*3);
		V3F_COPY(txyza+3*3, xyz+3*3);
		xyz+=4*3;

		V3F_COPY(txyzb+3*3, xyz+0*3);
		V3F_COPY(txyzb+2*3, xyz+1*3);
		V3F_COPY(txyzb+1*3, xyz+2*3);
		V3F_COPY(txyzb+0*3, xyz+3*3);
		xyz+=4*3;
#endif

		for(j=0; j<4; j++)
		{
			k=(j+1)%4;
			
			if(f>10)
			{
				V3F_COPY(txyza+k*3, xyz+0*3);
				V3F_COPY(txyza+j*3, xyz+1*3);
				V3F_COPY(txyzb+j*3, xyz+2*3);
				V3F_COPY(txyzb+k*3, xyz+3*3);
				xyz+=4*3;
			}else if(f<10)
			{
				V3F_COPY(txyza+k*3, xyz+3*3);
				V3F_COPY(txyza+j*3, xyz+2*3);
				V3F_COPY(txyzb+j*3, xyz+1*3);
				V3F_COPY(txyzb+k*3, xyz+0*3);
				xyz+=4*3;
			}
		}
	}

	i=(xyz-xyzarr)/3;
	if(i>n)
	{
		*(int *)-1=-1;
	}

	LBXGL_Shader_DrawArraysSimpleFlat(
		GL_QUADS, 0, i,
		3, GL_FLOAT, 0, xyzarr);

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, xyzarr);
	glDrawArrays(GL_QUADS, 0, i);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

#if 0
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
#endif
}

void LBXGL_Voxel_DrawChunkFinal(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

#if 1
	if(chk->flags&VOX_CHFL_USESHADER)
	{
//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);

		//LBXGL_Shader_Color4f(1,1,1,1);
		for(i=0; i<chk->n_face; i++)
		{
			tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);
//			LBXGL_Shader_BindTexture(chk->face_tex[i]);

			LBXGL_Shader_Begin(GL_QUADS);
			for(j=i; j<chk->n_face; j++)
			{
				if(chk->face_tex[j]!=chk->face_tex[i])
					break;

				for(k=0; k<4; k++)
				{
#ifndef BTGE_VOXEL_BYTERGBA
					LBXGL_Shader_Normal3fv(chk->face_norm+(j*4+k)*3);
#else
					LBXGL_Shader_Normal3sbv(chk->face_norm+(j*4+k)*3);
#endif

//					LBXGL_Shader_Normal3fv(chk->face_norm+(j*4+k)*3);
					LBXGL_Shader_TexCoord2fv(chk->face_st+(j*4+k)*2);
					LBXGL_Shader_Vertex3fv(chk->face_xyz+(j*4+k)*3);
				}
			}
			LBXGL_Shader_End();
			i=j-1;
		}
		return;
	}
#endif

#if 0
	for(i=0; i<chk->n_face; i++)
	{
		LBXGL_Shader_BindTexture(chk->face_tex[i]);
		LBXGL_Shader_BeginPoly();
		for(j=0; j<4; j++)
		{
			LBXGL_Shader_Normal3fv(chk->face_norm+(i*4+j)*3);
			LBXGL_Shader_TexCoord2fv(chk->face_st+(i*4+j)*2);
			LBXGL_Shader_Vertex3fv(chk->face_xyz+(i*4+j)*3);
		}
		LBXGL_Shader_EndPoly();
	}
#endif


#if 1
	for(i=0; i<chk->n_face; i++)
	{
		for(j=i; j<chk->n_face; j++)
			if(chk->face_tex[j]!=chk->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture2(chk, chk->face_tex[i]);
//		glBindTexture(GL_TEXTURE_2D, tn);
		LBXGL_Shader_BindTexture(tn);

		LBXGL_Shader_DrawArraysNormalTex(GL_QUADS, i*4, k*4,
			3, GL_FLOAT, 0, chk->face_xyz,
			2, GL_FLOAT, 0, chk->face_st,
			3, GL_BYTE, 3, chk->face_norm);

		i+=k-1;
	}
#endif


#if 0
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, chk->face_tex[0]);
//	tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[0]);
//	glBindTexture(GL_TEXTURE_2D, tn);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, chk->face_st);
#ifndef BTGE_VOXEL_BYTERGBA
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
#else
	glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
#endif

	for(i=0; i<chk->n_face; i++)
	{
		for(j=i; j<chk->n_face; j++)
			if(chk->face_tex[j]!=chk->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture2(chk, chk->face_tex[i]);
		glBindTexture(GL_TEXTURE_2D, tn);

//		glBindTexture(GL_TEXTURE_2D, chk->face_tex[i]);
		glDrawArrays(GL_QUADS, i*4, k*4);

		i+=k-1;
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
#endif

#if 0
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, chk->face_tex[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, chk->face_st);
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);

	glDrawArrays(GL_QUADS, 0, chk->n_face*4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
#endif
}

void LBXGL_Voxel_DrawChunkFinalRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 0
	for(i=0; i<chk->n_face; i++)
	{
		tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->face_tex[i]);

		LBXGL_Shader_Begin(GL_QUADS);
		for(j=i; j<chk->n_face; j++)
		{
			if(chk->face_tex[j]!=chk->face_tex[i])
				break;

			for(k=0; k<4; k++)
			{
				LBXGL_Shader_Color4fv(chk->face_rgba+(j*4+k)*4);
				LBXGL_Shader_Normal3fv(chk->face_norm+(j*4+k)*3);
				LBXGL_Shader_TexCoord2fv(chk->face_st+(j*4+k)*2);
				LBXGL_Shader_Vertex3fv(chk->face_xyz+(j*4+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif

#if 1
	for(i=0; i<chk->n_face; i++)
	{
		for(j=i; j<chk->n_face; j++)
			if(chk->face_tex[j]!=chk->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture2(chk, chk->face_tex[i]);
//		glBindTexture(GL_TEXTURE_2D, tn);
		LBXGL_Shader_BindTexture(tn);

		LBXGL_Shader_DrawArraysNormalTexRGB(GL_QUADS, i*4, k*4,
			3, GL_FLOAT, 0, chk->face_xyz,
			2, GL_FLOAT, 0, chk->face_st,
			3, GL_BYTE, 3, chk->face_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->face_rgba);

		i+=k-1;
	}
#endif

#if 0
	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, chk->face_tex[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, chk->face_st);
//	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
//	glColorPointer(4, GL_FLOAT, 0, chk->face_rgba);
#ifndef BTGE_VOXEL_BYTERGBA
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glColorPointer(4, GL_FLOAT, 0, chk->face_rgba);
#else
	glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->face_norm);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, chk->face_rgba);
#endif

	for(i=0; i<chk->n_face; i++)
	{
		for(j=i; j<chk->n_face; j++)
			if(chk->face_tex[j]!=chk->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture(chk, chk->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture2(chk, chk->face_tex[i]);
		glBindTexture(GL_TEXTURE_2D, tn);

//		glBindTexture(GL_TEXTURE_2D, chk->face_tex[i]);
		glDrawArrays(GL_QUADS, i*4, k*4);

		i+=k-1;
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
}

void LBXGL_Voxel_DrawChunkAlphaRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

//	if(!chk->n_face && !chk->n_aface)
	if(!chk->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 1
	for(i=0; i<chk->n_aface; i++)
	{
		if(BTGE_Voxel_VoxelFluidP(chk->aface_vox[i]))
			continue;

		tn=LBXGL_Voxel_MapTexture(chk, chk->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->face_tex[i]);

		LBXGL_Shader_Begin(GL_QUADS);
		for(j=i; j<chk->n_aface; j++)
		{
			if(chk->aface_tex[j]!=chk->aface_tex[i])
				break;

			for(k=0; k<4; k++)
			{
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->aface_rgba+(j*4+k)*4);
				LBXGL_Shader_Normal3fv(chk->aface_norm+(j*4+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->aface_rgba+(j*4+k)*4);
				LBXGL_Shader_Normal3sbv(chk->aface_norm+(j*4+k)*3);
#endif
				LBXGL_Shader_TexCoord2fv(chk->aface_st+(j*4+k)*2);
				LBXGL_Shader_Vertex3fv(chk->aface_xyz+(j*4+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawChunkSimpleAlphaFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->n_aface)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->aface_xyz);
	glDrawArrays(GL_QUADS, 0, chk->n_aface*4);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void LBXGL_Voxel_DrawChunkAlphaFlat(LBXGL_VoxelChunk *chk)
{
//	if(!chk->n_face && !chk->n_aface)
	if(!chk->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->aface_xyz);
//	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->aface_norm);
#ifndef BTGE_VOXEL_BYTERGBA
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->aface_norm);
#else
	glNormalPointer(GL_BYTE, 3*sizeof(sbyte), chk->aface_norm);
#endif
	glDrawArrays(GL_QUADS, 0, chk->n_aface*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void LBXGL_Voxel_DrawChunkAlphaFluidFlat(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->n_aface)return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!LBXGL_Voxel_CheckChunkFrustum(chk))return;

#if 1
	for(i=0; i<chk->n_aface; i++)
	{
		if(!BTGE_Voxel_VoxelFluidP(chk->aface_vox[i]))
			continue;
	
		glBegin(GL_QUADS);
		for(j=i; j<chk->n_aface; j++)
		{
			if(chk->aface_tex[j]!=chk->aface_tex[i])
				break;

			for(k=0; k<4; k++)
			{
//				glColor4fv(chk->aface_rgba+(j*4+k)*4);
//				glNormal3fv(chk->aface_norm+(j*4+k)*3);

#ifndef BTGE_VOXEL_BYTERGBA
//				glColor4fv(chk->aface_rgba+(j*4+k)*4);
				glNormal3fv(chk->aface_norm+(j*4+k)*3);
#else
//				glColor4ubv(chk->aface_rgba+(j*4+k)*4);
				glNormal3bv(chk->aface_norm+(j*4+k)*3);
#endif

//				glTexCoord2fv(chk->aface_st+(j*4+k)*2);
				glVertex3fv(chk->aface_xyz+(j*4+k)*3);
			}
		}
		glEnd();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawChunkAlphaFluid(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 1
	for(i=0; i<chk->n_aface; i++)
	{
		if(!BTGE_Voxel_VoxelFluidP(chk->aface_vox[i]))
			continue;
	
		tn=LBXGL_Voxel_MapTexture(chk, chk->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->face_tex[i]);

		glDisable(GL_CULL_FACE);

//		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
//		LBXGL_Shader_BindTexture(tn);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		LBXGL_Shader_Begin(GL_QUADS);
		for(j=i; j<chk->n_aface; j++)
		{
			if(chk->aface_tex[j]!=chk->aface_tex[i])
				break;

			for(k=0; k<4; k++)
			{
//				LBXGL_Shader_Color4fv(chk->aface_rgba+(j*4+k)*4);
//				LBXGL_Shader_Normal3fv(chk->aface_norm+(j*4+k)*3);
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->aface_rgba+(j*4+k)*4);
				LBXGL_Shader_Normal3fv(chk->aface_norm+(j*4+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->aface_rgba+(j*4+k)*4);
				LBXGL_Shader_Normal3sbv(chk->aface_norm+(j*4+k)*3);
#endif

				LBXGL_Shader_TexCoord2fv(chk->aface_st+(j*4+k)*2);
				LBXGL_Shader_Vertex3fv(chk->aface_xyz+(j*4+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawChunkAlphaFluid2(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 1
	for(i=0; i<chk->n_aface; i++)
	{
//		tn=LBXGL_Voxel_MapTexture(chk, chk->aface_tex[i]);
//		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->face_tex[i]);

		if(!BTGE_Voxel_VoxelFluidOverlayP(chk->aface_vox[i]))
			continue;

		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
		LBXGL_Shader_BindTexture(tn);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		glDisable(GL_CULL_FACE);

		LBXGL_Shader_Begin(GL_QUADS);
		for(j=i; j<chk->n_aface; j++)
		{
			if(chk->aface_tex[j]!=chk->aface_tex[i])
				break;

			for(k=0; k<4; k++)
			{
//				LBXGL_Shader_Color4fv(chk->aface_rgba+(j*4+k)*4);
//				LBXGL_Shader_Normal3fv(chk->aface_norm+(j*4+k)*3);
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->aface_rgba+(j*4+k)*4);
				LBXGL_Shader_Normal3fv(chk->aface_norm+(j*4+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->aface_rgba+(j*4+k)*4);
				LBXGL_Shader_Normal3sbv(chk->aface_norm+(j*4+k)*3);
#endif
				LBXGL_Shader_TexCoord2fv(chk->aface_st+(j*4+k)*2);
				LBXGL_Shader_Vertex3fv(chk->aface_xyz+(j*4+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawRegionQuery(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;
	int i;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		pdglBeginQuery(GL_SAMPLES_PASSED, cur->visid);
		LBXGL_Voxel_DrawChunkSimpleFlat(cur);
		pdglEndQuery(GL_SAMPLES_PASSED);
		cur=cur->vnext;
	}

	cur=rgn->vischk;
	while(cur)
	{
		pdglGetQueryObjectuiv(cur->visid, GL_QUERY_RESULT, &i);
		//if(i>0)
		if(i!=0)
			{ cur->flags&=~VOX_CHFL_VISCLIP; }
		else
			{ cur->flags|=VOX_CHFL_VISCLIP; }
		cur=cur->vnext;
	}

	glDepthMask(GL_FALSE);

	cur=rgn->vischk;
	while(cur)
	{
		if(cur->n_aface)
		{
			pdglBeginQuery(GL_SAMPLES_PASSED, cur->visid);
			LBXGL_Voxel_DrawChunkSimpleAlphaFlat(cur);
			pdglEndQuery(GL_SAMPLES_PASSED);
		}
		cur=cur->vnext;
	}

	cur=rgn->vischk;
	while(cur)
	{
		if(cur->n_aface)
		{
			pdglGetQueryObjectuiv(cur->visid, GL_QUERY_RESULT, &i);
			//if(i>0)
			if(i!=0)
				{ cur->flags&=~VOX_CHFL_ALPHAVISCLIP; }
			else
				{ cur->flags|=VOX_CHFL_ALPHAVISCLIP; }
		}
		cur=cur->vnext;
	}

	glDepthMask(GL_TRUE);
}

void LBXGL_Voxel_DrawRegionLight(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkLight(cur, light);
		cur=cur->vnext;
	}
}

void LBXGL_Voxel_DrawRegionShadow(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkShadow(cur, light);
		cur=cur->vnext;
	}
}

void LBXGL_Voxel_DrawRegionFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlat(cur);
		cur=cur->vnext;
	}
}

void LBXGL_Voxel_DrawRegionFlatRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatRGB(cur);
		cur=cur->vnext;
	}
}

void LBXGL_Voxel_DrawRegionFlatVLRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatVLRGB(cur);
		cur=cur->vnext;
	}
}

void LBXGL_Voxel_DrawRegionFinal(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinal(cur);
		cur=cur->vnext;
	}
}

void LBXGL_Voxel_DrawRegionFinalRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinalRGB(cur);
		cur=cur->vnext;
	}
}

void LBXGL_Voxel_DrawRegionAlphaRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaRGB(cur);
		cur=cur->avnext;
	}
}

void LBXGL_Voxel_DrawRegionAlphaFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFlat(cur);
		cur=cur->avnext;
	}
}

void LBXGL_Voxel_DrawRegionAlphaFluidFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluidFlat(cur);
		cur=cur->avnext;
	}
}

void LBXGL_Voxel_DrawRegionAlphaFluid(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluid(cur);
		cur=cur->avnext;
	}
}

void LBXGL_Voxel_DrawRegionAlphaFluid2(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluid2(cur);
		cur=cur->avnext;
	}
}

void LBXGL_Voxel_DrawRegionListQuery(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionQuery(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFlat(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFlat(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListLight(
	LBXGL_VoxelRegion *lst, LBXGL_Light *light)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionLight(cur, light);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListShadow(
	LBXGL_VoxelRegion *lst, LBXGL_Light *light)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionShadow(cur, light);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFlatRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFlatRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFlatVLRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFlatVLRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFinal(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFinal(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListFinalRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionFinalRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaRGB(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaRGB(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFlat(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFlat(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFluidFlat(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFluidFlat(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFluid(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFluid(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawRegionListAlphaFluid2(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionAlphaFluid2(cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_DrawWorldChunksAlphaRGB(LBXGL_BrushWorld *world)
{
	LBXGL_VoxelChunk *cur;

//	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
//		return;

	cur=world->vox_avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaRGB(cur);
		cur=cur->wavnext;
	}
}

void LBXGL_Voxel_GenerateChunks(LBXGL_BrushWorld *world)
{
	BTGE_BrushWorld *tmp;

	//for now...
	if(world->vox_region)
		return;

	tmp=BTGE_Brush_NewWorld();
	BTGE_Voxel_GenerateChunks(tmp);
	world->vox_region=tmp->vox_region;
	return;
}

float LBXGL_Voxel_CalcChunkCameraDistance(LBXGL_VoxelChunk *chk)
{
	float mins[3], maxs[3], org[3];
	float tv[3];
	float f, g;
	
//	LBXGL_Voxel_BoxChunk(chk, mins, maxs);
	LBXGL_Voxel_BoxChunkRel(chk, mins, maxs);
	V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);

	tv[0]=org[0]-lbxgl_cam->org[0];
	tv[1]=org[1]-lbxgl_cam->org[1];
	tv[2]=org[2]-lbxgl_cam->org[2];

//	tv[0]=chk->ox*32-lbxgl_cam->org[0];
//	tv[1]=chk->oy*32-lbxgl_cam->org[1];
//	tv[2]=chk->oz*32-lbxgl_cam->org[2];
	f=V3F_LEN(tv);
	return(f);
}

void LBXGL_Voxel_RebuildRegionVisible(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *chk, *ccur, *cprv;
	int i, j, k, l, n, m, cn;
	float f, g;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	BTGE_VoxelSave_FlattenRegion(world, rgn);

	rgn->avischk=NULL;
	rgn->vischk=NULL; n=0; m=0;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
			for(k=0; k<rgn->zs; k++)
	{
		cn=(i*rgn->ys+j)*rgn->zs+k;
		chk=rgn->chk[cn];
		if(!chk)
		{
			//is it within the frustum
//			if(!LBXGL_Voxel_CheckRegionChunkFrustumDistance(
//				rgn, i, j, k, 6144))
//					{ continue; }

			if(!rgn->chk_offs[cn])
				continue;

			l=PDGL_TimeMS();
			if(l<lbxgl_voxel_chkstart)continue;
			if(l>(lbxgl_voxel_chkstart+48))continue;

			if(!LBXGL_Voxel_CheckRegionChunkFrustumDistance(
				rgn, i, j, k, 7168))
					{ continue; }
					
//			BTGE_Voxel_GetRegionVoxel(rgn,
//				i*rgn->xst, j*rgn->yst, k*rgn->zst);

			//see if we can unpack it
			BTGE_VoxelSave_LoadRegionChunk(world, rgn, cn);
			chk=rgn->chk[cn];
			if(!chk)continue;
		}

//		if(LBXGL_Voxel_GetChunkFrustumDistance(chk)>1600)
//			{ chk->tickactive=0; }
//		else
//			{ chk->tickactive=1; }

		if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		{
			f=LBXGL_Voxel_GetChunkFrustumDistance(chk);
//			if(f>16384)
			if(f>12288)
			{
				BTGE_Voxel_FreeChunk(world, chk);
				rgn->chk[cn]=NULL;
				continue;
			}
			if(f>8192)
//			if(f>6144)
			{
//				BTGE_Voxel_FlushReleaseChunk(chk);
				BTGE_Voxel_FreeChunk(world, chk);
				rgn->chk[cn]=NULL;
				continue;
			}
			continue;
		}

//		LBXGL_Voxel_CheckRebuildChunk(chk);
		if(!chk->clean)
		{
			if(lbxgl_voxel_chkleft<=0)
				continue;
			lbxgl_voxel_chkleft--;

			l=PDGL_TimeMS();
			if(l<lbxgl_voxel_chkstart)continue;
			if(l>(lbxgl_voxel_chkstart+48))continue;

			BTGE_Voxel_CheckRebuildChunk(world, chk);
		}

		m++;
		if(!chk->n_face && !chk->n_aface)
			continue;

		if(chk->n_aface)
		{
			world->flags|=BTGE_WFL_VOXFLUID;
		}

#if 1
		f=LBXGL_Voxel_CalcChunkCameraDistance(chk);
		
		ccur=rgn->vischk; cprv=NULL;
		while(ccur)
		{
			g=LBXGL_Voxel_CalcChunkCameraDistance(ccur);
//			if(f>=g)break;
			if(f<=g)break;
			cprv=ccur; ccur=ccur->vnext;
		}

		if(cprv)
		{
			chk->vnext=cprv->vnext;
			cprv->vnext=chk;
		}else
		{
			chk->vnext=rgn->vischk;
			rgn->vischk=chk;
		}
#else
			chk->vnext=rgn->vischk;
			rgn->vischk=chk;
#endif

#if 0
#if 1
		f=LBXGL_Voxel_CalcChunkCameraDistance(chk);
		
		ccur=rgn->avischk; cprv=NULL;
		while(ccur)
		{
			g=LBXGL_Voxel_CalcChunkCameraDistance(ccur);
			if(f<=g)break;
			cprv=ccur; ccur=ccur->avnext;
		}

		if(cprv)
		{
			chk->avnext=cprv->avnext;
			cprv->avnext=chk;
		}else
		{
			chk->avnext=rgn->avischk;
			rgn->avischk=chk;
		}
#else
			chk->avnext=rgn->avischk;
			rgn->avischk=chk;
#endif
#endif

		chk->visid=n+1;
		n++;
	}

	rgn->avischk=NULL;
	ccur=rgn->vischk;
	while(ccur)
	{
		if(!ccur->n_aface)
			{ ccur=ccur->vnext; continue; }
	
		ccur->avnext=rgn->avischk;
		rgn->avischk=ccur;

		ccur=ccur->vnext;
	}

//	printf("Visible Chunks: %d/%d\n", n, m);
}

void LBXGL_Voxel_RebuildWorldVisibleChunks(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rlst)
{
	LBXGL_VoxelRegion *rcur;
	LBXGL_VoxelChunk *cur, *ccur, *cprv;
	int i, j, k, l, n, m, cn;
	float f, g;

	world->vox_avischk=NULL;
	world->vox_vischk=NULL;

	rcur=rlst;
	while(rcur)
	{
		if(!LBXGL_Voxel_CheckRegionFrustum(rcur))
			{ rcur=rcur->next; continue; }

		cur=rcur->vischk;
		while(cur)
		{
			if((cur->flags&VOX_CHFL_VISCLIP) &&
				(cur->flags&VOX_CHFL_ALPHAVISCLIP))
					{ cur=cur->vnext; continue; }

#if 1
			f=LBXGL_Voxel_CalcChunkCameraDistance(cur);
		
			ccur=world->vox_vischk; cprv=NULL;
			while(ccur)
			{
				g=LBXGL_Voxel_CalcChunkCameraDistance(ccur);
//				if(f>=g)break;
				if(f<=g)break;
				cprv=ccur; ccur=ccur->wvnext;
			}

			if(cprv)
			{
				cur->wvnext=cprv->wvnext;
				cprv->wvnext=cur;
			}else
			{
				cur->wvnext=world->vox_vischk;
				world->vox_vischk=cur;
			}
#else
			cur->wvnext=world->vox_vischk;
			world->vox_vischk=chk;
#endif

			cur=cur->vnext;
		}

		rcur=rcur->next;
	}

	world->vox_avischk=NULL;
	ccur=world->vox_vischk;
	while(ccur)
	{
		if(!ccur->n_aface)
			{ ccur=ccur->wvnext; continue; }

		if(ccur->flags&VOX_CHFL_ALPHAVISCLIP)
			{ ccur=ccur->wvnext; continue; }
	
		ccur->wavnext=world->vox_avischk;
		world->vox_avischk=ccur;

		ccur=ccur->wvnext;
	}
}

void LBXGL_Voxel_RebuildRegionListVisible(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_RebuildRegionVisible(world, cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_BeginTranslateForWorld(LBXGL_BrushWorld *world)
{
	float tv[3];

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	lbxgl_voxel_reforg[0]=world->cam_reforg[0];
	lbxgl_voxel_reforg[1]=world->cam_reforg[1];
	lbxgl_voxel_reforg[2]=world->cam_reforg[2];

	tv[0]=-world->cam_reforg[0];
	tv[1]=-world->cam_reforg[1];
	tv[2]=-world->cam_reforg[2];
	glTranslatef(tv[0], tv[1], tv[2]);
}

void LBXGL_Voxel_EndTranslateForWorld(LBXGL_BrushWorld *world)
{
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void LBXGL_Voxel_DrawWorldQuery(LBXGL_BrushWorld *world)
{
	lbxgl_voxel_chkleft=16;
	lbxgl_voxel_chkstart=PDGL_TimeMS();

//	glColor4f(0, 0, 0, 1);
	glColor4f(1, 1, 1, 1);

	world->flags&=~BTGE_WFL_VOXFLUID;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_GenerateChunks(world);
//	BTGE_Voxel_GenerateChunks(world);
	LBXGL_Voxel_RebuildRegionListVisible(world, world->vox_region);
//	BTGE_Voxel_RebuildRegionListVisible(world->vox_region);
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);

//	LBXGL_Voxel_DrawRegionListFlatVLRGB(world->vox_region);
	LBXGL_Voxel_DrawRegionListQuery(world->vox_region);

	LBXGL_Voxel_RebuildWorldVisibleChunks(world, world->vox_region);
	
	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldDark(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	lbxgl_voxel_chkleft=16;
	lbxgl_voxel_chkstart=PDGL_TimeMS();

//	glColor4f(0, 0, 0, 1);
	glColor4f(1, 1, 1, 1);

	world->flags&=~BTGE_WFL_VOXFLUID;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_GenerateChunks(world);
//	BTGE_Voxel_GenerateChunks(world);

//	LBXGL_Voxel_RebuildRegionListVisible(world, world->vox_region);

//	BTGE_Voxel_RebuildRegionListVisible(world->vox_region);
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);

	LBXGL_Voxel_DrawRegionListFlatVLRGB(world->vox_region);
	
	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListDark(world, mdls);
	
	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
	BTGE_ModelState *mdls;

	LBXGL_Shader_BindTexture(0);

	LBXGL_Voxel_BeginTranslateForWorld(world);

	glColor4f(1, 1, 1, 1);
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
	LBXGL_Voxel_DrawRegionListLight(world->vox_region, light);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListLight(world, mdls, light);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldShadows(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
	BTGE_ModelState *mdls;

	//for now...
	if(light->flags&LBXGL_LFL_SUN)
		return;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
	LBXGL_Voxel_DrawRegionListShadow(world->vox_region, light);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListShadow(world, mdls, light);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldPointLight(LBXGL_BrushWorld *world)
{
//	LBXGL_Voxel_DrawRegionListFlatRGB(world->vox_region);
}

void LBXGL_Voxel_DrawWorldFinal(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

	glColor4f(1, 1, 1, 1);
	LBXGL_Voxel_DrawRegionListFinal(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListFinal(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldAlpha(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
//	LBXGL_Voxel_DrawRegionListFinalRGB(world->vox_region);
//	LBXGL_Voxel_DrawRegionListAlphaRGB(world->vox_region);
	LBXGL_Voxel_DrawWorldChunksAlphaRGB(world);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListAlpha(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldFluidFlat(LBXGL_BrushWorld *world)
{
	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListAlphaFlat(world->vox_region);
	LBXGL_Voxel_DrawRegionListAlphaFluidFlat(world->vox_region);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldFluid(LBXGL_BrushWorld *world)
{
	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListAlphaRGB(world->vox_region);
	LBXGL_Voxel_DrawRegionListAlphaFluid(world->vox_region);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldFluid2(LBXGL_BrushWorld *world)
{
	LBXGL_Voxel_BeginTranslateForWorld(world);
	LBXGL_Voxel_DrawRegionListAlphaFluid2(world->vox_region);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_ClearWorldPointLight(LBXGL_BrushWorld *world)
{
}

void LBXGL_Voxel_UpdateWorldPointLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
}


int LBXGL_Voxel_CheckFluidCam(LBXGL_BrushWorld *world)
{
	BTGE_VoxelData *vox;
	int x, y, z;
	
	x=floor(lbxgl_cam->org[0]/32);
	y=floor(lbxgl_cam->org[1]/32);
	z=floor(lbxgl_cam->org[2]/32);

	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);
	if(BTGE_Voxel_VoxelFluidP(vox))
		return(1);
	return(0);
}

float LBXGL_Voxel_GetCameraSunGamma(LBXGL_BrushWorld *world)
{
	BTGE_VoxelData *vox;
	int x, y, z;
	float f;
	int i;

	x=floor(lbxgl_cam->org[0]/32);
	y=floor(lbxgl_cam->org[1]/32);
	z=floor(lbxgl_cam->org[2]/32);

	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);

	if(!vox)return(1.0);

//	i=(vox->aux>>4)&15;
	i=(vox->slight>>4)&15;
//	f=pow(0.80, 15-i);
	f=pow(0.60, 15-i);
	return(f);

//	if(BTGE_Voxel_VoxelFluidP(vox))
//		return(1);
//	return(1.0);
}

float LBXGL_Voxel_GetPointSunGamma(LBXGL_BrushWorld *world, float *org)
{
	BTGE_VoxelData *vox;
	int x, y, z;
	float f;
	int i;

	x=floor(org[0]/32);
	y=floor(org[1]/32);
	z=floor(org[2]/32);

	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);

	if(!vox)return(1.0);

//	i=(vox->aux>>4)&15;
	i=(vox->slight>>4)&15;
	f=pow(0.80, 15-i);
//	f=pow(0.60, 15-i);
	return(f);

//	if(BTGE_Voxel_VoxelFluidP(vox))
//		return(1);
//	return(1.0);
}

LBXGL_API void LBXGL_Voxel_DrawFluidFogVolume(
	LBXGL_BrushWorld *world, float *rgba)
{
	float s;

	if(!world->vox_region)
		return;
	if(!(world->flags&BTGE_WFL_VOXFLUID))
		return;

	glDepthMask(1);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
//	glCullFace(GL_BACK);

	LBXGL_Voxel_DrawWorldFluidFlat(world);

	glCullFace(GL_BACK);

	glDepthMask(0);
	glColorMask(1, 1, 1, 1);
	
	s=0.0625/rgba[3];
	LBXGL_Brush_DrawLayerFog(rgba, 2, s, 128);

	glDepthMask(1);
}

LBXGL_API void LBXGL_Voxel_DrawFluidSurfaceFogVolume(
	LBXGL_BrushWorld *world)
{
	float pt[4];
	float s;

	if(!world->vox_region)
		return;
	if(!(world->flags&BTGE_WFL_VOXFLUID))
		return;

	glDisable(GL_TEXTURE_2D);
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 0, 255);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

	glDepthMask(0);
	glColorMask(0, 0, 0, 0);

	glEnable(GL_CULL_FACE);
//	glCullFace(GL_FRONT);
	glCullFace(GL_BACK);

	LBXGL_Voxel_DrawWorldFluidFlat(world);

	glCullFace(GL_BACK);

	glDepthMask(0);
	glColorMask(1, 1, 1, 1);

	glStencilFunc(GL_NOTEQUAL, 0, 255);

	pt[0]=0.04; pt[1]=0.18; pt[2]=0.20;
//	pt[3]=1.0/384;
//	pt[3]=1.0/512;
	pt[3]=1.0/768;

	s=0.0625/pt[3];
	LBXGL_Brush_DrawLayerFog(pt, 2, s, 32);

	glDepthMask(1);

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_STENCIL_TEST);
}

LBXGL_API LBXGL_Light *LBXGL_Voxel_QueryLightsCamRegion(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn, LBXGL_Light *lst)
{
	LBXGL_VoxelChunk *cur;
	LBXGL_Light *lcur;
	float f;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return(lst);

	cur=rgn->vischk;
	while(cur)
	{
//		LBXGL_Voxel_DrawChunkFlatRGB(cur);

		if(cur->flags&VOX_CHFL_VISCLIP)
			{ cur=cur->vnext; continue; }

		f=LBXGL_Voxel_GetChunkFrustumDistance(cur);
//		if(f>1024)
//		if(f>768)
		if(f>512)
			{ cur=cur->vnext; continue; }

		lcur=cur->light;
		while(lcur)
		{
			lcur->org[0]=(cur->ox*32)+lcur->rel_org[0];
			lcur->org[1]=(cur->oy*32)+lcur->rel_org[1];
			lcur->org[2]=(cur->oz*32)+lcur->rel_org[2];

			lcur->chain=lst;
			lst=lcur;
			
			lcur=lcur->next;
		}

		cur=cur->vnext;
	}
	
	return(lst);
}

LBXGL_API LBXGL_Light *LBXGL_Voxel_QueryLightsCamWorld(
	LBXGL_BrushWorld *world)
{
	LBXGL_VoxelRegion *cur;
	LBXGL_Light *lst;

	cur=world->vox_region; lst=NULL;
	while(cur)
	{
		lst=LBXGL_Voxel_QueryLightsCamRegion(world, cur, lst);
		cur=cur->next;
	}
	return(lst);
}


LBXGL_API LBXGL_ModelState *LBXGL_Voxel_QueryModelsVisibleRegion(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn, LBXGL_ModelState *lst)
{
	LBXGL_VoxelChunk *cur;
	LBXGL_ModelState *mcur;
	float f;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return(lst);

	cur=rgn->vischk;
	while(cur)
	{
//		LBXGL_Voxel_DrawChunkFlatRGB(cur);

		if(cur->flags&VOX_CHFL_VISCLIP)
			{ cur=cur->vnext; continue; }

		f=LBXGL_Voxel_GetChunkFrustumDistance(cur);
//		if(f>1024)
		if(f>768)
//		if(f>512)
			{ cur=cur->vnext; continue; }

		mcur=cur->props;
		while(mcur)
		{
			mcur->org[0]=(cur->ox*32)+mcur->rel_org[0];
			mcur->org[1]=(cur->oy*32)+mcur->rel_org[1];
			mcur->org[2]=(cur->oz*32)+mcur->rel_org[2];

			mcur->chain=lst;
			lst=mcur;
			
			mcur=mcur->next;
		}

		cur=cur->vnext;
	}
	
	return(lst);
}

LBXGL_API LBXGL_ModelState *LBXGL_Voxel_QueryModelsVisibleWorld(
	LBXGL_BrushWorld *world)
{
	LBXGL_VoxelRegion *cur;
	LBXGL_ModelState *lst;

	cur=world->vox_region; lst=NULL;
	while(cur)
	{
		lst=LBXGL_Voxel_QueryModelsVisibleRegion(world, cur, lst);
		cur=cur->next;
	}
	return(lst);
}
