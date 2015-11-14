#include "bsde_internal.h"


void BSDE_SolidHull_MakePlaneFace(bsde_real *norm, bsde_real *pts)
{
	bsde_real v0[3], v1[3], v2[3];

//	BSDE_Plane_AdjacentNormals(norm, v0, v1);
	BSDE_Plane_AdjacentNormals2(norm, v0, v1);

	V3_SCALEADDSCALE(v0, -999, v1, -999, v2);
	V3_ADDSCALE(v2, norm, norm[3], pts+(0*3));
	V3_SCALEADDSCALE(v0,  999, v1, -999, v2);
	V3_ADDSCALE(v2, norm, norm[3], pts+(1*3));
	V3_SCALEADDSCALE(v0,  999, v1,  999, v2);
	V3_ADDSCALE(v2, norm, norm[3], pts+(2*3));
	V3_SCALEADDSCALE(v0, -999, v1,  999, v2);
	V3_ADDSCALE(v2, norm, norm[3], pts+(3*3));
}

int BSDE_SolidHull_ClipFace(bsde_real *norm,
	bsde_real *ipts, bsde_real *opts, int num)
{
	int i, j, k, l;

	//first outside
	for(i=0; i<num; i++)if(V3_NDOT(ipts+(i*3), norm)>0)break;

	if(i==num)	//nothing to clip
	{
		for(i=0; i<num*3; i++)opts[i]=ipts[i];
		return(num);	//nothing to clip
	}

	//first inside
	j=i;
	while(1)
	{
		if(V3_NDOT(ipts+(j*3), norm)<=0)break;
		j=(j+1)%num;
		if(j==i)return(0);	//everything clipped
	}

	//copy inside
	i=j;
	l=0;
	while(1)
	{
		V3_COPY(ipts+(j*3), opts+(l*3));
		l++;

		k=(j+1)%num;
		if(V3_NDOT(ipts+(k*3), norm)>0)break;
		j=k;
	}

	//exit point
	if(V3_NDOT(ipts+(j*3), norm)<0)
	{
		BSDE_Plane_LinePlaneIntersect(
			ipts+(j*3), ipts+(k*3), norm, opts+(l*3));
		l++;
	}

	j=k;
	while(1)
	{
		k=(j+1)%num;
		if(V3_NDOT(ipts+(k*3), norm)<=0)break;
		j=k;
	}

	//entry point
	if(V3_NDOT(ipts+(k*3), norm)<0)
	{
		BSDE_Plane_LinePlaneIntersect(
			ipts+(j*3), ipts+(k*3), norm, opts+(l*3));
		l++;
	}

	return(l);
}

void BSDE_SolidHull_MakeHullGeom(bsde_real *norm, int nnorm,
	bsde_real *rvec, int *rfv, int *rfc,
	int *rnvec, int *rnfv, int *rnfc)
{
	static bsde_real vecbuf[64*64*3];
	static bsde_real pointbuf[64*3], pointbuf2[64*3];
	static int facebuf[64*64];
	static int facevbuf[64];
	int i, j, k, l, n, nf, nv;
	int p0, p1, p2;
	bsde_real *v0, *v1, *v2;
	bsde_real dv0[3], dv1[3], dv2[3];
	bsde_real f, g, h;

	n=0;
	nv=0;
	nf=0;

	for(i=0; i<nnorm; i++)
	{
		BSDE_SolidHull_MakePlaneFace(norm+(i*4), pointbuf);
		l=4;

		for(j=0; j<nnorm; j++)
		{
			if(i==j)continue;

			l=BSDE_SolidHull_ClipFace(norm+(j*4),
				pointbuf, pointbuf2, l);
			for(k=0; k<(l*3); k++)pointbuf[k]=pointbuf2[k];
		}
		if(!l)continue;

//		printf("%d:%d", i, l);

		for(j=0; j<l; j++)
		{
			for(k=0; k<nv; k++)
				if(V3_DIST(pointbuf+j*3,
					vecbuf+k*3)<0.00001)
						break;
			if(k==nv)
			{
				V3_COPY(pointbuf+j*3, vecbuf+k*3);
				nv++;
			}

			facebuf[n++]=k;
		}

		facevbuf[nf]=l;
		nf++;
	}

	if(rvec) { for(i=0; i<(nv*3); i++)rvec[i]=vecbuf[i]; *rnvec=nv; }
	if(rfv) { for(i=0; i<n; i++)rfv[i]=facebuf[i]; *rnfv=nv; }
	if(rfc) { for(i=0; i<nf; i++)rfc[i]=facevbuf[i]; *rnfc=nf; }
}

