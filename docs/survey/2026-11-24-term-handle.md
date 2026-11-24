# A term type of Terms' own

Recorded 2026-11-24.

The first version of #197 (`2026-11-24-lazy-constants.md`) kept a
bit-vector constant out of the solver until it was needed by making it a
subclass of camada's `SMTExpr`. That made the walk 30% shorter on
array_loop_summary_decrement_fail without summaries, but the compiler could
not tell such a constant from a solver term: its safety rested on calling
`solverTerm` before each direct camada call outside terms.cpp, and a
constant that leaked reported kind `BVConst`, so camada's own encodings
could take it for a backend constant and build a wrong formula far from the
missed call. Moving the deferral into camada was declined
(mikhailramalho/camada#360). This note records the redesign that #197's
last comment planned.

## The design

`Terms` hands out `Term`, which does not convert to `camada::SMTExprRef`. A
`Term` is one pointer: the solver's handle or, with the low bit set, the
constant's entry. A constant's entry holds its `APInt` and, once one is
made, the solver's term for it. Entries live in a `std::deque`, which never
moves them, and `bvs` maps each value to its entry, so one value has one
entry. The solver term is cached in the entry, never in a `Term`, so copies
of a constant stay equal, and a term's identity is the pointer itself,
which `Term::id()` gives. The maps that keyed terms on `const
camada::SMTExpr *` (`nodes`, `memo`'s keys, `implied`, `arith`, `filled`,
`lambdas`, `under`, `reduced`, `selects`, `freeOf`, the encoder's
`floatBits` and facts, memory's `provBelow`, `fromInteger`, the provenance
walk's sets and `targets`, the model's values) key them on `id()`; no
`DenseMapInfo<Term>` was needed. A null `Term` has id null, as the old code
spelled `x ? x.get() : nullptr`.

`solverTerm(Term)` is the only way from a `Term` into camada; for a
constant it makes the solver's term the first time, as `bv()` did before
#197, and records it in `made`. `fromSolver(SMTExprRef)` is the way back: a
term camada hands over that is a constant's solver term becomes the
constant again, so folding still sees it (`mkIEEEFPToBV` gives back the
bits a native float was made from). Inside Terms, `build` takes the solver
terms of its operands and wraps the result directly, as #197 did. `Term`
answers `width()`, `isBool()` and `isBV()` itself; a constant is never a
Bool, as Bools stay solver terms (`boolean()`), and `isTrue`/`isFalse`
compare ids.

The handle goes in and out of a `Term` by `std::bit_cast`, which needs it
to be one pointer: true while camada's handles are unchecked
(`CAMADA_CHECKED_HANDLES 0`, as in the release archive Vereda downloads),
and checked by a `static_assert`. A camada built with checked handles (24
bytes) would need the two-pointer form below back.

Gone: the `SMTExpr` subclass, `makeExprRef`, the `camadaimpl.h` include
with its `NDEBUG` workaround, and the backend kind `0xff`.

Layering is unchanged: `Term` is defined in encode. checks.h, trace.h and
cellmap.h include `encode/terms.h` in place of `camada/camada.h`; the
witness stage no longer names a camada type, and the walker still uses
camada for sorts and its solver queries, as on main.

## What the compiler found

With the stages' `SMTExprRef` replaced by `Term`, a build outside terms.cpp
stopped at 178 errors on 102 lines in 9 files. They became 27 calls of
`solverTerm` (the walker's solver queries and constraints, memory's three
layout constraints, the checks `decide` asks about, the dumped formula, the
model's values and the encoder's floating-point operands), 26 of
`fromSolver` (the results of camada's floating-point operations, all in the
encoder), 33 sort queries (`->getWidth()`, `->isBoolSort()`) that became
`width()` and `isBool()`, and 22 identity uses of `.get()`. In the encoder,
`fp()` and `rm()` now return camada terms, as they only feed camada's
floating-point operations, and `bits()` takes the operation's result back.
`solver::decide` takes the `Terms` instead of the solver, and the `Model`
holds them non-const, as it hands a term to camada through `solverTerm`.

## Two pointers, then one

The plan was to start with a `Term` of two pointers, the entry or solver
term as identity plus the handle, 16 bytes, and to pack it into one only if
measurements asked for it. Release builds with the installed LLVM 21 and
the release camada, all with mimalloc; three runs of each, alternated, on
array_loop_summary_decrement_fail with `--no-summarize-array-loops`, on an
80-core machine under a load of 16 to 30. The peaks are those the binary
prints after the walk and after the solve. On main 7e3112fe, with #197 as
it was (a65f2924) merged with it:

| build | walk s | walk peak MB | solve s | run peak MB |
|---|---|---|---|---|
| main | 2.25, 2.21, 2.45 | 492 | 3.96, 3.93, 4.05 | 937 to 941 |
| #197, subclass | 1.57, 1.54, 1.66 | 437 to 438 | 3.50, 3.45, 3.53 | 880 to 885 |
| Term, two pointers | 1.51, 1.49, 1.53 | 422 | 3.44, 3.41, 3.46 | 852 to 856 |

Then #203 landed, which keeps up to 1 MiB of known bytes instead of 4,096.
On main bc52ab20, with #197 merged with it:

| build | walk s | walk peak MB | solve s | run peak MB |
|---|---|---|---|---|
| main | 2.35, 2.69, 2.28 | 560 to 561 | 3.91, 4.05, 3.77 | 937 to 941 |
| #197, subclass | 1.90, 1.85, 1.75 | 481 | 3.57, 3.56, 3.45 | 878 to 879 |
| Term, two pointers | 1.66, 1.67, 1.67 | 478 to 479 | 3.45, 3.50, 3.40 | 955 |
| Term, one pointer | 1.66, 1.60, 1.55 | 434 to 435 | 3.60, 3.48, 3.40 | 834 to 835 |

With two pointers the walk was as fast and its peak as low as #197's, but
the run peaked 76 MB higher, during the solve: sampled every 10 ms, RSS is
about 480 MB as the walk ends with both, and grows by 480 MB during the
solve with two pointers against 398 MB with #197, on the same formula. Two
pointers make a known byte's `Cell` 32 bytes instead of 16, so a node of
the known bytes' map lands in another of mimalloc's size classes, and the
solver no longer reuses what the walk's freed maps leave: #197 with its
`Cell` padded to 32 bytes peaked at 974 MB. One pointer brings the `Cell`
back to 16 bytes and a term's `Node` from 64 to 40, and the run peaks 44 MB
below #197's and about 105 MB below main's. A constant is smaller than in
#197 either way: there it was an `SMTExpr` in the solver's arena (vtable,
sort handle, kind, `APInt` and handle, 48 bytes) beside its 24-byte `bvs`
bucket, and the arena zero-filled its blocks; here it is a 24-byte deque
entry and the same bucket.

