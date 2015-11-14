#include <bgbdy.h>

void BinDec_ReserveIndexLump(dysObjectStream ctx, int idx)
{
	int i;

	if(!ctx->arr)
	{
		i=256;
		while(i<=(idx+1))i=i+(i>>1);
		ctx->arr=gcalloc(i*sizeof(BGBDY_ObjectLump *));
		ctx->n_arr=1; ctx->m_arr=i;
	}

	if((idx+1)>=ctx->m_arr)
	{
		i=ctx->m_arr;
		while(i<=(idx+1))i=i+(i>>1);
		ctx->arr=gcrealloc(ctx->arr, i*sizeof(BGBDY_ObjectLump *));
		ctx->m_arr=i;
	}
}

int BinDec_CommitIndexLump(dysObjectStream ctx, int idx)
{
//	int i;

//	if(!value)return(0);
//	i=BinEnc_LookupValue(ctx, value);
//	if(i>=0)return(i);

	if((idx>=0) && (idx<ctx->n_arr))
	{
		if(ctx->arr[idx])
			return(idx);
	}

	
	BinDec_ReserveIndexLump(ctx, idx);
	
//	i=ctx->n_arr++;
	ctx->arr[idx]=gcalloc(sizeof(BGBDY_ObjectLump));
//	ctx->arr[idx]->value=value;
	ctx->n_arr=idx+1;
	return(idx);
}

char *BinDec_GetIndexStrSym(dysObjectStream ctx, int idx)
{
	char tb[256];
	
	if(idx<=0)
		return(NULL);
	if(idx>=ctx->n_arr)
		return(NULL);
	if(!ctx->arr[idx])
		return(NULL);
	
	memset(tb, 0, 256);
	memcpy(tb, ctx->arr[idx]->data, ctx->arr[idx]->size);
	return(dystrsym(tb));
}

dyt BinDec_GetIndexValue(dysObjectStream ctx, int idx)
{
	if(idx<=0)
		return(NULL);
	if(idx>=ctx->n_arr)
		return(NULL);
	if(!ctx->arr[idx])
		return(NULL);
	return(ctx->arr[idx]->value);
}

char *BinDec_ReadLumpASCII(dysObjectStream ctx)
{
	char tb[4096];
	
	memset(tb, 0, 4096);
	memcpy(tb, ctx->lmp->data, ctx->lmp->size);
	return(gcrstrdup(tb));
}

