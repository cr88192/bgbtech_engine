#include "bsde_internal.h"

//Cylinders
//Issue: may have issues in some cases, eg, long/narrow cylinders, or
//parallel+offset cylinders

void BSDE_SolidCylinder_MakeBox(bsde_real height, bsde_real radius, int t,
	bsde_real *pos, bsde_real *mins, bsde_real *maxs)
{
	bsde_real f;
	int i;

	for(i=0; i<3; i++)
	{
		if(t)
		{
			f=fabs(pos[2*4+i]*height*0.5)+radius;
			mins[i]=pos[3*4+i]-f;
			maxs[i]=pos[3*4+i]+f;
		}else
		{
			f=	fabs(pos[0*4+i]*radius)+
				fabs(pos[1*4+i]*radius)+
				fabs(pos[2*4+i]*height*0.5);
			mins[i]=pos[3*4+i]-f;
			maxs[i]=pos[3*4+i]+f;
		}
	}
}

void BSDE_SolidCylinder_BoxMove(
	bsde_real height, bsde_real radius, int t,
	bsde_real *spos, bsde_real *epos,
	bsde_real *mins, bsde_real *maxs)
{
	bsde_real amins[3], amaxs[3];
	bsde_real bmins[3], bmaxs[3];
	int i;

	BSDE_SolidCylinder_MakeBox(height, radius, t, spos, amins, amaxs);
	BSDE_SolidCylinder_MakeBox(height, radius, t, epos, bmins, bmaxs);
	for(i=0; i<3; i++)mins[i]=(amins[i]<bmins[i])?amins[i]:bmins[i];
	for(i=0; i<3; i++)maxs[i]=(amaxs[i]>bmaxs[i])?amaxs[i]:bmaxs[i];
}

#if 0
int BSDE_SolidCylinder_CollideObbContact(
	bsde_real alen, bsde_real arad, int t, bsde_real *apos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bpos,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	bsde_real pt[3], dv[3], aorg[3];
	bsde_real d, r;

	aorg[0]=apos[3];
	aorg[1]=apos[7];
	aorg[2]=apos[11];

	BSDE_SolidOBB_ObbNearestPoint(bmins, bmaxs, bpos, aorg, pt);
	V3_SUB(aorg, pt, dv);
	d=V3_NORMALIZE(dv, dv);

	if(t)
	{
		r=arad +fabs(V3_DOT(apos+(2*4), dv)*alen*0.5);
	}else
	{
		r=	fabs(V3_DOT(apos+(0*4), dv)*arad)+
			fabs(V3_DOT(apos+(1*4), dv)*arad)+
			fabs(V3_DOT(apos+(2*4), dv)*alen*0.5);
	}

	if(d>r)return(0);

	if(org)
	{
		V3_COPY(pt, org);
		V3_COPY(dv, norm);
		*dist=r-d;
	}

	return(1);
}
#endif

bsde_real BSDE_SolidCylinder_AxisRadius(
	bsde_real len, bsde_real rad, int t, bsde_real *pos, bsde_real *norm)
{
	bsde_real r;

	if(t)
	{
		r=rad +	fabs(V3_DOT(pos+(2*4), norm)*len*0.5);
	}else
	{
		r=	fabs(V3_DOT(pos+(0*4), norm)*rad) +
			fabs(V3_DOT(pos+(1*4), norm)*rad) +
			fabs(V3_DOT(pos+(2*4), norm)*len*0.5);
	}
	return(r);
}

void BSDE_SolidCylinder_PointLocal(bsde_real *pos, bsde_real *org, bsde_real *pt)
{
	bsde_real v[3];

	V3_SUB(org, pos+3*4, v);
	pt[0]=V3_DOT(pos+0*4, v);
	pt[1]=V3_DOT(pos+1*4, v);
	pt[2]=V3_DOT(pos+2*4, v);
}

