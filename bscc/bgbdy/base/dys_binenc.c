#include <bgbdy.h>

int BinEnc_LookupValue(dysObjectStream ctx, dyt value)
{
	int i;

	if(!value)return(0);
	if(!ctx->arr)return(-1);

	for(i=1; i<ctx->n_arr; i++)
		if(ctx->arr[i])
	{
		if(ctx->arr[i]->value==value)
			return(i);
		if(dyListEqualP(ctx->arr[i]->value, value))
			return(i);
	}
			
	return(-1);
}

int BinEnc_IndexValue(dysObjectStream ctx, dyt value)
{
	int i;

	if(!value)return(0);
	i=BinEnc_LookupValue(ctx, value);
	if(i>=0)return(i);
	
	if(!ctx->arr)
	{
		i=256;
		ctx->arr=gcalloc(i*sizeof(BGBDY_ObjectLump *));
		ctx->n_arr=1; ctx->m_arr=i;
	}

	if((ctx->n_arr+1)>=ctx->m_arr)
	{
		i=ctx->m_arr+(ctx->m_arr>>1);
		ctx->arr=gcrealloc(ctx->arr, i*sizeof(BGBDY_ObjectLump *));
		ctx->m_arr=i;
	}

	i=ctx->n_arr++;
	ctx->arr[i]=gcalloc(sizeof(BGBDY_ObjectLump));
	ctx->arr[i]->value=value;
	return(i);
}

void BinEnc_EncodeBinaryObject(dysObjectStream ctx, dyt val)
{
	BGBGC_Object *pobj;
	int i, n;

	pobj=(BGBGC_Object *)val;

	dysBinaryEncodeVLI(ctx, pobj->cnt);

	for(i=0; i<pobj->cnt; i++)
	{
		if(!pobj->var[i])continue;
		dysBinaryEncodeValue(ctx, (dyt)pobj->var[i]);
		dysBinaryEncodeValue(ctx, dytfUnwrap(pobj->val[i]));
	}

	dysBinaryEncodeVLI(ctx, pobj->dcnt);

	for(i=0; i<pobj->dcnt; i++)
	{
		if(!pobj->dvar[i])continue;
		dysBinaryEncodeValue(ctx, (dyt)pobj->dvar[i]);
		dysBinaryEncodeValue(ctx, pobj->dval[i]);
	}
}

void BinEnc_EncodeBinaryGeneric(dysObjectStream ctx, dyt val)
{
	dyt c, t;
	dyt *pa;
	char *tyn, *s;
	int i, n;

	tyn=NULL;

	if(!val)tyn="_null_t";
	if(val==UNDEFINED)tyn="_undefined_t";

	if(tyn)
	{
		ctx->lmp->type=tyn;
		return;
	}
	
	if((val==BGBDY_TRUE)||(val==BGBDY_FALSE))
	{
		ctx->lmp->type="_bool_t";
		dysBinaryEncodeVLI(ctx, val==BGBDY_TRUE);
		return;
	}

	if(dyintp(val))
	{
		dysBinaryEncodeInt(ctx, dyintv(val));
		return;
	}

	if(dylongp(val))
	{
		dysBinaryEncodeInt(ctx, dyintv(val));
		return;
	}

	if(dyfloatp(val))
	{
		dysBinaryEncodeF32(ctx, dyrealv(val));
		return;
	}

	if(dyrealp(val))
	{
		dysBinaryEncodeF64(ctx, dyrealv(val));
		return;
	}

	if(dysymbolp(val))
	{
		dysBinaryEncodeString(ctx, dysymbolv(val));
		return;
	}

	if(dykeywordp(val))
	{
		dysBinaryEncodeString(ctx, dysymbolv(val));
		return;
	}

	if(dystringp(val))
	{
		dysBinaryEncodeString(ctx, dysymbolv(val));
		return;
	}

	if(dylistp(val))
	{
		ctx->lmp->type="_list_t";
		c=val;
		while(dyconsp(c))
		{
			t=dycar(c);
			dysBinaryEncodeValue(ctx, t);
			c=dycdr(c);
		}
		return;
	}

	if(dyconsp(val))
	{
		ctx->lmp->type="_cons_t";
		c=val;
		while(dyconsp(c))
		{
			t=dycar(c);
			dysBinaryEncodeValue(ctx, t);
			c=dycdr(c);
		}
		dysBinaryEncodeValue(ctx, c);
		return;
	}

#if 0
	if(dyarrayp(val))
	{
		pa=dyarrayv(val); n=dyarraysz(val);
		for(i=0; i<n; i++)
			dysBinaryEncodeValue(ctx, pa[i]);
		return;
	}
#endif
	
	if(dycArrayP(val))
	{
		s=dycArraySig((dycArray)val);
		dysBinaryEncodeValue(ctx, dysymbol(s));

		n=dycArraySz((dycArray)val);
		for(i=0; i<n; i++)
		{
			t=dycGetArrayVar((dycArray)val, i);
			dysBinaryEncodeValue(ctx, t);
		}
		return;
	}

	if(dyObjectp(val))
	{
		BinEnc_EncodeBinaryObject(ctx, val);
		return;
	}
	
	tyn=dygettype(val);
	if(!tyn)tyn="(NULL)";
	printf("BinEnc_EncodeBinaryGeneric: Unhandled type %s @%p\n", tyn, val);
}

