#include <bgbdy.h>

void DYLL_CopyValueBufI(dycValue *v, void *p)
    { *(s32 *)p=v->i; }
void DYLL_CopyValueBufL(dycValue *v, void *p)
    { *(s64 *)p=v->l; }
void DYLL_CopyValueBufF(dycValue *v, void *p)
    { *(f32 *)p=v->f; }
void DYLL_CopyValueBufD(dycValue *v, void *p)
    { *(f64 *)p=v->d; }
void DYLL_CopyValueBufP(dycValue *v, void *p)
    { *(void **)p=v->pv; }

void DYLL_CopyBufValueI(void *p, dycValue *v)
    { v->i=*(s32 *)p; }
void DYLL_CopyBufValueL(void *p, dycValue *v)
    { v->l=*(s64 *)p; }
void DYLL_CopyBufValueF(void *p, dycValue *v)
    { v->f=*(f32 *)p; }
void DYLL_CopyBufValueD(void *p, dycValue *v)
    { v->d=*(f64 *)p; }
void DYLL_CopyBufValueP(void *p, dycValue *v)
    { v->pv=*(void **)p; }

void DYLL_CopyValueBufV2F(dycValue *v, void *p)
    { *(vec2 *)p=*(vec2 *)(v->pv); }
void DYLL_CopyValueBufV3F(dycValue *v, void *p)
    { *(vec3 *)p=*(vec3 *)(v->pv); }
void DYLL_CopyValueBufV4F(dycValue *v, void *p)
    { *(vec4 *)p=*(vec4 *)(v->pv); }
void DYLL_CopyValueBufV2D(dycValue *v, void *p)
    { *(vec2d *)p=*(vec2d *)(v->pv); }
void DYLL_CopyValueBufV3D(dycValue *v, void *p)
    { *(vec3d *)p=*(vec3d *)(v->pv); }
void DYLL_CopyValueBufV4D(dycValue *v, void *p)
    { *(vec4d *)p=*(vec4d *)(v->pv); }

void DYLL_CopyBufValueV2F(void *p, dycValue *v)
    { *(vec2 *)(v->pv)=*(vec2 *)p; }
void DYLL_CopyBufValueV3F(void *p, dycValue *v)
    { *(vec3 *)(v->pv)=*(vec3 *)p; }
void DYLL_CopyBufValueV4F(void *p, dycValue *v)
    { *(vec4 *)(v->pv)=*(vec4 *)p; }
void DYLL_CopyBufValueV2D(void *p, dycValue *v)
    { *(vec2d *)(v->pv)=*(vec2d *)p; }
void DYLL_CopyBufValueV3D(void *p, dycValue *v)
    { *(vec3d *)(v->pv)=*(vec3d *)p; }
void DYLL_CopyBufValueV4D(void *p, dycValue *v)
    { *(vec4d *)(v->pv)=*(vec4d *)p; }

void DYLL_CopyValuesBuf0(DYLL_ArgList *ctx, dycValue *args, byte *buf)
{ }

void DYLL_CopyValuesBuf1(DYLL_ArgList *ctx, dycValue *args, byte *buf)
{
	ctx->args[0].CopyValueBuf(args+0, buf+ctx->args[0].offs);
}

void DYLL_CopyValuesBuf2(DYLL_ArgList *ctx, dycValue *args, byte *buf)
{
	ctx->args[0].CopyValueBuf(args+0, buf+ctx->args[0].offs);
	ctx->args[1].CopyValueBuf(args+1, buf+ctx->args[1].offs);
}

void DYLL_CopyValuesBuf3(DYLL_ArgList *ctx, dycValue *args, byte *buf)
{
	ctx->args[0].CopyValueBuf(args+0, buf+ctx->args[0].offs);
	ctx->args[1].CopyValueBuf(args+1, buf+ctx->args[1].offs);
	ctx->args[2].CopyValueBuf(args+2, buf+ctx->args[2].offs);
}

void DYLL_CopyValuesBuf4(DYLL_ArgList *ctx, dycValue *args, byte *buf)
{
	ctx->args[0].CopyValueBuf(args+0, buf+ctx->args[0].offs);
	ctx->args[1].CopyValueBuf(args+1, buf+ctx->args[1].offs);
	ctx->args[2].CopyValueBuf(args+2, buf+ctx->args[2].offs);
	ctx->args[3].CopyValueBuf(args+3, buf+ctx->args[3].offs);
}

