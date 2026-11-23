// The first loop moves s to b and back, which is no offset of s, so its havoc
// leaves s unconstrained. The second loop's writes through s then reach
// nothing in the inductive step, which must not prove a[0] < 3.
#include <stdlib.h>
extern void abort(void);
extern int __VERIFIER_nondet_int(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int main()
{
  unsigned char *a = malloc(4);
  unsigned char *b = malloc(4);
  a[0] = 0;
  unsigned char *s = a;
  while (__VERIFIER_nondet_int())
  {
    s = b;
    s = a;
  }
  for (;;)
  {
    *s = *s + 1;
    __VERIFIER_assert(a[0] < 3);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_moving_offset_reset_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --no-pointer-check --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
