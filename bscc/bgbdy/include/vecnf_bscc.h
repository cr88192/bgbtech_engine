typedef __vec2 vec2;
typedef __vec3 vec3;
typedef __vec4 vec4;
typedef __quat quat;

typedef __vec2d vec2d;
typedef __vec3d vec3d;
typedef __vec4d vec4d;
typedef __quatd quatd;

#define vec2(x,y)		__fvec2(x,y)
#define vec3(x,y,z)		__fvec3(x,y,z)
#define vec4(x,y,z,w)	__fvec4(x,y,z,w)
#define quat(w,x,y,z)	__qvec(x,y,z,w)

#define vec2d(x,y)		__dvec2(x,y)
#define vec3d(x,y,z)	__dvec3(x,y,z)
#define vec4d(x,y,z,w)	__dvec4(x,y,z,w)
#define quatd(w,x,y,z)	__qvec(x,y,z,w)


#if 1
#define dot(a,b)	__builtin_fvec_dot(a,b)
#define cross(a,b)	__builtin_fvec_cross(a,b)
#define distance(a,b)	__builtin_fvec_distance(a,b)
#define length(a)	__builtin_fvec_len(a)
#define normalize(a)	__builtin_fvec_normalize(a)
#endif

#if 1
#define v2dot(a,b)	__builtin_fvec_dot(a,b)
#define v3dot(a,b)	__builtin_fvec_dot(a,b)
#define v4dot(a,b)	__builtin_fvec_dot(a,b)

#define v2cross(a,b)	__builtin_fvec_cross(a,b)
#define v3cross(a,b)	__builtin_fvec_cross(a,b)
#define v4cross(a,b)	__builtin_fvec_cross(a,b)

#define v2dist(a,b)	__builtin_fvec_distance(a,b)
#define v3dist(a,b)	__builtin_fvec_distance(a,b)
#define v4dist(a,b)	__builtin_fvec_distance(a,b)

#define v2len(a)	__builtin_fvec_len(a)
#define v3len(a)	__builtin_fvec_len(a)
#define v4len(a)	__builtin_fvec_len(a)

#define v2norm(a)	__builtin_fvec_normalize(a)
#define v3norm(a)	__builtin_fvec_normalize(a)
#define v4norm(a)	__builtin_fvec_normalize(a)

#define v2neg(a)	__builtin_fvec_neg(a)
#define v3neg(a)	__builtin_fvec_neg(a)
#define v4neg(a)	__builtin_fvec_neg(a)

#define v2add(a,b)	__builtin_fvec_add(a,b)
#define v2sub(a,b)	__builtin_fvec_sub(a,b)
#define v2mul(a,b)	__builtin_fvec_mul(a,b)
#define v2div(a,b)	__builtin_fvec_div(a,b)

#define v3add(a,b)	__builtin_fvec_add(a,b)
#define v3sub(a,b)	__builtin_fvec_sub(a,b)
#define v3mul(a,b)	__builtin_fvec_mul(a,b)
#define v3div(a,b)	__builtin_fvec_div(a,b)

#define v4add(a,b)	__builtin_fvec_add(a,b)
#define v4sub(a,b)	__builtin_fvec_sub(a,b)
#define v4mul(a,b)	__builtin_fvec_mul(a,b)
#define v4div(a,b)	__builtin_fvec_div(a,b)

#define v2scale(a,f)	__builtin_fvec_mul(a,f)
#define v3scale(a,f)	__builtin_fvec_mul(a,f)
#define v4scale(a,f)	__builtin_fvec_mul(a,f)


#define v2lerp(a,b,f)	__builtin_fvec_lerp(a,b,f)
#define v3lerp(a,b,f)	__builtin_fvec_lerp(a,b,f)
#define v4lerp(a,b,f)	__builtin_fvec_lerp(a,b,f)

#define v2nlerp(a,b,f)	__builtin_fvec_nlerp(a,b,f)
#define v3nlerp(a,b,f)	__builtin_fvec_nlerp(a,b,f)
#define v4nlerp(a,b,f)	__builtin_fvec_nlerp(a,b,f)

#endif

#define qdot(a,b)	__builtin_fvec_dot(a,b)
#define qcross(a,b)	__builtin_fvec_cross(a,b)
#define qdist(a,b)	__builtin_fvec_distance(a,b)
#define qlen(a)		__builtin_fvec_len(a)
#define qnorm(a)	__builtin_fvec_normalize(a)

#define qneg(a)		__builtin_fvec_neg(a)
#define qconj(a)	__builtin_quat_conj(a)
#define qrcp(a)		__builtin_fvec_rcp(a)

#define qadd(a,b)	__builtin_fvec_add(a,b)
#define qsub(a,b)	__builtin_fvec_sub(a,b)
#define qmul(a,b)	__builtin_fvec_mul(a,b)
#define qdiv(a,b)	__builtin_fvec_div(a,b)
#define qrdiv(a,b)	qmul(qrcp(b),a)

#define qscale(a,f)	__builtin_fvec_mul(a,f)
#define qlerp(a,b,f)	__builtin_fvec_lerp(a,b,f)
#define qnlerp(a,b,f)	__builtin_fvec_nlerp(a,b,f)

#define qrot3(a,b)	__builtin_fvec_rot3(a,b)
#define qrot4(a,b)	__builtin_fvec_rot4(a,b)
#define qaxis(a,th)	__builtin_quat_axis_angle(a,th)

#define v32q(a)		__builtin_fvec_to_quat(a)
#define v42q(a)		__builtin_fvec_to_quat(a)
#define q2v3(a)		__builtin_quat_to_v3f(a)
#define q2v4(a)		__builtin_quat_to_v4f(a)
