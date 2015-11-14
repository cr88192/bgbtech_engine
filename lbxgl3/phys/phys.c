//do entity physics

#include <lbxgl2.h>

double lbxgl_physics_gravity[3];
double lbxgl_physics_ground;
double lbxgl_physics_sky;
double lbxgl_physics_friction;
int lbxgl_physics_init=0;

static inline double veclen(double *vec)
{
	return(sqrt((vec[0]*vec[0])+(vec[1]*vec[1])+(vec[2]*vec[2])));
}

int LBXGL_Physics_MoveExtents(LBXGL_Entity *ent, float dt,
	double *min, double *max)
{
	double *org, *vel;
	double *mins, *maxs;
	double f;
	int i;

	org=LBXGL_Entity_GetProperty(ent, "origin");
	vel=LBXGL_Entity_GetProperty(ent, "velocity");
	mins=LBXGL_Entity_GetProperty(ent, "mins");
	maxs=LBXGL_Entity_GetProperty(ent, "maxs");

	if(!org)return(-1);

	for(i=0; i<3; i++)min[i]=org[i];
	for(i=0; i<3; i++)max[i]=org[i];

	if(!mins || !maxs)
		return(-1);

	for(i=0; i<3; i++)
	{
		f=mins[i]+org[i];
		if(f<min[i])min[i]=f;
		if(f>max[i])max[i]=f;

		f=maxs[i]+org[i];
		if(f<min[i])min[i]=f;
		if(f>max[i])max[i]=f;

		if(vel)
		{
			f=mins[i]+org[i]+(vel[i]*dt);
			if(f<min[i])min[i]=f;
			if(f>max[i])max[i]=f;

			f=maxs[i]+org[i]+(vel[i]*dt);
			if(f<min[i])min[i]=f;
			if(f>max[i])max[i]=f;
		}
	}

#if 0
	if(vel)
	{
		for(i=0; i<3; i++)
			min[i]+=vel[i]*dt*0.5;
		for(i=0; i<3; i++)
			max[i]+=vel[i]*dt*0.5;
	}
#endif

	return(0);
}

int LBXGL_Physics_MoveBoxExtents(double *org, double *vel,
	double *mins, double *maxs, double dt,
	double *min, double *max)
{
	double f;
	int i;

	for(i=0; i<3; i++)min[i]=org[i];
	for(i=0; i<3; i++)max[i]=org[i];

	for(i=0; i<3; i++)
	{
		f=mins[i]+org[i];
		if(f<min[i])min[i]=f;
		if(f>max[i])max[i]=f;

		f=maxs[i]+org[i];
		if(f<min[i])min[i]=f;
		if(f>max[i])max[i]=f;

		if(vel)
		{
			f=mins[i]+org[i]+(vel[i]*dt);
			if(f<min[i])min[i]=f;
			if(f>max[i])max[i]=f;

			f=maxs[i]+org[i]+(vel[i]*dt);
			if(f<min[i])min[i]=f;
			if(f>max[i])max[i]=f;
		}
	}

	return(0);
}

int LBXGL_Physics_NOkAxis(LBXGL_Entity *aent, LBXGL_Entity *bent)
{
	double *aorg, *amins, *amaxs;
	double *borg, *bmins, *bmaxs;
	double ma[3], na[3], mb[3], nb[3];
	int i, j, k;

	aorg=LBXGL_Entity_GetProperty(aent, "origin");
	amins=LBXGL_Entity_GetProperty(aent, "mins");
	amaxs=LBXGL_Entity_GetProperty(aent, "maxs");
	borg=LBXGL_Entity_GetProperty(bent, "origin");
	bmins=LBXGL_Entity_GetProperty(bent, "mins");
	bmaxs=LBXGL_Entity_GetProperty(bent, "maxs");

	if(!aorg || !amins || !amaxs)return(0);
	if(!borg || !bmins || !bmaxs)return(0);

	k=0;
	for(i=0; i<6; i++)
	{
		for(j=0; j<3; j++)
		{
			ma[j]=aorg[j]+amins[j];
			na[j]=aorg[j]+amaxs[j];
			mb[j]=borg[j]+bmins[j];
			nb[j]=borg[j]+bmaxs[j];
		}
		if(i<3)
		{
			na[i]=999999999;
			mb[i]=-999999999;
		}else
		{
			ma[i-3]=-999999999;
			nb[i-3]=999999999;
		}
		if(LBXGL_Physics_BoxCollideP(ma, na, mb, nb))k|=1<<i;
	}
	return(k);
}

