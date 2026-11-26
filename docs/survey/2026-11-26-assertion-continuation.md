# Assertion continuation and loop feasibility queries

Predicate assertions previously recorded a violation and continued on the full
guard, even in single-property mode. For an unconditional assert(0), this walks
code that no execution surviving that assertion reaches. In single-property
mode, record the violation first, then learn the predicate in path-local facts
and continue under guard AND predicate. Multi-property mode retains the full
guard so later claims are independently checked. Disabled assertions still
continue normally. Existing noreturn failure handlers terminate in both modes.

Pass the multi-property option through both the initial walk and fresh solver
rewalks. Avoid adding global solver assumptions: narrowing must not erase the
violation just recorded or earlier failures.

For loops without an explicit unwind limit, capture the initial header guard.
If the next iteration has the identical guard, skip redundant feasibility
queries for at most 64 iterations. This also covers helper loops with constant
arguments that SCEV cannot count. Beyond that cap, query normally so an
unreachable long loop cannot unroll forever. Existing SCEV bounds and explicit
unwind checks retain their behavior.

With LLVM 23.1.6, the Fixedbv11 fixture passes in 0.35 seconds wall time.
A direct run with its checkOverflow entry and 32-bit target reports 0.001 seconds
walking, 0.094 seconds solving and 44 MB peak solver memory. The issue reports
161.5 seconds before this change; this is not a controlled same-machine
base/head comparison.

Regressions cover unconditional predicate pruning, multi-property continuation
and an unreachable long loop whose guard requires a solver query. The existing
incremental trip-count failure test now detects its third-iteration violation
at bound 3 rather than 4: the assertion continuation guard prevents resuming an
unconditional prefix, so that test follows the ordinary bound schedule.

Validation covers all 529 Vereda cases and 6,140 imported cases with the normal
300-second timeout. The Vereda run needs only the bound-expectation update above,
which passes on rerun. The imported run identifies one newly passing FUTURE test,
esbmc-unix/00_bbuf_01: its initial assert(0) now reports FAILED before unsupported
thread creation. Promote it to CORE; its rerun passes. After those metadata
updates, the runs account for 3,118 passing cases and 3,551 skips, with no
outstanding failures. The imported run takes 647.81 seconds locally.