void BSDE_SolidHull_AlignPoints(bsde_real *vec, int nvec,
	bsde_real *org, bsde_real *vx, bsde_real *vy, bsde_real *vz)
{
	bsde_real vt[3];
	bsde_real f;
	int i;

	//centroid
	V3_ZERO(org);
	for(i=0; i<nvec; i++) { V3_ADD(org, vec+i*3, org); }
	V3_SCALE(org, 1.0/nvec, org);

	//aligned box axii
	V3_ZERO(vx);
	for(i=0; i<nvec; i++)
	{
		vx[0]+=fabs(vec[i*3+0]-org[0]);
		vx[1]+=fabs(vec[i*3+1]-org[1]);
		vx[2]+=fabs(vec[i*3+2]-org[2]);
	}
	f=V3_NORMALIZE(vx, vx);

	if(f<0.01)
	{
		//points are symetric
		vx[0]=1; vx[1]=0; vx[2]=0; vx[3]=org[0];
		vy[0]=0; vy[1]=1; vy[2]=0; vy[3]=org[1];
		vz[0]=0; vz[1]=0; vz[2]=1; vz[3]=org[2];
		return;
	}

	vx[3]=V3_DOT(org, vx);
	
	V3_ZERO(vy);
	for(i=0; i<nvec; i++)
	{
		V3_SUB(vec+i*3, org, vt);
		f=V3_DOT(vt, vx);
		V3_ADDSCALE(vt, vx, -f, vt);
		vx[0]+=fabs(vt[0]);
		vy[1]+=fabs(vt[1]);
		vz[2]+=fabs(vt[2]);
	}
	f=V3_NORMALIZE(vy, vy);

	if(f<0.01)
	{
		V3_ZERO(vy);
		if(fabs(vx[0])>fabs(vx[1]))
		{
			if(fabs(vx[1])>fabs(vx[2]))
				vy[2]=1; else vy[1]=1;
		}else
		{
			if(fabs(vx[0])>fabs(vx[2]))
				vy[2]=1; else vy[0]=1;
		}

		f=V3_DOT(vy, vx);
		V3_ADDSCALE(vy, vx, -f, vy);
		V3_NORMALIZE(vy, vy);
	}

	vy[3]=V3_DOT(org, vy);

	V3_CROSS(vx, vy, vz);
	vz[3]=V3_DOT(org, vz);
}

void BSDE_SolidHull_FitBoxPoints(bsde_real *vec, int nvec,
	bsde_real *mins, bsde_real *maxs, bsde_real *pos)
{
	bsde_real org[3], vx[4], vy[4], vz[4];
	bsde_real f;
	int i;

	BSDE_SolidHull_AlignPoints(vec, nvec, org, vx, vy, vz);
	V3_COPY(vx, pos+0); pos[3]=0;
	V3_COPY(vy, pos+4); pos[7]=0;
	V3_COPY(vz, pos+8); pos[11]=0;
	V3_COPY(org, pos+12); pos[15]=1;


	V3_ZERO(mins);
	V3_ZERO(maxs);
	for(i=0; i<nvec; i++)
	{
		f=V3_NDOT(vec+i*3, vx);
		if(f<mins[0])mins[0]=f;
		if(f>maxs[0])maxs[0]=f;

		f=V3_NDOT(vec+i*3, vy);
		if(f<mins[1])mins[1]=f;
		if(f>maxs[1])maxs[1]=f;

		f=V3_NDOT(vec+i*3, vz);
		if(f<mins[2])mins[2]=f;
		if(f>maxs[2])maxs[2]=f;
	}
}


int BSDE_SolidHull_HullFromPoints(bsde_real *vec, int nvec, bsde_real *nv)
{
	bsde_real vx[3], vy[3], vz[4];
	bsde_real *v0, *v1, *v2;
	bsde_real f;
	int i, j, k, l, nnv;

	nnv=0;
	for(i=0; i<nvec; i++)
	{
		v0=vec+i*3;
		for(j=i+1; j<nvec; j++)
		{
			v1=vec+j*3;
			for(k=j+1; k<nvec; k++)
			{
				v2=vec+k*3;

				V3_SUB(v1, v0, vx);
				V3_SUB(v2, v0, vy);
				V3_CROSS(vx, vy, vz);

				f=V3_NORMALIZE(vz, vz);
				if(fabs(f)<0.01)continue;
				vz[3]=V3_DOT(v0, vz);

				for(l=0; l<nvec; l++)
					if(V3_NDOT(vec+l*3, vz)>0)
						break;
				if(l<nvec)continue;

				for(l=0; l<nnv; i++)
					if(V4_DIST(vz, nv+l*2)<0.1)
						break;
				if(l<nnv)continue;

				l=nnv++;
				V4_COPY(vz, nv+l*4);
			}
		}
	}

	return(nnv);
}

