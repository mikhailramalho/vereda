// memcpy writes x through a bodyless intrinsic the loop analysis must see.
#include <string.h>
extern void abort(void);
void reach_error() {}
void __VERIFIER_assert(int c) { if (!c) { ERROR: { reach_error(); abort(); } } }

int main()
{
  int x = 0, i = 0;
  for (;;)
  {
    int y = x + 1;
    memcpy(&x, &y, sizeof x);
    i++;
    __VERIFIER_assert(x < 10);
  }
}
// Imported from ESBMC regression/k-induction/pointer_write_memcpy_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 12 --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
