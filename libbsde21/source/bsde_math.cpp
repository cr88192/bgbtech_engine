#include "bsde_internal.h"

//#define TENSOR_STEPS	64
//#define TENSOR_STEPS	256
#define TENSOR_STEPS	16

void BSDE_CrossXForm(bsde_real *r, bsde_real *s)
{
	s[0]=0;		s[1]=-r[2];	s[2]=r[1];
	s[3]=r[2];	s[4]=0;		s[5]=-r[0];
	s[6]=-r[1];	s[7]=r[0];	s[8]=0;
}

#if 0
void BSDE_BuildInertiaTensor(BSDE_Solid *obj)
{
	bsde_real mins[3], maxs[3];
	bsde_real im[9]; //inertial tensor
	bsde_real tm0[9], tm1[9];
	bsde_real v[3], s[3], c[3], r[3];
	int i, j, k, l, n;

	printf("building inertia tensor\n");

	BSDE_Box(obj, mins, maxs);

	for(i=0; i<9; i++)im[i]=0;
	s[0]=(maxs[0]-mins[0])/TENSOR_STEPS;
	s[1]=(maxs[1]-mins[1])/TENSOR_STEPS;
	s[2]=(maxs[2]-mins[2])/TENSOR_STEPS;
	n=0;

	c[0]=(mins[0]+maxs[0])*0.5;
	c[1]=(mins[1]+maxs[1])*0.5;
	c[2]=(mins[2]+maxs[2])*0.5;

	for(i=0; i<TENSOR_STEPS; i++)
		for(j=0; j<TENSOR_STEPS; j++)
			for(k=0; k<TENSOR_STEPS; k++)
	{
		v[0]=s[0]*i+mins[0];
		v[1]=s[1]*j+mins[1];
		v[2]=s[2]*k+mins[2];

		V3_SUB(v, c, r);

//		l=obj->funcs->pointcollide(obj, v);
		l=BSDE_PointCollide(obj, v);
		if(!l)continue;

		BSDE_CrossXForm(r, tm0);
		Mat3_MatMult(tm0, tm0, tm1);

//		for(l=0; l<9; l++)
//			if(tm1[l]<0.001)tm1[l]=0;

		for(l=0; l<9; l++)
//			im[l]-=tm1[l];
			im[l]-=obj->mass*tm1[l];
		n++;
	}

//	if(!n)n=1;
	for(i=0; i<9; i++)im[i]*=1.0/n;


	MatN_Inverse(im, tm0, 3);
//	Mat3_Transpose(im, tm0);

	printf("%f %f %f\n", im[0], im[1], im[2]);
	printf("%f %f %f\n", im[3], im[4], im[5]);
	printf("%f %f %f\n", im[6], im[7], im[8]);
	printf("\n");

	printf("%f %f %f\n", tm0[0], tm0[1], tm0[2]);
	printf("%f %f %f\n", tm0[3], tm0[4], tm0[5]);
	printf("%f %f %f\n", tm0[6], tm0[7], tm0[8]);
	printf("\n");

	Mat3_Copy(im, obj->mass_imat);
	Mat3_Copy(tm0, obj->mass_tmat);

	BSDE_WorldInertiaTensor(obj);

	printf("ok\n");

#if 0
	printf("%f %f %f\n", im[0], im[1], im[2]);
	printf("%f %f %f\n", im[3], im[4], im[5]);
	printf("%f %f %f\n", im[6], im[7], im[8]);

	Mat3_Copy(im, obj->mass_imat);
	Mat3_Transpose(im, obj->mass_tmat);

	printf("ok\n");
#endif
}
#endif

#if 1
void BSDE_BuildInertiaTensor(BSDE_Solid *obj)
{
	bsde_real mins[3], maxs[3];
	bsde_real im[9]; //inertial tensor
	bsde_real tm0[9], tm1[9];
	bsde_real v[3], s[3], m;
	int i, j, k, l, n;

	for(i=0; i<16; i++)obj->xform[i]=0;
	for(i=0; i<4; i++)obj->xform[i*4+i]=1;
	for(i=0; i<16; i++)obj->e_xform[i]=0;
	for(i=0; i<4; i++)obj->e_xform[i*4+i]=1;

	BSDE_Box(obj, mins, maxs);

	for(i=0; i<9; i++)im[i]=0;

	s[0]=(maxs[0]-mins[0])/TENSOR_STEPS;
	s[1]=(maxs[1]-mins[1])/TENSOR_STEPS;
	s[2]=(maxs[2]-mins[2])/TENSOR_STEPS;
	n=0;

	m=0;
	for(i=0; i<TENSOR_STEPS; i++)
		for(j=0; j<TENSOR_STEPS; j++)
			for(k=0; k<TENSOR_STEPS; k++)
	{
		v[0]=s[0]*i+mins[0];
		v[1]=s[1]*j+mins[1];
		v[2]=s[2]*k+mins[2];

		l=BSDE_PointCollide(obj, v);
		if(!l)continue;

		im[0]+=v[1]*v[1]+v[2]*v[2];
		im[4]+=v[0]*v[0]+v[2]*v[2];
		im[8]+=v[0]*v[0]+v[1]*v[1];

		im[1]+=v[0]*v[1];
		im[2]+=v[0]*v[2];
		im[5]+=v[1]*v[2];
		im[3]+=v[0]*v[1];
		im[6]+=v[0]*v[2];
		im[7]+=v[1]*v[2];

		m+=v[0]*v[0]+v[1]*v[1]+v[2]*v[2];

		n++;
	}

	for(i=0; i<9; i++)im[i]*=1.0/n;
	m/=n;

	BSDE_MatN_Inverse(im, tm0, 3);

	BSDE_Mat3_Copy(im, obj->mass_imat);
	BSDE_Mat3_Copy(tm0, obj->mass_tmat);
	obj->moment=m;

	BSDE_WorldInertiaTensor(obj);
}
#endif

