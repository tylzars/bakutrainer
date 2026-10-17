# Compiler identification

How the toolchain settings in `tools/project.py` are established. Record each claim here with its evidence and how
to reproduce it; a setting with no exact match that breaks when it changes is unverified and says so.

## Result

| Code | Compiler | Flags | Evidence |
|---|---|---|---|
| Game code (main + overlays) | | | |
| NitroSDK | | | |
| NitroSystem | | | |
| C runtime (MSL) | | | |

## Tools

- `tools/probe_compilers.py <probe>`: compiles a hand-decompiled function with every installed compiler build and
  compares it byte for byte with the ROM, masking only each function's own relocation sites. Mark each function in
  the probe with `// PROBE: <mangled symbol> <module> <address>`; `--only`, `--flags` and `--show` narrow a run and
  print a side-by-side diff, and `--unit <source path>` probes with exactly the build's compiler and flags for that
  path. Each `--flags` set must spell out `-O`, `-inline` and `-ipa`.
- `tools/sp_differential.py`: compiles an external, already-matching codebase with every 2.0 build and lists the
  functions whose post-link bytes differ between builds (separates service packs that probes can't).
- `tools/match_external.py`: finds exact matches between those external objects and this ROM's functions (by this
  project's function sizes, so it misses functions whose boundaries aren't right yet).
- `tools/longmul_scan.py`: counts the pre-sp2p2 64-bit multiply idiom in disassembly.
- `tools/flag_effect.py "<flag>" "<other>" [paths]`: compiles every file with its build settings and with one flag
  replaced or removed, and lists the files whose objects change. A flag no file depends on is "not separated"; files
  that change while the build's setting links and matches are evidence for it.

## Method

1. Hand-decompile about 20 real functions spread across modules and constructs (constructors, destructors, virtual
   calls, singletons, soft float, 64-bit fixed point, switch tables, register-heavy loops, SDK struct access, string
   tables) as stand-alone probe files.
2. Compile each with every installed build and the candidate flag sets, and count exact matches. A miss that fails
   the same way on every build is a source detail, not a compiler signal.
3. If adjacent service packs can't be told apart on probes, compile a large external, already-matching codebase with
   every build and diff the per-function bytes after linking (`tools/sp_differential.py`). Then find exact matches
   between those objects and this ROM's functions (shared SDK code) and note which builds produce them.
4. Find a codegen idiom that separates the remaining groups and scan the ROM's disassembly for it, after checking the
   scanner on code whose build is known.
5. For each flag, record an exact match that breaks when the flag changes.
6. Identify each library separately: the SDK, NitroSystem, the C runtime and middleware are usually prebuilt with
   another build and other flags (often the default inlining instead of the game's `-inline on,noauto`). Count only
   functions with game idioms (singleton guards, the game's classes) as game evidence. One library can need different
   builds for different parts. Set `PATH_COMPILERS` and `PATH_DROP_FLAGS` from the result.
7. Don't commit the probes: document the method and the evidence here.

## What separates builds and flags

- Idioms: a 64x32 multiply with a known-zero high word gives `umull` + `mla` against a zeroed register up to 2.0 sp2
  and only `umull` from sp2p2 on; whole-function size differences of a few words between service packs.
- `-O` level: `-O3,p` allocates registers differently; `-O4,s` and `-O2` keep small accessors out of line.
- `-ipa file`: statics of inline functions are shared standalone sections referenced from every module; without it
  they land in each file's `.bss`. In C under `-ipa file`, a function can load a global defined in its own file from
  two literal-pool words (one through the section, one through the symbol); with the global `extern`, or without
  `-ipa file`, it uses one. So a duplicate pool word in the original shows the global is defined in that file.
- 2.0 sp2p2 vs sp2p3 (under `-ipa file`): sp2p3 and sp2p4 give every object of 64 bytes or more defined in the
  file its own section; sp2p2 and earlier keep the file's objects in one section. Accesses at constant offsets go
  through the section, so an object under 64 bytes used by member and by address in one function takes two pool
  words on every build (which shows it is defined in that file), a bigger one takes two up to sp2p2 and one from
  sp2p3 (the same code as an `extern` object). Without `-ipa file`, C uses the symbol and C++ the section.
  sp2p3 and sp2p4 gave the same code on every comparison so far. Duplicate pool words found from relocations are
  mostly Thumb literal-pool islands: check that both words sit in one pool.
- `-str reuse` vs `noreuse`: identical literals in one file share one copy with `reuse`. One copy referenced by
  several functions shows `reuse` (literals are file-local).
- `-inline on,noauto` vs `auto`: calls within one file kept as `bl`.
- `-enum int` vs `min`: struct field offsets after enum-typed members.
- `-char signed`: `ldrsb` when walking a plain `char` table.
- `-Cpp_exceptions off`: new-expression codegen with a constructor.
- `-RTTI off`: no typeinfo slot before vtables.
- `-interworking`: leaf returns are `bx lr`, never `mov pc, lr`.
- `-proc arm946e`: DSP instructions (`smulbb`) present; `arm9tdmi` lacks them.
- Host: switch-table density exactly at the 0.4 threshold needs Windows' 53-bit x87 precision (the pinned wibo).

Probing tips: `tools/probe_compilers.py` strips `-O`, `-inline` and `-ipa` from the base flags because it varies
them, so spell them out (`--flags="-O4,p -ipa file -inline on,noauto"`); it adds `-thumb` when every probed
function is Thumb and masks relocations per byte (a Thumb `bl` pair can straddle a word). Check it first on one
known match and one deliberate compile error.

## Open

Settings not yet backed by an exact match.
