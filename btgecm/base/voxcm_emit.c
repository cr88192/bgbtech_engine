#include <btgecm.h>

extern BTGE_VoxelInfo *btge_voxinfo[4096];
extern byte btge_voxel_opaque[4096];
extern int btge_voxel_use_flexform;

void BTGE_Voxel_EmitFace(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0,
	float *xyz1, float *st1,
	float *xyz2, float *st2,
	float *xyz3, float *st3,
	float *norm, float *rgba,
	int tex)
{
	int i;

	if(BTGE_Voxel_VoxelAlphaP(vox))
//	if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
	{
		if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
			chk->flags|=VOX_CHFL_ALPHALIGHTMAT;

		BTGE_Voxel_EmitAlphaFace(chk, vox,
			xyz0, st0,	xyz1, st1,
			xyz2, st2,	xyz3, st3,
			norm, rgba, tex);
		return;
	}

	BTGE_Voxel_EmitSolidFace(chk, vox,
		xyz0, st0,	xyz1, st1,
		xyz2, st2,	xyz3, st3,
		norm, rgba, tex);
}

void BTGE_Voxel_EmitSolidFace(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0,
	float *xyz1, float *st1,
	float *xyz2, float *st2,
	float *xyz3, float *st3,
	float *norm, float *rgba,
	int tex)
{
	float slrgba[4];
	float f;

//	f=(vox->slight&0xF0)/240.0;
	f=BTGE_Voxel_GetVoxelGamma(chk, vox);
	slrgba[0]=f*rgba[0];
	slrgba[1]=f*rgba[1];
	slrgba[2]=f*rgba[2];
	slrgba[3]=rgba[3];

	BTGE_Voxel_EmitSolidFace2(chk, vox,
		xyz0, st0, norm, rgba, slrgba,
		xyz1, st1, norm, rgba, slrgba,
		xyz2, st2, norm, rgba, slrgba,
		xyz3, st3, norm, rgba, slrgba,
		tex);
}

void BTGE_Voxel_EmitSolidFace2(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0, float *rgba0, float *slrgba0,
	float *xyz1, float *st1, float *norm1, float *rgba1, float *slrgba1,
	float *xyz2, float *st2, float *norm2, float *rgba2, float *slrgba2,
	float *xyz3, float *st3, float *norm3, float *rgba3, float *slrgba3,
	int tex)
{
	float tv[4];
	
	V3F_ZERO(tv);
	tv[3]=1;

	BTGE_Voxel_EmitSolidFace3(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0, tv,
		xyz1, st1, norm1, rgba1, slrgba1, tv,
		xyz2, st2, norm2, rgba2, slrgba2, tv,
		xyz3, st3, norm3, rgba3, slrgba3, tv,
		tex);
}

void V3F2SB_COPY(float *fv, sbyte *bv)
{
	int i;
	i=fv[0]*127; i=(i>=(-127))?((i<=127)?i:127):-127; bv[0]=i;
	i=fv[1]*127; i=(i>=(-127))?((i<=127)?i:127):-127; bv[1]=i;
	i=fv[2]*127; i=(i>=(-127))?((i<=127)?i:127):-127; bv[2]=i;
}

void V4F2UB_COPY(float *fv, sbyte *bv)
{
	int i;
	i=fv[0]*255; i=(i>=0)?((i<=255)?i:255):0; bv[0]=i;
	i=fv[1]*255; i=(i>=0)?((i<=255)?i:255):0; bv[1]=i;
	i=fv[2]*255; i=(i>=0)?((i<=255)?i:255):0; bv[2]=i;
	i=fv[3]*255; i=(i>=0)?((i<=255)?i:255):0; bv[3]=i;
}

void V3F2SB_COPY_NORMXY(float *fv, sbyte *bvx, sbyte *bvy)
{
	int i;
//	i=fv[0]*127; i=(i>=(-127))?((i<=127)?i:127):-127; bv[0]=i;
//	i=fv[1]*127; i=(i>=(-127))?((i<=127)?i:127):-127; bv[1]=i;
//	i=fv[2]*127; i=(i>=(-127))?((i<=127)?i:127):-127; bv[2]=i;
}

void V3SB_ZERO(sbyte *bv)
{
	int i;
	bv[0]=0;
	bv[1]=0;
	bv[2]=0;
}

void BTGE_Voxel_EmitSolidFace3(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0,
		float *rgba0, float *slrgba0, float *vlrgba0,
	float *xyz1, float *st1, float *norm1,
		float *rgba1, float *slrgba1, float *vlrgba1,
	float *xyz2, float *st2, float *norm2,
		float *rgba2, float *slrgba2, float *vlrgba2,
	float *xyz3, float *st3, float *norm3,
		float *rgba3, float *slrgba3, float *vlrgba3,
	int tex)
{
	int i;

	BTGE_Voxel_CheckExpandFace(chk);
	
//	i=chk->mesh->n_face++;

	i=chk->mesh->n_face;
//	chk->mesh->n_face+=2;
	chk->mesh->n_face+=(xyz2!=xyz3)?2:1;

	chk->mesh->face_tex[i+0]=tex;
	chk->mesh->face_vox[i+0]=*vox;
	
	V3F_COPY(xyz0, chk->mesh->face_xyz+(i+0)*3*3+0);
	V3F_COPY(xyz1, chk->mesh->face_xyz+(i+0)*3*3+3);
	V3F_COPY(xyz2, chk->mesh->face_xyz+(i+0)*3*3+6);

	V2F_COPY(st0, chk->mesh->face_st+(i+0)*3*2+0);
	V2F_COPY(st1, chk->mesh->face_st+(i+0)*3*2+2);
	V2F_COPY(st2, chk->mesh->face_st+(i+0)*3*2+4);

	V3F2SB_COPY(norm0, chk->mesh->face_norm+(i+0)*3*3+0);
	V3F2SB_COPY(norm1, chk->mesh->face_norm+(i+0)*3*3+3);
	V3F2SB_COPY(norm2, chk->mesh->face_norm+(i+0)*3*3+6);

#if 1
	V3SB_ZERO(chk->mesh->face_norm_x+(i+0)*3*3+0);
	V3SB_ZERO(chk->mesh->face_norm_x+(i+0)*3*3+3);
	V3SB_ZERO(chk->mesh->face_norm_x+(i+0)*3*3+6);

	V3SB_ZERO(chk->mesh->face_norm_y+(i+0)*3*3+0);
	V3SB_ZERO(chk->mesh->face_norm_y+(i+0)*3*3+3);
	V3SB_ZERO(chk->mesh->face_norm_y+(i+0)*3*3+6);
#endif

	V4F2UB_COPY(rgba0, chk->mesh->face_rgba+(i+0)*3*4+0);
	V4F2UB_COPY(rgba1, chk->mesh->face_rgba+(i+0)*3*4+4);
	V4F2UB_COPY(rgba2, chk->mesh->face_rgba+(i+0)*3*4+8);

	V4F2UB_COPY(slrgba0, chk->mesh->face_slrgba+(i+0)*3*4+0);
	V4F2UB_COPY(slrgba1, chk->mesh->face_slrgba+(i+0)*3*4+4);
	V4F2UB_COPY(slrgba2, chk->mesh->face_slrgba+(i+0)*3*4+8);

	V4F2UB_COPY(vlrgba0, chk->mesh->face_vlrgba+(i+0)*3*4+0);
	V4F2UB_COPY(vlrgba1, chk->mesh->face_vlrgba+(i+0)*3*4+4);
	V4F2UB_COPY(vlrgba2, chk->mesh->face_vlrgba+(i+0)*3*4+8);

	if(xyz2!=xyz3)
	{
		chk->mesh->face_tex[i+1]=tex;
		chk->mesh->face_vox[i+1]=*vox;

		V3F_COPY(xyz0, chk->mesh->face_xyz+(i+1)*3*3+0);
		V3F_COPY(xyz2, chk->mesh->face_xyz+(i+1)*3*3+3);
		V3F_COPY(xyz3, chk->mesh->face_xyz+(i+1)*3*3+6);

		V2F_COPY(st0, chk->mesh->face_st+(i+1)*3*2+0);
		V2F_COPY(st2, chk->mesh->face_st+(i+1)*3*2+2);
		V2F_COPY(st3, chk->mesh->face_st+(i+1)*3*2+4);

		V3F2SB_COPY(norm0, chk->mesh->face_norm+(i+1)*3*3+0);
		V3F2SB_COPY(norm2, chk->mesh->face_norm+(i+1)*3*3+3);
		V3F2SB_COPY(norm3, chk->mesh->face_norm+(i+1)*3*3+6);

		V4F2UB_COPY(rgba0, chk->mesh->face_rgba+(i+1)*3*4+0);
		V4F2UB_COPY(rgba2, chk->mesh->face_rgba+(i+1)*3*4+4);
		V4F2UB_COPY(rgba3, chk->mesh->face_rgba+(i+1)*3*4+8);

		V4F2UB_COPY(slrgba0, chk->mesh->face_slrgba+(i+1)*3*4+0);
		V4F2UB_COPY(slrgba2, chk->mesh->face_slrgba+(i+1)*3*4+4);
		V4F2UB_COPY(slrgba3, chk->mesh->face_slrgba+(i+1)*3*4+8);

		V4F2UB_COPY(vlrgba0, chk->mesh->face_vlrgba+(i+1)*3*4+0);
		V4F2UB_COPY(vlrgba2, chk->mesh->face_vlrgba+(i+1)*3*4+4);
		V4F2UB_COPY(vlrgba3, chk->mesh->face_vlrgba+(i+1)*3*4+8);
	}
}

