#define vec2d(x,y)		_vec2di(x,y)
#define vec3d(x,y,z)	_vec3di(x,y,z)
#define vec4d(x,y,z,w)	_vec4di(x,y,z,w)
#define quatd(w,x,y,z)	_quatdi(w,x,y,z)

#define vec2dvf(v)	_vec2dvf(v)
#define vec3dvf(v)	_vec3dvf(v)
#define vec4dvf(v)	_vec4dvf(v)
#define quatdvf(v)	_quatdvf(v)		//XYZW
#define quatdvf2(v)	_quatdvf2(v)	//WXYZ

#define vec2dvd(v)	_vec2dvd(v)
#define vec3dvd(v)	_vec3dvd(v)
#define vec4dvd(v)	_vec4dvd(v)
#define quatdvd(v)	_quatdvd(v)		//XYZW
#define quatdvd2(v)	_quatdvd2(v)	//WXYZ

#define pfvec2d(v)	_vfvec2d(v)
#define pfvec3d(v)	_vfvec4d(v)
#define pfvec4d(v)	_vfvec4d(v)
#define pfquatd(v)	_vfvec4d(v)		//XYZW
#define pfquatd2(v)	_vf2vec4d(v)	//WXYZ

#define pdvec2d(v)	_vdvec2d(v)
#define pdvec3d(v)	_vdvec4d(v)
#define pdvec4d(v)	_vdvec4d(v)
#define pdquatd(v)	_vdvec4d(v)		//XYZW
#define pdquatd2(v)	_vd2vec4d(v)	//WXYZ

#define vfvec2d(p,v)	_vfavec2d(p,v)
#define vfvec3d(p,v)	_vfavec3d(p,v)
#define vfvec4d(p,v)	_vfavec4d(p,v)
#define vfquatd(p,v)	_vfavec4d(p,v)		//XYZW
#define vfquat2d(p,v)	_vfa2vec4d(p,v)		//WXYZ

#define vdvec2d(p,v)	_vdavec2d(p,v)
#define vdvec3d(p,v)	_vdavec3d(p,v)
#define vdvec4d(p,v)	_vdavec4d(p,v)
#define vdquatd(p,v)	_vdavec4d(p,v)		//XYZW
#define vdquat2d(p,v)	_vda2vec4d(p,v)		//WXYZ

#define v4dshuf(a,sh)	_vf4d_shuf(a,sh)

#define v2dzero()	vec2d(0,0)
#define v3dzero()	vec3d(0,0,0)
#define v4dzero()	vec4d(0,0,0,0)
#define qdzero()	quatd(0,0,0,0)

typedef struct {double x,y;} vec2du;
typedef struct {double x,y,z,w;} vec4du;
typedef vec4du vec3du;
typedef vec4du quatdu;

typedef vec2du vec2d;
typedef vec4du vec3d;
typedef vec4du vec4d;
typedef vec4du quatd;

typedef dytAlign(8) vec2du vec2da;
typedef dytAlign(16) vec4du vec3da;
typedef dytAlign(16) vec4du vec4da;
typedef dytAlign(16) vec4du quatda;

static vec2d _vec2di(double x, double y)
	{ vec2d t; t.x=x; t.y=y; return(t); };
static vec3d _vec3di(double x, double y, double z)
	{ vec3d t; t.x=x; t.y=y; t.z=z; return(t); };
static vec4d _vec4di(double x, double y, double z, double w)
	{ vec4d t; t.x=x; t.y=y; t.z=z; t.w=w; return(t); };
static quatd _quatdi(double w, double x, double y, double z)
	{ quatd t; t.x=x; t.y=y; t.z=z; t.w=w; return(t); };

static vec2d _vec2dvf(float *x)
	{ return(_vec2di(x[0], x[1])); };
static vec3d _vec3dvf(float *x)
	{ return(_vec3di(x[0], x[1], x[2])); };
static vec4d _vec4dvf(float *x)
	{ return(_vec4di(x[0], x[1], x[2], x[3])); };
static quatd _quatdvf(float *x)
	{ return(_vec4di(x[3], x[0], x[1], x[2])); };
static quatd _quatdvf2(float *x)
	{ return(_quatdi(x[0], x[1], x[2], x[3])); };

static vec2d _vec2dvd(double *x)
	{ return(_vec2di(x[0], x[1])); };
static vec3d _vec3dvd(double *x)
	{ return(_vec3di(x[0], x[1], x[2])); };
