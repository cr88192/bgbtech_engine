; Listing generated by Microsoft (R) Optimizing Compiler Version 14.00.50727.42 

include listing.inc

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

_DATA	SEGMENT
$SG4545	DB	'%f', 0aH, 00H
$SG4550	DB	'%f', 0aH, 00H
_DATA	ENDS
PUBLIC	vec_dot1
EXTRN	_fltused:DWORD
pdata	SEGMENT
$pdata$vec_dot1 DD imagerel $LN6
	DD	imagerel $LN6+116
	DD	imagerel $unwind$vec_dot1
pdata	ENDS
xdata	SEGMENT
$unwind$vec_dot1 DD 011301H
	DD	02213H
; Function compile flags: /Odtp
xdata	ENDS
_TEXT	SEGMENT
i$ = 0
f$ = 4
a$ = 32
b$ = 40
len$ = 48
vec_dot1 PROC
; File j:\bgb.proj.pdsys41\bscc\dottest.c
; Line 8
$LN6:
	mov	DWORD PTR [rsp+24], r8d
	mov	QWORD PTR [rsp+16], rdx
	mov	QWORD PTR [rsp+8], rcx
	sub	rsp, 24
; Line 11
	xorps	xmm0, xmm0
	movss	DWORD PTR f$[rsp], xmm0
; Line 12
	mov	DWORD PTR i$[rsp], 0
	jmp	SHORT $LN3@vec_dot1
$LN2@vec_dot1:
	mov	eax, DWORD PTR i$[rsp]
	add	eax, 1
	mov	DWORD PTR i$[rsp], eax
$LN3@vec_dot1:
	mov	eax, DWORD PTR len$[rsp]
	cmp	DWORD PTR i$[rsp], eax
	jge	SHORT $LN1@vec_dot1
	movsxd	rax, DWORD PTR i$[rsp]
	movsxd	rcx, DWORD PTR i$[rsp]
	mov	rdx, QWORD PTR a$[rsp]
	mov	r8, QWORD PTR b$[rsp]
	movss	xmm0, DWORD PTR [rdx+rax*4]
	mulss	xmm0, DWORD PTR [r8+rcx*4]
	movss	xmm1, DWORD PTR f$[rsp]
	addss	xmm1, xmm0
	movaps	xmm0, xmm1
	movss	DWORD PTR f$[rsp], xmm0
	jmp	SHORT $LN2@vec_dot1
$LN1@vec_dot1:
; Line 13
	movss	xmm0, DWORD PTR f$[rsp]
; Line 14
	add	rsp, 24
	ret	0
vec_dot1 ENDP
_TEXT	ENDS
PUBLIC	vec_dot2
pdata	SEGMENT
$pdata$vec_dot2 DD imagerel $LN5
	DD	imagerel $LN5+106
	DD	imagerel $unwind$vec_dot2
pdata	ENDS
xdata	SEGMENT
$unwind$vec_dot2 DD 011301H
	DD	02213H
; Function compile flags: /Odtp
xdata	ENDS
_TEXT	SEGMENT
f$ = 0
a$ = 32
b$ = 40
len$ = 48
vec_dot2 PROC
; Line 17
$LN5:
	mov	DWORD PTR [rsp+24], r8d
	mov	QWORD PTR [rsp+16], rdx
	mov	QWORD PTR [rsp+8], rcx
	sub	rsp, 24
; Line 19
	xorps	xmm0, xmm0
	movss	DWORD PTR f$[rsp], xmm0
$LN2@vec_dot2:
; Line 20
	mov	eax, DWORD PTR len$[rsp]
	mov	ecx, DWORD PTR len$[rsp]
	sub	ecx, 1
	mov	DWORD PTR len$[rsp], ecx
	test	eax, eax
	je	SHORT $LN1@vec_dot2
	movsxd	rax, DWORD PTR len$[rsp]
	movsxd	rcx, DWORD PTR len$[rsp]
	mov	rdx, QWORD PTR a$[rsp]
	mov	r8, QWORD PTR b$[rsp]
	movss	xmm0, DWORD PTR [rdx+rax*4]
	mulss	xmm0, DWORD PTR [r8+rcx*4]
	movss	xmm1, DWORD PTR f$[rsp]
	addss	xmm1, xmm0
	movaps	xmm0, xmm1
	movss	DWORD PTR f$[rsp], xmm0
	jmp	SHORT $LN2@vec_dot2
