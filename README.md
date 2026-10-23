# Bakugan: Battle Brawlers - Battle Trainer (DS) decompilation

A matching decompilation of *Bakugan: Battle Brawlers - Battle Trainer* for the Nintendo DS. The C in this repository
is written by hand so that, built with the original compiler, it reproduces the game's code byte for byte.

**No game content is included.** The repository holds no ROM, no assets and no disassembly. To build, you need your
own dump of the game; see [INSTALL.md](INSTALL.md).

| Version | Game code | SHA-1 |
| --- | --- | --- |
| USA | `BBKE` | `1a228628834d7925186c3f42aa6a018922d11295` |

## Progress
<!-- progress:start -->
![Decompilation progress over time](progress/progress.svg)

| Measure | Progress |
|---|---|
| Matched code | 96.50% (300,570 / 311,460 bytes) |
| Linked code (complete units) | 100.00% (311,460 bytes) |
| Overall accuracy (fuzzy) | 97.29% |
| Matched functions | 1,620 / 1,795 |

All code is linked from this repository's source and the ROM matches byte for byte. See [docs/decompiling.md](docs/decompiling.md) for functions objdiff scores below 100% with identical code.

_Last updated 2026-10-22. Full history: [`progress/history.csv`](progress/history.csv)._
<!-- progress:end -->

## Documentation
- [INSTALL.md](INSTALL.md): setting up and building
- [CONTRIBUTING.md](CONTRIBUTING.md): repository layout, naming and workflow
- [docs/compiler.md](docs/compiler.md): how the compiler and flags were identified
- [docs/decompiling.md](docs/decompiling.md): source patterns that matter for matching

Built with [ds-decomp](https://github.com/AetiasHax/ds-decomp), Metrowerks CodeWarrior for DS run through
[wibo](https://github.com/decompals/wibo), and [objdiff](https://github.com/encounter/objdiff).