void BSDE_BuildBoxInertiaTensor(BSDE_Solid *obj)
{
	bsde_real mins[3], maxs[3];
	bsde_real im[9]; //inertial tensor
	bsde_real tm0[9], tm1[9];
	bsde_real v[3], s[3], m;
	int i, j, k, l, n;

//	printf("building inertia tensor\n");
	for(i=0; i<16; i++)obj->xform[i]=0;
	for(i=0; i<4; i++)obj->xform[i*4+i]=1;
	for(i=0; i<16; i++)obj->e_xform[i]=0;
	for(i=0; i<4; i++)obj->e_xform[i*4+i]=1;

	for(i=0; i<9; i++)im[i]=0;

	BSDE_Box(obj, mins, maxs);
	s[0]=(maxs[0]-mins[0])/TENSOR_STEPS;
	s[1]=(maxs[1]-mins[1])/TENSOR_STEPS;
	s[2]=(maxs[2]-mins[2])/TENSOR_STEPS;
	n=0;
	m=1;

	for(i=0; i<TENSOR_STEPS; i++)
		for(j=0; j<TENSOR_STEPS; j++)
			for(k=0; k<TENSOR_STEPS; k++)
	{
		v[0]=s[0]*i+mins[0];
		v[1]=s[1]*j+mins[1];
		v[2]=s[2]*k+mins[2];

		im[0]+=m*(v[1]*v[1]+v[2]*v[2]);
		im[4]+=m*(v[0]*v[0]+v[2]*v[2]);
		im[8]+=m*(v[0]*v[0]+v[1]*v[1]);

		im[1]+=m*(v[0]*v[1]);
		im[2]+=m*(v[0]*v[2]);
		im[5]+=m*(v[1]*v[2]);
		im[3]+=m*(v[0]*v[1]);
		im[6]+=m*(v[0]*v[2]);
		im[7]+=m*(v[1]*v[2]);

		n++;
	}

	if(!n)n=1;
	for(i=0; i<9; i++)im[i]*=1.0/n;

	BSDE_MatN_Inverse(im, tm0, 3);
	BSDE_Mat3_Copy(im, obj->mass_imat);
	BSDE_Mat3_Copy(tm0, obj->mass_tmat);

	BSDE_WorldInertiaTensor(obj);
}

void BSDE_WorldInertiaTensor(BSDE_Solid *obj)
{
//	bsde_real pad[16];
	bsde_real ma[9], mb[9], mc[9];

	BSDE_Mat3_Scale(obj->mass_tmat, 1.0/obj->mass, mc);
//	BSDE_Mat3_Scale(obj->mass_tmat, 1.0, mc);
//	BSDE_Mat3_Copy(obj->mass_tmat, mc);

	BSDE_Quat_To3Matrix(obj->rot, ma);
	BSDE_Mat3_MatMult(ma, mc, mb);
	BSDE_Mat3_Transpose(ma, mc);
//	BSDE_MatN_Inverse(ma, mc, 3);
	BSDE_Mat3_MatMult(mb, mc, obj->wimat);

	BSDE_MatN_MatMult(obj->wimat, 3, 3, obj->tvel, 3, 1, obj->avel);

//	BSDE_MatN_Inverse(obj->wimat, obj->wtmat, 3);

	//calculate "other" inertial matrix

	BSDE_Mat3_Scale(obj->mass_imat, obj->mass, mc);
	BSDE_Quat_To3Matrix(obj->rot, ma);
	BSDE_Mat3_MatMult(ma, mc, mb);
	BSDE_Mat3_Transpose(ma, mc);
	BSDE_Mat3_MatMult(mb, mc, obj->wtmat);

//	printf("%f %f %f\n%f %f %f\n%f %f %f\n",
//		obj->wtmat[0], obj->wtmat[1], obj->wtmat[2],
//		obj->wtmat[3], obj->wtmat[4], obj->wtmat[5],
//		obj->wtmat[6], obj->wtmat[7], obj->wtmat[8]);


//	printf("avel %f %f %f\n", obj->avel[0], obj->avel[1], obj->avel[2]);
}

void BSDE_TransformPointLocal(BSDE_Solid *obj, bsde_real *org, bsde_real *pt)
{
	bsde_real xform[16];
	int i;

	BSDE_Quat_ToMatrix(obj->rot, xform);
	for(i=0; i<3; i++)xform[3*4+i]+=obj->org[i];

	BSDE_Plane_TransformPointNT(org, xform, pt);
}

void BSDE_TransformPointGlobal(BSDE_Solid *obj, bsde_real *org, bsde_real *pt)
{
	BSDE_Plane_TransformPoint(org, obj->xform, pt);
}

void BSDE_TransformVectorLocal(BSDE_Solid *obj, bsde_real *org, bsde_real *pt)
{
	bsde_real xform[16];
	int i;

	BSDE_Quat_ToMatrix(obj->rot, xform);
	for(i=0; i<3; i++)xform[3*4+i]+=obj->org[i];

	BSDE_Plane_TransformVectorT(org, xform, pt);
}

void BSDE_TransformVectorGlobal(BSDE_Solid *obj, bsde_real *org, bsde_real *pt)
{
	BSDE_Plane_TransformVector(org, obj->xform, pt);
}
