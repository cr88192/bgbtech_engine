#include <btgecm.h>

BTGE_VoxelInfo *btge_voxinfo[4096];

BTGE_API void BTGE_VoxelSave_CheckImageSize(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int cells)
{
	int i;

	btge_voxel_lock();

	if(!rgn->img_buf)
	{
//		i=256;
		i=65536;
		while(i<=cells)i=i+(i>>1);
//		rgn->img_buf=gcatomic(i*16);
//		rgn->img_map=gcatomic(i);
		rgn->img_buf=gctatomic("btge_voxelregion_buffer", i*16);
		rgn->img_map=gctatomic("btge_voxelregion_map", i);
		rgn->img_m_cells=i;

		btge_voxel_unlock();
		return;
	}

	i=rgn->img_m_cells;
	while(i<=cells)i=i+(i>>1);
	rgn->img_buf=gcrealloc(rgn->img_buf, i*16);
	rgn->img_map=gcrealloc(rgn->img_map, i);
	rgn->img_m_cells=i;
	btge_voxel_unlock();
	return;
}

BTGE_API int BTGE_VoxelSave_TryAllocSpan(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int cells)
{
	int i, j;

	btge_voxel_lock();

	i=rgn->img_cell_rov;
	if(i<1)i=1;
	if((i+cells)>=rgn->img_m_cells)
	{
		btge_voxel_unlock();
		return(-1);
	}
	
//	i=1;
	while(i<rgn->img_m_cells)
	{
		for(j=0; (j<cells) && ((i+j)<rgn->img_m_cells); j++)
			{ if(rgn->img_map[i+j])break; }

		if(j>=cells)
		{
			for(j=0; j<cells; j++)
				{ rgn->img_map[i+j]=2; }
			rgn->img_map[i]=1;
			btge_voxel_unlock();
			return(i);
		}

		if((i+j)>=rgn->img_m_cells)
			break;

		i+=j;
		while(i<rgn->img_m_cells)
		{
			if(!rgn->img_map[i])
				break;
			i++;
		}
	}

	rgn->img_cell_rov=i;
	btge_voxel_unlock();
	return(-1);
}

BTGE_API int BTGE_VoxelSave_AllocSpan(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int cells)
{
	int i, j;

//	btge_voxel_lock();

//	if(!rgn->img_buf)
//		BTGE_VoxelSave_CheckImageSize(world, rgn, 16384+cells);
	
	i=BTGE_VoxelSave_TryAllocSpan(world, rgn, cells);
	if(i>0)
	{
//		btge_voxel_unlock();
		return(i);
	}

	rgn->img_cell_rov=1;
	i=BTGE_VoxelSave_TryAllocSpan(world, rgn, cells);
	if(i>0)
	{
//		btge_voxel_unlock();
		return(i);
	}

//	i=rgn->img_cell_rov;

	btge_voxel_lock();
	for(i=rgn->img_m_cells; i>0; i--)
	{
		if(rgn->img_map[i-1])
			break;
	}
	btge_voxel_unlock();

	//expand image and put at end
	BTGE_VoxelSave_CheckImageSize(world, rgn, i+cells);

	btge_voxel_lock();
	for(j=0; j<cells; j++)
		{ rgn->img_map[i+j]=2; }
	rgn->img_map[i]=1;
	btge_voxel_unlock();

	return(i);
}

BTGE_API void BTGE_VoxelSave_FreeSpan(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int base)
{
	int i;

	btge_voxel_lock();

	if(base>=rgn->img_m_cells)
	{
		btge_voxel_unlock();
		return;
	}

	if((rgn->img_map[base]&3)!=1)
	{
		btge_voxel_unlock();
		return;
	}

	rgn->img_map[base]=0;
	for(i=1; (base+i)<rgn->img_m_cells; i++)
	{
		if((rgn->img_map[base+i]&3)!=2)
			break;
		rgn->img_map[base+i]=0;
	}

	btge_voxel_unlock();
}

BTGE_API void BTGE_VoxelSave_MarkSpanUsed(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int base, int cells)
{
	int i;

	if(!base && !cells)
	{
		return;
	}

	btge_voxel_lock();

	if(base>=rgn->img_m_cells)
	{
		btge_voxel_unlock();
		return;
	}

	if((rgn->img_map[base]&3)==3)
	{
		while(((rgn->img_map[base]&3)==3) &&
			(cells>0) && (base<rgn->img_m_cells))
			{ base++; cells--; }

//		if(cells<=0)
//			return;
#if 1
		printf("BTGE_VoxelSave_MarkSpanUsed: Span Clash %d,%d\n",
			base, cells);
		for(i=0; i<cells; i++)
		{
			if((base+i)>=rgn->img_m_cells)
				break;
			rgn->img_map[base+i]=3;
		}
		btge_voxel_unlock();
		return;
#endif
	}

	if((rgn->img_map[base]&3)!=0)
	{
#if 1
		printf("BTGE_VoxelSave_MarkSpanUsed: Span Clash %d,%d\n",
			base, cells);
		for(i=0; i<cells; i++)
		{
			if((base+i)>=rgn->img_m_cells)
				break;
			rgn->img_map[base+i]=3;
		}
#endif
		btge_voxel_unlock();
		return;
	}

	rgn->img_map[base]=1;
	for(i=1; i<cells; i++)
	{
		if((base+i)>=rgn->img_m_cells)
			break;
		if((rgn->img_map[base+i]&3)!=0)
			continue;
//			break;
		rgn->img_map[base+i]=2;
	}

	btge_voxel_unlock();
}