static vec4d _vec4dvd(double *x)
	{ return(_vec4di(x[0], x[1], x[2], x[3])); };
static quatd _quatdvd(double *x)
	{ return(_vec4di(x[3], x[0], x[1], x[2])); };
static quatd _quatdvd2(double *x)
	{ return(_quatdi(x[0], x[1], x[2], x[3])); };

static float *_vfvec2d(vec2du v)
{
	float *p;
	p=(float *)gcralloc(4*sizeof(float));
	p[0]=v.x; p[1]=v.y; p[2]=0; p[3]=0;
	return(p);
}

static float *_vfvec4d(vec4du v)
{
	float *p;
	p=(float *)gcralloc(4*sizeof(float));
	p[0]=v.x; p[1]=v.y; p[2]=v.z; p[3]=v.w;
	return(p);
}

static float *_vf2vec4d(vec4du v)
{
	float *p;
	p=(float *)gcralloc(4*sizeof(float));
	p[0]=v.w; p[1]=v.x; p[2]=v.y; p[3]=v.z;
	return(p);
}

static double *_vdvec2d(vec2du v)
{
	double *p;
	p=(double *)gcralloc(4*sizeof(double));
	p[0]=v.x; p[1]=v.y; p[2]=0; p[3]=0;
	return(p);
}

static double *_vdvec4d(vec4du v)
{
	double *p;
	p=(double *)gcralloc(4*sizeof(double));
	p[0]=v.x; p[1]=v.y; p[2]=v.z; p[3]=v.w;
	return(p);
}

static double *_vd2vec4d(vec4du v)
{
	double *p;
	p=(double *)gcralloc(4*sizeof(double));
	p[0]=v.w; p[1]=v.x; p[2]=v.y; p[3]=v.z;
	return(p);
}

static void _vfavec2d(float *p, vec2du v)
	{ p[0]=v.x; p[1]=v.y; }
static void _vfavec3d(float *p, vec4du v)
	{ p[0]=v.x; p[1]=v.y; p[2]=v.z; }
static void _vfavec4d(float *p, vec4du v)
	{ p[0]=v.x; p[1]=v.y; p[2]=v.z; p[3]=v.w; }
static void _vfa2vec4d(float *p, vec4du v)
	{ p[0]=v.w; p[1]=v.x; p[2]=v.y; p[3]=v.z; }

static void _vdavec2d(double *p, vec2du v)
	{ p[0]=v.x; p[1]=v.y; }
static void _vdavec3d(double *p, vec4du v)
	{ p[0]=v.x; p[1]=v.y; p[2]=v.z; }
static void _vdavec4d(double *p, vec4du v)
	{ p[0]=v.x; p[1]=v.y; p[2]=v.z; p[3]=v.w; }
static void _vda2vec4d(double *p, vec4du v)
	{ p[0]=v.w; p[1]=v.x; p[2]=v.y; p[3]=v.z; }

static double _vf4d_getidx(vec4du v, int i)
{
	double f;
	switch(i)
	{
	case 0: f=v.x; break; case 1: f=v.y; break;
	case 2: f=v.z; break; case 3: f=v.w; break;
	default: f=0; break;
	}
	return(f);
}

static vec4d _vf4d_shuf(vec4du v, int sh)
{
	vec4d b;
	b.x=_vf4d_getidx(v, sh&3);
	b.y=_vf4d_getidx(v, (sh>>2)&3);
	b.z=_vf4d_getidx(v, (sh>>4)&3);
	b.w=_vf4d_getidx(v, (sh>>6)&3);
	return(b);
}


static double v2dx(vec2du a)	{ return(a.x); };
static double v2dy(vec2du a)	{ return(a.y); };

static double v3dx(vec3du a)	{ return(a.x); };
static double v3dy(vec3du a)	{ return(a.y); };
static double v3dz(vec3du a)	{ return(a.z); };

static double v4dx(vec4du a)	{ return(a.x); };
static double v4dy(vec4du a)	{ return(a.y); };
static double v4dz(vec4du a)	{ return(a.z); };
static double v4dw(vec4du a)	{ return(a.w); };

static vec2d v2dxy(vec2du a)	{ vec2d t; t.x=a.x; t.y=a.y; return(t); };
static vec2d v2dyx(vec2du a)	{ vec2d t; t.x=a.y; t.y=a.x; return(t); };

