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
	
	LBXGL_Voxel_BoxChunkRel(chk, mins, maxs);

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
	
	LBXGL_Voxel_BoxChunkRel(chk, mins, maxs);
	return(LBXGL_Brush_GetBBoxFrustumDistance(mins, maxs));
}

bool LBXGL_Voxel_CheckRegionFrustum(LBXGL_VoxelRegion *rgn)
{
	float mins[3], maxs[3], org[3];
	float f, g;
	
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

bool LBXGL_Voxel_CheckRegionDistance(
	LBXGL_VoxelRegion *rgn, float dist)
{
	float mins[3], maxs[3];
	float vsz, vsz2;
	float f, g;
	int i, j, k, l;

	vsz=rgn->voxsz;
	vsz2=vsz*0.5;

	mins[0]=(rgn->ox*vsz)-lbxgl_voxel_reforg[0];
	mins[1]=(rgn->oy*vsz)-lbxgl_voxel_reforg[1];
	mins[2]=(rgn->oz*vsz)-lbxgl_voxel_reforg[2];
	maxs[0]=mins[0]+rgn->xst*vsz;
	maxs[1]=mins[1]+rgn->yst*vsz;
	maxs[2]=mins[2]+rgn->zst*vsz;

	f=LBXGL_Brush_GetBBoxFrustumDistance(mins, maxs);
	if(f>dist)return(false);
	
	return(true);
}

void LBXGL_Voxel_DrawChunkSimpleFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_face)
		return;

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysSimpleFlat(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	LBXGL_Shader_DrawArraysSimpleFlat(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, chk->mesh->face_xyz);
#endif
}

void LBXGL_Voxel_DrawChunkFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysNormalFlat(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
		3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	LBXGL_Shader_DrawArraysNormalFlat(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, chk->mesh->face_xyz,
		3, GL_BYTE, 3, chk->mesh->face_norm);
#endif
}

void LBXGL_Voxel_DrawChunkFlatRGB(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysNormalFlatRGB(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
		3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
		4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	LBXGL_Shader_DrawArraysNormalFlatRGB(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, chk->mesh->face_xyz,
		3, GL_BYTE, 3, chk->mesh->face_norm,
		4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_rgba);
#endif
}

void LBXGL_Voxel_DrawChunkFlatVLRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

#if 0
//	if(chk->flags&VOX_CHFL_USESHADER)
	if((chk->flags&VOX_CHFL_USESHADER) && !(chk->flags&VOX_CHFL_DISTANT))
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

//		LBXGL_Shader_BindTexture(-1);
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);

		for(i=0; i<chk->mesh->n_face; i++)
		{
			for(j=i; j<chk->mesh->n_face; j++)
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);

			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
				4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_vlrgba);

			i+=k-1;
		}

		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		return;
	}
#endif

	LBXGL_Shader_BindTexture(-1);

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysNormalTexRGB(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
		2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
		3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
		4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_vlrgba);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysNormalFlatRGB(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
		3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
		4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_vlrgba);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	LBXGL_Shader_DrawArraysNormalFlatRGB(PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, chk->mesh->face_xyz,
		3, GL_BYTE, 3, chk->mesh->face_norm,
		4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_vlrgba);
#endif
}

void LBXGL_Voxel_DrawChunkFlatVLRGB_AlphaLightMat(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

	if(!chk->mesh->n_aface)
		return;

	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!(chk->flags&VOX_CHFL_ALPHALIGHTMAT))
		return;
	if(chk->flags&VOX_CHFL_DISTANT)
		return;

	LBXGL_Shader_NormalX3f(0, 0, 0);
	LBXGL_Shader_NormalY3f(0, 0, 0);

//	LBXGL_Shader_BindTexture(-1);
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);

	for(i=0; i<chk->mesh->n_aface; i++)
	{
		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if(!(fl&BTGE_TXFL_LIGHTMAT)) { i=j-1; continue; }

		k=j-i;

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_vlrgba);

		i+=k-1;
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LBXGL_Voxel_DrawChunkLight_AlphaLightMat(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!(chk->flags&VOX_CHFL_ALPHALIGHTMAT))
		return;
	if(chk->flags&VOX_CHFL_DISTANT)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	LBXGL_Shader_NormalX3f(0, 0, 0);
//	LBXGL_Shader_NormalY3f(0, 0, 0);

	LBXGL_Shader_NormalX3f(0, 0, 0);
	LBXGL_Shader_NormalY3f(0, 0, 0);

	if(light->flags&LBXGL_LFL_SUN)
		rgba=chk->mesh->aface_slrgba;
	else rgba=chk->mesh->aface_rgba;

	if(light->flags&LBXGL_LFL_SUN)
		rgba2=(byte *)chk->mesh->vbo_aface_slrgba;
	else rgba2=(byte *)chk->mesh->vbo_aface_rgba;

//	LBXGL_Shader_BindTexture(-1);

	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);

	for(i=0; i<chk->mesh->n_aface; i++)
	{
		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if(!(fl&BTGE_TXFL_LIGHTMAT)) { i=j-1; continue; }

		k=j-i;

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
			4, GL_UNSIGNED_BYTE, 0, rgba2);
#endif

		i+=k-1;
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LBXGL_Voxel_DrawChunkLight(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
#ifndef BTGE_VOXEL_BYTERGBA
	float *rgba;
#else
	byte *rgba;
	byte *rgba2;
#endif
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//	LBXGL_Shader_Color4f(1, 1, 1, 1);
//	LBXGL_Shader_NormalX3f(0, 0, 0);
//	LBXGL_Shader_NormalY3f(0, 0, 0);

#if 1
	if((chk->flags&VOX_CHFL_USESHADER) && !(chk->flags&VOX_CHFL_DISTANT))
//	if(1)
//	if(0)
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		if(light->flags&LBXGL_LFL_SUN)
//		if(1)
			rgba=chk->mesh->face_slrgba;
		else rgba=chk->mesh->face_rgba;
//		useva=false;

		if(light->flags&LBXGL_LFL_SUN)
			rgba2=(byte *)chk->mesh->vbo_face_slrgba;
		else rgba2=(byte *)chk->mesh->vbo_face_rgba;

//		LBXGL_Shader_BindTexture(-1);

		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);

		for(i=0; i<chk->mesh->n_face; i++)
		{
			for(j=i; j<chk->mesh->n_face; j++)
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);

#if 1
//			pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
				4, GL_UNSIGNED_BYTE, 0, rgba2);
#endif

#if 0
			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, chk->mesh->face_xyz,
				2, GL_FLOAT, 0, chk->mesh->face_st,
				3, GL_BYTE, 3, chk->mesh->face_norm,
				4, GL_UNSIGNED_BYTE, 0, rgba);
#endif

			i+=k-1;
		}

		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		
//		if(chk->flags&VOX_CHFL_ALPHALIGHTMAT)
//			LBXGL_Voxel_DrawChunkLight_AlphaLightMat(chk, light);
		
		return;
	}
#endif


#if 0
	if(chk->flags&VOX_CHFL_USESHADER)
