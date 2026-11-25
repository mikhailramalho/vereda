# Memory-stage hot-path performance and memory optimizations

Recorded 2026-11-25.

Five correctness-preserving hot-path optimizations in `src/memory/memory.{h,cpp}`
eliminate redundant term construction, quadratic scans over separate objects,
unbounded DAG traversal, and throwaway pointer-constant interning:

## Design

1. **Short-circuit constant `isInt` in `Memory::load`**:
   When loading a pointer, `isInt = (obj == IntProvenance)` is almost always
   constant `false` (ordinary pointers) or constant `true` (integer-provenance
   bytes). Previously `t.ite(isInt, fromAddress(bits), ptr(obj, sub(bits, base(obj))))`
   eagerly evaluated both arms before `Terms::ite` discarded the dead one. That
   caused every normal pointer load to walk every exposed object in
   `fromAddress(bits)` and pollute `fromInteger` with unused entries. Now
   `t.isTrue(isInt)` returns `fromAddress(bits)` directly (which already records
   `nullEq`), and `t.isFalse(isInt)` skips `fromAddress(bits)` while keeping the
   `nullEq` entry (`bits == 0 && obj == 0`) so null comparisons still avoid
   reading `bases`.

2. **`Memory::mergeSeparate` via `CellMap::diff`**:
   Previously `mergeSeparate` constructed a fresh empty `CellMap` and iterated
   every entry of both `a.separate` and `b.separate`. Starting from
   `CellMap r = a.separate` and running `CellMap::diff(a.separate, b.separate, ...)`
   visits only objects whose separate arrays differ across the join, preserving
   structural sharing of unchanged separate subtrees.

3. **DAG memoization in `Memory::candidates` and candidate-scoped `writeByte` /
   `readArrays`**:
   `candidates()` now tracks visited `t.choice` nodes in a
   `llvm::SmallPtrSet<const void *, 16>` so shared pointer-`ite` DAGs are
   traversed in linear time instead of exponentially. In `writeByte` (non-constant
   pointer), a constant object id delegates directly to `storeArrays`
   (`O(log |separate|)`); a symbolic object id iterates only over
   `candidates(at)`, skips entries whose `object(at) == o` folds to `false`
   before calling `storeOwn`, and skips updating `st.data`/`st.prov` when every
   candidate is in `st.separate`. `readArrays` mirrors this for symbolic object
   ids, iterating only over `candidates(at)`, skipping `t.isFalse(is)` before
   `readOwn`, and omitting `st.data`/`st.prov` selects when all candidates are
   separate.

4. **Constant-pointer integer-key loops in `Memory::load`, `setBytes`, and
   `copy`**:
   Mirroring `Memory::storeBytes`, constant-address multi-byte loads, byte fills,
   and byte copies compute `(k->first, (k->second + i) & mask)` directly and
   access `st.known` without interning throwaway 96-bit `bytePtr` `APInt`
   constants (falling back to `readArrays(st, ptr(ki))` only on a `st.known`
   miss).

5. **Lazy `sizes` SMT array in `Memory::size(Term obj)`**:
   Previously a non-constant `size(obj)` built an `O(N)` `ite` chain over all
   allocated objects. Mirroring `bases`, `Memory` now lazily allocates an
   uninterpreted `sizes` array (`ObjBits -> ptrBits`) on the first symbolic
   `size()` query, constraining `sizes[0..N-1]` on creation and `sizes[id] == size`
   in `allocate()` thereafter. Programs without symbolic `size()` queries pay no
   extra symbols or constraints.

## Measurements

Side-by-side comparison with `tests/bench.py` against `origin/main` (`afc20c02`,
clang 23.1.6, RelWithDebInfo):

| test | walk s | solve s | wall s | peak MB | verdict | change |
|---|---|---|---|---|---|---|
| esbmc-unix2/23_picosat-846_01 | 0.52 → 0.48 | 12.56 → 12.08 | 13.60 → 13.10 | 379 → 385 | FAILED | |
| esbmc/github_302 | 0.00 → 0.00 | 27.62 → 28.36 | 27.75 → 28.49 | 525 → 534 | FAILED | |
| esbmc/struct_array_ite_new_index | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 38 → 38 | SUCCESSFUL | |
| esbmc/struct_array_ite_new_index_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 39 → 38 | FAILED | |
| esbmc/nested_array_row_memcpy_at_cap | 0.00 → 0.00 | 0.00 → 0.00 | 0.12 → 0.12 | 38 → 39 | SUCCESSFUL | |
| esbmc/nested_array_row_memcpy_at_cap_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.12 → 0.13 | 38 → 38 | FAILED | |
| esbmc/array_loop_summary | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.11 | 37 → 38 | SUCCESSFUL | |
| esbmc/array_loop_summary_decrement | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 37 → 37 | SUCCESSFUL | |
| esbmc/array_loop_summary_store | 0.00 → 0.00 | 0.08 → 0.08 | 0.19 → 0.19 | 40 → 39 | SUCCESSFUL | |
| esbmc/array_loop_summary_update | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 38 → 37 | SUCCESSFUL | |
| esbmc/array_loop_summary_unroll | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.11 | 35 → 35 | SUCCESSFUL | |
| vereda/mem_own_arrays | 0.00 → 0.00 | 0.05 → 0.05 | 0.16 → 0.16 | 43 → 44 | SUCCESSFUL | |
| vereda/mem_own_large_unrolled | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 35 → 35 | SUCCESSFUL | |
| vereda/mem_constant_stores | 0.00 → 0.00 | 0.01 → 0.01 | 0.11 → 0.11 | 38 → 38 | SUCCESSFUL | |
| vereda/mem_null_fromaddress | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.12 | 39 → 40 | SUCCESSFUL | |
| vereda/mem_symbolic_separate | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.12 | 40 → 40 | SUCCESSFUL | |
| vereda/mem_merge_separate | 0.00 → 0.00 | 0.01 → 0.01 | 0.11 → 0.11 | 38 → 39 | SUCCESSFUL | |
| vereda/mem_load_ptr_isint | 0.00 → 0.00 | 0.72 → 0.72 | 0.82 → 0.83 | 52 → 51 | SUCCESSFUL | |
| vereda/mem_symbolic_size | 0.00 → 0.00 | 0.08 → 0.09 | 0.19 → 0.21 | 44 → 45 | SUCCESSFUL | |

Unrolled constant-index array loops with `--no-summarize-array-loops` (single
runs, showing the impact of integer-key `Memory::load` avoiding 96-bit `bytePtr`
interning):

| test | mode | walk s | peak MB |
|---|---|---:|---:|
| esbmc/array_loop_summary | `--no-summarize-array-loops` | 1.865 → 1.525 (-18.2%) | 242 → 174 (-28.1%) |
| esbmc/array_loop_summary_decrement | `--no-summarize-array-loops --dump-smt-only` | 1.063 → 0.988 (-7.1%) | 255 → 256 |

Full `ctest --test-dir build -j10 --output-on-failure` suite: 100% passed,
0 failed out of 6,943 tests in 167.55 s (including 8 new regression tests under
`tests/vereda/`).
