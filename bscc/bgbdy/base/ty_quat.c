#include <bgbgc.h>
#include <bgbdy.h>

BGBDY_API quat dtyQuatSgn(quat q)
	{ return(qnorm(q)); }

BGBDY_API float dtyQuatArg(quat q)
	{ return(acos(qreal(q)/qlen(q))); }

BGBDY_API quat dtyQuatLn(quat q)
{
	quat c;
	quat q0, q1, q2;
	float f0, f1, f2;

	f0=qlen(q);
	f1=acos(qreal(q)/f0);
	f2=log(f0);

	q0=qscale(qvec(q), 1.0/f0);
	q1=qscale(q0, f1);
	q2=quat(f2, 0, 0, 0);

	c=qadd(q2, q1);
	return(c);
}

BGBDY_API quat dtyQuatExp(quat q)
{
	quat qa, qt;
//	int i, j;

	qa=quat(1, 0, 0, 0);
	qa=qadd(qa, q); qt=qmul(q, q);

	qa=qadd(qa, qscale(qt, 1.0/        2.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/        6.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/       24.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/      120.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/      720.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/     5040.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/    40320.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/   362880.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/  3628800.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/ 39916800.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, 1.0/479001600.0)); qt=qmul(qt, q);

	return(qa);

#if 0
	qt=q; j=1;
	for(i=2; i<12; i++)
	{
		qa=qadd(qa, qscale(qt, 1.0/j));
		j*=i; qt=qmul(qt, q);
	}
#endif
}

BGBDY_API quat dtyQuatSin(quat q)
{
	quat qa, qs, qt;

	qs=qmul(q, q);
	qa=q; qt=qmul(q, qs);

	qa=qadd(qa, qscale(qt, -1.0/         6.0)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/       120.0)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/      5040.0)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/    362880.0)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/  39916800.0)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/6227020800.0)); qt=qmul(qt, qs);

	return(qa);
}

BGBDY_API quat dtyQuatCos(quat q)
{
	quat qa, qs, qt;

	qs=qmul(q, q);
	qa=quat(1, 0, 0, 0); qt=qs;

	qa=qadd(qa, qscale(qt, -1.0/        2.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt,  1.0/       24.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, -1.0/      720.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt,  1.0/    40320.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt, -1.0/  3628800.0)); qt=qmul(qt, q);
	qa=qadd(qa, qscale(qt,  1.0/479001600.0)); qt=qmul(qt, q);

	return(qa);
}

BGBDY_API quat dtyQuatTan(quat q)
{
	return(qdiv(qsin(q), qcos(q)));
}

BGBDY_API quat dtyQuatCot(quat q)
{
	return(qdiv(qcos(q), qsin(q)));
}

BGBDY_API quat dtyQuatSinh(quat q)
	{ return(qscaleaddscale(qexp(q), 0.5, qexp(qneg(q)), -0.5)); }
BGBDY_API quat dtyQuatCosh(quat q)
	{ return(qscaleaddscale(qexp(q), 0.5, qexp(qneg(q)), 0.5)); }
BGBDY_API quat dtyQuatTanh(quat q)
	{ return(qdiv(qsinh(q), qcosh(q))); }
BGBDY_API quat dtyQuatCoth(quat q)
	{ return(qdiv(qcosh(q), qsinh(q))); }

BGBDY_API quat dtyQuatASin(quat q)
{
	quat qa, qs, qt;

	qs=qmul(q, q);
	qa=q; qt=qmul(q, qs);

	qa=qadd(qa, qscale(qt, 0.1666667)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.075)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0446429)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0303819)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0223722)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0173528)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0139648)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0115518)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.00976161)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.00839034)); qt=qmul(qt, qs);

	return(qa);
}

BGBDY_API quat dtyQuatACos(quat q)
{
	quat qa, qs, qt;

	qs=qmul(q, q);
	qa=q; qt=qmul(q, qs);

	qa=qadd(qa, qscale(qt, 0.1666667)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.075)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0446429)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0303819)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0223722)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0173528)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0139648)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.0115518)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.00976161)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, 0.00839034)); qt=qmul(qt, qs);

	qa=qsub(quat(0.5*M_PI,0,0,0), qa);

	return(qa);
}

