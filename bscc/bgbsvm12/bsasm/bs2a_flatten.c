#include <bs2asm.h>

BS2ASM_List *BS2ASM_ConvertList(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_List *tmp;
	BS2ASM_TagEntity *cur, *n0;

	if(!ent)return(NULL);

	tmp=BS2ASM_NewList(ctx, NULL);
	
	cur=ent->first;
	while(cur)
	{
		n0=BS2ASM_ConvertTagEnt(ctx, cur);
		if(n0)
		{
			BS2ASM_LitIndexTagEnt(ctx, n0);
			BS2ASM_AddListEntry(tmp, n0);
		}
		
		cur=cur->next;
	}
	
//	BS2ASM_LitIndexTagEnt(ctx, tmp);
	return(tmp);
}

char *BS2ASM_ConvertMethodSignature(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	char tb[1024];
	BS2ASM_TagEntity *cur;
	char *s, *t;

	t=tb;
	*t++='(';
	cur=BS2ASM_FindEntityChildTag(ent, "args");
	if(cur)cur=cur->first;
	while(cur)
	{
		if(!strcmp(cur->tagtype, "var"))
		{
			strcpy(t, cur->type);
			t+=strlen(t);
		}
		if(!strcmp(cur->tagtype, "rest"))
			{ *t++='z'; }
		cur=cur->next;
	}

	*t++=')';
	if(ent->type)strcpy(t, ent->type);
		else strcpy(t, "v");
	t+=strlen(t);
	
	s=bs2asm_strdup(tb);
	return(s);
}

BS2ASM_TagEntity *BS2ASM_EnterScope(BS2ASM_Context *ctx)
{
	BS2ASM_TagEntity *tmp;
	
	tmp=BS2ASM_NewTagEntity(ctx, "ScopeInfo");
	tmp->scope=ctx->curscope;
	ctx->curscope=tmp;
	
	return(tmp);
}

void BS2ASM_ExitScope(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	ctx->curscope=ent->scope;
}

void BS2ASM_SetScopeObject(BS2ASM_Context *ctx, BS2ASM_TagEntity *ent)
{
	int i;

	i=BS2ASM_LitIndexTagEnt(ctx, ent);
	ctx->curscope->validx=i;
}

char *BS2ASM_GetFlagsWithFlag(char *flstr, char *flg)
{
	char tb[256];
	char *s;
	
	if(!flstr)
		return(bs2asm_strdup(flg));
	
	s=flstr;
	while(*s)
	{
		if(!strncmp(s, flg, strlen(flg)))
			return(flstr);
		s++;
	}
	
	strcpy(tb, flstr);
	strcat(tb, flg);
	return(bs2asm_strdup(tb));
}

