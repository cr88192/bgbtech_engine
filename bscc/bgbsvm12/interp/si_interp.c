#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <bgbsvm.h>

#define bsSet(rf, vl)		rf=vl

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];
extern int bsvm_ops_nops;

void *bsvm_sync_lock=NULL;
void *bsvm_vm_lock=NULL;

dytf bsvm_toplevel;
int bsvm_opcnt;

dyt bsvm_special_apply(dyt fcn, dyt *args, int nargs)
{
	dyt t, l;
	int i;

	l=NULL;
	for(i=nargs-1; i>=0; i--)
		l=dycons(args[i], l);
//	t=BSVM_CallFuncObj(dyoThis(), fcn, l);
	t=BSVM_CallFuncObj(NULL, fcn, l);
	return(t);
}

dyt bsvm_special_applyobj(dyt fcn, dyt obj, dyt *args, int nargs)
{
	dyt t, l;
	int i;

	l=NULL;
	for(i=nargs-1; i>=0; i--)
		l=dycons(args[i], l);
//	t=BSVM_CallFuncObj(dyoThis(), fcn, l);
	t=BSVM_CallFuncObj(obj, fcn, l);
	return(t);
}

void bsvm_special_encbinary_blk(dysObjectStream ctx, dyt val)
{
	BSVM_SVMBlock *blk;
	dyt exps;

	blk=(BSVM_SVMBlock *)val;

//	exps=blk->exps;
	exps=NULL;

	dysBinaryEncodeVLI(ctx, 0x10);

	dysBinaryEncodeValue(ctx, blk->type);
	dysBinaryEncodeValue(ctx, blk->name);
	dysBinaryEncodeValue(ctx, blk->args);
	dysBinaryEncodeValue(ctx, blk->fargs);
	dysBinaryEncodeValue(ctx, exps);
	dysBinaryEncodeValue(ctx, blk->lit);
	dysBinaryEncodeValue(ctx, blk->attr);

	dysBinaryEncodeValue(ctx, (dyt)(blk->body));
	dysBinaryEncodeVLI(ctx, blk->szbody);

	dysBinaryEncodeVLI(ctx, blk->flags);
	dysBinaryEncodeVLI(ctx, blk->access);
	dysBinaryEncodeVLI(ctx, blk->vmflags);
}

dyt bsvm_special_decbinary_blk(dysObjectStream ctx)
{
	BSVM_SVMBlock *blk;
	int ver;

	blk=(BSVM_SVMBlock *)dysBinaryGetValue(ctx);
	if(!blk)
	{
		blk=(BSVM_SVMBlock *)gctalloc(
			"svm_blk_t", sizeof(BSVM_SVMBlock));
	}

	ver=dysBinaryDecodeVLI(ctx);

	blk->type=dysBinaryDecodeValue(ctx);
	blk->name=dysBinaryDecodeValue(ctx);
	blk->args=dysBinaryDecodeValue(ctx);
	blk->fargs=dysBinaryDecodeValue(ctx);
	blk->exps=dysBinaryDecodeValue(ctx);
	blk->lit=dysBinaryDecodeValue(ctx);
	blk->attr=dysBinaryDecodeValue(ctx);

	blk->body=(byte *)dysBinaryDecodeValue(ctx);
	blk->szbody=dysBinaryDecodeVLI(ctx);

	blk->flags=dysBinaryDecodeVLI(ctx);
	blk->access=dysBinaryDecodeVLI(ctx);
	blk->vmflags=dysBinaryDecodeVLI(ctx);

	return((dyt)blk);
}

void bsvm_special_encbinary_blkbc(dysObjectStream ctx, dyt val)
{
	int sz;

	sz=gcgetsize(val);
	dysBinaryEncodeBytes(ctx, (byte *)val, sz);
}

dyt bsvm_special_decbinary_blkbc(dysObjectStream ctx)
{
	byte *buf;
	int sz;

	sz=dysBinaryGetSize(ctx);
	buf=(byte *)dysBinaryGetValue(ctx);
	if(!buf)buf=(byte *)gctalloc("svm_blk_bytecode_t", sz);

	dysBinaryDecodeBytes(ctx, buf, sz);
	return((dyt)buf);
}

void bsvm_special_encbinary_varinfo(dysObjectStream ctx, dyt val)
{
	BSVM_SVMVarInfo *vinf;

	vinf=(BSVM_SVMVarInfo *)val;

	dysBinaryEncodeVLI(ctx, 0x10);

	dysBinaryEncodeValue(ctx, dysymbol(vinf->name));
	dysBinaryEncodeValue(ctx, dysymbol(vinf->sig));
	dysBinaryEncodeValue(ctx, dysymbol(vinf->flsig));
	dysBinaryEncodeValue(ctx, dytfUnwrap(vinf->value));
	dysBinaryEncodeValue(ctx, dytfUnwrap(vinf->attr));
	dysBinaryEncodeVLI(ctx, vinf->flags);
	dysBinaryEncodeVLI(ctx, vinf->access);
}

void bsvm_special_encbinary_overlayinfo(dysObjectStream ctx, dyt val)
{
	BSVM_SVMOverlayInfo *vinf;

	vinf=(BSVM_SVMOverlayInfo *)val;

	dysBinaryEncodeVLI(ctx, 0x10);

	dysBinaryEncodeValue(ctx, dysymbol(vinf->name));
	dysBinaryEncodeVLI(ctx, vinf->base);
	dysBinaryEncodeVLI(ctx, vinf->count);
	dysBinaryEncodeVLI(ctx, vinf->flags);
}

dyt bsvm_special_decbinary_varinfo(dysObjectStream ctx)
{
	BSVM_SVMVarInfo *vinf;
	int ver;

	vinf=(BSVM_SVMVarInfo *)dysBinaryGetValue(ctx);
	if(!vinf)
	{
		vinf=(BSVM_SVMVarInfo *)gctalloc(
			"svm_varinfo_t", sizeof(BSVM_SVMVarInfo));
	}

	ver=dysBinaryDecodeVLI(ctx);

	vinf->name=(char *)dysBinaryDecodeValue(ctx);
	vinf->sig=(char *)dysBinaryDecodeValue(ctx);
	vinf->flsig=(char *)dysBinaryDecodeValue(ctx);
	vinf->value=dytfWrap(dysBinaryDecodeValue(ctx));
	vinf->attr=dytfWrap(dysBinaryDecodeValue(ctx));
	vinf->flags=dysBinaryDecodeVLI(ctx);
	vinf->access=dysBinaryDecodeVLI(ctx);

	return((dyt)vinf);
}

dyt bsvm_special_decbinary_overlayinfo(dysObjectStream ctx)
{
	BSVM_SVMOverlayInfo *vinf;
	int ver;

	vinf=(BSVM_SVMOverlayInfo *)dysBinaryGetValue(ctx);
	if(!vinf)
	{
		vinf=(BSVM_SVMOverlayInfo *)gctalloc(
			"svm_overlayinfo_t", sizeof(BSVM_SVMOverlayInfo));
	}

	ver=dysBinaryDecodeVLI(ctx);

	vinf->name=(char *)dysBinaryDecodeValue(ctx);
	vinf->base=dysBinaryDecodeVLI(ctx);
	vinf->count=dysBinaryDecodeVLI(ctx);
	vinf->flags=dysBinaryDecodeVLI(ctx);
	return((dyt)vinf);
}

int bsvm_special_blockedThread(void *data)
{
	BSVM_SVMState *ctx;
	BSVM_SVMState *ctx2;
	dyt t, c, obj;

	ctx=data;

	if(!dytfNullP(ctx->thread_join))
	{
		obj=dytfUnwrap(ctx->thread_join);
	
		if(dyconsp(obj))
		{
			c=obj;
			while(dyconsp(c))
			{
				t=dycar(c);
				if(dytypep(t, "svm_state_t"))
				{
					ctx2=(BSVM_SVMState *)t;
					if(ctx2->rs!=BSVM_RETS_RETURN)
						break;
				}
				c=dycddr(c);
			}
			if(c)return(1);

			c=obj;
			while(dyconsp(c))
			{
				t=dycar(c);
				if(dytypep(t, "svm_state_t"))
				{
					ctx2=(BSVM_SVMState *)t;
					t=dytfUnwrap(BSVM_Pop(ctx2));
					BSVM_DestroyState(ctx2);
				}
				c=dycddr(c);
			}

			BSVM_Push(ctx, dytfWrap(t));
			return(0);
		}
	
		if(dytypep(obj, "svm_state_t"))
		{
			ctx2=(BSVM_SVMState *)obj;
			if(ctx2->rs!=BSVM_RETS_RETURN)
				return(1);

			t=dytfUnwrap(BSVM_Pop(ctx2));
			BSVM_Push(ctx, dytfWrap(t));
			BSVM_DestroyState(ctx2);
			return(0);
		}

		return(1);
	}

	return(0);
}

void *bsvm_special_runThread(void *data)
{
	BSVM_SVMState *ctx;
	dyAccessMode tacc;
	dyt t;
	int i, rs;

	ctx=data;
	
	tacc=dyGetAccessMode();
	dySetAccessMode(ctx->access);
	rs=BSVM_RunCnt(ctx, 1000);
	dySetAccessMode(tacc);

	if((rs==BSVM_RETS_NONE) ||
		(rs==BSVM_RETS_THROW))
	{
		thAddJob(bsvm_special_runThread, ctx);
		return(BGBGC_NULLEND);
	}

	if(rs==BSVM_RETS_BLOCK)
	{
		thAddJobBlocked(
			bsvm_special_runThread,
			bsvm_special_blockedThread,
			ctx);
		return(BGBGC_NULLEND);
	}

	if(rs==BSVM_RETS_SLEEP)
	{
		t=dytfUnwrap(BSVM_Pop(ctx));
		BSVM_Push(ctx, dytfWrap(t));
		i=dyintv(t);
		dyDPrintf("bsvm_special_runThread: Sleep %d\n", i);
		thAddJobDelay(bsvm_special_runThread, ctx, i);
		return(BGBGC_NULLEND);
	}

	if(rs==BSVM_RETS_RETURN)
	{
		if(ctx->vmgflags&BSVM_VMGFL_JOINED)
		{
			//done, leave context around.
			return(BGBGC_NULLEND);
		}else
		{
//			t=BSVM_Pop(ctx);
			BSVM_PopDestroy(ctx);
			BSVM_DestroyState(ctx);
//			return(t);
			return(BGBGC_NULLEND);
		}
	}

	return(BGBGC_NULLEND);
}

