#!/usr/bin/env python3
"""Counts the pre-sp2p2 "long" 64-bit multiply idiom in disassembly.

CodeWarrior for DS 2.0 up to sp2 lowers a 64x32 multiply whose high operand word is
known to be zero as umull + mla against a register holding 0; sp2p2 and later drop
that mla. This scanner simulates registers forward from each umull/smull (tracking
`mov rX, #0` and register copies) and reports how many multiplies accumulate a
known-zero product into the high word.

  longmul_scan.py <asm files...>
"""
import collections
import re
import sys

REG = r"(r\d+|ip|lr|fp|sl|sb|sp)"
DEST = re.compile(rf"^[a-z]+\S*\s+{REG}\b")


def scan(path):
    func, ins = None, []
    for raw in open(path):
        line = raw.split(";")[0].split("@")[0].strip()
        if m := re.match(r"(?:arm|thumb)_func_start (\S+)", line):
            if func:
                yield func, ins
            func, ins = m.group(1), []
        elif line and not line.startswith((".", "arm_func_end", "thumb_func_end")) and not line.endswith(":"):
            ins.append(line)
    if func:
        yield func, ins


def classify(ins, i):
    m = re.match(rf"[us]mull {REG}, {REG}, {REG}, {REG}$", ins[i])
    hi = m.group(2)
    zero = set()
    for p in ins[max(0, i - 6):i]:  # zeros set just before the multiply
        if mz := re.match(rf"mov {REG}, #0x0$", p):
            zero.add(mz.group(1))
        elif md := DEST.match(p):
            zero.discard(md.group(1))
    zero -= {m.group(1), hi}
    for p in ins[i + 1:i + 8]:
        if ma := re.match(rf"mla {REG}, {REG}, {REG}, {REG}$", p):
            if ma.group(1) == hi and (ma.group(2) in zero or ma.group(3) in zero):
                return "long"
            zero.discard(ma.group(1))
            continue
        if mz := re.match(rf"mov {REG}, #0x0$", p):
            zero.add(mz.group(1))
        elif mc := re.match(rf"mov {REG}, {REG}$", p):
            (zero.add if mc.group(2) in zero else zero.discard)(mc.group(1))
        elif md := DEST.match(p):
            zero.discard(md.group(1))
    return "other"


counts = collections.Counter()
examples = collections.defaultdict(list)
for path in sys.argv[1:]:
    for func, ins in scan(path):
        for i, line in enumerate(ins):
            if re.match(r"[us]mull ", line):
                k = classify(ins, i)
                counts[(path, k)] += 1
                if k == "long":
                    examples[path].append(func)
for path in sys.argv[1:]:
    print(f"{path}: long={counts[(path, 'long')]} other={counts[(path, 'other')]} {sorted(set(examples[path]))[:4]}")