BS2ASM_TagEntity *BS2ASM_ConvertMethod(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *n0, *n1, *n2, *n3;
	char *s0;
	BS2ASM_TagEntity *tmp, *ts;
	BS2ASM_Block *blk;

	ts=BS2ASM_EnterScope(ctx);

	blk=NULL;
	n0=BS2ASM_FindEntityChildTag(ent, "body");
	if(n0)
	{
		ent->validx=BS2ASM_LitIndexBlob(ctx,
			n0->body, n0->ip-n0->body);

		n3=BS2ASM_CopyList(ctx, n0);
//		if(n3)BS2ASM_SetEntityValueEnt(blk, "labels", n3);

		n0=BS2ASM_FindEntityChildTag(ent, "args");
		if(n0)n0=BS2ASM_ConvertList(ctx, n0);

		n1=BS2ASM_FindEntityChildTag(ent, "vars");
		n1=BS2ASM_ConvertList(ctx, n1);

		n2=BS2ASM_FindEntityChildTag(ent, "catches");
		n2=BS2ASM_ConvertList(ctx, n2);


		BS2ASM_LitIndexTagEnt(ctx, n0);
		BS2ASM_LitIndexTagEnt(ctx, n1);
		BS2ASM_LitIndexTagEnt(ctx, n2);

		blk=BS2ASM_AllocBlock(ctx);
		blk->validx=ent->validx;
		if(n0)BS2ASM_SetEntityValueEnt(blk, "argslist", n0);
		if(n1)BS2ASM_SetEntityValueEnt(blk, "varslist", n1);
		if(n2)BS2ASM_SetEntityValueEnt(blk, "catchlist", n2);
		if(n3)BS2ASM_SetEntityValueEnt(blk, "labels", n3);
	}

	if(!strcmp(ent->tagtype, "s_proto"))
		ent->flagstr=BS2ASM_GetFlagsWithFlag(ent->flagstr, "d");

	s0=BS2ASM_ConvertMethodSignature(ctx, ent);

	BS2ASM_LitIndexSym(ctx, ent->name);
	BS2ASM_LitIndexUTF8(ctx, s0);
	BS2ASM_LitIndexUTF8(ctx, ent->flagstr);
	
	tmp=BS2ASM_NewTagEntity(ctx, "MethodInfo");
	tmp->scope=ctx->curscope;
	tmp->name=ent->name;
	tmp->type=s0;
	tmp->flagstr=ent->flagstr;

	if(blk)
	{
		blk->type=s0;
		tmp->validx=blk->litidx;
	}

	BS2ASM_SetScopeObject(ctx, tmp);

	BS2ASM_ExitScope(ctx, ts);

	return(tmp);
}

BS2ASM_TagEntity *BS2ASM_ConvertVar(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *n0, *n1, *n2, *n3;
	BS2ASM_TagEntity *tmp;
	char *s0, *s1, *s2;
	int i;

	if(!strcmp(ent->tagtype, "s_var"))
		ent->flagstr=BS2ASM_GetFlagsWithFlag(ent->flagstr, "Cd");

	BS2ASM_LitIndexSym(ctx, ent->name);
	BS2ASM_LitIndexUTF8(ctx, ent->type);
	BS2ASM_LitIndexUTF8(ctx, ent->flagstr);
	
	tmp=BS2ASM_NewTagEntity(ctx, "FieldInfo");
	tmp->scope=ctx->curscope;
	tmp->name=ent->name;
	tmp->type=ent->type;
	tmp->flagstr=ent->flagstr;
	return(tmp);
}

BS2ASM_TagEntity *BS2ASM_ConvertCatch(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *n0, *n1, *n2, *n3;
	BS2ASM_TagEntity *tmp;
	char *s0, *s1, *s2, *s3;
	int i;
	
	s0=BS2ASM_GetEntityValue(ent, "begin");
	s1=BS2ASM_GetEntityValue(ent, "end");
	s2=BS2ASM_GetEntityValue(ent, "target");
	s3=BS2ASM_GetEntityValue(ent, "class");
	BS2ASM_LitIndexSym(ctx, s0);
	BS2ASM_LitIndexSym(ctx, s1);
	BS2ASM_LitIndexSym(ctx, s2);
	i=BS2ASM_LitIndexClassRef(ctx, s3);

	tmp=BS2ASM_NewTagEntity(ctx, "Catch");
	BS2ASM_SetEntityValue(tmp, "begin", s0);
	BS2ASM_SetEntityValue(tmp, "end", s1);
	BS2ASM_SetEntityValue(tmp, "target", s2);
	BS2ASM_SetEntityValue(tmp, "class", s3);
	tmp->cref=ctx->lit[i];
	
	return(tmp);
}

