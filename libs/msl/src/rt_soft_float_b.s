; Soft-float runtime, second part: double and float comparisons, single-precision divide, multiply and
; subtract, float conversions, and the 64-bit integer divide, remainder and multiply. Register use as in
; rt_soft_float_a.s: a float or int in r0 (second operand r1), a double or 64-bit integer in r1:r0 (high:low),
; the second operand in r3:r2.
; The comparison routines return their result both in r0 and in the condition flags.
; _fsub branches into _fadd (.L_0204cb30, rt_soft_float_a.s) and _fadd into _fsub (.L_0204d64c, global here);
; _ll_sdiv calls _s32_div_f and _ull_mod calls .L_0204dcf4 inside _u32_div_f (both in rt_div32.s).
; "Not compiled" lines say why the compiler cannot have produced a routine. Save rule: the stmfd list of a
; compiled ARM function holds every register among r4-r7 it writes and, unless it is an interrupt function, also
; every one among r8-r11; lr if it calls or writes lr; and r3 when the saved registers and its stack frame would
; otherwise come to an odd number of words, or in place of a one-word frame. r0-r2 and r12 appear only in an
; interrupt function, which saves every register among r0-r7 it writes. A compiled function that saved lr
; returns by loading it into pc (a variadic one, which starts with stmfd {r0-r3}, excepted).

	.text
	.global .L_0204d64c
	.extern .L_0204cb30
	.extern _s32_div_f
	.extern .L_0204dcf4

; _dls: returns 1 if a < b (doubles a in r1:r0, b in r3:r2), else 0. A NaN in either operand
; gives 0 (unordered); +0 and -0 compare equal. Unordered and the +0/-0 case give 0 with C set.
; Non-negative operands compare as unsigned 64-bit values; if either is negative the raw bits
; are compared in reverse order, which is correct for two negatives and for mixed signs.
; Not compiled: sets the carry flag of its result with mrs/msr, which C has no expression for.
	.global _dls
	.type _dls, @function
_dls:
	mov ip, #0x200000
	cmn ip, r1, lsl #0x1
	bhs .L_dls_5
	cmn ip, r3, lsl #0x1
	bhs .L_dls_6
	; ordered: compare
.L_dls_1:
	orrs ip, r3, r1
	bmi .L_dls_3
	cmp r1, r3
	cmpeq r0, r2
	movlo r0, #0x1
	movhs r0, #0x0
	bx lr
	; unordered (NaN)
.L_dls_2:
	mov r0, #0x0
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	; at least one negative: equal zeros check, then reversed compare
.L_dls_3:
	orr ip, r0, ip, lsl #0x1
	orrs ip, ip, r2
	moveq r0, #0x0
	bne .L_dls_4
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bxeq lr
	; compare reversed (negative operands)
.L_dls_4:
	cmp r3, r1
	cmpeq r2, r0
	movlo r0, #0x1
	movhs r0, #0x0
	bx lr
	; first operand has exponent 0x7ff: NaN if mantissa nonzero
.L_dls_5:
	bne .L_dls_2
	cmp r0, #0x0
	bhi .L_dls_2
	cmn ip, r3, lsl #0x1
	blo .L_dls_1
	; second operand has exponent 0x7ff: NaN if mantissa nonzero
.L_dls_6:
	bne .L_dls_2
	cmp r2, #0x0
	bhi .L_dls_2
	b .L_dls_1
	.size _dls, .-_dls

; _deq: returns 1 if a == b (doubles), else 0. NaN gives 0 with Z cleared; +0 equals -0;
; otherwise both words must be equal.
; Not compiled: sets the Z flag of its result with mrs/msr, which C has no expression for.
	.global _deq
	.type _deq, @function
_deq:
	mov ip, #0x200000
	cmn ip, r1, lsl #0x1
	bhs .L_deq_4
	cmn ip, r3, lsl #0x1
	bhs .L_deq_5
	; ordered: compare
.L_deq_1:
	orrs ip, r3, r1
	bmi .L_deq_3
	cmp r1, r3
	cmpeq r0, r2
	moveq r0, #0x1
	movne r0, #0x0
	bx lr
	; unordered (NaN)
.L_deq_2:
	mov r0, #0x0
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
	; at least one negative: equal zeros check, then reversed compare
.L_deq_3:
	orr ip, r0, ip, lsl #0x1
	orrs ip, ip, r2
	moveq r0, #0x1
	bxeq lr
	cmp r3, r1
	cmpeq r2, r0
	moveq r0, #0x1
	movne r0, #0x0
	bx lr
	; first operand has exponent 0x7ff: NaN if mantissa nonzero
.L_deq_4:
	bne .L_deq_2
	cmp r0, #0x0
	bhi .L_deq_2
	cmn ip, r3, lsl #0x1
	blo .L_deq_1
	; second operand has exponent 0x7ff: NaN if mantissa nonzero
.L_deq_5:
	bne .L_deq_2
	cmp r2, #0x0
	bhi .L_deq_2
	b .L_deq_1
	.size _deq, .-_deq

; _fgeq: returns 1 if a >= b (floats a in r0, b in r1), else 0; C cleared when unordered.
; A NaN (magnitude above 0xff000000 when shifted left once) in either operand gives 0.
; Otherwise each operand is converted from sign-magnitude to two's complement and the two
; are compared as signed integers, so +0 and -0 compare equal. The flags are left set to
; match the returned condition.
; Not compiled: sets the carry flag of its result with mrs/msr, which C has no expression for.
	.global _fgeq
	.type _fgeq, @function
