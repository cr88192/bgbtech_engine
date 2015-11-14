/*
 * Entity C-based script wrappers
 */

#include <lbxgl2.h>

char *ent_model_name[1024];
void *ent_model_ptr[1024];
int ent_n_models=0;

void *EntGet(Entity *ent, char *name)
	{ return(LBXGL_Entity_GetProperty(ent, name)); }
char *EntGetString(Entity *ent, char *name)
	{ return(LBXGL_Entity_GetPropertyString(ent, name)); }
int EntGetInt(Entity *ent, char *name)
	{ return(LBXGL_Entity_GetPropertyFloat(ent, name)); }
double EntGetFloat(Entity *ent, char *name)
	{ return(LBXGL_Entity_GetPropertyFloat(ent, name)); }
int EntGetBool(Entity *ent, char *name)
	{ return(LBXGL_Entity_GetPropertyBool(ent, name)); }
int EntGetFV(Entity *ent, char *name, float *vec, int num)
	{ return(LBXGL_Entity_GetPropertyFVector(ent, name, vec, num)); }
int EntGetDV(Entity *ent, char *name, double *vec, int num)
	{ return(LBXGL_Entity_GetPropertyDVector(ent, name, vec, num)); }

void EntSet(Entity *ent, char *name, void *val)
	{ LBXGL_Entity_SetProperty(ent, name, val); }
void EntSetString(Entity *ent, char *name, char *val)
	{ LBXGL_Entity_SetPropertyString(ent, name, val); }
void EntSetInt(Entity *ent, char *name, int val)
	{ LBXGL_Entity_SetPropertyFloat(ent, name, val); }
void EntSetFloat(Entity *ent, char *name, double val)
	{ LBXGL_Entity_SetPropertyFloat(ent, name, val); }
void EntSetBool(Entity *ent, char *name, int val)
	{ LBXGL_Entity_SetPropertyBool(ent, name, val); }
void EntSetFV(Entity *ent, char *name, float *vec, int num)
	{ LBXGL_Entity_SetPropertyFVector(ent, name, vec, num); }
void EntSetDV(Entity *ent, char *name, double *vec, int num)
	{ LBXGL_Entity_SetPropertyDVector(ent, name, vec, num); }

int EntGet2FV(Entity *ent, char *name, float *vec)
	{ return(EntGetFV(ent, name, vec, 2)); }
void EntSet2FV(Entity *ent, char *name, float *vec)
	{ EntSetFV(ent, name, vec, 2); }
int EntGet3FV(Entity *ent, char *name, float *vec)
	{ return(EntGetFV(ent, name, vec, 3)); }
void EntSet3FV(Entity *ent, char *name, float *vec)
	{ EntSetFV(ent, name, vec, 3); }
int EntGet4FV(Entity *ent, char *name, float *vec)
	{ return(EntGetFV(ent, name, vec, 4)); }
void EntSet4FV(Entity *ent, char *name, float *vec)
	{ EntSetFV(ent, name, vec, 4); }

int EntGet9FV(Entity *ent, char *name, float *vec)
	{ return(EntGetFV(ent, name, vec, 9)); }
void EntSet9FV(Entity *ent, char *name, float *vec)
	{ EntSetFV(ent, name, vec, 9); }

int EntGet2DV(Entity *ent, char *name, double *vec)
	{ return(EntGetDV(ent, name, vec, 2)); }
void EntSet2DV(Entity *ent, char *name, double *vec)
	{ EntSetDV(ent, name, vec, 2); }
int EntGet3DV(Entity *ent, char *name, double *vec)
	{ return(EntGetDV(ent, name, vec, 3)); }
void EntSet3DV(Entity *ent, char *name, double *vec)
	{ EntSetDV(ent, name, vec, 3); }
int EntGet4DV(Entity *ent, char *name, double *vec)
	{ return(EntGetDV(ent, name, vec, 4)); }
