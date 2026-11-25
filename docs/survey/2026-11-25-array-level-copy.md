# Array-level copy for large memcpy and memmove

Recorded 2026-11-25.

`esbmc/struct_array_ite_new_index` copies a 32,768-byte struct on each arm of a
conditional (`struct W w = b ? w1 : w2;`), lowered to two 32,768-byte
`llvm.memcpy` calls. `Memory::copy()` had no array-level path (unlike
`Memory::set()`, which uses `fillObject()` for whole-object fills of at least
`MinSeparate` bytes), so it looped byte by byte on both branches. At the join,
both sides flushed into `w`'s array, producing 65,544 stores and 65,548 selects
(an 11.5 MB formula) that caused Bitwuzla to time out past 300 s on the UNSAT
check and take 14.6 s / 754 MB on `struct_array_ite_new_index_fail`.

`Memory::copyArray()` now handles copies of at least `MinSeparate` (1,024) bytes
into a known destination object without byte-by-byte stores. First, all
candidate source objects are flushed from `known` into their arrays. Two direct
fast paths avoid even a lambda: a whole-object copy between two `owned` objects
with matching element width reuses the source's arrays directly, and a
whole-object copy from a constant-filled object becomes `fill()`. Otherwise,
`dst` is flushed (or its known bytes erased for a whole-object overwrite), the
pre-copy state is snapshotted so overlapping `memmove` reads the bytes as they
were before the copy, and `dst`'s entry in `separate` becomes a `Terms::lambda`
reading `readArrays(srcState, advance(src, offset - dstOff))` for offsets in
`[dstOff, dstOff + n)` and falling through to the pre-copy array outside that
range (with a `misses` predicate forwarding out-of-range reads directly). When
no source candidate and no surviving destination bytes can hold a pointer, the
destination's provenance stays a constant `IntProvenance` array; otherwise a
matching provenance lambda is built and `provBelow` records the underlying
source arrays per source object so `Memory::pointers()` still discovers pointers
copied into `dst` across bodyless calls.

Side-by-side `tests/bench.py` measurements against main (`--timeout 60`):

| test | walk s | solve s | wall s | peak MB | verdict | change |
|---|---|---|---|---|---|---|
| esbmc/struct_array_ite_new_index | 0.56 → 0.00 | - → 0.01 | - → 0.11 | 142 → 39 | TIMEOUT → SUCCESSFUL | base timed out |
| esbmc/struct_array_ite_new_index_fail | 0.47 → 0.00 | 14.61 → 0.01 | 15.34 → 0.11 | 754 → 38 | FAILED | wall time x0.01, memory x0.05 |
| esbmc/array_loop_summary | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 39 → 37 | SUCCESSFUL | |
| esbmc/array_loop_summary_fail | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.10 | 38 → 37 | FAILED | |
| esbmc/array_loop_summary_struct | 0.00 → 0.00 | 0.03 → 0.03 | 0.14 → 0.14 | 43 → 41 | SUCCESSFUL | |
| esbmc/github_302 | 36.37 → 35.85 | 7.22 → 7.16 | 43.72 → 43.13 | 573 → 559 | FAILED | |
| esbmc/nested_array_plane_memcpy | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.12 | 40 → 39 | SUCCESSFUL | |
| esbmc/nested_array_plane_memcpy_fail | 0.00 → 0.00 | 0.01 → 0.01 | 0.11 → 0.11 | 40 → 39 | FAILED | |
| esbmc/nested_array_row_memcpy_at_cap | 0.01 → 0.01 | 4.78 → 4.79 | 4.91 → 4.92 | 362 → 361 | SUCCESSFUL | |
| esbmc/nested_array_3d_row_memcpy | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.12 | 40 → 38 | SUCCESSFUL | |

`esbmc/struct_array_ite_new_index` is promoted from `KNOWNBUG` to `CORE`, ten
focused tests under `tests/vereda/mem_copy_*.c` cover large joins, partial
copies, overlapping forward and backward `memmove`, pointer provenance and
bodyless-call reachability, and conditional copies, and the full 6,897-test
suite passes with zero failures.
