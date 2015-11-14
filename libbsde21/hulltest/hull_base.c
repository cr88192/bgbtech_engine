#include "hull.h"

int Hull_LinePlaneIntersect(float *start, float *end,
	float *norm, float *point)
{
	float dir[16], x;

	//calc direction
	dir[0]=end[0]-start[0];
	dir[1]=end[1]-start[1];
	dir[2]=end[2]-start[2];

	x=V3F_DOT(dir, norm);
	if(x==0)return(-1);

	x=1.0/((x<0)?-x:x);
	dir[0]*=x;
	dir[1]*=x;
	dir[2]*=x;

	//calc intersection
	x=V3F_NDOT(start, norm);

	x=(x<0)?-x:x;
	point[0]=start[0]+dir[0]*x;
	point[1]=start[1]+dir[1]*x;
	point[2]=start[2]+dir[2]*x;

	return(0);
}

void Hull_AdjacentNormals(float *norm, float *udir, float *vdir)
{
	float f;

	V3F_ZERO(udir);
	V3F_ZERO(vdir);

	//UxV=N

	if(fabs(norm[0])>fabs(norm[1]))
	{
		if(fabs(norm[0])>fabs(norm[2]))
		{
			udir[(norm[0]>0)?1:2]=1; 
			vdir[(norm[0]>0)?2:1]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}else
	{
		if(fabs(norm[1])>fabs(norm[2]))
		{
			udir[(norm[1]>0)?2:0]=1; 
			vdir[(norm[1]>0)?0:2]=1;
		}else
		{
			udir[(norm[2]>0)?0:1]=1; 
			vdir[(norm[2]>0)?1:0]=1;
		}
	}

	f=V3F_DOT(udir, norm);
	V3F_ADDSCALE(udir, norm, -f, udir);
	f=V3F_DOT(vdir, norm);
	V3F_ADDSCALE(vdir, norm, -f, vdir);

	V3F_NORMALIZE(udir, udir);
	V3F_NORMALIZE(vdir, vdir);
}

void Hull_MakePlaneFace(float *norm, float *pts)
{
	float v0[3], v1[3], v2[3];

	Hull_AdjacentNormals(norm, v0, v1);

	V3F_SCALEADDSCALE(v0, -9999, v1, -9999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(0*3));
	V3F_SCALEADDSCALE(v0,  9999, v1, -9999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(1*3));
	V3F_SCALEADDSCALE(v0,  9999, v1,  9999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(2*3));
	V3F_SCALEADDSCALE(v0, -9999, v1,  9999, v2);
	V3F_ADDSCALE(v2, norm, norm[3], pts+(3*3));
}

int Hull_ClipFace(float *norm,
	float *ipts, float *opts, int num)
{
	int i, j, k, l;

	//first outside
	for(i=0; i<num; i++)if(V3F_NDOT(ipts+(i*3), norm)>0)break;

	if(i==num)	//nothing to clip
	{
		for(i=0; i<num*3; i++)opts[i]=ipts[i];
		return(num);	//nothing to clip
	}

	//first inside
	j=i;
	while(1)
	{
		if(V3F_NDOT(ipts+(j*3), norm)<=0)break;
		j=(j+1)%num;
		if(j==i)return(0);	//everything clipped
	}

	//copy inside
	i=j;
	l=0;
	while(1)
	{
		V3F_COPY(ipts+(j*3), opts+(l*3));
		l++;

		k=(j+1)%num;
		if(V3F_NDOT(ipts+(k*3), norm)>0)break;
		j=k;
	}

	//exit point
	if(V3F_NDOT(ipts+(j*3), norm)<0)
	{
		Hull_LinePlaneIntersect(
			ipts+(j*3), ipts+(k*3), norm, opts+(l*3));
		l++;
	}

	j=k;
	while(1)
	{
		k=(j+1)%num;
		if(V3F_NDOT(ipts+(k*3), norm)<=0)break;
		j=k;
	}

	//entry point
	if(V3D_NDOT(ipts+(k*3), norm)<0)
	{
		Hull_LinePlaneIntersect(
			ipts+(j*3), ipts+(k*3), norm, opts+(l*3));
		l++;
	}

	return(l);
}


Solid_Hull *Hull_BuildHull(
	float *norm, int num)
{
	static float pts0[16*3];
	static float pts1[16*3];

	Solid_Hull *tmp;
	Hull_Face *fst, *lst, *ftmp;

	float tv0[3], tv1[3], tv2[3];
	float mins[3], maxs[3];
	float sx, sy, f;
	int i, j, k, l, t;

	V3F_INITBOX(mins, maxs);

	fst=NULL;
	lst=NULL;
	for(i=0; i<num; i++)
	{
		ftmp=malloc(sizeof(Hull_Face));
		V4F_COPY(norm+i*4, ftmp->norm);
		Hull_MakePlaneFace(norm+i*4, pts0);
		l=4;

		for(j=0; j<num; j++)
		{
			if(i==j)continue;

			l=Hull_ClipFace(norm+j*4, pts0, pts1, l);
			for(k=0; k<(l*3); k++)pts0[k]=pts1[k];
		}

		ftmp->nvecs=l;
		ftmp->vecs=malloc(3*l*sizeof(float));
		ftmp->evecs=malloc(4*l*sizeof(float));

		for(j=0; j<l; j++)
		{
			V3F_COPY(pts0+j*3, ftmp->vecs+j*3);
			k=(j+1)%l;

			V3F_SUB(pts0+k*3, pts0+j*3, tv0);
			V3F_CROSS(tv0, norm+i*4, tv1);
//			V3F_CROSS(norm+i*4, tv0, tv1);

			V3F_NORMALIZE(tv1, ftmp->evecs+j*4);
			ftmp->evecs[j*4+3]=V3F_DOT(pts0+j*3, ftmp->evecs+j*4);

			V3F_MIN(mins, pts0+j*3, mins);
			V3F_MAX(maxs, pts0+j*3, maxs);
		}

		if(fst)
		{
			lst->next=ftmp;
			lst=ftmp;
		}else
		{
			fst=ftmp;
			lst=ftmp;
		}
	}

	tmp=malloc(sizeof(Solid_Hull));
	memset(tmp, 0, sizeof(Solid_Hull));
	tmp->face=fst;

	tmp->rot[0]=1;
	tmp->rot[4]=1;
	tmp->rot[8]=1;

	V3F_COPY(mins, tmp->mins);
	V3F_COPY(maxs, tmp->maxs);

	tv0[0]=(maxs[0]-mins[0])/64;
	tv0[1]=(maxs[1]-mins[1])/64;
	tv0[2]=(maxs[2]-mins[2])/64;


	f=0; t=0;
	for(i=0; i<64; i++)
		for(j=0; j<64; j++)
			for(k=0; k<64; k++)
	{
		tv1[0]=mins[0]+tv0[0]*i;
		tv1[1]=mins[1]+tv0[1]*j;
		tv1[2]=mins[2]+tv0[2]*k;

		for(l=0; l<num; l++)
			if(V3F_NDOT(tv1, norm+l*4)>0)
				break;
		if(l<num)continue;

		f+=fabs(tv1[0]*tv1[1]*tv1[2]);
		t++;
	}

	tmp->mass=1;
	tmp->moment=f/t;

	printf("moment %f %f/%d\n", tmp->moment, f, t);


	return(tmp);
}

void Hull_WorldNormal(Solid_Hull *hull, float *ndir, float *dir)
{
	dir[0]=	hull->rot[0]*ndir[0] + hull->rot[3]*ndir[1] +
		hull->rot[6]*ndir[2];

	dir[1]=	hull->rot[1]*ndir[0] + hull->rot[4]*ndir[1] +
		hull->rot[7]*ndir[2];

	dir[2]=	hull->rot[2]*ndir[0] + hull->rot[5]*ndir[1] +
		hull->rot[8]*ndir[2];

	dir[3]=ndir[3] + V3F_DOT(hull->org, dir);
}


void Hull_DrawFace(Solid_Hull *hull, Hull_Face *face)
{
	float tv[4], tv1[4], tv2[4], *v;
	float r, g, b;
	int i;

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_POLYGON);

	for(i=0; i<face->nvecs; i++)
	{
		v=face->vecs+i*3;

		r=0.5+(sin(v[0])*0.5);
		g=0.5+(sin(v[1])*0.5);
		b=0.5+(sin(v[2])*0.5);
		glColor4f(r, g, b, 1.0);

		tv[0]=hull->rot[0]*v[0] + hull->rot[3]*v[1] +
			hull->rot[6]*v[2] + hull->org[0];
		tv[1]=hull->rot[1]*v[0] + hull->rot[4]*v[1] +
			hull->rot[7]*v[2] + hull->org[1];
		tv[2]=hull->rot[2]*v[0] + hull->rot[5]*v[1] +
			hull->rot[8]*v[2] + hull->org[2];

		glVertex3fv(tv);
	}

	glEnd();


#if 0
	Hull_WorldNormal(hull, face->norm, tv1);

	glBegin(GL_LINES);
	for(i=0; i<face->nvecs; i++)
	{
		Hull_WorldNormal(hull, face->evecs+i*4, tv);
		V3F_SCALEADDSCALE(tv1, tv1[3], tv, tv[3], tv2);

		glColor4f(0, 0, 1, 1);
		glVertex3f(tv2[0],		tv2[1],		tv2[2]);

		glColor4f(0, 1, 0, 1);
		glVertex3f(tv2[0]+tv[0],	tv2[1]+tv[1],	tv2[2]+tv[2]);
	}
	glEnd();
#endif
}