BS2ASM_TagEntity *BS2ASM_ConvertClass(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *n0, *n1, *n2, *n3;
	BS2ASM_TagEntity *tmp, *cur, *ts;
	BS2ASM_List *flst, *mlst;
	char *s0, *s1, *s2;
	int i;

	ts=BS2ASM_EnterScope(ctx);

	tmp=BS2ASM_NewTagEntity(ctx, "ClassInfo");

	if(!strcmp(ent->tagtype, "interface"))
		ent->flagstr=BS2ASM_GetFlagsWithFlag(ent->flagstr, "Ci");
	if(!strcmp(ent->tagtype, "struct"))
		ent->flagstr=BS2ASM_GetFlagsWithFlag(ent->flagstr, "Cj");
	if(!strcmp(ent->tagtype, "union"))
		ent->flagstr=BS2ASM_GetFlagsWithFlag(ent->flagstr, "Ck");

	BS2ASM_LitIndexClassRef(ctx, ent->name);
	BS2ASM_LitIndexClassRef(ctx, ent->extname);
	tmp->flagstr=ent->flagstr;
	tmp->scope=ctx->curscope;
	tmp->name=ent->name;
	tmp->extname=ent->extname;
	
	flst=BS2ASM_NewList(ctx, NULL);
	mlst=BS2ASM_NewList(ctx, NULL);
	
	cur=ent->first;
	while(cur)
	{
		if(!strcmp(cur->tagtype, "field") ||
			!strcmp(cur->tagtype, "var"))
		{
			n0=BS2ASM_ConvertTagEnt(ctx, cur);
			if(n0)
			{
				BS2ASM_LitIndexTagEnt(ctx, n0);
				BS2ASM_AddListEntry(flst, n0);
			}
		}

		if(!strcmp(cur->tagtype, "method") ||
			!strcmp(cur->tagtype, "function") ||
			!strcmp(cur->tagtype, "proto") ||
			!strcmp(cur->tagtype, "s_proto"))
		{
			n0=BS2ASM_ConvertTagEnt(ctx, cur);
			if(n0)
			{
				BS2ASM_LitIndexTagEnt(ctx, n0);
				BS2ASM_AddListEntry(mlst, n0);
			}
		}

		cur=cur->next;
	}

//	BS2ASM_SetEntityValueEnt(tmp, "fieldlist", flst);
//	BS2ASM_SetEntityValueEnt(tmp, "methodlist", mlst);

	if(!BS2ASM_ListEmptyP(flst))
		tmp->fieldlist=flst;
	if(!BS2ASM_ListEmptyP(mlst))
		tmp->methodlist=mlst;

	n0=BS2ASM_FindEntityChildTag(ent, "implements");
	if(n0)n0=BS2ASM_ConvertList(ctx, n0);
//	BS2ASM_SetEntityValueEnt(tmp, "ifacelist", n0);
	tmp->ifacelist=n0;

	BS2ASM_SetScopeObject(ctx, tmp);

	BS2ASM_ExitScope(ctx, ts);
	
	return(tmp);
}

