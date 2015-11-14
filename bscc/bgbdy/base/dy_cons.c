#include <bgbgc.h>
#include <bgbdy.h>

#if defined(BGBGC_USECONS64) && !defined(PTRBITS_64)

BGBDY_API dyt dycons(dyt a, dyt b)
{
	dytf *p;

#ifdef BGBGC_USECONS
	p=BGBGC_AllocCons();
#else
	p=(dytf *)BGBDY_New("_cons_t", 2*sizeof(dytf));
#endif

	p[0]=dytfWrap(a);
	p[1]=dytfWrap(b);
	return((dyt)p);
}

BGBDY_API int dyconsp(dyt p)
{
#ifdef BGBGC_USECONS
	return(BGBGC_ConsP(p));
#else
	return(dytypep(p, "_cons_t"));
#endif
}

BGBDY_API dyt dycar(dyt p)
{
	dytf *q;
	if(!p || (p==UNDEFINED))
		return(UNDEFINED);
	q=(dytf *)p;
	return(dytfUnwrap(q[0]));
}

BGBDY_API dyt dycdr(dyt p)
{
	dytf *q;
	if(!p || (p==UNDEFINED))
		return(UNDEFINED);
	q=(dytf *)p;
	return(dytfUnwrap(q[1]));
}

BGBDY_API dyt dycars(dyt p)
{
	dytf *q;
	if(!BGBGC_ConsP(p))return(UNDEFINED);
	q=(dytf *)p;
	return(dytfUnwrap(q[0]));
}

BGBDY_API dyt dycdrs(dyt p)
{
	dytf *q;
	if(!BGBGC_ConsP(p))return(UNDEFINED);
	q=(dytf *)p;
	return(dytfUnwrap(q[1]));
}

BGBDY_API void dysetcar(dyt p, dyt v)
{
	dytf *q;
	q=(dytf *)p;
//	gcSet(q[0], v);
	q[0]=dytfWrap(v);
}

BGBDY_API void dysetcdr(dyt p, dyt v)
{
	dytf *q;
	q=(dytf *)p;
//	gcSet(q[1], v);
	q[1]=dytfWrap(v);
}

BGBDY_API dyt BGBDY_CarF(dyt p)
{
	dytf *q;
	q=(dytf *)p;
	return(dytfUnwrap(q[0]));
}

BGBDY_API dyt BGBDY_CdrF(dyt p)
{
	dytf *q;
	q=(dytf *)p;
	return(dytfUnwrap(q[1]));
}

BGBDY_API void BGBDY_SetCarF(dyt p, dyt v)
{
	dytf *q;
	q=(dytf *)p;
	q[0]=dytfWrap(v);
//	gcSet(q[0], v);
}

BGBDY_API void BGBDY_SetCdrF(dyt p, dyt v)
{
	dytf *q;
	q=(dytf *)p;
	q[1]=dytfWrap(v);
//	gcSet(q[1], v);
}

#else

BGBDY_API dyt dycons(dyt a, dyt b)
{
	dyt *p;

#ifdef BGBGC_USECONS
	p=BGBGC_AllocCons();
#else
	p=(dyt *)BGBDY_New("_cons_t", 2*sizeof(void *));
#endif

	gcSet(p[0], a);
	gcSet(p[1], b);
	return((dyt)p);
}

BGBDY_API int dyconsp(dyt p)
{
#ifdef BGBGC_USECONS
	return(BGBGC_ConsP(p));
#else
	return(dytypep(p, "_cons_t"));
#endif
}

BGBDY_API dyt dycar(dyt p)
{
	dyt *q;
	if(!p || (p==UNDEFINED))
		return(UNDEFINED);
	q=(dyt *)p;
	return(q[0]);
}

BGBDY_API dyt dycdr(dyt p)
{
	dyt *q;
	if(!p || (p==UNDEFINED))
		return(UNDEFINED);
	q=(dyt *)p;
	return(q[1]);
}

BGBDY_API dyt dycars(dyt p)
{
	dyt *q;
	if(!BGBGC_ConsP(p))return(UNDEFINED);
	q=(dyt *)p;
	return(q[0]);
}

BGBDY_API dyt dycdrs(dyt p)
{
	dyt *q;
	if(!BGBGC_ConsP(p))return(UNDEFINED);
	q=(dyt *)p;
	return(q[1]);
}

