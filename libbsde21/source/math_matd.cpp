#include "bsde_internal.h"


//3x3 matrices

void BSDE_Mat3_MatMult(bsde_real *a, bsde_real *b, bsde_real *c)
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

void BSDE_Mat3_Transpose(bsde_real *a, bsde_real *b)
{
	b[0]=a[0]; b[1]=a[3]; b[2]=a[6];
	b[3]=a[1]; b[4]=a[4]; b[5]=a[7];
	b[6]=a[2]; b[7]=a[5]; b[8]=a[8];
}

void BSDE_Mat3_Copy(bsde_real *a, bsde_real *b)
{
	b[0]=a[0]; b[1]=a[1]; b[2]=a[2];
	b[3]=a[3]; b[4]=a[4]; b[5]=a[5];
	b[6]=a[6]; b[7]=a[7]; b[8]=a[8];
}

int BSDE_Mat3_Equal(bsde_real *a, bsde_real *b)
{
	int i;

	i=(b[0]==a[0]) && (b[1]==a[1]) && (b[2]==a[2]) &&
	  (b[3]==a[3]) && (b[4]==a[4]) && (b[5]==a[5]) &&
	  (b[6]==a[6]) && (b[7]==a[7]) && (b[8]==a[8]);
	return(i);
}

void BSDE_Mat3_Scale(bsde_real *a, bsde_real b, bsde_real *c)
{
	c[0]=a[0]*b; c[1]=a[1]*b; c[2]=a[2]*b;
	c[3]=a[3]*b; c[4]=a[4]*b; c[5]=a[5]*b;
	c[6]=a[6]*b; c[7]=a[7]*b; c[8]=a[8]*b;
}


void BSDE_Mat3_TransformVector(bsde_real *dir, bsde_real *xform, bsde_real *v)
{
	v[0]=(dir[0]*xform[0])+(dir[1]*xform[3])+(dir[2]*xform[6]);
	v[1]=(dir[0]*xform[1])+(dir[1]*xform[4])+(dir[2]*xform[7]);
	v[2]=(dir[0]*xform[2])+(dir[1]*xform[5])+(dir[2]*xform[8]);
}

void BSDE_Mat3_TransformVectorT(bsde_real *dir, bsde_real *xform, bsde_real *v)
{
	v[0]=(dir[0]*xform[0])+(dir[1]*xform[1])+(dir[2]*xform[2]);
	v[1]=(dir[0]*xform[3])+(dir[1]*xform[4])+(dir[2]*xform[5]);
	v[2]=(dir[0]*xform[6])+(dir[1]*xform[7])+(dir[2]*xform[8]);
}

void BSDE_Mat3_Normalize(bsde_real *rot)
{
	V3_NORMALIZE(rot+0, rot+0);
	V3_ADDSCALE(rot+3, rot+0, -V3_DOT(rot+3, rot+0), rot+3);
	V3_NORMALIZE(rot+3, rot+3);
	V3_CROSS(rot+0, rot+3, rot+6);
}

//4x4 matrices

void BSDE_Mat4_Identity(bsde_real *b)
{
	b[0]=1;  b[1]=0;  b[2]=0;  b[3]=0;
	b[4]=0;  b[5]=1;  b[6]=0;  b[7]=0;
	b[8]=0;  b[9]=0;  b[10]=1; b[11]=0;
	b[12]=0; b[13]=0; b[14]=0; b[15]=1;
}

void BSDE_Mat4_Copy(bsde_real *a, bsde_real *b)
{
	b[0]=a[0];   b[1]=a[1];   b[2]=a[2];   b[3]=a[3];
	b[4]=a[4];   b[5]=a[5];   b[6]=a[6];   b[7]=a[7];
	b[8]=a[8];   b[9]=a[9];   b[10]=a[10]; b[11]=a[11];
	b[12]=a[12]; b[13]=a[13]; b[14]=a[14]; b[15]=a[15];
}