int LBXGL_Physics_BoxCollideP(double *ma, double *na, double *mb, double *nb)
{
	if(na[0]<=mb[0])return(0);
	if(na[1]<=mb[1])return(0);
	if(na[2]<=mb[2])return(0);
	if(nb[0]<=ma[0])return(0);
	if(nb[1]<=ma[1])return(0);
	if(nb[2]<=ma[2])return(0);

	return(1);
}

int LBXGL_Physics_BoxCollideAxis(double *ma, double *na, double *mb, double *nb)
{
	int i;

	i=0;
	if((na[0]>mb[0]) && !(ma[0]>=nb[0]))i|=1;
	if((na[1]>mb[1]) && !(ma[1]>=nb[1]))i|=2;
	if((na[2]>mb[2]) && !(ma[2]>=nb[2]))i|=4;

	if((nb[0]>ma[0]) && !(mb[0]>=na[0]))i|=8;
	if((nb[1]>ma[1]) && !(mb[1]>=na[1]))i|=16;
	if((nb[2]>ma[2]) && !(mb[2]>=na[2]))i|=32;

	return(i);
}

int LBXGL_Physics_PointInBoxP(double *p, double *min, double *max)
{
	if(p[0]<=min[0])return(0);
	if(p[0]>=max[0])return(0);
	if(p[1]<=min[1])return(0);
	if(p[1]>=max[1])return(0);
	if(p[2]<=min[2])return(0);
	if(p[2]>=max[2])return(0);

	return(1);
}

LBXGL_Entity *LBXGL_Physics_CheckMove(LBXGL_Entity *ent,
	LBXGL_Entity *ents, float dt, double *min, double *max)
{
	LBXGL_Entity *cur, *ig;
	double m[3], n[3];
	int i;

	cur=ents;
	while(cur)
	{
		ig=LBXGL_Entity_GetProperty(cur, "ignore");
		i=LBXGL_Physics_MoveExtents(cur, dt, m, n);
		if(!i && (cur!=ent) && (ig!=ent))
		{
			if(LBXGL_Physics_BoxCollideP(m, n, min, max))
				return(cur);
		}
		cur=cur->next;
	}
	return(NULL);
}

