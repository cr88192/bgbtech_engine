#include <lbxgl2.h>

LBXGL_Entity *LBXGL_Entity_New()
{
	LBXGL_Entity *tmp;

	tmp=ObjType_New("lbxgl_entity_t", sizeof(LBXGL_Entity));
	return(tmp);
}

LBXGL_EntProperty *LBXGL_Entity_LookupProperty(LBXGL_Entity *ent, char *name)
{
	LBXGL_EntProperty *cur, *last;

	last=NULL;
	cur=ent->attr;
	while(cur)
	{
		if(!strcmp(cur->name, name))
		{
			if(last)
			{
				//mru move
				last->next=cur->next;
				cur->next=ent->attr;
				ent->attr=cur;
			}
			return(cur);
		}
		last=cur;
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat lbxgl;Entity
Form
	void *LBXGL_Entity_GetProperty(LBXGL_Entity *ent, char *name);
Description
	Gets a property associated with an entity.
	Returns NULL if the property is not present.
--*/
void *LBXGL_Entity_GetProperty(LBXGL_Entity *ent, char *name)
{
	LBXGL_EntProperty *tmp;
	double *fa;
	int i;

	if(!strcmp(name, "origin"))
	{
		fa=ObjType_New("bggl2_numarray_t", 3*sizeof(double));
		for(i=0; i<3; i++)fa[i]=ent->org[i];
		return(fa);
	}
	if(!strcmp(name, "rotation"))
	{
		fa=ObjType_New("bggl2_numarray_t", 9*sizeof(double));
		for(i=0; i<9; i++)fa[i]=ent->rot[i];
		return(fa);
	}

	tmp=LBXGL_Entity_LookupProperty(ent, name);

	if(tmp)return(tmp->value);
	return(NULL);
}

/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_SetProperty(LBXGL_Entity *ent, char *name, \
	void *value);
Description
	Sets a property for an entity.
	Value is a value in the 'ObjType' system.
--*/
int LBXGL_Entity_SetProperty(LBXGL_Entity *ent, char *name, void *value)
{
	LBXGL_EntProperty *tmp;
	double *fa;

	if(!strcmp(name, "origin"))
	{
		fa=value;
		VecND2F_Copy(fa, ent->org, 3);
		return(0);
	}
	if(!strcmp(name, "rotation"))
	{
		fa=value;
		VecND2F_Copy(fa, ent->rot, 9);
		return(0);
	}

	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)
	{
		tmp->value=value;
		return(0);
	}

	tmp=ObjType_New("lbxgl_entproperty_t", sizeof(LBXGL_EntProperty));
	tmp->name=kstrdup(name);
	tmp->value=value;

	tmp->next=ent->attr;
	ent->attr=tmp;
	return(0);
}

/*--
Cat lbxgl;Entity
Form
	char *LBXGL_Entity_GetPropertyString(LBXGL_Entity *ent, char *name);
	double LBXGL_Entity_GetPropertyFloat(LBXGL_Entity *ent, char *name);
	int LBXGL_Entity_GetPropertyBool(LBXGL_Entity *ent, char *name);
	int LBXGL_Entity_GetPropertyFVector(LBXGL_Entity *ent, char *name, \
	float *vec, int num);
	int LBXGL_Entity_GetPropertyDVector(LBXGL_Entity *ent, char *name, \
	double *vec, int num);
Description
	Specialized get functions for entities.
--*/
char *LBXGL_Entity_GetPropertyString(LBXGL_Entity *ent, char *name)
{
	void *p;
	p=LBXGL_Entity_GetProperty(ent, name);
	return(p);
}

double LBXGL_Entity_GetPropertyFloat(LBXGL_Entity *ent, char *name)
{
	void *p;

	if(!strcmp(name, "gamma"))return(ent->gamma);

	p=LBXGL_Entity_GetProperty(ent, name);
	if(!p)return(0);
	return(NetVal_UnwrapFloat(p));
}

int LBXGL_Entity_GetPropertyBool(LBXGL_Entity *ent, char *name)
{
	void *p;
	p=LBXGL_Entity_GetProperty(ent, name);
	if(!p)return(0);
	return(NetVal_UnwrapBool(p));
}

int LBXGL_Entity_GetPropertyFVector(LBXGL_Entity *ent, char *name,
	float *vec, int num)
{
	double *fa;
	int i;

	if(!strcmp(name, "origin"))
	{
		Vec3F_Copy(ent->org, vec);
		return(0);
	}
	if(!strcmp(name, "rotation"))
	{
		VecNF_Copy(ent->rot, vec, num);
		return(0);
	}

	fa=LBXGL_Entity_GetProperty(ent, name);
	if(!fa)
	{
		VecNF_Zero(vec, num);
		return(-1);
	}

	for(i=0; i<num; i++)vec[i]=fa[i];
	return(0);
}

int LBXGL_Entity_GetPropertyDVector(LBXGL_Entity *ent, char *name,
	double *vec, int num)
{
	double *fa;
	int i;

	if(!strcmp(name, "origin"))
	{
		VecNF2D_Copy(ent->org, vec, 3);
		return(0);
	}
	if(!strcmp(name, "rotation"))
	{
		VecNF2D_Copy(ent->rot, vec, num);
		return(0);
	}

	fa=LBXGL_Entity_GetProperty(ent, name);
	if(!fa)
	{
		VecND_Zero(vec, num);
		return(-1);
	}

	for(i=0; i<num; i++)vec[i]=fa[i];
	return(0);
}

/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_SetPropertyString(LBXGL_Entity *ent, char *name, \
	char *value);
	int LBXGL_Entity_SetPropertyFloat(LBXGL_Entity *ent, char *name, \
	double value);
	int LBXGL_Entity_SetPropertyBool(LBXGL_Entity *ent, char *name, int value);
	int LBXGL_Entity_SetPropertyFVector(LBXGL_Entity *ent, char *name, \
	float *value, int num);
	int LBXGL_Entity_SetPropertyDVector(LBXGL_Entity *ent, char *name, \
	double *value, int num);
Description
	Specialized set functions for entities.
--*/

int LBXGL_Entity_SetPropertyString(LBXGL_Entity *ent, char *name, char *value)
{
	return(LBXGL_Entity_SetProperty(ent, name, kdstrdup(value)));
}

int LBXGL_Entity_SetPropertyFloat(LBXGL_Entity *ent, char *name, double value)
{
	return(LBXGL_Entity_SetProperty(ent, name, NetVal_WrapFloat(value)));
}

int LBXGL_Entity_SetPropertyBool(LBXGL_Entity *ent, char *name, int value)
{
	return(LBXGL_Entity_SetProperty(ent, name, NetVal_WrapBool(value)));
}

int LBXGL_Entity_SetPropertyFVector(LBXGL_Entity *ent, char *name,
	float *value, int num)
{
	double *fa;
	int i;

	if(!strcmp(name, "origin"))
	{
		Vec3F_Copy(value, ent->org);
		return(0);
	}
	if(!strcmp(name, "rotation"))
	{
		VecNF_Copy(value, ent->rot, num);
		return(0);
	}

	fa=ObjType_New("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(LBXGL_Entity_SetProperty(ent, name, fa));
}

int LBXGL_Entity_SetPropertyDVector(LBXGL_Entity *ent, char *name,
	double *value, int num)
{
	double *fa;
	int i;

	if(!strcmp(name, "origin"))
	{
		VecND2F_Copy(value, ent->org, 3);
		return(0);
	}
	if(!strcmp(name, "rotation"))
	{
		VecND2F_Copy(value, ent->rot, num);
		return(0);
	}

	fa=ObjType_New("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(LBXGL_Entity_SetProperty(ent, name, fa));
}

/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_SendMessage(LBXGL_Entity *ent, char *name, \
	void **msgdata);
Description
	Sends a given message to an entity.
	Msgdata is an array of message specific data items (pdlib type \
	system, NULL terminated).
	Returns 0 if the message was successfully handled, -1 otherwise.

	The order goes: Entity Named callbacks, Global Named callbacks, \
	Entity Unnamed Callbacks, Global Unnamed callbacks.
--*/
int LBXGL_Entity_SendMessage(LBXGL_Entity *ent, char *name, void **msgdata)
{
	int i;

	return(-1);
}

/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_SendMessageAll(LBXGL_Entity *ents, char *name, \
	void **msgdata);
Description
	Sends a message to all entities in a list.
	This is intended to be used for, eg, init messages or others related
	the general world state/some occurance.
--*/
int LBXGL_Entity_SendMessageAll(LBXGL_Entity *ents, char *name, void **msgdata)
{
	LBXGL_Entity *cur;

	cur=ents;
	while(cur)
	{
		LBXGL_Entity_SendMessage(cur, name, msgdata);
		cur=cur->next;
	}
	return(0);
}

/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_Render(LBXGL_Entity *ent);
Description
	Render an entity.
--*/
int LBXGL_Entity_Render(LBXGL_Entity *ent)
{
	LBXGL_Entity *cur;

	void *p, *q;
	char *ty, *ty2;
	double org[3], ang[3], vec[3];
	double *frp, *skp;
	float pt[3], ra;
	float corgf[3], cfwf[3];
	char *s;
	int fr, sk;

	LBXGL_GetCamOriginF(corgf);
	LBXGL_GetCamVectorsF(cfwf, NULL, NULL);

	s=LBXGL_Entity_GetProperty(ent, "flare");
	if(s)
	{
		LBXGL_Entity_GetPropertyDVector(ent, "origin", org, 3);
		ra=LBXGL_Entity_GetPropertyFloat(ent, "flare_radius");
		if(!ra)ra=100;
		VecND2F_Copy(org, pt, 3);
		LBXGL_Sprite_RenderFlareName(s, pt, ra);
	}

	p=LBXGL_Entity_GetProperty(ent, "mdl");
	if(!p)return(0);

	ty=ObjType_GetTypeName(p);

	LBXGL_Entity_GetPropertyDVector(ent, "origin", org, 3);
	LBXGL_Entity_GetPropertyDVector(ent, "angles", ang, 3);

	if(!strcmp(ty, "lbxgl_bspmodel_t"))
	{
	}

	if(!strcmp(ty, "lbxgl_quakemdl_t"))
	{
		fr=0;
		sk=0;
		q=LBXGL_Entity_GetProperty(ent, "frame");
		if(ObjType_TypeP(q, "float_t"))
			fr=NetVal_UnwrapFloat(q);
		if(ObjType_TypeP(q, "string_t"))
			fr=LBXGL_QuakeMDL_IndexFrame(p, q);

		q=LBXGL_Entity_GetProperty(ent, "skin");
		if(ObjType_TypeP(q, "float_t"))
			sk=NetVal_UnwrapFloat(q);
		if(ObjType_TypeP(q, "string_t"))
			sk=LBXGL_Texture_LoadImage(q)+256;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslated(org[0], org[1], org[2]);
		glRotatef(ang[2], 0, 0, 1);
		glRotatef(ang[0], 1, 0, 0);
		glRotatef(ang[1], 0, 1, 0);

		LBXGL_QuakeMDL_Render(p, fr, sk);

		glPopMatrix();
	}

	if(!strcmp(ty, "lbxgl_skelmesh_t"))
	{
//		LBXGL_Physics2_SetupEntity(ent, 0);
//		LBXGL_PhysSkel_SetupEntity(ent);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslated(org[0], org[1], org[2]);
		glRotatef(ang[2], 0, 0, 1);
		glRotatef(ang[0], 1, 0, 0);
		glRotatef(ang[1], 0, 1, 0);

		VecND2F_Copy(org, pt, 3);
//		ra=LBXGL_Light_PointVal(pt);
//		LBXGL_FmtSmd_RenderState(ent->skel);

		LBXGL_FmtSmd_Render(p, -1, -1);

		glPopMatrix();
	}

	return(0);
}

/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_RenderEnts(LBXGL_Entity *lst);
Description
	Render a list of entities.
--*/
int LBXGL_Entity_RenderEnts(LBXGL_Entity *lst)
{
	LBXGL_Entity *cur;

//	return(0);

	cur=lst;
	while(cur)
	{
		LBXGL_Entity_Render(cur);
		cur=cur->next;
	}
	return(0);
}



/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_Init();
Description
	Init function for the entity system.
--*/
int LBXGL_Entity_Init()
{
	LBXGL_PDSCREntity_Init();
	return(0);
}
