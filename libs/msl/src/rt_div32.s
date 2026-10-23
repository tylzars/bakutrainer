; 32-bit integer division: _s32_div_f (signed) and _u32_div_f (unsigned), quotient in r0 and remainder in r1.
; .L_u32_div_f_nonzero_divisor, the entry of _u32_div_f after its zero-divisor test, is called by _ull_mod
; (rt_soft_float_b.s).

	.text
	.global .L_u32_div_f_nonzero_divisor

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
; Not compiled: `add pc, pc, r2, lsl #2` jumps into the middle of the unrolled divide steps. The compiler
; writes pc with an add only to dispatch a switch, and every slot it can land on is a one-instruction `b` to a
; case.
	.global _s32_div_f
	.type _s32_div_f, @function
_s32_div_f:
	; ip = sign flags; make both operands positive
	eor ip, r0, r1
	and ip, ip, #0x80000000
	cmp r0, #0
	rsblt r0, r0, #0
	addlt ip, ip, #1
	cmp r1, #0
	rsblt r1, r1, #0
	; divisor zero, or dividend below the divisor: no division steps
	beq .L_s32_div_f_apply_signs
	cmp r0, r1
	movlo r1, r0
	movlo r0, #0
	blo .L_s32_div_f_apply_signs
	; r2 = number of steps to skip: 28, lowered by 16, 8 and 4 while the quotient needs that many more bits
	; (divisor at most the dividend shifted right); r3 = the dividend's top r2 bits, the starting remainder
	mov r2, #28
	mov r3, r0, lsr #4
	cmp r1, r3, lsr #12
	suble r2, r2, #16
	movle r3, r3, lsr #16
	cmp r1, r3, lsr #4
	suble r2, r2, #8
	movle r3, r3, lsr #8
	cmp r1, r3
	suble r2, r2, #4
	movle r3, r3, lsr #4
	; r0 = the dividend's other bits, left-justified, the top one moved into carry; r1 = -divisor
	mov r0, r0, lsl r2
	rsb r1, r1, #0
	adds r0, r0, r0
	; jump over r2 steps of three instructions (pc reads 8 bytes ahead: the target is r2 steps past the first)
	add r2, r2, r2, lsl #1
	add pc, pc, r2, lsl #2
	mov r0, r0			; filler slot, never executed
	; each step: r3 = (r3 << 1) + next dividend bit - divisor; add the divisor back when that borrows (carry
	; clear); shift the carry into r0 as the next quotient bit, and the next dividend bit out of it
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	mov r1, r3			; remainder
	; apply the signs to quotient and remainder
.L_s32_div_f_apply_signs:
	ands r3, ip, #0x80000000
	rsbne r0, r0, #0
	ands r3, ip, #1
	rsbne r1, r1, #0
	bx lr
	.size _s32_div_f, .-_s32_div_f

; _u32_div_f: unsigned 32-bit divide, quotient in r0 and remainder in r1. Divisor zero returns
; immediately. The part after the zero test (_u32_div_f+0x8, also called from the code _ll_udiv and
; _ull_mod share) is the same shift-count search and unrolled 32-step computed-jump divide as in
; _s32_div_f, without sign handling. If a < b the quotient is 0 and the remainder is a.
; Not compiled: _ull_mod calls .L_u32_div_f_nonzero_divisor, an entry inside this routine past its zero-divisor
; test; a compiled function is entered only at its start.
	.global _u32_div_f
	.type _u32_div_f, @function
_u32_div_f:
	cmp r1, #0
	bxeq lr				; divisor zero: return with r0 and r1 unchanged
	; entry that skips the zero-divisor test; dividend below the divisor: quotient 0, remainder the dividend
.L_u32_div_f_nonzero_divisor:
	cmp r0, r1
	movlo r1, r0
	movlo r0, #0
	bxlo lr
	; r2 = number of steps to skip: 28, lowered by 16, 8 and 4 while the quotient needs that many more bits
	; (divisor at most the dividend shifted right); r3 = the dividend's top r2 bits, the starting remainder
	mov r2, #28
	mov r3, r0, lsr #4
	cmp r1, r3, lsr #12
	suble r2, r2, #16
	movle r3, r3, lsr #16
	cmp r1, r3, lsr #4
	suble r2, r2, #8
	movle r3, r3, lsr #8
	cmp r1, r3
	suble r2, r2, #4
	movle r3, r3, lsr #4
	; r0 = the dividend's other bits, left-justified, the top one moved into carry; r1 = -divisor
	mov r0, r0, lsl r2
	rsb r1, r1, #0
	adds r0, r0, r0
	; jump over r2 steps of three instructions (pc reads 8 bytes ahead: the target is r2 steps past the first)
	add r2, r2, r2, lsl #1
	add pc, pc, r2, lsl #2
	mov r0, r0			; filler slot, never executed
	; each step: r3 = (r3 << 1) + next dividend bit - divisor; add the divisor back when that borrows (carry
	; clear); shift the carry into r0 as the next quotient bit, and the next dividend bit out of it
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	mov r1, r3			; remainder
	bx lr
	.size _u32_div_f, .-_u32_div_f
