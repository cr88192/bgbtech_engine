
/*
external interface for BSDE.
*/

#include "bsde_internal.h"


extern "C" {

BSDE_API char *bsdeGetString(int str)
{
	char *s;

	switch(str)
	{
	case BSDE_NULL: s=""; break;
	case BSDE_CREATOR: s="Brendan G Bohannon"; break;
	case BSDE_ENGINE: s="BGB-BSDE"; break;
	case BSDE_ENGINE_VER: s="0.021 " __DATE__; break;
	case BSDE_VERSION: s="0.02"; break;
	case BSDE_EXTENSIONS: s=""; break;
	default: s=NULL; break;
	}
	return(s);
}

BSDE_API void bsdeSRand(bsdeUInt seed)
{
	if(!BSDE_GetWorld())
		return;
	BSDE_SRand(BSDE_GetWorld(), seed);
	BSDE_Seed21(BSDE_GetWorld());
//	bsde_seeded=1;
}

BSDE_API void bsdeSRand2(bsdeUInt seed1, bsdeUInt seed2)
{
	if(!BSDE_GetWorld())
		return;
	BSDE_SRand2(BSDE_GetWorld(), seed1, seed2);
	BSDE_Seed21(BSDE_GetWorld());
//	bsde_seeded=1;
}

BSDE_API int bsdeNewWorld()
	{ return(BSDE_CreateWorld()); }

BSDE_API void bsdeBindWorld(int i)
	{ BSDE_SetWorld(i); }

BSDE_API void bsdeStepWorld(double dt)
	{ BSDE_StepWorld(BSDE_GetWorld(), dt); }

BSDE_API void bsdeEmptyWorld()
{
	BSDE_EmptyWorld(BSDE_GetWorld());
}


BSDE_API double bsdeGetWorldAttrF(int var)
	{ return(BSDE_GetWorldAttrF(BSDE_GetWorld(), var)); }
BSDE_API void bsdeSetWorldAttrF(int var, double val)
	{ BSDE_SetWorldAttrF(BSDE_GetWorld(), var, val); }

BSDE_API int bsdeGetWorldAttrI(int var)
	{ return(BSDE_GetWorldAttrI(BSDE_GetWorld(), var)); }
BSDE_API void bsdeSetWorldAttrI(int var, int val)
	{ BSDE_SetWorldAttrI(BSDE_GetWorld(), var, val); }

BSDE_API void bsdeGetWorldAttrFV(int var, float *val, int n)
	{ BSDE_GetWorldAttrFV(BSDE_GetWorld(), var, val, n); }
BSDE_API void bsdeSetWorldAttrFV(int var, float *val, int n)
	{ BSDE_SetWorldAttrFV(BSDE_GetWorld(), var, val, n); }

BSDE_API void bsdeGetWorldAttrDV(int var, double *val, int n)
	{ BSDE_GetWorldAttrDV(BSDE_GetWorld(), var, val, n); }
BSDE_API void bsdeSetWorldAttrDV(int var, double *val, int n)
	{ BSDE_SetWorldAttrDV(BSDE_GetWorld(), var, val, n); }


BSDE_API double bsdeGetWorldf(int var)
	{ return(BSDE_GetWorldAttrF(BSDE_GetWorld(), var)); }
BSDE_API void bsdeSetWorldf(int var, double val)
	{ BSDE_SetWorldAttrF(BSDE_GetWorld(), var, val); }

BSDE_API int bsdeGetWorldi(int var)
	{ return(BSDE_GetWorldAttrI(BSDE_GetWorld(), var)); }
BSDE_API void bsdeSetWorldi(int var, int val)
	{ BSDE_SetWorldAttrI(BSDE_GetWorld(), var, val); }

BSDE_API void bsdeGetWorldfv(int var, float *val, int n)
	{ BSDE_GetWorldAttrFV(BSDE_GetWorld(), var, val, n); }
BSDE_API void bsdeSetWorldfv(int var, float *val, int n)
	{ BSDE_SetWorldAttrFV(BSDE_GetWorld(), var, val, n); }

BSDE_API void bsdeGetWorlddv(int var, double *val, int n)
	{ BSDE_GetWorldAttrDV(BSDE_GetWorld(), var, val, n); }
BSDE_API void bsdeSetWorlddv(int var, double *val, int n)
	{ BSDE_SetWorldAttrDV(BSDE_GetWorld(), var, val, n); }


BSDE_API void bsdeGetWorld2fv(int var, float *val) { bsdeGetWorldfv(var, val, 2); }
BSDE_API void bsdeSetWorld2fv(int var, float *val) { bsdeSetWorldfv(var, val, 2); }
BSDE_API void bsdeGetWorld3fv(int var, float *val) { bsdeGetWorldfv(var, val, 3); }
BSDE_API void bsdeSetWorld3fv(int var, float *val) { bsdeSetWorldfv(var, val, 3); }
BSDE_API void bsdeGetWorld4fv(int var, float *val) { bsdeGetWorldfv(var, val, 4); }
BSDE_API void bsdeSetWorld4fv(int var, float *val) { bsdeSetWorldfv(var, val, 4); }

BSDE_API void bsdeGetWorld2dv(int var, double *val) { bsdeGetWorlddv(var, val, 2); }
BSDE_API void bsdeSetWorld2dv(int var, double *val) { bsdeSetWorlddv(var, val, 2); }
BSDE_API void bsdeGetWorld3dv(int var, double *val) { bsdeGetWorlddv(var, val, 3); }
BSDE_API void bsdeSetWorld3dv(int var, double *val) { bsdeSetWorlddv(var, val, 3); }
BSDE_API void bsdeGetWorld4dv(int var, double *val) { bsdeGetWorlddv(var, val, 4); }
BSDE_API void bsdeSetWorld4dv(int var, double *val) { bsdeSetWorlddv(var, val, 4); }

BSDE_API void bsdeSetWorld2f(int var, double x, double y)
	{ double v[4]; v[0]=x; v[1]=y; bsdeSetWorlddv(var, v, 2); }
BSDE_API void bsdeSetWorld3f(int var, double x, double y, double z)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; bsdeSetWorlddv(var, v, 3); }
BSDE_API void bsdeSetWorld4f(int var, double x, double y, double z, double w)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; v[3]=w; bsdeSetWorlddv(var, v, 4); }


