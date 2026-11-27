# Path facts beyond ranges

Recorded 2026-11-27.

Issue #296, after #120's constants and ranges. A path's conditions now
give more facts, and the facts settle more of what the walk builds: the
value a load reads, each case of a switch, and the overflow tests of the
sanitizers, besides the icmp being encoded.

## Design

What a path learns (`Encoder::learn`). As before, an assumption, an
assumed `__VERIFIER_assert`, a branch or switch edge and a summarised
iteration's `k < trips`; never a counted check, after which the path goes
on where the check fails (#329). New:

- `x = y` between two terms. When one of them is a leaf (a symbol, or a
  term Terms built no node for), it stands for both: the other's fact is
  `equal = leaf`, a chain that `Encoder::root` follows, so `value()` returns
  the leaf for either and what is learnt later of one bounds the other
  (`assume(x == y); assume(y < 10)` settles `x >= 10`). A chain's bounds
  live on its last term; `Encoder::known` reads them there. When neither
  side is a leaf the two only meet each other's bounds (`Terms::narrow`).
  The field that held #163's constant is now `equal`, a constant or a leaf.
- Extensions, both ways. A bound on `zext(x)` bounds x (unsigned), one on
  `sext(x)` bounds x (signed), recursively; `zext(x) = c` or `sext(x) = c`
  fixes x to c's low bits when c is a value the extension can give
  (`Terms::equalities`). Truncation needs nothing new: a range that fits
  already carries through `extract` (#204).
- `x != c` where c is an end of x's range moves that end off c, signed and
  unsigned. This is what a switch's default edge (`!(x == c1 || ...)`) and
  the else side of `x == c` give. A c inside the range is not kept: that
  would need a set of excluded values per term.

Where facts fold (`Terms::assuming`, through `Encoder::assuming`). It
walks a term top down over at most 256 nodes: a comparison or an
overflow test `decide` settles becomes its constant, an ite whose
condition it settles becomes that arm, a select passes over each store
whose index the read's is never equal to under the path's bounds (and
reads the stored value where the index is the same), through not, and,
or, ite, concat, extract and the extensions. The encoder applies it to
the icmp it encodes (as #120 did with `decide` alone) and to the overflow
bit of `llvm.*.with.overflow`; the walker to each load's value, each
switch case's `x == c` (a case no value in range takes is pruned
there, with its edge), and the divrem sanitizer's zero test.

`decide` now also answers: `=` when both sides have one value, the same;
and an overflow test is false when the operands' ranges keep the result in
range (unsigned add, sub and mul; signed through `srange`). `srange` carries
a signed interval through add, sub and mul whose result cannot wrap, eight
levels deep since it keeps no cache. A zext of a term with no range has the
range of its operand's width. A select at the index of the store just below
it is the value stored. These three hold everywhere, not only on a path.

## Why it is sound

A fact holds on every execution of its path, as #120's do, and facts stay
in the same persistent map that each arrival, return, call and checkpoint
already copies (`common` keeps at a join what every path agrees on: a chain
link only if all paths have it, so the joined chains are a subset of each
path's and stay acyclic). A chain is made only from a root to another root,
so following it ends.

What a fact folds is bound to one instruction (the icmp, the load, the
overflow pair) or used for one edge (a switch case), whose uses run on
executions through that instruction, where the facts held and, being about
values of terms, still hold. `Terms::assuming` builds through Terms' usual
builders, which never read facts; its own results and the ranges it
computes under the path's bounds go to maps cleared per call (`scratch`),
never to `ranges`, `selects` or `reduced`. The memory state itself is never
rewritten: a load's folded value is only what that load returns.

A substitution x -> leaf inside a summarised iteration is undone with the
rest of the iteration's terms when `substitute` puts the element index n
for k, as #120 argued for constants: the store's guard, with n for k,
still holds the equality that gave it.

## Tests

`tests/vereda`: `path_facts` (every reach_error folds in the dumped
formula: x = y with later facts on either side, a bound through zext, sext
equal to a constant, x != 0 reaching a callee, a switch default and a case
outside the range, a load past a store whose index cannot meet the read's),
`path_facts_overflow` (signed and unsigned add, sub and mul checks fold
under `--overflow-check --unsigned-overflow-check`), and the failing
versions with each check at the edge of what the path knows or on a path
that does not know it: `path_facts_fail` (eight failures, one in a callee),
`path_facts_overflow_fail` (five), `path_facts_incremental_fail` (x = y and
a bound learnt on one side of a branch, under `--incremental-bmc`, whose
loop checkpoints are resumed at each bound).

Two tests named a check by its number after one that now folds: the VLA
size check of `vereda/uninit_array_false` (n is in [2, 8]) and, in
`esbmc/scanf_percent_n_reached_fail`, an assertion under `r < 1` where
sscanf of "12" returns 1. Vereda numbers a check when it is first reached,
so a check pruned on every path shifts the ones after it; both now match the
failing assertion by its line. ESBMC numbers assertions statically.

## Measurements

Base is main at 29759530 (#338), head this branch, both built with LLVM
23.1.6; cores 60-69 of a shared machine.

Regression suite: all 6,772 tests run locally with `REGRESS_STATS`, both
binaries pass every test but the two dump tests above on base; no verdict
differs. `tests/bench.py --stats` picked one test whose numbers moved:
`esbmc-z3/github_9222`, wall 2.82 s to 0.16 s (the solver's 2.69 s to
0.03 s). The 30 slowest tests by base's wall time, three `tests/bench.py
--list` runs (each test's two runs side by side): wall 370.8 s to 364.3 s
summed over the three, walk 44.5 s to 44.8 s, solve 211.4 s to 205.3 s, no
regression flagged (`esbmc-csmith/csmith04` is flagged on both binaries: it
does not compile). The largest moves the other way, steady in all three
runs: `esbmc/sprintf_symbolic_digits` 0.63 s to 0.80 s,
`esbmc-cstd/strcmp_bug` 0.87 s to 1.11 s, `vereda/lib_sprintf` 0.69 s to
0.88 s. In `strcmp_bug` the load folds make the formula smaller (566 kB
against 631 kB dumped) and the solver slower on it; in the sprintf test the
walk's own solver queries take the extra time.

sv-benchmarks: 300 tasks sampled (seed 296) from the unreach-call and
no-overflow tasks of nla-digbench-scaling, the loop-* directories, loops,
ntdrivers-simplified, openssl-simplified, array-examples,
infeasible-control-flow, signedintegeroverflow-regression and
unsignedintegeroverflow-sas23, run with `vereda.py --incremental-bmc
--property-file`. A first pass at 15 s: 194 tasks ended on both binaries,
106 timed out on both, no verdict differs. The 194 then ran three times
each with both binaries, interleaved (order shuffled per repeat, base or
head first alternately, eight runs at a time), medians:

| tasks | wall | walk | solve | checks |
|---|---|---|---|---|
| 194 | 160.0 s → 143.2 s | 30.5 s → 31.7 s | 99.4 s → 80.6 s | 1146 → 1119 |
| 161 unreach-call | 146.4 s → 129.4 s | 27.7 s → 29.0 s | 94.1 s → 75.7 s | 938 → 926 |
| 33 no-overflow | 14.0 s → 14.4 s | 2.8 s → 2.9 s | 5.1 s → 5.0 s | 208 → 193 |

No verdict changed. 14 tasks keep fewer checks, none more. The gains are
in nla-digbench-scaling: `divbin_unwindbound10` 9.3 s to 2.2 s,
`divbin_unwindbound100` 8.2 s to 4.7 s, `lcm1_unwindbound5` 4.2 s to
1.0 s, `lcm1_unwindbound10` 4.8 s to 2.1 s, `prodbin-ll_valuebound50` 9.8 s
to 8.5 s; the largest loss is `array-examples/standard_strcmp_ground` 13.2 s
to 14.0 s. Of the 106 tasks that time out, the walk is a little slower per
bound: the three endless-loop tasks of `loops` reach 2 to 4 % fewer bounds
in 15 s, `loop-invgen/id_build` 26 % more.

## Not done

A value excluded inside a range (`x != 5` with x in [0, 10]) is not kept,
nor is a relation between two terms that is not an equality (`v <= u`
leaves `u - v`'s overflow test to the solver). `srange` keeps no cache,
hence its depth limit.

Follow-up issues: #367, #368, #369.
