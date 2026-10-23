#!/usr/bin/env python3
"""Turns mwcc "multidef" definitions (binding 13) in a compiled object into mwld weak definitions (binding 14).

Definitions every user of an inline function emits (its function-local statics, `_ZZ...`/`_ZGVZ...`, and the
inline functions themselves, such as inline destructors and virtual thunks) appear in each compiled file that uses
them, and the original link kept one copy. Where that copy is still delinked, dsd writes it as weak (14) or as a
global. mwld prefers a multidef copy over a weak one, so without this a compiled copy would win and every reference
to it would move; against a global it is a duplicate-definition error. As weak definitions, the first one in link
order wins, and a weak function copy is dropped.

Usage: demote_multidef.py <object.o>...
"""
import sys
from pathlib import Path

from elftools.elf.elffile import ELFFile

STB_MW_MULTIDEF = 13
STB_MW_WEAK = 14

for path in map(Path, sys.argv[1:]):
    data = bytearray(path.read_bytes())
    with open(path, "rb") as fh:
        elf = ELFFile(fh)
        symtab = elf.get_section_by_name(".symtab")
        if symtab is None:
            continue
        base, size = symtab["sh_offset"], symtab["sh_entsize"]
        for i, sym in enumerate(symtab.iter_symbols()):
            p = base + i * size + 12  # st_info
            if data[p] >> 4 == STB_MW_MULTIDEF and sym["st_shndx"] != "SHN_UNDEF":
                data[p] = (STB_MW_WEAK << 4) | (data[p] & 0xF)
    path.write_bytes(data)
