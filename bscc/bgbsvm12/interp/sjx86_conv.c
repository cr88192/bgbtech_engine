#include <bgbsvm.h>

void BSVM_ThJitX86_Conv_RefToDouble(BSVM_ThreadJitContext *ctx)
{
	char *s0, *s1, *s2;

	s0=dyllGenSym();
	s1=dyllGenSym();
//	s2=dyllGenSym();

	dyllAsmPrint("mov ecx, edx\n");
	dyllAsmPrint("shr ecx, 28\n");
	dyllAsmPrint("sub ecx, 3\n");
	dyllAsmPrint("and ecx, 6\n");
	dyllAsmPrint("jnz %s\n", s0);
	//exact conversion...
	dyllAsmPrint("mov [esp+0], eax\n");
	dyllAsmPrint("mov [esp+4], edx\n");
	dyllAsmPrint("movsd xmm0, [esp+0]\n");
	dyllAsmPrint("jmp %s\n", s1);
	//assume flonum2
	dyllAsmPrint("%s:\n", s0);
	dyllAsmPrint("shld edx, eax, 4\n");
	dyllAsmPrint("mov [esp+0], eax\n");
	dyllAsmPrint("mov [esp+4], edx\n");
	dyllAsmPrint("movsd xmm0, [esp+0]\n");
	dyllAsmPrint("%s:\n", s1);
}

void BSVM_ThJitX86_Conv_DoubleToRef(BSVM_ThreadJitContext *ctx)
{
	char *s0, *s1, *s2;

	s0=dyllGenSym();
//	s1=dyllGenSym();
//	s2=dyllGenSym();

	dyllAsmPrint("movsd [esp+0], xmm0\n");
	dyllAsmPrint("mov eax, [esp+0]\n");
	dyllAsmPrint("mov edx, [esp+4]\n");

	dyllAsmPrint("mov ecx, edx\n");
	dyllAsmPrint("shr ecx, 28\n");
	dyllAsmPrint("sub ecx, 3\n");
	dyllAsmPrint("and ecx, 6\n");
	dyllAsmPrint("jz %s\n", s0);
	dyllAsmPrint("shrd eax, edx, 4\n");
	dyllAsmPrint("or edx, 0x80000000\n");
	dyllAsmPrint("%s:\n", s0);
}

void BSVM_ThJitX86_Conv_RefToFloat(BSVM_ThreadJitContext *ctx)
{
	BSVM_ThJitX86_Conv_RefToDouble(ctx);
	dyllAsmPrint("cvtsd2ss xmm0, xmm0\n");
}

void BSVM_ThJitX86_Conv_FloatToRef(BSVM_ThreadJitContext *ctx)
{
	dyllAsmPrint("cvtss2sd xmm0, xmm0\n");
	BSVM_ThJitX86_Conv_DoubleToRef(ctx);
}
