# Building

## Requirements
- Linux (the build runs the Windows compiler through wibo)
- Python 3.10+ with `pyelftools`
- Ninja, Git, and a Rust toolchain (`cargo`) to build `dsd`
- CMake, clang, lld and libclang (LLVM 17+) to build `wibo`
- ARM binutils (`arm-none-eabi-objdump`, `arm-none-eabi-readelf`; Debian/Ubuntu `binutils-arm-none-eabi`) for
  `probe_compilers.py --show`, `check_units.py`, `reorder_functions.py`, `fix_layout.py` and reading disassembly
- `gdb` for `tools/mwcc_trace.py`

## Steps
1. Put your ROM where [extract/README.md](extract/README.md) says and check its SHA-1.
2. Build the patched `dsd` (stock releases get the size of version-3 banners wrong) and the pinned `wibo` (the
   compiler needs Windows' 53-bit x87 precision, which no wibo release sets yet; see the script):
   ```shell
   tools/build_dsd.sh
   tools/build_wibo.sh
   ```
3. Generate the build script and build:
   ```shell
   python3 tools/configure.py
   ninja
   ```
   The first run downloads the CodeWarrior compilers into `tools/`, extracts the ROM, splits it into objects, links
   it and checks the result against `bakutrainer_usa.sha1`. objdiff is downloaded the first time a target needs it
   (`ninja report`, `ninja objdiff`).
4. Turn on the repository hooks (the pre-push hook keeps the progress graph current):
   ```shell
   git config core.hooksPath .githooks
   ```

## Matching the header
Optional: put your own ARM7 BIOS dump at `arm7_bios.bin` in the repository root, and `dsd` writes the header field
at 0x6C itself. Without it, `tools/fix_header.py` writes your base ROM's value there (or `secure_area_crc` from
`tools/project.py`) and recomputes the header checksum. The ROM matches either way.

## Useful targets
| Command | Result |
| --- | --- |
| `ninja` | Build the ROM and check its SHA-1 |
| `ninja objdiff` | Write `objdiff.json` for the objdiff GUI |
| `ninja report` | Write `build/usa/report.json` (matching progress) |
| `ninja check` | SHA-1 plus `dsd check symbols` |
| `ninja dis` | Disassembly for reading, in `build/usa/asm/` (never committed) |
