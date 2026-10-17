#!/usr/bin/env python3
"""Watch the 2.0/sp2p2 compiler's backend work on one function.

Runs the stock compiler under gdb (nothing is patched or rebuilt) and, each time it reaches one of the chosen
points, prints that function's backend instruction lists: blocks, opcodes, virtual registers (vN), physical
registers (rN), `=` marking a definition and `[copy]` a register copy. With --merges it also logs every merge made
by the register coalescer ("keep vA drop vB": vB's live range is folded into vA and the copy between them deleted),
with --color-order the order in which colouring gives each virtual register its physical one, and with --sched each
pick of the scheduler that runs before colouring (with its register-pressure score).

Points (repeatable --at, default: coalesce,color,after-color):
  cse          entry of local common-subexpression elimination (runs several times, before and after selection)
  licm         entry of local loop-invariant code motion
  coalesce     entry of each coalescing loop (also called repeatedly from register colouring, around spilling)
  color        entry of register colouring
  after-color  right after register colouring (registers are physical from here on)
  final        the final instruction list, after peephole and scheduling

Usage:
  tools/mwcc_trace.py src/Main/Some/Unit.cpp func_02012345 --merges > trace.txt
  tools/mwcc_trace.py my_variant.cpp NAME --flags-from src/Main/Some/Unit.cpp
Function names match by substring of the compiler's internal (mangled) name. Diagnostics only: the object file is
discarded. Only 2.0/sp2p2 is supported (the addresses below belong to that build and are checked before running).
"""
import argparse
import os
import re
import shlex
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from probe_compilers import INCLUDES  # noqa: E402
from project import VERSION  # noqa: E402  (tools/project.py)

ROOT = Path(__file__).resolve().parent.parent
COMPILER = ROOT / "tools/mwccarm/2.0/sp2p2/mwccarm.exe"
WIBO = ROOT / "tools/bin/wibo"

POINTS = {
    "cse": 0x505280,
    "licm": 0x507A20,
    "coalesce": 0x50AEE0,
    "color": 0x513DC0,
    "after-color": 0x471CE8,
    "final": 0x4FE7D0,
}
MERGE_SITE = 0x50B068  # inside the coalescing loop, after a merge has been accepted
# Bytes expected at each address in 2.0/sp2p2, so a different compiler build is refused instead of misread.
CHECKS = {
    0x50AEE0: "a128a86300",  # mov eax,[block list head]
    0x50B068: "0fbffd",  # movsx edi,bp
    0x471CE3: "e8d8200a00",  # call register colouring
}
OPNAME_TABLE = 0x5E7774  # opcode descriptors, 0x1c bytes each, name pointer first


def pe_reader(path: Path):
    data = path.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    nsec = struct.unpack_from("<H", data, pe + 6)[0]
    opt = struct.unpack_from("<H", data, pe + 20)[0]
    base = struct.unpack_from("<I", data, pe + 24 + 28)[0]
    secs = []
    for i in range(nsec):
        o = pe + 24 + opt + 40 * i
        vsize, va, rsize, roff = struct.unpack_from("<IIII", data, o + 8)
        secs.append((base + va, max(vsize, rsize), roff))

    def read(addr: int, n: int) -> bytes:
        for va, size, roff in secs:
            if va <= addr < va + size:
                return data[roff + addr - va : roff + addr - va + n]
        raise ValueError(hex(addr))

    return read


def pe_entry(path: Path) -> int:
    data = path.read_bytes()
    pe = struct.unpack_from("<I", data, 0x3C)[0]
    return struct.unpack_from("<I", data, pe + 24 + 28)[0] + struct.unpack_from("<I", data, pe + 24 + 16)[0]


def opcode_names(read) -> list[str]:
    names = []
    while True:
        ptr = struct.unpack("<I", read(OPNAME_TABLE + 0x1C * len(names), 4))[0]
        try:
            raw = read(ptr, 24).split(b"\0")[0].decode("ascii")
        except (ValueError, UnicodeDecodeError):
            break
        if not raw or not raw.isprintable():
            break
        names.append(raw.replace("%s", ""))
    return names


def ninja_flags(source: str, stand_in: bool = False) -> list[str]:
    """Compiler flags for `source` from build.ninja's mwcc rule (base_flags + cc_flags + include/define flags)."""
    text = (ROOT / "build.ninja").read_text()
    m = re.search(r"^build \S+: mwcc " + re.escape(source) + r"\b.*?\n((?:  \S.*\n)+)", text, re.M)
    if not m:
        sys.exit(f"{source}: no mwcc build line in build.ninja")
    var = dict(re.findall(r"^  (\w+) = (.*)$", m.group(1), re.M))
    if "sp2p2" not in var.get("cc", "") and not stand_in:
        sys.exit(f"{source} builds with {var.get('cc')}; only 2.0/sp2p2 is supported. If its probe shows the same "
                 "difference under 2.0/sp2p2, rerun with --sp2p2-stand-in to trace it with that compiler")
    flags = shlex.split(var.get("base_flags", "")) + shlex.split(var.get("cc_flags", ""))
    for inc in INCLUDES:  # the build's include path: include/ and every libs/*/include
        flags += ["-i", str(ROOT / inc)]
    return flags + ["-d", f"version_{VERSION.upper()}"]


