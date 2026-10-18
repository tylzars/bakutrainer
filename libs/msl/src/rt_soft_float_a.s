; Soft-float runtime, first part: double-precision add/subtract/multiply, conversions between double and
; integer, and single-precision add. A float or int travels in r0 (second operand r1); a double or 64-bit
; integer in the pair r0 (low word) / r1 (high word), the second operand in r2/r3.
; _fadd and _fsub (in rt_soft_float_b.s) branch into each other: the entry .L_0204cb30 inside _fadd is global.
; "Not compiled" lines say why the compiler cannot have produced a routine. Save rule: the stmfd list of a
; compiled ARM function holds every register among r4-r7 it writes and, unless it is an interrupt function, also
; every one among r8-r11; lr if it calls or writes lr; and r3 when the saved registers and its stack frame would
; otherwise come to an odd number of words, or in place of a one-word frame. r0-r2 and r12 appear only in an
; interrupt function, which saves every register among r0-r7 it writes. A compiled function that saved lr
; returns by loading it into pc (a variadic one, which starts with stmfd {r0-r3}, excepted).

	.text
	.global .L_0204cb30
	.extern .L_0204d64c

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
	bhs .L_dadd_1
	adds r2, r2, ip
	adc r3, r3, lr
	subs r0, r0, ip
	sbc r1, r1, lr
	; join point for both add and subtract: r1:r0 holds the larger magnitude. Unpack it
.L_dadd_1:
	mov lr, #0x80000000
	mov ip, r1, lsr #0x14
	orr r1, lr, r1, lsl #0xb
	orr r1, r1, r0, lsr #0x15
	mov r0, r0, lsl #0xb
	; exponent of a zero or 0x7ff?
	movs r4, ip, lsl #0x15
	cmnne r4, #0x200000
	beq .L_dadd_7
	; unpack b the same way
	mov r4, r3, lsr #0x14
	orr r3, lr, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs lr, r4, lsl #0x15
	beq .L_dadd_8
	; both operands normal: ip = exponent difference; align the smaller mantissa
.L_dadd_2:
	subs r4, ip, r4
	beq .L_dadd_4
	cmp r4, #0x20
	ble .L_dadd_3
	cmp r4, #0x38
	movge r4, #0x3f
	sub r4, r4, #0x20
	rsb lr, r4, #0x20
	orrs lr, r2, r3, lsl lr
	mov r2, r3, lsr r4
	orrne r2, r2, #0x1
	adds r0, r0, r2
	adcs r1, r1, #0x0
	blo .L_dadd_6
	b .L_dadd_5
	; difference 1..32: shift the smaller mantissa right across both words, sticky into bit 0
.L_dadd_3:
	rsb lr, r4, #0x20
	movs lr, r2, lsl lr
	rsb lr, r4, #0x20
	mov r2, r2, lsr r4
	orr r2, r2, r3, lsl lr
	mov r3, r3, lsr r4
	orrne r2, r2, #0x1
	; exponents equal (or aligned): add mantissas; no carry means already normalized
.L_dadd_4:
	adds r0, r0, r2
	adcs r1, r1, r3
	blo .L_dadd_6
	; carry out: shift right one (keep sticky), exponent + 1, overflow to Inf if exponent is 0x7ff
.L_dadd_5:
	add ip, ip, #0x1
	and r4, r0, #0x1
	movs r1, r1, rrx
	orr r0, r4, r0, rrx
	mov lr, ip, lsl #0x15
	cmn lr, #0x200000
	beq .L_dadd_19
	; pack: drop 11 low bits (round bits in r2), remove hidden bit, insert exponent; round to nearest even
.L_dadd_6:
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #0x15
	add r1, r1, r1
	mov r1, r1, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #0x1
	andeqs r2, r0, #0x1
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	adds r0, r0, #0x1
	adc r1, r1, #0x0
	ldmfd sp!, {r4, lr}
	bx lr
	; first operand has exponent 0 or 0x7ff; lr = its sign
.L_dadd_7:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0x0
	bics ip, ip, #0x800
	beq .L_dadd_9
	orrs r4, r0, r1, lsl #0x1
	bne .L_dadd_17
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	beq .L_dadd_16
	cmn r4, #0x200000
	bne .L_dadd_16
	orrs r4, r2, r3, lsl #0x1
	beq .L_dadd_16
	b .L_dadd_17
	; second operand has exponent 0 or 0x7ff; lr = its sign