void BinEnc_FlattenLump(dysObjectStream ctx, int idx)
{
	BGBDY_ObjVTab *ty;
	char *tyn;
	dyt val;

	ctx->lmp=ctx->arr[idx];

	if(!ctx->lmp)return;
	
	val=ctx->lmp->value;
	tyn=NULL;

	if(!val)tyn="_null_t";
	if(val==UNDEFINED)tyn="_undefined_t";

	if(tyn)
	{
		ctx->lmp->type=tyn;
		BinEnc_IndexValue(ctx, dysymbol(tyn));
		return;
	}

	ty=BGBDY_GetObjVTab(val);
	if(ty && ty->encodeBinary)
	{
		ty->encodeBinary(ctx, val);
		ctx->lmp->size=ctx->lmp->ip-ctx->lmp->data;
	}else
	{
		BinEnc_EncodeBinaryGeneric(ctx, val);
		ctx->lmp->size=ctx->lmp->ip-ctx->lmp->data;
	}

	tyn=ctx->lmp->type;
	if(!tyn)
	{
		tyn=dygettype(val);
		ctx->lmp->type=tyn;
	}

	BinEnc_IndexValue(ctx, dysymbol(tyn));
}

void BinEnc_FlattenImage(dysObjectStream ctx)
{
	char *str;
	int i;
	
	i=1;
	while(i<ctx->n_arr)
	{
		BinEnc_FlattenLump(ctx, i++);
	}

	ctx->imglump=gcalloc(sizeof(BGBDY_ObjectLump));
	ctx->lmp=ctx->imglump;

	str=ctx->magic;
	if(!str)
		str="dyBinEnc0";
	i=strlen(str);
	dysBinaryEncodeVLI(ctx, 0);
	dysBinaryEncodeVLI(ctx, i);
	dysBinaryEncodeBytes(ctx, str, i);

	i=1;
	while(i<ctx->n_arr)
	{
//		BinEnc_FlattenLump(ctx, i++);

		if(0)
		{
			printf("%d: %s %d %s\n", i,
				ctx->arr[i]->type, ctx->arr[i]->size,
				dyToString(ctx->arr[i]->value));
		}

		dysBinaryEncodeValue(ctx, dysymbol(ctx->arr[i]->type));
		dysBinaryEncodeVLI(ctx, ctx->arr[i]->size);
		dysBinaryEncodeBytes(ctx,
			ctx->arr[i]->data,
			ctx->arr[i]->size);
		i++;
	}
	dysBinaryEncodeVLI(ctx, 0);
	dysBinaryEncodeVLI(ctx, 0);

	ctx->lmp->size=ctx->lmp->ip-ctx->lmp->data;
}

void BinEnc_CheckEmitBytes(dysObjectStream ctx, int cnt)
{
	int i, j;

	if(!ctx->lmp->data)
	{
		i=16;
		while(cnt>i)i=i+(i>>1);
		ctx->lmp->data=gcalloc(i);
		ctx->lmp->ip=ctx->lmp->data;
		ctx->lmp->ipe=ctx->lmp->ip+i;
	}
	
	if((ctx->lmp->ip+cnt)>ctx->lmp->ipe)
	{
		j=ctx->lmp->ip-ctx->lmp->data;
		i=ctx->lmp->ipe-ctx->lmp->data;
		while((j+cnt)>i)i=i+(i>>1);
		ctx->lmp->data=gcrealloc(ctx->lmp->data, i);
		ctx->lmp->ip=ctx->lmp->data+j;
		ctx->lmp->ipe=ctx->lmp->data+i;
	}
}

