#include <bgbsvm.h>

void J2ArithX86_MovReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
		{ bsj2_print(ctx, "mov %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "movss %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "movsd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "movps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "movpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2ArithX86_AddReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "add %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "add %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
		{ bsj2_print(ctx, "add %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "addss %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "addsd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "addps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "addpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2ArithX86_SubReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "sub %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "sub %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
		{ bsj2_print(ctx, "sub %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "subss %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "subsd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "subps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "subpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2ArithX86_MulReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "imul %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "imul %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
		{ bsj2_print(ctx, "imul %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "mulss %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "mulsd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "mulps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "mulpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2ArithX86_DivReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
//	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
//		{ bsj2_print(ctx, "imul %s, %s\n", dname, sname); return; }
//	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
//		{ bsj2_print(ctx, "imul %s, %s\n", dname, sname); return; }
//	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
//		{ bsj2_print(ctx, "imul %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "divss %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "divsd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "divps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "divpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2ArithX86_AndReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "and %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "and %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
		{ bsj2_print(ctx, "and %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "andps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "andpd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "andps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "andpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2ArithX86_OrReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "or %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "or %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
		{ bsj2_print(ctx, "or %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "orps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "orpd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "orps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "orpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}

void J2ArithX86_XorReg(bsjContext ctx, bsjReg dst, bsjReg src, bsjType ty)
{
	char *dname, *sname;
	
	dname=BSVM_J2Reg_GetRegName(dst);
	sname=BSVM_J2Reg_GetRegName(src);
	
	if(J2RegX86_DWordP(dst) && J2RegX86_DWordP(src))
		{ bsj2_print(ctx, "xor %s, %s\n", dname, sname); return; }
	if(J2RegX86_QWordP(dst) && J2RegX86_QWordP(src))
		{ bsj2_print(ctx, "xor %s, %s\n", dname, sname); return; }
	if(J2RegX86_WordP(dst) && J2RegX86_WordP(src))
		{ bsj2_print(ctx, "xor %s, %s\n", dname, sname); return; }

	if(J2RegX86_XMMSSP(dst) && J2RegX86_XMMSSP(src))
		{ bsj2_print(ctx, "xorps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMSDP(dst) && J2RegX86_XMMSDP(src))
		{ bsj2_print(ctx, "xorpd %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPSP(dst) && J2RegX86_XMMPSP(src))
		{ bsj2_print(ctx, "xorps %s, %s\n", dname, sname); return; }
	if(J2RegX86_XMMPDP(dst) && J2RegX86_XMMPDP(src))
		{ bsj2_print(ctx, "xorpd %s, %s\n", dname, sname); return; }
		
	BSVM_Jit2_CaseError(ctx);
}
