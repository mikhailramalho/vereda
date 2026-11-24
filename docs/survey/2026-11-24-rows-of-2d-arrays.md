# Summaries for the rows of 2-D arrays

Recorded 2026-11-24. Issue #177.

`array-multidimensional/copy-2-u` and `transpose-u` were the only two tasks
the ReachSafety rerun on a453547a lost to the kernel's OOM killer (6 GB,
60 s); ESBMC times out on both. Each fills a 1000 by 1000 `int` array in a
loop nest, copies it (row by row, or into columns), and checks every
element in a third nest.

## What grows

With `--incremental-bmc` and the skip of #194 the bounds go 1, 3, 7, ...,
511 in a second; every walk ends in the first nest's inner loop. Bound
1001 unrolls the three nests whole, three million iterations, and the
walk reaches 6 GB at about 36 s. The same program at a smaller size, with
`--unwind n+1`:

| n | iterations | walk | peak memory | solve |
|---|---|---|---|---|
| 100 | 30,000 | 2.5 s | 271 MB | 37 s |
| 200 | 120,000 | 10.5 s | 950 MB | over 290 s |

About 7.5 KB an iteration, so 22 GB at n = 1000. A heap profile at
n = 100 (a build with glibc's malloc, under heaptrack; mimalloc hides
allocations from it) shows no one structure to cut: of 229 MB, the trace
vector holds 40 MB, Bitwuzla's nodes about 60 MB, Terms' maps about 35 MB,
camada's arena 17 MB, the rest smaller. Even the walk at a quarter of
that would not leave the solver a formula it answers: n = 200 already
takes it more than 290 s.

The loops are not summarised. The inner loop of each nest stores to
`{{a,+,4000}<outer>,+,4}<inner>`, whose start is an AddRec of the outer
loop, for which `evaluate` had no term (#170 counted 19 such loops in
ReachSafety and 12 in the suite).

## The change

The walker now knows the iteration of each loop it is unrolling, and an
AddRec of an enclosing loop evaluates at it: the inner loops are
summarised once per outer iteration, one row each.

That alone moved the blow-up. Each row's summary is a lambda over the
array before it, so a read at the checks' nondet index went through a
chain of 1000 lambdas, each building its terms; reading a copied row
substituted the whole chain of the row it was copied from. At n = 20 the
walk took 24 s (it grows as n^4), at n = 1000 it ran out of memory at 37 s
as before. Three things make a read reach only its row:

- `Terms::range` gives the values a term takes as a strided interval,
  lo + stride * i up to hi, through constants, add, sub, mul, logical
  shifts right, extensions, extract and concat. A summary's index k gets
  the bound k < trips once the body's guard, which holds that comparison,
  is built: every term with k counts only under that guard, or, once the
  summary puts an element's index n for k, only where n < trips.
  Registering it before the guard folded the guard's own k < trips away
  and gave FALSE on a program that cannot fail.
- A summary's lambda says which indices it misses: offsets all below its
  first element or past its last, or all the same distance from an element
  when their stride is a multiple of the loop's (a column of a transpose
  read at a nondet row). A read there goes to the array below without a
  term, hopping over every row it misses.
- Folds that make the copied element the one it was copied from:
  `y[w-1:k] ++ 0^k` is `y` when y's low k bits are zero (an offset made an
  element index and back), `x * c / c` is `x` and `x * c % c` is 0 when
  x * c does not wrap, and a comparison whose sides' ranges settle it.

One more: the arrays are VLAs, which clang indexes as `getelementptr i8,
ptr %a, i32 (i * 4000)` and then by `j`. The `i8` scale kept them on byte
arrays (#107 wants every offset a multiple of the element); the index's
known trailing zeros, from LLVM's `computeKnownBits`, now count, and the
arrays hold `i32` elements. Without that, the 300 by 300 copy took 15 s
and the transpose more than 30 s; with it, 0.2 s.

Each fold was taken out in turn on 300 by 300 copies of both tasks; the
ones that left them as fast (an extract above a multiplication by a power
of two, a shift of a concat by its low width, an ite under its own
condition, the range of a mask) are not in the change.

## array-multidimensional

The harness of the rerun (`vereda.py --incremental-bmc --property-file
unreach-call.prp`, 60 s and 6 GB in a systemd scope), two at a time,
main at ac2604b1 against this change. Every task of the set expects TRUE.

| task | main | this change |
|---|---|---|
| copy-2-u | out of memory at 35 s | TRUE, 2.7 s, 119 MB |
| transpose-u | out of memory at 37 s | TRUE, 2.0 s, 91 MB |
| copy-partial-2-n-u | out of memory at 35 s | TRUE, 1.3 s, 91 MB |
| init-2-n-u | out of memory at 36 s | TRUE, 1.4 s, 102 MB |
| rev-2-n-u | out of memory at 36 s | TRUE, 3.3 s, 142 MB |
| max-2-u, min-2-u | out of memory at 36 and 37 s | timeout |
| add-2-n-u, diff-2-n-u | out of memory at 36 and 35 s | out of memory at 43 and 39 s |
| init-non-constant-2-n-u | timeout | timeout |
| the 10 tasks of 3 or 4 dimensions | TRUE, 0.2 s | TRUE, 0.2 s |

Peak memory is the one `/usr/bin/time` gives, the larger of the front
end's (about 90 MB) and the binary's. The two still out of memory store to
two arrays in one inner loop, which a summary does not take; `max` and
`min` keep a running maximum, whose header phi is no AddRec, and now time
out at bound 1001 instead of running out of memory. The tasks of 3 and 4
dimensions are answered at bound 1 either way.

A checking loop of at most 256 iterations is unrolled, not summarised, so
its reads are at constant indices and hop over every row but one. At n
from 100 to 250 the copy now takes 1.0 to 11 s and 139 to 601 MB, where
main took 40 s at 100 and more than 300 s at 200.

## ReachSafety

To find what else changes, every one of the 1,407 unreach-call tasks of
Arrays, ControlFlow, Heap and Loops was walked at `--unwind 1` with both
binaries, counting the loops summarised, and once more with a build that
reports an object given elements only by the known trailing zeros. Twelve
tasks summarise a loop more (435 loops in 211 tasks before, 447 in 223
now): `loop-invgen/nested6` and 11 of array-multidimensional. The known
zeros give elements in 24: 14 of array-multidimensional, 7 of
`array-tiling` (`mbpr2` to `mbpr4`, `pr2` to `pr5`) and 3 of `array-cav19`
(`array_doub_access_init_const`, `array_tiling_tcpy`,
`array_tripl_access_init_const`). The 31 tasks, as the table above was
run:

| tasks | main | this change |
|---|---|---|
| array-multidimensional (20) | 10 TRUE, 9 out of memory, 1 timeout | 15 TRUE, 2 out of memory, 3 timeouts |
| array-tiling (7) | timeout at bounds 15 to 39 | timeout at bounds 21 to 63 |
| array-cav19 (3) | timeout | timeout (`array_tiling_tcpy` at bound 29, was 25) |
| loop-invgen/nested6 | timeout at bound 23 | timeout at bound 21 |

No verdict is lost and none is wrong. `ctest -L vereda` and the 281 tests
`ctest -R 'esbmc.*(array|summary)'` selects pass.

## Not done

Two stores in one loop (`add-2-n-u`, `diff-2-n-u`) and running reductions
(`max-2-u`) are the shapes #36 lists next. Summarising a whole 2-D nest at
two nondet indices would make each nest one check instead of one per row,
but the rows here already fold to no check at all.