BSDE_API int *bsdeGetTriggerArray()
{
	return(BSDE_GetTriggerArray(BSDE_GetWorld()));
}

BSDE_API int bsdeGetTriggerArrayPos()
{
	return(BSDE_GetTriggerArrayPos(BSDE_GetWorld()));
}

BSDE_API void bsdeResetTriggerArray()
{
	BSDE_ResetTriggerArray(BSDE_GetWorld());
}

BSDE_API void bsdeSetTriggerArray(int *arr, int num)
{
	BSDE_SetTriggerArray(BSDE_GetWorld(), arr, num);
}


BSDE_API int bsdeNewAABB(double mx, double my, double mz,
	double nx, double ny, double nz)
{
	BSDE_Solid *tmp;
	double f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	tmp=BSDE_NewAABB(mx*f, my*f, mz*f, nx*f, ny*f, nz*f);
	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewOBB(double mx, double my, double mz,
	double nx, double ny, double nz)
{
	BSDE_Solid *tmp;
	double f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	tmp=BSDE_NewOBB(mx*f, my*f, mz*f, nx*f, ny*f, nz*f);
	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewSphere(double radius)
{
	BSDE_Solid *tmp;
	double f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	tmp=BSDE_NewSphere(radius*f);
	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewCylinder(double height, double radius)
{
	BSDE_Solid *tmp;
	double f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	tmp=BSDE_NewCylinder(height*f, radius*f);
	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewCappedCylinder(double height, double radius)
{
	BSDE_Solid *tmp;
	double f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	tmp=BSDE_NewCappedCylinder(height*f, radius*f);
	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewPlane(double x, double y, double z, double d)
{
	BSDE_Solid *tmp;
	double f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	tmp=BSDE_NewPlane(x, y, z, d*f);
	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewHullF(float *norms, int n)
{
	BSDE_Solid *tmp;
	bsde_real *np;
	bsde_real f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	np=(bsde_real *)malloc(4*n*sizeof(bsde_real));
	for(i=0; i<(4*n); i++)np[i]=norms[i];
	for(i=0; i<n; i++)np[i*4+3]*=f;
	tmp=BSDE_NewHull(np, n);
	free(np);

	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewHullD(double *norms, int n)
{
	BSDE_Solid *tmp;
	bsde_real *np;
	bsde_real f;
	int i;

	f=BSDE_GetWorld()->sc_len;
	np=(bsde_real *)malloc(4*n*sizeof(bsde_real));
	for(i=0; i<(4*n); i++)np[i]=norms[i];
	for(i=0; i<n; i++)np[i*4+3]*=f;
	tmp=BSDE_NewHull(np, n);
	free(np);

	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewTriMeshF(float *vecs, int *tris, int nv, int nt)
{
	BSDE_Solid *tmp;
	bsde_real *np;
	int i;
	bsde_real f;

	f=BSDE_GetWorld()->sc_len;

	np=(bsde_real *)malloc(3*nv*sizeof(bsde_real));
	for(i=0; i<(3*nv); i++)np[i]=vecs[i]*f;
	tmp=BSDE_NewTriMesh(np, tris, nv, nt);
	free(np);

	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewTriMeshD(double *vecs, int *tris, int nv, int nt)
{
	BSDE_Solid *tmp;
	bsde_real *np;
	int i;
	double f;

	f=BSDE_GetWorld()->sc_len;

	np=(bsde_real *)malloc(3*nv*sizeof(double));
	for(i=0; i<(3*nv); i++)np[i]=vecs[i]*f;
	tmp=BSDE_NewTriMesh(np, tris, nv, nt);
	free(np);

	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API int bsdeNewHullf(float *norms, int n)
	{ return(bsdeNewHullF(norms, n)); }
BSDE_API int bsdeNewHulld(double *norms, int n)
	{ return(bsdeNewHullD(norms, n)); }

BSDE_API int bsdeNewTriMeshf(float *vecs, int *tris, int nv, int nt)
	{ return(bsdeNewTriMeshF(vecs, tris, nv, nt)); }
BSDE_API int bsdeNewTriMeshd(double *vecs, int *tris, int nv, int nt)
	{ return(bsdeNewTriMeshD(vecs, tris, nv, nt)); }

BSDE_API int bsdeNewVoxelChunk(int xs, int ys, int zs, double vsz)
{
	BSDE_Solid *tmp;
	int i;
	double f;

	f=BSDE_GetWorld()->sc_len;

	tmp=BSDE_NewVoxelChunk(xs, ys, zs, vsz*f);

	i=BSDE_AddSolidWorld(BSDE_GetWorld(), tmp);
	return(i);
}


BSDE_API void bsdeDestroy(int obj)
{
	BSDE_Solid *tmp;

	tmp=BSDE_GetWorldSolid(obj);
	if(!tmp)return;

	BSDE_Destroy(tmp);
	BSDE_SetWorldSolid(obj, NULL);
}

BSDE_API void bsdeHide(int obj)
{
	BSDE_Solid *tmp;
	tmp=BSDE_GetWorldSolid(obj);
	if(!tmp)return;
	tmp->moveflags|=BSDE_MOVEFL_HIDDEN;
}

BSDE_API void bsdeUnhide(int obj)
{
	BSDE_Solid *tmp;
	tmp=BSDE_GetWorldSolid(obj);
	if(!tmp)return;
	tmp->moveflags&=~BSDE_MOVEFL_HIDDEN;
}


BSDE_API double bsdeGetAttrF(int obj, int var)
{
	double f;

	f=BSDE_GetAttrF(BSDE_GetWorldSolid(obj), var);
	return(f);
}

BSDE_API void bsdeSetAttrF(int obj, int var, double val)
{
	BSDE_SetAttrF(BSDE_GetWorldSolid(obj), var, val);
}

BSDE_API int bsdeGetAttrI(int obj, int var)
{
	int i;
	i=BSDE_GetAttrI(BSDE_GetWorldSolid(obj), var);
	return(i);
}

BSDE_API void bsdeSetAttrI(int obj, int var, int val)
{
	BSDE_SetAttrI(BSDE_GetWorldSolid(obj), var, val);
}

BSDE_API void bsdeGetAttrFV(int obj, int var, float *val, int n)
{
	BSDE_GetAttrFV(BSDE_GetWorldSolid(obj), var, val, n);
}

BSDE_API void bsdeSetAttrFV(int obj, int var, float *val, int n)
{
	BSDE_SetAttrFV(BSDE_GetWorldSolid(obj), var, val, n);
}

BSDE_API void bsdeGetAttrDV(int obj, int var, double *val, int n)
{
	BSDE_GetAttrDV(BSDE_GetWorldSolid(obj), var, val, n);
}

BSDE_API void bsdeSetAttrDV(int obj, int var, double *val, int n)
{
	BSDE_SetAttrDV(BSDE_GetWorldSolid(obj), var, val, n);
}

BSDE_API void bsdeGetAttrIV(int obj, int var, int *val, int n)
{
	BSDE_GetAttrIV(BSDE_GetWorldSolid(obj), var, val, n);
}

BSDE_API void bsdeSetAttrIV(int obj, int var, int *val, int n)
{
	BSDE_SetAttrIV(BSDE_GetWorldSolid(obj), var, val, n);
}


BSDE_API void bsdeGetAttr2FV(int obj, int var, float *val)
	{ bsdeGetAttrFV(obj, var, val, 2); }
BSDE_API void bsdeSetAttr2FV(int obj, int var, float *val)
	{ bsdeSetAttrFV(obj, var, val, 2); }

BSDE_API void bsdeGetAttr2DV(int obj, int var, double *val)
	{ bsdeGetAttrDV(obj, var, val, 2); }
BSDE_API void bsdeSetAttr2DV(int obj, int var, double *val)
	{ bsdeSetAttrDV(obj, var, val, 2); }
BSDE_API void bsdeGetAttr2IV(int obj, int var, int *val)
	{ bsdeGetAttrIV(obj, var, val, 2); }
BSDE_API void bsdeSetAttr2IV(int obj, int var, int *val)
	{ bsdeSetAttrIV(obj, var, val, 2); }

BSDE_API void bsdeGetAttr3FV(int obj, int var, float *val)
	{ bsdeGetAttrFV(obj, var, val, 3); }
BSDE_API void bsdeSetAttr3FV(int obj, int var, float *val)
	{ bsdeSetAttrFV(obj, var, val, 3); }

BSDE_API void bsdeGetAttr3DV(int obj, int var, double *val)
	{ bsdeGetAttrDV(obj, var, val, 3); }
BSDE_API void bsdeSetAttr3DV(int obj, int var, double *val)
	{ bsdeSetAttrDV(obj, var, val, 3); }

BSDE_API void bsdeGetAttr3IV(int obj, int var, int *val)
	{ bsdeGetAttrIV(obj, var, val, 3); }
BSDE_API void bsdeSetAttr3IV(int obj, int var, int *val)
	{ bsdeSetAttrIV(obj, var, val, 3); }

BSDE_API void bsdeGetAttr4FV(int obj, int var, float *val)
	{ bsdeGetAttrFV(obj, var, val, 4); }
BSDE_API void bsdeSetAttr4FV(int obj, int var, float *val)
	{ bsdeSetAttrFV(obj, var, val, 4); }

BSDE_API void bsdeGetAttr4DV(int obj, int var, double *val)
	{ bsdeGetAttrDV(obj, var, val, 4); }
BSDE_API void bsdeSetAttr4DV(int obj, int var, double *val)
	{ bsdeSetAttrDV(obj, var, val, 4); }
BSDE_API void bsdeGetAttr4IV(int obj, int var, int *val)
	{ bsdeGetAttrIV(obj, var, val, 4); }
BSDE_API void bsdeSetAttr4IV(int obj, int var, int *val)
	{ bsdeSetAttrIV(obj, var, val, 4); }

BSDE_API void bsdeSetAttr2D(int obj, int var, double x, double y)
	{ double v[4];	v[0]=x; v[1]=y;
		bsdeSetAttrDV(obj, var, v, 2); }
BSDE_API void bsdeSetAttr2I(int obj, int var, int x, int y)
	{ int v[4];	v[0]=x; v[1]=y;
		bsdeSetAttrIV(obj, var, v, 2); }
BSDE_API void bsdeSetAttr2F(int obj, int var, double x, double y)
	{ bsdeSetAttr2D(obj, var, x, y); }

BSDE_API void bsdeSetAttr3D(int obj, int var, double x, double y, double z)
	{ double v[4];	v[0]=x; v[1]=y; v[2]=z;
		bsdeSetAttrDV(obj, var, v, 3); }
BSDE_API void bsdeSetAttr3I(int obj, int var, int x, int y, int z)
	{ int v[4];	v[0]=x; v[1]=y; v[2]=z;
		bsdeSetAttrIV(obj, var, v, 3); }
BSDE_API void bsdeSetAttr3F(int obj, int var, double x, double y, double z)
	{ bsdeSetAttr3D(obj, var, x, y, z); }

BSDE_API void bsdeSetAttr4D(int obj, int var, double x, double y, double z, double w)
	{ double v[4];	v[0]=x; v[1]=y; v[2]=z; v[3]=w;
		bsdeSetAttrDV(obj, var, v, 4); }
BSDE_API void bsdeSetAttr4I(int obj, int var, int x, int y, int z, int w)
	{ int v[4];	v[0]=x; v[1]=y; v[2]=z; v[3]=w;
		bsdeSetAttrIV(obj, var, v, 4); }
BSDE_API void bsdeSetAttr4F(int obj, int var, double x, double y, double z, double w)
	{ bsdeSetAttr4D(obj, var, x, y, z, w); }


BSDE_API double bsdeGetAttrf(int obj, int var)
	{ return(BSDE_GetAttrF(BSDE_GetWorldSolid(obj), var)); }
BSDE_API void bsdeSetAttrf(int obj, int var, double val)
	{ BSDE_SetAttrF(BSDE_GetWorldSolid(obj), var, val); }

BSDE_API int bsdeGetAttri(int obj, int var)
	{ return(BSDE_GetAttrI(BSDE_GetWorldSolid(obj), var)); }
BSDE_API void bsdeSetAttri(int obj, int var, int val)
	{ BSDE_SetAttrI(BSDE_GetWorldSolid(obj), var, val); }

BSDE_API void bsdeGetAttrfv(int obj, int var, float *val, int n)
	{ BSDE_GetAttrFV(BSDE_GetWorldSolid(obj), var, val, n); }
BSDE_API void bsdeSetAttrfv(int obj, int var, float *val, int n)
	{ BSDE_SetAttrFV(BSDE_GetWorldSolid(obj), var, val, n); }

BSDE_API void bsdeGetAttrdv(int obj, int var, double *val, int n)
	{ BSDE_GetAttrDV(BSDE_GetWorldSolid(obj), var, val, n); }
BSDE_API void bsdeSetAttrdv(int obj, int var, double *val, int n)
	{ BSDE_SetAttrDV(BSDE_GetWorldSolid(obj), var, val, n); }

BSDE_API void bsdeGetAttriv(int obj, int var, int *val, int n)
	{ BSDE_GetAttrIV(BSDE_GetWorldSolid(obj), var, val, n); }
BSDE_API void bsdeSetAttriv(int obj, int var, int *val, int n)
	{ BSDE_SetAttrIV(BSDE_GetWorldSolid(obj), var, val, n); }

BSDE_API void bsdeSetAttribPointer(int obj, int var,
		int size, int type, int stride, void *val, int num)
{
	BSDE_SetAttribPointer(BSDE_GetWorldSolid(obj), var,
		size, type, stride, val, num);
}

BSDE_API void bsdeGetAttr2fv(int obj, int var, float *val)
	{ bsdeGetAttrfv(obj, var, val, 2); }
BSDE_API void bsdeSetAttr2fv(int obj, int var, float *val)
	{ bsdeSetAttrfv(obj, var, val, 2); }
BSDE_API void bsdeGetAttr2dv(int obj, int var, double *val)
	{ bsdeGetAttrdv(obj, var, val, 2); }
BSDE_API void bsdeSetAttr2dv(int obj, int var, double *val)
	{ bsdeSetAttrdv(obj, var, val, 2); }
BSDE_API void bsdeGetAttr2iv(int obj, int var, int *val)
	{ bsdeGetAttriv(obj, var, val, 2); }
BSDE_API void bsdeSetAttr2iv(int obj, int var, int *val)
	{ bsdeSetAttriv(obj, var, val, 2); }

BSDE_API void bsdeGetAttr3fv(int obj, int var, float *val)
	{ bsdeGetAttrfv(obj, var, val, 3); }
BSDE_API void bsdeSetAttr3fv(int obj, int var, float *val)
	{ bsdeSetAttrfv(obj, var, val, 3); }
BSDE_API void bsdeGetAttr3dv(int obj, int var, double *val)
	{ bsdeGetAttrdv(obj, var, val, 3); }
BSDE_API void bsdeSetAttr3dv(int obj, int var, double *val)
	{ bsdeSetAttrdv(obj, var, val, 3); }
BSDE_API void bsdeGetAttr3iv(int obj, int var, int *val)
	{ bsdeGetAttriv(obj, var, val, 3); }
BSDE_API void bsdeSetAttr3iv(int obj, int var, int *val)
	{ bsdeSetAttriv(obj, var, val, 3); }

BSDE_API void bsdeGetAttr4fv(int obj, int var, float *val)
	{ bsdeGetAttrfv(obj, var, val, 4); }
BSDE_API void bsdeSetAttr4fv(int obj, int var, float *val)
	{ bsdeSetAttrfv(obj, var, val, 4); }
BSDE_API void bsdeGetAttr4dv(int obj, int var, double *val)
	{ bsdeGetAttrdv(obj, var, val, 4); }
BSDE_API void bsdeSetAttr4dv(int obj, int var, double *val)
	{ bsdeSetAttrdv(obj, var, val, 4); }
BSDE_API void bsdeGetAttr4iv(int obj, int var, int *val)
	{ bsdeGetAttriv(obj, var, val, 4); }
BSDE_API void bsdeSetAttr4iv(int obj, int var, int *val)
	{ bsdeSetAttriv(obj, var, val, 4); }

BSDE_API void bsdeSetAttr2f(int obj, int var, double x, double y)
	{ double v[4]; v[0]=x; v[1]=y; bsdeSetAttrdv(obj, var, v, 2); }
BSDE_API void bsdeSetAttr2i(int obj, int var, int x, int y)
	{ int v[4]; v[0]=x; v[1]=y; bsdeSetAttriv(obj, var, v, 2); }

BSDE_API void bsdeSetAttr3f(int obj, int var, double x, double y, double z)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; bsdeSetAttrdv(obj, var, v, 3); }
BSDE_API void bsdeSetAttr3i(int obj, int var, int x, int y, int z)
	{ int v[4]; v[0]=x; v[1]=y; v[2]=z; bsdeSetAttriv(obj, var, v, 3); }

BSDE_API void bsdeSetAttr4f(int obj, int var, double x, double y, double z, double w)
	{ double v[4];	v[0]=x; v[1]=y; v[2]=z; v[3]=w;
		bsdeSetAttrdv(obj, var, v, 4); }
BSDE_API void bsdeSetAttr4i(int obj, int var, int x, int y, int z, int w)
	{ int v[4];	v[0]=x; v[1]=y; v[2]=z; v[3]=w;
		bsdeSetAttriv(obj, var, v, 4); }


BSDE_API int bsdeNewJoint(int type)
{
	BSDE_Joint *tmp;
	int i;

	tmp=BSDE_NewJoint(type);
	i=BSDE_AddJointWorld(BSDE_GetWorld(), tmp);
	return(i);
}

BSDE_API void bsdeBindJointSolids(int obj, int obja, int objb)
{
	BSDE_BindJointSolids(BSDE_GetWorld()->joints[obj],
		BSDE_GetWorld()->solids[obja],
		BSDE_GetWorld()->solids[objb]);
}

BSDE_API double bsdeGetJointAttrF(int obj, int var)
{
	double f;

	f=BSDE_GetJointAttrF(BSDE_GetWorld()->joints[obj], var);
	return(f);
}

BSDE_API void bsdeSetJointAttrF(int obj, int var, double val)
{
	BSDE_SetJointAttrF(BSDE_GetWorld()->joints[obj], var, val);
}

BSDE_API int bsdeGetJointAttrI(int obj, int var)
{
	int i;
	i=BSDE_GetJointAttrI(BSDE_GetWorld()->joints[obj], var);
	return(i);
}

BSDE_API void bsdeSetJointAttrI(int obj, int var, int val)
{
	BSDE_SetJointAttrI(BSDE_GetWorld()->joints[obj], var, val);
}

BSDE_API void bsdeGetJointAttrFV(int obj, int var, float *val, int n)
{
	BSDE_GetJointAttrFV(BSDE_GetWorld()->joints[obj],
		var, val, n);
}

BSDE_API void bsdeSetJointAttrFV(int obj, int var, float *val, int n)
{
	BSDE_SetJointAttrFV(BSDE_GetWorld()->joints[obj],
		var, val, n);
}

BSDE_API void bsdeGetJointAttrDV(int obj, int var, double *val, int n)
{
	BSDE_GetJointAttrDV(BSDE_GetWorld()->joints[obj],
		var, val, n);
}

BSDE_API void bsdeSetJointAttrDV(int obj, int var, double *val, int n)
{
	BSDE_SetJointAttrDV(BSDE_GetWorld()->joints[obj],
		var, val, n);
}

BSDE_API double bsdeGetJointf(int obj, int var)
	{ return(BSDE_GetJointAttrF(BSDE_GetWorld()->joints[obj], var)); }
BSDE_API void bsdeSetJointf(int obj, int var, double val)
	{ BSDE_SetJointAttrF(BSDE_GetWorld()->joints[obj], var, val); }

BSDE_API int bsdeGetJointi(int obj, int var)
	{ return(BSDE_GetJointAttrI(BSDE_GetWorld()->joints[obj], var)); }
BSDE_API void bsdeSetJointi(int obj, int var, int val)
	{ BSDE_SetJointAttrI(BSDE_GetWorld()->joints[obj], var, val); }

BSDE_API void bsdeGetJointfv(int obj, int var, float *val, int n)
	{ BSDE_GetJointAttrFV(BSDE_GetWorld()->joints[obj], var, val, n); }
BSDE_API void bsdeSetJointfv(int obj, int var, float *val, int n)
	{ BSDE_SetJointAttrFV(BSDE_GetWorld()->joints[obj], var, val, n); }

BSDE_API void bsdeGetJointdv(int obj, int var, double *val, int n)
	{ BSDE_GetJointAttrDV(BSDE_GetWorld()->joints[obj], var, val, n); }
BSDE_API void bsdeSetJointdv(int obj, int var, double *val, int n)
	{ BSDE_SetJointAttrDV(BSDE_GetWorld()->joints[obj], var, val, n); }

BSDE_API void bsdeSetJoint2f(int obj, int var, double x, double y)
	{ double v[4]; v[0]=x; v[1]=y; bsdeSetJointdv(obj, var, v, 2); }
BSDE_API void bsdeSetJoint3f(int obj, int var, double x, double y, double z)
	{ double v[4]; v[0]=x; v[1]=y; v[2]=z; bsdeSetJointdv(obj, var, v, 3); }
BSDE_API void bsdeSetJoint4f(int obj, int var, double x, double y, double z, double w)
	{ double v[4];	v[0]=x; v[1]=y; v[2]=z; v[3]=w;
		bsdeSetJointdv(obj, var, v, 4); }

BSDE_API void bsdeSetJointCubic(int obj,
	double vx, double vy, double sx, double sy,
	double fa, double fb, double fc, double fd)
{
	BSDE_SetJointCubic(BSDE_GetWorld()->joints[obj],
		vx, vy, sx, sy, fa, fb, fc, fd);
}

BSDE_API void bsdeSetJointQuadratic(int obj,
	double vx, double vy, double sx, double sy,
	double fa, double fb, double fc)
{
	BSDE_SetJointCubic(BSDE_GetWorld()->joints[obj],
		vx, vy, sx, sy, 0, fa, fb, fc);
}

BSDE_API void bsdeBegin(int type)
	{ BSDE_Begin(BSDE_GetWorld(), type); }
BSDE_API int bsdeEnd()
	{ return(BSDE_End(BSDE_GetWorld())); }

BSDE_API void bsdeVertex3f(double x, double y, double z)
{
	BSDE_BuildEmitVector(x, y, z, 0);
	BSDE_BuildEmitInt(BSDE_TAG_VERTEX);
}

BSDE_API void bsdeVertex4f(double x, double y, double z, double w)
{
	BSDE_BuildEmitVector(x, y, z, w);
	BSDE_BuildEmitInt(BSDE_TAG_VERTEX);
}

BSDE_API void bsdeVertex3d(double x, double y, double z) { bsdeVertex3f(x, y, z); }
BSDE_API void bsdeVertex4d(double x, double y, double z, double w)
	{ bsdeVertex4f(x, y, z, w); }
BSDE_API void bsdeVertex3fv(float *v) { bsdeVertex3f(v[0], v[1], v[2]); }
BSDE_API void bsdeVertex4fv(float *v) { bsdeVertex4f(v[0], v[1], v[2], v[3]); }
BSDE_API void bsdeVertex3dv(double *v) { bsdeVertex3f(v[0], v[1], v[2]); }
BSDE_API void bsdeVertex4dv(double *v) { bsdeVertex4f(v[0], v[1], v[2], v[3]); }

BSDE_API void bsdeNormal3f(double x, double y, double z)
{
	BSDE_BuildEmitVector(x, y, z, 0);
	BSDE_BuildEmitInt(BSDE_TAG_NORMAL);
}

BSDE_API void bsdeNormal4f(double x, double y, double z, double w)
{
	BSDE_BuildEmitVector(x, y, z, w);
	BSDE_BuildEmitInt(BSDE_TAG_NORMAL);
}

BSDE_API void bsdeNormal3d(double x, double y, double z) { bsdeNormal3f(x, y, z); }
BSDE_API void bsdeNormal4d(double x, double y, double z, double w)
	{ bsdeNormal4f(x, y, z, w); }
BSDE_API void bsdeNormal3fv(float *v) { bsdeNormal3f(v[0], v[1], v[2]); }
BSDE_API void bsdeNormal4fv(float *v) { bsdeNormal4f(v[0], v[1], v[2], v[3]); }
BSDE_API void bsdeNormal3dv(double *v) { bsdeNormal3f(v[0], v[1], v[2]); }
BSDE_API void bsdeNormal4dv(double *v) { bsdeNormal4f(v[0], v[1], v[2], v[3]); }

BSDE_API void bsdeAttrI(int var, int val)
{
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_I);
	BSDE_BuildEmitInt(var);
	BSDE_BuildEmitInt(val);
}

BSDE_API void bsdeAttrF(int var, double val)
{
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_F);
	BSDE_BuildEmitInt(var);
	BSDE_BuildEmitScalar(val);
}

BSDE_API void bsdeAttrIV(int var, int *val, int num)
{
	int i;

	BSDE_BuildEmitInt(BSDE_TAG_ATTR_IV);
	BSDE_BuildEmitInt(var);
	BSDE_BuildEmitInt(num);

	for(i=0; i<num; i++)
		BSDE_BuildEmitInt(val[i]);
}

BSDE_API void bsdeAttrFV(int var, float *val, int num)
{
	int i;

	BSDE_BuildEmitInt(BSDE_TAG_ATTR_FV);
	BSDE_BuildEmitInt(var);
	BSDE_BuildEmitInt(num);

	for(i=0; i<num; i++)
		BSDE_BuildEmitScalar(val[i]);
}

BSDE_API void bsdeAttrDV(int var, double *val, int num)
{
	int i;

	BSDE_BuildEmitInt(BSDE_TAG_ATTR_FV);
	BSDE_BuildEmitInt(var);
	BSDE_BuildEmitInt(num);

	for(i=0; i<num; i++)
		BSDE_BuildEmitScalar(val[i]);
}


BSDE_API void bsdeAttri(int var, int val)
{
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_I);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitInt(val);
}

BSDE_API void bsdeAttrf(int var, double val)
{
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_F);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitScalar(val);
}

BSDE_API void bsdeAttriv(int var, int *val, int num)
{
	int i;
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_IV);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitInt(num);
	for(i=0; i<num; i++)BSDE_BuildEmitInt(val[i]);
}

BSDE_API void bsdeAttrfv(int var, float *val, int num)
{
	int i;
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_FV);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitInt(num);
	for(i=0; i<num; i++)BSDE_BuildEmitScalar(val[i]);
}

BSDE_API void bsdeAttrdv(int var, double *val, int num)
{
	int i;
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_FV);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitInt(num);
	for(i=0; i<num; i++)BSDE_BuildEmitScalar(val[i]);
}


BSDE_API void bsdeAttr2i(int var, int x, int y)
	{ int v[4]; v[0]=x; v[1]=y; bsdeAttriv(var, v, 2); }
BSDE_API void bsdeAttr3i(int var, int x, int y, int z)
	{ int v[4]; v[0]=x; v[1]=y; v[2]=z; bsdeAttriv(var, v, 3); }
BSDE_API void bsdeAttr4i(int var, int x, int y, int z, int w)
	{ int v[4]; v[0]=x; v[1]=y; v[2]=z; v[3]=w; bsdeAttriv(var, v, 4); }

BSDE_API void bsdeAttr2f(int var, double x, double y)
{
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_2F);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitVector(x, y, 0, 0);
}

BSDE_API void bsdeAttr3f(int var, double x, double y, double z)
{
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_3F);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitVector(x, y, z, 0);
}

