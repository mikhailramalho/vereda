#include <assert.h>

int __VERIFIER_nondet_int();

// Reusing the stale `a[j] + 1` made y equal x.
int main()
{
  int a[2] = {0, 0};
  int i = __VERIFIER_nondet_int() & 1;
  int j = __VERIFIER_nondet_int() & 1;
  __VERIFIER_assume(i == j);
  int x = a[j] + 1;
  a[i] = 5;
  int y = a[j] + 1;
  assert(y == x);
}
// Imported from ESBMC regression/esbmc/gcse_index_alias_direct_fail
// EXPECT: ^VERIFICATION FAILED$