BS2ASM_TagEntity *BS2ASM_ConvertPackage(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *n0, *n1, *n2, *n3;
	BS2ASM_TagEntity *tmp, *cur, *ts;
	BS2ASM_List *flst, *mlst, *clst;
	char *s0, *s1, *s2;
	int i;

	ts=BS2ASM_EnterScope(ctx);

	tmp=BS2ASM_NewTagEntity(ctx, "PackageInfo");
	
//	BS2ASM_LitIndexClassRef(ctx, ent->name);
//	BS2ASM_LitIndexClassRef(ctx, ent->extname);
	tmp->flagstr=ent->flagstr;
	tmp->name=ent->name;
	
	flst=BS2ASM_NewList(ctx, NULL);
	mlst=BS2ASM_NewList(ctx, NULL);
	clst=BS2ASM_NewList(ctx, NULL);
	
	cur=ent->first;
	while(cur)
	{
		if(!strcmp(cur->tagtype, "field") ||
			!strcmp(cur->tagtype, "var") ||
			!strcmp(cur->tagtype, "s_var"))
		{
			n0=BS2ASM_ConvertTagEnt(ctx, cur);
			if(n0)
			{
				BS2ASM_LitIndexTagEnt(ctx, n0);
				BS2ASM_AddListEntry(flst, n0);
			}
		}

		if(!strcmp(cur->tagtype, "method") ||
			!strcmp(cur->tagtype, "function") ||
			!strcmp(cur->tagtype, "proto") ||
			!strcmp(cur->tagtype, "s_proto"))
		{
			n0=BS2ASM_ConvertTagEnt(ctx, cur);
			if(n0)
			{
				BS2ASM_LitIndexTagEnt(ctx, n0);
				BS2ASM_AddListEntry(mlst, n0);
			}
		}

		if(!strcmp(cur->tagtype, "class") ||
			!strcmp(cur->tagtype, "interface") ||
			!strcmp(cur->tagtype, "struct") ||
			!strcmp(cur->tagtype, "union"))
		{
			n0=BS2ASM_ConvertTagEnt(ctx, cur);
			if(n0)
			{
				BS2ASM_LitIndexTagEnt(ctx, n0);
				BS2ASM_AddListEntry(clst, n0);
			}
		}

		cur=cur->next;
	}

//	BS2ASM_SetEntityValueEnt(tmp, "fieldlist", flst);
//	BS2ASM_SetEntityValueEnt(tmp, "methodlist", mlst);
//	BS2ASM_SetEntityValueEnt(tmp, "classlist", clst);

	if(!BS2ASM_ListEmptyP(flst))
		tmp->fieldlist=flst;
	if(!BS2ASM_ListEmptyP(mlst))
		tmp->methodlist=mlst;
	if(!BS2ASM_ListEmptyP(clst))
		tmp->classlist=clst;

//	n0=BS2ASM_FindEntityChildTag(ent, "implements");
//	if(n0)n0=BS2ASM_ConvertList(ctx, n0);
//	BS2ASM_SetEntityValueEnt(tmp, "ifacelist", n0);

	BS2ASM_SetScopeObject(ctx, tmp);

	BS2ASM_ExitScope(ctx, ts);

	return(tmp);
}

BS2ASM_TagEntity *BS2ASM_ConvertImport(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *n0, *n1, *n2, *n3;
	BS2ASM_TagEntity *tmp;
	char *s0, *s1, *s2;
	int i;

	BS2ASM_LitIndexSym(ctx, ent->name);
//	BS2ASM_LitIndexUTF8(ctx, ent->type);
	BS2ASM_LitIndexUTF8(ctx, ent->flagstr);
	
	tmp=BS2ASM_NewTagEntity(ctx, "NamedRef");
	tmp->scope=ctx->curscope;
	tmp->name=ent->name;
//	tmp->type=ent->type;
	tmp->flagstr=ent->flagstr;
	
	if(ctx->curscope)
	{
		if(!ctx->curscope->importlist)
			ctx->curscope->importlist=BS2ASM_NewList(ctx, NULL);
		BS2ASM_AddListEntry(ctx->curscope->importlist, tmp);
	}

	return(tmp);
}

BS2ASM_TagEntity *BS2ASM_ConvertTop(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *tmp, *ts;
	char *s0, *s1, *s2;
	int i;

	ts=BS2ASM_EnterScope(ctx);

//	tmp=BS2ASM_ConvertList(ctx, ent);
	tmp=BS2ASM_ConvertPackage(ctx, ent);

	BS2ASM_SetScopeObject(ctx, tmp);
	BS2ASM_ExitScope(ctx, ts);
	return(tmp);
}

