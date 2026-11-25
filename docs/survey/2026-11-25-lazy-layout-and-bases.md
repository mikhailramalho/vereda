# Lazy `bases`, on-demand layout constraints, and non-capturing attribute inference

Recorded 2026-11-25.

On `esbmc-unix2/23_picosat-846_01` (583 global objects, 325 of them local
`@.str*` literals), Vereda spent 9.92 s before `Unwinding loop 0 iteration 1`
(without `--unwind`), 7.81 s on `--unwind 1` (7.7 s solving check 0, peak
368 MB), and 8.12 s on `--unwind 2` (peak 361 MB). Dumping the SMT formula for
`--unwind 1` showed two root causes:

1. **1,167 eager global assertions before `main` starts**: `Memory::Memory()`
   created the `bases` array and `Memory::allocateGlobals()` emitted 583
   chained 64-bit `base#1 < ... < base#583` layout constraints plus 584
   `(= (select bases id) base#id)` assertions into the global solver context,
   even when the property checked referenced none of them.
2. **Over-approximated exposure of local string literals**: `Walk::exposed()`
   relies on LLVM's `PointerMayBeCaptured`. Because `__assert_fail` (233 call
   sites in `picosat`) lacked `captures(none)` and defined functions / linked
   models (`strcmp`, `fopen`, `sprintf`, `picosat_message`, etc.) did not have
   `function-attrs` run on them, ~320 `@.str*` literals were marked
   `exposed = true`. Converting `argv` via `fromAddress()` then built a
   320-branch ITE, laid out all 320 globals, and caused `Memory::prepare()` in
   `strcmp(argv[i], "-v")` to flush 305 globals (~5,991 stores) into `st.data`
   and `st.prov`. The non-constant `st.prov` also prevented `isInt` from
   folding in `Memory::load()` for `argv[i] != NULL`, triggering `base(obj)` on
   a symbolic provenance and materialising `bases` even though `nullEq`
   bypassed the result.

## Design

Three strictly correctness-preserving changes were implemented:

1. **Lazy `bases` SMT array** (`src/memory/memory.h`, `src/memory/memory.cpp`):
   Mirror `Memory::sizes`. Do not create `bases` in `Memory::Memory()` or
   assert `bases[id] = base#id` in `Memory::allocate()`. When `Memory::base()`
   encounters a non-constant object id for the first time, lay out all exposed
   objects (`ensureLayout(id)`), create `bases`, and assert
   `bases[id] = objects[id].base` for `id == 0` and every `objects[id].exposed`
   object. Only exposed objects can ever have their pointer stored in memory or
   appear in a symbolic `base(obj)` (matching `Memory::candidates()`); direct
   constant `base(id)` calls still work for any object via `ensureLayout(id)`.
2. **On-demand `base#id` layout constraints** (`src/memory/memory.h`,
   `src/memory/memory.cpp`): Record `align` and `bool laidOut` in
   `Memory::Object`. For constant-size allocations within the `2^(ptrBits - 2)`
   `endBounded` budget (when `!bases || !exposed`), defer asserting the
   alignment/non-overlap constraint and advancing `end` until
   `ensureLayout(id)` is called from `Memory::base()` or `Memory::fromAddress()`.
   Because all deferred objects together fit within a quarter of the address
   space, laying out any subset in any order remains satisfiable and
   non-overlapping. Symbolic-size and over-budget allocations are still laid
   out immediately with guarded constraints. Also short-circuit `Memory::equal()`
   to `t.eq(p, q)` when `outside` folds to `false`, avoiding unnecessary
   `address()` / `ensureLayout()` calls.
3. **Non-capturing attribute inference in `ir`** (`src/ir/load.cpp`): After
   `linkModels()`, mark pointer parameters of an external `__assert_fail`
   declaration as `captures(none)` (Vereda intercepts `__assert_fail` as an
   assertion check and never leaks its arguments), run
   `cgscc(function-attrs)` so defined functions and linked models (`strcmp`,
   `fopen`, `sprintf`, `picosat_message`, etc.) get `captures(none)` /
   `captures(address_is_null)` inferred, and re-run `stripUBAssumptions()` so
   no UB-based attributes (`noundef`, `nonnull`, `mustprogress`, etc.) remain.

## What was not adopted

- **Dynamic exposure for local-linkage globals (`internal`/`private`)**:
  Deferring `exposed = true` until `Memory::global(g)` is walked would miss the
  case where an integer is converted to a pointer before `&g` is evaluated
  (`int *p = (int *)a; if (p == &g) { *p = 20; assert(g == 10); }`, covered by
  `tests/vereda/mem_lazy_layout_fromaddress_fail.c`). Accurate static
  `captures(none)` inference avoids this unsoundness while cutting exposed
  globals in `23_picosat-846_01` from ~320 to 51.

## Measurements

Measured on `esbmc-unix2/23_picosat-846_01` against `330aed17` (`vereda-base`):

| configuration | `--unwind 1` wall (walk / peak) | `--unwind 2` wall (walk / peak) | no `--unwind`: to loop 0 iter 1 | no `--unwind`: first `strcmp` |
|---|---|---|---|---|
| `vereda-base` | 7.81 s (0.094 s / 368 MB) | 8.12 s (0.381 s / 361 MB) | 9.92 s (0.48 → 10.40 s) | >20 s |
| Fixes 1 + 2 only | 7.99 s (0.089 s / 364 MB) | 8.27 s (0.383 s / 389 MB) | 3.86 s (0.48 → 4.34 s) | 11.48 s (7.11 → 18.59 s) |
| Fixes 1 + 2 + 3 | 0.60 s (0.030 s / 61 MB) | 0.63 s (0.074 s / 62 MB) | 0.05 s (0.49 → 0.54 s) | 0.37 s (0.72 → 1.09 s) |

SMT formula size for `--unwind 1` dropped from 1,002 KB (583 `base#` symbols,
584 `select bases` assertions) to 186 KB (51 `base#` symbols, 52 `select bases`
assertions).

Side-by-side `tests/bench.py` highlights (`vereda-base` → `vereda`):

| test | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| `esbmc-unix2/23_picosat-846_01` | 0.39 → 0.08 | 5.84 → 0.05 | 6.73 → 0.62 | 375 → 61 | FAILED (wall x0.09, mem x0.16) |
| `vereda/lib_strtok` | 0.01 → 0.01 | 5.26 → 0.96 | 5.37 → 1.07 | 105 → 62 | SUCCESSFUL (wall x0.20) |
| `vereda/mem_copy_pointers` | 0.00 → 0.00 | 0.09 → 0.07 | 0.19 → 0.17 | 47 → 41 | SUCCESSFUL |
| `vereda/mem_elem_memcpy_pointer` | 0.00 → 0.00 | 0.12 → 0.09 | 0.21 → 0.19 | 44 → 42 | SUCCESSFUL |

On `esbmc/github_302`, the check formula after `(push 1)` is bit-for-bit
identical; only 12 unused `base#1..12` / `bases` assertions were removed, which
shifted Bitwuzla's FP search heuristic from 16.88 s to 26.14 s.

Full `ctest -j12`: 6,959 tests, 100% passed, 0 failed (4,184 skipped, 127.30 s).
