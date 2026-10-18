; Data of the code area between the stage map menu and the start-up code: the fonts used by the text routines
; (func_020215fc and func_02021624), the model and sprite scripts, and the pointer tables that name them.
;
; This is an assembler file because the compiler cannot lay the block out: it puts only non-const objects in the
; .text section, and it emits every data object as its own section in order of size, while the block holds objects
; of all sizes in an order that is not by size.
;
; Model script (read by func_02026718): a 16-bit word holds the command (bits 10-15), a mode (bits 8-9) and a signed
; 8-bit argument; most commands are followed by a 16-bit operand word.
; Sprite script (read by func_0202db1c): a 16-bit word above 0xfff0 is a command, any other word is a frame.

	.macro MODEL_OP cmd, mode, arg
	.hword ((\cmd) << 10) | ((\mode) << 8) | ((\arg) & 0xff)
	.endm

	.equ MC_JUMP, 0
	.equ MC_JUMP_IF_EQUAL, 1
	.equ MC_JUMP_IF_NOT_EQUAL, 2
	.equ MC_JUMP_IF_NOT_LESS, 3
	.equ MC_JUMP_IF_LESS, 4
	.equ MC_JUMP_IF_NOT_LESS_OR_EQUAL, 5
	.equ MC_JUMP_IF_EITHER, 6
	.equ MC_SET, 7
	.equ MC_ADD, 8
	.equ MC_SUB, 9
	.equ MC_MOD, 10
	.equ MC_CALL, 11
	.equ MC_COMPARE, 12
	.equ MC_ANIM, 14
	.equ MC_EVENT, 15
	.equ MC_TRACK, 16
	.equ MC_FLAG, 17
	.equ MC_STEP, 18
	.equ MC_SIGNAL, 19
	.equ MC_SET_3C, 20
	.equ MC_SET_3E, 21
	.equ MC_WAIT, 22
	.equ MC_WAIT_OR_IDLE, 23
	.equ MC_LOOK_AT, 24
	.equ MC_TEST_BIT, 25
	.equ MC_TEST_SET_BIT, 26
	.equ MC_TEST_CLEAR_BIT, 27
	.equ MC_MUL, 29
	.equ MC_AND, 30
	.equ MC_MODEL, 32
	.equ MC_READ, 63
	.equ MM_BITS, 0
	.equ MM_REG, 1
	.equ MM_VALUE, 2
	.equ MM_3, 3

	.equ SPRITE_CMD_SET_SPEED, 0xffff
	.equ SPRITE_CMD_SET_STATE, 0xfffe
	.equ SPRITE_CMD_END, 0xfffd
	.equ SPRITE_CMD_RESET, 0xfffc
	.equ SPRITE_CMD_ADD_STATE, 0xfffb
	.equ SPRITE_CMD_JUMP, 0xfffa
	.equ SPRITE_CMD_SET_REG, 0xfff9
	.equ SPRITE_CMD_COPY_REG, 0xfff8
	.equ SPRITE_CMD_REQUEST_STATE, 0xfff7
	.equ SPRITE_CMD_SET_RETURN, 0xfff6
	.equ SPRITE_CMD_SET_FRAME_HIGH, 0xfff5
	.equ SPRITE_CMD_COMPARE_REG, 0xfff4
	.equ SPRITE_CMD_JUMP_IF_EQUAL, 0xfff3
	.equ SPRITE_CMD_JUMP_IF_NOT_EQUAL, 0xfff2

	.text

; Fonts: one range of character codes {first, last, glyph} and the list terminator (first and last 0).

	.global data_0201a608
	.type data_0201a608, @object
data_0201a608:
	.hword 0x0021, 0x00ff, 0x0138, 0x0000, 0x0000
	.size data_0201a608, 10

	.global data_0201a612
	.type data_0201a612, @object
data_0201a612:
	.hword 0x0021, 0x00ff, 0x0217, 0x0000, 0x0000
	.size data_0201a612, 10

	.global data_0201a61c
	.type data_0201a61c, @object
data_0201a61c:
	.hword 0x0021, 0x00ff, 0x02f6, 0x0000, 0x0000
	.size data_0201a61c, 10

	.global data_0201a626
	.type data_0201a626, @object
data_0201a626:
	.hword 0x0021, 0x00ff, 0x03d5, 0x0000, 0x0000
	.size data_0201a626, 10

	.global data_0201a630
	.type data_0201a630, @object
data_0201a630:
	.hword 0x0021, 0x00ff, 0x04b4, 0x0000, 0x0000
	.size data_0201a630, 10

	.global data_0201a63a
	.type data_0201a63a, @object
data_0201a63a:
	.hword 0x0021, 0x00ff, 0x0593, 0x0000, 0x0000
	.size data_0201a63a, 10

	.global data_0201a644
	.type data_0201a644, @object
data_0201a644:
	.hword 0x0021, 0x00ff, 0x0672, 0x0000, 0x0000
	.size data_0201a644, 10

	.global data_0201a64e
	.type data_0201a64e, @object
data_0201a64e:
	.hword 0x0021, 0x00ff, 0x0751, 0x0000, 0x0000
	.size data_0201a64e, 10

	.global data_0201a658
	.type data_0201a658, @object
data_0201a658:
	.hword 0x0021, 0x00ff, 0x0830, 0x0000, 0x0000
	.size data_0201a658, 10

	.global data_0201a662
	.type data_0201a662, @object
data_0201a662:
	.hword 0x0021, 0x003e, 0x090f, 0x0000, 0x0000
	.size data_0201a662, 10

	.global data_0201a66c
	.type data_0201a66c, @object
data_0201a66c:
	.hword 0x0021, 0x00ff, 0x092d, 0x0000, 0x0000
	.size data_0201a66c, 10

	.global data_0201a676
	.type data_0201a676, @object
data_0201a676:
	.hword 0x002b, 0x003b, 0x0a0c, 0x0000, 0x0000
	.size data_0201a676, 10

	.global data_0201a680
	.type data_0201a680, @object
data_0201a680:
	.hword 0x002b, 0x003a, 0x0a1d, 0x0000, 0x0000
	.size data_0201a680, 10

	.global data_0201a68a
	.type data_0201a68a, @object
data_0201a68a:
	.hword 0x002b, 0x003b, 0x0a2d, 0x0000, 0x0000
	.size data_0201a68a, 10

	.global data_0201a694
	.type data_0201a694, @object
data_0201a694:
	.hword 0x002b, 0x003b, 0x0a3e, 0x0000, 0x0000
	.size data_0201a694, 10

	.global data_0201a69e
	.type data_0201a69e, @object
data_0201a69e:
	.hword 0x002b, 0x003b, 0x0a4f, 0x0000, 0x0000
	.size data_0201a69e, 10

	.global data_0201a6a8
	.type data_0201a6a8, @object
data_0201a6a8:
	.hword 0x002b, 0x003b, 0x0a60, 0x0000, 0x0000
	.size data_0201a6a8, 10

	.global data_0201a6b2
	.type data_0201a6b2, @object
