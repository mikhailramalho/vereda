# The decrement test's solve against ESBMC's

Recorded 2026-11-24.

Issue #99, a second look after word elements (#107) and
`bv-cnf-scope=permanent` (#104). `tests/esbmc/array_loop_summary_decrement_fail.c
--no-summarize-array-loops` writes `a[0]` to `a[99999]` in that order, then
asserts `a[x] == 7 - 2*x` for a nondet `x < 100000`; every `x` breaks it. On
main (5d351dc0, Release, mimalloc) the walk takes 2.2 to 2.5 s, the solve 3.9
to 4.4 s and the run peaks at 0.89 to 0.96 GB. ESBMC 8.7 with `--bitwuzla
--no-summarize-array-loops` on its own copy of the test takes 6.2 to 6.5 s of
symex, 0.7 s of slicing, 1.5 s of encoding and 1.58 s in the solver. The
machine has 80 cores and ran other jobs (load 10 to 25); compare numbers
within a table.

## What each tool hands the solver

| | Vereda | ESBMC |
|---|---|---|
| Stores | 100000, one per element, constant index and value | the same |
| Elements, index | 32 bits, 62 bits (byte offset / 4) | 32 bits, 17 bits (`extract` of a 64-bit constant) |
| Base | constant array of 0 | the same |
| Read | one select at `extract 63 2 (zext x * 4)` | one select at `extract 16 0 (zext x)` |
| Rest | 3 assertions on base addresses | 10 on pointer objects and copies of `x`; the bounds check of `a[x]` or'ed with the property |
| Posed | property under `check-sat-assuming` | everything asserted at level 0 |
| Size | 10.3 MB | 12.3 MB |

ESBMC's symex carries the constant `with` chain into the read, so the
300,000 assignments to `a`, `i` and `j` become dead and the slicer drops them
("6 assignments" left); of the 100002 VCCs, the bounds checks of the writes at
constant indices fold to true and 2 remain. Vereda's walker does the same
through its known bytes. Neither tool slices away a store the property could
read: `x` ranges over all of them.

## Where the solve goes

Each formula ran on its own through Bitwuzla 0.9.0 with CaDiCaL 2.1.2 (the
command-line tool ESBMC builds) and Bitwuzla 0.11.0 with CaDiCaL 3.0.1
(camada's library through its parser, with `bv-cnf-scope=permanent` as
Vereda sets it), `-v 2` and statistics on, all runs of a table at once. "As
ESBMC" asserts the property at level 0; "as Vereda" passes it to
`check-sat-assuming`. x is the value in the final model; the longest lemma
counts the stores its walk passed. Seconds:

| Formula, posed | Bitwuzla | x | lemmas | longest | AIG ANDs | preprocess | array check | bit-blast | CNF | SAT | solve |
|---|---|---|---|---|---|---|---|---|---|---|---|
| ESBMC's, as ESBMC | 0.9 | 99999 | 1 | 1 | 285 | 1.39 | 0.00 | 0.00 | 0.00 | 0.00 | 1.90 |
| ESBMC's, as ESBMC | 0.11 | 26 | 9 | 100000 | 1100193 | 2.42 | 5.39 | 1.36 | 2.95 | 0.50 | 14.32 |
| ESBMC's, as Vereda | 0.9 | 49860 | 1 | 50140 | 150697 | 1.34 | 0.49 | 0.32 | 0.29 | 0.03 | 3.18 |
| ESBMC's, as Vereda | 0.11 | 99812 | 1 | 100000 | 300271 | 1.55 | 1.12 | 0.56 | 0.73 | 0.24 | 5.04 |
| Vereda's, as ESBMC | 0.9 | 99999 | 1 | 1 | 598 | 0.52 | 0.00 | 0.00 | 0.00 | 0.00 | 0.83 |
| Vereda's, as ESBMC | 0.11 | 18 | 9 | 100000 | 1100509 | 0.81 | 5.50 | 2.17 | 2.94 | 0.57 | 13.36 |
| Vereda's, as Vereda | 0.9 | 4 | 1 | 99996 | 300574 | 0.53 | 1.17 | 1.28 | 0.62 | 0.10 | 4.38 |
| Vereda's, as Vereda | 0.11 | 99814 | 1 | 100000 | 300585 | 0.82 | 1.16 | 1.31 | 0.75 | 0.28 | 4.96 |

As #104 found, Bitwuzla treats the read as a free value, and when a model
gets it wrong it walks the chain from the outermost store down to the store
at the model's `x`, adding a lemma with one disequality per store it passed.
ESBMC's 1.6 s is preprocessing (1.4 s against 0.5 s for Vereda's formula
on the same solver; ESBMC's has 100001 more `extract`s to fold) and one
lemma over one store: Bitwuzla 0.9's first model, at level 0, has
`x = 99999`, the store the loop wrote last. Vereda's 4 s is
preprocessing (0.8 s) and one lemma over all 100000 stores: Bitwuzla 0.11's
first model has `x = 0`, the store the loop wrote first, deepest in the
chain. That lemma costs 1.2 s to build, 1.3 s to bit-blast into 300000 AIG
ANDs, 0.75 s to turn into 900000 clauses and 0.3 s of SAT.

The likely reason 0.11 starts at 0: CaDiCaL 3.0 tries "lucky phases" first,
the first of them every variable false, and since 3.0 it does so under
assumptions too (`luckyassumptions`, on by default); 2.1.2's
`lucky_phases` returns at once when there are assumptions. Every 0.11 run
above walks to store 0 first; 0.9 under an assumption, without lucky
phases, starts at 4 or 49860.

So the gap is not in Vereda's formula. Vereda's formula posed as ESBMC poses
it, on ESBMC's Bitwuzla, solves in 0.83 s, under ESBMC's own 1.90 s; ESBMC's
formula on Bitwuzla 0.11 takes 14.3 s as ESBMC poses it and 5.0 s as Vereda
does. The gap is Bitwuzla 0.9 against 0.11 (CaDiCaL 2.1.2 against 3.0.1)
picking the first `x` at opposite ends of a chain the loop wrote upwards.

The one difference in encoding that shows is the index width. With the
62-bit index narrowed by hand to 32 and 17 bits (0.11, as Vereda):

| Index | bit-blast | solve |
|---|---|---|
| 62 bits | 1.31 | 4.96 |
| 32 bits | 0.87 | 4.59 |
| 17 bits | 0.59 | 4.21 |

## The chain the other way round

With the same 100000 stores in reverse, `a[0]` outermost (the indices are
distinct constants, so the order changes nothing):

| Formula, posed | Bitwuzla | x | lemmas | longest | AIG ANDs | preprocess | solve |
|---|---|---|---|---|---|---|---|
| Vereda's, as Vereda | 0.11 | 22 | 2 | 20 | 684 | 0.81 | 1.32 |
| Vereda's, as Vereda | 0.9 | 4 | 1 | 5 | 608 | 0.54 | 0.84 |
| Vereda's, as ESBMC | 0.9 | 99999 | 1 | 100000 | 300585 | 0.53 | 4.32 |
| ESBMC's, as ESBMC | 0.11 | 18 | 9 | 29 | 528 | 2.40 | 3.07 |

What is left is preprocessing, as in ESBMC's run.

Vereda chooses the order of these stores: `Memory::flush` moves the known
bytes to the arrays in address order. But it flushes every time a state
holds more than `MaxKnown` (4096) known bytes, so the stores come in chunks
of 1025 elements in the order the loop wrote them, and reversing the order
inside `flush` leaves `a[0]` in the deepest chunk. Both had to change:
`flush` highest address first (a `Down` flag on `CellMap`'s in-order walk)
and `MaxKnown` raised to 1 MiB, so that the whole array goes in one flush at
the read of `a[x]`. One run of each, one after the other:

| Change | Walk | Solve | Peak |
|---|---|---|---|
| main | 2.48 s | 4.36 s | 886 MB |
| descending flush only | 2.46 s | 4.95 s | 949 MB |
| `MaxKnown` 1 MiB only | 2.50 s | 4.20 s | 892 MB |
| both | 2.49 s | 1.08 s | 532 MB |

With both, `tests/bench.py --list` over the 34 array_loop_summary tests that
give a verdict, each with `--no-summarize-array-loops` added (120 s limit;
the rows that changed, named after `array_loop_summary_`):

| Test | Walk | Solve | Wall | Peak MB | Verdict |
|---|---|---|---|---|---|
| decrement_fail | 2.30 → 2.57 | 4.04 → 1.03 | 7.24 → 4.49 | 936 → 576 | FAILED |
| (the plain one) | 9.36 → 4.49 | - → 0.00 | - → 4.65 | 1145 → 851 | TIMEOUT → SUCCESSFUL |
| fail | 9.74 → 4.95 | 0.00 → 0.00 | - → 5.82 | 1135 → 851 | TIMEOUT → FAILED |
| nondet | 21.95 → 11.59 | - → 0.00 | - → 11.81 | 1933 → 1328 | TIMEOUT → SUCCESSFUL |
| nondet_fail | 10.80 → 7.50 | - | - → 20.93 | 1322 → 1149 | TIMEOUT → KILLED |
| counter | 2.45 → 2.45 | - | - → 32.53 | 484 → 556 | TIMEOUT → KILLED |
| store | 3.59 → 11.60 | - | 52.15 → 47.94 | 551 → 1088 | KILLED |
| store_fail | 3.09 → 11.54 | - | 49.26 → 50.07 | 560 → 1102 | KILLED |

The other 26 kept their verdict: those that end moved by under 0.1 s;
decrement, stride and stride_fail run out of memory and stride_bound and
stride_bound_fail time out, with both. Only 9 of the 6827 test files, run
with their own flags for 20 s each, pass 4096 known bytes (6 more did not
get far enough to tell); side by side, 180 s limit:

| Test | Walk | Solve | Wall | Peak MB | Verdict |
|---|---|---|---|---|---|
| github_123 (KNOWNBUG, over 300 s) | - → 0.09 | - → 0.00 | - → 0.49 | - → 68 | TIMEOUT → SUCCESSFUL |
| overflow_09_bcnt | 0.07 → 0.04 | 0.45 → 0.00 | 0.68 → 0.19 | 57 → 51 | SUCCESSFUL |
| overflow_22_wordplay | 0.03 → 0.03 | 0.82 → 0.47 | 1.07 → 0.71 | 126 → 111 | FAILED |
| 26_exStbKey | 0.55 → 1.37 | 0.00 | 0.72 → 1.54 | 74 → 90 | SUCCESSFUL |
| struct_array_ite_new_index_fail | 0.88 → 1.42 | 15.72 → - | 16.92 → - | 770 → 193 | FAILED → TIMEOUT |

The rest (bzip2smp, pfscan, picosat, deep_binary_chain, github_302,
github_9356, struct_array_ite_new_index, github_9785_int_base_stores) did
not change. Of the 840 KNOWNBUG and FUTURE tests that give a verdict, the
837 that ended within 5 minutes flipped only github_123.

The wins come from the larger `MaxKnown` alone: github_123 and the three
no-summary tests that now end read the array at constant indices, which fold
against the known bytes instead of walking a chain of stores. The losses
come from it too, from three places that are linear in the known bytes:

- A join where one side has flushed an object and the other has not.
  `Memory::ite` makes a choice per differing byte, reading each from the
  flushed side's chain (`Terms::read`, up to 256 steps). In store_fail the
  `j == 4 ||` of the assertion joins a side that read `a[j]` with one that
  did not: 400000 such reads, 8 s of walk. In
  struct_array_ite_new_index_fail, `w = b ? w1 : w2` copies 32 KB on each
  side and the join holds 32768 byte choices where main joined arrays; the
  solve does not end in 150 s. Moving an object to the arrays on both sides
  once more than 4096 of its bytes differ brings that test back to 20 s
  (main 16 s), but leaves store_fail's reads, which happen before the count.
- `Memory::pointers`, which every bodyless call that may write other memory
  runs, looks at every known byte (26_exStbKey). Looking each provenance up
  once per object takes its walk from 1.6 s to 0.8 s, against main's 0.5 s.
- The descending order on its own: struct_array_ite_new_index_fail's solve
  goes from 16 to 17 s to 28 to 31 s and its peak from 770 to 1485 MB with
  only the order changed. The order that suits CaDiCaL 3's first guess
  is not better in general, and the guess may change with the next
  CaDiCaL.

## Conclusions

1. ESBMC's solve is preprocessing plus one lemma over one store, because
   Bitwuzla 0.9 at level 0 first tries `x = 99999`, the outermost store.
   Vereda's is preprocessing plus one lemma over all 100000 stores, because
   Bitwuzla 0.11 (most likely CaDiCaL 3's lucky phases, which now run under
   assumptions) first tries `x = 0`, the innermost. Vereda's formula on
   ESBMC's solver, posed as ESBMC poses it, takes 0.83 s; ESBMC's formula on
   Vereda's solver takes 14.3 s. The gap is the solver version, not Vereda's
   formula.
2. The 62-bit index costs about 0.7 s of bit-blasting against a 17-bit one;
   narrowing an object's own arrays to its size would take this solve from
   5.0 to 4.2 s, at the price of an index that wraps for an access out of
   bounds.
3. Putting the low addresses outermost takes the solve to about 1 s, but
   needs the whole array in one flush, so `MaxKnown` raised; that brings
   large wins (github_123, three no-summary tests from timeouts to seconds)
   and losses (the walks of store and store_fail from about 3.5 to 11.6 s,
   struct_array_ite_new_index_fail from 17 s to a timeout) whose fixes are
   changes to joins and to the pointer scan, not to the formula.
4. Nothing changed. In order: a larger `MaxKnown` as its own piece of work,
   once a join against a flushed side no longer reads every byte; then the
   descending flush, measured over the whole suite; the narrower index if
   the few tenths of a second matter. Loop summaries (#36), on by default,
   already remove this chain (0.15 s).
