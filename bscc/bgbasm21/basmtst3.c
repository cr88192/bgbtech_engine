#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <bgbasm.h>
#include <bgbasmx.h>

/* Autogenerated source */
char *sxil_asm_int128=
// "#ifdef x86\n\n"
"[section .text]\n"
"[cpu x86]\n"
"[bits 32]\n\n"

"sxil_abs_i128:\n"
"cmp dword [esi+12], 0; jge .l0\nnot dword [esi+0]; not dword [esi+4]\n"
"not dword [esi+8]; not dword [esi+12]\n"
"add dword [esi+0], 1; adc dword [esi+4], 0\n"
"adc dword [esi+8], 0; adc dword [esi+12], 0\n.l0: ret\n\n"
"sxil_abs_i128_2:\ncmp dword [esi+28], 0; jge .l0\n"
"not dword [esi+16]; not dword [esi+20]\n"
"not dword [esi+24]; not dword [esi+28]\n"
"add dword [esi+16], 1; adc dword [esi+20], 0\n"
"adc dword [esi+24], 0; adc dword [esi+28], 0\n.l0: ret\n\n"
"sxil_abs_i128_12:\ncmp dword [esi+12], 0; jge .l0\n"
"not dword [esi+0]; not dword [esi+4]\n"
"not dword [esi+8]; not dword [esi+12]\n"
"add dword [esi+0], 1; adc dword [esi+4], 0\n"
"adc dword [esi+8], 0; adc dword [esi+12], 0\n.l0:\n"
"cmp dword [esi+28], 0; jge .l1\n"
"not dword [esi+16]; not dword [esi+20]\n"
"not dword [esi+24]; not dword [esi+28]\n"
"add dword [esi+16], 1; adc dword [esi+20], 0\n"
"adc dword [esi+24], 0; adc dword [esi+28], 0\n.l1: ret\n\n"
";[esi+0]: dividand, [esi+16]: divisor\n"
";[esi+0]: quotient, [esi+16]: remainder\n\nsxil_div_i128_inner:\n"
"xor eax, eax\nmov [esp-4], eax; mov [esp-8], eax\n"
"mov [esp-12], eax; mov [esp-16], eax\nmov ecx, 128\n.l0:\n"
"shl dword [esi+0], 1; rcl dword [esi+4], 1\n"
"rcl dword [esi+8], 1; rcl dword [esi+12], 1\n"
"rcl dword [esp-16], 1; rcl dword [esp-12], 1\n"
"rcl dword [esp-8], 1; rcl dword [esp-4], 1\n"
"mov eax, [esp-4]; cmp eax, [esi+28]; jb .l1; ja .l2\n"
"mov eax, [esp-8]; cmp eax, [esi+24]; jb .l1; ja .l2\n"
"mov eax, [esp-12]; cmp eax, [esi+20]; jb .l1; ja .l2\n"
"mov eax, [esp-16]; cmp eax, [esi+16]; jb .l1\n.l2:\n"
"mov eax, [esi+16]; sub [esp-16], eax\n"
"mov eax, [esi+20]; sbb [esp-12], eax\n"
"mov eax, [esi+24]; sbb [esp-8], eax\n"
"mov eax, [esi+28]; sbb [esp-4], eax\n\n"
"add dword [esi+0], 1; adc dword [esi+4], 0\n"
"adc dword [esi+8], 0; adc dword [esi+12], 0\n\n"
".l1: sub ecx, 1; jnz .l0\n\nmov eax, [esp-16]; mov [esi+16], eax\n"
"mov eax, [esp-12]; mov [esi+20], eax\n"
"mov eax, [esp-8]; mov [esi+24], eax\n"
"mov eax, [esp-4]; mov [esi+28], eax\nret\n\nsxil_div_i128:\n"
"pushad; lea esi, [esp+36]\nmov edx, [esi+12]; xor edx, [esi+28]\n"
"call sxil_abs_i128_12\ncall sxil_div_i128_inner\nbt edx, 31; jnc .l0\n"
"not dword [esi+0]; not dword [esi+4]\n"
"not dword [esi+8]; not dword [esi+12]\n"
"add dword [esi+0], 1; adc dword [esi+4], 0\n"
"adc dword [esi+8], 0; adc dword [esi+12], 0\n.l0:\npopad; ret\n\n\n"
"sxil_mul_i128:\npushad\nlea esi, [esp+36]\nlea edi, [esp-16]\n\n"
"xor eax, eax\nmov [edi+8], eax\nmov [edi+12], eax\n\n"
"mov ecx, [esi+16]\n"
"mov eax, [esi+0]; imul ecx; mov [edi+0], eax; mov [edi+4], edx\n"
"mov eax, [esi+4]; imul ecx; add [edi+4], eax\n"
"adc [edi+8], edx; adc dword [edi+12], 0\n"
"mov eax, [esi+8]; imul ecx; add [edi+8], eax; adc [edi+12], edx\n"
"mov eax, [esi+12]; imul ecx; add [edi+12], eax\n\nmov ecx, [esi+20]\n"
"mov eax, [esi+0]; imul ecx; add [edi+4], eax\n"
"adc [edi+8], edx; adc dword [edi+12], 0\n"
"mov eax, [esi+4]; imul ecx; add [edi+8], eax; adc [edi+12], edx\n"
"mov eax, [esi+8]; imul ecx; add [edi+12], eax\n\nmov ecx, [esi+24]\n"
"mov eax, [esi+0]; imul ecx; add [edi+8], eax; adc [edi+12], edx\n"
"mov eax, [esi+4]; imul ecx; add [edi+12], eax\n\n"
"mov eax, [esi+0]; imul dword [esi+28]; add [edi+12], eax\n\n"
"mov eax, [edi+0]; mov [esi+0], eax\n"
"mov eax, [edi+4]; mov [esi+4], eax\n"
"mov eax, [edi+8]; mov [esi+8], eax\n"
"mov eax, [edi+12]; mov [esi+12], eax\npopad; ret\n\n\n"
"sxil_mul_i128_wide:\npushad\nlea esi, [esp+36]\nlea edi, [esp-32]\n\n"
"xor eax, eax\nmov [edi+0], eax\nmov [edi+4], eax\nmov [edi+8], eax\n"
"mov [edi+12], eax\nmov [edi+16], eax\nmov [edi+20], eax\n"
"mov [edi+24], eax\nmov [edi+28], eax\n\nmov ecx, [esi+16]\n"
"mov eax, [esi+0]; imul ecx; add [edi+0], eax\n"
"adc [edi+4], edx; adc dword [edi+8], 0\n"
"mov eax, [esi+4]; imul ecx; add [edi+4], eax\n"
"adc [edi+8], edx; adc dword [edi+12], 0\n"
"mov eax, [esi+8]; imul ecx; add [edi+8], eax\n"
"adc [edi+12], edx; adc dword [edi+16], 0\n"
"mov eax, [esi+12]; imul ecx; add [edi+12], eax\n"
"adc [edi+16], edx; adc dword [edi+20], 0\n\nmov ecx, [esi+20]\n"
"mov eax, [esi+0]; imul ecx; add [edi+4], eax\n"
"adc [edi+8], edx; adc dword [edi+12], 0\n"
"adc dword [edi+16], 0; adc dword [edi+20], 0\n"
"mov eax, [esi+4]; imul ecx; add [edi+8], eax\n"
"adc [edi+12], edx; adc dword [edi+16], 0; adc dword [edi+20], 0\n"
"mov eax, [esi+8]; imul ecx; add [edi+12], eax\n"
"adc [edi+16], edx; adc dword [edi+20], 0\n"
"mov eax, [esi+12]; imul ecx; add [edi+16], eax\n"
"adc [edi+20], edx; adc dword [edi+24], 0\n\nmov ecx, [esi+24]\n"
"mov eax, [esi+0]; imul ecx; add [edi+8], eax\n"
"adc [edi+12], edx; adc dword [edi+16], 0\n"
"adc dword [edi+20], 0; adc dword [edi+24], 0\n"
"mov eax, [esi+4]; imul ecx; add [edi+12], eax\n"
"adc [edi+16], edx; adc dword [edi+20], 0; adc dword [edi+24], 0\n"
"mov eax, [esi+8]; imul ecx; add [edi+16], eax\n"
"adc [edi+20], edx; adc dword [edi+24], 0\n"
"mov eax, [esi+12]; imul ecx; add [edi+20], eax\n"
"adc [edi+24], edx; adc dword [edi+28], 0\n\nmov ecx, [esi+28]\n"
"mov eax, [esi+0]; imul ecx; add [edi+12], eax\n"
"adc [edi+16], edx; adc dword [edi+20], 0\n"
"adc dword [edi+24], 0; adc dword [edi+28], 0\n"
"mov eax, [esi+4]; imul ecx; add [edi+16], eax\n"
"adc [edi+20], edx; adc dword [edi+24], 0; adc dword [edi+28], 0\n"
"mov eax, [esi+8]; imul ecx; add [edi+20], eax\n"
"adc [edi+24], edx; adc dword [edi+28], 0\n"
"mov eax, [esi+12]; imul ecx; add [edi+24], eax; adc [edi+28], edx\n\n\n"
"mov eax, [edi+0]; mov [esi+0], eax\n"
"mov eax, [edi+4]; mov [esi+4], eax\n"
"mov eax, [edi+8]; mov [esi+8], eax\n"
"mov eax, [edi+12]; mov [esi+12], eax\n"
"mov eax, [edi+16]; mov [esi+16], eax\n"
"mov eax, [edi+20]; mov [esi+20], eax\n"
"mov eax, [edi+24]; mov [esi+24], eax\n"
"mov eax, [edi+28]; mov [esi+28], eax\npopad; ret\n\nsxil_add_i128:\n"
"pushad\nlea esi, [esp+36]\nmov eax, [esi+16]; add [esi+0], eax\n"
"mov eax, [esi+20]; adc [esi+4], eax\n"
"mov eax, [esi+24]; adc [esi+8], eax\n"
"mov eax, [esi+28]; adc [esi+12], eax\npopad; ret\n\nsxil_sub_i128:\n"
"pushad\nlea esi, [esp+36]\nmov eax, [esi+16]; sub [esi+0], eax\n"
"mov eax, [esi+20]; sbb [esi+4], eax\n"
"mov eax, [esi+24]; sbb [esi+8], eax\n"
"mov eax, [esi+28]; sbb [esi+12], eax\npopad; ret\n\nsxil_cmp_i128:\n"
"pushad\nlea esi, [esp+36]\n"
"mov eax, [esi+28]; cmp [esi+12], eax; jne .l0\n"
"mov eax, [esi+24]; cmp [esi+8], eax; jne .l0\n"
"mov eax, [esi+20]; cmp [esi+4], eax; jne .l0\n"
"mov eax, [esi+16]; cmp [esi+0], eax; jne .l0\n.l0:\npopad; ret\n\n\n"
"sxil_shl_i128:\npushad\nlea esi, [esp+36]\nmov ecx, [esi+16]\n"
"shr ecx, 5\n.l0:\nmov eax, [esi+8]; mov [esi+12], eax\n"
"mov eax, [esi+4]; mov [esi+8], eax\n"
"mov eax, [esi+0]; mov [esi+4], eax\nxor eax, eax; mov [esi+0], eax\n"
"loop .l0\nmov ecx, [esi+16]; and ecx, 31\n"
"mov edx, [esi+12]; mov eax, [esi+8]; shld edx, eax, cl; mov [esi+12], edx\n"
"mov edx, eax; mov eax, [esi+4]; shld edx, eax, cl; mov [esi+8], edx\n"
"mov edx, eax; mov eax, [esi+0]; shld edx, eax, cl; mov [esi+4], edx\n"
"shl eax, cl; mov [esi+0], eax\npopad; ret\n\nsxil_shr_i128:\npushad\n"
"lea esi, [esp+36]\nmov ecx, [esi+16]\nshr ecx, 5\n.l0:\n"
"mov eax, [esi+4]; mov [esi+0], eax\n"
"mov eax, [esi+8]; mov [esi+4], eax\n"
"mov eax, [esi+12]; mov [esi+8], eax\n"
"bt eax, 31; setc al; movzx eax, al; neg eax\nmov [esi+12], eax\n"
"loop .l0\nmov ecx, [esi+16]; and ecx, 31\n"
"mov eax, [esi+0]; mov edx, [esi+4]; shrd eax, edx, cl; mov [esi+0], eax\n"
"mov eax, edx; mov edx, [esi+8]; shrd eax, edx, cl; mov [esi+4], eax\n"
"mov eax, edx; mov edx, [esi+12]; shrd eax, edx, cl; mov [esi+8], eax\n"
"sar edx, cl; mov [esi+12], edx\npopad; ret\n\nsxil_not_i128:\npushad\n"
"lea esi, [esp+36]\nnot dword [esi+0]; not dword [esi+4]\n"
"not dword [esi+8]; not dword [esi+12]\npopad; ret\n\nsxil_neg_i128:\n"
"pushad\nlea esi, [esp+36]\nnot dword [esi+0]; not dword [esi+4]\n"
"not dword [esi+8]; not dword [esi+12]\n"
"add dword [esi+0], 1; adc dword [esi+4], 0\n"
"adc dword [esi+8], 0; adc dword [esi+12], 0\npopad; ret\n\n"
"sxil_cmpz_i128:\npushad\nlea esi, [esp+36]\n"
"mov eax, [esi+12]; and eax, eax; jnz .l0\n"
"mov eax, [esi+8]; and eax, eax; jnz .l0\n"
"mov eax, [esi+4]; and eax, eax; jnz .l0\n"
"mov eax, [esi+0]; and eax, eax; jnz .l0\n.l0:\npopad; ret\n\n"

