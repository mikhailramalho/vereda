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
  p->b = __VERIFIER_nondet_int();
  if (p == &g1 && g1.b == 3)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_multi_target_global
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>-?[1-9][0-9]*</input>\n<input>3</input>\n</testcase>
// RUN: --generate-testcase
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