data_0201a6b2:
	.hword 0x002b, 0x003b, 0x0a71, 0x0000, 0x0000
	.size data_0201a6b2, 10

	.global data_0201a6bc
	.type data_0201a6bc, @object
data_0201a6bc:
	.hword 0x002b, 0x0039, 0x0a82, 0x0000, 0x0000
	.size data_0201a6bc, 10

	.global data_0201a6c6
	.type data_0201a6c6, @object
data_0201a6c6:
	.hword 0x0021, 0x00ff, 0x0a91, 0x0000, 0x0000
	.size data_0201a6c6, 10

	.global data_0201a6d0
	.type data_0201a6d0, @object
data_0201a6d0:
	.hword 0x0021, 0x00ff, 0x0b70, 0x0000, 0x0000
	.size data_0201a6d0, 10

	.global data_0201a6da
	.type data_0201a6da, @object
data_0201a6da:
	.hword 0x0021, 0x00ff, 0x0c4f, 0x0000, 0x0000
	.size data_0201a6da, 10

; Zero fill up to the first script.
	.global data_0201a6e4
	.type data_0201a6e4, @object
data_0201a6e4:
	.space 28
	.size data_0201a6e4, 28

; model script
	.global data_0201a700
	.type data_0201a700, @object
data_0201a700:
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d0a
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d09
	MODEL_OP MC_WAIT, MM_VALUE, 32
	.size data_0201a700, 14

; model script
	.global data_0201a70e
	.type data_0201a70e, @object
data_0201a70e:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0cf7
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0008, 0x0101
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0cf9
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0101
	MODEL_OP MC_SET, MM_VALUE, 7
	.hword 0x005a
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0cf8
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201a70e, 36

; model script
	.global data_0201a732
	.type data_0201a732, @object
data_0201a732:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0cfa
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0008, 0x0101
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0cfc
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0101
	MODEL_OP MC_SET, MM_VALUE, 7
	.hword 0x003c
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0cfb
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201a732, 36

; model script
	.global data_0201a756
	.type data_0201a756, @object
data_0201a756:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0cfd
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0008, 0x0101
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0cff
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0101
	MODEL_OP MC_SET, MM_VALUE, 7
	.hword 0x0042
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0cfe
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201a756, 36

; model script
	.global data_0201a77a
	.type data_0201a77a, @object
data_0201a77a:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d00
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0008, 0x0101
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d02
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0101
	MODEL_OP MC_SET, MM_VALUE, 7
	.hword 0x0075
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d01
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201a77a, 36

; model script
	.global data_0201a79e
	.type data_0201a79e, @object
data_0201a79e:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d03
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0008, 0x0101
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d05
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0101
	MODEL_OP MC_SET, MM_VALUE, 7
	.hword 0x004b
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d04
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201a79e, 36

; model script
	.global data_0201a7c2
	.type data_0201a7c2, @object
data_0201a7c2:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d06
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0008, 0x0101
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d08
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0101
	MODEL_OP MC_SET, MM_VALUE, 7
	.hword 0x0072
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d07
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201a7c2, 36

; sprite script
	.global data_0201a7e6
	.type data_0201a7e6, @object
data_0201a7e6:
	.hword SPRITE_CMD_SET_SPEED, 0x0001
	.hword 0x0e8c
	.hword 0x0e8d
	.hword 0x0e8e
	.hword 0x0e8f
	.hword 0x0e90
	.hword 0xfffd
	.size data_0201a7e6, 16

; model script
	.global data_0201a7f6
	.type data_0201a7f6, @object
data_0201a7f6:
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	.size data_0201a7f6, 4

; model script
	.global data_0201a7fa
	.type data_0201a7fa, @object
data_0201a7fa:
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d30
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d2f
	MODEL_OP MC_WAIT, MM_VALUE, 32
	.size data_0201a7fa, 14

; sprite script
	.global data_0201a808
	.type data_0201a808, @object
data_0201a808:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0e3b
	.hword 0x0e3c
	.hword 0x0e3d
	.hword 0x0e3e
	.hword 0x0e3f
	.hword 0x0e40
	.hword 0x0e41
	.hword 0x0e42
	.hword 0x0e43
	.hword 0x0e44
	.hword 0x0e45
	.hword 0x0e46
	.hword SPRITE_CMD_JUMP, 0xffe2
	.size data_0201a808, 32

; sprite script
	.global data_0201a828
	.type data_0201a828, @object
data_0201a828:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0e47
	.hword 0x0e48
	.hword 0x0e49
	.hword 0x0e4a
	.hword 0x0e4b
	.hword 0x0e4c
	.hword 0x0e4d
	.hword 0x0e4e
	.hword 0x0e4f
	.hword 0x0e50
	.hword 0x0e51
	.hword 0x0e52
	.hword 0x0e53
	.hword 0x0e54
	.hword 0x0e55
	.hword 0x0e56
	.hword 0x0e57
	.hword 0x0e58
	.hword SPRITE_CMD_JUMP, 0xffd6
	.size data_0201a828, 44

; sprite script
	.global data_0201a854
	.type data_0201a854, @object
data_0201a854:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0e59
	.hword 0x0e5a
	.hword 0x0e5b
	.hword 0x0e5c
	.hword 0x0e5d
	.hword 0x0e5e
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201a854, 20

; sprite script
	.global data_0201a868
	.type data_0201a868, @object
data_0201a868:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0e5f
	.hword 0x0e60
	.hword 0x0e61
	.hword 0x0e62
	.hword 0x0e63
	.hword 0x0e64
	.hword 0x0e65
	.hword 0x0e66
	.hword 0x0e67
	.hword 0x0e68
	.hword 0x0e69
	.hword 0x0e6a
	.hword SPRITE_CMD_JUMP, 0xffe2
	.size data_0201a868, 32

; sprite script
	.global data_0201a888
	.type data_0201a888, @object
data_0201a888:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0e6b
	.hword 0x0e6c
	.hword 0x0e6d
	.hword 0x0e6e
	.hword 0x0e6f
	.hword 0x0e70
	.hword 0x0e71
	.hword 0x0e72
	.hword 0x0e73
	.hword 0x0e74
	.hword 0x0e75
	.hword 0x0e76
	.hword SPRITE_CMD_JUMP, 0xffe2
	.size data_0201a888, 32

; sprite script
	.global data_0201a8a8
	.type data_0201a8a8, @object
data_0201a8a8:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d91
	.hword 0x0d92
	.hword 0x0d93
	.hword 0x0d94
	.hword 0x0d95
	.hword 0x0d96
	.hword 0x0d97
	.hword 0x0d98
	.hword 0x0d99
	.hword 0xfffd
	.size data_0201a8a8, 24

; sprite script
	.global data_0201a8c0
	.type data_0201a8c0, @object
data_0201a8c0:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d9a
	.hword 0x0d9b
	.hword 0x0d9c
	.hword 0x0d9d
	.hword 0x0d9e
	.hword 0x0d9f
	.hword 0x0da0
	.hword 0x0da1
	.hword 0x0da2
	.hword 0xfffd
	.size data_0201a8c0, 24

