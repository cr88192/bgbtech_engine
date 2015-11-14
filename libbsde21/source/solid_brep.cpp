#include "bsde_internal.h"


//BREP

void BSDE_SolidBRep_ProjectTriangleLine(
	bsde_real *u0, bsde_real *u1, bsde_real *u2,
	bsde_real *iv, bsde_real *rm, bsde_real *rn)
{
	bsde_real d0, d1, d2;
	bsde_real m0, n0;

	d0=V3_DOT(iv, u0);
	d1=V3_DOT(iv, u1);
	d2=V3_DOT(iv, u2);
	m0=(d0<d1)?((d0<d2)?d0:d2):((d1<d2)?d1:d2);
	n0=(d0>d1)?((d0>d2)?d0:d2):((d1>d2)?d1:d2);

	*rm=m0;
	*rn=n0;
}

//check if 2 triangles overlap on a particular line
int BSDE_SolidBRep_CheckLineTriangleTriangle(
	bsde_real *u0, bsde_real *u1, bsde_real *u2,
	bsde_real *v0, bsde_real *v1, bsde_real *v2,
	bsde_real *iv)
{
	bsde_real d0, d1, d2;
	bsde_real m0, n0, m1, n1;

	d0=V3_DOT(iv, u0);
	d1=V3_DOT(iv, u1);
	d2=V3_DOT(iv, u2);
	m0=(d0<d1)?((d0<d2)?d0:d2):((d1<d2)?d1:d2);
	n0=(d0>d1)?((d0>d2)?d0:d2):((d1>d2)?d1:d2);

	d0=V3_DOT(iv, v0);
	d1=V3_DOT(iv, v1);
	d2=V3_DOT(iv, v2);
	m1=(d0<d1)?((d0<d2)?d0:d2):((d1<d2)?d1:d2);
	n1=(d0>d1)?((d0>d2)?d0:d2):((d1>d2)?d1:d2);

	if((m0>n1) || (m1>n0))
		return(0);
	return(1);
}

//check if 2 triangles overlap on a particular line
//adjusts pt to fall within the space
int BSDE_SolidBRep_CheckLineTriangleTriangle2(
	bsde_real *u0, bsde_real *u1, bsde_real *u2,
	bsde_real *v0, bsde_real *v1, bsde_real *v2,
	bsde_real *iv, bsde_real *pt)
{
	bsde_real d0, d1, d2;
	bsde_real m0, n0, m1, n1, m2, n2;

	d0=V3_DOT(iv, u0);
	d1=V3_DOT(iv, u1);
	d2=V3_DOT(iv, u2);
	m0=(d0<d1)?((d0<d2)?d0:d2):((d1<d2)?d1:d2);
	n0=(d0>d1)?((d0>d2)?d0:d2):((d1>d2)?d1:d2);

	d0=V3_DOT(iv, v0);
	d1=V3_DOT(iv, v1);
	d2=V3_DOT(iv, v2);
	m1=(d0<d1)?((d0<d2)?d0:d2):((d1<d2)?d1:d2);
	n1=(d0>d1)?((d0>d2)?d0:d2):((d1>d2)?d1:d2);

	if((m0>n1) || (m1>n0))
		return(0);

	m2=(m0>m1)?m0:m1;
	n2=(n0<n1)?n0:n1;

	d0=V3_DOT(iv, pt);
	if(d0<m2)V3_ADDSCALE(pt, iv, m2-d0, pt);
	if(d0>n2)V3_ADDSCALE(pt, iv, n2-d0, pt);

	return(1);
}


