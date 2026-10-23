# Compiler identification

How the compiler settings in `tools/project.py` were worked out. Each claim comes with its evidence and how to
reproduce it. A setting that no exact match depends on is marked "not separated".

## Result

| Code | Compiler | Flags | Evidence |
|---|---|---|---|
| Game code (main; most of it Thumb) | CodeWarrior for DS 2.0 **sp2p3** (sp2p2 or later shown; sp2p3 rests on one assumption) | `-O4,p -ipa file -inline on,noauto -enum int -char signed -str reuse`, C; `-thumb` for Thumb code | 26 probe functions, below |
| NitroSDK | 2.0 sp1 ... sp2p2 give the same bytes; built with **sp1p5** | `-O4,p -ipa file -enum int`, C, default inlining | 313 functions, below |
| C runtime (MSL) | same build as NitroSDK | NitroSDK's flags with `-char signed -Cpp_exceptions on` | 52 functions, below |
| Mobiclip glue code | as the game | as the game | below |

`tools/project.py` sets these through `MWCC_VERSION`, `CC_FLAGS`, `PATH_COMPILERS` and `PATH_DROP_FLAGS` (for
`libs/nds/` and `libs/msl/`) and `PATH_ADD_FLAGS` (`-thumb` for `src/`).

## Tools

- `tools/probe_compilers.py <probe>`: compiles a hand-written function with every installed compiler build and
  compares it byte for byte with the ROM, masking only the function's own relocation sites. Mark each function in
  the probe with `// PROBE: <symbol> <module> <address>`. `--only` and `--flags` narrow a run, `--show` prints a
  side-by-side diff, and `--unit <source path>` uses exactly the build's compiler and flags for that path. Each
  `--flags` set must spell out `-O`, `-inline` and `-ipa`.
- `tools/sp_differential.py`: compiles an outside codebase that already matches with every 2.0 build and lists the
  functions whose bytes after linking differ between builds. This separates service packs that probes can't.
- `tools/match_external.py`: finds exact matches between those outside objects and this ROM's functions. It goes by
  this project's function sizes, so it misses functions whose boundaries are wrong.
- `tools/longmul_scan.py`: counts the pre-sp2p2 64-bit multiply idiom in disassembly.
- `tools/flag_effect.py "<flag>" "<other>" [paths]`: compiles every file with its build settings and again with one
  flag replaced or removed, and lists the files whose objects change. A flag no file depends on is "not separated".
  A file that changes, while the build's setting links and matches, is evidence for that setting.

## Method

1. Write about 20 real functions as stand-alone probe files, spread across the code and its constructs (soft float,
   64-bit fixed point, switch tables, register-heavy loops, SDK struct access, string tables).
2. Compile each with every installed build and each candidate flag set, and count exact matches. A miss that fails
   the same way on every build is a detail of the source, not a sign of the compiler.
3. If neighbouring service packs give the same bytes on the probes, compile a large outside codebase that already
   matches with every build and diff each function's bytes after linking (`tools/sp_differential.py`). Then find
   exact matches between those objects and this ROM's functions (shared SDK code) and note which builds give them.
4. Find a codegen idiom that separates the remaining builds and scan the ROM's disassembly for it. Check the scanner
   first on code whose build is known.
5. For each flag, record an exact match that breaks when the flag changes.
6. Identify each library on its own: the SDK, the C runtime and middleware are often prebuilt with another build and
   other flags (often the default inlining instead of the game's `-inline on,noauto`). Count only the game's own
   functions as evidence for the game's build. One library can need different builds for different parts. Set
   `PATH_COMPILERS` and `PATH_DROP_FLAGS` from the result.
7. Don't commit the probes: the method and the evidence are recorded here.

## What separates builds and flags

- Idioms: a 64x32 multiply whose high word is known to be zero gives `umull` + `mla` against a zeroed register up to
  2.0 sp2, and only `umull` from sp2p2 on. Whole functions also differ by a few words between service packs.
- `-O` level: `-O3,p` allocates registers differently; `-O4,s` and `-O2` keep small accessors out of line.
- `-ipa file`: statics of inline functions become shared sections of their own, referenced from every module;
  without it they land in each file's `.bss`. In C under `-ipa file`, a function can load a global defined in its own
  file from two literal-pool words (one through the section, one through the symbol). With the global `extern`, or
  without `-ipa file`, it uses one. So a duplicate pool word in the original shows the global is defined in that
  file.
- 2.0 sp2p2 vs sp2p3 (under `-ipa file`): sp2p3 and sp2p4 give every object of 64 bytes or more defined in the file a
  section of its own; sp2p2 and earlier keep the file's objects in one section. Accesses at constant offsets go
  through the section. So an object under 64 bytes, used both by member and by address in one function, takes two
  pool words on every build (which shows it is defined in that file). A bigger one takes two up to sp2p2 and one from
  sp2p3 (the same code as an `extern` object). Without `-ipa file`, C uses the symbol and C++ the section. sp2p3 and
  sp2p4 gave the same code on every comparison. Most duplicate pool words found from relocations are just two Thumb
  literal pools: check that both words sit in one pool.
- `-str reuse` vs `noreuse`: with `reuse`, identical literals in one file share one copy. One copy used by several
  functions shows `reuse` (literals are local to their file).
- `-inline on,noauto` vs `auto`: calls within one file stay `bl`.
- `-enum int` vs `min`: struct field offsets after enum-typed members.
- `-char signed`: `ldrsb` when walking a plain `char` table.
- `-Cpp_exceptions off`: the code of a `new` expression with a constructor (in the Mobiclip glue code).
- `-RTTI off`: no typeinfo slot before vtables.
- `-interworking`: leaf returns are `bx lr`, never `mov pc, lr`.
- `-proc arm946e`: DSP instructions (`smulbb`) present; `arm9tdmi` lacks them.
- Host: switch-table density exactly at the 0.4 threshold needs Windows' 53-bit x87 precision (the pinned wibo).

Probing tips: `tools/probe_compilers.py` strips `-O`, `-inline` and `-ipa` from the base flags because it varies
them, so spell them out (`--flags="-O4,p -ipa file -inline on,noauto"`). It adds `-thumb` when every probed function
is Thumb, and masks relocations per byte (a Thumb `bl` pair can straddle a word). Check it first on one known match
and one deliberate compile error.

## Evidence (this game)

The probes are C written for this ROM's functions, compiled with every installed build (1.2: 6 builds, 2.0: 10,
DSi: 9) and compared byte for byte with relocated bytes masked. They are not part of the repository (method step 7).
The tool was checked first: a library function whose C is known matches on every 2.0 build, and the same function
with one constant changed misses on every build.