void BSVM_RunThread(BSVM_SVMState *ctx)
{
	thAddJob(bsvm_special_runThread, ctx);
}

void BSVM_LockVM()
{
	thLockFastMutex(bsvm_vm_lock);
}

void BSVM_UnlockVM()
{
	thUnlockFastMutex(bsvm_vm_lock);
}

BGBSVM_API int BSVM_Init_Base()
{
	bsvm_sync_lock=thMutex();
	bsvm_vm_lock=thFastMutex();

	if(dytfNullP(bsvm_toplevel))
		bsvm_toplevel=dytfWrap(dyObject());

	dyTypeApply("svm_blk_t", bsvm_special_apply);
	dyTypeApply("svm_lambda_t", bsvm_special_apply);
	dyTypeApply("svm_builtin_t", bsvm_special_apply);

	dyTypeApplyObj("svm_blk_t", bsvm_special_applyobj);
	dyTypeApplyObj("svm_lambda_t", bsvm_special_applyobj);
	dyTypeApplyObj("svm_builtin_t", bsvm_special_applyobj);

	dyTypeEncodeBinary("svm_blk_t", bsvm_special_encbinary_blk);
	dyTypeDecodeBinary("svm_blk_t", bsvm_special_decbinary_blk);

	dyTypeEncodeBinary("svm_blk_bytecode_t", bsvm_special_encbinary_blkbc);
	dyTypeDecodeBinary("svm_blk_bytecode_t", bsvm_special_decbinary_blkbc);

	dyTypeEncodeBinary("svm_varinfo_t", bsvm_special_encbinary_varinfo);
	dyTypeDecodeBinary("svm_varinfo_t", bsvm_special_decbinary_varinfo);

	dyTypeEncodeBinary("svm_overlayinfo_t",
		bsvm_special_encbinary_overlayinfo);
	dyTypeDecodeBinary("svm_overlayinfo_t",
		bsvm_special_decbinary_overlayinfo);

	return(0);
}

static int bsvm_init=0;

/** Special Init: Don't bring up whole VM, only parts needed for tools. */
BGBSVM_API int BSVM_ToolsInit()
{
//	static int init=0;
	dyt t;

	if(bsvm_init)return(0);
	bsvm_init=1;

	BSVM_Init_Base();
	return(0);
}

/** Main Init */
BGBSVM_API int BSVM_Init()
{
//	static int init=0;
	dyt t, top;

	if(bsvm_init)return(0);
	bsvm_init=1;

	gcDelayGC(1);	//disable GC for now

	BSVM_Init_Base();

#ifdef _WIN32
	dyllLoadLibrary("msvcrt");
#endif

	BSVM_AddBinding("top", bsvm_toplevel);
	
	top=dytfUnwrap(bsvm_toplevel);
	t=dyllNewCTop("ctop", NULL);
	dySet(top, "ctop", t);
	dySetDelegate(top, "ctop", t);

	t=dyllNewCTop("bstop", "BSAPI");
	dySet(top, "bstop", t);
	dySetDelegate(top, "bstop", t);

	t=dyllNewCTop("Math", "BSAPI_Math");
	dySet(top, "Math", t);

	BSVM_Builtins_Init();
	BSVM_BVT_Init();

#ifdef X86
	BSVM_ThreadX86_Init();
#endif

	bsvmRootImportPackage("bsvm/init");

	gcDelayGC(0);

	return(1);
}



dytf BSVM_ReadID(BSVM_SVMState *ctx)
{
	dytf t;
	int i;

	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	t=dytfArrayIdx(ctx->lit, i);
	return(t);
}

int BSVM_ReadIDX(BSVM_SVMState *ctx)
{
	int i;
	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	return(i);
}

int BSVM_ReadSIDX(BSVM_SVMState *ctx)
{
	int i;
	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	i=(i&1)?(-((i+1)>>1)):(i>>1);
	return(i);
}

s64 BSVM_ReadVLI(BSVM_SVMState *ctx)
{
	return(BS2ASM_DecVLI(&(ctx->ip)));
}

s64 BSVM_ReadSVLI(BSVM_SVMState *ctx)
{
	return(BS2ASM_DecSVLI(&(ctx->ip)));
}

int BSVM_ReadShort(BSVM_SVMState *ctx)
{
	int i;
	
	i=*ctx->ip++;
	i|=(*ctx->ip++)<<8;
	return((signed short)i);
}

int BSVM_ReadRelAddr(BSVM_SVMState *ctx)
{
	int i;

	if(ctx->pfhint&BSVM_PF_WIDE)
	{
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i|=(*ctx->ip++)<<16;
		i|=(*ctx->ip++)<<24;
		return((s32)i);
	}
	
	i=*ctx->ip++;
	i|=(*ctx->ip++)<<8;
	return((signed short)i);
}

int BSVM_ReadArrayIndex(BSVM_SVMState *ctx)
{
	int i;

	if(ctx->pfhint&BSVM_PF_WIDE)
	{
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i|=(*ctx->ip++)<<16;
		i|=(*ctx->ip++)<<24;
		return((s32)i);
	}
	
	i=*ctx->ip++;
	return(i);
}

int BSVM_DumpOpcode(BSVM_SVMState *ctx)
{
	char *uops[]={"neg", "not", "lnot", "pos", "umul", "udiv", "uidiv",
		"umod", "uand", "uor", "uxor", "uland", "ulor", "ulxor", "inc",
		"dec", "defer", "car", "cdr",
		"copyvalue", "dropvalue", "delete", "clone",
		"sin", "cos", "tan", "asin", "acos", "atan", "sqr", "sqrt"};
	char *bops[]={"add", "sub", "mul", "div", "mod", "and", "or",
		"xor", "shl", "shr", "eq", "eqv", "neq", "neqv", "l",
		"g", "le", "ge", "idiv", "dadd", "dsub", "dmul", "ddiv",
		"dmod", "dand", "dor", "dxor", "didiv", "exp", "shrr",
		"cons", "atan2"};

	dyt t;
	byte *ip, *ip1;
	char *s;
	int i, j, k;

	ip=ctx->ip;

	i=*ip++;
	if(i>=192)
	{
		i=(i-192)*256+(*ip++);
		dyDPrintf("%08X %02X%02X ", ctx->ip,
			ctx->ip[0], ctx->ip[1]);
	}else
	{
		dyDPrintf("%08X %02X ", ctx->ip, i);
	}

#if 0
	if((i<0) || (i>=bsvm_ops_nops))
	{
		dyDPrintf("BAD\n");
		return;
	}
#endif

	k=0; s=bsvm_ops_args[i]; ip1=ip;
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N') || (*s=='J'))
			{ j=*ip1++; k++; if(j&128)k++; s++; continue; }
		if(*s=='A') { ip1+=2; k+=2; s++; continue; }

		if(*s=='C') { ip1++; k++; s++; continue; }
		if(*s=='B') { ip1++; k++; s++; continue; }
		if(*s=='W') { ip1++; k+=2; s++; continue; }
		if(*s=='K') { ip1++; k+=2; s++; continue; }
		if(*s=='H') { ip1++; k+=2; s++; continue; }

		if(*s=='u') { ip1++; k++; s++; continue; }
		if(*s=='b') { ip1++; k++; s++; continue; }

		s++;
	}

	ip1=ip;
	for(j=0; j<k; j++)
		dyDPrintf("%02X", *ip1++);

	if(i<192)
		dyDPrintf("  ");
	for(; j<4; j++)
		dyDPrintf("  ");

	dyDPrintf(" %s ", bsvm_ops_strs[i]);

	s=bsvm_ops_args[i];
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			t=dyarrayidx(dytfUnwrap(ctx->lit), i);
//			BSVM_TY_Print(t);
			dyDPrintf(" ");
			s++;
			continue;
		}
		if((*s=='N') || (*s=='J'))
		{
			i=*ip++;
			if(i&128)i=((i&127)<<8)|(*ip++);
			dyDPrintf("%d ", i);
			s++;
			continue;
		}
		if(*s=='A')
		{
			i=*ip++; i|=(*ip++)<<8;
			dyDPrintf("%08X ", ip+i);
			s++;
			continue;
		}
		if(*s=='C') { i=(signed char)(*ip++); dyDPrintf("%d ", i);
			s++; continue; }
		if(*s=='B') { i=*ip++; dyDPrintf("%d ", i); s++; continue; }
		if(*s=='K') { i=*ip++; i|=(*ip++)<<8; i=(short)i; dyDPrintf("%d ", i);
			s++; continue; }
		if(*s=='W') { i=*ip++; i|=(*ip++)<<8; dyDPrintf("%d ", i);
			s++; continue; }
		if(*s=='H') { i=*ip++; i|=(*ip++)<<8; dyDPrintf("%g ", i/100.0);
			s++; continue; }

		if(*s=='b') { i=*ip++; dyDPrintf("%s ", bops[i]); s++; continue; }
		if(*s=='u') { i=*ip++; dyDPrintf("%s ", uops[i]); s++; continue; }
		s++;
	}

	dyDPrintf("\n");

	return(ip-ctx->ip);
}

void BSVM_SkipOpcodeArgs(BSVM_SVMState *ctx, int op)
{
	dyt t;
	byte *ip, *ip1, *ip2;
	char *s;
	int i, j, k, l;

	ip=ctx->ip;

	k=0; s=bsvm_ops_args[op]; ip1=ip;
	while(*s)
	{
		if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N') || (*s=='J'))
			{ j=*ip1++; k++; if(j&128) { ip1++; k++;} s++; continue; }
		if(*s=='A') { ip1+=2; k+=2; s++; continue; }

		if(*s=='C') { ip1++; k++; s++; continue; }
		if(*s=='B') { ip1++; k++; s++; continue; }
		if(*s=='W') { ip1++; k+=2; s++; continue; }
		if(*s=='K') { ip1++; k+=2; s++; continue; }
		if(*s=='H') { ip1++; k+=2; s++; continue; }

		if((*s=='U') || (*s=='V'))
		{
			ip2=ip1; BS2ASM_DecVLI(&ip2); l=ip2-ip1;
			ip1+=l; k+=l; s++; continue;
		}

		if(*s=='u') { ip1++; k++; s++; continue; }
		if(*s=='b') { ip1++; k++; s++; continue; }

		s++;
	}

	ctx->ip=ip1;
}

