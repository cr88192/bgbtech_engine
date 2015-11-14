#include <bgbdy.h>

BGBDY_API mat2 dtyMat2vf(float *v)
{
	mat2 t;

	t.v[0]=v[0]; t.v[1]=v[1]; t.v[2]=v[2]; t.v[3]=v[3];
	return(t);
}

BGBDY_API mat3 dtyMat3vf(float *v)
{
	mat3 t;

	t.v[0]=v[0]; t.v[1]=v[1]; t.v[ 2]=v[2]; t.v[ 3]=0;
	t.v[4]=v[3]; t.v[5]=v[4]; t.v[ 6]=v[5]; t.v[ 7]=0;
	t.v[8]=v[6]; t.v[9]=v[7]; t.v[10]=v[8]; t.v[11]=0;
	return(t);
}

BGBDY_API mat4 dtyMat4vf(float *v)
{
	mat4 t;

	t.v[ 0]=v[ 0]; t.v[ 1]=v[ 1]; t.v[ 2]=v[ 2]; t.v[ 3]=v[ 3];
	t.v[ 4]=v[ 4]; t.v[ 5]=v[ 5]; t.v[ 6]=v[ 6]; t.v[ 7]=v[ 7];
	t.v[ 8]=v[ 8]; t.v[ 9]=v[ 9]; t.v[10]=v[10]; t.v[11]=v[11];
	t.v[12]=v[12]; t.v[13]=v[13]; t.v[14]=v[14]; t.v[15]=v[15];
	return(t);
}


BGBDY_API mat2 dtyMat2f(float a, float b, float c, float d)
{
	mat2 t;
	t.v[0]=a; t.v[1]=b; t.v[2]=c; t.v[3]=d;
	return(t);
}

BGBDY_API mat3 dtyMat3f(
	float a, float b, float c,
	float d, float e, float f,
	float g, float h, float i)
{
	mat3 t;
	t.v[0]=a; t.v[1]=b; t.v[ 2]=c; t.v[ 3]=0;
	t.v[4]=d; t.v[5]=e; t.v[ 6]=f; t.v[ 7]=0;
	t.v[8]=g; t.v[9]=h; t.v[10]=i; t.v[11]=0;
	return(t);
}

BGBDY_API mat4 dtyMat4f(
	float a, float b, float c, float d,
	float e, float f, float g, float h,
	float i, float j, float k, float l,
	float m, float n, float o, float p)
{
	mat4 t;
	t.v[ 0]=a; t.v[ 1]=b; t.v[ 2]=c; t.v[ 3]=d;
	t.v[ 4]=e; t.v[ 5]=f; t.v[ 6]=g; t.v[ 7]=h;
	t.v[ 8]=i; t.v[ 9]=j; t.v[10]=k; t.v[11]=l;
	t.v[12]=m; t.v[13]=h; t.v[14]=o; t.v[15]=p;
	return(t);
}


BGBDY_API mat2 dtyMat2Add(mat2 a, mat2 b)
{
	mat2 c;

	vfvec4(c.v, v4add(vec4vf(a.v), vec4vf(b.v)));

//	c.v[0]=a.v[0]+b.v[0];
//	c.v[1]=a.v[1]+b.v[1];
//	c.v[2]=a.v[2]+b.v[2];
//	c.v[3]=a.v[3]+b.v[3];
	return(c);
}

BGBDY_API mat2 dtyMat2Sub(mat2 a, mat2 b)
{
	mat2 c;

	vfvec4(c.v, v4sub(vec4vf(a.v), vec4vf(b.v)));
//	c.v[0]=a.v[0]-b.v[0];
//	c.v[1]=a.v[1]-b.v[1];
//	c.v[2]=a.v[2]-b.v[2];
//	c.v[3]=a.v[3]-b.v[3];
	return(c);
}

BGBDY_API mat2 dtyMat2Mul(mat2 a, mat2 b)
{
	vec4 a0, a1, b0, b1, t;
	mat2 c;

	t=vec4vf(a.v); a0=v4shuf(t, 0xA0); a1=v4shuf(t, 0xF5);
	t=vec4vf(b.v); b0=v4shuf(t, 0x44); b1=v4shuf(t, 0x66);

	t=v4add(v4mul(a0, b0), v4mul(a1, b1));
	vfvec4(c.v, t);

//	c.v[0]=a.v[0]*b.v[0]+a.v[1]*b.v[2];
//	c.v[1]=a.v[0]*b.v[1]+a.v[1]*b.v[3];
//	c.v[2]=a.v[2]*b.v[0]+a.v[3]*b.v[2];
//	c.v[3]=a.v[2]*b.v[1]+a.v[3]*b.v[3];
	return(c);
}


