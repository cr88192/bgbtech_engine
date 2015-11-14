#include <lbxgl2.h>

void BspUtil_ClosestAxisNormal(float *norm,
	float *vi, float *vj, float *vk)
{
	float tv0[3], tv1[3], tv2[3];
	float f;
	int i, j;

	j=-1;
	f=-1;
	for(i=0; i<3; i++)
		if(norm[i]>f)
	{
		j=i;
		f=norm[i];
	}

	V3F_ZERO(tv0);
	V3F_ZERO(tv1);
	V3F_ZERO(tv2);
	switch(j)
	{
	case 0:
		tv0[1]=1;
		tv1[2]=1;
		tv2[0]=1;
		break;
	case 1:
		tv0[0]=1;
		tv1[2]=1;
		tv2[1]=1;
		break;
	case 2:
		tv0[0]=1;
		tv1[1]=1;
		tv2[2]=1;
		break;
	default:
		break;
	}

	if(vi)V3F_COPY(tv0, vi);
	if(vj)V3F_COPY(tv1, vj);
	if(vk)V3F_COPY(tv2, vk);
}

void BspUtil_ClosestAxisNormalS(float *norm,
	float *vi, float *vj, float *vk)
{
	float tv0[3], tv1[3], tv2[3];
	float f;
	int i, j;

	j=-1;
	f=-1;
	for(i=0; i<3; i++)
		if(norm[i]>f)
	{
		j=i;
		f=norm[i];
	}
	for(i=0; i<3; i++)
		if((-norm[i])>f)
	{
		j=i+3;
		f=-norm[i];
	}

	V3F_ZERO(tv0);
	V3F_ZERO(tv1);
	V3F_ZERO(tv2);
	switch(j)
	{
	case 0:
		tv0[1]=1;
		tv1[2]=1;
		tv2[0]=1;
		break;
	case 1:
		tv0[0]=1;
		tv1[2]=1;
		tv2[1]=1;
		break;
	case 2:
		tv0[0]=1;
		tv1[1]=1;
		tv2[2]=1;
		break;

	case 3:
		tv0[1]=-1;
		tv1[2]=-1;
		tv2[0]=-1;
		break;
	case 4:
		tv0[0]=-1;
		tv1[2]=-1;
		tv2[1]=-1;
		break;
	case 5:
		tv0[0]=-1;
		tv1[1]=-1;
		tv2[2]=-1;
		break;

	default:
		break;
	}

	if(vi)V3F_COPY(tv0, vi);
	if(vj)V3F_COPY(tv1, vj);
	if(vk)V3F_COPY(tv2, vk);
}


int BspUtil_LinePlaneIntersect(float *start, float *end,
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