static vec2d v3dxy(vec3du a)	{ vec2d t; t.x=a.x; t.y=a.y; return(t); };
static vec2d v3dxz(vec3du a)	{ vec2d t; t.x=a.x; t.y=a.z; return(t); };
static vec2d v3dyx(vec3du a)	{ vec2d t; t.x=a.y; t.y=a.x; return(t); };
static vec2d v3dyz(vec3du a)	{ vec2d t; t.x=a.y; t.y=a.z; return(t); };
static vec2d v3dzx(vec3du a)	{ vec2d t; t.x=a.z; t.y=a.x; return(t); };
static vec2d v3dzy(vec3du a)	{ vec2d t; t.x=a.z; t.y=a.y; return(t); };

static vec3d v3dxyz(vec3du a) { vec3d t; t.x=a.x; t.y=a.y; t.z=a.z; return(t); };
static vec3d v3dxzy(vec3du a) { vec3d t; t.x=a.x; t.y=a.z; t.z=a.y; return(t); };
static vec3d v3dyxz(vec3du a) { vec3d t; t.x=a.y; t.y=a.x; t.z=a.z; return(t); };
static vec3d v3dyzx(vec3du a) { vec3d t; t.x=a.y; t.y=a.z; t.z=a.x; return(t); };
static vec3d v3dzxy(vec3du a) { vec3d t; t.x=a.z; t.y=a.x; t.z=a.y; return(t); };
static vec3d v3dzyx(vec3du a) { vec3d t; t.x=a.z; t.y=a.y; t.z=a.x; return(t); };

static vec2d v4dxy(vec4du a)	{ vec2d t; t.x=a.x; t.y=a.y; return(t); };
static vec2d v4dxz(vec4du a)	{ vec2d t; t.x=a.x; t.y=a.z; return(t); };
static vec2d v4dxw(vec4du a)	{ vec2d t; t.x=a.x; t.y=a.w; return(t); };
static vec2d v4dyx(vec4du a)	{ vec2d t; t.x=a.y; t.y=a.x; return(t); };
static vec2d v4dyz(vec4du a)	{ vec2d t; t.x=a.y; t.y=a.z; return(t); };
static vec2d v4dyw(vec4du a)	{ vec2d t; t.x=a.y; t.y=a.w; return(t); };
static vec2d v4dzx(vec4du a)	{ vec2d t; t.x=a.z; t.y=a.x; return(t); };
static vec2d v4dzy(vec4du a)	{ vec2d t; t.x=a.z; t.y=a.y; return(t); };
static vec2d v4dzw(vec4du a)	{ vec2d t; t.x=a.z; t.y=a.w; return(t); };
static vec2d v4dwx(vec4du a)	{ vec2d t; t.x=a.w; t.y=a.x; return(t); };
static vec2d v4dwy(vec4du a)	{ vec2d t; t.x=a.w; t.y=a.y; return(t); };
static vec2d v4dwz(vec4du a)	{ vec2d t; t.x=a.w; t.y=a.z; return(t); };

