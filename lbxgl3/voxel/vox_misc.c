#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

extern int lbxgl_voxel_chkleft;
extern int lbxgl_voxel_chkstart;

extern int lbxgl_voxel_reforg[3];
extern bool lbxgl_voxel_disablevbo;
extern float lbxgl_voxel_drawdist;

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
	return(LBXGL_Voxel_CheckChunkFrustumDist(chk, lbxgl_voxel_drawdist));
//	return(LBXGL_Voxel_CheckChunkFrustumDist(chk, 2048));
}

bool LBXGL_Voxel_CheckChunkFrustumDist(LBXGL_VoxelChunk *chk, float dist)
{
	float mins[3], maxs[3], org[3];
	float f, g;
	
	LBXGL_Voxel_BoxChunkRel(chk, mins, maxs);

//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 8192))
//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 4096))
//	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, 6144))
	if(!LBXGL_Brush_CheckBBoxFrustumDistance(mins, maxs, dist))
		return(false);
	
	return(true);
}

void LBXGL_Voxel_BoxChunkVoxelRel(LBXGL_VoxelChunk *chk,
	int x, int y, int z,
	float *mins, float *maxs)
{
	float vsz, vsz2;
	int i, j, k, l;

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;

	mins[0]=(x*vsz)-lbxgl_voxel_reforg[0];
	mins[1]=(y*vsz)-lbxgl_voxel_reforg[1];
	mins[2]=(z*vsz)-lbxgl_voxel_reforg[2];
	maxs[0]=mins[0]+vsz;
	maxs[1]=mins[1]+vsz;
	maxs[2]=mins[2]+vsz;
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
