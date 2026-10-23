; Soft-float runtime, second part: double and float comparisons, single-precision divide, multiply and
; subtract, float conversions, and the 64-bit integer divide, remainder and multiply. Register use as in
; rt_soft_float_a.s: a float or int in r0 (second operand r1), a double or 64-bit integer in r1:r0 (high:low),
; the second operand in r3:r2.
; The comparison routines return their result both in r0 and in the condition flags.
; _fsub branches into _fadd (.L_fadd_add_magnitudes, rt_soft_float_a.s) and _fadd into _fsub
; (.L_fsub_sub_magnitudes, global here); _ll_sdiv calls _s32_div_f and _ull_mod calls
; .L_u32_div_f_nonzero_divisor inside _u32_div_f (both in rt_div32.s).
; "Not compiled" lines say why the compiler cannot have produced a routine. Save rule: the stmfd list of a
; compiled ARM function holds every register among r4-r7 it writes and, unless it is an interrupt function, also
; every one among r8-r11; lr if it calls or writes lr; and r3 when the saved registers and its stack frame would
; otherwise come to an odd number of words, or in place of a one-word frame. r0-r2 and r12 appear only in an
; interrupt function, which saves every register among r0-r7 it writes. A compiled function that saved lr
; returns by loading it into pc (a variadic one, which starts with stmfd {r0-r3}, excepted).

	.text
	.global .L_fsub_sub_magnitudes
	.extern .L_fadd_add_magnitudes
	.extern _s32_div_f
	.extern .L_u32_div_f_nonzero_divisor

; _dls: returns 1 if a < b (doubles a in r1:r0, b in r3:r2), else 0. A NaN in either operand
; gives 0 (unordered); +0 and -0 compare equal. Unordered and the +0/-0 case give 0 with C set.
; Non-negative operands compare as unsigned 64-bit values; if either is negative the raw bits
; are compared in reverse order, which is correct for two negatives and for mixed signs.
; Not compiled: sets the carry flag of its result with mrs/msr, which C has no expression for.
	.global _dls
	.type _dls, @function
_dls:
	; exponent 0x7ff in either operand: check for NaN
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs .L_dls_first_exp_max
	cmn ip, r3, lsl #1
	bhs .L_dls_second_exp_max
	; ordered: compare
.L_dls_compare:
	orrs ip, r3, r1
	bmi .L_dls_negative
	cmp r1, r3
	cmpeq r0, r2
	movlo r0, #1
	movhs r0, #0
	bx lr
	; unordered (NaN): 0 with C set
.L_dls_unordered:
	mov r0, #0
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	; at least one negative: equal zeros check (0 with C set), then reversed compare
.L_dls_negative:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #0
	bne .L_dls_compare_reversed
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bxeq lr
	; compare reversed (negative operands)
.L_dls_compare_reversed:
	cmp r3, r1
	cmpeq r2, r0
	movlo r0, #1
	movhs r0, #0
	bx lr
	; first operand has exponent 0x7ff: NaN if mantissa nonzero
.L_dls_first_exp_max:
	bne .L_dls_unordered
	cmp r0, #0
	bhi .L_dls_unordered
	cmn ip, r3, lsl #1
	blo .L_dls_compare
	; second operand has exponent 0x7ff: NaN if mantissa nonzero
.L_dls_second_exp_max:
	bne .L_dls_unordered
	cmp r2, #0
	bhi .L_dls_unordered
	b .L_dls_compare
	.size _dls, .-_dls

; _deq: returns 1 if a == b (doubles), else 0. NaN gives 0 with Z cleared; +0 equals -0;
; otherwise both words must be equal.
; Not compiled: sets the Z flag of its result with mrs/msr, which C has no expression for.
	.global _deq
	.type _deq, @function
_deq:
	; exponent 0x7ff in either operand: check for NaN
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs .L_deq_first_exp_max
	cmn ip, r3, lsl #1
	bhs .L_deq_second_exp_max
	; ordered: compare
.L_deq_compare:
	orrs ip, r3, r1
	bmi .L_deq_negative
	cmp r1, r3
	cmpeq r0, r2
	moveq r0, #1
	movne r0, #0
	bx lr
	; unordered (NaN): 0 with Z cleared
.L_deq_unordered:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
	; at least one negative: equal zeros check, then reversed compare
.L_deq_negative:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #1
	bxeq lr
	cmp r3, r1
	cmpeq r2, r0
	moveq r0, #1
	movne r0, #0
	bx lr
	; first operand has exponent 0x7ff: NaN if mantissa nonzero
.L_deq_first_exp_max:
	bne .L_deq_unordered
	cmp r0, #0
	bhi .L_deq_unordered
	cmn ip, r3, lsl #1
	blo .L_deq_compare
	; second operand has exponent 0x7ff: NaN if mantissa nonzero
.L_deq_second_exp_max:
	bne .L_deq_unordered
	cmp r2, #0
	bhi .L_deq_unordered
	b .L_deq_compare
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
	; NaN in either operand: unordered
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	cmphs r3, r1, lsl #1
	blo .L_fgeq_unordered
	; sign-magnitude to two's complement for each operand
	cmp r0, #0
	bicmi r0, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r1, #0
	bicmi r1, r1, #0x80000000
	rsbmi r1, r1, #0
	; signed compare: r0 = 1 and C set if a >= b, else r0 = 0 and C clear
	cmp r0, r1
	movge r0, #1
	movlt r0, #0
	mrs ip, cpsr
	biclt ip, ip, #0x20000000
	orrge ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	; unordered (NaN) result: 0 with C clear