.L_dadd_8:
	cmp r4, #0x800
	movge lr, #0x80000000
	movlt lr, #0x0
	bic ip, ip, #0x800
	bics r4, r4, #0x800
	beq .L_dadd_11
	orrs r4, r2, r3, lsl #0x1
	bne .L_dadd_17
	b .L_dadd_16
	; first operand is a denormal (nonzero): use exponent 1, no hidden bit, then unpack the other
.L_dadd_9:
	orrs r4, r0, r1, lsl #0x1
	beq .L_dadd_10
	mov ip, #0x1
	bic r1, r1, #0x80000000
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	cmnne r4, #0x200000
	mov r4, r4, lsr #0x15
	orr r4, r4, lr, lsr #0x14
	beq .L_dadd_8
	orr r3, r3, #0x80000000
	orr ip, ip, lr, lsr #0x14
	b .L_dadd_2
	; first operand is zero: the result is the other operand (after NaN/Inf checks)
.L_dadd_10:
	mov ip, r3, lsr #0x14
	mov r1, r3, lsl #0xb
	orr r1, r1, r2, lsr #0x15
	mov r0, r2, lsl #0xb
	movs r4, ip, lsl #0x15
	beq .L_dadd_14
	cmn r4, #0x200000
	bne .L_dadd_14
	orrs r4, r0, r1, lsl #0x1
	beq .L_dadd_16
	b .L_dadd_18
	; second operand is zero or denormal
.L_dadd_11:
	orrs r4, r2, r3, lsl #0x1
	beq .L_dadd_15
	mov r4, #0x1
	bic r3, r3, #0x80000000
	cmp r1, #0x0
	bpl .L_dadd_12
	orr ip, ip, lr, lsr #0x14
	orr r4, r4, lr, lsr #0x14
	b .L_dadd_2
	; both denormal-like: add mantissas directly
.L_dadd_12:
	adds r0, r0, r2
	adcs r1, r1, r3
	blo .L_dadd_13
	add ip, ip, #0x1
	and r4, r0, #0x1
	movs r1, r1, rrx
	mov r0, r0, rrx
	orr r0, r0, r4
	; pack (exponent 0 unless the sum reached bit 63); returns when the round bits are clear or a
	; tie meets an even result. A denormal sum has no round bits, so the round-up case never
	; occurs; the code would fall through into the next block
.L_dadd_13:
	cmp r1, #0x0
	subges ip, ip, #0x1
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #0x15
	add r1, r1, r1
	orr r1, lr, r1, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #0x1
	andeqs r2, r0, #0x1
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	; return the second operand unchanged
.L_dadd_14:
	mov r1, r3
	mov r0, r2
	ldmfd sp!, {r4, lr}
	bx lr
	; second operand is zero: repack the first operand
.L_dadd_15:
	cmp r1, #0x0
	subges ip, ip, #0x1
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #0x15
	add r1, r1, r1
	orr r1, lr, r1, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	ldmfd sp!, {r4, lr}
	bx lr
	; result is a signed infinity (sign in lr)
.L_dadd_16:
	ldr r1, .L_dadd_pool_7ff00000
	orr r1, lr, r1
	mov r0, #0x0
	ldmfd sp!, {r4, lr}
	bx lr
	; NaN result: all-ones mantissa, exponent 0x7ff, sign cleared
.L_dadd_17:
	mov r1, r3
	; second entry; both give the same constant (r1 is overwritten)
.L_dadd_18:
	mvn r0, #0x0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; unreachable duplicate of the NaN return
	mvn r0, #0x0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; exponent overflow: signed infinity
.L_dadd_19:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0x0
	ldr r1, .L_dadd_pool_7ff00000
	orr r1, lr, r1
	mov r0, #0x0
	ldmfd sp!, {r4, lr}
	bx lr
.L_dadd_pool_7ff00000:
	.word 0x7ff00000
	.size _dadd, .-_dadd

; _dfix: double (r1:r0) to signed 32-bit integer, truncating toward zero.
; shift = 0x41e - biased exponent. shift <= 0 (|x| >= 2^31, Inf, NaN) saturates by sign
; to 0x7fffffff / 0x80000000; shift >= 32 (|x| < 1) gives 0; otherwise the top 32
; mantissa bits (hidden bit included) are shifted right by `shift` and negated if x < 0.
	.global _dfix
	.type _dfix, @function
