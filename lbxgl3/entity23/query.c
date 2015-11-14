#include <lbxgl2.h>

static inline double veclen(double *vec)
{
	return(sqrt((vec[0]*vec[0])+(vec[1]*vec[1])+(vec[2]*vec[2])));
}

static inline double vecnorm(double *vec)
{
	double l;

	l=(vec[0]*vec[0])+(vec[1]*vec[1])+(vec[2]*vec[2]);
	l=sqrt(l);
	if(l>0)
	{
		vec[0]/=l;
		vec[1]/=l;
		vec[2]/=l;
	}

	return(sqrt(l));
}

static inline double vecdist(double *avec, double *bvec)
{
	double cvec[3];

	cvec[0]=bvec[0]-avec[0];
	cvec[1]=bvec[1]-avec[1];
	cvec[2]=bvec[2]-avec[2];
	return(veclen(cvec));
}

static inline double vecdot(double *avec, double *bvec)
{
	return(sqrt((avec[0]*bvec[0])+(avec[1]*bvec[1])+(avec[2]*bvec[2])));
}

static inline int perp_vector(double *dir, double *dst)
{
	double tvec[3];
	double l;
	int i, j;

	for(i=0; i<3; i++)
	{
		for(j=0; j<3; j++)tvec[j]=0;
		tvec[i]=1;
		l=vecdot(tvec, dir);

		for(j=0; j<3; j++)tvec[j]-=l*tvec[j];
		if(vecnorm(tvec))
		{
			for(j=0; j<3; j++)dst[j]=tvec[j];
			return(0);
		}
	}
	return(0);
}

static inline int veccross(double *avec, double *bvec, double *cvec)
{
	cvec[0]=(avec[1]*bvec[2])-(avec[2]*bvec[1]);
	cvec[1]=(avec[2]*bvec[0])-(avec[0]*bvec[2]);
	cvec[2]=(avec[0]*bvec[1])-(avec[1]*bvec[0]);
	return(0);
}

double lbxEntGetRadius(lbxEntity *ent)
{
	float cmins[3], cmaxs[3];
	double crad;

	crad=0;

	lbxEntGetPropertyFVector(ent, "mins", cmins, 3);
	lbxEntGetPropertyFVector(ent, "maxs", cmaxs, 3);
	if(cmins)if(veclen(cmins)>crad)crad=veclen(cmins);
	if(cmaxs)if(veclen(cmaxs)>crad)crad=veclen(cmaxs);

	return(crad);
}

int lbxEntQueryRadius(lbxEntity *ents,
	lbxEntity **array, int max,
	double *origin, double radius)
{
	lbxEntity *cur;
	int i, j;
	double *corg, *cmins, *cmaxs;
	double crad;

	cur=ents;
	i=0;
	while(cur && (i<max))
	{
		corg=lbxEntGetProperty(cur, "origin");
		if(!corg)
		{
			cur=cur->next;
			continue;
		}

		crad=lbxEntGetRadius(cur);
		if(vecdist(corg, origin)<=(radius+crad))
		{
			array[i++]=cur;
			array[i]=NULL;
		}

		cur=cur->next;
	}
	return(i);
}

int lbxEntQueryLine(lbxEntity *ents,
	lbxEntity **array, int max,
	double *start, double *end)
{
	lbxEntity *cur;
	int i, j;
	double *corg, *cmins, *cmaxs;
	double crad, dx, dy, dz, dl, de;
	double dir[3], px[3], py[3];
	double rorg[3];

	for(i=0; i<3; i++)dir[i]=end[i]-start[i];
	vecnorm(dir);
	perp_vector(dir, px);
	veccross(dir, px, py);

	for(i=0; i<3; i++)rorg[i]=end[i]-start[i];
	de=vecdot(rorg, dir);

	cur=ents;
	i=0;
	while(cur && (i<max))
	{
		corg=lbxEntGetProperty(cur, "origin");
		if(!corg)
		{
			cur=cur->next;
			continue;
		}

		for(j=0; j<3; j++)rorg[j]=corg[j]-start[j];
		dx=vecdot(rorg, px);
		dy=vecdot(rorg, py);
		dz=vecdot(rorg, dir);
		dl=sqrt((dx*dx)+(dy*dy));

		crad=lbxEntGetRadius(cur);
		if((dl<=crad) && (dz>=0) && (dz<=de))
		{
			array[i++]=cur;
			array[i]=NULL;
		}

		cur=cur->next;
	}
	return(i);
}

