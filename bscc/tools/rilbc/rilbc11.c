/*
Idea:
Simply pack structs into a memory buffer.
Use a big array to point to structs.

so:
<header>
<type-pointers>
<object-pointers>
<data-region>
 */

struct RBC1_Object_s {
int type;		//type of object
int size;		//size of object
int next;		//next object (same type)
};

struct RBC1_ObjectType_s {
RBC1_Object head;
int name;		//type name
int sig;		//type layout sig (string)
int field;		//field list (string)
//management info
int first;		//first object of given type
};

struct RBC1_ObjectString_s {
RBC1_Object head;
char data[1];
};

struct RBC1_ObjectList_s {
RBC1_Object head;
int data[1];
};

struct RBC1_Head_s {
int magic;
int offs_obj;
int n_obj;
int offs_data;
int sz_data;
};

struct RBC1_Context_s {
//object offsets
int n_obj, m_obj;
int *obj;

//data region
int n_data, m_data;
byte *data;

int t_typetype;
int t_string;
};

void *rbc_malloc(int sz)
{
	void *p;
	p=malloc(sz);
	memset(p, 0, sz);
	return(p);
}

RBC1_Context *RBC1_AllocContext()
{
	RBC1_Context *ctx;
	
	ctx=rbc_malloc(sizeof(RBC1_Context));
	ctx->obj=rbc_malloc(256*sizeof(int));
	ctx->n_obj=1; ctx->m_obj=256;

	ctx->data=rbc_malloc(256);
	ctx->n_data=4; ctx->m_data=256;
	
	RBC1_AllocTypeType(ctx);

	RBC1_DeclareType(ctx, "Opcode", "ArArAr", "name;args;attr;");
	RBC1_DeclareType(ctx, "KeyVal", "ArAr", "key;value;");
	RBC1_DeclareType(ctx, "List", "A0Ar", "data;");

	return(ctx);
}

int RBC1_FlattenContext(RBC1_Context *ctx, byte *buf)
{
	RBC1_Head *head;
	int i, j, k;
	
	i=(sizeof(RBC1_Head)+3)&(~3);
	j=i+ctx->n_obj*sizeof(int);
	k=j+ctx->n_data;
	
	if(!buf)return(k);
	
	head=(RBC1_Head *)buf;
	head->magic=0x31434252;
	head->offs_obj=i;
	head->n_obj=ctx->n_obj;
	head->offs_data=j;
	head->sz_data=ctx->n_data;
	
	memcpy(buf+head->offs_obj, ctx->obj, ctx->n_obj*sizeof(int));
	memcpy(buf+head->offs_data, ctx->data, ctx->n_data);
	
	return(k);
}

int RBC1_GetFreeIndex(RBC1_Context *ctx)
{
	int i, j, k;

	for(i=1; i<ctx->n_obj; i++)
		if(!ctx->obj[i])
			return(i);

	i=ctx->n_obj++;
	if(i>=ctx->m_obj)
	{
		j=ctx->m_obj; k=j+(j>>1);
		ctx->obj=realloc(ctx->obj, k*sizeof(int));
		memset(ctx->obj+j, 0, (k-j)*sizeof(int));
		ctx->m_obj=k;
	}
	
	return(i);
}

int RBC1_AllocData(RBC1_Context *ctx, int sz)
{
	int i;

	ctx->n_data=(ctx->n_data+3)&(~3);
	if((ctx->n_data+sz)>=ctx->m_data)
	{
		i=ctx->n_data+sz; j=ctx->m_data;
		while(i>=j)j=j+(j>>1);
		ctx->data=realloc(ctx->data, j);
		memset(ctx->data+ctx->n_data, 0, j-ctx->m_data);
		ctx->m_data=j;
	}
	
	i=ctx->n_data;
	ctx->n_data+=sz;
	return(i);
}