void DYLL_CopyValuesBufN(DYLL_ArgList *ctx, dycValue *args, byte *buf)
{
	int i;
	for(i=0; i<ctx->nargs; i++)
		ctx->args[i].CopyValueBuf(args+i, buf+ctx->args[i].offs);
}

void DYLL_CopyBufValues0(DYLL_ArgList *ctx, byte *buf, dycValue *args)
{
}

void DYLL_CopyBufValues1(DYLL_ArgList *ctx, byte *buf, dycValue *args)
{
	ctx->args[0].CopyBufValue(buf+ctx->args[0].offs, args+0);
}

void DYLL_CopyBufValues2(DYLL_ArgList *ctx, byte *buf, dycValue *args)
{
	ctx->args[0].CopyBufValue(buf+ctx->args[0].offs, args+0);
	ctx->args[1].CopyBufValue(buf+ctx->args[1].offs, args+1);
}

void DYLL_CopyBufValues3(DYLL_ArgList *ctx, byte *buf, dycValue *args)
{
	ctx->args[0].CopyBufValue(buf+ctx->args[0].offs, args+0);
	ctx->args[1].CopyBufValue(buf+ctx->args[1].offs, args+1);
	ctx->args[2].CopyBufValue(buf+ctx->args[2].offs, args+2);
}

void DYLL_CopyBufValues4(DYLL_ArgList *ctx, byte *buf, dycValue *args)
{
	ctx->args[0].CopyBufValue(buf+ctx->args[0].offs, args+0);
	ctx->args[1].CopyBufValue(buf+ctx->args[1].offs, args+1);
	ctx->args[2].CopyBufValue(buf+ctx->args[2].offs, args+2);
	ctx->args[3].CopyBufValue(buf+ctx->args[3].offs, args+3);
}

void DYLL_CopyBufValuesN(DYLL_ArgList *ctx, byte *buf, dycValue *args)
{
	int i;
	for(i=0; i<ctx->nargs; i++)
		ctx->args[i].CopyBufValue(buf+ctx->args[i].offs, args+i);
}

BGBDY_API void DYLL_CopyValuesBuf(DYLL_ArgList *ctx,
	dycValue *args, byte *buf)
{
	ctx->CopyValuesBuf(ctx, args, buf);
}

BGBDY_API void DYLL_CopyBufValues(DYLL_ArgList *ctx,
	byte *buf, dycValue *args)
{
	ctx->CopyBufValues(ctx, buf, args);
}