BSDE_API void bsdeAttr4f(int var, double x, double y, double z, double w)
{
	BSDE_BuildEmitInt(BSDE_TAG_ATTR_4F);
	BSDE_BuildEmitInt(var); BSDE_BuildEmitVector(x, y, z, w);
}

BSDE_API int bsdePeekMessage()
	{ return(BSDE_MsgPeek()); }
BSDE_API int bsdeNextMessage()
	{ return(BSDE_MsgNext()); }

BSDE_API int bsdeGetMessagei(int var)
	{ return(BSDE_MsgGetAttrI(var)); }
BSDE_API double bsdeGetMessagef(int var)
	{ return(BSDE_MsgGetAttrF(var)); }

BSDE_API void bsdeGetMessageiv(int var, int *val, int n)
	{ BSDE_MsgGetAttrIV(var, val, n); }
BSDE_API void bsdeGetMessagefv(int var, float *val, int n)
	{ BSDE_MsgGetAttrFV(var, val, n); }
BSDE_API void bsdeGetMessagedv(int var, double *val, int n)
	{ BSDE_MsgGetAttrDV(var, val, n); }

BSDE_API void bsdeGetMessage2iv(int var, int *val)	{ bsdeGetMessageiv(var, val, 2); }
BSDE_API void bsdeGetMessage2fv(int var, float *val)	{ bsdeGetMessagefv(var, val, 2); }
BSDE_API void bsdeGetMessage2dv(int var, double *val)	{ bsdeGetMessagedv(var, val, 2); }
BSDE_API void bsdeGetMessage3iv(int var, int *val)	{ bsdeGetMessageiv(var, val, 3); }
BSDE_API void bsdeGetMessage3fv(int var, float *val)	{ bsdeGetMessagefv(var, val, 3); }
BSDE_API void bsdeGetMessage3dv(int var, double *val)	{ bsdeGetMessagedv(var, val, 3); }
BSDE_API void bsdeGetMessage4iv(int var, int *val)	{ bsdeGetMessageiv(var, val, 4); }
BSDE_API void bsdeGetMessage4fv(int var, float *val)	{ bsdeGetMessagefv(var, val, 4); }
BSDE_API void bsdeGetMessage4dv(int var, double *val)	{ bsdeGetMessagedv(var, val, 4); }


