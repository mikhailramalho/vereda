#include <assert.h>
#include <time.h>

extern int __VERIFIER_nondet_int(void);

struct timespec ts;

int main()
{
  /* timespec_get's model sets ts.tv_nsec from a nondet of its own, which
   * then sits in the literal `ts == { .tv_sec=5, .tv_nsec=nondet1 }`. It is
   * not a harness input: the only one is d. */
  timespec_get(&ts, TIME_UTC);
  ts.tv_sec = 5;
  int d = __VERIFIER_nondet_int();
  if (d == 22)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/testcomp_gen_model_nondet_in_literal
// Needs: --generate-testcase CHECK_FILE testcase.xml contains <testcase coversError="true">\n<input>22</input>\n</testcase>
// RUN: --generate-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
