#include <btgecm.h>

char *btge_voxnames_cache[4096];
BTGE_VoxelInfo *btge_voxinfo[4096];
BTGE_VoxelInfo *btge_oldvoxinfo[4096];

#if 0
int BTGE_Voxel_EncodeBufferRLE(byte *obuf, byte *ibuf, int isz)
{
	byte *cs, *cse, *ct, *cte;
	byte *cs1, *cs1e;
	int i;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+2*isz;
	while((cs<cse) && (ct<cte))
	{
		cs1=cs+1; cs1e=cs+64;
		if(cs1e>cse)cs1e=cse;
		while((*cs1==*cs) && (cs1<cs1e))cs1++;
		i=(cs1-cs);
		if((i>2) || (*cs>=0xC0))
		{
			if(i>63)i=63;
			*ct++=0xC0+i;
			*ct++=*cs;
			cs+=i;
			continue;
		}
		*ct++=*cs++;
	}
	
	if(cs<cse)return(-1);
	return(ct-obuf);
}

int BTGE_Voxel_DecodeBufferRLE(
	byte *obuf, int osz, byte *ibuf, int isz)
{
	byte *cs, *cse, *ct, *cte;
	byte *cs1, *cs1e;
	int i, j;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	while((cs<cse) && (ct<cte))
	{
		if((*cs)>=0xC0)
		{
			i=(*cs++)-0xC0;
			j=*cs++;
			while((i--) && (ct<cte))
				*ct++=j;
			continue;
		}
		
		*ct++=*cs++;
	}
	
	return(cs-ibuf);
}
#endif

#if 1
int BTGE_Voxel_EncodeBufferRLE(byte *obuf, byte *ibuf, int isz)
{
	byte *cs, *cse, *ct, *cte;
	byte *cs1, *cs1e;
	int i;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+2*isz;
	while((cs<cse) && (ct<cte))
	{
		cs1=cs+1; cs1e=cs+256;
		if(cs1e>cse)cs1e=cse;
		while((*cs1==*cs) && (cs1<cs1e))cs1++;
		i=(cs1-cs);
//		if((i>3) || (*cs==0xFE))
#if 1
		if(i>255)
		{
			if(i>65535)i=65535;
			*ct++=0xFE;		*ct++=0; *ct++=*cs;
			*ct++=i&0xFF;	*ct++=(i>>8)&0xFF;
			cs+=i;
			continue;
		}
#endif
		if(i>3)
		{
			if(i>255)i=255;
			*ct++=0xFE;
			*ct++=i;
			*ct++=*cs;
			cs+=i;
			continue;
		}
		if((*cs==0xFE) || (*cs==0xFF))
		{
			*ct++=0xFE;
			*ct++=1;
			*ct++=*cs++;
			continue;
		}
		*ct++=*cs++;
	}
	
	if(cs<cse)return(-1);
	return(ct-obuf);
}

int BTGE_Voxel_DecodeBufferRLE(
	byte *obuf, int osz, byte *ibuf, int isz)
{
	byte *cs, *cse, *ct, *cte;
	byte *cs1, *cs1e;
	int i, j;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	while((cs<cse) && (ct<cte))
	{
//		if((*cs)>=0xC0)
		if((*cs)==0xFE)
		{
			cs++;
			i=*cs++;
			j=*cs++;
			if(i==0)
				{ i=*cs++; i|=(*cs++)<<8; }
			while((i--) && (ct<cte))
				*ct++=j;
			continue;
		}
		
		*ct++=*cs++;
	}
	
	return(cs-ibuf);
}
#endif

#if 0
int BTGE_Voxel_EncodeBufferRLE(byte *obuf, byte *ibuf, int isz)
{
	byte *cs, *cse, *ct, *cte;
	byte *cs1, *cs1e;
	int i;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+2*isz;
	while((cs<cse) && (ct<cte))
	{
		cs1=cs+1; cs1e=cs+256;
		if(cs1e>cse)cs1e=cse;
		while((*cs1==*cs) && (cs1<cs1e))cs1++;
		i=(cs1-cs);
//		if((i>3) || (*cs==0xFE))
#if 1
		if(i>254)
		{
			if(i>65535)i=65535;
			*ct++=0xFE;		*ct++=0; *ct++=*cs;
			*ct++=i&0xFF;	*ct++=(i>>8)&0xFF;
			cs+=i;
			continue;
		}
#endif
		if(i>3)
		{
			if(i>254)i=254;
			*ct++=0xFE;
			*ct++=i;
			*ct++=*cs;
			cs+=i;
			continue;
		}
//		if(*cs==0xFE)
		if((*cs==0xFE) || (*cs==0xFF))
		{
			*ct++=0xFE;
			*ct++=1;
			*ct++=*cs++;
			continue;
		}
		if(*cs==0xFF)
		{
			*ct++=0xFF;
			*ct++=0x00;
			continue;
		}
		*ct++=*cs++;
	}
	
	if(cs<cse)return(-1);
	return(ct-obuf);
}

int BTGE_Voxel_DecodeBufferRLE(
	byte *obuf, int osz, byte *ibuf, int isz)
{
	byte *cs, *cse, *ct, *cte;
	byte *cs1, *cs1e;
	int i, j;
	
	cs=ibuf; cse=ibuf+isz;
	ct=obuf; cte=obuf+osz;
	while((cs<cse) && (ct<cte))
	{
//		if((*cs)>=0xC0)
		if((*cs)==0xFE)
		{
			cs++;
			i=*cs++;
			j=*cs++;
			if(i==0)
				{ i=*cs++; i|=(*cs++)<<8; }
			while((i--) && (ct<cte))
				*ct++=j;
			continue;
		}

		if((*cs)==0xFF)
		{
			if(cs[1]==0x00)
			{
				cs+=2;
				*ct++=0xFF;
				continue;
			}
			break;
		}
		
		*ct++=*cs++;
	}
	
	return(cs-ibuf);
}
#endif

