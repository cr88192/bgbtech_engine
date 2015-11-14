/*
 * Entity C-based script wrappers
 */

#include <lbxgl2.h>

char *ent_model_name[1024];
void *ent_model_ptr[1024];
int ent_n_models=0;

LBXGL_API void *lbxEntGet(lbxEntity *ent, char *name)
	{ return(lbxEntGetProperty(ent, name)); }
LBXGL_API char *lbxEntGetString(lbxEntity *ent, char *name)
	{ return(lbxEntGetPropertyString(ent, name)); }
LBXGL_API int lbxEntGetInt(lbxEntity *ent, char *name)
	{ return(lbxEntGetPropertyFloat(ent, name)); }
LBXGL_API double lbxEntGetFloat(lbxEntity *ent, char *name)
	{ return(lbxEntGetPropertyFloat(ent, name)); }
LBXGL_API int lbxEntGetBool(lbxEntity *ent, char *name)
	{ return(lbxEntGetPropertyBool(ent, name)); }
LBXGL_API int lbxEntGetFV(lbxEntity *ent, char *name, float *vec, int num)
	{ return(lbxEntGetPropertyFVector(ent, name, vec, num)); }
LBXGL_API int lbxEntGetDV(lbxEntity *ent, char *name, double *vec, int num)
	{ return(lbxEntGetPropertyDVector(ent, name, vec, num)); }

LBXGL_API void lbxEntSet(lbxEntity *ent, char *name, void *val)
	{ lbxEntSetProperty(ent, name, val); }
LBXGL_API void lbxEntSetString(lbxEntity *ent, char *name, char *val)
	{ lbxEntSetPropertyString(ent, name, val); }
LBXGL_API void lbxEntSetInt(lbxEntity *ent, char *name, int val)
	{ lbxEntSetPropertyFloat(ent, name, val); }
LBXGL_API void lbxEntSetFloat(lbxEntity *ent, char *name, double val)
	{ lbxEntSetPropertyFloat(ent, name, val); }
LBXGL_API void lbxEntSetBool(lbxEntity *ent, char *name, int val)
	{ lbxEntSetPropertyBool(ent, name, val); }
LBXGL_API void lbxEntSetFV(lbxEntity *ent, char *name, float *vec, int num)
	{ lbxEntSetPropertyFVector(ent, name, vec, num); }
LBXGL_API void lbxEntSetDV(lbxEntity *ent, char *name, double *vec, int num)
	{ lbxEntSetPropertyDVector(ent, name, vec, num); }

LBXGL_API int lbxEntGet2FV(lbxEntity *ent, char *name, float *vec)
	{ return(lbxEntGetFV(ent, name, vec, 2)); }
LBXGL_API void lbxEntSet2FV(lbxEntity *ent, char *name, float *vec)
	{ lbxEntSetFV(ent, name, vec, 2); }
LBXGL_API int lbxEntGet3FV(lbxEntity *ent, char *name, float *vec)
	{ return(lbxEntGetFV(ent, name, vec, 3)); }
LBXGL_API void lbxEntSet3FV(lbxEntity *ent, char *name, float *vec)
	{ lbxEntSetFV(ent, name, vec, 3); }
LBXGL_API int lbxEntGet4FV(lbxEntity *ent, char *name, float *vec)
	{ return(lbxEntGetFV(ent, name, vec, 4)); }
LBXGL_API void lbxEntSet4FV(lbxEntity *ent, char *name, float *vec)
	{ lbxEntSetFV(ent, name, vec, 4); }

LBXGL_API int lbxEntGet9FV(lbxEntity *ent, char *name, float *vec)
	{ return(lbxEntGetFV(ent, name, vec, 9)); }
LBXGL_API void lbxEntSet9FV(lbxEntity *ent, char *name, float *vec)
	{ lbxEntSetFV(ent, name, vec, 9); }

LBXGL_API int lbxEntGet2DV(lbxEntity *ent, char *name, double *vec)
	{ return(lbxEntGetDV(ent, name, vec, 2)); }
LBXGL_API void lbxEntSet2DV(lbxEntity *ent, char *name, double *vec)
	{ lbxEntSetDV(ent, name, vec, 2); }
LBXGL_API int lbxEntGet3DV(lbxEntity *ent, char *name, double *vec)
	{ return(lbxEntGetDV(ent, name, vec, 3)); }
LBXGL_API void lbxEntSet3DV(lbxEntity *ent, char *name, double *vec)
	{ lbxEntSetDV(ent, name, vec, 3); }
LBXGL_API int lbxEntGet4DV(lbxEntity *ent, char *name, double *vec)
	{ return(lbxEntGetDV(ent, name, vec, 4)); }
