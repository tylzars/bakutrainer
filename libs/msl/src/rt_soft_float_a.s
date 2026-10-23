; Soft-float runtime, first part: double-precision add/subtract/multiply, conversions between double and
; integer, and single-precision add. A float or int travels in r0 (second operand r1); a double or 64-bit
; integer in the pair r0 (low word) / r1 (high word), the second operand in r2/r3.
; _fadd and _fsub (in rt_soft_float_b.s) branch into each other: the entry .L_fadd_add_magnitudes inside _fadd
; is global.
; "Not compiled" lines say why the compiler cannot have produced a routine. Save rule: the stmfd list of a
; compiled ARM function holds every register among r4-r7 it writes and, unless it is an interrupt function, also
; every one among r8-r11; lr if it calls or writes lr; and r3 when the saved registers and its stack frame would
; otherwise come to an odd number of words, or in place of a one-word frame. r0-r2 and r12 appear only in an
; interrupt function, which saves every register among r0-r7 it writes. A compiled function that saved lr
; returns by loading it into pc (a variadic one, which starts with stmfd {r0-r3}, excepted).

	.text
	.global .L_fadd_add_magnitudes
	.extern .L_fsub_sub_magnitudes

; _dadd: double-precision addition, a in r1:r0 (high:low), b in r3:r2, result in r1:r0.
; Layout of the working registers:
;   ip   = sign (bit 11) | biased exponent of the larger-magnitude operand
;   r1:r0 = its mantissa, left-justified with the hidden 1 forced into bit 63
;   r4   = exponent of the other operand, r3:r2 = its mantissa in the same form
; Operands whose signs differ are not handled here: b's sign is flipped and control
; jumps into the magnitude-subtract body inside _dsub (.L_dsub_sub_magnitudes, _dsub+0x10).
; _dsub in turn jumps back to .L_dadd_add_magnitudes (_dadd+0x10) for operands that end up
; with equal signs.
; Special cases: NaN in either operand gives the all-ones-mantissa NaN 0x7fffffffffffffff,
; Inf + finite = Inf, Inf + Inf (same sign) = Inf, zero + x returns x, denormal inputs are
; treated as exponent 1 without hidden bit. Overflow gives +-Inf. Result is rounded to
; nearest, ties to even, using the 11 bits shifted out plus a sticky bit.
; Not compiled: branches into the middle of _dsub (.L_dsub_sub_magnitudes), and _dsub branches back into
; this body (.L_dadd_add_magnitudes); a compiled function is entered only at its start.
	.global _dadd
	.type _dadd, @function
_dadd:
	stmfd sp!, {r4, lr}
	; different signs? then flip b's sign and take the subtract path
	eors ip, r1, r3
	eormi r3, r3, #0x80000000
	bmi .L_dsub_sub_magnitudes
.L_dadd_add_magnitudes:
	; order the operands so r1:r0 has the larger magnitude (swap using the difference)
	subs ip, r0, r2
	sbcs lr, r1, r3
	bhs .L_dadd_unpack
	adds r2, r2, ip
	adc r3, r3, lr
	subs r0, r0, ip
	sbc r1, r1, lr
	; unpack the larger operand: ip = sign and exponent, r1:r0 = mantissa shifted left 11, hidden bit set
.L_dadd_unpack:
	mov lr, #0x80000000
	mov ip, r1, lsr #20
	orr r1, lr, r1, lsl #11
	orr r1, r1, r0, lsr #21
	mov r0, r0, lsl #11
	; exponent of 0 or 0x7ff?
	movs r4, ip, lsl #21
	cmnne r4, #0x200000
	beq .L_dadd_first_special
	; unpack the other operand the same way (r4 = sign and exponent); exponent 0 is special
	mov r4, r3, lsr #20
	orr r3, lr, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs lr, r4, lsl #21
	beq .L_dadd_second_special
	; both operands normal: r4 = exponent difference; align the smaller mantissa
.L_dadd_align:
	subs r4, ip, r4
	beq .L_dadd_add
	cmp r4, #32
	ble .L_dadd_align_short
	; difference above 32: shift the high word right by the difference (63 from 56 on) less 32, the bits
	; shifted out and the low word folded into the sticky bit 0; add
	cmp r4, #56
	movge r4, #63
	sub r4, r4, #32
	rsb lr, r4, #32
	orrs lr, r2, r3, lsl lr
	mov r2, r3, lsr r4
	orrne r2, r2, #1
	adds r0, r0, r2
	adcs r1, r1, #0
	blo .L_dadd_pack_round
	b .L_dadd_carry
	; difference 1..32: shift the smaller mantissa right across both words, sticky into bit 0
.L_dadd_align_short:
	rsb lr, r4, #32
	movs lr, r2, lsl lr
	rsb lr, r4, #32
	mov r2, r2, lsr r4
	orr r2, r2, r3, lsl lr
	mov r3, r3, lsr r4
	orrne r2, r2, #1
	; add the mantissas; no carry out means the sum is still normalized
.L_dadd_add:
	adds r0, r0, r2
	adcs r1, r1, r3
	blo .L_dadd_pack_round
	; carry out: shift right one (keep sticky), exponent + 1, overflow to Inf if exponent is 0x7ff
.L_dadd_carry:
	add ip, ip, #1
	and r4, r0, #1
	movs r1, r1, rrx
	orr r0, r4, r0, rrx
	mov lr, ip, lsl #21
	cmn lr, #0x200000
	beq .L_dadd_overflow
	; pack: r2 = the 11 bits shifted out (round bits), drop the hidden bit, insert sign and exponent