BGBDY_API mat3 dtyMat3Add(mat3 a, mat3 b)
{
	mat3 c;

	vfvec4(c.v+0, v4add(vec4vf(a.v+0), vec4vf(b.v+0)));
	vfvec4(c.v+4, v4add(vec4vf(a.v+4), vec4vf(b.v+4)));
	vfvec4(c.v+8, v4add(vec4vf(a.v+8), vec4vf(b.v+8)));

#if 0
	c.v[ 0]=a.v[ 0]+b.v[ 0];
	c.v[ 1]=a.v[ 1]+b.v[ 1];
	c.v[ 2]=a.v[ 2]+b.v[ 2];
	c.v[ 4]=a.v[ 4]+b.v[ 4];
	c.v[ 5]=a.v[ 5]+b.v[ 5];
	c.v[ 6]=a.v[ 6]+b.v[ 6];
	c.v[ 8]=a.v[ 8]+b.v[ 8];
	c.v[ 9]=a.v[ 9]+b.v[ 9];
	c.v[10]=a.v[10]+b.v[10];
#endif

	return(c);
}

BGBDY_API mat3 dtyMat3Sub(mat3 a, mat3 b)
{
	mat3 c;

	vfvec4(c.v+0, v4sub(vec4vf(a.v+0), vec4vf(b.v+0)));
	vfvec4(c.v+4, v4sub(vec4vf(a.v+4), vec4vf(b.v+4)));
	vfvec4(c.v+8, v4sub(vec4vf(a.v+8), vec4vf(b.v+8)));

#if 0
	c.v[ 0]=a.v[ 0]-b.v[ 0];
	c.v[ 1]=a.v[ 1]-b.v[ 1];
	c.v[ 2]=a.v[ 2]-b.v[ 2];
	c.v[ 4]=a.v[ 4]-b.v[ 4];
	c.v[ 5]=a.v[ 5]-b.v[ 5];
	c.v[ 6]=a.v[ 6]-b.v[ 6];
	c.v[ 8]=a.v[ 8]-b.v[ 8];
	c.v[ 9]=a.v[ 9]-b.v[ 9];
	c.v[10]=a.v[10]-b.v[10];
#endif
	return(c);
}

BGBDY_API mat3 dtyMat3Mul(mat3 a, mat3 b)
{
	vec4 b0, b1, b2;
	vec4 t0, t1, t2;
	vec4 c0, c1, c2;
	mat3 c;

	b0=vec4vf(b.v+ 0);
	b1=vec4vf(b.v+ 4);
	b2=vec4vf(b.v+ 8);

//	t0=v4scale(b0, a.v[0]);	t1=v4scale(b1, a.v[1]);
//	t2=v4scale(b2, a.v[2]); c0=v4add(v4add(t0, t1), t2);

//	t0=v4scale(b0, a.v[4]);	t1=v4scale(b1, a.v[5]);
//	t2=v4scale(b2, a.v[6]); c1=v4add(v4add(t0, t1), t2);

//	t0=v4scale(b0, a.v[ 8]); t1=v4scale(b1, a.v[ 9]);
//	t2=v4scale(b2, a.v[10]); c2=v4add(v4add(t0, t1), t2);

	c0=v4scaleadd3(b0, a.v[ 0], b1, a.v[ 1], b2, a.v[ 2]);
	c1=v4scaleadd3(b0, a.v[ 4], b1, a.v[ 5], b2, a.v[ 6]);
	c2=v4scaleadd3(b0, a.v[ 8], b1, a.v[ 9], b2, a.v[10]);

	vfvec4(c.v+ 0, c0);
	vfvec4(c.v+ 4, c1);
	vfvec4(c.v+ 8, c2);

	return(c);
#if 0
	mat3 c;

	c.v[ 0]=a.v[ 0]*b.v[ 0] + a.v[ 1]*b.v[ 4] + a.v[ 2]*b.v[ 8];
	c.v[ 1]=a.v[ 0]*b.v[ 1] + a.v[ 1]*b.v[ 5] + a.v[ 2]*b.v[ 9];
	c.v[ 2]=a.v[ 0]*b.v[ 2] + a.v[ 1]*b.v[ 6] + a.v[ 2]*b.v[10];

	c.v[ 4]=a.v[ 4]*b.v[ 0] + a.v[ 5]*b.v[ 4] + a.v[ 6]*b.v[ 8];
	c.v[ 5]=a.v[ 4]*b.v[ 1] + a.v[ 5]*b.v[ 5] + a.v[ 6]*b.v[ 9];
	c.v[ 6]=a.v[ 4]*b.v[ 2] + a.v[ 5]*b.v[ 6] + a.v[ 6]*b.v[10];

	c.v[ 8]=a.v[ 8]*b.v[ 0] + a.v[ 9]*b.v[ 4] + a.v[10]*b.v[ 8];
	c.v[ 9]=a.v[ 8]*b.v[ 1] + a.v[ 9]*b.v[ 5] + a.v[10]*b.v[ 9];
	c.v[10]=a.v[ 8]*b.v[ 2] + a.v[ 9]*b.v[ 6] + a.v[10]*b.v[10];

	return(c);
#endif
}