static vec3d v4dxyz(vec4du a) { vec3d t; t.x=a.x; t.y=a.y; t.z=a.z; return(t); };
static vec3d v4dxyw(vec4du a) { vec3d t; t.x=a.x; t.y=a.y; t.z=a.w; return(t); };
static vec3d v4dxzy(vec4du a) { vec3d t; t.x=a.x; t.y=a.z; t.z=a.y; return(t); };
static vec3d v4dxzw(vec4du a) { vec3d t; t.x=a.x; t.y=a.z; t.z=a.w; return(t); };
static vec3d v4dxwy(vec4du a) { vec3d t; t.x=a.x; t.y=a.w; t.z=a.y; return(t); };
static vec3d v4dxwz(vec4du a) { vec3d t; t.x=a.x; t.y=a.w; t.z=a.z; return(t); };
static vec3d v4dyxz(vec4du a) { vec3d t; t.x=a.y; t.y=a.x; t.z=a.z; return(t); };
static vec3d v4dyxw(vec4du a) { vec3d t; t.x=a.y; t.y=a.x; t.z=a.w; return(t); };
static vec3d v4dyzx(vec4du a) { vec3d t; t.x=a.y; t.y=a.z; t.z=a.x; return(t); };
static vec3d v4dyzw(vec4du a) { vec3d t; t.x=a.y; t.y=a.z; t.z=a.w; return(t); };
static vec3d v4dywx(vec4du a) { vec3d t; t.x=a.y; t.y=a.w; t.z=a.x; return(t); };
static vec3d v4dywz(vec4du a) { vec3d t; t.x=a.y; t.y=a.w; t.z=a.z; return(t); };
static vec3d v4dzxy(vec4du a) { vec3d t; t.x=a.z; t.y=a.x; t.z=a.y; return(t); };
static vec3d v4dzxw(vec4du a) { vec3d t; t.x=a.z; t.y=a.x; t.z=a.w; return(t); };
static vec3d v4dzyx(vec4du a) { vec3d t; t.x=a.z; t.y=a.y; t.z=a.x; return(t); };
static vec3d v4dzyw(vec4du a) { vec3d t; t.x=a.z; t.y=a.y; t.z=a.w; return(t); };
static vec3d v4dzwx(vec4du a) { vec3d t; t.x=a.z; t.y=a.w; t.z=a.x; return(t); };
static vec3d v4dzwy(vec4du a) { vec3d t; t.x=a.z; t.y=a.w; t.z=a.y; return(t); };
static vec3d v4dwxy(vec4du a) { vec3d t; t.x=a.w; t.y=a.x; t.z=a.y; return(t); };
static vec3d v4dwxz(vec4du a) { vec3d t; t.x=a.w; t.y=a.x; t.z=a.z; return(t); };
static vec3d v4dwyx(vec4du a) { vec3d t; t.x=a.w; t.y=a.y; t.z=a.x; return(t); };
static vec3d v4dwyz(vec4du a) { vec3d t; t.x=a.w; t.y=a.y; t.z=a.z; return(t); };
static vec3d v4dwzx(vec4du a) { vec3d t; t.x=a.w; t.y=a.z; t.z=a.x; return(t); };
static vec3d v4dwzy(vec4du a) { vec3d t; t.x=a.w; t.y=a.z; t.z=a.y; return(t); };

static vec4d v4dxyzw(vec4du a) { vec4d t; t.x=a.x; t.y=a.y; t.z=a.z; t.w=a.w; return(t); };
static vec4d v4dxywz(vec4du a) { vec4d t; t.x=a.x; t.y=a.y; t.z=a.w; t.w=a.z; return(t); };
static vec4d v4dxzyw(vec4du a) { vec4d t; t.x=a.x; t.y=a.z; t.z=a.y; t.w=a.w; return(t); };
static vec4d v4dxzwy(vec4du a) { vec4d t; t.x=a.x; t.y=a.z; t.z=a.w; t.w=a.y; return(t); };
static vec4d v4dxwyz(vec4du a) { vec4d t; t.x=a.x; t.y=a.w; t.z=a.y; t.w=a.z; return(t); };
static vec4d v4dxwzy(vec4du a) { vec4d t; t.x=a.x; t.y=a.w; t.z=a.z; t.w=a.y; return(t); };
static vec4d v4dyxzw(vec4du a) { vec4d t; t.x=a.y; t.y=a.x; t.z=a.z; t.w=a.w; return(t); };
static vec4d v4dyxwz(vec4du a) { vec4d t; t.x=a.y; t.y=a.x; t.z=a.w; t.w=a.z; return(t); };
static vec4d v4dyzxw(vec4du a) { vec4d t; t.x=a.y; t.y=a.z; t.z=a.x; t.w=a.w; return(t); };
static vec4d v4dyzwx(vec4du a) { vec4d t; t.x=a.y; t.y=a.z; t.z=a.w; t.w=a.x; return(t); };
static vec4d v4dywxz(vec4du a) { vec4d t; t.x=a.y; t.y=a.w; t.z=a.x; t.w=a.z; return(t); };
static vec4d v4dywzx(vec4du a) { vec4d t; t.x=a.y; t.y=a.w; t.z=a.z; t.w=a.x; return(t); };
static vec4d v4dzxyw(vec4du a) { vec4d t; t.x=a.z; t.y=a.x; t.z=a.y; t.w=a.w; return(t); };
static vec4d v4dzxwy(vec4du a) { vec4d t; t.x=a.z; t.y=a.x; t.z=a.w; t.w=a.y; return(t); };
static vec4d v4dzyxw(vec4du a) { vec4d t; t.x=a.z; t.y=a.y; t.z=a.x; t.w=a.w; return(t); };
static vec4d v4dzywx(vec4du a) { vec4d t; t.x=a.z; t.y=a.y; t.z=a.w; t.w=a.x; return(t); };
static vec4d v4dzwxy(vec4du a) { vec4d t; t.x=a.z; t.y=a.w; t.z=a.x; t.w=a.y; return(t); };
static vec4d v4dzwyx(vec4du a) { vec4d t; t.x=a.z; t.y=a.w; t.z=a.y; t.w=a.x; return(t); };
static vec4d v4dwxyz(vec4du a) { vec4d t; t.x=a.w; t.y=a.x; t.z=a.y; t.w=a.z; return(t); };
static vec4d v4dwxzy(vec4du a) { vec4d t; t.x=a.w; t.y=a.x; t.z=a.z; t.w=a.y; return(t); };
static vec4d v4dwyxz(vec4du a) { vec4d t; t.x=a.w; t.y=a.y; t.z=a.x; t.w=a.z; return(t); };
static vec4d v4dwyzx(vec4du a) { vec4d t; t.x=a.w; t.y=a.y; t.z=a.z; t.w=a.x; return(t); };
static vec4d v4dwzxy(vec4du a) { vec4d t; t.x=a.w; t.y=a.z; t.z=a.x; t.w=a.y; return(t); };
static vec4d v4dwzyx(vec4du a) { vec4d t; t.x=a.w; t.y=a.z; t.z=a.y; t.w=a.x; return(t); };