.L_dadd_pack_round:
	movs r2, r0, lsl #21
	mov r0, r0, lsr #11
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	mov r1, r1, lsr #12
	orr r1, r1, ip, lsl #20
	; round to nearest even: return below half, or at exactly half with an even result; else add 1
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmfd sp!, {r4, lr}
	bx lr
	; first operand has exponent 0 or 0x7ff; lr = its sign
.L_dadd_first_special:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bics ip, ip, #0x800
	beq .L_dadd_first_exp0
	; exponent 0x7ff: NaN if the mantissa is nonzero
	orrs r4, r0, r1, lsl #1
	bne .L_dadd_return_nan
	; Inf: unpack the second operand; the result is Inf unless the second operand is NaN
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r4, r4, lsl #21
	beq .L_dadd_return_inf
	cmn r4, #0x200000
	bne .L_dadd_return_inf
	orrs r4, r2, r3, lsl #1
	beq .L_dadd_return_inf
	b .L_dadd_return_nan
	; second operand has exponent 0 or 0x7ff; lr = its sign
.L_dadd_second_special:
	cmp r4, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bic ip, ip, #0x800
	bics r4, r4, #0x800
	beq .L_dadd_second_exp0
	; exponent 0x7ff: NaN if the mantissa is nonzero, else Inf
	orrs r4, r2, r3, lsl #1
	bne .L_dadd_return_nan
	b .L_dadd_return_inf
	; first operand has exponent 0: a zero goes on below; a denormal uses exponent 1, no hidden bit
.L_dadd_first_exp0:
	orrs r4, r0, r1, lsl #1
	beq .L_dadd_first_zero
	mov ip, #1
	bic r1, r1, #0x80000000
	; unpack the second operand; exponent 0 or 0x7ff goes to its special cases
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r4, r4, lsl #21
	cmnne r4, #0x200000
	mov r4, r4, lsr #21
	orr r4, r4, lr, lsr #20
	beq .L_dadd_second_special
	; second operand normal: set its hidden bit, put the sign back into ip, align
	orr r3, r3, #0x80000000
	orr ip, ip, lr, lsr #20
	b .L_dadd_align
	; first operand is zero: the result is the other operand (after NaN/Inf checks)
.L_dadd_first_zero:
	mov ip, r3, lsr #20
	mov r1, r3, lsl #11
	orr r1, r1, r2, lsr #21
	mov r0, r2, lsl #11
	movs r4, ip, lsl #21
	beq .L_dadd_return_second
	cmn r4, #0x200000
	bne .L_dadd_return_second
	; second operand Inf or NaN
	orrs r4, r0, r1, lsl #1
	beq .L_dadd_return_inf
	b .L_dadd_make_nan
	; second operand is zero or denormal: a zero returns the first; a denormal uses exponent 1
.L_dadd_second_exp0:
	orrs r4, r2, r3, lsl #1
	beq .L_dadd_return_first
	mov r4, #1
	bic r3, r3, #0x80000000
	cmp r1, #0
	bpl .L_dadd_add_denormals
	; first operand normal: put the signs back into both exponents and align
	orr ip, ip, lr, lsr #20
	orr r4, r4, lr, lsr #20
	b .L_dadd_align
	; both denormal-like: add mantissas directly; a carry out shifts right one and raises the exponent
.L_dadd_add_denormals:
	adds r0, r0, r2
	adcs r1, r1, r3
	blo .L_dadd_pack_denormal
	add ip, ip, #1
	and r4, r0, #1
	movs r1, r1, rrx
	mov r0, r0, rrx
	orr r0, r0, r4
	; pack (exponent 0 unless the sum reached bit 63); returns when the round bits are clear or a
	; tie meets an even result. A denormal sum has no round bits, so the round-up case never
	; occurs; the code would fall through into the next block
.L_dadd_pack_denormal:
	cmp r1, #0
	subges ip, ip, #1
	movs r2, r0, lsl #21
	mov r0, r0, lsr #11
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	; return the second operand unchanged
.L_dadd_return_second:
	mov r1, r3
	mov r0, r2
	ldmfd sp!, {r4, lr}
	bx lr
	; second operand is zero: repack the first operand (exponent 0 if it is a denormal)
.L_dadd_return_first:
	cmp r1, #0
	subges ip, ip, #1
	mov r0, r0, lsr #11
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmfd sp!, {r4, lr}
	bx lr
	; result is a signed infinity (sign in lr)
.L_dadd_return_inf:
	ldr r1, .L_dadd_inf_high
	orr r1, lr, r1
	mov r0, #0
	ldmfd sp!, {r4, lr}
	bx lr
	; NaN result: all-ones mantissa, exponent 0x7ff, sign cleared
.L_dadd_return_nan:
	mov r1, r3
	; second entry; both give the same constant (r1 is overwritten)
.L_dadd_make_nan:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; unreachable duplicate of the NaN return
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; exponent overflow: signed infinity
.L_dadd_overflow:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	ldr r1, .L_dadd_inf_high
	orr r1, lr, r1
	mov r0, #0
	ldmfd sp!, {r4, lr}
	bx lr
	; high word of +Inf
.L_dadd_inf_high:
	.word 0x7ff00000
	.size _dadd, .-_dadd