//check if 2 triangles overlap
int BSDE_SolidBRep_CheckTriangleTriangle(
	bsde_real *u0, bsde_real *u1, bsde_real *u2,
	bsde_real *v0, bsde_real *v1, bsde_real *v2)
{
	bsde_real du0[3], du1[3], du2[4], du3[3];
	bsde_real dv0[3], dv1[3], dv2[4], dv3[3];
	bsde_real iv[3];
	bsde_real d0, d1, d2, f;
	int i;


#if 1
	//check: v does not cross u

	V3_SUB(u1, u0, du0);
	V3_SUB(u2, u0, du1);
	V3_CROSS(du0, du1, du2);
	du2[3]=V3_DOT(du2, u0);

	d0=V3_DOT(du2, v0)-du2[3];
	d1=V3_DOT(du2, v1)-du2[3];
	d2=V3_DOT(du2, v2)-du2[3];

	if(((d0*d1)>0) && ((d0*d2)>0))
		return(0);
#endif

#if 1
	//check: u does not cross v

	V3_SUB(v1, v0, dv0);
	V3_SUB(v2, v0, dv1);
	V3_CROSS(dv0, dv1, dv2);
	dv2[3]=V3_DOT(dv2, v0);

	d0=V3_DOT(dv2, u0)-dv2[3];
	d1=V3_DOT(dv2, u1)-dv2[3];
	d2=V3_DOT(dv2, u2)-dv2[3];

	if(((d0*d1)>0) && ((d0*d2)>0))
		return(0);
#endif

#if 1
	//joint overlap

	V3_CROSS(du2, dv2, iv);
	f=V3_NORMALIZE(iv, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);
#endif

#if 1
	//check sides
	V3_CROSS(du2, du0, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);

	V3_CROSS(du2, du1, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);

#if 1
	V3_SUB(u2, u1, du3);
	V3_CROSS(du2, du3, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);
#endif
#endif

#if 1
	V3_CROSS(dv2, dv0, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);

	V3_CROSS(dv2, dv1, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);


#if 1
	//finally, check the C side
	V3_SUB(v2, v1, dv3);
	V3_CROSS(dv2, dv3, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);
#endif
#endif

	return(1);
}

