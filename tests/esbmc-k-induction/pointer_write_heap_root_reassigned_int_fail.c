// As pointer_write_heap_root_int_offset_fail, through a pointer the loop
// reassigns before writing through it.
#include <stdlib.h>
#include <assert.h>
extern int __VERIFIER_nondet_int(void);
int main()
{
  int *A = malloc(sizeof(int));
  int *B = malloc(sizeof(int));
  *A = 0;
  *B = 0;
  long off = (long)B - (long)A;
  int *r;
  while (__VERIFIER_nondet_int())
  {
    r = (int *)((long)A + off);
    *r = *r + 1;
    assert(*B < 3);
  }
  return 0;
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_reassigned_int_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
