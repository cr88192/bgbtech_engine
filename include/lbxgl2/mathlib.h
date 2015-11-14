#ifndef __LBXGL_MATHLIB_H__
#define __LBXGL_MATHLIB_H__

//float
void MatNF_MatMult(float *a, int ah, int aw, float *b, int bh, int bw, float *c);
void MatNF_MatMultVector(float *a, float *b, float *c, int n);
void MatNF_MatMultScaler(float *a, float b, float *c, int n);
void MatNF_ResizeMatrix(float *a, int ah, int aw, float *b, int bh, int bw);
void MatNF_FillIdentity(float *a, int n);
void MatNF_Copy(float *a, float *b, int h, int w);
void Vec16F_RotateMatrix(float *amat, float *angles, float *bmat);
void MatNF_GetLowerMatrix(float *a, float *b, int n);
void MatNF_GetUpperMatrix(float *a, float *b, int n);
void Vec16F_TranslateMatrix(float *amat, float *pos, float *bmat);

void MatNF_SwapOrder(float *a, float *b, int n);
void MatNF_Inverse(float *a, float *b, int n);

void VecNF_Copy(float *a, float *b, int n);
void VecNF_ResizeVector(float *a, int an, float *b, int bn);
float VecNF_DotProduct(float *a, float *b, int n);
float VecNF_Length(float *a, int n);
float VecNF_Normalize(float *a, float *b, int n);
float VecNF_Distance(float *a, float *b, int n);
void VecNF_Add(float *a, float *b, float *c, int n);
void VecNF_Subtract(float *a, float *b, float *c, int n);
void VecNF_Scale(float *a, float b, float *c, int n);
void VecNF_ScaleAdd(float *a, float b, float *c, float *d, int n);
void VecNF_Zero(float *a, int n);
void VecNF_Const(float *a, float b, int n);
void VecNF_ProjectPointOnPlane(float *d, float *v, float *z, int n);
void Vec3F_CrossProduct (float *v1, float *v2, float *cross);
void Vec3F_ToSphericalCoords(float *a, float *b);
void Vec3F_FromSphericalCoords(float *a, float *b);
void Vec16F_InvertAngles(float *amat, float *bmat);
void Vec3F_RotatePoint(float *p, float *angles);
void Vec3F_CalcAngleVectors (float *angles, float *forward, float *right, float *up);

//double
void MatND_MatMult(double *a, int ah, int aw, double *b, int bh, int bw, double *c);
void MatND_MatMultVector(double *a, double *b, double *c, int n);
void MatND_MatMultScaler(double *a, double b, double *c, int n);
void MatND_ResizeMatrix(double *a, int ah, int aw, double *b, int bh, int bw);
void MatND_FillIdentity(double *a, int n);
void MatND_Copy(double *a, double *b, int h, int w);
void Vec16D_RotateMatrix(double *amat, double *angles, double *bmat);
void MatND_GetLowerMatrix(double *a, double *b, int n);
void MatND_GetUpperMatrix(double *a, double *b, int n);
void Vec16D_TranslateMatrix(double *amat, double *pos, double *bmat);

void MatND_SwapOrder(double *a, double *b, int n);
void MatND_Inverse(double *a, double *b, int n);

void VecND_Copy(double *a, double *b, int n);
void VecND_ResizeVector(double *a, int an, double *b, int bn);
double VecND_DotProduct(double *a, double *b, int n);
double VecND_Length(double *a, int n);
double VecND_Normalize(double *a, double *b, int n);
double VecND_Distance(double *a, double *b, int n);
void VecND_Add(double *a, double *b, double *c, int n);
void VecND_Subtract(double *a, double *b, double *c, int n);
void VecND_Scale(double *a, double b, double *c, int n);
void VecND_ScaleAdd(double *a, double b, double *c, double *d, int n);
void VecND_Zero(double *a, int n);
void VecND_Const(double *a, double b, int n);
void VecND_ProjectPointOnPlane(double *d, double *v, double *z, int n);
void Vec3D_CrossProduct (double *v1, double *v2, double *cross);
void Vec3D_ToSphericalCoords(double *a, double *b);
void Vec3D_FromSphericalCoords(double *a, double *b);
void Vec16D_InvertAngles(double *amat, double *bmat);
void Vec3D_RotatePoint(double *p, double *angles);
void Vec3D_CalcAngleVectors (double *angles, double *forward, double *right, double *up);

float Scaler_Random();
float Scaler_SignRandom();

#define Scalar_Random Scaler_Random
#define Scalar_SignRandom Scaler_SignRandom

#endif