//check if 2 triangles overlap also while computing intersections
int BSDE_SolidBRep_CheckTriangleTriangle2(
	bsde_real *u0, bsde_real *u1, bsde_real *u2,
	bsde_real *v0, bsde_real *v1, bsde_real *v2,
	bsde_real *iorg, bsde_real *idir)
{
	bsde_real du0[3], du1[3], du2[4], du3[3];
	bsde_real dv0[3], dv1[3], dv2[4], dv3[3];
	bsde_real iv[3], pt[3];
	bsde_real d0, d1, d2;
	bsde_real f;
	int i;

	V3_ZERO(pt);

#if 1
	//check: v does not cross u

	V3_SUB(u1, u0, du0);
	V3_SUB(u2, u0, du1);
	V3_CROSS(du0, du1, du2);
	du2[3]=V3_DOT(du2, u0);

	d0=V3_DOT(du2, v0)-du2[3];
	d1=V3_DOT(du2, v1)-du2[3];
	d2=V3_DOT(du2, v2)-du2[3];

	if(((d0*d1)>0) && ((d0*d2)>0))
		return(0);

	f=V3_DOT(pt, du2)-du2[3];
	V3_ADDSCALE(pt, du2, -f, pt);
#endif

#if 1
	//check: u does not cross v

	V3_SUB(v1, v0, dv0);
	V3_SUB(v2, v0, dv1);
	V3_CROSS(dv0, dv1, dv2);
	dv2[3]=V3_DOT(dv2, v0);

	d0=V3_DOT(dv2, u0)-dv2[3];
	d1=V3_DOT(dv2, u1)-dv2[3];
	d2=V3_DOT(dv2, u2)-dv2[3];

	if(((d0*d1)>0) && ((d0*d2)>0))
		return(0);

	f=V3_DOT(pt, dv2)-dv2[3];
	V3_ADDSCALE(pt, dv2, -f, pt);
#endif

#if 1
	//joint overlap

	V3_CROSS(du2, dv2, iv);
	f=V3_NORMALIZE(iv, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle(
		u0, u1, u2, v0, v1, v2, iv);
	if(!i)return(0);
#endif

#if 1
	//check sides
	V3_CROSS(du2, du0, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle2(
		u0, u1, u2, v0, v1, v2, iv, pt);
	if(!i)return(0);

	V3_CROSS(du2, du1, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle2(
		u0, u1, u2, v0, v1, v2, iv, pt);
	if(!i)return(0);

#if 1
	V3_SUB(u2, u1, du3);
	V3_CROSS(du2, du3, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle2(
		u0, u1, u2, v0, v1, v2, iv, pt);
	if(!i)return(0);
#endif
#endif

#if 1
	V3_CROSS(dv2, dv0, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle2(
		u0, u1, u2, v0, v1, v2, iv, pt);
	if(!i)return(0);

	V3_CROSS(dv2, dv1, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle2(
		u0, u1, u2, v0, v1, v2, iv, pt);
	if(!i)return(0);


#if 1
	//finally, check the C side
	V3_SUB(v2, v1, dv3);
	V3_CROSS(dv2, dv3, iv);
	i=BSDE_SolidBRep_CheckLineTriangleTriangle2(
		u0, u1, u2, v0, v1, v2, iv, pt);
	if(!i)return(0);
#endif
#endif

	V3_COPY(pt, iorg);

	return(1);
}

int BSDE_SolidBRep_GetTriangleNormal(
	bsde_real *u0, bsde_real *u1, bsde_real *u2,
	bsde_real *io)
{
	bsde_real du0[3], du1[3];

	V3_SUB(u1, u0, du0);
	V3_SUB(u2, u0, du1);
	V3_CROSS(du0, du1, io);

	return(0);
}

#if 0
int BSDE_SolidBRep_GetTriangleIntersection(
	bsde_real *u0, bsde_real *u1, bsde_real *u2,
	bsde_real *v0, bsde_real *v1, bsde_real *v2,
	bsde_real *io)
{
	bsde_real du0[3], du1[3], du2[4];
	bsde_real dv0[3], dv1[3], dv2[4];
	bsde_real pt[3];
	bsde_real f;
	int i;

	V3_ZERO(pt);

	V3_SUB(u1, u0, du0);
	V3_SUB(u2, u0, du1);
	V3_CROSS(du0, du1, du2);
	V3_Normalize(du2, du2);
	du2[3]=V3_DOT(du2, u0);

	f=V3_DOT(pt, du2)-du2[3];
	VecNF_ADDSCALE(pt, du2, -f, pt, 3);

	V3_SUB(v1, v0, dv0);
	V3_SUB(v2, v0, dv1);
	V3_CROSS(dv0, dv1, dv2);
	V3_Normalize(dv2, dv2);
	dv2[3]=V3_DOT(dv2, v0);

	f=V3_DOT(pt, dv2)-dv2[3];
	VecNF_ADDSCALE(pt, dv2, -f, pt, 3);

	return(0);
}

#endif

//check 2 transformed brep solids
//av: vertices, at: triangles, na: count
int BSDE_SolidBRep_CheckBrep(
	bsde_real *av, int *at, int na,
	bsde_real *bv, int *bt, int nb,
	bsde_real *rio, bsde_real *riv)
{
	bsde_real io[3], iv[3];
	bsde_real *u0, *u1, *u2;
	bsde_real *v0, *v1, *v2;
	bsde_real w0[3], w1[3], w2[3];
	bsde_real m0, n0, apd, f;
	int i, j, k, l, li;

	V3_ZERO(io);
	V3_ZERO(iv);
	apd=0;

	l=0;
	for(i=0; i<(na-1); i++)
		for(j=0; j<(nb-1); j++)
	{
		u0=av+(at[i*3+0]*3);
		u1=av+(at[i*3+1]*3);
		u2=av+(at[i*3+2]*3);
		v0=bv+(bt[j*3+0]*3);
		v1=bv+(bt[j*3+1]*3);
		v2=bv+(bt[j*3+2]*3);

		k=BSDE_SolidBRep_CheckTriangleTriangle(
			u0, u1, u2, v0, v1, v2);

		if(k && rio)
		{
			w0[0]=u0[0]+u1[0]+u2[0];
			w0[1]=u0[1]+u1[1]+u2[1];
			w0[2]=u0[2]+u1[2]+u2[2];
			w1[0]=v0[0]+v1[0]+v2[0];
			w1[1]=v0[1]+v1[1]+v2[1];
			w1[2]=v0[2]+v1[2]+v2[2];

			io[0]+=w0[0]+w1[0];
			io[1]+=w0[1]+w1[1];
			io[2]+=w0[2]+w1[2];
		}

		if(k && (rio && riv))
		{
			BSDE_SolidBRep_GetTriangleNormal(u0, u1, u2, w2);
			iv[0]+=w2[0];
			iv[1]+=w2[1];
			iv[2]+=w2[2];

			BSDE_SolidBRep_ProjectTriangleLine(
				v0, v1, v2, w2, &m0, &n0);
			f=-(m0-V3_DOT(u0, w2));
			apd+=f;
		}

		if(k)l++;
	}

	if(rio)
	{
		li=l?l:1;

		rio[0]=io[0]/(6*li);
		rio[1]=io[1]/(6*li);
		rio[2]=io[2]/(6*li);
	}

	if(riv)
	{
		V3_NORMALIZE(iv, riv);
		riv[3]=apd;
	}

	return(l);
}

//check 2 transformed brep solids
//av: vertices, at: triangles, na: count
//rio is the returned collision points, padded to 4 bsde_reals
//riv is the returned collision normals and penetration distances
//rn is the limit for the number of returned points
//returns the number of collision points
int BSDE_SolidBRep_CheckBrep2(
	bsde_real *av, int *at, int na,
	bsde_real *bv, int *bt, int nb,
	bsde_real *rio, bsde_real *riv, int rn)
{
	bsde_real io[3], iv[3];
	bsde_real *u0, *u1, *u2;
	bsde_real *v0, *v1, *v2;
	bsde_real w0[3], w1[3], w2[3];
	bsde_real m0, n0, apd, f;
	int i, j, k, l, li;

	V3_ZERO(io);
	V3_ZERO(iv);
	apd=0;

	l=0;
	for(i=0; i<(na-1); i++)
		for(j=0; j<(nb-1); j++)
	{
		u0=av+(at[i*3+0]*3);
		u1=av+(at[i*3+1]*3);
		u2=av+(at[i*3+2]*3);
		v0=bv+(bt[j*3+0]*3);
		v1=bv+(bt[j*3+1]*3);
		v2=bv+(bt[j*3+2]*3);

		k=BSDE_SolidBRep_CheckTriangleTriangle(
			u0, u1, u2, v0, v1, v2);

		if(k && rio)
		{
			w0[0]=u0[0]+u1[0]+u2[0];
			w0[1]=u0[1]+u1[1]+u2[1];
			w0[2]=u0[2]+u1[2]+u2[2];
			w1[0]=v0[0]+v1[0]+v2[0];
			w1[1]=v0[1]+v1[1]+v2[1];
			w1[2]=v0[2]+v1[2]+v2[2];

			rio[0]=(w0[0]+w1[0])/6;
			rio[1]=(w0[1]+w1[1])/6;
			rio[2]=(w0[2]+w1[2])/6;
			rio[3]=0;
			rio+=4;

			if(riv)
			{
				BSDE_SolidBRep_GetTriangleNormal(u0, u1, u2, w2);
				riv[0]=w2[0];
				riv[1]=w2[1];
				riv[2]=w2[2];

				BSDE_SolidBRep_ProjectTriangleLine(
					v0, v1, v2, w2, &m0, &n0);
				f=-(m0-V3_DOT(u0, w2));
				riv[3]=f;

				riv+=4;
			}
		}

		if(k)l++;
	}

	if(rio)
	{
		li=l?l:1;

		rio[0]=io[0]/(6*li);
		rio[1]=io[1]/(6*li);
		rio[2]=io[2]/(6*li);
	}

	if(riv)
	{
		V3_NORMALIZE(iv, riv);
		riv[3]=apd;
	}

	return(l);
}

int BSDE_SolidBRep_TrimBrep(bsde_real *av, int *at, int na,
	int *at2, bsde_real *bmins, bsde_real *bmaxs)
{
	bsde_real m[3], n[3];
	bsde_real *v0, *v1, *v2;
	int i, j, k;

	j=0;
	for(i=0; i<na; i++)
	{
		v0=av+(at[i*3+0]*3);
		v1=av+(at[i*3+1]*3);
		v2=av+(at[i*3+2]*3);

		for(k=0; k<3; k++)
		{
			m[k]=(v0[k]<v1[k])?
				((v0[k]<v2[k])?v0[k]:v2[k]):
				((v1[k]<v2[k])?v1[k]:v2[k]);
			n[k]=(v0[k]>v1[k])?
				((v0[k]>v2[k])?v0[k]:v2[k]):
				((v1[k]>v2[k])?v1[k]:v2[k]);
		}

		if(BSDE_SolidAABB_BoxCollideP(m, n, bmins, bmaxs))
		{
			at2[j*3+0]=at[i*3+0];
			at2[j*3+1]=at[i*3+1];
			at2[j*3+2]=at[i*3+2];
			j++;
		}
	}
	return(j);
}


void BSDE_SolidBRep_ProjectLine(
	bsde_real *vecs, int *tris, int nv, int nt,
	bsde_real *pos, bsde_real *norm, bsde_real *rmin, bsde_real *rmax)
{
	bsde_real tv[3], m, n, f;
	int i;

	BSDE_Plane_TransformVectorT(norm, pos, tv);

	m=BSDE_M_BIGVAL;
	n=-BSDE_M_BIGVAL;
	for(i=0; i<nv; i++)
	{
//		BSDE_Plane_TransformPoint(vecs+i*3, pos, tv);
//		f=V3_DOT(tv, norm);

		f=V3_DOT(vecs+i*3, tv);
		if(f<m)m=f;
		if(f>n)n=f;
	}

	f=V3_DOT(pos+3*4, norm);
	*rmin=m+f;
	*rmax=n+f;
}

void BSDE_SolidBRep_ProjectVecsLine(
	bsde_real *vecs, int nv,
	bsde_real *norm, bsde_real *rmin, bsde_real *rmax)
{
	bsde_real tv[3], m, n, f;
	int i;

	m=BSDE_M_BIGVAL; n=-BSDE_M_BIGVAL;
	for(i=0; i<nv; i++)
	{
		f=V3_DOT(vecs+i*3, norm);
		if(f<m)m=f;
		if(f>n)n=f;
	}

	*rmin=m; *rmax=n;
}

void BSDE_SolidBRep_ProjectLine2(BSDE_Solid *obj, bsde_real *pos,
	bsde_real *norm, bsde_real *rmin, bsde_real *rmax)
{
	//use predicted info if possible
	if((obj->moveflags&BSDE_MOVEFL_PREDICT) &&
		BSDE_Mat4_Equal(pos, obj->e_xform))
	{
		BSDE_SolidBRep_ProjectVecsLine(
			obj->e_vecs, obj->n_vecs, norm, rmin, rmax);
		return;
	}

	BSDE_SolidBRep_ProjectLine(obj->vecs, obj->faces,
		obj->n_vecs, obj->n_faces, pos, norm, rmin, rmax);
}

//predict line in ending pos
//use cached values if available
void BSDE_SolidBRep_ProjectLine3(BSDE_Solid *obj,
	bsde_real *norm, bsde_real *rmin, bsde_real *rmax)
{
	int i;

	if(obj->moveflags&BSDE_MOVEFL_PREDICT)
	{
		BSDE_SolidBRep_ProjectVecsLine(
			obj->e_vecs, obj->n_vecs, norm, rmin, rmax);
		return;
	}

	BSDE_SolidBRep_ProjectLine(obj->vecs, obj->faces,
		obj->n_vecs, obj->n_faces, obj->e_xform, norm, rmin, rmax);
}

//use a few teqniques to attempt to find an approximate for the volume
//this function will assume a "solid" mesh
bsde_real BSDE_SolidBRep_ApproxBrepVolume(bsde_real *vecs, int *tris, int nv, int nt)
{
	bsde_real mins[3], maxs[3], org[3], org1[3];
	bsde_real mins2[3], maxs2[3];
	bsde_real tv[3], iv0[3], iv1[3], iv2[3];
	bsde_real sr0, sr1, f, vol;
	int i;

	//AABB
	V3_ZERO(org);
	V3_INITBOX(mins, maxs);
	for(i=0; i<nv; i++)
	{
		V3_COPY(vecs+i*3, tv);
		V3_ADD(org, tv, org);
		V3_MIN(mins, tv, mins);
		V3_MAX(maxs, tv, maxs);
	}
	V3_SCALE(org, 1.0/nv, org);
	vol=(maxs[0]-mins[0])*(maxs[1]-mins[1])*(maxs[2]-mins[2]);

	org1[0]=(mins[0]+maxs[0])*0.5;
	org1[1]=(mins[1]+maxs[1])*0.5;
	org1[2]=(mins[2]+maxs[2])*0.5;

	//Spheres
	sr0=0; sr1=0;
	for(i=0; i<nv; i++)
	{
		//from est COM
		f=V3_DIST(vecs+i*3, org);
		if(f>sr0)sr0=f;

		//from est center
		f=V3_DIST(vecs+i*3, org1);
		if(f>sr1)sr1=f;
	}

	f=4.0/3.0*BSDE_M_PI*sr0*sr0;
	if(f<vol)vol=f;
	f=4.0/3.0*BSDE_M_PI*sr1*sr1;
	if(f<vol)vol=f;

	//OBB
	V3_ZERO(iv0);
	V3_ZERO(iv1);

	for(i=0; i<nv; i++)
	{
		iv0[0]+=fabs(vecs[i*3+0]-org[0]);
		iv0[1]+=fabs(vecs[i*3+0]-org[1]);
		iv0[2]+=fabs(vecs[i*3+0]-org[2]);
	}
	V3_NORMALIZE(iv0, iv0);

	for(i=0; i<nv; i++)
	{
		V3_SUB(vecs+i*3, org, tv);
		f=V3_DOT(tv, iv0);
		V3_ADDSCALE(tv, iv0, -f, tv);

		iv1[0]+=fabs(tv[0]);
		iv1[1]+=fabs(tv[1]);
		iv1[2]+=fabs(tv[2]);
	}
	V3_NORMALIZE(iv1, iv1);

	V3_CROSS(iv0, iv1, iv2);

	V3_INITBOX(mins2, maxs2);
	for(i=0; i<nv; i++)
	{
		tv[0]=V3_DOT(vecs+i*3, iv0);
		tv[1]=V3_DOT(vecs+i*3, iv1);
		tv[2]=V3_DOT(vecs+i*3, iv2);
		V3_MIN(mins2, tv, mins2);
		V3_MAX(maxs2, tv, maxs2);
	}

	f=(maxs2[0]-mins2[0])*(maxs2[1]-mins2[1])*(maxs2[2]-mins2[2]);
	if(f<vol)vol=f;

	return(vol);
}

void BSDE_SolidBRep_PlaneIntersect(BSDE_Solid *obj, bsde_real *pos,
	bsde_real *norm, bsde_real *io)
{
	bsde_real bo[3], bn[4], pt[3];
	bsde_real d;
	int i, j;

	V3_ZERO(bo);
	j=0;

	BSDE_Plane_TransformNormalT(norm, pos, bn);

	for(i=0; i<obj->n_vecs; i++)
	{
//		BSDE_Plane_TransformPoint(obj->vecs+i*3, pos, pt);
//		d=V3_NDOT(pt, norm);

		V3_COPY(obj->vecs+i*3, pt);
		d=V3_NDOT(pt, bn);

		if(d<0)
		{
			V3_ADD(bo, pt, bo);
			j++;
		}
	}

	if(!j)j++;
	V3_SCALE(bo, 1.0/j, bo);
	BSDE_Plane_TransformPoint(bo, pos, io);
}

void BSDE_SolidBRep_Predict(BSDE_Solid *obj)
{
	int i;

	if(obj->moveflags&BSDE_MOVEFL_PREDICT)
		return;

	if(!obj->e_vecs)obj->e_vecs=
		(bsde_real *)malloc(obj->n_vecs*3*sizeof(bsde_real));
	if(!obj->e_evecs)obj->e_evecs=
		(bsde_real *)malloc(obj->n_faces*12*sizeof(bsde_real));

	for(i=0; i<obj->n_vecs; i++)
		BSDE_Plane_TransformPoint(
			obj->vecs+i*3, obj->e_xform, obj->e_vecs+i*3);

#if 0
	for(i=0; i<(obj->n_faces*3); i++)
		BSDE_Plane_TransformNormal(
			obj->evecs+i*4, obj->e_xform, obj->e_evecs+i*4);
#endif

	obj->moveflags|=BSDE_MOVEFL_PREDICT;
}

int BSDE_SolidBRep_CheckCollide(BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *apos, bsde_real *bpos)
{
	int i, j;

	i=0;
	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		break;

	case BSDE_SOLID_SPHERE:
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		break;

	case BSDE_SOLID_PLANE:
		break;

	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckCollide(bobj, aobj, bpos, apos);
		break;
	case BSDE_SOLID_TRIMESH:
//		i=BSDE_SolidHull_CheckTriMeshCollide(
//			aobj, bobj, axform, bxform);
		break;

	default:
		break;
	}

	return(i);
}

int BSDE_SolidBRep_MultiContact(BSDE_World *world,
	BSDE_Solid *aobj, BSDE_Solid *bobj,
	bsde_real *apos, bsde_real *bpos, BSDE_Contact **lst, int num)
{
	bsde_real io[3], iv[3], id;
	int i, j;

//	printf("brep\n");

	i=BSDE_SolidAABB_BoxCollideP(aobj->mmins, aobj->mmaxs,
		bobj->mmins, bobj->mmaxs);
	if(!i)return(num);

	switch(bobj->solidtype)
	{
	case BSDE_SOLID_AABB:
	case BSDE_SOLID_OBB:
		break;

	case BSDE_SOLID_SPHERE:
		break;

	case BSDE_SOLID_CYLINDER:
	case BSDE_SOLID_CCYLINDER:
		break;

	case BSDE_SOLID_PLANE:
		break;

	case BSDE_SOLID_HULL:
		i=BSDE_SolidHull_CheckTriMeshCollide(bobj, aobj, bpos, apos);
		if(i)
		{
			BSDE_SolidHull_ContactGeneric(bobj, aobj,
				bpos, apos, bpos+3*4, apos+3*4, io, iv, &id);
			lst[num++]=BSDE_MakeContact(world, bobj, aobj, io, iv, id);
		}
		break;
	case BSDE_SOLID_TRIMESH:
//		i=BSDE_SolidHull_CheckTriMeshCollide(
//			aobj, bobj, axform, bxform);
		break;

	default:
		break;
	}

	return(num);
}