BTGE_API bool BTGE_VoxelSave_CheckSpanSane(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int base, int cells)
{
	int i;

	if(base>=rgn->img_m_cells)
		return(false);
	if((base+cells)>=rgn->img_m_cells)
		return(false);
	return(true);

#if 0
	if((rgn->img_map[base]&3)!=1)
		return(false);

	for(i=1; i<cells; i++)
	{
		if((base+i)>=rgn->img_m_cells)
			return(false);
		if((rgn->img_map[base+i]&3)!=2)
			return(false);
	}
	return(true);
#endif
}

BTGE_API int BTGE_VoxelSave_GetSpanLength(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int base)
{
	int i;

	btge_voxel_lock();

	if((rgn->img_map[base]&3)!=1)
	{
		btge_voxel_unlock();
		return(0);
	}

	for(i=1; (base+i)<rgn->img_m_cells; i++)
	{
		if((rgn->img_map[base+i]&3)!=2)
			break;
	}
	btge_voxel_unlock();
	return(i);
}

BTGE_API bool BTGE_VoxelSave_FlattenRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *chk;
	byte *buf, *buf1, *ct, *cte;
	int i, j, k, l, n, rleofs;

	if(rgn->flags&VOX_CHFL_SAVE_OK)
		return(false);

	n=rgn->xs*rgn->ys*rgn->zs;
	for(i=0; i<n; i++)
	{
		chk=rgn->chk[i];
		if(!chk)continue;
		if(!(chk->flags&VOX_CHFL_SAVE_OK))
			break;
#if 0
		if(rgn->chk_offs && rgn->chk_offs[i])
		{
			k=rgn->chk_offs[i];
			ct=rgn->img_buf+k*16;
			if((ct[0]!=0xFF) || (ct[1]!=0x01))
				{ break; }
		}
#endif
	}
	if(i>=n)return(false);

#if 1
	if(rgn->img_map && (rgn->img_map[0]!=3))
	{
		printf("BTGE_VoxelSave_FlattenRegion: "
			"Bitmap Damaged\n");

		//attempt recovery
		for(i=0; i<64; i++)
			{ rgn->img_map[i]=3; }

		n=rgn->xs*rgn->ys*rgn->zs;
		for(i=0; i<n; i++)
		{
			j=rgn->chk_offs[i];
			k=rgn->chk_sz[i];
			l=(j+15)/16;

			if(j)
			{
				if(!BTGE_VoxelSave_CheckSpanSane(world, rgn, j, l))
				{
					printf("BTGE_VoxelSave_FlattenRegion: "
						"Can't Recover %d, Bad Span %d,%d\n", i, j, l);
					rgn->chk_offs[i]=0;
					rgn->chk_sz[i]=0;
					continue;
				}
				
				BTGE_VoxelSave_MarkSpanUsed(world, rgn, j, l);
			}
		}
	}
#endif

#if 1
	n=rgn->xs*rgn->ys*rgn->zs;
	for(i=0; i<n; i++)
	{
		chk=rgn->chk[i];
		if(!chk)continue;

		if(rgn->chk_offs && !rgn->chk_offs[i])
			{ chk->flags&=~VOX_CHFL_SAVE_OK; }

		if(rgn->chk_offs && rgn->chk_offs[i])
		{
			k=rgn->chk_offs[i];
			ct=rgn->img_buf+k*16;
			if((ct[0]!=0xFF) || (ct[1]!=0x01))
			{
				printf("BTGE_VoxelSave_FlattenRegion: "
					"Pre Flatten, Corrupt %d @%08X\n", i, k*16);

				chk->flags&=~VOX_CHFL_SAVE_OK;
//				BTGE_VoxelSave_FreeSpan(world, rgn, rgn->chk_offs[i]);
				rgn->chk_offs[i]=0;
			}
		}
	}
