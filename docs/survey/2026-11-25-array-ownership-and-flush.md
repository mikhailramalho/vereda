# Array ownership, element arrays across memcpy, and compressed flushes

Recorded 2026-11-25.

`tests/esbmc/nested_array_row_memcpy_at_cap.c` writes 256 elements of
`int a[2][128]` in straight-line code (`a[0][..] = 9`, `a[1][..] = 1`), copies
512 bytes from `int b[128]` (all `5`) over `a[1]`, and asserts
`a[1][i] == 5 && a[0][i] == 9` at a symbolic `i`. On main (`2263b0d3`) the
walk takes 0.006 s, but solving takes 4.79 s and 361 MB because `prepare()`
flushes 1,024 known bytes into the shared 96-bit-indexed `memory#0` array,
leaving Bitwuzla with 8 symbolic byte selects over a 1,024-deep store chain.

Three independent changes fix the three compounding causes:

1. **Capture-tracking use cap (`Walk::exposed`, `Walk::ownsArrays`).**
   LLVM's `PointerMayBeCaptured` defaults `MaxUsesToExplore` to 100. An array
   written by straight-line code or an unrolled loop easily exceeds 100 uses,
   causing LLVM to give up and report the pointer as captured and exposed.
   Raising the cap to a finite `MaxCaptureUses = 16384` (preserving the
   conservative captured/exposed fallback if exceeded) lets `ownsArrays` give
   such arrays their own offset-indexed arrays and lets `exposed` keep their
   addresses out of `fromAddress`. An unbounded cap was not adopted so
   pathological generated functions remain bounded in analysis time; on
   `esbmc/github_302` (a large function) walk time is unchanged (38.40 → 38.51 s).

2. **Aligned `memcpy`/`memmove`/`memset` on element arrays (`Walk::elementBits`).**
   `elementBits` previously rejected every call other than
   `__vereda_uninitialised`, forcing arrays touched by `memcpy`, `memmove`, or
   `memset` onto 8-bit byte arrays. It now accepts those calls (when their
   result is unused, as for the LLVM intrinsics) when the pointer's GEP offsets
   and the constant length are multiples of the element size `N/8`, and — when
   the object is the destination of a `memcpy`/`memmove` — the source's
   underlying object is an unexposed, non-escaping alloca or initialised global
   that holds no pointers (`holdsNoPointers`). Globals whose initialisers
   contain pointer types are also kept on bytes so pointer provenance cannot be
   lost on flush or copy-out. `Memory::copy` and `Memory::set` already handle
   element arrays at both constant and symbolic offsets via `readOwn`,
   `storeOwn`, and `flush`, so `Memory::copy`/`set` needed no changes.

3. **Compressed flushes (`Memory::flushObject`).**
   When every byte `[0, N)` of an object's constant size `N` is present in
   `st.known` (with no out-of-bounds keys) and the object already has separate
   arrays or `N >= MinSeparate`, the prior array is never read. `flushObject`
   packs element arrays into elements, finds the dominant value, and replaces
   the object's array instead of emitting stores on top of the nondet initial
   array. When a small number of contiguous non-dominant runs (at most 8 runs
   covering at least 8 elements and at least 2 elements per run) covers the
   remaining elements, it builds a `Terms::lambda` range `ite` over the
   dominant value, which `Terms::select` reduces directly on symbolic reads.
   Otherwise it starts from `constArray(indexSort, dom)` and emits stores only
   for differing elements. Byte arrays preserve per-byte provenance in the same
   way via a dominant-provenance `constArray` plus stores for differing bytes,
   which `Memory::pointers()` already handles.

## Measurements per change on `esbmc/nested_array_row_memcpy_at_cap`

Measured individually on the worktree with `/home/mgadelha/tools/llvm-23/bin/clang`:

| Stage | IR prep | Walk | Solve | Peak |
|---|---:|---:|---:|---:|
| Base (`2263b0d3`) | 0.011 s | 0.006 s | 4.792 s | 361 MB |
| + Change 1 (capture cap 16384) | 0.011 s | 0.005 s | 1.969 s | 181 MB |
| + Change 2 (`elementBits` across `memcpy`/`memset`) | 0.010 s | 0.003 s | 0.601 s | 85 MB |
| + Change 3 (compressed flush + range lambda) | 0.011 s | 0.002 s | 0.003 s | 39 MB |

## Side-by-side benchmark (`tests/bench.py`)

Measured side by side against base `/home/mgadelha/tools/vereda/build/vereda`:

| test | walk s | solve s | wall s | peak MB | verdict | change |
|---|---|---|---|---|---|---|
| esbmc/nested_array_row_memcpy_at_cap | 0.01 → 0.00 | 4.92 → 0.00 | 5.05 → 0.13 | 364 → 39 | SUCCESSFUL | wall time x0.03, memory x0.11 |
| esbmc/nested_array_row_memcpy_at_cap_fail | 0.01 → 0.00 | 0.04 → 0.00 | 0.17 → 0.13 | 44 → 38 | FAILED | |
| esbmc/nested_array_row_memcpy | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.11 | 40 → 38 | SUCCESSFUL | |
| esbmc/nested_array_row_memcpy_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 40 → 38 | FAILED | |
| esbmc/nested_array_3d_row_memcpy | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.11 | 40 → 39 | SUCCESSFUL | |
| esbmc/nested_array_3d_row_memcpy_fail | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.12 | 40 → 39 | FAILED | |
| esbmc/nested_array_plane_memcpy | 0.00 → 0.00 | 0.01 → 0.00 | 0.12 → 0.11 | 40 → 38 | SUCCESSFUL | |
| esbmc/nested_array_plane_memcpy_fail | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.12 | 40 → 38 | FAILED | |
| esbmc/nested_array_no_simplify_scale | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.10 | 36 → 35 | SUCCESSFUL | |
| esbmc/nested_array_no_simplify_scale_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 37 → 37 | FAILED | |
| esbmc/nested_array_loop_bound | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.10 | 37 → 36 | SUCCESSFUL | |
| esbmc/nested_array_3d_loop_bound | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.10 | 37 → 35 | SUCCESSFUL | |
| esbmc/nested_array_4d_outer_read | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 39 → 37 | SUCCESSFUL | |
| esbmc/nested_array_row_phi_merge | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 38 → 36 | SUCCESSFUL | |
| esbmc/array_loop_summary_decrement_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 39 → 37 | FAILED | |
| esbmc/array_loop_summary_counter_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 38 → 38 | FAILED | |
| esbmc/array_loop_summary_update | 0.00 → 0.00 | 0.00 → 0.00 | 0.12 → 0.11 | 39 → 37 | SUCCESSFUL | |
| esbmc/array_loop_summary_update_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 38 → 37 | FAILED | |
| esbmc/github_302 | 38.40 → 38.51 | 7.84 → 7.82 | 46.37 → 46.46 | 567 → 565 | FAILED | |
| esbmc/struct_array_ite_new_index_fail | 0.55 → 0.46 | 15.43 → 0.64 | 16.27 → 1.26 | 755 → 175 | FAILED | wall time x0.08, memory x0.23 |
| vereda/mem_own_arrays | 0.00 → 0.00 | 0.05 → 0.06 | 0.16 → 0.17 | 44 → 43 | SUCCESSFUL | |
| vereda/mem_large_memset | - → - | - → - | 0.12 → 0.12 | - → - | UNKNOWN | |
| vereda/mem_memcpy_memset | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 36 → 34 | SUCCESSFUL | |
| vereda/mem_known_bytes | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.12 | 40 → 39 | SUCCESSFUL | |
| vereda/mem_words_unknown | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.10 | 38 → 37 | SUCCESSFUL | |

`esbmc/struct_array_ite_new_index_fail` also improved substantially (15.43 → 0.64 s
solve, 755 → 175 MB peak) from compressed flushes and ownership. Full suite
(`ctest -j12`): 6,901 tests, 0 failures, 4,192 skipped, 383.53 s wall, with no
verdict changes on any KNOWNBUG or FUTURE test.