static double qdx(vec4du a)	{ return(a.x); };
static double qdy(vec4du a)	{ return(a.y); };
static double qdz(vec4du a)	{ return(a.z); };
static double qdw(vec4du a)	{ return(a.w); };

static double qdi(vec4du a)	{ return(a.x); };
static double qdj(vec4du a)	{ return(a.y); };
static double qdk(vec4du a)	{ return(a.z); };
static double qdr(vec4du a)	{ return(a.w); };

static double qdreal(vec4du a)	{ return(a.w); };

static quatd qdvec(vec4du a)	{ a.w=0; return(a); };

static vec2d v2dabs(vec2du a)
	{ vec2d t; t.x=fabs(a.x); t.y=fabs(a.y); return(t); };
static vec3d v3dabs(vec3du a)
	{ vec3d t; t.x=fabs(a.x); t.y=fabs(a.y); t.z=fabs(a.z); return(t); };
static vec4d v4dabs(vec4du a)
	{ vec4d t; t.x=fabs(a.x); t.y=fabs(a.y); t.z=fabs(a.z);
		t.w=fabs(a.w); return(t); };
static quatd qdabs(quatdu a)
	{ quatd t; t.x=fabs(a.x); t.y=fabs(a.y); t.z=fabs(a.z);
		t.w=fabs(a.w); return(t); };


static double v2ddot(vec2du a, vec2du b)
	{ return(a.x*b.x+a.y*b.y); }
static double v3ddot(vec3du a, vec3du b)
	{ return(a.x*b.x+a.y*b.y+a.z*b.z); }
static double v4ddot(vec4du a, vec4du b)
	{ return(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w); }
static double qddot(quatdu a, quatdu b)
	{ return(a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w); }

static double v2dcross(vec2du a, vec2du b)
	{ return(a.x*b.y-a.y*b.x); }

static vec3d v3dcross(vec3du a, vec3du b)
{
	vec3d c;
	c.x=(a.y*b.z)-(a.z*b.y);
	c.y=(a.z*b.x)-(a.x*b.z);
	c.z=(a.x*b.y)-(a.y*b.x);
	return(c);
}

static vec4d v4dcross(vec4du a, vec4du b)
{
	vec4d c;
	c.x=(a.y*b.z)-(a.z*b.y);
	c.y=(a.z*b.x)-(a.x*b.z);
	c.z=(a.x*b.y)-(a.y*b.x);
	c.w=0;
	return(c);
}

static vec4d qdcross(quatdu a, quatdu b)
	{ return(v4dcross(a, b)); }

static double v2dlen(vec2du a)
	{ return(sqrt(a.x*a.x+a.y*a.y)); }
static double v3dlen(vec3du a)
	{ return(sqrt(a.x*a.x+a.y*a.y+a.z*a.z)); }
static double v4dlen(vec4du a)
	{ return(sqrt(a.x*a.x+a.y*a.y+a.z*a.z+a.w*a.w)); }
static double qdlen(quatdu a)
	{ return(sqrt(a.x*a.x+a.y*a.y+a.z*a.z+a.w*a.w)); }

