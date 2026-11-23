#include <assert.h>

int __VERIFIER_nondet_int();

// Reusing the stale `*p + 1` made b equal a.
int main()
{
  int x = __VERIFIER_nondet_int();
  int *p = &x;
  int a = *p + 1;
  *p = 3;
  int b = *p + 1;
  assert(b == a);
}
// Imported from ESBMC regression/esbmc/gcse_deref_store_kill_fail
// Needs: --gcse
// RUN: --gcse
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
