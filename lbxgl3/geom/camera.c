#include <lbxgl2.h>

lbxEntity *lbxgl_cam_entity;

double lbxgl_cam_org[3];
double lbxgl_cam_ang[3];
double lbxgl_cam_vel[3];

double lbxgl_cam_fw[3];
double lbxgl_cam_rt[3];
double lbxgl_cam_up[3];

/*--
Cat lbxgl;Camera
Form
	void LBXGL_SetCamEntity(lbxEntity *ent);
Description
	Sets the entity for the camera.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_SetCamEntity(lbxEntity *ent)
{
	lbxgl_cam_entity=ent;
}

/*--
Cat lbxgl;Camera
Form
	void LBXGL_SetCamOrigin(double x, double y, double z);
	void LBXGL_SetCamOriginF(float *org);
	void LBXGL_SetCamOriginD(double *org);
Description
	Tells LBXGL where the camera is at.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_SetCamOrigin(double x, double y, double z)
{
	double v[3];
	v[0]=x;
	v[1]=y;
	v[2]=z;
	LBXGL_SetCamOriginD(v);
}

void LBXGL_SetCamOriginF(float *org)
{
	lbxgl_cam_org[0]=org[0];
	lbxgl_cam_org[1]=org[1];
	lbxgl_cam_org[2]=org[2];
}

void LBXGL_SetCamOriginD(double *org)
{
	lbxgl_cam_org[0]=org[0];
	lbxgl_cam_org[1]=org[1];
	lbxgl_cam_org[2]=org[2];
}


/*--
Cat lbxgl;Camera
Form
	void LBXGL_SetCamAngles(double x, double y, double z);
	void LBXGL_SetCamAnglesF(float *ang);
	void LBXGL_SetCamAnglesD(double *ang);
Description
	Tells LBXGL where the camera is at.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_SetCamAngles(double x, double y, double z)
{
	double v[3];
	v[0]=x;
	v[1]=y;
	v[2]=z;
	LBXGL_SetCamAnglesD(v);
}

void LBXGL_SetCamAnglesF(float *ang)
{
	lbxgl_cam_ang[0]=ang[0];
	lbxgl_cam_ang[1]=ang[1];
	lbxgl_cam_ang[2]=ang[2];

	Vec3D_CalcAngleVectors(lbxgl_cam_ang, lbxgl_cam_fw,
		lbxgl_cam_rt, lbxgl_cam_up);
}

void LBXGL_SetCamAnglesD(double *ang)
{
	lbxgl_cam_ang[0]=ang[0];
	lbxgl_cam_ang[1]=ang[1];
	lbxgl_cam_ang[2]=ang[2];

	Vec3D_CalcAngleVectors(lbxgl_cam_ang, lbxgl_cam_fw,
		lbxgl_cam_rt, lbxgl_cam_up);
}


/*--
Cat lbxgl;Camera
Form
	void LBXGL_SetCamVelocity(double x, double y, double z);
	void LBXGL_SetCamVelocityF(float *vel);
	void LBXGL_SetCamVelocityD(double *vel);
Description
	Tells LBXGL where the camera is at.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_SetCamVelocity(double x, double y, double z)
{
	double v[3];
	v[0]=x;
	v[1]=y;
	v[2]=z;
	LBXGL_SetCamVelocityD(v);
}

void LBXGL_SetCamVelocityF(float *vel)
{
	lbxgl_cam_vel[0]=vel[0];
	lbxgl_cam_vel[1]=vel[1];
	lbxgl_cam_vel[2]=vel[2];
}

void LBXGL_SetCamVelocityD(double *vel)
{
	lbxgl_cam_vel[0]=vel[0];
	lbxgl_cam_vel[1]=vel[1];
	lbxgl_cam_vel[2]=vel[2];
}

/*--
Cat lbxgl;Camera
Form
	lbxEntity *LBXGL_GetCamEntity();
Description
	Fetches previously set info about the camera.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
lbxEntity *LBXGL_GetCamEntity()
{
	return(lbxgl_cam_entity);
}

/*--
Cat lbxgl;Camera
Form
	void LBXGL_GetCamOriginF(float *org);
	void LBXGL_GetCamOriginD(double *org);
Description
	Fetches previously set info about the camera.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_GetCamOriginF(float *org)
{
	org[0]=lbxgl_cam_org[0];
	org[1]=lbxgl_cam_org[1];
	org[2]=lbxgl_cam_org[2];
}

void LBXGL_GetCamOriginD(double *org)
{
	org[0]=lbxgl_cam_org[0];
	org[1]=lbxgl_cam_org[1];
	org[2]=lbxgl_cam_org[2];
}

/*--
Cat lbxgl;Camera
Form
	void LBXGL_GetCamAnglesF(float *ang);
	void LBXGL_GetCamAnglesD(double *ang);
Description
	Fetches previously set info about the camera.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_GetCamAnglesF(float *ang)
{
	ang[0]=lbxgl_cam_ang[0];
	ang[1]=lbxgl_cam_ang[1];
	ang[2]=lbxgl_cam_ang[2];
}

void LBXGL_GetCamAnglesD(double *ang)
{
	ang[0]=lbxgl_cam_ang[0];
	ang[1]=lbxgl_cam_ang[1];
	ang[2]=lbxgl_cam_ang[2];
}

/*--
Cat lbxgl;Camera
Form
	void LBXGL_GetCamVelocityF(float *vel);
	void LBXGL_GetCamVelocityD(double *vel);
Description
	Fetches previously set info about the camera.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_GetCamVelocityF(float *vel)
{
	vel[0]=lbxgl_cam_vel[0];
	vel[1]=lbxgl_cam_vel[1];
	vel[2]=lbxgl_cam_vel[2];
}

void LBXGL_GetCamVelocityD(double *vel)
{
	vel[0]=lbxgl_cam_vel[0];
	vel[1]=lbxgl_cam_vel[1];
	vel[2]=lbxgl_cam_vel[2];
}

/*--
Cat lbxgl;Camera
Form
	void LBXGL_GetCamVectorsF(float *fw, float *rt, float *up);	
	void LBXGL_GetCamVectorsD(double *fw, double *rt, double *up);
Description
	Fetches previously set info about the camera.
	This can be used by various subsystems to gain info about the \
	camera.
--*/
void LBXGL_GetCamVectorsF(float *fw, float *rt, float *up)
{
	if(fw)
	{
		fw[0]=lbxgl_cam_fw[0];
		fw[1]=lbxgl_cam_fw[1];
		fw[2]=lbxgl_cam_fw[2];
	}

	if(rt)
	{
		rt[0]=lbxgl_cam_rt[0];
		rt[1]=lbxgl_cam_rt[1];
		rt[2]=lbxgl_cam_rt[2];
	}

	if(up)
	{
		up[0]=lbxgl_cam_up[0];
		up[1]=lbxgl_cam_up[1];
		up[2]=lbxgl_cam_up[2];
	}
}