void BTGE_Voxel_EmitAlphaFace(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0,
	float *xyz1, float *st1,
	float *xyz2, float *st2,
	float *xyz3, float *st3,
	float *norm, float *rgba,
	int tex)
{
	float slrgba[4];
	float f;
	int i;

//	f=(vox->slight&0xF0)/240.0;

	i=(vox->slight>>4)&15;
	f=pow(0.80, 15-i);

	slrgba[0]=f*rgba[0];
	slrgba[1]=f*rgba[1];
	slrgba[2]=f*rgba[2];
	slrgba[3]=rgba[3];

	BTGE_Voxel_EmitAlphaFace2(chk, vox,
		xyz0, st0, norm, rgba, slrgba,
		xyz1, st1, norm, rgba, slrgba,
		xyz2, st2, norm, rgba, slrgba,
		xyz3, st3, norm, rgba, slrgba,
		tex);
}

void BTGE_Voxel_EmitAlphaFace2(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0, float *rgba0, float *slrgba0,
	float *xyz1, float *st1, float *norm1, float *rgba1, float *slrgba1,
	float *xyz2, float *st2, float *norm2, float *rgba2, float *slrgba2,
	float *xyz3, float *st3, float *norm3, float *rgba3, float *slrgba3,
	int tex)
{
	float tv[4];
	
	V3F_ZERO(tv);
	tv[3]=1;

	BTGE_Voxel_EmitAlphaFace3(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0, tv,
		xyz1, st1, norm1, rgba1, slrgba1, tv,
		xyz2, st2, norm2, rgba2, slrgba2, tv,
		xyz3, st3, norm3, rgba3, slrgba3, tv,
		tex);
}

void BTGE_Voxel_EmitAlphaFace3(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0,
		float *rgba0, float *slrgba0, float *vlrgba0,
	float *xyz1, float *st1, float *norm1,
		float *rgba1, float *slrgba1, float *vlrgba1,
	float *xyz2, float *st2, float *norm2,
		float *rgba2, float *slrgba2, float *vlrgba2,
	float *xyz3, float *st3, float *norm3,
		float *rgba3, float *slrgba3, float *vlrgba3,
	int tex)
{
	int i;

	BTGE_Voxel_CheckExpandAlphaFace(chk);
	
	i=chk->mesh->n_aface;
//	chk->mesh->n_aface+=2;
	chk->mesh->n_aface+=(xyz2!=xyz3)?2:1;
	
	chk->mesh->aface_tex[i+0]=tex;
	chk->mesh->aface_vox[i+0]=*vox;
	
	V3F_COPY(xyz0, chk->mesh->aface_xyz+(i+0)*3*3+0);
	V3F_COPY(xyz1, chk->mesh->aface_xyz+(i+0)*3*3+3);
	V3F_COPY(xyz2, chk->mesh->aface_xyz+(i+0)*3*3+6);

	V2F_COPY(st0, chk->mesh->aface_st+(i+0)*3*2+0);
	V2F_COPY(st1, chk->mesh->aface_st+(i+0)*3*2+2);
	V2F_COPY(st2, chk->mesh->aface_st+(i+0)*3*2+4);

	V3F2SB_COPY(norm0, chk->mesh->aface_norm+(i+0)*3*3+0);
	V3F2SB_COPY(norm1, chk->mesh->aface_norm+(i+0)*3*3+3);
	V3F2SB_COPY(norm2, chk->mesh->aface_norm+(i+0)*3*3+6);

	V4F2UB_COPY(rgba0, chk->mesh->aface_rgba+(i+0)*3*4+0);
	V4F2UB_COPY(rgba1, chk->mesh->aface_rgba+(i+0)*3*4+4);
	V4F2UB_COPY(rgba2, chk->mesh->aface_rgba+(i+0)*3*4+8);

	V4F2UB_COPY(slrgba0, chk->mesh->aface_slrgba+(i+0)*3*4+0);
	V4F2UB_COPY(slrgba1, chk->mesh->aface_slrgba+(i+0)*3*4+4);
	V4F2UB_COPY(slrgba2, chk->mesh->aface_slrgba+(i+0)*3*4+8);

	V4F2UB_COPY(vlrgba0, chk->mesh->aface_vlrgba+(i+0)*3*4+0);
	V4F2UB_COPY(vlrgba1, chk->mesh->aface_vlrgba+(i+0)*3*4+4);
	V4F2UB_COPY(vlrgba2, chk->mesh->aface_vlrgba+(i+0)*3*4+8);

	if(xyz2!=xyz3)
//	if(1)
	{
		chk->mesh->aface_tex[i+1]=tex;
		chk->mesh->aface_vox[i+1]=*vox;

		V3F_COPY(xyz0, chk->mesh->aface_xyz+(i+1)*3*3+0);
		V3F_COPY(xyz2, chk->mesh->aface_xyz+(i+1)*3*3+3);
		V3F_COPY(xyz3, chk->mesh->aface_xyz+(i+1)*3*3+6);

		V2F_COPY(st0, chk->mesh->aface_st+(i+1)*3*2+0);
		V2F_COPY(st2, chk->mesh->aface_st+(i+1)*3*2+2);
		V2F_COPY(st3, chk->mesh->aface_st+(i+1)*3*2+4);

		V3F2SB_COPY(norm0, chk->mesh->aface_norm+(i+1)*3*3+0);
		V3F2SB_COPY(norm2, chk->mesh->aface_norm+(i+1)*3*3+3);
		V3F2SB_COPY(norm3, chk->mesh->aface_norm+(i+1)*3*3+6);

		V4F2UB_COPY(rgba0, chk->mesh->aface_rgba+(i+1)*3*4+0);
		V4F2UB_COPY(rgba2, chk->mesh->aface_rgba+(i+1)*3*4+4);
		V4F2UB_COPY(rgba3, chk->mesh->aface_rgba+(i+1)*3*4+8);

		V4F2UB_COPY(slrgba0, chk->mesh->aface_slrgba+(i+1)*3*4+0);
		V4F2UB_COPY(slrgba2, chk->mesh->aface_slrgba+(i+1)*3*4+4);
		V4F2UB_COPY(slrgba3, chk->mesh->aface_slrgba+(i+1)*3*4+8);

		V4F2UB_COPY(vlrgba0, chk->mesh->aface_vlrgba+(i+1)*3*4+0);
		V4F2UB_COPY(vlrgba2, chk->mesh->aface_vlrgba+(i+1)*3*4+4);
		V4F2UB_COPY(vlrgba3, chk->mesh->aface_vlrgba+(i+1)*3*4+8);
	}
}

