; The first 0x800 bytes of the ARM9 binary (0x02000000-0x02000800), the cartridge's secure area: 18 BIOS call
; thunks (Thumb) at halfword addresses between runs of filler. The filler is not code or data the program uses;
; each run is read from the ARM9 binary of the user's own ROM at build time (.incbin, offset and length from
; 0x02000000), which keeps every thunk at its address.
; Each thunk issues one BIOS software interrupt with the caller's arguments still in r0-r3 and returns with
; the BIOS result in r0.
; Not compiled: every thunk is built around a `swi` instruction, which the compiler has no C expression for
; (its inline `asm` is the only way to emit one); the thunks also start at halfword addresses inside filler.

	.text
	.thumb
	.global BitUnPack
	.global LZ77UnCompReadByCallbackWrite16bit
	.global Div
	.global RLUnCompReadNormalWrite8bit
	.global Mod
	.global LZ77UnCompReadNormalWrite8bit
	.global Halt
	.global IsDebugger
	.global IntrWait
	.global Sqrt
	.global HuffUnCompReadByCallback
	.global GetCRC16
	.global CpuFastSet
	.global RLUnCompReadByCallbackWrite16bit
	.global CpuSet
	.global WaitByLoop
	.global VBlankIntrWait
	.global SoftReset

; filler 0x02000000-0x0200007c
	.incbin "extract/usa/arm9/arm9.bin", 0x0, 0x7c

; BitUnPack: BIOS call 16.
	.type BitUnPack, @function
BitUnPack:
	swi 16
	bx lr
	.size BitUnPack, 4

; filler 0x02000080-0x020000da
	.incbin "extract/usa/arm9/arm9.bin", 0x80, 0x5a

; LZ77UnCompReadByCallbackWrite16bit: BIOS call 18.
	.type LZ77UnCompReadByCallbackWrite16bit, @function
LZ77UnCompReadByCallbackWrite16bit:
	swi 18
	bx lr
	.size LZ77UnCompReadByCallbackWrite16bit, 4

; filler 0x020000de-0x02000136
	.incbin "extract/usa/arm9/arm9.bin", 0xde, 0x58

; Div: BIOS call 9.
	.type Div, @function
Div:
	swi 9
	bx lr
	.size Div, 4

; filler 0x0200013a-0x020001a8
	.incbin "extract/usa/arm9/arm9.bin", 0x13a, 0x6e

; RLUnCompReadNormalWrite8bit: BIOS call 20.
	.type RLUnCompReadNormalWrite8bit, @function
RLUnCompReadNormalWrite8bit:
	swi 20
	bx lr
	.size RLUnCompReadNormalWrite8bit, 4

; filler 0x020001ac-0x02000206
	.incbin "extract/usa/arm9/arm9.bin", 0x1ac, 0x5a

; Mod: the BIOS division call leaves the remainder in r1; returns it.
	.type Mod, @function
Mod:
	swi 9
	add r0, r1, #0
	bx lr
	.size Mod, 6

; filler 0x0200020c-0x0200025e
	.incbin "extract/usa/arm9/arm9.bin", 0x20c, 0x52

; LZ77UnCompReadNormalWrite8bit: BIOS call 17.
	.type LZ77UnCompReadNormalWrite8bit, @function
LZ77UnCompReadNormalWrite8bit:
	swi 17
	bx lr
	.size LZ77UnCompReadNormalWrite8bit, 4

; filler 0x02000262-0x020002ca
	.incbin "extract/usa/arm9/arm9.bin", 0x262, 0x68

; Halt: BIOS call 6.
	.type Halt, @function
Halt:
	swi 6
	bx lr
	.size Halt, 4

; filler 0x020002ce-0x0200035e
	.incbin "extract/usa/arm9/arm9.bin", 0x2ce, 0x90

; IsDebugger: BIOS call 15.
	.type IsDebugger, @function
IsDebugger:
	swi 15
	bx lr
	.size IsDebugger, 4

; filler 0x02000362-0x020003d2
	.incbin "extract/usa/arm9/arm9.bin", 0x362, 0x70

; IntrWait: BIOS call 4.
	.type IntrWait, @function
IntrWait:
	swi 4
	bx lr
	.size IntrWait, 4

; filler 0x020003d6-0x02000424
	.incbin "extract/usa/arm9/arm9.bin", 0x3d6, 0x4e

; Sqrt: BIOS call 13.
	.type Sqrt, @function
Sqrt:
	swi 13
	bx lr
	.size Sqrt, 4

; filler 0x02000428-0x02000496
	.incbin "extract/usa/arm9/arm9.bin", 0x428, 0x6e

; HuffUnCompReadByCallback: BIOS call 19.
	.type HuffUnCompReadByCallback, @function
HuffUnCompReadByCallback:
	swi 19
	bx lr
	.size HuffUnCompReadByCallback, 4

; filler 0x0200049a-0x02000506
	.incbin "extract/usa/arm9/arm9.bin", 0x49a, 0x6c

; GetCRC16: BIOS call 14.
	.type GetCRC16, @function
GetCRC16:
	swi 14
	bx lr
	.size GetCRC16, 4

; filler 0x0200050a-0x0200055e
	.incbin "extract/usa/arm9/arm9.bin", 0x50a, 0x54

; CpuFastSet: BIOS call 12.
	.type CpuFastSet, @function
CpuFastSet:
	swi 12
	bx lr
	.size CpuFastSet, 4

; filler 0x02000562-0x020005cc
	.incbin "extract/usa/arm9/arm9.bin", 0x562, 0x6a

; RLUnCompReadByCallbackWrite16bit: BIOS call 21.
	.type RLUnCompReadByCallbackWrite16bit, @function
RLUnCompReadByCallbackWrite16bit:
	swi 21
	bx lr
	.size RLUnCompReadByCallbackWrite16bit, 4

; filler 0x020005d0-0x02000640
	.incbin "extract/usa/arm9/arm9.bin", 0x5d0, 0x70

; CpuSet: BIOS call 11.
	.type CpuSet, @function
CpuSet:
	swi 11
	bx lr
	.size CpuSet, 4

; filler 0x02000644-0x020006be
	.incbin "extract/usa/arm9/arm9.bin", 0x644, 0x7a

; WaitByLoop: BIOS call 3.
	.type WaitByLoop, @function
WaitByLoop:
	swi 3
	bx lr
	.size WaitByLoop, 4

; filler 0x020006c2-0x02000732
	.incbin "extract/usa/arm9/arm9.bin", 0x6c2, 0x70

; VBlankIntrWait: the BIOS interrupt wait with r2 = 0 (no discard of earlier flags).
	.type VBlankIntrWait, @function
VBlankIntrWait:
	mov r2, #0
	swi 5
	bx lr
	.size VBlankIntrWait, 6

; filler 0x02000738-0x0200078a
	.incbin "extract/usa/arm9/arm9.bin", 0x738, 0x52

; SoftReset: BIOS call 0.
	.type SoftReset, @function
SoftReset:
	swi 0
	bx lr
	.size SoftReset, 4

; filler 0x0200078e-0x02000800
	.incbin "extract/usa/arm9/arm9.bin", 0x78e, 0x72
