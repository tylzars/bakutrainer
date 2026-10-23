# Main module layout

What lives where in the ARM9 main module (0x02000000-0x020ce200; the game has no overlays), worked out from this
ROM's code, relocations and exception index. Each line says what shows it; what is still unsure is under "Open".

## Sections (config/usa/arm9/delinks.txt)

| Range | Section | Note |
|---|---|---|
| 0x02000000-0x0204df14 | `.text` | compiled code ends here; the exception index and then the game's tables follow |
| 0x0204df14-0x0204df1c | `.exception` | printf's long exception record (for `func_0204a5c8`) |
| 0x0204df1c-0x0204e054 | `.exceptix` | the exception index, built by the linker from the objects in link order |
| 0x0204e054-0x020638fc | `.rodata` | the game's tables in link order, the Mobiclip decoder, then the library tables |
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
| 0x02039dd8-0x0203b8bc | Mobiclip glue code (ARM, C++) | the only code that references the Mobiclip decoder ([compiler.md](compiler.md#mobiclip-glue-code)); its first routine passes the decoder's identification string to the start-up hook |
| 0x0203b8bc-0x02048b3c | NitroSDK (FX, GX, OS, MI, SND, PXI, FS, CP, TP, MIC, PM, RTC, CARD, WM, CTRDG, MATH) | the SDK's public API; the build-info block gives the SDK version |
| 0x02048b3c-0x0204df14 | C runtime: strings, printf, math, decimal conversion, soft float and division, the constructor walk | the runtime's public names; the last function, `func_0204dee8`, walks the `.ctor` table |
| 0x02052468-0x0205f33c | Mobiclip decoder: hand-written ARM routines mixed with their tables | referenced only from itself and the glue code |

The decoder sits in the read-only data, after the game's tables and before the libraries'. It is not in the
source: the build links it from your ROM, like every range no unit covers. Two 4-byte routines in that range are
compiled C (`func_02052468` and `func_02052e28`, each a plain `b` to the decoder routine after it, as
`#pragma interworking off` gives). They link from the ROM with the decoder, because a compiled function can't be
placed in `.rodata`.

## Read-only data after the code

| Range | Contents | Evidence |
|---|---|---|
| 0x0204df1c-0x0204e054 | Exception index: 26 entries {function, size, info} for C runtime functions | the entries point at function starts and give their sizes |
| 0x0204e054-0x02052468 | Tables, among them the script command keywords (`LET`, `SPR`, `GOTO`, `IF`, `MUSIC`, `FADE`, ...) | relocations from game code (pool words in the decompiled units) |
| 0x0205f33c-0x020638fc | Data referenced from game, glue and library code | relocations |

## Open

- Where some files begin and end, and some file names: the notes marked `provisional` in
  `config/usa/arm9/delinks.txt` say what is unsure about each.