void BTGE_Voxel_EmitFace2(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0, float *rgba0, float *slrgba0,
	float *xyz1, float *st1, float *norm1, float *rgba1, float *slrgba1,
	float *xyz2, float *st2, float *norm2, float *rgba2, float *slrgba2,
	float *xyz3, float *st3, float *norm3, float *rgba3, float *slrgba3,
	int tex)
{
	int i;

	if(BTGE_Voxel_VoxelAlphaP(vox))
//	if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
	{
		if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
			chk->flags|=VOX_CHFL_ALPHALIGHTMAT;

		BTGE_Voxel_EmitAlphaFace2(chk, vox,
			xyz0, st0, norm0, rgba0, slrgba0,
			xyz1, st1, norm1, rgba1, slrgba1,
			xyz2, st2, norm2, rgba2, slrgba2,
			xyz3, st3, norm3, rgba3, slrgba3,
			tex);
		return;
	}

	BTGE_Voxel_EmitSolidFace2(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0,
		xyz1, st1, norm1, rgba1, slrgba1,
		xyz2, st2, norm2, rgba2, slrgba2,
		xyz3, st3, norm3, rgba3, slrgba3,
		tex);
}

void BTGE_Voxel_EmitFace3(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0,
		float *rgba0, float *slrgba0, float *vlrgba0,
	float *xyz1, float *st1, float *norm1,
		float *rgba1, float *slrgba1, float *vlrgba1,
	float *xyz2, float *st2, float *norm2,
		float *rgba2, float *slrgba2, float *vlrgba2,
	float *xyz3, float *st3, float *norm3,
		float *rgba3, float *slrgba3, float *vlrgba3,
	int tex)
{
	int i;

	if(BTGE_Voxel_VoxelAlphaP(vox))
//	if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
	{
		if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
			chk->flags|=VOX_CHFL_ALPHALIGHTMAT;

		BTGE_Voxel_EmitAlphaFace3(chk, vox,
			xyz0, st0, norm0, rgba0, slrgba0, vlrgba0,
			xyz1, st1, norm1, rgba1, slrgba1, vlrgba1,
			xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
			xyz3, st3, norm3, rgba3, slrgba3, vlrgba3,
			tex);
		return;
	}

	BTGE_Voxel_EmitSolidFace3(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0, vlrgba0,
		xyz1, st1, norm1, rgba1, slrgba1, vlrgba1,
		xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
		xyz3, st3, norm3, rgba3, slrgba3, vlrgba3,
		tex);
}

void BTGE_Voxel_EmitTriangle3(
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *xyz0, float *st0, float *norm0,
		float *rgba0, float *slrgba0, float *vlrgba0,
	float *xyz1, float *st1, float *norm1,
		float *rgba1, float *slrgba1, float *vlrgba1,
	float *xyz2, float *st2, float *norm2,
		float *rgba2, float *slrgba2, float *vlrgba2,
	int tex)
{
	int i;

	if(BTGE_Voxel_VoxelAlphaP(vox))
//	if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
	{
		if(BTGE_Voxel_VoxelAlphaNonMatP(vox))
			chk->flags|=VOX_CHFL_ALPHALIGHTMAT;

		BTGE_Voxel_EmitAlphaFace3(chk, vox,
			xyz0, st0, norm0, rgba0, slrgba0, vlrgba0,
			xyz1, st1, norm1, rgba1, slrgba1, vlrgba1,
			xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
			xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
			tex);
		return;
	}

	BTGE_Voxel_EmitSolidFace3(chk, vox,
		xyz0, st0, norm0, rgba0, slrgba0, vlrgba0,
		xyz1, st1, norm1, rgba1, slrgba1, vlrgba1,
		xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
		xyz2, st2, norm2, rgba2, slrgba2, vlrgba2,
		tex);
}

BTGE_API void BTGE_Voxel_EmitBlockFluid(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	BTGE_VoxelData *vox1;

	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};
	static float nva[4]= { 0, 0, 1, 0};
	float vtxa[8*3];
	float rgba[4];
	int tex;
	float *pt1, *pt2, *pt3, *pt4;
	float f;
	int i, j;

	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z+1);
	if(vox1 && vox1->type)
		return;

	if(!btge_voxinfo[(vox->type)&4095])
		return;

	tex=btge_voxinfo[(vox->type)&4095]->img_top;

	f=(vox->slight&0xF0)/240.0;
	
	rgba[0]=1;
	rgba[1]=1;
	rgba[2]=1;
	rgba[3]=1;

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	pt1=vtxa+(4*3);
	pt2=vtxa+(6*3);
	pt3=vtxa+(7*3);
	pt4=vtxa+(5*3);
		
	BTGE_Voxel_EmitFace(chk, vox,
		pt4, sta+6, pt3, sta+4,
		pt2, sta+2, pt1, sta+0,
		nva, rgba, tex);
}


BTGE_API void BTGE_Voxel_ProjectFacePointCoords(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelData *vox, int x, int y, int z,
	float *org, float *nv, float *st)
{
	BTGE_VoxelInfo *inf;
	float f, g;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk, vox, x, y, z);
	if(!inf)return;

	f=inf->img_scale[0]*(1.0/32);

	if(fabs(nv[0])>fabs(nv[1]))
	{
		if(fabs(nv[0])>fabs(nv[2]))
		{
			st[0]=org[1]*f;
			st[1]=org[2]*f;
		}else
		{
			st[0]=org[0]*f;
			st[1]=org[1]*f;
		}
	}else
	{
		if(fabs(nv[1])>fabs(nv[2]))
		{
			st[0]=org[0]*f;
			st[1]=org[2]*f;
		}else
		{
			st[0]=org[0]*f;
			st[1]=org[1]*f;
		}
	}
}

BTGE_API void BTGE_Voxel_EmitBlock_FluidFlow(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	float stt[4*2];
	int tex[3];
	BTGE_VoxelData *vox1;
	float *pt1, *pt2, *pt3, *pt4;
	float f, g, h, g0, g1, g2, g3;
	float h0, h1, h2, h3;
	int i, j, ty;
	
	//skip air
	if(!vox->type)
		return;

	if(!btge_voxinfo[(vox->type)&4095])
		return;

	//check bad block type
//	ty=vox->type|((vox->type2&15)<<8);
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

	tex[0]=btge_voxinfo[ty]->img_top;
	tex[1]=btge_voxinfo[ty]->img_side;
	tex[2]=btge_voxinfo[ty]->img_bot;

	f=(vox->slight&0xF0)/240.0;
	g=1;
	h=(vox->aux&0x0F)/7.0;
	
	if(btge_voxinfo[ty]->flags&BTGE_TXFL_FLUID)
	{
		g=0.75;
		
//		if((vox->type==VOX_TY_WATER2) || (vox->type==VOX_TY_LAVA2))
//		{
//			max[2]=min[2]+(max[2]-min[2])*h;
//		}
	}

#if 1
//	g0=1; g1=1; g2=1; g3=1;
	g0=0.875; g1=0.875; g2=0.875; g3=0.875;
//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x-1, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x-1, y, z);
	if(vox1 && (vox1->type==vox->type))
		g0=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g0=0;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x+1, y, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x+1, y, z);
	if(vox1 && (vox1->type==vox->type))
		g1=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g1=0;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y-1, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y-1, z);
	if(vox1 && (vox1->type==vox->type))
		g2=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g2=0;

