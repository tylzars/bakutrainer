; 32-bit integer division: _s32_div_f (signed) and _u32_div_f (unsigned), quotient in r0 and remainder in r1.
; .L_0204dcf4, the entry of _u32_div_f after its zero-divisor test, is called by _ull_mod (rt_soft_float_b.s).

	.text
	.global .L_0204dcf4

; _s32_div_f: signed 32-bit divide, a / b, quotient in r0 and remainder in r1.
; The sign flags go in ip (bit 31: quotient negative; bit 0: dividend negative, so the
; remainder follows the dividend). Both operands are made positive. Divisor zero, or dividend
; smaller than divisor, skips the loop (divisor zero leaves r0 = a and r1 = 0 once the signs
; are applied; a smaller dividend gives r0 = 0 and r1 = dividend). Otherwise
; the number of significant quotient bits is found by comparing against shifted copies of the
; dividend in steps of 16, 8 and 4 bits; the dividend is shifted up by that count, and a
; computed jump (add pc, pc, r2*12) lands in an unrolled sequence of 32 three-instruction
; steps, each of which shifts the partial remainder in r3, subtracts the divisor when it fits,
; and shifts the new quotient bit into r0. The signs are then applied.
	.global _s32_div_f
	.type _s32_div_f, @function
_s32_div_f:
	eor ip, r0, r1
	and ip, ip, #0x80000000
	cmp r0, #0x0
	rsblt r0, r0, #0x0
	addlt ip, ip, #0x1
	cmp r1, #0x0
	rsblt r1, r1, #0x0
	beq .L_s32_div_f_1
	cmp r0, r1
	movlo r1, r0
	movlo r0, #0x0
	blo .L_s32_div_f_1
	mov r2, #0x1c
	mov r3, r0, lsr #0x4
	cmp r1, r3, lsr #0xc
	suble r2, r2, #0x10
	movle r3, r3, lsr #0x10
	cmp r1, r3, lsr #0x4
	suble r2, r2, #0x8
	movle r3, r3, lsr #0x8
	cmp r1, r3
	suble r2, r2, #0x4
	movle r3, r3, lsr #0x4
	mov r0, r0, lsl r2
	rsb r1, r1, #0x0
	adds r0, r0, r0
	add r2, r2, r2, lsl #0x1
	add pc, pc, r2, lsl #0x2
	mov r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	mov r1, r3
	; apply the signs to quotient and remainder
.L_s32_div_f_1:
	ands r3, ip, #0x80000000
	rsbne r0, r0, #0x0
	ands r3, ip, #0x1
	rsbne r1, r1, #0x0
	bx lr
	.size _s32_div_f, .-_s32_div_f

; _u32_div_f: unsigned 32-bit divide, quotient in r0 and remainder in r1. Divisor zero returns
; immediately. The part after the zero test (_u32_div_f+0x8, also called from the code _ll_udiv and
; _ull_mod share) is the same shift-count search and unrolled 32-step computed-jump divide as in
; _s32_div_f, without sign handling. If a < b the quotient is 0 and the remainder is a.
	.global _u32_div_f
	.type _u32_div_f, @function
_u32_div_f:
	cmp r1, #0x0
	bxeq lr
	; entry that skips the zero-divisor test
.L_0204dcf4:
	cmp r0, r1
	movlo r1, r0
	movlo r0, #0x0
	bxlo lr
	mov r2, #0x1c
	mov r3, r0, lsr #0x4
	cmp r1, r3, lsr #0xc
	suble r2, r2, #0x10
	movle r3, r3, lsr #0x10
	cmp r1, r3, lsr #0x4
	suble r2, r2, #0x8
	movle r3, r3, lsr #0x8
	cmp r1, r3
	suble r2, r2, #0x4
	movle r3, r3, lsr #0x4
	mov r0, r0, lsl r2
	rsb r1, r1, #0x0
	adds r0, r0, r0
	add r2, r2, r2, lsl #0x1
	add pc, pc, r2, lsl #0x2
	mov r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #0x1
	sublo r3, r3, r1
	adcs r0, r0, r0
	mov r1, r3
	bx lr
	.size _u32_div_f, .-_u32_div_f
