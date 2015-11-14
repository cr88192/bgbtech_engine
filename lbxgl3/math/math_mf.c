#include <lbxgl2.h>


//3x3 matrices

void Mat3F_MatMult(float *a, float *b, float *c)
{
	c[0]=a[0]*b[0] + a[1]*b[3] + a[2]*b[6];
	c[1]=a[0]*b[1] + a[1]*b[4] + a[2]*b[7];
	c[2]=a[0]*b[2] + a[1]*b[5] + a[2]*b[8];
	c[3]=a[3]*b[0] + a[4]*b[3] + a[5]*b[6];
	c[4]=a[3]*b[1] + a[4]*b[4] + a[5]*b[7];
	c[5]=a[3]*b[2] + a[4]*b[5] + a[5]*b[8];
	c[6]=a[6]*b[0] + a[7]*b[3] + a[8]*b[6];
	c[7]=a[6]*b[1] + a[7]*b[4] + a[8]*b[7];
	c[8]=a[6]*b[2] + a[7]*b[5] + a[8]*b[8];
}

void Mat3F_Transpose(float *a, float *b)
{
	b[0]=a[0]; b[1]=a[3]; b[2]=a[6];
	b[3]=a[1]; b[4]=a[4]; b[5]=a[7];
	b[6]=a[2]; b[7]=a[5]; b[8]=a[8];
}


//4x4 matrices

void Mat4F_MatMult(float *a, float *b, float *c)
{
	c[0] =a[0]*b[0]  + a[1]*b[4]  + a[2]*b[8]   + a[3]*b[12];
	c[1] =a[0]*b[1]  + a[1]*b[5]  + a[2]*b[9]   + a[3]*b[13];
	c[2] =a[0]*b[2]  + a[1]*b[6]  + a[2]*b[10]  + a[3]*b[14];
	c[3] =a[0]*b[3]  + a[1]*b[7]  + a[2]*b[11]  + a[3]*b[15];
	c[4] =a[4]*b[0]  + a[5]*b[4]  + a[6]*b[8]   + a[7]*b[12];
	c[5] =a[4]*b[1]  + a[5]*b[5]  + a[6]*b[9]   + a[7]*b[13];
	c[6] =a[4]*b[2]  + a[5]*b[6]  + a[6]*b[10]  + a[7]*b[14];
	c[7] =a[4]*b[3]  + a[5]*b[7]  + a[6]*b[11]  + a[7]*b[15];
	c[8] =a[8]*b[0]  + a[9]*b[4]  + a[10]*b[8]  + a[11]*b[12];
	c[9] =a[8]*b[1]  + a[9]*b[5]  + a[10]*b[9]  + a[11]*b[13];
	c[10]=a[8]*b[2]  + a[9]*b[6]  + a[10]*b[10] + a[11]*b[14];
	c[11]=a[8]*b[3]  + a[9]*b[7]  + a[10]*b[11] + a[11]*b[15];
	c[12]=a[12]*b[0] + a[13]*b[4] + a[14]*b[8]  + a[15]*b[12];
	c[13]=a[12]*b[1] + a[13]*b[5] + a[14]*b[9]  + a[15]*b[13];
	c[14]=a[12]*b[2] + a[13]*b[6] + a[14]*b[10] + a[15]*b[14];
	c[15]=a[12]*b[3] + a[13]*b[7] + a[14]*b[11] + a[15]*b[15];
}

void Mat4F_Transpose(float *a, float *b)
{
	b[0]=a[0];  b[1]=a[4];  b[2]=a[8];   b[3]=a[12];
	b[4]=a[1];  b[5]=a[5];  b[6]=a[9];   b[7]=a[13];
	b[8]=a[2];  b[9]=a[6];  b[10]=a[10]; b[11]=a[14];
	b[12]=a[3]; b[13]=a[7]; b[14]=a[11]; b[15]=a[15];
}


//3x3 matrices inside 3x4 matrices

int Mat34F_MatMult(float *a, float *b, float *c)
{
	c[0] =a[0]*b[0] + a[1]*b[4] + a[2]*b[8];
	c[1] =a[0]*b[1] + a[1]*b[5] + a[2]*b[9];
	c[2] =a[0]*b[2] + a[1]*b[6] + a[2]*b[10];
	c[4] =a[4]*b[0] + a[5]*b[4] + a[6]*b[8];
	c[5] =a[4]*b[1] + a[5]*b[5] + a[6]*b[9];
	c[6] =a[4]*b[2] + a[5]*b[6] + a[6]*b[10];
	c[8] =a[8]*b[0] + a[9]*b[4] + a[10]*b[8];
	c[9] =a[8]*b[1] + a[9]*b[5] + a[10]*b[9];
	c[10]=a[8]*b[2] + a[9]*b[6] + a[10]*b[10];
}

void Mat34F_Transpose(float *a, float *b)
{
	b[0]=a[0];  b[1]=a[4];  b[2]=a[8];
	b[4]=a[1];  b[5]=a[5];  b[6]=a[9];
	b[8]=a[2];  b[9]=a[6];  b[10]=a[10];
}