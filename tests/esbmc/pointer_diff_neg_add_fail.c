// a - (a + j) folds to -j in ptrdiff_t, negated after widening.
#include <assert.h>
#include <stddef.h>
_Bool __VERIFIER_nondet_bool(void);
long __VERIFIER_nondet_long(void);
unsigned __VERIFIER_nondet_uint(void);
int main(void)
{
  int a[8];
  int *p = &a[4];
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(j < 8);
  long k = __VERIFIER_nondet_long();
  ptrdiff_t n = k;
  if (__VERIFIER_nondet_bool())
    n = a - (a + j);
  assert(n != -(long)j);
  return 0;
}
// Imported from ESBMC regression/esbmc/pointer_diff_neg_add_fail
// EXPECT: ^VERIFICATION FAILED$
