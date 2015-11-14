#include <lbxgl2.h>

LBXGL_API void Draw_MatMult(float *mat)
{
	float tmat[16];

	MatNF_SwapOrder(mat, tmat, 4);
	pdglMultMatrixf(tmat);
}

LBXGL_API void Draw_RotateQuat(float *rot)
{
	float mat[16], tmat[16];

	QuatF_ToMatrix(rot, mat);
	MatNF_SwapOrder(mat, tmat, 4);
	pdglMultMatrixf(tmat);
}

LBXGL_API void Draw_TranslateRotateQuat(float *org, float *rot)
{
	float mat[16], tmat[16];

	pdglTranslatef(org[0], org[1], org[2]);
	QuatF_ToMatrix(rot, mat);
	MatNF_SwapOrder(mat, tmat, 4);
	pdglMultMatrixf(tmat);
}
