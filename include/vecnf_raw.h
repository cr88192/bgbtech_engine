#define vec2(x,y)	_vec2i(x,y)
#define vec3(x,y,z)	_vec3i(x,y,z)
#define vec4(x,y,z,w)	_vec4i(x,y,z,w)
#define quat(w,x,y,z)	_quati(w,x,y,z)

#define vec2vf(v)	_vec2vf(v)
#define vec3vf(v)	_vec3vf(v)
#define vec4vf(v)	_vec4vf(v)
#define quatvf(v)	_quatvf(v)	//XYZW
#define quatvf2(v)	_quatvf2(v)	//WXYZ

#define pfvec2(v)	_vfvec2(v)
#define pfvec3(v)	_vfvec4(v)
#define pfvec4(v)	_vfvec4(v)
#define pfquat(v)	_vfvec4(v)	//XYZW
#define pfquat2(v)	_vf2vec4(v)	//WXYZ

#define vfvec2(p,v)	_vfavec2(p,v)
#define vfvec3(p,v)	_vfavec3(p,v)
#define vfvec4(p,v)	_vfavec4(p,v)
#define vfquat(p,v)	_vfavec4(p,v)		//XYZW
#define vfquat2(p,v)	_vfa2vec4(p,v)		//WXYZ

#define v4shuf(a,sh)	_vf4_shuf(a,sh)

#define v2zero()	vec2(0,0)
#define v3zero()	vec3(0,0,0)
#define v4zero()	vec4(0,0,0,0)
#define qzero()		quat(0,0,0,0)

typedef struct {float x,y;} vec2u;
typedef struct {float x,y,z,w;} vec4u;
typedef vec4u vec3u;
typedef vec4u quatu;

typedef vec2u vec2;
typedef vec4u vec3;
typedef vec4u vec4;
typedef vec4u quat;

typedef dytAlign(8) vec2u vec2a;
typedef dytAlign(16) vec4u vec3a;
typedef dytAlign(16) vec4u vec4a;
typedef dytAlign(16) vec4u quata;

static vec2 _vec2i(float x, float y)
	{ vec2 t; t.x=x; t.y=y; return(t); };
static vec3 _vec3i(float x, float y, float z)
	{ vec3 t; t.x=x; t.y=y; t.z=z; return(t); };
static vec4 _vec4i(float x, float y, float z, float w)
	{ vec4 t; t.x=x; t.y=y; t.z=z; t.w=w; return(t); };
static quat _quati(float w, float x, float y, float z)
	{ quat t; t.x=x; t.y=y; t.z=z; t.w=w; return(t); };

static vec2 _vec2vf(float *x)
	{ return(_vec2i(x[0], x[1])); };
static vec3 _vec3vf(float *x)
	{ return(_vec3i(x[0], x[1], x[2])); };
static vec4 _vec4vf(float *x)
	{ return(_vec4i(x[0], x[1], x[2], x[3])); };
static quat _quatvf(float *x)
	{ return(_vec4i(x[3], x[0], x[1], x[2])); };
static quat _quatvf2(float *x)
	{ return(_quati(x[0], x[1], x[2], x[3])); };

static float *_vfvec2(vec2u v)
{
	float *p;
	p=(float *)gcralloc(4*sizeof(float));
	p[0]=v.x; p[1]=v.y; p[2]=0; p[3]=0;
	return(p);
}

static float *_vfvec4(vec4u v)
{
	float *p;
	p=(float *)gcralloc(4*sizeof(float));
	p[0]=v.x; p[1]=v.y; p[2]=v.z; p[3]=v.w;
	return(p);
}

static float *_vf2vec4(vec4u v)
{
	float *p;
	p=(float *)gcralloc(4*sizeof(float));
	p[0]=v.w; p[1]=v.x; p[2]=v.y; p[3]=v.z;
	return(p);
}