; _dfix: double (r1:r0) to signed 32-bit integer, truncating toward zero.
; shift = 0x41e - biased exponent. shift <= 0 (|x| >= 2^31, Inf, NaN) saturates by sign
; to 0x7fffffff / 0x80000000; shift >= 32 (|x| < 1) gives 0; otherwise the top 32
; mantissa bits (hidden bit included) are shifted right by `shift` and negated if x < 0.
; Not compiled: `subs r2, r2, r3, lsr #20` is followed by `ble`.
; The compiler makes a flag-setting subtract only by folding a following `cmp rd, #0` into it, which leaves
; the branch testing eq, ne, mi or pl (ge and lt become pl and mi, hs becomes always; lo, gt and le are not
; folded), or inside 64-bit subtract, compare and shift sequences, where the next sbc, sbcs or conditional orr
; reads the flags.
	.global _dfix
	.type _dfix, @function
_dfix:
	; r2 = shift = 0x41e - exponent (sign bit cleared first)
	bic r3, r1, #0x80000000
	ldr r2, .L_dfix_shift_base
	subs r2, r2, r3, lsr #20
	ble .L_dfix_saturate
	cmp r2, #32
	bge .L_dfix_zero
	; top 32 mantissa bits with the hidden bit, shifted right; negated when x < 0
	mov r3, r1, lsl #11
	orr r3, r3, #0x80000000
	orr r3, r3, r0, lsr #21
	cmp r1, #0
	mov r0, r3, lsr r2
	rsbmi r0, r0, #0
	bx lr
	; |x| < 1: zero
.L_dfix_zero:
	mov r0, #0
	bx lr
	; out of range: saturate by sign
.L_dfix_saturate:
	mvn r0, r1, asr #31
	add r0, r0, #0x80000000
	bx lr
	; exponent of 2^31 (0x3ff + 31)
.L_dfix_shift_base:
	.word 0x41e
	.size _dfix, .-_dfix

; _ll_ufrom_d: double (r1:r0) to unsigned 64-bit integer (result r1:r0), truncating.
; shift = 0x43e - biased exponent. Negative inputs give 0 (a negative NaN gives all ones);
; shift < 0 saturates to all ones; shift >= 64 gives 0; otherwise the 53-bit mantissa,
; left-justified in a 64-bit value, is shifted right by `shift`.
; Not compiled: `subs r2, r2, r1, lsr #20` is followed by `blt`.
; The compiler makes a flag-setting subtract only by folding a following `cmp rd, #0` into it, which leaves
; the branch testing eq, ne, mi or pl (ge and lt become pl and mi, hs becomes always; lo, gt and le are not
; folded), or inside 64-bit subtract, compare and shift sequences, where the next sbc, sbcs or conditional orr
; reads the flags.
	.global _ll_ufrom_d
	.type _ll_ufrom_d, @function
_ll_ufrom_d:
	tst r1, #0x80000000
	bne .L_ll_ufrom_d_negative
	; r2 = shift = 0x43e - exponent
	ldr r2, .L_ll_ufrom_d_shift_base
	subs r2, r2, r1, lsr #20
	blt .L_ll_ufrom_d_saturate
	cmp r2, #64
	bge .L_ll_ufrom_d_zero
	; ip = top 32 mantissa bits with the hidden bit
	mov ip, r1, lsl #11
	orr ip, ip, #0x80000000
	orr ip, ip, r0, lsr #21
	cmp r2, #32
	ble .L_ll_ufrom_d_shift_both
	; shift above 32: only the high mantissa word reaches the result
	sub r2, r2, #32
	mov r1, #0
	mov r0, ip, lsr r2
	bx lr
	; shift <= 32: two-word shift
.L_ll_ufrom_d_shift_both:
	mov r3, r0, lsl #11
	mov r1, ip, lsr r2
	mov r0, r3, lsr r2
	rsb r2, r2, #32
	orr r0, r0, ip, lsl r2
	bx lr
	; |x| < 1: zero
.L_ll_ufrom_d_zero:
	mov r1, #0
	mov r0, #0
	bx lr
	; negative input: 0 unless NaN
.L_ll_ufrom_d_negative:
	cmn r1, #0x100000
	cmpeq r0, #0
	bhi .L_ll_ufrom_d_saturate
	mov r1, #0
	mov r0, #0
	bx lr
	; saturate to all ones
.L_ll_ufrom_d_saturate:
	mvn r1, #0
	mvn r0, #0
	bx lr
	; exponent of 2^63 (0x3ff + 63)
.L_ll_ufrom_d_shift_base:
	.word 0x43e
	.size _ll_ufrom_d, .-_ll_ufrom_d

; _dflt: signed 32-bit integer (r0) to double (r1:r0).
; Takes the magnitude, returns +0 for zero, normalizes with a leading-zero count, and
; builds the exponent as 0x41e - clz. The 32-bit value always fits in the 53-bit
; mantissa, so no rounding is needed; the hidden bit is left out when packing.
; Not compiled: C has no operator or intrinsic for `clz` in this compiler; an asm statement would still be
; assembly.
	.global _dflt
	.type _dflt, @function
_dflt:
	; r2 = sign; take magnitude; zero returns +0
	ands r2, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r0, #0
	mov r1, #0
	bxeq lr
	; r3 = 0x41e, the exponent of a value whose top bit is bit 31
	mov r3, #0x400
	add r3, r3, #0x1e
	; normalize: shift left by the leading-zero count, exponent = 0x41e - count
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	; pack: low word = the mantissa's low 11 bits; high word = sign | exponent | the next 20 bits
	movs r1, r0
	mov r0, r1, lsl #21
	add r1, r1, r1
	orr r1, r2, r1, lsr #12
	orr r1, r1, r3, lsl #20
	bx lr
	.size _dflt, .-_dflt

