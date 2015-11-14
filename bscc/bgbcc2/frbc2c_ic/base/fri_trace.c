#include <frbc2ci.h>

FR2CI_Trace *FR2CI_TraceRunGeneric(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops, **opse;
	int i;

#if 1
	ops=tr->op; i=tr->n_op;
	opse=ops+i;
	while((ops+16)<=opse)
	{
		op=ops[ 0]; op->run(frm, op);	op=ops[ 1]; op->run(frm, op);
		op=ops[ 2]; op->run(frm, op);	op=ops[ 3]; op->run(frm, op);
		op=ops[ 4]; op->run(frm, op);	op=ops[ 5]; op->run(frm, op);
		op=ops[ 6]; op->run(frm, op);	op=ops[ 7]; op->run(frm, op);
		op=ops[ 8]; op->run(frm, op);	op=ops[ 9]; op->run(frm, op);
		op=ops[10]; op->run(frm, op);	op=ops[11]; op->run(frm, op);
		op=ops[12]; op->run(frm, op);	op=ops[13]; op->run(frm, op);
		op=ops[14]; op->run(frm, op);	op=ops[15]; op->run(frm, op);
		ops+=16;
	}

	while((ops+4)<=opse)
	{
		op=ops[ 0]; op->run(frm, op);	op=ops[ 1]; op->run(frm, op);
		op=ops[ 2]; op->run(frm, op);	op=ops[ 3]; op->run(frm, op);
		ops+=4;
	}

	while(ops<opse)
	{
		op=*ops++; op->run(frm, op);
	}
#endif

//	for(i=0; i<tr->n_op; i++)
//		{ op=tr->op[i]; op->run(frm, op); }
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric0(FR2CI_Frame *frm, FR2CI_Trace *tr)
	{ return(tr->top->run(frm, tr->top)); }

FR2CI_Trace *FR2CI_TraceRunGeneric1(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	op=tr->op[0]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric2(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric3(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric4(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);		op=ops[3]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric5(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);		op=ops[3]; op->run(frm, op);
	op=ops[4]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric6(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);		op=ops[3]; op->run(frm, op);
	op=ops[4]; op->run(frm, op);		op=ops[5]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric7(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);		op=ops[3]; op->run(frm, op);
	op=ops[4]; op->run(frm, op);		op=ops[5]; op->run(frm, op);
	op=ops[6]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric8(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);		op=ops[3]; op->run(frm, op);
	op=ops[4]; op->run(frm, op);		op=ops[5]; op->run(frm, op);
	op=ops[6]; op->run(frm, op);		op=ops[7]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric9(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);		op=ops[3]; op->run(frm, op);
	op=ops[4]; op->run(frm, op);		op=ops[5]; op->run(frm, op);
	op=ops[6]; op->run(frm, op);		op=ops[7]; op->run(frm, op);
	op=ops[8]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric10(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[0]; op->run(frm, op);		op=ops[1]; op->run(frm, op);
	op=ops[2]; op->run(frm, op);		op=ops[3]; op->run(frm, op);
	op=ops[4]; op->run(frm, op);		op=ops[5]; op->run(frm, op);
	op=ops[6]; op->run(frm, op);		op=ops[7]; op->run(frm, op);
	op=ops[8]; op->run(frm, op);		op=ops[9]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric11(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[ 0]; op->run(frm, op);	op=ops[1]; op->run(frm, op);
	op=ops[ 2]; op->run(frm, op);	op=ops[3]; op->run(frm, op);
	op=ops[ 4]; op->run(frm, op);	op=ops[5]; op->run(frm, op);
	op=ops[ 6]; op->run(frm, op);	op=ops[7]; op->run(frm, op);
	op=ops[ 8]; op->run(frm, op);	op=ops[9]; op->run(frm, op);
	op=ops[10]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric12(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[ 0]; op->run(frm, op);	op=ops[ 1]; op->run(frm, op);
	op=ops[ 2]; op->run(frm, op);	op=ops[ 3]; op->run(frm, op);
	op=ops[ 4]; op->run(frm, op);	op=ops[ 5]; op->run(frm, op);
	op=ops[ 6]; op->run(frm, op);	op=ops[ 7]; op->run(frm, op);
	op=ops[ 8]; op->run(frm, op);	op=ops[ 9]; op->run(frm, op);
	op=ops[10]; op->run(frm, op);	op=ops[11]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric13(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[ 0]; op->run(frm, op);	op=ops[ 1]; op->run(frm, op);
	op=ops[ 2]; op->run(frm, op);	op=ops[ 3]; op->run(frm, op);
	op=ops[ 4]; op->run(frm, op);	op=ops[ 5]; op->run(frm, op);
	op=ops[ 6]; op->run(frm, op);	op=ops[ 7]; op->run(frm, op);
	op=ops[ 8]; op->run(frm, op);	op=ops[ 9]; op->run(frm, op);
	op=ops[10]; op->run(frm, op);	op=ops[11]; op->run(frm, op);
	op=ops[12]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric14(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[ 0]; op->run(frm, op);	op=ops[ 1]; op->run(frm, op);
	op=ops[ 2]; op->run(frm, op);	op=ops[ 3]; op->run(frm, op);
	op=ops[ 4]; op->run(frm, op);	op=ops[ 5]; op->run(frm, op);
	op=ops[ 6]; op->run(frm, op);	op=ops[ 7]; op->run(frm, op);
	op=ops[ 8]; op->run(frm, op);	op=ops[ 9]; op->run(frm, op);
	op=ops[10]; op->run(frm, op);	op=ops[11]; op->run(frm, op);
	op=ops[12]; op->run(frm, op);	op=ops[13]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric15(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[ 0]; op->run(frm, op);	op=ops[ 1]; op->run(frm, op);
	op=ops[ 2]; op->run(frm, op);	op=ops[ 3]; op->run(frm, op);
	op=ops[ 4]; op->run(frm, op);	op=ops[ 5]; op->run(frm, op);
	op=ops[ 6]; op->run(frm, op);	op=ops[ 7]; op->run(frm, op);
	op=ops[ 8]; op->run(frm, op);	op=ops[ 9]; op->run(frm, op);
	op=ops[10]; op->run(frm, op);	op=ops[11]; op->run(frm, op);
	op=ops[12]; op->run(frm, op);	op=ops[13]; op->run(frm, op);
	op=ops[14]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}

FR2CI_Trace *FR2CI_TraceRunGeneric16(FR2CI_Frame *frm, FR2CI_Trace *tr)
{
	FR2CI_Opcode *op;
	FR2CI_Opcode **ops;
	ops=tr->op;
	op=ops[ 0]; op->run(frm, op);	op=ops[ 1]; op->run(frm, op);
	op=ops[ 2]; op->run(frm, op);	op=ops[ 3]; op->run(frm, op);
	op=ops[ 4]; op->run(frm, op);	op=ops[ 5]; op->run(frm, op);
	op=ops[ 6]; op->run(frm, op);	op=ops[ 7]; op->run(frm, op);
	op=ops[ 8]; op->run(frm, op);	op=ops[ 9]; op->run(frm, op);
	op=ops[10]; op->run(frm, op);	op=ops[11]; op->run(frm, op);
	op=ops[12]; op->run(frm, op);	op=ops[13]; op->run(frm, op);
	op=ops[14]; op->run(frm, op);	op=ops[15]; op->run(frm, op);
	return(tr->top->run(frm, tr->top));
}


FR2CI_Trace *FR2CI_CreateTrace(FR2CI_Image *img,
	FR2CI_Opcode **ops, FR2CI_TailOpcode *top, int n_ops)
{
	FR2CI_Trace *tmp;

	tmp=FR2CI_AllocTrace(img);
	tmp->op=ops;
	tmp->top=top;
	tmp->n_op=n_ops;
	top->trace=tmp;
	
	switch(n_ops)
	{
	case 0: tmp->run=FR2CI_TraceRunGeneric0; break;
	case 1: tmp->run=FR2CI_TraceRunGeneric1; break;
	case 2: tmp->run=FR2CI_TraceRunGeneric2; break;
	case 3: tmp->run=FR2CI_TraceRunGeneric3; break;
	case 4: tmp->run=FR2CI_TraceRunGeneric4; break;
	case 5: tmp->run=FR2CI_TraceRunGeneric5; break;
	case 6: tmp->run=FR2CI_TraceRunGeneric6; break;
	case 7: tmp->run=FR2CI_TraceRunGeneric7; break;
	case 8: tmp->run=FR2CI_TraceRunGeneric8; break;
	case 9: tmp->run=FR2CI_TraceRunGeneric9; break;
	case 10: tmp->run=FR2CI_TraceRunGeneric10; break;
	case 11: tmp->run=FR2CI_TraceRunGeneric11; break;
	case 12: tmp->run=FR2CI_TraceRunGeneric12; break;
	case 13: tmp->run=FR2CI_TraceRunGeneric13; break;
	case 14: tmp->run=FR2CI_TraceRunGeneric14; break;
	case 15: tmp->run=FR2CI_TraceRunGeneric15; break;
	case 16: tmp->run=FR2CI_TraceRunGeneric16; break;

	default:
		tmp->run=FR2CI_TraceRunGeneric;
		break;
	}
	return(tmp);
}