; model script
	.global data_0201a8d8
	.type data_0201a8d8, @object
data_0201a8d8:
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d9b
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d9a
	MODEL_OP MC_WAIT, MM_VALUE, 32
	.size data_0201a8d8, 14

; model script
	.global data_0201a8e6
	.type data_0201a8e6, @object
data_0201a8e6:
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d9d
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d9c
	MODEL_OP MC_WAIT, MM_VALUE, 32
	.size data_0201a8e6, 14

; model script
	.global data_0201a8f4
	.type data_0201a8f4, @object
data_0201a8f4:
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d99
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d98
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201a8f4, 16

; sprite script
	.global data_0201a904
	.type data_0201a904, @object
data_0201a904:
	.hword 0xfffc
	.hword SPRITE_CMD_SET_STATE, 0x0100
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x0128, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0e91
	.hword SPRITE_CMD_JUMP, 0xffea
	.size data_0201a904, 24

; sprite script
	.global data_0201a91c
	.type data_0201a91c, @object
data_0201a91c:
	.hword 0xfffc
	.hword SPRITE_CMD_SET_STATE, 0x0100
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x0110, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0e92
	.hword SPRITE_CMD_JUMP, 0xffea
	.size data_0201a91c, 24

; sprite script
	.global data_0201a934
	.type data_0201a934, @object
data_0201a934:
	.hword 0xfffc
	.hword SPRITE_CMD_SET_STATE, 0x0100
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x00f8, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0e93
	.hword SPRITE_CMD_JUMP, 0xffea
	.size data_0201a934, 24

; sprite script
	.global data_0201a94c
	.type data_0201a94c, @object
data_0201a94c:
	.hword 0xfffc
	.hword SPRITE_CMD_SET_STATE, 0x0100
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x00e0, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0e94
	.hword SPRITE_CMD_JUMP, 0xffea
	.size data_0201a94c, 24

; sprite script
	.global data_0201a964
	.type data_0201a964, @object
data_0201a964:
	.hword 0xfffc
	.hword SPRITE_CMD_SET_STATE, 0x0100
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x00c8, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0e95
	.hword SPRITE_CMD_JUMP, 0xffea
	.size data_0201a964, 24

; sprite script
	.global data_0201a97c
	.type data_0201a97c, @object
data_0201a97c:
	.hword 0xfffc
	.hword SPRITE_CMD_SET_STATE, 0x0100
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x00b0, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0e96
	.hword SPRITE_CMD_JUMP, 0xffea
	.size data_0201a97c, 24

; sprite script
	.global data_0201a994
	.type data_0201a994, @object
data_0201a994:
	.hword 0xfffc
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x009c, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0ea6
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201a994, 20

; sprite script
	.global data_0201a9a8
	.type data_0201a9a8, @object
data_0201a9a8:
	.hword 0xfffc
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x0088, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0ea7
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201a9a8, 20

; sprite script
	.global data_0201a9bc
	.type data_0201a9bc, @object
data_0201a9bc:
	.hword 0xfffc
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x0074, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0ea8
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201a9bc, 20

; sprite script
	.global data_0201a9d0
	.type data_0201a9d0, @object
data_0201a9d0:
	.hword 0xfffc
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x0060, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0ea9
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201a9d0, 20

; sprite script
	.global data_0201a9e4
	.type data_0201a9e4, @object
data_0201a9e4:
	.hword 0xfffc
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x004c, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0eaa
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201a9e4, 20

; sprite script
	.global data_0201a9f8
	.type data_0201a9f8, @object
data_0201a9f8:
	.hword 0xfffc
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x0038, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0eab
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201a9f8, 20

; sprite script
	.global data_0201aa0c
	.type data_0201aa0c, @object
data_0201aa0c:
	.hword 0xfffc
	.hword SPRITE_CMD_ADD_STATE, 0x0000, 0x0024, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0eac
	.hword SPRITE_CMD_JUMP, 0xffee
	.size data_0201aa0c, 20

; sprite script
	.global data_0201aa20
	.type data_0201aa20, @object
data_0201aa20:
	.hword 0xfffc
	.hword SPRITE_CMD_SET_SPEED, 0x0005
	.hword 0x0ead
	.hword 0x0eae
	.hword 0x0eaf
	.hword 0x0eb0
	.hword 0x0eb1
	.hword 0x0eb2
	.hword SPRITE_CMD_JUMP, 0xffec
	.hword SPRITE_CMD_SET_STATE, 0x0200
	.hword SPRITE_CMD_SET_SPEED, 0x0003
	.hword 0x0e97
	.hword 0x0e98
	.hword 0x0e99
	.hword 0x0e9a
	.hword 0x0e9b
	.hword 0x0e9c
	.hword 0x0e9d
	.hword 0x0e9e
	.hword 0x0e9f
	.hword 0x0ea0
	.hword 0x0ea1
	.hword 0x0ea2
	.hword 0x0ea3
	.hword 0x0ea4
	.hword 0x0ea5
	.hword 0xfffd
	.hword 0x0000
	.size data_0201aa20, 64

; Table of scripts: 37 model scripts, then 2 model scripts, then 2 sprite scripts (indices 39 and 40)
	.global data_0201aa60
	.type data_0201aa60, @object
data_0201aa60:
	.word data_0201adec
	.word data_0201ae08
	.word data_0201ae26
	.word data_0201ae44
	.word data_0201ae64
	.word data_0201ae82
	.word data_0201aea0
	.word data_0201aebe
	.word data_0201aedc
	.word data_0201aefc
	.word data_0201af1a
	.word data_0201af38
	.word data_0201af56
	.word data_0201af74
	.word data_0201af92
	.word data_0201afb2
	.word data_0201afd2
	.word data_0201aff2
	.word data_0201b010
	.word data_0201b030
	.word data_0201b04e
	.word data_0201b06e
	.word data_0201b08c
	.word data_0201b0aa
	.word data_0201b0c8
	.word data_0201b0e6
	.word data_0201b104
	.word data_0201b124
	.word data_0201b142
	.word data_0201b160
	.word data_0201b17e
	.word data_0201b19c
	.word data_0201b1bc
	.word data_0201b1da
	.word data_0201b1f8
	.word data_0201b216
	.word data_0201b234
	.word data_0201b252
	.word data_0201b270
	.word data_0201ac88
	.word data_0201aca2
	.size data_0201aa60, 164

; Table of model scripts
	.global data_0201ab04
	.type data_0201ab04, @object
data_0201ab04:
	.word data_0201b344
	.word data_0201b350
	.word data_0201b35c
	.word data_0201b368
	.word data_0201b374
	.word data_0201b380
	.word data_0201b38c
	.word data_0201b398
	.word data_0201b3a4
	.word data_0201b3b0
	.word data_0201b3bc
	.word data_0201b3c8
	.word data_0201b3d4
	.word data_0201b3e0
	.word data_0201b3ec
	.word data_0201b3f8
	.word data_0201b404
	.word data_0201b410
	.word data_0201b41c
	.word data_0201b428
	.word data_0201b434
	.word data_0201b440
	.word data_0201b44c
	.word data_0201b458
	.word data_0201b464
	.word data_0201b470
	.word data_0201b47c
	.word data_0201b488
	.word data_0201b494
	.word data_0201b4a0
	.word data_0201b4ac
	.word data_0201b4b8
	.word data_0201b4c4
	.word data_0201b4d0
	.word data_0201b4dc
	.word data_0201b4e8
	.word data_0201b4f4
	.word data_0201b500
	.word data_0201b50c
	.size data_0201ab04, 156