#if 0
"#endif\n\n"
"#ifdef x86_64\n\n[section .text]\n[bits 64]\n\nsxil_abs_rdax:\n"
"cmp rdx, 0; jge .l0\nnot rax; not rdx\nadd rax, 1; adc rdx, 0\n"
".l0: ret\n\nsxil_abs_rdsi:\ncmp rdi, 0; jge .l0\nnot rsi; not edi\n"
"add rsi, 1; adc edi, 0\n.l0: ret\n\n\n;rdx:rax / rdi:rsi, unsigned\n"
";r9:r8 = remainder, rcx=cnt\n\nsxil_div_rdax_rdsi_2x:\n"
"xor r9, r9; xor r8, r8\nmov ecx, 128\n.l0:\nshl rax, 1; rcl rdx, 1\n"
"rcl r8, 1; rcl r9, 1\ncmp r9, rdi; jb .l1; ja .l2\n"
"cmp r8, rsi; jb .l1\n.l2:\nsub r8, rsi; sbb r9, edi\n"
"add rax, 1; adc rdx, 0\n.l1: loop .l0\nret\n\n\n"
";unsigned divide rdx:rax / rdi:rsi\n;return:\n;rdx:rax = quotient\n"
";rdi:rsi = remainder\n\nsxil_div_rdax_rdsi_2u:\n"
"push r9; push r8; push rcx\ncall sxil_div_rdax_rdsi_2x\n"
"mov rdi, r9; mov rsi, r8\npop rcx; pop r8; pop r9\nret\n\n\n"
";signed divide rdx:rax / rdi:rsi\n;result = rdx:rax\n\n"
"sxil_div_rdax_rdsi:\npush r9; push r8; push rdi; push rsi; push rcx\n"
"mov rcx, rdx; xor rcx, rdi\n\npush rcx\n"
"call sxil_abs_rdax; call sxil_abs_rdsi\ncall sxil_div_rdax_rdsi_2x\n"
";mov edx, edi; mov eax, esi\npop rcx\n\nbt rcx, 63; jnc .l0\n"
"not rax; not rdx; add rax, 1; adc rdx, 0\n.l0:\n\n"
"pop rcx; pop rsi; pop rdi; pop r8; pop r9\nret\n\n\n"
";signed modulo rdx:rax % rdi:rsi\n;result = rdx:rax\n\n"
"sxil_mod_rdax_rdsi:\npush r9; push r8; push rdi; push rsi; push rcx\n\n"
"push rdx\ncall sxil_abs_rdax; call sxil_abs_rdsi\n"
"call sxil_div_rdax_rdsi_2x\nmov rdx, r9; mov rax, r8\n\npop rcx\n"
"bt rcx, 63; jnc .l0\nnot rax; not rdx; add rax, 1; adc rdx, 0\n.l0:\n\n"
"pop rcx; pop rsi; pop rdi; pop r8; pop r9\nret\n\n\nsxil_add_i128:\n"
"push rax\nmov rax, [esp+32]; add [esi+16], rax\n"
"mov rax, [esp+40]; adc [esi+24], rax\npop rax; ret\n\nsxil_sub_i128:\n"
"push rax\nmov rax, [esp+32]; sub [esi+16], rax\n"
"mov rax, [esp+40]; sbb [esi+24], rax\npop rax; ret\n\n\n"
"sxil_mul_i128:\npush rbp; mov rbp, rsp\n\n"
"push rax; push rcx; push rdx; push rbx\npush rsi; push rdi\n\n"
"mov rax, [rbp+16]; mov rdx, [ebp+24]\n"
"mov rsi, [rbp+32]; mov rdi, [ebp+40]\nmov rcx, rax; mov rbx, rdx\n"
"imul rsi\nimul rcx, rdi; add rdx, rcx\nimul rbx, rsi; add rdx, rbx\n"
"mov [rbp+16], rax; mov [rbp+24], rdx\n\npop rdi; pop rsi\n"
"pop rbx; pop rdx; pop rcx; pop rax\npop rbp\nret\n\nsxil_div_i128:\n"
";pushad\n\npush rbp; mov rbp, rsp\n\n"
"push rax; push rcx; push rdx; push rbx\n"
"push rsi; push rdi; push r8;  push r9\n\n"
"mov rax, [rbp+16]; mov rdx, [esp+24]\n"
"mov rsi, [rbp+32]; mov rdi, [esp+40]\n"
"mov rcx, rdx; xor rcx, rdi; push rcx\n"
"call sxil_abs_rdax; call sxil_abs_rdsi\ncall sxil_div_rdax_rdsi_2x\n"
"pop rcx\nbt rcx, 63; jnc .l0\n"
"not rax; not rdx; add rax, 1; adc rdx, 0\n.l0:\n"
"mov [rbp+16], rax; mov [rbp+24], rdx\n"
"mov [rbp+32], r8;  mov [rbp+40], r9\n"
"pop r9;  pop r8;  pop rdi; pop rsi\n"
"pop rbx; pop rdx; pop rcx; pop rax\npop rbp\nret\n\n#endif\n"
#endif
;