static void _vfavec2(float *p, vec2u v)
	{ p[0]=v.x; p[1]=v.y; }
static void _vfavec3(float *p, vec4u v)
	{ p[0]=v.x; p[1]=v.y; p[2]=v.z; }
static void _vfavec4(float *p, vec4u v)
	{ p[0]=v.x; p[1]=v.y; p[2]=v.z; p[3]=v.w; }
static void _vfa2vec4(float *p, vec4u v)
	{ p[0]=v.w; p[1]=v.x; p[2]=v.y; p[3]=v.z; }

static float _vf4_getidx(vec4u v, int i)
{
	float f;
	switch(i)
	{
	case 0: f=v.x; break; case 1: f=v.y; break;
	case 2: f=v.z; break; case 3: f=v.w; break;
	default: f=0; break;
	}
	return(f);
}

static vec4 _vf4_shuf(vec4u v, int sh)
{
	vec4 b;
	b.x=_vf4_getidx(v, sh&3);
	b.y=_vf4_getidx(v, (sh>>2)&3);
	b.z=_vf4_getidx(v, (sh>>4)&3);
	b.w=_vf4_getidx(v, (sh>>6)&3);
	return(b);
}


static float v2x(vec2u a)	{ return(a.x); };
static float v2y(vec2u a)	{ return(a.y); };

static float v3x(vec3u a)	{ return(a.x); };
static float v3y(vec3u a)	{ return(a.y); };
static float v3z(vec3u a)	{ return(a.z); };

static float v4x(vec4u a)	{ return(a.x); };
static float v4y(vec4u a)	{ return(a.y); };
static float v4z(vec4u a)	{ return(a.z); };
static float v4w(vec4u a)	{ return(a.w); };

static vec2 v2xy(vec2u a)	{ vec2 t; t.x=a.x; t.y=a.y; return(t); };
static vec2 v2yx(vec2u a)	{ vec2 t; t.x=a.y; t.y=a.x; return(t); };

static vec2 v3xy(vec3u a)	{ vec2 t; t.x=a.x; t.y=a.y; return(t); };
static vec2 v3xz(vec3u a)	{ vec2 t; t.x=a.x; t.y=a.z; return(t); };
static vec2 v3yx(vec3u a)	{ vec2 t; t.x=a.y; t.y=a.x; return(t); };
static vec2 v3yz(vec3u a)	{ vec2 t; t.x=a.y; t.y=a.z; return(t); };
static vec2 v3zx(vec3u a)	{ vec2 t; t.x=a.z; t.y=a.x; return(t); };
static vec2 v3zy(vec3u a)	{ vec2 t; t.x=a.z; t.y=a.y; return(t); };

static vec3 v3xyz(vec3u a) { vec3 t; t.x=a.x; t.y=a.y; t.z=a.z; return(t); };
static vec3 v3xzy(vec3u a) { vec3 t; t.x=a.x; t.y=a.z; t.z=a.y; return(t); };
static vec3 v3yxz(vec3u a) { vec3 t; t.x=a.y; t.y=a.x; t.z=a.z; return(t); };
static vec3 v3yzx(vec3u a) { vec3 t; t.x=a.y; t.y=a.z; t.z=a.x; return(t); };
static vec3 v3zxy(vec3u a) { vec3 t; t.x=a.z; t.y=a.x; t.z=a.y; return(t); };
static vec3 v3zyx(vec3u a) { vec3 t; t.x=a.z; t.y=a.y; t.z=a.x; return(t); };

