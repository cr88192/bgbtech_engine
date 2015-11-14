#include <stdio.h>

#include <complex.h>

#include <vecnf.h>

int bscc_main()
{
//	__v3f u, v, w;
//	__v3f a, b, c;

	double _Complex ci, cj;

	quat p, q, r;

//	v3f u, v, w;
//	v3f a, b, c;
	vec3 u, v, w;
	vec3 a, b, c;
	float x, y, z;

#if 1
//	u=__fvec3(1,0,0);
//	v=__fvec3(0,1,0);

	u=vec3(1,0,0);
	v=vec3(0,1,0);
	w=u%v;

//	printf("%f %f %f\n", w.x, w.y, w.z);
	printf("I %f %f %f %f\n", u.x, u.y, u.z, u.w);
	printf("J %f %f %f %f\n", v.x, v.y, v.z, v.w);
	printf("K %f %f %f %f\n", w.x, w.y, w.z, w.w);

	a=__fvec3(3,4,5);

	printf("A %f %f %f %f\n", a.x, a.y, a.z, a.w);

	x=1; y=2; z=3;

	x=a^u;
	y=a^v;
	z=a^w;

	printf("%f %f %f\n", x, y, z);

	ci=1+2i;
	cj=ci*ci;
	printf("%f %f\n", creal(ci), cimag(ci));
	printf("%f %f\n", creal(cj), cimag(cj));

//	p=QUAT(0,1,0,0);
//	q=QUAT(0,0,1,0);
	r=p*q;

#endif

	p=QUAT(3,1,0,0);
	q=QUAT(0,5,1,-2);
	r=p*q;
//	r=q*p;

//	p=QUAT(1,2,0,0);
//	q=QUAT(1,2,0,0);
//	r=p*q;

//	r=qlerp(p, q, 0.5);

	printf("P %g%+gi%+gj%+gk\n", p.w, p.x, p.y, p.z);
	printf("Q %g%+gi%+gj%+gk\n", q.w, q.x, q.y, q.z);
	printf("R %g%+gi%+gj%+gk\n", r.w, r.x, r.y, r.z);

	return(0);
}

int main()
{
	bscc_main();
}
