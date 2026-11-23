# Function pointers

Recorded 2026-11-22.

Measured first: rerunning the 1,671 imported ESBMC tests Vereda failed,
the unsupported constructs by tests blocked were floating point (about
190), vector constants and operations (about 110, mostly GCC vector
extension tests), function pointers (48), variadic functions (25) and
atomics (20).

CBMC lowers function pointers in a GOTO pass that always runs
(remove_function_pointers): each call through a pointer becomes `if (fp ==
&f) f(args);` per address-taken function with a body whose type is
compatible (same arity; integers, enums, bools and pointers interchange;
other types by width; the return only if used), arguments and result
converted by byte extraction, and a final `assert(false)` ("no candidates
for dereferenced function pointer") plus `assume(false)`. A const pointer
resolved to one function becomes a direct call. ESBMC instead forks in
symbolic execution over the pointer's value set, filters with a copy of
CBMC's rule (keeping all targets when the filter would remove every one),
and by default (open world) skips a call with no target and leaves its
result nondet; `--closed-world-fnptr` ends that path.

Vereda follows CBMC's shape in `ir` (lowerCalls, before InstCombine) with
ESBMC's open world: the fall-through calls a bodyless declaration, so the
result is unconstrained and the path goes on. Every function whose address
is taken gets a one-byte object, so a function pointer is an ordinary
pointer. The walker folds the tests, so the precision matches ESBMC's value
sets without an analysis: for `p = c ? &f : &g`, `p == &h` folds to false
for every other h, and InstCombine folds `(c ? f : g) == f` back into c.
A direct call to a defined function of another type (across files, or a
definition without a prototype) is converted the same way; the walker used
to reject it. Such a call does not count as taking the function's address
(hasAddressTaken's IgnoreCastedDirectCall): with the two undeclared
__VERIFIER_ calls of array_loop_summary_decrement_fail counted, their two
extra objects took that test from 66 s to over 120 s.

Tried and not kept: dispatching in the walker (works, but the lowered form
reuses every direct-call path and shows in `--dump-ir-only`); exact LLVM
types only (misses a definition without a prototype called through a typed
pointer, and a call through a pointer without a prototype, which clang
emits with type `(...)`); ending the path when nothing matches (a wrong
SUCCESSFUL for `extern int (*cb)(int)`, whose target lives in code not
given).

Of the 48 tests, 46 now reach a verdict and 29 imported tests pass that did
not; the verdict check's 188 tasks give one new correct FAILED
(pals_floodmax BOUNDED-6, UNKNOWN before) and no other change. Still open:
a call to an unknown function changes no memory, as for every bodyless
function; ESBMC's own `__ESBMC_assume` and an undeclared `assert` are not
modelled; a `byval` argument is not copied, so a callee that writes its
by-value struct parameter changes the caller's.
