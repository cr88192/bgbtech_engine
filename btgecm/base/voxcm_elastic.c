/*
Goal: Elastic Surface Net
 */
 
#include <btgecm.h>

BTGE_VoxelElasticNode *btge_voxelesn_freenode=NULL;

BTGE_API BTGE_VoxelElasticNode *BTGE_VoxelESN_AllocNode()
{
	BTGE_VoxelElasticNode *tmp;
	if(btge_voxelesn_freenode)
	{
		tmp=btge_voxelesn_freenode;
		btge_voxelesn_freenode=tmp->next;
		
		memset(tmp, 0, sizeof(BTGE_VoxelElasticNode));
		
		return(tmp);
	}
	
	tmp=gctalloc("btge_voxelelasticnode_t",
		sizeof(BTGE_VoxelElasticNode));
	return(tmp);
}

BTGE_API void BTGE_VoxelESN_FreeNode(BTGE_VoxelElasticNode *tmp)
{
	tmp->next=btge_voxelesn_freenode;
	btge_voxelesn_freenode=tmp;
}

BTGE_API BTGE_VoxelElasticNode *BTGE_VoxelESN_LookupChunkNode(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk, int x, int y, int z)
{
	BTGE_VoxelElasticNode *cur;
	
	cur=chk->mesh->esn_node;
	while(cur)
	{
		if((cur->x==x) && (cur->y==y) && (cur->z==z))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BTGE_API void BTGE_VoxelESN_LinkChunkNodes(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelElasticNode *ncur, *nnxt;
	int x, y, z;

	ncur=chk->mesh->esn_node;
	while(ncur)
	{
		x=ncur->x; y=ncur->y; z=ncur->z;
		ncur->link[0]=BTGE_VoxelESN_LookupChunkNode(world, chk, x-1, y, z);
		ncur->link[1]=BTGE_VoxelESN_LookupChunkNode(world, chk, x+1, y, z);
		ncur->link[2]=BTGE_VoxelESN_LookupChunkNode(world, chk, x, y-1, z);
		ncur->link[3]=BTGE_VoxelESN_LookupChunkNode(world, chk, x, y+1, z);
		ncur->link[4]=BTGE_VoxelESN_LookupChunkNode(world, chk, x, y, z-1);
		ncur->link[5]=BTGE_VoxelESN_LookupChunkNode(world, chk, x, y, z+1);
		ncur=ncur->next;
	}
}

BTGE_API void BTGE_VoxelESN_AdjustChunkNode(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelElasticNode *node)
{
	float org[3], mins[3], maxs[3];
//	vec3 org;
	float vsz;
	int x, y, z, n;
	int i;

	//non-flexform voxels don't move
	if((node->vox->type&VOX_TYM_MASK)!=VOX_TYM_FLEXFORM)
		return;

	V3F_ZERO(org); n=0;
	for(i=0; i<6; i++)
	{
		if(!node->link[i])
			continue;
		V3F_ADD(org, node->link[i]->org, org);
		n++;
	}

	if(!n)return;

	vsz=chk->rgn->voxsz;
	x=node->x; y=node->y; z=node->z;
	mins[0]=x*vsz; mins[1]=y*vsz; mins[2]=z*vsz+(vsz*0.5);
	maxs[0]=mins[0]+vsz; maxs[1]=mins[1]+vsz; maxs[2]=mins[2]+vsz;
	
	V3F_SCALEADDSCALE(node->org, 0.5, org, 0.5/n, org);
	for(i=0; i<3; i++)
	{
		if(org[i]<mins[i])org[i]=mins[i];
		if(org[i]>maxs[i])org[i]=maxs[i];
	}
	V3F_COPY(org, node->org);
}

BTGE_API void BTGE_VoxelESN_AdjustChunkNodes(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelElasticNode *ncur, *nnxt;
	int x, y, z;

	ncur=chk->mesh->esn_node;
	while(ncur)
	{
		BTGE_VoxelESN_AdjustChunkNode(world, chk, ncur);
		ncur=ncur->next;
	}
}

BTGE_API bool BTGE_VoxelESN_CheckPointOpaqueP(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	float *org)
{
	BTGE_VoxelData *vox;
	int x, y, z;

	x=floor(org[0]/32);
	y=floor(org[1]/32);
	z=floor(org[2]/32);

	vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
	if(!vox)return(false);
	if(!(vox->type&VOX_TY_MASK))
		return(false);

	if(!BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
		return(false);
		
	return(true);
}

BTGE_API bool BTGE_VoxelESN_ProjectNodePointCoords(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelElasticNode *node,
	float *org, float *nv, float *st)
{
	BTGE_VoxelInfo *inf;
	float f, g;

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk,
		node->vox, node->x, node->y, node->z);
	if(!inf)return(false);

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
	return(true);
}

BTGE_API void BTGE_VoxelESN_EmitChunkNodeSurfaces(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	BTGE_VoxelElasticNode *node)
{
	static float st[5*2]={
		0.5, 0.5,	0.0, 0.0,
		1.0, 0.0,	0.0, 1.0,
		1.0, 1.0};
	static int tris[5*4*3]={
		0, 2, 6,	1, 2, 6,	1, 3, 6,	0, 3, 6,
		0, 4, 6,	1, 4, 6,	2, 4, 6,	3, 4, 6,
		0, 5, 6,	1, 5, 6,	2, 5, 6,	3, 5, 6,
		0, 2, 4,	1, 2, 4,	1, 3, 4,	0, 3, 4,
		0, 2, 5,	1, 2, 5,	1, 3, 5,	0, 3, 5};
	
	float vlrgba[7*4];
	float slrgba[7*4];
	float brgba[4];
	float tv0[3], tv1[3], tv2[3];
	float v0[3], v1[3], v2[3];
	float dv0[3], dv1[3], dv2[3];
	float st0[2], st1[2], st2[2];
	float nv[3];
	BTGE_VoxelElasticNode *nla, *nlb, *nlc;
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf;
	int tex;
	int i, j, k, l;

	//non-flexform voxels don't move
//	if((node->vox->type&VOX_TYM_MASK)!=VOX_TYM_FLEXFORM)
//		return;

	vox=node->vox;

	if((vox->type&VOX_TYM_MASK)!=VOX_TYM_FLEXFORM)
	{
		for(i=0; i<6; i++)
		{
			if(!node->link[i])
				continue;
			if((node->link[i]->vox->type&VOX_TYM_MASK)!=VOX_TYM_FLEXFORM)
				continue;
			vox=node->link[i]->vox;
			break;
		}

		if(i>=6)
			return;
	}

	j=vox->slight;
	if(!j)
	{
		for(i=0; i<6; i++)
		{
			if(!node->link[i])
				continue;
			if((node->link[i]->vox->type&VOX_TYM_MASK)!=VOX_TYM_FLEXFORM)
				continue;

			j=node->link[i]->vox->slight;
			if(j)
			{
				vox=node->link[i]->vox;
				break;
			}
		}
	}

	inf=BTGE_Voxel_GetWorldChunkVoxelInfo(world, chk,
		vox, node->x, node->y, node->z);
	if(!inf)return;
	tex=inf->img_top;

	for(i=0; i<7; i++)
	{
		slrgba[i*4+3]=1;
		vlrgba[i*4+3]=1;
	}

	for(i=0; i<6; i++)
	{
		if(!node->link[i])
			continue;
			
		j=node->link[i]->vox->slight;
		k=node->link[i]->vox->vlight;
		
		if(!j)j=vox->slight;
		
		BTGE_Voxel_GetSLightColor(world, j, slrgba+i*4);
		BTGE_Voxel_GetVLightBaseColor(world, k, vlrgba+i*4);
	}

	j=vox->slight;
//	if(!j)return;
	
#if 0
	if(!j)
	{
		for(i=0; i<6; i++)
		{
			if(!node->link[i])
				continue;
			j=node->link[i]->vox->slight;
			if(j)break;
		}
	}
#endif
	
	k=vox->vlight;
	BTGE_Voxel_GetSLightColor(world, j, slrgba+6*4);
	BTGE_Voxel_GetVLightBaseColor(world, k, vlrgba+6*4);
	
	V4F_CONST(brgba, 1);
	
	for(i=0; i<(3*4); i++)
	{
		j=tris[i*3+0];
		k=tris[i*3+1];
		l=tris[i*3+2];

		nla=node->link[j];
		nlb=node->link[k];
		if(!nla || !nlb)
			continue;
		if(l<6)
		{
			nlc=node->link[l];
			if(!nlc)continue;
		}else
		{
			nlc=NULL;
		}
			
		
//		if(((j+k)&1) && (k<4))
		if(((j+k)&1))
			continue;
		
//		V3F_SCALEADDSCALE(nla->org, 0.5, node->org, 0.5, v0);
//		V3F_SCALEADDSCALE(nlb->org, 0.5, node->org, 0.5, v1);
		V3F_COPY(nla->org, v0);
		V3F_COPY(nlb->org, v1);
		if(nlc)
		{
			V3F_COPY(nlc->org, v2);
		}else
		{
			V3F_COPY(node->org, v2);
		}
		
		V3F_SUB(v1, v0, dv0);
		V3F_SUB(v2, v1, dv1);
		V3F_CROSS(dv0, dv1, dv2);
		V3F_NORMALIZE(dv2, nv);
		
		V3F_ADDSCALE(v2, nv, 32, tv0);
		
		BTGE_VoxelESN_ProjectNodePointCoords(
			world, chk, node, v0, nv, st0);
		BTGE_VoxelESN_ProjectNodePointCoords(
			world, chk, node, v1, nv, st1);
		BTGE_VoxelESN_ProjectNodePointCoords(
			world, chk, node, v2, nv, st2);
		
		if(BTGE_VoxelESN_CheckPointOpaqueP(world, chk, tv0))
		{
			V3F_SCALE(nv, -1, nv);
			BTGE_Voxel_EmitTriangle3(chk, node->vox,
				v2, st2, nv, brgba, slrgba+l*4, vlrgba+l*4,
				v1, st1, nv, brgba, slrgba+k*4, vlrgba+k*4,
				v0, st0, nv, brgba, slrgba+j*4, vlrgba+j*4,
				tex);
		}else
		{
			BTGE_Voxel_EmitTriangle3(chk, node->vox,
				v0, st0, nv, brgba, slrgba+j*4, vlrgba+j*4,
				v1, st1, nv, brgba, slrgba+k*4, vlrgba+k*4,
				v2, st2, nv, brgba, slrgba+l*4, vlrgba+l*4,
				tex);
		}
	}
}

BTGE_API void BTGE_VoxelESN_EmitChunkSurfaces(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelElasticNode *ncur;
	int x, y, z;

	ncur=chk->mesh->esn_node;
	while(ncur)
	{
		BTGE_VoxelESN_EmitChunkNodeSurfaces(world, chk, ncur);
		ncur=ncur->next;
	}
}

BTGE_API void BTGE_VoxelESN_RebuildChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelData *vox;
	BTGE_VoxelElasticNode *node, *ncur, *nnxt;
	float org[3], mins[3], maxs[3];
	float vsz, vsz2;
	int x, y, z;
	int i, j, k, l, nvox;

//	BTGE_Voxel_Setup();

	vsz=chk->rgn->voxsz;
	vsz2=vsz*0.5;
	
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

	nvox=0;

	for(i=-1; i<chk->xs+1; i++)
		for(j=-1; j<chk->ys+1; j++)
			for(k=-1; k<chk->zs+1; k++)
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

		if(!BTGE_Voxel_WorldChunkVoxelOpaqueP(world, chk, vox, x, y, z))
			continue;

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

#if 1
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
//		mins[0]=org[0]; mins[1]=org[1]; mins[2]=org[2];
//		maxs[0]=org[0]+vsz; maxs[1]=org[1]+vsz; maxs[2]=org[2]+vsz;

		org[0]=x*vsz+vsz2; org[1]=y*vsz+vsz2; org[2]=z*vsz+vsz;

		//BTGE_Voxel_EmitBlock(world, chk, vox, mins, maxs, l, x, y, z);
		
		node=BTGE_VoxelESN_AllocNode();
		node->x=x; node->y=y; node->z=z;
		//node->org=vec3fv(org);
		V3F_COPY(org, node->org);
		node->vox=vox;
		
		node->next=chk->mesh->esn_node;
		chk->mesh->esn_node=node;
		
		nvox++;
	}
	
	BTGE_VoxelESN_LinkChunkNodes(world, chk);
	
	for(i=0; i<8; i++)
	{
		BTGE_VoxelESN_AdjustChunkNodes(world, chk);
	}
	
	BTGE_VoxelESN_EmitChunkSurfaces(world, chk);
}
