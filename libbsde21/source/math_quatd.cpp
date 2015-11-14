#include "bsde_internal.h"

void BSDE_Quat_Identity(bsde_real *a)
{
	a[0]=0;
	a[1]=0;
	a[2]=0;
	a[3]=1;
}

void BSDE_Quat_Conjugate(bsde_real *a, bsde_real *b)
{
	int i;
	for(i=0; i<4; i++)b[i]=-a[i];
}

bsde_real BSDE_Quat_Magnitude(bsde_real *a)
{
	bsde_real f;

	f=(a[0]*a[0])+(a[1]*a[1])+(a[2]*a[2])+(a[3]*a[3]);
	return(sqrt(f));
}

bsde_real BSDE_Quat_Normalize(bsde_real *a, bsde_real *b)
{
	bsde_real f, g;

	f=BSDE_Quat_Magnitude(a);

	g=(f>0)?f:1;
	b[0]=a[0]/g;
	b[1]=a[1]/g;
	b[2]=a[2]/g;
	b[3]=a[3]/g;

	return(f);
}

void BSDE_Quat_Multiply(bsde_real *a, bsde_real *b, bsde_real *c)
{
	bsde_real ci[4];

	ci[0]=(a[3]*b[0])+(a[0]*b[3])+(a[1]*b[2])-(a[2]*b[1]);
	ci[1]=(a[3]*b[1])+(a[1]*b[3])+(a[2]*b[0])-(a[0]*b[2]);
	ci[2]=(a[3]*b[2])+(a[2]*b[3])+(a[0]*b[1])-(a[1]*b[0]);
	ci[3]=(a[3]*b[3])-(a[0]*b[0])-(a[1]*b[1])-(a[2]*b[2]);
	BSDE_Quat_Normalize(ci, c);
}

void BSDE_Quat_ToMatrix(bsde_real *a, bsde_real *b)
{
#if 1
	bsde_real xx, xy, xz, xw, yy, yz, yw, zz, zw;
	int i;

	for(i=0; i<16; i++)b[i]=0;
	for(i=0; i<4; i++)b[(i*4)+i]=1;

	xx=a[0]*a[0];
	xy=a[0]*a[1];
	xz=a[0]*a[2];
	xw=a[0]*a[3];
	yy=a[1]*a[1];
	yz=a[1]*a[2];
	yw=a[1]*a[3];
	zz=a[2]*a[2];
	zw=a[2]*a[3];
#endif

#if 1
	b[0]= 1-2*(yy+zz);
	b[1]=   2*(xy+zw);
	b[2]=   2*(xz-yw);

	b[4]=   2*(xy-zw);
	b[5]= 1-2*(xx+zz);
	b[6]=   2*(yz+xw);

	b[8]=   2*(xz+yw);
	b[9]=   2*(yz-xw);
	b[10]=1-2*(xx+yy);
#endif
#if 0
	b[0]= 1-(2*yy)-(2*zz);
	b[1]=   (2*xy)-(2*zw);
	b[2]=   (2*xz)+(2*yw);

	b[4]=   (2*xy)+(2*zw);
	b[5]= 1-(2*xx)-(2*zz);
	b[6]=   (2*yz)-(2*xw);

	b[8]=   (2*xz)-(2*yw);
	b[9]=   (2*yz)+(2*xw);
	b[10]=1-(2*xx)-(2*yy);
#endif
}

void BSDE_Quat_To3Matrix(bsde_real *a, bsde_real *b)
{
	bsde_real xx, xy, xz, xw, yy, yz, yw, zz, zw;
	int i;

	for(i=0; i<9; i++)b[i]=0;
	for(i=0; i<3; i++)b[(i*3)+i]=1;

	xx=a[0]*a[0];
	xy=a[0]*a[1];
	xz=a[0]*a[2];
	xw=a[0]*a[3];
	yy=a[1]*a[1];
	yz=a[1]*a[2];
	yw=a[1]*a[3];
	zz=a[2]*a[2];
	zw=a[2]*a[3];

#if 1
	b[0]= 1-2*(yy+zz);
	b[1]=   2*(xy+zw);
	b[2]=   2*(xz-yw);

	b[3]=   2*(xy-zw);
	b[4]= 1-2*(xx+zz);
	b[5]=   2*(yz+xw);

	b[6]=   2*(xz+yw);
	b[7]=   2*(yz-xw);
	b[8]=1-2*(xx+yy);
#endif
#if 0
	b[0]=1-(2*yy)-(2*zz);
	b[1]=  (2*xy)-(2*zw);
	b[2]=  (2*xz)+(2*yw);

	b[3]=  (2*xy)+(2*zw);
	b[4]=1-(2*xx)-(2*zz);
	b[5]=  (2*yz)-(2*xw);

	b[6]=  (2*xz)-(2*yw);
	b[7]=  (2*yz)+(2*xw);
	b[8]=1-(2*xx)-(2*yy);
#endif
}