BGBDY_API quat dtyQuatATan(quat q)
{
	quat qa, qs, qt;

	qs=qmul(q, q);
	qa=q; qt=qmul(q, qs);

	qa=qadd(qa, qscale(qt, -1.0/ 3)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/ 5)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/ 7)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/ 9)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/11)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/13)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/15)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/17)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/19)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/21)); qt=qmul(qt, qs);

	return(qa);
}

BGBDY_API quat dtyQuatACot(quat q)
{
	quat qa, qs, qt;

	qs=qmul(q, q);
	qa=q; qt=qmul(q, qs);

	qa=qadd(qa, qscale(qt, -1.0/ 3)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/ 5)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/ 7)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/ 9)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/11)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/13)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/15)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/17)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt, -1.0/19)); qt=qmul(qt, qs);
	qa=qadd(qa, qscale(qt,  1.0/21)); qt=qmul(qt, qs);

	qa=qsub(quat(0.5*M_PI,0,0,0), qa);

	return(qa);
}

BGBDY_API quat dtyExpRealQuat(float b, quat x)
{
	quat c;
	c=dtyQuatExp(qscale(x, log(b)));
	return(c);
}

BGBDY_API quat dtyExpQuatReal(quat p, float q)
{
	quat c;
	c=dtyQuatExp(qscale(dtyQuatLn(p), q));
	return(c);
}

BGBDY_API quat dtyQuatExpQuatQuat(quat p, quat q)
{
	quat c;
	c=dtyQuatExp(qmul(dtyQuatLn(p), q));
	return(c);
}

BGBDY_API vec3 dtyQuatSlerp(vec3 v0, vec3 v1, float t)
{
	quat c;
	c=qmul(dtyExpQuatReal(qdiv(v1, v0), t), v0);
	return(c);
}

BGBDY_API vec3 dtyVec3ArcPolate(vec3 v0, vec3 v1, float t)
{
	vec3 v2, nx, ny;
	float f, g;

	//very similar or very far apart
	f=v3dot(v0, v1);
	if(f>0.99)
	{
		v2=v3lerp(v0, v1, t);
		v2=v3norm(v2);
		return(v2);
	}

	if(f<-0.99)
	{
		v2=v3lerp(v0, v1, t);
		v2=v3norm(v2);
		return(v2);
	}

	nx=v3cross(v0, v1);
	g=v3len(nx); nx=v3norm(nx);
	f=acos(f)*t;

	ny=v3cross(nx, v0);
	v2=v3add(v3scale(v0, cos(f)), v3scale(ny, sin(f)));
	v2=v3norm(v2);

	return(v2);
}

BGBDY_API quat dtyQuatArcPolate(quat q0, quat q1, float t)
{
	quat v0, v1, v2, q2;
	float r0, r1, r2, th0, th1, th2;
	float f, g;

	//very similar or very far apart
	f=qdot(q0, q1);
	if(f>0.99)
	{
		q2=qlerp(q0, q1, t);
		q2=qnorm(q2);
		return(q2);
	}

	if(f<-0.99)
	{
		q2=qlerp(q0, q1, t);
		q2=qnorm(q2);
		return(q2);
	}

//	f=(M_PI/2)*t;
//	v2=qadd(qscale(v0, cos(f)), qscale(v1, sin(f)));
//	v2=qnorm(v2);

	r0=qreal(q0); r1=qreal(q1);
	th0=acos(r0)*2; th1=acos(r1)*2;
	th2=th0*(1.0-t) + th1*t;
	r2=cos(th2*0.5);

	f=sqrt(1.0-r0*r0);
	if(f<0.01) v0=vec3(0, 0, 1);
		else v0=qscale(qvec(q0), 1.0/f);
	f=sqrt(1.0-r1*r1);
	if(f<0.01) v1=vec3(0, 0, 1);
		else v1=qscale(qvec(q1), 1.0/f);

	v2=dtyVec3ArcPolate(v0, v1, t);
	q2=qadd(quat(r2, 0, 0, 0), qscale(qvec(v2), sin(th2*0.5)));

	return(q2);
}
