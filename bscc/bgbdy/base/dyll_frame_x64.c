#include <bgbgc.h>
#include <bgbdy.h>

extern int dyll_frame_sizes[48];
extern int dyll_frame_sizes_pad[48];
extern int dyll_frame_counts[48];


#ifdef __x86_64__

#ifdef WIN64

void BGBDY_Frames_EmitAlloc(int n)
{
	//NOTE: RBP pushed to re-align stack

	dyllAsmPrint("dyll_AllocFrame%d:\n", n);
	dyllAsmPrint("push rbp\nmov rbp, rsp\n");

	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jnz .lock\n");
	dyllAsmPrint(".lock_ok:\n");
	dyllAsmPrint("mov rax, [dyll_frame_list+%d]\n", n*4);
	dyllAsmPrint("and rax, rax\njz .alloc\n");
	dyllAsmPrint("mov rdx, [rax]\n");
	dyllAsmPrint("mov [dyll_frame_list+%d], rdx\n", n*4);

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop rbp\n");
	dyllAsmPrint("ret\n");


	dyllAsmPrint(".lock:\n");
	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jz .lock_ok\n");

	dyllAsmPrint("xor rcx, rcx\ncall Sleep\n");
	dyllAsmPrint("jmp .lock\n");


	dyllAsmPrint(".alloc:\n");
	dyllAsmPrint("mov rcx, %d\n",
		dyll_frame_counts[n]*dyll_frame_sizes_pad[n]*4);
	dyllAsmPrint("call gcalloc\n");

	dyllAsmPrint("xor rdx, rdx\n");
	dyllAsmPrint("mov rcx, %d\n", dyll_frame_counts[n]);
	dyllAsmPrint(".alloc_l0:\n");
	dyllAsmPrint("mov [rax], rdx\n");
	dyllAsmPrint("mov rdx, rax\n");
	dyllAsmPrint("add rax, %d\n", dyll_frame_sizes_pad[n]*4);
	dyllAsmPrint("loop .alloc_l0\n");

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop rbp\n");
	dyllAsmPrint("ret\n");
}

void BGBDY_Frames_EmitFree(int n)
{
	dyllAsmPrint("dyll_FreeFrame%d:\n", n);
	dyllAsmPrint("push rbp\nmov rbp, rsp\n");

	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jnz .lock\n");
	dyllAsmPrint(".lock_ok:\n");
	dyllAsmPrint("mov rax, [dyll_frame_list+%d]\n", n*4);
	dyllAsmPrint("mov [rcx], rax\n");
	dyllAsmPrint("mov [dyll_frame_list+%d], rcx\n", n*4);

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop rbp\n");
	dyllAsmPrint("ret\n");


	dyllAsmPrint(".lock:\n");
	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jz .lock_ok\n");
	dyllAsmPrint("xor rcx, rcx\ncall Sleep\n");
	dyllAsmPrint("jmp .lock\n");

}

#elif defined(__x86_64__)

void BGBDY_Frames_EmitAlloc(int n)
{
	//NOTE: RBP pushed to re-align stack

	dyllAsmPrint("dyll_AllocFrame%d:\n", n);
	dyllAsmPrint("push rbp\nmov rbp, rsp\n");

	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jnz .lock\n");
	dyllAsmPrint(".lock_ok:\n");
	dyllAsmPrint("mov rax, [dyll_frame_list+%d]\n", n*4);
	dyllAsmPrint("and rax, rax\njz .alloc\n");
	dyllAsmPrint("mov rdx, [rax]\n");
	dyllAsmPrint("mov [dyll_frame_list+%d], rdx\n", n*4);

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop rbp\n");
	dyllAsmPrint("ret\n");


	dyllAsmPrint(".lock:\n");
	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jz .lock_ok\n");

	dyllAsmPrint("xor rdi, rdi\ncall usleep\n");
	dyllAsmPrint("jmp .lock\n");


	dyllAsmPrint(".alloc:\n");
	dyllAsmPrint("mov rdi, %d\n",
		dyll_frame_counts[n]*dyll_frame_sizes_pad[n]*4);
	dyllAsmPrint("call gcalloc\n");

	dyllAsmPrint("xor rdx, rdx\n");
	dyllAsmPrint("mov rcx, %d\n", dyll_frame_counts[n]);
	dyllAsmPrint(".alloc_l0:\n");
	dyllAsmPrint("mov [rax], rdx\n");
	dyllAsmPrint("mov rdx, rax\n");
	dyllAsmPrint("add rax, %d\n", dyll_frame_sizes_pad[n]*4);
	dyllAsmPrint("loop .alloc_l0\n");

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop rbp\n");
	dyllAsmPrint("ret\n");
}

