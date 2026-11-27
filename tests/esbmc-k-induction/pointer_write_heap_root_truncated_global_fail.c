// As pointer_write_heap_root_truncated_fail, with the truncated address in a
// global another function assigns.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
unsigned G;
int *P;
void set(void) { G = (unsigned)(unsigned long)P; }
int main()
{
  P = malloc(sizeof(int));
  *P = 0;
  set();
  int *r;
  while (__VERIFIER_nondet_int())
  {
    r = (int *)(unsigned long)G;
    *r = *r + 1;
    assert(*P < 3);
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_truncated_global_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
