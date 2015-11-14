#include <lbxgl2.h>

void LBXGL_Voxel_DrawChunkFlat(LBXGL_VoxelChunk *chk)
{
	if(!chk->n_face)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

void LBXGL_Voxel_DrawChunkFlatRGB(LBXGL_VoxelChunk *chk)
{
	if(!chk->n_face)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glColorPointer(4, GL_FLOAT, 0, chk->face_rgba);
	glDrawArrays(GL_QUADS, 0, chk->n_face*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void LBXGL_Voxel_DrawChunkFinal(LBXGL_VoxelChunk *chk)
{
	int i, j, k;

	if(!chk->n_face)
		return;

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
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, chk->face_tex[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, chk->face_st);
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);

	for(i=0; i<chk->n_face; i++)
	{
		for(j=i; j<chk->n_face; j++)
			if(chk->face_tex[j]!=chk->face_tex[i])
				break;

		k=j-i;

		glBindTexture(GL_TEXTURE_2D, chk->face_tex[i]);
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
	int i, j, k;

	if(!chk->n_face)
		return;

#if 1
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, chk->face_tex[0]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, chk->face_xyz);
	glTexCoordPointer(2, GL_FLOAT, 0, chk->face_st);
	glNormalPointer(GL_FLOAT, 3*sizeof(float), chk->face_norm);
	glColorPointer(4, GL_FLOAT, 0, chk->face_rgba);

	for(i=0; i<chk->n_face; i++)
	{
		for(j=i; j<chk->n_face; j++)
			if(chk->face_tex[j]!=chk->face_tex[i])
				break;

		k=j-i;

		glBindTexture(GL_TEXTURE_2D, chk->face_tex[i]);
		glDrawArrays(GL_QUADS, i*4, k*4);

		i+=k-1;
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif
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

void LBXGL_Voxel_DrawWorldDark(LBXGL_BrushWorld *world)
{
//	LBXGL_Voxel_GenerateChunks(world);
	LBXGL_Voxel_RebuildRegionListVisible(world->vox_region);
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
}

void LBXGL_Voxel_DrawWorldLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
}

void LBXGL_Voxel_DrawWorldShadows(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
}

void LBXGL_Voxel_DrawWorldPointLight(LBXGL_BrushWorld *world)
{
//	LBXGL_Voxel_DrawRegionListFlatRGB(world->vox_region);
}

void LBXGL_Voxel_DrawWorldFinal(LBXGL_BrushWorld *world)
{
//	LBXGL_Voxel_DrawRegionListFinal(world->vox_region);
}

void LBXGL_Voxel_DrawWorldAlpha(LBXGL_BrushWorld *world)
{
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
	LBXGL_Voxel_DrawRegionListFinalRGB(world->vox_region);
}
