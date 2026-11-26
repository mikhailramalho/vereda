# Preserve symbolic guards after predicate assertions

Review of #277 found that narrowing symbolic assertion continuations defeats
#175's unconditional-prefix recognition. Loop header guards cease to be true,
disabling bound doubling and prefix reuse, and accumulated assertion predicates
can also trigger repeated loop feasibility queries. Changing the expected bound
from 4 to 3 in incremental_bmc_trip_count_false hid that regression.

Restrict single-property pruning to predicates that already fold to false.
Record the failed check and terminate that path without learning symbolic
assertion predicates or narrowing their guards. Multi-property runs continue
past predicate assertions, including constant-false ones. Noreturn handlers
still terminate in either mode. The unchanged-guard loop deferral remains capped
at 64 iterations, capturing its guard at the starting iteration.

Restore the existing incremental regression's bound-4 expectation. It checks
the 1, 2, 4 schedule on a loop containing a symbolic assertion, directly covering
the interaction that review exposed. Constant-false pruning still applies to
Fixedbv11 and the promoted bounded-buffer fixture.

The revised build passes all 531 selected CTest cases except the existing
lib_sprintf_overflow known-bug skip: 530 pass, one skips, no failures. This covers
the entire Vereda suite, the restored bound-4 regression and both affected
imported fixtures. Fixedbv11 takes 0.36 seconds and 00_bbuf_01 takes 0.30 seconds
in this run. The full imported suite is not rerun after this correction.