### Compiler family

| Builds | Thumb game probes (16 functions) | ARM game probes (10 functions, glue code excluded) |
|---|---|---|
| 1.2 (all 6) | 0 | 3 (functions of 1 to 6 instructions) |
| 2.0 base ... sp2p4 (each) | 15 | 9 |
| DSi (9 builds) | 6-8 | 4 |

1.2 has no `-ipa file`, so it was compiled without it. The Thumb miss on 2.0 is `EaseAngleToward`: 8 words differ
the same way on every build and flag set (register choice, which follows the source's shape). The ARM miss is
`StartVideo`: 4 words differ on sp2p2 ... sp2p4 (more on earlier builds, below).

### Which 2.0 service pack (game)

- **sp2p2 or later: shown.** A 64-bit value multiplied by a zero-extended 32-bit one gives `umull` plus one `mla`
  from sp2p2 on; earlier builds add a second `mla` against a register holding 0. `tools/longmul_scan.py` finds that
  long form in the compiled `StartVideo` probe on sp1p5 and sp2 but not on sp2p2 or sp2p3, and finds none in the
  game code (96 multiplies) or the SDK code (14). On sp2 and earlier the `StartVideo` probe is 4 bytes larger;
  sp2p2 ... sp2p4 give its size. Thumb code calls a helper for 64-bit multiplies, so the idiom only exists in ARM
  code.
- **sp2p3 rather than sp2p2: matches, given one assumption.** `StopVideo` and `StartVideo` use `gVideoState` (at
  least 0xb7 bytes) both by member and by address, through one pool word each. With `-ipa file` and the object
  defined in the probe's file, sp2p2 and every earlier build add a second pool word; sp2p3 and sp2p4 give
  `StopVideo` exactly (the 64-byte section rule above). With the object `extern`, every build matches.
  So sp2p3 holds if `gVideoState` is defined in the file of those functions. Two Thumb probes with their global
  defined in the file (`func_02026268`, `WaitForLinkStep`) split the same way and rest on the same kind of
  assumption. No probe matches on sp2p2 and misses on sp2p3.
- sp2p3 and sp2p4 gave the same code on every probe: not separated, sp2p3 kept.

### Flags (game)

Each flag was changed on its own, with every 2.0 build. Counts are game functions on sp2p3, out of the 26 that match
with the build's flags (a function probed in several forms counts once; SDK and glue functions are not counted).

| Flag | Evidence |
|---|---|
| `-O4,p` | `-O4,s` and `-O2` miss 14 of 26. `-O3,p` gives the same code on every probe: not separated |
| `-ipa file` | `DrawPageViewerButtons` (Thumb), `RemoveLeastRecentCacheEntry` and `FindCachedFileSize` (ARM) load one global's address from two pool words. Written as C they match only with `-ipa file` and the global defined in the probe's file; without `-ipa file`, or with the global `extern`, every build misses. Written as C++ they match with or without `-ipa file`, so this shows `-ipa file` for C code (the game code looks like C: `.ctor` is empty, every probe matches as C) |
| `-inline on,noauto` | `-inline auto` misses 4 of 26: it inlines same-file callees the original calls with `bl` (`TestBits` into `StartFade`, `NextRandom` into `RandomRange`, and in `StopVideo` and `UpdateCommChild`). The default inlining gives the same code as `on,noauto`: not separated |
| `-char signed` | not separated by game code: no game function changes with `-char unsigned`. The game's own text code reads bytes unsigned (`ldrb`, unsigned compares), so its text is `u8`. Shown for the C runtime and the glue code (below) |
| `-str reuse` | `.data` 0x02063a2c-0x02063a50 holds one `"+%d"` and one `"-%d"` used by three functions. A linked test file with the same literal uses reproduces those 0x24 bytes exactly with `-str reuse` (sp2p2 and sp2p3); `noreuse` adds copies, `pool` packs them unaligned, and named `char` arrays come before a file's function literals, so they give another order |
| `-enum int` | `-enum min` gives the same code on every probe: not separated |
| `-proc arm946e` | `-proc arm9tdmi` misses 18 of 26 |
| `-interworking` | without it 3 ARM functions miss (`AreScreensOff`, `ConnectCommAsChild`, `func_02034cdc`: returns and tail calls); no Thumb probe misses without it |
| `-thumb` | Thumb functions compile only with it. `#pragma thumb off` ... `#pragma thumb on` around a function gives the code of compiling it without `-thumb` |

### Mobiclip glue code

The ARM functions at 0x02039e08-0x0203b8bc are the only code whose relocations reach the Mobiclip decoder
(0x02052468, the `[SDK+Actimagine:Mobiclip SDK V1.0.3]` code). They are middleware, so they are not evidence for the
game's own build. Their two probes, `func_0203a938` and `func_0203b600`, match on every 2.0 build with the game's
settings. `func_0203a938` tests a file-header id against `'M'`, `'N'`, `'3'` with `ldrsb`: with a plain `char` array
it matches with `-char signed` and misses with `-char unsigned` (2 words); `unsigned char` misses either way.

### NitroSDK and C runtime

365 of this ROM's library functions (313 NitroSDK, 52 C runtime; SDK version word 67269945 in the build-info block)
are written as C in `libs/`. Exact matches when compiled with every 2.0 build:

| Set | base | sp1 ... sp2p2 | sp2p3, sp2p4 |
|---|---|---|---|
| `-O4,p -ipa file -enum int` | 341 | 353 | 352 |
| the same with `-char signed` | 350 | 364 | 363 |
| without `-ipa file` | 331 | 343 | 343 |
| `-O4,s` | 236 | 246 | 245 |

- 2.0 base misses `DC_FlushAll`, `DC_FlushRange`, `OSi_WaitVCount0`, `OSi_ExceptionHandler` and others. sp2p3 and
  sp2p4 miss `SND_CommandInit` (0x02040328): its 0x1800-byte buffer gets a section of its own, so the pool word
  points past the buffer instead of past the start of the file's `.bss` (the 64-byte rule seen from the other side).
  So the libraries are sp1 ... sp2p2, which give the same bytes; the build uses sp1p5.
- `-char signed`: 11 C runtime functions in 6 files (string functions, printf, decimal conversion) match only with
  it; no NitroSDK function depends on it.
- `-inline on,noauto` changes none of them. The build drops it for these libraries, which keeps the default inlining
  (not separated from the game's setting).
- `_ull_mod` (0x0204da84) doesn't come out of C on any build or set; it is an assembly routine in `rt_soft_float_b.s`.
- `-Cpp_exceptions on` for the C runtime: the exception index at 0x0204df1c lists exactly its non-leaf functions.
  Compiled with the flag, the code is byte-identical and each `.exceptix` entry equals the ROM's entry word for word
  (e.g. `qsort` size+1 0x165, info 0x70ff00; `memset` 0x15, 0x100100); leaf functions get none, as in the ROM.
- Its C++ file (`rt_global_ctors.cpp`: `operator delete` and the start-up hooks) is Embedded C++ (`-lang=ec++`,
  `PATH_CPP_DIALECT`). As plain C++ the compiler requires `throw()` on `operator delete`, which adds an
  exception-specification frame (`push {fp, lr}`, 24-byte frame, entry 0x38/0) that the ROM lacks. As Embedded C++
  the code, the symbol `_ZdlPv` and the exception entry (0x15, 0x100000) are exactly the ROM's.

Reproduce: `python3 tools/probe_compilers.py <probe>.c --unit <source path>` (or `--only 2.0 --flags=...` to try
several builds). The library numbers come from compiling each library source file with `-O4,p -ipa file -enum int`
and the variants above.

## Open

- sp2p3 over sp2p2 rests on `gVideoState` being defined in the file of `StopVideo` and `StartVideo`.
- `-O4,p` vs `-O3,p`, `-enum int` vs `min`, and `-inline on,noauto` vs the default are not separated by any probe.
- `-ipa file` for game code holds for C; written as C++ the deciding probes match without it too.
- `-char signed` is shown for the C runtime and the Mobiclip glue code, not for the game's own code.
- `-str reuse` is shown for game code; for the libraries it is not checked.
