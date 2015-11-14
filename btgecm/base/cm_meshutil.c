#include <btgecm.h>

BTGE_API int BTGE_Mesh_IndexSTf(BTGE_MeshPrim *prim,
	float s, float t)
{
	float tv[4]; tv[0]=s; tv[1]=t;
	return(BTGE_Mesh_IndexST(prim, tv));
}

BTGE_API int BTGE_Mesh_IndexXYZf(BTGE_MeshPrim *prim,
	float x, float y, float z)
{
	float tv[4]; tv[0]=x; tv[1]=y; tv[2]=z;
	return(BTGE_Mesh_IndexXYZ(prim, tv));
}

BTGE_API int BTGE_Mesh_AddQuadVec(BTGE_MeshPrim *prim,
	float *xyz0, float *st0, float *xyz1, float *st1,
	float *xyz2, float *st2, float *xyz3, float *st3)
{
	int i0, i1, i2, i3;
	int j0, j1, j2, j3;
	int i;

	i0=BTGE_Mesh_IndexXYZ(prim, xyz0);
	i1=BTGE_Mesh_IndexXYZ(prim, xyz1);
	i2=BTGE_Mesh_IndexXYZ(prim, xyz2);
	i3=BTGE_Mesh_IndexXYZ(prim, xyz3);
	j0=BTGE_Mesh_IndexST(prim, st0);
	j1=BTGE_Mesh_IndexST(prim, st1);
	j2=BTGE_Mesh_IndexST(prim, st2);
	j3=BTGE_Mesh_IndexST(prim, st3);

	i=BTGE_Mesh_AddQuad(prim, i0, j0, i1, j1, i2, j2, i3, j3);
	return(i);
}

BTGE_API int BTGE_Mesh_AddCube(
	BTGE_MeshPrim *prim, float *mins, float *maxs)
{
	int i, j;
	float r, g, b, a;

//	printf("cube (%f %f %f) (%f %f %f)\n",
//		minx, miny, minz,
//		maxx, maxy, maxz);

	float vtxa[8*3];
	static int vtxb[6*4]=
	{
		1, 3, 2, 0,
		4, 6, 7, 5,
		5, 7, 3, 1,
		0, 2, 6, 4,
		5, 1, 0, 4,
		3, 7, 6, 2
	};
	float vtxc[4*2];

	float *pt1, *pt2, *pt3, *pt4;
	float *st1, *st2, *st3, *st4;

	a=0.5;

	for(i=0; i<8; i++)
	{
		vtxa[i*3+0]=mins[0];
		vtxa[i*3+1]=mins[1];
		vtxa[i*3+2]=mins[2];
		if(i&1)vtxa[i*3+0]=maxs[0];
		if(i&2)vtxa[i*3+1]=maxs[1];
		if(i&4)vtxa[i*3+2]=maxs[2];
	}

	st1=vtxc+0; st2=vtxc+2;
	st3=vtxc+4; st4=vtxc+6;

	st1[0]=1; st1[1]=0;
	st2[0]=0; st2[1]=0;
	st3[0]=0; st3[1]=1;
	st4[0]=1; st4[1]=1;

	for(i=0; i<6; i++)
	{
		pt1=vtxa+(vtxb[i*4+0]*3);
		pt2=vtxa+(vtxb[i*4+1]*3);
		pt3=vtxa+(vtxb[i*4+2]*3);
		pt4=vtxa+(vtxb[i*4+3]*3);

		BTGE_Mesh_AddQuadVec(prim,
			pt1, st1, pt2, st2,
			pt3, st3, pt4, st4);
	}

	return(0);
}


BTGE_API void BTGE_Mesh_AdjacentNormals(float *norm, float *udir, float *vdir)
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

