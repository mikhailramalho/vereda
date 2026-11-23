# Pointers made from integers, stored and read back

Recorded 2026-11-23.

Issue #75: `(int *)0x1234` stored in memory and read back did not equal the
same integer converted later, once an object had been placed at that
address. A pointer converted from an integer that no object covers is
(object 0, offset a), and a load rebuilt it as (0, a - base(0)), never as
the object allocated there since. Four fixes were tried.

Keeping every later allocation off each address already converted to a
pointer fixes the test but gives a wrong TRUE on
esbmc/github_7855-reread-later-object: uninitialised heap bytes read before
a `malloc` may hold the address it then returns. Dropped.

Comparing pointers for equality by address, base + offset, rather than by
(object, offset) also fixes it, but turns the CORE test
esbmc/github_8517_assume_pins_pointer into a wrong FAILED: an `undef`
pointer pinned by `assume(d == &the_desc)` may then belong to another
object, or none, at the same address, so reads through it do not read
`the_desc`. Restricting it to comparisons with an object-less side fails
the same way. Dropped; C11 6.5.9p6 lets pointers to different objects
compare equal only one past the end of one at the start of the next, which
the gap between objects rules out, so (object, offset) equality stays.

Converting at the load every pointer read back with object 0 through
`fromAddress`, as integer bytes already were, fixes it with no verdict
change, but a stored null has object 0 too: each load of a `next` field
became a choice over every live object, and esbmc-nonz3/01_cbmc_Malloc18
went from 0.01 s to 3.5 s of solving (76 MB to 259 MB). Guarding the
conversion by a nonzero address does not help (8 s): the address is
base + offset with a symbolic base, so the guard does not fold on either
side of `ite(ok, p, null)` from a `malloc` that may fail.

Kept: `store` gives a pointer with object 0 and a nonzero offset the
provenance of an integer, and `load` converts only integer bytes, as before.
The test is on the offset, which is constant on each side of
`ite(ok, p, null)`, so for every stored null and every pointer to an
object it folds and the stored provenance is the object's, as on main.
Malloc18 solves in 0.015 s again. Over the whole suite (6644 tests) no
verdict changes and `tests/bench.py --stats` flags no test against main;
it flagged esbmc/github_2312_5 (1.6 s to 6.9 s with z3), but that follows
the length of the source path that `assert` embeds, not the binary: both
binaries take 1.6 s on one copy of the file and 6.9 s on the other.

One gap is left, recorded as the KNOWNBUG test vereda/mem_ptr_eq_address: a
pointer made from an integer and kept in a register still compares unequal
to an object later placed at its address. Real allocators do not hand out
such addresses.