static vec2 v4xy(vec4u a)	{ vec2 t; t.x=a.x; t.y=a.y; return(t); };
static vec2 v4xz(vec4u a)	{ vec2 t; t.x=a.x; t.y=a.z; return(t); };
static vec2 v4xw(vec4u a)	{ vec2 t; t.x=a.x; t.y=a.w; return(t); };
static vec2 v4yx(vec4u a)	{ vec2 t; t.x=a.y; t.y=a.x; return(t); };
static vec2 v4yz(vec4u a)	{ vec2 t; t.x=a.y; t.y=a.z; return(t); };
static vec2 v4yw(vec4u a)	{ vec2 t; t.x=a.y; t.y=a.w; return(t); };
static vec2 v4zx(vec4u a)	{ vec2 t; t.x=a.z; t.y=a.x; return(t); };
static vec2 v4zy(vec4u a)	{ vec2 t; t.x=a.z; t.y=a.y; return(t); };
static vec2 v4zw(vec4u a)	{ vec2 t; t.x=a.z; t.y=a.w; return(t); };
static vec2 v4wx(vec4u a)	{ vec2 t; t.x=a.w; t.y=a.x; return(t); };
static vec2 v4wy(vec4u a)	{ vec2 t; t.x=a.w; t.y=a.y; return(t); };
static vec2 v4wz(vec4u a)	{ vec2 t; t.x=a.w; t.y=a.z; return(t); };

static vec3 v4xyz(vec4u a) { vec3 t; t.x=a.x; t.y=a.y; t.z=a.z; return(t); };
static vec3 v4xyw(vec4u a) { vec3 t; t.x=a.x; t.y=a.y; t.z=a.w; return(t); };
static vec3 v4xzy(vec4u a) { vec3 t; t.x=a.x; t.y=a.z; t.z=a.y; return(t); };
static vec3 v4xzw(vec4u a) { vec3 t; t.x=a.x; t.y=a.z; t.z=a.w; return(t); };
static vec3 v4xwy(vec4u a) { vec3 t; t.x=a.x; t.y=a.w; t.z=a.y; return(t); };
static vec3 v4xwz(vec4u a) { vec3 t; t.x=a.x; t.y=a.w; t.z=a.z; return(t); };
static vec3 v4yxz(vec4u a) { vec3 t; t.x=a.y; t.y=a.x; t.z=a.z; return(t); };
static vec3 v4yxw(vec4u a) { vec3 t; t.x=a.y; t.y=a.x; t.z=a.w; return(t); };
static vec3 v4yzx(vec4u a) { vec3 t; t.x=a.y; t.y=a.z; t.z=a.x; return(t); };
static vec3 v4yzw(vec4u a) { vec3 t; t.x=a.y; t.y=a.z; t.z=a.w; return(t); };
static vec3 v4ywx(vec4u a) { vec3 t; t.x=a.y; t.y=a.w; t.z=a.x; return(t); };
static vec3 v4ywz(vec4u a) { vec3 t; t.x=a.y; t.y=a.w; t.z=a.z; return(t); };
static vec3 v4zxy(vec4u a) { vec3 t; t.x=a.z; t.y=a.x; t.z=a.y; return(t); };
static vec3 v4zxw(vec4u a) { vec3 t; t.x=a.z; t.y=a.x; t.z=a.w; return(t); };
static vec3 v4zyx(vec4u a) { vec3 t; t.x=a.z; t.y=a.y; t.z=a.x; return(t); };
static vec3 v4zyw(vec4u a) { vec3 t; t.x=a.z; t.y=a.y; t.z=a.w; return(t); };
static vec3 v4zwx(vec4u a) { vec3 t; t.x=a.z; t.y=a.w; t.z=a.x; return(t); };
static vec3 v4zwy(vec4u a) { vec3 t; t.x=a.z; t.y=a.w; t.z=a.y; return(t); };
static vec3 v4wxy(vec4u a) { vec3 t; t.x=a.w; t.y=a.x; t.z=a.y; return(t); };
static vec3 v4wxz(vec4u a) { vec3 t; t.x=a.w; t.y=a.x; t.z=a.z; return(t); };
static vec3 v4wyx(vec4u a) { vec3 t; t.x=a.w; t.y=a.y; t.z=a.x; return(t); };
static vec3 v4wyz(vec4u a) { vec3 t; t.x=a.w; t.y=a.y; t.z=a.z; return(t); };
static vec3 v4wzx(vec4u a) { vec3 t; t.x=a.w; t.y=a.z; t.z=a.x; return(t); };
static vec3 v4wzy(vec4u a) { vec3 t; t.x=a.w; t.y=a.z; t.z=a.y; return(t); };