LBXGL_Entity *LBXGL_Physics_MoveTimeExtents(LBXGL_Entity *ent,
	LBXGL_Entity *ents, float dt)
{
	double *org, *vel;
	double *mins, *maxs;
	double m[3], n[3], mb[3], nb[3], dv[3];
	double mv[3];
	double f, ts;
	int i;
	LBXGL_Entity *other, *tent, *ltent;

	org=LBXGL_Entity_GetProperty(ent, "origin");
	vel=LBXGL_Entity_GetProperty(ent, "velocity");

	if(!org || !vel)return(NULL);
	if(veclen(vel)<0.1)return(NULL);

	for(i=0; i<3; i++)mv[i]=vel[i];

//	LBXGL_Physics_MoveExtents(ent, dt, m, n);

	mins=LBXGL_Entity_GetProperty(ent, "mins");
	maxs=LBXGL_Entity_GetProperty(ent, "maxs");

	other=NULL;
	if(mins && maxs)
	{
		LBXGL_Physics_MoveBoxExtents(org, vel, mins, maxs, dt, m, n);
		other=LBXGL_Physics_CheckMove(ent, ents, dt, m, n);
	}
	if(!other)
	{
		org[0]+=vel[0]*dt;
		org[1]+=vel[1]*dt;
		org[2]+=vel[2]*dt;

		if(org[2]<lbxgl_physics_ground)
		{
			org[2]=lbxgl_physics_ground;
			if(vel[2]<0)vel[2]=0;

			vel[0]=vel[0]-(vel[0]*lbxgl_physics_friction*dt);
			vel[1]=vel[1]-(vel[1]*lbxgl_physics_friction*dt);
			vel[2]=vel[2]-(vel[2]*lbxgl_physics_friction*dt);
		}
		if(org[2]>lbxgl_physics_sky)
		{
			org[2]=lbxgl_physics_sky;
			if(vel[2]>0)vel[2]=0;
		}

		return(NULL);
	}else
	{
		tent=other;
		while(tent)
		{
			i=LBXGL_Physics_NOkAxis(ent, tent);
			if((i&1) && (vel[0]>0))vel[0]=0;
			if((i&2) && (vel[1]>0))vel[1]=0;
			if((i&4) && (vel[2]>0))vel[2]=0;
			if((i&8) && (vel[0]<0))vel[0]=0;
			if((i&16) && (vel[1]<0))vel[1]=0;
			if((i&32) && (vel[2]<0))vel[2]=0;

			ltent=tent;
			LBXGL_Physics_MoveBoxExtents(org, vel, mins, maxs, dt, m, n);
			tent=LBXGL_Physics_CheckMove(ent, tent->next, dt, m, n);
//			if(tent==ltent)break;
		}
		org[0]+=vel[0]*dt;
		org[1]+=vel[1]*dt;
		org[2]+=vel[2]*dt;

		if(org[2]<lbxgl_physics_ground)
		{
			org[2]=lbxgl_physics_ground;
			if(vel[2]<0)vel[2]=0;
		}
		if(org[2]>lbxgl_physics_sky)
		{
			org[2]=lbxgl_physics_sky;
			if(vel[2]>0)vel[2]=0;
		}

		vel[0]=vel[0]-(vel[0]*lbxgl_physics_friction*dt);
		vel[1]=vel[1]-(vel[1]*lbxgl_physics_friction*dt);
		vel[2]=vel[2]-(vel[2]*lbxgl_physics_friction*dt);

#if 0
		//check for stuck'ed ness
		tent=LBXGL_Physics_CheckMove(ent, ents, 0, mins, maxs);
		while(tent)
		{
			i=LBXGL_Physics_NOkAxis(ent, tent);
			if((i&0x3f)!=0x3f)break;

			kprint("unsticking %p %p (%lf %lf %lf)\n", ent, tent,
				org[0], org[1], org[2]);
			if(veclen(mv)<0.1)mv[2]=-1;
			if(dt<0.1)dt=0.1;
			org[0]-=mv[0]*dt;
			org[1]-=mv[1]*dt;
			org[2]-=mv[2]*dt;
//			tent=LBXGL_Physics_CheckMove(ent, ents, 0,
//				mins, maxs);
			break;
		}
#endif
	}

#if 0
	if((dt/2)<0.001)return(other);

	other=LBXGL_Physics_MoveTimeExtents(ent, ents, dt/2);
	if(other)return(other);

	//first half ok
	org=LBXGL_Entity_GetProperty(ent, "origin");
	vel=LBXGL_Entity_GetProperty(ent, "velocity");
	if(org && vel)
	{
		org[0]+=vel[0]*(dt/2);
		org[1]+=vel[1]*(dt/2);
		org[2]+=vel[2]*(dt/2);
	}

	other=LBXGL_Physics_MoveTimeExtents(ent, ents, dt/2);
#endif

	return(other);
}

