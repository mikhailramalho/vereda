# False alarms in --ub-uninit-check and --ub-alignment-check

## Uninitialised reads that only copy

`--ub-uninit-check` checked every load. Clang copies a struct by loading it
as integers: passing one by value (`load i64` for the coerced argument, which
the callee stores to its own copy), returning one (`load i64` from `%retval`,
stored by the caller) and assigning one (a small `memcpy` that InstCombine
turns into `load i64`/`store i64`). A struct with a member not yet set, or
with padding, then failed at the copy, although copying it is not a read of
an indeterminate value (C17 6.2.6.1p6: a struct value is never a trap
representation). ESBMC's `--uninitialised-vars-check` and MSan accept these.

A load, argument or call result is now "only copied" when all its uses are
stores of it, arguments of defined functions whose parameter is itself only
copied, or returns from a function whose every call result is only copied
(`Walk::copyOnly`, decided once per value). Such a load does not check;
it reads each byte's initialisation (`Memory::initBytes`), the walker carries
those bytes through arguments and returns as `Walk::shadows`, and the store
writes them back (`Memory::setInitBytes`). The next load that is not a copy
checks them. Every other load checks as before, including one whose value is
unused, and returning from `main`, which has no caller.

Not adopted: full value shadows as MSan has (checking at branches, addresses
and calls, propagating through arithmetic). That would also accept reading
padding through `unsigned char` and summing it with `& 0`, but needs a shadow
for every value; a scalar copied into a variable kept in memory now reports
at its next use instead of at the copy, which is the one behaviour change
for scalars.

## Alignment of a pointer chosen between objects

`__vereda_check_alignment` tested `(address(p) & (align - 1)) == 0` unless
`p` named one object. For `c ? h : &g` with `h` from `malloc` (itself
`ite(ok, h, null)`), the term builder nests the choice so that `object(p)`
is an extract of the whole `ite`, and `base()` fell back to the `bases`
array, which only constrains exposed objects: the solver picked a misaligned
base for `g`. `Memory::aligned` now splits a choice of pointers and decides
each object on its own, by offset when the object's alignment covers the
access and by its real base otherwise.

Known gap, unchanged: clang lowers some misaligned accesses to `memcpy` with
`align 1` (ESBMC's github_10891), which the check never sees.