_dfix:
	bic r3, r1, #0x80000000
	ldr r2, .L_dfix_pool_41e
	subs r2, r2, r3, lsr #0x14
	ble .L_dfix_2
	cmp r2, #0x20
	bge .L_dfix_1
	mov r3, r1, lsl #0xb
	orr r3, r3, #0x80000000
	orr r3, r3, r0, lsr #0x15
	cmp r1, #0x0
	mov r0, r3, lsr r2
	rsbmi r0, r0, #0x0
	bx lr
	; |x| < 1: zero
.L_dfix_1:
	mov r0, #0x0
	bx lr
	; out of range: saturate by sign
.L_dfix_2:
	mvn r0, r1, asr #0x1f
	add r0, r0, #0x80000000
	bx lr
.L_dfix_pool_41e:
	.word 0x41e
	.size _dfix, .-_dfix

; _ll_ufrom_d: double (r1:r0) to unsigned 64-bit integer (result r1:r0), truncating.
; shift = 0x43e - biased exponent. Negative inputs give 0 (a negative NaN gives all ones);
; shift < 0 saturates to all ones; shift >= 64 gives 0; otherwise the 53-bit mantissa,
; left-justified in a 64-bit value, is shifted right by `shift`.
	.global _ll_ufrom_d
	.type _ll_ufrom_d, @function
_ll_ufrom_d:
	tst r1, #0x80000000
	bne .L_ll_ufrom_d_3
	ldr r2, .L_ll_ufrom_d_pool_43e
	subs r2, r2, r1, lsr #0x14
	blt .L_ll_ufrom_d_4
	cmp r2, #0x40
	bge .L_ll_ufrom_d_2
	mov ip, r1, lsl #0xb
	orr ip, ip, #0x80000000
	orr ip, ip, r0, lsr #0x15
	cmp r2, #0x20
	ble .L_ll_ufrom_d_1
	sub r2, r2, #0x20
	mov r1, #0x0
	mov r0, ip, lsr r2
	bx lr
	; shift <= 32: two-word shift
.L_ll_ufrom_d_1:
	mov r3, r0, lsl #0xb
	mov r1, ip, lsr r2
	mov r0, r3, lsr r2
	rsb r2, r2, #0x20
	orr r0, r0, ip, lsl r2
	bx lr
	; |x| < 1: zero
.L_ll_ufrom_d_2:
	mov r1, #0x0
	mov r0, #0x0
	bx lr
	; negative input: 0 unless NaN
.L_ll_ufrom_d_3:
	cmn r1, #0x100000
	cmpeq r0, #0x0
	bhi .L_ll_ufrom_d_4
	mov r1, #0x0
	mov r0, #0x0
	bx lr
	; saturate to all ones
.L_ll_ufrom_d_4:
	mvn r1, #0x0
	mvn r0, #0x0
	bx lr
.L_ll_ufrom_d_pool_43e:
	.word 0x43e
	.size _ll_ufrom_d, .-_ll_ufrom_d

; _dflt: signed 32-bit integer (r0) to double (r1:r0).
; Takes the magnitude, returns +0 for zero, normalizes with a leading-zero count, and
; builds the exponent as 0x41e - clz. The 32-bit value always fits in the 53-bit
; mantissa, so no rounding is needed; the hidden bit is left out when packing.
; Not compiled: clz has no C expression.
	.global _dflt
	.type _dflt, @function
_dflt:
	; r2 = sign; take magnitude
	ands r2, r0, #0x80000000
	rsbmi r0, r0, #0x0
	cmp r0, #0x0
	mov r1, #0x0
	bxeq lr
	mov r3, #0x400
	add r3, r3, #0x1e
	; normalize: shift left by the leading-zero count, exponent = 0x41e - count
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	movs r1, r0
	mov r0, r1, lsl #0x15
	add r1, r1, r1
	orr r1, r2, r1, lsr #0xc
	orr r1, r1, r3, lsl #0x14
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
; Not compiled: clz has no C expression; and (save rule) it saves five words {r4-r7, lr} with no frame and
; no r3, then pops lr and returns with bx lr.
	.global _dmul
	.type _dmul, @function