static vec2d v2dnorm(vec2du a)
{	vec2d c; double l;
	l=v2dlen(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l;
	return(c); }
static vec3d v3dnorm(vec3du a)
{	vec3d c; double l;
	l=v3dlen(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l; c.z=a.z*l;
	return(c); }
static vec4d v4dnorm(vec4du a)
{	vec4d c; double l;
	l=v4dlen(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l; c.z=a.z*l; c.w=a.w*l;
	return(c); }
static quatd qdnorm(quatdu a)
{	vec4d c; double l;
	l=v4dlen(a); if(l!=0)l=1.0/l;
	c.x=a.x*l; c.y=a.y*l; c.z=a.z*l; c.w=a.w*l;
	return(c); }

static vec2d v2dneg(vec2du a)
	{ vec2d c; c.x=-a.x; c.y=-a.y; return(c); }
static vec3d v3dneg(vec3du a)
	{ vec3d c; c.x=-a.x; c.y=-a.y; c.z=-a.z; return(c); }
static vec4d v4dneg(vec4du a)
	{ vec4d c; c.x=-a.x; c.y=-a.y; c.z=-a.z; c.w=-a.w; return(c); }

static quatd qdneg(quatdu a)
	{ quatd c; c.x=-a.x; c.y=-a.y; c.z=-a.z; c.w=-a.w; return(c); }
static quatd qdconj(quatdu a)
	{ quatd c; c.x=-a.x; c.y=-a.y; c.z=-a.z; c.w=a.w; return(c); }

static quatd qdrcp(quatdu a)
{
	quatd c; double f;
	f=(a.w*a.w)+(a.x*a.x)+(a.y*a.y)+(a.z*a.z);
	c.x=-a.x/f; c.y=-a.y/f; c.z=-a.z/f; c.w=a.w/f;
	return(c);
}


static vec2d v2dadd(vec2du a, vec2du b)
	{ vec2d c; c.x=a.x+b.x; c.y=a.y+b.y; return(c); }
static vec2d v2dsub(vec2du a, vec2du b)
	{ vec2d c; c.x=a.x-b.x; c.y=a.y-b.y; return(c); }
static vec2d v2dmul(vec2du a, vec2du b)
	{ vec2d c; c.x=a.x*b.x; c.y=a.y*b.y; return(c); }
static vec2d v2ddiv(vec2du a, vec2du b)
	{ vec2d c; c.x=a.x/b.x; c.y=a.y/b.y; return(c); }

static vec3d v3dadd(vec3du a, vec3du b)
	{ vec3d c; c.x=a.x+b.x; c.y=a.y+b.y; c.z=a.z+b.z; return(c); }
static vec3d v3dsub(vec3du a, vec3du b)
	{ vec3d c; c.x=a.x-b.x; c.y=a.y-b.y; c.z=a.z-b.z; return(c); }
static vec3d v3dmul(vec3du a, vec3du b)
	{ vec3d c; c.x=a.x*b.x; c.y=a.y*b.y; c.z=a.z*b.z; return(c); }
static vec3d v3ddiv(vec3du a, vec3du b)
	{ vec3d c; c.x=a.x/b.x; c.y=a.y/b.y; c.z=a.z/b.z; return(c); }

static vec4d v4dadd(vec4du a, vec4du b)
	{vec4d c;c.x=a.x+b.x;c.y=a.y+b.y;c.z=a.z+b.z;c.w=a.w+b.w;return(c);}
static vec4d v4dsub(vec4du a, vec4du b)
	{vec4d c;c.x=a.x-b.x;c.y=a.y-b.y;c.z=a.z-b.z;c.w=a.w-b.w;return(c);}
static vec4d v4dmul(vec4du a, vec4du b)
	{vec4d c;c.x=a.x*b.x;c.y=a.y*b.y;c.z=a.z*b.z;c.w=a.w*b.w;return(c);}
static vec4d v4ddiv(vec4du a, vec4du b)
	{vec4d c;c.x=a.x/b.x;c.y=a.y/b.y;c.z=a.z/b.z;c.w=a.w/b.w;return(c);}

static quatd qdadd(quatdu a, quatdu b)
	{vec4d c;c.x=a.x+b.x;c.y=a.y+b.y;c.z=a.z+b.z;c.w=a.w+b.w;return(c);}
static quatd qdsub(quatdu a, quatdu b)
	{vec4d c;c.x=a.x-b.x;c.y=a.y-b.y;c.z=a.z-b.z;c.w=a.w-b.w;return(c);}

static quatd qdmul(quatdu a, quatdu b)
{
	quatd c;
	c.x=(a.w*b.x)+(a.x*b.w)+(a.y*b.z)-(a.z*b.y);
	c.y=(a.w*b.y)+(a.y*b.w)+(a.z*b.x)-(a.x*b.z);
	c.z=(a.w*b.z)+(a.z*b.w)+(a.x*b.y)-(a.y*b.x);
	c.w=(a.w*b.w)-(a.x*b.x)-(a.y*b.y)-(a.z*b.z);
	return(c);
}

static quatd qddiv(quatdu a, quatdu b)
	{ return(qdmul(a, qdrcp(b))); }
static quatd qdrdiv(quatdu a, quatdu b)
	{ return(qdmul(qdrcp(b), a)); }


static double v2ddist(vec2du a, vec2du b) { return(v2dlen(v2dsub(a, b))); }
static double v3ddist(vec3du a, vec3du b) { return(v3dlen(v3dsub(a, b))); }
static double v4ddist(vec4du a, vec4du b) { return(v4dlen(v4dsub(a, b))); }
static double qddist(quatdu a, quatdu b) { return(qdlen(qdsub(a, b))); }

static vec2d v2dscale(vec2du a, double f)
	{ vec2d c; c.x=a.x*f; c.y=a.y*f; return(c); }
static vec3d v3dscale(vec3du a, double f)
	{ vec3d c; c.x=a.x*f; c.y=a.y*f; c.z=a.z*f; return(c); }
static vec4d v4dscale(vec4du a, double f)
	{ vec4d c; c.x=a.x*f; c.y=a.y*f; c.z=a.z*f; c.w=a.w*f; return(c); }
static quatd qdscale(quatdu a, double f)
	{ quatd c; c.x=a.x*f; c.y=a.y*f; c.z=a.z*f; c.w=a.w*f; return(c); }


static int v2deqp(vec2du a, vec2du b)
	{ return((a.x==b.x) && (a.y==b.y)); }
static int v3deqp(vec3du a, vec3du b)
	{ return((a.x==b.x) && (a.y==b.y) && (a.z==b.z)); }
static int v4deqp(vec4du a, vec4du b)
	{ return((a.x==b.x) && (a.y==b.y) && (a.z==b.z) && (a.w==b.w)); }
static int qdeqp(quatdu a, quatdu b)
	{ return((a.x==b.x) && (a.y==b.y) && (a.z==b.z) && (a.w==b.w)); }

static int v2dneqp(vec2du a, vec2du b)
	{ return((a.x!=b.x) || (a.y!=b.y)); }
static int v3dneqp(vec3du a, vec3du b)
	{ return((a.x!=b.x) || (a.y!=b.y) || (a.z!=b.z)); }
static int v4dneqp(vec4du a, vec4du b)
	{ return((a.x!=b.x) || (a.y!=b.y) || (a.z!=b.z) || (a.w!=b.w)); }
static int qdneqp(quatdu a, quatdu b)
	{ return((a.x!=b.x) || (a.y!=b.y) || (a.z!=b.z) || (a.w!=b.w)); }


static vec2d v2daddscale(vec2du a, vec2du b, double f)
	{ return(v2dadd(a, v2dmul(b, vec2d(f, f)))); }
static vec3d v3daddscale(vec3du a, vec3du b, double f)
	{ return(v3dadd(a, v3dmul(b, vec3d(f, f, f)))); }
static vec4d v4daddscale(vec4du a, vec4du b, double f)
	{ return(v4dadd(a, v4dmul(b, vec4d(f, f, f, f)))); }
static quatd qdaddscale(quatdu a, quatdu b, double f)
	{ return(v4dadd(a, v4dmul(b, vec4d(f, f, f, f)))); }

static vec2d v2dscaleaddscale(vec2du a, double f, vec2du b, double g)
	{ return(v2dadd(v2dmul(a, vec2d(f, f)), v2dmul(b, vec2d(g, g)))); }
static vec3d v3dscaleaddscale(vec3du a, double f, vec3du b, double g)
	{ return(v3dadd(v3dmul(a, vec3d(f, f, f)), v3dmul(b, vec3d(g, g, g)))); }
static vec4d v4dscaleaddscale(vec4du a, double f, vec4du b, double g)
	{ return(v4dadd(v4dmul(a, vec4d(f, f, f, f)), v4dmul(b, vec4d(g, g, g, g)))); }
static quatd qdscaleaddscale(quatdu a, double f, quatdu b, double g)
	{ return(v4dadd(v4dmul(a, vec4d(f, f, f, f)), v4dmul(b, vec4d(g, g, g, g)))); }

static vec2d v2dscaleadd2(vec2du a, double f, vec2du b, double g)
	{ return(v2dadd(v2dmul(a, vec2d(f, f)), v2dmul(b, vec2d(g, g)))); }
static vec3d v3dscaleadd2(vec3du a, double f, vec3du b, double g)
	{ return(v3dadd(v3dmul(a, vec3d(f, f, f)), v3dmul(b, vec3d(g, g, g)))); }
static vec4d v4dscaleadd2(vec4du a, double f, vec4du b, double g)
	{ return(v4dadd(v4dmul(a, vec4d(f, f, f, f)), v4dmul(b, vec4d(g, g, g, g)))); }
static quatd qdscaleadd2(quatdu a, double f, quatdu b, double g)
	{ return(v4dadd(v4dmul(a, vec4d(f, f, f, f)), v4dmul(b, vec4d(g, g, g, g)))); }

static vec2d v2dscaleadd3(vec2du a, double f, vec2du b, double g, vec2du c, double h)
{
	return(v2dadd(
		v2dmul(a, vec2d(f, f)),
		v2dadd(	v2dmul(b, vec2d(g, g)),
			v2dmul(c, vec2d(h, h)))
		));
}
static vec3d v3dscaleadd3(vec3du a, double f, vec3du b, double g, vec3du c, double h)
{
	return(v3dadd(
		v3dmul(a, vec3d(f, f, f)),
		v3dadd(	v3dmul(b, vec3d(g, g, g)),
			v3dmul(c, vec3d(h, h, h)))
		));
}
static vec4d v4dscaleadd3(vec4du a, double f, vec4du b, double g, vec4du c, double h)
{
	return(v4dadd(
		v4dmul(a, vec4d(f, f, f, f)),
		v4dadd(	v4dmul(b, vec4d(g, g, g, g)),
			v4dmul(c, vec4d(h, h, h, h)))
		));
}

static quatd qdscaleadd3(quatdu a, double f, quatdu b, double g, quatdu c, double h)
{
	return(v4dadd(
		v4dmul(a, vec4d(f, f, f, f)),
		v4dadd(	v4dmul(b, vec4d(g, g, g, g)),
			v4dmul(c, vec4d(h, h, h, h)))
		));
}

static vec2d v2dscaleadd4(vec2du a, double f, vec2du b, double g, vec2du c, double h, vec2du d, double i)
{
	return(v2dadd(
		v2dadd(	v2dmul(a, vec2d(f, f)),
			v2dmul(b, vec2d(g, g))),
		v2dadd(	v2dmul(c, vec2d(h, h)),
			v2dmul(d, vec2d(i, i)))
		));
}

static vec3d v3dscaleadd4(vec3du a, double f, vec3du b, double g, vec3du c, double h, vec3du d, double i)
{
	return(v3dadd(
		v3dadd(	v3dmul(a, vec3d(f, f, f)),
			v3dmul(b, vec3d(g, g, g))),
		v3dadd(	v3dmul(c, vec3d(h, h, h)),
			v3dmul(d, vec3d(i, i, i)))
		));
}

static vec4d v4dscaleadd4(vec4du a, double f, vec4du b, double g, vec4du c, double h, vec4du d, double i)
{
	return(v4dadd(
		v4dadd(	v4dmul(a, vec4d(f, f, f, f)),
			v4dmul(b, vec4d(g, g, g, g))),
		v4dadd(	v4dmul(c, vec4d(h, h, h, h)),
			v4dmul(d, vec4d(i, i, i, i)))
		));
}

static quatd qdscaleadd4(quatdu a, double f, quatdu b, double g, quatdu c, double h, quatdu d, double i)
{
	return(v4dadd(
		v4dadd(	v4dmul(a, vec4d(f, f, f, f)),
			v4dmul(b, vec4d(g, g, g, g))),
		v4dadd(	v4dmul(c, vec4d(h, h, h, h)),
			v4dmul(d, vec4d(i, i, i, i)))
		));
}