_fgeq:
	mov r3, #0xff000000
	cmp r3, r0, lsl #0x1
	cmphs r3, r1, lsl #0x1
	blo .L_fgeq_1
	; sign-magnitude to two's complement for each operand
	cmp r0, #0x0
	bicmi r0, r0, #0x80000000
	rsbmi r0, r0, #0x0
	cmp r1, #0x0
	bicmi r1, r1, #0x80000000
	rsbmi r1, r1, #0x0
	cmp r0, r1
	movge r0, #0x1
	movlt r0, #0x0
	mrs ip, cpsr
	biclt ip, ip, #0x20000000
	orrge ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	; unordered (NaN) result
.L_fgeq_1:
	mov r0, #0x0
	mrs ip, cpsr
	bic ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	.size _fgeq, .-_fgeq

; _fls: returns 1 if a < b (floats a in r0, b in r1), else 0; unordered: C set.
; A NaN (magnitude above 0xff000000 when shifted left once) in either operand gives 0.
; Otherwise each operand is converted from sign-magnitude to two's complement and the two
; are compared as signed integers, so +0 and -0 compare equal. The flags are left set to
; match the returned condition.
; Not compiled: sets the carry flag of its result with mrs/msr, which C has no expression for.
	.global _fls
	.type _fls, @function
_fls:
	mov r3, #0xff000000
	cmp r3, r0, lsl #0x1
	cmphs r3, r1, lsl #0x1
	blo .L_fls_1
	; sign-magnitude to two's complement for each operand
	cmp r0, #0x0
	bicmi r0, r0, #0x80000000
	rsbmi r0, r0, #0x0
	cmp r1, #0x0
	bicmi r1, r1, #0x80000000
	rsbmi r1, r1, #0x0
	cmp r0, r1
	movlt r0, #0x1
	movge r0, #0x0
	mrs ip, cpsr
	orrge ip, ip, #0x20000000
	biclt ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	; unordered (NaN) result
.L_fls_1:
	mov r0, #0x0
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	.size _fls, .-_fls

; _fneq: returns 1 if a != b (floats), else 0. NaN -> 1 with Z cleared. Two zeros of either
; sign are equal (returns 0); otherwise the raw bit patterns are compared.
; Not compiled: sets the Z flag of its result with mrs/msr, which C has no expression for.
	.global _fneq
	.type _fneq, @function
_fneq:
	mov r3, #0xff000000
	cmp r3, r0, lsl #0x1
	blo .L_fneq_2
	cmp r3, r1, lsl #0x1
	blo .L_fneq_2
	; both zero (ignoring sign)?
	orr r3, r0, r1
	movs r3, r3, lsl #0x1
	moveq r0, #0x0
	bne .L_fneq_1
	mrs ip, cpsr
	orr ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
	; not both zero: compare raw bits
.L_fneq_1:
	cmp r0, r1
	movne r0, #0x1
	moveq r0, #0x0
	mrs ip, cpsr
	bicne ip, ip, #0x40000000
	orreq ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
	; unordered (NaN): not equal
.L_fneq_2:
	mov r0, #0x1
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
	.size _fneq, .-_fneq

; _frdiv: reverse-operand entry for _fdiv: swaps the two float operands with three
; exclusive-ors and falls through into _fdiv, which follows directly.
; Not compiled: has no return; execution runs on into _fdiv, the next routine. Each compiled function
; is a section of its own, so none can rely on the code placed after it.
	.global _frdiv
	.type _frdiv, @function
_frdiv:
	eor r0, r0, r1
	eor r1, r0, r1
	eor r0, r0, r1
	.size _frdiv, .-_frdiv

; _fdiv: single-precision divide, r0 = a / b (a in r0, b in r1), round to nearest even.
; Result sign is a^b. Normal case: both mantissas get the implicit bit; if a's mantissa is
; smaller than b's it is doubled and the exponent drops by one. The quotient is formed from a
; reciprocal of b: an initial estimate comes from a 256-entry byte table indexed by the top
; bits of b's mantissa, then refined with a few multiply steps, and finally multiplied by a's
; mantissa (umull). Exponent = ea - eb + bias; underflow goes to the denormal path, overflow to
; infinity. When the low bits of the product are in the ambiguous range, the remainder is
; recomputed exactly to decide the last rounding bit.
; Special cases: NaN operand, 0/0 and inf/inf return the quiet NaN 0x7fffffff; x/0 and
; inf/finite return a signed infinity; 0/finite and finite/inf return a signed zero. Denormal
; operands are normalised with clz first.
; Not compiled: clz has no C expression; and (save rule) it saves lr alone with no frame and no r3, then
; pops it and returns with bx lr.
	.global _fdiv
	.type _fdiv, @function
_fdiv:
	stmdb sp!, {lr}
	mov ip, #0xff
	ands r3, ip, r0, lsr #0x17
	cmpne r3, #0xff
	beq .L_fdiv_4
	ands ip, ip, r1, lsr #0x17
	cmpne ip, #0xff
	beq .L_fdiv_5
	orr r1, r1, #0x800000
	orr r0, r0, #0x800000
	bic r2, r0, #0xff000000
	bic lr, r1, #0xff000000
	; both operands are now normalised mantissas (a in r2, b in lr) with exponents in r3 / ip
