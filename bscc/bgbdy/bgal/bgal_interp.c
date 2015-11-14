#include <bgbgal.h>

byte bgal_graytab[256];
byte bgal_ungraytab[256];

void BGAL_InterpInit()
{
	static int init=0;
	int i, j;

	if(init)return;
	init=1;

	for(i=0; i<256; i++)
	{
		j=i^(i>>1);
		bgal_graytab[i]=j;
		bgal_ungraytab[j]=i;
	}
}

BGBDY_API int BGAL_Gray(int i)
	{ BGAL_InterpInit(); return(bgal_graytab[i]); }
BGBDY_API int BGAL_UnGray(int i)
	{ BGAL_InterpInit(); return(bgal_ungraytab[i]); }

float BGAL_InterpPop(BGAL_ICtx *ctx)
{
	if(ctx->stackpos<=0)
		return(0);
	return(ctx->stack[--ctx->stackpos]);
}

int BGAL_InterpPopI(BGAL_ICtx *ctx)
{
	if(ctx->stackpos<=0)
		return(0);
	return(ctx->stack[--ctx->stackpos]);
}

int BGAL_InterpPush(BGAL_ICtx *ctx, float f)
{
	if(ctx->stackpos>250)
		return(-1);
	ctx->stack[ctx->stackpos++]=f;
	return(0);
}

int BGAL_StepCall(BGAL_ICtx *ctx, int idx)
{
	int i;

	if(ctx->rstackpos>=60)return(-1);
//	i=idx%ctx->obj->ctx->sz_prog;
	i=idx;
	if(i<0)return(-1);
	if(i>=ctx->obj->ctx->sz_prog)return(-1);

	if(ctx->obj->prog[i])
	{
		ctx->rstack_ips[ctx->rstackpos++]=ctx->ips;
		ctx->rstack_ipe[ctx->rstackpos++]=ctx->ipe;
		ctx->rstack_ip [ctx->rstackpos++]=ctx->ip;
		ctx->ip=ctx->obj->prog[i];
		ctx->ips=ctx->ip;
		ctx->ipe=ctx->ips+strlen(ctx->ips);
		return(0);
	}
	return(-1);
}

int BGAL_StepTCall(BGAL_ICtx *ctx, int idx)
{
	int i;

//	i=idx%ctx->obj->ctx->sz_prog;
	i=idx;
	if(i<0)return(-1);
	if(i>=ctx->obj->ctx->sz_prog)return(-1);

	if(ctx->obj->prog[i])
	{
		ctx->ip=ctx->obj->prog[i];
		ctx->ips=ctx->ip;
		ctx->ipe=ctx->ips+strlen(ctx->ips);
		return(0);
	}
	return(-1);
}

int BGAL_StepCallSub(BGAL_ICtx *ctx, int idx)
{
	int i;

	if(ctx->rstackpos>=60) return(-1);
	if((ctx->ip+idx)< ctx->ips) return(-1);
	if((ctx->ip+idx)>=ctx->ipe) return(-1);
	ctx->rstack_ips[ctx->rstackpos++]=ctx->ips;
	ctx->rstack_ipe[ctx->rstackpos++]=ctx->ipe;
	ctx->rstack_ip [ctx->rstackpos++]=ctx->ip;
	ctx->ip=ctx->ip+idx;
	return(0);
}

int BGAL_StepJmp(BGAL_ICtx *ctx, int idx)
{
	int i;

	if((ctx->ip+idx)< ctx->ips) return(-1);
	if((ctx->ip+idx)>=ctx->ipe) return(-1);
	ctx->ip=ctx->ip+idx;
	return(0);
}

