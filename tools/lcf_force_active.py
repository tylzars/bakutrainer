#!/usr/bin/env python3
"""Prepends a FORCE_ACTIVE block to the dsd-generated LCF.

In a game with overlays it keeps each overlay's entry function (looked up by address in the overlay's symbols.txt):
main reaches every overlay through one shared load address, which dsd links against the first overlay only, so
-dead would strip the others. This game has no overlays.

It keeps the data symbols listed in <config-dir>/force_active.txt (one per line, `#` comments): objects the
original link kept although nothing references them, which -dead would otherwise strip from a unit linked from
source.

It also applies <config-dir>/lcf_edits.txt (one edit per line, `#` comments), for what dsd's per-unit ranges can't say:
  place <stub>.o <function> <unit>.o   the stub unit's line becomes OBJECT(<function>, <unit>.o): one function of a
                                       unit placed apart from the rest of it
  place <stub>.o .text <unit>.o        the stub unit's line becomes <unit>.o(.text): all the code of a unit whose
                                       data sits elsewhere in link order (its delinks entry has no .text range)
  org <unit>.o <address>               sets the location counter to <address> before the unit's .text line: zero
                                       fill the link lays down before a unit placed at a fixed address
  reserve <unit>.o <section> <symbol> <size>
                                       after the unit's <section> line, defines <symbol> at the location counter
                                       and advances it by <size>: zero space the linker script itself lays down
                                       (several reserves after one unit follow each other in file order)
  alias <symbol> <base> <offset>       defines <symbol> = <base> + <offset>, for names other units use for a point
                                       inside an object of a linked unit (a field of a singleton instance)
  inner <symbol> <size>                aliases every symbols.txt name inside <symbol>'s <size> bytes to <symbol> +
                                       offset (names of fields of a big global that other units still use)

(mwldarm's -force_active flag would be simpler but segfaults under wibo.)
"""
import argparse
import re
from pathlib import Path
from project import LCF_SYMBOLS  # noqa: E402  (tools/project.py)

parser = argparse.ArgumentParser()
parser.add_argument("lcf_in", type=Path)
parser.add_argument("lcf_out", type=Path)
parser.add_argument("--config-dir", type=Path, required=True, help="config/<ver>/arm9")
args = parser.parse_args()

TEXT_START = re.compile(r"^\s*\.text\s+start:(0x[0-9a-fA-F]+)")
SYMBOL = re.compile(r"^(\S+)\s+kind:function\S*\s+addr:(0x[0-9a-fA-F]+)")

entries: list[str] = []
ov_root = args.config_dir / "overlays"
for ov_dir in sorted(ov_root.iterdir()) if ov_root.is_dir() else ():  # a game may have no overlays
    base = None
    for line in (ov_dir / "delinks.txt").read_text().splitlines():
        if m := TEXT_START.match(line):
            base = int(m.group(1), 16)
            break
    if base is None:
        raise SystemExit(f"{ov_dir}: no .text section in delinks.txt")
    for line in (ov_dir / "symbols.txt").read_text().splitlines():
        if (m := SYMBOL.match(line)) and int(m.group(2), 16) == base:
            entries.append(m.group(1))
            break
    else:
        raise SystemExit(f"{ov_dir}: no function symbol at overlay base {base:#010x}")

extra = args.config_dir / "force_active.txt"
if extra.exists():
    for line in extra.read_text().splitlines():
        name = line.split("#", 1)[0].strip()
        if name:
            entries.append(name)

lcf = args.lcf_in.read_text()
# Linker-defined constants the linker script has to provide (values read from the original's literal pools):
# overlay ids under their SDK names, and the IRQ/system stack sizes.
sdk = [f"    SDK_OVERLAY_OV{n:03d}_ID = {n};" for n in range(lcf.count("_ID = "))]
sdk += [f"    {name} = {value};" for name, value in LCF_SYMBOLS.items()]
# Aliases of main symbols go at the end of the ARM9 block, where their bases have addresses. mwldarm files a symbol
# that the script defines outside every block under the last section (the last overlay), which tells `dsd check symbols` it
# belongs to overlay 8; inside the block it is filed under ARM9 like its base. Absolute aliases stay outside.
aliases = []  # (in ARM9 block, line)
main_syms = {}
for ln in (args.config_dir / "symbols.txt").read_text().splitlines():
    if m := re.match(r"(\S+) kind:\S+ addr:(0x[0-9a-fA-F]+)", ln):
        main_syms.setdefault(m.group(1), int(m.group(2), 16))
edits = args.config_dir / "lcf_edits.txt"
reserved = {}  # (unit, section) -> the last lines a reserve edit inserted after that unit's line
if edits.exists():
    for line in edits.read_text().splitlines():
        words = line.split("#", 1)[0].split()
        if not words:
            continue
        if words[0] == "place":
            stub, func, unit = words[1:]
            old = f"        {stub}(.text)\n"
            if old not in lcf:
                raise SystemExit(f"lcf_edits: no line for {stub} in the LCF")
            new = f"        {unit}(.text)\n" if func == ".text" else f"        OBJECT({func}, {unit})\n"
            lcf = lcf.replace(old, new, 1)
        elif words[0] == "org":
            unit, address = words[1:]
            old = f"        {unit}(.text)\n"
            if old not in lcf:
                raise SystemExit(f"lcf_edits: no line for {unit} in the LCF")
            lcf = lcf.replace(old, f"        . = {address};\n{old}", 1)
        elif words[0] == "reserve":
            unit, section, name, size = words[1:]
            anchor = reserved.get((unit, section), f"        {unit}({section})\n")
            if anchor not in lcf:
                raise SystemExit(f"lcf_edits: no {section} line for {unit} in the LCF")
            line = f"        {name} = .;\n        . = . + {size};\n"
            lcf = lcf.replace(anchor, anchor + line, 1)
            reserved[(unit, section)] = line
        elif words[0] == "alias":
            name, base, offset = words[1:]
            aliases.append((base in main_syms, f"    {name} = {base} + {offset};"))
        elif words[0] == "inner":
            base, size = words[1], int(words[2], 0)
            addrs = {}
            for ln in (args.config_dir / "symbols.txt").read_text().splitlines():
                m = re.match(r"(\S+) kind:(?:data|bss)\S* addr:(0x[0-9a-fA-F]+)", ln)
                if m:
                    addrs[m.group(1)] = int(m.group(2), 16)
            start = addrs[base]
            for name, addr in sorted(addrs.items(), key=lambda kv: kv[1]):
                if start < addr < start + size:
                    aliases.append((True, f"        {name} = {base} + {addr - start:#x};"))
        else:
            raise SystemExit(f"lcf_edits: unknown edit {words[0]!r}")
lcf = lcf.replace("SECTIONS {\n", "SECTIONS {\n" + "\n".join(sdk) + "\n", 1)
inside = [line.strip() for in_main, line in aliases if in_main]
outside = [line for in_main, line in aliases if not in_main]
if inside:
    end = lcf.index("    } > ARM9\n")
    lcf = lcf[:end] + "".join(f"        {line}\n" for line in inside) + lcf[end:]
if outside:
    end = lcf.rstrip().rfind("}")
    lcf = lcf[:end] + "\n".join(outside) + "\n" + lcf[end:]
# mwldarm rejects an empty block (a game without overlays and force_active entries has none).
force = f"FORCE_ACTIVE {{ {', '.join(entries)} }}\n\n" if entries else ""
args.lcf_out.write_text(f"{force}{lcf}")
