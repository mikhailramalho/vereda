# Arrays per object for whole-object fills

Recorded 2026-11-22.

`memset_whole_large_buffer` memsets two 16 MB heap buffers. Memory wrote
one byte per array store, so each memset became 16 million stores; the
walk passed 2.6 GB in 20 s without finishing. The 1 MB limit on byte
stores was meant to catch this but applied only to sizes the solver
works out, not to a constant size in the IR. ESBMC fixed the same test
(ee3bb61e54, #9991) with one constant array for a memset over a whole
array of scalars.

With one shared array for all objects that cannot be said: a range
inside it is no constant array, and SMT-LIB has no lambda. So, as a first
step towards the partitions of plan section 4.4, a fill that covers a
whole object of at least 1 KB (memset from offset 0 for the object's
constant size, calloc, a zeroed global) gives the object a data and a
provenance array of its own: constant arrays of the fill byte, indexed
by the whole pointer like the shared ones, so that an object separate on
one path only reads the shared arrays on the other at a join. A read at a
constant object uses its arrays; at another object, an ite over the
separate objects ends at the shared arrays; a write updates the separate
objects it may hit, and the shared arrays unless the object is constant
and separate. Known bytes flush to their object's arrays. The term
builder folds a read of a constant array to its value. Lazy range writes
in the shared array were considered and not used: every read through
stores, joins and flushes would have to see the range, and one the
walker does not build would miss it. The 1 MB limit moved into memory
and now also covers constant sizes, for fills and copies that go byte by
byte; initialisers keep no limit, as before.

| Test | Before | After |
|---|---|---|
| memset_whole_large_buffer, _fail | killed at 15 GB | 0.16 s, 73 MB, right verdict |
| multidim_padded_overflow, _fail | killed at 15 GB | 0.15 s, 70 MB, right verdict |

The array_loop_summary tests do not change: they unroll 100,000
iterations with a store each, another problem. The verdict check's 188
tasks and the spike tasks (uthash, Problem08, SpamAssassin) are
unchanged. A read past the end of a separate object now gives the fill
byte instead of an unconstrained one; that read is undefined and a
bounds check will report it, whatever its value.
