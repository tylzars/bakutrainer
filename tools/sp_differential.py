#!/usr/bin/env python3
"""Differential test of mwccarm builds on an external, already-matching codebase.

Compiles every source file with each compiler build under the given flags, hashes
each function's bytes (relocated words masked), and reports functions whose output
differs between builds. Used to establish whether the CodeWarrior 2.0 service packs
differ on real-world code, and which constructs distinguish them.

  sp_differential.py --root <repo> --glob 'src/*.c' --flags '...' [--compilers 2.0/]
"""
import argparse
import collections
import json
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from probe_compilers import MWCC_ROOT, WIBO, elf_functions  # noqa: E402

ap = argparse.ArgumentParser()
ap.add_argument("--root", type=Path, required=True)
ap.add_argument("--glob", action="append", required=True)
ap.add_argument("--flags", required=True)
ap.add_argument("--compilers", default="2.0/")
ap.add_argument("--out", type=Path, required=True, help="work dir for objects + results.json")
ap.add_argument("-j", type=int, default=8)
a = ap.parse_args()

compilers = sorted(str(p.parent.relative_to(MWCC_ROOT)) for p in MWCC_ROOT.glob("*/*/mwccarm.exe")
                   if a.compilers in str(p))
sources = sorted({p for g in a.glob for p in a.root.glob(g)})
a.out.mkdir(parents=True, exist_ok=True)


def job(args):
    comp, src = args
    obj = a.out / comp.replace("/", "_") / (str(src.relative_to(a.root)).replace("/", "__") + ".o")
    obj.parent.mkdir(parents=True, exist_ok=True)
    if not obj.exists():
        r = subprocess.run(f"{WIBO} {MWCC_ROOT / comp / 'mwccarm.exe'} {a.flags} -c {src.relative_to(a.root)} -o {obj.resolve()}",
                           shell=True, cwd=a.root, capture_output=True, text=True)
        if r.returncode or not obj.exists():
            return comp, src, None
    return comp, src, elf_functions(obj)


def same(x, y) -> bool:
    """Equal after linking: same size and same bytes outside either build's relocation sites."""
    (cx, mx), (cy, my) = x, y
    if len(cx) != len(cy):
        return False
    m = mx | my
    return all(i in m or cx[i:i + 4] == cy[i:i + 4] for i in range(0, len(cx), 4))


hashes = collections.defaultdict(dict)  # (src, func) -> {comp: hash}
failed = collections.Counter()
with ThreadPoolExecutor(a.j) as ex:
    for comp, src, funcs in ex.map(job, [(c, s) for s in sources for c in compilers]):
        if funcs is None:
            failed[str(src)] += 1
            continue
        for f, h in funcs.items():
            hashes[(str(src.relative_to(a.root)), f)][comp] = h

complete = {k: v for k, v in hashes.items() if len(v) == len(compilers)}
diffs = []
for (src, f), per in sorted(complete.items()):
    groups: list[list[str]] = []
    for c in compilers:
        for g in groups:
            if same(per[g[0]], per[c]):
                g.append(c)
                break
        else:
            groups.append([c])
    if len(groups) > 1:
        diffs.append({"src": src, "func": f, "groups": groups})

ok_files = len({s for s, _ in complete})
print(f"compilers: {', '.join(compilers)}")
print(f"sources: {len(sources)}, compiled by all: {ok_files}, failed in some/all: {len(failed)}")
print(f"functions compared: {len(complete)}, differing: {len(diffs)}")
partition = collections.Counter(json.dumps(d["groups"]) for d in diffs)
for p, n in partition.most_common():
    print(f"  {n:4} functions split as {p}")
(a.out / "results.json").write_text(json.dumps({"compilers": compilers, "compared": len(complete), "diffs": diffs}, indent=1))