BSDE_API int bsdeQueryLineDV(int *hdl, int mhdl, double *start, double *end)
{
	bsde_real sp[4], ep[4];
	BSDE_World *world;
	BSDE_Solid *cur;
	int i, j, k;

	sp[0]=start[0];	sp[1]=start[1];	sp[2]=start[2];
	ep[0]=end[0];	ep[1]=end[1];	ep[2]=end[2];

	world=BSDE_GetWorld();

	k=0;
	for(i=0; i<world->n_solids; i++)
	{
		cur=world->solids[i];
		if(!cur)continue;
		if(cur->moveflags&BSDE_MOVEFL_HIDDEN)
			continue;

		j=BSDE_LineCollide(cur, sp, ep);

		if(k>=mhdl)break;
		if(j)hdl[k++]=i;
	}

	return(k);
}

BSDE_API int bsdeQueryBoxDV(int *hdl, int mhdl, double *mins, double *maxs)
{
	bsde_real m[4], n[4];
	BSDE_World *world;
	BSDE_Solid *cur;
	int i, j, k;

	world=BSDE_GetWorld();

	m[0]=mins[0];	m[1]=mins[1];	m[2]=mins[2];
	n[0]=maxs[0];	n[1]=maxs[1];	n[2]=maxs[2];

	k=0;
	for(i=0; i<world->n_solids; i++)
	{
		cur=world->solids[i];
		if(!cur)continue;
		if(cur->moveflags&BSDE_MOVEFL_HIDDEN)
			continue;

		j=BSDE_BoxCollide(cur, m, n);

		if(k>=mhdl)break;
		if(j)hdl[k++]=i;
	}

	return(k);
}

