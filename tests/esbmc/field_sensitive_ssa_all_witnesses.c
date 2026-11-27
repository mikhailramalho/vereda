#include <assert.h>

struct S
{
  int a, b;
};

int __VERIFIER_nondet_int();

int main()
{
  struct S s;
  s.a = __VERIFIER_nondet_int();
  s.b = 2;
  assert(s.a != 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/field_sensitive_ssa_all_witnesses
// Needs: --all-witnesses
// RUN: --all-witnesses
// MODE: FUTURE
// EXPECT: ^WARNING: --field-sensitive-ssa is ignored with --all-witnesses$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