BGBDY_API mat4 dtyMat4Add(mat4 a, mat4 b)
{
	mat4 c;

	vfvec4(c.v+ 0, v4add(vec4vf(a.v+ 0), vec4vf(b.v+ 0)));
	vfvec4(c.v+ 4, v4add(vec4vf(a.v+ 4), vec4vf(b.v+ 4)));
	vfvec4(c.v+ 8, v4add(vec4vf(a.v+ 8), vec4vf(b.v+ 8)));
	vfvec4(c.v+12, v4add(vec4vf(a.v+12), vec4vf(b.v+12)));

#if 0
	c.v[ 0]=a.v[ 0]+b.v[ 0];
	c.v[ 1]=a.v[ 1]+b.v[ 1];
	c.v[ 2]=a.v[ 2]+b.v[ 2];
	c.v[ 3]=a.v[ 3]+b.v[ 3];
	c.v[ 4]=a.v[ 4]+b.v[ 4];
	c.v[ 5]=a.v[ 5]+b.v[ 5];
	c.v[ 6]=a.v[ 6]+b.v[ 6];
	c.v[ 7]=a.v[ 7]+b.v[ 7];
	c.v[ 8]=a.v[ 8]+b.v[ 8];
	c.v[ 9]=a.v[ 9]+b.v[ 9];
	c.v[10]=a.v[10]+b.v[10];
	c.v[11]=a.v[11]+b.v[11];
	c.v[12]=a.v[12]+b.v[12];
	c.v[13]=a.v[13]+b.v[13];
	c.v[14]=a.v[14]+b.v[14];
	c.v[15]=a.v[15]+b.v[15];
#endif

	return(c);
}

BGBDY_API mat4 dtyMat4Sub(mat4 a, mat4 b)
{
	mat4 c;

	vfvec4(c.v+ 0, v4sub(vec4vf(a.v+ 0), vec4vf(b.v+ 0)));
	vfvec4(c.v+ 4, v4sub(vec4vf(a.v+ 4), vec4vf(b.v+ 4)));
	vfvec4(c.v+ 8, v4sub(vec4vf(a.v+ 8), vec4vf(b.v+ 8)));
	vfvec4(c.v+12, v4sub(vec4vf(a.v+12), vec4vf(b.v+12)));

#if 0
	c.v[ 0]=a.v[ 0]-b.v[ 0];
	c.v[ 1]=a.v[ 1]-b.v[ 1];
	c.v[ 2]=a.v[ 2]-b.v[ 2];
	c.v[ 3]=a.v[ 3]-b.v[ 3];
	c.v[ 4]=a.v[ 4]-b.v[ 4];
	c.v[ 5]=a.v[ 5]-b.v[ 5];
	c.v[ 6]=a.v[ 6]-b.v[ 6];
	c.v[ 7]=a.v[ 7]-b.v[ 7];
	c.v[ 8]=a.v[ 8]-b.v[ 8];
	c.v[ 9]=a.v[ 9]-b.v[ 9];
	c.v[10]=a.v[10]-b.v[10];
	c.v[11]=a.v[11]-b.v[11];
	c.v[12]=a.v[12]-b.v[12];
	c.v[13]=a.v[13]-b.v[13];
	c.v[14]=a.v[14]-b.v[14];
	c.v[15]=a.v[15]-b.v[15];
#endif

	return(c);
}

