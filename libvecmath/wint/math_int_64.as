[section .text]
[bits 32]

;void math_int64_add(int64 *a, int64 *b, int64 *c)
;	c=a+b;

_math_int64_add:
push ebp
mov ebp, esp
push ebx
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]
mov ebx, [ebp+16]

mov eax, [esi]
mov ecx, [edi]
add eax, ecx
mov [ebx], eax

mov eax, [esi+4]
mov ecx, [edi+4]
adc eax, ecx
mov [ebx+4], eax

pop edi
pop esi
pop ebx
pop ebp
ret


;void math_int64_sub(int64 *a, int64 *b, int64 *c)
;	c=a+b;

_math_int64_sub:
push ebp
mov ebp, esp
push ebx
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]
mov ebx, [ebp+16]

mov eax, [esi]
mov ecx, [edi]
sub eax, ecx
mov [ebx], eax

mov eax, [esi+4]
mov ecx, [edi+4]
sbb eax, ecx
mov [ebx+4], eax

pop edi
pop esi
pop ebx
pop ebp
ret


;void math_int64_mul(int64 *a, int64 *b, int128 *c)
;	c=a+b;

_math_int64_mul:
push ebp
mov ebp, esp
push ebx
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]
mov ebx, [ebp+16]

xor eax, eax
mov [ebx+0], eax
mov [ebx+4], eax
mov [ebx+8], eax
mov [ebx+12], eax

mov ecx, [edi]

mov eax, [esi]
mul ecx
mov [ebx+0], eax
mov [ebx+4], edx

mov eax, [esi+4]
mul ecx
add [ebx+4], eax
adc edx, 0
mov [ebx+8], edx

mov ecx, [edi+4]

mov eax, [esi]
mul ecx
add [ebx+4], eax
adc edx, 0
add [ebx+8], edx
adc [ebx+12], 0

mov eax, [esi+4]
mul ecx
add [ebx+8], eax
adc [ebx+12], edx

pop edi
pop esi
pop ebx
pop ebp
ret

