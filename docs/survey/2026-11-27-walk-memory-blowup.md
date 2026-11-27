# Walk memory blowup at small bounds

Recorded 2026-11-27.

Issue #381 (Cause C) flagged small-bound walk memory blowups on the `ddv-machzwd` family (`ddv_machzwd_inb`, `ddv_machzwd_inl`, `ddv_machzwd_inb_p` at `--unwind 11`, hitting >4.2 GB OOM) and `loops/n.c24` (1,017 MB / 10.4 s at `--unwind 11`).

## Root causes

Profiling `ddv_machzwd_inb --unwind 11` under `prlimit --as=1200000000` with `gdb` exposed four compounding blowups during the walk:

1. **Unbounded `Terms::implied` cache and traversal in `Terms::entails`.** `Terms::implied` grew past 16M buckets (>400 MB per table, >3 GB across rehashes) when `Memory::fromAddress` and `Memory::side` queried entailment on large conjunction/disjunction guards.
2. **Unbounded `Terms::concat` ITE distribution.** `Terms::concat(ite(c, ha, hb), ite(c, la, lb))` allowed recursion up to `joinDepth < 1000` with no node budget, producing exponential term growth when joining deep ITE trees.
3. **Unsplit-blind `Memory::fromAddress` and missing direct-base fast path.** Every symbolic pointer load from a havoc'd object called `fromAddress(a)`, which split `a` via `sumChoice` up to 32 times even when no branch could fold to null or a known object base. Each symbolic leaf then scanned all exposed objects (~160–336 in `ddv_machzwd`), duplicating the exposed-object ITE chain up to 33 times per load.
4. **Quadratic state merges in `Memory::havocWith`.** For whole-object havoc (`!n`, as called by `havocCall`), `havocWith` recursively split pointer ITE trees with `t.split(p, c, x, y)` and merged full `State`s on the way back up via `Memory::mergeSeparate`, creating $O(N^2)$ `ite` terms over `st.separate` for an $N$-object pointer ITE.

Profiling `loops/n.c24` (a `char x[1000]` array initialised by a summarised loop and shifted by conditional `memmove(&x[i], &x[i + 1], 1000 - i)` inside a 3-deep loop) exposed three further issues:

5. **Byte-by-byte fallback below `MinSeparate` (1024).** `x[1000]` was already backed by its own array via loop summarisation, but `copyArray` rejected `n < 1024`, falling back to 1,000 byte stores per `memmove` (~1M `Store` nodes at `--unwind 11`).
6. **Cubic `Terms::under` flattening for nested conditional lambdas.** Every `Store`/`Ite` over array lambdas copied and deduplicated the full transitive list of reachable lambdas into `under[r]`, and `Terms::uncached` copied `std::vector<Ref>(u->second)` on every `Select`, turning $M$ conditional `copyArray` lambdas into $O(M^3)$ work.
7. **Redundant `copy_provenance#` lambdas on integer arrays.** `Memory::summarise` wrapped `below.prov` in a `summary_provenance#` lambda even when `below.prov` was already `constArray(IntProvenance)`, and `Terms::constArray` did not memoise by `(indexSort, value)`. As a result, `ite(c, constArray(IntProvenance), constArray(IntProvenance))` failed to fold, making `t.filledWith(arr) == intProv` return false and doubling every `copyArray` with a `copy_provenance#` lambda.

## Changes

- `src/encode/terms.{h,cpp}`:
  - Bound `Terms::implied` to `MaxImplied = 1 << 16` entries and give `Terms::entails` a per-call budget of 512 nodes (skipping negative caching when truncated).
  - Cap `Terms::concat` ITE joining to `joinDepth < 64` and a per-call `joinBudget = 64`.
  - Replace the flattened `under` vector with direct 1–2 child links (`DenseMap<const void *, SmallVector<Ref, 2>>`) plus a `constrained` set of `(array, index)` pairs so each reachable lambda is constrained at most once per index without $O(M^2)$ table construction or $O(M^3)$ copies.
  - Memoise `Terms::constArray` by `(indexSort, value)` so identical constant arrays share a single `Ref` and fold through `ite`.
- `src/memory/memory.{h,cpp}`:
  - Index allocated object bases in `baseToObj` and check `directBase(a)` first in `Memory::fromAddress`. Only split `sumChoice` when `hasFoldableAddress` finds a null or exposed base leaf within budget, resolve taken branches with `side(c)`, and memoise within each `fromAddress` call.
  - In `Memory::havocWith`, skip `t.split` when `!n` (whole-object havoc) and iterate `candidates(p)` directly so each candidate updates only its own object in `st`, avoiding quadratic `mergeSeparate` merges.
  - Lower `Memory::copyArray`'s threshold to `MinCopyLambda = 64` for objects already in `owned` or `st.separate`.
  - In `Memory::summarise`, keep `below.prov` directly when `t.filledWith(below.prov) == intProv`.

## Measurements

All runs use `-m32 --no-unwinding-assertions --property unreach-call` on SV-COMP benchmarks (`db722441` before vs this branch after):

| Benchmark | Unwind | Before (walk / peak RSS) | After (walk / peak RSS) | Verdict |
| --- | ---: | --- | --- | --- |
| `ddv-machzwd/ddv_machzwd_inb.i` | 11 | OOM (>4.2 GB, >40 s) | 12.73 s / 788 MB | SUCCESSFUL |
| `ddv-machzwd/ddv_machzwd_inl.i` | 11 | OOM (>4.2 GB) | 11.45 s / 737 MB | SUCCESSFUL |
| `ddv-machzwd/ddv_machzwd_inb_p.i` | 11 | OOM (>4.2 GB) | 11.60 s / 736 MB | SUCCESSFUL |
| `loops/n.c24.i` | 9 | 5.68 s / 653 MB | 1.28 s / 275 MB | SUCCESSFUL |
| `loops/n.c24.i` | 11 | 10.40 s / 1,017 MB | 4.72 s / 791 MB | SUCCESSFUL |

## Wide performance check

Side-by-side comparison against `main` (`672fa26f`) to verify the new folding bounds (`entails` 512-node budget / 64K cache cap, `concat` ITE join depth/budget 64, `copyArray` threshold 64 for separated/owned arrays) did not regress other tasks:

- `tests/bench.py` over `vereda` + `esbmc-cstd` (845 tests): 0 regressions.
- `tests/bench.py` over `esbmc` (3,598 tests): 0 tests changed, 0 regressions.
- 75 SV-COMP tasks across 8 categories (`loops`, `loop-lit`, `array-examples`, `array-memsafety`, `memsafety`, `heap-data`, `bitvector`, `list-properties`, `-m32 --unwind 12 --no-unwinding-assertions --property unreach-call`): 0 verdict changes, 0 time regressions (>0.25 s and >25%), 0 memory regressions (>15 MB and >20%).

## Remaining work

At higher unwind bounds, `ddv-machzwd` still grows with the number of `havoc_provenance#` ITEs built across ~200+ exposed objects in `Memory::havocCall`, and `loops/n.c24` still creates $O(M^2)$ `copy#` lambda instantiations across $M$ conditional overlapping `memmove` calls.

Follow-up issues: #381, #401
