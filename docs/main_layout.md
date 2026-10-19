# Main module layout

What lives where in the ARM9 main module (0x02000000-0x020ce200; no overlays), from this ROM's code, relocations
and the exception index. Each line says what shows it; anything not yet checked is listed under "Open".

## Sections (config/usa/arm9/delinks.txt)

| Range | Section | Note |
|---|---|---|
| 0x02000000-0x0204df14 | `.text` | compiled code ends here: the exception index, then the game's tables, follow |
| 0x0204df14-0x020638fc | `.rodata` | the game's tables in link order, the Mobiclip decoder, then the library tables |
| 0x020638fc | `.ctor` | empty |
| 0x02063900-0x02064820 | `.data` | |
| 0x02064820-0x020ce200 | `.bss` | |

## Code

| Range | Contents | Evidence |
|---|---|---|
| 0x02000000-0x02000800 | Secure area: 18 BIOS call thunks (Thumb) between filler | dsd names the thunks (`SoftReset`, `CpuSet`, ...) |
| 0x02000800-0x02000ba0 | Start-up code (`Entry` and its helpers) | ARM; `func_02000970`, `func_02000a1c` and `func_02000ab0` match NitroSDK start-up code of the same SDK version |
| 0x02001000-0x020011dc | Game code: four ARM functions | |
| 0x020011dc-0x02003000 | Zero filler | |
| 0x02003000-0x02039dd8 | Game code, from `main` | Thumb for the most part with ARM runs; calls between the modes in both directions |
| 0x02039dd8-0x0203b8bc | Mobiclip glue code (ARM, C++) | the only code that references the Mobiclip decoder (docs/compiler.md); starts with the routine that passes the decoder's identification string to the start-up hook |
| 0x0203b8d8-0x0204873c | NitroSDK (FX, GX, OS, MI, SND, PXI, FS, TP, MIC, PM, RTC, CARD, WM, CTRDG) | functions identical to NitroSDK code of the same SDK version |
| 0x0204873c-0x0204df14 | C runtime: strings, printf, math, decimal conversion, soft float and division, the constructor walk | as above; the last function, `func_0204dee8`, walks the `.ctor` table |
| 0x02052468-0x0205eeac | Mobiclip decoder: hand-written ARM routines mixed with their tables | referenced only from itself and the glue code |

The decoder is hand-written code inside the original's read-only data: after the game's tables and before the
library's, so the linker placed it with `.rodata`. Its assembler files put their code in a code-type section named
`.rodata` (`.section .rodata,.text`), and objdiff does not count it as code.

Two 4-byte routines in that range are compiled C left to the gaps, since a compiled function can't be placed in
`.rodata`: func_02052468 (`int func_02052468(Chan *ch) { return func_02052c78(ch); }`) and func_02052e28
(`void func_02052e28(Chan *ch) { func_02053834(ch); }`), each built with `#pragma interworking off`, which turns the
call into a plain `b` to the decoder routine after it.

## Read-only data after the code

| Range | Contents | Evidence |
|---|---|---|
| 0x0204df1c-0x0204e054 | Exception index: 26 entries {function, size, info} for C runtime functions | the entries point at function starts and give their sizes; four functions dsd had missed or cut short were fixed from it |
| 0x0204e054-0x02052468 | Tables, among them the script command keywords (`LET`, `SPR`, `GOTO`, `IF`, `MUSIC`, `FADE`, ...) | relocations from game code (pool words in the decompiled units) |
| 0x0205eeac-0x020638fc | Data referenced from game, glue and library code | relocations |

## Open

- 0x0204df14-0x0204df1c: two words (0x6009ff20, 0) before the exception index.
- Function boundaries in the library range where dsd's functions cover only part of a file's bytes (FS, CARD, WM),
  and the code at 0x020481bc-0x0204873c that dsd didn't analyse.
- Unit (file) boundaries for game code.
