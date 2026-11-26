// Pins the ranking certifier's ability to analyse two sequential
// loops in the same function. Each is independently bounded by a
// counter; the second appears in the dominator-path prefix of any
// loop that follows, so `scan_prefix_defs` must walk past the first
// loop's back-edge rather than bailing on it.
//
// The fix records every loop's modified set + exit-target location
// number in a `loop_skipt` map. When the prefix walker hits a known
// loop head it (a) drops defs/atoms tied to that loop's modified
// symbols and (b) treats the back-edge's successor as a justified
// merge point, then jumps the iterator to just past the back-edge.
#include <stdlib.h>
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int a = __VERIFIER_nondet_int();
  if (a < 0)
    a = 0;
  while (a > 0)
    a = a - 1;

  int b = __VERIFIER_nondet_int();
  if (b < 0)
    b = 0;
  while (b > 0)
    b = b - 1;

  return 0;
}
// Imported from ESBMC regression/termination/ranking_sequential_loops
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Ranking function shows all executions terminate$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