#endif

	if(world->HandleSaveRegion)
		{ world->HandleSaveRegion(world, rgn); }

	if(!rgn->img_buf)
	{
		BTGE_VoxelSave_CheckImageSize(world, rgn, 16384);

		btge_voxel_lock();
//		for(i=0; i<8; i++)
		for(i=0; i<64; i++)
		{
			rgn->img_map[i]=3;
		}
		btge_voxel_unlock();
	}

	btge_voxel_lock();
	if(!rgn->chk_offs)
	{
		rgn->chk_offs=gcatomic(rgn->xs*rgn->ys*rgn->zs*sizeof(int));
		rgn->chk_sz=gcatomic(rgn->xs*rgn->ys*rgn->zs*sizeof(int));
	}
	btge_voxel_unlock();

	buf=malloc(1<<18);
	cte=buf+(1<<18);

	n=rgn->xs*rgn->ys*rgn->zs;
	for(i=0; i<n; i++)
	{
		chk=rgn->chk[i];
		if(!chk)continue;
		if(chk->flags&VOX_CHFL_SAVE_OK)
		{
#if 1
			btge_voxel_lock();
			if(chk->tick_inhibit_voxrle>0)
			{
				chk->tick_inhibit_voxrle--;
			}else
			{
				if(chk->voxrle && chk->vox)
				{
					gcfree(chk->vox);
					chk->vox=NULL;
				}

				if(chk->voxrle && chk->pidx_vox)
				{
					gcfree(chk->pidx_vox);
					gcfree(chk->pidx_idx);
					chk->pidx_vox=NULL;
					chk->pidx_idx=NULL;
				}
			}
			btge_voxel_unlock();
#endif
			continue;
		}

#if 0
		if(rgn->chk_offs[i])
		{
			BTGE_VoxelSave_FreeSpan(world, rgn, rgn->chk_offs[i]);
			rgn->chk_offs[i]=0;
		}
#endif

		BTGE_Voxel_PackVoxelIndexChunk(chk);
	
//		buf1=BTGE_Voxel_DeltaChunkBuffer(world, chk, buf, 1<<17);
		ct=BTGE_Voxel_FlattenChunkBuffer(world, chk, buf, cte-buf);
//		chk->voxrle=chk->tmprle;
		rleofs=chk->tmprle-buf;
		chk->sz_voxrle=ct-chk->tmprle;
		
		if(world->EncodeChunkContents)
			{ ct=world->EncodeChunkContents(world, chk, ct, cte-ct); }
		
		j=ct-buf;
		if(j<=0)continue;

		if(rgn->chk_offs[i])
		{
			k=rgn->chk_offs[i];
			l=BTGE_VoxelSave_GetSpanLength(world, rgn, k);
			
			if(j>(l*16))
			{
				BTGE_VoxelSave_FreeSpan(world, rgn, rgn->chk_offs[i]);
				rgn->chk_offs[i]=0;

				l=(j+15)/16;
				k=BTGE_VoxelSave_AllocSpan(world, rgn, l);
				if(k<=0)continue;
			}
		}else
		{
			l=(j+15)/16;
			k=BTGE_VoxelSave_AllocSpan(world, rgn, l);
			if(k<=0)continue;
		}
		
		btge_voxel_lock();
		memcpy(rgn->img_buf+k*16, buf, j);
		rgn->chk_offs[i]=k;
		rgn->chk_sz[i]=j;
		chk->flags|=VOX_CHFL_SAVE_OK;
//		chk->voxrle=(rgn->img_buf+k*16)+rleofs;
		chk->voxrle=k*16+rleofs;
		btge_voxel_unlock();

//		if((rgn->img_buf+chk->voxrle)[0]!=1)
//		{
//			*(int *)-1=-1;
//		}
	}
	
	ct=buf;
	for(i=0; i<n; i++)
	{
		ct=BTGE_Voxel_EmitVLI(ct, rgn->chk_offs[i]);
		ct=BTGE_Voxel_EmitVLI(ct, rgn->chk_sz[i]);
	}

	j=ct-buf;
	k=rgn->img_idx_offs;

	l=BTGE_VoxelSave_GetSpanLength(world, rgn, k);
	if(j>(l*16))
	{
		BTGE_VoxelSave_FreeSpan(world, rgn, k);
		l=(j+15)/16;
		k=BTGE_VoxelSave_AllocSpan(world, rgn, l);
	}

	btge_voxel_lock();
	memcpy(rgn->img_buf+k*16, buf, j);
	rgn->img_idx_offs=k;
	rgn->img_idx_sz=j;
	btge_voxel_unlock();

	if(world->EncodeRegionContents)
	{
		ct=world->EncodeRegionContents(world, rgn, buf, cte-buf);

		BTGE_VoxelSave_FreeSpan(world, rgn, rgn->img_cont_offs);

		j=ct-buf;
		l=(j+15)/16;
		k=BTGE_VoxelSave_AllocSpan(world, rgn, l);

		btge_voxel_lock();
		memcpy(rgn->img_buf+k*16, buf, j);
		rgn->img_cont_offs=k;
		rgn->img_cont_sz=j;
		btge_voxel_unlock();
	}else
	{
		rgn->img_cont_offs=0;
		rgn->img_cont_sz=0;
	}

