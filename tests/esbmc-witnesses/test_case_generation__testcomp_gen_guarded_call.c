#include <assert.h>

extern int __VERIFIER_nondet_int(void);

int x;

int main()
{
  /* The property never reads x, but whether its call ran decides which call
   * receives which input, so the guard of that call must survive slicing. */
  int c = __VERIFIER_nondet_int();
  if (c)
    x = __VERIFIER_nondet_int();
  int d = __VERIFIER_nondet_int();
  if (c == 7 && d == 5)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_guarded_call
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>7</input>\n<input>-?[0-9]+</input>\n<input>5</input>\n</testcase>
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