int BSVM_SkipOpcodePrefix(BSVM_SVMState *ctx, int op)
{
	int j;

	while(1)
	{
		j=0;
		switch(op)
		{
		case BSVM_SOP_PF_WIDE:
			ctx->pfhint|=BSVM_PF_WIDE; break;
		case BSVM_SOP_PF_HINT_XI:
			ctx->pfhint|=BSVM_PF_HINT_XI; break;
		case BSVM_SOP_PF_HINT_XL:
			ctx->pfhint|=BSVM_PF_HINT_XL; break;
		case BSVM_SOP_PF_HINT_XF:
			ctx->pfhint|=BSVM_PF_HINT_XF; break;
		case BSVM_SOP_PF_HINT_XD:
			ctx->pfhint|=BSVM_PF_HINT_XD; break;
		default: j=1; break;
		}
		if(j)break;

		op=*ctx->ip++;
		if(op>=192)op=(op-192)*256+(*ctx->ip++);
	}
	return(op);
}

void BSVM_SkipOpcode(BSVM_SVMState *ctx)
{
	dyt t;
	byte *ip, *ip1;
	char *s;
	int i, j, k;

	BSVM_Thread_DecodePrefix(ctx);

	ip=ctx->ip;
	i=*ip++;
	if(i>=192)
		{ i=(i-192)*256+(*ip++); }
	ctx->ip=ip;
//	i=BSVM_SkipOpcodePrefix(ctx, i);
	BSVM_SkipOpcodeArgs(ctx, i);
}

void BSVM_StepJmpBool(BSVM_SVMState *ctx, int j)
{
	int i;

	i=*ctx->ip++;
	i|=(*ctx->ip++)<<8;
	i=((signed short)i);
	if(j)ctx->ip+=i;
}

#if 0
int BSVM_StepC1(BSVM_SVMState *ctx)
{
	dyt t, u, v;
	int i, j;

	i=256+(*ctx->ip++);
	switch(i)
	{
	case BSVM_SOP_JMP_L_FN:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, u<v);
		break;
	case BSVM_SOP_JMP_G_FN:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, u>v);
		break;
	case BSVM_SOP_JMP_LE_FN:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, u<=v);
		break;
	case BSVM_SOP_JMP_GE_FN:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, u>=v);
		break;

	case BSVM_SOP_JMP_L_FL:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)<dytfFloatv(v));
		break;
	case BSVM_SOP_JMP_G_FL:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)>dytfFloatv(v));
		break;
	case BSVM_SOP_JMP_LE_FL:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)<=dytfFloatv(v));
		break;
	case BSVM_SOP_JMP_GE_FL:
		v=ctx->stack[--ctx->stackpos];
		u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)>=dytfFloatv(v));
		break;

	case BSVM_SOP_JMP_E_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
//		u=BSVM_LookupIdx(ctx, *ctx->ip++);
//		v=BSVM_LookupIdx(ctx, *ctx->ip++);
		BSVM_StepJmpBool(ctx, u==v);
		break;
	case BSVM_SOP_JMP_NE_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, u!=v);
		break;
	case BSVM_SOP_JMP_L_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, u<v);
		break;
	case BSVM_SOP_JMP_G_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, u>v);
		break;
	case BSVM_SOP_JMP_LE_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, u<=v);
		break;
	case BSVM_SOP_JMP_GE_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, u>=v);
		break;

	case BSVM_SOP_JMP_E_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, t==dytfInt(0));
		break;
	case BSVM_SOP_JMP_NE_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, t!=dytfInt(0));
		break;
	case BSVM_SOP_JMP_L_ZFN:
		t=ctx->stack[--ctx->stackpos];
//		BSVM_StepJmpBool(ctx, t<dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)<0);
		break;
	case BSVM_SOP_JMP_G_ZFN:
		t=ctx->stack[--ctx->stackpos];
//		BSVM_StepJmpBool(ctx, t>dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)>0);
		break;
	case BSVM_SOP_JMP_LE_ZFN:
		t=ctx->stack[--ctx->stackpos];
//		BSVM_StepJmpBool(ctx, t<=dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)<=0);
		break;
	case BSVM_SOP_JMP_GE_ZFN:
		t=ctx->stack[--ctx->stackpos];
//		BSVM_StepJmpBool(ctx, t>=dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)>=0);
		break;

	case BSVM_SOP_JMP_E_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, t==dytfInt(0));
//		BSVM_StepJmpBool(ctx, !dyintv(t));
		break;
	case BSVM_SOP_JMP_NE_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, t!=dytfInt(0));
//		BSVM_StepJmpBool(ctx, dyintv(t));
		break;
	case BSVM_SOP_JMP_L_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
//		BSVM_StepJmpBool(ctx, t<dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)<0);
		break;
	case BSVM_SOP_JMP_G_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
//		BSVM_StepJmpBool(ctx, t>dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)>0);
		break;
	case BSVM_SOP_JMP_LE_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
//		BSVM_StepJmpBool(ctx, t<=dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)<=0);
		break;
	case BSVM_SOP_JMP_GE_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
//		BSVM_StepJmpBool(ctx, t>=dytfInt(0));
		BSVM_StepJmpBool(ctx, dytfIntv(t)>=0);
		break;
	default:
		break;
	}

	return(0);
}
#endif

int BSVM_Step(BSVM_SVMState *ctx, int pf)
{
	dytf t, u, v, w, x, y, z;
	int i, j, k, rs;

//	dyDPrintf("%d ", *ctx->ip);
//	BSVM_DumpOpcode(ctx);

//	i=*ctx->ip++;
//	if(i>=192)i=(i-192)*256+(*ctx->ip++);

	i=pf+(*ctx->ip++);

	rs=0;
//	switch(*ctx->ip++)
	switch(i)
	{
	case BSVM_SOP_NOP:
		break;
	case BSVM_SOP_BLOCK:
		rs=1;
		break;
	case BSVM_SOP_DBGMARK:
		t=BSVM_ReadID(ctx);
		t=BSVM_ReadID(ctx);
		break;
	case BSVM_SOP_CGENAST:
		t=BSVM_ReadID(ctx);
		break;
	case BSVM_SOP_FN:
		t=BSVM_ReadID(ctx);
		break;
	case BSVM_SOP_LN:
		t=BSVM_ReadID(ctx);
		break;
	case BSVM_SOP_LABEL:
		t=BSVM_ReadID(ctx);
		break;

	case BSVM_SOP_SLEEP:
		rs=4;
		break;
	case BSVM_SOP_SLEEP_SV:
		i=*ctx->ip++;
		i+=(*ctx->ip++)<<8;
		BSVM_PushF(ctx, dytfInt(i));
		rs=4;
		break;

	case BSVM_SOP_PUSH:
		t=BSVM_ReadID(ctx);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_POP:
		BSVM_PopDestroy(ctx);
		break;
	case BSVM_SOP_LOAD:
		t=BSVM_ReadID(ctx);
		t=BSVM_Lookup(ctx, t);
		t=dytfCopyValue(t);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_STORE:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		BSVM_Assign(ctx, t, u);
		break;
	case BSVM_SOP_BIND:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		BSVM_Bind(ctx, t, u);
		break;
	case BSVM_SOP_DYNBIND:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		BSVM_DynBind(ctx, t, u);
		break;
	case BSVM_SOP_LEXBIND:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		BSVM_LexBind(ctx, t, u);
		break;
	case BSVM_SOP_LOADA_S:
		t=BSVM_ReadID(ctx);
		t=BSVM_LookupRef(ctx, t);
		BSVM_Push(ctx, t);
		break;

	case BSVM_SOP_LOADINDEX:
		u=BSVM_Pop(ctx);
		t=BSVM_Pop(ctx);
		v=BSVM_LoadIndex(ctx, t, u);
		v=dytfCopyValue(v);
		BSVM_Push(ctx, v);
		break;
	case BSVM_SOP_STOREINDEX:
		v=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		t=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, u, v);
		break;
	case BSVM_SOP_LOADINDEX_S:
		u=BSVM_ReadID(ctx);
		t=BSVM_Pop(ctx);
		v=BSVM_LoadIndex(ctx, t, u);
		BSVM_Push(ctx, v);
		break;
	case BSVM_SOP_STOREINDEX_S:
		u=BSVM_ReadID(ctx);
		t=BSVM_Pop(ctx);
		v=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, u, v);
		break;

	case BSVM_SOP_LOADINDEX_I:
		u=dytfInt(*ctx->ip++);
		t=BSVM_Pop(ctx);
		v=BSVM_LoadIndex(ctx, t, u);
		BSVM_Push(ctx, v);
		break;
	case BSVM_SOP_STOREINDEX_I:
		u=dytfInt(*ctx->ip++);
		t=BSVM_Pop(ctx);
		v=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, u, v);
		break;

	case BSVM_SOP_LOADINDEX_0:
		t=BSVM_Pop(ctx);
		v=BSVM_LoadIndex(ctx, t, dytfInt(0));
		BSVM_Push(ctx, v); break;
	case BSVM_SOP_LOADINDEX_1:
		t=BSVM_Pop(ctx);
		v=BSVM_LoadIndex(ctx, t, dytfInt(1));
		BSVM_Push(ctx, v); break;
	case BSVM_SOP_LOADINDEX_2:
		t=BSVM_Pop(ctx);
		v=BSVM_LoadIndex(ctx, t, dytfInt(2));
		BSVM_Push(ctx, v); break;
	case BSVM_SOP_LOADINDEX_3:
		t=BSVM_Pop(ctx);
		v=BSVM_LoadIndex(ctx, t, dytfInt(3));
		BSVM_Push(ctx, v); break;

	case BSVM_SOP_STOREINDEX_0:
		t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, dytfInt(0), u); break;
	case BSVM_SOP_STOREINDEX_1:
		t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, dytfInt(1), u); break;
	case BSVM_SOP_STOREINDEX_2:
		t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, dytfInt(2), u); break;
	case BSVM_SOP_STOREINDEX_3:
		t=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, dytfInt(3), u); break;

	case BSVM_SOP_EXCH_SELF:
		u=ctx->stack[ctx->stackpos-1]; v=ctx->self;
		ctx->self=u; ctx->stack[ctx->stackpos-1]=v;
		break;
	case BSVM_SOP_EXCH_TOP:
		u=ctx->stack[ctx->stackpos-1]; v=ctx->top;
		ctx->top=u; ctx->stack[ctx->stackpos-1]=v;
		break;

	case BSVM_SOP_PUSH_SELF:
		BSVM_PushF(ctx, ctx->self);
		break;
	case BSVM_SOP_PUSH_NULL:
		BSVM_PushF(ctx, DYTF_NULL);
		break;
	case BSVM_SOP_PUSH_TRUE:
		BSVM_PushF(ctx, DYTF_TRUE);
		break;
	case BSVM_SOP_PUSH_FALSE:
		BSVM_PushF(ctx, DYTF_FALSE);
		break;
	case BSVM_SOP_PUSH_SV:
//		i=(signed char)(*ctx->ip++);
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		BSVM_PushF(ctx, dytfInt((short)i));
		break;
	case BSVM_SOP_PUSH_SV_P:
		i=*ctx->ip++;
		BSVM_PushF(ctx, dytfInt(i));
		break;
	case BSVM_SOP_PUSH_SV_N:
		i=*ctx->ip++;
		BSVM_PushF(ctx, dytfInt(-i));
		break;
	case BSVM_SOP_PUSH_SV_F:
		i=*ctx->ip++;
		i+=(*ctx->ip++)<<8;
		BSVM_PushF(ctx, dytfFloat(((short)i)/100.0));
		break;
	case BSVM_SOP_PUSH_SV_FB:
		i=*ctx->ip++;
		i+=(*ctx->ip++)<<8;
		BSVM_PushF(ctx, dytfFloat(((short)i)/10000.0));
		break;
	case BSVM_SOP_PUSH_SV_C:
		i=*ctx->ip++;
		i+=(*ctx->ip++)<<8;
		BSVM_PushF(ctx, dytfChar(i));
		break;

	case BSVM_SOP_PUSH_0: BSVM_PushF(ctx, dytfInt(0)); break;
	case BSVM_SOP_PUSH_1: BSVM_PushF(ctx, dytfInt(1)); break;
	case BSVM_SOP_PUSH_2: BSVM_PushF(ctx, dytfInt(2)); break;
	case BSVM_SOP_PUSH_3: BSVM_PushF(ctx, dytfInt(3)); break;
	case BSVM_SOP_PUSH_4: BSVM_PushF(ctx, dytfInt(4)); break;
	case BSVM_SOP_PUSH_5: BSVM_PushF(ctx, dytfInt(5)); break;
	case BSVM_SOP_PUSH_6: BSVM_PushF(ctx, dytfInt(6)); break;
	case BSVM_SOP_PUSH_7: BSVM_PushF(ctx, dytfInt(7)); break;

	case BSVM_SOP_PUSH_CF: BSVM_Push(ctx, ctx->func); break;

	case BSVM_SOP_LLOAD:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_LSTORE:
		i=*ctx->ip++;
		t=BSVM_Pop(ctx);
		BSVM_AssignIdx(ctx, i, t);
		break;
	case BSVM_SOP_LLOAD_F:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		BSVM_PushF(ctx, t);
		break;
	case BSVM_SOP_LSTORE_F:
		i=*ctx->ip++;
		t=BSVM_PopF(ctx);
		BSVM_AssignIdxF(ctx, i, t);
		break;

	case BSVM_SOP_LPOSTINC:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		u=dytfInc(t);
		BSVM_AssignIdx(ctx, i, u);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_LPOSTDEC:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		u=dytfDec(t);
		BSVM_AssignIdx(ctx, i, u);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_LPREINC:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		u=dytfInc(t);
		BSVM_AssignIdx(ctx, i, u);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_LPREDEC:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		u=dytfDec(t);
		BSVM_AssignIdx(ctx, i, u);
		BSVM_Push(ctx, u);
		break;

#if 0
	case BSVM_SOP_LLOAD_F2:
		i=*ctx->ip++;
		t=ctx->stack[ctx->eargs-i-1];
		ctx->stack[ctx->stackpos++]=t;
		break;
	case BSVM_SOP_LSTORE_F2:
		i=*ctx->ip++;
		t=ctx->stack[--ctx->stackpos];
		ctx->stack[ctx->eargs-i-1]=t;
		break;
#endif

//	case BSVM_SOP_LINC_FN:
	case BSVM_SOP_LINC:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		BSVM_AssignIdxF(ctx, i, dytfInt(dytfIntv(t)+1));
		break;
//	case BSVM_SOP_LDEC_FN:
	case BSVM_SOP_LDEC:
		i=*ctx->ip++;
		t=BSVM_LookupIdx(ctx, i);
		BSVM_AssignIdxF(ctx, i, dytfInt(dytfIntv(t)-1));
		break;

#if 0
	case BSVM_SOP_LINC_FN2:
		i=*ctx->ip++; j=ctx->eargs-i-1;
		t=ctx->stack[j];
		ctx->stack[j]=dytfInt(dytfIntv(t)+1);
		break;
	case BSVM_SOP_LDEC_FN2:
		i=*ctx->ip++; j=ctx->eargs-i-1;
		t=ctx->stack[j];
		ctx->stack[j]=dytfInt(dytfIntv(t)-1);
		break;