_dmul:
	stmfd sp!, {r4, r5, r6, r7, lr}
	; sign of the product
	eor lr, r1, r3
	and lr, lr, #0x80000000
	mov ip, r1, lsr #0x14
	mov r1, r1, lsl #0xb
	orr r1, r1, r0, lsr #0x15
	mov r0, r0, lsl #0xb
	movs r6, ip, lsl #0x15
	cmnne r6, #0x200000
	beq .L_dmul_3
	orr r1, r1, #0x80000000
	bic ip, ip, #0x800
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	cmnne r5, #0x200000
	beq .L_dmul_5
	orr r3, r3, #0x80000000
	bic r4, r4, #0x800
	; 128-bit product of r1:r0 and r3:r2 (four 32x32 partial products); ip = ea + eb
.L_dmul_1:
	add ip, r4, ip
	umull r5, r4, r0, r2
	umull r7, r6, r0, r3
	adds r4, r7, r4
	adc r6, r6, #0x0
	umull r7, r0, r1, r2
	adds r4, r7, r4
	adcs r0, r0, r6
	umull r7, r2, r1, r3
	adc r1, r2, #0x0
	adds r0, r0, r7
	adc r1, r1, #0x0
	orrs r4, r4, r5
	orrne r0, r0, #0x1
	cmp r1, #0x0
	blt .L_dmul_2
	sub ip, ip, #0x1
	adds r0, r0, r0
	adc r1, r1, r1
	; rebias exponent; <= 0 means underflow, >= 0x7ff means overflow
.L_dmul_2:
	add ip, ip, #0x2
	subs ip, ip, #0x400
	bmi .L_dmul_12
	beq .L_dmul_12
	mov r6, ip, lsl #0x14
	cmn r6, #0x100000
	bmi .L_dmul_19
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #0x15
	add r1, r1, r1
	orr r1, lr, r1, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	movs r2, r2, lsl #0x1
	andeqs r2, r0, #0x1
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	adds r0, r0, #0x1
	adc r1, r1, #0x0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; first operand exponent is 0 or 0x7ff
.L_dmul_3:
	; special-case path: strip sign from the exponent
	bics ip, ip, #0x800
	beq .L_dmul_6
	orrs r6, r0, r1, lsl #0x1
	bne .L_dmul_15
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	beq .L_dmul_4
	cmn r5, #0x200000
	bne .L_dmul_14
	orrs r5, r2, r3, lsl #0x1
	beq .L_dmul_14
	b .L_dmul_15
	; second operand is zero/denormal: zero -> NaN (Inf * 0), else Inf
.L_dmul_4:
	orrs r5, r3, r2
	beq .L_dmul_16
	b .L_dmul_14
	; second operand exponent is 0 or 0x7ff
.L_dmul_5:
	bics r4, r4, #0x800
	beq .L_dmul_10
	orrs r6, r2, r3, lsl #0x1
	bne .L_dmul_15
	b .L_dmul_14
	; first operand is zero or denormal
.L_dmul_6:
	orrs r6, r0, r1, lsl #0x1
	beq .L_dmul_9
	mov ip, #0x1
	cmp r1, #0x0
	bne .L_dmul_7
	sub ip, ip, #0x20
	movs r1, r0
	mov r0, #0x0
	bmi .L_dmul_8
	; normalize first operand's denormal mantissa with a leading-zero count
.L_dmul_7:
	clz r6, r1
	movs r1, r1, lsl r6
	rsb r6, r6, #0x20
	orr r1, r1, r0, lsr r6
	rsb r6, r6, #0x20
	mov r0, r0, lsl r6
	sub ip, ip, r6
	; unpack second operand
.L_dmul_8:
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	cmnne r5, #0x200000
	beq .L_dmul_5
	orr r3, r3, #0x80000000
	bic r4, r4, #0x800
	b .L_dmul_1
	; first operand is zero: signed zero unless second is Inf/NaN
.L_dmul_9:
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	beq .L_dmul_20
	cmn r5, #0x200000
	bne .L_dmul_20
	orrs r6, r2, r3, lsl #0x1
	beq .L_dmul_16
	b .L_dmul_15
	; second operand is zero or denormal
.L_dmul_10:
	orrs r5, r2, r3, lsl #0x1
	beq .L_dmul_20
	mov r4, #0x1
	cmp r3, #0x0
	bne .L_dmul_11
	sub r4, r4, #0x20
	movs r3, r2
	mov r2, #0x0
	bmi .L_dmul_1
	; normalize second operand's denormal mantissa