static vec4 v4xyzw(vec4u a) { vec4 t; t.x=a.x; t.y=a.y; t.z=a.z; t.w=a.w; return(t); };
static vec4 v4xywz(vec4u a) { vec4 t; t.x=a.x; t.y=a.y; t.z=a.w; t.w=a.z; return(t); };
static vec4 v4xzyw(vec4u a) { vec4 t; t.x=a.x; t.y=a.z; t.z=a.y; t.w=a.w; return(t); };
static vec4 v4xzwy(vec4u a) { vec4 t; t.x=a.x; t.y=a.z; t.z=a.w; t.w=a.y; return(t); };
static vec4 v4xwyz(vec4u a) { vec4 t; t.x=a.x; t.y=a.w; t.z=a.y; t.w=a.z; return(t); };
static vec4 v4xwzy(vec4u a) { vec4 t; t.x=a.x; t.y=a.w; t.z=a.z; t.w=a.y; return(t); };
static vec4 v4yxzw(vec4u a) { vec4 t; t.x=a.y; t.y=a.x; t.z=a.z; t.w=a.w; return(t); };
static vec4 v4yxwz(vec4u a) { vec4 t; t.x=a.y; t.y=a.x; t.z=a.w; t.w=a.z; return(t); };
static vec4 v4yzxw(vec4u a) { vec4 t; t.x=a.y; t.y=a.z; t.z=a.x; t.w=a.w; return(t); };
static vec4 v4yzwx(vec4u a) { vec4 t; t.x=a.y; t.y=a.z; t.z=a.w; t.w=a.x; return(t); };
static vec4 v4ywxz(vec4u a) { vec4 t; t.x=a.y; t.y=a.w; t.z=a.x; t.w=a.z; return(t); };
static vec4 v4ywzx(vec4u a) { vec4 t; t.x=a.y; t.y=a.w; t.z=a.z; t.w=a.x; return(t); };
static vec4 v4zxyw(vec4u a) { vec4 t; t.x=a.z; t.y=a.x; t.z=a.y; t.w=a.w; return(t); };
static vec4 v4zxwy(vec4u a) { vec4 t; t.x=a.z; t.y=a.x; t.z=a.w; t.w=a.y; return(t); };
static vec4 v4zyxw(vec4u a) { vec4 t; t.x=a.z; t.y=a.y; t.z=a.x; t.w=a.w; return(t); };
static vec4 v4zywx(vec4u a) { vec4 t; t.x=a.z; t.y=a.y; t.z=a.w; t.w=a.x; return(t); };
static vec4 v4zwxy(vec4u a) { vec4 t; t.x=a.z; t.y=a.w; t.z=a.x; t.w=a.y; return(t); };
static vec4 v4zwyx(vec4u a) { vec4 t; t.x=a.z; t.y=a.w; t.z=a.y; t.w=a.x; return(t); };
static vec4 v4wxyz(vec4u a) { vec4 t; t.x=a.w; t.y=a.x; t.z=a.y; t.w=a.z; return(t); };
static vec4 v4wxzy(vec4u a) { vec4 t; t.x=a.w; t.y=a.x; t.z=a.z; t.w=a.y; return(t); };
static vec4 v4wyxz(vec4u a) { vec4 t; t.x=a.w; t.y=a.y; t.z=a.x; t.w=a.z; return(t); };
static vec4 v4wyzx(vec4u a) { vec4 t; t.x=a.w; t.y=a.y; t.z=a.z; t.w=a.x; return(t); };
static vec4 v4wzxy(vec4u a) { vec4 t; t.x=a.w; t.y=a.z; t.z=a.x; t.w=a.y; return(t); };
static vec4 v4wzyx(vec4u a) { vec4 t; t.x=a.w; t.y=a.z; t.z=a.y; t.w=a.x; return(t); };

