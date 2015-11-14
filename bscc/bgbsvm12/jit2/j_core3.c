#include <bgbsvm.h>
#include <bgbasm.h>

#include <stdarg.h>


//#define JIT_OFFS(n)	((&BSVM_SVMState.n)-(&BSVM_SVMState))

extern char *bsvm_ops_strs[];
extern char *bsvm_ops_args[];

extern dyt bsvm_svm_toplevel;

BSVM_SVMState bsvm_jit_svmstate;	//example VM state
#define JIT_OFFS(n)	(((byte *)(&(bsvm_jit_svmstate.n)))-((byte *)(&bsvm_jit_svmstate)))


dyt bsvm_jit_temp[16];		//temp "registers"
bsjContext bsvm_jit_ctx;
int bsvm_jit_seq=1;


dyt BSVM_J2Core_PopDict(BSVM_SVMState *ctx)
{
	dyt t, u, v;

	t=BSVM_SVM_PopList(ctx);
	u=BSVM_SObj_New();
	v=t;
	while(dyconsp(v))
	{
		BSVM_SObj_BindSlot(u, dycar(v), dycadr(v));
		v=CDDR(v);
	}
	BSVM_GC_CheckRef(t);
	return(u);
}

dyt BSVM_J2Core_PopVector(BSVM_SVMState *ctx)
{
	dyt t, u, v;

	t=BSVM_SVM_PopList(ctx);
	u=BSVM_TYVec_List2FVec(t);
	BSVM_GC_CheckRef(t);
	return(u);
}

dyt BSVM_J2Core_PopMatrix(BSVM_SVMState *ctx)
{
	dyt t, u, v;

	t=BSVM_SVM_PopList(ctx);
	u=BSVM_TYMat_List2FMat(t);
	BSVM_GC_CheckRef(t);
	return(u);
}

dyt BSVM_J2Core_AddFl(dyt a, dyt b) { return(dyflonum(dyfloatv(a)+dyfloatv(b))); }
dyt BSVM_J2Core_SubFl(dyt a, dyt b) { return(dyflonum(dyfloatv(a)+dyfloatv(b))); }
dyt BSVM_J2Core_MulFl(dyt a, dyt b) { return(dyflonum(dyfloatv(a)+dyfloatv(b))); }
dyt BSVM_J2Core_DivFl(dyt a, dyt b) { return(dyflonum(dyfloatv(a)+dyfloatv(b))); }
dyt BSVM_J2Core_NegFl(dyt a) { return(dyflonum(-dyfloatv(a))); }

int BSVM_J2Core_CmpEFl(dyt a, dyt b) { return(dyfloatv(a)==dyfloatv(b)); }
int BSVM_J2Core_CmpNeFl(dyt a, dyt b) { return(dyfloatv(a)!=dyfloatv(b)); }
int BSVM_J2Core_CmpLFl(dyt a, dyt b) { return(dyfloatv(a)<dyfloatv(b)); }
int BSVM_J2Core_CmpGFl(dyt a, dyt b) { return(dyfloatv(a)>dyfloatv(b)); }
int BSVM_J2Core_CmpLeFl(dyt a, dyt b) { return(dyfloatv(a)<=dyfloatv(b)); }
int BSVM_J2Core_CmpGeFl(dyt a, dyt b) { return(dyfloatv(a)>=dyfloatv(b)); }

dyt BSVM_J2Core_DivFn(dyt a, dyt b) { return(dyflonum(dyfloatv(a)/dyfloatv(b))); }


int BSVM_J2Core_RetCleanup(BSVM_SVMState *ctx)
{
	dyt t;

	BSVM_SVM_PopFrame(ctx);
	if(ctx->flowstackpos<0)
		return(2);
	return(0);
}

dyt _cdecl BSVM_J2Core_Lookup(BSVM_SVMState *ctx, dyt key)
{
	*(int *)-1=-1;
}


void BSVM_J2Core_Init()
{
	static int init=0;
	int i, j, k, l;

	if(init)return;
	init=1;

	bsvm_jit_ctx=malloc(sizeof(BSVM_JitContext));
	memset(bsvm_jit_ctx, 0, sizeof(BSVM_JitContext));

#if 0
	BASM_RegisterPtr("IncRef", &BSVM_GC_IncRef);
	BASM_RegisterPtr("DecRef", &BSVM_GC_DecRef);
	BASM_RegisterPtr("SafeDecRef", &BSVM_GC_SafeDecRef);
	BASM_RegisterPtr("CheckRef", &BSVM_GC_CheckRef);

	BASM_RegisterPtr("Lookup", &BSVM_SVM_Lookup);
	BASM_RegisterPtr("Assign", &BSVM_SVM_Assign);
	BASM_RegisterPtr("Bind", &BSVM_SVM_Bind);
	BASM_RegisterPtr("LexBind", &BSVM_SVM_LexBind);

	BASM_RegisterPtr("LoadIndex", &BSVM_SVM_LoadIndex);
	BASM_RegisterPtr("StoreIndex", &BSVM_SVM_StoreIndex);

	BASM_RegisterPtr("LookupIdx", &BSVM_SVM_LookupIdx);
	BASM_RegisterPtr("AssignIdx", &BSVM_SVM_AssignIdx);
	BASM_RegisterPtr("AssignIdxF", &BSVM_SVM_AssignIdxF);

	BASM_RegisterPtr("CallObj", &BSVM_SVM_CallObj);
	BASM_RegisterPtr("TailCallObj", &BSVM_SVM_TailCallObj);
	BASM_RegisterPtr("MethodCall", &BSVM_SVM_MethodCall);
	BASM_RegisterPtr("MethodTailCall", &BSVM_SVM_MethodTailCall);

	BASM_RegisterPtr("Unary", &BSVM_SVM_Unary);
	BASM_RegisterPtr("Binary", &BSVM_SVM_Binary);
	BASM_RegisterPtr("CondUnary", &BSVM_Opr_CondUnary);
	BASM_RegisterPtr("CondBinary", &BSVM_Opr_CondBinary);

	BASM_RegisterPtr("GetCar", &BSVM_TY_GetCar);
	BASM_RegisterPtr("GetCdr", &BSVM_TY_GetCdr);
	BASM_RegisterPtr("GetCaar", &BSVM_TY_GetCaar);
	BASM_RegisterPtr("GetCdar", &BSVM_TY_GetCdar);
	BASM_RegisterPtr("GetCadr", &BSVM_TY_GetCadr);
	BASM_RegisterPtr("GetCddr", &BSVM_TY_GetCddr);

	BASM_RegisterPtr("PopList2", &BSVM_SVM_PopList2);

	BASM_RegisterPtr("PopDict", &BSVM_J2Core_PopDict);
	BASM_RegisterPtr("PopVector", &BSVM_J2Core_PopVector);
	BASM_RegisterPtr("PopMatrix", &BSVM_J2Core_PopMatrix);

	BASM_RegisterPtr("AddFl", &BSVM_J2Core_AddFl);
	BASM_RegisterPtr("SubFl", &BSVM_J2Core_SubFl);
	BASM_RegisterPtr("MulFl", &BSVM_J2Core_MulFl);
	BASM_RegisterPtr("DivFl", &BSVM_J2Core_DivFl);
	BASM_RegisterPtr("NegFl", &BSVM_J2Core_NegFl);
	BASM_RegisterPtr("DivFn", &BSVM_J2Core_DivFn);

	BASM_RegisterPtr("CmpEFl", &BSVM_J2Core_CmpEFl);
	BASM_RegisterPtr("CmpNeFl", &BSVM_J2Core_CmpNeFl);
	BASM_RegisterPtr("CmpLFl", &BSVM_J2Core_CmpLFl);
	BASM_RegisterPtr("CmpGFl", &BSVM_J2Core_CmpGFl);
	BASM_RegisterPtr("CmpLeFl", &BSVM_J2Core_CmpLeFl);
	BASM_RegisterPtr("CmpGeFl", &BSVM_J2Core_CmpGeFl);

	BASM_RegisterPtr("RetCleanup", &BSVM_J2Core_RetCleanup);
#endif

	BASM_Begin();

	basm_print("JIT_Base: ret;\n");

	basm_print("incref: push eax; call IncRef; pop eax; ret;\n");
	basm_print("decref: push eax; call DecRef; pop eax; ret;\n");
	basm_print("safedecref: push eax; call SafeDecRef; pop eax; ret;\n");
	basm_print("checkref: push eax; call CheckRef; pop eax; ret;\n");

	basm_print("incref2: push ecx; push eax; call IncRef; pop eax; pop ecx; ret;\n");
	basm_print("decref2: push ecx; push eax; call DecRef; pop eax; pop ecx; ret;\n");
	basm_print("safedecref2: push ecx; push eax; call SafeDecRef; pop eax; pop ecx; ret;\n");
	basm_print("checkref2: push ecx; push eax; call CheckRef; pop eax; pop ecx; ret;\n");

	BASM_End();
}

