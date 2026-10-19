"""Project settings shared by configure.py and the tools: edit this file when starting a new game.

Everything game-specific lives here: the game's short name, its versions (ROM file, secure-area CRC), the compiler
for game code and for each library folder, and per-path flag differences (-thumb included). The tools read the
memory layout (module base addresses) from the dsd extract, so nothing here needs addresses.

CC_FLAGS is the compiler command line for game code; tools/configure.py and tools/probe_compilers.py both use it.

The version the tools work on is VERSION: the first entry of VERSIONS, or $DECOMP_VERSION when set.
"""
import os
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Short name used for file names: extract/baserom_<GAME>_<ver>.nds, build/<ver>/<GAME>_<ver>.nds, <GAME>_<ver>.sha1.
GAME = "bakutrainer"

VERSIONS = {
    "usa": {
        "baserom": f"extract/baserom_{GAME}_usa.nds",
        # Header field at 0x6C (secure-area CRC), written by tools/fix_header.py when no ARM7 BIOS is supplied
        # (`--arm7-bios`). None takes it from the base ROM.
        "secure_area_crc": None,
    },
}

VERSION = os.environ.get("DECOMP_VERSION") or next(iter(VERSIONS))

# Extra `dsd init` options, used once when configure.py generates config/<ver>/. If init stops with "Local function
# call ... leads to no function" (e.g. a one-instruction `bx lr` function its analysis doesn't find), the hidden stock
# option "--allow-unknown-function-calls" gives such call targets a function symbol; fix their sizes afterwards.
# This game needs it: the start-up code calls a lone `bx lr` that the analysis doesn't find.
DSD_INIT_FLAGS = ["--allow-unknown-function-calls"]

# ---- compilers ---------------------------------------------------------------------------------------------------
# Pin each code family by evidence (tools/probe_compilers.py) and record the evidence in docs/compiler.md.
# Game code and each library are often built with different CodeWarrior versions.
MWCC_VERSION = "2.0/sp2p3"          # game code: 2.0 sp2p2 or later shown; sp2p3 rests on one assumption (docs/compiler.md)
# decomp.me's compiler id for each installed build (from decomp.me's compiler list), for objdiff's scratch button.
DECOMP_ME_IDS = {
    "1.2/b56": "mwcc_20_56", "1.2/base": "mwcc_20_72", "1.2/sp2": "mwcc_20_79", "1.2/sp2p3": "mwcc_20_82",
    "1.2/sp3": "mwcc_20_84", "1.2/sp4": "mwcc_20_87",
    "2.0/base": "mwcc_30_114", "2.0/sp1": "mwcc_30_123", "2.0/sp1p2": "mwcc_30_126", "2.0/sp1p5": "mwcc_30_131",
    "2.0/sp1p6": "mwcc_30_133", "2.0/sp1p7": "mwcc_30_134", "2.0/sp2": "mwcc_30_136", "2.0/sp2p2": "mwcc_30_137",
    "2.0/sp2p3": "mwcc_30_138", "2.0/sp2p4": "mwcc_30_139",
}
DECOMP_ME_COMPILER = DECOMP_ME_IDS.get(MWCC_VERSION)  # None: no decomp.me id (DSi builds); objdiff gets no scratches
# Sources under these paths (folders or single files; the longest matching prefix wins) compile with another
# installed version (folders under tools/mwccarm/).
PATH_COMPILERS = {
    "libs/nds/": "2.0/sp1p5",  # NitroSDK: sp1 ... sp2p2 give the same bytes (docs/compiler.md)
    "libs/msl/": "2.0/sp1p5",  # C runtime, built with the SDK
}
# Flags from CC_FLAGS that sources under these paths must not get.
PATH_DROP_FLAGS = {
    "libs/nds/": ["-inline on,noauto"],  # the libraries' default inlining (not separated from on,noauto)
    "libs/msl/": ["-inline on,noauto", "-Cpp_exceptions off"],
}
# Flags added for sources under these paths. A game with Thumb code: "src/": ["-thumb"], with an empty list for an
# ARM-only folder or file. An ARM function inside a Thumb file can be written between `#pragma thumb off` and
# `#pragma thumb on` instead (same code as compiling it in an ARM file).
# C++ dialect (-lang) for .cpp sources under these paths; others use c++.
PATH_CPP_DIALECT = {
    # verified: operator delete builds as the original (no exception-specification frame, exception entry 0x15/0x100000,
    # symbol _ZdlPv) only as Embedded C++; plain C++ requires throw() on it, which adds a frame
    "libs/msl/": "ec++",
}
PATH_ADD_FLAGS = {
    "src/": ["-thumb"],  # most game code is Thumb; ARM game files or folders get an empty list
    # verified: the exception index holds exactly the non-leaf C runtime functions, word for word
    "libs/msl/": ["-Cpp_exceptions on"],
}


def path_setting(table: dict, path, default=None):
    """The value for the longest key of table that path starts with, or default."""
    keys = [k for k in table if str(path).startswith(k)]
    return table[max(keys, key=len)] if keys else default


# Compiler flags for game code, each checked against this game's code as noted ("verified" = an exact match that
# breaks when the flag changes); the evidence is in docs/compiler.md. Library folders drop flags through
# PATH_DROP_FLAGS.
CC_FLAGS = [
    "-O4,p",              # verified: -O4,s and -O2 miss; -O3,p not separated
    "-ipa file",          # verified: duplicate pool words for in-file globals
    "-inline on,noauto",  # verified against auto (inlines same-file calls); not separated from the default
    "-enum int",          # not separated
    "-char signed",       # verified for the C runtime and the Mobiclip glue code; not separated in game code
    "-str reuse",         # verified: one copy of a literal serves three functions
    "-proc arm946e",      # verified: arm9tdmi misses
    "-gccext,on",
    "-fp soft",
    "-Cpp_exceptions off",
    "-RTTI off",          # vtables without a typeinfo pointer
    "-interworking",      # verified: leaf returns are bx lr
    "-w off",
    "-sym on",
    "-gccinc",            # treat #include "" and <> alike
    "-nolink",
    "-msgstyle gcc",
]


