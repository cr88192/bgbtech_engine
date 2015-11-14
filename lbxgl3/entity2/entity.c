#include <lbxgl2.h>

LBXGL_EntCallback *lbxgl_entity_ghandlers=NULL;

LBXGL_Entity *LBXGL_Entity_New()
{
	LBXGL_Entity *tmp;

	LBXGL_Entity_Init();
	tmp=ObjType_New("lbxgl_entity_t", sizeof(LBXGL_Entity));
	return(tmp);
}

LBXGL_Entity *LBXGL_Entity_NewClass(char *cn)
{
	LBXGL_Entity *tmp;

	LBXGL_Entity_Init();
	tmp=ObjType_New("lbxgl_entity_t", sizeof(LBXGL_Entity));
	return(tmp);
}

LBXGL_Entity *LBXGL_Entity_NewWorld(LBXGL_World *wrl)
{
	LBXGL_Entity *cur, *tmp;

	tmp=LBXGL_Entity_New();
	if(!wrl->ents)
	{
		wrl->ents=tmp;
		return(tmp);
	}

	cur=wrl->ents;
	while(cur->next)cur=cur->next;
	cur->next=tmp;

	tmp->world=wrl;

	return(tmp);
}

void LBXGL_Entity_Unlink(LBXGL_Entity *ent)
{
	LBXGL_Entity *cur, *lst;
	
	cur=ent->world->ents; lst=NULL;
	while(cur && cur!=ent) { lst=cur; cur=cur->next; }

	if(cur)
	{
		if(lst)
		{
			lst->next=cur->next;
		}else
		{
			ent->world->ents=cur->next;
		}
	}
}

LBXGL_Light *LBXGL_Entity_NewDLightWorld(LBXGL_World *wrl)
{
	LBXGL_Light *dl;

	dl=ObjType_New("lbxgl_light_t", sizeof(LBXGL_Light));
	dl->next=wrl->dlights;
	wrl->dlights=dl;

	return(dl);
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
	float *fb;
	void *p;
	int i;

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
	float *fb;
	int i;

	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)
	{
		tmp->value=value;
		tmp->flag|=1;
		return(0);
	}

	tmp=ObjType_New("lbxgl_entproperty_t", sizeof(LBXGL_EntProperty));
	tmp->name=kstrdup(name);
	tmp->value=value;
	tmp->flag=1;

	tmp->next=ent->attr;
	ent->attr=tmp;
	return(0);
}

int LBXGL_Entity_GetPropertyFlag(LBXGL_Entity *ent, char *name)
{
	LBXGL_EntProperty *tmp;
	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)return(tmp->flag);
	return(0);
}

void LBXGL_Entity_SetPropertyFlag(LBXGL_Entity *ent, char *name, int flag)
{
	LBXGL_EntProperty *tmp;
	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)tmp->flag=flag;
}

int LBXGL_Entity_SetPropertyAsString(LBXGL_Entity *ent,
	char *name, char *value)
{
	double *fa;
	float *fb;
	char **as2;
	void *p;
	char *t;
	int i, n;

	t=value;
	while(*t && (*t<=' '))t++;
	if(((*t>='0') && (*t<='9')) || (*t=='-'))
	{
		as2=ksplit(value);
		for(n=0; as2[n]; n++);

		if(n>1)
		{
			fa=ObjType_New("bggl2_numarray_t", n*sizeof(double));
			for(i=0; i<n; i++)fa[i]=atof(as2[i]);
			p=fa;
		}else p=dyfloat(atof(as2[0]));
	}else p=dystring(value);

	i=LBXGL_Entity_SetProperty(ent, name, p);
	return(i);
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
	p=LBXGL_Entity_GetProperty(ent, name);
	if(!p)return(0);
	return(dyfloatv(p));
}

int LBXGL_Entity_GetPropertyBool(LBXGL_Entity *ent, char *name)
{
	void *p;
	p=LBXGL_Entity_GetProperty(ent, name);
	if(!p)return(0);
	return(dytruep(p));
}