BGBDY_API void dysetcar(dyt p, dyt v)
{
	dyt *q;
	q=(dyt *)p;
	gcSet(q[0], v);
}

BGBDY_API void dysetcdr(dyt p, dyt v)
{
	dyt *q;
	q=(dyt *)p;
	gcSet(q[1], v);
}

BGBDY_API dyt BGBDY_CarF(dyt p)
{
	dyt *q;
	q=(dyt *)p;
	return(q[0]);
}

BGBDY_API dyt BGBDY_CdrF(dyt p)
{
	dyt *q;
	q=(dyt *)p;
	return(q[1]);
}

BGBDY_API void BGBDY_SetCarF(dyt p, dyt v)
{
	dyt *q;
	q=(dyt *)p;
	gcSet(q[0], v);
}

BGBDY_API void BGBDY_SetCdrF(dyt p, dyt v)
{
	dyt *q;
	q=(dyt *)p;
	gcSet(q[1], v);
}

#endif


BGBDY_API int dylistp(dyt lst)
{
	dyt cur;

	cur=lst;
	while(dyconsp(cur))
		cur=dycdr(cur);
	return(cur==NULL);
}

BGBDY_API int dylistlen(dyt lst)
{
	dyt cur;
	int i;

	cur=lst; i=0;
	while(dyconsp(cur))
		 { i++; cur=dycdr(cur); }
	return(i);
}

BGBDY_API int dyListLength(dyt lst)
{
	dyt cur;
	int i;

	cur=lst; i=0;
	while(dyconsp(cur))
		 { i++; cur=dycdr(cur); }
	return(i);
}

BGBDY_API void dyFreeCons(dyt p)
{
	BGBGC_FreeCons(p);
}

BGBDY_API void dyFreeList(dyt p)
{
	dyt c, n;

	c=p;
	while(dyconsp(c))
	{
		n=dycdr(c);
		BGBGC_FreeCons(c);
		c=n;
	}
}

BGBDY_API void dyFreeListF(dyt p)
{
	dyt c, n;

	c=p;
	while(c)
	{
//		n=((dyt *)c)[1];
		n=dycdr(c);
		BGBGC_FreeCons(c);
		c=n;
	}
}


BGBDY_API void dyFreeListR(dyt p)
{
	static int rcnt=0;
	dyt c, n, t;

	if(rcnt>=256)
		{ *(int *)-1=-1; }
	rcnt++;

	c=p;
	while(dyconsp(c))
	{
		t=dycar(c);
		n=dycdr(c);
		BGBGC_FreeCons(c);
		dyFreeListR(t);
		c=n;
	}
	
	rcnt--;
}

BGBDY_API int dyFormIsP(dyt lst, char *str)
{
	if(!dyconsp(lst))return(0);
//	if(dycar(lst)==dysymbol(str))
	if(dycar(lst)==dyFixedSymbol(str))
		return(1);
	return(0);
}

BGBDY_API dyt dylist1(dyt a)
	{ return(dycons(a, NULL)); }
BGBDY_API dyt dylist2(dyt a, dyt b)
	{ return(dycons(a, dylist1(b))); }
BGBDY_API dyt dylist3(dyt a, dyt b, dyt c)
	{ return(dycons(a, dylist2(b, c))); }
BGBDY_API dyt dylist4(dyt a, dyt b, dyt c, dyt d)
	{ return(dycons(a, dylist3(b, c, d))); }
BGBDY_API dyt dylist5(dyt a, dyt b, dyt c, dyt d, dyt e)
	{ return(dycons(a, dylist4(b, c, d, e))); }
BGBDY_API dyt dylist6(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dycons(a, dylist5(b, c, d, e, f))); }
BGBDY_API dyt dylist7(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g)
	{ return(dycons(a, dylist6(b, c, d, e, f, g))); }
BGBDY_API dyt dylist8(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h)
	{ return(dycons(a, dylist7(b, c, d, e, f, g, h))); }
BGBDY_API dyt dylist9(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h, dyt i)
	{ return(dycons(a, dylist8(b, c, d, e, f, g, h, i))); }
dyt dylist10(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h, dyt i, dyt j)
	{ return(dycons(a, dylist9(b, c, d, e, f, g, h, i, j))); }

BGBDY_API dyt dylist1s(char *a)
	{ return(dylist1(dysymbol(a))); }