; _dmul: double-precision multiplication, a in r1:r0, b in r3:r2, result in r1:r0.
; lr = sign of the result (xor of the input signs). Both mantissas are unpacked
; left-justified with the hidden bit; the 64x64 -> 128-bit product is formed from four
; 32x32 umull partial products, the top 64 bits are kept and any nonzero low half is
; folded into bit 0 as a sticky bit. The exponent is ea + eb - 0x3fe; if the product's
; top bit is clear it is shifted left once and the exponent decremented.
; Special cases: NaN operand -> NaN (0x7fffffffffffffff); Inf * 0 -> the same NaN;
; Inf * nonzero -> signed Inf; zero * finite -> signed zero; denormal operands are
; normalized first with a leading-zero count. Exponent overflow -> signed Inf; exponent
; underflow shifts into a denormal (smallest denormal / zero when far enough down).
; Rounding is to nearest, ties to even.
; Not compiled: C has no operator or intrinsic for `clz` in this compiler; an asm statement would still be
; assembly. Also (save rule) it saves five words {r4-r7, lr} with no frame and no r3, then pops lr and returns
; with bx lr.
	.global _dmul
	.type _dmul, @function
_dmul:
	stmfd sp!, {r4, r5, r6, r7, lr}
	; sign of the product
	eor lr, r1, r3
	and lr, lr, #0x80000000
	; unpack a: ip = sign and exponent, r1:r0 = mantissa shifted left 11; exponent 0 or 0x7ff is special
	mov ip, r1, lsr #20
	mov r1, r1, lsl #11
	orr r1, r1, r0, lsr #21
	mov r0, r0, lsl #11
	movs r6, ip, lsl #21
	cmnne r6, #0x200000
	beq .L_dmul_first_special
	orr r1, r1, #0x80000000
	bic ip, ip, #0x800
	; unpack b the same way into r4 and r3:r2
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r5, r4, lsl #21
	cmnne r5, #0x200000
	beq .L_dmul_second_special
	orr r3, r3, #0x80000000
	bic r4, r4, #0x800
	; 128-bit product of r1:r0 and r3:r2 (four 32x32 partial products); ip = ea + eb
.L_dmul_multiply:
	add ip, r4, ip
	umull r5, r4, r0, r2
	umull r7, r6, r0, r3
	adds r4, r7, r4
	adc r6, r6, #0
	umull r7, r0, r1, r2
	adds r4, r7, r4
	adcs r0, r0, r6
	umull r7, r2, r1, r3
	adc r1, r2, #0
	adds r0, r0, r7
	adc r1, r1, #0
	; keep the top 64 bits in r1:r0; a nonzero low half sets the sticky bit 0
	orrs r4, r4, r5
	orrne r0, r0, #1
	; top bit clear: shift left one, exponent - 1
	cmp r1, #0
	blt .L_dmul_rebias
	sub ip, ip, #1
	adds r0, r0, r0
	adc r1, r1, r1
	; rebias exponent (+ 2 - 0x400); <= 0 means underflow, >= 0x7ff means overflow
.L_dmul_rebias:
	add ip, ip, #2
	subs ip, ip, #0x400
	bmi .L_dmul_underflow
	beq .L_dmul_underflow
	mov r6, ip, lsl #20
	cmn r6, #0x100000
	bmi .L_dmul_overflow
	; pack: r2 = the 11 bits shifted out (round bits), drop the hidden bit, insert sign and exponent
	movs r2, r0, lsl #21
	mov r0, r0, lsr #11
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	; round to nearest even: return when the round bits are zero, below half, or exactly half with an even
	; result; else add 1
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; first operand exponent is 0 or 0x7ff
.L_dmul_first_special:
	; special-case path: strip sign from the exponent; exponent 0 is zero or denormal
	bics ip, ip, #0x800
	beq .L_dmul_first_exp0
	; exponent 0x7ff: NaN if the mantissa is nonzero
	orrs r6, r0, r1, lsl #1
	bne .L_dmul_return_nan
	; Inf: unpack b; Inf * normal is Inf, Inf * Inf is Inf, Inf * NaN is NaN
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r5, r4, lsl #21
	beq .L_dmul_inf_times_exp0
	cmn r5, #0x200000
	bne .L_dmul_return_inf
	orrs r5, r2, r3, lsl #1
	beq .L_dmul_return_inf
	b .L_dmul_return_nan
	; second operand is zero/denormal: zero -> NaN (Inf * 0), else Inf
.L_dmul_inf_times_exp0:
	orrs r5, r3, r2
	beq .L_dmul_invalid
	b .L_dmul_return_inf
	; second operand exponent is 0 or 0x7ff (first operand finite)
.L_dmul_second_special:
	bics r4, r4, #0x800
	beq .L_dmul_second_exp0
	; exponent 0x7ff: NaN if the mantissa is nonzero, else Inf
	orrs r6, r2, r3, lsl #1
	bne .L_dmul_return_nan
	b .L_dmul_return_inf
	; first operand is zero or denormal
.L_dmul_first_exp0:
	orrs r6, r0, r1, lsl #1
	beq .L_dmul_first_zero
	; denormal: exponent 1; a zero high word moves the low word up (exponent - 32), done if its top bit is set
	mov ip, #1
	cmp r1, #0
	bne .L_dmul_normalize_first
	sub ip, ip, #32
	movs r1, r0
	mov r0, #0
	bmi .L_dmul_unpack_second
	; normalize first operand's denormal mantissa with a leading-zero count