LBXGL_API void lbxEntSet4DV(lbxEntity *ent, char *name, double *vec)
	{ lbxEntSetDV(ent, name, vec, 4); }

LBXGL_API int lbxEntGet9DV(lbxEntity *ent, char *name, double *vec)
	{ return(lbxEntGetDV(ent, name, vec, 9)); }
LBXGL_API void lbxEntSet9DV(lbxEntity *ent, char *name, double *vec)
	{ lbxEntSetDV(ent, name, vec, 9); }

LBXGL_API void lbxEntSet2F(lbxEntity *ent, char *name, double x, double y)
	{ double v[4]; v[0]=x; v[1]=y; lbxEntSetDV(ent, name, v, 2); }
LBXGL_API void lbxEntSet3F(lbxEntity *ent, char *name, double x, double y, double z)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; lbxEntSetDV(ent, name, v, 3); }
LBXGL_API void lbxEntSet4F(lbxEntity *ent, char *name, double x, double y, double z, double w)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; v[3]=w; lbxEntSetDV(ent, name, v, 4); }


LBXGL_API int lbxEntGetFlag(lbxEntity *ent, char *name)
	{ return(lbxEntGetPropertyFlag(ent, name)); }
LBXGL_API void lbxEntSetFlag(lbxEntity *ent, char *name, int fl)
	{ lbxEntSetPropertyFlag(ent, name, fl); }

LBXGL_API int lbxEntIsDirty(lbxEntity *ent, char *name)
{
	int i;

	i=lbxEntGetPropertyFlag(ent, name);
	if(i&1)lbxEntSetPropertyFlag(ent, name, i&(~1));
	return(i&1);
}


LBXGL_API void lbxEntGetOrg3FV(lbxEntity *ent, float *vec)
	{ lbxEntGetFV(ent, "origin", vec, 3); }
LBXGL_API void lbxEntGetOrg3DV(lbxEntity *ent, double *vec)
	{ lbxEntGetDV(ent, "origin", vec, 3); }
LBXGL_API void lbxEntGetVel3FV(lbxEntity *ent, float *vec)
	{ lbxEntGetFV(ent, "velocity", vec, 3); }
LBXGL_API void lbxEntGetVel3DV(lbxEntity *ent, double *vec)
	{ lbxEntGetDV(ent, "velocity", vec, 3); }

LBXGL_API void lbxEntGetRot9FV(lbxEntity *ent, float *vec)
	{ lbxEntGetFV(ent, "rot", vec, 9); }
LBXGL_API void lbxEntGetRot9DV(lbxEntity *ent, double *vec)
	{ lbxEntGetDV(ent, "rot", vec, 9); }

LBXGL_API void lbxEntGetRot4FV(lbxEntity *ent, float *vec)
{
	float tv[9];
	lbxEntGetRot9FV(ent, tv);
	QuatF_From3Matrix(tv, vec);
}

LBXGL_API void lbxEntGetRot4DV(lbxEntity *ent, double *vec)
{
	float tv[9], tq[4];
	lbxEntGetRot9FV(ent, tv);
	QuatF_From3Matrix(tv, tq);
	vec[0]=tq[0]; vec[1]=tq[1];
	vec[2]=tq[2]; vec[3]=tq[3];
}

LBXGL_API void lbxEntSetOrg3F(lbxEntity *ent,
	double x, double y, double z)
{
	double v[4]; v[0]=x; v[1]=y; v[2]=z;

	LBXGL_EntPhys_RecvEntState(ent->world, ent);
	lbxEntSetDV(ent, "origin", v, 3);
	LBXGL_EntPhys_SendEntState(ent->world, ent);
}

LBXGL_API void lbxEntSetVel3F(lbxEntity *ent,
	double x, double y, double z)
{
	double v[4]; v[0]=x; v[1]=y; v[2]=z;

	LBXGL_EntPhys_RecvEntState(ent->world, ent);
	lbxEntSetDV(ent, "velocity", v, 3);
	LBXGL_EntPhys_SendEntState(ent->world, ent);
}

LBXGL_API void lbxEntSetRot4F(lbxEntity *ent,
	double w, double x, double y, double z)
{
	float v[4], tv[9];

	v[0]=x; v[1]=y; v[2]=z; v[3]=w;
	QuatF_To3Matrix(v, tv);

	LBXGL_EntPhys_RecvEntState(ent->world, ent);
	lbxEntSetFV(ent, "rot", tv, 9);
	LBXGL_EntPhys_SendEntState(ent->world, ent);
}