.L_fdiv_1:
	cmp r2, lr
	movlo r2, r2, lsl #0x1
	sublo r3, r3, #0x1
	teq r0, r1
	sub r0, pc, #0x94
	ldrb r1, [r0, lr, lsr #0xf]
	rsb lr, lr, #0x0
	mov r0, lr, asr #0x1
	mul r0, r1, r0
	add r0, r0, #0x80000000
	mov r0, r0, lsr #0x6
	mul r0, r1, r0
	mov r0, r0, lsr #0xe
	mul r1, lr, r0
	sub ip, r3, ip
	mov r1, r1, lsr #0xc
	mul r1, r0, r1
	mov r0, r0, lsl #0xe
	add r0, r0, r1, lsr #0xf
	umull r1, r0, r2, r0
	mov r3, r0
	orrmi r0, r0, #0x80000000
	adds ip, ip, #0x7e
	bmi .L_fdiv_12
	cmp ip, #0xfe
	bge .L_fdiv_19
	add r0, r0, ip, lsl #0x17
	mov ip, r1, lsr #0x1c
	cmp ip, #0x7
	beq .L_fdiv_3
	add r0, r0, r1, lsr #0x1f
	ldmia sp!, {lr}
	bx lr
	; reciprocal-estimate table (256 bytes), addressed pc-relative above
.L_fdiv_table:
	.word 0xfdfeffff
	.word 0xf9fafbfc
	.word 0xf5f6f7f8
	.word 0xf1f2f3f4
	.word 0xeeeff0f0
	.word 0xeaebeced
	.word 0xe7e8e9ea
	.word 0xe4e5e6e6
	.word 0xe1e2e2e3
	.word 0xdedfdfe0
	.word 0xdbdcdcdd
	.word 0xd8d9d9da
	.word 0xd5d6d7d7
	.word 0xd2d3d4d4
	.word 0xd0d0d1d2
	.word 0xcdcececf
	.word 0xcbcbcccc
	.word 0xc8c9c9ca
	.word 0xc6c6c7c8
	.word 0xc3c4c5c5
	.word 0xc1c2c2c3
	.word 0xbfbfc0c0
	.word 0xbdbdbebe
	.word 0xbabbbcbc
	.word 0xb8b9b9ba
	.word 0xb6b7b7b8
	.word 0xb4b5b5b6
	.word 0xb2b3b3b4
	.word 0xb0b1b1b2
	.word 0xafafafb0
	.word 0xadadaeae
	.word 0xababacac
	.word 0xa9aaaaaa
	.word 0xa7a8a8a9
	.word 0xa6a6a7a7
	.word 0xa4a4a5a5
	.word 0xa2a3a3a4
	.word 0xa1a1a2a2
	.word 0x9fa0a0a0
	.word 0x9e9e9e9f
	.word 0x9c9d9d9d
	.word 0x9b9b9b9c
	.word 0x999a9a9a
	.word 0x98989999
	.word 0x96979798
	.word 0x95959696
	.word 0x94949495
	.word 0x92939393
	.word 0x91919292
	.word 0x90909191
	.word 0x8f8f8f90
	.word 0x8d8e8e8e
	.word 0x8c8c8d8d
	.word 0x8b8b8c8c
	.word 0x8a8a8a8b
	.word 0x8989898a
	.word 0x88888888
	.word 0x86878787
	.word 0x85868686
	.word 0x84858585
	.word 0x83838484
	.word 0x82828383
	.word 0x81818282
	.word 0x80808181
	; near-tie: check the exact remainder to decide whether to round up
.L_fdiv_3:
	mov r1, r3, lsl #0x1
	add r1, r1, #0x1
	rsb lr, lr, #0x0
	mul r1, lr, r1
	cmp r1, r2, lsl #0x18
	addmi r0, r0, #0x1
	ldmia sp!, {lr}
	bx lr
	; a has exponent 0 or 255
.L_fdiv_4:
	eor lr, r0, r1
	and lr, lr, #0x80000000
	cmp r3, #0x0
	beq .L_fdiv_7
	movs r0, r0, lsl #0x9
	bne .L_fdiv_17
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x9
	ands ip, ip, #0xff
	beq .L_fdiv_16
	cmp ip, #0xff
	blt .L_fdiv_16
	cmp r1, #0x0
	beq .L_fdiv_18
	b .L_fdiv_15
	; a is normal, b has exponent 0 or 255
.L_fdiv_5:
	eor lr, r0, r1
	and lr, lr, #0x80000000
	cmp ip, #0x0
	beq .L_fdiv_10
.L_fdiv_6:
	movs r1, r1, lsl #0x9
	bne .L_fdiv_15
	b .L_fdiv_21
	; a has exponent 0: zero or denormal
.L_fdiv_7:
	movs r2, r0, lsl #0x9
	beq .L_fdiv_8
	clz r3, r2
	movs r2, r2, lsl r3
	rsb r3, r3, #0x0
	mov r2, r2, lsr #0x8
	ands ip, ip, r1, lsr #0x17
	beq .L_fdiv_11
	cmp ip, #0xff
	beq .L_fdiv_6
	orr r1, r1, #0x800000
	bic lr, r1, #0xff000000
	b .L_fdiv_1
	; a is zero: result is zero unless b is zero or NaN
.L_fdiv_8:
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x9
	ands ip, ip, #0xff
	beq .L_fdiv_9
	cmp ip, #0xff
	blt .L_fdiv_21
	cmp r1, #0x0
	beq .L_fdiv_21
	b .L_fdiv_15
.L_fdiv_9:
	cmp r1, #0x0
	beq .L_fdiv_18
	b .L_fdiv_21
	; b is zero or denormal: zero gives infinity, else normalise b
.L_fdiv_10:
	movs ip, r1, lsl #0x9
	beq .L_fdiv_16
	mov lr, ip
	clz ip, lr
	movs lr, lr, lsl ip
	rsb ip, ip, #0x0
	mov lr, lr, lsr #0x8
	orr r0, r0, #0x800000
	bic r2, r0, #0xff000000
	b .L_fdiv_1
	; a already normalised, b is zero or denormal
.L_fdiv_11:
	movs ip, r1, lsl #0x9
	beq .L_fdiv_16
	mov lr, ip
	clz ip, lr
	movs lr, lr, lsl ip
	rsb ip, ip, #0x0
	mov lr, lr, lsr #0x8
	b .L_fdiv_1
	; exponent below range: build a denormal (or zero) result; the shifted quotient is
	; corrected and rounded (nearest, ties to even) with exact remainder checks
.L_fdiv_12:
	and r0, r0, #0x80000000
	cmn ip, #0x18
	beq .L_fdiv_14
	bmi .L_fdiv_20
	add r1, ip, #0x17
	mov r2, r2, lsl r1
	rsb ip, ip, #0x0
	mov r3, r3, lsr ip
	orr r0, r0, r3
	rsb lr, lr, #0x0
	mul r1, lr, r3
	cmp r1, r2
	ldmeqia sp!, {lr}
	bxeq lr
	add r1, r1, lr
	cmp r1, r2
	beq .L_fdiv_13
	addmi r0, r0, #0x1
	subpl r1, r1, lr
	add r1, lr, r1, lsl #0x1
	cmp r1, r2, lsl #0x1
	and r3, r0, #0x1
	addmi r0, r0, #0x1
	addeq r0, r0, r3
	ldmia sp!, {lr}
	bx lr
	; quotient was one too low and is exact after the increment
.L_fdiv_13:
	add r0, r0, #0x1
	ldmia sp!, {lr}
	bx lr
	; exponent -24: rounds up to the smallest denormal unless the mantissas are equal
	; (an exact half, which rounds to the even zero)
.L_fdiv_14:
	cmn r2, lr
	addne r0, r0, #0x1
	ldmia sp!, {lr}
	bx lr
	; NaN
.L_fdiv_15:
	mov r0, r1
	b .L_fdiv_17
	; return signed infinity
.L_fdiv_16:
	mov r0, #0xff000000
	orr r0, lr, r0, lsr #0x1
	ldmia sp!, {lr}
	bx lr
	; return quiet NaN
.L_fdiv_17:
	mvn r0, #0x80000000
	ldmia sp!, {lr}
	bx lr
.L_fdiv_18:
	mvn r0, #0x80000000
	ldmia sp!, {lr}
	bx lr
	; exponent overflow: return signed infinity
.L_fdiv_19:
	tst r0, #0x80000000
	mov r0, #0xff000000
	movne r0, r0, asr #0x1
	moveq r0, r0, lsr #0x1
	ldmia sp!, {lr}
	bx lr
	; result underflows to zero keeping only the sign
.L_fdiv_20:
	ldmia sp!, {lr}
	bx lr
	; return signed zero (sign in lr)
.L_fdiv_21:
	mov r0, lr
	ldmia sp!, {lr}
	bx lr
	.size _fdiv, .-_fdiv

; _f2d: widen a float (r0) to a double (r1:r0).
; Normal numbers: rebias the exponent (+0x380) and shift the 23-bit mantissa into the 52-bit
; field. Zero maps to signed zero; a float denormal is normalised with clz so it becomes a
; normal double; infinity becomes the double infinity; any NaN becomes 0x7fffffff_ffffffff.
; Not compiled: clz has no C expression.
	.global _f2d
	.type _f2d, @function
_f2d:
	and r2, r0, #0x80000000
	mov ip, r0, lsr #0x17
	mov r3, r0, lsl #0x9
	ands ip, ip, #0xff
	beq .L_f2d_2
	cmp ip, #0xff
	beq .L_f2d_4
	; assemble sign | exponent | mantissa
.L_f2d_1:
	add ip, ip, #0x380
	mov r0, r3, lsl #0x14
	orr r1, r2, r3, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	bx lr
	; exponent 0: zero or denormal
.L_f2d_2:
	cmp r3, #0x0
	bne .L_f2d_3
	mov r1, r2
	mov r0, #0x0
	bx lr
.L_f2d_3:
	mov r3, r3, lsr #0x1
	clz ip, r3
	movs r3, r3, lsl ip
	rsb ip, ip, #0x1
	add r3, r3, r3
	b .L_f2d_1
	; exponent 255: infinity or NaN
.L_f2d_4:
	cmp r3, #0x0
	bhi .L_f2d_5
	ldr r1, .L_f2d_pool_7ff00000
	orr r1, r1, r2
	mov r0, #0x0
	bx lr
.L_f2d_5:
	mvn r0, #0x0
	bic r1, r0, #0x80000000
	bx lr
.L_f2d_pool_7ff00000:
	.word 0x7ff00000
	.size _f2d, .-_f2d

; _ffix: float (r0) to signed int, truncating toward zero.
; The shift is 0x9e minus the biased exponent; if it is not positive (|a| >= 2^31, infinity,
; NaN) the result saturates to 0x7fffffff for a non-negative input or 0x80000000 for a negative
; one. Otherwise the mantissa with its implicit bit is shifted right and negated if a < 0.
; Not compiled: `subs r2, r2, r1, lsr #0x17` is followed by `ble`.
; The compiler makes a flag-setting subtract only by folding a following `cmp rd, #0` into it, which leaves
; the branch testing eq, ne, mi or pl (ge and lt become pl and mi, hs becomes always; lo, gt and le are not
; folded), or inside 64-bit subtract, compare and shift sequences, where the next sbc, sbcs or conditional orr
; reads the flags.
	.global _ffix
	.type _ffix, @function
_ffix:
	bic r1, r0, #0x80000000
	mov r2, #0x9e
	subs r2, r2, r1, lsr #0x17
	ble .L_ffix_1
	mov r1, r1, lsl #0x8
	orr r1, r1, #0x80000000
	cmp r0, #0x0
	mov r0, r1, lsr r2
	rsbmi r0, r0, #0x0
	bx lr
	; out of range: saturate
.L_ffix_1:
	mvn r0, r0, asr #0x1f
	add r0, r0, #0x80000000
	bx lr
	.size _ffix, .-_ffix

; _fflt: signed int (r0) to float, round to nearest even.
; Takes the magnitude (remembering the sign), returns 0 for 0, normalises with clz, sets the
; exponent to 0x9e - clz, packs sign | exponent | top 23 mantissa bits, and uses the 8 bits
; shifted out below the mantissa to round: exact or below half truncates, above half rounds up,
; exactly half rounds to even.
; Not compiled: clz has no C expression.
	.global _fflt
	.type _fflt, @function
_fflt:
	ands r2, r0, #0x80000000
	rsbmi r0, r0, #0x0
	cmp r0, #0x0
	bxeq lr
	clz r3, r0
	movs r0, r0, lsl r3
	rsb r3, r3, #0x9e
	ands r1, r0, #0xff
	add r0, r0, r0
	orr r0, r2, r0, lsr #0x9
	orr r0, r0, r3, lsl #0x17
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r3, r1, #0x7f
	andeqs r3, r0, #0x1
	addne r0, r0, #0x1
	bx lr
	.size _fflt, .-_fflt

; _fmul: single-precision multiply, r0 = a * b, round to nearest even.
; Sign is a^b. Both mantissas are left-aligned with the implicit bit at bit 31 and multiplied
; with umull; if the product's top bit is clear it is doubled and the exponent reduced. The
; exponent is ea + eb - 0x7f (less one when doubled); packing then adds the implicit bit into
; the exponent field. Too large gives a signed infinity; too small goes to a denormal
; path (shift right with a sticky bit, round to even) or to signed zero. Rounding uses the 8
; bits below the mantissa plus a sticky bit from the low product word.
; Special cases: NaN operand or 0 * inf return 0x7fffffff; inf * finite gives signed infinity;
; zero * finite gives signed zero; denormal inputs are normalised with clz.
; Not compiled: clz has no C expression.
	.global _fmul
	.type _fmul, @function
_fmul:
	eor r2, r0, r1
	and r2, r2, #0x80000000
	mov ip, #0xff
	ands r3, ip, r0, lsr #0x17
	mov r0, r0, lsl #0x8
	cmpne r3, #0xff
	beq .L_fmul_2
	orr r0, r0, #0x80000000
	ands ip, ip, r1, lsr #0x17
	mov r1, r1, lsl #0x8
	cmpne ip, #0xff
	beq .L_fmul_4
	orr r1, r1, #0x80000000
	; operands normalised: a in r0, b in r1 (bit 31 set), exponents in r3 / ip
.L_fmul_1:
	add ip, r3, ip
	umull r1, r3, r0, r1
	movs r0, r3
	addpl r0, r0, r0
	subpl ip, ip, #0x1
	subs ip, ip, #0x7f
	bmi .L_fmul_9
	cmp ip, #0xfe
	bge .L_fmul_14
	ands r3, r0, #0xff
	orr r0, r2, r0, lsr #0x8
	add r0, r0, ip, lsl #0x17
	tst r3, #0x80
	bxeq lr
	orrs r1, r1, r3, lsl #0x19
	andeqs r3, r0, #0x1
	addne r0, r0, #0x1
	bx lr
	; a has exponent 0 or 255
.L_fmul_2:
	cmp r3, #0x0
	beq .L_fmul_6
	movs r0, r0, lsl #0x1
	bne .L_fmul_12
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x9
	ands ip, ip, #0xff
	beq .L_fmul_3
	cmp ip, #0xff
	blt .L_fmul_11
	cmp r1, #0x0
	beq .L_fmul_11
	b .L_fmul_12
.L_fmul_3:
	cmp r1, #0x0
	beq .L_fmul_13
	b .L_fmul_11
	; b has exponent 0 or 255
.L_fmul_4:
	cmp ip, #0x0
	beq .L_fmul_8
.L_fmul_5:
	movs r1, r1, lsl #0x1
	bne .L_fmul_12
	b .L_fmul_11
	; a is zero or denormal
.L_fmul_6:
	movs r0, r0, lsl #0x1
	beq .L_fmul_7
	mov r0, r0, lsr #0x1
	clz r3, r0
	movs r0, r0, lsl r3
	rsb r3, r3, #0x1
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x8
	ands ip, ip, #0xff
	beq .L_fmul_8
	cmp ip, #0xff
	beq .L_fmul_5
	orr r1, r1, #0x80000000
	b .L_fmul_1
	; a is zero: result is zero unless b is infinity/NaN
.L_fmul_7:
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x9
	ands ip, ip, #0xff
	beq .L_fmul_17
	cmp ip, #0xff
	blt .L_fmul_17
	cmp r1, #0x0
	beq .L_fmul_13
	b .L_fmul_12
	; b is zero or denormal
.L_fmul_8:
	movs r1, r1, lsl #0x1
	beq .L_fmul_17
	mov r1, r1, lsr #0x1
	clz ip, r1
	movs r1, r1, lsl ip
	rsb ip, ip, #0x1
	b .L_fmul_1
	; exponent underflow: denormal / zero result
.L_fmul_9:
	cmn ip, #0x18
	beq .L_fmul_10
	bmi .L_fmul_16
	cmp r1, #0x0
	orrne r0, r0, #0x1
	mov r3, r0
	mov r0, r0, lsr #0x8
	rsb ip, ip, #0x0
	orr r0, r2, r0, lsr ip
	rsb ip, ip, #0x18
	movs r1, r3, lsl ip
	bxeq lr
	tst r1, #0x80000000
	bxeq lr
	movs r1, r1, lsl #0x1
	andeqs r1, r0, #0x1
	addne r0, r0, #0x1
	bx lr
	; exponent -24: the top bit is the round bit; rounds up to the smallest denormal if any
	; other bit of the high product word is set (the low word is not consulted)
.L_fmul_10:
	mov r0, r0, lsl #0x1
	b .L_fmul_15
	; return signed infinity
.L_fmul_11:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #0x1
	bx lr
	; return quiet NaN
.L_fmul_12:
	mvn r0, #0x80000000
	bx lr
.L_fmul_13:
	mvn r0, #0x80000000
	bx lr
	; overflow: signed infinity
.L_fmul_14:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #0x1
	bx lr
.L_fmul_15:
	movs r1, r0
	mov r0, r2
	addne r0, r0, #0x1
	bx lr
.L_fmul_16:
	mov r0, r2
	bx lr
	; return signed zero
.L_fmul_17:
	mov r0, r2
	bx lr
	.size _fmul, .-_fmul

; _frsb: reverse-operand entry for _fsub: swaps r0 and r1 with three exclusive-ors
; and falls through into _fsub, which follows directly.
; Not compiled: has no return; execution runs on into _fsub, the next routine. Each compiled function
; is a section of its own, so none can rely on the code placed after it.
	.global _frsb
	.type _frsb, @function
_frsb:
	eor r0, r0, r1
	eor r1, r0, r1
	eor r0, r0, r1
	.size _frsb, .-_frsb

; _fsub: single-precision subtract, r0 = a - b.
; If the signs differ the operation is really an addition of magnitudes: b's sign is flipped
; and control goes to the add routine (the instruction at _fadd+0xc). With equal signs the
; operands are ordered so |a| >= |b| (swapping and flipping the result sign if needed), b's
; mantissa is shifted right by the exponent difference keeping a sticky bit, the mantissas are
; subtracted, and the result is renormalised with clz (exact cancellation returns +0) and
; rounded to nearest even. Denormals, infinities and NaNs are handled in the tail blocks:
; inf - inf and NaN operands give 0x7fffffff, otherwise the infinity or the other operand is
; returned.
; Not compiled: branches into the middle of _fadd (.L_0204cb30), and _fadd branches back into this body
; (.L_0204d64c); a compiled function is entered only at its start. Also uses clz.
	.global _fsub
	.type _fsub, @function
_fsub:
	eors r2, r0, r1
	eormi r1, r1, #0x80000000
	bmi .L_0204cb30
.L_0204d64c:
	subs ip, r0, r1
	eorlo ip, ip, #0x80000000
	sublo r0, r0, ip
	addlo r1, r1, ip
	mov r2, #0x80000000
	mov r3, r0, lsr #0x17
	orr r0, r2, r0, lsl #0x8
	ands ip, r3, #0xff
	cmpne ip, #0xff
	beq .L_fsub_6
	mov ip, r1, lsr #0x17
	orr r1, r2, r1, lsl #0x8
	ands r2, ip, #0xff
	beq .L_fsub_7
	; exponents in r3 (a) / ip (b), mantissas left-aligned in r0 / r1
.L_fsub_1:
	subs ip, r3, ip
	beq .L_fsub_2
	rsb r2, ip, #0x20
	movs r2, r1, lsl r2
	mov r1, r1, lsr ip
	orrne r1, r1, #0x1
	subs r0, r0, r1
	bpl .L_fsub_4
	ands r1, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #0x9
	orr r0, r0, r3, lsl #0x17
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #0x1
	addne r0, r0, #0x1
	bx lr
	; equal exponents: subtract directly
.L_fsub_2:
	subs r0, r0, r1
	beq .L_fsub_15
	mov r2, r3, lsl #0x17
	and r2, r2, #0x80000000
	bic r3, r3, #0x100
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	cmp r3, #0x0
	bgt .L_fsub_3
	rsb r3, r3, #0x9
	orr r0, r2, r0, lsr r3
	bx lr
.L_fsub_3:
	add r0, r0, r0
	orr r0, r2, r0, lsr #0x9
	orr r0, r0, r3, lsl #0x17
	bx lr
	; different exponents, top bit cleared by the subtraction: renormalise with clz; an
	; exponent <= 0 gives an unrounded denormal, otherwise pack and round to nearest even
.L_fsub_4:
	mov r2, r3, lsl #0x17
	and r2, r2, #0x80000000
	bic r3, r3, #0x100
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	cmp r3, #0x0
	bgt .L_fsub_5
	rsb r3, r3, #0x9
	orr r0, r2, r0, lsr r3
	bx lr
.L_fsub_5:
	ands r1, r0, #0xff
	add r0, r0, r0
	orr r0, r2, r0, lsr #0x9
	orr r0, r0, r3, lsl #0x17
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #0x1
	addne r0, r0, #0x1
	bx lr
	; a has exponent 0 or 255
.L_fsub_6:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0x0
	ands r3, r3, #0xff
	beq .L_fsub_9
	movs r0, r0, lsl #0x1
	bne .L_fsub_18
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x9
	ands ip, ip, #0xff
	beq .L_fsub_17
	cmp ip, #0xff
	blt .L_fsub_17
	cmp r1, #0x0
	beq .L_fsub_19
	b .L_fsub_18
	; b has exponent 0 or 255
.L_fsub_7:
	cmp ip, #0x100
	movge r2, #0x80000000
	movlt r2, #0x0
	and r3, r3, #0xff
	ands ip, ip, #0xff
	beq .L_fsub_12
.L_fsub_8:
	eor r2, r2, #0x80000000
	movs r1, r1, lsl #0x1
	bne .L_fsub_18
	b .L_fsub_17
	; a is zero or denormal
.L_fsub_9:
	movs r0, r0, lsl #0x1
	beq .L_fsub_10
	mov r0, r0, lsr #0x1
	mov r3, #0x1
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x8
	ands ip, ip, #0xff
	beq .L_fsub_12
	cmp ip, #0xff
	beq .L_fsub_8
	orr r1, r1, #0x80000000
	orr r3, r3, r2, lsr #0x17
	orr ip, ip, r2, lsr #0x17
	b .L_fsub_1
	; a is zero: a zero b gives +0, a finite nonzero b is returned
.L_fsub_10:
	mov r3, r1, lsr #0x17
	mov r0, r1, lsl #0x9
	ands r2, r3, #0xff
	beq .L_fsub_11
	cmp r2, #0xff
	blt .L_fsub_13
	cmp r0, #0x0
	bne .L_fsub_16
	b .L_fsub_17
.L_fsub_11:
	cmp r0, #0x0
	beq .L_fsub_15
	b .L_fsub_13
	; b is zero or denormal
.L_fsub_12:
	movs r1, r1, lsl #0x1
	beq .L_fsub_14
	mov r1, r1, lsr #0x1
	mov ip, #0x1
	orr ip, ip, r2, lsr #0x17
	orr r3, r3, r2, lsr #0x17
	b .L_fsub_1
.L_fsub_13:
	mov r0, r1
	bx lr
	; b is zero: result is a, repacked (exponent 0 if a was denormal)
.L_fsub_14:
	cmp r0, #0x0
	subges r3, r3, #0x1
	add r0, r0, r0
	orr r0, r2, r0, lsr #0x9
	orr r0, r0, r3, lsl #0x17
	bx lr
	; return +0
.L_fsub_15:
	mov r0, #0x0
	bx lr
	; unreachable repack sequence (no label leads here)
	cmp r0, #0x0
	subges r3, r3, #0x1
	add r0, r0, r0
	mov r0, r0, lsr #0x9
	orr r0, r0, r3, lsl #0x17
	bx lr
	; return quiet NaN
.L_fsub_16:
	mvn r0, #0x80000000
	bx lr
	; return signed infinity
.L_fsub_17:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #0x1
	bx lr
	; return quiet NaN
.L_fsub_18:
	mvn r0, #0x80000000
	bx lr
	; return quiet NaN
.L_fsub_19:
	mvn r0, #0x80000000
	bx lr
	.size _fsub, .-_fsub

; _ll_mod: signed 64-bit remainder, a % b (a in r1:r0, b in r3:r2); the remainder takes
; the sign of a. It sets r4 = (high word of a) | 1 -- bit 0 selects "return remainder" -- and
; branches into the shared code in _ll_sdiv (_ll_sdiv+0x10). _ll_sdiv, directly after it, is the
; entry for the signed 64-bit quotient: it sets r4 = (ahi ^ bhi) with bit 0 cleared (sign =
; quotient sign) and runs the shared code.
; Division by zero returns immediately with the registers unchanged. If both operands fit in 32
; bits (high words equal to the sign of the low word) the 32-bit divider _s32_div_f does the work and the
; result is sign-extended. Otherwise: both operands are made positive, the divisor is shifted
; left until its top bit is set (r5 counts the shifts), the dividend is shifted similarly to
; set the loop count (r6), and a bit-at-a-time non-restoring long division runs with the
; quotient in r11:ip (high:low) and the partial remainder in r7:r1:r0. The remainder case shifts
; the partial remainder back right by r5; finally the result is negated if bit 31 of r4 is set.
; _ll_sdiv+0x74 is also the target of a branch in _ull_mod (code shared with _ll_udiv).
; Not compiled: ends by branching into the middle of _ll_sdiv (.L_ll_sdiv_common) with its registers
; still saved; a compiled function is entered only at its start.
	.global _ll_mod
	.type _ll_mod, @function
_ll_mod:
	stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
	mov r4, r1
	orr r4, r4, #0x1
	b .L_ll_sdiv_common
	.size _ll_mod, .-_ll_mod

; _ll_sdiv: signed 64-bit quotient, a / b; also the shared body of _ll_mod (see above).
; Not compiled: _ll_mod branches into its body (.L_ll_sdiv_common) and _ull_mod into .L_ll_sdiv_unsigned;
; a compiled function is entered only at its start. Also (save rule) saves ip, yet writes r0-r3 unsaved.
	.global _ll_sdiv
	.type _ll_sdiv, @function
_ll_sdiv:
	stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
	eor r4, r1, r3
	mov r4, r4, asr #0x1
	mov r4, r4, lsl #0x1
	; common code: zero divisor returns
.L_ll_sdiv_common:
	orrs r5, r3, r2
	bne .L_ll_sdiv_2
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; both operands fit in 32 bits? use the 32-bit divider
.L_ll_sdiv_2:
	mov r5, r0, lsr #0x1f
	add r5, r5, r1
	mov r6, r2, lsr #0x1f
	add r6, r6, r3
	orrs r6, r5, r6
	bne .L_ll_sdiv_3
	mov r1, r2
	bl _s32_div_f
	ands r4, r4, #0x1
	movne r0, r1
	mov r1, r0, asr #0x1f
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; take absolute values of a and b
.L_ll_sdiv_3:
	cmp r1, #0x0
	bge .L_ll_sdiv_4
	rsbs r0, r0, #0x0
	rsc r1, r1, #0x0
.L_ll_sdiv_4:
	cmp r3, #0x0
	bge .L_ll_sdiv_unsigned
	rsbs r2, r2, #0x0
	rsc r3, r3, #0x0
	; dividend zero -> result zero; else align divisor and count steps
.L_ll_sdiv_unsigned:
	orrs r5, r1, r0
	beq .L_ll_sdiv_17
	mov r5, #0x0
	mov r6, #0x1
	cmp r3, #0x0
	bmi .L_ll_sdiv_7
.L_ll_sdiv_6:
	add r5, r5, #0x1
	adds r2, r2, r2
	adcs r3, r3, r3
	bpl .L_ll_sdiv_6
	add r6, r6, r5
.L_ll_sdiv_7:
	cmp r1, #0x0
	blt .L_ll_sdiv_9
.L_ll_sdiv_8:
	cmp r6, #0x1
	beq .L_ll_sdiv_9
	sub r6, r6, #0x1
	adds r0, r0, r0
	adcs r1, r1, r1
	bpl .L_ll_sdiv_8
	; long division loop (subtract / add back, one quotient bit per pass)
.L_ll_sdiv_9:
	mov r7, #0x0
	mov ip, #0x0
	mov r11, #0x0
	b .L_ll_sdiv_11
.L_ll_sdiv_10:
	orr ip, ip, #0x1
	subs r6, r6, #0x1
	beq .L_ll_sdiv_14
	adds r0, r0, r0
	adcs r1, r1, r1
	adcs r7, r7, r7
.L_ll_sdiv_11:
	subs r0, r0, r2
	sbcs r1, r1, r3
	sbcs r7, r7, #0x0
	adds ip, ip, ip
	adc r11, r11, r11
	cmp r7, #0x0
	bge .L_ll_sdiv_10
.L_ll_sdiv_12:
	subs r6, r6, #0x1
	beq .L_ll_sdiv_13
	adds r0, r0, r0
	adcs r1, r1, r1
	adc r7, r7, r7
	adds r0, r0, r2
	adcs r1, r1, r3
	adc r7, r7, #0x0
	adds ip, ip, ip
	adc r11, r11, r11
	cmp r7, #0x0
	bge .L_ll_sdiv_10
	b .L_ll_sdiv_12
.L_ll_sdiv_13:
	adds r0, r0, r2
	adc r1, r1, r3
	; select quotient or remainder and undo the alignment shift
.L_ll_sdiv_14:
	ands r7, r4, #0x1
	moveq r0, ip
	moveq r1, r11
	beq .L_ll_sdiv_15
	subs r7, r5, #0x20
	movge r0, r1, lsr r7
	bge .L_ll_sdiv_18
	rsb r7, r5, #0x20
	mov r0, r0, lsr r5
	orr r0, r0, r1, lsl r7
	mov r1, r1, lsr r5
	b .L_ll_sdiv_15
	; unreachable (no label leads here)
	mov r0, r1, lsr r7
	mov r1, #0x0
	; apply the result sign
.L_ll_sdiv_15:
	cmp r4, #0x0
	blt .L_ll_sdiv_16
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
.L_ll_sdiv_16:
	rsbs r0, r0, #0x0
	rsc r1, r1, #0x0
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; zero dividend: result 0
.L_ll_sdiv_17:
	mov r0, #0x0
.L_ll_sdiv_18:
	mov r1, #0x0
	cmp r4, #0x0
	blt .L_ll_sdiv_16
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	.size _ll_sdiv, .-_ll_sdiv

; _ll_udiv: unsigned 64-bit divide, quotient of a / b (a in r1:r0, b in r3:r2). It sets r4 = 0
; and branches into the shared code in _ull_mod, which follows; _ull_mod's own entry sets r4 = 1
; to return the remainder instead. Zero divisor returns a unchanged. If both high words are zero
; it is a 32-bit divide: the compare/divide part of _u32_div_f (_u32_div_f+0x8) is called, r0
; takes the quotient (or the remainder) and r1 is cleared. Otherwise it jumps into the 64-bit
; divide in _ll_sdiv at _ll_sdiv+0x74, past the absolute-value step.
; Not compiled: ends by branching into the middle of _ull_mod (.L_ull_mod_common) with its registers
; still saved; a compiled function is entered only at its start.
	.global _ll_udiv
	.type _ll_udiv, @function
_ll_udiv:
	stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
	mov r4, #0x0
	b .L_ull_mod_common
	.size _ll_udiv, .-_ll_udiv

; _ull_mod: unsigned 64-bit remainder, a % b; also the shared body of _ll_udiv (see above).
; Not compiled: calls .L_0204dcf4 inside _u32_div_f and branches into the middle of _ll_sdiv
; (.L_ll_sdiv_unsigned); a compiled function is entered only at its start.
	.global _ull_mod
	.type _ull_mod, @function
_ull_mod:
	stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
	mov r4, #0x1
	; common code
.L_ull_mod_common:
	orrs r5, r3, r2
	bne .L_ull_mod_2
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
.L_ull_mod_2:
	orrs r5, r1, r3
	bne .L_ll_sdiv_unsigned
	mov r1, r2
	bl .L_0204dcf4
	cmp r4, #0x0
	movne r0, r1
	mov r1, #0x0
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	.size _ull_mod, .-_ull_mod

; _ll_mul: 64-bit multiply, a * b (a in r1:r0, b in r3:r2), low 64 bits of the product in r1:r0:
; the full 64-bit product of the low words plus both cross products added into the high word.
; Not compiled (save rule): saves three words {r4, r5, lr} with no frame and no r3, then pops lr and
; returns with bx lr.
	.global _ll_mul
	.type _ll_mul, @function
_ll_mul:
	stmfd sp!, {r4, r5, lr}
	umull r5, r4, r0, r2
	mla r4, r0, r3, r4
	mla r4, r2, r1, r4
	mov r1, r4
	mov r0, r5
	ldmfd sp!, {r4, r5, lr}
	bx lr
	.size _ll_mul, .-_ll_mul