void Hull_DrawFace2(Solid_Hull *hull, Hull_Face *face)
{
	float tv[4], tv1[4], tv2[4], *v;
	float r, g, b;
	int i;

	glDisable(GL_TEXTURE_2D);

	glColor4f(1, 0, 0, 0.5);

	glBegin(GL_POLYGON);
	for(i=0; i<face->nvecs; i++)
	{
		v=face->vecs+i*3;

		tv[0]=hull->rot[0]*v[0] + hull->rot[3]*v[1] +
			hull->rot[6]*v[2] + hull->org[0];
		tv[1]=hull->rot[1]*v[0] + hull->rot[4]*v[1] +
			hull->rot[7]*v[2] + hull->org[1];
		tv[2]=hull->rot[2]*v[0] + hull->rot[5]*v[1] +
			hull->rot[8]*v[2] + hull->org[2];

		glVertex3fv(tv);
	}
	glEnd();
}

void Hull_DrawHull(Solid_Hull *hull)
{
	Hull_Face *cur;

	cur=hull->face;
	while(cur)
	{
		Hull_DrawFace(hull, cur);
		cur=cur->next;
	}
}



void Hull_AxisTo3Matrix(float *a, float th, float *b)
{
	float xx, xy, xz, xw, yy, yz, yw, zz, zw;
	float v[4];
	float sa, ca;
	int i;

	sa=sin(th/2);
	ca=cos(th/2);
	V3F_NORMALIZE(a, v);

	v[0]=v[0]*sa;
	v[1]=v[1]*sa;
	v[2]=v[2]*sa;
	v[3]=ca;

	for(i=0; i<9; i++)b[i]=0;
	for(i=0; i<3; i++)b[(i*3)+i]=1;

	xx=v[0]*v[0];	xy=v[0]*v[1];	xz=v[0]*v[2];	xw=v[0]*v[3];
			yy=v[1]*v[1];	yz=v[1]*v[2];	yw=v[1]*v[3];
					zz=v[2]*v[2];	zw=v[2]*v[3];

	b[0]=1-2*(yy+zz);	b[1]=  2*(xy+zw);	b[2]=  2*(xz-yw);
	b[3]=  2*(xy-zw);	b[4]=1-2*(xx+zz);	b[5]=  2*(yz+xw);
	b[6]=  2*(xz+yw);	b[7]=  2*(yz-xw);	b[8]=1-2*(xx+yy);
}

