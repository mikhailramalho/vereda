# Library models

Recorded 2026-11-23.

Rerunning the imported tests Vereda failed and collecting the bodyless
functions they call, by tests with a wrong verdict: pthread_create (120,
threads are out of scope), an undeclared assert (106), printf (40),
ESBMC's own intrinsics (__ESBMC_forall and others), sscanf, realloc
(18), strlen, strcmp, sprintf (14 each), the ctype tables, errno, time,
memcmp, bcmp, fmod and more.

An assert without <assert.h> is an implicitly declared function; ESBMC's
front end treats it as an assertion, and so does the walker now (with
the caller's line, which a C model would lose). The rest are C models in
models/, written from the C standard rather than ported from ESBMC: its
library models come from CBMC's, under the BSD 4-clause licence, and
Vereda is Apache 2.0. string.c has the <string.h> functions (memcpy,
memmove and memset stay with the walker), stdlib.c realloc (with
__vereda_object_size, the bytes to the object's end, from the walker),
libc.c glibc's ctype tables (the C locale, class bits byte-swapped as
_ISbit has them), the isX functions, tolower, toupper, __errno_location
and time, math.c isnan, isinf, isfinite and signbit as functions.

CMake builds them with the pinned clang (-fno-builtin, so nothing inside
is rewritten into other calls), runs Vereda's pass list over them with
opt, and links models-32.bc and models-64.bc. ir links the one matching
the module's pointer width after its own passes, with LinkOnlyNeeded, so
a program's own definitions win and a call InstCombine created (bcmp from
memcmp == 0) still finds its model. Two first tries did not hold:
linking in vereda.py before the passes left such calls bodyless, and
turning off the passes' knowledge of library functions to avoid them
also lost InstCombine's folding of cos, exp, pow, strtol and ffsl with
constant arguments, which 11 imported tests rely on.

Loops in the models run only as far as their result needs, as glibc's
do: strncpy pads with memset, strrchr hops between matches with strchr,
memcmp returns from inside the loop. The imported tests' --unwind bounds
are tuned to ESBMC's models, and every iteration costs the walk.

121 imported tests now pass. 16 that passed only because their
undeclared assert checked nothing now fail: they call functions without
a model (strstr, strspn, strcspn, strncat, strerror, ESBMC intrinsics) or
read uninitialised locals, which Vereda takes as a new nondet value at
each use. The verdict check's 188 tasks are unchanged. A read of the
ctype table at a nondet index costs the solver about 2 s.