int RBC1_AllocObject(RBC1_Context *ctx, int type, int sz)
{
	RBC1_Object *obj;
	RBC1_ObjectType *ty;
	int idx, offs;
	
	idx=RBC1_GetFreeIndex(ctx);
	offs=RBC1_AllocData(ctx, sz);
	
	obj=RBC1_GetObject(ctx, idx);
	obj->type=type;
	obj->size=sz;
	
	ty=RBC1_GetTypeObject(ctx, type);
	obj->next=ty->first;
	ty->first=idx;

	return(idx);
}

int RBC1_AllocTypeType(RBC1_Context *ctx)
{
	RBC1_Object *obj, *obj1;
	RBC1_ObjectType *ty, *ty1;
	int idx, idx1, offs, sz;
	
	idx=RBC1_GetFreeIndex(ctx);
	offs=RBC1_AllocData(ctx, sz);
	sz=sizeof(RBC1_ObjectType);
	
	obj=RBC1_GetObject(ctx, idx);
	obj->type=idx; obj->size=sz;
	ty=RBC1_GetTypeObject(ctx, idx);
	obj->next=ty->first; ty->first=idx;
	ctx->t_typetype=idx;

	idx1=RBC1_AllocObject(ctx, ctx->t_typetype, sz);
	ty1=RBC1_GetTypeObject(ctx, idx1);
	ctx->t_string=idx1;

	ty->name=RBC1_IndexString(ctx, "Type");
	ty->sig=RBC1_IndexString(ctx, "ArArArAr");
	ty->field=RBC1_IndexString(ctx, "name;sig;field;first;");

	ty1->name=RBC1_IndexString(ctx, "String");
	ty1->sig=RBC1_IndexString(ctx, "A0c");
	ty1->field=RBC1_IndexString(ctx, "data;");

	return(idx);
}

RBC1_Object *RBC1_GetObject(RBC1_Context *ctx, int idx)
{
	byte *p;

	if(!idx)return(NULL);
	if(!ctx->obj[idx])return(NULL);
	p=ctx->data+ctx->obj[idx];
	return((RBC1_Object *)p);
}

RBC1_ObjectType *RBC1_GetTypeObject(RBC1_Context *ctx, int idx)
	{ return((RBC1_ObjectType *)RBC1_GetObject(ctx, idx)); }

RBC1_ObjectString *RBC1_GetStringObject(RBC1_Context *ctx, int idx)
	{ return((RBC1_ObjectString *)RBC1_GetObject(ctx, idx)); }
RBC1_ObjectList *RBC1_GetListObject(RBC1_Context *ctx, int idx)
	{ return((RBC1_ObjectList *)RBC1_GetObject(ctx, idx)); }

int RBC1_GetObjectType(RBC1_Context *ctx, int idx)
	{ return(RBC1_GetObject(ctx, idx)->type); }
int RBC1_GetObjectNext(RBC1_Context *ctx, int idx)
	{ return(RBC1_GetObject(ctx, idx)->next); }
void *RBC1_GetObjectData(RBC1_Context *ctx, int idx)
	{ return((void *)(RBC1_GetStringObject(ctx, idx)->data)); }

char *RBC1_GetObjectTypeName(RBC1_Context *ctx, int idx)
{
	RBC1_ObjectType *ty;
	int i;
	
	i=ctx->obj[idx]->type;
	ty=RBC1_GetTypeObject(ctx, i);
	return(RBC1_GetString(ctx, ty->name));
}

char *RBC1_GetString(RBC1_Context *ctx, int idx)
{
	RBC1_ObjectString *str;
	str=RBC1_GetStringObject(ctx, i);
	return(str->data);
}

int RBC1_LookupType(RBC1_Context *ctx, char *name)
{
	RBC1_ObjectType *ty;
	char *s;
	int i;

	ty=RBC1_GetTypeObject(ctx, ctx->t_typetype);
	i=ty->first;
	
	while(i>0)
	{
		ty=RBC1_GetTypeObject(ctx, i);
		s=RBC1_GetString(ctx, ty->name);
		if(!strcmp(s, name))
			return(i);
		i=RBC1_GetObjectNext(ctx, i);
	}
	return(-1);
}