BS2ASM_TagEntity *BS2ASM_ConvertTagEnt(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent)
{
	int i;

	if(!ent)return(NULL);

	if(!strcmp(ent->tagtype, "UTF8"))return(NULL);
	if(!strcmp(ent->tagtype, "Symbol"))return(NULL);
	if(!strcmp(ent->tagtype, "String"))return(NULL);
	if(!strcmp(ent->tagtype, "Blob"))return(NULL);
	if(!strcmp(ent->tagtype, "List"))return(NULL);
	if(!strcmp(ent->tagtype, "Label"))return(NULL);
	if(!strcmp(ent->tagtype, "Block"))return(NULL);
	if(!strcmp(ent->tagtype, "ClassRef"))return(NULL);
	if(!strcmp(ent->tagtype, "ClassInfo"))return(NULL);
	if(!strcmp(ent->tagtype, "FieldInfo"))return(NULL);
	if(!strcmp(ent->tagtype, "NameType"))return(NULL);
	if(!strcmp(ent->tagtype, "MethodInfo"))return(NULL);
	if(!strcmp(ent->tagtype, "PackageInfo"))return(NULL);
	if(!strcmp(ent->tagtype, "ScopeInfo"))return(NULL);

	if(!strcmp(ent->tagtype, "top"))
	{
		return(BS2ASM_ConvertTop(ctx, ent));
//		return(NULL);
	}

	if(!strcmp(ent->tagtype, "import"))
		return(BS2ASM_ConvertImport(ctx, ent));

	if(!strcmp(ent->tagtype, "var"))
		return(BS2ASM_ConvertVar(ctx, ent));
	if(!strcmp(ent->tagtype, "field"))
		return(BS2ASM_ConvertVar(ctx, ent));
	if(!strcmp(ent->tagtype, "s_var"))
		return(BS2ASM_ConvertVar(ctx, ent));
	if(!strcmp(ent->tagtype, "rest"))
		return(BS2ASM_ConvertVar(ctx, ent));

	if(!strcmp(ent->tagtype, "method"))
		return(BS2ASM_ConvertMethod(ctx, ent));
	if(!strcmp(ent->tagtype, "proto"))
		return(BS2ASM_ConvertMethod(ctx, ent));
	if(!strcmp(ent->tagtype, "function"))
		return(BS2ASM_ConvertMethod(ctx, ent));
	if(!strcmp(ent->tagtype, "s_proto"))
		return(BS2ASM_ConvertMethod(ctx, ent));

	if(!strcmp(ent->tagtype, "catch"))
		return(BS2ASM_ConvertCatch(ctx, ent));
	if(!strcmp(ent->tagtype, "class"))
		return(BS2ASM_ConvertClass(ctx, ent));
	if(!strcmp(ent->tagtype, "interface"))
		return(BS2ASM_ConvertClass(ctx, ent));
	if(!strcmp(ent->tagtype, "struct"))
		return(BS2ASM_ConvertClass(ctx, ent));
	if(!strcmp(ent->tagtype, "union"))
		return(BS2ASM_ConvertClass(ctx, ent));

	if(!strcmp(ent->tagtype, "package"))
		return(BS2ASM_ConvertPackage(ctx, ent));

	if(!strcmp(ent->tagtype, "classref"))
	{
		i=BS2ASM_LitIndexClassRef2(ctx, ctx->curscope, ent->name);
		return(ctx->lit[i]);
	}

	if(!strcmp(ent->tagtype, "list"))
		return(BS2ASM_ConvertList(ctx, ent));

	if(!strcmp(ent->tagtype, "int"))
	{
		i=BS2ASM_LitIndexInt32(ctx, bs2asm_atoi(ent->value));
		return(ctx->lit[i]);
	}
	if(!strcmp(ent->tagtype, "long"))
	{
		i=BS2ASM_LitIndexInt64(ctx, bs2asm_atoi(ent->value));
		return(ctx->lit[i]);
	}
	if(!strcmp(ent->tagtype, "float"))
	{
		i=BS2ASM_LitIndexFloat32(ctx, atof(ent->value));
		return(ctx->lit[i]);
	}
	if(!strcmp(ent->tagtype, "double"))
	{
		i=BS2ASM_LitIndexFloat64(ctx, atof(ent->value));
		return(ctx->lit[i]);
	}

	if(!strcmp(ent->tagtype, "string"))
		{ i=BS2ASM_LitIndexString(ctx, ent->value); return(ctx->lit[i]); }
	if(!strcmp(ent->tagtype, "symbol"))
		{ i=BS2ASM_LitIndexSym(ctx, ent->value); return(ctx->lit[i]); }

	printf("Convert: unhandled %s\n", ent->tagtype);

	return(NULL);
}