.L_dmul_11:
	clz r6, r3
	movs r3, r3, lsl r6
	rsb r6, r6, #0x20
	orr r3, r3, r2, lsr r6
	rsb r6, r6, #0x20
	mov r2, r2, lsl r6
	sub r4, r4, r6
	b .L_dmul_1
	; result exponent <= 0: shift right into a denormal, rounding to nearest even
.L_dmul_12:
	cmn ip, #0x34
	beq .L_dmul_13
	bmi .L_dmul_18
	mov r2, r1
	mov r3, r0
	add r4, ip, #0x34
	cmp r4, #0x20
	movge r2, r3
	movge r3, #0x0
	subge r4, r4, #0x20
	rsb r5, r4, #0x20
	mov r2, r2, lsl r4
	orr r2, r2, r3, lsr r5
	movs r3, r3, lsl r4
	orrne r2, r2, #0x1
	rsb ip, ip, #0xc
	cmp ip, #0x20
	movge r0, r1
	movge r1, #0x0
	subge ip, ip, #0x20
	rsb r4, ip, #0x20
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	cmp r2, #0x0
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	movs r2, r2, lsl #0x1
	andeqs r2, r0, #0x1
	ldmeqfd sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	adds r0, r0, #0x1
	adc r1, r1, #0x0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; half the smallest denormal: round up only if any other bit is set
.L_dmul_13:
	orr r0, r0, r1, lsl #0x1
	b .L_dmul_17
	; signed infinity
.L_dmul_14:
	ldr r1, .L_dmul_pool_7ff00000
	orr r1, lr, r1
	mov r0, #0x0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; NaN (all-ones mantissa)
.L_dmul_15:
	mov r1, r3
	mvn r0, #0x0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; NaN (Inf * 0)
.L_dmul_16:
	mvn r0, #0x0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; return smallest denormal if r0 nonzero, else signed zero
.L_dmul_17:
	movs r2, r0
	mov r1, lr
	mov r0, #0x0
	addne r0, r0, #0x1
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; underflow to signed zero
.L_dmul_18:
	mov r1, lr
	mov r0, #0x0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; overflow: signed infinity
.L_dmul_19:
	ldr r1, .L_dmul_pool_7ff00000
	orr r1, lr, r1
	mov r0, #0x0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
	; signed zero
.L_dmul_20:
	mov r1, lr
	mov r0, #0x0
	ldmfd sp!, {r4, r5, r6, r7, lr}
	bx lr
.L_dmul_pool_7ff00000:
	.word 0x7ff00000
	.size _dmul, .-_dmul

; _drsb: reverse-operand entry for _dsub: swaps the two double operands with three
; exclusive-ors each and falls through into _dsub, which follows directly.
; Not compiled: has no return; execution runs on into _dsub, the next routine. Each compiled function
; is a section of its own, so none can rely on the code placed after it.
	.global _drsb
	.type _drsb, @function
_drsb:
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
; this body (.L_dsub_sub_magnitudes); a compiled function is entered only at its start. Also uses clz.
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
	bhs .L_dsub_1
	eor lr, lr, #0x80000000
	adds r2, r2, ip
	adc r3, r3, lr
	subs r0, r0, ip
	sbc r1, r1, lr
	; join point for both subtract and add: unpack the larger magnitude
.L_dsub_1:
	mov lr, #0x80000000
	mov ip, r1, lsr #0x14
	orr r1, lr, r1, lsl #0xb
	orr r1, r1, r0, lsr #0x15
	mov r0, r0, lsl #0xb
	movs r4, ip, lsl #0x15
	cmnne r4, #0x200000
	beq .L_dsub_13
	mov r4, r3, lsr #0x14
	orr r3, lr, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs lr, r4, lsl #0x15
	beq .L_dsub_14
	; both normal: align the smaller mantissa and subtract
.L_dsub_2:
	subs r4, ip, r4
	beq .L_dsub_5
	cmp r4, #0x20
	ble .L_dsub_3
	cmp r4, #0x38
	movge r4, #0x3f
	sub r4, r4, #0x20
	rsb lr, r4, #0x20
	orrs lr, r2, r3, lsl lr
	mov r2, r3, lsr r4
	orrne r2, r2, #0x1
	subs r0, r0, r2
	sbcs r1, r1, #0x0
	bmi .L_dsub_4
	b .L_dsub_10
	; difference 1..32
