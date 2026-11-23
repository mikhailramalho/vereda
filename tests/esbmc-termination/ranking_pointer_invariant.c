/* Loop-invariant pointer dereference for the ranking checker.
 *
 * The pointer `p` is loop-invariant -- it is assigned exactly once in the
 * function prefix (here from malloc) and never written inside the loop --
 * so every iteration's `*p` denotes the same memory cell. The ranking
 * checker substitutes `*p` with a fresh scalar symbol and proves
 * termination by the existing scalar pipeline: the guard `*p >= 0` and
 * the body `*p = *p - 1` become a standard counter loop.
 *
 * Soundness rests on three structural checks in recognize_loop:
 *   (1) `p` is not on the lhs of any body assignment (invariant pointer);
 *   (2) the prefix is a single dominator path (so the malloc reaches
 *       every entry to the loop head);
 *   (3) `p` has distinct-allocation provenance (here: malloc) with no
 *       aliasing pointer also dereferenced in the loop.
 *
 * Expected verdict: VERIFICATION SUCCESSFUL. */

#include <stdlib.h>

extern int __VERIFIER_nondet_int(void);

int main()
{
  int *p = malloc(sizeof(int));
  *p = __VERIFIER_nondet_int();
  while (*p >= 0)
    (*p)--;
  free(p);
  return 0;
}
// Imported from ESBMC regression/termination/ranking_pointer_invariant
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Ranking function shows all executions terminate
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
