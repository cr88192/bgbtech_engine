#include <bgbfrir.h>

#include <time.h>

int main(int argc, char *argv[])
{
	FRIR_Method *tmp;
	FRIR_Context *ctx;
	int i, t0, t1;

	t0=clock();
	for(i=0; i<1000000000; i++);
	t1=clock();
	
	printf("Native Dt=%d\n", t1-t0);
	
	tmp=FRIR_AllocMethod();
	FRIR_EmitOpCI(tmp, FRIR_SOP_MOV_IC, 1, 1000000000);
	FRIR_EmitLabel(tmp, 1);
//	FRIR_EmitOpCI(tmp, FRIR_SOP_MOV_IC, 2, 1);
//	FRIR_EmitOpCAB(tmp, FRIR_SOP_SUB_I, 1, 1, 2);
	FRIR_EmitOpCAI(tmp, FRIR_SOP_SUB_IC, 1, 1, 1);
	FRIR_EmitOpJccAI(tmp, FRIR_SOP_JMP_GT_IC, 1, 0, 1);
//	FRIR_EmitOpV(tmp, FRIR_SOP_RETV);
//	FRIR_EndTrace(tmp);
	FRIR_EmitRet(tmp, 1);
	
	t0=clock();
	ctx=FRIR_AllocContext();
	FRIR_Thread_ApplyMethod(ctx, tmp, NULL, 0);
	FRIR_Thread_RunStep(ctx);
	t1=clock();
	
	printf("Dt=%d\n", t1-t0);
	fgetc(stdin);
}