; Table of model scripts
	.global data_0201aba0
	.type data_0201aba0, @object
data_0201aba0:
	.word data_0201b5aa
	.word data_0201b5ce
	.word data_0201b60c
	.word data_0201b64a
	.word data_0201b67c
	.word data_0201ada6
	.word data_0201ad6a
	.size data_0201aba0, 28

; Table of model scripts
	.global data_0201abbc
	.type data_0201abbc, @object
data_0201abbc:
	.word data_0201a700
	.word data_0201a70e
	.word data_0201a732
	.word data_0201a756
	.word data_0201a77a
	.word data_0201a79e
	.word data_0201a7c2
	.word data_0201a7f6
	.word data_0201a7fa
	.word data_0201a8d8
	.word data_0201a8e6
	.word data_0201a8f4
	.size data_0201abbc, 48

; Table of sprite scripts
	.global data_0201abec
	.type data_0201abec, @object
data_0201abec:
	.word data_0201b68a
	.word data_0201b6a6
	.word data_0201b6cc
	.word data_0201b7ae
	.word data_0201b7d2
	.word data_0201a7e6
	.word data_0201a808
	.word data_0201a828
	.word data_0201a854
	.word data_0201a868
	.word data_0201a888
	.word data_0201a8a8
	.word data_0201a8c0
	.word data_0201a904
	.word data_0201a91c
	.word data_0201a934
	.word data_0201a94c
	.word data_0201a964
	.word data_0201a97c
	.word data_0201a994
	.word data_0201a9a8
	.word data_0201a9bc
	.word data_0201a9d0
	.word data_0201a9e4
	.word data_0201a9f8
	.word data_0201aa0c
	.word data_0201aa20
	.word data_0201acba
	.word data_0201ace4
	.word data_0201ad0e
	.word data_0201ad38
	.word data_0201ad62
	.word data_0201b6e4
	.word data_0201b712
	.word data_0201b72c
	.word data_0201b746
	.word data_0201b760
	.word data_0201b77a
	.word data_0201b794
	.size data_0201abec, 156

; sprite script
	.global data_0201ac88
	.type data_0201ac88, @object
data_0201ac88:
	.hword SPRITE_CMD_SET_SPEED, 0x0006
	.hword 0x0d6a
	.hword 0x0d6b
	.hword 0x0d6c
	.hword 0x0d6d
	.hword 0x0d6e
	.hword 0x0d6f
	.hword 0x0d70
	.hword 0x0d71
	.hword 0x0d72
	.hword 0x0d73
	.hword 0xfffd
	.size data_0201ac88, 26

; sprite script
	.global data_0201aca2
	.type data_0201aca2, @object
data_0201aca2:
	.hword SPRITE_CMD_SET_SPEED, 0x0006
	.hword 0x0d74
	.hword 0x0d75
	.hword 0x0d76
	.hword 0x0d77
	.hword 0x0d78
	.hword 0x0d79
	.hword 0x0d7a
	.hword 0x0d7b
	.hword 0x0d7c
	.hword 0xfffd
	.size data_0201aca2, 24

; sprite script
	.global data_0201acba
	.type data_0201acba, @object
data_0201acba:
	.hword SPRITE_CMD_SET_SPEED, 0x0003
	.hword 0x0046
	.hword 0x0047
	.hword 0x0048
	.hword 0x0049
	.hword 0x004a
	.hword 0x004b
	.hword 0x004c
	.hword 0x004d
	.hword 0x004e
	.hword 0x004f
	.hword 0x0050
	.hword 0x0051
	.hword 0x0052
	.hword 0x0053
	.hword 0x0054
	.hword SPRITE_CMD_SET_SPEED, 0x30d4
	.hword 0x0055
	.hword 0xfffd
	.size data_0201acba, 42

; sprite script
	.global data_0201ace4
	.type data_0201ace4, @object
data_0201ace4:
	.hword SPRITE_CMD_SET_SPEED, 0x0003
	.hword 0x0056
	.hword 0x0057
	.hword 0x0058
	.hword 0x0059
	.hword 0x005a
	.hword 0x005b
	.hword 0x005c
	.hword 0x005d
	.hword 0x005e
	.hword 0x005f
	.hword 0x0060
	.hword 0x0061
	.hword 0x0062
	.hword 0x0063
	.hword 0x0064
	.hword SPRITE_CMD_SET_SPEED, 0x30d4
	.hword 0x0065
	.hword 0xfffd
	.size data_0201ace4, 42

; sprite script
	.global data_0201ad0e
	.type data_0201ad0e, @object
data_0201ad0e:
	.hword SPRITE_CMD_SET_SPEED, 0x0003
	.hword 0x0066
	.hword 0x0067
	.hword 0x0068
	.hword 0x0069
	.hword 0x006a
	.hword 0x006b
	.hword 0x006c
	.hword 0x006d
	.hword 0x006e
	.hword 0x006f
	.hword 0x0070
	.hword 0x0071
	.hword 0x0072
	.hword 0x0073
	.hword 0x0074
	.hword SPRITE_CMD_SET_SPEED, 0x30d4
	.hword 0x0075
	.hword 0xfffd
	.size data_0201ad0e, 42

; sprite script
	.global data_0201ad38
	.type data_0201ad38, @object
data_0201ad38:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0027
	.hword 0x0028
	.hword 0x0029
	.hword 0x002a
	.hword 0x002b
	.hword 0x002c
	.hword 0x002d
	.hword 0x002e
	.hword 0x002f
	.hword 0x0030
	.hword 0x0031
	.hword 0x0032
	.hword 0x0033
	.hword 0x0034
	.hword 0x0035
	.hword 0x0036
	.hword 0x0037
	.hword 0x0038
	.hword 0xfffd
	.size data_0201ad38, 42

; sprite script
	.global data_0201ad62
	.type data_0201ad62, @object
data_0201ad62:
	.hword 0xfffc
	.hword 0x0039
	.hword SPRITE_CMD_JUMP, 0xfffa
	.size data_0201ad62, 8

; model script
	.global data_0201ad6a
	.type data_0201ad6a, @object
data_0201ad6a:
	MODEL_OP MC_SET, MM_BITS, 28
	MODEL_OP MC_FLAG, MM_BITS, 0
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x000c, 0x0102
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0011, 0x0203
	MODEL_OP MC_SET, MM_BITS, 62
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0da7
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0da6
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0102
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0da9
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0da8
	MODEL_OP MC_WAIT, MM_VALUE, 0
	MODEL_OP MC_JUMP, MM_VALUE, -13
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0203
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0da5
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0da4
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -21
	.size data_0201ad6a, 60