//	if(true)
	{
//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		if(light->flags&LBXGL_LFL_SUN)
//		if(1)
			rgba=chk->mesh->face_slrgba;
		else rgba=chk->mesh->face_rgba;
		useva=false;

		//LBXGL_Shader_Color4f(1,1,1,1);
		for(i=0; i<chk->mesh->n_face; i++)
		{
			fl=BTGE_Texture_GetImageFlags(chk->mesh->face_tex[i]);
			if(!(fl&BTGE_TXFL_LIGHTMAT))
			{
				for(j=i; j<chk->mesh->n_face; j++)
					if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
						break;
				i=j-1;
				useva=true;
				continue;
			}

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);
//			LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

			LBXGL_Shader_BeginLight(PDGL_TRIANGLES);
			for(j=i; j<chk->mesh->n_face; j++)
			{
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

				for(k=0; k<3; k++)
				{
#ifndef BTGE_VOXEL_BYTERGBA
					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*3+k)*3);
					LBXGL_Shader_Color4fv(rgba+(j*3+k)*4);
#else
					LBXGL_Shader_Normal3sbv(chk->mesh->face_norm+(j*3+k)*3);
					LBXGL_Shader_Color4ubv(rgba+(j*3+k)*4);
#endif

//					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*4+k)*3);
					LBXGL_Shader_TexCoord2fv(chk->mesh->face_st+(j*3+k)*2);
					LBXGL_Shader_Vertex3fv(chk->mesh->face_xyz+(j*3+k)*3);
				}
			}
			LBXGL_Shader_EndLight();
			i=j-1;
		}

#if 1
		if(useva)
		{
			LBXGL_Shader_BindTexture(-1);

			for(i=0; i<chk->mesh->n_face; i++)
			{
				fl=BTGE_Texture_GetImageFlags(chk->mesh->face_tex[i]);
				if(fl&BTGE_TXFL_LIGHTMAT)
				{
					for(j=i; j<chk->mesh->n_face; j++)
						if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
							break;
					i=j-1;
					continue;
				}

				for(j=i; j<chk->mesh->n_face; j++)
					if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
						break;

				k=j-i;

				LBXGL_Shader_DrawArraysNormalFlatRGB(
					PDGL_TRIANGLES, i*3, k*3,
					3, GL_FLOAT, 0, chk->mesh->face_xyz,
					3, GL_BYTE, 3, chk->mesh->face_norm,
					4, GL_UNSIGNED_BYTE, 0, rgba);

				i+=k-1;
			}
		}
#endif

		return;
	}
#endif

	LBXGL_Shader_BindTexture(-1);

	if(light->flags&LBXGL_LFL_SUN)
	{
#if 1
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_slrgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz,
			3, GL_BYTE, 3, chk->mesh->face_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_slrgba);
#endif
		return;
	}

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysNormalFlatRGB(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
		3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
		4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	LBXGL_Shader_DrawArraysNormalFlatRGB(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, chk->mesh->face_xyz,
		3, GL_BYTE, 3, chk->mesh->face_norm,
		4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_rgba);
#endif
}

void LBXGL_Voxel_DrawChunkShadow(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
	static float *xyzarr=NULL;
	static int nxyz=0;

	float txyza[4*3], txyzb[4*3];
	float tv0[3], tv1[3], tv2[3], tv3[3];
	float dv0[3], dv1[3], lorg[3];
	float *xyz, *v0;

	float f, g, h;
	int i, j, k, l, n, i0, i1, nva;

	return;

//	if(light->flags&LBXGL_LFL_SUN)
	if(light->flags&LBXGL_LFL_SHADOWMAP)
		return;

	if(!chk->mesh->n_face)
		return;

	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 1024))
//		return;
	if(LBXGL_Voxel_GetChunkFrustumDistance(chk)>=1024)
		return;

//	n=chk->mesh->n_face*6*4;
	n=chk->mesh->n_face*(6+6*4);
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
	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=0; j<3; j++)
		{
			v0=chk->mesh->face_xyz+(i*3+j)*3;
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
//		v0=chk->mesh->face_norm+(i*4)*3;
		V3F_COPY(chk->mesh->face_norm+(i*3)*3, tv0);
		f=V3F_DOT(light->org, tv0)-V3F_DOT(txyza, tv0);
//		f=-f;
#else
		tv0[0]=(chk->mesh->face_norm+(i*3)*3)[0]*(1.0/127.0);
		tv0[1]=(chk->mesh->face_norm+(i*3)*3)[1]*(1.0/127.0);
		tv0[2]=(chk->mesh->face_norm+(i*3)*3)[2]*(1.0/127.0);
		f=V3F_DOT(light->org, tv0)-V3F_DOT(txyza, tv0);
#endif

		for(j=0; j<3; j++)
		{
			k=(j+1)%3;
			
			if(f>10)
			{
				V3F_COPY(txyza+k*3, xyz+0*3);
				V3F_COPY(txyza+j*3, xyz+1*3);
				V3F_COPY(txyzb+j*3, xyz+2*3);

				V3F_COPY(txyzb+j*3, xyz+3*3);
				V3F_COPY(txyzb+k*3, xyz+4*3);
				V3F_COPY(txyza+k*3, xyz+5*3);
				xyz+=6*3;
			}else if(f<10)
			{
				V3F_COPY(txyza+k*3, xyz+5*3);
				V3F_COPY(txyza+j*3, xyz+4*3);
				V3F_COPY(txyzb+j*3, xyz+3*3);

				V3F_COPY(txyzb+j*3, xyz+2*3);
				V3F_COPY(txyzb+k*3, xyz+1*3);
				V3F_COPY(txyza+k*3, xyz+0*3);

				xyz+=6*3;
			}
		}
	}

	i=(xyz-xyzarr)/3;
	if(i>n)
	{
		*(int *)-1=-1;
	}

	LBXGL_Shader_DrawArraysSimpleFlat(
		PDGL_TRIANGLES, 0, i,
		3, GL_FLOAT, 0, xyzarr);
}

void LBXGL_Voxel_DrawChunkForLight(
	LBXGL_VoxelChunk *chk, LBXGL_Light *light)
{
	byte *rgba;
	byte *rgba2;
	bool useva;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_face)
		return;
//	if(chk->flags&VOX_CHFL_VISCLIP)
//		return;

	if(!LBXGL_Voxel_CheckChunkLight(chk, light))
		return;

#if 1
//	if(chk->flags&VOX_CHFL_USESHADER)
	if((chk->flags&VOX_CHFL_USESHADER) && !(chk->flags&VOX_CHFL_DISTANT))
	{
		LBXGL_Shader_NormalX3f(0, 0, 0);
		LBXGL_Shader_NormalY3f(0, 0, 0);

		if(light->flags&LBXGL_LFL_SUN)
			rgba=chk->mesh->face_slrgba;
		else rgba=chk->mesh->face_rgba;

		if(light->flags&LBXGL_LFL_SUN)
			rgba2=(byte *)chk->mesh->vbo_face_slrgba;
		else rgba2=(byte *)chk->mesh->vbo_face_rgba;

		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);

		for(i=0; i<chk->mesh->n_face; i++)
		{
#if 1
//			if(BTGE_Voxel_VoxelAlphaP(&(chk->mesh->face_vox[i])))
			if(BTGE_Voxel_VoxelNoShadowP(&(chk->mesh->face_vox[i])))
			{
				for(j=i; j<chk->mesh->n_face; j++)
					if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
						break;
				i=j;
				continue;
			}
#endif

			for(j=i; j<chk->mesh->n_face; j++)
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

			k=j-i;

			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);

			LBXGL_Shader_DrawArraysNormalTexRGB(
				PDGL_TRIANGLES, i*3, k*3,
				3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
				2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
				3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
				4, GL_UNSIGNED_BYTE, 0, rgba2);

			i+=k-1;
		}

		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		return;
	}
