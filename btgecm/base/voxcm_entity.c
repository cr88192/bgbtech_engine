#include <btgecm.h>

BTGE_API byte *BTGE_Voxel_FlattenRegionBrushBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	BTGE_Brush *brush, byte *obuf, int osz)
{
	byte *ct, *cte;
	int i;
	
	ct=obuf; cte=obuf+osz;

	if(brush->n_face)
	{
		*ct++=0xFF;
		*ct++=0x14;

		for(i=0; i<brush->n_face; i++)
		{
			*ct++=0x01;
			ct=BTGE_Voxel_EmitPlane4(ct, brush->norm+i*4);
			ct=BTGE_Voxel_EmitPlane4(ct, brush->sdir+i*4);
			ct=BTGE_Voxel_EmitPlane4(ct, brush->tdir+i*4);
			ct=BTGE_Voxel_EmitString(ct, brush->tex[i]);
		}

		*ct++=0;

		*ct++=0xFF;
		*ct++=0x15;

		return(ct);
	}

	if(brush->mesh)
	{
		*ct++=0xFF;
		*ct++=0x14;

		*ct++=0x08;
		ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->n_xyz);
		for(i=0; i<brush->mesh->n_xyz; i++)
			{ ct=BTGE_Voxel_EmitVec3(ct, brush->mesh->xyz+i*3); }

		*ct++=0x09;
		ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->n_st);
		for(i=0; i<brush->mesh->n_st; i++)
			{ ct=BTGE_Voxel_EmitVec2(ct, brush->mesh->st+i*2); }

		*ct++=0x0C;
		ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->n_pts);
		for(i=0; i<brush->mesh->n_pts; i++)
		{
			if(brush->mesh->pts2)
			{
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->pts2[i*2+0]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->pts2[i*2+1]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->pts2[i*2+2]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->pts2[i*2+3]);
			}else
			{
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->pts[i*2+0]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->pts[i*2+1]);
				ct=BTGE_Voxel_EmitVLI(ct, 0);
				ct=BTGE_Voxel_EmitVLI(ct, 0);
			}
		}

		*ct++=0x0D;
		ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->n_face);
		for(i=0; i<brush->mesh->n_face; i++)
		{
			if(brush->mesh->face2)
			{
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->face2[i*4+0]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->face2[i*4+1]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->face2[i*4+2]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->face2[i*4+3]);
			}else
			{
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->face[i*2+0]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->face[i*2+1]);
				ct=BTGE_Voxel_EmitVLI(ct, 9);
				ct=BTGE_Voxel_EmitVLI(ct, 0);
			}
		}

		if(brush->mesh->ftex2)
		{
			*ct++=0x0E;
			ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->n_ftex);
			for(i=0; i<brush->mesh->n_ftex; i++)
			{
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->ftex2[i*4+0]);
				ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->ftex2[i*4+1]);
				ct=BTGE_Voxel_EmitString(ct, brush->mesh->ftex2n[i]);
			}
		}else
		{
			*ct++=0x0E;
			ct=BTGE_Voxel_EmitVLI(ct, 1);

			ct=BTGE_Voxel_EmitVLI(ct, 0);
			ct=BTGE_Voxel_EmitVLI(ct, brush->mesh->n_face);
			ct=BTGE_Voxel_EmitString(ct, brush->mesh->tex);
		}

		*ct++=0;

		*ct++=0xFF;
		*ct++=0x15;

		return(ct);
	}


	return(ct);
}

BTGE_API byte *BTGE_Voxel_FlattenRegionSEntityBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	BTGE_SEntity *sent, byte *obuf, int osz)
{
	BTGE_SEntPair *kcur;
	BTGE_Brush *bcur;
	byte *ct, *cte;
	
	ct=obuf; cte=obuf+osz;
	
	*ct++=0xFF;
	*ct++=0x12;

	kcur=sent->keys;
	while(kcur)
	{
		ct=BTGE_Voxel_EmitString(ct, kcur->name);
		ct=BTGE_Voxel_EmitString(ct, kcur->value);
		kcur=kcur->next;
	}

	*ct++=0x00;

	bcur=sent->brush;
	while(bcur)
	{
		ct=BTGE_Voxel_FlattenRegionBrushBuffer(world,
			rgn, bcur, ct, cte-ct);
		bcur=bcur->enext;
	}

	*ct++=0xFF;
	*ct++=0x13;
	
	return(ct);
}

BTGE_API byte *BTGE_Voxel_FlattenRegionSEntityListBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	BTGE_SEntity *sent, byte *obuf, int osz)
{
	BTGE_SEntity *scur;
	byte *ct, *cte;
	
	ct=obuf; cte=obuf+osz;
	
	if(!sent)
		return(ct);
	
	*ct++=0xFF;
	*ct++=0x10;
	
	scur=sent;
	while(scur)
	{
		ct=BTGE_Voxel_FlattenRegionSEntityBuffer(
			world, rgn, scur, ct, cte-ct);
		scur=sent->next;
	}

	*ct++=0xFF;
	*ct++=0x11;
	return(ct);
}

BTGE_API byte *BTGE_Voxel_FlattenRegionSEntityDeltaListBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn,
	BTGE_SEntity *sent, byte *obuf, int osz)
{
	BTGE_SEntity *scur;
	byte *ct, *cte;
	
	ct=obuf; cte=obuf+osz;
	
	//check if anything has changed
	scur=sent;
	while(scur)
	{
		if(scur->flags&(BTGE_SEFL_NEW|BTGE_SEFL_DIRTY))
			break;
		scur=scur->next;
	}
	
	if(!scur)
	{
		return(ct);
	}

	return(ct);
}

