; Parameter block read by the start-up code: where the autoload list and the BSS are, followed by a version
; word, a marker pair and two library identification strings.

	.text
	.global BuildInfo
	.extern data_02064c40
	.extern data_02064c58
	.extern data_02064820

	.type BuildInfo, @object
BuildInfo:
	.word data_02064c40     ; autoload list: start
	.word data_02064c58     ; autoload list: end
	.word data_02064820     ; first autoload source
	.word data_02064820     ; BSS start
	.word 0x020ce200        ; BSS end
	.word 0                 ; end of the compressed part (0 in the unpacked image)
	.word 0x04027539        ; version word
	.word 0xdec00621        ; marker
	.word 0x2106c0de        ; marker, byte-reversed
	.byte "[SDK+Actimagine:Mobiclip SDK V1.0.3]", 0, 0, 0, 0
	.byte "[SDK+NINTENDO:BACKUP]", 0, 0, 0
	.size BuildInfo, 100
