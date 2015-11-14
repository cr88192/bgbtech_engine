#include "bsde_internal.h"

void BSDE_SolidPlane_TransformNormal(
	bsde_real *dir, bsde_real *xform, bsde_real *norm)
{
	bsde_real f;

	norm[0]=(dir[0]*xform[0])+(dir[1]*xform[4])+(dir[2]*xform[8]);
	norm[1]=(dir[0]*xform[1])+(dir[1]*xform[5])+(dir[2]*xform[9]);
	norm[2]=(dir[0]*xform[2])+(dir[1]*xform[6])+(dir[2]*xform[10]);

	f=(norm[0]*xform[12])+(norm[1]*xform[13])+(norm[2]*xform[14]);
	norm[3]=dir[3]+f;
}

int BSDE_SolidPlane_PointCollide(bsde_real *dir, bsde_real *xform, bsde_real *point)
{
	bsde_real norm[4], f;

	norm[0]=(dir[0]*xform[0])+(dir[1]*xform[4])+(dir[2]*xform[8]);
	norm[1]=(dir[0]*xform[1])+(dir[1]*xform[5])+(dir[2]*xform[9]);
	norm[2]=(dir[0]*xform[2])+(dir[1]*xform[6])+(dir[2]*xform[10]);

	f=(norm[0]*xform[12])+(norm[1]*xform[13])+(norm[2]*xform[14]);
	norm[3]=dir[3]+f;

	f=V3_NDOT(point, norm);
	return(f<0);
}
