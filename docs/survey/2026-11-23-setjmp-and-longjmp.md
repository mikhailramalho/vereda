# setjmp and longjmp

Recorded 2026-11-23.

How #11 models setjmp and longjmp, what else was looked at, and what it
gives on the sv-benchmarks `longjmp/` tasks.

## Choice

CBMC has no model to reuse: its library's setjmp returns 0 and its longjmp
fails an assertion ("longjmp requires instrumentation") and then assumes
false, so any program that reaches a longjmp is reported as failing.

ESBMC lowers the jump to plain control flow (`goto-programs/lower_longjmp.cpp`
with the globals of `c2goto/library/setjmp.c`): setjmp stores a fresh token
in the jmp_buf and in a slot of its frame; longjmp arms a pending flag, the
token and the value; after each call a pending jump branches to the
function's dispatch, which goes back to the setjmp whose slot holds the
token, or returns for the caller to dispatch in turn; a jump still pending
when main ends is checked as a longjmp to a setjmp no longer active. Vereda
does the same in `ir` (`lowerLongjmp` in `src/ir/load.cpp`), before
mem2reg, so the walker sees ordinary loops and needs no new state. The
issue's other idea, a walker that joins the longjmp's state into the
setjmp's continuation and unwinds frames itself, would have put a second
kind of back edge into the walker for the same result.

Three things differ from ESBMC:

- A token names a setjmp's frame and call site, not one run of it: a frame
  takes it the first time it runs that setjmp and reuses it after. A
  jmp_buf filled by an earlier run of the same setjmp in a live frame stays
  valid (C11 7.13.2.1p2 only ends it with the frame), and ESBMC, whose slot
  keeps only the last token, reports such a jump as unmatched.
- A function's jumps go back to their setjmps directly only when each
  resume block dominates every jump. Else they all go through a restart
  block after the entry block's allocas, which switches to the resume
  block, so a setjmp under an `if` still gives a loop with one way in.
  Mixing the two per jump was tried and dropped: with two setjmps, restart
  entered the direct jump's loop past its head (`siglongjmp_frames_fail`).
  What stays irreducible (restart entering a loop at a resume block) the
  walker already refuses, giving UNKNOWN. A function where the way through
  restart skips a value the code after the setjmp uses (a VLA's address)
  fails the IR verifier; its body becomes a call that gives UNKNOWN when
  reached.
- The unmatched check is the walker's: `__vereda_longjmp_unmatched` is a
  check in the outermost frame and a no-op in any other, so a main called
  again by the program needs no special case.

Locals that are not volatile get the value they had at the longjmp, which
is what mem2reg gives and what a program compiled at -O0 sees. C11
7.13.2.1p3 makes those changed since the setjmp indeterminate; sv-benchmarks
expect the -O0 value (`42-poison-reduced_true`), as ESBMC gives.

InstCombine turns the lowering's `v ? v : 1` into `llvm.umax`, so the
encoder now encodes umin, umax, smin and smax.

## Measured

The 34 `longjmp/` tasks of the ReachSafety baseline, all UNKNOWN on
f53bf26d, with `--incremental-bmc` and 60 s each: 32 correct (14 TRUE, 18
FALSE), none wrong, and 2 timeouts (`68-longjmp_19-simpler_true` and
`68-longjmp_20-simpler-multifun_true`, whose longjmp goes back to its
setjmp forever; ESBMC times out on both as well).