//	vox1=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y+1, z);
	vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y+1, z);
	if(vox1 && (vox1->type==vox->type))
		g3=(vox1->aux&0x0F)/7.0;
	if(!vox1 || !vox1->type)
		g3=0;

	h0=(g0+g2)/2;
	h1=(g1+g2)/2;
	h2=(g0+g3)/2;
	h3=(g1+g3)/2;
	h0=(h0+h)/2;
	h1=(h1+h)/2;
	h2=(h2+h)/2;
	h3=(h3+h)/2;
#endif

//	f=(flag&0xF0)/240.0;
//	f=f*f;
//	f=0.05;
	
	f=1;
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	if(flag&32)
	{
		vtxa[4*3+2]=min[2]+(32*h0);
		vtxa[5*3+2]=min[2]+(32*h1);
		vtxa[6*3+2]=min[2]+(32*h2);
		vtxa[7*3+2]=min[2]+(32*h3);
	}

//	if((vox->type==VOX_TY_GRASS) && (flag&32))
	if(0)
//	if(((vox->type==VOX_TY_GRASS)||(vox->type==VOX_TY_DIRT)) && (flag&32))
//	if(flag&32)
	{
#if 0
		if((!(flag&1) && !(flag&64)) && (!(flag&4) && !(flag&256)))
			vtxa[4*3+2]+=32;
		if((!(flag&2) && !(flag&128)) && (!(flag&4) && !(flag&256)))
			vtxa[5*3+2]+=32;
		if((!(flag&1) && !(flag&64)) && (!(flag&8) && !(flag&512)))
			vtxa[6*3+2]+=32;
		if((!(flag&2) && !(flag&128)) && (!(flag&8) && !(flag&512)))
			vtxa[7*3+2]+=32;
#endif

#if 0
		if((flag&VOX_FL_NX) && (flag&VOX_FL_NY))
			vtxa[4*3+2]-=32;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_NY))
			vtxa[5*3+2]-=32;
		if((flag&VOX_FL_NX) && (flag&VOX_FL_PY))
			vtxa[6*3+2]-=32;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_PY))
			vtxa[7*3+2]-=32;
#endif

#if 1
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_NY)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXNY)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_NXNY)))
				{ vtxa[4*3+2]-=32*(1-h); vtxa[0*3+2]-=32*(1-h); }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_NY)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXNY)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_PXNY)))
				{ vtxa[5*3+2]-=32*(1-h); vtxa[1*3+2]-=32*(1-h); }
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_PY)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXPY)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_NXPY)))
				{ vtxa[6*3+2]-=32*(1-h); vtxa[2*3+2]-=32*(1-h); }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_PY)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXPY)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_PXPY)))
				{ vtxa[7*3+2]-=32*(1-h); vtxa[3*3+2]-=32*(1-h); }
#endif

//		if((flag&15)!=0)
//			flag|=15;

//		flag|=15;
	}

//	if((flag&15)!=0)
//		flag|=15;

	for(i=0; i<6; i++)
	{
//		if(flag&(65536<<i))
//			continue;

		if(!(flag&(1<<i)))
		{
			if((i==0) && (g0==1.0))continue;
			if((i==1) && (g1==1.0))continue;
			if((i==2) && (g2==1.0))continue;
			if((i==3) && (g3==1.0))continue;
			if((i==4)||(i==5))continue;
//			continue;
		}

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
		BTGE_Voxel_ProjectFacePointCoords(
			world, chk, vox, x, y, z,
			pt1, nva+(i*4), stt+0);
		BTGE_Voxel_ProjectFacePointCoords(
			world, chk, vox, x, y, z,
			pt2, nva+(i*4), stt+2);
		BTGE_Voxel_ProjectFacePointCoords(
			world, chk, vox, x, y, z,
			pt3, nva+(i*4), stt+4);
		BTGE_Voxel_ProjectFacePointCoords(
			world, chk, vox, x, y, z,
			pt4, nva+(i*4), stt+6);

		BTGE_Voxel_EmitFace(chk, vox,
			pt4, stt+6,
			pt3, stt+4,
			pt2, stt+2,
			pt1, stt+0,
			nva+(i*4), rgba+(i*4),
			tex[vtxt[i]]);
		
//		BTGE_Voxel_EmitFace(chk, vox,
//			pt1, sta+0,
//			pt2, sta+2,
//			pt3, sta+4,
//			pt4, sta+6,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

//		BTGE_Voxel_EmitFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);
	}
}


BTGE_API void BTGE_Voxel_EmitMaterialCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag)
{
	BTGE_Voxel_EmitMaterialColorCube(world,
		chk, vox, min, max, flag, 1, 1, 1);
}

BTGE_API void BTGE_Voxel_EmitMaterialColorCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	float cr, float cg, float cb)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4];
	int tex[3];
	float *pt1, *pt2, *pt3, *pt4, *pnv, *prgba;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	//check bad block type
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

	tex[0]=btge_voxinfo[ty]->img_top;
	tex[1]=btge_voxinfo[ty]->img_side;
	tex[2]=btge_voxinfo[ty]->img_bot;

//	f=(vox->slight&0xF0)/240.0;
	f=1;
	g=1;
	
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=1;

		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
//		BTGE_Voxel_EmitSolidFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		pnv=nva+(i*4);
//		prgba=rgba+(i*4);
		BTGE_Voxel_EmitSolidFace2(chk, vox,
			pt4, sta+6, pnv, brgba, slrgba,
			pt3, sta+4, pnv, brgba, slrgba,
			pt2, sta+2, pnv, brgba, slrgba,
			pt1, sta+0, pnv, brgba, slrgba,
			tex[vtxt[i]]);
	}
}

BTGE_API void BTGE_Voxel_EmitMaterialAlphaCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	float alpha, float gamma)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4];
	int tex[3];
	float *pt1, *pt2, *pt3, *pt4, *pnv, *prgba;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	//check bad block type
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

	tex[0]=btge_voxinfo[ty]->img_top;
	tex[1]=btge_voxinfo[ty]->img_side;
	tex[2]=btge_voxinfo[ty]->img_bot;

//	f=(vox->slight&0xF0)/240.0;
	f=1;
	g=1;
	
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		brgba[0]=gamma;
		brgba[1]=gamma;
		brgba[2]=gamma;
		brgba[3]=alpha;

//		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		f=1;
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
//		BTGE_Voxel_EmitSolidFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		pnv=nva+(i*4);
//		prgba=rgba+(i*4);
//		BTGE_Voxel_EmitSolidFace2(chk, vox,
		BTGE_Voxel_EmitAlphaFace2(chk, vox,
			pt4, sta+6, pnv, brgba, slrgba,
			pt3, sta+4, pnv, brgba, slrgba,
			pt2, sta+2, pnv, brgba, slrgba,
			pt1, sta+0, pnv, brgba, slrgba,
			tex[vtxt[i]]);
	}
}

BTGE_API void BTGE_Voxel_EmitMaterialTextureCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag, int tex)
{
	BTGE_Voxel_EmitMaterialTextureColorCube(world,
		chk, vox, min, max, flag, tex, 1, 1, 1);
}