BGBDY_API dyt dylist2s(char *a, dyt b)
	{ return(dylist2(dysymbol(a), b)); }
BGBDY_API dyt dylist3s(char *a, dyt b, dyt c)
	{ return(dylist3(dysymbol(a), b, c)); }
BGBDY_API dyt dylist4s(char *a, dyt b, dyt c, dyt d)
	{ return(dylist4(dysymbol(a), b, c, d)); }
BGBDY_API dyt dylist5s(char *a, dyt b, dyt c, dyt d, dyt e)
	{ return(dylist5(dysymbol(a), b, c, d, e)); }
BGBDY_API dyt dylist6s(char *a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dylist6(dysymbol(a), b, c, d, e, f)); }
BGBDY_API dyt dylist7s(char *a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g)
	{ return(dylist7(dysymbol(a), b, c, d, e, f, g)); }
BGBDY_API dyt dylist8s(char *a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h)
	{ return(dylist8(dysymbol(a), b, c, d, e, f, g, h)); }


BGBDY_API dyt dycons2(dyt a, dyt b, dyt c)
	{ return(dycons(a, dycons(b, c))); }
BGBDY_API dyt dycons3(dyt a, dyt b, dyt c, dyt d)
	{ return(dycons(a, dycons2(b, c, d))); }
BGBDY_API dyt dycons4(dyt a, dyt b, dyt c, dyt d, dyt e)
	{ return(dycons(a, dycons3(b, c, d, e))); }
BGBDY_API dyt dycons5(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dycons(a, dycons4(b, c, d, e, f))); }
BGBDY_API dyt dycons6(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g)
	{ return(dycons(a, dycons5(b, c, d, e, f, g))); }
BGBDY_API dyt dycons7(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h)
	{ return(dycons(a, dycons6(b, c, d, e, f, g, h))); }
BGBDY_API dyt dycons8(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h, dyt i)
	{ return(dycons(a, dycons7(b, c, d, e, f, g, h, i))); }
BGBDY_API dyt dycons9(dyt a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h, dyt i, dyt j)
	{ return(dycons(a, dycons8(b, c, d, e, f, g, h, i, j))); }

BGBDY_API dyt dyconss(char *a, dyt b)
	{ return(dycons(dysymbol(a), b)); }
BGBDY_API dyt dycons2s(char *a, dyt b, dyt c)
	{ return(dycons2(dysymbol(a), b, c)); }
BGBDY_API dyt dycons3s(char *a, dyt b, dyt c, dyt d)
	{ return(dycons3(dysymbol(a), b, c, d)); }
BGBDY_API dyt dycons4s(char *a, dyt b, dyt c, dyt d, dyt e)
	{ return(dycons4(dysymbol(a), b, c, d, e)); }
BGBDY_API dyt dycons5s(char *a, dyt b, dyt c, dyt d, dyt e, dyt f)
	{ return(dycons5(dysymbol(a), b, c, d, e, f)); }
BGBDY_API dyt dycons6s(char *a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g)
	{ return(dycons6(dysymbol(a), b, c, d, e, f, g)); }
BGBDY_API dyt dycons7s(char *a, dyt b, dyt c, dyt d, dyt e, dyt f, dyt g, dyt h)
	{ return(dycons7(dysymbol(a), b, c, d, e, f, g, h)); }


