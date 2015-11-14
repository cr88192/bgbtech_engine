#include <frbc2ci.h>

#define OP_SWNI(OPN, YYN) \
	case FR2C_OPMODE_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNI(OPN) \
	switch(y) {	\
		OP_SWNI(OPN, RR)	\
		OP_SWNI(OPN, RA)	\
		OP_SWNI(OPN, RL)	\
		OP_SWNI(OPN, AR)	\
		OP_SWNI(OPN, AA)	\
		OP_SWNI(OPN, AL)	\
		OP_SWNI(OPN, LR)	\
		OP_SWNI(OPN, LA)	\
		OP_SWNI(OPN, LL)	}

#define OP_SWNW(OPN, YYN) \
	case FR2C_OPMODE2_##YYN: tmp->run=FR2CI_Op_##OPN##_##YYN;

#define OPB_SWNW(OPN) \
	switch(w) {	\
		OP_SWNW(OPN, IR)	\
		OP_SWNW(OPN, IA)	\
		OP_SWNW(OPN, IL)	\
		OP_SWNW(OPN, LR)	\
		OP_SWNW(OPN, LA)	\
		OP_SWNW(OPN, LL)	\
		OP_SWNW(OPN, FR)	\
		OP_SWNW(OPN, FA)	\
		OP_SWNW(OPN, FL)	\
		OP_SWNW(OPN, DR)	\
		OP_SWNW(OPN, DA)	\
		OP_SWNW(OPN, DL)	\
		OP_SWNW(OPN, PR)	\
		OP_SWNW(OPN, PA)	\
		OP_SWNW(OPN, PL)	}

void FR2CI_OpCall_CopyArgIR(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].i=frms->reg[op->s].i; }
void FR2CI_OpCall_CopyArgLR(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].l=frms->reg[op->s].l; }
void FR2CI_OpCall_CopyArgFR(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].f=frms->reg[op->s].f; }
void FR2CI_OpCall_CopyArgDR(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].d=frms->reg[op->s].d; }
void FR2CI_OpCall_CopyArgPR(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].p=frms->reg[op->s].p; }

void FR2CI_OpCall_CopyArgIA(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].i=frms->arg[op->s].i; }
void FR2CI_OpCall_CopyArgLA(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].l=frms->arg[op->s].l; }
void FR2CI_OpCall_CopyArgFA(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].f=frms->arg[op->s].f; }
void FR2CI_OpCall_CopyArgDA(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].d=frms->arg[op->s].d; }
void FR2CI_OpCall_CopyArgPA(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].p=frms->arg[op->s].p; }

void FR2CI_OpCall_CopyArgIL(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].i=frms->loc[op->s].i; }
void FR2CI_OpCall_CopyArgLL(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].l=frms->loc[op->s].l; }
void FR2CI_OpCall_CopyArgFL(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].f=frms->loc[op->s].f; }
void FR2CI_OpCall_CopyArgDL(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].d=frms->loc[op->s].d; }
void FR2CI_OpCall_CopyArgPL(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].p=frms->loc[op->s].p; }
void FR2CI_OpCall_CopyArgAL(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].p=&(frms->loc[op->s]); }

void FR2CI_OpCall_CopyArgIC(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].i=op->imm.i; }
void FR2CI_OpCall_CopyArgLC(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].l=op->imm.l; }
void FR2CI_OpCall_CopyArgFC(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].f=op->imm.f; }
void FR2CI_OpCall_CopyArgDC(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].d=op->imm.d; }
void FR2CI_OpCall_CopyArgPC(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].p=op->imm.p; }

void FR2CI_OpCall_CopyArgIG(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].i=((FR2CI_Value *)op->imm.p)[op->s].i; }
void FR2CI_OpCall_CopyArgLG(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].l=((FR2CI_Value *)op->imm.p)[op->s].l; }
void FR2CI_OpCall_CopyArgFG(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].f=((FR2CI_Value *)op->imm.p)[op->s].f; }
void FR2CI_OpCall_CopyArgDG(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].d=((FR2CI_Value *)op->imm.p)[op->s].d; }
void FR2CI_OpCall_CopyArgPG(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].p=((FR2CI_Value *)op->imm.p)[op->s].p; }
void FR2CI_OpCall_CopyArgAG(
		FR2CI_Frame *frms, FR2CI_Frame *frmd, FR2CI_ArgOpcode *op)
	{ frmd->arg[op->d].p=&(((FR2CI_Value *)op->imm.p)[op->s]); }