; model script
	.global data_0201ada6
	.type data_0201ada6, @object
data_0201ada6:
	MODEL_OP MC_SET, MM_BITS, 28
	MODEL_OP MC_FLAG, MM_BITS, 0
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x000b, 0x0002
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x000f, 0x0003
	MODEL_OP MC_EVENT, MM_BITS, 2
	.hword 0x0013, 0x0004
	MODEL_OP MC_SET, MM_BITS, 62
	MODEL_OP MC_STEP, MM_BITS, 0
	MODEL_OP MC_SET, MM_BITS, 94
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d5e
	MODEL_OP MC_ANIM, MM_VALUE, 1
	.hword 0x0d5d
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -8
	MODEL_OP MC_SET, MM_BITS, 126
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d5b
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0d5a
	MODEL_OP MC_WAIT, MM_VALUE, 1
	MODEL_OP MC_JUMP, MM_VALUE, -15
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0004
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0d59
	MODEL_OP MC_ANIM, MM_VALUE, 1
	.hword 0x0d58
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -23
	.size data_0201ada6, 70

; model script
	.global data_0201adec
	.type data_0201adec, @object
data_0201adec:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_BITS, 104
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0004
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0005
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0245
	.size data_0201adec, 28

; model script
	.global data_0201ae08
	.type data_0201ae08, @object
data_0201ae08:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0006
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0007
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0008
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0236
	.size data_0201ae08, 30

; model script
	.global data_0201ae26
	.type data_0201ae26, @object
data_0201ae26:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0009
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x000a
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x000b
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0227
	.size data_0201ae26, 30

; model script
	.global data_0201ae44
	.type data_0201ae44, @object
data_0201ae44:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0df4
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0df6
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0df5
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x113f
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x000c
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x000d
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x000e
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0217
	.size data_0201ae44, 32

; model script
	.global data_0201ae64
	.type data_0201ae64, @object
data_0201ae64:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x000f
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0010
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0011
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0208
	.size data_0201ae64, 30

; model script
	.global data_0201ae82
	.type data_0201ae82, @object
data_0201ae82:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0dff
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e01
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e00
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0012
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0013
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0014
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x01f9
	.size data_0201ae82, 30

; model script
	.global data_0201aea0
	.type data_0201aea0, @object
data_0201aea0:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0015
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0016
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0017
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x01ea
	.size data_0201aea0, 30

; model script
	.global data_0201aebe
	.type data_0201aebe, @object
data_0201aebe:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0018
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0019
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x001a
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x01db
	.size data_0201aebe, 30

; model script
	.global data_0201aedc
	.type data_0201aedc, @object
data_0201aedc:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e0a
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e0c
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e0b
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x1145
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x001b
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x001c
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x001d
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x01cb
	.size data_0201aedc, 32

; model script
	.global data_0201aefc
	.type data_0201aefc, @object
data_0201aefc:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x001e
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x001f
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0020
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x01bc
	.size data_0201aefc, 30

; model script
	.global data_0201af1a
	.type data_0201af1a, @object
data_0201af1a:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e15
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e17
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e16
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0021
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0022
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0023
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x01ad
	.size data_0201af1a, 30

; model script
	.global data_0201af38
	.type data_0201af38, @object
data_0201af38:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0024
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0025
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0026
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x019e
	.size data_0201af38, 30

; model script
	.global data_0201af56
	.type data_0201af56, @object
data_0201af56:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0de9
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0deb
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0dea
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0027
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0028
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0029
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x018f
	.size data_0201af56, 30

; model script
	.global data_0201af74
	.type data_0201af74, @object
data_0201af74:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e20
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e22
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e21
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x002a
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x002b
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x002c
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0180
	.size data_0201af74, 30

; model script
	.global data_0201af92
	.type data_0201af92, @object
data_0201af92:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e2b
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e2d
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e2c
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x1150
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x002d
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x002e
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x002f
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0170
	.size data_0201af92, 32

; model script
	.global data_0201afb2
	.type data_0201afb2, @object
data_0201afb2:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e36
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e38
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e37
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x1156
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0030
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0031
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0032
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0160
	.size data_0201afb2, 32

; model script
	.global data_0201afd2
	.type data_0201afd2, @object
data_0201afd2:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e41
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e43
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e42
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x115c
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0033
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0034
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0035
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0150
	.size data_0201afd2, 32

; model script
	.global data_0201aff2
	.type data_0201aff2, @object
data_0201aff2:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0efc
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0efe
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0efd
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0036
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0037
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0038
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0141
	.size data_0201aff2, 30

; model script
	.global data_0201b010
	.type data_0201b010, @object
data_0201b010:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e57
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e59
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e58
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x1162
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0039
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x003a
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x003b
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0131
	.size data_0201b010, 32

; model script
	.global data_0201b030
	.type data_0201b030, @object
data_0201b030:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e8e
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e90
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e8f
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x003c
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x003d
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x003e
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0122
	.size data_0201b030, 30

; model script
	.global data_0201b04e
	.type data_0201b04e, @object
data_0201b04e:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e99
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e9b
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e9a
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x1168
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x003f
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0040
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0041
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0112
	.size data_0201b04e, 32

; model script
	.global data_0201b06e
	.type data_0201b06e, @object
data_0201b06e:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ea4
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0ea6
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ea5
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0042
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0043
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0044
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0103
	.size data_0201b06e, 30

; model script
	.global data_0201b08c
	.type data_0201b08c, @object
data_0201b08c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0eaf
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0eb1
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0eb0
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0045
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0046
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0047
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00f4
	.size data_0201b08c, 30

; model script
	.global data_0201b0aa
	.type data_0201b0aa, @object
data_0201b0aa:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0eba
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0ebc
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ebb
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0048
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0049
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x004a
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00e5
	.size data_0201b0aa, 30

; model script
	.global data_0201b0c8
	.type data_0201b0c8, @object
data_0201b0c8:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ec5
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0ec7
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ec6
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x004b
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x004c
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x004d
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00d6
	.size data_0201b0c8, 30

; model script
	.global data_0201b0e6
	.type data_0201b0e6, @object
data_0201b0e6:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ed0
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0ed2
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ed1
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x004e
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x004f
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0050
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00c7
	.size data_0201b0e6, 30

; model script
	.global data_0201b104
	.type data_0201b104, @object
data_0201b104:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0edb
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0edd
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0edc
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x116e
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0051
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0052
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0053
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00b7
	.size data_0201b104, 32

; model script
	.global data_0201b124
	.type data_0201b124, @object
data_0201b124:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ee6
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0ee8
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ee7
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0054
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0055
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0056
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00a8
	.size data_0201b124, 30

; model script
	.global data_0201b142
	.type data_0201b142, @object
data_0201b142:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ef1
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0ef3
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ef2
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0057
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0058
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0059
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0099
	.size data_0201b142, 30

; model script
	.global data_0201b160
	.type data_0201b160, @object