.L_dmul_normalize_first:
	clz r6, r1
	movs r1, r1, lsl r6
	rsb r6, r6, #32
	orr r1, r1, r0, lsr r6
	rsb r6, r6, #32
	mov r0, r0, lsl r6
	sub ip, ip, r6
	; unpack second operand; exponent 0 or 0x7ff goes to its special cases
.L_dmul_unpack_second:
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r5, r4, lsl #21
	cmnne r5, #0x200000
	beq .L_dmul_second_special
	orr r3, r3, #0x80000000
	bic r4, r4, #0x800
	b .L_dmul_multiply
	; first operand is zero: signed zero unless second is Inf/NaN (zero * Inf is NaN)
.L_dmul_first_zero:
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r5, r4, lsl #21
	beq .L_dmul_return_zero
	cmn r5, #0x200000
	bne .L_dmul_return_zero
	orrs r6, r2, r3, lsl #1
	beq .L_dmul_invalid
	b .L_dmul_return_nan
	; second operand is zero or denormal
.L_dmul_second_exp0:
	orrs r5, r2, r3, lsl #1
	beq .L_dmul_return_zero
	; denormal: exponent 1; a zero high word moves the low word up (exponent - 32), done if its top bit is set
	mov r4, #1
	cmp r3, #0
	bne .L_dmul_normalize_second
	sub r4, r4, #32
	movs r3, r2
	mov r2, #0
	bmi .L_dmul_multiply
	; normalize second operand's denormal mantissa
.L_dmul_normalize_second:
	clz r6, r3
	movs r3, r3, lsl r6
	rsb r6, r6, #32
	orr r3, r3, r2, lsr r6
	rsb r6, r6, #32
	mov r2, r2, lsl r6
	sub r4, r4, r6
	b .L_dmul_multiply
	; result exponent <= 0: shift right into a denormal, rounding to nearest even
.L_dmul_underflow:
	cmn ip, #52
	beq .L_dmul_half_denormal
	bmi .L_dmul_underflow_zero
	; r2 = the bits that the shift drops, left-justified, with a sticky bit 0 from any below them
	mov r2, r1
	mov r3, r0
	add r4, ip, #52
	cmp r4, #32
	movge r2, r3
	movge r3, #0
	subge r4, r4, #32
	rsb r5, r4, #32
	mov r2, r2, lsl r4
	orr r2, r2, r3, lsr r5
	movs r3, r3, lsl r4
	orrne r2, r2, #1
	; shift r1:r0 right by 12 - exponent into the denormal field; sign into the high word
	rsb ip, ip, #12
	cmp ip, #32
	movge r0, r1
	movge r1, #0
	subge ip, ip, #32
	rsb r4, ip, #32
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	; round to nearest even using r2
	cmp r2, #0
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; half the smallest denormal: round up only if any other bit is set
.L_dmul_half_denormal:
	orr r0, r0, r1, lsl #1
	b .L_dmul_round_min_denormal
	; signed infinity
.L_dmul_return_inf:
	ldr r1, .L_dmul_inf_high
	orr r1, lr, r1
	mov r0, #0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; NaN (all-ones mantissa)
.L_dmul_return_nan:
	mov r1, r3
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; NaN (Inf * 0)
.L_dmul_invalid:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; return smallest denormal if r0 nonzero, else signed zero
.L_dmul_round_min_denormal:
	movs r2, r0
	mov r1, lr
	mov r0, #0
	addne r0, r0, #1
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; underflow to signed zero
.L_dmul_underflow_zero:
	mov r1, lr
	mov r0, #0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; overflow: signed infinity
.L_dmul_overflow:
	ldr r1, .L_dmul_inf_high
	orr r1, lr, r1
	mov r0, #0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; signed zero
.L_dmul_return_zero:
	mov r1, lr
	mov r0, #0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; high word of +Inf
.L_dmul_inf_high:
	.word 0x7ff00000
	.size _dmul, .-_dmul

; _drsb: reverse-operand entry for _dsub: swaps the two double operands with three
; exclusive-ors each and falls through into _dsub, which follows directly.
; Not compiled: has no return; execution runs on into _dsub, the next routine. Each compiled function
; is a section of its own, so none can rely on the code placed after it.
	.global _drsb
	.type _drsb, @function
_drsb:
	; swap r1 and r3, then r0 and r2
	eor r1, r1, r3
	eor r3, r1, r3
	eor r1, r1, r3
	eor r0, r0, r2
	eor r2, r0, r2
	eor r0, r0, r2
	.size _drsb, .-_drsb

; _dsub: double-precision subtraction a - b (a in r1:r0, b in r3:r2), result in r1:r0.
; Mirror image of _dadd: when the signs differ, b's sign is flipped and control jumps
; into _dadd's magnitude-add body (.L_dadd_add_magnitudes, _dadd+0x10). Otherwise
; magnitudes are subtracted after ordering the operands so that the
; larger is in r1:r0 (swapping them also flips the result sign). The smaller mantissa is
; aligned by shifting right with a sticky bit; the difference is renormalized with a
; leading-zero count (zero result -> +0), denormal results are shifted back down, and
; the result is rounded to nearest-even. NaN/Inf/zero/denormal inputs are handled as in
; _dadd (NaN -> 0x7fffffffffffffff; Inf - Inf of the same sign and Inf with NaN -> NaN).
; Not compiled: branches into the middle of _dadd (.L_dadd_add_magnitudes), and _dadd branches back into
; this body (.L_dsub_sub_magnitudes); a compiled function is entered only at its start. Also uses `clz`: C has
; no operator or intrinsic for `clz` in this compiler; an asm statement would still be assembly.
	.global _dsub
	.type _dsub, @function