void BS2ASM_FlattenTagEnt(BS2ASM_Context *ctx, BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *n0;
	BS2ASM_List *lst;
	int i;

	n0=BS2ASM_ConvertTagEnt(ctx, ent);
	if(n0)
	{
		BS2ASM_LitIndexTagEnt(ctx, n0);
		BS2ASM_FlattenTagEnt(ctx, n0);
		return;
	}

	if(!ent)return;

//	if(!strcmp(ent->tagtype, "method"))
//	{
//		BS2ASM_FlattenMethod(ctx, ent);
//		return;
//	}

	BS2ASM_LitIndexUTF8(ctx, ent->tagtype);

	if(!ent->lump)
		ent->lump=BS2ASM_AllocLump(ctx);
	BS2ASM_ResetLump(ent->lump);

	if(!strcmp(ent->tagtype, "String"))
		{ BS2ASM_EmitLumpUTF8(ent->lump, ent->name); return; }

	if(!strcmp(ent->tagtype, "Symbol"))
		{ BS2ASM_EmitLumpUTF8(ent->lump, ent->name); return; }

	if(!strcmp(ent->tagtype, "UTF8"))
	{
		BS2ASM_EmitLumpBytes(ent->lump, ent->name, strlen(ent->name));
		return;
	}

	if(!strcmp(ent->tagtype, "Blob"))
	{
		BS2ASM_EmitLumpBytes(ent->lump, ent->body, ent->szbody);
		return;
	}

	if(!strcmp(ent->tagtype, "Int32"))
		{ BS2ASM_EmitLumpInt32(ent->lump, ent->val.intval); return; }
	if(!strcmp(ent->tagtype, "Int64"))
		{ BS2ASM_EmitLumpInt64(ent->lump, ent->val.intval); return; }
	if(!strcmp(ent->tagtype, "Float32"))
		{ BS2ASM_EmitLumpFloat32(ent->lump, ent->val.realval); return; }
	if(!strcmp(ent->tagtype, "Float64"))
		{ BS2ASM_EmitLumpFloat64(ent->lump, ent->val.realval); return; }

	if(!strcmp(ent->tagtype, "Label"))
	{
		BS2ASM_EmitLumpSym(ent->lump, ent->name);
		BS2ASM_EmitLumpVLI(ent->lump, ent->offs);
		return;
	}

	if(!strcmp(ent->tagtype, "MethodInfo"))
	{
		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);
		BS2ASM_EmitLumpNameType(ent->lump, ent->name, ent->type);
		BS2ASM_EmitLumpVLI(ent->lump, ent->validx);
		BS2ASM_EmitLumpEntity(ent->lump, ent->attrlist);

//		lst=BS2ASM_GetEntityValueEnt(ent, "attrlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);
		return;
	}

	if(!strcmp(ent->tagtype, "FieldInfo"))
	{
		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);
		BS2ASM_EmitLumpNameType(ent->lump, ent->name, ent->type);
		BS2ASM_EmitLumpVLI(ent->lump, ent->validx);
		BS2ASM_EmitLumpEntity(ent->lump, ent->attrlist);

//		lst=BS2ASM_GetEntityValueEnt(ent, "attrlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);
		return;
	}

	if(!strcmp(ent->tagtype, "Block"))
	{
		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);
		BS2ASM_EmitLumpVLI(ent->lump, ent->validx);
		BS2ASM_EmitLumpUTF8(ent->lump, ent->type);