static float qx(vec4u a)	{ return(a.x); };
static float qy(vec4u a)	{ return(a.y); };
static float qz(vec4u a)	{ return(a.z); };
static float qw(vec4u a)	{ return(a.w); };

static float qi(vec4u a)	{ return(a.x); };
static float qj(vec4u a)	{ return(a.y); };
static float qk(vec4u a)	{ return(a.z); };
static float qr(vec4u a)	{ return(a.w); };

static float qreal(vec4u a)	{ return(a.w); };

static quat qvec(vec4u a)	{ a.w=0; return(a); };

static vec2 v2abs(vec2u a)
	{ vec2 t; t.x=fabs(a.x); t.y=fabs(a.y); return(t); };
static vec3 v3abs(vec3u a)
	{ vec3 t; t.x=fabs(a.x); t.y=fabs(a.y); t.z=fabs(a.z); return(t); };
static vec4 v4abs(vec4u a)
	{ vec4 t; t.x=fabs(a.x); t.y=fabs(a.y); t.z=fabs(a.z);
		t.w=fabs(a.w); return(t); };
static quat qabs(quatu a)
	{ quat t; t.x=fabs(a.x); t.y=fabs(a.y); t.z=fabs(a.z);
		t.w=fabs(a.w); return(t); };


static float v2dot(vec2u a, vec2u b)
	{ return(a.x*b.x+a.y*b.y); }
static float v3dot(vec3u a, vec3u b)
	{ return(a.x*b.x+a.y*b.y+a.z*b.z); }
static float v4dot(vec4u a, vec4u b)
	{ return(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w); }
static float qdot(quatu a, quatu b)
	{ return(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w); }

static float v2cross(vec2u a, vec2u b)
	{ return(a.x*b.y-a.y*b.x); }

static vec3 v3cross(vec3u a, vec3u b)
{
	vec3 c;
	c.x=(a.y*b.z)-(a.z*b.y);
	c.y=(a.z*b.x)-(a.x*b.z);
	c.z=(a.x*b.y)-(a.y*b.x);
	return(c);
}

static vec4 v4cross(vec4u a, vec4u b)
{
	vec4 c;
	c.x=(a.y*b.z)-(a.z*b.y);
	c.y=(a.z*b.x)-(a.x*b.z);
	c.z=(a.x*b.y)-(a.y*b.x);
	c.w=0;
	return(c);
}

static vec4 qcross(quatu a, quatu b)
	{ return(v4cross(a, b)); }

static float v2len(vec2u a)
	{ return(sqrt(a.x*a.x+a.y*a.y)); }
static float v3len(vec3u a)
	{ return(sqrt(a.x*a.x+a.y*a.y+a.z*a.z)); }
static float v4len(vec4u a)
	{ return(sqrt(a.x*a.x+a.y*a.y+a.z*a.z+a.w*a.w)); }
static float qlen(quatu a)
	{ return(sqrt(a.x*a.x+a.y*a.y+a.z*a.z+a.w*a.w)); }