#if 1
	buf1=malloc(1<<16);

	ct=buf;
	for(i=1; i<rgn->n_values; i++)
	{
//		ct=BTGE_Voxel_EmitVLI(ct, rgn->chk_offs[i]);
//		ct=BTGE_Voxel_EmitVLI(ct, rgn->chk_sz[i]);

		if(!rgn->values[i])
			continue;

		ct=BTGE_Voxel_EmitVLI(ct, i);
		dysDumpStr(buf1, rgn->values[i]);
		ct=BTGE_Voxel_EmitString(ct, buf1);
	}
	ct=BTGE_Voxel_EmitVLI(ct, 0);
	*ct++=0;

	free(buf1);

	j=ct-buf;
	k=rgn->img_vals_offs;

	l=BTGE_VoxelSave_GetSpanLength(world, rgn, k);
	if(j>(l*16))
	{
		BTGE_VoxelSave_FreeSpan(world, rgn, k);
		l=(j+15)/16;
		k=BTGE_VoxelSave_AllocSpan(world, rgn, l);
	}

//	BTGE_VoxelSave_FreeSpan(world, rgn, rgn->img_vals_offs);
//	j=ct-buf;
//	l=(j+15)/16;
//	k=BTGE_VoxelSave_AllocSpan(world, rgn, l);

	btge_voxel_lock();
	memcpy(rgn->img_buf+k*16, buf, j);
	rgn->img_vals_offs=k;
	rgn->img_vals_sz=j;
	btge_voxel_unlock();
#endif

#if 1
	ct=buf;
	for(i=1; i<4096; i++)
	{
		if(!btge_voxinfo[i])
			continue;

		ct=BTGE_Voxel_EmitVLI(ct, i);
		ct=BTGE_Voxel_EmitString(ct, btge_voxinfo[i]->name);
	}
	ct=BTGE_Voxel_EmitVLI(ct, 0);
	*ct++=0;

	j=ct-buf;
	k=rgn->img_voxty_offs;
	l=BTGE_VoxelSave_GetSpanLength(world, rgn, k);
	if(j>(l*16))
	{
		BTGE_VoxelSave_FreeSpan(world, rgn, k);
		l=(j+15)/16;
		k=BTGE_VoxelSave_AllocSpan(world, rgn, l);
	}

//	BTGE_VoxelSave_FreeSpan(world, rgn, rgn->img_vals_offs);
//	j=ct-buf;
//	l=(j+15)/16;
//	k=BTGE_VoxelSave_AllocSpan(world, rgn, l);

	btge_voxel_lock();
	memcpy(rgn->img_buf+k*16, buf, j);
	rgn->img_voxty_offs=k;
	rgn->img_voxty_sz=j;
	btge_voxel_unlock();
#endif

#if 1
	ct=BTGE_Voxel_FlattenRegionSEntityListBuffer(
		world, rgn, rgn->sentity, buf, 1<<18);

	j=ct-buf;
	k=rgn->img_sent_offs;
	l=BTGE_VoxelSave_GetSpanLength(world, rgn, k);
	if(j>(l*16))
	{
		BTGE_VoxelSave_FreeSpan(world, rgn, k);
		l=(j+15)/16;
		k=BTGE_VoxelSave_AllocSpan(world, rgn, l);
	}

	btge_voxel_lock();
	memcpy(rgn->img_buf+k*16, buf, j);
	rgn->img_sent_offs=k;
	rgn->img_sent_sz=j;
	btge_voxel_unlock();
#endif

	free(buf);

	ct=rgn->img_buf;
	*ct++='B'; *ct++='T'; *ct++='G'; *ct++='E';
	*ct++='R'; *ct++='G'; *ct++='N'; *ct++='1';

	//region size
	ct=BTGE_Voxel_EmitVLI(ct, rgn->xs);
	ct=BTGE_Voxel_EmitVLI(ct, rgn->ys);
	ct=BTGE_Voxel_EmitVLI(ct, rgn->zs);
	ct=BTGE_Voxel_EmitVLI(ct, 0);

	//voxel chunks
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_idx_offs);
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_idx_sz);

	//entities
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_cont_offs);
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_cont_sz);
	
	//values
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_vals_offs);
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_vals_sz);
	
	//voxel type data
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_voxty_offs);
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_voxty_sz);

	//voxel type data
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_sent_offs);
	ct=BTGE_Voxel_EmitVLI(ct, rgn->img_sent_sz);

	rgn->flags|=VOX_CHFL_SAVE_OK;