LBXGL_API void lbxEntSetRot9F(lbxEntity *ent,
	double ix, double iy, double iz,
	double jx, double jy, double jz,
	double kx, double ky, double kz)
{
	double v[9];
	v[0]=ix; v[1]=iy; v[2]=iz;
	v[3]=jx; v[4]=jy; v[5]=jz;
	v[6]=kx; v[7]=ky; v[8]=kz;

	LBXGL_EntPhys_RecvEntState(ent->world, ent);
	lbxEntSetDV(ent, "rot", v, 9);
	LBXGL_EntPhys_SendEntState(ent->world, ent);
}


LBXGL_API void lbxEntSetOrg3FV(lbxEntity *ent, float *vec)
	{ lbxEntSetOrg3F(ent, vec[0], vec[1], vec[2]); }
LBXGL_API void lbxEntSetOrg3DV(lbxEntity *ent, double *vec)
	{ lbxEntSetOrg3F(ent, vec[0], vec[1], vec[2]); }

LBXGL_API void lbxEntSetVel3FV(lbxEntity *ent, float *vec)
	{ lbxEntSetVel3F(ent, vec[0], vec[1], vec[2]); }
LBXGL_API void lbxEntSetVel3DV(lbxEntity *ent, double *vec)
	{ lbxEntSetVel3F(ent, vec[0], vec[1], vec[2]); }

LBXGL_API void lbxEntSetRot4FV(lbxEntity *ent, float *vec)
	{ lbxEntSetRot4F(ent, vec[3], vec[0], vec[1], vec[2]); }
LBXGL_API void lbxEntSetRot4DV(lbxEntity *ent, double *vec)
	{ lbxEntSetRot4F(ent, vec[3], vec[0], vec[1], vec[2]); }

LBXGL_API void lbxEntSetRot9FV(lbxEntity *ent, float *vec)
	{ lbxEntSetRot9F(ent,
		vec[0], vec[1], vec[2],
		vec[3], vec[4], vec[5],
		vec[6], vec[7], vec[8]); }
LBXGL_API void lbxEntSetRot9DV(lbxEntity *ent, double *vec)
	{ lbxEntSetRot9F(ent,
		vec[0], vec[1], vec[2],
		vec[3], vec[4], vec[5],
		vec[6], vec[7], vec[8]); }


LBXGL_API void lbxEntSetDLight(lbxEntity *ent, float r, float g, float b)
{
	LBXGL_Light *dl;

	dl=lbxEntGet(ent, "_dlight");
	if(!dl)
	{
		dl=lbxEntNewDLightWorld(ent->world);
		lbxEntSet(ent, "_dlight", dl);
	}

	lbxEntGet3FV(ent, "origin", dl->org);

	V3F_SET(dl->clr, r, g, b);
	dl->val[0]=Vec3F_Normalize(dl->clr, dl->clr);
	dl->flags|=LBXGL_LFL_CHANGED;
}

LBXGL_API void lbxEntClearDLight(lbxEntity *ent)
{
	LBXGL_Light *dl, *cur, *lst;

	dl=lbxEntGet(ent, "_dlight");
	if(!dl)return;

	cur=ent->world->light; lst=NULL;
	while(cur && (cur!=dl))
		{ lst=cur; cur=cur->next; }

	if(!cur)return;
	if(lst)
	{
		lst->next=cur->next;
	}else
	{
		ent->world->light=cur->next;
	}
}

LBXGL_API void lbxEntSpeak(lbxEntity *ent, char *text)
{
	char buf[64];
	float org[4], vel[4];
	PDGL_VoiceState *vs;
	char *s;
	int rt, tn;

	lbxEntGet3FV(ent, "origin", org);
	lbxEntGet3FV(ent, "velocity", vel);

	V3F_SCALE(org, 1.0/40, org);
	V3F_SCALE(vel, 1.0/40, vel);


	vs=lbxEntGet(ent, "_voice");
	if(!vs)
	{
		vs=PDGL_Voice_New("female0", 0, 40.0, 40.0, org, vel);
		lbxEntSet(ent, "_voice", vs);
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


LBXGL_API void lbxEntDestroy(lbxEntity *ent)
{
	int i;

	lbxEntClearDLight(ent);

	i=lbxEntGetPropertyFloat(ent, "_solid");
	if(i>0)bsdeDestroy(i);

	lbxEntUnlink(ent);
}

LBXGL_API void lbxEntRemove(lbxEntity *ent)
	{ lbxEntDestroy(ent); }

#if 0
LBXGL_API int lbxEntSetCallback0(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 0, func); return(0); }
LBXGL_API int lbxEntSetCallback1(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 1, func); return(0); }
LBXGL_API int lbxEntSetCallback2(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 2, func); return(0); }
LBXGL_API int lbxEntSetCallback3(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 3, func); return(0); }
LBXGL_API int lbxEntSetCallback4(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 4, func); return(0); }
LBXGL_API int lbxEntSetCallback5(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 5, func); return(0); }
LBXGL_API int lbxEntSetCallback6(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 6, func); return(0); }
LBXGL_API int lbxEntSetCallback7(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 7, func); return(0); }
LBXGL_API int lbxEntSetCallback8(lbxEntity *ent, char *name, void *(*func)())
	{ lbxEntSetCallbackN(ent, name, 8, func); return(0); }