byte *BTGE_Voxel_EmitVLI(byte *ct, int val)
{
	if(val<0)
	{
		*ct++=0;
		return(ct);
	}

	if(val<128)
	{
		*ct++=val;
		return(ct);
	}

	if(val<16384)
	{
		*ct++=0x80|(val>>8);
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<2097152)
	{
		*ct++=0xC0|(val>>16);
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	if(val<268435456)
	{
		*ct++=0xE0|(val>>24);
		*ct++=(val>>16)&0xFF;
		*ct++=(val>>8)&0xFF;
		*ct++=val&0xFF;
		return(ct);
	}

	*ct++=0xF0;
	*ct++=(val>>24)&0xFF;
	*ct++=(val>>16)&0xFF;
	*ct++=(val>>8)&0xFF;
	*ct++=val&0xFF;
	return(ct);
}

byte *BTGE_Voxel_EmitSVLI(byte *ct, int val)
{
	return(BTGE_Voxel_EmitVLI(ct,
		(val>=0)?(val<<1):(((-val)<<1)-1)));
}

byte *BTGE_Voxel_EmitString(byte *ct, char *str)
{
	memcpy(ct, str, strlen(str)+1);
	ct+=strlen(str)+1;
	return(ct);
}

byte *BTGE_Voxel_EmitFloat(byte *ct, float val)
{
	int ix, ib;
	
	if(val==0)
	{
		ct=BTGE_Voxel_EmitSVLI(ct, 0);
		ct=BTGE_Voxel_EmitSVLI(ct, 0);
		return(ct);
	}
	
	ix=floor(log(fabs(val))/log(2));
	ib=(16777216*val)/pow(2, ix);
	ix-=24;
	
	while(!(ib&1) && ((ib>1)||(ib<-1)))
		{ ib>>=1; ix++; }

	ct=BTGE_Voxel_EmitSVLI(ct, ib);
	ct=BTGE_Voxel_EmitSVLI(ct, ix);
	return(ct);
}

byte *BTGE_Voxel_EmitVec2(byte *ct, float *val)
{
	ct=BTGE_Voxel_EmitFloat(ct, val[0]);
	ct=BTGE_Voxel_EmitFloat(ct, val[1]);
	return(ct);
}

byte *BTGE_Voxel_EmitVec3(byte *ct, float *val)
{
	ct=BTGE_Voxel_EmitFloat(ct, val[0]);
	ct=BTGE_Voxel_EmitFloat(ct, val[1]);
	ct=BTGE_Voxel_EmitFloat(ct, val[2]);
	return(ct);
}

byte *BTGE_Voxel_EmitVec4(byte *ct, float *val)
{
	ct=BTGE_Voxel_EmitFloat(ct, val[0]);
	ct=BTGE_Voxel_EmitFloat(ct, val[1]);
	ct=BTGE_Voxel_EmitFloat(ct, val[2]);
	ct=BTGE_Voxel_EmitFloat(ct, val[3]);
	return(ct);
}

byte *BTGE_Voxel_EmitPlane4(byte *ct, float *val)
{
	ct=BTGE_Voxel_EmitVec4(ct, val);
	return(ct);
}

byte *BTGE_Voxel_FlattenChunkBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	byte *obuf, int osz)
{
	byte ctb[16*16*16];
	byte *ct, *cs1;
	int x, y, z, n, nv;
	int i, j, k;

	if(chk->pidx_vox)
	{
		ct=obuf;
		*ct++=0xFF;
		*ct++=0x01;

		ct=BTGE_Voxel_EmitSVLI(ct, chk->ox);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->oy);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->oz);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->xs);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->ys);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->zs);
		ct=BTGE_Voxel_EmitVLI(ct, chk->flags);

		chk->tmprle=ct;

		if(chk->biome!=0)
		{
			*ct++=VOX_DCHK_BIOME;
			ct=BTGE_Voxel_EmitVLI(ct, chk->biome);
		}
		if(chk->dungeon_type!=0)
		{
			*ct++=VOX_DCHK_DUNGEON_TYPE;
			ct=BTGE_Voxel_EmitVLI(ct, chk->dungeon_type);
		}
		if(chk->dungeon_type!=0)
		{
			*ct++=VOX_DCHK_DUNGEON_ATTR;
			ct=BTGE_Voxel_EmitVLI(ct, chk->dungeon_attr);
		}

		nv=chk->pidx_n_vox;
		*ct++=VOX_DCHK_SZINDEX;
		ct=BTGE_Voxel_EmitVLI(ct, nv);

		n=chk->xs*chk->ys*chk->zs;
		for(i=0; i<n; i++)
			{ ctb[i]=chk->pidx_idx[i]; }
		*ct++=VOX_DCHK_INDEX;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
		ct+=i;

		n=chk->pidx_n_vox;
		for(i=0; i<nv; i++)
			{ ctb[i]=chk->pidx_vox[i].type&0xFF; }
		*ct++=VOX_DCHK_IDXTYPEL;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, nv);
		ct+=i;

		for(i=0; i<nv; i++)
			{ ctb[i]=chk->pidx_vox[i].aux; }
		*ct++=VOX_DCHK_IDXAUX;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, nv);
		ct+=i;

		for(i=0; i<nv; i++)
			{ ctb[i]=(chk->pidx_vox[i].type>>8)&0xFF; }
		*ct++=VOX_DCHK_IDXTYPEH;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, nv);
		ct+=i;

//		n=chk->xs*chk->ys*chk->zs;
		for(i=0; i<nv; i++)
			{ ctb[i]=chk->pidx_vox[i].aux2&0xFF; }
		*ct++=VOX_DCHK_IDXAUX2L;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, nv);
		ct+=i;

		for(i=0; i<nv; i++)
			{ ctb[i]=(chk->pidx_vox[i].aux2>>8)&0xFF; }
		*ct++=VOX_DCHK_IDXAUX2H;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, nv);
		ct+=i;

		for(i=0; i<nv; i++)
			{ ctb[i]=chk->pidx_vox[i].vlight; }
		*ct++=VOX_DCHK_IDXVLIGHT;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, nv);
		ct+=i;

		for(i=0; i<nv; i++)
			{ ctb[i]=chk->pidx_vox[i].slight; }
		*ct++=VOX_DCHK_IDXSLIGHT;
		i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, nv);
		ct+=i;

		*ct++=0;
	
		return(ct);
	}

	if(!chk->vox)
	{
		if(!chk->voxrle || !chk->sz_voxrle)
		{
			printf("BTGE_Voxel_FlattenChunkBuffer: No Chunk Voxel Data\n");
			return(obuf);
		}

		ct=obuf;
		*ct++=0xFF;
		*ct++=0x01;

		ct=BTGE_Voxel_EmitSVLI(ct, chk->ox);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->oy);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->oz);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->xs);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->ys);
		ct=BTGE_Voxel_EmitSVLI(ct, chk->zs);
		ct=BTGE_Voxel_EmitVLI(ct, chk->flags);

		chk->tmprle=ct;
		cs1=chk->rgn->img_buf+chk->voxrle;
		memcpy(ct, cs1, chk->sz_voxrle);
		ct+=chk->sz_voxrle;

		return(ct);
	}

	ct=obuf;
	*ct++=0xFF;
	*ct++=0x01;

	ct=BTGE_Voxel_EmitSVLI(ct, chk->ox);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->oy);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->oz);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->xs);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->ys);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->zs);
	ct=BTGE_Voxel_EmitVLI(ct, chk->flags);

	chk->tmprle=ct;

	if(chk->biome!=0)
	{
		*ct++=VOX_DCHK_BIOME;
		*ct++=chk->biome;
	}
	if(chk->dungeon_type!=0)
	{
		*ct++=VOX_DCHK_DUNGEON_TYPE;
		ct=BTGE_Voxel_EmitVLI(ct, chk->dungeon_type);
	}
	if(chk->dungeon_type!=0)
	{
		*ct++=VOX_DCHK_DUNGEON_ATTR;
		ct=BTGE_Voxel_EmitVLI(ct, chk->dungeon_attr);
	}

	n=chk->xs*chk->ys*chk->zs;
	for(i=0; i<n; i++)
		{ ctb[i]=chk->vox[i].type&0xFF; }
	*ct++=1;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	for(i=0; i<n; i++)
		{ ctb[i]=chk->vox[i].aux; }
	*ct++=2;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	for(i=0; i<n; i++)
		{ ctb[i]=(chk->vox[i].type>>8)&0xFF; }
	*ct++=3;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	n=chk->xs*chk->ys*chk->zs;
	for(i=0; i<n; i++)
		{ ctb[i]=chk->vox[i].aux2&0xFF; }
	*ct++=4;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	for(i=0; i<n; i++)
		{ ctb[i]=(chk->vox[i].aux2>>8)&0xFF; }
	*ct++=5;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	for(i=0; i<n; i++)
		{ ctb[i]=chk->vox[i].vlight; }
	*ct++=6;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	for(i=0; i<n; i++)
		{ ctb[i]=chk->vox[i].slight; }
	*ct++=7;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	*ct++=0;
	
	return(ct);
}