BGBDY_API dyt dycaar(dyt p) { return(dycar(dycar(p))); }
BGBDY_API dyt dycdar(dyt p) { return(dycdr(dycar(p))); }
BGBDY_API dyt dycadr(dyt p) { return(dycar(dycdr(p))); }
BGBDY_API dyt dycddr(dyt p) { return(dycdr(dycdr(p))); }
BGBDY_API dyt dycaaar(dyt p) { return(dycar(dycaar(p))); }
BGBDY_API dyt dycdaar(dyt p) { return(dycdr(dycaar(p))); }
BGBDY_API dyt dycadar(dyt p) { return(dycar(dycdar(p))); }
BGBDY_API dyt dycddar(dyt p) { return(dycdr(dycdar(p))); }
BGBDY_API dyt dycaadr(dyt p) { return(dycar(dycadr(p))); }
BGBDY_API dyt dycdadr(dyt p) { return(dycdr(dycadr(p))); }
BGBDY_API dyt dycaddr(dyt p) { return(dycar(dycddr(p))); }
BGBDY_API dyt dycdddr(dyt p) { return(dycdr(dycddr(p))); }
BGBDY_API dyt dycaaaar(dyt p) { return(dycar(dycaaar(p))); }
BGBDY_API dyt dycdaaar(dyt p) { return(dycdr(dycaaar(p))); }
BGBDY_API dyt dycadaar(dyt p) { return(dycar(dycdaar(p))); }
BGBDY_API dyt dycddaar(dyt p) { return(dycdr(dycdaar(p))); }
BGBDY_API dyt dycaadar(dyt p) { return(dycar(dycadar(p))); }
BGBDY_API dyt dycdadar(dyt p) { return(dycdr(dycadar(p))); }
BGBDY_API dyt dycaddar(dyt p) { return(dycar(dycddar(p))); }
BGBDY_API dyt dycdddar(dyt p) { return(dycdr(dycddar(p))); }
BGBDY_API dyt dycaaadr(dyt p) { return(dycar(dycaadr(p))); }
BGBDY_API dyt dycdaadr(dyt p) { return(dycdr(dycaadr(p))); }
BGBDY_API dyt dycadadr(dyt p) { return(dycar(dycdadr(p))); }
BGBDY_API dyt dycddadr(dyt p) { return(dycdr(dycdadr(p))); }
BGBDY_API dyt dycaaddr(dyt p) { return(dycar(dycaddr(p))); }
BGBDY_API dyt dycdaddr(dyt p) { return(dycdr(dycaddr(p))); }
BGBDY_API dyt dycadddr(dyt p) { return(dycar(dycdddr(p))); }
BGBDY_API dyt dycddddr(dyt p) { return(dycdr(dycdddr(p))); }

BGBDY_API dyt dycaddddr(dyt p) { return(dycar(dycddddr(p))); }
BGBDY_API dyt dycdddddr(dyt p) { return(dycdr(dycddddr(p))); }
BGBDY_API dyt dycadddddr(dyt p) { return(dycar(dycdddddr(p))); }
BGBDY_API dyt dycddddddr(dyt p) { return(dycdr(dycdddddr(p))); }
BGBDY_API dyt dycaddddddr(dyt p) { return(dycar(dycddddddr(p))); }
BGBDY_API dyt dycdddddddr(dyt p) { return(dycdr(dycddddddr(p))); }
BGBDY_API dyt dycadddddddr(dyt p) { return(dycar(dycdddddddr(p))); }
BGBDY_API dyt dycddddddddr(dyt p) { return(dycdr(dycdddddddr(p))); }

BGBDY_API dyt dycaars(dyt p) { return(dycars(dycars(p))); }
BGBDY_API dyt dycdars(dyt p) { return(dycdrs(dycars(p))); }
BGBDY_API dyt dycadrs(dyt p) { return(dycars(dycdrs(p))); }
BGBDY_API dyt dycddrs(dyt p) { return(dycdrs(dycdrs(p))); }
BGBDY_API dyt dycaaars(dyt p) { return(dycars(dycaars(p))); }
BGBDY_API dyt dycdaars(dyt p) { return(dycdrs(dycaars(p))); }
BGBDY_API dyt dycadars(dyt p) { return(dycars(dycdars(p))); }
BGBDY_API dyt dycddars(dyt p) { return(dycdrs(dycdars(p))); }
BGBDY_API dyt dycaadrs(dyt p) { return(dycars(dycadrs(p))); }
BGBDY_API dyt dycdadrs(dyt p) { return(dycdrs(dycadrs(p))); }
BGBDY_API dyt dycaddrs(dyt p) { return(dycars(dycddrs(p))); }
BGBDY_API dyt dycdddrs(dyt p) { return(dycdrs(dycddrs(p))); }