BGBDY_API mat4 dtyMat4Mul(mat4 a, mat4 b)
{
	vec4 b0, b1, b2, b3;
	vec4 c0, c1, c2, c3;
	mat4 c;

	b0=vec4vf(b.v+ 0); b1=vec4vf(b.v+ 4);
	b2=vec4vf(b.v+ 8); b3=vec4vf(b.v+12);
	c0=v4scaleadd4(b0, a.v[ 0], b1, a.v[ 1], b2, a.v[ 2], b3, a.v[ 3]);
	c1=v4scaleadd4(b0, a.v[ 4], b1, a.v[ 5], b2, a.v[ 6], b3, a.v[ 7]);
	c2=v4scaleadd4(b0, a.v[ 8], b1, a.v[ 9], b2, a.v[10], b3, a.v[11]);
	c3=v4scaleadd4(b0, a.v[12], b1, a.v[13], b2, a.v[14], b3, a.v[15]);
	vfvec4(c.v+ 0, c0); vfvec4(c.v+ 4, c1);
	vfvec4(c.v+ 8, c2); vfvec4(c.v+12, c3);

	return(c);

#if 0
	c.v[ 0]=a.v[ 0]*b.v[0] + a.v[ 1]*b.v[4] + a.v[ 2]*b.v[ 8] + a.v[ 3]*b.v[12];
	c.v[ 1]=a.v[ 0]*b.v[1] + a.v[ 1]*b.v[5] + a.v[ 2]*b.v[ 9] + a.v[ 3]*b.v[13];
	c.v[ 2]=a.v[ 0]*b.v[2] + a.v[ 1]*b.v[6] + a.v[ 2]*b.v[10] + a.v[ 3]*b.v[14];
	c.v[ 3]=a.v[ 0]*b.v[3] + a.v[ 1]*b.v[7] + a.v[ 2]*b.v[11] + a.v[ 3]*b.v[15];

	c.v[ 4]=a.v[ 4]*b.v[0] + a.v[ 5]*b.v[4] + a.v[ 6]*b.v[ 8] + a.v[ 7]*b.v[12];
	c.v[ 5]=a.v[ 4]*b.v[1] + a.v[ 5]*b.v[5] + a.v[ 6]*b.v[ 9] + a.v[ 7]*b.v[13];
	c.v[ 6]=a.v[ 4]*b.v[2] + a.v[ 5]*b.v[6] + a.v[ 6]*b.v[10] + a.v[ 7]*b.v[14];
	c.v[ 7]=a.v[ 4]*b.v[3] + a.v[ 5]*b.v[7] + a.v[ 6]*b.v[11] + a.v[ 7]*b.v[15];

	c.v[ 8]=a.v[ 8]*b.v[0] + a.v[ 9]*b.v[4] + a.v[10]*b.v[ 8] + a.v[11]*b.v[12];
	c.v[ 9]=a.v[ 8]*b.v[1] + a.v[ 9]*b.v[5] + a.v[10]*b.v[ 9] + a.v[11]*b.v[13];
	c.v[10]=a.v[ 8]*b.v[2] + a.v[ 9]*b.v[6] + a.v[10]*b.v[10] + a.v[11]*b.v[14];
	c.v[11]=a.v[ 8]*b.v[3] + a.v[ 9]*b.v[7] + a.v[10]*b.v[11] + a.v[11]*b.v[15];

	c.v[12]=a.v[12]*b.v[0] + a.v[13]*b.v[4] + a.v[14]*b.v[ 8] + a.v[15]*b.v[12];
	c.v[13]=a.v[12]*b.v[1] + a.v[13]*b.v[5] + a.v[14]*b.v[ 9] + a.v[15]*b.v[13];
	c.v[14]=a.v[12]*b.v[2] + a.v[13]*b.v[6] + a.v[14]*b.v[10] + a.v[15]*b.v[14];
	c.v[15]=a.v[12]*b.v[3] + a.v[13]*b.v[7] + a.v[14]*b.v[11] + a.v[15]*b.v[15];

	return(c);
#endif
}

static void VecNF_Copy(float *a, float *b, int n)
	{ int i; for(i=0;i<n;i++)b[i]=a[i]; }
static void VecNF_Zero(float *a, int n)
	{ int i; for(i=0; i<n; i++)a[i]=0; }
static void VecNF_Add(float *a, float *b, float *c, int n)
	{ int i; for(i=0; i<n; i++)c[i]=a[i]+b[i]; }
static void VecNF_Subtract(float *a, float *b, float *c, int n)
	{ int i; for(i=0; i<n; i++)c[i]=a[i]-b[i]; }
static void VecNF_Scale(float *a, float b, float *c, int n)
	{ int i; for(i=0; i<n; i++)c[i]=a[i]*b; }