BTGE_API void BTGE_Voxel_EmitMaterialTextureColorCube(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag, int tex,
	float cr, float cg, float cb)
{
	static int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
		1, 3, 2, 0,	//-Z
		4, 6, 7, 5,	//+Z
	};
	static int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};

	static float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float vtxa[8*3];
	float rgba[6*4];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4];
	float *pt1, *pt2, *pt3, *pt4, *pnv, *prgba;
	float f, g, h;
	int ty, dk, fl2;
	int i, j;
	
	//check bad block type
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->img_top<=0)
		return;

//	f=(vox->slight&0xF0)/240.0;
	f=1;
	g=1;
	
	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=f*gam[i];
		rgba[i*4+1]=f*gam[i];
		rgba[i*4+2]=f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=1;

		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		slrgba[0]=f*brgba[0]*gam[i];
		slrgba[1]=f*brgba[1]*gam[i];
		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		pnv=nva+(i*4);
//		prgba=rgba+(i*4);
		BTGE_Voxel_EmitSolidFace2(chk, vox,
			pt4, sta+6, pnv, brgba, slrgba,
			pt3, sta+4, pnv, brgba, slrgba,
			pt2, sta+2, pnv, brgba, slrgba,
			pt1, sta+0, pnv, brgba, slrgba,
			tex);
	}
}

BTGE_API void BTGE_Voxel_EmitBlock_Fence(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	float mid[3], min2[3], max2[3];
	float h;
	
	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);

	min2[0]=mid[0]-4; min2[1]=mid[1]-4; min2[2]=min[2];
	max2[0]=mid[0]+4; max2[1]=mid[1]+4; max2[2]=max[2]+8;
	BTGE_Voxel_EmitMaterialCube(world, chk, vox,
		min2, max2, VOX_FL_ALL_CUBE);
	
	h=(mid[2]+max[2])*0.5;
	
	if(!(flag&VOX_FL_NX))
	{
		min2[0]=min[0];		min2[1]=mid[1]-2;	min2[2]=h-4;
		max2[0]=mid[0];		max2[1]=mid[1]+2;	max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}

	if(!(flag&VOX_FL_PX))
	{
		min2[0]=mid[0];		min2[1]=mid[1]-2;	min2[2]=h-4;
		max2[0]=max[0];		max2[1]=mid[1]+2;	max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}

	if(!(flag&VOX_FL_NY))
	{
		min2[0]=mid[0]-2;	min2[1]=min[1];		min2[2]=h-4;
		max2[0]=mid[0]+2;	max2[1]=mid[1];		max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}

	if(!(flag&VOX_FL_PY))
	{
		min2[0]=mid[0]-2;	min2[1]=mid[1];		min2[2]=h-4;
		max2[0]=mid[0]+2;	max2[1]=max[1];		max2[2]=h+4;
		BTGE_Voxel_EmitMaterialCube(world, chk, vox,
			min2, max2, VOX_FL_ALL_CUBE);
	}
}

BTGE_API void BTGE_Voxel_EmitBlock(
	BTGE_BrushWorld *world, 
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	float *min, float *max, int flag,
	int x, int y, int z)
{
	static const int vtxb[6*4]=
	{
		6, 4, 0, 2, //-X
		5, 7, 3, 1, //+X
		4, 5, 1, 0, //-Y
		7, 6, 2, 3, //+Y
//		1, 3, 2, 0,	//-Z
		3, 2, 0, 1,	//-Z
//		4, 6, 7, 5,	//+Z
		6, 7, 5, 4, //+Z
	};
	static const int vtxt[6]= {1, 1, 1, 1, 2, 0};
	static const float gam[6]= {0.75, 0.75, 0.75, 0.75, 0.5, 1};
//	static float sta[8]= { 0, 0, 1, 0, 1, 1, 0, 1};
	static const float sta[8]= { 0, 1, 1, 1, 1, 0, 0, 0};
	static const int sti[6*2]= { 1, 2, 1, 2, 0, 2, 0, 2, 0, 1, 0, 1};

	static const float nva[6*4]=
	{
		-1, 0, 0, 0,
		 1, 0, 0, 0,
		 0,-1, 0, 0,
		 0, 1, 0, 0,
		 0, 0,-1, 0,
		 0, 0, 1, 0,
	};

	float stb[8];
	float vtxa[8*3];
	float rgba[6*4];
	float vgam[6];
	float vlgam[6*3], slgam[6*3];
	float tv0[4], tv1[4], tv2[4], tv3[4];
	float nv[4], brgba[4], slrgba[4], vlrgba[4], mid[4];
	int tex[3], pos[3];
	BTGE_VoxelBiomeInfo *biome;
	BTGE_VoxelInfo *inf;
	BTGE_VoxelData *vox1;
	float *pt1, *pt2, *pt3, *pt4;
	float cr, cg, cb, ca;
	float f, g, h;
	int ty, dk, fl2;
	int i, j, k;
	
	//skip air
	if(!vox->type)
		return;
	if(!(vox->type&VOX_TY_MASK))
		return;

	if(!btge_voxinfo[(vox->type)&4095])
		return;

//	ty=vox->type|((vox->type2&15)<<8);
	ty=vox->type&4095;
	if(btge_voxinfo[ty]->EmitBlock)
	{
		btge_voxinfo[ty]->EmitBlock(
			world, chk, vox,
			min, max, flag, x, y, z);
		return;
	}
	
	//check bad block type
	if(btge_voxinfo[ty]->img_top<=0)
		return;

#if 0
	if(btge_voxinfo[ty]->flags&BTGE_TXFL_FLUID)
	{
		BTGE_Voxel_EmitBlockFluid(world, chk, vox, min, max, flag, x, y, z);
		return;
	}
#endif

	if((ty==VOX_TY_WATER2) || (ty==VOX_TY_LAVA2) ||
		(ty==VOX_TY_SLIME2) || (ty==VOX_TY_GLOWSLIME2))
	{
		BTGE_Voxel_EmitBlock_FluidFlow(
			world, chk, vox, min, max, flag, x, y, z);
		return;
	}

	if((ty==VOX_TY_FENCE) || (ty==VOX_TY_NETHERBRICK_FENCE))
	{
		BTGE_Voxel_EmitBlock_Fence(
			world, chk, vox, min, max, flag, x, y, z);
		return;
	}

	biome=BTGE_Voxel_FetchBiomeForID(world, chk->biome);

	inf=btge_voxinfo[ty];
	if(inf->flags2&BTGE_TXFL2_AUXVARIANT)
	{
		i=inf->img_auxvar_top[vox->aux&7];
//		if(i>0)
		if((i>0) && (vox->aux&7))
		{
			tex[0]=inf->img_auxvar_top[vox->aux&7];
			tex[1]=inf->img_auxvar_side[vox->aux&7];
			tex[2]=inf->img_auxvar_bot[vox->aux&7];
		}else
		{
			tex[0]=inf->img_top;
			tex[1]=inf->img_side;
			tex[2]=inf->img_bot;
		}
	}else
	{
		tex[0]=inf->img_top;
		tex[1]=inf->img_side;
		tex[2]=inf->img_bot;
	}

	if(inf->flags2&BTGE_TXFL2_EMITSSOUND)
	{
		chk->flags|=VOX_CHFL_HASSOUNDBLOCK;
	}

	pos[0]=x; pos[1]=y; pos[2]=z;

	f=BTGE_Voxel_GetVoxelGamma(chk, vox);
	for(i=0; i<6; i++)
	{
		vox1=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn,
			x+nva[i*4+0], y+nva[i*4+1], z+nva[i*4+2]);
		f=BTGE_Voxel_GetVoxelGamma(chk, vox1);

		vgam[i]=f;
		
		if(vox1)
		{
//			j=vox1->vlight;
			j=BTGE_Voxel_GetVoxelTransmitVLight(world, vox1);
//			j=(15<<4)|1;
			BTGE_Voxel_GetVLightColor(world, j, vlgam+i*3);

			j=BTGE_Voxel_GetVoxelTransmitSLight(world, vox1);
			BTGE_Voxel_GetSLightColor(world, j, slgam+i*3);
		}else
		{
			V3F_ZERO(vlgam+i*3);
//			vlgam[i*3+0]=1;

			slgam[i*3+0]=vgam[i];
			slgam[i*3+1]=vgam[i];
			slgam[i*3+2]=vgam[i];
		}
		
//		vlgam[i*3+0]=1;
	}