def source_flags(src) -> tuple[str | None, list[str]]:
    """(compiler version or None for MWCC_VERSION, compiler flags) for one source file, language flag included."""
    drop = path_setting(PATH_DROP_FLAGS, src, [])
    flags = [f for f in CC_FLAGS if f not in drop] + path_setting(PATH_ADD_FLAGS, src, [])
    return path_setting(PATH_COMPILERS, src), flags + [lang_flag(src)]


def lang_flag(src) -> str:
    """The -lang flag for one source file: C, or the path's C++ dialect for a .cpp file."""
    return "-lang=" + (path_setting(PATH_CPP_DIALECT, src, "c++") if str(src).endswith(".cpp") else "c")


LD_FLAGS = [
    "-proc arm946e",
    "-dead",
    "-nostdlib",
    "-interworking",
    "-map closure,unused",
    "-msgstyle gcc",
    "-m Entry",
]
# The assembler, for regions written as .s files (2.0 sp2p2 ships none; sp2p3 is the next build that does).
MWASM_VERSION = "2.0/sp2p3"

# ---- paths -------------------------------------------------------------------------------------------------------
CONFIG = ROOT / "config" / VERSION / "arm9"
BUILD = ROOT / "build" / VERSION
EXTRACT = ROOT / "extract" / VERSION


def _yaml_base(path):
    m = re.search(r"^base_address:\s*(\d+)", path.read_text(), re.M) if path.is_file() else None
    return int(m.group(1)) if m else None


def _base_addresses():
    """{module: base address} for 'main', 'itcm', 'dtcm' and 'ovNNN', read from the dsd extract."""
    bases = {}
    for module, name in (("main", "arm9"), ("itcm", "itcm"), ("dtcm", "dtcm")):
        if (base := _yaml_base(EXTRACT / "arm9" / f"{name}.yaml")) is not None:
            bases[module] = base
    ovl = EXTRACT / "arm9_overlays" / "overlays.yaml"
    if ovl.is_file():
        # One "- id: N" entry per overlay; the entries also carry file_id, so anchor on the list marker.
        for entry in re.split(r"^\s*- ", ovl.read_text(), flags=re.M)[1:]:
            oid = re.search(r"^id:\s*(\d+)", entry, re.M)
            base = re.search(r"^\s*base_address:\s*(\d+)", entry, re.M)
            if oid and base:
                bases[f"ov{int(oid.group(1)):03d}"] = int(base.group(1))
    return bases


BASES = _base_addresses()
# Hardware sizes of the tightly coupled memories, for address-to-module lookups.
TCM_SIZES = {"itcm": 0x8000, "dtcm": 0x4000}


def module_base(module: str) -> int:
    """Base address of 'main', 'itcm', 'dtcm' or an overlay ('ov001'); needs the ROM extracted (configure.py)."""
    if module not in BASES:
        raise SystemExit(f"no base address for {module}: extract the ROM first (python3 tools/configure.py)")
    return BASES[module]


def module_paths(module: str) -> tuple[Path, Path, Path]:
    """(config dir, extracted binary, rebuilt binary) of 'main', 'itcm', 'dtcm' or an overlay."""
    if module == "main":
        return CONFIG, EXTRACT / "arm9/arm9.bin", BUILD / "build/arm9.bin"
    if module in ("itcm", "dtcm"):
        return CONFIG / module, EXTRACT / f"arm9/{module}.bin", BUILD / f"build/{module}.bin"
    return CONFIG / "overlays" / module, EXTRACT / f"arm9_overlays/{module}.bin", BUILD / f"build/arm9_{module}.bin"


def modules() -> list[str]:
    """Every module of the extracted ROM: 'main' first, then the TCMs and overlays."""
    return ["main"] + sorted(m for m in BASES if m != "main")


def main_end() -> int:
    """First address past the main module: where the overlays start, or the end of its .bss in a game without any."""
    ovs = [v for k, v in BASES.items() if k.startswith("ov")]
    if ovs:
        return min(ovs)
    arm9 = EXTRACT / "arm9" / "arm9.yaml"
    m = re.search(r"^bss_end:\s*(\d+)", arm9.read_text(), re.M) if arm9.is_file() else None
    if not m:
        raise SystemExit("the ROM isn't extracted (python3 tools/configure.py): main_end() has no answer")
    return int(m.group(1))


def tcm_module(addr: int):
    """'itcm' or 'dtcm' when addr lies in one of them, else None."""
    for module, size in TCM_SIZES.items():
        if module in BASES and BASES[module] <= addr < BASES[module] + size:
            return module
    return None


# ---- linker script -----------------------------------------------------------------------------------------------
# Constants the linker script must define for the game's code (tools/lcf_force_active.py adds them). Read their
# values from the original's literal pools; the overlay ids (SDK_OVERLAY_OVnnn_ID) are added automatically.
LCF_SYMBOLS = {
    "SDK_IRQ_STACKSIZE": "0x800",  # pool words in func_0203d520, OS_InitThread and OS_GetInitArenaHi
    "SDK_SYS_STACKSIZE": "0",  # pool words in OS_InitThread and OS_GetInitArenaHi
}