_dsub:
	stmfd sp!, {r4, lr}
	; different signs? then flip b's sign and take the add path
	eors ip, r1, r3
	eormi r3, r3, #0x80000000
	bmi .L_dadd_add_magnitudes
.L_dsub_sub_magnitudes:
	; order the operands by magnitude; a swap also flips the result sign
	subs ip, r0, r2
	sbcs lr, r1, r3
	bhs .L_dsub_unpack
	eor lr, lr, #0x80000000
	adds r2, r2, ip
	adc r3, r3, lr
	subs r0, r0, ip
	sbc r1, r1, lr
	; unpack the larger operand: ip = sign and exponent, r1:r0 = mantissa shifted left 11, hidden bit set
.L_dsub_unpack:
	mov lr, #0x80000000
	mov ip, r1, lsr #20
	orr r1, lr, r1, lsl #11
	orr r1, r1, r0, lsr #21
	mov r0, r0, lsl #11
	movs r4, ip, lsl #21
	cmnne r4, #0x200000
	beq .L_dsub_first_special
	; unpack the other operand the same way (r4 = sign and exponent); exponent 0 is special
	mov r4, r3, lsr #20
	orr r3, lr, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs lr, r4, lsl #21
	beq .L_dsub_second_special
	; both normal: r4 = exponent difference; align the smaller mantissa and subtract
.L_dsub_align:
	subs r4, ip, r4
	beq .L_dsub_same_exponent
	cmp r4, #32
	ble .L_dsub_align_short
	; difference above 32: shift the high word right by the difference (63 from 56 on) less 32, the bits
	; shifted out and the low word folded into the sticky bit 0
	cmp r4, #56
	movge r4, #63
	sub r4, r4, #32
	rsb lr, r4, #32
	orrs lr, r2, r3, lsl lr
	mov r2, r3, lsr r4
	orrne r2, r2, #1
	; subtract; top bit still set: pack and round, else renormalize
	subs r0, r0, r2
	sbcs r1, r1, #0
	bmi .L_dsub_pack_round
	b .L_dsub_renormalize
	; difference 1..32: shift right across both words, sticky into bit 0; subtract
.L_dsub_align_short:
	rsb lr, r4, #32
	movs lr, r2, lsl lr
	rsb lr, r4, #32
	mov r2, r2, lsr r4
	orr r2, r2, r3, lsl lr
	mov r3, r3, lsr r4
	orrne r2, r2, #1
	subs r0, r0, r2
	sbcs r1, r1, r3
	bpl .L_dsub_renormalize
	; pack: r2 = the 11 bits shifted out (round bits), drop the hidden bit, insert sign and exponent
.L_dsub_pack_round:
	movs r2, r0, lsl #21
	mov r0, r0, lsr #11
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	mov r1, r1, lsr #12
	orr r1, r1, ip, lsl #20
	; round to nearest even: return below half, or at exactly half with an even result; else add 1
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmfd sp!, {r4, lr}
	bx lr
	; exponents equal: subtract; zero result gives +0, else renormalize
.L_dsub_same_exponent:
	subs r0, r0, r2
	sbc r1, r1, r3
	orrs lr, r1, r0
	beq .L_dsub_return_zero
	; lr = sign, ip = exponent alone
	mov lr, ip, lsl #20
	and lr, lr, #0x80000000
	bic ip, ip, #0x800
	cmp r1, #0
	bmi .L_dsub_same_exp_top_set
	bne .L_dsub_same_exp_clz
	; high word zero: move the low word up (exponent - 32); done if its top bit is set
	sub ip, ip, #32
	movs r1, r0
	mov r0, #0
	bmi .L_dsub_same_exp_range
	; leading-zero normalization of the difference (high word nonzero)
.L_dsub_same_exp_clz:
	clz r4, r1
	movs r1, r1, lsl r4
	rsb r4, r4, #32
	orr r1, r1, r0, lsr r4
	rsb r4, r4, #32
	mov r0, r0, lsl r4
	sub ip, ip, r4
	; exponent > 0: pack; otherwise the result is denormal: shift right by 12 - exponent
.L_dsub_same_exp_range:
	cmp ip, #0
	bgt .L_dsub_pack_exact
	rsb ip, ip, #12
	cmp ip, #32
	movge r0, r1
	movge r1, #0
	subge ip, ip, #32
	rsb r4, ip, #32
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	ldmfd sp!, {r4, lr}
	bx lr
	; top bit set: already normalized (reached only with r1 negative, so the decrement
	; never applies)
.L_dsub_same_exp_top_set:
	cmp r1, #0
	subges ip, ip, #1
	; pack (positive exponent): remove hidden bit, insert exponent and sign
.L_dsub_pack_exact:
	mov r0, r0, lsr #11
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmfd sp!, {r4, lr}
	bx lr
	; result needs renormalization: lr = sign, ip = exponent alone; a zero high word moves the low word up
	; (exponent - 32), done if its top bit is set
.L_dsub_renormalize:
	mov lr, ip, lsl #20
	and lr, lr, #0x80000000
	bic ip, ip, #0x800
	cmp r1, #0
	bne .L_dsub_renormalize_clz
	sub ip, ip, #32
	movs r1, r0
	mov r0, #0
	bmi .L_dsub_renormalize_range
	; leading-zero normalization (high word nonzero)