byte *BTGE_Voxel_DeltaChunkBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk,
	byte *obuf, int osz)
{
	byte ctb[16*16*16];
	byte *ct;
	int x, y, z, n;
	int i, j, k;

	if(osz<65536)
		return(obuf);

	if(chk->flags&VOX_CHFL_DELTAOK)
		return(obuf);

//	if(!chk->tickactive)
//		return(obuf);
	if(!(chk->flags&VOX_CHFL_DELTA_ACTIVE))
		return(obuf);

	ct=BTGE_Voxel_FlattenChunkBuffer(world, chk, obuf, osz);

#if 0
	ct=obuf;

	*ct++=0xFF;
	*ct++=0x01;

	ct=BTGE_Voxel_EmitSVLI(ct, chk->ox);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->oy);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->oz);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->xs);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->ys);
	ct=BTGE_Voxel_EmitSVLI(ct, chk->zs);
	ct=BTGE_Voxel_EmitVLI(ct, chk->flags);

	n=chk->xs*chk->ys*chk->zs;
	for(i=0; i<n; i++)
		{ ctb[i]=chk->vox[i].type&0xFF; }
	
	*ct++=1;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	for(i=0; i<n; i++)
		{ ctb[i]=chk->vox[i].aux; }
	
	*ct++=2;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	for(i=0; i<n; i++)
		{ ctb[i]=(chk->vox[i].type>>8)&0xFF; }
	
	*ct++=3;
	i=BTGE_Voxel_EncodeBufferRLE(ct, ctb, n);
	ct+=i;

	*ct++=0;
#endif

	chk->flags|=VOX_CHFL_DELTAOK;
	
	return(ct);
}

BTGE_API byte *BTGE_Voxel_DeltaRegionBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	byte *obuf, int osz)
{
	char tb[1024];
	BTGE_VoxelChunk *chk;
	byte *ct, *ct1, *cte;
	int i, j, k, l, n, m;

	if(osz<65536)
	{
		printf("BTGE_Voxel_DeltaRegionBuffer: Buffer Space Check\n");
		return(obuf);
	}

	ct=obuf; cte=obuf+osz;

	*ct++=0xFF;
	*ct++=0x02;

#if 1
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->ox);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->oy);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->oz);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->xs);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->ys);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->zs);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->xst);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->yst);
	ct=BTGE_Voxel_EmitSVLI(ct, rgn->zst);

	ct=BTGE_Voxel_EmitVLI(ct, (int)(rgn->voxsz));
	ct=BTGE_Voxel_EmitVLI(ct, rgn->flags);
#endif

	n=0; m=0;
	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
			for(k=0; k<rgn->zs; k++)
	{
		if((cte-ct)<65536)
			break;
	
		chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+k];
		if(!chk)continue;

		if(!(chk->flags&VOX_CHFL_DELTADIRTY))
			continue;
		ct1=BTGE_Voxel_DeltaChunkBuffer(world, chk, ct, cte-ct);
		if(ct1==ct)continue;
		ct=ct1;
		m++;
	}

	if(!m)return(obuf);

#if 1
	if(rgn->values)
	{
		j=0;
		for(i=0; i<rgn->n_values; i++)
		{
			if(rgn->oldvalues[i]!=rgn->values[i])
				j++;
		}

		if(j)
		{
			*ct++=0xFF;
			*ct++=0x06;

			for(i=1; i<rgn->n_values; i++)
			{
				if(rgn->oldvalues[i]!=rgn->values[i])
				{
					ct=BTGE_Voxel_EmitVLI(ct, i);
					dysDumpStr(tb, rgn->values[i]);
					ct=BTGE_Voxel_EmitString(ct, tb);
				}
			}
			ct=BTGE_Voxel_EmitVLI(ct, 0);
	
			*ct++=0xFF;
			*ct++=0x07;
		}
	}
#endif

	if((cte-ct)>65536)
	{
		ct=BTGE_Voxel_FlattenRegionSEntityDeltaListBuffer(
			world, rgn, rgn->sentity, ct, cte-ct);
	}

	if(!m)return(obuf);
	
//	printf("BTGE_Voxel_DeltaRegionBuffer: %d chunks\n", m);
	
	*ct++=0xFF;
	*ct++=0x03;
	return(ct);
}

BTGE_API void BTGE_Voxel_PostDeltaRegion(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn)
{
	BTGE_VoxelChunk *chk;
	BTGE_SEntity *scur;
	byte *ct;
	int i, j, k, l, n, m;

	n=0; m=0;
	for(i=0; i<rgn->xs; i++)
		for(j=0; j<rgn->ys; j++)
			for(k=0; k<rgn->zs; k++)
	{
		chk=rgn->chk[(i*rgn->ys+j)*rgn->zs+k];
		if(!chk)continue;
		if(chk->flags&VOX_CHFL_DELTAOK)
		{
			chk->flags&=~(VOX_CHFL_DELTADIRTY|VOX_CHFL_DELTAOK);
		}
	}

	for(i=0; i<rgn->n_values; i++)
	{
		rgn->oldvalues[i]=rgn->values[i];
	}

	scur=rgn->sentity;
	while(scur)
	{
		scur->flags&=~(BTGE_SEFL_NEW|BTGE_SEFL_DIRTY);
		scur=scur->next;
	}
}