GDB_SCRIPT = r'''
import gdb
inf = gdb.selected_inferior()
NAMES = %(names)r
FUNC = %(func)r
POINTS = %(points)r
MERGES = %(merges)r
def rd(a, n): return inf.read_memory(a, n).tobytes()
def u32(a): return int.from_bytes(rd(a, 4), 'little')
def s16(a):
    v = int.from_bytes(rd(a, 2), 'little'); return v - 0x10000 if v & 0x8000 else v
def curfunc():
    try:
        o = u32(0x63a79c)
        return rd(u32(o + 8) + 0xa, 120).split(b'\0')[0].decode('latin1') if o else ''
    except Exception:
        return ''
def opnd(a):
    kind, cls, fl, reg = rd(a, 1)[0], rd(a + 1, 1)[0], int.from_bytes(rd(a + 2, 2), 'little'), s16(a + 4)
    if kind == 0:
        nm = ('r%%d' %% reg) if reg < u32(0x63a2bc + 4 * cls) else ('v%%d' %% reg)
        return ('=' if fl & 2 else '') + ('%%d:' %% cls if cls else '') + nm
    if kind == 2:
        return '#%%d' %% int.from_bytes(rd(a + 2, 4), 'little', signed=True)
    if kind == 4:
        off = int.from_bytes(rd(a + 2, 4), 'little', signed=True)
        try:
            nm = rd(u32(u32(a + 6) + 8) + 0xa, 80).split(b'\0')[0].decode('latin1')
        except Exception:
            nm = '?%%x' %% u32(a + 6)
        return '&%%s%%s' %% (nm, '+%%d' %% off if off else '')
    if kind == 6:
        return 'L%%x' %% u32(a + 2)
    return 'k%%d(%%s)' %% (kind, rd(a + 2, 12).hex())
def dump(label):
    out = ['---- %%s: %%s' %% (curfunc(), label)]
    b = u32(0x63a828); bn = 0
    while b and bn < 2000:
        out.append('B%%d' %% bn)
        i = u32(b + 0x14); n = 0
        while i and n < 20000:
            op = int.from_bytes(rd(i + 0x28, 2), 'little'); cnt = s16(i + 0x2c)
            name = NAMES[op] if op < len(NAMES) else 'op%%d' %% op
            ops = ', '.join(opnd(i + 0x2e + 0xe * k) for k in range(max(0, min(cnt, 10))))
            out.append('   %%-10s %%s%%s' %% (name, ops, '   [copy]' if u32(i + 0xc) & 0x10 else ''))
            i = u32(i); n += 1
        b = u32(b); bn += 1
    print('\n'.join(out), flush=True)
class Point(gdb.Breakpoint):
    def __init__(self, addr, label):
        super().__init__('*0x%%x' %% addr, type=gdb.BP_BREAKPOINT, internal=True); self.label = label
    def stop(self):
        if FUNC in curfunc(): dump(self.label)
        return False
class Merge(gdb.Breakpoint):
    def stop(self):
        if FUNC in curfunc():
            f = gdb.selected_frame()
            keep = s16((int(f.read_register('rsp')) & 0xffffffff) + 8)
            print('     merge: keep v%%d  drop v%%d' %% (keep, int(f.read_register('rbp')) & 0xffff), flush=True)
        return False
for label, addr in POINTS:
    Point(addr, label)
if MERGES:
    Merge('*0x%%x' %% %(merge_site)d, type=gdb.BP_BREAKPOINT, internal=True)
def insn(i):
    op = int.from_bytes(rd(i + 0x28, 2), 'little'); cnt = s16(i + 0x2c)
    name = NAMES[op] if op < len(NAMES) else 'op%%d' %% op
    return name + ' ' + ', '.join(opnd(i + 0x2e + 0xe * k) for k in range(max(0, min(cnt, 10))))
class ColorLog(gdb.Breakpoint):
    # the colourer has just given a virtual register its physical register
    def stop(self):
        if FUNC in curfunc():
            node = int(gdb.selected_frame().read_register('rbx')) & 0xffffffff
            print('     color: v%%d -> r%%d' %% (s16(node + 0x28), s16(node + 0x2a)), flush=True)
        return False
class SchedLog(gdb.Breakpoint):
    # the pre-colouring scheduler has picked its next instruction
    def stop(self):
        if FUNC in curfunc():
            f = gdb.selected_frame()
            node = int(f.read_register('rbx')) & 0xffffffff
            ins = int(f.read_register('rbp')) & 0xffffffff
            cyc = u32(int(f.read_register('rsp')) & 0xffffffff)
            print('     sched: c%%d pressure=%%d live=%%d limit=%%d ready=%%d slack=%%d height=%%d | %%s' %% (
                cyc & 0xffff, u32(0x63a364), u32(0x616680), u32(0x61667c), s16(node + 0x14), s16(node + 0x16),
                s16(node + 0x18), insn(ins)), flush=True)
        return False
class PushLog(gdb.Breakpoint):
    # simplify has removed a node from the interference graph and pushed it; colouring pops in reverse order
    def __init__(self, addr, reg, kind):
        super().__init__('*0x%%x' %% addr, type=gdb.BP_BREAKPOINT, internal=True); self.reg = reg; self.kind = kind
    def stop(self):
        if FUNC in curfunc():
            node = int(gdb.selected_frame().read_register(self.reg)) & 0xffffffff
            idx = (node - u32(0x63ba40)) // 0x36
            print('     push: v%%d conflicts=%%d cost=%%d %%s' %% (idx, s16(node + 0x2e), u32(node + 0xc), self.kind), flush=True)
        return False
class SimplifyStart(gdb.Breakpoint):
    def stop(self):
        if FUNC in curfunc(): print('     simplify: start', flush=True)
        return False
if %(simplify)r:
    SimplifyStart('*0x5136b0', type=gdb.BP_BREAKPOINT, internal=True)
    PushLog(0x51377f, 'rcx', 'low')
    PushLog(0x5138d9, 'rdx', 'stuck: lowest cost/conflicts')
    PushLog(0x51399a, 'rax', 'low')
if %(color_order)r:
    ColorLog('*0x513b80', type=gdb.BP_BREAKPOINT, internal=True)
if %(sched)r:
    SchedLog('*0x4ff9b7', type=gdb.BP_BREAKPOINT, internal=True)
'''


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", help="file to compile (any path)")
    ap.add_argument("func", help="substring of the function's mangled name")
    ap.add_argument("--flags-from", help="repo-relative source whose build.ninja flags to use (default: source)")
    ap.add_argument("--at", action="append", choices=sorted(POINTS), help="dump points (repeatable)")
    ap.add_argument("--merges", action="store_true", help="log every coalescer merge")
    ap.add_argument("--sp2p2-stand-in", action="store_true",
                    help="trace a file that builds with another compiler using 2.0/sp2p2 (check first that the probe "
                         "shows the same difference under 2.0/sp2p2)")
    ap.add_argument("--color-order", action="store_true", help="log each register the colourer assigns, in order")
    ap.add_argument("--simplify", action="store_true",
                    help="log the colourer's simplify pushes (vreg, conflicts, spill cost); colouring pops them in "
                         "reverse, so the last pushed gets the first choice of register")
    ap.add_argument("--sched", action="store_true",
                    help="log each pick of the pre-colouring scheduler (pressure score, live count, limit)")
    args = ap.parse_args()

    read = pe_reader(COMPILER)
    for addr, want in CHECKS.items():
        if read(addr, len(want) // 2).hex() != want:
            sys.exit(f"{COMPILER} is not the expected 2.0/sp2p2 build (mismatch at {addr:#x})")
    entry = pe_entry(COMPILER)
    flags_src = args.flags_from or os.path.relpath(Path(args.source).resolve(), ROOT)
    flags = ninja_flags(flags_src, args.sp2p2_stand_in)
    points = [(p, POINTS[p]) for p in (args.at or ["coalesce", "color", "after-color"])]
    lang = "c" if args.source.endswith(".c") else "c++"

    with tempfile.TemporaryDirectory() as tmp:
        script = Path(tmp) / "trace.py"
        script.write_text(GDB_SCRIPT % dict(names=opcode_names(read), func=args.func, points=points,
                                            merges=args.merges, merge_site=MERGE_SITE,
                                            color_order=args.color_order, sched=args.sched,
                                            simplify=args.simplify))
        cmd = ["gdb", "-q", "-batch", "-nx", "-ex", "set pagination off", "-ex", "set debuginfod enabled off",
               # gdb decodes this 32-bit code as 64-bit; stepping over a breakpoint out of line would treat an
               # absolute address operand as rip-relative and fault, so step over breakpoints in place.
               "-ex", "set displaced-stepping off",
               "-ex", "starti",
               # The compiler's code is not mapped yet at the first instruction, so software breakpoints can't be
               # written. Stop once at its entry point with one hardware breakpoint, then plant the rest.
               "-ex", f"hbreak *{entry:#x}", "-ex", "continue", "-ex", "delete",
               "-ex", f"source {script}", "-ex", "continue", "--args",
               str(WIBO), str(COMPILER), *flags, "-lang=" + lang, "-c", str(Path(args.source).resolve()),
               "-o", str(Path(tmp) / "out.o")]
        proc = subprocess.run(cmd, cwd=ROOT, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    if "----" not in proc.stdout:
        sys.exit(proc.stdout[-3000:])
    for line in proc.stdout.splitlines():
        if line.startswith(("----", "B", "   ", "     merge", "     color", "     sched", "     push", "     simplify", "###", "#")):
            print(line)


if __name__ == "__main__":
    main()
