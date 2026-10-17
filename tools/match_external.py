#!/usr/bin/env python3
"""Finds functions from externally compiled objects that exactly match functions in
this ROM, and reports which compiler builds produced the matching bytes.

Input is the per-compiler object tree written by sp_differential.py
(<out>/<compiler>/<file>.o). Each compiled function is compared, with relocation
sites masked, against every ROM function of the same size.

  match_external.py build/spdiff/<name> [--modules main,itcm]
"""
import argparse
import collections
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from probe_compilers import ROOT, elf_functions, module_bytes  # noqa: E402
from project import VERSION  # noqa: E402  (tools/project.py)

ap = argparse.ArgumentParser()
ap.add_argument("objdir", type=Path)
ap.add_argument("--modules", default="main,itcm")
ap.add_argument("--min-size", type=lambda s: int(s, 0), default=0x20)
a = ap.parse_args()

SYM = re.compile(r"^(\S+) kind:function\((arm|thumb),size=(0x[0-9a-f]+)\) addr:(0x[0-9a-f]+)")
cfg = ROOT / f"config/{VERSION}/arm9"
rom_funcs = collections.defaultdict(list)  # size -> [(module, name, bytes)]
for mod in a.modules.split(","):
    path = cfg / "symbols.txt" if mod == "main" else cfg / mod / "symbols.txt" if mod in ("itcm", "dtcm") \
        else cfg / "overlays" / mod / "symbols.txt"
    base, blob = module_bytes(mod)
    for line in path.read_text().splitlines():
        if m := SYM.match(line):
            size, addr = int(m.group(3), 16), int(m.group(4), 16)
            rom_funcs[size].append((mod, m.group(1), blob[addr - base: addr - base + size]))

matches = collections.defaultdict(set)  # (obj file, func, rom func) -> {compilers}
compilers = sorted(d.name for d in a.objdir.iterdir() if d.is_dir())
for comp in compilers:
    for obj in (a.objdir / comp).glob("*.o"):
        for name, (code, mask) in elf_functions(obj).items():
            if len(code) < a.min_size:
                continue
            for mod, rname, rcode in rom_funcs.get(len(code), ()):
                if all(i in mask or code[i:i + 4] == rcode[i:i + 4] for i in range(0, len(code), 4)):
                    matches[(obj.name, name, f"{mod}:{rname}")].add(comp)

by_set = collections.defaultdict(list)
for key, comps in matches.items():
    by_set[tuple(sorted(comps))].append(key)
print(f"{len(matches)} exact function matches against the ROM")
for comps, keys in sorted(by_set.items(), key=lambda kv: -len(kv[1])):
    label = "ALL" if len(comps) == len(compilers) else ", ".join(c.replace("2.0_", "") for c in comps)
    print(f"\n[{label}]  {len(keys)} functions")
    for obj, name, rom in sorted(keys)[:15]:
        print(f"   {name:36} {rom:24} ({obj.replace('__', '/')[:-2]})")
