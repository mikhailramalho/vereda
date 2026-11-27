# memcpy, memmove and memset of a symbolic size

Recorded 2026-11-27.

Issue #292. A call to `llvm.memcpy`, `llvm.memmove` or `llvm.memset` (or the
library functions) whose size had more than one value on the path gave
`VERIFICATION UNKNOWN`. That blocked realloc's copy (`models/stdlib.c` copies
`min(old, n)` bytes), C23's empty initialiser of a VLA (a memset of its
size), the read-only write tests `{const_object,string_literal}_write_memcpy_symbolic_fail`
and `ntdrivers/diskperf.i.cil-{1,2}`.

## Encoding

A size with one value on the path still takes the constant paths, which did
not change; the old byte loop of `Memory::copy` was only split into
`readBytes` and `writeBytes`. Any other size `len` is extended or truncated
to pointer width, and the bytes below it change while the others keep
their value. There are two forms:

- Bounded. `rangeBound` (the walker's helper for memcmp and memchr) takes
  the bytes left in the destination's object, when the object and offset
  are constants and the solver shows `len` stays within them. Below
  `MinSeparate` (1024) bytes, byte `i` up to the bound becomes
  `ite(i < len, new, old)`. It goes through the known-byte map and
  `writeByte` as a constant-size write does, so field partitions, element
  arrays and pointers that choose between two objects need nothing new.
- Lambda. In any other case (no bound, a bound of 1024 or more, a
  destination object that is not a constant, a destination of symbolic
  size such as realloc's new object or a VLA), `Memory::writeRange`
  flushes the known bytes of every object the destination may point into.
  Then it wraps each array that holds such bytes in a `Terms::lambda`:
  each object's own arrays (`st.separate`), indexed by offset for one
  `own()` made, and the shared arrays if one of the objects has none. An
  index is in the range when its object is the destination's and its
  offset minus the destination's is below `len` (unsigned). There the byte
  is the fill byte, or the source byte at that distance. Elsewhere it is
  the array below. `misses` is the same test folded to false, so a read
  that cannot be in the range skips the lambda. A copy reads its source
  from a copy of the state taken after flushing the source objects' known
  bytes, so a memmove onto an overlapping range reads every byte before
  the write.

Objects of elements (`shifts`) or with field partitions (`fieldStorage`)
make `writeRange` give up before it changes anything. The bounded form then
runs if a bound up to `MaxBytes` exists, and the operation is unsupported
otherwise. This follows `copyArray` and `applyRange`, which refuse the same
objects.

Provenance follows `copyArray`. The provenance array becomes a lambda too
unless the source holds no pointers and the array below is all integers.
`provBelow` records which arrays the copied pointers come from. For the
shared arrays, `pointers()` gives each stored pointer to the objects of the
index it was stored at, which for a copied pointer would be the source's.
So these edges now name their destination objects (`ProvEdge::toObject`).
Without that, a call without a body that follows pointers did not see the
copied ones: `vereda/mem_memcpy_symbolic_pointers{,_own}_fail` gave a wrong
SUCCESSFUL. From a source with arrays indexed by offset, `pointers()`
would also have dereferenced an empty destination.
Initialisation tracking (`--ub-uninit-check`) uses the lambdas
`writeInitRange` and `copyInitBytes` already built past `MaxBytes`, now
taking a term as the length. The read-only write check holds where
`len != 0`.

Bounds checks of the range are #29's. A size past the object writes bytes
past its end, as a constant size does.

Not adopted: generalising `copyArray` and `applyRange` (`fillRange`) to a
symbolic length. It was the first version: an `n` as upper bound plus a
`len` term. It handled only a destination in one known object, or two
constant pointers through `copyArray`'s split. diskperf's memmove writes
through a pointer loaded from memory (`deviceExtension->PhysicalDeviceName.Buffer`),
which it could not take. One function over `candidates(dst)` covers that
case and the constant-object one, and leaves the constant paths alone.

## Tests

Fifteen imported FUTURE tests now give ESBMC's verdict with every
expectation met and are now CORE: `array_member_memset_walk_fail`,
`const_object_write_memcpy_symbolic_fail`,
`string_literal_write_memcpy_symbolic_fail`, `github_10991`,
`github_10991_fail`, `github_10991_fallback`, `github_11005`,
`github_11005_fail`, `mem_intrinsics_multibyte_array`,
`mem_intrinsics_multibyte_array_fail`, `mem_intrinsics_invalid_pointer`,
`memcpy_symbolic_length_unwind_invariant`, `realloc14`, `realloc15` and
`realloc16`. Four went from UNKNOWN to a SUCCESSFUL that ESBMC's checks
would refute, and are re-marked FUTURE with that verdict. Vereda has none of
those checks yet: `github_10991_bounds_fail` and
`memcpy_symbolic_length_oob_fail` want a length past the object reported,
`mem_intrinsics_invalid_pointer_fail` an integer address dereferenced, and
`const_object_write_memcpy_symbolic_null` a null destination. A
constant-size copy in their place gives SUCCESSFUL too.

New Vereda tests, each with a failing twin where it makes sense:
`mem_memset_symbolic` (the bounded form), `mem_memcpy_symbolic_large` (the
lambda, with known bytes on both sides of the range),
`mem_memmove_symbolic_overlap` (both forms), `mem_vla_empty_init` (`int
a[n] = {}` under `--std c23`), `lib_realloc_symbolic` (growing and
shrinking to a nondet size), `mem_const_write_memcpy_symbolic_fail`,
`mem_memcpy_symbolic_uninit` (`--ub-uninit-check`) and
`mem_memcpy_symbolic_pointers{,_own}_fail` (copied pointers reached by a
call without a body). The new tests also give their verdicts with `-m32`.

## Cost

`tests/bench.py --list` on the 502 regression tests whose sources mention
memcpy, memmove, memset, realloc or `= {}`, base 29759530 against head
side by side (cores 50-59 of the shared machine): no time or memory
regression. The tests that changed are the 33 that gave UNKNOWN before
and now finish in 0.13 to 0.19 s and under 45 MB. The total wall time was
73.1 s on base and 71.5 s on head.

The 52 sv-benchmarks unreach-call tasks whose source calls one of the four
functions, run with `--incremental-bmc` and a 60 s limit as
`contrib/benchexec/vereda.xml` runs them (5 at a time):

| | base | head |
|---|---|---|
| answered (all right) | 32 | 34 |
| UNKNOWN | 2 | 0 |
| timed out | 18 | 18 |
| time of the 32 both answer | 112.8 s | 113.3 s |

The two new answers are `ntdrivers/diskperf.i.cil-1` (TRUE, 17.7 s,
1.06 GB) and `ntdrivers/diskperf.i.cil-2` (FALSE, 3.9 s, 324 MB). Both
were UNKNOWN in under 1.5 s before. Without `--incremental-bmc`, `-1` takes
25.3 s and 1.26 GB. Every other task gave the same verdict in the same time
within 0.3 s. The timeouts (heap-data, `cdaudio`, `parport.i.cil-2` and
some list tasks) time out on both.