int LBXGL_Entity_GetPropertyFVector(LBXGL_Entity *ent, char *name,
	float *vec, int num)
{
	double *fa;
	int i;

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
	LBXGL_EntProperty *tmp;
	double *fa;
	int i;

/*	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value; *fa=value;
		tmp->flag|=1;
		return(0);
	} */

	i=LBXGL_Entity_SetProperty(ent, name, dyfloat(value));
	return(i);
}

int LBXGL_Entity_SetPropertyBool(LBXGL_Entity *ent, char *name, int value)
{
	return(LBXGL_Entity_SetProperty(ent, name, dyint(value)));
}

int LBXGL_Entity_SetPropertyFVector(LBXGL_Entity *ent, char *name,
	float *value, int num)
{
	LBXGL_EntProperty *tmp;
	double *fa;
	int i;

	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag|=1;
		return(0);
	}

	fa=ObjType_New("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(LBXGL_Entity_SetProperty(ent, name, fa));
}

int LBXGL_Entity_SetPropertyDVector(LBXGL_Entity *ent, char *name,
	double *value, int num)
{
	LBXGL_EntProperty *tmp;
	double *fa;
	int i;

	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag|=1;
		return(0);
	}

	fa=ObjType_New("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(LBXGL_Entity_SetProperty(ent, name, fa));
}

int LBXGL_Entity_SetPropertyFVector2(LBXGL_Entity *ent, char *name,
	float *value, int num)
{
	LBXGL_EntProperty *tmp;
	double *fa;
	int i;

	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag&=~1;
		return(0);
	}

	fa=ObjType_New("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(LBXGL_Entity_SetProperty(ent, name, fa));
}

int LBXGL_Entity_SetPropertyDVector2(LBXGL_Entity *ent, char *name,
	double *value, int num)
{
	LBXGL_EntProperty *tmp;
	double *fa;
	int i;

	tmp=LBXGL_Entity_LookupProperty(ent, name);
	if(tmp)
	{
		fa=tmp->value;
		for(i=0; i<num; i++)fa[i]=value[i];
		tmp->flag&=~1;
		return(0);
	}

	fa=ObjType_New("bggl2_numarray_t", num*sizeof(double));
	for(i=0; i<num; i++)fa[i]=value[i];

	return(LBXGL_Entity_SetProperty(ent, name, fa));
}

LBXGL_EntCallback *LBXGL_Entity_GetCallback(LBXGL_Entity *ent, char *name)
{
	LBXGL_EntCallback *cur, *tmp;

	if(name)
	{
		cur=ent->cb;
		while(cur)
		{
			if(cur->name)if(!strcmp(cur->name, name))
				return(cur);
			cur=cur->next;
		}
	}else
	{
		cur=ent->cb;
		while(cur)
		{
			if(!cur->name)return(cur);
			cur=cur->next;
		}
	}

	tmp=ObjType_New("lbxgl_entcallback_t", sizeof(LBXGL_EntCallback));
	if(name)tmp->name=kstrdup(name);
	tmp->next=ent->cb;
	ent->cb=tmp;
	return(tmp);
}

void LBXGL_Entity_SetCallbackN(LBXGL_Entity *ent, char *name, int n,
	void *(*func)())
{
	LBXGL_EntCallback *cb;
	cb=LBXGL_Entity_GetCallback(ent, name);
	if(n==0)cb->f0=func;
	if(n==1)cb->f1=func;
	if(n==2)cb->f2=func;
	if(n==3)cb->f3=func;
	if(n==4)cb->f4=func;
	if(n==5)cb->f5=func;
	if(n==6)cb->f6=func;
	if(n==7)cb->f7=func;
	if(n==8)cb->f8=func;
}