void FR2CI_DecodeOpCallArgs(
	FR2CI_Image *img, int na, FR2CI_CallInfo *cinf, byte **rcs)
{
	FR2CI_ArgOpcode *op;
	byte *cs;
	int i, j;
	
	cinf->oparg=FR2CI_ImagePoolAlloc(img, na*sizeof(FR2CI_ArgOpcode *));
	cinf->n_oparg=na;
	
	cs=*rcs;
	for(i=0; i<na; i++)
	{
		op=FR2CI_AllocArgOpcode(img);
		cinf->oparg[i]=op;
		
		op->run=NULL;
		switch(((*cs)>>4)&15)
		{
		case 0:
			op->run=FR2CI_OpCall_CopyArgIR;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 1:
			op->run=FR2CI_OpCall_CopyArgLR;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 2:
			op->run=FR2CI_OpCall_CopyArgFR;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 3:
			op->run=FR2CI_OpCall_CopyArgDR;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 4:
			op->run=FR2CI_OpCall_CopyArgPR;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 5:
			op->run=FR2CI_OpCall_CopyArgIL;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 6:
			op->run=FR2CI_OpCall_CopyArgLL;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 7:
			op->run=FR2CI_OpCall_CopyArgFL;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 8:
			op->run=FR2CI_OpCall_CopyArgDL;
			op->d=i; op->s=(*cs++)&15;
			break;
		case 9:
			op->run=FR2CI_OpCall_CopyArgPL;
			op->d=i; op->s=(*cs++)&15;
			break;
		default:
			break;
		}
		if(op->run)
			continue;

		switch(*cs)
		{
		case 0xA0:
			cs++; op->run=FR2CI_OpCall_CopyArgIR;
			op->d=i; op->s=*cs++; break;
		case 0xA1:
			cs++; op->run=FR2CI_OpCall_CopyArgLR;
			op->d=i; op->s=*cs++; break;
		case 0xA2:
			cs++; op->run=FR2CI_OpCall_CopyArgFR;
			op->d=i; op->s=*cs++; break;
		case 0xA3:
			cs++; op->run=FR2CI_OpCall_CopyArgDR;
			op->d=i; op->s=*cs++; break;
		case 0xA4:
			cs++; op->run=FR2CI_OpCall_CopyArgPR;
			op->d=i; op->s=*cs++; break;
		case 0xA5:
			cs++; op->run=FR2CI_OpCall_CopyArgIA;
			op->d=i; op->s=*cs++; break;
		case 0xA6:
			cs++; op->run=FR2CI_OpCall_CopyArgLA;
			op->d=i; op->s=*cs++; break;
		case 0xA7:
			cs++; op->run=FR2CI_OpCall_CopyArgFA;
			op->d=i; op->s=*cs++; break;
		case 0xA8:
			cs++; op->run=FR2CI_OpCall_CopyArgDA;
			op->d=i; op->s=*cs++; break;
		case 0xA9:
			cs++; op->run=FR2CI_OpCall_CopyArgPA;
			op->d=i; op->s=*cs++; break;
		case 0xAA:
			cs++; op->run=FR2CI_OpCall_CopyArgIL;
			op->d=i; op->s=*cs++; break;
		case 0xAB:
			cs++; op->run=FR2CI_OpCall_CopyArgLL;
			op->d=i; op->s=*cs++; break;
		case 0xAC:
			cs++; op->run=FR2CI_OpCall_CopyArgFL;
			op->d=i; op->s=*cs++; break;
		case 0xAD:
			cs++; op->run=FR2CI_OpCall_CopyArgDL;
			op->d=i; op->s=*cs++; break;
		case 0xAE:
			cs++; op->run=FR2CI_OpCall_CopyArgPL;
			op->d=i; op->s=*cs++; break;
		case 0xAF:
			cs++; op->run=FR2CI_OpCall_CopyArgAL;
			op->d=i; op->s=*cs++; break;
		case 0xB0:
			cs++; op->run=FR2CI_OpCall_CopyArgIC;
			op->d=i; op->imm.i=(signed char)(*cs++); break;
		case 0xB1:
			cs++; op->run=FR2CI_OpCall_CopyArgLC;
			op->d=i; op->imm.l=(signed char)(*cs++); break;
		case 0xB2:
			cs++; op->run=FR2CI_OpCall_CopyArgIC;
			j=(cs[0]<<8)|(cs[1]); cs+=2;
			op->d=i; op->imm.i=(short)j;
			break;
		case 0xB3:
			cs++; op->run=FR2CI_OpCall_CopyArgLC;
			j=(cs[0]<<8)|(cs[1]); cs+=2;
			op->d=i; op->imm.l=(short)j;
			break;

		case 0xB4:
			cs++; op->run=FR2CI_OpCall_CopyArgIC;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->imm.i=FR2CI_GetImageLiteralInt32(img, j);
			break;
		case 0xB5:
			cs++; op->run=FR2CI_OpCall_CopyArgLC;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->imm.l=FR2CI_GetImageLiteralInt64(img, j);
			break;
		case 0xB6:
			cs++; op->run=FR2CI_OpCall_CopyArgFC;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->imm.f=FR2CI_GetImageLiteralFloat32(img, j);
			break;
		case 0xB7:
			cs++; op->run=FR2CI_OpCall_CopyArgDC;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->imm.d=FR2CI_GetImageLiteralFloat64(img, j);
			break;
		case 0xB8:
			cs++; op->run=FR2CI_OpCall_CopyArgPC;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->imm.p=FR2CI_GetImageLiteralPointer(img, j);
			break;

		case 0xB9:
			cs++; op->run=FR2CI_OpCall_CopyArgIG;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->s=j; op->imm.p=img->gbl;
			break;
		case 0xBA:
			cs++; op->run=FR2CI_OpCall_CopyArgLG;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->s=j; op->imm.p=img->gbl;
			break;
		case 0xBB:
			cs++; op->run=FR2CI_OpCall_CopyArgFG;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->s=j; op->imm.p=img->gbl;
			break;
		case 0xBC:
			cs++; op->run=FR2CI_OpCall_CopyArgDG;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->s=j; op->imm.p=img->gbl;
			break;
		case 0xBD:
			cs++; op->run=FR2CI_OpCall_CopyArgPG;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->s=j; op->imm.p=img->gbl;
			break;
		case 0xBE:
			cs++; op->run=FR2CI_OpCall_CopyArgAG;
			j=(cs[0]<<16)|(cs[1]<<8)|(cs[2]); cs+=3;
			op->d=i; op->s=j; op->imm.p=img->gbl;
			break;
		default:
			break;
		}
	}
	*rcs=cs;
}

