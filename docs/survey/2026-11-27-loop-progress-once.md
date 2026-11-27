# Printing each loop iteration's progress line once (#383)

Recorded 2026-11-27.

Default mode printed `[PROGRESS] Unwinding loop N iteration K` for every
iteration it walked, and again for an inner loop at every outer iteration.
On a loop with a nondet bound and no `--unwind`, the walk unrolls until the
time limit and the output grows with it: `loop-acceleration/nested_1-2` (an
outer loop of 0x0fffffff iterations around an inner loop of 10, `-m32
--property-file unreach-call.prp`, 60 s) wrote 4,813,783,186 bytes on main
a6dcab95. A BenchExec run keeps all of it in its logs.

Now a loop prints an iteration's line only the first time it reaches that
iteration, and past iteration 1024 only at powers of two: the walker keeps the
deepest iteration printed per loop. The same task writes 151,698 bytes, and a
loop of 2^28 iterations prints about 1,040 lines. Every iteration up to 1024 is
still printed the first time it is reached, so the tests that check how deep a
loop was walked are unchanged. `--incremental-bmc` prints no iteration lines,
as before.

## Different from ESBMC

This is the one place where Vereda's output stops following ESBMC's line for
line. ESBMC's symex prints the line at every iteration of every visit to a
loop (`symex_goto.cpp`, `log_status("{} loop {} iteration {}   {}", ...)`),
unless `--quiet` turns all of them off. Vereda keeps the line's wording, so a
pattern that matches an ESBMC line matches Vereda's, but prints fewer of them:
a loop entered again prints nothing for the iterations it reached before, and
a long loop skips the iterations past 1024 that are not powers of two. A test
imported from ESBMC that counts these lines, or expects one past 1024, would
need adjusting; none of the current ones does.
