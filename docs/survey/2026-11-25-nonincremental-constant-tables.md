# Non-incremental solving of the decrement table

Investigated on main at 5eaab3df, after #247, with LLVM 23.1.6,
camada 0.25 and Bitwuzla 0.11.0. The reported timings concern the failing
assertion `a[x] == 7 - 2*x`, not the passing `(x + 1)` variant. Both were
measured. The original loop still runs all 100000 iterations with
`--no-summarize-array-loops` throughout this investigation.

## What regressed

The walk produces a fully written constant array. The existing flush
represents it as a constant array of its most frequent value plus 99999
stores of the other elements. There is one symbolic read, at a 62-bit
element index, and the failed assertion is satisfiable for every in-range
index. Its SMT dump is 10800886 bytes.

On this same formula, a small driver linked to camada's Bitwuzla library
measured these query modes. Each produces models, as Vereda does. Times
exclude parsing; runs were made in batches on a machine doing other work.

| Query | Solve | Peak RSS |
|---|---|---|
| asserted property, `check-sat`, defaults | 13.28 s | 1601400 KiB |
| asserted property, permanent CNF scope | 13.15 s | 1601268 KiB |
| property under an assumption, permanent CNF scope | 4.84 s | 568408 KiB |
| asserted property, no skeleton preprocessing | 13.98 s | 1600012 KiB |
| asserted property, no variable substitution | 13.26 s | 1598544 KiB |
| asserted property, no normalization | 12.32 s | 1583124 KiB |
| asserted property, preprocessing disabled | timeout at 40 s | |
| asserted property, AND flattening disabled | timeout at 40 s | |

The non-incremental query takes nine array lemmas, eight ACCESS_STORE and
one ACCESS_CONST_ARRAY, and traverses 999866 stores. Its bit-blast has
1100226 AIG ANDs and 3300288 CNF clauses. Under an assumption it takes two
lemmas, one of each, traverses 100000 stores, and builds 300314 ANDs and
900525 clauses. The store-chain search repeats at different proposed
indices. This agrees with the older investigation in
`2026-11-23-what-the-decrement-test-s-solve-depends-on.md`; the option changes
above do not recover the incremental query's behavior. Reordering stores
would favor particular proposed indices, rather than remove that dependence.

ESBMC 8.7.0 at `~/tools/esbmc-ctype/build/src/esbmc/esbmc`, using its
Bitwuzla 0.9.0 and a temporary source adapted to `__ESBMC_assume` and
`__ESBMC_assert(..., "")`, reports 6.16 s symex, 1.73 s encoding and 1.71 s
decision-procedure time on the failing version. The process was stopped as
soon as that last timing appeared, before its broken counterexample phase.
The passing version did not finish solving within 60 seconds.

## Exact representation of the already-written table

`Memory::flushObject` already replaces fully known tables with their
dominant value or compact constant ranges. Extend that representation to
constant arithmetic progressions. After the existing check that every byte
was written, compare every element with the expected first value plus a
fixed difference, using APInt arithmetic modulo the element width. Require
at least eight elements, a nonzero difference, an owned array indexed by
offset, and elements of at most eight bytes. A mismatch or symbolic element
uses the existing representation. Unwritten bytes never enter this path.

For a matching table, use the existing lambda mechanism to return
`first + difference * index` inside the finite table and the original
dominant value outside it. Resize the index to the element width before
the modular multiplication. This preserves every element and the original
constant-array default; the provenance representation is unchanged. Later
writes remain ordinary overrides of that array.

This does not infer an effect from the loop: all instructions, checks and
trace steps are still walked, including when loop summaries are disabled.
It compresses the resulting constant data, independently of how the program
wrote it. It also leaves solver selection, non-incremental queries and the
preprocessing settings from #247 unchanged.

The decrement read reduces to arithmetic, so its new dump is 1175 bytes,
QF_BV with no stores or selects. In the initial native runs, the failing
case changes from 13.29 s / 1656 MB non-incrementally and 4.89 s / 651 MB
incrementally to 0.054 s / 158 MB non-incrementally. The passing case now
proves in 0.049 s / 159 MB; the baseline was stopped after exceeding a
minute. The walk drops from about 1.3 s / 248 MB to 0.9 s / 159 MB.

Three further failing-case runs, rotating the order of main non-incremental,
main incremental and branch non-incremental, confirm the result:

| Mode | Solve times | Median | Solve peak MB |
|---|---|---|---|
| Main non-incremental | 12.837, 12.972, 13.047 s | 12.972 s | 1655–1656 |
| Main incremental | 4.557, 4.630, 4.811 s | 4.630 s | 650–652 |
| Branch non-incremental | 0.054, 0.054, 0.073 s | 0.054 s | 158–159 |

## Preserving the non-incremental arithmetic gains

Six `nla-digbench-scaling` tasks from #247 have byte-identical dumps on
main and this branch at bound 7. Running them under incremental BMC with
the default non-incremental solver retains every verdict and gives these
cumulative solve times:

| Task | Main | Branch |
|---|---|---|
| `prodbin-ll_valuebound50` | 4.79 s | 4.72 s |
| `prodbin-ll_valuebound100` | 13.99 s | 14.19 s |
| `lcm1_valuebound5` | 6.14 s | 5.97 s |
| `egcd3-ll_unwindbound10` | 0.31 s | 0.32 s |
| `hard-ll_valuebound100` | 6.54 s | 6.54 s |
| `prod4br-ll_valuebound20` | 13.09 s | 12.57 s |

## Regression coverage

The full 7013-test suite completes with 2836 passes, 4177 expected skips
and no failures. Five new regressions cover byte wraparound, 64-bit
decrement and later writes, a mismatch in the last element, a symbolic
element, and an incomplete table whose last value must remain unknown.
The suite statistics comparison reports no performance regression.
The paired runs record 6630 statistics entries. Their only verdict change
is the new byte-wrap regression, from a 300-second timeout on the baseline
to SUCCESSFUL here; `bench.py --stats` reruns it in 0.12 s and reports no
regressions. Existing tests retain their verdicts.