BTGE_API dyt BTGE_Voxel_ComposeWorldDelta(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur;
	byte *buf, *ct, *cte;
	dyt t;
	int sz;
	int i, j;

	buf=gcmalloc(1<<20);
	ct=buf; cte=buf+(1<<20);
	if(!buf)return(NULL);

	*ct++=0xFF;
	*ct++=0x04;

	cur=world->vox_region;
	while(cur)
	{
		ct=BTGE_Voxel_DeltaRegionBuffer(world, cur, ct, cte-ct);
		cur=cur->next;
	}

#if 1
	j=0;
	for(i=0; i<4096; i++)
	{
		if(btge_voxinfo[i]!=btge_oldvoxinfo[i])
			j++;
	}

	if(j)
	{
		*ct++=0xFF;
		*ct++=0x08;

		for(i=1; i<4096; i++)
		{
			if(btge_voxinfo[i]!=btge_oldvoxinfo[i])
			{
				ct=BTGE_Voxel_EmitVLI(ct, i);
				ct=BTGE_Voxel_EmitString(ct, btge_voxinfo[i]->name);
			}
		}

		ct=BTGE_Voxel_EmitVLI(ct, 0);

		*ct++=0xFF;
		*ct++=0x09;
	}
#endif

	*ct++=0xFF;
	*ct++=0x05;

	sz=ct-buf;

	if(sz<=4)
	{
		gcfree(buf);
		return(NULL);
	}

//	printf("BTGE_Voxel_ComposeWorldDelta: %d bytes\n", sz);

	t=dyByteArrayFromBuffer(buf, sz);
	gcfree(buf);
	
	t=dylist2s("voxdelta", t);
	return(t);
}

BTGE_API void BTGE_Voxel_WorldPreDelta(BTGE_BrushWorld *world)
{
}

BTGE_API void BTGE_Voxel_WorldPostDelta(BTGE_BrushWorld *world)
{
	BTGE_VoxelRegion *cur;
	int i, j;

	cur=world->vox_region;
	while(cur)
	{
		BTGE_Voxel_PostDeltaRegion(world, cur);
		cur=cur->next;
	}

	for(i=0; i<4096; i++)
	{
		btge_oldvoxinfo[i]=btge_voxinfo[i];
	}
}


byte *BTGE_Voxel_DecodeVLI(byte *cs, int *rval)
{
	int i;
	
	if(!cs)return(NULL);
	
	i=*cs++;
	if(!(i&0x80))
	{
		*rval=i; return(cs);
	}else if((i&0xC0)==0x80)
	{
		i=((i&0x3F)<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xE0)==0xC0)
	{
		i=((i&0x1F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xF0)==0xE0)
	{
		i=((i&0x0F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}else if((i&0xF8)==0xF0)
	{
		i=((i&0x0F)<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		i=(i<<8)|(*cs++);
		*rval=i; return(cs);
	}
	
//	*(int *)-1=-1;
	*rval=0;
//	return(cs);
	return(NULL);
}

byte *BTGE_Voxel_DecodeSVLI(byte *cs, int *rval)
{
	int i;
	
	cs=BTGE_Voxel_DecodeVLI(cs, &i);
	
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	
	*rval=i;
	return(cs);
}

byte *BTGE_Voxel_DecodeFloat(byte *cs, float *rval)
{
	float f;
	int ib, ix;

	cs=BTGE_Voxel_DecodeSVLI(cs, &ib);
	cs=BTGE_Voxel_DecodeSVLI(cs, &ix);
	
	f=ib*pow(2, ix);
	*rval=f;
	return(cs);
}

byte *BTGE_Voxel_DecodeVec2(byte *cs, float *rval)
{
	cs=BTGE_Voxel_DecodeFloat(cs, rval+0);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+1);
	return(cs);
}

byte *BTGE_Voxel_DecodeVec3(byte *cs, float *rval)
{
	cs=BTGE_Voxel_DecodeFloat(cs, rval+0);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+1);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+2);
	return(cs);
}

byte *BTGE_Voxel_DecodeVec4(byte *cs, float *rval)
{
	cs=BTGE_Voxel_DecodeFloat(cs, rval+0);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+1);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+2);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+3);
	return(cs);
}

byte *BTGE_Voxel_DecodePlane4(byte *cs, float *rval)
{
	cs=BTGE_Voxel_DecodeFloat(cs, rval+0);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+1);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+2);
	cs=BTGE_Voxel_DecodeFloat(cs, rval+3);
	return(cs);
}

byte *BTGE_Voxel_DecodeString(byte *cs, char *buf, int szBuf)
{
	strcpy(buf, cs);
	cs=cs+strlen(cs+1);
	return(cs);
}

byte *BTGE_Voxel_DecodeFindNextMarker(byte *cs, byte *cse)
{
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]!=0x00))
			return(cs);
		cs++;
	}
	return(NULL);
}

BTGE_API void BTGE_Voxel_CheckDecodeChunkVoxelRLE(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	int i;

	if(!chk->vox && !chk->pidx_vox)
	{
		if(!chk->voxrle)
		{
			i=chk->xs*chk->ys*chk->zs;
			chk->vox=gctatomic("btge_voxeldata_t",
				i*sizeof(BTGE_VoxelData));
			return;
		}
	
		if(chk->tick_inhibit_voxrle<128)
			chk->tick_inhibit_voxrle=128;
		BTGE_Voxel_DecodeChunkVoxelRLE(world, chk);
	}else
	{
		if(chk->tick_inhibit_voxrle<64)
			chk->tick_inhibit_voxrle=64;
	}
}

BTGE_API byte *BTGE_Voxel_DecodeChunkVoxelRLE(
	BTGE_BrushWorld *world, BTGE_VoxelChunk *chk)
{
	BTGE_VoxelRegion *rgn;
	BTGE_VoxelChunk *chk1;
	BTGE_VoxelData *vox;
	byte *cs, *cs1, *cse;
	byte *ctbuf[64];
	int biome, szidx, dntype, dnattr;
	int i, j, k, l, n, m;
	int xs, ys, zs, fl;
	int x, y, z;
	
	btge_voxel_lock();

	xs=chk->xs;
	ys=chk->ys;
	zs=chk->zs;

	rgn=chk->rgn;
	cs=rgn->img_buf+chk->voxrle;
	cse=cs+65536;

	if(!cs)
	{
		printf("BTGE_Voxel_DecodeChunkVoxelRLE: No Voxel RLE\n");
		btge_voxel_unlock();
		return(NULL);
	}

	for(i=0; i<64; i++)
		ctbuf[i]=NULL;

	biome=0; szidx=256;
	while(1)
	{
		l=*cs++;
		if(!l)break;

//		if(l==VOX_DCHK_BIOME)
//			{ biome=*cs++; continue; }
//		if(l==VOX_DCHK_SZINDEX)
//			{ szidx=*cs++; continue; }

		if(l==VOX_DCHK_BIOME)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &biome);
//			biome=*cs++;
			continue;
		}
		if(l==VOX_DCHK_SZINDEX)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &szidx);
