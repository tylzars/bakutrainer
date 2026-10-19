; Parameter block read by the start-up code: where the autoload list and the BSS are, followed by a version
; word, a marker pair and two library identification strings.
; Data only, no code: nothing here is a routine the compiler could have emitted.

	.text
	.global BuildInfo
	.extern data_02064c40
	.extern data_02064c58
	.extern ARM9_BSS_START

	.type BuildInfo, @object
BuildInfo:
	.word data_02064c40     ; autoload list: start
	.word data_02064c58     ; autoload list: end
	.word ARM9_BSS_START    ; first autoload source
	.word ARM9_BSS_START    ; BSS start
	.word 0x020ce200        ; BSS end
	.word 0                 ; end of the compressed part (0 in the unpacked image)
	.word 0x04027539        ; version word
	.word 0xdec00621        ; marker
	.word 0x2106c0de        ; marker, byte-reversed
	.size BuildInfo, 36

	; Library identification strings, each passed to the start-up hook by the library that carries it.
	.global data_02000bc4
	.type data_02000bc4, @object
data_02000bc4:
	.byte "[SDK+Actimagine:Mobiclip SDK V1.0.3]", 0, 0, 0, 0
	.size data_02000bc4, 40
	.global data_02000bec
	.type data_02000bec, @object
data_02000bec:
	.byte "[SDK+NINTENDO:BACKUP]", 0, 0, 0
	.size data_02000bec, 24
