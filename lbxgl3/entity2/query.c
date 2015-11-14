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

/*--
Cat lbxgl;Entity;Query
Form
	double LBXGL_Entity_GetRadius(LBXGL_Entity *ent);
Description
	Get the radius associated with an entity.
	The radius is computed via the bounding boxes, and will fully \
	enclose the box.
--*/
double LBXGL_Entity_GetRadius(LBXGL_Entity *ent)
{
//	double *cmins, *cmaxs;
	float cmins[3], cmaxs[3];
	double crad;

	crad=0;
//	cmins=LBXGL_Entity_GetProperty(ent, "mins");
//	cmaxs=LBXGL_Entity_GetProperty(ent, "maxs");

	LBXGL_Entity_GetPropertyFVector(ent, "mins", cmins, 3);
	LBXGL_Entity_GetPropertyFVector(ent, "maxs", cmaxs, 3);
	if(cmins)if(veclen(cmins)>crad)crad=veclen(cmins);
	if(cmaxs)if(veclen(cmaxs)>crad)crad=veclen(cmaxs);

	return(crad);
}

/*--
Cat lbxgl;Entity;Query
Form
	int LBXGL_Entity_QueryRadius(LBXGL_Entity *ents, \
	LBXGL_Entity **array, int max, \
	double *origin, double radius);
Description
	Query all entities in list withn the sphere defined by origin and \
	radius.
	array is used to hold the query results, and max is the maximal \
	number of entities that can be placed in array (at least max+1 \
	entries should exist within array to handle the trailing NULL).

	This returns the number of entities placed in array.
--*/
int LBXGL_Entity_QueryRadius(LBXGL_Entity *ents,
	LBXGL_Entity **array, int max,
	double *origin, double radius)
{
	LBXGL_Entity *cur;
	int i, j;
	double *corg, *cmins, *cmaxs;
	double crad;

	cur=ents;
	i=0;
	while(cur && (i<max))
	{
		corg=LBXGL_Entity_GetProperty(cur, "origin");
		if(!corg)
		{
			cur=cur->next;
			continue;
		}

		crad=LBXGL_Entity_GetRadius(cur);
		if(vecdist(corg, origin)<=(radius+crad))
		{
			array[i++]=cur;
			array[i]=NULL;
		}

		cur=cur->next;
	}
	return(i);
}

/*--
Cat lbxgl;Entity;Query
Form
	int LBXGL_Entity_QueryLine(LBXGL_Entity *ents, \
	LBXGL_Entity **array, int max, \
	double *start, double *end);
Description
	Query entities which intersect with the line defined by start and end.

	array is used to hold the query results, and max is the maximal \
	number of entities that can be placed in array (at least max+1 \
	entries should exist within array to handle the trailing NULL).

	This returns the number of entities placed in array.
--*/
int LBXGL_Entity_QueryLine(LBXGL_Entity *ents,
	LBXGL_Entity **array, int max,
	double *start, double *end)
{
	LBXGL_Entity *cur;
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
		corg=LBXGL_Entity_GetProperty(cur, "origin");
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

		crad=LBXGL_Entity_GetRadius(cur);
		if((dl<=crad) && (dz>=0) && (dz<=de))
		{
			array[i++]=cur;
			array[i]=NULL;
		}

		cur=cur->next;
	}
	return(i);
}