#if 1
	n=rgn->xs*rgn->ys*rgn->zs;
	for(i=0; i<n; i++)
	{
		chk=rgn->chk[i];
		if(!chk)continue;

		if(rgn->chk_offs && rgn->chk_offs[i])
		{
			k=rgn->chk_offs[i];
			ct=rgn->img_buf+k*16;
			if((ct[0]!=0xFF) || (ct[1]!=0x01))
			{
				rgn->flags&=~VOX_CHFL_SAVE_OK;

				printf("BTGE_VoxelSave_FlattenRegion: "
					"Post Flatten, Corrupt %d @%08X\n", i, k*16);
				chk->flags&=~VOX_CHFL_SAVE_OK;
//				BTGE_VoxelSave_FreeSpan(world, rgn, rgn->chk_offs[i]);
				rgn->chk_offs[i]=0;
			}
		}else if(rgn->chk_offs)
		{
				rgn->flags&=~VOX_CHFL_SAVE_OK;
				printf("BTGE_VoxelSave_FlattenRegion: "
					"Post Flatten, Missing %d\n", i);
				chk->flags&=~VOX_CHFL_SAVE_OK;
		}
	}
#endif

#if 0
	j=0;
	for(i=0; i<rgn->img_m_cells; i++)
	{
		if(rgn->img_map[i])
			j=i+1;
	}
	rgn->img_n_cells=j;
#endif

#if 0
	for(i=(rgn->img_m_cells-1); i>0; i--)
		{ if(rgn->img_map[i])break; }
	rgn->img_n_cells=i+1;
#endif

#if 1
	j=rgn->img_n_cells;
	for(i=(rgn->img_m_cells-1); i>j; i--)
	{
		if(rgn->img_map[i])
			{ rgn->img_n_cells=i+1; break; }
	}
#endif

	//don't save if region didn't flatten ok.
	if(!(rgn->flags&VOX_CHFL_SAVE_OK))
		return(false);

//	rgn->flags|=VOX_CHFL_SAVE_OK;

//	printf("BTGE_VoxelSave_FlattenRegion: %d bytes (%d/%d cells)\n",
//		16*rgn->img_n_cells, rgn->img_n_cells, rgn->img_m_cells);
	return(true);
}

BTGE_API void BTGE_Voxel_FlattenWorld(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur;
	int i, j, sz;

	cur=world->vox_region;
	while(cur)
	{
		BTGE_VoxelSave_FlattenRegion(world, cur);
		cur=cur->next;
	}
}

BTGE_API char *BTGE_VoxelSave_GetMapBase(BTGE_BrushWorld *world)
{
	char tb[256];
	char *s;

	strcpy(tb, world->map_name);
	s=tb+strlen(tb);
	while((s>tb) && (*s!='.') && (*s!='/'))s--;
	if(*s=='.')*s=0;
	while((s>tb) && (*s!='/'))s--;
	if(*s=='/')s++;
	return(gcrstrdup(s));
}

BTGE_API char *BTGE_VoxelSave_GetRegionName(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	char tb[256];
	char *s;
	int rx, ry, rz;

	rx=rgn->ox/(rgn->xs*rgn->xst);
	ry=rgn->oy/(rgn->ys*rgn->yst);
	rz=rgn->oz/(rgn->zs*rgn->zst);
	s=BTGE_VoxelSave_GetMapBase(world);
	sprintf(tb, "region/%s_%dx%dx%d.rgn", s, rx, ry, rz);

	return(gcrstrdup(tb));
}

BTGE_API char *BTGE_VoxelSave_GetRegionTagName(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, char *tag)
{
	char tb[256];
	char *s;
	int rx, ry, rz;

	rx=rgn->ox/(rgn->xs*rgn->xst);
	ry=rgn->oy/(rgn->ys*rgn->yst);
	rz=rgn->oz/(rgn->zs*rgn->zst);
	s=BTGE_VoxelSave_GetMapBase(world);
	sprintf(tb, "region/%s_%s_%dx%dx%d.rgn", s, tag, rx, ry, rz);

	return(gcrstrdup(tb));
}

BTGE_API char *BTGE_VoxelSave_GetRegionLockName(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	return(BTGE_VoxelSave_GetRegionTagName(world, rgn, "lock"));
}

BTGE_API char *BTGE_VoxelSave_GetRegionBaseName(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	return(BTGE_VoxelSave_GetRegionTagName(world, rgn, "base"));
}

BTGE_API bool BTGE_Voxel_WorldIsServerP(BTGE_BrushWorld *world)
{
	if(world->ents)return(true);
	return(false);
}

BTGE_API bool BTGE_Voxel_WorldSaveDisabledP(BTGE_BrushWorld *world)
{
	int i;

	i=BTGE_CvarGetNum("vox_savedisabled");
	if(i)return(true);

	if(world->flags&BTGE_WFL_NOSAVEVOXELWORLD)
		return(true);

	return(false);

//	if(world->ents)return(true);
//	return(false);
}

