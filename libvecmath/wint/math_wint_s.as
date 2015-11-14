global _WInt_Zero64
global _WInt_Zero128
global _WInt_Zero256
global _WInt_Zero512
global _WInt_Zero1024

global _WInt_Copy64
global _WInt_Copy128
global _WInt_Copy256
global _WInt_Copy512
global _WInt_Copy1024

global _WInt_Add64
global _WInt_Sub64
global _WInt_Add128
global _WInt_Sub128

global _WInt_Mul64
global _WInt_Mul64L

global _WInt_Mul128
global _WInt_Mul128L


[section .text]
[bits 32]

_WInt_Zero64:
xor eax, eax
mov ecx, [esp+4]
mov [ecx+0], eax
mov [ecx+4], eax
ret

_WInt_Zero128:
xor eax, eax
mov ecx, [esp+4]
mov [ecx+0], eax
mov [ecx+4], eax
mov [ecx+8], eax
mov [ecx+12], eax
ret

_WInt_Zero256:
xor eax, eax
mov ecx, [esp+4]
mov [ecx+0], eax
mov [ecx+4], eax
mov [ecx+8], eax
mov [ecx+12], eax
mov [ecx+16], eax
mov [ecx+20], eax
mov [ecx+24], eax
mov [ecx+28], eax
ret

_WInt_Zero512:
push dword [esp+4]
call _WInt_Zero256
add dword [esp], 32
call _WInt_Zero256
pop eax
ret

_WInt_Zero1024:
push dword [esp+4]
call _WInt_Zero512
add dword [esp], 64
call _WInt_Zero512
pop eax
ret

_WInt_Copy64:
mov ecx, [ebp+4]
mov edx, [ebp+8]
mov eax, [ecx+0]
mov [edx+0], eax
mov eax, [ecx+4]
mov [edx+4], eax
ret

_WInt_Copy128:
push ebp
mov ebp, esp
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]

mov eax, [esi+0]
mov edx, [esi+4]
mov ecx, [esi+8]
mov [edi+0], eax
mov [edi+4], edx
mov eax, [esi+12]
mov [edi+8], ecx
mov [edi+12], eax

pop edi
pop esi
pop ebp
ret

_WInt_Copy256:
push ebp
mov ebp, esp
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]

mov eax, [esi+0]
mov edx, [esi+4]
mov ecx, [esi+8]
mov [edi+0], eax
mov [edi+4], edx
mov eax, [esi+12]
mov edx, [esi+16]
mov [edi+8], ecx
mov [edi+12], eax
mov ecx, [esi+20]
mov [edi+16], edx
mov eax, [esi+24]
mov edx, [esi+28]
mov [edi+20], ecx
mov [edi+24], eax
mov [edi+28], edx

pop edi
pop esi
pop ebp
ret

_WInt_Copy512:
mov edx, [esp+8]
mov eax, [esp+4]
push edx
push eax
call _WInt_Copy256
add dword [esp+0], 32
add dword [esp+4], 32
call _WInt_Copy256
pop eax
pop edx
ret

_WInt_Copy1024:
mov edx, [esp+8]
mov eax, [esp+4]
push edx
push eax
call _WInt_Copy512
add dword [esp+0], 64
add dword [esp+4], 64
call _WInt_Copy512
pop eax
pop edx
ret

_WInt_Inc64:
mov ecx, [esp+4]
add dword [ecx+0], 1
adc dword [ecx+4], 0
ret

_WInt_Inc128:
mov ecx, [esp+4]
add dword [ecx+0], 1
adc dword [ecx+4], 0
adc dword [ecx+8], 0
adc dword [ecx+12], 0
ret

_WInt_Inc256:
mov ecx, [esp+4]
add dword [ecx+0], 1
adc dword [ecx+4], 0
adc dword [ecx+8], 0
adc dword [ecx+12], 0
adc dword [ecx+16], 0
adc dword [ecx+20], 0
adc dword [ecx+24], 0
adc dword [ecx+28], 0
ret

_WInt_Inc512:
mov ecx, [esp+4]
add dword [ecx+0], 1
adc dword [ecx+4], 0
adc dword [ecx+8], 0
adc dword [ecx+12], 0
adc dword [ecx+16], 0
adc dword [ecx+20], 0
adc dword [ecx+24], 0
adc dword [ecx+28], 0