//			szidx=*cs++;
			continue;
		}
		if(l==VOX_DCHK_DUNGEON_TYPE)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &dntype);
			continue;
		}
		if(l==VOX_DCHK_DUNGEON_ATTR)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &dnattr);
			continue;
		}

		if(l==VOX_DCHK_INDEX)
		{
			ctbuf[0x10]=malloc(2*xs*ys*zs);
			i=BTGE_Voxel_DecodeBufferRLE(ctbuf[0x10], xs*ys*zs, cs, cse-cs);
			cs+=i;
			continue;
		}
		
		if((l>=0x11) && (l<=0x1F))
		{
			i=szidx;
			ctbuf[l]=malloc(2*i);
			i=BTGE_Voxel_DecodeBufferRLE(ctbuf[l], i, cs, cse-cs);
			cs+=i;
			continue;
		}

		if((l<1) || (l>=16))
		{
			printf("BTGE_Voxel_DecodeChunkVoxelRLE: Bad Index\n");
			btge_voxel_unlock();
			return(NULL);
//			break;
		}
		
		ctbuf[l]=malloc(2*xs*ys*zs);
		i=BTGE_Voxel_DecodeBufferRLE(ctbuf[l], xs*ys*zs, cs, cse-cs);
		cs+=i;
	}
	
//	rgn=chk->rgn;
		
#if 0
	chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox-16, oy, oz, 4);
	chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox+16, oy, oz, 4);
	chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy-16, oz, 4);
	chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy+16, oz, 4);
	chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy, oz-16, 4);
	chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy, oz+16, 4);
#endif

	if(ctbuf[16])
	{
		if(chk->vox)
		{
			gcfree(chk->vox);
			chk->vox=NULL;
		}

		BTGE_Voxel_CheckSetupChunkVoxelIndex(chk, szidx);
		chk->flags|=VOX_CHFL_PACKINDEX;

		n=szidx;
		chk->pidx_n_vox=n;
		for(i=0; i<n; i++)
		{
			vox=&(chk->pidx_vox[i]);

			if(ctbuf[17])
				vox->type=ctbuf[17][i];
			if(ctbuf[18])
				vox->aux=ctbuf[18][i];
			if(ctbuf[19])
				vox->type|=ctbuf[19][i]<<8;
			if(ctbuf[20])
				vox->aux2=ctbuf[20][i];
			if(ctbuf[21])
				vox->aux2|=ctbuf[21][i]<<8;
			if(ctbuf[22])
				vox->vlight=ctbuf[22][i];
			if(ctbuf[23])
				vox->slight=ctbuf[23][i];
		}

		n=xs*ys*zs;
		for(i=0; i<n; i++)
			{ chk->pidx_idx[i]=ctbuf[16][i]; }
	}else
	{
		if(chk->pidx_vox)
			{ gcfree(chk->pidx_vox); chk->pidx_vox=NULL; }
		if(chk->pidx_idx)
			{ gcfree(chk->pidx_idx); chk->pidx_idx=NULL; }

		chk->flags&=~VOX_CHFL_PACKINDEX;

		if(!chk->vox)
		{
			i=chk->xs*chk->ys*chk->zs;
			chk->vox=gctatomic("btge_voxeldata_t",
				i*sizeof(BTGE_VoxelData));
			
			if(!chk->vox)
			{
				printf("BTGE_Voxel_DecodeChunkVoxelRLE: Fail Allocate\n");
				btge_voxel_unlock();
				return(NULL);
			}
		}

		n=xs*ys*zs;
		for(i=0; i<n; i++)
		{
			vox=&(chk->vox[i]);

			if(ctbuf[1])
				vox->type=ctbuf[1][i];
			if(ctbuf[2])
				vox->aux=ctbuf[2][i];
			if(ctbuf[3])
				vox->type|=ctbuf[3][i]<<8;

			if(ctbuf[4])
				vox->aux2=ctbuf[4][i];
			if(ctbuf[5])
				vox->aux2|=ctbuf[5][i]<<8;
			if(ctbuf[6])
				vox->vlight=ctbuf[6][i];
			if(ctbuf[7])
				vox->slight=ctbuf[7][i];
		}
	}

	for(i=0; i<64; i++)
	{
		if(ctbuf[i])
			free(ctbuf[i]);
	}

	btge_voxel_unlock();

	return(cs);
}

BTGE_API byte *BTGE_Voxel_DecodeDeltaChunkBuffer(
	BTGE_BrushWorld *world, byte *ibuf, int isz)
{
	return(BTGE_Voxel_DecodeChunkBufferFlags(world, ibuf, isz, 0));
}

#if 1
BTGE_API byte *BTGE_Voxel_DecodeLoadChunkBuffer(
	BTGE_BrushWorld *world, byte *ibuf, int isz)
{
	return(BTGE_Voxel_DecodeChunkBufferFlags(world, ibuf, isz, 1));
}
#endif

BTGE_API byte *BTGE_Voxel_DecodeChunkBufferFlags(
	BTGE_BrushWorld *world, byte *ibuf, int isz, int flags)
{
	BTGE_VoxelRegion *rgn;
	BTGE_VoxelChunk *chk, *chk1;
	BTGE_VoxelData *vox;
	byte *cs, *cs1, *cse, *voxrle;
	byte *ctbuf[64];
	int biome, szidx, dntype, dnattr;
	int i, j, k, l, n, m;
	int ox, oy, oz, xs, ys, zs, fl;
	int x, y, z;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x01))
	{
		printf("BTGE_Voxel_DecodeChunkBufferFlags: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

	cs=BTGE_Voxel_DecodeSVLI(cs, &ox);
	cs=BTGE_Voxel_DecodeSVLI(cs, &oy);
	cs=BTGE_Voxel_DecodeSVLI(cs, &oz);
	cs=BTGE_Voxel_DecodeSVLI(cs, &xs);
	cs=BTGE_Voxel_DecodeSVLI(cs, &ys);
	cs=BTGE_Voxel_DecodeSVLI(cs, &zs);

	cs=BTGE_Voxel_DecodeVLI(cs, &fl);

	if(!cs ||
		(xs<=0) || (ys<=0) || (zs<=0) ||
		(xs>256) || (ys>256) || (xs>256))
	{
		printf("BTGE_Voxel_DecodeChunkBufferFlags: "
			"Bad or Corrupt Chunk Header\n");
		return(ibuf);
	}

	if(flags&1)
	{
		fl&=~VOX_CHFL_DELTAOK;
//		fl&=~(VOX_CHFL_DELTADIRTY|VOX_CHFL_DELTAOK);
		fl|=VOX_CHFL_DELTADIRTY;
	}

//	printf("BTGE_Voxel_DecodeDeltaChunkBuffer: "
//		"Org=%d,%d,%d Sz=%d,%d,%d Fl=%d\n", ox, oy, oz, xs, ys, zs, fl);

	for(i=0; i<64; i++)
		ctbuf[i]=NULL;

	szidx=256; biome=0; dntype=0; dnattr=0;
	voxrle=cs;
	while(1)
	{
		l=*cs++;
		if(!l)break;
		
		if(l==VOX_DCHK_BIOME)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &biome);
//			biome=*cs++;
			continue;
		}
		if(l==VOX_DCHK_SZINDEX)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &szidx);
