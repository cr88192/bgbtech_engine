#include <bs2asm.h>

#define FOFS(type, field)	(((char *)&(((type *)NULL)->field))-((char *)NULL))

BS2ASM_EntityFieldInfo bs2asm_ent_field[]={
{"name",	"Pc",	FOFS(BS2ASM_TagEntity, name), 0},
{"type",	"Pc",	FOFS(BS2ASM_TagEntity, type), 0},
{"extends",	"Pc",	FOFS(BS2ASM_TagEntity, extname), 0},
{"flags",	"Pc",	FOFS(BS2ASM_TagEntity, flagstr), 0},
{"value",	"Pc",	FOFS(BS2ASM_TagEntity, value), 0},
{"offset",	"i",	FOFS(BS2ASM_TagEntity, offs), 0},

#if 0
{"importlist",	"Pv",	FOFS(BS2ASM_TagEntity, importlist), 0},
{"classlist",	"Pv",	FOFS(BS2ASM_TagEntity, classlist), 0},
{"fieldlist",	"Pv",	FOFS(BS2ASM_TagEntity, fieldlist), 0},
{"methodlist",	"Pv",	FOFS(BS2ASM_TagEntity, methodlist), 0},
{"ifacelist",	"Pv",	FOFS(BS2ASM_TagEntity, ifacelist), 0},
{"labelslist",	"Pv",	FOFS(BS2ASM_TagEntity, labelslist), 0},
{"varslist",	"Pv",	FOFS(BS2ASM_TagEntity, varslist), 0},
{"argslist",	"Pv",	FOFS(BS2ASM_TagEntity, argslist), 0},
{"catchlist",	"Pv",	FOFS(BS2ASM_TagEntity, catchlist), 0},
{"attrlist",	"Pv",	FOFS(BS2ASM_TagEntity, attrlist), 0},
#endif

{NULL, NULL, 0, 0}
};

void *bs2asm_tmalloc(char *type, int sz)
{
	return(gctalloc(type, sz));
}

void *bs2asm_ctalloc(BS2ASM_Context *ctx, char *type, int sz)
{
	return(gctalloc(type, sz));
}

void *bs2asm_cmalloc(BS2ASM_Context *ctx, int sz)
{
	return(gcmalloc(sz));
}

void *bs2asm_crealloc(BS2ASM_Context *ctx, void *ptr, int sz)
{
	return(gcrealloc(ptr, sz));
}

void *bs2asm_realloc(void *ptr, int sz)
{
	return(gcrealloc(ptr, sz));
}

void *bs2asm_strdup(char *str)
{
	return(dyllStrdup(str));
}

void *bs2asm_rstrdup(char *str)
{
	return(gcrstrdup(str));
}

BS2ASM_TagEntity *BS2ASM_NewTagEntity(BS2ASM_Context *ctx, char *tag)
{
	BS2ASM_TagEntity *tmp;
	
	tmp=bs2asm_ctalloc(ctx, "_bs2asm_tagent_t", sizeof(BS2ASM_TagEntity));
	tmp->tagtype=bs2asm_strdup(tag);
	tmp->ctx=ctx;

	return(tmp);
}

int BS2ASM_IndexEntityField(BS2ASM_TagEntity *ent, char *key)
{
	int i;

	for(i=0; bs2asm_ent_field[i].name; i++)
		if(!strcmp(bs2asm_ent_field[i].name, key))
			return(i);
	return(-1);
}

