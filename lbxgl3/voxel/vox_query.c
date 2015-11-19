#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

extern int lbxgl_voxel_chkleft;
extern int lbxgl_voxel_chkstart;

extern int lbxgl_voxel_reforg[3];
extern bool lbxgl_voxel_disablevbo;
extern float lbxgl_voxel_drawdist;

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

	pdglDepthMask(GL_FALSE);

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
	pdglDisable(GL_CULL_FACE);
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
	pdglEnable(GL_CULL_FACE);
#endif

	pdglDepthMask(GL_TRUE);
#endif

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
	
	LBXGL_Voxel_EmitWorldBarf(world);
}
