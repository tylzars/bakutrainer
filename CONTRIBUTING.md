# Contributing

## Layout
| Path | Contents |
| --- | --- |
| `config/usa/` | `dsd` configuration: symbols, relocations and translation-unit boundaries (`delinks.txt`) |
| `src/Main/` | Game code, grouped by subsystem |
| `include/` | Game headers, in the same subsystem folders under `include/Main/` |
| `libs/<name>/` | Library code the game links, each with `src/` and `include/`: NitroSDK in `libs/nds/`, the C runtime in `libs/msl/`, the Mobiclip video middleware in `libs/mobiclip/`; `libs/c/` holds the standard C headers. Libraries can need their own compiler (`PATH_COMPILERS` in `tools/project.py`) |
| `tools/` | Build scripts and analysis tools; `tools/project.py` holds the game's settings |
| `docs/` | Notes on the compiler, on matching and on the memory layout |
| `extract/`, `build/` | Your ROM, its extraction and the build output (never committed) |

## Naming
- Source files are named in `snake_case` after what they hold. A file's header has the same name under `include/`,
  in the same subsystem folder.
- Structs, enum types and functions use `PascalCase`; struct members use an `m` prefix (`mState`).
- File-scope statics use an `s` prefix (`sMapFileIds`), globals shared between units a `g` prefix (`gVideoState`).
- Locals and parameters use `camelCase`; `i`/`j`/`k` are fine for loop counters.
- Library functions keep their library's public API names (NitroSDK `OS_*`, `MI_*`, ...). Those names come from the
  SDK interface, not from this ROM, so a function only gets one when its identity is certain.
- Constants are `#define`s in `UPPER_SNAKE`. A `#define` holds a plain number and keeps the literal's value and type
  exactly (suffix included, `0x8000u`). It never stands for an object, a member, a register access or an expression.
- Naming never changes a variable's or member's type to an enum type: the compiler treats enum-typed values
  differently and the code would change.
- A constant names one value of a set: the values that one member, parameter or return value takes. The set's names
  share a prefix (`SCENE_STATE_`) and go in the header that declares the API carrying the value, or at the top of the
  one `.c` file that uses them. Every use of the set uses the names. A value the code gives no meaning (a sentinel, a
  count, a value doing another job) stays a literal with a comment. Coordinates, sizes and one-off waits stay
  literals.
- A recurring word keeps one meaning everywhere: `m...Timer` counts down, `m...Frames` counts up, `m...FrameCount`
  is a fixed length, `m...Handle` holds index + 1 (0 = none), `m...Visible` / `m...Enabled` are 1 when drawn /
  active, `Update<X>` runs every frame, `Load<X>` reads a file, `Init<X>` runs once, `Setup<X>` can run again.
- Every name must be backed by what this repository shows: what the code does, the game's own strings and file
  paths, the SDK headers, and names already given. Names carry no address (`symbols.txt` keeps it).
- No function-like or expression macros (`MIN`, `CLAMP`, `ASSERT`, register-cast wrappers) and no new helpers: each
  function is written out plainly. Hardware registers use the SDK headers' names (`reg_G3X_*`), or the address cast
  in place where there is none.
- Anything not yet understood keeps an address-based name: functions `func_<addr>`, data `data_<addr>`, members
  `mUnk_<offset>`.
- Struct members carry their offset in a comment: `/* 2c */ u8 mState;`.
- Headers live in `include/` and `libs/<name>/include/`, never next to a source file, and are included by their path
  there (`#include <Main/System/bg_image.h>`). There is one copy of every header.

## Changing a translation unit
Every unit is linked from source. The full steps and tools are in [docs/decompiling.md](docs/decompiling.md#workflow).
In short:
1. A unit's entry in `config/usa/arm9/delinks.txt` gives its source path and its section ranges (`dsd` splits the
   original at those boundaries). A boundary that is a guess has a `// provisional:` comment above the entry saying
   what is unsure.
2. Functions stay in address order, and each must keep matching.
3. After a change the ROM must still match (`ninja`).

## Commits
- One area per commit (a unit, a library family, a tool). Messages are short (about 50 characters) and say what the
  change does.
- Commit what the project is made of: source, headers, config, docs, the build and its tools. Scratch files, search
  output and one-off scripts stay out of the repository.

## Before committing
- `ninja` ends with `bakutrainer_usa.nds: OK` and `ninja check` passes.
- `tools/check_calls.py`, `tools/check_callees.py` and `tools/check_strings.py` end with a count of 0.
- After `ninja report`: `tools/check_signatures.py` lists no function and `tools/check_units.py` prints OK for every
  unit (each exits non-zero otherwise).
- No function's score in `build/usa/report.json` got worse; run `tools/progress.py update` before pushing.
- Docs say only what has been checked; anything unchecked says so.

## Rules
- Never commit anything taken from the game: ROMs, assets, extracted files or disassembly. Text the game code itself
  uses (string literals, file paths) belongs in source like any other constant.
- Write code and comments yourself; don't copy them from other projects. Library headers are written from the
  public API and what the code does.
- Data the code uses is written as typed source (structs and arrays with their values), not as raw byte dumps.
  Bytes with no meaning as data are read from your ROM at build time (`.incbin`, expanded by
  `tools/expand_incbin.py`), and ranges no unit covers link from your ROM.
- Comments say what the code or the compiler does, not where code came from. A routine written as assembly has a
  comment above it saying why the compiler can't produce it, and its instructions are commented.
- Third-party code the game links keeps its own copyright and permission notice.
- Format C and C++ with `clang-format` (the repository's `.clang-format`). It joins an `asm` function body into one
  line, so fence each `asm` function with `// clang-format off` and `// clang-format on`.
