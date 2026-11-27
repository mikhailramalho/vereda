// m holds the address of an object main allocated before f's loop, so the
// write through it is no object the iteration allocated. Unless the inductive
// step havocs that object, *p keeps its pre-loop 0 and the step proves
// *(int *)m < 3.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
void f(long m)
{
  int *r;
  while (__VERIFIER_nondet_int())
  {
    r = (int *)m;
    *r = *r + 1;
    assert(*(int *)m < 3);
  }
}
int main()
{
  int *p = malloc(sizeof(int));
  *p = 0;
  f((long)p);
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_preloop_alloc_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
