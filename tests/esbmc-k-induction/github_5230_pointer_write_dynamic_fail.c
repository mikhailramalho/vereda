// Issue #5230 (k-induction Phase 2) soundness guard: an UNSAFE loop that
// writes nondet array elements through a pointer to a heap object. Because
// Phase 2 abstains on dynamic (heap) pointees (it cannot havoc them as named
// symbols), the inductive step is disabled and the base case finds the
// genuinely reachable violation — VERIFICATION FAILED. If Phase 2 ever
// wrongly resolved the heap object, the inductive step could close on a
// too-strong hypothesis and report a spurious SUCCESSFUL (the #5027 / #5224
// unsoundness class); this test pins that it does not.
#include <stdlib.h>
extern unsigned char __VERIFIER_nondet_uchar(void);

int main(void)
{
  unsigned char (*dest)[8] = malloc(8);
  __VERIFIER_assume(dest != 0);

  for (int i = 0; i < 8; i++)
    (*dest)[i] = __VERIFIER_nondet_uchar();

  __VERIFIER_assert((*dest)[7] != 0x2A); // (*dest)[7] is nondet, so 0x2A is reachable
  return 0;
}
// Imported from ESBMC regression/k-induction/github_5230_pointer_write_dynamic_fail
// Needs: --k-induction --max-inductive-step 3
// RUN: --k-induction --max-inductive-step 3 --force-malloc-success --no-bounds-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: cannot havoc what a loop writes through a pointer
// Vereda: verdict none
