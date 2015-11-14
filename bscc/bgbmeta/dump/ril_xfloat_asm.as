ril_hf2float:
pushad
mov eax, [esp+36]
mov edx, eax
and edx, 1023; shl edx, 13
mov ecx, eax; shr ecx, 10; and ecx, 31
jz .l0
cmp ecx, 31; jz .l1
add ecx, 112; jmp .l0
.l1: mov ecx, 255
.l0:
shl ecx, 23; or edx, ecx
bt eax, 15; jnc .l2
or edx, 0x80000000; .l2:
mov [esp+36], edx
popad; ret
ril_float2hf:
pushad
mov eax, [esp+36]
mov edx, eax
and edx, 0x007FFFFF; shr edx, 13
mov ecx, eax; shr ecx, 23; and ecx, 255
jz .l0
cmp ecx, 255; jz .l1
sub ecx, 112
cmp ecx, 0; jle .l2
cmp ecx, 31; jge .l3
jmp .l0
.l2: xor edx, edx; jmp .l5
.l3: mov edx, 0x7C00; jmp .l4
.l1: mov ecx, 31
.l0:
shl ecx, 10; or edx, ecx
.l4:
bt eax, 31; jnc .l6
or edx, 0x8000; .l6:
.l5:
mov [esp+36], edx
popad; ret
ril_add_f128:
pushad; lea esi, [esp+36]
fld80 [esi+22]; fld80 [esi+6]
faddp
fstp80 [esi+6]
popad; ret
ril_sub_f128:
pushad; lea esi, [esp+36]
fld80 [esi+22]; fld80 [esi+6]
fsubp
fstp80 [esi+6]
popad; ret
ril_mul_f128:
pushad; lea esi, [esp+36]
fld80 [esi+22]; fld80 [esi+6]
fmulp
fstp80 [esi+6]
popad; ret
ril_div_f128:
pushad; lea esi, [esp+36]
fld80 [esi+22]; fld80 [esi+6]
fdivp
fstp80 [esi+6]
popad; ret
ril_cmp_f128:
pushad; lea esi, [esp+36]
fld80 [esi+22]; fld80 [esi+6]
faddp
fstp80 [esi+6]
fcomip
ffree st0; fincstp
popad; ret

