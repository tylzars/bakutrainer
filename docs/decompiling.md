# Decompiling notes

How to get functions to match exactly. The compiler and its flags are in [compiler.md](compiler.md). Paths use the
version `usa`.

## Workflow

1. `ninja dis` writes the original's disassembly into `build/usa/asm/`.
2. Write the function in a scratch file with a `// PROBE: <symbol> <module> <address>` line and compare it:
   `python3 tools/probe_compilers.py <file> --unit <the source path it will live at> --show`. This compiles it with
   the compiler and flags the build uses for that path and prints a side-by-side diff (`*` marks a difference).
   Without `--unit`, give `--only` and `--flags` yourself, and spell out `-O`, `-inline` and `-ipa` in `--flags`.
   Relocated bytes are masked, so check call targets and literal addresses against the original yourself.
3. When only registers or stack slots differ, mark the declarations that may move with a trailing `//PERM` and run
   `python3 tools/permute.py <file> <function>`. Declaration order sets the locals' numbering, and so the order they
   get registers in ([Register allocation](#register-allocation)); the tool tries the orders. `tools/auto_permute.py`,
   `tools/mutate.py` and `tools/permuter.py` search wider, and `tools/mwcc_trace.py` shows the backend's decisions
   ([Watching the compiler's backend](#watching-the-compilers-backend)). Run long searches in the background.
4. Put the code in its unit under `src/` (or `libs/<name>/src/`), functions in address order, with an entry in
   `config/usa/arm9/delinks.txt`: the source path, one line per section it owns, and `complete` once it links from
   source:

       src/Main/Scene/stage_progress.c:
           complete
           .text       start:0x020145bc end:0x02015340
           .rodata     start:0x020500d4 end:0x020500d8

   A `.text` range starts at the unit's first function and ends on a 4-byte boundary after the padding
   (`tools/check_units.py` checks both). Shared types go in `include/`, library declarations in
   `libs/<name>/include/`. Each function has one declaration, with the definition's types
   (`tools/check_signatures.py`), and every call has a declaration in scope: the build's `-w off` hides an undeclared
   call, and `tools/check_protos.py` lists them.
5. `ninja report`, then give the original's symbols the names your code uses:
   `python3 tools/sync_symbols.py build/usa/<path>.o main --write`. It finds each matching function by its bytes and
   reads every referenced address from the original code (ARM and Thumb calls); `--assume NAME=ADDR` settles
   identical candidates. `python3 tools/resolve_undefined.py <unit> --write` places symbols your code references at a
   point inside a bigger original object.
6. Inline functions and their statics are defined by every file that uses them (in this repository: the inline
   destructors of the Mobiclip glue code). Mark them `weak` in `symbols.txt`. The build demotes the compiled copies to
   weak (`tools/demote_multidef.py`), so the original's copy wins once a file is linked.
7. Run `python3 tools/check_calls.py`, `tools/check_callees.py` and `tools/check_strings.py`: matching masks
   relocated words, so a function can match while calling the wrong function, loading the wrong string or pointing at
   the wrong field (`&banks[9]` vs `&banks[6]`). Probes mask the same way; linking catches the rest.
8. A function that doesn't match yet stays in its file with a `// non-matching:` comment saying exactly what differs.
   `tools/unmatched.py` lists every function below 100% in the report, and `tools/clean_notes.py` removes the notes
   once the functions match. A file can be committed like that, unlinked.
9. To link a unit whose functions all match, run `python3 tools/try_link.py <unit>` (or `--libs`). It runs the steps
   below and restores the config if anything fails.
   - `tools/link_check.py <unit>` checks the function order (mwcc emits functions in source order;
     `tools/reorder_functions.py <unit> --write` fixes it) and that every compiled data section has a range in the
     entry.
   - `tools/data_ranges.py <unit> --write` finds those ranges from the relocations of matching functions. It refuses
     when sections disagree, come in a different order, hold different bytes, or land in the wrong kind of section
     (`.data` where the original has `.rodata` means the source has the const-ness wrong).
   - `tools/mark_complete.py <unit>`, then `ninja`: the ROM must still match.
   - `tools/sync_linked_symbols.py --write`, then `ninja` and `ninja check`: inside complete units, `symbols.txt`
     takes the linked binary's names and bindings. Never run it after a failed build: it would rewrite the config
     from a stale link.

When linking a unit breaks the ROM, `python3 tools/diff_rom.py` shows where the rebuilt module first differs and which
pointers moved. Read the differing pool words: a wrong table or field hides behind a masked relocation.

## Setting up a new game

Problems that come up when `tools/configure.py` first generates the `dsd` configuration, and their fixes.

- `dsd init` stops with "Local function call from X to Y leads to no function" when its analysis misses a target (a
  one-instruction `bx lr` function, code reached only through a table). Set
  `DSD_INIT_FLAGS = ["--allow-unknown-function-calls"]` in `tools/project.py` (this game needs it). The
  `func_<addr>_unk` symbols this creates have size 0, so dsd maps everything up to the next known function as ARM
  code, and the linker adds a 12-byte veneer (`ldr ip, [pc]; bx ip; .word`) for each pointer to a Thumb function in
  that stretch: the ROM doesn't match. Give each `_unk` its real size, literal pool included, and rename it
  `func_<addr>`. To read it: `arm-none-eabi-objdump -D -b binary -marm --adjust-vma=0x02000000
  --start-address=<addr> extract/usa/arm9/arm9.bin`.
- On a SHA-1 mismatch, `python3 tools/diff_rom.py` shows the size change, the shifted pointers and the first other
  difference. Thumb function pointers that now point at veneers mean a stretch is mapped as code when it isn't: look
  for a function before it with a wrong or zero size.
- dsd emits no `$d` mapping symbol for data symbols inside `.rodata`, so after a hand-written routine there (a codec's
  code mixed with its tables) the linker reads everything up to the next mapping symbol as ARM code. Thumb code in the
  same delinked object that takes the address of data in that stretch gets a 12-byte veneer
  (`bx pc; nop; ldr pc, [pc, #-4]`). Until units split the gap, keep such a routine at the end of `.text`.
- "Call goes to middle of function" warnings from `dsd init` usually mean merged functions. Some are real second
  entries (fast paths of the division routines, long-branch `bl` inside large Thumb functions) and stay labels.
- `ninja dis` stops with "Inline tables must have a known size" when a data symbol of unknown size sits inside a
  function (dsd folds a literal pool placed before a function into it): give it a size, e.g. `kind:data(word[4])`.
- With no overlays, the tools use the end of the main module's `.bss` where the overlays would start.
- Thumb game code gets `-thumb` through `PATH_ADD_FLAGS` ([Thumb code](#thumb-code)).

## Watching the compiler's backend

When the source shape looks right but registers or instruction order still differ, `tools/mwcc_trace.py` shows what
the 2.0/sp2p2 backend does with one function. It runs the stock compiler under gdb (nothing is patched) and prints the
function's instruction lists at chosen points: after selection, at each coalescing loop, before and after register
colouring, and the final list. `=` marks a definition, `vN` a virtual register, `rN` a physical one, `[copy]` a
register copy. `--merges` logs each coalescer merge (`keep vA drop vB`), `--simplify` the colourer's pushes,
`--color-order` each register it hands out, and `--sched` the picks of the first scheduling pass.

    tools/mwcc_trace.py my_variant.c SetupBattleScreen --flags-from src/Main/Scene/battle_screen.c \
        --sp2p2-stand-in --at coalesce --at color --at after-color --merges > trace.txt

Diff the traces of two variants to find the first stage where they part. Only 2.0/sp2p2 is supported. The game code
builds with 2.0/sp2p3, so trace it with `--sp2p2-stand-in`, after checking that its probe shows the same difference
under sp2p2. Each trace is its own gdb process, so several can run at once.

## Codegen rules

Each rule reads "the ROM shows X: write Y (because Z)". Tags say where a rule was seen: [sp2p3] on 2.0 sp2p3 (the
game code's build), [sp2p2] on 2.0 sp2p2 (the build `tools/mwcc_trace.py` traces), [2.0-all] also on the sp1p5
library code, [SP-diff] differs between 2.0 service packs, [host] depends on the computer running the compiler, and
[mwld], [dsd], [objdiff], [mwasm] are that tool's behaviour. "traced" means read from the backend with the tracer
(sp2p3 code through `--sp2p2-stand-in`), "tested" checked with small test files. Untagged rules were seen on 2.0
builds and probably hold on all of them.

### Rules of thumb

- `memset` and `memcpy` come from `<Main/clib.h>` in game code, never `<string.h>`. The compiler treats `memset` and
  `memcpy` declared in a header named `string.h` as built-ins and expands small calls inline (a 0x44-byte struct
  clear becomes nine stores, a 10-byte copy becomes loads and stores). The same declarations in a header of any other
  name give a real call, which is what the game has. The C runtime's own files may include `<string.h>`.
- Which variables exist, where they are defined and what type they have move registers and scheduling far more often
  than how an expression is spelled. Macro or inline, ternary or if/else, and index spellings usually compile the
  same. Change the variables first.
- A one-word literal-pool difference is often a wrong constant (a wrong divisor: `/ 24`, 182 vs 240), not a layout
  problem. A wrong constant can also look like a scheduling difference when the original builds a value from a
  neighbouring constant (`mov r7,#34; sub r7,r7,#47` gives -13 where the source said 3).
- Before fighting a caller's registers, check the callee's real signature: parameter widths, return type (void or a
  value), ignored parameters. A wrong callee prototype often looks like an unfixable caller.
- Source that is dead only by the program's logic still reaches the backend and can move the scheduler or the
  colourer, though it leaves no instructions: a store every reachable case overwrites, an unread zero initialiser of a
  variable that is read later, a product whose result is overwritten, an extra live value.
- What the front end removes outright never matters: locals that are never read, discarded expressions, empty
  inlines, self-assignments, `if (0)`, `do {} while (0)`, empty `asm` blocks [sp2p2, traced]; identity arithmetic
  (`x << 0`, `x | 0`, `x * 1`, `x & -1`, ...) and a repeated test with an empty body (`if (a || a) {}`, unlike the
  form with a body in [Copies and constants](#copies-and-constants)) [sp2p3, tested].
- To see whether a register difference comes from scheduling or from allocation, compile with `-proc generic` instead
  of `-proc arm946e` (other flags unchanged). It skips the ARM9 scheduler that runs before register allocation, and
  also shows the order of stack-parameter copies at entry.
- Don't change a shared helper (bit-test or flag setters) to fix one caller: other matches depend on it. Change the
  caller instead.
- `#pragma scheduling` does not exist in this compiler; setting it changes nothing. [sp2p2]
- A function that really uses an object declared `__attribute__((aligned(N)))` skips the whole IR optimiser: no loop
  rotation, no strength reduction, globals reloaded from the pool (a dead reference doesn't count). A hint when the
  original looks unoptimised.

### Register allocation

Numbering (traced with gdb on the stock compiler):
- Parameters come first, then IR objects, then expression temporaries in creation order, so named locals are numbered
  below every expression temporary. IR objects are numbered downward from the top of their range, each group in
  creation order. The groups from the top: named function-scope locals (reverse declaration order, so the first
  declared is highest) with the IR's hoisted loop invariants just below them; temporaries of the IR's
  common-subexpression pass; temporaries of its later lifetime-splitting pass; later webs of reused variables; call
  results kept in an object; division results; nested-scope locals lowest. [sp2p3, traced on DrawModelFileEx]
- A local whose uses all lie in its defining extended block loses its copy
  ([Copies and constants](#copies-and-constants)), so the colourer sees the expression temporary, numbered in IR
  statement order, and the local's declaration position changes nothing (ComputePoseMatrices' sin/cos locals). A
  value used in a join block keeps its variable's number: the declared one for the variable's first assignment, the
  web group (below the IR temporaries) for a later assignment that starts a new web. So a value held in a dead or
  reused earlier local is numbered low (DrawModelFileEx, func_0201e56c). [sp2p3, traced]

Colouring (traced with `--simplify`):
- Scans visit the virtual registers in increasing number and push each node with fewer conflicts than there are
  registers, until the graph is empty. Colours are handed out popping the stack: the last one pushed picks first and
  takes the lowest free register, so of two nodes pushed in one scan, the lower-numbered is coloured later. Values
  that rotate through callee-saved registers left the graph in different scans: count their conflicts at the scan
  where each is visited.
- A popped node takes the lowest free of r0-r3; else a callee-saved register already handed out; else r12, then r14;
  else a new callee-saved register.
- A stuck graph spills the value with the lowest cost/conflicts. Cost = 2 x (weighted use groups) + (weighted
  definitions), where a use starts a new group only when another value dies between it and the value's previous
  definition or use. So of two similar values, the one with no death among its uses goes to the stack
  (DrawModelFileEx's rotation half-sizes). [sp2p3, traced]

For int-sized values that live across calls:
- Named locals loaded from memory (or assigned later) get registers in declaration order, the first declared the
  lowest free one. Parameters go the other way round, after all locals.
- Call results (`int a = g();`) and loop invariants hoisted by the backend are coloured as temporaries, before named
  locals, in reverse creation order.
- Use counts make no difference; the shape of the live ranges (where the last use is) does.
- Under register pressure the value that interferes with more others gets the lower register whatever the
  declaration order, which is why permuting declarations often does nothing. Initialisation order can then swap a
  pair where declaration order can't.
- Declaration order sets registers, assignment order sets load order: declare `a, b, c, d` and assign `b, a, d, c` if
  the ROM shows that.
- Types and order interact, so search them together (`u32 id = (u8) x` matched only with one declaration order).
  `permute.py` supports `//PERM T=u8|u32|int`.
- `s16` locals get registers differently from `s32` ones in otherwise identical code (one cause: the `s16` table
  loads in [IR optimiser](#ir-optimiser)). Declaration order does not affect `double` locals.

A short-lived value in a long-lived value's callee-saved register:
- The ROM reuses the register of a dead long-lived value for a short-lived one: write the long-lived value as an
  expression temporary whose arithmetic folds into its later use (`Entry *entries = o->mData->mEntries;`, with the
  `+12` folding into the use), not `Data *d = o->mData; ... d->mEntries`. It must be coloured first, so it needs a
  higher number than a named local gets. Making the short-lived value a named local doesn't work: copy propagation
  removes a local in a straight-line block.
- Extra function-scope locals declared ahead of a value that lives across calls can swap two registers (r4/r5
  against a pointer parameter); the fix is the form above, not declaration order.

Register order by variable shape:
- A variable assigned in both arms of an if/else is coloured late (after a pointer parameter), a short-lived value
  per arm early. The same if/else inside an inline function that returns the value gets the lowest register
  (`u32 v = Entry(i);` with the helper keeping `u32 v; if (...) {...} else {...} return v;`; an early return changes
  it).
- A value returned from an inline helper, or a block-local copied into the variable, gets registers differently from
  a direct assignment. A variable left unset on some path is live from entry and gets low registers.
- Locals used in two blocks are coloured as named locals: `x, y, step` declared once at function scope and assigned
  in both blocks matched, where per-block locals were coloured as call-result temporaries. A block-local pointer gets
  a different register from a function-scope one.
- A local reused for an earlier, separate value: its later live range is coloured after the loop counter. Later live
  ranges of reused function-scope locals go in the order the variables first appear in the function, not
  declaration order (a dead `j = 0;` near the top, or moving `g = 0;` up, reorders them); separate variables still go
  by declaration order, ahead of them.
- A local reassigned to an unrelated value that lands in another register was two variables in the source.
- A global read repeated inside a loop is hoisted and coloured after named locals and loop temporaries (higher
  registers); a named local copy is coloured early.
- A field read through an inline accessor is a temporary, coloured before named locals: `Entries(o)` vs
  `o->mEntries` moved a pointer from r5 to r6, and `Item(o, n)` differs from `o->mItems[n]` (coloured like a named
  local).
- A load folded into an address is a temporary: `e = &o->mItems[o->mIndex]` colours the index before `o`; as its own
  statement (an enum-typed copy) it is coloured after `o`.
- `*q++ = x` moves `q` to the end of the order: write the increment as its own statement on the pointer it moves
  (`*base = *r; base++; r++;`). But `a[n++] = 0;` (n+1 computed before the store) needed the folded form, and
  `x[n] = v; n++;` schedules differently from `x[n++] = v;`: try both.
- A value that is also returned is forced into r0: `return g = &x;` gives a different order from a plain store.
- `T *p = tbl; ... e[id]; e += id;` (pointer created before the index load) gets registers differently from
  `e = &tbl[id]`.
- Reading components into named locals in the original's order (`z = p->z; y = p->y; x = p->x;`) fixes the load order
  before a call.

Operand and pointer shapes:
- `p[c]` (indexing in place) loads the other operand first; `p += c; *p` doesn't.
- `p += idx * 2; y = *(s16 *)(p + 2);` (advance, then read past) gives a different register pair from a second
  `p + idx*2 + 2` expression.
- `d->x = FX_Mul(s->x, len); d->x = FX_Mul(d->x, k);` (store, then scale in place) differs from one expression through
  a temporary.
- Index in place (`owner->mSlots[i].x`) rather than through a `slot` local; index a table with the global field in
  place (`&tbl[g.a]`, seen in C runtime code) rather than through a named copy of the index.
- `T *p = (T *) &g;` (initialised with a cast) addresses `g` through its own symbol and keeps the base (or a shared
  base like `o + 0x6000`) in a callee-saved register across calls. `T *p = &g;`, `g.f` and a cast written at each use
  all address an in-file `g` from its section base and reload it after each call. [sp2p2/sp2p3, ARM and Thumb,
  tested] A struct-typed table indexed directly is a temporary where a named local is not.

Coalescing [sp2p2, traced]:
- Greedy, blocks and instructions in order: each copy whose two sides don't interfere is folded into the
  lower-numbered register and deleted ("keep vA drop vB").
- A conversion temporary (a narrow value passed to a wider parameter, also an inline function's) can coalesce with
  the local, which then takes the temporary's register whatever its declaration order. Fix the callee's parameter
  type so the local is a named local again. A `u8` passed to an inline function with a 64-bit temporary merges into
  it, so its declaration position doesn't matter.

### Copies and constants

- The backend's first local CSE forwards a local's copy into every use in the same extended basic block (its block
  and the following single-predecessor blocks), and DCE drops the copy, so the local has no register of its own. It
  survives only when a use lies in a block with two predecessors, a call in between kills the value
  (memory-dependent values only), or the copy's source is a physical register (parameter copies). Several definitions
  alone don't keep it. [sp2p2, traced]
- So a call result kept in a local is replaced by the call's result temporary (numbered after all locals, coloured on
  its own) unless one use sits in a block with two predecessors. A repeated identical test `a || a` creates such a
  block for free; after colouring, head/tail merging and the conditional optimisation drop the repeated compare (a
  different-looking second test, `n >= 1` after `n > 0`, keeps its compare). Mark such a test as a placeholder.
- The IR's expression propagation substitutes a single-definition local into its uses: always when its right side is
  a plain operand or a direct object read, otherwise only when it is used in one statement. It looks at one block
  only, so a value used in another block keeps its statement (the backend may still forward it). [sp2p2, traced]
  Substituted values are hoisted out of loops where invariant; passed straight as a call argument they are
  recomputed. A local assigned twice (`s32 left = c; left += b;`) is not substituted and is computed where written
  (e.g. in an inner loop's preheader).
- A value whose only consumer is one later call is substituted into that call; a compound assignment or a later read
  keeps it in place. Reusing one variable for two arguments pins the first one's evaluation point.
- `T *p = o->a; p += o->b;` pins the computation where written. A pointer built by `p = base; p += idx;` is not
  substituted, so identical `base[idx]` accesses elsewhere share only `idx*size` and add the base again at each use
  (`ldrh r0,[r6,r7]`); `&e[idx]` or `e + idx` shares the whole address.
- `T *e = list->mItems[p];` makes `p*4` a common subexpression with a later `p*4 + 0x28`, hoisted above an `if`: index
  the container inside each arm to stop it.
- The IR's common-subexpression pass makes a temporary at the second occurrence of an equal subtree (repeated loads,
  `*p`, `a * 2`), including the index conversion of an `int` used as an array index: an `int` set by a statement with
  a call (`k = 31 - f(x)`) and used as `t[k]` in two expressions gets such a copy, numbered in that pass's group;
  `k = 31 - m` got none. Plain reads (`x + k`, `f(k)`, `n <= k`) are not index conversions, and a larger shared
  subtree hides the index inside it. An index declared `long` (or `unsigned long`) needs no conversion, so the
  variable itself is used, which moves its register or spill slot (DrawModelFileEx's texture key index).
  [sp2p3, traced]
- A repeated field read is shared only when no real call sits between the uses; a named local copy instead moves
  register pairs. Reads of a const table are merged even across calls, so reading it at each use can give the
  original order where a named local takes a register too early.
- Where the original reloads a table element or global chain in a loop, re-read it
  (`g.list->mItems[k]->a + g.list->mItems[k]->b` forces fresh loads); a named pointer lets the compiler reuse
  earlier ones.
- Where a shift sits matters: `fx32 d = f(...) << 12;` then `FX_Mul(d, k)` matched; shifting inside the `FX_Mul`
  argument did not.
- Write differences out at each use (`x0 - x1`) instead of naming them: `dx`/`dy` locals didn't match.
- Reading a member directly (`o->mData->x`) rather than through `T *data = o->mData;` makes the pointer a temporary;
  it is still loaded once when no store or call intervenes.
- A pointer copied to a local before a store fixes the pool-load order; fields loaded into named locals before the
  arithmetic fix the field-load order (then try both operand orders).
- A store followed at once by a reload of the same member through the same pointer is forwarded
  (`o->mData = data; data = o->mData;` emits no load). `o->mUnk_40 = data; entries = o->mUnk_40->mEntries;`
  schedules differently from `data->mEntries`.
- A leftover `mov rX, rX` means a store followed by a read-back of the same field (`g.mode = m; ...; if (g.mode<=1)`).
- `if (field)` right after `field = var;` compiles to `tst #255`: store first and test the field (`f = g; if (f)`)
  rather than `if (g & 0xff)`.
- `u8 now = ++field;` gave `add; and; strb` with no reload where `field++; now = field;` reloaded; in a small test
  both compiled the same, so try both.
- ARM store-to-load forwarding (the backend CSE just before scheduling): a just-stored byte read back becomes
  `and rD, rS, #255`, a word a plain copy; halfwords are not forwarded. A later peephole turns that `and #255` (or a
  shift pair that re-extends a value) into a copy only when it knows the source already has that width: a value from
  `ldrb/ldrh/ldrsb/ldrsh`, `mov #0..255`, `and #0xff` or `and` with a mask below 0x80, `and/orr` of known values, or
  a copy of one. `(x & 0xff0000) >> 16` is not known, so reading such a stored byte back keeps the `and`.
  [sp2p3, traced]
- A constant stored to a member and read back stays a run-time computation (`o->mUnk_38 = 0x20;
  o->mUnk_38 = (0x6c - o->mUnk_38)/2 + 0x1e;`); from a local it folds. Likewise `n = 8; f(n*12);` through memory keeps
  `mov #8; mov #12; smulbb`.

Constants in registers:
- Within an extended basic block the backend reuses a register that already holds a needed constant (`mov rd, rs`,
  or `add rd, rs, #(K-C)` from a nearby constant), but only after the constant's definition and only into
  single-predecessor blocks (a join, even an if-converted one, clears it; a call doesn't clear callee-saved
  registers). So `i = 0;` after an `if (n > lim) n = lim;` clamp gives `mov r5, r6`; before it, `mov r5,#0`.
- A variable updated by compound assignment keeps its constant start value in a register (`int r = 0; r += 0xe0;`
  gives `mov r2,#0; add r3,r2,#0xe0`); other uses still fold.
- Enum-typed locals are not constant-propagated: where the original computes with a known constant held in a
  register (`mov r2,#0; mov r1,#52; smulbb r4,r2,r1` for `array[0]`), use an enum-typed local. An enum-typed zero
  stays in its register through a loop of byte stores (`enum {ZERO} z = ZERO; p[k] = (unsigned char) z;`, the cast
  avoiding sign extension); plain int zeros fold back.
- Zero-initialised locals share one `mov rN,#0`, put in whichever is assigned first (later ones get `mov rM, rN`):
  choose with declare-then-assign. `s32 a = 0, b = 0, c = 0;` as one declaration shares one register where separate
  declarations (or `= a`) don't. Two `u8` zeros sharing a register can steer the scheduler; a `u32` counter with a
  `(u8)` wrap stops the sharing.
- A dead store can matter: `n = 30;` before a switch whose reachable cases all overwrite it (the compiler can't know
  which are reachable) keeps 30 in n's register for later calls with 30.
- Mixing signed and unsigned stops constants combining: `t + 0x24 + 0x28c0u` keeps two adds.
- Compound assignment keeps constants out of folding: `angle += 90;` and `fx *= 2.0f;` differ from
  `angle = angle + 90` (the variable also stays the first operand of the float multiply).

### Scheduling

- Two list schedulers run, one before colouring and one after. The first ready instruction in list (source) order
  wins unless a later one is strictly better by, in turn: latest start time arrived; more successors made ready;
  height; an instruction-kind rank. Ties go to source order, so moving a statement within its block (or adding a call
  before it) can flip them. "Made ready" counts the successors whose only unscheduled predecessor it is, so a
  successor still waiting on a second predecessor counts 0 (DrawModelFileEx's `16 << sh0`). [sp2p3, traced]
- Under register pressure the scheduler before colouring picks by a pressure score (start 16; each last use subtracts
  1 + max(1, live - limit); each new value adds max(1, live - limit)), keeping source order on ties. [sp2p2, traced]
  The mode is decided again at every pick (live count >= the limit of 14), and the lowest score wins. The live count
  holds every value that is live out or still has unscheduled uses; a definition adds one only when its value is
  still needed and is not also read by the same instruction. So one more live value can switch every pick it is live
  across to pressure mode, and a statement's list position decides its pressure ties. [sp2p3, traced]
- One more value live across a loop changes the loop body's schedule even if it leaves no code
  (`const u8 *mode = &g;` reloaded from the pool).
- Memory-dependence edges come from alias classes set by what defines the base register. An integer-constant base
  (a hardware register, `*(vu16*)0x04000300`) gets the worst-case class and is ordered after every earlier global
  store, whatever the volatility or casts; a base from a symbol (`lda &sym`) gets that symbol's class and can pass
  stores to other objects. So when the original reads a hardware register ahead of an earlier store, declare the
  register as an extern object and pin its address in the linker script (an `alias` line in `lcf_edits.txt`).
- Out of bounds (2.0, read from the compiler): a direct access to a global gets the worst-case alias set when
  offset + size runs past the declared type's size (incomplete arrays `T x[]` excepted), and a worst-case store is
  ordered against every later load and store. Within bounds, all members of one object alias each other and
  different objects never do. So when a store seems to pin a later load, check the declared size: an object declared
  too small pins its stores past the end. A Thumb access re-anchored to obj+A (A = offset & ~imm range) is checked as
  A + offset + 1 > declared size, so in a big object every access far enough in is already worst-case. Of the
  out-of-bounds spellings only `obj.arr[k]` / `&obj.arr[k]` with k past the end folds into the pool word and anchors
  like a member; casts compute the address at run time.
- A store through any pointer makes the compiler re-read a global declared with a complete type (`extern T g;`,
  `extern T g[1];`), but not one declared `extern T g[];` (kept in a register or hoisted). Give globals their real
  size (pad a struct to the size its block has in the ROM). Loads through a pointer local die at any global store;
  loads written as `g[i].f` don't.
- Loads from `const` data may move above stores (and stack-argument stores); non-const loads may not. Declare
  read-only externs const (pointer tables `const char *const`). [2.0-all] A function-local static table schedules
  like const data (no store can reach it) while staying in `.data` (file-scope const would move it to `.rodata`); its
  symbol is `name$N`.
- `abs` (and other functions) declared in a header named `stdlib.h` are treated as pure: calls with the same argument
  are merged, even across stores. Declare it in a project header when the original calls it repeatedly.
- A store just before a return is scheduled after the epilogue's `add sp, #N` only when it targets a function-local
  static whose address is never taken: the pop's memory alias is every non-const global the function touches,
  file-scope statics included, but not a local static. A store through a register the pop restores gets the same
  order another way. [2.0 sp2p3 Thumb, tested] In ARM code an `add sp` was seen scheduled before global stores at a
  return.
- Volatile: a field or parameter reloaded where the compiler would reuse or hoist it is volatile in the original
  (tick counter, file flags). A store followed by a reload of the same field is not a sign on its own: in small tests
  with nothing between store and load (2.0 sp1p5, sp2p2, sp2p3), Thumb code reloaded every byte and halfword and
  reused words; ARM code reused bytes (`strb` then `and #0xff`) and words and reloaded a halfword parameter. So in ARM
  code, look for
  what sits between (a call, an aliasing store) before calling a field volatile. A volatile field also pins a load
  where it is written.
- Arguments with side effects (a volatile read, `x++`) are evaluated before the stack-argument stores and the other
  arguments. Side-effect-free arguments are folded into the call and loaded after the stores, because the scheduler
  never lifts a load through the object pointer above a `[sp]` store. An enum-typed copy plus a named reference can
  give the same effect.
- A `u16`/`s16` local or stack parameter is spilled to a full 32-bit slot. Its spill stores and reloads use the
  worst-case alias set, so they are ordered against each other and the clean-up after allocation won't reuse a
  register value across them, so parameter and local widths move both scheduling and copies. A `u8` parameter's
  store to its home carries a broad alias set that holds a `u16` parameter's entry copy but not the int-sized ones, so
  only with the `u16` declaration is the store ordered after that copy (DrawTextEx's `angle`). [sp2p3, traced]
- Narrowing changes scheduling, not just instructions: `-(u8)(m/4)` and `-((u32)m >> 2)` emit the same operations,
  but the `u8` form schedules `rsb` before the next pool load.
- Equivalent arithmetic inside a long sum (`y%8 << 3` vs `y%8 * 8`, `(y/64)*8*512` vs `(y/64)*4096`) schedules
  differently; term order doesn't matter (the sum is re-sorted).
- Repeated calls with constant arguments schedule in a cycle (20 identical calls repeat every 7; the period is
  10/8/7/6/6 for 1-5 stack arguments). Code before the run shifts the phase: a global store, or a call that returns a
  value (a void call shifts it less). A BOOL-returning callee declared void caused several such differences.
- When a spilled value's register still holds it at its next use in the same block, the reload becomes a `mov` (or is
  dropped). A use directly after the definition reuses the defining register (`mov r5,#0; str r5,[sp,#n]`); with an
  instruction in between, it reloads and the clean-up turns store-then-load into a copy
  (`mov r0,#0; str r0,[sp]; mov r5,r0`). How the variable is built (`s16 x = base; x += off;` vs `x = off + base;`)
  and statement order move this.
- A zero shared by two spilled stores keeps its own register (`mov r0,#0; str; str; mov r5,r0`); one spilled zero
  before a loop lets the counter take it over (`mov r5,#0; str r5,[sp]`).
- A spilled `s32` whose register still holds the value at a predicated early return has its reload dropped; as `s16`
  (still stored as a word) it keeps `ldrcs r0,[sp]`.
- A spilled `u8` local holding a call result is reloaded before each compare of a switch chain; an int isn't. A
  spilled `u16` local is reloaded before each store of a shared zero; `s32`/`u32`/`int` share one register for all
  the stores.
- `Set(o->mUnk = value)` (an assignment as the argument) schedules the store differently from two statements. Compute
  all sums before storing (`u16 y = o->mRotY + r[1]; ...; o->mRotX = x;`); `o->mRotY += r[1]` interleaves loads and
  stores.
- Create the first operand early and add the second late (`u16 rx = p->mRotX; ...; rx += r[0];`) to get registers in
  reverse creation order.
- `x*3 + y` vs `x + x + x + y`: the original may have the spelled-out sum (`(s64) x + x + x + y` for 32-bit).
- `a*b + c*d` evaluates the right-hand product first; reverse the terms to swap them. [2.0-all]
- Chained assignments `a = b = c = k` store right to left and change which register holds the constant; in a
  constructor, `a = b = 0;` gives the reload-and-truncate sequences (`ldrb` / `and #0xff`).
- `s += *w++ * p[x];` keeps each address add next to its load; `w[2*k]` with `w += 16` after it groups the adds.
- An identical first instruction in both arms of an if/else (same registers) is hoisted above the branch after
  register allocation. That is not source-level CSE, so source forms can't stop it; the original avoids it only with
  different registers per arm. Register copies and `rsb` are never hoisted. [sp2p3, tested]
- `mov rX, rY` just before a load through rX, with rY live, can be a second identical load turned into a copy by the
  clean-up after allocation; reading through a `const T *` (an inline helper with a const parameter) keeps that second
  load apart.

### IR optimiser

- The IR hoists loop invariants into the loop's preheader. The backend has its own invariant pass that puts hoisted
  values in a block of their own just before the loop (after the preceding block's code). A value assigned inside a
  loop is loop-variant to the IR, so `d2 = delta; shift = d2*d2/8;` inside the loop leaves it to the backend's pass;
  a form before the loop is hoisted by the IR and scheduled differently.
- Loop-invariant expressions are hoisted out of an outer loop only when bound to a local; write them inline in the
  inner loop's condition to keep them in the loop.
- Pointer local placement: `T *xb;` declared with the other locals and assigned just before a loop puts the address
  computation before the loop guard; `((T*)&x)[k]` at each use puts it after; initialising at the declaration changes
  the registers of the whole function. A named `char *q = (char*)&x` gives indexed `q[i]` loads, while the cast
  expression form is strength-reduced to a running pointer. [2.0-all]
- Through a pointer local, a flag array's address is hoisted (pooled, coloured) before a table read that is only in
  the loop condition; used directly, it is hoisted after.
- `(u8 *) g[0].member` keeps aliasing with `g` (and the direct hoist order) where an extern alias for the same bytes
  doesn't.
- A table offset as its own variable before the bounds check (`u32 pos = id * 0x20; if (pos >= size) ...`) gives the
  separate shift the original has. [2.0-all]
- Indexing through a pointer-to-array type (`int (*t)[3][32]`) recomputes `t + k*4` each iteration, where a running
  pointer is strength-reduced.
- `tbl[k*3 + byte]` is reassociated into a hoisted `tbl + byte`; `const u8 *p = tbl; p += k*3; p[byte]` stops it.
- A dead product still shapes the code: scaling all three components before overwriting one matters.
- `x % 300` and `x - x/300*300` compile differently (different `smull` use).
- A declared 2-D member builds `j + row`; a cast to 2-D (`((u8(*)[2]) flat)[i][j]`) builds `row + j`. Use a union
  view with a real 2-D member.
- `u16 *` indexing keeps an index update in place where byte-pointer arithmetic lets it sink past calls; a local
  reassigned in place (`cx += width;`) keeps its register.
- Loads from a const table into `s16` locals used more than once are moved by the IR: each sinks to just before its
  first use (for `a * b`, the right operand's load first), past stores and other statements, unless a real
  reassignment of its index variable (one that starts a new web) lies in between; then it stays where written and
  gets an extra copy at its first use. Single-use locals are substituted into their use; arithmetic statements used
  more than once, and statements reading address-taken locals or through a pointer, stay where written. With
  `int`/`s32` locals nothing moves and no copy is made (IR order = source order), and the multiply is still `smulbb`
  (the backend knows the `ldrsh` width). ComputePoseMatrices needs `s32` sin/cos locals, written per axis.
  [sp2p3, traced]
- Basic blocks are cut by size: when the IR is turned into instructions, a statement starts a new block once the
  current one holds 101 or more instructions (copies, per-use address loads and stores count; comma-joined statements
  are cut separately). A statement pushed into the next block loses its scheduling height there, so a few extra copies
  early in a long straight-line block move later code across the cut (ComputePoseMatrices). [sp2p3, traced]

### Loops

- No test (`cmp; bls`) before the body: write `do { } while (k < count);`, not `for`.
- An entry test inside the body (the back edge jumps to a `count != 0` guard): a do-while with the guard as an `if`
  inside.
- `if (n) { i = 0; do ... while (++i < n); }` has one guard; a `for` keeps a second `cmp/bls`.
- One function-scope loop counter (`u8 i`, maybe `j`) reused by every loop is the usual original form, even where a
  loop counts down. Per-loop counters get registers differently and no declaration order fixes it. A shared counter
  is coloured above a hoisted address, per-loop counters below. When two counters step together, the order of
  `j++; i++;` matters.
- `u32 k = 0; ... k = (u8)(k + 1);` fixes counter/coordinate swaps where `u8 k` fails in every order, also for a
  counter passed to an inline with a 64-bit temporary.
- An `unsigned long` loop counter differs from `unsigned int` to the compiler, but only where it creates a mixed-type
  expression; it depends on the variable's own type, and casts change nothing. Rare in game code; in C middleware
  `long` locals and parameters matter much more (`long i` turned `mul` into `smulbb`): try `long` early there.
  [2.0-all]
- `(n + 1) * x; ...; n++;` keeps the unmasked `n + 1` in the multiply.
- A pointer declared before a loop and assigned inside it (`e = &Items(list)[i];`) fixed a base/element swap.
- A struct copy declared inside a loop block stays an `ldm/stm` copy re-read from the stack; at function scope it is
  split into registers.

### Branches and selects

- Switch layout: cases are sorted and merged left to right into jump-table clusters while count/range >= 0.4
  (computed in x87 floating point); the tree root is item (lo+hi)/2. A density of exactly 0.4 (12/30, 2/5) is decided
  by the host's x87 precision: run the compiler with Windows' 53-bit precision (the pinned wibo, see
  [INSTALL.md](../INSTALL.md)). [host]
- Gaps in a jump table that lead to a label inside the switch are case labels the source is missing, not default.
- `x == 0 || x == 2 || x == 3` folds into a range check; `cmp #0; cmpne #2; cmpne #3` comes from
  `switch (x) { case 0: case 2: case 3: }`. Write `||` chains in the original's test order.
- A `switch` instead of an if-chain stops predication of the arms. A two-case `switch` gives a `beq` chain with the
  case-0 block out of line, where if/else-if predicates.
- Explicit nested ifs setting a BOOL keep a branch that a ternary or a selected `u8` would predicate.
- `else { return x; }` keeps a branch where `if (!c) return x;` predicates. Conditional early returns (`bxeq/bxcs`):
  `if (x != 0) { x = 0; return TRUE; } return FALSE;`, `if (id < N) return table[id]; return 0;`; the inverted forms
  predicate.
- Guards branching to one shared `mov r0,#0` exit are nested ifs with a single `return 0` at the end; early returns
  become predicated `movne/popne`. Two early `return i;` became `goto found;` to one `found: return i;` after
  `return 0;` to get the original branch shape.
- The compiler predicates the else arm: in an if/else whose predicated arm branches over the other, the predicated
  arm is the source's else.
- If-conversion runs after register colouring, before the second scheduler. An arm is predicated only when it has at
  most 3 instructions counting its closing branch (a then-arm at most 2 real instructions, an else-arm 3). Each arm is
  decided on its own, so one arm can be predicated while a branch is kept around the other. Predicated arms are placed
  before the branch, then-arm first, and the second scheduler mixes them with the block's other code.
  [sp2p3, tested]
- Identical trailing statements in both arms are merged into the join, and identical setter calls in both arms after
  the join; duplicated call tails are not merged. Write statements in both arms when the original keeps them there.
  The comparison is on the IR: `sz = sx + 0` or `sz = (s32) sx` counts as `sz = sx`, but an arm with `sy = sz = sx;`
  doesn't match one with `sy = sx; sz = sx;`. [sp2p3, tested]
- A helper called with a different constant per arm is compiled per arm and merged into `moveq/movne` only after the
  first scheduling pass, so a later call's argument setup isn't moved up.
- A select between exactly 0 and 1 always puts the 1 first: `x ? 1 : 0`, `x != 0`, `!!x`, an if/else assigning 0/1
  to a local, or an inline `return x != 0` becomes `movCC #1; movNCC #0` whatever the source order. `#0` first comes
  from a select of two enum constants, an if/else storing straight to memory in both arms, a variable with other
  definitions, or constants other than 0 and 1. `b = 0; if (x) b = 1;` gives `mov #0; movne #1`.
- `if (c) { x = 0; } else { x = 1; }` as statements can give a `moveq/movne` order that `x = c ? 0 : 1` reverses.
  `x == 0 ? 3 : 2` vs `x ? 2 : 3` swaps `moveq/movne`. `mode == 0 ? a : b` if-converts (predicated loads) where
  `mode != 0 ? b : a` branches.
- `f = c != 0 ? tbl[i] : g;` branches; `if (c == 0) f = g; else f = tbl[i];` predicates both arms.
- Bool stores: `if (x) { f = 1; } else { f = 0; }` computes into the result register and stores before the
  return-value move; `f = x != 0;`, `?:` and `!!` don't. Bool-returning inline getters:
  `bool r = false; if (cond) r = true; return r;` matches where `return cond;` doesn't.
- A predicated `movCC/strCC` right after a `bCC` on the same condition is a dead re-test inside the branch
  (`if (x <= K) { if (x > K) x = 0; }`): keep it.
- An `&` chain no single expression reproduces: `BOOL lo = (a >= 0) & (b >= 0); if (lo & (a < 0x100) & ...)`.
- Two loads before two compares: an enum-typed copy of a field read is a real load at its declaration and isn't
  substituted into the compare, so `Mode a = (Mode) x.mMode; Mode b = ...;` (`Mode` an enum type) keeps both loads
  ahead of the tests, where int-typed forms don't.
- An enum-typed result stops the return being folded (an enum-typed `result` kept `return result`, which as `u32`
  folded to `mov r0,#0`).

### Constants and literal pools

- `mvn rX,#0` stored to a byte: the field is signed (`s8` = -1).
- A negative constant stored with `strh` and built as `mov r0,#0xe000; sub r1,r0,#0x12000` (0xffffc000) can't come
  from a `u16` field (every form folds to 0xc000): the lvalue was signed 16-bit (written as a cast placeholder when
  the field's type can't change).
- A register-form `and` with a pool-loaded mask comes from a division plus `u16` truncation
  (`u16 v = (x & 0x1fff) / 0x200;`); a shift pair would fold.
- On `u16` values `/ 32` emits `lsr` and `>> 5` emits `asr` (promotion to int); on a signed value `f / 8` gives an
  `lsr` sequence and `f >> 3` an `asr`.
- Two literal-pool entries for one address are two relocations (different symbols, or symbol + offset) that resolve
  to the same address. Model it with a second extern name for the same address; really it means the statics' layout
  isn't understood yet.
- Constant offsets into an object in a file's shared data section are addressed from the section base (re-anchored at
  `off & ~0xff` for `strh`, `& ~0xfff` for `str`), so two tables can share one pool word. Taking the address as a value
  or indexing with a variable keeps the symbol. Which objects get a symbol of their own:
  [Data layout and link order](#data-layout-and-link-order).
- A struct member at the head of an array element (`mItems[k].mHead = ...`) folds the element base into the offset;
  `*(T*)&mItems[k] = ...` hoists it.
- A cast on the address swapped address and value registers in a read-modify-write of a global in game code: direct
  access coloured the address first; the cast form gave the value the lower register and its own pool word. In a
  small test, a cast written at each use compiled exactly like direct access, while a pointer local initialised with a
  cast (`T *p = (T *) &g;`) changed the addressing ("Operand and pointer shapes" in
  [Register allocation](#register-allocation)). Try both.

### Types and casts

- `-char signed`: `ldrsb` on a string walk means plain `char`.
- Return types show in callers: an enum return gives `movlo #0; movhs #1` at the last compare where BOOL gives the
  reverse; a BOOL stored into a `u8` field gives `ands rX, r0, #0xff` before the store; a `u8` return removes the
  caller's `and #0xff`. In a function returning `u8`, `u8 r = c >= x; return r;` has no final `and`; a BOOL local
  does.
- A callee declared void that really returns a value changes its callers' allocation and scheduling (ties):
  declaring the true type can fix many callers at once. The other way round, a tail-call function whose callers all
  ignore the result may need to be void to keep another caller matching (a judgement call: mark it as a placeholder).
- If callers load a register the callee never reads (e.g. the object pointer into r0), the callee still takes that
  parameter; removing it makes the callers 8 bytes short.
- Parameter width shows in the caller: passing a `u8` to a `u32` parameter makes a conversion temporary that shifts
  registers; `lsl #16; asr #16` on an argument means the caller's variable was `s16`; an `ldrsb` value passed with no
  `and` means the parameter is wider than `u8`. `(u8)` casts at call sites often stand in for the callee's real
  parameter types.
- Implicit narrowing at a short parameter is not shared between calls (the original keeps `x + 4` as int and narrows
  at each call, hoisting the narrowed stack arguments in source order); an explicit `(s16)` cast passed to an `s32`
  parameter is shared. Get the callee's types right and drop the casts. `(s16)(int)(y - 8)` stops the sharing (a
  placeholder form).
- A `u16`/`s16` stack parameter is loaded at the top of the entry block; a 32-bit one sinks to its first consumer.
  This helps only when every consumer has the same narrow type (a mismatch adds a conversion temporary and 8 bytes of
  frame). A `u8` stack parameter is loaded first in the prologue; BOOL puts it later.
- A stack parameter the callee reads with a narrow load while all callers store a full word: declaring it narrow
  breaks the callers (they add an `and`). A callee reading `*(s16*)&param` really takes an `s16`: declared so, the
  callers narrow late and match. Otherwise `*(u8*)&param` is a placeholder.
- A signed compare (`bgt`) on an `ldrh` value means `int v = field; if (v > K)`; comparing the field gives `bhi`.
  Unsigned bitfield compares are unsigned (`bcc`); `(s32) bf >= 2` gives `blt`.
- An index the original masks with `and #0xff` at each use was not a `u8` local: write the expression at each use.
- `x *= 10; x &= 0xffff;` keeps a call result and its narrowed product in one callee-saved register (`movs rN,r0`);
  `x = (u16)(x*10)` splits them. Narrow read-modify-write toggles: `f++; f &= 1;` on a `u8` field, as two statements.
- A `(u8*)` cast store gets registers differently from a real `u8` field store.
- A byte array that is really an array of small structs: struct indexing stops address sharing across a call. A
  two-byte `{x, y}` struct instead of `u8[2]` keeps a table out of a section of its own
  ([Data layout and link order](#data-layout-and-link-order)).
- Separate globals, not a struct: count/head/free/tail bytes matched only as five globals (struct accesses reorder
  across each other). Several file-level `static u8` are addressed from one `.bss` base like a struct but schedule
  differently; their offsets follow declaration order under `-ipa file`.
- `-enum int` vs `-enum min` moves struct field offsets after enum members (the evidence for the flag is in
  [compiler.md](compiler.md)).
- NitroSDK's `u32` is `unsigned long`; a project's own `u32` may be `unsigned int`. They differ to the compiler.

### Calls and stack frames

- Spill slots: spilled parameters take the lowest stack slots, in parameter order; spilled named locals go above them
  in reverse declaration order (first declared = highest slot); a spilled compiler temporary (e.g. a merged repeated
  global read) sits just above the parameters. Function-scope stack locals get slots in descending declaration order.
  A local that won't take its slot may need to be a repeated expression. Each colouring round hands out spill slots
  walking the virtual registers in increasing number, so within a round a higher number gets a higher slot
  (numbering: [Register allocation](#register-allocation)), and a later round's spills sit above. To swap two spilled
  values' slots, change which number each value reaches the colourer with. [sp2p3, read from the compiler]
- Declared before, computed after: declaring `b/bLevel` before `a/aLevel` but computing the `a` chain first changes
  which value is spilled, independently of either order alone.
- Stack-parameter copies are emitted at entry in parameter order; the arm946e scheduler moves a copy early only when
  its loaded value has a consumer in the same region. A local with two definitions that shares the parameter's home
  slot pulls its copy early; narrow parameter widths put that copy at the top.
- `stm` where the original has two `str`s for stack arguments: declare the flag without an initialiser and assign it
  only in the arm that needs it, passing literals in the other.
- A parameter copied into r1-r3 at entry and kept until a later call is usually that call's argument in that position
  (a missing `%d` argument can look like a register problem).
- By-value struct parameters: modify fields in place (`spec.prec -= 2`), don't copy them to locals (that changes the
  spill slots). A `double` parameter copied to a local puts its argument loads at entry, above an early return.
- `T a[2] = {0,0};` and `VecFx32 v = {0,0,0};` store through an `add rN, sp, #off` base; separate element stores,
  field stores and setter calls use sp offsets.
- A local array filled from a one-use rodata table is an aggregate initialiser (`u8 buf[3] = {0,1,4};`): it emits an
  anonymous rodata object (often first in the unit's `.rodata`) and changes the temporaries' creation order;
  element-wise copies don't match.
- Struct copies: where the original loads every component before storing (often `ldm/stm`), fill a local temporary
  (z, y, x) and assign `*dst = t;`. Three `u16`s copied as one object (`*(Vec3u16 *)dst = *(Vec3u16 *)p->mRot;`, a
  struct of three `u16`) load all three first. A struct copy initialised at its declaration stays `ldm/stm`; assigned
  later, it is split into fields. Inline setters compute all values before storing, and write-backs through them load
  the owner pointer once. A scalar argument of an inline setter is loaded before the struct copy in its body.
- Float compared with an int: keep the float expression in a helper returning float and cast at the compare
  (`(s32) ToFloat(n) < x`): it evaluates the float, then the left side, then `_ffix`.

### Inlines and intrinsics

- A macro where an inline won't do: a bit-test macro lets the compiler reuse the loaded byte for a following set,
  which inline helpers don't.
- Write SDK inlines and macros as the public API's inline functions and macros (`FX_Mul`, ...): their multiple
  evaluation and casts show in the code.
- `__clear(ptr, size)` (also `__builtin__clear`) is a CodeWarrior intrinsic: `mov r0,#0; mov r1,r0; mov r2,r0;
  mov r3,r0` and an `stm lr!,{r0-r3}` loop. `memset(p,0,n)` compiles to a call; a zero-initialised struct goes
  through the stack.
- `__memcpy(dst, src, n)` is an intrinsic too: inline `ldrb/strb` pairs (source in r3), unlike `memcpy` or struct
  copies (the C runtime's printf uses it).
- With `-inline on,noauto`, calls within one file stay `bl`; `auto` would inline them (evidence for the flag).

### C++ (the Mobiclip glue code)

The game's own code is C. The Mobiclip glue code (`libs/mobiclip/`) and `rt_global_ctors.cpp` are C++.

- `new T(args)` emits a null check around the constructor call; `-Cpp_exceptions off` matters (on changes it). A
  guarded allocation that calls a constructor returning the object is `new T` (`operator new` and `operator delete`
  can be class members); the result of the `new` expression is a temporary.
- A virtual `delete p` calls vtable slot 1 (the deleting destructor) and needs no explicit null check. A plain
  `delete p;` has its own null check; an explicit `if (p) delete p;` changes the block shape: try both.
- A function that is only `operator delete(p); return p;` is a deleting destructor and belongs to its class's file.
- The compiler emits separate C1/C2 constructors and D1/D2 destructors for some classes; the original link dropped
  the unused copy (and strings only it used). They show as "unknown symbols" and are harmless.
- Vtables are emitted at the end of the file in reverse class order, only those the file emits (a key function
  defined mid-file still puts its vtable at the end). Inline virtual destructors are emitted with their vtables (D0,
  then D1). Destructors defined out of line after the other functions emit D1, D0, D2 (the linker drops D2): use that
  when vtable order and destructor order disagree.
- `-RTTI off`: vtables have a zero typeinfo slot.
- Inline destructors are emitted as weak copies in every file that uses them; the original link kept one.
- One class compiled under two names in different files shows as an undefined vtable only once the other links.
- An inline identity (`template <class T> inline T Id(T x) { return x; }`) around an operand can change the code:
  around a field read it moved the value to another register and split an `ldm`; around an argument it moved the
  argument's load ahead of an earlier call. `permuter.py` tries it in C++ files. It is a placeholder unless a real
  inline helper of the same shape exists.

### Data layout and link order

- Thumb anchoring (2.0 sp2p3): a constant member offset becomes the pool word object + (offset & ~0x7f) for words,
  & ~0x3f for halfwords, & ~0x1f for bytes. For an object the file defines, constant offsets go through the `.bss`
  section base plus the anchor; taking the address or a variable index uses the object's own symbol. A pool word that
  names no object start is an anchor: find the object it belongs to before declaring it.
- ARM anchoring: word and byte members are reached as object + (offset & ~0xfff), halfwords as object +
  (offset & ~0xff); pointer locals fold the same way. Register-offset loads carry the full address.
- Shared `.bss` order (2.0 sp2p3): ascending size; on a tie, definition order, except that the first-defined object
  goes last. Objects of 64 bytes or more get sections of their own, also smallest first. A smaller object after a
  bigger one starts a new file.

How the compiler orders a file's objects (exact on test files; real files need calibrating):
1. Objects join one list per file as they are declared, by prepending (so the list ends up reversed): one list for
   `.bss`, `.data` and `.rodata` together, each shared section emitted at the place of its first member.
2. File-level statics join where defined, preceded by the string literals of their initialiser. String literals used
   inside functions don't join (they are emitted afterwards, in creation order). Function-local static tables, and a
   local array initialiser's `@N` in `.rodata`, join at their function's position. File-scope objects without an
   initialiser (static or global) join after all the others, in reverse definition order; `extern` declarations
   never join (model.c's four `.bss` objects are defined in the order that gives the game's `.rodata` order).
   [sp2p3, tested]
3. In C++, a used singleton accessor (`static T &Get() { static T i; return i; }`) adds its guard, destructor-chain
   node and instance where its header is first included, and vtables join at the end in reverse class order.
4. The list is heapsorted by size (max-heap, strict compares): sizes ascend, and equal sizes come in an order that
   depends on the whole list. So a smaller object can never follow a bigger one from the same list.

So:
- Static definition order decides all three sections. `tools/layout_search.py <unit>` searches static order (and, in
  C++, singleton include order; it refuses unless it reproduces the current compile first), `tools/fix_static_order.py`
  applies a static order, `tools/data_order.py` works out a declaration order for one section, and
  `tools/fix_layout.py` tries C++ header orders. `.clang-format` keeps include order (`SortIncludes: Never`).
- Define every table in the range: `.rodata` tables take part in the same sort as `.data` ones.
- Merging tables into one struct changes what the sort can reach (a struct of two tables puts a 10-byte table before
  an 8-byte one). Byte tables indexed by a variable stay separate (inside a struct the index becomes an immediate
  offset).
- The order of identical string literals in a table initialiser follows from the whole list (`-str noreuse` keeps
  them as separate copies).

Objects that get a section of their own (and are addressed through their own symbol):
- Objects the file never references; byte-aligned const tables whose size is a multiple of 4; function-local mutable
  statics (except under 2.0 sp2p3 with the game's flags, where they join the shared `.bss` and are addressed through
  the section base); every string literal; arrays of `char`/`u8`/`s8` (1-D char arrays always, even referenced; 2-D
  char arrays and same-size structs stay shared); `extern` objects; weak objects (inline-function statics, template
  statics); objects under `#pragma section npdata begin/end`. Under sp2p2 nothing else matters (size, alignment,
  order, static vs global, optimisation level); 2.0 sp2p3 and sp2p4 also give every object of 64 bytes or more a
  section of its own [SP-diff, tested; see [compiler.md](compiler.md)]. These sections are emitted at their place in
  the size sort, after the shared one. mwld starts each section on 4 bytes.
- `const` always wins: a const object goes to `.rodata`, so wrong const-ness shows as a section landing in the other
  kind (`.data` where the original has `.rodata`).
- A zero-initialised table the original has in `.data`: `#pragma explicit_zero_data on` around it (`= {0}` alone
  goes to `.bss`).
- A const table read only with constant indices is folded into immediates (no relocation reaches it), yet it is still
  referenced and joins the shared section. Look for a gap's values among a function's immediates before calling it
  dead data, and read it in code to keep it referenced.
- Byte-identical code addressing the wrong `.bss` offset means the statics' definition order is wrong.

Where a file begins and ends:
- A smaller object between two larger ones means a boundary; another unit's data on both sides means a shared file;
  an object in a neighbour's block means the code using it belongs to that file. In C++, literals lying before a
  vtable mean an earlier file (function literals are emitted after a file's shared `.data`), and deleting-destructor
  stubs sit at the start of their class's file.
- When no declaration order works, look for a boundary problem first.

Link order and linking:
- Function order = definition order. A function defined before its caller (for the declaration) is emitted first:
  use a prototype and define it after. `-inline deferred` would reverse the function order (not used).
- [mwld] `-dead` strips unreferenced data once a unit links from source, though the original link kept it: list such
  objects in `config/usa/arm9/force_active.txt` (written into a FORCE_ACTIVE block; mwldarm's `-force_active` flag
  crashes under wibo). Unreferenced statics are dropped by the compiler; unreferenced globals take part in the sort
  and get sections of their own.
- [mwld] Linker-script aliases (`alias` lines in `config/usa/arm9/lcf_edits.txt`; `tools/lcf_force_active.py` lists
  the edit kinds) must go after the sections; before them they silently resolve to 0. Other units that name fields
  inside a big object of a linked unit get those names this way (`alias` and `inner` lines).
- [mwld] Weak vtable copies get stripped: write that unit's `.data` range by hand and mark the symbol weak.
- [mwld] mwcc emits inline-function statics with a processor-specific "multidef" binding (13) that mwld resolves to
  the first definition, and mwld prefers multidef over weak. To make the original's (delinked) copy win: demote
  compiled multidef to weak (14), promote delinked weak to multidef, and put delinked objects first.
- A unit whose code sits apart from its data (arm_system.c) gets a stub unit (arm_system_slot.c) that holds the
  code's slot, and `place arm_system_slot.o .text arm_system.o` in `lcf_edits.txt`; the unit's delinks entry then has
  no `.text` range. Zero space the linker itself lays down (the fill before a unit at a fixed address, the
  overlay-signature slot at the end of `.data`) is an `org` or `reserve` edit, not an object.
- [dsd] Once interior symbols of an object are removed, relocations into it must be written `to:<base> add:<off>`.
- [dsd] The ROM build rewrites BuildInfo's autoload words, so only `ninja check` checks the aliases that define them;
  the ARM9 footer's overlay-signature offset comes from the linked `OverlaySignatures` symbol.
- [dsd] dsd reads any word that looks like an address as a pointer: a word of a data blob (a font bitmap) recorded as
  a relocation becomes 0 when the target is renamed away. Delete such lines from `relocs.txt`, and check relocations
  into a newly linked range when the ROM differs by one word.
- [dsd] The `.text`/`.rodata` boundary may be set too late (tables counted as code). Fixing it lets units own their
  tables and corrects the progress numbers; hand-written routines inside data then live in `.rodata`.
- [dsd] dsd delink refuses a relocation from one unit to a local symbol of another: never make a symbol local while
  an outside relocation targets it.

### Soft float and runtime helpers

- `float` arithmetic calls runtime helpers (`_fadd _fsub _fmul _fdiv _fflt _ffltu _ffix _ffixu`, compares `_fgeq
  _fgr _fleq _fls _feq _fneq`). A source compare must use the same operator as the original (`a < b` gives `_fls`,
  `!(a >= b)` gives `_fgeq`).
- `s->x = 8;` on a float folds to a constant; `u32 left = 8; s->x = left;` keeps `mov r0,#8; bl _ffltu`.
- `/` and `%` on ints call a combined division helper (`_s32_div_f`: quotient in r0, remainder in r1). Write the
  operator, never call the helper by name. The divisor in r1 often already holds a value used elsewhere (keep it in
  a local). Compound-assigned locals (`s32 q = a; q /= b;`) fix the order of division calls; one big expression lets
  them be reordered.
- 64-bit fixed point (NitroSDK-style matrix concatenation): a product of two locals gives `umull` + `mla` with sign
  words on the stack; a local times a fresh load gives `smull/smlal`; both in one function shows the source's shape.
  `(fx32)((e64 >> 12) + t)` gives `adds`. Matrix concatenation reuses the last column loaded in a row for the next
  row's first element (load the columns into locals, alternating column order row by row).
- A 64x32 multiply with a known-zero high word: up to 2.0 sp2 it emits `umull` + `mla` against a zeroed register;
  sp2p2 and later drop the `mla`. [SP-diff]
- The soft-float and division runtime itself is hand-written assembly (assembler files in `libs/msl/src/`).

### Assembly

- Most hand-written routines are commented `asm` function bodies in C files, among them the start-up code, the CPU
  copy and fill routines, the cache, protection-region, interrupt, exception and thread-context routines, the divider
  save/restore pair, some matrix and graphics FIFO routines, and the Mobiclip YUV-to-RGB conversion. The soft-float
  and division runtime, the secure area, the build-info block, the game's engine routines
  (`src/Main/System/engine_asm.s`) and its script data (`src/Main/Graphics/script_data.s`) are assembler files. The
  comment rules are in [CONTRIBUTING.md](../CONTRIBUTING.md#rules).
- Each compiled function gets its own 4-byte-aligned section, so Thumb BIOS thunks at halfword addresses can't come
  from C: use an assembler file (mwasmarm), which can place global labels anywhere in one section.
- Labels inside one asm function aren't visible to another: a branch into the middle of a neighbouring routine is a
  raw instruction word (`dcd`) with a comment. objdiff then reads it as data and scores below 100% though the bytes
  match.
- [mwasm] mwasmarm reads `.incbin` input as text (a lone 0x0a becomes 0x0d), so the build expands `.incbin` lines
  into `.byte` lines in a copy under `build/`. The 2.0 sp2p2 package ships no assembler; the build uses 2.0 sp2p3's
  (`MWASM_VERSION`).
- Opaque filler bytes (the secure area) are read from your ROM at build time, never committed.

### objdiff and dsd quirks

Identical code that objdiff still scores below 100%:
- A `bl` to the function that starts right where the caller ends stays a mismatch (99.99%): symbol + -8 addend lands
  inside the caller. Linking is the proof.
- A `bl` to another function of the same unit counts as a mismatch while that callee doesn't match.
- Stripped weak copies (inline destructors) in our objects shift objdiff's view of later relocations (99.9x%).
- Data inside asm bodies (no `$d` mapping symbol), and a data block whose delinked target has no mapping symbol
  (scored as instructions: 40% for a parameter block).
- Relocation names in the literal pools of units not linked yet.
- A Thumb `bl` from a function into its own body (a far branch to a shared exit block) gets a relocation from
  `dsd init`, but the compiler resolves it without one, so the delinked original has a relocation the compiled object
  never has (99.99%). Remove such relocations (source and target inside one function) from `relocs.txt`.

`ninja objdiff` runs `tools/objdiff_place.py`, which gives each unit's decomp.me scratch its own compiler and flags.

### Thumb code

- Thumb game code gets `-thumb` through `PATH_ADD_FLAGS` in `tools/project.py`. `#pragma thumb off` ...
  `#pragma thumb on` around a function in such a file gives exactly the code of compiling it in an ARM file (checked
  on every 1.2, 2.0 and DSi build), so an ARM function can sit in a Thumb file. The ROM can't show whether the
  original set the mode per file or per function.
- A Thumb `bl` is two halfwords (one 4-byte instruction) and can straddle a word boundary: mask relocations per byte.
  objdump prints it as one line.
- Thumb functions end on a halfword, but a unit's range ends after the padding, on a 4-byte boundary (the linker
  places every section on 4 bytes, and a halfword end shifts everything after the unit).
- Thumb code has no `umull`/`smull` for 64-bit products; it calls a runtime helper, so the multiply idiom that
  separates 2.0 sp2 from sp2p2 only exists in ARM code.
- Thumb has no conditional execution other than branches: where ARM code would predicate an arm, Thumb code branches.

## Placeholders

Forms that give the original's bytes but are probably not its source. Use one only when the bytes match and the code
still behaves the same, mark it in place with a comment saying what it stands in for, and replace it when a real
form matches. Kinds that have worked:
- Layout: a dummy static of the right size in the data size sort (a missing object changes the order of equal-sized
  ones); named copies of identical initialisers to set their order; a 2-D shape that keeps a byte table in the shared
  `.rodata`; a stub unit holding the slot of code placed apart from its data.
- Registers and order: a pointer local for a hardware register read (numbers the address among the variables); a
  second extern name for part of a table (keeps a loop base a hoisted temporary); unread zero initialisers; volatile
  reads that keep two reloads apart; an enum-typed index; `*(u8 *)&param`; a repeated test `a || a`;
  `(s16)(int)(y - 8)`; a two-arm value whose arms read the same thing (`idx = c ? list->x : g.list->x;`), which keeps
  a value as its own statement past expression propagation and then merges to nothing.
- Expressions: `- -v`, a cast through a byte pointer, a zero base, a constant stored through `*(s16 *)&field`. The
  type or variable structure that does the same is the real form.
- A tail-call function declared `void` to keep another caller matching.

Before reaching for one, check the real forms above: two loads before two compares (enum-typed copies), scheduling
"cycles" and enum-zero workarounds (a BOOL-returning callee declared void), literal-order problems (wrong strings
behind masked addresses).

## objdiff below 100% with identical code

The causes are in [objdiff and dsd quirks](#objdiff-and-dsd-quirks); the ROM check is the proof.

| Function | Score | Cause |
|---|---|---|
| `BuildInfo`, `data_02000bc4`, `data_02000bec` (build_info.s) | 40% | data objects in `.text`: objdiff compares them as code |
| `_fdiv` (rt_soft_float_b.s) | 83.87% | its 256-byte reciprocal table sits inside the routine; the assembled object marks it as data (`$d`), the delinked original decodes it as instructions. `dsd delink -M` would emit `$d` there but lowers other units, so it is not used |
| `data_02027398`, `data_02027410`, `gTextScreenFont` (engine_asm.s) | 20% | data objects in `.text` (`.incbin` blocks), as for BuildInfo |
| `data_02028270`, `data_0202829c` (engine_asm.s) | 40% | data objects in `.text`, as for BuildInfo |
| 90 of the 166 objects `data_0201a608`..`data_0201b7d2` (script_data.s) | 15-40% | data objects in `.text`, as for BuildInfo; the assembled object matches the ROM byte for byte, relocations included |
