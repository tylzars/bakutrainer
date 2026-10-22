; Hand-written routines of the game's engine, with two sample packed blocks and the font of the error screen:
;   - an error screen on the sub display (Thumb): OpenTextScreen shows a message, PrintTextScreenRow prints a line;
;   - unpackers: a byte run-length format (UnpackRle, UnpackRlePart) and three packed formats with a
;     four-byte tag, "EZ00" (Huffman-coded runs), "EZ01" and "EZ10" (two bit-flag LZ formats), UnpackEz;
;   - the JPEG block decoder (func_02028980) and the colour conversion of a 16x16 block (func_02028b8c);
;   - small helpers: leading-zero count, palette row remapping, 4bpp to 8bpp tile conversion.
; Many routines pass values in registers and do not follow the C calling convention; each one says what it
; takes and returns. The C-callable ones are func_02027fb8, OpenTextScreen, ScrollAndPrintTextScreen,
; PrintTextScreenRow, UnpackRle, UnpackRlePart, LeadingZeroCount, func_02028258, GetEzUnpackedSize, UnpackEz,
; UnpackEz10, RemapPaletteRows, func_02028980, func_02028b8c, func_02028d48 and func_02028df4. Most of them save and
; restore every register they use, the argument registers included.
; The bytes of the packed samples and of the font are read from the ROM at build time.
; Each routine's "Not compiled" lines say why the compiler cannot have produced it. "Save rule": in a compiled
; ARM function the stmfd/ldmfd register list holds the callee-saved registers the function writes, r3 as a
; stack pad, the frame pointer and lr; r0-r3 and r12 appear only in functions with the compiler's special entry
; flag (the one that also makes it return with ldm ^), which then also save every register they write. Compiled
; functions each start their own 4-byte-aligned section with their own literal pool.

	.extern data_020c1818
	.extern data_020c1820
	.extern data_020c1824

	.text

; An "EZ10" packed block (unused by the game).
	.global data_02027398
	.type data_02027398, @object
data_02027398:
	.incbin "extract/usa/arm9/arm9.bin", 0x27398, 0x78
	.size data_02027398, 120

; An "EZ00" packed block (unused by the game).
	.global data_02027410
	.type data_02027410, @object
data_02027410:
	.incbin "extract/usa/arm9/arm9.bin", 0x27410, 0x68
	.size data_02027410, 104

; Font of the error screen: 90 tiles of 8x8 pixels at 4 bits per pixel, the characters '!' to 'z'.
	.global gTextScreenFont
	.type gTextScreenFont, @object
gTextScreenFont:
	.incbin "extract/usa/arm9/arm9.bin", 0x27478, 0xb40
	.size gTextScreenFont, 2880

; ---- Error screen (Thumb) ----
; The sub display shows text background 0: map at 0x06200000 (32 halfwords per row, tile = character - 32),
; tiles at 0x06204000 (tile 0 blank), VRAM bank H mapped there. Rows 0-3 stay put; rows 4-23 scroll.

	.thumb

; func_02027fb8: unmaps VRAM banks H and I (writes 0 to both control bytes). Keeps every register.
; Not compiled: loads .Lvramcnt_h and .Lzero from the pool after PrintTextScreenRow, which OpenTextScreen,
; ScrollTextScreen and PrintTextScreenRow share.
	.global func_02027fb8
	.type func_02027fb8, @function