void BSDE_SolidCylinder_NearestPoint(
	bsde_real len, bsde_real rad, int t, bsde_real *pos,
	bsde_real *point, bsde_real *pt)
{
	bsde_real v[3], v1[3], f, g;

	V3_SUB(point, pos+3*4, v1);
	v[0]=V3_DOT(pos+0*4, v1);
	v[1]=V3_DOT(pos+1*4, v1);
	v[2]=V3_DOT(pos+2*4, v1);

	f=V2_LEN(v);
	if(f>rad)
	{
		V2_SCALE(v, rad/f, v);
	}

	if(t)
	{
		f=fabs(v[2])-(len*0.5);
		if(f>0)
		{
			g=sqrt(v[0]*v[0]+v[1]*v[1]+f*f);
			if(g>rad)
			{
				V2_COPY(v, v1);
				v1[2]=(v[2]>0)?f:-f;

				V2_SCALE(v1, rad/g, v);
			}
		}
	}else
	{
		if(v[2]>(len*0.5))v[2]=len*0.5;
		if(v[2]<(-(len*0.5)))v[2]=-len*0.5;
	}

	pt[0]=v[0]*pos[0]+v[1]*pos[4]+v[2]*pos[8]+pos[12];
	pt[1]=v[0]*pos[1]+v[1]*pos[5]+v[2]*pos[9]+pos[13];
	pt[2]=v[0]*pos[2]+v[1]*pos[6]+v[2]*pos[10]+pos[14];
}

int BSDE_SolidCylinder_PointCollide(
	bsde_real len, bsde_real rad, int t, bsde_real *pos,
	bsde_real *point)
{
	bsde_real v[3], pt[3], f, g;

	V3_SUB(point, pos+3*4, pt);
	v[0]=V3_DOT(pos+0*4, pt);
	v[1]=V3_DOT(pos+1*4, pt);
	v[2]=V3_DOT(pos+2*4, pt);

	if(V2_LEN(v)>=rad)return(0);

	if(t)
	{
		if(fabs(v[2])>=(len*0.5+rad))return(0);

		f=fabs(v[2])-(len*0.5);
		if(f<=0)return(1);
		g=sqrt(v[0]*v[0]+v[1]*v[1]+f*f);
		if(g>=rad)return(0);
	}else
	{
		if(fabs(v[2])>=(len*0.5))return(0);
	}
	return(1);
}

void BSDE_SolidCylinder_OffsetCPA(
	bsde_real *ua, bsde_real *va, bsde_real *ub, bsde_real *vb,
	bsde_real *da, bsde_real *db)
{
	bsde_real dp[3];
	bsde_real vx, q0, q1, d;

	V3_SUB(ub, ua, dp);
	q0= V3_DOT(dp, va);
	q1=-V3_DOT(dp, vb);

	vx=V3_DOT(va, vb);
	d=1-vx*vx;

	if(d<=0.0001)
	{
//		*da=0;
//		*db=0;

		*da=V3_DOT(ub, va)-V3_DOT(ua, va);
		*db=V3_DOT(ua, vb)-V3_DOT(ub, vb);
		return;
	}

	d=1.0/d;
	*da=(q0+vx*q1)*d;
	*db=(vx*q0+q1)*d;
}

int BSDE_SolidCylinder_LineCollide(
	bsde_real len, bsde_real rad, int t, bsde_real *pos,
	bsde_real *sp, bsde_real *ep)
{
	bsde_real dv[3], v[3], l, r, d;
	bsde_real ra, rb;

	V3_SUB(ep, sp, dv);
	l=V3_NORMALIZE(dv, dv);

	BSDE_SolidCylinder_OffsetCPA(pos+3*4, pos+2*4, sp, dv,
		&ra, &rb);

	if(ra>(len*0.5))ra=len*0.5;
	if(ra<(-len*0.5))ra=-len*0.5;
	if(rb>l)rb=l;
	if(rb<0)rb=0;

	v[0]=(pos[2*4+0]*ra)-(dv[0]*rb)+(pos[3*4+0]-sp[0]);
	v[1]=(pos[2*4+1]*ra)-(dv[1]*rb)+(pos[3*4+1]-sp[1]);
	v[2]=(pos[2*4+2]*ra)-(dv[2]*rb)+(pos[3*4+2]-sp[2]);
	d=V3_NORMALIZE(v, v);

	if(!t)	r=fabs(V3_DOT(pos+0*4, v)*rad)+
			fabs(V3_DOT(pos+1*4, v)*rad);
		else r=rad;
	if(d>=r)return(0);

	return(1);
}


