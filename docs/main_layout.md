# Main module layout

What lives where in the ARM9 main module (0x02000000-0x020ce200; no overlays), from this ROM's code, relocations
and the exception index. Each line says what shows it; anything not yet checked is listed under "Open".

## Sections (config/usa/arm9/delinks.txt)

| Range | Section | Note |
|---|---|---|
| 0x02000000-0x0205eeac | `.text` | dsd's analysis; compiled code really ends at 0x0204df14 (below) |
| 0x0205eeac-0x020638fc | `.rodata` | |
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

The decoder is hand-written code inside the original's read-only data, so it links from the original bytes and is
not counted as code. Until a unit ends at 0x0204df14, `.text` has to run on past it to 0x0205eeac (see
docs/decompiling.md, "Setting up a new game": routines in `.rodata`).

## Read-only data after the code

| Range | Contents | Evidence |
|---|---|---|
| 0x0204df1c-0x0204e054 | Exception index: 26 entries {function, size, info} for C runtime functions | the entries point at function starts and give their sizes; four functions dsd had missed or cut short were fixed from it |
| 0x0204e054-0x02052468 | Tables, among them the script command keywords (`LET`, `SPR`, `GOTO`, `IF`, `MUSIC`, `FADE`, ...) | no relocation points into this range yet (open) |
| 0x0205eeac-0x020638fc | Data referenced from game, glue and library code | relocations |

## Open

- 0x0204df14-0x0204df1c: two words (0x6009ff20, 0) before the exception index.
- 0x0204e054-0x02052468: which code reads these tables (no relocation reaches them).
- Function boundaries in the library range where dsd's functions cover only part of a file's bytes (FS, CARD, WM),
  and the code at 0x020481bc-0x0204873c that dsd didn't analyse.
- Unit (file) boundaries for game code.
