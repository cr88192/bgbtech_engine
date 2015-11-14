#include <lbxgl2.h>

extern PDGLUI_InputState *lbxgl_state;
extern PDGLUI_Camera *lbxgl_cam;
extern LBXGL_BrushWorld *lbxgl_world;

extern int lbxgl_voxel_chkleft;
extern int lbxgl_voxel_chkstart;

extern int lbxgl_voxel_reforg[3];
extern bool lbxgl_voxel_disablevbo;
extern float lbxgl_voxel_drawdist;

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

void LBXGL_Voxel_RecalcStaticLight(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	int i, j, k, n;

	if(!chk->mesh)
		return;

	n=chk->mesh->n_face*3;
	if(n<=0)return;
	
	if(!chk->mesh->face_tlrgba)
	{
//		chk->mesh->face_tlrgba=gcalloc(n*4);
		k=chk->mesh->m_face*3;
		chk->mesh->face_tlrgba=gcalloc(k*4);
	}

	for(i=0; i<n; i++)
		for(j=0; j<4; j++)
	{
		k=
			chk->mesh->face_slrgba[i*4+j]+
			chk->mesh->face_vlrgba[i*4+j];
		if(k>255)k=255;
		chk->mesh->face_tlrgba[i*4+j]=k;
	}
}

void LBXGL_Voxel_CheckUploadVBO(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	return;

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
	LBXGL_Voxel_RecalcStaticLight(world, chk);
	chk->flags&=~VOX_CHFL_REBUILDING;
#endif
}

void LBXGL_Voxel_CheckChunkEmitSound(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	float mins[3], maxs[3], org[3], borg[3], vel[3];
	BTGE_VoxelData *vox;
	BTGE_VoxelInfo *inf, *binf;
	float d, bd;
	int x,y,z;
	int i, j, k, l, n;

	if(!(chk->flags&VOX_CHFL_HASSOUNDBLOCK))
		return;

	bd=999999.0; binf=NULL;

	for(i=0; i<chk->xs; i++)
		for(j=0; j<chk->ys; j++)
			for(k=0; k<chk->zs; k++)
	{
		l=0;
		x=chk->ox+i;
		y=chk->oy+j;
		z=chk->oz+k;

		vox=BTGE_Voxel_GetRegionVoxel(chk->rgn, x, y, z);
//		vox=BTGE_Voxel_GetWorldRegionVoxel(world, chk->rgn, x, y, z);
		if(!vox)continue;
		if(!(vox->type&VOX_TY_MASK))
			continue;
		inf=BTGE_Voxel_GetWorldVoxelInfo(world, vox, x, y, z);
		if(!inf)continue;
		
		if(!inf->sound_name)
			continue;
		
		LBXGL_Voxel_BoxChunkVoxelRel(chk, x, y, z, mins, maxs);
		V3F_SCALEADDSCALE(mins, 0.5, maxs, 0.5, org);
		d=V3F_DIST(org, lbxgl_cam->org);
		if(d<bd)
		{
			bd=d; binf=inf;
//			V3F_COPY(org, borg);
			V3F_SCALE(org, 1.0/32.0, borg);
		}
	}
	
	if(!binf)return;
	
	if(chk->mesh->sound_name &&
		!strcmp(chk->mesh->sound_name, binf->sound_name))
	{
		V3F_ZERO(vel);
		PDGL_Sound_SetPlayingOrigin(
			chk->mesh->sound_chan,
			borg, vel);
		return;
	}
	
	if(chk->mesh->sound_name)
	{
		PDGL_Sound_StopPlaying(chk->mesh->sound_chan);
		chk->mesh->sound_chan=0;
		chk->mesh->sound_name=NULL;
	}
	
	V3F_ZERO(vel);
	
	chk->mesh->sound_name=binf->sound_name;
	chk->mesh->sound_chan=
		PDGL_Sound_PlayPointSample(
			binf->sound_name, PDGL_SOUND_LOOP,
			binf->sound_vol, 1.0, borg, vel);
}

void LBXGL_Voxel_FlushChunkEmitSound(
	LBXGL_BrushWorld *world, LBXGL_VoxelChunk *chk)
{
	if(chk->mesh && chk->mesh->sound_name)
	{
		PDGL_Sound_StopPlaying(chk->mesh->sound_chan);
		chk->mesh->sound_chan=0;
		chk->mesh->sound_name=NULL;
	}
}