BTGE_API int BTGE_Mesh_AddCapsule(
	BTGE_MeshPrim *prim, float *v0, float *v1, float ra,
	float t1, float t2)
{
	float u[3], v[3], nv[3], pt[3];
	float tv0[3], tv1[3];
	float pt0[3], pt1[3], pt2[3], pt3[3];
	float st0[2], st1[2], st2[2], st3[2];
	float a1, a2, a3, a4, x1, y1, x2, y2, s1, s2;
	int i, j;

	V3F_SUB(v1, v0, nv);
	V3F_NORMALIZE(nv, nv);
	BTGE_Mesh_AdjacentNormals(nv, u, v);

	for(i=0; i<8; i++)
	{
		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);

		s1=(1.0/16.0)*i;
		s2=(1.0/16.0)*(i+1);

		st0[0]=s1; st0[1]=t1;
		st1[0]=s1; st1[1]=t2;
		st2[0]=s2; st2[1]=t2;
		st3[0]=s2; st3[1]=t1;

		x1=sin(a1)*ra; y1=cos(a1)*ra;
		x2=sin(a2)*ra; y2=cos(a2)*ra;

		tv0[0]=x1*u[0]+y1*v[0];
		tv0[1]=x1*u[1]+y1*v[1];
		tv0[2]=x1*u[2]+y1*v[2];

		tv1[0]=x2*u[0]+y2*v[0];
		tv1[1]=x2*u[1]+y2*v[1];
		tv1[2]=x2*u[2]+y2*v[2];


		pt0[0]=v1[0]+tv0[0];
		pt0[1]=v1[1]+tv0[1];
		pt0[2]=v1[2]+tv0[2];

		pt1[0]=v0[0]+tv0[0];
		pt1[1]=v0[1]+tv0[1];
		pt1[2]=v0[2]+tv0[2];

		pt2[0]=v0[0]+tv1[0];
		pt2[1]=v0[1]+tv1[1];
		pt2[2]=v0[2]+tv1[2];

		pt3[0]=v1[0]+tv1[0];
		pt3[1]=v1[1]+tv1[1];
		pt3[2]=v1[2]+tv1[2];

		BTGE_Mesh_AddQuadVec(prim,
			pt0, st0, pt1, st1, pt2, st2, pt3, st3);
	}

	for(i=0; i<8; i++)
		for(j=2; j<4; j++)
	{
		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);
		a3=((M_PI*2)/8)*j;
		a4=((M_PI*2)/8)*(j+1);

		st0[0]=(1.0/8.0)*i;		st0[1]=(-1.0/4.0)*j;
		st1[0]=(1.0/8.0)*i;		st1[1]=(-1.0/4.0)*(j+1);
		st2[0]=(1.0/8.0)*(i+1);	st2[1]=(-1.0/4.0)*(j+1);
		st3[0]=(1.0/8.0)*(i+1); st3[1]=(-1.0/4.0)*j;

		pt[0]=sin(a3)*sin(a1)*ra;
		pt[1]=sin(a3)*cos(a1)*ra;
		pt[2]=cos(a3)*ra;
		pt0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		pt0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		pt0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];

		pt[0]=sin(a4)*sin(a1)*ra;
		pt[1]=sin(a4)*cos(a1)*ra;
		pt[2]=cos(a4)*ra;
		pt1[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		pt1[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		pt1[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];

		pt[0]=sin(a4)*sin(a2)*ra;
		pt[1]=sin(a4)*cos(a2)*ra;
		pt[2]=cos(a4)*ra;
		pt2[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		pt2[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		pt2[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];

		pt[0]=sin(a3)*sin(a2)*ra;
		pt[1]=sin(a3)*cos(a2)*ra;
		pt[2]=cos(a3)*ra;
		pt3[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v0[0];
		pt3[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v0[1];
		pt3[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v0[2];

		BTGE_Mesh_AddQuadVec(prim,
			pt0, st0, pt1, st1, pt2, st2, pt3, st3);
	}

	for(i=0; i<8; i++)
		for(j=0; j<2; j++)
	{
		a1=((M_PI*2)/8)*i;
		a2=((M_PI*2)/8)*(i+1);
		a3=((M_PI*2)/8)*j;
		a4=((M_PI*2)/8)*(j+1);

		st0[0]=(1.0/8.0)*i;		st0[1]=(-1.0/4.0)*j;
		st1[0]=(1.0/8.0)*i;		st1[1]=(-1.0/4.0)*(j+1);
		st2[0]=(1.0/8.0)*(i+1);	st2[1]=(-1.0/4.0)*(j+1);
		st3[0]=(1.0/8.0)*(i+1); st3[1]=(-1.0/4.0)*j;

		pt[0]=sin(a3)*sin(a1)*ra;
		pt[1]=sin(a3)*cos(a1)*ra;
		pt[2]=cos(a3)*ra;
		pt0[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		pt0[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		pt0[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];

		pt[0]=sin(a4)*sin(a1)*ra;
		pt[1]=sin(a4)*cos(a1)*ra;
		pt[2]=cos(a4)*ra;
		pt1[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		pt1[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		pt1[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];

		pt[0]=sin(a4)*sin(a2)*ra;
		pt[1]=sin(a4)*cos(a2)*ra;
		pt[2]=cos(a4)*ra;
		pt2[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		pt2[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		pt2[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];

		pt[0]=sin(a3)*sin(a2)*ra;
		pt[1]=sin(a3)*cos(a2)*ra;
		pt[2]=cos(a3)*ra;
		pt3[0]=pt[0]*u[0]+pt[1]*v[0]+pt[2]*nv[0]+v1[0];
		pt3[1]=pt[0]*u[1]+pt[1]*v[1]+pt[2]*nv[1]+v1[1];
		pt3[2]=pt[0]*u[2]+pt[1]*v[2]+pt[2]*nv[2]+v1[2];

		BTGE_Mesh_AddQuadVec(prim,
			pt0, st0, pt1, st1, pt2, st2, pt3, st3);
	}

	return(0);
}