void Hull_Mat3Mult(float *a, float *b, float *c)
{
	c[0]=a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
	c[1]=a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
	c[2]=a[0]*b[2] + a[1]*b[5] + a[2]*b[8];
	c[3]=a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
	c[4]=a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
	c[5]=a[3]*b[2] + a[4]*b[5] + a[5]*b[8];
	c[6]=a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
	c[7]=a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
	c[8]=a[6]*b[2] + a[7]*b[5] + a[8]*b[8];
}

void Hull_Filter3Matrix(float *a, float *b)
{
	float v0[3], v1[3], v2[3];

	V3F_NORMALIZE(a+0, v0);

	V3F_ADDSCALE(a+3, v0, -V3F_DOT(a+3, v0), v1);
	V3F_NORMALIZE(v1, v1);

	V3F_ADDSCALE(a+6, v0, -V3F_DOT(a+6, v0), v2);
	V3F_ADDSCALE(v2, v1, -V3F_DOT(v2, v1), v2);
	V3F_NORMALIZE(v2, v2);

	V3F_COPY(v0, b+0);
	V3F_COPY(v1, b+3);
	V3F_COPY(v2, b+6);
}

void Hull_Rotate3Matrix(float *a, float *v, float th, float *b)
{
	float dm[9], tm[9];

	Hull_AxisTo3Matrix(v, th, dm);
	Hull_Mat3Mult(a, dm, tm);
	Hull_Filter3Matrix(tm, b);
}

