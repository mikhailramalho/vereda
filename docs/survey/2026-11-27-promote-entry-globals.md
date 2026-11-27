# Promoting globals only the entry function uses

Recorded 2026-11-27.

In `array-lopstr16/partial_lesser_bound-1`, three loops run to 10^7 over a
global `int i`. Because `i` lives in memory, SCEV sees no induction variable,
the array loop summary (`analysis/loops.h`) does not apply, and
`--incremental-bmc` resumed about 184,000 bounds before the time limit (#380,
cause 5). With `int i` declared in `main` instead, the three loops are
summarised and the task takes 0.2 s.

`ir::load` now turns such a global into a local of the entry function, after
the C library models are linked (so that a model using a global of the same
name still shares it), and runs mem2reg, loop-simplify and lcssa again. A
global qualifies only when nothing but the entry function can see it:

- the entry function has no uses at all (C allows calling `main`, also
  recursively, and each call would need the same object);
- the module has no inline assembly, which can name a global without an IR
  use, and no call in the entry function returns twice (`setjmp`);
- the global has an initializer, local or external linkage, no section, is
  not constant, thread-local or externally initialised, and holds an integer,
  pointer or floating-point value;
- every use is a plain (not volatile, not atomic) load or store of the whole
  value, as the pointer operand, in the entry function. A call argument, a
  stored address, a constant expression or `llvm.used` disqualifies it, and
  so constructors, destructors, `atexit` handlers, signal handlers and
  threads cannot reach it either.

The alloca holds the initial value from the entry. The global's debug
variable becomes a local one with a value record after each store, at the
store's location: the records mem2reg makes from a declare sit at line 0,
which counterexamples show at the variable's declaration, outside `main` for
a global. As before for a global, the initial value is not shown.

One test changed: `endless_loop_store_false` checks the unwinding claim of
an endless loop that stores to a global, and that store now goes away with
the promotion, so the loop counts as one with no effect (#373). The test now
stores to a volatile global. The same holds for programs: an endless loop
whose only effect is a store to a global only `main` uses now ends its path
as an effect-free loop does, so under `--unwind N` such a program, which
failed its unwinding assertion before, may now pass. That is #373's
dependence of the unwinding verdict on the IR's shape, which this change
makes reach more programs; it does not change what can be reached, so
reachability verdicts stay the same.

## Measurements

Main at a6dcab95 against the branch, side by side on cores 0-39 of the
80-core machine, at most 16 runs at a time, each run once with the SV-COMP
flags (`--incremental-bmc --property-file unreach-call.prp`), 60 s and
6 GiB of address space. The 35 tasks of `array-lopstr16`,
`loop-invgen` and the fermat tasks of `nla-digbench-scaling`, where globals
are common. Two changed:

| task | main | branch |
| --- | --- | --- |
| array-lopstr16/partial_lesser_bound-1 (true) | timeout | 0.21 s, TRUE |
| array-lopstr16/partial_lesser_bound (true) | timeout | out of memory at 41 s |

`partial_lesser_bound` mallocs in its loop, so it is not summarised; with
the counter visible to SCEV the bound doubles faster and runs out of memory
before the time limit. Neither answers. The other 33 tasks gave the same
result in about the same time (within 1 s). The fermat tasks, whose loops
test a global `counter`, still time out: the cut puts the unwinding
assertion before the header's test (#380, cause 3).

All 695 `vereda` tests and the 6,537 imported ESBMC tests pass.