int LBXGL_Entity_SetCallback(LBXGL_Entity *ent, char *name, void *data,
	void *(*func)(LBXGL_Entity *ent, void *data, int nargs, void **args))
{
	LBXGL_EntCallback *cur, *tmp;

	if(name)
	{
		cur=ent->cb;
		while(cur)
		{
			if(cur->name)if(!strcmp(cur->name, name))
			{
				cur->func=func;
				cur->data=data;
				return(0);
			}
			cur=cur->next;
		}
	}

	tmp=ObjType_New("lbxgl_entcallback_t", sizeof(LBXGL_EntCallback));
	if(name)tmp->name=kstrdup(name);
	tmp->func=func;
	tmp->data=data;

	tmp->next=ent->cb;
	ent->cb=tmp;
	return(0);
}

int LBXGL_Entity_SetGCallback(char *name, void *data,
	void *(*func)(LBXGL_Entity *ent, void *data, int nargs, void **args))
{
	LBXGL_EntCallback *cur, *tmp;

	if(name)
	{
		cur=lbxgl_entity_ghandlers;
		while(cur)
		{
			if(cur->name)if(!strcmp(cur->name, name))
			{
				cur->func=func;
				cur->data=data;
				return(0);
			}
			cur=cur->next;
		}
	}

	tmp=ObjType_New("lbxgl_entcallback_t", sizeof(LBXGL_EntCallback));
	if(name)tmp->name=kstrdup(name);
	tmp->func=func;
	tmp->data=data;

	tmp->next=lbxgl_entity_ghandlers;
	lbxgl_entity_ghandlers=tmp;
	return(0);
}

void *LBXGL_Entity_InvokeCallback(LBXGL_Entity *ent,
	LBXGL_EntCallback *cb, int nargs, void **args)
{
	if((nargs==0) && cb->f0)return(cb->f0(ent));
	if((nargs==1) && cb->f1)return(cb->f1(ent, args[0]));
	if((nargs==2) && cb->f2)return(cb->f2(ent, args[0], args[1]));
	if((nargs==3) && cb->f3)return(cb->f3(ent, args[0], args[1], args[2]));

	if((nargs==4) && cb->f4)return(cb->f4(ent, args[0], args[1], args[2],
		args[3]));
	if((nargs==5) && cb->f5)return(cb->f5(ent, args[0], args[1], args[2],
		args[3], args[4]));
	if((nargs==6) && cb->f6)return(cb->f6(ent, args[0], args[1], args[2],
		args[3], args[4], args[5]));
	if((nargs==7) && cb->f7)return(cb->f7(ent, args[0], args[1], args[2],
		args[3], args[4], args[5], args[6]));
	if((nargs==8) && cb->f8)return(cb->f8(ent, args[0], args[1], args[2],
		args[3], args[4], args[5], args[6], args[7]));

	if(cb->func)return(cb->func(ent, cb->data, nargs, args));
	return(UNDEFINED);
}

void *LBXGL_Entity_SendMessage(LBXGL_Entity *ent, char *name,
	int nargs, void **args)
{
	LBXGL_EntCallback *cur;
	void *p;
	int i;

	cur=ent->cb;
	while(cur)
	{
		if(cur->name)if(!strcmp(cur->name, name))
		{
			p=LBXGL_Entity_InvokeCallback(ent, cur, nargs, args);
			return(p);
		}
		cur=cur->next;
	}

	cur=lbxgl_entity_ghandlers;
	while(cur)
	{
		if(cur->name)if(!strcmp(cur->name, name))
		{
			p=LBXGL_Entity_InvokeCallback(ent, cur, nargs, args);
			return(p);
		}
		cur=cur->next;
	}

	cur=ent->cb;
	while(cur)
	{
		if(!cur->name)
		{
			p=LBXGL_Entity_InvokeCallback(ent, cur, nargs, args);
			if(p!=UNDEFINED)return(p);
		}
		cur=cur->next;
	}

	cur=lbxgl_entity_ghandlers;
	while(cur)
	{
		if(!cur->name)
		{
			p=LBXGL_Entity_InvokeCallback(ent, cur, nargs, args);
			if(p!=UNDEFINED)return(p);
		}
		cur=cur->next;
	}

	return(UNDEFINED);
}

