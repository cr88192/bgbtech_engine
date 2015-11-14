ril_abs_edax:
cmp edx, 0; jge .l0
not eax; not edx
add eax, 1; adc edx, 0
.l0: ret
ril_abs_edsi:
cmp edi, 0; jge .l0
not esi; not edi
add esi, 1; adc edi, 0
.l0: ret
ril_div_edax_edsi_2x:
xor ebp, ebp; xor ebx, ebx
mov ecx, 64
.l0:
shl eax, 1; rcl edx, 1
rcl ebx, 1; rcl ebp, 1
cmp ebp, edi; jb .l1; ja .l2
cmp ebx, esi; jb .l1
.l2:
sub ebx, esi; sbb ebp, edi
add eax, 1; adc edx, 0
.l1: loop .l0
ret
ril_div_edax_edsi_2u:
push ebp; push ebx; push ecx
call ril_div_edax_edsi_2x
mov edi, ebp; mov esi, ebx
pop ecx; pop ebx; pop ebp
ret
ril_div_edax_edsi:
push ebp; push ebx; push edi; push esi; push ecx
mov ecx, edx; xor ecx, edi
push ecx
call ril_abs_edax; call ril_abs_edsi
call ril_div_edax_edsi_2x
pop ecx
bt ecx, 31; jnc .l0
not eax; not edx; add eax, 1; adc edx, 0
.l0:
pop ecx; pop esi; pop edi; pop ebx; pop ebp
ret
ril_mod_edax_edsi:
push ebp; push ebx; push edi; push esi; push ecx
push edx
call ril_abs_edax; call ril_abs_edsi
call ril_div_edax_edsi_2x
mov edx, ebp; mov eax, ebx
pop ecx
bt ecx, 31; jnc .l0
not eax; not edx; add eax, 1; adc edx, 0
.l0:
pop ecx; pop esi; pop edi; pop ebx; pop ebp
ret
ril_mul_m64_m64:
pushad
mov eax, [esp+36]; mov edx, [esp+40]
mov esi, [esp+44]; mov edi, [esp+48]
mov ecx, eax; mov ebx, edx
imul esi
imul ecx, edi; add edx, ecx
imul ebx, esi; add edx, ebx
mov [esp+36], eax; mov [esp+40], edx
popad; ret
ril_div_m64_m64:
pushad
mov eax, [esp+36]; mov edx, [esp+40]
mov esi, [esp+44]; mov edi, [esp+48]
mov ecx, edx; xor ecx, edi; push ecx
call ril_abs_edax; call ril_abs_edsi
call ril_div_edax_edsi_2x
pop ecx
bt ecx, 31; jnc .l0
not eax; not edx; add eax, 1; adc edx, 0
.l0:
mov [esp+36], eax; mov [esp+40], edx
mov [esp+44], ebx; mov [esp+48], ebp
popad; ret