dyt BinDec_DecodeBinaryGeneric(dysObjectStream ctx, char *tyn)
{
	dyt va[256];
	dyt vb[256];
	dyt lst, val, cur, v0, v1;
	char *s;
	int i, n;

	if(!strcmp(tyn, "_null_t"))
		return(NULL);
	if(!strcmp(tyn, "_undefined_t"))
		return(UNDEFINED);

	if(!strcmp(tyn, "_bool_t"))
		return(dybool(dysBinaryDecodeVLI(ctx)));

	if(!strcmp(tyn, "_fixint_t"))
		return(dyint(dysBinaryDecodeSVLI(ctx)));
	if(!strcmp(tyn, "_int_t"))
		return(dyint(dysBinaryDecodeSVLI(ctx)));
	if(!strcmp(tyn, "_long_t"))
		return(dyint(dysBinaryDecodeSVLI(ctx)));

	if(!strcmp(tyn, "_flonum_t"))
		return(dyflonum(dysBinaryDecodeF32(ctx)));
	if(!strcmp(tyn, "_float_t"))
		return(dyfloat(dysBinaryDecodeF32(ctx)));
	if(!strcmp(tyn, "_double_t"))
		return(dydouble(dysBinaryDecodeF64(ctx)));

	if(!strcmp(tyn, "_symbol_t"))
		return(dysymbol(BinDec_ReadLumpASCII(ctx)));
	if(!strcmp(tyn, "_keyword_t"))
		return(dysymbol(BinDec_ReadLumpASCII(ctx)));
	if(!strcmp(tyn, "_string_t"))
		return(dysymbol(BinDec_ReadLumpASCII(ctx)));

	if(!strcmp(tyn, "_list_t"))
	{
		n=0;
		while(ctx->lmp->ip<ctx->lmp->ipe)
			va[n++]=dysBinaryDecodeValue(ctx);
		if(!n)return(NULL);

		val=dysBinaryGetValue(ctx);
		if(val)
		{
			cur=val;
			for(i=0; i<n; i++)
			{
				dysetcar(cur, va[i]);
				cur=dycdr(cur);
			}
			return(val);
		}

		lst=NULL;
		for(i=n-1; i>=0; i--)
			lst=dycons(va[i], lst);
		return(lst);
	}
	if(!strcmp(tyn, "_cons_t"))
	{
		n=0;
		while(ctx->lmp->ip<ctx->lmp->ipe)
			va[n++]=dysBinaryDecodeValue(ctx);		
		if(n==1)return(NULL);
		if(n==2)return(dylist1(va[0]));

		val=dysBinaryGetValue(ctx);
		if(val)
		{
			cur=val;
			for(i=0; i<(n-1); i++)
			{
				dysetcar(cur, va[i]);
				cur=dycdr(cur);
			}
			dysetcdr(cur, va[i]);
			return(val);
		}
		
		lst=va[n-1];
		for(i=n-2; i>=0; i--)
			lst=dycons(va[i], lst);
		return(lst);
	}

	if(!strcmp(tyn, "_array_t"))
	{
		v0=dysBinaryDecodeValue(ctx);
		s=dysymbolv(v0);
		if(!s)s="r";

		n=0;
		while(ctx->lmp->ip<ctx->lmp->ipe)
			va[n++]=dysBinaryDecodeValue(ctx);		
//		if(n<2)return(NULL);

		val=dysBinaryGetValue(ctx);
		if(!val)
		{
//			val=dyarray(n);
			val=(dyt)dycNewArray(s, n);
			return(val);
		}

//		val=dyarray(n);
		for(i=0; i<n; i++)
		{
//			dyarraysetidx(val, i, va[i]);
			dycSetArrayVar((dycArray)val, i, va[i]);
		}
		return(val);
	}

	if(!strcmp(tyn, "_object_t"))
	{
		val=dysBinaryGetValue(ctx);
		if(!val)
		{
			val=dyObject();
			return(val);
		}

		n=dysBinaryDecodeVLI(ctx);
		for(i=0; i<n; i++)
		{
			v0=dysBinaryDecodeValue(ctx);
			v1=dysBinaryDecodeValue(ctx);
			dyBind(val, dysymbolv(v0), v1);
		}

		n=dysBinaryDecodeVLI(ctx);
		for(i=0; i<n; i++)
		{
			v0=dysBinaryDecodeValue(ctx);
			v1=dysBinaryDecodeValue(ctx);
			dySetDelegate(val, dysymbolv(v0), v1);
		}
		
		return(val);
	}
	
	printf("BinEnc_DecodeBinaryGeneric: Unhandled type %s\n", tyn);
	return(UNDEFINED);
}

dyt BinDec_DecodeLump(dysObjectStream ctx, int idx)
{
	BGBDY_ObjVTab *ty;
	dyt val;

	ctx->lmp=ctx->arr[idx];
	ctx->lmp->ip=ctx->lmp->data;
	ctx->lmp->ipe=ctx->lmp->data+ctx->lmp->size;

	ty=BGBDY_GetTypeVTab(ctx->lmp->type);
	if(ty && ty->decodeBinary)
	{
		val=ty->decodeBinary(ctx);
	}else
	{
		val=BinDec_DecodeBinaryGeneric(ctx, ctx->lmp->type);
	}
	
	return(val);
}

BGBDY_API dyt dysBinaryGetValue(dysObjectStream ctx)
	{ return(ctx->lmp->value); }