int BSDE_SolidHull_HullFromPointsMask(bsde_real *vec, int base, int lim,
	bsdeUInt *mask, bsde_real *nv)
{
	bsde_real vx[3], vy[3], vz[4];
	bsde_real *v0, *v1, *v2;
	bsde_real f;
	int i, j, k, l, nnv;

	nnv=0;
	for(i=base; i<lim; i++)
	{
		if(!(mask[i>>5]&(1<<(i&31))))
			continue;
		mask[i>>5]&=~(1<<(i&31));

		v0=vec+i*3;
		for(j=i+1; j<lim; j++)
		{
			if(!(mask[j>>5]&(1<<(j&31))))
				continue;

			v1=vec+j*3;
			for(k=j+1; k<lim; k++)
			{
				if(!(mask[k>>5]&(1<<(k&31))))
					continue;

				v2=vec+k*3;

				V3_SUB(v1, v0, vx);
				V3_SUB(v2, v0, vy);
				V3_CROSS(vx, vy, vz);

				f=V3_NORMALIZE(vz, vz);
				if(fabs(f)<0.01)continue;
				vz[3]=V3_DOT(v0, vz);

				for(l=0; l<lim; l++)
					if(V3_NDOT(vec+l*3, vz)>0)
						break;
				if(l<lim)continue;

				l=nnv++;
				V4_COPY(vz, nv+l*4);
				mask[i>>5]|=1<<(i&31);
			}
		}
	}

	return(nnv);
}

int BSDE_SolidHull_HullFiltPoints(bsde_real *vec, int nvec, bsde_real *nv)
{
	bsde_real vx[3], vy[3], vz[4];
	bsde_real *v0, *v1, *v2;
	bsde_real f;
	int i, j, k, l, nnv;

	nnv=0;
	for(i=0; i<nvec; i++)
	{
		v0=vec+i*3;
		for(j=0; j<nvec; j++)
		{
			if(i==j)continue;
			v1=vec+j*3;
			for(k=0; k<nvec; k++)
			{
				if((i==k)||(j==k))continue;
				v2=vec+k*3;

				V3_SUB(v1, v0, vx);
				V3_SUB(v2, v0, vy);
				V3_CROSS(vx, vy, vz);

				f=V3_NORMALIZE(vz, vz);
				if(fabs(f)<0.01)continue;
				vz[3]=V3_DOT(v0, vz);

				for(l=0; l<nvec; l++)
					if(V3_NDOT(vec+l*3, vz)>0)
						break;
				if(l<nvec)continue;

				l=nnv++;
				V3_COPY(v0, nv+l*3);
				break;
			}
			if(k<nvec)break;
		}
	}

	return(nnv);
}

int BSDE_SolidHull_HullFiltPointsR(bsde_real *vec, int cnt, bsde_real *nv)
{
	int i, j, k;

	if(cnt<5)
	{
		i=BSDE_SolidHull_HullFiltPoints(vec, cnt, nv);
		return(i);
	}

	i=cnt/2;
	j=BSDE_SolidHull_HullFiltPointsR(vec, i, nv);
	k=BSDE_SolidHull_HullFiltPointsR(vec+i*3, cnt-i, nv+j*3);
	i=BSDE_SolidHull_HullFiltPoints(nv, k, nv);
	return(i);
}

int BSDE_SolidHull_HullFromPoints2(bsde_real *vec, int nvec, bsde_real *nv)
{
	bsde_real *vbuf;
	int i, j, k;

	vbuf=(bsde_real *)malloc(nvec*3*sizeof(bsde_real));

	//merge points
	k=0;
	for(i=0; i<nvec; i++)
	{
		for(j=0; j<k; j++)
			if(V3_DIST(vec+i*3, vbuf+j*3)<0.1)
				break;
		if(j<k)continue;

		j=k++;
		V3_COPY(vec+i*3, vbuf+j*3);
	}

	//eliminate add non-corner points
	i=BSDE_SolidHull_HullFiltPointsR(vbuf, k, vbuf);

	//make the hull
	i=BSDE_SolidHull_HullFromPoints(vbuf, i, nv);

	return(i);
}