void EntSet4DV(Entity *ent, char *name, double *vec)
	{ EntSetDV(ent, name, vec, 4); }

int EntGet9DV(Entity *ent, char *name, double *vec)
	{ return(EntGetDV(ent, name, vec, 9)); }
void EntSet9DV(Entity *ent, char *name, double *vec)
	{ EntSetDV(ent, name, vec, 9); }

void EntSet2F(Entity *ent, char *name, double x, double y)
	{ double v[4]; v[0]=x; v[1]=y; EntSetDV(ent, name, v, 2); }
void EntSet3F(Entity *ent, char *name, double x, double y, double z)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; EntSetDV(ent, name, v, 3); }
void EntSet4F(Entity *ent, char *name, double x, double y, double z, double w)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; v[3]=w; EntSetDV(ent, name, v, 4); }


int EntGetFlag(Entity *ent, char *name)
	{ return(LBXGL_Entity_GetPropertyFlag(ent, name)); }

int EntIsDirty(Entity *ent, char *name)
{
	int i;

	i=LBXGL_Entity_GetPropertyFlag(ent, name);
	if(i&1)LBXGL_Entity_SetPropertyFlag(ent, name, i&(~1));
	return(i&1);
}


void EntSetDLight(Entity *ent, float r, float g, float b)
{
	LBXGL_Light *dl;

	dl=EntGet(ent, "_dlight");
	if(!dl)
	{
		dl=LBXGL_Entity_NewDLightWorld(ent->world);
		EntSet(ent, "_dlight", dl);
	}

	EntGet3FV(ent, "origin", dl->org);

	V3F_SET(dl->clr, r, g, b);
	dl->val[0]=Vec3F_Normalize(dl->clr, dl->clr);
	dl->flags|=LBXGL_LFL_CHANGED;

	dl->leaf=BSP_PointLeafNode(ent->world->mdls->root, dl->org);
}

void EntClearDLight(Entity *ent)
{
	LBXGL_Light *dl, *cur, *lst;

	dl=EntGet(ent, "_dlight");
	if(!dl)return;

	cur=ent->world->dlights; lst=NULL;
	while(cur && (cur!=dl))
		{ lst=cur; cur=cur->next; }

	if(!cur)return;
	if(lst)
	{
		lst->next=cur->next;
	}else
	{
		ent->world->dlights=cur->next;
	}
}

void EntSpeak(Entity *ent, char *text)
{
	char buf[64];
	float org[4], vel[4];
	PDGL_VoiceState *vs;
	char *s;
	int rt, tn;

	EntGet3FV(ent, "origin", org);
	EntGet3FV(ent, "velocity", vel);

	V3F_SCALE(org, 1.0/40, org);
	V3F_SCALE(vel, 1.0/40, vel);


	vs=EntGet(ent, "_voice");
	if(!vs)
	{
		vs=PDGL_Voice_New("female0", 0, 40.0, 40.0, org, vel);
		EntSet(ent, "_voice", vs);
	}

//	s=EntGetString(ent, "voice");
//	if(!s)s="female0";

//	rt=EntGetFloat(ent, "voice_rate");
//	tn=EntGetFloat(ent, "voice_tone");

//	sprintf(buf, "_%s ", s);
//	PDGL_Voice_Speak(vs, buf);

//	sprintf(buf, "_0r ");
//	if(rt>0)sprintf(buf, "^%dr ", rt);
//	if(rt<0)sprintf(buf, "_%dr ", -rt);
//	PDGL_Voice_Speak(vs, buf);

//	sprintf(buf, "_0a ");
//	if(tn>0)sprintf(buf, "^%da ", tn);
//	if(tn<0)sprintf(buf, "_%da ", -tn);
//	PDGL_Voice_Speak(vs, buf);

	PDGL_Voice_Speak(vs, text);
}