void Hull_StepHull(Solid_Hull *hull, float dt)
{
	float f;

	V3F_ADD(hull->vel, hull->ivel, hull->vel);
	V3F_ADD(hull->tvel, hull->itvel, hull->tvel);
	V3F_ZERO(hull->ivel);
	V3F_ZERO(hull->itvel);

	V3F_ADDSCALE(hull->org, hull->vel, dt, hull->org);

	V3F_SCALE(hull->tvel, 1.0/(hull->mass*hull->moment), hull->avel);

	f=V3F_LEN(hull->avel);
	Hull_Rotate3Matrix(hull->rot, hull->avel, f*dt, hull->rot);
}

void Hull_ApplyPointForceHull(Solid_Hull *self, float *org, float *dir)
{
	double io[3], nra[3];
	double vel[3], f;
	double va[3], vb[3];

	V3F_SUB(org, self->org, io);
	V3F_CROSS(io, dir, nra);
	V3F_ADD(self->itvel, nra, self->itvel);

	V3F_SCALE(dir, 1/self->mass, vel);
	V3F_ADD(self->ivel, vel, self->ivel);
}



void Hull_ProjectHull(Solid_Hull *hull, float *ndir, float *rm, float *rn)
{
	Hull_Face *cur;

	float dir[3];
	float m, n, d;
	int i;

	dir[0]=V3F_DOT(ndir, hull->rot+0);
	dir[1]=V3F_DOT(ndir, hull->rot+3);
	dir[2]=V3F_DOT(ndir, hull->rot+6);

	m=999999;
	n=-999999;

	cur=hull->face;
	while(cur)
	{
		for(i=0; i<cur->nvecs; i++)
		{
			d=V3F_DOT(cur->vecs+i*3, dir);
			if(d<m)m=d;
			if(d>n)n=d;
		}

		cur=cur->next;
	}

	d=V3F_DOT(hull->org, ndir);
	*rm=m+d;
	*rn=n+d;
}

void Hull_ProjectHullFace(Solid_Hull *hull, Hull_Face *face,
	float *ndir, float *rm, float *rn)
{
	float dir[3];
	float m, n, d;
	int i;

	dir[0]=V3F_DOT(ndir, hull->rot+0);
	dir[1]=V3F_DOT(ndir, hull->rot+3);
	dir[2]=V3F_DOT(ndir, hull->rot+6);

	m=999999;
	n=-999999;

	for(i=0; i<face->nvecs; i++)
	{
		d=V3F_DOT(face->vecs+i*3, dir);
		if(d<m)m=d;
		if(d>n)n=d;
	}

	d=V3F_DOT(hull->org, ndir);
	*rm=m+d;
	*rn=n+d;
}

int Hull_CheckHullFaces(Solid_Hull *hulla, Hull_Face *facea,
	Solid_Hull *hullb, Hull_Face *faceb)
{
	float dir[4];
	float m, n;
	int i;

	Hull_WorldNormal(hulla, facea->norm, dir);
	Hull_ProjectHullFace(hullb, faceb, dir, &m, &n);
	if((n<dir[3]) || (m>dir[3]))return(0);

	Hull_WorldNormal(hullb, faceb->norm, dir);
	Hull_ProjectHullFace(hulla, facea, dir, &m, &n);
	if((n<dir[3]) || (m>dir[3]))return(0);

	for(i=0; i<facea->nvecs; i++)
	{
		Hull_WorldNormal(hulla, facea->evecs+i*4, dir);
		Hull_ProjectHullFace(hullb, faceb, dir, &m, &n);
		if(m>dir[3])return(0);
	}

	for(i=0; i<faceb->nvecs; i++)
	{
		Hull_WorldNormal(hullb, faceb->evecs+i*4, dir);
		Hull_ProjectHullFace(hulla, facea, dir, &m, &n);
		if(m>dir[3])return(0);
	}

	return(1);
}

