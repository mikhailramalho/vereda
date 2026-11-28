# Constant arrays for small zeroed globals

Recorded 2026-11-27.

Issue #297 reports a large solving-time discontinuity at
`Memory::MinSeparate` (1,024 bytes). A small zeroed global can be flushed as
one store per byte over nondeterministic memory. Proving a symbolic read is
zero then requires the solver to reason through those stores, whereas a
larger global already uses a constant array.

## Reproduction on current main

A direct symbolic read of a global array is already fast: the walker can
give that object offset-indexed arrays, and the flush code recognizes its
uniform contents. Reading through a non-inlined helper still exposes the
problem, because `ownObject()` does not find the helper's symbolic GEP among
the global's uses. The new `mem_global_zero_small.c` regression has that
shape:

```c
unsigned char bytes[1000];
unsigned char read_at(unsigned char *p, unsigned i) { return p[i]; }
/* Assume i < 1000, then assert read_at(bytes, i) == 0. */
```

Measurements start from main `4ce503b3`, with LLVM 23.1.6, camada 0.25,
RelWithDebInfo and mimalloc 3.7.0. Both binaries use the same models and
clang. Each run is pinned to one CPU with a 4 GiB address-space limit and
a 120 s wall limit. The front end compiles the fixture for the 64-bit host,
with `--unwind 1`. These are single paired observations, not medians.

| Global size | Before total wall | After total wall | Before peak RSS | After peak RSS |
| ---: | ---: | ---: | ---: | ---: |
| 1,000 bytes | 43.54 s | 0.20 s | 579 MiB | 43 MiB |
| 3,000 bytes | 0.20 s | 0.20 s | 43 MiB | 43 MiB |

The small fixture's solve falls from 43.401 s to below the displayed 1 ms
precision. Its walk falls from 3 ms to below that precision. All four runs
report SUCCESSFUL. Wall time and peak RSS include compilation; peak RSS
comes from `/usr/bin/time` and is converted from KiB to MiB.

## Change and scope

`Memory::fillObject()` now permits a fill below `MinSeparate` while the
existing `initialising` flag is set. This flag surrounds global
initialization only. All other eligibility checks stay in place: the
pointer must have a known nonzero object, start at offset zero, and cover
exactly the object's constant size. The existing `fill()` implementation
then supplies constant data and integer provenance arrays for that object.

This also lets the existing whole-object padding initialization of a small
nonzero aggregate use a zero base before its fields are written. Partial
zero fields still take the old path and cannot erase neighboring nonzero
fields. Pointer stores still replace the zero base's integer provenance.

Lowering `MinSeparate` globally would also change runtime fills and other
memory operations. It is unnecessary for this issue: the cutoff for runtime
`memset`, stack initialization, range operations and table compression is
unchanged.

## Verification

The new regressions cover symbolic helper reads and failing reads after a
write in both 32-bit and 64-bit modes. Further checks cover conditional
writes, joins, independent globals, copies, zero padding, partial zero
initializers, null pointer initialization and subsequent pointer writes.
The update test also runs with `--ub-uninit-check`.

The combined Vereda and `esbmc-cstd` suites complete in 49.12 s: 897 passed,
45 skipped, no failures (942 registered executions).

The original large-global fast path is retained, as the 3,000-byte control
measurement above confirms. The helper-read test is a performance fixture
as well as a verdict regression: benchmarking the base against the head's
fixture can expose a return of the byte-store chain.

Related issue: #297.
