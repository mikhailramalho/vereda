#include <assert.h>
double __VERIFIER_nondet_double(void);
int __VERIFIER_nondet_int(void);
int main(void)
{
  double x = __VERIFIER_nondet_double();
  __VERIFIER_assume(x == 3.75);
  assert((int)x == 3);
  assert((float)x == 3.75f);
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == -7);
  assert((double)i == -7.0);
  unsigned u = 42u;
  assert((double)u == 42.0);
  return 0;
}
// Imported from ESBMC regression/bitwuzla/native_fp_conv
// RUN: --solver bitwuzla
// EXPECT: ^VERIFICATION SUCCESSFUL$
