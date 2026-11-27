#include <assert.h>
struct S
{
  int a[4];
  int b[4];
} s;
int main(void)
{
  int *end = s.a + 4;
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k >= 1 && k <= 4);
  *(end - k) = 7;
  assert(s.a[4 - k] != 7);
  return 0;
}
// Imported from ESBMC regression/esbmc/array_member_one_past_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