#endif

	LBXGL_Shader_BindTexture(-1);

	if(light->flags&LBXGL_LFL_SUN)
	{
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalFlatRGB(
			PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_slrgba);
		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
		return;
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	LBXGL_Shader_DrawArraysNormalFlatRGB(
		PDGL_TRIANGLES, 0, chk->mesh->n_face*3,
		3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
		3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
		4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LBXGL_Voxel_DrawChunkFinal(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1,1,1,1);

#if 0
	if(chk->flags&VOX_CHFL_USESHADER)
	{
//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);

		//LBXGL_Shader_Color4f(1,1,1,1);
		for(i=0; i<chk->mesh->n_face; i++)
		{
			tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
			LBXGL_Shader_BindTexture(tn);
//			LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

			LBXGL_Shader_Begin(PDGL_TRIANGLES);
			for(j=i; j<chk->mesh->n_face; j++)
			{
				if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
					break;

				for(k=0; k<3; k++)
				{
#ifndef BTGE_VOXEL_BYTERGBA
					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*3+k)*3);
#else
					LBXGL_Shader_Normal3sbv(chk->mesh->face_norm+(j*3+k)*3);
#endif

//					LBXGL_Shader_Normal3fv(chk->mesh->face_norm+(j*3+k)*3);
					LBXGL_Shader_TexCoord2fv(chk->mesh->face_st+(j*3+k)*2);
					LBXGL_Shader_Vertex3fv(chk->mesh->face_xyz+(j*3+k)*3);
				}
			}
			LBXGL_Shader_End();
			i=j-1;
		}

//		LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);
//		LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
		LBXGL_Shader_Color4f(1,1,1,1);
		return;
	}
#endif

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=i; j<chk->mesh->n_face; j++)
			if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
//		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm);
//		pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
		LBXGL_Shader_DrawArraysNormalTex(PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz,
			2, GL_FLOAT, 0, chk->mesh->face_st,
			3, GL_BYTE, 3, chk->mesh->face_norm);
#endif

		i+=k-1;
	}
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void LBXGL_Voxel_DrawChunkFinalRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

//	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
//		return;

#if 1
	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=i; j<chk->mesh->n_face; j++)
			if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_face_rgba);
#endif

#if 0
		LBXGL_Shader_DrawArraysNormalTexRGB(PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, chk->mesh->face_xyz,
			2, GL_FLOAT, 0, chk->mesh->face_st,
			3, GL_BYTE, 3, chk->mesh->face_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->mesh->face_rgba);
#endif

		i+=k-1;
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void LBXGL_Voxel_DrawChunkFinal_AlphaLightMat(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!(chk->flags&VOX_CHFL_ALPHALIGHTMAT))
		return;
	if(chk->flags&VOX_CHFL_DISTANT)
		return;

//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ZERO);
//	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE);
//	LBXGL_Shader_BlendFunc(GL_SRC_COLOR, GL_ONE);

	LBXGL_Shader_BlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1,1,1,1);

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if(!(fl&BTGE_TXFL_LIGHTMAT)) { i=j-1; continue; }

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->aface_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm);

		i+=k-1;
	}
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void LBXGL_Voxel_DrawChunkBasic(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_face)
		return;
	if(chk->flags&VOX_CHFL_VISCLIP)
		return;

	for(i=0; i<chk->mesh->n_face; i++)
	{
		for(j=i; j<chk->mesh->n_face; j++)
			if(chk->mesh->face_tex[j]!=chk->mesh->face_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_face);
		LBXGL_Shader_DrawArraysNormalTex(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_face_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_face_norm);

		i+=k-1;
	}

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LBXGL_Voxel_DrawChunkAlphaRGB(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn, fl;

//	if(!chk->mesh->n_face && !chk->mesh->n_aface)
	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		if(BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
			continue;

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

		LBXGL_Shader_Begin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3sbv(chk->mesh->aface_norm+(j*3+k)*3);
#endif
				LBXGL_Shader_TexCoord2fv(chk->mesh->aface_st+(j*3+k)*2);
				LBXGL_Shader_Vertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif

#if 1
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
		if(BTGE_Voxel_VoxelFluidP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

#if 1
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if((fl&BTGE_TXFL_LIGHTMAT) && !(chk->flags&VOX_CHFL_DISTANT))
			{ i=j-1; continue; }
#endif
		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_rgba);
#endif

#if 0
		LBXGL_Shader_DrawArraysNormalTexRGB(PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, chk->mesh->aface_xyz,
			2, GL_FLOAT, 0, chk->mesh->aface_st,
			3, GL_BYTE, 3, chk->mesh->aface_norm,
			4, GL_UNSIGNED_BYTE, 0, chk->mesh->aface_rgba);
#endif

		i+=k-1;
	}
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void LBXGL_Voxel_DrawChunkSimpleAlphaFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh->n_aface)
		return;
	LBXGL_Shader_DrawArraysSimpleFlat(PDGL_TRIANGLES, 0, chk->mesh->n_aface*3,
		3, GL_FLOAT, 0, chk->mesh->aface_xyz);
}

void LBXGL_Voxel_DrawChunkAlphaFlat(LBXGL_VoxelChunk *chk)
{
//	if(!chk->mesh->n_face && !chk->mesh->n_aface)
	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	LBXGL_Shader_DrawArraysNormalFlat(PDGL_TRIANGLES, 0, chk->mesh->n_aface*3,
		3, GL_FLOAT, 0, chk->mesh->aface_xyz,
		3, GL_BYTE, 3, chk->mesh->aface_norm);
}

void LBXGL_Voxel_DrawChunkAlphaFluidFlat(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_aface)return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;
	if(!LBXGL_Voxel_CheckChunkFrustum(chk))return;

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
		if(!BTGE_Voxel_VoxelFluidP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
//		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
//		LBXGL_Shader_BindTexture(tn);

#if 1
		LBXGL_Shader_DrawArraysNormalFlat(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm);
#endif

		i+=k-1;
	}
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
			continue;
	
		pdglBegin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
#ifndef BTGE_VOXEL_BYTERGBA
				pdglNormal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				pdglNormal3bv(chk->mesh->aface_norm+(j*3+k)*3);
#endif
				pdglVertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
			}
		}
		pdglEnd();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawChunkAlphaFluid(LBXGL_VoxelChunk *chk)
{
	int i, j, k, tn;

	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 1
	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
		if(!BTGE_Voxel_VoxelFluidP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture2(chk, chk->mesh->face_tex[i]);
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);

#if 1
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_rgba);
#endif

		i+=k-1;
	}
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		if(!BTGE_Voxel_VoxelFluidP(chk->mesh->aface_vox[i]))
			continue;
	
		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

		glDisable(GL_CULL_FACE);

//		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
//		LBXGL_Shader_BindTexture(tn);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		LBXGL_Shader_Begin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
//				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
//				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3sbv(chk->mesh->aface_norm+(j*3+k)*3);
#endif

				LBXGL_Shader_TexCoord2fv(chk->mesh->aface_st+(j*3+k)*2);
				LBXGL_Shader_Vertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
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

	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

	if(!LBXGL_Voxel_CheckChunkFrustum(chk))
		return;