.L_dsub_renormalize_clz:
	clz r4, r1
	movs r1, r1, lsl r4
	rsb r4, r4, #32
	orr r1, r1, r0, lsr r4
	rsb r4, r4, #32
	mov r0, r0, lsl r4
	sub ip, ip, r4
	; exponent > 0: put the sign back, pack via the common rounding tail; else denormal shift
.L_dsub_renormalize_range:
	cmp ip, #0
	orrgt ip, ip, lr, lsr #20
	bgt .L_dsub_pack_round
	rsb ip, ip, #12
	cmp ip, #32
	movge r0, r1
	movge r1, #0
	subge ip, ip, #32
	rsb r4, ip, #32
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	ldmfd sp!, {r4, lr}
	bx lr
	; first operand exponent 0 or 0x7ff; lr = its sign
.L_dsub_first_special:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bics ip, ip, #0x800
	beq .L_dsub_first_exp0
	; exponent 0x7ff: NaN if the mantissa is nonzero
	orrs r4, r0, r1, lsl #1
	bne .L_dsub_return_nan
	; Inf: unpack the second operand; Inf - Inf is NaN, Inf - NaN is NaN, otherwise Inf
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r4, r4, lsl #21
	beq .L_dsub_return_inf
	cmn r4, #0x200000
	bne .L_dsub_return_inf
	orrs r4, r2, r3, lsl #1
	beq .L_dsub_invalid
	b .L_dsub_return_nan
	; second operand exponent 0 or 0x7ff; lr = its sign
.L_dsub_second_special:
	cmp r4, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bic ip, ip, #0x800
	bics r4, r4, #0x800
	beq .L_dsub_second_exp0
	; exponent 0x7ff: NaN if the mantissa is nonzero, else Inf
	orrs r4, r2, r3, lsl #1
	bne .L_dsub_return_nan
	b .L_dsub_return_inf
	; first operand denormal or zero: a denormal uses exponent 1, no hidden bit
.L_dsub_first_exp0:
	orrs r4, r0, r1, lsl #1
	beq .L_dsub_first_zero
	mov ip, #1
	bic r1, r1, #0x80000000
	; unpack the second operand; exponent 0 or 0x7ff goes to its special cases
	mov r4, r3, lsr #20
	mov r3, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #11
	movs r4, r4, lsl #21
	cmnne r4, #0x200000
	mov r4, r4, lsr #21
	orr r4, r4, lr, lsr #20
	beq .L_dsub_second_special
	; second operand normal: set its hidden bit, put the sign back into ip, align
	orr r3, r3, #0x80000000
	orr ip, ip, lr, lsr #20
	b .L_dsub_align
	; first operand is zero: a zero second operand gives +0, a finite nonzero one is returned
	; as is, Inf/NaN give +-Inf/NaN
.L_dsub_first_zero:
	mov ip, r3, lsr #20
	mov r1, r3, lsl #11
	orr r1, r1, r2, lsr #21
	mov r0, r2, lsl #11
	movs r4, ip, lsl #21
	beq .L_dsub_zero_minus_exp0
	cmn r4, #0x200000
	bne .L_dsub_return_second
	orrs r4, r0, r1, lsl #1
	bne .L_dsub_make_nan
	b .L_dsub_return_inf
	; second operand exponent 0: zero gives +0, denormal is returned as is
.L_dsub_zero_minus_exp0:
	orrs r4, r0, r1, lsl #1
	beq .L_dsub_return_zero
	b .L_dsub_return_second
	; second operand zero or denormal: a zero returns the first; a denormal uses exponent 1, then align
.L_dsub_second_exp0:
	orrs r4, r2, r3, lsl #1
	beq .L_dsub_return_first
	mov r4, #1
	bic r3, r3, #0x80000000
	orr ip, ip, lr, lsr #20
	orr r4, r4, lr, lsr #20
	b .L_dsub_align
	; return the second operand unchanged
.L_dsub_return_second:
	mov r1, r3
	mov r0, r2
	ldmfd sp!, {r4, lr}
	bx lr
	; second operand is zero: repack first (exponent 0 if it is a denormal)
.L_dsub_return_first:
	cmp r1, #0
	subges ip, ip, #1
	mov r0, r0, lsr #11
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmfd sp!, {r4, lr}
	bx lr
	; signed infinity
.L_dsub_return_inf:
	ldr r1, .L_dsub_inf_high
	orr r1, lr, r1
	mov r0, #0
	ldmfd sp!, {r4, lr}
	bx lr
	; NaN
.L_dsub_return_nan:
	mov r1, r3
	; NaN (all-ones mantissa)
.L_dsub_make_nan:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; NaN (Inf - Inf)
.L_dsub_invalid:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; +0
.L_dsub_return_zero:
	mov r1, #0
	mov r0, #0
	ldmfd sp!, {r4, lr}
	bx lr
	; high word of +Inf
.L_dsub_inf_high:
	.word 0x7ff00000
	.size _dsub, .-_dsub

; _fadd: single-precision addition, a in r0, b in r1, result r0.
; Operands with different signs get b's sign flipped and control jumps into the
; magnitude-subtract body of _fsub (.L_fsub_sub_magnitudes, _fsub+0xc). Otherwise
; the larger magnitude is placed in r0, both mantissas are left-justified with the hidden
; bit at bit 31, b is shifted right by the exponent difference keeping a sticky bit,
; the mantissas are added (a carry shifts right by one and bumps the exponent; exponent
; 255 gives +-Inf), and the 8 bits shifted out are rounded to nearest, ties to even.
; NaN -> 0x7fffffff, Inf + finite or Inf + Inf -> Inf, a zero operand returns the other
; one, denormal inputs are treated as exponent 1 without hidden bit.
; Not compiled: branches into the middle of _fsub (.L_fsub_sub_magnitudes), and _fsub branches back into this
; body (.L_fadd_add_magnitudes); a compiled function is entered only at its start.
	.global _fadd
	.type _fadd, @function