//		n0=BS2ASM_GetEntityValueEnt(ent, "labels");
//		BS2ASM_EmitLumpEntity(ent->lump, n0);
//		n0=BS2ASM_GetEntityValueEnt(ent, "argslist");
//		BS2ASM_EmitLumpEntity(ent->lump, n0);
//		n0=BS2ASM_GetEntityValueEnt(ent, "varslist");
//		BS2ASM_EmitLumpEntity(ent->lump, n0);
//		n0=BS2ASM_GetEntityValueEnt(ent, "catchlist");
//		BS2ASM_EmitLumpEntity(ent->lump, n0);
//		n0=BS2ASM_GetEntityValueEnt(ent, "attrlist");
//		BS2ASM_EmitLumpEntity(ent->lump, n0);

		BS2ASM_EmitLumpEntity(ent->lump, ent->labelslist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->argslist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->varslist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->catchlist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->attrlist);

		return;
	}

	if(!strcmp(ent->tagtype, "ClassInfo"))
	{
		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
//		BS2ASM_EmitLumpNameType(ent->lump, ent->name, ent->type);

		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);
		BS2ASM_EmitLumpClassRef(ent->lump, ent->name);
		BS2ASM_EmitLumpClassRef(ent->lump, ent->extname);

//		BS2ASM_EmitLumpVLI(ent->lump, ent->validx);

//		lst=BS2ASM_GetEntityValueEnt(ent, "importlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);

//		lst=BS2ASM_GetEntityValueEnt(ent, "attrlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);

		BS2ASM_EmitLumpEntity(ent->lump, ent->ifacelist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->fieldlist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->methodlist);

//		BS2ASM_EmitLumpEntity(ent->lump, ent->classlist);
//		BS2ASM_EmitLumpEntity(ent->lump, ent->varslist);
//		BS2ASM_EmitLumpEntity(ent->lump, ent->methodlist);

//		BS2ASM_EmitLumpEntity(ent->lump, ent->importlist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->attrlist);

		return;
	}

	if(!strcmp(ent->tagtype, "PackageInfo"))
	{
		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
//		BS2ASM_EmitLumpNameType(ent->lump, ent->name, ent->type);

//		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);

//		BS2ASM_EmitLumpVLI(ent->lump, ent->validx);
		BS2ASM_EmitLumpClassRef(ent->lump, ent->name);

//		lst=BS2ASM_GetEntityValueEnt(ent, "importlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);

//		lst=BS2ASM_GetEntityValueEnt(ent, "attrlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);

		BS2ASM_EmitLumpEntity(ent->lump, ent->classlist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->fieldlist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->methodlist);

//		BS2ASM_EmitLumpEntity(ent->lump, ent->importlist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->attrlist);

		return;
	}

	if(!strcmp(ent->tagtype, "ScopeInfo"))
	{
//		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
//		BS2ASM_EmitLumpNameType(ent->lump, ent->name, ent->type);

		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);

		BS2ASM_EmitLumpVLI(ent->lump, ent->validx);

//		lst=BS2ASM_GetEntityValueEnt(ent, "importlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);