int Hull_CollideHull(Solid_Hull *hulla, Solid_Hull *hullb)
{
	static float pts0[16*4], pts1[16*4];
	Hull_Face *cur, *cur2, *bf;
	float dir[4], bn[4], bo[3];
	float m, n, bd;
	int i, j;

	V3F_ZERO(bn);
	bd=999999;
	bf=NULL;

	cur=hulla->face;
	while(cur)
	{
		Hull_WorldNormal(hulla, cur->norm, dir);
		Hull_ProjectHull(hullb, dir, &m, &n);
		if(m>dir[3])return(0);

		if((dir[3]-m)<bd)
		{
			V4F_COPY(dir, bn);
			bd=dir[3]-m;
		}

		cur=cur->next;
	}

	cur=hullb->face;
	while(cur)
	{
		Hull_WorldNormal(hullb, cur->norm, dir);
		Hull_ProjectHull(hulla, dir, &m, &n);
		if(m>dir[3])return(0);

		if((dir[3]-m)<bd)
		{
			V4F_SCALE(dir, -1, bn);
			bd=dir[3]-m;
		}

		cur=cur->next;
	}


	Hull_MakePlaneFace(bn, pts0);
	j=4;

	cur=hulla->face;
	while(j && cur)
	{
		Hull_WorldNormal(hulla, cur->norm, dir);
		j=Hull_ClipFace(dir, pts0, pts1, j);
		for(i=0; i<(j*3); i++)
			pts0[i]=pts1[i];
		cur=cur->next;
	}

	cur=hullb->face;
	while(j && cur)
	{
		Hull_WorldNormal(hullb, cur->norm, dir);
		j=Hull_ClipFace(dir, pts0, pts1, j);
		for(i=0; i<(j*3); i++)
			pts0[i]=pts1[i];
		cur=cur->next;
	}


	glDisable(GL_DEPTH_TEST);
	glColor4f(1, 0, 0, 0.5);
	glBegin(GL_POLYGON);
	for(i=0; i<j; i++)
		glVertex3fv(pts0+i*3);
	glEnd();
	glEnable(GL_DEPTH_TEST);


	V3F_SCALE(bn, -bd, dir);
	for(i=0; i<j; i++)
		Hull_ApplyPointForceHull(hulla, pts0+i*3, dir);

	V3F_SCALE(bn, bd, dir);
	for(i=0; i<j; i++)
		Hull_ApplyPointForceHull(hullb, pts0+i*3, dir);

//	printf("collide\n");
//	fflush(stdout);

	V3F_SCALE(bn, bn[3], bo);

	glDisable(GL_DEPTH_TEST);

	glBegin(GL_LINES);
	glColor4f(1, 0, 0, 1);
	glVertex3f(bo[0],	bo[1],		bo[2]);
	glColor4f(0, 1, 0, 1);
	glVertex3f(bo[0]+bn[0],	bo[1]+bn[1],	bo[2]+bn[2]);
	glEnd();

	glEnable(GL_DEPTH_TEST);
}


void Hull_HullPlaneIntersect(Solid_Hull *hull, float *ndir,
	float *rorg)
{
	Hull_Face *cur;

	float dir[3], org[3];
	float d;
	int i, j;

	dir[0]=V3F_DOT(ndir, hull->rot+0);
	dir[1]=V3F_DOT(ndir, hull->rot+3);
	dir[2]=V3F_DOT(ndir, hull->rot+6);
	dir[3]=ndir[3]-V3F_DOT(hull->org, ndir);

	V3F_ZERO(org);
	j=0;

	cur=hull->face;
	while(cur)
	{
		for(i=0; i<cur->nvecs; i++)
		{
			d=V3F_DOT(cur->vecs+i*3, dir);
			if(d>=dir[3])continue;
			V3F_ADD(org, cur->vecs+i*3, org);
			j++;
		}

		cur=cur->next;
	}

	if(!j)j++;
	V3F_SCALE(org, 1.0/j, org);

	rorg[0]=hull->rot[0]*org[0] + hull->rot[3]*org[1] +
		hull->rot[6]*org[2] + hull->org[0];
	rorg[1]=hull->rot[1]*org[0] + hull->rot[4]*org[1] +
		hull->rot[7]*org[2] + hull->org[1];
	rorg[2]=hull->rot[2]*org[0] + hull->rot[5]*org[1] +
		hull->rot[8]*org[2] + hull->org[2];
}

int Hull_CollideHullPlane(Solid_Hull *hull, float *norm)
{
	float m, n, d, f;
	float bo[3], bv[3];

	Hull_ProjectHull(hull, norm, &m, &n);
	if(m>=norm[3])return(0);

	d=norm[3]-m;
	Hull_HullPlaneIntersect(hull, norm, bo);

	f=V3F_DOT(hull->vel, norm);
	V3F_SCALE(norm, d-f, bv);
	Hull_ApplyPointForceHull(hull, bo, bv);

	return(1);
}
