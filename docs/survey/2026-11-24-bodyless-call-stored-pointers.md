# What a call without a body writes through stored pointers

Recorded 2026-11-24.

Issue #114. Since #111 a call without a body gives nondet contents to each
object its pointer arguments point to, but not to the objects reached
through pointers stored inside those, so `struct S { int *p; } s = { &x };
ext(&s); assert(x == 0);` was proved, and so were a write through `readv`'s
iovec and a `getline` into a buffer the program kept an alias of.

## The rule

When LLVM's memory effects say the call may write memory other than its
arguments' (the `Other` location: any unknown function, and every C library
function `inferattrs` gives no memory attribute, which is nearly all of
them), it also writes, transitively, each object a pointer stored in an
object it writes may point to: its arguments' objects, the globals the
program declares without defining, and the memory outside the program. LLVM
itself counts memory reached through a loaded pointer as `Other`, so a
function it says writes only its arguments (frexp) follows nothing.

The pointers stored in an object come from the memory state at the call,
with no analysis of the program: the provenance of its known bytes and the
stores in the provenance arrays that hold its bytes (in the shared arrays,
the stores whose index may be in the object; in its own, all of them; a
summarised loop stores only integers, so its array holds the pointers of the
one below). One walk per call. A stored pointer whose object was itself read
from memory may point to any exposed object, as #154 has it. A pointer made
from an integer is not followed: an address converted to a pointer, an
uninitialised pointer, or bytes that held an integer. Without that, an
uninitialised `struct tm` handed to `localtime_r` reached every exposed
object through its `tm_zone` field (12 string constants in
`time_h_localtime_r_ranges`). A constant global or a function is never
written. Where the argument may point into several objects, an object
reached from only some of them is written only where the argument points
into one of those.

After the call, each object it wrote may hold a pointer to any object
reached: their provenance is a nondet choice among those and an integer,
so a second call on the same struct writes `x` again, and a pointer reached
through one argument may have been stored into another.

Sound because the code behind the call can only write memory it has a
pointer to: one it was given, one stored in memory it can reach, the
library's own, or one it kept from an earlier call. Not covered: the last
(a buffer `setvbuf` was given, a context registered earlier), arguments LLVM
marks read-only (only `inferattrs` does that for a declaration, on C library
functions that only read them), and integers that hold an exposed object's
address. The cost is precision where a C library function does not follow
what it finds: `qsort` on an array of pointers now also writes the objects
they point to, and `strtol` what a previous end pointer points to.

UNKNOWN instead, as the issue suggested, for any object that may hold a
pointer (with an allow-list of functions known not to follow them), would
have turned the 7 decided tests below into UNKNOWN; the havoc decides them
as before. The refusal is left only for a provenance array the walk does not
know, which never happened.

## Models

`getline`, `getdelim` and `readv` now have models in `models/stdio.c`, so
the common cases do not depend on the havoc: getline writes a NUL after
the line and keeps the buffer when it holds more than the line, otherwise
it allocates a new one, and readv writes each buffer of the iovec array.
The characters are not modelled: the object they go to gets nondet
contents through `__vereda_havoc`, a builtin for the models, as a buffer
of symbolic length cannot be written byte by byte. Both may change errno.

## Measured

Every imported test (6,425 that run, 60 s each) with logging: 468 reach a
call without a body that may write other memory, and 11 of them reach an
object through a stored pointer (5 FAILED, 2 SUCCESSFUL, 4 UNKNOWN for
other reasons): 31_cdaudio (52 objects), 27_exStbFb (33), 10_bicycle_01
(20), 05_pfscan-1.0_01 (30), github_1070, github_1353, github_8686,
github_7548_strsep_c, github_5868_strto, 25_git-remote-gitkrb5 and
mem_external_optind_false. Against the same binary with the rule off, no
verdict among the 468 changes and they take 120 s in all against 124 s;
the slowest change is 10_bicycle_01, 0.5 s to 0.9 s (UNKNOWN at
pthread_create either way).

A first version computed the reached objects per object a pointer read
from memory may point into, walking the provenance arrays each time:
10_bicycle_01 took 46 s, 34_llvm_01 16 s instead of 4 s and 31_cdaudio 9 s
instead of 0.9 s. With one walk per call and a condition per reached object
they take 0.9 s, 4.1 s and 0.9 s. That version also followed pointers made
from integers and reached objects in 27 tests.

Through `tests/run.py`, the 428 imported tests that reach such a call or
call getline, getdelim or readv, and the vereda suite: two FUTURE tests now
give the expected SUCCESSFUL, from the getline model, and are CORE
(github_7548_getline_c and github_7548_getline_reuse); none got worse.
github_7548_getline_stale_fail and github_7548_getline_eof_fail still give
FAILED.

ReachSafety (Arrays, ControlFlow, Heap, Loops; 1,407 unreach-call tasks):
13 call a function without a body other than the verifier's, malloc, free
and the memory functions (printf in 7, swprintf in 4, sscanf, read), none
reaches an object through a stored pointer, and all 13 give the verdict
they gave before (60 s, `--incremental-bmc`). The LDV tasks are not on this
machine and were not run.
