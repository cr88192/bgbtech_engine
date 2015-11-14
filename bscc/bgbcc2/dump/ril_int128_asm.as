ril_abs_i128:
cmp dword [esi+12], 0; jge .l0
not dword [esi+0]; not dword [esi+4]
not dword [esi+8]; not dword [esi+12]
add dword [esi+0], 1; adc dword [esi+4], 0
adc dword [esi+8], 0; adc dword [esi+12], 0
.l0: ret
ril_abs_i128_2:
cmp dword [esi+28], 0; jge .l0
not dword [esi+16]; not dword [esi+20]
not dword [esi+24]; not dword [esi+28]
add dword [esi+16], 1; adc dword [esi+20], 0
adc dword [esi+24], 0; adc dword [esi+28], 0
.l0: ret
ril_abs_i128_12:
cmp dword [esi+12], 0; jge .l0
not dword [esi+0]; not dword [esi+4]
not dword [esi+8]; not dword [esi+12]
add dword [esi+0], 1; adc dword [esi+4], 0
adc dword [esi+8], 0; adc dword [esi+12], 0
.l0:
cmp dword [esi+28], 0; jge .l1
not dword [esi+16]; not dword [esi+20]
not dword [esi+24]; not dword [esi+28]
add dword [esi+16], 1; adc dword [esi+20], 0
adc dword [esi+24], 0; adc dword [esi+28], 0
.l1: ret
ril_div_i128_inner:
xor eax, eax
mov [esp-4], eax; mov [esp-8], eax
mov [esp-12], eax; mov [esp-16], eax
mov ecx, 128
.l0:
shl dword [esi+0], 1; rcl dword [esi+4], 1
rcl dword [esi+8], 1; rcl dword [esi+12], 1
rcl dword [esp-16], 1; rcl dword [esp-12], 1
rcl dword [esp-8], 1; rcl dword [esp-4], 1
mov eax, [esp-4]; cmp eax, [esi+28]; jb .l1; ja .l2
mov eax, [esp-8]; cmp eax, [esi+24]; jb .l1; ja .l2
mov eax, [esp-12]; cmp eax, [esi+20]; jb .l1; ja .l2
mov eax, [esp-16]; cmp eax, [esi+16]; jb .l1
.l2:
mov eax, [esi+16]; sub [esp-16], eax
mov eax, [esi+20]; sbb [esp-12], eax
mov eax, [esi+24]; sbb [esp-8], eax
mov eax, [esi+28]; sbb [esp-4], eax
add dword [esi+0], 1; adc dword [esi+4], 0
adc dword [esi+8], 0; adc dword [esi+12], 0
.l1: sub ecx, 1; jnz .l0
mov eax, [esp-16]; mov [esi+16], eax
mov eax, [esp-12]; mov [esi+20], eax
mov eax, [esp-8]; mov [esi+24], eax
mov eax, [esp-4]; mov [esi+28], eax
ret
ril_div_i128:
pushad; lea esi, [esp+36]
mov edx, [esi+12]; xor edx, [esi+28]
call ril_abs_i128_12
call ril_div_i128_inner
bt edx, 31; jnc .l0
not dword [esi+0]; not dword [esi+4]
not dword [esi+8]; not dword [esi+12]
add dword [esi+0], 1; adc dword [esi+4], 0
adc dword [esi+8], 0; adc dword [esi+12], 0
.l0:
popad; ret
ril_mul_i128:
pushad
lea esi, [esp+36]
lea edi, [esp-16]
xor eax, eax
mov [edi+8], eax
mov [edi+12], eax
mov ecx, [esi+16]
mov eax, [esi+0]; imul ecx; mov [edi+0], eax; mov [edi+4], edx
mov eax, [esi+4]; imul ecx; add [edi+4], eax
adc [edi+8], edx; adc dword [edi+12], 0
mov eax, [esi+8]; imul ecx; add [edi+8], eax; adc [edi+12], edx
mov eax, [esi+12]; imul ecx; add [edi+12], eax
mov ecx, [esi+20]
mov eax, [esi+0]; imul ecx; add [edi+4], eax
adc [edi+8], edx; adc dword [edi+12], 0
mov eax, [esi+4]; imul ecx; add [edi+8], eax; adc [edi+12], edx
mov eax, [esi+8]; imul ecx; add [edi+12], eax
mov ecx, [esi+24]
mov eax, [esi+0]; imul ecx; add [edi+8], eax; adc [edi+12], edx
mov eax, [esi+4]; imul ecx; add [edi+12], eax
mov eax, [esi+0]; imul [esi+28]; add [edi+12], eax
mov eax, [edi+0]; mov [esi+0], eax
mov eax, [edi+4]; mov [esi+4], eax
mov eax, [edi+8]; mov [esi+8], eax
mov eax, [edi+12]; mov [esi+12], eax
popad; ret
ril_mul_i128_wide:
pushad
lea esi, [esp+36]
lea edi, [esp-32]
xor eax, eax
mov [edi+0], eax
mov [edi+4], eax
mov [edi+8], eax
mov [edi+12], eax
mov [edi+16], eax
mov [edi+20], eax
mov [edi+24], eax
mov [edi+28], eax
mov ecx, [esi+16]
mov eax, [esi+0]; imul ecx; add [edi+0], eax
adc [edi+4], edx; adc dword [edi+8], 0
mov eax, [esi+4]; imul ecx; add [edi+4], eax
adc [edi+8], edx; adc dword [edi+12], 0
mov eax, [esi+8]; imul ecx; add [edi+8], eax
adc [edi+12], edx; adc dword [edi+16], 0
mov eax, [esi+12]; imul ecx; add [edi+12], eax
adc [edi+16], edx; adc dword [edi+20], 0
mov ecx, [esi+20]
mov eax, [esi+0]; imul ecx; add [edi+4], eax
adc [edi+8], edx; adc dword [edi+12], 0
adc dword [edi+16], 0; adc dword [edi+20], 0
mov eax, [esi+4]; imul ecx; add [edi+8], eax
adc [edi+12], edx; adc dword [edi+16], 0; adc dword [edi+20], 0
mov eax, [esi+8]; imul ecx; add [edi+12], eax
adc [edi+16], edx; adc dword [edi+20], 0
mov eax, [esi+12]; imul ecx; add [edi+16], eax
adc [edi+20], edx; adc dword [edi+24], 0
mov ecx, [esi+24]
mov eax, [esi+0]; imul ecx; add [edi+8], eax
adc [edi+12], edx; adc dword [edi+16], 0
adc dword [edi+20], 0; adc dword [edi+24], 0
mov eax, [esi+4]; imul ecx; add [edi+12], eax
adc [edi+16], edx; adc dword [edi+20], 0; adc dword [edi+24], 0
mov eax, [esi+8]; imul ecx; add [edi+16], eax
adc [edi+20], edx; adc dword [edi+24], 0
mov eax, [esi+12]; imul ecx; add [edi+20], eax
adc [edi+24], edx; adc dword [edi+28], 0
mov ecx, [esi+28]
mov eax, [esi+0]; imul ecx; add [edi+12], eax
adc [edi+16], edx; adc dword [edi+20], 0
adc dword [edi+24], 0; adc dword [edi+28], 0
mov eax, [esi+4]; imul ecx; add [edi+16], eax
adc [edi+20], edx; adc dword [edi+24], 0; adc dword [edi+28], 0
mov eax, [esi+8]; imul ecx; add [edi+20], eax
adc [edi+24], edx; adc dword [edi+28], 0
mov eax, [esi+12]; imul ecx; add [edi+24], eax; adc [edi+28], edx
mov eax, [edi+0]; mov [esi+0], eax
mov eax, [edi+4]; mov [esi+4], eax
mov eax, [edi+8]; mov [esi+8], eax
mov eax, [edi+12]; mov [esi+12], eax
mov eax, [edi+16]; mov [esi+16], eax
mov eax, [edi+20]; mov [esi+20], eax
mov eax, [edi+24]; mov [esi+24], eax
mov eax, [edi+28]; mov [esi+28], eax
popad; ret
ril_add_i128:
pushad
lea esi, [esp+36]
mov eax, [esi+16]; add [esi+0], eax
mov eax, [esi+20]; adc [esi+4], eax
mov eax, [esi+24]; adc [esi+8], eax
mov eax, [esi+28]; adc [esi+12], eax
popad; ret
ril_sub_i128:
pushad
lea esi, [esp+36]
mov eax, [esi+16]; sub [esi+0], eax
mov eax, [esi+20]; sbb [esi+4], eax
mov eax, [esi+24]; sbb [esi+8], eax
mov eax, [esi+28]; sbb [esi+12], eax
popad; ret
ril_cmp_i128:
pushad
lea esi, [esp+36]
mov eax, [esi+28]; cmp [esi+12], eax; jne .l0
mov eax, [esi+24]; cmp [esi+8], eax; jne .l0
mov eax, [esi+20]; cmp [esi+4], eax; jne .l0
mov eax, [esi+16]; cmp [esi+0], eax; jne .l0
.l0:popad; ret
ril_shl_i128:
pushad
lea esi, [esp+36]
mov ecx, [esi+16]
shr ecx, 5
.l0:
mov eax, [esi+8]; mov [esi+12], eax
mov eax, [esi+4]; mov [esi+8], eax
mov eax, [esi+0]; mov [esi+4], eax
xor eax, eax; mov [esi+0], eax
loop .l0
mov ecx, [esi+16]; and ecx, 31
mov edx, [esi+12]; mov eax, [esi+8]; shld edx, eax, cl; mov [esi+12], edx
mov edx, eax; mov eax, [esi+4]; shld edx, eax, cl; mov [esi+8], edx
mov edx, eax; mov eax, [esi+0]; shld edx, eax, cl; mov [esi+4], edx
shl eax, cl; mov [esi+0], eax
popad; ret
ril_shr_i128:
pushad
lea esi, [esp+36]
mov ecx, [esi+16]
shr ecx, 5
.l0:
mov eax, [esi+4]; mov [esi+0], eax
mov eax, [esi+8]; mov [esi+4], eax
mov eax, [esi+12]; mov [esi+8], eax
bt eax, 31; setc al; movzx eax, al; neg eax
mov [esi+12], eax
loop .l0
mov ecx, [esi+16]; and ecx, 31
mov eax, [esi+0]; mov edx, [esi+4]; shrd eax, edx, cl; mov [esi+0], eax
mov eax, edx; mov edx, [esi+8]; shrd eax, edx, cl; mov [esi+4], eax
mov eax, edx; mov edx, [esi+12]; shrd eax, edx, cl; mov [esi+8], eax
sar edx, cl; mov [esi+12], edx
popad; ret
ril_not_i128:
pushad
lea esi, [esp+36]
not dword [esi+0]; not dword [esi+4]
not dword [esi+8]; not dword [esi+12]
popad; ret
ril_neg_i128:
pushad
lea esi, [esp+36]
not dword [esi+0]; not dword [esi+4]
not dword [esi+8]; not dword [esi+12]
add dword [esi+0], 1; adc dword [esi+4], 0
adc dword [esi+8], 0; adc dword [esi+12], 0
popad; ret
ril_cmpz_i128:
pushad
lea esi, [esp+36]
mov eax, [esi+12]; and eax, eax; jnz .l0
mov eax, [esi+8]; and eax, eax; jnz .l0
mov eax, [esi+4]; and eax, eax; jnz .l0
mov eax, [esi+0]; and eax, eax; jnz .l0
.l0:popad; ret