int DYLL_CompileSigArgList_Item(DYLL_ArgList *ctx,
	DYLL_ValueTransfer *item, char **rsig, int *rsz)
{
	char *s;
	int tb, ret;
	int i, j;
	
	s=*rsig;
	tb=*rsz;
	ret=0;

	switch(*s++)
	{
	case 'a': case 'b': case 'c': case 'h':
	case 's': case 't': case 'w':
	case 'i': case 'j':
		item->CopyValueBuf=DYLL_CopyValueBufI;
		item->CopyBufValue=DYLL_CopyBufValueI;
//		*(int *)tb=va_arg(lst, int);
		tb+=sizeof(nlint); break;
	case 'l': case 'm':
		if(sizeof(long)==sizeof(s32))
		{
			item->CopyValueBuf=DYLL_CopyValueBufI;
			item->CopyBufValue=DYLL_CopyBufValueI;
		}else
		{
			item->CopyValueBuf=DYLL_CopyValueBufL;
			item->CopyBufValue=DYLL_CopyBufValueL;
		}

//		*(long *)tb=va_arg(lst, long);
		tb+=sizeof(nlint); break;
	case 'f':
		item->CopyValueBuf=DYLL_CopyValueBufF;
		item->CopyBufValue=DYLL_CopyBufValueF;
//		*(float *)tb=va_arg(lst, double);
		tb+=sizeof(nlint); break;
	case 'd':
		item->CopyValueBuf=DYLL_CopyValueBufD;
		item->CopyBufValue=DYLL_CopyBufValueD;
//		*(double *)tb=va_arg(lst, double);
		tb+=sizeof(s64); break;
//	case 'e':
//		*(long double *)tb=va_arg(lst, long double);
//		tb+=12; break;
//	case 'g':
//		arr[i++]=dydouble(va_arg(lst, long double));
//		break;
	case 'k':
		item->CopyValueBuf=DYLL_CopyValueBufF;
		item->CopyBufValue=DYLL_CopyBufValueF;
//		*(float *)tb=va_arg(lst, double);
		tb+=sizeof(nlint); break;

	case 'r':
		item->CopyValueBuf=DYLL_CopyValueBufP;
		item->CopyBufValue=DYLL_CopyBufValueP;
//		*(void **)tb=va_arg(lst, void *);
		tb+=sizeof(nlint); break;

	case 'x': case 'y':
		item->CopyValueBuf=DYLL_CopyValueBufL;
		item->CopyBufValue=DYLL_CopyBufValueL;
//		*(long long *)tb=va_arg(lst, long long);
		tb+=sizeof(s64); break;

	case 'C':
		j=*s++;
		switch(j)
		{
		case 'a':
			item->CopyValueBuf=DYLL_CopyValueBufV2F;
			item->CopyBufValue=DYLL_CopyBufValueV2F;
//			*(vec2 *)tb=va_arg(lst, vec2);
			tb+=sizeof(vec2); break;
		case 'b':
			item->CopyValueBuf=DYLL_CopyValueBufV3F;
			item->CopyBufValue=DYLL_CopyBufValueV3F;
//			*(vec3 *)tb=va_arg(lst, vec3);
			tb+=sizeof(vec3); break;
		case 'c':
			item->CopyValueBuf=DYLL_CopyValueBufV4F;
			item->CopyBufValue=DYLL_CopyBufValueV4F;
//			*(vec4 *)tb=va_arg(lst, vec4);
			tb+=sizeof(vec4); break;
		case 'q':
			item->CopyValueBuf=DYLL_CopyValueBufV4F;
			item->CopyBufValue=DYLL_CopyBufValueV4F;
//			*(quat *)tb=va_arg(lst, quat);
			tb+=sizeof(quat); break;

		case 'f':
			item->CopyValueBuf=DYLL_CopyValueBufV2F;
			item->CopyBufValue=DYLL_CopyBufValueV2F;
//			*(fcomplex *)tb=va_arg(lst, fcomplex);
			tb+=sizeof(fcomplex); break;
		case 'd':
			item->CopyValueBuf=DYLL_CopyValueBufV2D;
			item->CopyBufValue=DYLL_CopyBufValueV2D;
//			*(dcomplex *)tb=va_arg(lst, dcomplex);
			tb+=sizeof(dcomplex); break;
//		case 'e': i+=32; break;
//		case 'g': i+=32; break;
//		case 'k': break;

		case 'r':
			item->CopyValueBuf=DYLL_CopyValueBufL;
			item->CopyBufValue=DYLL_CopyBufValueL;
//			*(dytf *)tb=va_arg(lst, dytf);
			tb+=sizeof(dytf); break;

		case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9':
//			*(void **)tb=va_arg(lst, void *);
			item->CopyValueBuf=DYLL_CopyValueBufP;
			item->CopyBufValue=DYLL_CopyBufValueP;
			tb+=sizeof(nlint); s=dyllSigNext(s); break;
		default:
			ret=-1; break;
		}
		break;

	case 'D':
		j=*s++;
		switch(j)
		{
		case 'a':
			item->CopyValueBuf=DYLL_CopyValueBufV2D;
			item->CopyBufValue=DYLL_CopyBufValueV2D;
//			*(vec2d *)tb=va_arg(lst, vec2d);
			tb+=sizeof(vec2d); break;
		case 'b':
			item->CopyValueBuf=DYLL_CopyValueBufV3D;
			item->CopyBufValue=DYLL_CopyBufValueV3D;
//			*(vec3d *)tb=va_arg(lst, vec3d);
			tb+=sizeof(vec3d); break;
		case 'c':
			item->CopyValueBuf=DYLL_CopyValueBufV4D;
			item->CopyBufValue=DYLL_CopyBufValueV4D;
//			*(vec4d *)tb=va_arg(lst, vec4d);
			tb+=sizeof(vec4d); break;
		case 'q':
			item->CopyValueBuf=DYLL_CopyValueBufV4D;
			item->CopyBufValue=DYLL_CopyBufValueV4D;
//			*(quatd *)tb=va_arg(lst, quatd);
			tb+=sizeof(quatd); break;
		default:
			ret=-1; break;
		}
		break;

	case 'P':
//		*(void **)tb=va_arg(lst, void *);
		item->CopyValueBuf=DYLL_CopyValueBufP;
		item->CopyBufValue=DYLL_CopyBufValueP;
		tb+=sizeof(nlint);

		while(*s=='P')s++;
		if((*s>='a') && (*s<='z'))
			{ s++; break; }
		if(*s=='X')
		{
			while(*s && *s!=';')s++;
			if(*s==';')s++;
		}
		break;

	case 'L':
//		*(void **)tb=va_arg(lst, void *);
		item->CopyValueBuf=DYLL_CopyValueBufP;
		item->CopyBufValue=DYLL_CopyBufValueP;
		tb+=sizeof(nlint);

		while(*s && *s!=';')s++;
		if(*s==';')s++;
		break;

	default:
		ret=-1; break;
	}
		
	*rsig=s;
	*rsz=tb;
	return(ret);
}

