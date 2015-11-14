#include <lbxgl2.h>

void LBXGL_Voxel_BarfEmitChunk(
	LBXGL_BARF_RenderContext *bctx,
	LBXGL_VoxelChunk *chk)
{
	LBXGL_BARF_RenderArray *arr;
	int i, j, k, tn, fl;

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

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->face_tex[i]);
		arr=LBXGL_BARF_GetRenderArrayForTexture(bctx, tn);

		LBXGL_BARF_ArraysEmitVertices(arr,
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 3*4, (byte *)chk->mesh->face_xyz,
			2, GL_FLOAT, 2*4, (byte *)chk->mesh->face_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->face_norm,
			3, GL_BYTE, 3, (byte *)chk->mesh->face_norm_x,
			3, GL_BYTE, 3, (byte *)chk->mesh->face_norm_y,
			4, GL_UNSIGNED_BYTE, 4, (byte *)chk->mesh->face_rgba,
			4, GL_UNSIGNED_BYTE, 4, (byte *)chk->mesh->face_slrgba,
			4, GL_UNSIGNED_BYTE, 4, (byte *)chk->mesh->face_vlrgba,
			4, GL_UNSIGNED_BYTE, 4, (byte *)chk->mesh->face_tlrgba);

		i+=k-1;
	}
}

void LBXGL_Voxel_BarfEmitChunkAlpha(
	LBXGL_BARF_RenderContext *bctx,
	LBXGL_VoxelChunk *chk)
{
	LBXGL_BARF_RenderArray *arr;
	int i, j, k, tn, fl;

	if(!chk->mesh->n_aface)
		return;
	if(chk->flags&VOX_CHFL_ALPHAVISCLIP)
		return;

//	LBXGL_Shader_NormalX3f(0, 0, 0);
//	LBXGL_Shader_NormalY3f(0, 0, 0);

//	LBXGL_Shader_BindTexture(-1);

	for(i=0; i<chk->mesh->n_aface; i++)
	{
		for(j=i; j<chk->mesh->n_aface; j++)
			if(chk->mesh->aface_tex[j]!=chk->mesh->aface_tex[i])
				break;
		fl=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
//		if(!(fl&BTGE_TXFL_LIGHTMAT)) { i=j-1; continue; }

		k=j-i;

		tn=LBXGL_Voxel_MapTexture(chk, chk->mesh->aface_tex[i]);
		arr=LBXGL_BARF_GetRenderArrayForTexture(bctx, tn);

		LBXGL_BARF_ArraysEmitVertices(arr,
			PDGL_TRIANGLES, i*3, k*3,
			3, GL_FLOAT, 3*4, (byte *)chk->mesh->aface_xyz,
			2, GL_FLOAT, 2*4, (byte *)chk->mesh->aface_st,
			3, GL_BYTE, 3, (byte *)chk->mesh->aface_norm,
			3, GL_BYTE, 3, NULL,
			3, GL_BYTE, 3, NULL,
			4, GL_UNSIGNED_BYTE, 4, (byte *)chk->mesh->aface_rgba,
			4, GL_UNSIGNED_BYTE, 4, (byte *)chk->mesh->aface_slrgba,
			4, GL_UNSIGNED_BYTE, 4, (byte *)chk->mesh->aface_vlrgba,
			4, GL_UNSIGNED_BYTE, 4, NULL);

		i+=k-1;
	}
}


void LBXGL_Voxel_BarfEmitRegion(
	LBXGL_BARF_RenderContext *bctx,
	LBXGL_VoxelRegion *rgn)
{
	float mins[3], maxs[3];
	LBXGL_VoxelChunk *cur;
	LBXGL_Light *lcur;
	int i;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
		return;

	LBXGL_Voxel_DrawBeginTranslateForRegion(rgn);

	cur=rgn->vischk;
	while(cur)
	{
		LBXGL_Voxel_BarfEmitChunk(bctx, cur);
//		LBXGL_Voxel_BarfEmitChunkAlpha(bctx, cur);
		cur=cur->mesh->vnext;
	}

	cur=rgn->avischk;
	while(cur)
	{
//		LBXGL_Voxel_BarfEmitChunk(bctx, cur);
		LBXGL_Voxel_BarfEmitChunkAlpha(bctx, cur);
		cur=cur->mesh->avnext;
	}

	LBXGL_Voxel_DrawEndTranslateForRegion(rgn);
}

void LBXGL_Voxel_BarfEmitRegionList(
	LBXGL_BARF_RenderContext *bctx,
	LBXGL_VoxelRegion *lst)
{
	LBXGL_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		LBXGL_Voxel_BarfEmitRegion(bctx, cur);
		cur=cur->next;
	}
}

void LBXGL_Voxel_BarfEmitWorld(
	LBXGL_BARF_RenderContext *bctx,
	LBXGL_BrushWorld *world)
{
	LBXGL_Voxel_BeginTranslateForWorld(world);

	LBXGL_Voxel_BarfEmitRegionList(bctx, world->vox_region);

	LBXGL_Voxel_EndTranslateForWorld(world);
}

void LBXGL_Voxel_EmitWorldBarf(
	LBXGL_BrushWorld *world)
{
	if(!world->barf_voxel)
		world->barf_voxel=LBXGL_BARF_AllocRenderContext();

	LBXGL_BARF_ResetContextArrays(world->barf_voxel);
	LBXGL_Voxel_BarfEmitWorld(world->barf_voxel, world);
	LBXGL_BARF_UpdateContextArrayVBOs(world->barf_voxel);
}