int lbxEntQueryProperty(lbxEntity *ents,
	lbxEntity **array, int max,
	char *name, void *value)
{
	lbxEntity *cur;
	int i;
	void *p;

	cur=ents;
	i=0;
	while(cur && (i<max))
	{
		p=lbxEntGetProperty(cur, name);
		if(!PDSCR0_Interp_Equal(p, value))
		{
			cur=cur->next;
			continue;
		}

		array[i++]=cur;
		array[i]=NULL;
		cur=cur->next;
	}
	return(i);
}

int lbxEntQueryProperties(lbxEntity *ents,
	lbxEntity **array, int max,
	char **names, char **ops, void **values)
{
	lbxEntity *cur;
	int i, j, op, ok;
	void *p, *t;

	cur=ents;
	i=0;
	while(cur && (i<max))
	{
		ok=1;
		for(j=0; names[j]; j++)
		{
			p=lbxEntGetProperty(cur, names[j]);
			op=PDSCR_Compile_IndexBinary(ops[j]);
			t=PDSCR0_Interp_BinaryOperation(p, values[j], op);
			ok=ok && NetVal_UnwrapBool(t);
		}
		if(!ok)
		{
			cur=cur->next;
			continue;
		}

		array[i++]=cur;
		array[i]=NULL;
		cur=cur->next;
	}
	return(i);
}

lbxEntity *lbxEntQueryRadiusNext(lbxEntity *ents,
	double *origin, double radius)
{
	lbxEntity *cur;
	int i, j;
	double *corg, *cmins, *cmaxs;
	double crad;

	cur=ents;
	while(cur)
	{
		corg=lbxEntGetProperty(cur, "origin");
		if(!corg)
		{
			cur=cur->next;
			continue;
		}

		crad=lbxEntGetRadius(cur);
		if(vecdist(corg, origin)<=(radius+crad))
			return(cur);

		cur=cur->next;
	}
	return(NULL);
}

lbxEntity *lbxEntQueryLineNext(lbxEntity *ents,
	double *start, double *end)
{
	lbxEntity *cur;
	int i, j;
	double *corg, *cmins, *cmaxs;
	double crad, dx, dy, dz, dl, de;
	double dir[3], px[3], py[3];
	double rorg[3];

	for(i=0; i<3; i++)dir[i]=end[i]-start[i];
	vecnorm(dir);
	perp_vector(dir, px);
	veccross(dir, px, py);

	for(i=0; i<3; i++)rorg[i]=end[i]-start[i];
	de=vecdot(rorg, dir);

	cur=ents;
	while(cur)
	{
		corg=lbxEntGetProperty(cur, "origin");
		if(!corg)
		{
			cur=cur->next;
			continue;
		}

		for(j=0; j<3; j++)rorg[j]=corg[j]-start[j];
		dx=vecdot(rorg, px);
		dy=vecdot(rorg, py);
		dz=vecdot(rorg, dir);
		dl=sqrt((dx*dx)+(dy*dy));

		crad=lbxEntGetRadius(cur);
		if((dl<=crad) && (dz>=0) && (dz<=de))
			return(cur);

		cur=cur->next;
	}
	return(NULL);
}

lbxEntity *lbxEntQueryPropertyNext(lbxEntity *ents,
	char *name, void *value)
{
	lbxEntity *cur;
	int i;
	void *p;

	cur=ents;
	while(cur)
	{
		p=lbxEntGetProperty(cur, name);
		if(PDSCR0_Interp_Equal(p, value))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

lbxEntity *lbxEntQueryPropertiesNext(lbxEntity *ents,
	char **names, char **ops, void **values)
{
	lbxEntity *cur;
	int i, j, op, ok;
	void *p, *t;

	cur=ents;
	while(cur)
	{
		ok=1;
		for(j=0; names[j]; j++)
		{
			p=lbxEntGetProperty(cur, names[j]);
			op=PDSCR_Compile_IndexBinary(ops[j]);
			t=PDSCR0_Interp_BinaryOperation(p, values[j], op);
			ok=ok && NetVal_UnwrapBool(t);
		}
		if(ok)return(cur);

		cur=cur->next;
	}
	return(NULL);
}

lbxEntity *lbxEntQueryStringPropertyNext(lbxEntity *ents,
	char *name, char *value)
{
	lbxEntity *cur;
	int i;
	void *s;

	cur=ents;
	while(cur)
	{
		s=lbxEntGetPropertyString(cur, name);
		if(s && !strcmp(s, value))return(cur);
		cur=cur->next;
	}
	return(NULL);
}
