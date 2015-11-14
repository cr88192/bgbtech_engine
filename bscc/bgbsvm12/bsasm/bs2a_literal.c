#include <bs2asm.h>

void BS2ASM_LitCheckExpand(BS2ASM_Context *ctx)
{
	int i;

	if(!ctx->lit)
	{
		ctx->lit=bs2asm_cmalloc(ctx, 256*sizeof(BS2ASM_TagEntity *));
		ctx->nlit=1;
		ctx->mlit=256;
		ctx->lit[0]=NULL;
		return;
	}
	
	if((ctx->nlit+1)>=ctx->mlit)
	{
		i=ctx->mlit+(ctx->mlit>>1);
		ctx->lit=bs2asm_crealloc(ctx,
			ctx->lit, i*sizeof(BS2ASM_TagEntity *));
		ctx->mlit=i;
	}
}

int BS2ASM_HashString(char *str)
{
	int i;
	char *s;
	
	if(!str)return(0);
	
	i=0; s=str;
	while(*s)i=(i*251)+(*s++);
	return(i);
}

int BS2ASM_HashString2(char *str0, char *str1)
{
	int i;

	i=BS2ASM_HashString(str0);
	i=(i*251)+BS2ASM_HashString(str1);
	return(i);
}

int BS2ASM_HashString3(char *str0, char *str1, char *str2)
{
	int i;

	i=BS2ASM_HashString(str0);
	i=(i*251)+BS2ASM_HashString(str1);
	i=(i*251)+BS2ASM_HashString(str2);
	return(i);
}

int BS2ASM_LitIndexSym(BS2ASM_Context *ctx, char *sym)
{
	int i, h;

	if(!sym)return(0);

	h=BS2ASM_HashString2("Symbol:", sym);
	h=(h>>8)&(BS2ASM_SZIDXHASH-1);
	i=ctx->idxhash[h];
	if(i && !strcmp(ctx->lit[i]->tagtype, "Symbol"))
		if(!strcmp(ctx->lit[i]->name, sym))
			return(i);

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "Symbol"))
		{
			if(!strcmp(ctx->lit[i]->name, sym))
			{
				ctx->idxhash[h]=i;
				return(i);
			}
		}
	}
	
	BS2ASM_LitIndexUTF8(ctx, "Symbol");
	BS2ASM_LitIndexUTF8(ctx, sym);
	
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "Symbol");
	ctx->lit[i]->name=bs2asm_strdup(sym);
	ctx->idxhash[h]=i;
	return(i);
}

int BS2ASM_LitIndexString(BS2ASM_Context *ctx, char *sym)
{
	int i, h;

	if(!sym)return(0);

	h=BS2ASM_HashString2("String:", sym);
	h=(h>>8)&(BS2ASM_SZIDXHASH-1);
	i=ctx->idxhash[h];
	if(i && !strcmp(ctx->lit[i]->tagtype, "String"))
		if(!strcmp(ctx->lit[i]->name, sym))
			return(i);

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "String"))
		{
			if(!strcmp(ctx->lit[i]->name, sym))
			{
				ctx->idxhash[h]=i;
				return(i);
			}
		}
	}

	BS2ASM_LitIndexUTF8(ctx, "String");
	BS2ASM_LitIndexUTF8(ctx, sym);
	
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "String");
	ctx->lit[i]->name=bs2asm_strdup(sym);
	ctx->idxhash[h]=i;
	return(i);
}

int BS2ASM_LitIndexUTF8(BS2ASM_Context *ctx, char *sym)
{
	int i, h;

	if(!sym)return(0);

	h=BS2ASM_HashString2("UTF8:", sym);
	h=(h>>8)&(BS2ASM_SZIDXHASH-1);
	i=ctx->idxhash[h];
	if(i && !strcmp(ctx->lit[i]->tagtype, "UTF8"))
		if(!strcmp(ctx->lit[i]->name, sym))
			return(i);

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "UTF8"))
		{
			if(!strcmp(ctx->lit[i]->name, sym))
			{
				ctx->idxhash[h]=i;
				return(i);
			}
		}
	}
	
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "UTF8");
	ctx->lit[i]->name=bs2asm_strdup(sym);
	ctx->idxhash[h]=i;
	return(i);
}

int BS2ASM_LitIndexInt(BS2ASM_Context *ctx, s32 val)
{
	int i;

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "Int32"))
		{
			if(ctx->lit[i]->val.intval==val)
				return(i);
		}
	}

	BS2ASM_LitIndexUTF8(ctx, "Int32");

	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "Int32");
	ctx->lit[i]->val.intval=val;
	return(i);
}

int BS2ASM_LitIndexLong(BS2ASM_Context *ctx, s64 val)
{
	int i;

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "Int64"))
		{
			if(ctx->lit[i]->val.intval==val)
				return(i);
		}
	}
	
	BS2ASM_LitIndexUTF8(ctx, "Int64");
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "Int64");
	ctx->lit[i]->val.intval=val;
	return(i);
}

int BS2ASM_LitIndexFloat(BS2ASM_Context *ctx, f32 val)
{
	int i;

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "Float32"))
		{
			if(ctx->lit[i]->val.realval==val)
				return(i);
		}
	}
	
	BS2ASM_LitIndexUTF8(ctx, "Float32");
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "Float32");
	ctx->lit[i]->val.realval=val;
	return(i);
}

int BS2ASM_LitIndexDouble(BS2ASM_Context *ctx, f64 val)
{
	int i;

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "Float64"))
		{
			if(ctx->lit[i]->val.realval==val)
				return(i);
		}
	}
	
	BS2ASM_LitIndexUTF8(ctx, "Float64");
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "Float64");
	ctx->lit[i]->val.realval=val;
	return(i);
}