void LBXGL_Voxel_RebuildRegionVisible_SortVisChksR(
	LBXGL_VoxelChunk **arr, int n, int rd)
{
	LBXGL_VoxelChunk *cur, *tc1, *tc2;
	int low, high, mid;
	float mp;
	int i, j, k;
	
	if(n<2)return;
	
	if(rd>=32)
	{
		for(i=0; i<n; i++)
			for(j=i+1; j<n; j++)
		{
			tc1=arr[i]; tc2=arr[j];
			if(tc1->tmp_dist>tc2->tmp_dist)
				{ arr[i]=tc2; arr[j]=tc1; }
		}
		return;
	}
	
	low=0; high=n;
	mid=(low+high)>>1;
	mp=arr[mid]->tmp_dist;
	
	i=low;
	while(i<high)
	{
		cur=arr[i];
		if(i<mp)
		{
			tc1=arr[low]; tc2=cur;
			arr[low]=tc2; arr[i]=tc1;
			low++; i++;
			continue;
		}
		if(i>mp)
		{
			high--;
			tc1=arr[high]; tc2=cur;
			arr[high]=tc2; arr[i]=tc1;
			continue;
		}
		i++;
	}
	
	LBXGL_Voxel_RebuildRegionVisible_SortVisChksR(arr, low, rd+1);
	LBXGL_Voxel_RebuildRegionVisible_SortVisChksR(arr+high, n-high, rd+1);
}

void LBXGL_Voxel_RebuildRegionVisible_SortVisChks(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *chkarr[8192];
	LBXGL_VoxelChunk *chk, *cur, *prv, *lst;
	int i, j, k, l, n;
	
	cur=rgn->vischk; n=0;
	while(cur)
	{
		chkarr[n++]=cur;
		cur=cur->mesh->vnext;
	}
	
	LBXGL_Voxel_RebuildRegionVisible_SortVisChksR(chkarr, n, 0);
	
	lst=NULL;
	for(i=n-1; i>=0; i--)
	{
		cur=chkarr[i];
		cur->mesh->vnext=lst;
		lst=cur;
	}
	rgn->vischk=lst;
}

void LBXGL_Voxel_RebuildRegionVisible_StatVisChunks(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *chk, *cur, *prv;
	int num_chk, num_achk;
	int tot_chktex, tot_chkface;
	int tot_chkatex, tot_chkaface;
	int i, j;

	if(btCvarGeti("vox_dumpstats")<=0)
		return;

	tot_chktex=0; tot_chkface=0;
	tot_chkatex=0; tot_chkaface=0;

	cur=rgn->vischk; num_chk=0;
	while(cur)
	{
		num_chk++;

		tot_chkface+=cur->mesh->n_face;
		for(i=0; i<cur->mesh->n_face; i++)
		{
			tot_chktex++;
			for(j=i+1; j<cur->mesh->n_face; j++)
				if(cur->mesh->face_tex[i]!=cur->mesh->face_tex[j])
					break;
			i=j-1;
		}
		
		cur=cur->mesh->vnext;
	}

	cur=rgn->avischk; num_achk=0;
	while(cur)
	{
		num_achk++;

		tot_chkaface+=cur->mesh->n_aface;
		for(i=0; i<cur->mesh->n_aface; i++)
		{
			tot_chkatex++;
			for(j=i+1; j<cur->mesh->n_aface; j++)
				if(cur->mesh->aface_tex[i]!=cur->mesh->aface_tex[j])
					break;
			i=j-1;
		}

		cur=cur->mesh->avnext;
	}
	
	printf("Voxel Stats(rgn=%p): %d vischk, %d avischk\n",
		rgn, num_chk, num_achk);
	
	if(num_chk)
	{
		printf("	%.2f face/chk, %.2f tex/chk\n",
			((float)tot_chkface)/num_chk,
			((float)tot_chktex)/num_chk);
	}
	if(num_achk)
	{
		printf("	%.2f face/achk, %.2f tex/achk\n",
			((float)tot_chkaface)/num_achk,
			((float)tot_chkatex)/num_achk);
	}
}

