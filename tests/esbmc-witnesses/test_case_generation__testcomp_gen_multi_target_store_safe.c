#include <assert.h>

extern int __VERIFIER_nondet_int(void);

struct pair
{
  int a, b;
};

int main()
{
  struct pair p1, p2;
  struct pair *p = __VERIFIER_nondet_int() ? &p1 : &p2;
  /* One step per possible target of `p`, all sharing the nondet. The `p1`
   * step comes first and is off the trace (#10315). */
  if (p == &p2)
    p->b = __VERIFIER_nondet_int() & 1;
  if (p == &p2 && p2.b == 7)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_multi_target_store_safe
// Needs: --generate-testcase
// RUN: --generate-testcase
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
