#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

extern int lbxgl_voxel_chkleft;
extern int lbxgl_voxel_chkstart;

extern int lbxgl_voxel_reforg[3];
extern bool lbxgl_voxel_disablevbo;
extern float lbxgl_voxel_drawdist;

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
	LBXGL_Light *lst, *lcur, *lnxt;
//	LBXGL_Light *lst2, *lcur2, *lprv2;
	float f;
	int i;

	cur=world->vox_region; lst=NULL;
	while(cur)
	{
		lst=LBXGL_Voxel_QueryLightsCamRegion(world, cur, lst);
		cur=cur->next;
	}

#if 0
	lcur=lst; i=0;
	while(lcur)
	{
		f=V3F_DIST(lcur->org, lbxgl_cam->org);
		lcur->tval=f*(lcur->val[0]/800);
		lcur=lcur->chain; i++;
	}
	
	lcur=lst; i=0; lst2=NULL;	
	while(lcur)
	{
		lnxt=lcur->chain;
		lcur2=lst2; lprv2=NULL; i=0;
		while(lcur2 && (lcur2->tval>=lcur->tval) && (i<64))
			{ lprv2=lcur2; lcur2=lcur2->chain; i++; }
		if(i>=64) { lcur=lnxt; continue; }
		if(lprv2)
		{
			lcur->chain=lprv2->chain; lprv2->chain=lcur;
			lcur=lnxt; continue;
		}else
		{
			lcur->chain=lst2; lst2=lcur;
			lcur=lnxt; continue;
		}
	}
	lst=lst2;
#endif

#if 0
	lcur=lst; i=0;
	while(lcur)
	{
		if(i>64) { lcur->chain=NULL; break; }
		lcur=lcur->chain; i++;
	}
#endif

#if 0
	lcur=lst; i=0;
	while(lcur)
	{
		if(i>32) { lcur->chain=NULL; break; }
		lcur=lcur->chain; i++;
	}
#endif

	if(!lst)
	{
		i=1;
	}

#if 1
	lcur=lst; i=0;
	while(lcur)
	{
		if(i>256)
		{
			lcur->chain=NULL; break;
		}
		lcur=lcur->chain; i++;
	}
#endif

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

void LBXGL_Voxel_ClearWorldPointLight(LBXGL_BrushWorld *world)
{
}

void LBXGL_Voxel_UpdateWorldPointLight(LBXGL_BrushWorld *world,
	LBXGL_Light *light)
{
//	LBXGL_Voxel_DrawRegionListFlat(world->vox_region);
}