BTGE_API byte *BTGE_Voxel_DecodeBrushBuffer(
	BTGE_BrushWorld *world, BTGE_Brush *brush,
	byte *ibuf, int isz)
{
	char tb[256];
	float vs[4], vt[4], tn[4];
	byte *cs, *cs1, *cse;
	char *tex;
	int i, j, k, l, n, m;
	int ox, oy, oz, xs, ys, zs, xst, yst, zst, fl, vsz;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x14))
	{
		printf("BTGE_Voxel_DecodeBrushBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

	n=0;
	while(cs<cse)
	{
		if(cs[0]==0x00)
			{ cs++; break; }

		if(cs[0]==0x01)
		{
			cs++;
			
			cs=BTGE_Voxel_DecodePlane4(cs, tn);
			cs=BTGE_Voxel_DecodePlane4(cs, vs);
			cs=BTGE_Voxel_DecodePlane4(cs, vt);
			cs=BTGE_Voxel_DecodeString(cs, tb, 256);

			tex=dystrdup(tb);

			BTGE_Brush_CheckExpandFaces(brush, n+1);
			V4F_COPY(tn, brush->norm+n*4);
			V4F_COPY(vs, brush->sdir+n*4);
			V4F_COPY(vt, brush->tdir+n*4);
			brush->tex[n]=tex;
			n++;

			continue;
		}
		
		break;
	}

	if((cs[0]==0xFF) && (cs[1]==0x15))
	{
		cs+=2;
	}else
	{
		cs=BTGE_Voxel_DecodeFindNextMarker(cs, cse);
	}
	return(cs);
}

BTGE_API byte *BTGE_Voxel_DecodeSEntityBuffer(
	BTGE_BrushWorld *world, BTGE_SEntity *sent,
	byte *ibuf, int isz)
{
	char tb1[256], tb2[4096];
	float vs[4], vt[4], tn[4];
	BTGE_Brush *btmp;
	byte *cs, *cs1, *cse;
	char *tex;
	int i, j, k, l, n, m;
//	int ox, oy, oz, xs, ys, zs, xst, yst, zst, fl, vsz;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x12))
	{
		printf("BTGE_Voxel_DecodeSEntityBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

	n=0;
	while(cs<cse)
	{
		if(cs[0]==0x00)
			{ cs++; break; }

		cs=BTGE_Voxel_DecodeString(cs, tb1, 256);
		cs=BTGE_Voxel_DecodeString(cs, tb2, 4096);
		BTGE_SEnt_SetStr(sent, tb1, tb2);
	}

	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==0x13))
			{ cs+=2; break; }

		if((cs[0]==0xFF) && (cs[1]==0x15))
		{
			btmp=BTGE_Brush_New(0);
			cs=BTGE_Voxel_DecodeBrushBuffer(world, btmp, cs, cse-cs);
			BTGE_BrushWorld_AddEntityBrush(world, sent, btmp);
			continue;
		}

		if(cs[0]==0xFF)
		{
			cs=BTGE_Voxel_DecodeFindNextMarker(cs+2, cse);
			continue;
		}

		cs=BTGE_Voxel_DecodeFindNextMarker(cs, cse);
	}

	return(cs);
}

BTGE_API byte *BTGE_Voxel_DecodeSEntityListBuffer(
	BTGE_BrushWorld *world, BTGE_SEntity **rsent,
	byte *ibuf, int isz)
{
	BTGE_SEntity *stmp, *slst;
	byte *cs, *cs1, *cse;

	cs=ibuf; cse=ibuf+isz;

	if((cs[0]!=0xFF) || (cs[1]!=0x10))
	{
		printf("BTGE_Voxel_DecodeSEntityBuffer: Bad Init Marker\n");
		return(ibuf);
	}
	cs+=2;

	slst=NULL;
	while(cs<cse)
	{
		if((cs[0]==0xFF) && (cs[1]==0x11))
			{ cs+=2; break; }

		if((cs[0]==0xFF) && (cs[1]==0x15))
		{
//			btmp=BTGE_Brush_New(0);
			stmp=gctalloc("lbxgl_sentity_s", sizeof(BTGE_SEntity));
			BTGE_SEnt_AddEntity(&slst, stmp);

			cs=BTGE_Voxel_DecodeSEntityBuffer(world, stmp, cs, cse-cs);
//			BTGE_BrushWorld_AddEntityBrush(world, sent, btmp);
			continue;
		}

		if(cs[0]==0xFF)
		{
			cs=BTGE_Voxel_DecodeFindNextMarker(cs+2, cse);
			continue;
		}

		cs=BTGE_Voxel_DecodeFindNextMarker(cs, cse);
	}

	*rsent=slst;

	return(cs);
}

BTGE_API byte *BTGE_Voxel_DecodeRegionSEntityListBuffer(
	BTGE_BrushWorld *world, BTGE_VoxelRegion *rgn, byte *ibuf, int isz)
{
	BTGE_SEntity *scur, *slst;
	byte *cs;
	
	cs=BTGE_Voxel_DecodeSEntityListBuffer(world, &slst, ibuf, isz);
	
	scur=slst;
	while(scur)
	{
		scur->rgn=rgn;
		scur=scur->next;
	}

	rgn->sentity=slst;
	return(cs);
}