#endif

	case BSVM_SOP_CLEARENV:
		BSVM_ClearEnv(ctx);
		break;

	case BSVM_SOP_BINDPATTERN: break;
	case BSVM_SOP_LOADMINDEX: break;
	case BSVM_SOP_STOREMINDEX: break;

	case BSVM_SOP_TRYLOAD:
		t=BSVM_ReadID(ctx);
		t=BSVM_Lookup(ctx, t);
		BSVM_Push(ctx, t);
		break;

	case BSVM_SOP_CAST_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		u=BSVM_Cast(ctx, t, u);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_CAST:
		t=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		u=BSVM_Cast(ctx, t, u);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_SIZEOF_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_OpSizeOf(ctx, t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_SIZEOF:
		t=BSVM_Pop(ctx);
		u=BSVM_OpSizeOf(ctx, t);
		BSVM_Push(ctx, u);
		break;

	case BSVM_SOP_MARK:
		ctx->markstack[ctx->markstackpos++]=ctx->stackpos;
		break;

	case BSVM_SOP_CALL:
		t=BSVM_Pop(ctx);
		BSVM_CallObj(ctx, ctx->self, t);
		break;
	case BSVM_SOP_TAILCALL:
		t=BSVM_Pop(ctx);
		BSVM_TailCallObj(ctx, ctx->self, t);
		if(ctx->flowstackpos<0)
		{
			rs=2;
			break;
		}
		break;
	case BSVM_SOP_CALL_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Lookup(ctx, t);
//		if(u==UNDEFINED)
//			dyDPrintf("Fail Lookup %s\n", dysymbolv(t));
		BSVM_CallObj(ctx, ctx->self, u);
		break;
	case BSVM_SOP_TAILCALL_S:
		t=BSVM_ReadID(ctx);
		t=BSVM_Lookup(ctx, t);
		BSVM_TailCallObj(ctx, ctx->self, t);
		if(ctx->flowstackpos<0)
		{
			rs=2;
			break;
		}
		break;

	case BSVM_SOP_METHODCALL:
		t=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		BSVM_MethodCall(ctx, u, t);
		break;
	case BSVM_SOP_METHODTAILCALL:
		t=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		BSVM_MethodTailCall(ctx, u, t);
		break;
	case BSVM_SOP_METHODCALL_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		BSVM_MethodCall(ctx, u, t);
		break;
	case BSVM_SOP_METHODTAILCALL_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		BSVM_MethodTailCall(ctx, u, t);
		break;

	case BSVM_SOP_CALL_ASYNC:
		t=BSVM_Pop(ctx);
		BSVM_CallObjAsync(ctx, ctx->self, t);
		break;

	case BSVM_SOP_CALL_IFDEF:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		if(BSVM_CheckDefined(ctx, t))
		{
			BSVM_CallObj(ctx, ctx->self, u);
		}else
		{
			v=BSVM_PopList(ctx);
			BSVM_Push(ctx, DYTF_NULL);
		}
		break;
	case BSVM_SOP_CALL_IFNDEF:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		if(!BSVM_CheckDefined(ctx, t))
		{
			BSVM_CallObj(ctx, ctx->self, u);
		}else
		{
			v=BSVM_PopList(ctx);
			BSVM_Push(ctx, DYTF_NULL);
		}
		break;

	case BSVM_SOP_BEGIN_SYNC:
		BSVM_OpBeginSync(ctx);
		break;
	case BSVM_SOP_END_SYNC:
		BSVM_OpEndSync(ctx);
		break;
	case BSVM_SOP_BEGIN_SYNC_OBJ:
	case BSVM_SOP_LOCK_OBJ:
		t=BSVM_Pop(ctx);
		BSVM_OpBeginSyncObj(ctx, t);
		break;
	case BSVM_SOP_END_SYNC_OBJ:
	case BSVM_SOP_UNLOCK_OBJ:
		t=BSVM_Pop(ctx);
		BSVM_OpEndSyncObj(ctx, t);
		break;

	case BSVM_SOP_BEGIN_TRY:
		BSVM_OpBeginTry(ctx);
		break;
	case BSVM_SOP_END_TRY:
		BSVM_OpEndTry(ctx);
		break;
	case BSVM_SOP_END_CATCH:
		BSVM_OpEndCatch(ctx);
		if(!dytfNullP(ctx->thrown_obj))
			rs=1;
		break;

	case BSVM_SOP_BEGIN_CATCH_FINAL:
		BSVM_OpBeginCatchFinal(ctx);
		break;
	case BSVM_SOP_BEGIN_CATCH:
		t=BSVM_ReadID(ctx);
		BSVM_OpBeginCatch(ctx, t);
		break;

	case BSVM_SOP_THROW_S:
		t=BSVM_ReadID(ctx);
//		t=BSVM_Pop(ctx);
		BSVM_OpThrowS(ctx, t);
		rs=1;
		break;
	case BSVM_SOP_THROW_OBJ:
		t=BSVM_Pop(ctx);
		BSVM_OpThrowObj(ctx, t);
		rs=1;
		break;

	case BSVM_SOP_CATCH:
		t=BSVM_ReadID(ctx);

//		j=BSVM_PopBool(ctx);
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
//		if(j)ctx->ip+=i;

//		t=BSVM_Pop(ctx);
//		BSVM_OpThrowObj(ctx, t);
		break;

	case BSVM_SOP_NEW_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_OpNew(ctx, t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_NEWARRAY_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Pop(ctx);
		v=BSVM_OpNewArray(ctx, t, u);
		BSVM_Push(ctx, v);
		break;
	case BSVM_SOP_DELETE:
		t=BSVM_Pop(ctx);
		u=BSVM_OpDelete(ctx, t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_CLONE:
		t=BSVM_Pop(ctx);
		u=dytfClone(t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_TOSTRING:
		t=BSVM_Pop(ctx);
		u=dytfWrap(dystring(dyToString(dytfUnwrap(t))));
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_UNQUOTE:
		t=BSVM_Pop(ctx);
//		u=dystring(dyToString(t));
		u=dytfWrap(BSVM_EvalExprsObj(
			dylist1(dytfUnwrap(t)), dytfUnwrap(ctx->self)));
		BSVM_Push(ctx, u);
		break;

	case BSVM_SOP_NEG:
		t=BSVM_Pop(ctx);
		u=dytfNeg(t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_LNOT:
		t=BSVM_Pop(ctx);
		u=dytfLNot(t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_DEFER:
		t=BSVM_Pop(ctx);
		u=dytfUMul(t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_SETDEFER:
		t=BSVM_Pop(ctx); v=BSVM_Pop(ctx);
		BSVM_StoreIndex(ctx, t, dytfInt(0), v); break;
		break;
	case BSVM_SOP_NOT:
		t=BSVM_Pop(ctx);
		u=dytfNot(t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_INC:
		t=BSVM_Pop(ctx);
		u=dytfInc(t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_DEC:
		t=BSVM_Pop(ctx);
		u=dytfDec(t);
		BSVM_Push(ctx, u);
		break;

	case BSVM_SOP_INC_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Lookup(ctx, t);
		u=dytfInc(u);
		BSVM_Assign(ctx, t, u);
		break;
	case BSVM_SOP_DEC_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Lookup(ctx, t);
		u=dytfDec(u);
		BSVM_Assign(ctx, t, u);
		break;
	case BSVM_SOP_POSTINC_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Lookup(ctx, t);
		v=dytfInc(u);
		BSVM_Assign(ctx, t, v);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_POSTDEC_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Lookup(ctx, t);
		v=dytfDec(u);
		BSVM_Assign(ctx, t, v);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_PREINC_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Lookup(ctx, t);
		v=dytfInc(u);
		BSVM_Assign(ctx, t, v);
		BSVM_Push(ctx, v);
		break;
	case BSVM_SOP_PREDEC_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_Lookup(ctx, t);
		v=dytfDec(u);
		BSVM_Assign(ctx, t, v);
		BSVM_Push(ctx, v);
		break;

	case BSVM_SOP_TYBIND:
		t=BSVM_ReadID(ctx);
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		w=BSVM_Pop(ctx);
		BSVM_OpTyBind(ctx, t, u, v, w);
		break;
	case BSVM_SOP_TYDYNBIND:
		t=BSVM_ReadID(ctx);
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		w=BSVM_Pop(ctx);
		BSVM_OpTyDynBind(ctx, t, u, v, w);
		break;
	case BSVM_SOP_TYLEXBIND:
		t=BSVM_ReadID(ctx);
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		w=BSVM_Pop(ctx);
		BSVM_OpTyLexBind(ctx, t, u, v, w);
		break;

	case BSVM_SOP_BEGIN_PACKAGE:
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		BSVM_OpBeginPackage(ctx, u, v);
		break;
	case BSVM_SOP_BEGIN_CLASS:
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		w=BSVM_ReadID(ctx);
		BSVM_OpBeginClass(ctx, u, v, w);
		break;
	case BSVM_SOP_BEGIN_IFACE:
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		w=BSVM_ReadID(ctx);
		BSVM_OpBeginIface(ctx, u, v, w);
		break;
	case BSVM_SOP_BEGIN_STRUCT:
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		w=BSVM_ReadID(ctx);
		BSVM_OpBeginStruct(ctx, u, v, w);
		break;
	case BSVM_SOP_END_PACKAGE:
		BSVM_OpEndPackage(ctx);
		break;
	case BSVM_SOP_END_CLASS:
		BSVM_OpEndClass(ctx);
		break;
	case BSVM_SOP_END_IFACE:
		BSVM_OpEndIface(ctx);
		break;
	case BSVM_SOP_END_STRUCT:
		BSVM_OpEndStruct(ctx);
		break;
	case BSVM_SOP_CLASS_IFACE:
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		BSVM_OpClassIface(ctx, u, v);
		break;
	case BSVM_SOP_IMPORT:
		u=BSVM_ReadID(ctx);
		v=BSVM_ReadID(ctx);
		BSVM_OpImport(ctx, u, v);
		break;

	case BSVM_SOP_RET:
		BSVM_PopFrame(ctx);
		if(ctx->flowstackpos<0)
		{
			rs=2;
			break;
		}
		break;

	case BSVM_SOP_CALL_CF:
		BSVM_CallObj(ctx, ctx->self, ctx->func);
		break;
	case BSVM_SOP_TAILCALL_CF:
		BSVM_TailCallObj(ctx, ctx->self, ctx->func);
		if(ctx->flowstackpos<0)
		{
			rs=2;
			break;
		}
		break;

	case BSVM_SOP_CAR:
		t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCar(t)); break;
	case BSVM_SOP_CDR:
		t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCdr(t)); break;
	case BSVM_SOP_CAAR:
		t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCaar(t)); break;
	case BSVM_SOP_CDAR:
		t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCdar(t)); break;
	case BSVM_SOP_CADR:
		t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCadr(t)); break;
	case BSVM_SOP_CDDR:
		t=BSVM_Pop(ctx); BSVM_Push(ctx, dytfCddr(t)); break;

	case BSVM_SOP_CONS:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfCons(u, v)); break;

	case BSVM_SOP_CLOSE:
		t=BSVM_Pop(ctx);
		t=BSVM_Close(ctx, t);
		BSVM_Push(ctx, t);
		break;

	case BSVM_SOP_DUP:
//		BSVM_Push(ctx, ctx->stack[ctx->stackpos-1]);
		u=BSVM_PopF(ctx);
		v=dytfCopyValue(u);
		BSVM_PushF(ctx, u);
		BSVM_PushF(ctx, v);
		break;

	case BSVM_SOP_DUP_F:
		BSVM_PushF(ctx, ctx->stack[ctx->stackpos-1]);
		break;
	case BSVM_SOP_EXCH:
		u=BSVM_PopF(ctx); v=BSVM_PopF(ctx);
		BSVM_PushF(ctx, u); BSVM_PushF(ctx, v);
		break;
	case BSVM_SOP_INDEX:
		i=BSVM_ReadIDX(ctx);
		t=ctx->stack[ctx->stackpos-(i+1)];
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_RINDEX:
		i=BSVM_ReadIDX(ctx);
		j=ctx->markstack[ctx->markstackpos-1];
		t=ctx->stack[i+j];
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_SETINDEX:
		t=BSVM_Pop(ctx);
		i=BSVM_ReadIDX(ctx);
		ctx->stack[ctx->stackpos-(i+1)]=t;
		break;
	case BSVM_SOP_SETRINDEX:
		t=BSVM_Pop(ctx);
		i=BSVM_ReadIDX(ctx);
		j=ctx->markstack[ctx->markstackpos-1];
		ctx->stack[i+j]=t;
		break;
	case BSVM_SOP_ROLL_I:
		i=BSVM_ReadIDX(ctx);
		j=BSVM_ReadSIDX(ctx);
		if(!j)break;
		if(j>0)
		{
			while(j>0)
			{
				t=ctx->stack[ctx->stackpos-1];
				for(k=0; k<i; k++)
				{
					v=ctx->stack[ctx->stackpos-k-2];
					ctx->stack[ctx->stackpos-k-1]=v;
				}
				ctx->stack[ctx->stackpos-i]=t;
				j--;
			}
		}else
		{
			j=-j;
			while(j>0)
			{
				t=ctx->stack[ctx->stackpos-i];
				for(k=0; k<i; k++)
				{
					v=ctx->stack[ctx->stackpos-i+k+1];
					ctx->stack[ctx->stackpos-i+k]=v;
				}
				ctx->stack[ctx->stackpos-1]=t;
				j--;
			}
		}
//		i=BSVM_ReadIDX(ctx);
//		t=ctx->stack[ctx->stackpos-(i+1)];
//		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_COPY_I:
//		k=*ctx->ip++;
		k=BSVM_ReadIDX(ctx);
		for(i=0; i<k; i++)
		{
			t=ctx->stack[ctx->stackpos-k+i];
			v=dytfCopyValue(t);
			ctx->stack[ctx->stackpos+i]=v;
		}
		ctx->stackpos+=k;
		break;

#if 0
	case BSVM_SOP_DUP_R:
		i=*ctx->ip++;
		t=BSVM_Pop(ctx);
		BSVM_Push(ctx, t);
		bsSet(ctx->reg[i], t);
		break;
	case BSVM_SOP_DUP_RF:
		i=*ctx->ip++;
		t=BSVM_PopF(ctx);
		BSVM_PushF(ctx, t);
		ctx->reg[i]=t;
		break;
	case BSVM_SOP_PUSH_R:
		i=*ctx->ip++;
		BSVM_Push(ctx, ctx->reg[i]);
		break;
	case BSVM_SOP_PUSH_RF:
		i=*ctx->ip++;
		BSVM_PushF(ctx, ctx->reg[i]);
		break;
	case BSVM_SOP_POP_R:
		i=*ctx->ip++;
		t=BSVM_PopF(ctx);
		ctx->reg[i]=t;
		break;
#endif

	case BSVM_SOP_UNARYOP:
		i=*ctx->ip++;
		t=BSVM_Pop(ctx);
		t=BSVM_Unary(ctx, i, t);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_BINARYOP:
		i=*ctx->ip++;
		v=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		t=BSVM_Binary(ctx, i, u, v);
		BSVM_Push(ctx, t);
		break;

	case BSVM_SOP_ADD:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfAdd(u, v));
		break;
	case BSVM_SOP_SUB:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfSub(u, v));
		break;
	case BSVM_SOP_MUL:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfMul(u, v));
		break;
	case BSVM_SOP_DIV:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfDiv(u, v));
		break;
	case BSVM_SOP_MOD:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfMod(u, v));
		break;
	case BSVM_SOP_AND:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfAnd(u, v));
		break;
	case BSVM_SOP_OR:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfOr(u, v));
		break;
	case BSVM_SOP_XOR:
		v=BSVM_Pop(ctx); u=BSVM_Pop(ctx);
		BSVM_Push(ctx, dytfXor(u, v));
		break;

	case BSVM_SOP_JMP:
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
		ctx->ip+=i;
		break;
	case BSVM_SOP_JMP_TRUE:
		j=BSVM_PopBool(ctx);
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
		if(j)ctx->ip+=i;
		break;
	case BSVM_SOP_JMP_FALSE:
		j=BSVM_PopBool(ctx);
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
		if(!j)ctx->ip+=i;
		break;

	case BSVM_SOP_JMP_COND_U:
		j=*ctx->ip++;
		j=BSVM_CondUnary(ctx, j, BSVM_Pop(ctx));
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
		if(j)ctx->ip+=i;
		break;
	case BSVM_SOP_JMP_COND_B:
		v=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		j=*ctx->ip++;
		j=BSVM_CondBinary(ctx, j, u, v);
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
		if(j)ctx->ip+=i;
		break;

	case BSVM_SOP_LDJMP:
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
		t=dytfPtrF(ctx->ip+i);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_JMPIA:
		t=BSVM_Pop(ctx);
		if(dytftypep(t, "svm_blk_bytecode_t"))
		{
			ctx->ip=(byte *)dytfPtrvF(t);
		}else
		{
			dyDPrintf("BSVM_SOP_JMPIA: Invalid Jump\n");
		}
		break;

	case BSVM_SOP_LIST:
		t=BSVM_PopList2(ctx);
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_DICT:
		t=BSVM_PopList(ctx);
		u=dytfWrap(dyObject());
		v=t;
		while(dytfConsP(v))
		{
			if(dykeywordp(dytfUnwrap(dytfCar(v))))
				dySetDelegate(dytfUnwrap(u), dytfPtrvF(dytfCar(v)),
					dytfUnwrap(dytfCadr(v)));
				else dyBind(dytfUnwrap(u), dytfPtrvF(dytfCar(v)),
					dytfUnwrap(dytfCadr(v)));
			v=dytfCddr(v);
		}
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_ATTR:
		t=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		v=dytfWrap(dyattr(dytfUnwrap(t), dytfUnwrap(u)));
		BSVM_Push(ctx, v);
		break;
	case BSVM_SOP_VECTOR:
		t=BSVM_PopList(ctx);