$LN1@vec_dot2:
; Line 21
	movss	xmm0, DWORD PTR f$[rsp]
; Line 22
	add	rsp, 24
	ret	0
vec_dot2 ENDP
_TEXT	ENDS
PUBLIC	__real@447a0000
PUBLIC	bscc_main
EXTRN	printf:PROC
EXTRN	clock:PROC
pdata	SEGMENT
$pdata$bscc_main DD imagerel $LN9
	DD	imagerel $LN9+269
	DD	imagerel $unwind$bscc_main
pdata	ENDS
xdata	SEGMENT
$unwind$bscc_main DD 020701H
	DD	0490107H
xdata	ENDS
;	COMDAT __real@447a0000
CONST	SEGMENT
__real@447a0000 DD 0447a0000r			; 1000
; Function compile flags: /Odtp
CONST	ENDS
_TEXT	SEGMENT
i$ = 32
v1$ = 48
v0$ = 304
j$ = 560
bscc_main PROC
; Line 25
$LN9:
	sub	rsp, 584				; 00000248H
; Line 29
	call	clock
	mov	DWORD PTR i$[rsp], eax
; Line 31
	mov	DWORD PTR i$[rsp], 0
	jmp	SHORT $LN6@bscc_main
$LN5@bscc_main:
	mov	eax, DWORD PTR j$[rsp]
	add	eax, 1
	mov	DWORD PTR j$[rsp], eax
$LN6@bscc_main:
	cmp	DWORD PTR j$[rsp], 10000000		; 00989680H
	jge	SHORT $LN4@bscc_main
; Line 32
	mov	r8d, 64					; 00000040H
	lea	rdx, QWORD PTR v1$[rsp]
	lea	rcx, QWORD PTR v0$[rsp]
	call	vec_dot1
	jmp	SHORT $LN5@bscc_main
$LN4@bscc_main:
; Line 34
	call	clock
	sub	eax, DWORD PTR i$[rsp]
	mov	DWORD PTR i$[rsp], eax
; Line 35
	cvtsi2ss xmm0, DWORD PTR i$[rsp]
	divss	xmm0, DWORD PTR __real@447a0000
	cvtss2sd xmm0, xmm0
	movsdx	xmm1, xmm0
	movd	rdx, xmm1
	lea	rcx, OFFSET FLAT:$SG4545
	call	printf
; Line 38
	call	clock
	mov	DWORD PTR i$[rsp], eax
; Line 40
	mov	DWORD PTR i$[rsp], 0
	jmp	SHORT $LN3@bscc_main
$LN2@bscc_main:
	mov	eax, DWORD PTR j$[rsp]
	add	eax, 1
	mov	DWORD PTR j$[rsp], eax
$LN3@bscc_main:
	cmp	DWORD PTR j$[rsp], 10000000		; 00989680H
	jge	SHORT $LN1@bscc_main
; Line 41
	mov	r8d, 64					; 00000040H
	lea	rdx, QWORD PTR v1$[rsp]
	lea	rcx, QWORD PTR v0$[rsp]
	call	vec_dot2
	jmp	SHORT $LN2@bscc_main
$LN1@bscc_main:
; Line 43
	call	clock
	sub	eax, DWORD PTR i$[rsp]
	mov	DWORD PTR i$[rsp], eax
; Line 44
	cvtsi2ss xmm0, DWORD PTR i$[rsp]
	divss	xmm0, DWORD PTR __real@447a0000
	cvtss2sd xmm0, xmm0
	movsdx	xmm1, xmm0
	movd	rdx, xmm1
	lea	rcx, OFFSET FLAT:$SG4550
	call	printf
; Line 45
	add	rsp, 584				; 00000248H
	ret	0
bscc_main ENDP
_TEXT	ENDS
PUBLIC	main
pdata	SEGMENT
$pdata$main DD	imagerel $LN3
	DD	imagerel $LN3+16
	DD	imagerel $unwind$main
pdata	ENDS
xdata	SEGMENT
$unwind$main DD	010401H
	DD	04204H
; Function compile flags: /Odtp
xdata	ENDS
_TEXT	SEGMENT
main	PROC
; Line 48
$LN3:
	sub	rsp, 40					; 00000028H
; Line 49
	call	bscc_main
; Line 50
	xor	eax, eax
	add	rsp, 40					; 00000028H
	ret	0
main	ENDP
_TEXT	ENDS
END