.L_fgeq_unordered:
	mov r0, #0
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
	; NaN in either operand: unordered
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	cmphs r3, r1, lsl #1
	blo .L_fls_unordered
	; sign-magnitude to two's complement for each operand
	cmp r0, #0
	bicmi r0, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r1, #0
	bicmi r1, r1, #0x80000000
	rsbmi r1, r1, #0
	; signed compare: r0 = 1 and C clear if a < b, else r0 = 0 and C set
	cmp r0, r1
	movlt r0, #1
	movge r0, #0
	mrs ip, cpsr
	orrge ip, ip, #0x20000000
	biclt ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
	; unordered (NaN) result: 0 with C set
.L_fls_unordered:
	mov r0, #0
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
	; NaN in either operand: unordered
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	blo .L_fneq_unordered
	cmp r3, r1, lsl #1
	blo .L_fneq_unordered
	; both zero (ignoring sign)? return 0 with Z set
	orr r3, r0, r1
	movs r3, r3, lsl #1
	moveq r0, #0
	bne .L_fneq_compare_bits
	mrs ip, cpsr
	orr ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
	; not both zero: compare raw bits; Z set to match
.L_fneq_compare_bits:
	cmp r0, r1
	movne r0, #1
	moveq r0, #0
	mrs ip, cpsr
	bicne ip, ip, #0x40000000
	orreq ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
	; unordered (NaN): not equal, Z clear
.L_fneq_unordered:
	mov r0, #1
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
	; swap r0 and r1
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
; Not compiled: C has no operator or intrinsic for `clz` in this compiler; an asm statement would still be
; assembly. Also (save rule) it saves lr alone with no frame and no r3, then pops it and returns with bx lr.
	.global _fdiv
	.type _fdiv, @function
_fdiv:
	stmdb sp!, {lr}
	; r3 = exponent of a, ip = exponent of b; 0 or 255 is special
	mov ip, #0xff
	ands r3, ip, r0, lsr #23
	cmpne r3, #0xff
	beq .L_fdiv_first_special
	ands ip, ip, r1, lsr #23
	cmpne ip, #0xff
	beq .L_fdiv_second_special
	; set the implicit bits: r2 = mantissa of a, lr = mantissa of b
	orr r1, r1, #0x800000
	orr r0, r0, #0x800000
	bic r2, r0, #0xff000000
	bic lr, r1, #0xff000000
	; both operands are now normalised mantissas (a in r2, b in lr) with exponents in r3 / ip
