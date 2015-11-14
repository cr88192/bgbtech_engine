#include <lbxgl2.h>

extern PDSCR0_Object *pdscr_object_toplevel;

void *LBXGL_PDSCREntity_LoadIndex(PDSCR0_Context *ctx, void *obj, void *index)
{
	LBXGL_Entity *ent;
	LBXGL_EntClassInfo *inf;
	void *p;
	char *ty, *s;

	if(!strcmp(index, "world"))
	{
		ent=obj;
		return(ent->world->ents);
	}

	p=LBXGL_Entity_GetProperty(obj, index);
	if(!p)
	{
		inf=NULL;

		s=LBXGL_Entity_GetPropertyString(obj, "classname");
		if(s)inf=LBXGL_EntClass_GetClass(s);
		if(inf && inf->toplevel)
			p=PDSCR_Object_GetSlot(inf->toplevel, index);
	}

	if(!p)p=PDSCR_Object_GetSlot(pdscr_object_toplevel, index);
	return(p);
}

int LBXGL_PDSCREntity_StoreIndex(PDSCR0_Context *ctx, void *obj,
	void *index, void *value)
{
	void *p;
	char *ty;

	LBXGL_Entity_SetProperty(obj, index, value);
	return(0);
}

void *LBXGL_PDSCREntity_MethodCall(PDSCR0_Context *ctx, void *obj, void *index)
{
	LBXGL_EntClassInfo *inf;
	void **a, **b;
	void *p;
	char *s;
	int i, n;

	p=LBXGL_Entity_GetProperty(obj, index);
	if(p)
	{
		PDSCR0_Interp_Call(ctx, obj, p);
		p=PDSCR_Interp_Pop(ctx);
		return(p);
	}

	a=PDSCR_Interp_PopMark(ctx, &n);
	i=LBXGL_Entity_SendMessage(obj, index, a);

	if(i==-1)
	{
		inf=NULL;
		p=NULL;

		s=LBXGL_Entity_GetPropertyString(obj, "classname");
		if(s)inf=LBXGL_EntClass_GetClass(s);
		if(inf && inf->toplevel)
			p=PDSCR_Object_GetSlot(inf->toplevel, index);

		if(p)
		{
			PDSCR_Interp_PushMark(ctx);
			PDSCR_Interp_PushMulti(ctx, a, n);
			PDSCR0_Interp_Call(ctx, obj, p);

			p=PDSCR_Interp_Pop(ctx);
			return(p);
		}
	}

	return(NULL);
}

int lbxgl_pdscrentity_ghandler(LBXGL_Entity *ent, void *data, void **msgdata)
{
	LBXGL_EntClassInfo *inf;
	PDSCR0_Context *ctx;
	void *p;
	char *s;
	int n;

	p=LBXGL_Entity_GetProperty(ent, data);
	if(p)
	{
		p=PDSCR_Interp_CallFunction(ent, p, msgdata);
		return(0);
	}

	if(1)
	{
		inf=NULL;
		p=NULL;

		s=LBXGL_Entity_GetPropertyString(ent, "classname");
		if(s)inf=LBXGL_EntClass_GetClass(s);
		if(inf && inf->toplevel)
			p=PDSCR_Object_GetSlot(inf->toplevel, data);

		if(p)
		{
			p=PDSCR_Interp_CallFunction(ent, p, msgdata);
			return(0);
		}
	}

	return(-1);
}

