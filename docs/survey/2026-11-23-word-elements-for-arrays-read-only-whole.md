# Word elements for arrays read and written only whole

Recorded 2026-11-23.

Issue #107, the narrow form of #72's word cells that the survey entry on
#104 sized. An object that already gets arrays of its own (#71, #88) and
whose every use, through the getelementptr tree from it, is a load or
store of one iN (N = 16, 32 or 64) at an offset that is a multiple of
N/8, a comparison, or the no-op `__vereda_uninitialised` gets a data
array of N-bit elements indexed by offset / (N/8), and no provenance
array. Any call taking the object, memset and memcpy among them, keeps it
on bytes, as does a mixed width, a char access, or an index whose scale
is not a multiple of N/8 (`gep i8, p, (shl i, 2)` too). The initialiser
needs no check: it goes through the known-byte map like any store at a
constant address.

The known-byte map stays byte-wise. A flush moves an element as one store
of its bytes, reading the bytes not known from the array. A load or store
of iN at a symbolic offset into the object is one select or store. An
access at a constant address reads the element and extracts bytes, which
the term builder joins back into the element. An access through a
pointer whose object is not constant, which may still hit the object
(a pointer made from an integer), extracts a byte or reads, masks and
writes the element back; a pointer stored that way loses its provenance
and reads back as one made from an integer.

Which objects qualify, counted over the 6506 imported tests (one compile
each, default flags): 826 objects get arrays of their own, 403 of them
elements (356 of i32, 32 of i64, 15 of i16), in 229 tests. Of the 42
array_loop_summary tests, 26 have one; most of those are FUTURE tests
that need `--summarize-array-loops`, so only the ones below run.

Measured at 657fd019 (after #104 set `bv-cnf-scope=permanent`) against
the same plus this change, on an 80-core machine shared with another
job (load 10 to 35). Side by side with `tests/bench.py --list`:

| Test | Walk | Solve | Wall | Peak |
|---|---|---|---|---|
| decrement_fail | 9.5 → 7.5 s | 15.5 → 5.0 s | 33.7 → 18.3 s | 2177 → 1056 MB |
| counter_fail | 9.9 → 8.1 s | 3.5 → 1.2 s | 21.2 → 15.1 s | 1103 → 647 MB |
| update | 0.02 → 0.02 s | 0.69 → 0.17 s | 0.92 → 0.35 s | 108 → 86 MB |
| update_fail | 0.01 → 0.01 s | 0.14 → 0.04 s | 0.33 → 0.20 s | 90 → 82 MB |

The decrement test's formula has 100000 stores of 32-bit elements at a
62-bit index and one select, against 400000 byte stores and four selects.
Its solve drops about 3 times, as #104 measured on the hand-built
formulas, and its peak halves.

The proofs of the family (decrement, counter, store, stride and their
SUCCESSFUL siblings) still run out of 4 GB or time out on both, so word
elements do not change their verdict; stride and stride_fail reach the
memory limit in 77 and 97 s instead of 171 and 203 s, which says only
that the solver gets further sooner. `store` and `store_fail` get no
arrays of their own at all, here as on main, so nothing changes for them.

Whole suite, 6505 imported tests with both binaries, 10 at a time each,
under the 4 GB limit: no verdict changed. Over the 6498 tests that end
within their limits on both, wall 1623 → 1601 s, walk 298 → 294 s,
solve 176 → 158 s. No test is 1.5 times and 2 s slower, or 1.5 times and
200 MB larger, on head; only decrement_fail, stride and stride_fail
changed by that much, all for the better.

This still leaves the decrement test at 5 s of solve where ESBMC takes
about 2 s by luck (#104); loop summaries (#36) remain the fix for this
family.
