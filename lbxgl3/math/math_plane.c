#include <lbxgl2.h>
#include <math.h>

int PlaneF_LinePlaneIntersect(float *start, float *end, float *norm,
	float *point)
{
	float dir[3], x;

	if(VecNF_DotProduct(norm, norm, 3)==0)
	{
		VecNF_Copy(start, point, 3);
		return(-1);
	}

	//calc direction
	VecNF_Subtract(end, start, dir, 3);

	x=VecNF_DotProduct(dir, norm, 3);
	x=(x<0)?-x:x;
	VecNF_Scale(dir, 1/x, dir, 3);

	//calc intersection
	x=VecNF_DotProduct(start, norm, 3)-norm[3];
	x=(x<0)?-x:x;
	VecNF_AddScale(start, dir, x, point, 3);

	return(0);
}

int PlaneF_CheckLinePlane(float *start, float *end, float *norm)
{
	float x;
	int i;

	if(VecNF_DotProduct(norm, norm, 3)==0)
		return(0);

	i=0;
	x=VecNF_DotProduct(start, norm, 3)-norm[3];
	if(x>=0)i|=1;
		else i|=2;

	x=VecNF_DotProduct(end, norm, 3)-norm[3];
	if(x>=0)i|=1;
		else i|=2;

	return(i);
}

int PlaneF_ClipLinePlane(float *start, float *end, float *norm)
{
	int i;

	i=PlaneF_CheckLinePlane(start, end, norm);
	if(i!=3)return(i);

	PlaneF_LinePlaneIntersect(start, end, norm, end);
	return(i);
}

int PlaneF_CheckPlaneBox(float *norm, float *mins, float *maxs)
{
	float d0, d1;
	int i;

	d0=0;
	d1=0;
	for(i=0; i<3; i++)
	{
		if(norm[i]>0)
		{
			d0+=maxs[i]*norm[i];
			d1+=mins[i]*norm[i];
		}else
		{
			d0+=mins[i]*norm[i];
			d1+=maxs[i]*norm[i];
		}
	}

	i=0;
	if(d0>=norm[3])i|=1;	//>=dist
	if(d1<norm[3])i|=2;	//<dist

	if(!i)
	{
		kprint("PlaneF_CheckPlaneBox: fail (%lf %lf %lf %lf)\n",
			norm[0], norm[1], norm[2], norm[3]);
		*(int *)-1=-1;
	}

	return(i);
}
