/*
Goal: Elastic Surface Net
But this time using faces and vertices.

Basic idea:
Emit a bunch of "elastic" faces and vertices (initially in their normal blocky form);
Have things jiggle around some;
Spit out geometry.
 */
 
#include <btgecm.h>

BTGE_VoxelElasticVertex *btge_voxelesn_freevtx=NULL;
BTGE_VoxelElasticFace *btge_voxelesn_freeface=NULL;


BTGE_API BTGE_VoxelElasticVertex *BTGE_VoxelESN_AllocVertex()
{
	BTGE_VoxelElasticVertex *tmp;
	
	btge_voxel_lock();
	
	if(btge_voxelesn_freevtx)
	{
		tmp=btge_voxelesn_freevtx;
		btge_voxelesn_freevtx=tmp->next;
		
		memset(tmp, 0, sizeof(BTGE_VoxelElasticVertex));
		
		btge_voxel_unlock();
		return(tmp);
	}
	
	tmp=gctalloc("btge_voxelelasticvertex_t",
		sizeof(BTGE_VoxelElasticVertex));
	btge_voxel_unlock();
	return(tmp);
}

BTGE_API BTGE_VoxelElasticFace *BTGE_VoxelESN_AllocFace()
{
	BTGE_VoxelElasticFace *tmp;

	btge_voxel_lock();
	if(btge_voxelesn_freeface)
	{
		tmp=btge_voxelesn_freeface;
		btge_voxelesn_freeface=tmp->next;
		
		memset(tmp, 0, sizeof(BTGE_VoxelElasticFace));
		
		btge_voxel_unlock();
		return(tmp);
	}
	
	tmp=gctalloc("btge_voxelelasticface_t",
		sizeof(BTGE_VoxelElasticFace));
	btge_voxel_unlock();
	return(tmp);
}

BTGE_API void BTGE_VoxelESN_FreeVertex(BTGE_VoxelElasticVertex *tmp)
{
	btge_voxel_lock();
	tmp->next=btge_voxelesn_freevtx;
	btge_voxelesn_freevtx=tmp;
	btge_voxel_unlock();
}

BTGE_API void BTGE_VoxelESN_FreeFace(BTGE_VoxelElasticFace *tmp)
{
	btge_voxel_lock();
	tmp->next=btge_voxelesn_freeface;
	btge_voxelesn_freeface=tmp;
	btge_voxel_unlock();
}

