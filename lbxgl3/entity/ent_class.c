/*
A system for managing entity classes.
*/

#include <lbxgl2.h>

LBXGL_EntClassInfo *lbxgl_entclass_root;
PDSCR0_Object *lbxgl_entclass_toplevel;

extern PDSCR0_Object *pdscr_object_toplevel;
extern LBXGL_Entity *lbxgl_entity_worldroot;


LBXGL_EntClassInfo *LBXGL_EntClass_Lookup(char *name)
{
	LBXGL_EntClassInfo *cur;

	cur=lbxgl_entclass_root;

	while(cur)
	{
		if(!strcmp(cur->classname, name))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_EntClassInfo *LBXGL_EntClass_GetClass(char *name)
{
	LBXGL_EntClassInfo *tmp;
	LBXGL_Entity *etmp, *ecur;
	VFILE *fd;
	char *s, *t;

	LBXGL_EntClass_Init();

	tmp=LBXGL_EntClass_Lookup(name);
	if(tmp)return(tmp);

	tmp=ObjType_New("lbxgl_entclassinfo_t", sizeof(LBXGL_EntClassInfo));
	tmp->classname=kstrdup(name);
	tmp->next=lbxgl_entclass_root;
	lbxgl_entclass_root=tmp;


	s=ksprint("entity/%s/classinfo.txt", name);
	fd=vffopen(s, "rb");
	if(!fd)
	{
		kprint("LBXGL_EntClass_GetClass: Can't open '%s'\n", s);
		return(NULL);
	}
//	etmp=LBXGL_MAP_UnFlattenEnts(fd);
	etmp=NULL;

	kprint("LBXGL_EntClass_GetClass: Read '%s'\n", s);

	ecur=etmp;
	while(ecur)
	{
		s=LBXGL_Entity_GetPropertyString(ecur, "classname");
		if(s && !strcmp(s, "classinfo"))
			break;
		ecur=ecur->next;
	}

	if(!ecur)
	{
		kprint("LBXGL_EntClass_GetClass: '%s': No ClassInfo ent\n", s);
		return(NULL);
	}

	s=LBXGL_Entity_GetPropertyString(ecur, "classparent");
	if(s)tmp->parent=LBXGL_EntClass_GetClass(s);

	tmp->toplevel=PDSCR_Object_NewEmpty();

	if(tmp->parent && tmp->parent->toplevel)
		PDSCR_Object_BindSlot(tmp->toplevel, "_parent",
			tmp->parent->toplevel);
		else PDSCR_Object_BindSlot(tmp->toplevel, "_parent",
			lbxgl_entclass_toplevel);

	s=LBXGL_Entity_GetPropertyString(ecur, "scriptname");
	if(s)
	{
		t=LBXGL_EntClass_OpenName(tmp, s);
		PDSCR_Interp_Load(t, tmp->toplevel);
	}

	return(tmp);
}

char *LBXGL_EntClass_OpenName(LBXGL_EntClassInfo *inf, char *fname)
{
	LBXGL_EntClassInfo *icur;
	VFILE *fd;
	char *s;

	icur=inf;
	while(icur)
	{
		s=ksprint("entity/%s/%s", icur->classname, fname);
		fd=vffopen(s, "rb");
		if(fd)
		{
			vfclose(fd);
			return(s);
		}

		icur=icur->parent;
	}

	return(fname);
}

VFILE *LBXGL_EntClass_OpenFile(LBXGL_EntClassInfo *inf,
	char *fname, char *mode)
{
	LBXGL_EntClassInfo *icur;
	VFILE *fd;
	char *s;

	icur=inf;
	while(icur)
	{
		s=ksprint("entity/%s/%s", icur->classname, fname);
		fd=vffopen(s, mode);
		if(fd)return(fd);

		icur=icur->parent;
	}

	fd=vffopen(fname, mode);
	return(fd);
}

int LBXGL_EntClass_SpawnEnt(LBXGL_Entity *ent)
{
	LBXGL_EntClassInfo *inf, *icur;
	char *s;
	void *p;
	void *args[4];

	s=LBXGL_Entity_GetPropertyString(ent, "classname");
	if(!s)
	{
		kprint("LBXGL_EntClass_SpawnEnt: No classname for ent\n");
		return(-1);
	}

//	kprint("try spawn ent %s\n", s);

	inf=LBXGL_EntClass_GetClass(s);

	icur=inf;
	while(icur)
	{
		if(icur->toplevel)
		{
//			kprint("have class for ent %s\n", s);

			//note: inf toplevel allowing overrides
			p=PDSCR_Object_GetSlot(inf->toplevel,
				icur->classname);
			if(p)
			{
//				kprint("run spawn ent %s\n", s);

				args[0]=0;
				PDSCR_Interp_CallFunction(ent, p, args);

				return(0);
			}
		}
		icur=icur->parent;
	}

	p=PDSCR_Object_GetSlot(pdscr_object_toplevel, s);
	if(p)
	{
//		kprint("run spawn ent %s\n", s);

		args[0]=0;
		PDSCR_Interp_CallFunction(ent, p, args);

		return(0);
	}

//	kprint("failed spawn ent %s\n", s);
	return(-1);
}

void *lbxgl_entclass_load(PDSCR0_Context *ctx, void **args, int n)
{
	LBXGL_EntClassInfo *inf;

	void *p, *q;
	char *ty;
	char *s, *t;


	p=args[0];
	if(n>1)q=args[1];
		else q=PDSCR_Interp_GetSelf(ctx);

	ty=ObjType_GetTypeName(q);
	if(!strcmp(ty, "lbxgl_entity_t"))
	{
		s=LBXGL_Entity_GetPropertyString(q, "classname");
		inf=LBXGL_EntClass_GetClass(s);
		t=LBXGL_EntClass_OpenName(inf, s);

		p=t;
		q=inf->toplevel;
	}

	PDSCR_Interp_Load(p, q);

	return(NULL);
}

int LBXGL_EntClass_Init()
{
	static int init;
	void *p;

	if(init)return(0);
	init=1;

	lbxgl_entclass_toplevel=PDSCR_Object_NewEmpty();

	PDSCR_Object_BindSlot(lbxgl_entclass_toplevel, "_toplevel",
			pdscr_object_toplevel);

	p=PDSCR_Builtin_NewBuiltin("entclass.load",
		"(name[, toplevel]): load and run a script",
		&lbxgl_entclass_load, -2);
	PDSCR_Object_BindSlot(lbxgl_entclass_toplevel, "load", p);
}