On main d9bfaaf7 (#204 merged), with #197 merged with it, the same way:

| build | walk s | walk peak MB | solve s | run peak MB | wall s |
|---|---|---|---|---|---|
| main | 2.47, 2.44, 2.46 | 560 to 561 | 4.03, 4.03, 4.03 | 935 to 941 | 7.24, 7.23, 7.26 |
| #197, subclass | 1.81, 1.75, 1.80 | 481 | 3.54, 3.51, 3.70 | 878 to 882 | 5.97, 5.89, 6.30 |
| Term, one pointer | 1.64, 1.66, 1.65 | 435 | 3.52, 3.51, 3.52 | 834 to 835 | 5.78, 5.79, 5.80 |

Against main, the walk takes 33% less time and 126 MB less memory, and the
run 20% less time and 103 MB less memory; against #197, the walk takes 8%
less time and both peaks are 46 MB lower.

## Exactness

Every single-file test under `tests/` (6,613) ran with main (d9bfaaf7) and
the branch with one pointer merged with it, with `--dump-smt-too
--dump-dir` added (or the test's own dump flag) and 60 s each, by #197's
scripts (cmpdump.py, canon.py, sexpr_eq.py). 379 were skipped, as they need
extra files, their own directory or an `__ESBMC_`/`__CPROVER_` builtin, and
5 took longer (below). Of the other 6,229, 6,048 gave the same output and
dumps byte for byte once the binary's path is left out, and 10 more
differed only in the witness's UUID. 24 have the same dumps byte for byte,
but the counterexample shows other values of the same formula. 147 dumps
differ: canon.py finds 118 of them the same up to the order of lines or
commands and the names of shared terms, and sexpr_eq.py, which also sorts
the operands of commutative operations and compares the assertions as
multisets, finds the other 29 the same too. No verdict changed. These are
about the counts #197 reported against its main (146 dumps, 23
counterexamples), on 32 more tests. Against main bc52ab20, before #204, the
counts were the same but for 4 tests #204 added.

The same sweep with #197 itself (a65f2924 merged with the same main) as the
base: every dump of the 6,230 tests that finished is the same byte for
byte, and every output too once the binary's path and the witness's UUID
are left out. The retype changes no term, no formula and no counterexample.

The two-pointer form, swept the same way against main 7e3112fe and against
#197 merged with it, gave the same counts (146 dumps, all equal up to order
and names; 24 counterexamples), and against #197 every dump and output was
the same byte for byte once the binary's path and the witness's UUID are
left out.