BTGE_API BTGE_VoxelElasticVertex *BTGE_VoxelESN_LookupChunkVertex(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk, float *org)
{
	BTGE_VoxelElasticVertex *cur;
	
	if(!chk->mesh)
		return(NULL);
	
	cur=chk->mesh->esn_vertex;
	while(cur)
	{
		if(V3F_DIST2(cur->oldorg, org)<=1)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTGE_API BTGE_VoxelElasticVertex *BTGE_VoxelESN_LookupRegionVertex(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, float *org)
{
	BTGE_VoxelElasticVertex *cur;
	int i;
	
	cur=rgn->esn_vertex; i=1000000;
	while(cur && (i>0))
	{
		if(V3F_DIST2(cur->oldorg, org)<=1)
			return(cur);
		cur=cur->gnext;
	}
	
	if(i<0)
	{
		printf("BTGE_VoxelESN_LookupRegionVertex: Cycle Timeout\n");
		cur=rgn->esn_vertex=NULL;
	}
	
	return(NULL);
}

BTGE_API BTGE_VoxelElasticVertex *BTGE_VoxelESN_GetRegionVertex(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, float *org)
{
	BTGE_VoxelElasticVertex *tmp;

#if 1
	tmp=BTGE_VoxelESN_AllocVertex();
	V3F_COPY(org, tmp->org);
	V3F_COPY(org, tmp->oldorg);
	tmp->flags|=1;
	return(tmp);
#endif

	tmp=BTGE_VoxelESN_LookupRegionVertex(world, rgn, org);
	if(tmp)return(tmp);
	
	tmp=BTGE_VoxelESN_AllocVertex();
	V3F_COPY(org, tmp->org);
	V3F_COPY(org, tmp->oldorg);
	tmp->flags|=256;
	tmp->gnext=rgn->esn_vertex;
	rgn->esn_vertex=tmp;
	return(tmp);
}

BTGE_API BTGE_VoxelElasticVertex *BTGE_VoxelESN_GetChunkVertex(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk, float *org)
{
	BTGE_VoxelElasticVertex *tmp;
	int mx, my, mz, nx, ny, nz;
	
//	mx=chk->ox;
//	my=chk->oy;
//	mz=chk->oz;
	mx=chk->ox-chk->rgn->ox;
	my=chk->oy-chk->rgn->oy;
	mz=chk->oz-chk->rgn->oz;
	nx=mx+chk->xs;
	ny=my+chk->ys;
	nz=mz+chk->zs;

	tmp=BTGE_VoxelESN_LookupChunkVertex(world, chk, org);
	if(tmp)return(tmp);

	if(	((org[0]-1)<=(mx*32)) ||
		((org[1]-1)<=(my*32)) ||
		((org[2]-1)<=(mz*32)) ||
		((org[0]+1)>=(nx*32)) ||
		((org[1]+1)>=(ny*32)) ||
		((org[2]+1)>=(nz*32)))
	{
		tmp=BTGE_VoxelESN_GetRegionVertex(world, chk->rgn, org);
		tmp->next=chk->mesh->esn_vertex;
		chk->mesh->esn_vertex=tmp;

		if((org[0]-1)<=(mx*32))tmp->flags|=2;
		if((org[1]-1)<=(my*32))tmp->flags|=4;
		if((org[2]-1)<=(mz*32))tmp->flags|=8;
		if((org[0]+1)>=(nx*32))tmp->flags|=2;
		if((org[1]+1)>=(ny*32))tmp->flags|=4;
		if((org[2]+1)>=(nz*32))tmp->flags|=8;

		return(tmp);
	}

	if(!chk->mesh)
	{
		chk->mesh=gctalloc("btge_voxelchunkmesh_t",
			sizeof(BTGE_VoxelChunkMesh));
	}

	tmp=BTGE_VoxelESN_AllocVertex();
	V3F_COPY(org, tmp->org);
	V3F_COPY(org, tmp->oldorg);
	tmp->next=chk->mesh->esn_vertex;
	chk->mesh->esn_vertex=tmp;
	return(tmp);
}

BTGE_API BTGE_VoxelElasticVertex *BTGE_VoxelESN_GetChunkVertexAnchor(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk, float *org)
{
	BTGE_VoxelElasticVertex *tmp;

	tmp=BTGE_VoxelESN_GetChunkVertex(world, chk, org);
	tmp->flags|=1;
	return(tmp);
}

void BTGE_VoxelESN_EmitFace3(
	BTGE_BrushWorld *world,
	BTGE_VoxelChunk *chk, BTGE_VoxelData *vox,
	int x, int y, int z,
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
	BTGE_VoxelElasticFace *tmp;

	if(!chk->mesh)
	{
		chk->mesh=gctalloc("btge_voxelchunkmesh_t",
			sizeof(BTGE_VoxelChunkMesh));
	}

	tmp=BTGE_VoxelESN_AllocFace();
	tmp->next=chk->mesh->esn_face;
	chk->mesh->esn_face=tmp;
	
	tmp->x=x; tmp->y=y; tmp->z=z;
	tmp->vox=vox;
	
	V4F_COPY(rgba0, tmp->brgba);
	V4F_COPY(slrgba0, tmp->slrgba);
	V4F_COPY(vlrgba0, tmp->vlrgba);
	
	tmp->vert[0]=BTGE_VoxelESN_GetChunkVertex(world, chk, xyz0);
	tmp->vert[1]=BTGE_VoxelESN_GetChunkVertex(world, chk, xyz1);
	tmp->vert[2]=BTGE_VoxelESN_GetChunkVertex(world, chk, xyz2);
	tmp->vert[3]=BTGE_VoxelESN_GetChunkVertex(world, chk, xyz3);
	tmp->tex=tex;
}


BTGE_API void BTGE_VoxelESN_EmitBlock(
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
		1, 3, 2, 0,	//-Z
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
	int i, j;
	
	//skip air
	if(!vox->type)
		return;
	if(!(vox->type&VOX_TY_MASK))
		return;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk,
		vox, x, y, z);
	if(!inf)return;

	ty=vox->type&4095;
	if(inf->EmitBlock)
	{
		//btge_voxinfo[ty]->EmitBlock(
			//world, chk, vox,
			//min, max, flag, x, y, z);
		//return;
	}
	
	//check bad block type
	if(inf->img_top<=0)
		return;

	biome=BTGE_Voxel_FetchBiomeForID(world, chk->biome);

	if(inf->flags2&BTGE_TXFL2_AUXVARIANT)
	{
		i=inf->img_auxvar_top[vox->aux&7];
		if(i>0)
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

//	tex[0]=inf->img_top;
//	tex[1]=inf->img_side;
//	tex[2]=inf->img_bot;
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

			slgam[i*3+0]=vgam[i];
			slgam[i*3+1]=vgam[i];
			slgam[i*3+2]=vgam[i];
		}
		
	}

	f=1;
	g=1;
	ca=1;

//	switch(ty)
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
//		cr=biome->clr_grass[0];
//		cg=biome->clr_grass[1];
//		cb=biome->clr_grass[2];
		BTGE_Voxel_CalculateGrassColorForPoint(world, x, y, z,
			&cr, &cg, &cb);
		break;
	default: cr=1; cg=1; cb=1; break;
	}

	if(inf->flags&BTGE_TXFL_ALPHA)
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

	if(inf->EmitBlock)
	{
		for(i=0; i<8; i++)
		{
			BTGE_VoxelESN_GetChunkVertexAnchor(world, chk, vtxa+i*3);
		}
		return;
	}

	for(i=0; i<6; i++)
	{
		if(!(flag&(1<<i)))
			continue;

		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);
		
		if(inf->img_scale[vtxt[i]]!=1.0)
		{
			f=inf->img_scale[vtxt[i]];
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
		
		brgba[0]=cr;	brgba[1]=cg;
		brgba[2]=cb;	brgba[3]=ca;

		f=vgam[i];
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

		BTGE_VoxelESN_EmitFace3(world, chk, vox, x, y, z,
			pt4, stb+6, nv, brgba, slrgba, vlrgba,
			pt3, stb+4, nv, brgba, slrgba, vlrgba,
			pt2, stb+2, nv, brgba, slrgba, vlrgba,
			pt1, stb+0, nv, brgba, slrgba, vlrgba,
			tex[vtxt[i]]);
	}
}