void *lbxgl_pdscrentity_setmodel(PDSCR0_Context *ctx, void **args, int n)
{
//	LBXGL_QMDL *mdl;
	char *s, *t, *ty;
	void *p;

	if(!args[1])
	{
		LBXGL_Entity_SetProperty(args[0], "mdl", NULL);
		LBXGL_Entity_SetProperty(args[0], "skin", args[2]);

		return(NULL);
	}

	if(ObjType_TypeP(args[1], "string_t"))
	{
		s=args[1];
		t=s+strlen(s);
		while((t>s) && (*t!='.'))t--;
		if(*t=='.')t++;

		ty=NULL;
		if(!kstricmp(t, "mdl"))ty="QuakeMDL";
		if(!kstricmp(t, "md2"))ty="QuakeMDL";
		if(!kstricmp(t, "md3"))ty="QuakeMDL";

		if(!kstricmp(t, "map"))ty="QuakeMap";
		if(!kstricmp(t, "ski"))ty="SkelModelInfo";

		if(!strcmp(ty, "QuakeMDL"))
			p=LBXGL_QuakeMDL_LoadModel(s);
		if(!strcmp(ty, "SkelModelInfo"))
			p=LBXGL_FmtSmd_LoadModelInfo(s);

		LBXGL_Entity_SetProperty(args[0], "mdl", p);
	}
	LBXGL_Entity_SetProperty(args[0], "skin", args[2]);
	
	return(NULL);
}

void *lbxgl_pdscrentity_loadmodel(PDSCR0_Context *ctx, void **args, int n)
{
	char *s, *t, *ty;
	void *p;

	s=args[0];

	ty=NULL;
	if(n>1)ty=args[1];
	if(!ty)
	{
		t=s+strlen(s);
		while((t>s) && (*t!='.'))t--;
		if(*t=='.')t++;

		if(!kstricmp(t, "mdl"))ty="QuakeMDL";
		if(!kstricmp(t, "md2"))ty="QuakeMDL";
		if(!kstricmp(t, "md3"))ty="QuakeMDL";

		if(!kstricmp(t, "map"))ty="QuakeMap";
		if(!kstricmp(t, "ski"))ty="SkelModelInfo";
	}

	if(!ty)return(NULL);

	p=NULL;
	if(!strcmp(ty, "QuakeMDL"))
		p=LBXGL_QuakeMDL_LoadModel(s);

//	if(!strcmp(ty, "QuakeMap"))
//		p=LBXGL_QuakeMD2_LoadModel(s);

	if(!strcmp(ty, "SkelModelInfo"))
		p=LBXGL_FmtSmd_LoadModelInfo(s);

	return(p);
}

void *lbxgl_pdscrentity_setanim(PDSCR0_Context *ctx, void **args, int n)
{
	LBXGL_Sequence_SetEntitySequence(args[0], args[1], args[2]);
	return(NULL);
}

void *lbxgl_pdscrentity_domove(PDSCR0_Context *ctx, void **args, int n)
{
	void *p;

	return(p);
}

void *lbxgl_pdscrentity_getradius(PDSCR0_Context *ctx, void **args, int n)
{
	double f;
	f=LBXGL_Entity_GetRadius(args[0]);
	return(NetVal_WrapFloat(f));
}

int LBXGL_PDSCREntity_Init()
{
	PDSCR0_TypeContext *ctx;

	ctx=PDSCR_TypeCtx_GetType("lbxgl_entity_t");
	ctx->loadindex=&LBXGL_PDSCREntity_LoadIndex;
	ctx->storeindex=&LBXGL_PDSCREntity_StoreIndex;
	ctx->methodcall=&LBXGL_PDSCREntity_MethodCall;

	PDSCR_Builtin_AddBuiltin("setmodel",
		"(entity, model, skin), sets the model for "
		"a given entity",
		&lbxgl_pdscrentity_setmodel, 3);

	PDSCR_Builtin_AddBuiltin("loadmodel",
		"(model[, type]), loads a model",
		&lbxgl_pdscrentity_loadmodel, -2);

	PDSCR_Builtin_AddBuiltin("setanim",
		"(entity, sequence, frame), sets the animation sequence for "
		"a given entity",
		&lbxgl_pdscrentity_setanim, 3);

	PDSCR_Builtin_AddBuiltin("DoMove",
		"(entity, dir, dist), moves a given entity a certain "
		"distance in a certain dir, with object collision detection",
		&lbxgl_pdscrentity_domove, 3);

	PDSCR_Builtin_AddBuiltin("GetRadius",
		"(entity), gets an entity's radius",
		&lbxgl_pdscrentity_getradius, 1);

	return(0);
}