/*--
Cat lbxgl;Entity;Query
Form
	int LBXGL_Entity_QueryProperty(LBXGL_Entity *ents, \
	LBXGL_Entity **array, int max, \
	char *name, void *value);
Description
	Query entities for which the property 'name' is equal to value.

	array is used to hold the query results, and max is the maximal \
	number of entities that can be placed in array (at least max+1 \
	entries should exist within array to handle the trailing NULL).

	This returns the number of entities placed in array.
--*/
int LBXGL_Entity_QueryProperty(LBXGL_Entity *ents,
	LBXGL_Entity **array, int max,
	char *name, void *value)
{
	LBXGL_Entity *cur;
	int i;
	void *p;

	cur=ents;
	i=0;
	while(cur && (i<max))
	{
		p=LBXGL_Entity_GetProperty(cur, name);
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

/*--
Cat lbxgl;Entity;Query
Form
	int LBXGL_Entity_QueryProperties(LBXGL_Entity *ents, \
	LBXGL_Entity **array, int max, \
	char **names, char **ops, void **values);
Description
	Query all entities in the list where each property named, compared \
	with the associated operator and value, is true.
	The names list is NULL terminated, this signifies the end of the
	comparisions list.

	array is used to hold the query results, and max is the maximal \
	number of entities that can be placed in array (at least max+1 \
	entries should exist within array to handle the trailing NULL).

	This returns the number of entities placed in array.
--*/
int LBXGL_Entity_QueryProperties(LBXGL_Entity *ents,
	LBXGL_Entity **array, int max,
	char **names, char **ops, void **values)
{
	LBXGL_Entity *cur;
	int i, j, op, ok;
	void *p, *t;

	cur=ents;
	i=0;
	while(cur && (i<max))
	{
		ok=1;
		for(j=0; names[j]; j++)
		{
			p=LBXGL_Entity_GetProperty(cur, names[j]);
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


/*--
Cat lbxgl;Entity;Query
Form
	LBXGL_Entity *LBXGL_Entity_QueryRadiusNext(LBXGL_Entity *ents, \
	double *origin, double radius);
Description
	Finds the first entity in the list of ents within the radius, \
	and returns that entity.

	Returns NULL if none match or the list is empty.
--*/
LBXGL_Entity *LBXGL_Entity_QueryRadiusNext(LBXGL_Entity *ents,
	double *origin, double radius)
{
	LBXGL_Entity *cur;
	int i, j;
	double *corg, *cmins, *cmaxs;
	double crad;

	cur=ents;
	while(cur)
	{
		corg=LBXGL_Entity_GetProperty(cur, "origin");
		if(!corg)
		{
			cur=cur->next;
			continue;
		}

		crad=LBXGL_Entity_GetRadius(cur);
		if(vecdist(corg, origin)<=(radius+crad))
			return(cur);

		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat lbxgl;Entity;Query
Form
	LBXGL_Entity *LBXGL_Entity_QueryLineNext(LBXGL_Entity *ents, \
	double *start, double *end);
Description
	Query entities which intersect with the line defined by start and end.

	Returns the first entity which intersects with the line.
	Returns NULL if none match or the list is empty.
--*/
LBXGL_Entity *LBXGL_Entity_QueryLineNext(LBXGL_Entity *ents,
	double *start, double *end)
{
	LBXGL_Entity *cur;
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
		corg=LBXGL_Entity_GetProperty(cur, "origin");
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

		crad=LBXGL_Entity_GetRadius(cur);
		if((dl<=crad) && (dz>=0) && (dz<=de))
			return(cur);

		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat lbxgl;Entity;Query
Form
	LBXGL_Entity *LBXGL_Entity_QueryPropertyNext(LBXGL_Entity *ents, \
	char *name, void *value);
Description
	Query entities for which the property 'name' is equal to value.

	Returns the first entity which matches the criterion.
	Returns NULL if none match or the list is empty.
--*/
LBXGL_Entity *LBXGL_Entity_QueryPropertyNext(LBXGL_Entity *ents,
	char *name, void *value)
{
	LBXGL_Entity *cur;
	int i;
	void *p;

	cur=ents;
	while(cur)
	{
		p=LBXGL_Entity_GetProperty(cur, name);
		if(PDSCR0_Interp_Equal(p, value))
			return(cur);
		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat lbxgl;Entity;Query
Form
	LBXGL_Entity *LBXGL_Entity_QueryPropertiesNext(LBXGL_Entity *ents, \
	char **names, char **ops, void **values);
Description
	Query all entities in the list where each property named, compared \
	with the associated operator and value, is true.
	The names list is NULL terminated, this signifies the end of the
	comparisions list.

	Returns the first entity which matches the criteria.
	Returns NULL if none match or the list is empty.
--*/
LBXGL_Entity *LBXGL_Entity_QueryPropertiesNext(LBXGL_Entity *ents,
	char **names, char **ops, void **values)
{
	LBXGL_Entity *cur;
	int i, j, op, ok;
	void *p, *t;

	cur=ents;
	while(cur)
	{
		ok=1;
		for(j=0; names[j]; j++)
		{
			p=LBXGL_Entity_GetProperty(cur, names[j]);
			op=PDSCR_Compile_IndexBinary(ops[j]);
			t=PDSCR0_Interp_BinaryOperation(p, values[j], op);
			ok=ok && NetVal_UnwrapBool(t);
		}
		if(ok)return(cur);

		cur=cur->next;
	}
	return(NULL);
}

/*--
Cat lbxgl;Entity;Query
Form
	LBXGL_Entity *LBXGL_Entity_QueryStringPropertyNext(\
	LBXGL_Entity *ents, char *name, char *value);
Description
	Query entities for which the property 'name' is equal to value.
	Assumes name, if present, is a string.

	Returns the first entity which matches the criterion.
	Returns NULL if none match or the list is empty.
--*/
LBXGL_Entity *LBXGL_Entity_QueryStringPropertyNext(LBXGL_Entity *ents,
	char *name, char *value)
{
	LBXGL_Entity *cur;
	int i;
	void *s;

	cur=ents;
	while(cur)
	{
		s=LBXGL_Entity_GetPropertyString(cur, name);
		if(s && !strcmp(s, value))return(cur);
		cur=cur->next;
	}
	return(NULL);
}