BTGE_API void BTGE_VoxelESN2_FlushChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelElasticNode *ncur, *nnxt;
	BTGE_VoxelElasticVertex *vcur, *vnxt;
	BTGE_VoxelElasticFace *fcur, *fnxt;

	if(!chk->mesh)
		return;

	if(chk->mesh->esn_node)
	{
		ncur=chk->mesh->esn_node;
		while(ncur)
		{
			nnxt=ncur->next;
			BTGE_VoxelESN_FreeNode(ncur);
			ncur=nnxt;
		}
		chk->mesh->esn_node=NULL;
	}

	if(chk->mesh->esn_vertex)
	{
		vcur=chk->mesh->esn_vertex;
		while(vcur)
		{
			vnxt=vcur->next;
			
			//don't touch region vertices
			if(vcur->flags&256)
				{ vcur=vnxt; continue; }
			
			BTGE_VoxelESN_FreeVertex(vcur);
			vcur=vnxt;
		}
		chk->mesh->esn_vertex=NULL;
	}

	if(chk->mesh->esn_face)
	{
		fcur=chk->mesh->esn_face;
		while(fcur)
		{
			fnxt=fcur->next;
			BTGE_VoxelESN_FreeFace(fcur);
			fcur=fnxt;
		}
		chk->mesh->esn_face=NULL;
	}
}

BTGE_API void BTGE_VoxelESN_AdjustChunkFace(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelElasticFace *face)
{
	float org[3], org1[3], dv[3];
	float f, g;
	int i;

	if((face->vox->type&VOX_TYM_MASK)!=VOX_TYM_FLEXFORM)
	{
		V3F_COPY(face->vert[0]->oldorg, face->vert[0]->org);
		V3F_COPY(face->vert[1]->oldorg, face->vert[1]->org);
		V3F_COPY(face->vert[2]->oldorg, face->vert[2]->org);
		V3F_COPY(face->vert[3]->oldorg, face->vert[3]->org);
		face->vert[0]->flags|=1;
		face->vert[1]->flags|=1;
		face->vert[2]->flags|=1;
		face->vert[3]->flags|=1;
		return;
	}
	
	V3F_ZERO(org);
	for(i=0; i<4; i++)
		{ V3F_ADD(org, face->vert[i]->org, org); }
	V3F_SCALE(org, 0.25, org);

	for(i=0; i<4; i++)
	{
		if(face->vert[i]->flags&1)
			continue;
	
		V3F_SUB(face->vert[i]->org, org, dv);
		//f=V3F_DIST(face->vert[i]->org, org);
		f=V3F_LEN(dv);
		f=V3F_NORMALIZE(dv, dv);
		f=(f-16)/16;
		V3F_ADDSCALE(org, dv, -f, org1);
		
		V3F_SCALEADDSCALE(face->vert[i]->org, 0.5, org, 0.5, org1);
		
//		V3F_SCALEADDSCALE(face->vert[i]->org, 0.75, org1, 0.25,
//			face->vert[i]->org);
		V3F_SCALEADDSCALE(face->vert[i]->org, 0.875, org1, 0.125,
			face->vert[i]->org);
	}
}

