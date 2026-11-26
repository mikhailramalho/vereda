#include <assert.h>

struct S
{
  int a;
  int b;
};

int main(void)
{
  int v = __VERIFIER_nondet_int() | 1;
  struct S s = {v, __VERIFIER_nondet_int()};
  assert(s.b != 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_print_sliced_rhs_fail
// RUN:
// EXPECT: \A(?![\s\S]*\n  s = )[\s\S]*\Z
// EXPECT: ^VERIFICATION FAILED$