void dtyMat_Inverse(float *a, float *b, int n)
{
	static float a2buf[8*8*2], a3buf[16];
	float *a2p[8], *a3p;
	int i, j;

	VecNF_Zero(a2buf, n*n*2);
	VecNF_Zero(b, n*n);
	for(i=0; i<n; i++)a2p[i]=&a2buf[i*n*2];
	for(i=0; i<n; i++)VecNF_Copy(&a[i*n], a2p[i], n);
	for(i=0; i<n; i++)a2p[i][i+n]=1.0;

	for(i=0; i<n; i++)
	{
//		MatNF_Dump(a2buf, n, 2*n);
//		printf("\n");
		if(a2p[i][i]==0) /* search for a new pivot in a lower row */
		{
			for(j=n-1; j>i; j--)if(a2p[j][i]!=0)
			{
				a3p=a2p[i];
				a2p[i]=a2p[j];
				a2p[j]=a3p;
			}
			if(a2p[i][i]==0)return; /* no pivot */
		}
		VecNF_Scale(a2p[i], 1/a2p[i][i], a2p[i], 2*n);
		for(j=0; j<n; j++)if(i!=j)
		{
			VecNF_Scale(a2p[i], -a2p[j][i], a3buf, 2*n);
			VecNF_Add(a3buf, a2p[j], a2p[j], 2*n);
		}
	}

//	MatNF_Dump(a2buf, n, 2*n);
//	printf("\n");

	for(i=0; i<n; i++)VecNF_Copy(&a2p[i][n], &b[i*n], n);
}

BGBDY_API mat2 dtyMat2Inv(mat2 a)
{
	mat2 t;
	dtyMat_Inverse(a.v, t.v, 2);
	return(t);
}

BGBDY_API mat3 dtyMat3Inv(mat3 a)
{
	mat3 t;
	dtyMat_Inverse(a.v, t.v, 3);
	return(t);
}

BGBDY_API mat4 dtyMat4Inv(mat4 a)
{
	mat4 t;
	dtyMat_Inverse(a.v, t.v, 4);
	return(t);
}

BGBDY_API mat2 dtyMat2Trans(mat2 a)
{
	mat2 t;
	t.v[0]=a.v[0]; t.v[1]=a.v[2];
	t.v[2]=a.v[1]; t.v[3]=a.v[3];
	return(t);
}

BGBDY_API mat3 dtyMat3Trans(mat3 a)
{
	mat3 t;
	t.v[0]=a.v[0]; t.v[1]=a.v[3]; t.v[2]=a.v[6];
	t.v[3]=a.v[1]; t.v[4]=a.v[4]; t.v[5]=a.v[7];
	t.v[6]=a.v[2]; t.v[7]=a.v[5]; t.v[8]=a.v[8];
	return(t);
}

BGBDY_API mat4 dtyMat4Trans(mat4 a)
{
	mat4 t;
	t.v[0]=a.v[0];  t.v[1]=a.v[4];  t.v[2]=a.v[8];  t.v[3]=a.v[12];
	t.v[4]=a.v[1];  t.v[5]=a.v[5];  t.v[6]=a.v[9];  t.v[7]=a.v[13];
	t.v[8]=a.v[2];  t.v[9]=a.v[6];  t.v[10]=a.v[10]; t.v[11]=a.v[14];
	t.v[12]=a.v[3]; t.v[13]=a.v[7]; t.v[14]=a.v[11]; t.v[15]=a.v[15];
	return(t);
}

BGBDY_API mat2 dtyMat2MulI(mat2 a, mat2 b)
	{ return(dtyMat2Mul(a, dtyMat2Inv(b))); }
BGBDY_API mat3 dtyMat3MulI(mat3 a, mat3 b)
	{ return(dtyMat3Mul(a, dtyMat3Inv(b))); }
BGBDY_API mat4 dtyMat4MulI(mat4 a, mat4 b)
	{ return(dtyMat4Mul(a, dtyMat4Inv(b))); }

BGBDY_API mat2 dtyMat2MulT(mat2 a, mat2 b)
	{ return(dtyMat2Mul(a, dtyMat2Trans(b))); }
BGBDY_API mat3 dtyMat3MulT(mat3 a, mat3 b)
	{ return(dtyMat3Mul(a, dtyMat3Trans(b))); }
BGBDY_API mat4 dtyMat4MulT(mat4 a, mat4 b)
	{ return(dtyMat4Mul(a, dtyMat4Trans(b))); }