_fadd:
	; different signs? flip b and take the subtract path
	eors r2, r0, r1
	eormi r1, r1, #0x80000000
	bmi .L_fsub_sub_magnitudes
.L_fadd_add_magnitudes:
	; order by magnitude (swap when |a| < |b|)
	subs ip, r0, r1
	sublo r0, r0, ip
	addlo r1, r1, ip
	; unpack a: r3 = sign|exponent, r0 = mantissa with hidden bit at bit 31; exponent 0 or 255 is special
	mov r2, #0x80000000
	mov r3, r0, lsr #23
	orr r0, r2, r0, lsl #8
	ands ip, r3, #0xff
	cmpne ip, #0xff
	beq .L_fadd_first_special
	; unpack b the same way: ip = sign|exponent, r1 = mantissa; exponent 0 is special
	mov ip, r1, lsr #23
	orr r1, r2, r1, lsl #8
	ands r2, ip, #0xff
	beq .L_fadd_second_special
	; both normal: b's mantissa shifted right by the exponent difference, with sticky
.L_fadd_align:
	subs ip, r3, ip
	beq .L_fadd_add
	rsb r2, ip, #32
	movs r2, r1, lsl r2
	mov r1, r1, lsr ip
	orrne r1, r1, #1
	; add mantissas; carry shifts right by one and bumps the exponent
.L_fadd_add:
	adds r0, r0, r1
	blo .L_fadd_pack_round
	and r1, r0, #1
	orr r0, r1, r0, rrx
	add r3, r3, #1
	and r2, r3, #0xff
	cmp r2, #0xff
	beq .L_fadd_overflow
	; pack: remove hidden bit, insert exponent, round to nearest even using the low 8 bits
.L_fadd_pack_round:
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
	; a has exponent 0 or 255; r2 = its sign
.L_fadd_first_special:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	ands r3, r3, #0xff
	beq .L_fadd_first_exp0
	; exponent 255: NaN if the mantissa is nonzero
	movs r0, r0, lsl #1
	bne .L_fadd_return_nan
	; Inf: the result is Inf unless b is NaN
	mov ip, r1, lsr #23
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq .L_fadd_return_inf
	cmp ip, #0xff
	blt .L_fadd_return_inf
	cmp r1, #0
	beq .L_fadd_return_inf
	b .L_fadd_return_nan
	; b has exponent 0 or 255
.L_fadd_second_special:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	and r3, r3, #0xff
	ands ip, ip, #0xff
	beq .L_fadd_second_exp0
	; b is Inf/NaN: NaN if its mantissa is nonzero else Inf
.L_fadd_second_inf_nan:
	movs r1, r1, lsl #1
	bne .L_fadd_return_nan
	b .L_fadd_return_inf
	; a is zero or denormal: a denormal uses exponent 1, no hidden bit
.L_fadd_first_exp0:
	movs r0, r0, lsl #1
	beq .L_fadd_first_zero
	mov r3, #1
	mov r0, r0, lsr #1
	; unpack b; exponent 0 or 255 goes to its cases, otherwise set the hidden bit, signs back, align
	mov ip, r1, lsr #23
	mov r1, r1, lsl #8
	ands ip, ip, #0xff
	beq .L_fadd_second_exp0
	cmp ip, #0xff
	beq .L_fadd_second_inf_nan
	orr r1, r1, #0x80000000
	orr r3, r3, r2, lsr #23
	orr ip, ip, r2, lsr #23
	b .L_fadd_align
	; a is zero: result is b
.L_fadd_first_zero:
	mov r3, r1, lsr #23
	mov r0, r1, lsl #9
	ands r3, r3, #0xff
	beq .L_fadd_return_second
	cmp r3, #0xff
	blt .L_fadd_return_second
	cmp r0, #0
	beq .L_fadd_return_inf
	b .L_fadd_second_nan
	; b is zero or denormal: a zero returns a; a denormal uses exponent 1
.L_fadd_second_exp0:
	movs r1, r1, lsl #1
	beq .L_fadd_return_first
	mov r1, r1, lsr #1
	mov ip, #1
	orr r3, r3, r2, lsr #23
	orr ip, ip, r2, lsr #23
	; a normal (hidden bit set): align; both denormal: add directly, a carry shifts right one
	cmp r0, #0
	bmi .L_fadd_align
	adds r0, r0, r1
	blo .L_fadd_pack_denormal
	and r1, r0, #1
	orr r0, r1, r0, rrx
	add ip, ip, #1
	; pack (exponent 0 unless the sum reached bit 31), round to nearest even
.L_fadd_pack_denormal:
	cmp r0, #0
	subge ip, ip, #1
	ands r1, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, ip, lsl #23
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
	; return b
.L_fadd_return_second:
	mov r0, r1
	bx lr
	; b is zero: repack a
.L_fadd_return_first:
	cmp r0, #0
	subges r3, r3, #1
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
	; exponent overflow: signed Inf
.L_fadd_overflow:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
	; NaN
.L_fadd_second_nan:
	mvn r0, #0x80000000
	bx lr
	; signed infinity
.L_fadd_return_inf:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
	; NaN
.L_fadd_return_nan:
	mvn r0, #0x80000000
	bx lr
	.size _fadd, .-_fadd
