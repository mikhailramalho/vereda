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
    p->b = __VERIFIER_nondet_int();
  if (p == &p2 && p2.b == 7)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_multi_target_store
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>0</input>\n<input>7</input>\n</testcase>
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