FR2CI_Trace *FR2CI_Op_CallG(FR2CI_Frame *frm, FR2CI_TailOpcode *op)
{
	FR2CI_Context *ctx;
	FR2CI_CallInfo *cinf;
	FR2CI_Trace *tr2;
	FR2CI_Frame *frm2;
	FR2CI_ArgOpcode *oparg;
	int i;

	cinf=op->imm.p;
	tr2=FR2CI_GetFunctionEntryTrace(cinf->func);
	if(!tr2)
	{
		frm->ctx->trap_trace=op->trace;
		frm->ctx->trap_opcode=NULL;
		frm->ctx->trap_tailop=op;
		frm->ctx->status=cinf->func->image->status;
		return(NULL);
	}

	frm->rettrace=op->nexttrace;

	ctx=frm->ctx;
	frm2=FR2CI_AllocFrame(ctx);
	ctx->framestack[ctx->framestackpos++]=frm;
	ctx->frame=frm2;

	FR2CI_SetupFrameArgs(ctx, frm2, cinf->n_arg);
	FR2CI_SetupFrameRegs(ctx, frm2, cinf->func->nregs);
	FR2CI_SetupFrameLocals(ctx, frm2, cinf->func->nlocals);

	for(i=0; i<cinf->n_oparg; i++)
	{
		oparg=cinf->oparg[i];
		oparg->run(frm, frm2, oparg);
	}
	
	return(tr2);
}

FR2CI_Trace *FR2CI_Op_CallPG(FR2CI_Frame *frm, FR2CI_TailOpcode *op)
{
	FR2CI_Context *ctx;
	FR2CI_CallInfo *cinf;
	FR2CI_Function *func;
	FR2CI_Trace *tr2;
	FR2CI_Frame *frm2;
	FR2CI_ArgOpcode *oparg;
	int i;

	switch(op->t)
	{
	case FR2C_OPMODE_RR:
		func=frm->reg[op->s].p;
		break;
	case FR2C_OPMODE_RA:
		func=frm->arg[op->s].p;
		break;
	case FR2C_OPMODE_RL:
		func=frm->loc[op->s].p;
		break;
	default:
		func=NULL;
		break;
	}

	cinf=op->imm.p;
	tr2=FR2CI_GetFunctionEntryTrace(func);
	if(!tr2)
	{
		frm->ctx->trap_trace=op->trace;
		frm->ctx->trap_opcode=NULL;
		frm->ctx->trap_tailop=op;
		frm->ctx->status=func->image->status;
		return(NULL);
	}

	frm->rettrace=op->nexttrace;

	ctx=frm->ctx;
	frm2=FR2CI_AllocFrame(ctx);
	ctx->framestack[ctx->framestackpos++]=frm;
	ctx->frame=frm2;

	FR2CI_SetupFrameArgs(ctx, frm2, cinf->n_arg);
	FR2CI_SetupFrameRegs(ctx, frm2, func->nregs);
	FR2CI_SetupFrameLocals(ctx, frm2, func->nlocals);

	for(i=0; i<cinf->n_oparg; i++)
	{
		oparg=cinf->oparg[i];
		oparg->run(frm, frm2, oparg);
	}
	
	return(tr2);
}