data_0201b160:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0f07
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0f09
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0f08
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x005a
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x005b
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x005c
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x008a
	.size data_0201b160, 30

; model script
	.global data_0201b17e
	.type data_0201b17e, @object
data_0201b17e:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0f12
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0f14
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0f13
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x005d
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x005e
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x005f
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x007b
	.size data_0201b17e, 30

; model script
	.global data_0201b19c
	.type data_0201b19c, @object
data_0201b19c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0f1d
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0f1f
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0f1e
	MODEL_OP MC_SET, MM_VALUE, 6
	.hword 0x1174
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0060
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0061
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0062
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x006b
	.size data_0201b19c, 32

; model script
	.global data_0201b1bc
	.type data_0201b1bc, @object
data_0201b1bc:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0f28
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0f2a
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0f29
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0063
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0064
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0065
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x005c
	.size data_0201b1bc, 30

; model script
	.global data_0201b1da
	.type data_0201b1da, @object
data_0201b1da:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0f33
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0f35
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0f34
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0066
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0067
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0068
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x004d
	.size data_0201b1da, 30

; model script
	.global data_0201b1f8
	.type data_0201b1f8, @object
data_0201b1f8:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0f3e
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0f40
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0f3f
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0069
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x006a
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x006b
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x003e
	.size data_0201b1f8, 30

; model script
	.global data_0201b216
	.type data_0201b216, @object
data_0201b216:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e62
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e64
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e63
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x006c
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x006d
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x006e
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x002f
	.size data_0201b216, 30

; model script
	.global data_0201b234
	.type data_0201b234, @object
data_0201b234:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e6d
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e6f
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e6e
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x006f
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0070
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0071
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0020
	.size data_0201b234, 30

; model script
	.global data_0201b252
	.type data_0201b252, @object
data_0201b252:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e78
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e7a
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e79
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0072
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0073
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0074
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0011
	.size data_0201b252, 30

; model script
	.global data_0201b270
	.type data_0201b270, @object
data_0201b270:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0e83
	MODEL_OP MC_SET, MM_VALUE, 1
	.hword 0x0e85
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0e84
	MODEL_OP MC_SET, MM_BITS, 6
	MODEL_OP MC_SET, MM_VALUE, 8
	.hword 0x0075
	MODEL_OP MC_SET, MM_VALUE, 9
	.hword 0x0076
	MODEL_OP MC_SET, MM_VALUE, 10
	.hword 0x0077
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0002
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x0013, 0x0100
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x001a, 0x0201
	MODEL_OP MC_EVENT, MM_BITS, 2
	.hword 0x0027, 0x0102
	MODEL_OP MC_EVENT, MM_BITS, 3
	.hword 0x0031, 0x0203
	MODEL_OP MC_EVENT, MM_BITS, 4
	.hword 0x003d, 0x0210
	MODEL_OP MC_SET, MM_BITS, 4
	MODEL_OP MC_SET, MM_BITS, -17
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x001c
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_SET, MM_BITS, 47
	MODEL_OP MC_SET, MM_REG, 7
	.hword 0x0008
	MODEL_OP MC_SET, MM_REG, 0
	.hword 0x0001
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0002
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0201
	MODEL_OP MC_SET, MM_BITS, 79
	MODEL_OP MC_SET, MM_REG, 7
	.hword 0x000a
	MODEL_OP MC_SET, MM_BITS, 68
	MODEL_OP MC_SET, MM_REG, 0
	.hword 0x0001
	MODEL_OP MC_ADD, MM_BITS, 64
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_BITS, 67
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0102
	MODEL_OP MC_SET, MM_REG, 0
	.hword 0x0001
	MODEL_OP MC_ADD, MM_VALUE, 0
	.hword 0x0004
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_VALUE, 3
	.hword 0x0004
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0203
	MODEL_OP MC_SET, MM_BITS, 111
	MODEL_OP MC_SET, MM_REG, 0
	.hword 0x0001
	MODEL_OP MC_ADD, MM_VALUE, 0
	.hword 0x0006
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_VALUE, 3
	.hword 0x0006
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -27
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0210
	MODEL_OP MC_SET, MM_BITS, 15
	MODEL_OP MC_SET, MM_REG, 7
	.hword 0x0009
	MODEL_OP MC_SET, MM_BITS, 36
	MODEL_OP MC_SET, MM_REG, 0
	.hword 0x0001
	MODEL_OP MC_ADD, MM_VALUE, 0
	.hword 0x0008
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_VALUE, 3
	.hword 0x0008
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201b270, 212

; model script
	.global data_0201b344
	.type data_0201b344, @object
data_0201b344:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0fd8
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0fd9
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00e6
	.size data_0201b344, 12

; model script
	.global data_0201b350
	.type data_0201b350, @object
data_0201b350:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0fde
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0fdf
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00e0
	.size data_0201b350, 12

; model script
	.global data_0201b35c
	.type data_0201b35c, @object
data_0201b35c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0fe4
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0fe5
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00da
	.size data_0201b35c, 12

; model script
	.global data_0201b368
	.type data_0201b368, @object
data_0201b368:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0fea
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0feb
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00d4
	.size data_0201b368, 12

; model script
	.global data_0201b374
	.type data_0201b374, @object
data_0201b374:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ff0
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ff1
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00ce
	.size data_0201b374, 12

; model script
	.global data_0201b380
	.type data_0201b380, @object
data_0201b380:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ff6
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ff7
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00c8
	.size data_0201b380, 12

; model script
	.global data_0201b38c
	.type data_0201b38c, @object
data_0201b38c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0ffc
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x0ffd
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00c2
	.size data_0201b38c, 12

; model script
	.global data_0201b398
	.type data_0201b398, @object
data_0201b398:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1002
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1003
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00bc
	.size data_0201b398, 12

; model script
	.global data_0201b3a4
	.type data_0201b3a4, @object
data_0201b3a4:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1008
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1009
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00b6
	.size data_0201b3a4, 12

; model script
	.global data_0201b3b0
	.type data_0201b3b0, @object
data_0201b3b0:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x100e
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x100f
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00b0
	.size data_0201b3b0, 12

; model script
	.global data_0201b3bc
	.type data_0201b3bc, @object
data_0201b3bc:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1014
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1015
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00aa
	.size data_0201b3bc, 12

; model script
	.global data_0201b3c8
	.type data_0201b3c8, @object
data_0201b3c8:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x101a
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x101b
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x00a4
	.size data_0201b3c8, 12

; model script
	.global data_0201b3d4
	.type data_0201b3d4, @object
data_0201b3d4:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1020
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1021
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x009e
	.size data_0201b3d4, 12

; model script
	.global data_0201b3e0
	.type data_0201b3e0, @object
data_0201b3e0:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1026
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1027
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0098
	.size data_0201b3e0, 12

; model script
	.global data_0201b3ec
	.type data_0201b3ec, @object
data_0201b3ec:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x102c
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x102d
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0092
	.size data_0201b3ec, 12

; model script
	.global data_0201b3f8
	.type data_0201b3f8, @object
data_0201b3f8:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1032
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1033
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x008c
	.size data_0201b3f8, 12

