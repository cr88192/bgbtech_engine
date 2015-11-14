#include <bgbfrir.h>

BGBFRIR_API FRIR_Method *FRIR_AllocMethod()
{
	FRIR_Method *tmp;
	
	tmp=gctalloc("frir_method_t", sizeof(FRIR_Method));
	tmp->tr_label=-1;
	return(tmp);
}

BGBFRIR_API FRIR_Opcode *FRIR_AllocMethodOpcode(FRIR_Method *mth)
{
	FRIR_Opcode *tmp;
	int i;

	if(!mth->ops)
	{
		i=16;
		mth->ops=gctalloc("frir_opcodelst_t", i*sizeof(FRIR_Opcode *));
		mth->m_ops=i;
	}
	
	if((mth->n_ops+1)>=mth->m_ops)
	{
		i=mth->m_ops;
		while((mth->n_ops+1)>=mth->m_ops)i=i+(i>>1);
		mth->ops=gcrealloc(mth->ops, i*sizeof(FRIR_Opcode *));
		mth->m_ops=i;
	}

	i=mth->n_ops++;
	tmp=gctalloc("frir_opcode_t", sizeof(FRIR_Opcode));
	mth->ops[i]=tmp;
	return(tmp);
}

BGBFRIR_API FRIR_Trace *FRIR_AllocMethodTrace(FRIR_Method *mth)
{
	FRIR_Trace *tmp;
	int i;

	if(!mth->trace)
	{
		i=16;
		mth->trace=gctalloc("frir_tracelst_t", i*sizeof(FRIR_Trace *));
		mth->m_trace=i;
	}
	
	if((mth->n_trace+1)>=mth->m_trace)
	{
		i=mth->m_trace;
		while((mth->n_trace+1)>=mth->m_trace)i=i+(i>>1);
		mth->trace=gcrealloc(mth->trace, i*sizeof(FRIR_Trace *));
		mth->m_trace=i;
	}

	i=mth->n_trace++;
	tmp=gctalloc("frir_trace_t", sizeof(FRIR_Trace));
	mth->trace[i]=tmp;
	tmp->method=mth;
	return(tmp);
}

BGBFRIR_API FRIR_Trace *FRIR_EndTrace(FRIR_Method *mth)
{
	FRIR_Trace *tr, *tr1;
	int i, n;

	n=mth->n_ops-mth->tr_start;
	if(!n)
	{
		//can't make empty trace
//		mth->tr_start=mth->n_ops;
//		mth->tr_label=-1;
		return(NULL);
	}
	
	if(mth->n_trace>0)
		{ tr1=mth->trace[mth->n_trace-1]; }
	else { tr1=NULL; }
	
	tr=FRIR_AllocMethodTrace(mth);
	tr->ops=gctalloc("frir_opcode_t", n*sizeof(FRIR_Opcode));
	tr->n_ops=n;
	tr->label=mth->tr_label;
	
	if(tr1)tr1->next=tr;
	
	for(i=0; i<n; i++)
		tr->ops[i]=*(mth->ops[mth->tr_start+i]);

	switch(n)
	{
	case 1:
		tr->run=FRIR_Trace_RunDefault1;
		tr->end=FRIR_Trace_EndDefault;
		break;
	case 2:
		tr->run=FRIR_Trace_RunDefault2;
		tr->end=FRIR_Trace_EndDefault;
		break;
	case 3:
		tr->run=FRIR_Trace_RunDefault3;
		tr->end=FRIR_Trace_EndDefault;
		break;
	case 4:
		tr->run=FRIR_Trace_RunDefault4;
		tr->end=FRIR_Trace_EndDefault;
		break;
	case 5:
		tr->run=FRIR_Trace_RunDefault5;
		tr->end=FRIR_Trace_EndDefault;
		break;
	case 6:
		tr->run=FRIR_Trace_RunDefault6;
		tr->end=FRIR_Trace_EndDefault;
		break;
	case 7:
		tr->run=FRIR_Trace_RunDefault7;
		tr->end=FRIR_Trace_EndDefault;
		break;
	case 8:
		tr->run=FRIR_Trace_RunDefault8;
		tr->end=FRIR_Trace_EndDefault;
		break;
	default:
		tr->run=FRIR_Trace_RunDefault;
		tr->end=FRIR_Trace_EndDefault;
		break;
	}

	mth->tr_start=mth->n_ops;
	mth->tr_label=-1;
	return(tr);
}