int BSDE_SolidCylinder_CollideContact(
	bsde_real alen, bsde_real arad, int at, bsde_real *apos,
	bsde_real blen, bsde_real brad, int bt, bsde_real *bpos,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	bsde_real dv[3], aorg[3], borg[3];
	bsde_real d, f, g, ra, rb;

	V3_COPY(apos+3*4, aorg);
	V3_COPY(bpos+3*4, borg);

	f=V3_DOT(apos+2*4, bpos+2*4);
	if(fabs(f)>0.9)
	{
		//ok, cylinders are near parallel
		V3_COPY(apos+2*4, dv);
		ra=V3_DOT(aorg, dv);
		rb=V3_DOT(borg, dv);
		if(rb>ra)
		{
			V3_SCALE(dv, -1, dv);
			ra=V3_DOT(aorg, dv);
			rb=V3_DOT(borg, dv);
		}

		f=-alen*0.5;
		g=blen*0.5;

		dv[0]=(dv[0]*f)-(dv[0]*g)+(aorg[0]-borg[0]);
		dv[1]=(dv[1]*f)-(dv[1]*g)+(aorg[1]-borg[1]);
		dv[2]=(dv[2]*f)-(dv[2]*g)+(aorg[2]-borg[2]);
		d=V3_NORMALIZE(dv, dv);
	}else
	{
		BSDE_SolidCylinder_OffsetCPA(aorg, apos+2*4, borg, bpos+2*4,
			&ra, &rb);
		if(ra>(alen*0.5))ra=alen*0.5;
		if(ra<(-alen*0.5))ra=-alen*0.5;
		if(rb>(blen*0.5))rb=blen*0.5;
		if(rb<(-blen*0.5))rb=-blen*0.5;

		dv[0]=(apos[2*4+0]*ra)-(bpos[2*4+0]*rb)+(aorg[0]-borg[0]);
		dv[1]=(apos[2*4+1]*ra)-(bpos[2*4+1]*rb)+(aorg[1]-borg[1]);
		dv[2]=(apos[2*4+2]*ra)-(bpos[2*4+2]*rb)+(aorg[2]-borg[2]);
		d=V3_NORMALIZE(dv, dv);
	}

	if(at)ra=arad;
		else ra=fabs(V3_DOT(apos+0*4, dv)*arad)+
			fabs(V3_DOT(apos+1*4, dv)*arad);
	if(bt)rb=brad;
		else rb=fabs(V3_DOT(bpos+0*4, dv)*brad)+
			fabs(V3_DOT(bpos+1*4, dv)*brad);
	if(d>=(ra+rb))return(0);

	if(org)
	{
		f=rb+(d-ra);
		V3_ADDSCALE(borg, dv, f, org);

		V3_COPY(dv, norm);
		*dist=(ra+rb)-d;
	}

	return(1);
}

int BSDE_SolidCylinder_CollideObbContact(
	bsde_real alen, bsde_real arad, int ty, bsde_real *apos,
	bsde_real *bmins, bsde_real *bmaxs, bsde_real *bpos,
	bsde_real *org, bsde_real *norm, bsde_real *dist)
{
	bsde_real aorg[3];
	bsde_real pt[3], dv[3];
	bsde_real ext[4];
	bsde_real bn[3];
	bsde_real d0, d1, d, r, bd;
	int i, j;

	aorg[0]=apos[12]; aorg[1]=apos[13]; aorg[2]=apos[14];

	BSDE_SolidOBB_ObbNearestPoint(bmins, bmaxs, bpos, aorg, pt);
	V3_SUB(aorg, pt, dv);
	d=V3_NORMALIZE(dv, dv);
	if(d<0.0001)
	{
		dv[2]=1;
		pt[2]-=arad;
	}

	r=BSDE_SolidCylinder_AxisRadius(alen, arad, ty, apos, dv);
	if(d>r)return(0);

	V3_COPY(dv, bn);
	bd=r-d;

	for(i=0; i<3; i++)
	{
		r=BSDE_SolidCylinder_AxisRadius(alen, arad, ty, apos,
			bpos+i*4);
		ext[0]=V3_DOT(aorg, bpos+i*4)-r;
		ext[1]=V3_DOT(aorg, bpos+i*4)+r;

		BSDE_SolidOBB_ObbPlaneExtents(bmins, bmaxs, bpos,
			bpos+i*4, ext+2);
		d0=(ext[0]>ext[2])?ext[0]:ext[2];
		d1=(ext[1]<ext[3])?ext[1]:ext[3];

		d=d1-d0;
		if(d<=0)return(0);

		if(d<bd)
		{
			V3_COPY(bpos+i*4, bn);
			bd=d;
		}
	}

	if(org)
	{
		V3_COPY(pt, org);
		V3_NORMALIZE(bn, bn);
		V3_COPY(bn, norm);
		*dist=bd;
	}
	return(1);
}