.L_dsub_3:
	rsb lr, r4, #0x20
	movs lr, r2, lsl lr
	rsb lr, r4, #0x20
	mov r2, r2, lsr r4
	orr r2, r2, r3, lsl lr
	mov r3, r3, lsr r4
	orrne r2, r2, #0x1
	subs r0, r0, r2
	sbcs r1, r1, r3
	bpl .L_dsub_10
	; pack: drop 11 low bits, remove hidden bit, insert exponent; round to nearest even
.L_dsub_4:
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #0x15
	add r1, r1, r1
	mov r1, r1, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	tst r2, #0x80000000
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #0x1
	andeqs r2, r0, #0x1
	ldmeqfd sp!, {r4, lr}
	bxeq lr
	adds r0, r0, #0x1
	adc r1, r1, #0x0
	ldmfd sp!, {r4, lr}
	bx lr
	; exponents equal: subtract; zero result gives +0, else renormalize
.L_dsub_5:
	subs r0, r0, r2
	sbc r1, r1, r3
	orrs lr, r1, r0
	beq .L_dsub_25
	mov lr, ip, lsl #0x14
	and lr, lr, #0x80000000
	bic ip, ip, #0x800
	cmp r1, #0x0
	bmi .L_dsub_8
	bne .L_dsub_6
	sub ip, ip, #0x20
	movs r1, r0
	mov r0, #0x0
	bmi .L_dsub_7
	; leading-zero normalization of the difference (high word nonzero)
.L_dsub_6:
	clz r4, r1
	movs r1, r1, lsl r4
	rsb r4, r4, #0x20
	orr r1, r1, r0, lsr r4
	rsb r4, r4, #0x20
	mov r0, r0, lsl r4
	sub ip, ip, r4
	; exponent > 0: pack; otherwise the result is denormal: shift right by 12 - exponent
.L_dsub_7:
	cmp ip, #0x0
	bgt .L_dsub_9
	rsb ip, ip, #0xc
	cmp ip, #0x20
	movge r0, r1
	movge r1, #0x0
	subge ip, ip, #0x20
	rsb r4, ip, #0x20
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	ldmfd sp!, {r4, lr}
	bx lr
	; top bit set: already normalized (reached only with r1 negative, so the decrement
	; never applies)
.L_dsub_8:
	cmp r1, #0x0
	subges ip, ip, #0x1
	; pack (positive exponent): remove hidden bit, insert exponent and sign
.L_dsub_9:
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #0x15
	add r1, r1, r1
	orr r1, lr, r1, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	ldmfd sp!, {r4, lr}
	bx lr
	; result needs renormalization: leading-zero count shift
.L_dsub_10:
	mov lr, ip, lsl #0x14
	and lr, lr, #0x80000000
	bic ip, ip, #0x800
	cmp r1, #0x0
	bne .L_dsub_11
	sub ip, ip, #0x20
	movs r1, r0
	mov r0, #0x0
	bmi .L_dsub_12
	; leading-zero normalization (high word nonzero)
.L_dsub_11:
	clz r4, r1
	movs r1, r1, lsl r4
	rsb r4, r4, #0x20
	orr r1, r1, r0, lsr r4
	rsb r4, r4, #0x20
	mov r0, r0, lsl r4
	sub ip, ip, r4
	; exponent > 0: pack via the common rounding tail; else denormal shift
.L_dsub_12:
	cmp ip, #0x0
	orrgt ip, ip, lr, lsr #0x14
	bgt .L_dsub_4
	rsb ip, ip, #0xc
	cmp ip, #0x20
	movge r0, r1
	movge r1, #0x0
	subge ip, ip, #0x20
	rsb r4, ip, #0x20
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	ldmfd sp!, {r4, lr}
	bx lr
	; first operand exponent 0 or 0x7ff
.L_dsub_13:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0x0
	bics ip, ip, #0x800
	beq .L_dsub_15
	orrs r4, r0, r1, lsl #0x1
	bne .L_dsub_22
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	beq .L_dsub_21
	cmn r4, #0x200000
	bne .L_dsub_21
	orrs r4, r2, r3, lsl #0x1
	beq .L_dsub_24
	b .L_dsub_22
	; second operand exponent 0 or 0x7ff
.L_dsub_14:
	cmp r4, #0x800
	movge lr, #0x80000000
	movlt lr, #0x0
	bic ip, ip, #0x800
	bics r4, r4, #0x800
	beq .L_dsub_18
	orrs r4, r2, r3, lsl #0x1
	bne .L_dsub_22
	b .L_dsub_21
	; first operand denormal or zero