adc dword [ecx+32], 0
adc dword [ecx+36], 0
adc dword [ecx+40], 0
adc dword [ecx+44], 0
adc dword [ecx+48], 0
adc dword [ecx+52], 0
adc dword [ecx+56], 0
adc dword [ecx+60], 0
ret

_WInt_Inc1024:
mov ecx, [esp+4]
add dword [ecx+0], 1
adc dword [ecx+4], 0
adc dword [ecx+8], 0
adc dword [ecx+12], 0
adc dword [ecx+16], 0
adc dword [ecx+20], 0
adc dword [ecx+24], 0
adc dword [ecx+28], 0

adc dword [ecx+32], 0
adc dword [ecx+36], 0
adc dword [ecx+40], 0
adc dword [ecx+44], 0
adc dword [ecx+48], 0
adc dword [ecx+52], 0
adc dword [ecx+56], 0
adc dword [ecx+60], 0

adc dword [ecx+64], 0
adc dword [ecx+68], 0
adc dword [ecx+72], 0
adc dword [ecx+76], 0
adc dword [ecx+80], 0
adc dword [ecx+84], 0
adc dword [ecx+88], 0
adc dword [ecx+92], 0
adc dword [ecx+96], 0
adc dword [ecx+100], 0
adc dword [ecx+104], 0
adc dword [ecx+108], 0
adc dword [ecx+112], 0
adc dword [ecx+116], 0
adc dword [ecx+120], 0
adc dword [ecx+124], 0
ret


_WInt_Add64:
push ebp
mov ebp, esp
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]
mov ecx, [ebp+16]

mov eax, [esi+0]
add eax, [edi+0]
mov [ecx+0], eax

mov eax, [esi+4]
adc eax, [edi+4]
mov [ecx+4], eax

pop edi
pop esi
pop ebp
ret

_WInt_Add128:
push ebp
mov ebp, esp
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]
mov ecx, [ebp+16]

mov eax, [esi+0]
add eax, [edi+0]
mov [ecx+0], eax

mov eax, [esi+4]
adc eax, [edi+4]
mov [ecx+4], eax

mov eax, [esi+8]
adc eax, [edi+8]
mov [ecx+8], eax

mov eax, [esi+12]
adc eax, [edi+12]
mov [ecx+12], eax

pop edi
pop esi
pop ebp
ret


_WInt_Sub64:
push ebp
mov ebp, esp
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]
mov ecx, [ebp+16]

mov eax, [esi+0]
sub eax, [edi+0]
mov [ecx+0], eax

mov eax, [esi+4]
sbb eax, [edi+4]
mov [ecx+4], eax

pop edi
pop esi
pop ebp
ret

_WInt_Sub128:
push ebp
mov ebp, esp
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]
mov ecx, [ebp+16]

mov eax, [esi+0]
sub eax, [edi+0]
mov [ecx+0], eax

mov eax, [esi+4]
sbb eax, [edi+4]
mov [ecx+4], eax

mov eax, [esi+8]
sbb eax, [edi+8]
mov [ecx+8], eax

mov eax, [esi+12]
sbb eax, [edi+12]
mov [ecx+12], eax

pop edi
pop esi
pop ebp
ret



;void WInt_Mul64(int64 *a, int64 *b, int128 *c)
;	c=a*b;
_WInt_Mul64:
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
adc dword [ebx+12], 0

mov eax, [esi+4]
mul ecx
add [ebx+8], eax
adc dword [ebx+12], edx

pop edi
pop esi
pop ebx
pop ebp
ret



;void WInt_Mul128(int128 *a, int128 *b, int256 *c)
;	c=a*b;
;uses:
;esp/ebp: normal
;edx:eax, temp
;edi:esi, accum (low 64)
;ecx, accum (high 32)
;ebx, output

_WInt_Mul128:
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


;void WInt_Mul128L(int128 *a, int128 *b, int128 *c)
;	c=a*b;
;128 multiply, low half
;uses:
;esp/ebp: normal
;edx:eax, temp
;edi:esi, accum (low 64)
;ecx, accum (high 32)
;ebx, output

_WInt_Mul128L:
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


add esp, 32
pop edi
pop esi
pop ebx
pop ebp
ret

