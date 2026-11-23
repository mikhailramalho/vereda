#include <assert.h>

extern int __VERIFIER_nondet_int(void);

struct pair
{
  int a, b;
};

int main()
{
  struct pair s;
  int c = __VERIFIER_nondet_int();
  /* The merge after this branch reads the untaken branch's nondet, which is
   * not an input. */
  if (c)
    s.a = __VERIFIER_nondet_int();
  else
    s.b = __VERIFIER_nondet_int() & 1;
  int d = __VERIFIER_nondet_int();
  if (!c && s.b == 4 && d == 5)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_untaken_branch_store_safe
// Needs: --generate-testcase
// RUN: --generate-testcase
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