int LBXGL_Physics_Step(LBXGL_Entity *ents, float dt)
{
	LBXGL_Entity *cur, *tmp;
	double *fp;
	void *a[4];
	void *p;
	void *dtp;

	dtp=NetVal_WrapFloat(dt);

	cur=ents;
	while(cur)
	{
		a[0]=dtp;
		a[1]=NULL;
		LBXGL_Entity_SendMessage(cur, "premove", a);

		fp=LBXGL_Entity_GetProperty(cur, "thinktime");
		if(fp)
		{
//			kprint("thinktime %p %lf\n", fp, *fp);
			if(*fp>0)*fp-=dt;
			if((*fp<=0) && (*fp!=-1))
			{
				*fp=-1;

				a[0]=NULL;
				LBXGL_Entity_SendMessage(cur, "think", a);

			}
		}

		tmp=LBXGL_Physics_MoveTimeExtents(cur, ents, dt);
		if(tmp)
		{
			p=LBXGL_Entity_GetProperty(cur, "touching");
			if(!p)
			{
				p=NetVal_WrapBool(0);
				LBXGL_Entity_SetProperty(cur, "touching", p);
			}
			if(!(*(int *)p))
			{
				a[0]=tmp;
				a[1]=NULL;
				LBXGL_Entity_SendMessage(cur, "touch", a);
				*(int *)p=1;
			}

			p=LBXGL_Entity_GetProperty(tmp, "touching");
			if(!p)
			{
				p=NetVal_WrapBool(0);
				LBXGL_Entity_SetProperty(tmp, "touching", p);
			}
			if(!NetVal_UnwrapBool(p))
			{
				a[0]=cur;
				a[1]=NULL;
				LBXGL_Entity_SendMessage(tmp, "touch", a);
				*(int *)p=1;
			}
		}else
		{
			p=NetVal_WrapBool(0);
			LBXGL_Entity_SetProperty(cur, "touching", p);
		}

		a[0]=dtp;
		a[1]=NULL;
		LBXGL_Entity_SendMessage(cur, "postmove", a);
		cur=cur->next;
	}
	return(0);
}

//move in a specific direction a specific amount
//returns NULL if move ok, otherwise returning the blocking entity
LBXGL_Entity *LBXGL_Physics_MoveDir(LBXGL_Entity *ent,
	LBXGL_Entity *ents, double *dir, double dist)
{
	double *org;
	double *mins, *maxs;
	double m[3], n[3], mb[3], nb[3], dv[3];
	double f, ts;
	int i;
	LBXGL_Entity *other, *tent, *ltent;

	org=LBXGL_Entity_GetProperty(ent, "origin");

	if(!org)return(NULL);

	mins=LBXGL_Entity_GetProperty(ent, "mins");
	maxs=LBXGL_Entity_GetProperty(ent, "maxs");

	if(mins && maxs)
	{
		LBXGL_Physics_MoveBoxExtents(org, dir, mins, maxs, dist, m, n);
		other=LBXGL_Physics_CheckMove(ent, ents, 0, m, n);
	}
	if(!other)
	{
		org[0]+=dir[0]*dist;
		org[1]+=dir[1]*dist;
		org[2]+=dir[2]*dist;

		return(NULL);
	}
	return(other);
}

int LBXGL_Physics_ApplyForce(LBXGL_Entity *ent, double *force, float dt)
{
	double *vel;
	int *ng;
	int i;

	ng=LBXGL_Entity_GetProperty(ent, "nograv");
	if(ng)if(*ng)return(0);

	vel=LBXGL_Entity_GetProperty(ent, "velocity");
	if(!vel)return(0);

	for(i=0; i<3; i++)
		vel[i]+=force[i]*dt;
	return(0);
}

int LBXGL_Physics_ApplyGeneralForce(LBXGL_Entity *ents,
	double *force, float dt)
{
	LBXGL_Entity *cur;

	cur=ents;
	while(cur)
	{
		LBXGL_Physics_ApplyForce(cur, force, dt);
		cur=cur->next;
	}
	return(0);
}

int lbxgl_physics_ev_attack(void *data,
	char *target, char *event, void **args)
{
	double *fa, *fb;
	float f;

	fa=args[0];
	fb=args[1];
	f=NetVal_UnwrapFloat(args[2]);

	kprint("Event: attack (%lf %lf %lf) (%lf %lf %lf) %d\n",
		fa[0], fa[1], fa[2], fb[0], fb[1], fb[2], f);

	return(0);
}

int LBXGL_Physics_Init()
{
	if(lbxgl_physics_init)
		return(0);
	lbxgl_physics_init=1;

	lbxgl_physics_gravity[0]=0;
	lbxgl_physics_gravity[1]=0;
	lbxgl_physics_gravity[2]=-1;

	lbxgl_physics_ground=-100;
	lbxgl_physics_sky=30000;
	lbxgl_physics_friction=5.0;

	LBXGL_Event_RegisterHandler(NULL, "attack_vec",
		&lbxgl_physics_ev_attack, NULL);

	LBXGL_Physics2_Init();
	LBXGL_PhysSkel_Init();
}
