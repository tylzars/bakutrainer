# Building

## Requirements
- Linux (the build runs the Windows compiler through wibo)
- Python 3.10+ with `pyelftools`
- Ninja, Git, and a Rust toolchain (`cargo`) for building `dsd`
- CMake, clang, lld and libclang (LLVM 17+) for building `wibo`
- ARM binutils (`arm-none-eabi-objdump`, `arm-none-eabi-readelf`; Debian/Ubuntu `binutils-arm-none-eabi`) for
  `probe_compilers.py --show`, `check_units.py`, `reorder_functions.py`, `fix_layout.py` and reading disassembly
- `gdb` for `tools/mwcc_trace.py`

## Steps
1. Put your ROM at the path `extract/README.md` gives and check its SHA-1.
2. Build the patched `dsd` (stock releases size version-3 banners incorrectly) and the pinned `wibo` (it needs
   Windows' 53-bit x87 precision, which no release sets yet; see the script):
   ```shell
   tools/build_dsd.sh
   tools/build_wibo.sh
   ```
3. Generate the build script and build:
   ```shell
   python3 tools/configure.py
   ninja
   ```
   The first run downloads the CodeWarrior compilers into `tools/`, extracts the ROM, splits it into objects,
   links it and checks the result against `<game>_<ver>.sha1`. objdiff is downloaded the first time a target needs
   it (`ninja report`, `ninja objdiff`). For a game with several versions,
   pass the version (`python3 tools/configure.py eur`) and set `DECOMP_VERSION=eur` for the other tools.
4. Enable the repository hooks (the pre-push hook keeps the progress graph current):
   ```shell
   git config core.hooksPath .githooks
   ```

## Matching the header
Optional: put your own ARM7 BIOS dump at `arm7_bios.bin` in the repository root and `dsd` writes the header
checksum at 0x6C itself. Without it, `tools/fix_header.py` writes your base ROM's value (or `secure_area_crc`) there and
recomputes the header checksum. Either way the ROM matches.

## Useful targets
| Command | Result |
| --- | --- |
| `ninja` | Build the ROM and verify its SHA-1 |
| `ninja objdiff` | Write `objdiff.json` for the objdiff GUI |
| `ninja report` | Write `build/<ver>/report.json` (matching progress) |
| `ninja check` | SHA-1 plus `dsd check symbols` |
| `ninja dis` | Disassembly for reading, in `build/<ver>/asm/` (never committed) |