void BSDE_Quat_From3Matrix(bsde_real *a, bsde_real *b)
{
	bsde_real s, t;

	t=a[0]+a[4]+a[8];
	if(t>0)
	{
		s=sqrt(t+1.0)*2.0;
		b[0]=(a[7]-a[5])/s;
		b[1]=(a[2]-a[6])/s;
		b[2]=(a[3]-a[1])/s;
		b[3]=0.25*s;
	}else if((a[0]>a[4])&&(a[0]>a[8]))
	{
		s=sqrt(1.0+a[0]-a[4]-a[8])*2.0;
		b[0]=0.25*s;
		b[1]=(a[1]+a[3])/s;
		b[2]=(a[2]+a[6])/s;
		b[3]=(a[7]-a[5])/s;
	}else if(a[4]>a[8])
	{
		s=sqrt(1.0-a[0]+a[4]-a[8])*2.0;
		b[0]=(a[1]+a[3])/s;
		b[1]=0.25*s;
		b[2]=(a[5]+a[7])/s;
		b[3]=(a[2]-a[6])/s;
	}else
	{
		s=sqrt(1.0-a[0]-a[4]+a[8])*2.0;
		b[0]=(a[2]+a[6])/s;
		b[1]=(a[5]+a[7])/s;
		b[2]=0.25*s;
		b[3]=(a[3]-a[1])/s;
	}
	BSDE_Quat_Normalize(b, b);
}

void BSDE_Quat_FromAxis(bsde_real *a, bsde_real b, bsde_real *c)
{
	bsde_real v[3], sa, ca;

	sa=sin(b/2);
	ca=cos(b/2);
	V3_NORMALIZE(a, v);

	c[0]=v[0]*sa;
	c[1]=v[1]*sa;
	c[2]=v[2]*sa;
	c[3]=ca;
}

void BSDE_Quat_ToAxis(bsde_real *a, bsde_real *b, bsde_real *c)
{
	bsde_real v[4], sa, ca, th;

	BSDE_Quat_Normalize(a, v);
	ca=v[4];
	th=acos(ca)*2;
	sa=sqrt(1.0-(ca*ca));
	if(fabs(sa)<0.0005)sa=1;

	b[0]=v[0]/sa;
	b[1]=v[1]/sa;
	b[2]=v[2]/sa;
	*c=th;
}

void BSDE_Quat_FromAngles(bsde_real *a, bsde_real *b)
{
	bsde_real sx, cx, sy, cy, sz, cz;

	sx=sin(a[0]*0.5);
	cx=cos(a[0]*0.5);
	sy=sin(a[1]*0.5);
	cy=cos(a[1]*0.5);
	sz=sin(a[2]*0.5);
	cz=cos(a[2]*0.5);

	b[0]=sx*cy*cz-cx*sy*sz; // X
	b[1]=cx*sy*cz+sx*cy*sz; // Y
	b[2]=cx*cy*sz-sx*sy*cz; // Z
	b[3]=cx*cy*cz+sx*sy*sz; // W
}

void BSDE_Quat_ToAngles(bsde_real *a, bsde_real *b)
{
	bsde_real u[4], v[4];

	V4_ZERO(u);
	u[1]=1;
	BSDE_Quat_Multiply(a, u, v);
	b[0]=atan2(v[2], v[1]);

	V4_ZERO(u);
	u[0]=1;
	BSDE_Quat_Multiply(a, u, v);
	b[1]=atan2(v[2], v[0]);

	V4_ZERO(u);
	u[0]=1;
	BSDE_Quat_Multiply(a, u, v);
	b[2]=atan2(v[1], v[0]);
}

void BSDE_Quat_Lerp(bsde_real *a, bsde_real *b, bsde_real t, bsde_real *c)
{
	V4_SCALE(a, 1-t, c);
	V4_ADDSCALE(c, b, t, c);
	BSDE_Quat_Normalize(c, c);
}

void BSDE_Quat_SLerp(bsde_real *a, bsde_real *ib, bsde_real t, bsde_real *c)
{
	bsde_real e, f, g, h;
	bsde_real b[4];

	V4_COPY(ib, b);
	if(a[0]==b[0] && a[1]==b[1] && 
		a[2]==b[2] && a[3]==b[3])
	{
		V4_COPY(a, c);
		return;
	}

	f=V4_DOT(a, b);
	if(f<0)
	{
		BSDE_Quat_Conjugate(b, b);
		f=-f;
	}

	if((1-f)>0.1)
	{
		e=acos(f);
		g=sin((1-t)*e)/sin(e);
		h=sin((t*e))/sin(e);
	}else
	{
		g=1-t;
		h=t;
	}

	c[0]=(g*a[0])+(h*b[0]);
	c[1]=(g*a[1])+(h*b[1]);
	c[2]=(g*a[2])+(h*b[2]);
	c[3]=(g*a[3])+(h*b[3]);
}