//	f=(vox->slight&0xF0)/240.0;
	f=1;
	g=1;
	
	if(btge_voxinfo[ty]->flags&BTGE_TXFL_FLUID)
	{
		g=0.75;
		
		if((ty==VOX_TY_WATER2) || (ty==VOX_TY_LAVA2) ||
			(ty==VOX_TY_SLIME2) || (ty==VOX_TY_GLOWSLIME2))
		{
			h=(vox->aux&0x0F)/7.0;
//			max[2]=min[2]+(max[2]-min[2])*h;
		}

		if((ty==VOX_TY_WATER) || (ty==VOX_TY_LAVA) ||
			(ty==VOX_TY_SLIME) || (ty==VOX_TY_GLOWSLIME))
		{
			h=7.0/8.0;
//			max[2]=min[2]+(max[2]-min[2])*h;
		}
	}

//	f=(flag&0xF0)/240.0;
//	f=f*f;
//	f=0.05;

	ca=g;

	switch(ty&VOX_TY_MASK)
	{
	case VOX_TY_CLOTH:
//	case VOX_TY_GLASS:
	case VOX_TY_REDSTONE_LAMP0:
	case VOX_TY_REDSTONE_LAMP1:
		cr=(vox->aux&1)?0.5:1;
		cg=(vox->aux&2)?0.5:1;
		cb=(vox->aux&4)?0.5:1;
		if(vox->aux&8)
			{ cr*=0.25; cg*=0.25; cb*=0.25; }
		break;
	case VOX_TY_GLASS:
		cr=(vox->aux&1)?1:0.25;
		cg=(vox->aux&2)?1:0.25;
		cb=(vox->aux&4)?1:0.25;
		if(vox->aux&8)
			{ cr*=0.25; cg*=0.25; cb*=0.25; }
		break;
	case VOX_TY_GRASS:
	case VOX_TY_LEAVES:
		if(biome)
		{
			cr=biome->clr_grass[0];
			cg=biome->clr_grass[1];
			cb=biome->clr_grass[2];
		}else
		{
			cr=255/255.0;
			cg=255/255.0;
			cb=192/255.0;
		}
		break;
	default: cr=1; cg=1; cb=1; break;
	}

#if 1
	V3F_SCALEADDSCALE(min, 0.5, max, 0.5, mid);
	inf=btge_voxinfo[ty];
	if(inf->vlight)
	{
		i=inf->vlight|0xF0;
		j=(inf->vlight>>4)&15;

		k=1;

		switch(ty)
		{
		case VOX_TY_REDSTONE_LAMP0:
		case VOX_TY_REDSTONE_LAMP1:
			rgba[0]=cr; rgba[1]=cg; rgba[2]=cb;
			if(	((vox->type&VOX_TYM_MASK)==VOX_TYM_NORMAL) ||
				((vox->type&VOX_TYM_MASK)==VOX_TYM_UNBREAKABLE) ||
				((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM))
					k=0;
			break;
		default:
//			BTGE_Voxel_GetVLightColor(world, i, rgba);
			BTGE_Voxel_GetVLightBaseColor(world, i, rgba);
			break;
		}

		//at least one face should be visible for this to emit light
		if((flag&VOX_FL_ALL_CUBE) && k)
		{
			if(btge_voxinfo[ty]->flags&BTGE_TXFL_FLUID)
			{
				BTGE_Voxel_EmitLightStyle(world, chk,
					mid[0], mid[1], mid[2],
					rgba[0], rgba[1], rgba[2], j*32,
					BTGE_LFL_NOSHADOW, inf->vlight_style);
			}else
			{
				BTGE_Voxel_EmitLightStyle(world, chk,
					mid[0], mid[1], mid[2],
					rgba[0], rgba[1], rgba[2], j*32,
					0, inf->vlight_style);
//					BTGE_LFL_NOSHADOW);
			}
		}
	}
#endif

#if 1
	if((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM)
	{
		if(BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
		{
			i=0;
			
			j=VOX_FL_NX|VOX_FL_PX|VOX_FL_NY|VOX_FL_PY;
			if((flag&j)==j)i=1;
		
			if(!i)return;
		}

		return;
	}
#endif

	if(btge_voxinfo[ty]->flags&BTGE_TXFL_ALPHA)
		f=2;

	for(i=0; i<6; i++)
	{
		rgba[i*4+0]=cr*f*gam[i];
		rgba[i*4+1]=cg*f*gam[i];
		rgba[i*4+2]=cb*f*gam[i];
		rgba[i*4+3]=g;
	}

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=(i&1)?max[0]:min[0];
		vtxa[i*3+1]=(i&2)?max[1]:min[1];
		vtxa[i*3+2]=(i&4)?max[2]:min[2];
	}

	if(ty==VOX_TY_SLAB)
	{
		vtxa[4*3+2]-=16;
		vtxa[5*3+2]-=16;
		vtxa[6*3+2]-=16;
		vtxa[7*3+2]-=16;
	}

//	if((vox->type==VOX_TY_GRASS) && (flag&32))
//	if(0)
//	if(((vox->type==VOX_TY_GRASS)||(vox->type==VOX_TY_DIRT)) && (flag&32))
//	if(flag&32)
	if(((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM) &&
		(flag&VOX_FL_PZ) && btge_voxel_use_flexform)
	{
		dk=32;
//		dk=16;
		fl2=0;
	
#if 1
		if((!(flag&VOX_FL_NX) && !(flag&VOX_FL_NXPZ)) &&
			(!(flag&VOX_FL_NY) && !(flag&VOX_FL_NYPZ)) &&
			(!(flag&VOX_FL_NXNY) && !(flag&VOX_FL_NXNYPZ)))
				{ fl2|=1; }
		if((!(flag&VOX_FL_PX) && !(flag&VOX_FL_PXPZ)) &&
			(!(flag&VOX_FL_NY) && !(flag&VOX_FL_NYPZ)) &&
			(!(flag&VOX_FL_PXNY) && !(flag&VOX_FL_PXNYPZ)))
				{ fl2|=2; }
		if((!(flag&VOX_FL_NX) && !(flag&VOX_FL_NXPZ)) &&
			(!(flag&VOX_FL_PY) && !(flag&VOX_FL_PYPZ)) &&
			(!(flag&VOX_FL_NXPY) && !(flag&VOX_FL_NXPYPZ)))
				{ fl2|=4; }
		if((!(flag&VOX_FL_PX) && !(flag&VOX_FL_PXPZ)) &&
			(!(flag&VOX_FL_PY) && !(flag&VOX_FL_PYPZ)) &&
			(!(flag&VOX_FL_PXPY) && !(flag&VOX_FL_PXPYPZ)))
				{ fl2|=8; }
#endif

#if 0
		if((flag&VOX_FL_NX) && (flag&VOX_FL_NY))
			vtxa[4*3+2]-=dk;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_NY))
			vtxa[5*3+2]-=dk;
		if((flag&VOX_FL_NX) && (flag&VOX_FL_PY))
			vtxa[6*3+2]-=dk;
		if((flag&VOX_FL_PX) && (flag&VOX_FL_PY))
			vtxa[7*3+2]-=dk;
#endif

#if 1
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_NY) &&
				(flag&VOX_FL_NXNY) && (flag&VOX_FL_NXNYPZ)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXNY) &&
				(flag&VOX_FL_NYPZ)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_NXNY) &&
				(flag&VOX_FL_NXPZ)))
			{ fl2|=16; }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_NY) &&
				(flag&VOX_FL_PXNY) && (flag&VOX_FL_PXNYPZ)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXNY) &&
				(flag&VOX_FL_NYPZ)) ||
			((flag&VOX_FL_NY) && (flag&VOX_FL_PXNY) &&
				(flag&VOX_FL_PXPZ)))
			{ fl2|=32; }
		if(((flag&VOX_FL_NX) && (flag&VOX_FL_PY) &&
				(flag&VOX_FL_NXPY) && (flag&VOX_FL_NXPYPZ)) ||
			((flag&VOX_FL_NX) && (flag&VOX_FL_NXPY) &&
				(flag&VOX_FL_PYPZ)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_NXPY) &&
				(flag&VOX_FL_NXPZ)))
			{ fl2|=64; }
		if(((flag&VOX_FL_PX) && (flag&VOX_FL_PY) &&
				(flag&VOX_FL_PXPY) && (flag&VOX_FL_PXPYPZ)) ||
			((flag&VOX_FL_PX) && (flag&VOX_FL_PXPY) &&
				(flag&VOX_FL_PYPZ)) ||
			((flag&VOX_FL_PY) && (flag&VOX_FL_PXPY) &&
				(flag&VOX_FL_PXPZ)))
			{ fl2|=128; }