int DYLL_CompileSigArgListI(DYLL_ArgList *ctx, char *sig)
{
	char *s;
	int i, j, sz;

	s=sig; i=0; sz=0;

	if(*s=='(')
	{
		s++;
		if(*s==')')
		{
			s++;
			ctx->esig=s;
			ctx->ret=ctx->args+0;
			j=DYLL_CompileSigArgList_Item(ctx, ctx->ret, &s, &sz);
			if(j<0)return(j);
			return(0);
		}
	}

	while(*s && (*s!=')'))
	{
		j=DYLL_CompileSigArgList_Item(ctx, ctx->args+i, &s, &sz);
		if(j<0)return(j);
		i++;
	}
	if(*s==')')s++;

	ctx->nargs=i;
	ctx->argsz=sz;
	ctx->sig=sig;
	ctx->esig=s;
	ctx->ret=ctx->args+i;
	
	sz=0;
	j=DYLL_CompileSigArgList_Item(ctx, ctx->ret, &s, &sz);
	if(j<0)return(j);
	if(sz>ctx->argsz)ctx->argsz=sz;
	return(i);
}

BGBDY_API DYLL_ArgList *DYLL_CompileSigArgList(char *sig)
{
	DYLL_ValueTransfer args[64];
	DYLL_ArgList tctx;
	DYLL_ArgList *tmp;
	int i, n;

	tctx.args=args;
	i=DYLL_CompileSigArgListI(&tctx, sig);
	if(i<0)return(NULL);

	n=tctx.nargs;
	tmp=gctalloc("dyll_arglist_t", sizeof(DYLL_ArgList));
	tmp->args=gcalloc((n+1)*sizeof(DYLL_ValueTransfer));
	tmp->nargs=n;
	tmp->argsz=tctx.argsz;

	for(i=0; i<(n+1); i++)
		tmp->args[i]=args[i];
	tmp->ret=tmp->args+n;

	tmp->sig=dystrdup(tctx.sig);
	tmp->esig=dystrdup(tctx.esig);

	switch(n)
	{
	case 0:
		tmp->CopyValuesBuf=DYLL_CopyValuesBuf0;
		tmp->CopyBufValues=DYLL_CopyBufValues0;
		break;
	case 1:
		tmp->CopyValuesBuf=DYLL_CopyValuesBuf1;
		tmp->CopyBufValues=DYLL_CopyBufValues1;
		break;
	case 2:
		tmp->CopyValuesBuf=DYLL_CopyValuesBuf2;
		tmp->CopyBufValues=DYLL_CopyBufValues2;
		break;
	case 3:
		tmp->CopyValuesBuf=DYLL_CopyValuesBuf3;
		tmp->CopyBufValues=DYLL_CopyBufValues3;
		break;
	default:
		tmp->CopyValuesBuf=DYLL_CopyValuesBufN;
		tmp->CopyBufValues=DYLL_CopyBufValuesN;
		break;
	}

	return(tmp);
}