void LBXGL_Voxel_RebuildRegionVisible(
	LBXGL_BrushWorld *world, LBXGL_VoxelRegion *rgn)
{
	LBXGL_VoxelChunk *chk, *ccur, *cprv;
	LBXGL_VoxelChunk *rbdlst;
	int dl2, dtl;
	int i, j, k, l, n, m, cn;
	float f, g, vsc;

	if(!LBXGL_Voxel_CheckRegionFrustum(rgn))
	{
		rgn->lvischk=NULL;
		rgn->svischk=NULL;
		rgn->avischk=NULL;
		rgn->vischk=NULL;
		return;
	}

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

			if(!(lbxgl_state->doshot&32))
			{
				l=PDGL_TimeMS();
				if(l<lbxgl_voxel_chkstart)continue;
//				if(l>(lbxgl_voxel_chkstart+48))continue;
				if(l>(lbxgl_voxel_chkstart+24))continue;
//				if(l>(lbxgl_voxel_chkstart+12))continue;
			}

//			if(!LBXGL_Voxel_CheckRegionChunkFrustumDistance(
//				rgn, i, j, k, 7168*vsc))
			if(!LBXGL_Voxel_CheckRegionChunkFrustumDistance(
				rgn, i, j, k, 1.75*lbxgl_voxel_drawdist*vsc))
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
			//lbxgl_voxel_drawdist

			f=LBXGL_Voxel_GetChunkFrustumDistance(chk);
//			if(f>16384)
//			if(f>(12288*vsc))
//			if(f>(16384*vsc))
//			if(f>(1.5*lbxgl_voxel_drawdist*vsc))
			if(f>(2.5*lbxgl_voxel_drawdist*vsc))
			{
				LBXGL_Voxel_FlushChunkEmitSound(world, chk);
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
//				if((f<768) && chk->mesh->light)
				if((f<(0.2*lbxgl_voxel_drawdist)) && chk->mesh->light)
				{
					chk->mesh->lvnext=rgn->lvischk;
					rgn->lvischk=chk;
				}

//				if((f<2048) && chk->mesh->n_face)
				if((f<(0.5*lbxgl_voxel_drawdist)) && chk->mesh->n_face)
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
//			if((f<768) && chk->mesh->light)
			if((f<(0.2*lbxgl_voxel_drawdist)) && chk->mesh->light)
//			if(1)
			{
				chk->mesh->lvnext=rgn->lvischk;
				rgn->lvischk=chk;
			}

//			if((f<2048) && chk->mesh->n_face)
			if((f<(0.5*lbxgl_voxel_drawdist)) && chk->mesh->n_face)
			{
				chk->mesh->svnext=rgn->svischk;
				rgn->svischk=chk;
			}

//			if(f>1024)
//			if(f>512)
			if(f>(0.125*lbxgl_voxel_drawdist))
				{ chk->flags|=VOX_CHFL_DISTANT; }
			else
				{ chk->flags&=~VOX_CHFL_DISTANT; }
			
			if(lbxgl_state->doshot&32)
				{ chk->flags&=~VOX_CHFL_DISTANT; }
			
			if(f<(0.125*lbxgl_voxel_drawdist))
			{
				LBXGL_Voxel_CheckChunkEmitSound(world, chk);
			}

			if(f>(0.25*lbxgl_voxel_drawdist))
			{
				LBXGL_Voxel_FlushChunkEmitSound(world, chk);
			}
		}

//		LBXGL_Voxel_CheckRebuildChunk(chk);
		if(!chk->clean && !(chk->flags&VOX_CHFL_REBUILDING))
		{
//			if(lbxgl_voxel_chkleft<=0)
//				continue;
//			lbxgl_voxel_chkleft--;

#if 0
			if(chk->flags&VOX_CHFL_DISTANT)
			{
				if(lbxgl_voxel_chkleft<=0)
					continue;
				lbxgl_voxel_chkleft--;
			}
#endif

			if(!(lbxgl_state->doshot&32))
			{
				f=LBXGL_Voxel_CalcChunkCameraDistance(chk);
//				dl2=(log(f)/log(2))-10;
//				if(dl2<0)dl2=0;
//				dtl=48>>dl2;
//				f=1024/(f+1);
//				f=512/(f+1);
				f=2048/(f+1);
				f=f*f;
//				if(f>1)f=1;
				dtl=48*f;
				if(dtl<1)dtl=1;
//				if(dtl>48)dtl=48;
				if(dtl>24)dtl=24;

				l=PDGL_TimeMS();
				if(l<lbxgl_voxel_chkstart)continue;
				if(l>(lbxgl_voxel_chkstart+dtl))continue;

//				if(l>(lbxgl_voxel_chkstart+48))continue;
//				if(l>(lbxgl_voxel_chkstart+32))continue;
//				if(l>(lbxgl_voxel_chkstart+24))continue;
//				if(l>(lbxgl_voxel_chkstart+12))continue;
			}

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

//		if(chk->mesh->n_face && (chk->mesh->vbo_face<=0))
//			continue;
//		if(chk->mesh->n_aface && (chk->mesh->vbo_aface<=0))
//			continue;

		if(chk->mesh->n_aface)
		{
			world->flags|=BTGE_WFL_VOXFLUID;
		}

#if 0
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
		f=LBXGL_Voxel_CalcChunkCameraDistance(chk);
		chk->tmp_dist=f;
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

	LBXGL_Voxel_RebuildRegionVisible_SortVisChks(world, rgn);

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

	LBXGL_Voxel_RebuildRegionVisible_StatVisChunks(world, rgn);

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
	float f;
	int n;

	lbxgl_voxel_disablevbo=
		(btCvarGetf("r_disablevbo")!=0.0);

	f=btCvarGetf("r_drawdist");
	if(f<=0.0)f=4096;
	lbxgl_voxel_drawdist=f;

	cur=world->vox_region; prv=NULL; n=0;
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

		n++;
		LBXGL_Voxel_RebuildRegionVisible(world, cur);
		prv=cur; cur=cur->next;
	}

	if(btCvarGeti("vox_dumpstats")>0)
	{
		printf("%d visible regions\n\n", n);
	}

}
