[section .text]
[bits 32]

;cross-multiply
;esi=input A (forwards)
;edi=input B (backwards)
;ecx=count, edx=initial low word
;return:
;edx:eax=result

mul_cross:
push ebp
push ebx
push esi
push edi

xor ebp, ebp
mov ebx, edx

lea edi, [edi+ecx*4-4]

.l0:
mov eax, [esi]
mul [edi]
add ebx, eax
adc ebp, edx

add esi, 4
sub edi, 4

sub ecx, 1
jnz .l0

mov eax, ebx
mov edx, ebp

pop edi
pop esi
pop ebx
pop ebp
ret

mul_cross2_256:
push ebp
push ebx
push esi
push edi

xor ebp, ebp
mov ebx, edx

lea esi, [esi-ecx*4+32]
lea edi, [edi+28]

.l0:
mov eax, [esi]
mul [edi]
add ebx, eax
adc ebp, edx

add esi, 4
sub edi, 4

sub ecx, 1
jnz .l0

mov eax, ebx
mov edx, ebp

pop edi
pop esi
pop ebx
pop ebp
ret


_math_int256_add:
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
mul ecx
mov [ebx+0], eax

mov ecx, 2
call mul_cross
mov [ebx+4], eax

mov ecx, 3
call mul_cross
mov [ebx+8], eax

mov ecx, 4
call mul_cross
mov [ebx+12], eax

mov ecx, 5
call mul_cross
mov [ebx+16], eax

mov ecx, 6
call mul_cross
mov [ebx+20], eax

mov ecx, 7
call mul_cross
mov [ebx+24], eax

mov ecx, 8
call mul_cross
mov [ebx+28], eax

mov ecx, 7
call mul_cross2_256
mov [ebx+32], eax

mov ecx, 6
call mul_cross2_256
mov [ebx+36], eax

mov ecx, 5
call mul_cross2_256
mov [ebx+40], eax

mov ecx, 4
call mul_cross2_256
mov [ebx+44], eax

mov ecx, 3
call mul_cross2_256
mov [ebx+48], eax

mov ecx, 2
call mul_cross2_256
mov [ebx+52], eax

mov ecx, 1
call mul_cross2_256
mov [ebx+56], eax

mov [ebx+60], edx

pop edi
pop esi
pop ebx
pop ebp
ret