#endif

#if 0
		if((flag&VOX_FL_NX) && (flag&VOX_FL_NY) && (flag&VOX_FL_NXNY))
				{ fl2|=16; }
		if((flag&VOX_FL_PX) && (flag&VOX_FL_NY) && (flag&VOX_FL_PXNY))
				{ fl2|=32; }
		if((flag&VOX_FL_NX) && (flag&VOX_FL_PY) && (flag&VOX_FL_NXPY))
				{ fl2|=64; }
		if((flag&VOX_FL_PX) && (flag&VOX_FL_PY) && (flag&VOX_FL_PXPY))
				{ fl2|=128; }
#endif

		if(fl2==0xF0)
		{
			vtxa[4*3+2]+=dk/2; vtxa[0*3+2]+=dk/2;
			vtxa[5*3+2]+=dk/2; vtxa[1*3+2]+=dk/2;
			vtxa[6*3+2]+=dk/2; vtxa[2*3+2]+=dk/2;
			vtxa[7*3+2]+=dk/2; vtxa[3*3+2]+=dk/2;
		}

		if(fl2==0x0F)
		{
			vtxa[4*3+2]-=dk/2;
			vtxa[5*3+2]-=dk/2;
			vtxa[6*3+2]-=dk/2;
			vtxa[7*3+2]-=dk/2;
		}

		if(fl2&1)		{ vtxa[4*3+2]+=dk; }
		if(fl2&2)		{ vtxa[5*3+2]+=dk; }
		if(fl2&4)		{ vtxa[6*3+2]+=dk; }
		if(fl2&8)		{ vtxa[7*3+2]+=dk; }
		if(fl2&16)		{ vtxa[4*3+2]-=dk; vtxa[0*3+2]-=dk; }
		if(fl2&32)		{ vtxa[5*3+2]-=dk; vtxa[1*3+2]-=dk; }
		if(fl2&64)		{ vtxa[6*3+2]-=dk; vtxa[2*3+2]-=dk; }
		if(fl2&128)		{ vtxa[7*3+2]-=dk; vtxa[3*3+2]-=dk; }

//		if((flag&15)!=0)
//			flag|=15;

//		flag|=15;
	}

//	if((flag&15)!=0)
//		flag|=15;

	for(i=0; i<6; i++)
	{
//		if(flag&(65536<<i))
//			continue;

		if(!(flag&(1<<i)))
			continue;

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
		if(btge_voxinfo[ty]->img_scale[vtxt[i]]!=1.0)
		{
			f=btge_voxinfo[ty]->img_scale[vtxt[i]];
			for(j=0; j<4; j++)
			{
				stb[j*2+0]=(pos[sti[i*2+0]]+sta[j*2+0])*f;
				stb[j*2+1]=(pos[sti[i*2+1]]+sta[j*2+1])*f;
			}
		}else
		{
			V4F_COPY(sta+0, stb+0);
			V4F_COPY(sta+4, stb+4);
		}
		
//		BTGE_Voxel_EmitFace(chk, vox,
//			pt1, sta+0,
//			pt2, sta+2,
//			pt3, sta+4,
//			pt4, sta+6,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

//		BTGE_Voxel_EmitFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nva+(i*4), rgba+(i*4),
//			tex[vtxt[i]]);

		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=ca;

//		f=BTGE_Voxel_GetVoxelGamma(chk, vox);
		f=vgam[i];
//		slrgba[0]=f*brgba[0]*gam[i];
//		slrgba[1]=f*brgba[1]*gam[i];
//		slrgba[2]=f*brgba[2]*gam[i];
		slrgba[0]=slgam[i*3+0]*brgba[0]*gam[i];
		slrgba[1]=slgam[i*3+1]*brgba[1]*gam[i];
		slrgba[2]=slgam[i*3+2]*brgba[2]*gam[i];
		slrgba[3]=brgba[3];

		vlrgba[0]=vlgam[i*3+0]*brgba[0]*gam[i];
		vlrgba[1]=vlgam[i*3+1]*brgba[1]*gam[i];
		vlrgba[2]=vlgam[i*3+2]*brgba[2]*gam[i];
		vlrgba[3]=brgba[3];

		V3F_SUB(pt3, pt4, tv0);
		V3F_SUB(pt2, pt3, tv1);
		V3F_CROSS(tv0, tv1, tv2);

		V3F_SUB(pt2, pt3, tv0);
		V3F_SUB(pt1, pt2, tv1);
		V3F_CROSS(tv0, tv1, tv3);
		
		V3F_ADD(tv2, tv3, tv0);
		V3F_NORMALIZE(tv0, nv);

//		BTGE_Voxel_EmitFace(chk, vox,
//			pt4, sta+6,
//			pt3, sta+4,
//			pt2, sta+2,
//			pt1, sta+0,
//			nv, rgba+(i*4),
//			tex[vtxt[i]]);

//		BTGE_Voxel_EmitSolidFace2(chk, vox,
		BTGE_Voxel_EmitFace3(chk, vox,
			pt4, stb+6, nv, brgba, slrgba, vlrgba,
			pt3, stb+4, nv, brgba, slrgba, vlrgba,
			pt2, stb+2, nv, brgba, slrgba, vlrgba,
			pt1, stb+0, nv, brgba, slrgba, vlrgba,
			tex[vtxt[i]]);
	}
}


static void lbxgl_swapvec(float *a, float *b, int n)
{
	float f;
	int i;
	
	for(i=0; i<n; i++)
		{ f=a[i]; a[i]=b[i]; b[i]=f; }
}

static void lbxgl_swapvec_sb(sbyte *a, sbyte *b, int n)
{
	int f;
	int i;
	
	for(i=0; i<n; i++)
		{ f=a[i]; a[i]=b[i]; b[i]=f; }
}

static void lbxgl_swapvec_ub(byte *a, byte *b, int n)
{
	int f;
	int i;
	
	for(i=0; i<n; i++)
		{ f=a[i]; a[i]=b[i]; b[i]=f; }
}