int BSDE_Mat4_Equal(bsde_real *a, bsde_real *b)
{
	int i;

	i=(b[0] ==a[0]) &&(b[1] ==a[1]) &&(b[2] ==a[2]) &&(b[3] ==a[3]) &&
	  (b[4] ==a[4]) &&(b[5] ==a[5]) &&(b[6] ==a[6]) &&(b[7] ==a[7]) &&
	  (b[8] ==a[8]) &&(b[9] ==a[9]) &&(b[10]==a[10])&&(b[11]==a[11]) &&
	  (b[12]==a[12])&&(b[13]==a[13])&&(b[14]==a[14])&&(b[15]==a[15]);
	return(i);
}

void BSDE_Mat4_MatMult(bsde_real *a, bsde_real *b, bsde_real *c)
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

void BSDE_Mat4_Transpose(bsde_real *a, bsde_real *b)
{
	b[0]=a[0];  b[1]=a[4];  b[2]=a[8];   b[3]=a[12];
	b[4]=a[1];  b[5]=a[5];  b[6]=a[9];   b[7]=a[13];
	b[8]=a[2];  b[9]=a[6];  b[10]=a[10]; b[11]=a[14];
	b[12]=a[3]; b[13]=a[7]; b[14]=a[11]; b[15]=a[15];
}

void BSDE_Mat4_InverseTransform(bsde_real *a, bsde_real *b)
{
	b[0]=a[0];	b[1]=a[4];	b[2]=a[8];	b[3]=0;
	b[4]=a[1];	b[5]=a[5];	b[6]=a[9];	b[7]=0;
	b[8]=a[2];	b[9]=a[6];	b[10]=a[10];	b[11]=0;
	b[12]=-a[12];	b[13]=-a[13];	b[14]=-a[14];	b[15]=1;

	b[12]=-V3_DOT(a+3*4, b+0*4);
	b[13]=-V3_DOT(a+3*4, b+1*4);
	b[14]=-V3_DOT(a+3*4, b+2*4);
}

// general matrices

void BSDE_MatN_MatMult(bsde_real *a, int ah, int aw, bsde_real *b, int bh, int bw, bsde_real *c)
{
	int i, j, k;

	if(aw!=bh)return;

	for(i=0; i<(ah*bw); i++)c[i]=0;
	for(i=0; i<ah; i++)
		for(j=0; j<bw; j++)
			for(k=0; k<aw; k++)
				c[i*bw+j]+=a[i*aw+k]*b[k*bw+j];
}

void BSDE_MatN_Inverse(bsde_real *a, bsde_real *b, int n)
{
	static bsde_real a2buf[8*8*2], a3buf[16];
	bsde_real *a2p[8], *a3p;
	bsde_real f;
	int i, j, k;

	for(i=0; i<n*n*2; i++)a2buf[i]=0;
	for(i=0; i<n*n; i++)b[i]=0;

	for(i=0; i<n; i++)a2p[i]=&a2buf[i*n*2];
	for(i=0; i<n; i++)
		for(j=0; j<n; j++)a2p[i][j]=a[i*n+j];
	for(i=0; i<n; i++)a2p[i][i+n]=1.0;

	for(i=0; i<n; i++)
	{
		if(a2p[i][i]==0) /* search for a new pivot in a lower row */
		{
			for(j=n-1; j>i; j--)if(a2p[j][i]!=0)
			{
				a3p=a2p[i];
				a2p[i]=a2p[j];
				a2p[j]=a3p;
			}
			if(a2p[i][i]==0)
			{
				printf("no pivot\n");
				return; /* no pivot */
			}
		}
		f=1.0/a2p[i][i];
		for(j=0; j<2*n; j++)a2p[i][j]*=f;
		for(j=0; j<n; j++)if(i!=j)
		{
			f=-a2p[j][i];
			for(k=0; k<2*n; k++)a3buf[k]=a2p[i][k]*f;
			for(k=0; k<2*n; k++)a2p[j][k]+=a3buf[k];
		}
	}

	for(i=0; i<n; i++)
		for(j=0; j<n; j++)
			b[i*n+j]=a2p[i][n+j];
}
