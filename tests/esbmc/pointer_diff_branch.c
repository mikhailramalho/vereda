// A pointer difference is a ptrdiff_t, including one the simplifier folds to
// a constant offset and merges with another value at a branch.
#include <assert.h>
#include <stddef.h>
_Bool __VERIFIER_nondet_bool(void);
long __VERIFIER_nondet_long(void);
int main(void)
{
  int a[4];
  long k = __VERIFIER_nondet_long();
  ptrdiff_t n = k;
  if (__VERIFIER_nondet_bool())
    n = (a + 3) - a;
  assert(n == 3 || n == k);
  return 0;
}
// Imported from ESBMC regression/esbmc/pointer_diff_branch
// EXPECT: ^VERIFICATION SUCCESSFUL$
