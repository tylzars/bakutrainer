#!/usr/bin/env python3
"""Explains why the rebuilt ROM doesn't match: compares each rebuilt module (main, ITCM, DTCM) in build/<ver>/build/
with the extracted one.

For each module that differs: the size difference, words that differ by a constant small amount grouped (pointers
shifted because code or data moved), and the first difference that isn't such a shift, with the symbol it falls in.
Typical causes: a function with a wrong or zero size, so dsd maps the data after it as ARM code (the linker then adds
ARM-to-Thumb veneers at the end of .text and later pointers shift by 12 bytes per veneer), a unit range that ends on a
halfword, or a unit whose compiled object is a different size.

  diff_rom.py [--max-shift 0x1000] [module ...]
"""
import argparse
import bisect
import collections
import re
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from project import module_base, module_paths, modules  # noqa: E402  (tools/project.py)


def is_pointer(v):
    """An address in ITCM, main RAM or DTCM."""
    return 0x01ff8000 <= v < 0x02000000 or 0x02000000 <= v < 0x02400000 or 0x027e0000 <= v < 0x02800000


def symbols(cfg):
    out = []
    for line in (cfg / "symbols.txt").read_text().splitlines():
        if m := re.match(r"(\S+) kind:(\S+) addr:(0x[0-9a-f]+)", line):
            out.append((int(m.group(3), 16), m.group(1), m.group(2)))
    return sorted(out)


def diff_module(module, max_shift):
    """Prints the report for one module; returns True when it matches."""
    cfg, extracted, rebuilt = module_paths(module)
    if not rebuilt.is_file():
        print(f"{module}: no rebuilt binary ({rebuilt}); run ninja")
        return False
    built, orig = rebuilt.read_bytes(), extracted.read_bytes()
    if built == orig:
        return True
    base = module_base(module)
    print(f"{module}: size built {len(built):#x}, original {len(orig):#x} ({len(built) - len(orig):+#x})")
    shifts = collections.defaultdict(list)
    first = None
    for i in range(0, min(len(built), len(orig)) - 3, 4):
        x, y = struct.unpack_from("<I", built, i)[0], struct.unpack_from("<I", orig, i)[0]
        if x == y:
            continue
        if is_pointer(y) and 0 < abs(x - y) <= max_shift:
            shifts[x - y].append((base + i, y))
        elif first is None:
            first = base + i
    if first is None and len(built) != len(orig):
        first = base + min(len(built), len(orig))
    for d, hits in sorted(shifts.items(), key=lambda kv: -len(kv[1])):
        lowest = min(t for _, t in hits)
        print(f"  pointers shifted by {d:+#x}: {len(hits)} words, lowest original target {lowest:#010x} "
              f"(something before that address grew or moved by {d:+#x})")
    if first is None:
        print("  no difference other than shifted pointers")
        return False
    syms = symbols(cfg)
    k = bisect.bisect_right([s[0] for s in syms], first) - 1
    name, kind = (syms[k][1], syms[k][2]) if k >= 0 else ("?", "?")
    print(f"  first other difference at {first:#010x}, in {name} ({kind})")
    for label, data in (("built", built), ("original", orig)):
        o = first - base
        print(f"    {label:8} {data[max(o - 8, 0):o + 16].hex(' ', 4)}")
    off = first - base
    if off + 4 <= min(len(built), len(orig)):
        ow, bw = struct.unpack_from("<I", orig, off)[0], struct.unpack_from("<I", built, off)[0]
        if ow & 1 and is_pointer(ow) and bw & 1 and bw > ow:
            print("  the original word is a Thumb function pointer and the built one points further on, at a veneer "
                  "the linker added: dsd mapped this stretch as ARM code. Look for a function before it with a wrong "
                  "or zero size (a size-0 `_unk` from dsd init) and give it its real size (docs/decompiling.md, "
                  "'Setting up a new game')")
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("modules", nargs="*", help="default: every module")
    ap.add_argument("--max-shift", type=lambda s: int(s, 0), default=0x1000)
    a = ap.parse_args()
    results = [diff_module(m, a.max_shift) for m in (a.modules or modules())]
    if all(results):
        print("every module matches")


if __name__ == "__main__":
    main()