BTGE_API bool BTGE_VoxelSave_SaveRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	char tb[256];
	char *s;
	int rx, ry, rz;
	bool b;

	b=BTGE_VoxelSave_FlattenRegion(world, rgn);
	if(!b)return(false);
	
	if(!rgn->img_fd)
	{
		if(!BTGE_Voxel_WorldIsServerP(world))
			return(false);
		
		if(BTGE_Voxel_WorldSaveDisabledP(world))
			return(false);
	
		rx=rgn->ox/(rgn->xs*rgn->xst);
		ry=rgn->oy/(rgn->ys*rgn->yst);
		rz=rgn->oz/(rgn->zs*rgn->zst);
		s=BTGE_VoxelSave_GetMapBase(world);
		sprintf(tb, "region/%s_%dx%dx%d.rgn", s, rx, ry, rz);
//		sprintf(tb, "region/%s_%dx%dx%d.rgn", world->map_name, rx, ry, rz);
		rgn->img_fd=vffopen(tb, "w+b");
		
		printf("BTGE_VoxelSave_SaveRegion: %s %s\n",
			tb, rgn->img_fd?"OK":"Fail");
	}
	
	if(!rgn->img_fd || (rgn->img_fd==UNDEFINED))
		return(false);

	if(BTGE_Voxel_WorldSaveDisabledP(world))
		return(false);

//	b=BTGE_VoxelSave_FlattenRegion(world, rgn);
//	if(!b)return(false);

	vfseek(rgn->img_fd, 0, 0);
	vfwrite(rgn->img_buf, 16, rgn->img_n_cells, rgn->img_fd);
	return(true);
}

BTGE_API void BTGE_Voxel_SaveWorld(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur;
	int i, j, sz;

	cur=world->vox_region;
	while(cur)
	{
		BTGE_VoxelSave_SaveRegion(world, cur);
		cur=cur->next;
	}
}

BTGE_API bool BTGE_VoxelSave_TrySaveRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	bool b;

	if(!rgn->img_fd || (rgn->img_fd==UNDEFINED))
		return(false);

	b=BTGE_VoxelSave_FlattenRegion(world, rgn);
	if(!b)return(false);
	
	if(!rgn->img_fd || (rgn->img_fd==UNDEFINED))
		return(false);
	if(BTGE_Voxel_WorldSaveDisabledP(world))
		return(false);

	vfseek(rgn->img_fd, 0, 0);
	vfwrite(rgn->img_buf, 16, rgn->img_n_cells, rgn->img_fd);
	return(true);
}

BTGE_API void BTGE_Voxel_TrySaveWorld(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur;
	int i, j, sz;

	cur=world->vox_region;
	while(cur)
	{
		BTGE_VoxelSave_TrySaveRegion(world, cur);
		cur=cur->next;
	}
}

BTGE_API void BTGE_VoxelSave_LoadRegionChunk(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int cn)
{
	static volatile BTGE_VoxelRegion *gbl_rgn=NULL;
	BTGE_VoxelChunk *chk;
	byte *ibuf, *cs, *cse;
	int offs, sz;

	if(!world)
		return;
	if(!rgn->chk_offs)
		return;
	if(!rgn->chk_offs[cn])
		return;

	btge_voxel_lock();
	if(gbl_rgn)
	{
		btge_voxel_unlock();
		return;
	}
	gbl_rgn=rgn;
	btge_voxel_unlock();

//	if(gcCheckMemoryLow())
//	{
//		return;
//	}

	offs=rgn->chk_offs[cn];
	sz=rgn->chk_sz[cn];
	ibuf=rgn->img_buf+(offs*16);
	cs=ibuf; cse=ibuf+sz;
//	cs=BTGE_Voxel_DecodeDeltaChunkBuffer(world, cs, sz);
	cs=BTGE_Voxel_DecodeLoadChunkBuffer(world, cs, sz);

	if(cs==ibuf)
	{
		printf("BTGE_VoxelSave_LoadRegionChunk: "
			"Deleted Bad Chunk %d\n", cn);
		rgn->chk_offs[cn]=0;
		rgn->chk_sz[cn]=0;
		gbl_rgn=NULL;
		return;
	}

	chk=rgn->chk[cn];

	if(!chk)
	{
#if 0
		printf("BTGE_VoxelSave_LoadRegionChunk: "
			"Decoded Chunk Absent, Deleting %d\n", cn);
		rgn->chk_offs[cn]=0;
		rgn->chk_sz[cn]=0;
#endif
		gbl_rgn=NULL;
		return;
	}

	if(chk && world->DecodeChunkContents)
		{ cs=world->DecodeChunkContents(world, chk, cs, cse-cs); }
		
	if(chk)
	{
		btge_voxel_lock();
		chk->voxrle=chk->tmprle-rgn->img_buf;
		chk->sz_voxrle=cs-chk->tmprle;
		btge_voxel_unlock();
	
//		chk->flags|=VOX_CHFL_LIGHT_DIRTY;
	}

	btge_voxel_lock();
	gbl_rgn=NULL;
	btge_voxel_unlock();
}