BTGE_API void BTGE_VoxelESN_AdjustChunkVertex(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelElasticVertex *vert)
{
	float org[3], org1[3], dv[3];
	float f, g;
	int i;

	if(vert->flags&1)
	{
		V3F_COPY(vert->oldorg, vert->org);
		return;
	}

#if 0
	if((vert->oldorg[0]-1)<=(chk->ox*32))vert->flags|=2;
	if((vert->oldorg[1]-1)<=(chk->oy*32))vert->flags|=4;
	if((vert->oldorg[2]-1)<=(chk->oz*32))vert->flags|=8;
	if((vert->oldorg[0]+1)>=((chk->ox+chk->xs)*32))vert->flags|=2;
	if((vert->oldorg[1]+1)>=((chk->oy+chk->ys)*32))vert->flags|=4;
	if((vert->oldorg[2]+1)>=((chk->oz+chk->zs)*32))vert->flags|=8;

	if(vert->flags&(2|4|8))
		vert->flags|=1;
#endif

	if(vert->flags&2)
		{ vert->org[0]=vert->oldorg[0]; }
	if(vert->flags&4)
		{ vert->org[1]=vert->oldorg[1]; }
	if(vert->flags&8)
		{ vert->org[2]=vert->oldorg[2]; }

//	if(vert->flags&(2|4|8))
//		return;

	V3F_SUB(vert->oldorg, vert->org, dv);
//	f=V3F_LEN(dv);
	f=V3F_NORMALIZE(dv, dv);
//	f=f/16;
	f=f/32;
	f=f*f;
//	if(f>1)f=1;
	V3F_ADDSCALE(vert->org, dv, f, vert->org);
}

BTGE_API void BTGE_VoxelESN_AdjustChunkFaces(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelElasticFace *cur;
	BTGE_VoxelElasticVertex *vcur;
	
	if(!chk->mesh)
		return;
	
	cur=chk->mesh->esn_face;
	while(cur)
	{
		BTGE_VoxelESN_AdjustChunkFace(world, chk, cur);
		cur=cur->next;
	}

	vcur=chk->mesh->esn_vertex;
	while(vcur)
	{
		BTGE_VoxelESN_AdjustChunkVertex(world, chk, vcur);
		vcur=vcur->next;
	}
}

BTGE_API void BTGE_VoxelESN_ProjectFacePointCoords(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelElasticFace *face,
	float *org, float *nv, float *st)
{
	BTGE_VoxelInfo *inf;
	float f, g;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk,
		face->vox, face->x, face->y, face->z);
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

BTGE_API void BTGE_VoxelESN_EmitChunkFace(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelElasticFace *face)
{
	float v0[3], v1[3], v2[3], v3[3];
	float d0[3], d1[3], d2[3], d3[3];
	float t0[3], t1[3], t2[3], t3[3];
	float nv[4];

	if((face->vox->type&VOX_TYM_MASK)!=VOX_TYM_FLEXFORM)
		return;

	V3F_COPY(face->vert[0]->org, v0);
	V3F_COPY(face->vert[1]->org, v1);
	V3F_COPY(face->vert[2]->org, v2);
	V3F_COPY(face->vert[3]->org, v3);

	V3F_SUB(v1, v0, d0);
	V3F_SUB(v2, v1, d1);
	V3F_SUB(v3, v2, d2);
	V3F_SUB(v0, v3, d3);
//	V3F_CROSS(d0, d1, d2);
//	V3F_NORMALIZE(d2, nv);
	V3F_CROSS(d0, d1, t0);
	V3F_CROSS(d1, d2, t1);
	V3F_CROSS(d2, d3, t2);
	V3F_CROSS(d3, d0, t3);
	
	V3F_ADD(t0, t1, t0);
	V3F_ADD(t2, t3, t1);
	V3F_ADD(t0, t1, t0);
	V3F_NORMALIZE(t0, nv);

	BTGE_VoxelESN_ProjectFacePointCoords(world, chk, face, v0, nv, t0);
	BTGE_VoxelESN_ProjectFacePointCoords(world, chk, face, v1, nv, t1);
	BTGE_VoxelESN_ProjectFacePointCoords(world, chk, face, v2, nv, t2);
	BTGE_VoxelESN_ProjectFacePointCoords(world, chk, face, v3, nv, t3);

	BTGE_Voxel_EmitFace3(chk, face->vox,
		v0, t0, nv, face->brgba, face->slrgba, face->vlrgba,
		v1, t1, nv, face->brgba, face->slrgba, face->vlrgba,
		v2, t2, nv, face->brgba, face->slrgba, face->vlrgba,
		v3, t3, nv, face->brgba, face->slrgba, face->vlrgba,
		face->tex);
}