int tst_stkdump()
{
	static void *ip[1024];
	static void *bp[1024];
	void *p, *q, *r, *ibp;
	char *s;
	int i, j, n;

	n=BASM_StackTrace(ip, bp, 1024);

	printf("Trace %d\n", n);
	for(i=0; i<n; i++)
	{
		p=ip[i];
		s=BASM_GetLastNamePtr(p, &q);

		j=((byte *)p)-((byte *)q);
		if(!s || (j>4096))s="?";
		printf("%p: %p %p %s %d\n", bp[i], p, q, s, j);
	}

#if 0
	p=BASM_GetEBP();
	ibp=p;
	while(1)
	{
		if((p<ibp) || (p>(ibp+65536)))
			break;

		q=((void **)p)[1];
		p=((void **)p)[0];
		s=BASM_GetLastNamePtr(q, &r);
		if((q-r)>1024)s="?";
		printf("%p %p %s %d\n", q, r, s, q-r);

		printf("next %p\n", p);
	}
#endif
}

int tst_w0()
{
	tst_stkdump();
}

int main(int argc, char *argv[])
{
	BASMX_Opcode **oplst;
	BASM_Context *ctx;


	char *s;
	double f, g;
	int i, j, sz, t, ts, te;

//	if(argc<3)
//	{
//		printf("usage: %s in-asm out-coff\n", argv[0]);
//		return(-1);
//	}

	BASM_ProcessEXE(argv[0]);

//	BASM_AssembleFile(argv[1], argv[2]);

//	tst_stkdump();
	tst_w0();

#if 1
	t=clock();
	ts=t; te=t+30*CLOCKS_PER_SEC;

	i=0;
	while(t<te)
	{
		printf("%d\r", i);
		fflush(stdout);

		BASM_BeginAssembly(NULL);
		basm_puts(sxil_asm_int128);
//		BASM_EndAssemblyQuiet();

//		s=(char *)BASM_EndAssemblyThunkInline(NULL);
//		gcfree(s);

		s=(char *)BASM_EndAssemblyObjBuf(&sz);
//		s=(char *)BASM_EndAssemblyObjBufFast(&sz);
		BASM_FreeObjBuf(s);

		t=clock();
		i++;
	}
	j=i*strlen(sxil_asm_int128);
	f=(t-ts)/((double)(CLOCKS_PER_SEC));
	printf("%d (%d bytes in %.3fs), %.3f MB/s\n",
		i, j, f,
		(j/f)*(1.0/1000000));
#endif

#if 1
	ctx=BASM_NewContext();


	s=sxil_asm_int128;
	oplst=BASMX_ParseOpcodeListMalloc(ctx, &s);


	t=clock();
	ts=t; te=t+30*CLOCKS_PER_SEC;

	i=0;
	while(t<te)
	{
		printf("%d\r", i);
		fflush(stdout);

		BASMX_BeginAssembly(ctx, NULL);

		BASMX_OutOpcodeList(ctx, oplst);

		s=(char *)BASMX_EndAssemblyObjBuf(ctx, &sz);
		BASM_FreeObjBuf(s);

		t=clock();
		i++;
	}
	j=i*strlen(sxil_asm_int128);
	f=(t-ts)/((double)(CLOCKS_PER_SEC));
	printf("%d (%d bytes in %.3fs), %.3f MB/s\n",
		i, j, f,
		(j/f)*(1.0/1000000));
#endif

	return(0);
}