void EntDestroy(Entity *ent)
{
	int i;

	EntClearDLight(ent);

	i=LBXGL_Entity_GetPropertyFloat(ent, "_solid");
//	if(i>=0)bsdeHide(i);
	if(i>0)bsdeDestroy(i);

	LBXGL_Entity_Unlink(ent);
}

void EntRemove(Entity *ent)
	{ EntDestroy(ent); }

int EntSetCallback(Entity *ent, char *name, void *data,
	void *(*func)(Entity *ent, void *data, int nargs, void **args))
{
	LBXGL_Entity_SetCallback(ent, name, func, data);
	return(0);
}

int EntSetGCallback(char *name, void *data,
	void *(*func)(Entity *ent, void *data, int nargs, void **args))
{
	LBXGL_Entity_SetGCallback(name, func, data);
	return(0);
}

int EntSetCallback0(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 0, func); return(0); }
int EntSetCallback1(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 1, func); return(0); }
int EntSetCallback2(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 2, func); return(0); }
int EntSetCallback3(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 3, func); return(0); }
int EntSetCallback4(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 4, func); return(0); }
int EntSetCallback5(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 5, func); return(0); }
int EntSetCallback6(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 6, func); return(0); }
int EntSetCallback7(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 7, func); return(0); }
int EntSetCallback8(Entity *ent, char *name, void *(*func)())
	{ LBXGL_Entity_SetCallbackN(ent, name, 8, func); return(0); }


void *EntSendMessage(Entity *ent, char *name,
	int nargs, void **args)
{
	void *p;
	p=LBXGL_Entity_SendMessage(ent, name, nargs, args);
	return(p);
}

int EntSendMessageAll(Entity *ents, char *name,
	int nargs, void **args)
{
	int i;
	i=LBXGL_Entity_SendMessageAll(ents, name, nargs, args);
	return(i);
}

int EntSendMessageWorld(World *wrl, char *name,
	int nargs, void **args)
{
	int i;
	i=LBXGL_Entity_SendMessageAll(wrl->ents, name, nargs, args);
	return(i);
}


void *EntSendMessage0(Entity *ent, char *name)
	{ return(EntSendMessage(ent, name, 1, NULL)); }
void *EntSendMessage1(Entity *ent, char *name, void *a)
{	void *args[4];	args[0]=a;
	return(EntSendMessage(ent, name, 1, args)); }
void *EntSendMessage2(Entity *ent, char *name, void *a, void *b)
{	void *args[4];	args[0]=a; args[1]=b;
	return(EntSendMessage(ent, name, 2, args)); }
void *EntSendMessage3(Entity *ent, char *name, void *a, void *b, void *c)
{	void *args[4];	args[0]=a; args[1]=b; args[2]=c;
	return(EntSendMessage(ent, name, 3, args)); }
void *EntSendMessage4(Entity *ent, char *name,
	void *a, void *b, void *c, void *d)
{	void *args[4];	args[0]=a; args[1]=b; args[2]=c; args[3]=d;
	return(EntSendMessage(ent, name, 4, args)); }

void EntSetSolid(Entity *ent, char *str)
	{ LBXGL_Physics_SetSolidType(ent, str); }
void EntSetMove(Entity *ent, char *str)
	{ LBXGL_Physics_SetMoveType(ent, str); }


