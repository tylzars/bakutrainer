# DS decompilation template

A starting point for a matching decompilation of a Nintendo DS game built with Metrowerks CodeWarrior: C and C++
written by hand so that, built with the original compiler, it reproduces the game's code byte for byte.

It holds the build (ninja, [ds-decomp](https://github.com/AetiasHax/ds-decomp), CodeWarrior through
[wibo](https://github.com/decompals/wibo), [objdiff](https://github.com/encounter/objdiff)) and the matching tools,
with nothing from any game. Everything game-specific is in `tools/project.py`.

## Starting a new game
1. Make the new repository from this one: `git clone <this repo> <game>` keeps the template's history as the
   first commits (then point `origin` at the game's remote); copying the files instead needs `git init`. Set the
   commit identity you publish under (`git config user.name`, `git config user.email`) and enable the hooks before
   the first push: `git config core.hooksPath .githooks` (the pre-push hook keeps the progress graph current).
2. Edit `tools/project.py`: `GAME` (a short name for file names) and the version table. Put your ROM at
   `extract/baserom_<GAME>_<ver>.nds` and fill in the version's row in `extract/README.md`.
3. Get the tools (see [INSTALL.md](INSTALL.md): build `dsd` and `wibo`, or copy `tools/bin/` and `tools/mwccarm/` from
   another checkout with the same pins), then:
   ```shell
   python3 tools/configure.py
   ninja
   ```
   The first run extracts the ROM, runs `dsd init` to generate `config/<ver>/arm9/` (symbols, relocations, one
   delinks entry per module), writes `<GAME>_<ver>.sha1` from your ROM, downloads the compilers and objdiff,
   rebuilds the ROM from the delinked objects and checks its SHA-1. Without an ARM7 BIOS dump, the header field at
   0x6C is taken from your ROM (`secure_area_crc` overrides it). If `dsd init` fails or the ROM doesn't match, see
   "Setting up a new game" in [docs/decompiling.md](docs/decompiling.md) (`tools/diff_rom.py` shows where it differs).
4. Commit `config/` and the `.sha1` file once the ROM matches: from here on the config is source.
5. Survey the ROM's shape: overlays (header words 0x50/0x54 are zero when there are none), ARM vs Thumb (count the
   relocation kinds in `relocs.txt`: `thumb_call` vs `arm_call`), libraries and middleware (ID strings such as
   `[SDK+...]` in the build info) and hand-written blocks with inline tables.
6. Identify the compilers before decompiling much, and make the result solid first: see
   [docs/compiler.md](docs/compiler.md). Game code and each library are often built with different CodeWarrior
   versions and flags; set `MWCC_VERSION`, `CC_FLAGS`, `PATH_COMPILERS`, `PATH_DROP_FLAGS` and `PATH_ADD_FLAGS`
   (e.g. `-thumb`) from the evidence, and record which settings are verified.
7. Classify the regions (code vs data, library vs game, hand-written code, file boundaries) and split the modules
   into translation units in `delinks.txt`.
8. Decompile and link: [docs/decompiling.md](docs/decompiling.md) (workflow and the source patterns that matter) and
   [CONTRIBUTING.md](CONTRIBUTING.md) (layout, naming, rules, what to check before committing).

Once the game has a name, replace this README with the game's own (keep the progress markers below:
`tools/progress.py update` fills them in).

## Progress
<!-- progress:start -->
<!-- progress:end -->