.L_dsub_15:
	orrs r4, r0, r1, lsl #0x1
	beq .L_dsub_16
	mov ip, #0x1
	bic r1, r1, #0x80000000
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #0x15
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	cmnne r4, #0x200000
	mov r4, r4, lsr #0x15
	orr r4, r4, lr, lsr #0x14
	beq .L_dsub_14
	orr r3, r3, #0x80000000
	orr ip, ip, lr, lsr #0x14
	b .L_dsub_2
	; first operand is zero: a zero second operand gives +0, a finite nonzero one is returned
	; as is, Inf/NaN give +-Inf/NaN
.L_dsub_16:
	mov ip, r3, lsr #0x14
	mov r1, r3, lsl #0xb
	orr r1, r1, r2, lsr #0x15
	mov r0, r2, lsl #0xb
	movs r4, ip, lsl #0x15
	beq .L_dsub_17
	cmn r4, #0x200000
	bne .L_dsub_19
	orrs r4, r0, r1, lsl #0x1
	bne .L_dsub_23
	b .L_dsub_21
	; second operand exponent 0: zero gives +0, denormal is returned as is
.L_dsub_17:
	orrs r4, r0, r1, lsl #0x1
	beq .L_dsub_25
	b .L_dsub_19
	; second operand zero or denormal
.L_dsub_18:
	orrs r4, r2, r3, lsl #0x1
	beq .L_dsub_20
	mov r4, #0x1
	bic r3, r3, #0x80000000
	orr ip, ip, lr, lsr #0x14
	orr r4, r4, lr, lsr #0x14
	b .L_dsub_2
	; return the second operand unchanged
.L_dsub_19:
	mov r1, r3
	mov r0, r2
	ldmfd sp!, {r4, lr}
	bx lr
	; second operand is zero: repack first
.L_dsub_20:
	cmp r1, #0x0
	subges ip, ip, #0x1
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #0x15
	add r1, r1, r1
	orr r1, lr, r1, lsr #0xc
	orr r1, r1, ip, lsl #0x14
	ldmfd sp!, {r4, lr}
	bx lr
	; signed infinity
.L_dsub_21:
	ldr r1, .L_dsub_pool_7ff00000
	orr r1, lr, r1
	mov r0, #0x0
	ldmfd sp!, {r4, lr}
	bx lr
	; NaN
.L_dsub_22:
	mov r1, r3
	; NaN (all-ones mantissa)
.L_dsub_23:
	mvn r0, #0x0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; NaN (Inf - Inf)
.L_dsub_24:
	mvn r0, #0x0
	bic r1, r0, #0x80000000
	ldmfd sp!, {r4, lr}
	bx lr
	; +0
.L_dsub_25:
	mov r1, #0x0
	mov r0, #0x0
	ldmfd sp!, {r4, lr}
	bx lr
.L_dsub_pool_7ff00000:
	.word 0x7ff00000
	.size _dsub, .-_dsub

; _fadd: single-precision addition, a in r0, b in r1, result r0.
; Operands with different signs get b's sign flipped and control jumps into the
; magnitude-subtract body of _fsub (.L_0204d64c, _fsub+0xc). Otherwise
; the larger magnitude is placed in r0, both mantissas are left-justified with the hidden
; bit at bit 31, b is shifted right by the exponent difference keeping a sticky bit,
; the mantissas are added (a carry shifts right by one and bumps the exponent; exponent
; 255 gives +-Inf), and the 8 bits shifted out are rounded to nearest, ties to even.
; NaN -> 0x7fffffff, Inf + finite or Inf + Inf -> Inf, a zero operand returns the other
; one, denormal inputs are treated as exponent 1 without hidden bit.
; Not compiled: branches into the middle of _fsub (.L_0204d64c), and _fsub branches back into this body
; (.L_0204cb30); a compiled function is entered only at its start.
	.global _fadd
	.type _fadd, @function
_fadd:
	; different signs? flip b and take the subtract path
	eors r2, r0, r1
	eormi r1, r1, #0x80000000
	bmi .L_0204d64c
