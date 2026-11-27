// The loop increments h's object through p, which holds what id returns in a
// goto_convert temporary. The inductive step must havoc that object, so it
// cannot prove *h < 4, and the base case finds the bug.
#include <stdlib.h>
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int *id(int *q) { return q; }

int main()
{
  int *h = malloc(sizeof(int));
  *h = 0;
  for (;;)
  {
    int *p = id(h);
    *p = *p + 1;
    __VERIFIER_assert(*h < 4);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap_root_temporary_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
