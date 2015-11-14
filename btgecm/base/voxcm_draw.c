#include <btgecm.h>

void BTGE_Voxel_DrawChunkFlat(BTGE_VoxelChunk *chk)
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

void BTGE_Voxel_DrawChunkFlatRGB(BTGE_VoxelChunk *chk)
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

void BTGE_Voxel_DrawChunkFinal(BTGE_VoxelChunk *chk)
{
	int i, j, k;

	if(!chk->n_face)
		return;

#if 0
	for(i=0; i<chk->n_face; i++)
	{
		BTGE_Shader_BindTexture(chk->face_tex[i]);
		BTGE_Shader_BeginPoly();
		for(j=0; j<4; j++)
		{
			BTGE_Shader_Normal3fv(chk->face_norm+(i*4+j)*3);
			BTGE_Shader_TexCoord2fv(chk->face_st+(i*4+j)*2);
			BTGE_Shader_Vertex3fv(chk->face_xyz+(i*4+j)*3);
		}
		BTGE_Shader_EndPoly();
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

void BTGE_Voxel_DrawChunkFinalRGB(BTGE_VoxelChunk *chk)
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

void BTGE_Voxel_DrawRegionFlat(BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *cur;

	if(!BTGE_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		BTGE_Voxel_DrawChunkFlat(cur);
		cur=cur->vnext;
	}
}

void BTGE_Voxel_DrawRegionFlatRGB(BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *cur;

	if(!BTGE_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		BTGE_Voxel_DrawChunkFlatRGB(cur);
		cur=cur->vnext;
	}
}

void BTGE_Voxel_DrawRegionFinal(BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *cur;

	if(!BTGE_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		BTGE_Voxel_DrawChunkFinal(cur);
		cur=cur->vnext;
	}
}

void BTGE_Voxel_DrawRegionFinalRGB(BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *cur;

	if(!BTGE_Voxel_CheckRegionFrustum(rgn))
		return;

	cur=rgn->vischk;
	while(cur)
	{
		BTGE_Voxel_DrawChunkFinalRGB(cur);
		cur=cur->vnext;
	}
}

void BTGE_Voxel_DrawRegionListFlat(BTGE_VoxelRegion *lst)
{
	BTGE_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		BTGE_Voxel_DrawRegionFlat(cur);
		cur=cur->next;
	}
}

void BTGE_Voxel_DrawRegionListFlatRGB(BTGE_VoxelRegion *lst)
{
	BTGE_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		BTGE_Voxel_DrawRegionFlatRGB(cur);
		cur=cur->next;
	}
}

void BTGE_Voxel_DrawRegionListFinal(BTGE_VoxelRegion *lst)
{
	BTGE_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		BTGE_Voxel_DrawRegionFinal(cur);
		cur=cur->next;
	}
}

void BTGE_Voxel_DrawRegionListFinalRGB(BTGE_VoxelRegion *lst)
{
	BTGE_VoxelRegion *cur;

	cur=lst;
	while(cur)
	{
		BTGE_Voxel_DrawRegionFinalRGB(cur);
		cur=cur->next;
	}
}

void BTGE_Voxel_DrawWorldDark(BTGE_BrushWorld *world)
{
	BTGE_Voxel_GenerateChunks(world);
	BTGE_Voxel_RebuildRegionListVisible(world->vox_region);
//	BTGE_Voxel_DrawRegionListFlat(world->vox_region);
}

void BTGE_Voxel_DrawWorldLight(BTGE_BrushWorld *world,
	BTGE_Light *light)
{
//	BTGE_Voxel_DrawRegionListFlat(world->vox_region);
}

void BTGE_Voxel_DrawWorldShadows(BTGE_BrushWorld *world,
	BTGE_Light *light)
{
//	BTGE_Voxel_DrawRegionListFlat(world->vox_region);
}

void BTGE_Voxel_DrawWorldPointLight(BTGE_BrushWorld *world)
{
//	BTGE_Voxel_DrawRegionListFlatRGB(world->vox_region);
}

void BTGE_Voxel_DrawWorldFinal(BTGE_BrushWorld *world)
{
//	BTGE_Voxel_DrawRegionListFinal(world->vox_region);
}

void BTGE_Voxel_DrawWorldAlpha(BTGE_BrushWorld *world)
{
//	BTGE_Voxel_DrawRegionListFlat(world->vox_region);
	BTGE_Voxel_DrawRegionListFinalRGB(world->vox_region);
}