LBXGL_API int lbxEntSendMessageWorld(lbxWorld *wrl, char *name,
	void **args, int nargs)
{
	int i;
	i=lbxEntSendMessageAll(wrl->ents, name, args, nargs);
	return(i);
}


LBXGL_API void *lbxEntSendMessage0(lbxEntity *ent, char *name)
	{ return(lbxEntSendMessage(ent, name, NULL, 0)); }
LBXGL_API void *lbxEntSendMessage1(lbxEntity *ent, char *name, void *a)
{	void *args[4];	args[0]=a;
	return(lbxEntSendMessage(ent, name, args, 1)); }
LBXGL_API void *lbxEntSendMessage2(lbxEntity *ent, char *name, void *a, void *b)
{	void *args[4];	args[0]=a; args[1]=b;
	return(lbxEntSendMessage(ent, name, args, 2)); }
LBXGL_API void *lbxEntSendMessage3(lbxEntity *ent, char *name, void *a, void *b, void *c)
{	void *args[4];	args[0]=a; args[1]=b; args[2]=c;
	return(lbxEntSendMessage(ent, name, args, 3)); }
LBXGL_API void *lbxEntSendMessage4(lbxEntity *ent, char *name,
	void *a, void *b, void *c, void *d)
{	void *args[4];	args[0]=a; args[1]=b; args[2]=c; args[3]=d;
	return(lbxEntSendMessage(ent, name, args, 4)); }

#endif

LBXGL_API void lbxEntSetSolid(lbxEntity *ent, char *str)
{
//	LBXGL_Physics_SetSolidType(ent, str);
}

LBXGL_API void lbxEntSetMove(lbxEntity *ent, char *str)
{
//	LBXGL_Physics_SetMoveType(ent, str);
}

LBXGL_API void lbxEntSetModel(lbxEntity *ent, char *mdl)
{
	void *p;

	if(!mdl || !*mdl)
	{
		lbxEntSet(ent, "model", NULL);
		lbxEntSet(ent, "mdl", NULL);
		return;
	}

	p=LBXGL_Mdl_LoadModelInstance(mdl);
	if(!p)return;

	lbxEntSetString(ent, "model", mdl);
	lbxEntSet(ent, "mdl", p);
}

LBXGL_API void lbxEntSetAnim(lbxEntity *ent, char *anim)
{
	void *p;

	p=lbxEntGet(ent, "mdl");
	if(p)LBXGL_Mdl_SetAnim(p, anim);
}


LBXGL_API void lbxEntEmitSound(lbxEntity *ent, char *snd, float vol, float scale)
{
	float org[3], vel[3];

	lbxEntGet3FV(ent, "origin", org);
	lbxEntGet3FV(ent, "velocity", vel);
	Vec3F_Scale(org, 1.0/40, org);
	Vec3F_Scale(vel, 1.0/40, vel);

	PDGL_Sound_PlayPointSample(snd, 0, vol, scale, org, vel);
}

LBXGL_API void lbxEntEmitSoundPoint(char *snd, float *pt, float vol, float scale)
{
	float org[3], vel[3];

	Vec3F_Scale(pt, 1.0/40, org);
	V3F_ZERO(vel);
//	Vec3F_Scale(vel, 1.0/40, vel);

	PDGL_Sound_PlayPointSample(snd, 0, vol, scale, org, vel);
}

LBXGL_API float lbxEntRangeRandom(float min, float max)
{
	float f, r;

	r=max-min;
	f=r*rand()/((float)RAND_MAX);
	return(f+min);
}



LBXGL_API void lbxEntDamage(lbxEntity *ent, lbxEntity *other, int hp)
{
	void *p;

//	p=lbxEntSendMessage2(ent, "render", other, dyint(hp));
//	if(p!=UNDEFINED)return;

	p=lbxEntGetProperty(ent, "damage");
	if(p)	((int (*)(lbxEntity *self, lbxEntity *other, int hp))p)
			(ent, other, hp);
}


