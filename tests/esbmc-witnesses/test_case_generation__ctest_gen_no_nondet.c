#include <assert.h>

int main()
{
  int x = 1;
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/witnesses/test_case_generation/ctest_gen_no_nondet
// Needs: --generate-ctest-testcase
// RUN: --generate-ctest-testcase --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^WARNING: No nondet variables found\. No CTest test case generated\.$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