FR2CI_TailOpcode *FR2CI_DecodeOpCall(
	FR2CI_Image *img, int gbl, int na, byte **rcs)
{
	FR2CI_Function *func;
	FR2CI_CallInfo *cinf;
	FR2CI_TailOpcode *tmp;	

	func=FR2CI_ImageGetFunction(img, gbl);
	if(!func)
	{
		img->status=FR2C_STATUS_ERR_CALLNOFUNC;
		return(NULL);
	}

	cinf=FR2CI_AllocCallInfo(img);
	cinf->func=func;
	FR2CI_DecodeOpCallArgs(img, na, cinf, rcs);

	tmp=FR2CI_AllocTailOpcode(img);
	tmp->s=gbl;
	tmp->imm.p=cinf;
	tmp->run=FR2CI_Op_CallG;	
	return(tmp);
}

FR2CI_TailOpcode *FR2CI_DecodeOpCallP(
	FR2CI_Image *img, int y, int s, int na, byte **rcs)
{
	FR2CI_Function *func;
	FR2CI_CallInfo *cinf;
	FR2CI_TailOpcode *tmp;	

//	func=FR2CI_ImageGetFunction(img, gbl);
//	if(!func)
//	{
//		img->status=FR2C_STATUS_ERR_CALLNOFUNC;
//		return(NULL);
//	}

	cinf=FR2CI_AllocCallInfo(img);
//	cinf->func=func;
	FR2CI_DecodeOpCallArgs(img, na, cinf, rcs);

	tmp=FR2CI_AllocTailOpcode(img);
	tmp->s=s;
	tmp->t=y;
	tmp->imm.p=cinf;
	tmp->run=FR2CI_Op_CallPG;	
	return(tmp);
}


FR2CI_Trace *FR2CI_Op_RetV(FR2CI_Frame *frm, FR2CI_TailOpcode *op)
{
	FR2CI_Context *ctx;
	FR2CI_Frame *frm2;
	
	ctx=frm->ctx;
	if(ctx->framestackpos>0)
	{
		ctx->framestackpos--;
		frm2=ctx->framestack[ctx->framestackpos];
		ctx->frame=frm2;
		FR2CI_CleanupFrame(ctx, frm);
		FR2CI_FreeFrame(ctx, frm);
		return(frm2->rettrace);
//		return(op->nexttrace);
	}
	
	FR2CI_CleanupFrame(ctx, frm);
	FR2CI_FreeFrame(ctx, frm);
	ctx->frame=NULL;
	ctx->status=FR2C_STATUS_NO;
	return(NULL);
}

FR2CI_TailOpcode *FR2CI_MakeOpRetV(
	FR2CI_Image *img)
{
	FR2CI_TailOpcode *tmp;	
	tmp=FR2CI_AllocTailOpcode(img);
	tmp->run=FR2CI_Op_RetV;	
	return(tmp);
}

#define OP_RET(OPN, YYN, SN, TY)							\
	FR2CI_Trace *FR2CI_Op_##OPN##_##YYN(					\
		FR2CI_Frame *frm, FR2CI_TailOpcode *op)				\
	{														\
		FR2CI_Context *ctx;									\
		FR2CI_Frame *frm2;									\
		ctx=frm->ctx;										\
		ctx->retval.TY=SN[op->s].TY;						\
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
	}														\

#define OPB_RET(OPN) \
	OP_RET(OPN, IR, frm->reg, i) \
	OP_RET(OPN, IA, frm->arg, i) \
	OP_RET(OPN, IL, frm->loc, i) \
	OP_RET(OPN, LR, frm->reg, l) \
	OP_RET(OPN, LA, frm->arg, l) \
	OP_RET(OPN, LL, frm->loc, l) \
	OP_RET(OPN, FR, frm->reg, f) \
	OP_RET(OPN, FA, frm->arg, f) \
	OP_RET(OPN, FL, frm->loc, f) \
	OP_RET(OPN, DR, frm->reg, d) \
	OP_RET(OPN, DA, frm->arg, d) \
	OP_RET(OPN, DL, frm->loc, d) \
	OP_RET(OPN, PR, frm->reg, p) \
	OP_RET(OPN, PA, frm->arg, p) \
	OP_RET(OPN, PL, frm->loc, p)

OPB_RET(Ret)

FR2CI_TailOpcode *FR2CI_MakeOpRet(
	FR2CI_Image *img, int w, int s)
{
	FR2CI_TailOpcode *tmp;	

	tmp=FR2CI_AllocTailOpcode(img);
	tmp->s=s;

	OPB_SWNW(Ret);

	if(!tmp->run)
	{
		FR2CI_FreeTailOpcode(img, tmp);
		return(NULL);
	}
	
	return(tmp);
}
