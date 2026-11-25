# Exact finite-table patterns

Extend PR #249's constant affine table compression (measured in
`2026-11-25-nonincremental-constant-tables.md`) to symbolic bases, point
overrides, quadratics, periods and contiguous affine segments. Recognition
uses only the already evaluated elements in `Memory::flushObject`, never
the loop IR, a model, or path-specific equalities. Every candidate must
match every element; otherwise the existing encoding remains available.
Loop summaries and solver settings are unchanged.

`memory/table.cpp` returns a compact function for the table's finite domain.
The caller retains the original default outside that domain and the existing
provenance encoding. All arithmetic is modular at the element width. Shared
symbolic addends/subtrahends cancel in `Terms::sub`; a constant difference
therefore also recognizes symbolic bases without solver queries.

The searches have fixed bounds, so recognition is O(N). Affine candidates
allow at most min(8, N/16) exact point overrides. Trying the first 2k+1
adjacent pairs suffices to find an intact pair when there are k exceptions;
duplicate candidates are skipped. Periods are restricted to 2, 4 and 8,
with at least two repetitions. Piecewise affine tables require at least
64 elements, at most eight segments and at least eight elements per segment.
Constant segments and negative strides are supported.

Quadratics require at least 256 elements and constant second differences.
The function is c0 + c1*j + c2*j*(j-1)/2. Computing the product modulo
2^(w+1), then extracting bits w:1, implements the exact triangular number
modulo 2^w, including odd c2. The original index must retain w+1 bits before
that multiplication: truncating it to w bits first is wrong past index
2^w for odd c2. This is covered by the 1,024-element byte regression.
The size threshold limits the introduction of nonlinear multiplication;
it is a heuristic, not a guarantee for arbitrary solver workloads.

## Measurements

LLVM 23.1.6, camada 0.25, Bitwuzla 0.11.0, Release build. Comparing the
previous PR #249 binary with these extensions, with loop summaries disabled:

| table | previous solve s | extended solve s | previous / extended peak MB |
|---|---:|---:|---:|
| symbolic base and two overrides, 256 words | >30 | 0.017 | timeout / 30 |
| periodic, 256 words | 1.632 | 0.003 | 79 / 30 |
| piecewise affine, 256 words | 11.550 | 0.029 | 99 / 31 |
| quadratic, 1,024 bytes | >30 | 0.009 | timeout / 30 |
| quadratic, 256 64-bit words | 1.601 | 0.174 | 78 / 39 |

These are individual local measurements, not aggregate speedup guarantees.
The original failing decrement benchmark still solves in 0.045 s with
159 MB peak memory (0.893 s walking), retaining PR #249's initial gain.
The nonlinear threshold of 256 also benefits the measured 64-bit case.
Arbitrary formulas, unsupported periods, excessive overrides/segments and
damaged quadratic prefixes fall back; synthesizing general expressions
from values is outside this change. General IR expressions belong in loop
summarization, where their original expression is available.

## Validation

Five end-to-end regressions cover symbolic overrides, periods, segments,
quadratic wraparound and a late quadratic mismatch. An exhaustive helper
test checks every element of accepted patterns at widths 8, 16, 32 and 64,
including symbolic values, first/middle/last overrides, odd second
differences and rejection beyond the matching limits. Symbolic equalities
that do not fold are proved with Bitwuzla in the test.

The full 7,019-test suite has 2,842 passes, 4,177 skips and no failures.
The previous binary also passes its 7,011 source regressions. Comparing the
paired suite statistics with `tests/bench.py` selects no changed tests and
reports no regressions. The six scalar nonlinear benchmarks from the
earlier survey retain byte-identical fixed-bound formulas and the same
incremental-BMC verdicts after these extensions.
