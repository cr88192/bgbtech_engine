#include <frbc2ci.h>

#define OP_NI(OPN, YYN, SN, TN, OP, TY) \
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(FR2CI_Frame *frm, \
		FR2CI_TailOpcode *op) \
	{ return((frm->SN[op->s].TY OP frm->TN[op->t].TY)? \
		(op->jmptrace):(op->nexttrace)); }

#define OP_NATHDRSEQ	\
		FR2CI_Context *ctx;									\
		FR2CI_Frame *frm2;									\
		ctx=frm->ctx;										\
		fcn=op->imm.p;										\

#define OP_NATRETSEQ	\
		if(ctx->framestackpos>0)							\
		{													\
			ctx->framestackpos--;							\
			frm2=ctx->framestack[ctx->framestackpos];		\
			ctx->frame=frm2;								\
			FR2CI_CleanupFrame(ctx, frm);					\
			FR2CI_FreeFrame(ctx, frm);						\
			return(frm2->rettrace);						\
		}													\
		FR2CI_CleanupFrame(ctx, frm);						\
		FR2CI_FreeFrame(ctx, frm);							\
		ctx->frame=NULL;									\
		ctx->status=FR2C_STATUS_NO;							\
		return(NULL);										\

#define OP_NATCALL0(OPN, YYN, RTY, RTYT)					\
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(					\
		FR2CI_Frame *frm, FR2CI_TailOpcode *op)				\
	{														\
		RTYT (*fcn)();										\
		OP_NATHDRSEQ										\
		ctx->retval.RTY=fcn();								\
		OP_NATRETSEQ										\
	}														\

#define OP_NATCALL1(OPN, YYN, RTY, RTYT, ATY0, ATYT0)		\
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(					\
		FR2CI_Frame *frm, FR2CI_TailOpcode *op)				\
	{														\
		RTYT (*fcn)(ATYT0);									\
		OP_NATHDRSEQ										\
		ctx->retval.RTY=fcn(frm->arg[0].ATY0);				\
		OP_NATRETSEQ										\
	}														\

#define OP_NATCALL2(OPN, YYN, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1)										\
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(					\
		FR2CI_Frame *frm, FR2CI_TailOpcode *op)				\
	{														\
		RTYT (*fcn)(ATYT0, ATYT1);							\
		OP_NATHDRSEQ										\
		ctx->retval.RTY=fcn(frm->arg[0].ATY0,				\
			frm->arg[1].ATY1);								\
		OP_NATRETSEQ										\
	}														\

#define OP_NATCALL3(OPN, YYN, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2)							\
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(					\
		FR2CI_Frame *frm, FR2CI_TailOpcode *op)				\
	{														\
		RTYT (*fcn)(ATYT0, ATYT1, ATYT2);					\
		OP_NATHDRSEQ										\
		ctx->retval.RTY=fcn(frm->arg[0].ATY0,				\
			frm->arg[1].ATY1, frm->arg[2].ATY2);			\
		OP_NATRETSEQ										\
	}														\

#define OP_NATCALL4(OPN, YYN, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2, ATY3, ATYT3)				\
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(					\
		FR2CI_Frame *frm, FR2CI_TailOpcode *op)				\
	{														\
		RTYT (*fcn)(ATYT0, ATYT1, ATYT2, ATYT3);			\
		OP_NATHDRSEQ										\
		ctx->retval.RTY=fcn(frm->arg[0].ATY0,				\
			frm->arg[1].ATY1, frm->arg[2].ATY2,				\
			frm->arg[2].ATY3);								\
		OP_NATRETSEQ										\
	}														\

#define OP_NATCALL5(OPN, YYN, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2, ATY3, ATYT3, ATY4, ATYT4)	\
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(					\
		FR2CI_Frame *frm, FR2CI_TailOpcode *op)				\
	{														\
		RTYT (*fcn)(ATYT0, ATYT1, ATYT2, ATYT3, ATYT4);		\
		OP_NATHDRSEQ										\
		ctx->retval.RTY=fcn(frm->arg[0].ATY0,				\
			frm->arg[1].ATY1, frm->arg[2].ATY2,				\
			frm->arg[2].ATY3, frm->arg[2].ATY4);			\
		OP_NATRETSEQ										\
	}														\