//		u=dyListToArray(t);
		u=BSVM_ListToVector(t);
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_COMPLEX_I:
		t=BSVM_Pop(ctx);
		t=dytfWrap(dycomplex2(0, dytfRealv(t)));
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_MATRIX:
		t=BSVM_PopList(ctx);
//		u=BSVM_TYMat_List2FMat(t);
		u=DYTF_UNDEFINED;
		BSVM_Push(ctx, u);
		break;
	case BSVM_SOP_COMPLEX:
		t=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx);
		t=dytfWrap(dycomplex2(dytfRealv(u), dytfRealv(t)));
		BSVM_Push(ctx, t);
		break;
	case BSVM_SOP_QUAT:
		w=BSVM_Pop(ctx); v=BSVM_Pop(ctx);
		u=BSVM_Pop(ctx); t=BSVM_Pop(ctx);
		t=dytfWrap(dyquat(
			dytfRealv(t), dytfRealv(u),
			dytfRealv(v), dytfRealv(w)));
		BSVM_Push(ctx, t);
		break;

	case BSVM_SOP_ARRAY:
		t=BSVM_PopList(ctx);
		u=dytfWrap(dyListToArray(dytfUnwrap(t)));
		BSVM_Push(ctx, u);
		break;

	case BSVM_SOP_ARRAY_S:
		t=BSVM_ReadID(ctx);
		u=BSVM_PopList(ctx);
		v=BSVM_OpNewArrayLst(ctx, t, u);
		BSVM_Push(ctx, v);
		break;

	case BSVM_SOP_EXCH_YXZ:
		BSVM_Pop3(ctx, &t, &u, &v);
		BSVM_Push3(ctx, u, t, v);
		break;
	case BSVM_SOP_EXCH_YZX:
		BSVM_Pop3(ctx, &t, &u, &v);
		BSVM_Push3(ctx, u, v, t);
		break;
	case BSVM_SOP_EXCH_ZXY:
		BSVM_Pop3(ctx, &t, &u, &v);
		BSVM_Push3(ctx, v, t, u);
		break;
	case BSVM_SOP_EXCH_ZYX:
		BSVM_Pop3(ctx, &t, &u, &v);
		BSVM_Push3(ctx, v, u, t);
		break;

	case BSVM_SOP_EXCH_WXYZ:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, w,x,y,z); break;
	case BSVM_SOP_EXCH_WXZY:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, w,x,z,y); break;
	case BSVM_SOP_EXCH_WYXZ:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, w,y,x,z); break;
	case BSVM_SOP_EXCH_WYZX:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, w,y,z,x); break;
	case BSVM_SOP_EXCH_WZXY:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, w,z,x,y); break;
	case BSVM_SOP_EXCH_WZYX:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, w,z,y,x); break;

	case BSVM_SOP_EXCH_XWYZ:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, x,w,y,z); break;
	case BSVM_SOP_EXCH_XWZY:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, x,w,z,y); break;
	case BSVM_SOP_EXCH_XYWZ:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, x,y,w,z); break;
	case BSVM_SOP_EXCH_XYZW:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, x,y,z,w); break;
	case BSVM_SOP_EXCH_XZWY:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, x,z,w,y); break;
	case BSVM_SOP_EXCH_XZYW:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, x,z,y,w); break;

	case BSVM_SOP_EXCH_YWXZ:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, y,w,x,z); break;
	case BSVM_SOP_EXCH_YWZX:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, y,w,z,x); break;
	case BSVM_SOP_EXCH_YXWZ:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, y,x,w,z); break;
	case BSVM_SOP_EXCH_YXZW:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, y,x,z,w); break;
	case BSVM_SOP_EXCH_YZWX:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, y,z,w,x); break;
	case BSVM_SOP_EXCH_YZXW:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, y,z,x,w); break;

	case BSVM_SOP_EXCH_ZWXY:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, z,w,x,y); break;
	case BSVM_SOP_EXCH_ZWYX:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, z,w,y,x); break;
	case BSVM_SOP_EXCH_ZXWY:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, z,x,w,y); break;
	case BSVM_SOP_EXCH_ZXYW:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, z,x,y,w); break;
	case BSVM_SOP_EXCH_ZYWX:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, z,y,w,x); break;
	case BSVM_SOP_EXCH_ZYXW:
		BSVM_Pop4(ctx, &w, &x, &y, &z); BSVM_Push4(ctx, z,y,x,w); break;