With 900 s, against main bc52ab20: scanf-false-3 and
struct_array_ite_new_index give the same output and dumps, github_302 the
same verdict and dumps up to the order of lines (its counterexample shows
other values, as with #197), and github_9356 and github_9356_success finish
with neither binary.

A Debug build (assertions on, against the release camada, which is built
with `NDEBUG`) passes the vereda suite and the z3, cvc5, dump, float and fp
tests (728), now without the `NDEBUG` workaround #197 needed for
`makeExprRef`.

## Benchmarks

`tests/bench.py --list` on #197's list (12_lms_new, 13_ludcmp_new,
github_302, github_1289 and every array_loop_summary test, each also as a
copy with `--no-summarize-array-loops` added, then github_123, github_9356,
github_9356_success and struct_array_ite_new_index with its _fail), 180 s
each, the branch with one pointer (1cc4443a) against main bc52ab20 and
against #197 merged with it, each pair side by side. The rows that changed
or ran long against main (walk, solve and wall seconds, peak MB, verdict):

| test | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| array_loop_summary, no summaries | 5.46 → 3.09 | 0.00 | 5.62 → 3.26 | 900 → 711 | SUCCESSFUL |
| array_loop_summary_fail, no summaries | 4.96 → 2.67 | 0.00 | 5.91 → 3.42 | 901 → 711 | FAILED |
| array_loop_summary_nondet, no summaries | 10.07 → 6.16 | 0.00 | 10.23 → 6.32 | 1374 → 1077 | SUCCESSFUL |
| array_loop_summary_nondet_fail, no summaries | 7.38 → 5.58 | - | - | 1218 → 977 | KILLED |
| array_loop_summary_counter, no summaries | 2.81 → 1.85 | - | - | 586 → 456 | TIMEOUT |
| array_loop_summary_counter_fail, no summaries | 3.22 → 2.27 | 1.36 → 0.69 | 5.75 → 3.87 | 614 → 506 | FAILED |
| array_loop_summary_decrement, no summaries | 3.53 → 1.95 | - | - | 574 → 452 | KILLED |
| array_loop_summary_decrement_fail, no summaries | 2.55 → 1.74 | 4.13 → 3.60 | 7.60 → 6.13 | 974 → 872 | FAILED |
| array_loop_summary_store, no summaries | 3.59 → 3.15 | - | - | 659 → 605 | KILLED |
| array_loop_summary_store_fail, no summaries | 3.73 → 3.00 | - | - | 655 → 628 | KILLED |
| array_loop_summary_stride, no summaries | 0.97 → 0.64 | - | - | 254 → 202 | KILLED |
| array_loop_summary_stride_fail, no summaries | 0.99 → 0.65 | - | - | 261 → 206 | KILLED |
| array_loop_summary_vla, no summaries | 0.61 → 0.63 | 0.78 → 0.89 | 1.53 → 1.66 | 85 → 86 | SUCCESSFUL |
| github_302 | 50.00 → 47.13 | 20.83 → 9.81 | 71.01 → 57.11 | 752 → 558 | FAILED |
| github_302, no summaries | 44.48 → 42.39 | 19.06 → 8.03 | 63.72 → 50.58 | 743 → 567 | FAILED |
| struct_array_ite_new_index | 0.81 → 0.58 | - | - | 170 → 145 | TIMEOUT |
| struct_array_ite_new_index_fail | 0.72 → 0.54 | 15.00 → 14.74 | 16.01 → 15.58 | 765 → 756 | FAILED |

KILLED is the 4096 MB limit on address space, which both binaries reach.
The other rows take about a second or less with both. No verdict changed.
github_9356, github_9356_success and the two stride_bound copies without
summaries time out with both; bench.py lists as regressed only the FUTURE
tests (k_induction, uninit, verifier_assert, witness and their copies) that
end with the same error with both binaries, as in #197's note.

Against #197 the walks are as long or shorter (array_loop_summary without
summaries 3.05 → 2.85 s, the decrement test 2.53 → 1.92 s) and the peaks 5
to 77 MB lower (array_loop_summary_nondet without summaries 1155 → 1078
MB), with github_302 the same (walk 46.2 → 46.8 s, solve 9.57 → 9.53 s). No
verdict changed, and bench.py lists the same FUTURE tests.