BGBFRIR_API void FRIR_EmitLabel(
	FRIR_Method *mth, int lbl)
{
	FRIR_Opcode *op;
	
	FRIR_EndTrace(mth);
	mth->tr_label=lbl;

//	op=FRIR_AllocMethodOpcode(mth);
//	op->op=FRIR_SOP_LABEL;
//	op->i.i=lbl;
//	op->fcn=FRIR_ThOp_NOP;
}

BGBFRIR_API void FRIR_EmitJump(
	FRIR_Method *mth, int lbl)
{
	FRIR_Opcode *op;
	FRIR_Trace *tr;

	op=FRIR_AllocMethodOpcode(mth);
	op->op=FRIR_SOP_JMP;
	op->i.i=lbl;
	op->fcn=FRIR_ThOp_NOP;
	tr=FRIR_EndTrace(mth);
	tr->jmp_label=lbl;
	tr->run=FRIR_Trace_RunDefault;
	tr->end=FRIR_Trace_EndJumpDefInit;
}

BGBFRIR_API void FRIR_EmitOpJccAI(
	FRIR_Method *mth, int opnum, int a, s32 i, int lbl)
{
	FRIR_Opcode *op;
	FRIR_Trace *tr;

	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->a=a;
	op->i.i=i;
//	op->i.k.x=i;
//	op->i.k.y=lbl;
	op->fcn=FRIR_ThOp_NOP;
	tr=FRIR_EndTrace(mth);
	tr->jmp_label=lbl;

	tr->run=FRIR_Trace_RunDefaultJcc;
	tr->end=FRIR_Trace_EndJccDefInit;
//	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitRet(
	FRIR_Method *mth, int reg)
{
	FRIR_Opcode *op;
	FRIR_Trace *tr;

	op=FRIR_AllocMethodOpcode(mth);
	op->op=FRIR_SOP_RET;
	op->a=reg;
	op->fcn=FRIR_ThOp_NOP;
	tr=FRIR_EndTrace(mth);
//	tr->jmp_label=lbl;
	tr->run=FRIR_Trace_RunDefault;
	tr->end=FRIR_Trace_EndRetDefInit;
}

BGBFRIR_API void FRIR_EmitOpV(
	FRIR_Method *mth, int opnum)
{
	FRIR_Opcode *op;
	
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCAB(
	FRIR_Method *mth, int opnum, int c, int a, int b)
{
	FRIR_Opcode *op;
	
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->a=a; op->b=b;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCA(
	FRIR_Method *mth, int opnum, int c, int a)
{
	FRIR_Opcode *op;
	
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->a=a;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCAI(
	FRIR_Method *mth, int opnum, int c, int a, s32 i)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->a=a; op->i.i=i;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCAL(
	FRIR_Method *mth, int opnum, int c, int a, s64 i)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->a=a; op->i.l=i;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCAF(
	FRIR_Method *mth, int opnum, int c, int a, f32 i)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->a=a; op->i.f=i;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCAD(
	FRIR_Method *mth, int opnum, int c, int a, f64 i)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->a=a; op->i.d=i;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCI(
	FRIR_Method *mth, int opnum, int c, s32 i)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->i.i=i;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCL(
	FRIR_Method *mth, int opnum, int c, s64 l)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->i.l=l;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCF(
	FRIR_Method *mth, int opnum, int c, f32 f)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->i.f=f;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}

BGBFRIR_API void FRIR_EmitOpCD(
	FRIR_Method *mth, int opnum, int c, f64 d)
{
	FRIR_Opcode *op;
	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum; op->c=c; op->i.d=d;
	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}


BGBFRIR_API void FRIR_EmitOpCallS(
	FRIR_Method *mth, int opnum,
	int ret, char *name, char *sig,
	int *args, int nargs)
{
	FRIR_Opcode *op;
	FRIR_Trace *tr;
	FRIR_MethodCallInfo *mci;
	int i;

	mci=FRIR_AllocMethodCallInfo();

	op=FRIR_AllocMethodOpcode(mth);
	op->op=opnum;
	//op->a=a; op->i.i=i;
	op->c=ret;
	op->fcn=FRIR_ThOp_NOP;
	tr=FRIR_EndTrace(mth);

	op->i.r=(dyt)mci;
	mci->name=(char *)dysymbol(name);
	mci->sig=dystrdup(sig);
	
	mci->args=gcatomic(nargs*sizeof(int));
	mci->nargs=nargs;
	
	for(i=0; i<nargs; i++)
		{ mci->args[i]=args[i]; }

	tr->run=FRIR_Trace_RunDefaultJcc;
	tr->end=FRIR_Trace_EndJccDefInit;
//	op->fcn=FRIR_FetchOpcodeHandler(opnum);
}