; model script
	.global data_0201b404
	.type data_0201b404, @object
data_0201b404:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1038
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1039
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0086
	.size data_0201b404, 12

; model script
	.global data_0201b410
	.type data_0201b410, @object
data_0201b410:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x103e
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x103f
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0080
	.size data_0201b410, 12

; model script
	.global data_0201b41c
	.type data_0201b41c, @object
data_0201b41c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1044
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1045
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x007a
	.size data_0201b41c, 12

; model script
	.global data_0201b428
	.type data_0201b428, @object
data_0201b428:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x104a
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x104b
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0074
	.size data_0201b428, 12

; model script
	.global data_0201b434
	.type data_0201b434, @object
data_0201b434:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1050
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1051
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x006e
	.size data_0201b434, 12

; model script
	.global data_0201b440
	.type data_0201b440, @object
data_0201b440:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1056
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1057
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0068
	.size data_0201b440, 12

; model script
	.global data_0201b44c
	.type data_0201b44c, @object
data_0201b44c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x105c
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x105d
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0062
	.size data_0201b44c, 12

; model script
	.global data_0201b458
	.type data_0201b458, @object
data_0201b458:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1062
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1063
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x005c
	.size data_0201b458, 12

; model script
	.global data_0201b464
	.type data_0201b464, @object
data_0201b464:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1068
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1069
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0056
	.size data_0201b464, 12

; model script
	.global data_0201b470
	.type data_0201b470, @object
data_0201b470:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x106e
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x106f
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0050
	.size data_0201b470, 12

; model script
	.global data_0201b47c
	.type data_0201b47c, @object
data_0201b47c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1074
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1075
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x004a
	.size data_0201b47c, 12

; model script
	.global data_0201b488
	.type data_0201b488, @object
data_0201b488:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x107a
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x107b
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0044
	.size data_0201b488, 12

; model script
	.global data_0201b494
	.type data_0201b494, @object
data_0201b494:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1080
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1081
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x003e
	.size data_0201b494, 12

; model script
	.global data_0201b4a0
	.type data_0201b4a0, @object
data_0201b4a0:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1086
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1087
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0038
	.size data_0201b4a0, 12

; model script
	.global data_0201b4ac
	.type data_0201b4ac, @object
data_0201b4ac:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x108c
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x108d
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0032
	.size data_0201b4ac, 12

; model script
	.global data_0201b4b8
	.type data_0201b4b8, @object
data_0201b4b8:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1092
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1093
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x002c
	.size data_0201b4b8, 12

; model script
	.global data_0201b4c4
	.type data_0201b4c4, @object
data_0201b4c4:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x1098
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x1099
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0026
	.size data_0201b4c4, 12

; model script
	.global data_0201b4d0
	.type data_0201b4d0, @object
data_0201b4d0:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x109e
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x109f
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0020
	.size data_0201b4d0, 12

; model script
	.global data_0201b4dc
	.type data_0201b4dc, @object
data_0201b4dc:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x10a4
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x10a5
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x001a
	.size data_0201b4dc, 12

; model script
	.global data_0201b4e8
	.type data_0201b4e8, @object
data_0201b4e8:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x10aa
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x10ab
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0014
	.size data_0201b4e8, 12

; model script
	.global data_0201b4f4
	.type data_0201b4f4, @object
data_0201b4f4:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x10b0
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x10b1
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x000e
	.size data_0201b4f4, 12

; model script
	.global data_0201b500
	.type data_0201b500, @object
data_0201b500:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x10b6
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x10b7
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0008
	.size data_0201b500, 12

; model script
	.global data_0201b50c
	.type data_0201b50c, @object
data_0201b50c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x10bc
	MODEL_OP MC_SET, MM_VALUE, 2
	.hword 0x10bd
	MODEL_OP MC_JUMP, MM_BITS, 0
	.hword 0x0002
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x0011, 0x0100
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x001f, 0x0101
	MODEL_OP MC_EVENT, MM_BITS, 2
	.hword 0x0026, 0x0102
	MODEL_OP MC_EVENT, MM_BITS, 3
	.hword 0x0035, 0x0103
	MODEL_OP MC_JUMP, MM_VALUE, 1
	MODEL_OP MC_SET, MM_BITS, 62
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0002
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_SET, MM_BITS, 39
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_VALUE, 3
	.hword 0x0004
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_BITS, 99
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -20
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0101
	MODEL_OP MC_SET, MM_BITS, 71
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_BITS, 35
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -30
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0102
	MODEL_OP MC_SET, MM_BITS, 39
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_VALUE, 3
	.hword 0x0004
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_SET, MM_BITS, 71
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_BITS, 35
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -48
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0103
	MODEL_OP MC_SET, MM_BITS, 39
	MODEL_OP MC_SET, MM_REG, 3
	.hword 0x0002
	MODEL_OP MC_ADD, MM_VALUE, 3
	.hword 0x0004
	MODEL_OP MC_ANIM, MM_REG, 2
	.hword 0x0003
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -59
	.size data_0201b50c, 158

; model script
	.global data_0201b5aa
	.type data_0201b5aa, @object
data_0201b5aa:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c7a
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x0008, 0x0100
	MODEL_OP MC_JUMP, MM_VALUE, 1
	MODEL_OP MC_SET, MM_BITS, 62
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0c79
	MODEL_OP MC_WAIT, MM_VALUE, 32
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c7c
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0100
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x0c7b
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -17
	.size data_0201b5aa, 36

; model script
	.global data_0201b5ce
	.type data_0201b5ce, @object
data_0201b5ce:
	MODEL_OP MC_SET, MM_BITS, 28
	MODEL_OP MC_FLAG, MM_BITS, 0
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x000c, 0x0201
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0011, 0x0300
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0200
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c82
	MODEL_OP MC_SET, MM_BITS, 23
	MODEL_OP MC_STEP, MM_BITS, 0
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0201
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c86
	MODEL_OP MC_ANIM, MM_VALUE, 1
	.hword 0x0c85
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -13
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0300
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c84
	MODEL_OP MC_ANIM, MM_VALUE, 1
	.hword 0x0c83
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_SET, MM_BITS, 29
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201b5ce, 62

; model script
	.global data_0201b60c
	.type data_0201b60c, @object
data_0201b60c:
	MODEL_OP MC_SET, MM_BITS, 28
	MODEL_OP MC_FLAG, MM_BITS, 0
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x000c, 0x0201
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x0011, 0x0300
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0200
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c7d
	MODEL_OP MC_SET, MM_BITS, 23
	MODEL_OP MC_STEP, MM_BITS, 0
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0201
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c81
	MODEL_OP MC_ANIM, MM_VALUE, 1
	.hword 0x0c80
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -13
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0300
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x0c7f
	MODEL_OP MC_ANIM, MM_VALUE, 1
	.hword 0x0c7e
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_SET, MM_BITS, 29
	MODEL_OP MC_STEP, MM_BITS, 0
	.size data_0201b60c, 62

; model script
	.global data_0201b64a
	.type data_0201b64a, @object