static vec2 v2norm(vec2u a)
{	vec2 c; float l;
	l=v2len(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l;
	return(c); }
static vec3 v3norm(vec3u a)
{	vec3 c; float l;
	l=v3len(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l; c.z=a.z*l;
	return(c); }
static vec4 v4norm(vec4u a)
{	vec4 c; float l;
	l=v4len(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l; c.z=a.z*l; c.w=a.w*l;
	return(c); }
static quat qnorm(quatu a)
{	vec4 c; float l;
	l=v4len(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l; c.z=a.z*l; c.w=a.w*l;
	return(c); }

static vec2 v2neg(vec2u a)
	{ vec2 c; c.x=-a.x; c.y=-a.y; return(c); }
static vec3 v3neg(vec3u a)
	{ vec3 c; c.x=-a.x; c.y=-a.y; c.z=-a.z; return(c); }
static vec4 v4neg(vec4u a)
	{ vec4 c; c.x=-a.x; c.y=-a.y; c.z=-a.z; c.w=-a.w; return(c); }

static quat qneg(quatu a)
	{ quat c; c.x=-a.x; c.y=-a.y; c.z=-a.z; c.w=-a.w; return(c); }
static quat qconj(quatu a)
	{ quat c; c.x=-a.x; c.y=-a.y; c.z=-a.z; c.w=a.w; return(c); }

static quat qrcp(quatu a)
{
	quat c; float f;
	f=(a.w*a.w)+(a.x*a.x)+(a.y*a.y)+(a.z*a.z);
	c.x=-a.x/f; c.y=-a.y/f; c.z=-a.z/f; c.w=a.w/f;
	return(c);
}


static vec2 v2add(vec2u a, vec2u b)
	{ vec2 c; c.x=a.x+b.x; c.y=a.y+b.y; return(c); }
static vec2 v2sub(vec2u a, vec2u b)
	{ vec2 c; c.x=a.x-b.x; c.y=a.y-b.y; return(c); }
static vec2 v2mul(vec2u a, vec2u b)
	{ vec2 c; c.x=a.x*b.x; c.y=a.y*b.y; return(c); }
static vec2 v2div(vec2u a, vec2u b)
	{ vec2 c; c.x=a.x/b.x; c.y=a.y/b.y; return(c); }

static vec3 v3add(vec3u a, vec3u b)
	{ vec3 c; c.x=a.x+b.x; c.y=a.y+b.y; c.z=a.z+b.z; return(c); }
static vec3 v3sub(vec3u a, vec3u b)
	{ vec3 c; c.x=a.x-b.x; c.y=a.y-b.y; c.z=a.z-b.z; return(c); }
static vec3 v3mul(vec3u a, vec3u b)
	{ vec3 c; c.x=a.x*b.x; c.y=a.y*b.y; c.z=a.z*b.z; return(c); }
static vec3 v3div(vec3u a, vec3u b)
	{ vec3 c; c.x=a.x/b.x; c.y=a.y/b.y; c.z=a.z/b.z; return(c); }

static vec4 v4add(vec4u a, vec4u b)
	{vec4 c;c.x=a.x+b.x;c.y=a.y+b.y;c.z=a.z+b.z;c.w=a.w+b.w;return(c);}
static vec4 v4sub(vec4u a, vec4u b)
	{vec4 c;c.x=a.x-b.x;c.y=a.y-b.y;c.z=a.z-b.z;c.w=a.w-b.w;return(c);}
static vec4 v4mul(vec4u a, vec4u b)
	{vec4 c;c.x=a.x*b.x;c.y=a.y*b.y;c.z=a.z*b.z;c.w=a.w*b.w;return(c);}
static vec4 v4div(vec4u a, vec4u b)
	{vec4 c;c.x=a.x/b.x;c.y=a.y/b.y;c.z=a.z/b.z;c.w=a.w/b.w;return(c);}

static quat qadd(quatu a, quatu b)
	{vec4 c;c.x=a.x+b.x;c.y=a.y+b.y;c.z=a.z+b.z;c.w=a.w+b.w;return(c);}
static quat qsub(quatu a, quatu b)
	{vec4 c;c.x=a.x-b.x;c.y=a.y-b.y;c.z=a.z-b.z;c.w=a.w-b.w;return(c);}

static quat qmul(quatu a, quatu b)
{
	quat c;
	c.x=(a.w*b.x)+(a.x*b.w)+(a.y*b.z)-(a.z*b.y);
	c.y=(a.w*b.y)+(a.y*b.w)+(a.z*b.x)-(a.x*b.z);
	c.z=(a.w*b.z)+(a.z*b.w)+(a.x*b.y)-(a.y*b.x);
	c.w=(a.w*b.w)-(a.x*b.x)-(a.y*b.y)-(a.z*b.z);
	return(c);
}

static quat qdiv(quatu a, quatu b)
	{ return(qmul(a, qrcp(b))); }
static quat qrdiv(quatu a, quatu b)
	{ return(qmul(qrcp(b), a)); }


static float v2dist(vec2u a, vec2u b) { return(v2len(v2sub(a, b))); }
static float v3dist(vec3u a, vec3u b) { return(v3len(v3sub(a, b))); }
static float v4dist(vec4u a, vec4u b) { return(v4len(v4sub(a, b))); }
static float qdist(quatu a, quatu b) { return(qlen(qsub(a, b))); }

static vec2 v2scale(vec2u a, float f)
	{ vec2 c; c.x=a.x*f; c.y=a.y*f; return(c); }
static vec3 v3scale(vec3u a, float f)
	{ vec3 c; c.x=a.x*f; c.y=a.y*f; c.z=a.z*f; return(c); }
static vec4 v4scale(vec4u a, float f)
	{ vec4 c; c.x=a.x*f; c.y=a.y*f; c.z=a.z*f; c.w=a.w*f; return(c); }
static quat qscale(quatu a, float f)
	{ quat c; c.x=a.x*f; c.y=a.y*f; c.z=a.z*f; c.w=a.w*f; return(c); }


static int v2eqp(vec2u a, vec2u b)
	{ return((a.x==b.x) && (a.y==b.y)); }
static int v3eqp(vec3u a, vec3u b)
	{ return((a.x==b.x) && (a.y==b.y) && (a.z==b.z)); }
static int v4eqp(vec4u a, vec4u b)
	{ return((a.x==b.x) && (a.y==b.y) && (a.z==b.z) && (a.w==b.w)); }
static int qeqp(quatu a, quatu b)
	{ return((a.x==b.x) && (a.y==b.y) && (a.z==b.z) && (a.w==b.w)); }

static int v2neqp(vec2u a, vec2u b)
	{ return((a.x!=b.x) || (a.y!=b.y)); }
static int v3neqp(vec3u a, vec3u b)
	{ return((a.x!=b.x) || (a.y!=b.y) || (a.z!=b.z)); }
static int v4neqp(vec4u a, vec4u b)
	{ return((a.x!=b.x) || (a.y!=b.y) || (a.z!=b.z) || (a.w!=b.w)); }
static int qneqp(quatu a, quatu b)
	{ return((a.x!=b.x) || (a.y!=b.y) || (a.z!=b.z) || (a.w!=b.w)); }


static vec2 v2addscale(vec2u a, vec2u b, float f)
	{ return(v2add(a, v2mul(b, vec2(f, f)))); }
static vec3 v3addscale(vec3u a, vec3u b, float f)
	{ return(v3add(a, v3mul(b, vec3(f, f, f)))); }
static vec4 v4addscale(vec4u a, vec4u b, float f)
	{ return(v4add(a, v4mul(b, vec4(f, f, f, f)))); }
static quat qaddscale(quatu a, quatu b, float f)
	{ return(v4add(a, v4mul(b, vec4(f, f, f, f)))); }

static vec2 v2scaleaddscale(vec2u a, float f, vec2u b, float g)
	{ return(v2add(v2mul(a, vec2(f, f)), v2mul(b, vec2(g, g)))); }
static vec3 v3scaleaddscale(vec3u a, float f, vec3u b, float g)
	{ return(v3add(v3mul(a, vec3(f, f, f)), v3mul(b, vec3(g, g, g)))); }
static vec4 v4scaleaddscale(vec4u a, float f, vec4u b, float g)
	{ return(v4add(v4mul(a, vec4(f, f, f, f)), v4mul(b, vec4(g, g, g, g)))); }
static quat qscaleaddscale(quatu a, float f, quatu b, float g)
	{ return(v4add(v4mul(a, vec4(f, f, f, f)), v4mul(b, vec4(g, g, g, g)))); }

static vec2 v2scaleadd2(vec2u a, float f, vec2u b, float g)
	{ return(v2add(v2mul(a, vec2(f, f)), v2mul(b, vec2(g, g)))); }
static vec3 v3scaleadd2(vec3u a, float f, vec3u b, float g)
	{ return(v3add(v3mul(a, vec3(f, f, f)), v3mul(b, vec3(g, g, g)))); }
static vec4 v4scaleadd2(vec4u a, float f, vec4u b, float g)
	{ return(v4add(v4mul(a, vec4(f, f, f, f)), v4mul(b, vec4(g, g, g, g)))); }
static quat qscaleadd2(quatu a, float f, quatu b, float g)
	{ return(v4add(v4mul(a, vec4(f, f, f, f)), v4mul(b, vec4(g, g, g, g)))); }

static vec2 v2scaleadd3(vec2u a, float f, vec2u b, float g, vec2u c, float h)
{
	return(v2add(
		v2mul(a, vec2(f, f)),
		v2add(	v2mul(b, vec2(g, g)),
			v2mul(c, vec2(h, h)))
		));
}
static vec3 v3scaleadd3(vec3u a, float f, vec3u b, float g, vec3u c, float h)
{
	return(v3add(
		v3mul(a, vec3(f, f, f)),
		v3add(	v3mul(b, vec3(g, g, g)),
			v3mul(c, vec3(h, h, h)))
		));
}
static vec4 v4scaleadd3(vec4u a, float f, vec4u b, float g, vec4u c, float h)
{
	return(v4add(
		v4mul(a, vec4(f, f, f, f)),
		v4add(	v4mul(b, vec4(g, g, g, g)),
			v4mul(c, vec4(h, h, h, h)))
		));
}

static quat qscaleadd3(quatu a, float f, quatu b, float g, quatu c, float h)
{
	return(v4add(
		v4mul(a, vec4(f, f, f, f)),
		v4add(	v4mul(b, vec4(g, g, g, g)),
			v4mul(c, vec4(h, h, h, h)))
		));
}

static vec2 v2scaleadd4(vec2u a, float f, vec2u b, float g, vec2u c, float h, vec2u d, float i)
{
	return(v2add(
		v2add(	v2mul(a, vec2(f, f)),
			v2mul(b, vec2(g, g))),
		v2add(	v2mul(c, vec2(h, h)),
			v2mul(d, vec2(i, i)))
		));
}

static vec3 v3scaleadd4(vec3u a, float f, vec3u b, float g, vec3u c, float h, vec3u d, float i)
{
	return(v3add(
		v3add(	v3mul(a, vec3(f, f, f)),
			v3mul(b, vec3(g, g, g))),
		v3add(	v3mul(c, vec3(h, h, h)),
			v3mul(d, vec3(i, i, i)))
		));
}

static vec4 v4scaleadd4(vec4u a, float f, vec4u b, float g, vec4u c, float h, vec4u d, float i)
{
	return(v4add(
		v4add(	v4mul(a, vec4(f, f, f, f)),
			v4mul(b, vec4(g, g, g, g))),
		v4add(	v4mul(c, vec4(h, h, h, h)),
			v4mul(d, vec4(i, i, i, i)))
		));
}

static quat qscaleadd4(quatu a, float f, quatu b, float g, quatu c, float h, quatu d, float i)
{
	return(v4add(
		v4add(	v4mul(a, vec4(f, f, f, f)),
			v4mul(b, vec4(g, g, g, g))),
		v4add(	v4mul(c, vec4(h, h, h, h)),
			v4mul(d, vec4(i, i, i, i)))
		));
}