int RBC1_IndexType(RBC1_Context *ctx, char *name)
{
	RBC1_ObjectType *ty;
	int i;

	i=RBC1_LookupType(ctx, name);
	if(i>0)return(i);
	
	i=RBC1_AllocObject(ctx, ctx->t_typetype, sizeof(RBC1_ObjectType));
	ty=RBC1_GetTypeObject(ctx, i);
	ty->name=RBC1_IndexString(ctx, name);

	return(i);
}

int RBC1_LookupString(RBC1_Context *ctx, char *name)
{
	RBC1_ObjectType *ty;
	char *s;
	int i;

	ty=RBC1_GetTypeObject(ctx, ctx->t_string);
	i=ty->first;
	
	while(i>0)
	{
		s=RBC1_GetString(ctx, i);
		if(!strcmp(s, name))
			return(i);
		i=RBC1_GetObjectNext(ctx, i);
	}
	return(-1);
}

int RBC1_IndexString(RBC1_Context *ctx, char *name)
{
	RBC1_ObjectString *str;
	int i, sz;
	
	i=RBC1_LookupString(ctx, name);
	if(i>0)return(i);

	sz=sizeof(RBC1_ObjectString)+strlen(name);
	i=RBC1_AllocObject(ctx, ctx->t_string, sz);
	str=RBC1_GetStringObject(ctx, i);
	strcpy(str->data, name);
	
	return(i);
}


int RBC1_GetTypeFirst(RBC1_Context *ctx, char *name)
{
	RBC1_ObjectType *ty;
	int i;

	i=RBC1_LookupType(ctx, name);
	if(i<=0)return(i);
	ty=RBC1_GetTypeObject(ctx, i);
	return(ty->first);
}

int RBC1_DeclareType(RBC1_Context *ctx,
	char *name, char *sig, char *field)
{
	RBC1_ObjectType *ty;
	int i, j, k;

	i=RBC1_IndexType(ctx, name);
	if(i<=0)return(i);

	j=RBC1_IndexString(ctx, sig);
	k=RBC1_IndexString(ctx, field);
	ty=RBC1_GetTypeObject(ctx, i);
	ty->sig=j; ty->field=k;

	return(i);
}

void *RBC1_AllocRaw(RBC1_Context *ctx, char *type, int sz)
{
	RBC1_ObjectString *str;
	int i, j, sz1;

	i=RBC1_IndexType(ctx, name);
	if(i<=0)return(i);

	sz1=sizeof(RBC1_ObjectString)+sz;
	j=RBC1_AllocObject(ctx, i, sz1);
	str=RBC1_GetStringObject(ctx, j);
	return(str->data);
}

int RBC1_LookupList(RBC1_Context *ctx, int *lst, int n)
{
	RBC1_ObjectList *l;
	char *s;
	int i, j;

	i=RBC1_GetTypeFirst(ctx, "List");	
	while(i>0)
	{
		l=RBC1_GetListObject(ctx, i);
		j=(l->head.size-sizeof(RBC1_Object))/sizeof(int);
		if(j!=(n+1))
			{ i=RBC1_GetObjectNext(ctx, i); continue; }

		for(j=0; j<n; j++)
			if(l->data[j]!=lst[j])
				break;
		if((j>=n) && !l->data[j])
			return(i);
		i=RBC1_GetObjectNext(ctx, i);
	}
	return(-1);
}

int RBC1_AllocList(RBC1_Context *ctx, int n)
{
	int i, j, sz;

	i=RBC1_IndexType(ctx, "List");
	sz=sizeof(RBC1_ObjectList)+n*sizeof(int);
	j=RBC1_AllocObject(ctx, i, sz);
	return(j);
}

int RBC1_IndexList(RBC1_Context *ctx, int *lst, int n)
{
	RBC1_ObjectList *l;
	int i, j;

	i=RBC1_LookupList(ctx, lst, n);
	if(i>0)return(i);
	
	i=RBC1_AllocList(ctx, n);
	l=RBC1_GetListObject(ctx, i);
	for(j=0; j<n; j++)
		l->data[j]=lst[j];
	l->data[j]=0;
	return(i);
}