//			szidx=*cs++;
			continue;
		}
		if(l==VOX_DCHK_DUNGEON_TYPE)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &dntype);
			continue;
		}
		if(l==VOX_DCHK_DUNGEON_ATTR)
		{
			cs=BTGE_Voxel_DecodeVLI(cs, &dnattr);
			continue;
		}

		if(l==VOX_DCHK_INDEX)
		{
			ctbuf[16]=malloc(2*xs*ys*zs);
			i=BTGE_Voxel_DecodeBufferRLE(ctbuf[16], xs*ys*zs, cs, cse-cs);
			cs+=i;
			continue;
		}
		
		if((l>=0x11) && (l<=0x1F))
		{
			i=szidx;
			ctbuf[l]=malloc(2*i);
			i=BTGE_Voxel_DecodeBufferRLE(ctbuf[l], i, cs, cse-cs);
			cs+=i;
			continue;
		}
		
		if((l<1) || (l>=16))
		{
			printf("BTGE_Voxel_DecodeChunkBufferFlags: Bad Index\n");
			return(ibuf);
//			break;
		}
		
		ctbuf[l]=malloc(2*xs*ys*zs);
		i=BTGE_Voxel_DecodeBufferRLE(ctbuf[l], xs*ys*zs, cs, cse-cs);
		cs+=i;
	}
	
	chk=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy, oz, 2|4);
	if(chk)
	{
		rgn=chk->rgn;
		
		if((ox==chk->ox) && (oy==chk->oy) && (oz==chk->oz) &&
			(xs==chk->xs) && (ys==chk->ys) && (zs==chk->zs))
		{
#if 0
			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox-16, oy, oz, 4);
			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox+16, oy, oz, 4);
			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy-16, oz, 4);
			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy+16, oz, 4);
			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy, oz-16, 4);
			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy, oz+16, 4);

//			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox-16, oy, oz+16, 4);
//			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox+16, oy, oz+16, 4);
//			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy-16, oz+16, 4);
//			chk1=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy+16, oz+16, 4);
#endif

			btge_voxel_lock();

			if(ctbuf[16])
			{
				if(chk->vox)
				{
					gcfree(chk->vox);
					chk->vox=NULL;
				}

				BTGE_Voxel_CheckSetupChunkVoxelIndex(chk, szidx);

				chk->biome=biome;
				chk->flags=fl;
				chk->flags|=VOX_CHFL_PACKINDEX;

				chk->tmprle=voxrle;

				n=szidx;
				chk->pidx_n_vox=n;
				for(i=0; i<n; i++)
				{
					vox=&(chk->pidx_vox[i]);

					if(ctbuf[17])
						vox->type=ctbuf[17][i];
					if(ctbuf[18])
						vox->aux=ctbuf[18][i];
					if(ctbuf[19])
						vox->type|=ctbuf[19][i]<<8;
					if(ctbuf[20])
						vox->aux2=ctbuf[20][i];
					if(ctbuf[21])
						vox->aux2|=ctbuf[21][i]<<8;
					if(ctbuf[22])
						vox->vlight=ctbuf[22][i];
					if(ctbuf[23])
						vox->slight=ctbuf[23][i];
				}

				n=xs*ys*zs;
				for(i=0; i<n; i++)
					{ chk->pidx_idx[i]=ctbuf[16][i]; }
			}else
			{
				if(chk->pidx_vox)
					{ gcfree(chk->pidx_vox); chk->pidx_vox=NULL; }
				if(chk->pidx_idx)
					{ gcfree(chk->pidx_idx); chk->pidx_idx=NULL; }

				if(!chk->vox)
				{
					i=chk->xs*chk->ys*chk->zs;
//					chk->vox=gcatomic(i*sizeof(BTGE_VoxelData));
					chk->vox=gctatomic("btge_voxeldata_t",
						i*sizeof(BTGE_VoxelData));

					if(!chk->vox)
					{
						for(i=0; i<64; i++)
						{
							if(ctbuf[i])
								free(ctbuf[i]);
						}

						chk->tmprle=voxrle;
						printf("BTGE_Voxel_DecodeChunkBufferFlags: "
							"Fail Allocate\n");
						btge_voxel_unlock();
						return(cs);
					}
				}

				chk->biome=biome;
				chk->flags=fl;
				chk->flags&=~VOX_CHFL_PACKINDEX;

//				chk->voxrle=0;
//				chk->voxrle=voxrle;
				chk->tmprle=voxrle;

//				if(chk->tick_inhibit_voxrle<255)
//					chk->tick_inhibit_voxrle=255;
				if(chk->tick_inhibit_voxrle<64)
					chk->tick_inhibit_voxrle=64;

				n=xs*ys*zs;
				for(i=0; i<n; i++)
				{
					vox=&(chk->vox[i]);

					if(ctbuf[1])
						vox->type=ctbuf[1][i];
					if(ctbuf[2])
						vox->aux=ctbuf[2][i];
					if(ctbuf[3])
						vox->type|=ctbuf[3][i]<<8;

					if(ctbuf[4])
						vox->aux2=ctbuf[4][i];
					if(ctbuf[5])
						vox->aux2|=ctbuf[5][i]<<8;
					if(ctbuf[6])
						vox->vlight=ctbuf[6][i];
					if(ctbuf[7])
						vox->slight=ctbuf[7][i];
				}

				for(i=0; i<64; i++)
				{
					if(ctbuf[i])
						free(ctbuf[i]);
				}
			}

			btge_voxel_unlock();

			return(cs);
		}
	}else
	{
		printf("BTGE_Voxel_DecodeChunkBufferFlags: "
			"Failed to get chunk At %d,%d,%d\n", ox, oy, oz);

		for(i=0; i<64; i++)
		{
			if(ctbuf[i])
				free(ctbuf[i]);
		}
		return(cs);
	}
	
	chk->biome=biome;
	chk->flags=fl;
	chk->dungeon_type=dntype;
	chk->dungeon_attr=dnattr;

	chk->voxrle=0;
	chk->tmprle=NULL;

	for(i=0; i<zs; i++)
		for(j=0; j<ys; j++)
			for(k=0; k<zs; k++)
	{
		x=ox+k;
		y=oy+j;
		z=oz+i;
		
		l=(i*ys+j)*xs+k;
		
		vox=BTGE_Voxel_GetWorldVoxelDirty3(world, x, y, z);
		if(ctbuf[1])
			vox->type=ctbuf[1][l];
		if(ctbuf[2])
			vox->aux=ctbuf[2][l];
		if(ctbuf[3])
			vox->type|=ctbuf[3][l]<<8;

		if(ctbuf[4])
			vox->aux2=ctbuf[4][l];
		if(ctbuf[5])
			vox->aux2|=ctbuf[5][l]<<8;
		if(ctbuf[6])
			vox->vlight=ctbuf[6][l];
		if(ctbuf[7])
			vox->slight=ctbuf[7][l];
	}

	for(i=0; i<64; i++)
	{
		if(ctbuf[i])
			free(ctbuf[i]);
	}
	
	return(cs);
}