.L_fdiv_divide:
	; a below b: double a, exponent - 1
	cmp r2, lr
	movlo r2, r2, lsl #1
	sublo r3, r3, #1
	; N = sign of the quotient, kept through the steps below that leave the flags alone
	teq r0, r1
	; r1 = 256 * x0, the table's estimate of x0 = 1 / b (b = its mantissa, in [1, 2)), by b's top 8 fraction bits
	adr r0, .L_fdiv_table - 256	; the index lr >> 15 runs from 256 to 511
	ldrb r1, [r0, lr, lsr #15]
	; first refinement, lr = -(b's mantissa): x1 = x0 * (2 - b * x0), in r0 as x1 * 2^18
	rsb lr, lr, #0
	mov r0, lr, asr #1
	mul r0, r1, r0
	add r0, r0, #0x80000000
	mov r0, r0, lsr #6
	mul r0, r1, r0
	mov r0, r0, lsr #14
	; second refinement: x2 = x1 + x1 * (1 - b * x1), in r0 as x2 * 2^32; ip = ea - eb
	mul r1, lr, r0
	sub ip, r3, ip
	mov r1, r1, lsr #12
	mul r1, r0, r1
	mov r0, r0, lsl #14
	add r0, r0, r1, lsr #15
	; quotient q = a * x2: r0 = high word (implicit bit in bit 23), r1 = the fraction bits below it
	umull r1, r0, r2, r0
	mov r3, r0
	orrmi r0, r0, #0x80000000
	; biased exponent: below range goes to the denormal path, 0xfe and up overflows
	adds ip, ip, #0x7e
	bmi .L_fdiv_underflow
	cmp ip, #0xfe
	bge .L_fdiv_overflow
	; insert the exponent (the implicit bit adds one); round up on the top fraction bit, unless the top
	; four fraction bits are 0111, too close to half: then check the exact remainder
	add r0, r0, ip, lsl #23
	mov ip, r1, lsr #28
	cmp ip, #7
	beq .L_fdiv_check_remainder
	add r0, r0, r1, lsr #31
	ldmia sp!, {lr}
	bx lr
	; reciprocal-estimate table (256 bytes), addressed pc-relative above
	; entry i = min(255, round(65536 / (i + 256.5))), about 256 / b for b's top 8 fraction bits i
.L_fdiv_table:
	.byte 0xff, 0xff, 0xfe, 0xfd, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf4, 0xf3, 0xf2, 0xf1
	.byte 0xf0, 0xf0, 0xef, 0xee, 0xed, 0xec, 0xeb, 0xea, 0xea, 0xe9, 0xe8, 0xe7, 0xe6, 0xe6, 0xe5, 0xe4
	.byte 0xe3, 0xe2, 0xe2, 0xe1, 0xe0, 0xdf, 0xdf, 0xde, 0xdd, 0xdc, 0xdc, 0xdb, 0xda, 0xd9, 0xd9, 0xd8
	.byte 0xd7, 0xd7, 0xd6, 0xd5, 0xd4, 0xd4, 0xd3, 0xd2, 0xd2, 0xd1, 0xd0, 0xd0, 0xcf, 0xce, 0xce, 0xcd
	.byte 0xcc, 0xcc, 0xcb, 0xcb, 0xca, 0xc9, 0xc9, 0xc8, 0xc8, 0xc7, 0xc6, 0xc6, 0xc5, 0xc5, 0xc4, 0xc3
	.byte 0xc3, 0xc2, 0xc2, 0xc1, 0xc0, 0xc0, 0xbf, 0xbf, 0xbe, 0xbe, 0xbd, 0xbd, 0xbc, 0xbc, 0xbb, 0xba
	.byte 0xba, 0xb9, 0xb9, 0xb8, 0xb8, 0xb7, 0xb7, 0xb6, 0xb6, 0xb5, 0xb5, 0xb4, 0xb4, 0xb3, 0xb3, 0xb2
	.byte 0xb2, 0xb1, 0xb1, 0xb0, 0xb0, 0xaf, 0xaf, 0xaf, 0xae, 0xae, 0xad, 0xad, 0xac, 0xac, 0xab, 0xab
	.byte 0xaa, 0xaa, 0xaa, 0xa9, 0xa9, 0xa8, 0xa8, 0xa7, 0xa7, 0xa7, 0xa6, 0xa6, 0xa5, 0xa5, 0xa4, 0xa4
	.byte 0xa4, 0xa3, 0xa3, 0xa2, 0xa2, 0xa2, 0xa1, 0xa1, 0xa0, 0xa0, 0xa0, 0x9f, 0x9f, 0x9e, 0x9e, 0x9e
	.byte 0x9d, 0x9d, 0x9d, 0x9c, 0x9c, 0x9b, 0x9b, 0x9b, 0x9a, 0x9a, 0x9a, 0x99, 0x99, 0x99, 0x98, 0x98
	.byte 0x98, 0x97, 0x97, 0x96, 0x96, 0x96, 0x95, 0x95, 0x95, 0x94, 0x94, 0x94, 0x93, 0x93, 0x93, 0x92
	.byte 0x92, 0x92, 0x91, 0x91, 0x91, 0x91, 0x90, 0x90, 0x90, 0x8f, 0x8f, 0x8f, 0x8e, 0x8e, 0x8e, 0x8d
	.byte 0x8d, 0x8d, 0x8c, 0x8c, 0x8c, 0x8c, 0x8b, 0x8b, 0x8b, 0x8a, 0x8a, 0x8a, 0x8a, 0x89, 0x89, 0x89
	.byte 0x88, 0x88, 0x88, 0x88, 0x87, 0x87, 0x87, 0x86, 0x86, 0x86, 0x86, 0x85, 0x85, 0x85, 0x85, 0x84
	.byte 0x84, 0x84, 0x83, 0x83, 0x83, 0x83, 0x82, 0x82, 0x82, 0x82, 0x81, 0x81, 0x81, 0x81, 0x80, 0x80
	; near-tie: round up when b * (2q + 1) < a * 2^24 (low 32 bits), i.e. the quotient is above q + 1/2
.L_fdiv_check_remainder:
	mov r1, r3, lsl #1
	add r1, r1, #1
	rsb lr, lr, #0
	mul r1, lr, r1
	cmp r1, r2, lsl #24
	addmi r0, r0, #1
	ldmia sp!, {lr}
	bx lr
	; a has exponent 0 or 255; lr = sign of the result
.L_fdiv_first_special:
	eor lr, r0, r1
	and lr, lr, #0x80000000
	cmp r3, #0
	beq .L_fdiv_first_exp0
	; a is NaN, or Inf: Inf / Inf is NaN, Inf / NaN is NaN, otherwise Inf
	movs r0, r0, lsl #9
	bne .L_fdiv_return_nan
	mov ip, r1, lsr #23
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq .L_fdiv_return_inf
	cmp ip, #0xff
	blt .L_fdiv_return_inf
	cmp r1, #0
	beq .L_fdiv_invalid
	b .L_fdiv_second_nan
	; a is normal, b has exponent 0 or 255; lr = sign of the result
.L_fdiv_second_special:
	eor lr, r0, r1
	and lr, lr, #0x80000000
	cmp ip, #0
	beq .L_fdiv_second_exp0
	; b is NaN, or Inf (finite / Inf is zero)
.L_fdiv_second_inf_nan:
	movs r1, r1, lsl #9
	bne .L_fdiv_second_nan
	b .L_fdiv_return_zero
	; a has exponent 0: zero or denormal
.L_fdiv_first_exp0:
	movs r2, r0, lsl #9
	beq .L_fdiv_first_zero
	; denormal: normalise with clz, implicit bit in bit 23 of r2, exponent r3 = -count
	clz r3, r2
	movs r2, r2, lsl r3
	rsb r3, r3, #0
	mov r2, r2, lsr #8
	; b: exponent 0 or 255 goes to its cases, otherwise set its implicit bit and divide
	ands ip, ip, r1, lsr #23
	beq .L_fdiv_denormal_by_exp0
	cmp ip, #0xff
	beq .L_fdiv_second_inf_nan
	orr r1, r1, #0x800000
	bic lr, r1, #0xff000000
	b .L_fdiv_divide
	; a is zero: result is zero unless b is zero or NaN
.L_fdiv_first_zero:
	mov ip, r1, lsr #23
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq .L_fdiv_zero_by_exp0
	cmp ip, #0xff
	blt .L_fdiv_return_zero
	cmp r1, #0
	beq .L_fdiv_return_zero
	b .L_fdiv_second_nan
	; zero / zero is NaN, zero / denormal is zero
.L_fdiv_zero_by_exp0:
	cmp r1, #0
	beq .L_fdiv_invalid
	b .L_fdiv_return_zero
	; b is zero or denormal: zero gives infinity, else normalise b
.L_fdiv_second_exp0:
	movs ip, r1, lsl #9
	beq .L_fdiv_return_inf
	mov lr, ip
	clz ip, lr
	movs lr, lr, lsl ip
	rsb ip, ip, #0
	mov lr, lr, lsr #8
	; set a's implicit bit and divide
	orr r0, r0, #0x800000
	bic r2, r0, #0xff000000
	b .L_fdiv_divide
	; a already normalised, b is zero or denormal
.L_fdiv_denormal_by_exp0:
	movs ip, r1, lsl #9
	beq .L_fdiv_return_inf
	mov lr, ip
	clz ip, lr
	movs lr, lr, lsl ip
	rsb ip, ip, #0
	mov lr, lr, lsr #8
	b .L_fdiv_divide
	; exponent below range: build a denormal (or zero) result; the shifted quotient is
	; corrected and rounded (nearest, ties to even) with exact remainder checks
.L_fdiv_underflow:
	and r0, r0, #0x80000000
	cmn ip, #24
	beq .L_fdiv_half_denormal
	bmi .L_fdiv_underflow_zero
	; r3 = quotient shifted right by -exponent into the denormal field; r2 = a scaled to match
	add r1, ip, #23
	mov r2, r2, lsl r1
	rsb ip, ip, #0
	mov r3, r3, lsr ip
	orr r0, r0, r3
	; lr = b's mantissa; b * q equal to a: exact, return
	rsb lr, lr, #0
	mul r1, lr, r3
	cmp r1, r2
	ldmeqia sp!, {lr}
	bxeq lr
	; b * (q + 1): equal to a returns q + 1; below a, q was one too low
	add r1, r1, lr
	cmp r1, r2
	beq .L_fdiv_round_up_exact
	addmi r0, r0, #1
	subpl r1, r1, lr
	; round to nearest even: compare b * (2q + 1) with 2a
	add r1, lr, r1, lsl #1
	cmp r1, r2, lsl #1
	and r3, r0, #1
	addmi r0, r0, #1
	addeq r0, r0, r3
	ldmia sp!, {lr}
	bx lr
	; quotient was one too low and is exact after the increment
.L_fdiv_round_up_exact:
	add r0, r0, #1
	ldmia sp!, {lr}
	bx lr
	; exponent -24: rounds up to the smallest denormal unless the mantissas are equal
	; (an exact half, which rounds to the even zero)
.L_fdiv_half_denormal:
	cmn r2, lr
	addne r0, r0, #1
	ldmia sp!, {lr}
	bx lr
	; NaN
.L_fdiv_second_nan:
	mov r0, r1
	b .L_fdiv_return_nan
	; return signed infinity
.L_fdiv_return_inf:
	mov r0, #0xff000000
	orr r0, lr, r0, lsr #1
	ldmia sp!, {lr}
	bx lr
	; return quiet NaN
.L_fdiv_return_nan:
	mvn r0, #0x80000000
	ldmia sp!, {lr}
	bx lr
	; return quiet NaN (0 / 0, Inf / Inf)
.L_fdiv_invalid:
	mvn r0, #0x80000000
	ldmia sp!, {lr}
	bx lr
	; exponent overflow: return signed infinity
.L_fdiv_overflow:
	tst r0, #0x80000000
	mov r0, #0xff000000
	movne r0, r0, asr #1
	moveq r0, r0, lsr #1
	ldmia sp!, {lr}
	bx lr
	; result underflows to zero keeping only the sign
.L_fdiv_underflow_zero:
	ldmia sp!, {lr}
	bx lr
	; return signed zero (sign in lr)
.L_fdiv_return_zero:
	mov r0, lr
	ldmia sp!, {lr}
	bx lr
	.size _fdiv, .-_fdiv

; _f2d: widen a float (r0) to a double (r1:r0).
; Normal numbers: rebias the exponent (+0x380) and shift the 23-bit mantissa into the 52-bit
; field. Zero maps to signed zero; a float denormal is normalised with clz so it becomes a
; normal double; infinity becomes the double infinity; any NaN becomes 0x7fffffff_ffffffff.
; Not compiled: C has no operator or intrinsic for `clz` in this compiler; an asm statement would still be
; assembly.
	.global _f2d
	.type _f2d, @function
_f2d:
	; r2 = sign, ip = exponent, r3 = mantissa left-justified; exponent 0 or 255 is special
	and r2, r0, #0x80000000
	mov ip, r0, lsr #23
	mov r3, r0, lsl #9
	ands ip, ip, #0xff
	beq .L_f2d_exp0
	cmp ip, #0xff
	beq .L_f2d_exp_max
	; assemble sign | exponent | mantissa
.L_f2d_pack:
	add ip, ip, #0x380
	mov r0, r3, lsl #20
	orr r1, r2, r3, lsr #12
	orr r1, r1, ip, lsl #20
	bx lr
	; exponent 0: zero or denormal
.L_f2d_exp0:
	cmp r3, #0
	bne .L_f2d_denormal
	mov r1, r2
	mov r0, #0
	bx lr
	; denormal: shift right one, normalise with clz (exponent 1 - count), drop the leading 1
.L_f2d_denormal:
	mov r3, r3, lsr #1
	clz ip, r3
	movs r3, r3, lsl ip
	rsb ip, ip, #1
	add r3, r3, r3
	b .L_f2d_pack
	; exponent 255: infinity or NaN
.L_f2d_exp_max:
	cmp r3, #0
	bhi .L_f2d_nan
	ldr r1, .L_f2d_inf_high
	orr r1, r1, r2
	mov r0, #0
	bx lr
	; NaN: 0x7fffffff_ffffffff
.L_f2d_nan:
	mvn r0, #0
	bic r1, r0, #0x80000000
	bx lr
	; high word of +Inf
.L_f2d_inf_high:
	.word 0x7ff00000
	.size _f2d, .-_f2d

; _ffix: float (r0) to signed int, truncating toward zero.
; The shift is 0x9e minus the biased exponent; if it is not positive (|a| >= 2^31, infinity,
; NaN) the result saturates to 0x7fffffff for a non-negative input or 0x80000000 for a negative
; one. Otherwise the mantissa with its implicit bit is shifted right and negated if a < 0.
; Not compiled: `subs r2, r2, r1, lsr #23` is followed by `ble`.
; The compiler makes a flag-setting subtract only by folding a following `cmp rd, #0` into it, which leaves
; the branch testing eq, ne, mi or pl (ge and lt become pl and mi, hs becomes always; lo, gt and le are not
; folded), or inside 64-bit subtract, compare and shift sequences, where the next sbc, sbcs or conditional orr
; reads the flags.
; In C, `shift -= m >> 23; if (shift > 0)` compiles to `sub r2, r1, r3, lsr #23; cmp r2, #0; ble`, and the
; negate under `a < 0` comes out as `rsblt` where this routine has `rsbmi`.
	.global _ffix
	.type _ffix, @function
_ffix:
	; r2 = shift = 0x9e - exponent (sign bit cleared first)
	bic r1, r0, #0x80000000
	mov r2, #0x9e
	subs r2, r2, r1, lsr #23
	ble .L_ffix_saturate
	; mantissa with the implicit bit at bit 31, shifted right; negated when a < 0
	mov r1, r1, lsl #8
	orr r1, r1, #0x80000000
	cmp r0, #0
	mov r0, r1, lsr r2
	rsbmi r0, r0, #0
	bx lr
	; out of range: saturate
.L_ffix_saturate:
	mvn r0, r0, asr #31
	add r0, r0, #0x80000000
	bx lr
	.size _ffix, .-_ffix

; _fflt: signed int (r0) to float, round to nearest even.
; Takes the magnitude (remembering the sign), returns 0 for 0, normalises with clz, sets the
; exponent to 0x9e - clz, packs sign | exponent | top 23 mantissa bits, and uses the 8 bits
; shifted out below the mantissa to round: exact or below half truncates, above half rounds up,
; exactly half rounds to even.
; Not compiled: C has no operator or intrinsic for `clz` in this compiler; an asm statement would still be
; assembly.
	.global _fflt
	.type _fflt, @function
_fflt:
	; r2 = sign; take the magnitude; zero returns 0
	ands r2, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r0, #0
	bxeq lr
	; normalise: shift left by the leading-zero count, exponent = 0x9e - count
	clz r3, r0
	movs r0, r0, lsl r3
	rsb r3, r3, #0x9e
	; pack: r1 = the 8 bits shifted out; drop the leading 1, insert sign and exponent
	ands r1, r0, #0xff
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	; round to nearest even (nothing shifted out: return at once)
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r3, r1, #0x7f
	andeqs r3, r0, #1
	addne r0, r0, #1
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
; Not compiled: C has no operator or intrinsic for `clz` in this compiler; an asm statement would still be
; assembly.
	.global _fmul
	.type _fmul, @function
_fmul:
	; r2 = sign of the product
	eor r2, r0, r1
	and r2, r2, #0x80000000
	; unpack a: r3 = exponent, r0 = mantissa shifted left 8; exponent 0 or 255 is special
	mov ip, #0xff
	ands r3, ip, r0, lsr #23
	mov r0, r0, lsl #8
	cmpne r3, #0xff
	beq .L_fmul_first_special
	orr r0, r0, #0x80000000
	; unpack b the same way into ip and r1
	ands ip, ip, r1, lsr #23
	mov r1, r1, lsl #8
	cmpne ip, #0xff
	beq .L_fmul_second_special
	orr r1, r1, #0x80000000
	; operands normalised: a in r0, b in r1 (bit 31 set), exponents in r3 / ip
.L_fmul_multiply:
	; r3:r1 = product; top bit clear: double the high word, exponent - 1
	add ip, r3, ip
	umull r1, r3, r0, r1
	movs r0, r3
	addpl r0, r0, r0
	subpl ip, ip, #1
	; rebias (- 0x7f): negative goes to the denormal path, 0xfe and up overflows
	subs ip, ip, #0x7f
	bmi .L_fmul_underflow
	cmp ip, #0xfe
	bge .L_fmul_overflow
	; pack: r3 = the 8 bits shifted out; sign, mantissa, exponent (the implicit bit adds one)
	ands r3, r0, #0xff
	orr r0, r2, r0, lsr #8
	add r0, r0, ip, lsl #23
	; round to nearest even: a tie is broken by the bits below the round bit and the low product word
	tst r3, #0x80
	bxeq lr
	orrs r1, r1, r3, lsl #25
	andeqs r3, r0, #1
	addne r0, r0, #1
	bx lr
	; a has exponent 0 or 255
.L_fmul_first_special:
	cmp r3, #0
	beq .L_fmul_first_exp0
	; a is NaN, or Inf: Inf * 0 is NaN, Inf * NaN is NaN, otherwise Inf
	movs r0, r0, lsl #1
	bne .L_fmul_return_nan
	mov ip, r1, lsr #23
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq .L_fmul_inf_times_exp0
	cmp ip, #0xff
	blt .L_fmul_return_inf
	cmp r1, #0
	beq .L_fmul_return_inf
	b .L_fmul_return_nan
	; Inf * zero is NaN, Inf * denormal is Inf
.L_fmul_inf_times_exp0:
	cmp r1, #0
	beq .L_fmul_invalid
	b .L_fmul_return_inf
	; b has exponent 0 or 255
.L_fmul_second_special:
	cmp ip, #0
	beq .L_fmul_second_exp0
	; b is NaN or Inf
.L_fmul_second_inf_nan:
	movs r1, r1, lsl #1
	bne .L_fmul_return_nan
	b .L_fmul_return_inf
	; a is zero or denormal
.L_fmul_first_exp0:
	movs r0, r0, lsl #1
	beq .L_fmul_first_zero
	; denormal: normalise with clz, exponent 1 - count
	mov r0, r0, lsr #1
	clz r3, r0
	movs r0, r0, lsl r3
	rsb r3, r3, #1
	; unpack b; exponent 0 or 255 goes to its cases, otherwise set its implicit bit and multiply
	mov ip, r1, lsr #23
	mov r1, r1, lsl #8
	ands ip, ip, #0xff
	beq .L_fmul_second_exp0
	cmp ip, #0xff
	beq .L_fmul_second_inf_nan
	orr r1, r1, #0x80000000
	b .L_fmul_multiply
	; a is zero: result is zero unless b is infinity/NaN
.L_fmul_first_zero:
	mov ip, r1, lsr #23
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq .L_fmul_return_zero
	cmp ip, #0xff
	blt .L_fmul_return_zero
	cmp r1, #0
	beq .L_fmul_invalid
	b .L_fmul_return_nan
	; b is zero or denormal: zero gives signed zero, a denormal is normalised (exponent 1 - count)
.L_fmul_second_exp0:
	movs r1, r1, lsl #1
	beq .L_fmul_return_zero
	mov r1, r1, lsr #1
	clz ip, r1
	movs r1, r1, lsl ip
	rsb ip, ip, #1
	b .L_fmul_multiply
	; exponent underflow: denormal / zero result
.L_fmul_underflow:
	cmn ip, #24
	beq .L_fmul_half_denormal
	bmi .L_fmul_underflow_zero
	; fold the low product word into a sticky bit 0; shift right by 8 - exponent into the denormal field
	cmp r1, #0
	orrne r0, r0, #1
	mov r3, r0
	mov r0, r0, lsr #8
	rsb ip, ip, #0
	orr r0, r2, r0, lsr ip
	; r1 = the bits shifted out, left-justified; round to nearest even
	rsb ip, ip, #24
	movs r1, r3, lsl ip
	bxeq lr
	tst r1, #0x80000000
	bxeq lr
	movs r1, r1, lsl #1
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
	; exponent -24: the top bit is the round bit; rounds up to the smallest denormal if any
	; other bit of the high product word is set (the low word is not consulted)
.L_fmul_half_denormal:
	mov r0, r0, lsl #1
	b .L_fmul_round_min_denormal
	; return signed infinity
.L_fmul_return_inf:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
	; return quiet NaN
.L_fmul_return_nan:
	mvn r0, #0x80000000
	bx lr
	; return quiet NaN (Inf * 0)
.L_fmul_invalid:
	mvn r0, #0x80000000
	bx lr
	; overflow: signed infinity
.L_fmul_overflow:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
	; smallest denormal if any bit is left, else signed zero
.L_fmul_round_min_denormal:
	movs r1, r0
	mov r0, r2
	addne r0, r0, #1
	bx lr
	; underflow to signed zero
.L_fmul_underflow_zero:
	mov r0, r2
	bx lr
	; return signed zero
.L_fmul_return_zero:
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
	; swap r0 and r1
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
; Not compiled: branches into the middle of _fadd (.L_fadd_add_magnitudes), and _fadd branches back into this
; body (.L_fsub_sub_magnitudes); a compiled function is entered only at its start. Also uses `clz`: C has no
; operator or intrinsic for `clz` in this compiler; an asm statement would still be assembly.
	.global _fsub
	.type _fsub, @function
_fsub:
	; different signs? flip b and take the add path
	eors r2, r0, r1
	eormi r1, r1, #0x80000000
	bmi .L_fadd_add_magnitudes
.L_fsub_sub_magnitudes:
	; order by magnitude (swap when |a| < |b|, which also flips the result sign)
	subs ip, r0, r1
	eorlo ip, ip, #0x80000000
	sublo r0, r0, ip
	addlo r1, r1, ip
	; unpack a: r3 = sign|exponent, r0 = mantissa with implicit bit at bit 31; exponent 0 or 255 is special
	mov r2, #0x80000000
	mov r3, r0, lsr #23
	orr r0, r2, r0, lsl #8
	ands ip, r3, #0xff
	cmpne ip, #0xff
	beq .L_fsub_first_special
	; unpack b the same way: ip = sign|exponent, r1 = mantissa; exponent 0 is special
	mov ip, r1, lsr #23
	orr r1, r2, r1, lsl #8
	ands r2, ip, #0xff
	beq .L_fsub_second_special
	; exponents in r3 (a) / ip (b), mantissas left-aligned in r0 / r1
.L_fsub_align:
	subs ip, r3, ip
	beq .L_fsub_same_exponent
	; shift b's mantissa right by the difference keeping a sticky bit; subtract
	rsb r2, ip, #32
	movs r2, r1, lsl r2
	mov r1, r1, lsr ip
	orrne r1, r1, #1
	subs r0, r0, r1
	bpl .L_fsub_renormalize
	; top bit still set: pack, round to nearest even using the low 8 bits
	ands r1, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, r3, lsl #23
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
	; equal exponents: subtract directly; zero gives +0
.L_fsub_same_exponent:
	subs r0, r0, r1
	beq .L_fsub_return_zero
	; r2 = sign, r3 = exponent alone; normalise with clz, exponent - count
	mov r2, r3, lsl #23
	and r2, r2, #0x80000000
	bic r3, r3, #0x100
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	; exponent <= 0: shift right by 9 - exponent into a denormal
	cmp r3, #0
	bgt .L_fsub_same_exp_pack
	rsb r3, r3, #9
	orr r0, r2, r0, lsr r3
	bx lr
	; pack: drop the implicit bit, insert sign and exponent (no bits shifted out, no rounding)
.L_fsub_same_exp_pack:
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
	; different exponents, top bit cleared by the subtraction: renormalise with clz; an
	; exponent <= 0 gives an unrounded denormal, otherwise pack and round to nearest even
.L_fsub_renormalize:
	mov r2, r3, lsl #23
	and r2, r2, #0x80000000
	bic r3, r3, #0x100
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	cmp r3, #0
	bgt .L_fsub_renormalize_pack
	rsb r3, r3, #9
	orr r0, r2, r0, lsr r3
	bx lr
	; pack: r1 = the 8 bits shifted out; round to nearest even (nothing shifted out: return at once)
.L_fsub_renormalize_pack:
	ands r1, r0, #0xff
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
	; a has exponent 0 or 255; r2 = its sign
.L_fsub_first_special:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	ands r3, r3, #0xff
	beq .L_fsub_first_exp0
	; a is NaN, or Inf: Inf - Inf is NaN, Inf - NaN is NaN, otherwise Inf
	movs r0, r0, lsl #1
	bne .L_fsub_return_nan
	mov ip, r1, lsr #23
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq .L_fsub_return_inf
	cmp ip, #0xff
	blt .L_fsub_return_inf
	cmp r1, #0
	beq .L_fsub_invalid
	b .L_fsub_return_nan
	; b has exponent 0 or 255; r2 = its sign
.L_fsub_second_special:
	cmp ip, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	and r3, r3, #0xff
	ands ip, ip, #0xff
	beq .L_fsub_second_exp0
	; b is NaN or Inf: flip the sign; NaN if the mantissa is nonzero, else Inf
.L_fsub_second_inf_nan:
	eor r2, r2, #0x80000000
	movs r1, r1, lsl #1
	bne .L_fsub_return_nan
	b .L_fsub_return_inf
	; a is zero or denormal: a denormal uses exponent 1, no implicit bit
.L_fsub_first_exp0:
	movs r0, r0, lsl #1
	beq .L_fsub_first_zero
	mov r0, r0, lsr #1
	mov r3, #1
	; unpack b; exponent 0 or 255 goes to its cases, otherwise set the implicit bit, signs back, align
	mov ip, r1, lsr #23
	mov r1, r1, lsl #8
	ands ip, ip, #0xff
	beq .L_fsub_second_exp0
	cmp ip, #0xff
	beq .L_fsub_second_inf_nan
	orr r1, r1, #0x80000000
	orr r3, r3, r2, lsr #23
	orr ip, ip, r2, lsr #23
	b .L_fsub_align
	; a is zero: a zero b gives +0, a finite nonzero b is returned
.L_fsub_first_zero:
	mov r3, r1, lsr #23
	mov r0, r1, lsl #9
	ands r2, r3, #0xff
	beq .L_fsub_zero_minus_exp0
	cmp r2, #0xff
	blt .L_fsub_return_second
	cmp r0, #0
	bne .L_fsub_second_nan
	b .L_fsub_return_inf
	; b has exponent 0: zero gives +0, a denormal is returned
.L_fsub_zero_minus_exp0:
	cmp r0, #0
	beq .L_fsub_return_zero
	b .L_fsub_return_second
	; b is zero or denormal: a zero returns a; a denormal uses exponent 1, signs back, align
.L_fsub_second_exp0:
	movs r1, r1, lsl #1
	beq .L_fsub_return_first
	mov r1, r1, lsr #1
	mov ip, #1
	orr ip, ip, r2, lsr #23
	orr r3, r3, r2, lsr #23
	b .L_fsub_align
	; return b
.L_fsub_return_second:
	mov r0, r1
	bx lr
	; b is zero: result is a, repacked (exponent 0 if a was denormal)
.L_fsub_return_first:
	cmp r0, #0
	subges r3, r3, #1
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
	; return +0
.L_fsub_return_zero:
	mov r0, #0
	bx lr
	; unreachable repack sequence (no label leads here)
	cmp r0, #0
	subges r3, r3, #1
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
	; return quiet NaN
.L_fsub_second_nan:
	mvn r0, #0x80000000
	bx lr
	; return signed infinity
.L_fsub_return_inf:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
	; return quiet NaN
.L_fsub_return_nan:
	mvn r0, #0x80000000
	bx lr
	; return quiet NaN (Inf - Inf)
.L_fsub_invalid:
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
	; r4 = sign of a in bit 31, bit 0 set: return the remainder
	mov r4, r1
	orr r4, r4, #1
	b .L_ll_sdiv_common
	.size _ll_mod, .-_ll_mod

; _ll_sdiv: signed 64-bit quotient, a / b; also the shared body of _ll_mod (see above).
; Not compiled: _ll_mod branches into its body (.L_ll_sdiv_common) and _ull_mod into .L_ll_sdiv_unsigned;
; a compiled function is entered only at its start. Also (save rule) saves ip, yet writes r0-r3 unsaved.
	.global _ll_sdiv
	.type _ll_sdiv, @function
_ll_sdiv:
	stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
	; r4 = sign of the quotient in bit 31, bit 0 clear: return the quotient
	eor r4, r1, r3
	mov r4, r4, asr #1
	mov r4, r4, lsl #1
	; common code: zero divisor returns
.L_ll_sdiv_common:
	orrs r5, r3, r2
	bne .L_ll_sdiv_nonzero_divisor
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; both operands fit in 32 bits? use the 32-bit divider
.L_ll_sdiv_nonzero_divisor:
	mov r5, r0, lsr #31
	add r5, r5, r1
	mov r6, r2, lsr #31
	add r6, r6, r3
	orrs r6, r5, r6
	bne .L_ll_sdiv_abs_dividend
	; r0 = a, r1 = b; the quotient or (bit 0 of r4) the remainder, sign-extended into r1
	mov r1, r2
	bl _s32_div_f
	ands r4, r4, #1
	movne r0, r1
	mov r1, r0, asr #31
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; take absolute values of a and b
.L_ll_sdiv_abs_dividend:
	cmp r1, #0
	bge .L_ll_sdiv_abs_divisor
	rsbs r0, r0, #0
	rsc r1, r1, #0
.L_ll_sdiv_abs_divisor:
	cmp r3, #0
	bge .L_ll_sdiv_unsigned
	rsbs r2, r2, #0
	rsc r3, r3, #0
	; dividend zero -> result zero; else align divisor and count steps
.L_ll_sdiv_unsigned:
	orrs r5, r1, r0
	beq .L_ll_sdiv_zero
	mov r5, #0
	mov r6, #1
	cmp r3, #0
	bmi .L_ll_sdiv_align_dividend
	; shift the divisor left until its top bit is set; r5 counts the shifts, r6 = r5 + 1
.L_ll_sdiv_align_divisor:
	add r5, r5, #1
	adds r2, r2, r2
	adcs r3, r3, r3
	bpl .L_ll_sdiv_align_divisor
	add r6, r6, r5
	; shift the dividend left until its top bit is set, at most r6 - 1 times; r6 = steps left
.L_ll_sdiv_align_dividend:
	cmp r1, #0
	blt .L_ll_sdiv_divide
.L_ll_sdiv_align_dividend_loop:
	cmp r6, #1
	beq .L_ll_sdiv_divide
	sub r6, r6, #1
	adds r0, r0, r0
	adcs r1, r1, r1
	bpl .L_ll_sdiv_align_dividend_loop
	; long division loop (subtract / add back, one quotient bit per pass); partial remainder in
	; r7:r1:r0, quotient in r11:ip
.L_ll_sdiv_divide:
	mov r7, #0
	mov ip, #0
	mov r11, #0
	b .L_ll_sdiv_subtract
	; remainder not negative: quotient bit 1; count the step, then shift the remainder left
.L_ll_sdiv_set_bit:
	orr ip, ip, #1
	subs r6, r6, #1
	beq .L_ll_sdiv_select
	adds r0, r0, r0
	adcs r1, r1, r1
	adcs r7, r7, r7
	; subtract the divisor, shift the quotient left
.L_ll_sdiv_subtract:
	subs r0, r0, r2
	sbcs r1, r1, r3
	sbcs r7, r7, #0
	adds ip, ip, ip
	adc r11, r11, r11
	cmp r7, #0
	bge .L_ll_sdiv_set_bit
	; remainder negative: quotient bit 0; count the step, shift the remainder left, add the divisor
.L_ll_sdiv_add_back:
	subs r6, r6, #1
	beq .L_ll_sdiv_restore
	adds r0, r0, r0
	adcs r1, r1, r1
	adc r7, r7, r7
	adds r0, r0, r2
	adcs r1, r1, r3
	adc r7, r7, #0
	adds ip, ip, ip
	adc r11, r11, r11
	cmp r7, #0
	bge .L_ll_sdiv_set_bit
	b .L_ll_sdiv_add_back
	; last step left the remainder negative: add the divisor back
.L_ll_sdiv_restore:
	adds r0, r0, r2
	adc r1, r1, r3
	; select quotient or remainder and undo the alignment shift
.L_ll_sdiv_select:
	ands r7, r4, #1
	moveq r0, ip
	moveq r1, r11
	beq .L_ll_sdiv_apply_sign
	; remainder: shift right by r5, the divisor's alignment
	subs r7, r5, #32
	movge r0, r1, lsr r7
	bge .L_ll_sdiv_clear_high
	rsb r7, r5, #32
	mov r0, r0, lsr r5
	orr r0, r0, r1, lsl r7
	mov r1, r1, lsr r5
	b .L_ll_sdiv_apply_sign
	; unreachable (no label leads here)
	mov r0, r1, lsr r7
	mov r1, #0
	; apply the result sign
.L_ll_sdiv_apply_sign:
	cmp r4, #0
	blt .L_ll_sdiv_negate
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; negate the 64-bit result
.L_ll_sdiv_negate:
	rsbs r0, r0, #0
	rsc r1, r1, #0
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; zero dividend: result 0
.L_ll_sdiv_zero:
	mov r0, #0
	; high word 0, then apply the sign
.L_ll_sdiv_clear_high:
	mov r1, #0
	cmp r4, #0
	blt .L_ll_sdiv_negate
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
	mov r4, #0			; return the quotient
	b .L_ull_mod_common
	.size _ll_udiv, .-_ll_udiv

; _ull_mod: unsigned 64-bit remainder, a % b; also the shared body of _ll_udiv (see above).
; Not compiled: calls .L_u32_div_f_nonzero_divisor inside _u32_div_f and branches into the middle of _ll_sdiv
; (.L_ll_sdiv_unsigned); a compiled function is entered only at its start.
	.global _ull_mod
	.type _ull_mod, @function
_ull_mod:
	stmdb sp!, {r4, r5, r6, r7, r11, ip, lr}
	mov r4, #1			; return the remainder
	; common code: zero divisor returns
.L_ull_mod_common:
	orrs r5, r3, r2
	bne .L_ull_mod_nonzero_divisor
	ldmia sp!, {r4, r5, r6, r7, r11, ip, lr}
	bx lr
	; either high word nonzero: 64-bit divide in _ll_sdiv; else the 32-bit divider with r0 = a, r1 = b
.L_ull_mod_nonzero_divisor:
	orrs r5, r1, r3
	bne .L_ll_sdiv_unsigned
	mov r1, r2
	bl .L_u32_div_f_nonzero_divisor
	cmp r4, #0
	movne r0, r1
	mov r1, #0
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
	; r4:r5 = low words' product; add both cross products into the high word
	umull r5, r4, r0, r2
	mla r4, r0, r3, r4
	mla r4, r2, r1, r4
	mov r1, r4
	mov r0, r5
	ldmfd sp!, {r4, r5, lr}
	bx lr
	.size _ll_mul, .-_ll_mul