#if 1
	tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
	LBXGL_Shader_BindTexture(tn);

	pdglBindBuffer(GL_ARRAY_BUFFER, chk->mesh->vbo_aface);
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		if(!BTGE_Voxel_VoxelFluidOverlayP(chk->mesh->aface_vox[i]))
		if(!BTGE_Voxel_VoxelFluidOverlayP(&(chk->mesh->aface_vox[i])))
		{
			for(j=i; j<chk->mesh->n_aface; j++)
				if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
					break;
			i=j-1;
			continue;
		}

		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

		k=j-i;

//		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
//		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
//		LBXGL_Shader_BindTexture(tn);

#if 1
		LBXGL_Shader_DrawArraysNormalTexRGB(
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_xyz,
			2, GL_FLOAT, 0, (byte *)chk->mesh->vbo_aface_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->vbo_aface_norm,
			4, GL_UNSIGNED_BYTE, 0, (byte *)chk->mesh->vbo_aface_rgba);
#endif

		i+=k-1;
	}
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

#if 0
	for(i=0; i<chk->mesh->n_aface; i++)
	{
//		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
//		LBXGL_Shader_BindTexture(tn);
//		LBXGL_Shader_BindTexture(chk->mesh->face_tex[i]);

		if(!BTGE_Voxel_VoxelFluidOverlayP(chk->mesh->aface_vox[i]))
			continue;

		tn=LBXGL_Texture_LoadImage("textures/shader/watery_base");
		LBXGL_Shader_BindTexture(tn);
		LBXGL_Shader_Color4f(1, 1, 1, 1);

		glDisable(GL_CULL_FACE);

		LBXGL_Shader_Begin(PDGL_TRIANGLES);
		for(j=i; j<chk->mesh->n_aface; j++)
		{
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;

			for(k=0; k<3; k++)
			{
//				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*4+k)*4);
//				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*4+k)*3);
#ifndef BTGE_VOXEL_BYTERGBA
				LBXGL_Shader_Color4fv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3fv(chk->mesh->aface_norm+(j*3+k)*3);
#else
				LBXGL_Shader_Color4ubv(chk->mesh->aface_rgba+(j*3+k)*4);
				LBXGL_Shader_Normal3sbv(chk->mesh->aface_norm+(j*3+k)*3);
#endif
				LBXGL_Shader_TexCoord2fv(chk->mesh->aface_st+(j*3+k)*2);
				LBXGL_Shader_Vertex3fv(chk->mesh->aface_xyz+(j*3+k)*3);
			}
		}
		LBXGL_Shader_End();
		i=j-1;
	}
#endif
}

void LBXGL_Voxel_DrawBeginTranslateForRegion(LBXGL_VoxelRegion *rgn)
{
	float tv[3];

#if 1
	tv[0]=(rgn->ox*rgn->voxsz)-lbxgl_voxel_reforg[0];
	tv[1]=(rgn->oy*rgn->voxsz)-lbxgl_voxel_reforg[1];
	tv[2]=(rgn->oz*rgn->voxsz)-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(tv[0], tv[1], tv[2]);
#endif

#if 0
	tv[0]=-lbxgl_voxel_reforg[0];
	tv[1]=-lbxgl_voxel_reforg[1];
	tv[2]=-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(tv[0], tv[1], tv[2]);
#endif
}

void LBXGL_Voxel_DrawEndTranslateForRegion(LBXGL_VoxelRegion *rgn)
{
	float tv[3];

#if 1
	tv[0]=(rgn->ox*rgn->voxsz)-lbxgl_voxel_reforg[0];
	tv[1]=(rgn->oy*rgn->voxsz)-lbxgl_voxel_reforg[1];
	tv[2]=(rgn->oz*rgn->voxsz)-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(-tv[0], -tv[1], -tv[2]);
#endif

#if 0
	tv[0]=-lbxgl_voxel_reforg[0];
	tv[1]=-lbxgl_voxel_reforg[1];
	tv[2]=-lbxgl_voxel_reforg[2];
	LBXGL_Shader_Translatef(-tv[0], -tv[1], -tv[2]);
#endif
}

void LBXGL_Voxel_DrawRegionQuery(LBXGL_VoxelRegion *rgn)
{
	float mins[3], maxs[3];
	LBXGL_VoxelChunk *cur;
	LBXGL_Light *lcur;
	int i;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

#ifndef GLES
	cur=rgn->vischk;
	while(cur)
	{
		pdglBeginQuery(GL_SAMPLES_PASSED, cur->visid);
		LBXGL_Voxel_DrawChunkSimpleFlat(cur);
		pdglEndQuery(GL_SAMPLES_PASSED);
		cur=cur->mesh->vnext;
	}

	cur=rgn->vischk;
	while(cur)
	{
		pdglGetQueryObjectuiv(cur->visid, GL_QUERY_RESULT, &i);
//		i=1;
		//if(i>0)
		if(i!=0)
			{ cur->flags&=~VOX_CHFL_VISCLIP; }
		else
			{ cur->flags|=VOX_CHFL_VISCLIP; }
		cur=cur->mesh->vnext;
	}

	glDepthMask(GL_FALSE);

	cur=rgn->vischk;
	while(cur)
	{
		if(cur->mesh->n_aface)
		{
			pdglBeginQuery(GL_SAMPLES_PASSED, cur->visid);
			LBXGL_Voxel_DrawChunkSimpleAlphaFlat(cur);
			pdglEndQuery(GL_SAMPLES_PASSED);
		}
		cur=cur->mesh->vnext;
	}

	cur=rgn->vischk;
	while(cur)
	{
		if(cur->mesh->n_aface)
		{
			pdglGetQueryObjectuiv(cur->visid, GL_QUERY_RESULT, &i);
//			i=1;
			//if(i>0)
			if(i!=0)
				{ cur->flags&=~VOX_CHFL_ALPHAVISCLIP; }
			else
				{ cur->flags|=VOX_CHFL_ALPHAVISCLIP; }
		}
		cur=cur->mesh->vnext;
	}

#if 0
	glDisable(GL_CULL_FACE);
	cur=rgn->lvischk;
	while(cur)
	{
		if(cur->light)
		{
			pdglBeginQuery(GL_SAMPLES_PASSED, cur->visid);
			lcur=cur->light;
			while(lcur)
			{
				lcur->org[0]=(cur->ox*32)+lcur->rel_org[0];
				lcur->org[1]=(cur->oy*32)+lcur->rel_org[1];
				lcur->org[2]=(cur->oz*32)+lcur->rel_org[2];
				V3F_ADD(lcur->org, lcur->mins, mins);
				V3F_ADD(lcur->org, lcur->maxs, maxs);
				PD3D_DrawFlatCube4(
					mins[0], mins[1], mins[2],
					maxs[0], maxs[1], maxs[2]);
				lcur=lcur->next;
			}
//			LBXGL_Voxel_DrawChunkSimpleAlphaFlat(cur);
			pdglEndQuery(GL_SAMPLES_PASSED);
		}
		cur=cur->mesh->lvnext;
	}

	cur=rgn->lvischk;
	while(cur)
	{
		if(cur->mesh->n_aface)
		{
			pdglGetQueryObjectuiv(cur->visid, GL_QUERY_RESULT, &i);
//			i=1;
			//if(i>0)
			if(i!=0)
				{ cur->flags&=~VOX_CHFL_LIGHTVISCLIP; }
			else
				{ cur->flags|=VOX_CHFL_LIGHTVISCLIP; }
		}
		cur=cur->mesh->lvnext;
	}
	glEnable(GL_CULL_FACE);
#endif

	glDepthMask(GL_TRUE);
#endif

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionLight(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkLight(cur, light);
		cur=cur->mesh->vnext;
	}

#if 1
	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkLight_AlphaLightMat(cur, light);
		cur=cur->mesh->avnext;
	}