BSDE_API int bsdeQueryHullDV(int *hdl, int mhdl, double *nv, int nnv)
{
	bsde_real n[64*4];
	BSDE_World *world;
	BSDE_Solid *cur;
	int i, j, k;

	world=BSDE_GetWorld();

	for(i=0; i<(nnv*4); i++)n[i]=nv[i];

	k=0;
	for(i=0; i<world->n_solids; i++)
	{
		cur=world->solids[i];
		if(!cur)continue;
		if(cur->moveflags&BSDE_MOVEFL_HIDDEN)
			continue;

		j=BSDE_HullCollide(cur, n, nnv);

		if(k>=mhdl)break;
		if(j)hdl[k++]=i;
	}

	return(k);
}

BSDE_API int bsdeQueryLineFV(int *hdl, int mhdl, float *start, float *end)
{
	double sp[4], ep[4];
	int i;

	sp[0]=start[0];	sp[1]=start[1];	sp[2]=start[2];
	ep[0]=end[0];	ep[1]=end[1];	ep[2]=end[2];

	i=bsdeQueryLineDV(hdl, mhdl, sp, ep);
	return(i);
}

BSDE_API int bsdeQueryBoxFV(int *hdl, int mhdl, float *mins, float *maxs)
{
	double m[4], n[4];
	int i;

	m[0]=mins[0];	m[1]=mins[1];	m[2]=mins[2];
	n[0]=maxs[0];	n[1]=maxs[1];	n[2]=maxs[2];

	i=bsdeQueryBoxDV(hdl, mhdl, m, n);
	return(i);
}

BSDE_API int bsdeQueryHullFV(int *hdl, int mhdl, float *nv, int nnv)
{
	double n[64*4];
	int i;

	for(i=0; i<(nnv*4); i++)n[i]=nv[i];
	i=bsdeQueryHullDV(hdl, mhdl, n, nnv);
	return(i);
}

} //extern "C"

