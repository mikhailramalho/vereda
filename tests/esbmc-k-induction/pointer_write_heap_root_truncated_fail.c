// lo holds p's address truncated to an int, and symex still finds p's object
// from it. Unless the inductive step havocs that object, *p keeps its
// pre-loop 0 and the step proves *p < 3.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
int main()
{
  int *p = malloc(sizeof(int));
  *p = 0;
  int *r;
  while (__VERIFIER_nondet_int())
  {
    int lo = (int)(long)p;
    r = (int *)(long)lo;
    *r = *r + 1;
    assert(*p < 3);
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_truncated_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
