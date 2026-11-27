# Layout of objects allocated on one path only

Recorded 2026-11-26.

`2026-11-25-picosat-layout-and-null-compare.md` (#233) laid out every
constant-size object unconditionally while all such objects so far fit in a
quarter of the address space, and #241 then deferred their layout until their
base is observed. The note argued that this "cannot remove valid inputs on any
path" because three quarters of the address space, more than `PTRDIFF_MAX`,
stay free. That argument holds for one object per path, not for several: malloc
accepts sizes up to `PTRDIFF_MAX`, and one path may allocate two objects that
together need more than three quarters of the address space. An object
allocated on another path then still takes its space on this path, the layout
becomes unsatisfiable, and the path's errors vanish.

`tests/vereda/mem_alloc_one_path_layout_fail.c` allocates `3 * 2^60` bytes on
one path and observes the address; the other path allocates `2^63 - 1` and
`1.25 * 2^62` bytes and calls `reach_error`. Main (`3ddfb3eb`) said
SUCCESSFUL, the commit before #233 said FAILED. With `-m32`
(`mem_alloc_one_path_layout32_fail.c`) it happens at realistic sizes: 0.8 GB
on one path, 2 GB and 1.35 GB on the other. If the first path does not observe
the address, the object is never laid out and the verdict was already right.

## Decision

`Memory::allocate` now lays out unconditionally, or defers, only objects whose
`exists` guard is true, that is, objects present on every path; their space is
taken on every path anyway. A conditional object always binds its layout under
`implies(exists, layout)` and moves `end` with `ite(exists, e, end)`, as before
#233. Conditional constant-size objects still add to `reserved`, so it stays an
upper bound on `end` and the argument for deferring the others still holds.

Not adopted: keeping the unconditional rule for conditional objects under a
smaller budget. No budget is safe, since the space a later path needs is not
known when an earlier object is allocated and the reservation cannot be
undone.

## Measurements

`tests/bench.py` side by side against `3ddfb3eb`, on the tests #233 cited and
the ones it added. The machine was shared with other jobs, so the times are
rough:

| test | wall s | peak MB | verdict |
|---|---|---|---|
| esbmc-unix2/23_picosat-846_01 | 0.71 → 0.82 | 80 → 102 | FAILED |
| the other 15 `mem_*` tests | 0.12 to 0.44, unchanged | +2 to +5 | unchanged |

Two more runs of picosat gave the same: about 0.1 s and 22 MB more with the
fix, below `bench.py`'s regression margins. Later changes made picosat fast
without the unconditional layout of conditional objects (it took 10 s right
after #233), so dropping it costs little there. `ctest -L vereda`, the `esbmc-unix*` suites and the esbmc tests
matching malloc, alloc, pointer, address, ptr or mem all pass.
