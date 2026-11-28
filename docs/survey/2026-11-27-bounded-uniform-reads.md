# Bounded uniform reads instead of eager zero arrays

Recorded 2026-11-27.

This supersedes the implementation in `2026-11-27-small-zeroed-globals.md`.
CI on PR #464 found a `32_floppy` memory regression (1,032 to 1,729 MB)
and five changed regression verdicts after enabling constant-array fills for
all small globals.

The eager fill also extended zeros beyond initialized storage. This hid
failures in `github_133`, `github_134`, `github_481_1_false`,
`github_481_2_false` and `struct_bitfields_13`, whose invalid accesses or
copies previously read unknown bytes. Changing their expectations would
hide the problem. The eager initialization change is therefore reverted.
Checking memory-function ranges remains the separate work in #463.

## Replacement

`Memory::load()` already folds reads over up to 16 known candidate locations
when all hold the same value, and can represent up to eight distinct pointer
choices. Extend only the uniform case to domains needing at most 1,024 byte
lookups, the size below which the initialization store chains arise.

Every candidate byte must exist in `st.known`. Every loaded value must be
structurally identical; pointer loads also require identical first-byte
provenance, matching the existing pointer-load semantics. Missing or
different bytes use the original memory path. The extended scan stops on
the first different value and stores at most the original 16 candidates,
so it neither allocates a large choice table nor builds a large ITE tree.
The existing 16-location path and eight-pointer choice limit are unchanged.

The optimization consumes the current path's bounded offset range but does
not modify memory or cache a path-specific result globally. A uniform read
on one branch cannot overwrite a sibling's different contents. Constant
out-of-bounds reads and symbolic ranges including unknown bytes retain
their prior behavior. Global initialization and runtime fill thresholds
are unchanged.

## Matched measurements

Both builds use main `1b6af0e1`, LLVM 23.1.6, camada 0.25,
RelWithDebInfo and mimalloc 3.7.0. Each run is pinned to one CPU with a
4 GiB address-space limit. These are single paired observations.

`32_floppy` uses the same compiled 64-bit IR for both binaries, with:

```
--unwind 1 --ub-pointer-check=false --ub-div-by-zero-check=false --error-label ERROR
```

| Metric | Main | Replacement |
| --- | ---: | ---: |
| Walk | 47.923 s | 48.044 s |
| Solve | 16.276 s | 16.160 s |
| Total wall | 66.36 s | 66.37 s |
| Peak RSS | 1,024 MiB | 1,022 MiB |

Both report FAILED. Peak RSS is from `/usr/bin/time`, including error-trace
construction, and converted from KiB to MiB.

The 1,000-byte zeroed-global/helper-read reproducer retains its improvement:
49.25 s total / 49.033 s solving / 579 MiB on main, versus 0.20 s total /
below 1 ms solving / 43 MiB with the replacement. Those measurements include
the C front end and use `--unwind 1`; both report SUCCESSFUL.

## Regression coverage

The five CI failures are checked without changing their fixtures or
expectations; the existing FUTURE cases keep their prior skipped status.
New 32-bit and 64-bit tests cover an unknown byte just beyond a uniform
initialized prefix, a uniform range on one branch with a different value
on its sibling, uniform multi-byte values and uniform pointer tables.
The original seven zero-global regression configurations remain in place.

The complete local regression run passes: 3,652 passed, 3,685 skipped and
no failures across 7,337 executions, in 264.80 s.

Follow-up issues: #463 (memory-function bounds).
