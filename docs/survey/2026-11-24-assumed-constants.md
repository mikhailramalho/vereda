# Constants from assumptions and branches

Recorded 2026-11-24.

Issue #120, first step. When a path's condition gains `x = c`, with x a term
and c a bitvector constant, `Encoder::value` returns c for every value bound
to x for the rest of that path, so the terms built later fold. The sources
are `__VERIFIER_assume`, an assumed `__VERIFIER_assert` (a property under
which it is no check), a branch edge, and a switch edge reached by one case.
`Terms::equalities` finds the equalities in the condition: through `and`
when it holds, `or` when it fails, `not`, and a Bool `ite` with a constant
arm (`a && b`, `a || b` as LLVM writes them with `select`). It visits at most
64 nodes: conditions share subterms, and walking one as a tree took
exponential time on esbmc-cstd/strxfrm (0.2 s to a timeout) before the bound.

The facts belong to a path. Each arrival at a block and each return carries
them, a join keeps the ones every arrival has, and a call continues with the
ones all its returns have, which is how `assume_abort_if_not(x == 5)` (a
branch in a callee) reaches the caller. They live in memory's persistent
`CellMap`, keyed by the term's address, so an arrival copies a pointer. A
`DenseMap` copied per arrival made a loop that assumes one fact per
iteration quadratic: 3000 iterations walked in 1.28 s against 0.15 s on
main; with the `CellMap`, 10000 iterations walk in 0.51 s against 0.42 s,
and the solver has nothing left to do (0.20 s to 0).

## Why it is sound, and the traps

A fact is an equality of terms that the path condition contains, and
replacing a term by an equal one changes nothing under that condition. Every
term built on a path is used only under the path's guard: checks and the
next guard are conjunctions with it, phi values and memory states join by
`ite` on the arrivals' guards, and the memory stage's global constraints on
an object hold only where the object exists, which is on that path. So the
work is in finding only equalities the condition implies, and in dropping
them where paths join.

- Floats. `fcmp oeq` is camada's `fp.eq`, which Terms cannot see into, so
  `x == 0.0` gives no fact and x keeps both zeros
  (`assume_constant_float_zero_fail`, also under `--fp2bv`). An equality on
  a float's bits, from a bitcast or `--fp2bv`, is exact, so it may count.
  `x != 1.0` holds for a NaN and gives nothing (`assume_constant_float_nan_fail`).
- Pointers. In Vereda's model `p == q` is equality of object and offset,
  which is what Terms' `=` states, so a fact on a pointer term is exact; an
  assumption on `(uintptr_t)p` fixes that address term, not p's object
  (`assume_constant_pointer`, `assume_constant_pointer_address_fail`).
- Width and sign. The fact names the term the comparison used, so
  `(signed char)x == -1` fixes the truncation, never x
  (`assume_constant_width_fail`).
- Joins and polarity. A fact from one branch does not survive the join
  (`assume_constant_join_fail`); `x != 5 || y == 6` fixes nothing, its
  negation both sides (`assume_constant_or_fail`); each loop iteration's
  fresh value gets its own fact (`assume_constant_loop`).

`assume_constant` checks that the forms fold: every check is `false` in the
dumped formula, where main asserts a disjunction of seven checks.

## Measurements

Base is main at 8333f006, head this branch, both Release with LLVM 21.1.2,
run side by side.

The 107 regression tests whose source has `__VERIFIER_assume(` or
`assume_abort_if_not(` with `==` (not FUTURE), with `tests/bench.py --list`:
no verdict changes, walk 0.07 s to 0.07 s and solve 5.82 s to 5.83 s in
total, no test past the bench's margins. Six of them keep fewer checks after
the walk (esbmc/github_10891_write_offset 2 to 0,
esbmc/github_8517_assume_pins_pointer 2 to 0, its `_fail` 2 to 1,
esbmc/github_8706 1 to 0, and two of the new tests), but all of these were
already fast.

ReachSafety with `--incremental-bmc`, 60 s and 6 GB per run, both binaries on
each task at once, 8 tasks at a time:

| Tasks | Answered by both | Verdict changes | Wrong | Wall, both answered | Solve, both answered |
|---|---|---|---|---|---|
| 55 with an `==` assumption | 9 | 0 | 0 | 19.2 s → 18.7 s | 17.2 s → 16.8 s |
| ControlFlow and Heap (209) | 161 | 0 | 0 | 271.8 s → 267.0 s | 240.1 s → 234.7 s |

No task moved by more than 1.5 times. The 55 are mostly nla-digbench fermat
tasks and array-tiling, whose assumptions compare a remainder or a product
with a constant, not a variable, so nothing folds there. In an earlier run of
ControlFlow and Heap, ldv-sets/test_mutex_unbounded-2 and memory-model/2SB
timed out with head and not with base; with 300 s both binaries answer both
(63 s against 66 s, 55 s against 50 s), so that was the 60 s limit, not the
change.

Locally the vereda suite and the imported cbmc, floats, cstd, k-induction,
loop-invariants and unix2 suites pass; of the esbmc suite, all 3551 tests that
finished within 5 minutes pass, and the other six time out with main too.

## Not done

Ranges (step two of #120): an interval per term, seeded by `x < c` and
branches, deciding later comparisons and loop exits. It needs its own
lattice and join, so it is a pull request of its own. Also left out: a fact
that a Bool atom is true (`assume(flag)`; #109's guard folding covers the
same branch condition), `x = y` between two non-constant terms, and an
equality under an extension (`zext(x) == c` fixing x).
