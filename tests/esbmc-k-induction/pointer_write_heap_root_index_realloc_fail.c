// realloc copies B's address, stored as an integer, into d's new object, so
// what the loop loads from it reaches B. Unless the inductive step havocs B's
// object, B[1] keeps its pre-loop 0 and the step proves B[1] < 3.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
int main()
{
  int *B = malloc(2 * sizeof(int));
  B[0] = 0;
  B[1] = 0;
  long *d0 = malloc(sizeof(long));
  *d0 = (long)B;
  long *d = realloc(d0, sizeof(long));
  while (__VERIFIER_nondet_int())
  {
    int *r = (int *)(*d + 4);
    *r = *r + 1;
    assert(B[1] < 3);
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_index_realloc_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