void BGBDY_Frames_EmitFree(int n)
{
	dyllAsmPrint("dyll_FreeFrame%d:\n", n);
	dyllAsmPrint("push rbp\nmov rbp, rsp\n");

	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jnz .lock\n");
	dyllAsmPrint(".lock_ok:\n");
	dyllAsmPrint("mov rax, [dyll_frame_list+%d]\n", n*4);
	dyllAsmPrint("mov [rdi], rax\n");
	dyllAsmPrint("mov [dyll_frame_list+%d], rdi\n", n*4);

	dyllAsmPrint("xor dl, dl\nlock mov [dyll_frame_lock], dl\n");
	dyllAsmPrint("pop rbp\n");
	dyllAsmPrint("ret\n");


	dyllAsmPrint(".lock:\n");
	dyllAsmPrint(
		"mov al, 1\n"
		"lock cmpxchg [dyll_frame_lock], al\n"
		"jz .lock_ok\n");
	dyllAsmPrint("xor rdi, rdi\ncall usleep\n");
	dyllAsmPrint("jmp .lock\n");

}
#endif


void BGBDY_Frames_EmitAllocLocals(int n)
{
	dyllAsmPrint("xframe_AllocLocals%d:\n", n);
	dyllAsmPrint("push rdx\n");
	dyllAsmPrint("call xframe_AllocFrame%d\n", n);
	dyllAsmPrint("mov rdx, [rbp+24]\n");
	dyllAsmPrint("mov [rax+0], rdx\n");
	dyllAsmPrint("mov [rbp+24], rax\n");
	dyllAsmPrint("pop rdx\n");
	dyllAsmPrint("ret\n");
}

void BGBDY_Frames_EmitReturnZ()
{
	dyllAsmPrint("xframe_ReturnZ:\n");
	dyllAsmPrint("mov rax, [rbp+8]\n");
	dyllAsmPrint("mov rbp, [rbp+0]\n");
	dyllAsmPrint("jmp rax\n");
}

void BGBDY_Frames_EmitReturn(int n)
{
	dyllAsmPrint("xframe_Return%d:\n", n);
	dyllAsmPrint("mov rax, [rbp+24]\n");
	dyllAsmPrint("mov rdx, [rax+0]\n");
	dyllAsmPrint("mov [rbp+24], rdx\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", n);

	dyllAsmPrint("mov rax, [rbp+32]\n");
	dyllAsmPrint("call xframe_FreeFrame%d\n", 3);

	dyllAsmPrint("mov rax, [rbp+8]\n");
	dyllAsmPrint("mov rbp, [rbp+0]\n");
	dyllAsmPrint("jmp rax\n");
}

void BGBDY_Frames_EmitCallN()
{
	dyllAsmPrint("xframe_CallN:\n");
	dyllAsmPrint("mov [rsi+0], ebp\n");
	dyllAsmPrint("pop qword [rsi+8]\n");

	dyllAsmPrint("mov rax, [rbp+16]\n");
	dyllAsmPrint("mov [rsi+16], rax\n");

	dyllAsmPrint("xor rax, rax\n");
	dyllAsmPrint("mov [rsi+24], rax\n");

	dyllAsmPrint("call xframe_AllocFrame%d\n", 3);
	dyllAsmPrint("mov rdx, [rbp+32]\n");
	dyllAsmPrint("mov [rax+0], rdx\n");
	dyllAsmPrint("mov [rsi+32], rax\n");
	dyllAsmPrint("mov [rax+8], rdi\n");

	dyllAsmPrint("mov rbp, rsi\n");
	dyllAsmPrint("jmp dword [rdi+64]\n");
}

#endif