BGBDY_API void dysBinaryEncodeBytes(dysObjectStream ctx, byte *data, int sz)
{
	BinEnc_CheckEmitBytes(ctx, sz);
	memcpy(ctx->lmp->ip, data, sz);
	ctx->lmp->ip+=sz;
}

BGBDY_API void dysBinaryEncodeByte(dysObjectStream ctx, int value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	*ctx->lmp->ip++=value;
}

BGBDY_API void dysBinaryEncodeS16(dysObjectStream ctx, int value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	ctx->lmp->ip=BGBDY_EncS16(ctx->lmp->ip, value);
}

BGBDY_API void dysBinaryEncodeS32(dysObjectStream ctx, int value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	ctx->lmp->ip=BGBDY_EncS32(ctx->lmp->ip, value);
}

BGBDY_API void dysBinaryEncodeString(dysObjectStream ctx, char *str)
{
	dysBinaryEncodeBytes(ctx, str, strlen(str));
}

BGBDY_API void dysBinaryEncodeStringZ(dysObjectStream ctx, char *str)
{
	dysBinaryEncodeBytes(ctx, str, strlen(str)+1);
}

BGBDY_API void dysBinaryEncodeVLI(dysObjectStream ctx, s64 value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	ctx->lmp->ip=BGBDY_EncVLI(ctx->lmp->ip, value);
}

BGBDY_API void dysBinaryEncodeSVLI(dysObjectStream ctx, s64 value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	ctx->lmp->ip=BGBDY_EncSVLI(ctx->lmp->ip, value);
}

BGBDY_API void dysBinaryEncodeVLF(dysObjectStream ctx, f64 value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	ctx->lmp->ip=BGBDY_EncVLF(ctx->lmp->ip, value);
}

BGBDY_API void dysBinaryEncodeValue(dysObjectStream ctx, dyt value)
{
	int i;
	
	i=BinEnc_IndexValue(ctx, value);
	dysBinaryEncodeVLI(ctx, i);
}

BGBDY_API void dysBinaryEncodeInt(dysObjectStream ctx, s64 value)
{
	dysBinaryEncodeSVLI(ctx, value);
}

BGBDY_API void dysBinaryEncodeUInt(dysObjectStream ctx, s64 value)
{
	dysBinaryEncodeVLI(ctx, value);
}

BGBDY_API void dysBinaryEncodeF32(dysObjectStream ctx, float value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	ctx->lmp->ip=BGBDY_EncF32(ctx->lmp->ip, value);
}

BGBDY_API void dysBinaryEncodeF64(dysObjectStream ctx, double value)
{
	BinEnc_CheckEmitBytes(ctx, 16);
	ctx->lmp->ip=BGBDY_EncF64(ctx->lmp->ip, value);
}

BGBDY_API dysObjectStream dysNewObjectStream()
{
	BGBDY_ObjectStream *tmp;
	
	tmp=gctalloc("_dys_objectstream_t", sizeof(BGBDY_ObjectStream));
	return(tmp);
}

BGBDY_API void dysFreeObjectStream(dysObjectStream ctx)
{
	int i;

	if(ctx->arr)
	{
		for(i=0; i<ctx->m_arr; i++)
		{
			if(!ctx->arr[i])
				continue;

			if(ctx->arr[i]->data)
				gcfree(ctx->arr[i]->data);
			gcfree(ctx->arr[i]);
			ctx->arr[i]=NULL;
		}
		gcfree(ctx->arr);
		ctx->arr=NULL;
	}
	
	if(ctx->imglump)
	{
		if(ctx->imglump->data)
			gcfree(ctx->imglump->data);
		gcfree(ctx->imglump);
		ctx->imglump=NULL;
	}
	
	gcfree(ctx);
}

BGBDY_API int dysEncodeBinaryBuffer(byte *buf, int msz, dyt value)
{
	dysObjectStream ctx;
	int sz;
	
	ctx=dysNewObjectStream();
	BinEnc_IndexValue(ctx, value);
	BinEnc_FlattenImage(ctx);

	if(buf)
	{
		sz=ctx->imglump->size;
		if(sz>msz)
		{
			*(int *)buf=sz;
			return(-1);
		}
		
		memcpy(buf, ctx->imglump->data, sz);
	}

	sz=ctx->imglump->size;
	dysFreeObjectStream(ctx);
	return(sz);
}
