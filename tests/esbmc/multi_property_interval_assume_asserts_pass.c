/* The twin of multi_property_interval_assume_asserts: both assertions hold,
   so the run must still end VERIFICATION SUCCESSFUL (esbmc/esbmc#7900). */
#include <assert.h>
int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 10);
  assert(x > 10);
  assert(x > 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/multi_property_interval_assume_asserts_pass
// Needs: --interval-analysis --interval-analysis-assume-asserts
// RUN: --interval-analysis --interval-analysis-assume-asserts --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*VERIFICATION FAILED)[\s\S]*\Z
// Vereda: verdict none
