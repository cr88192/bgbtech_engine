_math_wint_mul64:
push ebp
mov ebp, esp
push ebx
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]

xor eax, eax

xor ecx, ecx
xor ebx, ebx
mov eax, [esi+0]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+0], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+4], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+8], eax

mov [ebx+12], ecx
pop edi
pop esi
pop ebx
pop ebp
ret


_math_wint_mul128:
push ebp
mov ebp, esp
push ebx
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]

xor eax, eax

xor ecx, ecx
xor ebx, ebx
mov eax, [esi+0]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+0], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+4], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+8], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+12], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+16], eax

xor ebx, ebx
mov eax, [esi+4]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+20], eax

xor ebx, ebx
mov eax, [esi+8]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+24], eax

mov [ebx+28], ecx
pop edi
pop esi
pop ebx
pop ebp
ret


_math_wint_mul256:
push ebp
mov ebp, esp
push ebx
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]

xor eax, eax

xor ecx, ecx
xor ebx, ebx
mov eax, [esi+0]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+0], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+4], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+8], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+12], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+16], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+20], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+24], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+28], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+32], eax

xor ebx, ebx
mov eax, [esi+4]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+36], eax

xor ebx, ebx
mov eax, [esi+8]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+40], eax

xor ebx, ebx
mov eax, [esi+12]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+44], eax

xor ebx, ebx
mov eax, [esi+16]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+48], eax

xor ebx, ebx
mov eax, [esi+20]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+52], eax

xor ebx, ebx
mov eax, [esi+24]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+56], eax

mov [ebx+60], ecx
pop edi
pop esi
pop ebx
pop ebp
ret


_math_wint_mul512:
push ebp
mov ebp, esp
push ebx
push esi
push edi

mov esi, [ebp+8]
mov edi, [ebp+12]

xor eax, eax

xor ecx, ecx
xor ebx, ebx
mov eax, [esi+0]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+0], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+4], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+8], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+12], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+16], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+20], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+24], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+28], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+32], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+36], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+40], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+44], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+48], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+52], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+56], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+60], eax

xor ebx, ebx
mov eax, [esi+0]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+4]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+0]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+64], eax

xor ebx, ebx
mov eax, [esi+4]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+8]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+4]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+68], eax

xor ebx, ebx
mov eax, [esi+8]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+12]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+8]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+72], eax

xor ebx, ebx
mov eax, [esi+12]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+16]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+12]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+76], eax

xor ebx, ebx
mov eax, [esi+16]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+20]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+16]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+80], eax

xor ebx, ebx
mov eax, [esi+20]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+24]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+20]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+84], eax

xor ebx, ebx
mov eax, [esi+24]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+28]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+24]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+88], eax

xor ebx, ebx
mov eax, [esi+28]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+32]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+28]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+92], eax

xor ebx, ebx
mov eax, [esi+32]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+36]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+32]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+96], eax

xor ebx, ebx
mov eax, [esi+36]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+40]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+36]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+100], eax

xor ebx, ebx
mov eax, [esi+40]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+44]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+40]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+104], eax

xor ebx, ebx
mov eax, [esi+44]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+48]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+44]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+108], eax

xor ebx, ebx
mov eax, [esi+48]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+52]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+48]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+112], eax

xor ebx, ebx
mov eax, [esi+52]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+56]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+52]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+116], eax

xor ebx, ebx
mov eax, [esi+56]
mul [edi+60]
add ecx, eax
adc ebx, edx

mov eax, [esi+60]
mul [edi+56]
add ecx, eax
adc ebx, edx

mov eax, ecx
mov ecx, ebx
mov ebx, [ebp+16]
mov [ebx+120], eax

mov [ebx+124], ecx
pop edi
pop esi
pop ebx
pop ebp
ret