BGBDY_API dyt dycaaaars(dyt p) { return(dycars(dycaaars(p))); }
BGBDY_API dyt dycdaaars(dyt p) { return(dycdrs(dycaaars(p))); }
BGBDY_API dyt dycadaars(dyt p) { return(dycars(dycdaars(p))); }
BGBDY_API dyt dycddaars(dyt p) { return(dycdrs(dycdaars(p))); }
BGBDY_API dyt dycaadars(dyt p) { return(dycars(dycadars(p))); }
BGBDY_API dyt dycdadars(dyt p) { return(dycdrs(dycadars(p))); }
BGBDY_API dyt dycaddars(dyt p) { return(dycars(dycddars(p))); }
BGBDY_API dyt dycdddars(dyt p) { return(dycdrs(dycddars(p))); }
BGBDY_API dyt dycaaadrs(dyt p) { return(dycars(dycaadrs(p))); }
BGBDY_API dyt dycdaadrs(dyt p) { return(dycdrs(dycaadrs(p))); }
BGBDY_API dyt dycadadrs(dyt p) { return(dycars(dycdadrs(p))); }
BGBDY_API dyt dycddadrs(dyt p) { return(dycdrs(dycdadrs(p))); }
BGBDY_API dyt dycaaddrs(dyt p) { return(dycars(dycaddrs(p))); }
BGBDY_API dyt dycdaddrs(dyt p) { return(dycdrs(dycaddrs(p))); }
BGBDY_API dyt dycadddrs(dyt p) { return(dycars(dycdddrs(p))); }
BGBDY_API dyt dycddddrs(dyt p) { return(dycdrs(dycdddrs(p))); }

BGBDY_API dyt dycaaaaars(dyt p) { return(dycars(dycaaaars(p))); }
BGBDY_API dyt dycdaaaars(dyt p) { return(dycdrs(dycaaaars(p))); }
BGBDY_API dyt dycadaaars(dyt p) { return(dycars(dycdaaars(p))); }
BGBDY_API dyt dycddaaars(dyt p) { return(dycdrs(dycdaaars(p))); }
BGBDY_API dyt dycaadaars(dyt p) { return(dycars(dycadaars(p))); }
BGBDY_API dyt dycdadaars(dyt p) { return(dycdrs(dycadaars(p))); }
BGBDY_API dyt dycaddaars(dyt p) { return(dycars(dycddaars(p))); }
BGBDY_API dyt dycdddaars(dyt p) { return(dycdrs(dycddaars(p))); }
BGBDY_API dyt dycaaadars(dyt p) { return(dycars(dycaadars(p))); }
BGBDY_API dyt dycdaadars(dyt p) { return(dycdrs(dycaadars(p))); }
BGBDY_API dyt dycadadars(dyt p) { return(dycars(dycdadars(p))); }
BGBDY_API dyt dycddadars(dyt p) { return(dycdrs(dycdadars(p))); }
BGBDY_API dyt dycaaddars(dyt p) { return(dycars(dycaddars(p))); }
BGBDY_API dyt dycdaddars(dyt p) { return(dycdrs(dycaddars(p))); }
BGBDY_API dyt dycadddars(dyt p) { return(dycars(dycdddars(p))); }
BGBDY_API dyt dycddddars(dyt p) { return(dycdrs(dycdddars(p))); }
BGBDY_API dyt dycaaaadrs(dyt p) { return(dycars(dycaaadrs(p))); }
BGBDY_API dyt dycdaaadrs(dyt p) { return(dycdrs(dycaaadrs(p))); }
BGBDY_API dyt dycadaadrs(dyt p) { return(dycars(dycdaadrs(p))); }
BGBDY_API dyt dycddaadrs(dyt p) { return(dycdrs(dycdaadrs(p))); }
BGBDY_API dyt dycaadadrs(dyt p) { return(dycars(dycadadrs(p))); }
BGBDY_API dyt dycdadadrs(dyt p) { return(dycdrs(dycadadrs(p))); }
BGBDY_API dyt dycaddadrs(dyt p) { return(dycars(dycddadrs(p))); }
BGBDY_API dyt dycdddadrs(dyt p) { return(dycdrs(dycddadrs(p))); }
BGBDY_API dyt dycaaaddrs(dyt p) { return(dycars(dycaaddrs(p))); }
BGBDY_API dyt dycdaaddrs(dyt p) { return(dycdrs(dycaaddrs(p))); }
BGBDY_API dyt dycadaddrs(dyt p) { return(dycars(dycdaddrs(p))); }
BGBDY_API dyt dycddaddrs(dyt p) { return(dycdrs(dycdaddrs(p))); }
BGBDY_API dyt dycaadddrs(dyt p) { return(dycars(dycadddrs(p))); }
BGBDY_API dyt dycdadddrs(dyt p) { return(dycdrs(dycadddrs(p))); }
BGBDY_API dyt dycaddddrs(dyt p) { return(dycars(dycddddrs(p))); }
BGBDY_API dyt dycdddddrs(dyt p) { return(dycdrs(dycddddrs(p))); }

