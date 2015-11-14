#include <stdio.h>

#include <vecnf.h>

v3f q_rot_v3f(quat a, v3f b)
	{ return(q2v3f(qmul(qmul(a, v3f2q(b)), qconj(a)))); }
v4f q_rot_v4f(quat a, v4f b)
	{ return(q2v4f(qmul(qmul(a, v4f2q(b)), qconj(a)))); }

quat qlerp(quat a, quat b, float t)
{
//	quat c;
//	printf("qlerp %f\n", t);
//	c=qadd(qscale(a, 1.0-t), qscale(b, t));
//	printf("C %g%+gi%+gj%+gk\n", c.w, c.x, c.y, c.z);
//	return(c);

	return(qadd(qscale(a, 1.0-t), qscale(b, t)));
}
quat qnlerp(quat a, quat b, float t)
	{ return(qnorm(qadd(qscale(a, 1.0-t), qscale(b, t)))); }
