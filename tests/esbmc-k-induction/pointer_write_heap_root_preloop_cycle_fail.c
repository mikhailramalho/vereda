// As pointer_write_heap_root_preloop_alloc_fail, with the trace reaching g a
// second time through v, which g's result was assigned before the loop.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
long g(long a)
{
  long x;
  if (a)
    x = a;
  else
    x = (long)malloc(sizeof(int));
  return x;
}
int *h(long a) { return (int *)g(a); }
int main()
{
  long v;
  v = g(0);
  *(int *)v = 0;
  while (__VERIFIER_nondet_int())
  {
    int *r;
    r = h(v);
    *r = *r + 1;
    assert(*(int *)v < 3);
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_preloop_cycle_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