BTGE_API void BTGE_VoxelSave_LoadRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	VFILE *fd, *fd1;
	char *fn, *fn1;
	byte *cs, *cse;
	char *s;
	dyt v;
	int xs, ys, zs, fl;
	int i, j, k, l, n, sz;

	if(rgn->img_fd || (rgn->img_fd==UNDEFINED))
		return;

	fn=BTGE_VoxelSave_GetRegionName(world, rgn);
	fd=vffopen(fn, "r+b");
	rgn->img_fd=fd;
	printf("BTGE_VoxelSave_LoadRegion: %s %s\n",
		fn, rgn->img_fd?"OK":"Fail");

	if(!fd)
	{
		fn=BTGE_VoxelSave_GetRegionLockName(world, rgn);
		fd=vffopen(fn, "rb");
		rgn->img_fd=UNDEFINED;
		printf("BTGE_VoxelSave_LoadRegion: %s %s (Locked)\n",
			fn, fd?"OK":"Fail");
	}

	if(!fd)
	{
		fn=BTGE_VoxelSave_GetRegionBaseName(world, rgn);
		fd=vffopen(fn, "rb");
		rgn->img_fd=UNDEFINED;
		printf("BTGE_VoxelSave_LoadRegion: %s %s (Base)\n",
			fn, fd?"OK":"Fail");

		if(!BTGE_Voxel_WorldSaveDisabledP(world))
		{
			fn1=BTGE_VoxelSave_GetRegionName(world, rgn);
			fd1=vffopen(fn1, "w+b");
			if(fd1)rgn->img_fd=fd1;
		}else
		{
			rgn->img_fd=NULL;
		}
	}

#if 0
	if(!rgn->img_fd)
		return;

	vfseek(rgn->img_fd, 0, 2);
	sz=vftell(rgn->img_fd);
	
	n=(sz+15)/16;
	BTGE_VoxelSave_CheckImageSize(world, rgn, n);
	for(i=0; i<8; i++)
		{ rgn->img_map[i]=3; }

	vfseek(rgn->img_fd, 0, 0);
	vfread(rgn->img_buf, 16, n, rgn->img_fd);
#endif

#if 1
	if(!fd)
		return;

	vfseek(fd, 0, 2);
	sz=vftell(fd);
	
	n=(sz+15)/16;
	BTGE_VoxelSave_CheckImageSize(world, rgn, n);
//	for(i=0; i<8; i++)
	for(i=0; i<64; i++)
		{ rgn->img_map[i]=3; }

	vfseek(fd, 0, 0);
	vfread(rgn->img_buf, 16, n, fd);
#endif

	cs=rgn->img_buf;
	if(!memcmp(cs, "BTGERGN0", 8))
	{
		printf("BTGE_VoxelSave_LoadRegion: Sig OK\n");
		cs+=8;
		
		xs=32; ys=32; zs=8; fl=0;
	}else if(!memcmp(cs, "BTGERGN1", 8))
	{
		printf("BTGE_VoxelSave_LoadRegion: Sig OK\n");
		cs+=8;

		cs=BTGE_Voxel_DecodeVLI(cs, &xs);
		cs=BTGE_Voxel_DecodeVLI(cs, &ys);
		cs=BTGE_Voxel_DecodeVLI(cs, &zs);
		cs=BTGE_Voxel_DecodeVLI(cs, &fl);
	}else
	{
		printf("BTGE_VoxelSave_LoadRegion: Sig Fail\n");
		return;
	}

	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_idx_offs));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_idx_sz));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_cont_offs));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_cont_sz));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_vals_offs));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_vals_sz));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_voxty_offs));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_voxty_sz));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_sent_offs));
	cs=BTGE_Voxel_DecodeVLI(cs, &(rgn->img_sent_sz));

	j=rgn->img_idx_offs;
	k=(rgn->img_idx_sz+15)/16;
	BTGE_VoxelSave_MarkSpanUsed(world, rgn, j, k);

	if(rgn->img_cont_offs)
	{
		j=rgn->img_cont_offs;
		k=(rgn->img_cont_sz+15)/16;
		BTGE_VoxelSave_MarkSpanUsed(world, rgn, j, k);
	}

	if(rgn->img_vals_offs)
	{
		j=rgn->img_vals_offs;
		k=(rgn->img_vals_sz+15)/16;
		BTGE_VoxelSave_MarkSpanUsed(world, rgn, j, k);
	}

	if(rgn->img_voxty_offs)
	{
		j=rgn->img_voxty_offs;
		k=(rgn->img_voxty_sz+15)/16;
		BTGE_VoxelSave_MarkSpanUsed(world, rgn, j, k);
	}

	if(rgn->img_sent_offs)
	{
		j=rgn->img_sent_offs;
		k=(rgn->img_sent_sz+15)/16;
		BTGE_VoxelSave_MarkSpanUsed(world, rgn, j, k);
	}

//	n=rgn->xs*rgn->ys*rgn->zs;
//	n=xs*ys*zs;

	if((xs==rgn->xs) && (ys==rgn->ys) && (zs==rgn->zs))