void *EntLoadModel(char *mdl)
{
	char *s, *t, *ty;
	void *p;
	int i;

	for(i=0; i<ent_n_models; i++)
		if(!strcmp(ent_model_name[i], mdl))
			return(ent_model_ptr[i]);

	s=mdl;

	ty=NULL;
	if(!ty)
	{
		t=s+strlen(s);
		while((t>s) && (*t!='.'))t--;
		if(*t=='.')t++;

		if(!kstricmp(t, "mdl"))ty="QuakeMDL";
		if(!kstricmp(t, "md2"))ty="QuakeMDL";
		if(!kstricmp(t, "md3"))ty="QuakeMDL";

		if(!kstricmp(t, "ac"))ty="AC3D";

		if(!kstricmp(t, "map"))ty="QuakeMap";
		if(!kstricmp(t, "bsp"))ty="BSP";
		if(!kstricmp(t, "ski"))ty="SkelModelInfo";
		if(!kstricmp(t, "model"))ty="SkelModelInfo2";
	}

	if(!ty)return(NULL);

	p=NULL;
	if(!strcmp(ty, "QuakeMDL"))
		p=LBXGL_QuakeMDL_LoadModel(s);

//	if(!strcmp(ty, "QuakeMap"))
//		p=LBXGL_QuakeMD2_LoadModel(s);

	if(!strcmp(ty, "BSP"))
		p=BSP_ReadBSP(s);

	if(!strcmp(ty, "AC3D"))
		p=LBXGL_Mesh_LoadModel(s);

	if(!strcmp(ty, "SkelModelInfo"))
		p=LBXGL_FmtSmd_LoadModelInfo(s);

	if(!strcmp(ty, "SkelModelInfo2"))
	{
		p=Skel_LoadModel(s);
		if(!p)return(NULL);
		p=Skel_NewInstance(p);
	}


	i=ent_n_models++;
	ent_model_name[i]=kstrdup(mdl);
	ent_model_ptr[i]=p;

	return(p);
}

void EntSetModel(Entity *ent, char *mdl)
{
	void *p;

	if(!mdl || !*mdl)
	{
		EntSet(ent, "model", NULL);
		EntSet(ent, "mdl", NULL);
		return;
	}

//	printf("EntSetModel: %p %s\n", ent, mdl);
//	return;

	p=EntLoadModel(mdl);
	if(!p)return;

//	printf("EntSetModel: Model=%p %s\n", p);

	EntSetString(ent, "model", mdl);
	EntSet(ent, "mdl", p);
}

void EntSetAnim(Entity *ent, char *anim)
{
	void *p;

	p=EntGet(ent, "mdl");
	if(p)Skel_SetAnim(p, anim);
}


void EntEmitSound(Entity *ent, char *snd, float vol, float scale)
{
	float org[3], vel[3];

	EntGet3FV(ent, "origin", org);
	EntGet3FV(ent, "velocity", vel);
	Vec3F_Scale(org, 1.0/40, org);
	Vec3F_Scale(vel, 1.0/40, vel);

	PDGL_Sound_PlayPointSample(snd, 0, vol, scale, org, vel);
}

void EntEmitSoundPoint(char *snd, float *pt, float vol, float scale)
{
	float org[3], vel[3];

	Vec3F_Scale(pt, 1.0/40, org);
	V3F_ZERO(vel);
//	Vec3F_Scale(vel, 1.0/40, vel);

	PDGL_Sound_PlayPointSample(snd, 0, vol, scale, org, vel);
}

float EntRangeRandom(float min, float max)
{
	float f, r;

	r=max-min;
	f=r*rand()/((float)RAND_MAX);
	return(f+min);
}



void EntDamage(Entity *ent, Entity *other, int hp)
{
	void *p;

	p=EntSendMessage2(ent, "render", other, dyint(hp));
	if(p!=UNDEFINED)return;

	p=LBXGL_Entity_GetProperty(ent, "damage");
	if(p)	((int (*)(LBXGL_Entity *self, LBXGL_Entity *other, int hp))p)
			(ent, other, hp);
}


Entity *EntQueryAttrString(World *wrl, char *var, char *val)
{
	Entity *cur, *lst;
	char *s;

	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		s=EntGetString(cur, var);
		if(s && !strcmp(s, val))
		{	cur->chain=lst;
			lst=cur; }
		cur=cur->next;
	}

	return(lst);
}

Entity *EntSubQueryAttrString(World *wrl, char *var, char *val)
{
	Entity *cur, *nxt, *lst;
	char *s;

	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		s=EntGetString(cur, var);
		if(s && !strcmp(s, val))
		{	cur->chain=lst;
			lst=cur; }
		cur=nxt;
	}

	return(lst);
}