char *BSVM_J2Core_GenSym()
{
	char buf[64];
	sprintf(buf, "JIT$%d", bsvm_jit_seq++);
	return(dyllStrdup(buf));
}


#define BSVM_J2Core_CACHE

#ifdef BSVM_J2Core_CACHE

void BSVM_J2Core_PushEAX_F(bsjContext ctx)
{
//	basm_print("mov [edi+ebx*4], eax; inc_r ebx;\n");
	ctx->fl|=BSVM_JITFL_EAX;
}

void BSVM_J2Core_PushEAX(bsjContext ctx)
{
//	basm_print("call incref; mov [edi+ebx*4], eax; inc_r ebx;\n");

//	ctx->fl&=~BSVM_JITFL_ECX_EARG;
	ctx->fl|=BSVM_JITFL_EAX;
//	basm_print("call incref;\n");
	BSVM_J2Core_IncRef(ctx);
}

void BSVM_J2Core_Push(bsjContext ctx, dyt val)
{
	if(ctx->fl&BSVM_JITFL_EAX)
		basm_print("mov [edi+ebx*4], eax; inc_r ebx;\n");

	if(BSVM_HEAPP(val))
	{
//		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		basm_print("mov eax, 0x%x;\n", val);
		BSVM_J2Core_IncRef(ctx);
		ctx->fl|=BSVM_JITFL_EAX;
		return;
	}

	if(!val)
	{
		basm_print("xor eax, eax;\n");
		ctx->fl|=BSVM_JITFL_EAX;
		return;
	}

	basm_print("mov eax, 0x%x;\n", val);
	ctx->fl|=BSVM_JITFL_EAX;
}

#endif

#ifndef BSVM_J2Core_CACHE

void BSVM_J2Core_PushEAX_F(bsjContext ctx)
{
	basm_print("mov [edi+ebx*4], eax; inc_r ebx;\n");
}

void BSVM_J2Core_PushEAX(bsjContext ctx)
{
	BSVM_J2Core_IncRef(ctx);
	basm_print("mov [edi+ebx*4], eax; inc_r ebx;\n");
//	ctx->fl&=~BSVM_JITFL_ECX_EARG;
}

void BSVM_J2Core_Push(bsjContext ctx, dyt val)
{
	if(BSVM_HEAPP(val))
	{
		basm_print("mov eax, 0x%x; call incref;\n"
			"mov [edi+ebx*4], eax; inc_r ebx;\n", val);
		return;
	}

	basm_print("mov dword [edi+ebx*4], 0x%x; inc_r ebx;\n", val);
}
#endif


void BSVM_J2Core_PopEAX_F(bsjContext ctx)
{
	if(ctx->fl&BSVM_JITFL_EAX)
	{
		ctx->fl&=~BSVM_JITFL_EAX;
		return;
	}
	basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
}

void BSVM_J2Core_PopEAX(bsjContext ctx)
{
//	ctx->fl&=~BSVM_JITFL_ECX_EARG;

	if(ctx->fl&BSVM_JITFL_EAX)
	{
		ctx->fl&=~BSVM_JITFL_EAX;
		BSVM_J2Core_SafeDecRef(ctx);
		return;
	}
	basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
	BSVM_J2Core_SafeDecRef(ctx);
}

void BSVM_J2Core_PopToArg(bsjContext ctx)
{
	ctx->fl&=~BSVM_JITFL_ECX_EARG;

	if(ctx->fl&BSVM_JITFL_EAX)
	{
		ctx->fl&=~BSVM_JITFL_EAX;
		basm_print("push eax; call SafeDecRef;\n");
		return;
	}
	basm_print("dec_r ebx; push dword [edi+ebx*4]; call SafeDecRef;\n");
}

void BSVM_J2Core_PopToArgF(bsjContext ctx)
{
	if(ctx->fl&BSVM_JITFL_EAX)
	{
		ctx->fl&=~BSVM_JITFL_EAX;
		basm_print("push eax;\n");
		return;
	}
	basm_print("dec_r ebx; push dword [edi+ebx*4];\n");
}

void BSVM_J2Core_PopDestroy(bsjContext ctx)
{
//	ctx->fl&=~BSVM_JITFL_ECX_EARG;

	if(ctx->fl&BSVM_JITFL_EAX)
	{
		ctx->fl&=~BSVM_JITFL_EAX;
		BSVM_J2Core_DecRef(ctx);
		return;
	}

	basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
	BSVM_J2Core_DecRef(ctx);
}

void BSVM_J2Core_PopBool(bsjContext ctx)
{
//	ctx->fl&=~BSVM_JITFL_ECX_EARG;

	if(ctx->fl&BSVM_JITFL_EAX)
	{
		ctx->fl&=~BSVM_JITFL_EAX;
		BSVM_J2Core_DecRef(ctx);
		return;
	}

	basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
	BSVM_J2Core_DecRef(ctx);
}

void BSVM_J2Core_IncRef(bsjContext ctx)
{
}

void BSVM_J2Core_DecRef(bsjContext ctx)
{
}

void BSVM_J2Core_SafeDecRef(bsjContext ctx)
{
}


void BSVM_J2Core_FlattenStack(bsjContext ctx)
{
	ctx->fl&=~BSVM_JITFL_ECX_EARG;

	if(!(ctx->fl&BSVM_JITFL_EAX))return;

	ctx->fl&=~BSVM_JITFL_EAX;
	basm_print("mov [edi+ebx*4], eax; inc_r ebx;\n");
}

void BSVM_J2Core_AdjustStack(bsjContext ctx)
{
	BSVM_J2Core_FlattenStack(ctx);
	basm_print("mov [esi+%d], ebx;\n", JIT_OFFS(stackpos));
}

void BSVM_J2Core_Return(bsjContext ctx)
{
	BSVM_J2Core_AdjustStack(ctx);
	basm_print("push esi; call RetCleanup; pop ecx; pop ebx;\n"
		"pop edi; pop esi; pop ebp; ret;\n");
}

void BSVM_J2Core_Entry(bsjContext ctx)
{
	basm_print("push ebp; mov ebp, esp; push esi; push edi; push ebx;\n");
	basm_print("mov esi, [ebp+8]; mov edi, [esi+%d]; mov ebx, [esi+%d];\n",
		JIT_OFFS(stack), JIT_OFFS(stackpos));
	ctx->fl&=~255;
}

