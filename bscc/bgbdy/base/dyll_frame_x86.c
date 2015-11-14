#include <bgbgc.h>
#include <bgbdy.h>

extern int dyll_frame_sizes[48];
extern int dyll_frame_sizes_pad[48];
extern int dyll_frame_counts[48];


#ifdef __i386__

void BGBDY_Frames_EmitLock(int n)
{
	dyllAsmPrint(".lock:\n");
	dyllAsmPrint("push eax\n");
	dyllAsmPrint("push ecx\n");
#ifndef linux
	dyllAsmPrint("push ebx\n");
#endif
	dyllAsmPrint(".lock_l0:\n");
	dyllAsmPrint(
		"mov dl, 1\n"
		"lock cmpxchg [dyll_frame_lock], dl\n"
		"jz .lock_l1\n");

#ifdef WIN32
	dyllAsmPrint("push 0\ncall Sleep@4\n");
#endif
#ifdef linux
	dyllAsmPrint("push 0\ncall usleep\npop eax\n");
#endif
	dyllAsmPrint("jmp .lock_l0\n");
	dyllAsmPrint(".lock_l1:\n");
#ifndef linux
	dyllAsmPrint("pop ebx\n");
#endif
	dyllAsmPrint("pop ecx\n");
	dyllAsmPrint("pop eax\n");
	dyllAsmPrint("jmp .lock_ok\n");
}

void BGBDY_Frames_EmitAlloc(int n)
{
	dyllAsmPrint("xframe_AllocFrame%d:\n", n);
	dyllAsmPrint("push edx\n");
	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jnz .lock\n");
	dyllAsmPrint(".lock_ok:\n");
	dyllAsmPrint("mov eax, [dyll_frame_list+%d]\n", n*4);
	dyllAsmPrint("and eax, eax\njz .alloc\n");
	dyllAsmPrint("mov edx, [eax]\n");
	dyllAsmPrint("mov [dyll_frame_list+%d], edx\n", n*4);

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop edx\n");
	dyllAsmPrint("ret\n");


	dyllAsmPrint(".alloc:\n");
	dyllAsmPrint("push %d\n",
		dyll_frame_counts[n]*dyll_frame_sizes_pad[n]*4);
	dyllAsmPrint("call gcalloc\n");
	dyllAsmPrint("pop edx\n");

	dyllAsmPrint("xor edx, edx\n");
	dyllAsmPrint("mov ecx, %d\n", dyll_frame_counts[n]);
	dyllAsmPrint(".alloc_l0:\n");
	dyllAsmPrint("mov [eax], edx\n");
	dyllAsmPrint("mov edx, eax\n");
	dyllAsmPrint("add eax, %d\n", dyll_frame_sizes_pad[n]*4);
	dyllAsmPrint("loop .alloc_l0\n");

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("ret\n");

	BGBDY_Frames_EmitLock(n);
}

void BGBDY_Frames_EmitFree(int n)
{
	dyllAsmPrint("xframe_FreeFrame%d:\n", n);
	dyllAsmPrint("push edx\n");
	dyllAsmPrint(
		"mov dl, 1\n"
		"lock cmpxchg [dyll_frame_lock], dl\n"
		"jnz .lock\n");
	dyllAsmPrint(".lock_ok:\n");
//	dyllAsmPrint("mov eax, [esp+4]\n");
	dyllAsmPrint("mov edx, [dyll_frame_list+%d]\n", n*4);
	dyllAsmPrint("mov [eax], edx\n");
	dyllAsmPrint("mov [dyll_frame_list+%d], eax\n", n*4);

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop edx\n");
	dyllAsmPrint("ret\n");

	BGBDY_Frames_EmitLock(n);
}

void BGBDY_Frames_EmitAllocLocals(int n)
{
	dyllAsmPrint("xframe_AllocLocals%d:\n", n);
	dyllAsmPrint("push edx\n");
	dyllAsmPrint("call xframe_AllocFrame%d\n", n);
	dyllAsmPrint("mov edx, [ebp+12]\n");
	dyllAsmPrint("mov [eax+0], edx\n");
	dyllAsmPrint("mov [ebp+12], eax\n");
	dyllAsmPrint("pop edx\n");
	dyllAsmPrint("ret\n");
}

void BGBDY_Frames_EmitReturnZ()
{
	dyllAsmPrint("xframe_ReturnZ:\n");
	dyllAsmPrint("mov eax, [ebp+4]\n");
	dyllAsmPrint("mov ebp, [ebp+0]\n");
	dyllAsmPrint("jmp eax\n");
}