BTGE_API void BTGE_Voxel_RebuildSwapFaces(
	BTGE_VoxelChunk *chk, int a, int b)
{
	BTGE_VoxelData va, vb;
	float f, g;
	int i, j;
	
	if(a==b)return;
	
	lbxgl_swapvec(chk->mesh->face_xyz +a*9, chk->mesh->face_xyz +b*9, 9);
	lbxgl_swapvec(chk->mesh->face_st  +a*6, chk->mesh->face_st  +b*6, 6);

#ifndef BTGE_VOXEL_BYTERGBA
	lbxgl_swapvec(chk->mesh->face_norm+a*9, chk->mesh->face_norm+b*9, 9);
	lbxgl_swapvec(chk->mesh->face_norm_x+a*9, chk->mesh->face_norm_x+b*9, 9);
	lbxgl_swapvec(chk->mesh->face_norm_y+a*9, chk->mesh->face_norm_y+b*9, 9);
	lbxgl_swapvec(chk->mesh->face_rgba+a*12, chk->mesh->face_rgba+b*12, 12);
	lbxgl_swapvec(chk->mesh->face_slrgba+a*12, chk->mesh->face_slrgba+b*12, 12);
	lbxgl_swapvec(chk->mesh->face_vlrgba+a*12, chk->mesh->face_vlrgba+b*12, 12);
#else
	lbxgl_swapvec_sb(chk->mesh->face_norm+a*9, chk->mesh->face_norm+b*9, 9);
	lbxgl_swapvec_ub(chk->mesh->face_rgba+a*12, chk->mesh->face_rgba+b*12, 12);
	lbxgl_swapvec_ub(chk->mesh->face_slrgba+a*12, chk->mesh->face_slrgba+b*12, 12);
	lbxgl_swapvec_ub(chk->mesh->face_vlrgba+a*12, chk->mesh->face_vlrgba+b*12, 12);
#endif

	i=chk->mesh->face_tex[a];
	j=chk->mesh->face_tex[b];
	chk->mesh->face_tex[a]=j;
	chk->mesh->face_tex[b]=i;

	va=chk->mesh->face_vox[a];
	vb=chk->mesh->face_vox[b];
	chk->mesh->face_vox[a]=vb;
	chk->mesh->face_vox[b]=va;
}

BTGE_API void BTGE_Voxel_RebuildSwapAlphaFaces(
	BTGE_VoxelChunk *chk, int a, int b)
{
	BTGE_VoxelData va, vb;
	float f, g;
	int i, j;
	
	if(a==b)return;
	
	lbxgl_swapvec(chk->mesh->aface_xyz +a*9, chk->mesh->aface_xyz +b*9, 9);
	lbxgl_swapvec(chk->mesh->aface_st  +a*6, chk->mesh->aface_st  +b*6, 6);
#ifndef BTGE_VOXEL_BYTERGBA
	lbxgl_swapvec(chk->mesh->aface_norm+a*9, chk->mesh->aface_norm+b*9, 9);
	lbxgl_swapvec(chk->mesh->aface_rgba+a*12, chk->mesh->aface_rgba+b*12, 12);
	lbxgl_swapvec(chk->mesh->aface_slrgba+a*12, chk->mesh->aface_slrgba+b*12, 12);
	lbxgl_swapvec(chk->mesh->aface_vlrgba+a*12, chk->mesh->aface_vlrgba+b*12, 12);
#else
	lbxgl_swapvec_sb(chk->mesh->aface_norm+a*9, chk->mesh->aface_norm+b*9, 9);
	lbxgl_swapvec_ub(chk->mesh->aface_rgba+a*12, chk->mesh->aface_rgba+b*12, 12);
	lbxgl_swapvec_ub(chk->mesh->aface_slrgba+a*12, chk->mesh->aface_slrgba+b*12, 12);
	lbxgl_swapvec_ub(chk->mesh->aface_vlrgba+a*12, chk->mesh->aface_vlrgba+b*12, 12);
#endif

	i=chk->mesh->aface_tex[a];
	j=chk->mesh->aface_tex[b];
	chk->mesh->aface_tex[a]=j;
	chk->mesh->aface_tex[b]=i;

	va=chk->mesh->aface_vox[a];
	vb=chk->mesh->aface_vox[b];
	chk->mesh->aface_vox[a]=vb;
	chk->mesh->aface_vox[b]=va;
}

BTGE_API void BTGE_Voxel_RebuildSortFaces(BTGE_VoxelChunk *chk)
{
	int i, j;
	
	if(!chk->mesh)
		return;
	
	for(i=0; i<chk->mesh->n_face; i++)
		for(j=i+1; j<chk->mesh->n_face; j++)
	{
		if(chk->mesh->face_tex[j]<chk->mesh->face_tex[i])
			BTGE_Voxel_RebuildSwapFaces(chk, i, j);
	}

	for(i=0; i<chk->mesh->n_aface; i++)
		for(j=i+1; j<chk->mesh->n_aface; j++)
	{
		if(chk->mesh->aface_tex[j]<chk->mesh->aface_tex[i])
			BTGE_Voxel_RebuildSwapAlphaFaces(chk, i, j);
	}
	
	chk->flags&=~VOX_CHFL_USESHADER;
	for(i=0; i<chk->mesh->n_face; i++)
	{
		j=BTGE_Texture_GetImageFlags(chk->mesh->face_tex[i]);
		if(j&(BTGE_TXFL_LIGHTMAT|BTGE_TXFL_GLOSS|
			BTGE_TXFL_WARP_MASK))
		{
			chk->flags|=VOX_CHFL_USESHADER;
		}
	}

	chk->flags&=~VOX_CHFL_ALPHALIGHTMAT;
	for(i=0; i<chk->mesh->n_aface; i++)
	{
		j=BTGE_Texture_GetImageFlags(chk->mesh->aface_tex[i]);
		if(j&BTGE_TXFL_LIGHTMAT)
		{
			chk->flags|=VOX_CHFL_ALPHALIGHTMAT;
		}
	}
}

BTGE_API BTGE_Light *BTGE_Voxel_EmitLight(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	float x, float y, float z,
	float cr, float cg, float cb, float cv,
	int flags)
{
	return(BTGE_Voxel_EmitLightStyle(
		world, chk, x, y, z, cr, cg, cb, cv, flags, 0));
}

BTGE_API BTGE_Light *BTGE_Voxel_EmitLightStyle(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	float x, float y, float z,
	float cr, float cg, float cb, float cv,
	int flags, int style)
{
	BTGE_Light *tmp;

	if(!chk->mesh)
	{
		chk->mesh=gctalloc("btge_voxelchunkmesh_t",
			sizeof(BTGE_VoxelChunkMesh));
	}

	tmp=gctalloc("btge_light_t", sizeof(BTGE_Light));
	tmp->next=chk->mesh->light;
	chk->mesh->light=tmp;
	
	tmp->curval=cv;
	
//	tmp->rel_org[0]=x-(chk->ox*32);
//	tmp->rel_org[1]=y-(chk->oy*32);
//	tmp->rel_org[2]=z-(chk->oz*32);

	tmp->rel_org[0]=x-((chk->ox-chk->rgn->ox)*32);
	tmp->rel_org[1]=y-((chk->oy-chk->rgn->oy)*32);
	tmp->rel_org[2]=z-((chk->oz-chk->rgn->oz)*32);
	
	tmp->clr[0]=cr;		tmp->clr[1]=cg;		tmp->clr[2]=cb;
	tmp->val[0]=cv;		tmp->val[1]=0;
//	tmp->coeff[0]=0;	tmp->coeff[1]=0.1;	tmp->coeff[2]=1;
	tmp->mins[0]=-cv;	tmp->mins[1]=-cv;	tmp->mins[2]=-cv;
	tmp->maxs[0]= cv;	tmp->maxs[1]= cv;	tmp->maxs[2]= cv;

	tmp->coeff[0]=0.50/0.58;
	tmp->coeff[1]=0.50/0.58;
	tmp->coeff[2]=1;

	tmp->flags=flags;
	tmp->style=style;
	
	return(tmp);
}