void BSVM_J2Core_Call(bsjContext ctx)
{
	char *s;

	s=BSVM_J2Core_GenSym(); BSVM_J2Core_PopEAX(ctx); BSVM_J2Core_AdjustStack(ctx);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	basm_print("push eax; push dword [esi+%d]; push esi;\n"
		"call CallObj; add esp, 12;\n", JIT_OFFS(self));
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_TailCall(bsjContext ctx)
{
	char *s;

	s=BSVM_J2Core_GenSym(); BSVM_J2Core_PopEAX(ctx); BSVM_J2Core_AdjustStack(ctx);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	basm_print("push eax; push dword [esi+%d]; push esi;\n"
		"call TailCallObj; add esp, 12;\n", JIT_OFFS(self));
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_CallS(bsjContext ctx, dyt t)
{
	char *s;

	s=BSVM_J2Core_GenSym();
	BSVM_J2Core_AdjustStack(ctx);
	basm_print("push dword 0x%x; push esi; call Lookup; add esp, 8;\n", t);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	basm_print("push eax; push dword [esi+%d]; push esi;\n"
		"call CallObj; add esp, 12;\n", JIT_OFFS(self));
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_TailCallS(bsjContext ctx, dyt t)
{
	char *s;

	s=BSVM_J2Core_GenSym();
	BSVM_J2Core_AdjustStack(ctx);
	basm_print("push dword 0x%x; push esi; call Lookup; add esp, 8;\n", t);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	basm_print("push eax; push dword [esi+%d]; push esi;\n"
		"call TailCallObj; add esp, 12;\n", JIT_OFFS(self));
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_MethodCall(bsjContext ctx)
{
	char *s;

	s=BSVM_J2Core_GenSym(); BSVM_J2Core_PopToArg(ctx);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	BSVM_J2Core_PopToArg(ctx); BSVM_J2Core_AdjustStack(ctx);
	basm_print("push esi; call MethodCall; add esp, 12;\n");
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_MethodTailCall(bsjContext ctx)
{
	char *s;

	s=BSVM_J2Core_GenSym(); BSVM_J2Core_PopToArg(ctx); 
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	BSVM_J2Core_PopToArg(ctx);	BSVM_J2Core_AdjustStack(ctx);
	basm_print("push esi; call MethodTailCall; add esp, 12;\n");
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_MethodCallS(bsjContext ctx, dyt t)
{
	char *s;

	s=BSVM_J2Core_GenSym(); basm_print("push dword 0x%x;\n", t);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	BSVM_J2Core_PopToArg(ctx); BSVM_J2Core_AdjustStack(ctx);
	basm_print("push esi; call MethodCall; add esp, 12;\n");
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_MethodTailCallS(bsjContext ctx, dyt t)
{
	char *s;

	s=BSVM_J2Core_GenSym(); basm_print("push dword 0x%x;\n", t);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	BSVM_J2Core_PopToArg(ctx); BSVM_J2Core_AdjustStack(ctx);
	basm_print("push esi; call MethodTailCall; add esp, 12;\n");
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_CallCF(bsjContext ctx)
{
	char *s;

	s=BSVM_J2Core_GenSym(); BSVM_J2Core_AdjustStack(ctx);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	basm_print("push dword [esi+%d];\n", JIT_OFFS(func));
	basm_print("push dword [esi+%d]; push esi;\n"
		"call CallObj; add esp, 12;\n", JIT_OFFS(self));
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}

void BSVM_J2Core_TailCallCF(bsjContext ctx)
{
	char *s;

	s=BSVM_J2Core_GenSym(); BSVM_J2Core_AdjustStack(ctx);
	basm_print("mov dword [esi+%d], %s;\n", JIT_OFFS(thunk), s);
	basm_print("push dword [esi+%d];\n", JIT_OFFS(func));
	basm_print("push dword [esi+%d]; push esi;\n"
		"call TailCallObj; add esp, 12;\n", JIT_OFFS(self));
	basm_print("xor eax, eax; pop ebx; pop edi; pop esi; pop ebp; ret;\n\n");
	basm_print("%s:\n", s); BSVM_J2Core_Entry(ctx);
}


dyt BSVM_J2Core_ReadID(bsjContext ctx)
{
	dyt t;
	int i;

	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	t=BSVM_TY_GetArrayElem2(ctx->lit, i);
	return(t);
}

int BSVM_J2Core_ReadIDX(bsjContext ctx)
{
	int i;
	i=*ctx->ip++;
	if(i&128)i=((i&127)<<8)|(*ctx->ip++);
	return(i);
}

void BSVM_J2Core_PreStep(bsjContext ctx)
{
	char buf[64];
	byte *ip, *ip1;
	char *s;
	int i, j;

	ip=ctx->ip;
	i=*ip++;
	if(i>=192)i=(i-192)*256+(*ip++);
	switch(i)
	{
	case BSVM_SOP_JMP:
	case BSVM_SOP_JMP_TRUE:		case BSVM_SOP_JMP_FALSE:

	case BSVM_SOP_JMP_E_FN:		case BSVM_SOP_JMP_NE_FN:
	case BSVM_SOP_JMP_E_FL:		case BSVM_SOP_JMP_NE_FL:
	case BSVM_SOP_JMP_L_FN:		case BSVM_SOP_JMP_G_FN:
	case BSVM_SOP_JMP_LE_FN:		case BSVM_SOP_JMP_GE_FN:
	case BSVM_SOP_JMP_L_FL:		case BSVM_SOP_JMP_G_FL:
	case BSVM_SOP_JMP_LE_FL:		case BSVM_SOP_JMP_GE_FL:

	case BSVM_SOP_JMP_E_ZFN:		case BSVM_SOP_JMP_NE_ZFN:
	case BSVM_SOP_JMP_L_ZFN:		case BSVM_SOP_JMP_G_ZFN:
	case BSVM_SOP_JMP_LE_ZFN:	case BSVM_SOP_JMP_GE_ZFN:
		i=*ip++; i|=(*ip++)<<8; i=((signed short)i); ip1=ip+i;
		for(j=0; j<ctx->n_lbl; j++)
			if(ctx->lbl[j]==ip1)
				break;
		if(j>=ctx->n_lbl)
		{
			j=ctx->n_lbl++;
			ctx->lbl[j]=ip1;
			ctx->lbln[j]=BSVM_J2Core_GenSym();
//			printf("LBL %08X %s\n", (int)ip1, ctx->lbln[j]);
		}
		break;

	case BSVM_SOP_JMP_COND_U:	case BSVM_SOP_JMP_COND_B:

	case BSVM_SOP_JMP_E_LZFN:	case BSVM_SOP_JMP_NE_LZFN:
	case BSVM_SOP_JMP_L_LZFN:	case BSVM_SOP_JMP_G_LZFN:
	case BSVM_SOP_JMP_LE_LZFN:	case BSVM_SOP_JMP_GE_LZFN:

	case BSVM_SOP_JMP_E_FNC:		case BSVM_SOP_JMP_NE_FNC:
	case BSVM_SOP_JMP_L_FNC:		case BSVM_SOP_JMP_G_FNC:
	case BSVM_SOP_JMP_LE_FNC:	case BSVM_SOP_JMP_GE_FNC:
		ip++;
		i=*ip++; i|=(*ip++)<<8; i=((signed short)i); ip1=ip+i;
		for(j=0; j<ctx->n_lbl; j++)
			if(ctx->lbl[j]==ip1)
				break;
		if(j>=ctx->n_lbl)
		{
			j=ctx->n_lbl++;
			ctx->lbl[j]=ip1;
			ctx->lbln[j]=BSVM_J2Core_GenSym();
//			printf("LBL %08X %s\n", (int)ip1, ctx->lbln[j]);
		}
		break;

	case BSVM_SOP_JMP_E_LFN:		case BSVM_SOP_JMP_NE_LFN:
	case BSVM_SOP_JMP_L_LFN:		case BSVM_SOP_JMP_G_LFN:
	case BSVM_SOP_JMP_LE_LFN:	case BSVM_SOP_JMP_GE_LFN:

	case BSVM_SOP_JMP_E_LFNC:	case BSVM_SOP_JMP_NE_LFNC:
	case BSVM_SOP_JMP_L_LFNC:	case BSVM_SOP_JMP_G_LFNC:
	case BSVM_SOP_JMP_LE_LFNC:	case BSVM_SOP_JMP_GE_LFNC:
		ip+=2;
		i=*ip++; i|=(*ip++)<<8; i=((signed short)i); ip1=ip+i;
		for(j=0; j<ctx->n_lbl; j++)
			if(ctx->lbl[j]==ip1)
				break;
		if(j>=ctx->n_lbl)
		{
			j=ctx->n_lbl++;
			ctx->lbl[j]=ip1;
			ctx->lbln[j]=BSVM_J2Core_GenSym();
//			printf("LBL %08X %s\n", (int)ip1, ctx->lbln[j]);
		}
		break;

	default:
		s=bsvm_ops_args[i];
		while(*s)
		{
			if((*s=='I') || (*s=='S') || (*s=='T') || (*s=='N'))
				{ i=*ip++; if(i&128)ip++; }
			if(*s=='A')ip+=2;
			if(*s=='C')ip++;
			if(*s=='B')ip++;
			if(*s=='W')ip+=2;
			if(*s=='b')ip++;
			s++;
		}
		break;
	}

	ctx->ip=ip;
}

char *BSVM_J2Core_GetLabel(bsjContext ctx, byte *ip)
{
	int i;
	for(i=0; i<ctx->n_lbl; i++)
		if(ctx->lbl[i]==ip)
			return(ctx->lbln[i]);
	return(NULL);
}

int BSVM_J2Core_Step(bsjContext ctx)
{
	dyt t, u, v;
	int i, j, rs;
	char *s, *s1, *s2;

#ifndef BSVM_J2Core_CACHE
	ctx->fl&=~BSVM_JITFL_ECX_EARG;
#endif

//	BSVM_J2Core_DumpOpcode(ctx);

	for(i=0; i<ctx->n_lbl; i++)
		if(ctx->lbl[i]==ctx->ip)
	{
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("%s:\n", ctx->lbln[i]);
	}

	i=*ctx->ip++;
	if(i>=192)i=(i-192)*256+(*ctx->ip++);

	switch(i)
	{
	case BSVM_SOP_NOP:
		break;
	case BSVM_SOP_BLOCK:
		break;
	case BSVM_SOP_DBGMARK:
		break;
	case BSVM_SOP_CGENAST:
		break;

	case BSVM_SOP_PUSH:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2Core_Push(ctx, t);
		break;
	case BSVM_SOP_POP:
		BSVM_J2Core_PopDestroy(ctx);
		break;
	case BSVM_SOP_LOAD:
		BSVM_J2Core_FlattenStack(ctx);
		t=BSVM_J2Core_ReadID(ctx);
		basm_print("push dword 0x%x; push esi; call Lookup; add esp, 8;\n", t);
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_STORE:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword 0x%x; push esi; call Assign; add esp, 12;\n", t);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_BIND:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword 0x%x; push esi; call Bind; add esp, 12;\n", t);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_DYNBIND:
		break;
	case BSVM_SOP_LEXBIND:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		BSVM_J2Core_AdjustStack(ctx);
		basm_print("push dword 0x%x; push esi; call LexBind; add esp, 12;\n", t);
		basm_print("mov ebx, [esi+%d];\n", JIT_OFFS(stackpos));
		ctx->fl&=~255;
		break;

	case BSVM_SOP_LOADINDEX:
		BSVM_J2Core_PopToArgF(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call LoadIndex; add esp, 12;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_STOREINDEX:
		BSVM_J2Core_PopToArgF(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call StoreIndex; add esp, 16;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_LOADINDEX_S:
		t=BSVM_J2Core_ReadID(ctx);
		basm_print("push dword 0x%x; ", t);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call LoadIndex; add esp, 12;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_STOREINDEX_S:
		t=BSVM_J2Core_ReadID(ctx);
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword 0x%x; ", t);
		basm_print("push eax;\n");
		basm_print("push esi; call StoreIndex; add esp, 16;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_LOADINDEX_I:
		basm_print("push dword %d;\n", dyint(*ctx->ip++));
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call LoadIndex; add esp, 12;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_STOREINDEX_I:
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword %d; push eax;\n", dyint(*ctx->ip++));
		basm_print("push esi; call StoreIndex; add esp, 16;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_LOADINDEX_0:
		basm_print("push dword %d;\n", BSVM_MM_FIXNUM_0);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call LoadIndex; add esp, 12;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_LOADINDEX_1:
		basm_print("push dword %d;\n", BSVM_MM_FIXNUM_1);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call LoadIndex; add esp, 12;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_LOADINDEX_2:
		basm_print("push dword %d;\n", BSVM_MM_FIXNUM_2);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call LoadIndex; add esp, 12;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_LOADINDEX_3:
		basm_print("push dword %d;\n", BSVM_MM_FIXNUM_3);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push esi; call LoadIndex; add esp, 12;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;

	case BSVM_SOP_STOREINDEX_0:
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword %d; push eax;\n", BSVM_MM_FIXNUM_0);
		basm_print("push esi; call StoreIndex; add esp, 16;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_STOREINDEX_1:
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword %d; push eax;\n", BSVM_MM_FIXNUM_1);
		basm_print("push esi; call StoreIndex; add esp, 16;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_STOREINDEX_2:
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword %d; push eax;\n", BSVM_MM_FIXNUM_2);
		basm_print("push esi; call StoreIndex; add esp, 16;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_STOREINDEX_3:
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword %d; push eax ;\n", BSVM_MM_FIXNUM_3);
		basm_print("push esi; call StoreIndex; add esp, 16;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_PUSHSELF:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("mov eax, [esi+%d];\n", JIT_OFFS(self));
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_PUSH_NULL:
		BSVM_J2Core_Push(ctx, BSVM_MM_NULL);
		break;
	case BSVM_SOP_PUSH_TRUE:
		BSVM_J2Core_Push(ctx, BSVM_MM_TRUE);
		break;
	case BSVM_SOP_PUSH_FALSE:
		BSVM_J2Core_Push(ctx, BSVM_MM_FALSE);
		break;
	case BSVM_SOP_PUSH_SV:
		i=(signed char)(*ctx->ip++);
		BSVM_J2Core_Push(ctx, FIXNUM(i));
		break;
	case BSVM_SOP_PUSH_SV_P:
		i=*ctx->ip++;
		BSVM_J2Core_Push(ctx, FIXNUM(i));
		break;
	case BSVM_SOP_PUSH_SV_N:
		i=*ctx->ip++;
		BSVM_J2Core_Push(ctx, FIXNUM(-i));
		break;
	case BSVM_SOP_PUSH_SV_F:
		i=*ctx->ip++;
		i+=(*ctx->ip++)<<8;
		BSVM_J2Core_Push(ctx, dyflonum(((short)i)/100.0));
		break;

	case BSVM_SOP_PUSH_0: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_0); break;
	case BSVM_SOP_PUSH_1: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_1); break;
	case BSVM_SOP_PUSH_2: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_2); break;
	case BSVM_SOP_PUSH_3: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_3); break;
	case BSVM_SOP_PUSH_4: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_4); break;
	case BSVM_SOP_PUSH_5: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_5); break;
	case BSVM_SOP_PUSH_6: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_6); break;
	case BSVM_SOP_PUSH_7: BSVM_J2Core_Push(ctx, BSVM_MM_FIXNUM_7); break;
	case BSVM_SOP_PUSH_CF: BSVM_J2Core_Push(ctx, ctx->func); break;

	case BSVM_SOP_LLOAD:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword %d;\n", *ctx->ip++);
		basm_print("push esi; call LookupIdx; add esp, 8;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_LSTORE:
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword %d;\n", *ctx->ip++);
		basm_print("push esi; call AssignIdx; add esp, 12;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_LLOAD_F:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword %d;\n", *ctx->ip++);
		basm_print("push esi; call LookupIdx; add esp, 8;\n");
		BSVM_J2Core_PushEAX_F(ctx);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_LSTORE_F:
		BSVM_J2Core_PopToArgF(ctx);
		basm_print("push dword %d;\n", *ctx->ip++);
		basm_print("push esi; call AssignIdxF; add esp, 12;\n");
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_LLOAD_F2:
		if(ctx->fl&BSVM_JITFL_EAX)
			basm_print("mov [edi+ebx*4], eax; inc_r ebx;\n");
		i=*ctx->ip++;
		if(!(ctx->fl&BSVM_JITFL_ECX_EARG))
		{
			basm_print("mov ecx, [esi+%d]; ", JIT_OFFS(eargs));
			ctx->fl|=BSVM_JITFL_ECX_EARG;
		}
		basm_print("mov eax, [edi+ecx*4-%d];\n", (i+1)*4);
		BSVM_J2Core_PushEAX_F(ctx);
		break;
	case BSVM_SOP_LSTORE_F2:
		i=*ctx->ip++;
		BSVM_J2Core_PopEAX_F(ctx);
		if(!(ctx->fl&BSVM_JITFL_ECX_EARG))
		{
			basm_print("mov ecx, [esi+%d]; ", JIT_OFFS(eargs));
			ctx->fl|=BSVM_JITFL_ECX_EARG;
		}
		basm_print("mov [edi+ecx*4-%d], eax;\n", (i+1)*4);
		break;

	case BSVM_SOP_LINC_FN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("push dword %d;\n", i);
		basm_print("push esi; call LookupIdxF; add esp, 8;\n");
		basm_print("add, eax, 4;\n");
		basm_print("push eax; push dword %d;\n", i);
		basm_print("push esi; call AssignIdxF; add esp, 12;\n");
		break;
	case BSVM_SOP_LDEC_FN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("push dword %d;\n", i);
		basm_print("push esi; call LookupIdxF; add esp, 8;\n");
		basm_print("sub, eax, 4;\n");
		basm_print("push eax; push dword %d;\n", i);
		basm_print("push esi; call AssignIdxF; add esp, 12;\n");
		break;

	case BSVM_SOP_LINC_FN2:
//		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		if(!(ctx->fl&BSVM_JITFL_ECX_EARG))
		{
			basm_print("mov ecx, [esi+%d]; ", JIT_OFFS(eargs));
			ctx->fl|=BSVM_JITFL_ECX_EARG;
		}
		basm_print("add dword [edi+ecx*4-%d], 4;\n", (i+1)*4);
		break;
	case BSVM_SOP_LDEC_FN2:
//		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		if(!(ctx->fl&BSVM_JITFL_ECX_EARG))
		{
			basm_print("mov ecx, [esi+%d]; ", JIT_OFFS(eargs));
			ctx->fl|=BSVM_JITFL_ECX_EARG;
		}
		basm_print("add dword [edi+ecx*4-%d], 4;\n", (i+1)*4);
		break;

	case BSVM_SOP_CLEARENV: break;
	case BSVM_SOP_BINDPATTERN: break;
	case BSVM_SOP_LOADMINDEX: break;
	case BSVM_SOP_STOREMINDEX: break;

	case BSVM_SOP_TRYLOAD:
		break;

	case BSVM_SOP_CAST:
		break;

	case BSVM_SOP_MARK:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("mov ecx, [esi+%d]; ", JIT_OFFS(markstackpos));
		basm_print("mov edx, [esi+%d];\n", JIT_OFFS(markstack));
		basm_print("mov [edx+ecx*4], ebx; ");
		basm_print("inc dword [esi+%d];\n", JIT_OFFS(markstackpos));
	break;

	case BSVM_SOP_CALL:	BSVM_J2Core_Call(ctx); break;
	case BSVM_SOP_TAILCALL:	BSVM_J2Core_TailCall(ctx); break;
	case BSVM_SOP_CALL_S:
		t=BSVM_J2Core_ReadID(ctx); BSVM_J2Core_CallS(ctx, t); break;
	case BSVM_SOP_TAILCALL_S:
		t=BSVM_J2Core_ReadID(ctx); BSVM_J2Core_TailCallS(ctx, t); break;

	case BSVM_SOP_METHODCALL:	BSVM_J2Core_MethodCall(ctx); break;
	case BSVM_SOP_METHODTAILCALL:	BSVM_J2Core_MethodTailCall(ctx); break;
	case BSVM_SOP_METHODCALL_S:
		t=BSVM_J2Core_ReadID(ctx); BSVM_J2Core_MethodCallS(ctx, t); break;
	case BSVM_SOP_METHODTAILCALL_S:
		t=BSVM_J2Core_ReadID(ctx); BSVM_J2Core_MethodTailCallS(ctx, t); break;

	case BSVM_SOP_CALL_CF:		BSVM_J2Core_CallCF(ctx); break;
	case BSVM_SOP_TAILCALL_CF:	BSVM_J2Core_TailCallCF(ctx); break;

	case BSVM_SOP_RET:	BSVM_J2Core_Return(ctx); break;

	case BSVM_SOP_CAR:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call SafeDecRef; call GetCar;\n"
			"call incref; mov [edi+ebx*4-4], eax;\n"
			"pop eax; call checkref;\n");
		break;
	case BSVM_SOP_CDR:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call SafeDecRef; call GetCdr;\n"
			"call incref; mov [edi+ebx*4-4], eax;\n"
			"pop eax; call checkref;\n");
		break;
	case BSVM_SOP_CAAR:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call SafeDecRef; call GetCaar;\n"
			"call incref; mov [edi+ebx*4-4], eax;\n"
			"pop eax; call checkref;\n");
		break;
	case BSVM_SOP_CDAR:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call SafeDecRef; call GetCdar;\n"
			"call incref; mov [edi+ebx*4-4], eax;\n"
			"pop eax; call checkref;\n");
		break;
	case BSVM_SOP_CADR:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call SafeDecRef; call GetCadr;\n"
			"call incref; mov [edi+ebx*4-4], eax;\n"
			"pop eax; call checkref;\n");
		break;
	case BSVM_SOP_CDDR:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call SafeDecRef; call GetCddr;\n"
			"call incref; mov [edi+ebx*4-4], eax;\n"
			"pop eax; call checkref;\n");
		break;

	case BSVM_SOP_CLOSE:
//		basm_print("push dword [edi+ebx*4-4]; call SafeDecRef; call Close;\n"
//			"pop ecx; call incref; mov [edi+ebx*4-4], eax;\n");
		BSVM_J2Core_PopToArg(ctx);
		basm_print("call Close;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;

	case BSVM_SOP_DUP:
//		basm_print("mov eax, [edi+ebx*4-4]; call incref;\n"
//			"mov [edi+ebx*4], eax; inc_r ebx;\n");
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PushEAX_F(ctx);
		BSVM_J2Core_FlattenStack(ctx);
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_DUP_F:
//		basm_print("mov eax, [edi+ebx*4-4]; mov [edi+ebx*4], eax; inc_r ebx;\n");
		BSVM_J2Core_PopEAX_F(ctx);
		BSVM_J2Core_PushEAX_F(ctx);
		basm_print("mov [edi+ebx*4], eax; inc_r ebx;\n");
		BSVM_J2Core_PushEAX_F(ctx);
		break;
	case BSVM_SOP_EXCH:
//		BSVM_J2Core_FlattenStack(ctx);
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("xchg eax, [edi+ebx*4-4];\n");
			break;
		}

		basm_print("mov eax, [edi+ebx*4-4]; mov edx, [edi+ebx*4-8];\n"
			"mov [edi+ebx*4-8], eax; mov [edi+ebx*4-4], edx;\n");
		break;
	case BSVM_SOP_INDEX:
		BSVM_J2Core_FlattenStack(ctx);
		i=BSVM_J2Core_ReadIDX(ctx);
		basm_print("mov eax, [edi+ebx*4-%d];\n"
			"mov [edi+ebx*4], eax; inc_r ebx;\n", (i+1)*4);
		BSVM_J2Core_IncRef(ctx);
		break;
	case BSVM_SOP_RINDEX:
		BSVM_J2Core_FlattenStack(ctx);
		i=BSVM_J2Core_ReadIDX(ctx);
		basm_print("mov eax, [esi+%d]; mov ecx, [esi+%d]; dec ecx;\n"
			"mov edx, [eax+ecx*4]; add edx, %d;\n",
				JIT_OFFS(markstack),
				JIT_OFFS(markstackpos), i);
		basm_print("mov ecx, [esi+%d]; mov eax, [ecx+edx*4];\n",
			JIT_OFFS(stack));
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_SETINDEX:
		BSVM_J2Core_FlattenStack(ctx);
		i=BSVM_J2Core_ReadIDX(ctx);
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; "
			"mov [edi+ebx*4-%d], eax;\n",
				(i+1)*4);
		break;
	case BSVM_SOP_SETRINDEX:
		BSVM_J2Core_FlattenStack(ctx);
		i=BSVM_J2Core_ReadIDX(ctx);
		basm_print("mov eax, [esi+%d]; mov ecx, [esi+%d]; dec ecx;\n"
			"mov edx, [eax+ecx*4]; add edx, %d;\n",
				JIT_OFFS(markstack),
				JIT_OFFS(markstackpos), i);
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(stack));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
		basm_print("mov [ecx+edx*4], eax;\n");
		break;

	case BSVM_SOP_DUP_R:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov eax, [edi+ebx*4-4]; call incref;\n"
			"mov [esi+%d], eax;\n", JIT_OFFS(reg)+i*4);
		break;
	case BSVM_SOP_DUP_RF:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov eax, [edi+ebx*4-4]; mov [esi+%d], eax;\n",
			JIT_OFFS(reg)+i*4);
		break;
	case BSVM_SOP_PUSH_R:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov eax, [esi+%d]; call incref;\n"
			"mov [edi+ebx*4], eax; inc_r ebx;\n", JIT_OFFS(reg)+i*4);
		break;
	case BSVM_SOP_PUSH_RF:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov eax, [esi+%d]; mov [edi+ebx*4], eax; inc_r ebx;\n",
			JIT_OFFS(reg)+i*4);
		break;
	case BSVM_SOP_POP_R:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; mov [esi+%d], eax;\n",
			JIT_OFFS(reg)+i*4);
		break;

	case BSVM_SOP_UNARYOP:
		i=*ctx->ip++;
		BSVM_J2Core_PopToArg(ctx);
		basm_print("push dword %d; push esi;\n"
			"call Unary; add esp, 12;\n", i);
		BSVM_J2Core_PushEAX(ctx);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_BINARYOP:
		i=*ctx->ip++;
		BSVM_J2Core_PopToArg(ctx);
		BSVM_J2Core_PopToArg(ctx);
		basm_print("push dword %d; push esi; call Binary; add esp, 16;\n", i);
		BSVM_J2Core_PushEAX(ctx);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_JMP:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8; i=((signed short)i);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		basm_print("jmp %s;\n", s);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_TRUE:
//		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8; i=((signed short)i);

		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		s1=BSVM_J2Core_GenSym(); s2=BSVM_J2Core_GenSym();

		BSVM_J2Core_PopEAX(ctx);
		basm_print("cmp eax, 0x%x; je_b %s;\n", BSVM_MM_NULL, s1);
		basm_print("cmp eax, 0x%x; je_b %s;\n", BSVM_MM_FALSE, s1);
		basm_print("cmp eax, 0x%x; je_b %s;\n", BSVM_MM_FIXNUM_0, s1);
		basm_print("jmp_b %s; %s: jmp %s; %s: ", s2, s1, s, s2);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_FALSE:
//		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8; i=((signed short)i);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);

		BSVM_J2Core_PopEAX(ctx);
		basm_print("cmp eax, 0x%x; je %s;\n", BSVM_MM_NULL, s);
		basm_print("cmp eax, 0x%x; je %s;\n", BSVM_MM_FALSE, s);
		basm_print("cmp eax, 0x%x; je %s;\n", BSVM_MM_FIXNUM_0, s);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_JMP_COND_U:
//		BSVM_J2Core_FlattenStack(ctx);

		j=*ctx->ip++;
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);

//		j=BSVM_Opr_CondUnary(j, BSVM_SVM_Pop(ctx));

		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);
		BSVM_J2Core_PopToArg(ctx);
		basm_print("push dword %d; call CondUnary;\n"
			"add esp, 8;\n", j);
		basm_print("or eax, eax; jnz %s;\n", s);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_COND_B:
//		BSVM_J2Core_FlattenStack(ctx);

		j=*ctx->ip++;
//		j=BSVM_Opr_CondBinary(j, u, v);
		i=*ctx->ip++;
		i|=(*ctx->ip++)<<8;
		i=((signed short)i);
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+i);

		BSVM_J2Core_PopToArg(ctx);
		BSVM_J2Core_PopToArg(ctx);
		basm_print("push dword %d; call CondBinary; add esp, 12;\n", j);
		basm_print("or eax, eax; jnz %s;\n", s);
		ctx->fl&=~BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_LIST:
		BSVM_J2Core_AdjustStack(ctx);
		basm_print("call PopList2;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_DICT:
		BSVM_J2Core_AdjustStack(ctx);
		basm_print("call PopDict;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_VECTOR:
		BSVM_J2Core_AdjustStack(ctx);
		basm_print("call PopVector;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_COMPLEX_I:
		BSVM_J2Core_AdjustStack(ctx);
		basm_print("call PopComplexI;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;
	case BSVM_SOP_MATRIX:
		BSVM_J2Core_AdjustStack(ctx);
		basm_print("call PopMatrix;\n");
		BSVM_J2Core_PushEAX(ctx);
		break;

	case BSVM_SOP_INC_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("add eax, 4;\n");
			break;
		}
		basm_print("add dword [edi+ebx*4-4], 4;\n");
		break;
	case BSVM_SOP_DEC_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("sub eax, 4;\n");
			break;
		}
		basm_print("sub dword [edi+ebx*4-4], 4;\n");
		break;
	case BSVM_SOP_INC2_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("add eax, 8;\n");
			break;
		}
		basm_print("add dword [edi+ebx*4-4], 8;\n");
		break;
	case BSVM_SOP_DEC2_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("sub eax, 8;\n");
			break;
		}
		basm_print("sub dword [edi+ebx*4-4], 8;\n");
		break;

	case BSVM_SOP_ADD_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("and al, 0xFC; add [edi+ebx*4-4], eax;\n");
			ctx->fl&=~BSVM_JITFL_EAX;
			break;
		}
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n"
			"and al, 0xFC; add [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_SUB_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("and al, 0xFC; sub [edi+ebx*4-4], eax;\n");
			ctx->fl&=~BSVM_JITFL_EAX;
			break;
		}
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n"
			"and al, 0xFC; sub [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_MUL_FN:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"sar eax, 2; sar ecx, 2; xor edx, edx; imul ecx;\n"
			"shl eax, 2; or eax, 2; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_DIV_FN:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call DivFn; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_IDIV_FN:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"sar eax, 2; sar ecx, 2; xor edx, edx; idiv ecx;\n"
			"shl eax, 2; or eax, 2; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_MOD_FN:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"sar eax, 2; sar ecx, 2; xor edx, edx; idiv ecx;\n"
			"shl edx, 2; or edx, 2; mov [edi+ebx*4-4], edx;\n");
		break;
	case BSVM_SOP_AND_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("and [edi+ebx*4-4], eax;\n");
			ctx->fl&=~BSVM_JITFL_EAX;
			break;
		}
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; and [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_OR_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("or [edi+ebx*4-4], eax;\n");
			ctx->fl&=~BSVM_JITFL_EAX;
			break;
		}
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; or [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_XOR_FN:
		if(ctx->fl&BSVM_JITFL_EAX)
		{
			basm_print("and al, 0xFC; xor [edi+ebx*4-4], eax;\n");
			ctx->fl&=~BSVM_JITFL_EAX;
			break;
		}
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n"
			"and al, 0xFC; xor [edi+ebx*4-4], eax;\n");
		break;

	case BSVM_SOP_CMP_L_FN:
		BSVM_J2Core_FlattenStack(ctx);
		s=BSVM_J2Core_GenSym(); s1=BSVM_J2Core_GenSym();
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"cmp eax, ecx; jge_b %s; mov eax, 0x%x; jmp_b %s;\n"
			"%s: mov eax, 0x%x; %s: mov [edi+ebx*4-4], eax;\n",
				s, BSVM_MM_TRUE, s1, s, BSVM_MM_FALSE, s1);
		break;
	case BSVM_SOP_CMP_G_FN:
		BSVM_J2Core_FlattenStack(ctx);
		s=BSVM_J2Core_GenSym(); s1=BSVM_J2Core_GenSym();
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"cmp eax, ecx; jle_b %s; mov eax, 0x%x; jmp_b %s;\n"
			"%s: mov eax, 0x%x; %s: mov [edi+ebx*4-4], eax;\n",
				s, BSVM_MM_TRUE, s1, s, BSVM_MM_FALSE, s1);
		break;
	case BSVM_SOP_CMP_LE_FN:
		BSVM_J2Core_FlattenStack(ctx);
		s=BSVM_J2Core_GenSym(); s1=BSVM_J2Core_GenSym();
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"cmp eax, ecx; jg_b %s; mov eax, 0x%x; jmp_b %s;\n"
			"%s: mov eax, 0x%x; %s: mov [edi+ebx*4-4], eax;\n",
				s, BSVM_MM_TRUE, s1, s, BSVM_MM_FALSE, s1);
		break;
	case BSVM_SOP_CMP_GE_FN:
		BSVM_J2Core_FlattenStack(ctx);
		s=BSVM_J2Core_GenSym(); s1=BSVM_J2Core_GenSym();
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"cmp eax, ecx; jl_b %s; mov eax, 0x%x; jmp_b %s;\n"
			"%s: mov eax, 0x%x; %s: mov [edi+ebx*4-4], eax;\n",
				s, BSVM_MM_TRUE, s1, s, BSVM_MM_FALSE, s1);
		break;
	case BSVM_SOP_CMP_E_FN:
		BSVM_J2Core_FlattenStack(ctx);
		s=BSVM_J2Core_GenSym(); s1=BSVM_J2Core_GenSym();
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"cmp eax, ecx; jne_b %s; mov eax, 0x%x; jmp_b %s;\n"
			"%s: mov eax, 0x%x; %s: mov [edi+ebx*4-4], eax;\n",
				s, BSVM_MM_TRUE, s1, s, BSVM_MM_FALSE, s1);
		break;
	case BSVM_SOP_CMP_NE_FN:
		BSVM_J2Core_FlattenStack(ctx);
		s=BSVM_J2Core_GenSym(); s1=BSVM_J2Core_GenSym();
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"cmp eax, ecx; je_b %s; mov eax, 0x%x; jmp_b %s;\n"
			"%s: mov eax, 0x%x; %s: mov [edi+ebx*4-4], eax;\n",
				s, BSVM_MM_TRUE, s1, s, BSVM_MM_FALSE, s1);
		break;

	case BSVM_SOP_ADD_FN_C:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("add dword [edi+ebx*4-4], %d;\n", i<<2);
		break;
	case BSVM_SOP_SUB_FN_C:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("sub dword [edi+ebx*4-4], %d;\n", i<<2);
		break;
	case BSVM_SOP_MUL_FN_C:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov eax, [edi+ebx*4-4]; and al, 0xFC; imul eax, %d;\n"
			"or eax, 2; mov [edi+ebx*4-4], eax;\n", i);
		break;

	case BSVM_SOP_SHL_FN:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"sar eax, 2; sar ecx, 2; shl eax, cl;\n"
			"shl eax, 2; or eax, 2; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_SHR_FN:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; mov eax, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"sar eax, 2; sar ecx, 2; sar eax, cl;\n"
			"shl eax, 2; or eax, 2; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_EXP_FN:
		BSVM_J2Core_FlattenStack(ctx);
		s=BSVM_J2Core_GenSym(); s1=BSVM_J2Core_GenSym();
		basm_print("dec_r ebx; mov edx, [edi+ebx*4-4]; mov ecx, [edi+ebx*4];\n"
			"sar edx, 2; sar ecx, 2; mov eax, 1;\n"
			"%s: or ecx, ecx; jz_b %s; imul eax, edx; jmp_b %s;\n"
			"%s: shl eax, 2; or eax, 2; mov [edi+ebx*4-4], eax;\n",
			s, s1, s, s1);
		break;

	case BSVM_SOP_SHL_FN_C:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov eax, [edi+ebx*4-4]; sar eax, 2; shl eax, %d;\n"
			"or eax, 2; mov [edi+ebx*4-4], eax;\n", i+2);
		break;
	case BSVM_SOP_SHR_FN_C:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov eax, [edi+ebx*4-4]; sar eax, %d; shl eax, 2;\n"
			"or eax, 2; mov [edi+ebx*4-4], eax;\n", i+2);
		break;
	case BSVM_SOP_EXP_FN_C:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++;
		basm_print("mov edx, [edi+ebx*4-4]; sar edx, 2; mov eax, 1;\n");
		while(i--)basm_print("imul eax, edx;\n");
		basm_print("shl eax, 2; or eax, 2; mov [edi+ebx*4-4], eax;\n");
		break;


	case BSVM_SOP_CONV_FN2FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call ConvFnFl; pop edx;\n"
			" mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_CONV_FL2FN:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call ConvFlFn; pop edx;\n"
			" mov [edi+ebx*4-4], eax;\n");
		break;

	case BSVM_SOP_ADD_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call AddFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_SUB_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call SubFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_MUL_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call MulFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_DIV_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call DivFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_CMP_L_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call CmpLFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_CMP_G_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call CmpGFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_CMP_LE_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call CmpLeFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_CMP_GE_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call CmpGeFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_CMP_E_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call CmpEFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_CMP_NE_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("dec_r ebx; push dword [edi+ebx*4]; push dword [edi+ebx*4-4];\n"
			"call CmpNeFl; add esp, 8; mov [edi+ebx*4-4], eax;\n");
		break;
	case BSVM_SOP_NEG_FL:
		BSVM_J2Core_FlattenStack(ctx);
		basm_print("push dword [edi+ebx*4-4]; call NegFl; pop edx;\n"
			" mov [edi+ebx*4-4], eax;\n");
		break;

	case BSVM_SOP_JMP_L_FN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; mov eax, [edi+ebx*4];\n"
			"cmp eax, [edi+ebx*4+4]; jl %s;\n", s);
		break;

	case BSVM_SOP_JMP_G_FN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; mov eax, [edi+ebx*4];\n"
			"cmp eax, [edi+ebx*4+4]; jg %s;\n", s);
		break;
	case BSVM_SOP_JMP_LE_FN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; mov eax, [edi+ebx*4];\n"
			"cmp eax, [edi+ebx*4+4]; jle %s;\n", s);
		break;
	case BSVM_SOP_JMP_GE_FN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; mov eax, [edi+ebx*4];\n"
			"cmp eax, [edi+ebx*4+4]; jge %s;\n", s);
		break;

	case BSVM_SOP_JMP_L_FL:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; push dword [edi+ebx*4+4]; push dword [edi+ebx*4];\n"
			"call CmpLFl; add esp, 8; or eax, eax; jnz %s;\n", s);
		break;
	case BSVM_SOP_JMP_G_FL:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; push dword [edi+ebx*4+4]; push dword [edi+ebx*4];\n"
			"call CmpGFl; add esp, 8; or eax, eax; jnz %s;\n", s);
		break;
	case BSVM_SOP_JMP_LE_FL:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; push dword [edi+ebx*4+4]; push dword [edi+ebx*4];\n"
			"call CmpLeFl; add esp, 8; or eax, eax; jnz %s;\n", s);
		break;
	case BSVM_SOP_JMP_GE_FL:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("sub ebx, 2; push dword [edi+ebx*4+4]; push dword [edi+ebx*4];\n"
			"call CmpGeFl; add esp, 8; or eax, eax; jnz %s;\n", s);
		break;

	case BSVM_SOP_JMP_E_LFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; j=*ctx->ip++;
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (i+1)*4);
		basm_print("cmp eax, [edi+ecx*4-%d];\n", (j+1)*4);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("je %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_NE_LFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; j=*ctx->ip++;
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (i+1)*4);
		basm_print("cmp eax, [edi+ecx*4-%d];\n", (j+1)*4);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("jne %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_JMP_L_LFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; j=*ctx->ip++;
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (i+1)*4);
		basm_print("cmp eax, [edi+ecx*4-%d];\n", (j+1)*4);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("jl %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_G_LFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; j=*ctx->ip++;
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (i+1)*4);
		basm_print("cmp eax, [edi+ecx*4-%d];\n", (j+1)*4);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("jg %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_LE_LFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; j=*ctx->ip++;
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (i+1)*4);
		basm_print("cmp eax, [edi+ecx*4-%d];\n", (j+1)*4);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("jle %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_GE_LFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; j=*ctx->ip++;
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (i+1)*4);
		basm_print("cmp eax, [edi+ecx*4-%d];\n", (j+1)*4);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("jge %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;

	case BSVM_SOP_JMP_E_ZFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("je %s;\n", s);
		break;
	case BSVM_SOP_JMP_NE_ZFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jne %s;\n", s);
		break;
	case BSVM_SOP_JMP_L_ZFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jl %s;\n", s);
		break;
	case BSVM_SOP_JMP_G_ZFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jg %s;\n", s);
		break;
	case BSVM_SOP_JMP_LE_ZFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jle %s;\n", s);
		break;
	case BSVM_SOP_JMP_GE_ZFN:
		BSVM_J2Core_FlattenStack(ctx);
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4];\n");
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jge %s;\n", s);
		break;

	case BSVM_SOP_JMP_E_LZFN:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("je %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_NE_LZFN:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jne %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_L_LZFN:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jl %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_G_LZFN:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jg %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_LE_LZFN:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jle %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_GE_LZFN:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", BSVM_MM_FIXNUM_0);
		basm_print("jge %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;


	case BSVM_SOP_JMP_E_FNC:
		BSVM_J2Core_FlattenStack(ctx);
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; cmp eax, 0x%x;\n"
			"je %s;\n", t, s);
		break;
	case BSVM_SOP_JMP_NE_FNC:
		BSVM_J2Core_FlattenStack(ctx);
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; cmp eax, 0x%x;\n"
			"jne %s;\n", t, s);
		break;
	case BSVM_SOP_JMP_L_FNC:
		BSVM_J2Core_FlattenStack(ctx);
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; cmp eax, 0x%x;\n"
			"jl %s;\n", t, s);
		break;
	case BSVM_SOP_JMP_G_FNC:
		BSVM_J2Core_FlattenStack(ctx);
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; cmp eax, 0x%x;\n"
			"jg %s;\n", t, s);
		break;
	case BSVM_SOP_JMP_LE_FNC:
		BSVM_J2Core_FlattenStack(ctx);
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; cmp eax, 0x%x;\n"
			"jle %s;\n", t, s);
		break;
	case BSVM_SOP_JMP_GE_FNC:
		BSVM_J2Core_FlattenStack(ctx);
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("dec_r ebx; mov eax, [edi+ebx*4]; cmp eax, 0x%x;\n"
			"jge %s;\n", t, s);
		break;

	case BSVM_SOP_JMP_E_LFNC:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", t);
		basm_print("je %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_NE_LFNC:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", t);
		basm_print("jne %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_L_LFNC:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", t);
		basm_print("jl %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_G_LFNC:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", t);
		basm_print("jg %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_LE_LFNC:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", t);
		basm_print("jle %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;
	case BSVM_SOP_JMP_GE_LFNC:
		BSVM_J2Core_FlattenStack(ctx);
		j=*ctx->ip++;
		t=FIXNUM((signed char)(*ctx->ip++));
		i=*ctx->ip++; i|=(*ctx->ip++)<<8;
		s=BSVM_J2Core_GetLabel(ctx, ctx->ip+((signed short)i));
		basm_print("mov ecx, [esi+%d];\n", JIT_OFFS(eargs));
		basm_print("mov eax, [edi+ecx*4-%d];\n", (j+1)*4);
		basm_print("cmp eax, 0x%x;\n", t);
		basm_print("jge %s;\n", s);
		ctx->fl|=BSVM_JITFL_ECX_EARG;
		break;

	default:
		break;
	}

	basm_print("\n");

	return(0);
}


