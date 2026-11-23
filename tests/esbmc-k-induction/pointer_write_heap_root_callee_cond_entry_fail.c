// As pointer_write_heap_root_reassigned_callee_fail, with g's loop also
// entered by a conditional jump into its body.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
int *A;
int F;
int *g(void)
{
  int *x = A;
  int i = 0;
  if (F)
    goto inside;
  while (i < 1)
  {
    x = x;
  inside:
    i++;
  }
  return x;
}
int main()
{
  A = malloc(sizeof(int));
  *A = 0;
  int n = 0;
  int *r;
  while (__VERIFIER_nondet_int())
  {
    r = g();
    __VERIFIER_assume(*A != 7);
    if (n >= 3)
      *r = 7;
    assert(*A != 7);
    n++;
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_callee_cond_entry_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