LBXGL_API lbxEntity *lbxEntQueryAttrString(lbxWorld *wrl, char *var, char *val)
{
	lbxEntity *cur, *lst;
	char *s;

	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		s=lbxEntGetString(cur, var);
		if(s && !strcmp(s, val))
		{	cur->chain=lst;
			lst=cur; }
		cur=cur->next;
	}

	return(lst);
}

LBXGL_API lbxEntity *lbxEntSubQueryAttrString(lbxWorld *wrl, char *var, char *val)
{
	lbxEntity *cur, *nxt, *lst;
	char *s;

	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		nxt=cur->chain;
		s=lbxEntGetString(cur, var);
		if(s && !strcmp(s, val))
		{	cur->chain=lst;
			lst=cur; }
		cur=nxt;
	}

	return(lst);
}

LBXGL_API lbxEntity **lbxEntFlattenChain(lbxEntity *lst)
{
	lbxEntity *cur, **arr;
	int i;

	cur=lst; i=0;
	while(cur) { i++; cur=cur->chain; }
	arr=gcalloc((i+1)*sizeof(lbxEntity *));
	cur=lst; i=0;
	while(cur) { arr[i++]=cur; cur=cur->chain; }
	arr[i++]=NULL;

	return(arr);
}

LBXGL_API lbxEntity **lbxEntFlattenChainN(lbxEntity *lst, int *rn)
{
	lbxEntity *cur, **arr;
	int i;

	cur=lst; i=0;
	while(cur) { i++; cur=cur->chain; }
	*rn=i;

	arr=gcalloc((i+1)*sizeof(lbxEntity *));
	cur=lst; i=0;
	while(cur) { arr[i++]=cur; cur=cur->chain; }
	arr[i++]=NULL;

	return(arr);
}

LBXGL_API int lbxEntUseTargetName(lbxWorld *wrl, char *name)
{
	static char *tstk[16];
	static int tstkpos;
	lbxEntity *lst, **arr;
	int i;

	for(i=0; i<tstkpos; i++)
		if(!strcmp(tstk[i], name))
			return(-1);
	if(tstkpos>=16)return(-1);
	tstk[tstkpos++]=name;

	lst=lbxEntQueryAttrString(wrl, "targetname", name);
	arr=lbxEntFlattenChain(lst);
	i=0; while(arr[i])
	{
//		lbxEntSendMessage0(arr[i], "use");
		i++;
	}

	tstkpos--;
	return(0);
}

LBXGL_API int lbxEntUseTarget(lbxEntity *ent)
{
	char *s;

	s=lbxEntGetString(ent, "target");
	if(!s)return(-1);
	lbxEntUseTargetName(ent->world, s);
	return(0);
}

LBXGL_API lbxEntity *lbxEntFindRadius(lbxWorld *wrl, float *org, float radius)
{
	lbxEntity *cur, *lst;
	float io[3], f;

	cur=wrl->ents; lst=NULL;
	while(cur)
	{
		lbxEntGet3FV(cur, "origin", io);
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

LBXGL_API void lbxEntRadiusDamage(lbxEntity *ent, lbxEntity *src, float radius, int hp,
	lbxEntity *skip)
{
	lbxEntity *cur, *lst;
	float org[3], io[3], d, f;

	lbxEntGet3FV(ent, "origin", org);

	cur=ent->world->ents; lst=NULL;
	while(cur)
	{
		lbxEntGet3FV(cur, "origin", io);
		d=V3F_DIST(io, org);

		if((d<=radius) && (cur!=skip))
		{
			f=(1.0-(d/radius))*hp;
			lbxEntDamage(cur, src, f);
		}

		cur=cur->next;
	}
}

LBXGL_API int lbxEntDrawSolid(lbxEntity *self)
{
	static int fca[256], fva[4*256];
	static float va[3*256];

	float org[3], rot[9], im1[16];
	float mins[3], maxs[3], iv[4];
	float f, g;
	int solid, nf, nv, tfv;
	int i, j, k;


	solid=lbxEntGetFloat(self, "_solid");
	if(solid<0)return(0);

	lbxEntGet3FV(self, "origin", org);
	lbxEntGet9FV(self, "rot", rot);

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


// int trigger_relay(lbxEntity *self);

//void *ent_dummy_ptr;
//static void lbxEntInitSetDummy(void *p)
//{
//	ent_dummy_ptr=p;
//}

//int lbxEntInit()
//{
//	lbxEntInitSetDummy(&Prim3D_DrawFlatCube);
//	lbxEntInitSetDummy(&PD3D_DrawFlatCube);
//	lbxEntInitSetDummy(&trigger_relay);
//}