Entity **EntFlattenChain(Entity *lst)
{
	Entity *cur, **arr;
	int i;

	cur=lst; i=0;
	while(cur) { i++; cur=cur->chain; }
	arr=gcalloc((i+1)*sizeof(Entity *));
	cur=lst; i=0;
	while(cur) { arr[i++]=cur; cur=cur->chain; }
	arr[i++]=NULL;

	return(arr);
}

Entity **EntFlattenChainN(Entity *lst, int *rn)
{
	Entity *cur, **arr;
	int i;

	cur=lst; i=0;
	while(cur) { i++; cur=cur->chain; }
	*rn=i;

	arr=gcalloc((i+1)*sizeof(Entity *));
	cur=lst; i=0;
	while(cur) { arr[i++]=cur; cur=cur->chain; }
	arr[i++]=NULL;

	return(arr);
}

int EntUseTargetName(World *wrl, char *name)
{
	static char *tstk[16];
	static int tstkpos;
	Entity *lst, **arr;
	int i;

	for(i=0; i<tstkpos; i++)
		if(!strcmp(tstk[i], name))
			return(-1);
	if(tstkpos>=16)return(-1);
	tstk[tstkpos++]=name;

	lst=EntQueryAttrString(wrl, "targetname", name);
	arr=EntFlattenChain(lst);
	i=0; while(arr[i]) { EntSendMessage0(arr[i], "use"); i++; }

	tstkpos--;
	return(0);
}

int EntUseTarget(Entity *ent)
{
	char *s;

	s=EntGetString(ent, "target");
	if(!s)return(-1);
	EntUseTargetName(ent->world, s);
	return(0);
}

Entity *EntFindRadius(World *wrl, float *org, float radius)
{
	Entity *cur, *lst;
	float io[3], f;

	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		EntGet3FV(cur, "origin", io);
		f=V3F_DIST(io, org);

		if(f<=radius)
		{
			cur->chain=lst;
			lst=cur;
		}

		cur=cur->next;
	}

	return(lst);
}

void EntRadiusDamage(Entity *ent, Entity *src, float radius, int hp,
	Entity *skip)
{
	Entity *cur, *lst;
	float org[3], io[3], d, f;

	EntGet3FV(ent, "origin", org);

	cur=ent->world->ents; lst=NULL;
	while(cur)
	{
		EntGet3FV(cur, "origin", io);
		d=V3F_DIST(io, org);

		if((d<=radius) && (cur!=skip))
		{
			f=(1.0-(d/radius))*hp;
			EntDamage(cur, src, f);
		}

		cur=cur->next;
	}
}