void BGBDY_Frames_EmitReturn(int n)
{
	dyllAsmPrint("xframe_Return%d:\n", n);
	dyllAsmPrint("mov eax, [ebp+12]\n");
	dyllAsmPrint("mov edx, [eax+0]\n");
	dyllAsmPrint("mov [ebp+12], edx\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);

	dyllAsmPrint("mov eax, [ebp+16]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", 3);

	dyllAsmPrint("mov eax, [ebp+4]\n");
	dyllAsmPrint("mov ebp, [ebp+0]\n");
	dyllAsmPrint("jmp eax\n");
}

void BGBDY_Frames_EmitCallN()
{
	dyllAsmPrint("xframe_CallN:\n");
	dyllAsmPrint("mov [esi+0], ebp\n");
	dyllAsmPrint("pop dword [esi+4]\n");

	dyllAsmPrint("mov eax, [ebp+8]\n");
	dyllAsmPrint("mov [esi+8], eax\n");

	dyllAsmPrint("xor eax, eax\n");
	dyllAsmPrint("mov [esi+12], eax\n");

	dyllAsmPrint("call xframe_AllocFrame%d\n", 3);
	dyllAsmPrint("mov edx, [ebp+16]\n");
	dyllAsmPrint("mov [eax+0], edx\n");
	dyllAsmPrint("mov [esi+16], eax\n");
	dyllAsmPrint("mov [eax+4], edi\n");

	dyllAsmPrint("mov ebp, esi\n");
	dyllAsmPrint("jmp dword [edi+36]\n");
}


void BGBDY_Frames_EmitCall_i(int n)
{
	dyllAsmPrint("xframe_Call%di:\n", n);
	dyllAsmPrint("xframe_Call%da:\n", n);
	dyllAsmPrint("mov eax, [ebp+12]\n");
	dyllAsmPrint("mov edx, [eax+0]\n");
	dyllAsmPrint("mov [ebp+12], edx\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);

	dyllAsmPrint("mov eax, [ebp+16]\n");
	dyllAsmPrint("mov edi, [eax+4]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", 3);

	dyllAsmPrint("mov eax, ebp\n");
	dyllAsmPrint("mov ecx, [ebp+4]\n");
	dyllAsmPrint("mov ebp, [ebp+0]\n");

	dyllAsmPrint("mov edx, [ebp+20]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("mov eax, edx\n");

	dyllAsmPrint("jmp ecx\n");
//	dyllAsmPrint("jmp dword [ebp+4]\n");
}

void BGBDY_Frames_EmitCall_l(int n)
{
	dyllAsmPrint("xframe_Call%di:\n", n);
	dyllAsmPrint("mov eax, [ebp+12]\n");
	dyllAsmPrint("mov edx, [eax+0]\n");
	dyllAsmPrint("mov [ebp+12], edx\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);

	dyllAsmPrint("mov eax, [ebp+16]\n");
	dyllAsmPrint("mov edi, [eax+4]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", 3);

	dyllAsmPrint("mov eax, ebp\n");
	dyllAsmPrint("mov ecx, [ebp+4]\n");
	dyllAsmPrint("mov ebp, [ebp+0]\n");

	dyllAsmPrint("push ecx\n");

	dyllAsmPrint("mov ecx, [ebp+20]\n");
	dyllAsmPrint("mov edx, [ebp+24]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("mov eax, ecx\n");

	dyllAsmPrint("ret\n");

//	dyllAsmPrint("pop ecx\n");
//	dyllAsmPrint("jmp ecx\n");

//	dyllAsmPrint("jmp dword [ebp+4]\n");
}

void BGBDY_Frames_EmitCall_f(int n)
{
	dyllAsmPrint("xframe_Call%di:\n", n);
	dyllAsmPrint("mov eax, [ebp+12]\n");
	dyllAsmPrint("mov edx, [eax+0]\n");
	dyllAsmPrint("mov [ebp+12], edx\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("mov eax, [ebp+16]\n");
	dyllAsmPrint("mov edi, [eax+4]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", 3);
	dyllAsmPrint("mov eax, ebp\n");
	dyllAsmPrint("mov ecx, [ebp+4]\n");
	dyllAsmPrint("mov ebp, [ebp+0]\n");
	dyllAsmPrint("fld32 [ebp+20]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("jmp ecx\n");
}

void BGBDY_Frames_EmitCall_d(int n)
{
	dyllAsmPrint("xframe_Call%di:\n", n);
	dyllAsmPrint("mov eax, [ebp+12]\n");
	dyllAsmPrint("mov edx, [eax+0]\n");
	dyllAsmPrint("mov [ebp+12], edx\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("mov eax, [ebp+16]\n");
	dyllAsmPrint("mov edi, [eax+4]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", 3);
	dyllAsmPrint("mov eax, ebp\n");
	dyllAsmPrint("mov ecx, [ebp+4]\n");
	dyllAsmPrint("mov ebp, [ebp+0]\n");
	dyllAsmPrint("fld64 [ebp+20]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("jmp ecx\n");
}

void BGBDY_Frames_EmitCall_x(int n)
{
	dyllAsmPrint("xframe_Call%di:\n", n);
	dyllAsmPrint("mov eax, [ebp+12]\n");
	dyllAsmPrint("mov edx, [eax+0]\n");
	dyllAsmPrint("mov [ebp+12], edx\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("mov eax, [ebp+16]\n");
	dyllAsmPrint("mov edi, [eax+4]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", 3);
	dyllAsmPrint("mov eax, ebp\n");
	dyllAsmPrint("mov ecx, [ebp+4]\n");
	dyllAsmPrint("mov ebp, [ebp+0]\n");
	dyllAsmPrint("movdqu xmm0, [ebp+20]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);
	dyllAsmPrint("jmp ecx\n");
}


void BGBDY_Frames_EmitCall(int n)
{
	BGBDY_Frames_EmitCall_i(n);
	BGBDY_Frames_EmitCall_l(n);
	BGBDY_Frames_EmitCall_f(n);
	BGBDY_Frames_EmitCall_d(n);
	BGBDY_Frames_EmitCall_x(n);
}

#endif