#if 0
BTGE_API byte *BTGE_Voxel_DecodeLoadChunkBuffer(
	BTGE_BrushWorld *world, byte *ibuf, int isz)
{
	BTGE_VoxelRegion *rgn;
	BTGE_VoxelChunk *chk, *chk1;
	BTGE_VoxelData *vox;
	byte *cs, *cs1, *cse, *voxrle;
	byte *ctbuf[64];
	int i, j, k, l, n, m;
	int ox, oy, oz, xs, ys, zs, fl;
	int x, y, z;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x01))
	{
		printf("BTGE_Voxel_DecodeDeltaChunkBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

	cs=BTGE_Voxel_DecodeSVLI(cs, &ox);
	cs=BTGE_Voxel_DecodeSVLI(cs, &oy);
	cs=BTGE_Voxel_DecodeSVLI(cs, &oz);
	cs=BTGE_Voxel_DecodeSVLI(cs, &xs);
	cs=BTGE_Voxel_DecodeSVLI(cs, &ys);
	cs=BTGE_Voxel_DecodeSVLI(cs, &zs);

	cs=BTGE_Voxel_DecodeVLI(cs, &fl);

	if(!cs ||
		(xs<=0) || (ys<=0) || (zs<=0) ||
		(xs>256) || (ys>256) || (xs>256))
	{
		printf("BTGE_Voxel_DecodeDeltaChunkBuffer: "
			"Bad or Corrupt Chunk Header\n");
		return(ibuf);
	}

#if 1
//	printf("BTGE_Voxel_DecodeDeltaChunkBuffer: "
//		"Org=%d,%d,%d Sz=%d,%d,%d Fl=%d\n", ox, oy, oz, xs, ys, zs, fl);

	for(i=0; i<64; i++)
		ctbuf[i]=NULL;

	voxrle=cs;
	while(1)
	{
		l=*cs++;
		if(!l)break;
		
		if((l<1) || (l>=16))
		{
			printf("BTGE_Voxel_DecodeDeltaChunkBuffer: Bad Index\n");
			return(ibuf);
//			break;
		}
		
		ctbuf[l]=malloc(2*xs*ys*zs);
		i=BTGE_Voxel_DecodeBufferRLE(ctbuf[l], xs*ys*zs, cs, cse-cs);
		cs+=i;
	}
	
	chk=BTGE_Voxel_GetWorldChunkFlags(world, ox, oy, oz, 2|4);
	if(chk)
	{
		rgn=chk->rgn;
		
		if((ox==chk->ox) && (oy==chk->oy) && (oz==chk->oz) &&
			(xs==chk->xs) && (ys==chk->ys) && (zs==chk->zs))
		{
			if(chk->vox)
			{
				gcfree(chk->vox);
				chk->vox=NULL;
			}
		
			chk->voxrle=0;
			chk->tmprle=voxrle;

#if 0
			n=xs*ys*zs;
			for(i=0; i<n; i++)
			{
				vox=&(chk->vox[i]);

				if(ctbuf[1])
					vox->type=ctbuf[1][i];
				if(ctbuf[2])
					vox->aux=ctbuf[2][i];
				if(ctbuf[3])
					vox->type|=ctbuf[3][i]<<8;

				if(ctbuf[4])
					vox->aux2=ctbuf[4][i];
				if(ctbuf[5])
					vox->aux2|=ctbuf[5][i]<<8;
				if(ctbuf[6])
					vox->vlight=ctbuf[6][i];
				if(ctbuf[7])
					vox->slight=ctbuf[7][i];
			}
#endif

			for(i=0; i<64; i++)
			{
				if(ctbuf[i])
					free(ctbuf[i]);
			}
	
			return(cs);
		}
	}
	
#endif

	printf("BTGE_Voxel_DecodeLoadChunkBuffer: Bad\n");
	*(int *)-1=-1;

	return(cs);
}
#endif

BTGE_API byte *BTGE_Voxel_DecodeDeltaValuesBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, byte *ibuf, int isz)
{
	byte *cs, *cs1, *cse;
	byte *ctbuf[8];
	char *s;
	dyt v;
	int i, j, k, l, n, m;
	int ox, oy, oz, xs, ys, zs, fl;
	int x, y, z;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x06))
	{
		printf("BTGE_Voxel_DecodeDeltaValuesBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

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

	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==0x07))
			{ cs+=2; break; }
		cs++;
	}
	
	return(cs);
}

BTGE_API byte *BTGE_Voxel_DecodeDeltaVoxelTypesBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, byte *ibuf, int isz)
{
	byte *cs, *cs1, *cse;
	byte *ctbuf[8];
	char *s;
	dyt v;
	int i, j, k, l, n, m;
	int ox, oy, oz, xs, ys, zs, fl;
	int x, y, z;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x08))
	{
		printf("BTGE_Voxel_DecodeDeltaValuesBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

	while(cs<cse)
	{
		cs=BTGE_Voxel_DecodeVLI(cs, &i);
		if(i<=0)break;
		s=(char *)cs;
		cs+=strlen(s)+1;
		
		btge_voxnames_cache[i]=dystrdup(s);
		BTGE_Voxel_LoadVoxelType(i, s);
//		v=dysParseFromString(s);
//		BTGE_Voxel_WorldRegionValueCheckIndex(world, rgn, i);
//		rgn->values[i]=v;
	}

	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==0x09))
			{ cs+=2; break; }
		cs++;
	}
	
	return(cs);
}

