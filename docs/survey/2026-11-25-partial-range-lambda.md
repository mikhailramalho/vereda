# Partial-range array lambdas for havoc, fills and splat globals

Recorded 2026-11-25.

## Design

Previously, `Memory::havocWith`, `Memory::set`, `Memory::fillOrSet` and
`Memory::storeConstant` only used constant arrays when an operation covered a
whole object of at least `MinSeparate` (1024) bytes. Any partial havoc
(`__VERIFIER_nondet_memory` on a sub-array or struct field), partial `memset`,
large zeroed sub-array inside a non-zero global struct, or large repeated
non-zero constant array fell back to byte-by-byte or element-by-element stores,
building long store chains and failing with `Unsupported` past `MaxBytes`.

`Memory::applyRange` handles ranges of at least `MinSeparate` bytes in a known
non-zero object by wrapping the object's arrays (separate or shared) in a
`Terms::lambda`, mirroring `Memory::summarise`:

- For a constant start offset without wraparound, `CellMap::eraseRange` drops
  only the overwritten known bytes in `[off, off + n - 1]`, keeping surrounding
  fields in `st.known` without flushing them into a store chain. For a symbolic
  start offset, `flush(st, o)` runs before constructing the lambda.
- For word-element objects (`shifts.lookup(o) > 0`), the lambda operates on
  element indices when both offset and byte length are element-aligned;
  unaligned accesses fall back to the byte loop.
- Range membership uses a single unsigned subtraction test
  `ult(sub(off, start), n)`, with `misses` skipping the lambda for other
  objects and out-of-range offsets via `Terms::range`. Inside the lambda body,
  `t.isTrue(in)` short-circuits before reading `below.data` so stacked lambdas
  do not recursively force reads below them.
- Provenance keeps `below.prov` unchanged when `t.filledWith(below.prov)` is
  already `IntProvenance`. Otherwise a provenance lambda is built and
  registered in `provBelow`, so `Memory::pointers` remains sound across
  bodyless calls.
- `Memory::havocRange` backs the havocked slice with a fresh uninterpreted
  array `freshArr` and returns `select(freshArr, j)` inside the range.
- `Memory::fillRange` handles partial `set` and `fillOrSet` (including sizes
  past `MaxBytes`).
- `Memory::storeConstant` detects repeated (possibly nested) `ConstantDataArray`
  and `ConstantArray` initialisers via `splatElement` and routes uniform-byte
  splats through `fillOrSet` and multi-byte scalar splats through
  `fillElementRange` (using `constArray` when a whole word-element object
  matches, or a byte-selecting partial lambda otherwise).

## Adversarial experiment and `ite`-join fix

Four adversarial patterns were measured against `main` (`build/vereda`,
RelWithDebInfo, LLVM 23.1.6, camada, mimalloc):

1. `adv_havoc_const_reads`: 1024-byte partial havoc at `MinSeparate`, followed
   by summing all 1024 bytes at constant indices (base 53.30 s / 197 MB vs head
   52.39 s / 210 MB — parity).
2. `adv_stacked_partial`: 32 overlapping 1024-byte partial havocs/fills on a
   4096-byte array, then a symbolic read outside the overwritten prefix (base
   0.27 s / 64 MB vs head 0.13 s / 40 MB — 2x faster).
3. `adv_ite_join_havoc`: `if (c)` joining a 1024-byte partial havoc with a
   1024-byte partial `memset`, then summing 512 constant indices (base 11.68 s
   / 137 MB vs head 10.72 s / 126 MB).
4. `adv_ite_join_fill` (now `tests/vereda/mem_partial_fill_join.c`): `if (c)`
   joining two 1024-byte partial `memset`s (`3` vs `5`), then summing 512
   constant indices.

Before fixing `Terms::uncached`, `adv_ite_join_fill` regressed from 0.001 s
solve / 41 MB peak on `main` to 0.251 s solve / 98 MB peak: `main` kept the
1024 bytes in `st.known` and joined them as pure bitvector `ite(c, 3, 5)`,
whereas the partial lambdas produced `ite(c, lambda1, lambda2)` and
`Terms::uncached` emitted `select(ite(c, lambda1, lambda2), i)` plus two
global `select(lambda, i) == def(i)` constraints per read.

Adding a flat-`ite` reduction in `Terms::uncached` — when reading from a
non-nested `ite(c, a, b)` whose arms include a lambda (or two constant arrays),
reduce directly to `ite(c, select(a, index), select(b, index))` — eliminated
the regression completely (0.001 s solve / 0.126 s wall / 40 MB peak, matching
`main`), without risking exponential blow-up on nested `ite`s.

## Measurements (`tests/bench.py`)

Side-by-side comparison of `main` vs `memory-partial-lambda` with
`tests/bench.py` (median of 3 runs, LLVM 23.1.6 clang):

| test | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| `vereda/mem_partial_havoc` | - -> 0.00 | - -> 0.00 | 0.13 -> 0.11 | - -> 36 | UNKNOWN -> SUCCESSFUL |
| `vereda/mem_partial_havoc_false` | 0.02 -> 0.00 | 0.15 -> 0.00 | 0.27 -> 0.10 | 66 -> 37 | FAILED |
| `vereda/mem_partial_havoc_call_false` | 0.00 -> 0.00 | 0.00 -> 0.00 | 0.11 -> 0.10 | 42 -> 36 | FAILED |
| `vereda/mem_global_partial_zero` | 0.07 -> 0.00 | - -> 0.01 | 48.58 -> 0.12 | 54 -> 38 | KILLED (OOM) -> SUCCESSFUL |
| `vereda/mem_global_splat` | 0.07 -> 0.00 | - -> 0.01 | - -> 0.12 | 54 -> 38 | TIMEOUT -> SUCCESSFUL |
| `vereda/mem_global_splat_false` | 0.03 -> 0.00 | 0.90 -> 0.01 | 1.04 -> 0.12 | 126 -> 38 | FAILED |
| `vereda/mem_partial_fill_join` | 0.01 -> 0.01 | 0.00 -> 0.00 | 0.12 -> 0.12 | 39 -> 38 | SUCCESSFUL |
| `vereda/mem_large_memset` | - -> 0.00 | - -> 0.00 | 0.11 -> 0.11 | - -> 37 | UNKNOWN -> SUCCESSFUL |
| `vereda/mem_fill_whole` | 0.00 -> 0.00 | 0.01 -> 0.01 | 0.12 -> 0.12 | 40 -> 38 | SUCCESSFUL |
| `vereda/mem_own_arrays` | 0.00 -> 0.00 | 0.05 -> 0.05 | 0.16 -> 0.16 | 45 -> 44 | SUCCESSFUL |
| `vereda/mem_words` | 0.00 -> 0.00 | 0.03 -> 0.03 | 0.14 -> 0.14 | 43 -> 42 | SUCCESSFUL |

Full test suite (`ctest --test-dir build -j12 --output-on-failure`): 6894/6894
tests passed (0 failures; 388/388 in `-L vereda` with 1 pre-existing skip).