BGBDY_API char *dysBinaryGetType(dysObjectStream ctx)
	{ return(ctx->lmp->type); }
BGBDY_API int dysBinaryGetSize(dysObjectStream ctx)
	{ return(ctx->lmp->size); }

BGBDY_API int dysBinaryDecodeByte(dysObjectStream ctx)
{
	return(*ctx->lmp->ip++);
}

BGBDY_API int dysBinaryDecodeBytes(dysObjectStream ctx, byte *buf, int sz)
{
	memcpy(buf, ctx->lmp->ip, sz);
	ctx->lmp->ip+=sz;
	return(sz);
}

BGBDY_API int dysBinaryDecodeS16(dysObjectStream ctx)
{
	int i;
	
	i=dysBinaryDecodeByte(ctx);
	i|=dysBinaryDecodeByte(ctx)<<8;
	return((s16)i);
}

BGBDY_API int dysBinaryDecodeS32(dysObjectStream ctx)
{
	int i;
	
	i=dysBinaryDecodeByte(ctx);
	i|=dysBinaryDecodeByte(ctx)<<8;
	i|=dysBinaryDecodeByte(ctx)<<16;
	i|=dysBinaryDecodeByte(ctx)<<24;
	return((s16)i);
}

BGBDY_API s64 dysBinaryDecodeVLI(dysObjectStream ctx)
{
	s64 v;
	v=BGBDY_DecVLI(&(ctx->lmp->ip));
	return(v);
}

BGBDY_API s64 dysBinaryDecodeSVLI(dysObjectStream ctx)
{
	s64 v;
	v=BGBDY_DecSVLI(&(ctx->lmp->ip));
	return(v);
}

BGBDY_API f32 dysBinaryDecodeF32(dysObjectStream ctx)
{
	s64 v;
	v=BGBDY_DecFloat32(&(ctx->lmp->ip));
	return(v);
}

BGBDY_API f64 dysBinaryDecodeF64(dysObjectStream ctx)
{
	s64 v;
	v=BGBDY_DecFloat64(&(ctx->lmp->ip));
	return(v);
}

BGBDY_API dyt dysBinaryDecodeValue(dysObjectStream ctx)
{
	int v;
	v=BGBDY_DecVLI(&(ctx->lmp->ip));
	return(BinDec_GetIndexValue(ctx, v));
}

BGBDY_API dyt dysDecodeBinaryBuffer(byte *buf, int sz)
{
	dysObjectStream ctx;
	byte *cs, *ce;
	int i, j, n;

	ctx=dysNewObjectStream();
	
	cs=buf; ce=buf+sz; n=1;
	while(cs<ce)
	{
		i=BGBDY_DecVLI(&cs);
		j=BGBDY_DecVLI(&cs);
		//comment or terminal:
		if(!i)
		{
			//terminal:
			if(!j)break;
			//comment:
			cs+=j;
			continue;
		}
		
		BinDec_CommitIndexLump(ctx, n);
		ctx->arr[n]->data=cs;
		ctx->arr[n]->ty_idx=i;
		ctx->arr[n]->size=j;
		n++; cs+=j;
	}
	
	for(i=1; i<ctx->n_arr; i++)
	{
		if(!ctx->arr[i])
			continue;
		ctx->arr[i]->type=BinDec_GetIndexStrSym(ctx,
			ctx->arr[i]->ty_idx);
	}

	for(i=1; i<ctx->n_arr; i++)
	{
		ctx->lmp=ctx->arr[i];
		ctx->arr[i]->value=
			BinDec_DecodeLump(ctx, i);
	}

	for(i=1; i<ctx->n_arr; i++)
	{
		ctx->lmp=ctx->arr[i];
		ctx->arr[i]->value=
			BinDec_DecodeLump(ctx, i);
	}
	return(ctx->arr[1]->value);
//	return(UNDEFINED);
}
