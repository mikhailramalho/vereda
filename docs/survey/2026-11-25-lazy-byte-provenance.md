# Lazy provenance selects on byte-array reads (#184)

Follow-up from #182 (`docs/survey/2026-11-24-terms-read.md`). Compared against
`main` at `59b54a14` with LLVM 23.1.6 and camada 0.25 (Bitwuzla 0.11.0).

## Change

`Memory::readByte`, `Memory::readArrays`, and `Memory::readOwn` now take a
`withProvenance` flag (default `true`). When `false`, byte-array reads omit
`t.select(..., prov)` and the provenance `t.ite(...)` across symbolic pointer
candidates and `t.split` branches, returning a null provenance term.

Data-only paths opt out:
- `Memory::load` requests provenance only on byte 0 of a pointer load; bytes
  `1..n-1` of pointer loads and all bytes of integer/float/other loads pass
  `withProvenance = false`.
- `Memory::copyArray`'s data lambda passes `withProvenance = false`.

All provenance-carrying paths (`writeByte`, `ite`, `copy`, `copyArray`'s
provenance lambda, and byte 0 of pointer `load`) keep `withProvenance = true`.

## Exactness

Comparing `--dump-smt-only` output across 193 `vereda/lib_*` and `vereda/mem_*`
tests shows 184 byte-for-byte identical formulas and 9 tests with identical
byte lengths whose only difference is `@def` numbering order (unused provenance
terms are no longer interned in `Terms` before data terms).

Two new regression tests (`vereda/mem_data_read_then_pointer` and
`vereda/mem_data_read_then_pointer_large`) verify that data-only byte and float
reads through pointer choices and large-copy lambdas leave pointer provenance
intact for subsequent loads and copies.

## Measurements

Side-by-side runs via `tests/bench.py` on byte-heavy tests (string models,
`memcpy`/struct copies, and array summaries):

| Test | Walk s (base → head) | Solve s (base → head) | Peak MB (base → head) |
|---|---|---|---|
| `esbmc-cstd/scanf-false-3` | 18.31 → 18.60 | 13.09 → 13.03 | 489 → 449 |
| `vereda/lib_strtol` | 0.01 → 0.01 | 3.62 → 3.62 | 135 → 124 |
| `vereda/lib_sprintf` | 0.14 → 0.14 | 0.21 → 0.20 | 72 → 70 |
| `vereda/lib_strtok` | 0.01 → 0.01 | 1.02 → 1.00 | 63 → 62 |
| `vereda/lib_qsort` | 0.00 → 0.00 | 1.27 → 1.27 | 43 → 40 |
| `vereda/mem_copy_pointers` | 0.00 → 0.00 | 0.08 → 0.08 | 42 → 40 |
| `vereda/mem_data_read_then_pointer` | 0.00 → 0.00 | 0.02 → 0.02 | 44 → 42 |

`tests/bench.py` reports zero regressions across all 18 measured tests, with
consistent peak-memory reductions (40 MB on `scanf-false-3`, 11 MB on
`lib_strtol`). Full `ctest` suite: 2,825 passed, 4,177 expected skips, 0 failed.
