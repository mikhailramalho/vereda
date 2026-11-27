// off is a difference of two addresses, and symex follows A + off into B.
// Taking off as a root would havoc only A's object and prove *B < 3.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
void w(int *p) { *p = *p + 1; }
int main()
{
  int *A = malloc(sizeof(int));
  int *B = malloc(sizeof(int));
  *A = 0;
  *B = 0;
  long off = (long)B - (long)A;
  while (__VERIFIER_nondet_int())
  {
    w((int *)((long)A + off));
    assert(*B < 3);
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_int_offset_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