func_02027fb8:
	push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
	ldr r0, .Lvramcnt_h
	ldr r1, .Lzero
	strh r1, [r0, #0]
	pop {r0, r1, r2, r3, r4, r5, r6, r7, pc}
	.size func_02027fb8, .-func_02027fb8

; OpenTextScreen: sets up the error screen and prints the message in r0 on row 0. Sub display: brightness
; 0, mode 0 with background 0 only, background 0 using tile base 1; bank H to the sub backgrounds; clears
; the 32 KB of map and tiles; palette colour 0 black, colour 1 white; copies the font to tile 1 onwards.
; Keeps every register.
; Not compiled: starts at a halfword address, and loads 15 words of the pool after PrintTextScreenRow (shared
; with func_02027fb8, ScrollTextScreen and PrintTextScreenRow).
	.global OpenTextScreen
	.type OpenTextScreen, @function
OpenTextScreen:
	push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
	mov r7, r0			; message
	ldr r0, .Lmaster_bright_sub
	mov r1, #0
	strh r1, [r0, #0]
	ldr r0, .Ldispcnt_sub
	ldr r1, .Ldispcnt_text
	str r1, [r0, #0]
	ldr r0, .Lbg0cnt_sub
	ldr r1, .Lbg0cnt_text
	str r1, [r0, #0]
	ldr r0, .Lvramcnt_h
	ldr r1, .Lvram_h_sub_bg		; bank H on, bank I off
	strh r1, [r0, #0]
	ldr r6, .Lscreen_map
	mov r3, #0
	ldr r2, .Lclear_size
.Lclear:
	str r3, [r6, #0]
	add r6, #4
	sub r2, #4
	bne .Lclear
	ldr r0, .Lsub_bg_palette
	ldr r1, .Lzero
	strh r1, [r0, #0]
	ldr r1, .Lwhite
	strh r1, [r0, #2]
	ldr r6, .Lfont_tiles
	ldr r5, .Lfont
	ldr r4, .Lfont_size
.Lcopy_font:
	ldr r0, [r5, #0]
	add r5, #4
	str r0, [r6, #0]
	add r6, #4
	sub r4, #4
	bne .Lcopy_font
	mov r0, r7
	mov r1, #0
	bl PrintTextScreenRow
	pop {r0, r1, r2, r3, r4, r5, r6, r7, pc}
	.size OpenTextScreen, .-OpenTextScreen

; ScrollAndPrintTextScreen: scrolls the error screen up one row and prints the message in r0 on the last row (23).
; Keeps every register (both routines it calls keep theirs, r0 included).
; Not compiled: starts at a halfword address, and passes its incoming r0 to PrintTextScreenRow after a call to
; ScrollTextScreen without saving it (r0 is not preserved across a call under the C calling convention).
	.global ScrollAndPrintTextScreen
	.type ScrollAndPrintTextScreen, @function
ScrollAndPrintTextScreen:
	push {r1, lr}
	bl ScrollTextScreen
	mov r1, #23
	bl PrintTextScreenRow
	pop {r1, pc}
	.size ScrollAndPrintTextScreen, .-ScrollAndPrintTextScreen

; ScrollTextScreen: scrolls rows 5-24 of the map up into rows 4-23. Keeps every register.
; Not compiled: loads .Lmap_row4, .Lmap_row5 and .Lscroll_size from the pool after PrintTextScreenRow, which
; func_02027fb8, OpenTextScreen and PrintTextScreenRow share.
	.global ScrollTextScreen
	.type ScrollTextScreen, @function
ScrollTextScreen:
	push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
	ldr r6, .Lmap_row4
	ldr r7, .Lmap_row5
	ldr r5, .Lscroll_size
.Lscroll:
	ldr r1, [r7, #0]
	str r1, [r6, #0]
	add r7, r7, #4
	add r6, r6, #4
	sub r5, r5, #4
	bne .Lscroll
	pop {r0, r1, r2, r3, r4, r5, r6, r7, pc}
	.size ScrollTextScreen, .-ScrollTextScreen

; PrintTextScreenRow: prints the message in r0 on row r1, if the error screen is set up (sub display control
; as OpenTextScreen leaves it). A row takes up to 32 characters; character 0 or '\n' blanks the rest of the
; row, character 2 stops the program here (endless loop). After the row a '\n' is skipped; at the end of the
; message the routine returns. Otherwise, with the row past the last one (24) the screen scrolls and the row
; moves back up; the routine then goes back to the end-of-row check, not to the printing loop, so it only
; returns once the message ends. Keeps every register.
; Not compiled: starts at a halfword address, and its literal pool is also loaded by func_02027fb8,
; OpenTextScreen and ScrollTextScreen.
	.global PrintTextScreenRow
	.type PrintTextScreenRow, @function
PrintTextScreenRow:
	push {r0, r1, r2, r3, r4, r5, r6, r7, lr}
	ldr r7, .Ldispcnt_sub
	ldr r7, [r7, #0]
	ldr r6, .Ldispcnt_text
	cmp r7, r6
	bne .Lprint_done
	mov r7, r0			; character pointer
	ldr r6, .Lscreen_map
	lsl r1, r1, #6
	add r6, r6, r1			; map entry of the row's first column
	mov r2, #32			; columns left
.Lprint_char:
	ldrb r0, [r7, #0]
	cmp r0, #0
	beq .Lblank_rest
	cmp r0, #2
.Lhalt:
	beq .Lhalt
	cmp r0, #10
	beq .Lblank_rest
	sub r0, #32			; tile of the character
	strh r0, [r6, #0]
	add r6, #2
	add r7, #1
	sub r2, #1
	bne .Lprint_char
	b .Lend_of_row
.Lblank_rest:
	mov r0, #0
.Lblank_loop:
	strh r0, [r6, #0]
	add r6, #2
	sub r2, #1
	bne .Lblank_loop
.Lend_of_row:
	ldrb r0, [r7, #0]
	cmp r0, #10
	bne .Lnot_newline
	add r7, #1
.Lnot_newline:
	cmp r0, #0
	beq .Lprint_done
	ldr r0, .Lmap_row24
	cmp r0, r6
	bne .Lnext_row
	bl ScrollTextScreen
	sub r6, #64
.Lnext_row:
	b .Lend_of_row
.Lprint_done:
	pop {r0, r1, r2, r3, r4, r5, r6, r7, pc}
	lsl r0, r0, #0			; zero halfword before the pool (not reached)
	.align 4
.Lvramcnt_h:
	.word 0x04000248		; VRAM bank H and I control
.Lzero:
	.word 0
.Lmaster_bright_sub:
	.word 0x0400106c
.Ldispcnt_sub:
	.word 0x04001000
.Ldispcnt_text:
	.word 0x00010100		; display on, mode 0, background 0 only
.Lbg0cnt_sub:
	.word 0x04001008
.Lbg0cnt_text:
	.word 4				; tile base 1, map base 0, 16 colours, 32x32
.Lvram_h_sub_bg:
	.word 0x81			; bank H on, sub display backgrounds
.Lscreen_map:
	.word 0x06200000
.Lclear_size:
	.word 0x8000
.Lsub_bg_palette:
	.word 0x05000400
.Lwhite:
	.word 0xffff
.Lfont_tiles:
	.word 0x06204020		; tile 1
.Lfont:
	.word gTextScreenFont
.Lfont_size:
	.word 0xb40
.Lmap_row4:
	.word 0x06200100
.Lmap_row5:
	.word 0x06200140
.Lscroll_size:
	.word 0x500			; 20 rows
.Lmap_row24:
	.word 0x06200600
	.size PrintTextScreenRow, .-PrintTextScreenRow

; ---- Run-length format ----
; Each control byte c is followed by its data: c & 0x80: c & 0x7f literal bytes; else c & 0x40:
; c & 0x3f zero bytes; else one byte, repeated c & 0x3f times.

	.arm

; UnpackRle: unpacks r2 bytes of run-length data at r0 to r1 (a control byte of 0x40 is skipped).
; Returns the number of bytes written. Keeps r1-r6.
; Not compiled (save rule): saves and restores r1 and r2, yet writes r0 (mov r0, r1) without saving it;
; the result is also computed from the r1 the ldmfd restores.
	.global UnpackRle
	.type UnpackRle, @function
UnpackRle:
	stmfd sp!, {r1, r2, r3, r4, r5, r6}
	add r2, r0, r2			; end of the input
	mov r3, #0
.Lrle_next:
	ldrb r4, [r0], #1
	cmp r4, #64
	beq .Lrle_check_end
	movs r5, r4, lsl #25		; carry = bit 7
	bcc .Lrle_fill
	mov r4, r5, lsr #25
.Lrle_copy:
	ldrb r6, [r0], #1
	strb r6, [r1], #1
	subs r4, r4, #1
	bgt .Lrle_copy
	cmp r2, r0
	bgt .Lrle_next
	b .Lrle_done
.Lrle_fill:
	movs r5, r5, lsl #1		; carry = bit 6: zeros
	movcs r6, r3
	ldrccb r6, [r0], #1
	mov r4, r5, lsr #26
.Lrle_fill_loop:
	strb r6, [r1], #1
	subs r4, r4, #1
	bgt .Lrle_fill_loop
.Lrle_check_end:
	cmp r2, r0
	bgt .Lrle_next
.Lrle_done:
	mov r0, r1
	ldmfd sp!, {r1, r2, r3, r4, r5, r6}
	sub r0, r0, r1
	bx lr
	.size UnpackRle, .-UnpackRle

; UnpackRlePart: unpacks run-length data at r0: skips the first r2 bytes of the output, then writes r3 bytes
; to r1. A run that crosses the start is cut there. Fills of more than 12 bytes are stored by halfwords and
; words once r1 is aligned; the last word may write up to three bytes past the run, which the next run
; overwrites (r1 moves back to the run's end). Keeps r1-r6.
; Not compiled (save rule): saves and restores r1-r3, yet writes r0 (add r0, ...; sub r0, ...) without
; saving it.
	.global UnpackRlePart
	.type UnpackRlePart, @function
UnpackRlePart:
	stmfd sp!, {r1, r2, r3, r4, r5, r6}
	movs r4, r2			; bytes to skip
	beq .Lrlep_next
.Lrlep_skip:
	ldrb r2, [r0], #1
	movs r5, r2, lsl #25
	bcc .Lrlep_skip_fill
	add r0, r0, r5, lsr #25		; skip the literal bytes
	subs r4, r4, r5, lsr #25
	bgt .Lrlep_skip
	beq .Lrlep_next
	rsb r4, r4, #0			; bytes of the run left to copy
	sub r0, r0, r4
	mov r5, r4, lsl #25
	b .Lrlep_copy
.Lrlep_skip_fill:
	movs r5, r5, lsl #1
	movcs r6, #0
	ldrccb r6, [r0], #1
	subs r4, r4, r5, lsr #26
	bgt .Lrlep_skip
	beq .Lrlep_next
	rsb r4, r4, #0			; bytes of the fill left to write
	mov r5, r4, lsl #26
	b .Lrlep_fill_bytes
.Lrlep_next:
	ldrb r4, [r0], #1
	movs r5, r4, lsl #25
	bcc .Lrlep_fill
	mov r4, r5, lsr #25
.Lrlep_copy:
	ldrb r6, [r0], #1
	subs r4, r4, #1
	strb r6, [r1], #1
	bgt .Lrlep_copy
	subs r3, r3, r5, lsr #25
	bgt .Lrlep_next
	b .Lrlep_done
.Lrlep_fill:
	movs r5, r5, lsl #1
	movcs r6, #0
	ldrccb r6, [r0], #1
	mov r4, r5, lsr #26
	cmp r4, #12
	bls .Lrlep_fill_bytes
	orr r6, r6, r6, lsl #8		; the byte in all four bytes of a word
	orr r6, r6, r6, lsl #16
	tst r1, #3
	beq .Lrlep_fill_words
	tst r1, #1
	beq .Lrlep_fill_half
	strb r6, [r1], #1
	sub r4, r4, #1
	tst r1, #3
	beq .Lrlep_fill_words
.Lrlep_fill_half:
	strh r6, [r1], #2
	sub r4, r4, #2
.Lrlep_fill_words:
	subs r4, r4, #4
	str r6, [r1], #4
	bgt .Lrlep_fill_words
	b .Lrlep_fill_end
.Lrlep_fill_bytes:
	subs r4, r4, #1
	strb r6, [r1], #1
	bgt .Lrlep_fill_bytes
.Lrlep_fill_end:
	addne r1, r1, r4		; back to the end of the run
	subs r3, r3, r5, lsr #26
	bgt .Lrlep_next
.Lrlep_done:
	ldmfd sp!, {r1, r2, r3, r4, r5, r6}
	bx lr
	.size UnpackRlePart, .-UnpackRlePart

; LeadingZeroCount: returns the number of leading zero bits of r0 (32 for 0).
; Not compiled: clz has no C form in this compiler.
	.global LeadingZeroCount
	.type LeadingZeroCount, @function
LeadingZeroCount:
	clz r0, r0
	bx lr
	.size LeadingZeroCount, .-LeadingZeroCount

; func_02028258: stores the instruction at .Lpatch_add ("add r0, r0, #1") over the instruction right after
; the store, then runs that slot: returns 0 if the instruction already fetched ran, 1 if the stored one did.
; Uses r1.
; Not compiled: stores into its own code, over the instruction right after the store (str r1, [pc, #-4]).
	.global func_02028258
	.type func_02028258, @function
func_02028258:
	mov r0, #0
	ldr r1, .Lpatch_add
	str r1, [pc, #-4]		; over the next instruction
	add r0, r0, #0
	bx lr
	bx lr				; (not reached)
	.size func_02028258, .-func_02028258

; A credit line of the engine, ROT13-encoded.
	.global data_02028270
	.type data_02028270, @object
data_02028270:
	.asciz "Zntvp Cbpxrgf Ratvar, pbqrq ol Revp MZVEB"
	.align 4
	.size data_02028270, 44

; The instruction func_02028258 stores.
	.global data_0202829c
	.type data_0202829c, @object
data_0202829c:
.Lpatch_add:
	.word 0xe2800001		; add r0, r0, #1
	.size data_0202829c, 4

; ---- Packed formats with a tag ----
; Header: +0 tag "EZ00", "EZ01" or "EZ10", +4 unpacked size.

; GetEzUnpackedSize: returns the unpacked size of the packed block at r0, or 0 if it has none of the three
; tags. Uses r2; keeps r1.
; Not compiled: its pool words (the three tags) are loaded pc-relative by UnpackEz, so both share one
; section; also saves only r1 (save rule) while writing r0 and r2.
	.global GetEzUnpackedSize
	.type GetEzUnpackedSize, @function
GetEzUnpackedSize:
	stmfd sp!, {r1}
	ldr r1, [r0]
	ldr r2, .Ltag_ez00
	cmp r1, r2
	beq .Lsize_known
	ldr r2, .Ltag_ez01
	cmp r1, r2
	beq .Lsize_known
	ldr r2, .Ltag_ez10
	cmp r1, r2
	beq .Lsize_known
	mov r0, #0
	b .Lsize_done
.Lsize_known:
	ldr r0, [r0, #4]
.Lsize_done:
	ldmfd sp!, {r1}
	bx lr
.Ltag_ez00:
	.word 0x30305a45		; "EZ00"
.Ltag_ez01:
	.word 0x31305a45		; "EZ01"
.Ltag_ez10:
	.word 0x30315a45		; "EZ10"
	.size GetEzUnpackedSize, .-GetEzUnpackedSize

; UnpackEz: unpacks the packed block at r0 to r1 by its tag: "EZ00" here, "EZ01" in func_02028578
; (a branch; it returns for this routine), "EZ10" in the body of UnpackEz10. Returns 0 for an unknown
; tag, the end of the byte area for "EZ00". Keeps r1-r12.
; "EZ00": +8 offset of the byte area from +16, +12 number of sections (byte), +16 a stream of 16-bit words
; read from bit 0 up (func_02028558), then the bytes. The stream starts with 2 unused bits. Each section
; holds three code tables (func_02028480; literal run lengths, match distances, match lengths), a 16-bit
; count of literal runs, then: literal run, match, literal run, ..., literal run. A literal run copies its
; length of bytes from the byte area; a match copies length + 2 bytes from distance + 1 bytes back.
; Bit reader (here and in func_02028408, func_02028480, func_02028558): r10 = bit buffer, r9 = number of
; valid bits - 16, r12 = current input halfword, r6 = all ones.
; Not compiled: loads GetEzUnpackedSize's pool words pc-relative (backwards); branches (not calls) into
; func_02028578 and into UnpackEz10 after its two stmfd instructions; passes the bit reader to
; func_02028408, func_02028480 and func_02028558 in r6, r9, r10, r12 and the table pointer in r8.
	.global UnpackEz
	.type UnpackEz, @function
UnpackEz:
	stmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	stmfd sp!, {lr}
	mov r12, r0
	mov r11, r1			; output
	ldr r0, [r12], #4
	ldr r1, .Ltag_ez00
	cmp r0, r1
	beq .Lez00
	ldr r1, .Ltag_ez01
	cmp r0, r1
	beq func_02028578
	ldr r1, .Ltag_ez10
	cmp r0, r1
	subeq r0, r12, #4
	moveq r1, r11
	beq .Lez10_body
	mov r0, #0
	ldmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}
	bx lr
.Lez00:
	ldr r10, [r12], #4		; unpacked size (not used)
	ldr r7, [r12], #4
	ldrb r0, [r12], #4
	mov r5, r0			; sections
	ldrh r10, [r12]			; first input halfword
	add r7, r12, r7			; byte area
	mov r9, #0
	mov r1, #2
	mvn r6, #0
	bl func_02028558		; the 2 unused bits
	sub sp, sp, #384		; three code tables of 128 bytes
.Lez00_section:
	add r8, sp, #0
	bl func_02028480
	add r8, sp, #128
	bl func_02028480
	add r8, sp, #256
	bl func_02028480
	mov r1, #16
	bl func_02028558
	mov r4, r0			; literal runs
	b .Lez00_literals
.Lez00_match:
	add r8, sp, #128
	bl func_02028408
	mov r3, r0
	add r8, sp, #256
	bl func_02028408
	add r3, r3, #1			; distance
	add r0, r0, #2			; length
.Lez00_match_copy:
	ldrb r2, [r11, -r3]
	ldrb r1, [r11]			; (unused)
	subs r0, r0, #1
	strb r2, [r11], #1
	bne .Lez00_match_copy
.Lez00_literals:
	add r8, sp, #0
	bl func_02028408
	cmp r0, #0
	beq .Lez00_run_done
.Lez00_literal_copy:
	ldrb r1, [r7], #1
	subs r0, r0, #1
	strb r1, [r11], #1
	bne .Lez00_literal_copy
	ldrh r1, [r12]
	orr r10, r10, r1, lsl r9	; the current input halfword into the bit buffer again
.Lez00_run_done:
	subs r4, r4, #1
	bne .Lez00_match
	subs r5, r5, #1
	bne .Lez00_section
	add sp, sp, #384
	mov r0, r7
	ldmfd sp!, {lr}
	ldmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	bx lr
	.size UnpackEz, .-UnpackEz

; func_02028408: reads one value with the code table at r8 (built by func_02028480). Finds the entry whose
; code matches the next bits, takes the code's bits, then for symbol 0 or 1 returns it; for symbol s >= 2
; reads s - 1 more bits x and returns (1 << (s - 1)) | x. Takes the bit reader of UnpackEz; uses
; r1, r2 and r8.
; Not compiled: takes its inputs in r8 (table) and r6, r9, r10, r12 (bit reader) and returns r8, r9, r10
; and r12 updated; no C caller sets or reads callee-saved registers as arguments or results.
	.global func_02028408
	.type func_02028408, @function
func_02028408:
	and r2, r10, r6, lsr #16	; next 16 bits
.Lcode_find:
	ldr r0, [r8], #4		; mask | code << 16
	and r1, r0, r2
	cmp r1, r0, lsr #16
	bne .Lcode_find
	ldrb r1, [r8, #61]		; code length
	mov r10, r10, lsr r1
	subs r9, r9, r1
	bmi .Lcode_refill
.Lcode_symbol:
	ldrb r1, [r8, #60]		; symbol
	cmp r1, #2
	bcc .Lcode_small
	sub r2, r1, #1
	bic r0, r10, r6, lsl r2
	mov r10, r10, lsr r2
	subs r9, r9, r2
	bmi .Lcode_refill_extra
.Lcode_extra_done:
	mov r1, #1
	orr r0, r0, r1, lsl r2
	bx lr
.Lcode_small:
	mov r0, r1
	bx lr
.Lcode_refill:
	ldrh r1, [r12, #2]!
	add r9, r9, #16
	orr r10, r10, r1, lsl r9
	b .Lcode_symbol
.Lcode_refill_extra:
	ldrh r1, [r12, #2]!
	add r9, r9, #16
	orr r10, r10, r1, lsl r9
	b .Lcode_extra_done
	.size func_02028408, .-func_02028408

; func_02028480: reads a code table into the 128 bytes at r8: a 5-bit count n (0: the table stays empty),
; then n 4-bit code lengths, one per symbol 0..n-1 (0: no code). Codes are given out in canonical order
; (by length, then by symbol) and stored bit-reversed, since the stream is read from bit 0. Entry k:
; +4k mask (halfword), +4k+2 code, +4k+64 symbol (byte), +4k+65 length. Takes the bit reader of
; UnpackEz; advances r8, uses r0-r4.
; Not compiled: takes r8 and the bit reader in r6, r9, r10, r12 as inputs; moves sp inside its loops
; (stmfd/ldmfd) and reaches its locals relative to the moved sp ([sp, #20]).
	.global func_02028480
	.type func_02028480, @function
func_02028480:
	stmfd sp!, {r5, r6, lr}
	sub sp, sp, #16			; the code lengths
	mov r1, #5
	bl func_02028558
	movs r2, r0
	beq .Ltable_done
	mov r4, sp
	mov r3, r2
.Ltable_read_length:
	mov r1, #4
	bl func_02028558
	strb r0, [r4], #1
	subs r2, r2, #1
	bne .Ltable_read_length
	mov r2, r3
	mov r5, sp
	mov r0, #1			; length
	mov r1, #0			; next code, left-aligned in 16 bits
	mov r3, #32768			; code step of this length
.Ltable_length:
	stmfd sp!, {r2, r5}
.Ltable_symbol:
	ldrb r6, [r5]
	cmp r0, r6
	bne .Ltable_symbol_next
	stmfd sp!, {r0, r1, r2}
	mvn r2, #0
	bic r2, r2, r2, lsl r0
	strh r2, [r8], #2		; mask
	rsb r6, r6, #16
	mov r1, r1, lsr r6
	mov r2, r0
	mov r0, #0
.Ltable_reverse:
	movs r1, r1, lsr #1
	mov r0, r0, lsl #1
	orrcs r0, r0, #1
	subs r2, r2, #1
	bne .Ltable_reverse
	strh r0, [r8], #2		; code
	add r6, sp, #20			; first code length
	sub r0, r5, r6			; symbol
	ldrb r6, [r5]
	orr r0, r0, r6, lsl #8
	strh r0, [r8, #60]
	ldmfd sp!, {r0, r1, r2}
	add r1, r1, r3
.Ltable_symbol_next:
	add r5, r5, #1
	subs r2, r2, #1
	bne .Ltable_symbol
	ldmfd sp!, {r2, r5}
	mov r3, r3, lsr #1
	add r0, r0, #1
	cmp r0, #17
	bne .Ltable_length
.Ltable_done:
	add sp, sp, #16
	ldmfd sp!, {r5, r6, lr}
	bx lr
	.size func_02028480, .-func_02028480

; func_02028558: returns the next r1 bits of the bit reader of UnpackEz in r0. Uses r1.
; Not compiled: the bit reader is in r6, r9, r10, r12 (inputs, and r9, r10, r12 updated on return).
	.global func_02028558
	.type func_02028558, @function
func_02028558:
	bic r0, r10, r6, lsl r1
	mov r10, r10, lsr r1
	subs r9, r9, r1
	bxpl lr
	ldrh r1, [r12, #2]!
	add r9, r9, #16
	orr r10, r10, r1, lsl r9
	bx lr
	.size func_02028558, .-func_02028558

; func_02028578: unpacks an "EZ01" block for UnpackEz (reached by a branch with r12 = block + 4,
; r11 = output, and returns for it). +8 offset of the byte area from the block, +12 the flag words.
; Flags are read from bit 31 down (r0 holds a marker bit below the flags left; func_02028738 refills it);
; the first two bits of the first word are skipped. Flag 0: the next byte of the byte area. Flag 1: a match:
;   1 0 ...   length 4..8 from two or three more flags (9 instead: 4 more flags n, then 4 * (n + 3) bytes
;             taken as they are);
;   1 1 0     length 2, distance from one byte;
;   1 1 1 0   length 3;  1 1 1 1: length = next byte + 8, a 0 byte ends the data unless the next flag is 1.
; A match's distance is (h << 8 | next byte) + 1; h (0..15) comes from a few flags read before the byte
; (none after 1 1 0).
; r2 = flag words, r3 = byte area, r4 = h, r5 = length.
; Not compiled: entered by a branch from UnpackEz (no call), works on the r11/r12 set there and returns
; by popping the registers UnpackEz saved.
	.global func_02028578
	.type func_02028578, @function
func_02028578:
	sub r12, r12, #4
	add r2, r12, #12
	ldr r3, [r12, #8]
	add r3, r3, r12
	ldr r0, [r2], #4
	add r0, r0, r0
	add r0, r0, #1			; marker
	adds r0, r0, r0
	b .Lez01_flag
.Lez01_block:
	adds r0, r0, r0
	bleq func_02028738
	adc r4, r4, r4
	adds r0, r0, r0
	bleq func_02028738
	adc r4, r4, r4
	adds r0, r0, r0
	bleq func_02028738
	adc r4, r4, r4
	adds r0, r0, r0
	bleq func_02028738
	adc r4, r4, r4
	add r4, r4, #3
.Lez01_block_copy:
	ldrb r5, [r3], #1
	strb r5, [r11], #1
	ldrb r5, [r3], #1
	strb r5, [r11], #1
	ldrb r5, [r3], #1
	strb r5, [r11], #1
	ldrb r5, [r3], #1
	strb r5, [r11], #1
	subs r4, r4, #1
	bne .Lez01_block_copy
	b .Lez01_flag
.Lez01_long:
	adds r0, r0, r0
	bleq func_02028738
	adc r5, r5, r5
	adds r0, r0, r0
	bleq func_02028738
	bcc .Lez01_distance
	adds r0, r0, r0
	bleq func_02028738
	sub r5, r5, #1
	adc r5, r5, r5
	cmp r5, #9
	beq .Lez01_block
.Lez01_distance:
	adds r0, r0, r0
	bleq func_02028738
	bcc .Lez01_match
	adds r0, r0, r0
	bleq func_02028738
	adc r4, r4, r4
	adds r0, r0, r0
	bleq func_02028738
	bcc .Lez01_distance_short
	adds r0, r0, r0
	bleq func_02028738
	adc r4, r4, r4
	orr r4, r4, #4
	adds r0, r0, r0
	bleq func_02028738
	bcs .Lez01_match
	b .Lez01_distance_bit
.Lez01_distance_short:
	cmp r4, #0
	bne .Lez01_match
	add r4, r4, #1
.Lez01_distance_bit:
	adds r0, r0, r0
	bleq func_02028738
	adc r4, r4, r4
.Lez01_match:
	ldrb r6, [r3], #1
	orr r6, r6, r4, lsl #8
	mvn r6, r6			; -distance
.Lez01_match_copy:
	ldrb r4, [r11]			; (unused)
	ldrb r4, [r11, r6]
	strb r4, [r11], #1
	subs r5, r5, #1
	bne .Lez01_match_copy
	b .Lez01_flag
.Lez01_literal:
	ldrb r5, [r3], #1
	strb r5, [r11], #1
.Lez01_flag:
	adds r0, r0, r0
	bcs .Lez01_flag_set
	ldrb r5, [r3], #1
	strb r5, [r11], #1
	adds r0, r0, r0
	bcc .Lez01_literal
.Lez01_flag_set:
	bne .Lez01_match_start		; a flag, not the marker
	ldr r0, [r2], #4
	adcs r0, r0, r0
	bcc .Lez01_literal
.Lez01_match_start:
	mov r4, #0
	mov r5, #2
	adds r0, r0, r0
	bleq func_02028738
	bcc .Lez01_long
	adds r0, r0, r0
	bleq func_02028738
	bcc .Lez01_match
	add r5, r5, #1
	adds r0, r0, r0
	bleq func_02028738
	bcc .Lez01_distance
	ldrb r5, [r3], #1
	cmp r5, #0
	beq .Lez01_end
	add r5, r5, #8
	b .Lez01_distance
.Lez01_end:
	adds r0, r0, r0
	bleq func_02028738
	bcs .Lez01_flag
	ldmfd sp!, {lr}
	ldmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	bx lr
	.size func_02028578, .-func_02028578

; func_02028738: refills the flag word of func_02028578 (r0) from r2 and returns its first flag in the carry
; (called when r0 has run empty, so the carry in is the marker bit).
; Not compiled: takes r2 and the carry flag, and returns r0, r2 and the carry flag; flags are not part of
; any C call.
	.global func_02028738
	.type func_02028738, @function
func_02028738:
	ldr r0, [r2], #4
	adcs r0, r0, r0
	bx lr
	.size func_02028738, .-func_02028738

; func_02028744: refills the byte word of UnpackEz10: r9 = next word at r0, r8 = 3 bytes left after the
; first.
; Not compiled: returns its results in r8 and r9 (callee-saved registers) and advances r0.
	.global func_02028744
	.type func_02028744, @function
func_02028744:
	mov r8, #3
	ldr r9, [r0], #4
	bx lr
	.size func_02028744, .-func_02028744

; func_02028750: refills the flag word of UnpackEz10 (r10) from r0 and returns its first flag in the
; carry.
; Not compiled: returns its result in r10 and the carry flag, and advances r0.
	.global func_02028750
	.type func_02028750, @function
func_02028750:
	ldr r10, [r0], #4
	adcs r10, r10, r10
	bx lr
	.size func_02028750, .-func_02028750

; UnpackEz10: unpacks the "EZ10" block at r0 to r1. Data from +16: words, each used either as 32 flags
; (bit 31 first, in r10 with a marker bit) or as 4 bytes (lowest first, in r9), as the unpacker needs them.
; Unpacks until r1 reaches the output start + the unpacked size. Codes (flags, then values):
;   1 0 0          one literal byte;
;   0 0 / 0 1 / 1 0 1    length 2 / 3 / 4;
;   1 1 xx         length xx + 4 (5..7); xx = 0: four bits yyyy, length yyyy + 8 (8..22); yyyy = 15: a
;                  byte b, length b + 23; b = 255: a 16-bit length (high byte first).
; After a length: 1 = a match with a 12-bit distance (a byte, then 4 bits); 0 1 = a match with a byte
; distance; 0 0 = that many literal bytes. (Length 2 skips the first flag.) Keeps r1-r12;
; returns the end of the data used.
; Not compiled: UnpackEz branches into it after its two stmfd instructions; it keeps its state in r8,
; r9, r10 and calls func_02028744 and func_02028750, which return in r8, r9, r10 and the carry flag.
	.global UnpackEz10
	.type UnpackEz10, @function
UnpackEz10:
	stmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	stmfd sp!, {lr}
.Lez10_body:
	ldr r7, [r0, #4]
	add r7, r7, r1			; end of the output
	add r0, r0, #16
	mov r8, #0
	mov r10, #-2147483648		; marker only: the first flag refills
.Lez10_next:
	cmp r1, r7
	bge .Lez10_done
	movs r10, r10, lsl #1
	bleq func_02028750
	bcs .Lez10_1
	movs r10, r10, lsl #1
	bleq func_02028750
	movcc r2, #2
	bcc .Lez10_kind_short
	mov r2, #3
	b .Lez10_kind
.Lez10_1:
	movs r10, r10, lsl #1
	bleq func_02028750
	bcs .Lez10_11
	movs r10, r10, lsl #1
	bleq func_02028750
	bcs .Lez10_length_4
	subs r8, r8, #1
	bllt func_02028744
	strb r9, [r1], #1
	mov r9, r9, lsr #8
	b .Lez10_next
.Lez10_length_4:
	mov r2, #4
	b .Lez10_kind
.Lez10_11:
	mov r2, #0
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r2, r2, r2
	movs r10, r10, lsl #1
	bleq func_02028750
	adcs r2, r2, r2
	addne r2, r2, #4
	bne .Lez10_kind
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r2, r2, r2
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r2, r2, r2
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r2, r2, r2
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r2, r2, r2
	cmp r2, #15
	addne r2, r2, #8
	bne .Lez10_kind
	subs r8, r8, #1
	bllt func_02028744
	and r2, r9, #255
	mov r9, r9, lsr #8
	cmp r2, #255
	addne r2, r2, #23
	bne .Lez10_kind
	subs r8, r8, #1
	bllt func_02028744
	and r2, r9, #255
	mov r9, r9, lsr #8
	subs r8, r8, #1
	bllt func_02028744
	and r3, r9, #255
	mov r9, r9, lsr #8
	orr r2, r3, r2, lsl #8
.Lez10_kind:
	movs r10, r10, lsl #1
	bleq func_02028750
	bcs .Lez10_far
.Lez10_kind_short:
	movs r10, r10, lsl #1
	bleq func_02028750
	bcc .Lez10_bytes
	subs r8, r8, #1
	bllt func_02028744
	and r6, r9, #255		; distance
	mov r9, r9, lsr #8
	b .Lez10_match
.Lez10_far:
	subs r8, r8, #1
	bllt func_02028744
	and r6, r9, #255
	mov r9, r9, lsr #8
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r6, r6, r6
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r6, r6, r6
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r6, r6, r6
	movs r10, r10, lsl #1
	bleq func_02028750
	adc r6, r6, r6			; distance
.Lez10_match:
	ldrb r4, [r1, -r6]
	subs r2, r2, #1
	ldrb r3, [r1]			; (unused)
	strb r4, [r1], #1
	bne .Lez10_match
	b .Lez10_next
.Lez10_bytes:
	subs r8, r8, #1
	bllt func_02028744
	ldrb r3, [r1]			; (unused)
	strb r9, [r1], #1
	mov r9, r9, lsr #8
	subs r2, r2, #1
	bne .Lez10_bytes
	b .Lez10_next
.Lez10_done:
	ldmfd sp!, {lr}
	ldmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	bx lr
	.size UnpackEz10, .-UnpackEz10

; RemapPaletteRows: for each of the r2 halfwords at r0 (two pixel bytes, palette row << 4 | colour), gives
; every byte with a colour other than 0 the palette row the table at r1 holds for its row; bytes with colour
; 0 become 0. Halfwords with both colours 0 stay as they are. Keeps every register.
; Not compiled (save rule): saves r8, which it never writes (and r1, which it only reads); no rule puts an
; unwritten r8 in the list.
	.global RemapPaletteRows
	.type RemapPaletteRows, @function
RemapPaletteRows:
	stmfd sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, lr}
	ldr r7, .Lcolour_mask
.Lremap_next:
	ldrh r3, [r0]
	ands r4, r3, r7
	beq .Lremap_skip
	tst r4, #15
	andne r6, r3, #240
	ldrneb r5, [r1, r6, lsr #4]
	orrne r4, r4, r5, lsl #4
	tst r4, #3840
	ldrneb r5, [r1, r3, lsr #12]
	orrne r4, r4, r5, lsl #12
	strh r4, [r0], #2
	subs r2, r2, #1
	bne .Lremap_next
	b .Lremap_done
.Lremap_skip:
	subs r2, r2, #1
	add r0, r0, #2
	bne .Lremap_next
.Lremap_done:
	ldmfd sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, lr}
	bx lr
.Lcolour_mask:
	.word 0x0f0f
	.size RemapPaletteRows, .-RemapPaletteRows

; ---- JPEG ----
; Bit reader (func_02028980 and its helpers): r4 = bit buffer (next bit in bit 31), r5 = bits in it, r6 = next
; input byte; a 0xff byte is followed by a stuffed byte, which is skipped. Kept between blocks in
; data_020c1824, data_020c1818 and data_020c1820.

; func_02028980(block, component, maxcode, valptr, mincode, huffval, dc, zigzag): decodes one 8x8 block of
; coefficients into the 64 halfwords at block. Tables: index 0/1 the DC codes of luma/chroma, 2/3 the AC
; codes (component 2 uses the chroma tables like 1); maxcode, mincode: 32 halfwords per table, valptr: 32
; bytes, huffval: 256 bytes. dc[component] is the running DC value; zigzag maps the coefficient order to
; positions in the block. Keeps every register.
; Not compiled: uses the value func_02028b24 and func_02028a8c return in r1; keeps the bit reader in r4-r6
; across the helper calls, which read and update it; func_02028aac reads this routine's stack frame.
	.global func_02028980
	.type func_02028980, @function
func_02028980:
	stmfd sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}
	mov r12, #0
	mov r11, #0
	mov r10, #0
	mov r9, #0
	stmia r0!, {r9, r10, r11, r12}	; clear the block
	stmia r0!, {r9, r10, r11, r12}
	stmia r0!, {r9, r10, r11, r12}
	stmia r0!, {r9, r10, r11, r12}
	stmia r0!, {r9, r10, r11, r12}
	stmia r0!, {r9, r10, r11, r12}
	stmia r0!, {r9, r10, r11, r12}
	stmia r0!, {r9, r10, r11, r12}
	ldr r4, .Lbit_buffer
	ldr r4, [r4]
	ldr r5, .Lbit_count
	ldrb r5, [r5]
	ldr r6, .Lread_pointer
	ldr r6, [r6]
	ldr r12, [sp, #4]		; DC table
	cmp r12, #2
	moveq r12, #1
	bl func_02028aac
	bl func_02028b24
	bl func_02028a8c
	ldr r2, [sp, #4]
	add r2, r2, r2
	ldr r3, [sp, #64]		; dc
	ldrsh r0, [r3, r2]
	add r0, r0, r1
	strh r0, [r3, r2]
	ldr r11, [sp]
	strh r0, [r11]
	ldr r8, [sp, #68]		; zigzag
	add r9, r8, #64
	add r8, r8, #1
	ldr r12, [sp, #4]		; AC table
	cmp r12, #2
	moveq r12, #1
	add r12, r12, #2
.Lac_next:
	bl func_02028aac
	mov r1, r0, lsr #4		; zero run
	ands r0, r0, #15		; size
	beq .Lac_no_size
	add r8, r8, r1
	bl func_02028b24
	bl func_02028a8c
	ldrb r0, [r8], #1
	mov r0, r0, lsl #1
	strh r1, [r11, r0]
	b .Lac_check_end
.Lac_no_size:
	cmp r1, #15
	bne .Lac_done			; end of block
	add r8, r8, #16			; 16 zeros
.Lac_check_end:
	cmp r8, r9
	bcc .Lac_next
.Lac_done:
	ldr r0, .Lbit_buffer
	str r4, [r0]
	ldr r0, .Lbit_count
	strb r5, [r0]
	ldr r0, .Lread_pointer
	str r6, [r0]
	ldmfd sp!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, lr}
	bx lr
.Lbit_buffer:
	.word data_020c1824
.Lbit_count:
	.word data_020c1818
.Lread_pointer:
	.word data_020c1820
	.size func_02028980, .-func_02028980

; func_02028a8c: sign-extends the r0-bit value in r1 (JPEG: values below 1 << (r0 - 1) are negative,
; r1 - (1 << r0) + 1). Returns it in r1; uses r2.
; Not compiled: takes r0 and r1 and returns its result in r1.
	.global func_02028a8c
	.type func_02028a8c, @function
func_02028a8c:
	mov r2, #1
	mov r2, r2, lsl r0
	cmp r1, r2, lsr #1
	bxcs lr
	mvn r2, #0
	add r1, r1, r2, lsl r0
	add r1, r1, #1
	bx lr
	.size func_02028a8c, .-func_02028a8c

; func_02028aac: decodes one Huffman symbol with table r12 and returns it in r0. Called by func_02028980
; only: reads that routine's maxcode, valptr, mincode and huffval arguments from its stack frame. Codes
; grow a bit at a time until the code is at most maxcode[length] (-1: no code of that length); the symbol is
; huffval[code + valptr[length] - mincode[length]]. Uses r1-r3, r7 and r10.
; Not compiled: reads func_02028980's saved r2/r3 and stack arguments ([sp, #12], [sp, #16], [sp, #60],
; [sp, #64]), loads func_02028b8c's pool word .Lmaxcode_mask, and takes r12 and r4-r6 as inputs.
	.global func_02028aac
	.type func_02028aac, @function
func_02028aac:
	stmfd sp!, {lr}
	bl func_02028b60
	mov r1, r0			; code
	ldr r3, [sp, #12]		; maxcode
	add r2, r3, r12, lsl #6
	mov r7, #2			; length * 2
	ldr r10, .Lmaxcode_mask
.Lhuff_compare:
	ldrsh r3, [r2, r7]
	cmn r3, #1
	andne r3, r3, r10
	cmp r1, r3
	ble .Lhuff_found
	add r7, r7, #2
	bl func_02028b60
	add r1, r0, r1, lsl #1
	b .Lhuff_compare
.Lhuff_found:
	ldr r3, [sp, #16]		; valptr
	add r2, r3, r12, lsl #5
	add r2, r2, r7, lsr #1
	ldrb r2, [r2]
	add r1, r1, r2
	ldr r3, [sp, #60]		; mincode
	add r2, r3, r12, lsl #6
	ldrh r2, [r2, r7]
	sub r1, r1, r2
	ldr r3, [sp, #64]		; huffval
	add r2, r3, r12, lsl #8
	ldrb r0, [r2, r1]
	ldmfd sp!, {lr}
	bx lr
	.size func_02028aac, .-func_02028aac

; func_02028b24: returns the next r0 bits (r0 <= 24) in r1, filling the bit buffer a byte at a time.
; Uses r2.
; Not compiled: takes r0 and the bit reader in r4-r6 (updated on return) and returns its result in r1.
	.global func_02028b24
	.type func_02028b24, @function
func_02028b24:
	subs r5, r5, r0
	bcc .Lbits_fill
	rsb r1, r0, #32
	mov r1, r4, lsr r1
	mov r4, r4, lsl r0
	bx lr
.Lbits_fill:
	add r5, r5, r0
.Lbits_fill_byte:
	rsbs r2, r5, #24
	bls func_02028b24
	ldrb r1, [r6], #1
	cmp r1, #255
	addeq r6, r6, #1
	add r4, r4, r1, lsl r2
	add r5, r5, #8
	b .Lbits_fill_byte
	.size func_02028b24, .-func_02028b24

; func_02028b60: returns the next bit in r0.
; Not compiled: the bit reader is in r4-r6 (inputs, updated on return).
	.global func_02028b60
	.type func_02028b60, @function
func_02028b60:
	subs r5, r5, #1
	bcc .Lbit_fill
.Lbit_take:
	mov r0, r4, lsr #31
	mov r4, r4, lsl #1
	bx lr
.Lbit_fill:
	ldrb r4, [r6], #1
	cmp r4, #255
	addeq r6, r6, #1
	mov r4, r4, lsl #24
	mov r5, #7
	b .Lbit_take
	.size func_02028b60, .-func_02028b60

; func_02028b8c: converts a 16x16 block to 15-bit colour: r0 = four 8x8 luma blocks (Y, 64 bytes each:
; top left, top right, bottom left, bottom right), followed by the 8x8 Cb and Cr blocks; r1 = output in rows
; of 512 pixels; r2 = clamp table to 0..255, indexed by Y plus an offset (-256..511). Each Cb/Cr pair covers
; 2x2 pixels:
;   R = Y + 1.402 Cr', G = Y - 0.344 Cb' - 0.714 Cr', B = Y + 1.772 Cb'  (Cb' = Cb - 128, Cr' = Cr - 128;
;   factors in 13-bit fixed point), each clamped and cut to 5 bits; bit 15 set. Keeps r1-r12.
; Not compiled: its pool word .Lmaxcode_mask is loaded by func_02028aac; it also moves sp inside its loop
; (stmfd/ldmfd of r10-r12).
	.global func_02028b8c
	.type func_02028b8c, @function
func_02028b8c:
	stmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	add r8, r0, #256		; Cb
	mov r4, r2
	mov r10, #4			; luma blocks
.Lcolour_block:
	mov r11, #4			; row pairs
.Lcolour_rows:
	mov r12, #4			; column pairs
.Lcolour_pair:
	stmfd sp!, {r10, r11, r12}
	ldrb r12, [r0]			; Y
	ldrb r11, [r8, #64]		; Cr
	ldrb r10, [r8]			; Cb
	ldr r7, .Lfactor_r_cr
	mul r5, r11, r7
	mov r7, #179
	rsb r5, r7, r5, asr #13		; red offset
	mov r2, r5
	add r5, r5, r12
	ldrb r5, [r4, r5]
	mov r5, r5, lsr #3
	ldr r7, .Lfactor_b_cb
	mul r6, r10, r7
	mov r7, #226
	rsb r6, r7, r6, asr #13		; blue offset
	mov r3, r6
	add r6, r6, r12
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #10
	ldr r7, .Lfactor_g_cb
	mul r6, r10, r7
	ldr r7, .Lfactor_g_cr
	mla r6, r11, r7, r6
	mov r7, #135
	rsb r6, r7, r6, asr #13		; green offset (subtracted)
	mov r9, r6
	sub r6, r12, r6
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #5
	orr r5, r5, #32768
	strh r5, [r1], #2		; top left pixel
	ldrb r12, [r0, #1]
	add r5, r2, r12
	ldrb r5, [r4, r5]
	mov r5, r5, lsr #3
	add r6, r3, r12
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #10
	sub r6, r12, r9
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #5
	orr r5, r5, #32768
	strh r5, [r1], #-2		; top right pixel
	add r1, r1, #1024
	ldrb r12, [r0, #8]
	add r5, r2, r12
	ldrb r5, [r4, r5]
	mov r5, r5, lsr #3
	add r6, r3, r12
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #10
	sub r6, r12, r9
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #5
	orr r5, r5, #32768
	strh r5, [r1], #2		; bottom left pixel
	ldrb r12, [r0, #9]
	add r5, r2, r12
	ldrb r5, [r4, r5]
	mov r5, r5, lsr #3
	add r6, r3, r12
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #10
	sub r6, r12, r9
	ldrb r6, [r4, r6]
	mov r6, r6, lsr #3
	add r5, r5, r6, lsl #5
	orr r5, r5, #32768
	strh r5, [r1], #2		; bottom right pixel
	sub r1, r1, #1024
	add r0, r0, #2
	add r8, r8, #1
	ldmfd sp!, {r10, r11, r12}
	subs r12, r12, #1
	bne .Lcolour_pair
	add r8, r8, #4			; next chroma row
	add r0, r0, #8			; next luma row pair
	add r1, r1, #2032		; next output row pair
	subs r11, r11, #1
	bne .Lcolour_rows
	cmp r10, #3
	bne .Lcolour_not_second
	sub r1, r1, #16			; after the top right block: bottom left
	sub r8, r8, #4
	b .Lcolour_block_next
.Lcolour_not_second:
	sub r1, r1, #4096		; to the block on the right
	sub r1, r1, #4080
	sub r8, r8, #28
.Lcolour_block_next:
	subs r10, r10, #1
	bne .Lcolour_block
	ldmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	bx lr
.Lmaxcode_mask:
	.word 0xffff
.Lfactor_r_cr:
	.word 0x2cdd			; 1.402
.Lfactor_b_cb:
	.word 0x38b4			; 1.772
.Lfactor_g_cb:
	.word 0x0b03			; 0.344
.Lfactor_g_cr:
	.word 0x16da			; 0.714
	.size func_02028b8c, .-func_02028b8c

; ---- Tiles ----

; func_02028d48(src, tile, dst_tile, dst): makes four 8bpp tiles at dst + dst_tile * 64 from two sets of
; four 4bpp tiles at src + tile * 32 and 128 bytes further on: each pixel is the first set's value | the
; second set's value << 4. Keeps r1-r8.
; Not compiled (save rule): saves and restores r1-r3, yet writes r0 (add r0, r0, r1, lsl #5) without
; saving it.
	.global func_02028d48
	.type func_02028d48, @function
func_02028d48:
	stmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8}
	add r0, r0, r1, lsl #5
	add r3, r3, r2, lsl #6
	mov r2, #256			; pixels
	mov r5, #15
.Ltiles_merge:
	ldr r8, [r0, #128]
	ldr r1, [r0], #4
	and r4, r5, r1
	and r6, r5, r1, lsr #4
	orr r4, r4, r6, lsl #8
	and r6, r5, r1, lsr #8
	orr r4, r4, r6, lsl #16
	and r6, r5, r1, lsr #12
	orr r6, r4, r6, lsl #24
	and r4, r5, r8
	and r7, r5, r8, lsr #4
	orr r4, r4, r7, lsl #8
	and r7, r5, r8, lsr #8
	orr r4, r4, r7, lsl #16
	and r7, r5, r8, lsr #12
	orr r4, r4, r7, lsl #24
	orr r6, r6, r4, lsl #4
	str r6, [r3], #4
	and r4, r5, r1, lsr #16
	and r6, r5, r1, lsr #20
	orr r4, r4, r6, lsl #8
	and r6, r5, r1, lsr #24
	orr r4, r4, r6, lsl #16
	and r6, r5, r1, lsr #28
	orr r6, r4, r6, lsl #24
	and r4, r5, r8, lsr #16
	and r7, r5, r8, lsr #20
	orr r4, r4, r7, lsl #8
	and r7, r5, r8, lsr #24
	orr r4, r4, r7, lsl #16
	and r7, r5, r8, lsr #28
	orr r4, r4, r7, lsl #24
	orr r6, r6, r4, lsl #4
	str r6, [r3], #4
	subs r2, r2, #8
	bne .Ltiles_merge
	ldmfd sp!, {r1, r2, r3, r4, r5, r6, r7, r8}
	bx lr
	.size func_02028d48, .-func_02028d48

; func_02028df4(src, tile, dst_tile, dst): widens eight 4bpp tiles at src + tile * 32 to 8bpp tiles at
; dst + dst_tile * 64. Keeps r1-r6.
; Not compiled (save rule): saves and restores r1-r3, yet writes r0 (add r0, r0, r1, lsl #5) without
; saving it.
	.global func_02028df4
	.type func_02028df4, @function
func_02028df4:
	stmfd sp!, {r1, r2, r3, r4, r5, r6}
	add r0, r0, r1, lsl #5
	add r3, r3, r2, lsl #6
	mov r2, #256			; 64 input words, counted in steps of 4
	mov r5, #15
.Ltiles_widen:
	ldr r1, [r0], #4
	subs r2, r2, #4
	and r4, r5, r1
	and r6, r5, r1, lsr #4
	orr r4, r4, r6, lsl #8
	and r6, r5, r1, lsr #8
	orr r4, r4, r6, lsl #16
	and r6, r5, r1, lsr #12
	orr r4, r4, r6, lsl #24
	str r4, [r3], #4
	and r4, r5, r1, lsr #16
	and r6, r5, r1, lsr #20
	orr r4, r4, r6, lsl #8
	and r6, r5, r1, lsr #24
	orr r4, r4, r6, lsl #16
	and r6, r5, r1, lsr #28
	orr r4, r4, r6, lsl #24
	str r4, [r3], #4
	bne .Ltiles_widen
	ldmfd sp!, {r1, r2, r3, r4, r5, r6}
	bx lr
	.size func_02028df4, .-func_02028df4