int EntDrawSolid(Entity *self)
{
	static int fca[256], fva[4*256];
	static float va[3*256];

	float org[3], rot[9], im1[16];
	float mins[3], maxs[3], iv[4];
	float f, g;
	int solid, nf, nv, tfv;
	int i, j, k;


	solid=EntGetFloat(self, "_solid");
	if(solid<0)return(0);

	EntGet3FV(self, "origin", org);
	EntGet9FV(self, "rot", rot);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glTranslatef(org[0], org[1], org[2]);

	im1[ 0]=rot[0]; im1[ 1]=rot[1]; im1[ 2]=rot[2]; im1[ 3]=0;
	im1[ 4]=rot[3]; im1[ 5]=rot[4]; im1[ 6]=rot[5]; im1[ 7]=0;
	im1[ 8]=rot[6]; im1[ 9]=rot[7]; im1[10]=rot[8]; im1[11]=0;
	im1[12]=0;	im1[13]=0;	im1[14]=0;	im1[15]=1.0;

	glMultMatrixf(im1);


	i=bsdeGetAttrI(solid, BSDE_SOLIDTYPE);
	switch(i)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		bsdeGetAttrFV(solid, BSDE_MINS, mins, 3);
		bsdeGetAttrFV(solid, BSDE_MAXS, maxs, 3);
		PD3D_DrawCube(mins[0], mins[1], mins[2],
			maxs[0], maxs[1], maxs[2]);
//		PD3D_DrawGammaCube(mins[0], mins[1], mins[2],
//			maxs[0], maxs[1], maxs[2], cur->gamma);
		break;
	case BSDE_SOLID_SPHERE:
		f=bsdeGetAttrF(solid, BSDE_RADIUS);
		PD3D_DrawSphere(0, 0, 0, f);
//		PD3D_DrawGammaSphere(f, cur->gamma);
		break;
	case BSDE_SOLID_CYLINDER:
		f=bsdeGetAttrF(solid, BSDE_RADIUS);
		g=bsdeGetAttrF(solid, BSDE_HEIGHT);
		PD3D_DrawCylinder(0, 0, 0, g, f, f);
		PD3D_DrawDisk(g*0.5, 0, f);
		PD3D_DrawDisk(-g*0.5, 1, f);
		break;
	case BSDE_SOLID_CCYLINDER:
		f=bsdeGetAttrF(solid, BSDE_RADIUS);
		g=bsdeGetAttrF(solid, BSDE_HEIGHT);
		PD3D_DrawCylinder(0, 0, 0, g, f, f);
		PD3D_DrawSphere(0, 0, g*0.5, f);
		PD3D_DrawSphere(0, 0, -g*0.5, f);
		break;

	case BSDE_SOLID_PLANE:
		bsdeGetAttrFV(solid, BSDE_DIRECTION, iv, 4);
		PD3D_DrawPlane(iv[0], iv[1], iv[2], iv[3], 999);
		break;

	case BSDE_SOLID_TRIMESH:
		nf=bsdeGetAttrI(solid, BSDE_NUM_FACES);
		nv=bsdeGetAttrI(solid, BSDE_NUM_VERTICES);

		bsdeGetAttrIV(solid, BSDE_FACE_VERTICES, fva, nf*3);
		bsdeGetAttrFV(solid, BSDE_VERTICES, va, nv*3);

		glColor4f(0, 1, 0, 0.75);


		glBegin(GL_TRIANGLES);
		for(i=0; i<nf; i++)
		{
			glVertex3fv(va+fva[i*3+0]*3);
			glVertex3fv(va+fva[i*3+1]*3);
			glVertex3fv(va+fva[i*3+2]*3);
		}
		glEnd();

		break;

	case BSDE_SOLID_HULL:
		nf=bsdeGetAttrI(solid, BSDE_NUM_FACES);
		nv=bsdeGetAttrI(solid, BSDE_NUM_VERTICES);
		bsdeGetAttrIV(solid, BSDE_FACE_COUNTS, fca, nf);
		tfv=0;
		for(i=0; i<nf; i++)tfv+=fca[i];

		bsdeGetAttrIV(solid, BSDE_FACE_VERTICES, fva, tfv);
		bsdeGetAttrFV(solid, BSDE_VERTICES, va, nv*3);

		glColor4f(1, 0, 0, 0.75);



		k=0;
		for(i=0; i<nf; i++)
		{
			glBegin(GL_POLYGON);
			for(j=0; j<fca[i]; j++)
				glVertex3fv(va+fva[k++]*3);
			glEnd();
		}

		break;

	default:
		break;
	}

	glPopMatrix();

	return(0);
}


int trigger_relay(Entity *self);

void *ent_dummy_ptr;
static void EntInitSetDummy(void *p)
{
	ent_dummy_ptr=p;
}

int EntInit()
{
	EntInitSetDummy(&Prim3D_DrawFlatCube);
	EntInitSetDummy(&PD3D_DrawFlatCube);
	EntInitSetDummy(&trigger_relay);
}

