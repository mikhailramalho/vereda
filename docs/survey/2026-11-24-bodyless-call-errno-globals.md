# errno and the library's globals at a call without a body

Recorded 2026-11-24.

A call without a body wrote the objects its pointer arguments point to (see
2026-11-23-bodyless-call-writes.md) but left errno and the globals of the C
library alone, so `errno = 0; strtol(s, 0, 10); assert(errno == 0)` was
proved, and `getopt` kept `optind` (issue #113). Such a call now also gives
nondet contents to errno, when LLVM's memory attributes say it may write
errno's memory, and to every writable global the program declares without
defining, when they say it may write memory other than its arguments'. A call
that only reads memory (atoi), or only writes its arguments (frexp), keeps
both. errno is now the named global `__vereda_errno` of `models/libc.c`, zero
at program start (C17 7.5p3), so the walker finds it by name; it exists only
when the program uses errno.

Every declared global rather than a list of the C library's (optind, optarg,
opterr, optopt, environ, timezone, tzname, daylight, ...): a global the
program declares but does not define belongs to code Vereda does not see, the
C library or a file not given, and the call without a body may be that code. A
list would miss the ones it does not name (h_errno, `__environ`, a variable of
the program's own other file) and would need upkeep; its contents are unknown
at the start anyway. The exception is the models' `__vereda_stdin`,
`__vereda_stdout` and `__vereda_stderr`: no program reads a FILE's fields, and
a call that gets the stream writes its object already. Writing them at every
call cost 05_pfscan-1.0_01 about 9 s of 50 for nothing.

Measured against main (6c600af6) on the 196 imported tests whose sources
mention errno, getopt or the other library globals, the standard streams,
fopen or an extern variable (a call without a body can change only these
tests' verdicts). Three verdicts change, all SUCCESSFUL to FAILED and all
right, so the three FUTURE tests become CORE:

- esbmc/github_8703_fail: strtol of an out-of-range number sets errno to
  ERANGE.
- esbmc/github_9435_fail: sem_trywait on a zero semaphore fails with EAGAIN.
- esbmc/github_9435_timedwait_fail: sem_timedwait with a past deadline fails
  with ETIMEDOUT.

No verdict turns wrong. The only slower test is 05_pfscan-1.0_01
(--incremental-bmc, KNOWNBUG, UNKNOWN at calloc with a symbolic size either
way): about 25 s on main and 38 to 41 s now, all of it in the one feasibility
query of calloc's size, whose formula grows by errno's nondet contents after
each call on the path (strerror(errno) after open, fstat and mmap). The rest
stay within a second of main.