#if 1
	case BSVM_SOP_INC_FN: i=ctx->stackpos-1;
		ctx->stack[i]=dytfInt(dytfIntv(ctx->stack[i])+1); break;
	case BSVM_SOP_DEC_FN: i=ctx->stackpos-1;
		ctx->stack[i]=dytfInt(dytfIntv(ctx->stack[i])-1); break;
	case BSVM_SOP_INC2_FN: i=ctx->stackpos-1;
		ctx->stack[i]=dytfInt(dytfIntv(ctx->stack[i])+2); break;
	case BSVM_SOP_DEC2_FN: i=ctx->stackpos-1;
		ctx->stack[i]=dytfInt(dytfIntv(ctx->stack[i])-2); break;

	case BSVM_SOP_ADD_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)+dytfIntv(v));
		break;
	case BSVM_SOP_SUB_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)-dytfIntv(v));
		break;
	case BSVM_SOP_MUL_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)*dytfIntv(v));
		break;
	case BSVM_SOP_DIV_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=
			dytfFloat(((float)dytfIntv(u))/dytfIntv(v));
		break;
	case BSVM_SOP_IDIV_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)/dytfIntv(v));
		break;
	case BSVM_SOP_MOD_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)%dytfIntv(v));
		break;
	case BSVM_SOP_AND_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)&dytfIntv(v));
		break;
	case BSVM_SOP_OR_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)|dytfIntv(v));
		break;
	case BSVM_SOP_XOR_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)^dytfIntv(v));
		break;
	case BSVM_SOP_CMP_L_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)<dytfIntv(v));
		break;
	case BSVM_SOP_CMP_G_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)>dytfIntv(v));
		break;
	case BSVM_SOP_CMP_LE_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)<=dytfIntv(v));
		break;
	case BSVM_SOP_CMP_GE_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)>=dytfIntv(v));
		break;
	case BSVM_SOP_CMP_E_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)==dytfIntv(v)); break;
	case BSVM_SOP_CMP_NE_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfIntv(u)!=dytfIntv(v)); break;

	case BSVM_SOP_ADD_FN_C:
		i=*ctx->ip++; j=ctx->stackpos-1;
		ctx->stack[j]=dytfInt(dytfIntv(ctx->stack[j])+i); break;
		break;
	case BSVM_SOP_SUB_FN_C:
		i=*ctx->ip++; j=ctx->stackpos-1;
		ctx->stack[j]=dytfInt(dytfIntv(ctx->stack[j])-i); break;
		break;
	case BSVM_SOP_MUL_FN_C:
		i=*ctx->ip++; j=ctx->stackpos-1;
		ctx->stack[j]=dytfInt(dytfIntv(ctx->stack[j])*i); break;
		break;
		break;

	case BSVM_SOP_SHL_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)<<dytfIntv(v));
		break;
	case BSVM_SOP_SHR_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(u)>>dytfIntv(v));
		break;
	case BSVM_SOP_EXP_FN:
		i=dytfIntv(ctx->stack[--ctx->stackpos]);
		j=dytfIntv(ctx->stack[ctx->stackpos-1]);
		k=1; while(i--)k*=j;
		ctx->stack[ctx->stackpos-1]=dytfInt(k);
		break;

	case BSVM_SOP_SHL_FN_C:
		i=*ctx->ip++;
		t=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(t)<<i);
		break;
	case BSVM_SOP_SHR_FN_C:
		i=*ctx->ip++;
		j=dytfIntv(ctx->stack[ctx->stackpos-1]);
		t=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(dytfIntv(t)>>i);
		break;
	case BSVM_SOP_EXP_FN_C:
		i=*ctx->ip++;
		j=dytfIntv(ctx->stack[ctx->stackpos-1]);
		k=1; while(i--)k*=j;
		ctx->stack[ctx->stackpos-1]=dytfInt(k);
		break;

	case BSVM_SOP_CONV_FN2FL:
		t=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfFloat(dytfIntv(t));
		break;
	case BSVM_SOP_CONV_FL2FN:
		t=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfInt(((int)dytfFloatv(t)));
		break;

	case BSVM_SOP_ADD_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatv(u)+dytfFloatv(v));
		break;
	case BSVM_SOP_SUB_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatv(u)-dytfFloatv(v));
		break;
	case BSVM_SOP_MUL_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatv(u)*dytfFloatv(v));
		break;
	case BSVM_SOP_DIV_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfFloat(dytfFloatv(u)/dytfFloatv(v));
		break;
	case BSVM_SOP_CMP_L_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)<dytfFloatv(v));
		break;
	case BSVM_SOP_CMP_G_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)>dytfFloatv(v));
		break;
	case BSVM_SOP_CMP_LE_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)<=dytfFloatv(v));
		break;
	case BSVM_SOP_CMP_GE_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)>=dytfFloatv(v));
		break;
	case BSVM_SOP_CMP_E_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)==dytfFloatv(v));
		break;
	case BSVM_SOP_CMP_NE_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfBool(dytfFloatv(u)!=dytfFloatv(v));
		break;
	case BSVM_SOP_NEG_FL:
		t=ctx->stack[ctx->stackpos-1];
		ctx->stack[ctx->stackpos-1]=dytfFloat(-dytfFloatv(t));
		break;

	case BSVM_SOP_JMP_E_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(u)==dytfIntv(v));
		break;
	case BSVM_SOP_JMP_NE_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(u)!=dytfIntv(v));
		break;
	case BSVM_SOP_JMP_E_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)==dytfFloatv(v));
		break;
	case BSVM_SOP_JMP_NE_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)!=dytfFloatv(v));
		break;

#endif

	case 0xC0:
		rs=BSVM_Step(ctx, 0);
		break;
	case 0xC1:
//		BSVM_StepC1(ctx);
		rs=BSVM_Step(ctx, 256);
		break;
	case 0xC2:
		rs=BSVM_Step(ctx, 512);
		break;
	case 0xC3:
		rs=BSVM_Step(ctx, 768);
		break;
	case 0xC4: case 0xC5: case 0xC6: case 0xC7:
	case 0xC8: case 0xC9: case 0xCA: case 0xCB:
	case 0xCC: case 0xCD: case 0xCE: case 0xCF:
	case 0xD0: case 0xD1: case 0xD2: case 0xD3:
	case 0xD4: case 0xD5: case 0xD6: case 0xD7:
	case 0xD8: case 0xD9: case 0xDA: case 0xDB:
	case 0xDC: case 0xDD: case 0xDE: case 0xDF:
	case 0xE0: case 0xE1: case 0xE2: case 0xE3:
	case 0xE4: case 0xE5: case 0xE6: case 0xE7:
	case 0xE8: case 0xE9: case 0xEA: case 0xEB:
	case 0xEC: case 0xED: case 0xEE: case 0xEF:
		rs=BSVM_Step(ctx, (i-192)*256);
		break;
	case 0xF0: case 0xF1: case 0xF2: case 0xF3:
	case 0xF4: case 0xF5: case 0xF6: case 0xF7:
	case 0xF8: case 0xF9: case 0xFA: case 0xFB:
		j=((i-240)*256)+(*ctx->ip++);
		rs=BSVM_Step(ctx, j*256);
		break;
	case 0xFC: case 0xFD: case 0xFE:
		j=((i-252)*256)+(*ctx->ip++);
		j=(j*256)+(*ctx->ip++);
		rs=BSVM_Step(ctx, j*256);
		break;
	case 0xFF:
		j=*ctx->ip++;
		j=(j*256)+(*ctx->ip++);
		j=(j*256)+(*ctx->ip++);
		rs=BSVM_Step(ctx, j*256);
		break;

#if 1
	case BSVM_SOP_JMP_L_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(u)<dytfIntv(v));
		break;
	case BSVM_SOP_JMP_G_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(u)>dytfIntv(v));
		break;
	case BSVM_SOP_JMP_LE_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(u)<=dytfIntv(v));
		break;
	case BSVM_SOP_JMP_GE_FN:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(u)>=dytfIntv(v));
		break;

	case BSVM_SOP_JMP_L_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)<dytfFloatv(v));
		break;
	case BSVM_SOP_JMP_G_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)>dytfFloatv(v));
		break;
	case BSVM_SOP_JMP_LE_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)<=dytfFloatv(v));
		break;
	case BSVM_SOP_JMP_GE_FL:
		v=ctx->stack[--ctx->stackpos]; u=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfFloatv(u)>=dytfFloatv(v));
		break;

	case BSVM_SOP_JMP_E_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
//		u=BSVM_LookupIdx(ctx, *ctx->ip++);
//		v=BSVM_LookupIdx(ctx, *ctx->ip++);
		BSVM_StepJmpBool(ctx, dytfIntv(u)==dytfIntv(v));
		break;
	case BSVM_SOP_JMP_NE_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(u)!=dytfIntv(v));
		break;
	case BSVM_SOP_JMP_L_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(u)<dytfIntv(v));
		break;
	case BSVM_SOP_JMP_G_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(u)>dytfIntv(v));
		break;
	case BSVM_SOP_JMP_LE_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(u)<=dytfIntv(v));
		break;
	case BSVM_SOP_JMP_GE_LFN:
		u=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		v=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(u)>=dytfIntv(v));
		break;

	case BSVM_SOP_JMP_E_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, !dytfIntv(t));
		break;
	case BSVM_SOP_JMP_NE_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(t));
		break;
	case BSVM_SOP_JMP_L_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(t)<0);
		break;
	case BSVM_SOP_JMP_G_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(t)>0);
		break;
	case BSVM_SOP_JMP_LE_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(t)<=0);
		break;
	case BSVM_SOP_JMP_GE_ZFN:
		t=ctx->stack[--ctx->stackpos];
		BSVM_StepJmpBool(ctx, dytfIntv(t)>=0);
		break;

	case BSVM_SOP_JMP_E_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, !dytfIntv(t));
		break;
	case BSVM_SOP_JMP_NE_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(t));
		break;
	case BSVM_SOP_JMP_L_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(t)<0);
		break;
	case BSVM_SOP_JMP_G_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(t)>0);
		break;
	case BSVM_SOP_JMP_LE_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(t)<=0);
		break;
	case BSVM_SOP_JMP_GE_LZFN:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1];
		BSVM_StepJmpBool(ctx, dytfIntv(t)>=0);
		break;


	case BSVM_SOP_JMP_E_FNC:
		t=ctx->stack[--ctx->stackpos]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)==i);
		break;
	case BSVM_SOP_JMP_NE_FNC:
		t=ctx->stack[--ctx->stackpos]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)!=i);
		break;
	case BSVM_SOP_JMP_L_FNC:
		t=ctx->stack[--ctx->stackpos]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)<i);
		break;
	case BSVM_SOP_JMP_G_FNC:
		t=ctx->stack[--ctx->stackpos]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)>i);
		break;
	case BSVM_SOP_JMP_LE_FNC:
		t=ctx->stack[--ctx->stackpos]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)<=i);
		break;
	case BSVM_SOP_JMP_GE_FNC:
		t=ctx->stack[--ctx->stackpos]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)>=i);
		break;

	case BSVM_SOP_JMP_E_LFNC:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)==i);
		break;
	case BSVM_SOP_JMP_NE_LFNC:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)!=i);
		break;
	case BSVM_SOP_JMP_L_LFNC:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)<i);
		break;
	case BSVM_SOP_JMP_G_LFNC:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)>i);
		break;
	case BSVM_SOP_JMP_LE_LFNC:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)<=i);
		break;
	case BSVM_SOP_JMP_GE_LFNC:
		t=ctx->stack[ctx->eargs-(*ctx->ip++)-1]; i=*ctx->ip++;
		BSVM_StepJmpBool(ctx, dytfIntv(t)>=i);
		break;
#endif

	default:
		break;
	}

	return(rs);
}

int BSVM_RunReadOp(BSVM_SVMState *ctx)
{
	int i;
	i=*ctx->ip++;
	if(i>=192)
		{ i=(i-192)*256+(*ctx->ip++); }
	return(i);
}

int BSVM_RunSeekPastOp(BSVM_SVMState *ctx, int op)
{
	int i;

	//if we hit a begin_catch as a normal instruction
	//then it should not be executed
	//so, seek to the next end_catch instruction (to skip catch block)
	while(ctx->ip<ctx->ipe)
	{
		i=BSVM_RunReadOp(ctx);
		if(i==op)return(1);
		BSVM_SkipOpcodeArgs(ctx, i);
	}
	return(0);
}

