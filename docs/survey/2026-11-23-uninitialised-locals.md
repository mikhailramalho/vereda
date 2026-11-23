# Uninitialised locals

Recorded 2026-11-23.

mem2reg turns each read of an uninitialised local into undef, which the
encoder makes a fresh value per use, so `int y; a[0] = y;
assert(a[0] == y + 1)` failed; ESBMC and plan section 4.5 take such a
local as one nondet value. Before mem2reg, each static local's scalar
leaves (integers, pointers, half/float/double/fp128, struct fields; not
arrays, which stay in memory and read the same nondet bytes, nor long
double or bfloat, which locals only move as bits) are stored the result
of a bodyless `__VERIFIER_nondet_uninitialised.<type>` call, one fresh
value per call. freeze poison was the first idea, but InstCombine may
replace it by any constant. The debug record mem2reg makes for that store
shows the start value at the declaration, as ESBMC does when the local is
read while uninitialised; when it is written first, the record is removed
(corrected the same day: at first every such record was).

With the locals no longer undef, two intrinsics InstCombine used to fold
stay: llvm.objectsize and llvm.is.constant, now lowered as the back end
does (unknown: -1 or 0, and false). 40 imported tests pass; 8 that passed
by luck now fail for reasons Vereda does not model yet (pthread_exit
ending the path, __ESBMC_forall, __ESBMC_r_ok, ESBMC's alignment check).
The verdict check's 188 tasks are unchanged.
