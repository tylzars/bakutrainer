# Bakugan: Battle Brawlers - Battle Trainer

A matching decompilation of *Bakugan: Battle Brawlers - Battle Trainer* (Nintendo DS, USA, `BBKE`): C written by
hand so that, built with the original compiler (Metrowerks CodeWarrior for DS), it reproduces the game's code byte
for byte. Nothing from the game is stored here: building needs your own dump of the cartridge.

- [INSTALL.md](INSTALL.md): requirements and building.
- [CONTRIBUTING.md](CONTRIBUTING.md): layout, naming, rules, what to check before committing.
- [docs/compiler.md](docs/compiler.md): which compilers and flags the game and its libraries were built with, and the
  evidence.
- [docs/decompiling.md](docs/decompiling.md): workflow and the source patterns that matter.

The rebuilt ROM matches the original. Code is split from the original binary until it is decompiled; the
progress below counts what is decompiled and what links from source.

## Progress
<!-- progress:start -->
![Decompilation progress over time](progress/progress.svg)

| Measure | Progress |
|---|---|
| Matched code | 86.79% (270,330 / 311,460 bytes) |
| Linked code (complete units) | 66.56% (207,298 bytes) |
| Overall accuracy (fuzzy) | 97.10% |
| Matched functions | 1,606 / 1,795 |

The code size went from 389,724 to 311,460 bytes in two steps: the `.text`/`.rodata` boundary moved to where the original's code ends (0x0204df14; the game's constant tables and the Mobiclip decoder after them are in the original's `.rodata`), and the zero fill the linker lays down before 0x02001000 and 0x02003000 is no longer counted as code. The steps in the graph are these changes, not new matching.

_Last updated 2026-10-19. Full history: [`progress/history.csv`](progress/history.csv)._
<!-- progress:end -->
