// The loop writes a heap object through p, which it never moves, so the
// inductive step havocs *p and still proves the property.
#include <stdlib.h>
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int main()
{
  int *p = malloc(sizeof(int));
  *p = 0;
  for (;;)
  {
    *p = (*p + 1) % 10;
    __VERIFIER_assert(*p < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_heap
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false --no-bounds-check --force-malloc-success
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
