struct RBC1_Object_s {
int type;	//type of object
int next;	//next object (same type)
};

struct RBC1_ObjectType_s {
RBC1_Object head;
int name;
int sig;
int field;
int first;	//first object of given type
};

struct RBC1_ObjectString_s {
RBC1_Object head;
char data[1];
};

struct RBC1_Context_s {
int n_obj, m_obj;
RBC1_Object **obj;

int t_typetype;
};

int RBC1_GetObjectType(RBC1_Context *ctx, int idx)
	{ return(ctx->obj[idx]->type); }
int RBC1_GetObjectNext(RBC1_Context *ctx, int idx)
	{ return(ctx->obj[idx]->next); }

char *RBC1_GetObjectTypeName(RBC1_Context *ctx, int idx)
{
	RBC1_ObjectType *ty;
	int i;
	
	i=ctx->obj[idx]->type;
	ty=(RBC1_ObjectType *)ctx->obj[i];
	return(RBC1_GetString(ctx, ty->name));
}

char *RBC1_GetString(RBC1_Context *ctx, int idx)
{
	RBC1_ObjectString *str;
	str=(RBC1_ObjectString *)ctx->obj[idx];
	return(str->data);
}

int RBC1_LookupType(RBC1_Context *ctx, char *name)
{
	RBC1_ObjectType *ty;
	char *s;
	int i;

	ty=(RBC1_ObjectType *)ctx->obj[ctx->t_typetype];
	i=ty->first;
	
//	for(i=0; i<ctx->n_obj; i++)
	while(i>0)
	{
//		s=RBC1_GetObjectTypeName(ctx, i);
//		if(strcmp(s, "TypeType"))
//			continue;

		ty=(RBC1_ObjectType *)ctx->obj[i];
		s=RBC1_GetString(ctx, ty->name);
		if(!strcmp(s, name))
			return(i);
		i=RBC1_GetObjectNext(ctx, i);
	}
	return(-1);
}

int RBC1_LookupString(RBC1_Context *ctx, char *name)
{
	char *s;
	int i;
	
	for(i=0; i<ctx->n_obj; i++)
	{
		s=RBC1_GetObjectTypeName(ctx, i);
		if(strcmp(s, "String"))
			continue;
		s=RBC1_GetString(ctx, i);
		if(!strcmp(s, name))
			return(i);
	}
	return(-1);
}

int RBC1_IndexString(RBC1_Context *ctx, char *name)
{
	int i;
	
	i=RBC1_LookupString(ctx, name);
	if(i>=0)return(i);
}