//		lst=BS2ASM_GetEntityValueEnt(ent, "attrlist");
//		BS2ASM_EmitLumpEntity(ent->lump, lst);

		BS2ASM_EmitLumpEntity(ent->lump, ent->importlist);
		BS2ASM_EmitLumpEntity(ent->lump, ent->attrlist);
		return;
	}

	if(!strcmp(ent->tagtype, "List"))
	{
		BS2ASM_EmitLumpVLI(ent->lump, ent->nlbl);
		for(i=0; i<ent->nlbl; i++)
			BS2ASM_EmitLumpEntity(ent->lump, ent->lbl[i]);
		return;
	}

	if(!strcmp(ent->tagtype, "NameType"))
	{
		BS2ASM_EmitLumpSym(ent->lump, ent->name);
		BS2ASM_EmitLumpUTF8(ent->lump, ent->type);
		return;
	}

	if(!strcmp(ent->tagtype, "ClassRef"))
	{
		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);
		BS2ASM_EmitLumpSym(ent->lump, ent->name);
		return;
	}

	if(!strcmp(ent->tagtype, "FieldRef"))
	{
		BS2ASM_EmitLumpEntity(ent->lump, ent->cref);
		BS2ASM_EmitLumpNameType(ent->lump, ent->name, ent->type);
		return;
	}
	if(!strcmp(ent->tagtype, "MethodRef"))
	{
		BS2ASM_EmitLumpEntity(ent->lump, ent->cref);
		BS2ASM_EmitLumpNameType(ent->lump, ent->name, ent->type);
		return;
	}
	if(!strcmp(ent->tagtype, "NamedRef"))
	{
		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);
		BS2ASM_EmitLumpSym(ent->lump, ent->name);
		BS2ASM_EmitLumpEntity(ent->lump, ent->attrlist);
		return;
	}

	if(!strcmp(ent->tagtype, "NamedAttribute"))
	{
//		BS2ASM_EmitLumpUTF8(ent->lump, ent->flagstr);
		BS2ASM_EmitLumpEntity(ent->lump, ent->scope);
		BS2ASM_EmitLumpSym(ent->lump, ent->name);
		BS2ASM_EmitLumpEntity(ent->lump, ent->argslist);
		return;
	}

	printf("Flatten: unhandled %s\n", ent->tagtype);
}

byte *BS2ASM_FlattenEmitTagEntLump(BS2ASM_Context *ctx,
	BS2ASM_TagEntity *ent, byte *ip)
{
	int i, j;

	if(!ent)
	{
		i=BS2ASM_LitIndexUTF8(ctx, "NullEntry");
		ip=BS2ASM_EncVLI(ip, i);
		ip=BS2ASM_EncVLI(ip, 0);
		return(ip);
	}

	ent->lump->sz_data=
		ent->lump->dp - ent->lump->data;

	i=BS2ASM_LitIndexUTF8(ctx, ent->tagtype);
	j=ent->lump->sz_data;

	ip=BS2ASM_EncVLI(ip, i);
	ip=BS2ASM_EncVLI(ip, j);
	memcpy(ip, ent->lump->data, j);
	ip+=j;
	
	return(ip);
}

byte *BS2ASM_FlattenEmitCommentLump(BS2ASM_Context *ctx,
	char *str, byte *ip)
{
	int i;
	
	i=strlen(str);
	ip=BS2ASM_EncVLI(ip, 0);
	ip=BS2ASM_EncVLI(ip, i);
	memcpy(ip, str, i);
	ip+=i;
	return(ip);
}

int BS2ASM_FlattenBuffer(BS2ASM_Context *ctx, byte *buf, int lim)
{
	BS2ASM_TagEntity *ent;
	byte *ip;
	int i, j, n;

	BS2ASM_FlattenTagEnt(ctx, ctx->top);
	for(i=1; i<ctx->nlit; i++)
	{
		BS2ASM_FlattenTagEnt(ctx, ctx->lit[i]);
	}

	n=0;
	while(n!=ctx->nlit)
	{
		n=ctx->nlit;
		for(i=1; i<n; i++)
		{
			BS2ASM_FlattenTagEnt(ctx, ctx->lit[i]);
		}
	}
	
	ip=buf;
	ip=BS2ASM_FlattenEmitCommentLump(ctx, "BS2BC0", ip);
	for(i=1; i<ctx->nlit; i++)
	{
		ip=BS2ASM_FlattenEmitTagEntLump(ctx, ctx->lit[i], ip);
	}
	ip=BS2ASM_EncVLI(ip, 0);
	ip=BS2ASM_EncVLI(ip, 0);
	
	return(ip-buf);
}