#endif

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionShadow(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->svischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkShadow(cur, light);
		cur=cur->mesh->svnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionForLight(
	LBXGL_VoxelRegion *rgn, LBXGL_Light *light)
{
	LBXGL_VoxelChunk *cur;

//	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))return;
	if(!LBXGL_Voxel_CheckRegionLight(rgn, light))return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->svischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkForLight(cur, light);
		cur=cur->mesh->svnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlat(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFlatRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatRGB(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFlatVLRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatVLRGB(cur);
		cur=cur->mesh->vnext;
	}

#if 1
	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFlatVLRGB_AlphaLightMat(cur);
		cur=cur->mesh->avnext;
	}
#endif

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFinal(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinal(cur);
		cur=cur->mesh->vnext;
	}

#if 1
	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinal_AlphaLightMat(cur);
		cur=cur->mesh->avnext;
	}
#endif

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionFinalRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkFinalRGB(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionBasic(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkBasic(cur);
		cur=cur->mesh->vnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaRGB(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaRGB(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFlat(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFluidFlat(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluidFlat(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFluid(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluid(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_DrawRegionAlphaFluid2(LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *cur;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->avischk;
	while(cur)
	{
		LBXGL_Voxel_DrawChunkAlphaFluid2(cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
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

void LBXGL_Voxel_DrawRegionListForLight(
	LBXGL_VoxelRegion *lst, LBXGL_Light *light)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionForLight(cur, light);
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

void LBXGL_Voxel_DrawRegionListBasic(LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_DrawRegionBasic(cur);
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
		cur=cur->mesh->wavnext;
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

void LBXGL_Voxel_CheckUploadVBO(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	if((chk->flags&VOX_CHFL_VBODIRTY) &&
		!(chk->flags&VOX_CHFL_REBUILDING))
	{
		LBXGL_Voxel_CheckFlushVBO(world, chk);
		chk->flags&=~VOX_CHFL_VBODIRTY;
	}

	LBXGL_Voxel_CheckUploadSolidVBO(world, chk);
	LBXGL_Voxel_CheckUploadAlphaVBO(world, chk);
	glFinish();
}

void LBXGL_Voxel_CheckUploadSolidVBO(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	byte *buf, *ct;
	int n, id, sz;

	if(chk->mesh->vbo_face>0)
		return;

	n=chk->mesh->n_face*3;
	if(n<=0)return;

	sz= n*3*sizeof(float) + 
		n*2*sizeof(float) +
		n*3 +		n*3 +
		n*3 +		n*4 +
		n*4 +		n*4;
		
	buf=malloc(sz);
	ct=buf;

	chk->mesh->vbo_face_xyz=ct-buf;
	memcpy(ct, chk->mesh->face_xyz, n*3*sizeof(float));
	ct+=n*3*sizeof(float);

	chk->mesh->vbo_face_st=ct-buf;
	memcpy(ct, chk->mesh->face_st, n*2*sizeof(float));
	ct+=n*2*sizeof(float);

	chk->mesh->vbo_face_norm=ct-buf;
	memcpy(ct, chk->mesh->face_norm, n*3);
	ct+=n*3;

	chk->mesh->vbo_face_norm_x=ct-buf;
	memcpy(ct, chk->mesh->face_norm_x, n*3);
	ct+=n*3;

	chk->mesh->vbo_face_norm_y=ct-buf;
	memcpy(ct, chk->mesh->face_norm_y, n*3);
	ct+=n*3;

	chk->mesh->vbo_face_rgba=ct-buf;
	memcpy(ct, chk->mesh->face_rgba, n*4);
	ct+=n*4;

	chk->mesh->vbo_face_slrgba=ct-buf;
	memcpy(ct, chk->mesh->face_slrgba, n*4);
	ct+=n*4;

	chk->mesh->vbo_face_vlrgba=ct-buf;
	memcpy(ct, chk->mesh->face_vlrgba, n*4);
	ct+=n*4;
	
	pdglGenBuffers(1, &id);
	pdglBindBuffer(GL_ARRAY_BUFFER, id);
	pdglBufferData(GL_ARRAY_BUFFER, sz, buf, GL_STATIC_DRAW);
	chk->mesh->vbo_face=id;

	free(buf);

	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LBXGL_Voxel_CheckUploadAlphaVBO(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	byte *buf, *ct;
	int n, id, sz;

	if(chk->mesh->vbo_aface>0)
		return;

	n=chk->mesh->n_aface*3;
	if(n<=0)return;
	
	sz= n*3*sizeof(float) + 
		n*2*sizeof(float) +
		n*3 +		n*3 +
		n*3 +		n*4 +
		n*4 +		n*4;
		
	buf=malloc(sz);
	ct=buf;

	chk->mesh->vbo_aface_xyz=ct-buf;
	memcpy(ct, chk->mesh->aface_xyz, n*3*sizeof(float));
	ct+=n*3*sizeof(float);

	chk->mesh->vbo_aface_st=ct-buf;
	memcpy(ct, chk->mesh->aface_st, n*2*sizeof(float));
	ct+=n*2*sizeof(float);

	chk->mesh->vbo_aface_norm=ct-buf;
	memcpy(ct, chk->mesh->aface_norm, n*3);
	ct+=n*3;

//	chk->mesh->vbo_aface_norm_x=ct-buf;
//	memcpy(ct, chk->mesh->face_norm_x, n*3);
//	ct+=n*3;

//	chk->mesh->vbo_aface_norm_y=ct-buf;
//	memcpy(ct, chk->mesh->face_norm_y, n*3);
//	ct+=n*3;

	chk->mesh->vbo_aface_rgba=ct-buf;
	memcpy(ct, chk->mesh->aface_rgba, n*4);
	ct+=n*4;

	chk->mesh->vbo_aface_slrgba=ct-buf;
	memcpy(ct, chk->mesh->aface_slrgba, n*4);
	ct+=n*4;

	chk->mesh->vbo_aface_vlrgba=ct-buf;
	memcpy(ct, chk->mesh->aface_vlrgba, n*4);
	ct+=n*4;
	
	pdglGenBuffers(1, &id);
	pdglBindBuffer(GL_ARRAY_BUFFER, id);
	pdglBufferData(GL_ARRAY_BUFFER, sz, buf, GL_STATIC_DRAW);
	chk->mesh->vbo_aface=id;

	free(buf);
	pdglBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LBXGL_Voxel_CheckFlushVBO(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	if(!chk->mesh)
		return;

	if(chk->mesh->vbo_face>0)
	{
		pdglDeleteBuffers(1, &(chk->mesh->vbo_face));
		chk->mesh->vbo_face=0;
	}
	if(chk->mesh->vbo_aface>0)
	{
		pdglDeleteBuffers(1, &(chk->mesh->vbo_aface));
		chk->mesh->vbo_aface=0;
	}
}

void LBXGL_Voxel_MarkDirtyVBO(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	chk->flags|=VOX_CHFL_VBODIRTY;
//	LBXGL_Voxel_CheckFlushVBO(world, chk);
}

void LBXGL_Voxel_CheckFlushChunkLights(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	LBXGL_Light *lcur;

	if(chk->clean)
		return;

	if(chk->mesh)
	{
		lcur=chk->mesh->light;
		while(lcur)
		{
			LBXGL_BrushWorld_FlushLightEnvMap(world, lcur);
			lcur->flags&=~(LBXGL_LFL_ENVRENDER|
				LBXGL_LFL_SHADOWMAP);
			lcur=lcur->next;
		}
	}
}

void LBXGL_Voxel_CheckFlushRegion(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *chk, *ccur, *cprv;
	int i, j, k, l, n, m, cn;
	float f, g, vsc;

	rgn->lvischk=NULL;
	rgn->svischk=NULL;
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
			continue;
		}

		LBXGL_Voxel_CheckFlushVBO(world, chk);
		LBXGL_Voxel_CheckFlushChunkLights(world, chk);
	}
}

struct chkdata_s {
	LBXGL_BrushWorld *world;
	LBXGL_VoxelChunk *chk;
	int v;
};

void *LBXGL_Voxel_CheckRebuildChunk_cb(void *data)
{
	struct chkdata_s *inf;
	LBXGL_BrushWorld *world;
	LBXGL_VoxelChunk *chk;

	inf=data;
	if(inf->v!=1)
		return(NULL);
	world=inf->world;
	chk=inf->chk;
	inf->v=0;
	gcfree(data);

//	chk->flags|=VOX_CHFL_REBUILDING;
	LBXGL_Voxel_CheckFlushChunkLights(world, chk);
	BTGE_Voxel_CheckRebuildChunk(world, chk);
	chk->flags&=~VOX_CHFL_REBUILDING;
	
	return(BGBGC_NULLEND);
}

void LBXGL_Voxel_CheckRebuildChunk(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	struct chkdata_s *inf;

	inf=gcalloc(sizeof(struct chkdata_s));
	inf->world=world;
	inf->chk=chk;
	inf->v=1;
	
	chk->flags|=VOX_CHFL_REBUILDING;
//	thAddJob(LBXGL_Voxel_CheckRebuildChunk_cb, inf);
//	pdglAddJob(LBXGL_Voxel_CheckRebuildChunk_cb, inf);

#if 1
	chk->flags|=VOX_CHFL_REBUILDING;
	LBXGL_Voxel_CheckFlushChunkLights(world, chk);
	BTGE_Voxel_CheckRebuildChunk(world, chk);
	chk->flags&=~VOX_CHFL_REBUILDING;
#endif
}

void LBXGL_Voxel_RebuildRegionVisible(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *chk, *ccur, *cprv;
	LBXGL_VoxelChunk *rbdlst;
	int i, j, k, l, n, m, cn;
	float f, g, vsc;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	BTGE_VoxelSave_FlattenRegion(world, rgn);

	vsc=1;
	if(gcCheckMemoryLow())
		vsc=vsc*0.33;

	rgn->lvischk=NULL;
	rgn->svischk=NULL;
	rgn->avischk=NULL;
	rgn->vischk=NULL; n=0; m=0;
	rbdlst=NULL;

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
//			if(l>(lbxgl_voxel_chkstart+48))continue;
//			if(l>(lbxgl_voxel_chkstart+24))continue;
			if(l>(lbxgl_voxel_chkstart+12))continue;

			if(!LBXGL_Voxel_CheckRegionChunkFrustumDistance(
				rgn, i, j, k, 7168*vsc))
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
			if(f>(12288*vsc))
//			if(f>(16384*vsc))
			{
				LBXGL_Voxel_CheckFlushVBO(world, chk);
				BTGE_Voxel_FreeChunk(world, chk);
				rgn->chk[cn]=NULL;
				continue;
			}

#if 0
			if(f>(8192*vsc))
//			if(f>6144)
			{
				LBXGL_Voxel_CheckFlushVBO(world, chk);
//				BTGE_Voxel_FlushReleaseChunk(chk);
				BTGE_Voxel_FreeChunk(world, chk);
				rgn->chk[cn]=NULL;
				continue;
			}
#endif

			if(chk->mesh)
			{
				f=LBXGL_Voxel_GetChunkFrustumDistance(chk);
//				if(f<1024)
				if((f<768) && chk->mesh->light)
				{
					chk->mesh->lvnext=rgn->lvischk;
					rgn->lvischk=chk;
				}

				if((f<2048) && chk->mesh->n_face)
				{
					chk->mesh->svnext=rgn->svischk;
					rgn->svischk=chk;
				}
			}

			continue;
		}

		if(chk->mesh)
		{
			f=LBXGL_Voxel_GetChunkFrustumDistance(chk);
//			if(f<1024)
			if((f<768) && chk->mesh->light)
//			if(1)
			{
				chk->mesh->lvnext=rgn->lvischk;
				rgn->lvischk=chk;
			}

			if((f<2048) && chk->mesh->n_face)
			{
				chk->mesh->svnext=rgn->svischk;
				rgn->svischk=chk;
			}

//			if(f>1024)
			if(f>512)
				{ chk->flags|=VOX_CHFL_DISTANT; }
			else
				{ chk->flags&=~VOX_CHFL_DISTANT; }
		}

//		LBXGL_Voxel_CheckRebuildChunk(chk);
		if(!chk->clean && !(chk->flags&VOX_CHFL_REBUILDING))
		{
			if(lbxgl_voxel_chkleft<=0)
				continue;
			lbxgl_voxel_chkleft--;

			l=PDGL_TimeMS();
			if(l<lbxgl_voxel_chkstart)continue;
//			if(l>(lbxgl_voxel_chkstart+48))continue;
			if(l>(lbxgl_voxel_chkstart+24))continue;
//			if(l>(lbxgl_voxel_chkstart+12))continue;

#if 0
			f=LBXGL_Voxel_CalcChunkCameraDistance(chk);
			chk->tmp_dist=f;
		
			ccur=rbdlst; cprv=NULL;
			while(ccur)
			{
//				g=LBXGL_Voxel_CalcChunkCameraDistance(ccur);
				g=ccur->tmp_dist;
//				if(f>=g)break;
				if(f<=g)break;
				cprv=ccur; ccur=ccur->chain;
			}

			if(cprv)
				{ chk->chain=cprv->chain; cprv->chain=chk; }
			else
				{ chk->chain=rbdlst; rbdlst=chk; }
			continue;
#endif

#if 0
			chk->chain=rbdlst;
			rbdlst=chk;
			continue;
#endif

#if 1
			LBXGL_Voxel_MarkDirtyVBO(world, chk);
//			LBXGL_Voxel_CheckFlushVBO(world, chk);
//			BTGE_Voxel_CheckRebuildChunk(world, chk);
			LBXGL_Voxel_CheckRebuildChunk(world, chk);
//			LBXGL_Voxel_CheckUploadVBO(world, chk);
#endif
		}

		m++;
		
		if(chk->flags&VOX_CHFL_REBUILDING)
			continue;
		if(!chk->mesh)
			continue;
		if(!chk->mesh->n_face && !chk->mesh->n_aface)
			continue;

		LBXGL_Voxel_CheckUploadVBO(world, chk);

		if(chk->mesh->n_face && (chk->mesh->vbo_face<=0))
			continue;
		if(chk->mesh->n_aface && (chk->mesh->vbo_aface<=0))
			continue;

		if(chk->mesh->n_aface)
		{
			world->flags|=BTGE_WFL_VOXFLUID;
		}

#if 1
		f=LBXGL_Voxel_CalcChunkCameraDistance(chk);
		chk->tmp_dist=f;
		
		ccur=rgn->vischk; cprv=NULL;
		while(ccur)
		{
//			g=LBXGL_Voxel_CalcChunkCameraDistance(ccur);
			g=ccur->tmp_dist;
//			if(f>=g)break;
			if(f<=g)break;
			cprv=ccur; ccur=ccur->mesh->vnext;
		}

		if(cprv)
		{
			chk->mesh->vnext=cprv->mesh->vnext;
			cprv->mesh->vnext=chk;
		}else
		{
			chk->mesh->vnext=rgn->vischk;
			rgn->vischk=chk;
		}
#else
			chk->mesh->vnext=rgn->vischk;
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
			cprv=ccur; ccur=ccur->mesh->avnext;
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
		if(!ccur->mesh->n_aface)
			{ ccur=ccur->mesh->vnext; continue; }
	
		ccur->mesh->avnext=rgn->avischk;
		rgn->avischk=ccur;

		ccur=ccur->mesh->vnext;
	}

#if 0
	ccur=rbdlst;
	while(ccur)
	{
		if(lbxgl_voxel_chkleft<=0)
			break;
		lbxgl_voxel_chkleft--;

		l=PDGL_TimeMS();
		if(l<lbxgl_voxel_chkstart)break;
//		if(l>(lbxgl_voxel_chkstart+48))continue;
//		if(l>(lbxgl_voxel_chkstart+24))continue;
		if(l>(lbxgl_voxel_chkstart+12))break;

		if(l>(lbxgl_voxel_chkstart+6))
			{ if(ccur->tmp_dist>1024)break; }

		chk=ccur;
		LBXGL_Voxel_MarkDirtyVBO(world, chk);
//		LBXGL_Voxel_CheckFlushVBO(world, chk);
//		BTGE_Voxel_CheckRebuildChunk(world, chk);
		LBXGL_Voxel_CheckRebuildChunk(world, chk);
//		LBXGL_Voxel_CheckUploadVBO(world, chk);

		ccur=ccur->chain;
	}
#endif

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
					{ cur=cur->mesh->vnext; continue; }

#if 1
			f=LBXGL_Voxel_CalcChunkCameraDistance(cur);
		
			ccur=world->vox_vischk; cprv=NULL;
			while(ccur)
			{
				g=LBXGL_Voxel_CalcChunkCameraDistance(ccur);
//				if(f>=g)break;
				if(f<=g)break;
				cprv=ccur; ccur=ccur->mesh->wvnext;
			}

			if(cprv)
			{
				cur->mesh->wvnext=cprv->mesh->wvnext;
				cprv->mesh->wvnext=cur;
			}else
			{
				cur->mesh->wvnext=world->vox_vischk;
				world->vox_vischk=cur;
			}
#else
			cur->mesh->wvnext=world->vox_vischk;
			world->vox_vischk=chk;
#endif

			cur=cur->mesh->vnext;
		}

		rcur=rcur->next;
	}

	world->vox_avischk=NULL;
	ccur=world->vox_vischk;
	while(ccur)
	{
		if(!ccur->mesh->n_aface)
			{ ccur=ccur->mesh->wvnext; continue; }

		if(ccur->flags&VOX_CHFL_ALPHAVISCLIP)
			{ ccur=ccur->mesh->wvnext; continue; }
	
		ccur->mesh->wavnext=world->vox_avischk;
		world->vox_avischk=ccur;

		ccur=ccur->mesh->wvnext;
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

void LBXGL_Voxel_UnlinkRegion(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn)
{
	BTGE_VoxelRegion *cur, *prv;

	cur=world->vox_region; prv=NULL;
	while(cur)
	{
		if(cur==rgn)
		{
			if(prv)
				{ prv->next=cur->next; }
			else
				{ world->vox_region=cur->next; }
			break;
		}
		prv=cur; cur=cur->next;
	}
}

void LBXGL_Voxel_RebuildWorldRegions(LBXGL_BrushWorld *world)
{
//	LBXGL_Voxel_RebuildRegionListVisible(world, world->vox_region);

	LBXGL_VoxelRegion *cur, *prv, *nxt;

	cur=world->vox_region; prv=NULL;
	while(cur)
	{
//		if(!LBXGL_Voxel_CheckRegionDistance(cur, 24576))
		if(!LBXGL_Voxel_CheckRegionDistance(cur, 32768))
//		if(!LBXGL_Voxel_CheckRegionDistance(cur, 49152))
		{
			nxt=cur->next;
			LBXGL_Voxel_CheckFlushRegion(world, cur);
			LBXGL_Voxel_UnlinkRegion(world, cur);
			BTGE_Voxel_FreeRegion(world, cur);
			cur=nxt;
			continue;
		}
	
		LBXGL_Voxel_RebuildRegionVisible(world, cur);
		prv=cur; cur=cur->next;
	}
}

void LBXGL_Voxel_BeginTranslateForWorld(LBXGL_BrushWorld *world)
{
	float tv[3];

//	glMatrixMode(GL_MODELVIEW);
//	glPushMatrix();
	LBXGL_Shader_PushMatrix();

	lbxgl_voxel_reforg[0]=world->cam_reforg[0];
	lbxgl_voxel_reforg[1]=world->cam_reforg[1];
	lbxgl_voxel_reforg[2]=world->cam_reforg[2];

#if 0
	tv[0]=-world->cam_reforg[0];
	tv[1]=-world->cam_reforg[1];
	tv[2]=-world->cam_reforg[2];
//	glTranslatef(tv[0], tv[1], tv[2]);
	LBXGL_Shader_Translatef(tv[0], tv[1], tv[2]);
#endif
}

void LBXGL_Voxel_EndTranslateForWorld(LBXGL_BrushWorld *world)
{
	LBXGL_Shader_PopMatrix();
//	glMatrixMode(GL_MODELVIEW);
//	glPopMatrix();
}

void LBXGL_Voxel_DrawWorldQuery(LBXGL_BrushWorld *world)
{
//	lbxgl_voxel_chkleft=16;
	lbxgl_voxel_chkleft=64;
	lbxgl_voxel_chkstart=PDGL_TimeMS();

//	pdglColor4f(0, 0, 0, 1);
	pdglColor4f(1, 1, 1, 1);

	world->flags&=~BTGE_WFL_VOXFLUID;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_GenerateChunks(world);
//	BTGE_Voxel_GenerateChunks(world);
//	LBXGL_Voxel_RebuildRegionListVisible(world, world->vox_region);
	LBXGL_Voxel_RebuildWorldRegions(world);
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

//	lbxgl_voxel_chkleft=16;
	lbxgl_voxel_chkleft=64;
	lbxgl_voxel_chkstart=PDGL_TimeMS();

//	pdglColor4f(0, 0, 0, 1);
	pdglColor4f(1, 1, 1, 1);

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

	pdglColor4f(1, 1, 1, 1);
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

void LBXGL_Voxel_DrawWorldForLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
	BTGE_ModelState *mdls;

	//for now...
//	if(light->flags&LBXGL_LFL_SUN)
//		return;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
	LBXGL_Voxel_DrawRegionListForLight(world->vox_region, light);

//	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
//	LBXGL_BrushWorld_DrawModelListShadow(world, mdls, light);

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

	pdglColor4f(1, 1, 1, 1);
	LBXGL_Voxel_DrawRegionListFinal(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListFinal(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldFlat(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	pdglColor4f(1, 1, 1, 1);
	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelListFlat(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorld(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Shader_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LBXGL_Shader_Color4f(1, 1, 1, 1);

	LBXGL_Voxel_BeginTranslateForWorld(world);

	pdglColor4f(1, 1, 1, 1);
	LBXGL_Voxel_DrawRegionListBasic(world->vox_region);

	mdls=LBXGL_Voxel_QueryModelsVisibleWorld(world);
	LBXGL_BrushWorld_DrawModelList(world, mdls);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_DrawWorldAlpha(LBXGL_BrushWorld *world)
{
	BTGE_ModelState *mdls;

	LBXGL_Voxel_BeginTranslateForWorld(world);

//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
//	LBXGL_Voxel_DrawRegionListFinalRGB(world->vox_region);
	LBXGL_Voxel_DrawRegionListAlphaRGB(world->vox_region);
//	LBXGL_Voxel_DrawWorldChunksAlphaRGB(world);

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
	x+=world->cam_reforg[0]/32;
	y+=world->cam_reforg[1]/32;
	z+=world->cam_reforg[2]/32;

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
	x+=world->cam_reforg[0]/32;
	y+=world->cam_reforg[1]/32;
	z+=world->cam_reforg[2]/32;

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

//	x=floor((org[0]+world->cam_reforg[0])/32);
//	y=floor((org[1]+world->cam_reforg[1])/32);
//	z=floor((org[2]+world->cam_reforg[2])/32);

	x=floor((org[0])/32);
	y=floor((org[1])/32);
	z=floor((org[2])/32);
	x+=world->cam_reforg[0]/32;
	y+=world->cam_reforg[1]/32;
	z+=world->cam_reforg[2]/32;

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

void LBXGL_Voxel_GetPointSunLightColor(
	LBXGL_BrushWorld *world, float *org, float *rgb)
{
	BTGE_VoxelData *vox;
	int x, y, z;
	float f;
	int i;

	x=floor((org[0]+world->cam_reforg[0])/32);
	y=floor((org[1]+world->cam_reforg[1])/32);
	z=floor((org[2]+world->cam_reforg[2])/32);

#if 0
	vox=BTGE_Voxel_GetWorldVoxel(world, x, y, z);

	if(!vox)return(1.0);

//	i=(vox->aux>>4)&15;
	i=(vox->slight>>4)&15;
	f=pow(0.80, 15-i);
//	f=pow(0.60, 15-i);
	return(f);
#endif

	BTGE_Voxel_CalculateSunColorForPoint(world, x, y, z,
		rgb+0, rgb+1, rgb+2);
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
	float mins[3], maxs[3];
	LBXGL_VoxelChunk *cur;
	LBXGL_Light *lcur;
	float f;
	int i;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return(lst);

	cur=rgn->lvischk;
	while(cur)
	{
//		LBXGL_Voxel_DrawChunkFlatRGB(cur);

//		if(cur->flags&VOX_CHFL_LIGHTVISCLIP)
//			{ cur=cur->mesh->lvnext; continue; }

		f=LBXGL_Voxel_GetChunkFrustumDistance(cur);
//		if(f>1024)
//		if(f>768)
		if(f>512)
		{
			lcur=cur->mesh->light;
			while(lcur)
			{
				LBXGL_BrushWorld_FlushLightEnvMap(world, lcur);
				lcur->flags&=~(LBXGL_LFL_ENVRENDER|
					LBXGL_LFL_SHADOWMAP);
				lcur=lcur->next;
			}

			cur=cur->mesh->lvnext;
			continue;
		}

		lcur=cur->mesh->light;
		while(lcur)
		{
//			lcur->org[0]=(cur->ox*32)+lcur->rel_org[0];
//			lcur->org[1]=(cur->oy*32)+lcur->rel_org[1];
//			lcur->org[2]=(cur->oz*32)+lcur->rel_org[2];

			lcur->org[0]=(cur->ox*32-world->cam_reforg[0])+lcur->rel_org[0];
			lcur->org[1]=(cur->oy*32-world->cam_reforg[1])+lcur->rel_org[1];
			lcur->org[2]=(cur->oz*32-world->cam_reforg[2])+lcur->rel_org[2];

			V3F_ADD(lcur->org, lcur->mins, mins);
			V3F_ADD(lcur->org, lcur->maxs, maxs);
			if(!LBXGL_Brush_CheckBBoxFrustum(mins, maxs))
			{
//				f=V3F_DIST(lcur->org, lbxgl_cam->org);
//				if(f>384)
//					{ LBXGL_BrushWorld_FlushLightEnvMap(world, lcur); }
				lcur=lcur->next;
//				return(false);
				continue;
			}

			lcur->chain=lst;
			lst=lcur;
			
			lcur=lcur->next;
		}

		cur=cur->mesh->lvnext;
	}
	
	return(lst);
}

LBXGL_API LBXGL_Light *LBXGL_Voxel_QueryLightsCamWorld(
	LBXGL_BrushWorld *world)
{
	LBXGL_VoxelRegion *cur;
	LBXGL_Light *lst, *lcur;
	float f;
	int i;

	cur=world->vox_region; lst=NULL;
	while(cur)
	{
		lst=LBXGL_Voxel_QueryLightsCamRegion(world, cur, lst);
		cur=cur->next;
	}

	lcur=lst; i=0;
	while(lcur)
	{
		if(i>32) { lcur->chain=NULL; break; }
		lcur=lcur->chain; i++;
	}

#if 1
	i=btCvarGetf("r_shadowmap");
	if(i)
	{
		lcur=lst;
		while(lcur)
		{
			f=V3F_DIST(lcur->org, lbxgl_cam->org);
//			if(f<384)
//			if(1)
			if(!(lcur->flags&BTGE_LFL_NOSHADOW))
			{
				if(!(lcur->flags&LBXGL_LFL_ENVRENDER))
				{
					LBXGL_BrushWorld_UpdateLightEnvMap_Delay(
						world, lcur);
				}

				lcur->flags|=LBXGL_LFL_ENVRENDER|
					LBXGL_LFL_SHADOWMAP;
			}else
			{
//				LBXGL_BrushWorld_FlushLightEnvMap(world, lcur);
//				lcur->flags&=~(LBXGL_LFL_ENVRENDER|
//					LBXGL_LFL_SHADOWMAP);
			}
			lcur=lcur->chain;
		}
	}
#endif

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
			{ cur=cur->mesh->vnext; continue; }

		f=LBXGL_Voxel_GetChunkFrustumDistance(cur);
//		if(f>1024)
		if(f>768)
//		if(f>512)
			{ cur=cur->mesh->vnext; continue; }

		mcur=cur->mesh->props;
		while(mcur)
		{
			mcur->org[0]=(cur->ox*32)+mcur->rel_org[0];
			mcur->org[1]=(cur->oy*32)+mcur->rel_org[1];
			mcur->org[2]=(cur->oz*32)+mcur->rel_org[2];

			mcur->chain=lst;
			lst=mcur;
			
			mcur=mcur->next;
		}

		cur=cur->mesh->vnext;
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
