#include <assert.h>

int __VERIFIER_nondet_int();

// `a[i] = 5` writes `a[j]` when i == j, so `a[j] + 1` must be recomputed.
int main()
{
  int a[2] = {0, 0};
  int i = __VERIFIER_nondet_int() & 1;
  int j = __VERIFIER_nondet_int() & 1;
  __VERIFIER_assume(i == j);
  int x = a[j] + 1;
  a[i] = 5;
  int y = a[j] + 1;
  assert(y == 6);
}
// Imported from ESBMC regression/esbmc/gcse_index_alias_direct
// Needs: --gcse
// RUN: --gcse
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