int BS2ASM_LitIndexInt32(BS2ASM_Context *ctx, s32 val)
	{ return(BS2ASM_LitIndexInt(ctx, val)); }
int BS2ASM_LitIndexInt64(BS2ASM_Context *ctx, s64 val)
	{ return(BS2ASM_LitIndexLong(ctx, val)); }
int BS2ASM_LitIndexFloat32(BS2ASM_Context *ctx, f32 val)
	{ return(BS2ASM_LitIndexFloat(ctx, val)); }
int BS2ASM_LitIndexFloat64(BS2ASM_Context *ctx, f64 val)
	{ return(BS2ASM_LitIndexDouble(ctx, val)); }

int BS2ASM_LitIndexTagEnt(BS2ASM_Context *ctx, BS2ASM_TagEntity *ent)
{
	int i, h;

	if(!ent)return(0);

	h=BS2ASM_HashString(ent->tagtype);
	h=(h*251)+((int)ent);
	h=(h>>8)&(BS2ASM_SZIDXHASH-1);
	i=ctx->idxhash[h];
	if(i && (ctx->lit[i]==ent))
		return(i);

	for(i=1; i<ctx->nlit; i++)
	{
		if(ctx->lit[i]==ent)
		{
			ctx->idxhash[h]=i;
			return(i);
		}
	}

	for(i=1; i<ctx->nlit; i++)
	{
		if(!ctx->lit[i])
		{
			ctx->lit[i]=ent;
			ctx->idxhash[h]=i;
			return(i);
		}
	}
	
	BS2ASM_LitIndexUTF8(ctx, ent->tagtype);

	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=ent;
	ctx->idxhash[h]=i;
	return(i);
}


int BS2ASM_LitIndexCheckNameType(BS2ASM_Context *ctx,
	int idx, char *name, char *type)
{
	if(!strcmp(ctx->lit[idx]->tagtype, "NameType"))
	{
		if(ctx->lit[idx]->name && name)
		{
			if(strcmp(ctx->lit[idx]->name, name))
				return(0);
		}else if(ctx->lit[idx]->name || name)
			return(0);
		if(ctx->lit[idx]->type && type)
		{
			if(strcmp(ctx->lit[idx]->type, type))
				return(0);
		}else if(ctx->lit[idx]->type || type)
			return(0);
			
		return(idx);
	}
	return(0);
}

int BS2ASM_LitIndexNameType(BS2ASM_Context *ctx, char *name, char *type)
{
	int i, h;

//	if(!sym)return(0);

	h=BS2ASM_HashString3("NameType:", name, type);
	h=(h>>8)&(BS2ASM_SZIDXHASH-1);
	i=ctx->idxhash[h];
	if(i && !strcmp(ctx->lit[i]->tagtype, "NameType"))
		if(BS2ASM_LitIndexCheckNameType(ctx, i, name, type))
			return(i);

	for(i=1; i<ctx->nlit; i++)
	{
		if(BS2ASM_LitIndexCheckNameType(ctx, i, name, type))
		{
			ctx->idxhash[h]=i;
			return(i);
		}
	}

	BS2ASM_LitIndexUTF8(ctx, "NameType");

	BS2ASM_LitIndexSym(ctx, name);
	BS2ASM_LitIndexUTF8(ctx, type);
	
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "NameType");
	ctx->lit[i]->name=bs2asm_strdup(name);
	ctx->lit[i]->type=bs2asm_strdup(type);
	ctx->idxhash[h]=i;
	return(i);
}

#if 0
int BS2ASM_LitIndexClassRef2(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *scope, char *sym)
{
	int i;

	if(!sym)return(0);

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "ClassRef"))
		{
			if((ctx->lit[i]->scope==scope) &&
				!strcmp(ctx->lit[i]->name, sym))
				return(i);
		}
	}

	BS2ASM_LitIndexUTF8(ctx, "ClassRef");
	BS2ASM_LitIndexUTF8(ctx, sym);
	
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "ClassRef");
	ctx->lit[i]->scope=scope;
	ctx->lit[i]->name=bs2asm_strdup(sym);
	return(i);
}

int BS2ASM_LitIndexClassRef(BS2ASM_Context *ctx, char *sym)
	{ return(BS2ASM_LitIndexClassRef2(ctx, ctx->curscope, sym)); }
#endif

int BS2ASM_LitIndexBlob(BS2ASM_Context *ctx, byte *data, int sz)
{
	int i;

	for(i=1; i<ctx->nlit; i++)
	{
		if(!strcmp(ctx->lit[i]->tagtype, "Blob"))
		{
			if(ctx->lit[i]->szbody!=sz)
				continue;
			if(!memcmp(ctx->lit[i]->body, data, sz))
				return(i);
		}
	}
	
	BS2ASM_LitIndexUTF8(ctx, "Blob");
	BS2ASM_LitCheckExpand(ctx);
	
	i=ctx->nlit++;
	ctx->lit[i]=BS2ASM_NewTagEntity(ctx, "Blob");
	ctx->lit[i]->body=bs2asm_cmalloc(ctx, sz);
	ctx->lit[i]->szbody=sz;
	ctx->lit[i]->maxbody=sz;
	memcpy(ctx->lit[i]->body, data, sz);

	return(i);
}

int BS2ASM_LitIndexBlock(BS2ASM_Context *ctx, BS2ASM_Block *blk)
{
	return(blk->litidx);
}