BGBDY_API dyt dycadddddrs(dyt p) { return(dycars(dycdddddrs(p))); }
BGBDY_API dyt dycddddddrs(dyt p) { return(dycdrs(dycdddddrs(p))); }
BGBDY_API dyt dycaddddddrs(dyt p) { return(dycars(dycddddddrs(p))); }
BGBDY_API dyt dycdddddddrs(dyt p) { return(dycdrs(dycddddddrs(p))); }
BGBDY_API dyt dycadddddddrs(dyt p) { return(dycars(dycdddddddrs(p))); }
BGBDY_API dyt dycddddddddrs(dyt p) { return(dycdrs(dycdddddddrs(p))); }

BGBDY_API dyt dyreverse(dyt lst)
{
	dyt c, l, n;

	c=lst; l=NULL;
	while(dyconsp(c))
	{
		l=dycons(dycar(c), l);
		c=dycdr(c);
	}
	return(l);
}

BGBDY_API dyt dynreverse(dyt lst)
{
	dyt c, l, n;

	c=lst; l=NULL;
	while(dyconsp(c))
	{
		n=dycdr(c);
		dysetcdr(c, l);
		l=c; c=n;
	}
	return(l);
}

BGBDY_API dyt dynappend(dyt la, dyt lb)
{
	dyt c, l, n;

	if(!la)return(lb);
	if(!dyconsp(la))
		return(lb);

	c=la; l=NULL;
	while(dyconsp(c))
		{ l=c; c=dycdr(c); }
	if(!l)return(lb);
	dysetcdr(l, lb);
	return(la);
}

BGBDY_API dyt dyListToArray(dyt lst)
{
	dyt c, l;
	int i;

	c=lst; i=0;
	while(dyconsp(c)) { i++; c=dycdr(c); }

	l=dyarray(i);

	c=lst; i=0;
	while(dyconsp(c))
	{
		dyarraysetidx(l, i, dycar(c));
		i++;
		c=dycdr(c);
	}

	return(l);
}

BGBDY_API dyt dylistcopy(dyt lst)
{
	dyt cur, tlst;

	cur=lst; tlst=NULL;
	while(dyconsp(cur))
	{
		gcSet(tlst, dycons(dycar(cur), tlst));
		cur=dycdr(cur);
	}

	gcSet(tlst, dynreverse(tlst));
	return(tlst);
}

BGBDY_API dyt dyListCopyR(dyt lst)
{
	dyt cur, tlst;

	if(!dyconsp(lst))
		return(lst);

	cur=lst; tlst=NULL;
	while(dyconsp(cur))
	{
		gcSet(tlst, dycons(dyListCopyR(dycar(cur)), tlst));
		cur=dycdr(cur);
	}

	gcSet(tlst, dynreverse(tlst));
	return(tlst);
}

BGBDY_API dyt dyappend(dyt la, dyt lb)
{
	dyt c, l, n;

	if(!la)return(dylistcopy(lb));
	if(!lb)return(dylistcopy(la));
	return(dynappend(dylistcopy(la), dylistcopy(lb)));
}

BGBDY_API int dyListEqualP(dyt la, dyt lb)
{
	dyt ca, cb;

	if(la==lb)return(1);
	if(!dyconsp(la) || !dyconsp(lb))
		return(0);

	ca=la; cb=lb;
	while(dyconsp(ca) && dyconsp(cb))
	{
		if(!dyListEqualP(dycar(ca), dycar(cb)))
			return(0);
		ca=dycdr(ca); cb=dycdr(cb);
	}
	if(ca!=cb)
		return(0);
	return(1);
}

BGBDY_API int dyListEqvP(dyt la, dyt lb)
{
	dyt ca, cb;

	if(la==lb)return(1);
	if(!dyconsp(la) || !dyconsp(lb))
		return(dyeqvp(la, lb));

	ca=la; cb=lb;
	while(dyconsp(ca) && dyconsp(cb))
	{
		if(!dyeqvp(dycar(ca), dycar(cb)))
			return(0);
		ca=dycdr(ca); cb=dycdr(cb);
	}
	return(dyeqvp(ca, cb));
}