BTGE_API void BTGE_VoxelESN_EmitChunkFaces(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelElasticFace *cur;
	
	if(!chk->mesh)
		return;
	
	cur=chk->mesh->esn_face;
	while(cur)
	{
		BTGE_VoxelESN_EmitChunkFace(world, chk, cur);
		cur=cur->next;
	}
}

BTGE_API void BTGE_VoxelESN2_RebuildChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox;
//	BTGE_VoxelElasticNode *node, *ncur, *nnxt;
	float org[3], mins[3], maxs[3];
	float vsz, vsz2;
	int x, y, z;
	int i, j, k, l, nvox;

//	BTGE_Voxel_Setup();

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;
	
	BTGE_VoxelESN2_FlushChunk(world, chk);

	nvox=0;

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
			for(k=0; k<chk->zs; k++)
	{
		l=0;
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+k;

//		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		if(!vox)continue;
		if(!(vox->type&VOX_TY_MASK))
			continue;

//		if(BTGE_Voxel_WorldChunkVoxelFluidP(world, chk, vox, x, y, z))
//			continue;

		//if(!BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
			//continue;

		if(1)
		{
#if 1
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x-1, y, z))l|=VOX_FL_NX;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x+1, y, z))l|=VOX_FL_PX;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y-1, z))l|=VOX_FL_NY;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y+1, z))l|=VOX_FL_PY;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y, z-1))l|=VOX_FL_NZ;
			if(!BTGE_Voxel_WorldRegionVoxelSkipFaceP(
				world, chk->rgn, vox, x, y, z+1))l|=VOX_FL_PZ;
#endif

			if(BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
			{
				if(!z && ((vox->type&VOX_TY_MASK)==VOX_TY_BEDROCK))
					l&=~16;
//				if(!l)continue;
			}

		}

#if 0
		if((vox->type&VOX_TYM_MASK)==VOX_TYM_FLEXFORM)
		{
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y, z+1))l|=VOX_FL_NXPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y, z+1))l|=VOX_FL_PXPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y-1, z+1))l|=VOX_FL_NYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x, y+1, z+1))l|=VOX_FL_PYPZ;

			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y+1, z))l|=VOX_FL_NXPY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y-1, z))l|=VOX_FL_PXNY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y+1, z))l|=VOX_FL_PXPY;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y-1, z))l|=VOX_FL_NXNY;

			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y+1, z+1))l|=VOX_FL_NXPYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y-1, z+1))l|=VOX_FL_PXNYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x+1, y+1, z+1))l|=VOX_FL_PXPYPZ;
			if(!BTGE_Voxel_WorldRegionVoxelOpaqueP(
				world, chk->rgn, x-1, y-1, z+1))l|=VOX_FL_NXNYPZ;
		}
#endif

		if(!l)continue;

//		org[0]=x*vsz; org[1]=y*vsz; org[2]=z*vsz;

		org[0]=(x-chk->rgn->ox)*vsz;
		org[1]=(y-chk->rgn->oy)*vsz;
		org[2]=(z-chk->rgn->oz)*vsz;

		mins[0]=org[0]; mins[1]=org[1]; mins[2]=org[2];
		maxs[0]=org[0]+vsz; maxs[1]=org[1]+vsz; maxs[2]=org[2]+vsz;

//		org[0]=x*vsz+vsz2; org[1]=y*vsz+vsz2; org[2]=z*vsz+vsz;

		BTGE_VoxelESN_EmitBlock(world, chk, vox, mins, maxs, l, x, y, z);		
		nvox++;
	}
	
//	BTGE_VoxelESN_LinkChunkNodes(world, chk);
	
	for(i=0; i<8; i++)
	{
		BTGE_VoxelESN_AdjustChunkFaces(world, chk);
	}
	
	BTGE_VoxelESN_EmitChunkFaces(world, chk);

	BTGE_VoxelESN2_FlushChunk(world, chk);
}
