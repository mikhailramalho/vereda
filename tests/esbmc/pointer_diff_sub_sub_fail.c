// p - (p - j) folds to j in ptrdiff_t.
#include <assert.h>
#include <stddef.h>
_Bool __VERIFIER_nondet_bool(void);
long __VERIFIER_nondet_long(void);
int __VERIFIER_nondet_int(void);
int main(void)
{
  int a[8];
  int *p = &a[4];
  int j = __VERIFIER_nondet_int();
  __VERIFIER_assume(j >= -3 && j <= 4);
  long k = __VERIFIER_nondet_long();
  ptrdiff_t n = k;
  if (__VERIFIER_nondet_bool())
    n = p - (p - j);
  assert(n != j);
  return 0;
}
// Imported from ESBMC regression/esbmc/pointer_diff_sub_sub_fail
// EXPECT: ^VERIFICATION FAILED$