void BSVM_J2Core_CompileBlock(dyt blk)
{
	bsjContext ctx;
	BSVM_SVMBlock *fcn;
	byte *ip;
	char *buf;

	BSVM_J2Core_Init();


	fcn=BSVM_TY_GetObjData(blk);

	//don't bother with top-level expressions or anonymous closures
	if(!BSVM_SYMBOLP(fcn->name))
		return;

	SET(fcn->th_name, SYM(BSVM_J2Core_GenSym()));

	ctx=bsvm_jit_ctx;
	ctx->lit=fcn->lit;
	ctx->args=fcn->args;
	ctx->local=BSVM_MM_NULL;
	ctx->func=blk;
	ctx->blk=fcn;	//counter-intuitive...

	ctx->n_lbl=0;

	ctx->ip=fcn->body;
	while(ctx->ip<(fcn->body+fcn->szbody))
		BSVM_J2Core_PreStep(ctx);

	BASM_BeginAssembly(BSVM_TOSYM(fcn->th_name));
//	ip=BASM_GetIP();

	basm_print("; Func '%s'->'%s'\n",
		BSVM_TOSYM(fcn->name), BSVM_TOSYM(fcn->th_name));
	basm_print("%s:\n", BSVM_TOSYM(fcn->th_name));
	BSVM_J2Core_Entry(ctx);
	basm_print("\n");

	ctx->ip=fcn->body;
	while(ctx->ip<(fcn->body+fcn->szbody))
		BSVM_J2Core_Step(ctx);

	ip=BASM_EndAssembly();
	fcn->thunk=ip;
}