BTGE_API byte *BTGE_Voxel_DecodeDeltaRegionBuffer(
	BTGE_BrushWorld *world, byte *ibuf, int isz)
{
	BTGE_VoxelChunk *chk;
	BTGE_VoxelRegion *rgn;
	byte *cs, *cs1, *cse;
	int i, j, k, l, n, m;
	int ox, oy, oz, xs, ys, zs, xst, yst, zst, fl, vsz;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x02))
	{
		printf("BTGE_Voxel_DecodeDeltaRegionBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

#if 1
	cs=BTGE_Voxel_DecodeSVLI(cs, &ox);
	cs=BTGE_Voxel_DecodeSVLI(cs, &oy);
	cs=BTGE_Voxel_DecodeSVLI(cs, &oz);
	cs=BTGE_Voxel_DecodeSVLI(cs, &xs);
	cs=BTGE_Voxel_DecodeSVLI(cs, &ys);
	cs=BTGE_Voxel_DecodeSVLI(cs, &zs);
	cs=BTGE_Voxel_DecodeSVLI(cs, &xst);
	cs=BTGE_Voxel_DecodeSVLI(cs, &yst);
	cs=BTGE_Voxel_DecodeSVLI(cs, &zst);

	cs=BTGE_Voxel_DecodeVLI(cs, &fl);
	cs=BTGE_Voxel_DecodeVLI(cs, &vsz);
	
	rgn=BTGE_Voxel_GetWorldRegionFlags(world, ox, oy, oz, 1);
#endif

	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==0x03))
		{
			cs+=2;
			break;
		}
		if((cs[0]==0xFF) && (cs[1]==0x01))
		{
			cs1=BTGE_Voxel_DecodeDeltaChunkBuffer(world, cs, cse-cs);
			if(cs1==cs)break;
			cs=cs1;
			continue;
		}

		if((cs[0]==0xFF) && (cs[1]==0x06))
		{
			cs1=BTGE_Voxel_DecodeDeltaValuesBuffer(world, rgn, cs, cse-cs);
			if(cs1==cs)break;
			cs=cs1;
			continue;
		}

		if((cs[0]==0xFF) && (cs[1]==0x10))
		{
			cs1=BTGE_Voxel_DecodeRegionSEntityListBuffer(
				world, rgn, cs, cse-cs);
			if(cs1==cs)break;
			cs=cs1;
			continue;
		}
		
		printf("BTGE_Voxel_DecodeDeltaRegionBuffer: Bad Marker\n");
		break;
	}

	return(cs);
}

BTGE_API byte *BTGE_Voxel_DecodeDeltaBuffer(
	BTGE_BrushWorld *world, byte *ibuf, int isz)
{
	BTGE_VoxelChunk *chk;
	byte *cs, *cs1, *cse;
	int i, j, k, l, n, m;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x04))
	{
		printf("BTGE_Voxel_DecodeDeltaBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==0x05))
		{
			cs+=2;
			break;
		}
		if((cs[0]==0xFF) && (cs[1]==0x02))
		{
			cs1=BTGE_Voxel_DecodeDeltaRegionBuffer(world, cs, cse-cs);
			if(cs1==cs)break;
			cs=cs1;
			continue;
		}

		if((cs[0]==0xFF) && (cs[1]==0x08))
		{
			cs1=BTGE_Voxel_DecodeDeltaVoxelTypesBuffer(
				world, NULL, cs, cse-cs);
			if(cs1==cs)break;
			cs=cs1;
			continue;
		}

		printf("BTGE_Voxel_DecodeDeltaBuffer: Bad Marker\n");
		break;
	}

	return(cs);
}


BTGE_API void BTGE_Voxel_WorldDecodeDelta(BTGE_BrushWorld *world, dyt msg)
{
	byte *buf;
	dyt t;
	int sz;
	
	t=dycadr(msg);
	if(!dyByteArrayP(t))
	{
		printf("BTGE_Voxel_WorldDecodeDelta: Not Byte Array\n");
		return;
	}

	buf=dyByteArrayData(t);
	sz=dyByteArraySize(t);

	BTGE_Voxel_GlobalThink(world);

	BTGE_Voxel_DecodeDeltaBuffer(world, buf, sz);
}

BTGE_API void BTGE_Voxel_WorldRegionValueCheckIndex(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, int idx)
{
	int i;

	if(!rgn->values)
	{
		i=256;
		while(idx>=i)i=i+(i>>1);
		rgn->values=gcalloc(i*sizeof(dyt));
		rgn->oldvalues=gcalloc(i*sizeof(dyt));
		rgn->n_values=1;
		rgn->m_values=i;
		rgn->values[0]=NULL;
	}
	
	if(idx>=rgn->m_values)
	{
		i=rgn->m_values+(rgn->m_values>>1);
		while(idx>=i)i=i+(i>>1);
		rgn->values=gcrealloc(rgn->values, i*sizeof(dyt));
		rgn->oldvalues=gcrealloc(rgn->oldvalues, i*sizeof(dyt));
		rgn->m_values=i;
	}
}

BTGE_API int BTGE_Voxel_WorldIndexValue(
	BTGE_BrushWorld *world, int x, int y, int z, dyt val)
{
	BTGE_VoxelRegion *rgn;
	int i;
	
	rgn=BTGE_Voxel_GetWorldRegionFlags(world, x, y, z, 1);
	if(!rgn)return(0);

	if(dyintp(val))
	{
		i=dyintv(val);
		if((i>=0) && (i<16384))
			return(i);
	}

	if(!rgn->values)
	{
		i=256;
		rgn->values=gcalloc(i*sizeof(dyt));
		rgn->oldvalues=gcalloc(i*sizeof(dyt));
		rgn->n_values=1;
		rgn->m_values=i;
		rgn->values[0]=NULL;
	}
	
	if((rgn->n_values+1)>=rgn->m_values)
	{
		i=rgn->m_values+(rgn->m_values>>1);
		rgn->values=gcrealloc(rgn->values, i*sizeof(dyt));
		rgn->oldvalues=gcrealloc(rgn->oldvalues, i*sizeof(dyt));
		rgn->m_values=i;
	}
	
	for(i=0; i<rgn->n_values; i++)
		if(dyeqp(val, rgn->values[i]))
			return(0x4000|i);
			
	i=rgn->n_values++;
	rgn->values[i]=val;
	return(0x4000|i);
}

BTGE_API dyt BTGE_Voxel_WorldGetIndexValue(
	BTGE_BrushWorld *world, int x, int y, int z, int idx)
{
	BTGE_VoxelRegion *rgn;
	int i;
	
	rgn=BTGE_Voxel_GetWorldRegionFlags(world, x, y, z, 1);
	if(!rgn)return(NULL);

	if((idx&0xC000)==0x0000)
		return(dyint(idx));
	
	if((idx&0xC000)==0x4000)
	{
		if(!rgn->values)
			return(NULL);
		return(rgn->values[idx&0x3FFF]);
	}
	
	return(NULL);
//	return(rgn->values[idx&0x3FFF]);
}