OP_NATCALL0(NatCall, I_V, i, int)
OP_NATCALL0(NatCall, L_V, l, long long)
OP_NATCALL0(NatCall, F_V, f, float)
OP_NATCALL0(NatCall, D_V, d, double)
OP_NATCALL0(NatCall, P_V, p, void *)

#define OPB_NATCALL1(OPN, YYN, RTY, RTYT)					\
	OP_NATCALL1(NatCall, YYN##I, RTY, RTYT, i, int)			\
	OP_NATCALL1(NatCall, YYN##L, RTY, RTYT, l, long long)	\
	OP_NATCALL1(NatCall, YYN##F, RTY, RTYT, f, float)		\
	OP_NATCALL1(NatCall, YYN##D, RTY, RTYT, d, double)		\
	OP_NATCALL1(NatCall, YYN##P, RTY, RTYT, p, void *)		\

OPB_NATCALL1(NatCall, I_, i, int)
OPB_NATCALL1(NatCall, L_, l, long long)
OPB_NATCALL1(NatCall, F_, f, float)
OPB_NATCALL1(NatCall, D_, d, double)
OPB_NATCALL1(NatCall, P_, p, void *)


#define OPB2_NATCALL2(OPN, YYN, RTY, RTYT, ATY0, ATYT0)		\
	OP_NATCALL2(NatCall, YYN##I, RTY, RTYT, ATY0, ATYT0, i, int)		\
	OP_NATCALL2(NatCall, YYN##L, RTY, RTYT, ATY0, ATYT0, l, long long)	\
	OP_NATCALL2(NatCall, YYN##F, RTY, RTYT, ATY0, ATYT0, f, float)		\
	OP_NATCALL2(NatCall, YYN##D, RTY, RTYT, ATY0, ATYT0, d, double)	\
	OP_NATCALL2(NatCall, YYN##P, RTY, RTYT, ATY0, ATYT0, p, void*)		\

#define OPB_NATCALL2(OPN, YYN, RTY, RTYT)					\
	OPB2_NATCALL2(NatCall, YYN##I, RTY, RTYT, i, int)			\
	OPB2_NATCALL2(NatCall, YYN##L, RTY, RTYT, l, long long)	\
	OPB2_NATCALL2(NatCall, YYN##F, RTY, RTYT, f, float)		\
	OPB2_NATCALL2(NatCall, YYN##D, RTY, RTYT, d, double)		\
	OPB2_NATCALL2(NatCall, YYN##P, RTY, RTYT, p, void*)		\

OPB_NATCALL2(NatCall, I_, i, int)
OPB_NATCALL2(NatCall, L_, l, long long)
OPB_NATCALL2(NatCall, F_, f, float)
OPB_NATCALL2(NatCall, D_, d, double)
OPB_NATCALL2(NatCall, P_, p, void*)


#define OPB3_NATCALL3(OPN, YYN, RTY, RTYT, ATY0, ATYT0, ATY1, ATYT1)	\
	OP_NATCALL3(NatCall, YYN##I, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, i, int)									\
	OP_NATCALL3(NatCall, YYN##L, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, l, long long)								\
	OP_NATCALL3(NatCall, YYN##F, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, f, float)									\
	OP_NATCALL3(NatCall, YYN##D, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, d, double)								\
	OP_NATCALL3(NatCall, YYN##P, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, p, void*)									\

#define OPB2_NATCALL3(OPN, YYN, RTY, RTYT, ATY0, ATYT0)		\
	OPB3_NATCALL3(NatCall, YYN##I, RTY, RTYT, ATY0, ATYT0, i, int)		\
	OPB3_NATCALL3(NatCall, YYN##L, RTY, RTYT, ATY0, ATYT0, l, long long)	\
	OPB3_NATCALL3(NatCall, YYN##F, RTY, RTYT, ATY0, ATYT0, f, float)		\
	OPB3_NATCALL3(NatCall, YYN##D, RTY, RTYT, ATY0, ATYT0, d, double)	\
	OPB3_NATCALL3(NatCall, YYN##P, RTY, RTYT, ATY0, ATYT0, p, void*)		\

#define OPB_NATCALL3(OPN, YYN, RTY, RTYT)					\
	OPB2_NATCALL3(NatCall, YYN##I, RTY, RTYT, i, int)			\
	OPB2_NATCALL3(NatCall, YYN##L, RTY, RTYT, l, long long)	\
	OPB2_NATCALL3(NatCall, YYN##F, RTY, RTYT, f, float)		\
	OPB2_NATCALL3(NatCall, YYN##D, RTY, RTYT, d, double)		\
	OPB2_NATCALL3(NatCall, YYN##P, RTY, RTYT, p, void*)		\

OPB_NATCALL3(NatCall, I_, i, int)
OPB_NATCALL3(NatCall, L_, l, long long)
OPB_NATCALL3(NatCall, F_, f, float)
OPB_NATCALL3(NatCall, D_, d, double)
OPB_NATCALL3(NatCall, P_, p, void*)


#define OPB4_NATCALL4(OPN, YYN, RTY, RTYT, ATY0, ATYT0,			\
		ATY1, ATYT1, ATY2, ATYT2)								\
	OP_NATCALL4(NatCall, YYN##I, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2, i, int)						\
	OP_NATCALL4(NatCall, YYN##L, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2, l, long long)				\
	OP_NATCALL4(NatCall, YYN##F, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2, f, float)						\
	OP_NATCALL4(NatCall, YYN##D, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2, d, double)					\
	OP_NATCALL4(NatCall, YYN##P, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, ATY2, ATYT2, p, void*)						\

#define OPB3_NATCALL4(OPN, YYN, RTY, RTYT, ATY0, ATYT0, ATY1, ATYT1)	\
	OPB4_NATCALL4(NatCall, YYN##I, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, i, int)									\
	OPB4_NATCALL4(NatCall, YYN##L, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, l, long long)								\
	OPB4_NATCALL4(NatCall, YYN##F, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, f, float)									\
	OPB4_NATCALL4(NatCall, YYN##D, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, d, double)								\
	OPB4_NATCALL4(NatCall, YYN##P, RTY, RTYT, ATY0, ATYT0,		\
		ATY1, ATYT1, p, void*)									\

#define OPB2_NATCALL4(OPN, YYN, RTY, RTYT, ATY0, ATYT0)		\
	OPB3_NATCALL4(NatCall, YYN##I, RTY, RTYT, ATY0, ATYT0, i, int)		\
	OPB3_NATCALL4(NatCall, YYN##L, RTY, RTYT, ATY0, ATYT0, l, long long)	\
	OPB3_NATCALL4(NatCall, YYN##F, RTY, RTYT, ATY0, ATYT0, f, float)		\
	OPB3_NATCALL4(NatCall, YYN##D, RTY, RTYT, ATY0, ATYT0, d, double)	\
	OPB3_NATCALL4(NatCall, YYN##P, RTY, RTYT, ATY0, ATYT0, p, void*)		\

#define OPB_NATCALL4(OPN, YYN, RTY, RTYT)					\
	OPB2_NATCALL4(NatCall, YYN##I, RTY, RTYT, i, int)			\
	OPB2_NATCALL4(NatCall, YYN##L, RTY, RTYT, l, long long)	\
	OPB2_NATCALL4(NatCall, YYN##F, RTY, RTYT, f, float)		\
	OPB2_NATCALL4(NatCall, YYN##D, RTY, RTYT, d, double)		\
	OPB2_NATCALL4(NatCall, YYN##P, RTY, RTYT, p, void*)		\

OPB_NATCALL4(NatCall, I_, i, int)
OPB_NATCALL4(NatCall, L_, l, long long)
OPB_NATCALL4(NatCall, F_, f, float)
OPB_NATCALL4(NatCall, D_, d, double)
OPB_NATCALL4(NatCall, P_, p, void*)

#define OP_SWNI(OPN, YYN, IDX) \
	case IDX: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB0_SWNI(OPN, YYN)					\
	OP_SWNI(NatCall, I_V, 1*5+0)			\
	OP_SWNI(NatCall, L_V, 1*5+1)			\
	OP_SWNI(NatCall, F_V, 1*5+2)			\
	OP_SWNI(NatCall, D_V, 1*5+3)			\
	OP_SWNI(NatCall, P_V, 1*5+4)			\

#define OPB1_SWNI(OPN, YYN, IDX)			\
	OP_SWNI(NatCall, YYN##I, (IDX)*5+0)		\
	OP_SWNI(NatCall, YYN##L, (IDX)*5+1)		\
	OP_SWNI(NatCall, YYN##F, (IDX)*5+2)		\
	OP_SWNI(NatCall, YYN##D, (IDX)*5+3)		\
	OP_SWNI(NatCall, YYN##P, (IDX)*5+4)		\

#define OPB2_SWNI(OPN, YYN, IDX)			\
	OPB1_SWNI(NatCall, YYN##I, (IDX)*5+0)	\
	OPB1_SWNI(NatCall, YYN##L, (IDX)*5+1)	\
	OPB1_SWNI(NatCall, YYN##F, (IDX)*5+2)	\
	OPB1_SWNI(NatCall, YYN##D, (IDX)*5+3)	\
	OPB1_SWNI(NatCall, YYN##P, (IDX)*5+4)	\

#define OPB3_SWNI(OPN, YYN, IDX)			\
	OPB2_SWNI(NatCall, YYN##I, (IDX)*5+0)	\
	OPB2_SWNI(NatCall, YYN##L, (IDX)*5+1)	\
	OPB2_SWNI(NatCall, YYN##F, (IDX)*5+2)	\
	OPB2_SWNI(NatCall, YYN##D, (IDX)*5+3)	\
	OPB2_SWNI(NatCall, YYN##P, (IDX)*5+4)	\

#define OPB4_SWNI(OPN, YYN, IDX)			\
	OPB3_SWNI(NatCall, YYN##I, (IDX)*5+0)	\
	OPB3_SWNI(NatCall, YYN##L, (IDX)*5+1)	\
	OPB3_SWNI(NatCall, YYN##F, (IDX)*5+2)	\
	OPB3_SWNI(NatCall, YYN##D, (IDX)*5+3)	\
	OPB3_SWNI(NatCall, YYN##P, (IDX)*5+4)	\

#define OPB_SWNI(OPN)					\
	switch(idx) {						\
		OPB0_SWNI(OPN, _)				\
		OPB1_SWNI(OPN, I_, 1*5+0)		\
		OPB1_SWNI(OPN, L_, 1*5+1)		\
		OPB1_SWNI(OPN, F_, 1*5+2)		\
		OPB1_SWNI(OPN, D_, 1*5+3)		\
		OPB1_SWNI(OPN, P_, 1*5+4)		\
		OPB2_SWNI(OPN, I_, 1*5+0)		\
		OPB2_SWNI(OPN, L_, 1*5+1)		\
		OPB2_SWNI(OPN, F_, 1*5+2)		\
		OPB2_SWNI(OPN, D_, 1*5+3)		\
		OPB2_SWNI(OPN, P_, 1*5+4)		\
		OPB3_SWNI(OPN, I_, 1*5+0)		\
		OPB3_SWNI(OPN, L_, 1*5+1)		\
		OPB3_SWNI(OPN, F_, 1*5+2)		\
		OPB3_SWNI(OPN, D_, 1*5+3)		\
		OPB3_SWNI(OPN, P_, 1*5+4)		\
		OPB4_SWNI(OPN, I_, 1*5+0)		\
		OPB4_SWNI(OPN, L_, 1*5+1)		\
		OPB4_SWNI(OPN, F_, 1*5+2)		\
		OPB4_SWNI(OPN, D_, 1*5+3)		\
		OPB4_SWNI(OPN, P_, 1*5+4)		\
		}								\

char *FR2CI_ParseSigFindNext(
	FR2CI_Image *img, char *sig)
{
	char *s;
	s=sig;
	
	while(*s)
	{
		if(*s=='P')
			{ s++; continue; }
		if(*s=='W')
			{ s++; continue; }
		if(*s=='Q')
			{ s++; continue; }
		if((*s=='A') && (s[1]>='0') && (s[1]<='9'))
		{
			s++;
			while(*s)
			{
				if((*s>='0') && (*s<='9'))
					{ s++; continue; }
				if(*s==',')
					{ s++; continue; }
				break;
			}
			if(*s==';')s++;
			continue;
		}
	}
	
	if((*s=='X') || (*s=='L') || (*s=='U'))
	{
		s++;
		
		if((*s>='0') && (*s<='9'))
		{
			while(*s && (*s>='0') && (*s<='9'))
				s++;
			if(*s==';')s++;
			return(s);
		}

		while(*s && (*s!=';'))
			s++;
		if(*s==';')s++;
		return(s);
	}
	
	if((*s=='C') && (s[1]>='a') && (s[1]<='z'))
		{ s+=2; return(s); }
	if((*s=='D') && (s[1]>='a') && (s[1]<='z'))
		{ s+=2; return(s); }
	if((*s=='G') && (s[1]>='a') && (s[1]<='z'))
		{ s+=2; return(s); }

	if((*s>='a') && (*s<='z'))
		{ s++; return(s); }
	return(s);
}

int FR2CI_ParseSigBaseType5(
	FR2CI_Image *img, char *sig)
{
	if(!sig || !(*sig))
		return(FR2C_TY_I);

	if((*sig=='P') || (*sig=='Q') ||
			(*sig=='L') || (*sig=='A'))
		return(FR2C_TY_P);

	switch(*sig)
	{
	case 'a':	case 'b':
	case 'c':	case 'h':
	case 's':	case 't':
	case 'i':	case 'j':
		return(FR2C_TY_I);
	case 'l':	case 'm':
	case 'x':	case 'y':
		return(FR2C_TY_L);
	case 'f':
		return(FR2C_TY_F);
	case 'd':
		return(FR2C_TY_D);
	default:
		return(FR2C_TY_I);
	}
}

FR2CI_TailOpcode *FR2CI_MakeOpNatCall(
	FR2CI_Image *img, void *fcn, char *sig)
{
	FR2CI_TailOpcode *tmp;
	char *s;
	int i, j, k, idx, idx2;

	tmp=FR2CI_AllocTailOpcode(img);
//	tmp->s=s;
	tmp->imm.p=fcn;

	s=sig; idx=0; idx2=1;
	if(*s=='(')s++;
	while(*s && (*s!=')'))
	{
		i=FR2CI_ParseSigBaseType5(img, s);
		idx=(idx*5)+i;
		idx2=idx2*5;
		s=FR2CI_ParseSigFindNext(img, s);
	}
	if(*s==')')s++;
	i=FR2CI_ParseSigBaseType5(img, s);
//	idx=(idx*5)+i;
	idx=idx+(idx2*(1*5+i));

	OPB_SWNI(Ret);

	if(!tmp->run)
	{
		FR2CI_FreeTailOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}

FR2CI_Trace *FR2CI_GetFunctionNativeCall(
	FR2CI_Image *img, FR2CI_Function *func,
	char *name, char *sig)
{
	FR2CI_TailOpcode *top;
	FR2CI_Trace *trace;
	void *ptr;
	
	ptr=FR2CI_FFI_LookupLabelOS(name);
	if(!ptr)
		return(NULL);

	func->natfcn=ptr;
	
	top=FR2CI_MakeOpNatCall(img, ptr, sig);
	trace=FR2CI_CreateTrace(img, NULL, top, 0);

	func->ops=NULL;
	func->tops=fr2ci_malloc(1*sizeof(FR2CI_TailOpcode *));
	func->traces=fr2ci_malloc(1*sizeof(FR2CI_Trace *));
	func->n_ops=0;
	func->n_tops=1;
	func->n_traces=1;

	func->tops[0]=top;
	func->traces[0]=trace;
	func->entry=trace;

	return(trace);
}

void *FR2CI_FFI_LookupLabel(char *name)
{
	void *ptr;

	ptr=FR2CI_FFI_LookupLabelOS(name);
	return(ptr);
}

int FR2CI_FFI_LookupBlacklist(char *name)
{
	return(0);
}

void FR2CI_FFI_AddExeSection(char *name, s64 va, s64 sz, int fl)
{
}

void FR2CI_FFI_AddProxyPtr(char *name, void *ptr)
{
}

void *FR2CI_FFI_FetchSym(char *name)
{
	return(NULL);
}