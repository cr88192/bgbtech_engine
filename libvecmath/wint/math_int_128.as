[section .text]
[bits 32]


;void math_int128_add(int128 *a, int128 *b, int128 *c)
;	c=a+b;

_math_int128_add:
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

mov eax, [esi+8]
mov ecx, [edi+8]
adc eax, ecx
mov [ebx+8], eax

mov eax, [esi+12]
mov ecx, [edi+12]
adc eax, ecx
mov [ebx+12], eax

pop edi
pop esi
pop ebx
pop ebp
ret


;void math_int128_sub(int128 *a, int128 *b, int128 *c)
;	c=a+b;

_math_int128_sub:
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

mov eax, [esi+8]
mov ecx, [edi+8]
sbb eax, ecx
mov [ebx+8], eax

mov eax, [esi+12]
mov ecx, [edi+12]
sbb eax, ecx
mov [ebx+12], eax

pop edi
pop esi
pop ebx
pop ebp
ret


;void math_int128_mul(int128 *a, int128 *b, int256 *c)
;	c=a+b;

_math_int128_mul:
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
mov [ebx+16], eax
mov [ebx+20], eax
mov [ebx+24], eax
mov [ebx+28], eax

mov eax, [esi]
mov ecx, [edi]
mul ecx
mov [ebx+0], eax
mov [ebx+4], edx

mov eax, [esi+4]
mul ecx
add [ebx+4], eax
adc edx, 0
mov [ebx+8], edx

mov eax, [esi+8]
mul ecx
add [ebx+8], eax
adc edx, 0
mov [ebx+12], edx

mov eax, [esi+12]
mul ecx
add [ebx+12], eax
adc edx, 0
mov [ebx+16], edx

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
adc [ebx+16], 0

mov eax, [esi+8]
mul ecx
add [ebx+12], eax
adc [ebx+16], edx
adc [ebx+20], 0

mov eax, [esi+12]
mul ecx
add [ebx+16], eax
adc [ebx+20], edx
adc [ebx+24], 0


mov ecx, [edi+8]

mov eax, [esi]
mul ecx
add [ebx+8], eax
adc edx, 0
add [ebx+12], edx
adc [ebx+16], 0

mov eax, [esi+4]
mul ecx
add [ebx+12], eax
adc [ebx+16], edx
adc [ebx+20], 0

mov eax, [esi+8]
mul ecx
add [ebx+16], eax
adc [ebx+20], edx
adc [ebx+24], 0

mov eax, [esi+12]
mul ecx
add [ebx+20], eax
adc [ebx+24], edx
adc [ebx+28], 0


mov ecx, [edi+12]

mov eax, [esi]
mul ecx
add [ebx+12], eax
adc edx, 0
add [ebx+16], edx
adc [ebx+20], 0

mov eax, [esi+4]
mul ecx
add [ebx+16], eax
adc [ebx+20], edx
adc [ebx+24], 0

mov eax, [esi+8]
mul ecx
add [ebx+20], eax
adc [ebx+24], edx
adc [ebx+28], 0

mov eax, [esi+12]
mul ecx
add [ebx+24], eax
adc [ebx+28], edx


pop edi
pop esi
pop ebx
pop ebp
ret


;void math_int128_mul(int128 *a, int128 *b, int256 *c)
;	c=a*b;
;uses:
;esp/ebp: normal
;edx:eax, temp
;edi:esi, accum (low 64)
;ecx, accum (high 32)
;ebx, output

_math_int128_mul:
push ebp
mov ebp, esp
push ebx
push esi
push edi

sub esp, 32

mov esi, [ebp+8]
mov edi, [ebp+12]

mov eax, [esi+0]
mov ecx, [esi+4]
mov edx, [esi+8]
mov ebx, [esi+12]
mov [esp+0], eax
mov [esp+4], ecx
mov [esp+8], edx
mov [esp+12], ebx

mov eax, [edi+0]
mov ecx, [edi+4]
mov edx, [edi+8]
mov ebx, [edi+12]
mov [esp+16], eax
mov [esp+20], ecx
mov [esp+24], edx
mov [esp+28], ebx

mov ebx, [ebp+16]

mov eax, [esp+0]
mul dword [esp+16]
mov [ebx+0], eax
mov esi, edx
xor edi, edi
xor ecx, ecx

mov eax, [esp+4]
mul dword [esp+16]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+0]
mul dword [esp+20]
add esi, eax
adc edi, edx
adc ecx, 0

mov [ebx+4], esi
mov esi, edi
mov edi, ecx
xor ecx, ecx


mov eax, [esp+8]
mul dword [esp+16]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+4]
mul dword [esp+20]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+0]
mul dword [esp+24]
add esi, eax
adc edi, edx
adc ecx, 0

mov [ebx+8], esi
mov esi, edi
mov edi, ecx
xor ecx, ecx


mov eax, [esp+12]
mul dword [esp+16]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+8]
mul dword [esp+20]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+4]
mul dword [esp+24]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+0]
mul dword [esp+28]
add esi, eax
adc edi, edx
adc ecx, 0

mov [ebx+12], esi
mov esi, edi
mov edi, ecx
xor ecx, ecx


;upper half

mov eax, [esp+12]
mul dword [esp+20]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+8]
mul dword [esp+24]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+4]
mul dword [esp+28]
add esi, eax
adc edi, edx
adc ecx, 0

mov [ebx+16], esi
mov esi, edi
mov edi, ecx
xor ecx, ecx


mov eax, [esp+12]
mul dword [esp+24]
add esi, eax
adc edi, edx
adc ecx, 0

mov eax, [esp+8]
mul dword [esp+28]
add esi, eax
adc edi, edx
adc ecx, 0

mov [ebx+20], esi
mov esi, edi
mov edi, ecx
xor ecx, ecx


mov eax, [esp+12]
mul dword [esp+28]
add esi, eax
adc edi, edx

mov [ebx+24], esi
mov [ebx+28], edi


add esp, 32
pop edi
pop esi
pop ebx
pop ebp
ret