int BGAL_Step(BGAL_ICtx *ctx)
{
	float f, g;
	int op, ret;
	int i, j, k;

	op=bgal_ungraytab[*ctx->ip++]; ret=0;
	switch(op)
	{
	case BGAL_NULL:
	case BGAL_STOP:
		if(ctx->rstackpos<1) { ret=1; break; }
		ctx->rstackpos--;
		ctx->ip =ctx->rstack_ip [ctx->rstackpos];
		ctx->ips=ctx->rstack_ips[ctx->rstackpos];
		ctx->ipe=ctx->rstack_ipe[ctx->rstackpos];
		break;
	case BGAL_NOP:
//	case BGAL_NOP1:
		break;
	case BGAL_ADD:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, f+g);
		break;
	case BGAL_SUB:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, f-g);
		break;
	case BGAL_DIV:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		if(g==0) { BGAL_InterpPush(ctx, 0); break; }
		BGAL_InterpPush(ctx, f/g);
		break;
	case BGAL_MUL:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, f*g);
		break;

	case BGAL_XOR:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		j=BGAL_InterpPop(ctx); i=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, i^j);
		break;
	case BGAL_MOD:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		if(g==0) { BGAL_InterpPush(ctx, 0); break; }
		BGAL_InterpPush(ctx, fmod(f, g));
		break;
	case BGAL_AND:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		j=BGAL_InterpPop(ctx); i=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, i&j);
		break;
	case BGAL_OR:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		j=BGAL_InterpPop(ctx); i=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, i|j);
		break;

	case BGAL_NOT:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		i=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, ~i);
		break;
	case BGAL_NEG:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		BGAL_InterpPush(ctx, -BGAL_InterpPop(ctx));
		break;
	case BGAL_FLOOR:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		BGAL_InterpPush(ctx, floor(BGAL_InterpPop(ctx)));
		break;
	case BGAL_CEIL:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		BGAL_InterpPush(ctx, ceil(BGAL_InterpPop(ctx)));
		break;

	case BGAL_SQR:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, f*f);
		break;
	case BGAL_SSQR:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, bgbrng_ssqr(f));
		break;
	case BGAL_SSQRT:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, bgbrng_ssqrt(f));
		break;
	case BGAL_SQRT:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, sqrt(fabs(f)));
		break;

	case BGAL_SIN:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, sin(f));
		break;
	case BGAL_COS:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, cos(f));
		break;
	case BGAL_ACOS:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, acos(f));
		break;
	case BGAL_ASIN:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, asin(f));
		break;

	case BGAL_TAN:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, tan(f));
		break;
	case BGAL_ATAN:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, atan(f));
		break;
	case BGAL_ATAN2:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, atan2(f, g));
		break;

	case BGAL_LPRED:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, 2*f-g);
		break;

	case BGAL_POP:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		break;
	case BGAL_DUP:
		if(ctx->stackpos<1) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, f);
		BGAL_InterpPush(ctx, f);
		break;
	case BGAL_EXCH:
		if(ctx->stackpos<2) { ret=-1; break; }
		if(ctx->stackpos>250) { ret=-1; break; }
		g=BGAL_InterpPop(ctx); f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, g); BGAL_InterpPush(ctx, f);
		break;

	case BGAL_LDVEC:
		if(ctx->stackpos>250) { ret=-1; break; }
		i=bgal_ungraytab[*ctx->ip++];
		i%=ctx->obj->ctx->sz_vec;
		BGAL_InterpPush(ctx, ctx->obj->vec[i]);
		break;

	case BGAL_CALL:
		i=bgal_ungraytab[*ctx->ip++];
		ret=BGAL_StepCall(ctx, i);
		break;
	case BGAL_CALLSUB:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		ret=BGAL_StepCallSub(ctx, i);
		break;
	case BGAL_IFCALL:
		i=bgal_ungraytab[*ctx->ip++];
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)<0.5)break;
		ret=BGAL_StepCall(ctx, i);
		break;
	case BGAL_IFCALLSUB:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)<0.5)break;
		ret=BGAL_StepCallSub(ctx, i);
		break;
	case BGAL_IFNCALL:
		i=bgal_ungraytab[*ctx->ip++];
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)>=0.5)break;
		ret=BGAL_StepCall(ctx, i);
		break;
	case BGAL_IFNCALLSUB:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)>=0.5)break;
		ret=BGAL_StepCallSub(ctx, i);
		break;

	case BGAL_TCALL:
		i=bgal_ungraytab[*ctx->ip++];
		ret=BGAL_StepTCall(ctx, i);
		break;
	case BGAL_JMP:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		ret=BGAL_StepJmp(ctx, i);
		break;
	case BGAL_IFTCALL:
		i=bgal_ungraytab[*ctx->ip++];
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)<0.5)break;
		ret=BGAL_StepTCall(ctx, i);
		break;
	case BGAL_IFJMP:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)<0.5)break;
		ret=BGAL_StepJmp(ctx, i);
		break;
	case BGAL_IFNTCALL:
		i=bgal_ungraytab[*ctx->ip++];
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)>=0.5)break;
		ret=BGAL_StepTCall(ctx, i);
		break;
	case BGAL_IFNJMP:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		if(fabs(f)>=0.5)break;
		ret=BGAL_StepJmp(ctx, i);
		break;

	case BGAL_LDN1: BGAL_InterpPush(ctx, -1); break;
	case BGAL_LD0: BGAL_InterpPush(ctx, 0); break;
	case BGAL_LD1: BGAL_InterpPush(ctx, 1); break;
	case BGAL_LD2: BGAL_InterpPush(ctx, 2); break;
	case BGAL_LD3: BGAL_InterpPush(ctx, 3); break;
	case BGAL_LD4: BGAL_InterpPush(ctx, 4); break;
	case BGAL_LD5: BGAL_InterpPush(ctx, 5); break;
	case BGAL_LDPI: BGAL_InterpPush(ctx, M_PI); break;

	case BGAL_LDSI:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		BGAL_InterpPush(ctx, i);
		break;
	case BGAL_MULSI:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, f*i);
		break;
	case BGAL_DIVSI:
		i=(char)(bgal_ungraytab[*ctx->ip++]);
		if(!i)i=1;
		if(ctx->stackpos<1) { ret=-1; break; }
		f=BGAL_InterpPop(ctx);
		BGAL_InterpPush(ctx, f/i);
		break;

	case BGAL_MUL2:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)*2);
		break;
	case BGAL_MUL3:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)*3);
		break;
	case BGAL_MUL4:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)*4);
		break;
	case BGAL_MUL8:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)*8);
		break;
	case BGAL_DIV2:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)/2);
		break;
	case BGAL_DIV3:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)/3);
		break;
	case BGAL_DIV4:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)/4);
		break;
	case BGAL_DIV8:
		if(ctx->stackpos<1) { ret=-1; break; }
		BGAL_InterpPush(ctx, BGAL_InterpPop(ctx)/8);
		break;

	case BGAL_ZLDSI: *ctx->ip++; break;

	case BGAL_ZCALL:
	case BGAL_ZCALLSUB:
//	case BGAL_ZIFCALL:
//	case BGAL_ZIFCSUB:
//	case BGAL_ZIFNCALL:
//	case BGAL_ZIFNCSUB:
		*ctx->ip++; break;

	case BGAL_ZTCALL:
	case BGAL_ZJMP:
//	case BGAL_ZIFTCALL:
//	case BGAL_ZIFJMP:
//	case BGAL_ZIFNTCALL:
//	case BGAL_ZIFNJMP:
		*ctx->ip++; break;

	default:
		ret=-1;
		break;
	}

	return(ret);
}

BGBDY_API int bgalEval(bgalMember *obj, float *iv, int ni, float *ov, int *rno)
{
	BGAL_ICtx ctxbuf;
	BGAL_ICtx *ctx;
	int i, j;

	BGAL_InterpInit();

	ctx=&ctxbuf;
	memset(ctx, 0, sizeof(BGAL_ICtx));
	ctx->obj=obj;

	for(i=0; i<ni; i++)
		ctx->stack[i]=iv[i];
	ctx->stackpos=ni;

	BGAL_StepTCall(ctx, 0);
	if(!ctx->ip)return(-1);

	j=256;
	while(j--)
	{
		if(!ctx->ip)return(-1);
		i=BGAL_Step(ctx);
		if(i)break;
	}

	if(!j)
	{
		printf("expire\n");
		return(-1);
	}

	memcpy(ov, ctx->stack, ctx->stackpos*sizeof(float));
	*rno=ctx->stackpos;

	return(i);
}