.L_0204cb30:
	; order by magnitude (swap when |a| < |b|)
	subs ip, r0, r1
	sublo r0, r0, ip
	addlo r1, r1, ip
	; unpack a: r3 = sign|exponent, r0 = mantissa with hidden bit at bit 31
	mov r2, #0x80000000
	mov r3, r0, lsr #0x17
	orr r0, r2, r0, lsl #0x8
	ands ip, r3, #0xff
	cmpne ip, #0xff
	beq .L_fadd_4
	mov ip, r1, lsr #0x17
	orr r1, r2, r1, lsl #0x8
	ands r2, ip, #0xff
	beq .L_fadd_5
	; both normal: b's mantissa shifted right by the exponent difference, with sticky
.L_fadd_1:
	subs ip, r3, ip
	beq .L_fadd_2
	rsb r2, ip, #0x20
	movs r2, r1, lsl r2
	mov r1, r1, lsr ip
	orrne r1, r1, #0x1
	; add mantissas; carry shifts right by one and bumps the exponent
.L_fadd_2:
	adds r0, r0, r1
	blo .L_fadd_3
	and r1, r0, #0x1
	orr r0, r1, r0, rrx
	add r3, r3, #0x1
	and r2, r3, #0xff
	cmp r2, #0xff
	beq .L_fadd_13
	; pack: remove hidden bit, insert exponent, round to nearest even using the low 8 bits
.L_fadd_3:
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
	; a has exponent 0 or 255; r2 = its sign
.L_fadd_4:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0x0
	ands r3, r3, #0xff
	beq .L_fadd_7
	movs r0, r0, lsl #0x1
	bne .L_fadd_16
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x9
	ands ip, ip, #0xff
	beq .L_fadd_15
	cmp ip, #0xff
	blt .L_fadd_15
	cmp r1, #0x0
	beq .L_fadd_15
	b .L_fadd_16
	; b has exponent 0 or 255
.L_fadd_5:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0x0
	and r3, r3, #0xff
	ands ip, ip, #0xff
	beq .L_fadd_9
	; b is Inf/NaN: NaN if its mantissa is nonzero else Inf
.L_fadd_6:
	movs r1, r1, lsl #0x1
	bne .L_fadd_16
	b .L_fadd_15
	; a is zero or denormal
.L_fadd_7:
	movs r0, r0, lsl #0x1
	beq .L_fadd_8
	mov r3, #0x1
	mov r0, r0, lsr #0x1
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #0x8
	ands ip, ip, #0xff
	beq .L_fadd_9
	cmp ip, #0xff
	beq .L_fadd_6
	orr r1, r1, #0x80000000
	orr r3, r3, r2, lsr #0x17
	orr ip, ip, r2, lsr #0x17
	b .L_fadd_1
	; a is zero: result is b
.L_fadd_8:
	mov r3, r1, lsr #0x17
	mov r0, r1, lsl #0x9
	ands r3, r3, #0xff
	beq .L_fadd_11
	cmp r3, #0xff
	blt .L_fadd_11
	cmp r0, #0x0
	beq .L_fadd_15
	b .L_fadd_14
	; b is zero or denormal
.L_fadd_9:
	movs r1, r1, lsl #0x1
	beq .L_fadd_12
	mov r1, r1, lsr #0x1
	mov ip, #0x1
	orr r3, r3, r2, lsr #0x17
	orr ip, ip, r2, lsr #0x17
	cmp r0, #0x0
	bmi .L_fadd_1
	adds r0, r0, r1
	blo .L_fadd_10
	and r1, r0, #0x1
	orr r0, r1, r0, rrx
	add ip, ip, #0x1
	; pack (exponent 0 unless the sum reached bit 31), round to nearest even
.L_fadd_10:
	cmp r0, #0x0
	subge ip, ip, #0x1
	ands r1, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #0x9
	orr r0, r0, ip, lsl #0x17
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #0x1
	addne r0, r0, #0x1
	bx lr
	; return b
.L_fadd_11:
	mov r0, r1
	bx lr
	; b is zero: repack a
.L_fadd_12:
	cmp r0, #0x0
	subges r3, r3, #0x1
	add r0, r0, r0
	orr r0, r2, r0, lsr #0x9
	orr r0, r0, r3, lsl #0x17
	bx lr
	; exponent overflow: signed Inf
.L_fadd_13:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0x0
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #0x1
	bx lr
	; NaN
.L_fadd_14:
	mvn r0, #0x80000000
	bx lr
	; signed infinity
.L_fadd_15:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #0x1
	bx lr
	; NaN
.L_fadd_16:
	mvn r0, #0x80000000
	bx lr
	.size _fadd, .-_fadd
