#include <assert.h>

extern int __VERIFIER_nondet_int(void);

struct pair
{
  int a, b;
};

struct pair g1, g2;

int main()
{
  struct pair *p = __VERIFIER_nondet_int() ? &g1 : &g2;
  p->b = __VERIFIER_nondet_int() & 1;
  if (p == &g1 && g1.b == 3)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_multi_target_global_safe
// Needs: --generate-testcase
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
