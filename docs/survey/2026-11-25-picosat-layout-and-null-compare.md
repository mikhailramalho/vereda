# Unconditional layout for small allocations and null-comparison simplification

Recorded 2026-11-25.

On `origin/main` (`0f60211a`), `esbmc-unix2/23_picosat-846_01` took 239.54 s wall
time (4.87 s walk, 234.08 s solve) and 2,655 MB peak memory to reach its first
failing check (`main.unwind.0` at `main.c:79`). Three memory-encoding issues
compounded to produce that blow-up:

1. **Conditional `Memory::allocate()` pulled path guards into the global solver
   context.** Every conditional `alloca` or `malloc` asserted
   `implies(exists, layout)` globally and updated `end = ite(exists, e, end)`.
   The 9 conditional allocations inside `picosat_main` pulled 2,743 of 3,082
   definitions (including `strcmp`, global initialisers, and `fromAddress()`
   chains) into the global solver context before `main.unwind.0` was even
   queried.
2. **`fromAddress(0)` built a full exposed-object ITE chain.** Address `0` can
   never lie in any allocated object because `end` starts at `1` and object
   layouts do not wrap, yet `fromAddress(0)` still iterated over every exposed
   object. That prevented constant folding on null function pointers, keeping
   dead indirect-call branches alive during the walk (adding 5 spurious checks
   and two `__vereda_unknown_target` calls).
3. **Null comparisons on loaded and integer-provenance pointers expanded
   `fromAddress()` and `bases`.** `Memory::equal(p, null())` built
   `address(p) == 0` (reading `select(bases, object(p))`), and `argv[counter] != NULL`
   loaded a pointer with `IntProvenance` from `memory#0`, sending a 324-object
   `fromAddress(bits)` ITE chain into `main.unwind.0`.

## Design

All three fixes live in `src/memory/memory.{h,cpp}`:

1. **Unconditional layout for small constant-size allocations**
   (`Memory::allocate`): Track a running upper bound `reserved` on `end`
   (starting at `1`) and a flag `endBounded`. For any reachable allocation
   (`!t.isFalse(exists)`) whose `size` is a constant fitting in 64 bits, if
   `endBounded` holds and `reserved + size + max(align, 1) <= 2^(ptrBits - 2)`
   (computed in `unsigned __int128`), assert `layout` unconditionally and set
   `end = e` unconditionally. Because `layout` constrains only the fresh base
   symbol and leaves at least three quarters of the address space free (more
   than `PTRDIFF_MAX`), it cannot remove valid inputs on any path.
   `objects.back().exists` still records the real `exists` guard so
   `fromAddress()` never maps an address into an unallocated object. Once any
   symbolic-size or over-budget allocation occurs, `endBounded` becomes `false`
   and subsequent conditional allocations keep the guarded `implies(exists, layout)`
   and `end = ite(exists, e, end)`.
2. **`fromAddress(0)` fast path** (`Memory::fromAddress`): When `a` is constant
   `0`, return `null()` immediately.
3. **Null-comparison simplification** (`Memory::equal`, `Memory::isNull`,
   `Memory::fromAddress`, `Memory::load`): When either operand of `equal(p, q)`
   is `null()`, route to `isNull(other)` instead of building `address()` or
   `outside` tests (no allocated object holds address `0`, and outside pointers
   in object `0` have nonzero offset). Record simplified null predicates in
   `nullEq`:
   - `fromAddress(a) == null()` is equivalent to `a == 0`.
   - A loaded pointer `ite(isInt, fromAddress(bits), ptr(obj, sub(bits, base(obj))))`
     equals `null()` iff `bits == 0 && (isInt || obj == 0)`. This avoids both
     `fromAddress(bits)` and `select(bases, obj)` while remaining exact even
     for a wild out-of-bounds pointer with `obj != 0` and numeric address `0`.
   `isNullMemo()` pushes through `ite` nodes with per-call memoisation only when
   a branch contains a `nullEq` entry, leaving unrelated `ite` trees untouched.

## Measurements

Incremental timings on `esbmc-unix2/23_picosat-846_01` (single runs via
`tests/run.py`, clang 23.1.6, RelWithDebInfo):

| stage | walk s | solve s | wall s | peak MB | checks | SMT size |
|---|---:|---:|---:|---:|---:|---:|
| baseline (`0f60211a`) | 4.87 | 234.08 | 239.54 | 2655 | 9 | 11 MB |
| + unconditional layout (Fix 1) | 4.70 | 217.30 | 222.58 | 2641 | 9 | 11 MB |
| + `fromAddress(0)` & `isNull`/`nullEq` (Fixes 1–3) | 0.49 | 8.96 | 9.96 | 419 | 4 | 1.2 MB |

Side-by-side comparison with `tests/bench.py` against `0f60211a`:

| test | walk s | solve s | wall s | peak MB | verdict | change |
|---|---|---|---|---|---|---|
| esbmc-unix2/23_picosat-846_01 | 4.96 → 0.54 | - → 9.72 | - → 10.76 | 275 → 419 | TIMEOUT → FAILED | base timed out |
| vereda/mem_inttoptr | 0.00 → 0.00 | 1.81 → 1.85 | 1.92 → 1.95 | 60 → 59 | SUCCESSFUL | |
| vereda/mem_inttoptr_copy | 0.00 → 0.00 | 0.41 → 0.28 | 0.52 → 0.40 | 55 → 56 | SUCCESSFUL | |
| vereda/mem_null_absent_object | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 36 → 35 | SUCCESSFUL | |
| vereda/mem_null_stored | 0.00 → 0.00 | 0.00 → 0.00 | 0.12 → 0.11 | 36 → 36 | SUCCESSFUL | |
| vereda/mem_outside_pointer | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 35 → 34 | SUCCESSFUL | |
| vereda/mem_outside_pointer_exposed | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.11 | 36 → 36 | FAILED | |
| vereda/mem_ptr_compare | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.10 | 34 → 35 | SUCCESSFUL | |
| vereda/mem_ptr_eq_address | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.12 | 36 → 36 | SUCCESSFUL | |
| vereda/mem_alloc_vacuity1 | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 36 → 36 | FAILED | |
| vereda/mem_alloc_vacuity2 | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.10 | 36 → 36 | FAILED | |
| vereda/mem_huge_stack_object | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.10 | 34 → 34 | SUCCESSFUL | |
| vereda/mem_malloc_split | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 39 → 39 | FAILED | |

Full `ctest -j12` suite: 6,918 tests (2,734 passed, 4,184 skipped, 0 failed) in
141.12 s, with zero regressions and no KNOWNBUG or FUTURE verdict changes.