int LBXGL_Entity_SendMessageAll(LBXGL_Entity *ents, char *name,
	int nargs, void **args)
{
	LBXGL_Entity *cur;

	cur=ents;
	while(cur)
	{
		LBXGL_Entity_SendMessage(cur, name, nargs, args);
		cur=cur->next;
	}
	return(0);
}

/*--
Cat lbxgl;Entity
Form
	int LBXGL_Entity_Render(LBXGL_World *wrl, LBXGL_Entity *ent);
Description
	Render an entity.
--*/
int LBXGL_Entity_Render(LBXGL_World *wrl, LBXGL_Entity *ent)
{
	LBXGL_Entity *cur;
	BSP_Model *bmdl;

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

	p=LBXGL_Entity_SendMessage(ent, "render", 0, NULL);
	if(p!=UNDEFINED)return(0);

	p=LBXGL_Entity_GetProperty(ent, "render");
	if(p)
	{
		((int (*)(LBXGL_Entity *ent))p)(ent);
		return(0);
	}

	p=LBXGL_Entity_GetProperty(ent, "mdl");
	if(!p)return(0);

	ty=dygettype(p);

	LBXGL_Entity_GetPropertyDVector(ent, "origin", org, 3);
	LBXGL_Entity_GetPropertyDVector(ent, "angles", ang, 3);

//	printf("Render %s @ ( %f %f %f )\n", ty, org[0], org[1], org[2]);


	if(!strcmp(ty, "bsp_model_t"))
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslated(org[0], org[1], org[2]);
		glRotatef(ang[2], 0, 0, 1);
		glRotatef(ang[0], 1, 0, 0);
		glRotatef(ang[1], 0, 1, 0);

		bmdl=p;
		if(bmdl!=wrl->mdls)
			BSP_DrawModel(wrl, bmdl);

		glPopMatrix();
	}

	if(!strcmp(ty, "lbxgl_meshgroup_t"))
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslated(org[0], org[1], org[2]);
		glRotatef(ang[2], 0, 0, 1);
		glRotatef(ang[0], 1, 0, 0);
		glRotatef(ang[1], 0, 1, 0);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_LIGHTING);
		glColor4f(1, 1, 1, 1);

		LBXGL_Mesh_DrawGroup(p);

		glPopMatrix();
	}

	if(!strcmp(ty, "lbxgl_skel2state_t"))
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();

		glTranslated(org[0], org[1], org[2]);
		glRotatef(ang[2], 0, 0, 1);
		glRotatef(ang[0], 1, 0, 0);
		glRotatef(ang[1], 0, 1, 0);

//		V3F_COPY(ent->base->gamma, pt);
//		glColor4f(pt[0], pt[1], pt[2], 1);

		Skel_DrawModel(p);
		Skel_StepTime(p, pdgl_dt_f);

		glPopMatrix();
	}

	if(!strcmp(ty, "lbxgl_bspmodel_t"))
	{
	}

	if(!strcmp(ty, "lbxgl_quakemdl_t"))
	{
		fr=0;
		sk=0;
		q=LBXGL_Entity_GetProperty(ent, "frame");
		if(dytypep(q, "_float_t"))fr=dyfloatv(q);
		if(dytypep(q, "_string_t"))
			fr=LBXGL_QuakeMDL_IndexFrame(p, q);

		q=LBXGL_Entity_GetProperty(ent, "skin");
		if(dytypep(q, "_float_t"))sk=dyfloatv(q);
		if(dytypep(q, "_string_t"))
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
	int LBXGL_Entity_RenderEnts(LBXGL_World *wrl, LBXGL_Entity *lst);
Description
	Render a list of entities.
--*/
int LBXGL_Entity_RenderEnts(LBXGL_World *wrl, LBXGL_Entity *lst)
{
	LBXGL_Entity *cur;

//	return(0);

	cur=lst;
	while(cur)
	{
		LBXGL_Entity_Render(wrl, cur);
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
	static int init=0;

	if(init)return(0);
	init=1;

//	LBXGL_BS1IF_Init();

	return(0);
}