void LBXGL_GetCamVectorsD(double *fw, double *rt, double *up)
{
	if(fw)
	{
		fw[0]=lbxgl_cam_fw[0];
		fw[1]=lbxgl_cam_fw[1];
		fw[2]=lbxgl_cam_fw[2];
	}

	if(rt)
	{
		rt[0]=lbxgl_cam_rt[0];
		rt[1]=lbxgl_cam_rt[1];
		rt[2]=lbxgl_cam_rt[2];
	}

	if(up)
	{
		up[0]=lbxgl_cam_up[0];
		up[1]=lbxgl_cam_up[1];
		up[2]=lbxgl_cam_up[2];
	}
}


/*--
Cat lbxgl;Camera
Form
	void LBXGL_GetBBoxExtentsF(float *mins, float *maxs, float *dir, \
	float *min, float *max);
	void LBXGL_GetBBoxExtentsD(double *mins, double *maxs, \
	double *dir, double *min, double *max);
Description
	Get The extents of a bounding box along a line.
--*/
void LBXGL_GetBBoxExtentsF(float *mins, float *maxs, float *dir,
	float *min, float *max)
{
	double d0, d1;
	int i;

	d0=0;
	d1=0;
	for(i=0; i<3; i++)
	{
		if(dir[i]>0)
		{
			d0+=maxs[i]*dir[i];
			d1+=mins[i]*dir[i];
		}else
		{
			d0+=mins[i]*dir[i];
			d1+=maxs[i]*dir[i];
		}
	}

	*max=d0;
	*min=d1;
}

void LBXGL_GetBBoxExtentsD(double *mins, double *maxs, double *dir,
	double *min, double *max)
{
	double d0, d1;
	int i;

	d0=0;
	d1=0;
	for(i=0; i<3; i++)
	{
		if(dir[i]>0)
		{
			d0+=maxs[i]*dir[i];
			d1+=mins[i]*dir[i];
		}else
		{
			d0+=mins[i]*dir[i];
			d1+=maxs[i]*dir[i];
		}
	}

	*max=d0;
	*min=d1;
}

