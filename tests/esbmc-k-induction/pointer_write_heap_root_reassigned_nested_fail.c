// The inner loop havocs p at its head in the middle of an outer iteration,
// so the outer loop cannot trace p to its assignments in the body.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
int main()
{
  int *d = malloc(sizeof(int));
  int *e = malloc(sizeof(int));
  unsigned n = 0;
  while (n < 6)
  {
    int *p = d;
    *d = 0;
    int j = __VERIFIER_nondet_int();
    while (j > 0)
    {
      p = e;
      j = 0;
    }
    if (n == 3)
      *p = 10;
    assert(*d != 10);
    n++;
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_reassigned_nested_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