//	if(1)
	{
		n=rgn->xs*rgn->ys*rgn->zs;

		if(!rgn->chk_offs)
		{
			rgn->chk_offs=gcatomic(n*sizeof(int));
			rgn->chk_sz=gcatomic(n*sizeof(int));
		}

		cs=rgn->img_buf+(rgn->img_idx_offs*16);
		for(i=0; i<n; i++)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &j);
			cs=BTGE_Voxel_DecodeVLI(cs, &k);
			l=(k+15)/16;

			if(j && !BTGE_VoxelSave_CheckSpanSane(world, rgn, j, l))
			{
				printf("BTGE_VoxelSave_LoadRegion: Non-Sane Chunk Span "
					"Chunk=%d, Base=%d Size=%d\n",
						i, j*16, k);
				rgn->chk_offs[i]=0;
				rgn->chk_sz[i]=0;
				continue;
			}
		
			rgn->chk_offs[i]=j;
			rgn->chk_sz[i]=k;
			BTGE_VoxelSave_MarkSpanUsed(world, rgn, j, l);
		}
	}else
	{
		n=xs*ys*zs;

		cs=rgn->img_buf+(rgn->img_idx_offs*16);
		for(i=0; i<n; i++)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &j);
			cs=BTGE_Voxel_DecodeVLI(cs, &k);
			l=(k+15)/16;
			
			BTGE_Voxel_DecodeDeltaChunkBuffer(world, rgn->img_buf+j*16, k);
		}
	}

	cs=rgn->img_buf+(rgn->img_vals_offs*16);
	cse=cs+rgn->img_vals_sz;
	while(cs<cse)
	{
		cs=BTGE_Voxel_DecodeVLI(cs, &i);
		if(i<=0)break;
		s=(char *)cs;
		cs+=strlen(s)+1;
		
		v=dysParseFromString(s);
		BTGE_Voxel_WorldRegionValueCheckIndex(world, rgn, i);
		rgn->values[i]=v;
	}

	if(world->DecodeRegionContents)
	{
		cs=rgn->img_buf+(rgn->img_cont_offs*16);
		k=rgn->img_cont_sz;
		world->DecodeRegionContents(world, rgn, cs, k);
	}
	
	if(rgn->img_sent_offs && rgn->img_sent_sz)
	{
		cs=rgn->img_buf+(rgn->img_sent_offs*16);
		k=rgn->img_sent_sz;

		BTGE_Voxel_DecodeRegionSEntityListBuffer(world, rgn, cs, k);
	}

	if(world->HandleLoadRegion)
		{ world->HandleLoadRegion(world, rgn); }
}

BTGE_API char *BTGE_VoxelSave_GetTerrainName(
	BTGE_BrushWorld *world)
{
	char tb[256];
	char *s;

	s=BTGE_VoxelSave_GetMapBase(world);
	sprintf(tb, "region/%s_terr.txt", s);

	return(gcrstrdup(tb));
}

BTGE_API void BTGE_VoxelSave_LoadTerrain(
	BTGE_BrushWorld *world)
{
}


BTGE_API void BTGE_Voxel_FreeChunk(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_Voxel_FlushReleaseChunk(chk);
//	if(chk->vox)
//		gcfree(chk->vox);
	if(chk->vox)
		{ gcfree(chk->vox); chk->vox=NULL; }
	if(chk->pidx_vox)
		{ gcfree(chk->pidx_vox); chk->pidx_vox=NULL; }
	if(chk->pidx_idx)
		{ gcfree(chk->pidx_idx); chk->pidx_idx=NULL; }
	gcfree(chk);
}

BTGE_API void BTGE_Voxel_FreeRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *chk;
	int i, j, k, l, n, m;

	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
	{
		for(k=0; k<rgn->zs; k++)
		{
			l=rgn->zs-k-1;
			chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+l];
			if(!chk)continue;
			BTGE_Voxel_FreeChunk(world, chk);
		}
	}
	
	if(rgn->img_fd)
	{
		vfclose(rgn->img_fd);
	}

	if(rgn->chk_offs)
	{
		gcfree(rgn->chk_offs);
		gcfree(rgn->chk_sz);
	}

	if(rgn->img_buf)
	{
		gcfree(rgn->img_buf);
		gcfree(rgn->img_map);
	}

	gcfree(rgn->chk);
	gcfree(rgn);
}

BTGE_API void BTGE_Voxel_FreeWorld(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur, *nxt;
	int i, j, sz;

	cur=world->vox_region;
	while(cur)
	{
		nxt=cur->next;
		BTGE_Voxel_FreeRegion(world, cur);
		cur=nxt;
	}

	world->vox_region=NULL;
	world->vox_terr=NULL;
}

BTGE_API void BTGE_Voxel_WorldTeardown(BTGE_BrushWorld *world)
{
	BTGE_Voxel_TrySaveWorld(world);
	BTGE_Voxel_FreeWorld(world);
}