data_0201b64a:
	MODEL_OP MC_SET, MM_BITS, 28
	MODEL_OP MC_FLAG, MM_BITS, 0
	MODEL_OP MC_EVENT, MM_BITS, 0
	.hword 0x000b, 0x0300
	MODEL_OP MC_EVENT, MM_BITS, 1
	.hword 0x000e, 0x0400
	MODEL_OP MC_SET, MM_BITS, 62
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x01d4
	MODEL_OP MC_SET, MM_BITS, 23
	MODEL_OP MC_STEP, MM_BITS, 0
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0300
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x01d6
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, 1
	MODEL_OP MC_SET, MM_VALUE, 30
	.hword 0x0400
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x01d5
	MODEL_OP MC_WAIT_OR_IDLE, MM_VALUE, 32
	MODEL_OP MC_JUMP, MM_VALUE, -16
	.size data_0201b64a, 50

; model script
	.global data_0201b67c
	.type data_0201b67c, @object
data_0201b67c:
	MODEL_OP MC_SET, MM_VALUE, 29
	.hword 0x01d8
	MODEL_OP MC_JUMP, MM_VALUE, 1
	MODEL_OP MC_SET, MM_BITS, 62
	MODEL_OP MC_ANIM, MM_VALUE, 2
	.hword 0x01d7
	MODEL_OP MC_WAIT, MM_VALUE, 32
	.size data_0201b67c, 14

; sprite script
	.global data_0201b68a
	.type data_0201b68a, @object
data_0201b68a:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0ed5
	.hword 0x0ed4
	.hword 0x0ed3
	.hword 0x0ed2
	.hword 0x0ed1
	.hword 0x0ed0
	.hword 0x0ecf
	.hword 0x0ece
	.hword 0x0ecd
	.hword 0x0ecc
	.hword 0x0ecb
	.hword 0xfffd
	.size data_0201b68a, 28

; sprite script
	.global data_0201b6a6
	.type data_0201b6a6, @object
data_0201b6a6:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0e29
	.hword 0x0e2a
	.hword 0x0e2b
	.hword 0x0e2c
	.hword 0x0e2d
	.hword 0x0e2e
	.hword 0x0e2f
	.hword 0x0e30
	.hword 0x0e31
	.hword 0x0e32
	.hword 0x0e33
	.hword 0x0e34
	.hword 0x0e35
	.hword 0x0e36
	.hword 0x0e37
	.hword 0x0e38
	.hword 0xfffd
	.size data_0201b6a6, 38

; sprite script
	.global data_0201b6cc
	.type data_0201b6cc, @object
data_0201b6cc:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x1198
	.hword 0x1199
	.hword 0x119a
	.hword 0x119b
	.hword 0x119c
	.hword 0x119d
	.hword 0x119e
	.hword 0x119f
	.hword SPRITE_CMD_JUMP, 0xffea
	.size data_0201b6cc, 24

; sprite script
	.global data_0201b6e4
	.type data_0201b6e4, @object
data_0201b6e4:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d7d
	.hword 0x0d7e
	.hword 0x0d7f
	.hword 0x0d80
	.hword 0x0d81
	.hword 0x0d82
	.hword 0x0d83
	.hword 0x0d84
	.hword 0x0d85
	.hword 0x0d86
	.hword 0x0d87
	.hword 0x0d88
	.hword 0x0d89
	.hword 0x0d8a
	.hword 0x0d8b
	.hword 0x0d8c
	.hword 0x0d8d
	.hword 0x0d8e
	.hword 0x0d8f
	.hword 0x0d90
	.hword 0xfffd
	.size data_0201b6e4, 46

; sprite script
	.global data_0201b712
	.type data_0201b712, @object
data_0201b712:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d2e
	.hword 0x0d2f
	.hword 0x0d30
	.hword 0x0d31
	.hword 0x0d32
	.hword 0x0d33
	.hword 0x0d34
	.hword 0x0d35
	.hword 0x0d36
	.hword 0x0d37
	.hword 0xfffd
	.size data_0201b712, 26

; sprite script
	.global data_0201b72c
	.type data_0201b72c, @object
data_0201b72c:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d38
	.hword 0x0d39
	.hword 0x0d3a
	.hword 0x0d3b
	.hword 0x0d3c
	.hword 0x0d3d
	.hword 0x0d3e
	.hword 0x0d3f
	.hword 0x0d40
	.hword 0x0d41
	.hword 0xfffd
	.size data_0201b72c, 26

; sprite script
	.global data_0201b746
	.type data_0201b746, @object
data_0201b746:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d42
	.hword 0x0d43
	.hword 0x0d44
	.hword 0x0d45
	.hword 0x0d46
	.hword 0x0d47
	.hword 0x0d48
	.hword 0x0d49
	.hword 0x0d4a
	.hword 0x0d4b
	.hword 0xfffd
	.size data_0201b746, 26

; sprite script
	.global data_0201b760
	.type data_0201b760, @object
data_0201b760:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d4c
	.hword 0x0d4d
	.hword 0x0d4e
	.hword 0x0d4f
	.hword 0x0d50
	.hword 0x0d51
	.hword 0x0d52
	.hword 0x0d53
	.hword 0x0d54
	.hword 0x0d55
	.hword 0xfffd
	.size data_0201b760, 26

; sprite script
	.global data_0201b77a
	.type data_0201b77a, @object
data_0201b77a:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d56
	.hword 0x0d57
	.hword 0x0d58
	.hword 0x0d59
	.hword 0x0d5a
	.hword 0x0d5b
	.hword 0x0d5c
	.hword 0x0d5d
	.hword 0x0d5e
	.hword 0x0d5f
	.hword 0xfffd
	.size data_0201b77a, 26

; sprite script
	.global data_0201b794
	.type data_0201b794, @object
data_0201b794:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0d60
	.hword 0x0d61
	.hword 0x0d62
	.hword 0x0d63
	.hword 0x0d64
	.hword 0x0d65
	.hword 0x0d66
	.hword 0x0d67
	.hword 0x0d68
	.hword 0x0d69
	.hword 0xfffd
	.size data_0201b794, 26

; sprite script
	.global data_0201b7ae
	.type data_0201b7ae, @object
data_0201b7ae:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0fb7
	.hword 0x0fb8
	.hword 0x0fb9
	.hword 0x0fba
	.hword 0x0fbb
	.hword 0x0fbc
	.hword 0x0fbd
	.hword 0x0fbe
	.hword 0x0fbf
	.hword 0x0fc0
	.hword 0x0fc1
	.hword 0x0fc2
	.hword 0x0fc3
	.hword 0x0fc4
	.hword 0x0fc5
	.hword 0xfffd
	.size data_0201b7ae, 36

; sprite script
	.global data_0201b7d2
	.type data_0201b7d2, @object
data_0201b7d2:
	.hword SPRITE_CMD_SET_SPEED, 0x0002
	.hword 0x0fc6
	.hword 0x0fc7
	.hword 0x0fc8
	.hword 0x0fc9
	.hword 0xfffd
	.size data_0201b7d2, 14