int BSVM_CheckThrownObjType(BSVM_SVMState *ctx, dyt obj, dyt ty)
{
	if(dyllCheckCallSigValue(dysymbolv(ty), obj))
		return(1);
	return(0);
}

int BSVM_RunHandleThrown(BSVM_SVMState *ctx)
{
	dytf t;
	int rs;
	int i;

	rs=-1;

#if 1
	while(1)
	{
		i=BSVM_RunSeekPastOp(ctx, BSVM_SOP_END_TRY);
		if(i==BSVM_RETS_THROW)
		{
			while(1)
			{
				i=BSVM_RunReadOp(ctx);
				if(i==BSVM_SOP_BEGIN_CATCH)
				{
					t=BSVM_ReadID(ctx);
					//check if exception matches...
					if(BSVM_CheckThrownObjType(ctx,
						dytfUnwrap(ctx->thrown_obj), dytfUnwrap(t)))
					{
						//push exception
						BSVM_Push(ctx, ctx->thrown_obj);
						//exit exception state
						ctx->thrown_obj2=DYTF_NULL;
						ctx->thrown_obj=DYTF_NULL;
						//continue execution
						rs=BSVM_RETS_NONE;
						break;
					}
					i=BSVM_RunSeekPastOp(ctx, BSVM_SOP_END_CATCH);
					continue;
				}
				if(i==BSVM_SOP_BEGIN_CATCH_FINAL)
				{
					//suspend throw
					ctx->thrown_obj2=ctx->thrown_obj;
					ctx->thrown_obj=DYTF_NULL;
					//continue execution
					rs=BSVM_RETS_NONE;
					break;
				}
				break;
			}
			
			//continue...
			if(rs==BSVM_RETS_NONE)
				break;
		}

		//try to unwind frame
		BSVM_PopFrame(ctx);
		if(ctx->flowstackpos<0)
			{ rs=BSVM_RETS_THROW; break; }
	}
#endif

	return(rs);
}

int BSVM_Run(dyt ct)
{
	BSVM_SVMState *ctx;
	BSVM_ThreadOp *thop;
	int rs, i, j, t;

	ctx=(BSVM_SVMState *)ct;
	rs=0; ctx->rs=0;

//	t=clock();
	i=0;
	while(!rs)
	{
#ifdef BSVM_JIT
		if(ctx->thunk)
		{
			rs=ctx->thunk(ctx);
			continue;
		}
#endif

//		dyDPrintf("%p %p\n", ctx, ctx->ip);
		i++;
#ifdef BSVM_THREADCODE
#if 0
		thop=ctx->thop;
		if(thop)
		{
			while(thop && !ctx->rs)
				{ thop=thop->fcn(ctx, thop); }
			ctx->thop=thop;
			rs=ctx->rs;
		}else
		{
			rs=BSVM_Thread_Step(ctx);
		}
#endif
		rs=BSVM_Thread_Step(ctx);
//		rs=BSVM_Step(ctx, 0);
#else
		rs=BSVM_Step(ctx, 0);
#endif

		if(rs)
		{
			switch(rs)
			{
			case BSVM_RETS_BOUNDSCHK:
				BSVM_OpThrowName(ctx, "BoundsCheckException");
				ctx->rs=BSVM_RETS_THROW; rs=ctx->rs;
				break;
			case BSVM_RETS_RIGHTSCHK:
				BSVM_OpThrowName(ctx, "RightsCheckException");
				ctx->rs=BSVM_RETS_THROW; rs=ctx->rs;
				break;
			case BSVM_RETS_NULLCHK:
				BSVM_OpThrowName(ctx, "NullPointerException");
				ctx->rs=BSVM_RETS_THROW; rs=ctx->rs;
				break;
			case BSVM_RETS_TYPECHK:
				BSVM_OpThrowName(ctx, "CastFailException");
				ctx->rs=BSVM_RETS_THROW; rs=ctx->rs;
				break;
			}

			if(rs==BSVM_RETS_THROW)
			{
				dyDPrintf("Exception: %s:%d: %s\n",
					ctx->dbg_fn, ctx->dbg_ln,
					dyToString(dytfUnwrap(ctx->thrown_obj)));

				//if something was thrown
				//perform exception-handling magic
				if(!dytfNullP(ctx->thrown_obj))
				{
					rs=BSVM_RunHandleThrown(ctx);
					continue;
				}

				rs=BSVM_RETS_NONE; continue;
			}
		}
	}

//	j=clock()-t;
//	if(j)printf("MIPS %g\n", (i/(j/(float)CLOCKS_PER_SEC))/1000000.0);

	if(rs==BSVM_RETS_ASSERT)
	{
		dyDPrintf("Assertion Failed: %s:%d\n", ctx->dbg_fn, ctx->dbg_ln);
	}

	if(rs==BSVM_RETS_THROW)
	{
		dyDPrintf("Uncaught Exception: %s:%d: %s\n",
			ctx->dbg_fn, ctx->dbg_ln,
			dyToString(dytfUnwrap(ctx->thrown_obj)));
	}

	bsvm_opcnt+=i;

	return(rs);
}

int BSVM_RunCnt(BSVM_SVMState *ctx, int cnt)
{
	int rs, i, j, t;

	rs=0;

//	t=clock();
	i=0;
	while(!rs && (cnt>0))
	{
#ifdef BSVM_JIT
		if(ctx->thunk)
		{
			rs=ctx->thunk(ctx);
			continue;
		}
#endif

//		dyDPrintf("%p %p\n", ctx, ctx->ip);
		i++; cnt--;
		rs=BSVM_Step(ctx, 0);
	}

//	j=clock()-t;
//	if(j)printf("MIPS %g\n", (i/(j/(float)CLOCKS_PER_SEC))/1000000.0);

	bsvm_opcnt+=i;

	return(rs);
}



BGBSVM_API dyt bsvmCallName(char *name, dyt args)
{
	return(BSVM_CallFuncObj(NULL, dysymbol(name), args));
}

BGBSVM_API dyt bsvmCallName0(char *name)
{
	return(BSVM_CallFuncObj(NULL, dysymbol(name), NULL));
}

BGBSVM_API dyt bsvmCallName1(char *name, dyt a)
	{ return(BSVM_CallFuncObj(NULL, dysymbol(name), dylist1(a))); }
BGBSVM_API dyt bsvmCallName2(char *name, dyt a, dyt b)
	{ return(BSVM_CallFuncObj(NULL, dysymbol(name), dylist2(a,b))); }
BGBSVM_API dyt bsvmCallName3(char *name, dyt a, dyt b, dyt c)
	{ return(BSVM_CallFuncObj(NULL, dysymbol(name), dylist3(a,b,c))); }
BGBSVM_API dyt bsvmCallName4(char *name, dyt a, dyt b, dyt c, dyt d)
	{ return(BSVM_CallFuncObj(NULL, dysymbol(name), dylist4(a,b,c,d))); }

BGBSVM_API dyt bsvmCachedCallSigVr(dyt *rhdl,
	char *pkg, char *name, char *sig, va_list lst)
{
	dyt t, l, o, fcn;

	if(pkg && (*pkg))
	{
		BSVM_LoadQNameScriptCtx(NULL, pkg, NULL);
		o=dytfUnwrap(BSVM_GetTop(pkg));
	}else
	{
		o=NULL;
	}
	l=dyllUnpackArgsSigList(sig, lst);

	//name=(char *)dysymbol(name);

	if(o)
	{
		fcn=dyGet(o, name);
		if((fcn==NULL) || (fcn==UNDEFINED))
			return(UNDEFINED);
	}else
	{
		fcn=dyGet(dytfUnwrap(bsvm_toplevel), name);
		if((fcn==NULL) || (fcn==UNDEFINED))
			return(UNDEFINED);
	}

	if(dytypep(fcn, "_raw_func_t"))
	{
		dyDPrintf("bsvmCachedCallSigVr: Recursion Check Fail\n");
		return(UNDEFINED);
	}

	t=BSVM_CallFuncObj(o, fcn, l);
//	t=BSVM_CallFuncObj(o, dysymbol(name), l);
	return(t);
}

BGBSVM_API void bsvmCachedCallSigVv(dyt *rhdl,
	char *pkg, char *name, char *sig, va_list lst)
{
	bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
}

BGBSVM_API s32 bsvmCachedCallSigVi(dyt *rhdl,
	char *pkg, char *name, char *sig, va_list lst)
{
	dyt t;
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	return(dyintv(t));
}

BGBSVM_API void bsvmCachedCallSigv(dyt *rhdl,
	char *pkg, char *name, char *sig, ...)
{
	va_list lst;
	dyt t;
	va_start(lst, sig);
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	va_end(lst);
}

BGBSVM_API s32 bsvmCachedCallSigi(dyt *rhdl,
	char *pkg, char *name, char *sig, ...)
{
	va_list lst;
	dyt t;
	va_start(lst, sig);
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	va_end(lst);
	return(dyintv(t));
}

BGBSVM_API s64 bsvmCachedCallSigl(dyt *rhdl,
	char *pkg, char *name, char *sig, ...)
{
	va_list lst;
	dyt t;
	va_start(lst, sig);
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	va_end(lst);
	return(dylongv(t));
}

BGBSVM_API f32 bsvmCachedCallSigf(dyt *rhdl,
	char *pkg, char *name, char *sig, ...)
{
	va_list lst;
	dyt t;
	va_start(lst, sig);
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	va_end(lst);
	return(dyfloatv(t));
}

BGBSVM_API f64 bsvmCachedCallSigd(dyt *rhdl,
	char *pkg, char *name, char *sig, ...)
{
	va_list lst;
	dyt t;
	va_start(lst, sig);
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	va_end(lst);
	return(dylongv(t));
}

BGBSVM_API void *bsvmCachedCallSigp(dyt *rhdl,
	char *pkg, char *name, char *sig, ...)
{
	va_list lst;
	dyt t;
	va_start(lst, sig);
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	va_end(lst);
	return(t);
}

BGBSVM_API dyt bsvmCachedCallSigr(dyt *rhdl,
	char *pkg, char *name, char *sig, ...)
{
	va_list lst;
	dyt t;
	va_start(lst, sig);
	t=bsvmCachedCallSigVr(rhdl, pkg, name, sig, lst);
	va_end(lst);
	return(t);
}