BS2ASM_TagEntityAttr *BS2ASM_LookupEntityAttr(BS2ASM_TagEntity *ent, char *key)
{
	BS2ASM_TagEntityAttr *cur;
	char *ks;
	
	ks=bs2asm_strdup(key);
	
	cur=ent->attr;
	while(cur)
	{
		if(cur->key==ks)
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BS2ASM_TagEntityAttr *BS2ASM_GetEntityAttr(BS2ASM_TagEntity *ent, char *key)
{
	BS2ASM_TagEntityAttr *cur, *lst;
	char *ks;
	
	ks=bs2asm_strdup(key);
	
	cur=ent->attr; lst=NULL;
	while(cur)
	{
		if(cur->key==ks)
			return(cur);
		lst=cur; cur=cur->next;
	}
	
	cur=bs2asm_ctalloc(ent->ctx, "_bs2asm_tagentattr_t",
		sizeof(BS2ASM_TagEntityAttr));
	cur->key=ks;

	if(!lst)	{ ent->attr=cur; }
	else		{ lst->next=cur; }
	
	return(cur);
}

void BS2ASM_SetEntityValue(BS2ASM_TagEntity *ent, char *key, char *value)
{
	BS2ASM_TagEntityAttr *attr;
	byte *pb;
	int i;
	
//	i=-1;
	i=BS2ASM_IndexEntityField(ent, key);
	if(i>=0)
	{
//		printf("%s@%d:%s=%s\n", key,
//			bs2asm_ent_field[i].offs,
//			bs2asm_ent_field[i].type,
//			value);
	
		pb=((byte *)ent)+bs2asm_ent_field[i].offs;
		if(!strcmp(bs2asm_ent_field[i].type, "Pc"))
			{ *(char **)pb=bs2asm_strdup(value); return; }
		if(!strcmp(bs2asm_ent_field[i].type, "i"))
			{ *(int *)pb=atoi(value); return; }
		if(!strcmp(bs2asm_ent_field[i].type, "x"))
			{ *(s64 *)pb=atoi(value); return; }
		if(!strcmp(bs2asm_ent_field[i].type, "f"))
			{ *(float *)pb=atof(value); return; }
		if(!strcmp(bs2asm_ent_field[i].type, "d"))
			{ *(double *)pb=atof(value); return; }
		return;
	}
	
	attr=BS2ASM_GetEntityAttr(ent, key);
	attr->value=bs2asm_strdup(value);
}

char *BS2ASM_GetEntityValue(BS2ASM_TagEntity *ent, char *key)
{
	char tb[256];
	BS2ASM_TagEntityAttr *attr;
	byte *pb;
	int i;

	i=BS2ASM_IndexEntityField(ent, key);
	if(i>=0)
	{
//		printf("%s@%d:%s=%s\n", key,
//			bs2asm_ent_field[i].offs,
//			bs2asm_ent_field[i].type,
//			value);
	
		pb=((byte *)ent)+bs2asm_ent_field[i].offs;
		if(!strcmp(bs2asm_ent_field[i].type, "Pc"))
			{ return(*(char **)pb); }
		if(!strcmp(bs2asm_ent_field[i].type, "i"))
		{
			if(!(*(int *)pb))return(NULL);
			sprintf(tb, "%d", *(int *)pb);
			return(bs2asm_strdup(tb));
		}

		if(!strcmp(bs2asm_ent_field[i].type, "x"))
		{
			if(!(*(s64 *)pb))return(NULL);
			sprintf(tb, "%d", (int)(*(s64 *)pb));
			return(bs2asm_strdup(tb));
		}
		if(!strcmp(bs2asm_ent_field[i].type, "f"))
		{
			if((*(float *)pb)==0.0)return(NULL);
			sprintf(tb, "%f", *(float *)pb);
			return(bs2asm_strdup(tb));
		}
		if(!strcmp(bs2asm_ent_field[i].type, "d"))
		{
			if((*(double *)pb)==0.0)return(NULL);
			sprintf(tb, "%f", *(double *)pb);
			return(bs2asm_strdup(tb));
		}
		return(NULL);
	}

	attr=BS2ASM_LookupEntityAttr(ent, key);
	if(attr)return(attr->value);
	return(NULL);
}

void BS2ASM_SetEntityValueEnt(BS2ASM_TagEntity *ent, char *key,
	BS2ASM_TagEntity *entval)
{
	BS2ASM_TagEntityAttr *attr;
	byte *pb;
	int i;
	
	i=BS2ASM_IndexEntityField(ent, key);
	if(i>=0)
	{
		pb=((byte *)ent)+bs2asm_ent_field[i].offs;
		if(!strncmp(bs2asm_ent_field[i].type, "PX", 2))
			{ *(BS2ASM_TagEntity **)pb=entval; return; }
		if(!strncmp(bs2asm_ent_field[i].type, "Pv", 2))
			{ *(BS2ASM_TagEntity **)pb=entval; return; }
		return;
	}
	
	attr=BS2ASM_GetEntityAttr(ent, key);
	attr->entval=entval;
}

BS2ASM_TagEntity *BS2ASM_GetEntityValueEnt(BS2ASM_TagEntity *ent, char *key)
{
	BS2ASM_TagEntityAttr *attr;
	byte *pb;
	int i;

	i=BS2ASM_IndexEntityField(ent, key);
	if(i>=0)
	{
		pb=((byte *)ent)+bs2asm_ent_field[i].offs;
		if(!strncmp(bs2asm_ent_field[i].type, "PX", 2))
			{ return(*(BS2ASM_TagEntity **)pb); }
		if(!strncmp(bs2asm_ent_field[i].type, "Pv", 2))
			{ return(*(BS2ASM_TagEntity **)pb); }
		return(NULL);
	}

	attr=BS2ASM_LookupEntityAttr(ent, key);
	if(attr)return(attr->entval);
	return(NULL);
}


void BS2ASM_AddEntityChild(
	BS2ASM_TagEntity *parent, BS2ASM_TagEntity *child)
{
	BS2ASM_TagEntity *cur;

	if(!parent->first)
	{
		parent->first=child;
		return;
	}

	cur=parent->first;
	while(cur->next)cur=cur->next;
	cur->next=child;
}

BS2ASM_TagEntity *BS2ASM_FindEntityTag(
	BS2ASM_TagEntity *lst, char *tag)
{
	BS2ASM_TagEntity *cur;
	
	cur=lst;
	while(cur)
	{
		if(!strcmp(cur->tagtype, tag))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

BS2ASM_TagEntity *BS2ASM_FindEntityChildTag(
	BS2ASM_TagEntity *ent, char *tag)
{
	return(BS2ASM_FindEntityTag(ent->first, tag));
}

BS2ASM_List *BS2ASM_NewList(BS2ASM_Context *ctx, char *type)
{
	BS2ASM_TagEntity *tmp;
	
	tmp=BS2ASM_NewTagEntity(ctx, "List");
	tmp->type=bs2asm_strdup(type);
	tmp->ctx=ctx;

	return(tmp);
}

int BS2ASM_ListEmptyP(BS2ASM_List *list)
{
	return(list->nlbl==0);
}

int BS2ASM_AddListEntry(
	BS2ASM_List *list, BS2ASM_TagEntity *ent)
{
	int i;

	for(i=0; i<list->nlbl; i++)
		if(list->lbl[i]==ent)
			return(i);

	BS2ASM_LitIndexTagEnt(list->ctx, ent);
	
	BS2ASM_CheckExpandBlockLabel(list);
	i=list->nlbl++;
	list->lbl[i]=ent;

	return(i);
}

BS2ASM_List *BS2ASM_CopyList(BS2ASM_Context *ctx, BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntity *tmp;
	int i;
	
	tmp=BS2ASM_NewTagEntity(ctx, "List");
//	tmp->type=bs2asm_strdup(type);
	tmp->ctx=ctx;

	tmp->nlbl=ent->nlbl;
	BS2ASM_CheckExpandBlockLabel(tmp);

	for(i=0; i<ent->nlbl; i++)
		tmp->lbl[i]=ent->lbl[i];

	return(tmp);
}


void BS2ASM_DumpEntityTree(BS2ASM_TagEntity *ent)
{
	BS2ASM_TagEntityAttr *acur;
	BS2ASM_TagEntity *ecur;
	char *s0, *s1;
	int i;

	printf("%s {\n", ent->tagtype);
	
	for(i=0; bs2asm_ent_field[i].name; i++)
	{
		s0=bs2asm_ent_field[i].name;
		s1=BS2ASM_GetEntityValue(ent, s0);
		if(!s1)continue;
		printf("%s=%s ", s0, s1);
	}

	acur=ent->attr;
	while(acur)
	{
		s0=acur->key;
		s1=acur->value;
		if(!s1)continue;
		printf("%s=%s ", s0, s1);
		acur=acur->next;
	}

	if(ent->ip > ent->body)
		printf("body<%d> ", ent->ip - ent->body);

	printf("\n");

	ecur=ent->first;
	while(ecur)
	{
		BS2ASM_DumpEntityTree(ecur);
		ecur=ecur->next;
	}

	printf("}\n");
}
